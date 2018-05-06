--TEST--
Test protect()
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$p = new TurboSlim\Container();
$f = function () { return 'foo'; };
$p['foo'] = $p->protect($f);
var_dump($p['foo']);
?>
--EXPECTF--
object(Closure)#%d (0) {
}
