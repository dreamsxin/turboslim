--TEST--
Indirect FastCollection modifications (ArrayAccess)
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$c = new TurboSlim\FastCollection();
$c->offsetGet(0)[0] = 1;
var_dump($c->offsetGet(0));

$c->offsetGet(null)[0] = 2;
var_dump($c->offsetGet(1));

unset($c->offsetGet(3)[4]);
var_dump($c->offsetGet(3));

var_dump($c->offsetGet(4)[0]);

var_dump(++$c->offsetGet(0)[0]);

$x    = &$c->offsetGet(0);
$x[0] = 22;
var_dump($c[0]);

$x    = &$c->offsetGet(700);
var_dump($c->offsetGet(700));
?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(2)
}
NULL
NULL
int(2)
array(1) {
  [0]=>
  int(22)
}
NULL
