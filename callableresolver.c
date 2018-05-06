#include "callableresolver.h"

#include <Zend/zend_exceptions.h>
#include <Zend/zend_interfaces.h>
#include <Zend/zend_operators.h>
#include <Zend/zend_smart_str.h>
#include <ext/json/php_json.h>
#include <ext/pcre/php_pcre.h>
#include <ext/spl/spl_exceptions.h>
#include "interfaces.h"
#include "persistent.h"
#include "psr11.h"
#include "utils.h"

zend_class_entry* ce_TurboSlim_CallableResolver = NULL;

typedef struct callable_resolver {
    zval container;
    zend_object std;
} callable_resolver_t;

static zend_object_handlers handlers;

static inline callable_resolver_t* cr_from_zobj(const zend_object* obj)
{
    return (callable_resolver_t*)((char*)(obj) - XtOffsetOf(callable_resolver_t, std));
}

static zend_object* cr_create_object(zend_class_entry* ce)
{
    callable_resolver_t* v = ecalloc(1, sizeof(callable_resolver_t) + zend_object_properties_size(ce));

    zend_object_std_init(&v->std, ce);
    object_properties_init(&v->std, ce);
    v->std.handlers = &handlers;

    ZVAL_NULL(&v->container);
    return &v->std;
}

static void free_obj(zend_object* obj)
{
    callable_resolver_t* v = cr_from_zobj(obj);

    zend_object_std_dtor(&v->std);
    zval_ptr_dtor(&v->container);
}

static int compare_objects(zval* object1, zval* object2)
{
    zend_object* zobj1 = Z_OBJ_P(object1);
    zend_object* zobj2 = Z_OBJ_P(object2);

    /*
     * This is a cruel world, and therefore we need to defend ourselves
     * against someone who tries to assign the same compare handler
     * to different classes.
     *
     * No, you probably cannot do that from PHP's userland,
     * but hey, I am not the only one who uses dirty tricks :-)
     */
    if (zobj1->ce != zobj2->ce) {  /* LCOV_EXCL_BR_LINE */
        return 1;                  /* LCOV_EXCL_LINE - this cannot be tested without dirty hacks */
    }

    callable_resolver_t* v1 = cr_from_zobj(zobj1);
    callable_resolver_t* v2 = cr_from_zobj(zobj2);

    zval res;
    if (FAILURE == compare_function(&res, &v1->container, &v2->container) || Z_LVAL(res) != 0) {
        return 1;
    }

    return zend_get_std_object_handlers()->compare_objects(object1, object2);
}

static HashTable* get_properties(zval* object)
{
    callable_resolver_t* v = cr_from_zobj(Z_OBJ_P(object));

    HashTable* res = zend_std_get_properties(object);
    Z_ADDREF(v->container);
    zend_hash_update(res, str_container, &v->container);
    return res;
}

static HashTable* get_gc(zval* object, zval** table, int* n)
{
    callable_resolver_t* v = cr_from_zobj(Z_OBJ_P(object));
    *table = &v->container;
    *n     = 1;
    return zend_std_get_properties(object);
}

/*
 * public function __construct(Psr\Container\ContainerInterface $container)
 */
static PHP_METHOD(TurboSlim_CallableResolver, __construct)
{
    zval* container;
    zval* this_ptr = get_this(execute_data);
    callable_resolver_t* v = cr_from_zobj(Z_OBJ_P(this_ptr));

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(container, ce_Psr_Container_ContainerInterface)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    ZVAL_COPY(&v->container, container);
}

/**
 * <code>
 *  throw new RuntimeException(sprintf(
 *      '%s is not resolvable',
 *      is_array($callable) || is_object($callable) ? json_encode($callable) : $callable
 *  ));
 * </code>
 *
 * @param callable
 */
static void not_resolvable(zval* callable)
{
    zend_string* printable = NULL;
    if (Z_TYPE_P(callable) == IS_OBJECT || Z_TYPE_P(callable) == IS_ARRAY) {
        smart_str s = { NULL, 0 };
#if PHP_VERSION_ID < 70100
        php_json_encode(&s, callable, 0);
        smart_str_0(&s);
        printable = s.s;
#else
        if (UNEXPECTED(FAILURE == php_json_encode(&s, callable, 0))) {
            if (s.s) {
                zend_string_release(s.s);
            }
        }
        else {
            smart_str_0(&s);
            printable = s.s;
        }
#endif
    }
    else {
        printable = zval_get_string(callable);
    }

    zend_throw_exception_ex(spl_ce_RuntimeException, 0, "%s is not resolvable", (printable ? ZSTR_VAL(printable) : "<""???"">"));
    if (printable) {
        zend_string_release(printable);
    }
}

