/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _JER_CNT_H
#define _JER_CNT_H


#include <soc/dpp/TMC/tmc_api_cnt.h>

int soc_jer_stat_counter_filter_set(SOC_SAND_IN int                             unit, 
                                    SOC_SAND_IN SOC_TMC_CNT_SOURCE*             source, 
                                    SOC_SAND_IN SOC_TMC_CNT_FILTER_TYPE*        filter_array, 
                                    SOC_SAND_IN int                             filter_count,
                                    SOC_SAND_IN int                             is_active );

int soc_jer_stat_counter_filter_get(SOC_SAND_IN  int                             unit, 
                                    SOC_SAND_IN  SOC_TMC_CNT_SOURCE*             source, 
                                    SOC_SAND_IN  int                             filter_max_count,
                                    SOC_SAND_OUT SOC_TMC_CNT_FILTER_TYPE*        filter_array, 
                                    SOC_SAND_OUT int*                            filter_count );

int soc_jer_cnt_ingress_compensation_profile_delta_set(SOC_SAND_IN int unit, SOC_SAND_IN int core, SOC_SAND_IN int src_type, SOC_SAND_IN int profileIndex, SOC_SAND_IN int delta);
int soc_jer_cnt_ingress_compensation_port_profile_set(SOC_SAND_IN  int unit, SOC_SAND_IN  int core, SOC_SAND_IN  int commandId, SOC_SAND_IN  int src_type, SOC_SAND_IN  int port, SOC_SAND_IN  int profileIndex);
int soc_jer_cnt_ingress_compensation_outLif_delta_set(SOC_SAND_IN  int unit, SOC_SAND_IN  int core, SOC_SAND_IN  int commandId, SOC_SAND_IN  int src_type, SOC_SAND_IN  int lifIndex, SOC_SAND_IN  int delta);
int soc_jer_cnt_ingress_compensation_outLif_delta_get(SOC_SAND_IN  int unit, SOC_SAND_IN  int core, SOC_SAND_IN  int commandId, SOC_SAND_IN  int src_type, SOC_SAND_IN  int lifIndex, SOC_SAND_OUT  int * delta);
int soc_jer_cnt_ingress_compensation_port_delta_and_profile_get(SOC_SAND_IN  int unit, SOC_SAND_IN  int core, SOC_SAND_IN  int commandId, SOC_SAND_IN  int src_type, SOC_SAND_IN  int port, SOC_SAND_OUT  int * profileIndex,  SOC_SAND_OUT  int * delta);
int soc_jer_stat_egress_receive_tm_pointer_format_set(SOC_SAND_IN  int unit, SOC_SAND_IN  uint32 queue_pair_mask, SOC_SAND_IN  uint32 traffic_class_mask, SOC_SAND_IN  uint32 cast_mask, SOC_SAND_OUT uint32* queue_pair_shift, SOC_SAND_OUT uint32* traffic_class_shift, SOC_SAND_OUT uint32* cast_shift );
int soc_jer_stat_egress_receive_tm_pointer_format_get(SOC_SAND_IN  int unit, SOC_SAND_OUT uint32* queue_pair_mask, SOC_SAND_OUT uint32* traffic_class_mask, SOC_SAND_OUT uint32* cast_mask);
int soc_jer_filter_config_ingress_set_get(int unit, int relevant_bit, int command_id, int get, int* value);
int soc_jer_filter_config_egress_receive_set_get(int unit, int relevant_bit, int command_id, int get, int* value);
int jer_plus_cnt_out_lif_counting_range_set(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN SOC_TMC_CNT_SRC_TYPE source,    
    SOC_SAND_IN int crps_counter_id, 
    SOC_SAND_IN SOC_SAND_U32_RANGE  *profile_range,    
    SOC_SAND_IN int stif_counter_id,    
    SOC_SAND_IN int is_double_entry,
    SOC_SAND_IN int engine_range_offset
    );

int jer_plus_cnt_out_lif_counting_range_get(
    SOC_SAND_IN int                       unit,
    SOC_SAND_INOUT SOC_SAND_U32_RANGE*    range,
    SOC_SAND_OUT uint32*                  offset,
    SOC_SAND_OUT int*                     command_id,
    SOC_SAND_OUT int*                     stif_id,
    SOC_SAND_OUT int*                     is_double_entry
    );

#endif 
