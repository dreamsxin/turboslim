<?php
namespace TurboSlim\Tests\Helpers;

trait SerializeCompareTestTrait
{
    private function checkSerializeCompare($obj)
    {
        $s = \serialize($obj);
        $r = \unserialize($s);
        $this->assertTrue($obj == $r);
        $this->assertTrue($obj !== $r);

        $r->someProperty = 'value';
        $s = \serialize($r);
        $x = \unserialize($s);
        $this->assertTrue($x == $r);
        $this->assertEquals($r->someProperty, $x->someProperty);
    }
}
