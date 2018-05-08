<?php
namespace TurboSlim\Benchmarks\Base;

/**
 * @Revs(1000000)
 * @Iterations(5)
 * @Warmup(2)
 * @Groups({"Collection"})
 * @OutputMode("throughput")
 * @OutputTimeUnit("seconds", precision=1)
 */
abstract class CollectionBenchBase
{
    protected $x;

    /**
     * @Subject
     */
    public function has_str()
    {
        $this->x->has('a');
    }

    /**
     * @Subject
     */
    public function arrayExists_str()
    {
        isset($this->x['a']);
    }

    /**
     * @Subject
     */
    public function has_idx()
    {
        $this->x->has(1);
    }

    /**
     * @Subject
     */
    public function arrayExists_idx()
    {
        isset($this->x[1]);
    }
}
