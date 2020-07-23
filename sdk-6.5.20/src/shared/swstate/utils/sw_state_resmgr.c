/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Global resource allocator
 */

#if defined(BCM_PETRA_SUPPORT)
/* { */
#include <shared/bsl.h>

#include <sal/core/sync.h>
#include <sal/core/alloc.h>
#include <shared/error.h>
#include <shared/swstate/sw_state_resmgr.h>
#include <shared/swstate/sw_state_res_tag_bitmap.h>
#include <include/shared/swstate/access/sw_state_access.h>


#include <soc/cm.h>

/*****************************************************************************/
/*
 *  Internal implementation
 */

/* Sw state access macro. */
#define RESMGR_ACCESS sw_state_access[unit].dpp.shr.resmgr_info

/*
 *  Various function prototypes per method for the alloc managers.
 */
typedef int (*_sw_state_res_alloc_create)(int unit, 
                                     int pool_id,
                                     int low_id,
                                     int count,
                                     const void *extras,
                                     const char *name);
typedef int (*_sw_state_res_alloc_destroy)(int unit, 
                                     int pool_id);
typedef int (*_sw_state_res_alloc_backup_pool)(int unit,
                                               int pool_id);
typedef int (*_sw_state_res_alloc_restore_backup_pool)(int unit,
                                               int pool_id);
typedef int (*_sw_state_res_alloc_alloc)(int unit, 
                                    int pool_id,
                                    uint32 flags,
                                    int count,
                                    int *elem);
typedef int (*_sw_state_res_alloc_tag)(int unit, 
                                  int pool_id,
                                  uint32 flags,
                                  const void* tag,
                                  int count,
                                  int *elem);
typedef int (*_sw_state_res_alloc_align)(int unit, 
                                    int pool_id,
                                    uint32 flags,
                                    int align,
                                    int offset,
                                    int count,
                                    int *elem);
typedef int (*_sw_state_res_alloc_align_sparse)(int unit, 
                                           int pool_id,
                                           uint32 flags,
                                           int align,
                                           int offs,
                                           uint32 pattern,
                                           int length,
                                           int repeats,
                                           int *elem);
typedef int (*_sw_state_res_alloc_align_tag)(int unit, 
                                        int pool_id,
                                        uint32 flags,
                                        int align,
                                        int offset,
                                        const void* tag,
                                        int count,
                                        int *elem);
typedef int (*_sw_state_res_alloc_free)(int unit, 
                                   int pool_id,
                                   int count,
                                   int elem);
typedef int (*_sw_state_res_alloc_free_sparse)(int unit, 
                                          int pool_id,
                                          uint32 pattern,
                                          int length,
                                          int repeats,
                                          int elem);
typedef int (*_sw_state_res_alloc_check)(int unit, 
                                    int pool_id,
                                    int count,
                                    int elem);
typedef int (*_sw_state_res_alloc_check_all)(int unit, 
                                        int pool_id,
                                        int count,
                                        int elem);
typedef int (*_sw_state_res_alloc_check_all_sparse)(int unit, 
                                               int pool_id,
                                               uint32 pattern,
                                               int length,
                                               int repeats,
                                               int elem);
typedef int (*_sw_state_res_alloc_check_all_tag)(int unit, 
                                            int pool_id,
                                            const void *tag,
                                            int count,
                                            int elem);
typedef int (*_sw_state_res_alloc_tag_set)(int unit,
                                           int pool_id,
                                           int elemIndex,
                                           int elemCount,
                                           const void* tag);
typedef int (*_sw_state_res_alloc_tag_get) (int unit,
                                            int pool_id,
                                            int element,
                                            const void* tag);
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
typedef struct _sw_state_res_alloc_mgr_s {
    _sw_state_res_alloc_create create;                /* mandatory */
    _sw_state_res_alloc_destroy destroy;              /* mandatory */
    _sw_state_res_alloc_alloc alloc;                  /* mandatory */
    _sw_state_res_alloc_tag tag;                      /* optional */
    _sw_state_res_alloc_align align;                  /* optional */
    _sw_state_res_alloc_align_sparse align_sparse;    /* optional */
    _sw_state_res_alloc_align_tag tag_align;          /* optional */
    _sw_state_res_alloc_free free;                    /* mandatory */
    _sw_state_res_alloc_free_sparse free_sparse;      /* iff have align_sparse */
    _sw_state_res_alloc_check check;                  /* mandatory */
    _sw_state_res_alloc_check_all check_all;          /* mandatory */
    _sw_state_res_alloc_check_all_sparse c_a_sparse;  /* iff have align_sparse */
    _sw_state_res_alloc_check_all_tag check_all_tag;  /* iff have tag or tag_align */
    _sw_state_res_alloc_tag_set tag_set;
    _sw_state_res_alloc_tag_get tag_get; 
    char *name;
} _sw_state_res_alloc_mgr_t;

/*
 *  These prototypes are for the global const structure below that points to
 *  all of the various implementations.
 */
static int _sw_state_res_bitmap_create(int unit, 
                                  int pool_id,
                                  int low_id,
                                  int count,
                                  const void* extras,
                                  const char* name);
static int _sw_state_res_bitmap_destroy(int unit, 
                                   int pool_id);
static int _sw_state_res_bitmap_alloc(int unit, 
                                 int pool_id,
                                 uint32 flags,
                                 int count,
                                 int *elem);
static int _sw_state_res_bitmap_alloc_align(int unit, 
                                       int pool_id,
                                       uint32 flags,
                                       int align,
                                       int offs,
                                       int count,
                                       int *elem);
static int _sw_state_res_bitmap_alloc_align_sparse(int unit, 
                                              int pool_id,
                                              uint32 flags,
                                              int align,
                                              int offs,
                                              uint32 pattern,
                                              int length,
                                              int repeats,
                                              int *elem);
static int _sw_state_res_bitmap_free(int unit, 
                                int pool_id,
                                int count,
                                int elem);
static int _sw_state_res_bitmap_free_sparse(int unit, 
                                       int pool_id,
                                       uint32 pattern,
                                       int length,
                                       int repeats,
                                       int elem);
static int _sw_state_res_bitmap_check(int unit, 
                                 int pool_id,
                                 int count,
                                 int elem);
static int _sw_state_res_bitmap_check_all_desc(int unit, 
                                          int pool_id,
                                          int    count,
                                          int elem);
static int _sw_state_res_bitmap_check_all_sparse(int unit, 
                                            int pool_id,
                                            uint32 pattern,
                                            int length,
                                            int repeats,
                                            int elem);
static int _sw_state_res_tag_bitmap_create(int unit, 
                                      int pool_id,
                                      int low_id,
                                      int count,
                                      const void* extras,
                                      const char* name);
static int _sw_state_res_tag_bitmap_destroy(int unit, 
                                       int pool_id);
static int _sw_state_res_tag_bitmap_alloc(int unit, 
                                     int pool_id,
                                     uint32 flags,
                                     int count,
                                     int *elem);
static int _sw_state_res_tag_bitmap_alloc_tag(int unit, 
                                         int pool_id,
                                         uint32 flags,
                                         const void *tag,
                                         int count,
                                         int *elem);
static int _sw_state_res_tag_bitmap_alloc_align(int unit, 
                                           int pool_id,
                                           uint32 flags,
                                           int align,
                                           int offs,
                                           int count,
                                           int *elem);
static int _sw_state_res_tag_bitmap_alloc_align_tag(int unit, 
                                               int pool_id,
                                               uint32 flags,
                                               int align,
                                               int offs,
                                               const void *tag,
                                               int count,
                                               int *elem);
static int _sw_state_res_tag_bitmap_free(int unit, 
                                    int pool_id,
                                    int count,
                                    int elem);
static int _sw_state_res_tag_bitmap_check(int unit, 
                                     int pool_id,
                                     int count,
                                     int elem);
static int _sw_state_res_tag_bitmap_check_all(int unit, 
                                         int pool_id,
                                         int count,
                                         int elem);
static int _sw_state_res_tag_bitmap_check_all_tag(int unit, 
                                             int pool_id,
                                             const void *tag,
                                             int count,
                                             int elem);
static int
_sw_state_res_tag_set(int unit,
                     int pool_id,
                     int offset,
                     int count,
                     const void* tag);
static int
_sw_state_res_tag_get(int unit,
                     int pool_id,
                     int element,
                     const void* tag);
int
sw_state_mres_pool_unset(int unit,
                    int pool_id);

int
sw_state_mres_free(int unit,
                     int res_id,
                     int count,
                     int elem);

/*
 *  Global const structure describing the various allocator mechanisms.
 */
static const _sw_state_res_alloc_mgr_t _sw_state_res_alloc_mgrs[SW_STATE_RES_ALLOCATOR_COUNT] =
    {
        {
            _sw_state_res_bitmap_create,
            _sw_state_res_bitmap_destroy,
            _sw_state_res_bitmap_alloc,
            NULL,
            _sw_state_res_bitmap_alloc_align,
            _sw_state_res_bitmap_alloc_align_sparse,
            NULL,
            _sw_state_res_bitmap_free,
            _sw_state_res_bitmap_free_sparse,
            _sw_state_res_bitmap_check,
            _sw_state_res_bitmap_check_all_desc,
            _sw_state_res_bitmap_check_all_sparse,
            NULL,
            NULL,
            NULL,
            "SW_STATE_RES_ALLOCATOR_BITMAP"
        } /* bitmap */,
        {
            _sw_state_res_tag_bitmap_create,
            _sw_state_res_tag_bitmap_destroy,
            _sw_state_res_tag_bitmap_alloc,
            _sw_state_res_tag_bitmap_alloc_tag,
            _sw_state_res_tag_bitmap_alloc_align,
            NULL,
            _sw_state_res_tag_bitmap_alloc_align_tag,
            _sw_state_res_tag_bitmap_free,
            NULL,
            _sw_state_res_tag_bitmap_check,
            _sw_state_res_tag_bitmap_check_all,
            NULL,
            _sw_state_res_tag_bitmap_check_all_tag,
            _sw_state_res_tag_set,
            _sw_state_res_tag_get,
            "SW_STATE_RES_ALLOCATOR_TAGGED_BITMAP"
        } /* tagged bitmap */
    };

/*
 *  Basic checks performed for many functions
 */
#define RES_UNIT_CHECK(unit) \
    {                           \
        uint8 _exists;   \
        if ((0 > (unit)) || (BCM_LOCAL_UNITS_MAX <= (unit))) { \
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, \
                      (BSL_META("invalid unit number %d\n"), \
                       unit)); \
            return _SHR_E_PARAM; \
        } \
        /* Check if unit has been initailized by checking nof res types*/ \
        if (RESMGR_ACCESS.is_allocated(unit, &_exists)) { return _SHR_E_INTERNAL;} \
        if (!_exists) { \
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, \
                      (BSL_META("unit %d is not initialised\n"), \
                       unit)); \
            return _SHR_E_INIT; \
        } \
    }
#define RES_POOL_VALID_CHECK(unit, _pool) \
    {   \
        uint16 _resPoolCount;  \
        if (RESMGR_ACCESS.resPoolCount.get(unit, &_resPoolCount)) { return _SHR_E_INTERNAL;} \
        if ((0 > (_pool)) || (_resPoolCount <= (_pool))) { \
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, \
                      (BSL_META("unit %d: pool %d does not exist\n"), \
                       unit, _pool)); \
            return _SHR_E_PARAM; \
       } \
    }

#define RES_POOL_EXIST_CHECK(unit, _pool) \
    {   \
        uint8 _exists;  \
        if (RESMGR_ACCESS.pool.is_allocated(unit, _pool, &_exists)) { return _SHR_E_INTERNAL;} \
        if (!_exists) { \
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, \
                      (BSL_META("unit %d: %d is not configured\n"), \
                       unit, _pool)); \
            return _SHR_E_CONFIG; \
        } \
   }
#define RES_TYPE_VALID_CHECK(unit, _type) \
    {   \
        uint16 _resTypeCount;  \
        if (RESMGR_ACCESS.resTypeCount.get(unit, &_resTypeCount)) { return _SHR_E_INTERNAL;} \
        if ((0 > (_type)) || (_resTypeCount <= (_type))) { \
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, \
                      (BSL_META("unit %d: resource %d does not exist\n"), \
                       unit, _type)); \
            return _SHR_E_PARAM; \
        }   \
    }
#define RES_TYPE_EXIST_CHECK(unit, _type) \
    {   \
        uint8 _exists;  \
        if (RESMGR_ACCESS.res.is_allocated(unit, _type, &_exists)) { return _SHR_E_INTERNAL;} \
        if (!_exists) { \
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, \
                      (BSL_META("unit %d resource %d is not configured\n"), \
                       unit, _type)); \
            return _SHR_E_CONFIG; \
        }   \
    }

#define SW_STATE_ACCESS_ERROR_CHECK(rv) \
    if (rv != _SHR_E_NONE) {             \
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,    \
                  (BSL_META("Error in sw state access\n")));    \
        return _SHR_E_INTERNAL;  \
    }

/*
 *  Destroys all of the resources and then pools for a unit.
 */
