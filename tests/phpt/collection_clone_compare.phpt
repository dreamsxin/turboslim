--TEST--
Collection clone/compare
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$c = new \TurboSlim\Collection(['a' => 'b']);
$d = clone $c;
var_dump($c);
var_dump($d);
var_dump($c == $d);
var_dump($c !== $d);

$d->prop = [];
var_dump($c == $d);

$e = clone($d);
var_dump($d);
var_dump($e);
var_dump($d == $e);
var_dump($c != $e);

$e->set('c', 'd');
var_dump($d != $e);
?>
--EXPECTF--
object(TurboSlim\Collection)#%d (1) {
  ["data":protected]=>
  array(1) {
    ["a"]=>
    string(1) "b"
  }
}
object(TurboSlim\Collection)#%d (1) {
  ["data":protected]=>
  array(1) {
    ["a"]=>
    string(1) "b"
  }
}
bool(true)
bool(true)
bool(false)
object(TurboSlim\Collection)#%d (2) {
  ["data":protected]=>
  array(1) {
    ["a"]=>
    string(1) "b"
  }
  ["prop"]=>
  array(0) {
  }
}
object(TurboSlim\Collection)#%d (2) {
  ["data":protected]=>
  array(1) {
    ["a"]=>
    string(1) "b"
  }
  ["prop"]=>
  array(0) {
  }
}
bool(true)
bool(true)
bool(true)
