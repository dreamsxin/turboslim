#include "turboslim/http/stream.h"

#include <Zend/zend_exceptions.h>
#include <Zend/zend_interfaces.h>
#include <ext/spl/spl_exceptions.h>
#include <ext/standard/file.h>
#include <main/php_streams.h>
#include "turboslim/psr7.h"
#include "utils.h"

zend_class_entry* ce_TurboSlim_Http_Stream = NULL;
zend_object_handlers turboslim_http_stream_handlers;

typedef turboslim_http_stream_t stream_t;

static inline stream_t* stream_from_zobj(const zend_object* obj)
{
    return (stream_t*)((char*)(obj) - XtOffsetOf(stream_t, std));
}

static inline zend_bool is_attached(stream_t* x)
{
    return x->stream != NULL;
}

static zend_bool is_pipe(stream_t* x)
{
    if (x->is_pipe == -1 && is_attached(x)) {
        php_stream_statbuf stat_ssb;

        if (!php_stream_stat(x->stream, &stat_ssb)) {
            return S_ISFIFO(stat_ssb.sb.st_mode);
        }
    }

    return 0;
}

static zend_long get_size(stream_t* x)
{
    if (is_attached(x)) {
        php_stream_statbuf stat_ssb;

        if (!php_stream_stat(x->stream, &stat_ssb)) {
            return stat_ssb.sb.st_size;
        }
    }

    return -1;
}

static void handle_inheritance(zend_class_entry* ce, stream_t* v)
{
    v->turboslim_class = (ce->type == ZEND_INTERNAL_CLASS && ce->info.internal.module->handle == turboslim_module_entry.handle);
    if (v->turboslim_class) {
        v->fast_tostring = 1;
        v->fast_debug    = 1;
    }
    else {
        zend_function* f1;
        zend_function* f2;
        zend_function* f3;

        f1 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("rewind"));
        f2 = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("getcontents"));
        f3 = ce->__tostring;
        if (is_turboslim_func(f1) && is_turboslim_func(f2) && is_turboslim_func(f3)) {
            v->fast_tostring = 1;
        }
        else {
            v->fast_tostring = 0;
        }

        if (is_turboslim_func(ce->__debugInfo)) {
            v->fast_debug = 1;
        }
        else {
            v->fast_debug = 0;
        }
    }
}

zend_object* turboslim_http_stream_create_object(zend_class_entry* ce)
{
    stream_t* v = ecalloc(1, sizeof(stream_t) + zend_object_properties_size(ce));

    zend_object_std_init(&v->std, ce);
    object_properties_init(&v->std, ce);
    v->std.handlers = &turboslim_http_stream_handlers;

    ZVAL_NULL(&v->res);
    ZVAL_NULL(&v->meta);
    v->is_pipe = -1;

    handle_inheritance(ce, v);
    return &v->std;
}

void turboslim_http_stream_free_obj(zend_object* obj)
{
    stream_t* v = stream_from_zobj(obj);

    zend_object_std_dtor(&v->std);
    zval_ptr_dtor(&v->res);
    zval_ptr_dtor(&v->meta);
}

zend_object* turboslim_http_stream_clone_obj(zval* obj)
{
    zend_object* old_object = Z_OBJ_P(obj);
    zend_object* new_object = turboslim_http_stream_create_object(old_object->ce);

    stream_t* mine   = stream_from_zobj(old_object);
    stream_t* theirs = stream_from_zobj(new_object);

    ZVAL_COPY(&theirs->res,  &mine->res);
    ZVAL_COPY(&theirs->meta, &mine->meta);
    theirs->stream   = mine->stream;
    theirs->readable = mine->readable;
    theirs->writable = mine->writable;
    theirs->seekable = mine->seekable;
    theirs->is_pipe  = mine->is_pipe;

    zend_objects_clone_members(new_object, old_object);
    return new_object;
}

