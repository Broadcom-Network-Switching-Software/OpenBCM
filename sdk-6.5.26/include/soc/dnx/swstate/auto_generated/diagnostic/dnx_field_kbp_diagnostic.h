
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_KBP_DIAGNOSTIC_H__
#define __DNX_FIELD_KBP_DIAGNOSTIC_H__

#ifdef INCLUDE_KBP
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_kbp_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_FIELD_KBP_SW_INFO,
    DNX_FIELD_KBP_SW_OPCODE_INFO_INFO,
    DNX_FIELD_KBP_SW_OPCODE_INFO_MASTER_KEY_INFO_INFO,
    DNX_FIELD_KBP_SW_OPCODE_INFO_MASTER_KEY_INFO_SEGMENT_INFO_INFO,
    DNX_FIELD_KBP_SW_OPCODE_INFO_MASTER_KEY_INFO_SEGMENT_INFO_QUAL_IDX_INFO,
    DNX_FIELD_KBP_SW_OPCODE_INFO_MASTER_KEY_INFO_SEGMENT_INFO_FG_ID_INFO,
    DNX_FIELD_KBP_SW_OPCODE_INFO_MASTER_KEY_INFO_NOF_FWD_SEGMENTS_INFO,
    DNX_FIELD_KBP_SW_OPCODE_INFO_IS_VALID_INFO,
    DNX_FIELD_KBP_SW_INFO_NOF_ENTRIES
} sw_state_dnx_field_kbp_sw_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_field_kbp_sw_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_KBP_SW_INFO_NOF_ENTRIES];

extern const char* dnx_field_kbp_sw_layout_str[DNX_FIELD_KBP_SW_INFO_NOF_ENTRIES];
int dnx_field_kbp_sw_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_field_kbp_sw_opcode_info_dump(
    int unit, int opcode_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_kbp_sw_opcode_info_master_key_info_dump(
    int unit, int opcode_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_dump(
    int unit, int opcode_info_idx_0, int segment_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_qual_idx_dump(
    int unit, int opcode_info_idx_0, int segment_info_idx_0, int qual_idx_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_fg_id_dump(
    int unit, int opcode_info_idx_0, int segment_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_kbp_sw_opcode_info_master_key_info_nof_fwd_segments_dump(
    int unit, int opcode_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_kbp_sw_opcode_info_is_valid_dump(
    int unit, int opcode_info_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 
#endif  

#endif 
