<?php
namespace TurboSlim\Benchmarks\Base;

/**
 * @Revs(1000000)
 * @Iterations(5)
 * @Warmup(2)
 * @Groups({"Container"})
 * @OutputMode("throughput")
 * @OutputTimeUnit("seconds", precision=1)
 */
abstract class ContainerBenchBase
{
    protected $x;

    /**
     * @Subject
     */
    public function arrayGetShared()
    {
        $this->x['shared'];
    }

    /**
     * @Subject
     */
    public function arrayGetFactory()
    {
        $this->x['factory'];
    }

    /**
     * @Subject
     */
    public function offsetGetShared()
    {
        $this->x->offsetGet('shared');
    }

    /**
     * @Subject
     */
    public function offsetGetFactory()
    {
        $this->x->offsetGet('factory');
    }

    /**
     * @Subject
     */
    public function arrayExists()
    {
        isset($this->x['factory']);
    }

    /**
     * @Subject
     */
    public function offsetExists()
    {
        $this->x->offsetExists('factory');
    }

    /**
     * @Subject
     */
    public function arraySet()
    {
        $this->x['x'] = function() {};
    }

    /**
     * @Subject
     */
    public function offsetSet()
    {
        $this->x->offsetSet('x', function() {});
    }
}