static int
_sw_state_mres_destroy_data(int unit)
{
    int i;
    int result = _SHR_E_NONE;
    uint8 exists;
    _sw_state_res_type_desc_t type;
    _sw_state_res_pool_desc_t pool;
    uint16 resTypeCount, resPoolCount;
    int refCount;

    /* If soc is detaching, SW state will be freed collectively. */
    if (!SOC_IS_DETACHING(unit)) {
        /* destroy resources */
        result = RESMGR_ACCESS.resTypeCount.get(unit, &resTypeCount);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        for (i = 0; i < resTypeCount; i++) {
            result = RESMGR_ACCESS.res.is_allocated(unit, i, &exists);
            SW_STATE_ACCESS_ERROR_CHECK(result);

            if (exists) {
                result = RESMGR_ACCESS.res.get(unit, i, &type);
                SW_STATE_ACCESS_ERROR_CHECK(result);
                if (type.refCount) {
                    LOG_WARN(BSL_LS_SHARED_SWSTATE,
                             (BSL_META("unit %d: type %d (%s): still in use (%d)\n"),
                              unit,
                              i,
                              (type.name.name),
                              type.refCount));
                }

                result = RESMGR_ACCESS.pool.refCount.get(unit, type.resPoolId, &refCount);
                SW_STATE_ACCESS_ERROR_CHECK(result);
                
                refCount--;

                result = RESMGR_ACCESS.pool.refCount.set(unit, type.resPoolId, refCount);
                SW_STATE_ACCESS_ERROR_CHECK(result);

                result = RESMGR_ACCESS.res.free(unit, i);
                SW_STATE_ACCESS_ERROR_CHECK(result);
            }
        } /* for (all resources this unit) */

        /* Fress the resources pointer array. */
        result = RESMGR_ACCESS.res.ptr_free(unit);
        SW_STATE_ACCESS_ERROR_CHECK(result);


        /* destroy pools */
        result = RESMGR_ACCESS.resPoolCount.get(unit, &resPoolCount);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        for (i = 0;
             (i < resPoolCount) && (_SHR_E_NONE == result);
             i++) {

            result = RESMGR_ACCESS.pool.is_allocated(unit, i, &exists);
            SW_STATE_ACCESS_ERROR_CHECK(result);

            if (exists) {
                result = RESMGR_ACCESS.pool.get(unit, i, &pool);
                SW_STATE_ACCESS_ERROR_CHECK(result);

                if (pool.refCount) {
                    LOG_WARN(BSL_LS_SHARED_SWSTATE,
                             (BSL_META("unit %d: %d (%s): unexpectedly still"
                              " in use (%d) - invalid condition???\n"),
                              unit,
                              i,
                              (pool.name.name),
                              pool.refCount));
                }
                result = _sw_state_res_alloc_mgrs[pool.resManagerType].destroy(unit, i);
                if (_SHR_E_NONE != result) {
                    LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                              (BSL_META("unit %d: pool %d (%s): unable to destroy:"
                               " %d (%s)\n"),
                               unit,
                               i,
                               (pool.name.name),
                               result,
                               _SHR_ERRMSG(result)));
                } /* if (_SHR_E_NONE != result) */

                result = RESMGR_ACCESS.pool.free(unit, i);
                SW_STATE_ACCESS_ERROR_CHECK(result);

            } /* if (unitData->pool[i]) */
        } /* for (all pools as long as no errors) */

        /* Free the pool resources array. */
        result = RESMGR_ACCESS.pool.ptr_free(unit);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        /* Free the SW state pointer. */

        result = RESMGR_ACCESS.free(unit);
        SW_STATE_ACCESS_ERROR_CHECK(result);
    }

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
sw_state_mres_create(int unit,
                int num_res_types,
                int num_res_pools)
{
    int result = _SHR_E_NONE;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %d) enter\n"),
               unit,
               num_res_types,
               num_res_pools));

    /* a little parameter checking */
    if (SOC_WARM_BOOT(unit)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("Can't create bitmap during init\n")));
        return _SHR_E_DISABLED;
    }
    if (1 > num_res_pools) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("resource pools %d; must be > 0\n"),
                   num_res_pools));
        result =  _SHR_E_PARAM;
    }
    if (1 > num_res_types) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("resource types %d; must be > 0\n"),
                   num_res_types));
        result =  _SHR_E_PARAM;
    }
    if (_SHR_E_NONE != result) {
        /* displayed diagnostics above */
        return result;
    }

    result = RESMGR_ACCESS.alloc(unit);
    if (result != _SHR_E_NONE) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("Unit %d: failed to allocate resmgr sw state.\n"),
                   unit));
    }

    /* set things up */
    result = RESMGR_ACCESS.res.ptr_alloc(unit, num_res_types);
    if (result != _SHR_E_NONE) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("Unit %d: failed to allocate resmgr sw state.\n"),
                   unit));
    }
    result = RESMGR_ACCESS.pool.ptr_alloc(unit, num_res_pools);
    if (result != _SHR_E_NONE) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("Unit %d: failed to allocate resmgr sw state.\n"),
                   unit));
    }

    result = RESMGR_ACCESS.resPoolCount.set(unit, num_res_pools);
        SW_STATE_ACCESS_ERROR_CHECK(result);

    result = RESMGR_ACCESS.resTypeCount.set(unit, num_res_types);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    /* Also initialize other bitmap types SW states here. */
    result = sw_state_res_tag_bitmap_init(unit, num_res_pools);
    if (result != _SHR_E_NONE) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("Failed to initialize tag bitmaps.\n")));
        return result;
    }

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(unit %d, num_res_types: %d, num_res_pools: %d) return %d (%s)\n"),
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
sw_state_mres_get(int unit,
             int *num_res_types,
             int *num_res_pools)
{
    int result;
    uint16 tmp;
    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %p, %p) enter\n"),
               unit,
               (void*)num_res_types,
               (void*)num_res_pools));

    /* a little parameter checking */
    /* return the requested information */
    if (num_res_pools) {
        result = RESMGR_ACCESS.resPoolCount.get(unit, &tmp);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        *num_res_pools = tmp;
    }

    if (num_res_types) {
        result = RESMGR_ACCESS.resTypeCount.get(unit, &tmp);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        *num_res_types = tmp;
    }

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, &(%d), &(%d)) return %d (%s)\n"),
               unit,
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
sw_state_mres_destroy(int unit)
{
    int result = _SHR_E_NONE;


    /* tear things down */
    result = _sw_state_mres_destroy_data(unit);

    return result;
}

/*
 *  Configure a resource pool on a unit
 */
int
sw_state_mres_pool_set(int unit,
                  int pool_id,
                  sw_state_res_allocator_t manager,
                  int low_id,
                  int count,
                  const void *extras,
                  const char *name)
{
    int result = _SHR_E_NONE;
    const char *noname = "???";
    uint8 exists = 0;
    int name_len;
    sw_state_res_name_t name_handle;
    const char **name_ptr = name? &name : &noname;

    sal_memset(&name_handle, 0, sizeof(name_handle));

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %s, %d, %d, %p, \"%s\") enter\n"),
               unit,
               pool_id,
               ((0 <= manager) && (SW_STATE_RES_ALLOCATOR_COUNT > manager))?_sw_state_res_alloc_mgrs[manager].name:"INVALID",
               low_id,
               count,
               (void*)extras,
               *name_ptr));

    /* a little parameter checking */
    if (SOC_WARM_BOOT(unit)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("Can't create bitmap during init\n")));
        return _SHR_E_DISABLED;
    }

    RES_POOL_VALID_CHECK(unit, pool_id);
    if ((0 > manager) || (SW_STATE_RES_ALLOCATOR_COUNT <= manager)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("allocation manager type %d not supported\n"),
                   manager));
        return _SHR_E_PARAM;
    }
    if (0 > count) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("negative counts are not permitted\n")));
        return _SHR_E_PARAM;
    }

    name_len = sal_strlen(*name_ptr);
    if (name_len>= SW_STATE_RES_NAME_MAX_LENGTH) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("Name string \"%s\" too long. Must be shorter than %d "
                            "(including terminal \\0)\n"),
                   name,
                   SW_STATE_RES_NAME_MAX_LENGTH));
        return _SHR_E_PARAM;
    }


    /* Check if pool exists, and if so, if it's already used by a type.
       If yes and no, copy the old pool and destroy it. */
    result = RESMGR_ACCESS.pool.is_allocated(unit, pool_id, &exists);
    SW_STATE_ACCESS_ERROR_CHECK(result);
    if (exists) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("unit %d: pool %d already exists. "
                            "Please free it before reallocating.\n"),
                   unit,
                   pool_id));
        return _SHR_E_CONFIG;
    } else {
        /* Allocate the pool */
        result = RESMGR_ACCESS.pool.alloc(unit, pool_id);
        SW_STATE_ACCESS_ERROR_CHECK(result);
    }
    
    /* create the new pool */
    result = _sw_state_res_alloc_mgrs[manager].create(unit, 
                                                 pool_id,
                                                 low_id,
                                                 count,
                                                 extras,
                                                 *name_ptr);
    if (_SHR_E_NONE == result) {
        /* New one created successfully. Fill its data. */
        result = RESMGR_ACCESS.pool.resManagerType.set(unit, pool_id, manager);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        result = RESMGR_ACCESS.pool.low.set(unit, pool_id, low_id);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        result = RESMGR_ACCESS.pool.count.set(unit, pool_id, count);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        sal_strcpy(name_handle.name, *name_ptr);

        result = RESMGR_ACCESS.pool.name.set(unit, pool_id, &name_handle);
        SW_STATE_ACCESS_ERROR_CHECK(result);
    } /* if (_SHR_E_NONE == result) */

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(unit: %d, %d, %s, %d, %d, %p, \"%s\") return %d (%s)\n"),
               unit,
               pool_id,
               _sw_state_res_alloc_mgrs[manager].name,
               low_id,
               count,
               (void*)extras,
               *name_ptr,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Destroy a resource pool on a unit
 */
int
sw_state_mres_pool_unset(int unit,
                    int pool_id)
{
    _sw_state_res_pool_desc_t oldPool;
    int result = _SHR_E_NONE;
    uint8 exists;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d) enter\n"),
               unit, pool_id));

    /* a little parameter checking */
    RES_POOL_VALID_CHECK(unit, pool_id);

    result = RESMGR_ACCESS.pool.is_allocated(unit, pool_id, &exists);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    if (exists) {

        result = RESMGR_ACCESS.pool.get(unit, pool_id, &oldPool);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        if (oldPool.refCount) {
            LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                      (BSL_META("unit %d: pool %d (%s) can not be destroyed because"
                       " it has %d types that use it\n"),
                       unit,
                       pool_id,
                       oldPool.name.name,
                       oldPool.refCount));
            result = _SHR_E_CONFIG;
        } else { /* if (oldPool->refCount) */
            result = _sw_state_res_alloc_mgrs[oldPool.resManagerType].destroy(unit, pool_id);
            if (_SHR_E_NONE != result) {
                LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                          (BSL_META("unit %d: unable to destroy old pool %d (%s):"
                           " %d (%s)\n"),
                           unit,
                           pool_id,
                           oldPool.name.name,
                           result,
                           _SHR_ERRMSG(result)));
            } else {
                result = RESMGR_ACCESS.pool.free(unit, pool_id);
                SW_STATE_ACCESS_ERROR_CHECK(result);
            }/* if (_SHR_E_NONE != result) */
        } /* if (oldPool.refCount) */
    } /* if (exists) */

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d) return %d (%s)\n"),
               unit,
               pool_id,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Get information about a resource pool on a unit
 */
int
sw_state_mres_pool_get(int unit,
                  int pool_id,
                  sw_state_res_allocator_t *manager,
                  int *low_id,
                  int *count,
                  sw_state_res_tagged_bitmap_extras_t *extras,
                  sw_state_res_name_t *name)
{
    int result;
    _sw_state_res_pool_desc_t thisPool;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %p, %p, %p, %p, %p) enter\n"),
               unit,
               pool_id,
               (void*)manager,
               (void*)low_id,
               (void*)count,
               (void*)extras,
               (void*)name));

    /* a little parameter checking */
    RES_POOL_VALID_CHECK(unit, pool_id);
    RES_POOL_EXIST_CHECK(unit, pool_id);
    /* fill in the caller's request */
    
    result = RESMGR_ACCESS.pool.get(unit, pool_id, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    if (manager) {
        *manager = thisPool.resManagerType;
    }
    if (low_id) {
        *low_id = thisPool.low;
    }
    if (count) {
        *count = thisPool.count;
    }
    if (extras) {
        *extras = thisPool.tagged_bmp_extras;
    }
    if (name) {
        *name = thisPool.name;
    }

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, &(%s), &(%d), &(%d), &(%d %d), &(\"%s\")) return %d (%s)\n"),
               unit,
               pool_id,
               manager?_sw_state_res_alloc_mgrs[*manager].name:"NULL",
               low_id?*low_id:0,
               count?*count:0,
               extras?extras->grain_size:0,
               extras?extras->tag_length:0,
               name?name->name:"NULL",
               _SHR_E_NONE,
               _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}

/*
 *  Get certain details about a pool on a unit
 */
int
sw_state_mres_pool_info_get(int unit,
                       int pool_id,
                       sw_state_res_pool_info_t *info)
{
    int result;
    _sw_state_res_pool_desc_t thisPool;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %p) enter\n"),
               unit,
               pool_id,
               (void*)info));

    /* a little parameter checking */
    RES_POOL_VALID_CHECK(unit, pool_id);
    RES_POOL_EXIST_CHECK(unit, pool_id);

    result = RESMGR_ACCESS.pool.get(unit, pool_id, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    /* fill in the caller's request */
    info->free = thisPool.count - thisPool.inuse;
    info->used = thisPool.inuse;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %p) return %d (%s)\n"),
               unit,
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
sw_state_mres_type_set(int unit,
                  int res_id,
                  int pool_id,
                  int elem_size,
                  const char *name)
{
    _sw_state_res_type_desc_t oldType;
    int result = _SHR_E_NONE;
    const char *noname = "???";
    uint8 exists;
    int refCount;
    sw_state_res_name_t name_handle;
    int name_length;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %d, %d, \"%s\") enter\n"),
               unit,
               res_id,
               pool_id,
               elem_size,
               name?name:noname));

    /* a little parameter checking */
    if (SOC_WARM_BOOT(unit)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("Can't create types during init\n")));
        return _SHR_E_DISABLED;
    }

    RES_POOL_VALID_CHECK(unit, pool_id);
    RES_POOL_EXIST_CHECK(unit, pool_id);
    RES_TYPE_VALID_CHECK(unit, res_id);
    if (1 > elem_size) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("element size %d too small; must be >= 1\n"),
                   elem_size));
        return _SHR_E_PARAM;
    }

    result = RESMGR_ACCESS.res.is_allocated(unit, res_id, &exists);
    SW_STATE_ACCESS_ERROR_CHECK(result);
    if (exists) {

        result = RESMGR_ACCESS.res.get(unit, res_id, &oldType);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        if (oldType.refCount) {
            LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                      (BSL_META("%d resource %d (%s) can not be changed"
                       " because it has %d elements in use\n"),
                       unit,
                       res_id,
                       oldType.name.name,
                       oldType.refCount));
            return _SHR_E_CONFIG;
        }
    }
    if (!name) {
        /* force a non-NULL name pointer */
        name = noname;
    } else {
        /* Check that name is not too long */
        name_length = sal_strlen(name);
        if (name_length >= SW_STATE_RES_NAME_MAX_LENGTH) {
            LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                      (BSL_META("Name string \"%s\" too long. Must be shorter than %d "
                                "(including terminal \\0)\n"),
                       name,
                       SW_STATE_RES_NAME_MAX_LENGTH));
            return _SHR_E_PARAM;
        }
    }


    /* Allocate the res memory. */
    if (!exists) {
        result = RESMGR_ACCESS.res.alloc(unit, res_id);
        SW_STATE_ACCESS_ERROR_CHECK(result);
    }

    result = RESMGR_ACCESS.res.resElemSize.set(unit, res_id, elem_size);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    result = RESMGR_ACCESS.res.resPoolId.set(unit, res_id, pool_id);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    sal_memset(&name_handle, 0, sizeof(name_handle));
    sal_memcpy(name_handle.name, name, ((strlen(name) + 1)  * sizeof(char)));
    
    result = RESMGR_ACCESS.res.name.set(unit, res_id, &name_handle);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    if (exists) {
        /* there was an old one; get rid of it and adjust references */
        result = RESMGR_ACCESS.pool.refCount.get(unit, oldType.resPoolId, &refCount);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        refCount--;

        result = RESMGR_ACCESS.pool.refCount.set(unit, oldType.resPoolId, refCount);
        SW_STATE_ACCESS_ERROR_CHECK(result);
    }

    /* adjust references and put this type in place */
    result = RESMGR_ACCESS.pool.refCount.get(unit, pool_id, &refCount);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    refCount++;

    result = RESMGR_ACCESS.pool.refCount.set(unit, pool_id, refCount);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %d, %d, \"%s\") return %d (%s)\n"),
               unit,
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
sw_state_mres_type_unset(int unit,
                    int res_id)
{
    _sw_state_res_type_desc_t oldType;
    int result = _SHR_E_NONE;
    uint8 exists;
    int refCount;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d) enter\n"),
               unit, res_id));

    /* a little parameter checking */
    RES_TYPE_VALID_CHECK(unit, res_id);
    
    result = RESMGR_ACCESS.res.is_allocated(unit, res_id, &exists);
    SW_STATE_ACCESS_ERROR_CHECK(result);
    if (exists) {
        result = RESMGR_ACCESS.res.get(unit, res_id, &oldType);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        if (oldType.refCount) {
            LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                      (BSL_META("%d resource %d (%s) can not be destroyed"
                       " because it has %d elements in use\n"),
                       unit,
                       res_id,
                       oldType.name.name,
                       oldType.refCount));
            result = _SHR_E_CONFIG;
        } else {
            result = RESMGR_ACCESS.pool.refCount.get(unit, oldType.resPoolId, &refCount);
            SW_STATE_ACCESS_ERROR_CHECK(result);

            refCount--;

            result = RESMGR_ACCESS.pool.refCount.set(unit, oldType.resPoolId, refCount);
            SW_STATE_ACCESS_ERROR_CHECK(result);

            result = RESMGR_ACCESS.res.free(unit, res_id);
            SW_STATE_ACCESS_ERROR_CHECK(result);
        }
    } /* if (oldType) */

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d) return %d (%s)\n"),
               unit,
               res_id,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Get information about a resource type
 */
