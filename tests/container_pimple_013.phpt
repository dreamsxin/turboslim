--TEST--
Test keys()
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

$p = new TurboSlim\Container();

var_dump($p->keys());

$p['foo'] = 'bar';
$p[] = 'foo';

var_dump($p->keys());

unset($p['foo']);

var_dump($p->keys());
?>
--EXPECT--
array(0) {
}
array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  int(0)
}
array(1) {
  [0]=>
  int(0)
}