#ifndef TURBOSLIM_CALLABLERESOLVER_H
#define TURBOSLIM_CALLABLERESOLVER_H

#include "php_turboslim.h"

extern zend_class_entry* ce_TurboSlim_CallableResolver;

TURBOSLIM_VISIBILITY_HIDDEN extern const zend_function_entry fe_TurboSlim_CallableResolver[];

TURBOSLIM_ATTR_NONNULL TURBOSLIM_VISIBILITY_HIDDEN void Turboslim_CallableResolver_resolve(zval* return_value, zval* this_ptr, zval* callable, zend_fcall_info_cache* fcc);

#endif /* TURBOSLIM_CALLABLERESOLVER_H */
