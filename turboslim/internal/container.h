#ifndef TURBOSLIM_INTERNAL_CONTAINER_H
#define TURBOSLIM_INTERNAL_CONTAINER_H

#include "php_turboslim.h"

TURBOSLIM_VISIBILITY_HIDDEN void turboslim_container_createCallableWrapper(zval* return_value, zend_class_entry* ce, zval* callable);
TURBOSLIM_VISIBILITY_HIDDEN void turboslim_container_getCallableFromWrapper(zval* return_value, zval* wrapper);

TURBOSLIM_VISIBILITY_HIDDEN extern zend_class_entry* ce_TurboSlim_Internal_Container_ProtectedCallable;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_class_entry* ce_TurboSlim_Internal_Container_FactoryCallable;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_class_entry* ce_TurboSlim_Internal_Container_ServiceCallable;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_object_handlers turboslim_internal_container_handlers;

TURBOSLIM_VISIBILITY_HIDDEN zend_object* turboslim_internal_container_create_object(zend_class_entry* ce);
TURBOSLIM_VISIBILITY_HIDDEN void turboslim_internal_container_free_obj(zend_object* object);
TURBOSLIM_VISIBILITY_HIDDEN zend_object* turboslim_internal_container_clone_obj(zval* obj);
TURBOSLIM_VISIBILITY_HIDDEN zend_object* turboslim_internal_container_clone_obj(zval* obj);
TURBOSLIM_VISIBILITY_HIDDEN int turboslim_internal_container_compare_objects(zval* object1, zval* object2);
TURBOSLIM_VISIBILITY_HIDDEN HashTable* turboslim_internal_container_get_gc(zval* object, zval** table, int* n);

#endif /* TURBOSLIM_INTERNAL_CONTAINER_H */
