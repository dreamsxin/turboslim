--TEST--
Collection serialize/unserialize (inheritance)
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
class MyCollection extends TurboSlim\Collection
{
	public function serialize()
	{
		return parent::serialize();
	}

	public function unserialize($s)
	{
		return parent::unserialize($s);
	}
}

$c = new MyCollection(['a' => 'b']);
$s = serialize($c);
echo $s, PHP_EOL;
$d = unserialize($s);
var_dump($c);
var_dump($d);
var_dump($c == $d);
var_dump($c !== $d);

try {
	$s = 'C:12:"MyCollection":28:{a:a:{s:1:"a";s:1:"b";}a:0:{}}';
	unserialize($s);
}
catch (UnexpectedValueException $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	$s = 'C:12:"MyCollection":28:{a:1:{s:1:"a";s:1:"b";}a:a:{}}';
	unserialize($s);
}
catch (UnexpectedValueException $e) {
	echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
C:12:"MyCollection":28:{a:1:{s:1:"a";s:1:"b";}a:0:{}}
object(MyCollection)#%d (1) {
  ["data"]=>
  array(1) {
    ["a"]=>
    string(1) "b"
  }
}
object(MyCollection)#%d (1) {
  ["data"]=>
  array(1) {
    ["a"]=>
    string(1) "b"
  }
}
bool(true)
bool(true)
Error at offset %d of %d bytes
Error at offset %d of %d bytes
