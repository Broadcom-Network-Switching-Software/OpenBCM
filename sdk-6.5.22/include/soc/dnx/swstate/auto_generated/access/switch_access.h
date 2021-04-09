
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef __SWITCH_ACCESS_H__
#define __SWITCH_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/switch_types.h>



typedef int (*switch_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*switch_db_init_cb)(
    int unit);


typedef int (*switch_db_module_verification_set_cb)(
    int unit, uint32 module_verification_idx_0, uint8 module_verification);


typedef int (*switch_db_module_verification_get_cb)(
    int unit, uint32 module_verification_idx_0, uint8 *module_verification);


typedef int (*switch_db_module_error_recovery_set_cb)(
    int unit, uint32 module_error_recovery_idx_0, uint8 module_error_recovery);


typedef int (*switch_db_module_error_recovery_get_cb)(
    int unit, uint32 module_error_recovery_idx_0, uint8 *module_error_recovery);


typedef int (*switch_db_l3mcastl2_ipv4_fwd_type_set_cb)(
    int unit, uint8 l3mcastl2_ipv4_fwd_type);


typedef int (*switch_db_l3mcastl2_ipv4_fwd_type_get_cb)(
    int unit, uint8 *l3mcastl2_ipv4_fwd_type);


typedef int (*switch_db_l3mcastl2_ipv6_fwd_type_set_cb)(
    int unit, uint8 l3mcastl2_ipv6_fwd_type);


typedef int (*switch_db_l3mcastl2_ipv6_fwd_type_get_cb)(
    int unit, uint8 *l3mcastl2_ipv6_fwd_type);


typedef int (*switch_db_header_enablers_hashing_set_cb)(
    int unit, uint32 header_enablers_hashing_idx_0, uint32 header_enablers_hashing);


typedef int (*switch_db_header_enablers_hashing_get_cb)(
    int unit, uint32 header_enablers_hashing_idx_0, uint32 *header_enablers_hashing);


typedef int (*switch_db_tunnel_route_disabled_ctx_icmp_counter_set_cb)(
    int unit, uint32 icmp_counter_idx_0, int icmp_counter);


typedef int (*switch_db_tunnel_route_disabled_ctx_icmp_counter_get_cb)(
    int unit, uint32 icmp_counter_idx_0, int *icmp_counter);


typedef int (*switch_db_tunnel_route_disabled_ctx_igmp_counter_set_cb)(
    int unit, uint32 igmp_counter_idx_0, int igmp_counter);


typedef int (*switch_db_tunnel_route_disabled_ctx_igmp_counter_get_cb)(
    int unit, uint32 igmp_counter_idx_0, int *igmp_counter);


typedef int (*switch_db_tunnel_route_disabled_ctx_udp_counter_set_cb)(
    int unit, uint32 udp_counter_idx_0, int udp_counter);


typedef int (*switch_db_tunnel_route_disabled_ctx_udp_counter_get_cb)(
    int unit, uint32 udp_counter_idx_0, int *udp_counter);


typedef int (*switch_db_tunnel_route_disabled_ctx_tcp_counter_set_cb)(
    int unit, uint32 tcp_counter_idx_0, int tcp_counter);


typedef int (*switch_db_tunnel_route_disabled_ctx_tcp_counter_get_cb)(
    int unit, uint32 tcp_counter_idx_0, int *tcp_counter);


typedef int (*switch_db_tunnel_route_disabled_ctx_unknown_counter_set_cb)(
    int unit, uint32 unknown_counter_idx_0, int unknown_counter);


typedef int (*switch_db_tunnel_route_disabled_ctx_unknown_counter_get_cb)(
    int unit, uint32 unknown_counter_idx_0, int *unknown_counter);


typedef int (*switch_db_tunnel_route_disabled_ctx_gre_keep_alive_counter_set_cb)(
    int unit, uint32 gre_keep_alive_counter_idx_0, int gre_keep_alive_counter);


typedef int (*switch_db_tunnel_route_disabled_ctx_gre_keep_alive_counter_get_cb)(
    int unit, uint32 gre_keep_alive_counter_idx_0, int *gre_keep_alive_counter);


typedef int (*switch_db_tunnel_route_disabled_ctx_icmp_ctx_set_cb)(
    int unit, uint32 icmp_ctx_idx_0, uint32 icmp_ctx_idx_1, int icmp_ctx);


typedef int (*switch_db_tunnel_route_disabled_ctx_icmp_ctx_get_cb)(
    int unit, uint32 icmp_ctx_idx_0, uint32 icmp_ctx_idx_1, int *icmp_ctx);


