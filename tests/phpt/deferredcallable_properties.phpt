--TEST--
DeferredCallable's properties
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$x = new \TurboSlim\DeferredCallable('time');
var_dump($x);
?>
--EXPECTF--
object(TurboSlim\DeferredCallable)#%d (2) {
  ["callable":"TurboSlim\DeferredCallable":private]=>
  string(4) "time"
  ["container":"TurboSlim\DeferredCallable":private]=>
  NULL
}
