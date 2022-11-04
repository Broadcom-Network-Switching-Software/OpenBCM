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
#include <bcm/sat.h>
#include <bcm/pkt.h>
#include <bcm/switch.h>
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
#include <bcm/flexctr.h>
#include <bcm/flexstate.h>
#include <bcm/l2.h>
#include <bcm/trunk.h>
#include <bcm/udf.h>

#if defined(INCLUDE_L3)
#include <bcm/l3.h>
#include <bcm/nat.h>
#include <bcm/ipmc.h>
#include <bcm/tunnel.h>
#include <bcm/vxlan.h>
#include <bcm/l2gre.h>
#include <bcm/failover.h>
#endif /* defined(INCLUDE_L3) */

#include <bcm/policer.h>
#include <bcm/trunk.h>
#include <bcm/port.h>

#if defined(INCLUDE_L3)
/* L3 tunneling terminator. */
typedef struct bcm_compat6526_tunnel_terminator_s {
    uint32 flags;    /* Configuration flags. */
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
    bcm_pbmp_t pbmp;    /* Port bitmap for this tunnel */
    bcm_vlan_t vlan;    /* The VLAN ID or L3 ingress interface ID for IPMC lookup or WLAN tunnel */
    bcm_vlan_t fwd_vlan;    /* VLAN ID for forwarding terminated IP Tunnel */
    bcm_gport_t remote_port;    /* Remote termination */
    bcm_gport_t tunnel_id;    /* Tunnel id */
    int if_class;    /* L3 interface class this tunnel. */
    bcm_multicast_t failover_mc_group;    /* MC group used for bi-cast. */
    bcm_failover_t ingress_failover_id;    /* 1+1 protection. */
    bcm_failover_t failover_id;    /* Failover Object Identifier for protected tunnel. */
    bcm_gport_t failover_tunnel_id;    /* Failover Tunnel ID. */
    bcm_if_t tunnel_if;    /* hierarchical interface. */
    int tunnel_class;    /* Tunnel class id for VFP match. */
    int qos_map_id;    /* QoS DSCP map this tunnel. */
    int inlif_counting_profile;    /* In LIF counting profile */
    int tunnel_term_ecn_map_id;    /* Tunnel termination ecn map id. */
    bcm_qos_ingress_model_t ingress_qos_model;    /* ingress qos and ttl model */
    int priority;    /* Entry priority. */
    int subport_pbmp_profile_id;    /* profile id for allowed subports on this tunnel. */
    bcm_if_t egress_if;    /* Tunnel egress interface */
    bcm_vrf_t default_vrf;    /* default virtual router instance. */
    uint32 sci;    /* secure context index. */
    bcm_gport_t default_tunnel_id;    /* Use default tunnel ID for cascaded */
} bcm_compat6526_tunnel_terminator_t;

/* Add a tunnel terminator for DIP-SIP key. */
extern int bcm_compat6526_tunnel_terminator_add(
    int unit,
    bcm_compat6526_tunnel_terminator_t *info);

/* Delete a tunnel terminator for DIP-SIP key. */
extern int bcm_compat6526_tunnel_terminator_delete(
    int unit,
    bcm_compat6526_tunnel_terminator_t *info);

/* Update a tunnel terminator for DIP-SIP key. */
extern int bcm_compat6526_tunnel_terminator_update(
    int unit,
    bcm_compat6526_tunnel_terminator_t *info);

/* Get a tunnel terminator for DIP-SIP key. */
extern int bcm_compat6526_tunnel_terminator_get(
    int unit,
    bcm_compat6526_tunnel_terminator_t *info);

/* Create VXLAN Tunnel terminator. */
extern int bcm_compat6526_vxlan_tunnel_terminator_create(
    int unit,
    bcm_compat6526_tunnel_terminator_t *info);

/* Update multicast/active state per VXLAN Tunnel terminator. */
extern int bcm_compat6526_vxlan_tunnel_terminator_update(
    int unit,
    bcm_compat6526_tunnel_terminator_t *info);

/* Get VXLAN Tunnel terminator. */
extern int bcm_compat6526_vxlan_tunnel_terminator_get(
    int unit,
    bcm_compat6526_tunnel_terminator_t *info);

/* Create L2GRE Tunnel terminator. */
extern int bcm_compat6526_l2gre_tunnel_terminator_create(
    int unit,
    bcm_compat6526_tunnel_terminator_t *info);

/* Update L2GRE Tunnel terminator. */
extern int bcm_compat6526_l2gre_tunnel_terminator_update(
    int unit,
    bcm_compat6526_tunnel_terminator_t *info);

/* Get L2GRE Tunnel terminator. */
extern int bcm_compat6526_l2gre_tunnel_terminator_get(
    int unit,
    bcm_compat6526_tunnel_terminator_t *info);
#endif /* defined(INCLUDE_L3) */

/* Bit error mask to generate FEC error. */
typedef struct bcm_compat6526_port_phy_fec_error_mask_s {
    uint32 error_mask_bit_31_0;    /* Error mask bit 31-0. */
    uint32 error_mask_bit_63_32;    /* Error mask bit 63-32. */
    uint16 error_mask_bit_79_64;    /* Error mask bit 79-64. */
} bcm_compat6526_port_phy_fec_error_mask_t;

/* Set one-time fec error injection. */
extern int bcm_compat6526_port_phy_fec_error_inject_set(
    int unit,
    bcm_port_t port,
    uint16 error_control_map,
    bcm_compat6526_port_phy_fec_error_mask_t bit_error_mask);

/* Get configuration of one-time fec error injection. */
extern int bcm_compat6526_port_phy_fec_error_inject_get(
    int unit,
    bcm_port_t port,
    uint16 *error_control_map,
    bcm_compat6526_port_phy_fec_error_mask_t *bit_error_mask);

/*  */
typedef struct bcm_compat6526_policer_config_s {
    uint32 flags;    /* BCM_POLICER_* Flags. */
    bcm_policer_mode_t mode;    /* Policer mode. */
    uint32 ckbits_sec;    /* Committed rate (kbits per sec). */
    uint32 cbits_sec_lower;    /* Committed rate (lower digits; 0~999 bits per sec). */
    uint32 max_ckbits_sec;    /* Maximum Committed rate (kbits per sec), Applicable only for cascade mode. */
    uint32 max_cbits_sec_lower;    /* Maximum Committed rate (lower digits; 0~999 bits per sec). */
    uint32 ckbits_burst;    /* Committed burst size (kbits). */
    uint32 cbits_burst_lower;    /* Committed burst size (lower digits; 0~999 bits). */
    uint32 pkbits_sec;    /* Peak rate (kbits per sec). */
    uint32 pbits_sec_lower;    /* Peak rate (lower digits; 0~999 bits per sec). */
    uint32 max_pkbits_sec;    /* Maximum Peak rate (kbits per sec), Applicable only for cascade mode. */
    uint32 max_pbits_sec_lower;    /* Maximum Peak rate (lower digits; 0~999 bits per sec). */
    uint32 pkbits_burst;    /* Peak burst size (kbits). */
    uint32 pbits_burst_lower;    /* Peak burst size (lower digits; 0~999 bits). */
    uint32 kbits_current;    /* Current size (kbits). */
    uint32 bits_current_lower;    /* Current size (lower digits; 0~999 bits). */
    uint32 action_id;    /* Index of the global meter action entry */
    uint32 sharing_mode;    /* Meter sharing mode in case of Macro meter of Envelop */
    uint32 entropy_id;    /* If set, indicate the configuration group the policer belongs to */
    bcm_policer_pool_t pool_id;    /* Meter Pool Id of the current policer */
    uint32 bucket_width;    /* Bucket width in bits of the current policer */
    uint32 token_gran;    /* Token granularity of the current policer */
    uint32 bucket_init_val;    /* Initial bucket count of the current policer */
    uint32 bucket_rollover_val;    /* Rollover bucket count of the current policer */
    bcm_core_t core_id;    /* Device core on which to allocate the policer */
    bcm_policer_t ncoflow_policer_id;    /* Next lower priority committed policer ID to overflow to. Applicable in Cascaded  policer modes on MEF 10.3+ compliant devices */
    bcm_policer_t npoflow_policer_id;    /* Next lower priority peak policer ID to overflow to. Applicable in Cascaded and CoupledCascade policer modes on MEF 10.3+  compliant devices */
    uint32 actual_ckbits_sec;    /* Granularity adjusted Committed rate (kbits per sec). */
    uint32 actual_cbits_sec_lower;    /* Granularity adjusted Committed rate (lower digits; 0~999 bits per sec). */
    uint32 actual_ckbits_burst;    /* Granularity adjusted Committed burst size (kbits). */
    uint32 actual_cbits_burst_lower;    /* Granularity adjusted Committed burst size (lower digits; 0~999 bits). */
    uint32 actual_pkbits_sec;    /* Granularity adjusted Peak rate (kbits per sec). */
    uint32 actual_pbits_sec_lower;    /* Granularity adjusted Peak rate (lower digits; 0~999 bits per sec). */
    uint32 actual_pkbits_burst;    /* Granularity adjusted Peak burst size (kbits). */
    uint32 actual_pbits_burst_lower;    /* Granularity adjusted Peak burst size (lower digits; 0~999 bits). */
    uint32 average_pkt_size;    /* Estimated average Ethernet packet size (1 to 16383 Bytes) for more accurate metering. */
    int mark_drop_as_black;    /* For drop decision (no committed or Excess credits), mark it as black (otherwise, it will be marked as red). */
    int color_resolve_profile;    /* Input data for the resolved table. */
    uint32 meter_pair_index;    /* Meter pair index of this policer. */
    bcm_pbmp_t ports;    /* Policer member ports. */
} bcm_compat6526_policer_config_t;

