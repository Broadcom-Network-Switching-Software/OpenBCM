
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_OAM_DIAGNOSTIC_H__
#define __DNX_OAM_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnx/swstate/auto_generated/types/oam_types.h>
#include <include/bcm/oam.h>
#include <include/bcm/types.h>
#include <soc/dnxc/swstate/callbacks/sw_state_ll_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_linked_list.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    OAM_SW_DB_INFO_INFO,
    OAM_SW_DB_INFO_OAM_GROUP_SW_DB_INFO_INFO,
    OAM_SW_DB_INFO_OAM_GROUP_SW_DB_INFO_OAM_GROUP_ARRAY_OF_LINKED_LISTS_INFO,
    OAM_SW_DB_INFO_OAM_ENDPOINT_SW_DB_INFO_INFO,
    OAM_SW_DB_INFO_OAM_ENDPOINT_SW_DB_INFO_OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS_INFO,
    OAM_SW_DB_INFO_OAM_ENDPOINT_SW_DB_INFO_OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS_INFO,
    OAM_SW_DB_INFO_REFLECTOR_INFO,
    OAM_SW_DB_INFO_REFLECTOR_ENCAP_ID_INFO,
    OAM_SW_DB_INFO_REFLECTOR_IS_ALLOCATED_INFO,
    OAM_SW_DB_INFO_INFO_NOF_ENTRIES
} sw_state_oam_sw_db_info_layout_e;


extern dnx_sw_state_diagnostic_info_t oam_sw_db_info_info[SOC_MAX_NUM_DEVICES][OAM_SW_DB_INFO_INFO_NOF_ENTRIES];

extern const char* oam_sw_db_info_layout_str[OAM_SW_DB_INFO_INFO_NOF_ENTRIES];
int oam_sw_db_info_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int oam_sw_db_info_oam_group_sw_db_info_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int oam_sw_db_info_oam_endpoint_sw_db_info_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_mep_linked_lists_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int oam_sw_db_info_reflector_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int oam_sw_db_info_reflector_encap_id_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int oam_sw_db_info_reflector_is_allocated_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
