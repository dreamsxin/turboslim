<?php
namespace TurboSlim\Tests;

use TurboSlim\DeferredCallable;
use TurboSlim\Tests\Helpers\CloneCompareTestTrait;
use TurboSlim\Tests\Helpers\FakeResolver;
use TurboSlim\Tests\Helpers\MyDeferredCallable;

class DeferredCallableTest extends \PHPUnit\Framework\TestCase
{
    use CloneCompareTestTrait;

    public function testBasic()
    {
        $container = new \TurboSlim\Container();
        $container['callableResolver'] = function() {
            return new FakeResolver();
        };

        $c = new DeferredCallable('xxx');
        $this->assertTrue(\method_exists($c, 'resolveCallable'));

        $callable = function() {
            return "I am a closure, \$this is " . (isset($this) ? 'not NULL' : 'NULL');
        };

        $c = new DeferredCallable($callable, $container);
        $actual   = $c();
        $expected = "I am a closure, \$this is not NULL";
        $this->assertEquals($expected, $actual);

        /* Trigger (func->op_array.fn_flags & ZEND_ACC_NO_RT_ARENA) branch */
        $callable = \Closure::bind($callable, $container, \PHPUnit\Framework\TestCase::class);

        $c = new DeferredCallable($callable, null);
        $actual   = $c();
        $expected = "I am a closure, \$this is NULL";
        $this->assertEquals($expected, $actual);
    }

    /**
     * @expectedException \UnexpectedValueException
     */
    public function testUnexpectedValueException()
    {
        $container = new \TurboSlim\Container();
        $container['callableResolver'] = null;

        $callable = function() {
            return "I am a closure";
        };

        $c = new DeferredCallable($callable, $container);
        $c();
    }

    /**
     * @expectedException PHPUnit\Framework\Error\Warning
     */
    public function testBadCallable()
    {
        $container = new \TurboSlim\Container();
        $container['callableResolver'] = function() {
            return new FakeResolver();
        };

        $c = new \TurboSlim\DeferredCallable('bad-callable', $container);
        $c();
        // E_WARNING: function 'bad-callable' not found or invalid function name
    }

    public function testCloneCompare()
    {
        $orig = new DeferredCallable('time');
        $this->checkCloneCompare($orig);
    }

    public function testCompare()
    {
        $a = new DeferredCallable('time');
        $b = new DeferredCallable('date');
        $c = new DeferredCallable('date');

        $this->assertTrue($a != $b);
        $this->assertTrue($b == $c);

        $b->prop = 'val';
        $this->assertTrue($b != $c);

        $d = new MyDeferredCallable('date');
        $this->assertTrue($b != $d);
        $this->assertTrue($c != $d);
    }
}
