<?php
namespace TurboSlim\Benchmarks\Base;

/**
 * @Revs(1000000)
 * @Iterations(5)
 * @Warmup(2)
 * @Groups({"DeferredCallable"})
 * @OutputMode("throughput")
 * @OutputTimeUnit("seconds", precision=1)
 */
abstract class DeferredCallableBenchBase
{
    protected $dc_class;
    protected $container_class;
    protected $cr_class;

    private $container;
    private $invokable;

    public function __construct()
    {
        $this->container = new $this->container_class();
        $this->container['x'] = new class {
            public function method()
            {
            }
        };

        $this->container['callableResolver'] = function($c) {
            return new $this->cr_class($c);
        };

        $this->invokable = new class {
            public function __invoke()
            {
            }
        };
    }

    /**
     * @Subject
     */
    public function stringCallable()
    {
        $x = new $this->dc_class('time');
        $x();
    }

    /**
     * @Subject
     */
    public function closureCallable()
    {
        $x = new $this->dc_class(function() {}, $this->container);
        $x();
    }

    /**
     * @Subject
     */
    public function invokableCallable()
    {
        $x = new $this->dc_class($this->invokable, $this->container);
        $x();
    }

    /**
     * @Subject
     * @Revs(100000)
     */
    public function slimCallable()
    {
        $x = new $this->dc_class('x:method', $this->container);
        $x();
    }
}
