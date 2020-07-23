/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE1600 DEFS H
 */

#ifndef _SOC_DFE_DIAG_H_
#define _SOC_DFE_DIAG_H_





#include <soc/types.h>
#include <soc/error.h>
#include <soc/drv.h>

#include <soc/dcmn/error.h>

#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dfe/cmn/dfe_config_defs.h>






typedef struct soc_dfe_counter_s {
    uint32 value;
    uint32 overflow;
} soc_dfe_counter_t;



typedef struct soc_dch_counters_info_s { 
    soc_dfe_counter_t     dch_total_in; 
    soc_dfe_counter_t     dch_total_out;
    soc_dfe_counter_t     dch_fifo_discard;
    soc_dfe_counter_t     reorder_discard;
    soc_dfe_counter_t     sum_dch_unreach_discard;
    uint32                dch_max_q_size;    

} soc_dch_counters_info_t;

typedef struct soc_dcm_counters_info_s { 
    uint32                      dcma_tot;
    uint32                      dcma_drp;
    uint32                      dcma_max;
    uint32                      dcmb_tot;
    uint32                      dcmb_drp;
    uint32                      dcmb_max;
    soc_dfe_counter_t           dcm_tot;
    soc_dfe_counter_t           dcm_drop;
    uint32                      dcm_max;

} soc_dcm_counters_info_t;

typedef struct soc_dcl_counters_info_s { 
    soc_dfe_counter_t   dcl_tot_in;  
    soc_dfe_counter_t   dcl_tot_out;
    soc_dfe_counter_t   dcl_dropped;
    uint32     dcl_max;
} soc_dcl_counters_info_t;

typedef struct soc_ccs_counters_info_s { 
    soc_dfe_counter_t    ccs_total_control;
    soc_dfe_counter_t    ccs_flow_stat;
    soc_dfe_counter_t    ccs_credits;
    soc_dfe_counter_t    ccs_reachability;
    soc_dfe_counter_t    ccs_unreach_dest;
} soc_ccs_counters_info_t;

typedef struct soc_dfe_counters_info_s {
    soc_dch_counters_info_t         dch_counters_info[SOC_DFE_MAX_NOF_PIPES];
    soc_dcm_counters_info_t         dcm_counters_info[SOC_DFE_MAX_NOF_PIPES];
    soc_dcl_counters_info_t         dcl_counters_info[SOC_DFE_MAX_NOF_PIPES];

    soc_dfe_counter_t               dch_dcm_fc[SOC_DFE_MAX_NOF_PIPES];
                         
    soc_dfe_counter_t               dcm_dcl_fc[SOC_DFE_MAX_NOF_PIPES];
   
    soc_ccs_counters_info_t         ccs_counters_info;

    uint32                          nof_pipes;

} soc_dfe_counters_info_t;

typedef struct soc_dfe_queues_dch_info_s {    
    uint32 nof_link_a;
    uint32 value_a;
    uint32 nof_link_b;
    uint32 value_b;
	uint32 nof_link_c;
	uint32 value_c;
} soc_dfe_queues_dch_info_t;

typedef struct soc_dfe_queues_dch_s{         
    soc_dfe_queues_dch_info_t    nof_dch[SOC_DFE_NOF_DCH];          
} soc_dfe_queues_dch_t;

typedef struct soc_dfe_queues_dcm_info_s{                   
    uint32 nof_fifo[SOC_DFE_NOF_FIFOS_PER_DCM];

} soc_dfe_queues_dcm_info_t;

typedef struct soc_dfe_queues_dcm_s{
    soc_dfe_queues_dcm_info_t   nof_dcma[SOC_DFE_DEFS_MAX(NOF_INSTANCES_DCMA)];
	soc_dfe_queues_dcm_info_t	nof_dcmb[SOC_DFE_DEFS_MAX(NOF_INSTANCES_DCMB)];
	soc_dfe_queues_dcm_info_t	nof_dcm[SOC_DFE_DEFS_MAX(NOF_INSTANCES_DCM)];

} soc_dfe_queues_dcm_t;

typedef struct soc_dfe_queues_dcl_info_s {
    uint32   nof_link;
    uint32   value;

} soc_dfe_queues_dcl_info_t ;


typedef struct soc_dfe_queues_dcl_s{
    soc_dfe_queues_dcl_info_t   nof_dcl[SOC_DFE_NOF_DCL];
} soc_dfe_queues_dcl_t;

typedef struct soc_dfe_queues_info_s{
    soc_dfe_queues_dch_t        dch_queues_info[SOC_DFE_MAX_NOF_PIPES];
    soc_dfe_queues_dcm_t        dcm_queues_info[SOC_DFE_MAX_NOF_PIPES];
    soc_dfe_queues_dcl_t        dcl_queues_info[SOC_DFE_MAX_NOF_PIPES];
    uint32                      nof_pipes;

}soc_dfe_queues_info_t;

typedef struct soc_dfe_diag_flag_str_s {
    uint32 flag;
    char *name;
} soc_dfe_diag_flag_str_t;

typedef struct soc_dfe_diag_fabric_traffic_profile_s {
    uint32 unicast[soc_dfe_fabric_priority_nof];
    uint32 multicast[soc_dfe_fabric_priority_nof];
} soc_dfe_diag_fabric_traffic_profile_t;











extern const soc_dfe_diag_flag_str_t soc_dfe_diag_flag_str_fabric_cell_snake_test_stages[];






void soc_dfe_counters_info_init(soc_dfe_counters_info_t* fe_counters_info);
void soc_dfe_queues_info_init(int unit, soc_dfe_queues_info_t* fe_queues_info);
soc_error_t soc_dfe_diag_fabric_cell_snake_test_interrupts_name_get(int unit, const soc_dfe_diag_flag_str_t **intr_names);
soc_error_t soc_dfe_diag_cell_pipe_counter_get(int unit, int pipe, uint64 *counter);
soc_error_t soc_dfe_diag_fabric_traffic_profile_get(int unit, int source_id, int dest_id, soc_dfe_diag_fabric_traffic_profile_t *traffic_info);



#endif 
