
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_PORT_NIF_TYPES_H__
#define __DNX_PORT_NIF_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_nif.h>


typedef struct {
    int head;
    int tail;
    int size;
} dnx_port_nif_db_list_info_t;

typedef struct {
    int next_section;
} dnx_port_nif_db_linking_info_t;

typedef struct {
    dnx_port_nif_db_list_info_t list_info;
    dnx_port_nif_db_linking_info_t* linking_info;
    dnx_port_nif_db_list_info_t* allocated_list_info;
} dnx_port_nif_db_link_list_t;

typedef struct {
    dnx_port_nif_db_list_info_t* list_info;
    dnx_port_nif_db_linking_info_t** linking_info;
    dnx_port_nif_db_list_info_t** allocated_list_info;
} dnx_port_nif_db_double_priority_link_list_t;

typedef struct {
    uint32** client_last_objects;
    uint32* nof_client_slots;
    uint32 last_client;
} dnx_port_nif_db_calendar_data_t;

typedef struct {
    dnx_port_nif_db_link_list_t arb_link_list;
    dnx_port_nif_db_link_list_t oft_link_list;
    dnx_port_nif_db_double_priority_link_list_t ofr_link_list;
    dnx_port_nif_db_calendar_data_t* active_calendars;
    int g_hao_active_client;
    int priority_group_num_of_entries[DNX_DATA_MAX_NIF_ETH_PRIORITY_GROUPS_NOF][DNX_DATA_MAX_NIF_OFR_NOF_RMC_PER_PRIORITY_GROUP];
} dnx_port_nif_db_t;


#endif 
