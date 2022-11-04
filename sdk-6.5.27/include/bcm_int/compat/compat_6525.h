/*
* $Id: compat_6525.h,v 1.0 2021/11/18
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2022 Broadcom Inc. All rights reserved.
*
* RPC Compatibility with sdk-6.5.25 routines
*/

#ifndef _COMPAT_6525_H_
#define _COMPAT_6525_H_

#ifdef BCM_RPC_SUPPORT
#include <bcm_int/compat/compat_6526.h>
#include <bcm/multicast.h>
#include <bcm/field.h>
#include <bcm/mirror.h>
#include <bcm/cosq.h>
#include <bcm/vlan.h>
#include <bcm/mpls.h>
#include <bcm/l2.h>
#include <bcm/types.h>
#include <bcm/subport.h>
#include <bcm/switch.h>
#include <bcm/flowtracker.h>
#include <bcm/ifa.h>
#include <bcm/tsn.h>


#if defined(INCLUDE_L3)
#include <bcm/l3.h>
#include <bcm/failover.h>
#endif /* defined(INCLUDE_L3) */

/* bcm_subport_config_t */
typedef struct bcm_compat6525_subport_config_s {
    bcm_gport_t group;    /* Subport group */
    bcm_vlan_t pkt_vlan;    /* Packet 16-bit VLAN tag */
    bcm_vlan_t inner_vlan;    /* 12-bit inner VLAN */
    bcm_vlan_t outer_vlan;    /* 12-bit outerVLAN */
    int int_pri;    /* Internal Priority for this subport port */
    uint32 prop_flags;    /* Property flags for this subport port */
    uint16 stream_id_array[BCM_SUBPORT_CONFIG_MAX_STREAMS];    /* LinkPHY fragment header Stream ID array to be associated with the LinkPHY subport port. */
    int num_streams;    /* Number of streams associated with the LinkPHY subport port. */
    bcm_color_t color;    /* (internal) color or drop precedence for this subport port. */
    bcm_subport_match_t criteria;    /* match criteria. */
    bcm_gport_t subport_modport;    /* CoE - System port associated with the subport tag */
} bcm_compat6525_subport_config_t;

/* Add a subport to a subport group. */
extern int bcm_compat6525_subport_port_add(
    int unit,
    bcm_compat6525_subport_config_t *config,
    bcm_gport_t *port);

/* Get the subport configuration for the given gport identifier. */
extern int bcm_compat6525_subport_port_get(
    int unit,
    bcm_gport_t port,
    bcm_compat6525_subport_config_t *config);


/* Device-independent L2 egress structure. */
typedef struct bcm_compat6525_l2_egress_s {
    uint32 flags;    /* BCM_L2_EGRESS_XXX flags. */
    bcm_mac_t dest_mac;    /* Destination MAC address to replace with. */
    bcm_mac_t src_mac;    /* Source MAC address to replace with. */
    bcm_vlan_t outer_vlan;    /* outer vlan tag to replace with. */
    bcm_vlan_t inner_vlan;    /* inner vlan tag to replace with. */
    bcm_ethertype_t ethertype;    /* EtherType to replace with. */
    bcm_gport_t dest_port;    /* Destination gport to associate this l2 egress entry with */
    bcm_if_t encap_id;    /* Encapsulation index */
    uint16 outer_tpid;    /* outer vlan tag TPID */
    uint8 outer_prio;    /* outer vlan tag User Priority */
    uint16 inner_tpid;    /* inner vlan tag TPID */
    bcm_if_t l3_intf;    /* L3 interface (tunnel). Used only in Native Routing overlay protocols. */
    int vlan_qos_map_id;    /* VLAN PCP-DEI QoS settings. Used only in Native Routing overlay protocols. */
    int mpls_extended_label_value;    /* An extended label placed on top of a an egress encapsulation. */
    bcm_gport_t vlan_port_id;    /* vlan gport identifier */
    int recycle_header_extension_length;    /* Specify in bytes the size of extension header after recycle header */
    int additional_egress_termination_size;    /* Specify in bytes the size of egress additional termination */
    bcm_l2_egress_recycle_app_t recycle_app;    /* Indicates the recycle app */
    bcm_if_t dest_encap_id;    /* Destination encapsulation index */
    bcm_vrf_t vrf;    /* Virtual router instance */
    bcm_qos_egress_model_t egress_qos_model;    /* egress qos model */
} bcm_compat6525_l2_egress_t;

/* Create an l2 egress entry. */
extern int bcm_compat6525_l2_egress_create(
    int unit,
    bcm_compat6525_l2_egress_t *egr);

/* Get the config of an l2 egress entry with specified encap_id. */
extern int bcm_compat6525_l2_egress_get(
    int unit,
    bcm_if_t encap_id,
    bcm_compat6525_l2_egress_t *egr);

/* Find the encap_id with a given egress config */
extern int bcm_compat6525_l2_egress_find(
    int unit,
    bcm_compat6525_l2_egress_t *egr,
    bcm_if_t *encap_id);


/* Device-independent L2 address structure. */
typedef struct bcm_compat6525_l2_addr_s {
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
} bcm_compat6525_l2_addr_t;

/* Add an L2 address entry to the specified device. */
extern int bcm_compat6525_l2_addr_add(
    int unit,
    bcm_compat6525_l2_addr_t *l2addr);

/* Check if an L2 entry is present in the L2 table. */
extern int bcm_compat6525_l2_addr_get(
    int unit,
    bcm_mac_t mac_addr,
    bcm_vlan_t vid,
    bcm_compat6525_l2_addr_t *l2addr);

/* Given an L2 address entry, return existing addresses which could conflict. */
extern int bcm_compat6525_l2_conflict_get(
    int unit,
    bcm_compat6525_l2_addr_t *addr,
    bcm_compat6525_l2_addr_t *cf_array,
    int cf_max,
    int *cf_count);

/* Get the specified L2 statistic from the chip. */
extern int bcm_compat6525_l2_stat_get(
    int unit,
    bcm_compat6525_l2_addr_t *l2_addr,
    bcm_l2_stat_t stat,
    uint64 *val);

/* Get the specified L2 statistic from the chip. */
extern int bcm_compat6525_l2_stat_get32(
    int unit,
    bcm_compat6525_l2_addr_t *l2_addr,
    bcm_l2_stat_t stat,
    uint32 *val);

/* Set the specified L2 statistic to the indicated value. */
extern int bcm_compat6525_l2_stat_set(
    int unit,
    bcm_compat6525_l2_addr_t *l2_addr,
    bcm_l2_stat_t stat,
    uint64 val);

/* Set the specified L2 statistic to the indicated value. */
extern int bcm_compat6525_l2_stat_set32(
    int unit,
    bcm_compat6525_l2_addr_t *l2_addr,
    bcm_l2_stat_t stat,
    uint32 val);

/* Enable/disable collection of statistics on the indicated L2 entry. */
extern int bcm_compat6525_l2_stat_enable_set(
    int unit,
    bcm_compat6525_l2_addr_t *l2_addr,
    int enable);

/* Replace destination (or delete) multiple L2 entries. */
extern int bcm_compat6525_l2_replace(
    int unit,
    uint32 flags,
    bcm_compat6525_l2_addr_t *match_addr,
    bcm_module_t new_module,
    bcm_port_t new_port,
    bcm_trunk_t new_trunk);