static int is_my_property(zend_ulong hash, const zend_string* m, int type)
{
    if (
           (hash == zend_inline_hash_func(ZEND_STRL("stream"))   && zend_string_equals_literal(m, "stream"))
        || (hash == zend_inline_hash_func(ZEND_STRL("meta"))     && zend_string_equals_literal(m, "meta"))
        || (hash == zend_inline_hash_func(ZEND_STRL("readable")) && zend_string_equals_literal(m, "readable"))
        || (hash == zend_inline_hash_func(ZEND_STRL("writable")) && zend_string_equals_literal(m, "writable"))
        || (hash == zend_inline_hash_func(ZEND_STRL("seekable")) && zend_string_equals_literal(m, "seekable"))
        || (hash == zend_inline_hash_func(ZEND_STRL("size"))     && zend_string_equals_literal(m, "size"))
        || (hash == zend_inline_hash_func(ZEND_STRL("isPipe"))   && zend_string_equals_literal(m, "isPipe"))
    ) {
        if (type != BP_VAR_IS) {
            zend_throw_error(NULL, "Cannot access protected property %s::$%s", ZSTR_VAL(ce_TurboSlim_Http_Stream->name), ZSTR_VAL(m));
        }

        return 1;
    }

    return 0;
}

zval* turboslim_http_stream_read_property(zval* object, zval* member, int type, void** cache_slot, zval* rv)
{
    if (Z_TYPE_P(member) == IS_STRING) {
        zend_object* zobj = Z_OBJ_P(object);
        zend_string* m    = Z_STR_P(member);
        zend_ulong hash   = ZSTR_HASH(m);

        if (zend_check_protected(ce_TurboSlim_Http_Stream, zend_get_executed_scope())) {
            stream_t* x = stream_from_zobj(zobj);

            if (hash == zend_inline_hash_func(ZEND_STRL("stream")) && zend_string_equals_literal(m, "stream")) {
                return &x->res;
            }

            if (hash == zend_inline_hash_func(ZEND_STRL("meta")) && zend_string_equals_literal(m, "meta")) {
                return &x->meta;
            }

            if (hash == zend_inline_hash_func(ZEND_STRL("readable")) && zend_string_equals_literal(m, "readable")) {
                ZVAL_BOOL(rv, x->readable);
                return rv;
            }

            if (hash == zend_inline_hash_func(ZEND_STRL("writable")) && zend_string_equals_literal(m, "writable")) {
                ZVAL_BOOL(rv, x->writable);
                return rv;
            }

            if (hash == zend_inline_hash_func(ZEND_STRL("seekable")) && zend_string_equals_literal(m, "seekable")) {
                ZVAL_BOOL(rv, x->seekable);
                return rv;
            }

            if (hash == zend_inline_hash_func(ZEND_STRL("size")) && zend_string_equals_literal(m, "size")) {
                zend_long size = get_size(x);
                if (size >= 0) {
                    ZVAL_LONG(rv, size);
                }
                else {
                    ZVAL_NULL(rv);
                }

                return rv;
            }

            if (hash == zend_inline_hash_func(ZEND_STRL("isPipe")) && zend_string_equals_literal(m, "isPipe")) {
                ZVAL_BOOL(rv, is_pipe(x));
                return rv;
            }
        }
        else if (is_my_property(hash, m, type)) {
            return &EG(uninitialized_zval);
        }
    }

    return zend_get_std_object_handlers()->read_property(object, member, type, cache_slot, rv);
}

