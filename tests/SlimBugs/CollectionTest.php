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

    /**
     * @expectedException \PHPUnit\Framework\Error\Notice
     * @expectedExceptionMessage unserialize(): Error at offset
     */
    public function testTurboSlimDataOverwrite()
    {
        $s = 'C:20:"TurboSlim\\Collection":8:{N;a:0:{}}';
        $c = \unserialize($s);
    }
}
