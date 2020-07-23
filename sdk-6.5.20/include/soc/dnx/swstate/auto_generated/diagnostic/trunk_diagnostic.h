
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __TRUNK_DIAGNOSTIC_H__
#define __TRUNK_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/trunk_types.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    TRUNK_DB_INFO,
    TRUNK_DB_POOLS_INFO,
    TRUNK_DB_POOLS_GROUPS_INFO,
    TRUNK_DB_POOLS_GROUPS_PSC_INFO,
    TRUNK_DB_POOLS_GROUPS_PSC_PROFILE_ID_INFO,
    TRUNK_DB_POOLS_GROUPS_MEMBERS_INFO,
    TRUNK_DB_POOLS_GROUPS_MEMBERS_SYSTEM_PORT_INFO,
    TRUNK_DB_POOLS_GROUPS_MEMBERS_FLAGS_INFO,
    TRUNK_DB_POOLS_GROUPS_MEMBERS_INDEX_INFO,
    TRUNK_DB_POOLS_GROUPS_NOF_MEMBERS_INFO,
    TRUNK_DB_POOLS_GROUPS_LAST_MEMBER_ADDED_INDEX_INFO,
    TRUNK_DB_POOLS_GROUPS_IN_USE_INFO,
    TRUNK_DB_POOLS_GROUPS_PP_PORTS_INFO,
    TRUNK_DB_POOLS_GROUPS_IN_HEADER_TYPE_INFO,
    TRUNK_DB_POOLS_GROUPS_OUT_HEADER_TYPE_INFO,
    TRUNK_DB_POOLS_GROUPS_FLAGS_INFO,
    TRUNK_DB_POOLS_GROUPS_MASTER_TRUNK_ID_INFO,
    TRUNK_DB_TRUNK_SYSTEM_PORT_DB_INFO,
    TRUNK_DB_TRUNK_SYSTEM_PORT_DB_TRUNK_SYSTEM_PORT_ENTRIES_INFO,
    TRUNK_DB_TRUNK_SYSTEM_PORT_DB_TRUNK_SYSTEM_PORT_ENTRIES_TRUNK_ID_INFO,
    TRUNK_DB_TRUNK_SYSTEM_PORT_DB_TRUNK_SYSTEM_PORT_ENTRIES_FLAGS_INFO,
    TRUNK_DB_INFO_NOF_ENTRIES
} sw_state_trunk_db_layout_e;


extern dnx_sw_state_diagnostic_info_t trunk_db_info[SOC_MAX_NUM_DEVICES][TRUNK_DB_INFO_NOF_ENTRIES];

extern const char* trunk_db_layout_str[TRUNK_DB_INFO_NOF_ENTRIES];
int trunk_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int trunk_db_pools_dump(
    int unit, int pools_idx_0, dnx_sw_state_dump_filters_t filters);

int trunk_db_pools_groups_dump(
    int unit, int pools_idx_0, int groups_idx_0, dnx_sw_state_dump_filters_t filters);

int trunk_db_pools_groups_psc_dump(
    int unit, int pools_idx_0, int groups_idx_0, dnx_sw_state_dump_filters_t filters);

int trunk_db_pools_groups_psc_profile_id_dump(
    int unit, int pools_idx_0, int groups_idx_0, dnx_sw_state_dump_filters_t filters);

int trunk_db_pools_groups_members_dump(
    int unit, int pools_idx_0, int groups_idx_0, int members_idx_0, dnx_sw_state_dump_filters_t filters);

int trunk_db_pools_groups_members_system_port_dump(
    int unit, int pools_idx_0, int groups_idx_0, int members_idx_0, dnx_sw_state_dump_filters_t filters);

int trunk_db_pools_groups_members_flags_dump(
    int unit, int pools_idx_0, int groups_idx_0, int members_idx_0, dnx_sw_state_dump_filters_t filters);

int trunk_db_pools_groups_members_index_dump(
    int unit, int pools_idx_0, int groups_idx_0, int members_idx_0, dnx_sw_state_dump_filters_t filters);

int trunk_db_pools_groups_nof_members_dump(
    int unit, int pools_idx_0, int groups_idx_0, dnx_sw_state_dump_filters_t filters);

int trunk_db_pools_groups_last_member_added_index_dump(
    int unit, int pools_idx_0, int groups_idx_0, dnx_sw_state_dump_filters_t filters);

int trunk_db_pools_groups_in_use_dump(
    int unit, int pools_idx_0, int groups_idx_0, dnx_sw_state_dump_filters_t filters);

int trunk_db_pools_groups_pp_ports_dump(
    int unit, int pools_idx_0, int groups_idx_0, int pp_ports_idx_0, dnx_sw_state_dump_filters_t filters);

int trunk_db_pools_groups_in_header_type_dump(
    int unit, int pools_idx_0, int groups_idx_0, dnx_sw_state_dump_filters_t filters);

int trunk_db_pools_groups_out_header_type_dump(
    int unit, int pools_idx_0, int groups_idx_0, dnx_sw_state_dump_filters_t filters);

int trunk_db_pools_groups_flags_dump(
    int unit, int pools_idx_0, int groups_idx_0, dnx_sw_state_dump_filters_t filters);

int trunk_db_pools_groups_master_trunk_id_dump(
    int unit, int pools_idx_0, int groups_idx_0, dnx_sw_state_dump_filters_t filters);

int trunk_db_trunk_system_port_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int trunk_db_trunk_system_port_db_trunk_system_port_entries_dump(
    int unit, int trunk_system_port_entries_idx_0, dnx_sw_state_dump_filters_t filters);

int trunk_db_trunk_system_port_db_trunk_system_port_entries_trunk_id_dump(
    int unit, int trunk_system_port_entries_idx_0, dnx_sw_state_dump_filters_t filters);

int trunk_db_trunk_system_port_db_trunk_system_port_entries_flags_dump(
    int unit, int trunk_system_port_entries_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
