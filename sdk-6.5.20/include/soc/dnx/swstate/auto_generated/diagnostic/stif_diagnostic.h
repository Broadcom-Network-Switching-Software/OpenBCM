
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __STIF_DIAGNOSTIC_H__
#define __STIF_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/stif_types.h>
#include <include/bcm/stat.h>
#include <include/bcm/types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_STIF_DB_INFO,
    DNX_STIF_DB_INSTANCE_INFO,
    DNX_STIF_DB_INSTANCE_LOGICAL_PORT_INFO,
    DNX_STIF_DB_SOURCE_MAPPING_INFO,
    DNX_STIF_DB_SOURCE_MAPPING_FIRST_PORT_INFO,
    DNX_STIF_DB_SOURCE_MAPPING_SECOND_PORT_INFO,
    DNX_STIF_DB_INFO_NOF_ENTRIES
} sw_state_dnx_stif_db_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_stif_db_info[SOC_MAX_NUM_DEVICES][DNX_STIF_DB_INFO_NOF_ENTRIES];

extern const char* dnx_stif_db_layout_str[DNX_STIF_DB_INFO_NOF_ENTRIES];
int dnx_stif_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_stif_db_instance_dump(
    int unit, int instance_idx_0, int instance_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_stif_db_instance_logical_port_dump(
    int unit, int instance_idx_0, int instance_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_stif_db_source_mapping_dump(
    int unit, int source_mapping_idx_0, int source_mapping_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_stif_db_source_mapping_first_port_dump(
    int unit, int source_mapping_idx_0, int source_mapping_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_stif_db_source_mapping_second_port_dump(
    int unit, int source_mapping_idx_0, int source_mapping_idx_1, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
