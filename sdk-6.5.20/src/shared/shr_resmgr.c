/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Global resource allocator
 */

#include <shared/bsl.h>

#include <sal/core/sync.h>
#include <sal/core/alloc.h>
#include <shared/error.h>
#include <shared/shr_resmgr.h>

/* FIXME: No SOC headers in SHR !!! */
#include <soc/cm.h>

/*****************************************************************************/
/*
 *  Internal implementation
 */

/*
 *  This structure is global, and points to the information about all units.
 *
 *  For each unit, it's just a pointer here, since this reduces the overall
 *  memory footprint for the case of units that do not use this mechanism, plus
 *  it allows each unit to use only as much memory as needed to describe its
 *  resources and pools and how they map.
 */
_shr_res_unit_desc_t *_g_unitResDesc[BCM_LOCAL_UNITS_MAX] = { NULL };

/*
 *  Various function prototypes per method for the alloc managers.
 */
typedef int (*_shr_res_alloc_create)(_shr_res_pool_desc_t **desc,
                                     int low_id,
                                     int count,
                                     const void *extras,
                                     const char *name);
typedef int (*_shr_res_alloc_destroy)(_shr_res_pool_desc_t *desc);
typedef int (*_shr_res_alloc_alloc)(_shr_res_pool_desc_t *desc,
                                    uint32 flags,
                                    int count,
                                    int *elem);
typedef int (*_shr_res_alloc_tag)(_shr_res_pool_desc_t *desc,
                                  uint32 flags,
                                  const void* tag,
                                  int count,
                                  int *elem);
typedef int (*_shr_res_alloc_align)(_shr_res_pool_desc_t *desc,
                                    uint32 flags,
                                    int align,
                                    int offset,
                                    int count,
                                    int *elem);
typedef int (*_shr_res_alloc_align_sparse)(_shr_res_pool_desc_t *desc,
                                           uint32 flags,
                                           int align,
                                           int offs,
                                           uint32 pattern,
                                           int length,
                                           int repeats,
                                           int *elem);
typedef int (*_shr_res_alloc_align_tag)(_shr_res_pool_desc_t *desc,
                                        uint32 flags,
                                        int align,
                                        int offset,
                                        const void* tag,
                                        int count,
                                        int *elem);
typedef int (*_shr_res_alloc_free)(_shr_res_pool_desc_t *desc,
                                   int count,
                                   int elem);
typedef int (*_shr_res_alloc_free_sparse)(_shr_res_pool_desc_t *desc,
                                          uint32 pattern,
                                          int length,
                                          int repeats,
                                          int elem);
typedef int (*_shr_res_alloc_check)(_shr_res_pool_desc_t *desc,
                                    int count,
                                    int elem);
typedef int (*_shr_res_alloc_check_all)(_shr_res_pool_desc_t *desc,
                                        int count,
                                        int elem);
typedef int (*_shr_res_alloc_check_all_sparse)(_shr_res_pool_desc_t *desc,
                                               uint32 pattern,
                                               int length,
                                               int repeats,
                                               int elem);
typedef int (*_shr_res_alloc_check_all_tag)(_shr_res_pool_desc_t *desc,
                                            const void *tag,
                                            int count,
                                            int elem);

/*
 *  This structure describes a single allocator mechanism, specifically by
 *  providing a set pointers to functions that are used to manipulate it.
 *
 *  Many functions are mandatory.  Those that are optional are, in fact,
 *  mandatory in groups (all or none of the 'align' group must be provded; all
 *  or none of the 'tag' group must be provided; all or nothing of 'align_tag'
 *  must be provided, and if all is provided, all of 'align' and all of 'tag'
 *  must also be provided).
 */
typedef struct _shr_res_alloc_mgr_s {
    _shr_res_alloc_create create;                /* mandatory */
    _shr_res_alloc_destroy destroy;              /* mandatory */
    _shr_res_alloc_alloc alloc;                  /* mandatory */
    _shr_res_alloc_tag tag;                      /* optional */
    _shr_res_alloc_align align;                  /* optional */
    _shr_res_alloc_align_sparse align_sparse;    /* optional */
    _shr_res_alloc_align_tag tag_align;          /* optional */
    _shr_res_alloc_free free;                    /* mandatory */
    _shr_res_alloc_free_sparse free_sparse;      /* iff have align_sparse */
    _shr_res_alloc_check check;                  /* mandatory */
    _shr_res_alloc_check_all check_all;          /* mandatory */
    _shr_res_alloc_check_all_sparse c_a_sparse;  /* iff have align_sparse */
    _shr_res_alloc_check_all_tag check_all_tag;  /* iff have tag or tag_align */
    char *name;
} _shr_res_alloc_mgr_t;

/*
 *  These prototypes are for the global const structure below that points to
 *  all of the various implementations.
 */
static int _shr_res_bitmap_create(_shr_res_pool_desc_t **desc,
                                  int low_id,
                                  int count,
                                  const void* extras,
                                  const char* name);
static int _shr_res_bitmap_destroy(_shr_res_pool_desc_t *desc);
static int _shr_res_bitmap_alloc(_shr_res_pool_desc_t *desc,
                                 uint32 flags,
                                 int count,
                                 int *elem);
static int _shr_res_bitmap_alloc_align(_shr_res_pool_desc_t *desc,
                                       uint32 flags,
                                       int align,
                                       int offs,
                                       int count,
                                       int *elem);
static int _shr_res_bitmap_alloc_align_sparse(_shr_res_pool_desc_t *desc,
                                              uint32 flags,
                                              int align,
                                              int offs,
                                              uint32 pattern,
                                              int length,
                                              int repeats,
                                              int *elem);
static int _shr_res_bitmap_free(_shr_res_pool_desc_t *desc,
                                int count,
                                int elem);
static int _shr_res_bitmap_free_sparse(_shr_res_pool_desc_t *desc,
                                       uint32 pattern,
                                       int length,
                                       int repeats,
                                       int elem);
static int _shr_res_bitmap_check(_shr_res_pool_desc_t *desc,
                                 int count,
                                 int elem);
static int _shr_res_bitmap_check_all_desc(_shr_res_pool_desc_t *desc,
                                     int count,
                                     int elem);
static int _shr_res_bitmap_check_all_sparse(_shr_res_pool_desc_t *desc,
                                            uint32 pattern,
                                            int length,
                                            int repeats,
                                            int elem);
static int _shr_res_tag_bitmap_create(_shr_res_pool_desc_t **desc,
                                      int low_id,
                                      int count,
                                      const void* extras,
                                      const char* name);
static int _shr_res_tag_bitmap_destroy(_shr_res_pool_desc_t *desc);
static int _shr_res_tag_bitmap_alloc(_shr_res_pool_desc_t *desc,
                                     uint32 flags,
                                     int count,
                                     int *elem);
static int _shr_res_tag_bitmap_alloc_tag(_shr_res_pool_desc_t *desc,
                                         uint32 flags,
                                         const void *tag,
                                         int count,
                                         int *elem);
static int _shr_res_tag_bitmap_alloc_align(_shr_res_pool_desc_t *desc,
                                           uint32 flags,
                                           int align,
                                           int offs,
                                           int count,
                                           int *elem);
static int _shr_res_tag_bitmap_alloc_align_tag(_shr_res_pool_desc_t *desc,
                                               uint32 flags,
                                               int align,
                                               int offs,
                                               const void *tag,
                                               int count,
                                               int *elem);
static int _shr_res_tag_bitmap_free(_shr_res_pool_desc_t *desc,
                                    int count,
                                    int elem);
static int _shr_res_tag_bitmap_check(_shr_res_pool_desc_t *desc,
                                     int count,
                                     int elem);
static int _shr_res_tag_bitmap_check_all(_shr_res_pool_desc_t *desc,
                                         int count,
                                         int elem);
static int _shr_res_tag_bitmap_check_all_tag(_shr_res_pool_desc_t *desc,
                                             const void *tag,
                                             int count,
                                             int elem);
static int _shr_res_idxres_create(_shr_res_pool_desc_t **desc,
                                  int low_id,
                                  int count,
                                  const void* extras,
                                  const char* name);
static int _shr_res_idxres_destroy(_shr_res_pool_desc_t *desc);
static int _shr_res_idxres_alloc(_shr_res_pool_desc_t *desc,
                                 uint32 flags,
                                 int count,
                                 int *elem);
static int _shr_res_idxres_free(_shr_res_pool_desc_t *desc,
                                int count,
                                int elem);
static int _shr_res_idxres_check(_shr_res_pool_desc_t *desc,
                                 int count,
                                 int elem);
static int _shr_res_idxres_check_all(_shr_res_pool_desc_t *desc,
                                     int count,
                                     int elem);
static int _shr_res_aidxres_create(_shr_res_pool_desc_t **desc,
                                   int low_id,
                                   int count,
                                   const void* extras,
                                   const char* name);
static int _shr_res_aidxres_destroy(_shr_res_pool_desc_t *desc);
static int _shr_res_aidxres_alloc(_shr_res_pool_desc_t *desc,
                                  uint32 flags,
                                  int count,
                                  int *elem);
static int _shr_res_aidxres_free(_shr_res_pool_desc_t *desc,
                                 int count,
                                 int elem);
static int _shr_res_aidxres_check(_shr_res_pool_desc_t *desc,
                                  int count,
                                  int elem);
static int _shr_res_aidxres_check_all(_shr_res_pool_desc_t *desc,
                                      int count,
                                      int elem);
static int _shr_res_mdb_create(_shr_res_pool_desc_t **desc,
                               int low_id,
                               int count,
                               const void* extras,
                               const char* name);
static int _shr_res_mdb_destroy(_shr_res_pool_desc_t *desc);
static int _shr_res_mdb_alloc(_shr_res_pool_desc_t *desc,
                              uint32 flags,
                              int count,
                              int *elem);
static int _shr_res_mdb_free(_shr_res_pool_desc_t *desc,
                             int count,
                             int elem);
static int _shr_res_mdb_check(_shr_res_pool_desc_t *desc,
                              int count,
                              int elem);
static int _shr_res_mdb_check_all(_shr_res_pool_desc_t *desc,
                                  int count,
                                  int elem);

/*
 *  Global const structure describing the various allocator mechanisms.
 */
static const _shr_res_alloc_mgr_t _shr_res_alloc_mgrs[SHR_RES_ALLOCATOR_COUNT] =
    {
        {
            _shr_res_bitmap_create,
            _shr_res_bitmap_destroy,
            _shr_res_bitmap_alloc,
            NULL,
            _shr_res_bitmap_alloc_align,
            _shr_res_bitmap_alloc_align_sparse,
            NULL,
            _shr_res_bitmap_free,
            _shr_res_bitmap_free_sparse,
            _shr_res_bitmap_check,
            _shr_res_bitmap_check_all_desc,
            _shr_res_bitmap_check_all_sparse,
            NULL,
            "SHR_RES_ALLOCATOR_BITMAP"
        } /* bitmap */,
        {
            _shr_res_tag_bitmap_create,
            _shr_res_tag_bitmap_destroy,
            _shr_res_tag_bitmap_alloc,
            _shr_res_tag_bitmap_alloc_tag,
            _shr_res_tag_bitmap_alloc_align,
            NULL,
            _shr_res_tag_bitmap_alloc_align_tag,
            _shr_res_tag_bitmap_free,
            NULL,
            _shr_res_tag_bitmap_check,
            _shr_res_tag_bitmap_check_all,
            NULL,
            _shr_res_tag_bitmap_check_all_tag,
            "SHR_RES_ALLOCATOR_TAGGED_BITMAP"
        } /* tagged bitmap */,
        {
            _shr_res_idxres_create,
            _shr_res_idxres_destroy,
            _shr_res_idxres_alloc,
            NULL,
            NULL,
            NULL,
            NULL,
            _shr_res_idxres_free,
            NULL,
            _shr_res_idxres_check,
            _shr_res_idxres_check_all,
            NULL,
            NULL,
            "SHR_RES_ALLOCATOR_IDXRES"
        } /* idxres */,
        {
            _shr_res_aidxres_create,
            _shr_res_aidxres_destroy,
            _shr_res_aidxres_alloc,
            NULL,
            NULL,
            NULL,
            NULL,
            _shr_res_aidxres_free,
            NULL,
            _shr_res_aidxres_check,
            _shr_res_aidxres_check_all,
            NULL,
            NULL,
            "SHR_RES_ALLOCATOR_AIDXRES"
        } /* aidxres */,
        {
            _shr_res_mdb_create,
            _shr_res_mdb_destroy,
            _shr_res_mdb_alloc,
            NULL,
            NULL,
            NULL,
            NULL,
            _shr_res_mdb_free,
            NULL,
            _shr_res_mdb_check,
            _shr_res_mdb_check_all,
            NULL,
            NULL,
            "SHR_RES_ALLOCATOR_MDB"
        } /* mdb */
    };

/*
 *  Basic checks performed for many functions
 */
#define RES_UNIT_CHECK(_unit, _unitInfo) \
    if ((0 > (_unit)) || (BCM_LOCAL_UNITS_MAX <= (_unit))) { \
        LOG_ERROR(BSL_LS_SOC_COMMON, \
                  (BSL_META("invalid unit number %d\n"), \
                   _unit)); \
        return _SHR_E_PARAM; \
    } \
    if (!(_g_unitResDesc[_unit])) { \
        LOG_ERROR(BSL_LS_SOC_COMMON, \
                  (BSL_META("unit %d is not initialised\n"), \
                   _unit)); \
        return _SHR_E_INIT; \
    } \
    (_unitInfo) = _g_unitResDesc[_unit]
#define RES_HANDLE_VALID_CHECK(_handle) \
    if (!(_handle)) { \
        LOG_ERROR(BSL_LS_SOC_COMMON, \
                  (BSL_META("NULL handle is not valid\n"))); \
        return _SHR_E_PARAM; \
    }
#define RES_POOL_VALID_CHECK(_handle, _pool) \
    if ((0 > (_pool)) || ((_handle)->resPoolCount <= (_pool))) { \
        LOG_ERROR(BSL_LS_SOC_COMMON, \
                  (BSL_META("%p pool %d does not exist\n"), \
                   ((void*)(_handle)), _pool)); \
        return _SHR_E_PARAM; \
    }
#define RES_POOL_EXIST_CHECK(_handle, _pool) \
    if (!((_handle)->pool[_pool])) { \
        LOG_ERROR(BSL_LS_SOC_COMMON, \
                  (BSL_META("%p pool %d is not configured\n"), \
                   ((void*)(_handle)), _pool)); \
        return _SHR_E_CONFIG; \
    }
#define RES_TYPE_VALID_CHECK(_handle, _type) \
    if ((0 > (_type)) || ((_handle)->resTypeCount <= (_type))) { \
        LOG_ERROR(BSL_LS_SOC_COMMON, \
                  (BSL_META("%p resource %d does not exist\n"), \
                   ((void*)(_handle)), _type)); \
        return _SHR_E_PARAM; \
    }
#define RES_TYPE_EXIST_CHECK(_handle, _type) \
    if (!((_handle)->res[_type])) { \
        LOG_ERROR(BSL_LS_SOC_COMMON, \
                  (BSL_META("%p resource %d is not configured\n"), \
                   ((void*)(_handle)), _type)); \
        return _SHR_E_CONFIG; \
    }

/*
 *  Destroys all of the resources and then pools for a unit.
 */
static int
_shr_mres_destroy_data(_shr_res_unit_desc_t *unitData)
{
    int i;
    int result = _SHR_E_NONE;
    _shr_res_type_desc_t *type;
    _shr_res_pool_desc_t *pool;

    /* destroy resources */
    for (i = 0; i < unitData->resTypeCount; i++) {
        if (unitData->res[i]) {
            
            type = unitData->res[i];
            unitData->res[i] = NULL;
            if (type->refCount) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META("%p type %d (%s): still in use (%d)\n"),
                          (void*)unitData,
                          i,
                          &(type->name[0]),
                          type->refCount));
            }
            unitData->pool[type->resPoolId]->refCount--;
            sal_free(type);
        }
    } /* for (all resources this unit) */

    /* destroy pools */
    for (i = 0;
         (i < unitData->resPoolCount) && (_SHR_E_NONE == result);
         i++) {
        if (unitData->pool[i]) {
            
            pool = unitData->pool[i];
            unitData->pool[i] = NULL;
            if (pool->refCount) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META("%p pool %d (%s): unexpectedly still"
                          " in use (%d) - invalid condition???\n"),
                          (void*)unitData,
                          i,
                          &(pool->name[0]),
                          pool->refCount));
            }
            result = _shr_res_alloc_mgrs[pool->resManagerType].destroy(pool);
            if (_SHR_E_NONE != result) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META("%p pool %d (%s): unable to destroy:"
                           " %d (%s)\n"),
                           (void*)unitData,
                           i,
                           &(pool->name[0]),
                           result,
                           _SHR_ERRMSG(result)));
                unitData->pool[i] = pool;
            } /* if (_SHR_E_NONE != result) */
        } /* if (unitData->pool[i]) */
    } /* for (all pools as long as no errors) */
    return result;
}

/*****************************************************************************/
/*
 *  Exposed API implementation (handle based)
 */

/*
 *  Initialise unit
 */
int
shr_mres_create(shr_mres_handle_t *handle,
                int num_res_types,
                int num_res_pools)
{
    _shr_res_unit_desc_t *tempHandle;
    int result = _SHR_E_NONE;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %d) enter\n"),
               (void*)handle,
               num_res_types,
               num_res_pools));

    /* a little parameter checking */
    if (!handle) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("obligatory OUT argument must not be NULL\n")));
        result = _SHR_E_PARAM;
    }
    if (1 > num_res_pools) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("resource pools %d; must be > 0\n"),
                   num_res_pools));
        result =  _SHR_E_PARAM;
    }
    if (1 > num_res_types) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("resource types %d; must be > 0\n"),
                   num_res_types));
        result =  _SHR_E_PARAM;
    }
    if (_SHR_E_NONE != result) {
        /* displayed diagnostics above */
        return result;
    }
    /* set things up */
    tempHandle = sal_alloc(sizeof(_shr_res_unit_desc_t) +
                         (sizeof(_shr_res_pool_desc_t) * num_res_pools) +
                         (sizeof(_shr_res_type_desc_t) * num_res_types),
                         "resource descriptor");
    if (!tempHandle) {
        /* alloc failed */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to allocate %u bytes for info\n"),
                   (unsigned int)(sizeof(_shr_res_unit_desc_t) +
                   (sizeof(_shr_res_pool_desc_t) * num_res_pools) +
                   (sizeof(_shr_res_type_desc_t) * num_res_types))));
        result = _SHR_E_MEMORY;
    } else { /* if (!tempUnit) */
        /* got the unit information heap cell, set it up */
        sal_memset(tempHandle,
                   0x00,
                   sizeof(_shr_res_unit_desc_t) +
                   (sizeof(_shr_res_pool_desc_t*) * num_res_pools) +
                   (sizeof(_shr_res_type_desc_t*) * num_res_types));
        tempHandle->pool = (_shr_res_pool_desc_t**)(&(tempHandle[1]));
        tempHandle->res = (_shr_res_type_desc_t**)(&(tempHandle->pool[num_res_pools]));
        tempHandle->resTypeCount = num_res_types;
        tempHandle->resPoolCount = num_res_pools;
        *handle = tempHandle;
    } /* if (!tempUnit) */

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(&(%p), %d, %d) return %d (%s)\n"),
               (void*)(*handle),
               num_res_types,
               num_res_pools,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Get unit resource information (top level)
 */
