<?php
namespace TurboSlim\Benchmarks;

use TurboSlim\Benchmarks\Base\DeferredCallableBenchBase;

class SlimDeferredCallableBench extends DeferredCallableBenchBase
{
    protected $dc_class        = \Slim\DeferredCallable::class;
    protected $container_class = \Slim\Container::class;
    protected $cr_class        = \Slim\CallableResolver::class;
}
