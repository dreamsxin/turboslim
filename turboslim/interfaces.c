#include "turboslim/interfaces.h"

zend_class_entry* ce_TurboSlim_Interfaces_CallableResolverInterface   = NULL;
zend_class_entry* ce_TurboSlim_Interfaces_CollectionInterface         = NULL;
zend_class_entry* ce_TurboSlim_Interfaces_InvocationStrategyInterface = NULL;
zend_class_entry* ce_TurboSlim_Interfaces_RouteGroupInterface         = NULL;
zend_class_entry* ce_TurboSlim_Interfaces_RouteInterface              = NULL;
zend_class_entry* ce_TurboSlim_Interfaces_RouterInterface             = NULL;
zend_class_entry* ce_TurboSlim_Interfaces_Http_CookiesInterface       = NULL;
zend_class_entry* ce_TurboSlim_Interfaces_Http_EnvironmentInterface   = NULL;
zend_class_entry* ce_TurboSlim_Interfaces_Http_HeadersInterface       = NULL;

/* mixed func() */
ZEND_BEGIN_ARG_INFO_EX(arginfo_empty, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

/* mixed func(mixed $key) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_key, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

/* mixed func(mixed $key, mixed $value) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_kv, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

/* mixed func(mixed $key, mixed $default = null) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_kd, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, default)
ZEND_END_ARG_INFO()

/* mixed func(mixed $callable) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_callable, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, callable)
ZEND_END_ARG_INFO()

/* mixed func(Psr\Http\Message\ServerRequestInterface $request, Psr\Http\Message\ResponseInterface $response) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_rr, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_OBJ_INFO(0, request, Psr\\Http\\Message\\ServerRequestInterface, 0)
    ZEND_ARG_OBJ_INFO(0, response, Psr\\Http\\Message\\ResponseInterface, 0)
ZEND_END_ARG_INFO()

/* mixed func(mixed $name) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_name, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

/* mixed func(mixed $toResolve) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_callableresolverinterface_resolve, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, toResolve)
ZEND_END_ARG_INFO()

/* mixed func(array $items) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_collectioninterface_replace, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_ARRAY_INFO(0, items, 0)
ZEND_END_ARG_INFO()

/* mixed func(callable $callable, Psr\Http\Message\ServerRequestInterface $request, Psr\Http\Message\ResponseInterface $response, array $routeArguments) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_invocationstrategyinterface___invoke, 0, ZEND_RETURN_VALUE, 4)
    ZEND_ARG_TYPE_INFO(0, callable, IS_CALLABLE, 0)
    ZEND_ARG_OBJ_INFO(0, request, Psr\\Http\\Message\\ServerRequestInterface, 0)
    ZEND_ARG_OBJ_INFO(0, response, Psr\\Http\\Message\\ResponseInterface, 0)
    ZEND_ARG_ARRAY_INFO(0, routeArgument, 0)
ZEND_END_ARG_INFO()

/* mixed func(mixed $app) */
/**
 * @note $app is originally of Slim\App type but because we can use
 * Slim\App and TurboSlim\App interchangeably and there is no common interface
 * we have to strip off the type
 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_routegroupinterface___invoke, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, app)
ZEND_END_ARG_INFO()

/* mixed func(mixed $name, mixed $default = null) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_routeinterface_getargument, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, default)
ZEND_END_ARG_INFO()

/* mixed func(mixed $name, mixed $value) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_routeinterface_setargument, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

/* mixed func(array $arguments) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_routeinterface_setarguments, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_ARRAY_INFO(0, arguments, 0)
ZEND_END_ARG_INFO()

/* mixed func(mixed $mode) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_routeinterface_setoutputbuffering, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

/* mixed func(ServerRequestInterface $request, array $arguments) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_routeinterface_prepare, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_OBJ_INFO(0, request, Psr\\Http\\Message\\ServerRequestInterface, 0)
    ZEND_ARG_ARRAY_INFO(0, params, 0)
ZEND_END_ARG_INFO()

/* mixed func(mixed $methods, mixed $pattern, mixed $handler) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_routerinterface_map, 0, ZEND_RETURN_VALUE, 3)
    ZEND_ARG_INFO(0, methods)
    ZEND_ARG_INFO(0, pattern)
    ZEND_ARG_INFO(0, handler)
ZEND_END_ARG_INFO()

/* mixed func(Psr\Http\Message\ServerRequestInterface $requets) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_routerinterface_dispatch, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_OBJ_INFO(0, request, Psr\\Http\\Message\\ServerRequestInterface, 0)
ZEND_END_ARG_INFO()

/* mixed func(mixed $methods, mixed $pattern, mixed $handler) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_routerinterface_pushgroup, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, pattern)
    ZEND_ARG_INFO(0, callable)
ZEND_END_ARG_INFO()

/* mixed func(mixed $methods, mixed $pattern, mixed $handler) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_routerinterface_lookuproute, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, identifier)
ZEND_END_ARG_INFO()

/* mixed func(mixed $name, array $data = null, array $queryParams = null) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_routerinterface_pathfor, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_ARRAY_INFO(0, data, 0)
    ZEND_ARG_ARRAY_INFO(0, queryParams, 0)
ZEND_END_ARG_INFO()

/* mixed func(array $settings = null) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_http_cookiesinterface_parseheader, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, header)
ZEND_END_ARG_INFO()

/* mixed func(array $settings = null) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_environmentinterface_mock, 0, ZEND_RETURN_VALUE, 0)
    ZEND_ARG_ARRAY_INFO(0, settings, 0)
ZEND_END_ARG_INFO()

/*
 * interface CallableResolverInterface
 * {
 *     public function resolve($toResolve);
 * }
 */
const zend_function_entry fe_TurboSlim_Interfaces_CallableResolverInterface[] = {
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_CallableResolverInterface, resolve, arginfo_callableresolverinterface_resolve)
    PHP_FE_END
};

