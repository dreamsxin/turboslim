#include "http/body.h"

#include <Zend/zend_inheritance.h>
#include <Zend/zend_interfaces.h>
#include "http/stream.h"

zend_class_entry* ce_TurboSlim_Http_Body = NULL;

int init_http_body()
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Http\\Body", NULL);
    ce_TurboSlim_Http_Body = zend_register_internal_class(&ce);
    zend_do_inheritance(ce_TurboSlim_Http_Body, ce_TurboSlim_Http_Stream);

    ce_TurboSlim_Http_Stream->serialize   = zend_class_serialize_deny;
    ce_TurboSlim_Http_Stream->unserialize = zend_class_unserialize_deny;

    return SUCCESS;
}
