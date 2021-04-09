
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef __SWITCH_TYPES_H__
#define __SWITCH_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <bcm/switch.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>



typedef struct {
    
    int icmp_counter[4];
    
    int igmp_counter[4];
    
    int udp_counter[4];
    
    int tcp_counter[4];
    
    int unknown_counter[4];
    
    int gre_keep_alive_counter[4];
    
    int icmp_ctx[4][7];
    
    int igmp_ctx[4][7];
    
    int udp_ctx[4][7];
    
    int tcp_ctx[4][7];
    
    int unknown_ctx[4][7];
    
    int gre_keep_alive_ctx[4][14];
} tunnel_route_disabled_ctx_t;


typedef struct {
    
    uint8 module_verification[bcmModuleCount];
    
    uint8 module_error_recovery[bcmModuleCount];
    
    uint8 l3mcastl2_ipv4_fwd_type;
    
    uint8 l3mcastl2_ipv6_fwd_type;
    
    uint32 header_enablers_hashing[DBAL_NOF_ENUM_HASH_FIELD_ENABLERS_HEADER_VALUES];
    
    tunnel_route_disabled_ctx_t tunnel_route_disabled_ctx;
    
    uint32 tunnel_route_disabled;
} switch_sw_state_t;


#endif 
