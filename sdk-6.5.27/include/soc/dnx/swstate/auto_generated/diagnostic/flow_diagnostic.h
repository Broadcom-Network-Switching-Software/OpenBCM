
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FLOW_DIAGNOSTIC_H__
#define __DNX_FLOW_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnx/swstate/auto_generated/types/flow_types.h>
#include <include/bcm/types.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_max_flow.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    FLOW_DB_INFO,
    FLOW_DB_FLOW_APPLICATION_INFO_INFO,
    FLOW_DB_FLOW_APPLICATION_INFO_IS_VERIFY_DISABLED_INFO,
    FLOW_DB_FLOW_APPLICATION_INFO_IS_ERROR_RECOVERY_DISABLED_INFO,
    FLOW_DB_VALID_PHASES_PER_DBAL_TABLE_INFO,
    FLOW_DB_DBAL_TABLE_TO_VALID_RESULT_TYPES_INFO,
    FLOW_DB_INGRESS_GPORT_TO_MATCH_INFO_HTB_INFO,
    FLOW_DB_INFO_NOF_ENTRIES
} sw_state_flow_db_layout_e;


extern dnx_sw_state_diagnostic_info_t flow_db_info[SOC_MAX_NUM_DEVICES][FLOW_DB_INFO_NOF_ENTRIES];

extern const char* flow_db_layout_str[FLOW_DB_INFO_NOF_ENTRIES];
int flow_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int flow_db_flow_application_info_dump(
    int unit, int flow_application_info_idx_0, dnx_sw_state_dump_filters_t filters);

int flow_db_flow_application_info_is_verify_disabled_dump(
    int unit, int flow_application_info_idx_0, dnx_sw_state_dump_filters_t filters);

int flow_db_flow_application_info_is_error_recovery_disabled_dump(
    int unit, int flow_application_info_idx_0, dnx_sw_state_dump_filters_t filters);

int flow_db_valid_phases_per_dbal_table_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int flow_db_dbal_table_to_valid_result_types_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int flow_db_ingress_gport_to_match_info_htb_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
