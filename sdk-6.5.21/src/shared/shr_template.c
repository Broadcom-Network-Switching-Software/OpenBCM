/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Global template management
 */

#include <shared/bsl.h>

#include <sal/core/sync.h>
#include <shared/shr_template.h>
#include <shared/error.h>

#if defined(BCM_PETRA_SUPPORT)
#include <soc/cm.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_multi_set.h>
#include <bcm_int/dpp/error.h>
#include <shared/swstate/access/sw_state_access.h>

/*****************************************************************************/
/*
 *  Internal implementation
 */

/* Sw state access macro. */
#define TEMPLATE_ACCESS sw_state_access[unit].dpp.shr.template_info


#define SHR_TEMPLATE_MAX_BUFFER_SIZE_FOR_PRINT  2048

/* A handle for a non-unit-based template manager instance */
typedef struct _shr_template_unit_desc_s *shr_mtemplate_handle_t;


/*
 *  Various function prototypes per method for the alloc managers.
 */
typedef int (*_shr_template_manage_create)(int unit,
                                           int pool_id,
                                         int template_low_id,
                                         int template_count,
                                         int max_entities,
                                         uint32 global_max,
                                         size_t data_size,
                                         const void *extras,
                                         const char *name);
typedef int (*_shr_template_manage_destroy)(int unit, int pool_id);
typedef int (*_shr_template_manage_alloc)(int unit, 
                                          int pool_id,
                                        uint32 flags,
                                        const void* data,
                                        int *is_allocated,
                                        int *template);
typedef int (*_shr_template_manage_alloc_group)(int unit, 
                                                int pool_id,
                                                uint32 flags,
                                                const void* data,
                                                int nof_additions,
                                                int *is_allocated,
                                                int *template);
typedef int (*_shr_template_manage_exchange)(int unit, 
                                             int pool_id,
                                            uint32 flags,
                                            const void *data,
                                            int old_template,
                                            int *is_last,
                                            int *template,
                                            int *is_allocated); 
typedef int (*_shr_template_manage_exchange_test)(int unit, 
                                                  int pool_id,
                                                uint32 flags,
                                                const void *data,
                                                int old_template,
                                                int *is_last,
                                                int *template,
                                                int *is_allocated);                       
typedef int (*_shr_template_manage_free)(int unit, 
                                         int pool_id,                                  
                                        int template,
                                        int *is_free);
typedef int (*_shr_template_manage_free_group)(int unit, 
                                               int pool_id,                                  
                                                int template,
                                                int nof_deductions,
                                                int *is_free);
typedef int (*_shr_template_manage_data_get)(int unit, 
                                             int pool_id,
                                            int template,
                                            void *data);
typedef int (*_shr_template_manage_index_get)(int unit, 
                                              int pool_id,
                                            const void *data, 
                                            int *template);
typedef int (*_shr_template_manage_ref_count_get)(int unit, 
                                                  int pool_id,
                                                int template,
                                                uint32 *ref_count);
typedef int (*_shr_template_manage_clear)(int unit, int pool_id);

/*
 *  This structure describes a single allocator mechanism, specifically by
 *  providing a set pointers to functions that are used to manipulate it.
 */
typedef struct _shr_template_managements_s {
    _shr_template_manage_create create;
    _shr_template_manage_destroy destroy;
    _shr_template_manage_alloc alloc;
    _shr_template_manage_alloc_group alloc_group;
    _shr_template_manage_exchange exchange;
    _shr_template_manage_exchange_test exchange_test;
    _shr_template_manage_free free;
    _shr_template_manage_free_group free_group;
    _shr_template_manage_data_get data_get;
    _shr_template_manage_ref_count_get ref_count_get;
    _shr_template_manage_clear clear;
    _shr_template_manage_index_get index_get;
    char *name;
} _shr_template_managements_t;

/*
 *  These prototypes are for the global const structure below that points to
 *  all of the various implementations.
 */
static int _shr_template_hash_create(int unit, 
                                     int pool_id,
                                     int template_low_id,
                                     int template_count,
                                     int max_entities,
                                     uint32 global_max,
                                     size_t data_size,
                                     const void *extras,
                                     const char *name);
static int _shr_template_hash_destroy(int unit, int pool_id);
static int _shr_template_hash_alloc(int unit, 
                                    int pool_id,
                                     uint32 flags,
                                     const void* data,
                                     int *is_allocated,
                                     int *template);
static int _shr_template_hash_alloc_group(int unit, 
                                          int pool_id,
                                         uint32 flags,
                                         const void* data,
                                         int nof_additions,
                                         int *is_allocated,
                                         int *template);
static int _shr_template_hash_free(int unit, 
                                   int pool_id,
                                    int template,
                                    int *is_last);
static int _shr_template_hash_free_group(int unit, 
                                         int pool_id,
                                        int template,
                                        int nof_deductions,
                                        int *is_last);
static int _shr_template_hash_exchange(int unit, 
                                       int pool_id,
                                        uint32 flags,
                                        const void *data,
                                        int old_template,
                                        int *is_last,
                                        int *template,
                                        int *is_allocated);
static int _shr_template_hash_exchange_test(int unit, 
                                            int pool_id,
                                            uint32 flags,
                                            const void *data,
                                            int old_template,
                                            int *is_last,
                                            int *template,
                                            int *is_allocated);
static int _shr_template_hash_data_get(int unit, 
                                       int pool_id,
                                        int template,
                                        void *data);
static int _shr_template_hash_ref_count_get(int unit, 
                                            int pool_id,
                                            int template,
                                            uint32 *ref_count);
static int _shr_template_hash_clear(int unit, int pool_id);
static int _shr_template_hash_index_get(int unit, 
                                        int pool_id,
                                        const void *data,
                                        int* template);
/*
 *  Global const structure describing the various allocator mechanisms.
 */
static const _shr_template_managements_t _shr_template_managements_mgrs[SHR_TEMPLATE_MANAGE_COUNT] =
    {
        {
            _shr_template_hash_create,
            _shr_template_hash_destroy,
            _shr_template_hash_alloc,
            _shr_template_hash_alloc_group,
            _shr_template_hash_exchange,
            _shr_template_hash_exchange_test,
            _shr_template_hash_free,
            _shr_template_hash_free_group,
            _shr_template_hash_data_get,
            _shr_template_hash_ref_count_get,
            _shr_template_hash_clear,
            _shr_template_hash_index_get,
            "SHR_TEMPLATE_MANAGE_HASH"
        } /* hash */
    };

/*
 * Global array holding the callbacks to the to_stream and from_stream functions. 
 * Should be filled when template manager is started. 
 */
static shr_template_to_stream_t     *hash_compare_to_stream_cb[BCM_MAX_NUM_UNITS];
static shr_template_from_stream_t   *hash_compare_from_stream_cb[BCM_MAX_NUM_UNITS];


/*
 *  Basic checks performed for many functions
 */
#define SW_STATE_ACCESS_ERROR_CHECK(rv) \
    if (rv != _SHR_E_NONE) {             \
        LOG_ERROR(BSL_LS_SOC_COMMON,    \
                  (BSL_META("Error in sw state access\n")));    \
        return _SHR_E_INTERNAL;  \
    }


#define SW_STATE_ACCESS_ERROR_CHECK_NO_RET(rv) \
    if (rv != _SHR_E_NONE) {             \
        LOG_ERROR(BSL_LS_SOC_COMMON,    \
                  (BSL_META("Error in sw state access\n")));    \
        rv = _SHR_E_INTERNAL;  \
        goto err;   \
    }

#define TEMPLATE_UNIT_CHECK(_unit) \
    {\
        uint8 _exists;\
        if ((0 > (_unit)) || (BCM_LOCAL_UNITS_MAX <= (_unit))) { \
            LOG_ERROR(BSL_LS_SOC_COMMON, \
            (BSL_META_U(_unit, \
                        "invalid unit number %d\n"), \
                       _unit)); \
            return BCM_E_PARAM; \
        } \
        SW_STATE_ACCESS_ERROR_CHECK(TEMPLATE_ACCESS.is_allocated(_unit, &_exists)); \
        if (!_exists) { \
            LOG_ERROR(BSL_LS_SOC_COMMON, \
            (BSL_META_U(_unit, \
                        "unit %d is not initialised\n"), \
                       _unit)); \
            return BCM_E_INIT; \
        } \
    }
    
#define TEMPLATE_POOL_VALID_CHECK(_unit, _pool) \
    {\
        uint16 _templatePoolCount;\
        SW_STATE_ACCESS_ERROR_CHECK(TEMPLATE_ACCESS.templatePoolCount.get(_unit, &_templatePoolCount)); \
        if ((0 > (_pool)) || (_templatePoolCount <= (_pool))) { \
            LOG_ERROR(BSL_LS_SOC_COMMON, \
                      (BSL_META("unit %d pool %d does not exist\n"), \
                       _unit, _pool)); \
            return BCM_E_PARAM; \
        }\
    }
#define TEMPLATE_POOL_EXIST_CHECK(_unit, _pool) \
    {\
        uint8 _exists;\
        SW_STATE_ACCESS_ERROR_CHECK(TEMPLATE_ACCESS.pool.is_allocated(_unit, _pool, &_exists)); \
        if (!_exists) { \
            LOG_ERROR(BSL_LS_SOC_COMMON, \
                      (BSL_META("unit %d pool %d is not configured\n"), \
                       _unit, _pool)); \
            return BCM_E_CONFIG; \
        }\
    }
#define TEMPLATE_TYPE_VALID_CHECK(_unit, _type) \
    {\
        uint16 _templateTypeCount;\
        SW_STATE_ACCESS_ERROR_CHECK(TEMPLATE_ACCESS.templateTypeCount.get(_unit, &_templateTypeCount)); \
        if ((0 > (_type)) || (_templateTypeCount <= (_type))) { \
            LOG_ERROR(BSL_LS_SOC_COMMON, \
                      (BSL_META("unit %d template %d does not exist\n"), \
                       _unit, _type)); \
            return BCM_E_PARAM; \
        }\
    }
#define TEMPLATE_TYPE_EXIST_CHECK(_unit, _type) \
    {\
        uint8 _exists;\
        SW_STATE_ACCESS_ERROR_CHECK(TEMPLATE_ACCESS.template_param.is_allocated(_unit, _type, &_exists)); \
        if (!_exists) { \
            LOG_ERROR(BSL_LS_SOC_COMMON, \
                      (BSL_META("unit %d template %d is not configured\n"), \
                       _unit, _type)); \
            return BCM_E_CONFIG; \
        }\
    }
#define TEMPLATE_PARAM_NULL_CHECK(_unit, _param) \
    if ((_param) == NULL) { \
        LOG_ERROR(BSL_LS_SOC_COMMON, \
                  (BSL_META("unit %d template with obligatory argument is NULL\n"), \
                   _unit)); \
        return BCM_E_PARAM;  \
    }
#define TEMPLATE_POOL_PARAM_TEMPLATE_ID_CHECK(_unit,_thisPool,_template) \
    {\
        int _template_low_id, _template_count; \
        SW_STATE_ACCESS_ERROR_CHECK(TEMPLATE_ACCESS.pool.template_low_id.get(_unit, _thisPool, &_template_low_id)); \
        SW_STATE_ACCESS_ERROR_CHECK(TEMPLATE_ACCESS.pool.template_count.get(_unit, _thisPool, &_template_count)); \
        if (!(_template >= _template_low_id && _template <= (_template_low_id + _template_count-1))) { \
            LOG_ERROR(BSL_LS_SOC_COMMON, \
                      (BSL_META("_template id %d is not in correct range. Should be between thisPool.template_low_id %d template_count %d\n") \
                       , _template, _template_low_id, (_template_low_id + _template_count-1))); \
            return BCM_E_PARAM;  \
        }\
    }

    
/* Mtemplate_* prototypes */
static int
shr_mtemplate_alloc(int unit, 
               int template_type,
               uint32 flags,
               const void *data,
               int *is_allocated,
               int *template);
