
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef __FLOW_TYPES_H__
#define __FLOW_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <include/bcm/types.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>



typedef struct {
    
    dbal_tables_e dbal_table;
    
    uint8 encap_access;
} flow_app_encap_info_t;


typedef struct {
    
    uint8 is_verify_disabled;
    
    uint8 is_error_recovery_disabled;
} dnx_flow_app_common_config_t;


typedef struct {
    
    dnx_flow_app_common_config_t* flow_application_info;
    sw_state_htbl_t valid_phases_per_dbal_table;
    sw_state_htbl_t dbal_table_to_valid_result_types;
} flow_sw_state_t;


#endif 
