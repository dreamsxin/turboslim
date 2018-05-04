#ifndef TURBOSLIM_INTERNAL_INTERNALCLASS_H
#define TURBOSLIM_INTERNAL_INTERNALCLASS_H

#include "php_turboslim.h"

TURBOSLIM_VISIBILITY_HIDDEN zend_function* turboslim_deny_construct(zend_object* obj);
TURBOSLIM_VISIBILITY_HIDDEN zval* turboslim_deny_read_property(zval* object, zval* member, int type, void** cache_slot, zval* rv);
TURBOSLIM_VISIBILITY_HIDDEN void turboslim_deny_write_property(zval* object, zval* member, zval* value, void** cache_slot);
TURBOSLIM_VISIBILITY_HIDDEN zval* turboslim_deny_get_property_ptr_ptr(zval* object, zval* member, int type, void** cache_slot);
TURBOSLIM_VISIBILITY_HIDDEN int turboslim_deny_has_property(zval* object, zval* member, int has_set_exists, void** cache_slot);
TURBOSLIM_VISIBILITY_HIDDEN void turboslim_deny_unset_property(zval* object, zval* member, void** cache_slot);

#endif /* TURBOSLIM_INTERNAL_INTERNALCLASS_H */
