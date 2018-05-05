<?php
namespace TurboSlim\Tests;

use TurboSlim\CallableResolver;
use TurboSlim\Tests\Helpers\CallableResolverTestHelper2;

class CallableResolverTest extends \PHPUnit\Framework\TestCase
{
    private $container;
    private $anotherContainer;

    public function __construct($name = null, array $data = [], $dataName = '')
    {
        parent::__construct($name, $data, $dataName);

        $this->container = new \TurboSlim\Container();
        $this->container['test'] = function() { return new CallableResolverTestHelper2(); };

        $this->anotherContainer = new \TurboSlim\Container();
    }

    public function testResolve_StringCallable()
    {
        $resolver = new CallableResolver($this->container);
        $expected = 'time';
        $actual   = $resolver->resolve($expected);
        $this->assertEquals($expected, $actual);
    }

    /**
     * @expectedException \RuntimeException
     * @expectedExceptionMessage bad-callable does not exist
     */
    public function testResolve_InvalidClass()
    {
        $resolver = new CallableResolver($this->container);
        $callable = 'bad-callable';
        $resolver->resolve($callable);
    }

    /**
     * @expectedException \RuntimeException
     * @expectedExceptionMessage [] is not resolvable
     */
    public function testResolve_InvalidNonString()
    {
        $resolver = new CallableResolver($this->container);
        $callable = [];
        $resolver->resolve($callable);
    }

    public function testResolve_SlimCallable()
    {
        $resolver = new CallableResolver($this->container);
        $callable = 'TurboSlim\Tests\Helpers\CallableResolverTestHelper:xxx';
        $ret      = $resolver->resolve($callable);

        $this->assertTrue(\is_array($ret));
        $this->assertTrue(\is_callable($ret));
        $this->assertCount(2, $ret);

        $this->assertEquals(0xB61964F6, $ret());
    }

    public function testResolve_DIC()
    {
        $resolver = new CallableResolver($this->container);
        $callable = 'test';
        $ret      = $resolver->resolve($callable);

        $this->assertTrue(\is_array($ret));
        $this->assertTrue(\is_callable($ret));
        $this->assertCount(2, $ret);

        $this->assertEquals(36, $ret());
    }

    /**
     * @expectedException \RuntimeException
     * @expectedExceptionMessage  is not resolvable
     */
    public function testResolve_Bool()
    {
        $resolver = new CallableResolver($this->container);
        $callable = false;
        $resolver->resolve($callable);
    }

    public function testCompare()
    {
        $c1 = new CallableResolver($this->container);
        $this->assertTrue($c1 == $c1);
        $this->assertTrue($c1 === $c1);

        $c2 = new CallableResolver($this->container);
        $this->assertTrue($c1 == $c2);
        $this->assertTrue($c1 !== $c2);

        $c3 = new CallableResolver($this->anotherContainer);
        $this->assertTrue($c1 != $c3);
        $this->assertTrue($c1 !== $c3);

        $c2->property = 'value';
        $this->assertTrue($c1 != $c2);
    }
}