/* Replace destination (or delete) multiple L2 entries matching the given address and mask. Only bits that are 1 (turned on) in the mask will be checked when checking the match. */
extern int bcm_compat6525_l2_replace_match(
    int unit,
    uint32 flags,
    bcm_compat6525_l2_addr_t *match_addr,
    bcm_compat6525_l2_addr_t *mask_addr,
    bcm_compat6525_l2_addr_t *replace_addr,
    bcm_compat6525_l2_addr_t *replace_mask_addr);

/* Add mutli L2 address entry to the specified device. */
extern int bcm_compat6525_l2_addr_multi_add(
    int unit,
    bcm_compat6525_l2_addr_t *l2addr,
    int count);

/* Delete multi L2 address entry from the specified device. */
extern int bcm_compat6525_l2_addr_multi_delete(
    int unit,
    bcm_compat6525_l2_addr_t *l2addr,
    int count);

/* Retrieve a MAC entry according to mac, vid (used as VSI) and modid. When modid != 0 it should hold a VLAN ID for IVL entries. */
extern int bcm_compat6525_l2_addr_by_struct_get(
    int unit,
    bcm_compat6525_l2_addr_t *l2addr);

/* Delete a MAC entry according to mac, vid (used as VSI) and modid. When modid != 0 it should hold a VLAN ID for IVL entries. */
extern int bcm_compat6525_l2_addr_by_struct_delete(
    int unit,
    bcm_compat6525_l2_addr_t *l2addr);

/* Layer 2 Logical port type */
typedef struct bcm_compat6525_vlan_port_s {
    bcm_vlan_port_match_t criteria;    /* Match criteria. */
    uint32 flags;    /* BCM_VLAN_PORT_xxx. */
    bcm_vlan_t vsi;    /* Populated for bcm_compat6526_vlan_port_find only */
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
} bcm_compat6525_vlan_port_t;

/* Create a layer 2 logical port. */
extern int bcm_compat6525_vlan_port_create(
    int unit,
    bcm_compat6525_vlan_port_t *vlan_port);

/* Get/find a layer 2 logical port given the GPORT ID or match criteria. */
extern int bcm_compat6525_vlan_port_find(
    int unit,
    bcm_compat6525_vlan_port_t *vlan_port);


#if defined(INCLUDE_L3)
/* MPLS port type. */
typedef struct bcm_compat6525_mpls_port_s {
    bcm_gport_t mpls_port_id;    /* GPORT identifier. */
    uint32 flags;    /* BCM_MPLS_PORT_xxx. */
    uint32 flags2;    /* BCM_MPLS_PORT2_xxx. */
    int if_class;    /* Interface class ID. */
    int exp_map;    /* Incoming EXP map ID. */
    int int_pri;    /* Internal priority. */
    uint8 pkt_pri;    /* Packet priority. */
    uint8 pkt_cfi;    /* Packet CFI. */
    uint16 service_tpid;    /* Service VLAN TPID value. */
    bcm_gport_t port;    /* Match port and/or egress port. */
    bcm_mpls_port_match_t criteria;    /* Match criteria. */
    bcm_vlan_t match_vlan;    /* Outer VLAN ID to match. */
    bcm_vlan_t match_inner_vlan;    /* Inner VLAN ID to match. */
    bcm_mpls_label_t match_label;    /* VC label to match. */
    bcm_if_t egress_tunnel_if;    /* MPLS tunnel egress object. */
    bcm_mpls_egress_label_t egress_label;    /* Outgoing VC label. */
    int mtu;    /* MPLS port MTU. */
    bcm_vlan_t egress_service_vlan;    /* Service VLAN to Add/Replace. */
    uint32 pw_seq_number;    /* Initial-value of Pseudo-wire Sequence number */
    bcm_if_t encap_id;    /* Encap Identifier. */
    bcm_failover_t ingress_failover_id;    /* Ingress Failover Object Identifier. */
    bcm_gport_t ingress_failover_port_id;    /* Ingress Failover MPLS Port Identifier. */
    bcm_failover_t failover_id;    /* Failover Object Identifier. */
    bcm_gport_t failover_port_id;    /* Failover MPLS Port Identifier. */
    bcm_policer_t policer_id;    /* Policer ID to be associated with the MPLS gport */
    bcm_multicast_t failover_mc_group;    /* MC group used for bi-cast. */
    bcm_failover_t pw_failover_id;    /* Failover Object Identifier for Redundant PW. */
    bcm_gport_t pw_failover_port_id;    /* Redundant PW port Identifier. */
    bcm_mpls_port_control_channel_type_t vccv_type;    /* Indicate VCCV Control Channel */
    bcm_switch_network_group_t network_group_id;    /* Split Horizon network group identifier */
    bcm_vlan_t match_subport_pkt_vid;    /* LLTAG VLAN ID to match. */
    bcm_gport_t tunnel_id;    /* Tunnel Id pointing to ingress LSP. */
    bcm_gport_t per_flow_queue_base;    /* Base queue of the per flow queue set. Actual queue is decided based on internal priority and qos map. */
    int qos_map_id;    /* QOS map identifier. */
    bcm_failover_t egress_failover_id;    /* Failover object index for Egress Protection */
    bcm_gport_t egress_failover_port_id;    /* Failover MPLS Port identifier for Egress Protection */
    int ecn_map_id;    /* ECN map identifier. */
    uint32 class_id;    /* Class ID assigned during tunnel termination only. Not applicable for access flows */
    uint32 egress_class_id;    /* Egress Class ID */
    int inlif_counting_profile;    /* In LIF counting profile */
    bcm_mpls_egress_label_t egress_tunnel_label;    /* A mpls tunnel to be binded with the pwe */
    int nof_service_tags;    /* Number of Service-Tags expected on Native-ETH header */
    bcm_qos_ingress_model_t ingress_qos_model;    /* ingress qos&ttl model */
    bcm_mpls_label_t context_label;    /* Ingress match - context label. */
    bcm_if_t ingress_if;    /* Ingress match - context interface. */
    int port_group;    /* Port group to match. */
    int dscp_map_id;    /* DSCP based PHB map ID. */
    int vlan_pri_map_id;    /* VLAN priority and CFI based PHB map ID. */
    bcm_vlan_t match_default_vlan;    /* Default VLAN ID to match. */
    bcm_vpn_t match_default_vpn;    /* Default VPN ID to match. */
} bcm_compat6525_mpls_port_t;

/* Add an MPLS port to an L2 VPN. */
extern int bcm_compat6525_mpls_port_add(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6525_mpls_port_t *mpls_port);

/* Get an MPLS port from an L2 VPN. */
extern int bcm_compat6525_mpls_port_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6525_mpls_port_t *mpls_port);

/* Get all MPLS ports from an L2 VPN. */
extern int bcm_compat6525_mpls_port_get_all(
    int unit,
    bcm_vpn_t vpn,
    int port_max,
    bcm_compat6525_mpls_port_t *port_array,
    int *port_count);

/* L3 Egress Structure.

@p Description of an L3 forwarding destination.
 */
