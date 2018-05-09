#ifndef TURBOSLIM_HTTP_COOKIES_H
#define TURBOSLIM_HTTP_COOKIES_H

#include "php_turboslim.h"

typedef struct turboslim_http_cookies {
    zval* request_cookies;
    zval* response_cookies;
    zval* defaults;
    zend_object std;
} turboslim_http_cookies_t;

extern zend_class_entry* ce_TurboSlim_Http_Cookies;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_object_handlers turboslim_http_cookies_handlers;
TURBOSLIM_VISIBILITY_HIDDEN extern const zend_function_entry fe_TurboSlim_Http_Cookies[];

zend_object* turboslim_http_cookies_create_object(zend_class_entry* ce);
void turboslim_http_cookies_free_obj(zend_object* obj);
zend_object* turboslim_http_cookies_clone_obj(zval* obj);
zval* turboslim_http_stream_cookies_read_property(zval* object, zval* member, int type, void** cache_slot, zval* rv);
int turboslim_http_cookies_has_property(zval* object, zval* member, int has_set_exists, void** cache_slot);
HashTable* turboslim_http_cookies_get_properties(zval* object);
HashTable* turboslim_http_cookies_get_gc(zval* object, zval** table, int* n);
int turboslim_http_cookies_compare_objects(zval* z1, zval* z2);

#endif /* TURBOSLIM_HTTP_COOKIES_H */
