/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        alloc_mngr.c
 * Purpose:     Resource allocation manager for SOC_SAND chips.
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_COMMON

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

/*#include <soc/error.h>*/
#include <bcm_int/common/debug.h>
#include <soc/debug.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/module.h>

#include <shared/swstate/sw_state_resmgr.h>

#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr_shr.h>
#include <bcm_int/dpp/alloc_mngr_cosq.h>
#include <bcm_int/dpp/alloc_mngr_utils.h>
#include <bcm_int/dpp/alloc_mngr.h>

#include <shared/swstate/access/sw_state_access.h>


#define _DPP_AM_COSQ_QUEUE_INDEX_GET(pool_id) ((pool_id) - dpp_res_cosq_pool->dpp_res_pool_cosq_queue_unicast_cont)  

/* Returns a pool_id given a pool_index */
#define _DPP_AM_COSQ_QUEUE_POOL_GET(index) ((index) + dpp_res_cosq_type->dpp_res_type_cosq_queue_unicast_cont)  

#define _DPP_NO_REGION_REQUESTED -1

#define BCM_FMQ_RESERVED_QUEUES 4

static char * _dpp_res_cosq_desc[] = 
{
    "connector continuous",
    "connector non-continuous region type 1",
    "connector non-continuous region type 2",
    "se cl fq region type 1",
    "se cl fq region type 2",
    "se cl hr",
    "fq connector sync (region type 2)",
    "hr connector sync (region type 2)",
    "not valid"
};

static char * _dpp_res_cosq_queue_desc[] = 
{
    "queue unicast continuous",
    "queue unicast non-continuous",
    "queue multicast continuous",
    "queue multicast non-continuous",
    "queue isq continuous",
    "queue isq non-continuous",

    "not valid"
};

/* Macro to allocate bitmap */
#define _BCM_DPP_COSQ_ALLOC_BITMAP_REGION(unit, resource, num_regions, string) \
  { \
    alloc_size = SHR_BITALLOCSIZE(num_regions); \
    BCMDNX_ALLOC(resource, alloc_size, string); \
    if (NULL == resource) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("No memory for allocation " string))); \
    } \
    \
    sal_memset(resource, 0, alloc_size); \
  }

/* Macro to convert 1k region to interdigitated region */
#define _BCM_DPP_AM_COSQ_QUAD_REGION_GET(region) (region / SOC_DPP_DEFS_GET(unit, nof_pools_per_interdigitated_region))


typedef enum _bcm_dpp_am_cosq_allocate_func_state_e
{
    BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_NONE,
    BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_ALLOCATE,
    BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_DEALLOCATE,
    BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_RESOURCE_CHECK

}_bcm_dpp_am_cosq_allocate_func_state_t;

typedef enum bcm_dpp_am_cosq_region_type_e
{
    BCM_DPP_AM_COSQ_REGION_TYPE_0 = DPP_DEVICE_COSQ_REGION_TYPE0,
    BCM_DPP_AM_COSQ_REGION_TYPE_1 = DPP_DEVICE_COSQ_REGION_TYPE1,
    BCM_DPP_AM_COSQ_REGION_TYPE_2 = DPP_DEVICE_COSQ_REGION_TYPE2,
    BCM_DPP_AM_COSQ_REGION_TYPE_2HR,
    BCM_DPP_AM_COSQ_REGION_TYPE_LAST
} bcm_dpp_am_cosq_region_type_t;

typedef enum bcm_dpp_am_cosq_alloc_scheme_e
{
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_0,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CONNECTOR_REGION_0,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_1,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CONNECTOR_REGION_1,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_2,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_FQ_REGION_2,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_1,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_1,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2HR,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_CL_REGION_2HR,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_ENHANCED_CL_REGION_2HR,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_2HR,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2FQ,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_CL_REGION_2FQ,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_ENHANCED_CL_REGION_2FQ,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_2FQ,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_1,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_CL_REGION_1,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_1,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_ENHANCED_CL_REGION_1,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2_DEALLOC,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_2_DEALLOC,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2_DEALLOC,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2_DEALLOC,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2HR_DEALLOC,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_CL_REGION_2HR_DEALLOC,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2FQ_DEALLOC,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_CL_REGION_2FQ_DEALLOC,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_ENHANCED_CL_REGION_2HR_DEALLOC,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_ENHANCED_CL_REGION_2FQ_DEALLOC,
    BCM_DPP_AM_COSQ_ALLOC_SCHEME_TYPE_LAST
}bcm_dpp_am_cosq_alloc_scheme_t;

typedef struct _bcm_dpp_am_cosq_align_alloc_args_s
{
    int align;
    int offset;
    int (*calc_count)(int nof_elements);
    int(*flow_id_to_bitmap_elem)(int,int pool_base);
    int(*bitmap_elem_to_flow_id)(int,int pool_base);
} _bcm_dpp_am_cosq_align_alloc_args_t;

typedef struct _bcm_dpp_am_cosq_allocation_info_s
{
    int                                     pool_type /*Cont, cl_hr, sync, etc...*/; 
    int                                     pool_offset /*The offset corresponds to either connector, se or sync*/;
    int                                     is_mandatory;
    int                                     is_last; /*marks last stage in an alloc scheme*/
    int                                     is_tag; /*tagged allocation*/
    int                                     res_check_if_not_in_use_nxt_ptr;
    int                                     res_check_if_in_use_nxt_ptr;
    bcm_dpp_am_cosq_pool_ref_type_t         relevant_pool_indices_type;
    _bcm_dpp_am_cosq_align_alloc_args_t     alloc_args;
    _bcm_dpp_am_cosq_allocate_func_state_t  alloc_state; /*alloc, dealloc, res_check*/
}_bcm_dpp_am_cosq_allocation_info_t;

typedef struct _bcm_dpp_am_cosq_per_type_all_allocated_resource_find_s
{
    int flow_offset;
    int elem_offset;
    int (*flow_incr)(int);
    int elem_incr;
    int region_type;
    int pool_offset; /*indicating conn, se or sync*/
    _bcm_dpp_am_cosq_align_alloc_args_t alloc_args;
} _bcm_dpp_am_cosq_per_type_all_allocated_resource_find_t;

static _bcm_dpp_am_cosq_allocation_info_t _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[SOC_DPP_DEFS_MAX(NOF_CORES)][BCM_DPP_AM_COSQ_SCH_MAX_ALLOCATION_STAGES][BCM_DPP_AM_COSQ_SCH_NOF_ALLOCATION_SCHEMES];
static _bcm_dpp_am_cosq_allocation_info_t _bcm_dpp_am_cosq_per_type_flow_find_info_array[SOC_DPP_DEFS_MAX(NOF_CORES)][BCM_DPP_AM_COSQ_SCH_NOF_ALLOCATION_SCHEMES];

STATIC int _bcm_dpp_am_cosq_init_flow_alloc_info_db(int unit, int core);
STATIC int _bcm_dpp_am_cosq_init_flow_find_info_db(int unit, int core);

int 
_bcm_dpp_am_cosq_get_first_base_pool_id(int unit, int *curr_index){

    int rv, updated_index, last_resource_pool;
    uint8 core_offset;
    BCMDNX_INIT_FUNC_DEFS;

    *curr_index = DPP_AM_RES_COSQ_START(unit);
    /* If the warmboot pools weren't initialized yet, it means this funciton is called to get the resource id (and number of pools) for cosq.
       WB will return an error and the curr_index will remain unchanged.
       If the warmboot has been initialized, then the first cosq pool will be after the last lif pool. In that case, we want to point directly at
       the pool, since the mapping of {cosq_resource_id X core_id} -> cosq_pool isn't implemented yet. */
    if (SOC_DPP_CONFIG(unit)->am.nof_am_cosq_resource_ids) {

        last_resource_pool = *curr_index - 1;

        rv = bcm_dpp_am_resource_to_nof_pools(unit, last_resource_pool, &core_offset);
        BCMDNX_IF_ERR_EXIT(rv);


        /* Get the last pool index for the last resource before cosq. */
        rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, core_offset - 1, last_resource_pool, &updated_index);
        BCMDNX_IF_ERR_EXIT(rv);


        *curr_index = updated_index + 1;
    }

exit:
    BCMDNX_FUNC_RETURN;
}
/*find the pool numbers associated with the core, these pools are determined dynamiclly, so we need to find them by iterating over the cores*/
int
_bcm_dpp_resources_fill_pool_cosq(int unit, int core, _dpp_res_pool_cosq_t* dpp_res_cosq_pool)
{
    int curr_index;
    int core_i;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    curr_index = DPP_AM_RES_COSQ_START(unit);

    if (core < 0 || core >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        /*We do not support BCM_CORE_ALL in this function since it calculate the res types per core*/
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Core %d id out of range"),core));
    }
    for (core_i = 0 ; core_i < core + 1; core_i++) {
        dpp_res_cosq_pool->dpp_res_pool_cosq_connector_cont = curr_index; /* SHOULD BE FIRST */

        curr_index += BCM_DPP_AM_RES_POOL_COSQ_E2E_TOTAL_DYNAMIC_ARAD; /* NOT A VALID ID*/

        dpp_res_cosq_pool->dpp_res_pool_cosq_e2e_end = curr_index;

        dpp_res_cosq_pool->dpp_res_pool_cosq_queue_unicast_cont         = ++curr_index; /* VOQ QUEUE GROUPS RANGE */
        dpp_res_cosq_pool->dpp_res_pool_cosq_queue_unicast_non_cont     = ++curr_index;
        dpp_res_cosq_pool->dpp_res_pool_cosq_queue_multicast_cont       = ++curr_index; /* VOQ QUEUE GROUPS RANGE */
        dpp_res_cosq_pool->dpp_res_pool_cosq_queue_multicast_non_cont   = ++curr_index;
        dpp_res_cosq_pool->dpp_res_pool_cosq_queue_isq_cont             = ++curr_index; /* VOQ QUEUE GROUPS RANGE */
        dpp_res_cosq_pool->dpp_res_pool_cosq_queue_isq_non_cont         = ++curr_index;
        dpp_res_cosq_pool->dpp_res_pool_cosq_queue_dynamic_start        = ++curr_index; /* ORDER SHOULD NOT BE CHANGED */ 

        curr_index += BCM_DPP_AM_RES_POOL_COSQ_QUEUE_TOTAL_DYNAMIC_ARAD;

        dpp_res_cosq_pool->dpp_res_pool_cosq_queue_dynamic_end          = curr_index; /* SHOULD BE LAST */
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_resources_fill_type_cosq(int unit, int core, _dpp_res_type_cosq_t* dpp_res_cosq_type)
{
    int curr_index;
    int core_i;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    curr_index = DPP_AM_RES_COSQ_START(unit);

    if (core < 0 || core >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        /*We do not support BCM_CORE_ALL in this function since it calculate the res types per core*/
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Core %d id out of range"),core));
    }
    for (core_i = 0 ; core_i < core + 1; core_i++) {

        dpp_res_cosq_type->dpp_res_type_cosq_connector_cont = curr_index; /* SHOULD BE FIRST */

        dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_1       = ++curr_index;
        dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_2       = ++curr_index;
        dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_1                 = ++curr_index;
        dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_2                 = ++curr_index;
        dpp_res_cosq_type->dpp_res_type_cosq_se_cl_hr                               = ++curr_index;
        dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync        = ++curr_index;
        dpp_res_cosq_type->dpp_res_type_cosq_hr_connector_region_type_2_sync        = ++curr_index;

        curr_index += BCM_DPP_AM_RES_TYPE_COSQ_E2E_TOTAL_DYNAMIC_ARAD;

        dpp_res_cosq_type->dpp_res_type_cosq_e2e_end = curr_index; /* ORDER SHOULD NOT BE CHANGED */

        dpp_res_cosq_type->dpp_res_type_cosq_queue_unicast_cont       = ++curr_index; /* VOQ QUEUE GROUPS RANGE */
        dpp_res_cosq_type->dpp_res_type_cosq_queue_unicast_non_cont   = ++curr_index;
        dpp_res_cosq_type->dpp_res_type_cosq_queue_multicast_cont     = ++curr_index; /* VOQ QUEUE GROUPS RANGE */
        dpp_res_cosq_type->dpp_res_type_cosq_queue_multicast_non_cont = ++curr_index;
        dpp_res_cosq_type->dpp_res_type_cosq_queue_isq_cont           = ++curr_index; /* VOQ QUEUE GROUPS RANGE */
        dpp_res_cosq_type->dpp_res_type_cosq_queue_isq_non_cont       = ++curr_index;
        dpp_res_cosq_type->dpp_res_type_cosq_queue_dynamic_start      = ++curr_index; /* ORDER SHOULD NOT BE CHANGED */ 

        curr_index += BCM_DPP_AM_RES_TYPE_COSQ_QUEUE_TOTAL_DYNAMIC_ARAD;

        dpp_res_cosq_type->dpp_res_type_cosq_queue_dynamic_end        = curr_index; /* SHOULD BE LAST */
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * COSQ Module - Start
 */

STATIC int
_bcm_dpp_am_cosq_get_resource_region(int unit, int core, bcm_dpp_cosq_bmp_type_t type, int start_region, int end_region, int *region_config)
{
    int i;
    uint8 sw_state_bit;

    BCMDNX_INIT_FUNC_DEFS;
    if (core < 0 || core >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Core %d id out of range"),core));
    }
    for (i = start_region; i <= end_region; i++) {
        switch (type) {
            case queue_unicast_total_dynamic_res_bmp:{
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_total_dynamic_res.bit_get(unit, core, i, &sw_state_bit));
                break;
                }
            case queue_multicast_total_dynamic_res_bmp:{
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_total_dynamic_res.bit_get(unit, core, i, &sw_state_bit));
                break;
            }
            case queue_isq_total_dynamic_res_bmp:{
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_total_dynamic_res.bit_get(unit, core, i, &sw_state_bit));
                break;
            }
            default:{
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid type")));
            }

        }
        
        if (sw_state_bit) {
            /* Just indicate whether region is configured by setting to 1 */
            region_config[i] = 1;
        } else {
            region_config[i] = 0;
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_am_cosq_update_resource_region(int unit, int core, bcm_dpp_cosq_bmp_type_t type, int start_region, int end_region, int is_add, int idx) /*idx is used only on e2e_total_res_pool and queue_total_res_pool as these are BMP arrays*/
{
    int i;

    BCMDNX_INIT_FUNC_DEFS;
    if (core < 0 || core >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Core %d id out of range"),core));
    }
    if (is_add) {
                for (i = start_region; i <= end_region; i++) {
                    /* Mark region */
                    switch (type) {
                        case e2e_fixed_res_bmp: {
                            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_fixed_res.bit_set(unit, core, i));
                            break;
                        }
                        case queue_fixed_res_bmp: {
                            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_fixed_res.bit_set(unit, core, i));
                            break;
                        } 
                        case queue_unicast_total_dynamic_res_bmp:{
                            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_total_dynamic_res.bit_set(unit, core, i));
                            break;
                        }
                        case queue_unicast_free_dynamic_res_bmp:{
                            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_free_dynamic_res.bit_set(unit, core, i));
                            break;
                        }
                        case queue_multicast_total_dynamic_res_bmp:{
                            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_total_dynamic_res.bit_set(unit, core, i));
                            break;
                        }
                        case queue_multicast_free_dynamic_res_bmp:{
                            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_free_dynamic_res.bit_set(unit, core, i));
                            break;
                        }
                        case queue_isq_total_dynamic_res_bmp:{
                            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_total_dynamic_res.bit_set(unit, core, i));
                            break;
                        }
                        case queue_isq_free_dynamic_res_bmp:{
                            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_free_dynamic_res.bit_set(unit, core, i));
                            break;
                        }
                        case e2e_total_res_pool_type_bmp:{
                            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.res.bit_set(unit, core, idx, i));
                            break;
                        }
                        case queue_total_res_pool_type_bmp:{
                            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.res.bit_set(unit, core, idx, i));
                            break;
                        }
                    }
                }
            }
            else {
                for (i = start_region; i <= end_region; i++) {
                    /* Unmark region */
                    switch (type) {
                        case e2e_fixed_res_bmp: {
                            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_fixed_res.bit_clear(unit, core, i));
                            break;
                        }
                        case queue_fixed_res_bmp: {
                            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_fixed_res.bit_clear(unit, core, i));
                            break;
                        }
                        case queue_unicast_total_dynamic_res_bmp:{
                            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_total_dynamic_res.bit_clear(unit, core, i));
                            break;
                        }
                        case queue_unicast_free_dynamic_res_bmp:{
                            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_free_dynamic_res.bit_clear(unit, core, i));
                            break;
                        }
                        case queue_multicast_total_dynamic_res_bmp:{
                            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_total_dynamic_res.bit_clear(unit, core, i));
                            break;
                        }
                        case queue_multicast_free_dynamic_res_bmp:{
                            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_free_dynamic_res.bit_clear(unit, core, i));
                            break;
                        }
                        case queue_isq_total_dynamic_res_bmp:{
                            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_total_dynamic_res.bit_clear(unit, core, i));
                            break;
                        }
                        case queue_isq_free_dynamic_res_bmp:{
                            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_free_dynamic_res.bit_clear(unit, core, i));
                            break;
                        }
                        case e2e_total_res_pool_type_bmp:{
                            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.res.bit_clear(unit, core, idx, i));
                            break;
                        }
                        case queue_total_res_pool_type_bmp:{
                            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.res.bit_clear(unit, core, idx, i));
                            break;
                        }
                    }
                }
            }


exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_am_cosq_update_pool(int unit, int core, bcm_dpp_am_cosq_pool_ref_type_t type , int pool, int is_add)
{
    int i, index = -1;
    int pool_max_entries, pool_valid_entries, pool_entry;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(sw_state_bcm_dpp_am_cosq_pool_ref_t_get(unit, core, type, max_entries_field, -1, &pool_max_entries));
    if (is_add) {
        for (i = 0; i < pool_max_entries; i++) {
            BCMDNX_IF_ERR_EXIT(sw_state_bcm_dpp_am_cosq_pool_ref_t_get(unit, core, type, entries_field, i, &pool_entry));
            if ( (index == -1) && (pool_entry == -1) ) { /* first free entry */
                index = i;
                continue;
            }
            if (pool_entry == pool) { /* entry exists */
                break;
            }
        }

        if ( i >= pool_max_entries) {
            if (index != -1) {
                BCMDNX_IF_ERR_EXIT(sw_state_bcm_dpp_am_cosq_pool_ref_t_set(unit, core, type, entries_field, index, pool)); /* initialize entry */
                BCMDNX_IF_ERR_EXIT(sw_state_bcm_dpp_am_cosq_pool_ref_t_get(unit, core, type, valid_entries_field, -1, &pool_valid_entries)); /*valid_entries++*/
                BCMDNX_IF_ERR_EXIT(sw_state_bcm_dpp_am_cosq_pool_ref_t_set(unit, core, type, valid_entries_field, -1, pool_valid_entries + 1));
            }
            else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG_NO_UNIT("No more valid entries")));
            }
        }
    }
    else {
        for (i = 0; i < pool_max_entries; i++) {
            BCMDNX_IF_ERR_EXIT(sw_state_bcm_dpp_am_cosq_pool_ref_t_get(unit, core, type, entries_field, i, &pool_entry));
            if (pool_entry == pool) {
                BCMDNX_IF_ERR_EXIT(sw_state_bcm_dpp_am_cosq_pool_ref_t_set(unit, core, type, entries_field, i, -1)); /* free entry */
                BCMDNX_IF_ERR_EXIT(sw_state_bcm_dpp_am_cosq_pool_ref_t_get(unit, core, type, valid_entries_field, -1, &pool_valid_entries)); /*valid_entries--*/
                BCMDNX_IF_ERR_EXIT(sw_state_bcm_dpp_am_cosq_pool_ref_t_set(unit, core, type, valid_entries_field, -1, pool_valid_entries - 1));
                break;
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_am_cosq_update_pool_entry(
                   int unit,
                   int core,
                   int pool_index,
                   int is_valid,
                   int is_dynamic,
                   int start_region,
                   int end_region,
                   int desc_type,
                   _dpp_res_type_cosq_t *dpp_res_cosq_type)
{
    int rv = BCM_E_NONE;
    uint32 nof_remote_cores;
    int region_index;
    bcm_dpp_am_cosq_pool_info_type_t pool_info_type;
    int pool_id;

    BCMDNX_INIT_FUNC_DEFS;
    if (core < 0 || core >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Core %d id out of range"),core));
    }

    if ( (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_connector_cont) ||
     (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_1) ||
     (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_2) ||
     (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_1) ||
     (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_2) ||
     (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_se_cl_hr) || 
     (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync) ||
     (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_hr_connector_region_type_2_sync) ) {
        pool_info_type = e2e_total_res_pool_type;
    } else {
        pool_info_type = queue_total_res_pool_type;
    }

    switch (pool_info_type) {
        case e2e_total_res_pool_type:{
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.is_valid.set(unit, core, pool_index, is_valid));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.is_dynamic.set(unit, core, pool_index, is_dynamic));
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core, e2e_total_res_pool_type_bmp, start_region, end_region, TRUE, pool_index));
            break;
        }
        case queue_total_res_pool_type:{
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.is_valid.set(unit, core, pool_index, is_valid));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.is_dynamic.set(unit, core, pool_index, is_dynamic));
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core, queue_total_res_pool_type_bmp, start_region, end_region, TRUE, pool_index));
            break;
        }
        /* No need. It's a dead default.
        default:{
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Pool info type is invalid, type is %d"),pool_info_type));
        }*/
    } 

    if (is_dynamic == FALSE) {
        if ( (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_connector_cont) ||
             (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_1) ||
             (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_2) ||
             (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_1) ||
             (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_2) ||
             (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_se_cl_hr) ||
             (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync) ||
             (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_hr_connector_region_type_2_sync) ) {
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core, e2e_fixed_res_bmp, start_region, end_region, TRUE, -1));
        }
        else { 
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core, queue_fixed_res_bmp, start_region, end_region, TRUE, -1));
        }
    }
    if (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_connector_cont) {
        
        region_index = _BCM_DPP_AM_COSQ_GET_REGION_INDEX_FROM_POOL_INDEX(pool_index);
        nof_remote_cores = SOC_DPP_CONFIG(unit)->arad->region_nof_remote_cores[core][region_index];

        switch (pool_info_type) {
            case e2e_total_res_pool_type:{
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.start.set(unit, core, pool_index, start_region * 1024));
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.count.set(unit, core, pool_index, ((((end_region + 1) - start_region) * 1024) / 4) / (nof_remote_cores))); /*Connector ID is mapped to one qaud, and represnted by one element in the pool*/
                break;
            }
            case queue_total_res_pool_type:{
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.start.set(unit, core, pool_index, start_region * 1024));
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.count.set(unit, core, pool_index, ((((end_region + 1) - start_region) * 1024) / 4) / (nof_remote_cores))); /*Connector ID is mapped to one qaud, and represnted by one element in the pool*/
                break;
            }
            /* No need it's dead default
            default:{
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Pool info type is invalid, type is %d"),pool_info_type));
            }*/
        }
    }
    else if ( (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_1) ||
              (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_2) ||
              (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_se_cl_hr) || 
              (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync) || 
              (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_hr_connector_region_type_2_sync) ) {

        region_index = _BCM_DPP_AM_COSQ_GET_REGION_INDEX_FROM_POOL_INDEX(pool_index);
        nof_remote_cores = SOC_DPP_CONFIG(unit)->arad->region_nof_remote_cores[core][region_index];

        switch (pool_info_type) {
            case e2e_total_res_pool_type:{
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.start.set(unit, core, pool_index, start_region * 1024));
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.count.set(unit, core, pool_index, ((((end_region + 1) - start_region) * 1024) / 2) / (nof_remote_cores))); /*Num of SEs in a quad is 2 (for regions 1 &2)*/
                break;
            }
            case queue_total_res_pool_type:{
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.start.set(unit, core, pool_index, start_region * 1024));
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.count.set(unit, core, pool_index, ((((end_region + 1) - start_region) * 1024) / 2) / (nof_remote_cores))); /*Num of SEs in a quad is 2 (for regions 1 &2)*/
                break;
            }
            /* No nned it a dead code
            default:{
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Pool info type is invalid, type is %d"),pool_info_type));
            }*/
        }
    }
    else if ((desc_type == dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_1) ||
             (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_2)) {

        region_index = _BCM_DPP_AM_COSQ_GET_REGION_INDEX_FROM_POOL_INDEX(pool_index);
        nof_remote_cores = SOC_DPP_CONFIG(unit)->arad->region_nof_remote_cores[core][region_index];

        switch (pool_info_type) {
            case e2e_total_res_pool_type:{
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.start.set(unit, core, pool_index, start_region * 1024));
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.count.set(unit, core, pool_index, ((((end_region + 1) - start_region) * 1024) / 2) / (nof_remote_cores)));  /*Num of connectors in a quad is 2 (for regions 1 &2)*/
                break;
            }
            case queue_total_res_pool_type:{
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.start.set(unit, core, pool_index, start_region * 1024));
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.count.set(unit, core, pool_index, ((((end_region + 1) - start_region) * 1024) / 2) / (nof_remote_cores)));  /*Num of connectors in a quad is 2 (for regions 1 &2)*/
                break;
            }
            /*
            default:{
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Pool info type is invalid, type is %d"),pool_info_type));
            }*/
        }
    }
    else if ((desc_type == dpp_res_cosq_type->dpp_res_type_cosq_queue_unicast_cont) ||
             (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_queue_unicast_non_cont) || 
             (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_queue_multicast_cont) || 
             (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_queue_multicast_non_cont) ||
             (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_queue_isq_cont) ||
             (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_queue_isq_non_cont)) {

        /* Reserve queue 0-3 for FMQ - don't add these to the region */
        if (start_region == 0) {
            switch (pool_info_type) {
                case e2e_total_res_pool_type:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.start.set(unit, core, pool_index, (start_region * 1024) + BCM_FMQ_RESERVED_QUEUES / 4));
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.count.set(unit, core, pool_index, ((((end_region + 1) - start_region) * 1024) - BCM_FMQ_RESERVED_QUEUES) / 4));
                    break;
                }
                case queue_total_res_pool_type:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.start.set(unit, core, pool_index, (start_region * 1024) + BCM_FMQ_RESERVED_QUEUES / 4));
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.count.set(unit, core, pool_index, ((((end_region + 1) - start_region) * 1024) - BCM_FMQ_RESERVED_QUEUES) / 4));
                    break;
                }
                /*
                default:{
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Pool info type is invalid, type is %d"),pool_info_type));
                }*/
            }
        } else {
            switch (pool_info_type) {
                case e2e_total_res_pool_type:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.start.set(unit, core, pool_index, (start_region * 1024)));
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.count.set(unit, core, pool_index, (((end_region + 1) - start_region) * 1024) / 4));
                    break;
                }
                case queue_total_res_pool_type:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.start.set(unit, core, pool_index, start_region * 1024));
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.count.set(unit, core, pool_index, (((end_region + 1) - start_region) * 1024) / 4));
                    break;
                }
                /* No need. It's a deadcode.
                default:{
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Pool info type is invalid, type is %d"),pool_info_type));
                }*/
            }
        }
    }
    else {
        rv = BCM_E_INTERNAL;
    }
    switch (pool_info_type) {
        case e2e_total_res_pool_type:{
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.pool_id.get(unit, core, pool_index, &pool_id));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.p_pool_id.set(unit, core, pool_index, pool_id));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.type.set(unit, core, pool_index, desc_type));
            break;
        }
        case queue_total_res_pool_type:{
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.pool_id.get(unit, core, pool_index, &pool_id));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.p_pool_id.set(unit, core, pool_index, pool_id));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.type.set(unit, core, pool_index, desc_type));
            break;
        }
        /* No need. It's a dead code
        default:{
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Pool info type is invalid, type is %d"),pool_info_type));
        }*/
    }

    LOG_DEBUG(BSL_LS_BCM_COMMON,
              (BSL_META("    DBG, Updated Pool Entry, index(%d) Pool(%d)\n"), pool_index, pool_id));
 
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_am_cosq_pool_register(int unit, int core, int pool_index, int desc_type, _dpp_res_type_cosq_t *dpp_res_cosq_type)
{
    int rv = BCM_E_NONE;
    char *desc_ptr = NULL;
    sw_state_res_tagged_bitmap_extras_t   tagBitMapExtras;
    sw_state_res_allocator_t manager = SW_STATE_RES_ALLOCATOR_BITMAP;
    void *extraPtr = NULL;
    int pool_start = 0; 
    int internal_pool, internal_type, internal_pool_start;
    bcm_dpp_am_cosq_pool_info_type_t pool_info_type;
    int pool_type_id;
    int pool_id;
    int pool_count;
    BCMDNX_INIT_FUNC_DEFS;

    if (core < 0 || core >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Core %d id out of range"),core));
    }

    tagBitMapExtras.grain_size = 0;
    tagBitMapExtras.tag_length = 0;
    
    if (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_connector_cont) {
        pool_info_type = e2e_total_res_pool_type;
        desc_ptr = _dpp_res_cosq_desc[dpp_res_cosq_type->dpp_res_type_cosq_connector_cont - dpp_res_cosq_type->dpp_res_type_cosq_connector_cont];
        extraPtr = &tagBitMapExtras;
        manager = SW_STATE_RES_ALLOCATOR_TAGGED_BITMAP;
        tagBitMapExtras.grain_size = 2;
        tagBitMapExtras.tag_length = sizeof(int);
    } else if (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_1) {
        pool_info_type = e2e_total_res_pool_type;
        desc_ptr = _dpp_res_cosq_desc[dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_1 - dpp_res_cosq_type->dpp_res_type_cosq_connector_cont];
    } else if (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_2) {
        pool_info_type = e2e_total_res_pool_type;
        desc_ptr = _dpp_res_cosq_desc[dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_2 - dpp_res_cosq_type->dpp_res_type_cosq_connector_cont];
    } else if (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_1) {
        pool_info_type = e2e_total_res_pool_type;
        desc_ptr = _dpp_res_cosq_desc[dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_1 - dpp_res_cosq_type->dpp_res_type_cosq_connector_cont];
    } else if (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_2) {
        pool_info_type = e2e_total_res_pool_type;
        desc_ptr = _dpp_res_cosq_desc[dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_2 - dpp_res_cosq_type->dpp_res_type_cosq_connector_cont];
    } else if (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_se_cl_hr) {
        pool_info_type = e2e_total_res_pool_type;
        desc_ptr = _dpp_res_cosq_desc[dpp_res_cosq_type->dpp_res_type_cosq_se_cl_hr - dpp_res_cosq_type->dpp_res_type_cosq_connector_cont];
    } else if (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync) {
        pool_info_type = e2e_total_res_pool_type;
        desc_ptr = _dpp_res_cosq_desc[dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync - dpp_res_cosq_type->dpp_res_type_cosq_connector_cont];
    } else if (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_hr_connector_region_type_2_sync) {
        pool_info_type = e2e_total_res_pool_type;
        desc_ptr = _dpp_res_cosq_desc[dpp_res_cosq_type->dpp_res_type_cosq_hr_connector_region_type_2_sync - dpp_res_cosq_type->dpp_res_type_cosq_connector_cont];
    } else if (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_queue_unicast_cont) {
        pool_info_type = queue_total_res_pool_type;
        desc_ptr = _dpp_res_cosq_queue_desc[0];
    } else if (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_queue_unicast_non_cont) {
        pool_info_type = queue_total_res_pool_type;
        desc_ptr = _dpp_res_cosq_queue_desc[1];
    } else if (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_queue_multicast_cont) {
        pool_info_type = queue_total_res_pool_type;
        desc_ptr = _dpp_res_cosq_queue_desc[2];
    } else if (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_queue_multicast_non_cont) {
        pool_info_type = queue_total_res_pool_type;
        desc_ptr = _dpp_res_cosq_queue_desc[3];
    } else if (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_queue_isq_cont) {
        pool_info_type = queue_total_res_pool_type;
        desc_ptr = _dpp_res_cosq_queue_desc[4];
    } else if (desc_type == dpp_res_cosq_type->dpp_res_type_cosq_queue_isq_non_cont) {
        pool_info_type = queue_total_res_pool_type;
        desc_ptr = _dpp_res_cosq_queue_desc[5];
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("COSQ type is invalid type is %d"),desc_type));
    }

    switch (pool_info_type) {
        case e2e_total_res_pool_type:{
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.type_id.get(unit, core, pool_index, &pool_type_id));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.pool_id.get(unit, core, pool_index, &pool_id));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.count.get(unit, core, pool_index, &pool_count));
            break;
        }
        case queue_total_res_pool_type:{
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.type_id.get(unit, core, pool_index, &pool_type_id));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.pool_id.get(unit, core, pool_index, &pool_id));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.count.get(unit, core, pool_index, &pool_count));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.start.get(unit, core, pool_index, &pool_start));
            break;
        }
        /* No need. It's a dead code.
        default:{
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Pool info type is invalid, type is %d"),pool_info_type));
        }*/
    }

    LOG_DEBUG(BSL_LS_BCM_COMMON,
              (BSL_META_U(unit,
                          "  DBG, (%s) index(%d) Start(%d:0x%x) count(%d:0x%x)\n"), desc_ptr, pool_index, pool_start, pool_start, pool_count, pool_count));

    /* The type and pool numbers that will be used throghout the file are based on the resource id.
       However, the internal pool should start after all the other pools, at a different index than the resouce id. */
    rv = _bcm_dpp_am_cosq_get_first_base_pool_id(unit, &internal_pool_start);
    BCMDNX_IF_ERR_EXIT(rv);

    internal_type = pool_type_id - DPP_AM_RES_COSQ_START(unit) + internal_pool_start; 

    internal_pool = pool_id - DPP_AM_RES_COSQ_START(unit) + internal_pool_start;

    if(!SOC_WARM_BOOT(unit)) {
        rv = sw_state_res_pool_set(unit, internal_pool,
                        manager,
                        pool_start,
                        pool_count,
                        extraPtr,
                        desc_ptr);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = sw_state_res_type_set(unit, internal_type,
                                    internal_pool,
                                    1,
                                    desc_ptr);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Create mapping between the resource and pool. */
        rv = sw_state_access[unit].dpp.bcm.alloc_mngr_utils.resource_to_pool_map.set(unit, pool_type_id, internal_type);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int 
   _bcm_dpp_am_cosq_e2e_pool_init(int unit, int core, int region_index, int pool_index, int region_type, _dpp_res_type_cosq_t *dpp_res_cosq_type,
                                  bcm_dpp_am_cosq_pool_ref_type_t pool_type, int is_interdigitaded, int is_odd_even, int is_cl_cir) 
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    /*Update the corresponding pool entry field*/
    _bcm_dpp_am_cosq_update_pool_entry(unit, core, pool_index, TRUE, FALSE,
                  region_index, region_index, region_type, dpp_res_cosq_type);

    /*Register the pool_index to it's corresponding per type data base*/
    rv = _bcm_dpp_am_cosq_update_pool(unit, core, pool_type, pool_index, TRUE);
    BCMDNX_IF_ERR_EXIT(rv);

    /*Create region's pool*/
    rv = _bcm_dpp_am_cosq_pool_register(unit, core, pool_index, region_type, dpp_res_cosq_type);
    BCMDNX_IF_ERR_EXIT(rv);

    if (region_index > (SOC_DPP_DEFS_GET(unit, flow_region_fixed_type0_end)) - 1) {
        /*Driver call - configure region*/
        rv = _bcm_petra_flow_region_config(unit, core, region_index, region_index, is_interdigitaded, is_odd_even, is_cl_cir); 
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*Creating pools for all regions resources, according to SOC properties configuration*/
int
_bcm_dpp_am_cosq_process_regions(int unit, int core, int *flow_region_config, _dpp_res_type_cosq_t *dpp_res_cosq_type)
{
    int rv = BCM_E_NONE;
    int region_index = 0;
    int region_type;
    int pools_per_region = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_PER_REGION;
    int pool_index;
    bcm_dpp_am_cosq_pool_ref_type_t pool_type;

    BCMDNX_INIT_FUNC_DEFS;
    
    if (SOC_IS_QUX(unit)) { /*there are only 32 regions in QUX and these are the upper 32*/
        region_index = 96;
    }
    else if (SOC_IS_QAX(unit)) { /*there are only 64 regions in QAX and these are the upper 64*/
        region_index = 64;
    }
    /*A region corresponds to 1k of flows*/
    for (; region_index < DPP_DEVICE_COSQ_TOTAL_FLOW_REGIONS(unit)-1 /*last region is resrved to hr*/ ; ++region_index) {

         /*Set region type and initialize its corresponding pools according to soc property configuration*/
         switch (flow_region_config[region_index]) {
            case DPP_DEVICE_COSQ_REGION_TYPE0:
               /*Connector pool*/
               region_type = dpp_res_cosq_type->dpp_res_type_cosq_connector_cont;
               pool_type = connector_cont_res_pool_ref_type;
               pool_index =  region_index * pools_per_region + BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_CONNECTOR_OFFSET;
               _bcm_dpp_am_cosq_e2e_pool_init(unit, core, region_index, pool_index, region_type, dpp_res_cosq_type,
                                                pool_type, FALSE, TRUE, TRUE);
               break;
            case DPP_DEVICE_COSQ_REGION_TYPE1:
               /*Connector pool*/
               region_type = dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_1;
               pool_type = connector_non_cont_region_type_1_res_pool_ref_type;
               pool_index =  region_index * pools_per_region + BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_CONNECTOR_OFFSET;
               _bcm_dpp_am_cosq_e2e_pool_init(unit, core, region_index, pool_index, region_type, dpp_res_cosq_type,
                                                pool_type, TRUE, TRUE, TRUE);
               /*SE pool*/
               region_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_1;
               pool_type = se_cl_fq_region_type_1_res_pool_ref_type;
               pool_index =  region_index * pools_per_region + BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SE_OFFSET;
               _bcm_dpp_am_cosq_e2e_pool_init(unit, core, region_index, pool_index, region_type, dpp_res_cosq_type,
                                                pool_type, TRUE, TRUE, TRUE);
               break;
            case DPP_DEVICE_COSQ_REGION_TYPE2:
               /*Connector pool*/
               region_type = dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_2;
               pool_type = connector_non_cont_region_type_2_res_pool_ref_type;
               pool_index =  region_index * pools_per_region + BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_CONNECTOR_OFFSET;
               _bcm_dpp_am_cosq_e2e_pool_init(unit, core, region_index, pool_index, region_type, dpp_res_cosq_type,
                                                pool_type, TRUE, FALSE, TRUE);
               /*SE pool*/
               region_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_2;
               pool_type = se_cl_fq_region_type_2_res_pool_ref_type;
               pool_index =  region_index * pools_per_region + BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SE_OFFSET;
               _bcm_dpp_am_cosq_e2e_pool_init(unit, core, region_index, pool_index, region_type, dpp_res_cosq_type,
                                                pool_type, TRUE, FALSE, TRUE);
               /*Sync pool*/
               region_type = dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync;
               pool_type = fq_connector_region_type_2_sync_res_pool_ref_type;
               pool_index =  region_index * pools_per_region + BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SYNC_OFFSET;
               _bcm_dpp_am_cosq_e2e_pool_init(unit, core, region_index, pool_index, region_type, dpp_res_cosq_type,
                                                pool_type, TRUE, FALSE, TRUE);
               break;
               case DPP_DEVICE_COSQ_REGION_TYPE5:
               /*Connector pool*/
               region_type = dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_1;
               pool_type = connector_non_cont_region_type_1_res_pool_ref_type;
               pool_index =  region_index * pools_per_region + BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_CONNECTOR_OFFSET;
               _bcm_dpp_am_cosq_e2e_pool_init(unit, core, region_index, pool_index, region_type, dpp_res_cosq_type,
                                                pool_type, TRUE, TRUE, FALSE);
               /*SE pool*/
               region_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_1;
               pool_type = se_cl_fq_region_type_1_res_pool_ref_type;
               pool_index =  region_index * pools_per_region + BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SE_OFFSET;
               _bcm_dpp_am_cosq_e2e_pool_init(unit, core, region_index, pool_index, region_type, dpp_res_cosq_type,
                                                pool_type, TRUE, TRUE, FALSE);
               break;
            case DPP_DEVICE_COSQ_REGION_TYPE6:
               /*Connector pool*/
               region_type = dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_2;
               pool_type = connector_non_cont_region_type_2_res_pool_ref_type;
               pool_index =  region_index * pools_per_region + BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_CONNECTOR_OFFSET;
               _bcm_dpp_am_cosq_e2e_pool_init(unit, core, region_index, pool_index, region_type, dpp_res_cosq_type,
                                                pool_type, TRUE, FALSE, FALSE);
               /*SE pool*/
               region_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_2;
               pool_type = se_cl_fq_region_type_2_res_pool_ref_type;
               pool_index =  region_index * pools_per_region + BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SE_OFFSET;
               _bcm_dpp_am_cosq_e2e_pool_init(unit, core, region_index, pool_index, region_type, dpp_res_cosq_type,
                                                pool_type, TRUE, FALSE, FALSE);
               /*Sync pool*/
               region_type = dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync;
               pool_type = fq_connector_region_type_2_sync_res_pool_ref_type;
               pool_index =  region_index * pools_per_region + BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SYNC_OFFSET;
               _bcm_dpp_am_cosq_e2e_pool_init(unit, core, region_index, pool_index, region_type, dpp_res_cosq_type,
                                                pool_type, TRUE, FALSE, FALSE);
               break;
             }
    }
   /*Configuring the last region (special region resrved for HR)*/
   /*Connector pool*/
   region_type = dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_2;
   pool_type = connector_non_cont_region_type_2_res_pool_ref_type;
   pool_index =  region_index * pools_per_region + BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_CONNECTOR_OFFSET;
   _bcm_dpp_am_cosq_e2e_pool_init(unit, core, region_index, pool_index, region_type, dpp_res_cosq_type,
                                    pool_type, TRUE, FALSE, TRUE);
   /*SE pool*/
   region_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_hr;
   pool_type = se_cl_hr_res_pool_ref_type;
   pool_index =  region_index * pools_per_region + BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SE_OFFSET;
   _bcm_dpp_am_cosq_e2e_pool_init(unit, core, region_index, pool_index, region_type, dpp_res_cosq_type,
                                    pool_type, TRUE, FALSE, TRUE);
   /*Sync pool*/
   region_type = dpp_res_cosq_type->dpp_res_type_cosq_hr_connector_region_type_2_sync;
   pool_type = hr_connector_region_type_2_sync_res_pool_ref_type;
   pool_index =  region_index * pools_per_region + BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SYNC_OFFSET;
   _bcm_dpp_am_cosq_e2e_pool_init(unit, core, region_index, pool_index, region_type, dpp_res_cosq_type,
                                    pool_type, TRUE, FALSE, TRUE);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}




int
bcm_dpp_am_cosq_queue_region_config_get(int unit, int core, int *queue_region_config, int queue_region_type)
{
    int start_region = 0;
    int end_region, nof_queues_in_core;
    int region;

    int core_index, index = 0;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    nof_queues_in_core = DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit);
    end_region = nof_queues_in_core - 1;

    BCM_DPP_ASSYMETRIC_CORES_ITER(core, core_index) {

        if (queue_region_type == DPP_DEVICE_COSQ_QUEUE_REGION_MULTICAST) {
        
          BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_get_resource_region(unit, core_index, queue_multicast_total_dynamic_res_bmp, start_region,
                                                  end_region, &(queue_region_config[nof_queues_in_core * index])));

          for (region=0; 
               region < DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit); 
               region++) {
              /* set region type to indicate correct type */
              if (queue_region_config[region + (nof_queues_in_core * index)] == 1) {
                  queue_region_config[region + (nof_queues_in_core * index)] = DPP_DEVICE_COSQ_QUEUE_REGION_MULTICAST;
              }
          }
        } else if (queue_region_type == DPP_DEVICE_COSQ_QUEUE_REGION_UNICAST) {

           BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_get_resource_region(unit, core_index, queue_unicast_total_dynamic_res_bmp, start_region,
                                                  end_region, &(queue_region_config[nof_queues_in_core * index])));
           
           for (region=0; 
                region< DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit);
                region++) {
               /* set region type to indicate correct type */
               if (queue_region_config[region + (nof_queues_in_core * index)] == 1) {
                  queue_region_config[region + (nof_queues_in_core * index)] = DPP_DEVICE_COSQ_QUEUE_REGION_UNICAST;
               }
           }
        } else if (queue_region_type == DPP_DEVICE_COSQ_QUEUE_REGION_ISQ) {
          BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_get_resource_region(unit, core_index, queue_isq_total_dynamic_res_bmp, start_region,
                                                end_region, &(queue_region_config[nof_queues_in_core * index])));

          for (region=0; 
               region< DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit);
               region++) {
              /* set region type to indicate correct type */
              if (queue_region_config[region + (nof_queues_in_core * index)] == 1) {
                  queue_region_config[region + (nof_queues_in_core * index)] = DPP_DEVICE_COSQ_QUEUE_REGION_ISQ;
              }
          }
        }
        index++;
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_cosq_process_queue_region(int unit, 
                                      int core,
                                      int queue_config_type,
                                      int *queue_region_config)
{
    int rv = BCM_E_NONE;
    int region;
    int found_unicast;
    int found_multicast;
    int found_isq;
    int core_index;
    uint8 unicast_dynamic_free = 0, unicast_dynamic_total = 0, 
        multicast_dynamic_free = 0, multicast_dynamic_total = 0, 
        isq_dynamic_free = 0, isq_dynamic_total = 0,
        dynamic_free = 0;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_ASSYMETRIC_CORES_ITER(core, core_index) {
        for (region = 0;
             region < DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit);
             region++) {
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_total_dynamic_res.bit_get(unit, core_index, region, &unicast_dynamic_total));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_free_dynamic_res.bit_get(unit, core_index, region, &unicast_dynamic_free));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_total_dynamic_res.bit_get(unit, core_index, region, &multicast_dynamic_total));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_free_dynamic_res.bit_get(unit, core_index, region, &multicast_dynamic_free));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_total_dynamic_res.bit_get(unit, core_index, region, &isq_dynamic_total));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_free_dynamic_res.bit_get(unit, core_index, region, &isq_dynamic_free));

            dynamic_free = (unicast_dynamic_free && unicast_dynamic_total) /*This 1k was free for unicast queues*/ || 
                           (multicast_dynamic_free && multicast_dynamic_total) /*This 1k was free for multicast queues*/ || 
                           (isq_dynamic_free && isq_dynamic_total) /*This 1k was free for ISQs*/ ||
                           (!unicast_dynamic_total && !multicast_dynamic_total && !isq_dynamic_total); /*This 1k was not configured by anybody*/ 
            if (queue_config_type == DPP_DEVICE_COSQ_QUEUE_REGION_UNICAST) {
                found_unicast = (queue_region_config[region] == DPP_DEVICE_COSQ_QUEUE_REGION_UNICAST) ? TRUE : FALSE;

                if (found_unicast == FALSE) {
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core_index, queue_unicast_total_dynamic_res_bmp, region, region, FALSE, -1));
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core_index, queue_unicast_free_dynamic_res_bmp, region, region, FALSE, -1));
                } else {
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core_index, queue_unicast_total_dynamic_res_bmp, region, region, TRUE, -1));
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core_index, queue_unicast_free_dynamic_res_bmp, region, region, dynamic_free, -1));
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core_index, queue_multicast_total_dynamic_res_bmp, region, region, FALSE, -1));
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core_index, queue_multicast_free_dynamic_res_bmp, region, region, FALSE, -1));
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core_index, queue_isq_total_dynamic_res_bmp, region, region, FALSE, -1));
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core_index, queue_isq_free_dynamic_res_bmp, region, region, FALSE, -1));
                }
            }

            if (queue_config_type == DPP_DEVICE_COSQ_QUEUE_REGION_MULTICAST) {
                found_multicast = (queue_region_config[region] == DPP_DEVICE_COSQ_QUEUE_REGION_MULTICAST) ? TRUE : FALSE;

                if (found_multicast == FALSE) {
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core_index, queue_multicast_total_dynamic_res_bmp, region, region, FALSE, -1));
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core_index, queue_multicast_free_dynamic_res_bmp, region, region, FALSE, -1));
                } else {
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core_index, queue_unicast_total_dynamic_res_bmp, region, region, FALSE, -1));
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core_index, queue_unicast_free_dynamic_res_bmp, region, region, FALSE, -1));
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core_index, queue_multicast_total_dynamic_res_bmp, region, region, TRUE, -1));
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core_index, queue_multicast_free_dynamic_res_bmp, region, region, dynamic_free, -1));
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core_index, queue_isq_total_dynamic_res_bmp, region, region, FALSE, -1));
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core_index, queue_isq_free_dynamic_res_bmp, region, region, FALSE, -1));
                }
            }
            if (queue_config_type == DPP_DEVICE_COSQ_QUEUE_REGION_ISQ) {
                found_isq = (queue_region_config[region] == DPP_DEVICE_COSQ_QUEUE_REGION_ISQ) ? TRUE : FALSE;

                if (found_isq == FALSE) {
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core_index, queue_isq_total_dynamic_res_bmp, region, region, FALSE, -1));
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core_index, queue_isq_free_dynamic_res_bmp, region, region, FALSE, -1));
                } else {
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core_index, queue_unicast_total_dynamic_res_bmp, region, region, FALSE, -1));
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core_index, queue_unicast_free_dynamic_res_bmp, region, region, FALSE, -1));
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core_index, queue_multicast_total_dynamic_res_bmp, region, region, FALSE, -1));
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core_index, queue_multicast_free_dynamic_res_bmp, region, region, FALSE, -1));
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core_index, queue_isq_total_dynamic_res_bmp, region, region, TRUE, -1));
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_cosq_update_resource_region(unit, core_index, queue_isq_free_dynamic_res_bmp, region, region, dynamic_free, -1));
                }     
            }
        }
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Returns a region between 0-32 or 0-63 of the dynamic region range
 * user can request a region (requested_region).  If no requested region,
 * requested_region should be set to _DPP_NO_REGION_REQUESTED
 */
