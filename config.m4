PHP_ARG_ENABLE(turboslim, whether to enable TurboSlim extension [ --enable-turboslim  Enable TurboSlim extension])

if test "$PHP_TURBOSLIM" = "yes"; then
	AC_DEFINE([HAVE_TURBOSLIM], [1], [Whether TurboSlim extension is enabled])
	turboslim_src="
main.c
init.c
persistent.c
turboslim/callableresolver.c
turboslim/collection.c
turboslim/container.c
turboslim/deferredcallable.c
turboslim/fastcollection.c
turboslim/exception/containerexception.c
turboslim/exception/containervaluenotfoundexception.c
turboslim/exception/turboslimexception.c
turboslim/exception/invalidmethodexception.c
turboslim/exception/notfoundexception.c
turboslim/exception/methodnotallowedexception.c
turboslim/http/body.c
turboslim/http/stream.c
turboslim/internal/callablewrapper.c
turboslim/internal/closure.c
turboslim/internal/container.c
turboslim/internal/internalclass.c
turboslim/interfaces.c
turboslim/psr11.c
turboslim/psr7.c
turboslim/traits/callableresolverawaretrait.c
"
	PHP_NEW_EXTENSION([turboslim], $turboslim_src, $ext_shared,, [-Wall -std=gnu99])
	PHP_SUBST(TURBOSLIM_SHARED_LIBADD)
fi
