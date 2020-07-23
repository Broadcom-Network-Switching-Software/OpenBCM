
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FABRIC_DIAGNOSTIC_H__
#define __DNX_FABRIC_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_fabric_types.h>
#include <include/shared/periodic_event.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_FABRIC_DB_INFO,
    DNX_FABRIC_DB_MODIDS_TO_GROUP_MAP_INFO,
    DNX_FABRIC_DB_MODIDS_TO_GROUP_MAP_NOF_MODIDS_INFO,
    DNX_FABRIC_DB_MODIDS_TO_GROUP_MAP_MODIDS_INFO,
    DNX_FABRIC_DB_LINKS_INFO,
    DNX_FABRIC_DB_LINKS_IS_LINK_ALLOWED_INFO,
    DNX_FABRIC_DB_LINKS_IS_LINK_ISOLATED_INFO,
    DNX_FABRIC_DB_LOAD_BALANCING_PERIODIC_EVENT_HANDLER_INFO,
    DNX_FABRIC_DB_INFO_NOF_ENTRIES
} sw_state_dnx_fabric_db_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_fabric_db_info[SOC_MAX_NUM_DEVICES][DNX_FABRIC_DB_INFO_NOF_ENTRIES];

extern const char* dnx_fabric_db_layout_str[DNX_FABRIC_DB_INFO_NOF_ENTRIES];
int dnx_fabric_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_modids_to_group_map_dump(
    int unit, int modids_to_group_map_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_modids_to_group_map_nof_modids_dump(
    int unit, int modids_to_group_map_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_modids_to_group_map_modids_dump(
    int unit, int modids_to_group_map_idx_0, int modids_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_links_dump(
    int unit, int links_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_links_is_link_allowed_dump(
    int unit, int links_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_links_is_link_isolated_dump(
    int unit, int links_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_load_balancing_periodic_event_handler_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
