#ifndef TURBOSLIM_INTERNAL_CLOSURE_H
#define TURBOSLIM_INTERNAL_CLOSURE_H

#include "php_turboslim.h"

TURBOSLIM_VISIBILITY_HIDDEN extern zend_class_entry* ce_TurboSlim_Internal_Closure;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_object_handlers turboslim_internal_closure_handlers;

TURBOSLIM_VISIBILITY_HIDDEN zend_object* turboslim_internal_closure_create_object(zend_class_entry* ce);
TURBOSLIM_VISIBILITY_HIDDEN void turboslim_internal_closure_free_obj(zend_object* obj);
TURBOSLIM_VISIBILITY_HIDDEN zend_object* turboslim_internal_closure_clone_obj(zval* object);
TURBOSLIM_VISIBILITY_HIDDEN int turboslim_internal_closure_get_closure(zval* object, zend_class_entry** ce_ptr, zend_function** fptr_ptr, zend_object** obj_ptr);
TURBOSLIM_VISIBILITY_HIDDEN HashTable* turboslim_internal_closure_get_gc(zval* object, zval** table, int* n);
TURBOSLIM_VISIBILITY_HIDDEN int turboslim_internal_closure_compare_objects(zval* z1, zval* z2);

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