int
sw_state_mres_type_get(int unit,
                  int res_id,
                  int *pool_id,
                  int *elem_size,
                  sw_state_res_name_t *name)
{
    _sw_state_res_type_desc_t thisType;
    int result;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %p, %p, %p) enter\n"),
               unit,
               res_id,
               (void*)pool_id,
               (void*)elem_size,
               (void*)name));
    
    /* a little parameter checking */
    RES_TYPE_VALID_CHECK(unit, res_id);
    RES_TYPE_EXIST_CHECK(unit, res_id);
    /* fill in the caller's request */
    result = RESMGR_ACCESS.res.get(unit, res_id, &thisType);
    SW_STATE_ACCESS_ERROR_CHECK(result);
    if (pool_id) {
        *pool_id = thisType.resPoolId;
    }
    if (elem_size) {
        *elem_size = thisType.resElemSize;
    }
    if (name) {
        *name = thisType.name;
    }

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, &(%d), &(%d), &(\"%s\")) return %d (%s)\n"),
               unit,
               res_id,
               pool_id?*pool_id:0,
               elem_size?*elem_size:0,
               name?name->name:"NULL",
               _SHR_E_NONE,
               _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}

/*
 *  Get information about a resource type
 */
int
sw_state_mres_type_info_get(int unit,
                       int res_id,
                       sw_state_res_type_info_t *info)
{
    int result;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %p) enter\n"),
               unit,
               res_id,
               (void*)info));

    /* a little parameter checking */
    RES_TYPE_VALID_CHECK(unit, res_id);
    RES_TYPE_EXIST_CHECK(unit, res_id);
    /* fill in the caller's request */
    if (info) {
        result = RESMGR_ACCESS.res.refCount.get(unit, res_id, &info->used);
        SW_STATE_ACCESS_ERROR_CHECK(result);
    }

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %p) return %d (%s)\n"),
               unit,
               res_id,
               (void*)info,
               _SHR_E_NONE,
               _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}


int
sw_state_mres_type_status_get(int unit,
                             int res_id){
    int result;
    int resElemSize, count, inuse;
    int pool_id;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d) enter\n"),
               unit,
               res_id));

    /* a little parameter checking */
    RES_TYPE_VALID_CHECK(unit, res_id);
    RES_TYPE_EXIST_CHECK(unit, res_id);
    result = RESMGR_ACCESS.res.resElemSize.get(unit, res_id, &resElemSize);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    result = RESMGR_ACCESS.res.resPoolId.get(unit, res_id, &pool_id);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    result = RESMGR_ACCESS.pool.count.get(unit, pool_id, &count);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    result = RESMGR_ACCESS.pool.inuse.get(unit, pool_id, &inuse);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    if (!inuse) {
        result = _SHR_E_EMPTY;
    } else if (count - inuse >= resElemSize) {
        /* There's free space. */
        result = _SHR_E_NONE;
    } else {
        result = _SHR_E_FULL;
    }
    

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d) return %d (%s)\n"),
               unit,
               res_id,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

STATIC int
sw_state_mres_ref_count_update(int unit,
                               int res_id,
                               int flags,
                               int count)
{
    int refCount;
    _sw_state_res_type_desc_t tempRes;
    int result = _SHR_E_NONE;

    if (0 == (flags & SW_STATE_RES_ALLOC_REPLACE)) {
        /* only account for alloc if new allocation */

        result = RESMGR_ACCESS.res.get(unit, res_id, &tempRes);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        /* Update resource refCount */
        refCount = tempRes.refCount + count;

        result = RESMGR_ACCESS.res.refCount.set(unit, res_id, refCount);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        /* Updates pool inuse. */
        result = RESMGR_ACCESS.pool.inuse.get(unit, tempRes.resPoolId, &refCount);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        refCount += count * tempRes.resElemSize;

        result = RESMGR_ACCESS.pool.inuse.set(unit, tempRes.resPoolId, refCount);
        SW_STATE_ACCESS_ERROR_CHECK(result);
    }

    return result;
}

/*
 *  Allocate elements of a resource type
 */