/*
 * interface CollectionInterface extends \ArrayAccess, \Countable, \IteratorAggregate
 * {
 *     public function set($key, $value);
 *     public function get($key, $default = null);
 *     public function replace(array $items);
 *     public function all();
 *     public function has($key);
 *     public function remove($key);
 *     public function clear();
 * }
 */
const zend_function_entry fe_TurboSlim_Interfaces_CollectionInterface[] = {
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_CollectionInterface, set,     arginfo_kv)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_CollectionInterface, get,     arginfo_kd)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_CollectionInterface, replace, arginfo_collectioninterface_replace)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_CollectionInterface, all,     arginfo_empty)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_CollectionInterface, has,     arginfo_key)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_CollectionInterface, remove,  arginfo_key)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_CollectionInterface, clear,   arginfo_empty)
    PHP_FE_END
};

/*
 * interface InvocationStrategyInterface
 * {
 *     public function __invoke(callable $callable, Psr\Http\Message\ServerRequestInterface $request, Psr\Http\Message\ResponseInterface $response, array $routeArguments);
 * }
 */
const zend_function_entry fe_TurboSlim_Interfaces_InvocationStrategyInterface[] = {
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_InvocationStrategyInterface, __invoke, arginfo_invocationstrategyinterface___invoke)
    PHP_FE_END
};

/*
 * interface RouteGroupInterface
 * {
 *     public function getPattern();
 *     public function add($callable);
 *     public function __invoke(App $app);
 * }
 */
const zend_function_entry fe_TurboSlim_Interfaces_RouteGroupInterface[] = {
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_RouteGroupInterface, getPattern, arginfo_empty)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_RouteGroupInterface, add,        arginfo_callable)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_RouteGroupInterface, __invoke,   arginfo_routegroupinterface___invoke)
    PHP_FE_END
};

/*
 * interface RouteInterface
 * {
 *     public function getArgument($name, $default = null);
 *     public function getArguments();
 *     public function getName();
 *     public function getPattern();
 *     public function setArgument($name, $value);
 *     public function setArguments(array $arguments);
 *     public function setOutputBuffering($mode);
 *     public function setName($name);
 *     public function add($callable);
 *     public function prepare(ServerRequestInterface $request, array $arguments);
 *     public function run(ServerRequestInterface $request, ResponseInterface $response);
 *     public function __invoke(ServerRequestInterface $request, ResponseInterface $response);
 * }
 */
