#include "methodnotallowedexception.h"

#include <Zend/zend_interfaces.h>
#include "persistent.h"
#include "psr7.h"
#include "turboslimexception.h"
#include "utils.h"

zend_class_entry* ce_TurboSlim_Exception_MethodNotAllowedException;

/*
 * public function __construct(ServerRequestInterface $request, ResponseInterface $response, array $allowedMethods)
 */
static PHP_METHOD(TurboSlim_Exception_MethodNotAllowedException, __construct)
{
    zval* request;
    zval* response;
    zval* methods;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(3, 3)
        Z_PARAM_OBJECT_OF_CLASS(request,  ce_Psr_Http_Message_ServerRequestInterface)
        Z_PARAM_OBJECT_OF_CLASS(response, ce_Psr_Http_Message_ResponseInterface)
        Z_PARAM_ARRAY(methods)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    zend_update_property_ex(ce_TurboSlim_Exception_MethodNotAllowedException, this_ptr, str_allowedMethods, methods);

    zend_function* ctor = ce_TurboSlim_Exception_TurboSlimException->constructor;
    if (EXPECTED(ctor != NULL)) {
        zend_call_method(this_ptr, ce_TurboSlim_Exception_TurboSlimException, &ctor, ZSTR_VAL(ctor->common.function_name), ZSTR_LEN(ctor->common.function_name), NULL, 2, request, response);
    }
}

/*
 * public function getAllowedMethods()
 */
static PHP_METHOD(TurboSlim_Exception_MethodNotAllowedException, getAllowedMethods)
{
    zval rv;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    ZVAL_COPY(return_value, read_property_ex(ce_TurboSlim_Exception_MethodNotAllowedException, this_ptr, str_allowedMethods, 0, &rv));
}

ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, ZEND_RETURN_VALUE, 3)
    ZEND_ARG_OBJ_INFO(0, request, Psr\\Http\\Message\\ServerRequestInterface, 0)
    ZEND_ARG_OBJ_INFO(0, response, Psr\\Http\\Message\\ResponseInterface, 0)
    ZEND_ARG_ARRAY_INFO(0, allowedMethods, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_empty, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry fe_TurboSlim_Exception_InvalidMethodException[] = {
    PHP_ME(TurboSlim_Exception_MethodNotAllowedException, __construct,        arginfo___construct, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_Exception_MethodNotAllowedException, getAllowedMethods,  arginfo_empty,       ZEND_ACC_PUBLIC)
    PHP_FE_END
};

int init_method_not_allowed_exception()
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Exception\\MethodNotAllowedException", fe_TurboSlim_Exception_InvalidMethodException);
    ce_TurboSlim_Exception_MethodNotAllowedException = zend_register_internal_class_ex(&ce, ce_TurboSlim_Exception_TurboSlimException);
    zend_declare_property_null(ce_TurboSlim_Exception_MethodNotAllowedException, ZEND_STRL("allowedMethods"), ZEND_ACC_PROTECTED);
    return SUCCESS;
}