static int
shr_mtemplate_alloc_group(int unit, 
               int template_type,
               uint32 flags,
               const void *data,
               int nof_additions,
               int *is_allocated,
               int *template);
static int
shr_mtemplate_free(int unit, 
               int template_type,
               int template,
               int *is_last);
static int
shr_mtemplate_free_group(int unit, 
               int template_type,
               int template,
               int nof_deductions,
               int *is_last);
static int
shr_mtemplate_exchange(int unit, 
                      int template_type,
                      uint32 flags,
                      const void *data,
                      int old_template,
                      int *is_last,
                      int *template,
                      int *is_allocated);
static int
shr_mtemplate_exchange_test(int unit, 
                          int template_type,
                          uint32 flags,
                          const void *data,
                          int old_template,
                          int *is_last,
                          int *template,
                          int *is_allocated);
static int
shr_mtemplate_data_get(int unit, 
                      int template_type,
                      int template,
                      void *data);
static int
shr_mtemplate_index_get(int unit,
                      int template_type,
                      const void *data, 
                      int *template);
static int
shr_mtemplate_ref_count_get(int unit,
                      int template_type,
                      int template,
                      uint32 *ref_count);
static int
shr_mtemplate_create(int unit,
                int num_template_types,
                int num_template_pools,
                shr_template_to_stream_t *to_stream_cb_arr,
                shr_template_from_stream_t *from_stream_cb_arr,
                int cb_arr_length);
static int
shr_mtemplate_get(int unit,
                 int *num_template_types,
                 int *num_template_pools);
static int
shr_mtemplate_pool_set(int unit,
                 int pool_id,
                 shr_template_manage_t manager,
                 int template_low_id,
                 int template_count,
                 int max_entities,
                 uint32 global_max,
                 size_t data_size,
                 const void *extras,
                 const char *name);
static int
shr_mtemplate_pool_get(int unit,
                      int pool_id,
                      shr_template_manage_t *manager,
                      int *template_low_id,
                      int *template_count,
                      int *max_entities,
                      size_t *data_size,
                      shr_template_manage_hash_compare_extras_t *extras,
                      sw_state_template_name_t *name);
static int
shr_mtemplate_pool_unset(int unit,
                    int pool_id);
static int
shr_mtemplate_type_set(int unit,
                      int template_type,
                      int pool_id,
                      const char *name);
static int
shr_mtemplate_type_get(int unit,
                  int template_type,
                  int *pool_id,
                  sw_state_template_name_t *name);
static int
shr_mtemplate_type_unset(int unit,
                    int template_id);
static int
_shr_mtemplate_destroy_data(int unit);
static int 
shr_mtemplate_dump(int unit, int template_type);
static int 
shr_mtemplate_clear(int unit, int template_type);

/*****************************************************************************/
/*
 *  Exposed API implementation (unit based)
 */

/*
 *      Initialize the tamplate manager for the unit
 */
int
shr_template_init(int unit,
                  int num_template_types,
                  int num_template_pools,
                  shr_template_to_stream_t *to_stream_cb_arr,
                  shr_template_from_stream_t *from_stream_cb_arr,
                  int cb_arr_length)
{
    int result = BCM_E_NONE;
    uint8 exists;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "(%d, %d, %d) enter\n"),
               unit,
               num_template_types,
               num_template_pools));

    /* a little parameter checking */
    if ((0 > unit) || (BCM_LOCAL_UNITS_MAX <= unit)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "invalid unit number %d\n"),
                   unit));
        return BCM_E_PARAM;
    }

    /* get this unit's current information, mark it as destroyed */
    result = TEMPLATE_ACCESS.is_allocated(unit, &exists);
    SW_STATE_ACCESS_ERROR_CHECK(result); 

    if (exists) {
        /* this unit has already been initialised; tear it down */
        result = _shr_mtemplate_destroy_data(unit);
    } /* if (exists) */


    if (BCM_E_NONE == result) {
        result = shr_mtemplate_create(unit,
                                 num_template_types,
                                 num_template_pools,
                                 to_stream_cb_arr,
                                 from_stream_cb_arr,
                                 cb_arr_length);
    } /* if (BCM_E_NONE == result) */

    /* return the result */
    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "(%d, %d, %d) return %d (%s)\n"),
               unit,
               num_template_types,
               num_template_pools,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

/*
 *      Get number of template pools and types configured for a unit
 */
int
shr_template_get(int unit,
                 int *num_template_types,
                 int *num_template_pools)
{
    /* a little parameter checking */
    TEMPLATE_UNIT_CHECK(unit);

    /* perform the fetch */
    return shr_mtemplate_get(unit, num_template_types, num_template_pools);
}

/*
 *      Configure a template pool for the unit
 */
int
shr_template_pool_set(int unit,
                 int pool_id,
                 shr_template_manage_t manager,
                 int template_low_id,
                 int template_count,
                 int max_entities,
                 uint32 global_max,
                 size_t data_size,
                 const void *extras,
                 const char *name)
{
    /* a little parameter checking */
    TEMPLATE_UNIT_CHECK(unit);

    /* perform the action */
    return shr_mtemplate_pool_set(unit,
                             pool_id,
                             manager,
                             template_low_id,
                             template_count,
                             max_entities,
                             global_max,
                             data_size,
                             extras,
                             name);
}

/*
 *      Get configuration for a resource pool on a particular unit
 */
int
shr_template_pool_get(int unit,
                      int pool_id,
                      shr_template_manage_t *manager,
                      int *template_low_id,
                      int *template_count,
                      int *max_entities,
                      size_t *data_size,
                      shr_template_manage_hash_compare_extras_t *extras,
                      sw_state_template_name_t *name)
{
    /* a little parameter checking */
    TEMPLATE_UNIT_CHECK(unit);

    /* perform the action */
    return shr_mtemplate_pool_get(unit,
                                 pool_id,
                                 manager,
                                 template_low_id,
                                 template_count,
                                 max_entities,
                                 data_size,
                                 extras,
                                 name);
}

/*
 *  Destroy a template pool on a unit
 */
int
shr_template_pool_unset(int unit,
                   int pool_id)
{
    /* a little parameter checking */
    TEMPLATE_UNIT_CHECK(unit);

    /* perform the action */
    return shr_mtemplate_pool_unset(unit, pool_id);
}

/*
 *      Configure a template type
 */
int
shr_template_type_set(int unit,
                      int template_type,
                      int pool_id,
                      const char *name)
{
    /* a little parameter checking */
    TEMPLATE_UNIT_CHECK(unit);

    /* perform the action */
    return shr_mtemplate_type_set(unit, template_type, pool_id, name);
}

/*
 *      Get information about a template type
 */
extern int
shr_template_type_get(int unit,
                      int template_type,
                      int *pool_id,
                      sw_state_template_name_t *name)
{
    /* a little parameter checking */
    TEMPLATE_UNIT_CHECK(unit);

    /* perform the action */
    return shr_mtemplate_type_get(unit, template_type, pool_id, name);
}


/*
 *  Destroy a template type on a unit
 */
int
shr_template_type_unset(int unit,
                   int template_id)
{
    /* a little parameter checking */
    TEMPLATE_UNIT_CHECK(unit);

    /* perform the action */
    return shr_mtemplate_type_unset(unit, template_id);
}

/*
 *      Remove all template management for a unit
 */
int
shr_template_detach(int unit)
{
    int result = BCM_E_NONE;
    uint8 exists;


    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "(%d) enter\n"),
               unit));

    /* get this unit's current information, mark it as destroyed */
    result = TEMPLATE_ACCESS.is_allocated(unit, &exists);
    SW_STATE_ACCESS_ERROR_CHECK(result); 

    if (exists) {
        /* this unit has already been initialised; tear it down */
        result = _shr_mtemplate_destroy_data(unit);
    } /* if (exists) */
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
 *      Find a template to assign to the requested data.
 */
int
shr_template_allocate(int unit,
                      int template_type,
                      uint32 flags,
                      const void *data,
                      int *is_allocated,
                      int *template)
{
    /* a little parameter checking */
    TEMPLATE_UNIT_CHECK(unit);

    /* perform the action */
    return shr_mtemplate_alloc(unit, template_type, flags, data, is_allocated, template);
}

/*
 *      Find a template to assign to the requested data for a group of elements.
 */
int
shr_template_allocate_group(int unit,
                      int template_type,
                      uint32 flags,
                      const void *data,
                      int nof_additions,
                      int *is_allocated,
                      int *template)
{
    /* a little parameter checking */
    TEMPLATE_UNIT_CHECK(unit);

    /* perform the action */
    return shr_mtemplate_alloc_group(unit, template_type, flags, data, nof_additions, is_allocated, template);
}

/*
 *      Release a template (referrer no longer points to it)
 */
extern int
shr_template_free(int unit,
                  int template_type,
                  int template,
                  int *is_last)
{
    /* a little parameter checking */
    TEMPLATE_UNIT_CHECK(unit);

    /* perform the action */
    return shr_mtemplate_free(unit, template_type, template, is_last);
}

/*
 *      Release several template referers (referrer no longer points to it)
 */
extern int
shr_template_free_group(int unit,
                  int template_type,
                  int template,
                  int nof_deductions,
                  int *is_last)
{
    /* a little parameter checking */
    TEMPLATE_UNIT_CHECK(unit);

    /* perform the action */
    return shr_mtemplate_free_group(unit, template_type, template, nof_deductions, is_last);
}

/*
 *      Free a current template and allocate a new one for new data
 */
int
shr_template_exchange(int unit,
                      int template_type,
                      uint32 flags,
                      const void *data,
                      int old_template,
                      int *is_last,
                      int *template,
                      int *is_allocated)
{
    /* a little parameter checking */
    TEMPLATE_UNIT_CHECK(unit);

    /* perform the action */
    return shr_mtemplate_exchange(unit, template_type, flags, data, old_template, is_last, template, is_allocated);
}

/*
 *      Tests if Free a current template and allocate a new one for new data is possible
 */
int
shr_template_exchange_test(int unit,
                      int template_type,
                      uint32 flags,
                      const void *data,
                      int old_template,
                      int *is_last,
                      int *template,
                      int *is_allocated)
{
    /* a little parameter checking */
    TEMPLATE_UNIT_CHECK(unit);

    /* perform the action */
    return shr_mtemplate_exchange_test(unit, template_type, flags, data, old_template, is_last, template, is_allocated);
}

/*
 *   Function
 *      shr_template_data_get
 *   Purpose
 *      Get the data from a specific template
 *   Parameters
 *      (IN) unit          : unit number of the device
 *      (IN) template_type : which template type to use
 *      (IN) template      : which template to get
 *      (OUT) data         : where to put the template's data
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 */
int
shr_template_data_get(int unit,
                      int template_type,
                      int template,
                      void *data)
{
    /* a little parameter checking */
    TEMPLATE_UNIT_CHECK(unit);

    /* perform the action */
    return shr_mtemplate_data_get(unit, template_type, template, data);
}

/*
 *   Function
 *      shr_template_template_get
 *   Purpose
 *      Get the template from a specific data
 *   Parameters
 *      (IN) unit          : unit number of the device
 *      (IN) template_type : which template type to use
 *      (IN) data         : the template's data
 *      (OUT) template      : which template
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 */
int
shr_template_template_get(int unit,
                      int template_type,
                      const void *data,
                      int *template)
{
    /* a little parameter checking */
    TEMPLATE_UNIT_CHECK(unit);

    /* perform the action */
    return shr_mtemplate_index_get(unit, template_type, data, template);
}


/*
 *   Function
 *      shr_template_ref_count_get
 *   Purpose
 *      Get the reference counter from a specific template id
 *   Parameters
 *      (IN) unit          : unit number of the device
 *      (IN) template_type : which template type to use
 *      (IN) template      : which template to get the reference count
 *      (OUT) ref_count    : where to put the template's reference count
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 */
int
shr_template_ref_count_get(int unit,
                      int template_type,
                      int template,
                      uint32 *ref_count)
{
    /* a little parameter checking */
    TEMPLATE_UNIT_CHECK(unit);

    /* perform the action */
    return shr_mtemplate_ref_count_get(unit, template_type, template, ref_count);
}

