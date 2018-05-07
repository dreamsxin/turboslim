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

typedef turboslim_collection_t collection_t;

zend_class_entry* ce_TurboSlim_Collection = NULL;
zend_object_handlers turboslim_collection_handlers;

static inline collection_t* collection_from_zobj(const zend_object* obj)
{
    return (collection_t*)((char*)(obj) - XtOffsetOf(collection_t, std));
}

static void handle_inheritance(collection_t* v, zend_class_entry* ce)
{
    if (ce != ce_TurboSlim_Collection) {
        zend_function* f1;
        zend_function* f2;

        f1 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("offsetget"));
        f2 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("get"));
        v->fast_readdim = (f1->common.scope == ce_TurboSlim_Collection && f2->common.scope == ce_TurboSlim_Collection);

        f1 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("offsetset"));
        f2 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("set"));
        v->fast_writedim = (f1->common.scope == ce_TurboSlim_Collection && f2->common.scope == ce_TurboSlim_Collection);

        f1 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("offsetunset"));
        f2 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("remove"));
        v->fast_unsetdim = (f1->common.scope == ce_TurboSlim_Collection && f2->common.scope == ce_TurboSlim_Collection);

        f1 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("offsetexists"));
        f2 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("has"));
        v->fast_hasdim = (f1->common.scope == ce_TurboSlim_Collection && f2->common.scope == ce_TurboSlim_Collection);

        f1 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("count"));
        v->fast_count = (f1->common.scope == ce_TurboSlim_Collection);
    }
    else {
        v->fast_readdim  = 1;
        v->fast_writedim = 1;
        v->fast_hasdim   = 1;
        v->fast_unsetdim = 1;
        v->fast_count    = 1;
    }
}

zend_object* turboslim_collection_create_object(zend_class_entry* ce)
{
    collection_t* v = ecalloc(1, sizeof(collection_t) + zend_object_properties_size(ce));

    zend_object_std_init(&v->std, ce);
    object_properties_init(&v->std, ce);
    v->std.handlers = &turboslim_collection_handlers;

    array_init(&v->data);
    handle_inheritance(v, ce);

    return &v->std;
}

void turboslim_collection_free_obj(zend_object* obj)
{
    collection_t* v = collection_from_zobj(obj);

    zend_object_std_dtor(&v->std);
    zval_ptr_dtor(&v->data);
}

zend_object* turboslim_collection_clone_obj(zval* obj)
{
    zend_object* old_object = Z_OBJ_P(obj);
    zend_object* new_object = turboslim_collection_create_object(old_object->ce);

    collection_t* mine      = collection_from_zobj(old_object);
    collection_t* theirs    = collection_from_zobj(new_object);

    zend_hash_copy(Z_ARRVAL(theirs->data), Z_ARRVAL(mine->data), zval_add_ref);

    zend_objects_clone_members(new_object, old_object);
    return new_object;
}

static zend_bool is_public_property(zval* object, zval* member)
{
    zend_class_entry* ce = Z_OBJCE_P(object);
    if (ce != ce_TurboSlim_Collection) {
        zend_property_info* p = zend_get_property_info(Z_OBJCE_P(object), Z_STR_P(member), 1);
        return p && (p->flags & ZEND_ACC_PUBLIC);
    }

    return 0;
}

zval* turboslim_collection_read_property(zval* object, zval* member, int type, void** cache_slot, zval *rv)
{
    if (Z_TYPE_P(member) == IS_STRING && zend_string_equals_literal(Z_STR_P(member), "data")) {
        zend_class_entry* called_scope = EG(fake_scope) ? EG(fake_scope) : zend_get_executed_scope();
        zend_bool is_public            = is_public_property(object, member);
        if (is_public || zend_check_protected(ce_TurboSlim_Collection, called_scope)) {
            zend_object* zobj = Z_OBJ_P(object);
            collection_t* v   = collection_from_zobj(zobj);
            return &v->data;
        }

        if (!is_public && type != BP_VAR_IS) {
            zend_throw_error(NULL, "Cannot access protected property %s::$%s", ZSTR_VAL(ce_TurboSlim_Collection->name), Z_STRVAL_P(member));
        }

        return &EG(uninitialized_zval);
    }

    return zend_get_std_object_handlers()->read_property(object, member, type, cache_slot, rv);
}

