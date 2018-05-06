--TEST--
TurboSlim\Http\Stream::__clone
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$s = new \TurboSlim\Http\Stream(fopen('php://memory', 'r+'));
$s->test = 4;

$v = clone $s;
var_dump($v->test);
var_dump($s->isReadable() == $v->isReadable());
var_dump($s->isWritable() == $v->isWritable());
var_dump($s->isSeekable() == $v->isSeekable());
var_dump($s->isPipe() == $v->isPipe());
?>
--EXPECT--
int(4)
bool(true)
bool(true)
bool(true)
bool(true)