/*
 *   Function
 *      shr_template_dump
 *   Purpose
 *      Diagnostic dump of a unit's resource management information
 *   Parameters
 *      (IN) unit      : unit number of the device
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This displays the information using bsl_printf.
 */
int
shr_template_dump(int unit, int template_type)
{
    /* a little parameter checking */
    TEMPLATE_UNIT_CHECK(unit);

    /* perform the action */
    return shr_mtemplate_dump(unit, template_type);
}

/*
 *   Function
 *      shr_template_clear
 *   Purpose
 *      Free all templates located in this template_type
 *      (without free memory of the template resource)
 *   Parameters
 *      (IN) unit     	   : unit number of the device
 *      (IN) template_type : which template type to use
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 */
int
shr_template_clear(int unit,
                   int template_type)
{
    /* a little parameter checking */
    TEMPLATE_UNIT_CHECK(unit);

    /* perform the action */
    return shr_mtemplate_clear(unit,template_type);
}


static int
shr_mtmemplate_type_change_count(int unit, int template_type, int count){
    int refCount;
    int result = _SHR_E_NONE;
    
    result = TEMPLATE_ACCESS.template_param.refCount.get(unit, template_type, &refCount);
    SW_STATE_ACCESS_ERROR_CHECK(result); 

    refCount += count;
    
    /*
     * A free operation on an empty template would not return any indication, but would still try to 
     * reduce the refCount. This could go below zero, which is meaningless, so pervent it.
     */
    if (refCount < 0){
        refCount = 0;
    }

    result = TEMPLATE_ACCESS.template_param.refCount.set(unit, template_type, refCount);
    SW_STATE_ACCESS_ERROR_CHECK(result); 

    return result;
}

/*
 *  Allocate template of a resource type
 */
static int
shr_mtemplate_alloc(int unit, 
               int template_type,
               uint32 flags,
               const void *data,
               int *is_allocated,
               int *template)               
{
    int thisPool;
    int result = BCM_E_NONE;
    shr_template_manage_t mngr_type;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, %08X) enter\n"),
               unit,
               template_type,
               flags));
               

    /* a little parameter checking */
    TEMPLATE_TYPE_VALID_CHECK(unit, template_type);
    TEMPLATE_TYPE_EXIST_CHECK(unit, template_type);
    
    TEMPLATE_PARAM_NULL_CHECK(unit,data);
    TEMPLATE_PARAM_NULL_CHECK(unit,is_allocated);
    TEMPLATE_PARAM_NULL_CHECK(unit,template);

    if (flags & (~SHR_TEMPLATE_MANAGE_SINGLE_FLAGS)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("invalid flags %08X\n"),
                   flags & (~SHR_TEMPLATE_MANAGE_SINGLE_FLAGS)));
        return BCM_E_PARAM;
    }
    
    /* get the pool information */
    result = TEMPLATE_ACCESS.template_param.templatePoolId.get(unit, template_type, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result); 

    if (flags & SHR_TEMPLATE_MANAGE_SINGLE_FLAGS) {
        /* verify template id from pool information */
        TEMPLATE_POOL_PARAM_TEMPLATE_ID_CHECK(unit, thisPool,*template);
    }

    result = TEMPLATE_ACCESS.pool.templateManagerType.get(unit, thisPool, &mngr_type);
    SW_STATE_ACCESS_ERROR_CHECK(result); 

    /* make the call */
    result = _shr_template_managements_mgrs[mngr_type].alloc(unit,
                                                             thisPool,
                                                             flags,
                                                             data,
                                                             is_allocated,
                                                             template);

     if (BCM_E_NONE == result) {
        /* account for successful allocation */
        result = shr_mtmemplate_type_change_count(unit, template_type, 1);
        SW_STATE_ACCESS_ERROR_CHECK(result); 
    }

     LOG_DEBUG(BSL_LS_SOC_COMMON,
               (BSL_META("(%d, %d, %08X, %p, %d, %d) return %d (%s)\n"),
                unit,
                template_type,
                flags,
                (void *)data,
                *is_allocated,
                *template,
                result,
                _SHR_ERRMSG(result)));

    return result;
}

/*
 *  Allocate template of a resource type for a bunch of elements
 */
static int
shr_mtemplate_alloc_group(int unit, 
               int template_type,
               uint32 flags,
               const void *data,
               int nof_additions,
               int *is_allocated,
               int *template)               
{
    shr_template_manage_t mngr_type;
    int thisPool;
    int result = BCM_E_NONE;
    

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, %08X) enter\n"),
               unit,
               template_type,
               flags));
               

    /* a little parameter checking */
    TEMPLATE_TYPE_VALID_CHECK(unit, template_type);
    TEMPLATE_TYPE_EXIST_CHECK(unit, template_type);
    
    TEMPLATE_PARAM_NULL_CHECK(unit,data);
    TEMPLATE_PARAM_NULL_CHECK(unit,is_allocated);
    TEMPLATE_PARAM_NULL_CHECK(unit,template);

    if (flags & (~SHR_TEMPLATE_MANAGE_SINGLE_FLAGS)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("invalid flags %08X\n"),
                   flags & (~SHR_TEMPLATE_MANAGE_SINGLE_FLAGS)));
        return BCM_E_PARAM;
    }

    
    if ((flags & (SHR_TEMPLATE_MANAGE_SET_WITH_ID)) == 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("WITH_ID must be specify %08X\n"),
                   flags));
        return BCM_E_PARAM;
    }
    
    /* get the pool information */
    result = TEMPLATE_ACCESS.template_param.templatePoolId.get(unit, template_type, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result); 

    if (flags & SHR_TEMPLATE_MANAGE_SINGLE_FLAGS) {
        /* verify template id from pool information */
        TEMPLATE_POOL_PARAM_TEMPLATE_ID_CHECK(unit, thisPool,*template);
    }

    result = TEMPLATE_ACCESS.pool.templateManagerType.get(unit, thisPool, &mngr_type);
    SW_STATE_ACCESS_ERROR_CHECK(result); 
    /* make the call */
    result = _shr_template_managements_mgrs[mngr_type].alloc_group(unit,
                                                                     thisPool,
                                                                     flags,
                                                                     data,
                                                                     nof_additions,
                                                                     is_allocated,
                                                                     template);

     if (BCM_E_NONE == result) {
        /* account for successful allocation */
        result = shr_mtmemplate_type_change_count(unit, template_type, nof_additions);
        SW_STATE_ACCESS_ERROR_CHECK(result); 
    }

     LOG_DEBUG(BSL_LS_SOC_COMMON,
               (BSL_META("(%d, %d, %08X, %p, %d, %d, %d) return %d (%s)\n"),
                unit,
                template_type,
                flags,
                (void *)data,
                nof_additions,
                *is_allocated,
                *template,
                result,
                _SHR_ERRMSG(result)));

    return result;
}

/* Free template (if needed) and decrement reference of the given resource */
static int
shr_mtemplate_free(int unit,
               int template_type,
               int template,
               int *is_last)
{
    shr_template_manage_t mngr_type;
    int  thisPool;
    int result = BCM_E_NONE;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, %d) enter\n"),
               unit,
               template_type,
               template));
               

    /* a little parameter checking */
    TEMPLATE_TYPE_VALID_CHECK(unit, template_type);
    TEMPLATE_TYPE_EXIST_CHECK(unit, template_type);

    TEMPLATE_PARAM_NULL_CHECK(unit,is_last);
    
    result = TEMPLATE_ACCESS.template_param.templatePoolId.get(unit, template_type, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result); 

    /* verify template id from pool information */
    TEMPLATE_POOL_PARAM_TEMPLATE_ID_CHECK(unit, thisPool,template);

    result = TEMPLATE_ACCESS.pool.templateManagerType.get(unit, thisPool, &mngr_type);
    SW_STATE_ACCESS_ERROR_CHECK(result); 

    /* make the call */
    result = _shr_template_managements_mgrs[mngr_type].free(unit,
                                                            thisPool,
                                                            template,
                                                            is_last);

    if (BCM_E_NONE == result) {
        /* account for successful deallocation */
        result = shr_mtmemplate_type_change_count(unit, template_type, -1);
        SW_STATE_ACCESS_ERROR_CHECK(result); 
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, %d, %d) return %d (%s)\n"),
               unit,
               template_type,
               template,
               *is_last,
               result,
               _SHR_ERRMSG(result)));
    return result;
}


/* Free template (if needed) and decrement reference of the given resource */
static int
shr_mtemplate_free_group(int unit, 
               int template_type,
               int template,
               int nof_deductions,
               int *is_last)
{
    shr_template_manage_t mngr_type;
    int thisPool;
    int result = BCM_E_NONE;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, %d) enter\n"),
               unit,
               template_type,
               template));
               

    /* a little parameter checking */
    TEMPLATE_TYPE_VALID_CHECK(unit, template_type);
    TEMPLATE_TYPE_EXIST_CHECK(unit, template_type);

    TEMPLATE_PARAM_NULL_CHECK(unit,is_last);
    
    result = TEMPLATE_ACCESS.template_param.templatePoolId.get(unit, template_type, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result); 

    /* verify template id from pool information */
    TEMPLATE_POOL_PARAM_TEMPLATE_ID_CHECK(unit, thisPool,template);
    
    /* make the call */
    result = TEMPLATE_ACCESS.pool.templateManagerType.get(unit, thisPool, &mngr_type);
    SW_STATE_ACCESS_ERROR_CHECK(result); 
    result = _shr_template_managements_mgrs[mngr_type].free_group(unit,
                                                                thisPool,
                                                                template,
                                                                nof_deductions,
                                                                is_last);

    if (BCM_E_NONE == result) {
        /* account for successful deallocation */
        result = shr_mtmemplate_type_change_count(unit, template_type, -nof_deductions);
        SW_STATE_ACCESS_ERROR_CHECK(result); 
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, %d, %d) return %d (%s)\n"),
               unit,
               template_type,
               template,
               *is_last,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

static int
shr_mtemplate_exchange(int unit, 
                      int template_type,
                      uint32 flags,
                      const void *data,
                      int old_template,
                      int *is_last,
                      int *template,
                      int *is_allocated)
{
    shr_template_manage_t mngr_type;
    int thisPool;
    int result = BCM_E_NONE;
    uint32 valid_flags = SHR_TEMPLATE_MANAGE_SINGLE_FLAGS | SHR_TEMPLATE_MANAGE_SET_WITH_ID | SHR_TEMPLATE_MANAGE_IGNORE_DATA;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, %08X, %d) enter\n"),
               unit,
               template_type,
               flags,
               old_template));
               

    /* a little parameter checking */
    TEMPLATE_TYPE_VALID_CHECK(unit, template_type);
    TEMPLATE_TYPE_EXIST_CHECK(unit, template_type);
    
    TEMPLATE_PARAM_NULL_CHECK(unit,data);
    TEMPLATE_PARAM_NULL_CHECK(unit,is_last);
    TEMPLATE_PARAM_NULL_CHECK(unit,is_allocated);
    TEMPLATE_PARAM_NULL_CHECK(unit,template);

    if (flags & ~valid_flags) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("invalid flags %08X\n"),
                   flags & ~valid_flags));
        return BCM_E_PARAM;
    }

    if ((flags & SHR_TEMPLATE_MANAGE_IGNORE_DATA) && !(flags & SHR_TEMPLATE_MANAGE_SET_WITH_ID)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("SHR_TEMPLATE_MANAGE_IGNORE_DATA is only valid in combination with SHR_TEMPLATE_MANAGE_SET_WITH_ID\n")));
        return BCM_E_PARAM;
    }
    
    /* get the pool information */
    result = TEMPLATE_ACCESS.template_param.templatePoolId.get(unit, template_type, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result); 


    /* verify template id from pool information */
    TEMPLATE_POOL_PARAM_TEMPLATE_ID_CHECK(unit, thisPool,old_template);
    /* make the call */
    result = TEMPLATE_ACCESS.pool.templateManagerType.get(unit, thisPool, &mngr_type);
    SW_STATE_ACCESS_ERROR_CHECK(result); 
    result = _shr_template_managements_mgrs[mngr_type].exchange(unit,
                                                                thisPool,
                                                                 flags,
                                                                 data,
                                                                 old_template,
                                                                 is_last,
                                                                 template,
                                                                 is_allocated);

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, %08X, %d, %d) return %d (%s)\n"),
               unit,
               template_type,
               flags,
               old_template,
               *is_last,
               result,
               _SHR_ERRMSG(result)));

    return result;
}

