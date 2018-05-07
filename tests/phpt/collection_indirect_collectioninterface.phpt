--TEST--
Indirect Collection modifications (CollectionInterface)
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$c = new \TurboSlim\Collection();
$c->get(0)[0] = 1;
var_dump($c[0]);

$c->get(null)[0] = 2;
var_dump($c->all());

unset($c->get(3)[4]);
var_dump($c->get(3));
var_dump($c->get(4)[0]);
// var_dump(++$c->get(5)); // Can't use method return value in write context

// Can't use method return value in write context
/*
$c[7] = 7;
var_dump(++$c->get(7));
*/
$c->set(0, [0 => 1]);
var_dump(++$c->get(0)[0]);
var_dump($c->get(0));

$x    = &$c->get(0);
$x[0] = 22;
var_dump($c[0]);
?>
--EXPECTF--
NULL
array(0) {
}
NULL
NULL
int(2)
array(1) {
  [0]=>
  int(1)
}

Notice: Only variables should be assigned by reference in %s on line %d
array(1) {
  [0]=>
  int(1)
}
