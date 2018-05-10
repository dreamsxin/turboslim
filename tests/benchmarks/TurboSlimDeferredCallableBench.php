<?php
namespace TurboSlim\Benchmarks;

use TurboSlim\Benchmarks\Base\DeferredCallableBenchBase;

class TurboSlimDeferredCallableBench extends DeferredCallableBenchBase
{
    protected $dc_class        = \TurboSlim\DeferredCallable::class;
    protected $container_class = \TurboSlim\Container::class;
    protected $cr_class        = \TurboSlim\CallableResolver::class;
}