int
shr_mres_get(shr_mres_handle_t handle,
             int *num_res_types,
             int *num_res_pools)
{
    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %p, %p) enter\n"),
               (void*)handle,
               (void*)num_res_types,
               (void*)num_res_pools));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    /* return the requested information */
    if (num_res_pools) {
        *num_res_pools = handle->resPoolCount;
    }
    if (num_res_types) {
        *num_res_types = handle->resTypeCount;
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, &(%d), &(%d)) return %d (%s)\n"),
               (void*)handle,
               num_res_types?*num_res_types:-1,
               num_res_pools?*num_res_pools:-1,
               _SHR_E_NONE,
               _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}

/*
 *  Deinitialise unit
 */
int
shr_mres_destroy(shr_mres_handle_t handle)
{
    int result = _SHR_E_NONE;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p) enter\n"),
               (void*)handle));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    /* tear things down */
    result = _shr_mres_destroy_data(handle);
    if (_SHR_E_NONE == result) {
        sal_free(handle);
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p) return %d (%s)\n"),
               (void*)handle,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Configure a resource pool on a unit
 */
int
shr_mres_pool_set(shr_mres_handle_t handle,
                  int pool_id,
                  shr_res_allocator_t manager,
                  int low_id,
                  int count,
                  const void *extras,
                  const char *name)
{
    _shr_res_pool_desc_t *tempPool;
    _shr_res_pool_desc_t *oldPool;
    int result = _SHR_E_NONE;
    int xresult;
    const char *noname = "???";

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %s, %d, %d, %p, \"%s\") enter\n"),
               (void*)handle,
               pool_id,
               ((0 <= manager) && (SHR_RES_ALLOCATOR_COUNT > manager))?_shr_res_alloc_mgrs[manager].name:"INVALID",
               low_id,
               count,
               (void*)extras,
               name?name:noname));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_POOL_VALID_CHECK(handle, pool_id);
    if ((0 > manager) || (SHR_RES_ALLOCATOR_COUNT <= manager)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("allocation manager type %d not supported\n"),
                   manager));
        return _SHR_E_PARAM;
    }
    if (0 > count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("negative counts are not permitted\n")));
        return _SHR_E_PARAM;
    }
    if ((handle->pool[pool_id]) && (handle->pool[pool_id]->refCount)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("%p pool %d (%s) can not be changed because it"
                   " has %d types that use it\n"),
                   (void*)handle,
                   pool_id,
                   handle->pool[pool_id]->name,
                   handle->pool[pool_id]->refCount));
        return _SHR_E_CONFIG;
    }
    
    oldPool = handle->pool[pool_id];
    handle->pool[pool_id] = NULL;
    /* create the new pool */
    result = _shr_res_alloc_mgrs[manager].create(&tempPool,
                                                 low_id,
                                                 count,
                                                 extras,
                                                 name?name:noname);
    if (_SHR_E_NONE == result) {
        /* new one created successfully */
        tempPool->resManagerType = manager;
        tempPool->refCount = 0;
        if (oldPool) {
            /* old one exists; get rid of it */
            result = _shr_res_alloc_mgrs[oldPool->resManagerType].destroy(oldPool);
            if (_SHR_E_NONE != result) {
                handle->pool[pool_id] = oldPool;
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META("unable to destroy %p old pool %d (%s):"
                           " %d (%s)\n"),
                           (void*)handle,
                           pool_id,
                           oldPool->name,
                           result,
                           _SHR_ERRMSG(result)));
                xresult = _shr_res_alloc_mgrs[tempPool->resManagerType].destroy(tempPool);
                if (_SHR_E_NONE != xresult) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META("unable to destroy new pool for %p pool"
                               " %d after replace error: %d (%s)\n"),
                               (void*)handle,
                               pool_id,
                               xresult,
                               _SHR_ERRMSG(xresult)));
                }
            } /* if (_SHR_E_NONE != result) */
        } /* if (oldPool) */
    } /* if (_SHR_E_NONE == result) */
    if (_SHR_E_NONE == result) {
        handle->pool[pool_id] = tempPool;
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %s, %d, %d, %p, \"%s\") return %d (%s)\n"),
               (void*)handle,
               pool_id,
               _shr_res_alloc_mgrs[manager].name,
               low_id,
               count,
               (void*)extras,
               name?name:noname,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Destroy a resource pool on a unit
 */
int
shr_mres_pool_unset(shr_mres_handle_t handle,
                    int pool_id)
{
    _shr_res_pool_desc_t *oldPool;
    int result = _SHR_E_NONE;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d) enter\n"),
               (void*)handle, pool_id));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_POOL_VALID_CHECK(handle, pool_id);
    
    oldPool = handle->pool[pool_id];
    handle->pool[pool_id] = NULL;
    if (oldPool) {
        if (oldPool->refCount) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("%p pool %d (%s) can not be destroyed because"
                       " it has %d types that use it\n"),
                       (void*)handle,
                       pool_id,
                       oldPool->name,
                       oldPool->refCount));
            result = _SHR_E_CONFIG;
        } else { /* if (oldPool->refCount) */
            result = _shr_res_alloc_mgrs[oldPool->resManagerType].destroy(oldPool);
            if (_SHR_E_NONE != result) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META("unable to destroy %p old pool %d (%s):"
                           " %d (%s)\n"),
                           (void*)handle,
                           pool_id,
                           oldPool->name,
                           result,
                           _SHR_ERRMSG(result)));
            } /* if (_SHR_E_NONE != result) */
        } /* if (oldPool->refCount) */
    } /* if (oldPool) */
    if (_SHR_E_NONE != result) {
        handle->pool[pool_id] = oldPool;
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d) return %d (%s)\n"),
               (void*)handle,
               pool_id,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Get information about a resource pool on a unit
 */
int
shr_mres_pool_get(shr_mres_handle_t handle,
                  int pool_id,
                  shr_res_allocator_t *manager,
                  int *low_id,
                  int *count,
                  const void **extras,
                  const char **name)
{
    _shr_res_pool_desc_t *thisPool;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %p, %p, %p, %p, %p) enter\n"),
               (void*)handle,
               pool_id,
               (void*)manager,
               (void*)low_id,
               (void*)count,
               (void*)extras,
               (void*)name));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_POOL_VALID_CHECK(handle, pool_id);
    RES_POOL_EXIST_CHECK(handle, pool_id);
    /* fill in the caller's request */
    thisPool = handle->pool[pool_id];
    if (manager) {
        *manager = thisPool->resManagerType;
    }
    if (low_id) {
        *low_id = thisPool->low;
    }
    if (count) {
        *count = thisPool->count;
    }
    if (extras) {
        *extras = thisPool->extras;
    }
    if (name) {
        *name = thisPool->name;
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, &(%s), &(%d), &(%d), &(%p), &(\"%s\")) return %d (%s)\n"),
               (void*)handle,
               pool_id,
               manager?_shr_res_alloc_mgrs[*manager].name:"NULL",
               low_id?*low_id:0,
               count?*count:0,
               extras?(void*)(*extras):NULL,
               name?*name:"NULL",
               _SHR_E_NONE,
               _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}

/*
 *  Get certain details about a pool on a unit
 */
int
shr_mres_pool_info_get(shr_mres_handle_t handle,
                       int pool_id,
                       shr_res_pool_info_t *info)
{
    _shr_res_pool_desc_t *thisPool;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %p) enter\n"),
               (void*)handle,
               pool_id,
               (void*)info));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_POOL_VALID_CHECK(handle, pool_id);
    RES_POOL_EXIST_CHECK(handle, pool_id);
    /* fill in the caller's request */
    thisPool = handle->pool[pool_id];
    if (info) {
        info->free = thisPool->count - thisPool->inuse;
        info->used = thisPool->inuse;
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %p) return %d (%s)\n"),
               (void*)handle,
               pool_id,
               (void*)info,
               _SHR_E_NONE,
               _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}

/*
 *  Configure a resource type on a unit
 */
int
shr_mres_type_set(shr_mres_handle_t handle,
                  int res_id,
                  int pool_id,
                  int elem_size,
                  const char *name)
{
    _shr_res_type_desc_t *tempType;
    _shr_res_type_desc_t *oldType;
    int result = _SHR_E_NONE;
    const char *noname = "???";
    int len_name = 0;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %d, %d, \"%s\") enter\n"),
               (void*)handle,
               res_id,
               pool_id,
               elem_size,
               name?name:noname));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_POOL_VALID_CHECK(handle, pool_id);
    RES_POOL_EXIST_CHECK(handle, pool_id);
    RES_TYPE_VALID_CHECK(handle, res_id);
    if (1 > elem_size) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("element size %d too small; must be >= 1\n"),
                   elem_size));
        return _SHR_E_PARAM;
    }
    if ((handle->res[res_id]) && (handle->res[res_id]->refCount)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("%p resource %d (%s) can not be changed"
                   " because it has %d elements in use\n"),
                   (void*)handle,
                   res_id,
                   handle->res[res_id]->name,
                   handle->res[res_id]->refCount));
        return _SHR_E_CONFIG;
    }
    if (!name) {
        /* force a non-NULL name pointer */
        name = noname;
    }

    len_name = sal_strlen(name);
    
    oldType = handle->res[res_id];
    handle->res[res_id] = NULL;
    /* allocate new type descriptor */
    /* note base type includes one character, so don't need to add NUL here */
    tempType = sal_alloc(sizeof(*tempType) + len_name,
                         "resource type descriptor");
    if (tempType) {
        /* got the needed memory; set it up */
        sal_memset(tempType,
                   0x00,
                   sizeof(*tempType) + len_name);
        tempType->resElemSize = elem_size;
        tempType->resPoolId = pool_id;
        sal_strncpy(&(tempType->name[0]), name, len_name);
        if (len_name)
            *((char*)&(tempType->name[0])+len_name) = '\0';

        if (oldType) {
            /* there was an old one; get rid of it and adjust references */
            handle->pool[oldType->resPoolId]->refCount--;
            sal_free(oldType);
        }
        /* adjust references and put this type in place */
        handle->pool[pool_id]->refCount++;
        handle->res[res_id] = tempType;
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to allocate %u bytes for %p resource"
                   " type %d\n"),
                   (unsigned int)(sizeof(*tempType) + sal_strlen(name)),
                   (void*)handle,
                   res_id));
        result = _SHR_E_MEMORY;
        /* restore the old type */
        handle->res[res_id] = oldType;
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %d, %d, \"%s\") return %d (%s)\n"),
               (void*)handle,
               res_id,
               pool_id,
               elem_size,
               name?name:noname,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Destroy a resource type on a unit
 */
int
shr_mres_type_unset(shr_mres_handle_t handle,
                    int res_id)
{
    _shr_res_type_desc_t *oldType;
    int result = _SHR_E_NONE;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d) enter\n"),
               (void*)handle, res_id));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_TYPE_VALID_CHECK(handle, res_id);
    
    oldType = handle->res[res_id];
    handle->res[res_id] = NULL;
    if (oldType) {
        if (oldType->refCount) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("%p resource %d (%s) can not be destroyed"
                       " because it has %d elements in use\n"),
                       (void*)handle,
                       res_id,
                       oldType->name,
                       oldType->refCount));
            result = _SHR_E_CONFIG;
        } else {
            handle->pool[oldType->resPoolId]->refCount--;
            sal_free(oldType);
        }
    } /* if (oldType) */
    if (_SHR_E_NONE != result) {
        handle->res[res_id] = oldType;
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d) return %d (%s)\n"),
               (void*)handle,
               res_id,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Get information about a resource type
 */
int
shr_mres_type_get(shr_mres_handle_t handle,
                  int res_id,
                  int *pool_id,
                  int *elem_size,
                  const char **name)
{
    _shr_res_type_desc_t *thisType;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %p, %p, %p) enter\n"),
               (void*)handle,
               res_id,
               (void*)pool_id,
               (void*)elem_size,
               (void*)name));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_TYPE_VALID_CHECK(handle, res_id);
    RES_TYPE_EXIST_CHECK(handle, res_id);
    /* fill in the caller's request */
    thisType = handle->res[res_id];
    if (pool_id) {
        *pool_id = thisType->resPoolId;
    }
    if (elem_size) {
        *elem_size = thisType->resElemSize;
    }
    if (name) {
        *name = thisType->name;
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, &(%d), &(%d), &(\"%s\")) return %d (%s)\n"),
               (void*)handle,
               res_id,
               pool_id?*pool_id:0,
               elem_size?*elem_size:0,
               name?*name:"NULL",
               _SHR_E_NONE,
               _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}

/*
 *  Get information about a resource type
 */
int
shr_mres_type_info_get(shr_mres_handle_t handle,
                       int res_id,
                       shr_res_type_info_t *info)
{
    _shr_res_type_desc_t *thisType;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %p) enter\n"),
               (void*)handle,
               res_id,
               (void*)info));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_TYPE_VALID_CHECK(handle, res_id);
    RES_TYPE_EXIST_CHECK(handle, res_id);
    /* fill in the caller's request */
    thisType = handle->res[res_id];
    if (info) {
        info->used = thisType->refCount;
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %p) return %d (%s)\n"),
               (void*)handle,
               res_id,
               (void*)info,
               _SHR_E_NONE,
               _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}

/*
 *  Allocate elements of a resource type
 */
int
shr_mres_alloc(shr_mres_handle_t handle,
               int res_id,
               uint32 flags,
               int count,
               int *elem)
{
    _shr_res_pool_desc_t *thisPool;
    int result = _SHR_E_NONE;
    int scaled;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, %p) enter\n"),
               (void*)handle,
               res_id,
               flags,
               count,
               (void*)elem));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_TYPE_VALID_CHECK(handle, res_id);
    RES_TYPE_EXIST_CHECK(handle, res_id);
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("element count %d must be > 0\n"),
                   count));
        return _SHR_E_PARAM;
    }
    if (flags & (~SHR_RES_ALLOC_SINGLE_FLAGS)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("invalid flags %08X\n"),
                   flags & (~SHR_RES_ALLOC_SINGLE_FLAGS)));
        return _SHR_E_PARAM;
    }
    if (!elem) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("obligatory argument is NULL\n")));
        return _SHR_E_PARAM;
    }
    /* adjust element count per scaling factor */
    scaled = count * handle->res[res_id]->resElemSize;
    
    /* get the pool information */
    thisPool = handle->pool[handle->res[res_id]->resPoolId];
    /* make the call */
    result = _shr_res_alloc_mgrs[thisPool->resManagerType].alloc(thisPool,
                                                                 flags,
                                                                 scaled,
                                                                 elem);
    if (_SHR_E_NONE == result) {
        /* account for successful allocation */
        if (0 == (flags & SHR_RES_ALLOC_REPLACE)) {
            /* only account for alloc if new allocation */
            handle->res[res_id]->refCount += count;
            thisPool->inuse += scaled;
        }
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, &(%d)) return %d (%s)\n"),
               (void*)handle,
               res_id,
               flags,
               count,
               *elem,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Allocate a bunch of elements or blocks of a resource type
 */
int
shr_mres_alloc_group(shr_mres_handle_t handle,
                     int res_id,
                     uint32 grp_flags,
                     int grp_size,
                     int *grp_done,
                     const uint32 *flags,
                     const int *count,
                     int *elem)
{
    _shr_res_pool_desc_t *thisPool;
    _shr_res_type_desc_t *thisRes;
    int result = _SHR_E_NONE;
    int xresult;
    int scaled = 0;
    int index;
    uint32 blkFlags;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, %p, %p, %p, %p) enter\n"),
               (void*)handle,
               res_id,
               grp_flags,
               grp_size,
               (void*)grp_done,
               (void*)flags,
               (void*)count,
               (void*)elem));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_TYPE_VALID_CHECK(handle, res_id);
    RES_TYPE_EXIST_CHECK(handle, res_id);
    if (!grp_done) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("obligatory out argument grp_done is NULL\n")));
        return _SHR_E_PARAM;
    }
    *grp_done = 0;
    if (0 > grp_size) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("group member count %d must be >= 0\n"),
                   grp_size));
        return _SHR_E_PARAM;
    }
    if ((0 < grp_size) && (!flags || !count || !elem)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("an obligatory array pointer is NULL\n")));
        return _SHR_E_PARAM;
    }
    if (grp_flags & (~(SHR_RES_ALLOC_SINGLE_FLAGS | SHR_RES_ALLOC_GROUP_FLAGS))) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("invalid group flags %08X\n"),
                   grp_flags & (~(SHR_RES_ALLOC_SINGLE_FLAGS |
                   SHR_RES_ALLOC_GROUP_FLAGS))));
        return _SHR_E_PARAM;
    }
    /* get the resource information */
    thisRes = handle->res[res_id];
    /* get the pool information */
    thisPool = handle->pool[thisRes->resPoolId];
    /* try to collect the requested blocks */
    for (index = 0;
         (_SHR_E_NONE == result) && (index < grp_size);
         index++) {
        /* check parameters for this block */
        blkFlags = flags[index] | (grp_flags & SHR_RES_ALLOC_SINGLE_FLAGS);
        if (blkFlags & (~SHR_RES_ALLOC_SINGLE_FLAGS)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("invalid flags %08X for block %d\n"),
                       blkFlags & (~SHR_RES_ALLOC_SINGLE_FLAGS),
                       index));
            result = _SHR_E_PARAM;
        }
        if (0 >= count[index]) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("element count %d must be > 0\n"),
                       count[index]));
            result = _SHR_E_PARAM;
        }
        if (_SHR_E_NONE == result) {
            /* adjust element count per scaling factor */
            scaled = count[index] * thisRes->resElemSize;
            
            /* make the call */
            result = _shr_res_alloc_mgrs[thisPool->resManagerType].alloc(thisPool,
                                                                         blkFlags,
                                                                         scaled,
                                                                         &(elem[index]));
        }
        if (_SHR_E_NONE == result) {
            /* account for successful allocation */
            if (0 == (blkFlags & SHR_RES_ALLOC_REPLACE)) {
                /* only account for alloc if new allocation */
                handle->res[res_id]->refCount += count[index];
                thisPool->inuse += scaled;
            }
        } else {
            /* we'll hope the allocation manager displayed an error */
            /* don't want postincrement if an error occurred */
            break;
        }
    } /* for (all blocks in the caller's group) */
    if ((_SHR_E_NONE != result) && (grp_flags & SHR_RES_ALLOC_GROUP_ATOMIC)) {
        /* atomic mode and it failed; back out everything that we have */
        /* index is at the first failure */
        while (index > 0) {
            /* look at previous index (must have been successful) */
            index--;
            blkFlags = flags[index] | (grp_flags & SHR_RES_ALLOC_SINGLE_FLAGS);
            if (0 == (blkFlags & SHR_RES_ALLOC_REPLACE)) {
                /* free only blocks that were not trying to be replaced */
                /* adjust element count per scaling factor */
                scaled = count[index] * thisRes->resElemSize;
                /* free this element or block */
                xresult = _shr_res_alloc_mgrs[thisPool->resManagerType].free(thisPool,
                                                                             scaled,
                                                                             elem[index]);
                if (_SHR_E_NONE != xresult) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META("unable to back out %p resource %d"
                               " index %d base %d count %d: %d (%s)\n"),
                               (void*)handle,
                               res_id,
                               index,
                               elem[index],
                               count[index],
                               result,
                               _SHR_ERRMSG(result)));
                } else {
                    thisRes->refCount -= count[index];
                    thisPool->inuse -= scaled;
                }
            } /* if (0 == (blkFlags & SHR_RES_ALLOC_REPLACE)) */
        } /* while (index > 0) */
    } /* if (error && SHR_RES_ALLOC_GROUP_ATOMIC was set) */
    /* update number of allocations that succeeded */
    *grp_done = index;

    /* return the result */
    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, &(%d), %p, %p, %p) enter\n"),
               (void*)handle,
               res_id,
               grp_flags,
               grp_size,
               *grp_done,
               (void*)flags,
               (void*)count,
               (void*)elem));
    for (index = 0; index < grp_size; index++) {
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("  block %12d: %08X %12d %12d\n"),
                   index,
                   flags[index],
                   count[index],
                   elem[index]));
    }
    return result;
}

