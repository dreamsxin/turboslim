--TEST--
DeferredCallable and Inheritance
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

class X extends \TurboSlim\DeferredCallable
{
	public $container = 4;
}

$x = new X('time');
echo $x(), PHP_EOL;
echo $x->container, PHP_EOL;
?>
--EXPECTF--
%d
4
