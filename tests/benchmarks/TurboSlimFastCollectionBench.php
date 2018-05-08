<?php
namespace TurboSlim\Benchmarks;

use TurboSlim\FastCollection;
use TurboSlim\Benchmarks\Base\CollectionBenchBase;

class TurboSlimFastCollectionBench extends CollectionBenchBase
{
    public function __construct()
    {
        $array = ['a' => 1, 'b' => 2, 3 => 3];
        $this->x = new FastCollection($array);
    }
}
