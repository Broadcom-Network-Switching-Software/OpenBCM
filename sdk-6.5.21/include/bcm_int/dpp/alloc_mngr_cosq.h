/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        alloc_mngr_cosq.h
 * Purpose:     Resource allocation for cosq.
 */

#ifndef  INCLUDE_ALLOC_MNGR_COSQ_H
#define  INCLUDE_ALLOC_MNGR_COSQ_H

#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_lif.h>

#define BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_PER_REGION 3
#define BCM_DPP_AM_RES_TYPE_COSQ_E2E_DYNAMIC_PER_REGION 3
#define BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_PER_REGION_MAX_ENTRIES (DPP_DEVICE_COSQ_TOTAL_FLOW_REGIONS(unit) * BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_PER_REGION)
#define BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_CONNECTOR_OFFSET 0
#define BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SE_OFFSET 1
#define BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SYNC_OFFSET 2

#define BCM_DPP_AM_RES_POOL_COSQ_E2E_TOTAL_DYNAMIC_ARAD (SOC_TMC_COSQ_TOTAL_FLOW_REGIONS*BCM_DPP_AM_RES_TYPE_COSQ_E2E_DYNAMIC_PER_REGION)
#define BCM_DPP_AM_RES_TYPE_COSQ_E2E_TOTAL_DYNAMIC_ARAD (SOC_TMC_COSQ_TOTAL_FLOW_REGIONS*BCM_DPP_AM_RES_TYPE_COSQ_E2E_DYNAMIC_PER_REGION)


#define BCM_DPP_AM_RES_POOL_COSQ_QUEUE_TOTAL_DYNAMIC_ARAD 96
#define BCM_DPP_AM_RES_TYPE_COSQ_QUEUE_TOTAL_DYNAMIC_ARAD 96

/*Cosq scheduler E2E*/
#define BCM_DPP_AM_COSQ_SCH_MAX_ALLOCATION_STAGES 18
#define BCM_DPP_AM_COSQ_SCH_MAX_ALLOCATION_REGIONS_TYPES_PER_FLOW_TYPE 4
#define BCM_DPP_AM_COSQ_SCH_NOF_ALLOCATION_SCHEMES 48
#define _BCM_DPP_AM_COSQ_GET_POOL_INDEX(flow_id,pool_offset) SOC_TMC_SCH_FLOW_TO_1K_ID((flow_id)) * BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_PER_REGION + (pool_offset)
#define _BCM_DPP_AM_COSQ_GET_REGION_INDEX_FROM_POOL_INDEX(pool_index) ((pool_index)/BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_PER_REGION)
#define _BCM_DPP_AM_COSQ_GET_REGION_INDEX_FROM_FLOW_INDEX(flow_id) _BCM_DPP_AM_COSQ_GET_REGION_INDEX_FROM_POOL_INDEX(_BCM_DPP_AM_COSQ_GET_POOL_INDEX(flow_id, 0))
#define _BCM_DPP_AM_COSQ_GET_FLOW_INDEX_FROM_REMOTE_CORE_INDEX(base_flow_id, nof_remote_cores, core) (base_flow_id + (1024/nof_remote_cores)*core)
#define _BCM_DPP_AM_COSQ_RESOURCE_CHECK_BREAK -1
#define _BCM_DPP_AM_COSQ_PER_TYPE_ALL_ALLOCATED_RESOURCE_FIND_MAX_STAGES 4
#define _BCM_DPP_AM_COSQ_QUARTET_LEN 4

#define DPP_DEVICE_QAX_COSQ_FLOW_REGION_API_OFFSET 64 /* The offset between user defined regions to hw regions */
#define DPP_DEVICE_QUX_COSQ_FLOW_REGION_API_OFFSET 96 /* The offset between user defined regions to hw regions */



/* The resouces order is:
   - Normal resources
   - Cosq pools 
   */
#define DPP_AM_RES_COSQ_START(_unit)   (dpp_am_res_count)

#define DPP_AM_RES_ID_IS_COSQ(_unit, _res_id)  \
    (_res_id >= DPP_AM_RES_COSQ_START(_unit))


/*
 * TM (COSQ)
 */
int 
_bcm_dpp_am_cosq_get_first_base_pool_id(int unit, int *curr_index) ;
int
bcm_dpp_am_cosq_init(int unit);

int
bcm_dpp_am_ingress_voq_allocate(int unit,
                                int core,
                                uint32 flags,
                                int is_non_contiguous,
                                int num_cos,
                                int queue_config_type,
                                int *voq_base);

int
bcm_dpp_am_ingress_voq_deallocate(int unit,
                                  int core,
                                  uint32 flags,
                                  int is_non_contiguous,
                                  int num_cos,
                                  int voq_base);

int
bcm_dpp_am_cosq_scheduler_allocate(int unit,
                                       int core,
                                       uint32 nof_remote_cores,
                                       uint32 flags,
                                       int is_composite,
                                       int is_enhanced,
                                       int is_dual,
                                       int is_non_contiguous,
                                       int num_cos,
                                       SOC_TMC_AM_SCH_FLOW_TYPE flow_type,
                                       uint8* src_modid,
                                       int *flow_id);
int
bcm_dpp_am_cosq_scheduler_deallocate(int unit,
                                       int core,
                                       uint32 flags,
                                       int is_composite,
                                       int is_enhanced,
                                       int is_dual,
                                       int is_non_contiguous,
                                       int num_cos,
                                       SOC_TMC_AM_SCH_FLOW_TYPE flow_type,
                                       int flow_id);
int 
bcm_dpp_am_cosq_get_region_type(int unit,
                           int core,
                           int flow_id,
                           int *region_type);

int 
bcm_dpp_am_cosq_fetch_quad(int unit,
                           int core,
                           int flow_id,
                           int region_type,
                           bcm_dpp_am_cosq_quad_allocation_t *is_allocated);


int     
bcm_dpp_am_cosq_fetch_allocated_resources(int unit,
                                        int core,
                                        SOC_TMC_AM_SCH_FLOW_TYPE flow_type,
                                        bcm_dpp_am_cosq_pool_ref_t* allocated_ref);

int
bcm_dpp_am_cosq_queue_region_config_get(int unit,
                                        int core,
                                        int *queue_region_config,
                                        int queue_config_type);
int
_bcm_dpp_am_cosq_process_queue_region(int unit, 
                                      int core,
                                      int queue_config_type,
                                      int *queue_region_config);


int
_bcm_dpp_resources_fill_type_cosq(int unit, int core, _dpp_res_type_cosq_t* dpp_res_cosq_type);

int
_bcm_dpp_resources_fill_pool_cosq(int unit, int core, _dpp_res_pool_cosq_t* dpp_res_cosq_pool);

int sw_state_bcm_dpp_am_cosq_pool_ref_t_get (int unit, int core, bcm_dpp_am_cosq_pool_ref_type_t type, bcm_dpp_am_cosq_pool_ref_field_t field, int idx, int *result);
int sw_state_bcm_dpp_am_cosq_pool_ref_t_set (int unit, int core, bcm_dpp_am_cosq_pool_ref_type_t type, bcm_dpp_am_cosq_pool_ref_field_t field, int idx, int value);

#endif /*INCLUDE_ALLOC_MNGR_COSQ_H*/
