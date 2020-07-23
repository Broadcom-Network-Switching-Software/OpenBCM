
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __KBP_DIAGNOSTIC_H__
#define __KBP_DIAGNOSTIC_H__

#ifdef BCM_DNX_SUPPORT
#if defined(INCLUDE_KBP)
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/kbp_types.h>
#include <soc/kbp/alg_kbp/include/ad.h>
#include <soc/kbp/alg_kbp/include/db.h>
#include <soc/kbp/alg_kbp/include/instruction.h>
#include <soc/kbp/alg_kbp/include/kbp_hb.h>
#include <soc/kbp/alg_kbp/include/key.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    KBP_SW_STATE_INFO,
    KBP_SW_STATE_IS_DEVICE_LOCKED_INFO,
    KBP_SW_STATE_DB_HANDLES_INFO_INFO,
    KBP_SW_STATE_DB_HANDLES_INFO_CACHING_BMP_INFO,
    KBP_SW_STATE_DB_HANDLES_INFO_DB_P_INFO,
    KBP_SW_STATE_DB_HANDLES_INFO_AD_DB_ZERO_SIZE_P_INFO,
    KBP_SW_STATE_DB_HANDLES_INFO_AD_ENTRY_ZERO_SIZE_P_INFO,
    KBP_SW_STATE_DB_HANDLES_INFO_AD_DB_P_INFO,
    KBP_SW_STATE_DB_HANDLES_INFO_OPT_RESULT_SIZE_INFO,
    KBP_SW_STATE_DB_HANDLES_INFO_CLONED_DB_ID_INFO,
    KBP_SW_STATE_DB_HANDLES_INFO_ASSOCIATED_DBAL_TABLE_ID_INFO,
    KBP_SW_STATE_INSTRUCTION_INFO_INFO,
    KBP_SW_STATE_INSTRUCTION_INFO_INST_P_INFO,
    KBP_SW_STATE_INFO_NOF_ENTRIES
} sw_state_kbp_sw_state_layout_e;


extern dnx_sw_state_diagnostic_info_t kbp_sw_state_info[SOC_MAX_NUM_DEVICES][KBP_SW_STATE_INFO_NOF_ENTRIES];

extern const char* kbp_sw_state_layout_str[KBP_SW_STATE_INFO_NOF_ENTRIES];
int kbp_sw_state_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int kbp_sw_state_is_device_locked_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int kbp_sw_state_db_handles_info_dump(
    int unit, int db_handles_info_idx_0, dnx_sw_state_dump_filters_t filters);

int kbp_sw_state_db_handles_info_caching_bmp_dump(
    int unit, int db_handles_info_idx_0, dnx_sw_state_dump_filters_t filters);

int kbp_sw_state_db_handles_info_db_p_dump(
    int unit, int db_handles_info_idx_0, dnx_sw_state_dump_filters_t filters);

int kbp_sw_state_db_handles_info_ad_db_zero_size_p_dump(
    int unit, int db_handles_info_idx_0, dnx_sw_state_dump_filters_t filters);

int kbp_sw_state_db_handles_info_ad_entry_zero_size_p_dump(
    int unit, int db_handles_info_idx_0, dnx_sw_state_dump_filters_t filters);

int kbp_sw_state_db_handles_info_ad_db_p_dump(
    int unit, int db_handles_info_idx_0, int ad_db_p_idx_0, dnx_sw_state_dump_filters_t filters);

int kbp_sw_state_db_handles_info_opt_result_size_dump(
    int unit, int db_handles_info_idx_0, dnx_sw_state_dump_filters_t filters);

int kbp_sw_state_db_handles_info_cloned_db_id_dump(
    int unit, int db_handles_info_idx_0, dnx_sw_state_dump_filters_t filters);

int kbp_sw_state_db_handles_info_associated_dbal_table_id_dump(
    int unit, int db_handles_info_idx_0, dnx_sw_state_dump_filters_t filters);

int kbp_sw_state_instruction_info_dump(
    int unit, int instruction_info_idx_0, dnx_sw_state_dump_filters_t filters);

int kbp_sw_state_instruction_info_inst_p_dump(
    int unit, int instruction_info_idx_0, int inst_p_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 
#endif  
#endif  

#endif 
