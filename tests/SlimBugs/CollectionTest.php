<?php
namespace SlimBugs\Tests;

use PHPUnit\Framework\TestCase;

class CollectionTest extends TestCase
{
    /**
     * @expectedException \PHPUnit\Framework\Error\Warning
     * @expectedExceptionMessage array_key_exists() expects parameter 2 to be array, null given
     */
    public function testSlimDataOverwrite()
    {
        $s = 'O:15:"Slim\\Collection":1:{s:7:"' . "\0" . '*' . "\0" . 'data";N;}';
        $c = \unserialize($s);

        $this->assertInstanceOf(\Slim\Collection::class, $c);
        $c->has('something');
    }

    public function testTurboSlimDataOverwrite()
    {
        $s = 'O:20:"TurboSlim\\Collection":1:{s:7:"' . "\0" . '*' . "\0" . 'data";N;}';
        $c = \unserialize($s);

        $this->assertInstanceOf(\TurboSlim\Collection::class, $c);
        $this->assertFalse($c->has('something'));
        $this->assertEquals([], $c->all());
    }
}
