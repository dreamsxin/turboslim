#include "turboslim/internal/closure.h"

#include <Zend/zend_interfaces.h>
#include <Zend/zend_operators.h>
#include "turboslim/internal/internalclass.h"

typedef struct turboslim_closure {
    zend_object std;
    zend_function* invoker;
    zval callable;
    zend_fcall_info fci;
    zend_fcall_info_cache fcc;
    HashTable bound;
} turboslim_closure_t;

TURBOSLIM_ATTR_NONNULL static inline turboslim_closure_t* turboslim_closure_from_zobj(zend_object* z)
{
    return (turboslim_closure_t*)z;
}

zend_class_entry* ce_TurboSlim_Internal_Closure = NULL;
zend_object_handlers turboslim_internal_closure_handlers;

zend_object* turboslim_internal_closure_create_object(zend_class_entry* ce)
{
    turboslim_closure_t* v = ecalloc(1, sizeof(turboslim_closure_t));

    zend_object_std_init(&v->std, ce);
    v->std.handlers = &turboslim_internal_closure_handlers;
    v->invoker = NULL;

    ZVAL_UNDEF(&v->callable);

    zend_hash_init(&v->bound, 0, NULL, ZVAL_PTR_DTOR, 0);

    return (zend_object*)v;
}

void turboslim_internal_closure_free_obj(zend_object* obj)
{
    turboslim_closure_t* v = turboslim_closure_from_zobj(obj);
    zend_object_std_dtor(&v->std);
    zval_ptr_dtor(&v->callable);
    zend_hash_destroy(&v->bound);
}

zend_object* turboslim_internal_closure_clone_obj(zval* object)
{
    zend_object* old_object     = Z_OBJ_P(object);
    zend_object* new_object     = turboslim_internal_closure_create_object(old_object->ce);

    turboslim_closure_t* mine   = turboslim_closure_from_zobj(old_object);
    turboslim_closure_t* theirs = turboslim_closure_from_zobj(new_object);

    assert(old_object->ce == ce_TurboSlim_Internal_Closure); /* LCOV_EXCL_BR_LINE */
    assert(!theirs->fci.params && !theirs->fci.param_count); /* LCOV_EXCL_BR_LINE */

    ZVAL_COPY(&theirs->callable, &mine->callable);
    theirs->invoker = mine->invoker;
    theirs->fci     = mine->fci;
    theirs->fcc     = mine->fcc;

    zend_hash_copy(&theirs->bound, &mine->bound, zval_add_ref);
    return new_object;
}

int turboslim_internal_closure_get_closure(zval* object, zend_class_entry** ce_ptr, zend_function** fptr_ptr, zend_object** obj_ptr)
{
    if (UNEXPECTED(Z_TYPE_P(object) != IS_OBJECT)) {
        return FAILURE;
    }

    assert(Z_OBJCE_P(object) == ce_TurboSlim_Internal_Closure); /* LCOV_EXCL_BR_LINE */

    turboslim_closure_t* w = turboslim_closure_from_zobj(Z_OBJ_P(object));

    if (UNEXPECTED(w->invoker == NULL)) {
        return FAILURE;
    }

    *fptr_ptr = w->invoker;
    *ce_ptr   = Z_OBJCE_P(object);
    *obj_ptr  = Z_OBJ_P(object);
    return SUCCESS;
}

HashTable* turboslim_internal_closure_get_gc(zval* object, zval** table, int* n)
{
    turboslim_closure_t* v = turboslim_closure_from_zobj(Z_OBJ_P(object));

    *table = &v->callable;
    *n     = 1;
    return &v->bound;
}

int turboslim_internal_closure_compare_objects(zval* z1, zval* z2)
{
    zval r;
    zend_object* zobj1 = Z_OBJ_P(z1);
    zend_object* zobj2 = Z_OBJ_P(z2);

    if (zobj1->ce != zobj2->ce) {  /* LCOV_EXCL_BR_LINE */
        return 1;                  /* LCOV_EXCL_LINE - this cannot be tested without dirty hacks */
    }

    turboslim_closure_t* v = turboslim_closure_from_zobj(zobj1);
    turboslim_closure_t* w = turboslim_closure_from_zobj(zobj2);

    if (v->invoker != w->invoker) {
        return 1;
    }

    if (zend_compare_symbol_tables(&v->bound, &w->bound)) {
        return 1;
    }

    if (UNEXPECTED(FAILURE == compare_function(&r, &v->callable, &w->callable)) || Z_LVAL(r) != 0) {
        return 1;
    }

    /*
     * This does not work:
     *
     * memcmp(&v->fcc, &w->fcc, sizeof(v))
     *
     * Well, it works, but valgrind complains:
     *
     *      Conditional jump or move depends on uninitialised value(s)
     *
     * If we check fields one by one, everything is OK.
     */
#if PHP_VERSION_ID < 70300
    if (!v->fcc.initialized && !w->fcc.initialized) {
        return 0;
    }

    if (v->fcc.initialized != w->fcc.initialized) {
        return 1;
    }
#endif

    if (
           v->fcc.called_scope     != w->fcc.called_scope
        || v->fcc.calling_scope    != w->fcc.calling_scope
        || v->fcc.function_handler != w->fcc.function_handler
        || v->fcc.object           != w->fcc.object
    ) {
        return 1;
    }

    return 0;
}

void turboslim_closure_create(zval* return_value, zend_function* invoker, zval* callable, zend_fcall_info* fci, zend_fcall_info_cache* fcc, HashTable* bound)
{
    object_init_ex(return_value, ce_TurboSlim_Internal_Closure);
    turboslim_closure_t* w = turboslim_closure_from_zobj(Z_OBJ_P(return_value));

    w->invoker = invoker;

    if (callable) {
        ZVAL_COPY(&w->callable, callable);
    }

    if (fcc && fci && fci->size == sizeof(zend_fcall_info)) {
        w->fci = *fci;
        w->fcc = *fcc;
    }

    if (bound) {
        zend_hash_copy(&w->bound, bound, zval_add_ref);
    }
}

HashTable* turboslim_closure_get_bound(zval* object)
{
    assert(Z_TYPE_P(object) == IS_OBJECT && Z_OBJCE_P(object) == ce_TurboSlim_Internal_Closure);    /* LCOV_EXCL_BR_LINE */

    turboslim_closure_t* w = turboslim_closure_from_zobj(Z_OBJ_P(object));
    return &w->bound;
}

zval* turboslim_closure_get_callable(zval* object, zend_fcall_info** fci, zend_fcall_info_cache** fcc)
{
    assert(Z_TYPE_P(object) == IS_OBJECT && Z_OBJCE_P(object) == ce_TurboSlim_Internal_Closure);    /* LCOV_EXCL_BR_LINE */

    turboslim_closure_t* w = turboslim_closure_from_zobj(Z_OBJ_P(object));
    *fci = &w->fci;
    *fcc = &w->fcc;
    return &w->callable;
}