int turboslim_http_stream_has_property(zval* object, zval* member, int has_set_exists, void** cache_slot)
{
    if (Z_TYPE_P(member) == IS_STRING) {
        zend_object* zobj = Z_OBJ_P(object);
        zend_string* m    = Z_STR_P(member);
        zend_ulong hash   = ZSTR_HASH(m);
        zval* rv          = NULL;
        zval z;

        if (zend_check_protected(ce_TurboSlim_Http_Stream, zend_get_executed_scope())) {
            stream_t* x = stream_from_zobj(zobj);

            if (hash == zend_inline_hash_func(ZEND_STRL("stream")) && zend_string_equals_literal(m, "stream")) {
                rv = &x->res;
            }
            else if (hash == zend_inline_hash_func(ZEND_STRL("meta")) && zend_string_equals_literal(m, "meta")) {
                rv = &x->meta;
            }
            else if (hash == zend_inline_hash_func(ZEND_STRL("readable")) && zend_string_equals_literal(m, "readable")) {
                ZVAL_BOOL(&z, x->readable);
                rv = &z;
            }
            else if (hash == zend_inline_hash_func(ZEND_STRL("writable")) && zend_string_equals_literal(m, "writable")) {
                ZVAL_BOOL(&z, x->writable);
                rv = &z;
            }
            else if (hash == zend_inline_hash_func(ZEND_STRL("seekable")) && zend_string_equals_literal(m, "seekable")) {
                ZVAL_BOOL(&z, x->seekable);
                rv = &z;
            }
            else if (hash == zend_inline_hash_func(ZEND_STRL("size")) && zend_string_equals_literal(m, "size")) {
                zend_long size = get_size(x);
                if (size >= 0) {
                    ZVAL_LONG(&z, size);
                }
                else {
                    ZVAL_NULL(&z);
                }

                rv = &z;
            }
            else if (hash == zend_inline_hash_func(ZEND_STRL("isPipe")) && zend_string_equals_literal(m, "isPipe")) {
                ZVAL_BOOL(&z, is_pipe(x));
                rv = &z;
            }

            if (rv) {
                switch (has_set_exists) {
                    /* HAS: whether property exists and is not NULL */
                    case 0: return Z_TYPE_P(rv) != IS_NULL;
                    /* SET: whether property exists and is true */
                    default:
                    case 1:
                        return zend_is_true(rv);
                    /* EXISTS: whether property exists */
                    case 2: return 1;
                }
            }
        }
        else if (is_my_property(hash, m, BP_VAR_IS)) {
            return 0;
        }
    }

    return zend_get_std_object_handlers()->has_property(object, member, has_set_exists, cache_slot);
}

HashTable* turboslim_http_stream_get_properties(zval* object)
{
    zend_object* zobj = Z_OBJ_P(object);
    stream_t* x       = stream_from_zobj(zobj);
    HashTable* ret    = zend_std_get_properties(object);
    zval* z;
    zval t;

    if (zend_check_protected(ce_TurboSlim_Http_Stream, zend_get_executed_scope())) {
        if ((z = _zend_hash_str_add(ret, ZEND_STRL("stream"), &x->res ZEND_FILE_LINE_CC))) {
            Z_TRY_ADDREF_P(z);
        }

        if ((z = _zend_hash_str_add(ret, ZEND_STRL("meta"), &x->meta ZEND_FILE_LINE_CC))) {
            Z_TRY_ADDREF_P(z);
        }

        ZVAL_BOOL(&t, x->readable);
        _zend_hash_str_add(ret, ZEND_STRL("readable"), &t ZEND_FILE_LINE_CC);

        ZVAL_BOOL(&t, x->writable);
        _zend_hash_str_add(ret, ZEND_STRL("writable"), &t ZEND_FILE_LINE_CC);

        ZVAL_BOOL(&t, x->seekable);
        _zend_hash_str_add(ret, ZEND_STRL("seekable"), &t ZEND_FILE_LINE_CC);

        zend_long size = get_size(x);
        if (size >= 0) {
            ZVAL_LONG(&t, size);
        }
        else {
            ZVAL_NULL(&t);
        }

        _zend_hash_str_add(ret, ZEND_STRL("size"), &t ZEND_FILE_LINE_CC);

        ZVAL_BOOL(&t, is_pipe(x));
        _zend_hash_str_add(ret, ZEND_STRL("isPipe"), &t ZEND_FILE_LINE_CC);
    }

    return ret;
}

