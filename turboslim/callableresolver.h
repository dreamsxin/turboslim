#ifndef TURBOSLIM_CALLABLERESOLVER_H
#define TURBOSLIM_CALLABLERESOLVER_H

#include "php_turboslim.h"

typedef struct turboslim_callableresolver {
    zval container;
    zend_object std;
} turboslim_callableresolver_t;

extern zend_class_entry* ce_TurboSlim_CallableResolver;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_object_handlers turboslim_callableresolver_handlers;

TURBOSLIM_VISIBILITY_HIDDEN extern const zend_function_entry fe_TurboSlim_CallableResolver[];

TURBOSLIM_VISIBILITY_HIDDEN zend_object* turboslim_callableresolver_create_object(zend_class_entry* ce);
TURBOSLIM_VISIBILITY_HIDDEN void turboslim_callableresolver_free_obj(zend_object* obj);
TURBOSLIM_VISIBILITY_HIDDEN zend_object* turboslim_callableresolver_clone_obj(zval* obj);
TURBOSLIM_VISIBILITY_HIDDEN int turboslim_callableresolver_compare_objects(zval* object1, zval* object2);
TURBOSLIM_VISIBILITY_HIDDEN HashTable* turboslim_callableresolver_get_properties(zval* object);
TURBOSLIM_VISIBILITY_HIDDEN HashTable* turboslim_callableresolver_get_gc(zval* object, zval** table, int* n);

TURBOSLIM_ATTR_NONNULL TURBOSLIM_VISIBILITY_HIDDEN void Turboslim_CallableResolver_resolve(zval* return_value, zval* this_ptr, zval* callable, zend_fcall_info_cache* fcc);

#endif /* TURBOSLIM_CALLABLERESOLVER_H */
