#include "deferredcallable.h"

#include <Zend/zend_closures.h>
#include <Zend/zend_inheritance.h>
#include <Zend/zend_interfaces.h>
#include "traits/callableresolverawaretrait.h"
#include "psr11.h"
#include "utils.h"

#include <Zend/zend_compile.h>

zend_class_entry* ce_TurboSlim_DeferredCallable = NULL;
zend_object_handlers turboslim_deferredcallable_handlers;

typedef turboslim_deferredcallable_t deferred_callable_t;

static inline deferred_callable_t* dc_from_zobj(const zend_object* obj)
{
    return (deferred_callable_t*)((char*)(obj) - XtOffsetOf(deferred_callable_t, std));
}

zend_object* turboslim_deferredcallable_create_object(zend_class_entry* ce)
{
    deferred_callable_t* v = ecalloc(1, sizeof(deferred_callable_t) + zend_object_properties_size(ce));

    zend_object_std_init(&v->std, ce);
    object_properties_init(&v->std, ce);
    v->std.handlers = &turboslim_deferredcallable_handlers;

    ZVAL_UNDEF(&v->callable);
    ZVAL_NULL(&v->container);
    return &v->std;
}

void turboslim_deferredcallable_free_obj(zend_object* obj)
{
    deferred_callable_t* v = dc_from_zobj(obj);

    zend_object_std_dtor(&v->std);
    zval_ptr_dtor(&v->callable);
    zval_ptr_dtor(&v->container);
}

zend_object* turboslim_deferredcallable_clone_obj(zval* obj)
{
    zend_object* old_object     = Z_OBJ_P(obj);
    zend_object* new_object     = turboslim_deferredcallable_create_object(old_object->ce);
    deferred_callable_t* mine   = dc_from_zobj(old_object);
    deferred_callable_t* theirs = dc_from_zobj(new_object);

    ZVAL_COPY(&theirs->callable,  &mine->callable);
    ZVAL_COPY(&theirs->container, &mine->container);

    zend_objects_clone_members(new_object, old_object);
    return new_object;
}

/**
 * @todo There can be a different $container in a child
 */
zval* turboslim_deferredcallable_read_property(zval* object, zval* member, int type, void** cache_slot, zval* rv)
{
    if (Z_TYPE_P(member) == IS_STRING && zend_string_equals_literal(Z_STR_P(member), "container")) {
        zend_object* zobj = Z_OBJ_P(object);

        if (ce_TurboSlim_DeferredCallable == zobj->ce) {
            deferred_callable_t* v = dc_from_zobj(zobj);
            return &v->container;
        }

        if (type != BP_VAR_IS) {
            zend_throw_error(NULL, "Cannot access private property %s::$%s", ZSTR_VAL(ce_TurboSlim_DeferredCallable->name), Z_STRVAL_P(member));
        }

        return &EG(uninitialized_zval);
    }

    return zend_get_std_object_handlers()->read_property(object, member, type, cache_slot, rv);
}

HashTable* turboslim_deferredcallable_get_gc(zval* obj, zval** table, int* n)
{
    deferred_callable_t* v = dc_from_zobj(Z_OBJ_P(obj));

    /* callable and container are located together */
    *table = &v->callable;
    *n     = 2;

    return zend_std_get_properties(obj);
}

int turboslim_deferredcallable_compare_objects(zval* object1, zval* object2)
{
    zend_object* zobj1 = Z_OBJ_P(object1);
    zend_object* zobj2 = Z_OBJ_P(object2);

    if (zobj1->ce != zobj2->ce) {  /* LCOV_EXCL_BR_LINE */
        return 1;                  /* LCOV_EXCL_LINE - this cannot be tested without dirty hacks */
    }

    deferred_callable_t* v1 = dc_from_zobj(zobj1);
    deferred_callable_t* v2 = dc_from_zobj(zobj2);

    zval r;
    if (UNEXPECTED(FAILURE == compare_function(&r, &v1->callable, &v2->callable)) || Z_LVAL(r) != 0) {
        return 1;
    }

    if (UNEXPECTED(FAILURE == compare_function(&r, &v1->container, &v2->container)) || Z_LVAL(r) != 0) {
        return 1;
    }

    return zend_get_std_object_handlers()->compare_objects(object1, object2);
}

