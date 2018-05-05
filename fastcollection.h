#ifndef FASTCOLLECTION_H
#define FASTCOLLECTION_H

#include "php_turboslim.h"

TURBOSLIM_VISIBILITY_HIDDEN int init_fast_collection();
TURBOSLIM_ATTR_NONNULL TURBOSLIM_VISIBILITY_HIDDEN void turboslim_create_FastCollection(zval* return_value, zval* data);

extern zend_class_entry* ce_TurboSlim_FastCollection;

#endif /* FASTCOLLECTION_H */