int
_bcm_dpp_am_cosq_region_alloc(int unit, int core, bcm_dpp_cosq_bmp_type_t type, int requested_region, int max_regions, int is_interdigit, int *region)
{
    int rv = BCM_E_NONE;
    int cur_region, quad_region = -1;
    uint8 result;
    int is_allocated, is_interdigitated;
    char *interdigit_str = NULL;
    
    BCMDNX_INIT_FUNC_DEFS;

    if (core < 0 || core >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Core %d id out of range"),core));
    }

    if (requested_region == _DPP_NO_REGION_REQUESTED) {         
        for (cur_region = 0; cur_region < max_regions; cur_region++) {
            switch (type) {
                case queue_unicast_free_dynamic_res_bmp:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_free_dynamic_res.bit_range_test(unit, core,  cur_region, 1, &result));
                    break;
                }
                case queue_multicast_free_dynamic_res_bmp:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_free_dynamic_res.bit_range_test(unit, core, cur_region, 1, &result));
                    break;
                }
                case queue_isq_free_dynamic_res_bmp:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_free_dynamic_res.bit_range_test(unit, core, cur_region, 1, &result));
                    break;
                }
                default:{
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid type")));
                }
            }

            if (result) {
                quad_region = _BCM_DPP_AM_COSQ_QUAD_REGION_GET(cur_region);
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.quad_interdigitated_res.is_quad_region_allocated.get(unit, core, quad_region, &is_allocated));

                if (is_allocated) {
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.quad_interdigitated_res.is_interdigitated.get(unit, core, quad_region, &is_interdigitated));
                    if (is_interdigitated != is_interdigit) {
                        continue;
                    }
                }

                break;
            }
        }
    
        if (cur_region >= max_regions) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("Current region is higher than maximal possible")));
        }
    }
    else {
        cur_region = requested_region;

        switch (type) {
            case queue_unicast_free_dynamic_res_bmp:{
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_free_dynamic_res.bit_range_test(unit, core,  cur_region, 1, &result));
                break;
            }
            case queue_multicast_free_dynamic_res_bmp:{
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_free_dynamic_res.bit_range_test(unit, core, cur_region, 1, &result));
                break;
            }
            case queue_isq_free_dynamic_res_bmp:{
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_free_dynamic_res.bit_range_test(unit, core, cur_region, 1, &result));
                break;
            }
            default:{
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid type")));
            }
        }

        if (result) {
            quad_region = _BCM_DPP_AM_COSQ_QUAD_REGION_GET(cur_region);
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.quad_interdigitated_res.is_quad_region_allocated.get(unit, core, quad_region, &is_allocated));

            if (is_allocated) {
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.quad_interdigitated_res.is_interdigitated.get(unit, core, quad_region, &is_interdigitated));
                if (is_interdigitated != is_interdigit) {
                    interdigit_str = (is_interdigitated) ? "interdigitated" : "not interdigitated";
                    BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("This 4K region(%d) was already allocated as %s. Can't configure the opposite on the same region."), quad_region, interdigit_str));
                }
            }
        } else {                             
            BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("No more space for given resource %d"),cur_region));
        }
    }

    /* (*resource) &= ~(1 << cur_region);  */
    switch (type) {
        case queue_unicast_free_dynamic_res_bmp:{
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_free_dynamic_res.bit_clear(unit, core, cur_region));
            break;
        }
        case queue_multicast_free_dynamic_res_bmp:{
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_free_dynamic_res.bit_clear(unit, core, cur_region));
            break;
        }
        case queue_isq_free_dynamic_res_bmp:{
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_free_dynamic_res.bit_clear(unit, core, cur_region));
            break;
        }
        /* must default. Otherwise - compilation error */
        /* coverity[dead_error_begin:FALSE] */
        default:{
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid type")));
        }
    }

    /* Mark quad region as contiguous/not-contiguous */
    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.quad_interdigitated_res.is_quad_region_allocated.set(unit, core, quad_region, 1));
    if (is_interdigit) {
        BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.quad_interdigitated_res.is_interdigitated.set(unit, core, quad_region, 1));
    } else {
        BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.quad_interdigitated_res.is_interdigitated.set(unit, core, quad_region, 0));
    }

    (*region) = cur_region;

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_cosq_region_free(int unit, SHR_BITDCL *resource, int region, int max_regions)
{
    int rv = BCM_E_NONE;
    

    BCMDNX_INIT_FUNC_DEFS;
    
    if (region >= max_regions) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("region given is higher than maximal region")));
    }

    (*resource) &= ~(1 << region); 
    SHR_BITCLR(resource,region);

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_cosq_allocate_resource_dynamically(int unit, int core_id, int access_core, int type, int requested_region, int *pool,
                                            _dpp_res_pool_cosq_t *dpp_res_cosq_pool, _dpp_res_type_cosq_t *dpp_res_cosq_type)
{
    int rv = BCM_E_NONE;
    int region, pool_index = 0;
    int start_region = 0, end_region = 0, max_regions = 0, is_interdigit = 0;
    bcm_dpp_cosq_bmp_type_t bmp_type;
    int pool_type_id;

    BCMDNX_INIT_FUNC_DEFS;

    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Core %d id out of range"),core_id));
    }

    if ((type == dpp_res_cosq_type->dpp_res_type_cosq_connector_cont) ||
        (type == dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_1) ||
        (type == dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_2) ||
        (type == dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_1) ||
        (type == dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_2) ||
        (type == dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync) ||
        (type == dpp_res_cosq_type->dpp_res_type_cosq_hr_connector_region_type_2_sync) ) {
        
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Dynamic allocation is not supported for E2E pool types")));
    }

    if ( (type == dpp_res_cosq_type->dpp_res_type_cosq_queue_unicast_non_cont) ||
         (type == dpp_res_cosq_type->dpp_res_type_cosq_queue_unicast_cont) ) {
        bmp_type = queue_unicast_free_dynamic_res_bmp;
        max_regions = DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit);
    
    } else if ( (type == dpp_res_cosq_type->dpp_res_type_cosq_queue_multicast_non_cont) ||
                (type == dpp_res_cosq_type->dpp_res_type_cosq_queue_multicast_cont) ) {
        bmp_type = queue_multicast_free_dynamic_res_bmp;
        max_regions = DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit);
    } else if ( (type == dpp_res_cosq_type->dpp_res_type_cosq_queue_isq_non_cont) ||
                (type == dpp_res_cosq_type->dpp_res_type_cosq_queue_isq_cont) ) {
        bmp_type = queue_isq_free_dynamic_res_bmp;
        max_regions = DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit);
    }
    else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("wrong type")));
    }

    if ( (type == dpp_res_cosq_type->dpp_res_type_cosq_queue_unicast_cont) ||
         (type == dpp_res_cosq_type->dpp_res_type_cosq_queue_multicast_cont) ||
         (type == dpp_res_cosq_type->dpp_res_type_cosq_queue_isq_cont) ) {
        is_interdigit = 0;
    } else {
        is_interdigit = 1;
    }

    rv = _bcm_dpp_am_cosq_region_alloc(unit, core_id, bmp_type, requested_region, max_regions, is_interdigit, &region);
    BCMDNX_IF_ERR_EXIT(rv);

    
    if ((type == dpp_res_cosq_type->dpp_res_type_cosq_queue_unicast_non_cont) ||
        (type == dpp_res_cosq_type->dpp_res_type_cosq_queue_unicast_cont) ||
        (type == dpp_res_cosq_type->dpp_res_type_cosq_queue_multicast_non_cont) ||
        (type == dpp_res_cosq_type->dpp_res_type_cosq_queue_multicast_cont) || 
        (type == dpp_res_cosq_type->dpp_res_type_cosq_queue_isq_cont) ||
        (type == dpp_res_cosq_type->dpp_res_type_cosq_queue_isq_non_cont) ) {

        start_region = end_region = region;
        pool_index = _DPP_AM_COSQ_QUEUE_INDEX_GET(dpp_res_cosq_pool->dpp_res_pool_cosq_queue_dynamic_start + region);
    }

    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.type_id.get(unit, core_id, pool_index, &pool_type_id));

    if (type == dpp_res_cosq_type->dpp_res_type_cosq_queue_unicast_cont) {

            rv = _bcm_dpp_am_cosq_update_pool_entry(unit, core_id, pool_index, TRUE, TRUE,
                              start_region, end_region, dpp_res_cosq_type->dpp_res_type_cosq_queue_unicast_cont, dpp_res_cosq_type);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_am_cosq_update_pool(unit, core_id, queue_unicast_cont_res_pool_ref_type, pool_type_id, TRUE);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_am_cosq_pool_register(unit, core_id, pool_index, dpp_res_cosq_type->dpp_res_type_cosq_queue_unicast_cont, dpp_res_cosq_type);
            BCMDNX_IF_ERR_EXIT(rv);

            (*pool) = _DPP_AM_COSQ_QUEUE_POOL_GET(pool_index);

            rv = _bcm_petra_queue_region_config(unit, access_core, region, region, FALSE /* itdg */);
    } else if (type == dpp_res_cosq_type->dpp_res_type_cosq_queue_unicast_non_cont) {
              rv = _bcm_dpp_am_cosq_update_pool_entry(unit, core_id, pool_index, TRUE, TRUE,
                                start_region, end_region, dpp_res_cosq_type->dpp_res_type_cosq_queue_unicast_non_cont, dpp_res_cosq_type);
              BCMDNX_IF_ERR_EXIT(rv);

              rv = _bcm_dpp_am_cosq_update_pool(unit, core_id, queue_unicast_non_cont_res_pool_ref_type, pool_type_id, TRUE);
              BCMDNX_IF_ERR_EXIT(rv);

              rv = _bcm_dpp_am_cosq_pool_register(unit, core_id, pool_index, dpp_res_cosq_type->dpp_res_type_cosq_queue_unicast_non_cont, dpp_res_cosq_type);
              BCMDNX_IF_ERR_EXIT(rv);

              (*pool) = _DPP_AM_COSQ_QUEUE_POOL_GET(pool_index);
              rv = _bcm_petra_queue_region_config(unit, access_core, region, region, TRUE /* itdg */);
    } else if (type == dpp_res_cosq_type->dpp_res_type_cosq_queue_multicast_cont) {
            rv = _bcm_dpp_am_cosq_update_pool_entry(unit, core_id, pool_index, TRUE, TRUE,
                              start_region, end_region, dpp_res_cosq_type->dpp_res_type_cosq_queue_multicast_cont, dpp_res_cosq_type);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_am_cosq_update_pool(unit, core_id, queue_multicast_cont_res_pool_ref_type, pool_type_id, TRUE);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_am_cosq_pool_register(unit, core_id, pool_index, dpp_res_cosq_type->dpp_res_type_cosq_queue_multicast_cont, dpp_res_cosq_type);
            BCMDNX_IF_ERR_EXIT(rv);

            (*pool) = _DPP_AM_COSQ_QUEUE_POOL_GET(pool_index);

            rv = _bcm_petra_queue_region_config(unit, access_core, region, region, FALSE /* itdg */);
    } else if (type == dpp_res_cosq_type->dpp_res_type_cosq_queue_multicast_non_cont) {
            rv = _bcm_dpp_am_cosq_update_pool_entry(unit, core_id, pool_index, TRUE, TRUE,
                              start_region, end_region, dpp_res_cosq_type->dpp_res_type_cosq_queue_multicast_non_cont, dpp_res_cosq_type);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_am_cosq_update_pool(unit, core_id, queue_multicast_non_cont_res_pool_ref_type, pool_type_id, TRUE);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_am_cosq_pool_register(unit, core_id, pool_index, dpp_res_cosq_type->dpp_res_type_cosq_queue_multicast_non_cont, dpp_res_cosq_type);
            BCMDNX_IF_ERR_EXIT(rv);

            (*pool) = _DPP_AM_COSQ_QUEUE_POOL_GET(pool_index);
            rv = _bcm_petra_queue_region_config(unit, access_core, region, region, TRUE /* itdg */);
    } else if (type == dpp_res_cosq_type->dpp_res_type_cosq_queue_isq_cont) {
            rv = _bcm_dpp_am_cosq_update_pool_entry(unit, core_id, pool_index, TRUE, TRUE,
                              start_region, end_region, dpp_res_cosq_type->dpp_res_type_cosq_queue_isq_cont, dpp_res_cosq_type);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_am_cosq_update_pool(unit, core_id, queue_isq_cont_res_pool_ref_type, pool_type_id, TRUE);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_am_cosq_pool_register(unit, core_id, pool_index, dpp_res_cosq_type->dpp_res_type_cosq_queue_isq_cont, dpp_res_cosq_type);
            BCMDNX_IF_ERR_EXIT(rv);

            (*pool) = _DPP_AM_COSQ_QUEUE_POOL_GET(pool_index);

            rv = _bcm_petra_queue_region_config(unit, access_core, region, region, FALSE /* itdg */);
    } else if (type == dpp_res_cosq_type->dpp_res_type_cosq_queue_isq_non_cont) {
            rv = _bcm_dpp_am_cosq_update_pool_entry(unit, core_id, pool_index, TRUE, TRUE,
                              start_region, end_region, dpp_res_cosq_type->dpp_res_type_cosq_queue_isq_non_cont, dpp_res_cosq_type);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_am_cosq_update_pool(unit, core_id, queue_isq_non_cont_res_pool_ref_type, pool_type_id, TRUE);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_am_cosq_pool_register(unit, core_id, pool_index, dpp_res_cosq_type->dpp_res_type_cosq_queue_isq_non_cont, dpp_res_cosq_type);
            BCMDNX_IF_ERR_EXIT(rv);

            (*pool) = _DPP_AM_COSQ_QUEUE_POOL_GET(pool_index);
            rv = _bcm_petra_queue_region_config(unit, access_core, region, region, TRUE /* itdg */);
    }


    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     bcm_dpp_am_cosq_init
 * Purpose:
 *     INIT for cosq module
 *     this has a dependency on "bcm_dpp_am_attach()" been already done
 * Parameters:
 *     unit       - Device number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_dpp_am_cosq_init    (int unit)
{
    int core = 0;
    int rv = BCM_E_NONE;
    int *flow_region_config = NULL, num_region, region;
    int num_entries, index;
    int *queue_region_config = NULL;
    _dpp_res_pool_cosq_t *dpp_res_cosq_pool = NULL;
    _dpp_res_type_cosq_t *dpp_res_cosq_type = NULL;
    int i;
    uint8 is_coldboot;
    BCMDNX_INIT_FUNC_DEFS;
    /* consistency check. if am module not initialized return error */

    /* allocate temporary storage */
    BCMDNX_ALLOC(flow_region_config, sizeof(int) *DPP_DEVICE_COSQ_TOTAL_FLOW_REGIONS(unit), "flow_region");
    if (flow_region_config == NULL) {
         LOG_ERROR(BSL_LS_BCM_COMMON,
                   (BSL_META_U(unit,
                               "unit %d, resource memory allocation failure\n"), unit));
         BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("resource memory allocation failure")));
    }
    BCMDNX_ALLOC(queue_region_config, sizeof(int) * DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit), "queue_region");
    if (queue_region_config == NULL) {
         LOG_ERROR(BSL_LS_BCM_COMMON,
                   (BSL_META_U(unit,
                               "unit %d, resource memory allocation failure\n"), unit));
         BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("resource memory allocation failure")));
    }
    BCMDNX_ALLOC(dpp_res_cosq_pool, (sizeof(_dpp_res_pool_cosq_t)), "res cos pool");
    if (dpp_res_cosq_pool == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    BCMDNX_ALLOC(dpp_res_cosq_type, (sizeof(_dpp_res_type_cosq_t)), "res cos type");
    if (dpp_res_cosq_type == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    /*
     * For voq, it is suffucient to use:
     * BCM_DPP_ASSYMETRIC_CORES_ITER(BCM_CORE_ALL, core) 
     * and, for symmetric mode, initialize only one 'core' entry.
     * However, if we initialize 'both cores' entries, we do the
     * right thing for e2e and we do no harm to voq.
     * Since the code below mixes the two cases, it is correct
     * to initialize all cores always.
     */

    is_coldboot = !SOC_WARM_BOOT(unit);

    BCM_DPP_CORES_ITER(BCM_CORE_ALL, core) {
        rv = _bcm_dpp_resources_fill_type_cosq(unit, core, dpp_res_cosq_type);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_resources_fill_pool_cosq(unit, core, dpp_res_cosq_pool);
        BCMDNX_IF_ERR_EXIT(rv);

        /*
         * allocate E2E storage
         */
        num_entries = dpp_res_cosq_pool->dpp_res_pool_cosq_e2e_end - dpp_res_cosq_pool->dpp_res_pool_cosq_connector_cont;
        LOG_DEBUG(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "  DBG, end(%d) start(%d)\n"), dpp_res_cosq_pool->dpp_res_pool_cosq_e2e_end, dpp_res_cosq_pool->dpp_res_pool_cosq_connector_cont));

        if (is_coldboot) {
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.alloc(unit, core, num_entries)); 
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.max_entries.set(unit, core, num_entries));
        

            /* setup to pre-allocated pools */
            for (index = 0; index < num_entries; index++) {
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.pool_id.set(unit, core, index, dpp_res_cosq_pool->dpp_res_pool_cosq_connector_cont + index));
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.type_id.set(unit, core, index, dpp_res_cosq_type->dpp_res_type_cosq_connector_cont + index));
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.p_pool_id.set(unit, core, index, dpp_res_cosq_pool->dpp_res_pool_cosq_connector_cont + index));

                LOG_DEBUG(BSL_LS_BCM_COMMON,
                          (BSL_META_U(unit,
                                      "  DBG, index(%d) Pool(%d)\n"), index, dpp_res_cosq_pool->dpp_res_pool_cosq_connector_cont + index));
            }


            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_cont_res_pool_ref.entries.alloc(unit, core, num_entries));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_cont_res_pool_ref.max_entries.set(unit, core, num_entries));

            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_non_cont_region_type_1_res_pool_ref.entries.alloc(unit, core, num_entries));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_non_cont_region_type_1_res_pool_ref.max_entries.set(unit, core, num_entries));

            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_non_cont_region_type_2_res_pool_ref.entries.alloc(unit, core, num_entries));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_non_cont_region_type_2_res_pool_ref.max_entries.set(unit, core, num_entries));

            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_fq_region_type_1_res_pool_ref.entries.alloc(unit, core, num_entries));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_fq_region_type_1_res_pool_ref.max_entries.set(unit, core, num_entries));

            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_fq_region_type_2_res_pool_ref.entries.alloc(unit, core, num_entries));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_fq_region_type_2_res_pool_ref.max_entries.set(unit, core, num_entries));

            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_hr_res_pool_ref.entries.alloc(unit, core, num_entries));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_hr_res_pool_ref.max_entries.set(unit, core, num_entries));

            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.fq_connector_region_type_2_sync_res_pool_ref.entries.alloc(unit, core, num_entries));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.fq_connector_region_type_2_sync_res_pool_ref.max_entries.set(unit, core, num_entries));

            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.hr_connector_region_type_2_sync_res_pool_ref.entries.alloc(unit, core, num_entries));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.hr_connector_region_type_2_sync_res_pool_ref.max_entries.set(unit, core, num_entries));

            for (i = 0; i < num_entries; i++) { /*mark all pools entries as free*/
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_cont_res_pool_ref.entries.set(unit, core, i, -1));
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_non_cont_region_type_1_res_pool_ref.entries.set(unit, core, i, -1));
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_non_cont_region_type_2_res_pool_ref.entries.set(unit, core, i, -1));
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_fq_region_type_1_res_pool_ref.entries.set(unit, core, i, -1));
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_fq_region_type_2_res_pool_ref.entries.set(unit, core, i, -1));
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_hr_res_pool_ref.entries.set(unit, core, i, -1));
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.fq_connector_region_type_2_sync_res_pool_ref.entries.set(unit, core, i, -1));
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.hr_connector_region_type_2_sync_res_pool_ref.entries.set(unit, core, i, -1));
            }
            /* Allocate valid regions */
            /* Allocate E2E regions */
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_fixed_res.alloc_bitmap(unit, core, DPP_DEVICE_COSQ_TOTAL_FLOW_REGIONS(unit)));

            /* Allocate Queue regions */
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_fixed_res.alloc_bitmap(unit, core, DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit)));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_total_dynamic_res.alloc_bitmap(unit, core, DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit)));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_free_dynamic_res.alloc_bitmap(unit, core, DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit)));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_total_dynamic_res.alloc_bitmap(unit, core, DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit)));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_free_dynamic_res.alloc_bitmap(unit, core, DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit)));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_total_dynamic_res.alloc_bitmap(unit, core, DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit)));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_free_dynamic_res.alloc_bitmap(unit, core, DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit)));

            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.quad_interdigitated_res.alloc(unit, core, DPP_DEVICE_COSQ_CONFIG_INTERDIGITATED_REGIONS(unit)));
            for (i = 0; i < DPP_DEVICE_COSQ_CONFIG_INTERDIGITATED_REGIONS(unit); i++) { /* mark all regions quads as not allocated and not interdigitated */
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.quad_interdigitated_res.is_quad_region_allocated.set(unit, core, i, 0));
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.quad_interdigitated_res.is_interdigitated.set(unit, core, i, 0));
            }

        }
        /*
         * retrieve region properties
         */
        num_region = 0;

        /* Fixed region0 */
        while (num_region < SOC_DPP_DEFS_GET(unit, flow_region_fixed_type0_end)) {

            flow_region_config[num_region]=DPP_DEVICE_COSQ_REGION_TYPE0;
            num_region++;
        }

        /* Soc property defined */
        region = num_region + 1; /*1 base*/

        if (SOC_IS_QUX(unit)) {
            region -= DPP_DEVICE_QUX_COSQ_FLOW_REGION_API_OFFSET; /*user SE regions are 16-32 while 112-128 in HW*/
        }
        else if (SOC_IS_QAX(unit)) {
            region -= DPP_DEVICE_QAX_COSQ_FLOW_REGION_API_OFFSET; /*user SE regions are 32-64 while 96-128 in HW*/
        }
        while (num_region < (DPP_DEVICE_COSQ_TOTAL_FLOW_REGIONS(unit) - 1 /*without HR region*/)) {

            rv = soc_dpp_prop_parse_dtm_mapping_mode(unit, core, region, DPP_DEVICE_COSQ_REGION_TYPE1, &flow_region_config[num_region]);
            BCMDNX_IF_ERR_EXIT(rv);

            if ( (flow_region_config[num_region] != DPP_DEVICE_COSQ_REGION_TYPE0) &&
                 (flow_region_config[num_region] != DPP_DEVICE_COSQ_REGION_TYPE1) &&
                 (flow_region_config[num_region] != DPP_DEVICE_COSQ_REGION_TYPE2) &&
                 (flow_region_config[num_region] != DPP_DEVICE_COSQ_REGION_TYPE5) &&
                 (flow_region_config[num_region] != DPP_DEVICE_COSQ_REGION_TYPE6) ) {
                    LOG_ERROR(BSL_LS_BCM_COMMON,
                              (BSL_META_U(unit,
                                          " unit(%d), invalid region(%d) mode %d setting default mode %d\n"),
                                          unit, region, flow_region_config[num_region], DPP_DEVICE_COSQ_REGION_TYPE1));
                
                    flow_region_config[num_region] = DPP_DEVICE_COSQ_REGION_TYPE1;
            }
            num_region++;
            region++;
        }
        
        /*HR region mapping mode default mode is type 2*/
        region = DPP_DEVICE_COSQ_CONFIG_FLOW_REGIONS_END(unit); /*1 base*/
        num_region = region - 1; /*0 base*/

        rv = soc_dpp_prop_parse_dtm_mapping_mode(unit, core, region, DPP_DEVICE_COSQ_REGION_TYPE2, &flow_region_config[num_region]);
        BCMDNX_IF_ERR_EXIT(rv);
        
        /*
         * determine and initialize all regions of all types 
         */
        if (is_coldboot) {
            rv = _bcm_dpp_am_cosq_process_regions(unit, core, flow_region_config, dpp_res_cosq_type); 
            BCMDNX_IF_ERR_EXIT(rv);
        }
        /*
         * Init am_cosq_allocation_db
         */
        rv = _bcm_dpp_am_cosq_init_flow_alloc_info_db(unit, core);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_am_cosq_init_flow_find_info_db(unit, core);
        BCMDNX_IF_ERR_EXIT(rv);
        /* Queue config start */
        if (is_coldboot) {
            num_entries = dpp_res_cosq_pool->dpp_res_pool_cosq_queue_dynamic_end - dpp_res_cosq_pool->dpp_res_pool_cosq_queue_unicast_cont;

            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.alloc(unit, core, num_entries));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.max_entries.set(unit, core, num_entries));

            for (index = 0; index < num_entries; index++) {
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.pool_id.set(unit, core, index, dpp_res_cosq_pool->dpp_res_pool_cosq_queue_unicast_cont + index));
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.type_id.set(unit, core, index, dpp_res_cosq_type->dpp_res_type_cosq_queue_unicast_cont + index));
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.p_pool_id.set(unit, core, index, dpp_res_cosq_pool->dpp_res_pool_cosq_queue_unicast_cont + index));
            }

            /* unicast queue pools */
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_cont_res_pool_ref.entries.alloc(unit, core, num_entries));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_cont_res_pool_ref.max_entries.set(unit, core, num_entries));

            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_non_cont_res_pool_ref.entries.alloc(unit, core, num_entries));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_non_cont_res_pool_ref.max_entries.set(unit, core, num_entries));

            /* multicast queue pools */
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_cont_res_pool_ref.entries.alloc(unit, core, num_entries));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_cont_res_pool_ref.max_entries.set(unit, core, num_entries));

            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_non_cont_res_pool_ref.entries.alloc(unit, core, num_entries));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_non_cont_res_pool_ref.max_entries.set(unit, core, num_entries));

            /* ISQ queue pools */
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_cont_res_pool_ref.entries.alloc(unit, core, num_entries));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_cont_res_pool_ref.max_entries.set(unit, core, num_entries));

            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_non_cont_res_pool_ref.entries.alloc(unit, core, num_entries));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_non_cont_res_pool_ref.max_entries.set(unit, core, num_entries));

            for (i = 0; i < num_entries; i++) { /*mark all pools entries as free*/
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_cont_res_pool_ref.entries.set(unit, core, i, -1));
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_non_cont_res_pool_ref.entries.set(unit, core, i, -1));
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_cont_res_pool_ref.entries.set(unit, core, i, -1));
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_non_cont_res_pool_ref.entries.set(unit, core, i, -1));
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_cont_res_pool_ref.entries.set(unit, core, i, -1));
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_non_cont_res_pool_ref.entries.set(unit, core, i, -1));
            }

            /* Configure 0-nof_queue_regions 1k queue regions to be valid */
            for (region = 0;
             region < (DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit));
             region++) {
                queue_region_config[region] = DPP_DEVICE_COSQ_QUEUE_REGION_UNICAST;
            }

            rv = _bcm_dpp_am_cosq_process_queue_region(unit, 
                                                       core, 
                                                       DPP_DEVICE_COSQ_QUEUE_REGION_UNICAST,
                                                       queue_region_config);

            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_petra_cosq_ucast_qid_range_set(unit, BCM_CORE_ALL, DPP_DEVICE_COSQ_FMQ_NON_ENHANCED_QID_MAX + 1, ((DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit) * 1024) - 1)); 
            BCMDNX_IF_ERR_EXIT(rv);

            /* queue config end */
        }
    }

