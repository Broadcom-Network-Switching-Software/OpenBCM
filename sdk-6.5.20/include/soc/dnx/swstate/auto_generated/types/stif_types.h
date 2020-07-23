
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __STIF_TYPES_H__
#define __STIF_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <include/bcm/stat.h>
#include <include/bcm/types.h>



typedef struct {
    
    bcm_port_t logical_port;
} dnx_instance_info_t;


typedef struct {
    
    bcm_port_t first_port;
    
    bcm_port_t second_port;
} dnx_source_to_logical_port_mapping_t;


typedef struct {
    
    dnx_instance_info_t** instance;
    
    dnx_source_to_logical_port_mapping_t** source_mapping;
} dnx_stif_db_t;


#endif 