typedef struct bcm_compat6525_l3_egress_s {
    uint32 flags;    /* Interface flags (BCM_L3_TGID, BCM_L3_L2TOCPU). */
    uint32 flags2;    /* See BCM_L3_FLAGS2_xxx flag definitions. */
    uint32 flags3;   /* See BCM_L3_FLAGS3_xxx flag definitions. */
    bcm_if_t intf;    /* L3 interface (source MAC, tunnel). */
    bcm_mac_t mac_addr;    /* Next hop forwarding destination mac. */
    bcm_vlan_t vlan;    /* Next hop vlan id. */
    bcm_module_t module;    /*  */
    bcm_port_t port;    /* Port packet switched to (if !BCM_L3_TGID). */
    bcm_trunk_t trunk;    /* Trunk packet switched to (if BCM_L3_TGID). */
    uint32 mpls_flags;    /* BCM_MPLS flag definitions. */
    bcm_mpls_label_t mpls_label;    /* MPLS label. */
    bcm_mpls_egress_action_t mpls_action;    /* MPLS action. */
    int mpls_qos_map_id;    /* MPLS EXP map ID. */
    int mpls_ttl;    /* MPLS TTL threshold. */
    uint8 mpls_pkt_pri;    /* MPLS Packet Priority Value. */
    uint8 mpls_pkt_cfi;    /* MPLS Packet CFI Value. */
    uint8 mpls_exp;    /* MPLS Exp. */
    int qos_map_id;    /* General QOS map id */
    bcm_if_t encap_id;    /* Encapsulation index. */
    bcm_failover_t failover_id;    /* Failover Object Index. */
    bcm_if_t failover_if_id;    /* Failover Egress Object index. */
    bcm_multicast_t failover_mc_group;    /* Failover Multicast Group. */
    int dynamic_scaling_factor;    /* Scaling factor for dynamic load balancing thresholds. */
    int dynamic_load_weight;    /* Weight of traffic load in determining a dynamic load balancing member's quality. */
    int dynamic_queue_size_weight;    /* Weight of queue size in determining a dynamic load balancing member's quality. */
    int intf_class;    /* L3 interface class ID */
    uint32 multicast_flags;    /* BCM_L3_MULTICAST flag definitions. */
    uint16 oam_global_context_id;    /* OAM global context id passed from ingress to egress XGS chip. */
    bcm_vntag_t vntag;    /* VNTAG. */
    bcm_vntag_action_t vntag_action;    /* VNTAG action. */
    bcm_etag_t etag;    /* ETAG. */
    bcm_etag_action_t etag_action;    /* ETAG action. */
    uint32 flow_handle;    /* FLOW handle for flex entries. */
    uint32 flow_option_handle;    /* FLOW option handle for flex entries. */
    uint32 flow_label_option_handle;    /* FLOW option handle for egress label flex entries. */
    bcm_flow_logical_field_t logical_fields[BCM_FLOW_MAX_NOF_LOGICAL_FIELDS];    /* logical fields array for flex entries. */
    uint32 num_of_fields;    /* number of logical fields. */
    int counting_profile;    /* counting profile. If not required, set it to BCM_STAT_LIF_COUNTING_PROFILE_NONE */
    int mpls_ecn_map_id;    /* IP ECN/INT CN to MPLS EXP map ID. */
    bcm_l3_ingress_urpf_mode_t urpf_mode;    /* fec rpf mode. */
    bcm_multicast_t mc_group;    /* Multicast Group. */
    bcm_mac_t src_mac_addr;    /* Source MAC Address. */
    bcm_gport_t hierarchical_gport;    /* Hierarchical TM-Flow. */
    uint32 stat_id;    /* Object statistics ID. */
    int stat_pp_profile;    /* Statistics profile. */
    bcm_gport_t vlan_port_id;    /* Pointer to egress vlan translation lif entry in EEDB. */
    int replication_id;    /* Replication copy ID of multicast Egress object. */
    int mtu;    /* MTU. */
    int nat_realm_id;    /* NAT destination realm ID. */
    bcm_qos_egress_model_t egress_qos_model;    /* egress qos and ttl model */
    int egress_opaque_ctrl_id;    /* Egress opaque control ID. */
    bcm_encap_access_t encap_access;    /* Encapsulation Access stage. */
} bcm_compat6525_l3_egress_t;

/* Create an Egress forwarding object. */
extern int bcm_compat6525_l3_egress_create(
    int unit,
    uint32 flags,
    bcm_compat6525_l3_egress_t *egr,
    bcm_if_t *if_id);

/* Get an Egress forwarding object. */
extern int bcm_compat6525_l3_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6525_l3_egress_t *egr);

/* Find an interface pointing to an Egress forwarding object. */
extern int bcm_compat6525_l3_egress_find(
    int unit,
    bcm_compat6525_l3_egress_t *egr,
    bcm_if_t *intf);

/* Retrieve valid allocation for a given number of FECs. */
extern int bcm_compat6525_l3_egress_allocation_get(
    int unit,
    uint32 flags,
    bcm_compat6525_l3_egress_t *egr,
    uint32 nof_members,
    bcm_if_t *if_id);

/* Set a failover egress object with fixed offset to protect primary NH intf. */
extern int bcm_compat6525_failover_egress_set(
    int unit,
    bcm_if_t intf,
    bcm_compat6525_l3_egress_t *failover_egr);

/* Get a failover egress object with fixed offset to protect primary NH intf. */
extern int bcm_compat6525_failover_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6525_l3_egress_t *failover_egr);

/* L3 ECMP structure */
typedef struct bcm_compat6525_l3_egress_ecmp_s {
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
} bcm_compat6525_l3_egress_ecmp_t;

/* Resilient ecmp entry */
typedef struct bcm_compat6525_l3_egress_ecmp_resilient_entry_s {
    uint64 hash_key;            /* Hash key. */
    bcm_compat6525_l3_egress_ecmp_t ecmp;  /* ECMP. */
    bcm_if_t intf;              /* L3 interface. */
} bcm_compat6525_l3_egress_ecmp_resilient_entry_t;

/* Create an Egress ECMP forwarding object. */
extern int bcm_compat6525_l3_ecmp_create(
    int unit,
    uint32 options,
    bcm_compat6525_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_count,
    bcm_l3_ecmp_member_t *ecmp_member_array);

/* Get info about an Egress ECMP forwarding object. */
extern int bcm_compat6525_l3_ecmp_get(
    int unit,
    bcm_compat6525_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_size,
    bcm_l3_ecmp_member_t *ecmp_member_array,
    int *ecmp_member_count);

/* Find an Egress ECMP forwarding object. */
extern int bcm_compat6525_l3_ecmp_find(
    int unit,
    int ecmp_member_count,
    bcm_l3_ecmp_member_t *ecmp_member_array,
    bcm_compat6525_l3_egress_ecmp_t *ecmp_info);

/* Create an Egress ECMP forwarding object. */
extern int bcm_compat6525_l3_egress_ecmp_create(
    int unit,
    bcm_compat6525_l3_egress_ecmp_t *ecmp,
    int intf_count,
    bcm_if_t *intf_array);

/* Destroy an Egress ECMP forwarding object. */
extern int bcm_compat6525_l3_egress_ecmp_destroy(
    int unit,
    bcm_compat6525_l3_egress_ecmp_t *ecmp);

/* Get info about an Egress ECMP forwarding object. */
extern int bcm_compat6525_l3_egress_ecmp_get(
    int unit,
    bcm_compat6525_l3_egress_ecmp_t *ecmp,
    int intf_size,
    bcm_if_t *intf_array,
    int *intf_count);

/* Add an Egress forwarding object to an Egress ECMP forwarding object. */
extern int bcm_compat6525_l3_egress_ecmp_add(
    int unit,
    bcm_compat6525_l3_egress_ecmp_t *ecmp,
    bcm_if_t intf);

/*
 * Delete an Egress forwarding object from an Egress ECMP forwarding
 * object.
 */
extern int bcm_compat6525_l3_egress_ecmp_delete(
    int unit,
    bcm_compat6525_l3_egress_ecmp_t *ecmp,
    bcm_if_t intf);