/*
 *  Allocate elements of a resource type (tagged)
 */
int
shr_mres_alloc_tag(shr_mres_handle_t handle,
                   int res_id,
                   uint32 flags,
                   const void *tag,
                   int count,
                   int *elem)
{
    _shr_res_pool_desc_t *thisPool;
    int result = _SHR_E_NONE;
    int scaled;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %p, %d, %p) enter\n"),
               (void*)handle,
               res_id,
               flags,
               (void*)tag,
               count,
               (void*)elem));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_TYPE_VALID_CHECK(handle, res_id);
    RES_TYPE_EXIST_CHECK(handle, res_id);
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("element count %d must be > 0\n"),
                   count));
        return _SHR_E_PARAM;
    }
    if (flags & (~SHR_RES_ALLOC_SINGLE_FLAGS)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("invalid flags %08X\n"),
                   flags & (~SHR_RES_ALLOC_SINGLE_FLAGS)));
        return _SHR_E_PARAM;
    }
    if (!elem) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("obligatory argument is NULL\n")));
        return _SHR_E_PARAM;
    }
    /* adjust element count per scaling factor */
    scaled = count * handle->res[res_id]->resElemSize;
    
    /* get the pool information */
    thisPool = handle->pool[handle->res[res_id]->resPoolId];
    /* make the call */
    if (_shr_res_alloc_mgrs[thisPool->resManagerType].tag) {
        /* allocator supports it; make the call */
        result = _shr_res_alloc_mgrs[thisPool->resManagerType].tag(thisPool,
                                                                   flags,
                                                                   tag,
                                                                   scaled,
                                                                   elem);
    } else {
        /* not supported by this allocator */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("allocator type %s does not support tagged alloc\n"),
                   _shr_res_alloc_mgrs[thisPool->resManagerType].name));
        result = _SHR_E_UNAVAIL;
    }
    if (_SHR_E_NONE == result) {
        /* account for successful allocation */
        if (0 == (flags & SHR_RES_ALLOC_REPLACE)) {
            /* only account for alloc if new allocation */
            handle->res[res_id]->refCount += count;
            thisPool->inuse += scaled;
        }
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %p, %d, &(%d)) return %d (%s)\n"),
               (void*)handle,
               res_id,
               flags,
               (void*)tag,
               count,
               *elem,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Allocate a bunch of elements or blocks of a resource type (tagged)
 */
int
shr_mres_alloc_tag_group(shr_mres_handle_t handle,
                         int res_id,
                         uint32 grp_flags,
                         int grp_size,
                         int *grp_done,
                         const uint32 *flags,
                         const void **tag,
                         const int *count,
                         int *elem)
{
    _shr_res_pool_desc_t *thisPool;
    _shr_res_type_desc_t *thisRes;
    int result = _SHR_E_NONE;
    int xresult;
    int scaled = 0;
    int index;
    uint32 blkFlags;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, %p, %p, %p, %p, %p) enter\n"),
               (void*)handle,
               res_id,
               grp_flags,
               grp_size,
               (void*)grp_done,
               (void*)flags,
               (void*)tag,
               (void*)count,
               (void*)elem));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_TYPE_VALID_CHECK(handle, res_id);
    RES_TYPE_EXIST_CHECK(handle, res_id);
    if (!grp_done) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("obligatory out argument grp_done is NULL\n")));
        return _SHR_E_PARAM;
    }
    *grp_done = 0;
    if (0 > grp_size) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("group member count %d must be >= 0\n"),
                   grp_size));
        return _SHR_E_PARAM;
    }
    if ((0 < grp_size) && (!flags || !count || !elem)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("an obligatory array pointer is NULL\n")));
        return _SHR_E_PARAM;
    }
    if (grp_flags & (~(SHR_RES_ALLOC_SINGLE_FLAGS | SHR_RES_ALLOC_GROUP_FLAGS))) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("invalid group flags %08X\n"),
                   grp_flags & (~(SHR_RES_ALLOC_SINGLE_FLAGS |
                   SHR_RES_ALLOC_GROUP_FLAGS))));
        return _SHR_E_PARAM;
    }
    /* get the resource information */
    thisRes = handle->res[res_id];
    /* get the pool information */
    thisPool = handle->pool[thisRes->resPoolId];
    /* try to collect the requested blocks */
    if (!_shr_res_alloc_mgrs[thisPool->resManagerType].tag) {
        /* not supported by this allocator */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("allocator type %s does not support tagged alloc\n"),
                   _shr_res_alloc_mgrs[thisPool->resManagerType].name));
        return _SHR_E_UNAVAIL;
    }
    for (index = 0;
         (_SHR_E_NONE == result) && (index < grp_size);
         index++) {
        /* check parameters for this block */
        blkFlags = flags[index] | (grp_flags & SHR_RES_ALLOC_SINGLE_FLAGS);
        if (blkFlags & (~SHR_RES_ALLOC_SINGLE_FLAGS)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("invalid flags %08X for block %d\n"),
                       blkFlags & (~SHR_RES_ALLOC_SINGLE_FLAGS),
                       index));
            result = _SHR_E_PARAM;
        }
        if (0 >= count[index]) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("element count %d must be > 0\n"),
                       count[index]));
            result = _SHR_E_PARAM;
        }
        if (_SHR_E_NONE == result) {
            /* adjust element count per scaling factor */
            scaled = count[index] * thisRes->resElemSize;
            
            /* make the call */
            result = _shr_res_alloc_mgrs[thisPool->resManagerType].tag(thisPool,
                                                                       blkFlags,
                                                                       tag[index],
                                                                       scaled,
                                                                       &(elem[index]));
        }
        if (_SHR_E_NONE == result) {
            /* account for successful allocation */
            if (0 == (blkFlags & SHR_RES_ALLOC_REPLACE)) {
                /* only account for alloc if new allocation */
                handle->res[res_id]->refCount += count[index];
                thisPool->inuse += scaled;
            }
        } else {
            /* we'll hope the allocation manager displayed an error */
            /* don't want postincrement if an error occurred */
            break;
        }
    } /* for (all blocks in the caller's group) */
    if ((_SHR_E_NONE != result) && (grp_flags & SHR_RES_ALLOC_GROUP_ATOMIC)) {
        /* atomic mode and it failed; back out everything that we have */
        /* index is at the first failure */
        while (index > 0) {
            /* look at previous index (must have been successful) */
            index--;
            blkFlags = flags[index] | (grp_flags & SHR_RES_ALLOC_SINGLE_FLAGS);
            if (0 == (blkFlags & SHR_RES_ALLOC_REPLACE)) {
                /* only free blocks that were not trying to be replaced */
                /* adjust element count per scaling factor */
                scaled = count[index] * thisRes->resElemSize;
                /* free this element or block */
                xresult = _shr_res_alloc_mgrs[thisPool->resManagerType].free(thisPool,
                                                                             scaled,
                                                                             elem[index]);
                if (_SHR_E_NONE != xresult) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META("unable to back out %p resource %d"
                               " index %d base %d count %d: %d (%s)\n"),
                               (void*)handle,
                               res_id,
                               index,
                               elem[index],
                               count[index],
                               result,
                               _SHR_ERRMSG(result)));
                } else {
                    thisRes->refCount -= count[index];
                    thisPool->inuse -= scaled;
                }
            } /* if (0 == (blkFlags & SHR_RES_ALLOC_REPLACE)) */
        } /* while (index > 0) */
    } /* if (error && SHR_RES_ALLOC_GROUP_ATOMIC was set) */
    /* update number of allocations that succeeded */
    *grp_done = index;

    /* return the result */
    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, &(%d), %p, %p, %p, %p) enter\n"),
               (void*)handle,
               res_id,
               grp_flags,
               grp_size,
               *grp_done,
               (void*)flags,
               (void*)tag,
               (void*)count,
               (void*)elem));
    for (index = 0; index < grp_size; index++) {
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("  block %12d: %08X %12d %12d\n"),
                   index,
                   flags[index],
                   count[index],
                   elem[index]));
    }
    return result;
}

/*
 *  Allocate a block of elements with the requested alignment and offset.
 */
int
shr_mres_alloc_align(shr_mres_handle_t handle,
                     int res_id,
                     uint32 flags,
                     int align,
                     int offset,
                     int count,
                     int *elem)
{
    _shr_res_pool_desc_t *thisPool;
    _shr_res_type_desc_t *thisType;
    int result = _SHR_E_NONE;
    int base;
    int scaled;
    int scaledAlign;
    int scaledOffset;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, %d, %d, %p) enter\n"),
               (void*)handle,
               res_id,
               flags,
               align,
               offset,
               count,
               (void*)elem));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_TYPE_VALID_CHECK(handle, res_id);
    RES_TYPE_EXIST_CHECK(handle, res_id);
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("element count %d must be > 0\n"),
                   count));
        return _SHR_E_PARAM;
    }
    if (1 > align) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META("align <= 0 invalid, using align = 1 instead\n")));
        align = 1;
    }
    if ((offset >= align) || (0 > offset)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("offset %d must be >= 0 and < align %d\n"),
                   offset,
                   align));
        return _SHR_E_PARAM;
    }
    if (flags & (~SHR_RES_ALLOC_SINGLE_FLAGS)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("invalid flags %08X\n"),
                   flags & (~SHR_RES_ALLOC_SINGLE_FLAGS)));
        return _SHR_E_PARAM;
    }
    if (!elem) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("obligatory argument is NULL\n")));
        return _SHR_E_PARAM;
    }
    /* get the pool information */
    thisType = handle->res[res_id];
    thisPool = handle->pool[thisType->resPoolId];
    /* adjust element count per scaling factor */
    scaled = count * thisType->resElemSize;
    scaledAlign = align * thisType->resElemSize;
    scaledOffset = offset * thisType->resElemSize;
    
    /* check alignment for WITH_ID case */
    if (flags & SHR_RES_ALLOC_WITH_ID) {
        if (flags & SHR_RES_ALLOC_ALIGN_ZERO) {
            base = *elem;
        } else {
            base = *elem - thisPool->low;
        }
        if (((((base) / scaledAlign) * scaledAlign) + scaledOffset) != base) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("WITH_ID requested element %d does not comply"
                       " with alignment specifications\n"),
                       *elem));
            return _SHR_E_PARAM;
        }
    }
    if (_shr_res_alloc_mgrs[thisPool->resManagerType].align) {
        /* allocator supports it; make the call */
        result = _shr_res_alloc_mgrs[thisPool->resManagerType].align(thisPool,
                                                                     flags,
                                                                     scaledAlign,
                                                                     scaledOffset,
                                                                     scaled,
                                                                     elem);
    } else {
        /* not supported by this allocator */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("allocator type %s does not support aligned alloc\n"),
                   _shr_res_alloc_mgrs[thisPool->resManagerType].name));
        result = _SHR_E_UNAVAIL;
    }
    if (_SHR_E_NONE == result) {
        /* account for successful allocation */
        if (0 == (flags & SHR_RES_ALLOC_REPLACE)) {
            /* only account for alloc if new allocation */
            handle->res[res_id]->refCount += count;
            thisPool->inuse += scaled;
        }
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, %d, %d, &(%d)) return %d (%s)\n"),
               (void*)handle,
               res_id,
               flags,
               align,
               offset,
               count,
               *elem,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Allocate a bunch of aligned elements or blocks of a resource type
 */
int
shr_mres_alloc_align_group(shr_mres_handle_t handle,
                           int res_id,
                           uint32 grp_flags,
                           int grp_size,
                           int *grp_done,
                           const uint32 *flags,
                           const int *align,
                           const int *offset,
                           const int *count,
                           int *elem)
{
    _shr_res_pool_desc_t *thisPool;
    _shr_res_type_desc_t *thisRes;
    int result = _SHR_E_NONE;
    int xresult;
    int scaled = 0;
    int scaledAlign;
    int scaledOffset;
    int index;
    int xalign;
    int base;
    uint32 blkFlags;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, %p, %p, %p, %p, %p, %p) enter\n"),
               (void*)handle,
               res_id,
               grp_flags,
               grp_size,
               (void*)grp_done,
               (void*)flags,
               (void*)align,
               (void*)offset,
               (void*)count,
               (void*)elem));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_TYPE_VALID_CHECK(handle, res_id);
    RES_TYPE_EXIST_CHECK(handle, res_id);
    if (!grp_done) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("obligatory out argument grp_done is NULL\n")));
        return _SHR_E_PARAM;
    }
    *grp_done = 0;
    if (0 > grp_size) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("group member count %d must be >= 0\n"),
                   grp_size));
        return _SHR_E_PARAM;
    }
    if ((0 < grp_size) && (!flags || !count || !elem || !align || !offset)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("an obligatory array pointer is NULL\n")));
        return _SHR_E_PARAM;
    }
    if (grp_flags & (~(SHR_RES_ALLOC_SINGLE_FLAGS | SHR_RES_ALLOC_GROUP_FLAGS))) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("invalid group flags %08X\n"),
                   grp_flags & (~(SHR_RES_ALLOC_SINGLE_FLAGS |
                   SHR_RES_ALLOC_GROUP_FLAGS))));
        return _SHR_E_PARAM;
    }
    /* get the resource information */
    thisRes = handle->res[res_id];
    /* get the pool information */
    thisPool = handle->pool[thisRes->resPoolId];
    /* try to collect the requested blocks */
    if (_shr_res_alloc_mgrs[thisPool->resManagerType].align) {
        /* allocator does not support this feature */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("allocator type %s does not support aligned alloc\n"),
                   _shr_res_alloc_mgrs[thisPool->resManagerType].name));
        result = _SHR_E_UNAVAIL;
    }
    for (index = 0;
         (_SHR_E_NONE == result) && (index < grp_size);
         index++) {
        /* check parameters for this block */
        blkFlags = flags[index] | (grp_flags & SHR_RES_ALLOC_SINGLE_FLAGS);
        if (blkFlags & (~SHR_RES_ALLOC_SINGLE_FLAGS)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("invalid flags %08X for block %d\n"),
                       blkFlags & (~SHR_RES_ALLOC_SINGLE_FLAGS),
                       index));
            result = _SHR_E_PARAM;
        }
        if (0 >= count[index]) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("element count %d must be > 0\n"),
                       count[index]));
            result = _SHR_E_PARAM;
        }
        if (1 > align[index]) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META("align <= 0 invalid, using align = 1 instead\n")));
            xalign = 1;
        } else {
            xalign = align[index];
        }
        if ((offset[index] >= xalign) || (0 > offset[index])) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("offset %d must be >= 0 and < align %d\n"),
                       offset[index],
                       xalign));
            result = _SHR_E_PARAM;
        }
        if (_SHR_E_NONE == result) {
            /* adjust element count per scaling factor */
            scaled = count[index] * thisRes->resElemSize;
            scaledAlign = xalign * thisRes->resElemSize;
            scaledOffset = offset[index] * thisRes->resElemSize;
            
            /* check alignment for WITH_ID case */
            if (blkFlags & SHR_RES_ALLOC_WITH_ID) {
                if (blkFlags & SHR_RES_ALLOC_ALIGN_ZERO) {
                    base = elem[index];
                } else {
                    base = elem[index] - thisPool->low;
                }
                if (((((base) / scaledAlign) * scaledAlign) + scaledOffset) != base) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META("WITH_ID requested element %d does not"
                               " comply with alignment specifications\n"),
                               elem[index]));
                    result = _SHR_E_PARAM;
                }
            }
            if (_SHR_E_NONE == result) {
                result = _shr_res_alloc_mgrs[thisPool->resManagerType].align(thisPool,
                                                                             blkFlags,
                                                                             scaledAlign,
                                                                             scaledOffset,
                                                                             scaled,
                                                                             &(elem[index]));
            }
        }
        if (_SHR_E_NONE == result) {
            /* account for successful allocation */
            if (0 == (blkFlags & SHR_RES_ALLOC_REPLACE)) {
                /* only account for alloc if new allocation */
                handle->res[res_id]->refCount += count[index];
                thisPool->inuse += scaled;
            }
        } else {
            /* we'll hope the allocation manager displayed an error */
            /* don't want postincrement if an error occurred */
            break;
        }
    } /* for (all blocks in the caller's group) */
    if ((_SHR_E_NONE != result) && (grp_flags & SHR_RES_ALLOC_GROUP_ATOMIC)) {
        /* atomic mode and it failed; back out everything that we have */
        /* index is at the first failure */
        while (index > 0) {
            /* look at previous index (must have been successful) */
            index--;
            blkFlags = flags[index] | (grp_flags & SHR_RES_ALLOC_SINGLE_FLAGS);
            if (0 == (blkFlags & SHR_RES_ALLOC_REPLACE)) {
                /* only free blocks that were not trying to be replaced */
                /* adjust element count per scaling factor */
                scaled = count[index] * thisRes->resElemSize;
                /* free this element or block */
                xresult = _shr_res_alloc_mgrs[thisPool->resManagerType].free(thisPool,
                                                                             scaled,
                                                                             elem[index]);
                if (_SHR_E_NONE != xresult) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META("unable to back out %p resource %d"
                               " index %d base %d count %d: %d (%s)\n"),
                               (void*)handle,
                               res_id,
                               index,
                               elem[index],
                               count[index],
                               result,
                               _SHR_ERRMSG(result)));
                } else {
                    thisRes->refCount -= count[index];
                    thisPool->inuse -= scaled;
                }
            } /* if (0 == (blkFlags & SHR_RES_ALLOC_REPLACE)) */
        } /* while (index > 0) */
    } /* if (error && SHR_RES_ALLOC_GROUP_ATOMIC was set) */
    /* update number of allocations that succeeded */
    *grp_done = index;

    /* return the result */
    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, &(%d), %p, %p, %p, %p, %p)"
               " return %d (%s)\n"),
               (void*)handle,
               res_id,
               grp_flags,
               grp_size,
               *grp_done,
               (void*)flags,
               (void*)align,
               (void*)offset,
               (void*)count,
               (void*)elem,
               result,
               _SHR_ERRMSG(result)));
    for (index = 0; index < grp_size; index++) {
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("  block %12d: %08X %12d %12d %12d %12d\n"),
                   index,
                   flags[index],
                   align[index],
                   offset[index],
                   count[index],
                   elem[index]));
    }
    return result;
}

