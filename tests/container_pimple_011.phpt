--TEST--
Test service callback throwing an exception
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
class CallbackException extends RuntimeException {}

$p = new TurboSlim\Container();
$p['foo'] = function () { throw new CallBackException; };
try {
	echo $p['foo'], PHP_EOL;
	echo "FAIL", PHP_EOL;
}
catch (CallbackException $e) {
	echo "OK", PHP_EOL;
}
?>
--EXPECT--
OK
