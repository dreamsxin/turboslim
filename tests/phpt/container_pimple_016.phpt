--TEST--
Test extend()
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
/*
    $extended = function ($c) use ($callable, $factory) {
        return $callable($factory($c), $c);
    };
*/

$p          = new TurboSlim\Container();
$p['param'] = 'param';
$p[12]      = function ($p) { var_dump($p['param']); return 'foo'; }; /* $factory in code above */
$c          = $p->extend(12, function ($w) { var_dump($w); return 'bar'; }); /* $callable in code above */

var_dump($p[12]);
?>
--EXPECT--
string(5) "param"
string(3) "foo"
string(3) "bar"
