#include "persistent.h"

#include <Zend/zend_hash.h>

zend_string** turboslim_known_strings = NULL;

zend_string* str_callable_pattern       = NULL;
zend_string* str_magic_function         = NULL;

zval znull;
zval zundef;
zval zemptyarr;
zval container_default_settings;

static const char* known_strings[] = {
#define _TURBOSLIM_STR_DSC(id, str) str,
    TURBOSLIM_KNOWN_STRINGS(_TURBOSLIM_STR_DSC)
#undef _TURBOSLIM_STR_DSC
    NULL
};

static void init_container_default_settings(HashTable* ht)
{
    zval tmp;
    zend_string* zs;

    zend_hash_init(ht, 8, NULL, ZVAL_PTR_DTOR, 1);

    zs = zend_string_init(ZEND_STRL("1.1"), 1);
    ZVAL_NEW_STR(&tmp, zs);
    _zend_hash_add_new(ht, TSKSTR(TKS_httpVersion), &tmp ZEND_FILE_LINE_CC);

    ZVAL_LONG(&tmp, 4096);
    _zend_hash_add_new(ht, TSKSTR(TKS_responseChunkSize), &tmp ZEND_FILE_LINE_CC);

    zs = zend_string_init(ZEND_STRL("append"), 1);
    ZVAL_NEW_STR(&tmp, zs);
    _zend_hash_add_new(ht, TSKSTR(TKS_outputBuffering), &tmp ZEND_FILE_LINE_CC);

    ZVAL_FALSE(&tmp);
    _zend_hash_add_new(ht, TSKSTR(TKS_determineRouteBeforeAppMiddleware), &tmp ZEND_FILE_LINE_CC);
    _zend_hash_add_new(ht, TSKSTR(TKS_displayErrorDetails), &tmp ZEND_FILE_LINE_CC);
    _zend_hash_add_new(ht, TSKSTR(TKS_routerCacheFile), &tmp ZEND_FILE_LINE_CC);

    ZVAL_TRUE(&tmp);
    _zend_hash_add_new(ht, TSKSTR(TKS_addContentLengthHeader), &tmp ZEND_FILE_LINE_CC);
}

static void init_strings(const char** strings, size_t count)
{
    turboslim_known_strings = pecalloc(count, sizeof(zend_string*), 1);
    for (size_t i=0; i<count; ++i) {
        turboslim_known_strings[i] = zend_new_interned_string(zend_string_init(strings[i], strlen(strings[i]), 1));
    }
}

int init_persistent_data()
{
    init_strings(known_strings, (sizeof(known_strings) / sizeof(known_strings[0])) - 1);

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
    for (size_t i=0; i<(sizeof(known_strings) / sizeof(known_strings[0])) - 1; ++i) {
        zend_string_release(turboslim_known_strings[i]);
    }

    pefree(turboslim_known_strings, 1);

    zend_string_release(str_callable_pattern);
    zend_string_release(str_magic_function);

    zend_hash_destroy(Z_ARRVAL(zemptyarr));
    pefree(Z_ARRVAL(zemptyarr), 1);

    zend_hash_destroy(Z_ARRVAL(container_default_settings));
    pefree(Z_ARRVAL(container_default_settings), 1);

    return SUCCESS;
}
