#include "collection.h"
#include "interfaces.h"

#include <Zend/zend_exceptions.h>
#include <Zend/zend_interfaces.h>
#include <Zend/zend_operators.h>
#include <Zend/zend_smart_str.h>
#include <Zend/zend_string.h>
#include <ext/spl/spl_array.h>
#include <ext/spl/spl_exceptions.h>
#include <ext/standard/php_var.h>
#include "persistent.h"
#include "utils.h"

static zend_object_iterator* get_iterator(zend_class_entry* ce, zval* object, int by_ref);

zend_class_entry* ce_TurboSlim_Collection = NULL;

typedef struct collection {
    zval data;
    zend_object std;
} collection_t;

static zend_object_handlers handlers;

static inline collection_t* collection_from_zobj(const zend_object* obj)
{
    return (collection_t*)((char*)(obj) - XtOffsetOf(collection_t, std));
}

static void free_obj(zend_object* obj)
{
    collection_t* v = collection_from_zobj(obj);

    zend_object_std_dtor(&v->std);
    zval_ptr_dtor(&v->data);
}

static zval* read_property(zval* object, zval* member, int type, void** cache_slot, zval *rv)
{
    zval tmp;
    int is_data;

    ZVAL_UNDEF(&tmp);
    if (UNEXPECTED(Z_TYPE_P(member) != IS_STRING)) {
        ZVAL_STR(&tmp, zval_get_string(member));
        member = &tmp;
    }

    is_data = zend_string_equals_literal(Z_STR_P(member), "data");

    if (UNEXPECTED(Z_REFCOUNTED(tmp))) {
        zval_ptr_dtor(&tmp);
    }

    if (is_data) {
        zend_object* zobj = Z_OBJ_P(object);

        if (zend_check_protected(ce_TurboSlim_Collection, zobj->ce)) {
            collection_t* v = collection_from_zobj(zobj);
            return &v->data;
        }

        if (type != BP_VAR_IS) {
            zend_throw_error(NULL, "Cannot access protected property %s::$%s", ZSTR_VAL(ce_TurboSlim_Collection->name), Z_STRVAL_P(member));
        }

        return &EG(uninitialized_zval);
    }

    return zend_get_std_object_handlers()->read_property(object, member, type, cache_slot, rv);
}

static void write_property(zval* object, zval* member, zval* value, void** cache_slot)
{
    zval tmp;
    int is_data;

    ZVAL_UNDEF(&tmp);
    if (UNEXPECTED(Z_TYPE_P(member) != IS_STRING)) {
        ZVAL_STR(&tmp, zval_get_string(member));
        member = &tmp;
    }

    is_data = zend_string_equals_literal(Z_STR_P(member), "data");

    if (UNEXPECTED(Z_REFCOUNTED(tmp))) {
        zval_ptr_dtor(&tmp);
    }

    if (is_data) {
        zend_object* zobj = Z_OBJ_P(object);

        if (zend_check_protected(ce_TurboSlim_Collection, zobj->ce)) {
            collection_t* v = collection_from_zobj(zobj);
            if (Z_TYPE_P(value) != IS_ARRAY) {
                SEPARATE_ZVAL(value);
                convert_to_array(value);

                if (UNEXPECTED(EG(exception))) {
                    return;
                }
            }

            zval_ptr_dtor(&v->data);
            ZVAL_COPY(&v->data, value);
            return;
        }

        zend_throw_error(NULL, "Cannot access protected property %s::$%s", ZSTR_VAL(ce_TurboSlim_Collection->name), Z_STRVAL_P(member));
        return;
    }

    zend_get_std_object_handlers()->write_property(object, member, value, cache_slot);
}

