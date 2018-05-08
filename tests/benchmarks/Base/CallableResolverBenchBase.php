<?php
namespace TurboSlim\Benchmarks\Base;

/**
 * @Revs(1000000)
 * @Iterations(5)
 * @Warmup(2)
 * @Groups({"CallableResolver"})
 * @OutputMode("throughput")
 * @OutputTimeUnit("seconds", precision=1)
 */
abstract class CallableResolverBenchBase
{
    protected $x;
    protected $invokable;

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
