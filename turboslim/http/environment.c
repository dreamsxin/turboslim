#include "turboslim/http/environment.h"

#include <Zend/zend_interfaces.h>
#include <ext/standard/php_array.h>
#include "utils.h"

zend_class_entry* ce_TurboSlim_Http_Environment;

/*
 * $https = (isset($ht['HTTPS']) && $ht['HTTPS'] !== 'off') || (isset($ht['REQUEST_SCHEME']) && $ht['REQUEST_SCHEME'] === 'https')
 */
static zend_bool is_https(HashTable* ht)
{
    zval* x = zend_hash_str_find(ht, ZEND_STRL("HTTPS"));
    if (x && Z_TYPE_P(x) == IS_STRING && !zend_string_equals_literal(Z_STR_P(x), "off")) {
        return 1;
    }

    x = zend_hash_str_find(ht, ZEND_STRL("REQUEST_SCHEME"));
    if (x && Z_TYPE_P(x) == IS_STRING && zend_string_equals_literal(Z_STR_P(x), "https")) {
        return 1;
    }

    return 0;
}

/*
 *  [
 *      'SERVER_PROTOCOL'      => 'HTTP/1.1',
 *      'REQUEST_METHOD'       => 'GET',
 *      'REQUEST_SCHEME'       => $def_scheme,
 *      'SCRIPT_NAME'          => '',
 *      'REQUEST_URI'          => '',
 *      'QUERY_STRING'         => '',
 *      'SERVER_NAME'          => 'localhost',
 *      'SERVER_PORT'          => $def_port,
 *      'HTTP_HOST'            => 'localhost',
 *      'HTTP_ACCEPT'          => 'text/html,application/xhtml+xml,application/xml;q=0.9,* / *;q=0.8',
 *      'HTTP_ACCEPT_LANGUAGE' => 'en-US,en;q=0.8',
 *      'HTTP_ACCEPT_CHARSET'  => 'ISO-8859-1,utf-8;q=0.7,*;q=0.3',
 *      'HTTP_USER_AGENT'      => 'Slim Framework',
 *      'REMOTE_ADDR'          => '127.0.0.1',
 *      'REQUEST_TIME'         => time(),
 *      'REQUEST_TIME_FLOAT'   => microtime(true),
 *  ]
 */
