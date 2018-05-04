#ifndef TURBOSLIM_CALLABLEWRAPPER_H
#define TURBOSLIM_CALLABLEWRAPPER_H

#include "php_turboslim.h"

TURBOSLIM_VISIBILITY_HIDDEN int init_callable_wrapper();
TURBOSLIM_VISIBILITY_HIDDEN void create_CallableWrapper(zval* rv, zend_fcall_info* fci, zend_fcall_info_cache* fcc, zval* next);

#endif /* CALLABLEWRAPPER_H */
