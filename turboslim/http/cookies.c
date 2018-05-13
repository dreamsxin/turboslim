#include "turboslim/http/cookies.h"

#include <Zend/zend_exceptions.h>
#include <Zend/zend_interfaces.h>
#include <Zend/zend_operators.h>
#include <Zend/zend_smart_str.h>
#include <ext/date/php_date.h>
#include <ext/spl/spl_exceptions.h>
#include <ext/standard/url.h>
#include "persistent.h"
#include "utils.h"

zend_class_entry* ce_TurboSlim_Http_Cookies = NULL;
zend_object_handlers turboslim_http_cookies_handlers;

static inline void check_array(zval* z)
{
    if (UNEXPECTED(Z_TYPE_P(z)) != IS_ARRAY) {
        SEPARATE_ZVAL(z);
        convert_to_array_ex(z);
    }
}

static inline zval* get_request_cookies(zend_object* zobj)
{
    return OBJ_PROP_NUM(zobj, 0);
}

static inline zval* get_response_cookies(zend_object* zobj)
{
    return OBJ_PROP_NUM(zobj, 1);
}

static inline zval* get_defaults(zend_object* zobj)
{
    return OBJ_PROP_NUM(zobj, 2);
}

static zend_object* create_no_init(zend_class_entry* ce)
{
    zend_object* v = ecalloc(1, sizeof(zend_object) + zend_object_properties_size(ce));

    zend_object_std_init(v, ce);
    object_properties_init(v, ce);

    v->handlers = &turboslim_http_cookies_handlers;
    return v;
}

zend_object* turboslim_http_cookies_create_object(zend_class_entry* ce)
{
    zend_object* v = create_no_init(ce);

    zval* request_cookies  = get_request_cookies(v);
    zval* response_cookies = get_response_cookies(v);
    zval* defaults         = get_defaults(v);

    array_init(request_cookies);
    array_init(response_cookies);

    /*
     * protected $defaults = [
     *     'value' => '',
     *     'domain' => null,
     *     'hostonly' => null,
     *     'path' => null,
     *     'expires' => null,
     *     'secure' => false,
     *     'httponly' => false,
     *     'samesite' => null
     * ];
     */
    zval zfalse;
    ZVAL_FALSE(&zfalse);

    zval zempty;
    ZVAL_EMPTY_STRING(&zempty);

    array_init_size(defaults, 8);
    Z_TRY_ADDREF(zempty);
    zend_hash_add_new(Z_ARRVAL_P(defaults), TSKSTR(TKS_value),    &zempty);
    zend_hash_add_new(Z_ARRVAL_P(defaults), TSKSTR(TKS_domain),   &EG(uninitialized_zval));
    zend_hash_add_new(Z_ARRVAL_P(defaults), TSKSTR(TKS_hostonly), &EG(uninitialized_zval));
    zend_hash_add_new(Z_ARRVAL_P(defaults), TSKSTR(TKS_path),     &EG(uninitialized_zval));
    zend_hash_add_new(Z_ARRVAL_P(defaults), TSKSTR(TKS_expires),  &EG(uninitialized_zval));
    zend_hash_add_new(Z_ARRVAL_P(defaults), TSKSTR(TKS_secure),   &zfalse);
    zend_hash_add_new(Z_ARRVAL_P(defaults), TSKSTR(TKS_httponly), &zfalse);
    zend_hash_add_new(Z_ARRVAL_P(defaults), TSKSTR(TKS_samesite), &EG(uninitialized_zval));

    return v;
}

zend_object* turboslim_http_cookies_clone_obj(zval* obj)
{
    zend_object* old_object = Z_OBJ_P(obj);
    zend_object* new_object = create_no_init(old_object->ce);

    zend_objects_clone_members(new_object, old_object);
    return new_object;
}

