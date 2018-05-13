<?php
namespace TurboSlim\Tests;

use TurboSlim\Collection;
use TurboSlim\Tests\Helpers\CloneCompareTestTrait;
use TurboSlim\Tests\Helpers\SerializeCompareTestTrait;

class CollectionTest extends \PHPUnit\Framework\TestCase
{
    /**
     * @var Collection
     */
    protected $bag;

    use CloneCompareTestTrait;
    use SerializeCompareTestTrait;

    public function setUp()
    {
        $this->bag = new Collection();
    }

    public function testConstruct()
    {
        $c = new Collection();
        $this->assertSame([], $c->all());

        $expected = ['a' => 1];
        $c = new Collection($expected);
        $this->assertEquals($expected, $c->all());
    }

    public function testIterator()
    {
        $expected = ['a' => 1, 'b' => 2, 'c' => 3];
        $actual   = [];
        $c = new Collection($expected);
        foreach ($c as $k => $v) {
            $actual[$k] = $v;
        }

        $this->assertEquals($expected, $actual);

        $actual = [];
        $it = $c->getIterator();
        foreach ($it as $k => $v) {
            $actual[$k] = $v;
        }

        $this->assertEquals($expected, $actual);
    }

    public function testAccessors()
    {
        $c = new Collection();
        $this->assertFalse($c->has('a'));

        $c->replace(['a' => 1, 'b' => 2, 'c' => 3]);
        $this->assertTrue($c->has('a'));
        $this->assertTrue($c->has('b'));
        $this->assertTrue($c->has('c'));
        $this->assertFalse($c->has('d'));
        $this->assertEquals(1, $c->get('a'));
        $this->assertEquals(2, $c->get('b'));
        $this->assertEquals(3, $c->get('c'));
        $this->assertEquals(null, $c->get('d'));
        $this->assertEquals(-1, $c->get('d', -1));

        $c->set('d', 4);
        $this->assertTrue($c->has('d'));
        $this->assertEquals(4, $c->get('d'));

        $expected = ['a' => 1, 'b' => 2, 'c' => 3, 'd' => 4];
        $actual   = $c->all();
        $this->assertEquals($expected, $actual);

        $expected = \array_keys($expected);
        $actual   = $c->keys();
        $this->assertEquals($expected, $actual);

        $c->remove('a');
        $this->assertFalse($c->has('a'));

        $c->clear();
        $this->assertEmpty($c->keys());
        $this->assertEmpty($c->all());

        $string = 'some string';
        $c[0] = $string;
        $this->assertTrue($c->has(0));
        $string = '';
        $this->assertEquals('some string', $c->get(0));
    }

    public function testCountable()
    {
        $c = new Collection(['a' => 1, 'b' => 2, 'c' => 3]);
        $this->assertEquals(3, \count($c));
        $this->assertEquals(3, $c->count());

        $c->set('a', 2);
        $this->assertEquals(3, \count($c));

        $c->set('d', 4);
        $this->assertEquals(4, \count($c));

        $c->remove('a');
        $this->assertEquals(3, \count($c));

        $c->clear();
        $this->assertEquals(0, \count($c));
    }

    public function testCloneCompare()
    {
        $orig = new Collection(['a' => 'b']);
        $this->checkCloneCompare($orig);
    }

    public function testSerializeCompare()
    {
        $obj = new Collection(['a' => 'b']);
        $this->checkSerializeCompare($obj);
    }

    // Tests adapted from Slim

    public function testInitializeWithData()
    {
        $expected = ['foo' => 'bar'];
        $bag      = new Collection($expected);
        $this->assertEquals($expected, $bag->all());
    }

    public function testSet()
    {
        $this->bag->set('foo', 'bar');
        $bag = $this->bag->all();
        $this->assertArrayHasKey('foo', $bag);
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
        $this->bag->set('foo', 'bar');
        $this->assertEquals('bar', $this->bag->get('foo'));
    }

    public function testGetWithDefault()
    {
        $this->bag->set('foo', 'bar');
        $this->assertEquals('default', $this->bag->get('abc', 'default'));
    }

    public function testReplace()
    {
        $this->bag->replace([
            'abc' => '123',
            'foo' => 'bar',
        ]);

        $bag = $this->bag->all();
        $this->assertArrayHasKey('abc', $bag);
        $this->assertArrayHasKey('foo', $bag);
        $this->assertEquals('123', $bag['abc']);
        $this->assertEquals('bar', $bag['foo']);
    }

    public function testAll()
    {
        $data = [
            'abc' => '123',
            'foo' => 'bar',
        ];

        $this->bag->replace($data);
        $this->assertEquals($data, $this->bag->all());
    }

    public function testKeys()
    {
        $data = [
            'abc' => '123',
            'foo' => 'bar',
        ];

        $this->bag->replace($data);
        $this->assertEquals(['abc', 'foo'], $this->bag->keys());
    }

    public function testHas()
    {
        $this->bag->set('foo', 'bar');
        $this->assertTrue($this->bag->has('foo'));
        $this->assertFalse($this->bag->has('abc'));
    }

    public function testOffsetExists()
    {
        $this->bag->set('foo', 'bar');
        $this->assertTrue(isset($this->bag['foo']));
    }

    public function testRemove()
    {
        $data = [
            'abc' => '123',
            'foo' => 'bar',
        ];
        $this->bag->replace($data);
        $this->bag->remove('foo');
        $this->assertEquals(['abc' => '123'], $this->bag->all());
        $this->assertNotEquals($data, $this->bag->all());
    }

    public function testOffsetUnset()
    {
        $data = [
            'abc' => '123',
            'foo' => 'bar',
        ];
        $this->bag->replace($data);
        unset($this->bag['foo']);
        $this->assertEquals(['abc' => '123'], $this->bag->all());
        $this->assertNotEquals($data, $this->bag->all());
    }

    public function testClear()
    {
        $data = [
            'abc' => '123',
            'foo' => 'bar',
        ];
        $this->bag->replace($data);
        $this->bag->clear();
        $this->assertEquals([], $this->bag->all());
    }

    public function testCount()
    {
        $data = [
            'abc' => '123',
            'foo' => 'bar',
        ];
        $this->bag->replace($data);

        $this->assertEquals(2, $this->bag->count());
        $this->assertEquals(2, count($this->bag));
    }
}
