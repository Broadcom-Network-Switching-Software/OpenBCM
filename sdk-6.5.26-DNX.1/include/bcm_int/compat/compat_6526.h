/*
 * $Id: compat_6526.h,v 1.0 2019/04/03
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-6.5.26 routines
 */

#ifndef _COMPAT_6526_H_
#define _COMPAT_6526_H_

#ifdef BCM_RPC_SUPPORT
#if defined(INCLUDE_BFD)
#include <bcm/bfd.h>
#endif /* defined(INCLUDE_BFD) */
#include <bcm/vlan.h>
#include <bcm/rx.h>
#include <bcm/mirror.h>
#include <bcm/cosq.h>
#include <bcm/types.h>
#include <bcm/flow.h>
#include <bcm/flowtracker.h>
#include <bcm/l2.h>

#if defined(INCLUDE_L3)
#include <bcm/l3.h>
#include <bcm/ipmc.h>
#endif /* defined(INCLUDE_L3) */

/* Flowtracker check information. */
typedef struct bcm_compat6526_flowtracker_check_info_s {
    uint32 flags;                       /* Configuration flags for Check
                                           Creation. */
    bcm_flowtracker_tracking_param_type_t param; /* The attribute of flow on which the
                                           check is performed. */
    uint32 min_value;                   /* Element value to do greater than or
                                           equal checks. Minimum value to do
                                           range checks. */
    uint32 max_value;                   /* Element value to do smaller than or
                                           mask checks. Maximum value to do
                                           range checks. */
    uint32 mask_value;                  /* Mask value to be applied on flow
                                           attribute. */
    uint8 shift_value;                  /* Right shift value to be applied on
                                           flow attribute. */
    bcm_flowtracker_check_operation_t operation; /* Operation to be performed for this
                                           check. */
    bcm_flowtracker_check_t primary_check_id; /* primary check id to associate second
                                           check for aggregated checks on same
                                           flow. */
    uint32 state_transition_flags;      /* State Transition flags */
    uint16 num_drop_reason_group_id;    /* Number of drop reason group id. */
    bcm_flowtracker_drop_reason_group_t drop_reason_group_id[BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX]; /* Flowtracker drop reason group ids.
                                           Applicable only if tracking param is
                                           IngDropReasonGroupIdVector or
                                           EgrDropReasonGroupIdVector. */
    uint16 custom_id;                   /* Custom Identifier for Flowtracker
                                           Tracking parameter type custom. */
    bcm_flowtracker_stage_t stage;      /* Flowtracker stage to be used for the
                                           flowtracker check. */
} bcm_compat6526_flowtracker_check_info_t;

/* Actions related information for Flow check. */
typedef struct bcm_compat6526_flowtracker_check_action_info_s {
    uint32 flags;                       /* Action flags. */
    bcm_flowtracker_tracking_param_type_t param; /* The attribute of flow on which the
                                           check is performed. */
    bcm_flowtracker_check_action_t action; /* Action to be performed if check
                                           passes. */
    uint32 mask_value;                  /* Mask value to be applied on flow
                                           attribute. */
    uint8 shift_value;                  /* Right shift value to be applied on
                                           flow attribute. */
    bcm_flowtracker_tracking_param_weight_t weight; /* Weight given to current data over
                                           running average. */
    bcm_flowtracker_direction_t direction; /* Direction of flow from where tracking
                                           paramter is picked. */
    uint16 custom_id;                   /* Custom Identifier for Flowtracker
                                           Tracking parameter type custom. */
} bcm_compat6526_flowtracker_check_action_info_t;

/* Export related information for Flow check. */
typedef struct bcm_compat6526_flowtracker_check_export_info_s {
    uint32 export_check_threshold;      /* Threshold at which the export should
                                           be triggered for this check. At each
                                           incoming matching flow's packet the
                                           tracking data is updated and that
                                           value is checked with threshold
                                           value. If the check mentioned by
                                           operation passes between two values
                                           then export is triggered. */
    bcm_flowtracker_check_operation_t operation; /* Operation to be checked with
                                           threshold value for export check. */
} bcm_compat6526_flowtracker_check_export_info_t;

/* Create a flowtracker flow group. */
extern int bcm_compat6526_flowtracker_check_create(
    int unit,
    uint32 options,
    bcm_compat6526_flowtracker_check_info_t check_info,
    bcm_flowtracker_check_t *check_id);

/* Get a flowtracker flow check. */
extern int bcm_compat6526_flowtracker_check_get(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_compat6526_flowtracker_check_info_t *check_info);

/* Initialize a flow check action information structure. */
extern int bcm_compat6526_flowtracker_check_action_info_set(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_compat6526_flowtracker_check_action_info_t info);

/* Initialize a flow check action information structure. */
extern int bcm_compat6526_flowtracker_check_action_info_get(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_compat6526_flowtracker_check_action_info_t *info);

/* Set flow check export information to flowtracker check. */
extern int bcm_compat6526_flowtracker_check_export_info_set(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_compat6526_flowtracker_check_export_info_t info);

/* Get flow check export information for a flowtracker check. */
extern int bcm_compat6526_flowtracker_check_export_info_get(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_compat6526_flowtracker_check_export_info_t *info);

/* 
 * Information required for performing a delete/modify action on a learnt
 * flow.
 */
typedef struct bcm_compat6526_flowtracker_group_flow_action_info_s {
    uint8 pipe_idx;                     /* Pipe on which the flow was learnt. */
    bcm_flowtracker_exact_match_idx_mode_t mode; /* Exact match index mode. */
    uint32 exact_match_idx;             /* Exact match index of the learnt flow. */
} bcm_compat6526_flowtracker_group_flow_action_info_t;

/* Device-independent L2 address structure. */
typedef struct bcm_compat6526_l2_addr_s {
    uint32 flags;    /* BCM_L2_xxx flags. */
    uint32 flags2;    /* BCM_L2_FLAGS2_xxx flags. */
    uint32 station_flags;    /* BCM_L2_STATION_xxx flags. */
    bcm_mac_t mac;    /* 802.3 MAC address. */
    bcm_vlan_t vid;    /* VLAN identifier. */
    int port;    /* Zero-based port number. */
    int modid;    /* XGS: modid. */
    bcm_trunk_t tgid;    /* Trunk group ID. */
    bcm_cos_t cos_dst;    /* COS based on dst addr. */
    bcm_cos_t cos_src;    /* COS based on src addr. */
    bcm_multicast_t l2mc_group;    /* XGS: index in L2MC table */
    bcm_if_t egress_if;    /* XGS: index in Next Hop Tables. Used it with BCM_L2_FLAGS2_ROE_NHI flag */
    bcm_multicast_t l3mc_group;    /* XGS: index in L3_IPMC table. Use it with BCM_L2_FLAGS2_L3_MULTICAST. */
    bcm_pbmp_t block_bitmap;    /* XGS: blocked egress bitmap. */
    int auth;    /* Used if auth enabled on port. */
    int group;    /* Group number for FP. */
    bcm_fabric_distribution_t distribution_class;    /* Fabric Distribution Class. */
    int encap_id;    /* out logical interface */
    int age_state;    /* Age state of the entry */
    uint32 flow_handle;    /* FLOW handle for flex entries. */
    uint32 flow_option_handle;    /* FLOW option handle for flex entries. */
    bcm_flow_logical_field_t logical_fields[BCM_FLOW_MAX_NOF_LOGICAL_FIELDS];    /* logical fields array for flex entries. */
    uint32 num_of_fields;    /* number of logical fields. */
    bcm_pbmp_t sa_source_filter_pbmp;    /* Source port filter bitmap for this SA */
    bcm_tsn_flowset_t tsn_flowset;    /* Time-Sensitive Networking: associated flow set */
    bcm_tsn_sr_flowset_t sr_flowset;    /* Seamless Redundancy: associated flow set */
    bcm_policer_t policer_id;    /* Base policer to be used */
    bcm_tsn_pri_map_t taf_gate_primap;    /* TAF (Time Aware Filtering) gate priority mapping */
    uint32 stat_id;    /* Object statistics ID */
    int stat_pp_profile;    /* Statistics profile */
    uint16 gbp_src_id;    /* GBP Source ID */
    int opaque_ctrl_id;    /* Opaque control ID. */
    uint32 learn_strength;              /* Learn strength */
    uint32 age_profile;                 /*  Age profile */
} bcm_compat6526_l2_addr_t;