static ZEND_METHOD(TurboSlim_Http_Cookies, __construct)
{
    HashTable* ht = NULL;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY_HT_EX(ht, 1, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    if (ht) {
        zval* this_ptr  = get_this(execute_data);
        zend_object* zo = Z_OBJ_P(this_ptr);
        zval* r         = get_request_cookies(zo);
        check_array(r);

        zend_hash_copy(Z_ARRVAL_P(r), ht, zval_add_ref);
    }
}

static ZEND_METHOD(TurboSlim_Http_Cookies, setDefaults)
{
    HashTable* settings;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY_HT(settings)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr  = get_this(execute_data);
    zend_object* zo = Z_OBJ_P(this_ptr);
    zval* r         = get_defaults(zo);
    check_array(r);

    /* $this->defaults = array_replace($this->defaults, $settings); */
    SEPARATE_ARRAY(r);
    zend_hash_merge(Z_ARRVAL_P(r), settings, zval_add_ref, 1);
}

static ZEND_METHOD(TurboSlim_Http_Cookies, get)
{
    zval* name;
    zval* def = NULL;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ZVAL(name)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL_EX(def, 1, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr  = get_this(execute_data);
    zend_object* zo = Z_OBJ_P(this_ptr);
    zval* r         = get_request_cookies(zo);
    check_array(r);

    zval* z = array_zval_offset_get(Z_ARRVAL_P(r), name);
    if (z) {
        RETURN_ZVAL(z, 1, 0);
    }

    if (def) {
        RETURN_ZVAL(def, 1, 0);
    }

    RETURN_NULL();
}

static ZEND_METHOD(TurboSlim_Http_Cookies, set)
{
    zval* name;
    zval* value;
    zval v;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_ZVAL(name)
        Z_PARAM_ZVAL(value)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    /*
     *  if (!is_array($value)) {
     *      $value = ['value' => (string)$value];
     *  }
     */
    if (Z_TYPE_P(value) != IS_ARRAY) {
        array_init_size(&v, 1);
        if (Z_TYPE_P(value) == IS_STRING) {
            Z_TRY_ADDREF_P(value);
            zend_hash_add_new(Z_ARRVAL(v), TSKSTR(TKS_value), value);
        }
        else {
            zval vstring;
            ZVAL_STR(&vstring, zval_get_string(value));
            zend_hash_add_new(Z_ARRVAL(v), TSKSTR(TKS_value), &vstring);
        }

        value = &v;
    }

    zval* this_ptr  = get_this(execute_data);
    zend_object* zo = Z_OBJ_P(this_ptr);
    zval* d         = get_defaults(zo);
    zval* r         = get_response_cookies(zo);
    check_array(r);
    check_array(d);

    zval tmp;
    array_init_size(&tmp, 8);
    zend_hash_copy(Z_ARRVAL(tmp),  Z_ARRVAL_P(d), zval_add_ref);
    zend_hash_merge(Z_ARRVAL(tmp), Z_ARRVAL_P(value), zval_add_ref, 1);

    SEPARATE_ARRAY(r);
    array_set_zval_key(Z_ARRVAL_P(r), name, &tmp);
    zval_ptr_dtor(&tmp);

    if (UNEXPECTED(value == &v)) {
        zval_ptr_dtor(&v);
    }
}

static ZEND_METHOD(TurboSlim_Http_Cookies, toHeaders)
{
    zend_function* zf_toHeader = NULL;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr  = get_this(execute_data);
    zend_object* zo = Z_OBJ_P(this_ptr);
    zval* r         = get_response_cookies(zo);
    check_array(r);

    array_init_size(return_value, zend_hash_num_elements(Z_ARRVAL_P(r)));
    zend_hash_real_init(Z_ARRVAL_P(return_value), 1);

    ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
        zval* properties;
        zend_string* key;

        ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(r), key, properties) {
            zval header;
            zval rv;
            if (key) {
                ZVAL_STR(&header, key);
                zend_call_method(this_ptr, zo->ce, &zf_toHeader, ZEND_STRL("toheader"), &rv, 2, &header, properties);
                if (UNEXPECTED(EG(exception))) {
                    break;
                }

                ZEND_HASH_FILL_ADD(&rv);
            }
        } ZEND_HASH_FOREACH_END();
    } ZEND_HASH_FILL_END();

    if (UNEXPECTED(EG(exception))) {
        zval_ptr_dtor(return_value);
        RETURN_NULL();
    }
}

