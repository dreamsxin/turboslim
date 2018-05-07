<?php
namespace TurboSlim\Tests\Http;

use TurboSlim\Http\Body;
use TurboSlim\Http\Stream;
use TurboSlim\Tests\Helpers\CloneCompareTestTrait;

class BodyTest extends \PHPUnit\Framework\TestCase
{
    use CloneCompareTestTrait;

    public function testCloneCompare()
    {
        $orig = new Body(fopen('php://memory', 'r'));
        $this->checkCloneCompare($orig);
    }

    public function testCompare()
    {
        $v1 = new Body(fopen('php://memory', 'r'));
        $v2 = new Body(fopen('php://memory', 'r'));
        $v3 = clone $v2;

        $this->assertTrue($v1 != $v2);
        $this->assertTrue($v2 == $v3);

        $v3->detach();
        $this->assertTrue($v2 != $v3);

        $f = fopen('php://memory', 'r');
        $v4 = new Body($f);
        $v5 = new Body($f);
        $this->assertTrue($v4 == $v5);

        $v4->property = 1;
        $this->assertTrue($v4 != $v5);

        unset($v4->property);
        $this->assertTrue($v4 == $v5);

        $v5->detach();
        $this->assertTrue($v4 != $v5);
    }

    public function testCompareParent()
    {
        $f  = fopen('php://input', 'r');
        $v1 = new Stream($f);
        $v2 = new Body($f);

        $this->assertTrue($v1 != $v2);
    }
}
