#ifndef TURBOSLIM_CALLABLEWRAPPER_H
#define TURBOSLIM_CALLABLEWRAPPER_H

#include "php_turboslim.h"

TURBOSLIM_VISIBILITY_HIDDEN extern zend_function turboslim_internal_callablewrapper_invoker;

TURBOSLIM_VISIBILITY_HIDDEN PHP_METHOD(TurboSlim_Internal_CallableWrapper, __invoke);
TURBOSLIM_VISIBILITY_HIDDEN void turboslim_create_CallableWrapper(zval* rv, zend_fcall_info* fci, zend_fcall_info_cache* fcc, zval* next);

#endif /* CALLABLEWRAPPER_H */
