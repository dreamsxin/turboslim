#include "turboslim/psr11.h"

zend_class_entry* ce_Psr_Container_ContainerInterface;
zend_class_entry* ce_Psr_Container_ContainerExceptionInterface;
zend_class_entry* ce_Psr_Container_NotFoundExceptionInterface;

ZEND_BEGIN_ARG_INFO_EX(arginfo_has_get, 0, 0, 1)
    ZEND_ARG_INFO(0, id)
ZEND_END_ARG_INFO()

const zend_function_entry fe_Psr_Container_ContainerInterface[] = {
    PHP_ABSTRACT_ME(Psr_Container_ContainerInterface, has, arginfo_has_get)
    PHP_ABSTRACT_ME(Psr_Container_ContainerInterface, get, arginfo_has_get)
    PHP_FE_END
};
