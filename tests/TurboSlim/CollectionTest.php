<?php
namespace TurboSlim\Tests;

use TurboSlim\Collection;
use TurboSlim\Tests\Helpers\CloneCompareTestTrait;

class CollectionTest extends \PHPUnit\Framework\TestCase
{
    use CloneCompareTestTrait;

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
}
