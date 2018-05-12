#ifndef PHP_TURBOSLIM_H
#define PHP_TURBOSLIM_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef COVERAGE
extern void __gcov_flush(void);
#endif

#include <main/php.h>

#define PHP_TURBOSLIM_EXTNAME  "turboslim"
#define PHP_TURBOSLIM_EXTVER   "0.1"

#if defined(__GNUC__) && __GNUC__ >= 4
#   define TURBOSLIM_VISIBILITY_HIDDEN          __attribute__((visibility("hidden")))
#else
#   define TURBOSLIM_VISIBILITY_HIDDEN
#endif

#if defined(__GNUC__) || defined(__clang__) || __has_attribute(nonnull)
#   define TURBOSLIM_ATTR_NONNULL               __attribute__((nonnull))
#   define TURBOSLIM_ATTR_NONNULL1(x)           __attribute__((nonnull (x)))
#   define TURBOSLIM_ATTR_NONNULL2(x, y)        __attribute__((nonnull (x, y)))
#   define TURBOSLIM_ATTR_NONNULL3(x, y, z)     __attribute__((nonnull (x, y, z)))
#else
#   define TURBOSLIM_ATTR_NONNULL
#   define TURBOSLIM_ATTR_NONNULL1(x)
#   define TURBOSLIM_ATTR_NONNULL2(x, y)
#   define TURBOSLIM_ATTR_NONNULL3(x, y, z)
#endif

#if defined(__GNUC__) && ((__GNUC__ == 4 && __GNUC_MINOR__ >= 9) || __GNUC__ > 4) || __has_attribute(returns_nonnull)
#   define TURBOSLIM_ATTR_RETURNS_NONNULL       __attribute__((returns_nonnull))
#else
#   define TURBOSLIM_ATTR_RETURNS_NONNULL
#endif

#ifdef COMPILE_DL_TURBOSLIM
TURBOSLIM_VISIBILITY_HIDDEN
#endif
extern zend_module_entry turboslim_module_entry;

ZEND_BEGIN_MODULE_GLOBALS(turboslim)
    int slim_interfaces_mocked;
ZEND_END_MODULE_GLOBALS(turboslim)

TURBOSLIM_VISIBILITY_HIDDEN ZEND_EXTERN_MODULE_GLOBALS(turboslim)

#ifdef ZTS
#include "TSRM.h"
#define TURBOSLIM_G(v) TSRMG(turboslim_globals_id, zend_turboslim_globals*, v)
#else
#define TURBOSLIM_G(v) (turboslim_globals.v)
#endif

#endif /* PHP_TURBOSLIM_H */