static int
shr_mtemplate_exchange_test(int unit, 
                      int template_type,
                      uint32 flags,
                      const void *data,
                      int old_template,
                      int *is_last,
                      int *template,
                      int *is_allocated)
{
    shr_template_manage_t mngr_type;
    int thisPool;
    int result = BCM_E_NONE;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, %08X, %d) enter\n"),
               unit,
               template_type,
               flags,
               old_template));
               

    /* a little parameter checking */
    TEMPLATE_TYPE_VALID_CHECK(unit, template_type);
    TEMPLATE_TYPE_EXIST_CHECK(unit, template_type);
    
    TEMPLATE_PARAM_NULL_CHECK(unit,data);
    TEMPLATE_PARAM_NULL_CHECK(unit,is_last);
    TEMPLATE_PARAM_NULL_CHECK(unit,is_allocated);
    TEMPLATE_PARAM_NULL_CHECK(unit,template);

    if (flags & (~SHR_TEMPLATE_MANAGE_SINGLE_FLAGS)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("invalid flags %08X\n"),
                   flags & (~SHR_TEMPLATE_MANAGE_SINGLE_FLAGS)));
        return BCM_E_PARAM;
    }
    
    /* get the pool information */
    result = TEMPLATE_ACCESS.template_param.templatePoolId.get(unit, template_type, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result); 


    /* verify template id from pool information */
    TEMPLATE_POOL_PARAM_TEMPLATE_ID_CHECK(unit, thisPool,old_template);
    /* make the call */
    result = TEMPLATE_ACCESS.pool.templateManagerType.get(unit, thisPool, &mngr_type);
    SW_STATE_ACCESS_ERROR_CHECK(result); 
    result = _shr_template_managements_mgrs[mngr_type].exchange_test(unit,
                                                                     thisPool,
                                                                     flags,
                                                                     data,
                                                                     old_template,
                                                                     is_last,
                                                                     template,
                                                                     is_allocated);

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, %08X, %d, %d) return %d (%s)\n"),
               unit,
               template_type,
               flags,
               old_template,
               *is_last,
               result,
               _SHR_ERRMSG(result)));

    return result;
}

static int
shr_mtemplate_data_get(int unit, 
                      int template_type,
                      int template,
                      void *data)
{
    shr_template_manage_t mngr_type;
    int thisPool;
    int result = BCM_E_NONE;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, %d) enter\n"),
               unit,
               template_type,
               template));
               

    /* a little parameter checking */
    TEMPLATE_TYPE_VALID_CHECK(unit, template_type);
    TEMPLATE_TYPE_EXIST_CHECK(unit, template_type);
    
    TEMPLATE_PARAM_NULL_CHECK(unit,data);
    /* get the pool information */
    result = TEMPLATE_ACCESS.template_param.templatePoolId.get(unit, template_type, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result); 


    /* verify template id from pool information */
    TEMPLATE_POOL_PARAM_TEMPLATE_ID_CHECK(unit, thisPool,template);

    /* make the call */
    result = TEMPLATE_ACCESS.pool.templateManagerType.get(unit, thisPool, &mngr_type);
    SW_STATE_ACCESS_ERROR_CHECK(result); 
    result = _shr_template_managements_mgrs[mngr_type].data_get(unit,
                                                                thisPool,
                                                                template,
                                                                data);

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, %d) return %d (%s)\n"),
               unit,
               template_type,
               template,
               result,
               _SHR_ERRMSG(result)));

    return result;
}

static int
shr_mtemplate_index_get(int unit, 
                      int template_type,
                      const void *data, 
                      int *template)
{
    shr_template_manage_t mngr_type;
    int thisPool;
    int result = BCM_E_NONE;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d) enter\n"),
               unit,
               template_type));


    /* a little parameter checking */
    TEMPLATE_TYPE_VALID_CHECK(unit, template_type);
    TEMPLATE_TYPE_EXIST_CHECK(unit, template_type);

    TEMPLATE_PARAM_NULL_CHECK(unit,data);
    /* get the pool information */
    result = TEMPLATE_ACCESS.template_param.templatePoolId.get(unit, template_type, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result); 

    /* make the call */
    result = TEMPLATE_ACCESS.pool.templateManagerType.get(unit, thisPool, &mngr_type);
    SW_STATE_ACCESS_ERROR_CHECK(result); 
    result = _shr_template_managements_mgrs[mngr_type].index_get(unit,
                                                                 thisPool,
                                                                 data,
                                                                 template);

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, %d) return %d (%s)\n"),
               unit,
               template_type,
               *template,
               result,
               _SHR_ERRMSG(result)));

    return result;
}


static int
shr_mtemplate_ref_count_get(int unit, 
                            int template_type,
                            int template,
                            uint32 *ref_count)
{
    shr_template_manage_t mngr_type;
    int thisPool;
    int result = BCM_E_NONE;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, %d) enter\n"),
               unit,
               template_type,
               template));
               

    /* a little parameter checking */
    TEMPLATE_TYPE_VALID_CHECK(unit, template_type);
    TEMPLATE_TYPE_EXIST_CHECK(unit, template_type);
    
    TEMPLATE_PARAM_NULL_CHECK(unit,ref_count);

    /* get the pool information */
    result = TEMPLATE_ACCESS.template_param.templatePoolId.get(unit, template_type, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result); 

    /* verify template id from pool information */
    TEMPLATE_POOL_PARAM_TEMPLATE_ID_CHECK(unit, thisPool,template);
    /* make the call */
    result = TEMPLATE_ACCESS.pool.templateManagerType.get(unit, thisPool, &mngr_type);
    SW_STATE_ACCESS_ERROR_CHECK(result); 
    result = _shr_template_managements_mgrs[mngr_type].ref_count_get(unit, 
                                                                     thisPool,
                                                                     template,
                                                                     ref_count);

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, %d) return %d (%s)\n"),
               unit,
               template_type,
               template,
               result,
               _SHR_ERRMSG(result)));

    return result;
}

static int
shr_mtemplate_create(int unit, 
                int num_template_types,
                int num_template_pools,
                shr_template_to_stream_t *to_stream_cb_arr,
                shr_template_from_stream_t *from_stream_cb_arr,
                int cb_arr_length)
{
    int result = BCM_E_NONE;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, %d) enter\n"),
               unit,
               num_template_types,
               num_template_pools));

    /* a little parameter checking */
    if (1 > num_template_pools) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("resource pools %d; must be > 0\n"),
                   num_template_pools));
        result =  BCM_E_PARAM;
    }
    if (1 > num_template_types) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("resource types %d; must be > 0\n"),
                   num_template_types));
        result =  BCM_E_PARAM;
    }
    if (BCM_E_NONE != result) {
        /* displayed diagnostics above */
        return result;
    }
    /* set things up */

    if (cb_arr_length > 0) {
        /*
         * Allocate and copy the callback arrays. We can this on warmboot because callbacks are static and don't change between runs.
         */
        if (!hash_compare_to_stream_cb[unit]) {
            hash_compare_to_stream_cb[unit] = to_stream_cb_arr;
        }

        if (!hash_compare_from_stream_cb[unit]) {
            hash_compare_from_stream_cb[unit] = from_stream_cb_arr;
        }
    }


    if (!SOC_WARM_BOOT(unit)) {
        result = TEMPLATE_ACCESS.alloc(unit);
        SW_STATE_ACCESS_ERROR_CHECK_NO_RET(result); 

        result = TEMPLATE_ACCESS.pool.ptr_alloc(unit, num_template_pools);
        SW_STATE_ACCESS_ERROR_CHECK_NO_RET(result); 

        result = TEMPLATE_ACCESS.template_param.ptr_alloc(unit, num_template_types);
        SW_STATE_ACCESS_ERROR_CHECK_NO_RET(result); 

        result = TEMPLATE_ACCESS.templateTypeCount.set(unit, num_template_types);
        SW_STATE_ACCESS_ERROR_CHECK_NO_RET(result); 

        result = TEMPLATE_ACCESS.templatePoolCount.set(unit, num_template_pools);
        SW_STATE_ACCESS_ERROR_CHECK_NO_RET(result); 
    }
err:
    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, %d) return %d (%s)\n"),
               unit,
               num_template_types,
               num_template_pools,
               result,
               _SHR_ERRMSG(result)));
    return result;
}


static int
shr_mtemplate_get(int unit, 
                 int *num_template_types,
                 int *num_template_pools)
{
    uint16 tmp;
    int result = BCM_E_NONE;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %p, %p) enter\n"),
               unit,
               (void *)num_template_types,
               (void *)num_template_pools));

    /* a little parameter checking */
    /* return the requested information */
    if (num_template_pools) {
        result = TEMPLATE_ACCESS.templatePoolCount.get(unit, &tmp);
        SW_STATE_ACCESS_ERROR_CHECK(result); 
        *num_template_pools = tmp;
    }
    if (num_template_types) {
        result = TEMPLATE_ACCESS.templateTypeCount.get(unit, &tmp);
        SW_STATE_ACCESS_ERROR_CHECK(result); 
        *num_template_types = tmp;
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, &(%d), &(%d)) return %d (%s)\n"),
               unit,
               num_template_types?*num_template_types:-1,
               num_template_pools?*num_template_pools:-1,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

static int
shr_mtemplate_pool_set(int unit, 
                 int pool_id,
                 shr_template_manage_t manager,
                 int template_low_id,
                 int template_count,
                 int max_entities,
                 uint32 global_max,
                 size_t data_size,
                 const void *extras,
                 const char *name)
{
    int result = BCM_E_NONE;
    uint8 exists;
    const char *noname = "???";
    sw_state_template_name_t name_handle;
    int name_len;

    sal_memset(&name_handle, 0, sizeof(name_handle));

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, %s, %d, %d, %d, %d, %p, \"%s\") enter\n"),
               unit,
               pool_id,
               ((0 <= manager) && (SHR_TEMPLATE_MANAGE_COUNT > manager))?_shr_template_managements_mgrs[manager].name:"INVALID",
               template_low_id,
               template_count,
               max_entities,
               (int)data_size,
               (void *)extras,
               name?name:noname));

    /* a little parameter checking */
    if (SOC_WARM_BOOT(unit)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("Can't create templates during init\n")));
        return _SHR_E_DISABLED;
    }
    TEMPLATE_POOL_VALID_CHECK(unit,pool_id);
    if ((0 > manager) || (SHR_TEMPLATE_MANAGE_COUNT <= manager)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("template manager type %d not supported\n"),
                   manager));
        return BCM_E_PARAM;
    }
    if (0 >= data_size) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("data size must be positive\n")));
        return BCM_E_PARAM;
    }
    if (0 >= max_entities) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("maximum entities must be positive\n")));
        return BCM_E_PARAM;
    }
    if (0 > template_low_id) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("template low id cant be negative \n")));
        return BCM_E_PARAM;
    }
    if (0 >= template_count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("template count must be positive \n")));
        return BCM_E_PARAM;
    }
    name_len = name ? sal_strlen(name) : 0;
    if (name_len>= SW_STATE_TEMPLATE_NAME_MAX_LENGTH) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("Name string \"%s\" too long. Must be shorter than %d "
                            "(including terminal \\0)\n"),
                   name,
                   SW_STATE_RES_NAME_MAX_LENGTH));
        return _SHR_E_PARAM;
    }


    TEMPLATE_PARAM_NULL_CHECK(unit,extras);

    result = TEMPLATE_ACCESS.pool.is_allocated(unit, pool_id, &exists);
    SW_STATE_ACCESS_ERROR_CHECK(result); 

    if (exists) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("%d pool %d already exists. "
                            "Please free it before reallocating.\n"),
                   unit,
                   pool_id));
        return BCM_E_CONFIG;
    } else {
        /* Allocate the pool. */
        result = TEMPLATE_ACCESS.pool.alloc(unit, pool_id);
        SW_STATE_ACCESS_ERROR_CHECK(result); 
    }
    
    /* create the new pool */
    result = _shr_template_managements_mgrs[manager].create(unit,
                                                         pool_id,
                                                         template_low_id,
                                                         template_count,
                                                         max_entities,
                                                         global_max,
                                                         data_size,
                                                         extras,
                                                         name?name:noname);
    if (BCM_E_NONE == result) {
        /* New one created successfully. Fill the information.  */
        result = TEMPLATE_ACCESS.pool.templateManagerType.set(unit, pool_id, manager);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        result = TEMPLATE_ACCESS.pool.template_low_id.set(unit, pool_id, template_low_id);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        result = TEMPLATE_ACCESS.pool.template_count.set(unit, pool_id, template_count);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        result = TEMPLATE_ACCESS.pool.max_entities.set(unit, pool_id, max_entities);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        result = TEMPLATE_ACCESS.pool.data_size.set(unit, pool_id, data_size);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        sal_memcpy(name_handle.name, name?name:noname, sal_strlen(name?name:noname) + 1);

        result = TEMPLATE_ACCESS.pool.name.set(unit, pool_id, &name_handle);
        SW_STATE_ACCESS_ERROR_CHECK(result);

    } /* if (BCM_E_NONE == result) */

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, %s, %d, %d, %d, %d, %p, \"%s\") return %d (%s)\n"),
               unit,
               pool_id,
               _shr_template_managements_mgrs[manager].name,
               template_low_id,
               template_count,
               max_entities,
               (int)data_size,
               (void *)extras,
               name?name:noname,
               result,
               _SHR_ERRMSG(result)));

    return result;
}