/*
 *  Allocate a block of elements with the requested alignment and offset.
 */
int
shr_mres_alloc_align_sparse(shr_mres_handle_t handle,
                            int res_id,
                            uint32 flags,
                            int align,
                            int offset,
                            uint32 pattern,
                            int length,
                            int repeats,
                            int *elem)
{
    _shr_res_pool_desc_t *thisPool;
    _shr_res_type_desc_t *thisType;
    int result = _SHR_E_NONE;
    int count;
    int index;
    int base;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, %d, %08X, %d, %d, %p) enter\n"),
               (void*)handle,
               res_id,
               flags,
               align,
               offset,
               pattern,
               length,
               repeats,
               (void*)elem));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_TYPE_VALID_CHECK(handle, res_id);
    RES_TYPE_EXIST_CHECK(handle, res_id);
    if (0 >= length) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("pattern length must be greater than zero\n")));
        return _SHR_E_PARAM;
    }
    if (32 < length) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("pattern length must be 32 or less\n")));
        return _SHR_E_PARAM;
    }
    if (0 >= repeats) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("repeat count must be greater than zero\n")));
        return _SHR_E_PARAM;
    }
    if (1 > align) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META("align <= 0 invalid, using align = 1 instead\n")));
        align = 1;
    }
    if ((offset >= align) || (0 > offset)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("offset %d must be >= 0 and < align %d\n"),
                   offset,
                   align));
        return _SHR_E_PARAM;
    }
    if (flags & (~SHR_RES_ALLOC_SINGLE_FLAGS)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("invalid flags %08X\n"),
                   flags & (~SHR_RES_ALLOC_SINGLE_FLAGS)));
        return _SHR_E_PARAM;
    }
    if (!elem) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("obligatory argument is NULL\n")));
        return _SHR_E_PARAM;
    }
    /* get the pool information */
    thisType = handle->res[res_id];
    thisPool = handle->pool[thisType->resPoolId];
    if (1 != thisType->resElemSize) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("not compatible with scaled resources\n")));
        return _SHR_E_CONFIG;
    }
    /* check alignment for WITH_ID case */
    if (flags & SHR_RES_ALLOC_WITH_ID) {
        if (flags & SHR_RES_ALLOC_ALIGN_ZERO) {
            base = *elem;
        } else {
            base = *elem - thisPool->low;
        }
        if (((((base) / align) * align) + offset) != base) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("WITH_ID requested element %d does not comply"
                       " with alignment specifications\n"),
                       *elem));
            return _SHR_E_PARAM;
        }
    }
    if (_shr_res_alloc_mgrs[thisPool->resManagerType].align_sparse) {
        /* allocator supports it; make the call */
        result = _shr_res_alloc_mgrs[thisPool->resManagerType].align_sparse(thisPool,
                                                                            flags,
                                                                            align,
                                                                            offset,
                                                                            pattern,
                                                                            length,
                                                                            repeats,
                                                                            elem);
    } else {
        /* not supported by this allocator */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("allocator type %s does not support aligned"
                   " sparse alloc\n"),
                   _shr_res_alloc_mgrs[thisPool->resManagerType].name));
        result = _SHR_E_UNAVAIL;
    }
    if (_SHR_E_NONE == result) {
        /* account for successful allocation */
        if (0 == (flags & SHR_RES_ALLOC_REPLACE)) {
            /* only account for alloc if new allocation */
            for (index = 0, count = 0; index < length; index++) {
                if (pattern & (1 << index)) {
                    count++;
                }
            }
            count *= repeats;
            handle->res[res_id]->refCount += count;
            thisPool->inuse += count;
        }
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, %d, %08X, %d, %d, &(%d))"
               " return %d (%s)\n"),
               (void*)handle,
               res_id,
               flags,
               align,
               offset,
               pattern,
               length,
               repeats,
               *elem,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Alloc a tagged block of elements with the requested alignment and offset.
 */
int
shr_mres_alloc_align_tag(shr_mres_handle_t handle,
                         int res_id,
                         uint32 flags,
                         int align,
                         int offset,
                         const void *tag,
                         int count,
                         int *elem)
{
    _shr_res_pool_desc_t *thisPool;
    _shr_res_type_desc_t *thisType;
    int result = _SHR_E_NONE;
    int base;
    int scaled;
    int scaledAlign;
    int scaledOffset;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, %d, %p, %d, %p) enter\n"),
               (void*)handle,
               res_id,
               flags,
               align,
               offset,
               (void*)tag,
               count,
               (void*)elem));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_TYPE_VALID_CHECK(handle, res_id);
    RES_TYPE_EXIST_CHECK(handle, res_id);
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("element count %d must be > 0\n"),
                   count));
        return _SHR_E_PARAM;
    }
    if (1 > align) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META("align <= 0 invalid, using align = 1 instead\n")));
        align = 1;
    }
    if ((offset >= align) || (0 > offset)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("offset %d must be >= 0 and < align %d\n"),
                   offset,
                   align));
        return _SHR_E_PARAM;
    }
    if (flags & (~SHR_RES_ALLOC_SINGLE_FLAGS)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("invalid flags %08X\n"),
                   flags & (~SHR_RES_ALLOC_SINGLE_FLAGS)));
        return _SHR_E_PARAM;
    }
    if (!elem) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("obligatory argument is NULL\n")));
        return _SHR_E_PARAM;
    }
    /* get the pool information */
    thisType = handle->res[res_id];
    thisPool = handle->pool[thisType->resPoolId];
    /* adjust element count per scaling factor */
    scaled = count * thisType->resElemSize;
    scaledAlign = align * thisType->resElemSize;
    scaledOffset = offset * thisType->resElemSize;
    
    /* check alignment for WITH_ID case */
    if (flags & SHR_RES_ALLOC_WITH_ID) {
        if (flags & SHR_RES_ALLOC_ALIGN_ZERO) {
            base = *elem;
        } else {
            base = *elem - thisPool->low;
        }
        if (((((base) / scaledAlign) * scaledAlign) + scaledOffset) != base) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("WITH_ID requested element %d does not comply"
                       " with alignment specifications\n"),
                       *elem));
            return _SHR_E_PARAM;
        }
    }
    if (_shr_res_alloc_mgrs[thisPool->resManagerType].tag_align) {
        /* allocator supports it; make the call */
        result = _shr_res_alloc_mgrs[thisPool->resManagerType].tag_align(thisPool,
                                                                         flags,
                                                                         scaledAlign,
                                                                         scaledOffset,
                                                                         tag,
                                                                         scaled,
                                                                         elem);
    } else {
        /* not supported by this allocator */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("allocator type %s does not support tagged aligned"
                   " alloc\n"),
                   _shr_res_alloc_mgrs[thisPool->resManagerType].name));
        result = _SHR_E_UNAVAIL;
    }
    if (_SHR_E_NONE == result) {
        /* account for successful allocation */
        if (0 == (flags & SHR_RES_ALLOC_REPLACE)) {
            /* only account for alloc if new allocation */
            handle->res[res_id]->refCount += count;
            thisPool->inuse += scaled;
        }
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, %d, %p, %d, &(%d)) return"
               " %d (%s)\n"),
               (void*)handle,
               res_id,
               flags,
               align,
               offset,
               (void*)tag,
               count,
               *elem,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Allocate a bunch of aligned elements or blocks of a resource type
 */
int
shr_mres_alloc_align_tag_group(shr_mres_handle_t handle,
                               int res_id,
                               uint32 grp_flags,
                               int grp_size,
                               int *grp_done,
                               const uint32 *flags,
                               const int *align,
                               const int *offset,
                               const void **tag,
                               const int *count,
                               int *elem)
{
    _shr_res_pool_desc_t *thisPool;
    _shr_res_type_desc_t *thisRes;
    int result = _SHR_E_NONE;
    int xresult;
    int scaled = 0;
    int scaledAlign;
    int scaledOffset;
    int index;
    int xalign;
    int base;
    uint32 blkFlags;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, %p, %p, %p, %p, %p, %p, %p)"
               " enter\n"),
               (void*)handle,
               res_id,
               grp_flags,
               grp_size,
               (void*)grp_done,
               (void*)flags,
               (void*)align,
               (void*)offset,
               (void*)tag,
               (void*)count,
               (void*)elem));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_TYPE_VALID_CHECK(handle, res_id);
    RES_TYPE_EXIST_CHECK(handle, res_id);
    if (!grp_done) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("obligatory out argument grp_done is NULL\n")));
        return _SHR_E_PARAM;
    }
    *grp_done = 0;
    if (0 > grp_size) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("group member count %d must be >= 0\n"),
                   grp_size));
        return _SHR_E_PARAM;
    }
    if ((0 < grp_size) && (!flags || !count || !elem || !align || !offset)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("an obligatory array pointer is NULL\n")));
        return _SHR_E_PARAM;
    }
    if (grp_flags & (~(SHR_RES_ALLOC_SINGLE_FLAGS | SHR_RES_ALLOC_GROUP_FLAGS))) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("invalid group flags %08X\n"),
                   grp_flags & (~(SHR_RES_ALLOC_SINGLE_FLAGS |
                   SHR_RES_ALLOC_GROUP_FLAGS))));
        return _SHR_E_PARAM;
    }
    /* get the resource information */
    thisRes = handle->res[res_id];
    /* get the pool information */
    thisPool = handle->pool[thisRes->resPoolId];
    /* try to collect the requested blocks */
    if (_shr_res_alloc_mgrs[thisPool->resManagerType].tag_align) {
        /* allocator does not support this feature */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("allocator type %s does not support tagged aligned"
                   " alloc\n"),
                   _shr_res_alloc_mgrs[thisPool->resManagerType].name));
        result = _SHR_E_UNAVAIL;
    }
    for (index = 0;
         (_SHR_E_NONE == result) && (index < grp_size);
         index++) {
        /* check parameters for this block */
        blkFlags = flags[index] | (grp_flags & SHR_RES_ALLOC_SINGLE_FLAGS);
        if (blkFlags & (~SHR_RES_ALLOC_SINGLE_FLAGS)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("invalid flags %08X for block %d\n"),
                       blkFlags & (~SHR_RES_ALLOC_SINGLE_FLAGS),
                       index));
            result = _SHR_E_PARAM;
        }
        if (0 >= count[index]) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("element count %d must be > 0\n"),
                       count[index]));
            result = _SHR_E_PARAM;
        }
        if (1 > align[index]) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META("align <= 0 invalid, using align = 1 instead\n")));
            xalign = 1;
        } else {
            xalign = align[index];
        }
        if ((offset[index] >= xalign) || (0 > offset[index])) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("offset %d must be >= 0 and < align %d\n"),
                       offset[index],
                       xalign));
            result = _SHR_E_PARAM;
        }
        if (_SHR_E_NONE == result) {
            /* adjust element count per scaling factor */
            scaled = count[index] * thisRes->resElemSize;
            scaledAlign = xalign * thisRes->resElemSize;
            scaledOffset = offset[index] * thisRes->resElemSize;
            
            /* check alignment for WITH_ID case */
            if (blkFlags & SHR_RES_ALLOC_WITH_ID) {
                if (blkFlags & SHR_RES_ALLOC_ALIGN_ZERO) {
                    base = elem[index];
                } else {
                    base = elem[index] - thisPool->low;
                }
                if (((((base) / scaledAlign) * scaledAlign) + scaledOffset) != base) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META("WITH_ID requested element %d does not"
                               " comply with alignment specifications\n"),
                               elem[index]));
                    result = _SHR_E_PARAM;
                }
            }
            if (_SHR_E_NONE == result) {
                result = _shr_res_alloc_mgrs[thisPool->resManagerType].tag_align(thisPool,
                                                                                 blkFlags,
                                                                                 scaledAlign,
                                                                                 scaledOffset,
                                                                                 tag[index],
                                                                                 scaled,
                                                                                 &(elem[index]));
            }
        }
        if (_SHR_E_NONE == result) {
            /* account for successful allocation */
            if (0 == (blkFlags & SHR_RES_ALLOC_REPLACE)) {
                /* only account for alloc if new allocation */
                handle->res[res_id]->refCount += count[index];
                thisPool->inuse += scaled;
            }
        } else {
            /* we'll hope the allocation manager displayed an error */
            /* don't want postincrement if an error occurred */
            break;
        }
    } /* for (all blocks in the caller's group) */
    if ((_SHR_E_NONE != result) && (grp_flags & SHR_RES_ALLOC_GROUP_ATOMIC)) {
        /* atomic mode and it failed; back out everything that we have */
        /* index is at the first failure */
        while (index > 0) {
            /* look at previous index (must have been successful) */
            index--;
            blkFlags = flags[index] | (grp_flags & SHR_RES_ALLOC_SINGLE_FLAGS);
            if (0 == (blkFlags & SHR_RES_ALLOC_REPLACE)) {
                /* only free blocks that were not trying to be replaced */
                /* adjust element count per scaling factor */
                scaled = count[index] * thisRes->resElemSize;
                /* free this element or block */
                xresult = _shr_res_alloc_mgrs[thisPool->resManagerType].free(thisPool,
                                                                             scaled,
                                                                             elem[index]);
                if (_SHR_E_NONE != xresult) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META("unable to back out %p resource %d"
                               " index %d base %d count %d: %d (%s)\n"),
                               (void*)handle,
                               res_id,
                               index,
                               elem[index],
                               count[index],
                               result,
                               _SHR_ERRMSG(result)));
                } else {
                    thisRes->refCount -= count[index];
                    thisPool->inuse -= scaled;
                }
            } /* if (0 == (blkFlags & SHR_RES_ALLOC_REPLACE)) */
        } /* while (index > 0) */
    } /* if (error && SHR_RES_ALLOC_GROUP_ATOMIC was set) */
    /* update number of allocations that succeeded */
    *grp_done = index;

    /* return the result */
    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, &(%d), %p, %p, %p, %p, %p, %p)"
               " return %d (%s)\n"),
               (void*)handle,
               res_id,
               grp_flags,
               grp_size,
               *grp_done,
               (void*)flags,
               (void*)align,
               (void*)offset,
               (void*)tag,
               (void*)count,
               (void*)elem,
               result,
               _SHR_ERRMSG(result)));
    for (index = 0; index < grp_size; index++) {
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("  block %12d: %08X %12d %12d %12d %12d\n"),
                   index,
                   flags[index],
                   align[index],
                   offset[index],
                   count[index],
                   elem[index]));
    }
    return result;
}

/*
 *  Free elements of a resource type then get status flags
 */
int
shr_mres_free_and_status(shr_mres_handle_t handle,
              int res_id,
              int count,
                         int elem,
                         uint32 *status)
{
    _shr_res_pool_desc_t *thisPool;
    int result = _SHR_E_NONE;
    int scaled;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %d, %d, %p) enter\n"),
               (void*)handle,
               res_id,
               count,
               elem,
               (void*)status));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_TYPE_VALID_CHECK(handle, res_id);
    RES_TYPE_EXIST_CHECK(handle, res_id);
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("element count %d must be > 0\n"),
                   count));
        return _SHR_E_PARAM;
    }
    /* adjust element count per scaling factor */
    scaled = count * handle->res[res_id]->resElemSize;
    
    /* get the pool information */
    thisPool = handle->pool[handle->res[res_id]->resPoolId];
    /* make the call */
    result = _shr_res_alloc_mgrs[thisPool->resManagerType].free(thisPool,
                                                                scaled,
                                                                elem);
    if (_SHR_E_NONE == result) {
        /* account for successful deallocation */
        handle->res[res_id]->refCount -= count;
        thisPool->inuse -= scaled;
        if (status) {
            *status = 0;
            if (!(handle->res[res_id]->refCount)) {
                (*status) |= SHR_RES_FREED_TYPE_LAST_ELEM;
            }
            if (!(thisPool->inuse)) {
                (*status) |= SHR_RES_FREED_POOL_LAST_ELEM;
            }
        } /* if (flags) */
    } /* if (_SHR_E_NONE == result) */

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %d, %d, &(%08X)) return %d (%s)\n"),
               (void*)handle,
               res_id,
               count,
               elem,
               status?(*status):0,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Free elements of a resource type
 */
int
shr_mres_free(shr_mres_handle_t handle,
              int res_id,
              int count,
              int elem)
{
    return shr_mres_free_and_status(handle, res_id, count, elem, NULL);
}

/*
 *  Free a bunch of elements/blocks of a resource type then get status flags
 */