/* Create a policer entry. */
extern int bcm_compat6526_policer_create(
    int unit,
    bcm_compat6526_policer_config_t *pol_cfg,
    bcm_policer_t *policer_id);

/* Set the config settings for a policer entry. */
extern int bcm_compat6526_policer_set(
    int unit,
    bcm_policer_t policer_id,
    bcm_compat6526_policer_config_t *pol_cfg);

/* Get the config settings for a policer entry. */
extern int bcm_compat6526_policer_get(
    int unit,
    bcm_policer_t policer_id,
    bcm_compat6526_policer_config_t *pol_cfg);

/* Trunk profile information structure. */
typedef struct bcm_compat6526_trunk_psc_profile_info_s {
    int weight_array[16];    /* Array of weights for members in trunk. */
    int weight_array_size;    /* actual Number of members in weight array */
    uint32 psc_flags;    /* BCM_TRUNK_PSC_xxx */
    int max_nof_members_in_profile;    /* max number of members in a profile */
} bcm_compat6526_trunk_psc_profile_info_t;

/* Trunk group attributes structure. */
typedef struct bcm_compat6526_trunk_info_s {
    uint32 flags;    /* BCM_TRUNK_FLAG_xxx. */
    int psc;    /* Port selection criteria. */
    bcm_compat6526_trunk_psc_profile_info_t psc_info;    /* Port selection criteria information. */
    int ipmc_psc;    /* Port selection criteria for software IPMC trunk resolution. */
    int dlf_index;    /* DLF/broadcast port for trunk group. */
    int mc_index;    /* Multicast port for trunk group. */
    int ipmc_index;    /* IPMC port for trunk group. */
    int dynamic_size;    /* Number of flows for dynamic load balancing. Valid values are 512, 1k, doubling up to 32k */
    int dynamic_age;    /* Inactivity duration, in microseconds. */
    int dynamic_load_exponent;    /* The exponent used in the exponentially weighted moving average calculation of historical member load. */
    int dynamic_expected_load_exponent;    /* The exponent used in the exponentially weighted moving average calculation of historical expected member load. */
    bcm_trunk_t master_tid;    /* Master trunk id. */
    bcm_trunk_dgm_t dgm;    /* DGM properties */
    uint32 rh_random_seed;    /* Random seed for the RH Trunk group. */
} bcm_compat6526_trunk_info_t;

/* Structure describing a trunk member. */
typedef struct bcm_compat6526_trunk_member_s {
    uint32 flags;    /* BCM_TRUNK_MEMBER_xxx */
    bcm_gport_t gport;    /* Trunk member GPORT ID. */
    int dynamic_scaling_factor;    /* Dynamic load balancing threshold scaling factor. */
    int dynamic_load_weight;    /* Relative weight of historical load in determining member quality. */
    int dynamic_queue_size_weight;    /* Relative weight of queue size in determining member quality. */
    uint32 spa_id;    /* spa_id of this member, provided together with BDM_TRUNK_MEMBER_SPA_WITH_ID flag */
} bcm_compat6526_trunk_member_t;

/* Get the current attributes and member ports for the specified trunk group. */
extern int bcm_compat6526_trunk_get(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6526_trunk_info_t *t_data,
    int member_max,
    bcm_compat6526_trunk_member_t *member_array,
    int *member_count);

/* Specify the ports in a trunk group. */
extern int bcm_compat6526_trunk_set(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6526_trunk_info_t *trunk_info,
    int member_count,
    bcm_compat6526_trunk_member_t *member_array);

/* Trunk multiset information structure. */
typedef struct bcm_compat6526_trunk_multiset_info_s {
    bcm_trunk_t tid;    /* Trunk ID */
    bcm_compat6526_trunk_info_t trunk_info;    /* Trunk info structure (see \ref bcm_trunk_info_t) */
    int member_count;    /* Number of members in this trunk. */
    int set_result;    /* Trunk set result (returned error) */
} bcm_compat6526_trunk_multiset_info_t;

/* Specify and update the ports in multiple trunk groups. */
extern int bcm_compat6526_trunk_multi_set(
    int unit,
    int trunk_count,
    bcm_compat6526_trunk_multiset_info_t *trunk_array,
    int tot_member_count,
    bcm_compat6526_trunk_member_t *member_array);

/* Add a member to a trunk group. */
extern int bcm_compat6526_trunk_member_add(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6526_trunk_member_t *member);

/* Delete a member from a trunk group. */
extern int bcm_compat6526_trunk_member_delete(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6526_trunk_member_t *member);

/* Port Configuration structure. */
typedef struct bcm_compat6526_port_config_s {
    bcm_pbmp_t fe;    /* Mask of FE ports. */
    bcm_pbmp_t ge;    /* Mask of GE ports. */
    bcm_pbmp_t xe;    /* Mask of 10gig ports. */
    bcm_pbmp_t ce;    /* Mask of 100gig ports. */
    bcm_pbmp_t le;    /* Mask of 50gig ports. */
    bcm_pbmp_t cc;    /* Mask of 200gig ports. */
    bcm_pbmp_t cd;    /* Mask of 400gig ports. */
    bcm_pbmp_t e;    /* Mask of eth ports. */
    bcm_pbmp_t hg;    /* Mask of HiGig ports. */
    bcm_pbmp_t sci;    /* Mask of SCI ports. */
    bcm_pbmp_t sfi;    /* Mask of SFI ports. */
    bcm_pbmp_t spi;    /* Mask of SPI ports. */
    bcm_pbmp_t spi_subport;    /* Mask of SPI subports. */
    bcm_pbmp_t port;    /* Mask of all front panel ports. */
    bcm_pbmp_t cpu;    /* Mask of CPU ports. */
    bcm_pbmp_t all;    /* Mask of all ports. */
    bcm_pbmp_t stack_int;    /* Deprecated - unused. */
    bcm_pbmp_t stack_ext;    /* Mask of Stack ports. */
    bcm_pbmp_t tdm;    /* Mask of TDM ports. */
    bcm_pbmp_t pon;    /* Mask of PON ports. */
    bcm_pbmp_t llid;    /* Mask of LLID ports. */
    bcm_pbmp_t il;    /* Mask of ILKN ports. */
    bcm_pbmp_t xl;    /* Mask of XLAUI ports. */
    bcm_pbmp_t rcy;    /* Mask of RECYCLE ports. */
    bcm_pbmp_t sat;    /* Mask of SAT ports. */
    bcm_pbmp_t ipsec;    /* Mask of IPSEC ports. */
    bcm_pbmp_t per_pipe[BCM_PIPES_MAX];    /* Mask of ports per pipe. The number of pipes per device can be obtained via num_pipes field of bcm_info_t. */
    bcm_pbmp_t nif;    /* Mask of Network Interfaces ports. */
    bcm_pbmp_t control;    /* Mask of hot swap controlling ports. */
    bcm_pbmp_t eventor;    /* Mask of Eventor ports. */
    bcm_pbmp_t olp;    /* Mask of OLP ports. */
    bcm_pbmp_t oamp;    /* Mask of OAMP ports. */
    bcm_pbmp_t erp;    /* Mask of ERP ports. */
    bcm_pbmp_t roe;    /* Mask of ROE ports. */
    bcm_pbmp_t rcy_mirror;    /* Mask of mirror recycle ports (sniff) */
    bcm_pbmp_t per_pp_pipe[BCM_PP_PIPES_MAX];    /* Mask of ports per PP pipe. The number of PP pipes per device can be obtained via num_pp_pipes field of bcm_info_t. */
    bcm_pbmp_t stat;    /* Mask of stat ports. */
    bcm_pbmp_t crps;    /* Mask of crps ports. */
    bcm_pbmp_t lb;    /* Mask of lb ports. */
    bcm_pbmp_t d3c;    /* Mask of 800gig ports. */
    bcm_pbmp_t otn;    /* Mask of otn ports. */
} bcm_compat6526_port_config_t;

