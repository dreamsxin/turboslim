<?php
namespace TurboSlim\Tests\Http;

use PHPUnit\Framework\TestCase;
use TurboSlim\Http\Cookies;
use TurboSlim\Tests\Helpers\CloneCompareTestTrait;
use TurboSlim\Tests\Helpers\SerializeCompareTestTrait;

class CookiesTest extends TestCase
{
    use CloneCompareTestTrait;
    use SerializeCompareTestTrait;

    public function testCloneCompare()
    {
        $orig = new Cookies(['name' => 'value']);
        $this->checkCloneCompare($orig);
    }

    public function testSerializeCompare()
    {
        $obj = new Cookies(['name' => 'value']);
        $this->checkSerializeCompare($obj);
    }

    public function testCompare()
    {
        $c1 = new Cookies();
        $c2 = new Cookies();

        $this->assertTrue($c1 == $c2);

        $c1->set('name', 'value');
        $this->assertTrue($c1 != $c2);

        $c2->set('name', 'value');
        $this->assertTrue($c1 == $c2);

        $c1->property = 123;
        $this->assertTrue($c1 != $c2);

        $c2->property = 123;
        $this->assertTrue($c1 == $c2);
    }
}
