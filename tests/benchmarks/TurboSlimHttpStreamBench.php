<?php
namespace TurboSlim\Benchmarks;

use TurboSlim\Benchmarks\Base\HttpStreamBenchBase;

class TurboSlimHttpStreamBench extends HttpStreamBenchBase
{
    protected $class = \TurboSlim\Http\Stream::class;
}
