--TEST--
Test for read_dim/write_dim handlers
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$p = new TurboSlim\Container();
$p[42] = 'foo';
$p['foo'] = 42;

echo $p[42], PHP_EOL;
echo $p['foo'], PHP_EOL;
try {
	var_dump($p['nonexistant']);
	echo "Exception expected", PHP_EOL;
}
catch (Psr\Container\NotFoundExceptionInterface $e) {
}

$p[54.2] = 'foo2';
echo $p[54], PHP_EOL;

$p[242.99] = 'foo99';
echo $p[242], PHP_EOL;

$p[5] = 'bar';
$p[5] = 'baz';
echo $p[5], PHP_EOL;

$p['str'] = 'str';
$p['str'] = 'strstr';
echo $p['str'], PHP_EOL;
?>
--EXPECT--
foo
42
foo2
foo99
baz
strstr
