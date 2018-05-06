--TEST--
Indirect FastCollection modifications
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$c = new TurboSlim\FastCollection();
$c[0][0] = 1;                   // type = BP_VAR_W
var_dump($c[0]);                // type = BP_VAR_R

$c[][0] = 2;                    // type = BP_VAR_W, key = nullptr
var_dump($c[1]);                // type = BP_VAR_R

unset($c[3][4]);                // type = BP_VAR_UNSET
var_dump($c[3]);                // type = BP_VAR_R

var_dump($c[4][0]);             // type = BP_VAR_R

var_dump(++$c[5]);              // type = BP_VAR_RW

$c[7] = 7;
var_dump(++$c[7]);              // type = BP_VAR_RW
var_dump(++$c[0][0]);           // type = BP_VAR_RW

$x    = &$c[0];                 // type = BP_VAR_W
$x[0] = 22;
var_dump($c[0]);                // type = BP_VAR_R

$x    = &$c[700];
var_dump($c[700]);
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
int(1)
int(8)
int(2)
array(1) {
  [0]=>
  int(22)
}
NULL
