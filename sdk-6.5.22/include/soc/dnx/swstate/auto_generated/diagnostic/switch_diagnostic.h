
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef __SWITCH_DIAGNOSTIC_H__
#define __SWITCH_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/switch_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    SWITCH_DB_INFO,
    SWITCH_DB_MODULE_VERIFICATION_INFO,
    SWITCH_DB_MODULE_ERROR_RECOVERY_INFO,
    SWITCH_DB_L3MCASTL2_IPV4_FWD_TYPE_INFO,
    SWITCH_DB_L3MCASTL2_IPV6_FWD_TYPE_INFO,
    SWITCH_DB_HEADER_ENABLERS_HASHING_INFO,
    SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_INFO,
    SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_ICMP_COUNTER_INFO,
    SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_IGMP_COUNTER_INFO,
    SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_UDP_COUNTER_INFO,
    SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_TCP_COUNTER_INFO,
    SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_UNKNOWN_COUNTER_INFO,
    SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_GRE_KEEP_ALIVE_COUNTER_INFO,
    SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_ICMP_CTX_INFO,
    SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_IGMP_CTX_INFO,
    SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_UDP_CTX_INFO,
    SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_TCP_CTX_INFO,
    SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_UNKNOWN_CTX_INFO,
    SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_GRE_KEEP_ALIVE_CTX_INFO,
    SWITCH_DB_TUNNEL_ROUTE_DISABLED_INFO,
    SWITCH_DB_INFO_NOF_ENTRIES
} sw_state_switch_db_layout_e;


extern dnx_sw_state_diagnostic_info_t switch_db_info[SOC_MAX_NUM_DEVICES][SWITCH_DB_INFO_NOF_ENTRIES];

extern const char* switch_db_layout_str[SWITCH_DB_INFO_NOF_ENTRIES];
int switch_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int switch_db_module_verification_dump(
    int unit, int module_verification_idx_0, dnx_sw_state_dump_filters_t filters);

int switch_db_module_error_recovery_dump(
    int unit, int module_error_recovery_idx_0, dnx_sw_state_dump_filters_t filters);

int switch_db_l3mcastl2_ipv4_fwd_type_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int switch_db_l3mcastl2_ipv6_fwd_type_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int switch_db_header_enablers_hashing_dump(
    int unit, int header_enablers_hashing_idx_0, dnx_sw_state_dump_filters_t filters);

int switch_db_tunnel_route_disabled_ctx_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int switch_db_tunnel_route_disabled_ctx_icmp_counter_dump(
    int unit, int icmp_counter_idx_0, dnx_sw_state_dump_filters_t filters);

int switch_db_tunnel_route_disabled_ctx_igmp_counter_dump(
    int unit, int igmp_counter_idx_0, dnx_sw_state_dump_filters_t filters);

int switch_db_tunnel_route_disabled_ctx_udp_counter_dump(
    int unit, int udp_counter_idx_0, dnx_sw_state_dump_filters_t filters);

int switch_db_tunnel_route_disabled_ctx_tcp_counter_dump(
    int unit, int tcp_counter_idx_0, dnx_sw_state_dump_filters_t filters);

int switch_db_tunnel_route_disabled_ctx_unknown_counter_dump(
    int unit, int unknown_counter_idx_0, dnx_sw_state_dump_filters_t filters);

int switch_db_tunnel_route_disabled_ctx_gre_keep_alive_counter_dump(
    int unit, int gre_keep_alive_counter_idx_0, dnx_sw_state_dump_filters_t filters);

int switch_db_tunnel_route_disabled_ctx_icmp_ctx_dump(
    int unit, int icmp_ctx_idx_0, int icmp_ctx_idx_1, dnx_sw_state_dump_filters_t filters);

int switch_db_tunnel_route_disabled_ctx_igmp_ctx_dump(
    int unit, int igmp_ctx_idx_0, int igmp_ctx_idx_1, dnx_sw_state_dump_filters_t filters);

int switch_db_tunnel_route_disabled_ctx_udp_ctx_dump(
    int unit, int udp_ctx_idx_0, int udp_ctx_idx_1, dnx_sw_state_dump_filters_t filters);

int switch_db_tunnel_route_disabled_ctx_tcp_ctx_dump(
    int unit, int tcp_ctx_idx_0, int tcp_ctx_idx_1, dnx_sw_state_dump_filters_t filters);

int switch_db_tunnel_route_disabled_ctx_unknown_ctx_dump(
    int unit, int unknown_ctx_idx_0, int unknown_ctx_idx_1, dnx_sw_state_dump_filters_t filters);

int switch_db_tunnel_route_disabled_ctx_gre_keep_alive_ctx_dump(
    int unit, int gre_keep_alive_ctx_idx_0, int gre_keep_alive_ctx_idx_1, dnx_sw_state_dump_filters_t filters);

int switch_db_tunnel_route_disabled_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