exit:    
    if (flow_region_config != NULL) {
        BCM_FREE(flow_region_config);
    }
    if (queue_region_config != NULL) {
        BCM_FREE(queue_region_config);
    }
    if (dpp_res_cosq_pool != NULL) {
        BCM_FREE(dpp_res_cosq_pool);
    }
    if (dpp_res_cosq_type != NULL) {
        BCM_FREE(dpp_res_cosq_type);
    }
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_ingress_voq_allocate(int unit,
                                int core,
                                uint32 flags,
                                int is_non_contiguous,
                                int num_cos,
                                int queue_region_type,
                                int *voq_base)
{
    bcm_error_t rc = BCM_E_NONE;
    int core_index;
    int access_core;
    int align, offset, count;
    int cur_pool, valid_pool;
    int res_flags = 0;
    int element;
    int succeeded = FALSE;
    int pool_index, pool_offset, pool_base, pool_ref_index;
    int type;
    int requested_region = _DPP_NO_REGION_REQUESTED;
    _dpp_res_pool_cosq_t *dpp_res_cosq_pool = NULL;
    _dpp_res_type_cosq_t *dpp_res_cosq_type = NULL;
    int pool_valid_entires, pool_max_entires, pool_entry;
    bcm_dpp_am_cosq_pool_ref_type_t ref_pool_type;
    int pool_type;
    int pool_is_valid;
    int pool_type_id;
    int pool_start;

    BCMDNX_INIT_FUNC_DEFS;
    /* calling application always invokes in multiples of 4 */

    if ((num_cos % 4) != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Always invoke in multiples of 4")));
    }
    if (core == BCM_CORE_ALL && !SOC_DPP_CORE_MODE_IS_SYMMETRIC(unit) && !(SW_STATE_RES_ALLOC_WITH_ID & flags)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Cannot allocate ingress queue without ID in asymmetric mode.")));
    }

    BCMDNX_ALLOC(dpp_res_cosq_pool, (sizeof(_dpp_res_pool_cosq_t)), "res cos pool");
    if (dpp_res_cosq_pool == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    BCMDNX_ALLOC(dpp_res_cosq_type, (sizeof(_dpp_res_type_cosq_t)), "res cos type");
    if (dpp_res_cosq_type == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    
    BCM_DPP_ASSYMETRIC_CORES_ITER(core, core_index) {

        rc = _bcm_dpp_resources_fill_type_cosq(unit, core_index,dpp_res_cosq_type);
        BCMDNX_IF_ERR_EXIT(rc);

        rc = _bcm_dpp_resources_fill_pool_cosq(unit, core_index,dpp_res_cosq_pool);
        BCMDNX_IF_ERR_EXIT(rc);
      
        res_flags = flags;
        align = 1;
        offset = 0;

        if (is_non_contiguous) {
               count = (num_cos/4) * 2;
        }
        else {
               count = num_cos/4;
        }

        if (is_non_contiguous) {
            if (queue_region_type == DPP_DEVICE_COSQ_QUEUE_REGION_MULTICAST) {
                type = dpp_res_cosq_type->dpp_res_type_cosq_queue_multicast_non_cont;
                ref_pool_type = queue_multicast_non_cont_res_pool_ref_type;
            } else if (queue_region_type == DPP_DEVICE_COSQ_QUEUE_REGION_UNICAST) {
                ref_pool_type = queue_unicast_non_cont_res_pool_ref_type;
                type = dpp_res_cosq_type->dpp_res_type_cosq_queue_unicast_non_cont;
            } else {
                ref_pool_type = queue_isq_non_cont_res_pool_ref_type;
                type = dpp_res_cosq_type->dpp_res_type_cosq_queue_isq_non_cont;
            }
        }
        else {
            if (queue_region_type == DPP_DEVICE_COSQ_QUEUE_REGION_MULTICAST) {
                type = dpp_res_cosq_type->dpp_res_type_cosq_queue_multicast_cont;
                ref_pool_type = queue_multicast_cont_res_pool_ref_type;
            } else if (queue_region_type == DPP_DEVICE_COSQ_QUEUE_REGION_UNICAST) {
                ref_pool_type = queue_unicast_cont_res_pool_ref_type;
                type = dpp_res_cosq_type->dpp_res_type_cosq_queue_unicast_cont;
            } else {
                ref_pool_type = queue_isq_cont_res_pool_ref_type;
                type = dpp_res_cosq_type->dpp_res_type_cosq_queue_isq_cont;
            }
        }

        if (SW_STATE_RES_ALLOC_WITH_ID & res_flags) {

            LOG_DEBUG(BSL_LS_BCM_COMMON,
                      (BSL_META_U(unit,
                                  " with_id request for base queue(%d)\n"), *voq_base));

            pool_base = ((*voq_base) / 1024) * 1024;
                pool_offset = (*voq_base) % 1024;
            element = pool_base + pool_offset / 4;
                requested_region = pool_base/1024;

            pool_index =_DPP_AM_COSQ_QUEUE_INDEX_GET(dpp_res_cosq_pool->dpp_res_pool_cosq_queue_dynamic_start) + (pool_base/1024);

            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.is_valid.get(unit, core_index, pool_index, &pool_is_valid));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.type.get(unit, core_index, pool_index, &pool_type));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.type_id.get(unit, core_index, pool_index, &pool_type_id));

            if (pool_is_valid) {
                if (pool_type != type) {
                LOG_DEBUG(BSL_LS_BCM_COMMON,
                          (BSL_META_U(unit,
                                      " DBG, existing region attribute(%d) does not match the requested region attribute(%d)\n"), pool_type, type));
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Existing region attribute does not match the requested region attribute")));
                }
                LOG_DEBUG(BSL_LS_BCM_COMMON,
                          (BSL_META_U(unit,
                                      "  DBG, index(%d) element requested(%d)\n"), pool_index, element));
                cur_pool = pool_type_id;
                if (!SOC_WARM_BOOT(unit)) {
                    rc = dpp_am_res_alloc_align(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, cur_pool, res_flags,
                                 align, offset, count, &element);
                    BCMDNX_IF_ERR_EXIT(rc);
                }
                pool_index = _DPP_AM_COSQ_QUEUE_INDEX_GET(cur_pool);  
                succeeded = TRUE;
            } else {
                succeeded = FALSE;
            }

        } else {
            BCMDNX_IF_ERR_EXIT(sw_state_bcm_dpp_am_cosq_pool_ref_t_get(unit, core_index, ref_pool_type, max_entries_field, -1, &pool_max_entires));
            BCMDNX_IF_ERR_EXIT(sw_state_bcm_dpp_am_cosq_pool_ref_t_get(unit, core_index, ref_pool_type, valid_entries_field, -1, &pool_valid_entires));
            for (pool_ref_index = 0, valid_pool = 0, succeeded = FALSE;
                 ((valid_pool < pool_valid_entires) &&
                  (pool_ref_index < pool_max_entires)); pool_ref_index++) {

                BCMDNX_IF_ERR_EXIT(sw_state_bcm_dpp_am_cosq_pool_ref_t_get(unit, core_index, ref_pool_type, entries_field, pool_ref_index, &pool_entry));
                if (pool_entry == -1) {
                    continue;        
                }
                
                LOG_DEBUG(BSL_LS_BCM_COMMON,
                          (BSL_META_U(unit,
                                      "  DBG, index(%d)\n"), pool_ref_index));

                valid_pool++;
                
                    cur_pool = pool_entry;
                if (!SOC_WARM_BOOT(unit)) {
                    rc = dpp_am_res_alloc_align(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, cur_pool, res_flags, align, offset, count, &element);
                    if (rc == BCM_E_NONE) {
                        succeeded = TRUE;
                        break;
                    }
                    if (rc == BCM_E_RESOURCE) {
                        continue;
                    }
                }
                break;
            }
        }

        /* if required, attempt dynamic allocation */
        if (!succeeded) {

            if (SOC_DPP_CORE_MODE_IS_SYMMETRIC(unit))
            {
                /*
                 * in symmetrical case, if core is BCM_CORE_ALL then the core_index will be '0' (to allocate only one queue).
                 * In this case, HW should be configured in both cores.
                 */
                 access_core = core;
            }
            else
            {
                /*
                 * in asymmetrical case, if core is BCM_CORE_ALL function will iterate over all cores.
                 * Each HW core should be configured individually.
                 */
                 access_core = core_index;
            }

            rc = _bcm_dpp_cosq_allocate_resource_dynamically(unit, core_index, access_core, type, requested_region, &cur_pool, dpp_res_cosq_pool, dpp_res_cosq_type);
            BCMDNX_IF_ERR_EXIT(rc);

            LOG_DEBUG(BSL_LS_BCM_COMMON,
                      (BSL_META_U(unit,
                                  "  DBG, dynamic allocate index(%d)\n"), cur_pool));

            if (!SOC_WARM_BOOT(unit)) {
                rc = dpp_am_res_alloc_align(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, cur_pool, res_flags, align, offset, count, &element);
                if (rc != BCM_E_NONE) {
                    goto err;
                }
            }
        }
        pool_index = _DPP_AM_COSQ_QUEUE_INDEX_GET(cur_pool - dpp_res_cosq_type->dpp_res_type_cosq_queue_unicast_cont + dpp_res_cosq_pool->dpp_res_pool_cosq_queue_unicast_cont);
        BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.start.get(unit, core_index, pool_index, &pool_start));
        pool_base = pool_start & (~0x3ff);
        pool_offset = (element % 1024) * 4;
        (*voq_base) = pool_base + pool_offset;

        LOG_DEBUG(BSL_LS_BCM_COMMON,(BSL_META_U(unit,"  DBG, VOQ ID(%d:0x%x)\n"), (*voq_base), (*voq_base)));
    }
    BCMDNX_IF_ERR_EXIT(rc);
    BCM_EXIT;

err:
    /*dealloc pools if needed*/
    if ((rc != BCM_E_NONE) && (core == BCM_CORE_ALL) && (SW_STATE_RES_ALLOC_WITH_ID & res_flags)) {
        int core_index_dealloc;
        bcm_error_t rc_free = BCM_E_NONE;
        for (core_index_dealloc = 0; core_index_dealloc < core_index; core_index_dealloc++) {
            /* In Jericho core_index can be higher than 0, but for Arad it must be 0 (since num of cores is 1).*/ 
            /*coverity[dead_error_begin:FALSE] */              
            rc_free = bcm_dpp_am_ingress_voq_deallocate(unit, core_index_dealloc, flags, is_non_contiguous, num_cos, *voq_base);
            BCMDNX_IF_ERR_EXIT(rc_free);
        }
    }
    /*return err*/
    BCMDNX_IF_ERR_EXIT(rc);
    
exit:
    if (dpp_res_cosq_pool != NULL)  
       BCM_FREE(dpp_res_cosq_pool);
    if (dpp_res_cosq_type != NULL)
       BCM_FREE(dpp_res_cosq_type);
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_ingress_voq_deallocate(int unit,
                                  int core,
                                  uint32 flags,
                                  int is_non_contiguous,
                                  int num_cos,
                                  int voq_base)
{
    bcm_error_t rc = BCM_E_NONE;
    int core_index;
    int count;
    int element;
    int pool_index, pool_offset, pool_base, pool;
    _dpp_res_pool_cosq_t *dpp_res_cosq_pool = NULL;

    BCMDNX_INIT_FUNC_DEFS;
    /* calling application always invokes in multiples of 4 */
    if ((num_cos % 4) != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Always invoke in multiples of 4")));
    }

    BCMDNX_ALLOC(dpp_res_cosq_pool, (sizeof(_dpp_res_pool_cosq_t)), "res cos type");
    if (dpp_res_cosq_pool == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    BCM_DPP_ASSYMETRIC_CORES_ITER(core, core_index) {
        rc = _bcm_dpp_resources_fill_pool_cosq(unit, core_index,dpp_res_cosq_pool);
        BCMDNX_IF_ERR_EXIT(rc);

        if (is_non_contiguous) {
            count = (num_cos/4) * 2;
        }
        else {
            count = num_cos/4;
        }
       
        pool_base = (voq_base / 1024) * 1024;
        pool_offset = (voq_base - pool_base) / 4;
        element = pool_base + pool_offset;

        pool_index = _DPP_AM_COSQ_QUEUE_INDEX_GET((dpp_res_cosq_pool->dpp_res_pool_cosq_queue_dynamic_start) + (pool_base/1024));

        BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_total_res_pool.entries.type_id.get(unit, core_index, pool_index, &pool));
        if (!SOC_WARM_BOOT(unit)) {
            rc = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, pool, count, element);

            BCMDNX_IF_ERR_EXIT(rc);
        }
    }
exit:
    if (dpp_res_cosq_pool != NULL)
       BCM_FREE(dpp_res_cosq_pool);

    BCMDNX_FUNC_RETURN;
}



STATIC int _bcm_dpp_am_cosq_calc_count_identity(int nof_elements)
{
    return nof_elements;
}

STATIC int _bcm_dpp_am_cosq_calc_count_double(int nof_elements)
{
    return nof_elements * 2;
}

STATIC int _bcm_dpp_am_cosq_calc_count_div_by_4(int nof_elements)
{
    return nof_elements / 4;
}

STATIC int _bcm_dpp_am_cosq_calc_count_div_by_8(int nof_elements)
{
    return nof_elements / 8;
}

STATIC int _bcm_dpp_am_cosq_calc_count_cont(int nof_elements)
{
    return nof_elements / 4;
}

STATIC int _bcm_dpp_am_cosq_calc_count_cont_composite(int nof_elements)
{
    return nof_elements / 2;
}

STATIC int _bcm_dpp_am_cosq_se_flow_id_to_512_bitmap_element(int flow_id,int pool_base)
{
    int inner_pool_flow_id_offset = flow_id - pool_base;
    return (inner_pool_flow_id_offset / 2) + (inner_pool_flow_id_offset & 0x1);
}

STATIC int _bcm_dpp_am_cosq_se_512_bitmap_element_to_flow_id(int bitmap_element ,int pool_base)
{
    return pool_base + ((bitmap_element & ~(0x1)) * 2) + (bitmap_element & 0x1);
}


STATIC int _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element(int flow_id, int pool_base)
{
    int inner_pool_flow_id_offset = flow_id - pool_base;
    return (((inner_pool_flow_id_offset) & (~0x2))/2) + (inner_pool_flow_id_offset &0x1);
}

STATIC int _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_1(int flow_id, int pool_base)
{
    int inner_pool_flow_id_offset = flow_id - pool_base;
    return (((inner_pool_flow_id_offset) & (~0x2))/2) + (inner_pool_flow_id_offset &0x1) + 1;
}

STATIC int _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_2(int flow_id, int pool_base)
{
    int inner_pool_flow_id_offset = flow_id - pool_base;
    return (((inner_pool_flow_id_offset) & (~0x2))/2) + (inner_pool_flow_id_offset &0x1) + 2;
}

STATIC int _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_3(int flow_id, int pool_base)
{
    int inner_pool_flow_id_offset = flow_id - pool_base;
    return (((inner_pool_flow_id_offset) & (~0x2))/2) + (inner_pool_flow_id_offset &0x1) + 3;
}

STATIC int _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_4(int flow_id, int pool_base)
{
    int inner_pool_flow_id_offset = flow_id - pool_base;
    return (((inner_pool_flow_id_offset) & (~0x2))/2) + (inner_pool_flow_id_offset &0x1) + 4;
}

STATIC int _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_5(int flow_id, int pool_base)
{
    int inner_pool_flow_id_offset = flow_id - pool_base;
    return (((inner_pool_flow_id_offset) & (~0x2))/2) + (inner_pool_flow_id_offset &0x1) + 5;
}

STATIC int _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_6(int flow_id, int pool_base)
{
    int inner_pool_flow_id_offset = flow_id - pool_base;
    return (((inner_pool_flow_id_offset) & (~0x2))/2) + (inner_pool_flow_id_offset &0x1) + 6;
}

STATIC int _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_7(int flow_id, int pool_base)
{
    int inner_pool_flow_id_offset = flow_id - pool_base;
    return (((inner_pool_flow_id_offset) & (~0x2))/2) + (inner_pool_flow_id_offset &0x1) + 7;
}

STATIC int _bcm_dpp_am_cosq_connnector_512_bitmap_element_to_flow_id(int bitmap_element, int pool_base)
{
    return pool_base + (((bitmap_element & (~0x1)) * 2) + (bitmap_element & 0x1) + 2);
}

STATIC int _bcm_dpp_am_cosq_connector_flow_id_to_256_bitmap_element(int flow_id, int pool_base)
{
    int inner_pool_flow_id_offset = flow_id - pool_base;
    return inner_pool_flow_id_offset / 4 ;
}


STATIC int _bcm_dpp_am_cosq_connector_256_bitmap_element_to_flow_id(int bitmap_element, int pool_base)
{
    return pool_base + bitmap_element * 4 ;
}