int
shr_mres_free_group_and_status(shr_mres_handle_t handle,
                    int res_id,
                    uint32 grp_flags,
                    int grp_size,
                    int *grp_done,
                    const int *count,
                               const int *elem,
                               uint32 *status)
{
    _shr_res_pool_desc_t *thisPool;
    _shr_res_type_desc_t *thisRes;
    int result = _SHR_E_NONE;
    int index;
    int scaled;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, %p, %p, %p, %p) enter\n"),
               (void*)handle,
               res_id,
               grp_flags,
               grp_size,
               (void*)grp_done,
               (void*)count,
               (void*)elem,
               (void*)status));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_TYPE_VALID_CHECK(handle, res_id);
    RES_TYPE_EXIST_CHECK(handle, res_id);
    if (!grp_done) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("obligatory out argument grp_done is NULL\n")));
        return _SHR_E_PARAM;
    }
    *grp_done = 0;
    if (0 > grp_size) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("group member count %d must be >= 0\n"),
                   grp_size));
        return _SHR_E_PARAM;
    }
    if ((0 < grp_size) && (!count || !elem)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("an obligatory array pointer is NULL\n")));
        return _SHR_E_PARAM;
    }
    if (grp_flags & (~SHR_RES_ALLOC_GROUP_FLAGS)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("invalid group flags %08X\n"),
                   grp_flags & (~SHR_RES_ALLOC_GROUP_FLAGS)));
        return _SHR_E_PARAM;
    }
    /* get the resource information */
    thisRes = handle->res[res_id];
    /* get the pool information */
    thisPool = handle->pool[thisRes->resPoolId];
    /* try to release the requested blocks */
    for (index = 0;
         (_SHR_E_NONE == result) && (index < grp_size);
         index++) {
        /* adjust element count per scaling factor */
        scaled = count[index] * thisRes->resElemSize;
        /* free this element or block */
        result = _shr_res_alloc_mgrs[thisPool->resManagerType].free(thisPool,
                                                                    scaled,
                                                                    elem[index]);
        if (_SHR_E_NONE == result) {
            /* account for successful deallocation */
            handle->res[res_id]->refCount -= count[index];
            thisPool->inuse -= scaled;
        } else {
            /* we'll hope the allocation manager displayed an error */
            /* don't want postincrement if an error occurred */
            break;
        }
    } /* for (all elements/blocks as long as no errors) */
    /* update number of frees that succeeded */
    *grp_done = index;
    if (status) {
        *status = 0;
        if (!(handle->res[res_id]->refCount)) {
            (*status) |= SHR_RES_FREED_TYPE_LAST_ELEM;
        }
        if (!(thisPool->inuse)) {
            (*status) |= SHR_RES_FREED_POOL_LAST_ELEM;
        }
    } /* if (flags) */

    /* return the result */
    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, &(%d), %p, %p, &(%08X)) return %d (%s)\n"),
               (void*)handle,
               res_id,
               grp_flags,
               grp_size,
               *grp_done,
               (void*)count,
               (void*)elem,
               status?(*status):0,
               result,
               _SHR_ERRMSG(result)));
    for (index = 0; index < grp_size; index++) {
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("  block %12d: %12d, %12d\n"),
                   index,
                   count[index],
                   elem[index]));
    }
    return result;
}

/*
 *  Free a bunch of elements/blocks of a resource type
 */
int
shr_mres_free_group(shr_mres_handle_t handle,
                    int res_id,
                    uint32 grp_flags,
                    int grp_size,
                    int *grp_done,
                    const int *count,
                    const int *elem)
{
    return shr_mres_free_group_and_status(handle,
                                          res_id,
                                          grp_flags,
                                          grp_size,
                                          grp_done,
                                          count,
                                          elem,
                                          NULL);
}

/*
 *  Free elements of a resource type then get status flags
 */
int
shr_mres_free_sparse_and_status(shr_mres_handle_t handle,
                                int res_id,
                                uint32 pattern,
                                int length,
                                int repeats,
                                int elem,
                                uint32 *status)
{
    _shr_res_pool_desc_t *thisPool;
    int result = _SHR_E_NONE;
    int count;
    int index;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, %d, %d, %p) enter\n"),
               (void*)handle,
               res_id,
               pattern,
               length,
               repeats,
               elem,
               (void*)status));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_TYPE_VALID_CHECK(handle, res_id);
    RES_TYPE_EXIST_CHECK(handle, res_id);
    if (0 >= length) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("pattern length must be greater than zero\n")));
        return _SHR_E_PARAM;
    }
    if (32 < length) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("pattern length must be 32 or less\n")));
        return _SHR_E_PARAM;
    }
    if (0 >= repeats) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("repeat count must be greater than zero\n")));
        return _SHR_E_PARAM;
    }
    if (1 != handle->res[res_id]->resElemSize) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("not compatible with scaled resources\n")));
        return _SHR_E_CONFIG;
    }
    /* get the pool information */
    thisPool = handle->pool[handle->res[res_id]->resPoolId];
    if (_shr_res_alloc_mgrs[thisPool->resManagerType].free_sparse) {
        /* make the call */
        result = _shr_res_alloc_mgrs[thisPool->resManagerType].free_sparse(thisPool,
                                                                           pattern,
                                                                           length,
                                                                           repeats,
                                                                           elem);
        if (_SHR_E_NONE == result) {
            /* account for successful deallocation */
            for (index = 0, count = 0; index < length; index++) {
                if (pattern & (1 << index)) {
                    count++;
                }
            }
            count *= repeats;
            handle->res[res_id]->refCount -= count;
            thisPool->inuse -= count;
            if (status) {
                *status = 0;
                if (!(handle->res[res_id]->refCount)) {
                    (*status) |= SHR_RES_FREED_TYPE_LAST_ELEM;
                }
                if (!(thisPool->inuse)) {
                    (*status) |= SHR_RES_FREED_POOL_LAST_ELEM;
                }
            } /* if (status) */
        } /* if (_SHR_E_NONE == result) */
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("allocator does not support sparse free\n")));
        return _SHR_E_UNAVAIL;
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, %d, %d, &(%08X)) return %d (%s)\n"),
               (void*)handle,
               res_id,
               pattern,
               length,
               repeats,
               elem,
               status?(*status):0,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Free elements of a resource type
 */
int
shr_mres_free_sparse(shr_mres_handle_t handle,
                     int res_id,
                     uint32 pattern,
                     int length,
                     int repeats,
                     int elem)
{
    return shr_mres_free_sparse_and_status(handle,
                                           res_id,
                                           pattern,
                                           length,
                                           repeats,
                                           elem,
                                           NULL);
}

/*
 *  Check whether there are in-use elements in a block
 */
int
shr_mres_check(shr_mres_handle_t handle,
               int res_id,
               int count,
               int elem)
{
    _shr_res_pool_desc_t *thisPool;
    int scaled;
    int result;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %d, %d) enter\n"),
               (void*)handle,
               res_id,
               count,
               elem));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_TYPE_VALID_CHECK(handle, res_id);
    RES_TYPE_EXIST_CHECK(handle, res_id);
    if (1 > count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("element count %d must be > 0\n"),
                   count));
        return _SHR_E_PARAM;
    }
    /* adjust element count per scaling factor */
    scaled = count * handle->res[res_id]->resElemSize;
    
    /* get the pool information */
    thisPool = handle->pool[handle->res[res_id]->resPoolId];
    /* make the call */
    result = _shr_res_alloc_mgrs[thisPool->resManagerType].check(thisPool,
                                                                 scaled,
                                                                 elem);

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %d, %d) return %d (%s)\n"),
               (void*)handle,
               res_id,
               count,
               elem,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Check a bunch of elements/blocks of a resource type
 */
int
shr_mres_check_group(shr_mres_handle_t handle,
                     int res_id,
                     uint32 grp_flags,
                     int grp_size,
                     int *grp_done,
                     const int *count,
                     const int *elem,
                     int *status)
{
    _shr_res_pool_desc_t *thisPool;
    _shr_res_type_desc_t *thisRes;
    int result = _SHR_E_NONE;
    int xresult;
    int index;
    int scaled;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, %p, %p, %p, %p) enter\n"),
               (void*)handle,
               res_id,
               grp_flags,
               grp_size,
               (void*)grp_done,
               (void*)count,
               (void*)elem,
               (void*)status));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_TYPE_VALID_CHECK(handle, res_id);
    RES_TYPE_EXIST_CHECK(handle, res_id);
    if (!grp_done) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("obligatory out argument grp_done is NULL\n")));
        return _SHR_E_PARAM;
    }
    *grp_done = 0;
    if (0 > grp_size) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("group member count %d must be >= 0\n"),
                   grp_size));
        return _SHR_E_PARAM;
    }
    if ((0 < grp_size) && (!count || !elem || !status)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("an obligatory array pointer is NULL\n")));
        return _SHR_E_PARAM;
    }
    if (grp_flags & (~SHR_RES_ALLOC_GROUP_FLAGS)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("invalid group flags %08X\n"),
                   grp_flags & (~SHR_RES_ALLOC_GROUP_FLAGS)));
        return _SHR_E_PARAM;
    }
    /* get the resource information */
    thisRes = handle->res[res_id];
    /* get the pool information */
    thisPool = handle->pool[thisRes->resPoolId];
    /* try to check the requested blocks */
    for (index = 0;
         (_SHR_E_NONE == result) && (index < grp_size);
         index++) {
        /* adjust element count per scaling factor */
        scaled = count[index] * thisRes->resElemSize;
        /* check this element or block */
        xresult = _shr_res_alloc_mgrs[thisPool->resManagerType].check(thisPool,
                                                                      scaled,
                                                                      elem[index]);
        status[index] = xresult;
        if ((_SHR_E_NOT_FOUND != xresult) &&
            (_SHR_E_EXISTS != xresult)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("unexpected result checking %p resource %d"
                       " index %d elem %d count %d: %d (%s)\n"),
                       (void*)handle,
                       res_id,
                       index,
                       elem[index],
                       count[index],
                       result,
                       _SHR_ERRMSG(result)));
            if (!(grp_flags & SHR_RES_ALLOC_GROUP_ATOMIC)) {
                /* if not atomic mode, abort on first unexpected result */
                result = _SHR_E_FAIL;
                break;
            }
        } /* if (result is neither NOT_FOUND nor EXISTS) */
    } /* for (all elements/blocks as long as no errors) */
    /* update number of frees that succeeded */
    *grp_done = index;

    /* return the result */
    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, &(%d), %p, %p, %p) return %d (%s)\n"),
               (void*)handle,
               res_id,
               grp_flags,
               grp_size,
               *grp_done,
               (void*)count,
               (void*)elem,
               (void*)status,
               result,
               _SHR_ERRMSG(result)));
    for (index = 0; index < grp_size; index++) {
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("  block %12d: %d, %d, %d (%s)\n"),
                   index,
                   count[index],
                   elem[index],
                   status[index],
                   _SHR_ERRMSG(status[index])));
    }
    return result;
}

/*
 *  Check whether all elements are free, all elements are used, mixed free and
 *  in-use, or block size is appropriate, for a block of elements.  The block
 *  size check assumes the intent is to 'reallocate' the specified block for a
 *  paritcular purpose, and failing it does not indicate corruption or fault.
 */
int
shr_mres_check_all(shr_mres_handle_t handle,
                   int res_id,
                   int count,
                   int elem)
{
    _shr_res_pool_desc_t *thisPool;
    int scaled;
    int result;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %d, %d) enter\n"),
               (void*)handle,
               res_id,
               count,
               elem));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_TYPE_VALID_CHECK(handle, res_id);
    RES_TYPE_EXIST_CHECK(handle, res_id);
    if (1 > count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("element count %d must be > 0\n"),
                   count));
        return _SHR_E_PARAM;
    }
    /* adjust element count per scaling factor */
    scaled = count * handle->res[res_id]->resElemSize;
    
    /* get the pool information */
    thisPool = handle->pool[handle->res[res_id]->resPoolId];
    /* make the call */
    result = _shr_res_alloc_mgrs[thisPool->resManagerType].check_all(thisPool,
                                                                     scaled,
                                                                     elem);

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %d, %d) return %d (%s)\n"),
               (void*)handle,
               res_id,
               count,
               elem,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Check_all for a bunch of elements/blocks of a resource type
 */
int
shr_mres_check_all_group(shr_mres_handle_t handle,
                         int res_id,
                         uint32 grp_flags,
                         int grp_size,
                         int *grp_done,
                         const int *count,
                         const int *elem,
                         int *status)
{
    _shr_res_pool_desc_t *thisPool;
    _shr_res_type_desc_t *thisRes;
    int result = _SHR_E_NONE;
    int xresult;
    int index;
    int scaled;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, %p, %p, %p, %p) enter\n"),
               (void*)handle,
               res_id,
               grp_flags,
               grp_size,
               (void*)grp_done,
               (void*)count,
               (void*)elem,
               (void*)status));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_TYPE_VALID_CHECK(handle, res_id);
    RES_TYPE_EXIST_CHECK(handle, res_id);
    if (!grp_done) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("obligatory out argument grp_done is NULL\n")));
        return _SHR_E_PARAM;
    }
    *grp_done = 0;
    if (0 > grp_size) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("group member count %d must be >= 0\n"),
                   grp_size));
        return _SHR_E_PARAM;
    }
    if ((0 < grp_size) && (!count || !elem || !status)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("an obligatory array pointer is NULL\n")));
        return _SHR_E_PARAM;
    }
    if (grp_flags & (~SHR_RES_ALLOC_GROUP_FLAGS)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("invalid group flags %08X\n"),
                   grp_flags & (~SHR_RES_ALLOC_GROUP_FLAGS)));
        return _SHR_E_PARAM;
    }
    /* get the resource information */
    thisRes = handle->res[res_id];
    /* get the pool information */
    thisPool = handle->pool[thisRes->resPoolId];
    /* try to check the requested blocks */
    for (index = 0;
         (_SHR_E_NONE == result) && (index < grp_size);
         index++) {
        /* adjust element count per scaling factor */
        scaled = count[index] * thisRes->resElemSize;
        /* check this element or block */
        xresult = _shr_res_alloc_mgrs[thisPool->resManagerType].check_all(thisPool,
                                                                          scaled,
                                                                          elem[index]);
        status[index] = xresult;
        if ((_SHR_E_NOT_FOUND != xresult) &&
            (_SHR_E_EXISTS != xresult)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("unexpected result checking %p resource %d"
                       " index %d elem %d count %d: %d (%s)\n"),
                       (void*)handle,
                       res_id,
                       index,
                       elem[index],
                       count[index],
                       result,
                       _SHR_ERRMSG(result)));
            if (!(grp_flags & SHR_RES_ALLOC_GROUP_ATOMIC)) {
                /* if not atomic mode, abort on first unexpected result */
                result = _SHR_E_FAIL;
                break;
            }
        } /* if (result is neither NOT_FOUND nor EXISTS) */
    } /* for (all elements/blocks as long as no errors) */
    /* update number of frees that succeeded */
    *grp_done = index;

    /* return the result */
    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, &(%d), %p, %p, %p) return %d (%s)\n"),
               (void*)handle,
               res_id,
               grp_flags,
               grp_size,
               *grp_done,
               (void*)count,
               (void*)elem,
               (void*)status,
               result,
               _SHR_ERRMSG(result)));
    for (index = 0; index < grp_size; index++) {
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("  block %12d: %d, %d, %d (%s)\n"),
                   index,
                   count[index],
                   elem[index],
                   status[index],
                   _SHR_ERRMSG(status[index])));
    }
    return result;
}

/*
 *  Check whether all elements are free, all elements are used, mixed free and
 *  in-use, or block size is appropriate, for a sparse block of elements.  The
 *  block size check assumes the intent is to 'reallocate' the specified block
 *  for a paritcular purpose, and failing it does not indicate corruption or
 *  fault.
 */
int
shr_mres_check_all_sparse(shr_mres_handle_t handle,
                          int res_id,
                          uint32 pattern,
                          int length,
                          int repeats,
                          int elem)
{
    _shr_res_pool_desc_t *thisPool;
    int result;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, %d, %d) enter\n"),
               (void*)handle,
               res_id,
               pattern,
               length,
               repeats,
               elem));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_TYPE_VALID_CHECK(handle, res_id);
    RES_TYPE_EXIST_CHECK(handle, res_id);
    if (0 >= length) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("pattern length must be greater than zero\n")));
        return _SHR_E_PARAM;
    }
    if (32 < length) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("pattern length must be 32 or less\n")));
        return _SHR_E_PARAM;
    }
    if (0 >= repeats) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("repeat count must be greater than zero\n")));
        return _SHR_E_PARAM;
    }
    if (1 != handle->res[res_id]->resElemSize) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("not compatible with scaled resources\n")));
        return _SHR_E_CONFIG;
    }
    /* get the pool information */
    thisPool = handle->pool[handle->res[res_id]->resPoolId];
    if (_shr_res_alloc_mgrs[thisPool->resManagerType].c_a_sparse) {
        /* make the call */
        result = _shr_res_alloc_mgrs[thisPool->resManagerType].c_a_sparse(thisPool,
                                                                          pattern,
                                                                          length,
                                                                          repeats,
                                                                          elem);
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("allocator does not support sparse check all\n")));
        return _SHR_E_UNAVAIL;
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, %d, %d) return %d (%s)\n"),
               (void*)handle,
               res_id,
               pattern,
               length,
               repeats,
               elem,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Check whether all elements are free, all elements are used, mixed free and
 *  in-use, or block size is appropriate, for a block of elements.  The block
 *  size check assumes the intent is to 'reallocate' the specified block for a
 *  paritcular purpose, and failing it does not indicate corruption or fault.
 */
int
shr_mres_check_all_tag(shr_mres_handle_t handle,
                       int res_id,
                       const void *tag,
                       int count,
                       int elem)
{
    _shr_res_pool_desc_t *thisPool;
    int scaled;
    int result;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %p, %d, %d) enter\n"),
               (void*)handle,
               res_id,
               (void*)tag,
               count,
               elem));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_TYPE_VALID_CHECK(handle, res_id);
    RES_TYPE_EXIST_CHECK(handle, res_id);
    if (1 > count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("element count %d must be > 0\n"),
                   count));
        return _SHR_E_PARAM;
    }
    /* adjust element count per scaling factor */
    scaled = count * handle->res[res_id]->resElemSize;
    
    /* get the pool information */
    thisPool = handle->pool[handle->res[res_id]->resPoolId];
    /* make the call */
    result = _shr_res_alloc_mgrs[thisPool->resManagerType].check_all_tag(thisPool,
                                                                         tag,
                                                                         scaled,
                                                                         elem);

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %p, %d, %d) return %d (%s)\n"),
               (void*)handle,
               res_id,
               (void*)tag,
               count,
               elem,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Check_all for a bunch of elements/blocks of a resource type
 */