const zend_function_entry fe_TurboSlim_Interfaces_RouteInterface[] = {
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_RouteInterface, getArgument,        arginfo_routeinterface_getargument)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_RouteInterface, getArguments,       arginfo_empty)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_RouteInterface, getName,            arginfo_empty)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_RouteInterface, getPattern,         arginfo_empty)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_RouteInterface, setArgument,        arginfo_routeinterface_setargument)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_RouteInterface, setArguments,       arginfo_routeinterface_setarguments)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_RouteInterface, setOutputBuffering, arginfo_routeinterface_setoutputbuffering)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_RouteInterface, setName,            arginfo_name)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_RouteInterface, add,                arginfo_callable)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_RouteInterface, prepare,            arginfo_routeinterface_prepare)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_RouteInterface, run,                arginfo_rr)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_RouteInterface, __invoke,           arginfo_rr)
    PHP_FE_END
};

/*
 * interface RouterInterface
 * {
 *     public function map($methods, $pattern, $handler);
 *     public function dispatch(ServerRequestInterface $request);
 *     public function pushGroup($pattern, $callable);
 *     public function popGroup();
 *     public function getNamedRoute($name);
 *     public function lookupRoute($identifier);
 *     public function relativePathFor($name, array $data = [], array $queryParams = []);
 *     public function pathFor($name, array $data = [], array $queryParams = []);
 * }
 */
const zend_function_entry fe_TurboSlim_Interfaces_RouterInterface[] = {
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_RouterInterface, map,             arginfo_routerinterface_map)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_RouterInterface, dispatch,        arginfo_routerinterface_dispatch)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_RouterInterface, pushGroup,       arginfo_routerinterface_pushgroup)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_RouterInterface, popGroup,        arginfo_empty)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_RouterInterface, getNamedRoute,   arginfo_name)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_RouterInterface, lookupRoute,     arginfo_routerinterface_lookuproute)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_RouterInterface, relativePathFor, arginfo_routerinterface_pathfor)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_RouterInterface, pathFor,         arginfo_routerinterface_pathfor)
    PHP_FE_END
};
/*
 * interface CookiesInterface
 * {
 *     public function get($name, $default = null);
 *     public function set($name, $value);
 *     public function toHeaders();
 *     public static function parseHeader($header);
 * }
 */
const zend_function_entry fe_TurboSlim_Interfaces_Http_CookiesInterface[] = {
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_Http_CookiesInterface, get,       arginfo_kd)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_Http_CookiesInterface, set,       arginfo_kv)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_Http_CookiesInterface, toHeaders, arginfo_empty)
    ZEND_FENTRY(parseHeader, NULL, arginfo_http_cookiesinterface_parseheader, ZEND_ACC_PUBLIC | ZEND_ACC_ABSTRACT | ZEND_ACC_STATIC)
    PHP_FE_END
};

/*
 * interface EnvironmentInterface
 * {
 *     public static function mock(array $settings = []);
 * }
 */
const zend_function_entry fe_TurboSlim_Interfaces_Http_EnvironmentInterface[] = {
    ZEND_FENTRY(mock, NULL, arginfo_environmentinterface_mock, ZEND_ACC_PUBLIC | ZEND_ACC_ABSTRACT | ZEND_ACC_STATIC)
    PHP_FE_END
};

/*
 * interface HeadersInterface extends CollectionInterface
 * {
 *     public function add($key, $value);
 *     public function normalizeKey($key);
 * }
 */
const zend_function_entry fe_TurboSlim_Interfaces_Http_HeadersInterface[] = {
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_Http_HeadersInterface, add,          arginfo_kv)
    PHP_ABSTRACT_ME(TurboSlim_Interfaces_Http_HeadersInterface, normalizeKey, arginfo_key)
    PHP_FE_END
};
