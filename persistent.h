#ifndef TURBOSLIM_PERSISTENT_H
#define TURBOSLIM_PERSISTENT_H

#include "php_turboslim.h"
#include <Zend/zend_string.h>

TURBOSLIM_VISIBILITY_HIDDEN int init_persistent_data();
TURBOSLIM_VISIBILITY_HIDDEN int deinit_persistent_data();

TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_addContentLengthHeader;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_allowedMethods;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_callable;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_callableResolver;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_container;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_data;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_determineRouteBeforeAppMiddleware;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_displayErrorDetails;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_httpVersion;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_outputBuffering;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_request;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_response;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_responseChunkSize;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_routerCacheFile;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_settings;

TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_callable_pattern;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_magic_function;

TURBOSLIM_VISIBILITY_HIDDEN extern zval znull;
TURBOSLIM_VISIBILITY_HIDDEN extern zval zundef;
TURBOSLIM_VISIBILITY_HIDDEN extern zval zemptyarr;

TURBOSLIM_VISIBILITY_HIDDEN extern zval container_default_settings;

#endif /* TURBOSLIM_PERSISTENT_H */