int
shr_mres_check_all_tag_group(shr_mres_handle_t handle,
                             int res_id,
                             uint32 grp_flags,
                             int grp_size,
                             int *grp_done,
                             const void **tag,
                             const int *count,
                             const int *elem,
                             int *status)
{
    _shr_res_pool_desc_t *thisPool;
    _shr_res_type_desc_t *thisRes;
    int result = _SHR_E_NONE;
    int xresult;
    int index;
    int scaled;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, %p, %p, %p, %p, %p) enter\n"),
               (void*)handle,
               res_id,
               grp_flags,
               grp_size,
               (void*)grp_done,
               (void*)tag,
               (void*)count,
               (void*)elem,
               (void*)status));

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);
    RES_TYPE_VALID_CHECK(handle, res_id);
    RES_TYPE_EXIST_CHECK(handle, res_id);
    if (!grp_done) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("obligatory out argument grp_done is NULL\n")));
        return _SHR_E_PARAM;
    }
    *grp_done = 0;
    if (0 > grp_size) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("group member count %d must be >= 0\n"),
                   grp_size));
        return _SHR_E_PARAM;
    }
    if ((0 < grp_size) && (!count || !elem || !status)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("an obligatory array pointer is NULL\n")));
        return _SHR_E_PARAM;
    }
    if (grp_flags & (~SHR_RES_ALLOC_GROUP_FLAGS)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("invalid group flags %08X\n"),
                   grp_flags & (~SHR_RES_ALLOC_GROUP_FLAGS)));
        return _SHR_E_PARAM;
    }
    /* get the resource information */
    thisRes = handle->res[res_id];
    /* get the pool information */
    thisPool = handle->pool[thisRes->resPoolId];
    /* try to check the requested blocks */
    for (index = 0;
         (_SHR_E_NONE == result) && (index < grp_size);
         index++) {
        /* adjust element count per scaling factor */
        scaled = count[index] * thisRes->resElemSize;
        /* check this element or block */
        xresult = _shr_res_alloc_mgrs[thisPool->resManagerType].check_all_tag(thisPool,
                                                                              tag[index],
                                                                              scaled,
                                                                              elem[index]);
        status[index] = xresult;
        if ((_SHR_E_NOT_FOUND != xresult) &&
            (_SHR_E_EXISTS != xresult)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("unexpected result checking %p resource %d"
                       " index %d elem %d count %d: %d (%s)\n"),
                       (void*)handle,
                       res_id,
                       index,
                       elem[index],
                       count[index],
                       result,
                       _SHR_ERRMSG(result)));
            if (!(grp_flags & SHR_RES_ALLOC_GROUP_ATOMIC)) {
                /* if not atomic mode, abort on first unexpected result */
                result = _SHR_E_FAIL;
                break;
            }
        } /* if (result is neither NOT_FOUND nor EXISTS) */
    } /* for (all elements/blocks as long as no errors) */
    /* update number of frees that succeeded */
    *grp_done = index;

    /* return the result */
    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%p, %d, %08X, %d, &(%d), %p, %p, %p, %p) return %d (%s)\n"),
               (void*)handle,
               res_id,
               grp_flags,
               grp_size,
               *grp_done,
               (void*)tag,
               (void*)count,
               (void*)elem,
               (void*)status,
               result,
               _SHR_ERRMSG(result)));
    for (index = 0; index < grp_size; index++) {
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("  block %12d: %p, %d, %d, %d (%s)\n"),
                   index,
                   (void*)(tag[index]),
                   count[index],
                   elem[index],
                   status[index],
                   _SHR_ERRMSG(status[index])));
    }
    return result;
}

/*
 *  Diagnostic dump
 */
int
shr_mres_dump(shr_mres_handle_t handle)
{
    _shr_res_pool_desc_t *thisPool;
    _shr_res_type_desc_t *thisRes;
    int index;
    int result = _SHR_E_NONE;

    /* a little parameter checking */
    RES_HANDLE_VALID_CHECK(handle);

    /* dump information about the handle */
    LOG_CLI((BSL_META("%p resource allocation manager\n"), (void*)handle));
    LOG_CLI((BSL_META("  resource type count = %d\n"), handle->resTypeCount));
    LOG_CLI((BSL_META("  resource pool count = %d\n"), handle->resPoolCount));
    for (index = 0; index < handle->resPoolCount; index++) {
        thisPool = handle->pool[index];
        if (thisPool) {
            LOG_CLI((BSL_META("  resource pool %d (%s):\n"),
                     index,
                     &(thisPool->name[0])));
            LOG_CLI((BSL_META("    method = %d (%s)\n"),
                     thisPool->resManagerType,
                     _shr_res_alloc_mgrs[thisPool->resManagerType].name));
            LOG_CLI((BSL_META("    handle = %p\n"), (void*)thisPool->resHandle));
            LOG_CLI((BSL_META("    range  = %d..%d\n"),
                     thisPool->low,
                     thisPool->low + thisPool->count - 1));
            LOG_CLI((BSL_META("    elems  = %d (%d in use)\n"),
                     thisPool->count,
                     thisPool->inuse));
            LOG_CLI((BSL_META("    refcnt = %d\n"), thisPool->refCount));
        } else {
            LOG_CLI((BSL_META("  resource pool %d is not in use\n"), index));
        }
    }
    for (index = 0; index < handle->resTypeCount; index++) {
        thisRes = handle->res[index];
        if (thisRes) {
            thisPool = handle->pool[thisRes->resPoolId];
            LOG_CLI((BSL_META("  resource type %d (%s):\n"),
                     index,
                     &(thisRes->name[0])));
            LOG_CLI((BSL_META("    resource pool   = %d (%s)\n"),
                     thisRes->resPoolId,
                     &(thisPool->name[0])));
            LOG_CLI((BSL_META("    pool elem each  = %d\n"),
                     thisRes->resElemSize));
            LOG_CLI((BSL_META("    active elements = %d\n"),
                     thisRes->refCount));
        } else {
            LOG_CLI((BSL_META("  resource type %d is not in use\n"), index));
        }
    }
    return result;
}

/*****************************************************************************/
/*
 *  Exposed API implementation (unit based, local handle cache)
 */

/*
 *  Initialise unit
 */
int
shr_res_init(int unit,
             int num_res_types,
             int num_res_pools)
{
    _shr_res_unit_desc_t *tempUnit;
    int result = _SHR_E_NONE;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "(%d, %d, %d) enter\n"),
               unit,
               num_res_types,
               num_res_pools));

    /* a little parameter checking */
    if ((0 > unit) || (BCM_LOCAL_UNITS_MAX <= unit)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "invalid unit number %d\n"),
                   unit));
        return _SHR_E_PARAM;
    }

    /* get this unit's current information, mark it as destroyed */
    
    tempUnit = _g_unitResDesc[unit];
    _g_unitResDesc[unit] = NULL;
    if (tempUnit) {
        /* this unit has already been initialised; tear it down */
        result = _shr_mres_destroy_data(tempUnit);
        if (_SHR_E_NONE != result) {
            /* something went wrong with the teardown, put what's left back */
            _g_unitResDesc[unit] = tempUnit;
        }
        tempUnit = NULL;
    }

    if (_SHR_E_NONE == result) {
        result = shr_mres_create(&tempUnit,
                                 num_res_types,
                                 num_res_pools);
        if (_SHR_E_NONE == result) {
            _g_unitResDesc[unit] = tempUnit;
        }
    } /* if (_SHR_E_NONE == result) */

    /* return the result */
    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "(%d, %d, %d) return %d (%s)\n"),
               unit,
               num_res_types,
               num_res_pools,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Get unit resource information (top level)
 */
int
shr_res_get(int unit,
            int *num_res_types,
            int *num_res_pools)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the fetch */
    return shr_mres_get(thisUnit, num_res_types, num_res_pools);
}

/*
 *  Deinitialise unit
 */
int
shr_res_detach(int unit)
{
    _shr_res_unit_desc_t *tempUnit;
    int result = _SHR_E_NONE;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "(%d) enter\n"),
               unit));

    /* get this unit's current information, mark it as destroyed */
    
    tempUnit = _g_unitResDesc[unit];
    _g_unitResDesc[unit] = NULL;
    if (tempUnit) {
        /* this unit has already been initialised; tear it down */
        result = _shr_mres_destroy_data(tempUnit);
        if (_SHR_E_NONE != result) {
            /* something went wrong with the teardown, put what's left back */
            _g_unitResDesc[unit] = tempUnit;
        } else {
            sal_free(tempUnit);
        }
        tempUnit = NULL;
    } /* if (tempUnit) */
    /* else would be not inited, again, easy to detach in that case - NOP */

    /* return the result */
    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "(%d) return %d (%s)\n"),
               unit,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Configure a resource pool on a unit
 */
int
shr_res_pool_set(int unit,
                 int pool_id,
                 shr_res_allocator_t manager,
                 int low_id,
                 int count,
                 const void *extras,
                 const char *name)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_pool_set(thisUnit,
                             pool_id,
                             manager,
                             low_id,
                             count,
                             extras,
                             name);
}

/*
 *  Destroy a resource pool on a unit
 */
int
shr_res_pool_unset(int unit,
                   int pool_id)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_pool_unset(thisUnit, pool_id);
}

/*
 *  Get information about a resource pool on a unit
 */
int
shr_res_pool_get(int unit,
                 int pool_id,
                 shr_res_allocator_t *manager,
                 int *low_id,
                 int *count,
                 const void **extras,
                 const char **name)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_pool_get(thisUnit,
                             pool_id,
                             manager,
                             low_id,
                             count,
                             extras,
                             name);
}

/*
 *  Get information about a resource pool on a unit
 */
int
shr_res_pool_info_get(int unit,
                      int pool_id,
                      shr_res_pool_info_t *info)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_pool_info_get(thisUnit,
                                  pool_id,
                                  info);
}

/*
 *  Configure a resource type on a unit
 */
int
shr_res_type_set(int unit,
                 int res_id,
                 int pool_id,
                 int elem_size,
                 const char *name)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_type_set(thisUnit, res_id, pool_id, elem_size, name);
}

/*
 *  Destroy a resource type on a unit
 */
int
shr_res_type_unset(int unit,
                   int res_id)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_type_unset(thisUnit, res_id);
}

/*
 *  Get information about a resource type
 */
int
shr_res_type_get(int unit,
                 int res_id,
                 int *pool_id,
                 int *elem_size,
                 const char **name)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_type_get(thisUnit, res_id, pool_id, elem_size, name);
}

/*
 *  Get information about a resource type
 */
int
shr_res_type_info_get(int unit,
                      int res_id,
                      shr_res_type_info_t *info)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_type_info_get(thisUnit, res_id, info);
}

/*
 *  Allocate elements of a resource type
 */
int
shr_res_alloc(int unit,
              int res_id,
              uint32 flags,
              int count,
              int *elem)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_alloc(thisUnit, res_id, flags, count, elem);
}

/*
 *  Allocate a bunch of elements or blocks of a resource type
 */
int
shr_res_alloc_group(int unit,
                    int res_id,
                    uint32 grp_flags,
                    int grp_size,
                    int *grp_done,
                    const uint32 *flags,
                    const int *count,
                    int *elem)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_alloc_group(thisUnit,
                                res_id,
                                grp_flags,
                                grp_size,
                                grp_done,
                                flags,
                                count,
                                elem);
}

/*
 *  Allocate elements of a resource type
 */
int
shr_res_alloc_tag(int unit,
                  int res_id,
                  uint32 flags,
                  const void *tag,
                  int count,
                  int *elem)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_alloc_tag(thisUnit, res_id, flags, tag, count, elem);
}

/*
 *  Allocate a bunch of elements or blocks of a resource type
 */
int
shr_res_alloc_tag_group(int unit,
                        int res_id,
                        uint32 grp_flags,
                        int grp_size,
                        int *grp_done,
                        const uint32 *flags,
                        const void **tag,
                        const int *count,
                        int *elem)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_alloc_tag_group(thisUnit,
                                    res_id,
                                    grp_flags,
                                    grp_size,
                                    grp_done,
                                    flags,
                                    tag,
                                    count,
                                    elem);
}

/*
 *  Allocate a block of elements with the requested alignment and offset.
 */
int
shr_res_alloc_align(int unit,
                    int res_id,
                    uint32 flags,
                    int align,
                    int offset,
                    int count,
                    int *elem)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_alloc_align(thisUnit,
                                res_id,
                                flags,
                                align,
                                offset,
                                count,
                                elem);
}

/*
 *  Allocate a sparse block of elements with the requested alignment and
 *  offset.
 */
int
shr_res_alloc_align_sparse(int unit,
                           int res_id,
                           uint32 flags,
                           int align,
                           int offset,
                           uint32 pattern,
                           int length,
                           int repeats,
                           int *elem)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_alloc_align_sparse(thisUnit,
                                       res_id,
                                       flags,
                                       align,
                                       offset,
                                       pattern,
                                       length,
                                       repeats,
                                       elem);
}

/*
 *  Allocate a bunch of aligned elements or blocks of a resource type
 */
int
shr_res_alloc_align_group(int unit,
                          int res_id,
                          uint32 grp_flags,
                          int grp_size,
                          int *grp_done,
                          const uint32 *flags,
                          const int *align,
                          const int *offset,
                          const int *count,
                          int *elem)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_alloc_align_group(thisUnit,
                                      res_id,
                                      grp_flags,
                                      grp_size,
                                      grp_done,
                                      flags,
                                      align,
                                      offset,
                                      count,
                                      elem);
}

/*
 *  Allocate a block of elements with the requested alignment and offset.
 */
int
shr_res_alloc_align_tag(int unit,
                        int res_id,
                        uint32 flags,
                        int align,
                        int offset,
                        const void *tag,
                        int count,
                        int *elem)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_alloc_align_tag(thisUnit,
                                    res_id,
                                    flags,
                                    align,
                                    offset,
                                    tag,
                                    count,
                                    elem);
}

/*
 *  Allocate a bunch of aligned elements or blocks of a resource type
 */
int
shr_res_alloc_align_tag_group(int unit,
                              int res_id,
                              uint32 grp_flags,
                              int grp_size,
                              int *grp_done,
                              const uint32 *flags,
                              const int *align,
                              const int *offset,
                              const void **tag,
                              const int *count,
                              int *elem)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_alloc_align_tag_group(thisUnit,
                                          res_id,
                                          grp_flags,
                                          grp_size,
                                          grp_done,
                                          flags,
                                          align,
                                          offset,
                                          tag,
                                          count,
                                          elem);
}

/*
 *  Free elements of a resource type
 */
int
shr_res_free(int unit,
             int res_id,
             int count,
             int elem)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_free_and_status(thisUnit, res_id, count, elem, NULL);
}

/*
 *  Free elements of a resource type, get status flags
 */
int
shr_res_free_and_status(int unit,
                        int res_id,
                        int count,
                        int elem,
                        uint32 *flags)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_free_and_status(thisUnit, res_id, count, elem, flags);
}

/*
 *  Free sparse block of elements of a resource type
 */
int
shr_res_free_sparse(int unit,
                    int res_id,
                    uint32 pattern,
                    int length,
                    int repeats,
                    int elem)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_free_sparse_and_status(thisUnit,
                                           res_id,
                                           pattern,
                                           length,
                                           repeats,
                                           elem,
                                           NULL);
}

/*
 *  Free sparse block of elements of a resource type, get status flags
 */
int
shr_res_free_sparse_and_status(int unit,
                               int res_id,
                               uint32 pattern,
                               int length,
                               int repeats,
                               int elem,
                               uint32 *flags)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_free_sparse_and_status(thisUnit,
                                           res_id,
                                           pattern,
                                           length,
                                           repeats,
                                           elem,
                                           flags);
}

/*
 *  Free a bunch of elements/blocks of a resource type
 */
int
shr_res_free_group(int unit,
                   int res_id,
                   uint32 grp_flags,
                   int grp_size,
                   int *grp_done,
                   const int *count,
                   const int *elem)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_free_group_and_status(thisUnit,
                               res_id,
                               grp_flags,
                               grp_size,
                               grp_done,
                               count,
                                          elem,
                                          NULL);
}


/*
 *  Free a bunch of elements/blocks of a resource type, get status flags
 */
int
shr_res_free_group_and_status(int unit,
                              int res_id,
                              uint32 grp_flags,
                              int grp_size,
                              int *grp_done,
                              const int *count,
                              const int *elem,
                              uint32 *status)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_free_group_and_status(thisUnit,
                                          res_id,
                                          grp_flags,
                                          grp_size,
                                          grp_done,
                                          count,
                                          elem,
                                          status);
}

/*
 *  Check whether elements are free or not
 */
int
shr_res_check(int unit,
              int res_id,
              int count,
              int elem)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_check(thisUnit, res_id, count, elem);
}

/*
 *  Check a bunch of elements/blocks of a resource type
 */
int
shr_res_check_group(int unit,
                    int res_id,
                    uint32 grp_flags,
                    int grp_size,
                    int *grp_done,
                    const int *count,
                    const int *elem,
                    int *status)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_check_group(thisUnit,
                                res_id,
                                grp_flags,
                                grp_size,
                                grp_done,
                                count,
                                elem,
                                status);
}

/*
 *  Check whether elements are free or not
 */
int
shr_res_check_all(int unit,
                  int res_id,
                  int count,
                  int elem)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_check_all(thisUnit, res_id, count, elem);
}

/*
 *  Check whether elements in a sparse block are free or not
 */
int
shr_res_check_all_sparse(int unit,
                         int res_id,
                         uint32 pattern,
                         int length,
                         int repeats,
                         int elem)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_check_all_sparse(thisUnit,
                                     res_id,
                                     pattern,
                                     length,
                                     repeats,
                                     elem);
}

/*
 *  Check a bunch of elements/blocks of a resource type
 */
int
shr_res_check_all_group(int unit,
                        int res_id,
                        uint32 grp_flags,
                        int grp_size,
                        int *grp_done,
                        const int *count,
                        const int *elem,
                        int *status)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_check_all_group(thisUnit,
                                    res_id,
                                    grp_flags,
                                    grp_size,
                                    grp_done,
                                    count,
                                    elem,
                                    status);
}

/*
 *  Check whether elements are free or not
 */
int
shr_res_check_all_tag(int unit,
                      int res_id,
                      const void *tag,
                      int count,
                      int elem)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_check_all_tag(thisUnit, res_id, tag, count, elem);
}

/*
 *  Check a bunch of elements/blocks of a resource type
 */
int
shr_res_check_all_tag_group(int unit,
                            int res_id,
                            uint32 grp_flags,
                            int grp_size,
                            int *grp_done,
                            const void **tag,
                            const int *count,
                            const int *elem,
                            int *status)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_check_all_tag_group(thisUnit,
                                        res_id,
                                        grp_flags,
                                        grp_size,
                                        grp_done,
                                        tag,
                                        count,
                                        elem,
                                        status);
}

/*
 *  Diagnostic dump
 */
int
shr_res_dump(int unit)
{
    _shr_res_unit_desc_t *thisUnit;

    /* a little parameter checking */
    RES_UNIT_CHECK(unit, thisUnit);

    /* perform the action */
    return shr_mres_dump(thisUnit);
}

/*****************************************************************************/
/*
 *  Interface to shr_res_bitmap (external implementation)
 */

/*
 *  shr_res_bitmap is a fairly simple bitmap allocator that supports a number
 *  of features, but isn't terribly optimised.  It allows alignment and offset,
 *  blocks of multiple elements (arbitrary size).  It has a few other minor
 *  upgrades versus simple bitmap, including keeping track of the next place
 *  where it wants to search for free elements and the last place where it
 *  freed elements, to improve performance under simple conditions, and which
 *  are designed to not have significant negative impact even when they fail to
 *  provide improvements to performance.
 */
