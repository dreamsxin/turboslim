#ifndef TURBOSLIM_INTERNAL_CLOSURE_H
#define TURBOSLIM_INTERNAL_CLOSURE_H

#include "php_turboslim.h"

TURBOSLIM_VISIBILITY_HIDDEN extern zend_class_entry* ce_TurboSlim_Internal_Closure;

TURBOSLIM_VISIBILITY_HIDDEN int init_turboslim_closure();

TURBOSLIM_VISIBILITY_HIDDEN void turboslim_closure_create(zval* return_value, zend_function* invoker, zval* callable, zend_fcall_info* fcc, zend_fcall_info_cache* fci, HashTable* bound);
TURBOSLIM_ATTR_NONNULL TURBOSLIM_VISIBILITY_HIDDEN HashTable* turboslim_closure_get_bound(zval* object);

/**
 * @warning Callable is returned as is, reference count is not modified
 * @param object
 * @param fci
 * @param fcc
 * @return
 */
TURBOSLIM_ATTR_NONNULL TURBOSLIM_VISIBILITY_HIDDEN zval* turboslim_closure_get_callable(zval* object, zend_fcall_info** fci, zend_fcall_info_cache** fcc);

#endif /* TURBOSLIM_INTERNAL_CLOSURE_H */
