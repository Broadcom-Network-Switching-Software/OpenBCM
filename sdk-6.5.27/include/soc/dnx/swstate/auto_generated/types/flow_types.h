
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __FLOW_TYPES_H__
#define __FLOW_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <include/bcm/types.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_max_flow.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>


typedef struct {
    dbal_tables_e dbal_table;
    uint8 encap_access;
} flow_app_encap_info_t;

typedef struct {
    uint8 is_verify_disabled;
    uint8 is_error_recovery_disabled;
} dnx_flow_app_common_config_t;

typedef struct {
    dbal_tables_e dbal_table;
    uint32 match_key[DNX_DATA_MAX_FLOW_GENERAL_MATCH_ENTRY_INFO_MAX_KEY_SIZE_IN_WORDS];
} match_entry_info_t;

typedef struct {
    match_entry_info_t single_match_info[DNX_DATA_MAX_FLOW_GENERAL_MATCH_PER_GPORT_SW_NOF_ENTRIES];
    int counter;
} ingress_gport_sw_info_t;

typedef struct {
    dnx_flow_app_common_config_t* flow_application_info;
    sw_state_htbl_t valid_phases_per_dbal_table;
    sw_state_htbl_t dbal_table_to_valid_result_types;
    sw_state_htbl_t ingress_gport_to_match_info_htb;
} flow_sw_state_t;


#endif 
