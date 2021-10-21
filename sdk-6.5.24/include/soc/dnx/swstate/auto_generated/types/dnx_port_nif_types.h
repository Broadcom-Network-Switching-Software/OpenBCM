
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_PORT_NIF_TYPES_H__
#define __DNX_PORT_NIF_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>


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
    
    uint8* is_client_allocated;
} dnx_port_nif_db_link_list_t;


typedef struct {
    
    dnx_port_nif_db_link_list_t arb_link_list;
    
    dnx_port_nif_db_link_list_t oft_link_list;
    
    dnx_port_nif_db_link_list_t ofr_link_list;
} dnx_port_nif_db_t;


#endif 
