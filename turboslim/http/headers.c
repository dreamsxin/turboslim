#include "turboslim/http/headers.h"

#include <Zend/zend_interfaces.h>
#include <ext/standard/php_array.h>
#include <ext/standard/php_string.h>
#include <ext/standard/php_var.h>
#include "turboslim/collection.h"
#include "turboslim/interfaces.h"
#include "functions.h"
#include "persistent.h"
#include "utils.h"

zend_class_entry* ce_TurboSlim_Http_Headers = NULL;

TURBOSLIM_ATTR_NONNULL static void array_lowercase_keys(zval* restrict return_value, zval* restrict arr)
{
    assert(Z_TYPE_P(arr) == IS_ARRAY);

    zend_ulong num_key;
    zend_string* string_key;
    zval* entry;

    array_init_size(return_value, zend_hash_num_elements(Z_ARRVAL_P(arr)));
    ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(arr), num_key, string_key, entry) {
        if (!string_key) {
            entry = zend_hash_index_add_new(Z_ARRVAL_P(return_value), num_key, entry);
        }
        else {
            zend_string* new_key = php_string_tolower(string_key);
            entry = zend_hash_update(Z_ARRVAL_P(return_value), new_key, entry);
            zend_string_release(new_key);
        }

        Z_TRY_ADDREF_P(entry);
    } ZEND_HASH_FOREACH_END();
}

/**
 * @brief `return strtr(strtolower($s), '_', '-');`
 * @param s
 * @return
 */
TURBOSLIM_ATTR_NONNULL TURBOSLIM_ATTR_RETURNS_NONNULL zend_string* lctr(zend_string* s)
{
    char* c = ZSTR_VAL(s);
    char* e = c + ZSTR_LEN(s);

    while (c < e) {
        if (isupper(*c) || *c == '_') {
            zend_string* res = zend_string_alloc(ZSTR_LEN(s), 0);
            if (c != ZSTR_VAL(s)) {
                memcpy(ZSTR_VAL(res), ZSTR_VAL(s), c - ZSTR_VAL(s));
            }

            char* r = c + (ZSTR_VAL(res) - ZSTR_VAL(s));
            while (c < e) {
                if (*c == '_') {
                    *r = '-';
                }
                else {
                    *r = tolower(*c);
                }

                ++r;
                ++c;
            }

            *r = '\0';
            return res;
        }
        else {
            ++c;
        }
    }

    return zend_string_copy(s);
}

TURBOSLIM_ATTR_NONNULL static void normalize_key(zval* return_value, zend_string* key)
{
    zend_string* new_key = lctr(key);

    /* if (strpos($key, 'http-') === 0) { */
    if (ZSTR_LEN(new_key) >= 5 && !memcmp(ZSTR_VAL(new_key), "http-", 5)) {
        /* $key = substr($key, 5); */
        RETVAL_STR(zend_string_init(ZSTR_VAL(new_key) + 5, ZSTR_LEN(new_key) - 5, 0));
        zend_string_release(new_key);
    }
    else {
        RETVAL_STR(new_key);
    }
}

TURBOSLIM_ATTR_NONNULL static void determineAuthorization(zval* environment)
{
    zval auth;
    int fast_path = is_turboslim_class(Z_OBJCE_P(environment));

    zval header;
    ZVAL_STR(&header, TSKSTR(TKS_HTTP_AUTHORIZATION));

    /* $authorization = $environment->get('HTTP_AUTHORIZATION'); */
    if (fast_path) {
        turboslim_Collection_get(&auth, environment, &header);
    }
    else {
        zend_call_method(environment, Z_OBJCE_P(environment), NULL, ZEND_STRL("get"), &auth, 1, &header, NULL);
        if (UNEXPECTED(EG(exception))) {
            return;
        }
    }

    /* if (empty($authorization) && is_callable('getallheaders')) { */
    if (Z_TYPE(auth) == IS_UNDEF || Z_TYPE(auth) == IS_NULL) {
        zval headers;
        zval lcheaders;

        zend_function* func = zend_hash_str_find_ptr(CG(function_table), ZEND_STRL("getallheaders"));
        if (!func) {
            return;
        }

        /* $headers = getallheaders(); */
        zend_call_method(NULL, NULL, &func, ZEND_STRL("getallheaders"), &headers, 0, NULL, NULL);
        if (UNEXPECTED(EG(exception))) {
            return;
        }

        /* $headers = array_change_key_case($headers, CASE_LOWER); */
        array_lowercase_keys(&lcheaders, &headers);
        zval_ptr_dtor(&headers);

        /* if (isset($headers['authorization'])) { */
        zval* z = zend_hash_find(Z_ARRVAL(lcheaders), TSKSTR(TKS_authorization));
        if (z) {
            /* $environment->set('HTTP_AUTHORIZATION', $headers['authorization']); */
            if (fast_path) {
                turboslim_Collection_set(environment, &header, z);
            }
            else {
                zend_call_method(environment, Z_OBJCE_P(environment), NULL, ZEND_STRL("set"), NULL, 2, &header, z);
            }
        }

        zval_ptr_dtor(&lcheaders);
    }
}