STATIC int _bcm_dpp_am_cosq_flow_incrment_plus_4(int flow_id)
{
    return flow_id + 4 ;
}

STATIC int _bcm_dpp_am_cosq_flow_incrment_connector_non_contiguous(int flow_id)
{
    return (flow_id % 2) ? flow_id + 3 : flow_id + 1 ;
}

STATIC int
_bcm_dpp_am_cosq_init_flow_alloc_info_db(int unit, int core)
{
    int rc = BCM_E_NONE;
    _dpp_res_type_cosq_t *dpp_res_cosq_type = NULL;

    BCMDNX_INIT_FUNC_DEFS; 

    /*Get type structs*/
    BCMDNX_ALLOC(dpp_res_cosq_type, (sizeof(_dpp_res_type_cosq_t)), "res cos type");
    if (dpp_res_cosq_type == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    rc = _bcm_dpp_resources_fill_type_cosq(unit, core, dpp_res_cosq_type);
    BCMDNX_IF_ERR_EXIT(rc);
        
    /*Connector non composite region 0*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_0].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_connector_cont;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_0].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_CONNECTOR_OFFSET;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_0].is_mandatory = TRUE;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_0].is_last = TRUE;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_0].alloc_args.align = 1;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_0].alloc_args.offset = 0;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_0].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_cont;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_0].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_256_bitmap_element;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_0].alloc_state = BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_NONE;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_0].is_tag = TRUE;


    /*Connector composite region 0*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CONNECTOR_REGION_0] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_0];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CONNECTOR_REGION_0].alloc_args.align = 2;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CONNECTOR_REGION_0].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_cont_composite;


    /*Connector non composite region 1*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_1].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_1;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_1].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_CONNECTOR_OFFSET;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_1].is_mandatory = TRUE;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_1].is_last = TRUE;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_1].alloc_args.align = 4;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_1].alloc_args.offset = 0;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_1].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_1].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_1].alloc_state = BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_NONE;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_1].is_tag = FALSE;


    /*Connector composite region 1*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CONNECTOR_REGION_1] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_1];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CONNECTOR_REGION_1].alloc_args.align = 8;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CONNECTOR_REGION_1].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_double;


    /*Connector non composite region 2HR numcos==4 stage 1*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_1];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_2;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4].is_last = FALSE;
    /*Connector non composite region 2HR numcos==4 stage 2*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_hr_connector_region_type_2_sync;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SYNC_OFFSET;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4].is_mandatory = FALSE;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_div_by_4;
    /*Connector non composite region 2HR numcos==4 stage 3*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_1;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4].alloc_args.offset = 1;
    /*Connector non composite region 2HR numcos==4 stage 4*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_2;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4].alloc_args.offset = 2;
    /*Connector non composite region 2HR numcos==4 stage 5*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_3;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4].alloc_args.offset = 3;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4].is_last = TRUE;


    /*Connector non composite region 2FQ numcos==4 stage 1*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4];
    /*Connector non composite region 2FQ numcos==4 stage 2*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync;
    /*Connector non composite region 2FQ numcos==4 stage 3*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync;
    /*Connector non composite region 2FQ numcos==4 stage 4*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync;
    /*Connector non composite region 2FQ numcos==4 stage 5*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync;


    /*Connector non composite region 2HR numcos==8 stage 1*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8] =  _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4];
    /*Connector non composite region 2HR numcos==8 stage 2*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8] =  _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_div_by_8;
    /*Connector non composite region 2HR numcos==8 stage 3*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8] =  _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_div_by_8;
    /*Connector non composite region 2HR numcos==8 stage 4*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8] =  _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_div_by_8;
    /*Connector non composite region 2HR numcos==8 stage 5*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8] =  _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_div_by_8;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8].is_last = FALSE;
    /*Connector non composite region 2HR numcos==8 stage 6*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][5][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][5][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_4;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][5][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8].alloc_args.offset = 0;
    /*Connector non composite region 2HR numcos==8 stage 7*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][6][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][6][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_5;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][6][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8].alloc_args.offset = 1;
    /*Connector non composite region 2HR numcos==8 stage 8*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][7][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][7][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_6;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][7][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8].alloc_args.offset = 2;
    /*Connector non composite region 2HR numcos==8 stage 9*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][8][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][8][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_7;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][8][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8].is_last = TRUE;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][8][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8].alloc_args.offset = 3;


    /*Connector non composite region 2FQ numcos==8 stage 1*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8];
    /*Connector non composite region 2FQ numcos==8 stage 2*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync;
    /*Connector non composite region 2FQ numcos==8 stage 3*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync;
    /*Connector non composite region 2FQ numcos==8 stage 4*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync;
    /*Connector non composite region 2FQ numcos==8 stage 5*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync;
    /*Connector non composite region 2FQ numcos==8 stage 6*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][5][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][5][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][5][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync;
    /*Connector non composite region 2FQ numcos==8 stage 7*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][6][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][6][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][6][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync;
    /*Connector non composite region 2FQ numcos==8 stage 8*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][7][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][7][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][7][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync;
    /*Connector non composite region 2FQ numcos==8 stage 9*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][8][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][8][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][8][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync;


    /*Connector non composite region 2HR numcos==4 DEALLOC stage 1*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4];
    /*Connector non composite region 2HR numcos==4 DEALLOC stage 2*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC].alloc_state = BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_RESOURCE_CHECK;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SE_OFFSET;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC].pool_type =  dpp_res_cosq_type->dpp_res_type_cosq_se_cl_hr;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC].res_check_if_in_use_nxt_ptr = 1;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC].res_check_if_not_in_use_nxt_ptr = 0;
    /*Connector non composite region 2HR numcos==4 DEALLOC stage 3*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4];
    /*Connector non composite region 2HR numcos==4 DEALLOC stage 4*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_1;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC].alloc_args.offset = 1;
    /*Connector non composite region 2HR numcos==4 DEALLOC stage 5*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4];
    /*Connector non composite region 2HR numcos==4 DEALLOC stage 6*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][5][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][5][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_2;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][5][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC].alloc_args.offset = 2;
    /*Connector non composite region 2HR numcos==4 DEALLOC stage 7*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][6][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4];
    /*Connector non composite region 2HR numcos==4 DEALLOC stage 8*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][7][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][7][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_3;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][7][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC].alloc_args.offset = 3;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][7][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC].res_check_if_in_use_nxt_ptr = _BCM_DPP_AM_COSQ_RESOURCE_CHECK_BREAK;
    /*Connector non composite region 2HR numcos==4 DEALLOC stage 9*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][8][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4];


    /*Connector non composite region 2HR numcos==8 DEALLOC stage 1*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC];
    /*Connector non composite region 2HR numcos==8 DEALLOC stage 2*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC];
    /*Connector non composite region 2HR numcos==8 DEALLOC stage 3*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC];
    /*Connector non composite region 2HR numcos==8 DEALLOC stage 4*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC];
    /*Connector non composite region 2HR numcos==8 DEALLOC stage 5*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC];
    /*Connector non composite region 2HR numcos==8 DEALLOC stage 6*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][5][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][5][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC];
    /*Connector non composite region 2HR numcos==8 DEALLOC stage 7*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][6][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][6][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC];
    /*Connector non composite region 2HR numcos==8 DEALLOC stage 8*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][7][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][7][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][7][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC].res_check_if_in_use_nxt_ptr = 1;
    /*Connector non composite region 2HR numcos==8 DEALLOC stage 9*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][8][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][8][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][8][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC].is_last = FALSE;
    /*Connector non composite region 2HR numcos==8 DEALLOC stage 10*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][9][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][9][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC].alloc_args.offset = 0;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][9][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_4;
    /*Connector non composite region 2HR numcos==8 DEALLOC stage 11*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][10][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][10][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC].alloc_args.offset = 0;                                                                                 
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][10][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_4;   
    /*Connector non composite region 2HR numcos==8 DEALLOC stage 12*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][11][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][11][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC].alloc_args.offset = 1;                                                                                 
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][11][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_5;   
    /*Connector non composite region 2HR numcos==8 DEALLOC stage 13*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][12][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][12][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC].alloc_args.offset = 1;                                                                                 
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][12][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_5;   
    /*Connector non composite region 2HR numcos==8 DEALLOC stage 14*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][13][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][5][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][13][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC].alloc_args.offset = 2;                                                                                 
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][13][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_6;   
    /*Connector non composite region 2HR numcos==8 DEALLOC stage 15*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][14][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][6][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][14][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC].alloc_args.offset = 2;                                                                                 
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][14][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_6;   
    /*Connector non composite region 2HR numcos==8 DEALLOC stage 16*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][15][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][7][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][15][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC].alloc_args.offset = 3;                                                                                 
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][15][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_7;   
    /*Connector non composite region 2HR numcos==8 DEALLOC stage 17*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][16][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][8][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][16][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC].alloc_args.offset = 3;                                                                              
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][16][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_7;


    /*Connector non composite region 2FQ numcos==4 DEALLOC stage 1*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4];
    /*Connector non composite region 2FQ numcos==4 DEALLOC stage 2*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC].alloc_state = BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_RESOURCE_CHECK;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SE_OFFSET;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC].pool_type =  dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_2;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC].res_check_if_in_use_nxt_ptr = 1;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC].res_check_if_not_in_use_nxt_ptr = 0;
    /*Connector non composite region 2FQ numcos==4 DEALLOC stage 3*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4];
    /*Connector non composite region 2FQ numcos==4 DEALLOC stage 4*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_1;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC].alloc_args.offset = 1;
    /*Connector non composite region 2FQ numcos==4 DEALLOC stage 5*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4];
    /*Connector non composite region 2FQ numcos==4 DEALLOC stage 6*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][5][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][5][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_2;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][5][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC].alloc_args.offset = 2;
    /*Connector non composite region 2FQ numcos==4 DEALLOC stage 7*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][6][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4];
    /*Connector non composite region 2FQ numcos==4 DEALLOC stage 8*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][7][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][7][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_3;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][7][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC].alloc_args.offset = 3;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][7][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC].res_check_if_in_use_nxt_ptr = _BCM_DPP_AM_COSQ_RESOURCE_CHECK_BREAK;
    /*Connector non composite region 2FQ numcos==4 DEALLOC stage 9*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][8][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4];


    /*Connector non composite region 2FQ numcos==8 DEALLOC stage 1*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC];
    /*Connector non composite region 2FQ numcos==8 DEALLOC stage 2*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC];
    /*Connector non composite region 2FQ numcos==8 DEALLOC stage 3*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC];
    /*Connector non composite region 2FQ numcos==8 DEALLOC stage 4*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC];
    /*Connector non composite region 2FQ numcos==8 DEALLOC stage 5*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC];
    /*Connector non composite region 2FQ numcos==8 DEALLOC stage 6*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][5][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][5][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC];
    /*Connector non composite region 2FQ numcos==8 DEALLOC stage 7*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][6][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][6][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC];
    /*Connector non composite region 2FQ numcos==8 DEALLOC stage 8*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][7][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][7][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][7][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC].res_check_if_in_use_nxt_ptr = 1;
    /*Connector non composite region 2FQ numcos==8 DEALLOC stage 9*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][8][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][8][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][8][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC].is_last = FALSE;
    /*Connector non composite region 2FQ numcos==8 DEALLOC stage 10*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][9][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][9][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC].alloc_args.offset = 0;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][9][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_4;
    /*Connector non composite region 2FQ numcos==8 DEALLOC stage 11*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][10][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][10][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC].alloc_args.offset = 0;                                                                                 
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][10][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_4;   
    /*Connector non composite region 2FQ numcos==8 DEALLOC stage 12*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][11][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][3][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][11][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC].alloc_args.offset = 1;                                                                                 
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][11][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_5;   
    /*Connector non composite region 2FQ numcos==8 DEALLOC stage 13*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][12][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][4][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][12][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC].alloc_args.offset = 1;                                                                                 
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][12][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_5;   
    /*Connector non composite region 2FQ numcos==8 DEALLOC stage 14*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][13][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][5][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][13][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC].alloc_args.offset = 2;                                                                                 
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][13][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_6;   
    /*Connector non composite region 2FQ numcos==8 DEALLOC stage 15*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][14][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][6][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][14][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC].alloc_args.offset = 2;                                                                                 
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][14][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_6;   
    /*Connector non composite region 2FQ numcos==8 DEALLOC stage 16*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][15][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][7][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][15][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC].alloc_args.offset = 3;                                                                                 
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][15][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_7;   
    /*Connector non composite region 2FQ numcos==8 DEALLOC stage 17*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][16][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][8][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][16][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC].alloc_args.offset = 3;                                                                              
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][16][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element_plus_7;


    /*HR non composite non dual stage 1*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_hr;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SE_OFFSET;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].is_mandatory = TRUE;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].is_last = FALSE;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].alloc_args.align = 2;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].alloc_args.offset = 1;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_se_flow_id_to_512_bitmap_element;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].alloc_state = BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_NONE;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].is_tag = FALSE;
    /*HR non composite non dual stage 2*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_hr_connector_region_type_2_sync;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SYNC_OFFSET;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].is_mandatory = FALSE;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].is_last = TRUE;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].alloc_args.align = 2;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].alloc_args.offset = 1;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_se_flow_id_to_512_bitmap_element;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].alloc_state = BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_NONE;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].is_tag = FALSE;


    /*HR non composite non dual DEALLOC stage 1*/                                             
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2];
    /*HR non composite non dual DEALLOC stage 2*/                                             
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2_DEALLOC].alloc_state = BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_RESOURCE_CHECK;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2_DEALLOC].res_check_if_in_use_nxt_ptr = _BCM_DPP_AM_COSQ_RESOURCE_CHECK_BREAK;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2_DEALLOC].res_check_if_not_in_use_nxt_ptr = 0;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2_DEALLOC].is_last = FALSE;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2_DEALLOC].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_CONNECTOR_OFFSET;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2_DEALLOC].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_2;
    /*HR non composite non dual DEALLOC stage 3*/                                             
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2];


    /*HR dual stage 1*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2].alloc_args.offset = 0;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_double;
    /*HR dual stage 2*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2].alloc_args.offset = 0;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_double;


    /*HR non composite dual DEALLOC stage 1*/     
    sal_memcpy(&_bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2_DEALLOC],
               &_bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2_DEALLOC],
               sizeof(_bcm_dpp_am_cosq_allocation_info_t));
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2_DEALLOC].alloc_args.offset = 0;                                      
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2_DEALLOC].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_double; 
    /*HR non composite dual DEALLOC stage 2*/                                             
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2_DEALLOC].alloc_args.offset = 0;                                      
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2_DEALLOC].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_double; 
    /*HR non composite dual DEALLOC stage 3*/                                             
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2_DEALLOC].alloc_args.offset = 0;                                      
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2_DEALLOC].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_double; 


    /*HR composite stage 1*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_hr_connector_region_type_2_sync;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SYNC_OFFSET;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].is_mandatory = TRUE;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].is_last = FALSE;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].alloc_args.align = 2;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].alloc_args.offset = 1;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_se_flow_id_to_512_bitmap_element;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].alloc_state = BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_NONE;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].is_tag = FALSE;
    /*HR composite stage 2 - same as regular HR stage 1*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2];
    /*HR composite stage 3*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_2;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_CONNECTOR_OFFSET;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].is_mandatory = TRUE;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].is_last = TRUE;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].alloc_args.align = 1;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].alloc_args.offset = 0;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_se_flow_id_to_512_bitmap_element;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].alloc_state = BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_NONE;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].is_tag = FALSE;


    /*FQ region 2 non composite non dual stage 1*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_2] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_2].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_2;
    /*FQ region 2 non composite non dual stage 2*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_2] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_2].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync;


    /*FQ non composite non dual DEALLOC stage 1*/                                              
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_2_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_2_DEALLOC].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_2;
    /*FQ non composite non dual DEALLOC stage 2*/                                              
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_2_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2_DEALLOC];
    /*FQ non composite non dual DEALLOC stage 3*/                                              
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_2_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_2_DEALLOC].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync;


    /*FQ region 2 dual stage 1*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_2;
    /*FQ region 2 dual stage 2*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync;


    /*FQ non composite dual DEALLOC stage 1*/                                              
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2_DEALLOC].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_2;
    /*FQ non composite dual DEALLOC stage 2*/                                              
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2_DEALLOC];
    /*FQ non composite dual DEALLOC stage 3*/                                              
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2_DEALLOC].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync;


    /*FQ region 2 composite stage 1*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_FQ_REGION_2] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_FQ_REGION_2].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync;
    /*FQ region 2 composite stage 2*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_FQ_REGION_2] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_FQ_REGION_2].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_2;
    /*FQ region 2 composite stage 3*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_FQ_REGION_2] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2];


    /*FQ region 1 non composite non dual*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_1] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_1].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_1;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_1].is_last = TRUE;
    /*FQ region 1 dual*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_1] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_1].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_1;
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_1].is_last = TRUE;


    /*CL region_2HR non composite non dual stage 1 */
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2HR] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2HR].alloc_args.offset = 0;
    /*CL region_2HR non composite non dual stage 2 */
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2HR] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2HR].alloc_args.offset = 0;


    /*CL region_2HR non composite non dual DEALLOC stage 1 */
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2HR_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2HR_DEALLOC].alloc_args.offset = 0;
    /*CL region_2HR non composite non dual DEALLOC stage 2 */
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2HR_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2HR_DEALLOC].alloc_args.offset = 0;
    /*CL region_2HR non composite non dual DEALLOC stage 3 */
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2HR_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2HR_DEALLOC].alloc_args.offset = 0;


    /*CL region_2HR dual stage 1*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_CL_REGION_2HR] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2];
    /*CL region_2HR dual stage 2*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_CL_REGION_2HR] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2];


    /*CL region_2HR dual DEALLOC stage 1*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_CL_REGION_2HR_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2_DEALLOC];
    /*CL region_2HR dual DEALLOC stage 2*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_CL_REGION_2HR_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2_DEALLOC];
    /*CL region_2HR dual DEALLOC stage 3*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_CL_REGION_2HR_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2_DEALLOC];


    /*CL region_2HR enhanced stage 1*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_ENHANCED_CL_REGION_2HR] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2];
    /*CL region_2HR enhanced stage 2*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_ENHANCED_CL_REGION_2HR] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2];


    /*CL region_2HR enhanced DEALLOC stage 1*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_ENHANCED_CL_REGION_2HR_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2_DEALLOC];
    /*CL region_2HR enhanced DEALLOC stage 2*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_ENHANCED_CL_REGION_2HR_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2_DEALLOC];
    /*CL region_2HR enhanced DEALLOC stage 3*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_ENHANCED_CL_REGION_2HR_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2_DEALLOC];


    /*CL region_2HR composite stage 1 */
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_2HR] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_2HR].alloc_args.offset = 0;
    /*CL region_2HR composite stage 2*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_2HR] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_2HR].alloc_args.offset = 0;
    /*CL region_2HR composite stage 3*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_2HR] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_2HR].alloc_args.offset = 0;


    /*CL region_2FQ non composite non dual stage 1 */
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2FQ] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2FQ].alloc_args.offset = 0;
    /*CL region_2FQ non composite non dual stage 2*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2FQ] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2FQ].alloc_args.offset = 0;


    /*CL region_2FQ non composite non dual DEALLOC stage 1 */
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2FQ_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_2_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2FQ_DEALLOC].alloc_args.offset = 0;
    /*CL region_2FQ non composite non dual DEALLOC stage 2 */
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2FQ_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_2_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2FQ_DEALLOC].alloc_args.offset = 0;
    /*CL region_2FQ non composite non dual DEALLOC stage 3 */
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2FQ_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_2_DEALLOC];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2FQ_DEALLOC].alloc_args.offset = 0;


    /*CL region_2FQ dual stage 1 */
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_CL_REGION_2FQ] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2];
    /*CL region_2FQ dual stage 2*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_CL_REGION_2FQ] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2];


    /*CL region_2FQ dual DEALLOC stage 1*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_CL_REGION_2FQ_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2_DEALLOC];
    /*CL region_2FQ dual DEALLOC stage 2*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_CL_REGION_2FQ_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2_DEALLOC];
    /*CL region_2FQ dual DEALLOC stage 3*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_CL_REGION_2FQ_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2_DEALLOC];


    /*CL region_2FQ enhanced stage 1 */
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_ENHANCED_CL_REGION_2FQ] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2];
    /*CL region_2FQ enhanced stage 2*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_ENHANCED_CL_REGION_2FQ] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2];


    /*CL region_2FQ enhanced DEALLOC stage 1*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_ENHANCED_CL_REGION_2FQ_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2_DEALLOC];
    /*CL region_2FQ enhanced DEALLOC stage 2*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_ENHANCED_CL_REGION_2FQ_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2_DEALLOC];
    /*CL region_2FQ enhanced DEALLOC stage 3*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_ENHANCED_CL_REGION_2FQ_DEALLOC] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2_DEALLOC];

    /*CL region_2FQ composite stage 1*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_2FQ] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_FQ_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_2FQ].alloc_args.offset = 0;
    /*CL region_2FQ composite stage 2*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_2FQ] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_FQ_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][1][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_2FQ].alloc_args.offset = 0;
    /*CL region_2FQ composite stage 3*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_2FQ] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_FQ_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][2][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_2FQ].alloc_args.offset = 0;


    /*CL region 1 non composite non dual*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_1] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_1];
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_1].alloc_args.offset = 0;


    /*CL region 1 dual*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_CL_REGION_1] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_1];


    /*CL region 1 composite*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_1] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_1];


    /*CL region 1 enhanced*/
    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_ENHANCED_CL_REGION_1] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][0][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_1];