static int
shr_mtemplate_pool_get(int unit, 
                      int pool_id,
                      shr_template_manage_t *manager,
                      int *template_low_id,
                      int *template_count,
                      int *max_entities,
                      size_t *data_size,
                      shr_template_manage_hash_compare_extras_t *extras,
                      sw_state_template_name_t *name)
{
    int result;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, %p, %p, %p, %p, %p, %p, %p) enter\n"),
               unit,
               pool_id,
               (void *)manager,
               (void *)template_low_id,
               (void *)template_count,
               (void *)max_entities,
               (void *)data_size,
               (void *)extras,
               (void *)name));

    /* a little parameter checking */
    TEMPLATE_POOL_VALID_CHECK(unit, pool_id);
    TEMPLATE_POOL_EXIST_CHECK(unit, pool_id);
    /* fill in the caller's request */
    if (manager) {
        result = TEMPLATE_ACCESS.pool.templateManagerType.get(unit, pool_id, manager);
        SW_STATE_ACCESS_ERROR_CHECK(result); 
    }
    if (template_low_id) {
        result = TEMPLATE_ACCESS.pool.template_low_id.get(unit, pool_id, template_low_id);
        SW_STATE_ACCESS_ERROR_CHECK(result); 
    }
    if (template_count) {
        result = TEMPLATE_ACCESS.pool.template_count.get(unit, pool_id, template_count);
        SW_STATE_ACCESS_ERROR_CHECK(result); 
    }
    if (max_entities) {
        result = TEMPLATE_ACCESS.pool.max_entities.get(unit, pool_id, max_entities);
        SW_STATE_ACCESS_ERROR_CHECK(result); 
    }
    if (data_size) {
        result = TEMPLATE_ACCESS.pool.data_size.get(unit, pool_id, data_size);
        SW_STATE_ACCESS_ERROR_CHECK(result); 
    }
    if (extras) {
        result = TEMPLATE_ACCESS.pool.extras.get(unit, pool_id, extras);
        SW_STATE_ACCESS_ERROR_CHECK(result); 
    }
    if (name) {
        result = TEMPLATE_ACCESS.pool.name.get(unit, pool_id, name);
        SW_STATE_ACCESS_ERROR_CHECK(result); 
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, &(%s), &(%d), &(%d), &(%d), &(%d), &(%p), &(\"%s\")) return %d (%s)\n"),
               unit,
               pool_id,
               manager?_shr_template_managements_mgrs[*manager].name:"NULL",
               template_low_id?*template_low_id:0,
               template_count?*template_count:0,
               max_entities?*max_entities:0,
               data_size?(int)*data_size:0,
               (void *)(extras?extras:NULL),
               name?name->name:"NULL",
               BCM_E_NONE,
               _SHR_ERRMSG(BCM_E_NONE)));
    return BCM_E_NONE;
}

/*
 *  Destroy a resource pool on a unit
 */
static int
shr_mtemplate_pool_unset(int unit,
                    int pool_id)
{
    _shr_template_pool_desc_t oldPool;
    int result = _SHR_E_NONE;
    uint8 exists;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d) enter\n"),
               unit, pool_id));

    /* a little parameter checking */
    TEMPLATE_POOL_VALID_CHECK(unit, pool_id);

    result = TEMPLATE_ACCESS.pool.is_allocated(unit, pool_id, &exists);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    if (exists) {

        result = TEMPLATE_ACCESS.pool.get(unit, pool_id, &oldPool);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        if (oldPool.refCount) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("unit %d: pool %d (%s) can not be destroyed because"
                       " it has %d types that use it\n"),
                       unit,
                       pool_id,
                       oldPool.name.name,
                       oldPool.refCount));
            result = _SHR_E_CONFIG;
        } else { /* if (oldPool->refCount) */
            result = _shr_template_managements_mgrs[oldPool.templateManagerType].destroy(unit, pool_id);
            if (_SHR_E_NONE != result) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META("unit %d: unable to destroy old pool %d (%s):"
                           " %d (%s)\n"),
                           unit,
                           pool_id,
                           oldPool.name.name,
                           result,
                           _SHR_ERRMSG(result)));
            } else {
                result = TEMPLATE_ACCESS.pool.free(unit, pool_id);
                SW_STATE_ACCESS_ERROR_CHECK(result);
            }/* if (_SHR_E_NONE != result) */
        } /* if (oldPool.refCount) */
    } /* if (exists) */

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d) return %d (%s)\n"),
               unit,
               pool_id,
               result,
               _SHR_ERRMSG(result)));
    return result;
}


static int
shr_mtemplate_type_set(int unit, 
                      int template_type,
                      int pool_id,
                      const char *name)
{
    int result = BCM_E_NONE;
    const char *noname = "???";
    int len_name = 0;
    uint8 exists;
    sw_state_template_name_t name_handle;

    sal_memset(&name_handle, 0, sizeof(name_handle));

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, %d, \"%s\") enter\n"),
               unit,
               template_type,
               pool_id,
               name?name:noname));
    if (SOC_WARM_BOOT(unit)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("Can't create templates during init\n")));
        return _SHR_E_DISABLED;
    }

    /* a little parameter checking */
    TEMPLATE_POOL_VALID_CHECK(unit, pool_id);
    TEMPLATE_POOL_EXIST_CHECK(unit, pool_id);
    TEMPLATE_TYPE_VALID_CHECK(unit, template_type);


    result = TEMPLATE_ACCESS.template_param.is_allocated(unit, template_type, &exists);
    SW_STATE_ACCESS_ERROR_CHECK(result); 
    if (exists) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("%d template type %d already exists."
                            "Unset it before setting again.\n"),
                   unit,
                   template_type));
        return BCM_E_CONFIG;
    }
    if (!name) {
        /* force a non-NULL name pointer */
        name = noname;
    }

    len_name = sal_strlen(name);
    if (len_name>= SW_STATE_TEMPLATE_NAME_MAX_LENGTH) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("Name string \"%s\" too long. Must be shorter than %d "
                            "(including terminal \\0)\n"),
                   name,
                   SW_STATE_RES_NAME_MAX_LENGTH));
        return _SHR_E_PARAM;
    }


    /* note base type includes one character, so don't need to add NUL here */
    result = TEMPLATE_ACCESS.template_param.alloc(unit, template_type);
    SW_STATE_ACCESS_ERROR_CHECK(result); 

    result = TEMPLATE_ACCESS.template_param.templatePoolId.set(unit, template_type, pool_id);
    SW_STATE_ACCESS_ERROR_CHECK(result); 

    sal_strncpy(name_handle.name, name, len_name + 1);

    result = TEMPLATE_ACCESS.template_param.name.set(unit, template_type, &name_handle);
    SW_STATE_ACCESS_ERROR_CHECK(result); 


    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, %d, \"%s\") return %d (%s)\n"),
               unit,
               template_type,
               pool_id,
               name?name:noname,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

static int
shr_mtemplate_type_get(int unit, 
                  int template_type,
                  int *pool_id,
                  sw_state_template_name_t *name)
{
    int result;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, %p, %p) enter\n"),
               unit,
               template_type,
               (void *)pool_id,
               (void *)name));

    /* a little parameter checking */
    TEMPLATE_TYPE_VALID_CHECK(unit, template_type);
    TEMPLATE_TYPE_EXIST_CHECK(unit, template_type);
    /* fill in the caller's request */
    if (pool_id) {
        result = TEMPLATE_ACCESS.template_param.templatePoolId.get(unit, template_type, pool_id);
        SW_STATE_ACCESS_ERROR_CHECK(result); 
    }
    if (name) {
        result = TEMPLATE_ACCESS.template_param.name.get(unit, template_type, name);
        SW_STATE_ACCESS_ERROR_CHECK(result); 
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d, &(%d), &(\"%s\")) return %d (%s)\n"),
               unit,
               template_type,
               pool_id ? *pool_id : 0,
               name ? name->name : "NULL",
               BCM_E_NONE,
               _SHR_ERRMSG(BCM_E_NONE)));
    return BCM_E_NONE;
}


/*
 *  Destroy a resource type on a unit
 */
static int
shr_mtemplate_type_unset(int unit,
                    int template_id)
{
    _shr_template_type_desc_t oldType;
    int result = _SHR_E_NONE;
    uint8 exists;
    int refCount;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d) enter\n"),
               unit, template_id));

    /* a little parameter checking */
    TEMPLATE_TYPE_VALID_CHECK(unit, template_id);
    
    result = TEMPLATE_ACCESS.template_param.is_allocated(unit, template_id, &exists);
    SW_STATE_ACCESS_ERROR_CHECK(result);
    if (exists) {
        result = TEMPLATE_ACCESS.template_param.get(unit, template_id, &oldType);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        if (oldType.refCount) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("%d resource %d (%s) can not be destroyed"
                       " because it has %d elements in use\n"),
                       unit,
                       template_id,
                       oldType.name.name,
                       oldType.refCount));
            result = _SHR_E_CONFIG;
        } else {
            result = TEMPLATE_ACCESS.pool.refCount.get(unit, oldType.templatePoolId, &refCount);
            SW_STATE_ACCESS_ERROR_CHECK(result);

            refCount--;

            result = TEMPLATE_ACCESS.pool.refCount.set(unit, oldType.templatePoolId, refCount);
            SW_STATE_ACCESS_ERROR_CHECK(result);

            result = TEMPLATE_ACCESS.template_param.free(unit, template_id);
            SW_STATE_ACCESS_ERROR_CHECK(result);
        }
    } /* if (oldType) */

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d) return %d (%s)\n"),
               unit,
               template_id,
               result,
               _SHR_ERRMSG(result)));
    return result;
}


/*
 *  Destroys all of the resources and then pools for a unit.
 */