/* Retrieved the port configuration for the specified device. */
extern int bcm_compat6526_port_config_get(
    int unit,
    bcm_compat6526_port_config_t *config);

/* Flex state operation structure. */
typedef struct bcm_compat6526_flexstate_operation_s {
    bcm_flexstate_update_compare_t update_compare; /* Comparison operation in the
                                           conditional update logic. */
    bcm_flexstate_value_operation_t compare_operation; /* Comparison value in the conditional
                                           update logic. */
    bcm_flexstate_value_operation_t operation_true; /* Update value when conditional update
                                           logic returns true. */
    bcm_flexstate_value_operation_t operation_false; /* Update value when conditional update
                                           logic returns false. */
    bcm_flexstate_update_source_t update_pool; /* Determines the source of counter pool
                                           update. */
    bcm_flexstate_update_source_t update_bus; /* Determines the source of bus update. */
} bcm_compat6526_flexstate_operation_t;

/* 
 * Flex state action structure.
 * 
 * This data structure lists the properties of a flex state action.
 */
typedef struct bcm_compat6526_flexstate_action_s {
    uint32 flags;                       /* Action flags. */
    bcm_flexstate_source_t source;      /* Flex state source. */
    bcm_pbmp_t ports;                   /* Flex state ports. */
    int hint;                           /* Flex state hint. */
    bcm_flexstate_drop_count_mode_t drop_count_mode; /* Counter drop mode for functional
                                           packet drops. */
    int exception_drop_count_enable;    /* Enable to count on hardware exception
                                           drops. */
    int egress_mirror_count_enable;     /* Enable to count egress mirrored
                                           packets also. */
    bcm_flexstate_counter_mode_t mode;  /* Action mode. */
    uint32 index_num;                   /* Total counter index number. */
    bcm_flexstate_index_operation_t index_operation; /* Flex state index operation structure. */
    bcm_compat6526_flexstate_operation_t operation_a; /* Flex state value A operation
                                           structure. */
    bcm_compat6526_flexstate_operation_t operation_b; /* Flex state value B operation
                                           structure. */
    bcm_flexstate_trigger_t trigger;    /* Flex state trigger structure. */
} bcm_compat6526_flexstate_action_t;

/* Flex counter action creation function. */
extern int bcm_compat6526_flexstate_action_create(
    int unit, 
    int options, 
    bcm_compat6526_flexstate_action_t *action, 
    uint32 *action_id);

/* Flex counter action get function. */
extern int bcm_compat6526_flexstate_action_get(
    int unit, 
    uint32 action_id, 
    bcm_compat6526_flexstate_action_t *action);

/*
 * Flex state quantization structure.
 *
 * This data structure lists the properties of a flex state quantization.
 * Each quantization instance generates a range check result.
 */
typedef struct bcm_compat6526_flexstate_quantization_s {
    bcm_flexstate_object_t object;      /* Flex state object. */
    bcm_flexstate_object_t object_upper; /* Flex state object for the upper 16
                                           bits when flag
                                           BCM_FLEXSTATE_QUANTIZATION_FLAGS_32BIT_MODE
                                           is set. The 'object' field is for the
                                           lower 16 bits. */
    bcm_pbmp_t ports;                   /* Flex state ports. */
    uint32 range_check_min[BCM_FLEXSTATE_QUANTIZATION_RANGE_SIZE]; /* Range check min value array. */
    uint32 range_check_max[BCM_FLEXSTATE_QUANTIZATION_RANGE_SIZE]; /* Range check max value array. */
    uint8 range_num;                    /* Total range check number */
    uint32 flags;                       /* Quantization flags. */
} bcm_compat6526_flexstate_quantization_t;

/* Flex counter quantization creation function. */
extern int bcm_compat6526_flexstate_quantization_create(
    int unit,
    int options,
    bcm_compat6526_flexstate_quantization_t *quantization,
    uint32 *quant_id);

/* Flex counter quantization get function. */
extern int bcm_compat6526_flexstate_quantization_get(
    int unit,
    uint32 quant_id,
    bcm_compat6526_flexstate_quantization_t *quantization);

/*
 * Flex counter quantization structure.
 *
 * This data structure lists the properties of a flex counter
 * quantization.
 * Each quantization instance generates a range check result.
 */
typedef struct bcm_compat6526_flexctr_quantization_s {
    bcm_flexctr_object_t object;        /* Flex counter object. */
    bcm_flexctr_object_t object_upper;  /* Flex counter object for the upper 16
                                           bits when flag
                                           BCM_FLEXCTR_QUANTIZATION_FLAGS_32BIT_MODE
                                           is set. The 'object' field is for the
                                           lower 16 bits. */
    bcm_pbmp_t ports;                   /* Flex counter ports. */
    uint32 range_check_min[BCM_FLEXCTR_QUANTIZATION_RANGE_SIZE]; /* Range check min value array. */
    uint32 range_check_max[BCM_FLEXCTR_QUANTIZATION_RANGE_SIZE]; /* Range check max value array. */
    uint8 range_num;                    /* Total range check number */
    uint32 object_id;                   /* Flex counter object ID. */
    uint32 object_id_upper;             /* Flex counter object ID (0-based
                                           index) for the counter object
                                           specified by 'object_upper' field (if
                                           required). Valid when flag
                                           BCM_FLEXCTR_QUANTIZATION_FLAGS_32BIT_MODE
                                           is set. */
    uint32 flags;                       /* Quantization flags. */
} bcm_compat6526_flexctr_quantization_t;

/* Flex counter quantization creation function. */
extern int bcm_compat6526_flexctr_quantization_create(
    int unit,
    int options,
    bcm_compat6526_flexctr_quantization_t *quantization,
    uint32 *quant_id);

/* Flex counter quantization get function. */
extern int bcm_compat6526_flexctr_quantization_get(
    int unit,
    uint32 quant_id,
    bcm_compat6526_flexctr_quantization_t *quantization);

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

/*
 * Delete a flow entry learnt by HW in exact match table. This API is
 * allowed only when flowtracker_hardware_learn_enable = 2.
 */
extern int bcm_compat6526_flowtracker_group_flow_delete(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_compat6526_flowtracker_group_flow_action_info_t *action_info);

/*
 * Modify the action data of a flow entry learnt by HW in exact match
 * table. Note that the action types should match whatever is configured
 * for the group already. This allows only changing the values of those
 * actions. This API is allowed only when
 * flowtracker_hardware_learn_enable = 2.
 */
extern int bcm_compat6526_flowtracker_group_flow_action_update(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_compat6526_flowtracker_group_flow_action_info_t *action_info,
    int num_actions,
    bcm_flowtracker_group_action_info_t *action_list);

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

/* Set flex counter object value for the given IPMC group. */
extern int bcm_compat6526_ipmc_flexctr_object_set(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
    uint32 value);

/* Get flex counter object value for the given IPMC group. */
extern int bcm_compat6526_ipmc_flexctr_object_get(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
    uint32 *value);

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

/* Find a longest prefix matched route given an IP address. */
extern int bcm_compat6526_l3_route_find(
    int unit,
    bcm_compat6526_l3_host_t *host,
    bcm_l3_route_t *route);

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

