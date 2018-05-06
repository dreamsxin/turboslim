--TEST--
Test service factory
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$p = new TurboSlim\Container();
$f = function() { var_dump('called-1'); return 'ret-1'; };

// This won't work: our factory accepts any callable (not only closures/invocable objects)
// Our factory() neither modifies the state of the container nor the callable itself
/*
$p->factory($f);
$p[] = $f;
*/

$p[] = $p->factory($f);

$p[] = function () { var_dump('called-2'); return 'ret-2'; };

var_dump($p[0]);
var_dump($p[0]);
var_dump($p[1]);
var_dump($p[1]);
?>
--EXPECT--
string(8) "called-1"
string(5) "ret-1"
string(8) "called-1"
string(5) "ret-1"
string(8) "called-2"
string(5) "ret-2"
string(5) "ret-2"
