--TEST--
Make sure ReflectionProperty cannot damage CallableResolver
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$container = new class implements \Psr\Container\ContainerInterface {
	public function has($id)
	{
		return $id === 'something';
	}

	public function get($id)
	{
		if ($id === 'something') {
			return new class {
				public function method()
				{
					return 123;
				}
			};
		}

		return null;
	}
};

$cr = new \TurboSlim\CallableResolver($container);
$rp = new ReflectionProperty($cr, 'container');
$rp->setAccessible(true);
var_dump($rp->getValue($cr));
$rp->setValue($cr, null);
var_dump($rp->getValue($cr));
var_dump($cr);
echo $cr->resolve('something:method')(), PHP_EOL;
gc_collect_cycles();
?>
--EXPECTF--
object(class@anonymous)#%d (0) {
}
NULL
object(TurboSlim\CallableResolver)#%d (1) {
  ["container":"TurboSlim\CallableResolver":private]=>
  NULL
}

Fatal error: TurboSlim\CallableResolver::$container is no longer an object in %s on line %d
