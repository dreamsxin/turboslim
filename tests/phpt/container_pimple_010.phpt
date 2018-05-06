--TEST--
Test service is called as callback for every callback type
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
function callme()
{
	return 'called';
}

$a = function() { return 'called'; };

class Foo
{
	public static function bar()
	{
		return 'called';
	}
}
 
$p = new TurboSlim\Container();
$p['foo'] = 'callme';
echo $p['foo'], PHP_EOL;

$p['bar'] = $a;
echo $p['bar'], PHP_EOL;

$p['baz'] = "Foo::bar";
echo $p['baz'], PHP_EOL;

$p['foobar'] = ['Foo', 'bar'];
var_dump($p['foobar']);

?>
--EXPECT--
callme
called
Foo::bar
array(2) {
  [0]=>
  string(3) "Foo"
  [1]=>
  string(3) "bar"
}