static int append_cookie_value(smart_str* out, HashTable* properties)
{
    zval* v = zend_hash_find(properties, TSKSTR(TKS_value));
    if (v) {
        zend_string* s = zval_get_string(v);
        assert(s != NULL);

        if (UNEXPECTED(EG(exception))) {
            zend_string_release(s);
            return 0;
        }

        zend_string* encoded = php_url_encode(ZSTR_VAL(s), ZSTR_LEN(s));
        smart_str_append(out, encoded);
        zend_string_release(encoded);
        zend_string_release(s);
    }

    return 1;
}

static int append_param(smart_str* out, zend_string* key, HashTable* properties)
{
    zval* v = zend_hash_find(properties, key);
    if (v && Z_TYPE_P(v) != IS_NULL && Z_TYPE_P(v) != IS_FALSE) {
        zend_string* s = zval_get_string(v);
        assert(s != NULL);

        if (UNEXPECTED(EG(exception))) {
            zend_string_release(s);
            return 0;
        }

        if (ZSTR_LEN(key) > 0) {
            smart_str_appends(out, "; ");
            smart_str_append(out, key);
            smart_str_appendc(out, '=');
            smart_str_append(out, s);
        }

        zend_string_release(s);
    }

    return 1;
}

static int append_empty_param(smart_str* out, zend_string* key, const char* value, HashTable* properties)
{
    zval* v = zend_hash_find(properties, key);
    if (v && zend_is_true(v)) {
        if (UNEXPECTED(EG(exception))) {
            return 0;
        }

        smart_str_appends(out, "; ");
        smart_str_appends(out, value);
    }

    return 1;
}

static int append_expires(smart_str* out, HashTable* properties)
{
    static zend_function* zf_strtotime = NULL;

    zval* v = zend_hash_find(properties, TSKSTR(TKS_expires));
    if (v) {
        zend_long timestamp;
        /*
         *  if (is_string($properties['expires'])) {
         *      $timestamp = strtotime($properties['expires']);
         *  } else {
         *      $timestamp = (int)$properties['expires'];
         *  }
         */
        if (Z_TYPE_P(v) == IS_STRING) {
            zval rv;
            zend_call_method(NULL, NULL, &zf_strtotime, ZEND_STRL("strtotime"), &rv, 1, v, NULL);
            if (Z_TYPE(rv) == IS_FALSE) {
                /* Malformed time, ignore `expires` */
                return 1;
            }

            timestamp = zval_get_long(&rv);
            maybe_destroy_zval(&rv);
        }
        else {
            timestamp = zval_get_long(v);
        }

        if (UNEXPECTED(EG(exception))) {
            return 0;
        }

        if (timestamp != 0) {
            zend_string* dt = php_format_date(ZEND_STRL("D, d-M-Y H:i:s e"), timestamp, 0);
            smart_str_appends(out, "; expires=");
            smart_str_append(out, dt);
            zend_string_release(dt);
        }
    }

    return 1;
}

static int append_samesite(smart_str* out, HashTable* properties)
{
    zval* v = zend_hash_find(properties, TSKSTR(TKS_samesite));
    if (v) {
        zend_string* s = zval_get_string(v);
        if (UNEXPECTED(EG(exception))) {
            zend_string_release(s);
            return 0;
        }

        if (zend_string_equals_literal_ci(s, "lax") || zend_string_equals_literal_ci(s, "strict")) {
            smart_str_appends(out, "; SameSite=");
            smart_str_append(out, s);
        }

        zend_string_release(s);
    }

    return 1;
}

static ZEND_METHOD(TurboSlim_Http_Cookies, toHeader)
{
    smart_str out = { NULL, 0 };
    char* header;
    size_t header_len;
    HashTable* properties;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(header, header_len)
        Z_PARAM_ARRAY_HT(properties)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zend_string* encoded = php_url_encode(header, header_len);
    smart_str_append(&out, encoded);
    smart_str_appendc(&out, '=');
    zend_string_release(encoded);

    if (
           append_cookie_value(&out, properties)
        && append_param(&out, TSKSTR(TKS_domain), properties)
        && append_param(&out, TSKSTR(TKS_path), properties)
        && append_expires(&out, properties)
        && append_empty_param(&out, TSKSTR(TKS_secure),   "secure", properties)
        && append_empty_param(&out, TSKSTR(TKS_hostonly), "HostOnly", properties)
        && append_empty_param(&out, TSKSTR(TKS_httponly), "HttpOnly", properties)
        && append_samesite(&out, properties)
    ) {
        smart_str_0(&out);
        RETURN_NEW_STR(out.s);
    }

    zend_string_release(out.s);
}

