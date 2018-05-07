<?php
namespace TurboSlim\Tests\Helpers;

class FakeResolver implements \TurboSlim\Interfaces\CallableResolverInterface
{
    public function resolve($s)
    {
        return $s;
    }
}