/* Find an Egress ECMP forwarding object. */
extern int bcm_compat6525_l3_egress_ecmp_find(
    int unit,
    int intf_count,
    bcm_if_t *intf_array,
    bcm_compat6525_l3_egress_ecmp_t *ecmp);

/*
 * Update an ECMP table that was already allocated, used in cases where
 * an ECMP have more than one table.
 */
extern int bcm_compat6525_l3_egress_ecmp_tunnel_priority_set(
    int unit,
    bcm_compat6525_l3_egress_ecmp_t *ecmp,
    int intf_count,
    bcm_if_t *intf_array);

/* Replace ECMP resilient entries matching given criteria. */
extern int bcm_compat6525_l3_egress_ecmp_resilient_replace(
    int unit,
    uint32 flags,
    bcm_compat6525_l3_egress_ecmp_resilient_entry_t *match_entry,
    int *num_entries,
    bcm_compat6525_l3_egress_ecmp_resilient_entry_t *replace_entry);

/* Add ECMP resilient entries matching given criteria. */
extern int bcm_compat6525_l3_egress_ecmp_resilient_add(
    int unit,
    uint32 flags,
    bcm_compat6525_l3_egress_ecmp_resilient_entry_t *entry);

/* Delete ECMP resilient entries matching given criteria. */
extern int bcm_compat6525_l3_egress_ecmp_resilient_delete(
    int unit,
    uint32 flags,
    bcm_compat6525_l3_egress_ecmp_resilient_entry_t *entry);

#endif /* defined(INCLUDE_L3) */
/* EBST data buffer info structure */
typedef struct bcm_compat6525_cosq_ebst_data_entry_s {
    uint64 timestamp;    /* Timestamp */
    uint32 bytes;    /* Buffer occupancy level in bytes */
    bcm_cosq_color_packet_drop_state_t green_drop_state;    /* Green packet drop state */
    bcm_cosq_color_packet_drop_state_t yellow_drop_state;    /* Yellow packet drop state */
    bcm_cosq_color_packet_drop_state_t red_drop_state;    /* Red packet drop state */
} bcm_compat6525_cosq_ebst_data_entry_t;

/* Get EBST stat for a given stat Id. */
extern int bcm_compat6525_cosq_ebst_data_get(
    int unit,
    bcm_cosq_object_id_t *object_id,
    bcm_bst_stat_id_t bid,
    int array_size,
    bcm_compat6525_cosq_ebst_data_entry_t *entry_array,
    int *count);

/* Port Configuration structure. */
typedef struct bcm_compat6525_port_config_s {
    bcm_pbmp_t fe;                      /* Mask of FE ports. */
    bcm_pbmp_t ge;                      /* Mask of GE ports. */
    bcm_pbmp_t xe;                      /* Mask of 10gig ports. */
    bcm_pbmp_t ce;                      /* Mask of 100gig ports. */
    bcm_pbmp_t le;                      /* Mask of 50gig ports. */
    bcm_pbmp_t cc;                      /* Mask of 200gig ports. */
    bcm_pbmp_t cd;                      /* Mask of 400gig ports. */
    bcm_pbmp_t e;                       /* Mask of eth ports. */
    bcm_pbmp_t hg;                      /* Mask of HiGig ports. */
    bcm_pbmp_t sci;                     /* Mask of SCI ports. */
    bcm_pbmp_t sfi;                     /* Mask of SFI ports. */
    bcm_pbmp_t spi;                     /* Mask of SPI ports. */
    bcm_pbmp_t spi_subport;             /* Mask of SPI subports. */
    bcm_pbmp_t port;                    /* Mask of all front panel ports. */
    bcm_pbmp_t cpu;                     /* Mask of CPU ports. */
    bcm_pbmp_t all;                     /* Mask of all ports. */
    bcm_pbmp_t stack_int;               /* Deprecated - unused. */
    bcm_pbmp_t stack_ext;               /* Mask of Stack ports. */
    bcm_pbmp_t tdm;                     /* Mask of TDM ports. */
    bcm_pbmp_t pon;                     /* Mask of PON ports. */
    bcm_pbmp_t llid;                    /* Mask of LLID ports. */
    bcm_pbmp_t il;                      /* Mask of ILKN ports. */
    bcm_pbmp_t xl;                      /* Mask of XLAUI ports. */
    bcm_pbmp_t rcy;                     /* Mask of RECYCLE ports. */
    bcm_pbmp_t sat;                     /* Mask of SAT ports. */
    bcm_pbmp_t ipsec;                   /* Mask of IPSEC ports. */
    bcm_pbmp_t per_pipe[BCM_PIPES_MAX]; /* Mask of ports per pipe. The number of
                                           pipes per device can be obtained via
                                           num_pipes field of bcm_info_t. */
    bcm_pbmp_t nif;                     /* Mask of Network Interfaces ports. */
    bcm_pbmp_t control;                 /* Mask of hot swap controlling ports.*/
    bcm_pbmp_t eventor;                 /* Mask of Eventor ports. */
    bcm_pbmp_t olp;                     /* Mask of OLP ports. */
    bcm_pbmp_t oamp;                    /* Mask of OAMP ports. */
    bcm_pbmp_t erp;                     /* Mask of ERP ports. */
    bcm_pbmp_t roe;                     /* Mask of ROE ports. */
    bcm_pbmp_t rcy_mirror;              /* Mask of mirror recycle port (sniff)*/
    bcm_pbmp_t per_pp_pipe[BCM_PP_PIPES_MAX];   /* Mask of ports per PP pipe.
                                                   The number of PP pipes per
                                                   device can be obtained via
                                                   num_pp_pipes field of
                                                   bcm_info_t. */
    bcm_pbmp_t stat;                    /* Mask of stat ports. */
    bcm_pbmp_t crps;                    /* Mask of crps ports. */
    bcm_pbmp_t d3c;                     /* Mask of 800gig ports. */
    bcm_pbmp_t lb;                      /* Mask of lb ports. */
} bcm_compat6525_port_config_t;

/* Retrieved the port configuration for the specified device. */
extern int bcm_compat6525_port_config_get(
    int unit,
    bcm_compat6525_port_config_t *config);

/* Port Resource Configuration */
typedef struct bcm_compat6525_port_resource_s {
    uint32 flags;                   /* BCM_PORT_RESOURCE_XXX */
    bcm_gport_t port;               /* Local logical port number to connect to
                                       the given physical port */
    int physical_port;              /* Local physical port, -1 if the logical to
                                       physical mapping is to be deleted */
    int speed;                      /* Initial speed after FlexPort operation */
    int rx_speed;                   /* Indicate the Rx speed, valid only
                                       BCM_PORT_RESOURCE_ASYMMETRICAL flags is
                                       set */
    int lanes;                      /* Number of PHY lanes for this physical
                                       port */
    bcm_port_encap_t encap;         /* Encapsulation mode for port */
    bcm_port_phy_fec_t fec_type;    /* fec_type for port */
    int phy_lane_config;            /* serdes pmd lane config for port */
    int link_training;              /* link training on or off */
    int roe_compression;            /* enable roe_compression */
    int num_subports;               /* Number of subports if this is cascaded
                                       port. */
    int line_card;                  /* Line card number for the cascaded port */
    int base_flexe_instance;        /* Base FlexE instance ID. "-1" means the
                                       base instance ID is allocated by SW. For
                                       50G FlexE PHY, base instance ID can be
                                       0~7; For 100G FlexE PHY, valid values are
                                       0/2/4/6; For 200G FlexE PHY, valid values
                                       are 0/4; For 400G FlexE PHY, valid value
                                       is 0 */
    int mmu_lossless;               /* lossless port */
} bcm_compat6525_port_resource_t;

