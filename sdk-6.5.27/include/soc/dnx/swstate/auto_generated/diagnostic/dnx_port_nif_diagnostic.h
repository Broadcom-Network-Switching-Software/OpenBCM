
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_PORT_NIF_DIAGNOSTIC_H__
#define __DNX_PORT_NIF_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_port_nif_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_nif.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_PORT_NIF_DB_INFO,
    DNX_PORT_NIF_DB_ARB_LINK_LIST_INFO,
    DNX_PORT_NIF_DB_ARB_LINK_LIST_LIST_INFO_INFO,
    DNX_PORT_NIF_DB_ARB_LINK_LIST_LIST_INFO_HEAD_INFO,
    DNX_PORT_NIF_DB_ARB_LINK_LIST_LIST_INFO_TAIL_INFO,
    DNX_PORT_NIF_DB_ARB_LINK_LIST_LIST_INFO_SIZE_INFO,
    DNX_PORT_NIF_DB_ARB_LINK_LIST_LINKING_INFO_INFO,
    DNX_PORT_NIF_DB_ARB_LINK_LIST_LINKING_INFO_NEXT_SECTION_INFO,
    DNX_PORT_NIF_DB_ARB_LINK_LIST_ALLOCATED_LIST_INFO_INFO,
    DNX_PORT_NIF_DB_ARB_LINK_LIST_ALLOCATED_LIST_INFO_HEAD_INFO,
    DNX_PORT_NIF_DB_ARB_LINK_LIST_ALLOCATED_LIST_INFO_TAIL_INFO,
    DNX_PORT_NIF_DB_ARB_LINK_LIST_ALLOCATED_LIST_INFO_SIZE_INFO,
    DNX_PORT_NIF_DB_OFT_LINK_LIST_INFO,
    DNX_PORT_NIF_DB_OFT_LINK_LIST_LIST_INFO_INFO,
    DNX_PORT_NIF_DB_OFT_LINK_LIST_LIST_INFO_HEAD_INFO,
    DNX_PORT_NIF_DB_OFT_LINK_LIST_LIST_INFO_TAIL_INFO,
    DNX_PORT_NIF_DB_OFT_LINK_LIST_LIST_INFO_SIZE_INFO,
    DNX_PORT_NIF_DB_OFT_LINK_LIST_LINKING_INFO_INFO,
    DNX_PORT_NIF_DB_OFT_LINK_LIST_LINKING_INFO_NEXT_SECTION_INFO,
    DNX_PORT_NIF_DB_OFT_LINK_LIST_ALLOCATED_LIST_INFO_INFO,
    DNX_PORT_NIF_DB_OFT_LINK_LIST_ALLOCATED_LIST_INFO_HEAD_INFO,
    DNX_PORT_NIF_DB_OFT_LINK_LIST_ALLOCATED_LIST_INFO_TAIL_INFO,
    DNX_PORT_NIF_DB_OFT_LINK_LIST_ALLOCATED_LIST_INFO_SIZE_INFO,
    DNX_PORT_NIF_DB_OFR_LINK_LIST_INFO,
    DNX_PORT_NIF_DB_OFR_LINK_LIST_LIST_INFO_INFO,
    DNX_PORT_NIF_DB_OFR_LINK_LIST_LIST_INFO_HEAD_INFO,
    DNX_PORT_NIF_DB_OFR_LINK_LIST_LIST_INFO_TAIL_INFO,
    DNX_PORT_NIF_DB_OFR_LINK_LIST_LIST_INFO_SIZE_INFO,
    DNX_PORT_NIF_DB_OFR_LINK_LIST_LINKING_INFO_INFO,
    DNX_PORT_NIF_DB_OFR_LINK_LIST_LINKING_INFO_NEXT_SECTION_INFO,
    DNX_PORT_NIF_DB_OFR_LINK_LIST_ALLOCATED_LIST_INFO_INFO,
    DNX_PORT_NIF_DB_OFR_LINK_LIST_ALLOCATED_LIST_INFO_HEAD_INFO,
    DNX_PORT_NIF_DB_OFR_LINK_LIST_ALLOCATED_LIST_INFO_TAIL_INFO,
    DNX_PORT_NIF_DB_OFR_LINK_LIST_ALLOCATED_LIST_INFO_SIZE_INFO,
    DNX_PORT_NIF_DB_ACTIVE_CALENDARS_INFO,
    DNX_PORT_NIF_DB_ACTIVE_CALENDARS_CLIENT_LAST_OBJECTS_INFO,
    DNX_PORT_NIF_DB_ACTIVE_CALENDARS_NOF_CLIENT_SLOTS_INFO,
    DNX_PORT_NIF_DB_ACTIVE_CALENDARS_LAST_CLIENT_INFO,
    DNX_PORT_NIF_DB_G_HAO_ACTIVE_CLIENT_INFO,
    DNX_PORT_NIF_DB_PRIORITY_GROUP_NUM_OF_ENTRIES_INFO,
    DNX_PORT_NIF_DB_INFO_NOF_ENTRIES
} sw_state_dnx_port_nif_db_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_port_nif_db_info[SOC_MAX_NUM_DEVICES][DNX_PORT_NIF_DB_INFO_NOF_ENTRIES];

