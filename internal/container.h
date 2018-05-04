#ifndef TURBOSLIM_INTERNAL_CONTAINER_H
#define TURBOSLIM_INTERNAL_CONTAINER_H

#include "php_turboslim.h"

TURBOSLIM_VISIBILITY_HIDDEN int init_internal_container_classes();

TURBOSLIM_VISIBILITY_HIDDEN void turboslim_container_createCallableWrapper(zval* return_value, zend_class_entry* ce, zval* callable);
TURBOSLIM_VISIBILITY_HIDDEN void turboslim_container_getCallableFromWrapper(zval* return_value, zval* wrapper);

TURBOSLIM_VISIBILITY_HIDDEN extern zend_class_entry* ce_TurboSlim_Internal_Container_ProtectedCallable;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_class_entry* ce_TurboSlim_Internal_Container_FactoryCallable;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_class_entry* ce_TurboSlim_Internal_Container_ServiceCallable;

#endif /* TURBOSLIM_INTERNAL_CONTAINER_H */
