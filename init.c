#include "init.h"

#include <sys/stat.h>
#include <Zend/zend_exceptions.h>
#include <Zend/zend_inheritance.h>
#include <Zend/zend_interfaces.h>
#include <ext/spl/spl_exceptions.h>
#include <ext/spl/spl_iterators.h>
#include "persistent.h"
#include "turboslim/callableresolver.h"
#include "turboslim/collection.h"
#include "turboslim/container.h"
#include "turboslim/deferredcallable.h"
#include "turboslim/exception/containerexception.h"
#include "turboslim/exception/containervaluenotfoundexception.h"
#include "turboslim/exception/invalidmethodexception.h"
#include "turboslim/exception/methodnotallowedexception.h"
#include "turboslim/exception/notfoundexception.h"
#include "turboslim/exception/turboslimexception.h"
#include "turboslim/fastcollection.h"
#include "turboslim/http/body.h"
#include "turboslim/http/cookies.h"
#include "turboslim/http/environment.h"
#include "turboslim/http/headers.h"
#include "turboslim/http/requestbody.h"
#include "turboslim/http/stream.h"
#include "turboslim/interfaces.h"
#include "turboslim/internal/callablewrapper.h"
#include "turboslim/internal/closure.h"
#include "turboslim/internal/container.h"
#include "turboslim/internal/internalclass.h"
#include "turboslim/psr11.h"
#include "turboslim/psr7.h"
#include "turboslim/traits/callableresolverawaretrait.h"

static void maybe_init_psr7()
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
}

