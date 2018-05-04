#include "container.h"

#include <Zend/zend_closures.h>
#include <Zend/zend_exceptions.h>
#include <Zend/zend_interfaces.h>
#include <ext/spl/spl_exceptions.h>
#include "exception/containerexception.h"
#include "exception/containervaluenotfoundexception.h"
#include "internal/closure.h"
#include "internal/container.h"
#include "interfaces.h"
#include "persistent.h"
#include "psr11.h"
#include "utils.h"

zend_class_entry* ce_TurboSlim_Container = NULL;

static zend_object_handlers container_handlers;
static zend_function invoker;

typedef struct container {
    HashTable items;
    zval* gc;
    int gc_num;
    zend_object std;
} container_t;

typedef enum definition_type {
    def_parameter = 0, /* Parameter (string, integer,...); value.z will be returned */
    def_resolved  = 1, /* Resolved shared service; value.z will be returned */
    def_shared    = 2, /* Shared service; z.c() will be called once, type will be set to def_resolved, and z.val set to the result of z.c() */
    def_factory   = 3, /* Factory method; z.c() will be called every time */
    def_callable  = 4  /* "Protected" callable; z.c() will be called once, type will be set to def_resolved,... */
} definition_type_t;

typedef struct call_info {
    zend_fcall_info fci;
    zend_fcall_info_cache fcc;
} call_info_t;

typedef struct definition {
    int type; // parameter, service, factory, callable
    zval z;
    zval callable;
    call_info_t* cache;
} definition_t;

static void definition_dtor(zval* z)
{
    assert(Z_TYPE_P(z) == IS_PTR); /* LCOV_EXCL_BR_LINE */

    definition_t* p = (definition_t*)Z_PTR_P(z);

    zval_ptr_dtor(&p->z);
    zval_ptr_dtor(&p->callable);
    if (p->cache) {
        efree(p->cache);
    }

    efree(p);
    ZVAL_UNDEF(z);
}

static inline container_t* container_from_zobj(const zend_object* obj)
{
    return (container_t*)((char*)(obj) - XtOffsetOf(container_t, std));
}

static void container_free_obj(zend_object* obj)
{
    container_t* v = container_from_zobj(obj);

    zend_object_std_dtor(&v->std);
    zend_hash_destroy(&v->items);
    if (v->gc) {
        efree(v->gc);
        v->gc_num = 0;
    }
}

static HashTable* container_get_gc(zval* obj, zval** table, int* n)
{
    container_t* v = container_from_zobj(Z_OBJ_P(obj));
    if (v->items.nNumOfElements > 2 * v->gc_num) {
        v->gc_num = 2 * v->items.nNumOfElements;
        v->gc     = (zval*)erealloc(v->gc, sizeof(zval) * v->gc_num);
    }

    int i = 0;
    definition_t* el;
    ZEND_HASH_FOREACH_PTR(&v->items, el) {
        ZVAL_COPY_VALUE(&v->gc[i], &el->callable); ++i;
        ZVAL_COPY_VALUE(&v->gc[i], &el->z); ++i;
    } ZEND_HASH_FOREACH_END();

    *table = v->gc;
    *n     = i;

    return std_object_handlers.get_properties(obj);
}

