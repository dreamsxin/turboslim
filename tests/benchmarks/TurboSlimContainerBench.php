<?php
namespace TurboSlim\Benchmarks;

use TurboSlim\Container;
use TurboSlim\Benchmarks\Base\ContainerBenchBase;

class TurboSlimContainerBench extends ContainerBenchBase
{
    public function __construct()
    {
        $this->x = new Container();
        $this->x['factory'] = $this->x->factory(function() {
            return 1;
        });

        $this->x['shared'] = function() {
            return 2;
        };

        $x = $this->x['shared']; // Resolve
    }
}
