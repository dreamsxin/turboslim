--TEST--
Collection
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$c = new TurboSlim\Collection(['a' => 0, 'b' => 1]);
echo count($c), PHP_EOL;
var_dump($c->has('c'));
$c['c'] = 3;
echo count($c), PHP_EOL;
var_dump($c->has('c'));
?>
--EXPECT--
2
bool(false)
3
bool(true)