static HashTable* get_debug_info_fast(stream_t* x, zval* object)
{
    HashTable* ret;
    zval* z;
    zval t;

    ALLOC_HASHTABLE(ret);
    zend_hash_init(ret, 8, NULL, ZVAL_PTR_DTOR, 0);

    if ((z = _zend_hash_str_add(ret, ZEND_STRL("stream:protected"), &x->res ZEND_FILE_LINE_CC))) {
        Z_TRY_ADDREF_P(z);
    }

    if ((z = _zend_hash_str_add(ret, ZEND_STRL("meta:protected"), &x->meta ZEND_FILE_LINE_CC))) {
        Z_TRY_ADDREF_P(z);
    }

    ZVAL_BOOL(&t, x->readable);
    _zend_hash_str_add(ret, ZEND_STRL("readable:protected"), &t ZEND_FILE_LINE_CC);

    ZVAL_BOOL(&t, x->writable);
    _zend_hash_str_add(ret, ZEND_STRL("writable:protected"), &t ZEND_FILE_LINE_CC);

    ZVAL_BOOL(&t, x->seekable);
    _zend_hash_str_add(ret, ZEND_STRL("seekable:protected"), &t ZEND_FILE_LINE_CC);

    ZVAL_LONG(&t, get_size(x));
    _zend_hash_str_add(ret, ZEND_STRL("size:protected"), &t ZEND_FILE_LINE_CC);

    ZVAL_BOOL(&t, is_pipe(x));
    _zend_hash_str_add(ret, ZEND_STRL("isPipe:protected"), &t ZEND_FILE_LINE_CC);

    HashTable* std = zend_std_get_properties(object);
    zend_hash_copy(ret, std, zval_add_ref);

    return ret;
}

HashTable* turboslim_http_stream_get_debug_info(zval* object, int* is_temp)
{
    stream_t* v = stream_from_zobj(Z_OBJ_P(object));
    if (v->fast_debug) {
        *is_temp = 1;
        return get_debug_info_fast(v, object);
    }

    return zend_std_get_debug_info(object, is_temp);
}

HashTable* turboslim_http_stream_get_gc(zval* object, zval** table, int* n)
{
    stream_t* v = stream_from_zobj(Z_OBJ_P(object));

    *table = &v->res;
    *n     = 2;
    return zend_std_get_properties(object);
}

int turboslim_http_stream_compare_objects(zval* z1, zval* z2)
{
    zval r;
    zend_object* zobj1 = Z_OBJ_P(z1);
    zend_object* zobj2 = Z_OBJ_P(z2);

    if (zobj1->ce != zobj2->ce) {  /* LCOV_EXCL_BR_LINE */
        return 1;                  /* LCOV_EXCL_LINE - this cannot be tested without dirty hacks */
    }

    stream_t* v = stream_from_zobj(zobj1);
    stream_t* w = stream_from_zobj(zobj2);

    /* It should be enough to compare streams, as the rest of the properties are derived */
    if (UNEXPECTED(FAILURE == compare_function(&r, &v->res, &w->res)) || Z_LVAL(r) != 0) {
        return 1;
    }

    return zend_get_std_object_handlers()->compare_objects(z1, z2);
}

int turboslim_http_stream_cast_object(zval* readobj, zval* writeobj, int type)
{
    stream_t* x = stream_from_zobj(Z_OBJ_P(readobj));

    if (x->fast_tostring) {
        zval retval;
        if (type == IS_STRING) {
            if (!is_attached(x) || !x->seekable || !x->readable || -1 == php_stream_rewind(x->stream)) {
                ZVAL_EMPTY_STRING(&retval);
            }
            else {
                zend_string* contents = php_stream_copy_to_mem(x->stream, PHP_STREAM_COPY_ALL, 0);
                if (contents) {
                    ZVAL_STR(&retval, contents);
                }
                else {
                    ZVAL_EMPTY_STRING(&retval);
                }
            }

            if (readobj == writeobj) {
                zval_ptr_dtor(readobj);
            }

            ZVAL_COPY_VALUE(writeobj, &retval);
            return SUCCESS;
        }
    }

    return zend_std_cast_object_tostring(readobj, writeobj, type);
}

static void detach(stream_t* x)
{
    zval_ptr_dtor(&x->res);
    ZVAL_NULL(&x->res);
    x->stream = NULL;

    zval_ptr_dtor(&x->meta);
    ZVAL_NULL(&x->meta);

    x->readable = 0;
    x->writable = 0;
    x->seekable = 0;
    x->is_pipe  = -1;
}

