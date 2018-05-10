<?php
namespace TurboSlim\Benchmarks;

use TurboSlim\CallableResolver;
use TurboSlim\Container;
use TurboSlim\Benchmarks\Base\CallableResolverBenchBase;

class TurboSlimCallableResolverBench extends CallableResolverBenchBase
{
    public function __construct()
    {
        parent::__construct();

        $container = new Container();
        $container['x'] = new class {
            public function method()
            {
            }
        };

        $this->x = new CallableResolver($container);
    }

    /**
     * @Subject
     */
    public function resolveStringCallable()
    {
        $this->x->resolve('time');
    }

    /**
     * @Revs(100000)
     * @Subject
     */
    public function resolveStringNotCallable()
    {
        try {
            $this->x->resolve('bad-callable');
        }
        catch (\Throwable $e) {
        }
    }

    /**
     * @Subject
     */
    public function resolveClosure()
    {
        $this->x->resolve(function() {});
    }

    /**
     * @Subject
     */
    public function resolveInvokable()
    {
        $this->x->resolve($this->invokable);
    }

    /**
     * @Subject
     * @Revs(100000)
     */
    public function resolveSlimCallable()
    {
        $this->x->resolve('x:method');
    }
}