static definition_t* set_item(container_t* c, zval* key, zval* value)
{
    definition_t* d = emalloc(sizeof(definition_t));
    ZVAL_UNDEF(&d->callable);
    ZVAL_UNDEF(&d->z);
    if (Z_TYPE_P(value) == IS_OBJECT) {
        zend_class_entry* tmp_ce;
        zend_function* tmp_func;
        zend_object* tmp_obj;

        zend_class_entry* ce = Z_OBJCE_P(value);
        if (instanceof_function(ce, zend_ce_closure)) {                     /* Fast path - explicitly check for Closure */
            d->type = def_shared;
            ZVAL_COPY(&d->callable, value);
        }
        else if (ce == ce_TurboSlim_Internal_Container_FactoryCallable) {   /* FactoryCallable is final, it is OK to compare directly */
            d->type = def_factory;
            turboslim_container_getCallableFromWrapper(&d->callable, value);
        }
        else if (ce == ce_TurboSlim_Internal_Container_ProtectedCallable) { /* same */
            d->type = def_callable;
            turboslim_container_getCallableFromWrapper(&d->callable, value);
        }
        else if (ce == ce_TurboSlim_Internal_Container_ServiceCallable) {   /* same */
            d->type = def_shared;
            turboslim_container_getCallableFromWrapper(&d->callable, value);
        }
        else if (Z_OBJ_HANDLER_P(value, get_closure) && FAILURE != Z_OBJ_HANDLER_P(value, get_closure)(value, &tmp_ce, &tmp_func, &tmp_obj)) {
            d->type = def_shared;
            ZVAL_COPY(&d->callable, value);
        }
        else {
            d->type = def_parameter;
            ZVAL_COPY(&d->callable, value);
        }
    }
    else {
        d->type = def_parameter;
        ZVAL_COPY(&d->callable, value);
    }

    ZVAL_UNDEF(&d->z);
    d->cache = NULL;

    zval tmp;
    ZVAL_PTR(&tmp, d);
    if (!array_zval_offset_set(&c->items, key, &tmp)) { /* LCOV_EXCL_BR_LINE */
        definition_dtor(&tmp);                          /* LCOV_EXCL_LINE */
        return NULL;                                    /* LCOV_EXCL_LINE */
    }

    return d;
}

static zval* get_item(container_t* c, zval* this_ptr, zval* key, int type, zval* rv)
{
    zval znull;
    zval* z;
    if (key) {
        z = array_zval_offset_get(&c->items, key);
    }
    else {
        z = NULL;
        ZVAL_NULL(&znull);
        key = &znull;
    }

    if (!z) {
        switch (type) {
            case BP_VAR_R:
            case BP_VAR_RW:
            default:
                zend_throw_exception_ex(ce_TurboSlim_Exception_ContainerValueNotFoundException, 0, "\"%Z\" does not exist", key);
                /* no break */

            case BP_VAR_IS:
                return &EG(uninitialized_zval);

            case BP_VAR_UNSET:
            case BP_VAR_W: {
                ZVAL_NULL(rv);
                definition_t* d = set_item(c, key, rv);
                if (UNEXPECTED(!d)) {
                    return &EG(uninitialized_zval);
                }

                assert(d->type == def_parameter); /* LCOV_EXCL_BR_LINE */
                rv = &d->callable;
                ZVAL_NEW_REF(rv, rv);
                return rv;
            }
        }

        /* unreachable */
    }

    assert(Z_TYPE_P(z) == IS_PTR); /* LCOV_EXCL_BR_LINE */
    definition_t* d = (definition_t*)Z_PTR_P(z);
    zval* retval;
    switch (d->type) {
        case def_parameter:
        case def_callable:
            retval = &d->callable;
            break;

        case def_resolved:
            retval = &d->z;
            break;

        case def_shared: {
            zend_fcall_info fci;
            zend_fcall_info_cache fcc;
            int rc;
            char* error;

            if (UNEXPECTED(FAILURE == zend_fcall_info_init(&d->callable, IS_CALLABLE_STRICT, &fci, &fcc, NULL, &error))) {
                zend_throw_exception_ex(ce_TurboSlim_Exception_ContainerException, 0, "Unable to call service with \"%Z\": %s", key, error);
                efree(error);
                return &EG(uninitialized_zval);
            }

            if (UNEXPECTED(error != NULL)) {
                zend_error(E_DEPRECATED, "%s", error);
                efree(error);

                if (UNEXPECTED(EG(exception))) {
                    return &EG(uninitialized_zval);
                }
            }

            zend_fcall_info_argn(&fci, 1, this_ptr);
            rc = zend_fcall_info_call(&fci, &fcc, &d->z, NULL);
            zend_fcall_info_args_clear(&fci, 1);
            if (UNEXPECTED(FAILURE == rc || EG(exception))) {
                return &EG(uninitialized_zval);
            }

            d->type = def_resolved;
            retval  = &d->z;
            break;
        }

        case def_factory: {
            if (!d->cache) {
                char* error;
                d->cache = ecalloc(1, sizeof(call_info_t));
                if (UNEXPECTED(FAILURE == zend_fcall_info_init(&d->callable, IS_CALLABLE_STRICT, &d->cache->fci, &d->cache->fcc, NULL, &error))) {
                    zend_throw_exception_ex(ce_TurboSlim_Exception_ContainerException, 0, "Unable to call factory service with \"%Z\": %s", key, error);
                    efree(error);
                    return &EG(uninitialized_zval);
                }

                if (UNEXPECTED(error != NULL)) {
                    zend_error(E_DEPRECATED, "%s", error);
                    efree(error);
                    if (UNEXPECTED(EG(exception))) {
                        return &EG(uninitialized_zval);
                    }
                }
            }

            zend_fcall_info_argn(&d->cache->fci, 1, this_ptr);
            zend_fcall_info_call(&d->cache->fci, &d->cache->fcc, rv, NULL);
            zend_fcall_info_args_clear(&d->cache->fci, 1);
            retval = rv;
            break;
        }

        /* LCOV_EXCL_BR_START LCOV_EXCL_START */
        default:
            zend_error_noreturn(E_ERROR, "Bad container data");
            return &EG(uninitialized_zval); /* not reachable */
        /* LCOV_EXCL_BR_STOP LCOV_EXCL_STOP */
    }

    /* not reachable */
    if ((type == BP_VAR_W || type == BP_VAR_RW || type == BP_VAR_UNSET) && !Z_ISREF_P(retval)) {
        ZVAL_NEW_REF(retval, retval);
    }

    return retval;
}

