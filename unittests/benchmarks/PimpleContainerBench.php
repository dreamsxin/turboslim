<?php

/**
 * @Revs(1000000)
 * @Iterations(5)
 * @OutputMode("throughput")
 * @OutputTimeUnit("seconds", precision=3)
 * @Groups({"Container"})
 */
class PimpleContainerBench
{
    private $x;

    public function __construct()
    {
        $this->x = new Pimple\Container();
        $this->x['factory'] = $this->x->factory(function() {
            return 1;
        });

        $this->x['shared'] = function() {
            return 2;
        };

        $x = $this->x['shared']; // Resolve
    }

    /**
     * @Subject
     */
    public function arrayGetShared()
    {
        $this->x['shared'];
    }

    /**
     * @Subject
     */
    public function arrayGetFactory()
    {
        $this->x['factory'];
    }

    /**
     * @Subject
     */
    public function offsetGetShared()
    {
        $this->x->offsetGet('shared');
    }

    /**
     * @Subject
     */
    public function offsetGetFactory()
    {
        $this->x->offsetGet('factory');
    }

    /**
     * @Subject
     */
    public function arrayExists()
    {
        isset($this->x['factory']);
    }

    /**
     * @Subject
     */
    public function offsetExists()
    {
        $this->x->offsetExists('factory');
    }

    /**
     * @Subject
     */
    public function arraySet()
    {
        $this->x['x'] = function() {};
    }

    /**
     * @Subject
     */
    public function offsetSet()
    {
        $this->x->offsetSet('x', function() {});
    }
}
