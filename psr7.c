#include "psr7.h"

#include <Zend/zend_inheritance.h>

zend_class_entry* ce_Psr_Http_Message_MessageInterface       = NULL;
zend_class_entry* ce_Psr_Http_Message_RequestInterface       = NULL;
zend_class_entry* ce_Psr_Http_Message_ServerRequestInterface = NULL;
zend_class_entry* ce_Psr_Http_Message_ResponseInterface      = NULL;
zend_class_entry* ce_Psr_Http_Message_StreamInterface        = NULL;
zend_class_entry* ce_Psr_Http_Message_UriInterface           = NULL;
zend_class_entry* ce_Psr_Http_Message_UploadedFileInterface  = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_empty, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_name, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_nv, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_nd, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, default)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_withProtocolVersion, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, version)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_withBody, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_OBJ_INFO(0, body, Psr\\Http\\Message\\StreamInterface, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_withRequestTraget, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, requestTraget)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_withMethod, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, method)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_withUri, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_OBJ_INFO(0, body, Psr\\Http\\Message\\UriInterface, 0)
    ZEND_ARG_INFO(0, preserveHost)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_withCookieParams, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_ARRAY_INFO(0, cookies, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_withQueryParams, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_ARRAY_INFO(0, query, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_withUploadedFiles, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_ARRAY_INFO(0, uploadedFiles, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_withParsedBody, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_withStatus, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, code)
    ZEND_ARG_INFO(0, reasonPhrase)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_seek, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, offset)
    ZEND_ARG_INFO(0, whence)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_write, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, string)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_read, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_getMetadata, 0, ZEND_RETURN_VALUE, 0)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_withScheme, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, scheme)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_withUserInfo, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, user)
    ZEND_ARG_INFO(0, $password)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_withHost, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, host)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_withPort, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_withPath, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_withQuery, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, query)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_withFragment, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, fragment)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_moveTo, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, targetPath)
ZEND_END_ARG_INFO()

/**
 * @see https://www.php-fig.org/psr/psr-7/#31-psrhttpmessagemessageinterface
 *
 * interface MessageInterface
 * {
 *     public function getProtocolVersion();
 *     public function withProtocolVersion($version);
 *     public function getHeaders();
 *     public function hasHeader($name);
 *     public function getHeader($name);
 *     public function getHeaderLine($name);
 *     public function withHeader($name, $value);
 *     public function withAddedHeader($name, $value);
 *     public function withoutHeader($name);
 *     public function getBody();
 *     public function withBody(StreamInterface $body);
 * }
 */
static const zend_function_entry fe_Psr_Http_Message_MessageInterface[] = {
    PHP_ABSTRACT_ME(Psr_Http_Message_MessageInterface, getProtocolVersion,  arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_MessageInterface, withProtocolVersion, arginfo_withProtocolVersion)
    PHP_ABSTRACT_ME(Psr_Http_Message_MessageInterface, getHeaders,          arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_MessageInterface, hasHeader,           arginfo_name)
    PHP_ABSTRACT_ME(Psr_Http_Message_MessageInterface, getHeader,           arginfo_name)
    PHP_ABSTRACT_ME(Psr_Http_Message_MessageInterface, hasHeaderLine,       arginfo_name)
    PHP_ABSTRACT_ME(Psr_Http_Message_MessageInterface, withHeader,          arginfo_nv)
    PHP_ABSTRACT_ME(Psr_Http_Message_MessageInterface, withoutHeader,       arginfo_name)
    PHP_ABSTRACT_ME(Psr_Http_Message_MessageInterface, getBody,             arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_MessageInterface, withBody,            arginfo_withBody)
    PHP_FE_END
};

/**
 * @see https://www.php-fig.org/psr/psr-7/#32-psrhttpmessagerequestinterface
 *
 * interface RequestInterface extends MessageInterface
 * {
 *     public function getRequestTarget();
 *     public function withRequestTarget($requestTarget);
 *     public function getMethod();
 *     public function withMethod($method);
 *     public function getUri();
 *     public function withUri(UriInterface $uri, $preserveHost = false);
 * }
 */