static int
_shr_mtemplate_destroy_data(int unit)
{
    int i;
    int result = BCM_E_NONE;
    uint16 templateTypeCount, templatePoolCount;
    uint8 exists;
    _shr_template_type_desc_t type;
    _shr_template_pool_desc_t pool;
    int refCount;

    if (!SOC_IS_DETACHING(unit) && !SOC_WARM_BOOT(unit)) {
        result = TEMPLATE_ACCESS.templateTypeCount.get(unit, &templateTypeCount);
        SW_STATE_ACCESS_ERROR_CHECK(result); 

        result = TEMPLATE_ACCESS.templatePoolCount.get(unit, &templatePoolCount);
        SW_STATE_ACCESS_ERROR_CHECK(result); 


        /* destroy resources */
        for (i = 0; i < templateTypeCount; i++) {
            result = TEMPLATE_ACCESS.template_param.is_allocated(unit, i, &exists);
            SW_STATE_ACCESS_ERROR_CHECK(result);

            if (exists) {
                result = TEMPLATE_ACCESS.template_param.get(unit, i, &type);
                SW_STATE_ACCESS_ERROR_CHECK(result);
                if (type.refCount) {
                    LOG_WARN(BSL_LS_SOC_COMMON,
                             (BSL_META("unit %d: type %d (%s): still in use (%d)\n"),
                              unit,
                              i,
                              (type.name.name),
                              type.refCount));
                }

                result = TEMPLATE_ACCESS.pool.refCount.get(unit, type.templatePoolId, &refCount);
                SW_STATE_ACCESS_ERROR_CHECK(result);
                
                refCount--;

                result = TEMPLATE_ACCESS.pool.refCount.set(unit, type.templatePoolId, refCount);
                SW_STATE_ACCESS_ERROR_CHECK(result);

                result = TEMPLATE_ACCESS.template_param.free(unit, i);
                SW_STATE_ACCESS_ERROR_CHECK(result);
            }
        } /* for (all resources this unit) */

        /* Fress the resources pointer array. */
        result = TEMPLATE_ACCESS.template_param.ptr_free(unit);
        SW_STATE_ACCESS_ERROR_CHECK(result);


        /* destroy pools */
        for (i = 0;
             (i < templatePoolCount) && (BCM_E_NONE == result);
             i++) {

            result = TEMPLATE_ACCESS.pool.is_allocated(unit, i, &exists);
            SW_STATE_ACCESS_ERROR_CHECK(result);

            if (exists) {
                result = TEMPLATE_ACCESS.pool.get(unit, i, &pool);
                SW_STATE_ACCESS_ERROR_CHECK(result);

                if (pool.refCount) {
                    LOG_WARN(BSL_LS_SOC_COMMON,
                             (BSL_META("unit %d: %d (%s): unexpectedly still"
                              " in use (%d) - invalid condition???\n"),
                              unit,
                              i,
                              (pool.name.name),
                              pool.refCount));
                }
                result = _shr_template_managements_mgrs[pool.templateManagerType].destroy(unit, i);
                if (_SHR_E_NONE != result) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META("unit %d: pool %d (%s): unable to destroy:"
                               " %d (%s)\n"),
                               unit,
                               i,
                               (pool.name.name),
                               result,
                               _SHR_ERRMSG(result)));
                } /* if (_SHR_E_NONE != result) */

                result = TEMPLATE_ACCESS.pool.free(unit, i);
                SW_STATE_ACCESS_ERROR_CHECK(result);

            } /* if (unitData->pool[i]) */
        } /* for (all pools as long as no errors) */

        /* Free the pool resources array. */
        result = TEMPLATE_ACCESS.pool.ptr_free(unit);
        SW_STATE_ACCESS_ERROR_CHECK(result);
    }
    return result;
}

static int 
shr_mtemplate_dump(int unit, int template_type)
{
    _shr_template_pool_desc_t thisPool;
    _shr_template_type_desc_t thisTemplate;
    int index, template, start_index, last_index, buff_counter;
    int result = BCM_E_NONE;
    char buff[SHR_TEMPLATE_MAX_BUFFER_SIZE_FOR_PRINT];
    uint32 ref_count;
    uint16 templateTypeCount, templatePoolCount;
    uint8 exists;
    int template_id, pool_idx;
    
    /* a little parameter checking */

    result = TEMPLATE_ACCESS.templateTypeCount.get(unit, &templateTypeCount);
    SW_STATE_ACCESS_ERROR_CHECK(result); 

    result = TEMPLATE_ACCESS.templatePoolCount.get(unit, &templatePoolCount);
    SW_STATE_ACCESS_ERROR_CHECK(result); 

    if( template_type != -1)
    {
        if (template_type < 0 || template_type > templateTypeCount)
        {
            LOG_CLI((BSL_META("  templateType not exist (%d..%d)\n"), 0, templateTypeCount));
            return BCM_E_PARAM;
        }
        start_index = template_type;
        last_index = template_type + 1;
    }
    else
    {
        /* dump information about the handle */
        LOG_CLI((BSL_META("  template manager (%d)\n"), unit));
        LOG_CLI((BSL_META("  template type count = %d\n"), templateTypeCount));
        LOG_CLI((BSL_META("  template pool count = %d\n"), templatePoolCount));
        start_index = 0;
        last_index = templateTypeCount;
    }
    
    for (index = start_index; index < last_index; index++) {
        result = TEMPLATE_ACCESS.template_param.is_allocated(unit, index, &exists);
        SW_STATE_ACCESS_ERROR_CHECK(result); 

        if (exists) {
            result = TEMPLATE_ACCESS.template_param.get(unit, index, &thisTemplate);
            SW_STATE_ACCESS_ERROR_CHECK(result); 

            result = TEMPLATE_ACCESS.pool.get(unit, thisTemplate.templatePoolId, &thisPool);
            SW_STATE_ACCESS_ERROR_CHECK(result); 

            bcm_dpp_am_pool_id_to_template_id_get(unit, index, &template_id, &pool_idx);

            LOG_CLI((BSL_META("  template type %d (%s) in core #%d\n"), index, thisTemplate.name.name, pool_idx));
            LOG_CLI((BSL_META("  first template index = %d nof templates     = %d\n"), thisPool.template_low_id, thisPool.template_count));
            LOG_CLI((BSL_META("  number of elements using this template type = %d\n"), thisTemplate.refCount));
            LOG_CLI((BSL_META("  template data size %d \n"), (int)(thisPool.data_size)));

            if(template_type != -1) {
                for (template = thisPool.template_low_id; template < thisPool.template_low_id + thisPool.template_count; template++) {
                    result = _shr_template_managements_mgrs[thisPool.templateManagerType].ref_count_get(unit, thisTemplate.templatePoolId, template, &ref_count);
                    if (BCM_E_NONE != result) {
                        LOG_CLI((BSL_META("  error retriving ref counter \n")));
                        return BCM_E_FAIL;
                    }

                    if (ref_count > 0 && SHR_TEMPLATE_MAX_BUFFER_SIZE_FOR_PRINT >= thisPool.data_size) {
                        result = _shr_template_managements_mgrs[thisPool.templateManagerType].data_get(unit, thisTemplate.templatePoolId, template, buff);
                        if (BCM_E_NONE != result) {
                            LOG_CLI((BSL_META("  error geting data for template ID %d\n"), template));
                        } else{
                            LOG_CLI((BSL_META("  template id = %d ref counter = %d \n"), template, ref_count));
                            if (_bcm_dpp_am_template_print_func[template_id] == NULL ) {
                                LOG_CLI((BSL_META("  template data (hex): ")));
                                for (buff_counter = 0; buff_counter < thisPool.data_size; buff_counter++) {
                                    /* %02hhx means print an unsigned char with at least two numbers in hexa. */
                                    LOG_CLI((BSL_META("%02hhx"), buff[buff_counter]));
                                }
                                LOG_CLI((BSL_META("\n\n")));
                            } else {
                                (_bcm_dpp_am_template_print_func[template_id]) (unit, buff);
                            }
                        }
                    }
                }
            }
        } else {
            LOG_CLI((BSL_META("   template type %d is not in use\n"), index));
        }
        LOG_CLI((BSL_META("\n")));
    }
    return result;
}

static int 
shr_mtemplate_clear(int unit, 
                    int template_type)
{
    shr_template_manage_t mngr_type;
    int thisPool;
    int result = BCM_E_NONE;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d) enter\n"),
               unit,
               template_type));
               
    /* a little parameter checking */
    TEMPLATE_TYPE_VALID_CHECK(unit, template_type);
    TEMPLATE_TYPE_EXIST_CHECK(unit, template_type);
    
    /* get the pool information */
    result = TEMPLATE_ACCESS.template_param.templatePoolId.get(unit, template_type, &thisPool);
    SW_STATE_ACCESS_ERROR_CHECK(result); 

    result = TEMPLATE_ACCESS.pool.templateManagerType.get(unit, thisPool, &mngr_type);
    SW_STATE_ACCESS_ERROR_CHECK(result); 

    /* make the call */
    result = _shr_template_managements_mgrs[mngr_type].clear(unit, thisPool);

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%d, %d) return %d (%s)\n"),
               unit,
               template_type,
               result,
               _SHR_ERRMSG(result)));

    return result;
}
/* ******************************************************************/

/* Hash Utils functions } */

static int _shr_template_hash_create(int unit, 
                                     int pool_id,
                                     int template_low_id,
                                     int template_count,
                                     int max_entities,
                                     uint32 global_max,
                                     size_t data_size,
                                     const void *extras,
                                     const char *name)
{
    /* need the base descriptor */
    int result = BCM_E_NONE;
    const shr_template_manage_hash_compare_extras_t *info = extras;
    uint32 soc_sand_rv = 0;
    SOC_SAND_MULTI_SET_PTR multi_set_info;
    SOC_SAND_MULTI_SET_INIT_INFO multi_set_init_info;

    /* Set the pool's extras. */
    result = TEMPLATE_ACCESS.pool.extras.set(unit, pool_id, *info);
    SW_STATE_ACCESS_ERROR_CHECK(result); 

    /* Clear structures */
    
    /* Init hash template */
    soc_sand_os_memset(&multi_set_init_info, 0x0, sizeof(SOC_SAND_MULTI_SET_INIT_INFO));
    multi_set_init_info.get_entry_fun = NULL;
    multi_set_init_info.set_entry_fun = NULL;
    multi_set_init_info.max_duplications = max_entities;
    multi_set_init_info.member_size = data_size;
    multi_set_init_info.nof_members = template_count;
    multi_set_init_info.sec_handle = 0;
    multi_set_init_info.prime_handle = 0;

    soc_sand_rv = soc_sand_multi_set_create(
        unit,
        &multi_set_info,
        multi_set_init_info
    );

    result = TEMPLATE_ACCESS.pool.templateHandle.set(unit, pool_id, multi_set_info);
    SW_STATE_ACCESS_ERROR_CHECK(result); 

    if (SOC_SAND_FAILURE(soc_sand_rv)) {
        /* Init failed */
        return BCM_E_MEMORY;
    }
    
    /* all's well if we got here */
    return result;
}

static int _shr_template_hash_destroy(int unit, int pool_id)
{
    int res = BCM_E_NONE;
    uint32 soc_sand_rv;
    SOC_SAND_MULTI_SET_PTR hash_handle;

    res = TEMPLATE_ACCESS.pool.templateHandle.get(unit, pool_id, &hash_handle);
    SW_STATE_ACCESS_ERROR_CHECK(res); 

    soc_sand_rv = soc_sand_multi_set_destroy(unit, hash_handle);
    SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

    return res;
}

