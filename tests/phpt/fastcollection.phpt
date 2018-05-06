--TEST--
FastCollection
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$c = new TurboSlim\FastCollection(['a' => 0, 'b' => 1]);
echo count($c), PHP_EOL;
var_dump($c->has('c'));
$c['c'] = 3;
echo count($c), PHP_EOL;
var_dump($c->has('c'));
foreach ($c as $k => $v) {
	echo $k, ' ', $v, PHP_EOL;
}
?>
--EXPECT--
2
bool(false)
3
bool(true)
a 0
b 1
c 3
