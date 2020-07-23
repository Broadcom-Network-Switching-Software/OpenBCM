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
#include <shared/swstate/sw_state_resmgr.h>
#include <shared/shr_template.h>

#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr_shr.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_utils.h>
#include <bcm_int/dpp/alloc_mngr_cosq.h>
#include <bcm_int/dpp/alloc_mngr_lif.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>
#include <bcm_int/dpp/alloc_mngr_local_lif.h>


/* If set, copies the warmboot array before performing binary search to find resource id by pool id. */
#define DPP_AM_UTILS_COPY_WB_ARRAY 1

/*
 * Debug test defines - end. 
 */

STATIC int
_bcm_dpp_resources_destroy(int unit);

STATIC int 
dpp_am_pp_resource_setup_in_range_by_pool_idx(int unit, int first_res, int last_res, int *pool_id);

STATIC int
_bcm_dpp_resources_create(int unit, int nof_resource_pools, int nof_cosq_pools);

STATIC int
_bcm_dpp_templates_create(int unit, int nof_template_pools);

STATIC int
_bcm_dpp_config_am_get(int unit, int *nof_resource_pools, int *nof_cosq_pools, int *nof_template_pools);

STATIC int 
bcm_dpp_am_calculate_nof_resource_pools(int unit, int *nof_resource_pools, int *nof_cosq_pools);

STATIC int 
bcm_dpp_am_calculate_nof_template_pools(int unit, int *nof_template_pools);



/*************************************/
/*************************************/
/**** ALLOC MANAGER INIT - START  ****/  
/*************************************/
/*************************************/

