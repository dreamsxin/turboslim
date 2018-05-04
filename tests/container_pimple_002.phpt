--TEST--
Test for constructor
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$p = new TurboSlim\Container();
// Pimple C extension returns EG(uninitialized_zval) for integer indices
// This is inconsistent with the PHP implementation, which throws an exception
try {
	var_dump($p[42]);
	echo "FAIL", PHP_EOL;
}
catch (Psr\Container\NotFoundExceptionInterface $e) {
	echo 'NULL', PHP_EOL;
}

$p = new TurboSlim\Container([42 => 'foo']);
var_dump($p[42]);
?>
--EXPECT--
NULL
string(3) "foo"