static void maybe_init_psr11()
{
    zend_class_entry ce;

    ce_Psr_Container_ContainerInterface = zend_hash_str_find_ptr(CG(class_table), ZEND_STRL("psr\\container\\containerinterface"));
    if (!ce_Psr_Container_ContainerInterface) {
        INIT_CLASS_ENTRY(ce, "Psr\\Container\\ContainerInterface", fe_Psr_Container_ContainerInterface);
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
}

static void init_interfaces()
{
    zend_class_entry ce;

    /* TurboSlim\Interfaces\CallableResolverInterface */
    INIT_CLASS_ENTRY(ce, "TurboSlim\\Interfaces\\CallableResolverInterface", fe_TurboSlim_Interfaces_CallableResolverInterface);
    ce_TurboSlim_Interfaces_CallableResolverInterface = zend_register_internal_interface(&ce);

    /* TurboSlim\Interfaces\CollectionInterface */
    INIT_CLASS_ENTRY(ce, "TurboSlim\\Interfaces\\CollectionInterface", fe_TurboSlim_Interfaces_CollectionInterface);
    ce_TurboSlim_Interfaces_CollectionInterface = zend_register_internal_interface(&ce);
    zend_class_implements(ce_TurboSlim_Interfaces_CollectionInterface, 3, zend_ce_arrayaccess, spl_ce_Countable, zend_ce_aggregate);

    /* TurboSlim\Interfaces\InvocationStrategyInterface */
    INIT_CLASS_ENTRY(ce, "TurboSlim\\Interfaces\\InvocationStrategyInterface", fe_TurboSlim_Interfaces_InvocationStrategyInterface);
    ce_TurboSlim_Interfaces_InvocationStrategyInterface = zend_register_internal_interface(&ce);

    /* TurboSlim\Interfaces\RouteGroupInterface */
    INIT_CLASS_ENTRY(ce, "TurboSlim\\Interfaces\\RouteGroupInterface", fe_TurboSlim_Interfaces_RouteGroupInterface);
    ce_TurboSlim_Interfaces_RouteGroupInterface = zend_register_internal_interface(&ce);

    /* TurboSlim\Interfaces\RouteInterface */
    INIT_CLASS_ENTRY(ce, "TurboSlim\\Interfaces\\RouteInterface", fe_TurboSlim_Interfaces_RouteInterface);
    ce_TurboSlim_Interfaces_RouteInterface = zend_register_internal_interface(&ce);

    /* TurboSlim\Interfaces\RouterInterface */
    INIT_CLASS_ENTRY(ce, "TurboSlim\\Interfaces\\RouterInterface", fe_TurboSlim_Interfaces_RouterInterface);
    ce_TurboSlim_Interfaces_RouterInterface = zend_register_internal_interface(&ce);
    zend_declare_class_constant_long(ce_TurboSlim_Interfaces_RouterInterface, ZEND_STRL("DISPATCH_STATUS"), 0);
    zend_declare_class_constant_long(ce_TurboSlim_Interfaces_RouterInterface, ZEND_STRL("ALLOWED_METHODS"), 1);

    /* TurboSlim\Interfaces\Http\CookiesInterface */
    INIT_CLASS_ENTRY(ce, "TurboSlim\\Interfaces\\Http\\CookiesInterface", fe_TurboSlim_Interfaces_Http_CookiesInterface);
    ce_TurboSlim_Interfaces_Http_CookiesInterface = zend_register_internal_interface(&ce);

    /* TurboSlim\Interfaces\Http\EnvironmentInterface */
    INIT_CLASS_ENTRY(ce, "TurboSlim\\Interfaces\\Http\\EnvironmentInterface", fe_TurboSlim_Interfaces_Http_EnvironmentInterface);
    ce_TurboSlim_Interfaces_Http_EnvironmentInterface = zend_register_internal_interface(&ce);

    /* TurboSlim\Interfaces\Http\HeadersInterface */
    INIT_CLASS_ENTRY(ce, "TurboSlim\\Interfaces\\Http\\HeadersInterface", fe_TurboSlim_Interfaces_Http_HeadersInterface);
    ce_TurboSlim_Interfaces_Http_HeadersInterface = zend_register_internal_interface(&ce);
    zend_do_inheritance(ce_TurboSlim_Interfaces_Http_HeadersInterface, ce_TurboSlim_Interfaces_CollectionInterface);
}

static void init_exceptions()
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Exception\\TurboSlimException", fe_TurboSlim_Exception_TurboSlimException);
    ce_TurboSlim_Exception_TurboSlimException = zend_register_internal_class_ex(&ce, zend_ce_exception);
    zend_declare_property_null(ce_TurboSlim_Exception_TurboSlimException, ZEND_STRL("request"),  ZEND_ACC_PROTECTED);
    zend_declare_property_null(ce_TurboSlim_Exception_TurboSlimException, ZEND_STRL("response"), ZEND_ACC_PROTECTED);

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Exception\\ContainerException", NULL);
    ce_TurboSlim_Exception_ContainerException = zend_register_internal_class_ex(&ce, spl_ce_InvalidArgumentException);
    zend_class_implements(ce_TurboSlim_Exception_ContainerException, 1, ce_Psr_Container_ContainerExceptionInterface);

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Exception\\ContainerValueNotFoundException", NULL);
    ce_TurboSlim_Exception_ContainerValueNotFoundException = zend_register_internal_class_ex(&ce, spl_ce_RuntimeException);
    zend_class_implements(ce_TurboSlim_Exception_ContainerValueNotFoundException, 1, ce_Psr_Container_NotFoundExceptionInterface);

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Exception\\InvalidMethodException", fe_TurboSlim_Exception_InvalidMethodException);
    ce_TurboSlim_Exception_InvalidMethodException = zend_register_internal_class_ex(&ce, spl_ce_InvalidArgumentException);
    zend_declare_property_null(ce_TurboSlim_Exception_InvalidMethodException, ZEND_STRL("request"), ZEND_ACC_PROTECTED);

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Exception\\MethodNotAllowedException", fe_TurboSlim_Exception_MethodNotAllowedException);
    ce_TurboSlim_Exception_MethodNotAllowedException = zend_register_internal_class_ex(&ce, ce_TurboSlim_Exception_TurboSlimException);
    zend_declare_property_null(ce_TurboSlim_Exception_MethodNotAllowedException, ZEND_STRL("allowedMethods"), ZEND_ACC_PROTECTED);

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Exception\\NotFoundException", NULL);
    ce_TurboSlim_Exception_TurboSlimException = zend_register_internal_class_ex(&ce, ce_TurboSlim_Exception_TurboSlimException);

    INIT_CLASS_ENTRY(ce, "TurboSlim\\CallableResolverAwareTrait", fe_TurboSlim_CallableResolverAwareTrait);
    ce_TurboSlim_CallableResolverAwareTrait = zend_register_internal_class(&ce);
    ce_TurboSlim_CallableResolverAwareTrait->ce_flags |= ZEND_ACC_TRAIT;
}

