#ifndef TURBOSLIM_PERSISTENT_H
#define TURBOSLIM_PERSISTENT_H

#include "php_turboslim.h"
#include <Zend/zend_string.h>

TURBOSLIM_VISIBILITY_HIDDEN int init_persistent_data();
TURBOSLIM_VISIBILITY_HIDDEN int deinit_persistent_data();

TURBOSLIM_VISIBILITY_HIDDEN extern zend_string** turboslim_known_strings;

#define TURBOSLIM_KNOWN_STRINGS(_) \
    _(TKS_HTTP_AUTHORIZATION,                   "HTTP_AUTHORIZATION")                   \
    _(TKS_addContentLengthHeader,               "addContentLengthHeader")               \
    _(TKS_allowedMethods,                       "allowedMethod")                        \
    _(TKS_authorization,                        "authorization")                        \
    _(TKS_callable,                             "callable")                             \
    _(TKS_callableResolver,                     "callableResolver")                     \
    _(TKS_container,                            "container")                            \
    _(TKS_data,                                 "data")                                 \
    _(TKS_domain,                               "domain")                               \
    _(TKS_determineRouteBeforeAppMiddleware,    "determineRouteBeforeAppMiddleware")    \
    _(TKS_displayErrorDetails,                  "displayErrorDetails")                  \
    _(TKS_expires,                              "expires")                              \
    _(TKS_hostonly,                             "hostonly")                             \
    _(TKS_httpVersion,                          "httpVersion")                          \
    _(TKS_httponly,                             "httponly")                             \
    _(TKS_originalKey,                          "originalKey")                          \
    _(TKS_outputBuffering,                      "outputBuffering")                      \
    _(TKS_path,                                 "path")                                 \
    _(TKS_request,                              "request")                              \
    _(TKS_response,                             "response")                             \
    _(TKS_responseChunkSize,                    "responseChunkSize")                    \
    _(TKS_routerCacheFile,                      "routerCacheFile")                      \
    _(TKS_samesite,                             "samesite")                             \
    _(TKS_secure,                               "secure")                               \
    _(TKS_settings,                             "settings")                             \
    _(TKS_value,                                "value")                                \

/* Please leave the empty line above :-) */

typedef enum _turboslim_known_string_id {
#define _TURBOSLIM_STR_ID(id, str) id,
    TURBOSLIM_KNOWN_STRINGS(_TURBOSLIM_STR_ID)
#undef _TURBOSLIM_STR_ID
    TURBOSLIM_STR_LAST_KNOWN
} turboslim_known_string_id;

static inline zend_string* TSKSTR(turboslim_known_string_id idx)
{
    return turboslim_known_strings[idx];
}

TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_callable_pattern;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_string* str_magic_function;

TURBOSLIM_VISIBILITY_HIDDEN extern zval zundef;
TURBOSLIM_VISIBILITY_HIDDEN extern zval zemptyarr;

TURBOSLIM_VISIBILITY_HIDDEN extern zval container_default_settings;

#endif /* TURBOSLIM_PERSISTENT_H */
