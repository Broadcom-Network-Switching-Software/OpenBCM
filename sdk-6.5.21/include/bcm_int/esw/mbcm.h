/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        mbcm.h
 * Purpose:     Multiplexing of the bcm layer
 *
 * Different chip families require such different implementations
 * of some basic BCM layer functionality that the functions are
 * multiplexed to allow a fast runtime decision as to which function
 * to call.  This file contains the basic declarations for this
 * process.
 *
 * See internal/design/soft_arch/xgs_plan.txt for more info.
 *
 * Conventions:
 *    MBCM is the multiplexed bcm prefix
 *    _f is the function type declaration postfix
 */

#ifndef	_BCM_INT_MBCM_H_
#define	_BCM_INT_MBCM_H_

#include <soc/macipadr.h>
#include <soc/drv.h>

#include <bcm/l2.h>
#include <bcm/port.h>
#include <bcm/l3.h>
#include <bcm/trunk.h>
#include <bcm/ipmc.h>
#include <bcm/mcast.h>
#include <bcm/vlan.h>
#include <bcm/switch.h>
#include <bcm/mpls.h>
#include <bcm/mim.h>
#include <bcm/wlan.h>
#include <bcm/subport.h>
#include <bcm/multicast.h>
#include <bcm/cosq.h>
#include <bcm/bregex.h>

#include <bcm_int/esw/port.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/trunk.h>

#include <bcm_int/common/family.h>

/****************************************************************
 *
 * Type definitions for multiplexed BCM functions.
 */
/* L2 functions */
typedef int (*mbcm_l2_init_f)(int);
typedef int (*mbcm_l2_term_f)(int);
typedef int (*mbcm_l2_addr_get_f)(int, bcm_mac_t, bcm_vlan_t,
				  bcm_l2_addr_t *);
typedef int (*mbcm_l2_addr_add_f)(int, bcm_l2_addr_t *);
typedef int (*mbcm_l2_addr_delete_f)(int, bcm_mac_t, bcm_vlan_t);
typedef int (*mbcm_l2_addr_delete_by_mac_f)(int, bcm_mac_t, uint32);
typedef int (*mbcm_l2_addr_delete_by_mac_port_f)(int, bcm_mac_t,
						 bcm_module_t, bcm_port_t,
						 uint32);
typedef int (*mbcm_l2_addr_register_f)(int, bcm_l2_addr_callback_t, void*);
typedef int (*mbcm_l2_addr_unregister_f)(int, bcm_l2_addr_callback_t, void*);
typedef int (*mbcm_l2_conflict_get_f)(int, bcm_l2_addr_t *,
                                      bcm_l2_addr_t *, int, int *);

/* Port table related functions */
typedef int (*mbcm_port_cfg_get_f)(int, bcm_port_t, bcm_port_cfg_t *);
typedef int (*mbcm_port_cfg_set_f)(int, bcm_port_t, bcm_port_cfg_t *);
typedef int (*mbcm_port_cfg_init_f)(int, bcm_port_t, bcm_vlan_data_t *);

/* VLAN functions */
typedef int (*mbcm_vlan_init_f)(int, bcm_vlan_data_t *);
typedef int (*mbcm_vlan_reload_f)(int, vbmp_t *, int *);
typedef int (*mbcm_vlan_create_f)(int, bcm_vlan_t);
typedef int (*mbcm_vlan_destroy_f)(int, bcm_vlan_t);
typedef int (*mbcm_vlan_port_add_f)(int, bcm_vlan_t, bcm_pbmp_t, bcm_pbmp_t,
                                    bcm_pbmp_t);
typedef int (*mbcm_vlan_port_remove_f)(int, bcm_vlan_t, bcm_pbmp_t, bcm_pbmp_t,
                                       bcm_pbmp_t);
typedef int (*mbcm_vlan_port_get_f)(int, bcm_vlan_t, bcm_pbmp_t *,
				    bcm_pbmp_t *, bcm_pbmp_t *);
