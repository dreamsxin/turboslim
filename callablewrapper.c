#include "callablewrapper.h"

#include <Zend/zend_exceptions.h>
#include <ext/spl/spl_exceptions.h>
#include "internal/closure.h"
#include "persistent.h"
#include "psr7.h"
#include "utils.h"

/*
 * public function __invoke(RequestInterface $request, ResponseInterface $response)
 */
static PHP_METHOD(TurboSlim_Internal_CallableWrapper, __invoke)
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

    if (EXPECTED(bound != NULL)) {
        zend_fcall_info* fci;
        zend_fcall_info_cache* fcc;

        zval* callable = turboslim_closure_get_callable(this_ptr, &fci, &fcc);
        zval* next     = zend_hash_index_find(bound, 0);

        if (EXPECTED(callable != NULL && next != NULL)) {
            zend_fcall_info_argn(fci, 3, request, response, next);
            zend_fcall_info_call(fci, fcc, return_value, NULL);
            zend_fcall_info_args_clear(fci, 1);

            if (UNEXPECTED(EG(exception))) {
                return;
            }

            if (!instanceof_function_ex(Z_OBJCE_P(return_value), ce_Psr_Http_Message_ResponseInterface, 1)) {
                zend_throw_exception(spl_ce_UnexpectedValueException, "Middleware must return instance of \\Psr\\Http\\Message\\ResponseInterface", 0);
                zval_dtor(return_value);
                ZVAL_NULL(return_value);
            }
        }
    }

    zend_throw_error(NULL, "Bad callable data");
}

#if PHP_VERSION_ID < 70200
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo___invoke, ZEND_RETURN_VALUE, 2, IS_OBJECT, "Psr\\Http\\Message\\ResponseInterface", 0)
#else
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo___invoke, ZEND_RETURN_VALUE, 2, "Psr\\Http\\Message\\ResponseInterface", 0)
#endif
    ZEND_ARG_OBJ_INFO(0, request, Psr\\Http\\Message\\ServerRequestInterface, 0)
    ZEND_ARG_OBJ_INFO(0, response, Psr\\Http\\Message\\ResponseInterface, 0)
ZEND_END_ARG_INFO()

static zend_function invoker;

int init_callable_wrapper()
{
    invoker.type = ZEND_INTERNAL_FUNCTION;
    invoker.internal_function.arg_flags[0]      = 0;
    invoker.internal_function.arg_flags[1]      = 0;
    invoker.internal_function.arg_flags[2]      = 0;
    invoker.internal_function.fn_flags          = ZEND_ACC_CLOSURE;
    invoker.internal_function.function_name     = str_magic_function;
    invoker.internal_function.scope             = ce_TurboSlim_Internal_Closure;
    invoker.internal_function.prototype         = NULL;
    invoker.internal_function.num_args          = 2;
    invoker.internal_function.required_num_args = 2;
    invoker.internal_function.arg_info          = (zend_internal_arg_info*)(arginfo___invoke + 1);
    invoker.internal_function.handler           = ZEND_MN(TurboSlim_Internal_CallableWrapper___invoke);
    invoker.internal_function.module            = &turboslim_module_entry;

    return SUCCESS;
}

void create_CallableWrapper(zval* rv, zend_fcall_info* fci, zend_fcall_info_cache* fcc, zval* next)
{
    HashTable vars;

    zend_hash_init(&vars, 1, NULL, ZVAL_PTR_DTOR, 0);
    if (zend_hash_next_index_insert(&vars, next)) {
        Z_TRY_ADDREF_P(next);
    }

    turboslim_closure_create(rv, &invoker, NULL, fci, fcc, &vars);
}