/* Add an L2 address entry to the specified device. */
extern int bcm_compat6526_l2_addr_add(
    int unit,
    bcm_compat6526_l2_addr_t *l2addr);

/* Check if an L2 entry is present in the L2 table. */
extern int bcm_compat6526_l2_addr_get(
    int unit,
    bcm_mac_t mac_addr,
    bcm_vlan_t vid,
    bcm_compat6526_l2_addr_t *l2addr);

/* Given an L2 address entry, return existing addresses which could conflict. */
extern int bcm_compat6526_l2_conflict_get(
    int unit,
    bcm_compat6526_l2_addr_t *addr,
    bcm_compat6526_l2_addr_t *cf_array,
    int cf_max,
    int *cf_count);

/* Get the specified L2 statistic from the chip. */
extern int bcm_compat6526_l2_stat_get(
    int unit,
    bcm_compat6526_l2_addr_t *l2_addr,
    bcm_l2_stat_t stat,
    uint64 *val);

/* Get the specified L2 statistic from the chip. */
extern int bcm_compat6526_l2_stat_get32(
    int unit,
    bcm_compat6526_l2_addr_t *l2_addr,
    bcm_l2_stat_t stat,
    uint32 *val);

/* Set the specified L2 statistic to the indicated value. */
extern int bcm_compat6526_l2_stat_set(
    int unit,
    bcm_compat6526_l2_addr_t *l2_addr,
    bcm_l2_stat_t stat,
    uint64 val);

/* Set the specified L2 statistic to the indicated value. */
extern int bcm_compat6526_l2_stat_set32(
    int unit,
    bcm_compat6526_l2_addr_t *l2_addr,
    bcm_l2_stat_t stat,
    uint32 val);

/* Enable/disable collection of statistics on the indicated L2 entry. */
extern int bcm_compat6526_l2_stat_enable_set(
    int unit,
    bcm_compat6526_l2_addr_t *l2_addr,
    int enable);

/* Replace destination (or delete) multiple L2 entries. */
extern int bcm_compat6526_l2_replace(
    int unit,
    uint32 flags,
    bcm_compat6526_l2_addr_t *match_addr,
    bcm_module_t new_module,
    bcm_port_t new_port,
    bcm_trunk_t new_trunk);

/* Replace destination (or delete) multiple L2 entries matching the given address and mask. Only bits that are 1 (turned on) in the mask will be checked when checking the match. */
extern int bcm_compat6526_l2_replace_match(
    int unit,
    uint32 flags,
    bcm_compat6526_l2_addr_t *match_addr,
    bcm_compat6526_l2_addr_t *mask_addr,
    bcm_compat6526_l2_addr_t *replace_addr,
    bcm_compat6526_l2_addr_t *replace_mask_addr);

/* Add mutli L2 address entry to the specified device. */
extern int bcm_compat6526_l2_addr_multi_add(
    int unit,
    bcm_compat6526_l2_addr_t *l2addr,
    int count);

/* Delete multi L2 address entry from the specified device. */
extern int bcm_compat6526_l2_addr_multi_delete(
    int unit,
    bcm_compat6526_l2_addr_t *l2addr,
    int count);

/* Retrieve a MAC entry according to mac, vid (used as VSI) and modid. When modid != 0 it should hold a VLAN ID for IVL entries. */
extern int bcm_compat6526_l2_addr_by_struct_get(
    int unit,
    bcm_compat6526_l2_addr_t *l2addr);

/* Delete a MAC entry according to mac, vid (used as VSI) and modid. When modid != 0 it should hold a VLAN ID for IVL entries. */
extern int bcm_compat6526_l2_addr_by_struct_delete(
    int unit,
    bcm_compat6526_l2_addr_t *l2addr);

#if defined(INCLUDE_L3)

/* IPMC address structure. */
typedef struct bcm_compat6526_ipmc_addr_s {
    bcm_ip_t s_ip_addr;                 /* IPv4 Source address. */
    bcm_ip_t mc_ip_addr;                /* IPv4 Destination address. */
    bcm_ip6_t s_ip6_addr;               /* IPv6 Source address. */
    bcm_ip6_t mc_ip6_addr;              /* IPv6 Destination address. */
    bcm_vlan_t vid;                     /* VLAN identifier. */
    bcm_vrf_t vrf;                      /* Virtual Router Instance. */
    bcm_cos_t cos;                      /* COS based on dst IP multicast addr. */
    int ts;                             /* Source port or TGID bit. */
    int port_tgid;                      /* Source port or TGID. */
    int v;                              /* Valid bit. */
    int mod_id;                         /* Module ID. */
    bcm_multicast_t group;              /* Use this index to program IPMC table
                                           for XGS chips based on flags value */
    uint32 flags;                       /* See BCM_IPMC_XXX flag definitions. */
    int lookup_class;                   /* Classification lookup class ID. */
    bcm_fabric_distribution_t distribution_class; /* Fabric Distribution Class. */
    bcm_if_t l3a_intf;                  /* L3 interface associated with route. */
    int rp_id;                          /* Rendezvous point ID. */
    bcm_ip_t s_ip_mask;                 /* IPv4 Source subnet mask. */
    bcm_if_t ing_intf;                  /* L3 interface associated with this
                                           Entry */
    bcm_ip_t mc_ip_mask;                /* IPv4 Destination subnet mask. */
    bcm_ip6_t mc_ip6_mask;              /* IPv6 Destination subnet mask. */
    bcm_multicast_t group_l2;           /* Use this index to program IPMC table
                                           for l2 recipients if TTL/RPF check
                                           fails. */
    uint32 stat_id;                     /* Object statistics ID. */
    int stat_pp_profile;                /* Statistics profile. */
    bcm_ip6_t s_ip6_mask;               /* IPv6 Source subnet mask. */
    int priority;                       /* Entry priority. */
    uint32 flow_handle;                 /* FLOW handle for flex entries. */
    uint32 flow_option_handle;          /* FLOW option handle for flex entries. */
    bcm_flow_logical_field_t logical_fields[BCM_FLOW_MAX_NOF_LOGICAL_FIELDS]; /* logical fields array for flex
                                           entries. */
    uint32 num_logical_fields;          /* number of logical fields. */
    int mtu;                            /* MTU. */
    int opaque_ctrl_id;                 /* Opaque control ID. */
} bcm_compat6526_ipmc_addr_t;

