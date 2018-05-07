--TEST--
FastCollection serialize/unserialize
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$c = new \TurboSlim\FastCollection(['a' => 'b']);
$s = serialize($c);
$d = unserialize($s);
var_dump($c);
var_dump($d);
var_dump($c == $d);
var_dump($c !== $d);
?>
--EXPECTF--
object(TurboSlim\FastCollection)#%d (1) {
  ["a"]=>
  string(1) "b"
}
object(TurboSlim\FastCollection)#%d (1) {
  ["a"]=>
  string(1) "b"
}
bool(true)
bool(true)
