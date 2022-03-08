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
#include <bcm/field.h>
#include <bcm/mirror.h>
#include <bcm/cosq.h>
#include <bcm/vlan.h>
#include <bcm/mpls.h>
#include <bcm/l2.h>
#include <bcm/types.h>
#include <bcm/subport.h>





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
} bcm_compat6525_vlan_port_t;

/* Create a layer 2 logical port. */
extern int bcm_compat6525_vlan_port_create(
    int unit,
    bcm_compat6525_vlan_port_t *vlan_port);

/* Get/find a layer 2 logical port given the GPORT ID or match criteria. */
extern int bcm_compat6525_vlan_port_find(
    int unit,
    bcm_compat6525_vlan_port_t *vlan_port);

extern int bcm_compat6525_field_qualify_RangeCheckGroup(
    int unit,
    bcm_field_entry_t entry,
    uint8 data,
    uint8 mask);

extern int bcm_compat6525_field_qualify_RangeCheckGroup_get(
    int unit,
    bcm_field_entry_t entry,
    uint8 *data,
    uint8 *mask);

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
    bcm_pkt_dnx_udh_t udh[4];    /* UDH header */
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


#endif /* BCM_RPC_SUPPORT */
#endif /* _COMPAT_6525_H_ */
