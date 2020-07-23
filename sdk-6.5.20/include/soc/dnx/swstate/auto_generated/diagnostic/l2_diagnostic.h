
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __L2_DIAGNOSTIC_H__
#define __L2_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/l2_types.h>
#include <bcm/switch.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    L2_DB_CONTEXT_INFO,
    L2_DB_CONTEXT_FID_PROFILES_CHANGED_INFO,
    L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_INFO,
    L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_IPV4UC_COUNTER_INFO,
    L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_IPV4MC_COUNTER_INFO,
    L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_IPV6UC_COUNTER_INFO,
    L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_IPV6MC_COUNTER_INFO,
    L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_MPLS_COUNTER_INFO,
    L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_IPV4UC_INFO,
    L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_IPV4MC_INFO,
    L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_IPV6UC_INFO,
    L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_IPV6MC_INFO,
    L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_MPLS_INFO,
    L2_DB_CONTEXT_ROUTING_LEARNING_INFO,
    L2_DB_CONTEXT_INFO_NOF_ENTRIES
} sw_state_l2_db_context_layout_e;


extern dnx_sw_state_diagnostic_info_t l2_db_context_info[SOC_MAX_NUM_DEVICES][L2_DB_CONTEXT_INFO_NOF_ENTRIES];

extern const char* l2_db_context_layout_str[L2_DB_CONTEXT_INFO_NOF_ENTRIES];
int l2_db_context_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int l2_db_context_fid_profiles_changed_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int l2_db_context_opportunistic_learning_ctx_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int l2_db_context_opportunistic_learning_ctx_ipv4uc_counter_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int l2_db_context_opportunistic_learning_ctx_ipv4mc_counter_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int l2_db_context_opportunistic_learning_ctx_ipv6uc_counter_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int l2_db_context_opportunistic_learning_ctx_ipv6mc_counter_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int l2_db_context_opportunistic_learning_ctx_mpls_counter_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int l2_db_context_opportunistic_learning_ctx_ipv4uc_dump(
    int unit, int ipv4uc_idx_0, dnx_sw_state_dump_filters_t filters);

int l2_db_context_opportunistic_learning_ctx_ipv4mc_dump(
    int unit, int ipv4mc_idx_0, dnx_sw_state_dump_filters_t filters);

int l2_db_context_opportunistic_learning_ctx_ipv6uc_dump(
    int unit, int ipv6uc_idx_0, dnx_sw_state_dump_filters_t filters);

int l2_db_context_opportunistic_learning_ctx_ipv6mc_dump(
    int unit, int ipv6mc_idx_0, dnx_sw_state_dump_filters_t filters);

int l2_db_context_opportunistic_learning_ctx_mpls_dump(
    int unit, int mpls_idx_0, dnx_sw_state_dump_filters_t filters);

int l2_db_context_routing_learning_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