/*
 * Perform a FlexPort operation changing the port resources for a given
 * logical port.
 */
extern int bcm_compat6525_port_resource_set(
    int unit,
    bcm_gport_t port,
    bcm_compat6525_port_resource_t *resource);

/* Get the port resource configuration. */
extern int bcm_compat6525_port_resource_get(
    int unit,
    bcm_gport_t port,
    bcm_compat6525_port_resource_t *resource);

/*
 * Get suggested default resource values prior to calling
 * bcm_port_resource_set.
 */
extern int bcm_compat6525_port_resource_default_get(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    bcm_compat6525_port_resource_t *resource);

/*
 * Performs a FlexPort operation changing the port resources for a set of
 * ports.
 */
extern int bcm_compat6525_port_resource_multi_set(
    int unit,
    int nport,
    bcm_compat6525_port_resource_t *resource);

/*
 * Validate a port's speed configuration on the BCM56980 by fetching the
 * affected ports.
 */
extern int bcm_compat6525_port_resource_speed_config_validate(
    int unit,
    bcm_compat6525_port_resource_t *resource,
    bcm_pbmp_t *pbmp);

/* Get/Modify the speed for a given port. */
extern int bcm_compat6525_port_resource_speed_get(
    int unit,
    bcm_gport_t port,
    bcm_compat6525_port_resource_t *resource);

/* Get/Modify the speed for a given port. */
extern int bcm_compat6525_port_resource_speed_set(
    int unit,
    bcm_gport_t port,
    bcm_compat6525_port_resource_t *resource);

/* Modify the Port Speeds for a set of ports in a single Port Macro. */
extern int bcm_compat6525_port_resource_speed_multi_set(
    int unit,
    int nport,
    bcm_compat6525_port_resource_t *resource);

/* PP header information for mirrored packet in DNX devices */
typedef struct bcm_compat6525_mirror_pkt_dnx_pp_header_s {
    uint8 tail_edit_profile;    /* Tail edit profile, used at ETPP for tail editing */
    uint8 bytes_to_remove;    /* Bytes to remove from the start of the packet */
    uint8 eth_header_remove;    /* Indication to remove Ethernet header from mirror copy */
    uint8 force_mirror;    /* Indication to force mirroring processing packet at egress */
    bcm_gport_t out_vport_ext[3];    /* Extended outlif(s) */
    uint32 vsi;    /* Virtual switch instance */
} bcm_compat6525_mirror_pkt_dnx_pp_header_t;

/* Mirror packet system header information for DNX devices */
typedef struct bcm_compat6525_mirror_header_info_s {
    bcm_mirror_pkt_dnx_ftmh_header_t tm;    /* TM header */
    bcm_compat6525_mirror_pkt_dnx_pp_header_t pp;    /* PP header */
    bcm_compat6526_pkt_dnx_udh_t udh[4];    /* UDH header */
} bcm_compat6525_mirror_header_info_t;

/* Set system header information */
extern int bcm_compat6525_mirror_header_info_set(
    int unit,
    uint32 flags,
    bcm_gport_t mirror_dest_id,
    bcm_compat6525_mirror_header_info_t *mirror_header_info);

/* Get system header information */
extern int bcm_compat6525_mirror_header_info_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    uint32 *flags,
    bcm_compat6525_mirror_header_info_t *mirror_header_info);

/* Sample profile structure. */
typedef struct bcm_compat6525_mirror_sample_profile_s {
    uint8 enable;    /* Sample enable. */
    uint32 rate;    /* Sample rate. */
    uint8 pool_count_enable;    /* Enable to count the packets seen by the sampler. */
    uint8 sample_count_enable;    /* Enable to count the packets sampled by the sampler. */
    uint8 trace_enable;    /* Enable to generate trace event. */
    bcm_mirror_sample_mirror_mode_t mirror_mode;    /* Sample mirror mode. */
    uint8 copy_to_cpu;    /* Copy sampled packets to CPU. */
    uint8 mirror;    /* Copy sampled packets to all mirror destinations created with BCM_MIRROR_PORT_SFLOW flag. */
} bcm_compat6525_mirror_sample_profile_t;

/* Set sample profile entry with given profile id and type. */
extern int bcm_compat6525_mirror_sample_profile_set(
    int unit,
    bcm_mirror_sample_type_t type,
    int profile_id,
    bcm_compat6525_mirror_sample_profile_t *profile);

/* Get sample profile entry with given profile id and type. */
extern int bcm_compat6525_mirror_sample_profile_get(
    int unit,
    bcm_mirror_sample_type_t type,
    int profile_id,
    bcm_compat6525_mirror_sample_profile_t *profile);

/* Field destination match. */
typedef struct bcm_compat6525_field_destination_match_s {
    int priority;    /* Match priority. */
    bcm_gport_t gport;    /* Outgoing mod port or trunk. */
    uint32 gport_mask;    /* Outgoing mod port or trunk mask. */
    bcm_multicast_t mc_group;    /* Multicast group. */
    uint32 mc_group_mask;    /* Multicast group mask. */
    bcm_port_encap_t port_encap;    /* Incoming port encapsulation. */
    uint8 port_encap_mask;    /* Incoming port encapsulation mask. */
    bcm_color_t color;    /* Color. */
    uint8 color_mask;    /* Color mask. */
    bcm_color_t elephant_color;    /* Elephant color. */
    uint8 elephant_color_mask;    /* Elephant color mask. */
    uint8 elephant;    /* Flowtracker elephant flow indicator. */
    uint8 elephant_mask;    /* Elephant mask. */
    uint8 elephant_notify;    /* Indicator that flow exceeds the elephant threshold byte count and becomes an elephant flow. */
    uint8 elephant_notify_mask;    /* elephant_notify mask. */
    int int_pri;    /* Internal priority. */
    uint32 int_pri_mask;    /* Internal priority mask. */
    int opaque_object_2;    /* Opaque object 2 assigned in ingress field. */
    uint32 opaque_object_2_mask;    /* Opaque object 2 mask. */
    uint32 ifp_opaque_ctrl_id;    /* IFP opaque control ID. */
    uint32 ifp_opaque_ctrl_id_mask;    /* IFP opaque control ID mask. */
    bcm_field_drop_reason_t drop_reason;    /* Highest drop reason. */
    uint32 drop_reason_mask;    /* Highest drop reason mask. */
} bcm_compat6525_field_destination_match_t;

/* Field destination action. */
typedef struct bcm_compat6525_field_destination_action_s {
    uint32 flags;    /* BCM_FIELD_DESTINATION_ACTION_xxx flags. */
    uint8 drop;    /* Drop the packet. */
    uint8 copy_to_cpu;    /* Copy the packet to the CPU. */
    bcm_color_t color;    /* Internal color. */
    bcm_field_redirect_destination_type_t destination_type;    /*  Destination type. */
    bcm_gport_t gport;    /* Redirection logical port. */
    bcm_multicast_t mc_group;    /* Multicast group. */
    bcm_gport_t mirror_dest_id;    /* Mirror destination ID. */
    bcm_cos_queue_t uc_cos;    /* Unicast cos queue. */
    bcm_cos_queue_t mc_cos;    /* Multicast cos queue. */
    bcm_cos_queue_t rqe_cos;    /* RQE cos queue. */
    bcm_cos_queue_t cpu_cos;    /* CPU cos queue. */
} bcm_compat6525_field_destination_action_t;

