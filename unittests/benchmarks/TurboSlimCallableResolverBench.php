<?php

/**
 * @Revs(1000000)
 * @Iterations(5)
 * @OutputMode("throughput")
 * @OutputTimeUnit("seconds", precision=3)
 * @Groups({"CallableResolver"})
 */
class TurboSlimCallableResolverBench
{
    private $x;

    private $invokable;

    public function __construct()
    {
        $container = new Slim\Container();
        $container['x'] = new class {
            public function method()
            {
            }
        };

        $this->x = new TurboSlim\CallableResolver($container);

        $this->invokable = new class {
            public function __invoke()
            {
            }
        };
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