static const zend_function_entry fe_Psr_Http_Message_RequestInterface[] = {
    PHP_ABSTRACT_ME(Psr_Http_Message_RequestInterface, getRequestTarget,  arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_RequestInterface, withRequestTraget, arginfo_withRequestTraget)
    PHP_ABSTRACT_ME(Psr_Http_Message_RequestInterface, getMethod,         arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_RequestInterface, withMethod,        arginfo_withMethod)
    PHP_ABSTRACT_ME(Psr_Http_Message_RequestInterface, getUri,            arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_RequestInterface, withUri,           arginfo_withUri)
    PHP_FE_END
};

/**
 * @see https://www.php-fig.org/psr/psr-7/#321-psrhttpmessageserverrequestinterface
 *
 * interface ServerRequestInterface extends RequestInterface
 * {
 *     public function getServerParams();
 *     public function getCookieParams();
 *     public function withCookieParams(array $cookies);
 *     public function getQueryParams();
 *     public function withQueryParams(array $query);
 *     public function getUploadedFiles();
 *     public function withUploadedFiles(array $uploadedFiles);
 *     public function getParsedBody();
 *     public function withParsedBody($data);
 *     public function getAttributes();
 *     public function getAttribute($name, $default = null);
 *     public function withAttribute($name, $value);
 *     public function withoutAttribute($name);
 * }
 */
static const zend_function_entry fe_Psr_Http_Message_ServerRequestInterface[] = {
    PHP_ABSTRACT_ME(Psr_Http_Message_ServerRequestInterface, getServerParams,   arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_ServerRequestInterface, getCookieParams,   arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_ServerRequestInterface, withCookieParams,  arginfo_withCookieParams)
    PHP_ABSTRACT_ME(Psr_Http_Message_ServerRequestInterface, getQueryParams,    arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_ServerRequestInterface, withQueryParams,   arginfo_withQueryParams)
    PHP_ABSTRACT_ME(Psr_Http_Message_ServerRequestInterface, getUploadedFiles,  arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_ServerRequestInterface, withUploadedFiles, arginfo_withUploadedFiles)
    PHP_ABSTRACT_ME(Psr_Http_Message_ServerRequestInterface, getParsedBody,     arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_ServerRequestInterface, withParsedBody,    arginfo_withParsedBody)
    PHP_ABSTRACT_ME(Psr_Http_Message_ServerRequestInterface, getAttributes,     arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_ServerRequestInterface, getAttribute,      arginfo_nd)
    PHP_ABSTRACT_ME(Psr_Http_Message_ServerRequestInterface, withAttribute,     arginfo_nv)
    PHP_ABSTRACT_ME(Psr_Http_Message_ServerRequestInterface, withoutAttribute,  arginfo_name)
    PHP_FE_END
};

/**
 * @see https://www.php-fig.org/psr/psr-7/#33-psrhttpmessageresponseinterface
 *
 * interface ResponseInterface extends MessageInterface
 * {
 *     public function getStatusCode();
 *     public function withStatus($code, $reasonPhrase = '');
 *     public function getReasonPhrase();
 * }
 */
static const zend_function_entry fe_Psr_Http_Message_ResponseInterface[] = {
    PHP_ABSTRACT_ME(Psr_Http_Message_ResponseInterface, getStatusCode,   arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_ResponseInterface, withStatus,      arginfo_withStatus)
    PHP_ABSTRACT_ME(Psr_Http_Message_ResponseInterface, getReasonPhrase, arginfo_empty)
    PHP_FE_END
};

/**
 * @see https://www.php-fig.org/psr/psr-7/#34-psrhttpmessagestreaminterface
 *
 * interface StreamInterface
 * {
 *     public function __toString();
 *     public function close();
 *     public function detach();
 *     public function getSize();
 *     public function tell();
 *     public function eof();
 *     public function isSeekable();
 *     public function seek($offset, $whence = SEEK_SET);
 *     public function rewind();
 *     public function isWritable();
 *     public function write($string);
 *     public function isReadable();
 *     public function read($length);
 *     public function getContents();
 *     public function getMetadata($key = null);
 * }
 */