#if defined(INCLUDE_L3)
/* L3 Ingress Structure */
typedef struct bcm_compat6526_l3_ingress_s {
    uint32 flags;                       /* Interface flags. */
    bcm_vrf_t vrf;                      /* Virtual router id. */
    bcm_l3_ingress_urpf_mode_t urpf_mode; /* URPF mode setting for L3 Ingress
                                           Interface. */
    int intf_class;                     /* Classification class ID. */
    bcm_vlan_t ipmc_intf_id;            /* IPMC L2 distribution Vlan. */
    int qos_map_id;                     /* QoS DSCP priority map. */
    int ip4_options_profile_id;         /* IP4 Options handling Profile ID */
    int nat_realm_id;                   /* Realm id of the interface for NAT */
    int tunnel_term_ecn_map_id;         /* Tunnel termination ecn map id */
    uint32 intf_class_route_disable;    /* routing enablers bit map in rif
                                           profile */
    int oam_default_profile;            /* OAM profile for RIF */
    uint32 hash_layers_disable;         /* load balancing disable hash layers
                                           bit map using the
                                           BCM_HASH_LAYER_XXX_DISABLE flags */
    int opaque_ctrl_id;                 /* Opaque control ID. */
    bcm_l3_int_cosq_stat_t int_cosq_stat[BCM_L3_INT_MAX_COSQ_STAT]; /* Select INT COSQ stat. */
} bcm_compat6526_l3_ingress_t;

extern int bcm_compat6526_l3_ingress_create(
    int unit,
    bcm_compat6526_l3_ingress_t *ing_intf,
    bcm_if_t *intf_id);

extern int bcm_compat6526_l3_ingress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6526_l3_ingress_t *ing_intf);

extern int bcm_compat6526_l3_ingress_find(
    int unit,
    bcm_compat6526_l3_ingress_t *ing_intf,
    bcm_if_t *intf_id);

typedef struct bcm_compat6526_l3_egress_s {
    uint32 flags;                       /* Interface flags (BCM_L3_TGID,
                                           BCM_L3_L2TOCPU). */
    uint32 flags2;                      /* See BCM_L3_FLAGS2_xxx flag
                                           definitions. */
    uint32 flags3;                      /* See BCM_L3_FLAGS3_xxx flag
                                           definitions. */
    bcm_if_t intf;                      /* L3 interface (source MAC, tunnel). */
    bcm_mac_t mac_addr;                 /* Next hop forwarding destination mac. */
    bcm_vlan_t vlan;                    /* Next hop vlan id. */
    bcm_module_t module;
    bcm_port_t port;                    /* Port packet switched to (if
                                           !BCM_L3_TGID). */
    bcm_trunk_t trunk;                  /* Trunk packet switched to (if
                                           BCM_L3_TGID). */
    uint32 mpls_flags;                  /* BCM_MPLS flag definitions. */
    bcm_mpls_label_t mpls_label;        /* MPLS label. */
    bcm_mpls_egress_action_t mpls_action; /* MPLS action. */
    int mpls_qos_map_id;                /* MPLS EXP map ID. */
    int mpls_ttl;                       /* MPLS TTL threshold. */
    uint8 mpls_pkt_pri;                 /* MPLS Packet Priority Value. */
    uint8 mpls_pkt_cfi;                 /* MPLS Packet CFI Value. */
    uint8 mpls_exp;                     /* MPLS Exp. */
    int qos_map_id;                     /* General QOS map id */
    bcm_if_t encap_id;                  /* Encapsulation index. */
    bcm_failover_t failover_id;         /* Failover Object Index. */
    bcm_if_t failover_if_id;            /* Failover Egress Object index. */
    bcm_multicast_t failover_mc_group;  /* Failover Multicast Group. */
    int dynamic_scaling_factor;         /* Scaling factor for dynamic load
                                           balancing thresholds. */
    int dynamic_load_weight;            /* Weight of traffic load in determining
                                           a dynamic load balancing member's
                                           quality. */
    int dynamic_queue_size_weight;      /* Weight of queue size in determining a
                                           dynamic load balancing member's
                                           quality. */
    int intf_class;                     /* L3 interface class ID */
    uint32 multicast_flags;             /* BCM_L3_MULTICAST flag definitions. */
    uint16 oam_global_context_id;       /* OAM global context id passed from
                                           ingress to egress XGS chip. */
    bcm_vntag_t vntag;                  /* VNTAG. */
    bcm_vntag_action_t vntag_action;    /* VNTAG action. */
    bcm_etag_t etag;                    /* ETAG. */
    bcm_etag_action_t etag_action;      /* ETAG action. */
    uint32 flow_handle;                 /* FLOW handle for flex entries. */
    uint32 flow_option_handle;          /* FLOW option handle for flex entries. */
    uint32 flow_label_option_handle;    /* FLOW option handle for egress label
                                           flex entries. */
    bcm_flow_logical_field_t logical_fields[BCM_FLOW_MAX_NOF_LOGICAL_FIELDS]; /* logical fields array for flex
                                           entries. */
    uint32 num_of_fields;               /* number of logical fields. */
    int counting_profile;               /* counting profile. If not required,
                                           set it to
                                           BCM_STAT_LIF_COUNTING_PROFILE_NONE */
    int mpls_ecn_map_id;                /* IP ECN/INT CN to MPLS EXP map ID. */
    bcm_l3_ingress_urpf_mode_t urpf_mode; /* fec rpf mode. */
    bcm_multicast_t mc_group;           /* Multicast Group. */
    bcm_mac_t src_mac_addr;             /* Source MAC Address. */
    bcm_gport_t hierarchical_gport;     /* Hierarchical TM-Flow. */
    uint32 stat_id;                     /* Object statistics ID. */
    int stat_pp_profile;                /* Statistics profile. */
    bcm_gport_t vlan_port_id;           /* Pointer to egress vlan translation
                                           lif entry in EEDB. */
    int replication_id;                 /* Replication copy ID of multicast
                                           Egress object. */
    int mtu;                            /* MTU. */
    int nat_realm_id;                   /* NAT destination realm ID. */
    bcm_qos_egress_model_t egress_qos_model; /* egress qos and ttl model */
    int egress_opaque_ctrl_id;          /* Egress opaque control ID. */
    bcm_encap_access_t encap_access;    /* Encapsulation Access stage. */
    int ep_recirc_profile_id;           /* EPRC profile ID. */
} bcm_compat6526_l3_egress_t;

extern int bcm_compat6526_l3_egress_create(
    int unit,
    uint32 flags,
    bcm_compat6526_l3_egress_t *egr,
    bcm_if_t *if_id);

extern int bcm_compat6526_l3_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6526_l3_egress_t *egr);

extern int bcm_compat6526_l3_egress_find(
    int unit,
    bcm_compat6526_l3_egress_t *egr,
    bcm_if_t *intf);

extern int bcm_compat6526_l3_egress_allocation_get(
    int unit,
    uint32 flags,
    bcm_compat6526_l3_egress_t *egr,
    uint32 nof_members,
    bcm_if_t *if_id);

extern int bcm_compat6526_failover_egress_set(
    int unit,
    bcm_if_t intf,
    bcm_compat6526_l3_egress_t *failover_egr);

extern int bcm_compat6526_failover_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6526_l3_egress_t *failover_egr);

/* Egress nat table entry specification. */
typedef struct bcm_compat6526_l3_nat_egress_s {
    uint32 flags;               /* See BCM_L3_NAT_EGRESS_XXX flag definitions. */
    bcm_l3_nat_id_t nat_id;     /* Index into packet edit table. */
    bcm_ip_t sip_addr;          /* Translated source IP. */
    bcm_ip_t sip_addr_mask;     /* Bits of source IP prefix to translate. */
    uint16 src_port;            /* Source l4 port for NAPT */
    bcm_ip_t dip_addr;          /* Translated dest IP. */
    bcm_ip_t dip_addr_mask;     /* Bits of dest IP prefix to translate. */
    uint16 dst_port;            /* Destination l4 port for NAPT */
    bcm_l3_nat_id_t snat_id;    /* Key for source and twice NAT/NAPT hash edit
                                   table. */
    bcm_l3_nat_id_t dnat_id;    /* Key for destination and twice NAT/NAPT hash
                                   edit table. */
} bcm_compat6526_l3_nat_egress_t;

