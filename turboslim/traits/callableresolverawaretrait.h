#ifndef TURBOSLIM_TRAITS_CALLABLERESOLVERAWARETRAIT_H
#define TURBOSLIM_TRAITS_CALLABLERESOLVERAWARETRAIT_H

#include "php_turboslim.h"

TURBOSLIM_VISIBILITY_HIDDEN extern zend_class_entry* ce_TurboSlim_CallableResolverAwareTrait;

TURBOSLIM_VISIBILITY_HIDDEN extern const zend_function_entry fe_TurboSlim_CallableResolverAwareTrait[];

TURBOSLIM_VISIBILITY_HIDDEN PHP_METHOD(TurboSlim_CallableResolverAwareTrait, resolveCallable);
TURBOSLIM_VISIBILITY_HIDDEN TURBOSLIM_ATTR_NONNULL int turboslim_CallableResolverAwareTrait_resolveCallable(zval* return_value, zval* this_ptr, zval* callable, zend_fcall_info_cache* fcc);

#endif /* TURBOSLIM_TRAITS_CALLABLERESOLVERAWARETRAIT_H */
