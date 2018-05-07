#include "fastcollection.h"
#include "interfaces.h"

#include <Zend/zend_exceptions.h>
#include <Zend/zend_interfaces.h>
#include <Zend/zend_operators.h>
#include <Zend/zend_smart_str.h>
#include <Zend/zend_string.h>
#include <ext/spl/spl_array.h>
#include <ext/spl/spl_exceptions.h>
#include "persistent.h"
#include "utils.h"

zend_class_entry* ce_TurboSlim_FastCollection = NULL;
zend_object_handlers turboslim_fastcollection_handlers;

typedef struct collection {
    zend_object std;
    zval data;
} collection_t;

static inline collection_t* collection_from_zobj(const zend_object* obj)
{
    return (collection_t*)obj;
}

zend_object* turboslim_fastcollection_create_object(zend_class_entry* ce)
{
    collection_t* v = ecalloc(1, sizeof(collection_t));

    zend_object_std_init(&v->std, ce);
    v->std.handlers = &turboslim_fastcollection_handlers;

    array_init(&v->data);
    return &v->std;
}

void turboslim_fastcollection_free_obj(zend_object* obj)
{
    collection_t* v = collection_from_zobj(obj);

    zend_object_std_dtor(&v->std);
    zval_ptr_dtor(&v->data);
}

zend_object* turboslim_fastcollection_clone_obj(zval* obj)
{
    zend_object* old_object = Z_OBJ_P(obj);
    zend_object* new_object = turboslim_fastcollection_create_object(old_object->ce);

    collection_t* mine      = collection_from_zobj(old_object);
    collection_t* theirs    = collection_from_zobj(new_object);

    zend_hash_copy(Z_ARRVAL(theirs->data), Z_ARRVAL(mine->data), zval_add_ref);
    return new_object;
}

zval* turboslim_fastcollection_read_dimension(zval* object, zval* offset, int type, zval *rv)
{
    zval* z;

    zend_object* zobj = Z_OBJ_P(object);
    collection_t* v   = collection_from_zobj(zobj);

    if (offset) {
        z = array_zval_offset_get(Z_ARRVAL(v->data), offset);
    }
    else {
        z      = NULL;
        offset = &znull;
    }

    if (!z) {
        switch (type) {
            case BP_VAR_R:
            default:
                ZVAL_NULL(rv);
                return rv;

            case BP_VAR_IS:
                return &EG(uninitialized_zval);

            case BP_VAR_RW:
            case BP_VAR_UNSET:
            case BP_VAR_W: {
                ZVAL_NULL(rv);
                z = zend_hash_next_index_insert(Z_ARRVAL(v->data), rv);
                if (UNEXPECTED(!z)) {
                    return &EG(uninitialized_zval);
                }

                ZVAL_NEW_REF(z, z);
                return z;
            }
        }
        /* unreachable */
    }

    if ((type == BP_VAR_W || type == BP_VAR_RW || type == BP_VAR_UNSET) && !Z_ISREF_P(z)) {
        ZVAL_NEW_REF(z, z);
    }

    return z;
}

void turboslim_fastcollection_write_dimension(zval* object, zval* offset, zval* value)
{
    zend_object* zobj = Z_OBJ_P(object);
    collection_t* v   = collection_from_zobj(zobj);

    if (!offset) {
        Z_TRY_ADDREF_P(value);
        zend_hash_next_index_insert(Z_ARRVAL(v->data), value);
    }
    else {
        array_set_zval_key(Z_ARRVAL(v->data), offset, value);
    }
}

void turboslim_fastcollection_unset_dimension(zval* object, zval* offset)
{
    zend_object* zobj = Z_OBJ_P(object);
    collection_t* v   = collection_from_zobj(zobj);
    array_zval_offset_unset(Z_ARRVAL(v->data), offset);
}

int turboslim_fastcollection_has_dimension(zval* object, zval* member, int check_empty)
{
    zend_object* zobj = Z_OBJ_P(object);
    collection_t* v   = collection_from_zobj(zobj);
    zval* result      = array_zval_offset_get(Z_ARRVAL(v->data), member);

    if (check_empty && result) {
        return zend_is_true(result);
    }

    return result != NULL;
}

int turboslim_fastcollection_count_elements(zval* object, zend_long* count)
{
    zend_object* zobj = Z_OBJ_P(object);
    collection_t* v   = collection_from_zobj(zobj);
    *count = zend_hash_num_elements(Z_ARRVAL(v->data));
    return SUCCESS;
}

HashTable* turboslim_fastcollection_get_properties(zval* object)
{
    zend_object* zobj = Z_OBJ_P(object);
    collection_t* v   = collection_from_zobj(zobj);
    return Z_ARRVAL(v->data);
}