static zval* read_dimension(zval* object, zval* offset, int type, zval *rv)
{
    zend_object* zobj = Z_OBJ_P(object);
    container_t* v    = container_from_zobj(zobj);
    return get_item(v, object, offset, type, rv);
}

static void write_dimension(zval* object, zval* offset, zval* value)
{
    zval tmp;
    zend_object* zobj = Z_OBJ_P(object);
    container_t* v    = container_from_zobj(zobj);

    if (UNEXPECTED(offset == NULL)) {
        ZVAL_NULL(&tmp);
        offset = &tmp;
    }

    set_item(v, offset, value);
}

static void unset_dimension(zval* object, zval* offset)
{
    zend_object* zobj = Z_OBJ_P(object);
    container_t* v    = container_from_zobj(zobj);

    array_zval_offset_unset(&v->items, offset);
}

static int has_dimension(zval* object, zval* member, int check_empty)
{
    zend_object* zobj = Z_OBJ_P(object);
    container_t* v    = container_from_zobj(zobj);
    zval* result      = array_zval_offset_get(&v->items, member);

    if (check_empty && result) {
        return i_zend_is_true(result);
    }

    return result != NULL;
}

static void handle_inheritance(zend_class_entry* ce)
{
    if (ce != ce_TurboSlim_Container) {
        zend_function* f1;
        zend_function* f2;

        f1 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("offsetget"));
        f2 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("get"));
        if (f1->common.scope == ce_TurboSlim_Container && f2->common.scope == ce_TurboSlim_Container) {
            container_handlers.read_dimension  = read_dimension;
        }

        f1 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("offsetset"));
        if (f1->common.scope == ce_TurboSlim_Container) {
            container_handlers.write_dimension = write_dimension;
        }

        f1 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("offsetunset"));
        if (f1->common.scope == ce_TurboSlim_Container) {
            container_handlers.unset_dimension = unset_dimension;
        }

        f1 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("offsetexists"));
        f2 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("has"));
        if (f1->common.scope == ce_TurboSlim_Container && f2->common.scope == ce_TurboSlim_Container) {
            container_handlers.has_dimension   = has_dimension;
        }
    }
    else {
        /* ArrayAccess */
        container_handlers.read_dimension  = read_dimension;
        container_handlers.write_dimension = write_dimension;
        container_handlers.unset_dimension = unset_dimension;
        container_handlers.has_dimension   = has_dimension;
    }
}

