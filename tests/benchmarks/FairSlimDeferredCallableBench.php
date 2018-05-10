<?php
namespace TurboSlim\Benchmarks;

use TurboSlim\Benchmarks\Base\DeferredCallableBenchBase;

class FairSlimDeferredCallableBench extends DeferredCallableBenchBase
{
    protected $dc_class        = \Slim\DeferredCallable::class;
    protected $container_class = \TurboSlim\Container::class;
    protected $cr_class        = \TurboSlim\CallableResolver::class;
}