typedef int (*switch_db_tunnel_route_disabled_ctx_igmp_ctx_set_cb)(
    int unit, uint32 igmp_ctx_idx_0, uint32 igmp_ctx_idx_1, int igmp_ctx);


typedef int (*switch_db_tunnel_route_disabled_ctx_igmp_ctx_get_cb)(
    int unit, uint32 igmp_ctx_idx_0, uint32 igmp_ctx_idx_1, int *igmp_ctx);


typedef int (*switch_db_tunnel_route_disabled_ctx_udp_ctx_set_cb)(
    int unit, uint32 udp_ctx_idx_0, uint32 udp_ctx_idx_1, int udp_ctx);


typedef int (*switch_db_tunnel_route_disabled_ctx_udp_ctx_get_cb)(
    int unit, uint32 udp_ctx_idx_0, uint32 udp_ctx_idx_1, int *udp_ctx);


typedef int (*switch_db_tunnel_route_disabled_ctx_tcp_ctx_set_cb)(
    int unit, uint32 tcp_ctx_idx_0, uint32 tcp_ctx_idx_1, int tcp_ctx);


typedef int (*switch_db_tunnel_route_disabled_ctx_tcp_ctx_get_cb)(
    int unit, uint32 tcp_ctx_idx_0, uint32 tcp_ctx_idx_1, int *tcp_ctx);


typedef int (*switch_db_tunnel_route_disabled_ctx_unknown_ctx_set_cb)(
    int unit, uint32 unknown_ctx_idx_0, uint32 unknown_ctx_idx_1, int unknown_ctx);


typedef int (*switch_db_tunnel_route_disabled_ctx_unknown_ctx_get_cb)(
    int unit, uint32 unknown_ctx_idx_0, uint32 unknown_ctx_idx_1, int *unknown_ctx);


typedef int (*switch_db_tunnel_route_disabled_ctx_gre_keep_alive_ctx_set_cb)(
    int unit, uint32 gre_keep_alive_ctx_idx_0, uint32 gre_keep_alive_ctx_idx_1, int gre_keep_alive_ctx);


typedef int (*switch_db_tunnel_route_disabled_ctx_gre_keep_alive_ctx_get_cb)(
    int unit, uint32 gre_keep_alive_ctx_idx_0, uint32 gre_keep_alive_ctx_idx_1, int *gre_keep_alive_ctx);


typedef int (*switch_db_tunnel_route_disabled_set_cb)(
    int unit, uint32 tunnel_route_disabled);


typedef int (*switch_db_tunnel_route_disabled_get_cb)(
    int unit, uint32 *tunnel_route_disabled);




typedef struct {
    switch_db_module_verification_set_cb set;
    switch_db_module_verification_get_cb get;
} switch_db_module_verification_cbs;


typedef struct {
    switch_db_module_error_recovery_set_cb set;
    switch_db_module_error_recovery_get_cb get;
} switch_db_module_error_recovery_cbs;


typedef struct {
    switch_db_l3mcastl2_ipv4_fwd_type_set_cb set;
    switch_db_l3mcastl2_ipv4_fwd_type_get_cb get;
} switch_db_l3mcastl2_ipv4_fwd_type_cbs;


typedef struct {
    switch_db_l3mcastl2_ipv6_fwd_type_set_cb set;
    switch_db_l3mcastl2_ipv6_fwd_type_get_cb get;
} switch_db_l3mcastl2_ipv6_fwd_type_cbs;


typedef struct {
    switch_db_header_enablers_hashing_set_cb set;
    switch_db_header_enablers_hashing_get_cb get;
} switch_db_header_enablers_hashing_cbs;


typedef struct {
    switch_db_tunnel_route_disabled_ctx_icmp_counter_set_cb set;
    switch_db_tunnel_route_disabled_ctx_icmp_counter_get_cb get;
} switch_db_tunnel_route_disabled_ctx_icmp_counter_cbs;


typedef struct {
    switch_db_tunnel_route_disabled_ctx_igmp_counter_set_cb set;
    switch_db_tunnel_route_disabled_ctx_igmp_counter_get_cb get;
} switch_db_tunnel_route_disabled_ctx_igmp_counter_cbs;


typedef struct {
    switch_db_tunnel_route_disabled_ctx_udp_counter_set_cb set;
    switch_db_tunnel_route_disabled_ctx_udp_counter_get_cb get;
} switch_db_tunnel_route_disabled_ctx_udp_counter_cbs;


typedef struct {
    switch_db_tunnel_route_disabled_ctx_tcp_counter_set_cb set;
    switch_db_tunnel_route_disabled_ctx_tcp_counter_get_cb get;
} switch_db_tunnel_route_disabled_ctx_tcp_counter_cbs;