static int
_shr_res_bitmap_create(_shr_res_pool_desc_t **desc,
                       int low_id,
                       int count,
                       const void* extras,
                       const char* name)
{
    shr_res_bitmap_handle_t handle;
    int result;
    int len_name = sal_strlen(name);

    /* need the base descriptor */
    *desc = sal_alloc(sizeof(_shr_res_pool_desc_t) + len_name,
                      "bitmap resource descriptor");
    if (!(*desc)) {
        /* alloc failed */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to allocate %u bytes for descriptor\n"),
                   (unsigned int)(sizeof(_shr_res_pool_desc_t) +
                   sal_strlen(name))));
        return _SHR_E_MEMORY;
    }
    sal_memset(*desc, 0x00, sizeof(_shr_res_pool_desc_t) + sal_strlen(name));
    (*desc)->count = count;
    (*desc)->low = low_id;
    (*desc)->extras = NULL; /* don't need this here */
    sal_strncpy(&((*desc)->name[0]), name, len_name);
    if (len_name)
        *((char*)&((*desc)->name[0])+len_name) = '\0';

    /* create the bitmap allocator */
    result = shr_res_bitmap_create(&handle, low_id, count);
    if (_SHR_E_NONE == result) {
        (*desc)->resHandle = handle;
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to create bitmap allocator, low_id = %d,"
                   " count = %d\n"),
                   low_id,
                   count));
        sal_free(*desc);
        *desc = NULL;
    }
    return result;
}

static int
_shr_res_bitmap_destroy(_shr_res_pool_desc_t *desc)
{
    shr_res_bitmap_handle_t handle = desc->resHandle;
    int result;

    /* destroy the list */
    result = shr_res_bitmap_destroy(handle);
    if(desc->extras) {
        sal_free(desc->extras);
    }
    if (_SHR_E_NONE == result) {
        /* free the descriptor */
        sal_free(desc);
        /* that usually crashes if something is wrong */
    }
    return result;
}

static int
_shr_res_bitmap_alloc(_shr_res_pool_desc_t *desc,
                      uint32 flags,
                      int count,
                      int *elem)
{
    shr_res_bitmap_handle_t handle = desc->resHandle;
    uint32 iflags = 0;

    if (flags & SHR_RES_ALLOC_WITH_ID) {
        iflags |= SHR_RES_BITMAP_ALLOC_WITH_ID;
    }
    if (flags & SHR_RES_ALLOC_REPLACE) {
        iflags |= SHR_RES_BITMAP_ALLOC_REPLACE;
    }
    return shr_res_bitmap_alloc(handle, iflags, count, elem);
}

static int
_shr_res_bitmap_alloc_align(_shr_res_pool_desc_t *desc,
                            uint32 flags,
                            int align,
                            int offs,
                            int count,
                            int *elem)
{
    shr_res_bitmap_handle_t handle = desc->resHandle;
    uint32 iflags = 0;

    if (flags & SHR_RES_ALLOC_WITH_ID) {
        iflags |= SHR_RES_BITMAP_ALLOC_WITH_ID;
    }
    if (flags & SHR_RES_ALLOC_ALIGN_ZERO) {
        iflags |= SHR_RES_BITMAP_ALLOC_ALIGN_ZERO;
    }
    if (flags & SHR_RES_ALLOC_REPLACE) {
        iflags |= SHR_RES_BITMAP_ALLOC_REPLACE;
    }
    return shr_res_bitmap_alloc_align(handle,
                                      iflags,
                                      align,
                                      offs,
                                      count,
                                      elem);
}

static int
_shr_res_bitmap_alloc_align_sparse(_shr_res_pool_desc_t *desc,
                                   uint32 flags,
                                   int align,
                                   int offs,
                                   uint32 pattern,
                                   int length,
                                   int repeats,
                                   int *elem)
{
    shr_res_bitmap_handle_t handle = desc->resHandle;
    uint32 iflags = 0;

    if (flags & SHR_RES_ALLOC_WITH_ID) {
        iflags |= SHR_RES_BITMAP_ALLOC_WITH_ID;
    }
    if (flags & SHR_RES_ALLOC_ALIGN_ZERO) {
        iflags |= SHR_RES_BITMAP_ALLOC_ALIGN_ZERO;
    }
    if (flags & SHR_RES_ALLOC_REPLACE) {
        iflags |= SHR_RES_BITMAP_ALLOC_REPLACE;
    }
    return shr_res_bitmap_alloc_align_sparse(handle,
                                             flags,
                                             align,
                                             offs,
                                             pattern,
                                             length,
                                             repeats,
                                             elem);
}

static int
_shr_res_bitmap_free(_shr_res_pool_desc_t *desc,
                     int count,
                     int elem)
{
    shr_res_bitmap_handle_t handle = desc->resHandle;

    return shr_res_bitmap_free(handle, count, elem);
}

static int
_shr_res_bitmap_free_sparse(_shr_res_pool_desc_t *desc,
                            uint32 pattern,
                            int length,
                            int repeats,
                            int elem)
{
    shr_res_bitmap_handle_t handle = desc->resHandle;

    return shr_res_bitmap_free_sparse(handle, pattern, length, repeats, elem);
}

static int
_shr_res_bitmap_check(_shr_res_pool_desc_t *desc,
                      int count,
                      int elem)
{
    shr_res_bitmap_handle_t handle = desc->resHandle;

    return shr_res_bitmap_check(handle, count, elem);
}

static int
_shr_res_bitmap_check_all_desc(_shr_res_pool_desc_t *desc,
                          int count,
                          int elem)
{
    shr_res_bitmap_handle_t handle = desc->resHandle;

    return shr_res_bitmap_check_all(handle, count, elem);
}

static int
_shr_res_bitmap_check_all_sparse(_shr_res_pool_desc_t *desc,
                                 uint32 pattern,
                                 int length,
                                 int repeats,
                                 int elem)
{
    shr_res_bitmap_handle_t handle = desc->resHandle;

    return shr_res_bitmap_check_all_sparse(handle,
                                           pattern,
                                           length,
                                           repeats,
                                           elem);
}

/*****************************************************************************/
/*
 *  Interface to shr_res_tag_bitmap (external implementation)
 */

/*
 *  shr_res_tag_bitmap is a fairly simple bitmap allocator that supports a
 *  number of features, but isn't terribly optimised.  It allows alignment and
 *  offset, blocks of multiple elements (arbitrary size).  It also optionally
 *  tracks tags (an arbitrary number of bytes assigned to each grain, where a
 *  grain is one or more elements -- a grain can only be shared between blocks
 *  if the blocks have same tag).  Disabling tagging renders it roughly
 *  equivalent to the shr_res_bitmap implementation, but perhaps a little bit
 *  less optimal.
 */
static int
_shr_res_tag_bitmap_create(_shr_res_pool_desc_t **desc,
                           int low_id,
                           int count,
                           const void* extras,
                           const char* name)
{
    shr_res_tag_bitmap_handle_t handle;
    shr_res_tagged_bitmap_extras_t *extra;
    int result;
    int len_name = sal_strlen(name);

    /* need the base descriptor */
    *desc = sal_alloc(sizeof(_shr_res_pool_desc_t) +
                      sizeof(shr_res_tagged_bitmap_extras_t) +
                      len_name,
                      "tagged bitmap resource descriptor");
    if (!(*desc)) {
        /* alloc failed */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to allocate %u bytes for descriptor\n"),
                   (unsigned int)(sizeof(_shr_res_pool_desc_t) +
                   sal_strlen(name))));
        return _SHR_E_MEMORY;
    }
    sal_memset(*desc, 0x00, sizeof(_shr_res_pool_desc_t) + sal_strlen(name));
    (*desc)->count = count;
    (*desc)->low = low_id;
    (*desc)->extras = sal_alloc(sizeof(*extra), "tagged bitmap extras");
    if (!((*desc)->extras)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to allocate %u bytes for extras\n"),
                   (unsigned int)(sizeof(*extra))));
        result = _SHR_E_MEMORY;
    } else {
        sal_strncpy(&((*desc)->name[0]), name, len_name);
        if (len_name)
            *((char*)&((*desc)->name[0])+len_name) = '\0';

        extra = (*desc)->extras;
        if (extras) {
            sal_memcpy((*desc)->extras, extras, sizeof(*extra));
        } else {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META("assuming zero tag length and one element"
                      " per grain, since no extras provided\n")));
            extra->grain_size = 1;
            extra->tag_length = 0;
        }
        /* create the bitmap allocator */
        result = shr_res_tag_bitmap_create(&handle,
                                           low_id,
                                           count,
                                           extra->grain_size,
                                           extra->tag_length);
        if (_SHR_E_NONE != result) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("unable to create tagged bitmap allocator:"
                       " %d (%s)\n"),
                       result,
                       _SHR_ERRMSG(result)));
        }
    }
    if (_SHR_E_NONE == result) {
        (*desc)->resHandle = handle;
    } else {
        if ((*desc)->extras) {
            sal_free((*desc)->extras);
        }
        sal_free(*desc);
        *desc = NULL;
    }
    return result;
}

static int
_shr_res_tag_bitmap_destroy(_shr_res_pool_desc_t *desc)
{
    shr_res_tag_bitmap_handle_t handle = desc->resHandle;
    int result;

    /* destroy the list */
    result = shr_res_tag_bitmap_destroy(handle);
    if (_SHR_E_NONE == result) {
        sal_free(desc->extras);
        /* free the descriptor */
        sal_free(desc);
        /* that usually crashes if something is wrong */
    }
    return result;
}

static int
_shr_res_tag_bitmap_alloc(_shr_res_pool_desc_t *desc,
                          uint32 flags,
                          int count,
                          int *elem)
{
    shr_res_tag_bitmap_handle_t handle = desc->resHandle;
    uint32 iflags = 0;

    if (flags & SHR_RES_ALLOC_WITH_ID) {
        iflags |= SHR_RES_TAG_BITMAP_ALLOC_WITH_ID;
    }
    if (flags & SHR_RES_ALLOC_REPLACE) {
        iflags |= SHR_RES_TAG_BITMAP_ALLOC_REPLACE;
    }
    return shr_res_tag_bitmap_alloc(handle, iflags, count, elem);
}

static int
_shr_res_tag_bitmap_alloc_tag(_shr_res_pool_desc_t *desc,
                              uint32 flags,
                              const void* tag,
                              int count,
                              int *elem)
{
    shr_res_tag_bitmap_handle_t handle = desc->resHandle;
    uint32 iflags = 0;

    if (flags & SHR_RES_ALLOC_WITH_ID) {
        iflags |= SHR_RES_TAG_BITMAP_ALLOC_WITH_ID;
    }
    if (flags & SHR_RES_ALLOC_REPLACE) {
        iflags |= SHR_RES_TAG_BITMAP_ALLOC_REPLACE;
    }
    return shr_res_tag_bitmap_alloc_tag(handle, iflags, tag, count, elem);
}

static int
_shr_res_tag_bitmap_alloc_align(_shr_res_pool_desc_t *desc,
                                uint32 flags,
                                int align,
                                int offs,
                                int count,
                                int *elem)
{
    shr_res_tag_bitmap_handle_t handle = desc->resHandle;
    uint32 iflags = 0;

    if (flags & SHR_RES_ALLOC_WITH_ID) {
        iflags |= SHR_RES_TAG_BITMAP_ALLOC_WITH_ID;
    }
    if (flags & SHR_RES_ALLOC_ALIGN_ZERO) {
        iflags |= SHR_RES_TAG_BITMAP_ALLOC_ALIGN_ZERO;
    }
    if (flags & SHR_RES_ALLOC_REPLACE) {
        iflags |= SHR_RES_TAG_BITMAP_ALLOC_REPLACE;
    }
    return shr_res_tag_bitmap_alloc_align(handle,
                                          iflags,
                                          align,
                                          offs,
                                          count,
                                          elem);
}

static int
_shr_res_tag_bitmap_alloc_align_tag(_shr_res_pool_desc_t *desc,
                                    uint32 flags,
                                    int align,
                                    int offs,
                                    const void *tag,
                                    int count,
                                    int *elem)
{
    shr_res_tag_bitmap_handle_t handle = desc->resHandle;
    uint32 iflags = 0;

    if (flags & SHR_RES_ALLOC_WITH_ID) {
        iflags |= SHR_RES_TAG_BITMAP_ALLOC_WITH_ID;
    }
    if (flags & SHR_RES_ALLOC_ALIGN_ZERO) {
        iflags |= SHR_RES_TAG_BITMAP_ALLOC_ALIGN_ZERO;
    }
    if (flags & SHR_RES_ALLOC_REPLACE) {
        iflags |= SHR_RES_TAG_BITMAP_ALLOC_REPLACE;
    }
    return shr_res_tag_bitmap_alloc_align_tag(handle,
                                              iflags,
                                              align,
                                              offs,
                                              tag,
                                              count,
                                              elem);
}

static int
_shr_res_tag_bitmap_free(_shr_res_pool_desc_t *desc,
                         int count,
                         int elem)
{
    shr_res_tag_bitmap_handle_t handle = desc->resHandle;

    return shr_res_tag_bitmap_free(handle, count, elem);
}

static int
_shr_res_tag_bitmap_check(_shr_res_pool_desc_t *desc,
                          int count,
                          int elem)
{
    shr_res_tag_bitmap_handle_t handle = desc->resHandle;

    return shr_res_tag_bitmap_check(handle, count, elem);
}

static int
_shr_res_tag_bitmap_check_all(_shr_res_pool_desc_t *desc,
                              int count,
                              int elem)
{
    shr_res_tag_bitmap_handle_t handle = desc->resHandle;

    return shr_res_tag_bitmap_check_all(handle, count, elem);
}

static int
_shr_res_tag_bitmap_check_all_tag(_shr_res_pool_desc_t *desc,
                                  const void *tag,
                                  int count,
                                  int elem)
{
    shr_res_tag_bitmap_handle_t handle = desc->resHandle;

    return shr_res_tag_bitmap_check_all_tag(handle, tag, count, elem);
}

/*****************************************************************************/
/*
 *  Interface to idxres (external implementation)
 */

/*
 *  idxres_fl is a folded freelist implementation that has roughly O(1) alloc,
 *  free, and check times, at the cost of about 8.1 bits per element.  It does
 *  not deal well with blocks of elements (unless everything works by a
 *  specific number of elements, and so can scale the list), though, and should
 *  not be asked to allocate blocks except WITH_ID, which unhappily has more
 *  like O(log128(n)) operation time where n is elements in the list.
 *
 *  When WITH_ID is provided, we can use the reserve function to specify the
 *  block requested, but in other cases, count must equal the scale else we
 *  will return _SHR_E_PARAM.
 *
 *  Since idxres checks arguments, fewer explicit argument checks are performed
 *  by these function than, for example, by the bitmap implementation above.
 */

static int
_shr_res_idxres_create(_shr_res_pool_desc_t **desc,
                       int low_id,
                       int count,
                       const void* extras,
                       const char* name)
{
    int result;
    int len_name = (sal_strlen(name));
    const shr_res_idxres_extras_t *info = extras;
    int namesize = ((len_name + 3) & (~3)); /* align to quadbyte */
    int nodesize = namesize + sizeof(**desc) + sizeof(*info);

    /* need the base descriptor */
    *desc = sal_alloc(nodesize, "idxres resource descriptor");
    if (!(*desc)) {
        /* alloc failed */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to allocate %d bytes for descriptor\n"),
                   nodesize));
        return _SHR_E_MEMORY;
    }
    sal_memset(*desc, 0x00, nodesize);
    (*desc)->count = count;
    (*desc)->low = low_id;
    (*desc)->extras = &((*desc)->name[namesize]);
    sal_strncpy(&((*desc)->name[0]), name, len_name);
    if (len_name)
        *((char*)&((*desc)->name[0])+len_name) = '\0';

    if (info) {
        if ((1 > info->scaling_factor)) {
            /* negative or zero scaling factors are ignored */
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META("invalid scaling factor %d; using 1 instead\n"),
                      info->scaling_factor));
            ((shr_res_idxres_extras_t*)((*desc)->extras))->scaling_factor = 1;
        } else {
            ((shr_res_idxres_extras_t*)((*desc)->extras))->scaling_factor = info->scaling_factor;
        }
    } else {
        /* no scaling factor provided */
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META("missing scaling factor; using 1\n")));
        ((shr_res_idxres_extras_t*)((*desc)->extras))->scaling_factor = 1;
    }
    if (1 == ((shr_res_idxres_extras_t*)((*desc)->extras))->scaling_factor) {
        result = shr_idxres_list_create((shr_idxres_list_handle_t*)&((*desc)->resHandle),
                                        low_id,
                                        low_id + count - 1,
                                        low_id,
                                        low_id + count - 1,
                                        "managed idxres");
    } else {
        result = shr_idxres_list_create_scaled((shr_idxres_list_handle_t*)&((*desc)->resHandle),
                                               low_id,
                                               low_id + count - 1,
                                               low_id,
                                               low_id + count - 1,
                                               ((shr_res_idxres_extras_t*)((*desc)->extras))->scaling_factor,
                                               "managed idxres (scaled)");
    }
    if (_SHR_E_NONE != result) {
        /* creation failed */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to create idxres(%d,%d,%d,%d,%d): %d (%s)\n"),
                   low_id,
                   low_id + count - 1,
                   low_id,
                   low_id + count - 1,
                   ((shr_res_idxres_extras_t*)((*desc)->extras))->scaling_factor,
                   result,
                   _SHR_ERRMSG(result)));
        sal_free(*desc);
        *desc = NULL;
    }
    /* return the result */
    return result;
}

static int
_shr_res_idxres_destroy(_shr_res_pool_desc_t *desc)
{
    int result;

    result = shr_idxres_list_destroy((shr_idxres_list_handle_t)(desc->resHandle));
    if (_SHR_E_NONE == result) {
        sal_free(desc);
    }
    return result;
}

static int
_shr_res_idxres_alloc(_shr_res_pool_desc_t *desc,
                      uint32 flags,
                      int count,
                      int *elem)
{
    int result;
    const shr_res_idxres_extras_t *info = desc->extras;
    shr_idxres_list_handle_t handle = desc->resHandle;
    shr_idxres_element_t item;

    if (SHR_RES_ALLOC_REPLACE & flags) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("REPLACE not yet supported on idxres\n")));
    }

    if (SHR_RES_ALLOC_WITH_ID & flags) {
        /* allocate WITH_ID */
        result = shr_idxres_list_reserve(handle,
                                         *elem,
                                         (*elem) + count - 1);
    } else {
        /* allocate next available */
        if (count > info->scaling_factor) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("tried to allocate %d elements from idxres list"
                       " of scaling_factor %d\n"),
                       count,
                       info->scaling_factor));
            return _SHR_E_PARAM;
        }
        result = shr_idxres_list_alloc(handle, &item);
        if (_SHR_E_NONE == result) {
            *elem = item;
        }
    }
    return result;
}

static int
_shr_res_idxres_free(_shr_res_pool_desc_t *desc,
                     int count,
                     int elem)
{
    int result = _SHR_E_NONE;
    const shr_res_idxres_extras_t *info = desc->extras;
    shr_idxres_list_handle_t handle = desc->resHandle;

    while ((_SHR_E_NONE == result) && (count > 0)) {
        result = shr_idxres_list_free(handle, elem);
        elem += info->scaling_factor;
        count -= info->scaling_factor;
    }
    if (_SHR_E_RESOURCE == result) {
        /* return NOT_FOUND instead */
        result = _SHR_E_NOT_FOUND;
    }
    return result;
}