static ZEND_METHOD(TurboSlim_Http_Headers, createFromEnvironment)
{
    zval* environment;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(environment, ce_TurboSlim_Interfaces_CollectionInterface)
    ZEND_PARSE_PARAMETERS_END();

    determineAuthorization(environment);
    if (UNEXPECTED(EG(exception))) {
        return;
    }

    zval arr;
    zval z;
    ZVAL_NEW_ARR(&arr);
    zend_hash_init(Z_ARRVAL(arr), 8, NULL, ZVAL_PTR_DTOR, 0);
    ZVAL_LONG(&z, 1);
    _zend_hash_str_add_new(Z_ARRVAL(arr), ZEND_STRL("CONTENT_TYPE"), &z ZEND_FILE_LINE_CC);
    _zend_hash_str_add_new(Z_ARRVAL(arr), ZEND_STRL("CONTENT_LENGTH"), &z ZEND_FILE_LINE_CC);
    _zend_hash_str_add_new(Z_ARRVAL(arr), ZEND_STRL("PHP_AUTH_USER"), &z ZEND_FILE_LINE_CC);
    _zend_hash_str_add_new(Z_ARRVAL(arr), ZEND_STRL("PHP_AUTH_PW"), &z ZEND_FILE_LINE_CC);
    _zend_hash_str_add_new(Z_ARRVAL(arr), ZEND_STRL("PHP_AUTH_DIGEST"), &z ZEND_FILE_LINE_CC);
    _zend_hash_str_add_new(Z_ARRVAL(arr), ZEND_STRL("AUTH_TYPE"), &z ZEND_FILE_LINE_CC);


    zval* special = zend_read_static_property(ce_TurboSlim_Http_Headers, ZEND_STRL("special"), 1);
    if (UNEXPECTED(!special || Z_TYPE_P(special) != IS_ARRAY)) {
        special = &zemptyarr;
        special = &arr;
    }

    turboslim_Collection_create(return_value, ce_TurboSlim_Http_Headers, NULL);
    zend_function* f = NULL;

    zend_object_iterator* it = Z_OBJCE_P(environment)->get_iterator(Z_OBJCE_P(environment), environment, 0);
    if (EXPECTED(it->funcs->valid && it->funcs->get_current_key && it->funcs->get_current_data && it->funcs->move_forward)) {
        if (EXPECTED(it->funcs->rewind)) {
            it->funcs->rewind(it);
        }

        while (!EG(exception) && SUCCESS == it->funcs->valid(it)) {
            zval* value = it->funcs->get_current_data(it);
            zval key;

            it->funcs->get_current_key(it, &key);
            if (!EG(exception)) {
                if (Z_TYPE(key) == IS_STRING) {
                    zend_string* key_upper = php_string_toupper(Z_STR(key));

                    if (
                           (ZSTR_LEN(key_upper) > 5 && !memcmp(ZSTR_VAL(key_upper), "HTTP_", 5) && !zend_string_equals_literal(key_upper, "HTTP_CONTENT_LENGTH"))
                        || zend_hash_exists(Z_ARRVAL_P(special), key_upper)
                    ) {
                        zend_call_method(return_value, ce_TurboSlim_Http_Headers, &f, ZEND_STRL("set"), NULL, 2, &key, value);
                    }

                    zend_string_release(key_upper);
                }

                it->funcs->move_forward(it);
            }

            zval_ptr_dtor(&key);
        }
    }

    zend_iterator_dtor(it);
    zval_ptr_dtor(&arr);
}

