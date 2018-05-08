--TEST--
Collection serialize/unserialize
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$c  = new \TurboSlim\Collection(['a' => 'b']);
$cc = clone($c);
var_dump($c == $cc);
$s = serialize($c);
// turboslim_collection_serialize() fools zend_std_get_properties(); make sure it restores everything the way it should be
var_dump($c == $cc);

$d = unserialize($s);
var_dump($c);
var_dump($d);
var_dump($c == $d);
var_dump($c !== $d);

// Trying to overwrite the internal data array
$s = 'C:20:"TurboSlim\\Collection":44:{a:1:{s:1:"a";s:1:"b";}a:1:{s:7:"' . "\0" . '*' . "\0" . 'data";N;}}';
$d = unserialize($s);
var_dump($d);

// Invoking unserialize() with crafted data
$c = new \TurboSlim\Collection();
$s = 'a:1:{s:1:"a";s:1:"b";}a:1:{s:7:"' . "\0" . '*' . "\0" . 'data";i:1;}';
try {
	$c->unserialize($s);
	var_dump($c);
}
catch (\UnexpectedValueException $e) {
	echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
bool(true)
bool(true)
object(TurboSlim\Collection)#%d (1) {
  ["data":protected]=>
  array(1) {
    ["a"]=>
    string(1) "b"
  }
}
object(TurboSlim\Collection)#%d (1) {
  ["data":protected]=>
  array(1) {
    ["a"]=>
    string(1) "b"
  }
}
bool(true)
bool(true)

Notice: unserialize(): Error at offset %d of %d bytes in %s on line %d
bool(false)
Error at offset %d of %d bytes
