
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __L2_ACCESS_H__
#define __L2_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/l2_types.h>
#include <bcm/switch.h>



typedef int (*l2_db_context_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*l2_db_context_init_cb)(
    int unit);


typedef int (*l2_db_context_fid_profiles_changed_set_cb)(
    int unit, uint8 fid_profiles_changed);


typedef int (*l2_db_context_fid_profiles_changed_get_cb)(
    int unit, uint8 *fid_profiles_changed);


typedef int (*l2_db_context_opportunistic_learning_ctx_ipv4uc_counter_set_cb)(
    int unit, int ipv4uc_counter);


typedef int (*l2_db_context_opportunistic_learning_ctx_ipv4uc_counter_get_cb)(
    int unit, int *ipv4uc_counter);


typedef int (*l2_db_context_opportunistic_learning_ctx_ipv4mc_counter_set_cb)(
    int unit, int ipv4mc_counter);


typedef int (*l2_db_context_opportunistic_learning_ctx_ipv4mc_counter_get_cb)(
    int unit, int *ipv4mc_counter);


typedef int (*l2_db_context_opportunistic_learning_ctx_ipv6uc_counter_set_cb)(
    int unit, int ipv6uc_counter);


typedef int (*l2_db_context_opportunistic_learning_ctx_ipv6uc_counter_get_cb)(
    int unit, int *ipv6uc_counter);


typedef int (*l2_db_context_opportunistic_learning_ctx_ipv6mc_counter_set_cb)(
    int unit, int ipv6mc_counter);


typedef int (*l2_db_context_opportunistic_learning_ctx_ipv6mc_counter_get_cb)(
    int unit, int *ipv6mc_counter);


typedef int (*l2_db_context_opportunistic_learning_ctx_mpls_counter_set_cb)(
    int unit, int mpls_counter);


typedef int (*l2_db_context_opportunistic_learning_ctx_mpls_counter_get_cb)(
    int unit, int *mpls_counter);


typedef int (*l2_db_context_opportunistic_learning_ctx_ipv4uc_set_cb)(
    int unit, uint32 ipv4uc_idx_0, int ipv4uc);


typedef int (*l2_db_context_opportunistic_learning_ctx_ipv4uc_get_cb)(
    int unit, uint32 ipv4uc_idx_0, int *ipv4uc);


typedef int (*l2_db_context_opportunistic_learning_ctx_ipv4mc_set_cb)(
    int unit, uint32 ipv4mc_idx_0, int ipv4mc);


typedef int (*l2_db_context_opportunistic_learning_ctx_ipv4mc_get_cb)(
    int unit, uint32 ipv4mc_idx_0, int *ipv4mc);


typedef int (*l2_db_context_opportunistic_learning_ctx_ipv6uc_set_cb)(
    int unit, uint32 ipv6uc_idx_0, int ipv6uc);


typedef int (*l2_db_context_opportunistic_learning_ctx_ipv6uc_get_cb)(
    int unit, uint32 ipv6uc_idx_0, int *ipv6uc);


typedef int (*l2_db_context_opportunistic_learning_ctx_ipv6mc_set_cb)(
    int unit, uint32 ipv6mc_idx_0, int ipv6mc);


typedef int (*l2_db_context_opportunistic_learning_ctx_ipv6mc_get_cb)(
    int unit, uint32 ipv6mc_idx_0, int *ipv6mc);


typedef int (*l2_db_context_opportunistic_learning_ctx_mpls_set_cb)(
    int unit, uint32 mpls_idx_0, int mpls);


typedef int (*l2_db_context_opportunistic_learning_ctx_mpls_get_cb)(
    int unit, uint32 mpls_idx_0, int *mpls);


typedef int (*l2_db_context_routing_learning_set_cb)(
    int unit, uint32 routing_learning);


typedef int (*l2_db_context_routing_learning_get_cb)(
    int unit, uint32 *routing_learning);




typedef struct {
    l2_db_context_fid_profiles_changed_set_cb set;
    l2_db_context_fid_profiles_changed_get_cb get;
} l2_db_context_fid_profiles_changed_cbs;


typedef struct {
    l2_db_context_opportunistic_learning_ctx_ipv4uc_counter_set_cb set;
    l2_db_context_opportunistic_learning_ctx_ipv4uc_counter_get_cb get;
} l2_db_context_opportunistic_learning_ctx_ipv4uc_counter_cbs;


