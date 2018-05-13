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
#include "utils/array.h"
#include "persistent.h"
#include "utils.h"

typedef turboslim_collection_t collection_t;

zend_class_entry* ce_TurboSlim_Collection = NULL;
zend_object_handlers turboslim_collection_handlers;

static inline collection_t* collection_from_zobj(const zend_object* obj)
{
    return (collection_t*)((char*)(obj) - XtOffsetOf(collection_t, std));
}

static inline zval* get_data(zend_object* zobj)
{
    zval* z = OBJ_PROP_NUM(zobj, 0);
    if (UNEXPECTED(Z_TYPE_P(z)) != IS_ARRAY) {
        if (Z_TYPE_P(z) == IS_UNDEF) {
            array_init(z);
        }
        else {
            convert_to_array(z);
        }
    }

    return z;
}

static void handle_inheritance(collection_t* v, zend_class_entry* ce)
{
    if (ce != ce_TurboSlim_Collection) {
        zend_function* f1;
        zend_function* f2;

        f1 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("offsetget"));
        f2 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("get"));
        v->opt.fast_readdim = (f1->common.scope == ce_TurboSlim_Collection && f2->common.scope == ce_TurboSlim_Collection);
        v->opt.fast_get     = f2->common.scope == ce_TurboSlim_Collection;

        f1 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("offsetset"));
        f2 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("set"));
        v->opt.fast_writedim = (f1->common.scope == ce_TurboSlim_Collection && f2->common.scope == ce_TurboSlim_Collection);

        f1 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("offsetunset"));
        f2 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("remove"));
        v->opt.fast_unsetdim = (f1->common.scope == ce_TurboSlim_Collection && f2->common.scope == ce_TurboSlim_Collection);

        f1 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("offsetexists"));
        f2 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("has"));
        v->opt.fast_hasdim = (f1->common.scope == ce_TurboSlim_Collection && f2->common.scope == ce_TurboSlim_Collection);

        f1 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("count"));
        v->opt.fast_count = (f1->common.scope == ce_TurboSlim_Collection);
    }
    else {
        v->opt.fast_readdim  = 1;
        v->opt.fast_writedim = 1;
        v->opt.fast_hasdim   = 1;
        v->opt.fast_unsetdim = 1;
        v->opt.fast_count    = 1;
        v->opt.fast_get      = 1;
    }
}

zend_object* turboslim_collection_create_object(zend_class_entry* ce)
{
    collection_t* v = ecalloc(1, sizeof(collection_t) + zend_object_properties_size(ce));

    zend_object_std_init(&v->std, ce);
    object_properties_init(&v->std, ce);
    v->std.handlers = &turboslim_collection_handlers;

    handle_inheritance(v, ce);
    return &v->std;
}

zend_object* turboslim_collection_clone_obj(zval* obj)
{
    zend_object* old_object = Z_OBJ_P(obj);
    zend_object* new_object = turboslim_collection_create_object(old_object->ce);

    collection_t* mine      = collection_from_zobj(old_object);
    collection_t* theirs    = collection_from_zobj(new_object);

    theirs->opt = mine->opt;

    zend_objects_clone_members(new_object, old_object);
    return new_object;
}

