--TEST--
TurboSlim\Http\Stream and get_object_vars()
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
class MyStream extends TurboSlim\Http\Stream
{
	public function getVars()
	{
		return get_object_vars($this);
	}
}

$f = fopen('php://memory', 'r');
$s = new TurboSlim\Http\Stream($f);
$t = new MyStream($f);
var_dump(get_object_vars($s));
var_dump($t->getVars());
?>
--EXPECTF--
array(0) {
}
array(7) {
  ["stream"]=>
  resource(%d) of type (stream)
  ["meta"]=>
  NULL
  ["readable"]=>
  bool(true)
  ["writable"]=>
  bool(false)
  ["seekable"]=>
  bool(true)
  ["size"]=>
  int(0)
  ["isPipe"]=>
  bool(false)
}