static const zend_function_entry fe_Psr_Http_Message_StreamInterface[] = {
    PHP_ABSTRACT_ME(Psr_Http_Message_StreamInterface, __toString,   arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_StreamInterface, close,        arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_StreamInterface, detach,       arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_StreamInterface, getSize,      arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_StreamInterface, tell,         arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_StreamInterface, eof,          arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_StreamInterface, isSeekable,   arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_StreamInterface, seek,         arginfo_seek)
    PHP_ABSTRACT_ME(Psr_Http_Message_StreamInterface, rewind,       arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_StreamInterface, isWritable,   arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_StreamInterface, write,        arginfo_write)
    PHP_ABSTRACT_ME(Psr_Http_Message_StreamInterface, isReadable,   arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_StreamInterface, read,         arginfo_read)
    PHP_ABSTRACT_ME(Psr_Http_Message_StreamInterface, getContents,  arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_StreamInterface, getMetadata,  arginfo_getMetadata)
    PHP_FE_END
};

/**
 * @see https://www.php-fig.org/psr/psr-7/#35-psrhttpmessageuriinterface
 *
 * interface UriInterface
 * {
 *     public function getScheme();
 *     public function getAuthority();
 *     public function getUserInfo();
 *     public function getHost();
 *     public function getPort();
 *     public function getPath();
 *     public function getQuery();
 *     public function getFragment();
 *     public function withScheme($scheme);
 *     public function withUserInfo($user, $password = null);
 *     public function withHost($host);
 *     public function withPort($port);
 *     public function withPath($path);
 *     public function withQuery($query);
 *     public function withFragment($fragment);
 *     public function __toString();
 * }
 */
static const zend_function_entry fe_Psr_Http_Message_UriInterface[] = {
    PHP_ABSTRACT_ME(Psr_Http_Message_UriInterface, getScheme,    arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_UriInterface, getAuthority, arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_UriInterface, getUserInfo,  arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_UriInterface, getHost,      arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_UriInterface, getPort,      arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_UriInterface, getPath,      arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_UriInterface, getQuery,     arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_UriInterface, getFragment,  arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_UriInterface, withScheme,   arginfo_withScheme)
    PHP_ABSTRACT_ME(Psr_Http_Message_UriInterface, withUserInfo, arginfo_withUserInfo)
    PHP_ABSTRACT_ME(Psr_Http_Message_UriInterface, withHost,     arginfo_withHost)
    PHP_ABSTRACT_ME(Psr_Http_Message_UriInterface, withPort,     arginfo_withPort)
    PHP_ABSTRACT_ME(Psr_Http_Message_UriInterface, withPath,     arginfo_withPath)
    PHP_ABSTRACT_ME(Psr_Http_Message_UriInterface, withQuery,    arginfo_withQuery)
    PHP_ABSTRACT_ME(Psr_Http_Message_UriInterface, withFragment, arginfo_withFragment)
    PHP_ABSTRACT_ME(Psr_Http_Message_UriInterface, __toString,   arginfo_empty)
    PHP_FE_END
};

/**
 * @see https://www.php-fig.org/psr/psr-7/#36-psrhttpmessageuploadedfileinterface
 *
 * interface UploadedFileInterface
 * {
 *     public function getStream();
 *     public function moveTo($targetPath);
 *     public function getSize();
 *     public function getError();
 *     public function getClientFilename();
 *     public function getClientMediaType();
 * }
 */
static const zend_function_entry fe_Psr_Http_Message_UploadedFileInterface[] = {
    PHP_ABSTRACT_ME(Psr_Http_Message_UploadedFileInterface, getStream,          arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_UploadedFileInterface, moveTo,             arginfo_moveTo)
    PHP_ABSTRACT_ME(Psr_Http_Message_UploadedFileInterface, getSize,            arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_UploadedFileInterface, getError,           arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_UploadedFileInterface, getClientFilename,  arginfo_empty)
    PHP_ABSTRACT_ME(Psr_Http_Message_UploadedFileInterface, getClientMediaType, arginfo_empty)
    PHP_FE_END
};

