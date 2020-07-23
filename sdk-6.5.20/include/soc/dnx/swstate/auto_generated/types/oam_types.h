
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __OAM_TYPES_H__
#define __OAM_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <include/bcm/oam.h>
#include <include/bcm/types.h>
#include <soc/dnxc/swstate/callbacks/sw_state_ll_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_linked_list.h>



typedef struct {
    
    int encap_id;
    
    int is_allocated;
} oam_reflector_sw_db_info_t;


typedef struct {
    
    sw_state_multihead_ll_t oam_group_array_of_linked_lists;
} oam_group_sw_db_info_t;


typedef struct {
    
    sw_state_multihead_ll_t oam_endpoint_array_of_rmep_linked_lists;
} oam_endpoint_sw_db_info_t;


typedef struct {
    
    oam_group_sw_db_info_t oam_group_sw_db_info;
    
    oam_endpoint_sw_db_info_t oam_endpoint_sw_db_info;
    
    oam_reflector_sw_db_info_t reflector;
    
    uint32 tst_trap_used_cnt;
} oam_sw_db_info_t;


#endif 