/**
 * @warning The function assumes that @a obj is a temporary variable and therefore will not invoke Z_TRY_ADDREF_P() on it.
 * @param return_value
 * @param obj
 * @param method
 */
static void make_callable(zval* return_value, zval* obj, zval* method)
{
    array_init_size(return_value, 2);
    zend_hash_real_init(Z_ARRVAL_P(return_value), 1);
    ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
        ZEND_HASH_FILL_ADD(obj);

        if (method) {
            Z_TRY_ADDREF_P(method);
            ZEND_HASH_FILL_ADD(method);
        }
        else {
            zval tmp;
#if PHP_VERSION_ID < 70100
            ZVAL_STRINGL(&tmp, ZEND_INVOKE_FUNC_NAME, strlen(ZEND_INVOKE_FUNC_NAME));
#elif PHP_VERSION_ID < 70200
            ZVAL_NEW_STR(&tmp, CG(known_strings)[ZEND_STR_MAGIC_INVOKE]);
            Z_TRY_ADDREF_P(&tmp);
#else
            ZVAL_NEW_STR(&tmp, ZSTR_KNOWN(ZEND_STR_MAGIC_INVOKE));
            Z_TRY_ADDREF_P(&tmp);
#endif

            ZEND_HASH_FILL_ADD(&tmp);
        }
    } ZEND_HASH_FILL_END();
}

static void ensure_is_callable(zval* callable)
{
    if (!zend_is_callable(callable, IS_CALLABLE_STRICT, NULL)) {
        not_resolvable(callable);
        zval_ptr_dtor(callable);
        ZVAL_NULL(callable);
    }
}

/**
 * 1. Checks whether @a obj is in the DIC; if so, returns [obj, method]
 * 2. Makes sure @a obj class exists, throws RuntimeError if not
 * 3. Returns [obj(container), method]
 *
 * @param return_value
 * @param c
 * @param obj
 * @param method
 */
static void resolve_callable(zval* return_value, callable_resolver_t* c, zval* obj, zval* method)
{
    zval rv;

    /* LCOV_EXCL_BR_START */
    assert(Z_TYPE_P(obj) == IS_STRING);
    assert(!method || Z_TYPE_P(method) == IS_STRING);
    /* LCOV_EXCL_BR_STOP */

    /*
     *  if ($this->container->has($class)) {
     *      return [$this->container->get($class), $method];
     *  }
     */
    zend_call_method_with_1_params(&c->container, Z_OBJCE(c->container), NULL, "has", &rv, obj);
    if (UNEXPECTED(EG(exception))) { /* has() should not throw, but anyway */
        return;
    }

    int has = zend_is_true(&rv);
    /* PSR-11 does not enforce the return type of has() */
    maybe_destroy_zval(&rv);

    if (has) {
        zend_call_method_with_1_params(&c->container, Z_OBJCE(c->container), NULL, "get", &rv, obj);
        if (UNEXPECTED(EG(exception))) {
            return;
        }

        make_callable(return_value, &rv, method); /* make_callable assumes rv is a temporary variable, no need to destroy it */
        ensure_is_callable(return_value);         /* Will kill return_value if not */
        return;
    }

    /*
     *  if (!class_exists($class)) {
     *      throw new RuntimeException(sprintf('Callable %s does not exist', $class));
     *  }
     */
    zend_class_entry* ce = zend_lookup_class(Z_STR_P(obj));
    if (!ce || (ce->ce_flags & (ZEND_ACC_INTERFACE | ZEND_ACC_TRAIT)) != 0) {
        zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Callable %s does not exist", Z_STRVAL_P(obj));
        return;
    }

    /* return [new $class($this->container), $method]; */
    object_init_ex(&rv, ce);
    zend_function* ctor = Z_OBJ_HANDLER(rv, get_constructor)(Z_OBJ(rv));
    if (UNEXPECTED(EG(exception))) {
        zval_ptr_dtor(&rv);
        return;
    }

    if (ctor) {
        zend_call_method(&rv, Z_OBJCE(rv), &ctor, ZSTR_VAL(ctor->common.function_name), ZSTR_LEN(ctor->common.function_name), NULL, 1, &c->container, NULL);
    }

    make_callable(return_value, &rv, method); /* make_callable assumes rv is a temporary variable, no need to destroy it */
    ensure_is_callable(return_value);         /* Will kill return_value if not */
}

