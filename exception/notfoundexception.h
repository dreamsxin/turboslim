#ifndef TURBOSLIM_EXCEPTION_NOTFOUNDEXCEPTION_H
#define TURBOSLIM_EXCEPTION_NOTFOUNDEXCEPTION_H

#include "php_turboslim.h"

TURBOSLIM_VISIBILITY_HIDDEN int init_not_found_exception();

extern zend_class_entry* ce_TurboSlim_Exception_NotFoundException;

#endif /* TURBOSLIM_EXCEPTION_NOTFOUNDEXCEPTION_H */