/* Add a field destination entry. */
extern int bcm_compat6525_field_destination_entry_add(
    int unit,
    uint32 options,
    bcm_compat6525_field_destination_match_t *match,
    bcm_compat6525_field_destination_action_t *action);

/* Get a field destination entry. */
extern int bcm_compat6525_field_destination_entry_get(
    int unit,
    bcm_compat6525_field_destination_match_t *match,
    bcm_compat6525_field_destination_action_t *action);

/* Delete a field destination entry. */
extern int bcm_compat6525_field_destination_entry_delete(
    int unit,
    bcm_compat6525_field_destination_match_t *match);

/* Attach the statistic counter ID to a field destination entry. */
extern int bcm_compat6525_field_destination_stat_attach(
    int unit,
    bcm_compat6525_field_destination_match_t *match,
    uint32 stat_counter_id);

/* Get the statistic counter ID attached to a field destination entry. */
extern int bcm_compat6525_field_destination_stat_id_get(
    int unit,
    bcm_compat6525_field_destination_match_t *match,
    uint32 *stat_counter_id);

/* Detach the statistic counter ID attached to a field destination entry. */
extern int bcm_compat6525_field_destination_stat_detach(
    int unit,
    bcm_compat6525_field_destination_match_t *match);

/* Set the flex counter object value for a field destination entry. */
extern int bcm_compat6525_field_destination_flexctr_object_set(
    int unit,
    bcm_compat6525_field_destination_match_t *match,
    uint32 value);

/* Get the flex counter object value of a field destination entry. */
extern int bcm_compat6525_field_destination_flexctr_object_get(
    int unit,
    bcm_compat6525_field_destination_match_t *match,
    uint32 *value);

typedef struct bcm_compat6525_policer_config_s {
    uint32 flags;                       /* BCM_POLICER_* Flags. */
    bcm_policer_mode_t mode;            /* Policer mode. */
    uint32 ckbits_sec;                  /* Committed rate (kbits per sec). */
    uint32 cbits_sec_lower;             /* Committed rate (lower digits; 0~999
                                           bits per sec). */
    uint32 max_ckbits_sec;              /* Maximum Committed rate (kbits per
                                           sec), Applicable only for cascade
                                           mode. */
    uint32 max_cbits_sec_lower;         /* Maximum Committed rate (lower digits;
                                           0~999 bits per sec). */
    uint32 ckbits_burst;                /* Committed burst size (kbits). */
    uint32 cbits_burst_lower;           /* Committed burst size (lower digits;
                                           0~999 bits). */
    uint32 pkbits_sec;                  /* Peak rate (kbits per sec). */
    uint32 pbits_sec_lower;             /* Peak rate (lower digits; 0~999 bits
                                           per sec). */
    uint32 max_pkbits_sec;              /* Maximum Peak rate (kbits per sec),
                                           Applicable only for cascade mode. */
    uint32 max_pbits_sec_lower;         /* Maximum Peak rate (lower digits;
                                           0~999 bits per sec). */
    uint32 pkbits_burst;                /* Peak burst size (kbits). */
    uint32 pbits_burst_lower;           /* Peak burst size (lower digits; 0~999
                                           bits). */
    uint32 kbits_current;               /* Current size (kbits). */
    uint32 bits_current_lower;          /* Current size (lower digits; 0~999
                                           bits). */
    uint32 action_id;                   /* Index of the global meter action
                                           entry */
    uint32 sharing_mode;                /* Meter sharing mode in case of Macro
                                           meter of Envelop */
    uint32 entropy_id;                  /* If set, indicate the configuration
                                           group the policer belongs to */
    bcm_policer_pool_t pool_id;         /* Meter Pool Id of the current policer */
    uint32 bucket_width;                /* Bucket width in bits of the current
                                           policer */
    uint32 token_gran;                  /* Token granularity of the current
                                           policer */
    uint32 bucket_init_val;             /* Initial bucket count of the current
                                           policer */
    uint32 bucket_rollover_val;         /* Rollover bucket count of the current
                                           policer */
    bcm_core_t core_id;                 /* Device core on which to allocate the
                                           policer */
    bcm_policer_t ncoflow_policer_id;   /* Next lower priority committed policer
                                           ID to overflow to. Applicable in
                                           Cascaded  policer modes on MEF 10.3+
                                           compliant devices */
    bcm_policer_t npoflow_policer_id;   /* Next lower priority peak policer ID
                                           to overflow to. Applicable in
                                           Cascaded and CoupledCascade policer
                                           modes on MEF 10.3+  compliant devices */
    uint32 actual_ckbits_sec;           /* Granularity adjusted Committed rate
                                           (kbits per sec). */
    uint32 actual_cbits_sec_lower;      /* Granularity adjusted Committed rate
                                           (lower digits; 0~999 bits per sec). */
    uint32 actual_ckbits_burst;         /* Granularity adjusted Committed burst
                                           size (kbits). */
    uint32 actual_cbits_burst_lower;    /* Granularity adjusted Committed burst
                                           size (lower digits; 0~999 bits). */
    uint32 actual_pkbits_sec;           /* Granularity adjusted Peak rate (kbits
                                           per sec). */
    uint32 actual_pbits_sec_lower;      /* Granularity adjusted Peak rate (lower
                                           digits; 0~999 bits per sec). */
    uint32 actual_pkbits_burst;         /* Granularity adjusted Peak burst size
                                           (kbits). */
    uint32 actual_pbits_burst_lower;    /* Granularity adjusted Peak burst size
                                           (lower digits; 0~999 bits). */
    uint32 average_pkt_size;            /* Estimated average Ethernet packet
                                           size (1 to 16383 Bytes) for more
                                           accurate metering. */
    int mark_drop_as_black;             /* For drop decision (no committed or
                                           Excess credits), mark it as black
                                           (otherwise, it will be marked as
                                           red). */
    int color_resolve_profile;          /* Input data for the resolved table. */
} bcm_compat6525_policer_config_t;

/* Create a policer entry. */
extern int bcm_compat6525_policer_create(
    int unit,
    bcm_compat6525_policer_config_t *pol_cfg,
    bcm_policer_t *policer_id);

/* Set the config settings for a policer entry. */
extern int bcm_compat6525_policer_set(
    int unit,
    bcm_policer_t policer_id,
    bcm_compat6525_policer_config_t *pol_cfg);

/* Get the config settings for a policer entry. */
extern int bcm_compat6525_policer_get(
    int unit,
    bcm_policer_t policer_id,
    bcm_compat6525_policer_config_t *pol_cfg);

/*  */
typedef struct bcm_compat6525_cosq_bst_profile_s {
    uint32 byte;    /*  */
} bcm_compat6525_cosq_bst_profile_t;

/* Set/Get the BST profile for CosQ objects */
extern int bcm_compat6525_cosq_bst_profile_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_bst_stat_id_t bid,
    bcm_compat6525_cosq_bst_profile_t *profile);

/* Set/Get the BST profile for CosQ objects */
extern int bcm_compat6525_cosq_bst_profile_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_bst_stat_id_t bid,
    bcm_compat6525_cosq_bst_profile_t *profile);

/* Set/Get the BST profile for CosQ objects */
extern int bcm_compat6525_cosq_bst_multi_profile_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_bst_stat_id_t bid,
    int array_size,
    bcm_compat6525_cosq_bst_profile_t *profile_array);

