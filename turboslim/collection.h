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

/**
 * @brief Creates a collection of class @ce in @a return_value, and optionally assigns @a items to it
 * @param return_value
 * @param ce
 * @param items Can be @c NULL
 * @pre `ce instanceof TurboSlim\Interfaces\CollectionInterface`
 * @post `Z_TYPE_P(return_value) == IS_OBJECT`
 * @note Equivalent to calling `new class_implementing_CollectionInterface($items)`;
 * the only difference is that the class constructor is not called.
 */
TURBOSLIM_ATTR_NONNULL2(1, 2) TURBOSLIM_VISIBILITY_HIDDEN void turboslim_Collection_create(zval* return_value, zend_class_entry* ce, zval* items);

/**
 * Returns @a key from @a collection into @a return_value. If @a key is not found, @a return_value is set to @c IS_UNDEF
 * @param return_value
 * @param collection
 * @param key
 * @pre `collection instanceof TurboSlim\Interfaces\CollectionInterface`
 * @note Equivalent to `TurboSlim\Collection::get($key)` but returns @c IS_UNDEF instead of @c IS_NULL if @a key is not found
 */
TURBOSLIM_ATTR_NONNULL TURBOSLIM_VISIBILITY_HIDDEN void turboslim_Collection_get(zval* return_value, zval* collection, zval* key);

/**
 * @brief Sets @a key in @a collection to @a value
 * @param collection
 * @param key
 * @param value
 * @pre `collection instanceof TurboSlim\Interfaces\CollectionInterface`
 * @note Equivalent to `TurboSlim\Collection::set($key, $value)`
 */
TURBOSLIM_ATTR_NONNULL TURBOSLIM_VISIBILITY_HIDDEN void turboslim_Collection_set(zval* collection, zval* key, zval* value);

/**
 * @brief Removes @a key from @a collection
 * @param collection
 * @param key
 * @pre `collection instanceof TurboSlim\Interfaces\CollectionInterface`
 * @note Equivalent to `TurboSlim\Collection::remove($key)`
 */
TURBOSLIM_ATTR_NONNULL TURBOSLIM_VISIBILITY_HIDDEN void turboslim_Collection_remove(zval* collection, zval* key);

/**
 * @brief Returns a pointer to @a collection's internal storage
 * @param collection
 * @return
 * @pre `collection instanceof TurboSlim\Interfaces\CollectionInterface`
 * @post `Z_TYPE_P(result) == IS_ARRAY`
 */
TURBOSLIM_ATTR_NONNULL TURBOSLIM_ATTR_RETURNS_NONNULL TURBOSLIM_VISIBILITY_HIDDEN zval* turboslim_Collection_all(zval* collection);

#endif /* TURBOSLIM_COLLECTION_H */
