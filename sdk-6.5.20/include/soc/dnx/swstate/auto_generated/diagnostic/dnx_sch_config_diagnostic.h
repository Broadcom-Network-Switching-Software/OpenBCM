
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_SCH_CONFIG_DIAGNOSTIC_H__
#define __DNX_SCH_CONFIG_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_sch_config_types.h>
#include <include/bcm/types.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    COSQ_CONFIG_INFO,
    COSQ_CONFIG_HR_GROUP_BW_INFO,
    COSQ_CONFIG_GROUPS_BW_INFO,
    COSQ_CONFIG_IPF_CONFIG_MODE_INFO,
    COSQ_CONFIG_FLOW_INFO,
    COSQ_CONFIG_FLOW_CREDIT_SRC_INFO,
    COSQ_CONFIG_FLOW_CREDIT_SRC_WEIGHT_INFO,
    COSQ_CONFIG_FLOW_CREDIT_SRC_MODE_INFO,
    COSQ_CONFIG_CONNECTOR_INFO,
    COSQ_CONFIG_CONNECTOR_NUM_COS_INFO,
    COSQ_CONFIG_CONNECTOR_CONNECTION_VALID_INFO,
    COSQ_CONFIG_CONNECTOR_SRC_MODID_INFO,
    COSQ_CONFIG_SE_INFO,
    COSQ_CONFIG_SE_CHILD_COUNT_INFO,
    COSQ_CONFIG_INFO_NOF_ENTRIES
} sw_state_cosq_config_layout_e;


extern dnx_sw_state_diagnostic_info_t cosq_config_info[SOC_MAX_NUM_DEVICES][COSQ_CONFIG_INFO_NOF_ENTRIES];

extern const char* cosq_config_layout_str[COSQ_CONFIG_INFO_NOF_ENTRIES];
int cosq_config_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int cosq_config_hr_group_bw_dump(
    int unit, int hr_group_bw_idx_0, int hr_group_bw_idx_1, dnx_sw_state_dump_filters_t filters);

int cosq_config_groups_bw_dump(
    int unit, int groups_bw_idx_0, int groups_bw_idx_1, dnx_sw_state_dump_filters_t filters);

int cosq_config_ipf_config_mode_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int cosq_config_flow_dump(
    int unit, int flow_idx_0, int flow_idx_1, dnx_sw_state_dump_filters_t filters);

int cosq_config_flow_credit_src_dump(
    int unit, int flow_idx_0, int flow_idx_1, dnx_sw_state_dump_filters_t filters);

int cosq_config_flow_credit_src_weight_dump(
    int unit, int flow_idx_0, int flow_idx_1, dnx_sw_state_dump_filters_t filters);

int cosq_config_flow_credit_src_mode_dump(
    int unit, int flow_idx_0, int flow_idx_1, dnx_sw_state_dump_filters_t filters);

int cosq_config_connector_dump(
    int unit, int connector_idx_0, int connector_idx_1, dnx_sw_state_dump_filters_t filters);

int cosq_config_connector_num_cos_dump(
    int unit, int connector_idx_0, int connector_idx_1, dnx_sw_state_dump_filters_t filters);

int cosq_config_connector_connection_valid_dump(
    int unit, int connector_idx_0, int connector_idx_1, dnx_sw_state_dump_filters_t filters);

int cosq_config_connector_src_modid_dump(
    int unit, int connector_idx_0, int connector_idx_1, dnx_sw_state_dump_filters_t filters);

int cosq_config_se_dump(
    int unit, int se_idx_0, int se_idx_1, dnx_sw_state_dump_filters_t filters);

int cosq_config_se_child_count_dump(
    int unit, int se_idx_0, int se_idx_1, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
