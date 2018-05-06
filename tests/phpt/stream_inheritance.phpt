--TEST--
Stream Inheritance
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
class MyStream extends \TurboSlim\Http\Stream
{
	public function __toString()
	{
		return 'XXX';
	}
}

$f = fopen('php://memory', 'rw');
fwrite($f, 'Hello!');
rewind($f);

$s1 = new MyStream($f);
echo $s1, PHP_EOL;

$s2 = new \TurboSlim\Http\Stream($f);
echo $s2, PHP_EOL;

echo $s1, PHP_EOL;
?>
--EXPECT--
XXX
Hello!
XXX