static zend_object* container_create_object(zend_class_entry* ce)
{
    container_t* v = ecalloc(1, sizeof(container_t) + zend_object_properties_size(ce));

    zend_object_std_init(&v->std, ce);
    object_properties_init(&v->std, ce);
    v->std.handlers = &container_handlers;
    handle_inheritance(ce);

    zend_hash_init(&v->items, 0, NULL, definition_dtor, 0);
    v->gc     = NULL;
    v->gc_num = 0;

    return &v->std;
}

static PHP_METHOD(TurboSlim_Container, __construct)
{
    HashTable* values = NULL;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY_HT_EX(values, 1, 0);
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    if (values && zend_hash_num_elements(values) > 0) {
        zval* this_ptr = get_this(execute_data);
        container_t* c = container_from_zobj(Z_OBJ_P(this_ptr));

        zend_ulong h;
        zend_string* key;
        zval* val;
        ZEND_HASH_FOREACH_KEY_VAL(values, h, key, val)
            zval k;
            if (!key) {
                ZVAL_LONG(&k, h);
            }
            else {
                ZVAL_NEW_STR(&k, key);
            }

            set_item(c, &k, val);
        ZEND_HASH_FOREACH_END();
    }
}

static PHP_METHOD(TurboSlim_Container, offsetSet)
{
    zval* key;
    zval* value;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_ZVAL_EX(key, 1, 0);
        Z_PARAM_ZVAL(value);
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    container_t* c = container_from_zobj(Z_OBJ_P(this_ptr));
    set_item(c, key, value);
}

static PHP_METHOD(TurboSlim_Container, offsetGet)
{
    zval* key;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL_EX(key, 1, 0);
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    container_t* c = container_from_zobj(Z_OBJ_P(this_ptr));
    zval* res      = get_item(c, this_ptr, key, key ? BP_VAR_RW : BP_VAR_W, return_value);
    if (UNEXPECTED(EG(exception))) {
        return;
    }

    if (res != return_value) {
        ZVAL_COPY(return_value, res);
    }
}

static PHP_METHOD(TurboSlim_Container, offsetUnset)
{
    zval* key;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(key);
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    container_t* c = container_from_zobj(Z_OBJ_P(this_ptr));
    array_zval_offset_unset(&c->items, key);
}

static PHP_METHOD(TurboSlim_Container, offsetExists)
{
    zval* key;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(key);
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    container_t* c = container_from_zobj(Z_OBJ_P(this_ptr));
    RETURN_BOOL(array_zval_offset_exists(&c->items, key));
}

static void wrap_callable(INTERNAL_FUNCTION_PARAMETERS, zend_class_entry* ce)
{
    zval* callable;
    zend_fcall_info fci;
    zend_fcall_info_cache fcc;
    char* error;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(callable);
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    if (UNEXPECTED(zend_fcall_info_init(callable, 0, &fci, &fcc, NULL, &error) != SUCCESS)) {
        wrong_callback_error(E_WARNING, 1, error);    /* will efree(error) */
        return;
    }

    if (UNEXPECTED(error != NULL)) {
        wrong_callback_error(E_DEPRECATED, 1, error); /* will efree(error) */
    }

    turboslim_container_createCallableWrapper(return_value, ce, callable);
}

static PHP_METHOD(TurboSlim_Container, factory)
{
    wrap_callable(INTERNAL_FUNCTION_PARAM_PASSTHRU, ce_TurboSlim_Internal_Container_FactoryCallable);
}

static PHP_METHOD(TurboSlim_Container, protect)
{
    wrap_callable(INTERNAL_FUNCTION_PARAM_PASSTHRU, ce_TurboSlim_Internal_Container_ProtectedCallable);
}

static PHP_METHOD(TurboSlim_Container, sharedService)
{
    wrap_callable(INTERNAL_FUNCTION_PARAM_PASSTHRU, ce_TurboSlim_Internal_Container_ServiceCallable);
}

static PHP_METHOD(TurboSlim_Container, raw)
{
    zval* key;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(key);
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    container_t* c = container_from_zobj(Z_OBJ_P(this_ptr));

    zval* value = array_zval_offset_get(&c->items, key);
    if (value) {
        assert(Z_TYPE_P(value) == IS_PTR); /* LCOV_EXCL_BR_LINE */

        definition_t* d = (definition_t*)Z_PTR_P(value);
        RETURN_ZVAL(&d->callable, 1, 0);
    }

    zend_throw_exception_ex(ce_TurboSlim_Exception_ContainerValueNotFoundException, 0, "\"%Z\" does not exist", key);
}