static int _shr_template_hash_alloc(int unit, 
                                    int pool_id,
                                     uint32 flags,
                                     const void* data,
                                     int *is_allocated,
                                     int *template)
{
    _shr_template_pool_desc_t desc;
    int res = BCM_E_NONE;   
    SOC_SAND_MULTI_SET_KEY * val;
    SOC_SAND_MULTI_SET_PTR hash_handle;
    uint8 add_success;
    shr_template_manage_hash_compare_extras_t extra;
    uint32 soc_sand_rv = SOC_SAND_OK;
    uint8 first_appear;
    int template_alloc = 0;
    shr_template_to_stream_t *hcts_cb = NULL;

    res = TEMPLATE_ACCESS.pool.get(unit, pool_id, &desc);
    SW_STATE_ACCESS_ERROR_CHECK(res); 

    hash_handle = desc.templateHandle;
    extra = desc.extras;
    
    val = sal_alloc((desc.data_size),"Data buffer");

    if (!val) {
        /* alloc failed */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to allocate %u bytes for info\n"),
                   (unsigned int)
                   ((desc.data_size))));
        res = BCM_E_MEMORY;
    } else {
        sal_memset(val,0x0,(desc.data_size));

        /* move to buffer */
        hcts_cb = hash_compare_to_stream_cb[unit];
        res = hcts_cb[extra](data,val,desc.data_size);
        if (res != BCM_E_NONE) {
            goto exit;
        }

        if (flags & SHR_TEMPLATE_MANAGE_SET_WITH_ID) {
            template_alloc = *template - desc.template_low_id;
            soc_sand_rv = soc_sand_multi_set_member_add_at_index(
                        unit,
                        hash_handle,
                        val,
                        template_alloc,
                        &first_appear,
                        &add_success);            
        } else {
           /* Add new data */
            soc_sand_rv = soc_sand_multi_set_member_add(
                        unit,
                        hash_handle,
                        val,
                        (uint32 *)&template_alloc,
                        &first_appear,                        
                        &add_success);
            *template = template_alloc + desc.template_low_id;
        }
   
        *is_allocated = first_appear;

        if (SOC_SAND_FAILURE(soc_sand_rv)) {
            res = BCM_E_INTERNAL;
            goto exit;
        }

        if (!add_success) {
            res = BCM_E_MEMORY;
            goto exit;
        }
        
    }

exit:
    if (val) {
        sal_free(val);
    }
    
    return res;
}

static int _shr_template_hash_alloc_group(int unit, 
                                          int pool_id,
                                     uint32 flags,
                                     const void* data,
                                     int nof_additions,
                                     int *is_allocated,
                                     int *template)
{
    _shr_template_pool_desc_t desc;
    int res = BCM_E_NONE;   
    SOC_SAND_MULTI_SET_KEY * val;
    SOC_SAND_MULTI_SET_PTR hash_handle;
    uint8 add_success;
    shr_template_manage_hash_compare_extras_t extra;
    uint32 soc_sand_rv = SOC_SAND_OK;
    uint8 first_appear;
    int template_alloc = 0;
    shr_template_to_stream_t *hcts_cb = NULL;

    res = TEMPLATE_ACCESS.pool.get(unit, pool_id, &desc);
    SW_STATE_ACCESS_ERROR_CHECK(res); 

    hash_handle = desc.templateHandle;
    extra = desc.extras;
    
    val = sal_alloc((desc.data_size),"Data buffer");

    if (!val) {
        /* alloc failed */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to allocate %u bytes for info\n"),
                   (unsigned int)
                   ((desc.data_size))));
        res = BCM_E_MEMORY;
    } else {
        sal_memset(val,0x0,(desc.data_size));

        /* move to buffer */
        hcts_cb = hash_compare_to_stream_cb[unit];
        res = hcts_cb[extra](data,val,desc.data_size);
        if (res != BCM_E_NONE) {
            goto exit;
        }

        if (flags & SHR_TEMPLATE_MANAGE_SET_WITH_ID) {
            template_alloc = *template - desc.template_low_id;
            soc_sand_rv = soc_sand_multi_set_member_add_at_index_nof_additions(
                        unit,
                        hash_handle,
                        val,
                        template_alloc,
                        nof_additions,
                        &first_appear,
                        &add_success);            
        } else {
             
             res = BCM_E_INTERNAL;
             goto exit;
        }
   
        *is_allocated = first_appear;

        if (SOC_SAND_FAILURE(soc_sand_rv)) {
            res = BCM_E_INTERNAL;
            goto exit;
        }

        if (!add_success) {
            res = BCM_E_MEMORY;
            goto exit;
        }
        
    }

exit:
    if (val) {
        sal_free(val);
    }
    
    return res;
}

static int _shr_template_hash_free(int unit, 
                                   int pool_id,
                                    int template,
                                    int *is_last)
{
    _shr_template_pool_desc_t desc;
    int res = BCM_E_NONE;   
    SOC_SAND_MULTI_SET_PTR hash_handle;
    uint8 last_appear = 0;
    uint32 soc_sand_rv;
    int template_alloc;

    res = TEMPLATE_ACCESS.pool.get(unit, pool_id, &desc);
    SW_STATE_ACCESS_ERROR_CHECK(res); 

    hash_handle = desc.templateHandle;
    
    /* Remove old template */
    template_alloc = template - desc.template_low_id;
    soc_sand_rv = soc_sand_multi_set_member_remove_by_index(
              unit,
              hash_handle,
              template_alloc,
              &last_appear);
    
    if (SOC_SAND_FAILURE(soc_sand_rv)) {
        res = BCM_E_INTERNAL;
    } else {
        if (last_appear) {
            *is_last = 1;
        } else {
            *is_last = 0;
        }
    }

    return res;
}

static int _shr_template_hash_free_group(int unit, 
                                         int pool_id,
                                        int template,
                                        int nof_deductions,
                                        int *is_last)
{
    _shr_template_pool_desc_t desc;
    int res = BCM_E_NONE;   
    SOC_SAND_MULTI_SET_PTR hash_handle;
    uint8 last_appear = 0;
    uint32 soc_sand_rv;
    int template_alloc;

    res = TEMPLATE_ACCESS.pool.get(unit, pool_id, &desc);
    SW_STATE_ACCESS_ERROR_CHECK(res); 

    hash_handle = desc.templateHandle;
    
    /* Remove old template */
    template_alloc = template - desc.template_low_id;
    soc_sand_rv = soc_sand_multi_set_member_remove_by_index_multiple(
              unit,
              hash_handle,
              template_alloc,
              nof_deductions,
              &last_appear);
    
    if (SOC_SAND_FAILURE(soc_sand_rv)) {
        res = BCM_E_INTERNAL;
    } else {
        if (last_appear) {
            *is_last = 1;
        } else {
            *is_last = 0;
        }
    }

    return res;
}

static int _shr_template_hash_exchange(int unit, 
                                       int pool_id,
                                        uint32 flags,
                                        const void *data,
                                        int old_template,
                                        int *is_last,
                                        int *template,
                                        int *is_allocated)
{
    _shr_template_pool_desc_t desc;
    int res = BCM_E_NONE;   
    SOC_SAND_MULTI_SET_KEY * val;
    SOC_SAND_MULTI_SET_KEY * old_val;
    SOC_SAND_MULTI_SET_PTR hash_handle;
    uint8 add_success;
    uint8 first_appear = 0;
    uint8 last_appear = 0;
    int template_alloc = 0;
    int old_template_alloc;

    shr_template_manage_hash_compare_extras_t extra;
    uint32 soc_sand_rv;
    uint32 ref_count_old;
    shr_template_to_stream_t *hcts_cb = NULL;

    res = TEMPLATE_ACCESS.pool.get(unit, pool_id, &desc);
    SW_STATE_ACCESS_ERROR_CHECK(res); 

    hash_handle = desc.templateHandle;
    extra = desc.extras;
    
    old_template_alloc = old_template - desc.template_low_id;
    
    val = sal_alloc((desc.data_size),"Data buffer");
    old_val = sal_alloc((desc.data_size),"Data buffer old");
    if (!val || !old_val) {
        /* alloc failed */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to allocate %u bytes for info\n"),
                   (unsigned int)
                   ((desc.data_size))));
        res = BCM_E_MEMORY;
        goto exit;
    } else {
        sal_memset(val,0x0,(desc.data_size));

        /* move to buffer */
        hcts_cb = hash_compare_to_stream_cb[unit];
        res = hcts_cb[extra](data,val,(desc.data_size));
        if (res != BCM_E_NONE) {
            goto exit;
        }

        /* Save old data in case of failure */
        soc_sand_rv = soc_sand_multi_set_get_by_index(
                      unit,
                      hash_handle,
                      old_template_alloc,
                      old_val,
                      &ref_count_old);
        if (SOC_SAND_FAILURE(soc_sand_rv)) {
            /* Get old data failed */
            res = BCM_E_INTERNAL;
            goto exit;
        }

        if (ref_count_old == 0 && !(flags & SHR_TEMPLATE_MANAGE_IGNORE_NOT_EXIST_OLD_TEMPLATE)) {
            /* User gave old template that was empty */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("Old template(%d) was already empty\n"), old_template));
            res = BCM_E_PARAM;
            goto exit;
        }
                      
        if (ref_count_old != 0) {
            /* Remove old template */
            soc_sand_rv = soc_sand_multi_set_member_remove_by_index(
                          unit,
                          hash_handle,
                          old_template_alloc,
                          &last_appear);
                          
            *is_last = last_appear;
            if (SOC_SAND_FAILURE(soc_sand_rv)) {
                /* Remove failed */
                res = BCM_E_INTERNAL;
                goto exit;
            }
        }
        
        /* WITH_ID means that the user would like to exchange the current template for a specific new template */
        /* (as opposed to changing the template to any new template containing the supplied data). */
        /* If IGNORE_DATA is not specified, then the user would also like to change the value of the new template. */
        if (flags & SHR_TEMPLATE_MANAGE_SET_WITH_ID) {
            uint32 new_template = (uint32) *template - desc.template_low_id;
            uint32 old_data_ref_count;

            if (!(flags & SHR_TEMPLATE_MANAGE_IGNORE_DATA)) {
                uint32 new_data_ref_count;
                uint32 old_template_of_new_data;

                /* Make sure that the new data does not exist already for another index. */
                /* This check is actually made inside the insert by index as well. */
                /* However no distinguishable error is returned for it. */
                soc_sand_rv = soc_sand_multi_set_member_lookup(
                              unit,
                              hash_handle,
                              val,
                              &old_template_of_new_data,
                              &new_data_ref_count);
                if (SOC_SAND_FAILURE(soc_sand_rv)) {
                    /* Get data failed */
                    res = BCM_E_INTERNAL;
                    goto exit;
                }

                if ((old_template_of_new_data != new_template) && (new_data_ref_count > 0)) {
                    res = BCM_E_EXISTS;
                    goto exit;
                }

            }

            /* Get the current data at index (and the ref count to it) and put it into the buffer (val). */
            soc_sand_rv = soc_sand_multi_set_get_by_index(
                          unit,
                          hash_handle,
                          new_template,
                          val,
                          &old_data_ref_count);
            if (SOC_SAND_FAILURE(soc_sand_rv)) {
                /* Get data failed */
                res = BCM_E_INTERNAL;
                goto exit;
            }

            if ((flags & SHR_TEMPLATE_MANAGE_IGNORE_DATA) && (old_data_ref_count == 0)) {
                /* WITH_ID and IGNORE_DATA are invalid if the template does not exist - because we don't know where to get */
                /* the new value from. */
                res = BCM_E_NOT_FOUND;
                goto exit;
            }
            
            /* If we ignore the data or we have a new template, then we need not change the existing template value. */
            if ((flags & SHR_TEMPLATE_MANAGE_IGNORE_DATA) || (old_data_ref_count == 0)) {
                /* Add data by index. */
                soc_sand_rv = soc_sand_multi_set_member_add_at_index(
                        unit,
                        hash_handle,
                        val,
                        new_template,
                        &first_appear,
                        &add_success
                );  
            } else /* We need to change the current template value. */ {
                uint8 last_appear;

                /* Free all members using the old data. */
                soc_sand_rv = soc_sand_multi_set_member_remove_by_index_multiple(
                              unit,
                              hash_handle,
                              new_template,
                              old_data_ref_count,
                              &last_appear);
                if (SOC_SAND_FAILURE(soc_sand_rv)) {
                    /* Get data failed */
                    res = BCM_E_INTERNAL;
                    goto exit;
                }

                /* VERIFY(data_template == new_template); */
                /* VERIFY(last_appear); */

                /* Move the new data to the buffer. */
                hcts_cb = hash_compare_to_stream_cb[unit];
                res = hcts_cb[extra](data,val,(desc.data_size));
                if (res != BCM_E_NONE) {
                    goto exit;
                }

                /* Add ref_count + 1 members using the new data at the required index. */
                soc_sand_rv = soc_sand_multi_set_member_add_at_index_nof_additions(
                          unit,
                          hash_handle,
                          val,
                          new_template,
                          old_data_ref_count + 1,
                          &first_appear,
                          &add_success
                );  

            }
        } else /* if (!WITH_ID) */ {
            /* Add new data. */
            soc_sand_rv = soc_sand_multi_set_member_add(
                    unit,
                    hash_handle,
                    val,
                    (uint32 *)&template_alloc,
                    &first_appear,
                    &add_success
            );

            *template = template_alloc + desc.template_low_id;
        }

        *is_allocated = first_appear;
   
        if (SOC_SAND_FAILURE(soc_sand_rv) || !add_success) {
            /* Add new failed, return old data */
            if (!add_success) {
                res = BCM_E_MEMORY;
            } else {
                /* General error */
                res = BCM_E_INTERNAL;
            }
            
            if (ref_count_old != 0) {
            
                soc_sand_rv = soc_sand_multi_set_member_add_at_index(
                     unit,
                     hash_handle,
                     old_val,
                     old_template_alloc,                 
                     &first_appear,
                     &add_success);
                
                if (SOC_SAND_FAILURE(soc_sand_rv) || !add_success) {
                    /* Internal error */
                    res = BCM_E_INTERNAL;
                }
            }
            
            goto exit;
        }

    }

