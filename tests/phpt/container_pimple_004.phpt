--TEST--
Test has/unset dim handlers
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$p = new TurboSlim\Container();
$p[] = 42;
var_dump($p[0]);
unset($p[0]);
var_dump(isset($p[0])); // var_dump($p[0]) as in the original test won't work, as we treat all keys the same way
$p['foo'] = 'bar';
var_dump(isset($p['foo']));
unset($p['foo']);
try {
	var_dump($p['foo']);
	echo "Excpected exception", PHP_EOL;
}
catch (Psr\Container\NotFoundExceptionInterface $e) {
}

var_dump(isset($p['bar']));
$p['bar'] = NULL;
var_dump(isset($p['bar']));
var_dump(empty($p['bar']));
?>
--EXPECT--
int(42)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
