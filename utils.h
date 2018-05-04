#ifndef TURBOSLIM_UTILS_H
#define TURBOSLIM_UTILS_H

#include "php_turboslim.h"

#include <Zend/zend_operators.h>

static inline zval* array_zval_offset_get(HashTable* h, zval* key)
{
    switch (Z_TYPE_P(key)) {
        case IS_STRING:
            return zend_symtable_find(h, Z_STR_P(key));

        case IS_LONG:
            return zend_hash_index_find(h, Z_LVAL_P(key));

        case IS_FALSE:
            return zend_hash_index_find(h, 0);

        case IS_TRUE:
            return zend_hash_index_find(h, 1);

        case IS_DOUBLE:
            return zend_hash_index_find(h, zend_dval_to_lval(Z_DVAL_P(key)));

        case IS_RESOURCE:
            return zend_hash_index_find(h, Z_RES_HANDLE_P(key));

        case IS_NULL:
        case IS_UNDEF:
            return zend_hash_find(h, ZSTR_EMPTY_ALLOC());

        default:
            zend_error(E_WARNING, "Illegal offset type");
            return NULL;
    }
}

static inline zval* array_zval_offset_set(HashTable* h, zval* key, zval* value)
{
    zval* res;
    switch (Z_TYPE_P(key)) {
        case IS_STRING:
            res = zend_symtable_update(h, Z_STR_P(key), value);
            break;

        case IS_LONG:
            res = zend_hash_index_update(h, Z_LVAL_P(key), value);
            break;

        case IS_FALSE:
            res = zend_hash_index_update(h, 0, value);
            break;

        case IS_TRUE:
            res = zend_hash_index_update(h, 1, value);
            break;

        case IS_DOUBLE:
            res = zend_hash_index_update(h, zend_dval_to_lval(Z_DVAL_P(key)), value);
            break;

        case IS_RESOURCE:
            res = zend_hash_index_update(h, Z_RES_HANDLE_P(key), value);
            break;

        case IS_NULL:
        case IS_UNDEF:
            res = zend_hash_next_index_insert(h, value);
            break;

        default:
            zend_error(E_WARNING, "Illegal offset type");
            return NULL;
    }

    if (res) {
        if (Z_REFCOUNTED_P(res)) {
            Z_ADDREF_P(res);
        }
    }

    return res;
}

static inline int array_zval_offset_exists(HashTable* h, zval* key)
{
    switch (Z_TYPE_P(key)) {
        case IS_STRING:
            return zend_symtable_exists(h, Z_STR_P(key));

        case IS_LONG:
            return zend_hash_index_exists(h, Z_LVAL_P(key));

        case IS_FALSE:
            return zend_hash_index_exists(h, 0);

        case IS_TRUE:
            return zend_hash_index_exists(h, 1);

        case IS_DOUBLE:
            return zend_hash_index_exists(h, zend_dval_to_lval(Z_DVAL_P(key)));

        case IS_RESOURCE:
            return zend_hash_index_exists(h, Z_RES_HANDLE_P(key));

        case IS_NULL:
        case IS_UNDEF:
            return zend_hash_exists(h, ZSTR_EMPTY_ALLOC());

        default:
            zend_error(E_WARNING, "Illegal offset type");
            return 0;
    }
}

static inline int array_zval_offset_unset(HashTable* h, zval* key)
{
    switch (Z_TYPE_P(key)) {
        case IS_STRING:
            return zend_symtable_del(h, Z_STR_P(key));

        case IS_LONG:
            return zend_hash_index_del(h, Z_LVAL_P(key));

        case IS_FALSE:
            return zend_hash_index_del(h, 0);

        case IS_TRUE:
            return zend_hash_index_del(h, 1);

        case IS_DOUBLE:
            return zend_hash_index_del(h, zend_dval_to_lval(Z_DVAL_P(key)));

        case IS_RESOURCE:
            return zend_hash_index_del(h, Z_RES_HANDLE_P(key));

        case IS_NULL:
        case IS_UNDEF:
            return zend_hash_del(h, ZSTR_EMPTY_ALLOC());

        default:
            zend_error(E_WARNING, "Illegal offset type");
            return 0;
    }
}

static inline void maybe_destroy_zval(zval* z)
{
    if (UNEXPECTED(Z_REFCOUNTED_P(z))) {
        i_zval_ptr_dtor(z ZEND_FILE_LINE_CC);
    }
}

static inline zval* get_this(zend_execute_data* execute_data)
{
    zval* this_ptr = getThis();
    if (UNEXPECTED(this_ptr == NULL)) {         /* LCOV_EXCL_BR_LINE - this cannot happen without hacks */
        zend_error_noreturn(E_ERROR, "Unexpected condition: EX(This) is not object"); /* LCOV_EXCL_LINE */
    }

    return this_ptr;
}

TURBOSLIM_ATTR_NONNULL static inline zval* read_property_ex(zend_class_entry* scope, zval* object, zend_string* name, int silent, zval* rv)
{
#if PHP_VERSION_ID < 70100
    zval* ret = zend_read_property(scope, object, ZSTR_VAL(name), ZSTR_LEN(name), silent, rv);
#else
    zval* ret = zend_read_property_ex(scope, object, name, silent, rv);
#endif
    if (UNEXPECTED(ret == NULL)) {
        ret = &EG(uninitialized_zval);
    }

    return ret;
}

TURBOSLIM_ATTR_NONNULL static inline void wrong_callback_error(int severity, int num, char* error)
{
#if PHP_VERSION_ID < 70200
    zend_wrong_callback_error(severity, num, error);
#else
    zend_wrong_callback_error(0, severity, num, error);
#endif
}

#endif /* UTILS_H */