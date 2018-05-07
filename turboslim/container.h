#ifndef TURBOSLIM_CONTAINER_H
#define TURBOSLIM_CONTAINER_H

#include "php_turboslim.h"

typedef struct turboslim_container {
    HashTable items;
    zval* gc;
    int gc_num;
    zend_bool fast_readdim;
    zend_bool fast_writedim;
    zend_bool fast_hasdim;
    zend_bool fast_unsetdim;
    zend_bool fast_rh;
    zend_object std;
} turboslim_container_t;

extern zend_class_entry* ce_TurboSlim_Container;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_function_entry fe_TurboSlim_Container[];
TURBOSLIM_VISIBILITY_HIDDEN zend_object_handlers turboslim_container_handlers;
TURBOSLIM_VISIBILITY_HIDDEN zend_function turboslim_container_invoker;

TURBOSLIM_VISIBILITY_HIDDEN PHP_FUNCTION(TurboSlim_Container_Invoker);

TURBOSLIM_VISIBILITY_HIDDEN zend_object* turboslim_container_create_object(zend_class_entry* ce);
TURBOSLIM_VISIBILITY_HIDDEN void turboslim_container_free_obj(zend_object* obj);
TURBOSLIM_VISIBILITY_HIDDEN zend_object* turboslim_container_clone_obj(zval* obj);
TURBOSLIM_VISIBILITY_HIDDEN HashTable* turboslim_container_get_gc(zval* obj, zval** table, int* n);
TURBOSLIM_VISIBILITY_HIDDEN zval* turboslim_container_read_dimension(zval* object, zval* offset, int type, zval *rv);
TURBOSLIM_VISIBILITY_HIDDEN void turboslim_container_write_dimension(zval* object, zval* offset, zval* value);
TURBOSLIM_VISIBILITY_HIDDEN void turboslim_container_unset_dimension(zval* object, zval* offset);
TURBOSLIM_VISIBILITY_HIDDEN int turboslim_container_has_dimension(zval* object, zval* member, int check_empty);
TURBOSLIM_VISIBILITY_HIDDEN zval* turboslim_container_read_property(zval* object, zval* member, int type, void** cache_slot, zval *rv);
TURBOSLIM_VISIBILITY_HIDDEN int turboslim_container_has_property(zval* object, zval* member, int has_set_exists, void** cache_slot);
TURBOSLIM_VISIBILITY_HIDDEN int turboslim_container_compare_objects(zval* object1, zval* object2);

#endif /* TURBOSLIM_CONTAINER_H */
