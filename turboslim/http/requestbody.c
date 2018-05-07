#include "turboslim/http/requestbody.h"

#include <Zend/zend_interfaces.h>
#include <Zend/zend_exceptions.h>
#include <ext/spl/spl_exceptions.h>
#include <ext/standard/file.h>
#include "utils.h"

zend_class_entry* ce_TurboSlim_Http_RequestBody = NULL;

static int open_stream(zval* return_value, const char* url, const char* mode)
{
    zval* zcontext = NULL;

    php_stream_context* context = php_stream_context_from_zval(zcontext, 0);
    php_stream* stream = php_stream_open_wrapper_ex(url, mode, 0, NULL, context);

    if (UNEXPECTED(!stream)) {
        return FAILURE;
    }

    php_stream_to_zval(stream, return_value);
    return SUCCESS;
}

static ZEND_METHOD(TurboSlim_Http_RequestBody, __construct)
{
    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval zsrc;
    zval zdst;

    if (UNEXPECTED(FAILURE == open_stream(&zsrc, "php://input", "r"))) {
        zend_throw_exception(spl_ce_RuntimeException, "Failed to open php://input for reading", 0);
        return;
    }

    if (UNEXPECTED(FAILURE == open_stream(&zdst, "php://temp", "w+"))) {
        zval_ptr_dtor(&zsrc);
        zend_throw_exception(spl_ce_RuntimeException, "Failed to open php://temp for writing", 0);
        return;
    }

    php_stream* src;
    php_stream* dst;
    size_t len;

    php_stream_from_zval_no_verify(src, &zsrc);
    php_stream_from_zval_no_verify(dst, &zdst);

    if (SUCCESS == php_stream_copy_to_stream_ex(src, dst, PHP_STREAM_COPY_ALL, &len) && -1 != php_stream_rewind(dst)) {
        zval* this_ptr = get_this(execute_data);
        zend_function* ctor = ce_TurboSlim_Http_RequestBody->parent->constructor;
        zend_call_method(this_ptr, ce_TurboSlim_Http_RequestBody->parent, &ctor, ZSTR_VAL(ctor->common.function_name), ZSTR_LEN(ctor->common.function_name), NULL, 1, &zdst, NULL);

        zval_ptr_dtor(&zsrc);
        zval_ptr_dtor(&zdst);
        return;
    }

    zval_ptr_dtor(&zsrc);
    zval_ptr_dtor(&zdst);
    zend_throw_exception(spl_ce_RuntimeException, "Failed to copy php://stdin to php://temp", 0);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_empty, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

const zend_function_entry fe_TurboSlim_Http_RequestBody[] = {
    ZEND_ME(TurboSlim_Http_RequestBody, __construct, arginfo_empty, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    ZEND_FE_END
};