extern const char* dnx_port_nif_db_layout_str[DNX_PORT_NIF_DB_INFO_NOF_ENTRIES];
int dnx_port_nif_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_arb_link_list_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_arb_link_list_list_info_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_arb_link_list_list_info_head_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_arb_link_list_list_info_tail_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_arb_link_list_list_info_size_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_arb_link_list_linking_info_dump(
    int unit, int linking_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_arb_link_list_linking_info_next_section_dump(
    int unit, int linking_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_arb_link_list_allocated_list_info_dump(
    int unit, int allocated_list_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_arb_link_list_allocated_list_info_head_dump(
    int unit, int allocated_list_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_arb_link_list_allocated_list_info_tail_dump(
    int unit, int allocated_list_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_arb_link_list_allocated_list_info_size_dump(
    int unit, int allocated_list_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_oft_link_list_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_oft_link_list_list_info_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_oft_link_list_list_info_head_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_oft_link_list_list_info_tail_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_oft_link_list_list_info_size_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_oft_link_list_linking_info_dump(
    int unit, int linking_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_oft_link_list_linking_info_next_section_dump(
    int unit, int linking_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_oft_link_list_allocated_list_info_dump(
    int unit, int allocated_list_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_oft_link_list_allocated_list_info_head_dump(
    int unit, int allocated_list_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_oft_link_list_allocated_list_info_tail_dump(
    int unit, int allocated_list_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_oft_link_list_allocated_list_info_size_dump(
    int unit, int allocated_list_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_ofr_link_list_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_ofr_link_list_list_info_dump(
    int unit, int list_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_ofr_link_list_list_info_head_dump(
    int unit, int list_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_ofr_link_list_list_info_tail_dump(
    int unit, int list_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_ofr_link_list_list_info_size_dump(
    int unit, int list_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_ofr_link_list_linking_info_dump(
    int unit, int linking_info_idx_0, int linking_info_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_ofr_link_list_linking_info_next_section_dump(
    int unit, int linking_info_idx_0, int linking_info_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_ofr_link_list_allocated_list_info_dump(
    int unit, int allocated_list_info_idx_0, int allocated_list_info_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_ofr_link_list_allocated_list_info_head_dump(
    int unit, int allocated_list_info_idx_0, int allocated_list_info_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_ofr_link_list_allocated_list_info_tail_dump(
    int unit, int allocated_list_info_idx_0, int allocated_list_info_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_ofr_link_list_allocated_list_info_size_dump(
    int unit, int allocated_list_info_idx_0, int allocated_list_info_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_active_calendars_dump(
    int unit, int active_calendars_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_active_calendars_client_last_objects_dump(
    int unit, int active_calendars_idx_0, int client_last_objects_idx_0, int client_last_objects_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_active_calendars_nof_client_slots_dump(
    int unit, int active_calendars_idx_0, int nof_client_slots_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_active_calendars_last_client_dump(
    int unit, int active_calendars_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_g_hao_active_client_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_port_nif_db_priority_group_num_of_entries_dump(
    int unit, int priority_group_num_of_entries_idx_0, int priority_group_num_of_entries_idx_1, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
