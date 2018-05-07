<?php
namespace TurboSlim\Tests\Helpers;

class MyRequestBody extends \TurboSlim\Http\RequestBody
{
    public function doAttach($stream)
    {
        parent::attach($stream);
    }
}
