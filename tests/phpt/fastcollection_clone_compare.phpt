--TEST--
FastCollection clone/compare
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$c = new \TurboSlim\FastCollection(['a' => 'b']);
$d = clone $c;
var_dump($c);
var_dump($d);
var_dump($c == $d);
var_dump($c !== $d);

$d->prop = [];
var_dump($c == $d);
var_dump($d);

$d->set('c', 'd');
var_dump($c != $d);
?>
--EXPECTF--
object(TurboSlim\FastCollection)#1 (1) {
  ["a"]=>
  string(1) "b"
}
object(TurboSlim\FastCollection)#2 (1) {
  ["a"]=>
  string(1) "b"
}
bool(true)
bool(true)
bool(true)
object(TurboSlim\FastCollection)#2 (1) {
  ["a"]=>
  string(1) "b"
}
bool(true)