static void init_internal_classes()
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Internal\\Closure", NULL);
    ce_TurboSlim_Internal_Closure = zend_register_internal_class(&ce);

    ce_TurboSlim_Internal_Closure->ce_flags |= ZEND_ACC_FINAL;

    ce_TurboSlim_Internal_Closure->create_object = turboslim_internal_closure_create_object;
    ce_TurboSlim_Internal_Closure->serialize     = zend_class_serialize_deny;
    ce_TurboSlim_Internal_Closure->unserialize   = zend_class_unserialize_deny;

    memcpy(&turboslim_internal_closure_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    turboslim_internal_closure_handlers.free_obj             = turboslim_internal_closure_free_obj;
    turboslim_internal_closure_handlers.get_constructor      = turboslim_deny_construct;
    turboslim_internal_closure_handlers.clone_obj            = turboslim_internal_closure_clone_obj;
    turboslim_internal_closure_handlers.write_property       = turboslim_deny_write_property;
    turboslim_internal_closure_handlers.read_property        = turboslim_deny_read_property;
    turboslim_internal_closure_handlers.get_property_ptr_ptr = turboslim_deny_get_property_ptr_ptr;
    turboslim_internal_closure_handlers.has_property         = turboslim_deny_has_property;
    turboslim_internal_closure_handlers.unset_property       = turboslim_deny_unset_property;
    turboslim_internal_closure_handlers.get_closure          = turboslim_internal_closure_get_closure;
    turboslim_internal_closure_handlers.compare_objects      = turboslim_internal_closure_compare_objects;
    turboslim_internal_closure_handlers.get_gc               = turboslim_internal_closure_get_gc;

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Internal\\Container\\ProtectedCallable", NULL);
    ce_TurboSlim_Internal_Container_ProtectedCallable = zend_register_internal_class(&ce);
    ce_TurboSlim_Internal_Container_ProtectedCallable->ce_flags |= ZEND_ACC_FINAL;

    ce_TurboSlim_Internal_Container_ProtectedCallable->create_object = turboslim_internal_container_create_object;
    ce_TurboSlim_Internal_Container_ProtectedCallable->serialize     = zend_class_serialize_deny;
    ce_TurboSlim_Internal_Container_ProtectedCallable->unserialize   = zend_class_unserialize_deny;

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Internal\\Container\\FactoryCallable", NULL);
    ce_TurboSlim_Internal_Container_FactoryCallable = zend_register_internal_class(&ce);
    ce_TurboSlim_Internal_Container_FactoryCallable->ce_flags |= ZEND_ACC_FINAL;

    ce_TurboSlim_Internal_Container_FactoryCallable->create_object = turboslim_internal_container_create_object;
    ce_TurboSlim_Internal_Container_FactoryCallable->serialize     = zend_class_serialize_deny;
    ce_TurboSlim_Internal_Container_FactoryCallable->unserialize   = zend_class_unserialize_deny;

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Internal\\Container\\ServiceCallable", NULL);
    ce_TurboSlim_Internal_Container_ServiceCallable = zend_register_internal_class(&ce);
    ce_TurboSlim_Internal_Container_ServiceCallable->ce_flags |= ZEND_ACC_FINAL;

    ce_TurboSlim_Internal_Container_ServiceCallable->create_object = turboslim_internal_container_create_object;
    ce_TurboSlim_Internal_Container_ServiceCallable->serialize     = zend_class_serialize_deny;
    ce_TurboSlim_Internal_Container_ServiceCallable->unserialize   = zend_class_unserialize_deny;

    /* Handlers common to all TurboSlim\Internal\Container\* classes */
    memcpy(&turboslim_internal_container_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    turboslim_internal_container_handlers.free_obj             = turboslim_internal_container_free_obj;
    turboslim_internal_container_handlers.get_constructor      = turboslim_deny_construct;
    turboslim_internal_container_handlers.write_property       = turboslim_deny_write_property;
    turboslim_internal_container_handlers.read_property        = turboslim_deny_read_property;
    turboslim_internal_container_handlers.get_property_ptr_ptr = turboslim_deny_get_property_ptr_ptr;
    turboslim_internal_container_handlers.has_property         = turboslim_deny_has_property;
    turboslim_internal_container_handlers.unset_property       = turboslim_deny_unset_property;
    turboslim_internal_container_handlers.clone_obj            = turboslim_internal_container_clone_obj;
    turboslim_internal_container_handlers.compare_objects      = turboslim_internal_container_compare_objects;
    turboslim_internal_container_handlers.get_gc               = turboslim_internal_container_get_gc;

    /* TurboSlim\Internal\CallableWrapper */
    memset(&turboslim_internal_callablewrapper_invoker, 0, sizeof(turboslim_internal_callablewrapper_invoker));
    turboslim_internal_callablewrapper_invoker.type = ZEND_INTERNAL_FUNCTION;
    turboslim_internal_callablewrapper_invoker.internal_function.function_name = str_magic_function;
    turboslim_internal_callablewrapper_invoker.internal_function.scope         = ce_TurboSlim_Internal_Closure;
    turboslim_internal_callablewrapper_invoker.internal_function.handler       = ZEND_MN(TurboSlim_Internal_CallableWrapper___invoke);
    turboslim_internal_callablewrapper_invoker.internal_function.module        = &turboslim_module_entry;
}

static void init_callable_resolver()
{
    zend_class_entry ce;
    zval znull;
    ZVAL_NULL(&znull);

    INIT_CLASS_ENTRY(ce, "TurboSlim\\CallableResolver", fe_TurboSlim_CallableResolver);
    ce_TurboSlim_CallableResolver = zend_register_internal_class(&ce);
    ce_TurboSlim_CallableResolver->ce_flags |= ZEND_ACC_FINAL;
    zend_class_implements(ce_TurboSlim_CallableResolver, 1, ce_TurboSlim_Interfaces_CallableResolverInterface);
    zend_declare_class_constant_stringl(ce_TurboSlim_CallableResolver, ZEND_STRL("CALLABLE_PATTERN"), ZSTR_VAL(str_callable_pattern), ZSTR_LEN(str_callable_pattern));
    zend_declare_property_ex(ce_TurboSlim_CallableResolver, TSKSTR(TKS_container), &znull, ZEND_ACC_PRIVATE, NULL);

    ce_TurboSlim_CallableResolver->serialize   = zend_class_serialize_deny;
    ce_TurboSlim_CallableResolver->unserialize = zend_class_unserialize_deny;
}

static void init_collection()
{
    zend_class_entry ce;
    zval znull;
    ZVAL_NULL(&znull);

    /* TurboSlim\Collection */
    INIT_CLASS_ENTRY(ce, "TurboSlim\\Collection", fe_TurboSlim_Collection);
    ce_TurboSlim_Collection = zend_register_internal_class(&ce);
    zend_declare_property_ex(ce_TurboSlim_Collection, TSKSTR(TKS_data), &znull, ZEND_ACC_PROTECTED, NULL);

    ce_TurboSlim_Collection->create_object = turboslim_collection_create_object;
    ce_TurboSlim_Collection->get_iterator  = turboslim_collection_get_iterator;

    memcpy(&turboslim_collection_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    turboslim_collection_handlers.offset          = XtOffsetOf(turboslim_collection_t, std);
    turboslim_collection_handlers.clone_obj       = turboslim_collection_clone_obj;
    turboslim_collection_handlers.read_dimension  = turboslim_collection_read_dimension;
    turboslim_collection_handlers.write_dimension = turboslim_collection_write_dimension;
    turboslim_collection_handlers.has_dimension   = turboslim_collection_has_dimension;
    turboslim_collection_handlers.unset_dimension = turboslim_collection_unset_dimension;
    turboslim_collection_handlers.count_elements  = turboslim_collection_count_elements;

    zend_class_implements(ce_TurboSlim_Collection, 1, ce_TurboSlim_Interfaces_CollectionInterface);

    /* TurboSlim\FastCollection */
    INIT_CLASS_ENTRY(ce, "TurboSlim\\FastCollection", fe_TurboSlim_FastCollection);
    ce_TurboSlim_FastCollection = zend_register_internal_class(&ce);
    ce_TurboSlim_FastCollection->ce_flags |= ZEND_ACC_FINAL;

    ce_TurboSlim_FastCollection->create_object = turboslim_fastcollection_create_object;
    ce_TurboSlim_FastCollection->get_iterator  = turboslim_fastcollection_get_iterator;

    memcpy(&turboslim_fastcollection_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    turboslim_fastcollection_handlers.free_obj        = turboslim_fastcollection_free_obj;
    turboslim_fastcollection_handlers.clone_obj       = turboslim_fastcollection_clone_obj;
    turboslim_fastcollection_handlers.read_property   = turboslim_no_props_read_property;
    turboslim_fastcollection_handlers.write_property  = turboslim_no_props_write_property;
    turboslim_fastcollection_handlers.has_property    = turboslim_no_props_has_property;
    turboslim_fastcollection_handlers.unset_property  = turboslim_no_props_unset_property;
    turboslim_fastcollection_handlers.read_dimension  = turboslim_fastcollection_read_dimension;
    turboslim_fastcollection_handlers.write_dimension = turboslim_fastcollection_write_dimension;
    turboslim_fastcollection_handlers.unset_dimension = turboslim_fastcollection_unset_dimension;
    turboslim_fastcollection_handlers.has_dimension   = turboslim_fastcollection_has_dimension;
    turboslim_fastcollection_handlers.get_properties  = turboslim_fastcollection_get_properties;
    turboslim_fastcollection_handlers.count_elements  = turboslim_fastcollection_count_elements;
    turboslim_fastcollection_handlers.compare_objects = turboslim_fastcollection_compare_objects;

    zend_class_implements(ce_TurboSlim_FastCollection, 1, ce_TurboSlim_Interfaces_CollectionInterface);
}

static void init_container()
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Container", fe_TurboSlim_Container);
    ce_TurboSlim_Container = zend_register_internal_class(&ce);

    ce_TurboSlim_Container->create_object = turboslim_container_create_object;
    ce_TurboSlim_Container->serialize     = zend_class_serialize_deny;
    ce_TurboSlim_Container->unserialize   = zend_class_unserialize_deny;

    memcpy(&turboslim_container_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    turboslim_container_handlers.offset          = XtOffsetOf(turboslim_container_t, std);
    turboslim_container_handlers.free_obj        = turboslim_container_free_obj;
    turboslim_container_handlers.clone_obj       = turboslim_container_clone_obj;
    turboslim_container_handlers.read_dimension  = turboslim_container_read_dimension;
    turboslim_container_handlers.write_dimension = turboslim_container_write_dimension;
    turboslim_container_handlers.has_dimension   = turboslim_container_has_dimension;
    turboslim_container_handlers.unset_dimension = turboslim_container_unset_dimension;
    turboslim_container_handlers.read_property   = turboslim_container_read_property;
    turboslim_container_handlers.has_property    = turboslim_container_has_property;
    turboslim_container_handlers.get_gc          = turboslim_container_get_gc;
    turboslim_container_handlers.compare_objects = turboslim_container_compare_objects;

    zend_class_implements(ce_TurboSlim_Container, 2, zend_ce_arrayaccess, ce_Psr_Container_ContainerInterface);

    memset(&turboslim_container_invoker, 0, sizeof(turboslim_container_invoker));
    turboslim_container_invoker.type                                = ZEND_INTERNAL_FUNCTION;
    turboslim_container_invoker.internal_function.function_name     = str_magic_function;
    turboslim_container_invoker.internal_function.scope             = ce_TurboSlim_Internal_Closure;
    turboslim_container_invoker.internal_function.handler           = ZEND_FN(TurboSlim_Container_Invoker);
    turboslim_container_invoker.internal_function.module            = &turboslim_module_entry;
}

static void init_deferred_callable()
{
    zend_class_entry ce;
    zval znull;
    ZVAL_NULL(&znull);

    INIT_CLASS_ENTRY(ce, "TurboSlim\\DeferredCallable", fe_TurboSlim_DeferredCallable);
    ce_TurboSlim_DeferredCallable = zend_register_internal_class(&ce);
    zend_declare_property_ex(ce_TurboSlim_DeferredCallable, TSKSTR(TKS_callable),  &znull, ZEND_ACC_PRIVATE, NULL);
    zend_declare_property_ex(ce_TurboSlim_DeferredCallable, TSKSTR(TKS_container), &znull, ZEND_ACC_PRIVATE, NULL);
    ce_TurboSlim_DeferredCallable->serialize   = zend_class_serialize_deny;
    ce_TurboSlim_DeferredCallable->unserialize = zend_class_unserialize_deny;
}

static void init_http_stream()
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Http\\Stream", fe_TurboSlim_Http_Stream);
    ce_TurboSlim_Http_Stream = zend_register_internal_class(&ce);
    zend_class_implements(ce_TurboSlim_Http_Stream, 1, ce_Psr_Http_Message_StreamInterface);

    zend_declare_class_constant_long(ce_TurboSlim_Http_Stream, ZEND_STRL("FSTAT_MODE_S_IFIFO"), S_IFIFO);
    zend_declare_property_null(ce_TurboSlim_Http_Stream, ZEND_STRL("stream"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(ce_TurboSlim_Http_Stream, ZEND_STRL("meta"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(ce_TurboSlim_Http_Stream, ZEND_STRL("readable"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(ce_TurboSlim_Http_Stream, ZEND_STRL("writable"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(ce_TurboSlim_Http_Stream, ZEND_STRL("seekable"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(ce_TurboSlim_Http_Stream, ZEND_STRL("size"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(ce_TurboSlim_Http_Stream, ZEND_STRL("isPipe"), ZEND_ACC_PROTECTED);

    ce_TurboSlim_Http_Stream->create_object = turboslim_http_stream_create_object;
    ce_TurboSlim_Http_Stream->serialize     = zend_class_serialize_deny;
    ce_TurboSlim_Http_Stream->unserialize   = zend_class_unserialize_deny;

    memcpy(&turboslim_http_stream_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    turboslim_http_stream_handlers.offset          = XtOffsetOf(turboslim_http_stream_t, std);
    turboslim_http_stream_handlers.free_obj        = turboslim_http_stream_free_obj;
    turboslim_http_stream_handlers.clone_obj       = turboslim_http_stream_clone_obj;
    turboslim_http_stream_handlers.read_property   = turboslim_http_stream_read_property;
    turboslim_http_stream_handlers.has_property    = turboslim_http_stream_has_property;
    turboslim_http_stream_handlers.write_property  = turboslim_http_stream_write_property;
    turboslim_http_stream_handlers.get_properties  = turboslim_http_stream_get_properties;
    turboslim_http_stream_handlers.get_gc          = turboslim_http_stream_get_gc;
    turboslim_http_stream_handlers.cast_object     = turboslim_http_stream_cast_object;
    turboslim_http_stream_handlers.compare_objects = turboslim_http_stream_compare_objects;

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Http\\Body", NULL);
    ce_TurboSlim_Http_Body = zend_register_internal_class_ex(&ce, ce_TurboSlim_Http_Stream);

    ce_TurboSlim_Http_Body->serialize   = zend_class_serialize_deny;
    ce_TurboSlim_Http_Body->unserialize = zend_class_unserialize_deny;

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Http\\RequestBody", fe_TurboSlim_Http_RequestBody);
    ce_TurboSlim_Http_RequestBody = zend_register_internal_class_ex(&ce, ce_TurboSlim_Http_Body);

    ce_TurboSlim_Http_Body->serialize   = zend_class_serialize_deny;
    ce_TurboSlim_Http_Body->unserialize = zend_class_unserialize_deny;
}

static void init_http_cookies()
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Http\\Cookies", fe_TurboSlim_Http_Cookies);
    ce_TurboSlim_Http_Cookies = zend_register_internal_class(&ce);
    zend_class_implements(ce_TurboSlim_Http_Cookies, 1, ce_TurboSlim_Interfaces_Http_CookiesInterface);

    ce_TurboSlim_Http_Cookies->create_object = turboslim_http_cookies_create_object;

    zend_declare_property_null(ce_TurboSlim_Http_Cookies, ZEND_STRL("requestCookies"),  ZEND_ACC_PROTECTED);
    zend_declare_property_null(ce_TurboSlim_Http_Cookies, ZEND_STRL("responseCookies"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(ce_TurboSlim_Http_Cookies, ZEND_STRL("defaults"),        ZEND_ACC_PROTECTED);

    memcpy(&turboslim_http_cookies_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    turboslim_http_cookies_handlers.clone_obj = turboslim_http_cookies_clone_obj;
}

static void init_http_environment()
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Http\\Environment", fe_TurboSlim_Http_Environment);
    ce.get_iterator = ce_TurboSlim_Collection->get_iterator;
    ce_TurboSlim_Http_Environment = zend_register_internal_class_ex(&ce, ce_TurboSlim_Collection);
    zend_class_implements(ce_TurboSlim_Http_Environment, 1, ce_TurboSlim_Interfaces_Http_EnvironmentInterface);
}

static void init_http_headers()
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Http\\Headers", fe_TurboSlim_Http_Headers);
    ce_TurboSlim_Http_Headers = zend_register_internal_class_ex(&ce, ce_TurboSlim_Collection);
    zend_declare_property_null(ce_TurboSlim_Http_Headers, ZEND_STRL("special"), ZEND_ACC_PROTECTED | ZEND_ACC_STATIC);
}

int init_module()
{
    init_persistent_data();

    maybe_init_psr7();
    maybe_init_psr11();
    init_interfaces();

    init_exceptions();

    init_internal_classes();

    init_callable_resolver();
    init_collection();
    init_container();
    init_deferred_callable();

    init_http_stream();
    init_http_cookies();
    init_http_environment();
    init_http_headers();

    return SUCCESS;
}

int init_request()
{
    zval arr;
    zval z;
    array_init_size(&arr, 8);
    ZVAL_LONG(&z, 1);
    Z_SET_REFCOUNT(arr, 0);
    _zend_hash_str_add_new(Z_ARRVAL(arr), ZEND_STRL("CONTENT_TYPE"), &z ZEND_FILE_LINE_CC);
    _zend_hash_str_add_new(Z_ARRVAL(arr), ZEND_STRL("CONTENT_LENGTH"), &z ZEND_FILE_LINE_CC);
    _zend_hash_str_add_new(Z_ARRVAL(arr), ZEND_STRL("PHP_AUTH_USER"), &z ZEND_FILE_LINE_CC);
    _zend_hash_str_add_new(Z_ARRVAL(arr), ZEND_STRL("PHP_AUTH_PW"), &z ZEND_FILE_LINE_CC);
    _zend_hash_str_add_new(Z_ARRVAL(arr), ZEND_STRL("PHP_AUTH_DIGEST"), &z ZEND_FILE_LINE_CC);
    _zend_hash_str_add_new(Z_ARRVAL(arr), ZEND_STRL("AUTH_TYPE"), &z ZEND_FILE_LINE_CC);
    zend_update_static_property(ce_TurboSlim_Http_Headers, ZEND_STRL("special"), &arr);

    return SUCCESS;
}