static void populate_defaults(zval* data, int def_port, const char* def_scheme)
{
    zval z;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    double dnow    = (double)(tv.tv_sec + tv.tv_usec / 1E6);    /* REQUEST_TIME_FLOAT */
    zend_long inow = tv.tv_sec;                                 /* REQUEST_TIME       */

    array_init_size(data, 16);
    ZVAL_STRING(&z, "HTTP/1.1");
    _zend_hash_str_add(Z_ARRVAL_P(data), ZEND_STRL("SERVER_PROTOCOL"), &z ZEND_FILE_LINE_CC);
    ZVAL_STRING(&z, "GET");
    _zend_hash_str_add(Z_ARRVAL_P(data), ZEND_STRL("REQUEST_METHOD"), &z ZEND_FILE_LINE_CC);
    ZVAL_STRING(&z, def_scheme);
    _zend_hash_str_add(Z_ARRVAL_P(data), ZEND_STRL("REQUEST_SCHEME"), &z ZEND_FILE_LINE_CC);
    ZVAL_EMPTY_STRING(&z);
    assert(ZSTR_IS_INTERNED(Z_STR(z)));
    _zend_hash_str_add(Z_ARRVAL_P(data), ZEND_STRL("SCRIPT_NAME"), &z ZEND_FILE_LINE_CC);
    _zend_hash_str_add(Z_ARRVAL_P(data), ZEND_STRL("REQUEST_URI"), &z ZEND_FILE_LINE_CC);
    _zend_hash_str_add(Z_ARRVAL_P(data), ZEND_STRL("QUERY_STRING"), &z ZEND_FILE_LINE_CC);
    ZVAL_STRING(&z, "localhost");
    _zend_hash_str_add(Z_ARRVAL_P(data), ZEND_STRL("SERVER_NAME"), &z ZEND_FILE_LINE_CC);
    Z_ADDREF(z);
    _zend_hash_str_add(Z_ARRVAL_P(data), ZEND_STRL("HTTP_HOST"), &z ZEND_FILE_LINE_CC);
    ZVAL_LONG(&z, def_port);
    _zend_hash_str_add(Z_ARRVAL_P(data), ZEND_STRL("SERVER_PORT"), &z ZEND_FILE_LINE_CC);
    ZVAL_STRING(&z, "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    _zend_hash_str_add(Z_ARRVAL_P(data), ZEND_STRL("HTTP_ACCEPT"), &z ZEND_FILE_LINE_CC);
    ZVAL_STRING(&z, "en-US,en;q=0.8");
    _zend_hash_str_add(Z_ARRVAL_P(data), ZEND_STRL("HTTP_ACCEPT_LANGUAGE"), &z ZEND_FILE_LINE_CC);
    ZVAL_STRING(&z, "ISO-8859-1,utf-8;q=0.7,*;q=0.3");
    _zend_hash_str_add(Z_ARRVAL_P(data), ZEND_STRL("HTTP_ACCEPT_CHARSET"), &z ZEND_FILE_LINE_CC);
    ZVAL_STRING(&z, "Slim Framework");
    _zend_hash_str_add(Z_ARRVAL_P(data), ZEND_STRL("HTTP_USER_AGENT"), &z ZEND_FILE_LINE_CC);
    ZVAL_STRING(&z, "127.0.0.1");
    _zend_hash_str_add(Z_ARRVAL_P(data), ZEND_STRL("REMOTE_ADDR"), &z ZEND_FILE_LINE_CC);
    ZVAL_LONG(&z, inow);
    _zend_hash_str_add(Z_ARRVAL_P(data), ZEND_STRL("REQUEST_TIME"), &z ZEND_FILE_LINE_CC);
    ZVAL_DOUBLE(&z, dnow);
    _zend_hash_str_add(Z_ARRVAL_P(data), ZEND_STRL("REQUEST_TIME_FLOAT"), &z ZEND_FILE_LINE_CC);

}

static ZEND_METHOD(TurboSlim_Http_Environment, mock)
{
    zval* user_data = NULL;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL_EX(user_data, 1, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    int def_port           = 80;     /* SERVER_ADDR    */
    const char* def_scheme = "http"; /* REQUEST_SCHEME */

    if (user_data) {
        assert(Z_TYPE_P(user_data) == IS_ARRAY);
        if (is_https(Z_ARRVAL_P(user_data))) {
            def_port   = 443;
            def_scheme = "https";
        }
    }

    zval data;
    populate_defaults(&data, def_port, def_scheme);

    /* $data = array_merge($data, $user_data); */
    if (user_data) {
        php_array_merge(Z_ARRVAL(data), Z_ARRVAL_P(user_data));
    }

    /* return new static($data) */
    assert(EG(current_execute_data) == execute_data);
    zend_class_entry* called_scope = zend_get_called_scope(execute_data);
    if (EXPECTED(called_scope != NULL)) {
        zend_function* ctor = called_scope->constructor;
        assert(ctor != NULL);

        object_init_ex(return_value, called_scope);
        zend_call_method(return_value, called_scope, &ctor, ZSTR_VAL(ctor->common.function_name), ZSTR_LEN(ctor->common.function_name), NULL, 1, &data, NULL);
        zval_ptr_dtor(&data);
        return;
    }

    zval_ptr_dtor(&data);
    zend_error(E_ERROR, "cannot access static:: when no class scope is active");
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_mock, 0, ZEND_RETURN_VALUE, 0)
    ZEND_ARG_ARRAY_INFO(0, userData, 0)
ZEND_END_ARG_INFO()

const zend_function_entry fe_TurboSlim_Http_Environment[] = {
    ZEND_ME(TurboSlim_Http_Environment, mock, arginfo_mock, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_FE_END
};