int turboslim_fastcollection_compare_objects(zval* object1, zval* object2)
{
    zend_object* zobj1 = Z_OBJ_P(object1);
    zend_object* zobj2 = Z_OBJ_P(object2);

    if (zobj1->ce != zobj2->ce) {  /* LCOV_EXCL_BR_LINE */
        return 1;                  /* LCOV_EXCL_LINE - this cannot be tested without dirty hacks */
    }

    collection_t* v1 = collection_from_zobj(zobj1);
    collection_t* v2 = collection_from_zobj(zobj2);

    zval r;
    if (UNEXPECTED(FAILURE == compare_function(&r, &v1->data, &v2->data)) || Z_LVAL(r) != 0) {
        return 1;
    }

    return 0;
}

zend_object_iterator* turboslim_fastcollection_get_iterator(zend_class_entry* ce, zval* object, int by_ref)
{
    zval iterator;
    zend_object* zobj   = Z_OBJ_P(object);
    collection_t* c     = collection_from_zobj(zobj);
    zend_function* ctor = spl_ce_ArrayIterator->constructor;

    object_init_ex(&iterator, spl_ce_ArrayIterator);
    zend_call_method(&iterator, spl_ce_ArrayIterator, &ctor, ZSTR_VAL(ctor->common.function_name), ZSTR_LEN(ctor->common.function_name), NULL, 1, &c->data, NULL);

    if (UNEXPECTED(EG(exception))) {
        zval_ptr_dtor(&iterator);
        return NULL;
    }

    zend_object_iterator* it = spl_ce_ArrayIterator->get_iterator(spl_ce_ArrayIterator, &iterator, by_ref);
    zval_ptr_dtor(&iterator);
    return it;
}

/*
 * public function __construct(array $items = [])
 */
static PHP_METHOD(TurboSlim_FastCollection, __construct)
{
    zval* items       = NULL;
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    collection_t* c   = collection_from_zobj(zobj);

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY(items)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    if (items) {
        zend_hash_copy(Z_ARRVAL(c->data), Z_ARRVAL_P(items), zval_add_ref);
    }
}

/*
 * public function set($key, $value)
 */
static PHP_METHOD(TurboSlim_FastCollection, set)
{
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    collection_t* c   = collection_from_zobj(zobj);
    zval* key;
    zval* value;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_ZVAL(key)
        Z_PARAM_ZVAL(value);
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    array_set_zval_key(Z_ARRVAL(c->data), key, value);
}

/*
 * public function get($key, $default = null)
 */
static PHP_METHOD(TurboSlim_FastCollection, get)
{
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    collection_t* c   = collection_from_zobj(zobj);
    zval* key;
    zval* def = NULL;
    zval* tmp;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ZVAL(key)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(def);
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    tmp = array_zval_offset_get(Z_ARRVAL(c->data), key);
    if (tmp) {
        RETURN_ZVAL(tmp, 1, 0);
    }

    if (def) {
        RETURN_ZVAL(def, 1, 0);
    }

    RETURN_NULL();
}

/*
 * public function replace(array $items)
 */
static PHP_METHOD(TurboSlim_FastCollection, replace)
{
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    collection_t* c   = collection_from_zobj(zobj);
    HashTable* items;
    zend_ulong h;
    zend_string* key;
    zval* val;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY_HT(items)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    ZEND_HASH_FOREACH_KEY_VAL(items, h, key, val)
        Z_TRY_ADDREF_P(val);
        if (!key) {
            zend_hash_index_update(Z_ARRVAL(c->data), h, val);
        }
        else {
            zend_hash_update(Z_ARRVAL(c->data), key, val);
        }
    ZEND_HASH_FOREACH_END();
}

/*
 * public function all()
 */
static PHP_METHOD(TurboSlim_FastCollection, all)
{
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    collection_t* c   = collection_from_zobj(zobj);

    array_init_size(return_value, zend_hash_num_elements(Z_ARRVAL(c->data)));
    zend_hash_copy(Z_ARRVAL_P(return_value), Z_ARRVAL(c->data), zval_add_ref);
}

/*
 * public function keys()
 */
static PHP_METHOD(TurboSlim_FastCollection, keys)
{
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    collection_t* c   = collection_from_zobj(zobj);

    int n = zend_hash_num_elements(Z_ARRVAL(c->data));
    array_init_size(return_value, n);
    if (!n) {
        return;
    }

    zend_hash_real_init(Z_ARRVAL_P(return_value), 1);
    ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
        zend_ulong num_idx;
        zend_string *str_idx;
        zval item;

        ZEND_HASH_FOREACH_KEY(Z_ARRVAL(c->data), num_idx, str_idx) {
            if (str_idx) {
                ZVAL_STR_COPY(&item, str_idx);
            }
            else {
                ZVAL_LONG(&item, num_idx);
            }

            ZEND_HASH_FILL_ADD(&item);
        } ZEND_HASH_FOREACH_END();
    } ZEND_HASH_FILL_END();
}

