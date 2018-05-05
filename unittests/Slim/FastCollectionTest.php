<?php
namespace Slim\Tests;

use TurboSlim\FastCollection as Collection;

class FastCollectionTest extends \PHPUnit\Framework\TestCase
{
    /**
     * @var Collection
     */
    protected $bag;

    public function setUp()
    {
        $this->bag = new Collection();
    }

    public function testInitializeWithData()
    {
        $bag = new Collection(['foo' => 'bar']);
        $this->assertEquals(['foo' => 'bar'], $bag->all());
    }

    public function testSet()
    {
        $this->bag->set('foo', 'bar');
        $this->assertArrayHasKey('foo', $this->bag->all());
        $bag = $this->bag->all();
        $this->assertEquals('bar', $bag['foo']);
    }

    public function testOffsetSet()
    {
        $this->bag['foo'] = 'bar';
        $this->assertArrayHasKey('foo', $this->bag->all());
        $bag = $this->bag->all();
        $this->assertEquals('bar', $bag['foo']);
    }

    public function testGet()
    {
        $this->bag->replace(['foo' => 'bar']);
        $this->assertEquals('bar', $this->bag->get('foo'));
    }

    public function testGetWithDefault()
    {
        $this->bag->replace(['foo' => 'bar']);
        $this->assertEquals('default', $this->bag->get('abc', 'default'));
    }

    public function testReplace()
    {
        $this->bag->replace([
            'abc' => '123',
            'foo' => 'bar',
        ]);
        $this->assertArrayHasKey('abc', $this->bag->all());
        $this->assertArrayHasKey('foo', $this->bag->all());
        $bag = $this->bag->all();
        $this->assertEquals('123', $bag['abc']);
        $this->assertEquals('bar', $bag['foo']);
    }

    public function testAll()
    {
        $data = [
            'abc' => '123',
            'foo' => 'bar',
        ];
        $this->bag->clear();
        $this->bag->replace($data);
        $this->assertEquals($data, $this->bag->all());
    }

    public function testKeys()
    {
        $data = [
            'abc' => '123',
            'foo' => 'bar',
        ];
        $this->bag->clear();
        $this->bag->replace($data);
        $this->assertEquals(['abc', 'foo'], $this->bag->keys());
    }

    public function testHas()
    {
        $this->bag->clear();
        $this->bag->replace(['foo' => 'bar']);
        $this->assertTrue($this->bag->has('foo'));
        $this->assertFalse($this->bag->has('abc'));
    }

    public function testOffsetExists()
    {
        $this->bag->clear();
        $this->bag->replace(['foo' => 'bar']);
        $this->assertTrue(isset($this->bag['foo']));
    }

    public function testRemove()
    {
        $data = [
            'abc' => '123',
            'foo' => 'bar',
        ];
        $this->bag->clear();
        $this->bag->replace($data);
        $this->bag->remove('foo');
        $this->assertEquals(['abc' => '123'], $this->bag->all());
    }

    public function testOffsetUnset()
    {
        $data = [
            'abc' => '123',
            'foo' => 'bar',
        ];
        $this->bag->clear();
        $this->bag->replace($data);

        unset($this->bag['foo']);
        $this->assertEquals(['abc' => '123'], $this->bag->all());
    }

    public function testClear()
    {
        $data = [
            'abc' => '123',
            'foo' => 'bar',
        ];

        $bag = new Collection($data);
        $bag->clear();
        $this->assertEquals([], $bag->all());
    }

    public function testCount()
    {
        $data = [
            'abc' => '123',
            'foo' => 'bar',
        ];
        $this->bag->clear();
        $this->bag->replace($data);

        $this->assertEquals(2, $this->bag->count());
    }
}
