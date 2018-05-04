<?php

class FactoryWrapper
{
	private $callable;

	public function __construct(callable $c)
	{
		$this->callable = $c;
	}

	public function invoke(...$args)
	{
		$this->callable(...$args);
	}
}


class ProtectWrapper
{
	private $callable;

	public function __construct(callable $c)
	{
		$this->callable = $c;
	}

	public function invoke(...$args)
	{
		$this->callable(...$args);
	}

	public function getCallable() : callable
	{
		return $this->callable;
	}
}


class Container implements \ArrayAccess, \Psr\Container\ContainerInterface
{
	private $values     = [];
	private $singletons = [];

	public function __construct(array $v = null)
	{
		if ($v) {
			$this->values = $v;
		}
	}

	public function offsetSet($key, $value)
	{
		if (isset($this->values[$key])) {
			throw new Exception("Frozen, {$key}");
		}

		$this->values[$key] = $value;
	}

	public function offsetGet($key)
	{
		if (!isset($this->values[$key])) {
			throw new Exception("Not found, {$key}");
		}

		$val = $this->values[$key];
		if (
		       isset($this->singletons[$key]) /* Already instantiated */
		    || !is_object($val)               /* Parameter */
		    || $val instanceof ProtectWrapper
		) {
			return $val instanceof ProtectWrapper ? $val->getCallable() : $val;
		}

		if ($val instanceof FactoryWrapper) {
			return $val($this);
		}

		$this->values[$key]     = $val($this);
		$this->singletons[$key] = true;
	}
}
