--TEST--
Test simple class inheritance
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
class MyContainer extends TurboSlim\Container
{
	public $someAttr = 'fooAttr';

	public function& offsetGet($o)
	{
		var_dump("hit");
		return parent::offsetGet($o);
	}
}

$p = new MyContainer();
$p[42] = 'foo';
echo $p[42], PHP_EOL;
echo $p->someAttr, PHP_EOL;
?>
--EXPECT--
string(3) "hit"
foo
fooAttr
