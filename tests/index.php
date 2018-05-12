<?php
require __DIR__ . '/../vendor/autoload.php';

header('Content-Type: text/plain; charset=utf-8');
ini_set('implicit_flush', true);
ini_set('output_buffering', false);

while (ob_get_level()) {
    ob_end_flush();
}

$argv = ['index.php', '--configuration', '../phpunit.xml', '--colors=always'];

$_SERVER['argc'] = count($argv);
$_SERVER['argv'] = $argv;

PHPUnit\TextUI\Command::main(false);
