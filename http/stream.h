#ifndef TURBOSLIM_HTTP_STREAM_H
#define TURBOSLIM_HTTP_STREAM_H

/**
 * TurboSlim\Http\Stream differs is implemented differently than Slim\Http\Stream
 *
 * Below is the quick summary of differences:
 *   * `tell()` works for pipes;
 *   * `getSize()` return 0 for pipes (just like PHP's `fstat()`);
 *   * stream properties (readable/writable/seekable) are determined when the stream is attached
 *     (in the C land, this is a cheap operation), and therefore:
 *     * `isReadable()`, `isWritable()`, and `isSeekable()` are getters for the respective cached stream properties;
 *       their return value can be cached until `detach()` is called;
 *     * `seek()`, `tell()`, `rewind()` do not call `isSeekable()`;
 *     * `read()`, `getContents()` do not call `isReadable()`;
 *     * `write()` does not call `isWritable()`;
 *     * none of the above methods calls `isPipe()`.
 *   * getSize() does not cache its result: the same stream can be shared, and therefore the size of the stream can change without prior notice;
 *   * attach() validates that the resource passed to it is actually a stream
 *     (see `SlimBugs\Tests\StreamTest::testSlimWrongResource()`);
 *   * all protected properties are read-only;
 *   * all protected properties are emulated (they are not real), therefore the following will not work:
 *     ```
 *      class MyStream extends \TurboSlim\Http\Stream
 *      {
 *          public $size;
 *      }
 *
 *      $x = new MyStream(fopen('php://memory', 'rw'));
 *      $x->size = 10;
 *      var_dump($x->size); // Fatal error: Uncaught Error: Cannot access protected property TurboSlim\Http\Stream::$size
 *     ```
 */

#include "php_turboslim.h"

extern zend_class_entry* ce_TurboSlim_Http_Stream;

TURBOSLIM_VISIBILITY_HIDDEN int init_http_stream();

#endif /* TURBOSLIM_HTTP_STREAM_H */
