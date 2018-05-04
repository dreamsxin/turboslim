#include "notfoundexception.h"
#include "turboslimexception.h"

zend_class_entry* ce_TurboSlim_Exception_NotFoundException = NULL;

int init_not_found_exception()
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Exception\\NotFoundException", NULL);
    ce_TurboSlim_Exception_TurboSlimException = zend_register_internal_class_ex(&ce, ce_TurboSlim_Exception_TurboSlimException);
    return SUCCESS;
}
