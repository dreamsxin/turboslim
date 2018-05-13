#ifndef TURBOSLIM_UTILS_ARRAY_H
#define TURBOSLIM_UTILS_ARRAY_H

#include "php_turboslim.h"

TURBOSLIM_ATTR_NONNULL static inline void hash_key_to_zval(zval* return_value, zend_hash_key* key)
{
    if (key->key) {
        ZVAL_STR(return_value, key->key);
    }
    else {
        ZVAL_LONG(return_value, key->h);
    }
}

TURBOSLIM_ATTR_NONNULL static inline zval* array_key_add_new(HashTable* ht, zend_string* key, zval* value)
{
    value = zend_hash_add_new(ht, key, value);
    if (value) {
        Z_TRY_ADDREF_P(value);
    }

    return value;
}

TURBOSLIM_ATTR_NONNULL static inline zval* array_index_add_new(HashTable* ht, zend_ulong h, zval* value)
{
    value = zend_hash_index_add_new(ht, h, value);
    if (value) {
        Z_TRY_ADDREF_P(value);
    }

    return value;
}

TURBOSLIM_ATTR_NONNULL static inline zval* array_key_update(HashTable* ht, zend_string* key, zval* value)
{
    value = zend_hash_update(ht, key, value);
    if (value) {
        Z_TRY_ADDREF_P(value);
    }

    return value;
}

TURBOSLIM_ATTR_NONNULL static inline zval* array_index_update(HashTable* ht, zend_ulong h, zval* value)
{
    value = zend_hash_index_update(ht, h, value);
    if (value) {
        Z_TRY_ADDREF_P(value);
    }

    return value;
}

TURBOSLIM_ATTR_NONNULL static inline zval* array_hashkey_update(HashTable* ht, zend_hash_key* key, zval* value)
{
    if (key->key) {
        return array_key_update(ht, key->key, value);
    }

    return array_index_update(ht, key->h, value);
}

TURBOSLIM_ATTR_NONNULL static inline zval* array_append(HashTable* ht, zval* value)
{
    value = zend_hash_next_index_insert(ht, value);
    if (value) {
        Z_TRY_ADDREF_P(value);
    }

    return value;
}

#endif /* TURBOSLIM_UTILS_ARRAY_H */