extern int bcm_compat6526_l3_nat_egress_add(
        int unit,
        bcm_compat6526_l3_nat_egress_t *nat_info);

extern int bcm_compat6526_l3_nat_egress_get(
        int unit,
        bcm_compat6526_l3_nat_egress_t *nat_info);

extern int bcm_compat6526_l3_nat_egress_destroy(
        int unit,
        bcm_compat6526_l3_nat_egress_t *nat_info);

extern int bcm_compat6526_l3_nat_egress_stat_attach(
        int unit,
        bcm_compat6526_l3_nat_egress_t *info,
        uint32 stat_counter_id);

extern int bcm_compat6526_l3_nat_egress_stat_detach(
        int unit,
        bcm_compat6526_l3_nat_egress_t *info);

extern int bcm_compat6526_l3_nat_egress_stat_counter_get(
        int unit,
        bcm_compat6526_l3_nat_egress_t *info,
        bcm_l3_nat_egress_stat_t stat,
        uint32 num_entries,
        uint32 *counter_indexes,
        bcm_stat_value_t *counter_values);

extern int bcm_compat6526_l3_nat_egress_stat_counter_sync_get(
        int unit,
        bcm_compat6526_l3_nat_egress_t *info,
        bcm_l3_nat_egress_stat_t stat,
        uint32 num_entries,
        uint32 *counter_indexes,
        bcm_stat_value_t *counter_values);

extern int bcm_compat6526_l3_nat_egress_stat_counter_set(
        int unit,
        bcm_compat6526_l3_nat_egress_t *info,
        bcm_l3_nat_egress_stat_t stat,
        uint32 num_entries,
        uint32 *counter_indexes,
        bcm_stat_value_t *counter_values);

extern int bcm_compat6526_l3_nat_egress_stat_multi_get(
        int unit,
        bcm_compat6526_l3_nat_egress_t *info,
        int nstat,
        bcm_l3_nat_egress_stat_t *stat_arr,
        uint64 *value_arr);

extern int bcm_compat6526_l3_nat_egress_stat_multi_get32(
        int unit,
        bcm_compat6526_l3_nat_egress_t *info,
        int nstat,
        bcm_l3_nat_egress_stat_t *stat_arr,
        uint32 *value_arr);

extern int bcm_compat6526_l3_nat_egress_stat_multi_set(
        int unit,
        bcm_compat6526_l3_nat_egress_t *info,
        int nstat,
        bcm_l3_nat_egress_stat_t *stat_arr,
        uint64 *value_arr);

extern int bcm_compat6526_l3_nat_egress_stat_multi_set32(
        int unit,
        bcm_compat6526_l3_nat_egress_t *info,
        int nstat,
        bcm_l3_nat_egress_stat_t *stat_arr,
        uint32 *value_arr);

extern int bcm_compat6526_l3_nat_egress_stat_id_get(
        int unit,
        bcm_compat6526_l3_nat_egress_t *info,
        bcm_l3_nat_egress_stat_t stat,
        uint32 *stat_counter_id);

/* L3 Ingress NAT Structure. */
typedef struct bcm_compat6526_l3_nat_ingress_s {
    uint32 flags;           /* See BCM_L3_NAT_INGRESS_XXX flag definitions. */
    bcm_ip_t ip_addr;       /* IP address to be translated. */
    bcm_vrf_t vrf;          /* Virtual router instance. */
    uint16 l4_port;         /* TCP/UDP port. */
    uint8 ip_proto;         /* IP proto. */
    bcm_l3_nat_id_t nat_id; /* nat edit index to use on egress on hit. */
    bcm_cos_t pri;          /* New priority in packet. */
    int class_id;           /* Classification lookup class id. */
    bcm_if_t nexthop;       /* Nexthop of ecmp_ptr depending on MULTIPATH flag
                               setting. */
    int realm_id;           /* Realm ID for NAT. */
} bcm_compat6526_l3_nat_ingress_t;


/*
 * L3 Interface Structure.
 *
 * Contains information required for manipulating L3 interfaces.
 */
typedef struct bcm_compat6526_l3_intf_s {
    uint32 l3a_flags;                   /* See BCM_L3_FLAGS_XXX flags
                                           definitions. */
    uint32 l3a_flags2;                  /* See BCM_L3_FLAGS2_XXX flags
                                           definitions. */
    bcm_vrf_t l3a_vrf;                  /* Virtual router instance. */
    bcm_if_t l3a_intf_id;               /* Interface ID. */
    bcm_mac_t l3a_mac_addr;             /* MAC address. */
    bcm_vlan_t l3a_vid;                 /* VLAN ID. */
    bcm_vlan_t l3a_inner_vlan;          /* Inner vlan for double tagged packets. */
    int l3a_tunnel_idx;                 /* Tunnel (initiator) index. */
    int l3a_ttl;                        /* TTL threshold. */
    int l3a_mtu;                        /* MTU. */
    int l3a_mtu_forwarding;             /* Forwarding Layer MTU. */
    bcm_if_group_t l3a_group;           /* Interface group number. */
    bcm_l3_intf_qos_t vlan_qos;         /* Outer-Vlan QoS Setting. */
    bcm_l3_intf_qos_t inner_vlan_qos;   /* Inner-Vlan QoS Setting. */
    bcm_l3_intf_qos_t dscp_qos;         /* DSCP QoS Setting. */
    int l3a_intf_class;                 /* L3 interface class ID */
    int l3a_ip4_options_profile_id;     /* IP4 Options handling Profile ID */
    int l3a_nat_realm_id;               /* Realm id of the interface for NAT */
    uint16 outer_tpid;                  /* TPID value */
    uint32 l3a_intf_flags;              /* See BCM_L3_INTF_XXX flag definitions. */
    uint8 native_routing_vlan_tags;     /* Set number of VLAN tags expected when
                                           interface is used for native routing */
    bcm_gport_t l3a_source_vp;          /* Source virtual port in overlay
                                           domain. SVP is used for deriving port
                                           properties in the egress device when
                                           packets are sent over HG flow. */
    uint32 flow_handle;                 /* FLOW handle for flex entries. */
    uint32 flow_option_handle;          /* FLOW option handle for flex entries. */
    bcm_flow_logical_field_t logical_fields[BCM_FLOW_MAX_NOF_LOGICAL_FIELDS]; /* logical fields array for flex
                                           entries. */
    uint32 num_of_fields;               /* number of logical fields. */
    bcm_qos_ingress_model_t ingress_qos_model; /* ingress qos and ttl model */
    bcm_qos_egress_model_t egress_qos_model; /* egress qos and ttl model */
    uint32 stat_id;                     /* Object statistics ID. */
    int stat_pp_profile;                /* Statistics profile. */
    uint8 l3a_ttl_dec;                  /* TTL decrement value. */
    int opaque_ctrl_id;                 /* Opqaue control ID for EFP key
                                           selection. */
    uint32 l3a_mpls_flags;              /* BCM_MPLS flag definitions. */
    bcm_mpls_label_t l3a_mpls_label;    /* MPLS VC label. */
    uint8 l3a_mpls_ttl;                 /* MPLS TTL threshold. */
    uint8 l3a_mpls_exp;                 /* MPLS Exp. */
} bcm_compat6526_l3_intf_t;

/* Create a new L3 interface. */
extern int bcm_compat6526_l3_intf_create(
    int unit,
    bcm_compat6526_l3_intf_t *intf);

/* Delete an L3 interface */
extern int bcm_compat6526_l3_intf_delete(
    int unit,
    bcm_compat6526_l3_intf_t *intf);

/* Search for L3 interface by MAC address and VLAN. */
extern int bcm_compat6526_l3_intf_find(
    int unit,
    bcm_compat6526_l3_intf_t *intf);

/* Search for L3 interface by VLAN only. */
extern int bcm_compat6526_l3_intf_find_vlan(
    int unit,
    bcm_compat6526_l3_intf_t *intf);

/* Given the L3 interface number, return the interface information. */
extern int bcm_compat6526_l3_intf_get(
    int unit,
    bcm_compat6526_l3_intf_t *intf);