/* Set/Get the BST profile for CosQ objects */
extern int bcm_compat6525_cosq_bst_multi_profile_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_bst_stat_id_t bid,
    int array_size,
    bcm_compat6525_cosq_bst_profile_t *profile_array,
    int *count);


/* Describes multicast encapsulation info. */
typedef struct bcm_compat6525_multicast_encap_s {
    bcm_multicast_encap_type_t encap_type;    /* Multicast Encapsulation type. */
    bcm_if_t ul_egress_if;    /* Underlay egress object ID. It is valid for L3Tunnel/L2Tunnel encap type. */
    bcm_if_t l3_intf;    /* Overlay L3 interface ID for L3Tunnel/L2Tunnel encap type or L3 interface ID for L3 encap type. */
    bcm_gport_t port_id;    /* Gport ID for L2Tunnel encap type. */
} bcm_compat6525_multicast_encap_t;

/* Create a multicast encapsualtion id based on multicast encapsulation info. */
extern int bcm_compat6525_multicast_encap_create(
    int unit,
    bcm_compat6525_multicast_encap_t *mc_encap,
    bcm_if_t *encap_id);

/* Get the multicast encapsulation info from the given encap_id. */
extern int bcm_compat6525_multicast_encap_get(
    int unit,
    bcm_if_t encap_id,
    bcm_compat6525_multicast_encap_t *mc_encap);

/* EP Recirculate profile structure. */
typedef struct bcm_compat6525_switch_ep_recirc_profile_s {
    int recirc_profile_id; /* Profile id. */
    uint32 flags; /* See BCM_XXX_EP_RECIRC_PROFILE_XXX flag definitions. */
    bcm_switch_ep_recirc_pkt_copy_type_t redirect_type; /* Specify types of packets to be redirected. */
    bcm_switch_ep_recirc_pkt_copy_type_t tocpu_type; /* Specify types of packets to be copied to CPU. */
    int truncate_cell_length; /* Truncated size (in cells) for the recirc packet. 0 means no truncation. */
    int buffer_priority; /* Enqueue priority specified for the redirect data buffer (RDB). */
    bcm_switch_ep_recirc_nih_config_t nih_cfg; /* Config for NIH header construction for redirected packets. */
    bcm_switch_ep_recirc_cpu_dma_config_t cpu_dma_cfg; /* Config for CPU DMA header construction for copy-to-CPU packets. */
} bcm_compat6525_switch_ep_recirc_profile_t;

/* Create an EP Recirculate profile. */
extern int bcm_compat6525_switch_ep_recirc_profile_create(
    int unit,
    bcm_compat6525_switch_ep_recirc_profile_t *recirc_profile,
    int *recirc_profile_id);

/* Get an EP Recirculate profile. */
extern int bcm_compat6525_switch_ep_recirc_profile_get(
    int unit,
    int recirc_profile_id,
    bcm_compat6525_switch_ep_recirc_profile_t *recirc_profile);

/* VLAN Egress Translate Key Configuration. */
typedef struct bcm_compat6525_vlan_translate_egress_key_config_s {
    uint32 flags;    /*  */
    bcm_vlan_translate_key_t key_type;    /* Lookup Key Type. */
    bcm_vlan_t inner_vlan;    /* Inner VLAN ID to match. */
    bcm_vlan_t outer_vlan;    /* Outer VLAN ID to match. */
    bcm_gport_t port;    /* Port to match on. */
    int otag_preserve;    /* Outer tag preserve to match on. */
    int itag_preserve;    /* Inner tag preserve to match on. */
    bcm_gport_t ingress_port;    /* Ingress port to match. */
} bcm_compat6525_vlan_translate_egress_key_config_t;

/* Add an entry to the egress VLAN Translation table and assign VLAN actions. It extends bcm_vlan_egress_action_add API for additional keys. */
extern int bcm_compat6525_vlan_translate_egress_action_extended_add(
    int unit,
    bcm_compat6525_vlan_translate_egress_key_config_t *key_config,
    bcm_vlan_translate_egress_action_set_t *action);

/* Delete an egress translation entry including key types added by bcm_vlan_translate_egress_extended_action_add API. */
extern int bcm_compat6525_vlan_translate_egress_action_extended_delete(
    int unit,
    bcm_compat6525_vlan_translate_egress_key_config_t *key_config);

/* Retrive an egress translation entry including key types added by bcm_vlan_translate_egress_extended_action_add API. */
extern int bcm_compat6525_vlan_translate_egress_action_extended_get(
    int unit,
    bcm_compat6525_vlan_translate_egress_key_config_t *key_config,
    bcm_vlan_translate_egress_action_set_t *action);

/* Attach a flex counter into a given egress VLAN translation table with additional keys. */
extern int bcm_compat6525_vlan_translate_egress_flexctr_extended_attach(
    int unit,
    bcm_compat6525_vlan_translate_egress_key_config_t *key_config,
    bcm_vlan_flexctr_config_t *config);

/* Detach a flex counter from a given egress VLAN translation table with additional keys. */
extern int bcm_compat6525_vlan_translate_egress_flexctr_extended_detach(
    int unit,
    bcm_compat6525_vlan_translate_egress_key_config_t *key_config,
    bcm_vlan_flexctr_config_t *config);

/* Detach all flex counters from a given egress VLAN translation table with additional keys. */
extern int bcm_compat6525_vlan_translate_egress_flexctr_extended_detach_all(
    int unit,
    bcm_compat6525_vlan_translate_egress_key_config_t *key_config);

/* Get the attached flex counters from a given egress VLAN translation table with additional keys. */
extern int bcm_compat6525_vlan_translate_egress_flexctr_extended_get(
    int unit,
    bcm_compat6525_vlan_translate_egress_key_config_t *key_config,
    int array_size,
    bcm_vlan_flexctr_config_t *config_array,
    int *count);

/* packet trace ingress process data */
typedef struct bcm_compat6525_switch_pkt_trace_info_s {
    bcm_switch_pkt_trace_lookup_result_t pkt_trace_lookup_status; /* packet trace lookup status set */
    bcm_switch_pkt_trace_resolution_t pkt_trace_resolution; /* packet trace resolution result */
    bcm_switch_pkt_trace_hashing_info_t pkt_trace_hash_info; /* packet trace hashing information */
    bcm_stg_stp_t pkt_trace_stp_state;  /* packet trace ingress stp state */
    uint32 pkt_trace_drop_reason[BCM_SWITCH_PKT_TRACE_DROP_REASON_MAX]; /* packet trace drop reason */
    uint32 dest_pipe_num;               /* ingress pipeline number of the pkt
                                           trace packet */
    uint32 raw_data_length;             /* number of bytes returned as raw_data */
    uint8 raw_data[BCM_SWITCH_PKT_TRACE_RAW_DATA_MAX]; /* packet trace process result data in
                                           raw format */
    bcm_switch_pkt_trace_fwd_dst_info_t pkt_trace_fwd_dst_info; /* packet trace forward destination
                                           information */
    bcm_switch_pkt_trace_field_info_t pkt_trace_field_info[BCM_SWITCH_PKT_TRACE_FIELD_STAGE_MAX]; /* packet trace field processor
                                           information */
} bcm_compat6525_switch_pkt_trace_info_t;

/*
 * Generate a visibility trace packet and/or read the result of the
 * packets ingress processing information
 */
extern int bcm_compat6525_switch_pkt_trace_info_get(
    int unit,
    uint32 options,
    uint8 port,
    int len,
    uint8 *data,
    bcm_compat6525_switch_pkt_trace_info_t *pkt_trace_info);

