#include "psr11.h"

#include <Zend/zend_inheritance.h>

zend_class_entry* ce_Psr_Container_ContainerInterface;
zend_class_entry* ce_Psr_Container_ContainerExceptionInterface;
zend_class_entry* ce_Psr_Container_NotFoundExceptionInterface;

ZEND_BEGIN_ARG_INFO_EX(arginfo_has_get, 0, 0, 1)
    ZEND_ARG_INFO(0, id)
ZEND_END_ARG_INFO()

static const zend_function_entry fe_psr_container_containerinterface[] = {
    PHP_ABSTRACT_ME(Psr_Container_ContainerInterface, has, arginfo_has_get)
    PHP_ABSTRACT_ME(Psr_Container_ContainerInterface, get, arginfo_has_get)
    PHP_FE_END
};

int maybe_init_psr11()
{
    zend_class_entry ce;

    ce_Psr_Container_ContainerInterface = zend_hash_str_find_ptr(CG(class_table), ZEND_STRL("psr\\container\\containerinterface"));
    if (!ce_Psr_Container_ContainerInterface) {
        INIT_CLASS_ENTRY(ce, "Psr\\Container\\ContainerInterface", fe_psr_container_containerinterface);
        ce_Psr_Container_ContainerInterface = zend_register_internal_interface(&ce);
    }

    ce_Psr_Container_ContainerExceptionInterface = zend_hash_str_find_ptr(CG(class_table), ZEND_STRL("psr\\container\\containerexceptioninterface"));
    if (!ce_Psr_Container_ContainerExceptionInterface) {
        INIT_CLASS_ENTRY(ce, "Psr\\Container\\ContainerExceptionInterface", NULL);
        ce_Psr_Container_ContainerExceptionInterface = zend_register_internal_interface(&ce);
    }

    ce_Psr_Container_NotFoundExceptionInterface = zend_hash_str_find_ptr(CG(class_table), ZEND_STRL("psr\\container\\notfoundexceptioninterface"));
    if (!ce_Psr_Container_NotFoundExceptionInterface) {
        INIT_CLASS_ENTRY(ce, "Psr\\Container\\NotFoundExceptionInterface", NULL);
        ce_Psr_Container_NotFoundExceptionInterface = zend_register_internal_interface(&ce);
        zend_do_inheritance(ce_Psr_Container_NotFoundExceptionInterface, ce_Psr_Container_ContainerExceptionInterface);
    }

    return SUCCESS;
}
