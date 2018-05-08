<?php
namespace TurboSlim\Benchmarks;

use TurboSlim\Collection;
use TurboSlim\Benchmarks\Base\CollectionBenchBase;

class TurboSlimCollectionBench extends CollectionBenchBase
{
    public function __construct()
    {
        $array = ['a' => 1, 'b' => 2, 3 => 3];
        $this->x = new Collection($array);
    }
}
