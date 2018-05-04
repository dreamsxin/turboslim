#include "container.h"

#include <Zend/zend_interfaces.h>
#include "internal/internalclass.h"

typedef struct wrapper {
    zend_object std;
    zval orig;
} wrapper_t;

zend_class_entry* ce_TurboSlim_Internal_Container_ProtectedCallable = NULL;
zend_class_entry* ce_TurboSlim_Internal_Container_FactoryCallable   = NULL;
zend_class_entry* ce_TurboSlim_Internal_Container_ServiceCallable   = NULL;

static zend_object_handlers handlers;

static zend_object* create_object(zend_class_entry* ce)
{
    wrapper_t* v = ecalloc(1, sizeof(wrapper_t));

    zend_object_std_init(&v->std, ce);
    v->std.handlers = &handlers;
    ZVAL_UNDEF(&v->orig);

    return (zend_object*)v;
}

static void free_obj(zend_object* object)
{
    wrapper_t* w = (wrapper_t*)object;

    zend_object_std_dtor(&w->std);
    zval_ptr_dtor(&w->orig);
}

static zend_object* clone_obj(zval* obj)
{
    wrapper_t* mine = (wrapper_t*)Z_OBJ_P(obj);
    zval rv;

    object_init_ex(&rv, Z_OBJCE_P(obj));
    wrapper_t* theirs = (wrapper_t*)Z_OBJ(rv);
    ZVAL_COPY(&theirs->orig, &mine->orig);

    return Z_OBJ(rv);
}

int compare_objects(zval* object1, zval* object2)
{
    zend_object* zobj1 = Z_OBJ_P(object1);
    zend_object* zobj2 = Z_OBJ_P(object2);

    if (zobj1->ce != zobj2->ce) {  /* LCOV_EXCL_BR_LINE */
        return 1;                  /* LCOV_EXCL_LINE - this cannot be tested without dirty hacks */
    }

    wrapper_t* v1 = (wrapper_t*)zobj1;
    wrapper_t* v2 = (wrapper_t*)zobj2;

    zval res;
    is_equal_function(&res, &v1->orig, &v2->orig);
    return i_zend_is_true(&res) ? 0 : 1;
}


static HashTable* get_gc(zval* object, zval** table, int* n)
{
    wrapper_t* w = (wrapper_t*)Z_OBJ_P(object);

    *table = &w->orig;
    *n     = 1;
    return NULL;
}

int init_internal_container_classes()
{
    zend_class_entry ce;

    memcpy(&handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    handlers.free_obj             = free_obj;
    handlers.get_constructor      = turboslim_deny_construct;
    handlers.write_property       = turboslim_deny_write_property;
    handlers.read_property        = turboslim_deny_read_property;
    handlers.get_property_ptr_ptr = turboslim_deny_get_property_ptr_ptr;
    handlers.has_property         = turboslim_deny_has_property;
    handlers.unset_property       = turboslim_deny_unset_property;
    handlers.clone_obj            = clone_obj;
    handlers.compare_objects      = compare_objects;
    handlers.get_gc               = get_gc;

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Internal\\Container\\ProtectedCallable", NULL);
    ce_TurboSlim_Internal_Container_ProtectedCallable = zend_register_internal_class(&ce);
    ce_TurboSlim_Internal_Container_ProtectedCallable->ce_flags |= ZEND_ACC_FINAL;

    ce_TurboSlim_Internal_Container_ProtectedCallable->create_object = create_object;
    ce_TurboSlim_Internal_Container_ProtectedCallable->serialize     = zend_class_serialize_deny;
    ce_TurboSlim_Internal_Container_ProtectedCallable->unserialize   = zend_class_unserialize_deny;

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Internal\\Container\\FactoryCallable", NULL);
    ce_TurboSlim_Internal_Container_FactoryCallable = zend_register_internal_class(&ce);
    ce_TurboSlim_Internal_Container_FactoryCallable->ce_flags |= ZEND_ACC_FINAL;

    ce_TurboSlim_Internal_Container_FactoryCallable->create_object = create_object;
    ce_TurboSlim_Internal_Container_FactoryCallable->serialize     = zend_class_serialize_deny;
    ce_TurboSlim_Internal_Container_FactoryCallable->unserialize   = zend_class_unserialize_deny;

    INIT_CLASS_ENTRY(ce, "TurboSlim\\Internal\\Container\\ServiceCallable", NULL);
    ce_TurboSlim_Internal_Container_ServiceCallable = zend_register_internal_class(&ce);
    ce_TurboSlim_Internal_Container_ServiceCallable->ce_flags |= ZEND_ACC_FINAL;

    ce_TurboSlim_Internal_Container_ServiceCallable->create_object = create_object;
    ce_TurboSlim_Internal_Container_ServiceCallable->serialize     = zend_class_serialize_deny;
    ce_TurboSlim_Internal_Container_ServiceCallable->unserialize   = zend_class_unserialize_deny;

    return SUCCESS;
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
    if (is_supported_ce(ce)) {                      /* LCOV_EXCL_BR_LINE */
        object_init_ex(return_value, ce);
        wrapper_t* w = (wrapper_t*)Z_OBJ_P(return_value);
        ZVAL_COPY(&w->orig, callable);
        return;
    }

    ZVAL_NULL(return_value);                        /* LCOV_EXCL_LINE */
}

void turboslim_container_getCallableFromWrapper(zval* return_value, zval* wrapper)
{
    if (EXPECTED(Z_TYPE_P(wrapper) == IS_OBJECT)) { /* LCOV_EXCL_BR_LINE */
        zend_object* obj     = Z_OBJ_P(wrapper);
        zend_class_entry* ce = obj->ce;

        if (is_supported_ce(ce)) {                  /* LCOV_EXCL_BR_LINE */
            wrapper_t* w = (wrapper_t*)obj;
            ZVAL_COPY(return_value, &w->orig);
            return;
        }
    }

    ZVAL_NULL(return_value);                        /* LCOV_EXCL_LINE */
}