static PHP_METHOD(TurboSlim_Http_Stream, attach)
{
    zval* res;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(res)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    if (UNEXPECTED(Z_TYPE_P(res) != IS_RESOURCE)) {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "%s argument must be a valid PHP resource", "TurboSlim\\Http\\Stream::attach");
        return;
    }

    zval* this_ptr = get_this(execute_data);
    stream_t* x    = stream_from_zobj(Z_OBJ_P(this_ptr));

    php_stream* stream;
    php_stream_from_zval_no_verify(stream, res);
    if (!stream) {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "%s argument is not a stream", "TurboSlim\\Http\\Stream::attach");
        return;
    }

    if (is_attached(x)) {
        detach(x);
    }

    ZVAL_COPY(&x->res, res);

    x->stream   = stream;
    x->readable = (strpbrk(stream->mode, "r+") != NULL);
    x->writable = (strpbrk(stream->mode, "waxc+") != NULL);
    x->seekable = (stream->ops->seek != NULL) && ((stream->flags & PHP_STREAM_FLAG_NO_SEEK) == 0);
}

static PHP_METHOD(TurboSlim_Http_Stream, __construct)
{
    ZEND_MN(TurboSlim_Http_Stream_attach)(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

static PHP_METHOD(TurboSlim_Http_Stream, getMetadata)
{
    zval* key = NULL;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL_EX(key, 1, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    stream_t* x    = stream_from_zobj(Z_OBJ_P(this_ptr));

    if (!is_attached(x)) {
        RETURN_NULL();
    }

    /* $this->meta = stream_get_meta_data($this->stream); */
    zval_ptr_dtor(&x->meta);

    static zend_function* stream_get_meta_data = NULL;
    zend_call_method(NULL, NULL, &stream_get_meta_data, ZEND_STRL("stream_get_meta_data"), &x->meta, 1, &x->res, NULL);
    if (UNEXPECTED(EG(exception))) {
        return;
    }

    /*
     *  if (is_null($key) === true) {
     *      return $this->meta;
     *  }
     */
    if (!key || Z_TYPE_P(key) == IS_NULL) {
        RETURN_ZVAL(&x->meta, 1, 0);
    }

    /* return isset($this->meta[$key]) ? $this->meta[$key] : null; */
    if (EXPECTED(Z_TYPE(x->meta) == IS_ARRAY)) {
        zval* value = array_zval_offset_get(Z_ARRVAL(x->meta), key);
        if (value) {
            RETURN_ZVAL(value, 1, 0);
        }
    }

    RETURN_NULL();
}

static PHP_METHOD(TurboSlim_Http_Stream, isAttached)
{
    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    stream_t* x    = stream_from_zobj(Z_OBJ_P(this_ptr));

    RETURN_BOOL(is_attached(x));
}

static PHP_METHOD(TurboSlim_Http_Stream, detach)
{
    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    stream_t* x    = stream_from_zobj(Z_OBJ_P(this_ptr));

    RETVAL_ZVAL(&x->res, 1, 0);
    detach(x);
}

static PHP_METHOD(TurboSlim_Http_Stream, __toString)
{
    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    stream_t* x    = stream_from_zobj(Z_OBJ_P(this_ptr));

    if (!is_attached(x)) {
        RETURN_EMPTY_STRING();
    }

    if (x->fast_tostring) {
        if (!x->seekable || !x->readable || -1 == php_stream_rewind(x->stream)) {
            RETURN_EMPTY_STRING();
        }

        zend_string* contents = php_stream_copy_to_mem(x->stream, PHP_STREAM_COPY_ALL, 0);
        if (contents) {
            RETURN_STR(contents);
        }

        RETURN_EMPTY_STRING();
    }

    zend_call_method(this_ptr, Z_OBJCE_P(this_ptr), NULL, ZEND_STRL("rewind"), NULL, 0, NULL, NULL);
    if (UNEXPECTED(EG(exception))) {
        zend_clear_exception();
        RETURN_EMPTY_STRING();
    }

    zend_call_method(this_ptr, Z_OBJCE_P(this_ptr), NULL, ZEND_STRL("getcontents"), return_value, 0, NULL, NULL);
    if (UNEXPECTED(EG(exception))) {
        zend_clear_exception();
        RETURN_EMPTY_STRING();
    }
}

static PHP_METHOD(TurboSlim_Http_Stream, close)
{
    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    stream_t* x    = stream_from_zobj(Z_OBJ_P(this_ptr));

    if (is_attached(x)) {
        if (x->is_pipe == 1) {
            FG(pclose_wait) = 1;
            zend_list_close(x->stream->res);
            FG(pclose_wait) = 0;
        }
        else if ((x->stream->flags & PHP_STREAM_FLAG_NO_FCLOSE) == 0) {
            int flags = PHP_STREAM_FREE_KEEP_RSRC;
            flags    |= (x->stream->is_persistent ? PHP_STREAM_FREE_CLOSE_PERSISTENT : PHP_STREAM_FREE_CLOSE);
            php_stream_free(x->stream, flags);
        }
        else {
            zend_error(E_WARNING, "%d is not a valid stream resource", x->stream->res->handle);
        }
    }

    detach(x);
}

static PHP_METHOD(TurboSlim_Http_Stream, getSize)
{
    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    stream_t* x    = stream_from_zobj(Z_OBJ_P(this_ptr));

    if (is_attached(x)) {
        php_stream_statbuf stat_ssb;

        if (!php_stream_stat(x->stream, &stat_ssb)) {
            RETURN_LONG(stat_ssb.sb.st_size);
        }
    }

    RETURN_NULL();
}

static PHP_METHOD(TurboSlim_Http_Stream, tell)
{
    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    stream_t* x    = stream_from_zobj(Z_OBJ_P(this_ptr));

    if (is_attached(x)) {
        zend_long pos = php_stream_tell(x->stream);

        if (pos != -1) {
            RETURN_LONG(pos);
        }
    }

    zend_throw_exception(spl_ce_RuntimeException, "Could not get the position of the pointer in stream", 0);
}

static PHP_METHOD(TurboSlim_Http_Stream, eof)
{
    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    stream_t* x    = stream_from_zobj(Z_OBJ_P(this_ptr));

    if (is_attached(x)) {
        RETURN_BOOL(php_stream_eof(x->stream));
    }

    RETURN_TRUE;
}

static PHP_METHOD(TurboSlim_Http_Stream, isReadable)
{
    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    stream_t* x    = stream_from_zobj(Z_OBJ_P(this_ptr));

    if (is_attached(x)) {
        RETURN_BOOL(x->readable);
    }

    RETURN_FALSE;
}

static PHP_METHOD(TurboSlim_Http_Stream, isWritable)
{
    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    stream_t* x    = stream_from_zobj(Z_OBJ_P(this_ptr));

    if (is_attached(x)) {
        RETURN_BOOL(x->writable);
    }

    RETURN_FALSE;
}

static PHP_METHOD(TurboSlim_Http_Stream, isSeekable)
{
    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    stream_t* x    = stream_from_zobj(Z_OBJ_P(this_ptr));

    if (is_attached(x)) {
        RETURN_BOOL(x->seekable);
    }

    RETURN_FALSE;
}

static PHP_METHOD(TurboSlim_Http_Stream, seek)
{
    zend_long offset;
    zend_long whence = SEEK_SET;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_LONG(offset)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(whence)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    stream_t* x    = stream_from_zobj(Z_OBJ_P(this_ptr));

    if (is_attached(x) && x->seekable && -1 != php_stream_seek(x->stream, offset, (int)whence)) {
        return;
    }

    zend_throw_exception(spl_ce_RuntimeException, "Could not seek in stream", 0);
}

static PHP_METHOD(TurboSlim_Http_Stream, rewind)
{
    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    stream_t* x    = stream_from_zobj(Z_OBJ_P(this_ptr));

    if (is_attached(x) && x->seekable && -1 != php_stream_rewind(x->stream)) {
        return;
    }

    zend_throw_exception(spl_ce_RuntimeException, "Could not rewind stream", 0);
}

static PHP_METHOD(TurboSlim_Http_Stream, read)
{
    zend_long length;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(length)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    if (length <= 0) {
        zend_throw_exception(spl_ce_RuntimeException, "length parameter must be positive", 0);
        return;
    }

    zval* this_ptr = get_this(execute_data);
    stream_t* x    = stream_from_zobj(Z_OBJ_P(this_ptr));

    if (is_attached(x) && x->readable) {
        ZVAL_NEW_STR(return_value, zend_string_alloc(length, 0));
        Z_STRLEN_P(return_value) = php_stream_read(x->stream, Z_STRVAL_P(return_value), length);
        Z_STRVAL_P(return_value)[Z_STRLEN_P(return_value)] = 0;

        if (Z_STRLEN_P(return_value) < length / 2) {
            Z_STR_P(return_value) = zend_string_truncate(Z_STR_P(return_value), Z_STRLEN_P(return_value), 0);
        }

        return;
    }

    zend_throw_exception(spl_ce_RuntimeException, "Could not read from stream", 0);
}

static PHP_METHOD(TurboSlim_Http_Stream, write)
{
    char* string;
    size_t string_len;

    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(string, string_len)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    if (string_len == 0) {
        RETURN_LONG(0);
    }

    zval* this_ptr = get_this(execute_data);
    stream_t* x    = stream_from_zobj(Z_OBJ_P(this_ptr));

    if (is_attached(x) && x->writable) {
        zend_long written = php_stream_write(x->stream, string, string_len);
        RETURN_LONG(written);
    }

    zend_throw_exception(spl_ce_RuntimeException, "Could not write to stream", 0);
}

static PHP_METHOD(TurboSlim_Http_Stream, getContents)
{
    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    stream_t* x    = stream_from_zobj(Z_OBJ_P(this_ptr));

    if (is_attached(x) && x->readable) {
        zend_string* contents = php_stream_copy_to_mem(x->stream, PHP_STREAM_COPY_ALL, 0);
        if (contents) {
            RETURN_STR(contents);
        }

        RETURN_EMPTY_STRING();
    }

    zend_throw_exception(spl_ce_RuntimeException, "Could not get contents of stream", 0);
}

static PHP_METHOD(TurboSlim_Http_Stream, isPipe)
{
    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    stream_t* x    = stream_from_zobj(Z_OBJ_P(this_ptr));

    RETURN_BOOL(is_pipe(x));
}

static PHP_METHOD(TurboSlim_Http_Stream, __debugInfo)
{
    /* LCOV_EXCL_BR_START */
    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    /* LCOV_EXCL_BR_STOP */

    zval* this_ptr = get_this(execute_data);
    stream_t* v    = stream_from_zobj(Z_OBJ_P(this_ptr));
    HashTable* ht  = get_debug_info_fast(v, this_ptr);

    RETURN_ARR(ht);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_empty, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_attach, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_getmetadata, 0, ZEND_RETURN_VALUE, 0)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_seek, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, offset)
    ZEND_ARG_INFO(0, whence)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_read, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_write, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, string)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo___debuginfo, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO();

const zend_function_entry fe_TurboSlim_Http_Stream[] = {
    ZEND_ME(TurboSlim_Http_Stream, __construct, arginfo_attach,      ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    ZEND_ME(TurboSlim_Http_Stream, getMetadata, arginfo_getmetadata, ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Stream, isAttached,  arginfo_empty,       ZEND_ACC_PROTECTED)
    ZEND_ME(TurboSlim_Http_Stream, attach,      arginfo_attach,      ZEND_ACC_PROTECTED)
    ZEND_ME(TurboSlim_Http_Stream, detach,      arginfo_empty,       ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Stream, __toString,  arginfo_empty,       ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Stream, close,       arginfo_empty,       ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Stream, getSize,     arginfo_empty,       ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Stream, tell,        arginfo_empty,       ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Stream, eof,         arginfo_empty,       ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Stream, isReadable,  arginfo_empty,       ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Stream, isWritable,  arginfo_empty,       ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Stream, isSeekable,  arginfo_empty,       ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Stream, seek,        arginfo_seek,        ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Stream, rewind,      arginfo_empty,       ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Stream, read,        arginfo_read,        ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Stream, write,       arginfo_write,       ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Stream, getContents, arginfo_empty,       ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Stream, isPipe,      arginfo_empty,       ZEND_ACC_PUBLIC)
    ZEND_ME(TurboSlim_Http_Stream, __debugInfo, arginfo___debuginfo, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};
