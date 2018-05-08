<?php
namespace TurboSlim\Benchmarks;

use TurboSlim\Benchmarks\Base\HttpStreamBenchBase;

class SlimHttpStreamBench extends HttpStreamBenchBase
{
    protected $class = \Slim\Http\Stream::class;
}
