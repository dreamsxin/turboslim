#ifndef TURBOSLIM_PSR7_H
#define TURBOSLIM_PSR7_H

#include "php_turboslim.h"

TURBOSLIM_VISIBILITY_HIDDEN int maybe_init_psr7();

extern zend_class_entry* ce_Psr_Http_Message_MessageInterface;
extern zend_class_entry* ce_Psr_Http_Message_RequestInterface;
extern zend_class_entry* ce_Psr_Http_Message_ServerRequestInterface;
extern zend_class_entry* ce_Psr_Http_Message_ResponseInterface;
extern zend_class_entry* ce_Psr_Http_Message_StreamInterface;
extern zend_class_entry* ce_Psr_Http_Message_UriInterface;
extern zend_class_entry* ce_Psr_Http_Message_UploadedFileInterface;

#endif /* TURBOSLIM_PSR7_H */
