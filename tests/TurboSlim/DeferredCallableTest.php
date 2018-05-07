<?php
namespace TurboSlim\Tests;

use TurboSlim\DeferredCallable;
use TurboSlim\Tests\Helpers\CloneCompareTestTrait;

class DeferredCallableTest extends \PHPUnit\Framework\TestCase
{
    use CloneCompareTestTrait;

    public function testBasic()
    {
        $c = new DeferredCallable('xxx');
        $this->assertTrue(\method_exists($c, 'resolveCallable'));

        $container = new class implements \Psr\Container\ContainerInterface {
            public function has($id)
            {
                return false;
            }

            public function get($id)
            {
                if ($id == 'callableResolver') {
                    return new class implements \TurboSlim\Interfaces\CallableResolverInterface {
                        public function resolve($s)
                        {
                            return $s;
                        }
                    };
                }

                return $id;
            }
        };

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
        $container = new class implements \Psr\Container\ContainerInterface {
            public function has($id)
            {
                return false;
            }

            public function get($id)
            {
                return null;
            }
        };

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
        $container = new class implements \Psr\Container\ContainerInterface {
            public function has($id)
            {
                return false;
            }

            public function get($id)
            {
                if ($id == 'callableResolver') {
                    return new class implements \TurboSlim\Interfaces\CallableResolverInterface {
                        public function resolve($s)
                        {
                            return $s;
                        }
                    };
                }

                return $id;
            }
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
    }
}