exit:
    if (dpp_res_cosq_type != NULL)
       BCM_FREE(dpp_res_cosq_type);
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_am_cosq_init_flow_find_info_db(int unit, int core)
{
    int rc;
    _dpp_res_type_cosq_t *dpp_res_cosq_type = NULL;

    BCMDNX_INIT_FUNC_DEFS; 

    /*Get type structs*/
    BCMDNX_ALLOC(dpp_res_cosq_type, (sizeof(_dpp_res_type_cosq_t)), "res cos type");
    if (dpp_res_cosq_type == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    rc = _bcm_dpp_resources_fill_type_cosq(unit, core ,dpp_res_cosq_type);
    BCMDNX_IF_ERR_EXIT(rc);

    /*Connector non composite region 0*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_0].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_connector_cont;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_0].relevant_pool_indices_type = connector_cont_res_pool_ref_type;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_0].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_CONNECTOR_OFFSET;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_0].alloc_args.align = 1;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_0].alloc_args.offset = 0;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_0].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_cont;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_0].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_256_bitmap_element;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_0].alloc_args.bitmap_elem_to_flow_id = _bcm_dpp_am_cosq_connector_256_bitmap_element_to_flow_id;


    /*Connector composite region 0*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CONNECTOR_REGION_0] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_0];
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CONNECTOR_REGION_0].alloc_args.align = 2;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CONNECTOR_REGION_0].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_cont_composite;


    /*Connector non composite region 1*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_1].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_1;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_1].relevant_pool_indices_type = connector_non_cont_region_type_1_res_pool_ref_type;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_1].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_CONNECTOR_OFFSET;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_1].alloc_args.align = 4;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_1].alloc_args.offset = 0;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_1].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_1].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_1].alloc_args.bitmap_elem_to_flow_id = _bcm_dpp_am_cosq_connnector_512_bitmap_element_to_flow_id;


    /*Connector composite region 1*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CONNECTOR_REGION_1] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_0];
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CONNECTOR_REGION_1].alloc_args.align = 8;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CONNECTOR_REGION_1].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_double;


    /*Connector non composite region 2HR num_cos==4*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_1];
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_2;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4].relevant_pool_indices_type = connector_non_cont_region_type_2_res_pool_ref_type;
    

    /*Connector non composite region 2FQ num_cos==4*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4];


    /*Connector non composite region 2FQ num_cos==8*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4];


    /*Connector non composite region 2FQ num_cos==8*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4];


    /*HR non composite non dual*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_hr;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].relevant_pool_indices_type = se_cl_hr_res_pool_ref_type;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SE_OFFSET;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].alloc_args.align = 2;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].alloc_args.offset = 1;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_se_flow_id_to_512_bitmap_element;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2].alloc_args.bitmap_elem_to_flow_id = _bcm_dpp_am_cosq_se_512_bitmap_element_to_flow_id;


    /*HR dual*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2].alloc_args.offset = 0;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_double;
    

    /*HR composite*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_hr_connector_region_type_2_sync;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].relevant_pool_indices_type = hr_connector_region_type_2_sync_res_pool_ref_type;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SYNC_OFFSET;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].alloc_args.align = 2;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].alloc_args.offset = 1;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_se_flow_id_to_512_bitmap_element;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2].alloc_args.bitmap_elem_to_flow_id  = _bcm_dpp_am_cosq_se_512_bitmap_element_to_flow_id;
    

    /*FQ region 2 non composite non dual*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_2] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_2].relevant_pool_indices_type = se_cl_fq_region_type_2_res_pool_ref_type;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_2].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_2;


    /*FQ region 2 dual*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2].relevant_pool_indices_type = se_cl_fq_region_type_2_res_pool_ref_type;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_2;


    /*FQ region 2 composite*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_FQ_REGION_2] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_FQ_REGION_2].relevant_pool_indices_type = fq_connector_region_type_2_sync_res_pool_ref_type;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_FQ_REGION_2].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync;


    /*FQ region 1 non composite non dual*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_1] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_1].relevant_pool_indices_type = se_cl_fq_region_type_1_res_pool_ref_type;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_1].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_1;


    /*FQ region 1 dual*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_1] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_1].relevant_pool_indices_type = se_cl_fq_region_type_1_res_pool_ref_type;
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_1].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_1;


    /*CL region_2HR non composite non dual*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2HR] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2HR].alloc_args.offset = 0;


    /*CL region_2HR dual*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_CL_REGION_2HR] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2];


    /*CL region_2HR enhanced*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_ENHANCED_CL_REGION_2HR] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2];


    /*CL region_2HR composite*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_2HR] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_2HR].alloc_args.offset = 0;


    /*CL region_2FQ non composite non dual*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2FQ] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2FQ].alloc_args.offset = 0;


    /*CL region_2FQ dual*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_CL_REGION_2FQ] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2];
    

    /*CL region_2FQ enhanced*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_ENHANCED_CL_REGION_2FQ] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2];


    /*CL region_2FQ composite*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_2FQ] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_FQ_REGION_2];
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_2FQ].alloc_args.offset = 0;
    

    /*CL region 1 non composite non dual*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_1] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_1];
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_1].alloc_args.offset = 0;


    /*CL region 1 dual*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_CL_REGION_1] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_1];


    /*CL region 1 composite*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_1] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_1];


    /*CL region 1 enhanced*/
    _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_ENHANCED_CL_REGION_1] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_1];

exit:
    if (dpp_res_cosq_type != NULL)
       BCM_FREE(dpp_res_cosq_type);
    BCMDNX_FUNC_RETURN;
}




/* This function has several state and can perform allocation* 
 * deallocation and resource check as the arguments are the same*/
STATIC int
    _bcm_dpp_am_cosq_scheduler_with_id_a_mngr_action(int unit,
                                                int core,
                                                int* type_arr,
                                                int type_arr_len,
                                                _bcm_dpp_am_cosq_align_alloc_args_t* alloc_args,
                                                int nof_elements,
                                                int pool_offset,
                                                int flow_id,
                                                _bcm_dpp_am_cosq_allocate_func_state_t func_state,
                                                int* resource_check /*relevant only for func_state == res_check*/,
                                                uint8 *tag /*relevant only for tagged allocation */)
{
    int rc = BCM_E_NONE; 
    int res_flags = SW_STATE_RES_ALLOC_WITH_ID;
    int i = 0;
    int pool_base_flow_id;
    int pool_element;
    int type_id;
    int count;
    /*Pool index is caluculated acoording to it's correspponding region*3 + offset (which indicates pool type - connector, se or sync)*/
    int pool_index = _BCM_DPP_AM_COSQ_GET_POOL_INDEX(flow_id,pool_offset);
    int pool_type, pool_is_valid;

    BCMDNX_INIT_FUNC_DEFS;

    if (core < 0 || core >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Core %d id out of range"),core));
    }

    /*Region type consistency check*/
    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.type.get(unit, core, pool_index, &pool_type));
    while (i < type_arr_len) {
        if (type_arr[i] == pool_type) {
            break;
        }
        ++i;
        if (i == type_arr_len) {
            LOG_DEBUG(BSL_LS_BCM_COMMON,
                      (BSL_META_U(unit,
                                  " DBG, given region type(%d) does not match the requested region attribute\n"), pool_type));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Mismatch in flow type and given type")));
        }
    }
    
    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.is_valid.get(unit, core, pool_index, &pool_is_valid));
    if (pool_is_valid) {
        /*type_id identifies the pool*/
        BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.type_id.get(unit, core, pool_index, &type_id));
        /*pool_base_flow_id is the first flow_id that the current pool points to*/
        BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.start.get(unit, core, pool_index, &pool_base_flow_id));
        /*calculate pool_element according to it's flow_id and the flow_id in the base of the pool*/
        pool_element = alloc_args->flow_id_to_bitmap_elem(flow_id ,pool_base_flow_id);
        /*calculate count*/
        count = alloc_args->calc_count(nof_elements);

        LOG_DEBUG(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "  DBG, index(%d) element requested(%d)\n"), pool_index, pool_element));

        switch (func_state){
		case BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_RESOURCE_CHECK:
                /*
                 * Call this procedure with 'core_id = 0' since, in cosq, 'type_id' (resource id) matches
                 * 'pool id' in 1 to 1 correspondence so there is no need for another translation.
                 */
                *resource_check = ((rc = dpp_am_res_check(unit, 0,type_id,count,pool_element)) == BCM_E_EXISTS);
                break;
        
            case BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_ALLOCATE:
                if (!tag) {
                    /*
                     * Call this procedure with 'core_id = 0' since, in cosq, 'type_id' (resource id) matches
                     * 'pool id' in 1 to 1 correspondence so there is no need for another translation.
                     */
                    if (!SOC_WARM_BOOT(unit)) {
                        rc = dpp_am_res_alloc_align(unit, 0, type_id, res_flags, alloc_args->align, alloc_args->offset, count, &pool_element); 
                        if(rc == BCM_E_RESOURCE){
                            BCM_ERR_EXIT_NO_MSG(rc);
                        }
                        BCMDNX_IF_ERR_EXIT_MSG(rc, (_BSL_BCM_MSG("Error allocating flow %d"),flow_id)); 
                    }
                }
                else{
                    /*
                     * Call this procedure with 'core_id = 0' since, in cosq, 'type_id' (resource id) matches
                     * 'pool id' in 1 to 1 correspondence so there is no need for another translation.
                     */
                    if (!SOC_WARM_BOOT(unit)) {
                        rc = dpp_am_res_alloc_align_tag(unit, 0, type_id, res_flags, alloc_args->align, alloc_args->offset, tag, count, &pool_element); 
                        if(rc == BCM_E_RESOURCE){
                            BCM_ERR_EXIT_NO_MSG(rc);
                        }
                        BCMDNX_IF_ERR_EXIT_MSG(rc, (_BSL_BCM_MSG("Error allocating flow %d"),flow_id)); 
                    }
                }
                break;

            case BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_DEALLOCATE:
            default:
                /*
                 * Call this procedure with 'core_id = 0' since, in cosq, 'type_id' (resource id) matches
                 * 'pool id' in 1 to 1 correspondence so there is no need for another translation.
                 */
                if (!SOC_WARM_BOOT(unit)) {
                    rc = dpp_am_res_free(unit, 0, type_id, count, pool_element); 
                    if(rc == BCM_E_RESOURCE){
                            BCM_ERR_EXIT_NO_MSG(rc);
                    }
                    BCMDNX_IF_ERR_EXIT_MSG(rc, (_BSL_BCM_MSG("Error deallocating flow %d"),flow_id)); 
                }
                break;
        }
    } else {
        rc = BCM_E_PARAM;
    }
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
    _bcm_dpp_am_cosq_scheduler_without_id_allocate(int unit,
                                                   int core,
                                                   uint32 nof_remote_cores,
                                                   bcm_dpp_am_cosq_pool_ref_type_t res_pool_ref_type,
                                                   _bcm_dpp_am_cosq_align_alloc_args_t* alloc_args,
                                                   int nof_elements,
                                                   SOC_TMC_AM_SCH_FLOW_TYPE flow_type,
                                                   int* flow_id)
{
    int res_flags = 0;
    int rc = BCM_E_RESOURCE;
    int pool_ref_index;
    int valid_pool;
    int pool_element = 0;
    int pool_base_flow_id;
    int type_id;
    int pool_index;
    int count;
    uint32 region;
    int pool_ref_valid_entries, pool_ref_max_entries, pool_entry;
   
    BCMDNX_INIT_FUNC_DEFS;

    if (core < 0 || core >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Core %d id out of range"),core));
    }

    BCMDNX_IF_ERR_EXIT(sw_state_bcm_dpp_am_cosq_pool_ref_t_get(unit, core, res_pool_ref_type, valid_entries_field, -1, &pool_ref_valid_entries));
    BCMDNX_IF_ERR_EXIT(sw_state_bcm_dpp_am_cosq_pool_ref_t_get(unit, core, res_pool_ref_type, max_entries_field, -1, &pool_ref_max_entries));
    /*Try to allocate in the first valid_pool found*/
    for (pool_ref_index  = 0, valid_pool = 0 ;
              ((valid_pool < pool_ref_valid_entries) &&
                   (pool_ref_index < pool_ref_max_entries)); ++pool_ref_index) 
    {
        /*Find first pool_index of the requested type*/
        BCMDNX_IF_ERR_EXIT(sw_state_bcm_dpp_am_cosq_pool_ref_t_get(unit, core, res_pool_ref_type, entries_field, pool_ref_index, &pool_entry));
        if (pool_entry == -1) {
            continue;        
        }
        LOG_DEBUG(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "  DBG, refIndex(%d)\n"), pool_ref_index));

        ++valid_pool;

        /*Get its index in the pools entries array*/
        pool_index = pool_entry;

        /* validate requested nof_remote_cores matches the pool */
        if (flow_type == SOC_TMC_AM_SCH_FLOW_TYPE_CONNECTOR) {
            region = _BCM_DPP_AM_COSQ_GET_REGION_INDEX_FROM_POOL_INDEX(pool_index);
            if (nof_remote_cores != 
                    SOC_DPP_CONFIG(unit)->arad->region_nof_remote_cores[core][region]) {
                continue;
            }
        }
        /*Get it's relevant per device pool id*/
        BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.type_id.get(unit, core, pool_index, &type_id));
        /*calculate count*/
        count = alloc_args->calc_count(nof_elements);

        BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.start.get(unit, core, pool_index, &pool_base_flow_id));
        /*
         * Call this procedure with 'core_id = 0' since, in cosq, 'type_id' (resource id) matches
         * 'pool id' in 1 to 1 correspondence so there is no need for another translation.
         */
        if (!SOC_WARM_BOOT(unit)) {
        rc = dpp_am_res_alloc_align(unit, 0, type_id, res_flags, alloc_args->align, alloc_args->offset, count, &pool_element);
        if (rc == BCM_E_RESOURCE) {
            continue;
        }
        if (rc != BCM_E_NONE) {
            break;
            }
        }
        /*In case valid flow id is found calculate it and break*/
        (*flow_id) = alloc_args->bitmap_elem_to_flow_id(pool_element,pool_base_flow_id);
        /*Break outer loop*/
        break;
    }
    if(rc == BCM_E_RESOURCE){
        BCM_ERR_EXIT_NO_MSG(rc);
    }
    BCMDNX_IF_ERR_EXIT_MSG(rc, (_BSL_BCM_MSG("Error in allocating flow")));

exit:
    BCMDNX_FUNC_RETURN;
}

/*Retrieves a region type according to the given flow_id*/
STATIC int 
_bcm_dpp_am_cosq_flow_id_to_region_type_get(int unit, int core, int flow_id, bcm_dpp_am_cosq_region_type_t* region_type)
{
    int rc = BCM_E_NONE;
    int pool_offset;
    int pool_index;
    _dpp_res_type_cosq_t *dpp_res_cosq_type = NULL;
    int pool_type;

    BCMDNX_INIT_FUNC_DEFS; 

    /*Get type structs*/
    BCMDNX_ALLOC(dpp_res_cosq_type, (sizeof(_dpp_res_type_cosq_t)), "res cos type");
    if (dpp_res_cosq_type == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    rc = _bcm_dpp_resources_fill_type_cosq(unit, core,dpp_res_cosq_type);
    BCMDNX_IF_ERR_EXIT(rc);

    /*Set pool offset*/
    pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_CONNECTOR_OFFSET;

    /*Get the pool_index corresponding to the flow_id*/
    pool_index = _BCM_DPP_AM_COSQ_GET_POOL_INDEX(flow_id,pool_offset);

    /*Get region*/
    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.type.get(unit, core, pool_index, &pool_type));
    if (pool_type == dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_1) {
        *region_type = BCM_DPP_AM_COSQ_REGION_TYPE_1;
    }
    else { 
        if (pool_type == dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_2) {
            *region_type = BCM_DPP_AM_COSQ_REGION_TYPE_2;
        }
        else {
            *region_type = BCM_DPP_AM_COSQ_REGION_TYPE_0;
        }
    }
    
    /*In region 2 we need to differentiate between hr and fq regions*/
    if (*region_type == BCM_DPP_AM_COSQ_REGION_TYPE_2) {
        /*Set pool offset*/
        pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SE_OFFSET;
        /*Get the pool_id corresponding to the flow_id*/
        pool_index = _BCM_DPP_AM_COSQ_GET_POOL_INDEX(flow_id,pool_offset);
        /*Check for HR*/
        BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.type.get(unit, core, pool_index, &pool_type));
        if (pool_type == dpp_res_cosq_type->dpp_res_type_cosq_se_cl_hr) {
            *region_type = BCM_DPP_AM_COSQ_REGION_TYPE_2HR;
        }
    }

exit:
    if (dpp_res_cosq_type != NULL)
       BCM_FREE(dpp_res_cosq_type);
    BCMDNX_FUNC_RETURN;
}


