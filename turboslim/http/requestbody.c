#include "turboslim/http/requestbody.h"

#include <Zend/zend_interfaces.h>
#include <Zend/zend_exceptions.h>
#include <ext/spl/spl_exceptions.h>
#include <ext/standard/file.h>
#include "utils.h"

zend_class_entry* ce_TurboSlim_Http_RequestBody = NULL;

static ZEND_METHOD(TurboSlim_Http_RequestBody, __construct)
{
    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* zcontext = NULL;
    zval zstream;

    php_stream_context* context = php_stream_context_from_zval(zcontext, 0);
    php_stream* stream = php_stream_open_wrapper_ex("php://input", "r", 0, NULL, context);

    if (UNEXPECTED(!stream)) {
        zend_throw_exception(spl_ce_RuntimeException, "Failed to open php://input for reading", 0);
        return;
    }

    php_stream_to_zval(stream, &zstream);

    zval* this_ptr = get_this(execute_data);
    zend_function* ctor = ce_TurboSlim_Http_RequestBody->parent->constructor;
    zend_call_method(this_ptr, ce_TurboSlim_Http_RequestBody->parent, &ctor, ZSTR_VAL(ctor->common.function_name), ZSTR_LEN(ctor->common.function_name), NULL, 1, &zstream, NULL);
    zval_ptr_dtor(&zstream);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_empty, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

const zend_function_entry fe_TurboSlim_Http_RequestBody[] = {
    ZEND_ME(TurboSlim_Http_RequestBody, __construct, arginfo_empty, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    ZEND_FE_END
};