int maybe_init_psr7()
{
    zend_class_entry ce;

    /* Psr\Http\Message\MessageInterface */
    ce_Psr_Http_Message_MessageInterface = zend_hash_str_find_ptr(CG(class_table), ZEND_STRL("psr\\http\\message\\messageinterface"));
    if (!ce_Psr_Http_Message_MessageInterface) {
        INIT_CLASS_ENTRY(ce, "Psr\\Http\\Message\\MessageInterface", fe_Psr_Http_Message_MessageInterface);
        ce_Psr_Http_Message_MessageInterface = zend_register_internal_interface(&ce);
    }

    /* Psr\Http\Message\RequestInterface */
    ce_Psr_Http_Message_RequestInterface = zend_hash_str_find_ptr(CG(class_table), ZEND_STRL("psr\\http\\message\\requestinterface"));
    if (!ce_Psr_Http_Message_RequestInterface) {
        INIT_CLASS_ENTRY(ce, "Psr\\Http\\Message\\RequestInterface", fe_Psr_Http_Message_RequestInterface);
        ce_Psr_Http_Message_RequestInterface = zend_register_internal_interface(&ce);
        zend_do_inheritance(ce_Psr_Http_Message_RequestInterface, ce_Psr_Http_Message_MessageInterface);
    }

    /* Psr\Http\Message\ServerRequestInterface */
    ce_Psr_Http_Message_ServerRequestInterface = zend_hash_str_find_ptr(CG(class_table), ZEND_STRL("psr\\http\\message\\serverrequestinterface"));
    if (!ce_Psr_Http_Message_ServerRequestInterface) {
        INIT_CLASS_ENTRY(ce, "Psr\\Http\\Message\\ServerRequestInterface", fe_Psr_Http_Message_ServerRequestInterface);
        ce_Psr_Http_Message_ServerRequestInterface = zend_register_internal_interface(&ce);
        zend_do_inheritance(ce_Psr_Http_Message_ServerRequestInterface, ce_Psr_Http_Message_RequestInterface);
    }

    /* Psr\Http\Message\ResponseInterface */
    ce_Psr_Http_Message_ResponseInterface = zend_hash_str_find_ptr(CG(class_table), ZEND_STRL("psr\\http\\message\\responseinterface"));
    if (!ce_Psr_Http_Message_ResponseInterface) {
        INIT_CLASS_ENTRY(ce, "Psr\\Http\\Message\\ResponseInterface", fe_Psr_Http_Message_ResponseInterface);
        ce_Psr_Http_Message_ResponseInterface = zend_register_internal_interface(&ce);
        zend_do_inheritance(ce_Psr_Http_Message_ResponseInterface, ce_Psr_Http_Message_MessageInterface);
    }

    /* Psr\Http\Message\StreamInterface */
    ce_Psr_Http_Message_StreamInterface = zend_hash_str_find_ptr(CG(class_table), ZEND_STRL("psr\\http\\message\\streaminterface"));
    if (!ce_Psr_Http_Message_StreamInterface) {
        INIT_CLASS_ENTRY(ce, "Psr\\Http\\Message\\StreamInterface", fe_Psr_Http_Message_StreamInterface);
        ce_Psr_Http_Message_StreamInterface = zend_register_internal_interface(&ce);
    }

    /* Psr\Http\Message\UriInterface */
    ce_Psr_Http_Message_UriInterface = zend_hash_str_find_ptr(CG(class_table), ZEND_STRL("psr\\http\\message\\uriinterface"));
    if (!ce_Psr_Http_Message_UriInterface) {
        INIT_CLASS_ENTRY(ce, "Psr\\Http\\Message\\UriInterface", fe_Psr_Http_Message_UriInterface);
        ce_Psr_Http_Message_UriInterface = zend_register_internal_interface(&ce);
    }

    /* Psr\Http\Message\UploadedFileInterface */
    ce_Psr_Http_Message_UploadedFileInterface = zend_hash_str_find_ptr(CG(class_table), ZEND_STRL("psr\\http\\message\\uploadedfileinterface"));
    if (!ce_Psr_Http_Message_UploadedFileInterface) {
        INIT_CLASS_ENTRY(ce, "Psr\\Http\\Message\\UploadedFileInterface", fe_Psr_Http_Message_UploadedFileInterface);
        ce_Psr_Http_Message_UploadedFileInterface = zend_register_internal_interface(&ce);
    }

    return SUCCESS;
}
