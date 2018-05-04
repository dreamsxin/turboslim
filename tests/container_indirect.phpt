--TEST--
Indirect Container modifications
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$c = new TurboSlim\Container();
$c[0][0] = 1;                   // type = BP_VAR_W
var_dump($c[0]);                // type = BP_VAR_R

$c[][0] = 2;                    // type = BP_VAR_W, key = nullptr
var_dump($c[1]);                // type = BP_VAR_R

unset($c[3][4]);                // type = BP_VAR_UNSET
var_dump($c[3]);                // type = BP_VAR_R

try {
	var_dump($c[4][0]);     // type = BP_VAR_R
	echo "FAIL", PHP_EOL;
}
catch (\Psr\Container\ContainerExceptionInterface $e) {
	echo "OK", PHP_EOL;
}

try {
	++$c[5];                // type = BP_VAR_RW
	echo "FAIL", PHP_EOL;
}
catch (\Psr\Container\ContainerExceptionInterface $e) {
	echo "OK", PHP_EOL;
}

$c[7] = 7;
var_dump(++$c[7]);              // type = BP_VAR_RW
var_dump(++$c[0][0]);           // type = BP_VAR_RW

$x    = &$c[0];                 // type = BP_VAR_W
$x[0] = 22;
var_dump($c[0]);                // type = BP_VAR_R

$x    = &$c[700];
var_dump($c[700]);
?>
--EXPECTF--
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(2)
}
NULL
OK

Notice: Indirect modification of overloaded element of TurboSlim\Container has no effect in %s on line %d
OK
int(8)
int(2)
array(1) {
  [0]=>
  int(22)
}
NULL