static int has_property(zval* object, zval* member, int has_set_exists, void** cache_slot)
{
    zval tmp;
    int is_data;

    ZVAL_UNDEF(&tmp);
    if (UNEXPECTED(Z_TYPE_P(member) != IS_STRING)) {
        ZVAL_STR(&tmp, zval_get_string(member));
        member = &tmp;
    }

    is_data = zend_string_equals_literal(Z_STR_P(member), "data");

    if (UNEXPECTED(Z_REFCOUNTED(tmp))) {
        zval_ptr_dtor(&tmp);
    }

    if (is_data) {
        zend_object* zobj = Z_OBJ_P(object);

        if (zend_check_protected(ce_TurboSlim_Collection, zobj->ce)) {
            collection_t* v = collection_from_zobj(zobj);
            switch (has_set_exists) {
                /* HAS: whether property exists and is not NULL */
                case 0: return 1;
                /* SET: whether property exists and is true */
                default:
                case 1:
                    return zend_hash_num_elements(Z_ARRVAL(v->data)) > 0;
                /* EXISTS: whether property exists */
                case 2: return 1;
            }
        }
    }

    return zend_get_std_object_handlers()->has_property(object, member, has_set_exists, cache_slot);
}

static void unset_property(zval* object, zval* member, void** cache_slot)
{
    zval tmp;
    int is_data;

    ZVAL_UNDEF(&tmp);
    if (UNEXPECTED(Z_TYPE_P(member) != IS_STRING)) {
        ZVAL_STR(&tmp, zval_get_string(member));
        member = &tmp;
    }

    is_data = zend_string_equals_literal(Z_STR_P(member), "data");

    if (UNEXPECTED(Z_REFCOUNTED(tmp))) {
        zval_ptr_dtor(&tmp);
    }

    if (is_data) {
        zend_object* zobj = Z_OBJ_P(object);

        if (zend_check_protected(ce_TurboSlim_Collection, zobj->ce)) {
            collection_t* v = collection_from_zobj(zobj);
            zend_hash_clean(Z_ARRVAL(v->data));
            return;
        }

        zend_throw_error(NULL, "Cannot access protected property %s::$%s", ZSTR_VAL(ce_TurboSlim_Collection->name), Z_STRVAL_P(member));
        return;
    }

    zend_get_std_object_handlers()->unset_property(object, member, cache_slot);
}

