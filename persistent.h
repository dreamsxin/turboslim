#ifndef TURBOSLIM_PERSISTENT_H
#define TURBOSLIM_PERSISTENT_H

#include "php_turboslim.h"
#include <Zend/zend_string.h>

TURBOSLIM_VISIBILITY_HIDDEN int init_persistent_data();
TURBOSLIM_VISIBILITY_HIDDEN int deinit_persistent_data();

TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_allowedMethods;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_callable;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_callableResolver;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_container;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_data;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_request;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_response;

TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_callable_pattern;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_magic_function;

#endif /* TURBOSLIM_PERSISTENT_H */
