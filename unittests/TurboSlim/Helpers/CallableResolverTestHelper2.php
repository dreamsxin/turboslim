<?php

namespace TurboSlim\Tests\Helpers;

class CallableResolverTestHelper2
{
    public function __invoke()
    {
        return 36;
    }
}