static ZEND_METHOD(TurboSlim_Http_Headers, determineAuthorization)
{
    zval* environment;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(environment, ce_TurboSlim_Interfaces_CollectionInterface)
    ZEND_PARSE_PARAMETERS_END();

    determineAuthorization(environment);
    RETURN_ZVAL(environment, 1, 0);
}

static ZEND_METHOD(TurboSlim_Http_Headers, all)
{
    zval* this_ptr = get_this(execute_data);
    zval* all      = turboslim_Collection_all(this_ptr);

    array_init_size(return_value, zend_hash_num_elements(Z_ARRVAL_P(all)));

    zval* entry;
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(all), entry) {
        if (Z_TYPE_P(entry) == IS_ARRAY) {
            zval* orig_key = zend_hash_find(Z_ARRVAL_P(entry), TSKSTR(TKS_originalKey));
            zval* value    = zend_hash_find(Z_ARRVAL_P(entry), TSKSTR(TKS_value));

            if (orig_key && value && Z_TYPE_P(orig_key) == IS_STRING) {
                value = zend_hash_update(Z_ARRVAL_P(return_value), Z_STR_P(orig_key), value);
                if (value) {
                    Z_TRY_ADDREF_P(value);
                }
            }
        }
    } ZEND_HASH_FOREACH_END();
}

static void set(zval* this_ptr, zend_string* key, zval* value)
{
    if (Z_TYPE_P(value) != IS_ARRAY) {
        convert_to_array(value);
    }

    zval v;
    zval k;
    zval* z;
    array_init_size(&v, 2);

    z = zend_hash_add_new(Z_ARRVAL(v), TSKSTR(TKS_value), value);
    if (z) {
        Z_TRY_ADDREF_P(z);
    }

    ZVAL_STR(&k, key);
    z = zend_hash_add_new(Z_ARRVAL(v), TSKSTR(TKS_originalKey), &k);
    if (z) {
        Z_TRY_ADDREF_P(z);
    }

    normalize_key(&k, key);
    turboslim_Collection_set(this_ptr, &k, &v);
    zval_ptr_dtor(&v);
}

static ZEND_METHOD(TurboSlim_Http_Headers, set)
{
    zend_string* key;
    zval* value;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STR(key)
        Z_PARAM_ZVAL(value)
    ZEND_PARSE_PARAMETERS_END();

    set(get_this(execute_data), key, value);
}

static ZEND_METHOD(TurboSlim_Http_Headers, get)
{
    zend_string* key;
    zval* def = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_STR(key)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL_EX(def, 1, 0)
    ZEND_PARSE_PARAMETERS_END();

    zval normalized;
    normalize_key(&normalized, key);

    zval z;
    zval* v;
    zval* this_ptr = get_this(execute_data);
    turboslim_Collection_get(&z, this_ptr, &normalized);
    zval_ptr_dtor(&normalized);
    if (Z_TYPE(z) != IS_ARRAY || (v = zend_hash_find(Z_ARRVAL(z), TSKSTR(TKS_value))) == NULL) {
        if (def) {
            RETURN_ZVAL(def, 1, 0);
        }

        RETURN_NULL();
    }

    RETURN_ZVAL(v, 1, 0);
}

static ZEND_METHOD(TurboSlim_Http_Headers, getOriginalKey)
{
    zend_string* key;
    zval* def = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_STR(key)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL_EX(def, 1, 0)
    ZEND_PARSE_PARAMETERS_END();

    zval normalized;
    normalize_key(&normalized, key);

    zval z;
    zval* v;
    zval* this_ptr = get_this(execute_data);
    turboslim_Collection_get(&z, this_ptr, &normalized);
    zval_ptr_dtor(&normalized);
    if (Z_TYPE(z) != IS_ARRAY || (v = zend_hash_find(Z_ARRVAL(z), TSKSTR(TKS_originalKey))) == NULL) {
        if (def) {
            RETURN_ZVAL(def, 1, 0);
        }

        RETURN_NULL();
    }

    RETURN_ZVAL(v, 1, 0);
}

