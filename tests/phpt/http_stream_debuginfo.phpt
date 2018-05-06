--TEST--
TurboSlim\Http\Stream and __debugInfo
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
class MyStream extends TurboSlim\Http\Stream
{
	public function __debugInfo()
	{
		return parent::__debugInfo();
	}
}

$f = fopen('php://memory', 'r');
$t = new MyStream($f);
var_dump($t);
?>
--EXPECTF--
object(MyStream)#%d (7) {
  ["stream:protected"]=>
  resource(%d) of type (stream)
  ["meta:protected"]=>
  NULL
  ["readable:protected"]=>
  bool(true)
  ["writable:protected"]=>
  bool(false)
  ["seekable:protected"]=>
  bool(true)
  ["size:protected"]=>
  int(0)
  ["isPipe:protected"]=>
  bool(false)
}
