--TEST--
ReflectionProperty should not confuse Collection
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$c = new TurboSlim\Collection(['a', 'b', 'c']);
$property = new ReflectionProperty($c, 'data');
$property->setAccessible(true);
$property->setValue($c, 123);
var_dump($c->all());
?>
--EXPECT--
array(1) {
  [0]=>
  int(123)
}
