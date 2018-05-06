<?php
namespace SlimBugs\Tests;

class StreamTest extends \PHPUnit\Framework\TestCase
{
    /**
     * @expectedException \PHPUnit\Framework\Error\Warning
     * @expectedExceptionMessage supplied resource is not a valid stream resource
     * @requires function \imagecreate
     */
    public function testSlimWrongResource()
    {
        $r = \imagecreate(1, 1);
        $stream = new \Slim\Http\Stream($r);
        $stream->isReadable();
    }

    /**
     * @expectedException \InvalidArgumentException
     * @expectedExceptionMessage argument is not a stream
     * @requires function \imagecreate
     */
    public function testTurboSlimWrongResource()
    {
        $r = \imagecreate(1, 1);
        $stream = new \TurboSlim\Http\Stream($r);
    }
}