typedef int (*mbcm_vlan_stg_get_f)(int, bcm_vlan_t, bcm_stg_t *);
typedef int (*mbcm_vlan_stg_set_f)(int, bcm_vlan_t, bcm_stg_t);

/* trunking function */
typedef int (*mbcm_trunk_modify_f)(int, bcm_trunk_t,
				bcm_trunk_info_t *, int, bcm_trunk_member_t *,
                                trunk_private_t *, int, bcm_trunk_member_t *);
typedef int (*mbcm_trunk_get_f)(int, bcm_trunk_t,
				bcm_trunk_info_t *, int, bcm_trunk_member_t *,
                                int *, trunk_private_t *);
typedef int (*mbcm_trunk_destroy_f)(int, bcm_trunk_t, trunk_private_t *);
typedef int (*mbcm_trunk_mcast_join_f)(int, bcm_trunk_t,
				       bcm_vlan_t, bcm_mac_t,
				       trunk_private_t *);

/* STG functions */
typedef int (*mbcm_stg_stp_init_f)(int, bcm_stg_t);
typedef int (*mbcm_stg_stp_get_f)(int, bcm_stg_t, bcm_port_t, int *);
typedef int (*mbcm_stg_stp_set_f)(int, bcm_stg_t, bcm_port_t, int);

/* Multicast functions */
typedef int (*mbcm_mcast_addr_add_f)(int, bcm_mcast_addr_t *);
typedef int (*mbcm_mcast_addr_remove_f)(int, bcm_mac_t, bcm_vlan_t);
typedef int (*mbcm_mcast_port_get_f)(int, bcm_mac_t, bcm_vlan_t,
				     bcm_mcast_addr_t *);
typedef int (*mbcm_mcast_init_f)(int);
typedef int (*mbcm_mcast_detach_f)(int);
typedef int (*mbcm_mcast_addr_add_w_l2mcindex_f)(int, bcm_mcast_addr_t *);
typedef int (*mbcm_mcast_addr_remove_w_l2mcindex_f)(int, bcm_mcast_addr_t *);
typedef int (*mbcm_mcast_port_add_f)(int, bcm_mcast_addr_t *);
typedef int (*mbcm_mcast_port_remove_f)(int, bcm_mcast_addr_t *);

/* COSQ functions */
typedef int (*mbcm_cosq_init_f)(int unit);
typedef int (*mbcm_cosq_detach_f)(int unit, int software_state_only);
typedef int (*mbcm_cosq_config_set_f)(int unit, int numq);
typedef int (*mbcm_cosq_config_get_f)(int unit, int *numq);
typedef int (*mbcm_cosq_mapping_set_f)(int unit,
				       bcm_port_t port,
				       bcm_cos_t priority,
				       bcm_cos_queue_t cosq);
typedef int (*mbcm_cosq_mapping_get_f)(int unit,
				       bcm_port_t port,
				       bcm_cos_t priority,
				       bcm_cos_queue_t *cosq);
typedef int (*mbcm_cosq_port_sched_set_f)(int unit, bcm_pbmp_t, int mode,
                                   const int weights[], int delay);
typedef int (*mbcm_cosq_port_sched_get_f)(int unit, bcm_pbmp_t, int *mode,
                                   int weights[], int *delay);
typedef int (*mbcm_cosq_sched_weight_max_get_f)(int unit, int mode,
                                                     int *weight_max);
typedef int (*mbcm_cosq_port_bandwidth_set_f)(int unit, bcm_port_t port,
                                              bcm_cos_queue_t cosq,
                                              uint32 kbits_sec_min,
                                              uint32 kbits_sec_max,
                                              uint32 kbits_sec_burst,
                                              uint32 flags);
typedef int (*mbcm_cosq_port_bandwidth_get_f)(int unit, bcm_port_t port,
                                              bcm_cos_queue_t cosq,
                                              uint32 *kbits_sec_min,
                                              uint32 *kbits_sec_max,
                                              uint32 *kbits_sec_burst,
                                              uint32 *flags);