/*
 * Function:
 *     bcm_dpp_am_attach
 * Purpose:
 *     INIT alloc manager state
 * Parameters:
 *     unit       - Device number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_dpp_am_attach(int unit)
{
    int rv = BCM_E_NONE;
    int nof_resource_pools  = 0, 
        nof_cosq_pools      = 0, 
        nof_template_pools  = 0;
    uint8 is_allocated;
    BCMDNX_INIT_FUNC_DEFS;

    /* Initialise the resource/template mapping buffer. */
    rv = _bcm_dpp_config_am_get(unit, &nof_resource_pools, &nof_cosq_pools, &nof_template_pools);
    BCMDNX_IF_ERR_EXIT(rv);

    if(!SOC_WARM_BOOT(unit)) {
        rv = sw_state_access[unit].dpp.bcm.alloc_mngr.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if(!is_allocated) {
            rv = sw_state_access[unit].dpp.bcm.alloc_mngr.alloc(unit);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        rv = sw_state_access[unit].dpp.bcm.alloc_mngr_utils.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!is_allocated) {
            rv = sw_state_access[unit].dpp.bcm.alloc_mngr_utils.alloc(unit);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        rv = sw_state_access[unit].dpp.bcm.alloc_mngr_utils.resource_to_pool_map.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if(!is_allocated) {
            rv = sw_state_access[unit].dpp.bcm.alloc_mngr_utils.resource_to_pool_map.alloc(
                unit,
                SOC_DPP_CONFIG(unit)->am.nof_am_resource_ids + SOC_DPP_CONFIG(unit)->am.nof_am_cosq_resource_ids
            );
            BCMDNX_IF_ERR_EXIT(rv);
        }

        rv = sw_state_access[unit].dpp.bcm.alloc_mngr_utils.template_to_pool_map.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if(!is_allocated) {
            rv = sw_state_access[unit].dpp.bcm.alloc_mngr_utils.template_to_pool_map.alloc(
                unit,
                SOC_DPP_CONFIG(unit)->am.nof_am_template_ids
            );
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    rv = _bcm_dpp_resources_create(unit, nof_resource_pools, nof_cosq_pools);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_templates_create(unit, nof_template_pools);
    BCMDNX_IF_ERR_EXIT(rv);

#ifdef BCM_ARAD_SUPPORT
    /* Ingress LIF & Egress encapsulation initalization */
    if (SOC_IS_ARADPLUS_AND_BELOW(unit) && SOC_DPP_PP_ENABLE(unit)) {
        rv = _bcm_dpp_am_sync_lif_init(unit);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_am_egress_encap_sw_state_init(unit);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_am_ingress_lif_init(unit);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_am_egress_encap_init(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }
#endif /* BCM_ARAD_SUPPORT */
   

    if (SOC_IS_JERICHO(unit) && SOC_DPP_PP_ENABLE(unit)) {

        /* Init global lif module. */
        rv = _bcm_dpp_am_global_lif_init(unit);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Initialize global lif mapping module. */
        rv = _bcm_dpp_global_lif_mapping_init(unit);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Initialize local inlif module. */
        rv = _bcm_dpp_am_local_inlif_init(unit);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_am_local_outlif_init(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     bcm_dpp_am_clear
 * Purpose:
 *     Restart alloc manager state.
 * Parameters:
 *     unit       - Device number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_dpp_am_clear(int unit)
{
        int rv = BCM_E_NONE;

        BCMDNX_INIT_FUNC_DEFS;

        /* first destroy, then re-create */
        rv = _bcm_dpp_resources_destroy(unit);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = shr_template_detach(unit);
        BCMDNX_IF_ERR_EXIT(rv);


        rv = bcm_dpp_am_attach(unit);
        BCMDNX_IF_ERR_EXIT(rv);

exit:
        BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      _bcm_dpp_resources_destroy
 *   Purpose
 *      Create resource management information for the unit
 *   Parameters
 *      (IN) unit = unit number of the device
 *   Results
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise
 *   Notes
 *      This function needs to be called very early during bcm_petra_init().
 */
STATIC int
_bcm_dpp_resources_destroy(int unit)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    LOG_DEBUG(BSL_LS_BCM_COMMON,
              (BSL_META_U(unit,
                          "Unit:%d, Starting resources destroy\n"), unit));
#ifdef BCM_ARAD_SUPPORT
    /* Destroy Ingress LIF & Egress encapsulation */
    if (!SOC_IS_DETACHING(unit))
    {
        /*
         * This operation is not expected during 'deinit' (after new sw state is implemented)
         * so, for now, just avoid calling it while detaching. (Otherwise, an 'assert - like'
         * macro, SW_STATE_IS_DEINIT_CHECK, will fail the system.
         */
        if (SOC_DPP_PP_ENABLE(unit)) {
          rv = _bcm_dpp_am_ingress_lif_deinit(unit);
          BCMDNX_IF_ERR_EXIT(rv);

          rv = _bcm_dpp_am_sync_lif_deinit(unit);
          BCMDNX_IF_ERR_EXIT(rv);
        }
#endif /* BCM_ARAD_SUPPORT */
    }
    if (!SOC_IS_DETACHING(unit)) {
        rv = shr_template_detach(unit);
        BCMDNX_IF_ERR_EXIT(rv); 
    
        rv = sw_state_res_detach(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     bcm_dpp_am_attach
 * Purpose:
 *     DEINIT alloc manager state
 * Parameters:
 *     unit       - Device number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_dpp_am_detach(
    int unit)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_IS_JERICHO(unit) && SOC_DPP_PP_ENABLE(unit)) {
        rv = _bcm_dpp_global_lif_mapping_deinit(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = _bcm_dpp_resources_destroy(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_template_detach(unit);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      dpp_am_pp_resource_setup_in_range_by_pool_idx
 *   Purpose
 *      Given a unit, a range of resources (Not including cosq), an initialized pool id pointer and a pool info array pointer,
 *      inits all the resources in range with their appropriate number of pools, saves the base pool number for the resource
 *      and saves the relevant data in the pool array.
 *   Parameters
 *      (IN) unit           unit number of the device
 *      (IN) first_res      Start of the range to allocate.
 *      (IN) last_res       End of the range to allocate.
 *      (INOUT) pool_id     Should contain the base pool for the first resource. Will be returned with the first unused pool.
 *      (OUT) p_info        For every allocated pool, will be filled with the pool's information.
 *   Results
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise
 */
STATIC int 
dpp_am_pp_resource_setup_in_range_by_pool_idx(int unit, int first_res, int last_res, int *pool_id){
    int res_id, pool_idx, rv, adj_pool_id;
    uint8 nof_pools_for_res;
    BCMDNX_INIT_FUNC_DEFS;

    for (res_id = first_res; res_id < last_res ; res_id++)   {

        /* First, save the index of this resource's first pool. */
        rv = sw_state_access[unit].dpp.bcm.alloc_mngr_utils.resource_to_pool_map.set(unit, res_id, *pool_id);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Get how many pools to allocate for the resource. */
        rv = bcm_dpp_am_resource_to_nof_pools(unit, res_id, &nof_pools_for_res);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Allocate the pools. */
        for (pool_idx = 0 ; pool_idx < nof_pools_for_res ; pool_idx++) {
            adj_pool_id = *pool_id + pool_idx;
            rv = _bcm_dpp_pp_resource_setup(unit, res_id, pool_idx, adj_pool_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* The next resource will start from this index. */
        *pool_id += nof_pools_for_res;
    }
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 *   Function
 *      _bcm_dpp_resources_create
 *   Purpose
 *      Initializes the resource manager, and the regular and lif resources for the unit.
 *   Parameters
 *      (IN) unit                   unit number of the device
 *      (IN) nof_resource_pools     Number of resource pools to allocate.
 *      (IN) nof_cosq_pools         Number of cosq resource pools to allocate.
 *   Results
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise
 */
STATIC int
_bcm_dpp_resources_create(int unit, int nof_resource_pools, int nof_cosq_pools)
{
    int rv;
    int pool_id, num_pools;
    BCMDNX_INIT_FUNC_DEFS;

    /* The number of pools has already been calculated on device. */
    num_pools = nof_resource_pools + nof_cosq_pools;

    LOG_DEBUG(BSL_LS_BCM_COMMON,
              (BSL_META_U(unit,
                          "Unit:%d, Starting resources create\n"), unit));

    if (!SOC_WARM_BOOT(unit)) { /* In WB, we only load from sw state. */
        rv = sw_state_res_init(unit,
                          num_pools, /* number of types is the same as the number of pools */
                          num_pools /* number of resource pools */);

        pool_id = 0; /* Start from the first pool. */
        /* Allocate the regular resources */
        rv = dpp_am_pp_resource_setup_in_range_by_pool_idx(unit, 0, dpp_am_res_count, &pool_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      _bcm_dpp_templates_create
 *   Purpose
 *      Create template management information for the unit
 *   Parameters
 *      (IN) unit = unit number of the device
 *   Results
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise
 *   Notes
 *      This function needs to be called very early during bcm_petra_init().
 */
STATIC int
_bcm_dpp_templates_create(int unit, int nof_template_pools)
{
    int template_id, pool_id, pool_idx;
    uint8 nof_pools_for_template;
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    /* account for all the template types/template pools */
    /* pool resources for all modules are allocated in this function */
    
    rv = shr_template_init(unit,
                      nof_template_pools /* number of template types (depecrated, equals nof_pools) */,
                      nof_template_pools /* number of template pools */,
                      _bcm_dpp_am_template_to_stream_arr,
                      _bcm_dpp_am_template_from_stream_arr,
                      _bcm_dpp_am_template_hash_compare_cb_idx_count);

    if (!SOC_WARM_BOOT(unit)) {
        pool_id = 0;
        for (template_id = 0 ; template_id < SOC_DPP_CONFIG(unit)->am.nof_am_template_ids ; template_id++)   {

            /* First, save the index of this template's first pool. */
            rv = sw_state_access[unit].dpp.bcm.alloc_mngr_utils.template_to_pool_map.set(unit, template_id, pool_id);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Get how many pools to allocate for the resource. */
            rv = bcm_dpp_am_template_to_nof_pools(unit, template_id, &nof_pools_for_template);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Allocate the pools. */
            for (pool_idx = 0 ; pool_idx < nof_pools_for_template ; pool_idx++) {
                rv = _bcm_dpp_template_setup(unit, template_id, pool_idx, pool_id);
                BCMDNX_IF_ERR_EXIT(rv);
                pool_id++;
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*******************************************/
/** Number of pools calculation functions.**/
/*******************************************/
/*
 *   Function
 *      bcm_dpp_am_calculate_nof_resource_pools
 *      bcm_dpp_am_calculate_nof_template_pools
 *      _bcm_dpp_config_am_get
 *  
 *  
 *   Purpose
 *      _bcm_dpp_config_am_get is the wrapper function for the other two functions.
 *      Those functions fill the number of pools per resource 
 *   Parameters
 *      (IN) unit               - unit number of the device
 *  
 * In bcm_dpp_am_calculate_nof_resource_pools:
 *      (OUT)nof_resource_pools - Will be filled with the number of regular resource pools to allocate. 
 *      (OUT)nof_cosq_pools     - Will be filled with the number of lif resource pools to allocate. 
 *  
 * In bcm_dpp_am_calculate_nof_template_pools:
 *      (OUT)nof_template_pools - Will be filled with the number of template pools to allocate. 
 *  
 *  
 *      SOC_DPP_CONFIG(unit)->am
 * In bcm_dpp_am_calculate_nof_resource_pools:
 *                                  ->nof_am_resource_ids   - Will be filled with the number of regular resource ids.
 *                                  ->nof_am_cosq_ids       - Will be filled with the number of cosq resource ids.
 * In bcm_dpp_am_calculate_nof_template_pools: 
 *                                  ->nof_am_template_ids   - Will be filled with the number of template ids.
 *  
 *   Results
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise
 */


/* See function description above. */
STATIC int
_bcm_dpp_config_am_get(int unit, int *nof_resource_pools, int *nof_cosq_pools, int *nof_template_pools){
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_calculate_nof_resource_pools(unit, nof_resource_pools, nof_cosq_pools);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = bcm_dpp_am_calculate_nof_template_pools(unit, nof_template_pools);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;    
}


/* See function description above. */
STATIC int 
bcm_dpp_am_calculate_nof_resource_pools(int unit, int *nof_resource_pools, int *nof_cosq_pools){
    int rv;
    int res_id;
    uint8 nof_pools;
    _dpp_res_type_cosq_t dpp_res_cosq_type;
    soc_dpp_config_am_t *am_config;
    int nof_cosq_res;
    int cosq_res_start;

    BCMDNX_INIT_FUNC_DEFS;

    am_config = &SOC_DPP_CONFIG(unit)->am;

    /*
     * GENERAL RESOURCES
     * All resources that are not lif or cosq (in the _dpp_am_res_t enum).
 */

    /* Calcualte the number of pools for resources that are not lif or cosq. */
    am_config->nof_am_resource_ids = dpp_am_res_count;

    /* Reset the global number of resources. */
    *nof_resource_pools = 0;

    /* For each resource type, get the number of pools, and add it to the total.*/
    for (res_id = 0 ; res_id < am_config->nof_am_resource_ids ; res_id++) {
        rv = bcm_dpp_am_resource_to_nof_pools(unit, res_id, &nof_pools);
        BCMDNX_IF_ERR_EXIT(rv);
        *nof_resource_pools += nof_pools;
    }

    /*
     * 
     * COSQ RESOURCES 
     * All resources mapped by the cosq allocation functions. 
 */
    
    /* Reset the global number of resources to indicate that we want to read the number of resources. */
    am_config->nof_am_cosq_resource_ids = 0;
    
    /* Fill cosq resources */
    rv = _bcm_dpp_resources_fill_type_cosq(unit, SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores - 1, &dpp_res_cosq_type);
    BCMDNX_IF_ERR_EXIT(rv);


    /* Calculate cosq number of pools and types. Cosq allocation doesn't use type_id == pool_id,
       so there are more pool pointers than resources allocated.
       Type id is used as res_id in this case. */
    cosq_res_start = DPP_AM_RES_COSQ_START(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    am_config->nof_am_cosq_resource_ids = 
        dpp_res_cosq_type.dpp_res_type_cosq_queue_dynamic_end - cosq_res_start;

    nof_cosq_res = am_config->nof_am_cosq_resource_ids;

    *nof_cosq_pools = 0;
    /* For each resource type, get the number of pools, and add it to the total.*/
    for (res_id = DPP_AM_RES_COSQ_START(unit) ; res_id < DPP_AM_RES_COSQ_START(unit) + nof_cosq_res ; res_id++) {
        rv = bcm_dpp_am_resource_to_nof_pools(unit, res_id, &nof_pools);
        BCMDNX_IF_ERR_EXIT(rv);
        *nof_cosq_pools += nof_pools;
    }


exit:
    BCMDNX_FUNC_RETURN;
}

/* See function description above. */
STATIC int 
bcm_dpp_am_calculate_nof_template_pools(int unit, int *nof_template_pools){
    int rv, template_id;
    soc_dpp_config_am_t *am_config;
    uint8 nof_pools;
    BCMDNX_INIT_FUNC_DEFS;

    /* Update global am template information. */
    am_config = &SOC_DPP_CONFIG(unit)->am;
    am_config->nof_am_template_ids = dpp_am_template_count;
    
    /* Iterate over the pools and sum the number of pools each template use. */
    *nof_template_pools = 0;
    for (template_id = 0 ; template_id < am_config->nof_am_template_ids ; template_id++) {
        rv = bcm_dpp_am_template_to_nof_pools(unit, template_id, &nof_pools);
        BCMDNX_IF_ERR_EXIT(rv);
        *nof_template_pools += nof_pools;
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

/*************************************/
/*************************************/
/***** ALLOC MANAGER INIT - END  *****/  
/*************************************/
/*************************************/


/********************************************/
/********************************************/
/**** GENERAL CORE HANDLING UTILS - START ***/  
/********************************************/
/********************************************/

/**
 * Most of the functions in the general core handling utils 
 * block have one function for templates and resources and both 
 * call an inner generic function.  
 */

/*************************************************/
/** Resource / template to nof_pools functions. **/
/*************************************************/

/*
 *   Function
 *      bcm_dpp_am_resource_to_nof_pools
 *      bcm_dpp_am_template_to_nof_pools
 *   Purpose
 *      Given a resource id or template id, returns the number of pools that should be allocated fot this resource or template.
 *      The default is 1. For all chips below ARADPLUS, this is always 1. 
 *  
 *   Parameters
 *      (IN) unit                   - unit number of the device
 *      (IN) res_id | template_id   - Resource id or template id to be checked.    
 *      (OUT)nof_pools_per_res      - Will be filled with the number of pools for this resource.
 *  
 *  
 *   Results
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise
 */
int 
bcm_dpp_am_resource_to_nof_pools(int unit, int res_id, uint8 *nof_pools_per_res){
    BCMDNX_INIT_FUNC_DEFS;

    /* Only one core in aradplus and below. */
   
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        /* 
         *  Lif resources are the only ones currently using more than one pool in arad.
         */
        *nof_pools_per_res = (res_id == dpp_am_res_obs_inlif || res_id == dpp_am_res_obs_eg_encap) ? _BCM_DPP_AM_LIF_NOF_BANKS 
                                                                                                : BCM_DPP_AM_DEFAULT_NOF_POOLS;
        BCM_EXIT;
    }
  
    /* Enter your resource id as a case here, and handle it as needed. */
    switch (res_id) {

        case dpp_am_res_fec_global:
            *nof_pools_per_res = SOC_DPP_DEFS_GET(unit, nof_fec_banks);
            break;
        case dpp_am_res_local_inlif_common:
        case dpp_am_res_local_inlif_wide:
            *nof_pools_per_res = _BCM_DPP_AM_INGRESS_LIF_NOF_BANKS;
            break;
        case dpp_am_res_local_outlif:
            *nof_pools_per_res = _BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_HALF_BANKS(unit);
            break;
        case dpp_am_res_obs_inlif:
        case dpp_am_res_obs_eg_encap:
            *nof_pools_per_res = (SOC_IS_ARADPLUS_AND_BELOW(unit)) ? _BCM_DPP_AM_LIF_NOF_BANKS : BCM_DPP_AM_DEFAULT_NOF_POOLS;
            break;
        case dpp_am_res_vsq_src_port:
        case dpp_am_res_vsq_pg:
            *nof_pools_per_res = SOC_DPP_DEFS_GET(unit, nof_cores);
            break;
	    case dpp_am_res_meter_a:
	    case dpp_am_res_meter_b:
            *nof_pools_per_res = SOC_DPP_CONFIG(unit)->meter.nof_meter_cores;
			break;
		case dpp_am_res_ether_policer:
			*nof_pools_per_res = SOC_DPP_CONFIG(unit)->meter.nof_meter_cores;
			break;
		default:
            *nof_pools_per_res = BCM_DPP_AM_DEFAULT_NOF_POOLS;
    }

    /*
     *  If your resource is within a certain range (cosq or lif), then add an if block here for the range.
 */
    
exit:
    BCMDNX_FUNC_RETURN;
}

/* See function description above. */
int 
bcm_dpp_am_template_to_nof_pools(int unit, int template_id, uint8 *nof_pools){
    BCMDNX_INIT_FUNC_DEFS;

    /* Only one core in aradplus and below. */
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        *nof_pools = BCM_DPP_AM_DEFAULT_NOF_POOLS;
        BCM_EXIT;
    }

    /* Enter your template id as a case here, and handle it as needed. */
    switch (template_id) {
    case dpp_am_template_egress_thresh:
    case dpp_am_template_egress_interface_unicast_thresh:
    case dpp_am_template_egress_interface_multicast_thresh:
    case dpp_am_template_egress_queue_mapping:
    case dpp_am_template_nrdy_threshold:
    case dpp_am_template_scheduler_adjust_size_final_delta:
    case dpp_am_template_lif_mtu_profile:
        *nof_pools = SOC_DPP_DEFS_GET(unit, nof_cores);
        break;

    case dpp_am_template_cosq_port_hr_flow_control:
        *nof_pools = SOC_DPP_DEFS_GET(unit, nof_cores);
        break;

    case dpp_am_template_queue_discount_cls:
    case dpp_am_template_pp_port_discount_cls:
        *nof_pools = SOC_DPP_DEFS_GET(unit, nof_cores);
        break;

    case dpp_am_template_cosq_sched_class:
        *nof_pools = SOC_DPP_DEFS_GET(unit, nof_cores);
        break;

    case dpp_am_template_egress_port_discount_cls_type_raw:
        *nof_pools = SOC_DPP_DEFS_GET(unit, nof_cores);
        break;

    case dpp_am_template_egress_port_discount_cls_type_cpu:
        *nof_pools = SOC_DPP_DEFS_GET(unit, nof_cores);
        break;

    case dpp_am_template_egress_port_discount_cls_type_eth:
        *nof_pools = SOC_DPP_DEFS_GET(unit, nof_cores);
        break;

    case dpp_am_template_egress_port_discount_cls_type_tm:
        *nof_pools = SOC_DPP_DEFS_GET(unit, nof_cores);
        break;
    case dpp_am_template_ingress_uc_tc_mapping:
    case dpp_am_template_ingress_flow_tc_mapping:
        *nof_pools = (!SOC_DPP_CORE_MODE_IS_SYMMETRIC(unit)) ? SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores : BCM_DPP_AM_DEFAULT_NOF_POOLS;
        break;
    case dpp_am_template_meter_profile_a_low:
	case dpp_am_template_meter_profile_b_low:
    	*nof_pools = SOC_DPP_CONFIG(unit)->meter.nof_meter_cores;	
		break;
	case dpp_am_template_vsq_pg_tc_mapping:
    	*nof_pools = SOC_DPP_DEFS_GET(unit, nof_cores);
        break;
	case dpp_am_template_crps_inpp_port_compensation_profile:
    case dpp_am_template_crps_intm_port_compensation_profile:
    case dpp_am_template_stat_interface_ing_port_compensation_profile:
    case dpp_am_template_stat_interface_egr_port_compensation_profile:
	    *nof_pools = SOC_DPP_DEFS_GET(unit, nof_cores);
	    break;
    default:
        *nof_pools = BCM_DPP_AM_DEFAULT_NOF_POOLS;
    }

    BCM_EXIT;    
exit:
    BCMDNX_FUNC_RETURN;
}

/****************************/
/** Base pool id functions **/
/****************************/
/*
 *   Function
 *      _bcm_dpp_am_id_to_pool_id_get
 *      bcm_dpp_am_resource_id_to_pool_id_get
 *      bcm_dpp_am_template_id_to_pool_id_get
 *   Purpose
 *      Given a resource id or a template id and a core id, returns the pool id for this <resource/template , core> tuple.
 *      _bcm_dpp_am_id_to_pool_id_get is the internal implementation of the two other functions.
 *  
 *  
 *   Parameters
 *      (IN) unit                                       - unit number of the device
 *      (IN) pool_idx                                    - core id for the resource. If core id is BCM_DPP_AM_INVALID_pool_idx,
 *                                                          the mapping will be direct mapping (i.e., the pool id will be the
 *                                                          resource/template id and not according to the alloc manager's mapping)
 *      (IN) res_id | template_id | res_template_id     - Resource id or template id.
 *      (OUT)pool_id                                    - Pool id for the tuple.
 *  
 * _bcm_dpp_am_base_pool_id_set only: 
 *      (IN) engine_id                                  - WB engine where the mapping should be saved
 *                                                          (either  template or resource)
 *  
 *   Results
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise
 */
STATIC int
_bcm_dpp_am_id_to_pool_id_get(int unit, int pool_idx, int (*get_func)(int, int, int*), int res_template_id, int *pool_id){
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    if (pool_idx == BCM_DPP_AM_INVALID_POOL_IDX) {
        *pool_id = res_template_id;
    } else {
        rv = get_func(unit, res_template_id, pool_id);
        BCMDNX_IF_ERR_EXIT(rv);

        *pool_id += pool_idx;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* See function description above. */
int
bcm_dpp_am_resource_id_to_pool_id_get(int unit, int pool_idx, int resource_id, int *pool_id){
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_am_id_to_pool_id_get(unit, pool_idx, sw_state_access[unit].dpp.bcm.alloc_mngr_utils.resource_to_pool_map.get, resource_id, pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* See function description above. */
int
bcm_dpp_am_template_id_to_pool_id_get(int unit, int pool_idx, int template_id, int *pool_id){
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_am_id_to_pool_id_get(unit, pool_idx, sw_state_access[unit].dpp.bcm.alloc_mngr_utils.template_to_pool_map.get, template_id, pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/****************************/
/** Base pool id functions **/
/****************************/
/*
 *   Function
 *      _bcm_dpp_am_pool_id_to_id_get
 *      bcm_dpp_am_pool_id_to_resource_id_get
 *      bcm_dpp_am_pool_id_to_template_id_get
 *   Purpose
 *      Given a pool id, returns the resource id and the core id mapped to this pool. 
 *      _bcm_dpp_am_pool_id_to_id_get is the internal implementation of the two other functions.
 *      It uses binary search in the base_pool_id mapping array to find the the closest base_pool_id.
 *  
 *   Parameters
 *      (IN) unit                                       - unit number of the device
 *      (IN) pool_id                                    - Pool id to be retrieved.
 *      (OUT)pool_idx                                    - Core id mapped to this pool.
 *      (OUT)res_id | template_id | res_template_id     - Resource id or template id mapped to this pool. 
 *  
 * _bcm_dpp_am_base_pool_id_set only: 
 *      (IN) engine_id                                  - WB engine where the mapping should be saved
 *                                                          (either  template or resource)
 *  
 *   Results
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise
 */
STATIC int 
_bcm_dpp_am_pool_id_to_id_get(int unit, int (*get_func)(int, int, int*), int nof_pools, int pool_id, int *res_template_id, int *pool_idx){
    int rv;
#if DPP_AM_UTILS_COPY_WB_ARRAY
    int *map_array = NULL;
    int alloc_size = sizeof(int) * nof_pools;
#endif
    int imin, imax, imid;
    int current;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(res_template_id);
    BCMDNX_NULL_CHECK(pool_idx);

#if DPP_AM_UTILS_COPY_WB_ARRAY
    /* Copy the WB array to save the calls to individual vars. */
    BCMDNX_ALLOC(map_array, (alloc_size), "binary search array");
    if (map_array == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }

    for(current = 0; current < nof_pools; ++current) {
        rv = get_func(unit, current, &map_array[current]);
        BCMDNX_IF_ERR_EXIT(rv);
    }
#endif

    /* Binary search for the pool_id. */
    imax = nof_pools - 1;
    imin = 0;

    /* continue searching while [imin,imax] is not empty */
    while (imax >= imin)
        {
        /* calculate the midpoint for roughly equal partition */
        imid = imin + ((imax - imin) / 2);

#if DPP_AM_UTILS_COPY_WB_ARRAY
        current = map_array[imid];
#else
        rv = get_func(unit, imid, &current);
        BCMDNX_IF_ERR_EXIT(rv);
#endif
        if(current == pool_id){
            /* pool_id found at index imid */
            *res_template_id = imid;
            break;
        } else if (imin == imax) {
            /* We're one off from the pool_id. It's either the current, or the one below. */
            if (current < pool_id) {
                *res_template_id = imid;
            } else {
                *res_template_id = imid - 1;
            }
            break;
        }
        /* determine which subarray to search */
        else if (current < pool_id){
            /* change min index to search upper subarray */
            imin = imid + 1;
        } else {
            /* change max index to search lower subarray */
            imax = imid - 1;
        }
    }

    /* Set core id */

  
exit:
#if DPP_AM_UTILS_COPY_WB_ARRAY
    BCM_FREE(map_array);
#endif
    BCMDNX_FUNC_RETURN;
}

/* See function description above. */
int 
bcm_dpp_am_pool_id_to_resource_id_get(int unit, int pool_id, int *res_id, int *pool_idx){
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_am_pool_id_to_id_get(unit, sw_state_access[unit].dpp.bcm.alloc_mngr_utils.resource_to_pool_map.get, 
                                       DPP_AM_RES_COSQ_START(unit), pool_id, res_id, pool_idx);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* See function description above. */
int 
bcm_dpp_am_pool_id_to_template_id_get(int unit, int pool_id, int *template_id, int *pool_idx){
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_am_pool_id_to_id_get(unit, sw_state_access[unit].dpp.bcm.alloc_mngr_utils.template_to_pool_map.get, 
                                       dpp_am_template_count, pool_id, template_id, pool_idx);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/********************************************/
/********************************************/
/**** GENERAL CORE HANDLING UTILS - END  ****/  
/********************************************/
/********************************************/


/********************************************/
/********************************************/
/** RESOURCE MANAGER ENCAPSULATION - START **/
/********************************************/
/********************************************/



int 
dpp_am_res_free_and_status(int unit, int pool_idx, int res_id, int count, int elem, uint32 *flags){
    int rv, pool_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, pool_idx, res_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = sw_state_res_free_and_status(unit, pool_id, count, elem, flags);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
dpp_am_res_free(int unit, int pool_idx, int res_id, int count, int elem){
    int rv;
    uint32 flags;
    BCMDNX_INIT_FUNC_DEFS;

    rv = dpp_am_res_free_and_status(unit, pool_idx, res_id, count, elem, &flags);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*******/

int 
dpp_am_res_alloc(int unit, int pool_idx, int res_id, uint32 flags, int count, int *elem){
    int rv, pool_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, pool_idx, res_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = sw_state_res_alloc(unit, pool_id, flags, count, elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     dpp_am_res_silent_alloc
 *     dpp_am_res_silent_alloc_align
 * Purpose:
 *     Some functions iterate over many resource banks when trying to allocate.
 *     The regular allocation functions would print spam for every visited full bank.
 *     This function optimises this by:
 *     1. Skipping the allocation of full banks.
 *     2. Not printing an error message for failed out of resource allocation.
 *  
 * Parameters:
 *     unit       - Device number
 * Returns: 
 *     BCM_E_RESOURCE - if a bank is full. 
 *     BCM_E_XXX
 */

int 
dpp_am_res_silent_alloc(int unit, int pool_idx, int res_id, uint32 flags, int count, int *elem){
    int rv = BCM_E_NONE, pool_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, pool_idx, res_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = sw_state_res_type_status_get(unit, pool_id);

    if (rv != BCM_E_FULL) {
        rv = sw_state_res_alloc(unit, pool_id, flags, count, elem);
        if (rv != BCM_E_RESOURCE) {
            BCMDNX_IF_ERR_EXIT(rv);
        }
    } else {
        rv = BCM_E_RESOURCE;
    }
    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


int
dpp_am_res_check(int unit, int pool_idx, int res_id, int count, int elem){
    int rv, pool_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, pool_idx, res_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = sw_state_res_check(unit, pool_id, count, elem);
    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


int
dpp_am_res_used_count_get(int unit, int pool_idx, int res_id, int *used_count){
    int rv, pool_id;
    sw_state_res_type_info_t info;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, pool_idx, res_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = sw_state_res_type_info_get(unit, pool_id, &info);
    BCMDNX_IF_ERR_EXIT(rv);

    *used_count = info.used;

exit:
    BCMDNX_FUNC_RETURN;
}
/*******/

/*******/

int 
dpp_am_res_alloc_align(int unit, int pool_idx, int res_id, uint32 flags, int align, int offset, int count, int *elem){
    int rv, pool_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, pool_idx, res_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = sw_state_res_alloc_align(unit, pool_id, flags, align, offset, count, elem);
    if(rv == BCM_E_RESOURCE) {
        BCM_ERR_EXIT_NO_MSG(rv);
    }
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int 
dpp_am_res_silent_alloc_align(int unit, int pool_idx, int res_id, uint32 flags, int align, int offset, int count, int *elem){
    int rv = BCM_E_NONE, pool_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, pool_idx, res_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = sw_state_res_type_status_get(unit, pool_id);

    if (rv != BCM_E_FULL) {
        rv = sw_state_res_alloc_align(unit, pool_id, flags, align, offset, count, elem);
        if (rv != BCM_E_RESOURCE) {
            BCMDNX_IF_ERR_EXIT(rv);
        }
    } else {
        rv = BCM_E_RESOURCE;
    }
    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/*******/

int
dpp_am_res_alloc_align_tag(int unit, int pool_idx, int res_id, uint32 flags, int align, int offset, const void *tag, int count, int *elem){
    int rv, pool_id;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, pool_idx, res_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = sw_state_res_alloc_align_tag(unit, pool_id, flags, align, offset, tag, count, elem);
    DPP_AM_RES_CHECK_SILENT_MODE(rv,flags);

exit:
    BCMDNX_FUNC_RETURN;
}

int
dpp_am_res_alloc_tag(int unit, int pool_idx, int res_id, uint32 flags, const void *tag, int count, int *elem){
    int rv, pool_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, pool_idx, res_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = sw_state_res_alloc_tag(unit, pool_id, flags, tag, count, elem);
    DPP_AM_RES_CHECK_SILENT_MODE(rv,flags);

exit:
    BCMDNX_FUNC_RETURN;
}

int
dpp_am_res_tag_set(int unit, int pool_idx, int res_id, uint32 flags, int offset, int count, const void *tag) {
    int rv, pool_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, pool_idx, res_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = sw_state_res_tag_set(unit, pool_id, offset, count, tag);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
dpp_am_res_tag_get(int unit, int pool_idx, int res_id, uint32 flags, int elem, const void *tag) {
    int rv, pool_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, pool_idx, res_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = sw_state_res_tag_get(unit, pool_id, elem, tag);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/*******/

int
dpp_am_res_check_all(int unit, int pool_idx, int res_id, int count, int elem){
    int rv, pool_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, pool_idx, res_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = sw_state_res_check_all(unit,pool_id,count,elem);
    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*******/

int
dpp_am_res_set_pool(int unit, int pool_idx, int res_id, sw_state_res_allocator_t manager, 
                                 int low_id, int count, const void *extras, const char *name){
    int rv, pool_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, pool_idx, res_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = sw_state_res_pool_set(unit, pool_id, manager, low_id, count, extras, name);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = sw_state_res_type_set(unit, pool_id, pool_id, 1, name);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}
/*******/
int
dpp_am_res_unset_pool(int unit, int pool_idx, int res_id){
    int rv, pool_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, pool_idx, res_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = sw_state_res_type_unset(unit, pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = sw_state_res_pool_unset(unit, pool_id);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/********************************************/
/********************************************/
/*** RESOURCE MANAGER ENCAPSULATION - END ***/
/********************************************/
/********************************************/


/********************************************/
/********************************************/
/** TEMPLATE MANAGER ENCAPSULATION - START **/
/********************************************/
/********************************************/

int 
dpp_am_template_allocate(int unit, int pool_idx, int template_id, uint32 flags, const void *data, int *is_allocated, int *template)
{
    int rv, pool_id;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_template_id_to_pool_id_get(unit, pool_idx, template_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_template_allocate(unit, pool_id, flags, data, is_allocated, template);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*******/

int 
dpp_am_template_allocate_group(int unit, int pool_idx, int template_id, 
                                            uint32 flags, const void *data, int nof_additions, int *is_allocated, int *template)
{
    int rv, pool_id;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_template_id_to_pool_id_get(unit, pool_idx, template_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_template_allocate_group(unit, pool_id, flags, data, nof_additions, is_allocated, template);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*******/

int
dpp_am_template_exchange(int unit, int pool_idx, int template_id, uint32 flags, const void *data, int old_template,
                                      int *is_last, int *template, int *is_allocated){
    int rv, pool_id;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_template_id_to_pool_id_get(unit, pool_idx, template_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_template_exchange(unit, pool_id, flags, data, old_template, is_last, template, is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*******/

int
dpp_am_template_free(int unit, int pool_idx, int template_id, int template, int *is_last){
    int rv, pool_id;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_template_id_to_pool_id_get(unit, pool_idx, template_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_template_free(unit, pool_id, template, is_last);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
dpp_am_template_free_group(int unit, int pool_idx, int template_id, int template, int nof_deductions, int *is_last){
    int rv, pool_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_template_id_to_pool_id_get(unit, pool_idx, template_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_template_free_group(unit, pool_id, template, nof_deductions, is_last);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/*******/
int
dpp_am_template_free_all(int unit, int pool_idx, int template_id, int template, int *nof_deductions){
    int rv, pool_id;
    int is_last;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_template_id_to_pool_id_get(unit, pool_idx, template_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_template_ref_count_get(unit, pool_id, template, (uint32*)nof_deductions);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_template_free_group(unit, pool_id, template, *nof_deductions, &is_last);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*******/
int 
dpp_am_template_clear(int unit, int pool_idx, int template_id){
    int rv, pool_id;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_template_id_to_pool_id_get(unit, pool_idx, template_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!SOC_IS_DETACHING(unit)) {
        rv = shr_template_clear(unit, pool_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*******/

int
dpp_am_template_exchange_test(int unit, int pool_idx, int template_id, uint32 flags, const void *data, int old_template,
                                      int *is_last, int *template, int *is_allocated){
    int rv, pool_id;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_template_id_to_pool_id_get(unit, pool_idx, template_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_template_exchange_test(unit, pool_id, flags, data, old_template, is_last, template, is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/*******/

int
dpp_am_template_data_get(int unit, int pool_idx, int template_id, int template, void *data){
    int rv, pool_id;

    BCMDNX_INIT_FUNC_DEFS;
    
    rv = bcm_dpp_am_template_id_to_pool_id_get(unit, pool_idx, template_id, &pool_id);
    if (rv != BCM_E_NONE) {
        BCM_RETURN_VAL_EXIT(rv);
    }

    rv = shr_template_data_get(unit, pool_id, template, data);
    if (rv != BCM_E_NONE) {
        BCM_RETURN_VAL_EXIT(rv);
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*******/

int
dpp_am_template_template_get(int unit, int pool_idx, int template_id, const void *data, int *template){
    int rv, pool_id;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_template_id_to_pool_id_get(unit, pool_idx, template_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_template_template_get(unit, pool_id, data, template);
    BCM_RETURN_VAL_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


/*********/

int
dpp_am_template_ref_count_get(int unit, int pool_idx, int template_id, int template, uint32 *ref_count){
    int rv, pool_id;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_template_id_to_pool_id_get(unit, pool_idx, template_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_template_ref_count_get(unit, pool_id, template, ref_count);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/*********/

int
dpp_am_template_template_count_get(int unit, int pool_idx, int template_id, int *template_count)
{

    int rv;
    bcm_dpp_am_template_info_t t_info;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_am_template_tinfo_get( unit, pool_idx, template_id, &t_info);
    BCMDNX_IF_ERR_EXIT(rv);

    *template_count = t_info.count;

exit:
    BCMDNX_FUNC_RETURN;
}



/********************************************/
/********************************************/
/*** TEMPLATE MANAGER ENCAPSULATION - END ***/
/********************************************/
/********************************************/

