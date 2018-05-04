--TEST--
Indirect Container modifications (ArrayAccess interface)
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
class MyContainer extends TurboSlim\Container
{
	/*
	 * This will behave very differently than TurboSlim\Container::offsetGet()
	 * TurboSlim\Container::offsetGet() has no way to get the context of the operation
	 * and therefore it always assumes BP_VAR_RW
	 */
	public function& offsetGet($key)
	{
		return parent::offsetGet($key);
	}
}

$c = new MyContainer();
try {
	$c[0][0] = 1;    // This will throw
	var_dump($c[0]);
}
catch (\Psr\Container\ContainerExceptionInterface $e) {
	echo "Exception", PHP_EOL;
}

$c[][0] = 2;             // This will work - there is an exception for NULL keys
var_dump($c[0]);

try {
	unset($c[3][4]); // This will throw
	var_dump($c[3]);
}
catch (\Psr\Container\ContainerExceptionInterface $e) {
	echo "Exception", PHP_EOL;
}

try {
	var_dump($c[4][0]);
	echo "FAIL", PHP_EOL;
}
catch (\Psr\Container\ContainerExceptionInterface $e) {
	echo "OK", PHP_EOL;
}

try {
	++$c[5];
	echo "FAIL", PHP_EOL;
}
catch (\Psr\Container\ContainerExceptionInterface $e) {
	echo "OK", PHP_EOL;
}

$c[7] = 7;
var_dump(++$c[7]);
var_dump(++$c[0][0]);

$x    = &$c[0];
$x[0] = 22;
var_dump($c[0]);

try {
	$x    = &$c[700];       // This will throw
	var_dump($c[700]);
}
catch (\Psr\Container\ContainerExceptionInterface $e) {
	echo "Exception", PHP_EOL;
}
?>
--EXPECTF--
Exception
array(1) {
  [0]=>
  int(2)
}
Exception
OK
OK
int(8)
int(3)
array(1) {
  [0]=>
  int(22)
}
Exception

