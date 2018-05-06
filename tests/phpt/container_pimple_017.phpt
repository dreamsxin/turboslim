--TEST--
Test extend() with exception in service extension
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$p = new TurboSlim\Container();

$p[12] = function ($v) { return 'foo'; };
$c = $p->extend(12, function ($w) { throw new BadMethodCallException(); });

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
