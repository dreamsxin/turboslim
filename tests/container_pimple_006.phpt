--TEST--
Test complex class inheritance
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
class MyContainer extends TurboSlim\Container
{
	public function& offsetGet($o)
	{
		var_dump("hit offsetget in " . __CLASS__);
		return parent::offsetGet($o);
	}
}

class TestContainer extends MyContainer
{
	public function __construct($values)
	{
		array_shift($values);
		parent::__construct($values);
	}

	public function& offsetGet($o)
	{
		var_dump("hit offsetget in " . __CLASS__);
		return parent::offsetGet($o);
	}

	public function offsetSet($o, $v)
	{
		var_dump('hit offsetset');
		return parent::offsetset($o, $v);
	}
}

$defaultValues = ['foo' => 'bar', 88 => 'baz'];

$p = new TestContainer($defaultValues);
$p[42] = 'foo';
var_dump($p[42]);
var_dump($p[0]);
?>
--EXPECT--
string(13) "hit offsetset"
string(30) "hit offsetget in TestContainer"
string(28) "hit offsetget in MyContainer"
string(3) "foo"
string(30) "hit offsetget in TestContainer"
string(28) "hit offsetget in MyContainer"
string(3) "baz"
