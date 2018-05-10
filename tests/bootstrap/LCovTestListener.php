<?php

class LCovTestListener implements PHPUnit\Framework\TestListener
{
    use PHPUnit\Framework\TestListenerDefaultImplementation;

    private $enabled;
    private $onecoverage;

    private $base;
    private $dir;
    private $tracedir;

    private $tests = [];

    private static function delTree(string $dir)
    {
        $files = \array_diff(\scandir($dir), ['.', '..']);
        foreach ($files as $file) {
            $item = $dir . '/' . $file;
            \is_dir($item) ? self::delTree($item) : \unlink($item);
        }

        return \rmdir($dir);
    }

    public function __construct()
    {
        $this->enabled = \function_exists('TurboSlim\\flush_coverage');
        if ($this->enabled && \function_exists('TurboSlim\\under_valgrind') && TurboSlim\under_valgrind()) {
            $this->enabled = false;
        }

        $this->onecoverage = getenv("ONECOVERAGE");

        if ($this->enabled) {
            $this->dir       = dirname(dirname(__DIR__));
            $this->base      = dirname(dirname(__DIR__));
            $this->tracedir  = $this->base . '/coverage/';

            if (\is_dir($this->base . '/coverage')) {
                self::delTree($this->base . '/coverage');
            }

            \mkdir($this->base . '/coverage');
            \passthru(\sprintf('lcov --zerocounters --directory %s --quiet', \escapeshellarg($this->dir)));

            // Do not run `lcov --initial` as we use different test names
            // lcov --no-external --capture --directory {$this->dir} --base {$this->base} --initial --quiet --output-file %s
        }
    }

    public function __destruct()
    {
        if ($this->enabled) {
            if ($this->onecoverage) {
                TurboSlim\flush_coverage();

                $command = \sprintf(
                    'lcov --no-external --capture --directory %s --base %s --quiet --output-file %s',
                    \escapeshellarg($this->dir),
                    \escapeshellarg($this->base),
                    \escapeshellarg($this->tracedir . 'COMBINED.info')
                );

                \passthru($command);
            }
            else {
                $args = [];
                foreach ($this->tests as $test) {
                    $args[] = '-a ' . \escapeshellarg($this->tracedir . $test . '.info');
                }

                $args[]  = '-o ' . \escapeshellarg($this->tracedir . 'COMBINED.info');
                $command = 'lcov -q ' . join(' ', $args);
                \passthru($command);
            }

            $command = \sprintf('genhtml -q -s -o %s %s', \escapeshellarg($this->tracedir . 'OUTPUT'), \escapeshellarg($this->tracedir . 'COMBINED.info'));
            \passthru($command);
        }
    }

    public function endTest(PHPUnit\Framework\Test $test, $time)
    {
        if ($this->enabled && !$this->onecoverage) {
            TurboSlim\flush_coverage();

            $name  = $test->getName(false);
            $class = \get_class($test);

            if (\substr($name, -5) === '.phpt') {
                $name = 'PHPT_' . \basename($name, '.phpt');
            }
            else {
                $name = $class . '_' . $name;
            }

            $name = \str_replace(['\\', '-'], '_', $name);

            $this->tests[] = $name;
            $command = \sprintf(
                'lcov --no-external --capture --directory %s --base %s --test-name %s --quiet --output-file %s',
                \escapeshellarg($this->dir),
                \escapeshellarg($this->base),
                \escapeshellarg($name),
                \escapeshellarg($this->tracedir . $name . '.info')
            );

            \passthru($command);
            // Tests cover different file, make sure to zero all counters in order not to affect subsequent tests
            \passthru(\sprintf('lcov --zerocounters --directory %s --quiet', \escapeshellarg($this->dir)));
        }
    }
}