static inline int is_terminator(char c)
{
    return isspace(c) || c == ';';
}

static ZEND_METHOD(TurboSlim_Http_Cookies, parseHeader)
{
    zval* zheader;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(zheader)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    if (Z_TYPE_P(zheader) == IS_ARRAY) {
        zval* h = zend_hash_index_find(Z_ARRVAL_P(zheader), 0);
        if (!h) {
            array_init_size(return_value, 0);
            return;
        }

        zheader = h;
    }

    if (Z_TYPE_P(zheader) != IS_STRING) {
        zend_throw_exception(spl_ce_InvalidArgumentException, "Cannot parse Cookie data. Header value must be a string.", 0);
        return;
    }

    array_init(return_value);

    const char* header_start = Z_STRVAL_P(zheader);
    const char* header_end   = header_start + Z_STRLEN_P(zheader);
    const char* s            = header_start;

    /*
     * The original code:
     *  ```
     *  $header = rtrim($header, "\r\n");
     *  $pieces = preg_split('@[;]\s*@', $header);
     *  ```
     *
     * We don't want to create unnecessary string copies, therefore we work with pointers.
     * Our rtrim() condition is (isspace(c) || ';' == c), as spaces and semicolons should be ignored anyway
     */
    while (header_end > header_start && is_terminator(*(header_end-1))) {
        --header_end;
    }

    /*
     *  $pieces = preg_split('@[;]\s*@', $header);
     *  foreach ($pieces as $cookie) {
     */
    while (s < header_end) {
        char* semicolon = memchr(s, ';', header_end - s);
        size_t n = semicolon ? semicolon - s : header_end - s;
        if (n) {
            /*
             * $cookie = explode('=', $cookie, 2);
             * if (count($cookie) === 2) {
             */
            char* equals = memchr(s, '=', n);

            if (equals) {
                /*
                 * $key   = urldecode($cookie[0]);
                 * $value = urldecode($cookie[1]);
                 */
                zend_string* name = zend_string_init(s, equals - s, 0);
                ZSTR_LEN(name)    = php_url_decode(ZSTR_VAL(name),  ZSTR_LEN(name));

                zend_string* value;
                if (equals + 1 < s + n) {
                    value = zend_string_init(equals + 1, s + n - equals - 1, 0);
                    ZSTR_LEN(value) = php_url_decode(ZSTR_VAL(value), ZSTR_LEN(value));
                }
                else {
                    value = ZSTR_EMPTY_ALLOC();
                }

                /*
                 *  if (!isset($cookies[$key])) {
                 *      $cookies[$key] = $value;
                 *  }
                 */
                if (!zend_hash_exists(Z_ARRVAL_P(return_value), name)) {
                    zval z;
                    ZVAL_STR(&z, value);
                    if (UNEXPECTED(!zend_hash_add_new(Z_ARRVAL_P(return_value), name, &z))) {
                        zend_string_release(value);
                    }
                }

                zend_string_release(name);
            }

            s += n;
        }

        ++s;
        while (s < header_end && isspace(*s)) {
            ++s;
        }
    }
}

ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, ZEND_RETURN_VALUE, 0)
    ZEND_ARG_ARRAY_INFO(0, cookies, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_setdefaults, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_ARRAY_INFO(0, settings, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_get, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, default)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_set, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_empty, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_toheader, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_ARRAY_INFO(0, properties, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_parseheader, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, header)
ZEND_END_ARG_INFO()

const zend_function_entry fe_TurboSlim_Http_Cookies[] = {
    ZEND_ME(TurboSlim_Http_Cookies, __construct, arginfo___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    ZEND_ME(TurboSlim_Http_Cookies, setDefaults, arginfo_setdefaults, ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Cookies, get,         arginfo_get,         ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Cookies, set,         arginfo_set,         ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Cookies, toHeaders,   arginfo_empty,       ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Cookies, toHeader,    arginfo_toheader,    ZEND_ACC_PROTECTED)
    ZEND_ME(TurboSlim_Http_Cookies, parseHeader, arginfo_parseheader, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_FE_END
};
