#ifndef FASTCOLLECTION_H
#define FASTCOLLECTION_H

#include "php_turboslim.h"

TURBOSLIM_ATTR_NONNULL TURBOSLIM_VISIBILITY_HIDDEN void turboslim_create_FastCollection(zval* return_value, zval* data);

extern zend_class_entry* ce_TurboSlim_FastCollection;
TURBOSLIM_VISIBILITY_HIDDEN extern const zend_function_entry fe_TurboSlim_FastCollection[];
TURBOSLIM_VISIBILITY_HIDDEN extern zend_object_handlers turboslim_fastcollection_handlers;

TURBOSLIM_VISIBILITY_HIDDEN zend_object* turboslim_fastcollection_create_object(zend_class_entry* ce);
TURBOSLIM_VISIBILITY_HIDDEN void turboslim_fastcollection_free_obj(zend_object* obj);
TURBOSLIM_VISIBILITY_HIDDEN zend_object* turboslim_fastcollection_clone_obj(zval* obj);
TURBOSLIM_VISIBILITY_HIDDEN zval* turboslim_fastcollection_read_dimension(zval* object, zval* offset, int type, zval *rv);
TURBOSLIM_VISIBILITY_HIDDEN void turboslim_fastcollection_write_dimension(zval* object, zval* offset, zval* value);
TURBOSLIM_VISIBILITY_HIDDEN void turboslim_fastcollection_unset_dimension(zval* object, zval* offset);
TURBOSLIM_VISIBILITY_HIDDEN int turboslim_fastcollection_has_dimension(zval* object, zval* member, int check_empty);
TURBOSLIM_VISIBILITY_HIDDEN int turboslim_fastcollection_count_elements(zval* object, zend_long* count);
TURBOSLIM_VISIBILITY_HIDDEN HashTable* turboslim_fastcollection_get_properties(zval* object);
TURBOSLIM_VISIBILITY_HIDDEN int turboslim_fastcollection_compare_objects(zval* object1, zval* object2);
TURBOSLIM_VISIBILITY_HIDDEN zend_object_iterator* turboslim_fastcollection_get_iterator(zend_class_entry* ce, zval* object, int by_ref);

#endif /* FASTCOLLECTION_H */
