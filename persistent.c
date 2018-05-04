#include "persistent.h"

#include <Zend/zend_hash.h>

zend_string* str_allowedMethods   = NULL;
zend_string* str_callable         = NULL;
zend_string* str_callableResolver = NULL;
zend_string* str_container        = NULL;
zend_string* str_data             = NULL;
zend_string* str_request          = NULL;
zend_string* str_response         = NULL;

zend_string* str_callable_pattern = NULL;
zend_string* str_magic_function   = NULL;

int init_persistent_data()
{
    str_allowedMethods   = zend_new_interned_string(zend_string_init(ZEND_STRL("allowedMethods"), 1));
    str_callable         = zend_new_interned_string(zend_string_init(ZEND_STRL("callable"), 1));
    str_callableResolver = zend_new_interned_string(zend_string_init(ZEND_STRL("callableResolver"), 1));
    str_container        = zend_new_interned_string(zend_string_init(ZEND_STRL("container"), 1));
    str_data             = zend_new_interned_string(zend_string_init(ZEND_STRL("data"), 1));
    str_request          = zend_new_interned_string(zend_string_init(ZEND_STRL("request"), 1));
    str_response         = zend_new_interned_string(zend_string_init(ZEND_STRL("response"), 1));

    str_callable_pattern = zend_string_init(ZEND_STRL("!^([^\\:]+)\\:([a-zA-Z_\\x7f-\\xff][a-zA-Z0-9_\\x7f-\\xff]*)$!"), 1);
    str_magic_function   = zend_string_init(ZEND_STRL("Magic Function"), 1);

    return SUCCESS;
}

int deinit_persistent_data()
{
    zend_string_release(str_allowedMethods);
    zend_string_release(str_callable);
    zend_string_release(str_callableResolver);
    zend_string_release(str_container);
    zend_string_release(str_data);
    zend_string_release(str_request);
    zend_string_release(str_response);

    zend_string_release(str_callable_pattern);
    zend_string_release(str_magic_function);

    return SUCCESS;
}
