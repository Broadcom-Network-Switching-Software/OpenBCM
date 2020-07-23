/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_DNXF_DIAG_H_
#define _SOC_DNXF_DIAG_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif





#include <soc/types.h>
#include <soc/error.h>
#include <soc/drv.h>

#include <soc/dnxc/error.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>

#include <soc/dnxf/cmn/dnxf_defs.h>





typedef struct soc_dnxf_counter_s {
    uint32 value;
    uint32 overflow;
} soc_dnxf_counter_t;

typedef struct soc_dnxf_dch_counters_info_s {
    uint64                 dch_total_in;
    uint64                 dch_total_out;
    uint64                 dch_fifo_discard;
    uint64                 dch_unreach_discard;
    uint32                 dch_max_fifo_cells;
} soc_dnxf_dch_counters_info_t;

typedef struct soc_dnxf_dtm_counters_info_s {
    uint64                 dtm_total_in_lr;
    uint64                 dtm_total_in_nlr;
    uint32                 dtm_max_fifo_cells_lr;
    uint32                 dtm_max_fifo_cells_nlr;
    uint64                 dtm_total_out_lr;
    uint64                 dtm_total_out_nlr;
} soc_dnxf_dtm_counters_info_t;

typedef struct soc_dnxf_dtl_counters_info_s {
    uint64                 dtl_total_in_lr;
    uint64                 dtl_total_in_nlr;
    uint64                 dtl_total_dropped_lr_cells[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES];
    uint64                 dtl_total_dropped_nlr_cells[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES];
    uint32                 dtl_max_fifo_cells_lr;
    uint32                 dtl_max_fifo_cells_nlr;
    uint64                 dtl_total_out;
} soc_dnxf_dtl_counters_info_t;

typedef struct soc_dnxf_counters_info_s {
    soc_dnxf_dch_counters_info_t         dch_fe1_counters_info[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    soc_dnxf_dch_counters_info_t         dch_fe3_counters_info[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    soc_dnxf_dtm_counters_info_t         dtm_fe1_counters_info[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    soc_dnxf_dtm_counters_info_t         dtm_fe3_counters_info[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    soc_dnxf_dtl_counters_info_t         dtl_fe1_counters_info[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    soc_dnxf_dtl_counters_info_t         dtl_fe3_counters_info[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    uint32                               nof_pipes;
} soc_dnxf_counters_info_t;

typedef struct soc_dnxf_queues_dch_link_group_s {
    uint32 most_occupied_link;
    uint32 max_occupancy_value;
    uint32 link_group_first_link;
    uint32 link_group_last_link;
} soc_dnxf_queues_dch_link_group_t;

typedef struct soc_dnxf_queues_dch_info_s {    
    soc_dnxf_queues_dch_link_group_t dch_link_group[DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_DCH_LINK_GROUPS];
} soc_dnxf_queues_dch_info_t;

typedef struct soc_dnxf_queues_dch_s{
    soc_dnxf_queues_dch_info_t    nof_dch[DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_INSTANCES_DCH];          
} soc_dnxf_queues_dch_t;

typedef struct soc_dnxf_queues_dcm_info_s{                   
    uint32 fifo_max_occupancy_value[DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_DTM_FIFOS];
} soc_dnxf_queues_dcm_info_t;

typedef struct soc_dnxf_queues_dtm_s{
    soc_dnxf_queues_dcm_info_t  nof_dtm[DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_INSTANCES_DCML];
} soc_dnxf_queues_dtm_t;

typedef struct soc_dnxf_queues_dtl_info_s {
    uint32   most_occupied_link;
    uint32   max_occupancy_value;
} soc_dnxf_queues_dtl_info_t ;

typedef struct soc_dnxf_queues_dtl_s{
    soc_dnxf_queues_dtl_info_t   nof_dtl[DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_INSTANCES_DCML];
} soc_dnxf_queues_dtl_t;

typedef struct soc_dnxf_queues_dfl_info_s {
    uint32   sub_bank_min_free_entries_value[2];
} soc_dnxf_queues_dfl_info_t;

typedef struct soc_dnxf_queues_dfl_s{
    soc_dnxf_queues_dfl_info_t   nof_dfl[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_DFL_BANKS];
} soc_dnxf_queues_dfl_t;

typedef struct soc_dnxf_queues_info_s{
    soc_dnxf_queues_dch_t        dch_queues_info[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    soc_dnxf_queues_dtm_t        dtm_queues_info[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    soc_dnxf_queues_dtl_t        dtl_queues_info_lr[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    soc_dnxf_queues_dtl_t        dtl_queues_info_nlr[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    soc_dnxf_queues_dfl_t        dfl_queues_info[DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_INSTANCES_DCML];
    uint32                       nof_pipes;
}soc_dnxf_queues_info_t;

typedef struct soc_dnxf_diag_flag_str_s {
    uint32 flag;
    char *name;
} soc_dnxf_diag_flag_str_t;

typedef struct soc_dnxf_diag_fabric_traffic_profile_s {
    uint32 unicast[soc_dnxf_fabric_priority_nof];
    uint32 multicast[soc_dnxf_fabric_priority_nof];
} soc_dnxf_diag_fabric_traffic_profile_t;


typedef enum
{
    soc_dnxf_threshold_group_gci_option = 0,
    soc_dnxf_threshold_group_rci_option,
    soc_dnxf_threshold_group_fc_option,
    soc_dnxf_threshold_group_drop_option,
    soc_dnxf_threshold_group_size_option,
    soc_dnxf_threshold_group_dynamic_weight_option
} soc_dnxf_threshold_group_options_t;


typedef enum
{
    soc_dnxf_threshold_stage_rx_option = 0,
    soc_dnxf_threshold_stage_middle_option,
    soc_dnxf_threshold_stage_tx_option,
    soc_dnxf_threshold_stage_shared_option,
    soc_dnxf_threshold_stage_nof
} soc_dnxf_threshold_stage_options_t;


typedef enum diag_queue_stage_options_e
{
    soc_dnxf_dch_stage_option = 0,
    soc_dnxf_dtm_stage_option = 1,
    soc_dnxf_dtl_stage_option = 2,
    soc_dnxf_dfl_stage_option = 3
} soc_dnxf_queue_stage_options_t;












extern const soc_dnxf_diag_flag_str_t soc_dnxf_diag_flag_str_fabric_cell_snake_test_stages[];






void soc_dnxf_counters_info_init(soc_dnxf_counters_info_t* fe_counters_info);
void soc_dnxf_queues_info_init(int unit, soc_dnxf_queues_info_t* fe_queues_info);
shr_error_e soc_dnxf_diag_fabric_cell_snake_test_interrupts_name_get(int unit, const soc_dnxf_diag_flag_str_t **intr_names);
shr_error_e soc_dnxf_diag_cell_pipe_counter_get(int unit, int pipe, uint64 *counter);
shr_error_e soc_dnxf_diag_fabric_traffic_profile_get(int unit, int source_id, int dest_id, soc_dnxf_diag_fabric_traffic_profile_t *traffic_info);

#endif 
