--TEST--
Collection protects its $data property
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
class MyCollection extends TurboSlim\Collection
{
	public function setData($val)
	{
		$this->data = $val;
	}

	public function unsetData()
	{
		unset($this->data);
	}
}

$c = new MyCollection();
$c->setData(123);
var_dump($c->all());
$c->unsetData();
var_dump($c->all());
$c->setData(null); // Should give empty array, just like (array)null
var_dump($c->all());
$c->setData([0, 1, 2]);
var_dump($c->all());
?>
--EXPECT--
array(1) {
  [0]=>
  int(123)
}
array(0) {
}
array(0) {
}
array(3) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
}