static zval* read_dimension(zval* object, zval* offset, int type, zval *rv)
{
    zval znull;
    zval* z;

    zend_object* zobj = Z_OBJ_P(object);
    collection_t* v   = collection_from_zobj(zobj);

    if (offset) {
        z = array_zval_offset_get(Z_ARRVAL(v->data), offset);
    }
    else {
        z = NULL;
        ZVAL_NULL(&znull);
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
                SEPARATE_ARRAY(&v->data);
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

static void write_dimension(zval* object, zval* offset, zval* value)
{
    zend_object* zobj = Z_OBJ_P(object);
    collection_t* v   = collection_from_zobj(zobj);

    SEPARATE_ARRAY(&v->data);
    if (EXPECTED(offset != NULL)) {
        array_set_zval_key(Z_ARRVAL(v->data), offset, value);
    }
    else {
        Z_TRY_ADDREF_P(value);
        zend_hash_next_index_insert(Z_ARRVAL(v->data), value);
    }
}

static void unset_dimension(zval* object, zval* offset)
{
    zend_object* zobj = Z_OBJ_P(object);
    collection_t* v   = collection_from_zobj(zobj);

    SEPARATE_ARRAY(&v->data);
    array_zval_offset_unset(Z_ARRVAL(v->data), offset);
}

static int has_dimension(zval* object, zval* member, int check_empty)
{
    zend_object* zobj = Z_OBJ_P(object);
    collection_t* v   = collection_from_zobj(zobj);
    zval* result      = array_zval_offset_get(Z_ARRVAL(v->data), member);

    if (check_empty && result) {
        return i_zend_is_true(result);
    }

    return result != NULL;
}

static int count_elements(zval* object, zend_long* count)
{
    assert(object != NULL);

    zend_object* zobj = Z_OBJ_P(object);
    collection_t* v   = collection_from_zobj(zobj);
    *count = zend_hash_num_elements(Z_ARRVAL(v->data));
    return SUCCESS;
}

static HashTable* get_properties(zval* object)
{
    zend_object* zobj = Z_OBJ_P(object);
    collection_t* v   = collection_from_zobj(zobj);

    HashTable* res = zend_std_get_properties(object);
    if (zend_hash_update(res, str_data, &v->data)) {
        Z_ADDREF(v->data);
    }

    return res;
}

static zend_object_iterator* get_iterator(zend_class_entry* ce, zval* object, int by_ref)
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

static int serialize(zval* object, unsigned char** buffer, size_t* buf_len, zend_serialize_data* data)
{
    zend_object* zobj = Z_OBJ_P(object);
    collection_t* c   = collection_from_zobj(zobj);
    smart_str buf     = { NULL, 0 };
    php_serialize_data_t var_hash = (php_serialize_data_t)data;

    PHP_VAR_SERIALIZE_INIT(var_hash);
        php_var_serialize(&buf, &c->data, &var_hash);

        ZVAL_ARR(&c->data, zend_std_get_properties(object));
        php_var_serialize(&buf, &c->data, &var_hash);
    PHP_VAR_SERIALIZE_DESTROY(var_hash);

    *buffer  = (unsigned char*)estrndup(ZSTR_VAL(buf.s), ZSTR_LEN(buf.s));
    *buf_len = ZSTR_LEN(buf.s);
    zend_string_release(buf.s);
    return SUCCESS;
}

static int unserialize(zval *object, zend_class_entry *ce, const unsigned char *buf, size_t buf_len, zend_unserialize_data *data)
{
    php_unserialize_data_t unserialize_data = (php_unserialize_data_t)data;
    const unsigned char* p   = buf;
    const unsigned char* max = p + buf_len;
    zval object_copy;
    zval* zv;

    PHP_VAR_UNSERIALIZE_INIT(unserialize_data);

    object_init_ex(object, ce);
    ZVAL_COPY_VALUE(&object_copy, object);

    zend_object* zobj = Z_OBJ(object_copy);
    collection_t* c   = collection_from_zobj(zobj);

    zv = var_tmp_var(&unserialize_data);
    if (!php_var_unserialize(zv, &p, max, &unserialize_data) || Z_TYPE_P(zv) != IS_ARRAY) {
        PHP_VAR_UNSERIALIZE_DESTROY(unserialize_data);
        return FAILURE;
    }

    ZVAL_COPY(&c->data, zv);

    zv = var_tmp_var(&unserialize_data);
    if (!php_var_unserialize(zv, &p, max, &unserialize_data) || Z_TYPE_P(zv) != IS_ARRAY) {
        PHP_VAR_UNSERIALIZE_DESTROY(unserialize_data);
        return FAILURE;
    }

    object_properties_load(&c->std, Z_ARRVAL_P(zv));
    PHP_VAR_UNSERIALIZE_DESTROY(unserialize_data);
    return SUCCESS;
}

static void handle_inheritance(zend_class_entry* ce)
{
    if (ce != ce_TurboSlim_Collection) {
        zend_function* f1;
        zend_function* f2;

        f1 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("offsetget"));
        f2 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("get"));
        if (f1->common.scope == ce_TurboSlim_Collection && f2->common.scope == ce_TurboSlim_Collection) {
            handlers.read_dimension  = read_dimension;
        }

        f1 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("offsetset"));
        f2 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("set"));
        if (f1->common.scope == ce_TurboSlim_Collection && f2->common.scope == ce_TurboSlim_Collection) {
            handlers.write_dimension = write_dimension;
        }

        f1 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("offsetunset"));
        f2 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("remove"));
        if (f1->common.scope == ce_TurboSlim_Collection && f2->common.scope == ce_TurboSlim_Collection) {
            handlers.unset_dimension = unset_dimension;
        }

        f1 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("offsetexists"));
        f2 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("has"));
        if (f1->common.scope == ce_TurboSlim_Collection && f2->common.scope == ce_TurboSlim_Collection) {
            handlers.has_dimension   = has_dimension;
        }

        f1 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("count"));
        if (f1->common.scope == ce_TurboSlim_Collection) {
            handlers.count_elements  = count_elements;
        }
    }
    else {
        /* ArrayAccess */
        handlers.read_dimension  = read_dimension;
        handlers.write_dimension = write_dimension;
        handlers.unset_dimension = unset_dimension;
        handlers.has_dimension   = has_dimension;

        /* Countable */
        handlers.count_elements  = count_elements;
    }
}