static ZEND_METHOD(TurboSlim_Http_Headers, add)
{
    zend_string* key;
    zval* value;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STR(key)
        Z_PARAM_ZVAL(value)
    ZEND_PARSE_PARAMETERS_END();

    zval normalized;
    normalize_key(&normalized, key);

    zval z;
    zval* old;
    zval* this_ptr = get_this(execute_data);

    /* $oldValues = $this->get($key, []); */
    turboslim_Collection_get(&z, this_ptr, &normalized);
    if (Z_TYPE(z) != IS_ARRAY || (old = zend_hash_find(Z_ARRVAL(z), TSKSTR(TKS_value))) == NULL) {
        old = NULL;
    }

    /* $newValues = is_array($value) ? $value : [$value]; */
    zval new_values;
    if (Z_TYPE_P(value) != IS_ARRAY) {
        ZVAL_COPY(&new_values, value);
        convert_to_array(&new_values);
    }
    else {
        turboslim_array_values(&new_values, value);
    }

    /* $this->set($key, array_merge($oldValues, array_values($newValues))); */
    zval old_values;
    if (old) {
        array_init_size(&old_values, zend_hash_num_elements(Z_ARRVAL_P(old)) + zend_hash_num_elements(Z_ARRVAL(new_values)));
        zend_hash_copy(Z_ARRVAL(old_values), Z_ARRVAL_P(old), zval_add_ref);
        php_array_merge(Z_ARRVAL(old_values), Z_ARRVAL(new_values));
    }
    else {
        ZVAL_COPY(&old_values, &new_values);
    }

    set(this_ptr, Z_STR(normalized), &old_values);
    zval_ptr_dtor(&normalized);
    zval_ptr_dtor(&old_values);
    zval_ptr_dtor(&new_values);
}

static ZEND_METHOD(TurboSlim_Http_Headers, has)
{
    zend_string* key;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STR(key)
    ZEND_PARSE_PARAMETERS_END();

    zval normalized;
    normalize_key(&normalized, key);

    zval z;
    zval* this_ptr = get_this(execute_data);
    turboslim_Collection_get(&z, this_ptr, &normalized);
    zval_ptr_dtor(&normalized);
    RETURN_BOOL(Z_TYPE(z) == IS_ARRAY);
}

static ZEND_METHOD(TurboSlim_Http_Headers, remove)
{
    zend_string* key;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STR(key)
    ZEND_PARSE_PARAMETERS_END();

    zval normalized;
    normalize_key(&normalized, key);

    zval* this_ptr = get_this(execute_data);
    turboslim_Collection_remove(this_ptr, &normalized);
    zval_ptr_dtor(&normalized);
}

static ZEND_METHOD(TurboSlim_Http_Headers, normalizeKey)
{
    zend_string* key;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STR(key)
    ZEND_PARSE_PARAMETERS_END();

    normalize_key(return_value, key);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_createfromenv, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_OBJ_INFO(0, environment, TurboSlim\\Interfaces\\CollectionInterface, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_empty, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_key, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_kv, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_kd, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, default)
ZEND_END_ARG_INFO()

const zend_function_entry fe_TurboSlim_Http_Headers[] = {
    ZEND_ME(TurboSlim_Http_Headers, createFromEnvironment,  arginfo_createfromenv, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_ME(TurboSlim_Http_Headers, determineAuthorization, arginfo_createfromenv, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_ME(TurboSlim_Http_Headers, all,                    arginfo_empty,         ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Headers, set,                    arginfo_kv,            ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Headers, get,                    arginfo_kd,            ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Headers, getOriginalKey,         arginfo_kv,            ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Headers, add,                    arginfo_kv,            ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Headers, has,                    arginfo_key,           ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Headers, remove,                 arginfo_key,           ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Headers, normalizeKey,           arginfo_key,           ZEND_ACC_PUBLIC)
    ZEND_FE_END
};
