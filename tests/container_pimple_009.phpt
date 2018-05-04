--TEST--
Test service is called as callback, and only once
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$p = new TurboSlim\Container();
$p['foo'] = function($arg) use ($p) { var_dump($p === $arg); };
$a = $p['foo'];
$b = $p['foo']; /* should return not calling the callback */
?>
--EXPECT--
bool(true)
