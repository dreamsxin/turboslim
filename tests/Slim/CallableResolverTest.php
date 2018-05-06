<?php
namespace Slim\Tests;

use Slim\Container;
use Slim\Tests\Mocks\CallableTest;
use Slim\Tests\Mocks\InvokableTest;
use TurboSlim\CallableResolver;

class CallableResolverTest extends \PHPUnit\Framework\TestCase
{
    /**
     * @var Container
     */
    private $container;

    public function setUp()
    {
        CallableTest::$CalledCount = 0;
        InvokableTest::$CalledCount = 0;
        $this->container = new Container();
    }

    public function testClosure()
    {
        $test = function () {
            static $called_count = 0;
            return $called_count++;
        };
        $resolver = new CallableResolver($this->container);
        $callable = $resolver->resolve($test);
        $callable();
        $this->assertEquals(1, $callable());
    }

    public function testFunctionName()
    {
        // @codingStandardsIgnoreStart
        function testCallable()
        {
            static $called_count = 0;
            return $called_count++;
        };
        // @codingStandardsIgnoreEnd

        $resolver = new CallableResolver($this->container);
        $callable = $resolver->resolve(__NAMESPACE__ . '\testCallable');
        $callable();
        $this->assertEquals(1, $callable());
    }

    public function testObjMethodArray()
    {
        $obj = new CallableTest();
        $resolver = new CallableResolver($this->container);
        $callable = $resolver->resolve([$obj, 'toCall']);
        $callable();
        $this->assertEquals(1, CallableTest::$CalledCount);
    }

    public function testSlimCallable()
    {
        $resolver = new CallableResolver($this->container);
        $callable = $resolver->resolve('Slim\Tests\Mocks\CallableTest:toCall');
        $callable();
        $this->assertEquals(1, CallableTest::$CalledCount);
    }

    public function testSlimCallableContainer()
    {
        $resolver = new CallableResolver($this->container);
        $resolver->resolve('Slim\Tests\Mocks\CallableTest:toCall');
        $this->assertEquals($this->container, CallableTest::$CalledContainer);
    }

    public function testContainer()
    {
        $this->container['callable_service'] = new CallableTest();
        $resolver = new CallableResolver($this->container);
        $callable = $resolver->resolve('callable_service:toCall');
        $callable();
        $this->assertEquals(1, CallableTest::$CalledCount);
    }

    public function testResolutionToAnInvokableClassInContainer()
    {
        $this->container['an_invokable'] = function ($c) {
            return new InvokableTest();
        };
        $resolver = new CallableResolver($this->container);
        $callable = $resolver->resolve('an_invokable');
        $callable();
        $this->assertEquals(1, InvokableTest::$CalledCount);
    }

    public function testResolutionToAnInvokableClass()
    {
        $resolver = new CallableResolver($this->container);
        $callable = $resolver->resolve('Slim\Tests\Mocks\InvokableTest');
        $callable();
        $this->assertEquals(1, InvokableTest::$CalledCount);
    }

    /**
     * @expectedException \RuntimeException
     */
    public function testMethodNotFoundThrowException()
    {
        $this->container['callable_service'] = new CallableTest();
        $resolver = new CallableResolver($this->container);
        $resolver->resolve('callable_service:noFound');
    }

    /**
     * @expectedException \RuntimeException
     */
    public function testFunctionNotFoundThrowException()
    {
        $resolver = new CallableResolver($this->container);
        $resolver->resolve('noFound');
    }

    /**
     * @expectedException \RuntimeException
     * @expectedExceptionMessage Callable Unknown does not exist
     */
    public function testClassNotFoundThrowException()
    {
        $resolver = new CallableResolver($this->container);
        $resolver->resolve('Unknown:notFound');
    }

    /**
     * @expectedException \RuntimeException
     * @expectedExceptionMessage is not resolvable
     */
    public function testCallableClassNotFoundThrowException()
    {
        $resolver = new CallableResolver($this->container);
        $resolver->resolve(['Unknown', 'notFound']);
    }

    /**
     * @expectedException \RuntimeException
     * @expectedExceptionMessage is not resolvable
     */
    public function testCallableInvalidTypeThrowException()
    {
        $resolver = new CallableResolver($this->container);
        $resolver->resolve(__LINE__);
    }
}
