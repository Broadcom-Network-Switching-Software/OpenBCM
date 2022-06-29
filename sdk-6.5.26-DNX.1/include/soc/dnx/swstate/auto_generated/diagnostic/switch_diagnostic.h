
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_SWITCH_DIAGNOSTIC_H__
#define __DNX_SWITCH_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnx/swstate/auto_generated/types/switch_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    SWITCH_DB_INFO,
    SWITCH_DB_MODULE_VERIFICATION_INFO,
    SWITCH_DB_MODULE_ERROR_RECOVERY_INFO,
    SWITCH_DB_L3MCASTL2_IPV4_FWD_TYPE_INFO,
    SWITCH_DB_L3MCASTL2_IPV6_FWD_TYPE_INFO,
    SWITCH_DB_HEADER_ENABLERS_HASHING_INFO,
    SWITCH_DB_TUNNEL_ROUTE_DISABLED_INFO,
    SWITCH_DB_WIDE_DATA_EXTENSION_KEY_MODE_INFO,
    SWITCH_DB_WIDE_DATA_EXTENSION_KEY_FFC_BASE_OFFSET_INFO,
    SWITCH_DB_PER_PORT_SPECIAL_LABEL_TERMINATION_DISABLED_INFO,
    SWITCH_DB_PREDEFINED_MPLS_SPECIAL_LABEL_BITMAP_INFO,
    SWITCH_DB_INFO_NOF_ENTRIES
} sw_state_switch_db_layout_e;


extern dnx_sw_state_diagnostic_info_t switch_db_info[SOC_MAX_NUM_DEVICES][SWITCH_DB_INFO_NOF_ENTRIES];

extern const char* switch_db_layout_str[SWITCH_DB_INFO_NOF_ENTRIES];
int switch_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int switch_db_module_verification_dump(
    int unit, int module_verification_idx_0, dnx_sw_state_dump_filters_t filters);

int switch_db_module_error_recovery_dump(
    int unit, int module_error_recovery_idx_0, dnx_sw_state_dump_filters_t filters);

int switch_db_l3mcastl2_ipv4_fwd_type_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int switch_db_l3mcastl2_ipv6_fwd_type_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int switch_db_header_enablers_hashing_dump(
    int unit, int header_enablers_hashing_idx_0, dnx_sw_state_dump_filters_t filters);

int switch_db_tunnel_route_disabled_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int switch_db_wide_data_extension_key_mode_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int switch_db_wide_data_extension_key_ffc_base_offset_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int switch_db_per_port_special_label_termination_disabled_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int switch_db_predefined_mpls_special_label_bitmap_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
