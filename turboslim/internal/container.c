#include "turboslim/container.h"

#include <Zend/zend_interfaces.h>
#include "turboslim/internal/internalclass.h"

typedef struct wrapper {
    zend_object std;
    zval orig;
} wrapper_t;

zend_class_entry* ce_TurboSlim_Internal_Container_ProtectedCallable = NULL;
zend_class_entry* ce_TurboSlim_Internal_Container_FactoryCallable   = NULL;
zend_class_entry* ce_TurboSlim_Internal_Container_ServiceCallable   = NULL;
zend_object_handlers turboslim_internal_container_handlers;

zend_object* turboslim_internal_container_create_object(zend_class_entry* ce)
{
    wrapper_t* v = ecalloc(1, sizeof(wrapper_t));

    zend_object_std_init(&v->std, ce);
    v->std.handlers = &turboslim_internal_container_handlers;
    ZVAL_UNDEF(&v->orig);

    return (zend_object*)v;
}

void turboslim_internal_container_free_obj(zend_object* object)
{
    wrapper_t* w = (wrapper_t*)object;

    zend_object_std_dtor(&w->std);
    zval_ptr_dtor(&w->orig);
}

zend_object* turboslim_internal_container_clone_obj(zval* obj)
{
    zend_object* old_object = Z_OBJ_P(obj);
    zend_object* new_object = turboslim_internal_container_create_object(old_object->ce);

    wrapper_t* mine   = (wrapper_t*)old_object;
    wrapper_t* theirs = (wrapper_t*)new_object;

    ZVAL_COPY(&theirs->orig, &mine->orig);

    return new_object;
}

int turboslim_internal_container_compare_objects(zval* object1, zval* object2)
{
    zend_object* zobj1 = Z_OBJ_P(object1);
    zend_object* zobj2 = Z_OBJ_P(object2);

    if (zobj1->ce != zobj2->ce) {  /* LCOV_EXCL_BR_LINE */
        return 1;                  /* LCOV_EXCL_LINE - this cannot be tested without dirty hacks */
    }

    wrapper_t* v1 = (wrapper_t*)zobj1;
    wrapper_t* v2 = (wrapper_t*)zobj2;

    zval r;
    if (UNEXPECTED(FAILURE == compare_function(&r, &v1->orig, &v2->orig)) || Z_LVAL(r) != 0) {
        return 1;
    }

    return 0;
}

HashTable* turboslim_internal_container_get_gc(zval* object, zval** table, int* n)
{
    wrapper_t* w = (wrapper_t*)Z_OBJ_P(object);

    *table = &w->orig;
    *n     = 1;
    return NULL;
}

static inline int is_supported_ce(zend_class_entry* ce)
{
    /* LCOV_EXCL_BR_START */
    return EXPECTED(
            ce == ce_TurboSlim_Internal_Container_ProtectedCallable
         || ce == ce_TurboSlim_Internal_Container_FactoryCallable
         || ce == ce_TurboSlim_Internal_Container_ServiceCallable
    );
    /* LCOV_EXCL_BR_STOP */
}

void turboslim_container_createCallableWrapper(zval* return_value, zend_class_entry* ce, zval* callable)
{
    assert(is_supported_ce(ce));                /* LCOV_EXCL_BR_LINE */
    object_init_ex(return_value, ce);
    wrapper_t* w = (wrapper_t*)Z_OBJ_P(return_value);
    ZVAL_COPY(&w->orig, callable);
}

void turboslim_container_getCallableFromWrapper(zval* return_value, zval* wrapper)
{
    assert(Z_TYPE_P(wrapper) == IS_OBJECT);     /* LCOV_EXCL_BR_LINE */
    zend_object* obj     = Z_OBJ_P(wrapper);
    zend_class_entry* ce = obj->ce;

    assert(is_supported_ce(ce));                /* LCOV_EXCL_BR_LINE */
    wrapper_t* w = (wrapper_t*)obj;
    ZVAL_COPY(return_value, &w->orig);
}