/* Add a new entry to IPMC table. */
extern int bcm_compat6526_ipmc_add(
    int unit,
    bcm_compat6526_ipmc_addr_t *data);

/* Find info about an IPMC entry. */
extern int bcm_compat6526_ipmc_find(
    int unit,
    bcm_compat6526_ipmc_addr_t *data);

/* Remove an entry from IPMC table. */
extern int bcm_compat6526_ipmc_remove(
    int unit,
    bcm_compat6526_ipmc_addr_t *data);

/* Attach counters entries to the given IPMC group. */
extern int bcm_compat6526_ipmc_stat_attach(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
    uint32 stat_counter_id);

/* Detach counters entries to the given IPMC group. */
extern int bcm_compat6526_ipmc_stat_detach(
    int unit,
    bcm_compat6526_ipmc_addr_t *info);

/* Get counter statistic values for an IPMC group. */
extern int bcm_compat6526_ipmc_stat_counter_get(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
    bcm_ipmc_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

/*
 * Force an immediate counter update and retrieve counter statistic
 * values for an IPMC group.
 */
 extern int bcm_compat6526_ipmc_stat_counter_sync_get(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
    bcm_ipmc_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

/* Set counter statistic values for an IPMC group. */
extern int bcm_compat6526_ipmc_stat_counter_set(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
    bcm_ipmc_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

/* Get multiple counter statistic values for multiple IPMC group. */
extern int bcm_compat6526_ipmc_stat_multi_get(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
    int nstat,
    bcm_ipmc_stat_t *stat_arr,
    uint64 *value_arr);

/* Get 32bit multiple counter statistic values for multiple IPMC group. */
extern int bcm_compat6526_ipmc_stat_multi_get32(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
    int nstat,
    bcm_ipmc_stat_t *stat_arr,
    uint32 *value_arr);

/* Set multiple counter statistic values for multiple IPMC group. */
extern int bcm_compat6526_ipmc_stat_multi_set(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
    int nstat,
    bcm_ipmc_stat_t *stat_arr,
    uint64 *value_arr);

/* Set 32bit multiple counter statistic values for multiple IPMC group. */
extern int bcm_compat6526_ipmc_stat_multi_set32(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
    int nstat,
    bcm_ipmc_stat_t *stat_arr,
    uint32 *value_arr);

/* Get stat counter ID associated with given IPMC group. */
extern int bcm_compat6526_ipmc_stat_id_get(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
    bcm_ipmc_stat_t stat,
    uint32 *stat_counter_id);

/* Add a new configurational entry. */
extern int bcm_compat6526_ipmc_config_add(
    int unit,
    bcm_compat6526_ipmc_addr_t *config);

/* Find a configurational entry. */
extern int bcm_compat6526_ipmc_config_find(
    int unit,
    bcm_compat6526_ipmc_addr_t *config);

/* Remove a configurational entry */
extern int bcm_compat6526_ipmc_config_remove(
    int unit,
    bcm_compat6526_ipmc_addr_t *config);

/* L3 Host Structure. */
typedef struct bcm_compat6526_l3_host_s {
    uint32 l3a_flags;                   /* See BCM_L3_xxx flag definitions. */
    uint32 l3a_flags2;                  /* See BCM_L3_FLAGS2_xxx flag
                                           definitions. */
    bcm_vrf_t l3a_vrf;                  /* Virtual router instance. */
    bcm_ip_t l3a_ip_addr;               /* Destination host IP address (IPv4). */
    bcm_ip6_t l3a_ip6_addr;             /* Destination host IP address (IPv6). */
    bcm_cos_t l3a_pri;                  /* New priority in packet. */
    bcm_if_t l3a_intf;                  /* L3 intf associated with this address. */
    bcm_if_t l3a_ul_intf;               /* Underlay L3 egress object associated
                                           with this address. */
    bcm_mac_t l3a_nexthop_mac;          /* Next hop MAC addr. */
    bcm_module_t l3a_modid;             /* Module ID packet is switched to. */
    bcm_port_t l3a_port_tgid;           /* Port/TGID packet is switched to. */
    bcm_port_t l3a_stack_port;          /* Used if modid not local (Strata
                                           Only). */
    int l3a_ipmc_ptr;                   /* Pointer to IPMC table. */
    int l3a_lookup_class;               /* Classification lookup class ID. */
    bcm_if_t encap_id;                  /* Encapsulation index. */
    bcm_if_t native_intf;               /*  L3 native interface (source MAC). */
    uint32 flow_handle;                 /* FLOW handle for flex entries. */
    uint32 flow_option_handle;          /* FLOW option handle for flex entries. */
    bcm_flow_logical_field_t logical_fields[BCM_FLOW_MAX_NOF_LOGICAL_FIELDS]; /* logical fields array for flex
                                           entries. */
    uint32 num_of_fields;               /* number of logical fields. */
    int l3a_ipmc_ptr_l2;                /* Pointer to IPMC table for L2
                                           recipients if TTL/RPF check fails. */
    uint32 stat_id;                     /* Object statistics ID. */
    int stat_pp_profile;                /* Statistics profile. */
    int l3a_mtu;                        /* Multicast packet MTU. */
    int l3a_opaque_ctrl_id;             /* Opaque control ID. */
    uint8 l3a_traffic_class;            /* Traffic class used when
                                           l3_alpm_tc_mode is enabled. */
} bcm_compat6526_l3_host_t;

/* Look up an L3 host table entry based on IP address. */
extern int bcm_compat6526_l3_host_find(
    int unit,
    bcm_compat6526_l3_host_t *info);

/* Add an entry into the L3 switching table. */
extern int bcm_compat6526_l3_host_add(
    int unit,
    bcm_compat6526_l3_host_t *info);

/* Delete an entry from the L3 host table. */
extern int bcm_compat6526_l3_host_delete(
    int unit,
    bcm_compat6526_l3_host_t *ip_addr);

/*
 * Deletes L3 entries that match or do not match a specified L3 interface
 * number
 */
extern int bcm_compat6526_l3_host_delete_by_interface(
    int unit,
    bcm_compat6526_l3_host_t *info);

/* Deletes all L3 host table entries. */
extern int bcm_compat6526_l3_host_delete_all(
    int unit,
    bcm_compat6526_l3_host_t *info);

/* Attach counters entries to the given L3 host entry */
extern int bcm_compat6526_l3_host_stat_attach(
    int unit,
    bcm_compat6526_l3_host_t *info,
    uint32 stat_counter_id);

/* Detach  counters entries to the given L3 host entry */
extern int bcm_compat6526_l3_host_stat_detach(
    int unit,
    bcm_compat6526_l3_host_t *info);

/* Set flex counter object value for the given L3 host entry. */
extern int bcm_compat6526_l3_host_flexctr_object_set(
    int unit,
    bcm_compat6526_l3_host_t *info,
    uint32 value);

/* Get flex counter object value for the given L3 host entry. */
extern int bcm_compat6526_l3_host_flexctr_object_get(
    int unit,
    bcm_compat6526_l3_host_t *info,
    uint32 *value);

/* Get the specified counter statistic for a L3 host entry. */
extern int bcm_compat6526_l3_host_stat_counter_get(
    int unit,
    bcm_compat6526_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

/*
 * Force an immediate counter update and retrieve the specified counter
 * statistic for a L3 host entry.
 */
extern int bcm_compat6526_l3_host_stat_counter_sync_get(
    int unit,
    bcm_compat6526_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

/* Set the specified counter statistic for a L3 host entry */
extern int bcm_compat6526_l3_host_stat_counter_set(
    int unit,
    bcm_compat6526_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

/* Provide stat counter ids associated with given L3 host entry */
extern int bcm_compat6526_l3_host_stat_id_get(
    int unit,
    bcm_compat6526_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 *stat_counter_id);

/* L3 ECMP structure */
typedef struct bcm_compat6526_l3_egress_ecmp_s {
    uint32 flags;                       /* See BCM_L3_xxx flag definitions. */
    bcm_if_t ecmp_intf;                 /* L3 interface ID pointing to egress
                                           ecmp object. */
    int max_paths;                      /* Max number of paths in ECMP group. If
                                           max_paths <= 0, the default max path
                                           which can be set by the API
                                           bcm_l3_route_max_ecmp_set will be
                                           picked. */
    uint32 ecmp_group_flags;            /* BCM_L3_ECMP_xxx flags. */
    uint32 dynamic_mode;                /* Dynamic load balancing mode. See
                                           BCM_L3_ECMP_DYNAMIC_MODE_xxx
                                           definitions. */
    uint32 dynamic_size;                /* Number of flows for dynamic load
                                           balancing. Valid values are 512, 1k,
                                           doubling up to 32k */
    uint32 dynamic_age;                 /* Inactivity duration, in
                                           microseconds. */
    uint32 dynamic_load_exponent;       /* The exponent used in the
                                           exponentially weighted moving average
                                           calculation of historical member
                                           load. */
    uint32 dynamic_expected_load_exponent; /* The exponent used in the
                                           exponentially weighted moving average
                                           calculation of historical expected
                                           member load. */
    bcm_l3_dgm_t dgm;                   /* DGM properties */
    uint32 stat_id;                     /* Object statistics ID. */
    int stat_pp_profile;                /* Statistics profile. */
    bcm_l3_ecmp_urpf_mode_t rpf_mode;   /* Define RPF mode for the ECMP
                                           group. */
    bcm_l3_tunnel_priority_info_t tunnel_priority; /* ECMP tunnel priority. */
    uint32 rh_random_seed;              /* Random seed for the RH ECMP group. */
    int user_profile;                   /* User defined ECMP profile. */
    bcm_failover_t failover_id;         /* Two level 1:1 ECMP failover group
                                           object ID created with
                                           BCM_FAILOVER_MULTI_LEVEL_TYPE flag
                                           set. */
    uint32 failover_size;               /* The failover group size, indicates
                                           the total number of sequential egress
                                           forwarding objects that are members
                                           of this group. */
    bcm_if_t failover_base_egress_id;   /* Failover group member base egress
                                           forwarding object ID. */
    bcm_l3_ecmp_failover_lb_mode_t failover_lb_mode; /* Failover group load-balancing mode. */
    bcm_if_t secondary_ecmp_intf;       /* L3 interface ID pointing to a
                                          secondary egress ECMP. */
    int port_failover;                  /* Perform failover when the egress port
                                          is link down. */
    int same_src_dst_port_failover;     /* Perform failover when the destination
                                          port is the same as the source port. */
    int egress_intf_failover;           /* Perform failover when the egress
                                          object failover is triggered by
                                          bcm_failover_egress_status_set. */
    bcm_l3_ecmp_tertiary_lb_mode_t tertiary_lb_mode; /* Tertiary ECMP load-balancing mode. */
} bcm_compat6526_l3_egress_ecmp_t;

/* Resilient ecmp entry */
typedef struct bcm_compat6526_l3_egress_ecmp_resilient_entry_s {
    uint64 hash_key;            /* Hash key. */
    bcm_compat6526_l3_egress_ecmp_t ecmp;  /* ECMP. */
    bcm_if_t intf;              /* L3 interface. */
} bcm_compat6526_l3_egress_ecmp_resilient_entry_t;

/* Create an Egress ECMP forwarding object. */
extern int bcm_compat6526_l3_ecmp_create(
    int unit,
    uint32 options,
    bcm_compat6526_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_count,
    bcm_l3_ecmp_member_t *ecmp_member_array);

/* Get info about an Egress ECMP forwarding object. */
extern int bcm_compat6526_l3_ecmp_get(
    int unit,
    bcm_compat6526_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_size,
    bcm_l3_ecmp_member_t *ecmp_member_array,
    int *ecmp_member_count);

/* Find an Egress ECMP forwarding object. */
extern int bcm_compat6526_l3_ecmp_find(
    int unit,
    int ecmp_member_count,
    bcm_l3_ecmp_member_t *ecmp_member_array,
    bcm_compat6526_l3_egress_ecmp_t *ecmp_info);

/* Create an Egress ECMP forwarding object. */
extern int bcm_compat6526_l3_egress_ecmp_create(
    int unit,
    bcm_compat6526_l3_egress_ecmp_t *ecmp,
    int intf_count,
    bcm_if_t *intf_array);

/* Destroy an Egress ECMP forwarding object. */
extern int bcm_compat6526_l3_egress_ecmp_destroy(
    int unit,
    bcm_compat6526_l3_egress_ecmp_t *ecmp);

/* Get info about an Egress ECMP forwarding object. */
extern int bcm_compat6526_l3_egress_ecmp_get(
    int unit,
    bcm_compat6526_l3_egress_ecmp_t *ecmp,
    int intf_size,
    bcm_if_t *intf_array,
    int *intf_count);

/* Add an Egress forwarding object to an Egress ECMP forwarding object. */
extern int bcm_compat6526_l3_egress_ecmp_add(
    int unit,
    bcm_compat6526_l3_egress_ecmp_t *ecmp,
    bcm_if_t intf);

/*
 * Delete an Egress forwarding object from an Egress ECMP forwarding
 * object.
 */
extern int bcm_compat6526_l3_egress_ecmp_delete(
    int unit,
    bcm_compat6526_l3_egress_ecmp_t *ecmp,
    bcm_if_t intf);

/* Find an Egress ECMP forwarding object. */
extern int bcm_compat6526_l3_egress_ecmp_find(
    int unit,
    int intf_count,
    bcm_if_t *intf_array,
    bcm_compat6526_l3_egress_ecmp_t *ecmp);

/*
 * Update an ECMP table that was already allocated, used in cases where
 * an ECMP have more than one table.
 */
extern int bcm_compat6526_l3_egress_ecmp_tunnel_priority_set(
    int unit,
    bcm_compat6526_l3_egress_ecmp_t *ecmp,
    int intf_count,
    bcm_if_t *intf_array);

/* Replace ECMP resilient entries matching given criteria. */
extern int bcm_compat6526_l3_egress_ecmp_resilient_replace(
    int unit,
    uint32 flags,
    bcm_compat6526_l3_egress_ecmp_resilient_entry_t *match_entry,
    int *num_entries,
    bcm_compat6526_l3_egress_ecmp_resilient_entry_t *replace_entry);

/* Add ECMP resilient entries matching given criteria. */
extern int bcm_compat6526_l3_egress_ecmp_resilient_add(
    int unit,
    uint32 flags,
    bcm_compat6526_l3_egress_ecmp_resilient_entry_t *entry);

/* Delete ECMP resilient entries matching given criteria. */
extern int bcm_compat6526_l3_egress_ecmp_resilient_delete(
    int unit,
    uint32 flags,
    bcm_compat6526_l3_egress_ecmp_resilient_entry_t *entry);

#endif /* INCLUDE_L3 */

#if defined(INCLUDE_L3)
/* Flow match config structure */
typedef struct bcm_compat6526_flow_match_config_s {
    uint32 vnid;    /* VN_ID. */
    bcm_vlan_t vlan;    /* Outer VLAN ID to match. */
    bcm_vlan_t inner_vlan;    /* Inner VLAN ID to match. */
    bcm_gport_t port;    /* Match port / trunk */
    bcm_ip_t sip;    /* source IPv4 address */
    bcm_ip_t sip_mask;    /* source IPv4 address mask */
    bcm_ip6_t sip6;    /* source IPv6 address */
    bcm_ip6_t sip6_mask;    /* source IPv6 address mask */
    bcm_ip_t dip;    /* destination IPv4 address */
    bcm_ip_t dip_mask;    /* destination IPv4 address mask */
    bcm_ip6_t dip6;    /* destination IPv6 address */
    bcm_ip6_t dip6_mask;    /* destination IPv6 address mask */
    uint16 udp_dst_port;    /* udp destination port. */
    uint16 udp_src_port;    /* udp source port. */
    uint16 protocol;    /* IP protocol type. */
    bcm_mpls_label_t mpls_label;    /* MPLS label. */
    bcm_gport_t flow_port;    /* flow port id representing a SVP */
    bcm_vpn_t vpn;    /* VPN representing a vfi or vrf */
    bcm_if_t intf_id;    /* l3 interface id */
    uint32 options;    /* BCM_FLOW_MATCH_OPTION_xxx. */
    bcm_flow_match_criteria_t criteria;    /* flow match criteria. */
    uint32 valid_elements;    /* bitmap of valid fields for the match action */
    bcm_flow_handle_t flow_handle;    /* flow handle derived from flow name */
    uint32 flow_option;    /* flow option derived from flow option name */
    uint32 flags;    /* BCM_FLOW_MATCH_FLAG_xxx */
    bcm_vlan_t default_vlan;    /* Default VLAN ID to match. */
    bcm_vpn_t default_vpn;    /* Default VPN ID to match. */
    uint32 class_id;    /* class id field */
    uint16 next_hdr;    /* next header field */
    bcm_flow_tunnel_srv6_action_t tunnel_action;    /* SRv6 tunnel action on match */
    bcm_vlan_t etag_vlan;    /* ETAG VLAN ID to match. */
} bcm_compat6526_flow_match_config_t;

/* Add a match entry to assign switch objects for the matched packets */
extern int bcm_compat6526_flow_match_add(
    int unit,
    bcm_compat6526_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Delete a match entry for the given matching key */
extern int bcm_compat6526_flow_match_delete(
    int unit,
    bcm_compat6526_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Get a match entry for the given matching key */
extern int bcm_compat6526_flow_match_get(
    int unit,
    bcm_compat6526_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Set the flex counter object value to the given match entry. */
extern int bcm_compat6526_flow_match_flexctr_object_set(
    int unit,
    bcm_compat6526_flow_match_config_t *info,
    uint32 value);

/* Get the flex counter object value from the given match entry. */
extern int bcm_compat6526_flow_match_flexctr_object_get(
    int unit,
    bcm_compat6526_flow_match_config_t *info,
    uint32 *value);

/* Attach counter entries to the given match entry. */
extern int bcm_compat6526_flow_match_stat_attach(
    int unit,
    bcm_compat6526_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 stat_counter_id);

/* Detach counter entries from the given match entry. */
extern int bcm_compat6526_flow_match_stat_detach(
    int unit,
    bcm_compat6526_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Get stat counter ID associated to the given match entry. */
extern int bcm_compat6526_flow_match_stat_id_get(
    int unit,
    bcm_compat6526_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 *stat_counter_id);

/* Flow tunneling terminator structure. */
typedef struct bcm_compat6526_flow_tunnel_terminator_s {
    uint32 flags;    /* Configuration flags. Use BCM_TUNNEL flags */
    uint32 multicast_flag;    /* VXLAN Multicast trigger flags. */
    bcm_vrf_t vrf;    /* Virtual router instance. */
    bcm_ip_t sip;    /* SIP for tunnel header match. */
    bcm_ip_t dip;    /* DIP for tunnel header match. */
    bcm_ip_t sip_mask;    /* Source IP mask. */
    bcm_ip_t dip_mask;    /* Destination IP mask. */
    bcm_ip6_t sip6;    /* SIP for tunnel header match (IPv6). */
    bcm_ip6_t dip6;    /* DIP for tunnel header match (IPv6). */
    bcm_ip6_t sip6_mask;    /* Source IP mask (IPv6). */
    bcm_ip6_t dip6_mask;    /* Destination IP mask (IPv6). */
    uint32 udp_dst_port;    /* UDP dst port for UDP packets. */
    uint32 udp_src_port;    /* UDP src port for UDP packets */
    bcm_tunnel_type_t type;    /* Tunnel type */
    bcm_vlan_t vlan;    /* VLAN ID for tunnel header match. */
    uint32 protocol;    /* Protocol type */
    uint32 valid_elements;    /* bitmap of valid fields */
    uint32 flow_handle;    /* flow handle */
    uint32 flow_option;    /* flow option */
    bcm_vlan_t vlan_mask;    /* The VLAN ID mask. */
    uint32 class_id;    /* Class ID as a key in VFP. */
    bcm_pbmp_t term_pbmp;    /* Allowed port bitmap for tunnel termination. */
    uint16 next_hdr;    /* Match next header. */
    bcm_gport_t flow_port;    /* Assign SVP. */
    bcm_if_t intf_id;    /* Assign L3_IIF. */
    bcm_vpn_t vpn;    /* VPN representing a vfi */
} bcm_compat6526_flow_tunnel_terminator_t;

/* Create a flow tunnel terminator object. */
extern int bcm_compat6526_flow_tunnel_terminator_create(
    int unit,
    bcm_compat6526_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Delete a flow tunnel terminator object. */
extern int bcm_compat6526_flow_tunnel_terminator_destroy(
    int unit,
    bcm_compat6526_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Get the flow tunnel terminator object. */
extern int bcm_compat6526_flow_tunnel_terminator_get(
    int unit,
    bcm_compat6526_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Set the flex counter object value to the flow tunnel terminator object. */
extern int bcm_compat6526_flow_tunnel_terminator_flexctr_object_set(
    int unit,
    bcm_compat6526_flow_tunnel_terminator_t *info,
    uint32 value);

/* Get the flex counter object value from the flow tunnel terminator object. */
extern int bcm_compat6526_flow_tunnel_terminator_flexctr_object_get(
    int unit,
    bcm_compat6526_flow_tunnel_terminator_t *info,
    uint32 *value);

/* Attach counter entries to the flow tunnel terminator object. */
extern int bcm_compat6526_flow_tunnel_terminator_stat_attach(
    int unit,
    bcm_compat6526_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 stat_counter_id);

/* Detach counter entries from the flow tunnel terminator object. */
extern int bcm_compat6526_flow_tunnel_terminator_stat_detach(
    int unit,
    bcm_compat6526_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Get stat counter ID associated to the flow tunnel terminator object. */
extern int bcm_compat6526_flow_tunnel_terminator_stat_id_get(
    int unit,
    bcm_compat6526_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 *stat_counter_id);


#endif /* defined(INCLUDE_L3) */
/* Mirror-on-drop profile structure. */
typedef struct bcm_compat6526_cosq_mod_profile_s {
    uint16 percent_0_25;    /* Sampling probability when current filled cells in mirror-on-drop buffer are less than 25% of maximum cell number. A 16-bit random number is compared against this value and if the random number is less than this value, then the packet is sampled. */
    uint16 percent_25_50;    /* Sampling probability when current filled cells in mirror-on-drop buffer are between 25% and 50% of maximum cell number. */
    uint16 percent_50_75;    /* Sampling probability when current filled cells in mirror-on-drop buffer are between 50% and 75% of maximum cell number. */
    uint16 percent_75_100;    /* Sampling probability when current filled cells in mirror-on-drop buffer are greater than 75% of maximum cell number. */
    uint8 enable;    /* MOD enable/disable. */
} bcm_compat6526_cosq_mod_profile_t;

/* Set/Get the mirror-on-drop profile entry with given profile id. */
extern int bcm_compat6526_cosq_mod_profile_set(
    int unit,
    int profile_id,
    bcm_compat6526_cosq_mod_profile_t *profile);

/* Set/Get the mirror-on-drop profile entry with given profile id. */
extern int bcm_compat6526_cosq_mod_profile_get(
    int unit,
    int profile_id,
    bcm_compat6526_cosq_mod_profile_t *profile);


/* Ingress mirror-on-drop event profile structure. */
typedef struct bcm_compat6526_mirror_ingress_mod_event_profile_s {
    uint8 reason_code;    /* Configurable reason code for ingress mirror-on-drop packets. */
    uint32 sample_rate;    /* Sampler threshold. Sample the mirror packets if the generated sFlow random number is less than or equal to the threshold. Set 0 to disable sampler. */
    uint32 priority;    /* Profile priority. Higher value means higher priority. When multiple profiles are hit simultaneously, the profile with the highest priority will be chosen to mirror. */
} bcm_compat6526_mirror_ingress_mod_event_profile_t;

/* Create an ingress mirror-on-drop event profile. */
extern int bcm_compat6526_mirror_ingress_mod_event_profile_create(
    int unit,
    bcm_compat6526_mirror_ingress_mod_event_profile_t *profile,
    int *profile_id);

/* Get an ingress mirror-on-drop event profile config. */
extern int bcm_compat6526_mirror_ingress_mod_event_profile_get(
    int unit,
    int profile_id,
    bcm_compat6526_mirror_ingress_mod_event_profile_t *profile);


/* Rx cosq mapping. */
typedef struct bcm_compat6526_rx_cosq_mapping_s {
    uint32 flags;    /* flags */
    int index;    /* Index into COSQ mapping table (0 is highest match priority) */
    bcm_rx_reasons_t reasons;    /* packet reasons bitmap */
    bcm_rx_reasons_t reasons_mask;    /* mask for packet reasons bitmap */
    uint8 int_prio;    /* internal priority value */
    uint8 int_prio_mask;    /* mask for internal priority value */
    uint32 packet_type;    /* packet type bitmap */
    uint32 packet_type_mask;    /* mask for packet type bitmap */
    bcm_cos_queue_t cosq;    /* cosq value */
    uint32 flex_key1;    /* Flex key 1. */
    uint32 flex_key1_mask;    /* Mask for flex key 1. */
    uint32 flex_key2;    /* Flex key 2. */
    uint32 flex_key2_mask;    /* Mask for flex key 2. */
    bcm_rx_pkt_trace_reasons_t trace_reasons;    /* Copy to CPU trace event reasons bitmap. */
    bcm_rx_pkt_trace_reasons_t trace_reasons_mask;    /* Mask for copy to CPU trace event reasons bitmap. */
    bcm_pkt_drop_event_t drop_event;    /* Drop event. */
    bcm_pkt_drop_event_t drop_event_mask;    /* Mask for drop event. */
    int priority;    /* Entry priority. */
} bcm_compat6526_rx_cosq_mapping_t;

/* Map packets to a CPU COS queue. */
extern int bcm_compat6526_rx_cosq_mapping_extended_set(
    int unit,
    uint32 options,
    bcm_compat6526_rx_cosq_mapping_t *rx_cosq_mapping);

/* Get packets to CPU COS queue mappings. */
extern int bcm_compat6526_rx_cosq_mapping_extended_get(
    int unit,
    bcm_compat6526_rx_cosq_mapping_t *rx_cosq_mapping);

/* Add a CPU COS map. */
extern int bcm_compat6526_rx_cosq_mapping_extended_add(
    int unit,
    uint32 options,
    bcm_compat6526_rx_cosq_mapping_t *rx_cosq_mapping);

/* Delete a CPU COS map. */
extern int bcm_compat6526_rx_cosq_mapping_extended_delete(
    int unit,
    bcm_compat6526_rx_cosq_mapping_t *rx_cosq_mapping);


/* Layer 2 Logical port type */
typedef struct bcm_compat6526_vlan_port_s {
    bcm_vlan_port_match_t criteria;    /* Match criteria. */
    uint32 flags;    /* BCM_VLAN_PORT_xxx. */
    bcm_vlan_t vsi;    /* Populated for bcm_vlan_port_find only */
    bcm_vlan_t match_vlan;    /* Outer VLAN ID to match. */
    bcm_vlan_t match_inner_vlan;    /* Inner VLAN ID to match. */
    int match_pcp;    /* Outer PCP ID to match. */
    bcm_tunnel_id_t match_tunnel_value;    /* Tunnel value to match. */
    bcm_port_ethertype_t match_ethertype;    /* Ethernet type value to match. */
    bcm_gport_t port;    /* Gport: local or remote Physical or logical gport. */
    uint8 pkt_pri;    /* Service tag priority. */
    uint8 pkt_cfi;    /* Service tag cfi. */
    uint16 egress_service_tpid;    /* Service VLAN TPID value. */
    bcm_vlan_t egress_vlan;    /* Egress Outer VLAN or SD-TAG VLAN ID. */
    bcm_vlan_t egress_inner_vlan;    /* Egress Inner VLAN. */
    bcm_tunnel_id_t egress_tunnel_value;    /* Egress Tunnel value. */
    bcm_if_t encap_id;    /* Encapsulation Index. */
    int qos_map_id;    /* QoS Map Index. */
    bcm_policer_t policer_id;    /* Policer ID */
    bcm_policer_t egress_policer_id;    /* Egress Policer ID */
    bcm_failover_t failover_id;    /* Failover Object Index. */
    bcm_gport_t failover_port_id;    /* Failover VLAN Port Identifier. */
    bcm_gport_t vlan_port_id;    /* GPORT identifier */
    bcm_multicast_t failover_mc_group;    /* MC group used for bi-cast. */
    bcm_failover_t ingress_failover_id;    /* 1+1 protection. */
    bcm_failover_t egress_failover_id;    /* Failover object index for Egress Protection */
    bcm_gport_t egress_failover_port_id;    /* Failover VLAN Port Identifier for Egress Protection */
    bcm_switch_network_group_t ingress_network_group_id;    /* Split Horizon ingress network group identifier */
    bcm_switch_network_group_t egress_network_group_id;    /* Split Horizon egress network group identifier */
    int inlif_counting_profile;    /* In LIF counting profile */
    int outlif_counting_profile;    /* Out LIF counting profile */
    uint32 if_class;    /* Egress Interface class ID */
    bcm_gport_t tunnel_id;    /* Pointer from Native AC to next entry in EEDB list. */
    uint32 group;    /* flush group id to be used by l2 flush mechanism */
    bcm_gport_t ingress_failover_port_id;    /* Failover VLAN Port Identifier for ingress Protection. */
    uint16 class_id;    /* Class ID of the source port */
    uint32 match_class_id;    /* Match Namespace. */
    bcm_qos_ingress_model_t ingress_qos_model;    /* ingress qos and ttl model */
    bcm_qos_egress_model_t egress_qos_model;    /* egress qos and ttl model */
    uint32 flags2;    /* BCM_VLAN_PORT_FLAGS2_xxx. */
    uint32 learn_strength;    /* Learn strength */
} bcm_compat6526_vlan_port_t;

/* Create a layer 2 logical port. */
extern int bcm_compat6526_vlan_port_create(
    int unit,
    bcm_compat6526_vlan_port_t *vlan_port);

/* Get/find a layer 2 logical port given the GPORT ID or match criteria. */
extern int bcm_compat6526_vlan_port_find(
    int unit,
    bcm_compat6526_vlan_port_t *vlan_port);


/* Flex Flow VP Config Structure. */
typedef struct bcm_compat6526_flow_port_s {
    bcm_gport_t flow_port_id;    /* GPORT identifier. */
    uint32 flags;    /* BCM_FLOW_PORT_xxx. */
    uint32 if_class;    /* Interface Class ID. */
    bcm_switch_network_group_t network_group_id;    /* Split Horizon network group identifier. */
    uint16 egress_service_tpid;    /* Service Vlan TPID Value. */
    int dscp_map_id;    /* DSCP based PHB map ID. */
    int vlan_pri_map_id;    /* VLAN priority and CFI based PHB map ID. */
    bcm_flow_port_split_horizon_drop_t drop_mode;    /* Drop mode when the split horizon check fails. */
    int ingress_opaque_ctrl_id;    /* Ingress opaque control ID. */
} bcm_compat6526_flow_port_t;


#if defined(INCLUDE_L3)
/* Flow encap config structure */
typedef struct bcm_compat6526_flow_encap_config_s {
    bcm_vpn_t vpn;    /* VPN representing a vfi or vrf */
    bcm_gport_t flow_port;    /* flow port id representing a DVP */
    bcm_if_t intf_id;    /* l3 interface id */
    uint32 dvp_group;    /* DVP group ID */
    uint32 oif_group;    /* L3 OUT interface group ID */
    uint32 vnid;    /* VN_ID. */
    uint8 pri;    /* service tag priority. */
    uint8 cfi;    /* service tag cfi */
    uint16 tpid;    /* service tag tpid */
    bcm_vlan_t vlan;    /* service VLAN ID. */
    uint32 flags;    /* BCM_FLOW_ENCAP_FLAG_xxx. */
    uint32 options;    /* BCM_FLOW_ENCAP_OPTION_xxx. */
    bcm_flow_encap_criteria_t criteria;    /* flow encap criteria. */
    uint32 valid_elements;    /* bitmap of valid fields for the encap action */
    bcm_flow_handle_t flow_handle;    /* flow handle derived from flow name */
    uint32 flow_option;    /* flow option derived from flow option name */
    bcm_gport_t src_flow_port;    /* flow port id representing a SVP */
    uint32 class_id;    /* Class ID as a key in EFP. */
    uint32 port_group;    /* Port group ID. */
    int vlan_pri_map_id;    /* VLAN priority and CFI map ID. */
    uint32 function;    /* Function value */
} bcm_compat6526_flow_encap_config_t;

/* Add a flow encap entry for L2 tunnel or L3 tunnel. */
extern int bcm_compat6526_flow_encap_add(
    int unit,
    bcm_compat6526_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Delete a flow encap entry based on the given key.  */
extern int bcm_compat6526_flow_encap_delete(
    int unit,
    bcm_compat6526_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Get a flow encap entry based on the given key.  */
extern int bcm_compat6526_flow_encap_get(
    int unit,
    bcm_compat6526_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Set the flex counter object value to the encap entry. */
extern int bcm_compat6526_flow_encap_flexctr_object_set(
    int unit,
    bcm_compat6526_flow_encap_config_t *info,
    uint32 value);

/* Get the flex counter object value from the encap entry. */
extern int bcm_compat6526_flow_encap_flexctr_object_get(
    int unit,
    bcm_compat6526_flow_encap_config_t *info,
    uint32 *value);

/* Attach counter entries to the encap entry. */
extern int bcm_compat6526_flow_encap_stat_attach(
    int unit,
    bcm_compat6526_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 stat_counter_id);

/* Detach the counter entries from the encap entry. */
extern int bcm_compat6526_flow_encap_stat_detach(
    int unit,
    bcm_compat6526_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Get stat counter ID associated to the encap entry. */
extern int bcm_compat6526_flow_encap_stat_id_get(
    int unit,
    bcm_compat6526_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 *stat_counter_id);

/* Create a flow port
bcm_flow_port_create creates an Access or Network FLOW port */
extern int bcm_compat6526_flow_port_create(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6526_flow_port_t *flow_port);

/* Get FLOW port information. */
extern int bcm_compat6526_flow_port_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6526_flow_port_t *flow_port);

/* Get all FLOW port information. */
extern int bcm_compat6526_flow_port_get_all(
    int unit,
    bcm_vpn_t vpn,
    int port_max,
    bcm_compat6526_flow_port_t *flow_port,
    int *port_count);


#endif /* defined(INCLUDE_L3) */
#if defined(INCLUDE_BFD)
/* BFD endpoint object. */
typedef struct bcm_compat6526_bfd_endpoint_info_s {
    uint32 flags;    /* Control flags. */
    bcm_bfd_endpoint_t id;    /* Endpoint identifier. */
    bcm_bfd_endpoint_t remote_id;    /* Remote endpoint identifier. */
    bcm_bfd_tunnel_type_t type;    /* Type of BFD encapsulation. */
    bcm_gport_t gport;    /* Gport identifier. In case of VxLAN, Tunnel ID */
    bcm_gport_t tx_gport;    /* TX gport associated with this endpoint. */
    bcm_gport_t remote_gport;    /* Gport identifier of trapping destination: OAMP or CPU, local or remote. */
    int bfd_period;    /* For local endpoints, this is the BFD transmission period in ms. */
    bcm_vpn_t vpn;    /* VPN. */
    uint8 vlan_pri;    /* VLAN tag priority. */
    uint8 inner_vlan_pri;    /* Inner VLAN tag priority. */
    bcm_vrf_t vrf_id;    /* Vrf identifier. */
    bcm_mac_t dst_mac;    /* Destination MAC. In case of VxLAN, Inner/Overlay Destination MAC. */
    bcm_mac_t src_mac;    /* Source MAC. In case of VxLAN, Inner/Overlay Source MAC. */
    bcm_vlan_t pkt_inner_vlan_id;    /* TX Packet inner Vlan Id. In case of VxLAN, Inner/Overlay Inner VLAN Id and priority. */
    bcm_ip_t dst_ip_addr;    /* Destination IPv4 address. In case of VxLAN, Inner/Overlay Destination IPv4 address. */
    bcm_ip6_t dst_ip6_addr;    /* Destination IPv6 address. In case of VxLAN, Inner/Overlay Destination IPv6 address. */
    bcm_ip_t src_ip_addr;    /* Source IPv4 address. In case of VxLAN, Inner/Overlay Source IPv4 address. */
    bcm_ip6_t src_ip6_addr;    /* Source IPv6 address. In case of VxLAN, Inner/Overlay Source IPv6 address. */
    uint8 ip_tos;    /* IPv4 Tos / IPv6 Traffic Class. In case of VxLAN, Inner/Overlay IPv4 Tos / IPv6 Traffic Class. */
    uint8 ip_ttl;    /* IPv4 TTL / IPv6 Hop Count. In case of VxLAN, Inner/Overlay IPv4 TTL / IPv6 Hop Count. */
    bcm_ip_t inner_dst_ip_addr;    /* Inner destination IPv4 address. */
    bcm_ip6_t inner_dst_ip6_addr;    /* Inner destination IPv6 address. */
    bcm_ip_t inner_src_ip_addr;    /* Inner source IPv4 address. */
    bcm_ip6_t inner_src_ip6_addr;    /* Inner source IPv6 address. */
    uint8 inner_ip_tos;    /* Inner IPv4 Tos / IPv6 Traffic Class. */
    uint8 inner_ip_ttl;    /* Inner IPv4 TTL / IPv6 Hop Count. */
    uint32 udp_src_port;    /* UDP source port for Ipv4, Ipv6. */
    bcm_mpls_label_t label;    /* Incoming inner label. */
    bcm_mpls_label_t mpls_hdr;    /* MPLS Header. */
    bcm_mpls_egress_label_t egress_label;    /* The MPLS outgoing label information. */
    bcm_if_t egress_if;    /* Egress interface. */
    uint8 mep_id[BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH];    /* MPLS-TP CC/CV TLV and Source MEP ID. */
    uint8 mep_id_length;    /* Length of MPLS-TP CC/CV TLV and MEP-ID. */
    bcm_cos_t int_pri;    /* Egress queuing for outgoing BFD to remote. */
    uint8 cpu_qid;    /* CPU queue for BFD. */
    bcm_bfd_state_t local_state;    /* Local session state. */
    uint32 local_discr;    /* Local discriminator. */
    bcm_bfd_diag_code_t local_diag;    /* Local diagnostic code. */
    uint32 local_flags;    /* Flags combination on outgoing frames. */
    uint32 local_min_tx;    /* Desired local min tx interval. */
    uint32 local_min_rx;    /* Required local rx interval. */
    uint32 local_min_echo;    /* Local minimum echo interval. */
    uint8 local_detect_mult;    /* Local detection interval multiplier. */
    bcm_bfd_auth_type_t auth;    /* Authentication type. */
    uint32 auth_index;    /* Authentication index. */
    uint32 tx_auth_seq;    /* Tx authentication sequence id. */
    uint32 rx_auth_seq;    /* Rx authentication sequence id. */
    uint32 remote_flags;    /* Remote flags. */
    bcm_bfd_state_t remote_state;    /* Remote session state. */
    uint32 remote_discr;    /* Remote discriminator. */
    bcm_bfd_diag_code_t remote_diag;    /* Remote diagnostic code. */
    uint32 remote_min_tx;    /* Desired remote min tx interval. */
    uint32 remote_min_rx;    /* Required remote rx interval. */
    uint32 remote_min_echo;    /* Remote minimum echo interval. */
    uint8 remote_detect_mult;    /* Remote detection interval multiplier. */
    int sampling_ratio;    /* 0 - No packets sampled to the CPU. 1-8 - Count of packets (with events) that need to arrive before one is sampled to the CPU. */
    uint8 loc_clear_threshold;    /* Number of packets required to reset the Loss-of-Continuity status per endpoint. */
    uint32 ip_subnet_length;    /* The subnet length for incoming packet validity check. Value 0 indicates no check is performed, positive values indicate the amount of MSBs to be compared. */
    uint8 remote_mep_id[BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH];    /* MPLS-TP CC/CV TLV and Remote MEP ID. */
    uint8 remote_mep_id_length;    /* Length of MPLS-TP CC/CV TLV and REMOTE MEP-ID. */
    uint8 mis_conn_mep_id[BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH];    /* MPLS-TP CC/CV TLV and Mis connectivity MEP ID. */
    uint8 mis_conn_mep_id_length;    /* Length of MPLS-TP CC/CV TLV and Mis connectivity MEP-ID. */
    uint32 bfd_detection_time;    /* Optional: BFD Detection Time, in microseconds. */
    bcm_vlan_t pkt_vlan_id;    /* TX Packet Vlan Id. In case of VxLAN, Inner/Overlay VLAN Id and priority. */
    bcm_vlan_t rx_pkt_vlan_id;    /* RX Packet Vlan Id. */
    bcm_mpls_label_t gal_label;    /* MPLS GAL label. */
    uint32 faults;    /* Fault flags. */
    uint32 flags2;    /* Second set of control flags. */
    int ipv6_extra_data_index;    /* Pointer to first extended data entry */
    int punt_good_packet_period;    /* BFD good packets sampling period. Every punt_good_packet_period milliseconds, a single packet is punted to the CPU */
    uint8 bfd_period_cluster;    /* Tx period group. All MEPs in a group must be created with the same Tx period. Modifying the Tx period in one MEP affects the others. */
    uint32 vxlan_vnid;    /* VxLAN Network Identifier. */
    uint16 vlan_tpid;    /* Vlan Tag Protocol Identifier. In case of VxLAN, Inner/Overlay Vlan Tag Protocol Identifier. */
    uint16 inner_vlan_tpid;    /* Inner Vlan Tag Protocol Identifier. In case of VxLAN, Inner/Overlay inner Vlan Tag Protocol Identifier. */
    int injected_network_qos;    /* QoS of the injected network packets. */
} bcm_compat6526_bfd_endpoint_info_t;

/* Create or update an BFD endpoint object */
extern int bcm_compat6526_bfd_endpoint_create(
    int unit,
    bcm_compat6526_bfd_endpoint_info_t *endpoint_info);

/* Get an BFD endpoint object. */
extern int bcm_compat6526_bfd_endpoint_get(
    int unit,
    bcm_bfd_endpoint_t endpoint,
    bcm_compat6526_bfd_endpoint_info_t *endpoint_info);


#endif /* defined(INCLUDE_BFD) */
#endif /* BCM_RPC_SUPPORT */
#endif /* !_COMPAT_6526_H */
