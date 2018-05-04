--TEST--
FastCollection ignores properties
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$c = new TurboSLim\FastCollection;

$c->prop = 1;
var_dump(isset($c->prop));
var_dump($c->prop);
unset($c->data);
?>
--EXPECT--
bool(false)
NULL
