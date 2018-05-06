PHP_ARG_ENABLE(turboslim, whether to enable TurboSlim extension [ --enable-turboslim  Enable TurboSlim extension])

if test "$PHP_TURBOSLIM" = "yes"; then
	AC_DEFINE([HAVE_TURBOSLIM], [1], [Whether TurboSlim extension is enabled])
	turboslim_src="
main.c
callableresolver.c
callablewrapper.c
collection.c
container.c
deferredcallable.c
fastcollection.c
exception/containerexception.c
exception/containervaluenotfoundexception.c
exception/turboslimexception.c
exception/invalidmethodexception.c
exception/notfoundexception.c
exception/methodnotallowedexception.c
http/stream.c
internal/closure.c
internal/container.c
internal/internalclass.c
interfaces.c
persistent.c
psr11.c
psr7.c
traits/callableresolverawaretrait.c
"
	PHP_NEW_EXTENSION([turboslim], $turboslim_src, $ext_shared,, [-Wall -std=gnu99])
	PHP_SUBST(TURBOSLIM_SHARED_LIBADD)
fi
