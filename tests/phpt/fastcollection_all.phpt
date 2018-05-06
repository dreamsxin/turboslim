--TEST--
FastCollection::all()
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$c = new TurboSlim\FastCollection(['a' => 0, 'b' => 1]);
$all = $c->all();
var_dump($all);
$c['c'] = 3;
var_dump($all);
var_dump($c);
?>
--EXPECTF--
array(2) {
  ["a"]=>
  int(0)
  ["b"]=>
  int(1)
}
array(2) {
  ["a"]=>
  int(0)
  ["b"]=>
  int(1)
}
object(TurboSlim\FastCollection)#%d (3) {
  ["a"]=>
  int(0)
  ["b"]=>
  int(1)
  ["c"]=>
  int(3)
}
