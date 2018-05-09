#include "turboslim/internal/callablewrapper.h"

#include <Zend/zend_exceptions.h>
#include <ext/spl/spl_exceptions.h>
#include "turboslim/internal/closure.h"
#include "turboslim/psr7.h"
#include "persistent.h"
#include "utils.h"

zend_function turboslim_internal_callablewrapper_invoker;

/*
 * public function __invoke(RequestInterface $request, ResponseInterface $response)
 */
PHP_METHOD(TurboSlim_Internal_CallableWrapper, __invoke)
{
    zval* request;
    zval* response;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_OBJECT_OF_CLASS(request, ce_Psr_Http_Message_RequestInterface)
        Z_PARAM_OBJECT_OF_CLASS(response, ce_Psr_Http_Message_ResponseInterface)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr   = get_this(execute_data);
    HashTable* bound = turboslim_closure_get_bound(this_ptr);
    zend_fcall_info* fci;
    zend_fcall_info_cache* fcc;

    assert(bound != NULL);                      /* LCOV_EXCL_BR_LINE */

#ifndef NDEBUG
    zval* callable =
#endif
                     turboslim_closure_get_callable(this_ptr, &fci, &fcc)
    ;
    zval* next     = zend_hash_index_find(bound, 0);

    assert(callable != NULL && next != NULL);   /* LCOV_EXCL_BR_LINE */

    zend_fcall_info_argn(fci, 3, request, response, next);
    zend_fcall_info_call(fci, fcc, return_value, NULL);
    zend_fcall_info_args_clear(fci, 1);

    if (UNEXPECTED(EG(exception))) {
        return;
    }

    if (!instanceof_function_ex(Z_OBJCE_P(return_value), ce_Psr_Http_Message_ResponseInterface, 1)) {
        zend_throw_exception(spl_ce_UnexpectedValueException, "Middleware must return instance of \\Psr\\Http\\Message\\ResponseInterface", 0);
        zval_ptr_dtor(return_value);
        ZVAL_NULL(return_value);
    }
}

void turboslim_create_CallableWrapper(zval* rv, zend_fcall_info* fci, zend_fcall_info_cache* fcc, zval* next)
{
    HashTable vars;

    zend_hash_init(&vars, 1, NULL, ZVAL_PTR_DTOR, 0);
    if (zend_hash_next_index_insert(&vars, next)) {
        Z_TRY_ADDREF_P(next);
    }

    turboslim_closure_create(rv, &turboslim_internal_callablewrapper_invoker, &fci->function_name, fci, fcc, &vars);
}
