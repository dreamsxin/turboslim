#include "turboslim/traits/callableresolverawaretrait.h"

#include <Zend/zend_exceptions.h>
#include <Zend/zend_interfaces.h>
#include <Zend/zend_operators.h>
#include <ext/spl/spl_exceptions.h>
#include "turboslim/interfaces.h"
#include "turboslim/psr11.h"
#include "persistent.h"
#include "utils.h"

zend_class_entry* ce_TurboSlim_CallableResolverAwareTrait = NULL;

int turboslim_CallableResolverAwareTrait_resolveCallable(zval* return_value, zval* this_ptr, zval* callable)
{
    zend_class_entry* scope = get_executed_scope();
    zval resolver;
    zval tmp;

    zval* container = read_property_ex(scope, this_ptr, str_container, 0, &tmp);
    if (UNEXPECTED(EG(exception))) {
        return FAILURE;
    }

    if (Z_TYPE_P(container) != IS_OBJECT || !instanceof_function_ex(Z_OBJCE_P(container), ce_Psr_Container_ContainerInterface, 1)) {
        ZVAL_COPY(return_value, callable);
        return SUCCESS;
    }

    ZVAL_STR(&tmp, str_callableResolver);
    zend_call_method_with_1_params(container, Z_OBJCE_P(container), NULL, "get", &resolver, &tmp);
    /* Do not run zval_ptr_dtor(&tmp) - str_callableResolver is used elsewhere */
    if (UNEXPECTED(EG(exception))) {
        return FAILURE;
    }

    /*
     * We check if resolver actually implements CallableResolverInterface.
     * If it does not, this is probably an error anyway (per https://www.slimframework.com/docs/v3/concepts/di.html#required-services
     * callableResolver is Instance of \Slim\Interfaces\CallableResolverInterface). But if it does, we can use
     * relatively cheap zend_call_method_with_1_params() instead of
     * zend_fcall_info_init() + zend_fcall_info_argn + zend_fcall_info_call() + zend_fcall_info_args_clear()
     */
    if (Z_TYPE(resolver) != IS_OBJECT || !instanceof_function_ex(Z_OBJCE(resolver), ce_TurboSlim_Interfaces_CallableResolverInterface, 1)) {
        zval_ptr_dtor(&resolver);
        zend_throw_exception(spl_ce_UnexpectedValueException, "callableResolver must implement TurboSlim\\Interfaces\\CallableResolverInterface", 0);
        return FAILURE;
    }

    zend_call_method_with_1_params(&resolver, Z_OBJCE(resolver), NULL, "resolve", return_value, callable);
    zval_ptr_dtor(&resolver);
    return SUCCESS;
}

/**
 * @warning Unlike the original @c CallableResolverAwareTrait::resolveCallable(),
 * this one may also throw @c UnexpectedValueException if @c callableResolver
 * from the container does not implement @c TurboSlim\Interfaces\CallableResolverInterface
 */
PHP_METHOD(TurboSlim_CallableResolverAwareTrait, resolveCallable)
{
    zval* callable;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(callable)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    turboslim_CallableResolverAwareTrait_resolveCallable(return_value, this_ptr, callable);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_resolveCallable, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, callable)
ZEND_END_ARG_INFO()

const zend_function_entry fe_TurboSlim_CallableResolverAwareTrait[] = {
    PHP_ME(TurboSlim_CallableResolverAwareTrait, resolveCallable, arginfo_resolveCallable, ZEND_ACC_PROTECTED)
    PHP_FE_END
};