STATIC int 
_bcm_dpp_am_cosq_scheduler_allocate_param_validate(int unit,
                                                   int is_composite,
                                                   int is_dual,
                                                   int is_enhanced,
                                                   int is_non_contiguous,
                                                   bcm_dpp_am_cosq_region_type_t region_type,
                                                   SOC_TMC_AM_SCH_FLOW_TYPE flow_type)
{
    int is_region_type_2 = ((region_type == BCM_DPP_AM_COSQ_REGION_TYPE_2) || (region_type == BCM_DPP_AM_COSQ_REGION_TYPE_2HR)) ? TRUE : FALSE ;

    BCMDNX_INIT_FUNC_DEFS;

    
    if ((region_type == BCM_DPP_AM_COSQ_REGION_TYPE_0) && (flow_type != SOC_TMC_AM_SCH_FLOW_TYPE_CONNECTOR)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Only connector allocations are allowed in region 0")));
    }
    if ((region_type == BCM_DPP_AM_COSQ_REGION_TYPE_1) && (flow_type != SOC_TMC_AM_SCH_FLOW_TYPE_CONNECTOR) &&
         (flow_type != SOC_TMC_AM_SCH_FLOW_TYPE_CL) && (is_composite)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("HR and FQ composites can only be allocated in region 2")));
    }
    if (flow_type == SOC_TMC_AM_SCH_FLOW_TYPE_CONNECTOR) {
        if ((is_composite) && (is_region_type_2)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Composite connectors allocations are not allowed in region 2")));
        }
        if (is_dual) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Dual flag can't be specified for connectors")));
        }
        if (region_type == BCM_DPP_AM_COSQ_REGION_TYPE_0) {
            if (is_non_contiguous) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,(_BSL_BCM_MSG("Non contiguos flag raised while flow_id was in contiguos region")));
            }
        }
        else {
            if (! is_non_contiguous) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,(_BSL_BCM_MSG("Non contiguos flag was not raised while flow_id was in non contiguos region")));
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*Retrives a reference for the alloc_schemes array according to the given params*/
STATIC int 
_bcm_dpp_am_cosq_scheduler_alloc_scheme_idx_get(int unit,
                                               int is_composite,
                                               int is_dual,
                                               int is_enhanced,
                                               int region_type,
                                               int num_cos,
                                               SOC_TMC_AM_SCH_FLOW_TYPE flow_type,
                                               bcm_dpp_am_cosq_alloc_scheme_t *alloc_scheme,
                                               _bcm_dpp_am_cosq_allocate_func_state_t func_state)
{
    int is_allocate = (func_state ==  BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_ALLOCATE) ? TRUE : FALSE;

    BCMDNX_INIT_FUNC_DEFS;

    if ((func_state != BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_ALLOCATE) && (func_state != BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_DEALLOCATE)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("func_state to _bcm_dpp_am_cosq_scheduler_alloc_scheme_idx_get can be either allocate or deaalocate")));
    }
    
    switch (flow_type) {
    case SOC_TMC_AM_SCH_FLOW_TYPE_CONNECTOR : 
        switch (region_type) {
        case BCM_DPP_AM_COSQ_REGION_TYPE_0:
            *alloc_scheme = is_composite ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CONNECTOR_REGION_0 : BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_0;
            break;
        case BCM_DPP_AM_COSQ_REGION_TYPE_1:
            *alloc_scheme = is_composite ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CONNECTOR_REGION_1 : BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_1;
            break;
        case BCM_DPP_AM_COSQ_REGION_TYPE_2:
            if (is_allocate) {
                *alloc_scheme = (num_cos == 8) ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8 : BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4;
            } else {
                *alloc_scheme = (num_cos == 8) ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_8_DEALLOC : BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2FQ_NUM_COS_4_DEALLOC;
            }
            break;
        case BCM_DPP_AM_COSQ_REGION_TYPE_2HR:
            if (is_allocate) {
                *alloc_scheme = (num_cos == 8) ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8 : BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4;
            } else {
                *alloc_scheme = (num_cos == 8) ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_8_DEALLOC : BCM_DPP_AM_COSQ_ALLOC_SCHEME_CONNECTOR_REGION_2HR_NUM_COS_4_DEALLOC;
            }
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unhandled region type given for connector flow type")));
        }

        break;
    case SOC_TMC_AM_SCH_FLOW_TYPE_HR :
        if (is_allocate) {
            *alloc_scheme = is_composite ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2 : BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2;
            *alloc_scheme = is_dual ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2 : *alloc_scheme;
        } else {
            *alloc_scheme = is_composite ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_HR_REGION_2 : BCM_DPP_AM_COSQ_ALLOC_SCHEME_HR_REGION_2_DEALLOC;
            *alloc_scheme = is_dual ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_HR_REGION_2_DEALLOC : *alloc_scheme;
        }
        break;
    case SOC_TMC_AM_SCH_FLOW_TYPE_FQ :
        switch (region_type) {
        case BCM_DPP_AM_COSQ_REGION_TYPE_1:
            *alloc_scheme = is_dual ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_1 : BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_1;
            break;
        case BCM_DPP_AM_COSQ_REGION_TYPE_2:
            if (is_allocate) {
                *alloc_scheme = is_composite ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_FQ_REGION_2 : BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_2;
                *alloc_scheme = is_dual ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2 : *alloc_scheme;
            } else {
                *alloc_scheme = is_composite ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_FQ_REGION_2 : BCM_DPP_AM_COSQ_ALLOC_SCHEME_FQ_REGION_2_DEALLOC;
                *alloc_scheme = is_dual ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_FQ_REGION_2_DEALLOC : *alloc_scheme;
            }
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unhandled region type given for FQ flow type")));
        }
        break;
    case SOC_TMC_AM_SCH_FLOW_TYPE_CL :
        switch (region_type) {
        case BCM_DPP_AM_COSQ_REGION_TYPE_1:
            *alloc_scheme = is_composite ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_1 : BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_1;
            *alloc_scheme = is_dual ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_CL_REGION_1 : *alloc_scheme;
            *alloc_scheme = is_enhanced ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_ENHANCED_CL_REGION_1 : *alloc_scheme;
            break;
        case BCM_DPP_AM_COSQ_REGION_TYPE_2:
            if (is_allocate) {
                *alloc_scheme = is_composite ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_2FQ : BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2FQ;
                *alloc_scheme = is_dual ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_CL_REGION_2FQ : *alloc_scheme;
                *alloc_scheme = is_enhanced ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_ENHANCED_CL_REGION_2FQ : *alloc_scheme;
            } else {
                *alloc_scheme = is_composite ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_2FQ : BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2FQ_DEALLOC;
                *alloc_scheme = is_dual ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_CL_REGION_2FQ_DEALLOC : *alloc_scheme;
                *alloc_scheme = is_enhanced ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_ENHANCED_CL_REGION_2FQ_DEALLOC : *alloc_scheme;
            }
            break;
        case BCM_DPP_AM_COSQ_REGION_TYPE_2HR:
            if (is_allocate) {
                *alloc_scheme = is_composite ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_2HR : BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2HR;
                *alloc_scheme = is_dual ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_CL_REGION_2HR : *alloc_scheme;
                *alloc_scheme = is_enhanced ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_ENHANCED_CL_REGION_2HR : *alloc_scheme;
            } else {
                *alloc_scheme = is_composite ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_COMPOSITE_CL_REGION_2HR : BCM_DPP_AM_COSQ_ALLOC_SCHEME_CL_REGION_2HR_DEALLOC;
                *alloc_scheme = is_dual ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_DUAL_CL_REGION_2HR_DEALLOC : *alloc_scheme;
                *alloc_scheme = is_enhanced ? BCM_DPP_AM_COSQ_ALLOC_SCHEME_ENHANCED_CL_REGION_2HR_DEALLOC : *alloc_scheme;
            }
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unhandled region type given for CL flow type")));
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unhandled flow type given")));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*Retrieves all possible alloc schemes for without id allocation*/
STATIC int
   _bcm_dpp_am_cosq_scheduler_possible_alloc_schemes_idx_get(int unit,
                                                             int is_composite,
                                                             int is_dual,
                                                             int is_enhanced,
                                                             int is_non_contiguous,
                                                             int flow_type,
                                                             int num_cos,
                                                             bcm_dpp_am_cosq_alloc_scheme_t *alloc_scheme_arr,
                                                             int *alloc_scheme_arr_len,
                                                             _bcm_dpp_am_cosq_allocate_func_state_t func_state) {
    int rc;
    int i = 1;
    bcm_dpp_am_cosq_region_type_t region_type_start,
       region_type_end;

    BCMDNX_INIT_FUNC_DEFS;

    switch (flow_type) {
    case SOC_TMC_AM_SCH_FLOW_TYPE_CONNECTOR :
        if (!is_non_contiguous) {
            region_type_start = BCM_DPP_AM_COSQ_REGION_TYPE_0;
            region_type_end = BCM_DPP_AM_COSQ_REGION_TYPE_1;
        } else {
            region_type_start = BCM_DPP_AM_COSQ_REGION_TYPE_1;
            region_type_end = BCM_DPP_AM_COSQ_REGION_TYPE_LAST;
        }
        for (i = 0; (region_type_start < region_type_end) && (i < *alloc_scheme_arr_len);
             ++region_type_start, ++i, ++alloc_scheme_arr) {
            rc = _bcm_dpp_am_cosq_scheduler_alloc_scheme_idx_get(unit, is_composite, is_dual, is_enhanced, region_type_start, num_cos, flow_type, alloc_scheme_arr, BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_ALLOCATE);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        break;
    case SOC_TMC_AM_SCH_FLOW_TYPE_HR :
        region_type_start = BCM_DPP_AM_COSQ_REGION_TYPE_2HR;
        rc = _bcm_dpp_am_cosq_scheduler_alloc_scheme_idx_get(unit, is_composite, is_dual, is_enhanced, region_type_start, num_cos, flow_type, alloc_scheme_arr, BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_ALLOCATE);
        BCMDNX_IF_ERR_EXIT(rc);
        break;
    case SOC_TMC_AM_SCH_FLOW_TYPE_FQ :
        for (region_type_start = is_composite ? BCM_DPP_AM_COSQ_REGION_TYPE_2:BCM_DPP_AM_COSQ_REGION_TYPE_1, i = 0;
             (region_type_start < BCM_DPP_AM_COSQ_REGION_TYPE_2HR) && (i < *alloc_scheme_arr_len);
             ++region_type_start, ++i, ++alloc_scheme_arr) {
            rc = _bcm_dpp_am_cosq_scheduler_alloc_scheme_idx_get(unit, is_composite, is_dual, is_enhanced, region_type_start, num_cos, flow_type, alloc_scheme_arr, BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_ALLOCATE);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        break;
    default:
    case SOC_TMC_AM_SCH_FLOW_TYPE_CL :
        for (region_type_start = BCM_DPP_AM_COSQ_REGION_TYPE_1, i = 0;
             (region_type_start < BCM_DPP_AM_COSQ_REGION_TYPE_LAST) && (i < *alloc_scheme_arr_len);
             ++region_type_start, ++i, ++alloc_scheme_arr) {
            rc = _bcm_dpp_am_cosq_scheduler_alloc_scheme_idx_get(unit, is_composite, is_dual, is_enhanced, region_type_start, num_cos, flow_type, alloc_scheme_arr, BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_ALLOCATE);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        break;
    }
    *alloc_scheme_arr_len = i;
exit:
    BCMDNX_FUNC_RETURN;
}

/*Finds a valid free flow_id for without id allocation*/
STATIC int
   _bcm_dpp_am_cosq_scheduler_find_valid_flow_id(int unit,
                                                 int core,
                                                 uint32 nof_remote_cores,
                                                 bcm_dpp_am_cosq_alloc_scheme_t *alloc_scheme_arr,
                                                 int alloc_scheme_arr_len,
                                                 int nof_elements,
                                                 SOC_TMC_AM_SCH_FLOW_TYPE flow_type,
                                                 int *flow_id) {
    int rc = BCM_E_NONE;
    bcm_dpp_am_cosq_pool_ref_type_t res_pool_ref_type;
    int type_arr[1];
    int type_arr_len = 1;
    int i = 0;

    BCMDNX_INIT_FUNC_DEFS;

    /*Try to find requested scheme*/
    while (i < alloc_scheme_arr_len) {

        res_pool_ref_type =  _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][alloc_scheme_arr[i]].relevant_pool_indices_type;


        rc = _bcm_dpp_am_cosq_scheduler_without_id_allocate(unit,
                                                            core,
                                                            nof_remote_cores,
                                                            res_pool_ref_type,
                                                            &_bcm_dpp_am_cosq_per_type_flow_find_info_array[core][alloc_scheme_arr[i]].alloc_args,
                                                            nof_elements,
                                                            flow_type,
                                                            flow_id);
        if (rc == BCM_E_RESOURCE) {
            ++i;
            continue;
        }
        if (rc == BCM_E_NONE) {
            break;
        } else {
            BCMDNX_IF_ERR_EXIT(rc);
        }
    }
    /*In case no available flow_id was found exit*/
    if (rc == BCM_E_RESOURCE) {
        BCMDNX_IF_ERR_EXIT(rc);
    }
    /*Deallocate the element the find loop had allocated*/
    else {
        type_arr[0] = _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][alloc_scheme_arr[i]].pool_type;
        rc = _bcm_dpp_am_cosq_scheduler_with_id_a_mngr_action(unit,
                                                              core,
                                                              type_arr,
                                                              type_arr_len,
                                                              &_bcm_dpp_am_cosq_per_type_flow_find_info_array[core][alloc_scheme_arr[i]].alloc_args,
                                                              nof_elements,
                                                              _bcm_dpp_am_cosq_per_type_flow_find_info_array[core][alloc_scheme_arr[i]].pool_offset,
                                                              *flow_id,
                                                              BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_DEALLOCATE,
                                                              NULL,
                                                              NULL);
        if (rc != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed in deallocating found flow")));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
   _bcm_dpp_am_cosq_scheduler_execute_alloc_scheme(int unit,
                                                   int core,
                                                   bcm_dpp_am_cosq_alloc_scheme_t alloc_scheme_idx,
                                                   int nof_elements,
                                                   int flow_id,
                                                   _bcm_dpp_am_cosq_allocate_func_state_t func_state,
                                                   uint8 *src_modid) {
    int rc = BCM_E_NONE,
       rc_2 = BCM_E_NONE;
    int alloc_stage = -1;
    int type_arr[1];
    int type_arr_len = 1;
    int actual_func_state;
    int in_use = FALSE;
    BCMDNX_INIT_FUNC_DEFS;

    do {
        ++alloc_stage;
        /*Update func_state*/
        actual_func_state =
           (_bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][alloc_stage][alloc_scheme_idx].alloc_state == BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_NONE) ?
           func_state : _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][alloc_stage][alloc_scheme_idx].alloc_state;

        if (!_bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][alloc_stage][alloc_scheme_idx].is_tag) {
            src_modid = NULL;
        }

        /*Execute current allocation stage*/
        type_arr[0] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][alloc_stage][alloc_scheme_idx].pool_type;
        rc = _bcm_dpp_am_cosq_scheduler_with_id_a_mngr_action(unit,
                                                              core,
                                                              type_arr,
                                                              type_arr_len,
                                                              &_bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][alloc_stage][alloc_scheme_idx].alloc_args,
                                                              nof_elements,
                                                              _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][alloc_stage][alloc_scheme_idx].pool_offset,
                                                              flow_id,
                                                              actual_func_state,
                                                              &in_use,
                                                              src_modid);

        /*If a mandatory allocation has failed - break , deallocate, and exit*/
        if ((actual_func_state == BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_ALLOCATE) && (rc == BCM_E_RESOURCE)) {
            if (_bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][alloc_stage][alloc_scheme_idx].is_mandatory) {
                break;
            } else {
                rc = BCM_E_NONE;
            }
        }

        BCMDNX_IF_ERR_EXIT(rc);

        /*In case resource check was called - jump to next result dependent stage*/
        if (actual_func_state == BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_RESOURCE_CHECK) {
            if (in_use) {
                if (_BCM_DPP_AM_COSQ_RESOURCE_CHECK_BREAK == _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][alloc_stage][alloc_scheme_idx].res_check_if_in_use_nxt_ptr) {
                    break;
                } else {
                    alloc_stage += _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][alloc_stage][alloc_scheme_idx].res_check_if_in_use_nxt_ptr;
                }
            } else {
                if (_BCM_DPP_AM_COSQ_RESOURCE_CHECK_BREAK == _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][alloc_stage][alloc_scheme_idx].res_check_if_not_in_use_nxt_ptr) {
                    break;
                } else {
                    alloc_stage += _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][alloc_stage][alloc_scheme_idx].res_check_if_not_in_use_nxt_ptr;
                }
            }
        }

    }
    while (!_bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][alloc_stage][alloc_scheme_idx].is_last);

    /*Deallocating all allocations in case a mandatory allocation has failed (only for the allocating func_state))*/
    if ((rc == BCM_E_RESOURCE) &&
        (func_state == BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_ALLOCATE) &&
        (_bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][alloc_stage][alloc_scheme_idx].is_mandatory)) {
        while ((--alloc_stage) > -1) {
            type_arr[0] = _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][alloc_stage][alloc_scheme_idx].pool_type;
            rc_2 = _bcm_dpp_am_cosq_scheduler_with_id_a_mngr_action(unit,
                                                                    core,
                                                                    type_arr,
                                                                    type_arr_len,
                                                                    &_bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][alloc_stage][alloc_scheme_idx].alloc_args,
                                                                    nof_elements,
                                                                    _bcm_dpp_am_cosq_per_type_flow_allocation_scheme_array[core][alloc_stage][alloc_scheme_idx].pool_offset,
                                                                    flow_id,
                                                                    BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_DEALLOCATE,
                                                                    NULL,
                                                                    NULL);
            if (rc_2 != BCM_E_NONE) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed in allocating mandatory flow, failed in deallocating allocated flows")));
            }
        }
        BCMDNX_IF_ERR_EXIT(rc);
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_dpp_am_cosq_scheduler_allocate(int unit,
                                       int core,
                                       uint32 nof_remote_cores,
                                       uint32 flags,
                                       int is_composite,
                                       int is_enhanced,
                                       int is_dual,
                                       int is_non_contiguous,
                                       int num_cos,
                                       SOC_TMC_AM_SCH_FLOW_TYPE flow_type,
                                       uint8 *src_modid,
                                       int *flow_id) {
    int rc = BCM_E_NONE;
    int alloc_scheme_arr_len = BCM_DPP_AM_COSQ_SCH_MAX_ALLOCATION_REGIONS_TYPES_PER_FLOW_TYPE;
    bcm_dpp_am_cosq_alloc_scheme_t alloc_scheme_arr[BCM_DPP_AM_COSQ_SCH_MAX_ALLOCATION_REGIONS_TYPES_PER_FLOW_TYPE];
    bcm_dpp_am_cosq_alloc_scheme_t alloc_scheme_idx;
    bcm_dpp_am_cosq_region_type_t region_type;
    uint32                        region;

    BCMDNX_INIT_FUNC_DEFS;

    /*Preliminary parameter checking*/
    if (is_dual && is_composite) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("SE can't be both dual and composite")));
    }
    if ((is_enhanced) && (flow_type != SOC_TMC_AM_SCH_FLOW_TYPE_CL)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Enhanced flag can only be specified in cl case")));
    }
    /*If we are allocating without_id - first find an available flow_id*/
    if (!(SW_STATE_RES_ALLOC_WITH_ID & flags)) {
        /*Get relevant alloc schemes - i.e relevant pools for allocating*/
        rc = _bcm_dpp_am_cosq_scheduler_possible_alloc_schemes_idx_get(unit,
                                                                       is_composite,
                                                                       is_dual,
                                                                       is_enhanced,
                                                                       is_non_contiguous,
                                                                       flow_type,
                                                                       num_cos,
                                                                       alloc_scheme_arr,
                                                                       &alloc_scheme_arr_len,
                                                                       BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_ALLOCATE);
        BCMDNX_IF_ERR_EXIT(rc);
        /*Search the relevant pools for available flow_id*/
        rc = _bcm_dpp_am_cosq_scheduler_find_valid_flow_id(unit,
                                                           core,
                                                           nof_remote_cores,
                                                           alloc_scheme_arr,
                                                           alloc_scheme_arr_len,
                                                           num_cos,
                                                           flow_type,
                                                           flow_id);
        BCMDNX_IF_ERR_EXIT(rc);

    } else { /* SW_STATE_RES_ALLOC_WITH_ID */
        if (flow_type == SOC_TMC_AM_SCH_FLOW_TYPE_CONNECTOR) {
            region =  _BCM_DPP_AM_COSQ_GET_REGION_INDEX_FROM_FLOW_INDEX(*flow_id);
            if (region > SOC_TMC_COSQ_TOTAL_FLOW_REGIONS) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid region %d"), region));
            }
            if (nof_remote_cores != SOC_DPP_CONFIG(unit)->arad->region_nof_remote_cores[core][region]) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Requested region[%d] (remote cores %d) doesn't support requested number of remote cores = %d"), region, SOC_DPP_CONFIG(unit)->arad->region_nof_remote_cores[core][region], nof_remote_cores));
            }
        }
    }

    /*Get region type*/
    rc = _bcm_dpp_am_cosq_flow_id_to_region_type_get(unit, core, *flow_id, &region_type);
    BCMDNX_IF_ERR_EXIT(rc);
    /*Validate parameters*/
    rc = _bcm_dpp_am_cosq_scheduler_allocate_param_validate(unit, is_composite, is_dual, is_enhanced, is_non_contiguous, region_type, flow_type);
    BCMDNX_IF_ERR_EXIT(rc);
    /*Get the idx to the correct alloc scheme*/
    rc = _bcm_dpp_am_cosq_scheduler_alloc_scheme_idx_get(unit, is_composite, is_dual, is_enhanced, region_type, num_cos, flow_type, &alloc_scheme_idx, BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_ALLOCATE);
    BCMDNX_IF_ERR_EXIT(rc);
    /*Execute alloc scheme*/
    rc = _bcm_dpp_am_cosq_scheduler_execute_alloc_scheme(unit, core, alloc_scheme_idx, num_cos, *flow_id, BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_ALLOCATE, src_modid);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_dpp_am_cosq_scheduler_deallocate(int unit,
                                         int core,
                                         uint32 flags,
                                         int is_composite,
                                         int is_enhanced,
                                         int is_dual,
                                         int is_non_contiguous,
                                         int num_cos,
                                         SOC_TMC_AM_SCH_FLOW_TYPE flow_type,
                                         int flow_id) {
    int rc = BCM_E_NONE;
    bcm_dpp_am_cosq_alloc_scheme_t alloc_scheme_idx;
    bcm_dpp_am_cosq_region_type_t region_type;

    BCMDNX_INIT_FUNC_DEFS;

    /*Preliminary parameter checking*/
    if (is_dual && is_composite) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("SE can't be both dual and composite")));
    }
    if ((is_enhanced) && (flow_type != SOC_TMC_AM_SCH_FLOW_TYPE_CL)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Enhanced flag can only be specified in cl case")));
    }

    /*Get region type*/
    rc = _bcm_dpp_am_cosq_flow_id_to_region_type_get(unit, core, flow_id, &region_type);
    BCMDNX_IF_ERR_EXIT(rc);
    /*Validate parameters*/
    rc = _bcm_dpp_am_cosq_scheduler_allocate_param_validate(unit, is_composite, is_dual, is_enhanced, is_non_contiguous, region_type, flow_type);
    BCMDNX_IF_ERR_EXIT(rc);
    /*Get the idx to the correct alloc scheme*/
    rc = _bcm_dpp_am_cosq_scheduler_alloc_scheme_idx_get(unit, is_composite, is_dual, is_enhanced, region_type, num_cos, flow_type, &alloc_scheme_idx, BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_DEALLOCATE);
    BCMDNX_IF_ERR_EXIT(rc);
    /*Execute alloc scheme*/
    rc = _bcm_dpp_am_cosq_scheduler_execute_alloc_scheme(unit, core, alloc_scheme_idx, num_cos, flow_id, BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_DEALLOCATE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

/*Diag functions*/
int
bcm_dpp_am_cosq_per_type_all_allocated_resource_find_get_look_up_info(int unit,
                                                                      int core,
                                                                      SOC_TMC_AM_SCH_FLOW_TYPE flow_type,
                                                                      int *nof_find_stages,
                                                                      _bcm_dpp_am_cosq_per_type_all_allocated_resource_find_t *find_look_up_info) {
    int rc = BCM_E_NONE;
    _dpp_res_type_cosq_t *dpp_res_cosq_type = NULL;

    BCMDNX_INIT_FUNC_DEFS;

    /*Get type struct*/
    BCMDNX_ALLOC(dpp_res_cosq_type, (sizeof(_dpp_res_type_cosq_t)), "res cos type");
    if (dpp_res_cosq_type == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    rc = _bcm_dpp_resources_fill_type_cosq(unit, core, dpp_res_cosq_type);
    BCMDNX_IF_ERR_EXIT(rc);

    /*Fill the other type dependent fields*/
    switch (flow_type) {
    case SOC_TMC_AM_SCH_FLOW_TYPE_HR :
        find_look_up_info[0].flow_offset = 1;
        find_look_up_info[0].elem_offset = 1;
        find_look_up_info[0].flow_incr = _bcm_dpp_am_cosq_flow_incrment_plus_4;
        find_look_up_info[0].elem_incr = 2;
        find_look_up_info[0].region_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_hr;
        find_look_up_info[0].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SE_OFFSET;
        find_look_up_info[0].alloc_args.align = 2;
        find_look_up_info[0].alloc_args.offset = 1;
        find_look_up_info[0].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
        find_look_up_info[0].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_se_flow_id_to_512_bitmap_element;
        *nof_find_stages = 1;
        break;
    case SOC_TMC_AM_SCH_FLOW_TYPE_FQ :
        find_look_up_info[0].flow_offset = 1;
        find_look_up_info[0].elem_offset = 1;
        find_look_up_info[0].flow_incr = _bcm_dpp_am_cosq_flow_incrment_plus_4;
        find_look_up_info[0].elem_incr = 2;
        find_look_up_info[0].region_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_1;
        find_look_up_info[0].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SE_OFFSET;
        find_look_up_info[0].alloc_args.align = 2;
        find_look_up_info[0].alloc_args.offset = 1;
        find_look_up_info[0].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
        find_look_up_info[0].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_se_flow_id_to_512_bitmap_element;
        find_look_up_info[1].flow_offset = 1;
        find_look_up_info[1].elem_offset = 1;
        find_look_up_info[1].flow_incr = _bcm_dpp_am_cosq_flow_incrment_plus_4;
        find_look_up_info[1].elem_incr = 2;
        find_look_up_info[1].region_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_2;
        find_look_up_info[1].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SE_OFFSET;
        find_look_up_info[1].alloc_args.align = 2;
        find_look_up_info[1].alloc_args.offset = 1;
        find_look_up_info[1].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
        find_look_up_info[1].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_se_flow_id_to_512_bitmap_element;
        *nof_find_stages = 2;
        break;
    case SOC_TMC_AM_SCH_FLOW_TYPE_CL :
        find_look_up_info[0].flow_offset = 0;
        find_look_up_info[0].elem_offset = 0;
        find_look_up_info[0].flow_incr = _bcm_dpp_am_cosq_flow_incrment_plus_4;
        find_look_up_info[0].elem_incr = 2;
        find_look_up_info[0].region_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_1;
        find_look_up_info[0].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SE_OFFSET;
        find_look_up_info[0].alloc_args.align = 2;
        find_look_up_info[0].alloc_args.offset = 0;
        find_look_up_info[0].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
        find_look_up_info[0].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_se_flow_id_to_512_bitmap_element;
        find_look_up_info[1].flow_offset = 0;
        find_look_up_info[1].elem_offset = 0;
        find_look_up_info[1].flow_incr = _bcm_dpp_am_cosq_flow_incrment_plus_4;
        find_look_up_info[1].elem_incr = 2;
        find_look_up_info[1].region_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_2;
        find_look_up_info[1].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SE_OFFSET;
        find_look_up_info[1].alloc_args.align = 2;
        find_look_up_info[1].alloc_args.offset = 0;
        find_look_up_info[1].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
        find_look_up_info[1].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_se_flow_id_to_512_bitmap_element;
        find_look_up_info[2].flow_offset = 0;
        find_look_up_info[2].elem_offset = 0;
        find_look_up_info[2].flow_incr = _bcm_dpp_am_cosq_flow_incrment_plus_4;
        find_look_up_info[2].elem_incr = 2;
        find_look_up_info[2].region_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_hr;
        find_look_up_info[2].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SE_OFFSET;
        find_look_up_info[2].alloc_args.align = 2;
        find_look_up_info[2].alloc_args.offset = 0;
        find_look_up_info[2].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
        find_look_up_info[2].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_se_flow_id_to_512_bitmap_element;
        *nof_find_stages = 3;
        break;
    case SOC_TMC_AM_SCH_FLOW_TYPE_HR_COMPOSITE :
        find_look_up_info[0].flow_offset = 1;
        find_look_up_info[0].elem_offset = 1;
        find_look_up_info[0].flow_incr = _bcm_dpp_am_cosq_flow_incrment_plus_4;
        find_look_up_info[0].elem_incr = 2;
        find_look_up_info[0].region_type = dpp_res_cosq_type->dpp_res_type_cosq_hr_connector_region_type_2_sync;
        find_look_up_info[0].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SYNC_OFFSET;
        find_look_up_info[0].alloc_args.align = 2;
        find_look_up_info[0].alloc_args.offset = 1;
        find_look_up_info[0].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
        find_look_up_info[0].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_se_flow_id_to_512_bitmap_element;
        *nof_find_stages = 1;
        break;
    case SOC_TMC_AM_SCH_FLOW_TYPE_FQ_COMPOSITE :
        find_look_up_info[0].flow_offset = 1;
        find_look_up_info[0].elem_offset = 1;
        find_look_up_info[0].flow_incr = _bcm_dpp_am_cosq_flow_incrment_plus_4;
        find_look_up_info[0].elem_incr = 2;
        find_look_up_info[0].region_type = dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync;
        find_look_up_info[0].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SYNC_OFFSET;
        find_look_up_info[0].alloc_args.align = 2;
        find_look_up_info[0].alloc_args.offset = 1;
        find_look_up_info[0].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
        find_look_up_info[0].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_se_flow_id_to_512_bitmap_element;
        *nof_find_stages = 1;
        break;
    case SOC_TMC_AM_SCH_FLOW_TYPE_CL_COMPOSITE :
        find_look_up_info[0].flow_offset = 0;
        find_look_up_info[0].elem_offset = 0;
        find_look_up_info[0].flow_incr = _bcm_dpp_am_cosq_flow_incrment_plus_4;
        find_look_up_info[0].elem_incr = 2;
        find_look_up_info[0].region_type = dpp_res_cosq_type->dpp_res_type_cosq_fq_connector_region_type_2_sync;
        find_look_up_info[0].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SYNC_OFFSET;
        find_look_up_info[0].alloc_args.align = 2;
        find_look_up_info[0].alloc_args.offset = 0;
        find_look_up_info[0].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
        find_look_up_info[0].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_se_flow_id_to_512_bitmap_element;
        find_look_up_info[1].flow_offset = 0;
        find_look_up_info[1].elem_offset = 0;
        find_look_up_info[1].flow_incr = _bcm_dpp_am_cosq_flow_incrment_plus_4;
        find_look_up_info[1].elem_incr = 2;
        find_look_up_info[1].region_type = dpp_res_cosq_type->dpp_res_type_cosq_hr_connector_region_type_2_sync;
        find_look_up_info[1].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SYNC_OFFSET;
        find_look_up_info[1].alloc_args.align = 2;
        find_look_up_info[1].alloc_args.offset = 0;
        find_look_up_info[1].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
        find_look_up_info[1].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_se_flow_id_to_512_bitmap_element;
        *nof_find_stages = 2;
        break;
    case SOC_TMC_AM_SCH_FLOW_TYPE_CONNECTOR :
        find_look_up_info[0].flow_offset = 0;
        find_look_up_info[0].elem_offset = 0;
        find_look_up_info[0].flow_incr = _bcm_dpp_am_cosq_flow_incrment_plus_4;
        find_look_up_info[0].elem_incr = 1;
        find_look_up_info[0].region_type = dpp_res_cosq_type->dpp_res_type_cosq_connector_cont;
        find_look_up_info[0].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_CONNECTOR_OFFSET;
        find_look_up_info[0].alloc_args.align = 1;
        find_look_up_info[0].alloc_args.offset = 0;
        find_look_up_info[0].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
        find_look_up_info[0].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_256_bitmap_element;
        find_look_up_info[1].flow_offset = 0;
        find_look_up_info[1].elem_offset = 0;
        find_look_up_info[1].flow_incr = _bcm_dpp_am_cosq_flow_incrment_connector_non_contiguous;
        find_look_up_info[1].elem_incr = 1;
        find_look_up_info[1].region_type = dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_1;
        find_look_up_info[1].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_CONNECTOR_OFFSET;
        find_look_up_info[1].alloc_args.align = 1;
        find_look_up_info[1].alloc_args.offset = 0;
        find_look_up_info[1].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
        find_look_up_info[1].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element;
        find_look_up_info[2].flow_offset = 0;
        find_look_up_info[2].elem_offset = 0;
        find_look_up_info[2].flow_incr = _bcm_dpp_am_cosq_flow_incrment_connector_non_contiguous;
        find_look_up_info[2].elem_incr = 1;
        find_look_up_info[2].region_type = dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_2;
        find_look_up_info[2].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_CONNECTOR_OFFSET;
        find_look_up_info[2].alloc_args.align = 1;
        find_look_up_info[2].alloc_args.offset = 0;
        find_look_up_info[2].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
        find_look_up_info[2].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element;
        *nof_find_stages = 3;
        break;
    default:
        rc = BCM_E_PARAM;
        BCMDNX_IF_ERR_EXIT(rc);
    }
exit:
    BCM_FREE(dpp_res_cosq_type);
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_cosq_fetch_allocated_resources(int unit,
                                          int core,
                                          SOC_TMC_AM_SCH_FLOW_TYPE flow_type,
                                          bcm_dpp_am_cosq_pool_ref_t *allocated_ref) {
    int rc = BCM_E_NONE;
    int pool_index;
    int pool_arr_max_len = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_PER_REGION_MAX_ENTRIES;
    int is_allocated;
    int pool_elem = 0;
    int type_arr[1], type_arr_len = 1;
    int stage_iter = -1;
    int flow_id;
    int nof_find_stages = _BCM_DPP_AM_COSQ_PER_TYPE_ALL_ALLOCATED_RESOURCE_FIND_MAX_STAGES;
    _bcm_dpp_am_cosq_per_type_all_allocated_resource_find_t find_look_up_info[_BCM_DPP_AM_COSQ_PER_TYPE_ALL_ALLOCATED_RESOURCE_FIND_MAX_STAGES];
    int pool_type;
    int pool_is_valid;
    int pool_start;
    int pool_count;

    BCMDNX_INIT_FUNC_DEFS;

    /*Parameter checking*/
    BCMDNX_NULL_CHECK(allocated_ref);

    if (flow_type >= SOC_TMC_AM_SCH_FLOW_TYPE_LAST) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid flow type")));
    }

    /*Get all relevant look-up infos*/
    rc = bcm_dpp_am_cosq_per_type_all_allocated_resource_find_get_look_up_info(unit, core, flow_type, &nof_find_stages, find_look_up_info);
    BCMDNX_IF_ERR_EXIT(rc);

    /*Zero...*/
    allocated_ref->max_entries = 0;
    allocated_ref->valid_entries = 0;
    /*Iterate over all relavent pool types*/
    while (++stage_iter < nof_find_stages) {
        /*pool_index depends on the pool_offset - either conn, se or sync pool*/
        pool_index = _BCM_DPP_AM_COSQ_GET_POOL_INDEX(0, find_look_up_info[stage_iter].pool_offset);
        type_arr[0] = find_look_up_info[stage_iter].region_type;
        /*Iterate over all pools - find only the specified type*/
        while (pool_index < pool_arr_max_len) {
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.is_valid.get(unit, core, pool_index, &pool_is_valid));
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.type.get(unit, core, pool_index, &pool_type));
            if (!pool_is_valid || (pool_type != type_arr[0])) {
                pool_index += BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_PER_REGION;
                continue;
            }
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.start.get(unit, core, pool_index, &pool_start));
            flow_id = pool_start + find_look_up_info[stage_iter].flow_offset;
            pool_elem = find_look_up_info[stage_iter].elem_offset;
            /*For every RELEVANT pool_elem check allocation*/
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.count.get(unit, core, pool_index, &pool_count));
            while (pool_elem < pool_count) {
                _bcm_dpp_am_cosq_scheduler_with_id_a_mngr_action(unit,
                                                                 core,
                                                                 type_arr,
                                                                 type_arr_len,
                                                                 &find_look_up_info[stage_iter].alloc_args,
                                                                 1 /*nof_elements*/,
                                                                 find_look_up_info[stage_iter].pool_offset,
                                                                 flow_id,
                                                                 BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_RESOURCE_CHECK,
                                                                 &is_allocated,
                                                                 NULL /*tag*/);
                BCMDNX_IF_ERR_EXIT(rc);
                if (is_allocated) {
                    allocated_ref->entries[allocated_ref->max_entries] = flow_id;
                    allocated_ref->max_entries += 1;
                    allocated_ref->valid_entries += 1;
                }
                pool_elem += find_look_up_info[stage_iter].elem_incr;
                flow_id = find_look_up_info[stage_iter].flow_incr(flow_id);
            }
            pool_index += BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_PER_REGION;
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_am_cosq_quartet_look_up_info_per_region_type_get(int unit,
                                                          int core,
                                                          int region_type, 
                                                       int *quartet_look_up_info_len,
                                                       _bcm_dpp_am_cosq_allocation_info_t *quartet_look_up_info)
{
    int rc = BCM_E_NONE;
    _dpp_res_type_cosq_t *dpp_res_cosq_type = NULL;

    BCMDNX_INIT_FUNC_DEFS;
    
    /*Parameter checking*/
    BCMDNX_NULL_CHECK(quartet_look_up_info_len);
    BCMDNX_NULL_CHECK(quartet_look_up_info);

    /*Get type struct*/
    BCMDNX_ALLOC(dpp_res_cosq_type, (sizeof(_dpp_res_type_cosq_t)), "res cos type");
    if (dpp_res_cosq_type == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    rc = _bcm_dpp_resources_fill_type_cosq(unit, core,dpp_res_cosq_type);
    BCMDNX_IF_ERR_EXIT(rc);

    
    switch (region_type) {
    case DPP_DEVICE_COSQ_REGION_TYPE0:
        quartet_look_up_info[0].alloc_args.align = 1;
        quartet_look_up_info[0].alloc_args.offset = 0;
        quartet_look_up_info[0].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
        quartet_look_up_info[0].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_256_bitmap_element;
        quartet_look_up_info[0].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_connector_cont;
        quartet_look_up_info[0].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_CONNECTOR_OFFSET;
        *quartet_look_up_info_len = 1;
        break;
    case DPP_DEVICE_COSQ_REGION_TYPE1:
    case DPP_DEVICE_COSQ_REGION_TYPE5:
        quartet_look_up_info[0].alloc_args.align = 1;
        quartet_look_up_info[0].alloc_args.offset = 0;
        quartet_look_up_info[0].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
        quartet_look_up_info[0].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_se_flow_id_to_512_bitmap_element;
        quartet_look_up_info[0].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_1;
        quartet_look_up_info[0].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SE_OFFSET;

        quartet_look_up_info[1] = quartet_look_up_info[0];

        quartet_look_up_info[2].alloc_args.align = 1;
        quartet_look_up_info[2].alloc_args.offset = 0;
        quartet_look_up_info[2].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
        quartet_look_up_info[2].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element;
        quartet_look_up_info[2].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_1;
        quartet_look_up_info[2].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_CONNECTOR_OFFSET;

        quartet_look_up_info[3] = quartet_look_up_info[2];
        *quartet_look_up_info_len = 4;
        break;
    case DPP_DEVICE_COSQ_REGION_TYPE2:
    case DPP_DEVICE_COSQ_REGION_TYPE6:
        quartet_look_up_info[0].alloc_args.align = 1;
        quartet_look_up_info[0].alloc_args.offset = 0;
        quartet_look_up_info[0].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
        quartet_look_up_info[0].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_se_flow_id_to_512_bitmap_element;
        quartet_look_up_info[0].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_fq_region_type_2;
        quartet_look_up_info[0].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SE_OFFSET;

        quartet_look_up_info[1] = quartet_look_up_info[0];

        quartet_look_up_info[2].alloc_args.align = 1;
        quartet_look_up_info[2].alloc_args.offset = 0;
        quartet_look_up_info[2].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
        quartet_look_up_info[2].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element;
        quartet_look_up_info[2].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_2;
        quartet_look_up_info[2].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_CONNECTOR_OFFSET;

        quartet_look_up_info[3] = quartet_look_up_info[2];
        *quartet_look_up_info_len = 4;
        break;
    case DPP_DEVICE_COSQ_REGION_TYPE3: /*Special hr region*/
        quartet_look_up_info[0].alloc_args.align = 1;
        quartet_look_up_info[0].alloc_args.offset = 0;
        quartet_look_up_info[0].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
        quartet_look_up_info[0].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_se_flow_id_to_512_bitmap_element;
        quartet_look_up_info[0].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_se_cl_hr;
        quartet_look_up_info[0].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SE_OFFSET;

        quartet_look_up_info[1] = quartet_look_up_info[0];

        quartet_look_up_info[2].alloc_args.align = 1;
        quartet_look_up_info[2].alloc_args.offset = 0;
        quartet_look_up_info[2].alloc_args.calc_count = _bcm_dpp_am_cosq_calc_count_identity;
        quartet_look_up_info[2].alloc_args.flow_id_to_bitmap_elem = _bcm_dpp_am_cosq_connector_flow_id_to_512_bitmap_element;
        quartet_look_up_info[2].pool_type = dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_2;
        quartet_look_up_info[2].pool_offset = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_CONNECTOR_OFFSET;

        quartet_look_up_info[3] = quartet_look_up_info[2];
        *quartet_look_up_info_len = 4;
        break;
    default:
         BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unknown region type")));
    }

exit:
    BCM_FREE(dpp_res_cosq_type);
    BCMDNX_FUNC_RETURN;
}



int 
bcm_dpp_am_cosq_fetch_quad(int unit,
                           int core,
                           int flow_id,
                           int region_type,
                           bcm_dpp_am_cosq_quad_allocation_t *is_allocated)
{
    int rc = BCM_E_NONE;
    int i = -1;
    int quartet_start_flow_id = (flow_id/_BCM_DPP_AM_COSQ_QUARTET_LEN)*_BCM_DPP_AM_COSQ_QUARTET_LEN;
    int type_arr[1], type_arr_len = 1;
    int quartet_look_up_info_len = _BCM_DPP_AM_COSQ_QUARTET_LEN;
    _bcm_dpp_am_cosq_allocation_info_t quartet_look_up_info[_BCM_DPP_AM_COSQ_QUARTET_LEN];

    BCMDNX_INIT_FUNC_DEFS; 

    /*Parameter checking*/
    BCMDNX_NULL_CHECK(is_allocated);

    if (region_type >= DPP_DEVICE_COSQ_REGION_TYPE_LAST) {
         BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid regiion type")));
    }

    if ((flow_id/1024) > DPP_DEVICE_COSQ_TOTAL_FLOW_REGIONS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid flow_id")));
    }

    sal_memset(quartet_look_up_info, 0, sizeof(_bcm_dpp_am_cosq_allocation_info_t)*_BCM_DPP_AM_COSQ_QUARTET_LEN);
    /*Get look up info*/
    rc = _bcm_dpp_am_cosq_quartet_look_up_info_per_region_type_get(unit,core,region_type, &quartet_look_up_info_len, quartet_look_up_info);
    BCMDNX_IF_ERR_EXIT(rc);

    /*Check allocation*/
    while (++i < quartet_look_up_info_len) {
        type_arr[0] = quartet_look_up_info[i].pool_type;
        _bcm_dpp_am_cosq_scheduler_with_id_a_mngr_action(unit,
                                                    core,
                                                    type_arr,
                                                    type_arr_len,
                                                    &quartet_look_up_info[i].alloc_args,
                                                    1 /*nof_elements*/,
                                                    quartet_look_up_info[i].pool_offset,
                                                    quartet_start_flow_id,
                                                    BCM_DPP_AM_COSQ_ALLOCATE_FUNC_STATE_RESOURCE_CHECK,
                                                    ((int*)is_allocated + i),
                                                    NULL /*tag*/);
        ++quartet_start_flow_id;
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_dpp_am_cosq_get_region_type(int unit,
                           int core,
                           int flow_id,
                           int *region_type)
{
    int rc = BCM_E_NONE;
    int pool_arr_max_len = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_PER_REGION_MAX_ENTRIES - 1;
    int pool_index = _BCM_DPP_AM_COSQ_GET_POOL_INDEX(0,BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_CONNECTOR_OFFSET);
    int pool_index_incr = BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_PER_REGION;
    _dpp_res_type_cosq_t *dpp_res_cosq_type = NULL;
    int pool_type;
    int pool_start, pool_inc_start;

    BCMDNX_INIT_FUNC_DEFS; 

    
    /*Parameter checking*/
    BCMDNX_NULL_CHECK(region_type);

    if (SOC_TMC_SCH_FLOW_TO_1K_ID(flow_id) >= DPP_DEVICE_COSQ_TOTAL_FLOW_REGIONS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid flow_id")));
    }

    /*Get type struct*/
    BCMDNX_ALLOC(dpp_res_cosq_type, (sizeof(_dpp_res_type_cosq_t)), "res cos type");
    if (dpp_res_cosq_type == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    rc = _bcm_dpp_resources_fill_type_cosq(unit, core,dpp_res_cosq_type);
    BCMDNX_IF_ERR_EXIT(rc);

    /*Iterate over all regions and find region type*/
    while (pool_index < pool_arr_max_len) {
        BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.start.get(unit, core, pool_index, &pool_start));
        BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.start.get(unit, core, pool_index + pool_index_incr, &pool_inc_start));
        if(pool_start <= flow_id && pool_inc_start > flow_id)
        {
            break;
        }
        pool_index += pool_index_incr;
    }
    
    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.e2e_total_res_pool.entries.type.get(unit, core, pool_index, &pool_type));
    if(pool_type == dpp_res_cosq_type->dpp_res_type_cosq_connector_cont) {
        *region_type = DPP_DEVICE_COSQ_REGION_TYPE0;
    } else if (pool_type == dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_1) {
        *region_type = DPP_DEVICE_COSQ_REGION_TYPE1;
    } else if (pool_type == dpp_res_cosq_type->dpp_res_type_cosq_connector_non_cont_region_type_2) {
        /*last region is resrved for HR (type == 3)*/
        *region_type = ((flow_id / 1024) >= 127) ? DPP_DEVICE_COSQ_REGION_TYPE3 : DPP_DEVICE_COSQ_REGION_TYPE2;
    } else {
        rc = BCM_E_PARAM;
        BCMDNX_IF_ERR_EXIT(rc);
    }

exit:
    BCM_FREE(dpp_res_cosq_type);
    BCMDNX_FUNC_RETURN;
}

int sw_state_bcm_dpp_am_cosq_pool_ref_t_get (int unit, int core, bcm_dpp_am_cosq_pool_ref_type_t type, bcm_dpp_am_cosq_pool_ref_field_t field, int idx, int *result){

    BCMDNX_INIT_FUNC_DEFS; 

    if (core < 0 || core >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Core %d id out of range"),core));
    }

    switch (type) {
        case connector_cont_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_cont_res_pool_ref.max_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_cont_res_pool_ref.valid_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_cont_res_pool_ref.entries.get(unit, core, idx, result));
                    BCM_EXIT;
                }
            }
        }
        case connector_non_cont_region_type_1_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_non_cont_region_type_1_res_pool_ref.max_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_non_cont_region_type_1_res_pool_ref.valid_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_non_cont_region_type_1_res_pool_ref.entries.get(unit, core, idx, result));
                    BCM_EXIT;
                }
            }
        }
        case connector_non_cont_region_type_2_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_non_cont_region_type_2_res_pool_ref.max_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_non_cont_region_type_2_res_pool_ref.valid_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_non_cont_region_type_2_res_pool_ref.entries.get(unit, core, idx, result));
                    BCM_EXIT;
                }
            }
        }
        case se_cl_fq_region_type_1_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_fq_region_type_1_res_pool_ref.max_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_fq_region_type_1_res_pool_ref.valid_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_fq_region_type_1_res_pool_ref.entries.get(unit, core, idx, result));
                    BCM_EXIT;
                }
            }
        }
        case se_cl_fq_region_type_2_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_fq_region_type_2_res_pool_ref.max_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_fq_region_type_2_res_pool_ref.valid_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_fq_region_type_2_res_pool_ref.entries.get(unit, core, idx, result));
                    BCM_EXIT;
                }
            }
        }
        case se_cl_hr_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_hr_res_pool_ref.max_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_hr_res_pool_ref.valid_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_hr_res_pool_ref.entries.get(unit, core, idx, result));
                    BCM_EXIT;
                }
            }
        }
        case fq_connector_region_type_2_sync_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.fq_connector_region_type_2_sync_res_pool_ref.max_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.fq_connector_region_type_2_sync_res_pool_ref.valid_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.fq_connector_region_type_2_sync_res_pool_ref.entries.get(unit, core, idx, result));
                    BCM_EXIT;
                }
            }
        }
        case hr_connector_region_type_2_sync_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.hr_connector_region_type_2_sync_res_pool_ref.max_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.hr_connector_region_type_2_sync_res_pool_ref.valid_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.hr_connector_region_type_2_sync_res_pool_ref.entries.get(unit, core, idx, result));
                    BCM_EXIT;
                }
            }
        }
        case queue_unicast_cont_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_cont_res_pool_ref.max_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_cont_res_pool_ref.valid_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_cont_res_pool_ref.entries.get(unit, core, idx, result));
                    BCM_EXIT;
                }
            }
        }
        case queue_unicast_non_cont_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_non_cont_res_pool_ref.max_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_non_cont_res_pool_ref.valid_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_non_cont_res_pool_ref.entries.get(unit, core, idx, result));
                    BCM_EXIT;
                }
            }
        }
        case queue_multicast_cont_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_cont_res_pool_ref.max_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_cont_res_pool_ref.valid_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_cont_res_pool_ref.entries.get(unit, core, idx, result));
                    BCM_EXIT;
                }
            }
        }
        case queue_multicast_non_cont_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_non_cont_res_pool_ref.max_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_non_cont_res_pool_ref.valid_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_non_cont_res_pool_ref.entries.get(unit, core, idx, result));
                    BCM_EXIT;
                }
            }
        }
        case queue_isq_cont_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_cont_res_pool_ref.max_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_cont_res_pool_ref.valid_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_cont_res_pool_ref.entries.get(unit, core, idx, result));
                    BCM_EXIT;
                }
            }
        }
        case queue_isq_non_cont_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_non_cont_res_pool_ref.max_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_non_cont_res_pool_ref.valid_entries.get(unit, core, result));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_non_cont_res_pool_ref.entries.get(unit, core, idx, result));
                    BCM_EXIT;
                }
            }
        }
        default:{
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid type")));
        }

    }
