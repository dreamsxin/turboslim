<?php
/** @var \Composer\Autoload\ClassLoader $autoloader */
$autoloader = require __DIR__ . '/../vendor/autoload.php';

$autoloader->addPsr4('Slim\\Tests\\', __DIR__ . '/Slim');
$autoloader->addPsr4('SlimBugs\\Tests\\', __DIR__ . '/SlimBugs');
$autoloader->addPsr4('TurboSlim\\Tests\\', __DIR__ . '/TurboSlim');

TurboSlim\mock_slim_interfaces();