int
sw_state_mres_alloc(int unit,
               int res_id,
               uint32 flags,
               int count,
               int *elem)
{
    int result = _SHR_E_NONE;
    int scaled, resElemSize, pool_id;
    sw_state_res_allocator_t resManagerType;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, %p) enter\n"),
               unit,
               res_id,
               flags,
               count,
               (void*)elem));

    /* a little parameter checking */
    RES_TYPE_VALID_CHECK(unit, res_id);
    RES_TYPE_EXIST_CHECK(unit, res_id);
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("element count %d must be > 0\n"),
                   count));
        return _SHR_E_PARAM;
    }
    if (flags & (~SW_STATE_RES_ALLOC_SINGLE_FLAGS)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("invalid flags %08X\n"),
                   flags & (~SW_STATE_RES_ALLOC_SINGLE_FLAGS)));
        return _SHR_E_PARAM;
    }
    if (!elem) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("obligatory argument is NULL\n")));
        return _SHR_E_PARAM;
    }
    /* adjust element count per scaling factor */
    result = RESMGR_ACCESS.res.resElemSize.get(unit, res_id, &resElemSize);
    SW_STATE_ACCESS_ERROR_CHECK(result);
    scaled = count * resElemSize;

    result = RESMGR_ACCESS.res.resPoolId.get(unit, res_id, &pool_id);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    result = RESMGR_ACCESS.pool.resManagerType.get(unit, pool_id, &resManagerType);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    /* get the pool information */
    /* make the call */
    result = _sw_state_res_alloc_mgrs[resManagerType].alloc(unit, 
                                                             pool_id,
                                                             flags,
                                                             scaled,
                                                             elem);
    if (_SHR_E_NONE == result && !(flags & SW_STATE_RES_ALLOC_CHECK_ONLY)) {
        /* account for successful allocation */
        result = sw_state_mres_ref_count_update(unit,
                                                res_id,
                                                flags,
                                                count);
        SW_STATE_ACCESS_ERROR_CHECK(result);
    }

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, &(%d)) return %d (%s)\n"),
               unit,
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
sw_state_mres_alloc_group(int unit,
                     int res_id,
                     uint32 grp_flags,
                     int grp_size,
                     int *grp_done,
                     const uint32 *flags,
                     const int *count,
                     int *elem)
{
    int result = _SHR_E_NONE;
    int xresult;
    int index;
    uint32 blkFlags;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, %p, %p, %p, %p) enter\n"),
               unit,
               res_id,
               grp_flags,
               grp_size,
               (void*)grp_done,
               (void*)flags,
               (void*)count,
               (void*)elem));

    /* a little parameter checking */
    RES_TYPE_VALID_CHECK(unit, res_id);
    RES_TYPE_EXIST_CHECK(unit, res_id);
    if (!grp_done) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("obligatory out argument grp_done is NULL\n")));
        return _SHR_E_PARAM;
    }
    *grp_done = 0;
    if (0 > grp_size) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("group member count %d must be >= 0\n"),
                   grp_size));
        return _SHR_E_PARAM;
    }
    if ((0 < grp_size) && (!flags || !count || !elem)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("an obligatory array pointer is NULL\n")));
        return _SHR_E_PARAM;
    }
    if (grp_flags & (~(SW_STATE_RES_ALLOC_SINGLE_FLAGS | SW_STATE_RES_ALLOC_GROUP_FLAGS))) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("invalid group flags %08X\n"),
                   grp_flags & (~(SW_STATE_RES_ALLOC_SINGLE_FLAGS |
                   SW_STATE_RES_ALLOC_GROUP_FLAGS))));
        return _SHR_E_PARAM;
    }
    /* try to collect the requested blocks */
    for (index = 0;
         (_SHR_E_NONE == result) && (index < grp_size);
         index++) {

        /* Use the generic allocation function. */
        blkFlags = flags[index] | (grp_flags & SW_STATE_RES_ALLOC_SINGLE_FLAGS);

        result = sw_state_mres_alloc(unit, 
                                     res_id, 
                                     blkFlags, 
                                     count[index], 
                                     &(elem[index]));

        if (_SHR_E_NONE != result) {
            /* we'll hope the allocation manager displayed an error */
            /* don't want postincrement if an error occurred */
            break;
        }
    } /* for (all blocks in the caller's group) */
    if ((_SHR_E_NONE != result) && (grp_flags & SW_STATE_RES_ALLOC_GROUP_ATOMIC)) {
        /* atomic mode and it failed; back out everything that we have */
        /* index is at the first failure */
        while (index > 0) {
            /* look at previous index (must have been successful) */
            index--;
            blkFlags = flags[index] | (grp_flags & SW_STATE_RES_ALLOC_SINGLE_FLAGS);
            if (0 == (blkFlags & SW_STATE_RES_ALLOC_REPLACE)) {
                /* Free only blocks that were not trying to be replaced,
                   using the general dealloction function. */
                xresult = sw_state_mres_free(unit, 
                                             res_id, 
                                             count[index], 
                                             elem[index]);

                if (_SHR_E_NONE != xresult) {
                    LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                              (BSL_META("unable to back out unit %d resource %d"
                               " index %d base %d count %d: %d (%s)\n"),
                               unit,
                               res_id,
                               index,
                               elem[index],
                               count[index],
                               result,
                               _SHR_ERRMSG(result)));
                } 
            } /* if (0 == (blkFlags & SW_STATE_RES_ALLOC_REPLACE)) */
        } /* while (index > 0) */
    } /* if (error && SW_STATE_RES_ALLOC_GROUP_ATOMIC was set) */
    /* update number of allocations that succeeded */
    *grp_done = index;

    /* return the result */
    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, &(%d), %p, %p, %p) enter\n"),
               unit,
               res_id,
               grp_flags,
               grp_size,
               *grp_done,
               (void*)flags,
               (void*)count,
               (void*)elem));
    for (index = 0; index < grp_size; index++) {
        LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
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
sw_state_mres_alloc_tag(int unit,
                   int res_id,
                   uint32 flags,
                   const void *tag,
                   int count,
                   int *elem)
{
    _sw_state_res_pool_desc_t thisPool;
    _sw_state_res_type_desc_t thisType;
    int result = _SHR_E_NONE;
    int scaled;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %p, %d, %p) enter\n"),
               unit,
               res_id,
               flags,
               (void*)tag,
               count,
               (void*)elem));

    /* a little parameter checking */
    RES_TYPE_VALID_CHECK(unit, res_id);
    RES_TYPE_EXIST_CHECK(unit, res_id);
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("element count %d must be > 0\n"),
                   count));
        return _SHR_E_PARAM;
    }
    if (flags & (~SW_STATE_RES_ALLOC_SINGLE_FLAGS)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("invalid flags %08X\n"),
                   flags & (~SW_STATE_RES_ALLOC_SINGLE_FLAGS)));
        return _SHR_E_PARAM;
    }
    if (!elem) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("obligatory argument is NULL\n")));
        return _SHR_E_PARAM;
    }
    /* Get the type and pool information */
    result = RESMGR_ACCESS.res.get(unit, res_id, &thisType);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    result = RESMGR_ACCESS.pool.get(unit, thisType.resPoolId, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    /* adjust element count per scaling factor */
    scaled = count * thisType.resElemSize;


    /* make the call */
    if (_sw_state_res_alloc_mgrs[thisPool.resManagerType].tag) {
        /* allocator supports it; make the call */
        result = _sw_state_res_alloc_mgrs[thisPool.resManagerType].tag(unit, 
                                                                   thisType.resPoolId,
                                                                   flags,
                                                                   tag,
                                                                   scaled,
                                                                   elem);
    } else {
        /* not supported by this allocator */
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("allocator type %s does not support tagged alloc\n"),
                   _sw_state_res_alloc_mgrs[thisPool.resManagerType].name));
        result = _SHR_E_UNAVAIL;
    }
    if (_SHR_E_NONE == result && !(flags & SW_STATE_RES_ALLOC_CHECK_ONLY)) {
        /* account for successful allocation */
        result = sw_state_mres_ref_count_update(unit,
                                                res_id,
                                                flags,
                                                count);
        SW_STATE_ACCESS_ERROR_CHECK(result);

    }

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %p, %d, &(%d)) return %d (%s)\n"),
               unit,
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
sw_state_mres_alloc_tag_group(int unit,
                         int res_id,
                         uint32 grp_flags,
                         int grp_size,
                         int *grp_done,
                         const uint32 *flags,
                         const void **tag,
                         const int *count,
                         int *elem)
{
    int result = _SHR_E_NONE;
    int xresult;
    int index;
    uint32 blkFlags;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, %p, %p, %p, %p, %p) enter\n"),
               unit,
               res_id,
               grp_flags,
               grp_size,
               (void*)grp_done,
               (void*)flags,
               (void*)tag,
               (void*)count,
               (void*)elem));

    /* a little parameter checking */
    RES_TYPE_VALID_CHECK(unit, res_id);
    RES_TYPE_EXIST_CHECK(unit, res_id);
    if (!grp_done) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("obligatory out argument grp_done is NULL\n")));
        return _SHR_E_PARAM;
    }
    *grp_done = 0;
    if (0 > grp_size) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("group member count %d must be >= 0\n"),
                   grp_size));
        return _SHR_E_PARAM;
    }
    if ((0 < grp_size) && (!flags || !count || !elem)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("an obligatory array pointer is NULL\n")));
        return _SHR_E_PARAM;
    }
    if (grp_flags & (~(SW_STATE_RES_ALLOC_SINGLE_FLAGS | SW_STATE_RES_ALLOC_GROUP_FLAGS))) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("invalid group flags %08X\n"),
                   grp_flags & (~(SW_STATE_RES_ALLOC_SINGLE_FLAGS |
                   SW_STATE_RES_ALLOC_GROUP_FLAGS))));
        return _SHR_E_PARAM;
    }
    /* get the resource information */
    for (index = 0;
         (_SHR_E_NONE == result) && (index < grp_size);
         index++) {
        /* check parameters for this block */
        blkFlags = flags[index] | (grp_flags & SW_STATE_RES_ALLOC_SINGLE_FLAGS);
        /* make the call */
        result = sw_state_mres_alloc_tag(unit, 
                                         res_id, 
                                         blkFlags, 
                                         tag, 
                                         count[index], 
                                         &elem[index]);
        if (_SHR_E_NONE != result) {
            /* we'll hope the allocation manager displayed an error */
            /* don't want postincrement if an error occurred */
            break;
        }
    } /* for (all blocks in the caller's group) */
    if ((_SHR_E_NONE != result) && (grp_flags & SW_STATE_RES_ALLOC_GROUP_ATOMIC)) {
        /* atomic mode and it failed; back out everything that we have */
        /* index is at the first failure */
        while (index > 0) {
            /* look at previous index (must have been successful) */
            index--;
            blkFlags = flags[index] | (grp_flags & SW_STATE_RES_ALLOC_SINGLE_FLAGS);
            if (0 == (blkFlags & SW_STATE_RES_ALLOC_REPLACE)) {
                /* Free only blocks that were not trying to be replaced,
                   using the general dealloction function. */
                xresult = sw_state_mres_free(unit, 
                                             res_id, 
                                             count[index], 
                                             elem[index]);

                if (_SHR_E_NONE != xresult) {
                    LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                              (BSL_META("unable to back out unit %d resource %d"
                               " index %d base %d count %d: %d (%s)\n"),
                               unit,
                               res_id,
                               index,
                               elem[index],
                               count[index],
                               result,
                               _SHR_ERRMSG(result)));
                } 
            } /* if (0 == (blkFlags & SW_STATE_RES_ALLOC_REPLACE)) */
        } /* while (index > 0) */
    } /* if (error && SW_STATE_RES_ALLOC_GROUP_ATOMIC was set) */
    /* update number of allocations that succeeded */
    *grp_done = index;

    /* return the result */
    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, &(%d), %p, %p, %p, %p) enter\n"),
               unit,
               res_id,
               grp_flags,
               grp_size,
               *grp_done,
               (void*)flags,
               (void*)tag,
               (void*)count,
               (void*)elem));
    for (index = 0; index < grp_size; index++) {
        LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
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
sw_state_mres_alloc_align(int unit,
                     int res_id,
                     uint32 flags,
                     int align,
                     int offset,
                     int count,
                     int *elem)
{
    _sw_state_res_pool_desc_t thisPool;
    _sw_state_res_type_desc_t thisType;
    int result = _SHR_E_NONE;
    int base;
    int scaled;
    int scaledAlign;
    int scaledOffset;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, %d, %d, %p) enter\n"),
               unit,
               res_id,
               flags,
               align,
               offset,
               count,
               (void*)elem));

    /* a little parameter checking */
    RES_TYPE_VALID_CHECK(unit, res_id);
    RES_TYPE_EXIST_CHECK(unit, res_id);
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("element count %d must be > 0\n"),
                   count));
        return _SHR_E_PARAM;
    }
    if (1 > align) {
        LOG_WARN(BSL_LS_SHARED_SWSTATE,
                 (BSL_META("align <= 0 invalid, using align = 1 instead\n")));
        align = 1;
    }
    if ((offset >= align) || (0 > offset)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("offset %d must be >= 0 and < align %d\n"),
                   offset,
                   align));
        return _SHR_E_PARAM;
    }
    if (flags & (~SW_STATE_RES_ALLOC_SINGLE_FLAGS)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("invalid flags %08X\n"),
                   flags & (~SW_STATE_RES_ALLOC_SINGLE_FLAGS)));
        return _SHR_E_PARAM;
    }
    if (!elem) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("obligatory argument is NULL\n")));
        return _SHR_E_PARAM;
    }
    /* get the pool information */
    result = RESMGR_ACCESS.res.get(unit, res_id, &thisType);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    result = RESMGR_ACCESS.pool.get(unit, thisType.resPoolId, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    /* adjust element count per scaling factor */
    scaled = count * thisType.resElemSize;
    scaledAlign = align * thisType.resElemSize;
    scaledOffset = offset * thisType.resElemSize;

    /* check alignment for WITH_ID case */
    if (flags & SW_STATE_RES_ALLOC_WITH_ID) {
        if (flags & SW_STATE_RES_ALLOC_ALIGN_ZERO) {
            base = *elem;
        } else {
            base = *elem - thisPool.low;
        }
        if (((((base) / scaledAlign) * scaledAlign) + scaledOffset) != base) {
            LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                      (BSL_META("WITH_ID requested element %d does not comply"
                       " with alignment specifications\n"),
                       *elem));
            return _SHR_E_PARAM;
        }
    }
    if (_sw_state_res_alloc_mgrs[thisPool.resManagerType].align) {
        /* allocator supports it; make the call */
        result = _sw_state_res_alloc_mgrs[thisPool.resManagerType].align(unit, 
                                                                     thisType.resPoolId,
                                                                     flags,
                                                                     scaledAlign,
                                                                     scaledOffset,
                                                                     scaled,
                                                                     elem);
    } else {
        /* not supported by this allocator */
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("allocator type %s does not support aligned alloc\n"),
                   _sw_state_res_alloc_mgrs[thisPool.resManagerType].name));
        result = _SHR_E_UNAVAIL;
    }
    if (_SHR_E_NONE == result && !(flags & SW_STATE_RES_ALLOC_CHECK_ONLY)) {
        /* account for successful allocation */
        result = sw_state_mres_ref_count_update(unit,
                                                res_id,
                                                flags,
                                                count);
        SW_STATE_ACCESS_ERROR_CHECK(result);
    }

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, %d, %d, &(%d)) return %d (%s)\n"),
               unit,
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
sw_state_mres_alloc_align_group(int unit,
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
    _sw_state_res_pool_desc_t thisPool;
    _sw_state_res_type_desc_t thisRes;
    int result = _SHR_E_NONE;
    int xresult;
    int index;
    int xalign;
    uint32 blkFlags;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, %p, %p, %p, %p, %p, %p) enter\n"),
               unit,
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
    RES_TYPE_VALID_CHECK(unit, res_id);
    RES_TYPE_EXIST_CHECK(unit, res_id);
    if (!grp_done) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("obligatory out argument grp_done is NULL\n")));
        return _SHR_E_PARAM;
    }
    *grp_done = 0;
    if (0 > grp_size) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("group member count %d must be >= 0\n"),
                   grp_size));
        return _SHR_E_PARAM;
    }
    if ((0 < grp_size) && (!flags || !count || !elem || !align || !offset)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("an obligatory array pointer is NULL\n")));
        return _SHR_E_PARAM;
    }
    if (grp_flags & (~(SW_STATE_RES_ALLOC_SINGLE_FLAGS | SW_STATE_RES_ALLOC_GROUP_FLAGS))) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("invalid group flags %08X\n"),
                   grp_flags & (~(SW_STATE_RES_ALLOC_SINGLE_FLAGS |
                   SW_STATE_RES_ALLOC_GROUP_FLAGS))));
        return _SHR_E_PARAM;
    }
    /* get the resource information */
    result = RESMGR_ACCESS.res.get(unit, res_id, &thisRes);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    /* get the pool information */
    result = RESMGR_ACCESS.pool.get(unit, thisRes.resPoolId, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    /* try to collect the requested blocks */
    if (_sw_state_res_alloc_mgrs[thisPool.resManagerType].align) {
        /* allocator does not support this feature */
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("allocator type %s does not support aligned alloc\n"),
                   _sw_state_res_alloc_mgrs[thisPool.resManagerType].name));
        result = _SHR_E_UNAVAIL;
    }
    for (index = 0;
         (_SHR_E_NONE == result) && (index < grp_size);
         index++) {
        /* check parameters for this block */
        blkFlags = flags[index] | (grp_flags & SW_STATE_RES_ALLOC_SINGLE_FLAGS);
        if (1 > align[index]) {
            LOG_WARN(BSL_LS_SHARED_SWSTATE,
                     (BSL_META("align <= 0 invalid, using align = 1 instead\n")));
            xalign = 1;
        } else {
            xalign = align[index];
        }

        result = sw_state_mres_alloc_align(unit, res_id, blkFlags, xalign, offset[index], count[index], &elem[index]);

        if (_SHR_E_NONE != result) {
            /* we'll hope the allocation manager displayed an error */
            /* don't want postincrement if an error occurred */
            break;
        }
    } /* for (all blocks in the caller's group) */
    if ((_SHR_E_NONE != result) && (grp_flags & SW_STATE_RES_ALLOC_GROUP_ATOMIC)) {
        /* atomic mode and it failed; back out everything that we have */
        /* index is at the first failure */
        while (index > 0) {
            /* look at previous index (must have been successful) */
            index--;
            blkFlags = flags[index] | (grp_flags & SW_STATE_RES_ALLOC_SINGLE_FLAGS);
            if (0 == (blkFlags & SW_STATE_RES_ALLOC_REPLACE)) {
                /* Free only blocks that were not trying to be replaced,
                   using the general dealloction function. */
                xresult = sw_state_mres_free(unit, 
                                             res_id, 
                                             count[index], 
                                             elem[index]);

                if (_SHR_E_NONE != xresult) {
                    LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                              (BSL_META("unable to back out %d resource %d"
                               " index %d base %d count %d: %d (%s)\n"),
                               unit,
                               res_id,
                               index,
                               elem[index],
                               count[index],
                               result,
                               _SHR_ERRMSG(result)));
                } 
            } /* if (0 == (blkFlags & SW_STATE_RES_ALLOC_REPLACE)) */
        } /* while (index > 0) */
    } /* if (error && SW_STATE_RES_ALLOC_GROUP_ATOMIC was set) */
    /* update number of allocations that succeeded */
    *grp_done = index;

    /* return the result */
    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, &(%d), %p, %p, %p, %p, %p)"
               " return %d (%s)\n"),
               unit,
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
        LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
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
sw_state_mres_alloc_align_sparse(int unit,
                            int res_id,
                            uint32 flags,
                            int align,
                            int offset,
                            uint32 pattern,
                            int length,
                            int repeats,
                            int *elem)
{
    _sw_state_res_pool_desc_t thisPool;
    _sw_state_res_type_desc_t thisType;
    int result = _SHR_E_NONE;
    int count;
    int index;
    int base;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, %d, %08X, %d, %d, %p) enter\n"),
               unit,
               res_id,
               flags,
               align,
               offset,
               pattern,
               length,
               repeats,
               (void*)elem));

    /* a little parameter checking */
    RES_TYPE_VALID_CHECK(unit, res_id);
    RES_TYPE_EXIST_CHECK(unit, res_id);
    if (0 >= length) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("pattern length must be greater than zero\n")));
        return _SHR_E_PARAM;
    }
    if (32 < length) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("pattern length must be 32 or less\n")));
        return _SHR_E_PARAM;
    }
    if (0 >= repeats) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("repeat count must be greater than zero\n")));
        return _SHR_E_PARAM;
    }
    if (1 > align) {
        LOG_WARN(BSL_LS_SHARED_SWSTATE,
                 (BSL_META("align <= 0 invalid, using align = 1 instead\n")));
        align = 1;
    }
    if ((offset >= align) || (0 > offset)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("offset %d must be >= 0 and < align %d\n"),
                   offset,
                   align));
        return _SHR_E_PARAM;
    }
    if (flags & (~SW_STATE_RES_ALLOC_SINGLE_FLAGS)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("invalid flags %08X\n"),
                   flags & (~SW_STATE_RES_ALLOC_SINGLE_FLAGS)));
        return _SHR_E_PARAM;
    }
    if (!elem) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("obligatory argument is NULL\n")));
        return _SHR_E_PARAM;
    }
    /* get the pool information */
    result = RESMGR_ACCESS.res.get(unit, res_id, &thisType);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    result = RESMGR_ACCESS.pool.get(unit, thisType.resPoolId, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    if (1 != thisType.resElemSize) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("not compatible with scaled resources\n")));
        return _SHR_E_CONFIG;
    }
    /* check alignment for WITH_ID case */
    if (flags & SW_STATE_RES_ALLOC_WITH_ID) {
        if (flags & SW_STATE_RES_ALLOC_ALIGN_ZERO) {
            base = *elem;
        } else {
            base = *elem - thisPool.low;
        }
        if (((((base) / align) * align) + offset) != base) {
            LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                      (BSL_META("WITH_ID requested element %d does not comply"
                       " with alignment specifications\n"),
                       *elem));
            return _SHR_E_PARAM;
        }
    }
    if (_sw_state_res_alloc_mgrs[thisPool.resManagerType].align_sparse) {
        /* allocator supports it; make the call */
        result = _sw_state_res_alloc_mgrs[thisPool.resManagerType].align_sparse(unit, 
                                                                            thisType.resPoolId,
                                                                            flags,
                                                                            align,
                                                                            offset,
                                                                            pattern,
                                                                            length,
                                                                            repeats,
                                                                            elem);
    } else {
        /* not supported by this allocator */
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("allocator type %s does not support aligned"
                   " sparse alloc\n"),
                   _sw_state_res_alloc_mgrs[thisPool.resManagerType].name));
        result = _SHR_E_UNAVAIL;
    }
    if (_SHR_E_NONE == result && !(flags & SW_STATE_RES_ALLOC_CHECK_ONLY)) {
        /* account for successful allocation */
        if (0 == (flags & SW_STATE_RES_ALLOC_REPLACE)) {
            /* only account for alloc if new allocation */
            for (index = 0, count = 0; index < length; index++) {
                if (pattern & (1 << index)) {
                    count++;
                }
            }
            count *= repeats;

            result = sw_state_mres_ref_count_update(unit,
                                                    res_id,
                                                    flags,
                                                    count);
            SW_STATE_ACCESS_ERROR_CHECK(result);
        }
    }

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, %d, %08X, %d, %d, &(%d))"
               " return %d (%s)\n"),
               unit,
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
sw_state_mres_alloc_align_tag(int unit,
                         int res_id,
                         uint32 flags,
                         int align,
                         int offset,
                         const void *tag,
                         int count,
                         int *elem)
{
    _sw_state_res_pool_desc_t thisPool;
    _sw_state_res_type_desc_t thisType;
    int result = _SHR_E_NONE;
    int base;
    int scaled;
    int scaledAlign;
    int scaledOffset;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, %d, %p, %d, %p) enter\n"),
               unit,
               res_id,
               flags,
               align,
               offset,
               (void*)tag,
               count,
               (void*)elem));

    /* a little parameter checking */
    RES_TYPE_VALID_CHECK(unit, res_id);
    RES_TYPE_EXIST_CHECK(unit, res_id);
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("element count %d must be > 0\n"),
                   count));
        return _SHR_E_PARAM;
    }
    if (1 > align) {
        LOG_WARN(BSL_LS_SHARED_SWSTATE,
                 (BSL_META("align <= 0 invalid, using align = 1 instead\n")));
        align = 1;
    }
    if ((offset >= align) || (0 > offset)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("offset %d must be >= 0 and < align %d\n"),
                   offset,
                   align));
        return _SHR_E_PARAM;
    }
    if (flags & (~SW_STATE_RES_ALLOC_SINGLE_FLAGS)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("invalid flags %08X\n"),
                   flags & (~SW_STATE_RES_ALLOC_SINGLE_FLAGS)));
        return _SHR_E_PARAM;
    }
    if (!elem) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("obligatory argument is NULL\n")));
        return _SHR_E_PARAM;
    }
    /* get the pool information */
    result = RESMGR_ACCESS.res.get(unit, res_id, &thisType);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    result = RESMGR_ACCESS.pool.get(unit, thisType.resPoolId, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    /* adjust element count per scaling factor */
    scaled = count * thisType.resElemSize;
    scaledAlign = align * thisType.resElemSize;
    scaledOffset = offset * thisType.resElemSize;

    /* check alignment for WITH_ID case */
    if (flags & SW_STATE_RES_ALLOC_WITH_ID) {
        if (flags & SW_STATE_RES_ALLOC_ALIGN_ZERO) {
            base = *elem;
        } else {
            base = *elem - thisPool.low;
        }
        if (((((base) / scaledAlign) * scaledAlign) + scaledOffset) != base) {
            LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                      (BSL_META("WITH_ID requested element %d does not comply"
                       " with alignment specifications\n"),
                       *elem));
            return _SHR_E_PARAM;
        }
    }
    if (_sw_state_res_alloc_mgrs[thisPool.resManagerType].tag_align) {
        /* allocator supports it; make the call */
        result = _sw_state_res_alloc_mgrs[thisPool.resManagerType].tag_align(unit, 
                                                                         thisType.resPoolId,
                                                                         flags,
                                                                         scaledAlign,
                                                                         scaledOffset,
                                                                         tag,
                                                                         scaled,
                                                                         elem);
    } else {
        /* not supported by this allocator */
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("allocator type %s does not support tagged aligned"
                   " alloc\n"),
                   _sw_state_res_alloc_mgrs[thisPool.resManagerType].name));
        result = _SHR_E_UNAVAIL;
    }
    if (_SHR_E_NONE == result && !(flags & SW_STATE_RES_ALLOC_CHECK_ONLY)) {
        /* account for successful allocation */
        result = sw_state_mres_ref_count_update(unit,
                                                res_id,
                                                flags,
                                                count);
        SW_STATE_ACCESS_ERROR_CHECK(result);
    }

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, %d, %p, %d, &(%d)) return"
               " %d (%s)\n"),
               unit,
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
sw_state_mres_alloc_align_tag_group(int unit,
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
    _sw_state_res_pool_desc_t thisPool;
    _sw_state_res_type_desc_t thisRes;
    int result = _SHR_E_NONE;
    int xresult;
    int index;
    int xalign;
    uint32 blkFlags;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, %p, %p, %p, %p, %p, %p, %p)"
               " enter\n"),
               unit,
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
    RES_TYPE_VALID_CHECK(unit, res_id);
    RES_TYPE_EXIST_CHECK(unit, res_id);
    if (!grp_done) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("obligatory out argument grp_done is NULL\n")));
        return _SHR_E_PARAM;
    }
    *grp_done = 0;
    if (0 > grp_size) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("group member count %d must be >= 0\n"),
                   grp_size));
        return _SHR_E_PARAM;
    }
    if ((0 < grp_size) && (!flags || !count || !elem || !align || !offset)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("an obligatory array pointer is NULL\n")));
        return _SHR_E_PARAM;
    }
    if (grp_flags & (~(SW_STATE_RES_ALLOC_SINGLE_FLAGS | SW_STATE_RES_ALLOC_GROUP_FLAGS))) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("invalid group flags %08X\n"),
                   grp_flags & (~(SW_STATE_RES_ALLOC_SINGLE_FLAGS |
                   SW_STATE_RES_ALLOC_GROUP_FLAGS))));
        return _SHR_E_PARAM;
    }
    /* get the resource information */
    result = RESMGR_ACCESS.res.get(unit, res_id, &thisRes);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    /* get the pool information */
    result = RESMGR_ACCESS.pool.get(unit, thisRes.resPoolId, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    /* try to collect the requested blocks */
    if (_sw_state_res_alloc_mgrs[thisPool.resManagerType].tag_align) {
        /* allocator does not support this feature */
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("allocator type %s does not support tagged aligned"
                   " alloc\n"),
                   _sw_state_res_alloc_mgrs[thisPool.resManagerType].name));
        result = _SHR_E_UNAVAIL;
    }
    for (index = 0;
         (_SHR_E_NONE == result) && (index < grp_size);
         index++) {
        /* check parameters for this block */
        blkFlags = flags[index] | (grp_flags & SW_STATE_RES_ALLOC_SINGLE_FLAGS);
        if (1 > align[index]) {
            LOG_WARN(BSL_LS_SHARED_SWSTATE,
                     (BSL_META("align <= 0 invalid, using align = 1 instead\n")));
            xalign = 1;
        } else {
            xalign = align[index];
        }
        result = sw_state_mres_alloc_align_tag(unit, res_id, blkFlags, xalign, offset[index], tag[index], count[index], &elem[index]);
        if (_SHR_E_NONE != result) {
            /* we'll hope the allocation manager displayed an error */
            /* don't want postincrement if an error occurred */
            break;
        }
    } /* for (all blocks in the caller's group) */
    if ((_SHR_E_NONE != result) && (grp_flags & SW_STATE_RES_ALLOC_GROUP_ATOMIC)) {
        /* atomic mode and it failed; back out everything that we have */
        /* index is at the first failure */
        while (index > 0) {
            /* look at previous index (must have been successful) */
            index--;
            blkFlags = flags[index] | (grp_flags & SW_STATE_RES_ALLOC_SINGLE_FLAGS);
            if (0 == (blkFlags & SW_STATE_RES_ALLOC_REPLACE)) {
                /* Free only blocks that were not trying to be replaced,
                   using the internal dealloction function. */
                xresult = sw_state_mres_free(unit, 
                                             res_id, 
                                             count[index], 
                                             elem[index]);
                if (_SHR_E_NONE != xresult) {
                    LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                              (BSL_META("unable to back out %d resource %d"
                               " index %d base %d count %d: %d (%s)\n"),
                               unit,
                               res_id,
                               index,
                               elem[index],
                               count[index],
                               result,
                               _SHR_ERRMSG(result)));
                }
            } /* if (0 == (blkFlags & SW_STATE_RES_ALLOC_REPLACE)) */
        } /* while (index > 0) */
    } /* if (error && SW_STATE_RES_ALLOC_GROUP_ATOMIC was set) */
    /* update number of allocations that succeeded */
    *grp_done = index;

    /* return the result */
    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, &(%d), %p, %p, %p, %p, %p, %p)"
               " return %d (%s)\n"),
               unit,
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
        LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
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
sw_state_mres_free_and_status(int unit,
                         int res_id,
                         int count,
                         int elem,
                         uint32 *status)
{
    _sw_state_res_type_desc_t thisType;
    _sw_state_res_pool_desc_t thisPool;
    int result = _SHR_E_NONE;
    int scaled;
    int refCount;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %d, %d, %p) enter\n"),
               unit,
               res_id,
               count,
               elem,
               (void*)status));

    /* a little parameter checking */
    RES_TYPE_VALID_CHECK(unit, res_id);
    RES_TYPE_EXIST_CHECK(unit, res_id);
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("element count %d must be > 0\n"),
                   count));
        return _SHR_E_PARAM;
    }

    /* get the type information */
    result = RESMGR_ACCESS.res.get(unit, res_id, &thisType);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    /* get the pool information */
    result = RESMGR_ACCESS.pool.get(unit, thisType.resPoolId, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result);
    /* adjust element count per scaling factor */
    scaled = count * thisType.resElemSize;

    
    /* make the call */
    result = _sw_state_res_alloc_mgrs[thisPool.resManagerType].free(unit, 
                                                                thisType.resPoolId,
                                                                scaled,
                                                                elem);
    if (_SHR_E_NONE == result) {
        /* account for successful deallocation */
        result = sw_state_mres_ref_count_update(unit,
                                                res_id,
                                                0,
                                                -count);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        if (status) {
            *status = 0;

            /* Get new type ref count. */
            result = RESMGR_ACCESS.res.refCount.get(unit, res_id, &refCount);
            SW_STATE_ACCESS_ERROR_CHECK(result);
            if (!refCount) {
                (*status) |= SW_STATE_RES_FREED_TYPE_LAST_ELEM;
            }

            /* Get new pool ref count. */
            result = RESMGR_ACCESS.pool.inuse.get(unit, thisType.resPoolId, &refCount);
            SW_STATE_ACCESS_ERROR_CHECK(result);
            if (!refCount) {
                (*status) |= SW_STATE_RES_FREED_POOL_LAST_ELEM;
            }
        } /* if (flags) */
    } /* if (_SHR_E_NONE == result) */

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %d, %d, &(%08X)) return %d (%s)\n"),
               unit,
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
sw_state_mres_free(int unit,
              int res_id,
              int count,
              int elem)
{
    return sw_state_mres_free_and_status(unit, res_id, count, elem, NULL);
}

