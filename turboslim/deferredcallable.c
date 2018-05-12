#include "deferredcallable.h"

#include <Zend/zend_closures.h>
#include <Zend/zend_inheritance.h>
#include <Zend/zend_interfaces.h>
#include <Zend/zend_operators.h>
#include "turboslim/psr11.h"
#include "turboslim/traits/callableresolverawaretrait.h"
#include "persistent.h"
#include "utils.h"

#include <Zend/zend_compile.h>

zend_class_entry* ce_TurboSlim_DeferredCallable = NULL;

static inline zval* get_callable(zend_object* zobj)
{
    return OBJ_PROP_NUM(zobj, 0);
}

static inline zval* get_container(zend_object* zobj)
{
    return OBJ_PROP_NUM(zobj, 1);
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

    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);

    ZVAL_COPY(get_callable(zobj), callable);
    if (container) {
        ZVAL_COPY(get_container(zobj), container);
    }
}

static PHP_METHOD(TurboSlim_DeferredCallable, __invoke)
{
    zval* argv = NULL;
    int argn;
    zval callable;

    zend_fcall_info fci;
    zend_fcall_info_cache fcc;

    memset(&fcc, 0, sizeof(fcc));

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, -1)
        Z_PARAM_VARIADIC('*', argv, argn)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    if (FAILURE == turboslim_CallableResolverAwareTrait_resolveCallable(&callable, this_ptr, get_callable(zobj), &fcc)) {
        return;
    }

    if (IS_OBJECT == Z_TYPE(callable) && instanceof_function(Z_OBJCE(callable), zend_ce_closure)) {
        zval new_callable;
        zval* container = get_container(zobj);
        if (Z_TYPE_P(container) != IS_OBJECT) {
            container = &znull;
        }

        if (!zend_is_identical(container, zend_get_closure_this_ptr(&callable))) {
            /* The code is equivalent for $callable = $callable->bindTo($this->container); */
#if PHP_VERSION_ID < 70114
            static zend_function* proxy = NULL;
            zend_call_method(&callable, zend_ce_closure, &proxy, ZEND_STRL("bindto"), &new_callable, 1, container, NULL);
#else
            zend_function* func            = (zend_function*)zend_get_closure_method_def(&callable);
            zend_class_entry* called_scope = (IS_OBJECT == Z_TYPE_P(container)) ? Z_OBJCE_P(container) : func->common.scope;
            zend_class_entry* ce           = func->common.scope;
            zend_create_closure(&new_callable, func, ce, called_scope, container);
#endif

            zval_ptr_dtor(&callable);
            ZVAL_COPY_VALUE(&callable, &new_callable);

            if (!Z_OBJ_HANDLER(callable, get_closure) || FAILURE == Z_OBJ_HANDLER(callable, get_closure)(&callable, &fcc.calling_scope, &fcc.function_handler, &fcc.object)) {
#ifdef COVERAGE
                /*
                 * lcov incorrectly shows that `fcc.function_handler = NULL` line is hit;
                 * if `memset()` is not hit, `fcc` is not either
                 */
                memset(&fcc, 0, sizeof(fcc));
#endif
                fcc.function_handler = NULL;
            }
        }
    }

    if (UNEXPECTED(!fcc.function_handler)) {
        /* We usually land here is the callable is not resolvable */
        char* error;
        if (EXPECTED(FAILURE == zend_fcall_info_init(&callable, 0, &fci, &fcc, NULL, &error))) {
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
    }
    else {
        fci.size          = sizeof(fci);
        fci.object        = fcc.object;
        ZVAL_COPY_VALUE(&fci.function_name, &callable);
        fci.retval        = NULL;
        fci.param_count   = 0;
        fci.params        = NULL;
        fci.no_separation = 1;
#if PHP_VERSION_ID < 70100
        fci.symbol_table  = NULL;
#endif
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
    ZEND_NAMED_ME(resolveCallable, ZEND_MN(TurboSlim_CallableResolverAwareTrait_resolveCallable), arginfo_resolveCallable, ZEND_ACC_PROTECTED)
    PHP_ME(TurboSlim_DeferredCallable, __invoke, arginfo___invoke, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
