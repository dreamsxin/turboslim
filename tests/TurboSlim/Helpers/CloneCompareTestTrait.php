<?php
namespace TurboSlim\Tests\Helpers;

trait CloneCompareTestTrait
{
    private function checkCloneCompare($orig)
    {
        $clone = clone $orig;
        $this->assertTrue($clone == $orig);

        $clone->property = 0xB61964F6;
        $this->assertTrue($clone != $orig);

        $second = clone $clone;
        $this->assertTrue($second == $clone);
        $this->assertEquals($second->property, $clone->property);
        $this->assertTrue($second != $orig);
    }
}