/*
 *  Free a bunch of elements/blocks of a resource type then get status flags
 */
int
sw_state_mres_free_group_and_status(int unit,
                               int res_id,
                               uint32 grp_flags,
                               int grp_size,
                               int *grp_done,
                               const int *count,
                               const int *elem,
                               uint32 *status)
{
    int result = _SHR_E_NONE;
    int index;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, %p, %p, %p, %p) enter\n"),
               unit,
               res_id,
               grp_flags,
               grp_size,
               (void*)grp_done,
               (void*)count,
               (void*)elem,
               (void*)status));

    /* a little parameter checking */
    RES_TYPE_VALID_CHECK(unit, res_id);
    RES_TYPE_EXIST_CHECK(unit, res_id);
    if (!grp_done) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("obligatory out argument grp_done is NULL\n")));
        return _SHR_E_PARAM;
    }
    *grp_done = 0;
    if (0 > grp_size) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("group member count %d must be >= 0\n"),
                   grp_size));
        return _SHR_E_PARAM;
    }
    if ((0 < grp_size) && (!count || !elem)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("an obligatory array pointer is NULL\n")));
        return _SHR_E_PARAM;
    }
    if (grp_flags & (~SW_STATE_RES_ALLOC_GROUP_FLAGS)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("invalid group flags %08X\n"),
                   grp_flags & (~SW_STATE_RES_ALLOC_GROUP_FLAGS)));
        return _SHR_E_PARAM;
    }

    /* try to release the requested blocks */
    for (index = 0;
         (_SHR_E_NONE == result) && (index < grp_size);
         index++) {
        /* free this element or block */
        result = sw_state_mres_free_and_status(unit,
                                               res_id,
                                               count[index], 
                                               elem[index], 
                                               status /* We don't care about overriding status. 
                                                It refers to the same resource anyway. */
                                               );
        if (_SHR_E_NONE != result) {
            /* we'll hope the allocation manager displayed an error */
            /* don't want postincrement if an error occurred */
            break;
        }
    } /* for (all elements/blocks as long as no errors) */
    /* update number of frees that succeeded */
    *grp_done = index;

    /* return the result */
    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, &(%d), %p, %p, &(%08X)) return %d (%s)\n"),
               unit,
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
        LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
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
sw_state_mres_free_group(int unit,
                    int res_id,
                    uint32 grp_flags,
                    int grp_size,
                    int *grp_done,
                    const int *count,
                    const int *elem)
{
    return sw_state_mres_free_group_and_status(unit, 
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
sw_state_mres_free_sparse_and_status(int unit,
                                int res_id,
                                uint32 pattern,
                                int length,
                                int repeats,
                                int elem,
                                uint32 *status)
{
    _sw_state_res_type_desc_t thisType;
    _sw_state_res_pool_desc_t thisPool;
    int result = _SHR_E_NONE;
    int count;
    int index;
    int refCount;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, %d, %d, %p) enter\n"),
               unit,
               res_id,
               pattern,
               length,
               repeats,
               elem,
               (void*)status));

    /* a little parameter checking */
    RES_TYPE_VALID_CHECK(unit, res_id);
    RES_TYPE_EXIST_CHECK(unit, res_id);
    if (0 >= length) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("pattern length must be greater than zero\n")));
        return _SHR_E_PARAM;
    }
    if (32 < length) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("pattern length must be 32 or less\n")));
        return _SHR_E_PARAM;
    }
    if (0 >= repeats) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("repeat count must be greater than zero\n")));
        return _SHR_E_PARAM;
    }

    /* get the type information */
    result = RESMGR_ACCESS.res.get(unit, res_id, &thisType);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    /* get the pool information */
    result = RESMGR_ACCESS.pool.get(unit, thisType.resPoolId, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    if (1 != thisType.resElemSize) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("not compatible with scaled resources\n")));
        return _SHR_E_CONFIG;
    }

    if (_sw_state_res_alloc_mgrs[thisPool.resManagerType].free_sparse) {
        /* make the call */
        result = _sw_state_res_alloc_mgrs[thisPool.resManagerType].free_sparse(unit, 
                                                                           thisType.resPoolId,
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
            result = sw_state_mres_ref_count_update(unit,
                                                    res_id,
                                                    0,
                                                    -count);
            SW_STATE_ACCESS_ERROR_CHECK(result);

            if (status) {
                *status = 0;

                /* Get new type ref count. */
                result = RESMGR_ACCESS.res.refCount.get(unit, res_id, &refCount);
                SW_STATE_ACCESS_ERROR_CHECK(result);
                if (!refCount) {
                    (*status) |= SW_STATE_RES_FREED_TYPE_LAST_ELEM;
                }

                /* Get new pool ref count. */
                result = RESMGR_ACCESS.pool.inuse.get(unit, thisType.resPoolId, &refCount);
                SW_STATE_ACCESS_ERROR_CHECK(result);
                if (!refCount) {
                    (*status) |= SW_STATE_RES_FREED_POOL_LAST_ELEM;
                }
            } /* if (flags) */
        } /* if (_SHR_E_NONE == result) */
    } else {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("allocator does not support sparse free\n")));
        return _SHR_E_UNAVAIL;
    }

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, %d, %d, &(%08X)) return %d (%s)\n"),
               unit,
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
sw_state_mres_free_sparse(int unit,
                     int res_id,
                     uint32 pattern,
                     int length,
                     int repeats,
                     int elem)
{
    return sw_state_mres_free_sparse_and_status(unit, 
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
sw_state_mres_check(int unit,
               int res_id,
               int count,
               int elem)
{
    _sw_state_res_type_desc_t thisType;
    _sw_state_res_pool_desc_t thisPool;
    int scaled;
    int result;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %d, %d) enter\n"),
               unit,
               res_id,
               count,
               elem));

    /* a little parameter checking */
    RES_TYPE_VALID_CHECK(unit, res_id);
    RES_TYPE_EXIST_CHECK(unit, res_id);
    if (1 > count) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("element count %d must be > 0\n"),
                   count));
        return _SHR_E_PARAM;
    }
    /* get the type information */
    result = RESMGR_ACCESS.res.get(unit, res_id, &thisType);
    SW_STATE_ACCESS_ERROR_CHECK(result);
    /* get the pool information */  
    result = RESMGR_ACCESS.pool.get(unit, thisType.resPoolId, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    /* adjust element count per scaling factor */
    scaled = count * thisType.resElemSize;

    
    /* make the call */
    result = _sw_state_res_alloc_mgrs[thisPool.resManagerType].check(unit, 
                                                                 thisType.resPoolId,
                                                                 scaled,
                                                                 elem);

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %d, %d) return %d (%s)\n"),
               unit,
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
sw_state_mres_check_group(int unit,
                     int res_id,
                     uint32 grp_flags,
                     int grp_size,
                     int *grp_done,
                     const int *count,
                     const int *elem,
                     int *status)
{
    int result = _SHR_E_NONE;
    int xresult;
    int index;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, %p, %p, %p, %p) enter\n"),
               unit,
               res_id,
               grp_flags,
               grp_size,
               (void*)grp_done,
               (void*)count,
               (void*)elem,
               (void*)status));

    /* a little parameter checking */
    RES_TYPE_VALID_CHECK(unit, res_id);
    RES_TYPE_EXIST_CHECK(unit, res_id);
    if (!grp_done) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("obligatory out argument grp_done is NULL\n")));
        return _SHR_E_PARAM;
    }
    *grp_done = 0;
    if (0 > grp_size) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("group member count %d must be >= 0\n"),
                   grp_size));
        return _SHR_E_PARAM;
    }
    if ((0 < grp_size) && (!count || !elem || !status)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("an obligatory array pointer is NULL\n")));
        return _SHR_E_PARAM;
    }
    if (grp_flags & (~SW_STATE_RES_ALLOC_GROUP_FLAGS)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("invalid group flags %08X\n"),
                   grp_flags & (~SW_STATE_RES_ALLOC_GROUP_FLAGS)));
        return _SHR_E_PARAM;
    }
    /* try to check the requested blocks */
    for (index = 0;
         (_SHR_E_NONE == result) && (index < grp_size);
         index++) {

        /* check this element or block */
        xresult = sw_state_mres_check(unit, 
                                      res_id, 
                                      count[index], 
                                      elem[index]);

        status[index] = xresult;
        if ((_SHR_E_NOT_FOUND != xresult) &&
            (_SHR_E_EXISTS != xresult)) {
            LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                      (BSL_META("unexpected result checking %d resource %d"
                       " index %d elem %d count %d: %d (%s)\n"),
                       unit,
                       res_id,
                       index,
                       elem[index],
                       count[index],
                       result,
                       _SHR_ERRMSG(result)));
            if (!(grp_flags & SW_STATE_RES_ALLOC_GROUP_ATOMIC)) {
                /* if not atomic mode, abort on first unexpected result */
                result = _SHR_E_FAIL;
                break;
            }
        } /* if (result is neither NOT_FOUND nor EXISTS) */
    } /* for (all elements/blocks as long as no errors) */
    /* update number of frees that succeeded */
    *grp_done = index;

    /* return the result */
    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, &(%d), %p, %p, %p) return %d (%s)\n"),
               unit,
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
        LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
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
sw_state_mres_check_all(int unit,
                   int res_id,
                   int count,
                   int elem)
{
    _sw_state_res_type_desc_t thisType;
    _sw_state_res_pool_desc_t thisPool;
    int scaled;
    int result;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %d, %d) enter\n"),
               unit,
               res_id,
               count,
               elem));

    /* a little parameter checking */
    RES_TYPE_VALID_CHECK(unit, res_id);
    RES_TYPE_EXIST_CHECK(unit, res_id);
    if (1 > count) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("element count %d must be > 0\n"),
                   count));
        return _SHR_E_PARAM;
    }

    /* get the type information */
    result = RESMGR_ACCESS.res.get(unit, res_id, &thisType);
    SW_STATE_ACCESS_ERROR_CHECK(result);
    /* get the pool information */  
    result = RESMGR_ACCESS.pool.get(unit, thisType.resPoolId, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    /* adjust element count per scaling factor */
    scaled = count * thisType.resElemSize;

    /* make the call */
    result = _sw_state_res_alloc_mgrs[thisPool.resManagerType].check_all(unit, 
                                                                     thisType.resPoolId,
                                                                     scaled,
                                                                     elem);

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %d, %d) return %d (%s)\n"),
               unit,
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
sw_state_mres_check_all_group(int unit,
                         int res_id,
                         uint32 grp_flags,
                         int grp_size,
                         int *grp_done,
                         const int *count,
                         const int *elem,
                         int *status)
{
    int result = _SHR_E_NONE;
    int xresult;
    int index;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, %p, %p, %p, %p) enter\n"),
               unit,
               res_id,
               grp_flags,
               grp_size,
               (void*)grp_done,
               (void*)count,
               (void*)elem,
               (void*)status));

    /* a little parameter checking */
    RES_TYPE_VALID_CHECK(unit, res_id);
    RES_TYPE_EXIST_CHECK(unit, res_id);
    if (!grp_done) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("obligatory out argument grp_done is NULL\n")));
        return _SHR_E_PARAM;
    }
    *grp_done = 0;
    if (0 > grp_size) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("group member count %d must be >= 0\n"),
                   grp_size));
        return _SHR_E_PARAM;
    }
    if ((0 < grp_size) && (!count || !elem || !status)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("an obligatory array pointer is NULL\n")));
        return _SHR_E_PARAM;
    }
    if (grp_flags & (~SW_STATE_RES_ALLOC_GROUP_FLAGS)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("invalid group flags %08X\n"),
                   grp_flags & (~SW_STATE_RES_ALLOC_GROUP_FLAGS)));
        return _SHR_E_PARAM;
    }
    /* try to check the requested blocks */
    for (index = 0;
         (_SHR_E_NONE == result) && (index < grp_size);
         index++) {
        /* check this element or block */
        xresult = sw_state_mres_check_all(unit,
                                          res_id,
                                          count[index], 
                                          elem[index]);
        status[index] = xresult;
        if ((_SHR_E_NOT_FOUND != xresult) &&
            (_SHR_E_EXISTS != xresult)) {
            LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                      (BSL_META("unexpected result checking %d resource %d"
                       " index %d elem %d count %d: %d (%s)\n"),
                       unit,
                       res_id,
                       index,
                       elem[index],
                       count[index],
                       result,
                       _SHR_ERRMSG(result)));
            if (!(grp_flags & SW_STATE_RES_ALLOC_GROUP_ATOMIC)) {
                /* if not atomic mode, abort on first unexpected result */
                result = _SHR_E_FAIL;
                break;
            }
        } /* if (result is neither NOT_FOUND nor EXISTS) */
    } /* for (all elements/blocks as long as no errors) */
    /* update number of frees that succeeded */
    *grp_done = index;

    /* return the result */
    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, &(%d), %p, %p, %p) return %d (%s)\n"),
               unit,
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
        LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
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
sw_state_mres_check_all_sparse(int unit,
                          int res_id,
                          uint32 pattern,
                          int length,
                          int repeats,
                          int elem)
{
    _sw_state_res_type_desc_t thisType;
    _sw_state_res_pool_desc_t thisPool;
    int result;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, %d, %d) enter\n"),
               unit,
               res_id,
               pattern,
               length,
               repeats,
               elem));

    /* a little parameter checking */
    RES_TYPE_VALID_CHECK(unit, res_id);
    RES_TYPE_EXIST_CHECK(unit, res_id);
    if (0 >= length) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("pattern length must be greater than zero\n")));
        return _SHR_E_PARAM;
    }
    if (32 < length) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("pattern length must be 32 or less\n")));
        return _SHR_E_PARAM;
    }
    if (0 >= repeats) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("repeat count must be greater than zero\n")));
        return _SHR_E_PARAM;
    }

    /* get the type information */
    result = RESMGR_ACCESS.res.get(unit, res_id, &thisType);
    SW_STATE_ACCESS_ERROR_CHECK(result);
    /* get the pool information */  
    result = RESMGR_ACCESS.pool.get(unit, thisType.resPoolId, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    if (1 != thisType.resElemSize) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("not compatible with scaled resources\n")));
        return _SHR_E_CONFIG;
    }

    if (_sw_state_res_alloc_mgrs[thisPool.resManagerType].c_a_sparse) {
        /* make the call */
        result = _sw_state_res_alloc_mgrs[thisPool.resManagerType].c_a_sparse(unit, 
                                                                          thisType.resPoolId,
                                                                          pattern,
                                                                          length,
                                                                          repeats,
                                                                          elem);
    } else {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("allocator does not support sparse check all\n")));
        return _SHR_E_UNAVAIL;
    }

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, %d, %d) return %d (%s)\n"),
               unit,
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
sw_state_mres_check_all_tag(int unit,
                       int res_id,
                       const void *tag,
                       int count,
                       int elem)
{
    _sw_state_res_type_desc_t thisType;
    _sw_state_res_pool_desc_t thisPool;
    int scaled;
    int result;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %p, %d, %d) enter\n"),
               unit,
               res_id,
               (void*)tag,
               count,
               elem));

    /* a little parameter checking */
    RES_TYPE_VALID_CHECK(unit, res_id);
    RES_TYPE_EXIST_CHECK(unit, res_id);
    if (1 > count) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("element count %d must be > 0\n"),
                   count));
        return _SHR_E_PARAM;
    }
    /* get the type information */
    result = RESMGR_ACCESS.res.get(unit, res_id, &thisType);
    SW_STATE_ACCESS_ERROR_CHECK(result);
    /* get the pool information */  
    result = RESMGR_ACCESS.pool.get(unit, thisType.resPoolId, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    /* adjust element count per scaling factor */
    scaled = count * thisType.resElemSize;

    /* make the call */
    result = _sw_state_res_alloc_mgrs[thisPool.resManagerType].check_all_tag(unit, 
                                                                         thisType.resPoolId,
                                                                         tag,
                                                                         scaled,
                                                                         elem);

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %p, %d, %d) return %d (%s)\n"),
               unit,
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
sw_state_mres_check_all_tag_group(int unit,
                             int res_id,
                             uint32 grp_flags,
                             int grp_size,
                             int *grp_done,
                             const void **tag,
                             const int *count,
                             const int *elem,
                             int *status)
{
    int result = _SHR_E_NONE;
    int xresult;
    int index;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, %p, %p, %p, %p, %p) enter\n"),
               unit,
               res_id,
               grp_flags,
               grp_size,
               (void*)grp_done,
               (void*)tag,
               (void*)count,
               (void*)elem,
               (void*)status));

    /* a little parameter checking */
    RES_TYPE_VALID_CHECK(unit, res_id);
    RES_TYPE_EXIST_CHECK(unit, res_id);
    if (!grp_done) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("obligatory out argument grp_done is NULL\n")));
        return _SHR_E_PARAM;
    }
    *grp_done = 0;
    if (0 > grp_size) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("group member count %d must be >= 0\n"),
                   grp_size));
        return _SHR_E_PARAM;
    }
    if ((0 < grp_size) && (!count || !elem || !status)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("an obligatory array pointer is NULL\n")));
        return _SHR_E_PARAM;
    }
    if (grp_flags & (~SW_STATE_RES_ALLOC_GROUP_FLAGS)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("invalid group flags %08X\n"),
                   grp_flags & (~SW_STATE_RES_ALLOC_GROUP_FLAGS)));
        return _SHR_E_PARAM;
    }
    /* try to check the requested blocks */
    for (index = 0;
         (_SHR_E_NONE == result) && (index < grp_size);
         index++) {
        /* check this element or block */
        xresult = sw_state_mres_check_all_tag(unit, 
                                              res_id, 
                                              tag[index], 
                                              count[index], 
                                              elem[index]);
        status[index] = xresult;
        if ((_SHR_E_NOT_FOUND != xresult) &&
            (_SHR_E_EXISTS != xresult)) {
            LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                      (BSL_META("unexpected result checking unit %d resource %d"
                       " index %d elem %d count %d: %d (%s)\n"),
                       unit,
                       res_id,
                       index,
                       elem[index],
                       count[index],
                       result,
                       _SHR_ERRMSG(result)));
            if (!(grp_flags & SW_STATE_RES_ALLOC_GROUP_ATOMIC)) {
                /* if not atomic mode, abort on first unexpected result */
                result = _SHR_E_FAIL;
                break;
            }
        } /* if (result is neither NOT_FOUND nor EXISTS) */
    } /* for (all elements/blocks as long as no errors) */
    /* update number of frees that succeeded */
    *grp_done = index;

    /* return the result */
    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META("(%d, %d, %08X, %d, &(%d), %p, %p, %p, %p) return %d (%s)\n"),
               unit,
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
        LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
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
sw_state_mres_dump(int unit)
{
    _sw_state_res_type_desc_t thisRes;
    _sw_state_res_pool_desc_t thisPool;
    int index;
    int result = _SHR_E_NONE;
    uint16 resTypeCount, resPoolCount;
    uint8 exists;

    /* a little parameter checking */

    /* dump information about the handle */
    result = RESMGR_ACCESS.resTypeCount.get(unit, &resTypeCount);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    result = RESMGR_ACCESS.resPoolCount.get(unit, &resPoolCount);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    LOG_CLI((BSL_META("%d resource allocation manager\n"), unit));
    LOG_CLI((BSL_META("  resource type count = %d\n"), resTypeCount));
    LOG_CLI((BSL_META("  resource pool count = %d\n"), resPoolCount));
    for (index = 0; index < resPoolCount; index++) {
        result = RESMGR_ACCESS.pool.is_allocated(unit, index, &exists);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        if (exists) {
            result = RESMGR_ACCESS.pool.get(unit, index, &thisPool);
            SW_STATE_ACCESS_ERROR_CHECK(result);
            LOG_CLI((BSL_META("  resource pool %d (%s):\n"),
                     index,
                     (thisPool.name.name)));
            LOG_CLI((BSL_META("    method = %d (%s)\n"),
                     thisPool.resManagerType,
                     _sw_state_res_alloc_mgrs[thisPool.resManagerType].name));
            LOG_CLI((BSL_META("    handle = %u\n"), thisPool.resHandle));
            LOG_CLI((BSL_META("    range  = %d..%d\n"),
                     thisPool.low,
                     thisPool.low + thisPool.count - 1));
            LOG_CLI((BSL_META("    elems  = %d (%d in use)\n"),
                     thisPool.count,
                     thisPool.inuse));
            LOG_CLI((BSL_META("    refcnt = %d\n"), thisPool.refCount));
        } else {
            LOG_CLI((BSL_META("  resource pool %d is not in use\n"), index));
        }
    }
    for (index = 0; index < resTypeCount; index++) {
        result = RESMGR_ACCESS.res.is_allocated(unit, index, &exists);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        if (exists) {
            result = RESMGR_ACCESS.res.get(unit, index, &thisRes);
            SW_STATE_ACCESS_ERROR_CHECK(result);

            result = RESMGR_ACCESS.pool.get(unit, thisRes.resPoolId, &thisPool);
            SW_STATE_ACCESS_ERROR_CHECK(result);
            LOG_CLI((BSL_META("  resource type %d (%s):\n"),
                     index,
                     (thisRes.name.name)));
            LOG_CLI((BSL_META("    resource pool   = %d (%s)\n"),
                     thisRes.resPoolId,
                     (thisPool.name.name)));
            LOG_CLI((BSL_META("    pool elem each  = %d\n"),
                     thisRes.resElemSize));
            LOG_CLI((BSL_META("    active elements = %d\n"),
                     thisRes.refCount));
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
sw_state_res_init(int unit,
             int num_res_types,
             int num_res_pools)
{
    int result = _SHR_E_NONE;
    uint8 is_allocated;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META_U(unit,
                          "(%d, %d, %d) enter\n"),
               unit,
               num_res_types,
               num_res_pools));

    /* a little parameter checking */
    if ((0 > (unit)) || (BCM_LOCAL_UNITS_MAX <= (unit))) { 
        LOG_ERROR(BSL_LS_SHARED_SWSTATE, 
                  (BSL_META("invalid unit number %d\n"), 
                   unit)); 
        return _SHR_E_PARAM; 
    } 


    /* If this unit's resource manager was already created,
       destroy the current information. */
    result = RESMGR_ACCESS.is_allocated(unit, &is_allocated);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    if (is_allocated) { 
        /* this unit has already been initialised; tear it down */
        result = _sw_state_mres_destroy_data(unit);
    }

    if (_SHR_E_NONE == result) {
        result = sw_state_mres_create(unit, 
                                 num_res_types,
                                 num_res_pools);
        SW_STATE_ACCESS_ERROR_CHECK(result);
    } /* if (_SHR_E_NONE == result) */


    /* return the result */
    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
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
sw_state_res_get(int unit,
            int *num_res_types,
            int *num_res_pools)
{

    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the fetch */
    return sw_state_mres_get(unit, num_res_types, num_res_pools);
}

/*
 *  Deinitialise unit
 */
int
sw_state_res_detach(int unit)
{
    int result = _SHR_E_NONE;
    uint16 resTypeCount;

    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
              (BSL_META_U(unit,
                          "(%d) enter\n"),
               unit));

    /* get this unit's current information, mark it as destroyed */
    /* If this unit's resource manager was already created,
       destroy the current information. */
    result = RESMGR_ACCESS.resTypeCount.get(unit, &resTypeCount);
    SW_STATE_ACCESS_ERROR_CHECK(result);


    if (resTypeCount) { 
        /* this unit has already been initialised; tear it down */
        result = _sw_state_mres_destroy_data(unit);
    }


    /* return the result */
    LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
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
sw_state_res_pool_set(int unit,
                 int pool_id,
                 sw_state_res_allocator_t manager,
                 int low_id,
                 int count,
                 const void *extras,
                 const char *name)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_pool_set(unit, 
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
sw_state_res_pool_unset(int unit,
                   int pool_id)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_pool_unset(unit, pool_id);
}

/*
 *  Get information about a resource pool on a unit
 */
int
sw_state_res_pool_get(int unit,
                 int pool_id,
                 sw_state_res_allocator_t *manager,
                 int *low_id,
                 int *count,
                 sw_state_res_tagged_bitmap_extras_t *extras,
                 sw_state_res_name_t *name)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_pool_get(unit, 
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
sw_state_res_pool_info_get(int unit,
                      int pool_id,
                      sw_state_res_pool_info_t *info)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_pool_info_get(unit, 
                                  pool_id,
                                  info);
}

/*
 *  Configure a resource type on a unit
 */
int
sw_state_res_type_set(int unit,
                 int res_id,
                 int pool_id,
                 int elem_size,
                 const char *name)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_type_set(unit, res_id, pool_id, elem_size, name);
}

/*
 *  Destroy a resource type on a unit
 */
int
sw_state_res_type_unset(int unit,
                   int res_id)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_type_unset(unit, res_id);
}