static zval* read_dimension(zval* data, zval* offset, int type, zval *rv)
{
    zval* z;

    if (offset) {
        z = array_zval_offset_get(Z_ARRVAL_P(data), offset);
    }
    else {
        z      = NULL;
        offset = &EG(uninitialized_zval);
    }

    if (!z) {
        switch (type) {
            case BP_VAR_R:      /* Read */
            default:
                ZVAL_NULL(rv);
                return rv;

            case BP_VAR_IS:     /* isset() */
                return &EG(uninitialized_zval);

            case BP_VAR_RW:     /* ++, --  */
            case BP_VAR_UNSET:  /* unset() */
            case BP_VAR_W: {    /* write   */
                ZVAL_NULL(rv);
                SEPARATE_ARRAY(data);
                z = zend_hash_next_index_insert(Z_ARRVAL_P(data), rv);
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

zval* turboslim_collection_read_dimension(zval* object, zval* offset, int type, zval *rv)
{
    zend_object* zobj = Z_OBJ_P(object);
    collection_t* v   = collection_from_zobj(zobj);

    if (v->opt.fast_readdim) {
        return read_dimension(get_data(zobj), offset, type, rv);
    }

    return zend_get_std_object_handlers()->read_dimension(object, offset, type, rv);
}

static void write_dimension(zval* data, zval* offset, zval* value)
{
    SEPARATE_ARRAY(data);
    if (EXPECTED(offset != NULL)) {
        array_set_zval_key(Z_ARRVAL_P(data), offset, value);
    }
    else {
        array_append(Z_ARRVAL_P(data), value);
    }
}

void turboslim_collection_write_dimension(zval* object, zval* offset, zval* value)
{
    zend_object* zobj = Z_OBJ_P(object);
    collection_t* v   = collection_from_zobj(zobj);

    if (v->opt.fast_writedim) {
        write_dimension(get_data(zobj), offset, value);
    }
    else {
        zend_get_std_object_handlers()->write_dimension(object, offset, value);
    }
}

void turboslim_collection_unset_dimension(zval* object, zval* offset)
{
    zend_object* zobj = Z_OBJ_P(object);
    collection_t* v   = collection_from_zobj(zobj);

    if (v->opt.fast_unsetdim) {
        zval* data = get_data(zobj);
        SEPARATE_ARRAY(data);
        array_zval_offset_unset(Z_ARRVAL_P(data), offset);
        return;
    }

    zend_get_std_object_handlers()->unset_dimension(object, offset);
}

int turboslim_collection_has_dimension(zval* object, zval* member, int check_empty)
{
    zend_object* zobj = Z_OBJ_P(object);
    collection_t* v   = collection_from_zobj(zobj);

    if (v->opt.fast_hasdim) {
        zval* data   = get_data(zobj);
        zval* result = array_zval_offset_get(Z_ARRVAL_P(data), member);

        if (check_empty && result) {
            return zend_is_true(result);
        }

        return result != NULL;
    }

    return zend_get_std_object_handlers()->has_dimension(object, member, check_empty);
}

int turboslim_collection_count_elements(zval* object, zend_long* count)
{
    zend_object* zobj = Z_OBJ_P(object);
    collection_t* v   = collection_from_zobj(zobj);

    if (v->opt.fast_count) {
        zval* data = get_data(zobj);
        *count     = zend_hash_num_elements(Z_ARRVAL_P(data));
        return SUCCESS;
    }

    /* Force count() call */
    return FAILURE;
}

zend_object_iterator* turboslim_collection_get_iterator(zend_class_entry* ce, zval* object, int by_ref)
{
    zval iterator;
    zend_object* zobj   = Z_OBJ_P(object);
    zval* data          = get_data(zobj);
    zend_function* ctor = spl_ce_ArrayIterator->constructor;

    object_init_ex(&iterator, spl_ce_ArrayIterator);
    zend_call_method(&iterator, spl_ce_ArrayIterator, &ctor, ZSTR_VAL(ctor->common.function_name), ZSTR_LEN(ctor->common.function_name), NULL, 1, data, NULL);

    if (UNEXPECTED(EG(exception))) {
        zval_ptr_dtor(&iterator);
        return NULL;
    }

    zend_object_iterator* it = spl_ce_ArrayIterator->get_iterator(spl_ce_ArrayIterator, &iterator, by_ref);
    zval_ptr_dtor(&iterator);
    return it;
}

TURBOSLIM_ATTR_NONNULL static void slow_replace(zend_object* zobj, zval* this_ptr, HashTable* items)
{
    zend_hash_key key;
    zval* val;

    zend_function* f = zend_hash_str_find_ptr(&zobj->ce->function_table, ZEND_STRL("set"));
    ZEND_HASH_FOREACH_KEY_VAL(items, key.h, key.key, val) {
        zval k;
        hash_key_to_zval(&k, &key);
        zend_call_method(this_ptr, zobj->ce, &f, ZEND_STRL("set"), NULL, 2, &k, val);
        if (UNEXPECTED(EG(exception))) {
            break;
        }
    } ZEND_HASH_FOREACH_END();
}

TURBOSLIM_ATTR_NONNULL static void construct(zval* this_ptr, HashTable* items)
{
    zend_object* zobj = Z_OBJ_P(this_ptr);
    zval* data        = get_data(zobj);

    if (collection_from_zobj(zobj)->opt.fast_writedim) {
        zend_hash_copy(Z_ARRVAL_P(data), items, zval_add_ref);
    }
    else {
        slow_replace(zobj, this_ptr, items);
    }
}

TURBOSLIM_ATTR_NONNULL static void replace(zval* this_ptr, HashTable* items)
{
    zend_object* zobj = Z_OBJ_P(this_ptr);
    zval* data        = get_data(zobj);
    zend_hash_key key;
    zval* val;

    if (collection_from_zobj(zobj)->opt.fast_writedim) {
        SEPARATE_ARRAY(data);
        ZEND_HASH_FOREACH_KEY_VAL(items, key.h, key.key, val) {
            array_hashkey_update(Z_ARRVAL_P(data), &key, val);
        } ZEND_HASH_FOREACH_END();
    }
    else {
        slow_replace(zobj, this_ptr, items);
    }
}

/*
 * public function __construct(array $items = [])
 */
static PHP_METHOD(TurboSlim_Collection, __construct)
{
    HashTable* items = NULL;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY_HT(items)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    if (items) {
        construct(get_this(execute_data), items);
    }
}

/*
 * public function set($key, $value)
 */
static PHP_METHOD(TurboSlim_Collection, set)
{
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    zval* data        = get_data(zobj);
    zval* key;
    zval* value;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_ZVAL(key)
        Z_PARAM_ZVAL(value)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    SEPARATE_ARRAY(data);
    /* if Z_TYPE_P(key) = IS_NULL, this will use an empty key rather then append */
    array_set_zval_key(Z_ARRVAL_P(data), key, value);
}

/*
 * public function get($key, $default = null)
 */
static PHP_METHOD(TurboSlim_Collection, get)
{
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    zval* data        = get_data(zobj);
    zval* key;
    zval* def = NULL;
    zval* tmp;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ZVAL(key)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(def)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    tmp = array_zval_offset_get(Z_ARRVAL_P(data), key);
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
    HashTable* items;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY_HT(items)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    replace(get_this(execute_data), items);
}

/*
 * public function all()
 */
static PHP_METHOD(TurboSlim_Collection, all)
{
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    zval* data        = get_data(zobj);

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    RETURN_ZVAL(data, 1, 0);
}

/*
 * public function keys()
 */
static PHP_METHOD(TurboSlim_Collection, keys)
{
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    zval* data        = get_data(zobj);

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    int n = zend_hash_num_elements(Z_ARRVAL_P(data));
    array_init_size(return_value, n);
    if (!n) {
        return;
    }

    zend_hash_real_init(Z_ARRVAL_P(return_value), 1);
    ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
        zend_ulong num_idx;
        zend_string *str_idx;
        zval item;

        ZEND_HASH_FOREACH_KEY(Z_ARRVAL_P(data), num_idx, str_idx) {
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
    zval* data        = get_data(zobj);
    zval* key;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(key)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    RETURN_BOOL(array_zval_offset_exists(Z_ARRVAL_P(data), key));
}

/*
 * public function remove($key)
 */
static PHP_METHOD(TurboSlim_Collection, remove)
{
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    zval* data        = get_data(zobj);
    zval* key;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(key)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    SEPARATE_ARRAY(data);
    array_zval_offset_unset(Z_ARRVAL_P(data), key);
}

/*
 * public function clear()
 */
static PHP_METHOD(TurboSlim_Collection, clear)
{
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    zval* data        = get_data(zobj);

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    SEPARATE_ARRAY(data);
    zend_hash_clean(Z_ARRVAL_P(data));
}
/*
 * public function count()
 *
 * This function is not really used, as we have count_elements
 * object handler installed.
 */
static PHP_METHOD(TurboSlim_Collection, count)
{
    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    zval* data     = get_data(Z_OBJ_P(this_ptr));
    RETURN_LONG(zend_hash_num_elements(Z_ARRVAL_P(data)));
}

/*
 * public function offsetGet($key)
 */
static PHP_METHOD(TurboSlim_Collection, offsetGet)
{
    zval* key;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(key)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    if (collection_from_zobj(zobj)->opt.fast_get) {
        zval* data = get_data(zobj);
        zval* res  = read_dimension(data, key, key ? BP_VAR_W : BP_VAR_RW, return_value);
        if (res != return_value) {
            ZVAL_COPY(return_value, res);
        }
    }
    else {
        zend_call_method(this_ptr, zobj->ce, NULL, ZEND_STRL("get"), return_value, 1, key, NULL);
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
        Z_PARAM_ZVAL(key)
        Z_PARAM_ZVAL(value)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    if (collection_from_zobj(zobj)->opt.fast_writedim) {
        zval* data = get_data(zobj);
        write_dimension(data, key, value);
    }
    else {
        zend_call_method(this_ptr, zobj->ce, NULL, ZEND_STRL("set"), NULL, 2, key, value);
    }
}

/*
 * public function getIterator()
 */
static PHP_METHOD(TurboSlim_Collection, getIterator)
{
    zval* this_ptr    = get_this(execute_data);
    zend_object* zobj = Z_OBJ_P(this_ptr);
    zval* data        = get_data(zobj);

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zend_function* ctor = spl_ce_ArrayIterator->constructor;

    object_init_ex(return_value, spl_ce_ArrayIterator);
    zend_call_method(return_value, spl_ce_ArrayIterator, &ctor, ZSTR_VAL(ctor->common.function_name), ZSTR_LEN(ctor->common.function_name), NULL, 1, data, NULL);
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

const zend_function_entry fe_TurboSlim_Collection[] = {
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
    PHP_ME(TurboSlim_Collection, offsetGet,   arginfo_offsetGet,   ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_Collection, offsetSet,   arginfo_kv,          ZEND_ACC_PUBLIC)

    PHP_MALIAS(TurboSlim_Collection, offsetExists, has,    arginfo_key, ZEND_ACC_PUBLIC)
    PHP_MALIAS(TurboSlim_Collection, offsetUnset,  remove, arginfo_key, ZEND_ACC_PUBLIC)

    PHP_FE_END
};

void turboslim_Collection_create(zval* return_value, zend_class_entry* ce, zval* items)
{
    assert(instanceof_function_ex(ce, ce_TurboSlim_Interfaces_CollectionInterface, 1));
    object_init_ex(return_value, ce);
    if (items && Z_TYPE_P(items) == IS_ARRAY) {
        construct(return_value, Z_ARRVAL_P(items));
    }
}

void turboslim_Collection_get(zval* return_value, zval* collection, zval* key)
{
    assert(Z_TYPE_P(collection) == IS_OBJECT);
    assert(instanceof_function_ex(Z_OBJCE_P(collection), ce_TurboSlim_Interfaces_CollectionInterface, 1));

    zend_object* zobj = Z_OBJ_P(collection);
    zval* data        = get_data(zobj);

    zval* tmp = array_zval_offset_get(Z_ARRVAL_P(data), key);
    if (tmp) {
        ZVAL_COPY(return_value, tmp);
    }
    else {
        ZVAL_UNDEF(return_value);
    }
}

void turboslim_Collection_set(zval* collection, zval* key, zval* value)
{
    assert(Z_TYPE_P(collection) == IS_OBJECT);
    assert(instanceof_function_ex(Z_OBJCE_P(collection), ce_TurboSlim_Interfaces_CollectionInterface, 1));

    zend_object* zobj = Z_OBJ_P(collection);
    zval* data        = get_data(zobj);

    SEPARATE_ARRAY(data);
    /* if Z_TYPE_P(key) = IS_NULL, this will use an empty key rather then append */
    array_set_zval_key(Z_ARRVAL_P(data), key, value);
}

void turboslim_Collection_remove(zval* collection, zval* key)
{
    assert(Z_TYPE_P(collection) == IS_OBJECT);
    assert(instanceof_function_ex(Z_OBJCE_P(collection), ce_TurboSlim_Interfaces_CollectionInterface, 1));

    zend_object* zobj = Z_OBJ_P(collection);
    zval* data        = get_data(zobj);

    SEPARATE_ARRAY(data);
    array_zval_offset_unset(Z_ARRVAL_P(data), key);
}

zval* turboslim_Collection_all(zval* collection)
{
    assert(Z_TYPE_P(collection) == IS_OBJECT);
    assert(instanceof_function_ex(Z_OBJCE_P(collection), ce_TurboSlim_Interfaces_CollectionInterface, 1));

    return get_data(Z_OBJ_P(collection));
}
