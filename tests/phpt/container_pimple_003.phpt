--TEST--
Test empty dimensions
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$p = new TurboSlim\Container();
$p[] = 42;
var_dump($p[0]);
$p[41] = 'foo';
$p[] = 'bar';
var_dump($p[42]);
?>
--EXPECT--
int(42)
string(3) "bar"
