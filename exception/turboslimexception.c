#include "turboslimexception.h"

#include <Zend/zend_interfaces.h>
#include <Zend/zend_exceptions.h>
#include "persistent.h"
#include "psr7.h"
#include "utils.h"

zend_class_entry* ce_TurboSlim_Exception_TurboSlimException = NULL;

/*
 * public function __construct(ServerRequestInterface $request, ResponseInterface $response)
 */
static PHP_METHOD(TurboSlim_Exception_TurboSlimException, __construct)
{
    zval* request;
    zval* response;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_OBJECT_OF_CLASS(request,  ce_Psr_Http_Message_ServerRequestInterface)
        Z_PARAM_OBJECT_OF_CLASS(response, ce_Psr_Http_Message_ResponseInterface)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    zend_update_property_ex(ce_TurboSlim_Exception_TurboSlimException, this_ptr, str_request, request);
    zend_update_property_ex(ce_TurboSlim_Exception_TurboSlimException, this_ptr, str_response, response);

    zend_function* ctor = zend_ce_exception->constructor;
    if (EXPECTED(ctor != NULL)) {
        zend_call_method(this_ptr, zend_ce_exception, &ctor, ZSTR_VAL(ctor->common.function_name), ZSTR_LEN(ctor->common.function_name), NULL, 0, NULL, NULL);
    }
}

/*
 * public function getRequest()
 */
static PHP_METHOD(TurboSlim_Exception_TurboSlimException, getRequest)
{
    zval rv;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    ZVAL_COPY(return_value, read_property_ex(ce_TurboSlim_Exception_TurboSlimException, this_ptr, str_request, 0, &rv));
}

/*
 * public function getResponse()
 */
static PHP_METHOD(TurboSlim_Exception_TurboSlimException, getResponse)
{
    zval rv;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    ZVAL_COPY(return_value, read_property_ex(ce_TurboSlim_Exception_TurboSlimException, this_ptr, str_response, 0, &rv));
}

ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_OBJ_INFO(0, request, Psr\\Http\\Message\\ServerRequestInterface, 0)
    ZEND_ARG_OBJ_INFO(0, response, Psr\\Http\\Message\\ResponseInterface, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_empty, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry fe_TurboSlim_Exception_TurboSlimException[] = {
    PHP_ME(TurboSlim_Exception_TurboSlimException, __construct, arginfo___construct, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_Exception_TurboSlimException, getRequest,  arginfo_empty,       ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_Exception_TurboSlimException, getResponse, arginfo_empty,       ZEND_ACC_PUBLIC)
    PHP_FE_END
};

int init_turbo_slim_exception()
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Exception\\TurboSlimException", fe_TurboSlim_Exception_TurboSlimException);
    ce_TurboSlim_Exception_TurboSlimException = zend_register_internal_class_ex(&ce, zend_ce_exception);
    zend_declare_property_null(ce_TurboSlim_Exception_TurboSlimException, ZEND_STRL("request"),  ZEND_ACC_PROTECTED);
    zend_declare_property_null(ce_TurboSlim_Exception_TurboSlimException, ZEND_STRL("response"), ZEND_ACC_PROTECTED);
    return SUCCESS;
}