static PHP_METHOD(TurboSlim_Container, keys)
{
    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    container_t* c = container_from_zobj(Z_OBJ_P(this_ptr));

    int n = zend_hash_num_elements(&c->items);
    array_init_size(return_value, n);
    if (!n) {
        return;
    }

    zend_hash_real_init(Z_ARRVAL_P(return_value), 1);
    ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
        zend_ulong num_idx;
        zend_string *str_idx;
        zval item;

        ZEND_HASH_FOREACH_KEY(&c->items, num_idx, str_idx) {
            if (str_idx) {
                ZVAL_STR_COPY(&item, str_idx);
            }
            else {
                ZVAL_LONG(&item, num_idx);
            }

            ZEND_HASH_FILL_ADD(&item);
        } ZEND_HASH_FOREACH_END();
    } ZEND_HASH_FILL_END();
}

static PHP_METHOD(TurboSlim_Container, extend)
{
    zval* id;
    zend_fcall_info fci;
    zend_fcall_info_cache fcc;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_ZVAL(id)
        Z_PARAM_FUNC(fci, fcc)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    container_t* c = container_from_zobj(Z_OBJ_P(this_ptr));

    zval* value = array_zval_offset_get(&c->items, id);
    if (value) {
        assert(Z_TYPE_P(value) == IS_PTR); /* LCOV_EXCL_BR_LINE */

        definition_t* d = (definition_t*)Z_PTR_P(value);

        switch (d->type) {
            case def_parameter:
                zend_throw_exception_ex(ce_TurboSlim_Exception_ContainerException, 0, "\"%Z\" is not an object definition or callable", id);
                return;

            case def_callable:
                zend_throw_exception_ex(ce_TurboSlim_Exception_ContainerException, 0, "Unable to extend a protected item (\"%Z\")", id);
                return;

            case def_resolved:
                zend_throw_exception_ex(ce_TurboSlim_Exception_ContainerException, 0, "Unable to extend a shared service (\"%Z\")", id);
                return;

            default: {
                HashTable bound;
                zend_hash_init(&bound, 1, NULL, ZVAL_PTR_DTOR, 0);
                zend_hash_real_init(&bound, 1);
                ZEND_HASH_FILL_PACKED(&bound) {
                    /* Z_TRY_ADDREF(d->callable); */ /* do not add a reference here to avoid calling zval_ptr_dtor() later */
                    ZEND_HASH_FILL_ADD(&d->callable);
                } ZEND_HASH_FILL_END();

                turboslim_closure_create(return_value, &invoker, &fci.function_name, &fci, &fcc, &bound);
                zend_hash_destroy(&bound);
                /* zval_ptr_dtor(&d->callable); */ /* see above */
                ZVAL_COPY(&d->callable, return_value);
                if (d->cache) {
                    efree(d->cache);
                    d->cache = NULL;
                }

                return;
            }
        }
    }

    zend_throw_exception_ex(ce_TurboSlim_Exception_ContainerValueNotFoundException, 0, "\"%Z\" does not exist", id);
}

/*
 *  $invoker = function(Container $c) use (Callable $extender, Callable $orig_callable) {
 *      return $extender($orig_callable($c), $c);
 *  };
 */
