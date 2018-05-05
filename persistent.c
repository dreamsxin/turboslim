#include "persistent.h"

#include <Zend/zend_hash.h>

zend_string* str_addContentLengthHeader = NULL;
zend_string* str_allowedMethods         = NULL;
zend_string* str_callable               = NULL;
zend_string* str_callableResolver       = NULL;
zend_string* str_container              = NULL;
zend_string* str_data                   = NULL;
zend_string* str_determineRouteBeforeAppMiddleware = NULL;
zend_string* str_displayErrorDetails    = NULL;
zend_string* str_httpVersion            = NULL;
zend_string* str_outputBuffering        = NULL;
zend_string* str_request                = NULL;
zend_string* str_response               = NULL;
zend_string* str_responseChunkSize      = NULL;
zend_string* str_routerCacheFile        = NULL;
zend_string* str_settings               = NULL;

zend_string* str_callable_pattern       = NULL;
zend_string* str_magic_function         = NULL;

zval znull;
zval zundef;
zval zemptyarr;
zval container_default_settings;

static void init_container_default_settings(HashTable* ht)
{
    zval tmp;
    zend_string* zs;

    zend_hash_init(ht, 8, NULL, ZVAL_PTR_DTOR, 1);

    zs = zend_string_init(ZEND_STRL("1.1"), 1);
    ZVAL_NEW_STR(&tmp, zs);
    _zend_hash_add(ht, str_httpVersion, &tmp ZEND_FILE_LINE_CC);

    ZVAL_LONG(&tmp, 4096);
    _zend_hash_add(ht, str_responseChunkSize, &tmp ZEND_FILE_LINE_CC);

    zs = zend_string_init(ZEND_STRL("append"), 1);
    ZVAL_NEW_STR(&tmp, zs);
    _zend_hash_add(ht, str_outputBuffering, &tmp ZEND_FILE_LINE_CC);

    ZVAL_FALSE(&tmp);
    _zend_hash_add(ht, str_determineRouteBeforeAppMiddleware, &tmp ZEND_FILE_LINE_CC);
    _zend_hash_add(ht, str_displayErrorDetails, &tmp ZEND_FILE_LINE_CC);
    _zend_hash_add(ht, str_routerCacheFile, &tmp ZEND_FILE_LINE_CC);

    ZVAL_TRUE(&tmp);
    _zend_hash_add(ht, str_addContentLengthHeader, &tmp ZEND_FILE_LINE_CC);
}

int init_persistent_data()
{
    str_addContentLengthHeader = zend_new_interned_string(zend_string_init(ZEND_STRL("addContentLengthHeader"), 1));
    str_allowedMethods         = zend_new_interned_string(zend_string_init(ZEND_STRL("allowedMethods"), 1));
    str_callable               = zend_new_interned_string(zend_string_init(ZEND_STRL("callable"), 1));
    str_callableResolver       = zend_new_interned_string(zend_string_init(ZEND_STRL("callableResolver"), 1));
    str_container              = zend_new_interned_string(zend_string_init(ZEND_STRL("container"), 1));
    str_data                   = zend_new_interned_string(zend_string_init(ZEND_STRL("data"), 1));
    str_determineRouteBeforeAppMiddleware = zend_new_interned_string(zend_string_init(ZEND_STRL("determineRouteBeforeAppMiddleware"), 1));
    str_displayErrorDetails    = zend_new_interned_string(zend_string_init(ZEND_STRL("displayErrorDetails"), 1));
    str_httpVersion            = zend_new_interned_string(zend_string_init(ZEND_STRL("httpVersion"), 1));
    str_outputBuffering        = zend_new_interned_string(zend_string_init(ZEND_STRL("outputBuffering"), 1));
    str_request                = zend_new_interned_string(zend_string_init(ZEND_STRL("request"), 1));
    str_response               = zend_new_interned_string(zend_string_init(ZEND_STRL("response"), 1));
    str_responseChunkSize      = zend_new_interned_string(zend_string_init(ZEND_STRL("responseChunkSize"), 1));
    str_routerCacheFile        = zend_new_interned_string(zend_string_init(ZEND_STRL("routerCacheFile"), 1));
    str_settings               = zend_new_interned_string(zend_string_init(ZEND_STRL("settings"), 1));

    str_callable_pattern       = zend_string_init(ZEND_STRL("!^([^\\:]+)\\:([a-zA-Z_\\x7f-\\xff][a-zA-Z0-9_\\x7f-\\xff]*)$!"), 1);
    str_magic_function         = zend_string_init(ZEND_STRL("Magic Function"), 1);

    ZVAL_NULL(&znull);
    ZVAL_UNDEF(&zundef);

    ZVAL_NEW_PERSISTENT_ARR(&zemptyarr);
    zend_hash_init(Z_ARRVAL(zemptyarr), 0, NULL, ZVAL_PTR_DTOR, 1);

    ZVAL_NEW_PERSISTENT_ARR(&container_default_settings);
    init_container_default_settings(Z_ARRVAL(container_default_settings));

    return SUCCESS;
}

int deinit_persistent_data()
{
    zend_string_release(str_addContentLengthHeader);
    zend_string_release(str_allowedMethods);
    zend_string_release(str_callable);
    zend_string_release(str_callableResolver);
    zend_string_release(str_container);
    zend_string_release(str_data);
    zend_string_release(str_determineRouteBeforeAppMiddleware);
    zend_string_release(str_displayErrorDetails);
    zend_string_release(str_httpVersion);
    zend_string_release(str_outputBuffering);
    zend_string_release(str_request);
    zend_string_release(str_response);
    zend_string_release(str_responseChunkSize);
    zend_string_release(str_routerCacheFile);
    zend_string_release(str_settings);

    zend_string_release(str_callable_pattern);
    zend_string_release(str_magic_function);

    zend_hash_destroy(Z_ARRVAL(zemptyarr));
    pefree(Z_ARRVAL(zemptyarr), 1);

    zend_hash_destroy(Z_ARRVAL(container_default_settings));
    pefree(Z_ARRVAL(container_default_settings), 1);

    return SUCCESS;
}
