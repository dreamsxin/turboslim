--TEST--
Collection serialize/unserialize
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$c = new \TurboSlim\Collection(['a' => 'b']);
$s = serialize($c);
echo $s, PHP_EOL;
$d = unserialize($s);
var_dump($c);
var_dump($d);
var_dump($c == $d);
var_dump($c !== $d);

$s = 'C:20:"TurboSlim\Collection":28:{a:a:{s:1:"a";s:1:"b";}a:0:{}}';
unserialize($s);

$s = 'C:20:"TurboSlim\Collection":28:{a:1:{s:1:"a";s:1:"b";}a:a:{}}';
unserialize($s);
?>
--EXPECTF--
C:20:"TurboSlim\Collection":28:{a:1:{s:1:"a";s:1:"b";}a:0:{}}
object(TurboSlim\Collection)#%d (1) {
  ["data"]=>
  array(1) {
    ["a"]=>
    string(1) "b"
  }
}
object(TurboSlim\Collection)#%d (1) {
  ["data"]=>
  array(1) {
    ["a"]=>
    string(1) "b"
  }
}
bool(true)
bool(true)

Notice: unserialize(): Error at offset %d of %d bytes in %s on line %d

Notice: unserialize(): Error at offset %d of %d bytes in %s on line %d
