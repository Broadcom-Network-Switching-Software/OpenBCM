
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_OFP_RATE_DIAGNOSTIC_H__
#define __DNX_OFP_RATE_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_ofp_rate_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_egr_queuing.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_OFP_RATE_DB_INFO,
    DNX_OFP_RATE_DB_OTM_INFO,
    DNX_OFP_RATE_DB_OTM_CAL_INSTANCE_INFO,
    DNX_OFP_RATE_DB_OTM_SHAPING_INFO,
    DNX_OFP_RATE_DB_OTM_SHAPING_RATE_INFO,
    DNX_OFP_RATE_DB_OTM_SHAPING_BURST_INFO,
    DNX_OFP_RATE_DB_OTM_SHAPING_VALID_INFO,
    DNX_OFP_RATE_DB_QPAIR_INFO,
    DNX_OFP_RATE_DB_QPAIR_RATE_INFO,
    DNX_OFP_RATE_DB_QPAIR_VALID_INFO,
    DNX_OFP_RATE_DB_TCG_INFO,
    DNX_OFP_RATE_DB_TCG_RATE_INFO,
    DNX_OFP_RATE_DB_TCG_VALID_INFO,
    DNX_OFP_RATE_DB_OTM_CAL_INFO,
    DNX_OFP_RATE_DB_OTM_CAL_RATE_INFO,
    DNX_OFP_RATE_DB_OTM_CAL_MODIFIED_INFO,
    DNX_OFP_RATE_DB_OTM_CAL_NOF_CALCAL_INSTANCES_INFO,
    DNX_OFP_RATE_DB_CALCAL_IS_MODIFIED_INFO,
    DNX_OFP_RATE_DB_CALCAL_LEN_INFO,
    DNX_OFP_RATE_DB_INFO_NOF_ENTRIES
} sw_state_dnx_ofp_rate_db_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_ofp_rate_db_info[SOC_MAX_NUM_DEVICES][DNX_OFP_RATE_DB_INFO_NOF_ENTRIES];

extern const char* dnx_ofp_rate_db_layout_str[DNX_OFP_RATE_DB_INFO_NOF_ENTRIES];
int dnx_ofp_rate_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_ofp_rate_db_otm_dump(
    int unit, int otm_idx_0, int otm_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_ofp_rate_db_otm_cal_instance_dump(
    int unit, int otm_idx_0, int otm_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_ofp_rate_db_otm_shaping_dump(
    int unit, int otm_idx_0, int otm_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_ofp_rate_db_otm_shaping_rate_dump(
    int unit, int otm_idx_0, int otm_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_ofp_rate_db_otm_shaping_burst_dump(
    int unit, int otm_idx_0, int otm_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_ofp_rate_db_otm_shaping_valid_dump(
    int unit, int otm_idx_0, int otm_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_ofp_rate_db_qpair_dump(
    int unit, int qpair_idx_0, int qpair_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_ofp_rate_db_qpair_rate_dump(
    int unit, int qpair_idx_0, int qpair_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_ofp_rate_db_qpair_valid_dump(
    int unit, int qpair_idx_0, int qpair_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_ofp_rate_db_tcg_dump(
    int unit, int tcg_idx_0, int tcg_idx_1, int tcg_idx_2, dnx_sw_state_dump_filters_t filters);

int dnx_ofp_rate_db_tcg_rate_dump(
    int unit, int tcg_idx_0, int tcg_idx_1, int tcg_idx_2, dnx_sw_state_dump_filters_t filters);

int dnx_ofp_rate_db_tcg_valid_dump(
    int unit, int tcg_idx_0, int tcg_idx_1, int tcg_idx_2, dnx_sw_state_dump_filters_t filters);

int dnx_ofp_rate_db_otm_cal_dump(
    int unit, int otm_cal_idx_0, int otm_cal_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_ofp_rate_db_otm_cal_rate_dump(
    int unit, int otm_cal_idx_0, int otm_cal_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_ofp_rate_db_otm_cal_modified_dump(
    int unit, int otm_cal_idx_0, int otm_cal_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_ofp_rate_db_otm_cal_nof_calcal_instances_dump(
    int unit, int otm_cal_idx_0, int otm_cal_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_ofp_rate_db_calcal_is_modified_dump(
    int unit, int calcal_is_modified_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_ofp_rate_db_calcal_len_dump(
    int unit, int calcal_len_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