void turboslim_collection_write_property(zval* object, zval* member, zval* value, void** cache_slot)
{
    if (Z_TYPE_P(member) == IS_STRING && zend_string_equals_literal(Z_STR_P(member), "data")) {
        zend_class_entry* called_scope = EG(fake_scope) ? EG(fake_scope) : zend_get_executed_scope();
        zend_bool is_public            = is_public_property(object, member);
        if (is_public || zend_check_protected(ce_TurboSlim_Collection, called_scope)) {
            zend_object* zobj = Z_OBJ_P(object);
            collection_t* v   = collection_from_zobj(zobj);

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

        if (!is_public) {
            zend_throw_error(NULL, "Cannot access protected property %s::$%s", ZSTR_VAL(ce_TurboSlim_Collection->name), Z_STRVAL_P(member));
        }

        return;
    }

    zend_get_std_object_handlers()->write_property(object, member, value, cache_slot);
}

int turboslim_collection_has_property(zval* object, zval* member, int has_set_exists, void** cache_slot)
{
    if (Z_TYPE_P(member) == IS_STRING && zend_string_equals_literal(Z_STR_P(member), "data")) {
        zend_class_entry* called_scope = EG(fake_scope) ? EG(fake_scope) : zend_get_executed_scope();
        zend_bool is_public            = is_public_property(object, member);
        if (is_public || zend_check_protected(ce_TurboSlim_Collection, called_scope)) {
            zend_object* zobj = Z_OBJ_P(object);
            collection_t* v   = collection_from_zobj(zobj);

            switch (has_set_exists) {
                /* HAS: whether property exists and is not NULL */
                case 0:
                /* EXISTS: whether property exists */
                case 2:
                    return 1;
                /* SET: whether property exists and is true */
                default:
                case 1:
                    return zend_hash_num_elements(Z_ARRVAL(v->data)) > 0;
            }
        }
    }

    return zend_get_std_object_handlers()->has_property(object, member, has_set_exists, cache_slot);
}

void turboslim_collection_unset_property(zval* object, zval* member, void** cache_slot)
{
    if (Z_TYPE_P(member) == IS_STRING && zend_string_equals_literal(Z_STR_P(member), "data")) {
        zend_class_entry* called_scope = EG(fake_scope) ? EG(fake_scope) : zend_get_executed_scope();
        zend_bool is_public            = is_public_property(object, member);
        if (is_public || zend_check_protected(ce_TurboSlim_Collection, called_scope)) {
            zend_object* zobj = Z_OBJ_P(object);
            collection_t* v   = collection_from_zobj(zobj);

            zend_hash_clean(Z_ARRVAL(v->data));
            return;
        }

        if (!is_public) {
            zend_throw_error(NULL, "Cannot access protected property %s::$%s", ZSTR_VAL(ce_TurboSlim_Collection->name), Z_STRVAL_P(member));
        }

        return;
    }

    zend_get_std_object_handlers()->unset_property(object, member, cache_slot);
}

static zval* read_dimension(collection_t* v, zval* offset, int type, zval *rv)
{
    zval* z;

    if (offset) {
        z = array_zval_offset_get(Z_ARRVAL(v->data), offset);
    }
    else {
        z      = NULL;
        offset = &znull;
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

zval* turboslim_collection_read_dimension(zval* object, zval* offset, int type, zval *rv)
{
    zend_object* zobj = Z_OBJ_P(object);
    collection_t* v   = collection_from_zobj(zobj);

    if (v->fast_readdim) {
        return read_dimension(v, offset, type, rv);
    }

    return zend_get_std_object_handlers()->read_dimension(object, offset, type, rv);
}

static void write_dimension(collection_t* v, zval* offset, zval* value)
{
    SEPARATE_ARRAY(&v->data);
    if (EXPECTED(offset != NULL)) {
        array_set_zval_key(Z_ARRVAL(v->data), offset, value);
    }
    else {
        Z_TRY_ADDREF_P(value);
        zend_hash_next_index_insert(Z_ARRVAL(v->data), value);
    }
}

void turboslim_collection_write_dimension(zval* object, zval* offset, zval* value)
{
    zend_object* zobj = Z_OBJ_P(object);
    collection_t* v   = collection_from_zobj(zobj);

    if (v->fast_writedim) {
        write_dimension(v, offset, value);
    }
    else {
        zend_get_std_object_handlers()->write_dimension(object, offset, value);
    }
}

void turboslim_collection_unset_dimension(zval* object, zval* offset)
{
    zend_object* zobj = Z_OBJ_P(object);
    collection_t* v   = collection_from_zobj(zobj);

    if (v->fast_unsetdim) {
        SEPARATE_ARRAY(&v->data);
        array_zval_offset_unset(Z_ARRVAL(v->data), offset);
        return;
    }

    zend_get_std_object_handlers()->unset_dimension(object, offset);
}

int turboslim_collection_has_dimension(zval* object, zval* member, int check_empty)
{
    zend_object* zobj = Z_OBJ_P(object);
    collection_t* v   = collection_from_zobj(zobj);

    if (v->fast_hasdim) {
        zval* result = array_zval_offset_get(Z_ARRVAL(v->data), member);

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

    if (v->fast_count) {
        *count = zend_hash_num_elements(Z_ARRVAL(v->data));
        return SUCCESS;
    }

    /* Force count() call */
    return FAILURE;
}

HashTable* turboslim_collection_get_properties(zval* object)
{
    zend_object* zobj = Z_OBJ_P(object);
    collection_t* v   = collection_from_zobj(zobj);
    zval* z;

    HashTable* res = zend_std_get_properties(object);
    if ((z = zend_hash_update(res, str_data, &v->data))) {
        Z_ADDREF_P(z);
    }

    return res;
}

int turboslim_collection_compare_objects(zval* object1, zval* object2)
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

    return zend_get_std_object_handlers()->compare_objects(object1, object2);
}

zend_object_iterator* turboslim_collection_get_iterator(zend_class_entry* ce, zval* object, int by_ref)
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

int turboslim_collection_serialize(zval* object, unsigned char** buffer, size_t* buf_len, zend_serialize_data* data)
{
    zend_object* zobj = Z_OBJ_P(object);
    collection_t* c   = collection_from_zobj(zobj);
    smart_str buf     = { NULL, 0 };
    php_serialize_data_t var_hash = (php_serialize_data_t)data;

    PHP_VAR_SERIALIZE_INIT(var_hash);
        zval z;
        php_var_serialize(&buf, &c->data, &var_hash);

        ZVAL_ARR(&z, zend_std_get_properties(object));
        php_var_serialize(&buf, &z, &var_hash);
    PHP_VAR_SERIALIZE_DESTROY(var_hash);

    *buffer  = (unsigned char*)estrndup(ZSTR_VAL(buf.s), ZSTR_LEN(buf.s));
    *buf_len = ZSTR_LEN(buf.s);
    zend_string_release(buf.s);
    return SUCCESS;
}

int turboslim_collection_unserialize(zval *object, zend_class_entry *ce, const unsigned char *buf, size_t buf_len, zend_unserialize_data *data)
{
    php_unserialize_data_t unserialize_data = (php_unserialize_data_t)data;
    const unsigned char* p   = buf;
    const unsigned char* max = p + buf_len;
    zval* zv;

    PHP_VAR_UNSERIALIZE_INIT(unserialize_data);

    object_init_ex(object, ce);
    zend_object* zobj = Z_OBJ_P(object);
    collection_t* c   = collection_from_zobj(zobj);

    zv = var_tmp_var(&unserialize_data);
    if (!php_var_unserialize(zv, &p, max, &unserialize_data) || Z_TYPE_P(zv) != IS_ARRAY) {
        PHP_VAR_UNSERIALIZE_DESTROY(unserialize_data);
        return FAILURE;
    }

    zend_hash_copy(Z_ARRVAL(c->data), Z_ARRVAL_P(zv), zval_add_ref);

    zv = var_tmp_var(&unserialize_data);
    if (!php_var_unserialize(zv, &p, max, &unserialize_data) || Z_TYPE_P(zv) != IS_ARRAY) {
        PHP_VAR_UNSERIALIZE_DESTROY(unserialize_data);
        return FAILURE;
    }

    object_properties_load(&c->std, Z_ARRVAL_P(zv));
    PHP_VAR_UNSERIALIZE_DESTROY(unserialize_data);
    return SUCCESS;
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
        Z_PARAM_ZVAL(value)
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
        Z_PARAM_ZVAL(def)
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
    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr  = get_this(execute_data);
    collection_t* c = collection_from_zobj(Z_OBJ_P(this_ptr));
    RETURN_LONG(zend_hash_num_elements(Z_ARRVAL(c->data)));
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

    zval* this_ptr  = get_this(execute_data);
    collection_t* c = collection_from_zobj(Z_OBJ_P(this_ptr));
    zval* res       = read_dimension(c, key, key ? BP_VAR_W : BP_VAR_RW, return_value);
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
        Z_PARAM_ZVAL(value)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr  = get_this(execute_data);
    collection_t* c = collection_from_zobj(Z_OBJ_P(this_ptr));
    write_dimension(c, key, value);
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
        zval z;
        php_var_serialize(&buf, &c->data, &var_hash);

        ZVAL_ARR(&z, zend_std_get_properties(this_ptr));
        php_var_serialize(&buf, &z, &var_hash);
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
    PHP_ME(TurboSlim_Collection, serialize,   arginfo_empty,       ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_Collection, unserialize, arginfo_unserialize, ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_Collection, offsetGet,   arginfo_offsetGet,   ZEND_ACC_PUBLIC)
    PHP_ME(TurboSlim_Collection, offsetSet,   arginfo_kv,          ZEND_ACC_PUBLIC)

    PHP_MALIAS(TurboSlim_Collection, offsetExists, has,    arginfo_key, ZEND_ACC_PUBLIC)
    PHP_MALIAS(TurboSlim_Collection, offsetUnset,  remove, arginfo_key, ZEND_ACC_PUBLIC)

    PHP_FE_END
};