typedef struct {
    l2_db_context_opportunistic_learning_ctx_ipv4mc_counter_set_cb set;
    l2_db_context_opportunistic_learning_ctx_ipv4mc_counter_get_cb get;
} l2_db_context_opportunistic_learning_ctx_ipv4mc_counter_cbs;


typedef struct {
    l2_db_context_opportunistic_learning_ctx_ipv6uc_counter_set_cb set;
    l2_db_context_opportunistic_learning_ctx_ipv6uc_counter_get_cb get;
} l2_db_context_opportunistic_learning_ctx_ipv6uc_counter_cbs;


typedef struct {
    l2_db_context_opportunistic_learning_ctx_ipv6mc_counter_set_cb set;
    l2_db_context_opportunistic_learning_ctx_ipv6mc_counter_get_cb get;
} l2_db_context_opportunistic_learning_ctx_ipv6mc_counter_cbs;


typedef struct {
    l2_db_context_opportunistic_learning_ctx_mpls_counter_set_cb set;
    l2_db_context_opportunistic_learning_ctx_mpls_counter_get_cb get;
} l2_db_context_opportunistic_learning_ctx_mpls_counter_cbs;


typedef struct {
    l2_db_context_opportunistic_learning_ctx_ipv4uc_set_cb set;
    l2_db_context_opportunistic_learning_ctx_ipv4uc_get_cb get;
} l2_db_context_opportunistic_learning_ctx_ipv4uc_cbs;


typedef struct {
    l2_db_context_opportunistic_learning_ctx_ipv4mc_set_cb set;
    l2_db_context_opportunistic_learning_ctx_ipv4mc_get_cb get;
} l2_db_context_opportunistic_learning_ctx_ipv4mc_cbs;


typedef struct {
    l2_db_context_opportunistic_learning_ctx_ipv6uc_set_cb set;
    l2_db_context_opportunistic_learning_ctx_ipv6uc_get_cb get;
} l2_db_context_opportunistic_learning_ctx_ipv6uc_cbs;


typedef struct {
    l2_db_context_opportunistic_learning_ctx_ipv6mc_set_cb set;
    l2_db_context_opportunistic_learning_ctx_ipv6mc_get_cb get;
} l2_db_context_opportunistic_learning_ctx_ipv6mc_cbs;


typedef struct {
    l2_db_context_opportunistic_learning_ctx_mpls_set_cb set;
    l2_db_context_opportunistic_learning_ctx_mpls_get_cb get;
} l2_db_context_opportunistic_learning_ctx_mpls_cbs;


typedef struct {
    
    l2_db_context_opportunistic_learning_ctx_ipv4uc_counter_cbs ipv4uc_counter;
    
    l2_db_context_opportunistic_learning_ctx_ipv4mc_counter_cbs ipv4mc_counter;
    
    l2_db_context_opportunistic_learning_ctx_ipv6uc_counter_cbs ipv6uc_counter;
    
    l2_db_context_opportunistic_learning_ctx_ipv6mc_counter_cbs ipv6mc_counter;
    
    l2_db_context_opportunistic_learning_ctx_mpls_counter_cbs mpls_counter;
    
    l2_db_context_opportunistic_learning_ctx_ipv4uc_cbs ipv4uc;
    
    l2_db_context_opportunistic_learning_ctx_ipv4mc_cbs ipv4mc;
    
    l2_db_context_opportunistic_learning_ctx_ipv6uc_cbs ipv6uc;
    
    l2_db_context_opportunistic_learning_ctx_ipv6mc_cbs ipv6mc;
    
    l2_db_context_opportunistic_learning_ctx_mpls_cbs mpls;
} l2_db_context_opportunistic_learning_ctx_cbs;


typedef struct {
    l2_db_context_routing_learning_set_cb set;
    l2_db_context_routing_learning_get_cb get;
} l2_db_context_routing_learning_cbs;


typedef struct {
    l2_db_context_is_init_cb is_init;
    l2_db_context_init_cb init;
    
    l2_db_context_fid_profiles_changed_cbs fid_profiles_changed;
    
    l2_db_context_opportunistic_learning_ctx_cbs opportunistic_learning_ctx;
    
    l2_db_context_routing_learning_cbs routing_learning;
} l2_db_context_cbs;





extern l2_db_context_cbs l2_db_context;

#endif 
