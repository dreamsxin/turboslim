#include "turboslim/callableresolver.h"

#include <Zend/zend_exceptions.h>
#include <Zend/zend_interfaces.h>
#include <Zend/zend_operators.h>
#include <Zend/zend_smart_str.h>
#include <ext/json/php_json.h>
#include <ext/pcre/php_pcre.h>
#include <ext/spl/spl_exceptions.h>
#include "turboslim/container.h"
#include "turboslim/interfaces.h"
#include "turboslim/psr11.h"
#include "persistent.h"
#include "utils.h"

zend_class_entry* ce_TurboSlim_CallableResolver = NULL;

static zval* get_container(zend_object* zobj)
{
    return OBJ_PROP_NUM(zobj, 0);
}

/*
 * public function __construct(Psr\Container\ContainerInterface $container)
 */
static PHP_METHOD(TurboSlim_CallableResolver, __construct)
{
    zval* container;
    zval* this_ptr = get_this(execute_data);
    zval* ctr      = get_container(Z_OBJ_P(this_ptr));

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(container, ce_Psr_Container_ContainerInterface)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    ZVAL_COPY(ctr, container);
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

static void ensure_is_callable(zval* callable, zend_fcall_info_cache* fcc)
{
    if (!zend_is_callable_ex(callable, NULL, 0, NULL, fcc, NULL)) {
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
static void resolve_callable(zval* return_value, zval* container, zval* obj, zval* method, zend_fcall_info_cache* fcc)
{
    zval rv;

    /* LCOV_EXCL_BR_START */
    assert(Z_TYPE_P(obj) == IS_STRING);
    assert(!method || Z_TYPE_P(method) == IS_STRING);
    /* LCOV_EXCL_BR_STOP */

    zend_class_entry* container_ce = Z_OBJCE_P(container);
    if (container_ce == ce_TurboSlim_Container) {
        /* Fast path - container is TurboSlim\Container */
        if (FAILURE == turboslim_Container_get(&rv, container, obj, BP_VAR_IS)) {
            return;
        }

        /* if ($this->container->has($class)) */
        if (Z_TYPE(rv) != IS_UNDEF) {
            make_callable(return_value, &rv, method); /* make_callable assumes rv is a temporary variable, no need to destroy it */
            ensure_is_callable(return_value, fcc);    /* Will kill return_value if not */
            return;
        }
    }
    else {
        /*
         *  if ($this->container->has($class)) {
         *      return [$this->container->get($class), $method];
         *  }
         */
        zend_call_method_with_1_params(container, container_ce, NULL, "has", &rv, obj);
        if (UNEXPECTED(EG(exception))) { /* has() should not throw, but anyway */
            return;
        }

        int has = zend_is_true(&rv);
        /* PSR-11 does not enforce the return type of has() */
        maybe_destroy_zval(&rv);

        if (has) {
            zend_call_method_with_1_params(container, container_ce, NULL, "get", &rv, obj);
            if (UNEXPECTED(EG(exception))) {
                return;
            }

            make_callable(return_value, &rv, method); /* make_callable assumes rv is a temporary variable, no need to destroy it */
            ensure_is_callable(return_value, fcc);    /* Will kill return_value if not */
            return;
        }
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
        zend_call_method(&rv, Z_OBJCE(rv), &ctor, ZSTR_VAL(ctor->common.function_name), ZSTR_LEN(ctor->common.function_name), NULL, 1, container, NULL);
    }

    make_callable(return_value, &rv, method); /* make_callable assumes rv is a temporary variable, no need to destroy it */
    ensure_is_callable(return_value, fcc);    /* Will kill return_value if not */
}

static int is_slim_callable(zval* return_value, zval* container, zval* callable, zend_fcall_info_cache* fcc)
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
            resolve_callable(return_value, container, zend_hash_index_find(Z_ARRVAL(subpats), 1), zend_hash_index_find(Z_ARRVAL(subpats), 2), fcc);
            retval = 1;
        }

        zval_ptr_dtor(&subpats);
    }

    return retval;
}

void Turboslim_CallableResolver_resolve(zval* return_value, zval* this_ptr, zval* callable, zend_fcall_info_cache* fcc)
{
    zval* container = get_container(Z_OBJ_P(this_ptr));
    if (UNEXPECTED(Z_TYPE_P(container) != IS_OBJECT)) {
        zend_error(E_ERROR, "TurboSlim\\CallableResolver::$container is no longer an object");
        return;
    }

    if (zend_is_callable_ex(callable, NULL, 0, NULL, fcc, NULL)) {
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
    if (is_slim_callable(return_value, container, callable, fcc)) {
        return;
    }

    resolve_callable(return_value, container, callable, NULL, fcc);
}

/*
 * public function resolve($toResolve)
 */
static PHP_METHOD(TurboSlim_CallableResolver, resolve)
{
    zval* callable;
    zval* this_ptr = get_this(execute_data);
    zend_fcall_info_cache fcc;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(callable)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    Turboslim_CallableResolver_resolve(return_value, this_ptr, callable, &fcc);
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
const zend_function_entry fe_TurboSlim_CallableResolver[] = {
    PHP_ME(TurboSlim_CallableResolver, __construct, arginfo___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(TurboSlim_CallableResolver, resolve,     arginfo_resolve,     ZEND_ACC_PUBLIC)
    PHP_FE_END
};