/* Set the tunneling initiator parameters on an L3 interface. */
extern int bcm_compat6526_tunnel_initiator_set(
    int unit,
    bcm_compat6526_l3_intf_t *intf,
    bcm_tunnel_initiator_t *tunnel);

/* Set the tunneling initiator parameters on an L3 interface. */
extern int bcm_compat6526_tunnel_initiator_create(
    int unit,
    bcm_compat6526_l3_intf_t *intf,
    bcm_tunnel_initiator_t *tunnel);

/* Delete the tunnel association for the given L3 interface. */
extern int bcm_compat6526_tunnel_initiator_clear(
    int unit,
    bcm_compat6526_l3_intf_t *intf);

/* Get the tunnel property for the given L3 interface. */
extern int bcm_compat6526_tunnel_initiator_get(
    int unit,
    bcm_compat6526_l3_intf_t *intf,
    bcm_tunnel_initiator_t *tunnel);

#endif
#if defined(INCLUDE_L3)
/* Flow stat info structure */
typedef struct bcm_compat6526_flow_stat_info_s {
    bcm_gport_t flow_port;    /* Flow port */
    bcm_gport_t port;    /* Match port /trunk */
    bcm_vpn_t vpn;    /* VPN representing a vfi or vrf */
    bcm_vlan_t inner_vlan;    /* Inner VLAN ID to match. */
    bcm_vlan_t vlan;    /* VLAN ID to match. */
    uint32 vnid;    /* VN_ID. */
    bcm_ip_t sip;    /* source IPv4 address */
    bcm_ip_t dip;    /* destination IPv4 address */
    bcm_ip_t sip_mask;    /* source IPv4 address mask */
    bcm_ip_t dip_mask;    /* destination IPv4 address mask */
    bcm_ip6_t sip6;    /* source IPv6 address */
    bcm_ip6_t dip6;    /* destination IPv6 address */
    bcm_ip6_t sip6_mask;    /* source IPv6 address mask */
    bcm_ip6_t dip6_mask;    /* destination IPv6 address mask */
    uint16 udp_src_port;    /* udp source port. */
    uint16 udp_dst_port;    /* udp destination port. */
    uint16 protocol;    /* IP protocol type. */
    bcm_gport_t tunnel_id;    /* Tunnel ID */
    bcm_if_t egress_if;    /* egress object */
    bcm_if_t intf_id;    /* l3 interface id */
    bcm_mac_t mac;    /* 802.3 MAC address. */
    uint32 l3a_flags;    /* See BCM_L3_xxx flag definitions. */
    bcm_vrf_t l3a_vrf;    /* Virtual router instance. */
    uint32 oif_group;    /* L3 OUT interface group ID */
    uint32 dvp_group;    /* DVP group ID */
    bcm_flow_match_criteria_t match_criteria;    /* flow match criteria. */
    bcm_flow_encap_criteria_t encap_criteria;    /* flow encap criteria. */
    bcm_mpls_label_t mpls_label;    /* MPLS label. */
    uint32 valid_elements;    /* bitmap of valid fields */
    uint32 flow_handle;    /* flow handle derived from flow name */
    uint32 flow_option;    /* flow option derived from flow option name */
    bcm_flow_function_type_t function_type;    /* flow function type */
} bcm_compat6526_flow_stat_info_t;

/* Attach counter for an entry. */
extern int bcm_compat6526_flow_stat_attach(
    int unit,
    bcm_compat6526_flow_stat_info_t *flow_stat_info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 stat_counter_id);

/* Detach counter entries to the given flex view table. */
extern int bcm_compat6526_flow_stat_detach(
    int unit,
    bcm_compat6526_flow_stat_info_t *flow_stat_info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 stat_counter_id);

/* Get Stat Counter id for flow tables. */
extern int bcm_compat6526_flow_stat_id_get(
    int unit,
    bcm_compat6526_flow_stat_info_t *flow_stat_info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    bcm_stat_object_t stat_object,
    uint32 *stat_counter_id);


#endif /* defined(INCLUDE_L3) */
#if defined(INCLUDE_L3)
/* Flow tunnel SRV6 header structure. */
typedef struct bcm_compat6526_flow_tunnel_srv6_srh_s {
    uint32 flags;    /* Configuration flags. See BCM_FLOW_TUNNEL_SRV6_SRH_xxx */
    bcm_gport_t tunnel_id;    /* Tunnel ID */
    uint8 next_hdr;    /* next header type in the SRH */
    uint16 tag;    /* Tag field in the SRH */
    bcm_if_t egr_obj;    /* Egress Object for Non-Canonical tunnel */
    uint32 valid_elements;    /* bitmap of valid fields */
} bcm_compat6526_flow_tunnel_srv6_srh_t;

/* Add a Flow tunnel SRv6 SRH entry */
extern int bcm_compat6526_flow_tunnel_srv6_srh_add(
    int unit,
    uint32 options,
    bcm_compat6526_flow_tunnel_srv6_srh_t *info,
    int nofs,
    bcm_ip6_t *sid_list);

/* Get the SRV6 SRH entry's configuration parameters */
extern int bcm_compat6526_flow_tunnel_srv6_srh_get(
    int unit,
    bcm_compat6526_flow_tunnel_srv6_srh_t *info,
    int max_nofs,
    bcm_ip6_t *sid_list,
    int *count);


#endif /* defined(INCLUDE_L3) */

/* Packet protocol match structure. */
typedef struct bcm_compat6526_switch_pkt_protocol_match_s {
    uint8 l2_iif_opaque_ctrl_id;        /* L2 iif opaque ctrl id. */
    uint8 l2_iif_opaque_ctrl_id_mask;   /* L2 iif opaque ctrl id mask. */
    bcm_vlan_t vfi;                     /* Vfi. */
    bcm_vlan_t vfi_mask;                /* Vfi mask. */
    bcm_mac_t macda;                    /* Macda. */
    bcm_mac_t macda_mask;               /* Macda mask. */
    uint16 ethertype;                   /* Ethertype. */
    uint16 ethertype_mask;              /* Ethertype mask. */
    uint16 arp_rarp_opcode;             /* Arp rarp opcode. */
    uint16 arp_rarp_opcode_mask;        /* Arp rarp opcode mask. */
    uint8 icmp_type;                    /* Icmp type. */
    uint8 icmp_type_mask;               /* Icmp type mask. */
    uint8 igmp_type;                    /* Igmp type. */
    uint8 igmp_type_mask;               /* Igmp type mask. */
    uint8 l4_valid;                     /* L4 valid. If set, layer 4 header
                                           present. */
    uint8 l4_valid_mask;                /* L4 valid mask. */
    uint16 l4_src_port;                 /* L4 src port. */
    uint16 l4_src_port_mask;            /* L4 src port mask. */
    uint16 l4_dst_port;                 /* L4 dst port. */
    uint16 l4_dst_port_mask;            /* L4 dst port mask. */
    uint16 l5_bytes_0_1;                /* L5 bytes 0 1. First 2 bytes of layer
                                           5 header. */
    uint16 l5_bytes_0_1_mask;           /* L5 bytes 0 1 mask. */
    uint16 outer_l5_bytes_0_1;          /* Outer l5 bytes 0 1. First 2 bytes of
                                           outer layer 5 header. */
    uint16 outer_l5_bytes_0_1_mask;     /* Outer l5 bytes 0 1 mask. */
    uint8 ip_last_protocol;             /* Ip last protocol. Protocol from IP
                                           extension header if present,
                                           otherwise from IP header. */
    uint8 ip_last_protocol_mask;        /* Ip last protocol mask. */
    uint8 fixed_hve_result_set_1;       /* Fixed hve result set 1. */
    uint8 fixed_hve_result_set_1_mask;  /* Fixed hve result set 1 mask. */
    uint8 fixed_hve_result_set_5;       /* Fixed hve result set 5. */
    uint8 fixed_hve_result_set_5_mask;  /* Fixed hve result set 5 mask. */
    uint8 flex_hve_result_set_1;        /* Flex hve result set 1. */
    uint8 flex_hve_result_set_1_mask;   /* Flex hve result set 1 mask. */
    uint8 tunnel_processing_results_1;  /* Tunnel processing results 1. */
    uint8 tunnel_processing_results_1_mask; /* Tunnel processing results 1 mask. */
    uint8 vfp_opaque_ctrl_id;           /* Vfp opaque ctrl id. */
    uint8 vfp_opaque_ctrl_id_mask;      /* Vfp opaque ctrl id mask. */
    uint8 vlan_xlate_opaque_ctrl_id;    /* Vlan xlate opaque ctrl id.
                                           Bit[0] - Vlan xlate drop;
                                           Bit[1] - Vlan xlate copy to CPU;
                                           Bit[3:2] - The highest 2 bits of vlan
                                           xlate opaque ctrl id; */
    uint8 vlan_xlate_opaque_ctrl_id_mask; /* Vlan xlate opaque ctrl id mask. */
    uint8 vlan_xlate_opaque_ctrl_id_1;  /* Vlan xlate opaque ctrl id 1.
                                           Bit[3:0] - The lowest 4 bits of vlan
                                           xlate opaque ctrl id. */
    uint8 vlan_xlate_opaque_ctrl_id_1_mask; /* Vlan xlate opaque ctrl id 1 mask. */
    uint8 l2_iif_opaque_ctrl_id_1;      /* L2 iif opaque ctrl id 1. */
    uint8 l2_iif_opaque_ctrl_id_1_mask; /* L2 iif opaque ctrl id 1 mask. */
    uint8 fixed_hve_result_set_4;       /* Fixed hve result set 4. */
    uint8 fixed_hve_result_set_4_mask;  /* Fixed hve result set 4 mask. */
    uint8 outer_l2_valid_otag_present;  /* Outer l2 valid otag present. */
    uint8 outer_l2_valid_otag_present_mask; /* Outer l2 valid otag present mask. */
    bcm_vlan_t ovid;                    /* Ovid. */
    bcm_vlan_t ovid_mask;               /* Ovid mask. */
    uint8 ip_hdr_protocol;              /* Ip hdr protocol. Valid for outer
                                           header. */
    uint8 ip_hdr_protocol_mask;         /* Ip hdr protocol mask. Valid for outer
                                           header. */
    bcm_port_t ingress_pp_port;         /* Ingress pp port. */
    bcm_port_t ingress_pp_port_mask;    /* Ingress pp port mask. */
} bcm_compat6526_switch_pkt_protocol_match_t;

