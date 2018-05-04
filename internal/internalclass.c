#include "internalclass.h"

#include <Zend/zend_exceptions.h>

static void property_error(zend_class_entry* ce)
{
    zend_throw_error(NULL, "%s object cannot have properties", ZSTR_VAL(ce->name));
}

zend_function* turboslim_deny_construct(zend_object* obj)
{
    zend_throw_error(NULL, "Instantiation of %s is not allowed", ZSTR_VAL(obj->ce->name));
    return NULL;
}

zval* turboslim_deny_read_property(zval* object, zval* member, int type, void** cache_slot, zval* rv)
{
    property_error(Z_OBJCE_P(object));
    return &EG(uninitialized_zval);
}

void turboslim_deny_write_property(zval* object, zval* member, zval* value, void** cache_slot)
{
    property_error(Z_OBJCE_P(object));
}

zval* turboslim_deny_get_property_ptr_ptr(zval* object, zval* member, int type, void** cache_slot)
{
    property_error(Z_OBJCE_P(object));
    return NULL;
}

int turboslim_deny_has_property(zval* object, zval* member, int has_set_exists, void** cache_slot)
{
    if (has_set_exists != 2) {
        property_error(Z_OBJCE_P(object));
    }

    return 0;
}

void turboslim_deny_unset_property(zval* object, zval* member, void** cache_slot)
{
    property_error(Z_OBJCE_P(object));
}
