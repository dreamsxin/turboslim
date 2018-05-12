#include "functions.h"

void turboslim_array_values(zval* return_value, zval* arr)
{
    assert(Z_TYPE_P(arr) == IS_ARRAY);

    zend_array* a = Z_ARRVAL_P(arr);
    size_t cnt    = zend_hash_num_elements(a);

    if ((!cnt || (HT_IS_PACKED(a) && HT_IS_WITHOUT_HOLES(a))) && a->nNextFreeElement == cnt) {
        ZVAL_COPY(return_value, arr);
        return;
    }

    array_init_size(return_value, cnt);
    zend_hash_real_init(Z_ARRVAL_P(return_value), 1);

    ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
        zval* entry;
        ZEND_HASH_FOREACH_VAL(a, entry) {
            if (UNEXPECTED(Z_ISREF_P(entry) && Z_REFCOUNT_P(entry) == 1)) {
                entry = Z_REFVAL_P(entry);
            }

            Z_TRY_ADDREF_P(entry);
            ZEND_HASH_FILL_ADD(entry);
        } ZEND_HASH_FOREACH_END();
    } ZEND_HASH_FILL_END();
}
