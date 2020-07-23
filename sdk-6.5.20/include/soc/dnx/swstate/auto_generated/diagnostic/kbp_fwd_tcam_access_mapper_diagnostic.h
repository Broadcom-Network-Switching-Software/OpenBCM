
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __KBP_FWD_TCAM_ACCESS_MAPPER_DIAGNOSTIC_H__
#define __KBP_FWD_TCAM_ACCESS_MAPPER_DIAGNOSTIC_H__

#if defined(INCLUDE_KBP)
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/kbp_fwd_tcam_access_mapper_types.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    KBP_FWD_TCAM_ACCESS_MAPPER_INFO,
    KBP_FWD_TCAM_ACCESS_MAPPER_KEY_2_ENTRY_ID_HASH_INFO,
    KBP_FWD_TCAM_ACCESS_MAPPER_INFO_NOF_ENTRIES
} sw_state_kbp_fwd_tcam_access_mapper_layout_e;


extern dnx_sw_state_diagnostic_info_t kbp_fwd_tcam_access_mapper_info[SOC_MAX_NUM_DEVICES][KBP_FWD_TCAM_ACCESS_MAPPER_INFO_NOF_ENTRIES];

extern const char* kbp_fwd_tcam_access_mapper_layout_str[KBP_FWD_TCAM_ACCESS_MAPPER_INFO_NOF_ENTRIES];
int kbp_fwd_tcam_access_mapper_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int kbp_fwd_tcam_access_mapper_key_2_entry_id_hash_dump(
    int unit, int key_2_entry_id_hash_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 
#endif  

#endif 
