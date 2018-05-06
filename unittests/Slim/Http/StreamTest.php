<?php
/**
 * Slim Framework (https://slimframework.com)
 *
 * @link      https://github.com/slimphp/Slim
 * @copyright Copyright (c) 2011-2017 Josh Lockhart
 * @license   https://github.com/slimphp/Slim/blob/3.x/LICENSE.md (MIT License)
 */
namespace Slim\Tests\Http;

use TurboSlim\Http\Stream;

class StreamTest extends \PHPUnit\Framework\TestCase
{
    /**
     * @var resource pipe stream file handle
     */
    private $pipeFh;

    /**
     * @var Stream
     */
    private $pipeStream;

    public function tearDown()
    {
        if ($this->pipeFh != null) {
            stream_get_contents($this->pipeFh); // prevent broken pipe error message
        }
    }

    public function testIsPipe()
    {
        $this->openPipeStream();

        $this->assertTrue($this->pipeStream->isPipe());

        $this->pipeStream->detach();
        $this->assertFalse($this->pipeStream->isPipe());

        $fhFile = fopen(__FILE__, 'r');
        $fileStream = new Stream($fhFile);
        $this->assertFalse($fileStream->isPipe());
    }

    public function testIsPipeReadable()
    {
        $this->openPipeStream();

        $this->assertTrue($this->pipeStream->isReadable());
    }

    public function testPipeIsNotSeekable()
    {
        $this->openPipeStream();

        $this->assertFalse($this->pipeStream->isSeekable());
    }

    /**
     * @expectedException \RuntimeException
     */
    public function testCannotSeekPipe()
    {
        $this->openPipeStream();

        $this->pipeStream->seek(0);
    }

    /**
     * Slim thinks you cannot ftell() pipe, which is not true
     */
    public function testCanTellPipe()
    {
        $this->openPipeStream();

        $this->assertEquals(0, $this->pipeStream->tell());
        $this->pipeStream->getContents();
        $this->assertTrue($this->pipeStream->tell() > 2);
    }

    /**
     * @expectedException \RuntimeException
     */
    public function testCannotRewindPipe()
    {
        $this->openPipeStream();

        $this->pipeStream->rewind();
    }

    /**
     * Slim thinks getSize() returns null, but fstat() on a pipe handle returns size = 0
     */
    public function testPipeGetSizeYieldsZero()
    {
        $this->openPipeStream();

        $this->assertEquals(0, $this->pipeStream->getSize());
    }

    public function testClosePipe()
    {
        $this->openPipeStream();

        stream_get_contents($this->pipeFh); // prevent broken pipe error message
        $this->pipeStream->close();
        $this->pipeFh = null;

        $this->assertFalse($this->pipeStream->isPipe());
    }

    public function testPipeToString()
    {
        $this->openPipeStream();

        $this->assertSame('', (string) $this->pipeStream);
    }

    public function testPipeGetContents()
    {
        $this->openPipeStream();

        $contents = \trim($this->pipeStream->getContents());
        $this->assertSame('12', $contents);
    }

    /**
     * Opens the pipe stream
     *
     * @see StreamTest::pipeStream
     */
    private function openPipeStream()
    {
        $this->pipeFh = \popen('echo 12', 'r');
        $this->pipeStream = new Stream($this->pipeFh);
    }
}
