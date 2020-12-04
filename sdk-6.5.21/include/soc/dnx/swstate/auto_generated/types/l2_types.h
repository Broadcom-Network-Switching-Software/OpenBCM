
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __L2_TYPES_H__
#define __L2_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <bcm/switch.h>



typedef struct {
    
    int ipv4uc_counter;
    
    int ipv4mc_counter;
    
    int ipv6uc_counter;
    
    int ipv6mc_counter;
    
    int mpls_counter;
    
    int ipv4uc[64];
    
    int ipv4mc[64];
    
    int ipv6uc[64];
    
    int ipv6mc[64];
    
    int mpls[64];
} opportunistic_learning_ctx_t;


typedef struct {
    
    uint8 fid_profiles_changed;
    
    opportunistic_learning_ctx_t opportunistic_learning_ctx;
    
    uint32 routing_learning;
} l2_db_t;


#endif 
