
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_PORT_NIF_DIAGNOSTIC_H__
#define __DNX_PORT_NIF_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_port_nif_types.h>
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
    DNX_PORT_NIF_DB_ARB_LINK_LIST_NOF_SECTIONS_ALLOCATED_INFO,
    DNX_PORT_NIF_DB_OFT_LINK_LIST_INFO,
    DNX_PORT_NIF_DB_OFT_LINK_LIST_LIST_INFO_INFO,
    DNX_PORT_NIF_DB_OFT_LINK_LIST_LIST_INFO_HEAD_INFO,
    DNX_PORT_NIF_DB_OFT_LINK_LIST_LIST_INFO_TAIL_INFO,
    DNX_PORT_NIF_DB_OFT_LINK_LIST_LIST_INFO_SIZE_INFO,
    DNX_PORT_NIF_DB_OFT_LINK_LIST_LINKING_INFO_INFO,
    DNX_PORT_NIF_DB_OFT_LINK_LIST_LINKING_INFO_NEXT_SECTION_INFO,
    DNX_PORT_NIF_DB_OFT_LINK_LIST_NOF_SECTIONS_ALLOCATED_INFO,
    DNX_PORT_NIF_DB_OFR_LINK_LIST_INFO,
    DNX_PORT_NIF_DB_OFR_LINK_LIST_LIST_INFO_INFO,
    DNX_PORT_NIF_DB_OFR_LINK_LIST_LIST_INFO_HEAD_INFO,
    DNX_PORT_NIF_DB_OFR_LINK_LIST_LIST_INFO_TAIL_INFO,
    DNX_PORT_NIF_DB_OFR_LINK_LIST_LIST_INFO_SIZE_INFO,
    DNX_PORT_NIF_DB_OFR_LINK_LIST_LINKING_INFO_INFO,
    DNX_PORT_NIF_DB_OFR_LINK_LIST_LINKING_INFO_NEXT_SECTION_INFO,
    DNX_PORT_NIF_DB_OFR_LINK_LIST_NOF_SECTIONS_ALLOCATED_INFO,
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

int dnx_port_nif_db_arb_link_list_nof_sections_allocated_dump(
    int unit, int nof_sections_allocated_idx_0, dnx_sw_state_dump_filters_t filters);

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

int dnx_port_nif_db_oft_link_list_nof_sections_allocated_dump(
    int unit, int nof_sections_allocated_idx_0, dnx_sw_state_dump_filters_t filters);

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

int dnx_port_nif_db_ofr_link_list_nof_sections_allocated_dump(
    int unit, int nof_sections_allocated_idx_0, int nof_sections_allocated_idx_1, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