/* An API to add a protocol control entry. */
extern int bcm_compat6526_switch_pkt_protocol_control_add(
    int unit,
    uint32 options,
    bcm_compat6526_switch_pkt_protocol_match_t *match,
    bcm_switch_pkt_control_action_t *action,
    int priority);

/* An API to get action and priority for a protocol control entry. */
extern int bcm_compat6526_switch_pkt_protocol_control_get(
    int unit,
    bcm_compat6526_switch_pkt_protocol_match_t *match,
    bcm_switch_pkt_control_action_t *action,
    int *priority);

/* An API to delete a protocol control entry. */
extern int bcm_compat6526_switch_pkt_protocol_control_delete(
    int unit,
    bcm_compat6526_switch_pkt_protocol_match_t *match);

/* VLAN Egress Translate Key Configuration. */
typedef struct bcm_compat6526_vlan_translate_egress_key_config_s {
    uint32 flags;    /*  */
    bcm_vlan_translate_key_t key_type;    /* Lookup Key Type. */
    bcm_vlan_t inner_vlan;    /* Inner VLAN ID to match. */
    bcm_vlan_t outer_vlan;    /* Outer VLAN ID to match. */
    bcm_gport_t port;    /* Port to match on. */
    int otag_preserve;    /* Outer tag preserve to match on. */
    int itag_preserve;    /* Inner tag preserve to match on. */
    bcm_gport_t ingress_port;    /* Ingress port to match. */
    bcm_vlan_t ingress_outer_vlan;    /* Incoming packet's Outer VLAN ID to match. */
} bcm_compat6526_vlan_translate_egress_key_config_t;

/* Add an entry to the egress VLAN Translation table and assign VLAN actions. It extends bcm_vlan_egress_action_add API for additional keys. */
extern int bcm_compat6526_vlan_translate_egress_action_extended_add(
    int unit,
    bcm_compat6526_vlan_translate_egress_key_config_t *key_config,
    bcm_vlan_translate_egress_action_set_t *action);

/* Delete an egress translation entry including key types added by bcm_vlan_translate_egress_extended_action_add API. */
extern int bcm_compat6526_vlan_translate_egress_action_extended_delete(
    int unit,
    bcm_compat6526_vlan_translate_egress_key_config_t *key_config);

/* Retrive an egress translation entry including key types added by bcm_vlan_translate_egress_extended_action_add API. */
extern int bcm_compat6526_vlan_translate_egress_action_extended_get(
    int unit,
    bcm_compat6526_vlan_translate_egress_key_config_t *key_config,
    bcm_vlan_translate_egress_action_set_t *action);

/* Attach a flex counter into a given egress VLAN translation table with additional keys. */
extern int bcm_compat6526_vlan_translate_egress_flexctr_extended_attach(
    int unit,
    bcm_compat6526_vlan_translate_egress_key_config_t *key_config,
    bcm_vlan_flexctr_config_t *config);

/* Detach a flex counter from a given egress VLAN translation table with additional keys. */
extern int bcm_compat6526_vlan_translate_egress_flexctr_extended_detach(
    int unit,
    bcm_compat6526_vlan_translate_egress_key_config_t *key_config,
    bcm_vlan_flexctr_config_t *config);

/* Detach all flex counters from a given egress VLAN translation table with additional keys. */
extern int bcm_compat6526_vlan_translate_egress_flexctr_extended_detach_all(
    int unit,
    bcm_compat6526_vlan_translate_egress_key_config_t *key_config);

/* Get the attached flex counters from a given egress VLAN translation table with additional keys. */
extern int bcm_compat6526_vlan_translate_egress_flexctr_extended_get(
    int unit,
    bcm_compat6526_vlan_translate_egress_key_config_t *key_config,
    int array_size,
    bcm_vlan_flexctr_config_t *config_array,
    int *count);
/* Reflector encap data */
typedef struct bcm_compat6526_switch_reflector_data_s {
    bcm_switch_reflector_type_t type;    /* reflector type */
    bcm_mac_t mc_reflector_my_mac;    /* Source MAC address stamped on the response packet. Relevant for MC reflector only. */
    uint32 error_estimate;    /* ERROR_ESTIMATE */
    bcm_if_t next_encap_id;    /* Next pointer encap id for the RCH Ethernet encapsulation */
    uint32 reflector_member_link_id;    /* Reflector member link id */
} bcm_compat6526_switch_reflector_data_t;

/* Create/Destroy/Get encap ID for L2/L3 relfectors (both unicast and multicast) . */
extern int bcm_compat6526_switch_reflector_create(
    int unit,
    uint32 flags,
    bcm_if_t *encap_id,
    bcm_compat6526_switch_reflector_data_t *data);

/* Create/Destroy/Get encap ID for L2/L3 relfectors (both unicast and multicast) . */
extern int bcm_compat6526_switch_reflector_get(
    int unit,
    uint32 flags,
    bcm_if_t encap_id,
    bcm_compat6526_switch_reflector_data_t *data);


/* UDH header */
typedef struct bcm_compat6526_pkt_dnx_udh_s {
    uint8 size;    /* UDH size: 0/1/2/4 */
    uint32 data;    /* UDH data */
} bcm_compat6526_pkt_dnx_udh_t;

/* Mirror packet system header information for DNX devices */
typedef struct bcm_compat6526_mirror_header_info_s {
    bcm_mirror_pkt_dnx_ftmh_header_t tm;    /* TM header */
    bcm_mirror_pkt_dnx_pp_header_t pp;    /* PP header */
    bcm_compat6526_pkt_dnx_udh_t udh[4];    /* UDH header */
} bcm_compat6526_mirror_header_info_t;

/* Set system header information */
extern int bcm_compat6526_mirror_header_info_set(
    int unit,
    uint32 flags,
    bcm_gport_t mirror_dest_id,
    bcm_compat6526_mirror_header_info_t *mirror_header_info);

/* Get system header information */
extern int bcm_compat6526_mirror_header_info_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    uint32 *flags,
    bcm_compat6526_mirror_header_info_t *mirror_header_info);

