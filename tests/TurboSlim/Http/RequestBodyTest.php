<?php
namespace TurboSlim\Tests\Http;

use TurboSlim\Http\Body;
use TurboSlim\Http\RequestBody;
use TurboSlim\Tests\Helpers\CloneCompareTestTrait;
use TurboSlim\Tests\Helpers\MyRequestBody;

class RequestBodyTest extends \PHPUnit\Framework\TestCase
{
    use CloneCompareTestTrait;

    public function testCloneCompare()
    {
        $orig = new RequestBody();
        $this->checkCloneCompare($orig);
    }

    public function testCompare()
    {
        $v1 = new RequestBody();
        $v2 = new RequestBody();
        $v3 = clone $v2;

        $this->assertTrue($v1 != $v2);
        $this->assertTrue($v2 == $v3);

        $v3->detach();
        $this->assertTrue($v2 != $v3);
    }

    public function testCompareParent()
    {
        $f  = fopen('php://input', 'r');
        $v1 = new RequestBody();
        $v2 = new Body($f);

        $this->assertTrue($v1 != $v2);

        $v3 = new MyRequestBody();
        $v3->doAttach($f);

        $this->assertTrue($v1 != $v2);
        $this->assertTrue($v2 != $v3);
        $this->assertTrue($v1 != $v3);
    }
}
