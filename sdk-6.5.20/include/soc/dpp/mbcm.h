/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        mbcm.h
 * Purpose:     Multiplexing of the bcm layer
 *
 * Different chip families require such different implementations
 * of some basic BCM layer functionality that the functions are
 * multiplexed to allow a fast runtime decision as to which function
 * to call.  This file contains the basic declarations for this
 * process.
 *
 * See internal/design/soft_arch/xgs_plan.txt for more info.
 *
 * Conventions:
 *    MBCM is the multiplexed bcm prefix
 *    _f is the function type declaration postfix
 */

#ifndef _SOC_DPP_MBCM_H
#define _SOC_DPP_MBCM_H

#include <soc/macipadr.h>
#include <soc/drv.h>
#include <soc/dpp/cosq.h>
#include <soc/dpp/dpp_defs.h>
#include <soc/dpp/fabric.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dcmn/dcmn_port.h>

#include <soc/dpp/TMC/tmc_api_ingress_traffic_mgmt.h>
#include <soc/dpp/TMC/tmc_api_action_cmd.h>
#include <soc/dpp/TMC/tmc_api_cnm.h>
#include <soc/dpp/TMC/tmc_api_cnt.h>
#include <soc/dpp/TMC/tmc_api_diagnostics.h> 
#include <soc/dpp/TMC/tmc_api_egr_queuing.h> 
#include <soc/dpp/TMC/tmc_api_fabric.h> 
#include <soc/dpp/TMC/tmc_api_general.h> 
#include <soc/dpp/TMC/tmc_api_ingress_traffic_mgmt.h> 
#include <soc/dpp/TMC/tmc_api_ports.h> 
#include <soc/dpp/TMC/tmc_api_ssr.h> 
#include <soc/dpp/TMC/tmc_api_stack.h> 
#include <soc/dpp/TMC/tmc_api_tdm.h> 
#include <soc/dpp/TMC/tmc_api_cell.h> 
#include <soc/dpp/TMC/tmc_api_diagnostics.h> 
#include <soc/dpp/TMC/tmc_api_egr_queuing.h> 
#include <soc/dpp/TMC/tmc_api_end2end_scheduler.h> 
#include <soc/dpp/TMC/tmc_api_fabric.h> 
#include <soc/dpp/TMC/tmc_api_ingress_packet_queuing.h> 
#include <soc/dpp/TMC/tmc_api_ingress_scheduler.h> 
#include <soc/dpp/TMC/tmc_api_ingress_traffic_mgmt.h> 
#include <soc/dpp/TMC/tmc_api_multicast_egress.h> 
#include <soc/dpp/TMC/tmc_api_multicast_fabric.h> 
#include <soc/dpp/TMC/tmc_api_multicast_ingress.h> 
#include <soc/dpp/TMC/tmc_api_ofp_rates.h> 
#include <soc/dpp/TMC/tmc_api_packet.h> 
#include <soc/dpp/TMC/tmc_api_ports.h> 
#include <soc/dpp/TMC/tmc_api_reg_access.h>
#include <soc/dpp/TMC/tmc_api_mgmt.h>
#include <soc/dpp/TMC/tmc_api_header_parsing_utils.h>
#include <soc/dpp/TMC/tmc_api_flow_control.h>
#include <soc/dpp/TMC/tmc_api_link_bonding.h>
#include <soc/dpp/multicast_imp.h> 
#include <soc/dpp/JER/jer_mgmt.h>
#include <soc/dpp/JER/jer_nif_prd.h>
#include <soc/dpp/JER/jer_nif.h>
#include <soc/dpp/ARAD/arad_egr_prog_editor.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>


typedef enum soc_dpp_chip_family_e {
    BCM_FAMILY_PETRAB,
    BCM_FAMILY_ARAD,
    BCM_FAMILY_JERICHO,
    BCM_FAMILY_QAX
} soc_dpp_chip_family_t;



 
 
 
typedef uint32 (*mbcm_dpp_action_cmd_snoop_set_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  uint32                              action_ndx, 
    SOC_SAND_IN  SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO               *info 
  ); 
typedef uint32 (*mbcm_dpp_action_cmd_snoop_get_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  uint32                              action_ndx, 
    SOC_SAND_OUT SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO               *info 
  ); 
typedef uint32 (*mbcm_dpp_action_cmd_mirror_set_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  uint32                              action_ndx, 
    SOC_SAND_IN  SOC_TMC_CMD_TYPE                               cmnd_type,
    SOC_SAND_IN  SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO              *info
  ); 
typedef uint32 (*mbcm_dpp_action_cmd_mirror_get_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  uint32                              action_ndx, 
    SOC_SAND_IN  SOC_TMC_CMD_TYPE                  cmnd_type,
    SOC_SAND_OUT SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO              *info 
  );
typedef uint32 (*mbcm_dpp_action_cmd_cosq_flush_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  soc_port_t                       port, 
    SOC_SAND_OUT uint32                           enable  
 ); 

 
typedef uint32 (*mbcm_dpp_cnm_cp_set_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  SOC_TMC_CNM_CP_INFO                         *info 
  ); 
typedef uint32 (*mbcm_dpp_cnm_cp_get_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_OUT SOC_TMC_CNM_CP_INFO                         *info 
  ); 
typedef uint32 (*mbcm_dpp_cnm_q_mapping_set_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  SOC_TMC_CNM_Q_MAPPING_INFO                  *info 
  ); 
typedef uint32 (*mbcm_dpp_cnm_q_mapping_get_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_OUT SOC_TMC_CNM_Q_MAPPING_INFO                  *info 
  ); 
typedef uint32 (*mbcm_dpp_cnm_congestion_test_set_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  SOC_TMC_CNM_CONGESTION_TEST_INFO            *info 
  ); 
typedef uint32 (*mbcm_dpp_cnm_congestion_test_get_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_OUT SOC_TMC_CNM_CONGESTION_TEST_INFO            *info 
  ); 
typedef uint32 (*mbcm_dpp_cnm_cp_profile_set_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  uint32                              profile_ndx, 
    SOC_SAND_IN  SOC_TMC_CNM_CP_PROFILE_INFO                 *info 
  ); 
typedef uint32 (*mbcm_dpp_cnm_cp_profile_get_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  uint32                              profile_ndx, 
    SOC_SAND_OUT SOC_TMC_CNM_CP_PROFILE_INFO                 *info 
  ); 
typedef uint32 (*mbcm_dpp_cnm_sampling_profile_set_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  uint32                              profile_ndx, 
    SOC_SAND_IN  uint32                               sampling_rate 
  ); 
typedef uint32 (*mbcm_dpp_cnm_sampling_profile_get_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  uint32                              profile_ndx, 
    SOC_SAND_OUT uint32                               *sampling_rate 
  ); 
typedef uint32 (*mbcm_dpp_cnm_cpq_pp_set_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  uint32                              cpq_ndx, 
    SOC_SAND_IN  SOC_TMC_CNM_CPQ_INFO                        *info 
  ); 
typedef uint32 (*mbcm_dpp_cnm_cpq_pp_get_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  uint32                              cpq_ndx, 
    SOC_SAND_OUT SOC_TMC_CNM_CPQ_INFO                        *info 
  ); 
typedef uint32 (*mbcm_dpp_cnm_cpq_sampling_set_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  uint32                              cpq_ndx, 
    SOC_SAND_IN  SOC_TMC_CNM_CPQ_INFO                        *info 
  ); 
typedef uint32 (*mbcm_dpp_cnm_cpq_sampling_get_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  uint32                              cpq_ndx, 
    SOC_SAND_OUT SOC_TMC_CNM_CPQ_INFO                        *info 
  ); 
typedef uint32 (*mbcm_dpp_cnm_intercept_timer_set_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  uint32                               delay 
  ); 
typedef uint32 (*mbcm_dpp_cnm_intercept_timer_get_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_OUT uint32                               *delay 
  ); 
 
 
typedef uint32 (*mbcm_dpp_cnt_counters_set_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  SOC_TMC_CNT_PROCESSOR_ID                    processor_ndx, 
    SOC_SAND_IN  SOC_TMC_CNT_COUNTERS_INFO                   *info 
  ); 

typedef uint32 (*mbcm_dpp_cnt_channel_to_fifo_mapping_set_f) (
     SOC_SAND_IN  int                             unit,
     SOC_SAND_IN  uint8                           channel,
     SOC_SAND_IN uint8                            crps_fifo);

typedef uint32 (*mbcm_dpp_cnt_channel_to_fifo_mapping_get_f)(
     SOC_SAND_IN  int                             unit,
     SOC_SAND_IN  uint8                           channel,
     SOC_SAND_OUT uint8*                          crps_fifo);

typedef uint32 (*mbcm_dpp_cnt_dma_unset_f) ( 
    SOC_SAND_IN  int                            unit, 
    SOC_SAND_IN  int                            processor_ndx, 
    SOC_SAND_IN  uint8                             channel
  ); 

typedef uint32 (*mbcm_dpp_cnt_dma_set_f) ( 
    SOC_SAND_IN  int                            unit, 
    SOC_SAND_IN  int                            dma_ndx,
    SOC_SAND_IN  uint8                             channel,
    SOC_SAND_IN  int                            cache_length
  ); 

typedef uint32 (*mbcm_dpp_cnt_counters_get_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  SOC_TMC_CNT_PROCESSOR_ID                    processor_ndx, 
    SOC_SAND_OUT SOC_TMC_CNT_COUNTERS_INFO                   *info 
  ); 
typedef uint32 (*mbcm_dpp_cnt_status_get_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  SOC_TMC_CNT_PROCESSOR_ID                    processor_ndx, 
    SOC_SAND_OUT SOC_TMC_CNT_STATUS                          *proc_status 
  ); 
typedef uint32 (*mbcm_dpp_cnt_engine_to_fifo_dma_index_f) (
     SOC_SAND_IN  int               unit,
     SOC_SAND_IN  soc_port_t        crps_index, 
     SOC_SAND_OUT int               *fifo_dma_index,
     SOC_SAND_OUT int               *cach_length
  );
typedef uint32 (*mbcm_dpp_cnt_fifo_dma_offset_in_engine_f) (
     SOC_SAND_IN  int               unit,
     SOC_SAND_IN  uint32            crps_index, 
     SOC_SAND_OUT uint32            *offset);

typedef uint32 (*mbcm_dpp_max_we_val_get_f)(
       SOC_SAND_IN  int                             unit,
       SOC_SAND_IN  ARAD_CNT_SRC_TYPE               src_type,
       SOC_SAND_OUT SOC_TMC_CNT_COUNTER_WE_BITMAP   *max_we_val);

typedef uint32 (*mbcm_dpp_cnt_algorithmic_read_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  SOC_TMC_CNT_PROCESSOR_ID                    processor_ndx, 
    SOC_SAND_IN  uint8                                          channel,
    SOC_SAND_IN  SOC_TMC_CNT_COUNTERS_INFO                   *counter_info_array,
    SOC_SAND_OUT SOC_TMC_CNT_RESULT_ARR                      *result_arr 
  ); 
typedef uint32 (*mbcm_dpp_cnt_direct_read_f) ( 
    SOC_SAND_IN  int                                         unit, 
    SOC_SAND_IN  SOC_TMC_CNT_PROCESSOR_ID                    processor_ndx, 
    SOC_SAND_IN  SOC_TMC_CNT_COUNTERS_INFO                   *counter_info,
    SOC_SAND_IN  uint32                                      counter_ndx, 
    SOC_SAND_OUT SOC_TMC_CNT_RESULT                          *read_rslt 
  ); 
typedef uint32 (*mbcm_dpp_cnt_q2cnt_id_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  SOC_TMC_CNT_PROCESSOR_ID                    processor_ndx, 
    SOC_SAND_IN  uint32                              queue_ndx, 
    SOC_SAND_OUT uint32                              *counter_ndx 
  ); 
typedef uint32 (*mbcm_dpp_cnt_cnt2q_id_f) ( 
    SOC_SAND_IN  int                   unit, 
    SOC_SAND_IN  SOC_TMC_CNT_PROCESSOR_ID         processor_ndx, 
    SOC_SAND_IN  uint32                   counter_ndx, 
    SOC_SAND_OUT uint32                   *queue_ndx 
  ); 
typedef uint32 (*mbcm_dpp_cnt_lif_counting_set_f) (
    SOC_SAND_IN int                  unit,
    SOC_SAND_IN SOC_TMC_CNT_SRC_TYPE source,
    SOC_SAND_IN int                  command_id,
    SOC_SAND_IN uint32               lif_counting_mask,
    SOC_SAND_IN int                  lif_stack_to_count
  );
typedef uint32 (*mbcm_dpp_cnt_lif_counting_get_f)(
    SOC_SAND_IN int                  unit,               
    SOC_SAND_IN SOC_TMC_CNT_SRC_TYPE source,             
    SOC_SAND_IN int                  command_id,         
    SOC_SAND_IN uint32               lif_counting_mask,  
    SOC_SAND_OUT int*                 lif_stack_to_count  
  );
typedef uint32 (*mbcm_dpp_cnt_lif_counting_range_set_f)(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN SOC_TMC_CNT_SRC_TYPE source,    
    SOC_SAND_IN int crps_counter_id, 
    SOC_SAND_IN SOC_SAND_U32_RANGE  *profile_range,    
    SOC_SAND_IN int stif_counter_id,    
    SOC_SAND_IN int is_double_entry,
    SOC_SAND_IN int engine_range_offset    
    );
typedef int (*mbcm_dpp_cnt_lif_counting_range_get_f)(
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN SOC_TMC_CNT_SRC_TYPE      source,
    SOC_SAND_IN int                       range_id,
    SOC_SAND_INOUT SOC_SAND_U32_RANGE*    range       
    );
typedef int (*mbcm_dpp_cnt_out_lif_counting_range_get_f)(
    SOC_SAND_IN int                       unit,
    SOC_SAND_INOUT SOC_SAND_U32_RANGE*    range,
    SOC_SAND_OUT uint32*                  offset,
    SOC_SAND_OUT int*                     command_id,
    SOC_SAND_OUT int*                     stif_id,
    SOC_SAND_OUT int*                     is_double_entry    
    );   
typedef uint32 (*mbcm_dpp_cnt_base_val_set_f) (
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN SOC_TMC_CNT_PROCESSOR_ID  processor_ndx,
    SOC_SAND_IN SOC_TMC_CNT_COUNTERS_INFO *info
   );
typedef uint32 (*mbcm_dpp_cnt_pni_statistics_enable_f) (
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN int                       value
   );
typedef uint32 (*mbcm_dpp_cnt_epni_regs_set_f) (
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_TMC_CNT_SRC_TYPE      src_type,
    SOC_SAND_IN  int                       src_core,
    SOC_SAND_IN  SOC_TMC_CNT_MODE_EG_TYPE  eg_mode_type,
    SOC_SAND_IN  int                       command_id,
    SOC_SAND_IN  int                       crps_config
    );

typedef uint32 (*mbcm_dpp_cnt_meter_hdr_compensation_set_f) ( 
    SOC_SAND_IN  int                   unit, 
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx, 
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  int                        counter_adjust_type,
    SOC_SAND_IN  int32                    hdr_compensation 
  ); 
typedef uint32 (*mbcm_dpp_cnt_meter_hdr_compensation_get_f) ( 
    SOC_SAND_IN  int                   unit, 
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx, 
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  int                        counter_adjust_type,
    SOC_SAND_OUT int32                    *hdr_compensation 
  ); 

 
typedef uint32 (*mbcm_dpp_diag_last_packet_info_get_f) ( 
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  int                core, 
    SOC_SAND_OUT SOC_TMC_DIAG_LAST_PACKET_INFO *last_packet 
  ); 
typedef uint32 (*mbcm_dpp_diag_sample_enable_set_f) ( 
    SOC_SAND_IN  int                               unit, 
    SOC_SAND_IN  uint8                               enable 
  ); 
typedef uint32 (*mbcm_dpp_diag_sample_enable_get_f) ( 
    SOC_SAND_IN  int                               unit, 
    SOC_SAND_OUT uint8                               *enable 
  ); 
typedef uint32 (*mbcm_dpp_diag_signals_dump_f) ( 
    SOC_SAND_IN  int  unit, 
    SOC_SAND_IN  uint32   flags 
  ); 
 
 
typedef int (*mbcm_dpp_egr_q_prio_set_f) ( 
    SOC_SAND_IN  int                                unit, 
    SOC_SAND_IN  int                                core, 
    SOC_SAND_IN  SOC_TMC_EGR_Q_PRIO_MAPPING_TYPE    map_type_ndx, 
    SOC_SAND_IN  uint32                             tc_ndx, 
    SOC_SAND_IN  uint32                             dp_ndx, 
    SOC_SAND_IN  uint32                             map_profile_ndx, 
    SOC_SAND_IN  SOC_TMC_EGR_Q_PRIORITY             *priority 
  ); 
typedef int (*mbcm_dpp_egr_q_prio_get_f) ( 
    SOC_SAND_IN  int                                unit, 
    SOC_SAND_IN  int                                core, 
    SOC_SAND_IN  SOC_TMC_EGR_Q_PRIO_MAPPING_TYPE    map_type_ndx, 
    SOC_SAND_IN  uint32                             tc_ndx, 
    SOC_SAND_IN  uint32                             dp_ndx, 
    SOC_SAND_IN  uint32                             map_profile_ndx, 
    SOC_SAND_OUT SOC_TMC_EGR_Q_PRIORITY             *priority 
  ); 
typedef int (*mbcm_dpp_egr_q_profile_map_set_f) ( 
    SOC_SAND_IN  int                      unit, 
    SOC_SAND_IN  int                      core_id, 
    SOC_SAND_IN  uint32                   tm_port, 
    SOC_SAND_IN  uint32                   map_profile_id 
  ); 
typedef int (*mbcm_dpp_egr_q_profile_map_get_f) ( 
    SOC_SAND_IN  int                      unit, 
    SOC_SAND_IN  int                      core_id, 
    SOC_SAND_IN  uint32                   tm_port, 
    SOC_SAND_OUT uint32                   *map_profile_id 
  ); 
typedef int (*mbcm_dpp_egr_q_cgm_interface_set_f) ( 
    SOC_SAND_IN  int                      unit, 
    SOC_SAND_IN  int                      core, 
    SOC_SAND_IN  uint32                   tm_port, 
    SOC_SAND_OUT uint32                   cgm_interface 
  );
typedef int (*mbcm_dpp_egr_q_fqp_scheduler_config_f) ( 
    SOC_SAND_IN  int                      unit 
  ); 
typedef uint32 (*mbcm_dpp_egr_dsp_pp_to_base_q_pair_get_f) (
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  int                      core,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_OUT uint32                   *base_q_pair
  );
typedef int (*mbcm_dpp_egr_dsp_pp_to_base_q_pair_set_f) (
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  int                      core,
    SOC_SAND_IN  uint32                   tm_port,
    SOC_SAND_IN  uint32                   base_q_pair
  );
typedef uint32 (*mbcm_dpp_egr_dsp_pp_priorities_mode_get_f) (
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID               ofp_ndx,
    SOC_SAND_OUT ARAD_EGR_PORT_PRIORITY_MODE    *priority_mode
  );
typedef int (*mbcm_dpp_egr_dsp_pp_priorities_mode_set_f) (
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  int                         core,
    SOC_SAND_IN  uint32                      tm_port,
    SOC_SAND_OUT ARAD_EGR_PORT_PRIORITY_MODE priority_mode
  );
typedef uint32 (*mbcm_dpp_egr_dsp_pp_shaper_mode_set_f) (
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  int                         coref,
    SOC_SAND_IN  uint32                      tm_port,
    SOC_SAND_OUT ARAD_EGR_PORT_SHAPER_MODE shaper_mode
  );
typedef int (*mbcm_dpp_egr_queuing_dev_set_f) (
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN SOC_TMC_EGR_QUEUING_DEV_TH    *info
  );
typedef int (*mbcm_dpp_egr_queuing_dev_get_f) (
    SOC_SAND_IN     int    unit,
    SOC_SAND_IN     int    core,
    SOC_SAND_OUT SOC_TMC_EGR_QUEUING_DEV_TH    *info
  );
typedef int (*mbcm_dpp_egr_queuing_global_drop_set_f) (
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_IN    int    threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  );
typedef int (*mbcm_dpp_egr_queuing_global_drop_get_f) (
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_OUT   int*    threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  );
typedef int (*mbcm_dpp_egr_queuing_sp_tc_drop_set_f) (
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_IN    int    threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  );
typedef int (*mbcm_dpp_egr_queuing_sp_tc_drop_get_f) (
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_OUT   int*   threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  );
typedef uint32 (*mbcm_dpp_egr_queuing_sch_unsch_drop_set_f) (
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    threshold_type,
    SOC_SAND_IN    SOC_TMC_EGR_QUEUING_DEV_TH *dev_thresh
  );
typedef uint32 (*mbcm_dpp_egr_queuing_sch_unsch_drop_get_unsafe_f) (
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    threshold_type,
    SOC_SAND_OUT   SOC_TMC_EGR_QUEUING_DEV_TH *dev_thresh
  );  
typedef int (*mbcm_dpp_egr_queuing_sp_reserved_set_f) (
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_IN    int    threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  );
                                                   
typedef int (*mbcm_dpp_egr_queuing_sp_reserved_get_f) (
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_OUT   int*    threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  );
                                                   
typedef int (*mbcm_dpp_egr_queuing_global_fc_set_f) (
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_IN    int    threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  );

typedef int (*mbcm_dpp_egr_queuing_global_fc_get_f) (
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_OUT   int*   threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  );

typedef int (*mbcm_dpp_egr_queuing_mc_tc_fc_set_f) (
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_IN    int    threshold_value
  );

typedef int (*mbcm_dpp_egr_queuing_mc_tc_fc_get_f) (
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_OUT   int*    threshold_value
  );

typedef int (*mbcm_dpp_egr_queuing_mc_cos_map_set_f)(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    uint32    tc_ndx,
    SOC_SAND_IN    uint32    dp_ndx,
    SOC_SAND_IN ARAD_EGR_QUEUING_MC_COS_MAP    *info
  );
typedef uint32 (*mbcm_dpp_egr_queuing_mc_cos_map_get_f)(
    SOC_SAND_IN     int    unit,
    SOC_SAND_IN     int    core,
    SOC_SAND_IN     uint32    tc_ndx,
    SOC_SAND_IN     uint32    dp_ndx,
    SOC_SAND_OUT ARAD_EGR_QUEUING_MC_COS_MAP    *info
  );
typedef int (*mbcm_dpp_egr_queuing_if_fc_set_f)(
    SOC_SAND_IN     int    unit,
    SOC_SAND_IN     ARAD_INTERFACE_ID    if_ndx,
    SOC_SAND_IN  ARAD_EGR_QUEUING_IF_FC    *info
  );
typedef uint32 (*mbcm_dpp_egr_queuing_if_fc_get_f)(
    SOC_SAND_IN     int    unit,
    SOC_SAND_IN     ARAD_INTERFACE_ID    if_ndx,
    SOC_SAND_OUT ARAD_EGR_QUEUING_IF_FC    *info
  );
typedef int (*mbcm_dpp_egr_queuing_if_fc_uc_max_set_f)(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    uint32    uc_if_profile_ndx,
    SOC_SAND_IN ARAD_EGR_QUEUING_IF_UC_FC    *info
  );

typedef int (*mbcm_dpp_egr_queuing_if_fc_uc_set_f)(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    uint32    uc_if_profile_ndx,
    SOC_SAND_IN ARAD_EGR_QUEUING_IF_UC_FC    *info
  );
typedef int (*mbcm_dpp_egr_queuing_if_fc_uc_get_f)(
    SOC_SAND_IN     int    unit,
    SOC_SAND_IN     int    core,
    SOC_SAND_IN     uint32    uc_if_profile_ndx,
    SOC_SAND_OUT ARAD_EGR_QUEUING_IF_UC_FC    *info
  );
typedef int (*mbcm_dpp_egr_queuing_if_fc_mc_set_f)(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    uint32    mc_if_profile_ndx,
    SOC_SAND_IN uint32    pd_th
  );
typedef int (*mbcm_dpp_egr_queuing_if_fc_mc_get_f)(
    SOC_SAND_IN     int    unit,
    SOC_SAND_IN     uint32    mc_if_profile_ndx,
    SOC_SAND_OUT uint32   *pd_th
  );

typedef uint32 (*mbcm_dpp_egr_queuing_if_uc_map_set_f) (
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  int        core,
    SOC_SAND_IN  soc_port_if_t interface_type,
    SOC_SAND_IN  uint32     internal_if_id,
    SOC_SAND_IN  int        profile
  );

typedef uint32 (*mbcm_dpp_egr_queuing_if_mc_map_set_f) (
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  int        core,
    SOC_SAND_IN  soc_port_if_t interface_type,
    SOC_SAND_IN  uint32     internal_if_id,
    SOC_SAND_IN  int        profile
  );

typedef uint32 (*mbcm_dpp_egr_queuing_ofp_tcg_set_f) (
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  int                      core,
    SOC_SAND_IN  uint32                   tm_port,    
    SOC_SAND_IN  SOC_TMC_EGR_QUEUING_TCG_INFO      *tcg_info
  );

typedef uint32 (*mbcm_dpp_egr_queuing_ofp_tcg_get_f) (
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  int                      core,
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID               tm_port,    
    SOC_SAND_OUT SOC_TMC_EGR_QUEUING_TCG_INFO      *tcg_info
  );

typedef uint32 (*mbcm_dpp_egr_queuing_tcg_weight_set_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core,
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID          tm_port,
    SOC_SAND_IN  SOC_TMC_TCG_NDX              tcg_ndx,
    SOC_SAND_IN  SOC_TMC_EGR_TCG_SCH_WFQ      *tcg_weight
  );

typedef uint32 (*mbcm_dpp_egr_queuing_tcg_weight_get_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core,
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID          tm_port,
    SOC_SAND_IN  SOC_TMC_TCG_NDX              tcg_ndx,
    SOC_SAND_OUT SOC_TMC_EGR_TCG_SCH_WFQ      *tcg_weight
  );

typedef int (*mbcm_dpp_egr_queuing_is_high_priority_port_get_f) (
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  int     core,
    SOC_SAND_IN  uint32  tm_port,
    SOC_SAND_OUT int     *is_high_priority
  );


typedef int (*mbcm_dpp_egr_queuing_init_thresholds_f) (
    SOC_SAND_IN int unit,
    SOC_SAND_IN int port_rate,
    SOC_SAND_IN int nof_priorities,
    SOC_SAND_IN int nof_channels,
    SOC_SAND_OUT SOC_TMC_EGR_QUEUING_CGM_INIT_THRESHOLDS* cgm_init_thresholds
    );

typedef int (*mbcm_dpp_egr_queuing_egr_interface_alloc_f) (
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  soc_port_t    port
  );

typedef int (*mbcm_dpp_egr_queuing_egr_interface_free_f) (
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  soc_port_t    port
  );

 
typedef int (*mbcm_dpp_egr_queuing_nrdy_th_profile_data_set_f) ( 
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core,
    SOC_SAND_IN uint32 profile,
    SOC_SAND_IN uint32 profile_data
    );

typedef int (*mbcm_dpp_egr_queuing_nrdy_th_profile_data_get_f) ( 
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core,
    SOC_SAND_IN uint32 profile,
    SOC_SAND_OUT uint32 *profile_data
    );

typedef int (*mbcm_dpp_egr_congestion_statistics_get_f)(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core,
    SOC_SAND_INOUT ARAD_EGR_CGM_CONGENSTION_STATS *cur_stats,   
    SOC_SAND_INOUT ARAD_EGR_CGM_CONGENSTION_STATS *max_stats,   
    SOC_SAND_INOUT ARAD_EGR_CGM_CONGENSTION_COUNTERS *counters, 
    SOC_SAND_IN  int disable_updates 
    );

typedef uint32 (*mbcm_dpp_ofp_rates_max_credit_empty_port_set_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                    arg
  );
typedef uint32 (*mbcm_dpp_ofp_rates_max_credit_empty_port_get_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT  int*                    arg
  );

 
typedef uint32 (*mbcm_dpp_fabric_line_coding_set_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  uint32                              link_ndx, 
    SOC_SAND_IN  SOC_TMC_FABRIC_LINE_CODING                  coding 
  ); 
typedef uint32 (*mbcm_dpp_fabric_line_coding_get_f) ( 
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  uint32                              link_ndx, 
    SOC_SAND_OUT SOC_TMC_FABRIC_LINE_CODING                  *coding 
  );
typedef soc_error_t (*mbcm_dpp_fabric_pcp_dest_mode_config_set_f) (
    SOC_SAND_IN int                             unit,
    SOC_SAND_IN uint32                              flags,
    SOC_SAND_IN uint32                                  modid,
    SOC_SAND_IN uint32                                  pcp_mode
  );
typedef soc_error_t (*mbcm_dpp_fabric_pcp_dest_mode_config_get_f) (
    SOC_SAND_IN int                             unit,
    SOC_SAND_IN uint32                              flags,
    SOC_SAND_IN uint32                                  modid,
    SOC_SAND_OUT uint32                                 *pcp_mode
  );

typedef int (*mbcm_dpp_port_fabric_clk_freq_init_f) (
    SOC_SAND_IN int                             unit,
    SOC_SAND_IN soc_pbmp_t                      pbmp
  );

typedef soc_error_t (*mbcm_dpp_fabric_priority_set_f) (
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  uint32                              tc, 
    SOC_SAND_IN  uint32                              dp,
    SOC_SAND_IN  uint32                              flags,
    SOC_SAND_IN  int                                 fabric_priority
  ); 

typedef soc_error_t (*mbcm_dpp_fabric_priority_get_f) (
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  uint32                              tc, 
    SOC_SAND_IN  uint32                              dp,
    SOC_SAND_IN  uint32                              flags,
    SOC_SAND_OUT int                                 *fabric_priority
  ); 


typedef uint32 (*mbcm_dpp_fabric_link_thresholds_pipe_set_f) (
    SOC_SAND_IN int                                  unit,
    SOC_SAND_IN soc_dpp_fabric_pipe_t                pipe,  
    SOC_SAND_IN soc_dpp_fabric_link_threshold_type_t type, 
    SOC_SAND_IN int                                  value
  );

typedef uint32 (*mbcm_dpp_fabric_link_thresholds_pipe_get_f) (
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  soc_dpp_fabric_pipe_t                pipe,  
    SOC_SAND_IN  soc_dpp_fabric_link_threshold_type_t type, 
    SOC_SAND_OUT int                                  *value
  );

typedef soc_error_t (*mbcm_dpp_fabric_cosq_control_backward_flow_control_set_f) (
    SOC_SAND_IN int                                   unit,
    SOC_SAND_IN soc_gport_t                           port,
    SOC_SAND_IN int                                   enable,
    SOC_SAND_IN soc_dpp_cosq_gport_egress_core_fifo_t fifo_type
  );

typedef soc_error_t (*mbcm_dpp_fabric_cosq_control_backward_flow_control_get_f) (
    SOC_SAND_IN int                                   unit,
    SOC_SAND_IN soc_gport_t                           port,
    SOC_SAND_OUT int                                  *enable,
    SOC_SAND_IN soc_dpp_cosq_gport_egress_core_fifo_t fifo_type
  );

typedef soc_error_t (*mbcm_dpp_fabric_egress_core_cosq_gport_sched_set_f) (
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_IN  int                                weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t   fifo_type
  );

typedef soc_error_t (*mbcm_dpp_fabric_egress_core_cosq_gport_sched_get_f) (
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_OUT int                                *weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t   fifo_type
  );

typedef soc_error_t (*mbcm_dpp_fabric_cosq_gport_rci_threshold_set_f) (
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  bcm_gport_t            gport,
    SOC_SAND_IN  int                    threshold_val,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  );

typedef soc_error_t (*mbcm_dpp_fabric_cosq_gport_rci_threshold_get_f) (
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport,
    SOC_SAND_OUT int                    *threshold_val,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  );

typedef soc_error_t (*mbcm_dpp_fabric_cosq_gport_priority_drop_threshold_set_f) (
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  soc_gport_t                            gport,
    SOC_SAND_IN  soc_cosq_threshold_t                   *threshold,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  );
 
typedef soc_error_t (*mbcm_dpp_fabric_cosq_gport_priority_drop_threshold_get_f) (
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  soc_gport_t                            gport,
    SOC_SAND_INOUT  soc_cosq_threshold_t                *threshold,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  ); 

typedef uint32 (*mbcm_dpp_fabric_link_topology_set_f) (
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  soc_module_t                           destination, 
    SOC_SAND_IN  int                                    links_count,
    SOC_SAND_IN  soc_port_t                             *links_array
  ); 

typedef uint32 (*mbcm_dpp_fabric_link_topology_get_f) (
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  soc_module_t                           destination, 
    SOC_SAND_IN  int                                    max_links_count,
    SOC_SAND_OUT int                                    *links_count, 
    SOC_SAND_OUT soc_port_t                             *links_array
  ); 

typedef uint32 (*mbcm_dpp_fabric_link_topology_unset_f) (
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  int                                    local_dest_id
  );

typedef int (*mbcm_dpp_fabric_static_replication_set_f) (
    int                                                 unit, 
    bcm_port_t                                          port, 
    uint32                                              flags, 
    uint32                                              destid_count, 
    bcm_module_t                                        *destid_array
  );

typedef soc_error_t (*mbcm_dpp_fabric_multicast_set_f) (
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  soc_multicast_t                        mc_id,
    SOC_SAND_IN  uint32                                 destid_count,
    SOC_SAND_IN  soc_module_t                           *destid_array
  ); 

typedef uint32 (*mbcm_dpp_fabric_multicast_get_f) (
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  soc_multicast_t                        mc_id,
    SOC_SAND_IN  uint32                                 destid_count_max,
    SOC_SAND_OUT int                                    *destid_count,
    SOC_SAND_OUT soc_module_t                           *destid_array
  ); 

typedef uint32 (*mbcm_dpp_fabric_modid_group_set_f) (
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  soc_module_t                           group,
    SOC_SAND_IN  int                                    modid_count,
    SOC_SAND_IN  soc_module_t                           *modid_array
  ); 

typedef uint32 (*mbcm_dpp_fabric_modid_group_get_f) (
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  soc_module_t                           gport,
    SOC_SAND_IN  int                                    modid_max_count,
    SOC_SAND_OUT  soc_module_t                           *modid_array,
    SOC_SAND_OUT  int                                    *modid_count
  ); 

typedef uint32 (*mbcm_dpp_fabric_local_dest_id_verify_f) (
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  int                                    local_dest
  );
  
typedef uint32 (*mbcm_dpp_fabric_rci_thresholds_config_set_f) (
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  soc_dpp_fabric_rci_config_t            rci_config
  ); 
 
typedef uint32 (*mbcm_dpp_fabric_rci_thresholds_config_get_f) (
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_OUT soc_dpp_fabric_rci_config_t            *rci_config
  ); 

typedef uint32 (*mbcm_dpp_fabric_link_repeater_enable_set_f) (
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  soc_port_t                             port,
    SOC_SAND_IN  int                                    enable,
    SOC_SAND_IN  int                                    empty_cell_size
  );

typedef uint32 (*mbcm_dpp_fabric_link_repeater_enable_get_f) (
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  soc_port_t                             port,
    SOC_SAND_OUT int                                    *enable,
    SOC_SAND_OUT int                                    *empty_cell_size
  );

typedef uint32 (*mbcm_dpp_fabric_queues_info_get_f) (
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_OUT soc_dpp_fabric_queues_info_t           *queues_info
  );

typedef uint32 (*mbcm_dpp_fabric_cpu2cpu_write_f) (
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN dcmn_sr_cell_link_list_t                *sr_link_list,
    SOC_SAND_IN uint32                                  data_in_size,
    SOC_SAND_IN uint32                                  *data_in
  );
 
typedef uint32 (*mbcm_dpp_fabric_mesh_topology_get_f) (
    SOC_SAND_IN  int                                     unit, 
    SOC_SAND_OUT soc_dcmn_fabric_mesh_topology_diag_t    *mesh_topology_diag
  );

typedef uint32 (*mbcm_dpp_fabric_mesh_check_f) (
    SOC_SAND_IN  int                                     unit, 
    SOC_SAND_IN uint8                                    stand_alone,
    SOC_SAND_OUT uint8                                   *success
  );
 
typedef uint32 (*mbcm_dpp_fabric_rx_fifo_status_get_f) (
    SOC_SAND_IN  int                                unit, 
    SOC_SAND_OUT soc_dpp_fabric_rx_fifo_diag_t     *rx_fifo_diag
  );

typedef uint32 (*mbcm_dpp_fabric_port_sync_e_link_set_f) (
    SOC_SAND_IN  int                                unit, 
    SOC_SAND_IN  int                                is_master, 
    SOC_SAND_IN  int                                port
  );

typedef uint32 (*mbcm_dpp_fabric_port_sync_e_link_get_f) (
    SOC_SAND_IN  int                                unit, 
    SOC_SAND_IN  int                                is_master, 
    SOC_SAND_OUT int                                *port
  );

typedef uint32 (*mbcm_dpp_fabric_port_sync_e_divider_set_f) (
    SOC_SAND_IN  int                                unit, 
    SOC_SAND_IN  int                                is_master,
    SOC_SAND_IN  int                                devider
  );

typedef uint32 (*mbcm_dpp_fabric_port_sync_e_divider_get_f) (
    SOC_SAND_IN  int                                unit, 
    SOC_SAND_IN  int                                is_master,
    SOC_SAND_OUT int                                *devider
  );

typedef uint32 (*mbcm_dpp_fabric_sync_e_enable_get_f) (
    SOC_SAND_IN  int                                unit, 
    SOC_SAND_IN  int                                is_master, 
    SOC_SAND_OUT int                                *is_fabric_synce
  );
typedef soc_error_t (*mbcm_dpp_fabric_force_set_f) (
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN soc_dpp_fabric_force_t       force
  ); 
 


typedef int (*mbcm_dpp_pll_info_get_f) (
    SOC_SAND_IN  int                unit, 
    SOC_SAND_OUT  soc_dpp_pll_info_t *pll_info 
  );  
typedef uint32 (*mbcm_dpp_interface_id_verify_f) ( 
    SOC_SAND_IN  SOC_TMC_INTERFACE_ID    if_ndx 
  ); 
typedef uint32 (*mbcm_dpp_if_type_from_id_f) ( 
    SOC_SAND_IN  SOC_TMC_INTERFACE_ID   soc_pb_nif_id, 
    SOC_SAND_OUT SOC_TMC_INTERFACE_TYPE *interface_type 
  ); 
typedef uint32 (*mbcm_dpp_mal_equivalent_id_verify_f) ( 
    SOC_SAND_IN  uint32 mal_ndx 
  ); 
typedef uint32 (*mbcm_dpp_ipt_init_f) (
    SOC_SAND_IN  int                  unit
  );
typedef int (*mbcm_dpp_stat_fabric_init_f) (
    SOC_SAND_IN  int                  unit
  );
typedef int (*mbcm_dpp_stat_nif_init_f) (
    SOC_SAND_IN  int                  unit 
  );

typedef int (*mbcm_dpp_stat_path_info_get_f) (
    int unit,
    soc_dpp_stat_path_info_t *info
);
                                              
typedef int (*mbcm_dpp_info_config_custom_reg_access_f) (
    SOC_SAND_IN  int                  unit 
  );
typedef int (*mbcm_dpp_mapping_stat_get_f) (
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  soc_port_t           port,
    SOC_SAND_OUT uint32               *counters,
    SOC_SAND_OUT int                  *array_size,
    SOC_SAND_IN  bcm_stat_val_t       type,
    SOC_SAND_IN  int                  max_array_size
  );

typedef soc_error_t (*mbcm_dpp_stat_counter_length_get_f) (
    int unit,
    int counter_id,
    int * length
);

typedef soc_error_t (*mbcm_dpp_stat_controlled_counter_enable_get_f) (
    int unit,
    soc_port_t port,
    int counter_id,
    int *enable,
    int *printable
);

 
typedef int (*mbcm_dpp_itm_committed_q_size_set_f) ( 
    SOC_SAND_IN  int                  unit, 
    SOC_SAND_IN  uint32                  rt_cls4_ndx, 
    SOC_SAND_IN  SOC_TMC_ITM_GUARANTEED_INFO *info, 
    SOC_SAND_OUT SOC_TMC_ITM_GUARANTEED_INFO *exact_info 
  ); 
typedef int (*mbcm_dpp_itm_committed_q_size_get_f) ( 
    SOC_SAND_IN  int                  unit, 
    SOC_SAND_IN  uint32                  rt_cls4_ndx, 
    SOC_SAND_OUT SOC_TMC_ITM_GUARANTEED_INFO *exact_info 
  ); 
typedef int (* mbcm_dpp_itm_max_latency_pkts_get_f)(
    int unit,
    int max_count,
    SOC_TMC_MAX_LATENCY_PACKETS_INFO *max_latency_packets, 
    int *actual_count
  );   
typedef int (*mbcm_dpp_itm_rate_limit_mpps_set_f) (int unit, int rate);
typedef int (*mbcm_dpp_itm_rate_limit_mpps_get_f) (int unit, int* rate);

 
typedef uint32 (*mbcm_dpp_port_pp_port_set_f) ( 
    SOC_SAND_IN  int                    unit, 
    SOC_SAND_IN  int                    core_id, 
    SOC_SAND_IN  uint32                 pp_port_ndx,
    SOC_SAND_IN  SOC_TMC_PORT_PP_PORT_INFO         *info, 
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success 
  ); 
typedef uint32 (*mbcm_dpp_port_pp_port_get_f) ( 
    SOC_SAND_IN  int                        unit, 
    SOC_SAND_IN  int                        core_id,
    SOC_SAND_IN  uint32                     pp_port_ndx, 
    SOC_SAND_OUT SOC_TMC_PORT_PP_PORT_INFO  *info 
  ); 
typedef uint32 (*mbcm_dpp_port_to_pp_port_map_set_f) ( 
    SOC_SAND_IN  int                    unit, 
    SOC_SAND_IN  soc_port_t             port, 
    SOC_SAND_IN  SOC_TMC_PORT_DIRECTION direction_ndx  
  ); 
typedef uint32 (*mbcm_dpp_port_to_pp_port_map_get_f) ( 
    SOC_SAND_IN  int                    unit, 
    SOC_SAND_IN  uint32                 port_ndx, 
    SOC_SAND_OUT uint32                 *pp_port_in, 
    SOC_SAND_OUT uint32                 *pp_port_out 
  );
typedef soc_error_t (*mbcm_dpp_port_control_low_latency_set_f) (
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  soc_port_t              port,
    SOC_SAND_IN  int                     value
 );
typedef soc_error_t (*mbcm_dpp_port_control_fec_error_detect_set_f) (
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  soc_port_t              port,
    SOC_SAND_IN  int                     value
 );
typedef soc_error_t (*mbcm_dpp_port_control_low_latency_get_f) (
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  soc_port_t              port,
    SOC_SAND_OUT int                     *value
 );
typedef soc_error_t (*mbcm_dpp_port_control_fec_error_detect_get_f) (
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  soc_port_t              port,
    SOC_SAND_OUT int                     *value
 );
typedef soc_error_t (*mbcm_dpp_port_extract_cig_from_llfc_enable_set_f) (
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  soc_port_t              port,
    SOC_SAND_IN  int                     value
 );
typedef soc_error_t (*mbcm_dpp_port_extract_cig_from_llfc_enable_get_f) (
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  soc_port_t              port,
    SOC_SAND_OUT int                     *value
 );
typedef uint32 (*mbcm_dpp_port_forwarding_header_set_f) ( 
    SOC_SAND_IN  int                       unit, 
    SOC_SAND_IN  uint32                       pp_port_ndx, 
    SOC_SAND_IN  SOC_TMC_PORTS_FORWARDING_HEADER_INFO *info 
  ); 
typedef uint32 (*mbcm_dpp_port_forwarding_header_get_f) ( 
    SOC_SAND_IN  int                       unit, 
    SOC_SAND_IN  uint32                       pp_port_ndx, 
    SOC_SAND_OUT SOC_TMC_PORTS_FORWARDING_HEADER_INFO *info 
  );
 
typedef int (*mbcm_dpp_port_nrdy_th_profile_set_f) ( 
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_IN uint32 profile
  );

typedef int (*mbcm_dpp_port_nrdy_th_profile_get_f) ( 
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_OUT uint32 *profile
  ); 

typedef int (*mbcm_dpp_port_nrdy_th_optimial_value_get_f) ( 
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_OUT uint32 *value
  ); 
 
 
typedef uint32 (*mbcm_dpp_ports_logical_sys_id_build_with_device_f) (
    SOC_SAND_IN  int                       unit, 
    SOC_SAND_IN  uint8 is_lag_not_phys,
    SOC_SAND_IN  uint32  lag_id,
    SOC_SAND_IN  uint32  lag_member_id,
    SOC_SAND_IN  uint32  sys_phys_port_id,
    SOC_SAND_OUT uint32  *sys_logic_port_id
  ); 
typedef uint32 (*mbcm_dpp_sys_virtual_port_to_local_port_map_set_unsafe_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 sys_phys_port_ndx, 
    SOC_SAND_IN  uint32                 mapped_fap_id, 
    SOC_SAND_IN  uint32                 mapped_fap_port_id 
  ); 
 
 
typedef uint32 (*mbcm_dpp_ports_logical_sys_id_parse_f) (
    SOC_SAND_IN  int                       unit, 
    SOC_SAND_IN uint32 sys_logic_port_id,
    SOC_SAND_OUT  uint8 * is_lag_not_phys,
    SOC_SAND_OUT  uint32 * lag_id,
    SOC_SAND_OUT  uint32  * lag_member_id,
    SOC_SAND_OUT  uint32  * sys_phys_port_id
  ); 
 
 
typedef uint32 (*mbcm_dpp_ssr_buff_size_get_f) ( 
    SOC_SAND_IN  int               unit, 
    SOC_SAND_OUT uint32                       *byte_size_ptr 
  ); 
typedef uint32 (*mbcm_dpp_ssr_to_buff_f) ( 
    SOC_SAND_IN  int                      unit, 
    SOC_SAND_OUT uint8                        *store_buff, 
    SOC_SAND_IN  uint32                       buff_byte_size 
  ); 
typedef uint32 (*mbcm_dpp_ssr_from_buff_f) ( 
    SOC_SAND_IN  int      unit, 
    SOC_SAND_IN  uint8        *store_buff, 
    SOC_SAND_IN  uint32       buff_byte_size 
  ); 
typedef uint32 (*mbcm_dpp_ssr_is_device_init_done_f) ( 
    SOC_SAND_IN  int unit, 
    SOC_SAND_OUT uint8 *is_init_done 
  ); 
 
 
typedef uint32 (*mbcm_dpp_stack_global_info_set_f) ( 
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  SOC_TMC_STACK_GLBL_INFO       *info 
  ); 
typedef uint32 (*mbcm_dpp_stack_global_info_get_f) ( 
    SOC_SAND_IN  int                unit, 
    SOC_SAND_OUT SOC_TMC_STACK_GLBL_INFO       *info 
  ); 
typedef uint32 (*mbcm_dpp_stack_port_distribution_info_set_f) ( 
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  uint32                local_stack_port_ndx, 
    SOC_SAND_IN  SOC_TMC_STACK_PORT_DISTR_INFO *distribution_info 
  ); 
typedef uint32 (*mbcm_dpp_stack_port_distribution_info_get_f) ( 
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  uint32                local_stack_port_ndx, 
    SOC_SAND_OUT SOC_TMC_STACK_PORT_DISTR_INFO *distribution_info 
  );
typedef uint32 (*mbcm_dpp_fabric_stack_module_all_reachable_ignore_id_set_f) ( 
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  soc_module_t       module, 
    SOC_SAND_IN  int                enable
  );
typedef uint32 (*mbcm_dpp_fabric_stack_module_all_reachable_ignore_id_get_f) ( 
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  soc_module_t       module, 
    SOC_SAND_OUT int                *enable
  );
typedef uint32 (*mbcm_dpp_fabric_stack_module_max_all_reachable_set_f) (
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  soc_module_t       max_module
  );
typedef uint32 (*mbcm_dpp_fabric_stack_module_max_all_reachable_get_f) (
    SOC_SAND_IN  int                unit, 
    SOC_SAND_OUT  soc_module_t       *max_module
  );
typedef uint32 (*mbcm_dpp_fabric_stack_module_max_set_f) ( 
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  soc_module_t       max_module
  );
typedef uint32 (*mbcm_dpp_fabric_stack_module_max_get_f) ( 
    SOC_SAND_IN  int                unit, 
    SOC_SAND_OUT soc_module_t       *max_module
  );
typedef uint32 (*mbcm_dpp_fabric_stack_module_devide_by_32_verify_f) ( 
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN soc_module_t       max_module
  ); 

 
typedef uint32 (*mbcm_dpp_tdm_ftmh_set_f) ( 
    SOC_SAND_IN  int                   unit, 
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx, 
    SOC_SAND_IN  SOC_TMC_TDM_FTMH_INFO            *info 
  ); 
typedef uint32 (*mbcm_dpp_tdm_ftmh_get_f) ( 
    SOC_SAND_IN  int                   unit, 
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx, 
    SOC_SAND_OUT SOC_TMC_TDM_FTMH_INFO            *info 
  ); 
typedef uint32 (*mbcm_dpp_tdm_opt_size_set_f) ( 
    SOC_SAND_IN  int                   unit, 
    SOC_SAND_IN  uint32                    cell_size 
  ); 
typedef uint32 (*mbcm_dpp_tdm_opt_size_get_f) ( 
    SOC_SAND_IN  int                   unit, 
    SOC_SAND_OUT uint32                    *cell_size 
  ); 
typedef uint32 (*mbcm_dpp_tdm_stand_size_range_set_f) ( 
    SOC_SAND_IN  int                   unit, 
    SOC_SAND_IN  SOC_SAND_U32_RANGE              *size_range 
  ); 
typedef uint32 (*mbcm_dpp_tdm_stand_size_range_get_f) ( 
    SOC_SAND_IN  int                   unit, 
    SOC_SAND_OUT SOC_SAND_U32_RANGE              *size_range 
  ); 
typedef uint32 (*mbcm_dpp_tdm_mc_static_route_set_f) ( 
    SOC_SAND_IN  int                   unit, 
    SOC_SAND_IN  uint32                   mc_id_route_ndx, 
    SOC_SAND_IN  SOC_TMC_TDM_MC_STATIC_ROUTE_INFO *route_info 
  ); 
typedef uint32 (*mbcm_dpp_tdm_mc_static_route_get_f) ( 
    SOC_SAND_IN  int                   unit, 
    SOC_SAND_IN  uint32                   mc_id_route_ndx, 
    SOC_SAND_OUT SOC_TMC_TDM_MC_STATIC_ROUTE_INFO *route_info 
  ); 
typedef uint32 (*mbcm_dpp_tdm_port_packet_crc_set_f) ( 
    SOC_SAND_IN  int                   unit, 
    SOC_SAND_IN  soc_port_t               port_ndx, 
    SOC_SAND_IN  uint8                    is_enable,
    SOC_SAND_IN  uint8                    configure_ingress,
    SOC_SAND_IN  uint8                    configure_egress
  ); 
typedef uint32 (*mbcm_dpp_tdm_port_packet_crc_get_f) ( 
    SOC_SAND_IN  int                   unit, 
    SOC_SAND_IN  soc_port_t               port_ndx, 
    SOC_SAND_OUT uint8                    *is_ingress_enabled,
    SOC_SAND_OUT uint8                    *is_egress_enabled
  ); 
typedef uint32 (*mbcm_dpp_tdm_direct_routing_set_f) ( 
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   direct_routing_profile,
    SOC_SAND_IN  SOC_TMC_TDM_DIRECT_ROUTING_INFO *direct_routing_info,
    SOC_SAND_IN  uint8 enable
  ); 
typedef uint32 (*mbcm_dpp_tdm_direct_routing_get_f) ( 
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   direct_routing_profile,
    SOC_SAND_OUT SOC_TMC_TDM_DIRECT_ROUTING_INFO *direct_routing_info,
    SOC_SAND_OUT uint8 *enable
  ); 
typedef uint32 (*mbcm_dpp_tdm_direct_routing_profile_map_set_f) ( 
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  uint32                   direct_routing_profile
  ); 
typedef uint32 (*mbcm_dpp_tdm_direct_routing_profile_map_get_f) ( 
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT uint32                   *direct_routing_profile
  ); 
typedef int (*mbcm_dpp_tdm_ifp_get_f) (
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  soc_port_t            port,
    SOC_SAND_OUT uint8                 *is_tdm
  );
typedef int (*mbcm_dpp_tdm_ifp_set_f) (
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  soc_port_t            port,
    SOC_SAND_IN  uint8                 is_tdm
  );
 
typedef uint32 (*mbcm_dpp_read_from_fe600_f) ( 
    SOC_SAND_IN  int                       unit, 
    SOC_SAND_IN  SOC_TMC_SR_CELL_LINK_LIST        *sr_link_list, 
    SOC_SAND_IN  uint32                        size, 
    SOC_SAND_IN  uint32                        offset, 
    SOC_SAND_OUT uint32                        *data_out 
  ); 
typedef uint32 (*mbcm_dpp_write_to_fe600_f) ( 
    SOC_SAND_IN  int                       unit, 
    SOC_SAND_IN  SOC_TMC_SR_CELL_LINK_LIST        *sr_link_list, 
    SOC_SAND_IN  uint32                        size, 
    SOC_SAND_IN  uint32                        offset, 
    SOC_SAND_IN  uint32                        *data_in 
  ); 
typedef uint32 (*mbcm_dpp_indirect_read_from_fe600_f) ( 
    SOC_SAND_IN  int                       unit, 
    SOC_SAND_IN  SOC_TMC_SR_CELL_LINK_LIST        *sr_link_list, 
    SOC_SAND_IN  uint32                        size, 
    SOC_SAND_IN  uint32                        offset, 
    SOC_SAND_OUT uint32                        *data_out 
  ); 
typedef uint32 (*mbcm_dpp_indirect_write_to_fe600_f) ( 
    SOC_SAND_IN  int                       unit, 
    SOC_SAND_IN  SOC_TMC_SR_CELL_LINK_LIST          *sr_link_list, 
    SOC_SAND_IN  uint32                        size, 
    SOC_SAND_IN  uint32                        offset, 
    SOC_SAND_IN  uint32                        *data_in 
  ); 
typedef uint32 (*mbcm_dpp_cpu2cpu_write_f) ( 
    SOC_SAND_IN  int                       unit, 
    SOC_SAND_IN  SOC_TMC_SR_CELL_LINK_LIST         *sr_link_list, 
    SOC_SAND_IN  uint32                        size, 
    SOC_SAND_IN  uint32                        *data_in 
  ); 
typedef uint32 (*mbcm_dpp_cpu2cpu_read_f) ( 
    SOC_SAND_IN  int                       unit, 
    SOC_SAND_OUT uint32                        *data_out 
  ); 
typedef uint32 (*mbcm_dpp_cell_mc_tbl_write_f) ( 
    SOC_SAND_IN  int                                 unit, 
    SOC_SAND_IN  uint32                                 mc_id_ndx, 
    SOC_SAND_IN  SOC_TMC_CELL_MC_TBL_INFO                    *info, 
    SOC_SAND_IN  SOC_TMC_CELL_MC_TBL_DATA                    *tbl_data 
  ); 
typedef uint32 (*mbcm_dpp_cell_mc_tbl_read_f) ( 
    SOC_SAND_IN  int                                 unit, 
    SOC_SAND_IN  uint32                                 mc_id_ndx, 
    SOC_SAND_IN  SOC_TMC_CELL_MC_TBL_INFO                    *info, 
    SOC_SAND_OUT SOC_TMC_CELL_MC_TBL_DATA                    *tbl_data 
  ); 
 
 
typedef uint32 (*mbcm_dpp_diag_ipt_rate_get_f) ( 
    SOC_SAND_IN  int                unit, 
    SOC_SAND_OUT SOC_SAND_64CNT               *rate 
  ); 
typedef uint32 (*mbcm_dpp_diag_iddr_set_f) ( 
    SOC_SAND_IN  int    unit, 
    SOC_SAND_IN  uint32    dram_ndx, 
    SOC_SAND_IN  uint32    bank_ndx, 
    SOC_SAND_IN  uint32     dram_offset, 
    SOC_SAND_IN  uint32     *data, 
    SOC_SAND_IN  uint32     size 
  ); 
typedef uint32 (*mbcm_dpp_diag_iddr_get_f) ( 
    SOC_SAND_IN  int    unit, 
    SOC_SAND_IN  uint32    dram_ndx, 
    SOC_SAND_IN  uint32    bank_ndx, 
    SOC_SAND_IN  uint32     dram_offset, 
    SOC_SAND_OUT uint32     *data, 
    SOC_SAND_IN  uint32     size 
  ); 
typedef uint32 (*mbcm_dpp_diag_regs_dump_f) ( 
    SOC_SAND_IN int unit, 
    SOC_SAND_IN uint32  start, 
    SOC_SAND_IN uint32  end 
  ); 
typedef uint32 (*mbcm_dpp_diag_tbls_dump_f) ( 
    SOC_SAND_IN  int  unit, 
    SOC_SAND_IN  uint8    block_id, 
    SOC_SAND_IN  uint32   tbl_base_addr, 
    SOC_SAND_IN  uint8  print_zero 
  ); 
typedef uint32 (*mbcm_dpp_diag_tbls_dump_all_f) ( 
    SOC_SAND_IN  int  unit, 
    SOC_SAND_IN  uint8  print_zero 
  ); 
typedef uint32 (*mbcm_dpp_nif_diag_last_packet_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_OUT SOC_TMC_PORTS_ITMH          *last_incoming_header 
  ); 
typedef uint32 (*mbcm_dpp_diag_soft_error_test_start_f) ( 
    SOC_SAND_IN  int                           unit, 
    SOC_SAND_IN  SOC_TMC_DIAG_SOFT_ERR_INFO            *info 
  ); 
typedef uint32 (*mbcm_dpp_egq_resources_print_f) ( 
    int unit 
  ); 
 
 
typedef uint32 (*mbcm_dpp_egr_threshold_types_verify_f) (
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type
  );
typedef uint32 (*mbcm_dpp_egr_ofp_thresh_type_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                         core_id,
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_IN  SOC_TMC_EGR_PORT_THRESH_TYPE ofp_thresh_type 
  ); 
typedef uint32 (*mbcm_dpp_egr_ofp_thresh_type_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                         core_id,
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID         ofp_ndx, 
    SOC_SAND_OUT SOC_TMC_EGR_PORT_THRESH_TYPE *ofp_thresh_type 
  ); 
typedef uint32 (*mbcm_dpp_egr_sched_drop_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  int                 profile,
    SOC_SAND_IN  SOC_TMC_EGR_Q_PRIO          prio_ndx, 
    SOC_SAND_IN  SOC_TMC_EGR_DROP_THRESH     *thresh, 
    SOC_SAND_OUT SOC_TMC_EGR_DROP_THRESH     *exact_thresh 
  ); 
typedef uint32 (*mbcm_dpp_egr_sched_drop_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_EGR_Q_PRIO          prio_ndx, 
    SOC_SAND_OUT SOC_TMC_EGR_DROP_THRESH     *thresh 
  ); 
typedef uint32 (*mbcm_dpp_egr_unsched_drop_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  int                 profile,
    SOC_SAND_IN  SOC_TMC_EGR_Q_PRIO          prio_ndx, 
    SOC_SAND_IN  uint32                 dp_ndx, 
    SOC_SAND_IN  SOC_TMC_EGR_DROP_THRESH     *thresh, 
    SOC_SAND_OUT SOC_TMC_EGR_DROP_THRESH     *exact_thresh 
  ); 
typedef uint32 (*mbcm_dpp_egr_unsched_drop_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  SOC_TMC_EGR_Q_PRIO          prio_ndx, 
    SOC_SAND_IN  uint32                 dp_ndx, 
    SOC_SAND_OUT SOC_TMC_EGR_DROP_THRESH     *thresh 
  ); 
typedef int (*mbcm_dpp_egr_dev_fc_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  SOC_TMC_EGR_FC_DEVICE_THRESH *thresh, 
    SOC_SAND_OUT SOC_TMC_EGR_FC_DEVICE_THRESH *exact_thresh 
  ); 
typedef int (*mbcm_dpp_egr_dev_fc_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT SOC_TMC_EGR_FC_DEVICE_THRESH *thresh 
  ); 
typedef uint32 (*mbcm_dpp_egr_xaui_spaui_fc_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_INTERFACE_ID        if_ndx, 
    SOC_SAND_IN  SOC_TMC_EGR_FC_CHNIF_THRESH *thresh, 
    SOC_SAND_OUT SOC_TMC_EGR_FC_CHNIF_THRESH *exact_thresh 
  ); 
typedef uint32 (*mbcm_dpp_egr_xaui_spaui_fc_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_INTERFACE_ID        if_ndx, 
    SOC_SAND_OUT SOC_TMC_EGR_FC_CHNIF_THRESH *thresh 
  ); 
typedef int (*mbcm_dpp_egr_ofp_fc_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  SOC_TMC_EGR_Q_PRIO          prio_ndx, 
    SOC_SAND_IN  SOC_TMC_EGR_PORT_THRESH_TYPE ofp_type_ndx, 
    SOC_SAND_IN  SOC_TMC_EGR_FC_OFP_THRESH   *thresh, 
    SOC_SAND_OUT SOC_TMC_EGR_FC_OFP_THRESH   *exact_thresh 
  ); 

typedef uint32 (*mbcm_dpp_egr_sched_port_fc_thresh_set_f) (
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  int       core,
    SOC_SAND_IN  int       threshold_type,
    SOC_SAND_IN  SOC_TMC_EGR_FC_OFP_THRESH  *thresh
  );

typedef uint32 (*mbcm_dpp_egr_sched_q_fc_thresh_set_f) (
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  int       core,
    SOC_SAND_IN  int       prio,
    SOC_SAND_IN  int       threshold_type,
    SOC_SAND_IN  SOC_TMC_EGR_FC_OFP_THRESH  *thresh
  );

typedef int (*mbcm_dpp_egr_ofp_fc_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  SOC_TMC_EGR_Q_PRIO          prio_ndx, 
    SOC_SAND_IN  SOC_TMC_EGR_PORT_THRESH_TYPE ofp_type_ndx, 
    SOC_SAND_OUT SOC_TMC_EGR_FC_OFP_THRESH   *thresh 
  ); 
typedef uint32 (*mbcm_dpp_egr_mci_fc_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_EGR_MCI_PRIO        mci_prio_ndx, 
    SOC_SAND_IN  SOC_TMC_EGR_FC_MCI_THRESH   *thresh, 
    SOC_SAND_OUT SOC_TMC_EGR_FC_MCI_THRESH   *exact_thresh 
  ); 
typedef uint32 (*mbcm_dpp_egr_mci_fc_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_EGR_MCI_PRIO        mci_prio_ndx, 
    SOC_SAND_OUT SOC_TMC_EGR_FC_MCI_THRESH   *thresh 
  ); 
typedef uint32 (*mbcm_dpp_egr_mci_fc_enable_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_EGR_MCI_PRIO        mci_prio_ndx, 
    SOC_SAND_IN  uint8                 mci_enable, 
    SOC_SAND_IN  uint8                 erp_enable 
  ); 
typedef uint32 (*mbcm_dpp_egr_mci_fc_enable_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_EGR_MCI_PRIO        mci_prio_ndx, 
    SOC_SAND_OUT uint8                 *mci_enable, 
    SOC_SAND_OUT uint8                 *erp_enable 
  ); 
typedef uint32 (*mbcm_dpp_egr_ofp_sch_mode_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_EGR_OFP_SCH_MODE    *sch_mode 
  ); 
typedef uint32 (*mbcm_dpp_egr_ofp_sch_mode_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_OUT SOC_TMC_EGR_OFP_SCH_MODE    *sch_mode 
  ); 
typedef uint32 (*mbcm_dpp_egr_ofp_scheduling_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  SOC_TMC_EGR_OFP_SCH_INFO    *info 
  ); 
typedef uint32 (*mbcm_dpp_egr_ofp_scheduling_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_OUT SOC_TMC_EGR_OFP_SCH_INFO    *info 
  ); 
typedef uint32 (*mbcm_dpp_egr_unsched_drop_prio_set_f) ( 
    SOC_SAND_IN  int  unit, 
    SOC_SAND_IN  uint32  tc_ndx, 
    SOC_SAND_IN  uint32  dp_ndx, 
    SOC_SAND_IN  uint32   drop_prio 
  ); 
typedef uint32 (*mbcm_dpp_egr_unsched_drop_prio_get_f) ( 
    SOC_SAND_IN  int  unit, 
    SOC_SAND_IN  uint32  tc_ndx, 
    SOC_SAND_IN  uint32  dp_ndx, 
    SOC_SAND_OUT uint32   *drop_prio 
  );
 
 

typedef uint32 (*mbcm_dpp_sch_device_rate_entry_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 rci_level_ndx, 
    SOC_SAND_IN  uint32                 nof_active_links_ndx, 
    SOC_SAND_IN  uint32                  rate
  ); 
typedef uint32 (*mbcm_dpp_sch_device_rate_entry_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 rci_level_ndx, 
    SOC_SAND_IN  uint32                 nof_active_links_ndx, 
    SOC_SAND_OUT uint32                   *rate 
  ); 
typedef uint32 (*mbcm_dpp_sch_device_rate_entry_core_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  uint32              rci_level_ndx, 
    SOC_SAND_IN  uint32              nof_active_links_ndx, 
    SOC_SAND_IN  uint32              rate
  ); 
typedef uint32 (*mbcm_dpp_sch_device_rate_entry_core_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  uint32              rci_level_ndx, 
    SOC_SAND_IN  uint32              nof_active_links_ndx, 
    SOC_SAND_OUT uint32              *rate 
  ); 
typedef int (*mbcm_dpp_sch_if_shaper_rate_set_f) ( 
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  uint32              if_rate
  ); 
typedef int (*mbcm_dpp_sch_if_shaper_rate_get_f) ( 
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  int           core,
    SOC_SAND_IN  uint32        tm_port,
    SOC_SAND_OUT uint32        *if_rate
  ); 
typedef uint32 (*mbcm_dpp_sch_device_if_weight_idx_set_f) ( 
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  soc_port_t          port,
    SOC_SAND_IN  uint32              weight_index
  ); 
typedef uint32 (*mbcm_dpp_sch_device_if_weight_idx_get_f) ( 
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  soc_port_t          port,
    SOC_SAND_OUT uint32              *weight_index
  ); 
typedef uint32 (*mbcm_dpp_sch_if_weight_conf_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_SCH_IF_WEIGHTS      *if_weights 
  ); 
typedef uint32 (*mbcm_dpp_sch_if_weight_conf_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_OUT SOC_TMC_SCH_IF_WEIGHTS      *if_weights 
  ); 
typedef uint32 (*mbcm_dpp_sch_class_type_params_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  SOC_TMC_SCH_CL_CLASS_TYPE_ID cl_type_ndx, 
    SOC_SAND_IN  SOC_TMC_SCH_SE_CL_CLASS_INFO *class_type, 
    SOC_SAND_OUT SOC_TMC_SCH_SE_CL_CLASS_INFO *exact_class_type 
  ); 
typedef uint32 (*mbcm_dpp_sch_class_type_params_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  SOC_TMC_SCH_CL_CLASS_TYPE_ID cl_type_ndx, 
    SOC_SAND_OUT SOC_TMC_SCH_SE_CL_CLASS_INFO *class_type 
  ); 
typedef uint32 (*mbcm_dpp_sch_class_type_params_table_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  SOC_TMC_SCH_SE_CL_CLASS_TABLE *sct, 
    SOC_SAND_OUT SOC_TMC_SCH_SE_CL_CLASS_TABLE *exact_sct 
  ); 
typedef uint32 (*mbcm_dpp_sch_class_type_params_table_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_OUT SOC_TMC_SCH_SE_CL_CLASS_TABLE *sct 
  ); 
typedef uint32 (*mbcm_dpp_sch_port_sched_set_f) ( 
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  int                core, 
    SOC_SAND_IN  SOC_TMC_SCH_PORT_ID        tm_port, 
    SOC_SAND_IN  SOC_TMC_SCH_PORT_INFO      *port_info 
  ); 
typedef uint32 (*mbcm_dpp_sch_port_sched_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  SOC_TMC_SCH_PORT_ID        tm_port, 
    SOC_SAND_OUT SOC_TMC_SCH_PORT_INFO      *port_info 
  ); 
typedef uint32 (*mbcm_dpp_sch_port_hp_class_conf_set_f) ( 
    SOC_SAND_IN  int                    unit, 
    SOC_SAND_IN  SOC_TMC_SCH_PORT_HP_CLASS_INFO  *hp_class_info 
  ); 
typedef uint32 (*mbcm_dpp_sch_port_hp_class_conf_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_OUT SOC_TMC_SCH_PORT_HP_CLASS_INFO *hp_class_info 
  ); 
typedef uint32 (*mbcm_dpp_sch_slow_max_rates_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 slow_rate_type,
    SOC_SAND_IN  int                 slow_rate_val 
  ); 
typedef uint32 (*mbcm_dpp_sch_slow_max_rates_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 slow_rate_type,
    SOC_SAND_OUT int      *slow_rate_val
  ); 
typedef uint32 (*mbcm_dpp_sch_slow_max_rates_per_level_set_f) (
    SOC_SAND_IN  int                  unit, 
    SOC_SAND_IN  int                  core, 
    SOC_SAND_IN  int                  level, 
    SOC_SAND_IN  int                  slow_rate_type, 
    SOC_SAND_IN  int                  slow_rate_val
  );
typedef uint32 (*mbcm_dpp_sch_slow_max_rates_per_level_get_f) (
    SOC_SAND_IN  int                  unit, 
    SOC_SAND_IN  int                  core, 
    SOC_SAND_IN  int                  level, 
    SOC_SAND_IN  int                  slow_rate_type, 
    SOC_SAND_OUT int                  *slow_rate_val
  );

typedef uint32 (*mbcm_dpp_sch_aggregate_set_f) ( 
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  int                core, 
    SOC_SAND_IN  SOC_TMC_SCH_SE_ID          se_ndx, 
    SOC_SAND_IN  SOC_TMC_SCH_SE_INFO        *se, 
    SOC_SAND_IN  SOC_TMC_SCH_FLOW           *flow, 
    SOC_SAND_OUT SOC_TMC_SCH_FLOW           *exact_flow 
  ); 
typedef uint32 (*mbcm_dpp_sch_aggregate_group_set_f) ( 
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  int                core, 
    SOC_SAND_IN  SOC_TMC_SCH_SE_ID          se_ndx, 
    SOC_SAND_IN  SOC_TMC_SCH_SE_INFO        *se, 
    SOC_SAND_IN  SOC_TMC_SCH_FLOW           *flow
  );
typedef uint32 (*mbcm_dpp_sch_aggregate_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  SOC_TMC_SCH_SE_ID          se_ndx, 
    SOC_SAND_OUT SOC_TMC_SCH_SE_INFO        *se, 
    SOC_SAND_OUT SOC_TMC_SCH_FLOW           *flow 
  ); 
typedef uint32 (*mbcm_dpp_sch_flow_delete_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  SOC_TMC_SCH_FLOW_ID         flow_ndx 
  ); 
typedef uint32 (*mbcm_dpp_sch_flow_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  SOC_TMC_SCH_FLOW_ID        flow_ndx, 
    SOC_SAND_IN  SOC_TMC_SCH_FLOW           *flow, 
    SOC_SAND_OUT SOC_TMC_SCH_FLOW           *exact_flow 
  ); 

typedef uint32 (*mbcm_dpp_sch_flow_ipf_mode_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE  mode
  ); 

typedef uint32 (*mbcm_dpp_sch_flow_ipf_mode_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_OUT  SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE  *mode
  ); 

typedef uint32 (*mbcm_dpp_sch_flow_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  SOC_TMC_SCH_FLOW_ID        flow_ndx, 
    SOC_SAND_OUT SOC_TMC_SCH_FLOW           *flow 
  ); 
typedef uint32 (*mbcm_dpp_sch_flow_status_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  SOC_TMC_SCH_FLOW_ID        flow_ndx, 
    SOC_SAND_IN  SOC_TMC_SCH_FLOW_STATUS    state 
  ); 
typedef uint32 (*mbcm_dpp_sch_per1k_info_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  uint32                 k_flow_ndx, 
    SOC_SAND_IN  SOC_TMC_SCH_GLOBAL_PER1K_INFO *per1k_info 
  ); 
typedef uint32 (*mbcm_dpp_sch_per1k_info_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  uint32                 k_flow_ndx, 
    SOC_SAND_OUT SOC_TMC_SCH_GLOBAL_PER1K_INFO *per1k_info 
  ); 
typedef uint32 (*mbcm_dpp_sch_flow_to_queue_mapping_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  uint32                 quartet_ndx, 
    SOC_SAND_IN  uint32                 nof_quartets_to_map, 
    SOC_SAND_IN  SOC_TMC_SCH_QUARTET_MAPPING_INFO *quartet_flow_info 
  ); 
typedef uint32 (*mbcm_dpp_sch_flow_to_queue_mapping_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  uint32                 quartet_ndx, 
    SOC_SAND_OUT SOC_TMC_SCH_QUARTET_MAPPING_INFO *quartet_flow_info 
  ); 
typedef uint32 (*mbcm_dpp_sch_flow_id_verify_unsafe_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_SCH_FLOW_ID         flow_id 
  ); 
typedef uint32 (*mbcm_dpp_sch_se_id_verify_unsafe_f) ( 
    SOC_SAND_IN  int               unit, 
    SOC_SAND_IN  SOC_TMC_SCH_SE_ID        se_id 
  ); 
typedef uint32 (*mbcm_dpp_sch_port_id_verify_unsafe_f) ( 
    SOC_SAND_IN  int               unit, 
    SOC_SAND_IN  SOC_TMC_SCH_PORT_ID      port_id 
  ); 
typedef uint32 (*mbcm_dpp_sch_k_flow_id_verify_unsafe_f) ( 
    SOC_SAND_IN  int        unit, 
    SOC_SAND_IN  uint32        k_flow_id 
  ); 
typedef uint32 (*mbcm_dpp_sch_quartet_id_verify_unsafe_f) ( 
    SOC_SAND_IN  int        unit, 
    SOC_SAND_IN  uint32        quartet_id 
  ); 
 
typedef SOC_TMC_SCH_PORT_ID (*mbcm_dpp_sch_se2port_id_f) (
    SOC_SAND_IN  SOC_TMC_SCH_SE_ID          se_id
  ); 
 
typedef SOC_TMC_SCH_SE_ID (*mbcm_dpp_sch_flow2se_id_f) (
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_TMC_SCH_FLOW_ID        flow_id
  );

typedef SOC_TMC_SCH_SE_ID (*mbcm_dpp_sch_port2se_id_f) (
    SOC_SAND_IN  SOC_TMC_SCH_PORT_ID        port_id
  );

typedef uint32 (*mbcm_dpp_sch_se2port_tc_id_f) (
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  int               core,
    SOC_SAND_IN  SOC_TMC_SCH_SE_ID           se_id,
    SOC_SAND_OUT SOC_TMC_SCH_PORT_ID         *port_id,
    SOC_SAND_OUT uint32               *tc 
  );

typedef uint32 (*mbcm_dpp_sch_port_tc2se_id_f) (
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  int              core,
    SOC_SAND_IN  SOC_TMC_SCH_PORT_ID        port_id,
    SOC_SAND_IN  uint32              tc,
    SOC_SAND_OUT ARAD_SCH_SE_ID          *se_id
  );

typedef SOC_TMC_SCH_FLOW_ID (*mbcm_dpp_sch_se2flow_id_f) (
    SOC_SAND_IN  SOC_TMC_SCH_SE_ID          se_id
  );

typedef SOC_TMC_SCH_FLOW_ID (*mbcm_dpp_sch_se_get_type_by_id_f) (
    SOC_SAND_IN  SOC_TMC_SCH_SE_ID  se_id
  );

typedef int (*mbcm_dpp_sch_e2e_interface_allocate_f)(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  soc_port_t      port
    );

typedef int (*mbcm_dpp_sch_e2e_interface_deallocate_f)(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  soc_port_t      port
    );

typedef int (*mbcm_dpp_sch_prio_propagation_enable_set_f)(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  int enable
    );

typedef int (*mbcm_dpp_sch_prio_propagation_enable_get_f)(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT int *enable
    );

typedef int (*mbcm_dpp_sch_prio_propagation_port_set_f)(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_IN  int cosq,
    SOC_SAND_IN  int is_high_prio
   );
typedef int (*mbcm_dpp_sch_prio_propagation_port_get_f)(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_IN  int cosq,
    SOC_SAND_OUT int *is_high_prio
   );

typedef uint32 (*mbcm_dpp_sch_shds_tbl_get_unsafe_f)(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          entry_offset,
    SOC_SAND_OUT  ARAD_SCH_SHDS_TBL_DATA* SCH_shds_tbl_data
  );

typedef uint32 (*mbcm_dpp_sch_shds_tbl_set_unsafe_f)(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_SHDS_TBL_DATA* SCH_shds_tbl_data
  );

typedef uint32 (*mbcm_dpp_sch_port_tcg_weight_set_f) (
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  int                      core,
    SOC_SAND_IN  uint32                   tm_port,
    SOC_SAND_IN  SOC_TMC_TCG_NDX          tcg_ndx,
    SOC_SAND_IN  SOC_TMC_SCH_TCG_WEIGHT   *tcg_weight
  );
 
typedef uint32 (*mbcm_dpp_sch_port_tcg_weight_get_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core,
    SOC_SAND_IN  uint32                       tm_port,
    SOC_SAND_IN  SOC_TMC_TCG_NDX              tcg_ndx,
    SOC_SAND_OUT  SOC_TMC_SCH_TCG_WEIGHT      *tcg_weight
  );
 
 
typedef int (*mbcm_dpp_mgmt_max_pckt_size_set_f) (
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  soc_port_t               port_ndx,
    SOC_SAND_IN  int                      max_size
  );
 
typedef int (*mbcm_dpp_mgmt_max_pckt_size_get_f) (
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  soc_port_t               port,
    SOC_SAND_OUT int                      *max_size
  );
 
typedef uint32 (*mbcm_dpp_mgmt_ocb_voq_eligible_dynamic_set_f) (
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      qid,
    SOC_SAND_IN  uint32                      enable 
  ); 

typedef int (*mbcm_dpp_mgmt_voq_is_ocb_eligible_get_f) (
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  int         core_id,
    SOC_SAND_IN  uint32      qid,
    SOC_SAND_OUT uint32      *is_ocb_eligible
  );
  
typedef uint32 (*mbcm_dpp_mult_cud_to_port_map_set_f) (
    SOC_SAND_IN int                 unit, 
    SOC_SAND_IN uint32              flags,
    SOC_SAND_IN uint32              cud,  
    SOC_SAND_IN SOC_TMC_FAP_PORT_ID port  
  );

typedef uint32 (*mbcm_dpp_mult_cud_to_port_map_get_f) (
    SOC_SAND_IN  int          unit, 
    SOC_SAND_IN  uint32          cud,       
    SOC_SAND_OUT SOC_TMC_FAP_PORT_ID *port      
  );

 
typedef uint32 (*mbcm_dpp_fabric_fc_enable_set_f) ( 
    SOC_SAND_IN  int                  unit, 
    SOC_SAND_IN  SOC_TMC_CONNECTION_DIRECTION direction_ndx, 
    SOC_SAND_IN  SOC_TMC_FABRIC_FC            *info 
  ); 
typedef uint32 (*mbcm_dpp_fabric_fc_enable_get_f) ( 
    SOC_SAND_IN  int                  unit, 
    SOC_SAND_OUT SOC_TMC_FABRIC_FC           *info_rx, 
    SOC_SAND_OUT SOC_TMC_FABRIC_FC           *info_tx 
  ); 
typedef uint32 (*mbcm_dpp_fabric_fc_shaper_set_f) ( 
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                        link_ndx,
    SOC_SAND_IN  SOC_TMC_FABRIC_FC_SHAPER_MODE_INFO  *shaper_mode,  
    SOC_SAND_IN  SOC_TMC_FABRIC_FC_SHAPER            *info,
    SOC_SAND_OUT SOC_TMC_FABRIC_FC_SHAPER            *exact_info
  ); 
typedef uint32 (*mbcm_dpp_fabric_fc_shaper_get_f) ( 
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                         link_ndx,
    SOC_SAND_OUT SOC_TMC_FABRIC_FC_SHAPER_MODE_INFO  *shaper_mode,  
    SOC_SAND_OUT SOC_TMC_FABRIC_FC_SHAPER            *info
  ); 

typedef uint32 (*mbcm_dpp_fabric_cell_format_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_OUT SOC_TMC_FABRIC_CELL_FORMAT  *info 
  ); 
typedef uint32 (*mbcm_dpp_fabric_coexist_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_FABRIC_COEXIST_INFO *info 
  ); 
typedef uint32 (*mbcm_dpp_fabric_coexist_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_OUT SOC_TMC_FABRIC_COEXIST_INFO *info 
  ); 
typedef uint32 (*mbcm_dpp_fabric_stand_alone_fap_mode_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_OUT uint8                 *is_single_fap_mode 
  ); 
typedef uint32 (*mbcm_dpp_fabric_connect_mode_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_FABRIC_CONNECT_MODE fabric_mode 
  ); 
typedef uint32 (*mbcm_dpp_fabric_connect_mode_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_OUT SOC_TMC_FABRIC_CONNECT_MODE *fabric_mode 
  ); 
typedef uint32 (*mbcm_dpp_fabric_fap20_map_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 other_device_ndx, 
    SOC_SAND_IN  uint8                 is_fap20_device 
  ); 
typedef uint32 (*mbcm_dpp_fabric_fap20_map_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 other_device_ndx, 
    SOC_SAND_OUT uint8                 *is_fap20_device 
  ); 
typedef uint32 (*mbcm_dpp_fabric_topology_status_connectivity_get_f) ( 
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                    link_index_min,
    SOC_SAND_IN  int                    link_index_max, 
    SOC_SAND_OUT SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR *connectivity_map 
  ); 
typedef uint32 (*mbcm_dpp_fabric_links_status_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint8                 read_crc_counter, 
    SOC_SAND_OUT SOC_TMC_FABRIC_LINKS_STATUS_ALL    *links_status 
  );
typedef int (*mbcm_dpp_fabric_aldwp_config_f) ( 
    SOC_SAND_IN  int                 unit
  );
typedef uint32 (*mbcm_dpp_fabric_nof_links_get_f) ( 
    SOC_SAND_IN  int                 unit,  
    SOC_SAND_OUT int                    *nof_links 
  );
typedef uint32 (*mbcm_dpp_fabric_gci_enable_set_f) ( 
    SOC_SAND_IN  int                                 unit,  
    SOC_SAND_IN  SOC_TMC_FABRIC_GCI_TYPE                type,
    SOC_SAND_IN  int                                    value
  );
typedef uint32 (*mbcm_dpp_fabric_gci_enable_get_f) ( 
    SOC_SAND_IN  int                                     unit,  
    SOC_SAND_IN SOC_TMC_FABRIC_GCI_TYPE                     type,
    SOC_SAND_OUT int                                        *value
  );
typedef uint32 (*mbcm_dpp_fabric_gci_config_set_f) ( 
    SOC_SAND_IN  int                                 unit,  
    SOC_SAND_IN  SOC_TMC_FABRIC_GCI_CONFIG_TYPE         type,
    SOC_SAND_IN  int                                    value
  );
typedef uint32 (*mbcm_dpp_fabric_gci_config_get_f) ( 
    SOC_SAND_IN  int                                     unit,  
    SOC_SAND_IN SOC_TMC_FABRIC_GCI_CONFIG_TYPE              type,
    SOC_SAND_OUT int                                        *value
  );
typedef int (*mbcm_dpp_fabric_gci_backoff_masks_init_f) ( 
    SOC_SAND_IN  int                                     unit  
  );
typedef uint32 (*mbcm_dpp_fabric_llfc_threshold_set_f) ( 
    SOC_SAND_IN  int                                 unit,  
    SOC_SAND_IN int                                     value
  );
typedef uint32 (*mbcm_dpp_fabric_llfc_threshold_get_f) ( 
    SOC_SAND_IN  int                                     unit,  
    SOC_SAND_OUT int                                        *value
  );
typedef uint32 (*mbcm_dpp_fabric_rci_enable_set_f) ( 
    SOC_SAND_IN  int                                 unit,  
    SOC_SAND_IN  soc_dcmn_fabric_control_source_t       value
  );
typedef uint32 (*mbcm_dpp_fabric_rci_enable_get_f) ( 
    SOC_SAND_IN  int                                     unit,  
    SOC_SAND_OUT soc_dcmn_fabric_control_source_t           *value
  );      
typedef uint32 (*mbcm_dpp_fabric_rci_config_set_f) ( 
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN ARAD_FABRIC_RCI_CONFIG_TYPE             rci_config_type,  
    SOC_SAND_IN int                                     value
  );
typedef uint32 (*mbcm_dpp_fabric_rci_config_get_f) ( 
    SOC_SAND_IN  int                                     unit,  
    SOC_SAND_IN ARAD_FABRIC_RCI_CONFIG_TYPE                 rci_config_type,
    SOC_SAND_OUT int                                        *value
  ); 
 
typedef uint32 (*mbcm_dpp_fabric_minimal_links_to_dest_set_f) (
    SOC_SAND_IN int          unit,
    SOC_SAND_IN soc_module_t    module_id,
    SOC_SAND_IN int             num_of_links
  );

typedef uint32 (*mbcm_dpp_fabric_minimal_links_to_dest_get_f) (
    SOC_SAND_IN int             unit,
    SOC_SAND_IN soc_module_t    module_id,
    SOC_SAND_OUT int            *num_of_links
  );

typedef uint32 (*mbcm_dpp_fabric_minimal_links_all_reachable_set_f) (
    SOC_SAND_IN int             unit,
    SOC_SAND_IN int             num_of_links
  );

typedef uint32 (*mbcm_dpp_fabric_minimal_links_all_reachable_get_f) (
    SOC_SAND_IN int          unit,
    SOC_SAND_OUT int            *num_of_links
  );
 
typedef soc_error_t (*mbcm_dpp_fabric_link_tx_traffic_disable_set_f) (
    int unit,
    soc_port_t link,
    int disable
  );
           
typedef soc_error_t (*mbcm_dpp_fabric_link_tx_traffic_disable_get_f) (
    int unit,
    soc_port_t link,
    int *disable
  );
           
typedef uint32 (*mbcm_dpp_fabric_topology_status_connectivity_print_f) ( 
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN  SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR *connectivity_map 
  );

typedef uint32 (*mbcm_dpp_fabric_cell_cpu_data_get_f) (
    SOC_SAND_IN  int                           unit,
    SOC_SAND_OUT uint32                        *cell_buffer
  );

typedef int (*mbcm_dpp_fabric_efms_enable_set_f) (
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint32                        modid,
    SOC_SAND_IN  int                           val
  );

typedef int (*mbcm_dpp_fabric_efms_enable_get_f) (
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint32                        modid,
    SOC_SAND_OUT int                           *val
  );

 
typedef int (*mbcm_dpp_ipq_explicit_mapping_mode_info_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO *info 
  ); 
typedef int (*mbcm_dpp_ipq_explicit_mapping_mode_info_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_OUT SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO *info 
  ); 
typedef uint32 (*mbcm_dpp_ipq_traffic_class_map_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_IPQ_TR_CLS          tr_cls_ndx, 
    SOC_SAND_IN  SOC_TMC_IPQ_TR_CLS          new_class 
  ); 
typedef uint32 (*mbcm_dpp_ipq_traffic_class_map_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_IPQ_TR_CLS          tr_cls_ndx, 
    SOC_SAND_OUT SOC_TMC_IPQ_TR_CLS          *new_class 
  ); 
typedef int (*mbcm_dpp_ipq_traffic_class_multicast_priority_map_set_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 traffic_class,
    SOC_SAND_IN  uint8                  enable
  );
typedef int (*mbcm_dpp_ipq_traffic_class_multicast_priority_map_get_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 traffic_class,
    SOC_SAND_OUT uint8                  *enable
  );
typedef uint32 (*mbcm_dpp_ipq_destination_id_packets_base_queue_id_set_f) ( 
   SOC_SAND_IN  int                 unit,
   SOC_SAND_IN  int                 core,
   SOC_SAND_IN  uint32              dest_ndx,
   SOC_SAND_IN  uint8               valid,
   SOC_SAND_IN  uint8               sw_only,
   SOC_SAND_IN  uint32              base_queue
  ); 
typedef uint32 (*mbcm_dpp_ipq_destination_id_packets_base_queue_id_get_f) ( 
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              dest_ndx,
    SOC_SAND_OUT uint8               *valid,
    SOC_SAND_OUT uint8               *sw_only,
    SOC_SAND_OUT uint32              *base_queue
  ); 
typedef uint32 (*mbcm_dpp_ipq_queue_interdigitated_mode_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  uint32                  k_queue_ndx, 
    SOC_SAND_IN  uint8                 is_interdigitated 
  ); 
typedef uint32 (*mbcm_dpp_ipq_queue_interdigitated_mode_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  uint32                  k_queue_ndx, 
    SOC_SAND_OUT uint8                 *is_interdigitated 
  ); 
typedef uint32 (*mbcm_dpp_ipq_queue_to_flow_mapping_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  uint32              queue_quartet_ndx, 
    SOC_SAND_IN  SOC_TMC_IPQ_QUARTET_MAP_INFO *info 
  ); 
typedef uint32 (*mbcm_dpp_ipq_queue_to_flow_mapping_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  uint32                  queue_quartet_ndx, 
    SOC_SAND_OUT SOC_TMC_IPQ_QUARTET_MAP_INFO *info 
  ); 
typedef uint32 (*mbcm_dpp_ipq_queue_qrtt_unmap_f) ( 
    SOC_SAND_IN  int unit, 
    SOC_SAND_IN  int core,
    SOC_SAND_IN  uint32  queue_quartet_ndx 
  ); 
typedef uint32 (*mbcm_dpp_ipq_quartet_reset_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_quartet_ndx 
  ); 
typedef uint32 (*mbcm_dpp_ipq_attached_flow_port_get_f) ( 
    SOC_SAND_IN  int unit, 
    SOC_SAND_IN  int core,
    SOC_SAND_IN  uint32 queue_ndx, 
    SOC_SAND_OUT uint32 *flow_id, 
    SOC_SAND_OUT uint32 *sys_port 
  ); 
typedef uint32 (*mbcm_dpp_ipq_tc_profile_set_f) (
    SOC_SAND_IN     int         unit,
    SOC_SAND_IN     int         core,
    SOC_SAND_IN     uint8       is_flow_ndx,
    SOC_SAND_IN     uint32      dest_ndx,
    SOC_SAND_IN     uint32      tc_profile
  );
typedef uint32 (*mbcm_dpp_ipq_tc_profile_get_f) (
    SOC_SAND_IN      int        unit,
    SOC_SAND_IN      int        core,
    SOC_SAND_IN      uint8      is_flow_ndx,
    SOC_SAND_IN      uint32     dest_ndx,
    SOC_SAND_OUT     uint32     *tc_profile
  );
typedef uint32 (*mbcm_dpp_ipq_tc_profile_map_set_f) (
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core_id,
    SOC_SAND_IN  uint32             profile_ndx,
    SOC_SAND_IN  uint8              is_flow_profile,
    SOC_SAND_IN  SOC_TMC_IPQ_TR_CLS tr_cls_ndx,
    SOC_SAND_IN  SOC_TMC_IPQ_TR_CLS new_class
  );
typedef uint32 (*mbcm_dpp_ipq_tc_profile_map_get_f) (
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  int              core_id,
    SOC_SAND_IN  uint32           profile_ndx,
    SOC_SAND_IN  uint8              is_flow_profile,
    SOC_SAND_IN  SOC_TMC_IPQ_TR_CLS tr_cls_ndx,
    SOC_SAND_OUT SOC_TMC_IPQ_TR_CLS *new_class
  );

typedef uint32 (*mbcm_dpp_ipq_stack_lag_packets_base_queue_id_set_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  tmd,
    SOC_SAND_IN  uint32                  entry,
    SOC_SAND_IN  uint32                  base_queue
  );

typedef uint32 (*mbcm_dpp_ipq_stack_lag_packets_base_queue_id_get_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 tmd,
    SOC_SAND_IN  uint32                 entry,
    SOC_SAND_OUT uint32              *base_queue
  );

typedef uint32 (*mbcm_dpp_ipq_stack_fec_map_stack_lag_set_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  tmd,
    SOC_SAND_IN  uint32                  entry,
    SOC_SAND_IN  uint32                  stack_lag
  );

typedef uint32 (*mbcm_dpp_ipq_stack_fec_map_stack_lag_get_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  tmd,
    SOC_SAND_IN  uint32                  entry,
    SOC_SAND_OUT uint32*                 stack_lag
  );

typedef int (*mbcm_dpp_ipq_default_invalid_queue_set_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              queue_id,
    SOC_SAND_IN  int                 enable
  );

typedef int (*mbcm_dpp_ipq_default_invalid_queue_get_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT uint32              *queue_id,
    SOC_SAND_OUT int                 *enable
  );


typedef soc_error_t (*mbcm_dpp_cosq_gport_sched_set_f) (
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_IN  int                                weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_fabric_pipe_t   fabric_pipe_type
  );

typedef soc_error_t (*mbcm_dpp_cosq_gport_sched_get_f) (
    SOC_SAND_IN   int                                unit,
    SOC_SAND_IN   int                                pipe,
    SOC_SAND_OUT  int*                               weight,
    SOC_SAND_IN   soc_dpp_cosq_gport_fabric_pipe_t   fabric_pipe_type
  );

  
typedef soc_error_t (*mbcm_dpp_ingress_scheduler_clos_bandwidth_set_f) (
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_IN uint32               rate
    ); 

typedef soc_error_t (*mbcm_dpp_ingress_scheduler_clos_bandwidth_get_f) (
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_OUT uint32              *rate
    );

typedef soc_error_t (*mbcm_dpp_ingress_scheduler_mesh_bandwidth_set_f) (
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_IN uint32               rate
    ); 

typedef soc_error_t (*mbcm_dpp_ingress_scheduler_mesh_bandwidth_get_f) (
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_OUT uint32              *rate
    );
typedef soc_error_t (*mbcm_dpp_ingress_scheduler_clos_sched_set_f) (
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_IN  int                 weight
    ); 

typedef soc_error_t (*mbcm_dpp_ingress_scheduler_clos_sched_get_f) (
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_OUT int                 *weight
    );

typedef soc_error_t (*mbcm_dpp_ingress_scheduler_mesh_sched_set_f) (
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_IN  int                 weight
    ); 

typedef soc_error_t (*mbcm_dpp_ingress_scheduler_mesh_sched_get_f) (
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_OUT int                 *weight
    );

typedef soc_error_t (*mbcm_dpp_ingress_scheduler_clos_burst_set_f) (
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_IN  int                 burst
    ); 

typedef soc_error_t (*mbcm_dpp_ingress_scheduler_clos_burst_get_f) (
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_OUT int                 *burst
    );

typedef soc_error_t (*mbcm_dpp_ingress_scheduler_mesh_burst_set_f) (
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_IN  int                 burst
    ); 

typedef soc_error_t (*mbcm_dpp_ingress_scheduler_mesh_burst_get_f) (
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_OUT int                 *burst
    );

typedef int (*mbcm_dpp_ingress_scheduler_clos_slow_start_set_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  SOC_TMC_ING_SCH_CLOS_INFO   *clos_info
    );

typedef int (*mbcm_dpp_ingress_scheduler_clos_slow_start_get_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT SOC_TMC_ING_SCH_CLOS_INFO   *clos_info
    );

typedef int (*mbcm_dpp_ingress_scheduler_mesh_slow_start_set_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  SOC_TMC_ING_SCH_SHAPER   *shaper_info
    );

typedef int (*mbcm_dpp_ingress_scheduler_mesh_slow_start_get_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT SOC_TMC_ING_SCH_SHAPER   *shaper_info
    );

typedef int (*mbcm_dpp_ingress_scheduler_init_f) (
    SOC_SAND_IN  int                 unit
    );

 

typedef int (*mbcm_dpp_ocb_control_range_dram_mix_dbuff_threshold_get_f) (
  SOC_SAND_IN int                    unit,
  SOC_SAND_IN int                    core,
  SOC_SAND_OUT int                   *range_max,
  SOC_SAND_OUT int                   *range_min
  );

typedef int (*mbcm_dpp_ocb_control_range_dram_mix_dbuff_threshold_set_f) (
  SOC_SAND_IN int                   unit,
  SOC_SAND_IN int                   core,
  SOC_SAND_IN int                   range_max,
  SOC_SAND_IN int                   range_min
  );

typedef int (*mbcm_dpp_ocb_control_range_ocb_committed_multicast_get_f) (
  SOC_SAND_IN int                    unit,
  SOC_SAND_IN uint32                 index,
  SOC_SAND_OUT int                   *range_max,
  SOC_SAND_OUT int                   *range_min
  );

typedef int (*mbcm_dpp_ocb_control_range_ocb_committed_multicast_set_f) (
  SOC_SAND_IN int                    unit,
  SOC_SAND_IN uint32                 index,
  SOC_SAND_IN int                   range_max,
  SOC_SAND_IN int                   range_min
  );

typedef int (*mbcm_dpp_ocb_control_range_ocb_eligible_multicast_get_f) (
  SOC_SAND_IN int                    unit,
  SOC_SAND_IN uint32                 index,
  SOC_SAND_OUT int                   *range_max,
  SOC_SAND_OUT int                   *range_min
  );

typedef int (*mbcm_dpp_ocb_control_range_ocb_eligible_multicast_set_f) (
  SOC_SAND_IN int                    unit,
  SOC_SAND_IN uint32                 index,
  SOC_SAND_IN int                   range_max,
  SOC_SAND_IN int                   range_min
  );

typedef uint32 (*mbcm_dpp_itm_dram_buffs_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_OUT SOC_TMC_ITM_DRAM_BUFFERS_INFO *dram_buffs 
  ); 

typedef int (*mbcm_dpp_dram_info_verify_f) ( 
    int                 unit, 
    soc_dpp_drc_combo28_info_t *drc_info
  );

typedef uint32 (*mbcm_dpp_user_buffer_dram_write_f) (
    SOC_SAND_IN    int      unit, 
    SOC_SAND_IN    uint32   flags, 
    SOC_SAND_INOUT    uint8    *buf, 
    SOC_SAND_IN    int      offset, 
    SOC_SAND_IN    int      nbytes
  );
typedef uint32 (*mbcm_dpp_user_buffer_dram_read_f) (
    SOC_SAND_IN    int      unit, 
    SOC_SAND_IN    uint32   flags, 
    SOC_SAND_INOUT   uint8    *buf, 
    SOC_SAND_IN    int      offset, 
    SOC_SAND_IN    int      nbytes
  );
typedef int (*mbcm_dpp_dram_validate_address_f) (
    SOC_SAND_IN     int                         unit,
    SOC_SAND_IN     uint32                      addr 
  );
typedef int (*mbcm_dpp_dram_recovery_init_f) (
    SOC_SAND_IN     int                         unit
  );
typedef uint32 (*mbcm_dpp_cache_table_update_all_f) (
    SOC_SAND_IN    int      unit
  );
typedef uint32 (*mbcm_dpp_itm_glob_rcs_fc_set_f) ( 
    SOC_SAND_IN   int                 unit, 
    SOC_SAND_IN   SOC_TMC_ITM_GLOB_RCS_FC_TH  *info, 
    SOC_SAND_OUT  SOC_TMC_ITM_GLOB_RCS_FC_TH  *exact_info 
  ); 
typedef uint32 (*mbcm_dpp_itm_glob_rcs_fc_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_OUT SOC_TMC_ITM_GLOB_RCS_FC_TH  *info 
  ); 
typedef int (*mbcm_dpp_itm_glob_rcs_drop_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_IN  SOC_TMC_ITM_GLOB_RCS_DROP_TH *info, 
    SOC_SAND_OUT SOC_TMC_ITM_GLOB_RCS_DROP_TH *exact_info 
  ); 
typedef int (*mbcm_dpp_itm_glob_rcs_drop_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_OUT SOC_TMC_ITM_GLOB_RCS_DROP_TH *info 
  ); 
typedef int (*mbcm_dpp_itm_category_rngs_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  SOC_TMC_ITM_CATEGORY_RNGS *info 
  ); 
typedef int (*mbcm_dpp_itm_category_rngs_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_OUT SOC_TMC_ITM_CATEGORY_RNGS *info 
  ); 
typedef uint32 (*mbcm_dpp_itm_admit_test_tmplt_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32                 admt_tst_ndx, 
    SOC_SAND_IN  SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO *info 
  ); 
typedef uint32 (*mbcm_dpp_itm_admit_test_tmplt_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32                 admt_tst_ndx, 
    SOC_SAND_OUT SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO *info 
  ); 
typedef int (*mbcm_dpp_itm_init_f) ( 
    SOC_SAND_IN  int                 unit
  ); 
typedef int (*mbcm_dpp_ipq_init_f) ( 
    SOC_SAND_IN  int                 unit
  ); 
typedef uint32 (*mbcm_dpp_itm_cr_request_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_ITM_QT_NDX          qt_ndx, 
    SOC_SAND_IN  SOC_TMC_ITM_CR_REQUEST_INFO *info, 
    SOC_SAND_OUT SOC_TMC_ITM_CR_REQUEST_INFO *exact_info 
  ); 
typedef uint32 (*mbcm_dpp_itm_cr_request_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32          qt_ndx, 
    SOC_SAND_OUT SOC_TMC_ITM_CR_REQUEST_INFO *info 
  ); 
typedef uint32 (*mbcm_dpp_itm_cr_discount_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_ITM_CR_DISCNT_CLS_NDX cr_cls_ndx, 
    SOC_SAND_IN  SOC_TMC_ITM_CR_DISCOUNT_INFO *info 
  ); 
typedef uint32 (*mbcm_dpp_itm_cr_discount_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_ITM_CR_DISCNT_CLS_NDX cr_cls_ndx, 
    SOC_SAND_OUT SOC_TMC_ITM_CR_DISCOUNT_INFO *info 
  ); 
typedef uint32 (*mbcm_dpp_itm_queue_test_tmplt_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 rt_cls_ndx, 
    SOC_SAND_IN  uint32                 drop_precedence_ndx, 
    SOC_SAND_IN  SOC_TMC_ITM_ADMIT_TSTS      test_tmplt 
  ); 
typedef uint32 (*mbcm_dpp_itm_queue_test_tmplt_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 rt_cls_ndx, 
    SOC_SAND_IN  uint32                 drop_precedence_ndx, 
    SOC_SAND_OUT SOC_TMC_ITM_ADMIT_TSTS      *test_tmplt 
  ); 
typedef int (*mbcm_dpp_itm_wred_exp_wq_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 rt_cls_ndx, 
    SOC_SAND_IN  uint32                  exp_wq, 
    SOC_SAND_IN  uint8                   enable 
  ); 
typedef int (*mbcm_dpp_itm_wred_exp_wq_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 rt_cls_ndx, 
    SOC_SAND_OUT  uint32                  *exp_wq 
  ); 
typedef int (*mbcm_dpp_itm_wred_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 rt_cls_ndx, 
    SOC_SAND_IN  uint32                 drop_precedence_ndx, 
    SOC_SAND_IN  SOC_TMC_ITM_WRED_QT_DP_INFO *info, 
    SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO *exact_info 
  ); 
typedef int (*mbcm_dpp_itm_wred_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 rt_cls_ndx, 
    SOC_SAND_IN  uint32                 drop_precedence_ndx, 
    SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO *info 
  ); 
typedef int (*mbcm_dpp_itm_tail_drop_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 rt_cls_ndx, 
    SOC_SAND_IN  uint32                 drop_precedence_ndx, 
    SOC_SAND_IN  SOC_TMC_ITM_TAIL_DROP_INFO  *info, 
    SOC_SAND_OUT  SOC_TMC_ITM_TAIL_DROP_INFO  *exact_info 
  ); 
typedef int (*mbcm_dpp_itm_tail_drop_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 rt_cls_ndx, 
    SOC_SAND_IN  uint32                 drop_precedence_ndx, 
    SOC_SAND_OUT SOC_TMC_ITM_TAIL_DROP_INFO  *info 
  ); 

typedef int (*mbcm_dpp_itm_fadt_tail_drop_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 rt_cls_ndx, 
    SOC_SAND_IN  uint32                 drop_precedence_ndx, 
    SOC_SAND_IN  SOC_TMC_ITM_FADT_DROP_INFO  *info,
    SOC_SAND_OUT SOC_TMC_ITM_FADT_DROP_INFO  *exact_info

  ); 
typedef int (*mbcm_dpp_itm_fadt_tail_drop_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 rt_cls_ndx, 
    SOC_SAND_IN  uint32                 drop_precedence_ndx, 
    SOC_SAND_OUT SOC_TMC_ITM_FADT_DROP_INFO  *info
  ); 

typedef uint32 (*mbcm_dpp_itm_cr_wd_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_IN  SOC_TMC_ITM_CR_WD_INFO      *info, 
    SOC_SAND_OUT SOC_TMC_ITM_CR_WD_INFO      *exact_info 
  ); 
typedef uint32 (*mbcm_dpp_itm_cr_wd_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_OUT SOC_TMC_ITM_CR_WD_INFO      *info 
  ); 

typedef uint32 (*mbcm_dpp_itm_enable_ecn_f) (
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  uint32   enabled 
  );

typedef uint32 (*mbcm_dpp_itm_get_ecn_enabled_f) (
    SOC_SAND_IN  int   unit,
    SOC_SAND_OUT uint32   *enabled 
  );

typedef int (*mbcm_dpp_itm_dram_bound_set_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
                 SOC_TMC_ITM_DRAM_BOUND_INFO  *info,
    SOC_SAND_OUT SOC_TMC_ITM_DRAM_BOUND_INFO  *exact_info
          );

typedef int (*mbcm_dpp_itm_dram_bound_get_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_DRAM_BOUND_INFO  *info
    );

typedef int (*mbcm_dpp_itm_vsq_qt_rt_cls_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_IN  uint8               is_ocb_only,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP       vsq_group_ndx, 
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_NDX         vsq_in_group_ndx, 
    SOC_SAND_IN  uint32                 vsq_rt_cls 
  ); 
typedef int (*mbcm_dpp_itm_vsq_qt_rt_cls_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_IN  uint8               is_ocb_only,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP       vsq_group_ndx, 
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_NDX         vsq_in_group_ndx, 
    SOC_SAND_OUT uint32                 *vsq_rt_cls 
  ); 
typedef int (*mbcm_dpp_itm_vsq_fc_set_f) ( 
    SOC_SAND_IN  int                        unit, 
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP      vsq_group_ndx, 
    SOC_SAND_IN  uint32                     vsq_rt_cls_ndx, 
    SOC_SAND_IN  int                        pool_id,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_FC_INFO    *info, 
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_FC_INFO    *exact_info 
  ); 
typedef int (*mbcm_dpp_itm_vsq_fc_get_f) ( 
    SOC_SAND_IN  int                        unit, 
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP      vsq_group_ndx, 
    SOC_SAND_IN  uint32                     vsq_rt_cls_ndx, 
    SOC_SAND_IN  int                        pool_id,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_FC_INFO     *info 
  ); 
typedef int (*mbcm_dpp_itm_vsq_tail_drop_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx, 
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx, 
    SOC_SAND_IN  uint32                 drop_precedence_ndx, 
    SOC_SAND_IN  int                    pool_id,
    SOC_SAND_IN  int                    is_headroom,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_TAIL_DROP_INFO *info, 
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_TAIL_DROP_INFO *exact_info 
  ); 
typedef int (*mbcm_dpp_itm_vsq_tail_drop_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP       vsq_group_ndx, 
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx, 
    SOC_SAND_IN  uint32                 drop_precedence_ndx, 
    SOC_SAND_IN  int                    pool_id,
    SOC_SAND_IN  int                    is_headroom,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *info 
  ); 
typedef int (*mbcm_dpp_itm_vsq_fc_default_get_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_FC_INFO *vsq_fc_default_info
  );
typedef int (*mbcm_dpp_itm_vsq_tail_drop_default_get_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_TAIL_DROP_INFO *vsq_max_size_default_info
  );
typedef int (*mbcm_dpp_itm_vsq_src_port_rjct_set_f) (
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  int                    pool_id,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_SRC_PORT_INFO       *info,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_SRC_PORT_INFO       *exact_info
  );
typedef int (*mbcm_dpp_itm_vsq_src_port_rjct_get_f) (
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  int                    pool_id,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_SRC_PORT_INFO       *exact_info
  );
typedef int (*mbcm_dpp_itm_vsq_pg_rjct_set_f) (
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                     drop_precedence_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_PG_INFO    *info,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_PG_INFO    *exact_info
  );
typedef int (*mbcm_dpp_itm_vsq_pg_rjct_get_f) (
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                     drop_precedence_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_PG_INFO    *exact_info
  );
typedef int (*mbcm_dpp_itm_vsq_wred_gen_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP       vsq_group_ndx, 
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx, 
    SOC_SAND_IN  int                            pool_id,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_WRED_GEN_INFO *info 
  ); 
typedef int (*mbcm_dpp_itm_vsq_wred_gen_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP       vsq_group_ndx, 
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx, 
    SOC_SAND_IN  int                            pool_id,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_WRED_GEN_INFO *info 
  ); 
typedef int (*mbcm_dpp_itm_vsq_wred_set_f) ( 
    SOC_SAND_IN  int                            unit, 
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP          vsq_group_ndx, 
    SOC_SAND_IN  uint32                         vsq_rt_cls_ndx, 
    SOC_SAND_IN  uint32                         drop_precedence_ndx,
    SOC_SAND_IN  int                            pool_id,
    SOC_SAND_IN  SOC_TMC_ITM_WRED_QT_DP_INFO    *info, 
    SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO    *exact_info 
  ); 
typedef int (*mbcm_dpp_itm_vsq_wred_get_f) ( 
    SOC_SAND_IN  int                            unit, 
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP          vsq_group_ndx, 
    SOC_SAND_IN  uint32                         vsq_rt_cls_ndx, 
    SOC_SAND_IN  uint32                         drop_precedence_ndx, 
    SOC_SAND_IN  int                            pool_id,
    SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO    *info 
  ); 
typedef uint32 (*mbcm_dpp_itm_vsq_counter_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint8               is_ocb_only,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP       vsq_group_ndx, 
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_NDX         vsq_in_group_ndx 
  ); 
typedef uint32 (*mbcm_dpp_itm_vsq_counter_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_OUT uint8                       *is_ocb_only,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_GROUP       *vsq_group_ndx, 
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_NDX         *vsq_in_group_ndx 
  ); 
typedef uint32 (*mbcm_dpp_itm_vsq_counter_read_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_OUT uint32                  *pckt_count 
  ); 
typedef int (*mbcm_dpp_itm_vsq_pg_tc_profile_mapping_set_f) (
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32 vsq_id,
    SOC_SAND_IN int pg_tc_profile
   );
typedef int (*mbcm_dpp_itm_vsq_pg_tc_profile_mapping_get_f) (
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32 src_pp_port,
    SOC_SAND_OUT int *pg_tc_profile
   );
typedef int (*mbcm_dpp_itm_vsq_pg_tc_profile_set_f) (
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core_id,
    SOC_SAND_IN int pg_tc_profile_id,
    SOC_SAND_IN uint32 pg_tc_bitmap
   );
typedef int (*mbcm_dpp_itm_vsq_pg_tc_profile_get_f) (
    SOC_SAND_IN int         unit,
    SOC_SAND_IN int         core_id,
    SOC_SAND_IN int      pg_tc_profile_id,
    SOC_SAND_OUT uint32     *pg_tc_bitmap
   );
typedef int (*mbcm_dpp_itm_vsq_pg_prm_set_f)(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              vsq_rt_cls_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_PG_PRM  *info
  );
typedef int (*mbcm_dpp_itm_vsq_pg_prm_get_f)(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              vsq_rt_cls_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_PG_PRM  *info
  );
typedef int (*mbcm_dpp_itm_vsq_pb_prm_set_f)(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              pg_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_PG_PRM  *info
  );
typedef int (*mbcm_dpp_itm_vsq_pb_prm_get_f)(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              pg_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_PG_PRM  *info
  );
typedef int (*mbcm_dpp_itm_src_vsqs_mapping_set_f)(
       SOC_SAND_IN int unit,
       SOC_SAND_IN int core_id,
       SOC_SAND_IN int src_pp_port,
       SOC_SAND_IN int src_port_vsq_index,
       SOC_SAND_IN int pg_base,
       SOC_SAND_IN uint8 enable
       );
typedef int (*mbcm_dpp_itm_src_vsqs_mapping_get_f)(
       SOC_SAND_IN int unit,
       SOC_SAND_IN int core_id,
       SOC_SAND_IN int src_pp_port,
       SOC_SAND_OUT int *src_port_vsq_index,
       SOC_SAND_OUT int *pg_base,
       SOC_SAND_OUT uint8 *enable
       );
typedef int (*mbcm_dpp_itm_vsq_src_port_get_f) (
       SOC_SAND_IN int    unit,
       SOC_SAND_IN int    core_id,
       SOC_SAND_IN int    src_port_vsq_index,
       SOC_SAND_OUT uint32 *src_pp_port,
       SOC_SAND_OUT uint8  *enable
       );
typedef int (*mbcm_dpp_itm_vsq_pg_mapping_get_f) (
       SOC_SAND_IN int     unit,
       SOC_SAND_IN int     core_id,
       SOC_SAND_IN uint32  pg_vsq_base,
       SOC_SAND_IN int     cosq,
       SOC_SAND_OUT uint32 *src_pp_port,
       SOC_SAND_OUT uint8  *enable
       );
typedef uint32 (*mbcm_dpp_itm_vsq_pg_ocb_set_f)(
       SOC_SAND_IN int unit,
       SOC_SAND_IN int core_id,
       SOC_SAND_IN int pg_base,
       SOC_SAND_IN int numq,
       SOC_SAND_IN uint8* ocb_only
       );
typedef uint32 (*mbcm_dpp_itm_vsq_pg_ocb_get_f)(
       SOC_SAND_IN int unit,
       SOC_SAND_IN int core_id,
       SOC_SAND_IN int pg_base,
       SOC_SAND_IN int numq,
       SOC_SAND_OUT uint8* ocb_only
       );
typedef uint32 (*mbcm_dpp_itm_queue_is_ocb_only_get_f) (
     SOC_SAND_IN  int                 unit,
     SOC_SAND_IN  int                 core,
     SOC_SAND_IN  uint32              queue_ndx,
     SOC_SAND_OUT uint8               *enable
     );

typedef uint32 (*mbcm_dpp_itm_queue_is_ocb_only_set_f) (
     SOC_SAND_IN  int                 unit,
     SOC_SAND_IN  int                 core,
     SOC_SAND_IN  uint32              queue_ndx,
     SOC_SAND_IN  uint8               enable
     );
typedef uint32 (*mbcm_dpp_itm_queue_info_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  uint32                  queue_ndx, 
    SOC_SAND_IN  SOC_TMC_ITM_QUEUE_INFO      *old_info,
    SOC_SAND_IN  SOC_TMC_ITM_QUEUE_INFO      *info 
  ); 
typedef uint32 (*mbcm_dpp_itm_queue_info_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  uint32                  queue_ndx, 
    SOC_SAND_OUT SOC_TMC_ITM_QUEUE_INFO      *info 
  ); 
typedef uint32 (*mbcm_dpp_itm_ingress_shape_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_IN  SOC_TMC_ITM_INGRESS_SHAPE_INFO *info 
  ); 
typedef uint32 (*mbcm_dpp_itm_ingress_shape_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_OUT SOC_TMC_ITM_INGRESS_SHAPE_INFO *info 
  ); 
typedef uint32 (*mbcm_dpp_itm_priority_map_tmplt_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 map_ndx, 
    SOC_SAND_IN  SOC_TMC_ITM_PRIORITY_MAP_TMPLT *info 
  ); 
typedef uint32 (*mbcm_dpp_itm_priority_map_tmplt_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 map_ndx, 
    SOC_SAND_OUT SOC_TMC_ITM_PRIORITY_MAP_TMPLT *info 
  ); 
typedef uint32 (*mbcm_dpp_itm_priority_map_tmplt_select_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                  queue_64_ndx, 
    SOC_SAND_IN  uint32                 priority_map 
  ); 
typedef uint32 (*mbcm_dpp_itm_priority_map_tmplt_select_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                  queue_64_ndx, 
    SOC_SAND_OUT uint32                 *priority_map 
  ); 
typedef uint32 (*mbcm_dpp_itm_sys_red_drop_prob_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_ITM_SYS_RED_DROP_PROB *info 
  ); 
typedef uint32 (*mbcm_dpp_itm_sys_red_drop_prob_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_OUT SOC_TMC_ITM_SYS_RED_DROP_PROB *info 
  ); 
typedef int (*mbcm_dpp_itm_sys_red_queue_size_boundaries_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 rt_cls_ndx, 
    SOC_SAND_IN  SOC_TMC_ITM_SYS_RED_QT_INFO *info, 
    SOC_SAND_OUT SOC_TMC_ITM_SYS_RED_QT_INFO *exact_info 
  ); 
typedef int (*mbcm_dpp_itm_sys_red_queue_size_boundaries_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 rt_cls_ndx, 
    SOC_SAND_OUT SOC_TMC_ITM_SYS_RED_QT_INFO *info 
  ); 
typedef int (*mbcm_dpp_itm_sys_red_q_based_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 rt_cls_ndx, 
    SOC_SAND_IN  uint32                 sys_red_dp_ndx, 
    SOC_SAND_IN  SOC_TMC_ITM_SYS_RED_QT_DP_INFO *info 
  ); 
typedef int (*mbcm_dpp_itm_sys_red_q_based_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 rt_cls_ndx, 
    SOC_SAND_IN  uint32                 sys_red_dp_ndx, 
    SOC_SAND_OUT SOC_TMC_ITM_SYS_RED_QT_DP_INFO *info 
  ); 
typedef uint32 (*mbcm_dpp_itm_sys_red_eg_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_ITM_SYS_RED_EG_INFO *info, 
    SOC_SAND_OUT SOC_TMC_ITM_SYS_RED_EG_INFO *exact_info 
  ); 
typedef uint32 (*mbcm_dpp_itm_sys_red_eg_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_OUT SOC_TMC_ITM_SYS_RED_EG_INFO *info 
  ); 
typedef int (*mbcm_dpp_itm_sys_red_glob_rcs_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_ITM_SYS_RED_GLOB_RCS_INFO *info 
  ); 
typedef int (*mbcm_dpp_itm_sys_red_glob_rcs_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_OUT SOC_TMC_ITM_SYS_RED_GLOB_RCS_INFO *info 
  ); 
typedef uint32 (*mbcm_dpp_itm_vsq_index_global2group_f) ( 
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_NDX   vsq_ndx, 
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_GROUP *vsq_group, 
    SOC_SAND_OUT uint32            *vsq_in_group_ndx,
    SOC_SAND_OUT uint8             *is_ocb_only
  ); 
typedef uint32 (*mbcm_dpp_itm_vsq_index_group2global_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP vsq_group, 
    SOC_SAND_IN  uint32            vsq_in_group_ndx, 
    SOC_SAND_IN  uint8             is_ocb_only,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_NDX   *vsq_ndx 
  );
typedef uint32 (*mbcm_dpp_itm_vsq_index_fc_group2global_f) ( 
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP vsq_group, 
    SOC_SAND_IN  uint32            vsq_in_group_ndx, 
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_NDX   *vsq_ndx 
  ); 
typedef uint32 (*mbcm_dpp_itm_pfc_tc_map_set_f) (
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN int32                    tc_in,
    SOC_SAND_IN int32                    port_id,
    SOC_SAND_IN int32                    tc_out
  ); 
typedef int (*mbcm_dpp_itm_vsq_src_reserve_set_f)(
   SOC_SAND_IN  int                     unit,
   SOC_SAND_IN  int                     core_id,
   SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP   vsq_type,
   SOC_SAND_IN  uint32                  vsq_rt_cls_ndx,
   SOC_SAND_IN  uint32                  drop_precedence_ndx,
   SOC_SAND_IN  int                     pool_id,
   SOC_SAND_IN  uint32                  reserved_amount,
   SOC_SAND_OUT uint32*                 exact_reserved_amount
   );
typedef int (*mbcm_dpp_itm_vsq_src_reserve_get_f)(
   SOC_SAND_IN  int                     unit,
   SOC_SAND_IN  int                     core_id,
   SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP   vsq_type,
   SOC_SAND_IN  uint32                  vsq_rt_cls_ndx,
   SOC_SAND_IN  uint32                  drop_precedence_ndx,
   SOC_SAND_IN  int                     pool_id,
   SOC_SAND_OUT uint32                  *reserved_amount
   );
typedef int (*mbcm_dpp_itm_resource_allocation_set_f)(
   SOC_SAND_IN  int                     unit,
   SOC_SAND_IN  int                     core_id,
   SOC_SAND_IN  uint8                   pool_id,
   SOC_SAND_IN  uint8                   is_ocb_only,
   SOC_SAND_IN  uint32                  max_shared_pool,
   SOC_SAND_IN  uint32                  max_headroom
   );
typedef int (*mbcm_dpp_itm_resource_allocation_get_f)(
   SOC_SAND_IN  int                     unit,
   SOC_SAND_IN  int                     core_id,
   SOC_SAND_IN  uint8                   pool_id,
   SOC_SAND_IN  uint8                   is_ocb_only,
   SOC_SAND_OUT uint32                  *max_shared_pool,
   SOC_SAND_OUT uint32                  *max_headroom
   );
typedef int (*mbcm_dpp_itm_global_resource_allocation_set_f)(
   SOC_SAND_IN int                                  unit, 
   SOC_SAND_IN int                                  core, 
   SOC_SAND_IN SOC_TMC_ITM_INGRESS_CONGESTION_MGMT  *ingress_congestion_mgmt
   );
typedef int (*mbcm_dpp_itm_global_resource_allocation_get_f)(
   SOC_SAND_IN int                                  unit, 
   SOC_SAND_IN int                                  core, 
   SOC_SAND_OUT SOC_TMC_ITM_INGRESS_CONGESTION_MGMT *ingress_congestion_mgmt
   );
typedef int (*mbcm_dpp_itm_dyn_total_thresh_set_f) (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core_id,
    SOC_SAND_IN uint8    is_ocb_only,
                int32    reservation_increase[SOC_DPP_DEFS_MAX(NOF_CORES)][SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES]
  );
typedef int (*mbcm_dpp_itm_queue_dyn_info_get_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_QUEUE_DYN_INFO *info
        );  

typedef int (*mbcm_dpp_itm_congestion_statistics_get_f) (
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core,
    SOC_SAND_OUT ARAD_ITM_CGM_CONGENSTION_STATS *stats
  );

typedef int (*mbcm_dpp_itm_min_free_resources_stat_get_f) (
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core,
    SOC_SAND_IN SOC_TMC_ITM_CGM_RSRC_STAT_TYPE type,
    SOC_SAND_OUT uint64 *value
  );

typedef int (*  mbcm_dpp_itm_per_queue_info_set_f) (
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          queue,
    SOC_SAND_IN   ARAD_IQM_STATIC_TBL_DATA* IQM_static_tbl_data
  );

typedef int (*  mbcm_dpp_itm_per_queue_info_get_f) (
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          queue,
    SOC_SAND_OUT  ARAD_IQM_STATIC_TBL_DATA* IQM_static_tbl_data
  );

typedef int (*  mbcm_dpp_itm_in_pp_port_scheduler_compensation_profile_set_f) (
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          in_pp_port,
    SOC_SAND_IN   int  scheduler_profile
        );

typedef int (*  mbcm_dpp_itm_in_pp_port_scheduler_compensation_profile_get_f) (
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          in_pp_port,
    SOC_SAND_OUT  int*  scheduler_profile
        );

typedef int (*  mbcm_dpp_iqm_dynamic_tbl_get_unsafe_f) (
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_DYNAMIC_TBL_DATA* IQM_dynamic_tbl_data
  );

typedef int (*mbcm_dpp_ingress_drop_status_f) (
    SOC_SAND_IN int        unit,
    SOC_SAND_OUT uint32    *is_max_size
  );

typedef int (*mbcm_dpp_itm_profile_ocb_only_set_f) (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   rate_class,
    SOC_SAND_IN int   is_ocb_only
  );

typedef int (*mbcm_dpp_itm_profile_ocb_only_get_f) (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   rate_class,
    SOC_SAND_OUT int  *is_ocb_only
  );

typedef int (*mbcm_dpp_itm_credits_adjust_size_set_f) (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core,
    SOC_SAND_IN int   index, 
    SOC_SAND_IN SOC_TMC_ITM_CGM_PKT_SIZE_ADJUST_TYPE adjust_type,
    SOC_SAND_IN int   delta,
    SOC_SAND_IN SOC_TMC_ITM_PKT_SIZE_ADJUST_INFO* additional_info
  );

typedef int (*mbcm_dpp_itm_credits_adjust_size_get_f) (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core,
    SOC_SAND_IN int   index, 
    SOC_SAND_IN SOC_TMC_ITM_CGM_PKT_SIZE_ADJUST_TYPE adjust_type,
    SOC_SAND_OUT int  *delta
  );

typedef int (*mbcm_dpp_itm_sch_final_delta_map_set_f) (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core,
    SOC_SAND_IN int   delta,
    SOC_SAND_IN uint32   delta_profile
  );

typedef int (*mbcm_dpp_itm_sch_final_delta_map_get_f) (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core,
    SOC_SAND_IN int   delta,
    SOC_SAND_OUT uint32   *delta_profile
  );

typedef int (*mbcm_dpp_itm_sch_final_delta_set_f) (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core,
    SOC_SAND_IN uint32   delta_profile,
    SOC_SAND_IN int   final_delta
  );

typedef int (*mbcm_dpp_itm_sch_final_delta_get_f) (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core,
    SOC_SAND_IN uint32   delta_profile,
    SOC_SAND_OUT int   *final_delta
  );

typedef uint32 (*mbcm_dpp_itm_pfc_tc_map_get_f) (
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int32                    tc_in,
    SOC_SAND_IN  int32                    port_id,
    SOC_SAND_OUT int32                    *tc_out
  );
typedef soc_error_t (*mbcm_dpp_fc_gen_cal_get_f) (
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID                if_ndx,
    SOC_SAND_OUT SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_OUT SOC_TMC_FC_GEN_CALENDAR             *cal_buff
  );
typedef soc_error_t (*mbcm_dpp_fc_gen_cal_set_f) (
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID                if_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_TMC_FC_GEN_CALENDAR             *cal_buff
  );
typedef soc_error_t (*mbcm_dpp_fc_gen_inbnd_set_f) (
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_port_t                     port,
    SOC_SAND_IN  SOC_TMC_FC_GEN_INBND_INFO      *info
  );
typedef soc_error_t (*mbcm_dpp_fc_gen_inbnd_get_f) (
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_port_t                     port,
    SOC_SAND_OUT  SOC_TMC_FC_GEN_INBND_INFO     *info
  );
typedef uint32 (*mbcm_dpp_fc_gen_inbnd_glb_hp_set_f) (
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_INGR_GEN_GLB_HP_MODE     fc_mode
  );
typedef uint32 (*mbcm_dpp_fc_gen_inbnd_glb_hp_get_f) (
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT  SOC_TMC_FC_INGR_GEN_GLB_HP_MODE     *fc_mode
  );
typedef soc_error_t (*mbcm_dpp_fc_rec_cal_set_f) (
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID                if_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_TMC_FC_REC_CALENDAR             *cal_buff
  );
typedef soc_error_t (*mbcm_dpp_fc_rec_cal_get_f) (
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID                if_ndx,
    SOC_SAND_OUT  SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_OUT  SOC_TMC_FC_REC_CALENDAR             *cal_buff
  );

typedef soc_error_t (*mbcm_dpp_fc_pfc_generic_bitmap_set_f) (
    SOC_SAND_IN   int   unit,
    SOC_SAND_IN   int       priority,
    SOC_SAND_IN   uint32    bitmap_index,
    SOC_SAND_IN  SOC_TMC_FC_PFC_GENERIC_BITMAP    *pfc_bitmap
  );
typedef soc_error_t (*mbcm_dpp_fc_pfc_generic_bitmap_get_f) (
    SOC_SAND_IN   int   unit,
    SOC_SAND_IN   int       priority,
    SOC_SAND_IN   uint32    bitmap_index,
    SOC_SAND_OUT  SOC_TMC_FC_PFC_GENERIC_BITMAP    *pfc_bitmap
  );

typedef int (*mbcm_dpp_fc_port_fifo_threshold_set_f) (
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  soc_port_t                 port,
    SOC_SAND_IN  SOC_TMC_FC_PORT_FIFO_TH    *info
  );
typedef int (*mbcm_dpp_fc_port_fifo_threshold_get_f) (
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  soc_port_t                 port,
    SOC_SAND_OUT SOC_TMC_FC_PORT_FIFO_TH    *info
  );  

 
typedef uint32 (*mbcm_dpp_mult_eg_vlan_membership_group_range_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info 
  ); 
typedef uint32 (*mbcm_dpp_mult_eg_vlan_membership_group_range_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_OUT SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info 
  ); 
typedef uint32 (*mbcm_dpp_mult_eg_group_open_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_MULT_ID             multicast_id_ndx, 
    SOC_SAND_IN  SOC_TMC_MULT_EG_ENTRY       *mc_group, 
    SOC_SAND_IN  uint32                  mc_group_size, 
    SOC_SAND_OUT uint8                 *insufficient_memory 
  ); 
typedef uint32 (*mbcm_dpp_mult_eg_group_update_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_MULT_ID             multicast_id_ndx, 
    SOC_SAND_IN  SOC_TMC_MULT_EG_ENTRY      *mc_group, 
    SOC_SAND_IN  uint32                  mc_group_size, 
    SOC_SAND_OUT uint8                 *insufficient_memory 
  ); 
typedef uint32 (*mbcm_dpp_mult_eg_group_set_f) (
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  ARAD_MULT_ID         multicast_id_ndx,    
    SOC_SAND_IN  uint8                allow_create,        
    SOC_SAND_IN  uint32               group_size,          
    SOC_SAND_IN  dpp_mc_replication_t *reps,               
    SOC_SAND_OUT SOC_TMC_ERROR        *out_err             
  );
typedef uint32 (*mbcm_dpp_mult_eg_group_close_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_MULT_ID             multicast_id_ndx 
  ); 
typedef uint32 (*mbcm_dpp_mult_eg_port_add_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_MULT_ID             multicast_id_ndx, 
    SOC_SAND_IN  SOC_TMC_MULT_EG_ENTRY       *entry, 
    SOC_SAND_OUT SOC_TMC_ERROR            *out_err 
  ); 
typedef uint32 (*mbcm_dpp_mult_eg_reps_add_f) ( 
    SOC_SAND_IN  int                    unit, 
    SOC_SAND_IN  SOC_TMC_MULT_ID        multicast_id_ndx, 
    SOC_SAND_IN  uint32                 nof_reps, 
    SOC_SAND_IN  dpp_mc_replication_t   *reps,    
    SOC_SAND_OUT SOC_TMC_ERROR          *out_err  
  ); 
typedef uint32 (*mbcm_dpp_mult_eg_port_remove_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_MULT_ID             multicast_id_ndx, 
    SOC_SAND_IN  SOC_TMC_MULT_EG_ENTRY       *entry,
    SOC_SAND_OUT SOC_TMC_ERROR            *out_err 
  ); 
typedef uint32 (*mbcm_dpp_mult_eg_reps_remove_f) ( 
    SOC_SAND_IN  int                    unit, 
    SOC_SAND_IN  SOC_TMC_MULT_ID        multicast_id_ndx, 
    SOC_SAND_IN  uint32                 nof_reps, 
    SOC_SAND_IN  dpp_mc_replication_t   *reps,    
    SOC_SAND_OUT SOC_TMC_ERROR          *out_err  
  ); 
typedef uint32 (*mbcm_dpp_mult_eg_group_size_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_MULT_ID             multicast_id_ndx, 
    SOC_SAND_OUT uint32                  *mc_group_size 
  ); 
typedef uint32 (*mbcm_dpp_mult_eg_group_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_MULT_ID             multicast_id_ndx, 
    SOC_SAND_IN  uint32                  mc_group_size, 
    SOC_SAND_OUT  SOC_TMC_MULT_EG_ENTRY      *mc_group, 
    SOC_SAND_OUT uint32                  *exact_mc_group_size, 
    SOC_SAND_OUT uint8                 *is_open 
  ); 
typedef uint32 (*mbcm_dpp_mult_eg_get_group_f) ( 
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  dpp_mc_id_t    group_mcid,           
    SOC_SAND_IN  uint32         mc_group_size,        
    SOC_SAND_OUT soc_gport_t    *ports,               
    SOC_SAND_OUT soc_if_t       *cuds,                
    SOC_SAND_OUT soc_multicast_replication_t *reps,   
    SOC_SAND_OUT uint32         *exact_mc_group_size, 
    SOC_SAND_OUT uint8          *is_open              
  );
typedef uint32 (*mbcm_dpp_mult_eg_vlan_membership_group_open_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_MULT_ID             multicast_id_ndx, 
    SOC_SAND_IN  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *group 
  ); 
typedef uint32 (*mbcm_dpp_mult_eg_bitmap_group_create_f) (  
                 int                 unit,
                 SOC_TMC_MULT_ID     multicast_id_ndx
  ); 
typedef uint32 (*mbcm_dpp_mult_eg_vlan_membership_group_update_f) ( 
                 int                 unit, 
                 SOC_TMC_MULT_ID             multicast_id_ndx, 
                 SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *group 
  ); 
typedef uint32 (*mbcm_dpp_mult_eg_vlan_membership_group_close_f) ( 
                 int                 unit, 
                 SOC_TMC_MULT_ID             multicast_id_ndx 
  ); 
typedef uint32 (*mbcm_dpp_mult_eg_vlan_membership_port_add_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_MULT_ID             multicast_id_ndx, 
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID        port,
    SOC_SAND_OUT SOC_TMC_ERROR            *out_err 
  ); 
typedef uint32 (*mbcm_dpp_mult_eg_vlan_membership_port_remove_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_MULT_ID             multicast_id_ndx, 
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID        port,
    SOC_SAND_OUT SOC_TMC_ERROR            *out_err 
  ); 
typedef uint32 (*mbcm_dpp_mult_eg_vlan_membership_bm_add_f) ( 
                 int                                   unit, 
                 SOC_TMC_MULT_ID                       multicast_id_ndx, 
                 SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *bitmap, 
                 SOC_TMC_ERROR                         *out_err 
  ); 
typedef uint32 (*mbcm_dpp_mult_eg_vlan_membership_bm_remove_f) ( 
                 int                                   unit, 
                 SOC_TMC_MULT_ID                       multicast_id_ndx, 
                 SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *bitmap, 
                 SOC_TMC_ERROR                         *out_err 
  ); 
typedef uint32 (*mbcm_dpp_mult_eg_vlan_membership_group_get_f) ( 
                 int                 unit, 
                 SOC_TMC_MULT_ID             multicast_id_ndx, 
                 SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *group 
  ); 
typedef uint32 (*dpp_mcds_multicast_init2_f) ( 
    SOC_SAND_IN  int                 unit
  ); 
 
typedef uint32 (*mbcm_dpp_mult_fabric_traffic_class_to_multicast_cls_map_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_TR_CLS              tr_cls_ndx, 
    SOC_SAND_IN  SOC_TMC_MULT_FABRIC_CLS     new_mult_cls 
  ); 
typedef uint32 (*mbcm_dpp_mult_fabric_traffic_class_to_multicast_cls_map_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_TR_CLS         tr_cls_ndx, 
    SOC_SAND_OUT SOC_TMC_MULT_FABRIC_CLS     *new_mult_cls 
  ); 
typedef uint32 (*mbcm_dpp_mult_fabric_base_queue_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                  queue_id 
  ); 
typedef uint32 (*mbcm_dpp_mult_fabric_base_queue_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_OUT uint32                  *queue_id 
  ); 
typedef int (*mbcm_dpp_mult_fabric_credit_source_set_f) ( 
    SOC_SAND_IN  int                        unit, 
    SOC_SAND_IN  int                        core, 
    SOC_SAND_IN  SOC_TMC_MULT_FABRIC_INFO   *info
  ); 
typedef int (*mbcm_dpp_mult_fabric_credit_source_get_f) ( 
    SOC_SAND_IN  int                        unit, 
    SOC_SAND_IN  int                        core, 
    SOC_SAND_OUT SOC_TMC_MULT_FABRIC_INFO   *info 
  ); 
typedef uint32 (*mbcm_dpp_mult_fabric_enhanced_set_f) ( 
    SOC_SAND_IN  int                                 unit, 
    SOC_SAND_IN  int                                 core_id, 
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *queue_range 
  ); 
typedef uint32 (*mbcm_dpp_mult_fabric_enhanced_get_f) ( 
    SOC_SAND_IN  int                                 unit, 
    SOC_SAND_IN  int                                 core_id, 
    SOC_SAND_OUT SOC_SAND_U32_RANGE                            *queue_range 
  ); 

typedef uint32 (*mbcm_dpp_mult_fabric_flow_control_set_f) ( 
    SOC_SAND_IN  int                                 unit, 
    SOC_SAND_IN  SOC_TMC_MULT_FABRIC_FLOW_CONTROL_MAP   *fc_map 
  ); 

typedef uint32 (*mbcm_dpp_mult_fabric_flow_control_get_f) ( 
    SOC_SAND_IN  int                                 unit, 
    SOC_SAND_OUT SOC_TMC_MULT_FABRIC_FLOW_CONTROL_MAP   *fc_map 
  ); 

typedef uint32 (*mbcm_dpp_mult_fabric_active_links_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_MULT_FABRIC_ACTIVE_LINKS *links, 
    SOC_SAND_IN  uint8                 tbl_refresh_enable 
  ); 
typedef uint32 (*mbcm_dpp_mult_fabric_active_links_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_OUT SOC_TMC_MULT_FABRIC_ACTIVE_LINKS *links, 
    SOC_SAND_OUT uint8                 *tbl_refresh_enable 
  ); 


typedef uint32 (*mbcm_dpp_mult_does_group_exist_f) ( 
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_MULT_ID mcid,      
    SOC_SAND_IN  int             is_egress, 
    SOC_SAND_OUT uint8           *is_open   
  ); 
 
 
typedef uint32 (*mbcm_dpp_mult_ing_traffic_class_map_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_MULT_ING_TR_CLS_MAP *map 
  ); 
typedef uint32 (*mbcm_dpp_mult_ing_traffic_class_map_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_OUT SOC_TMC_MULT_ING_TR_CLS_MAP *map 
  ); 
typedef uint32 (*mbcm_dpp_mult_ing_group_open_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_MULT_ID             multicast_id_ndx, 
    SOC_SAND_IN  SOC_TMC_MULT_ING_ENTRY      *mc_group, 
    SOC_SAND_IN  uint32                  mc_group_size, 
    SOC_SAND_OUT SOC_TMC_ERROR         *out_err             
  ); 
typedef uint32 (*mbcm_dpp_mult_ing_group_update_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_MULT_ID             multicast_id_ndx, 
    SOC_SAND_IN  SOC_TMC_MULT_ING_ENTRY      *mc_group, 
    SOC_SAND_IN  uint32                  mc_group_size, 
    SOC_SAND_OUT SOC_TMC_ERROR         *out_err             
  ); 
typedef uint32 (*mbcm_dpp_mult_ing_group_close_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_MULT_ID             multicast_id_ndx 
  ); 
typedef uint32 (*mbcm_dpp_mult_ing_destination_add_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_MULT_ID             multicast_id_ndx, 
    SOC_SAND_IN  SOC_TMC_MULT_ING_ENTRY      *entry, 
    SOC_SAND_OUT SOC_TMC_ERROR            *out_err 
  ); 
typedef uint32 (*mbcm_dpp_mult_ing_encode_entry_f) (
    SOC_SAND_IN    int                 unit,
    SOC_SAND_IN    SOC_TMC_MULT_ING_ENTRY *ing_entry,       
    SOC_SAND_OUT   uint32                 *out_destination, 
    SOC_SAND_OUT   uint32                 *out_cud          
  );
typedef uint32 (*mbcm_dpp_mult_ing_destination_remove_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_MULT_ID             multicast_id_ndx, 
    SOC_SAND_IN  SOC_TMC_MULT_ING_ENTRY      *entry,
    SOC_SAND_OUT SOC_TMC_ERROR            *out_err 
  ); 
typedef uint32 (*mbcm_dpp_mult_ing_group_size_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_MULT_ID             multicast_id_ndx, 
    SOC_SAND_OUT uint32                 *mc_group_size 
  ); 
typedef uint32 (*mbcm_dpp_mult_ing_group_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_MULT_ID             multicast_id_ndx, 
    SOC_SAND_IN  uint32                 mc_group_size, 
    SOC_SAND_OUT SOC_TMC_MULT_ING_ENTRY      *mc_group, 
    SOC_SAND_OUT uint32                  *exact_mc_group_size, 
    SOC_SAND_OUT uint8                 *is_open 
  ); 
typedef uint32 (*mbcm_dpp_mult_ing_get_group_f) (
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  ARAD_MULT_ID        group_mcid,           
    SOC_SAND_IN  uint32              mc_group_size,        
    SOC_SAND_OUT soc_gport_t         *ports,               
    SOC_SAND_OUT soc_if_t            *cuds,                
    SOC_SAND_OUT soc_multicast_replication_t *reps,        
    SOC_SAND_OUT uint32              *exact_mc_group_size, 
    SOC_SAND_OUT uint8               *is_open              
  );
 
 
typedef uint32 (*mbcm_dpp_pkt_packet_callback_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_PKT_PACKET_TRANSFER *packet_transfer 
  ); 
typedef uint32 (*mbcm_dpp_pkt_packet_callback_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_OUT SOC_TMC_PKT_PACKET_TRANSFER *packet_transfer 
  ); 
typedef uint32 (*mbcm_dpp_pkt_packet_send_f) ( 
    SOC_SAND_IN  int                    unit, 
    SOC_SAND_IN  SOC_TMC_PKT_TX_PACKET_INFO     *tx_packet 
  ); 
typedef uint32 (*mbcm_dpp_pkt_packet_recv_f) ( 
    SOC_SAND_IN  int                    unit, 
    SOC_SAND_IN  uint32                     bytes_to_get, 
    SOC_SAND_OUT SOC_TMC_PKT_RX_PACKET_INFO     *rx_packet 
  ); 
typedef uint32 (*mbcm_dpp_pkt_packet_receive_mode_set_f) ( 
    SOC_SAND_IN  SOC_TMC_PKT_PACKET_RECV_MODE  recv_mode 
  ); 
typedef uint32 (*mbcm_dpp_pkt_packet_receive_mode_get_f) ( 
    SOC_SAND_OUT SOC_TMC_PKT_PACKET_RECV_MODE  *recv_mode 
  ); 
 
 
typedef uint32 (*mbcm_dpp_sys_phys_to_local_port_map_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 sys_phys_port_ndx, 
    SOC_SAND_IN  uint32                 mapped_fap_id, 
    SOC_SAND_IN  uint32                 mapped_fap_port_id 
  ); 
typedef uint32 (*mbcm_dpp_sys_phys_to_local_port_map_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 sys_phys_port_ndx, 
    SOC_SAND_OUT uint32                 *mapped_fap_id, 
    SOC_SAND_OUT uint32                 *mapped_fap_port_id 
  ); 
typedef uint32 (*mbcm_dpp_local_to_sys_phys_port_map_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 fap_ndx, 
    SOC_SAND_IN  uint32                 fap_local_port_ndx, 
    SOC_SAND_OUT uint32                 *sys_phys_port_id 
  ); 
typedef uint32 (*mbcm_dpp_modport_to_sys_phys_port_map_get_f) ( 
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32  fap_id,            
    SOC_SAND_IN  uint32  tm_port,           
    SOC_SAND_OUT uint32  *sys_phys_port_id  
  );
typedef int (*mbcm_dpp_port_to_interface_map_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_port_t          port,
    SOC_SAND_IN  int                 unmap,
    SOC_SAND_IN  int                 is_dynamic_port
  ); 
typedef int (*mbcm_dpp_port_to_interface_map_get_f) ( 
    SOC_SAND_IN  int                    unit, 
    SOC_SAND_IN  int                    core, 
    SOC_SAND_IN  uint32                 tm_port, 
    SOC_SAND_OUT SOC_TMC_INTERFACE_ID   *if_id, 
    SOC_SAND_OUT uint32                 *channel_id 
  ); 
 typedef uint32 (*mbcm_dpp_is_port_lag_member_f)( 
     SOC_SAND_IN  int                                 unit, 
     SOC_SAND_IN  int                                 core_id, 
     SOC_SAND_IN  uint32                                 port_id, 
     SOC_SAND_OUT uint8                                 *is_in_lag, 
     SOC_SAND_OUT uint32                                 *lag_id 
     ); 
typedef uint32 (*mbcm_dpp_ports_lag_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_PORT_DIRECTION      direction_ndx, 
    SOC_SAND_IN  uint32                 lag_ndx, 
    SOC_SAND_IN  SOC_TMC_PORTS_LAG_INFO      *info 
  ); 
typedef uint32 (*mbcm_dpp_ports_lag_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 lag_ndx, 
    SOC_SAND_OUT SOC_TMC_PORTS_LAG_INFO      *info_incoming, 
    SOC_SAND_OUT SOC_TMC_PORTS_LAG_INFO      *info_outgoing 
  ); 
typedef uint32 (*mbcm_dpp_ports_lag_sys_port_add_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_PORT_DIRECTION      direction_ndx, 
    SOC_SAND_IN  uint32                 lag_ndx, 
    SOC_SAND_IN  uint32                 sys_port 
  ); 
typedef uint32 (*mbcm_dpp_ports_lag_member_add_f) ( 
    SOC_SAND_IN  int                                 unit, 
    SOC_SAND_IN  SOC_TMC_PORT_DIRECTION                      direction_ndx, 
    SOC_SAND_IN  uint32                                 lag_ndx, 
    SOC_SAND_IN  SOC_TMC_PORTS_LAG_MEMBER                    *lag_member, 
    SOC_SAND_OUT uint8                                 *success 
  ); 
typedef uint32 (*mbcm_dpp_ports_lag_sys_port_remove_f) (
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_PORT_DIRECTION      direction_ndx, 
    SOC_SAND_IN  uint32                 lag_ndx, 
    SOC_SAND_IN  SOC_TMC_PORTS_LAG_MEMBER                    *lag_member
  ); 
typedef uint32 (*mbcm_dpp_ports_lag_order_preserve_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                  lag_ndx, 
    SOC_SAND_IN  uint8                 is_order_preserving 
  ); 
typedef uint32 (*mbcm_dpp_ports_lag_order_preserve_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                  lag_ndx, 
    SOC_SAND_OUT uint8                 *is_order_preserving 
  ); 
typedef uint32 (*mbcm_dpp_port_header_type_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                 port_ndx, 
    SOC_SAND_IN  SOC_TMC_PORT_DIRECTION      direction_ndx, 
    SOC_SAND_IN  SOC_TMC_PORT_HEADER_TYPE    header_type 
  ); 
typedef uint32 (*mbcm_dpp_port_header_type_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                 port_ndx, 
    SOC_SAND_OUT SOC_TMC_PORT_HEADER_TYPE    *header_type_incoming, 
    SOC_SAND_OUT SOC_TMC_PORT_HEADER_TYPE    *header_type_outgoing 
  ); 
typedef uint32 (*mbcm_dpp_ports_mirror_inbound_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 ifp_ndx, 
    SOC_SAND_IN  SOC_TMC_PORT_INBOUND_MIRROR_INFO *info 
  ); 
typedef uint32 (*mbcm_dpp_ports_mirror_inbound_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 ifp_ndx, 
    SOC_SAND_OUT SOC_TMC_PORT_INBOUND_MIRROR_INFO *info 
  ); 
typedef uint32 (*mbcm_dpp_ports_mirror_outbound_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 ofp_ndx, 
    SOC_SAND_IN  SOC_TMC_PORT_OUTBOUND_MIRROR_INFO *info 
  ); 
typedef uint32 (*mbcm_dpp_ports_mirror_outbound_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 ofp_ndx, 
    SOC_SAND_OUT SOC_TMC_PORT_OUTBOUND_MIRROR_INFO *info 
  );
typedef int (*mbcm_dpp_ports_mirror_to_rcy_port_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              pp_port,
    SOC_SAND_OUT soc_port_t          *rcy_mirror_port
  ); 
typedef uint32 (*mbcm_dpp_ports_snoop_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 snoop_cmd_ndx, 
    SOC_SAND_IN  SOC_TMC_PORT_SNOOP_INFO     *info 
  ); 
typedef uint32 (*mbcm_dpp_ports_snoop_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 snoop_cmd_ndx, 
    SOC_SAND_OUT SOC_TMC_PORT_SNOOP_INFO     *info 
  ); 
typedef uint32 (*mbcm_dpp_ports_itmh_extension_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID         port_ndx, 
    SOC_SAND_IN  uint8                 ext_en 
  ); 
typedef uint32 (*mbcm_dpp_ports_itmh_extension_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID         port_ndx, 
    SOC_SAND_OUT uint8                 *ext_en 
  ); 
typedef uint32 (*mbcm_dpp_ports_shaping_header_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID         port_ndx, 
    SOC_SAND_IN  SOC_TMC_PORTS_ISP_INFO      *info 
  ); 
typedef uint32 (*mbcm_dpp_ports_shaping_header_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID         port_ndx, 
    SOC_SAND_OUT SOC_TMC_PORTS_ISP_INFO      *info 
  ); 
typedef uint32 (*mbcm_dpp_ports_forwarding_header_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID         port_ndx, 
    SOC_SAND_IN  SOC_TMC_PORTS_ITMH          *info 
  ); 
typedef uint32 (*mbcm_dpp_ports_forwarding_header_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID         port_ndx, 
    SOC_SAND_OUT SOC_TMC_PORTS_ITMH          *info 
  ); 
typedef uint32 (*mbcm_dpp_ports_stag_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID         port_ndx, 
    SOC_SAND_IN  SOC_TMC_PORTS_STAG_FIELDS   *info 
  ); 
typedef uint32 (*mbcm_dpp_ports_stag_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID         port_ndx, 
    SOC_SAND_OUT SOC_TMC_PORTS_STAG_FIELDS   *info 
  ); 
typedef uint32 (*mbcm_dpp_ports_ftmh_extension_set_f) ( 
    SOC_SAND_IN  int                   unit, 
    SOC_SAND_IN  SOC_TMC_PORTS_FTMH_EXT_OUTLIF ext_option 
  ); 
typedef uint32 (*mbcm_dpp_ports_ftmh_extension_get_f) ( 
    SOC_SAND_IN  int                   unit, 
    SOC_SAND_OUT SOC_TMC_PORTS_FTMH_EXT_OUTLIF *ext_option 
  );
typedef int (*mbcm_dpp_ports_reference_clock_set_f) ( 
    SOC_SAND_IN  int                   unit, 
    SOC_SAND_OUT soc_port_t            port
  ); 
typedef int (*mbcm_dpp_ports_port_to_nif_id_get_f) ( 
      SOC_SAND_IN   int                   unit,
      SOC_SAND_IN   int                   core,
      SOC_SAND_IN   uint32                tm_port,
      SOC_SAND_OUT  ARAD_INTERFACE_ID     *if_id
  );
 
typedef int (*mbcm_dpp_ports_mirrored_channel_and_context_map_f) (
                                             int    unit,
                                             int    core,
                                             uint32 termination_context,
                                             uint32 reassembly_context,
                                             uint32 channel
  );

 
typedef uint32 (*mbcm_dpp_port_egr_hdr_credit_discount_type_set_f) ( 
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  SOC_TMC_PORT_HEADER_TYPE    port_hdr_type_ndx, 
    SOC_SAND_IN  SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type_ndx, 
    SOC_SAND_IN  SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO *info 
  ); 
typedef uint32 (*mbcm_dpp_port_egr_hdr_credit_discount_type_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_PORT_HEADER_TYPE    port_hdr_type_ndx, 
    SOC_SAND_IN  SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type_ndx, 
    SOC_SAND_OUT SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO *info 
  ); 
typedef uint32 (*mbcm_dpp_port_egr_hdr_credit_discount_select_set_f) ( 
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core, 
    SOC_SAND_IN  uint32                 tm_port, 
    SOC_SAND_IN  SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type 
  ); 
typedef uint32 (*mbcm_dpp_port_egr_hdr_credit_discount_select_get_f) ( 
    SOC_SAND_IN  int                    unit, 
    SOC_SAND_IN  int                    core, 
    SOC_SAND_IN  uint32                 tm_port, 
    SOC_SAND_OUT SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_TYPE *cr_discnt_type 
  ); 
typedef uint32 (*mbcm_dpp_port_stacking_info_set_f) ( 
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  int                              core_id,
    SOC_SAND_IN  uint32                              local_port_ndx,
    SOC_SAND_IN  uint32                              is_stacking,
    SOC_SAND_IN  uint32                              peer_tmd 
  );

typedef uint32 (*mbcm_dpp_port_stacking_info_get_f) ( 
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  int                              core_id,
    SOC_SAND_IN  uint32                              local_port_ndx,
    SOC_SAND_OUT  uint32                              *is_stacking,
    SOC_SAND_OUT  uint32                              *peer_tmd 
  ); 

typedef uint32 (*mbcm_dpp_port_stacking_route_history_bitmap_set_f) ( 
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  int                              core_id,
    SOC_SAND_IN  uint32                              local_port_ndx,
    SOC_SAND_IN  SOC_TMC_STACK_EGR_PROG_TM_PORT_PROFILE_STACK tm_port_profile_stack,
    SOC_SAND_IN  uint32                              bitmap
  ); 

typedef uint32 (*mbcm_dpp_port_direct_lb_key_set_f) (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32 local_port,
    SOC_SAND_IN uint32 min_lb_key,
    SOC_SAND_IN uint32 set_min,
    SOC_SAND_IN uint32 max_lb_key,
    SOC_SAND_IN uint32 set_max
  );

typedef uint32 (*mbcm_dpp_port_direct_lb_key_get_f) (
      SOC_SAND_IN   int unit, 
      SOC_SAND_IN   int core_id,
      SOC_SAND_IN   uint32  local_port,
      SOC_SAND_OUT  uint32 *min_lb_key,
      SOC_SAND_OUT  uint32 *max_lb_key
  );

typedef uint32 (*mbcm_dpp_port_direct_lb_key_min_set_f) (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_IN uint32  min_lb_key
  );

typedef uint32 (*mbcm_dpp_port_direct_lb_key_max_set_f) (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_IN uint32  max_lb_key
  );

typedef uint32 (*mbcm_dpp_port_direct_lb_key_min_get_f) (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32  local_port,
    uint32* min_lb_key
  );

typedef uint32 (*mbcm_dpp_port_direct_lb_key_max_get_f) (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32  local_port,
    uint32* max_lb_key
  );

typedef uint32 (*mbcm_dpp_port_synchronize_lb_key_tables_at_egress_f) (
    SOC_SAND_IN int unit
  );

typedef uint32 (*mbcm_dpp_port_switch_lb_key_tables_f) (
    SOC_SAND_IN int unit
  );

typedef int (*mbcm_dpp_port_rx_enable_get_f) (
    int unit, 
    soc_port_t port,
    int* enable
  );

typedef int (*mbcm_dpp_port_rx_enable_set_f) (
    int unit, 
    soc_port_t local_port,
    int enable
  );

typedef int (*mbcm_dpp_port_tx_enable_get_f) (
    int unit, 
    soc_port_t port,
    int* enable
  );

typedef int (*mbcm_dpp_port_tx_enable_set_f) (
    int unit, 
    soc_port_t local_port,
    int enable
  );

typedef uint32 (*mbcm_dpp_port_ingr_reassembly_context_get_f) (
        SOC_SAND_IN int         unit,
        SOC_SAND_IN soc_port_t  port,
        SOC_SAND_OUT uint32     *port_termination_context,
        SOC_SAND_OUT uint32     *reassembly_context
    );

typedef uint32 (*mbcm_dpp_port_rate_egress_pps_set_f) (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_IN uint32 pps, 
    SOC_SAND_IN uint32 burst
    );

typedef uint32 (*mbcm_dpp_port_rate_egress_pps_get_f) (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_OUT uint32 *pps, 
    SOC_SAND_OUT uint32 *burst
    );


typedef uint32 (*mbcm_dpp_ports_swap_set_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID       port_ndx,
    SOC_SAND_IN  ARAD_PORTS_SWAP_INFO   *info
  );

typedef uint32 (*mbcm_dpp_ports_swap_get_f) (
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT ARAD_PORTS_SWAP_INFO     *info
  );


typedef uint32 (*mbcm_dpp_ports_pon_tunnel_info_set_f) (
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_SAND_PON_TUNNEL_ID   tunnel,
    SOC_SAND_IN  ARAD_PORTS_PON_TUNNEL_INFO *info
  );

typedef uint32 (*mbcm_dpp_ports_pon_tunnel_info_get_f) (
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_SAND_PON_TUNNEL_ID   tunnel,
    SOC_SAND_OUT ARAD_PORTS_PON_TUNNEL_INFO *info
  );


typedef int (*mbcm_dpp_ports_extender_mapping_enable_set_f) ( 
      SOC_SAND_IN   int                   unit,
      SOC_SAND_IN   soc_port_t            port,
      SOC_SAND_IN   int                   value
  ); 

typedef int (*mbcm_dpp_ports_extender_mapping_enable_get_f) ( 
      SOC_SAND_IN   int                   unit,
      SOC_SAND_IN   soc_port_t            port,
      SOC_SAND_OUT  int                   *value
  ); 

typedef int (*mbcm_dpp_pon_ports_enable_set_f) ( 
      SOC_SAND_IN   int                   unit,
      SOC_SAND_IN   soc_port_t            port,
      SOC_SAND_IN   int                   value
  ); 

typedef int (*mbcm_dpp_pon_ports_enable_get_f) ( 
      SOC_SAND_IN   int                   unit,
      SOC_SAND_IN   soc_port_t            port,
      SOC_SAND_OUT   int                   *value
  );

typedef int (*mbcm_dpp_ports_tm_port_var_set_f) ( 
      SOC_SAND_IN   int                   unit,
      SOC_SAND_IN   soc_port_t            port,
      SOC_SAND_IN   int                   value
  ); 

typedef int (*mbcm_dpp_ports_tm_port_var_get_f) ( 
      SOC_SAND_IN   int                   unit,
      SOC_SAND_IN   soc_port_t            port,
      SOC_SAND_OUT  int                   *value
  ); 

typedef int (*mbcm_dpp_port_protocol_offset_verify_f) ( 
      SOC_SAND_IN   int                   unit,
      SOC_SAND_IN   soc_port_t            port,
      SOC_SAND_IN   uint32                protocol_offset
  ); 

typedef int (* mbcm_dpp_port_cable_diag_f) (
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN soc_port_t              port,
    SOC_SAND_OUT soc_port_cable_diag_t  *status
  );

 
typedef uint32 (*mbcm_dpp_read_fld_f) ( 
    SOC_SAND_IN  int      unit, 
    SOC_SAND_IN  SOC_TMC_REG_FIELD*  field, 
    SOC_SAND_IN  uint32      instance_idx, 
    SOC_SAND_OUT uint32*         val 
  ); 
typedef uint32 (*mbcm_dpp_write_fld_f) ( 
    SOC_SAND_IN  int          unit, 
    SOC_SAND_IN  SOC_TMC_REG_FIELD   *field, 
    SOC_SAND_IN  uint32          instance_idx, 
    SOC_SAND_IN  uint32           val 
  ); 
typedef uint32 (*mbcm_dpp_read_reg_f) ( 
    SOC_SAND_IN  int      unit, 
    SOC_SAND_IN  SOC_TMC_REG_ADDR*   reg, 
    SOC_SAND_IN  uint32      instance_idx, 
    SOC_SAND_OUT uint32*         val 
  ); 
typedef uint32 (*mbcm_dpp_write_reg_f) ( 
    SOC_SAND_IN  int          unit, 
    SOC_SAND_IN  SOC_TMC_REG_ADDR    *reg, 
    SOC_SAND_IN  uint32          instance_idx, 
    SOC_SAND_IN  uint32           val 
  ); 
typedef uint32 (*mbcm_dpp_status_fld_poll_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_REG_FIELD           *field, 
    SOC_SAND_IN  uint32                 instance_ndx, 
    SOC_SAND_IN  SOC_TMC_POLL_INFO           *poll_info, 
    SOC_SAND_OUT uint8                 *success 
  ); 
typedef int (*mbcm_dpp_brdc_fsrd_blk_id_set_f) ( 
    SOC_SAND_IN  int                 unit
  ); 
 
typedef int (*mbcm_dpp_mgmt_credit_worth_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32              credit_worth 
  ); 

typedef int (*mbcm_dpp_mgmt_credit_worth_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_OUT uint32              *credit_worth 
  ); 

typedef uint32 (*mbcm_dpp_mgmt_credit_worth_remote_set_f) (
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    credit_worth_remote
  );

typedef uint32 (*mbcm_dpp_mgmt_credit_worth_remote_get_f) (
    SOC_SAND_IN  int    unit,
    SOC_SAND_OUT uint32    *credit_worth_remote
  );


typedef uint32 (*mbcm_dpp_mgmt_change_all_faps_credit_worth_unsafe_f) (
    SOC_SAND_IN  int    unit,
    SOC_SAND_OUT uint8     credit_value_to_use
  );


typedef uint32 (*mbcm_dpp_mgmt_module_to_credit_worth_map_set_f) (
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    fap_id,
    SOC_SAND_IN  uint32    credit_value_type
  );

typedef uint32 (*mbcm_dpp_mgmt_module_to_credit_worth_map_get_f) (
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    fap_id,
    SOC_SAND_OUT uint32    *credit_value_type
  );


typedef uint32 (*mbcm_dpp_mgmt_all_ctrl_cells_enable_get_f) ( 
    SOC_SAND_IN  int  unit, 
    SOC_SAND_OUT uint8  *enable 
  ); 
typedef uint32 (*mbcm_dpp_mgmt_all_ctrl_cells_enable_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint8                 enable 
  ); 
typedef uint32 (*mbcm_dpp_force_tdm_bypass_traffic_to_fabric_set_f) ( 
    SOC_SAND_IN  int        unit, 
    SOC_SAND_IN  int        enable 
  ); 
typedef uint32 (*mbcm_dpp_force_tdm_bypass_traffic_to_fabric_get_f) ( 
    SOC_SAND_IN  int        unit, 
    SOC_SAND_OUT int        *enable 
  ); 
typedef uint32 (*mbcm_dpp_mgmt_enable_traffic_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint8                 enable 
  ); 
typedef uint32 (*mbcm_dpp_mgmt_enable_traffic_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_OUT uint8                 *enable 
  ); 
typedef uint32 (*mbcm_dpp_register_device_f) ( 
    uint32                           *base_address,
    SOC_SAND_IN  SOC_SAND_RESET_DEVICE_FUNC_PTR reset_device_ptr,
    SOC_SAND_OUT int                 *unit_ptr
  ); 
typedef uint32 (*mbcm_dpp_unregister_device_f) ( 
    SOC_SAND_IN  int                 unit
  ); 
typedef uint32 (*mbcm_dpp_mgmt_system_fap_id_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 sys_fap_id 
  ); 
typedef uint32 (*mbcm_dpp_mgmt_system_fap_id_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_OUT uint32                 *sys_fap_id 
  ); 

typedef uint32 (*mbcm_dpp_mgmt_tm_domain_set_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32                 tm_domain 
  ); 
typedef uint32 (*mbcm_dpp_mgmt_tm_domain_get_f) ( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_OUT uint32                 *tm_domain 
  ); 
typedef int (*mbcm_dpp_mgmt_dma_fifo_channel_free_find_f) ( 
    SOC_SAND_IN int     unit,
    SOC_SAND_IN uint8   skip_pci_cmc,
    SOC_SAND_OUT int    *channel_number
  ); 
typedef int (*mbcm_dpp_mgmt_dma_fifo_channel_set_f) ( 
    SOC_SAND_IN int unit,
    SOC_SAND_IN int channel,
    SOC_SAND_IN dma_fifo_channel_src_t value
  ); 

typedef int (*mbcm_dpp_mgmt_dma_fifo_channel_get_f) ( 
    SOC_SAND_IN int unit,
    SOC_SAND_IN dma_fifo_channel_src_t source,
    SOC_SAND_OUT int* channel
  ); 
  
 
 
typedef uint32 (*mbcm_dpp_hpu_itmh_build_verify_f) ( 
    SOC_SAND_IN  SOC_TMC_PORTS_ITMH            *info 
  ); 
typedef uint32 (*mbcm_dpp_hpu_ftmh_build_verify_f) ( 
    SOC_SAND_IN  SOC_TMC_HPU_FTMH            *info 
  ); 
typedef uint32 (*mbcm_dpp_hpu_otmh_build_verify_f) ( 
    SOC_SAND_IN  SOC_TMC_HPU_OTMH            *info 
  ); 
typedef uint32 (*mbcm_dpp_hpu_itmh_build_f) ( 
    SOC_SAND_IN  SOC_TMC_PORTS_ITMH          *info, 
    SOC_SAND_OUT SOC_TMC_HPU_ITMH_HDR        *itmh 
  ); 
typedef uint32 (*mbcm_dpp_hpu_itmh_parse_f) ( 
    SOC_SAND_IN  SOC_TMC_HPU_ITMH_HDR        *itmh, 
    SOC_SAND_OUT SOC_TMC_PORTS_ITMH          *info 
  ); 
typedef uint32 (*mbcm_dpp_hpu_ftmh_build_f) ( 
    SOC_SAND_IN  SOC_TMC_HPU_FTMH            *info, 
    SOC_SAND_OUT SOC_TMC_HPU_FTMH_HDR        *ftmh 
  ); 
typedef uint32 (*mbcm_dpp_hpu_ftmh_parse_f) ( 
    SOC_SAND_IN  SOC_TMC_HPU_FTMH_HDR        *ftmh, 
    SOC_SAND_OUT SOC_TMC_HPU_FTMH            *info 
  ); 
typedef uint32 (*mbcm_dpp_hpu_otmh_build_f) ( 
    SOC_SAND_IN  SOC_TMC_HPU_OTMH            *info, 
    SOC_SAND_OUT SOC_TMC_HPU_OTMH_HDR        *otmh 
  ); 
typedef uint32 (*mbcm_dpp_hpu_otmh_parse_f) ( 
    SOC_SAND_IN  SOC_TMC_HPU_OTMH_HDR        *otmh, 
    SOC_SAND_OUT SOC_TMC_HPU_OTMH            *info 
  ); 
typedef int (*mbcm_dpp_loopback_set_f) (
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  soc_port_t                 port,
    SOC_SAND_IN  soc_dcmn_loopback_mode_t   loopback
  );
typedef int (*mbcm_dpp_loopback_get_f) (
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  soc_port_t                 port,
    SOC_SAND_OUT soc_dcmn_loopback_mode_t   *loopback
  ); 
typedef uint32 (*mbcm_dpp_nif_synce_clk_sel_port_set_f) (
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      synce_cfg_num,
    SOC_SAND_IN  soc_port_t                  port
  ); 
typedef uint32 (*mbcm_dpp_nif_synce_clk_sel_port_get_f) (
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      synce_cfg_num,
    SOC_SAND_OUT soc_port_t                  *port
  ); 
typedef uint32 (*mbcm_dpp_nif_synce_clk_div_set_f) (
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                    synce_cfg_num,
    SOC_SAND_IN  ARAD_NIF_SYNCE_CLK_DIV   clk_div
  ); 
typedef uint32 (*mbcm_dpp_nif_synce_clk_div_get_f) (
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   synce_cfg_num,
    SOC_SAND_OUT ARAD_NIF_SYNCE_CLK_DIV*   clk_div
  ); 
typedef uint32 (*mbcm_dpp_nif_synce_clk_squelch_set_f) (
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      synce_cfg_num,
    SOC_SAND_IN  int                   enable
  );
typedef uint32 (*mbcm_dpp_nif_synce_clk_squelch_get_f) (
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      synce_cfg_num,
    SOC_SAND_OUT int                   *enable
  );

typedef uint32(*mbcm_dpp_port_link_up_mac_update_f) (
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  soc_port_t               port, 
    SOC_SAND_IN  int                      link
  );

typedef uint32 (*mbcm_dpp_flow_and_up_info_get_f) (
    SOC_SAND_IN     int                          unit,
    SOC_SAND_IN     int                          core,
    SOC_SAND_IN     uint32                          flow_id,
    SOC_SAND_IN     uint32                          reterive_status,
    SOC_SAND_INOUT  SOC_TMC_SCH_FLOW_AND_UP_INFO    *flow_and_up_info
  );

typedef uint32 (*mbcm_dpp_ips_non_empty_queues_info_get_f) (  
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                first_queue,
    SOC_SAND_IN  uint32                max_array_size,
    SOC_SAND_OUT soc_ips_queue_info_t* queues,
    SOC_SAND_OUT uint32*               nof_queues_filled,
    SOC_SAND_OUT uint32*               next_queue,
    SOC_SAND_OUT uint32*               reached_end
  );

typedef int (*mbcm_dpp_itm_dp_discard_set_f) (
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN  uint32                  discard_dp
  );
 
typedef int (*mbcm_dpp_itm_dp_discard_get_f) (
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT uint32                  *discard_dp
  ); 

typedef uint32 (*mbcm_dpp_itm_alpha_set_f) (
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32       rt_cls_ndx,
    SOC_SAND_IN  uint32       drop_precedence_ndx,
    SOC_SAND_IN  int32        alpha 
  );

typedef uint32 (*mbcm_dpp_itm_alpha_get_f) (
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32       rt_cls_ndx,
    SOC_SAND_IN  uint32       drop_precedence_ndx,
    SOC_SAND_OUT int32        *alpha 
  );

typedef uint32 (*mbcm_dpp_itm_fair_adaptive_tail_drop_enable_set_f) (
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT uint8                   enabled
  ); 

typedef uint32 (*mbcm_dpp_itm_fair_adaptive_tail_drop_enable_get_f) (
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT uint8                   *enabled
  ); 

typedef uint32 (*mbcm_dpp_ports_application_mapping_info_set_f) (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN SOC_TMC_FAP_PORT_ID port_ndx, 
    SOC_SAND_IN SOC_TMC_PORTS_APPLICATION_MAPPING_INFO *info
  );
 
typedef uint32 (*mbcm_dpp_ports_application_mapping_info_get_f) (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN SOC_TMC_FAP_PORT_ID port_ndx, 
    SOC_SAND_INOUT SOC_TMC_PORTS_APPLICATION_MAPPING_INFO *info
  ); 



typedef uint32 (*mbcm_dpp_ofp_rates_port_priority_max_burst_for_fc_queues_set_f) (
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 max_burst_fc_queues
    );
typedef uint32 (*mbcm_dpp_ofp_rates_port_priority_max_burst_for_empty_queues_set_f) (
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 max_burst_empty_queues
    );
typedef uint32 (*mbcm_dpp_ofp_rates_port_priority_max_burst_for_fc_queues_get_f) (
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT  uint32 *max_burst_fc_queues
    );
typedef uint32 (*mbcm_dpp_ofp_rates_port_priority_max_burst_for_empty_queues_get_f) (
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT  uint32 *max_burst_empty_queues
    );
typedef int (*mbcm_dpp_ofp_rates_sch_single_port_rate_sw_set_f)(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,
    SOC_SAND_IN  uint32                rate
  );
typedef int (*mbcm_dpp_ofp_rates_sch_single_port_rate_hw_set_f)(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,
    SOC_SAND_IN  uint32                is_alt_calc

  );
typedef int (*mbcm_dpp_ofp_rates_egq_single_port_rate_sw_set_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  uint32              rate
  );

typedef int (*mbcm_dpp_ofp_rates_egq_single_port_rate_sw_get_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_OUT uint32              *rate
  );

typedef int (*mbcm_dpp_ofp_rates_egq_single_port_rate_hw_set_f)(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  uint32              is_alt_calc

  );
typedef int (*mbcm_dpp_ofp_rates_sch_single_port_rate_hw_get_f) (
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_OUT uint32                 *rate
  );
typedef int(*mbcm_dpp_ofp_rates_egq_single_port_rate_hw_get_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_OUT uint32              *rate
  );
typedef int(*mbcm_dpp_ofp_rates_single_port_max_burst_set_f) (
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  uint32                 max_burst
  );
typedef int (*mbcm_dpp_ofp_rates_single_port_max_burst_get_f) (
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_OUT uint32                 *max_burst
  );
typedef int (*mbcm_dpp_ofp_rates_egq_interface_shaper_set_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  SOC_TMC_OFP_SHPR_UPDATE_MODE rate_update_mode,
    SOC_SAND_IN  uint32              if_shaper_rate
  );
typedef int (*mbcm_dpp_ofp_rates_egq_interface_shaper_get_f) (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_OUT uint32              *if_shaper_rate
  );
typedef uint32(*mbcm_dpp_ofp_rates_egq_tcg_rate_sw_set_f) (
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                tcg_ndx, 
    SOC_SAND_IN  uint32                rate
    );
typedef uint32 (*mbcm_dpp_ofp_rates_egq_tcg_rate_hw_set_f)(
    SOC_SAND_IN  int                unit
    );
typedef uint32 (*mbcm_dpp_ofp_rates_sch_tcg_rate_set_f) (
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             tcg_ndx, 
    SOC_SAND_IN  uint32             rate
    );
typedef uint32 (*mbcm_dpp_ofp_rates_egq_tcg_rate_hw_get_f) (
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,   
    SOC_SAND_IN  uint32                 tcg_ndx, 
    SOC_SAND_OUT uint32                *rate
    );
typedef uint32 (*mbcm_dpp_ofp_rates_sch_tcg_rate_get_f) (
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             tcg_ndx, 
    SOC_SAND_OUT uint32             *rate
    );
typedef int (*mbcm_dpp_ofp_rates_sch_tcg_shaper_disable_f) (
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            base_qpair,
    SOC_SAND_IN     uint32            nof_q_pairs
    );

typedef uint32 (*mbcm_dpp_ofp_rates_egq_tcg_max_burst_set_f)(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             tcg_ndx, 
    SOC_SAND_IN  uint32             max_burst
    );
typedef uint32 (*mbcm_dpp_ofp_rates_sch_tcg_max_burst_set_f)(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             tcg_ndx, 
    SOC_SAND_IN  uint32             max_burst
    );
typedef uint32 (*mbcm_dpp_ofp_rates_egq_tcg_max_burst_get_f)(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,   
    SOC_SAND_IN  uint32                 tcg_ndx, 
    SOC_SAND_OUT  uint32                *max_burst
    );
typedef uint32 (*mbcm_dpp_ofp_rates_sch_tcg_max_burst_get_f) (
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             tcg_ndx, 
    SOC_SAND_OUT uint32             *max_burst
    );
typedef uint32 (*mbcm_dpp_ofp_rates_egq_port_priority_rate_sw_set_f) (
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                port_priority_ndx, 
    SOC_SAND_IN  uint32                rate
    );
typedef uint32 (*mbcm_dpp_ofp_rates_egq_port_priority_rate_hw_set_f) (
    SOC_SAND_IN  int                unit
    );
typedef uint32 (*mbcm_dpp_ofp_rates_sch_port_priority_rate_set_f) (
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             port_priority, 
    SOC_SAND_IN  uint32             rate
    );

typedef uint32 (*mbcm_dpp_ofp_rates_sch_port_priority_rate_sw_set_f) (
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             port_priority, 
    SOC_SAND_IN  uint32             rate
    );

typedef uint32 (*mbcm_dpp_ofp_rates_sch_port_priority_hw_set_f) (
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core
    );

typedef uint32 (*mbcm_dpp_ofp_rates_egq_port_priority_rate_hw_get_f) (
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,   
    SOC_SAND_IN  uint32                 port_priority, 
    SOC_SAND_OUT uint32                 *rate
    );
typedef uint32 (*mbcm_dpp_ofp_rates_sch_port_priority_rate_get_f) (
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32       fap_port,   
    SOC_SAND_IN  uint32                 port_priority, 
    SOC_SAND_OUT  uint32                *rate
    );
typedef uint32 (*mbcm_dpp_ofp_rates_egq_port_priority_max_burst_set_f) (
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             port_priority, 
    SOC_SAND_IN  uint32             max_burst
    );
typedef uint32 (*mbcm_dpp_ofp_rates_sch_port_priority_max_burst_set_f) (
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             port_priority, 
    SOC_SAND_IN  uint32             max_burst
    );
typedef uint32 (*mbcm_dpp_ofp_rates_egq_port_priority_max_burst_get_f) (
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,   
    SOC_SAND_IN  uint32                 port_priority, 
    SOC_SAND_OUT  uint32                *max_burst
    );
typedef uint32 (*mbcm_dpp_ofp_rates_sch_port_priority_max_burst_get_f) (
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             port_priority, 
    SOC_SAND_OUT uint32             *max_burst
    );

typedef int (*mbcm_dpp_ofp_rates_port2chan_cal_get_f) (
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,   
    SOC_SAND_IN  uint32             tm_port, 
    SOC_SAND_OUT uint32             *calendar
    );

typedef int (*mbcm_dpp_ofp_rates_egress_shaper_cal_write_f) (
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  int                                      core,
    SOC_SAND_IN  SOC_TMC_OFP_RATES_CAL_INFO              *cal_info,
    SOC_SAND_IN  SOC_TMC_OFP_RATES_CAL_SET                cal2set,    
    SOC_SAND_IN  SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_TYPE field_type,
    SOC_SAND_IN  uint32                                   data
    );

typedef int (*mbcm_dpp_ofp_rates_egress_shaper_cal_read_f) (
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  int                                      core,
    SOC_SAND_IN  SOC_TMC_OFP_RATES_CAL_INFO              *cal_info,
    SOC_SAND_IN  SOC_TMC_OFP_RATES_CAL_SET                cal2set,    
    SOC_SAND_IN  SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_TYPE field_type,
    SOC_SAND_OUT uint32                                   *data
    );

typedef int (*mbcm_dpp_ofp_rates_retrieve_egress_shaper_reg_field_names_f) (
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  SOC_TMC_OFP_RATES_CAL_INFO              *cal_info,
    SOC_SAND_IN  SOC_TMC_OFP_RATES_CAL_SET                cal2set,    
    SOC_SAND_IN  SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_TYPE field_type,
    SOC_SAND_OUT soc_reg_t                                *register_name,
    SOC_SAND_OUT soc_field_t                              *field_name
    );

typedef int (*mbcm_dpp_ofp_rates_egress_shaper_reg_field_read_f) (
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  int                                      core,
    SOC_SAND_IN  SOC_TMC_OFP_RATES_CAL_INFO               *cal_info,   
    SOC_SAND_IN  soc_reg_t                                register_name,
    SOC_SAND_IN  soc_field_t                              field_name,
    SOC_SAND_OUT uint32                                   *data
    );

typedef int (*mbcm_dpp_ofp_rates_egress_shaper_reg_field_write_f) (
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  int                                      core,
    SOC_SAND_IN  SOC_TMC_OFP_RATES_CAL_INFO               *cal_info,   
    SOC_SAND_IN  soc_reg_t                                register_name,
    SOC_SAND_IN  soc_field_t                              field_name,
    SOC_SAND_OUT uint32                                   data
    );

typedef soc_mem_t (*mbcm_dpp_ofp_rates_egq_scm_chan_arb_id2scm_id_f) (
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 chan_arb_id
    );


typedef int (*mbcm_dpp_ofp_rates_interface_internal_rate_get_f) (
    SOC_SAND_IN   int                   unit, 
    SOC_SAND_IN   int                   core, 
    SOC_SAND_IN   uint32                egr_if_id, 
    SOC_SAND_OUT  uint32                *internal_rate
    );

typedef int (*mbcm_dpp_ofp_rates_packet_mode_packet_size_get_f) (
    SOC_SAND_IN   int                   unit, 
    SOC_SAND_IN   int                   core,  
    SOC_SAND_OUT  uint32                *num_of_bytes
    );


typedef int (*mbcm_dpp_nof_interrupts_f) (
      int unit, 
      int* nof_interrupts
    );
typedef int (*mbcm_dpp_nof_block_instances_f) (
    int unit, 
    soc_block_types_t block_types, 
    int *nof_block_instances
                          );

typedef int (*mbcm_dpp_temp_pvt_get_f) (
    int unit, 
    int temperature_max,
    soc_switch_temperature_monitor_t *temperature_array,
    int *temperature_count
  );
 
typedef int (*mbcm_dpp_avs_value_get_f) (
    int unit, 
    uint32* avs_val
  );
 

typedef int (*mbcm_dpp_drv_allocate_rcy_port_f)(
    int unit, int core, soc_port_t *port 
  );

typedef int (*mbcm_dpp_drv_free_tm_port_and_recycle_channel_f)(
    int unit, 
    int port
  );

typedef int (*mbcm_dpp_drv_info_config_device_ports_f)(
    int unit
  );

typedef int (*mbcm_dpp_drv_is_olp_f)(
    int unit, 
    soc_port_t port, 
    uint32* is_olp
   );

typedef int (*mbcm_dpp_drv_is_oamp_f)(
    int unit, 
    soc_port_t port, 
    uint32* is_oamp
   );

typedef int (*mbcm_dpp_drv_validate_fabric_mode_f)(
    int unit, 
    SOC_TMC_FABRIC_CONNECT_MODE *fabric_connect_mode
   );

typedef int (*mbcm_dpp_drv_prop_fap_device_mode_get_f)(
    int unit, 
    uint8 *pp_enable, 
    SOC_TMC_MGMT_TDM_MODE *tdm_mode
   );

typedef int (*mbcm_dpp_drv_device_deinit_f)(
    int unit
   );

typedef int (*mbcm_dpp_drv_device_attach_f)(
    int unit
   );

typedef int(*mbcm_dpp_drv_fc_oob_mode_validate_f)(
    int unit, 
    int port
   );

typedef int (*mbcm_dpp_port_to_interface_egress_map_set_f)(
    int                   unit,
    soc_port_t            port
   );

typedef int (*mbcm_dpp_port2egress_offset_f)(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_OUT uint32              *egr_if
  );

typedef int (*mbcm_dpp_egr_is_channelized_f) (
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core, 
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_OUT uint32                *is_channalzied
    );

typedef int (*mbcm_dpp_sch_cal_tbl_set_f) (
    SOC_SAND_IN   int                    unit,
    SOC_SAND_IN   int                    core_id,
    SOC_SAND_IN   uint32                 sch_offset,
    SOC_SAND_IN   uint32                 sch_to_set,
    SOC_SAND_IN   uint32                 slots_count,
                  uint32*                slots
    );

typedef int (*mbcm_dpp_sch_cal_tbl_get_f) (
    SOC_SAND_IN   int                    unit,
    SOC_SAND_IN   int                    core_id,
    SOC_SAND_IN   uint32                 sch_offset,
    SOC_SAND_IN   uint32                 sch_to_set,
    SOC_SAND_IN   uint32                 slots_count,
    SOC_SAND_OUT  uint32*                slots
    );

typedef int (*mbcm_dpp_sch_cal_max_size_get_f) (
    SOC_SAND_IN   int                    unit,
    SOC_SAND_IN   uint32                 sch_offset,
    SOC_SAND_OUT  uint32*                max_cal_size
   );

typedef int (* mbcm_dpp_local_to_tm_port_get_f) (
     int        unit, 
     soc_port_t port,
     uint32*    tm_port,
     int*       core
   );

typedef int (* mbcm_dpp_local_to_pp_port_get_f) (
     int            unit, 
     soc_port_t     port,
     uint32*        pp_port,
     int*           core
   );

typedef int (* mbcm_dpp_tm_to_local_port_get_f) (
     int            unit, 
     int            core,
     uint32         tm_port,
     soc_port_t*    port
   );

typedef int (* mbcm_dpp_pp_to_local_port_get_f) (
     int            unit, 
     int            core,
     uint32         pp_port,
     soc_port_t*    port
   );


typedef int (* mbcm_port_init_f)(
     int unit
   );

typedef int (* mbcm_port_post_init_f)(
     int unit,
     soc_pbmp_t* ports
   );

typedef int (* mbcm_port_deinit_f)(
     int unit
   );

typedef int (* mbcm_port_enable_set_f)(
     int unit,
     soc_port_t port,
     uint32 mac_only, 
     int enable
   );

typedef int (* mbcm_port_enable_get_f)(
     int unit,
     soc_port_t port,
     uint32 mac_only,
     int* enable
   );

typedef int (* mbcm_port_speed_set_f)(
     int unit,
     soc_port_t port,
     int speed
   );

typedef int (* mbcm_port_speed_get_f)(
     int unit,
     soc_port_t port,
     int* speed
   );

typedef int (* mbcm_port_interface_set_f)(
     int unit,
     soc_port_t port,
     soc_port_if_t intf
   );

typedef int (* mbcm_port_interface_get_f)(
     int unit,
     soc_port_t port,
     soc_port_if_t* intf
   );

typedef int (* mbcm_port_link_state_get_f)(
     int unit,
     soc_port_t port,
     int clear_status,
     int *is_link_up,
     int *is_latch_down
   );

typedef int (* mbcm_is_supported_encap_get_f)(
     int unit,
     int mode,
     int *is_supported
   );

typedef int (* mbcm_egr_q_nif_cal_set_f)(
    SOC_SAND_IN int unit
  );

typedef int (* mbcm_dpp_egr_q_fast_port_set_f)(
        SOC_SAND_IN  int unit, 
        SOC_SAND_IN  soc_port_t port, 
        SOC_SAND_IN  int is_ilkn, 
        SOC_SAND_IN  int turn_on
  );

typedef int (* mbcm_dpp_parser_nof_bytes_to_remove_set_f)(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core,
    SOC_SAND_IN uint32 tm_port,
    SOC_SAND_IN uint32 nof_bytes
  );

typedef int (* mbcm_dpp_ps_db_find_free_binding_ps_f)(
    SOC_SAND_IN int unit,
    SOC_SAND_IN soc_port_t port,
    SOC_SAND_IN int nof_priorities,
    SOC_SAND_IN int is_init,
    SOC_SAND_OUT int *base_q_pair
  );

typedef int (* mbcm_dpp_ps_db_release_binding_ps_f)(
    SOC_SAND_IN int unit,
    SOC_SAND_IN soc_port_t port,
    SOC_SAND_IN int base_q_pair
  );

typedef int (* mbcm_dpp_ps_db_alloc_binding_ps_with_id_f)(
    SOC_SAND_IN int unit,
    SOC_SAND_IN soc_port_t port,
    SOC_SAND_IN int nof_priorities,
    SOC_SAND_IN int base_q_pair
  );

typedef int (* mbcm_dpp_egr_prog_editor_profile_set_f)(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  int                        core,
    SOC_SAND_IN  uint32                     q_pair,
    SOC_SAND_IN  uint32                     prog_editor_profile
  );

typedef soc_error_t (* mbcm_fc_pfc_mapping_set_f)(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN uint32                       nif_id,
    SOC_SAND_IN uint32                       src_pfc_ndx,
    SOC_SAND_IN SOC_TMC_FC_PFC_MAP           *pfc_map
  );

typedef soc_error_t (* mbcm_fc_pfc_mapping_get_f)(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN uint32                       nif_id,
    SOC_SAND_IN uint32                       src_pfc_ndx,
    SOC_SAND_OUT SOC_TMC_FC_PFC_MAP          *pfc_map
  );


typedef int (* mbcm_dpp_port_probe_f)(
    int unit, 
    pbmp_t pbmp, 
    pbmp_t *okay_pbmp,
    int is_init_sequence
    ); 

typedef int (* mbcm_dpp_port_detach_f)(
    int unit, 
    soc_port_t port
    ); 
typedef int (* mbcm_dpp_port_fabric_detach_f)(
    int unit, 
    soc_port_t port
    ); 

typedef int (* mbcm_dpp_port_is_pcs_loopback_f)(
    int unit,
    soc_port_t port,
    int *result
    );

typedef int (*mbcm_dpp_qsgmii_offsets_add_f)(
    int unit, 
    uint32 phy, 
    uint32 *new_phy
    );

typedef int (*mbcm_dpp_qsgmii_offsets_remove_f)(
    int unit, 
    uint32 phy, 
    uint32 *new_phy
    );

typedef int (*mbcm_dpp_qsgmii_offsets_add_pbmp_f)(
    int unit, 
    soc_pbmp_t* pbmp, 
    soc_pbmp_t* new_pbmp
    );

typedef int (*mbcm_dpp_qsgmii_offsets_remove_pbmp_f)(
    int unit, 
    soc_pbmp_t* pbmp, 
    soc_pbmp_t* new_pbmp
    );

typedef int (*mbcm_dpp_port_sch_config_f)(
    int unit, 
    soc_port_t port
    );

typedef int (*mbcm_dpp_port_open_fab_o_nif_path_f)(
    int unit, 
    soc_port_t port
    );

typedef int (*mbcm_dpp_port_open_ilkn_path_f)(
    int unit, 
    soc_port_t port
    );

typedef int (*mbcm_dpp_nif_sif_set_f)(
    int unit, 
    uint32 first_phy
    );

typedef int (*mbcm_dpp_port_close_ilkn_path_f)(
    int unit, 
    soc_port_t port
    );

typedef int (*mbcm_dpp_port_ilkn_init_f)(
        int unit
        );

typedef int (*mbcm_dpp_port_nif_ilkn_pbmp_get_f)(
        int unit,
        soc_port_t port,
        uint32 ilkn_id,
        soc_pbmp_t* phys,
        soc_pbmp_t* src_pbmp
        );

typedef int (*mbcm_dpp_port_nif_ilkn_phys_aligned_pbmp_get_f)(
        int unit,
        soc_port_t port,
        soc_pbmp_t* phys_aligned,
        int is_format_adjust
        );

typedef int (*mbcm_dpp_port_nif_qsgmii_pbmp_get_f)(
        int unit,
        soc_port_t port,
        uint32 id,
        soc_pbmp_t* phys
        );

typedef int (*mbcm_dpp_port_ilkn_nif_port_get_f)(
        int unit,
        uint32 ilkn_intf_offset,
        uint32* nif_port
        );

typedef int (*mbcm_dpp_port_ilkn_nof_segments_set_f)(
        int unit,
        soc_port_t port,
        uint32 nof_segments
        );

typedef int (*mbcm_port_ilkn_bypass_interface_enable_f)(
        int unit,
        int port,
        int enable);

typedef int (*mbcm_port_fabric_o_nif_bypass_interface_enable_f)(
        int unit,
        int port,
        int enable);

typedef int (* mbcm_dpp_port_prd_enable_set_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        int enable
);

typedef int (* mbcm_dpp_port_prd_enable_get_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        int *enable
);

typedef int (* mbcm_dpp_port_prd_config_set_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        soc_dpp_port_prd_config_t *config
);

typedef int (* mbcm_dpp_port_prd_config_get_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        soc_dpp_port_prd_config_t *config
);

typedef int (* mbcm_dpp_port_prd_threshold_set_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        int priority,
        uint32 value
);

typedef int (* mbcm_dpp_port_prd_threshold_get_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        int priority,
        uint32 *value
);

typedef int (* mbcm_dpp_port_prd_map_set_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        soc_dpp_prd_map_t map,
        uint32 key,
        int priority
);

typedef int (* mbcm_dpp_port_prd_map_get_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        soc_dpp_prd_map_t map,
        uint32 key,
        int *priority
);

typedef int (* mbcm_dpp_port_prd_drop_count_get_f)(
        int unit,
        soc_port_t port,
        uint64 *count
);

typedef int (* mbcm_dpp_port_prd_tpid_set_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        int index,
        uint32 tpid
);

typedef int (* mbcm_dpp_port_prd_tpid_get_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        int index,
        uint32 *tpid
);

typedef int (* mbcm_dpp_port_speed_sku_restrictions_f)(
        int unit,
        soc_port_t port,
        int speed
);

typedef int (* mbcm_dpp_port_prd_ignore_ip_dscp_set_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        uint32 ip_dscp_ignore
);

typedef int (* mbcm_dpp_port_prd_ignore_ip_dscp_get_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        uint32 *ip_dscp_ignore
);

typedef int (* mbcm_dpp_port_prd_ignore_mpls_exp_set_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        uint32 mpls_exp_ignore
);

typedef int (* mbcm_dpp_port_prd_ignore_mpls_exp_get_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        uint32 *mpls_exp_ignore
);

typedef int (* mbcm_dpp_port_prd_ignore_inner_tag_set_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        uint32 inner_tag_ignore
);

typedef int (* mbcm_dpp_port_prd_ignore_inner_tag_get_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        uint32 *inner_tag_ignore
);

typedef int (* mbcm_dpp_port_prd_ignore_outer_tag_set_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        uint32 outer_tag_ignore
);

typedef int (* mbcm_dpp_port_prd_ignore_outer_tag_get_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        uint32 *outer_tag_ignore
);

typedef int (* mbcm_dpp_port_prd_default_priority_set_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        uint32 default_priority
);

typedef int (* mbcm_dpp_port_prd_default_priority_get_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        uint32 *default_priority
);

typedef int (* mbcm_dpp_port_prd_custom_ether_type_set_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        uint32 ether_type_code,
        uint32 ether_type_val
);

typedef int (* mbcm_dpp_port_prd_custom_ether_type_get_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        uint32 ether_type_code,
        uint32 *ether_type_val
);

typedef int (* mbcm_dpp_port_prd_control_frame_set_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        uint32 control_frame_index,
        soc_jer_nif_prd_control_plane_t *prd_ctrl_plane
);

typedef int (* mbcm_dpp_port_prd_control_frame_get_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        uint32 control_frame_index,
        soc_jer_nif_prd_control_plane_t *prd_ctrl_plane
);

typedef int (* mbcm_dpp_port_prd_flex_key_construct_set_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        uint32 *offset_array,
        uint32 array_size
);

typedef int (* mbcm_dpp_port_prd_flex_key_construct_get_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        uint32 *offset_array,
        uint32 *array_size
);

typedef int (* mbcm_dpp_port_prd_flex_key_entry_set_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        uint32 key_index,
        soc_jer_nif_prd_flex_key_entry_t *key_entry
);

typedef int (* mbcm_dpp_port_prd_flex_key_entry_get_f)(
        int unit,
        soc_port_t port,
        uint32 flags,
        uint32 key_index,
        soc_jer_nif_prd_flex_key_entry_t *key_entry
);

typedef int (* mbcm_dpp_port_prd_restore_hw_defaults_f)(
        int unit,
        soc_port_t port
);
typedef uint32 (* mbcm_dpp_egr_prog_editor_prog_sel_value1_set_f)(
    int                        unit,
    ARAD_EGR_PROG_EDITOR_PROGRAMS program,
    uint32                     value1
  );



typedef soc_error_t (*mbcm_dpp_port_control_pcs_set_f) (int unit, soc_port_t port, soc_dcmn_port_pcs_t pcs);
typedef soc_error_t (*mbcm_dpp_port_control_pcs_get_f) (int unit, soc_port_t port, soc_dcmn_port_pcs_t* pcs);
typedef soc_error_t (*mbcm_dpp_port_control_power_set_f) (int unit, soc_port_t port, uint32 flags, soc_dcmn_port_power_t power);
typedef soc_error_t (*mbcm_dpp_port_control_power_get_f) (int unit, soc_port_t port, soc_dcmn_port_power_t* power);
typedef soc_error_t (*mbcm_dpp_port_control_rx_enable_set_f) (int unit, soc_port_t port, uint32 flags, int enable);
typedef soc_error_t (*mbcm_dpp_port_control_tx_enable_set_f) (int unit, soc_port_t port, int enable);
typedef soc_error_t (*mbcm_dpp_port_control_tx_nif_enable_set_f) (int unit, soc_port_t port, int enable);
typedef soc_error_t (*mbcm_dpp_port_control_rx_enable_get_f) (int unit, soc_port_t port, int* enable);
typedef soc_error_t (*mbcm_dpp_port_control_tx_enable_get_f) (int unit, soc_port_t port, int* enable);
typedef soc_error_t (*mbcm_dpp_port_control_tx_nif_enable_get_f) (int unit, soc_port_t port, int* enable);
typedef soc_error_t (*mbcm_dpp_port_control_strip_crc_set_f) (int unit, soc_port_t port, int strip_crc);
typedef soc_error_t (*mbcm_dpp_port_control_strip_crc_get_f) (int unit, soc_port_t port, int* strip_crc);
typedef soc_error_t (*mbcm_dpp_port_prbs_tx_enable_set_f) (int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int value);
typedef soc_error_t (*mbcm_dpp_port_prbs_tx_enable_get_f) (int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int* value);
typedef soc_error_t (*mbcm_dpp_port_prbs_rx_enable_set_f) (int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int value);
typedef soc_error_t (*mbcm_dpp_port_prbs_rx_enable_get_f) (int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int* value);
typedef soc_error_t (*mbcm_dpp_port_prbs_rx_status_get_f) (int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int* value);
typedef soc_error_t (*mbcm_dpp_port_prbs_polynomial_set_f) (int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int value);
typedef soc_error_t (*mbcm_dpp_port_prbs_polynomial_get_f) (int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int* value);
typedef soc_error_t (*mbcm_dpp_port_prbs_tx_invert_data_set_f) (int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int value);
typedef soc_error_t (*mbcm_dpp_port_prbs_tx_invert_data_get_f) (int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int *value);
typedef soc_error_t (*mbcm_dpp_port_pfc_refresh_set_f) (int unit, soc_port_t port, int value);
typedef soc_error_t (*mbcm_dpp_port_pfc_refresh_get_f) (int unit, soc_port_t port, int *value);
typedef soc_error_t (*mbcm_dpp_local_fault_clear_f) (int unit, soc_port_t port);
typedef soc_error_t (*mbcm_dpp_remote_fault_clear_f) (int unit, soc_port_t port);
typedef soc_error_t (*mbcm_dpp_pad_size_set_f) (int unit, soc_port_t port, int value);
typedef soc_error_t (*mbcm_dpp_pad_size_get_f) (int unit, soc_port_t port, int *value);
typedef soc_error_t (*mbcm_dpp_port_reset_f) (int unit, soc_port_t port);
typedef soc_error_t (*mbcm_dpp_port_phy_control_set_f) (int unit, soc_port_t port, int phyn, int lane, int is_sys_side, soc_phy_control_t type, uint32 value);
typedef soc_error_t (*mbcm_dpp_port_phy_control_get_f) (int unit, soc_port_t port, int phyn, int lane, int is_sys_side, soc_phy_control_t type, uint32 *value);
typedef soc_error_t (*mbcm_dpp_port_phy_get_f) (int unit, soc_port_t port, uint32 flags, uint32 phy_reg_addr, uint32 *phy_data);
typedef soc_error_t (*mbcm_dpp_port_phy_set_f) (int unit, soc_port_t port, uint32 flags, uint32 phy_reg_addr, uint32 phy_data);
typedef soc_error_t (*mbcm_dpp_port_phy_modify_f) (int unit, soc_port_t port, uint32 flags, uint32 phy_reg_addr, uint32 phy_data, uint32 phy_mask);
typedef soc_error_t (*mbcm_dpp_port_mac_sa_set_f) (int unit, int port, sal_mac_addr_t mac_sa);
typedef soc_error_t (*mbcm_dpp_port_mac_sa_get_f) (int unit, int port, sal_mac_addr_t mac_sa);
typedef soc_error_t (*mbcm_dpp_port_eee_enable_get_f)(int unit, soc_port_t port, uint32 *value);
typedef soc_error_t (*mbcm_dpp_port_eee_enable_set_f)(int unit, soc_port_t port, uint32 value);
typedef soc_error_t (*mbcm_dpp_port_eee_tx_idle_time_get_f)(int unit, soc_port_t port, uint32 *value);
typedef soc_error_t (*mbcm_dpp_port_eee_tx_idle_time_set_f)(int unit, soc_port_t port, uint32 value);
typedef soc_error_t (*mbcm_dpp_port_eee_tx_wake_time_get_f)(int unit, soc_port_t port, uint32 *value);
typedef soc_error_t (*mbcm_dpp_port_eee_tx_wake_time_set_f)(int unit, soc_port_t port, uint32 value);
typedef soc_error_t (*mbcm_dpp_port_eee_link_active_duration_get_f)(int unit, soc_port_t port, uint32 *value);
typedef soc_error_t (*mbcm_dpp_port_eee_link_active_duration_set_f)(int unit, soc_port_t port, uint32 value);

typedef soc_error_t (*mbcm_dpp_port_eee_statistics_clear_f)(int unit, soc_port_t port);
typedef soc_error_t (*mbcm_dpp_port_eee_event_count_symmetric_set_f)(int unit, soc_port_t port, uint32 value);
typedef soc_error_t (*mbcm_dpp_port_eee_tx_event_count_get_f)(int unit, soc_port_t port, uint32 *value);
typedef soc_error_t (*mbcm_dpp_port_eee_tx_duration_get_f)(int unit, soc_port_t port, uint32 *value);
typedef soc_error_t (*mbcm_dpp_port_eee_rx_event_count_get_f)(int unit, soc_port_t port, uint32 *value);
typedef soc_error_t (*mbcm_dpp_port_eee_rx_duration_get_f)(int unit, soc_port_t port, uint32 *value);
typedef soc_error_t (*mbcm_dpp_port_eee_event_count_symmetric_get_f)(int unit, soc_port_t port, uint32 *value);


typedef soc_error_t (* mbcm_dpp_fc_enables_set_f)(
    SOC_SAND_IN int                           unit,
    SOC_SAND_IN SOC_TMC_FC_ENABLE_BITMAP      *cfc_enables,
    SOC_SAND_IN SOC_TMC_FC_ENABLE_BITMAP      *ena_info
    );

typedef soc_error_t (* mbcm_dpp_fc_enables_get_f)(
    SOC_SAND_IN int                           unit,
    SOC_SAND_IN SOC_TMC_FC_ENABLE_BITMAP      *cfc_enables,
    SOC_SAND_OUT SOC_TMC_FC_ENABLE_BITMAP     *ena_info
    );

typedef soc_error_t (* mbcm_dpp_fc_ilkn_mub_channel_set_f)(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID           ilkn_ndx,
    SOC_SAND_IN  SOC_TMC_FC_DIRECTION           direction_ndx,
    SOC_SAND_OUT uint8                          bitmap
    );

typedef soc_error_t (* mbcm_dpp_fc_ilkn_mub_channel_get_f)(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID           ilkn_ndx,
    SOC_SAND_IN  SOC_TMC_FC_DIRECTION           direction_ndx,
    SOC_SAND_OUT uint8                          *bitmap
    );

typedef soc_error_t (* mbcm_dpp_fc_ilkn_mub_gen_cal_set_f)(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID             ilkn_ndx,
    SOC_SAND_IN  SOC_TMC_FC_ILKN_MUB_GEN_CAL      *cal_info
    );

typedef soc_error_t (* mbcm_dpp_fc_ilkn_mub_gen_cal_get_f)(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID             ilkn_ndx,
    SOC_SAND_OUT SOC_TMC_FC_ILKN_MUB_GEN_CAL      *cal_info
    );


typedef soc_error_t (* mbcm_dpp_fc_cat_2_tc_hcfc_bitmap_set_f)(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   int                           core,
    SOC_SAND_IN   int                           tc,
    SOC_SAND_IN   int                           is_ocb_only,
    SOC_SAND_OUT  SOC_TMC_FC_HCFC_BITMAP        *hcfc_bitmap
    );

typedef soc_error_t (* mbcm_dpp_fc_cat_2_tc_hcfc_bitmap_get_f)(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   int                           core,
    SOC_SAND_IN   int                           tc,
    SOC_SAND_IN   int                           is_ocb_only,
    SOC_SAND_OUT  SOC_TMC_FC_HCFC_BITMAP        *hcfc_bitmap
    );

typedef soc_error_t (* mbcm_dpp_fc_glb_hcfc_bitmap_set_f)(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   int                           core,
    SOC_SAND_IN   int                           is_high_prio,
    SOC_SAND_IN   int                           is_ocb_only,
    SOC_SAND_IN   int                           pool_id,
    SOC_SAND_OUT  SOC_TMC_FC_HCFC_BITMAP        *hcfc_bitmap
    );

typedef soc_error_t (* mbcm_dpp_fc_glb_hcfc_bitmap_get_f)(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   int                           core,
    SOC_SAND_IN   int                           is_high_prio,
    SOC_SAND_IN   int                           is_ocb_only,
    SOC_SAND_IN   int                           pool_id,
    SOC_SAND_OUT  SOC_TMC_FC_HCFC_BITMAP        *hcfc_bitmap
    );

typedef soc_error_t (* mbcm_dpp_fc_inbnd_mode_set_f)(
    SOC_SAND_IN int                           unit,
    SOC_SAND_IN soc_port_t                    port,
    SOC_SAND_IN int                           is_generate,
    SOC_SAND_IN SOC_TMC_FC_INBND_MODE         mode
    );

typedef soc_error_t (* mbcm_dpp_fc_inbnd_mode_get_f)(
    SOC_SAND_IN int                           unit,
    SOC_SAND_IN soc_port_t                    port,
    SOC_SAND_IN int                           is_generate,
    SOC_SAND_OUT SOC_TMC_FC_INBND_MODE        *mode
    );

typedef soc_error_t (* mbcm_dpp_fc_glb_rcs_mask_set_f)(
    SOC_SAND_IN int                           unit,
    SOC_SAND_IN int                           core,
    SOC_SAND_IN int                           is_high_prio,
    SOC_SAND_IN SOC_TMC_FC_GLB_RES_TYPE       glb_res_dst,
    SOC_SAND_IN uint32                        glb_res_src_bitmap
    );

typedef soc_error_t (* mbcm_dpp_fc_glb_rcs_mask_get_f)(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN int                           core,
      SOC_SAND_IN int                           is_high_prio,
      SOC_SAND_IN SOC_TMC_FC_GLB_RES_TYPE       glb_res_dst,
      SOC_SAND_OUT uint32                       *glb_res_src_bitmap
    );

typedef soc_error_t (* mbcm_dpp_fc_init_pfc_mapping_f)(
      SOC_SAND_IN int  unit
    );

typedef soc_error_t (* mbcm_dpp_fc_ilkn_llfc_set_f)(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID          ilkn_ndx,
    SOC_SAND_IN  ARAD_FC_DIRECTION             direction_ndx,
    SOC_SAND_IN  ARAD_FC_ILKN_LLFC_INFO        *info
  );

typedef soc_error_t (* mbcm_dpp_fc_ilkn_llfc_get_f) (
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID             ilkn_ndx,
    SOC_SAND_OUT ARAD_FC_ILKN_LLFC_INFO           *rec_info,
    SOC_SAND_OUT ARAD_FC_ILKN_LLFC_INFO           *gen_info
  );

typedef soc_error_t (* mbcm_dpp_port_ipg_set_f) (
    SOC_SAND_IN int         unit,
    SOC_SAND_IN soc_port_t  port,
    SOC_SAND_IN int         ipg
  );

typedef soc_error_t (* mbcm_dpp_port_ipg_get_f) (
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  soc_port_t  port,
    SOC_SAND_OUT int         *ipg
  );

typedef soc_error_t (* mbcm_dpp_port_link_get_f) (
    int unit, 
    int port, 
    int* link
  );

typedef soc_error_t (* mbcm_dpp_port_ilkn_bypass_interface_rx_check_and_enable_f) (
    int unit,
    int port
  );

typedef soc_error_t (* mbcm_dpp_port_ilkn_bypass_interface_rx_enable_get_f) (
    int unit, 
    int port, 
    uint32* rx_enable
  );

typedef soc_error_t (*mbcm_dpp_port_autoneg_set_f) (
    int                      unit,
    soc_port_t               port,
    int                      autoneg_en
  );

typedef soc_error_t (*mbcm_dpp_port_autoneg_get_f) (
    int                      unit,
    soc_port_t               port,
    int                      *autoneg_en
  );

typedef soc_error_t (*mbcm_dpp_port_ability_local_get_f) (
    int                      unit,
    soc_port_t               port,
    soc_port_ability_t       *ability_mask
  );

typedef soc_error_t (*mbcm_dpp_port_ability_remote_get_f) (
    int                      unit,
    soc_port_t               port,
    soc_port_ability_t       *ability_mask
  );

typedef soc_error_t (*mbcm_dpp_port_ability_advert_set_f) (
    int                      unit,
    soc_port_t               port,
    soc_port_ability_t       *ability_mask
  );

typedef soc_error_t (*mbcm_dpp_port_ability_advert_get_f) (
    int                      unit,
    soc_port_t               port,
    soc_port_ability_t       *ability_mask
  );

typedef soc_error_t (*mbcm_dpp_port_mdix_set_f) (
    int                      unit,
    soc_port_t               port,
    soc_port_mdix_t          mode
  );

typedef soc_error_t (*mbcm_dpp_port_mdix_get_f) (
    int                      unit,
    soc_port_t               port,
    soc_port_mdix_t          *mode
  );

typedef soc_error_t (*mbcm_dpp_port_mdix_status_get_f) (
    int                      unit,
    soc_port_t               port,
    soc_port_mdix_status_t   *status
  );

typedef soc_error_t (*mbcm_dpp_port_duplex_set_f) (
    int                      unit,
    soc_port_t               port,
    int                      duplex
  );

typedef soc_error_t (*mbcm_dpp_port_duplex_get_f) (
    int                      unit,
    soc_port_t               port,
    int                      *duplex
  );

typedef soc_error_t (*mbcm_dpp_port_nif_nof_lanes_get_f) (
    int                 unit, 
    soc_port_if_t       interface, 
    uint32              first_phy_port, 
    uint32              nof_lanes_to_set, 
    uint32              *nof_lanes
  );

typedef soc_error_t (*mbcm_dpp_port_nif_quad_to_core_validate_f) (
    int                 unit
  );

typedef soc_error_t (*mbcm_dpp_port_close_path_f)(
    int                 unit,
    soc_port_t          port
  );

typedef soc_error_t (* mbcm_dpp_fc_hcfc_watchdog_set_f) (
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   SOC_TMC_FC_CAL_IF_ID          if_ndx,
    SOC_SAND_IN   SOC_TMC_FC_HCFC_WATCHDOG      *hcfc_watchdog
  );

typedef soc_error_t (* mbcm_dpp_fc_hcfc_watchdog_get_f) (
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   SOC_TMC_FC_CAL_IF_ID          if_ndx,
    SOC_SAND_OUT  SOC_TMC_FC_HCFC_WATCHDOG      *hcfc_watchdog
  );

typedef soc_error_t (*mbcm_dpp_port_fault_get_f) (
    int                      unit,
    soc_port_t               port,
    uint32                   *flags
  );

typedef soc_error_t (*mbcm_dpp_fabric_link_fault_get_f) (
    int                      unit,
    soc_port_t               link,
    uint32                   *flags
  );

typedef soc_error_t (*mbcm_dpp_stat_if_queue_range_set_f) (
    int                      unit,
    int                      sif_port,
    int                      is_scrubber,
    int                      min_queue,
    int                      max_queue
  );

typedef soc_error_t (*mbcm_dpp_stat_if_queue_range_get_f) (
    int                      unit,
    int                      sif_port,
    int                      is_scrubber,
    int*                     min_queue,
    int*                     max_queue
  );

typedef soc_error_t (*mbcm_dpp_fc_vsq_index_group2global_f) (
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP vsq_group,
    SOC_SAND_IN  uint32                vsq_in_group_ndx,
    SOC_SAND_IN  int                   cosq,
    SOC_SAND_IN  uint8                 is_ocb_only,
    SOC_SAND_IN  uint32                src_port,
    SOC_SAND_OUT uint32                *vsq_fc_ndx
  );

typedef soc_error_t (*mbcm_dpp_stat_oversize_set_f) (
    int                      unit,
    int                      port,
    int                      val
  );

typedef soc_error_t (*mbcm_dpp_stat_oversize_get_f) (
    int                      unit,
    int                      port,
    int                      *val
  );

typedef soc_error_t (*mbcm_dpp_fc_status_info_get_f) (
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN SOC_TMC_FC_STATUS_KEY   *fc_status_key,
    SOC_SAND_OUT SOC_TMC_FC_STATUS_INFO *fc_status_info
  );

typedef soc_error_t (*mbcm_dpp_stat_counter_filter_set_f) (
    SOC_SAND_IN     int                             unit, 
    SOC_SAND_IN     SOC_TMC_CNT_SOURCE*             source,
    SOC_SAND_IN     SOC_TMC_CNT_FILTER_TYPE*        filter_array, 
    SOC_SAND_IN     int                             filter_count,
    SOC_SAND_IN     int                             is_active
  );

typedef soc_error_t (*mbcm_dpp_stat_counter_filter_get_f) (
    SOC_SAND_IN     int                             unit, 
    SOC_SAND_IN     SOC_TMC_CNT_SOURCE*             source,
    SOC_SAND_IN     int                             filter_max_count,
    SOC_SAND_OUT    SOC_TMC_CNT_FILTER_TYPE*        filter_array, 
    SOC_SAND_OUT    int*                            filter_count
  );

typedef soc_error_t (*mbcm_dpp_cnt_ingress_compensation_profile_delta_set_f) (
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  int                   src_type,
    SOC_SAND_IN  int                profileIndex,
    SOC_SAND_IN  int                   delta
  );

typedef soc_error_t (*mbcm_dpp_cnt_ingress_compensation_port_profile_set_f) (
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  int                   commandId,
    SOC_SAND_IN  int                   src_type,
    SOC_SAND_IN  int                   port,
    SOC_SAND_IN  int                   profileIndex
  );

typedef soc_error_t (*mbcm_dpp_cnt_ingress_compensation_outLif_delta_set_f) (
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  int                   commandId,
    SOC_SAND_IN  int                   src_type,
    SOC_SAND_IN  int                   lifIndex,
    SOC_SAND_IN  int                   delta
  );

typedef soc_error_t (*mbcm_dpp_cnt_ingress_compensation_outLif_delta_get_f) (
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  int                   commandId,
    SOC_SAND_IN  int                   src_type,
    SOC_SAND_IN  int                   lifIndex,
    SOC_SAND_OUT int*                  delta
  );

typedef soc_error_t (*mbcm_dpp_cnt_ingress_compensation_port_delta_and_profile_get_f) (
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  int                   commandId,
    SOC_SAND_IN  int                   src_type,
    SOC_SAND_IN  int                   port,
    SOC_SAND_OUT int*                  profileIndex,
    SOC_SAND_OUT int*                  delta
  );

typedef soc_error_t (*mbcm_dpp_stat_egress_receive_tm_pointer_format_set_f) (
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN uint32                 queue_pair_mask,
    SOC_SAND_IN uint32                 traffic_class_mask,
    SOC_SAND_IN uint32                 cast_mask,
    SOC_SAND_OUT uint32*               queue_pair_shift, 
    SOC_SAND_OUT uint32*               traffic_class_shift, 
    SOC_SAND_OUT uint32*               cast_shift
  );

typedef soc_error_t (*mbcm_dpp_stat_egress_receive_tm_pointer_format_get_f) (
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32*               queue_pair_mask,
    SOC_SAND_OUT uint32*               traffic_class_mask,
    SOC_SAND_OUT uint32*               cast_mask
  );
typedef soc_error_t (*mbcm_dpp_stat_counter_erpp_offset_set_f) (
    SOC_SAND_IN int                         unit, 
    SOC_SAND_IN SOC_TMC_CNT_SRC_TYPE        src_type,
    SOC_SAND_IN SOC_TMC_CNT_MODE_EG_TYPE    eg_type,
    SOC_SAND_IN int                         counter_profile_offset_value
  );

typedef soc_error_t (*mbcm_dpp_stat_counter_erpp_offset_get_f) (
    SOC_SAND_IN int                         unit, 
    SOC_SAND_IN SOC_TMC_CNT_SRC_TYPE        src_type,
    SOC_SAND_IN SOC_TMC_CNT_MODE_EG_TYPE    eg_type,
    SOC_SAND_IN int*                        counter_profile_offset_value
  );

typedef int (* mbcm_dpp_cnt_filter_config_ingress_set_get_f)(int unit, int relevant_bit, int command_id, int get, int* value);
typedef int (* mbcm_dpp_cnt_filter_config_egress_receive_set_get_f)(int unit, int relevant_bit, int command_id, int get, int* value);
typedef uint32 (* mbcm_dpp_cnt_crps_iqm_cmd_get_f)(int unit, int proc_id, int command_id, SOC_SAND_OUT ARAD_CNT_CRPS_IQM_CMD *crps_iqm_cmd);
typedef uint32 (* mbcm_dpp_cnt_counter_bmap_mem_by_src_type_get_f)(    
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                proc_id,
    SOC_SAND_IN  int                   src_core,
    SOC_SAND_IN  ARAD_CNT_SRC_TYPE     src_type,
    SOC_SAND_IN  int                   command_id,
    SOC_SAND_OUT soc_reg_t             *reg, 
    SOC_SAND_OUT soc_field_t           *fields
  );
typedef uint32 (* mbcm_dpp_cnt_do_not_count_field_by_src_type_get_f)(    
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 proc_id,
    SOC_SAND_IN  int                    src_core,
    SOC_SAND_IN  ARAD_CNT_SRC_TYPE      src_type,
    SOC_SAND_IN  int                    command_id,
    SOC_SAND_OUT soc_field_t            *field
  );

typedef int (* mbcm_dpp_cnt_stif_ingress_pp_source_set_f)(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int command_id, 
    SOC_SAND_IN SOC_TMC_CNT_MODE_EG_TYPE eg_type, 
    SOC_SAND_IN int counter_base
);

typedef soc_error_t (*mbcm_dpp_fc_pfc_generic_bitmap_valid_update_f) (
    SOC_SAND_IN   int                               unit,
    SOC_SAND_IN   SOC_TMC_FC_PFC_GEN_BMP_SRC_TYPE   src_type,
    SOC_SAND_IN   int                               priority,
    SOC_SAND_OUT  uint32                           *is_valid
  );

typedef soc_error_t (*mbcm_dpp_fc_pfc_generic_bitmap_used_update_f) (
    SOC_SAND_IN   int                               unit,
    SOC_SAND_IN   SOC_TMC_FC_PFC_GEN_BMP_SRC_TYPE   src_type,
    SOC_SAND_IN   int                               priority,
    SOC_SAND_IN   int                               pfc_bitmap_index,
    SOC_SAND_OUT  uint32                            is_set
  );

typedef int (*mbcm_dpp_interrupts_array_init_f) (
    SOC_SAND_IN     int                             unit
  );

typedef void (*mbcm_dpp_interrupts_array_deinit_f) (
    SOC_SAND_IN     int                             unit
  );


typedef void (*mbcm_dpp_jer_interrupt_cb_init_f) (
    SOC_SAND_IN     int                             unit
  );

#ifdef PORTMOD_SUPPORT
typedef int (*mbcm_dpp_soc_pm_instances_get_f) (
    int                             unit,
    portmod_pm_instances_t**        pm_instances,
    int*                            pms_instances_arr_len
  );
#endif

typedef int (*mbcm_dpp_soc_pml_table_get_f) (
    int                             unit,
    soc_dpp_pm_entry_t**            soc_pml_table
  );

typedef soc_error_t (*mbcm_dpp_port_quad_get_f) (
    SOC_SAND_IN     int                             unit,
    SOC_SAND_IN     soc_port_t                      port,
    SOC_SAND_OUT    soc_pbmp_t*                     quad_bmp,
    SOC_SAND_OUT    int *                           is_ilkn_over_fabric
  );

typedef soc_error_t (*mbcm_dpp_port_ports_to_same_quad_get_f) (
    SOC_SAND_IN     int                             unit,
    SOC_SAND_IN     soc_port_t                      port,
    SOC_SAND_OUT    soc_pbmp_t*                     ports
  );

typedef soc_error_t (*mbcm_dpp_nif_priority_set_f) (
    SOC_SAND_IN     int                             unit,
    SOC_SAND_IN     int                             core,
    SOC_SAND_IN     uint32                          quad_ilkn,
    SOC_SAND_IN     uint32                          is_ilkn,
    SOC_SAND_IN     uint32                          flags,
    SOC_SAND_IN     uint32                          allow_tdm,
    SOC_SAND_IN     int                             priority_level
  );

typedef soc_error_t (*mbcm_dpp_nif_priority_get_f) (
    SOC_SAND_IN     int                             unit,
    SOC_SAND_IN     int                             core,
    SOC_SAND_IN     uint32                          quad_ilkn,
    SOC_SAND_IN     uint32                          is_ilkn,
    SOC_SAND_IN     uint32                          flags,
    SOC_SAND_IN     uint32                          allow_tdm,
    SOC_SAND_OUT    int*                            priority_level
  );

typedef int (*mbcm_dpp_nif_priority_quad_tdm_high_low_clear_f) (
    SOC_SAND_INOUT     int                             unit,
    SOC_SAND_INOUT     uint32                          quad,
    SOC_SAND_INOUT     int                             clear_tdm,
    SOC_SAND_INOUT     int                             clear_high_low
  );

typedef int (*mbcm_dpp_nif_priority_ilkn_tdm_clear_f) (
    SOC_SAND_INOUT     int                             unit,
    SOC_SAND_INOUT     uint32                          ilkn_id
  );

typedef int (*mbcm_dpp_nif_priority_ilkn_high_low_clear_f) (
    SOC_SAND_INOUT     int                             unit,
    SOC_SAND_INOUT     uint32                          ilkn_id
  );

typedef int (*mbcm_dpp_nif_sku_restrictions_f) (
    SOC_SAND_IN     int                             unit,
    SOC_SAND_IN     soc_pbmp_t                      phy_pbmp,
    SOC_SAND_IN     soc_port_if_t                   interface,
    SOC_SAND_IN     uint32                          interface_id,
    SOC_SAND_IN     uint32                          is_kbp
  );

typedef soc_error_t (*mbcm_dpp_fabric_link_config_ovrd_f) (
    int                             unit
  );

typedef int (*mbcm_dpp_wait_gtimer_trigger_f) (
    int                             unit
  );

typedef int (*mbcm_dpp_phy_nif_measure_f) (
    int unit,
    soc_port_t port,
    uint32 *type_of_bit_clk,
    int *one_clk_time_measured_int,
    int *one_clk_time_measured_remainder,
    int *serdes_freq_int,
    int *serdes_freq_remainder,
    uint32 *lane
    );

typedef int (*mbcm_dpp_soc_bist_all_f)(
    const int unit, 
    const int skip_errors   
    );

typedef soc_error_t (*mbcm_dpp_device_reset_f) (
      SOC_SAND_IN int   unit,
      SOC_SAND_IN int   mode,
      SOC_SAND_IN int   action
      );

typedef soc_error_t (*mbcm_dpp_fc_cmic_rx_set_f) (
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN soc_port_t                    port,
      SOC_SAND_IN uint32                        priority_bmp,
      SOC_SAND_IN int                           is_ena
  );

typedef soc_error_t (*mbcm_dpp_fc_cmic_rx_get_f) (
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN soc_port_t                    port,
      SOC_SAND_OUT uint32                       *priority_bmp,
      SOC_SAND_OUT int                          *is_ena
  );




typedef soc_error_t (*mbcm_dpp_mult_get_entry_f) (
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN  uint32                       mcdb_index,
      SOC_SAND_OUT uint32                       *entry
  );

typedef int (*mbcm_dpp_port_ilkn_over_fabric_set_f)(
    const int  unit, 
    soc_port_t port, 
    uint32     ilkn_id
  );

typedef soc_error_t (*mbcm_dpp_lb_ing_total_shared_buffer_config_set_f) (
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 nof_buffers
  );

typedef soc_error_t (*mbcm_dpp_lb_ing_total_shared_buffer_config_get_f) (
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT int                                *nof_buffers
  );

typedef soc_error_t (*mbcm_dpp_lb_ing_guaranteed_buffer_config_set_f) (
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  soc_lbg_t                           lbg_id,
    SOC_SAND_IN  int                                 nof_buffers
  );

typedef soc_error_t (*mbcm_dpp_lb_ing_guaranteed_buffer_config_get_f) (
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  soc_lbg_t                           lbg_id,
    SOC_SAND_OUT int                                *nof_buffers
  );

typedef soc_error_t (*mbcm_dpp_lb_ing_modem_to_lbg_map_set_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_lbg_t                    lbg_id
  );

typedef soc_error_t (*mbcm_dpp_lb_ing_modem_to_lbg_unmap_set_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_lbg_t                    lbg_id
  );

typedef soc_error_t (*mbcm_dpp_lb_ing_modem_to_lbg_map_get_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_OUT soc_lbg_t                   *lbg_id
  );

typedef soc_error_t (*mbcm_dpp_lb_ing_expected_seqeunce_num_set_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  int                          expected_seq_num
  );

typedef soc_error_t (*mbcm_dpp_lb_ing_expected_seqeunce_num_get_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_OUT int                         *expected_seq_num
  );

typedef soc_error_t (*mbcm_dpp_lb_ing_glb_seqeunce_num_width_set_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          num_width
  );

typedef soc_error_t (*mbcm_dpp_lb_ing_glb_seqeunce_num_width_get_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_OUT int                         *num_width
  );

typedef soc_error_t (*mbcm_dpp_lb_ing_packet_to_modem_map_set_f) (
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  soc_lb_rx_modem_map_index_t      *map_index,
    SOC_SAND_IN  soc_lb_rx_modem_map_config_t     *map_config
  );

typedef soc_error_t (*mbcm_dpp_lb_ing_packet_to_modem_map_get_f) (
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  soc_lb_rx_modem_map_index_t         *map_index,
    SOC_SAND_INOUT  soc_lb_rx_modem_map_config_t     *map_config
  );

typedef soc_error_t (*mbcm_dpp_lb_ing_reorder_config_set_f) (
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  soc_lbg_t                        lbg_id,
    SOC_SAND_IN  soc_lb_ing_reorder_config_t     *reorder_config
  );

typedef soc_error_t (*mbcm_dpp_lb_ing_reorder_config_get_f) (
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  soc_lbg_t                        lbg_id,
    SOC_SAND_OUT soc_lb_ing_reorder_config_t     *reorder_config
  );

typedef soc_error_t (*mbcm_dpp_lb_ing_flush_config_get_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  soc_lb_flush_type_t          type,
    SOC_SAND_OUT int                         *enable
  );

typedef soc_error_t (*mbcm_dpp_lb_ing_flush_config_set_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  soc_lb_flush_type_t          type,
    SOC_SAND_IN  int                          enable
  );

typedef soc_error_t (*mbcm_dpp_lb_ing_status_get_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  soc_lb_status_type_t         status_type,
    SOC_SAND_OUT int                         *status
  );

typedef soc_error_t (*mbcm_dpp_lb_ing_modem_status_get_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_lb_modem_status_type_t   status_type,
    SOC_SAND_OUT int                         *status
  );

typedef soc_error_t (*mbcm_dpp_lb_ing_glb_stat_get_f) (
    SOC_SAND_IN  int                          unit, 
    SOC_SAND_IN  soc_lb_stat_val_t            type, 
    SOC_SAND_OUT uint64                      *value
  );

typedef soc_error_t (*mbcm_dpp_lb_ing_lbg_stat_get_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  soc_lb_stat_val_t            type, 
    SOC_SAND_OUT uint64                      *value
  );

typedef soc_error_t (*mbcm_dpp_lb_ing_modem_stat_get_f) (
    SOC_SAND_IN  int                          unit, 
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_lb_modem_stat_val_t      type, 
    SOC_SAND_OUT uint64                      *value
  );

typedef soc_error_t (*mbcm_dpp_lb_egr_modem_to_port_map_set_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_port_t                   port
  );

typedef soc_error_t (*mbcm_dpp_lb_egr_modem_to_port_unmap_set_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_port_t                   port
  );

typedef soc_error_t (*mbcm_dpp_lb_egr_modem_to_port_map_get_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_OUT  soc_port_t                 *port
  );

typedef soc_error_t (*mbcm_dpp_lb_egr_lbg_enable_set_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN soc_lbg_t                    lbg_id,
    SOC_SAND_IN  uint32                       is_enable
  );

typedef soc_error_t (*mbcm_dpp_lb_egr_lbg_enable_get_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_OUT uint32                       *is_enable
  );

typedef soc_error_t (*mbcm_dpp_lb_egr_modem_to_lbg_map_set_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_lbg_t                    lbg_id
  );

typedef soc_error_t (*mbcm_dpp_lb_egr_modem_to_lbg_unmap_set_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_lbg_t                    lbg_id
  );

typedef soc_error_t (*mbcm_dpp_lb_egr_modem_to_lbg_map_get_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_OUT soc_lbg_t                   *lbg_id
  );

typedef soc_error_t (*mbcm_dpp_lb_egr_expected_seqeunce_num_set_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  int                          expected_seq_num
  );

typedef soc_error_t (*mbcm_dpp_lb_egr_expected_seqeunce_num_get_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_OUT int                         *expected_seq_num
  );

typedef soc_error_t (*mbcm_dpp_lb_egr_seqeunce_num_width_set_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  int                          num_width
  );

typedef soc_error_t (*mbcm_dpp_lb_egr_seqeunce_num_width_get_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_OUT int                         *num_width
  );

typedef soc_error_t (*mbcm_dpp_lb_egr_modem_seqeunce_num_width_set_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  int                          num_width
  );

typedef soc_error_t (*mbcm_dpp_lb_egr_modem_seqeunce_num_width_get_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_OUT int                         *num_width
  );

typedef soc_error_t (*mbcm_dpp_lb_global_packet_config_set_f) (
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  soc_lb_global_packet_config_t      *config
  );

typedef soc_error_t (*mbcm_dpp_lb_global_packet_config_get_f) (
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT soc_lb_global_packet_config_t      *config
  );

typedef soc_error_t (*mbcm_dpp_lb_lbg_packet_config_set_f) (
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  soc_lbg_t                           lbg_id,
    SOC_SAND_IN  soc_lb_lbg_packet_config_t         *config
  );

typedef soc_error_t (*mbcm_dpp_lb_lbg_packet_config_get_f) (
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  soc_lbg_t                           lbg_id,
    SOC_SAND_OUT soc_lb_lbg_packet_config_t         *config
  );

typedef soc_error_t (*mbcm_dpp_lb_modem_packet_config_set_f) (
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  soc_modem_t                        modem_id,
    SOC_SAND_IN  soc_lb_modem_packet_config_t      *config
  );

typedef soc_error_t (*mbcm_dpp_lb_modem_packet_config_get_f) (
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  soc_modem_t                        modem_id,
    SOC_SAND_OUT soc_lb_modem_packet_config_t      *config
  );

typedef soc_error_t (*mbcm_dpp_lb_tc_dp_to_packet_priority_config_set_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lb_tc_dp_t              *tc_dp,
    SOC_SAND_IN  soc_lb_pkt_pri_t            *pkt_pri
  );

typedef soc_error_t (*mbcm_dpp_lb_tc_dp_to_packet_priority_config_get_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lb_tc_dp_t              *tc_dp,
    SOC_SAND_OUT soc_lb_pkt_pri_t            *pkt_pri
  );

typedef soc_error_t (*mbcm_dpp_lb_egr_sched_set_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                       lbg_count,
    SOC_SAND_IN  soc_lb_lbg_weight_t         *lbg_weights
  );

typedef soc_error_t (*mbcm_dpp_lb_egr_sched_get_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                       max_lbg_count,
    SOC_SAND_OUT soc_lb_lbg_weight_t         *lbg_weights,
    SOC_SAND_OUT uint32                      *lbg_count    
  );

typedef soc_error_t (*mbcm_dpp_lb_egr_segment_config_set_f) (
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  soc_lbg_t                        lbg_id,
    SOC_SAND_IN  soc_lb_segment_mode_t            segment_mode
  );

typedef soc_error_t (*mbcm_dpp_lb_egr_segment_config_get_f) (
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  soc_lbg_t                        lbg_id,
    SOC_SAND_OUT soc_lb_segment_mode_t            *segment_mode
  );


typedef soc_error_t (*mbcm_dpp_lb_egr_modem_shaper_set_f) (
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  soc_modem_t                      modem_id,
    SOC_SAND_IN  soc_lb_modem_shaper_config_t    *shaper_config
  );

typedef soc_error_t (*mbcm_dpp_lb_egr_modem_shaper_get_f) (
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  soc_modem_t                      modem_id,
    SOC_SAND_OUT soc_lb_modem_shaper_config_t     *shaper_config
  );

typedef soc_error_t (*mbcm_dpp_lb_egr_modem_segment_shaper_set_f) (
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  soc_modem_t                               modem_id,
    SOC_SAND_IN  soc_lb_modem_segment_shaper_config_t     *shaper_config
  );

typedef soc_error_t (*mbcm_dpp_lb_egr_modem_segment_shaper_get_f) (
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  soc_modem_t                               modem_id,
    SOC_SAND_OUT soc_lb_modem_segment_shaper_config_t     *shaper_config
  );

typedef soc_error_t (*mbcm_dpp_lb_egr_status_get_f) (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  soc_lb_status_type_t         status_type,
    SOC_SAND_OUT int                         *status
  );

typedef soc_error_t (*mbcm_dpp_lb_egr_stat_get_f) (
    SOC_SAND_IN  int                          unit, 
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  soc_lb_stat_val_t            type, 
    SOC_SAND_OUT uint64                      *value
  );

typedef soc_error_t (*mbcm_dpp_lb_egr_modem_stat_get_f) (
    SOC_SAND_IN  int                          unit, 
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_lb_modem_stat_val_t      type, 
    SOC_SAND_OUT uint64                      *value
  );

typedef soc_error_t (*mbcm_dpp_lb_ports_on_reserve_intf_get_f) (
    SOC_SAND_IN  int         unit, 
    SOC_SAND_OUT soc_pbmp_t *ports,
    SOC_SAND_OUT uint32     *port_num
  );

typedef soc_error_t (*mbcm_dpp_lb_port_speed_validate_f) (
    SOC_SAND_IN  int         unit, 
    SOC_SAND_IN  int         is_check_by_port, 
    SOC_SAND_IN  soc_port_t  port,
    SOC_SAND_IN  soc_lbg_t   lbg_id,
    SOC_SAND_IN  int         speed
  );

typedef soc_error_t (*mbcm_dpp_lb_port_speed_set_f) (
    SOC_SAND_IN  int         unit, 
    SOC_SAND_IN  soc_port_t  port,
    SOC_SAND_IN  int         speed
  );

typedef soc_error_t (*mbcm_dpp_lb_port_speed_get_f) (
    SOC_SAND_IN  int         unit, 
    SOC_SAND_IN  soc_port_t  port,
    SOC_SAND_OUT int         *speed
  );

typedef soc_error_t (*mbcm_dpp_lb_config_info_get_f) (
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  soc_lb_info_key_t      *lb_key,
    SOC_SAND_OUT soc_lb_cfg_info_t      *lb_cfg_info 
  );

typedef uint32 (*mbcm_dpp_mtr_ir_val_to_max_rev_exp_optimized_for_bucket_rate_f) (
    SOC_SAND_IN int                          unit,
    SOC_SAND_IN uint32                       *rates,
    SOC_SAND_IN uint32                       nof_rates,
    SOC_SAND_OUT uint32                      *rev_exp
  );

typedef int (* mbcm_dpp_fc_pfc_index_get_f)(
    int unit, 
    uint32 phy_port, 
    uint32 *entry_offset
    );

typedef soc_error_t (*mbcm_dpp_port_timestamp_adjust_set_f) (
    int                                 unit,
    soc_port_t                          port,
    soc_port_timestamp_adjust_t         *ts_adjust
  );

typedef soc_error_t (*mbcm_dpp_port_timestamp_adjust_get_f) (
    int                                 unit,
    soc_port_t                          port,
    soc_port_timestamp_adjust_t         *ts_adjust
  );

typedef soc_error_t (*mbcm_dpp_port_master_set_f) (
    int                                 unit,
    soc_port_t                          port,
    int                                 ms_mode
  );

typedef soc_error_t (*mbcm_dpp_port_master_get_f) (
    int                                 unit,
    soc_port_t                          port,
    int                                 *ms_mode
  );

typedef soc_error_t (*mbcm_dpp_queue_flush_set_f) (
    int                                 unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  int                    queue_id,
    SOC_SAND_IN  int                    timeout
  );

typedef soc_error_t (*mbcm_dpp_sysport_ingress_queue_map_enable_set_f) (
    int                                 unit,
    SOC_SAND_IN  int                    sysport,
    SOC_SAND_IN  int                    enable
  );

typedef soc_error_t (*mbcm_dpp_sysport_ingress_queue_map_enable_get_f) (
    int                                 unit,
    SOC_SAND_IN  int                    sysport,
    SOC_SAND_OUT int                    *enable
  );


typedef struct mbcm_dpp_functions_s {
     
    mbcm_dpp_action_cmd_snoop_set_f               mbcm_dpp_action_cmd_snoop_set; 
    mbcm_dpp_action_cmd_snoop_get_f               mbcm_dpp_action_cmd_snoop_get; 
    mbcm_dpp_action_cmd_mirror_set_f              mbcm_dpp_action_cmd_mirror_set; 
    mbcm_dpp_action_cmd_mirror_get_f              mbcm_dpp_action_cmd_mirror_get; 
    mbcm_dpp_action_cmd_cosq_flush_f              mbcm_dpp_action_cmd_cosq_flush;  
     
    mbcm_dpp_cnm_cp_set_f                         mbcm_dpp_cnm_cp_set; 
    mbcm_dpp_cnm_cp_get_f                         mbcm_dpp_cnm_cp_get; 
    mbcm_dpp_cnm_q_mapping_set_f                  mbcm_dpp_cnm_q_mapping_set; 
    mbcm_dpp_cnm_q_mapping_get_f                  mbcm_dpp_cnm_q_mapping_get; 
    mbcm_dpp_cnm_congestion_test_set_f            mbcm_dpp_cnm_congestion_test_set; 
    mbcm_dpp_cnm_congestion_test_get_f            mbcm_dpp_cnm_congestion_test_get; 
    mbcm_dpp_cnm_cp_profile_set_f                 mbcm_dpp_cnm_cp_profile_set; 
    mbcm_dpp_cnm_cp_profile_get_f                 mbcm_dpp_cnm_cp_profile_get; 
    mbcm_dpp_cnm_sampling_profile_set_f           mbcm_dpp_cnm_sampling_profile_set; 
    mbcm_dpp_cnm_sampling_profile_get_f           mbcm_dpp_cnm_sampling_profile_get; 
    mbcm_dpp_cnm_cpq_pp_set_f                     mbcm_dpp_cnm_cpq_pp_set; 
    mbcm_dpp_cnm_cpq_pp_get_f                     mbcm_dpp_cnm_cpq_pp_get; 
    mbcm_dpp_cnm_cpq_sampling_set_f               mbcm_dpp_cnm_cpq_sampling_set; 
    mbcm_dpp_cnm_cpq_sampling_get_f               mbcm_dpp_cnm_cpq_sampling_get; 
    mbcm_dpp_cnm_intercept_timer_set_f            mbcm_dpp_cnm_intercept_timer_set; 
    mbcm_dpp_cnm_intercept_timer_get_f            mbcm_dpp_cnm_intercept_timer_get; 
     
    mbcm_dpp_cnt_counters_set_f                   mbcm_dpp_cnt_counters_set; 
    mbcm_dpp_cnt_channel_to_fifo_mapping_set_f    mbcm_dpp_cnt_channel_to_fifo_mapping_set;
    mbcm_dpp_cnt_channel_to_fifo_mapping_get_f    mbcm_dpp_cnt_channel_to_fifo_mapping_get;
    mbcm_dpp_cnt_dma_unset_f                      mbcm_dpp_cnt_dma_unset;
    mbcm_dpp_cnt_dma_set_f                        mbcm_dpp_cnt_dma_set; 
    mbcm_dpp_cnt_counters_get_f                   mbcm_dpp_cnt_counters_get; 
    mbcm_dpp_cnt_status_get_f                     mbcm_dpp_cnt_status_get; 
    mbcm_dpp_cnt_engine_to_fifo_dma_index_f       mbcm_dpp_cnt_engine_to_fifo_dma_index;
    mbcm_dpp_cnt_fifo_dma_offset_in_engine_f      mbcm_dpp_cnt_fifo_dma_offset_in_engine;
    mbcm_dpp_max_we_val_get_f                     mbcm_dpp_max_we_val_get;
    mbcm_dpp_cnt_algorithmic_read_f               mbcm_dpp_cnt_algorithmic_read; 
    mbcm_dpp_cnt_direct_read_f                    mbcm_dpp_cnt_direct_read; 
    mbcm_dpp_cnt_q2cnt_id_f                       mbcm_dpp_cnt_q2cnt_id; 
    mbcm_dpp_cnt_cnt2q_id_f                       mbcm_dpp_cnt_cnt2q_id; 
    mbcm_dpp_cnt_lif_counting_set_f               mbcm_dpp_cnt_lif_counting_set;
    mbcm_dpp_cnt_lif_counting_get_f               mbcm_dpp_cnt_lif_counting_get;
    mbcm_dpp_cnt_lif_counting_range_set_f         mbcm_dpp_cnt_lif_counting_range_set;
    mbcm_dpp_cnt_lif_counting_range_get_f         mbcm_dpp_cnt_lif_counting_range_get;
    mbcm_dpp_cnt_out_lif_counting_range_get_f     mbcm_dpp_cnt_out_lif_counting_range_get;
    mbcm_dpp_cnt_base_val_set_f                   mbcm_dpp_cnt_base_val_set;
    mbcm_dpp_cnt_pni_statistics_enable_f          mbcm_dpp_cnt_pni_statistics_enable;
    mbcm_dpp_cnt_epni_regs_set_f                  mbcm_dpp_cnt_epni_regs_set;
    mbcm_dpp_cnt_meter_hdr_compensation_set_f     mbcm_dpp_cnt_meter_hdr_compensation_set; 
    mbcm_dpp_cnt_meter_hdr_compensation_get_f     mbcm_dpp_cnt_meter_hdr_compensation_get; 

     
    mbcm_dpp_diag_last_packet_info_get_f          mbcm_dpp_diag_last_packet_info_get; 
    mbcm_dpp_diag_sample_enable_set_f             mbcm_dpp_diag_sample_enable_set; 
    mbcm_dpp_diag_sample_enable_get_f             mbcm_dpp_diag_sample_enable_get; 
    mbcm_dpp_diag_signals_dump_f                  mbcm_dpp_diag_signals_dump; 
     
    mbcm_dpp_egr_q_prio_set_f                     mbcm_dpp_egr_q_prio_set; 
    mbcm_dpp_egr_q_prio_get_f                     mbcm_dpp_egr_q_prio_get; 
    mbcm_dpp_egr_q_profile_map_set_f              mbcm_dpp_egr_q_profile_map_set; 
    mbcm_dpp_egr_q_profile_map_get_f              mbcm_dpp_egr_q_profile_map_get;
    mbcm_dpp_egr_q_cgm_interface_set_f            mbcm_dpp_egr_q_cgm_interface_set;
    mbcm_dpp_egr_q_fqp_scheduler_config_f         mbcm_dpp_egr_q_fqp_scheduler_config;
    mbcm_dpp_egr_congestion_statistics_get_f      mbcm_dpp_egr_congestion_statistics_get;
     
    mbcm_dpp_fabric_line_coding_set_f             mbcm_dpp_fabric_line_coding_set; 
    mbcm_dpp_fabric_line_coding_get_f             mbcm_dpp_fabric_line_coding_get;
    mbcm_dpp_fabric_pcp_dest_mode_config_set_f    mbcm_dpp_fabric_pcp_dest_mode_config_set;
    mbcm_dpp_fabric_pcp_dest_mode_config_get_f    mbcm_dpp_fabric_pcp_dest_mode_config_get;
    mbcm_dpp_port_fabric_clk_freq_init_f          mbcm_dpp_port_fabric_clk_freq_init;
    
    mbcm_dpp_pll_info_get_f                       mbcm_dpp_pll_info_get;
    mbcm_dpp_interface_id_verify_f                mbcm_dpp_interface_id_verify; 
    mbcm_dpp_if_type_from_id_f                    mbcm_dpp_if_type_from_id; 
    mbcm_dpp_mal_equivalent_id_verify_f           mbcm_dpp_mal_equivalent_id_verify;
    mbcm_dpp_ipt_init_f                           mbcm_dpp_ipt_init;
    mbcm_dpp_stat_fabric_init_f                   mbcm_dpp_stat_fabric_init;
    mbcm_dpp_stat_nif_init_f                      mbcm_dpp_stat_nif_init;
    mbcm_dpp_stat_path_info_get_f                 mbcm_dpp_stat_path_info_get;
    mbcm_dpp_info_config_custom_reg_access_f      mbcm_dpp_info_config_custom_reg_access;
    mbcm_dpp_mapping_stat_get_f                   mbcm_dpp_mapping_stat_get;
    mbcm_dpp_stat_counter_length_get_f            mbcm_dpp_stat_counter_length_get;
    mbcm_dpp_stat_controlled_counter_enable_get_f mbcm_dpp_stat_controlled_counter_enable_get;
     
    mbcm_dpp_itm_committed_q_size_set_f           mbcm_dpp_itm_committed_q_size_set; 
    mbcm_dpp_itm_committed_q_size_get_f           mbcm_dpp_itm_committed_q_size_get; 
    mbcm_dpp_itm_max_latency_pkts_get_f           mbcm_dpp_itm_max_latency_pkts_get;        
    mbcm_dpp_itm_rate_limit_mpps_set_f            mbcm_dpp_itm_rate_limit_mpps_set;
    mbcm_dpp_itm_rate_limit_mpps_get_f            mbcm_dpp_itm_rate_limit_mpps_get;
    
    mbcm_dpp_cosq_gport_sched_set_f               mbcm_dpp_cosq_gport_sched_set;
    mbcm_dpp_cosq_gport_sched_get_f               mbcm_dpp_cosq_gport_sched_get;
     
    mbcm_dpp_port_pp_port_set_f                   mbcm_dpp_port_pp_port_set; 
    mbcm_dpp_port_pp_port_get_f                   mbcm_dpp_port_pp_port_get; 
    mbcm_dpp_port_to_pp_port_map_set_f            mbcm_dpp_port_to_pp_port_map_set; 
    mbcm_dpp_port_to_pp_port_map_get_f            mbcm_dpp_port_to_pp_port_map_get; 
    mbcm_dpp_port_control_low_latency_set_f       mbcm_dpp_port_control_low_latency_set;
    mbcm_dpp_port_control_fec_error_detect_set_f  mbcm_dpp_port_control_fec_error_detect_set;
    mbcm_dpp_port_control_low_latency_get_f       mbcm_dpp_port_control_low_latency_get;
    mbcm_dpp_port_control_fec_error_detect_get_f  mbcm_dpp_port_control_fec_error_detect_get;
    mbcm_dpp_port_extract_cig_from_llfc_enable_set_f  mbcm_dpp_port_extract_cig_from_llfc_enable_set;
    mbcm_dpp_port_extract_cig_from_llfc_enable_get_f  mbcm_dpp_port_extract_cig_from_llfc_enable_get;
    mbcm_dpp_port_forwarding_header_set_f         mbcm_dpp_port_forwarding_header_set; 
    mbcm_dpp_port_forwarding_header_get_f         mbcm_dpp_port_forwarding_header_get;
    mbcm_dpp_port_nrdy_th_profile_set_f           mbcm_dpp_port_nrdy_th_profile_set;
    mbcm_dpp_port_nrdy_th_profile_get_f           mbcm_dpp_port_nrdy_th_profile_get;
    mbcm_dpp_port_nrdy_th_optimial_value_get_f    mbcm_dpp_port_nrdy_th_optimial_value_get;
     
    mbcm_dpp_ports_logical_sys_id_build_with_device_f         mbcm_dpp_ports_logical_sys_id_build_with_device; 
    mbcm_dpp_sys_virtual_port_to_local_port_map_set_unsafe_f mbcm_dpp_sys_virtual_port_to_local_port_map_set_unsafe; 
     
    mbcm_dpp_ssr_buff_size_get_f                  mbcm_dpp_ssr_buff_size_get; 
    mbcm_dpp_ssr_to_buff_f                        mbcm_dpp_ssr_to_buff; 
    mbcm_dpp_ssr_from_buff_f                      mbcm_dpp_ssr_from_buff; 
    mbcm_dpp_ssr_is_device_init_done_f            mbcm_dpp_ssr_is_device_init_done; 
     
    mbcm_dpp_stack_global_info_set_f              mbcm_dpp_stack_global_info_set; 
    mbcm_dpp_stack_global_info_get_f              mbcm_dpp_stack_global_info_get; 
    mbcm_dpp_stack_port_distribution_info_set_f   mbcm_dpp_stack_port_distribution_info_set; 
    mbcm_dpp_stack_port_distribution_info_get_f   mbcm_dpp_stack_port_distribution_info_get;
     
    mbcm_dpp_tdm_ftmh_set_f                       mbcm_dpp_tdm_ftmh_set; 
    mbcm_dpp_tdm_ftmh_get_f                       mbcm_dpp_tdm_ftmh_get; 
    mbcm_dpp_tdm_opt_size_set_f                   mbcm_dpp_tdm_opt_size_set; 
    mbcm_dpp_tdm_opt_size_get_f                   mbcm_dpp_tdm_opt_size_get; 
    mbcm_dpp_tdm_stand_size_range_set_f           mbcm_dpp_tdm_stand_size_range_set; 
    mbcm_dpp_tdm_stand_size_range_get_f           mbcm_dpp_tdm_stand_size_range_get; 
    mbcm_dpp_tdm_mc_static_route_set_f            mbcm_dpp_tdm_mc_static_route_set; 
    mbcm_dpp_tdm_mc_static_route_get_f            mbcm_dpp_tdm_mc_static_route_get; 
    mbcm_dpp_tdm_port_packet_crc_set_f            mbcm_dpp_tdm_port_packet_crc_set;
    mbcm_dpp_tdm_port_packet_crc_get_f            mbcm_dpp_tdm_port_packet_crc_get;
    mbcm_dpp_tdm_direct_routing_set_f             mbcm_dpp_tdm_direct_routing_set;
    mbcm_dpp_tdm_direct_routing_get_f             mbcm_dpp_tdm_direct_routing_get;
    mbcm_dpp_tdm_direct_routing_profile_map_set_f mbcm_dpp_tdm_direct_routing_profile_map_set;
    mbcm_dpp_tdm_direct_routing_profile_map_get_f mbcm_dpp_tdm_direct_routing_profile_map_get;
    mbcm_dpp_tdm_ifp_get_f                        mbcm_dpp_tdm_ifp_get;
    mbcm_dpp_tdm_ifp_set_f                        mbcm_dpp_tdm_ifp_set;
     
    mbcm_dpp_read_from_fe600_f                    mbcm_dpp_read_from_fe600; 
    mbcm_dpp_write_to_fe600_f                     mbcm_dpp_write_to_fe600; 
    mbcm_dpp_indirect_read_from_fe600_f           mbcm_dpp_indirect_read_from_fe600; 
    mbcm_dpp_indirect_write_to_fe600_f            mbcm_dpp_indirect_write_to_fe600; 
    mbcm_dpp_cpu2cpu_write_f                      mbcm_dpp_cpu2cpu_write; 
    mbcm_dpp_cpu2cpu_read_f                       mbcm_dpp_cpu2cpu_read;   
    mbcm_dpp_cell_mc_tbl_write_f                  mbcm_dpp_cell_mc_tbl_write; 
    mbcm_dpp_cell_mc_tbl_read_f                   mbcm_dpp_cell_mc_tbl_read; 
     
    mbcm_dpp_diag_ipt_rate_get_f                  mbcm_dpp_diag_ipt_rate_get; 
    mbcm_dpp_diag_iddr_set_f                      mbcm_dpp_diag_iddr_set; 
    mbcm_dpp_diag_iddr_get_f                      mbcm_dpp_diag_iddr_get; 
    mbcm_dpp_diag_regs_dump_f                     mbcm_dpp_diag_regs_dump; 
    mbcm_dpp_diag_tbls_dump_f                     mbcm_dpp_diag_tbls_dump; 
    mbcm_dpp_diag_tbls_dump_all_f                 mbcm_dpp_diag_tbls_dump_all; 
    mbcm_dpp_nif_diag_last_packet_get_f           mbcm_dpp_nif_diag_last_packet_get; 
    mbcm_dpp_diag_soft_error_test_start_f         mbcm_dpp_diag_soft_error_test_start; 
    mbcm_dpp_egq_resources_print_f                mbcm_dpp_egq_resources_print; 
     
    mbcm_dpp_egr_threshold_types_verify_f         mbcm_dpp_egr_threshold_types_verify;
    mbcm_dpp_egr_ofp_thresh_type_set_f            mbcm_dpp_egr_ofp_thresh_type_set; 
    mbcm_dpp_egr_ofp_thresh_type_get_f            mbcm_dpp_egr_ofp_thresh_type_get; 
    mbcm_dpp_egr_sched_drop_set_f                 mbcm_dpp_egr_sched_drop_set; 
    mbcm_dpp_egr_sched_drop_get_f                 mbcm_dpp_egr_sched_drop_get; 
    mbcm_dpp_egr_unsched_drop_set_f               mbcm_dpp_egr_unsched_drop_set; 
    mbcm_dpp_egr_unsched_drop_get_f               mbcm_dpp_egr_unsched_drop_get; 
    mbcm_dpp_egr_dev_fc_set_f                     mbcm_dpp_egr_dev_fc_set; 
    mbcm_dpp_egr_dev_fc_get_f                     mbcm_dpp_egr_dev_fc_get; 
    mbcm_dpp_egr_xaui_spaui_fc_set_f              mbcm_dpp_egr_xaui_spaui_fc_set; 
    mbcm_dpp_egr_xaui_spaui_fc_get_f              mbcm_dpp_egr_xaui_spaui_fc_get; 
    mbcm_dpp_egr_ofp_fc_set_f                     mbcm_dpp_egr_ofp_fc_set; 
    mbcm_dpp_egr_sched_port_fc_thresh_set_f       mbcm_dpp_egr_sched_port_fc_thresh_set;
    mbcm_dpp_egr_sched_q_fc_thresh_set_f          mbcm_dpp_egr_sched_q_fc_thresh_set;
    mbcm_dpp_egr_ofp_fc_get_f                     mbcm_dpp_egr_ofp_fc_get; 
    mbcm_dpp_egr_mci_fc_set_f                     mbcm_dpp_egr_mci_fc_set; 
    mbcm_dpp_egr_mci_fc_get_f                     mbcm_dpp_egr_mci_fc_get; 
    mbcm_dpp_egr_mci_fc_enable_set_f              mbcm_dpp_egr_mci_fc_enable_set; 
    mbcm_dpp_egr_mci_fc_enable_get_f              mbcm_dpp_egr_mci_fc_enable_get; 
    mbcm_dpp_egr_ofp_sch_mode_set_f               mbcm_dpp_egr_ofp_sch_mode_set; 
    mbcm_dpp_egr_ofp_sch_mode_get_f               mbcm_dpp_egr_ofp_sch_mode_get; 
    mbcm_dpp_egr_ofp_scheduling_set_f             mbcm_dpp_egr_ofp_scheduling_set; 
    mbcm_dpp_egr_ofp_scheduling_get_f             mbcm_dpp_egr_ofp_scheduling_get; 
    mbcm_dpp_egr_unsched_drop_prio_set_f          mbcm_dpp_egr_unsched_drop_prio_set; 
    mbcm_dpp_egr_unsched_drop_prio_get_f          mbcm_dpp_egr_unsched_drop_prio_get;   
     
    mbcm_dpp_sch_device_rate_entry_set_f          mbcm_dpp_sch_device_rate_entry_set; 
    mbcm_dpp_sch_device_rate_entry_get_f          mbcm_dpp_sch_device_rate_entry_get; 
    mbcm_dpp_sch_device_rate_entry_core_set_f     mbcm_dpp_sch_device_rate_entry_core_set; 
    mbcm_dpp_sch_device_rate_entry_core_get_f     mbcm_dpp_sch_device_rate_entry_core_get; 
    mbcm_dpp_sch_if_shaper_rate_set_f             mbcm_dpp_sch_if_shaper_rate_set; 
    mbcm_dpp_sch_if_shaper_rate_get_f             mbcm_dpp_sch_if_shaper_rate_get; 
    mbcm_dpp_sch_device_if_weight_idx_set_f       mbcm_dpp_sch_device_if_weight_idx_set; 
    mbcm_dpp_sch_device_if_weight_idx_get_f       mbcm_dpp_sch_device_if_weight_idx_get; 
    mbcm_dpp_sch_if_weight_conf_set_f             mbcm_dpp_sch_if_weight_conf_set; 
    mbcm_dpp_sch_if_weight_conf_get_f             mbcm_dpp_sch_if_weight_conf_get; 
    mbcm_dpp_sch_class_type_params_set_f          mbcm_dpp_sch_class_type_params_set; 
    mbcm_dpp_sch_class_type_params_get_f          mbcm_dpp_sch_class_type_params_get; 
    mbcm_dpp_sch_class_type_params_table_set_f    mbcm_dpp_sch_class_type_params_table_set; 
    mbcm_dpp_sch_class_type_params_table_get_f    mbcm_dpp_sch_class_type_params_table_get; 
    mbcm_dpp_sch_port_sched_set_f                 mbcm_dpp_sch_port_sched_set; 
    mbcm_dpp_sch_port_sched_get_f                 mbcm_dpp_sch_port_sched_get; 
    mbcm_dpp_sch_port_hp_class_conf_set_f         mbcm_dpp_sch_port_hp_class_conf_set; 
    mbcm_dpp_sch_port_hp_class_conf_get_f         mbcm_dpp_sch_port_hp_class_conf_get; 
    mbcm_dpp_sch_slow_max_rates_set_f             mbcm_dpp_sch_slow_max_rates_set; 
    mbcm_dpp_sch_slow_max_rates_get_f             mbcm_dpp_sch_slow_max_rates_get; 
    mbcm_dpp_sch_slow_max_rates_per_level_set_f   mbcm_dpp_sch_slow_max_rates_per_level_set;
    mbcm_dpp_sch_slow_max_rates_per_level_get_f   mbcm_dpp_sch_slow_max_rates_per_level_get;
    mbcm_dpp_sch_aggregate_set_f                  mbcm_dpp_sch_aggregate_set; 
    mbcm_dpp_sch_aggregate_group_set_f            mbcm_dpp_sch_aggregate_group_set;
    mbcm_dpp_sch_aggregate_get_f                  mbcm_dpp_sch_aggregate_get; 
    mbcm_dpp_sch_flow_delete_f                    mbcm_dpp_sch_flow_delete; 
    mbcm_dpp_sch_flow_set_f                       mbcm_dpp_sch_flow_set; 
    mbcm_dpp_sch_flow_get_f                       mbcm_dpp_sch_flow_get; 
    mbcm_dpp_sch_flow_status_set_f                mbcm_dpp_sch_flow_status_set; 
    mbcm_dpp_sch_flow_ipf_mode_set_f              mbcm_dpp_sch_ipf_mode_set;
    mbcm_dpp_sch_flow_ipf_mode_get_f              mbcm_dpp_sch_ipf_mode_get;
    mbcm_dpp_sch_per1k_info_set_f                 mbcm_dpp_sch_per1k_info_set; 
    mbcm_dpp_sch_per1k_info_get_f                 mbcm_dpp_sch_per1k_info_get; 
    mbcm_dpp_sch_flow_to_queue_mapping_set_f      mbcm_dpp_sch_flow_to_queue_mapping_set; 
    mbcm_dpp_sch_flow_to_queue_mapping_get_f      mbcm_dpp_sch_flow_to_queue_mapping_get; 
    mbcm_dpp_sch_flow_id_verify_unsafe_f          mbcm_dpp_sch_flow_id_verify_unsafe; 
    mbcm_dpp_sch_se_id_verify_unsafe_f            mbcm_dpp_sch_se_id_verify_unsafe; 
    mbcm_dpp_sch_port_id_verify_unsafe_f          mbcm_dpp_sch_port_id_verify_unsafe; 
    mbcm_dpp_sch_k_flow_id_verify_unsafe_f        mbcm_dpp_sch_k_flow_id_verify_unsafe; 
    mbcm_dpp_sch_quartet_id_verify_unsafe_f       mbcm_dpp_sch_quartet_id_verify_unsafe; 
    mbcm_dpp_sch_se2port_id_f                     mbcm_dpp_sch_se2port_id; 
    mbcm_dpp_sch_se2port_tc_id_f                  mbcm_dpp_sch_se2port_tc_id;
    mbcm_dpp_sch_flow2se_id_f                     mbcm_dpp_sch_flow2se_id; 
    mbcm_dpp_sch_port2se_id_f                     mbcm_dpp_sch_port2se_id; 
    mbcm_dpp_sch_port_tc2se_id_f                  mbcm_dpp_sch_port_tc2se_id;
    mbcm_dpp_sch_se2flow_id_f                     mbcm_dpp_sch_se2flow_id;
    mbcm_dpp_sch_se_get_type_by_id_f              mbcm_dpp_sch_se_get_type_by_id;
    mbcm_dpp_sch_e2e_interface_allocate_f         mbcm_dpp_sch_e2e_interface_allocate;
    mbcm_dpp_sch_e2e_interface_deallocate_f       mbcm_dpp_sch_e2e_interface_deallocate;
    mbcm_dpp_sch_prio_propagation_enable_set_f    mbcm_dpp_sch_prio_propagation_enable_set;
    mbcm_dpp_sch_prio_propagation_enable_get_f    mbcm_dpp_sch_prio_propagation_enable_get;
    mbcm_dpp_sch_prio_propagation_port_set_f      mbcm_dpp_sch_prio_propagation_port_set;
    mbcm_dpp_sch_prio_propagation_port_get_f      mbcm_dpp_sch_prio_propagation_port_get;
    mbcm_dpp_sch_shds_tbl_get_unsafe_f            mbcm_dpp_sch_shds_tbl_get_unsafe; 
    mbcm_dpp_sch_shds_tbl_set_unsafe_f            mbcm_dpp_sch_shds_tbl_set_unsafe; 
     
    mbcm_dpp_fabric_fc_enable_set_f               mbcm_dpp_fabric_fc_enable_set; 
    mbcm_dpp_fabric_fc_enable_get_f               mbcm_dpp_fabric_fc_enable_get; 
    mbcm_dpp_fabric_fc_shaper_get_f               mbcm_dpp_fabric_fc_shaper_get;
    mbcm_dpp_fabric_fc_shaper_set_f               mbcm_dpp_fabric_fc_shaper_set;
    mbcm_dpp_fabric_cell_format_get_f             mbcm_dpp_fabric_cell_format_get; 
    mbcm_dpp_fabric_coexist_set_f                 mbcm_dpp_fabric_coexist_set; 
    mbcm_dpp_fabric_coexist_get_f                 mbcm_dpp_fabric_coexist_get; 
    mbcm_dpp_fabric_stand_alone_fap_mode_get_f    mbcm_dpp_fabric_stand_alone_fap_mode_get; 
    mbcm_dpp_fabric_connect_mode_set_f            mbcm_dpp_fabric_connect_mode_set; 
    mbcm_dpp_fabric_connect_mode_get_f            mbcm_dpp_fabric_connect_mode_get; 
    mbcm_dpp_fabric_fap20_map_set_f               mbcm_dpp_fabric_fap20_map_set; 
    mbcm_dpp_fabric_fap20_map_get_f               mbcm_dpp_fabric_fap20_map_get;
    mbcm_dpp_fabric_priority_set_f                mbcm_dpp_fabric_priority_set; 
    mbcm_dpp_fabric_priority_get_f                mbcm_dpp_fabric_priority_get;
    mbcm_dpp_fabric_topology_status_connectivity_get_f mbcm_dpp_fabric_topology_status_connectivity_get; 
    mbcm_dpp_fabric_links_status_get_f            mbcm_dpp_fabric_links_status_get; 
    mbcm_dpp_fabric_aldwp_config_f                mbcm_dpp_fabric_aldwp_config;
    mbcm_dpp_fabric_topology_status_connectivity_print_f mbcm_dpp_fabric_topology_status_connectivity_print;
    mbcm_dpp_fabric_nof_links_get_f               mbcm_dpp_fabric_nof_links_get;
    mbcm_dpp_fabric_gci_enable_set_f              mbcm_dpp_fabric_gci_enable_set;
    mbcm_dpp_fabric_gci_enable_get_f              mbcm_dpp_fabric_gci_enable_get;
    mbcm_dpp_fabric_gci_config_set_f              mbcm_dpp_fabric_gci_config_set;
    mbcm_dpp_fabric_gci_config_get_f              mbcm_dpp_fabric_gci_config_get;
    mbcm_dpp_fabric_gci_backoff_masks_init_f      mbcm_dpp_fabric_gci_backoff_masks_init;
    mbcm_dpp_fabric_llfc_threshold_set_f          mbcm_dpp_fabric_llfc_threshold_set;
    mbcm_dpp_fabric_llfc_threshold_get_f          mbcm_dpp_fabric_llfc_threshold_get;
    mbcm_dpp_fabric_rci_enable_set_f              mbcm_dpp_fabric_rci_enable_set;
    mbcm_dpp_fabric_rci_enable_get_f              mbcm_dpp_fabric_rci_enable_get;
    mbcm_dpp_fabric_rci_config_set_f              mbcm_dpp_fabric_rci_config_set;
    mbcm_dpp_fabric_rci_config_get_f              mbcm_dpp_fabric_rci_config_get;
    mbcm_dpp_fabric_minimal_links_to_dest_set_f   mbcm_dpp_fabric_minimal_links_to_dest_set; 
    mbcm_dpp_fabric_minimal_links_to_dest_get_f   mbcm_dpp_fabric_minimal_links_to_dest_get;
    mbcm_dpp_fabric_minimal_links_all_reachable_set_f  mbcm_dpp_fabric_minimal_links_all_reachable_set;
    mbcm_dpp_fabric_minimal_links_all_reachable_get_f  mbcm_dpp_fabric_minimal_links_all_reachable_get;
    mbcm_dpp_fabric_link_tx_traffic_disable_set_f mbcm_dpp_fabric_link_tx_traffic_disable_set;
    mbcm_dpp_fabric_link_tx_traffic_disable_get_f mbcm_dpp_fabric_link_tx_traffic_disable_get;
    mbcm_dpp_fabric_link_thresholds_pipe_set_f    mbcm_dpp_fabric_link_thresholds_pipe_set;
    mbcm_dpp_fabric_link_thresholds_pipe_get_f    mbcm_dpp_fabric_link_thresholds_pipe_get;
    mbcm_dpp_fabric_cosq_control_backward_flow_control_set_f  mbcm_dpp_fabric_cosq_control_backward_flow_control_set;
    mbcm_dpp_fabric_cosq_control_backward_flow_control_get_f  mbcm_dpp_fabric_cosq_control_backward_flow_control_get;
    mbcm_dpp_fabric_egress_core_cosq_gport_sched_set_f  mbcm_dpp_fabric_egress_core_cosq_gport_sched_set;
    mbcm_dpp_fabric_egress_core_cosq_gport_sched_get_f  mbcm_dpp_fabric_egress_core_cosq_gport_sched_get;
    mbcm_dpp_fabric_cosq_gport_rci_threshold_set_f  mbcm_dpp_fabric_cosq_gport_rci_threshold_set;
    mbcm_dpp_fabric_cosq_gport_rci_threshold_get_f  mbcm_dpp_fabric_cosq_gport_rci_threshold_get;
    mbcm_dpp_fabric_cosq_gport_priority_drop_threshold_set_f  mbcm_dpp_fabric_cosq_gport_priority_drop_threshold_set;
    mbcm_dpp_fabric_cosq_gport_priority_drop_threshold_get_f  mbcm_dpp_fabric_cosq_gport_priority_drop_threshold_get;
    mbcm_dpp_fabric_link_topology_set_f           mbcm_dpp_fabric_link_topology_set;
    mbcm_dpp_fabric_link_topology_get_f           mbcm_dpp_fabric_link_topology_get;
    mbcm_dpp_fabric_link_topology_unset_f         mbcm_dpp_fabric_link_topology_unset;
    mbcm_dpp_fabric_static_replication_set_f      mbcm_dpp_static_replication_set;
    mbcm_dpp_fabric_multicast_set_f               mbcm_dpp_fabric_multicast_set;
    mbcm_dpp_fabric_multicast_get_f               mbcm_dpp_fabric_multicast_get;
    mbcm_dpp_fabric_modid_group_set_f             mbcm_dpp_fabric_modid_group_set;
    mbcm_dpp_fabric_modid_group_get_f             mbcm_dpp_fabric_modid_group_get;
    mbcm_dpp_fabric_local_dest_id_verify_f        mbcm_dpp_fabric_local_dest_id_verify;
    mbcm_dpp_fabric_rci_thresholds_config_set_f              mbcm_dpp_fabric_rci_thresholds_config_set;
    mbcm_dpp_fabric_rci_thresholds_config_get_f              mbcm_dpp_fabric_rci_thresholds_config_get;
    mbcm_dpp_fabric_link_repeater_enable_set_f    mbcm_dpp_fabric_link_repeater_enable_set;
    mbcm_dpp_fabric_link_repeater_enable_get_f    mbcm_dpp_fabric_link_repeater_enable_get;
    mbcm_dpp_fabric_queues_info_get_f             mbcm_dpp_fabric_queues_info_get;
    mbcm_dpp_fabric_cpu2cpu_write_f               mbcm_dpp_fabric_cpu2cpu_write;
    mbcm_dpp_fabric_mesh_topology_get_f           mbcm_dpp_fabric_mesh_topology_get;
    mbcm_dpp_fabric_mesh_check_f                  mbcm_dpp_fabric_mesh_check;
    mbcm_dpp_fabric_rx_fifo_status_get_f          mbcm_dpp_fabric_rx_fifo_status_get;
    mbcm_dpp_fabric_port_sync_e_link_set_f        mbcm_dpp_fabric_port_sync_e_link_set;
    mbcm_dpp_fabric_port_sync_e_link_get_f        mbcm_dpp_fabric_port_sync_e_link_get;
    mbcm_dpp_fabric_port_sync_e_divider_set_f     mbcm_dpp_fabric_port_sync_e_divider_set;
    mbcm_dpp_fabric_port_sync_e_divider_get_f     mbcm_dpp_fabric_port_sync_e_divider_get;
    mbcm_dpp_fabric_sync_e_enable_get_f           mbcm_dpp_fabric_sync_e_enable_get;
    mbcm_dpp_fabric_force_set_f                   mbcm_dpp_fabric_force_set;
    mbcm_dpp_fabric_stack_module_all_reachable_ignore_id_set_f  mbcm_dpp_fabric_stack_module_all_reachable_ignore_id_set;
    mbcm_dpp_fabric_stack_module_all_reachable_ignore_id_get_f  mbcm_dpp_fabric_stack_module_all_reachable_ignore_id_get;
    mbcm_dpp_fabric_stack_module_max_all_reachable_set_f  mbcm_dpp_fabric_stack_module_max_all_reachable_set;
    mbcm_dpp_fabric_stack_module_max_all_reachable_get_f  mbcm_dpp_fabric_stack_module_max_all_reachable_get;
    mbcm_dpp_fabric_stack_module_max_set_f               mbcm_dpp_fabric_stack_module_max_set;
    mbcm_dpp_fabric_stack_module_max_get_f               mbcm_dpp_fabric_stack_module_max_get;
    mbcm_dpp_fabric_stack_module_devide_by_32_verify_f   mbcm_dpp_fabric_stack_module_devide_by_32_verify;
    mbcm_dpp_fabric_cell_cpu_data_get_f           mbcm_dpp_fabric_cell_cpu_data_get;
    mbcm_dpp_fabric_efms_enable_set_f               mbcm_dpp_fabric_efms_enable_set;
    mbcm_dpp_fabric_efms_enable_get_f               mbcm_dpp_fabric_efms_enable_get;
     
    mbcm_dpp_ipq_explicit_mapping_mode_info_set_f mbcm_dpp_ipq_explicit_mapping_mode_info_set; 
    mbcm_dpp_ipq_explicit_mapping_mode_info_get_f mbcm_dpp_ipq_explicit_mapping_mode_info_get; 
    mbcm_dpp_ipq_traffic_class_map_set_f          mbcm_dpp_ipq_traffic_class_map_set; 
    mbcm_dpp_ipq_traffic_class_map_get_f          mbcm_dpp_ipq_traffic_class_map_get; 
    mbcm_dpp_ipq_traffic_class_multicast_priority_map_set_f          mbcm_dpp_ipq_traffic_class_multicast_priority_map_set;
    mbcm_dpp_ipq_traffic_class_multicast_priority_map_get_f          mbcm_dpp_ipq_traffic_class_multicast_priority_map_get;
    mbcm_dpp_ipq_destination_id_packets_base_queue_id_set_f mbcm_dpp_ipq_destination_id_packets_base_queue_id_set; 
    mbcm_dpp_ipq_destination_id_packets_base_queue_id_get_f mbcm_dpp_ipq_destination_id_packets_base_queue_id_get; 
    mbcm_dpp_ipq_queue_interdigitated_mode_set_f  mbcm_dpp_ipq_queue_interdigitated_mode_set; 
    mbcm_dpp_ipq_queue_interdigitated_mode_get_f  mbcm_dpp_ipq_queue_interdigitated_mode_get; 
    mbcm_dpp_ipq_queue_to_flow_mapping_set_f      mbcm_dpp_ipq_queue_to_flow_mapping_set; 
    mbcm_dpp_ipq_queue_to_flow_mapping_get_f      mbcm_dpp_ipq_queue_to_flow_mapping_get; 
    mbcm_dpp_ipq_queue_qrtt_unmap_f               mbcm_dpp_ipq_queue_qrtt_unmap; 
    mbcm_dpp_ipq_quartet_reset_f                  mbcm_dpp_ipq_quartet_reset; 
    mbcm_dpp_ipq_attached_flow_port_get_f         mbcm_dpp_ipq_attached_flow_port_get; 
    mbcm_dpp_ipq_tc_profile_set_f                 mbcm_dpp_ipq_tc_profile_set;
    mbcm_dpp_ipq_tc_profile_get_f                 mbcm_dpp_ipq_tc_profile_get;
    mbcm_dpp_ipq_tc_profile_map_set_f             mbcm_dpp_ipq_tc_profile_map_set;
    mbcm_dpp_ipq_tc_profile_map_get_f             mbcm_dpp_ipq_tc_profile_map_get;
    mbcm_dpp_ipq_stack_lag_packets_base_queue_id_set_f            mbcm_dpp_ipq_stack_lag_packets_base_queue_id_set;
    mbcm_dpp_ipq_stack_lag_packets_base_queue_id_get_f            mbcm_dpp_ipq_stack_lag_packets_base_queue_id_get;
    mbcm_dpp_ipq_stack_fec_map_stack_lag_set_f                           mbcm_dpp_ipq_stack_fec_map_stack_lag_set;
    mbcm_dpp_ipq_stack_fec_map_stack_lag_get_f                           mbcm_dpp_ipq_stack_fec_map_stack_lag_get;
    mbcm_dpp_ipq_default_invalid_queue_set_f                      mbcm_dpp_ipq_default_invalid_queue_set;
    mbcm_dpp_ipq_default_invalid_queue_get_f                      mbcm_dpp_ipq_default_invalid_queue_get;
     
    mbcm_dpp_ingress_scheduler_clos_bandwidth_set_f         mbcm_dpp_ingress_scheduler_clos_bandwidth_set; 
    mbcm_dpp_ingress_scheduler_clos_bandwidth_get_f         mbcm_dpp_ingress_scheduler_clos_bandwidth_get;
    mbcm_dpp_ingress_scheduler_mesh_bandwidth_set_f         mbcm_dpp_ingress_scheduler_mesh_bandwidth_set; 
    mbcm_dpp_ingress_scheduler_mesh_bandwidth_get_f         mbcm_dpp_ingress_scheduler_mesh_bandwidth_get;
    mbcm_dpp_ingress_scheduler_clos_sched_set_f   mbcm_dpp_ingress_scheduler_clos_sched_set; 
    mbcm_dpp_ingress_scheduler_clos_sched_get_f   mbcm_dpp_ingress_scheduler_clos_sched_get;
    mbcm_dpp_ingress_scheduler_mesh_sched_set_f   mbcm_dpp_ingress_scheduler_mesh_sched_set; 
    mbcm_dpp_ingress_scheduler_mesh_sched_get_f   mbcm_dpp_ingress_scheduler_mesh_sched_get;
    mbcm_dpp_ingress_scheduler_clos_burst_set_f   mbcm_dpp_ingress_scheduler_clos_burst_set; 
    mbcm_dpp_ingress_scheduler_clos_burst_get_f   mbcm_dpp_ingress_scheduler_clos_burst_get;
    mbcm_dpp_ingress_scheduler_mesh_burst_set_f   mbcm_dpp_ingress_scheduler_mesh_burst_set; 
    mbcm_dpp_ingress_scheduler_mesh_burst_get_f   mbcm_dpp_ingress_scheduler_mesh_burst_get;
    mbcm_dpp_ingress_scheduler_clos_slow_start_set_f   mbcm_dpp_ingress_scheduler_clos_slow_start_set;
    mbcm_dpp_ingress_scheduler_clos_slow_start_get_f   mbcm_dpp_ingress_scheduler_clos_slow_start_get;
    mbcm_dpp_ingress_scheduler_mesh_slow_start_set_f   mbcm_dpp_ingress_scheduler_mesh_slow_start_set;
    mbcm_dpp_ingress_scheduler_mesh_slow_start_get_f   mbcm_dpp_ingress_scheduler_mesh_slow_start_get;
    mbcm_dpp_ingress_scheduler_init_f             mbcm_dpp_ingress_scheduler_init;     
     
    mbcm_dpp_ocb_control_range_dram_mix_dbuff_threshold_get_f mbcm_dpp_ocb_control_range_dram_mix_dbuff_threshold_get;
    mbcm_dpp_ocb_control_range_dram_mix_dbuff_threshold_set_f mbcm_dpp_ocb_control_range_dram_mix_dbuff_threshold_set;
    mbcm_dpp_ocb_control_range_ocb_committed_multicast_get_f  mbcm_dpp_ocb_control_range_ocb_committed_multicast_get;
    mbcm_dpp_ocb_control_range_ocb_committed_multicast_set_f  mbcm_dpp_ocb_control_range_ocb_committed_multicast_set;
    mbcm_dpp_ocb_control_range_ocb_eligible_multicast_get_f   mbcm_dpp_ocb_control_range_ocb_eligible_multicast_get;
    mbcm_dpp_ocb_control_range_ocb_eligible_multicast_set_f   mbcm_dpp_ocb_control_range_ocb_eligible_multicast_set;
    mbcm_dpp_itm_dram_buffs_get_f                 mbcm_dpp_itm_dram_buffs_get;
    mbcm_dpp_dram_info_verify_f                   mbcm_dpp_dram_info_verify;
    mbcm_dpp_user_buffer_dram_write_f             mbcm_dpp_user_buffer_dram_write;
    mbcm_dpp_user_buffer_dram_read_f              mbcm_dpp_user_buffer_dram_read;
    mbcm_dpp_dram_validate_address_f              mbcm_dpp_dram_validate_address;
    mbcm_dpp_dram_recovery_init_f                 mbcm_dpp_dram_recovery_init;
    mbcm_dpp_cache_table_update_all_f             mbcm_dpp_cache_table_update_all;
    mbcm_dpp_itm_glob_rcs_fc_set_f                mbcm_dpp_itm_glob_rcs_fc_set; 
    mbcm_dpp_itm_glob_rcs_fc_get_f                mbcm_dpp_itm_glob_rcs_fc_get; 
    mbcm_dpp_itm_glob_rcs_drop_set_f              mbcm_dpp_itm_glob_rcs_drop_set; 
    mbcm_dpp_itm_glob_rcs_drop_get_f              mbcm_dpp_itm_glob_rcs_drop_get; 
    mbcm_dpp_itm_category_rngs_set_f              mbcm_dpp_itm_category_rngs_set; 
    mbcm_dpp_itm_category_rngs_get_f              mbcm_dpp_itm_category_rngs_get; 
    mbcm_dpp_itm_admit_test_tmplt_set_f           mbcm_dpp_itm_admit_test_tmplt_set; 
    mbcm_dpp_itm_admit_test_tmplt_get_f           mbcm_dpp_itm_admit_test_tmplt_get; 
    mbcm_dpp_itm_init_f                           mbcm_dpp_itm_init; 
    mbcm_dpp_ipq_init_f                           mbcm_dpp_ipq_init; 
    mbcm_dpp_itm_cr_request_set_f                 mbcm_dpp_itm_cr_request_set; 
    mbcm_dpp_itm_cr_request_get_f                 mbcm_dpp_itm_cr_request_get; 
    mbcm_dpp_itm_cr_discount_set_f                mbcm_dpp_itm_cr_discount_set; 
    mbcm_dpp_itm_cr_discount_get_f                mbcm_dpp_itm_cr_discount_get; 
    mbcm_dpp_itm_queue_test_tmplt_set_f           mbcm_dpp_itm_queue_test_tmplt_set; 
    mbcm_dpp_itm_queue_test_tmplt_get_f           mbcm_dpp_itm_queue_test_tmplt_get; 
    mbcm_dpp_itm_wred_exp_wq_set_f                mbcm_dpp_itm_wred_exp_wq_set; 
    mbcm_dpp_itm_wred_exp_wq_get_f                mbcm_dpp_itm_wred_exp_wq_get; 
    mbcm_dpp_itm_wred_set_f                       mbcm_dpp_itm_wred_set; 
    mbcm_dpp_itm_wred_get_f                       mbcm_dpp_itm_wred_get; 
    mbcm_dpp_itm_tail_drop_set_f                  mbcm_dpp_itm_tail_drop_set; 
    mbcm_dpp_itm_tail_drop_get_f                  mbcm_dpp_itm_tail_drop_get; 

    mbcm_dpp_itm_fadt_tail_drop_set_f             mbcm_dpp_itm_fadt_tail_drop_set; 
    mbcm_dpp_itm_fadt_tail_drop_get_f             mbcm_dpp_itm_fadt_tail_drop_get; 

    mbcm_dpp_itm_cr_wd_set_f                      mbcm_dpp_itm_cr_wd_set; 
    mbcm_dpp_itm_cr_wd_get_f                      mbcm_dpp_itm_cr_wd_get; 
    mbcm_dpp_itm_enable_ecn_f                     mbcm_dpp_itm_enable_ecn;
    mbcm_dpp_itm_get_ecn_enabled_f                mbcm_dpp_itm_get_ecn_enabled;

    mbcm_dpp_itm_dram_bound_set_f                 mbcm_dpp_itm_dram_bound_set;
    mbcm_dpp_itm_dram_bound_get_f                 mbcm_dpp_itm_dram_bound_get;

    mbcm_dpp_itm_vsq_qt_rt_cls_set_f              mbcm_dpp_itm_vsq_qt_rt_cls_set; 
    mbcm_dpp_itm_vsq_qt_rt_cls_get_f              mbcm_dpp_itm_vsq_qt_rt_cls_get; 
    mbcm_dpp_itm_vsq_fc_set_f                     mbcm_dpp_itm_vsq_fc_set; 
    mbcm_dpp_itm_vsq_fc_get_f                     mbcm_dpp_itm_vsq_fc_get; 
    mbcm_dpp_itm_vsq_tail_drop_set_f              mbcm_dpp_itm_vsq_tail_drop_set; 
    mbcm_dpp_itm_vsq_tail_drop_get_f              mbcm_dpp_itm_vsq_tail_drop_get; 
    mbcm_dpp_itm_vsq_tail_drop_default_get_f      mbcm_dpp_itm_vsq_tail_drop_default_get; 
    mbcm_dpp_itm_vsq_fc_default_get_f             mbcm_dpp_itm_vsq_fc_default_get; 
    mbcm_dpp_itm_vsq_src_port_rjct_set_f          mbcm_dpp_itm_vsq_src_port_rjct_set;
    mbcm_dpp_itm_vsq_src_port_rjct_get_f          mbcm_dpp_itm_vsq_src_port_rjct_get;
    mbcm_dpp_itm_vsq_pg_rjct_set_f                mbcm_dpp_itm_vsq_pg_rjct_set;
    mbcm_dpp_itm_vsq_pg_rjct_get_f                mbcm_dpp_itm_vsq_pg_rjct_get;
    mbcm_dpp_itm_vsq_wred_gen_set_f               mbcm_dpp_itm_vsq_wred_gen_set; 
    mbcm_dpp_itm_vsq_wred_gen_get_f               mbcm_dpp_itm_vsq_wred_gen_get; 
    mbcm_dpp_itm_vsq_wred_set_f                   mbcm_dpp_itm_vsq_wred_set; 
    mbcm_dpp_itm_vsq_wred_get_f                   mbcm_dpp_itm_vsq_wred_get; 
    mbcm_dpp_itm_vsq_counter_set_f                mbcm_dpp_itm_vsq_counter_set; 
    mbcm_dpp_itm_vsq_counter_get_f                mbcm_dpp_itm_vsq_counter_get; 
    mbcm_dpp_itm_vsq_counter_read_f               mbcm_dpp_itm_vsq_counter_read; 
    mbcm_dpp_itm_vsq_pg_tc_profile_mapping_set_f  mbcm_dpp_itm_vsq_pg_tc_profile_mapping_set;
    mbcm_dpp_itm_vsq_pg_tc_profile_mapping_get_f  mbcm_dpp_itm_vsq_pg_tc_profile_mapping_get;
    mbcm_dpp_itm_vsq_pg_tc_profile_set_f          mbcm_dpp_itm_vsq_pg_tc_profile_set;
    mbcm_dpp_itm_vsq_pg_tc_profile_get_f          mbcm_dpp_itm_vsq_pg_tc_profile_get;
    mbcm_dpp_itm_vsq_pg_prm_set_f                 mbcm_dpp_itm_vsq_pg_prm_set;
    mbcm_dpp_itm_vsq_pg_prm_get_f                 mbcm_dpp_itm_vsq_pg_prm_get;
    mbcm_dpp_itm_vsq_pb_prm_set_f                 mbcm_dpp_itm_vsq_pb_prm_set;
    mbcm_dpp_itm_vsq_pb_prm_get_f                 mbcm_dpp_itm_vsq_pb_prm_get;
    mbcm_dpp_itm_src_vsqs_mapping_set_f           mbcm_dpp_itm_src_vsqs_mapping_set;
    mbcm_dpp_itm_src_vsqs_mapping_get_f           mbcm_dpp_itm_src_vsqs_mapping_get;
    mbcm_dpp_itm_vsq_src_port_get_f               mbcm_dpp_itm_vsq_src_port_get;
    mbcm_dpp_itm_vsq_pg_mapping_get_f             mbcm_dpp_itm_vsq_pg_mapping_get;
    mbcm_dpp_itm_vsq_pg_ocb_set_f                 mbcm_dpp_itm_vsq_pg_ocb_set;
    mbcm_dpp_itm_vsq_pg_ocb_get_f                 mbcm_dpp_itm_vsq_pg_ocb_get;
    mbcm_dpp_itm_queue_is_ocb_only_get_f          mbcm_dpp_itm_queue_is_ocb_only_get;
    mbcm_dpp_itm_queue_info_set_f                 mbcm_dpp_itm_queue_info_set; 
    mbcm_dpp_itm_queue_info_get_f                 mbcm_dpp_itm_queue_info_get; 
    mbcm_dpp_itm_ingress_shape_set_f              mbcm_dpp_itm_ingress_shape_set; 
    mbcm_dpp_itm_ingress_shape_get_f              mbcm_dpp_itm_ingress_shape_get; 
    mbcm_dpp_itm_priority_map_tmplt_set_f         mbcm_dpp_itm_priority_map_tmplt_set; 
    mbcm_dpp_itm_priority_map_tmplt_get_f         mbcm_dpp_itm_priority_map_tmplt_get; 
    mbcm_dpp_itm_priority_map_tmplt_select_set_f  mbcm_dpp_itm_priority_map_tmplt_select_set; 
    mbcm_dpp_itm_priority_map_tmplt_select_get_f  mbcm_dpp_itm_priority_map_tmplt_select_get; 
    mbcm_dpp_itm_sys_red_drop_prob_set_f          mbcm_dpp_itm_sys_red_drop_prob_set; 
    mbcm_dpp_itm_sys_red_drop_prob_get_f          mbcm_dpp_itm_sys_red_drop_prob_get; 
    mbcm_dpp_itm_sys_red_queue_size_boundaries_set_f mbcm_dpp_itm_sys_red_queue_size_boundaries_set; 
    mbcm_dpp_itm_sys_red_queue_size_boundaries_get_f mbcm_dpp_itm_sys_red_queue_size_boundaries_get; 
    mbcm_dpp_itm_sys_red_q_based_set_f            mbcm_dpp_itm_sys_red_q_based_set; 
    mbcm_dpp_itm_sys_red_q_based_get_f            mbcm_dpp_itm_sys_red_q_based_get; 
    mbcm_dpp_itm_sys_red_eg_set_f                 mbcm_dpp_itm_sys_red_eg_set; 
    mbcm_dpp_itm_sys_red_eg_get_f                 mbcm_dpp_itm_sys_red_eg_get; 
    mbcm_dpp_itm_sys_red_glob_rcs_set_f           mbcm_dpp_itm_sys_red_glob_rcs_set; 
    mbcm_dpp_itm_sys_red_glob_rcs_get_f           mbcm_dpp_itm_sys_red_glob_rcs_get; 
    mbcm_dpp_itm_vsq_index_global2group_f         mbcm_dpp_itm_vsq_index_global2group; 
    mbcm_dpp_itm_vsq_index_group2global_f         mbcm_dpp_itm_vsq_index_group2global; 
    mbcm_dpp_itm_vsq_src_reserve_set_f            mbcm_dpp_itm_vsq_src_reserve_set;
    mbcm_dpp_itm_vsq_src_reserve_get_f            mbcm_dpp_itm_vsq_src_reserve_get;
    mbcm_dpp_itm_resource_allocation_set_f        mbcm_dpp_itm_resource_allocation_set;
    mbcm_dpp_itm_resource_allocation_get_f        mbcm_dpp_itm_resource_allocation_get;
    mbcm_dpp_itm_global_resource_allocation_set_f mbcm_dpp_itm_global_resource_allocation_set;
    mbcm_dpp_itm_global_resource_allocation_get_f mbcm_dpp_itm_global_resource_allocation_get;
    mbcm_dpp_itm_dyn_total_thresh_set_f           mbcm_dpp_itm_dyn_total_thresh_set; 
    mbcm_dpp_itm_queue_dyn_info_get_f             mbcm_dpp_itm_queue_dyn_info_get;
    mbcm_dpp_itm_per_queue_info_set_f             mbcm_dpp_itm_per_queue_info_set;
    mbcm_dpp_itm_per_queue_info_get_f             mbcm_dpp_itm_per_queue_info_get;
    mbcm_dpp_itm_in_pp_port_scheduler_compensation_profile_set_f  mbcm_dpp_itm_in_pp_port_scheduler_compensation_profile_set;
    mbcm_dpp_itm_in_pp_port_scheduler_compensation_profile_get_f  mbcm_dpp_itm_in_pp_port_scheduler_compensation_profile_get;
    mbcm_dpp_iqm_dynamic_tbl_get_unsafe_f         mbcm_dpp_iqm_dynamic_tbl_get_unsafe;
    mbcm_dpp_itm_congestion_statistics_get_f      mbcm_dpp_itm_congestion_statistics_get;
    mbcm_dpp_itm_min_free_resources_stat_get_f    mbcm_dpp_itm_min_free_resources_stat_get;
    mbcm_dpp_ingress_drop_status_f                mbcm_dpp_ingress_drop_status;
    mbcm_dpp_itm_profile_ocb_only_set_f           mbcm_dpp_itm_profile_ocb_only_set;
    mbcm_dpp_itm_profile_ocb_only_get_f           mbcm_dpp_itm_profile_ocb_only_get;
    mbcm_dpp_itm_credits_adjust_size_set_f        mbcm_dpp_itm_credits_adjust_size_set;
    mbcm_dpp_itm_credits_adjust_size_get_f        mbcm_dpp_itm_credits_adjust_size_get;
    mbcm_dpp_itm_sch_final_delta_map_set_f        mbcm_dpp_itm_sch_final_delta_map_set;
    mbcm_dpp_itm_sch_final_delta_map_get_f        mbcm_dpp_itm_sch_final_delta_map_get;
    mbcm_dpp_itm_sch_final_delta_set_f            mbcm_dpp_itm_sch_final_delta_set;
    mbcm_dpp_itm_sch_final_delta_get_f            mbcm_dpp_itm_sch_final_delta_get;
     
    mbcm_dpp_mult_eg_vlan_membership_group_range_set_f mbcm_dpp_mult_eg_vlan_membership_group_range_set; 
    mbcm_dpp_mult_eg_vlan_membership_group_range_get_f mbcm_dpp_mult_eg_vlan_membership_group_range_get; 
    mbcm_dpp_mult_eg_group_open_f                 mbcm_dpp_mult_eg_group_open;   
    mbcm_dpp_mult_eg_group_update_f               mbcm_dpp_mult_eg_group_update; 
    mbcm_dpp_mult_eg_group_set_f                  mbcm_dpp_mult_eg_group_set;    
    mbcm_dpp_mult_eg_group_close_f                mbcm_dpp_mult_eg_group_close; 
    mbcm_dpp_mult_eg_port_add_f                   mbcm_dpp_mult_eg_port_add; 
    mbcm_dpp_mult_eg_reps_add_f                   mbcm_dpp_mult_eg_reps_add; 
    mbcm_dpp_mult_eg_port_remove_f                mbcm_dpp_mult_eg_port_remove; 
    mbcm_dpp_mult_eg_reps_remove_f                mbcm_dpp_mult_eg_reps_remove; 
    mbcm_dpp_mult_eg_group_size_get_f             mbcm_dpp_mult_eg_group_size_get; 
    mbcm_dpp_mult_eg_group_get_f                  mbcm_dpp_mult_eg_group_get; 
    mbcm_dpp_mult_eg_get_group_f                  mbcm_dpp_mult_eg_get_group; 
    mbcm_dpp_mult_eg_vlan_membership_group_open_f mbcm_dpp_mult_eg_vlan_membership_group_open; 
    mbcm_dpp_mult_eg_bitmap_group_create_f        mbcm_dpp_mult_eg_bitmap_group_create; 
    mbcm_dpp_mult_eg_vlan_membership_group_update_f mbcm_dpp_mult_eg_vlan_membership_group_update; 
    mbcm_dpp_mult_eg_vlan_membership_group_close_f mbcm_dpp_mult_eg_vlan_membership_group_close; 
    mbcm_dpp_mult_eg_vlan_membership_port_add_f   mbcm_dpp_mult_eg_vlan_membership_port_add; 
    mbcm_dpp_mult_eg_vlan_membership_port_remove_f mbcm_dpp_mult_eg_vlan_membership_port_remove; 
    mbcm_dpp_mult_eg_vlan_membership_bm_add_f   mbcm_dpp_mult_eg_vlan_membership_bm_add; 
    mbcm_dpp_mult_eg_vlan_membership_bm_remove_f mbcm_dpp_mult_eg_vlan_membership_bm_remove; 
    mbcm_dpp_mult_eg_vlan_membership_group_get_f  mbcm_dpp_mult_eg_vlan_membership_group_get; 

    dpp_mcds_multicast_init2_f                    mbcm_dpp_mcds_multicast_init2;
     
    mbcm_dpp_mult_fabric_traffic_class_to_multicast_cls_map_set_f mbcm_dpp_mult_fabric_traffic_class_to_multicast_cls_map_set; 
    mbcm_dpp_mult_fabric_traffic_class_to_multicast_cls_map_get_f mbcm_dpp_mult_fabric_traffic_class_to_multicast_cls_map_get; 
    mbcm_dpp_mult_fabric_base_queue_set_f         mbcm_dpp_mult_fabric_base_queue_set; 
    mbcm_dpp_mult_fabric_base_queue_get_f         mbcm_dpp_mult_fabric_base_queue_get; 
    mbcm_dpp_mult_fabric_credit_source_set_f      mbcm_dpp_mult_fabric_credit_source_set; 
    mbcm_dpp_mult_fabric_credit_source_get_f      mbcm_dpp_mult_fabric_credit_source_get; 
    mbcm_dpp_mult_fabric_enhanced_set_f           mbcm_dpp_mult_fabric_enhanced_set; 
    mbcm_dpp_mult_fabric_enhanced_get_f           mbcm_dpp_mult_fabric_enhanced_get; 
    mbcm_dpp_mult_fabric_flow_control_set_f       mbcm_dpp_mult_fabric_flow_control_set;
    mbcm_dpp_mult_fabric_flow_control_get_f       mbcm_dpp_mult_fabric_flow_control_get;
    mbcm_dpp_mult_fabric_active_links_set_f       mbcm_dpp_mult_fabric_active_links_set; 
    mbcm_dpp_mult_fabric_active_links_get_f       mbcm_dpp_mult_fabric_active_links_get; 
    
    mbcm_dpp_mult_does_group_exist_f              mbcm_dpp_mult_does_group_exist; 
     
    mbcm_dpp_mult_ing_traffic_class_map_set_f     mbcm_dpp_mult_ing_traffic_class_map_set; 
    mbcm_dpp_mult_ing_traffic_class_map_get_f     mbcm_dpp_mult_ing_traffic_class_map_get; 
    mbcm_dpp_mult_ing_group_open_f                mbcm_dpp_mult_ing_group_open; 
    mbcm_dpp_mult_ing_group_update_f              mbcm_dpp_mult_ing_group_update; 
    mbcm_dpp_mult_ing_group_close_f               mbcm_dpp_mult_ing_group_close; 
    mbcm_dpp_mult_ing_destination_add_f           mbcm_dpp_mult_ing_destination_add; 
    mbcm_dpp_mult_ing_encode_entry_f              mbcm_dpp_mult_ing_encode_entry;
    mbcm_dpp_mult_ing_destination_remove_f        mbcm_dpp_mult_ing_destination_remove; 
    mbcm_dpp_mult_ing_group_size_get_f            mbcm_dpp_mult_ing_group_size_get; 
    mbcm_dpp_mult_ing_group_get_f                 mbcm_dpp_mult_ing_group_get;  
    mbcm_dpp_mult_ing_get_group_f                 mbcm_dpp_mult_ing_get_group;  
     
    
    
    
     
    mbcm_dpp_pkt_packet_callback_set_f            mbcm_dpp_pkt_packet_callback_set; 
    mbcm_dpp_pkt_packet_callback_get_f            mbcm_dpp_pkt_packet_callback_get; 
    mbcm_dpp_pkt_packet_send_f                    mbcm_dpp_pkt_packet_send; 
    mbcm_dpp_pkt_packet_recv_f                    mbcm_dpp_pkt_packet_recv; 
    mbcm_dpp_pkt_packet_receive_mode_set_f        mbcm_dpp_pkt_packet_receive_mode_set; 
    mbcm_dpp_pkt_packet_receive_mode_get_f        mbcm_dpp_pkt_packet_receive_mode_get; 
     
    mbcm_dpp_sys_phys_to_local_port_map_set_f     mbcm_dpp_sys_phys_to_local_port_map_set; 
    mbcm_dpp_sys_phys_to_local_port_map_get_f     mbcm_dpp_sys_phys_to_local_port_map_get; 
    mbcm_dpp_local_to_sys_phys_port_map_get_f     mbcm_dpp_local_to_sys_phys_port_map_get; 
    mbcm_dpp_modport_to_sys_phys_port_map_get_f   mbcm_dpp_modport_to_sys_phys_port_map_get; 
    mbcm_dpp_port_to_interface_map_set_f          mbcm_dpp_port_to_interface_map_set; 
    mbcm_dpp_port_to_interface_map_get_f          mbcm_dpp_port_to_interface_map_get;
    mbcm_dpp_is_port_lag_member_f                 mbcm_dpp_is_port_lag_member;
    mbcm_dpp_ports_lag_set_f                      mbcm_dpp_ports_lag_set; 
    mbcm_dpp_ports_lag_get_f                      mbcm_dpp_ports_lag_get; 
    mbcm_dpp_ports_lag_sys_port_add_f             mbcm_dpp_ports_lag_sys_port_add; 
    mbcm_dpp_ports_lag_member_add_f               mbcm_dpp_ports_lag_member_add; 
    mbcm_dpp_ports_lag_sys_port_remove_f          mbcm_dpp_ports_lag_sys_port_remove; 
    mbcm_dpp_ports_lag_order_preserve_set_f       mbcm_dpp_ports_lag_order_preserve_set; 
    mbcm_dpp_ports_lag_order_preserve_get_f       mbcm_dpp_ports_lag_order_preserve_get; 
    mbcm_dpp_port_header_type_set_f               mbcm_dpp_port_header_type_set; 
    mbcm_dpp_port_header_type_get_f               mbcm_dpp_port_header_type_get; 
    mbcm_dpp_ports_mirror_inbound_set_f           mbcm_dpp_ports_mirror_inbound_set; 
    mbcm_dpp_ports_mirror_inbound_get_f           mbcm_dpp_ports_mirror_inbound_get; 
    mbcm_dpp_ports_mirror_outbound_set_f          mbcm_dpp_ports_mirror_outbound_set; 
    mbcm_dpp_ports_mirror_outbound_get_f          mbcm_dpp_ports_mirror_outbound_get;
    mbcm_dpp_ports_mirror_to_rcy_port_get_f       mbcm_dpp_ports_mirror_to_rcy_port_get; 
    mbcm_dpp_ports_snoop_set_f                    mbcm_dpp_ports_snoop_set; 
    mbcm_dpp_ports_snoop_get_f                    mbcm_dpp_ports_snoop_get; 
    mbcm_dpp_ports_itmh_extension_set_f           mbcm_dpp_ports_itmh_extension_set; 
    mbcm_dpp_ports_itmh_extension_get_f           mbcm_dpp_ports_itmh_extension_get; 
    mbcm_dpp_ports_shaping_header_set_f           mbcm_dpp_ports_shaping_header_set; 
    mbcm_dpp_ports_shaping_header_get_f           mbcm_dpp_ports_shaping_header_get; 
    mbcm_dpp_ports_forwarding_header_set_f        mbcm_dpp_ports_forwarding_header_set; 
    mbcm_dpp_ports_forwarding_header_get_f        mbcm_dpp_ports_forwarding_header_get; 
    mbcm_dpp_ports_stag_set_f                     mbcm_dpp_ports_stag_set; 
    mbcm_dpp_ports_stag_get_f                     mbcm_dpp_ports_stag_get; 
    mbcm_dpp_ports_ftmh_extension_set_f           mbcm_dpp_ports_ftmh_extension_set; 
    mbcm_dpp_ports_ftmh_extension_get_f           mbcm_dpp_ports_ftmh_extension_get;
    mbcm_dpp_ports_reference_clock_set_f          mbcm_dpp_ports_reference_clock_set;
    mbcm_dpp_ports_port_to_nif_id_get_f           mbcm_dpp_ports_port_to_nif_id_get;
    mbcm_dpp_ports_mirrored_channel_and_context_map_f mbcm_dpp_ports_mirrored_channel_and_context_map;
    mbcm_dpp_port_egr_hdr_credit_discount_type_set_f mbcm_dpp_port_egr_hdr_credit_discount_type_set; 
    mbcm_dpp_port_egr_hdr_credit_discount_type_get_f mbcm_dpp_port_egr_hdr_credit_discount_type_get; 
    mbcm_dpp_port_egr_hdr_credit_discount_select_set_f mbcm_dpp_port_egr_hdr_credit_discount_select_set; 
    mbcm_dpp_port_egr_hdr_credit_discount_select_get_f mbcm_dpp_port_egr_hdr_credit_discount_select_get; 
    mbcm_dpp_port_stacking_info_set_f             mbcm_dpp_port_stacking_info_set;
    mbcm_dpp_port_stacking_info_get_f             mbcm_dpp_port_stacking_info_get;
    mbcm_dpp_port_stacking_route_history_bitmap_set_f mbcm_dpp_port_stacking_route_history_bitmap_set;
    mbcm_dpp_port_direct_lb_key_set_f             mbcm_dpp_port_direct_lb_key_set;
    mbcm_dpp_port_direct_lb_key_get_f             mbcm_dpp_port_direct_lb_key_get;
    mbcm_dpp_port_direct_lb_key_min_set_f         mbcm_dpp_port_direct_lb_key_min_set;
    mbcm_dpp_port_direct_lb_key_max_set_f         mbcm_dpp_port_direct_lb_key_max_set;
    mbcm_dpp_port_direct_lb_key_min_get_f         mbcm_dpp_port_direct_lb_key_min_get;
    mbcm_dpp_port_direct_lb_key_max_get_f         mbcm_dpp_port_direct_lb_key_max_get;
    mbcm_dpp_port_synchronize_lb_key_tables_at_egress_f mbcm_dpp_port_synchronize_lb_key_tables_at_egress;
    mbcm_dpp_port_switch_lb_key_tables_f          mbcm_dpp_port_switch_lb_key_tables;
    mbcm_dpp_port_rx_enable_get_f                 mbcm_dpp_port_rx_enable_get;
    mbcm_dpp_port_rx_enable_set_f                 mbcm_dpp_port_rx_enable_set;
    mbcm_dpp_port_tx_enable_get_f                 mbcm_dpp_port_tx_enable_get;
    mbcm_dpp_port_tx_enable_set_f                 mbcm_dpp_port_tx_enable_set;
    mbcm_dpp_port_ingr_reassembly_context_get_f   mbcm_dpp_port_ingr_reassembly_context_get;
    mbcm_dpp_port_rate_egress_pps_set_f           mbcm_dpp_port_rate_egress_pps_set;
    mbcm_dpp_port_rate_egress_pps_get_f           mbcm_dpp_port_rate_egress_pps_get;
    mbcm_dpp_port_protocol_offset_verify_f        mbcm_dpp_port_protocol_offset_verify;        
    mbcm_dpp_port_cable_diag_f                    mbcm_dpp_port_cable_diag;

    
    mbcm_dpp_ports_swap_set_f                 mbcm_dpp_ports_swap_set;
    mbcm_dpp_ports_swap_get_f                 mbcm_dpp_ports_swap_get;  

    
    mbcm_dpp_ports_pon_tunnel_info_set_f      mbcm_dpp_ports_pon_tunnel_info_set;
    mbcm_dpp_ports_pon_tunnel_info_get_f      mbcm_dpp_ports_pon_tunnel_info_get;  

    
    mbcm_dpp_ports_extender_mapping_enable_set_f  mbcm_dpp_ports_extender_mapping_enable_set;
    mbcm_dpp_ports_extender_mapping_enable_get_f  mbcm_dpp_ports_extender_mapping_enable_get;

    mbcm_dpp_ports_extender_mapping_enable_set_f  mbcm_dpp_ports_tm_port_var_set;
    mbcm_dpp_ports_extender_mapping_enable_get_f  mbcm_dpp_ports_tm_port_var_get;

     
    mbcm_dpp_read_fld_f                           mbcm_dpp_read_fld; 
    mbcm_dpp_write_fld_f                          mbcm_dpp_write_fld; 
    mbcm_dpp_read_reg_f                           mbcm_dpp_read_reg; 
    mbcm_dpp_write_reg_f                          mbcm_dpp_write_reg; 
    mbcm_dpp_status_fld_poll_f                    mbcm_dpp_status_fld_poll;
    mbcm_dpp_brdc_fsrd_blk_id_set_f              mbcm_dpp_brdc_fsrd_blk_id_set;
     
    mbcm_dpp_mgmt_credit_worth_set_f              mbcm_dpp_mgmt_credit_worth_set; 
    mbcm_dpp_mgmt_credit_worth_get_f              mbcm_dpp_mgmt_credit_worth_get; 
    mbcm_dpp_mgmt_module_to_credit_worth_map_set_f mbcm_dpp_mgmt_module_to_credit_worth_map_set;
    mbcm_dpp_mgmt_module_to_credit_worth_map_get_f mbcm_dpp_mgmt_module_to_credit_worth_map_get;
    mbcm_dpp_mgmt_credit_worth_remote_set_f     mbcm_dpp_mgmt_credit_worth_remote_set;
    mbcm_dpp_mgmt_credit_worth_remote_get_f     mbcm_dpp_mgmt_credit_worth_remote_get;
    mbcm_dpp_mgmt_change_all_faps_credit_worth_unsafe_f     mbcm_dpp_mgmt_change_all_faps_credit_worth_unsafe;
    mbcm_dpp_mgmt_all_ctrl_cells_enable_get_f     mbcm_dpp_mgmt_all_ctrl_cells_enable_get; 
    mbcm_dpp_mgmt_all_ctrl_cells_enable_set_f     mbcm_dpp_mgmt_all_ctrl_cells_enable_set; 
    mbcm_dpp_force_tdm_bypass_traffic_to_fabric_set_f mbcm_dpp_force_tdm_bypass_traffic_to_fabric_set;
    mbcm_dpp_force_tdm_bypass_traffic_to_fabric_get_f mbcm_dpp_force_tdm_bypass_traffic_to_fabric_get;
    mbcm_dpp_mgmt_enable_traffic_set_f            mbcm_dpp_mgmt_enable_traffic_set; 
    mbcm_dpp_mgmt_enable_traffic_get_f            mbcm_dpp_mgmt_enable_traffic_get; 
    mbcm_dpp_register_device_f                    mbcm_dpp_register_device; 
    mbcm_dpp_unregister_device_f                  mbcm_dpp_unregister_device; 
    mbcm_dpp_mgmt_system_fap_id_set_f             mbcm_dpp_mgmt_system_fap_id_set; 
    mbcm_dpp_mgmt_system_fap_id_get_f             mbcm_dpp_mgmt_system_fap_id_get;
    mbcm_dpp_mgmt_tm_domain_set_f                 mbcm_dpp_mgmt_tm_domain_set; 
    mbcm_dpp_mgmt_tm_domain_get_f                 mbcm_dpp_mgmt_tm_domain_get;
    mbcm_dpp_mgmt_dma_fifo_channel_free_find_f    mbcm_dpp_mgmt_dma_fifo_channel_free_find;
    mbcm_dpp_mgmt_dma_fifo_channel_set_f          mbcm_dpp_mgmt_dma_fifo_channel_set;
    mbcm_dpp_mgmt_dma_fifo_channel_get_f          mbcm_dpp_mgmt_dma_fifo_channel_get;     
    
     
    mbcm_dpp_hpu_itmh_build_verify_f              mbcm_dpp_hpu_itmh_build_verify; 
    mbcm_dpp_hpu_ftmh_build_verify_f              mbcm_dpp_hpu_ftmh_build_verify; 
    mbcm_dpp_hpu_otmh_build_verify_f              mbcm_dpp_hpu_otmh_build_verify; 
    mbcm_dpp_hpu_itmh_build_f                     mbcm_dpp_hpu_itmh_build; 
    mbcm_dpp_hpu_itmh_parse_f                     mbcm_dpp_hpu_itmh_parse; 
    mbcm_dpp_hpu_ftmh_build_f                     mbcm_dpp_hpu_ftmh_build; 
    mbcm_dpp_hpu_ftmh_parse_f                     mbcm_dpp_hpu_ftmh_parse; 
    mbcm_dpp_hpu_otmh_build_f                     mbcm_dpp_hpu_otmh_build; 
    mbcm_dpp_hpu_otmh_parse_f                     mbcm_dpp_hpu_otmh_parse; 
     
    mbcm_dpp_loopback_set_f                       mbcm_dpp_loopback_set; 
    mbcm_dpp_loopback_get_f                       mbcm_dpp_loopback_get; 
    mbcm_dpp_nif_synce_clk_sel_port_set_f         mbcm_dpp_nif_synce_clk_sel_port_set;
    mbcm_dpp_nif_synce_clk_sel_port_get_f         mbcm_dpp_nif_synce_clk_sel_port_get;
    mbcm_dpp_nif_synce_clk_div_set_f              mbcm_dpp_nif_synce_clk_div_set;
    mbcm_dpp_nif_synce_clk_div_get_f              mbcm_dpp_nif_synce_clk_div_get;
    mbcm_dpp_nif_synce_clk_squelch_set_f          mbcm_dpp_nif_synce_clk_squelch_set;
    mbcm_dpp_nif_synce_clk_squelch_get_f          mbcm_dpp_nif_synce_clk_squelch_get;
    mbcm_dpp_port_link_up_mac_update_f            mbcm_dpp_port_link_up_mac_update;
    mbcm_dpp_flow_and_up_info_get_f               mbcm_dpp_flow_and_up_info_get;
    mbcm_dpp_ips_non_empty_queues_info_get_f      mbcm_dpp_ips_non_empty_queues_info_get;
    mbcm_dpp_itm_pfc_tc_map_set_f                 mbcm_dpp_itm_pfc_tc_map_set;
    mbcm_dpp_itm_pfc_tc_map_get_f                 mbcm_dpp_itm_pfc_tc_map_get;
    mbcm_dpp_fc_gen_cal_set_f                     mbcm_dpp_fc_gen_cal_set;
    mbcm_dpp_fc_gen_cal_get_f                     mbcm_dpp_fc_gen_cal_get;
    mbcm_dpp_fc_gen_inbnd_set_f                   mbcm_dpp_fc_gen_inbnd_set;
    mbcm_dpp_fc_gen_inbnd_get_f                   mbcm_dpp_fc_gen_inbnd_get;
    mbcm_dpp_fc_gen_inbnd_glb_hp_set_f            mbcm_dpp_fc_gen_inbnd_glb_hp_set;
    mbcm_dpp_fc_gen_inbnd_glb_hp_get_f            mbcm_dpp_fc_gen_inbnd_glb_hp_get;
    mbcm_dpp_fc_rec_cal_set_f                     mbcm_dpp_fc_rec_cal_set;
    mbcm_dpp_fc_rec_cal_get_f                     mbcm_dpp_fc_rec_cal_get;
    mbcm_dpp_fc_pfc_generic_bitmap_set_f          mbcm_dpp_fc_pfc_generic_bitmap_set;
    mbcm_dpp_fc_pfc_generic_bitmap_get_f          mbcm_dpp_fc_pfc_generic_bitmap_get;
    mbcm_dpp_fc_port_fifo_threshold_set_f         mbcm_dpp_fc_port_fifo_threshold_set;
    mbcm_dpp_fc_port_fifo_threshold_get_f         mbcm_dpp_fc_port_fifo_threshold_get;
    mbcm_dpp_egr_dsp_pp_to_base_q_pair_get_f      mbcm_dpp_egr_dsp_pp_to_base_q_pair_get;
    mbcm_dpp_egr_dsp_pp_to_base_q_pair_set_f      mbcm_dpp_egr_dsp_pp_to_base_q_pair_set;
    mbcm_dpp_egr_dsp_pp_priorities_mode_get_f     mbcm_dpp_egr_dsp_pp_priorities_mode_get;
    mbcm_dpp_egr_dsp_pp_priorities_mode_set_f     mbcm_dpp_egr_dsp_pp_priorities_mode_set;
    mbcm_dpp_egr_dsp_pp_shaper_mode_set_f         mbcm_dpp_egr_dsp_pp_shaper_mode_set;
    mbcm_dpp_egr_queuing_dev_set_f                mbcm_dpp_egr_queuing_dev_set;
    mbcm_dpp_egr_queuing_dev_get_f                mbcm_dpp_egr_queuing_dev_get;
    mbcm_dpp_egr_queuing_global_drop_set_f        mbcm_dpp_egr_queuing_global_drop_set;
    mbcm_dpp_egr_queuing_global_drop_get_f        mbcm_dpp_egr_queuing_global_drop_get;
    mbcm_dpp_egr_queuing_sp_tc_drop_set_f         mbcm_dpp_egr_queuing_sp_tc_drop_set;
    mbcm_dpp_egr_queuing_sp_tc_drop_get_f         mbcm_dpp_egr_queuing_sp_tc_drop_get; 
    mbcm_dpp_egr_queuing_sch_unsch_drop_set_f     mbcm_dpp_egr_queuing_sch_unsch_drop_set;
    mbcm_dpp_egr_queuing_sch_unsch_drop_get_unsafe_f     mbcm_dpp_egr_queuing_sch_unsch_drop_get_unsafe;
    mbcm_dpp_egr_queuing_sp_reserved_set_f        mbcm_dpp_egr_queuing_sp_reserved_set;
    mbcm_dpp_egr_queuing_sp_reserved_get_f        mbcm_dpp_egr_queuing_sp_reserved_get;
    mbcm_dpp_egr_queuing_global_fc_set_f          mbcm_dpp_egr_queuing_global_fc_set;
    mbcm_dpp_egr_queuing_global_fc_get_f          mbcm_dpp_egr_queuing_global_fc_get;
    mbcm_dpp_egr_queuing_mc_tc_fc_set_f           mbcm_dpp_egr_queuing_mc_tc_fc_set;
    mbcm_dpp_egr_queuing_mc_tc_fc_get_f           mbcm_dpp_egr_queuing_mc_tc_fc_get;
    mbcm_dpp_egr_queuing_mc_cos_map_set_f         mbcm_dpp_egr_queuing_mc_cos_map_set;
    mbcm_dpp_egr_queuing_mc_cos_map_get_f         mbcm_dpp_egr_queuing_mc_cos_map_get;
    mbcm_dpp_egr_queuing_if_fc_set_f              mbcm_dpp_egr_queuing_if_fc_set;
    mbcm_dpp_egr_queuing_if_fc_get_f              mbcm_dpp_egr_queuing_if_fc_get;
    mbcm_dpp_egr_queuing_if_fc_uc_max_set_f       mbcm_dpp_egr_queuing_if_fc_uc_max_set;
    mbcm_dpp_egr_queuing_if_fc_uc_set_f           mbcm_dpp_egr_queuing_if_fc_uc_set;
    mbcm_dpp_egr_queuing_if_fc_uc_get_f           mbcm_dpp_egr_queuing_if_fc_uc_get;
    mbcm_dpp_egr_queuing_if_fc_mc_set_f           mbcm_dpp_egr_queuing_if_fc_mc_set;
    mbcm_dpp_egr_queuing_if_fc_mc_get_f           mbcm_dpp_egr_queuing_if_fc_mc_get;
    mbcm_dpp_egr_queuing_if_uc_map_set_f          mbcm_dpp_egr_queuing_if_uc_map_set;
    mbcm_dpp_egr_queuing_if_mc_map_set_f          mbcm_dpp_egr_queuing_if_mc_map_set;
    
    mbcm_dpp_egr_queuing_ofp_tcg_set_f            mbcm_dpp_egr_queuing_ofp_tcg_set;
    mbcm_dpp_egr_queuing_ofp_tcg_get_f            mbcm_dpp_egr_queuing_ofp_tcg_get;
    mbcm_dpp_egr_queuing_tcg_weight_set_f         mbcm_dpp_egr_queuing_tcg_weight_set;
    mbcm_dpp_egr_queuing_tcg_weight_get_f         mbcm_dpp_egr_queuing_tcg_weight_get;
    mbcm_dpp_egr_queuing_is_high_priority_port_get_f         mbcm_dpp_egr_queuing_is_high_priority_port_get;
    mbcm_dpp_egr_queuing_init_thresholds_f        mbcm_dpp_egr_queuing_init_thresholds;
    mbcm_dpp_egr_queuing_egr_interface_alloc_f    mbcm_dpp_egr_queuing_egr_interface_alloc;
    mbcm_dpp_egr_queuing_egr_interface_free_f     mbcm_dpp_egr_queuing_egr_interface_free;
    mbcm_dpp_egr_queuing_nrdy_th_profile_data_set_f       mbcm_dpp_egr_queuing_nrdy_th_profile_data_set;
    mbcm_dpp_egr_queuing_nrdy_th_profile_data_get_f       mbcm_dpp_egr_queuing_nrdy_th_profile_data_get;
    mbcm_dpp_sch_port_tcg_weight_set_f            mbcm_dpp_sch_port_tcg_weight_set;
    mbcm_dpp_sch_port_tcg_weight_get_f            mbcm_dpp_sch_port_tcg_weight_get;
    mbcm_dpp_mgmt_max_pckt_size_set_f             mbcm_dpp_mgmt_max_pckt_size_set;
    mbcm_dpp_mgmt_max_pckt_size_get_f             mbcm_dpp_mgmt_max_pckt_size_get;
    mbcm_dpp_mgmt_ocb_voq_eligible_dynamic_set_f  mbcm_dpp_mgmt_ocb_voq_eligible_dynamic_set;
    mbcm_dpp_mgmt_voq_is_ocb_eligible_get_f       mbcm_dpp_mgmt_voq_is_ocb_eligible_get;
    mbcm_dpp_mult_cud_to_port_map_set_f           mbcm_dpp_mult_cud_to_port_map_set;
    mbcm_dpp_mult_cud_to_port_map_get_f           mbcm_dpp_mult_cud_to_port_map_get;

    
    mbcm_dpp_nof_interrupts_f                     mbcm_dpp_nof_interrupts;
    mbcm_dpp_nof_block_instances_f                mbcm_dpp_nof_block_instance;
    mbcm_dpp_temp_pvt_get_f                       mbcm_dpp_temp_pvt_get;
    mbcm_dpp_avs_value_get_f                      mbcm_dpp_avs_value_get;
    mbcm_dpp_itm_dp_discard_set_f                 mbcm_dpp_itm_dp_discard_set;
    mbcm_dpp_itm_dp_discard_get_f                 mbcm_dpp_itm_dp_discard_get;
    mbcm_dpp_itm_alpha_set_f                      mbcm_dpp_itm_alpha_set;
    mbcm_dpp_itm_alpha_get_f                      mbcm_dpp_itm_alpha_get;
    mbcm_dpp_itm_fair_adaptive_tail_drop_enable_set_f mbcm_dpp_itm_fair_adaptive_tail_drop_enable_set;
    mbcm_dpp_itm_fair_adaptive_tail_drop_enable_get_f mbcm_dpp_itm_fair_adaptive_tail_drop_enable_get;
    mbcm_dpp_ports_application_mapping_info_set_f mbcm_dpp_ports_application_mapping_info_set;
    mbcm_dpp_ports_application_mapping_info_get_f mbcm_dpp_ports_application_mapping_info_get;

    mbcm_dpp_ofp_rates_max_credit_empty_port_set_f       mbcm_dpp_ofp_rates_max_credit_empty_port_set;
    mbcm_dpp_ofp_rates_max_credit_empty_port_get_f       mbcm_dpp_ofp_rates_max_credit_empty_port_get;

    mbcm_dpp_drv_allocate_rcy_port_f                             mbcm_dpp_drv_allocate_rcy_port;
    mbcm_dpp_drv_free_tm_port_and_recycle_channel_f              mbcm_dpp_drv_free_tm_port_and_recycle_channel;
    mbcm_dpp_drv_info_config_device_ports_f                      mbcm_dpp_drv_info_config_device_ports;
    mbcm_dpp_drv_is_olp_f                                        mbcm_dpp_drv_is_olp;
    mbcm_dpp_drv_is_oamp_f                                       mbcm_dpp_drv_is_oamp;
    mbcm_dpp_drv_validate_fabric_mode_f                          mbcm_dpp_drv_validate_fabric_mode;
    mbcm_dpp_drv_prop_fap_device_mode_get_f                      mbcm_dpp_drv_prop_fap_device_mode_get;
    mbcm_dpp_drv_device_deinit_f                                 mbcm_dpp_drv_device_deinit;
    mbcm_dpp_drv_device_attach_f                                 mbcm_dpp_drv_device_attach;
    mbcm_dpp_drv_fc_oob_mode_validate_f                          mbcm_dpp_drv_fc_oob_mode_validate;
    mbcm_dpp_port_to_interface_egress_map_set_f                  mbcm_dpp_port_to_interface_egress_map_set;
    mbcm_dpp_ofp_rates_port_priority_max_burst_for_fc_queues_set_f      mbcm_dpp_ofp_rates_port_priority_max_burst_for_fc_queues_set;   
    mbcm_dpp_ofp_rates_port_priority_max_burst_for_empty_queues_set_f   mbcm_dpp_ofp_rates_port_priority_max_burst_for_empty_queues_set;
    mbcm_dpp_ofp_rates_port_priority_max_burst_for_fc_queues_get_f      mbcm_dpp_ofp_rates_port_priority_max_burst_for_fc_queues_get;  
    mbcm_dpp_ofp_rates_port_priority_max_burst_for_empty_queues_get_f   mbcm_dpp_ofp_rates_port_priority_max_burst_for_empty_queues_get; 
    mbcm_dpp_ofp_rates_sch_single_port_rate_sw_set_f                    mbcm_dpp_ofp_rates_sch_single_port_rate_sw_set;               
    mbcm_dpp_ofp_rates_sch_single_port_rate_hw_set_f                    mbcm_dpp_ofp_rates_sch_single_port_rate_hw_set;               
    mbcm_dpp_ofp_rates_egq_single_port_rate_sw_set_f                    mbcm_dpp_ofp_rates_egq_single_port_rate_sw_set;
    mbcm_dpp_ofp_rates_egq_single_port_rate_sw_get_f                    mbcm_dpp_ofp_rates_egq_single_port_rate_sw_get;                    
    mbcm_dpp_ofp_rates_egq_single_port_rate_hw_set_f                    mbcm_dpp_ofp_rates_egq_single_port_rate_hw_set;               
    mbcm_dpp_ofp_rates_sch_single_port_rate_hw_get_f                    mbcm_dpp_ofp_rates_sch_single_port_rate_hw_get;                     
    mbcm_dpp_ofp_rates_egq_single_port_rate_hw_get_f                    mbcm_dpp_ofp_rates_egq_single_port_rate_hw_get;                     
    mbcm_dpp_ofp_rates_single_port_max_burst_set_f                      mbcm_dpp_ofp_rates_single_port_max_burst_set;                    
    mbcm_dpp_ofp_rates_single_port_max_burst_get_f                      mbcm_dpp_ofp_rates_single_port_max_burst_get;                                       
    mbcm_dpp_ofp_rates_egq_interface_shaper_set_f                       mbcm_dpp_ofp_rates_egq_interface_shaper_set;                                       
    mbcm_dpp_ofp_rates_egq_interface_shaper_get_f                       mbcm_dpp_ofp_rates_egq_interface_shaper_get;                     
    mbcm_dpp_ofp_rates_egq_tcg_rate_sw_set_f                            mbcm_dpp_ofp_rates_egq_tcg_rate_sw_set;                       
    mbcm_dpp_ofp_rates_egq_tcg_rate_hw_set_f                            mbcm_dpp_ofp_rates_egq_tcg_rate_hw_set;                       
    mbcm_dpp_ofp_rates_sch_tcg_rate_set_f                               mbcm_dpp_ofp_rates_sch_tcg_rate_set;                             
    mbcm_dpp_ofp_rates_egq_tcg_rate_hw_get_f                            mbcm_dpp_ofp_rates_egq_tcg_rate_hw_get;                             
    mbcm_dpp_ofp_rates_sch_tcg_rate_get_f                               mbcm_dpp_ofp_rates_sch_tcg_rate_get; 
    mbcm_dpp_ofp_rates_sch_tcg_shaper_disable_f                         mbcm_dpp_ofp_rates_sch_tcg_shaper_disable;
    mbcm_dpp_ofp_rates_egq_tcg_max_burst_set_f                          mbcm_dpp_ofp_rates_egq_tcg_max_burst_set;                        
    mbcm_dpp_ofp_rates_sch_tcg_max_burst_set_f                          mbcm_dpp_ofp_rates_sch_tcg_max_burst_set;                        
    mbcm_dpp_ofp_rates_egq_tcg_max_burst_get_f                          mbcm_dpp_ofp_rates_egq_tcg_max_burst_get;                        
    mbcm_dpp_ofp_rates_sch_tcg_max_burst_get_f                          mbcm_dpp_ofp_rates_sch_tcg_max_burst_get;                        
    mbcm_dpp_ofp_rates_egq_port_priority_rate_sw_set_f                  mbcm_dpp_ofp_rates_egq_port_priority_rate_sw_set;             
    mbcm_dpp_ofp_rates_egq_port_priority_rate_hw_set_f                  mbcm_dpp_ofp_rates_egq_port_priority_rate_hw_set;             
    mbcm_dpp_ofp_rates_sch_port_priority_rate_set_f                     mbcm_dpp_ofp_rates_sch_port_priority_rate_set;                   
    mbcm_dpp_ofp_rates_sch_port_priority_rate_sw_set_f                  mbcm_dpp_ofp_rates_sch_port_priority_rate_sw_set;                   
    mbcm_dpp_ofp_rates_sch_port_priority_hw_set_f                       mbcm_dpp_ofp_rates_sch_port_priority_hw_set;                   
    mbcm_dpp_ofp_rates_egq_port_priority_rate_hw_get_f                  mbcm_dpp_ofp_rates_egq_port_priority_rate_hw_get;                   
    mbcm_dpp_ofp_rates_sch_port_priority_rate_get_f                     mbcm_dpp_ofp_rates_sch_port_priority_rate_get;                   
    mbcm_dpp_ofp_rates_egq_port_priority_max_burst_set_f                mbcm_dpp_ofp_rates_egq_port_priority_max_burst_set;              
    mbcm_dpp_ofp_rates_sch_port_priority_max_burst_set_f                mbcm_dpp_ofp_rates_sch_port_priority_max_burst_set;              
    mbcm_dpp_ofp_rates_egq_port_priority_max_burst_get_f                mbcm_dpp_ofp_rates_egq_port_priority_max_burst_get;              
    mbcm_dpp_ofp_rates_sch_port_priority_max_burst_get_f                mbcm_dpp_ofp_rates_sch_port_priority_max_burst_get;
    mbcm_dpp_ofp_rates_port2chan_cal_get_f                              mbcm_dpp_ofp_rates_port2chan_cal_get;
    mbcm_dpp_ofp_rates_retrieve_egress_shaper_reg_field_names_f         mbcm_dpp_ofp_rates_retrieve_egress_shaper_reg_field_names;
    mbcm_dpp_ofp_rates_egress_shaper_reg_field_read_f                   mbcm_dpp_ofp_rates_egress_shaper_reg_field_read;
    mbcm_dpp_ofp_rates_egress_shaper_reg_field_write_f                  mbcm_dpp_ofp_rates_egress_shaper_reg_field_write;
    mbcm_dpp_ofp_rates_egq_scm_chan_arb_id2scm_id_f                     mbcm_dpp_ofp_rates_egq_scm_chan_arb_id2scm_id;
    mbcm_dpp_ofp_rates_interface_internal_rate_get_f                    mbcm_dpp_ofp_rates_interface_internal_rate_get;
    mbcm_dpp_ofp_rates_packet_mode_packet_size_get_f                    mbcm_dpp_ofp_rates_packet_mode_packet_size_get;
    mbcm_dpp_ofp_rates_egress_shaper_cal_write_f                        mbcm_dpp_ofp_rates_egress_shaper_cal_write;
    mbcm_dpp_ofp_rates_egress_shaper_cal_read_f                         mbcm_dpp_ofp_rates_egress_shaper_cal_read;
    mbcm_dpp_port2egress_offset_f                                       mbcm_dpp_port2egress_offset;        
    mbcm_dpp_egr_is_channelized_f                                       mbcm_dpp_egr_is_channelized;  
    mbcm_dpp_sch_cal_tbl_set_f                                          mbcm_dpp_sch_cal_tbl_set;
    mbcm_dpp_sch_cal_tbl_get_f                                          mbcm_dpp_sch_cal_tbl_get;
    mbcm_dpp_sch_cal_max_size_get_f                                     mbcm_dpp_sch_cal_max_size_get;
    mbcm_dpp_local_to_tm_port_get_f                                     mbcm_dpp_local_to_tm_port_get;
    mbcm_dpp_local_to_pp_port_get_f                                     mbcm_dpp_local_to_pp_port_get;
    mbcm_dpp_tm_to_local_port_get_f                                     mbcm_dpp_tm_to_local_port_get;
    mbcm_dpp_pp_to_local_port_get_f                                     mbcm_dpp_pp_to_local_port_get;
    mbcm_port_init_f                                                    mbcm_dpp_port_init;
    mbcm_port_post_init_f                                               mbcm_dpp_port_post_init;
    mbcm_port_deinit_f                                                  mbcm_dpp_port_deinit;
    mbcm_port_enable_set_f                                              mbcm_dpp_port_enable_set;
    mbcm_port_enable_get_f                                              mbcm_dpp_port_enable_get;
    mbcm_port_speed_set_f                                               mbcm_dpp_port_speed_set;
    mbcm_port_speed_get_f                                               mbcm_dpp_port_speed_get;
    mbcm_port_interface_set_f                                           mbcm_dpp_port_interface_set;
    mbcm_port_interface_get_f                                           mbcm_dpp_port_interface_get;
    mbcm_port_link_state_get_f                                          mbcm_dpp_port_link_state_get;
    mbcm_is_supported_encap_get_f                                       mbcm_dpp_is_supported_encap_get;                                                          
    mbcm_egr_q_nif_cal_set_f                                            mbcm_dpp_egr_q_nif_cal_set;
    mbcm_dpp_egr_q_fast_port_set_f                                      mbcm_dpp_egr_q_fast_port_set;
    mbcm_dpp_parser_nof_bytes_to_remove_set_f                           mbcm_dpp_parser_nof_bytes_to_remove_set;
    mbcm_dpp_ps_db_find_free_binding_ps_f                               mbcm_dpp_ps_db_find_free_binding_ps;
    mbcm_dpp_ps_db_release_binding_ps_f                                 mbcm_dpp_ps_db_release_binding_ps;
    mbcm_dpp_ps_db_alloc_binding_ps_with_id_f                           mbcm_dpp_ps_db_alloc_binding_ps_with_id;
    mbcm_dpp_egr_prog_editor_profile_set_f                              mbcm_dpp_egr_prog_editor_profile_set;
    mbcm_dpp_egr_prog_editor_prog_sel_value1_set_f                      mbcm_dpp_egr_prog_editor_prog_sel_value1_set;
    mbcm_fc_pfc_mapping_set_f                                           mbcm_dpp_fc_pfc_mapping_set;
    mbcm_fc_pfc_mapping_get_f                                           mbcm_dpp_fc_pfc_mapping_get;
    mbcm_dpp_port_probe_f                                               mbcm_dpp_port_probe;
    mbcm_dpp_port_detach_f                                              mbcm_dpp_port_detach;
    mbcm_dpp_port_fabric_detach_f                                       mbcm_dpp_port_fabric_detach;
    mbcm_dpp_port_is_pcs_loopback_f                                     mbcm_dpp_port_is_pcs_loopback;
    mbcm_dpp_qsgmii_offsets_add_f                                       mbcm_dpp_qsgmii_offsets_add;
    mbcm_dpp_qsgmii_offsets_remove_f                                    mbcm_dpp_qsgmii_offsets_remove;
    mbcm_dpp_qsgmii_offsets_add_pbmp_f                                  mbcm_dpp_qsgmii_offsets_add_pbmp;
    mbcm_dpp_qsgmii_offsets_remove_pbmp_f                               mbcm_dpp_qsgmii_offsets_remove_pbmp;
    mbcm_dpp_port_sch_config_f                                          mbcm_dpp_port_sch_config;
    mbcm_dpp_port_open_fab_o_nif_path_f                                 mbcm_dpp_port_open_fab_o_nif_path;
    mbcm_dpp_port_open_ilkn_path_f                                      mbcm_dpp_port_open_ilkn_path;
    mbcm_dpp_nif_sif_set_f                                              mbcm_dpp_nif_sif_set; 
    mbcm_dpp_port_close_ilkn_path_f                                     mbcm_dpp_port_close_ilkn_path;
    mbcm_dpp_port_ilkn_init_f                                            mbcm_dpp_port_ilkn_init;
    mbcm_dpp_port_nif_ilkn_pbmp_get_f                                    mbcm_dpp_port_nif_ilkn_pbmp_get;
    mbcm_dpp_port_nif_ilkn_phys_aligned_pbmp_get_f                      mbcm_dpp_port_nif_ilkn_phys_aligned_pbmp_get;
    mbcm_dpp_port_nif_qsgmii_pbmp_get_f                                 mbcm_dpp_port_nif_qsgmii_pbmp_get;
    mbcm_dpp_port_ilkn_nif_port_get_f                                    mbcm_dpp_port_ilkn_nif_port_get;
    mbcm_dpp_port_ilkn_nof_segments_set_f                               mbcm_dpp_port_ilkn_nof_segments_set;
    mbcm_port_ilkn_bypass_interface_enable_f                            mbcm_port_ilkn_bypass_interface_enable;
    mbcm_port_fabric_o_nif_bypass_interface_enable_f                    mbcm_port_fabric_o_nif_bypass_interface_enable;
    mbcm_dpp_port_prd_enable_set_f                                      mbcm_dpp_port_prd_enable_set;
    mbcm_dpp_port_prd_enable_get_f                                      mbcm_dpp_port_prd_enable_get;
    mbcm_dpp_port_prd_config_set_f                                      mbcm_dpp_port_prd_config_set;
    mbcm_dpp_port_prd_config_get_f                                      mbcm_dpp_port_prd_config_get;
    mbcm_dpp_port_prd_threshold_set_f                                   mbcm_dpp_port_prd_threshold_set;
    mbcm_dpp_port_prd_threshold_get_f                                   mbcm_dpp_port_prd_threshold_get;
    mbcm_dpp_port_prd_map_set_f                                         mbcm_dpp_port_prd_map_set;
    mbcm_dpp_port_prd_map_get_f                                         mbcm_dpp_port_prd_map_get;
    mbcm_dpp_port_prd_drop_count_get_f                                  mbcm_dpp_port_prd_drop_count_get;
    mbcm_dpp_port_prd_tpid_set_f                                        mbcm_dpp_port_prd_tpid_set;
    mbcm_dpp_port_prd_tpid_get_f                                        mbcm_dpp_port_prd_tpid_get;
    mbcm_dpp_port_speed_sku_restrictions_f                              mbcm_dpp_port_speed_sku_restrictions;
    mbcm_dpp_port_prd_ignore_ip_dscp_set_f                              mbcm_dpp_port_prd_ignore_ip_dscp_set;
    mbcm_dpp_port_prd_ignore_ip_dscp_get_f                              mbcm_dpp_port_prd_ignore_ip_dscp_get;
    mbcm_dpp_port_prd_ignore_mpls_exp_set_f                             mbcm_dpp_port_prd_ignore_mpls_exp_set;
    mbcm_dpp_port_prd_ignore_mpls_exp_get_f                             mbcm_dpp_port_prd_ignore_mpls_exp_get;
    mbcm_dpp_port_prd_ignore_inner_tag_set_f                            mbcm_dpp_port_prd_ignore_inner_tag_set;
    mbcm_dpp_port_prd_ignore_inner_tag_get_f                            mbcm_dpp_port_prd_ignore_inner_tag_get;
    mbcm_dpp_port_prd_ignore_outer_tag_set_f                            mbcm_dpp_port_prd_ignore_outer_tag_set;
    mbcm_dpp_port_prd_ignore_outer_tag_get_f                            mbcm_dpp_port_prd_ignore_outer_tag_get;
    mbcm_dpp_port_prd_default_priority_set_f                            mbcm_dpp_port_prd_default_priority_set;
    mbcm_dpp_port_prd_default_priority_get_f                            mbcm_dpp_port_prd_default_priority_get;
    mbcm_dpp_port_prd_custom_ether_type_set_f                           mbcm_dpp_port_prd_custom_ether_type_set;
    mbcm_dpp_port_prd_custom_ether_type_get_f                           mbcm_dpp_port_prd_custom_ether_type_get;
    mbcm_dpp_port_prd_control_frame_set_f                               mbcm_dpp_port_prd_control_frame_set;
    mbcm_dpp_port_prd_control_frame_get_f                               mbcm_dpp_port_prd_control_frame_get;
    mbcm_dpp_port_prd_flex_key_construct_set_f                          mbcm_dpp_port_prd_flex_key_construct_set;
    mbcm_dpp_port_prd_flex_key_construct_get_f                          mbcm_dpp_port_prd_flex_key_construct_get;
    mbcm_dpp_port_prd_flex_key_entry_set_f                              mbcm_dpp_port_prd_flex_key_entry_set;
    mbcm_dpp_port_prd_flex_key_entry_get_f                              mbcm_dpp_port_prd_flex_key_entry_get;
    mbcm_dpp_port_prd_restore_hw_defaults_f                                  mbcm_dpp_port_prd_restore_hw_defaults;
    mbcm_dpp_port_control_pcs_set_f                                     mbcm_dpp_port_control_pcs_set;
    mbcm_dpp_port_control_pcs_get_f                                     mbcm_dpp_port_control_pcs_get;
    mbcm_dpp_port_control_power_set_f                                   mbcm_dpp_port_control_power_set;
    mbcm_dpp_port_control_power_get_f                                   mbcm_dpp_port_control_power_get;
    mbcm_dpp_port_control_rx_enable_set_f                               mbcm_dpp_port_control_rx_enable_set;
    mbcm_dpp_port_control_tx_enable_set_f                               mbcm_dpp_port_control_tx_enable_set;
    mbcm_dpp_port_control_tx_nif_enable_set_f                           mbcm_dpp_port_control_tx_nif_enable_set;
    mbcm_dpp_port_control_rx_enable_get_f                               mbcm_dpp_port_control_rx_enable_get;
    mbcm_dpp_port_control_tx_enable_get_f                               mbcm_dpp_port_control_tx_enable_get;
    mbcm_dpp_port_control_tx_nif_enable_get_f                           mbcm_dpp_port_control_tx_nif_enable_get;
    mbcm_dpp_port_control_strip_crc_set_f                               mbcm_dpp_port_control_strip_crc_set;
    mbcm_dpp_port_control_strip_crc_get_f                               mbcm_dpp_port_control_strip_crc_get;
    mbcm_dpp_port_prbs_tx_enable_set_f                                  mbcm_dpp_port_prbs_tx_enable_set;     
    mbcm_dpp_port_prbs_tx_enable_get_f                                  mbcm_dpp_port_prbs_tx_enable_get;     
    mbcm_dpp_port_prbs_rx_enable_set_f                                  mbcm_dpp_port_prbs_rx_enable_set;     
    mbcm_dpp_port_prbs_rx_enable_get_f                                  mbcm_dpp_port_prbs_rx_enable_get;    
    mbcm_dpp_port_prbs_rx_status_get_f                                  mbcm_dpp_port_prbs_rx_status_get;     
    mbcm_dpp_port_prbs_polynomial_set_f                                 mbcm_dpp_port_prbs_polynomial_set;    
    mbcm_dpp_port_prbs_polynomial_get_f                                 mbcm_dpp_port_prbs_polynomial_get;   
    mbcm_dpp_port_prbs_tx_invert_data_set_f                             mbcm_dpp_port_prbs_tx_invert_data_set;
    mbcm_dpp_port_prbs_tx_invert_data_get_f                             mbcm_dpp_port_prbs_tx_invert_data_get;
    mbcm_dpp_port_pfc_refresh_set_f                                     mbcm_dpp_port_pfc_refresh_set;
    mbcm_dpp_port_pfc_refresh_get_f                                     mbcm_dpp_port_pfc_refresh_get;
    mbcm_dpp_local_fault_clear_f                                        mbcm_dpp_local_fault_clear;
    mbcm_dpp_remote_fault_clear_f                                       mbcm_dpp_remote_fault_clear;
    mbcm_dpp_pad_size_set_f                                             mbcm_dpp_pad_size_set;
    mbcm_dpp_pad_size_get_f                                             mbcm_dpp_pad_size_get; 
    mbcm_dpp_port_reset_f                                               mbcm_dpp_port_reset;
    mbcm_dpp_port_phy_control_set_f                                     mbcm_dpp_port_phy_control_set;
    mbcm_dpp_port_phy_control_get_f                                     mbcm_dpp_port_phy_control_get;
    mbcm_dpp_port_phy_get_f                                             mbcm_dpp_port_phy_get;
    mbcm_dpp_port_phy_set_f                                             mbcm_dpp_port_phy_set;
    mbcm_dpp_port_phy_modify_f                                          mbcm_dpp_port_phy_modify;
    mbcm_dpp_port_mac_sa_set_f                                          mbcm_dpp_port_mac_sa_set;
    mbcm_dpp_port_mac_sa_get_f                                          mbcm_dpp_port_mac_sa_get; 
    mbcm_dpp_port_eee_enable_get_f                                      mbcm_dpp_port_eee_enable_get;
    mbcm_dpp_port_eee_enable_set_f                                      mbcm_dpp_port_eee_enable_set;
    mbcm_dpp_port_eee_tx_idle_time_get_f                                mbcm_dpp_port_eee_tx_idle_time_get;
    mbcm_dpp_port_eee_tx_idle_time_set_f                                mbcm_dpp_port_eee_tx_idle_time_set;
    mbcm_dpp_port_eee_tx_wake_time_get_f                                mbcm_dpp_port_eee_tx_wake_time_get;
    mbcm_dpp_port_eee_tx_wake_time_set_f                                mbcm_dpp_port_eee_tx_wake_time_set;
    mbcm_dpp_port_eee_link_active_duration_get_f                        mbcm_dpp_port_eee_link_active_duration_get;
    mbcm_dpp_port_eee_link_active_duration_set_f                        mbcm_dpp_port_eee_link_active_duration_set;
    mbcm_dpp_port_eee_statistics_clear_f                                mbcm_dpp_port_eee_statistics_clear;
    mbcm_dpp_port_eee_event_count_symmetric_set_f                       mbcm_dpp_port_eee_event_count_symmetric_set;
    mbcm_dpp_port_eee_tx_event_count_get_f                              mbcm_dpp_port_eee_tx_event_count_get;
    mbcm_dpp_port_eee_tx_duration_get_f                                 mbcm_dpp_port_eee_tx_duration_get;
    mbcm_dpp_port_eee_rx_event_count_get_f                              mbcm_dpp_port_eee_rx_event_count_get;
    mbcm_dpp_port_eee_rx_duration_get_f                                 mbcm_dpp_port_eee_rx_duration_get;
    mbcm_dpp_port_eee_event_count_symmetric_get_f                       mbcm_dpp_port_eee_event_count_symmetric_get;
    mbcm_dpp_fc_enables_set_f                                           mbcm_dpp_fc_enables_set;
    mbcm_dpp_fc_enables_get_f                                           mbcm_dpp_fc_enables_get;
    mbcm_dpp_fc_ilkn_mub_channel_set_f                                  mbcm_dpp_fc_ilkn_mub_channel_set;
    mbcm_dpp_fc_ilkn_mub_channel_get_f                                  mbcm_dpp_fc_ilkn_mub_channel_get;
    mbcm_dpp_fc_ilkn_mub_gen_cal_set_f                                  mbcm_dpp_fc_ilkn_mub_gen_cal_set;
    mbcm_dpp_fc_ilkn_mub_gen_cal_get_f                                  mbcm_dpp_fc_ilkn_mub_gen_cal_get;
    mbcm_dpp_fc_cat_2_tc_hcfc_bitmap_set_f                              mbcm_dpp_fc_cat_2_tc_hcfc_bitmap_set;
    mbcm_dpp_fc_cat_2_tc_hcfc_bitmap_get_f                              mbcm_dpp_fc_cat_2_tc_hcfc_bitmap_get;
    mbcm_dpp_fc_glb_hcfc_bitmap_set_f                                   mbcm_dpp_fc_glb_hcfc_bitmap_set;
    mbcm_dpp_fc_glb_hcfc_bitmap_get_f                                   mbcm_dpp_fc_glb_hcfc_bitmap_get;
    mbcm_dpp_fc_inbnd_mode_set_f                                        mbcm_dpp_fc_inbnd_mode_set;
    mbcm_dpp_fc_inbnd_mode_get_f                                        mbcm_dpp_fc_inbnd_mode_get;
    mbcm_dpp_fc_glb_rcs_mask_set_f                                      mbcm_dpp_fc_glb_rcs_mask_set;
    mbcm_dpp_fc_glb_rcs_mask_get_f                                      mbcm_dpp_fc_glb_rcs_mask_get;
    mbcm_dpp_fc_init_pfc_mapping_f                                      mbcm_dpp_fc_init_pfc_mapping;
    mbcm_dpp_fc_ilkn_llfc_set_f                                         mbcm_dpp_fc_ilkn_llfc_set;
    mbcm_dpp_fc_ilkn_llfc_get_f                                         mbcm_dpp_fc_ilkn_llfc_get;
    mbcm_dpp_port_ipg_set_f                                             mbcm_dpp_port_ipg_set;
    mbcm_dpp_port_ipg_get_f                                             mbcm_dpp_port_ipg_get;
    mbcm_dpp_port_link_get_f                                            mbcm_dpp_port_link_get;
    mbcm_dpp_port_ilkn_bypass_interface_rx_check_and_enable_f           mbcm_dpp_port_ilkn_bypass_interface_rx_check_and_enable;
    mbcm_dpp_port_ilkn_bypass_interface_rx_enable_get_f                 mbcm_dpp_port_ilkn_bypass_interface_rx_enable_get;
    mbcm_dpp_port_autoneg_set_f                                         mbcm_dpp_port_autoneg_set;
    mbcm_dpp_port_autoneg_get_f                                         mbcm_dpp_port_autoneg_get;
    mbcm_dpp_port_ability_local_get_f                                   mbcm_dpp_port_ability_local_get;
    mbcm_dpp_port_ability_remote_get_f                                  mbcm_dpp_port_ability_remote_get;
    mbcm_dpp_port_ability_advert_set_f                                  mbcm_dpp_port_ability_advert_set;
    mbcm_dpp_port_ability_advert_get_f                                  mbcm_dpp_port_ability_advert_get;
    mbcm_dpp_port_mdix_set_f                                            mbcm_dpp_port_mdix_set;
    mbcm_dpp_port_mdix_get_f                                            mbcm_dpp_port_mdix_get;
    mbcm_dpp_port_mdix_status_get_f                                     mbcm_dpp_port_mdix_status_get;
    mbcm_dpp_port_duplex_set_f                                          mbcm_dpp_port_duplex_set;
    mbcm_dpp_port_duplex_get_f                                          mbcm_dpp_port_duplex_get;
    mbcm_dpp_port_nif_nof_lanes_get_f                                   mbcm_dpp_port_nif_nof_lanes_get;
    mbcm_dpp_port_nif_quad_to_core_validate_f                           mbcm_dpp_port_nif_quad_to_core_validate;
    mbcm_dpp_port_close_path_f                                          mbcm_dpp_port_close_path;
    mbcm_dpp_fc_hcfc_watchdog_set_f                                     mbcm_dpp_fc_hcfc_watchdog_set;
    mbcm_dpp_fc_hcfc_watchdog_get_f                                     mbcm_dpp_fc_hcfc_watchdog_get;
    mbcm_dpp_port_fault_get_f                                           mbcm_dpp_port_fault_get;
    mbcm_dpp_fabric_link_fault_get_f                                    mbcm_dpp_fabric_link_fault_get;    
    mbcm_dpp_stat_if_queue_range_set_f                                  mbcm_dpp_stat_if_queue_range_set;
    mbcm_dpp_stat_if_queue_range_get_f                                  mbcm_dpp_stat_if_queue_range_get;
    mbcm_dpp_ports_logical_sys_id_parse_f                               mbcm_dpp_ports_logical_sys_id_parse;
    mbcm_dpp_fc_vsq_index_group2global_f                                mbcm_dpp_fc_vsq_index_group2global;
    mbcm_dpp_stat_oversize_set_f                                        mbcm_dpp_stat_oversize_set;
    mbcm_dpp_stat_oversize_get_f                                        mbcm_dpp_stat_oversize_get;
    mbcm_dpp_fc_status_info_get_f                                       mbcm_dpp_fc_status_info_get;
    mbcm_dpp_stat_counter_filter_set_f                                  mbcm_dpp_stat_counter_filter_set;
    mbcm_dpp_stat_counter_filter_get_f                                  mbcm_dpp_stat_counter_filter_get;
    mbcm_dpp_cnt_ingress_compensation_profile_delta_set_f               mbcm_dpp_cnt_ingress_compensation_profile_delta_set;
    mbcm_dpp_cnt_ingress_compensation_port_profile_set_f                mbcm_dpp_cnt_ingress_compensation_port_profile_set;
    mbcm_dpp_cnt_ingress_compensation_outLif_delta_set_f                mbcm_dpp_cnt_ingress_compensation_outLif_delta_set;
    mbcm_dpp_cnt_ingress_compensation_outLif_delta_get_f                mbcm_dpp_cnt_ingress_compensation_outLif_delta_get;
    mbcm_dpp_cnt_ingress_compensation_port_delta_and_profile_get_f      mbcm_dpp_cnt_ingress_compensation_port_delta_and_profile_get;
    mbcm_dpp_stat_egress_receive_tm_pointer_format_set_f                mbcm_dpp_stat_egress_receive_tm_pointer_format_set;
    mbcm_dpp_stat_egress_receive_tm_pointer_format_get_f                mbcm_dpp_stat_egress_receive_tm_pointer_format_get;
    mbcm_dpp_stat_counter_erpp_offset_set_f                             mbcm_dpp_stat_counter_erpp_offset_set;
    mbcm_dpp_stat_counter_erpp_offset_get_f                             mbcm_dpp_stat_counter_erpp_offset_get;
    mbcm_dpp_cnt_filter_config_ingress_set_get_f                        mbcm_dpp_cnt_filter_config_ingress_set_get;
    mbcm_dpp_cnt_filter_config_egress_receive_set_get_f                 mbcm_dpp_cnt_filter_config_egress_receive_set_get;
    mbcm_dpp_cnt_crps_iqm_cmd_get_f                                     mbcm_dpp_cnt_crps_iqm_cmd_get;
    mbcm_dpp_cnt_counter_bmap_mem_by_src_type_get_f                     mbcm_dpp_cnt_counter_bmap_mem_by_src_type_get;
    mbcm_dpp_cnt_do_not_count_field_by_src_type_get_f                   mbcm_dpp_cnt_do_not_count_field_by_src_type_get;
    mbcm_dpp_cnt_stif_ingress_pp_source_set_f                           mbcm_dpp_cnt_stif_ingress_pp_source_set;    
    mbcm_dpp_fc_pfc_generic_bitmap_valid_update_f                       mbcm_dpp_fc_pfc_generic_bitmap_valid_update;
    mbcm_dpp_fc_pfc_generic_bitmap_used_update_f                        mbcm_dpp_fc_pfc_generic_bitmap_used_update;
     mbcm_dpp_interrupts_array_init_f                                   mbcm_dpp_interrupts_array_init;
    mbcm_dpp_interrupts_array_deinit_f                                  mbcm_dpp_interrupts_array_deinit;
    mbcm_dpp_jer_interrupt_cb_init_f                                    mbcm_dpp_interrupt_cb_init;
#ifdef PORTMOD_SUPPORT
    mbcm_dpp_soc_pm_instances_get_f                                     mbcm_dpp_soc_pm_instances_get;
#endif
    mbcm_dpp_soc_pml_table_get_f                                        mbcm_dpp_soc_pml_table_get;
    mbcm_dpp_port_quad_get_f                                            mbcm_dpp_port_quad_get;
    mbcm_dpp_port_ports_to_same_quad_get_f                              mbcm_dpp_port_ports_to_same_quad_get;    
    mbcm_dpp_nif_priority_set_f                                         mbcm_dpp_nif_priority_set;
    mbcm_dpp_nif_priority_get_f                                         mbcm_dpp_nif_priority_get;
    mbcm_dpp_nif_priority_quad_tdm_high_low_clear_f                     mbcm_dpp_nif_priority_quad_tdm_high_low_clear;
    mbcm_dpp_nif_priority_ilkn_tdm_clear_f                              mbcm_dpp_nif_priority_ilkn_tdm_clear;
    mbcm_dpp_nif_priority_ilkn_high_low_clear_f                         mbcm_dpp_nif_priority_ilkn_high_low_clear;
    mbcm_dpp_nif_sku_restrictions_f                                     mbcm_dpp_nif_sku_restrictions;
    mbcm_dpp_fabric_link_config_ovrd_f                                  mbcm_dpp_fabric_link_config_ovrd;
    mbcm_dpp_wait_gtimer_trigger_f                                      mbcm_dpp_wait_gtimer_trigger;
    mbcm_dpp_phy_nif_measure_f                                          mbcm_dpp_phy_nif_measure;
    mbcm_dpp_soc_bist_all_f                                             mbcm_dpp_soc_bist_all;
    mbcm_dpp_device_reset_f                                             mbcm_dpp_device_reset;
    mbcm_dpp_fc_cmic_rx_set_f                                           mbcm_dpp_fc_cmic_rx_set;
    mbcm_dpp_fc_cmic_rx_get_f                                           mbcm_dpp_fc_cmic_rx_get;
    mbcm_dpp_mult_get_entry_f                                           mbcm_dpp_mult_get_entry;
    mbcm_dpp_port_ilkn_over_fabric_set_f                                mbcm_dpp_port_ilkn_over_fabric_set;
    mbcm_dpp_lb_ing_total_shared_buffer_config_set_f                    mbcm_dpp_lb_ing_total_shared_buffer_config_set;
    mbcm_dpp_lb_ing_total_shared_buffer_config_get_f                    mbcm_dpp_lb_ing_total_shared_buffer_config_get;
    mbcm_dpp_lb_ing_guaranteed_buffer_config_set_f                      mbcm_dpp_lb_ing_guaranteed_buffer_config_set;
    mbcm_dpp_lb_ing_guaranteed_buffer_config_get_f                      mbcm_dpp_lb_ing_guaranteed_buffer_config_get;
    mbcm_dpp_lb_ing_modem_to_lbg_map_set_f                              mbcm_dpp_lb_ing_modem_to_lbg_map_set;
    mbcm_dpp_lb_ing_modem_to_lbg_unmap_set_f                            mbcm_dpp_lb_ing_modem_to_lbg_unmap_set;
    mbcm_dpp_lb_ing_modem_to_lbg_map_get_f                              mbcm_dpp_lb_ing_modem_to_lbg_map_get;
    mbcm_dpp_lb_ing_expected_seqeunce_num_set_f                         mbcm_dpp_lb_ing_expected_seqeunce_num_set;
    mbcm_dpp_lb_ing_expected_seqeunce_num_get_f                         mbcm_dpp_lb_ing_expected_seqeunce_num_get;
    mbcm_dpp_lb_ing_glb_seqeunce_num_width_set_f                        mbcm_dpp_lb_ing_glb_seqeunce_num_width_set;
    mbcm_dpp_lb_ing_glb_seqeunce_num_width_get_f                        mbcm_dpp_lb_ing_glb_seqeunce_num_width_get;
    mbcm_dpp_lb_ing_packet_to_modem_map_set_f                           mbcm_dpp_lb_ing_packet_to_modem_map_set;
    mbcm_dpp_lb_ing_packet_to_modem_map_get_f                           mbcm_dpp_lb_ing_packet_to_modem_map_get;
    mbcm_dpp_lb_ing_reorder_config_set_f                                mbcm_dpp_lb_ing_reorder_config_set;
    mbcm_dpp_lb_ing_reorder_config_get_f                                mbcm_dpp_lb_ing_reorder_config_get;
    mbcm_dpp_lb_ing_flush_config_set_f                                  mbcm_dpp_lb_ing_flush_config_set;
    mbcm_dpp_lb_ing_flush_config_get_f                                  mbcm_dpp_lb_ing_flush_config_get;
    mbcm_dpp_lb_ing_status_get_f                                        mbcm_dpp_lb_ing_status_get;
    mbcm_dpp_lb_ing_modem_status_get_f                                  mbcm_dpp_lb_ing_modem_status_get;
    mbcm_dpp_lb_ing_glb_stat_get_f                                      mbcm_dpp_lb_ing_glb_stat_get;
    mbcm_dpp_lb_ing_modem_stat_get_f                                    mbcm_dpp_lb_ing_modem_stat_get;
    mbcm_dpp_lb_egr_modem_to_port_map_set_f                             mbcm_dpp_lb_egr_modem_to_port_map_set;
    mbcm_dpp_lb_egr_modem_to_port_unmap_set_f                           mbcm_dpp_lb_egr_modem_to_port_unmap_set;
    mbcm_dpp_lb_egr_modem_to_port_map_get_f                             mbcm_dpp_lb_egr_modem_to_port_map_get;
    mbcm_dpp_lb_egr_lbg_enable_set_f                                    mbcm_dpp_lb_egr_lbg_enable_set;
    mbcm_dpp_lb_egr_lbg_enable_get_f                                    mbcm_dpp_lb_egr_lbg_enable_get;
    mbcm_dpp_lb_egr_modem_to_lbg_map_set_f                              mbcm_dpp_lb_egr_modem_to_lbg_map_set;
    mbcm_dpp_lb_egr_modem_to_lbg_unmap_set_f                            mbcm_dpp_lb_egr_modem_to_lbg_unmap_set;
    mbcm_dpp_lb_egr_modem_to_lbg_map_get_f                              mbcm_dpp_lb_egr_modem_to_lbg_map_get;
    mbcm_dpp_lb_egr_expected_seqeunce_num_set_f                         mbcm_dpp_lb_egr_expected_seqeunce_num_set;
    mbcm_dpp_lb_egr_expected_seqeunce_num_get_f                         mbcm_dpp_lb_egr_expected_seqeunce_num_get;
    mbcm_dpp_lb_egr_seqeunce_num_width_set_f                            mbcm_dpp_lb_egr_seqeunce_num_width_set;
    mbcm_dpp_lb_egr_seqeunce_num_width_get_f                            mbcm_dpp_lb_egr_seqeunce_num_width_get;
    mbcm_dpp_lb_egr_modem_seqeunce_num_width_set_f                      mbcm_dpp_lb_egr_modem_seqeunce_num_width_set;
    mbcm_dpp_lb_egr_modem_seqeunce_num_width_get_f                      mbcm_dpp_lb_egr_modem_seqeunce_num_width_get;
    mbcm_dpp_lb_global_packet_config_set_f                              mbcm_dpp_lb_global_packet_config_set;
    mbcm_dpp_lb_global_packet_config_get_f                              mbcm_dpp_lb_global_packet_config_get;
    mbcm_dpp_lb_modem_packet_config_set_f                               mbcm_dpp_lb_modem_packet_config_set;
    mbcm_dpp_lb_modem_packet_config_get_f                               mbcm_dpp_lb_modem_packet_config_get;
    mbcm_dpp_lb_tc_dp_to_packet_priority_config_set_f                   mbcm_dpp_lb_tc_dp_to_packet_priority_config_set;
    mbcm_dpp_lb_tc_dp_to_packet_priority_config_get_f                   mbcm_dpp_lb_tc_dp_to_packet_priority_config_get;
    mbcm_dpp_lb_egr_sched_set_f                                         mbcm_dpp_lb_egr_sched_set;
    mbcm_dpp_lb_egr_sched_get_f                                         mbcm_dpp_lb_egr_sched_get;
    mbcm_dpp_lb_egr_segment_config_set_f                                mbcm_dpp_lb_egr_segment_config_set;
    mbcm_dpp_lb_egr_segment_config_get_f                                mbcm_dpp_lb_egr_segment_config_get;
    mbcm_dpp_lb_egr_modem_shaper_set_f                                  mbcm_dpp_lb_egr_modem_shaper_set;
    mbcm_dpp_lb_egr_modem_shaper_get_f                                  mbcm_dpp_lb_egr_modem_shaper_get;
    mbcm_dpp_lb_egr_modem_segment_shaper_set_f                          mbcm_dpp_lb_egr_modem_segment_shaper_set;
    mbcm_dpp_lb_egr_modem_segment_shaper_get_f                          mbcm_dpp_lb_egr_modem_segment_shaper_get;
    mbcm_dpp_lb_egr_status_get_f                                        mbcm_dpp_lb_egr_status_get;
    mbcm_dpp_lb_egr_stat_get_f                                          mbcm_dpp_lb_egr_stat_get;
    mbcm_dpp_lb_egr_modem_stat_get_f                                    mbcm_dpp_lb_egr_modem_stat_get;
    mbcm_dpp_lb_ports_on_reserve_intf_get_f                             mbcm_dpp_lb_ports_on_reserve_intf_get;
    mbcm_dpp_lb_config_info_get_f                                       mbcm_dpp_lb_config_info_get;
    mbcm_dpp_mtr_ir_val_to_max_rev_exp_optimized_for_bucket_rate_f      mbcm_dpp_mtr_ir_val_to_max_rev_exp_optimized_for_bucket_rate;    
    mbcm_dpp_fc_pfc_index_get_f                                         mbcm_dpp_fc_pfc_index_get;
    mbcm_dpp_lb_lbg_packet_config_set_f                                 mbcm_dpp_lb_lbg_packet_config_set;
    mbcm_dpp_lb_lbg_packet_config_get_f                                 mbcm_dpp_lb_lbg_packet_config_get;    
    mbcm_dpp_lb_ing_lbg_stat_get_f                                      mbcm_dpp_lb_ing_lbg_stat_get;
    mbcm_dpp_pon_ports_enable_set_f                                     mbcm_dpp_pon_ports_enable_set;
    mbcm_dpp_pon_ports_enable_get_f                                     mbcm_dpp_pon_ports_enable_get;
    mbcm_dpp_lb_port_speed_validate_f                                   mbcm_dpp_lb_port_speed_validate;
    mbcm_dpp_lb_port_speed_set_f                                        mbcm_dpp_lb_port_speed_set;
    mbcm_dpp_lb_port_speed_get_f                                        mbcm_dpp_lb_port_speed_get;
    mbcm_dpp_port_timestamp_adjust_set_f                                mbcm_dpp_port_timestamp_adjust_set;
    mbcm_dpp_port_timestamp_adjust_get_f                                mbcm_dpp_port_timestamp_adjust_get;
    mbcm_dpp_port_master_set_f                                          mbcm_dpp_port_master_set;
    mbcm_dpp_port_master_get_f                                          mbcm_dpp_port_master_get;
    mbcm_dpp_queue_flush_set_f                                          mbcm_dpp_queue_flush_set;
    mbcm_dpp_sysport_ingress_queue_map_enable_set_f                     mbcm_dpp_sysport_ingress_queue_map_enable_set;
    mbcm_dpp_sysport_ingress_queue_map_enable_get_f                     mbcm_dpp_sysport_ingress_queue_map_enable_get;
} mbcm_dpp_functions_t; 

extern CONST mbcm_dpp_functions_t *mbcm_dpp_driver[BCM_MAX_NUM_UNITS]; 
extern soc_dpp_chip_family_t mbcm_dpp_family[BCM_MAX_NUM_UNITS];

extern CONST mbcm_dpp_functions_t mbcm_petrab_driver;
extern CONST mbcm_dpp_functions_t mbcm_arad_driver;
extern CONST mbcm_dpp_functions_t mbcm_jericho_driver;
extern CONST mbcm_dpp_functions_t mbcm_qax_driver;

extern int mbcm_dpp_init(int unit);


#define MBCM_DPP_DRIVER_CALL(unit, function, args) ((mbcm_dpp_driver[unit] && mbcm_dpp_driver[unit]->function)?mbcm_dpp_driver[unit]->function args:SOC_SAND_ERR)

#define MBCM_DPP_DRIVER_CALL_VOID(unit, function, args) do { \
                                                            if (mbcm_dpp_driver[unit] && mbcm_dpp_driver[unit]->function)\
                                                                mbcm_dpp_driver[unit]->function args;\
                                                            } while (0)


#define MBCM_DPP_DRIVER_CALL_WITHOUT_DEV_ID(unit, function, args) ((mbcm_dpp_driver[unit] && mbcm_dpp_driver[unit]->function)?mbcm_dpp_driver[unit]->function args:SOC_SAND_ERR)


#define MBCM_DPP_SOC_DRIVER_CALL(unit, function, args) ((mbcm_dpp_driver[unit] && mbcm_dpp_driver[unit]->function)?mbcm_dpp_driver[unit]->function args: SOC_E_UNAVAIL)




#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif  