/* Sat common configuration Object */
typedef struct bcm_compat6526_sat_config_s {
    bcm_sat_timestamp_format_t timestamp_format;    /* SAT timestamp format */
    uint32 config_flags;    /*  */
    uint32 seq_number_wrap_around;    /* SEQ number wrap-around */
} bcm_compat6526_sat_config_t;

/* Set common configuration. */
extern int bcm_compat6526_sat_config_set(
    int unit,
    bcm_compat6526_sat_config_t *conf);
typedef struct bcm_compat6526_udf_pkt_format_info_s {
    int prio;                           /* Priority of the UDF */
    bcm_ethertype_t ethertype;          /* EtherType in the packet */
    bcm_ethertype_t ethertype_mask;     /* EtherType Mask */
    uint8 ip_protocol;                  /* IP protocol field in the packet */
    uint8 ip_protocol_mask;             /* IP protocol Mask */
    uint16 l2;                          /* L2 packet format.
                                           (BCM_PKT_FORMAT_L2_XXX) */
    uint16 vlan_tag;                    /* Vlan tag format.
                                           (BCM_PKT_FORMAT_VLAN_XXX) */
    uint16 outer_ip;                    /* Outer IP header type.
                                           (BCM_PKT_FORMAT_IP_XXX) */
    uint16 inner_ip;                    /* Inner IP header type.
                                           (BCM_PKT_FORMAT_IP_XXX) */
    uint16 tunnel;                      /* Tunnel type.
                                           (BCM_PKT_FORMAT_TUNNEL_XXX) */
    uint16 mpls;                        /* Mpls labels.
                                           (BCM_PKT_FORMAT_MPLS_XXX) */
    uint16 fibre_chan_outer;            /* Fibre Channel outer header type.
                                           (BCM_PKT_FORMAT_FIBRE_CHAN_XXX) */
    uint16 fibre_chan_inner;            /* Fibre Channel inner header
                                           type.(BCM_PKT_FORMAT_FIBRE_CHAN_XXX) */
    uint16 higig;                       /* HIGIG packet
                                           format.(BCM_PKT_FORMAT_HIGIG_XXX) */
    uint16 vntag;                       /* NIV packet
                                           format.(BCM_PKT_FORMAT_VNTAG_XXX) */
    uint16 etag;                        /* Extender tag packet
                                           format.(BCM_PKT_FORMAT_ETAG_XXX) */
    uint16 cntag;                       /* CNTAG packet
                                           format.(BCM_PKT_FORMAT_CNTAG_XXX) */
    uint16 icnm;                        /* ICNM packet
                                           format.(BCM_PKT_FORMAT_ICNM_XXX) */
    uint16 subport_tag;                 /* SUBPORT_TAG packet format.
                                           (BCM_PKT_FORMAT_SUBPORT_TAG_XXX) */
    int class_id;                       /* id used by the IFP Key Selection
                                           Mechanism. Range 0~255 */
    uint16 inner_protocol;              /* Inner protocol field in the packet.
                                           Inner_ip should be set to
                                           BCM_PKT_FORMAT_IP_NONE to make this
                                           take effect. IP-IN-IP type GRE should
                                           take original API sequence. */
    uint16 inner_protocol_mask;         /* Inner protocol mask. */
    uint32 l4_dst_port;                 /* TCP or UDP Destination Port number. */
    uint32 l4_dst_port_mask;            /* TCP or UDP Destination Port number
                                           mask. */
    uint8 opaque_tag_type;              /* Tag type field in the opaque tag. */
    uint8 opaque_tag_type_mask;         /* Mask for tag type field in the opaque
                                           tag. */
    uint8 int_pkt;                      /* Inband Telemetry packet indicator. */
    uint8 src_port;                     /* Logical Source Port number. */
    uint8 src_port_mask;                /* Logical Source Port number mask. */
    uint8 lb_pkt_type;                  /* Loopback Packet Type.
                                           (BCM_PKT_FORMAT_LB_TYPE_XXX) */
    uint16 first_2bytes_after_mpls_bos; /* First 2 bytes after BOS in an mpls
                                           packet. */
    uint16 first_2bytes_after_mpls_bos_mask; /* Mask for first 2 bytes after BOS in
                                           an mpls packet. */
    uint8 outer_ifa;                    /* Outer IFA header
                                           (BCM_PKT_FORMAT_IFA_XXX) */
    uint8 inner_ifa;                    /* Inner IFA header
                                           (BCM_PKT_FORMAT_IFA_XXX) */
    uint16 ip_gre_first_2bytes;         /* First 2 bytes after L3 when packet is
                                           first fragment in IP-GRE packets. */
    uint16 ip_gre_first_2bytes_mask;    /* Mask of first 2 bytes after L3 when
                                           packet is first fragment in IP-GRE
                                           packets. */
    uint8 dcn_pkt;                      /* DCN packet */
    uint8 dcn_pkt_mask;                 /* Mask of DCN packet */
    uint8 byte_after_bos_label;         /* Byte after BOS Label */
    uint8 byte_after_bos_label_mask;    /* Mask of Byte after BOS Label */
    uint8 dcn_shim_extn_byte;           /* DCN SHIM Extension Byte */
    uint8 dcn_shim_extn_byte_mask;      /* Mask of DCN SHIM Extension Byte */
    uint8 dcn_shim_flags;               /* DCN SHIM Flags */
    uint8 dcn_shim_flags_mask;          /* Mask of DCN SHIM Flags */
    int gsh_header_type;                /* GSH Header Type */
    int module_header_type;             /* Module Header Type */
    uint8 svtag_present;                /* SVTAG present */
} bcm_compat6526_udf_pkt_format_info_t;

/* Create a packet format entry */
extern int bcm_compat6526_udf_pkt_format_create(
    int unit, 
    bcm_udf_pkt_format_options_t options, 
    bcm_compat6526_udf_pkt_format_info_t *pkt_format, 
    bcm_udf_pkt_format_id_t *pkt_format_id);

/* Retrieve packet format info given the packet format Id */
extern int bcm_compat6526_udf_pkt_format_info_get(
    int unit, 
    bcm_udf_pkt_format_id_t pkt_format_id, 
    bcm_compat6526_udf_pkt_format_info_t *pkt_format);

/* Get common configuration. */
extern int bcm_compat6526_sat_config_get(
    int unit,
    bcm_compat6526_sat_config_t *conf);

/* Switch drop event monitor structure. */
typedef struct bcm_compat6526_switch_drop_event_mon_s {
    bcm_pkt_drop_event_t drop_event;    /* Drop event. */
    uint32 actions;    /* BCM_SWITCH_MON_ACTION_XXX actions definitions. */
    uint32 flags;    /* BCM_SWITCH_DROP_EVENT_MON_FLAGS_XXX flags definitions. */
} bcm_compat6526_switch_drop_event_mon_t;

/* These APIs set/get monitor configuration for drop events defined in <bcm/types.h>. */
extern int bcm_compat6526_switch_drop_event_mon_set(
    int unit,
    bcm_compat6526_switch_drop_event_mon_t *monitor);

/* These APIs set/get monitor configuration for drop events defined in <bcm/types.h>. */
extern int bcm_compat6526_switch_drop_event_mon_get(
    int unit,
    bcm_compat6526_switch_drop_event_mon_t *monitor);

/* Parameters info for Field Action. */
typedef struct bcm_compat6526_field_action_params_s {
    uint32 param0;      /* First Parameter to Action */
    uint32 param1;      /* Second Parameter to Action */
    uint32 param2;      /* Third Parameter to Action */
    bcm_pbmp_t pbmp;    /* Port bitmap */
} bcm_compat6526_field_action_params_t;

/* Add an action to a field entry */
extern int bcm_compat6526_field_action_config_info_add(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_action_t action, 
    bcm_compat6526_field_action_params_t *params, 
    bcm_field_action_match_config_t *match_config);

/* 
 * Retrieve the parameters for an action previously added to a field
 * entry
 */
extern int bcm_compat6526_field_action_config_info_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_action_t action, 
    bcm_compat6526_field_action_params_t *params, 
    bcm_field_action_match_config_t *match_config);

#endif /* BCM_RPC_SUPPORT */
#endif /* !_COMPAT_6526_H */