exit:
    if (val) {
        sal_free(val);
    }
    if (old_val) {
        sal_free(old_val);
    }
    

    return res;
}

static int _shr_template_hash_exchange_test(int unit, 
                                            int pool_id,
                                            uint32 flags,
                                            const void *data,
                                            int old_template,
                                            int *is_last,
                                            int *template,
                                            int *is_allocated)
{
    _shr_template_pool_desc_t desc;
    int res = BCM_E_NONE;   
    SOC_SAND_MULTI_SET_KEY * val;
    SOC_SAND_MULTI_SET_KEY * old_val;
    SOC_SAND_MULTI_SET_PTR hash_handle;
    uint8 add_success;
    uint8 first_appear = 0;
    uint8 last_appear = 0;
    int old_template_alloc = 0;
    int new_template_alloc = 0;

    shr_template_manage_hash_compare_extras_t extra;
    uint32 soc_sand_rv;
    uint32 ref_count_old;
    shr_template_to_stream_t *hcts_cb = NULL;

    res = TEMPLATE_ACCESS.pool.get(unit, pool_id, &desc);
    SW_STATE_ACCESS_ERROR_CHECK(res); 

    hash_handle = desc.templateHandle;
    extra = desc.extras;

    old_template_alloc = old_template - desc.template_low_id;
    
    val = sal_alloc((desc.data_size),"Data buffer");
    old_val = sal_alloc((desc.data_size),"Data buffer old");
    if (!val || !old_val) {
        /* alloc failed */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to allocate %u bytes for info\n"),
                   (unsigned int)
                   ((desc.data_size))));
        res = BCM_E_MEMORY;
        goto exit;
    } else {
        sal_memset(val,0x0,(desc.data_size));
             
        /* move to buffer */
        hcts_cb = hash_compare_to_stream_cb[unit];
        res = hcts_cb[extra](data,val,(desc.data_size));
        if (res != BCM_E_NONE) {
            goto exit;
        }

        /* Save old data in case of failure */
        soc_sand_rv = soc_sand_multi_set_get_by_index(
                      unit,
                      hash_handle,
                      old_template_alloc,
                      old_val,
                      &ref_count_old);
        if (SOC_SAND_FAILURE(soc_sand_rv)) {
            /* Get old data failed */
            res = BCM_E_INTERNAL;
            goto exit;
        }

        if (ref_count_old == 0 && !(flags & SHR_TEMPLATE_MANAGE_IGNORE_NOT_EXIST_OLD_TEMPLATE)) {
            /* User gave old template that was empty */
            res = BCM_E_PARAM;
            goto exit;
        }
                      
        if (ref_count_old != 0) {    
            soc_sand_rv = soc_sand_multi_set_member_remove_by_index(
                          unit,
                          hash_handle,
                          old_template_alloc,
                          &last_appear);

            *is_last = last_appear;
            
            if (SOC_SAND_FAILURE(soc_sand_rv)) {
                res = BCM_E_INTERNAL;
                goto exit;
            }
        }

        /* Add new data */
        soc_sand_rv = soc_sand_multi_set_member_add(
                unit,
                hash_handle,
                val,
                (uint32 *)&new_template_alloc,
                &first_appear,
                &add_success
        );
        *template = new_template_alloc + desc.template_low_id;

        *is_allocated = first_appear;
   
        if (SOC_SAND_FAILURE(soc_sand_rv) || !add_success) {
            if (!add_success) {
                res = BCM_E_MEMORY;
            } else {
                /* General error */
                res = BCM_E_INTERNAL;
            }
        }
        else {
            /* Remove new data */
            soc_sand_rv = soc_sand_multi_set_member_remove_by_index(
                          unit,
                          hash_handle,
                          new_template_alloc,
                          &last_appear);
            if (SOC_SAND_FAILURE(soc_sand_rv)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("Failed to release new allocated data.\n")));
                res = BCM_E_INTERNAL;
                goto exit;                   
            }        
        }

        if (ref_count_old != 0) {
            soc_sand_rv = soc_sand_multi_set_member_add_at_index(
                 unit,
                 hash_handle,
                 old_val,
                 old_template_alloc,
                 &first_appear,
                 &add_success);
             
            if (SOC_SAND_FAILURE(soc_sand_rv)) {
                res = BCM_E_INTERNAL;
                goto exit;                   
            }        
        }
    }

exit:
    if (val) {
        sal_free(val);
    }
    if (old_val) {
        sal_free(old_val);
    }
    

    return res;
}

static int _shr_template_hash_data_get(int unit, 
                                       int pool_id,
                                        int template,
                                        void *data)
{
    _shr_template_pool_desc_t desc;
    int res = BCM_E_NONE;   
    SOC_SAND_MULTI_SET_KEY * val;
    SOC_SAND_MULTI_SET_PTR hash_handle;
    uint32 ref_count;
    shr_template_manage_hash_compare_extras_t extra;
    uint32 soc_sand_rv;
    int template_alloc;
    shr_template_from_stream_t *hcfs_cb = NULL;

    res = TEMPLATE_ACCESS.pool.get(unit, pool_id, &desc);
    SW_STATE_ACCESS_ERROR_CHECK(res); 

    hash_handle = desc.templateHandle;
    extra = desc.extras;
        
    template_alloc = template - desc.template_low_id;

    val = sal_alloc((desc.data_size),"Data buffer");

    if (!val) {
        /* alloc failed */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to allocate %u bytes for info\n"),
                   (unsigned int)
                   ((desc.data_size))));
        res = BCM_E_MEMORY;
    } else {
        sal_memset(val,0x0,(desc.data_size));

        soc_sand_rv = soc_sand_multi_set_get_by_index(
                    unit,
                    hash_handle,
                    template_alloc,
                    val,
                    &ref_count);

   
        if (!SOC_SAND_FAILURE(soc_sand_rv) && ref_count != 0) {
            hcfs_cb = hash_compare_from_stream_cb[unit];
            res = (hcfs_cb[extra](data,val,(desc.data_size)));
            if (res != BCM_E_NONE) {
                /* Problem with from stream */
                res =  BCM_E_INTERNAL;
            }
        } else {
            /* No data */
            res = BCM_E_NOT_FOUND;
        }
    }

    if (val) {
        sal_free(val);
    }
    return res;
}

static int _shr_template_hash_index_get(int unit, 
                                        int pool_id,
                                        const void *data,
                                        int* template)
{
    _shr_template_pool_desc_t desc;
    int res = BCM_E_NONE;
    SOC_SAND_MULTI_SET_KEY * val;
    SOC_SAND_MULTI_SET_PTR hash_handle;
    uint32 ref_count;
    shr_template_manage_hash_compare_extras_t extra;
    uint32 soc_sand_rv;
    uint32 template_alloc;
    shr_template_to_stream_t *hcts_cb = NULL;

    res = TEMPLATE_ACCESS.pool.get(unit, pool_id, &desc);
    SW_STATE_ACCESS_ERROR_CHECK(res); 

    hash_handle = desc.templateHandle;
    extra = desc.extras;

    val = sal_alloc((desc.data_size),"Data buffer");

    if (!val) {
        /* alloc failed */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to allocate %u bytes for info\n"),
                   (unsigned int)
                   ((desc.data_size))));
        res = BCM_E_MEMORY;
    } else {
        sal_memset(val,0x0,(desc.data_size));

        /* move to buffer */
        hcts_cb = hash_compare_to_stream_cb[unit];
        res = hcts_cb[extra](data,val,(desc.data_size));
        if (res != BCM_E_NONE) {
            goto exit;
        }

        /* Make sure that the new data does not exist already for another index. */
        /* This check is actually made inside the insert by index as well. */
        /* However no distinguishable error is returned for it. */
        soc_sand_rv = soc_sand_multi_set_member_lookup(
                      unit,
                      hash_handle,
                      val,
                      &template_alloc,
                      &ref_count);
        if (SOC_SAND_FAILURE(soc_sand_rv)) {
            /* Get data failed */
            res = BCM_E_INTERNAL;
            goto exit;
        }

        if (ref_count == 0) {
            res = BCM_E_NOT_FOUND;
        } else {
            *template = template_alloc + desc.template_low_id;
        }
    }

exit:
    if (val) {
        sal_free(val);
    }
    return res;
}


static int _shr_template_hash_ref_count_get(int unit, 
                                            int pool_id,
                                            int template,
                                            uint32 *ref_count)
{
    _shr_template_pool_desc_t desc;
    int res = BCM_E_NONE;   
    SOC_SAND_MULTI_SET_KEY * val;
    SOC_SAND_MULTI_SET_PTR hash_handle;        
    uint32 soc_sand_rv;
    uint32 tmp_ref_count = 0;
    int template_alloc;

    res = TEMPLATE_ACCESS.pool.get(unit, pool_id, &desc);
    SW_STATE_ACCESS_ERROR_CHECK(res); 

    hash_handle = desc.templateHandle;

    template_alloc = template - desc.template_low_id;
        
    val = sal_alloc((desc.data_size),"Data buffer");

    if (!val) {
        /* alloc failed */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to allocate %u bytes for info\n"),
                   (unsigned int)
                   ((desc.data_size))));
        res = BCM_E_MEMORY;
    } else {
        sal_memset(val,0x0,(desc.data_size));

        soc_sand_rv = soc_sand_multi_set_get_by_index(
                    unit,
                    hash_handle,
                    template_alloc,
                    val,
                    &tmp_ref_count);

   
        if (!SOC_SAND_FAILURE(soc_sand_rv)) {
            /* Update reference count */
            *ref_count = tmp_ref_count;
        } else {
            /* Got error */
            res = BCM_E_INTERNAL;
        }
    }

    if (val) {
        sal_free(val);
    }
    return res;
}

static int _shr_template_hash_clear(int unit, int pool_id)
{
    _shr_template_pool_desc_t desc;
    int res = BCM_E_NONE;   
    SOC_SAND_MULTI_SET_PTR hash_handle;
    uint32 soc_sand_rv;

    res = TEMPLATE_ACCESS.pool.get(unit, pool_id, &desc);
    SW_STATE_ACCESS_ERROR_CHECK(res); 

    hash_handle = desc.templateHandle;
    
    soc_sand_rv = soc_sand_multi_set_clear(unit, hash_handle);
    SOC_SAND_IF_ERR_RETURN(soc_sand_rv);    
   
    return res;
}
#endif /* BCM_PETRA_SUPPORT || BCM_DFE_SUPPORT */