exit:
    BCMDNX_FUNC_RETURN;
}

int sw_state_bcm_dpp_am_cosq_pool_ref_t_set (int unit, int core, bcm_dpp_am_cosq_pool_ref_type_t type, bcm_dpp_am_cosq_pool_ref_field_t field, int idx, int value){

    BCMDNX_INIT_FUNC_DEFS; 

    if (core < 0 || core >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Core %d id out of range"),core));
    }

    switch (type) {
        case connector_cont_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_cont_res_pool_ref.max_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_cont_res_pool_ref.valid_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_cont_res_pool_ref.entries.set(unit, core, idx, value));
                    BCM_EXIT;
                }
            }
        }
        case connector_non_cont_region_type_1_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_non_cont_region_type_1_res_pool_ref.max_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_non_cont_region_type_1_res_pool_ref.valid_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_non_cont_region_type_1_res_pool_ref.entries.set(unit, core, idx, value));
                    BCM_EXIT;
                }
            }
        }
        case connector_non_cont_region_type_2_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_non_cont_region_type_2_res_pool_ref.max_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_non_cont_region_type_2_res_pool_ref.valid_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.connector_non_cont_region_type_2_res_pool_ref.entries.set(unit, core, idx, value));
                    BCM_EXIT;
                }
            }
        }
        case se_cl_fq_region_type_1_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_fq_region_type_1_res_pool_ref.max_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_fq_region_type_1_res_pool_ref.valid_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_fq_region_type_1_res_pool_ref.entries.set(unit, core, idx, value));
                    BCM_EXIT;
                }
            }
        }
        case se_cl_fq_region_type_2_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_fq_region_type_2_res_pool_ref.max_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_fq_region_type_2_res_pool_ref.valid_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_fq_region_type_2_res_pool_ref.entries.set(unit, core, idx, value));
                    BCM_EXIT;
                }
            }
        }
        case se_cl_hr_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_hr_res_pool_ref.max_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_hr_res_pool_ref.valid_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.se_cl_hr_res_pool_ref.entries.set(unit, core, idx, value));
                    BCM_EXIT;
                }
            }
        }
        case fq_connector_region_type_2_sync_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.fq_connector_region_type_2_sync_res_pool_ref.max_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.fq_connector_region_type_2_sync_res_pool_ref.valid_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.fq_connector_region_type_2_sync_res_pool_ref.entries.set(unit, core, idx, value));
                    BCM_EXIT;
                }
            }
        }
        case hr_connector_region_type_2_sync_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.hr_connector_region_type_2_sync_res_pool_ref.max_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.hr_connector_region_type_2_sync_res_pool_ref.valid_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.hr_connector_region_type_2_sync_res_pool_ref.entries.set(unit, core, idx, value));
                    BCM_EXIT;
                }
            }
        }
        case queue_unicast_cont_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_cont_res_pool_ref.max_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_cont_res_pool_ref.valid_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_cont_res_pool_ref.entries.set(unit, core, idx, value));
                    BCM_EXIT;
                }
            }
        }
        case queue_unicast_non_cont_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_non_cont_res_pool_ref.max_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_non_cont_res_pool_ref.valid_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_unicast_non_cont_res_pool_ref.entries.set(unit, core, idx, value));
                    BCM_EXIT;
                }
            }
        }
        case queue_multicast_cont_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_cont_res_pool_ref.max_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_cont_res_pool_ref.valid_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_cont_res_pool_ref.entries.set(unit, core, idx, value));
                    BCM_EXIT;
                }
            }
        }
        case queue_multicast_non_cont_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_non_cont_res_pool_ref.max_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_non_cont_res_pool_ref.valid_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_multicast_non_cont_res_pool_ref.entries.set(unit, core, idx, value));
                    BCM_EXIT;
                }
            }
        }
        case queue_isq_cont_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_cont_res_pool_ref.max_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_cont_res_pool_ref.valid_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_cont_res_pool_ref.entries.set(unit, core, idx, value));
                    BCM_EXIT;
                }
            }
        }
        case queue_isq_non_cont_res_pool_ref_type:{
            switch (field) {
                case max_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_non_cont_res_pool_ref.max_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case valid_entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_non_cont_res_pool_ref.valid_entries.set(unit, core, value));
                    BCM_EXIT;
                }
                case entries_field:{
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.cosq.cosq_res_info.queue_isq_non_cont_res_pool_ref.entries.set(unit, core, idx, value));
                    BCM_EXIT;
                }
            }
        }
        default:{
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid type")));
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * COSQ Module - End
 */

