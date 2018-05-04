<?php
namespace TurboSlim\Tests;

class CallableResolverAwareTraitTest extends \PHPUnit\Framework\TestCase
{
    private $container = null;

    use \TurboSlim\CallableResolverAwareTrait;

    public function testResolveNoContainer()
    {
        $this->container = null;

        $expected = 'callable';
        $actual   = $this->resolveCallable($expected);

        $this->assertEquals($expected, $actual);
    }
}
