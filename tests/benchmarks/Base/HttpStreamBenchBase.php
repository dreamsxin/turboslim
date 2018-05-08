<?php
namespace TurboSlim\Benchmarks\Base;

/**
 * @Revs(100000)
 * @Iterations(5)
 * @Warmup(2)
 * @Groups({"HttpStream"})
 * @OutputMode("throughput")
 * @OutputTimeUnit("seconds", precision=1)
 */
abstract class HttpStreamBenchBase
{
    protected $class;
    protected $content;

    public function __construct()
    {
        $this->content = str_repeat('a', 16384);
    }

    /**
     * @Subject
     */
    public function typicalCycle()
    {
        /**
         * @var \Psr\Http\Message\StreamInterface $stream
         */
        $stream = new $this->class(fopen('php://memory', 'rw'));
        $stream->write($this->content);
        $s = (string)$stream;
        $stream->close();
    }
}