/*
 *  Get information about a resource type
 */
int
sw_state_res_type_get(int unit,
                 int res_id,
                 int *pool_id,
                 int *elem_size,
                 sw_state_res_name_t *name)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_type_get(unit, res_id, pool_id, elem_size, name);
}

/*
 *  Get information about a resource type
 */
int
sw_state_res_type_info_get(int unit,
                      int res_id,
                      sw_state_res_type_info_t *info)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_type_info_get(unit, res_id, info);
}


int
sw_state_res_type_status_get(int unit,
                             int res_id){
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_type_status_get(unit, res_id);
}

/*
 *  Allocate elements of a resource type
 */
int
sw_state_res_alloc(int unit,
              int res_id,
              uint32 flags,
              int count,
              int *elem)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_alloc(unit, res_id, flags, count, elem);
}

/*
 *  Allocate a bunch of elements or blocks of a resource type
 */
int
sw_state_res_alloc_group(int unit,
                    int res_id,
                    uint32 grp_flags,
                    int grp_size,
                    int *grp_done,
                    const uint32 *flags,
                    const int *count,
                    int *elem)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_alloc_group(unit, 
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
sw_state_res_alloc_tag(int unit,
                  int res_id,
                  uint32 flags,
                  const void *tag,
                  int count,
                  int *elem)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_alloc_tag(unit, res_id, flags, tag, count, elem);
}