static zend_object* create_object(zend_class_entry* ce)
{
    collection_t* v = ecalloc(1, sizeof(collection_t) + zend_object_properties_size(ce));

    zend_object_std_init(&v->std, ce);
    object_properties_init(&v->std, ce);
    v->std.handlers = &handlers;

    handle_inheritance(ce);

    array_init(&v->data);
    return &v->std;
}

/*
 * public function __construct(array $items = [])
 */
static PHP_METHOD(TurboSlim_Collection, __construct)
{
    HashTable* items  = NULL;
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    collection_t* c   = collection_from_zobj(zobj);

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY_HT(items)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    if (items) {
        zend_hash_copy(Z_ARRVAL(c->data), items, zval_add_ref);
    }
}

/*
 * public function set($key, $value)
 */
static PHP_METHOD(TurboSlim_Collection, set)
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

    SEPARATE_ARRAY(&c->data);
    /* if Z_TYPE_P(key) = IS_NULL, this will use an empty key rather then append */
    array_set_zval_key(Z_ARRVAL(c->data), key, value);
}

/*
 * public function get($key, $default = null)
 */
static PHP_METHOD(TurboSlim_Collection, get)
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
static PHP_METHOD(TurboSlim_Collection, replace)
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

    SEPARATE_ARRAY(&c->data);
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
static PHP_METHOD(TurboSlim_Collection, all)
{
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    collection_t* c   = collection_from_zobj(zobj);

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    RETURN_ZVAL(&c->data, 1, 0);
}

/*
 * public function keys()
 */
static PHP_METHOD(TurboSlim_Collection, keys)
{
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    collection_t* c   = collection_from_zobj(zobj);

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

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
static PHP_METHOD(TurboSlim_Collection, has)
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
static PHP_METHOD(TurboSlim_Collection, remove)
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

    SEPARATE_ARRAY(&c->data);
    array_zval_offset_unset(Z_ARRVAL(c->data), key);
}

/*
 * public function clear()
 */
static PHP_METHOD(TurboSlim_Collection, clear)
{
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    collection_t* c   = collection_from_zobj(zobj);

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    SEPARATE_ARRAY(&c->data);
    zend_hash_clean(Z_ARRVAL(c->data));
}
/*
 * public function count()
 *
 * This function is not really used, as we have count_elements
 * object handler installed.
 */
static PHP_METHOD(TurboSlim_Collection, count)
{
    zend_long count;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    count_elements(this_ptr, &count);
    RETURN_LONG(count);
}

/*
 * public function offsetGet($key)
 */
static PHP_METHOD(TurboSlim_Collection, offsetGet)
{
    zval* key;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL_EX(key, 1, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    zval* res      = read_dimension(this_ptr, key, key ? BP_VAR_W : BP_VAR_RW, return_value);
    if (res != return_value) {
        ZVAL_COPY(return_value, res);
    }
}

/*
 * public function offsetSet($key, $value)
 */
static PHP_METHOD(TurboSlim_Collection, offsetSet)
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
    write_dimension(this_ptr, key, value);
}

/*
 * public function serialize()
 */
static PHP_METHOD(TurboSlim_Collection, serialize)
{
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    collection_t* c   = collection_from_zobj(zobj);
    smart_str buf     = { NULL, 0 };
    php_serialize_data_t var_hash;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    PHP_VAR_SERIALIZE_INIT(var_hash);
        php_var_serialize(&buf, &c->data, &var_hash);

        ZVAL_ARR(&c->data, zend_std_get_properties(this_ptr));
        php_var_serialize(&buf, &c->data, &var_hash);
    PHP_VAR_SERIALIZE_DESTROY(var_hash);

    if (buf.s) {
        RETURN_NEW_STR(buf.s);
    }

    RETURN_NULL();
}

/*
 * public function unserialize($serialized)
 */
