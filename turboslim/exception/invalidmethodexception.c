#include "turboslim/exception/invalidmethodexception.h"

#include <string.h>
#include <Zend/zend_interfaces.h>
#include <Zend/zend_string.h>
#include <ext/spl/spl_exceptions.h>
#include "turboslim/psr7.h"
#include "persistent.h"
#include "utils.h"

zend_class_entry* ce_TurboSlim_Exception_InvalidMethodException;

static zend_string* build_error_message(const char* method, size_t method_len)
{
    const char* msg_start = "Unsupported HTTP method \"";
    const char* msg_end   = "\" provided";
    size_t msg_start_len  = strlen(msg_start);
    size_t msg_end_len    = strlen(msg_end);
    zend_string* msg      = zend_string_alloc(msg_start_len + msg_end_len + method_len, 0);
    char* p               = ZSTR_VAL(msg);

    memcpy(p, msg_start, msg_start_len);
    memcpy(p + msg_start_len, method, method_len);
    memcpy(p + msg_start_len + method_len, msg_end, msg_end_len);
    p[msg_start_len + msg_end_len + method_len] = '\0';

    return msg;
}

/*
 * public function __construct(Psr\Http\Message\ServerRequestInterface $request, $method)
 */
static PHP_METHOD(TurboSlim_Exception_InvalidMethodException, __construct)
{
    zval* request;
    char* method;
    size_t method_len;
    zval message;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_OBJECT_OF_CLASS(request, ce_Psr_Http_Message_ServerRequestInterface)
        Z_PARAM_STRING(method, method_len)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    zend_update_property_ex(ce_TurboSlim_Exception_InvalidMethodException, this_ptr, str_request, request);

    zend_function* ctor = spl_ce_InvalidArgumentException->constructor;
    if (EXPECTED(ctor != NULL)) {
        ZVAL_STR(&message, build_error_message(method, method_len));
        zend_call_method(this_ptr, spl_ce_InvalidArgumentException, &ctor, ZSTR_VAL(ctor->common.function_name), ZSTR_LEN(ctor->common.function_name), NULL, 1, &message, NULL);
        zval_ptr_dtor(&message);
    }
}

/*
 * public function getRequest()
 */
static PHP_METHOD(TurboSlim_Exception_InvalidMethodException, getRequest)
{
    zval rv;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    ZVAL_COPY(return_value, read_property_ex(ce_TurboSlim_Exception_InvalidMethodException, this_ptr, str_request, 0, &rv));
}

ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_OBJ_INFO(0, request, Psr\\Http\\Message\\ServerRequestInterface, 0)
    ZEND_ARG_INFO(0, method)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_empty, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

const zend_function_entry fe_TurboSlim_Exception_InvalidMethodException[] = {
    PHP_ME(TurboSlim_Exception_InvalidMethodException, __construct, arginfo___construct, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_Exception_InvalidMethodException, getRequest,  arginfo_empty,       ZEND_ACC_PUBLIC)
    PHP_FE_END
};
