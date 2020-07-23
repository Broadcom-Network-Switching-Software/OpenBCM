
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_ECGM_DIAGNOSTIC_H__
#define __DNX_ECGM_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_ecgm_types.h>
#include <bcm_int/dnx/algo/ecgm/algo_ecgm.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_ECGM_DB_INFO,
    DNX_ECGM_DB_INTERFACE_CACHING_INFO,
    DNX_ECGM_DB_INTERFACE_CACHING_PORT_INFO,
    DNX_ECGM_DB_INTERFACE_CACHING_INFO_INFO,
    DNX_ECGM_DB_INTERFACE_CACHING_PROFILE_ID_INFO,
    DNX_ECGM_DB_INTERFACE_CACHING_VALID_INFO,
    DNX_ECGM_DB_PORT_CACHING_INFO,
    DNX_ECGM_DB_PORT_CACHING_PORT_INFO,
    DNX_ECGM_DB_PORT_CACHING_INFO_INFO,
    DNX_ECGM_DB_PORT_CACHING_PROFILE_ID_INFO,
    DNX_ECGM_DB_PORT_CACHING_VALID_INFO,
    DNX_ECGM_DB_INFO_NOF_ENTRIES
} sw_state_dnx_ecgm_db_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_ecgm_db_info[SOC_MAX_NUM_DEVICES][DNX_ECGM_DB_INFO_NOF_ENTRIES];

extern const char* dnx_ecgm_db_layout_str[DNX_ECGM_DB_INFO_NOF_ENTRIES];
int dnx_ecgm_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_ecgm_db_interface_caching_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_ecgm_db_interface_caching_port_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_ecgm_db_interface_caching_info_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_ecgm_db_interface_caching_profile_id_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_ecgm_db_interface_caching_valid_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_ecgm_db_port_caching_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_ecgm_db_port_caching_port_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_ecgm_db_port_caching_info_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_ecgm_db_port_caching_profile_id_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_ecgm_db_port_caching_valid_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