/*
 * public function has($key)
 */
static PHP_METHOD(TurboSlim_FastCollection, has)
{
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    collection_t* c   = collection_from_zobj(zobj);
    zval* key;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(key)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    RETURN_BOOL(array_zval_offset_exists(Z_ARRVAL(c->data), key));
}

/*
 * public function remove($key)
 */
static PHP_METHOD(TurboSlim_FastCollection, remove)
{
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    collection_t* c   = collection_from_zobj(zobj);
    zval* key;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(key)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    array_zval_offset_unset(Z_ARRVAL(c->data), key);
}

/*
 * public function clear()
 */
static PHP_METHOD(TurboSlim_FastCollection, clear)
{
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    collection_t* c   = collection_from_zobj(zobj);

    zend_hash_clean(Z_ARRVAL(c->data));
}

/*
 * public function count()
 */
static PHP_METHOD(TurboSlim_FastCollection, count)
{
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    collection_t* v   = collection_from_zobj(zobj);

    RETURN_LONG(zend_hash_num_elements(Z_ARRVAL(v->data)));
}

/*
 * public function getIterator()
 */
static PHP_METHOD(TurboSlim_FastCollection, getIterator)
{
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    collection_t* c   = collection_from_zobj(zobj);

    zend_function* ctor = spl_ce_ArrayIterator->constructor;

    object_init_ex(return_value, spl_ce_ArrayIterator);
    zend_call_method(return_value, spl_ce_ArrayIterator, &ctor, ZSTR_VAL(ctor->common.function_name), ZSTR_LEN(ctor->common.function_name), NULL, 1, &c->data, NULL);
}
/*
 * public function offsetGet($key)
 */
static PHP_METHOD(TurboSlim_FastCollection, offsetGet)
{
    zval* key;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL_EX(key, 1, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    zval* res      = turboslim_fastcollection_read_dimension(this_ptr, key, key ? BP_VAR_W : BP_VAR_RW, return_value);
    if (res != return_value) {
        ZVAL_COPY(return_value, res);
    }
}

/*
 * public function offsetSet($key, $value)
 */
static PHP_METHOD(TurboSlim_FastCollection, offsetSet)
{
    zval* key;
    zval* value;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_ZVAL_EX(key, 1, 0)
        Z_PARAM_ZVAL(value);
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    turboslim_fastcollection_write_dimension(this_ptr, key, value);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, ZEND_RETURN_VALUE, 0)
    ZEND_ARG_ARRAY_INFO(0, items, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_empty, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

/* mixed func(mixed $key) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_key, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

/* mixed func(mixed $key, mixed $value) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_kv, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

/* mixed func(mixed $key, mixed $default = null) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_kd, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, default)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_replace, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_ARRAY_INFO(0, items, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_offsetGet, 0, ZEND_RETURN_REFERENCE, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

const zend_function_entry fe_TurboSlim_FastCollection[] = {
    PHP_ME(TurboSlim_FastCollection, __construct, arginfo___construct, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_FastCollection, set,         arginfo_kv,          ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_FastCollection, get,         arginfo_kd,          ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_FastCollection, replace,     arginfo_replace,     ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_FastCollection, all,         arginfo_empty,       ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_FastCollection, keys,        arginfo_empty,       ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_FastCollection, has,         arginfo_key,         ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_FastCollection, remove,      arginfo_key,         ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_FastCollection, clear,       arginfo_empty,       ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_FastCollection, count,       arginfo_empty,       ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_FastCollection, getIterator, arginfo_empty,       ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_FastCollection, offsetGet,   arginfo_offsetGet,   ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_FastCollection, offsetSet,   arginfo_kv,          ZEND_ACC_PUBLIC)

    PHP_MALIAS(TurboSlim_FastCollection, offsetExists, has,    arginfo_key, ZEND_ACC_PUBLIC)
    PHP_MALIAS(TurboSlim_FastCollection, offsetUnset,  remove, arginfo_key, ZEND_ACC_PUBLIC)

    PHP_FE_END
};

void turboslim_create_FastCollection(zval* return_value, zval* data)
{
    assert(data != NULL);                /* LCOV_EXCL_BR_LINE */
    assert(Z_TYPE_P(data) == IS_ARRAY);  /* LCOV_EXCL_BR_LINE */

    object_init_ex(return_value, ce_TurboSlim_FastCollection);

    zend_object* zobj = Z_OBJ_P(return_value);
    collection_t* c   = collection_from_zobj(zobj);
    zend_hash_copy(Z_ARRVAL(c->data), Z_ARRVAL_P(data), zval_add_ref);
}