/*
 * public function __construct($callable, ContainerInterface $container = null)
 */
static PHP_METHOD(TurboSlim_DeferredCallable, __construct)
{
    zval* callable;
    zval* container = NULL;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ZVAL(callable)
        Z_PARAM_OPTIONAL
        Z_PARAM_OBJECT_OF_CLASS_EX(container, ce_Psr_Container_ContainerInterface, 1, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    deferred_callable_t* v = dc_from_zobj(Z_OBJ_P(this_ptr));

    ZVAL_COPY(&v->callable, callable);
    if (container) {
        ZVAL_COPY(&v->container, container);
    }
}

static PHP_METHOD(TurboSlim_DeferredCallable, __invoke)
{
    zval* argv = NULL;
    int argn;
    zval callable;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, -1)
        Z_PARAM_VARIADIC('*', argv, argn)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr         = get_this(execute_data);
    deferred_callable_t* v = dc_from_zobj(Z_OBJ_P(this_ptr));
    if (FAILURE == turboslim_CallableResolverAwareTrait_resolveCallable(&callable, this_ptr, &v->callable)) {
        return;
    }

    if (IS_OBJECT == Z_TYPE(callable) && instanceof_function(Z_OBJCE(callable), zend_ce_closure)) {
        zval new_callable;

        /* The code is equivalent for $callable = $callable->bindTo($this->container); */
#if PHP_VERSION_ID < 70114
        static zend_function* proxy = NULL;
        zend_call_method(&callable, zend_ce_closure, &proxy, ZEND_STRL("bindto"), &new_callable, 1, &v->container, NULL);
#else
        zend_function* func            = (zend_function*)zend_get_closure_method_def(&callable);
        zend_class_entry* called_scope = (IS_OBJECT == Z_TYPE(v->container)) ? Z_OBJCE(v->container) : func->common.scope;
        zend_class_entry* ce           = func->common.scope;
        zend_create_closure(&new_callable, func, ce, called_scope, &v->container);
#endif

        zval_ptr_dtor(&callable);
        ZVAL_COPY_VALUE(&callable, &new_callable);
    }

    zend_fcall_info fci;
    zend_fcall_info_cache fcc;
    char* error;
    if (UNEXPECTED(FAILURE == zend_fcall_info_init(&callable, IS_CALLABLE_STRICT, &fci, &fcc, NULL, &error))) {
        zval_ptr_dtor(&callable);
        zend_error(E_WARNING, "%s\n", error);
        efree(error);
        return;
    }

    if (UNEXPECTED(error != NULL)) {
        zend_error(E_DEPRECATED, "%s", error);
        efree(error);
        if (UNEXPECTED(EG(exception))) {
            zval_ptr_dtor(&callable);
            return;
        }
    }

    zend_fcall_info_argp(&fci, argn, argv);
    zend_fcall_info_call(&fci, &fcc, return_value, NULL);
    zend_fcall_info_args_clear(&fci, 1);
    zval_ptr_dtor(&callable);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, callable)
    ZEND_ARG_OBJ_INFO(0, container, Psr\\Container\\ContainerInterface, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_resolveCallable, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, callable)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo___invoke, 0, ZEND_RETURN_VALUE, 0)
    ZEND_ARG_VARIADIC_INFO(0, args)
ZEND_END_ARG_INFO()

const zend_function_entry fe_TurboSlim_DeferredCallable[] = {
    PHP_ME(TurboSlim_DeferredCallable, __construct, arginfo___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    ZEND_NAMED_ME(resolveCallable, ZEND_MN(TurboSlim_CallableResolverAwareTrait_resolveCallable), arginfo_resolveCallable, ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_DeferredCallable, __invoke, arginfo___invoke, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
