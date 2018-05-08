<?php
namespace TurboSlim\Benchmarks;

use Slim\CallableResolver;
use TurboSlim\Container;
use TurboSlim\Benchmarks\Base\CallableResolverBenchBase;

class FairSlimCallableResolverBench extends CallableResolverBenchBase
{
    public function __construct()
    {
        $container = new Container();
        $container['x'] = new class {
            public function method()
            {
            }
        };

        $this->x = new CallableResolver($container);

        $this->invokable = new class {
            public function __invoke()
            {
            }
        };
    }
}
