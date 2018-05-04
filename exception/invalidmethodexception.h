#ifndef TURBOSLIM_EXCEPTION_INVALIDMETHODEXCEPTION_H
#define TURBOSLIM_EXCEPTION_INVALIDMETHODEXCEPTION_H

#include "php_turboslim.h"

TURBOSLIM_VISIBILITY_HIDDEN int init_invalid_method_exception();

extern zend_class_entry* ce_TurboSlim_Exception_InvalidMethodException;

#endif /* TURBOSLIM_EXCEPTION_INVALIDMETHODEXCEPTION_H */
