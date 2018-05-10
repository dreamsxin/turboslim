#ifndef TURBOSLIM_COLLECTION_H
#define TURBOSLIM_COLLECTION_H

#include "php_turboslim.h"

typedef struct turboslim_collection {
    struct {
        unsigned int fast_readdim:1;
        unsigned int fast_writedim:1;
        unsigned int fast_hasdim:1;
        unsigned int fast_unsetdim:1;
        unsigned int fast_count:1;
    } opt;
    zend_object std;
} turboslim_collection_t;

extern zend_class_entry* ce_TurboSlim_Collection;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_object_handlers turboslim_collection_handlers;
TURBOSLIM_VISIBILITY_HIDDEN extern const zend_function_entry fe_TurboSlim_Collection[];

TURBOSLIM_VISIBILITY_HIDDEN zend_object* turboslim_collection_create_object(zend_class_entry* ce);
TURBOSLIM_VISIBILITY_HIDDEN zend_object* turboslim_collection_clone_obj(zval* obj);
TURBOSLIM_VISIBILITY_HIDDEN zval* turboslim_collection_read_dimension(zval* object, zval* offset, int type, zval *rv);
TURBOSLIM_VISIBILITY_HIDDEN void turboslim_collection_write_dimension(zval* object, zval* offset, zval* value);
TURBOSLIM_VISIBILITY_HIDDEN void turboslim_collection_unset_dimension(zval* object, zval* offset);
TURBOSLIM_VISIBILITY_HIDDEN int turboslim_collection_has_dimension(zval* object, zval* member, int check_empty);
TURBOSLIM_VISIBILITY_HIDDEN int turboslim_collection_count_elements(zval* object, zend_long* count);
TURBOSLIM_VISIBILITY_HIDDEN zend_object_iterator* turboslim_collection_get_iterator(zend_class_entry* ce, zval* object, int by_ref);

#endif /* TURBOSLIM_COLLECTION_H */