static int is_slim_callable(zval* return_value, callable_resolver_t* c, zval* callable)
{
    pcre_cache_entry* pce = pcre_get_compiled_regex_cache(str_callable_pattern);
    int retval = 0;

    if (pce) {
        zval subpats;
        zval rv;

#if PHP_VERSION_ID < 70300
        ++pce->refcount;
#else
        php_pcre_pce_incref(pce);
#endif
        ZVAL_NULL(&subpats);
        php_pcre_match_impl(pce, Z_STRVAL_P(callable), (int)Z_STRLEN_P(callable), &rv, &subpats, 0, 0, 0, 0);
#if PHP_VERSION_ID < 70300
        ++pce->refcount;
#else
        php_pcre_pce_decref(pce);
#endif

        if (zend_is_true(&rv) && Z_TYPE(subpats) == IS_ARRAY) {
            resolve_callable(return_value, c, zend_hash_index_find(Z_ARRVAL(subpats), 1), zend_hash_index_find(Z_ARRVAL(subpats), 2));
            retval = 1;
        }

        zval_ptr_dtor(&subpats);
    }

    return retval;
}

/*
 * public function resolve($toResolve)
 */
static PHP_METHOD(TurboSlim_CallableResolver, resolve)
{
    zval* callable;
    zval* this_ptr = get_this(execute_data);
    callable_resolver_t* v = cr_from_zobj(Z_OBJ_P(this_ptr));

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(callable)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    if (zend_is_callable(callable, IS_CALLABLE_STRICT, NULL)) {
        RETURN_ZVAL(callable, 1, 0);
    }

    if (Z_TYPE_P(callable) != IS_STRING) {
        /*
         * Slim calls $this->assertCallable() callable which calls is_callable() on its argument.
         * We have already invoked zend_is_callable(), it returned false, there is no sense
         * to call it on the same argument again.
         */
        not_resolvable(callable);
        return;
    }

    /* check for slim callable as "class:method" */
    if (is_slim_callable(return_value, v, callable)) {
        return;
    }

    resolve_callable(return_value, v, callable, NULL);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_OBJ_INFO(0, container, Psr\\Container\\ContainerInterface, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_resolve, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, toResolve)
ZEND_END_ARG_INFO()

/*
 * CallableResolver in Slim has several protected methods,
 * but since CallableResolver is final, those methods cannot
 * be called from the outside, and we can safely ignore them
 */
static const zend_function_entry fe_TurboSlim_CallableResolver[] = {
    PHP_ME(TurboSlim_CallableResolver, __construct, arginfo___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(TurboSlim_CallableResolver, resolve,     arginfo_resolve,     ZEND_ACC_PUBLIC)
    PHP_FE_END
};

int init_callable_resolver()
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "TurboSlim\\CallableResolver", fe_TurboSlim_CallableResolver);
    ce_TurboSlim_CallableResolver = zend_register_internal_class(&ce);
    ce_TurboSlim_CallableResolver->ce_flags |= ZEND_ACC_FINAL;
    zend_class_implements(ce_TurboSlim_CallableResolver, 1, ce_TurboSlim_Interfaces_CallableResolverInterface);
    zend_declare_class_constant_stringl(ce_TurboSlim_CallableResolver, ZEND_STRL("CALLABLE_PATTERN"), ZSTR_VAL(str_callable_pattern), ZSTR_LEN(str_callable_pattern));

    ce_TurboSlim_CallableResolver->create_object = cr_create_object;

    memcpy(&handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    handlers.offset          = XtOffsetOf(callable_resolver_t, std);
    handlers.free_obj        = free_obj;
    handlers.compare_objects = compare_objects;
    handlers.get_properties  = get_properties;
    handlers.get_gc          = get_gc;

    return SUCCESS;
}