static PHP_FUNCTION(invoker)
{
    zval* container;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(container, ce_TurboSlim_Container)
    ZEND_PARSE_PARAMETERS_END();

    zval* this_ptr   = get_this(execute_data);
    HashTable* bound = turboslim_closure_get_bound(this_ptr);
    if (EXPECTED(bound != NULL)) {
        zend_fcall_info* fci;
        zend_fcall_info_cache* fcc;

        /* extender will be NULL, as we don't pass a zval to turboslim_closure_create(); but fcc and fci will be OK */
        /* zval* extender =*/ turboslim_closure_get_callable(this_ptr, &fci, &fcc);
        zval* orig_callable = zend_hash_index_find(bound, 0);

        if (EXPECTED(orig_callable != NULL)) {
            zend_fcall_info fci_oc;
            zend_fcall_info_cache fcc_oc;
            char* error;
            zval tmp;

            /* $tmp = $orig_callable($c); */
            if (UNEXPECTED(FAILURE == zend_fcall_info_init(orig_callable, IS_CALLABLE_CHECK_SILENT, &fci_oc, &fcc_oc, NULL, &error))) {
                zend_throw_exception_ex(spl_ce_LogicException, 0, "Unable to call function: %s", error);
                return;
            }
            else if (UNEXPECTED(error != NULL)) {
                zend_error(E_DEPRECATED, "%s", error);
                efree(error);
            }

            zend_fcall_info_argn(&fci_oc, 1, container);
            zend_fcall_info_call(&fci_oc, &fcc_oc, &tmp, NULL);
            zend_fcall_info_args_clear(&fci_oc, 1);

            if (UNEXPECTED(EG(exception))) {
                return;
            }
            /**/

            /* $extender($tmp, $c) */
            zend_fcall_info_argn(fci, 2, &tmp, container);
            zend_fcall_info_call(fci, fcc, return_value, NULL);
            zend_fcall_info_args_clear(fci, 1);
            /**/

            return;
        }
    }

    zend_error(E_CORE_ERROR, "Impossible has just happened");
}

ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, ZEND_RETURN_VALUE, 0)
    ZEND_ARG_ARRAY_INFO(0, values, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_empty, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_kv, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_k, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_offsetGet, 0, ZEND_RETURN_REFERENCE, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_callable, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_TYPE_INFO(0, callable, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_extend, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, id)
    ZEND_ARG_TYPE_INFO(0, callable, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry fe_TurboSlim_Container[] = {
    ZEND_ME(TurboSlim_Container, __construct,   arginfo___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    ZEND_ME(TurboSlim_Container, offsetSet,     arginfo_kv,          ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Container, offsetGet,     arginfo_offsetGet,   ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Container, offsetUnset,   arginfo_k,           ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Container, offsetExists,  arginfo_k,           ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Container, factory,       arginfo_callable,    ZEND_ACC_PUBLIC | ZEND_ACC_ALLOW_STATIC)
    ZEND_ME(TurboSlim_Container, protect,       arginfo_callable,    ZEND_ACC_PUBLIC | ZEND_ACC_ALLOW_STATIC)
    ZEND_ME(TurboSlim_Container, sharedService, arginfo_callable,    ZEND_ACC_PUBLIC | ZEND_ACC_ALLOW_STATIC)
    ZEND_ME(TurboSlim_Container, raw,           arginfo_k,           ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Container, keys,          arginfo_empty,       ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Container, extend,        arginfo_extend,      ZEND_ACC_PUBLIC)

    PHP_MALIAS(TurboSlim_Container, has, offsetExists, arginfo_k, ZEND_ACC_PUBLIC)
    PHP_MALIAS(TurboSlim_Container, get, offsetGet,    arginfo_k, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};

int init_container()
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Container", fe_TurboSlim_Container);
    ce_TurboSlim_Container = zend_register_internal_class(&ce);

    ce_TurboSlim_Container->create_object = container_create_object;

    memcpy(&container_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    container_handlers.offset   = XtOffsetOf(container_t, std);
    container_handlers.free_obj = container_free_obj;
    container_handlers.get_gc   = container_get_gc;

    zend_class_implements(ce_TurboSlim_Container, 2, zend_ce_arrayaccess, ce_Psr_Container_ContainerInterface);

    memset(&invoker, 0, sizeof(invoker));
    invoker.type                                = ZEND_INTERNAL_FUNCTION;
    invoker.internal_function.fn_flags          = 0;
    invoker.internal_function.function_name     = str_magic_function;
    invoker.internal_function.scope             = ce_TurboSlim_Internal_Closure;
    invoker.internal_function.prototype         = NULL;
    invoker.internal_function.num_args          = 0;
    invoker.internal_function.required_num_args = 0;
    invoker.internal_function.arg_info          = NULL;
    invoker.internal_function.handler           = ZEND_FN(invoker);
    invoker.internal_function.module            = &turboslim_module_entry;

    return SUCCESS;
}
