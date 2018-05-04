#include "php_turboslim.h"

#include <ext/standard/info.h>
#include <traits/callableresolverawaretrait.h>
#include "exception/containerexception.h"
#include "exception/containervaluenotfoundexception.h"
#include "exception/invalidmethodexception.h"
#include "exception/methodnotallowedexception.h"
#include "exception/notfoundexception.h"
#include "exception/turboslimexception.h"
#include "internal/closure.h"
#include "internal/container.h"
#include "internal/internalclass.h"
#include "callableresolver.h"
#include "callablewrapper.h"
#include "collection.h"
#include "container.h"
#include "deferredcallable.h"
#include "fastcollection.h"
#include "interfaces.h"
#include "persistent.h"
#include "psr7.h"
#include "psr11.h"

ZEND_DECLARE_MODULE_GLOBALS(turboslim);

static PHP_GINIT_FUNCTION(turboslim)
{
    turboslim_globals->slim_interfaces_mocked = 0;
}

static PHP_MINIT_FUNCTION(turboslim)
{
    EG(full_tables_cleanup) = 1;

    /* Init common data structures */
    init_persistent_data();

    /* Init interfaces */
    maybe_init_psr7();
    maybe_init_psr11();
    init_interfaces();

    /* Init exceptions */
    init_turbo_slim_exception();
    init_container_exception();
    init_container_value_not_found_exception();
    init_invalid_method_exception();
    init_method_not_allowed_exception();
    init_not_found_exception();

    /* Init traits */
    init_callable_resolver_aware_trait();

    /* Init internal classes */
    init_turboslim_closure();
    init_callable_wrapper();
    init_internal_container_classes();

    /* Init other classes */
    init_callable_resolver();
    init_collection();
    init_container();
    init_deferred_callable();
    init_fast_collection();

    return SUCCESS;
}

static PHP_MSHUTDOWN_FUNCTION(turboslim)
{
    deinit_persistent_data();
    return SUCCESS;
}

static PHP_RSHUTDOWN_FUNCTION(turboslim)
{
    if (TURBOSLIM_G(slim_interfaces_mocked)) {
        zend_hash_str_del(CG(class_table), ZEND_STRL("slim\\interfaces\\callableresolverinterface"));
        zend_hash_str_del(CG(class_table), ZEND_STRL("slim\\interfaces\\collectioninterface"));
        zend_hash_str_del(CG(class_table), ZEND_STRL("slim\\interfaces\\invocationstrategyinterface"));
        zend_hash_str_del(CG(class_table), ZEND_STRL("slim\\interfaces\\routegroupinterface"));
        zend_hash_str_del(CG(class_table), ZEND_STRL("slim\\interfaces\\routeinterface"));
        zend_hash_str_del(CG(class_table), ZEND_STRL("slim\\interfaces\\routerinterface"));

        zend_hash_str_del(CG(class_table), ZEND_STRL("slim\\interfaces\\http\\cookiesinterface"));
        zend_hash_str_del(CG(class_table), ZEND_STRL("slim\\interfaces\\http\\environmentinterface"));
        zend_hash_str_del(CG(class_table), ZEND_STRL("slim\\interfaces\\http\\headersinterface"));

        TURBOSLIM_G(slim_interfaces_mocked) = 0;
    }

    return SUCCESS;
}

static PHP_MINFO_FUNCTION(turboslim)
{

}

static const zend_module_dep turboslim_deps[] = {
    ZEND_MOD_REQUIRED("json")
    ZEND_MOD_REQUIRED("spl")
    ZEND_MOD_REQUIRED("pcre")
    ZEND_MOD_END
};

#ifdef COVERAGE
static PHP_FUNCTION(flush_coverage)
{
    __gcov_flush();
}
#endif

static PHP_FUNCTION(mock_slim_interfaces)
{
    if (!TURBOSLIM_G(slim_interfaces_mocked)) {
        zend_register_class_alias_ex(ZEND_STRL("Slim\\Interfaces\\CallableResolverInterface"),   ce_TurboSlim_Interfaces_CallableResolverInterface);
        zend_register_class_alias_ex(ZEND_STRL("Slim\\Interfaces\\CollectionInterface"),         ce_TurboSlim_Interfaces_CollectionInterface);
        zend_register_class_alias_ex(ZEND_STRL("Slim\\Interfaces\\InvocationStrategyInterface"), ce_TurboSlim_Interfaces_InvocationStrategyInterface);
        zend_register_class_alias_ex(ZEND_STRL("Slim\\Interfaces\\RouteGroupInterface"),         ce_TurboSlim_Interfaces_RouteGroupInterface);
        zend_register_class_alias_ex(ZEND_STRL("Slim\\Interfaces\\RouteInterface"),              ce_TurboSlim_Interfaces_RouteInterface);
        zend_register_class_alias_ex(ZEND_STRL("Slim\\Interfaces\\RouterInterface"),             ce_TurboSlim_Interfaces_RouterInterface);

        zend_register_class_alias_ex(ZEND_STRL("Slim\\Interfaces\\Http\\CookiesInterface"),      ce_TurboSlim_Interfaces_Http_CookiesInterface);
        zend_register_class_alias_ex(ZEND_STRL("Slim\\Interfaces\\Http\\EnvironmentInterface"),  ce_TurboSlim_Interfaces_Http_EnvironmentInterface);
        zend_register_class_alias_ex(ZEND_STRL("Slim\\Interfaces\\Http\\HeadersInterface"),      ce_TurboSlim_Interfaces_Http_HeadersInterface);

        TURBOSLIM_G(slim_interfaces_mocked) = 1;
    }
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_empty, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry fe[] = {
#ifdef COVERAGE
    ZEND_FE(flush_coverage, NULL)
#endif
    ZEND_NS_FE("TurboSlim", mock_slim_interfaces, arginfo_empty)
    ZEND_FE_END
};

zend_module_entry turboslim_module_entry = {
    STANDARD_MODULE_HEADER_EX,
    NULL,
    turboslim_deps,
    PHP_TURBOSLIM_EXTNAME,
    fe,
    PHP_MINIT(turboslim),
    PHP_MSHUTDOWN(turboslim),
    NULL,
    PHP_RSHUTDOWN(turboslim),
    PHP_MINFO(turboslim),
    PHP_TURBOSLIM_EXTVER,
    PHP_MODULE_GLOBALS(turboslim),
    PHP_GINIT(turboslim),
    NULL,
    NULL,
    STANDARD_MODULE_PROPERTIES_EX
};


#ifdef COMPILE_DL_TURBOSLIM
ZEND_GET_MODULE(turboslim)
#endif