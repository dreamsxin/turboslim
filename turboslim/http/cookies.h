#ifndef TURBOSLIM_HTTP_COOKIES_H
#define TURBOSLIM_HTTP_COOKIES_H

#include "php_turboslim.h"

extern zend_class_entry* ce_TurboSlim_Http_Cookies;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_object_handlers turboslim_http_cookies_handlers;
TURBOSLIM_VISIBILITY_HIDDEN extern const zend_function_entry fe_TurboSlim_Http_Cookies[];

TURBOSLIM_VISIBILITY_HIDDEN zend_object* turboslim_http_cookies_create_object(zend_class_entry* ce);
TURBOSLIM_VISIBILITY_HIDDEN zend_object* turboslim_http_cookies_clone_obj(zval* obj);

#endif /* TURBOSLIM_HTTP_COOKIES_H */