/*
 *  Allocate a bunch of elements or blocks of a resource type
 */
int
sw_state_res_alloc_tag_group(int unit,
                        int res_id,
                        uint32 grp_flags,
                        int grp_size,
                        int *grp_done,
                        const uint32 *flags,
                        const void **tag,
                        const int *count,
                        int *elem)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_alloc_tag_group(unit, 
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
sw_state_res_alloc_align(int unit,
                    int res_id,
                    uint32 flags,
                    int align,
                    int offset,
                    int count,
                    int *elem)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_alloc_align(unit, 
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
sw_state_res_alloc_align_sparse(int unit,
                           int res_id,
                           uint32 flags,
                           int align,
                           int offset,
                           uint32 pattern,
                           int length,
                           int repeats,
                           int *elem)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_alloc_align_sparse(unit, 
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
sw_state_res_alloc_align_group(int unit,
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
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_alloc_align_group(unit, 
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
sw_state_res_alloc_align_tag(int unit,
                        int res_id,
                        uint32 flags,
                        int align,
                        int offset,
                        const void *tag,
                        int count,
                        int *elem)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_alloc_align_tag(unit, 
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

static int
_sw_state_res_tag_set(int unit,
                     int pool_id,
                     int offset,
                     int count,
                     const void* tag) {
    sw_state_res_tag_bitmap_handle_t handle;
    int result;
    sw_state_res_allocator_t manager;
    int start;
    int end;
    int pool_count;
    sw_state_res_tagged_bitmap_extras_t extras;
    sw_state_res_name_t name;
    RES_UNIT_CHECK(unit);
    RES_POOL_VALID_CHECK(unit, pool_id);
    RES_POOL_EXIST_CHECK(unit, pool_id);

    result = sw_state_mres_pool_get(unit, pool_id, &manager, &start, &pool_count, &extras, &name);
    SW_STATE_ACCESS_ERROR_CHECK(result);
    end = pool_count + start;

    if (offset < start) {
        SW_STATE_ACCESS_ERROR_CHECK(_SHR_E_PARAM);
    }
    if (count + offset > end) {
        SW_STATE_ACCESS_ERROR_CHECK(_SHR_E_PARAM);
    }
    if (manager != SW_STATE_RES_ALLOCATOR_TAGGED_BITMAP) {
        SW_STATE_ACCESS_ERROR_CHECK(_SHR_E_PARAM);
    }
    if (extras.tag_length == 0) {
        SW_STATE_ACCESS_ERROR_CHECK(_SHR_E_PARAM);
    }
    result = RESMGR_ACCESS.pool.resHandle.get(unit, pool_id, &handle);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    offset = offset - start;
    return sw_state_res_tag_bitmap_tag_set(unit, handle, tag, offset, count);
}

int
sw_state_res_tag_set(int unit,
                     int res_id,
                     int offset,
                     int count,
                     const void* tag) {
    _sw_state_res_pool_desc_t thisPool;
    _sw_state_res_type_desc_t thisType;
    int result = _SHR_E_NONE;
    LOG_DEBUG(BSL_LS_SHARED_SWSTATE, (BSL_META("(%d, %d, %p, %d, %d) enter\n"), unit, res_id, (void*)tag, offset, count));

    /* a little parameter checking */
    RES_TYPE_VALID_CHECK(unit, res_id);
    RES_TYPE_EXIST_CHECK(unit, res_id);

    /* Get the type and pool information */
    result = RESMGR_ACCESS.res.get(unit, res_id, &thisType);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    result = RESMGR_ACCESS.pool.get(unit, thisType.resPoolId, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    /* make the call */
    if (_sw_state_res_alloc_mgrs[thisPool.resManagerType].tag_set) {
        /* allocator supports it; make the call */
        /*(int unit,int pool_id,const void* tag,int elemIndex,int elemCount)*/
        result = _sw_state_res_alloc_mgrs[thisPool.resManagerType].tag_set(unit, thisType.resPoolId, offset, count, tag);
    } else {
        /* not supported by this allocator */
        LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META("allocator type %s does not support tagged alloc\n"), _sw_state_res_alloc_mgrs[thisPool.resManagerType].name));
        result = _SHR_E_UNAVAIL;
    }

    return result;

}

static int
_sw_state_res_tag_get(int unit,
                     int pool_id,
                     int element,
                     const void* tag) {
    sw_state_res_tag_bitmap_handle_t handle;
    int result;
    sw_state_res_allocator_t manager;
    int start;
    int end;
    int pool_count;
    sw_state_res_tagged_bitmap_extras_t extras;
    sw_state_res_name_t name;
    RES_UNIT_CHECK(unit);
    RES_POOL_VALID_CHECK(unit, pool_id);
    RES_POOL_EXIST_CHECK(unit, pool_id);

    result = sw_state_mres_pool_get(unit, pool_id, &manager, &start, &pool_count, &extras, &name);
    SW_STATE_ACCESS_ERROR_CHECK(result);
    end = pool_count + start;

    if (element < start) {
        SW_STATE_ACCESS_ERROR_CHECK(_SHR_E_PARAM);
    }
    if (element >= end) {
        SW_STATE_ACCESS_ERROR_CHECK(_SHR_E_PARAM);
    }
    if (manager != SW_STATE_RES_ALLOCATOR_TAGGED_BITMAP) {
        SW_STATE_ACCESS_ERROR_CHECK(_SHR_E_PARAM);
    }
    if (extras.tag_length == 0) {
        SW_STATE_ACCESS_ERROR_CHECK(_SHR_E_PARAM);
    }

    element = element - start;

    result = RESMGR_ACCESS.pool.resHandle.get(unit, pool_id, &handle);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    return sw_state_res_tag_bitmap_tag_get(unit, handle, element, tag);
}

int
sw_state_res_tag_get(int unit,
                     int res_id,
                     int element,
                     const void* tag) {
    _sw_state_res_pool_desc_t thisPool;
    _sw_state_res_type_desc_t thisType;
    int result = _SHR_E_NONE;
    LOG_DEBUG(BSL_LS_SHARED_SWSTATE, (BSL_META("(%d, %d, %p, %d) enter\n"), unit, res_id, (void*)tag, element));

    /* a little parameter checking */
    RES_TYPE_VALID_CHECK(unit, res_id);
    RES_TYPE_EXIST_CHECK(unit, res_id);

    /* Get the type and pool information */
    result = RESMGR_ACCESS.res.get(unit, res_id, &thisType);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    result = RESMGR_ACCESS.pool.get(unit, thisType.resPoolId, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    /* make the call */
    if (_sw_state_res_alloc_mgrs[thisPool.resManagerType].tag_get) {
        /* allocator supports it; make the call */
        /*(int unit,int pool_id,const void* tag,int elemIndex,int elemCount)*/
        result = _sw_state_res_alloc_mgrs[thisPool.resManagerType].tag_get(unit, thisType.resPoolId, element, tag);
    } else {
        /* not supported by this allocator */
        LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META("allocator type %s does not support tagged alloc\n"), _sw_state_res_alloc_mgrs[thisPool.resManagerType].name));
        result = _SHR_E_UNAVAIL;
    }

    return result;

}
int
sw_state_res_alloc_align_tag_group(int unit,
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
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_alloc_align_tag_group(unit, 
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
sw_state_res_free(int unit,
             int res_id,
             int count,
             int elem)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_free_and_status(unit, res_id, count, elem, NULL);
}

/*
 *  Free elements of a resource type, get status flags
 */
int
sw_state_res_free_and_status(int unit,
                        int res_id,
                        int count,
                        int elem,
                        uint32 *flags)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_free_and_status(unit, res_id, count, elem, flags);
}

/*
 *  Free sparse block of elements of a resource type
 */
int
sw_state_res_free_sparse(int unit,
                    int res_id,
                    uint32 pattern,
                    int length,
                    int repeats,
                    int elem)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_free_sparse_and_status(unit, 
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
sw_state_res_free_sparse_and_status(int unit,
                               int res_id,
                               uint32 pattern,
                               int length,
                               int repeats,
                               int elem,
                               uint32 *flags)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_free_sparse_and_status(unit, 
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
sw_state_res_free_group(int unit,
                   int res_id,
                   uint32 grp_flags,
                   int grp_size,
                   int *grp_done,
                   const int *count,
                   const int *elem)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_free_group_and_status(unit, 
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
sw_state_res_free_group_and_status(int unit,
                              int res_id,
                              uint32 grp_flags,
                              int grp_size,
                              int *grp_done,
                              const int *count,
                              const int *elem,
                              uint32 *status)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_free_group_and_status(unit, 
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
sw_state_res_check(int unit,
              int res_id,
              int count,
              int elem)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_check(unit, res_id, count, elem);
}

/*
 *  Check a bunch of elements/blocks of a resource type
 */
int
sw_state_res_check_group(int unit,
                    int res_id,
                    uint32 grp_flags,
                    int grp_size,
                    int *grp_done,
                    const int *count,
                    const int *elem,
                    int *status)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_check_group(unit, 
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
sw_state_res_check_all(int unit,
                  int res_id,
                  int count,
                  int elem)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_check_all(unit, res_id, count, elem);
}

/*
 *  Check whether elements in a sparse block are free or not
 */
int
sw_state_res_check_all_sparse(int unit,
                         int res_id,
                         uint32 pattern,
                         int length,
                         int repeats,
                         int elem)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_check_all_sparse(unit, 
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
sw_state_res_check_all_group(int unit,
                        int res_id,
                        uint32 grp_flags,
                        int grp_size,
                        int *grp_done,
                        const int *count,
                        const int *elem,
                        int *status)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_check_all_group(unit, 
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
sw_state_res_check_all_tag(int unit,
                      int res_id,
                      const void *tag,
                      int count,
                      int elem)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_check_all_tag(unit, res_id, tag, count, elem);
}

/*
 *  Check a bunch of elements/blocks of a resource type
 */
int
sw_state_res_check_all_tag_group(int unit,
                            int res_id,
                            uint32 grp_flags,
                            int grp_size,
                            int *grp_done,
                            const void **tag,
                            const int *count,
                            const int *elem,
                            int *status)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_check_all_tag_group(unit, 
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
sw_state_res_dump(int unit)
{
    /* a little parameter checking */
    RES_UNIT_CHECK(unit);

    /* perform the action */
    return sw_state_mres_dump(unit);
}

/*****************************************************************************/
/*
 *  Interface to sw_state_res_bitmap (external implementation)
 */

/*
 *  sw_state_res_bitmap is a fairly simple bitmap allocator that supports a number
 *  of features, but isn't terribly optimised.  It allows alignment and offset,
 *  blocks of multiple elements (arbitrary size).  It has a few other minor
 *  upgrades versus simple bitmap, including keeping track of the next place
 *  where it wants to search for free elements and the last place where it
 *  freed elements, to improve performance under simple conditions, and which
 *  are designed to not have significant negative impact even when they fail to
 *  provide improvements to performance.
 */
static int
_sw_state_res_bitmap_create(int unit, 
                       int pool_id,
                       int low_id,
                       int count,
                       const void* extras,
                       const char* name)
{
    int result;
    sw_state_res_bitmap_handle_t handle;
    /* create the bitmap allocator */
    result = sw_state_res_bitmap_create(unit, &handle, low_id, count);
    if (_SHR_E_NONE == result) {
        result = RESMGR_ACCESS.pool.resHandle.set(unit, pool_id, handle);
        SW_STATE_ACCESS_ERROR_CHECK(result);
    } else {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("unable to create bitmap allocator, low_id = %d,"
                   " count = %d\n"),
                   low_id,
                   count));
    }
    return result;
}

static int
_sw_state_res_bitmap_destroy(int unit, 
                        int pool_id)
{
    sw_state_res_bitmap_handle_t handle;
    int result;

    result = RESMGR_ACCESS.pool.resHandle.get(unit, pool_id, &handle);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    /* destroy the list */
    result = sw_state_res_bitmap_destroy(unit, handle);
    return result;
}

static int
_sw_state_res_bitmap_alloc(int unit, 
                      int pool_id,
                      uint32 flags,
                      int count,
                      int *elem)
{
    sw_state_res_bitmap_handle_t handle;
    int result;
    uint32 iflags = 0;

    result = RESMGR_ACCESS.pool.resHandle.get(unit, pool_id, &handle);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    if (flags & SW_STATE_RES_ALLOC_WITH_ID) {
        iflags |= SW_STATE_RES_BITMAP_ALLOC_WITH_ID;
    }
    if (flags & SW_STATE_RES_ALLOC_REPLACE) {
        iflags |= SW_STATE_RES_BITMAP_ALLOC_REPLACE;
    }
    return sw_state_res_bitmap_alloc(unit, handle, iflags, count, elem);
}

static int
_sw_state_res_bitmap_alloc_align(int unit, 
                            int pool_id,
                            uint32 flags,
                            int align,
                            int offs,
                            int count,
                            int *elem)
{
    sw_state_res_bitmap_handle_t handle;
    int result;
    uint32 iflags = 0;

    result = RESMGR_ACCESS.pool.resHandle.get(unit, pool_id, &handle);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    
    if (flags & SW_STATE_RES_ALLOC_WITH_ID) {
        iflags |= SW_STATE_RES_BITMAP_ALLOC_WITH_ID;
    }
    if (flags & SW_STATE_RES_ALLOC_ALIGN_ZERO) {
        iflags |= SW_STATE_RES_BITMAP_ALLOC_ALIGN_ZERO;
    }
    if (flags & SW_STATE_RES_ALLOC_REPLACE) {
        iflags |= SW_STATE_RES_BITMAP_ALLOC_REPLACE;
    }
    return sw_state_res_bitmap_alloc_align(unit, 
                                      handle,
                                      iflags,
                                      align,
                                      offs,
                                      count,
                                      elem);
}

static int
_sw_state_res_bitmap_alloc_align_sparse(int unit, 
                                   int pool_id,
                                   uint32 flags,
                                   int align,
                                   int offs,
                                   uint32 pattern,
                                   int length,
                                   int repeats,
                                   int *elem)
{
    sw_state_res_bitmap_handle_t handle;
    int result;
    uint32 iflags = 0;

    result = RESMGR_ACCESS.pool.resHandle.get(unit, pool_id, &handle);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    if (flags & SW_STATE_RES_ALLOC_WITH_ID) {
        iflags |= SW_STATE_RES_BITMAP_ALLOC_WITH_ID;
    }
    if (flags & SW_STATE_RES_ALLOC_ALIGN_ZERO) {
        iflags |= SW_STATE_RES_BITMAP_ALLOC_ALIGN_ZERO;
    }
    if (flags & SW_STATE_RES_ALLOC_REPLACE) {
        iflags |= SW_STATE_RES_BITMAP_ALLOC_REPLACE;
    }
    return sw_state_res_bitmap_alloc_align_sparse(unit, 
                                             handle,
                                             flags,
                                             align,
                                             offs,
                                             pattern,
                                             length,
                                             repeats,
                                             elem);
}

static int
_sw_state_res_bitmap_free(int unit, 
                     int pool_id,
                     int count,
                     int elem)
{
    sw_state_res_bitmap_handle_t handle;
    int result;

    result = RESMGR_ACCESS.pool.resHandle.get(unit, pool_id, &handle);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    return sw_state_res_bitmap_free(unit, handle, count, elem);
}

static int
_sw_state_res_bitmap_free_sparse(int unit, 
                            int pool_id,
                            uint32 pattern,
                            int length,
                            int repeats,
                            int elem)
{
    sw_state_res_bitmap_handle_t handle;
    int result;

    result = RESMGR_ACCESS.pool.resHandle.get(unit, pool_id, &handle);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    return sw_state_res_bitmap_free_sparse(unit, handle, pattern, length, repeats, elem);
}

static int
_sw_state_res_bitmap_check(int unit, 
                      int pool_id,
                      int count,
                      int elem)
{
    sw_state_res_bitmap_handle_t handle;
    int result;

    result = RESMGR_ACCESS.pool.resHandle.get(unit, pool_id, &handle);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    return sw_state_res_bitmap_check(unit, handle, count, elem);
}

static int
_sw_state_res_bitmap_check_all_desc(int unit, 
                               int pool_id,
                               int count,
                               int elem)
{
    sw_state_res_bitmap_handle_t handle;
    int result;

    result = RESMGR_ACCESS.pool.resHandle.get(unit, pool_id, &handle);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    return sw_state_res_bitmap_check_all(unit, handle, count, elem);
}

static int
_sw_state_res_bitmap_check_all_sparse(int unit, 
                                 int pool_id,
                                 uint32 pattern,
                                 int length,
                                 int repeats,
                                 int elem)
{
    sw_state_res_bitmap_handle_t handle;
    int result;

    result = RESMGR_ACCESS.pool.resHandle.get(unit, pool_id, &handle);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    return sw_state_res_bitmap_check_all_sparse(unit, 
                                           handle,
                                           pattern,
                                           length,
                                           repeats,
                                           elem);
}

/*****************************************************************************/
/*
 *  Interface to sw_state_res_tag_bitmap (external implementation)
 */

/*
 *  sw_state_res_tag_bitmap is a fairly simple bitmap allocator that supports a
 *  number of features, but isn't terribly optimised.  It allows alignment and
 *  offset, blocks of multiple elements (arbitrary size).  It also optionally
 *  tracks tags (an arbitrary number of bytes assigned to each grain, where a
 *  grain is one or more elements -- a grain can only be shared between blocks
 *  if the blocks have same tag).  Disabling tagging renders it roughly
 *  equivalent to the sw_state_res_bitmap implementation, but perhaps a little bit
 *  less optimal.
 */
static int
_sw_state_res_tag_bitmap_create(int unit,
                           int pool_id,
                           int low_id,
                           int count,
                           const void* extras,
                           const char* name)
{
    sw_state_res_tagged_bitmap_extras_t *extra;
    sw_state_res_bitmap_handle_t handle;
    int result;

    extra = (sw_state_res_tagged_bitmap_extras_t*) extras;

    result = RESMGR_ACCESS.pool.tagged_bmp_extras.set(unit, pool_id, extra);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    /* create the bitmap allocator */
    result = sw_state_res_tag_bitmap_create(unit,
                                           &handle,
                                           low_id,
                                           count,
                                           extra->grain_size,
                                           extra->tag_length);
    if (_SHR_E_NONE != result) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("unable to create tagged bitmap allocator:"
                   " %d (%s)\n"),
                   result,
                   _SHR_ERRMSG(result)));
    } else {
        result = RESMGR_ACCESS.pool.resHandle.set(unit, pool_id, handle);
        SW_STATE_ACCESS_ERROR_CHECK(result);
    }
    return result;
}


static int
_sw_state_res_tag_bitmap_destroy(int unit, 
                            int pool_id)
{
    sw_state_res_tag_bitmap_handle_t handle;
    int result;

    result = RESMGR_ACCESS.pool.resHandle.get(unit, pool_id, &handle);
    SW_STATE_ACCESS_ERROR_CHECK(result);


    /* destroy the list */
    result = sw_state_res_tag_bitmap_destroy(unit, handle);
    return result;
}

static int
_sw_state_res_tag_bitmap_alloc(int unit, 
                          int pool_id,
                          uint32 flags,
                          int count,
                          int *elem)
{
    sw_state_res_tag_bitmap_handle_t handle;
    int result;
    uint32 iflags = 0;

    result = RESMGR_ACCESS.pool.resHandle.get(unit, pool_id, &handle);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    if (flags & SW_STATE_RES_ALLOC_WITH_ID) {
        iflags |= SW_STATE_RES_TAG_BITMAP_ALLOC_WITH_ID;
    }
    if (flags & SW_STATE_RES_ALLOC_REPLACE) {
        iflags |= SW_STATE_RES_TAG_BITMAP_ALLOC_REPLACE;
    }
    if (flags & SW_STATE_RES_ALLOC_CHECK_ONLY) {
        iflags |= SW_STATE_RES_TAG_BITMAP_ALLOC_CHECK_ONLY;
    }
    return sw_state_res_tag_bitmap_alloc(unit, handle, iflags, count, elem);
}

static int
_sw_state_res_tag_bitmap_alloc_tag(int unit, 
                              int pool_id,
                              uint32 flags,
                              const void* tag,
                              int count,
                              int *elem)
{
    sw_state_res_tag_bitmap_handle_t handle;
    int result;
    uint32 iflags = 0;

    result = RESMGR_ACCESS.pool.resHandle.get(unit, pool_id, &handle);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    if (flags & SW_STATE_RES_ALLOC_WITH_ID) {
        iflags |= SW_STATE_RES_TAG_BITMAP_ALLOC_WITH_ID;
    }
    if (flags & SW_STATE_RES_ALLOC_REPLACE) {
        iflags |= SW_STATE_RES_TAG_BITMAP_ALLOC_REPLACE;
    }
    if (flags & SW_STATE_RES_ALLOC_ALWAYS_CHECK_TAG) {
        iflags |= SW_STATE_RES_TAG_BITMAP_ALWAYS_CHECK_TAG;
    }
    if (flags & SW_STATE_RES_ALLOC_CHECK_ONLY) {
        iflags |= SW_STATE_RES_TAG_BITMAP_ALLOC_CHECK_ONLY;
    }
    return sw_state_res_tag_bitmap_alloc_tag(unit, handle, iflags, tag, count, elem);
}

static int
_sw_state_res_tag_bitmap_alloc_align(int unit, 
                                int pool_id,
                                uint32 flags,
                                int align,
                                int offs,
                                int count,
                                int *elem)
{
    sw_state_res_tag_bitmap_handle_t handle;
    int result;
    uint32 iflags = 0;

    result = RESMGR_ACCESS.pool.resHandle.get(unit, pool_id, &handle);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    if (flags & SW_STATE_RES_ALLOC_WITH_ID) {
        iflags |= SW_STATE_RES_TAG_BITMAP_ALLOC_WITH_ID;
    }
    if (flags & SW_STATE_RES_ALLOC_ALIGN_ZERO) {
        iflags |= SW_STATE_RES_TAG_BITMAP_ALLOC_ALIGN_ZERO;
    }
    if (flags & SW_STATE_RES_ALLOC_REPLACE) {
        iflags |= SW_STATE_RES_TAG_BITMAP_ALLOC_REPLACE;
    }
    if (flags & SW_STATE_RES_ALLOC_CHECK_ONLY) {
        iflags |= SW_STATE_RES_TAG_BITMAP_ALLOC_CHECK_ONLY;
    }
    return sw_state_res_tag_bitmap_alloc_align(unit, 
                                              handle,
                                              iflags,
                                              align,
                                              offs,
                                              count,
                                              elem);
}

static int
_sw_state_res_tag_bitmap_alloc_align_tag(int unit, 
                                    int pool_id,
                                    uint32 flags,
                                    int align,
                                    int offs,
                                    const void *tag,
                                    int count,
                                    int *elem)
{
    sw_state_res_tag_bitmap_handle_t handle;
    int result;
    uint32 iflags = 0;

    result = RESMGR_ACCESS.pool.resHandle.get(unit, pool_id, &handle);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    if (flags & SW_STATE_RES_ALLOC_WITH_ID) {
        iflags |= SW_STATE_RES_TAG_BITMAP_ALLOC_WITH_ID;
    }
    if (flags & SW_STATE_RES_ALLOC_ALIGN_ZERO) {
        iflags |= SW_STATE_RES_TAG_BITMAP_ALLOC_ALIGN_ZERO;
    }
    if (flags & SW_STATE_RES_ALLOC_REPLACE) {
        iflags |= SW_STATE_RES_TAG_BITMAP_ALLOC_REPLACE;
    }
    if (flags & SW_STATE_RES_ALLOC_ALWAYS_CHECK_TAG) {
        iflags |= SW_STATE_RES_TAG_BITMAP_ALWAYS_CHECK_TAG;
    }
    if (flags & SW_STATE_RES_ALLOC_CHECK_ONLY) {
        iflags |= SW_STATE_RES_TAG_BITMAP_ALLOC_CHECK_ONLY;
    }
    return sw_state_res_tag_bitmap_alloc_align_tag(unit,
                                                  handle,
                                                  iflags,
                                                  align,
                                                  offs,
                                                  tag,
                                                  count,
                                                  elem);
}

static int
_sw_state_res_tag_bitmap_free(int unit, 
                         int pool_id,
                         int count,
                         int elem)
{
    sw_state_res_tag_bitmap_handle_t handle;
    int result;

    result = RESMGR_ACCESS.pool.resHandle.get(unit, pool_id, &handle);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    return sw_state_res_tag_bitmap_free(unit, handle, count, elem);
}

static int
_sw_state_res_tag_bitmap_check(int unit, 
                          int pool_id,
                          int count,
                          int elem)
{
    sw_state_res_tag_bitmap_handle_t handle;
    int result;

    result = RESMGR_ACCESS.pool.resHandle.get(unit, pool_id, &handle);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    return sw_state_res_tag_bitmap_check(unit, handle, count, elem);
}

static int
_sw_state_res_tag_bitmap_check_all(int unit, 
                              int pool_id,
                              int count,
                              int elem)
{
    sw_state_res_tag_bitmap_handle_t handle;
    int result;

    result = RESMGR_ACCESS.pool.resHandle.get(unit, pool_id, &handle);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    return sw_state_res_tag_bitmap_check_all(unit, handle, count, elem);
}

static int
_sw_state_res_tag_bitmap_check_all_tag(int unit, 
                                  int pool_id,
                                  const void *tag,
                                  int count,
                                  int elem)
{
    sw_state_res_tag_bitmap_handle_t handle;
    int result;

    result = RESMGR_ACCESS.pool.resHandle.get(unit, pool_id, &handle);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    return sw_state_res_tag_bitmap_check_all_tag(unit, handle, tag, count, elem);
}

/*****************************************************************************/
/* } */
#else
int sw_state_resmgr_cfile;
#endif
