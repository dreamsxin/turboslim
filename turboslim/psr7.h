#ifndef TURBOSLIM_PSR7_H
#define TURBOSLIM_PSR7_H

#include "php_turboslim.h"

extern zend_class_entry* ce_Psr_Http_Message_MessageInterface;
extern zend_class_entry* ce_Psr_Http_Message_RequestInterface;
extern zend_class_entry* ce_Psr_Http_Message_ServerRequestInterface;
extern zend_class_entry* ce_Psr_Http_Message_ResponseInterface;
extern zend_class_entry* ce_Psr_Http_Message_StreamInterface;
extern zend_class_entry* ce_Psr_Http_Message_UriInterface;
extern zend_class_entry* ce_Psr_Http_Message_UploadedFileInterface;

TURBOSLIM_VISIBILITY_HIDDEN extern const zend_function_entry fe_Psr_Http_Message_MessageInterface[];
TURBOSLIM_VISIBILITY_HIDDEN extern const zend_function_entry fe_Psr_Http_Message_RequestInterface[];
TURBOSLIM_VISIBILITY_HIDDEN extern const zend_function_entry fe_Psr_Http_Message_ServerRequestInterface[];
TURBOSLIM_VISIBILITY_HIDDEN extern const zend_function_entry fe_Psr_Http_Message_ResponseInterface[];
TURBOSLIM_VISIBILITY_HIDDEN extern const zend_function_entry fe_Psr_Http_Message_StreamInterface[];
TURBOSLIM_VISIBILITY_HIDDEN extern const zend_function_entry fe_Psr_Http_Message_UriInterface[];
TURBOSLIM_VISIBILITY_HIDDEN extern const zend_function_entry fe_Psr_Http_Message_UploadedFileInterface[];

#endif /* TURBOSLIM_PSR7_H */
