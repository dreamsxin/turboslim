#ifndef TURBOSLIM_DEFERREDCALLABLE_H
#define TURBOSLIM_DEFERREDCALLABLE_H

#include "php_turboslim.h"

/**
 * @warning @c turboslim_deferredcallable_get_gc() relies upon the layout of this structure
 */
typedef struct turboslim_deferredcallable {
    zval callable;
    zval container;
    zend_object std;
} turboslim_deferredcallable_t;

extern zend_class_entry* ce_TurboSlim_DeferredCallable;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_object_handlers turboslim_deferredcallable_handlers;
TURBOSLIM_VISIBILITY_HIDDEN extern const zend_function_entry fe_TurboSlim_DeferredCallable[];

TURBOSLIM_VISIBILITY_HIDDEN zend_object* turboslim_deferredcallable_create_object(zend_class_entry* ce);
TURBOSLIM_VISIBILITY_HIDDEN void turboslim_deferredcallable_free_obj(zend_object* obj);
TURBOSLIM_VISIBILITY_HIDDEN zend_object* turboslim_deferredcallable_clone_obj(zval* obj);
TURBOSLIM_VISIBILITY_HIDDEN HashTable* turboslim_deferredcallable_get_properties(zval* object);
TURBOSLIM_VISIBILITY_HIDDEN HashTable* turboslim_deferredcallable_get_gc(zval* obj, zval** table, int* n);
/* CallableResolverAwareTrait needs r/o access to $container */
TURBOSLIM_VISIBILITY_HIDDEN zval* turboslim_deferredcallable_read_property(zval* object, zval* member, int type, void** cache_slot, zval* rv);
TURBOSLIM_VISIBILITY_HIDDEN int turboslim_deferredcallable_compare_objects(zval* object1, zval* object2);

#endif /* DEFERREDCALLABLE_H */
