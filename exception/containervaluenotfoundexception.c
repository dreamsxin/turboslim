#include "containervaluenotfoundexception.h"
#include "psr11.h"

#include <ext/spl/spl_exceptions.h>

zend_class_entry* ce_TurboSlim_Exception_ContainerValueNotFoundException = NULL;

int init_container_value_not_found_exception()
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Exception\\ContainerValueNotFoundException", NULL);
    ce_TurboSlim_Exception_ContainerValueNotFoundException = zend_register_internal_class_ex(&ce, spl_ce_RuntimeException);
    zend_class_implements(ce_TurboSlim_Exception_ContainerValueNotFoundException, 1, ce_Psr_Container_NotFoundExceptionInterface);
    return SUCCESS;
}
