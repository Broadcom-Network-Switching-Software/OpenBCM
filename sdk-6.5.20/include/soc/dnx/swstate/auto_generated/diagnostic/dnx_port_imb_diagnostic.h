
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_PORT_IMB_DIAGNOSTIC_H__
#define __DNX_PORT_IMB_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_port_imb_types.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <soc/dnxc/swstate/types/sw_state_mutex.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    IMBM_INFO,
    IMBM_IMBS_IN_USE_INFO,
    IMBM_IMB_INFO,
    IMBM_IMB_TYPE_INFO,
    IMBM_PMD_LOCK_COUNTER_INFO,
    IMBM_CREDIT_TX_RESET_MUTEX_INFO,
    IMBM_INFO_NOF_ENTRIES
} sw_state_imbm_layout_e;


extern dnx_sw_state_diagnostic_info_t imbm_info[SOC_MAX_NUM_DEVICES][IMBM_INFO_NOF_ENTRIES];

extern const char* imbm_layout_str[IMBM_INFO_NOF_ENTRIES];
int imbm_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int imbm_imbs_in_use_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int imbm_imb_dump(
    int unit, int imb_idx_0, dnx_sw_state_dump_filters_t filters);

int imbm_imb_type_dump(
    int unit, int imb_type_idx_0, dnx_sw_state_dump_filters_t filters);

int imbm_pmd_lock_counter_dump(
    int unit, int pmd_lock_counter_idx_0, dnx_sw_state_dump_filters_t filters);

int imbm_credit_tx_reset_mutex_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
