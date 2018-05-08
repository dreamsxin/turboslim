#ifndef TURBOSLIM_COLLECTION_H
#define TURBOSLIM_COLLECTION_H

#include "php_turboslim.h"

typedef struct turboslim_collection {
    zval data;
    zend_bool fast_readdim;
    zend_bool fast_writedim;
    zend_bool fast_hasdim;
    zend_bool fast_unsetdim;
    zend_bool fast_count;
    zend_object std;
} turboslim_collection_t;

extern zend_class_entry* ce_TurboSlim_Collection;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_object_handlers turboslim_collection_handlers;
TURBOSLIM_VISIBILITY_HIDDEN extern const zend_function_entry fe_TurboSlim_Collection[];

TURBOSLIM_VISIBILITY_HIDDEN zend_object* turboslim_collection_create_object(zend_class_entry* ce);
TURBOSLIM_VISIBILITY_HIDDEN void turboslim_collection_free_obj(zend_object* obj);
TURBOSLIM_VISIBILITY_HIDDEN zend_object* turboslim_collection_clone_obj(zval* obj);
TURBOSLIM_VISIBILITY_HIDDEN zval* turboslim_collection_read_property(zval* object, zval* member, int type, void** cache_slot, zval *rv);
TURBOSLIM_VISIBILITY_HIDDEN void turboslim_collection_write_property(zval* object, zval* member, zval* value, void** cache_slot);
TURBOSLIM_VISIBILITY_HIDDEN int turboslim_collection_has_property(zval* object, zval* member, int has_set_exists, void** cache_slot);
TURBOSLIM_VISIBILITY_HIDDEN void turboslim_collection_unset_property(zval* object, zval* member, void** cache_slot);
TURBOSLIM_VISIBILITY_HIDDEN zval* turboslim_collection_read_dimension(zval* object, zval* offset, int type, zval *rv);
TURBOSLIM_VISIBILITY_HIDDEN void turboslim_collection_write_dimension(zval* object, zval* offset, zval* value);
TURBOSLIM_VISIBILITY_HIDDEN void turboslim_collection_unset_dimension(zval* object, zval* offset);
TURBOSLIM_VISIBILITY_HIDDEN int turboslim_collection_has_dimension(zval* object, zval* member, int check_empty);
TURBOSLIM_VISIBILITY_HIDDEN int turboslim_collection_count_elements(zval* object, zend_long* count);
TURBOSLIM_VISIBILITY_HIDDEN HashTable* turboslim_collection_get_properties(zval* object);
TURBOSLIM_VISIBILITY_HIDDEN int turboslim_collection_compare_objects(zval* object1, zval* object2);
TURBOSLIM_VISIBILITY_HIDDEN zend_object_iterator* turboslim_collection_get_iterator(zend_class_entry* ce, zval* object, int by_ref);
int turboslim_collection_serialize(zval* object, unsigned char** buffer, size_t* buf_len, zend_serialize_data* data);
int turboslim_collection_unserialize(zval *object, zend_class_entry *ce, const unsigned char *buf, size_t buf_len, zend_unserialize_data *data);


#endif /* TURBOSLIM_COLLECTION_H */