typedef int (*mbcm_cosq_discard_set_f)(int unit, uint32 flags);
typedef int (*mbcm_cosq_discard_get_f)(int unit, uint32 *flags);
typedef int (*mbcm_cosq_discard_port_set_f)(int unit, bcm_port_t port,
                                            bcm_cos_queue_t cosq,
                                            uint32 color,
                                            int drop_start,
                                            int drop_slope,
                                            int average_time);
typedef int (*mbcm_cosq_discard_port_get_f)(int unit, bcm_port_t port,
                                            bcm_cos_queue_t cosq,
                                            uint32 color,
                                            int *drop_start,
                                            int *drop_slope,
                                            int *average_time);
#ifdef BCM_WARM_BOOT_SUPPORT
typedef int (*mbcm_cosq_sync_f)(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifndef BCM_SW_STATE_DUMP_DISABLE
typedef void (*mbcm_cosq_sw_dump_f)(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/* Port meter functions */
typedef int (*mbcm_port_rate_egress_set_f)(int unit, int port, 
					   uint32 kbits_sec,
					   uint32 kbits_burst,
                       uint32 flags);
typedef int (*mbcm_port_rate_egress_get_f)(int unit, int port, 
					   uint32 *kbits_sec,
					   uint32 *kbits_burst,
                       uint32 *flags);

#ifdef INCLUDE_L3
/* L3 functions */
typedef int (*mbcm_l3_tables_init_f)(int);
typedef int (*mbcm_l3_tables_cleanup_f)(int);
typedef int (*mbcm_l3_enable_f)(int, int);
typedef int (*mbcm_l3_intf_get_f)(int, _bcm_l3_intf_cfg_t *);
typedef int (*mbcm_l3_intf_get_by_vid_f)(int, _bcm_l3_intf_cfg_t *);
typedef int (*mbcm_l3_intf_create_f)(int, _bcm_l3_intf_cfg_t *);
typedef int (*mbcm_l3_intf_id_create_f)(int, _bcm_l3_intf_cfg_t *);
typedef int (*mbcm_l3_intf_lookup_f)(int, _bcm_l3_intf_cfg_t *);
typedef int (*mbcm_l3_intf_delete_f)(int, _bcm_l3_intf_cfg_t *);
typedef int (*mbcm_l3_intf_delete_all_f)(int);

typedef int (*mbcm_l3_ip4_get_f)(int, _bcm_l3_cfg_t *);
typedef int (*mbcm_l3_ip4_add_f)(int, _bcm_l3_cfg_t *);
typedef int (*mbcm_l3_ip4_delete_f)(int, _bcm_l3_cfg_t *);
typedef int (*mbcm_l3_ip4_delete_prefix_f)(int, _bcm_l3_cfg_t *);
typedef int (*mbcm_l3_delete_intf_f)(int, _bcm_l3_cfg_t *, int);
typedef int (*mbcm_l3_delete_all_f)(int);
typedef int (*mbcm_l3_ip4_replace_f)(int, _bcm_l3_cfg_t *);

typedef int (*mbcm_l3_ip6_get_f)(int, _bcm_l3_cfg_t *);
typedef int (*mbcm_l3_ip6_add_f)(int, _bcm_l3_cfg_t *);
typedef int (*mbcm_l3_ip6_delete_f)(int, _bcm_l3_cfg_t *);
typedef int (*mbcm_l3_ip6_delete_prefix_f)(int, _bcm_l3_cfg_t *);
typedef int (*mbcm_l3_ip6_replace_f)(int, _bcm_l3_cfg_t *);

typedef int (*mbcm_ip4_defip_cfg_get_f)(int, _bcm_defip_cfg_t *);
typedef int (*mbcm_ip4_defip_ecmp_get_all_f)(int, _bcm_defip_cfg_t *,
                                             bcm_l3_route_t *, int, int *);
typedef int (*mbcm_ip4_defip_add_f)(int, _bcm_defip_cfg_t *);
typedef int (*mbcm_ip4_defip_delete_f)(int, _bcm_defip_cfg_t *);
typedef int (*mbcm_defip_delete_intf_f)(int, _bcm_defip_cfg_t *, int);
typedef int (*mbcm_defip_delete_all_f)(int);

typedef int (*mbcm_ip6_defip_cfg_get_f)(int, _bcm_defip_cfg_t *);
typedef int (*mbcm_ip6_defip_ecmp_get_all_f)(int, _bcm_defip_cfg_t *,
                                             bcm_l3_route_t *, int, int *);
typedef int (*mbcm_ip6_defip_add_f)(int, _bcm_defip_cfg_t *);
typedef int (*mbcm_ip6_defip_delete_f)(int, _bcm_defip_cfg_t *);

typedef int (*mbcm_l3_conflict_get_f)(int, bcm_l3_key_t*, bcm_l3_key_t*,
				      int, int *);
typedef int (*mbcm_l3_host_age_f)(int, uint32, bcm_l3_host_traverse_cb, void *);
typedef int (*mbcm_l3_traverse_f)(int, uint32, uint32, uint32,
				  bcm_l3_host_traverse_cb, void*);
typedef int (*mbcm_defip_age_f)(int, bcm_l3_route_traverse_cb, void *);
typedef int (*mbcm_defip_traverse_f)(int, uint32, uint32,
				     bcm_l3_route_traverse_cb, void *);
typedef int (*mbcm_l3_info_get_f)(int, bcm_l3_info_t *);

/* IPMC functions */
typedef int (*mbcm_ipmc_init_f)(int);
typedef int (*mbcm_ipmc_detach_f)(int);
typedef int (*mbcm_ipmc_enable_f)(int, int);
typedef int (*mbcm_ipmc_src_port_check_f)(int, int);
typedef int (*mbcm_ipmc_src_ip_search_f)(int, int);
typedef int (*mbcm_ipmc_add_f)(int, bcm_ipmc_addr_t *);
typedef int (*mbcm_ipmc_delete_f)(int, bcm_ipmc_addr_t *);
typedef int (*mbcm_ipmc_delete_all_f)(int);
typedef int (*mbcm_ipmc_lookup_f)(int, int *, bcm_ipmc_addr_t *);
typedef int (*mbcm_ipmc_get_f)(int, int, bcm_ipmc_addr_t *);
typedef int (*mbcm_ipmc_put_f)(int, int, bcm_ipmc_addr_t *);
typedef int (*mbcm_ipmc_egress_port_get_f)(int, bcm_port_t, sal_mac_addr_t,
                                           int *, bcm_vlan_t *, int *);
typedef int (*mbcm_ipmc_egress_port_set_f)(int, bcm_port_t, const bcm_mac_t,
                                           int, bcm_vlan_t, int);

/* IPMC VLAN Replication functions */
typedef int (*mbcm_ipmc_repl_init_f)(int);
typedef int (*mbcm_ipmc_repl_detach_f)(int);
typedef int (*mbcm_ipmc_repl_get_f)(int, int, bcm_port_t, bcm_vlan_vector_t);
typedef int (*mbcm_ipmc_repl_add_f)(int, int, bcm_port_t, bcm_vlan_t);
typedef int (*mbcm_ipmc_repl_delete_f)(int, int, bcm_port_t, bcm_vlan_t);
typedef int (*mbcm_ipmc_repl_delete_all_f)(int, int, bcm_port_t);
typedef int (*mbcm_ipmc_egress_intf_add_f)(int, int, bcm_port_t, 
                                           bcm_l3_intf_t *);
typedef int (*mbcm_ipmc_egress_intf_delete_f)(int, int, bcm_port_t, 
                                              bcm_l3_intf_t *);
typedef int (*mbcm_ipmc_age_f)(int, uint32, bcm_ipmc_traverse_cb, void *);
typedef int (*mbcm_ipmc_traverse_f)(int, uint32, bcm_ipmc_traverse_cb, void *);
#endif /* INCLUDE_L3 */

#ifdef INCLUDE_REGEX
typedef int (*mbcm_regex_config_set_f)(int, bcm_regex_config_t*);
typedef int (*mbcm_regex_config_get_f)(int, bcm_regex_config_t*);
typedef int (*mbcm_regex_exclude_add_f)(int, uint8, uint16, uint16);
typedef int (*mbcm_regex_exclude_delete_f)(int, uint8, uint16, uint16);
typedef int (*mbcm_regex_exclude_delete_all_f)(int);
typedef int (*mbcm_regex_exclude_get_f)(int,int,uint8*,uint16*,uint16*,int*);
typedef int (*mbcm_regex_engine_create_f)(int,bcm_regex_engine_config_t*,
                                            bcm_regex_engine_t*);
typedef int (*mbcm_regex_engine_destroy_f)(int,bcm_regex_engine_t);
typedef int (*mbcm_regex_engine_traverse_f)(int,bcm_regex_engine_traverse_cb,void*);
typedef int (*mbcm_regex_engine_get_f)(int,bcm_regex_engine_t,
                                        bcm_regex_engine_config_t*);
typedef int (*bcm_regex_match_check_f)(int, int, bcm_regex_match_t*, int*);

typedef int (*bcm_regex_match_set_f)(int, bcm_regex_engine_t, int, bcm_regex_match_t*);
typedef int (*bcm_regex_report_register_f)(int, uint32, bcm_regex_report_cb, void *);
typedef int (*bcm_regex_report_unregister_f)(int, uint32, bcm_regex_report_cb, void*);
#endif /* INCLUDE_REGEX */

/****************************************************************
 *
 * Multiplexed BCM Driver Structure
 */
typedef struct mbcm_functions_s {
    /* L2 functions */
    mbcm_l2_init_f		    mbcm_l2_init;
    mbcm_l2_term_f		    mbcm_l2_term;
    mbcm_l2_addr_get_f              mbcm_l2_addr_get;
    mbcm_l2_addr_add_f              mbcm_l2_addr_add;
    mbcm_l2_addr_delete_f		mbcm_l2_addr_delete;
    mbcm_l2_conflict_get_f	    mbcm_l2_conflict_get;

    /* Port table related functions */
    mbcm_port_cfg_init_f	    mbcm_port_cfg_init;
    mbcm_port_cfg_get_f             mbcm_port_cfg_get;
    mbcm_port_cfg_set_f             mbcm_port_cfg_set;

    /* VLAN functions */
    mbcm_vlan_init_f		    mbcm_vlan_init;
    mbcm_vlan_reload_f		    mbcm_vlan_reload;
    mbcm_vlan_create_f              mbcm_vlan_create;
    mbcm_vlan_destroy_f             mbcm_vlan_destroy;
    mbcm_vlan_port_add_f            mbcm_vlan_port_add;
    mbcm_vlan_port_remove_f         mbcm_vlan_port_remove;
    mbcm_vlan_port_get_f            mbcm_vlan_port_get;
    mbcm_vlan_stg_get_f             mbcm_vlan_stg_get;
    mbcm_vlan_stg_set_f             mbcm_vlan_stg_set;

    /* trunking */
    mbcm_trunk_modify_f             mbcm_trunk_modify;
    mbcm_trunk_get_f                mbcm_trunk_get;
    mbcm_trunk_destroy_f            mbcm_trunk_destroy;
    mbcm_trunk_mcast_join_f         mbcm_trunk_mcast_join;

    /* STG functions */
    mbcm_stg_stp_init_f   	    mbcm_stg_stp_init;
    mbcm_stg_stp_get_f   	    mbcm_stg_stp_get;
    mbcm_stg_stp_set_f   	    mbcm_stg_stp_set;

    /* Multicasting functions */
    mbcm_mcast_addr_add_f           mbcm_mcast_addr_add;
    mbcm_mcast_addr_remove_f        mbcm_mcast_addr_remove;
    mbcm_mcast_port_get_f           mbcm_mcast_port_get;
    mbcm_mcast_init_f               mbcm_mcast_init;
    mbcm_mcast_detach_f             mbcm_mcast_detach;
    mbcm_mcast_addr_add_w_l2mcindex_f  mbcm_mcast_addr_add_w_l2mcindex;
    mbcm_mcast_addr_remove_w_l2mcindex_f  mbcm_mcast_addr_remove_w_l2mcindex;
    mbcm_mcast_port_add_f           mbcm_mcast_port_add;
    mbcm_mcast_port_remove_f        mbcm_mcast_port_remove;

    /* COSQ functions */
    mbcm_cosq_init_f                mbcm_cosq_init;
    mbcm_cosq_detach_f              mbcm_cosq_detach;
    mbcm_cosq_config_set_f          mbcm_cosq_config_set;
    mbcm_cosq_config_get_f          mbcm_cosq_config_get;
    mbcm_cosq_mapping_set_f         mbcm_cosq_mapping_set;
    mbcm_cosq_mapping_get_f         mbcm_cosq_mapping_get;
    mbcm_cosq_port_sched_set_f      mbcm_cosq_port_sched_set;
    mbcm_cosq_port_sched_get_f      mbcm_cosq_port_sched_get;
    mbcm_cosq_sched_weight_max_get_f  mbcm_cosq_sched_weight_max_get;
    mbcm_cosq_port_bandwidth_set_f  mbcm_cosq_port_bandwidth_set;
    mbcm_cosq_port_bandwidth_get_f  mbcm_cosq_port_bandwidth_get;
    mbcm_cosq_discard_set_f         mbcm_cosq_discard_set;
    mbcm_cosq_discard_get_f         mbcm_cosq_discard_get;
    mbcm_cosq_discard_port_set_f    mbcm_cosq_discard_port_set;
    mbcm_cosq_discard_port_get_f    mbcm_cosq_discard_port_get;
#ifdef BCM_WARM_BOOT_SUPPORT
    mbcm_cosq_sync_f                mbcm_cosq_sync;
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifndef BCM_SW_STATE_DUMP_DISABLE
    mbcm_cosq_sw_dump_f             mbcm_cosq_sw_dump;
#endif /* BCM_SW_STATE_DUMP_DISABLE */

    /*  Port meter functions */
    mbcm_port_rate_egress_set_f     mbcm_port_rate_egress_set;
    mbcm_port_rate_egress_get_f     mbcm_port_rate_egress_get;

#ifdef INCLUDE_L3
    /* L3 functions */
    mbcm_l3_tables_init_f           mbcm_l3_tables_init;
    mbcm_l3_tables_cleanup_f        mbcm_l3_tables_cleanup;
    mbcm_l3_enable_f                mbcm_l3_enable;
    mbcm_l3_intf_get_f              mbcm_l3_intf_get;
    mbcm_l3_intf_get_by_vid_f       mbcm_l3_intf_get_by_vid;
    mbcm_l3_intf_create_f           mbcm_l3_intf_create;
    mbcm_l3_intf_id_create_f        mbcm_l3_intf_id_create;
    mbcm_l3_intf_lookup_f           mbcm_l3_intf_lookup;
    mbcm_l3_intf_delete_f           mbcm_l3_intf_delete;
    mbcm_l3_intf_delete_all_f       mbcm_l3_intf_delete_all;

    mbcm_l3_ip4_get_f               mbcm_l3_ip4_get;
    mbcm_l3_ip4_add_f               mbcm_l3_ip4_add;
    mbcm_l3_ip4_delete_f            mbcm_l3_ip4_delete;
    mbcm_l3_ip4_delete_prefix_f     mbcm_l3_ip4_delete_prefix;
    mbcm_l3_delete_intf_f           mbcm_l3_delete_intf;
    mbcm_l3_delete_all_f            mbcm_l3_delete_all;
    mbcm_l3_ip4_replace_f           mbcm_l3_ip4_replace;
    mbcm_l3_host_age_f              mbcm_l3_age;
    mbcm_l3_traverse_f              mbcm_l3_ip4_traverse;

    mbcm_l3_ip6_get_f               mbcm_l3_ip6_get;
    mbcm_l3_ip6_add_f               mbcm_l3_ip6_add;
    mbcm_l3_ip6_delete_f            mbcm_l3_ip6_delete;
    mbcm_l3_ip6_delete_prefix_f     mbcm_l3_ip6_delete_prefix;
    mbcm_l3_ip6_replace_f           mbcm_l3_ip6_replace;
    mbcm_l3_traverse_f              mbcm_l3_ip6_traverse;

    mbcm_ip4_defip_cfg_get_f        mbcm_ip4_defip_cfg_get;
    mbcm_ip4_defip_ecmp_get_all_f   mbcm_ip4_defip_ecmp_get_all;
    mbcm_ip4_defip_add_f            mbcm_ip4_defip_add;
    mbcm_ip4_defip_delete_f         mbcm_ip4_defip_delete;
    mbcm_defip_delete_intf_f        mbcm_defip_delete_intf;
    mbcm_defip_delete_all_f         mbcm_defip_delete_all;
    mbcm_defip_age_f                mbcm_defip_age;
    mbcm_defip_traverse_f           mbcm_ip4_defip_traverse;

    mbcm_ip6_defip_cfg_get_f        mbcm_ip6_defip_cfg_get;
    mbcm_ip6_defip_ecmp_get_all_f   mbcm_ip6_defip_ecmp_get_all;
    mbcm_ip6_defip_add_f            mbcm_ip6_defip_add;
    mbcm_ip6_defip_delete_f         mbcm_ip6_defip_delete;
    mbcm_defip_traverse_f           mbcm_ip6_defip_traverse;

    mbcm_l3_conflict_get_f          mbcm_l3_conflict_get;
    mbcm_l3_info_get_f              mbcm_l3_info_get;

    /* IPMC functions */
    mbcm_ipmc_init_f                mbcm_ipmc_init;
    mbcm_ipmc_detach_f              mbcm_ipmc_detach;
    mbcm_ipmc_enable_f              mbcm_ipmc_enable;
    mbcm_ipmc_src_port_check_f      mbcm_ipmc_src_port_check;
    mbcm_ipmc_src_ip_search_f       mbcm_ipmc_src_ip_search;
    mbcm_ipmc_add_f                 mbcm_ipmc_add;
    mbcm_ipmc_delete_f              mbcm_ipmc_delete;
    mbcm_ipmc_delete_all_f          mbcm_ipmc_delete_all;
    mbcm_ipmc_lookup_f              mbcm_ipmc_lookup;
    mbcm_ipmc_get_f                 mbcm_ipmc_get;
    mbcm_ipmc_put_f                 mbcm_ipmc_put;
    mbcm_ipmc_egress_port_get_f     mbcm_ipmc_egress_port_get;
    mbcm_ipmc_egress_port_set_f     mbcm_ipmc_egress_port_set;
    mbcm_ipmc_repl_init_f           mbcm_ipmc_repl_init;
    mbcm_ipmc_repl_detach_f         mbcm_ipmc_repl_detach;
    mbcm_ipmc_repl_get_f            mbcm_ipmc_repl_get;
    mbcm_ipmc_repl_add_f            mbcm_ipmc_repl_add;
    mbcm_ipmc_repl_delete_f         mbcm_ipmc_repl_delete;
    mbcm_ipmc_repl_delete_all_f     mbcm_ipmc_repl_delete_all;
    mbcm_ipmc_egress_intf_add_f     mbcm_ipmc_egress_intf_add;
    mbcm_ipmc_egress_intf_delete_f  mbcm_ipmc_egress_intf_delete;
    mbcm_ipmc_age_f                 mbcm_ipmc_age;
    mbcm_ipmc_traverse_f            mbcm_ipmc_traverse;

#endif /* INCLUDE_L3 */

#ifdef INCLUDE_REGEX
    mbcm_regex_config_set_f         mbcm_regex_config_set;
    mbcm_regex_config_get_f         mbcm_regex_config_get;

    mbcm_regex_exclude_add_f        mbcm_regex_exclude_add;
    mbcm_regex_exclude_delete_f     mbcm_regex_exclude_delete;
    mbcm_regex_exclude_delete_all_f mbcm_regex_exclude_delete_all;
    mbcm_regex_exclude_get_f        mbcm_regex_exclude_get;
    mbcm_regex_engine_create_f      mbcm_regex_engine_create;
    mbcm_regex_engine_destroy_f     mbcm_regex_engine_destroy;
    mbcm_regex_engine_traverse_f    mbcm_regex_engine_traverse;
    mbcm_regex_engine_get_f         mbcm_regex_engine_get;
    bcm_regex_match_check_f         mbcm_regex_match_check;
    bcm_regex_match_set_f           mbcm_regex_match_set;
    bcm_regex_report_register_f     mbcm_regex_report_register;
    bcm_regex_report_unregister_f   mbcm_regex_report_unregister;
#endif /* INCLUDE_REGEX */
} mbcm_functions_t;

extern mbcm_functions_t *mbcm_driver[BCM_MAX_NUM_UNITS];
extern uint32		mbcm_capability[BCM_MAX_NUM_UNITS];

extern mbcm_functions_t mbcm_allayer_driver;
extern mbcm_functions_t mbcm_hercules_driver;
extern mbcm_functions_t mbcm_firebolt_driver;
extern mbcm_functions_t mbcm_bradley_driver;
extern mbcm_functions_t mbcm_humv_driver;
extern mbcm_functions_t mbcm_triumph_driver;
extern mbcm_functions_t mbcm_scorpion_driver;
extern mbcm_functions_t mbcm_conqueror_driver;
extern mbcm_functions_t mbcm_triumph2_driver;
extern mbcm_functions_t mbcm_enduro_driver;
extern mbcm_functions_t mbcm_hurricane_driver;
extern mbcm_functions_t mbcm_hurricane2_driver;
extern mbcm_functions_t mbcm_trident_driver;
extern mbcm_functions_t mbcm_titan_driver;
extern mbcm_functions_t mbcm_katana_driver;
extern mbcm_functions_t mbcm_triumph3_driver;
extern mbcm_functions_t mbcm_trident2_driver;
extern mbcm_functions_t mbcm_titan2_driver;
extern mbcm_functions_t mbcm_katana2_driver;
extern mbcm_functions_t mbcm_greyhound_driver;
extern mbcm_functions_t mbcm_tomahawk_driver;
extern mbcm_functions_t mbcm_apache_driver;
extern mbcm_functions_t mbcm_monterey_driver;
extern mbcm_functions_t mbcm_hurricane3_driver;
extern mbcm_functions_t mbcm_trident2plus_driver;
extern mbcm_functions_t mbcm_trident3_driver;
extern mbcm_functions_t mbcm_titan2plus_driver;
extern mbcm_functions_t mbcm_greyhound2_driver;
extern mbcm_functions_t mbcm_tomahawk2_driver;
extern mbcm_functions_t mbcm_helix5_driver;
extern mbcm_functions_t mbcm_hurricane4_driver;
extern mbcm_functions_t mbcm_firebolt6_driver;
extern mbcm_functions_t mbcm_tomahawk3_driver;

int mbcm_init(int unit);
int mbcm_deinit(int unit);

#endif	/* !_BCM_INT_MBCM_H_ */