typedef struct {
    switch_db_tunnel_route_disabled_ctx_unknown_counter_set_cb set;
    switch_db_tunnel_route_disabled_ctx_unknown_counter_get_cb get;
} switch_db_tunnel_route_disabled_ctx_unknown_counter_cbs;


typedef struct {
    switch_db_tunnel_route_disabled_ctx_gre_keep_alive_counter_set_cb set;
    switch_db_tunnel_route_disabled_ctx_gre_keep_alive_counter_get_cb get;
} switch_db_tunnel_route_disabled_ctx_gre_keep_alive_counter_cbs;


typedef struct {
    switch_db_tunnel_route_disabled_ctx_icmp_ctx_set_cb set;
    switch_db_tunnel_route_disabled_ctx_icmp_ctx_get_cb get;
} switch_db_tunnel_route_disabled_ctx_icmp_ctx_cbs;


typedef struct {
    switch_db_tunnel_route_disabled_ctx_igmp_ctx_set_cb set;
    switch_db_tunnel_route_disabled_ctx_igmp_ctx_get_cb get;
} switch_db_tunnel_route_disabled_ctx_igmp_ctx_cbs;


typedef struct {
    switch_db_tunnel_route_disabled_ctx_udp_ctx_set_cb set;
    switch_db_tunnel_route_disabled_ctx_udp_ctx_get_cb get;
} switch_db_tunnel_route_disabled_ctx_udp_ctx_cbs;


typedef struct {
    switch_db_tunnel_route_disabled_ctx_tcp_ctx_set_cb set;
    switch_db_tunnel_route_disabled_ctx_tcp_ctx_get_cb get;
} switch_db_tunnel_route_disabled_ctx_tcp_ctx_cbs;


typedef struct {
    switch_db_tunnel_route_disabled_ctx_unknown_ctx_set_cb set;
    switch_db_tunnel_route_disabled_ctx_unknown_ctx_get_cb get;
} switch_db_tunnel_route_disabled_ctx_unknown_ctx_cbs;


typedef struct {
    switch_db_tunnel_route_disabled_ctx_gre_keep_alive_ctx_set_cb set;
    switch_db_tunnel_route_disabled_ctx_gre_keep_alive_ctx_get_cb get;
} switch_db_tunnel_route_disabled_ctx_gre_keep_alive_ctx_cbs;


typedef struct {
    
    switch_db_tunnel_route_disabled_ctx_icmp_counter_cbs icmp_counter;
    
    switch_db_tunnel_route_disabled_ctx_igmp_counter_cbs igmp_counter;
    
    switch_db_tunnel_route_disabled_ctx_udp_counter_cbs udp_counter;
    
    switch_db_tunnel_route_disabled_ctx_tcp_counter_cbs tcp_counter;
    
    switch_db_tunnel_route_disabled_ctx_unknown_counter_cbs unknown_counter;
    
    switch_db_tunnel_route_disabled_ctx_gre_keep_alive_counter_cbs gre_keep_alive_counter;
    
    switch_db_tunnel_route_disabled_ctx_icmp_ctx_cbs icmp_ctx;
    
    switch_db_tunnel_route_disabled_ctx_igmp_ctx_cbs igmp_ctx;
    
    switch_db_tunnel_route_disabled_ctx_udp_ctx_cbs udp_ctx;
    
    switch_db_tunnel_route_disabled_ctx_tcp_ctx_cbs tcp_ctx;
    
    switch_db_tunnel_route_disabled_ctx_unknown_ctx_cbs unknown_ctx;
    
    switch_db_tunnel_route_disabled_ctx_gre_keep_alive_ctx_cbs gre_keep_alive_ctx;
} switch_db_tunnel_route_disabled_ctx_cbs;


typedef struct {
    switch_db_tunnel_route_disabled_set_cb set;
    switch_db_tunnel_route_disabled_get_cb get;
} switch_db_tunnel_route_disabled_cbs;


typedef struct {
    switch_db_is_init_cb is_init;
    switch_db_init_cb init;
    
    switch_db_module_verification_cbs module_verification;
    
    switch_db_module_error_recovery_cbs module_error_recovery;
    
    switch_db_l3mcastl2_ipv4_fwd_type_cbs l3mcastl2_ipv4_fwd_type;
    
    switch_db_l3mcastl2_ipv6_fwd_type_cbs l3mcastl2_ipv6_fwd_type;
    
    switch_db_header_enablers_hashing_cbs header_enablers_hashing;
    
    switch_db_tunnel_route_disabled_ctx_cbs tunnel_route_disabled_ctx;
    
    switch_db_tunnel_route_disabled_cbs tunnel_route_disabled;
} switch_db_cbs;





extern switch_db_cbs switch_db;

#endif 