static int
_shr_res_idxres_check(_shr_res_pool_desc_t *desc,
                      int count,
                      int elem)
{
    int result = _SHR_E_NOT_FOUND;
    shr_idxres_list_handle_t handle = desc->resHandle;

    while ((_SHR_E_NOT_FOUND == result) && (0 < count)) {
        result = shr_idxres_list_elem_state(handle, elem);
        elem++;
        count--;
    }
    return result;
}

static int
_shr_res_idxres_check_all(_shr_res_pool_desc_t *desc,
                          int count,
                          int elem)
{
    int freed = 0;
    int inuse = 0;
    int index;
    int result;
    shr_idxres_list_handle_t handle = desc->resHandle;

    for (index = 0; index < count; index++) {
        result = shr_idxres_list_elem_state(handle, elem + index);
        if (_SHR_E_NOT_FOUND == result) {
            freed++;
        } else if (_SHR_E_EXISTS == result) {
            inuse++;
        } else {
            /* unexpected result */
            return result;
        }
    }
    if (freed == count) {
        return _SHR_E_EMPTY;
    } else if (inuse == count) {
        return _SHR_E_FULL;
    } else {
        return _SHR_E_EXISTS;
    }
}

/*****************************************************************************/
/*
 *  Interface to aidxres (external implementation)
 */

/*
 *  aidxres_fl is a folded freelist implementation that has roughly O(1) alloc,
 *  free, and check times, at the cost of about 16.2 bits per element.  It
 *  deals reasonably well with blocks of elements but ends up aligning them to
 *  the next higher power of two (unless they are exactly a power of two
 *  already, then that size is also the alignment).  It suffers similar issues
 *  to idxres when used WITH_ID, but not quite at such a performance loss.
 *  Blocks will be aligned unless they are allocated WITH_ID, then it places
 *  the block as requested.
 *
 *  Note that the blocking factor has some effect on the performance; larger
 *  blocking factor means also larger memory footprint (more sublists).
 *  Despite these limitations, the default blocking factor is 7 if it is not
 *  provided by the caller, as this is the largest value supported in all of
 *  the normal operation modes (4b is *not* normal operation), and provides the
 *  ability to manage blocks up to 128 elements in size.
 *
 *  Since aidxres checks arguments, fewer argument checks are performed by
 *  these function than, for example, by the bitmap implementation above.
 */

static int
_shr_res_aidxres_create(_shr_res_pool_desc_t **desc,
                        int low_id,
                        int count,
                        const void* extras,
                        const char* name)
{
    int result;
    int len_name = sal_strlen(name);
    const shr_res_aidxres_extras_t *info = extras;
    int namesize = ((len_name + 3) & (~3)); /* align to quadbyte */
    int nodesize = namesize + sizeof(**desc) + sizeof(*info);

    /* need the base descriptor */
    *desc = sal_alloc(nodesize, "aidxres resource descriptor");
    if (!(*desc)) {
        /* alloc failed */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to allocate %d bytes for descriptor\n"),
                   nodesize));
        return _SHR_E_MEMORY;
    }
    sal_memset(*desc, 0x00, nodesize);
    (*desc)->count = count;
    (*desc)->low = low_id;
    (*desc)->extras = &((*desc)->name[namesize]);
    sal_strncpy(&((*desc)->name[0]), name, len_name);
    if (len_name)
        *((char*)&((*desc)->name[0])+len_name) = '\0';

    if (info) {
        if ((1 >= info->blocking_factor)) {
            /* negative or zero scaling factors are ignored */
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META("invalid blocking factor %d; using 7 instead\n"),
                      info->blocking_factor));
            ((shr_res_aidxres_extras_t*)((*desc)->extras))->blocking_factor = 7;
        } else {
            ((shr_res_aidxres_extras_t*)((*desc)->extras))->blocking_factor = info->blocking_factor;
        }
    } else {
        /* no scaling factor provided */
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META("missing blocking factor; using 7\n")));
        ((shr_res_aidxres_extras_t*)((*desc)->extras))->blocking_factor = 7;
    }
    result = shr_aidxres_list_create((shr_aidxres_list_handle_t*)&((*desc)->resHandle),
                                     low_id,
                                     low_id + count - 1,
                                     low_id,
                                     low_id + count - 1,
                                     ((shr_res_aidxres_extras_t*)((*desc)->extras))->blocking_factor,
                                     "managed aidxres");
    if (_SHR_E_NONE != result) {
        /* creation failed */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to create aidxres(%d,%d,%d,%d,%d): %d (%s)\n"),
                   low_id,
                   low_id + count - 1,
                   low_id,
                   low_id + count - 1,
                   ((shr_res_aidxres_extras_t*)((*desc)->extras))->blocking_factor,
                   result,
                   _SHR_ERRMSG(result)));
        sal_free(*desc);
        *desc = NULL;
    }
    /* return the result */
    return result;
}

static int
_shr_res_aidxres_destroy(_shr_res_pool_desc_t *desc)
{
    int result;

    result = shr_aidxres_list_destroy((shr_aidxres_list_handle_t)(desc->resHandle));
    if (_SHR_E_NONE == result) {
        sal_free(desc);
    }
    return result;
}

static int
_shr_res_aidxres_alloc(_shr_res_pool_desc_t *desc,
                       uint32 flags,
                       int count,
                       int *elem)
{
    int result;
    const shr_res_aidxres_extras_t *info = desc->extras;
    shr_aidxres_list_handle_t handle = desc->resHandle;
    shr_aidxres_element_t item;

    if (SHR_RES_ALLOC_REPLACE & flags) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("REPLACE not yet supported on aidxres\n")));
    }

    if (SHR_RES_ALLOC_WITH_ID & flags) {
        /* allocate WITH_ID */
        result = shr_aidxres_list_reserve(handle,
                                          *elem,
                                          (*elem) + count - 1);
    } else {
        /* allocate next available */
        if (count > (2 << info->blocking_factor)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("tried to allocate %d elements from idxres list"
                       " with blocking_factor %d\n"),
                       count,
                       info->blocking_factor));
            return _SHR_E_PARAM;
        }
        if (count > 1) {
            result = shr_aidxres_list_alloc_block(handle, count, &item);
        } else {
            result = shr_aidxres_list_alloc(handle, &item);
        }
        if (_SHR_E_NONE == result) {
            *elem = item;
        }
    }
    return result;
}

static int
_shr_res_aidxres_free(_shr_res_pool_desc_t *desc,
                      int count,
                      int elem)
{
    int result = _SHR_E_NONE;
    int xresult;
    shr_aidxres_list_handle_t handle = desc->resHandle;

    result = shr_aidxres_list_free(handle, elem);
    /*
     *  If it was a single block that was allocated, then it is all free, but
     *  if it was more than one block, or if it was marked by 'reserve' (so
     *  WITH_ID), then we need to traverse it.  Since we're freeing blocks as
     *  we come to them, any members of those blocks will already be free after
     *  the first element, but there isn't a way to know how many elements were
     *  in the block.  Basically, we need to skip any elements after the first
     *  that are marked NOT_FOUND (not in use) as if nothing was wrong.
     */
    if (_SHR_E_NONE == result) {
        count--;
        elem++;
        xresult = _SHR_E_NONE;
        while (((_SHR_E_NONE == xresult) || (_SHR_E_RESOURCE == xresult)) &&
               (count > 0)) {
            xresult = shr_aidxres_list_free(handle, elem);
            if ((_SHR_E_NONE != xresult) && (_SHR_E_RESOURCE != xresult)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META("element %d unable to free: %d (%s)\n"),
                           elem,
                           xresult,
                           _SHR_ERRMSG(xresult)));
                result = xresult;
            }
            elem++;
            count--;
        }
    }
    if (_SHR_E_RESOURCE == result) {
        /* return NOT_FOUND instead */
        result = _SHR_E_NOT_FOUND;
    }
    return result;
}

static int
_shr_res_aidxres_check(_shr_res_pool_desc_t *desc,
                       int count,
                       int elem)
{
    int result = _SHR_E_NOT_FOUND;
    shr_aidxres_list_handle_t handle = desc->resHandle;

    while ((_SHR_E_NOT_FOUND == result) && (0 < count)) {
        result = shr_aidxres_list_elem_state(handle, elem);
        elem++;
        count--;
    }
    return result;
}

static int
_shr_res_aidxres_check_all(_shr_res_pool_desc_t *desc,
                           int count,
                           int elem)
{
    shr_aidxres_list_handle_t handle = desc->resHandle;

    return shr_aidxres_list_block_state(handle, elem, count);
}

/*****************************************************************************/
/*
 *  Interface to mdb (external implementation)
 */

/*
 *  MDB has a lot of features that are not exposed through this interface, but
 *  it does improve upon aidxres for allowing greater versatility in terms of
 *  not requiring alignment of multiple element blocks, and it can track
 *  variable (non-power-of-two) size free block lists, in order to optimise
 *  allocation of certain sizes of blocks.  Its performance will be slightly
 *  worse than aidxres because mdb likes to combine neighbouring free blocks
 *  when allocating and freeing, and this causes roughly O(logm(b)) where m is
 *  the number of free lists additional overhead after the primary one and b is
 *  the size of the banks.  Note that poor choice of free list sizes (such as
 *  only using the mandatory single-element free list) will significantly
 *  imparir performance, degrading it as far as O(c*b) where c is the number of
 *  elements in a block.
 *
 *  The overhead for mdb is fairly close to 32.5 bits per element.  This may
 *  sound like rather a lot, but if you consider it against a traditional
 *  linked list, which requires an alloc cell per block (so 16 bytes of memory
 *  per block) and can't do query in O(1) time or best-fit as described above,
 *  it's still worthwhile in case where there are large resources to manage.
 *
 *  The mdb interface here also offers a lot more sanity checking than most of
 *  the other mechanisms.  This is because mdb allows information to be
 *  retrieved about a block of elements, and so free() and check() call
 *  parameters can be validated very specifically.
 *
 *  In order to take advantage of the other features of mdb, such as user
 *  lists, live adjustments to allocation mechanism, and so on, the mdb code
 *  should be used directly.
 */

static int
_shr_res_mdb_create(_shr_res_pool_desc_t **desc,
                    int low_id,
                    int count,
                    const void* extras,
                    const char* name)
{
    int result = _SHR_E_NONE;
    int len_name = sal_strlen(name);
    const shr_res_mdb_extras_t *info = extras;
    shr_res_mdb_extras_t *intInfo;
    int namesize = ((len_name + 3) & (~3)); /* align to quadbyte */
    int nodesize = namesize + sizeof(**desc) + sizeof(*info);
    int index;

    /* need the base descriptor */
    *desc = sal_alloc(nodesize, "mdb resource descriptor");
    if (!(*desc)) {
        /* alloc failed */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to allocate %d bytes for descriptor\n"),
                   nodesize));
        return _SHR_E_MEMORY;
    }
    sal_memset(*desc, 0x00, nodesize);
    (*desc)->count = count;
    (*desc)->low = low_id;
    (*desc)->extras = &((*desc)->name[namesize]);
    intInfo = (shr_res_mdb_extras_t*)((*desc)->extras);
    sal_strncpy(&((*desc)->name[0]), name, len_name);
    if (len_name)
        *((char*)&((*desc)->name[0])+len_name) = '\0';
    if (info) {
        /* Just copy the settings; the mdb manager will check things */
        intInfo->bank_size = info->bank_size;
        intInfo->free_lists = info->free_lists;
        for (index = 0; index < info->free_lists; index++) {
            intInfo->free_counts[index] = info->free_counts[index];
        }
    } else {
        /* no settings provided; pick a reasonable(?) default set */
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META("missing extra information; using defaults\n")));
        intInfo->bank_size = 4096;
        intInfo->free_lists = 12;
        intInfo->free_counts[0] = 2;
        intInfo->free_counts[1] = 4;
        intInfo->free_counts[2] = 8;
        intInfo->free_counts[3] = 16;
        intInfo->free_counts[4] = 32;
        intInfo->free_counts[5] = 64;
        intInfo->free_counts[6] = 128;
        intInfo->free_counts[7] = 256;
        intInfo->free_counts[8] = 512;
        intInfo->free_counts[9] = 1024;
        intInfo->free_counts[10] = 2048;
        intInfo->free_counts[11] = 4096;
    }
    result = shr_mdb_create((shr_mdb_list_handle_t*)&((*desc)->resHandle),
                            intInfo->bank_size,
                            intInfo->free_lists,
                            &(intInfo->free_counts[0]),
                            0 /* user lists */,
                            low_id,
                            low_id + count - 1,
                            FALSE /* no implied locking */);
    if (_SHR_E_NONE != result) {
        /* creation failed */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to create mdb(%d,%d,%d,%d,...): %d (%s)\n"),
                   low_id,
                   low_id + count - 1,
                   intInfo->bank_size,
                   intInfo->free_lists,
                   result,
                   _SHR_ERRMSG(result)));
        sal_free(*desc);
        *desc = NULL;
    } else {
        /* set the thing to use a reasonable alloc mode */
        result = shr_mdb_allocmode_set((shr_mdb_list_handle_t)((*desc)->resHandle),
                                       shr_mdb_alloc_bank_first |
                                       shr_mdb_alloc_block_low |
                                       shr_mdb_free_block_low |
                                       shr_mdb_join_alloc_and_free |
                                       shr_mdb_join_high_and_low);
    }
    /* return the result */
    return result;
}

static int
_shr_res_mdb_destroy(_shr_res_pool_desc_t *desc)
{
    int result;

    result = shr_mdb_destroy((shr_mdb_list_handle_t)(desc->resHandle));
    if (_SHR_E_NONE == result) {
        sal_free(desc);
    }
    return result;
}

static int
_shr_res_mdb_alloc(_shr_res_pool_desc_t *desc,
                   uint32 flags,
                   int count,
                   int *elem)
{
    shr_mdb_list_handle_t handle = desc->resHandle;
    shr_mdb_elem_index_t item;
    int result;

    if (SHR_RES_ALLOC_WITH_ID & flags) {
        /* allocate WITH_ID */
        /*
         *  More complicated than idxres, aidxress.  For starters, mdb honours
         *  BCM tendency to declare _SHR_E_NOT_FOUND when it would be more
         *  helpful for _SHR_E_PARAM -- trying to go beyond the end of a list.
         *  Therefore we need to check and provide the results documented for
         *  this API.
         */
        if (((*elem) < desc->low) || ((*elem) + count > desc->low + desc->count)) {
            return _SHR_E_PARAM;
        }
        if (SHR_RES_ALLOC_REPLACE & flags) {
            /* replacing, see if it is already there */
            result = shr_mdb_block_check_all(handle, *elem, count);
            switch (result) {
            case _SHR_E_FULL:
                /* matches existing block, consider it allocated (again) */
                result = _SHR_E_NONE;
                break;
            case _SHR_E_EMPTY:
                /* nothing here to reallocate */
                result = _SHR_E_NOT_FOUND;
                break;
            case _SHR_E_CONFIG:
            case _SHR_E_EXISTS:
                /* not all free or specified range includes >1 one block */
                result = _SHR_E_RESOURCE;
                break;
            default:
                /* should never see this */
                result = _SHR_E_INTERNAL;
            }
        } else {
            /* not replacing */
            result = shr_mdb_alloc_id(handle, *elem, count);
        }
    } else {
        if (SHR_RES_ALLOC_REPLACE & flags) {
            /* replace is not supported except WITH_ID */
            return _SHR_E_PARAM;
        }
        /* allocate next available */
        result = shr_mdb_alloc(handle, &item, count);
        if (_SHR_E_NONE == result) {
            *elem = item;
        }
    }
    return result;
}

static int
_shr_res_mdb_free(_shr_res_pool_desc_t *desc,
                  int count,
                  int elem)
{
    int result;
    shr_mdb_list_handle_t handle = desc->resHandle;
    shr_mdb_block_info_t info;

    /* Coverity :: 21913 */                          
    memset(&info, 0, sizeof(info));   

    /*
     *  More complicated than idxres, aidxress.  For starters, mdb honours BCM
     *  tendency to declare _SHR_E_NOT_FOUND when it would be more helpful for
     *  _SHR_E_PARAM -- trying to go beyond the end of a list.  Therefore we
     *  need to check and provide the results documented for this API.
     */
    if ((elem < desc->low) || (elem + count > desc->low + desc->count)) {
        return _SHR_E_PARAM;
    }
    /*
     *  Life gets interesting now.  In idxres and aidxres and even bitmap
     *  models, we had no way of getting the block size back from the starting
     *  element.  Here we do have that ability.  Ensure the block is the
     *  claimed size before freeing it.
     */
    result = shr_mdb_block_info(handle, elem, &info);
    if (_SHR_E_NONE == result) {
        if (info.size != count) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("freeing block size %d but claimed %d\n"),
                       info.size,
                       count));
            result = _SHR_E_FAIL;
        }
        if (info.head != (uint32)elem) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("freeing block with head %d by non-head"
                       " element %d\n"),
                       info.head,
                       elem));
            result = _SHR_E_FAIL;
        }
        if (_SHR_E_NONE == result) {
            result = shr_mdb_free(handle, elem);
        }
    }
    return result;
}

static int
_shr_res_mdb_check(_shr_res_pool_desc_t *desc,
                   int count,
                   int elem)
{
    int result = _SHR_E_NOT_FOUND;
    shr_mdb_list_handle_t handle = desc->resHandle;
    shr_mdb_block_info_t info;

    /* Coverity :: 21912 */
    memset(&info, 0, sizeof(info));

    /*
     *  More complicated than idxres, aidxres.  For starters, mdb honours BCM
     *  tendency to declare _SHR_E_NOT_FOUND when it would be more helpful for
     *  _SHR_E_PARAM -- trying to go beyond the end of a list.  Therefore we
     *  need to check and provide the results documented for this API.
     */
    if ((elem < desc->low) || (elem + count > desc->low + desc->count)) {
        return _SHR_E_PARAM;
    }
    /*
     *  Since this is meant to be a probe for elements in use, we don't want to
     *  simply bail out if the caller requests a block that is different in
     *  size than one that already exists, or if the caller's block is a member
     *  of an existing block that started elsewhere.  Just scan for elements
     *  that are in use and bail for those, otherwise keep going.
     */
    while ((count > 0) && (_SHR_E_NOT_FOUND == result)) {
        result = shr_mdb_block_info(handle, elem, &info);
        count--;
        elem++;
    }
    if (_SHR_E_NONE == result) {
        /* shr_mdb_block_info returns _SHR_E_NONE if it finds a block */
        result = _SHR_E_EXISTS;
    }
    return result;
}

static int
_shr_res_mdb_check_all(_shr_res_pool_desc_t *desc,
                       int count,
                       int elem)
{
    shr_mdb_list_handle_t handle = desc->resHandle;

    return shr_mdb_block_check_all(handle, count, elem);
}


