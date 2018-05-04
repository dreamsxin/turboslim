#ifndef TURBOSLIM_EXCEPTION_METHODNOTALLOWEDEXCEPTION_H
#define TURBOSLIM_EXCEPTION_METHODNOTALLOWEDEXCEPTION_H

#include "php_turboslim.h"

TURBOSLIM_VISIBILITY_HIDDEN int init_method_not_allowed_exception();

extern zend_class_entry* ce_TurboSlim_Exception_MethodNotAllowedException;

#endif /* TURBOSLIM_EXCEPTION_METHODNOTALLOWEDEXCEPTION_H */