static PHP_METHOD(TurboSlim_Collection, unserialize)
{
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    collection_t* c   = collection_from_zobj(zobj);

    php_unserialize_data_t unserialize_data;
    char* buf;
    size_t buf_len;
    zval* zv;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(buf, buf_len)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    if (!buf_len) {
        return;
    }

    const unsigned char* p   = (const unsigned char*)buf;
    const unsigned char* max = p + buf_len;

    PHP_VAR_UNSERIALIZE_INIT(unserialize_data);
    zv = var_tmp_var(&unserialize_data);
    if (!php_var_unserialize(zv, &p, max, &unserialize_data) || Z_TYPE_P(zv) != IS_ARRAY) {
        PHP_VAR_UNSERIALIZE_DESTROY(unserialize_data);
        zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0, "Error at offset " ZEND_LONG_FMT " of %zd bytes", (zend_long)((char*)p - buf), buf_len);
        return;
    }

    zval_ptr_dtor(&c->data);
    ZVAL_COPY(&c->data, zv);

    zv = var_tmp_var(&unserialize_data);
    if (!php_var_unserialize(zv, &p, max, &unserialize_data) || Z_TYPE_P(zv) != IS_ARRAY) {
        PHP_VAR_UNSERIALIZE_DESTROY(unserialize_data);
        zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0, "Error at offset " ZEND_LONG_FMT " of %zd bytes", (zend_long)((char*)p - buf), buf_len);
        return;
    }

    object_properties_load(&c->std, Z_ARRVAL_P(zv));
    PHP_VAR_UNSERIALIZE_DESTROY(unserialize_data);
}

/*
 * public function getIterator()
 */
static PHP_METHOD(TurboSlim_Collection, getIterator)
{
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    collection_t* c   = collection_from_zobj(zobj);

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zend_function* ctor = spl_ce_ArrayIterator->constructor;

    object_init_ex(return_value, spl_ce_ArrayIterator);
    zend_call_method(return_value, spl_ce_ArrayIterator, &ctor, ZSTR_VAL(ctor->common.function_name), ZSTR_LEN(ctor->common.function_name), NULL, 1, &c->data, NULL);
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_unserialize, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, serialized)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_offsetGet, 0, ZEND_RETURN_REFERENCE, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

static const zend_function_entry fe_TurboSlim_Collection[] = {
    PHP_ME(TurboSlim_Collection, __construct, arginfo___construct, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_Collection, set,         arginfo_kv,          ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_Collection, get,         arginfo_kd,          ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_Collection, replace,     arginfo_replace,     ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_Collection, all,         arginfo_empty,       ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_Collection, keys,        arginfo_empty,       ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_Collection, has,         arginfo_key,         ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_Collection, remove,      arginfo_key,         ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_Collection, clear,       arginfo_empty,       ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_Collection, count,       arginfo_empty,       ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_Collection, getIterator, arginfo_empty,       ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_Collection, serialize,   arginfo_empty,       ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_Collection, unserialize, arginfo_unserialize, ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_Collection, offsetGet,   arginfo_offsetGet,   ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_Collection, offsetSet,   arginfo_kv,          ZEND_ACC_PUBLIC)

    PHP_MALIAS(TurboSlim_Collection, offsetExists, has,    arginfo_key, ZEND_ACC_PUBLIC)
    PHP_MALIAS(TurboSlim_Collection, offsetUnset,  remove, arginfo_key, ZEND_ACC_PUBLIC)

    PHP_FE_END
};

int init_collection()
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Collection", fe_TurboSlim_Collection);
    ce_TurboSlim_Collection = zend_register_internal_class(&ce);

    ce_TurboSlim_Collection->create_object = create_object;

    memcpy(&handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    handlers.offset          = XtOffsetOf(collection_t, std);
    handlers.free_obj        = free_obj;

    /* Handlers to guard $data property from being incorrectly overwritten */
    handlers.read_property   = read_property;
    handlers.write_property  = write_property;
    handlers.has_property    = has_property;
    handlers.unset_property  = unset_property;

    /* Misc */
    handlers.get_properties  = get_properties;

    /* IteratorAggregate interface */
    ce_TurboSlim_Collection->get_iterator = get_iterator;

    /* Serializable interface */
    ce_TurboSlim_Collection->serialize    = serialize;
    ce_TurboSlim_Collection->unserialize  = unserialize;

    zend_class_implements(ce_TurboSlim_Collection, 2, ce_TurboSlim_Interfaces_CollectionInterface, zend_ce_serializable);
    return SUCCESS;
}
