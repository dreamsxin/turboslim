--TEST--
Test extend() with exception in service factory
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$p = new TurboSlim\Container();
$p[12] = function ($v) { throw new BadMethodCallException(); };

$c = $p->extend(12, function ($w) { return 'foobar'; });

try {
	$p[12];
	echo "Exception expected", PHP_EOL;
}
catch (BadMethodCallException $e) {
	echo "OK", PHP_EOL;
}
?>
--EXPECT--
OK