/* Flowtracker flow group information. */
typedef struct bcm_compat6525_flowtracker_group_info_s {
    bcm_flowtracker_ipfix_observation_domain_t observation_domain_id; /* IPFIX observation domain to which
                                           this flow group belongs to. */
    int group_class;                    /* Flowtracker flow group Class id. */
    uint32 group_flags;                 /* Flowtracker flow group flags. See
                                           BCM_FLOWTRACKER_GROUP_XXX. */
    bcm_flowtracker_group_type_t group_type; /* Flowtracker flow group type. See
                                           bcm_flowtracker_group_type_t. */
    bcm_field_group_t field_group[BCM_PIPES_MAX]; /* Field group corresponding to the flow
                                           group. */
} bcm_compat6525_flowtracker_group_info_t;

/* Create a flowtracker flow group. */
extern int bcm_compat6525_flowtracker_group_create(
    int unit,
    uint32 options,
    bcm_flowtracker_group_t *flow_group_id,
    bcm_compat6525_flowtracker_group_info_t *flow_group_info);

/* Get a flowtracker flow group. */
extern int bcm_compat6525_flowtracker_group_get(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_compat6525_flowtracker_group_info_t *flow_group_info);
/* Packet format based UDF specification structure. */
typedef struct bcm_compat6525_udf_pkt_format_info_s {
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
} bcm_compat6525_udf_pkt_format_info_t;
/* Create a packet format entry */
extern int bcm_compat6525_udf_pkt_format_create(
    int unit, 
    bcm_udf_pkt_format_options_t options, 
    bcm_compat6525_udf_pkt_format_info_t *pkt_format, 
    bcm_udf_pkt_format_id_t *pkt_format_id);

/* Retrieve packet format info given the packet format Id */
extern int bcm_compat6525_udf_pkt_format_info_get(
    int unit, 
    bcm_udf_pkt_format_id_t pkt_format_id, 
    bcm_compat6525_udf_pkt_format_info_t *pkt_format);

typedef struct bcm_compat6525_switch_pkt_info_s {
    uint32 flags;                       /* BCM_SWITCH_PKT_INFO_xxx flags for
                                           packet parameter hashing. */
    bcm_gport_t src_gport;              /* Source module and port,
                                           BCM_GPORT_INVALID to wildcard. */
    bcm_vlan_t vid;                     /* VLAN ID. */
    bcm_ethertype_t ethertype;          /* EtherType */
    bcm_mac_t src_mac;                  /* Source MAC address. */
    bcm_mac_t dst_mac;                  /* Destination MAC address. */
    bcm_ip_t sip;                       /* Source host IP address (IPv4). */
    bcm_ip_t dip;                       /* Destination host IP address (IPv4). */
    bcm_ip6_t sip6;                     /* Source host IP address (IPv6). */
    bcm_ip6_t dip6;                     /* Destination host IP address (IPv6). */
    uint8 protocol;                     /* IP protocol */
    uint32 src_l4_port;                 /* Source L4 port */
    uint32 dst_l4_port;                 /* Destination L4 port. */
    bcm_gport_t trunk_gport;            /* Gport of trunk type on which packet
                                           will egress. */
    bcm_if_t mpintf;                    /* Multipath egress forwarding object. */
    bcm_switch_pkt_hash_info_fwd_reason_t fwd_reason; /* Packet forward reason. */
    bcm_mpls_label_t top_label;         /* First label in MPLS stack. */
    bcm_mpls_label_t second_label;      /* Second label in MPLS stack. */
    bcm_mpls_label_t third_label;       /* Third label in MPLS stack. */
} bcm_compat6525_switch_pkt_info_t;

extern int bcm_compat6525_switch_pkt_info_hash_get(
    int unit,
    bcm_compat6525_switch_pkt_info_t *pkt_info,
    bcm_gport_t *dst_gport,
    bcm_if_t *dst_intf);

/* IFA header data. */
typedef struct bcm_compat6525_ifa_header_s {
    uint8 ip_protocol;  /* IP protocol to indicate IFA. */
    uint8 max_length;   /* The max length in the IFA base header. */
    uint8 hop_limit;    /* The hop limit in the IFA metadata header. */
} bcm_compat6525_ifa_header_t;

/* Create the IFA header. */
extern int bcm_compat6525_ifa_header_create(
    int unit,
    uint32 options,
    int *header_id,
    bcm_compat6525_ifa_header_t *header);

/* Get the IFA header. */
extern int bcm_compat6525_ifa_header_get(
    int unit,
    int header_id,
    bcm_compat6525_ifa_header_t *header);

#define BCM_TSN_TAF_CALENDAR_TABLE_SIZE_6525 (16)

/* The TAF profile data structure */
typedef struct bcm_compat6525_tsn_taf_profile_s {
    int num_entries;    /* Specify the number of entries in the calendar table. */
    bcm_tsn_taf_entry_t entries[BCM_TSN_TAF_CALENDAR_TABLE_SIZE_6525];    /* Specify the interval, gate states, attribute entry sets in the calendar table. */
    bcm_ptp_timestamp_t ptp_base_time;    /* Specify base time in PTP time. */
    uint32 ptp_cycle_time;    /* Specify cycle time in ns. */
    uint32 ptp_max_cycle_extension;    /* Specify max cycle extension time in ns. Maximum time allowed for a cycle to be extended before profile change. */
} bcm_compat6525_tsn_taf_profile_t;

/* Create the TAF profile */
extern int bcm_compat6525_tsn_taf_profile_create(
    int unit,
    int taf_gate,
    bcm_compat6525_tsn_taf_profile_t *profile,
    bcm_tsn_taf_profile_id_t *pid);

/* Get/set TAF profile information */
extern int bcm_compat6525_tsn_taf_profile_set(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_compat6525_tsn_taf_profile_t *profile);

/* Get/set TAF profile information */
extern int bcm_compat6525_tsn_taf_profile_get(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_compat6525_tsn_taf_profile_t *profile);

/* The TAS profile data structure */
typedef struct bcm_compat6525_cosq_tas_profile_s {
    int num_entries;    /* Specify the number of entries in the calendar table. */
    bcm_cosq_tas_entry_t entries[BCM_COSQ_MAX_TAS_CALENDAR_TABLE_SIZE];    /* Specify the interval, gate states, attribute entry sets in the calendar table. */
    bcm_ptp_timestamp_t ptp_base_time;    /* Specify base time in PTP time. */
    uint32 ptp_cycle_time;    /* Specify cycle time in ns. */
    uint32 ptp_max_cycle_extension;    /* Specify max cycle extension time in ns. Maximum time allowed for a cycle to be extended before profile change. */
} bcm_compat6525_cosq_tas_profile_t;

/* Create the TAS profile */
extern int bcm_compat6525_cosq_tas_profile_create(
    int unit,
    bcm_gport_t port,
    bcm_compat6525_cosq_tas_profile_t *profile,
    bcm_cosq_tas_profile_id_t *pid);

/* Get/set TAS profile information */
extern int bcm_compat6525_cosq_tas_profile_set(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_id_t pid,
    bcm_compat6525_cosq_tas_profile_t *profile);

/* Get/set TAS profile information */
extern int bcm_compat6525_cosq_tas_profile_get(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_id_t pid,
    bcm_compat6525_cosq_tas_profile_t *profile);
#endif /* BCM_RPC_SUPPORT */
#endif /* _COMPAT_6525_H_ */
