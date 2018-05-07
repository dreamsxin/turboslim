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

/**
 * @warning turboslim_http_stream_get_gc() depends on the layout of the structure
 */
typedef struct turboslim_http_stream {
    zval res;
    zval meta;
    php_stream* stream;
    zend_bool readable;
    zend_bool writable;
    zend_bool seekable;
    signed char is_pipe;
    zend_bool turboslim_class;
    zend_bool fast_tostring;
    zend_bool fast_debug;
    zend_object std;
} turboslim_http_stream_t;


extern zend_class_entry* ce_TurboSlim_Http_Stream;
TURBOSLIM_VISIBILITY_HIDDEN extern zend_object_handlers turboslim_http_stream_handlers;
TURBOSLIM_VISIBILITY_HIDDEN extern const zend_function_entry fe_TurboSlim_Http_Stream[];

TURBOSLIM_VISIBILITY_HIDDEN zend_object* turboslim_http_stream_create_object(zend_class_entry* ce);
TURBOSLIM_VISIBILITY_HIDDEN void turboslim_http_stream_free_obj(zend_object* obj);
TURBOSLIM_VISIBILITY_HIDDEN zend_object* turboslim_http_stream_clone_obj(zval* obj);
TURBOSLIM_VISIBILITY_HIDDEN zval* turboslim_http_stream_read_property(zval* object, zval* member, int type, void** cache_slot, zval* rv);
TURBOSLIM_VISIBILITY_HIDDEN int turboslim_http_stream_has_property(zval* object, zval* member, int has_set_exists, void** cache_slot);
TURBOSLIM_VISIBILITY_HIDDEN HashTable* turboslim_http_stream_get_properties(zval* object);
TURBOSLIM_VISIBILITY_HIDDEN HashTable* turboslim_http_stream_get_debug_info(zval* object, int* is_temp);
TURBOSLIM_VISIBILITY_HIDDEN HashTable* turboslim_http_stream_get_gc(zval* object, zval** table, int* n);
TURBOSLIM_VISIBILITY_HIDDEN int turboslim_http_stream_compare_objects(zval* z1, zval* z2);
TURBOSLIM_VISIBILITY_HIDDEN int turboslim_http_stream_cast_object(zval* readobj, zval* writeobj, int type);

#endif /* TURBOSLIM_HTTP_STREAM_H */
