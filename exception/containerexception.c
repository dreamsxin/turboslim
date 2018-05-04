#include "containerexception.h"
#include "psr11.h"

#include <ext/spl/spl_exceptions.h>

zend_class_entry* ce_TurboSlim_Exception_ContainerException = NULL;

int init_container_exception()
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Exception\\ContainerException", NULL);
    ce_TurboSlim_Exception_ContainerException = zend_register_internal_class_ex(&ce, spl_ce_InvalidArgumentException);
    zend_class_implements(ce_TurboSlim_Exception_ContainerException, 1, ce_Psr_Container_ContainerExceptionInterface);
    return SUCCESS;
}
