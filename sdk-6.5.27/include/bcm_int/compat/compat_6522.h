/*
* $Id: compat_6522.h,v 1.0 2021/04/13
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2022 Broadcom Inc. All rights reserved.
*
* RPC Compatibility with sdk-6.5.22 routines
*/

#ifndef _COMPAT_6522_H_
#define _COMPAT_6522_H_

#ifdef BCM_RPC_SUPPORT
#include <bcm_int/compat/compat_6526.h>
#include <bcm_int/compat/compat_6525.h>
#include <bcm_int/compat/compat_6524.h>
#include <bcm_int/compat/compat_6523.h>
#include <bcm/vlan.h>
#include <bcm/collector.h>
#include <bcm/types.h>
#include <bcm/mirror.h>
#include <bcm/switch.h>
#include <bcm/udf.h>
#include <bcm/sat.h>
#if defined(INCLUDE_L3)
#include <bcm/l3.h>
#include <bcm/extender.h>
#include <bcm/flow.h>
#include <bcm/mpls.h>
#endif /* INCLUDE_L3 */

/* PP header information for mirrored packet in DNX devices */
typedef struct bcm_compat6522_mirror_pkt_dnx_pp_header_s {
    uint8 tail_edit_profile;    /* Tail edit profile, used at ETPP for tail editing */
    uint8 eth_header_remove;    /* Indication to remove Ethernet header from mirror copy */
    bcm_gport_t out_vport_ext[3];    /* Extended outlif(s) */
    uint32 vsi;    /* Virtual switch instance */
} bcm_compat6522_mirror_pkt_dnx_pp_header_t;

/* Mirror packet system header information for DNX devices */
typedef struct bcm_compat6522_mirror_header_info_s {
    bcm_mirror_pkt_dnx_ftmh_header_t tm;    /* TM header */
    bcm_compat6522_mirror_pkt_dnx_pp_header_t pp;    /* PP header */
    bcm_compat6526_pkt_dnx_udh_t udh[4];    /* UDH header */
} bcm_compat6522_mirror_header_info_t;

/* Set system header information */
extern int bcm_compat6522_mirror_header_info_set(
    int unit,
    uint32 flags,
    bcm_gport_t mirror_dest_id,
    bcm_compat6522_mirror_header_info_t *mirror_header_info);

/* Get system header information */
extern int bcm_compat6522_mirror_header_info_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    uint32 *flags,
    bcm_compat6522_mirror_header_info_t *mirror_header_info);

/* Collector information. */
typedef struct bcm_compat6522_collector_info_s {
    bcm_collector_type_t collector_type;    /* Collector type. Remote vs Local */
    bcm_collector_transport_type_t transport_type;    /* Transport type used for exporting data to the collector. This identifies the usable fields within the encap structure member defined below. */
    bcm_collector_eth_header_t eth;    /* Ethernet encapsulation of the packet sent to collector. */
    bcm_collector_ipv4_header_t ipv4;    /* IPv4 encapsulation of the packet sent to collector. */
    bcm_collector_ipv6_header_t ipv6;    /* IPv6 encapsulation of the packet sent to collector. */
    bcm_collector_udp_header_t udp;    /* UDP encapsulation of the packet sent to collector. */
    bcm_collector_ipfix_header_t ipfix;    /* IPFIX encapsulation of the packet sent to collector. */
    bcm_collector_protobuf_header_t protobuf;    /* Protobuf header information. */
    bcm_collector_timestamp_source_t src_ts;    /* Timestamp source info. */
    int max_records_reserve;    /* Maximum number of export record entries reserved in the hardware FIFO for the collector. */
    bcm_gport_t mirror_dest_id;    /* Mirror destination id. */
} bcm_compat6522_collector_info_t;

/* Create a collector. */
extern int bcm_compat6522_collector_create(
    int unit,
    uint32 options,
    bcm_collector_t *collector_id,
    bcm_compat6522_collector_info_t *collector_info);

/* Gets a Collector information. */
extern int bcm_compat6522_collector_get(
    int unit,
    bcm_collector_t id,
    bcm_compat6522_collector_info_t *collector_info);

/*
 * Packet control action structure, common for integrity check API and
 * protocol control API.
 */
typedef struct bcm_compat6522_switch_pkt_control_action_s {
    uint8 discard;                  /* Packet is discarded. */
    uint8 copy_to_cpu;              /* Packet is copied to CPU. */
    uint8 flood;                    /* Packet is flooded to the forwarding
                                       domain. */
    uint8 bpdu;                     /* Packet is BPDU. */
    uint8 opaque_ctrl_id;           /* Opaque control ID. */
    uint32 class_id;                /* Class ID. */
} bcm_compat6522_switch_pkt_control_action_t;

/* Packet protocol match structure. */
typedef struct bcm_compat6522_switch_pkt_protocol_match_s {
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
} bcm_compat6522_switch_pkt_protocol_match_t;

/* An API to add a protocol control entry. */
extern int bcm_compat6522_switch_pkt_protocol_control_add(
    int unit,
    uint32 options,
    bcm_compat6522_switch_pkt_protocol_match_t *match,
    bcm_compat6522_switch_pkt_control_action_t *action,
    int priority);

/* An API to get action and priority for a protocol control entry. */
extern int bcm_compat6522_switch_pkt_protocol_control_get(
    int unit,
    bcm_compat6522_switch_pkt_protocol_match_t *match,
    bcm_compat6522_switch_pkt_control_action_t *action,
    int *priority);

/* An API to delete a protocol control entry. */
extern int bcm_compat6522_switch_pkt_protocol_control_delete(
    int unit,
    bcm_compat6522_switch_pkt_protocol_match_t *match);

/* Packet format based UDF specification structure. */
typedef struct bcm_compat6522_udf_pkt_format_info_s {
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
} bcm_compat6522_udf_pkt_format_info_t;

/* Create a packet format entry */
extern int bcm_compat6522_udf_pkt_format_create(
    int unit,
    bcm_udf_pkt_format_options_t options,
    bcm_compat6522_udf_pkt_format_info_t *pkt_format,
    bcm_udf_pkt_format_id_t *pkt_format_id);

/* Retrieve packet format info given the packet format Id */
extern int bcm_compat6522_udf_pkt_format_info_get(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_compat6522_udf_pkt_format_info_t *pkt_format);

/* Sat ctf stat object for configuration */
typedef struct bcm_compat6522_sat_ctf_stat_config_s {
    uint32 use_global_bin_config;    /* Use global bin config */
    uint32 bin_min_delay;    /* Bin min delay, for linear bins */
    uint32 bin_step;    /* Bin step, for linear bins */
    uint32 update_counters_in_unvavail_state;    /* Update counters in unavail state */
} bcm_compat6522_sat_ctf_stat_config_t;

/* Configure ctf stat. */
extern int bcm_compat6522_sat_ctf_stat_config_set(
    int unit,
    bcm_sat_ctf_t ctf_id,
    bcm_compat6522_sat_ctf_stat_config_t *stat);

#if defined(INCLUDE_L3)
/* L3 ECMP structure */
typedef struct bcm_compat6522_l3_egress_ecmp_s {
    uint32 flags;    /* See BCM_L3_xxx flag definitions. */
    bcm_if_t ecmp_intf;    /* L3 interface ID pointing to egress ecmp object. */
    int max_paths;    /* Max number of paths in ECMP group. If max_paths <= 0, the default max path which can be set by the API bcm_l3_route_max_ecmp_set will be picked. */
    uint32 ecmp_group_flags;    /* BCM_L3_ECMP_xxx flags. */
    uint32 dynamic_mode;    /* Dynamic load balancing mode. See BCM_L3_ECMP_DYNAMIC_MODE_xxx definitions. */
    uint32 dynamic_size;    /* Number of flows for dynamic load balancing. Valid values are 512, 1k, doubling up to 32k */
    uint32 dynamic_age;    /* Inactivity duration, in microseconds. */
    uint32 dynamic_load_exponent;    /* The exponent used in the exponentially weighted moving average calculation of historical member load. */
    uint32 dynamic_expected_load_exponent;    /* The exponent used in the exponentially weighted moving average calculation of historical expected member load. */
    bcm_l3_dgm_t dgm;    /* DGM properties */
    uint32 stat_id;    /* Object statistics ID. */
    int stat_pp_profile;    /* Statistics profile. */
    bcm_l3_ecmp_urpf_mode_t rpf_mode;    /* Define RPF mode for the ECMP group. */
    bcm_l3_tunnel_priority_info_t tunnel_priority;    /* ECMP tunnel priority. */
    uint32 rh_random_seed;    /* Random seed for the RH ECMP group. */
} bcm_compat6522_l3_egress_ecmp_t;

/* Resilient ecmp entry */
typedef struct bcm_compat6522_l3_egress_ecmp_resilient_entry_s {
    uint64 hash_key;    /* Hash key. */
    bcm_compat6522_l3_egress_ecmp_t ecmp;    /* ECMP. */
    bcm_if_t intf;    /* L3 interface. */
} bcm_compat6522_l3_egress_ecmp_resilient_entry_t;

/* Replace ECMP resilient entries matching given criteria. */
extern int bcm_compat6522_l3_egress_ecmp_resilient_replace(
    int unit,
    uint32 flags,
    bcm_compat6522_l3_egress_ecmp_resilient_entry_t *match_entry,
    int *num_entries,
    bcm_compat6522_l3_egress_ecmp_resilient_entry_t *replace_entry);

/* Add ECMP resilient entries matching given criteria. */
extern int bcm_compat6522_l3_egress_ecmp_resilient_add(
    int unit,
    uint32 flags,
    bcm_compat6522_l3_egress_ecmp_resilient_entry_t *entry);

/* Delete ECMP resilient entries matching given criteria. */
extern int bcm_compat6522_l3_egress_ecmp_resilient_delete(
    int unit,
    uint32 flags,
    bcm_compat6522_l3_egress_ecmp_resilient_entry_t *entry);

/* Create an Egress ECMP forwarding object. */
extern int bcm_compat6522_l3_ecmp_create(
    int unit,
    uint32 options,
    bcm_compat6522_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_count,
    bcm_l3_ecmp_member_t *ecmp_member_array);

/* Get info about an Egress ECMP forwarding object. */
extern int bcm_compat6522_l3_ecmp_get(
    int unit,
    bcm_compat6522_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_size,
    bcm_l3_ecmp_member_t *ecmp_member_array,
    int *ecmp_member_count);

/* Find an Egress ECMP forwarding object. */
extern int bcm_compat6522_l3_ecmp_find(
    int unit,
    int ecmp_member_count,
    bcm_l3_ecmp_member_t *ecmp_member_array,
    bcm_compat6522_l3_egress_ecmp_t *ecmp_info);

/* Create an Egress ECMP forwarding object. */
extern int bcm_compat6522_l3_egress_ecmp_create(
    int unit,
    bcm_compat6522_l3_egress_ecmp_t *ecmp,
    int intf_count,
    bcm_if_t *intf_array);

/* Destroy an Egress ECMP forwarding object. */
extern int bcm_compat6522_l3_egress_ecmp_destroy(
    int unit,
    bcm_compat6522_l3_egress_ecmp_t *ecmp);

/* Get info about an Egress ECMP forwarding object. */
extern int bcm_compat6522_l3_egress_ecmp_get(
    int unit,
    bcm_compat6522_l3_egress_ecmp_t *ecmp,
    int intf_size,
    bcm_if_t *intf_array,
    int *intf_count);

/* Add an Egress forwarding object to an Egress ECMP forwarding object. */
extern int bcm_compat6522_l3_egress_ecmp_add(
    int unit,
    bcm_compat6522_l3_egress_ecmp_t *ecmp,
    bcm_if_t intf);

/* Delete an Egress forwarding object from an Egress ECMP forwarding object. */
extern int bcm_compat6522_l3_egress_ecmp_delete(
    int unit,
    bcm_compat6522_l3_egress_ecmp_t *ecmp,
    bcm_if_t intf);

/* Find an Egress ECMP forwarding object. */
extern int bcm_compat6522_l3_egress_ecmp_find(
    int unit,
    int intf_count,
    bcm_if_t *intf_array,
    bcm_compat6522_l3_egress_ecmp_t *ecmp);

/* Update an ECMP table that was already allocated, used in cases where an ECMP have more than one table. */
extern int bcm_compat6522_l3_egress_ecmp_tunnel_priority_set(
    int unit,
    bcm_compat6522_l3_egress_ecmp_t *ecmp,
    int intf_count,
    bcm_if_t *intf_array);
#endif /* INCLUDE_L3 */

#if defined(INCLUDE_L3)
typedef struct bcm_compat6522_flow_match_config_s {
    uint32 vnid;                        /* VN_ID. */
    bcm_vlan_t vlan;                    /* Outer VLAN ID to match. */
    bcm_vlan_t inner_vlan;              /* Inner VLAN ID to match. */
    bcm_gport_t port;                   /* Match port / trunk */
    bcm_ip_t sip;                       /* source IPv4 address */
    bcm_ip_t sip_mask;                  /* source IPv4 address mask */
    bcm_ip6_t sip6;                     /* source IPv6 address */
    bcm_ip6_t sip6_mask;                /* source IPv6 address mask */
    bcm_ip_t dip;                       /* destination IPv4 address */
    bcm_ip_t dip_mask;                  /* destination IPv4 address mask */
    bcm_ip6_t dip6;                     /* destination IPv6 address */
    bcm_ip6_t dip6_mask;                /* destination IPv6 address mask */
    uint16 udp_dst_port;                /* udp destination port. */
    uint16 udp_src_port;                /* udp source port. */
    uint16 protocol;                    /* IP protocol type. */
    bcm_mpls_label_t mpls_label;        /* MPLS label. */
    bcm_gport_t flow_port;              /* flow port id representing a SVP */
    bcm_vpn_t vpn;                      /* VPN representing a vfi or vrf */
    bcm_if_t intf_id;                   /* l3 interface id */
    uint32 options;                     /* BCM_FLOW_MATCH_OPTION_xxx. */
    bcm_flow_match_criteria_t criteria; /* flow match criteria. */
    uint32 valid_elements;              /* bitmap of valid fields for the match
                                           action */
    bcm_flow_handle_t flow_handle;      /* flow handle derived from flow name */
    uint32 flow_option;                 /* flow option derived from flow option
                                           name */
    uint32 flags;                       /* BCM_FLOW_MATCH_FLAG_xxx */
} bcm_compat6522_flow_match_config_t;

extern int bcm_compat6522_flow_match_add(
    int unit,
    bcm_compat6522_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcm_compat6522_flow_match_delete(
    int unit,
    bcm_compat6522_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcm_compat6522_flow_match_get(
    int unit,
    bcm_compat6522_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcm_compat6522_flow_match_flexctr_object_set(
    int unit,
    bcm_compat6522_flow_match_config_t *info,
    uint32 value);

extern int bcm_compat6522_flow_match_flexctr_object_get(
    int unit,
    bcm_compat6522_flow_match_config_t *info,
    uint32 *value);

extern int bcm_compat6522_flow_match_stat_attach(
    int unit,
    bcm_compat6522_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 stat_counter_id);

extern int bcm_compat6522_flow_match_stat_detach(
    int unit,
    bcm_compat6522_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcm_compat6522_flow_match_stat_id_get(
    int unit,
    bcm_compat6522_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 *stat_counter_id);

#endif /* INCLUDE_L3 */

#if defined(INCLUDE_L3)
/* MPLS port type. */
typedef struct bcm_compat6522_mpls_port_s {
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
    uint32 class_id;    /* Class ID */
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
} bcm_compat6522_mpls_port_t;

/* Add an MPLS port to an L2 VPN. */
extern int bcm_compat6522_mpls_port_add(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6522_mpls_port_t *mpls_port);

/* Get an MPLS port from an L2 VPN. */
extern int bcm_compat6522_mpls_port_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6522_mpls_port_t *mpls_port);

/* Get all MPLS ports from an L2 VPN. */
extern int bcm_compat6522_mpls_port_get_all(
    int unit,
    bcm_vpn_t vpn,
    int port_max,
    bcm_compat6522_mpls_port_t *port_array,
    int *port_count);
#endif /* INCLUDE_L3 */

#if defined(INCLUDE_L3)
/* L3 Host Structure. */
typedef struct bcm_compat6522_l3_host_s {
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
} bcm_compat6522_l3_host_t;

/* Look up an L3 host table entry based on IP address. */
extern int bcm_compat6522_l3_host_find(
    int unit,
    bcm_compat6522_l3_host_t *info);

/* Add an entry into the L3 switching table. */
extern int bcm_compat6522_l3_host_add(
    int unit,
    bcm_compat6522_l3_host_t *info);

/* Delete an entry from the L3 host table. */
extern int bcm_compat6522_l3_host_delete(
    int unit,
    bcm_compat6522_l3_host_t *ip_addr);

/*
 * Deletes L3 entries that match or do not match a specified L3 interface
 * number
 */
extern int bcm_compat6522_l3_host_delete_by_interface(
    int unit,
    bcm_compat6522_l3_host_t *info);

/* Deletes all L3 host table entries. */
extern int bcm_compat6522_l3_host_delete_all(
    int unit,
    bcm_compat6522_l3_host_t *info);

/* Attach counters entries to the given L3 host entry */
extern int bcm_compat6522_l3_host_stat_attach(
    int unit,
    bcm_compat6522_l3_host_t *info,
    uint32 stat_counter_id);

/* Detach  counters entries to the given L3 host entry */
extern int bcm_compat6522_l3_host_stat_detach(
    int unit,
    bcm_compat6522_l3_host_t *info);

/* Set flex counter object value for the given L3 host entry. */
extern int bcm_compat6522_l3_host_flexctr_object_set(
    int unit,
    bcm_compat6522_l3_host_t *info,
    uint32 value);

/* Get flex counter object value for the given L3 host entry. */
extern int bcm_compat6522_l3_host_flexctr_object_get(
    int unit,
    bcm_compat6522_l3_host_t *info,
    uint32 *value);

/* Get the specified counter statistic for a L3 host entry. */
extern int bcm_compat6522_l3_host_stat_counter_get(
    int unit,
    bcm_compat6522_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

/*
 * Force an immediate counter update and retrieve the specified counter
 * statistic for a L3 host entry.
 */
extern int bcm_compat6522_l3_host_stat_counter_sync_get(
    int unit,
    bcm_compat6522_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

/* Set the specified counter statistic for a L3 host entry */
extern int bcm_compat6522_l3_host_stat_counter_set(
    int unit,
    bcm_compat6522_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

/* Provide stat counter ids associated with given L3 host entry */
extern int bcm_compat6522_l3_host_stat_id_get(
    int unit,
    bcm_compat6522_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 *stat_counter_id);

/* L3 Route Structure. */
typedef struct bcm_compat6522_l3_route_s {
    uint32 l3a_flags;                   /* See BCM_L3_xxx flag definitions. */
    uint32 l3a_flags2;                  /* See BCM_L3_FLAGS2_xxx flag
                                           definitions. */
    uint32 l3a_flags3;                  /* See BCM_L3_FLAGS3_xxx flag
                                           definitions. */
    uint32 l3a_ipmc_flags;              /* Used for multicast route entry. See
                                           BCM_IPMC_xxx flag definitions. */
    bcm_vrf_t l3a_vrf;                  /* Virtual router instance. */
    bcm_ip_t l3a_subnet;                /* IP subnet address (IPv4). */
    bcm_ip6_t l3a_ip6_net;              /* IP subnet address (IPv6). */
    bcm_ip_t l3a_ip_mask;               /* IP subnet mask (IPv4). */
    bcm_ip6_t l3a_ip6_mask;             /* IP subnet mask (IPv6). */
    bcm_if_t l3a_intf;                  /* L3 interface associated with route. */
    bcm_if_t l3a_ul_intf;               /* Underlay L3 egress object associated
                                           with this address. */
    bcm_ip_t l3a_nexthop_ip;            /* Next hop IP address (XGS1/2, IPv4). */
    bcm_mac_t l3a_nexthop_mac;          /* Next hop MAC address. */
    bcm_module_t l3a_modid;             /* Module ID. */
    bcm_port_t l3a_port_tgid;           /* Port or trunk group ID. */
    bcm_port_t l3a_stack_port;          /* Used if modid is not local (Strata
                                           Only). */
    bcm_vlan_t l3a_vid;                 /* BCM5695 only - for per-VLAN def
                                           route. */
    bcm_cos_t l3a_pri;                  /* Priority (COS). */
    uint32 l3a_tunnel_option;           /* Tunnel option value. */
    bcm_mpls_label_t l3a_mpls_label;    /* MPLS label. */
    int l3a_lookup_class;               /* Classification class ID. */
    bcm_if_t l3a_expected_intf;         /* Expected L3 Interface used for
                                           multicast RPF check */
    int l3a_rp;                         /* Rendezvous point ID */
    bcm_multicast_t l3a_mc_group;       /* L3 Multicast group index */
    bcm_gport_t l3a_expected_src_gport; /* L3 Multicast group expected source
                                           port/trunk */
    uint32 flow_handle;                 /* FLOW handle for flex entries. */
    uint32 flow_option_handle;          /* FLOW option handle for flex entries. */
    bcm_flow_logical_field_t logical_fields[BCM_FLOW_MAX_NOF_LOGICAL_FIELDS]; /* logical fields array for flex
                                           entries. */
    uint32 num_of_fields;               /* number of logical fields. */
    uint32 stat_id;                     /* Object statistics ID. */
    int stat_pp_profile;                /* Statistics profile. */
    int l3a_mtu;                        /* Multicast packet MTU. */
    int l3a_int_action_profile_id;      /* Profile ID for metadata generation
                                           and packet updating after metadata
                                           insertion. */
    int l3a_int_opaque_data_profile_id; /* Profile ID to the opaque data for
                                           metadata construction. */
    int l3a_opaque_ctrl_id;             /* Opaque control ID. */
} bcm_compat6522_l3_route_t;

/* Delete L3 entries based on IP prefix (network). */
extern int bcm_compat6522_l3_host_delete_by_network(
    int unit,
    bcm_compat6522_l3_route_t *ip_addr);

/* Add an IP route to the L3 route table. */
extern int bcm_compat6522_l3_route_add(
    int unit,
    bcm_compat6522_l3_route_t *info);

/* Delete an IP route from the DEFIP table. */
extern int bcm_compat6522_l3_route_delete(
    int unit,
    bcm_compat6522_l3_route_t *info);

/* Delete routes based on matching or non-matching L3 interface number. */
extern int bcm_compat6522_l3_route_delete_by_interface(
    int unit,
    bcm_compat6522_l3_route_t *info);

/* Delete all routes. */
extern int bcm_compat6522_l3_route_delete_all(
    int unit,
    bcm_compat6522_l3_route_t *info);

/* Look up a route given the network and netmask. */
extern int bcm_compat6522_l3_route_get(
    int unit,
    bcm_compat6522_l3_route_t *info);

/* Given a network, return all the paths for this route. */
extern int bcm_compat6522_l3_route_multipath_get(
    int unit,
    bcm_compat6522_l3_route_t *the_route,
    bcm_compat6522_l3_route_t *path_array,
    int max_path,
    int *path_count);

/* Set flex counter object value for the given L3 unicast route. */
extern int bcm_compat6522_l3_route_flexctr_object_set(
    int unit,
    bcm_compat6522_l3_route_t *info,
    uint32 value);

/* Get flex counter object value for the given L3 unicast route. */
extern int bcm_compat6522_l3_route_flexctr_object_get(
    int unit,
    bcm_compat6522_l3_route_t *info,
    uint32 *value);

/* Get L3 route statistics. */
extern int bcm_compat6522_l3_route_stat_get(
    int unit,
    bcm_compat6522_l3_route_t *route,
    bcm_l3_stat_t stat,
    uint64 *val);

/* Get L3 route statistics. */
extern int bcm_compat6522_l3_route_stat_get32(
    int unit,
    bcm_compat6522_l3_route_t *route,
    bcm_l3_stat_t stat,
    uint32 *val);

/* Set L3 route statistics. */
extern int bcm_compat6522_l3_route_stat_set(
    int unit,
    bcm_compat6522_l3_route_t *route,
    bcm_l3_stat_t stat,
    uint64 val);

/* Set L3 route statistics. */
extern int bcm_compat6522_l3_route_stat_set32(
    int unit,
    bcm_compat6522_l3_route_t *route,
    bcm_l3_stat_t stat,
    uint32 val);

/* Enable/disable collection of statistics for the indicated L3 route. */
extern int bcm_compat6522_l3_route_stat_enable_set(
    int unit,
    bcm_compat6522_l3_route_t *route,
    int enable);

/* Attach counters entries to the given L3 route index */
extern int bcm_compat6522_l3_route_stat_attach(
    int unit,
    bcm_compat6522_l3_route_t *info,
    uint32 stat_counter_id);

/* Detach  counters entries to the given L3 route index. */
extern int bcm_compat6522_l3_route_stat_detach(
    int unit,
    bcm_compat6522_l3_route_t *info);

/* Get counter statistic values for a l3 route index. */
extern int bcm_compat6522_l3_route_stat_counter_get(
    int unit,
    bcm_compat6522_l3_route_t *info,
    bcm_l3_route_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

/*
 * Force an immediate counter update and retrieve counter statistic
 * values for a l3 route index.
 */
extern int bcm_compat6522_l3_route_stat_counter_sync_get(
    int unit,
    bcm_compat6522_l3_route_t *info,
    bcm_l3_route_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

/* Set counter statistic values for a l3 route index */
extern int bcm_compat6522_l3_route_stat_counter_set(
    int unit,
    bcm_compat6522_l3_route_t *info,
    bcm_l3_route_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

/* Get multiple counter statistic values for multiple l3 route index. */
extern int bcm_compat6522_l3_route_stat_multi_get(
    int unit,
    bcm_compat6522_l3_route_t *info,
    int nstat,
    bcm_l3_route_stat_t *stat_arr,
    uint64 *value_arr);

/* Get 32bit multiple counter statistic values for multiple l3 route index. */
extern int bcm_compat6522_l3_route_stat_multi_get32(
    int unit,
    bcm_compat6522_l3_route_t *info,
    int nstat,
    bcm_l3_route_stat_t *stat_arr,
    uint32 *value_arr);

/* Set multiple counter statistic values for multiple l3 route index. */
extern int bcm_compat6522_l3_route_stat_multi_set(
    int unit,
    bcm_compat6522_l3_route_t *info,
    int nstat,
    bcm_l3_route_stat_t *stat_arr,
    uint64 *value_arr);

/* Set 32bit multiple counter statistic values for multiple l3 route index. */
extern int bcm_compat6522_l3_route_stat_multi_set32(
    int unit,
    bcm_compat6522_l3_route_t *info,
    int nstat,
    bcm_l3_route_stat_t *stat_arr,
    uint32 *value_arr);

/* Get stat counter ID associated with given L3 route index. */
extern int bcm_compat6522_l3_route_stat_id_get(
    int unit,
    bcm_compat6522_l3_route_t *info,
    bcm_l3_route_stat_t stat,
    uint32 *stat_counter_id);

/* Find a longest prefix matched route given an IP address. */
extern int bcm_compat6522_l3_route_find(
    int unit,
    bcm_compat6522_l3_host_t *host,
    bcm_compat6522_l3_route_t *route);

/* Find a longest prefix matched route given the ip and netmask. */
extern int bcm_compat6522_l3_subnet_route_find(
    int unit,
    bcm_compat6522_l3_route_t *input,
    bcm_compat6522_l3_route_t *route);
#endif /* INCLUDE_L3 */

/* Mirror destination Structure

@p Contains information required for manipulating mirror destinations.

 */
typedef struct bcm_compat6522_mirror_destination_s {
    bcm_gport_t mirror_dest_id;    /* Unique mirror destination and encapsulation identifier. */
    uint32 flags;    /* See BCM_MIRROR_DEST_xxx flag definitions. */
    bcm_gport_t gport;    /* Mirror destination. */
    uint8 version;    /* IP header version. */
    uint8 tos;    /* Traffic Class/Tos byte. */
    uint8 ttl;    /* Hop limit. */
    bcm_ip_t src_addr;    /* Tunnel source ip address (IPv4). */
    bcm_ip_t dst_addr;    /* Tunnel destination ip address (IPv4). */
    bcm_ip6_t src6_addr;    /* Tunnel source ip address (IPv6). */
    bcm_ip6_t dst6_addr;    /* Tunnel destination ip address (IPv6). */
    uint32 flow_label;    /* IPv6 header flow label field. */
    bcm_mac_t src_mac;    /* L2 source mac address. */
    bcm_mac_t dst_mac;    /* L2 destination mac address. */
    uint16 tpid;    /* L2 header outer TPID. */
    bcm_vlan_t vlan_id;    /* Vlan id. */
    bcm_trill_name_t trill_src_name;    /* TRILL source bridge nickname */
    bcm_trill_name_t trill_dst_name;    /* TRILL destination bridge nickname */
    int trill_hopcount;    /* TRILL hop count */
    uint16 niv_src_vif;    /* Source Virtual Interface of NIV tag */
    uint16 niv_dst_vif;    /* Destination Virtual Interface of NIV tag */
    uint32 niv_flags;    /* NIV flags BCM_MIRROR_NIV_XXX */
    uint16 gre_protocol;    /* L3 GRE header protocol */
    bcm_policer_t policer_id;    /* policer_id */
    int stat_id;    /* stat_id */
    int stat_id2;    /* stat_id2 for second counter pointer */
    bcm_if_t encap_id;    /* Encapsulation index */
    bcm_if_t tunnel_id;    /* IP tunnel for encapsulation. Valid only if BCM_MIRROR_DEST_TUNNEL_IP_GRE is set */
    uint16 span_id;    /* SPAN-ID. Valid only if BCM_MIRROR_DEST_TUNNEL_WITH_SPAN_ID is set */
    uint8 pkt_prio;    /* L2 header PCP */
    uint32 sample_rate_dividend;    /* The probability of mirroring a packet is: sample_rate_dividend >= sample_rate_divisor ? 1 : sample_rate_dividend / sample_rate_divisor */
    uint32 sample_rate_divisor;    /*  */
    uint8 int_pri;    /* Internal Priority */
    uint16 etag_src_vid;    /* Extended (source) port vlan id */
    uint16 etag_dst_vid;    /* Extended (destination) port vlan id  */
    uint16 udp_src_port;    /* UDP source port */
    uint16 udp_dst_port;    /* UDP destination port */
    uint32 egress_sample_rate_dividend;    /* The probability of outbound mirroring a packet from the destination is sample_rate_dividend >= sample_rate_divisor ? 1 : sample_rate_dividend / sample_rate_divisor */
    uint32 egress_sample_rate_divisor;    /*  */
    uint8 recycle_context;    /* recycle context of egress originating packets */
    uint16 packet_copy_size;    /*  If non zero, the copied packet will be truncated to the first packet_copy_size . Current supported values for DNX are 0, 64, 128, 192 */
    uint16 egress_packet_copy_size;    /* If non zero and the packet is copied from the egress, the packet will be truncated to the first packet_copy_size . Current supported values for DNX are 0, 256. */
    bcm_mirror_pkt_header_updates_t packet_control_updates;    /*  */
    bcm_mirror_psc_t rtag;    /* specify RTAG HASH algorithm used for shared-id mirror destination */
    uint8 df;    /* Set the do not fragment bit of IP header in mirror encapsulation */
    uint8 truncate;    /* Setting truncate would result in mirroring a truncated frame  */
    uint16 template_id;    /* Template ID for IPFIX HDR */
    uint32 observation_domain;    /* Observation domain for IPFIX HDR */
    uint32 is_recycle_strict_priority;    /* Recycle priority (1-lossless, 0-high) */
    int ext_stat_id[BCM_MIRROR_EXT_STAT_ID_COUNT];    /* ext_stat_id to support statistic interface for mirror packets. */
    uint32 flags2;    /* See BCM_MIRROR_DEST_FLAGS2_xxx flag definitions. */
    uint32 vni;    /* Virtual Network Interface ID. */
    uint32 gre_seq_number;    /* Sequence number value used in GRE header. If no value is provided, gre_seq_number will start with 0. Valid only if BCM_MIRROR_DEST_TUNNEL_IP_GRE is set. */
    bcm_mirror_pkt_erspan_encap_t erspan_header;    /* ERSPAN encapsulation header fields. Valid only if only BCM_MIRROR_DEST_TUNNEL_IP_GRE is set. */
    uint32 initial_seq_number;    /* Starting seq number in SFLOW or PSAMP header */
    bcm_mirror_psamp_fmt2_meta_data_t meta_data_type;    /* Type of item in 'meta_data' */
    uint16 meta_data;    /* Actual value of class id or meta data, based on 'meta_data_type' */
    int ext_stat_valid;    /* Validates the ext_stat. */
    int ext_stat_type_id[BCM_MIRROR_EXT_STAT_ID_COUNT];    /* Type ids for statistic interface. */
    uint16 ipfix_version;    /* IPFIX version number in PSAMP encapsulation. */
    uint16 psamp_epoch;    /* PSAMP epoch field. */
    int cosq;    /* Queue id that mirror copy is enqueued into. */
    uint8 cfi;    /* CFI of VLAN tag. */
    bcm_mirror_timestamp_mode_t timestamp_mode;    /* Set timestamp mode for mirror encapsulation. */
    bcm_mirror_multi_dip_group_t multi_dip_group;    /* Multiple destination IP group. */
    int drop_group_bmp;    /*  */
    bcm_port_t second_pass_src_port;    /* Source port in the second pass. */
    bcm_mirror_truncate_mode_t encap_truncate_mode;    /* Truncate mode for mirror encapsulation packets. */
    int encap_truncate_profile_id;    /* Truncate length profile ID for mirror encapsulation packets. */
    bcm_mirror_loopback_header_type_t loopback_header_type;    /* Loopback header type. */
    bcm_port_t second_pass_dst_port;    /* Destination port in the second pass. */
    bcm_mirror_int_probe_header_t int_probe_header;    /* INT(Inband Network Telemetry) probe header for mirror encapsulation packets. */
    uint8 duplicate_pri;    /* The priority of duplicate mirror packets. The packet with highest priority (0 is lowest) will be mirrored and others will be dropped. Applicable only if duplication is not allowed. */
    int ip_proto;    /* IP Protocol number in ERSPAN IP encap. */
    uint32 switch_id;    /* Switch ID in the PSAMP mirror-on-drop encapsulation header. */
} bcm_compat6522_mirror_destination_t;

/* Add mirror destination descriptor. */
extern int bcm_compat6522_mirror_destination_create(
    int unit,
    bcm_compat6522_mirror_destination_t *mirror_dest);

/* Read content of mirror destination descriptor */
extern int bcm_compat6522_mirror_destination_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    bcm_compat6522_mirror_destination_t *mirror_dest);

#if defined(INCLUDE_L3)
/* Extender port type. */
typedef struct bcm_compat6522_extender_port_s {
    bcm_extender_port_match_t criteria; /* Match criteria. */
    uint32 flags;                       /* BCM_EXTENDER_PORT_xxx. */
    bcm_gport_t extender_port_id;       /* GPORT identifier. */
    bcm_gport_t port;                   /* Physical port / trunk */
    uint16 extended_port_vid;           /* Extender port VID. */
    bcm_vlan_t match_vlan;              /* Outer VLAN ID to match. */
    bcm_vlan_t match_inner_vlan;        /* Inner VLAN ID to match. */
    bcm_extender_pcp_de_select_t pcp_de_select; /* Selection of PCP and DE fields for
                                           egress ETAG. */
    int qos_map_id;                     /* Qos map id for egress etag mapping
                                           profile. */
    uint8 pcp;                          /* Default PCP field of ETAG. */
    uint8 de;                           /* Default DE field of ETAG. */
    bcm_if_t encap_id;                  /* Encap identifier. */
    bcm_gport_t failover_port_id;       /* Failover id. */
    bcm_failover_t failover_id;         /* Failover object index. */
    bcm_failover_t ingress_failover_id; /* 1+1 protection. */
    bcm_multicast_t failover_mc_group;  /* MC group for 1+1 scheme failover. */
    int inlif_counting_profile;         /* In LIF counting profile */
    int outlif_counting_profile;        /* Out LIF counting profile */
} bcm_compat6522_extender_port_t;


/* Create a Port Extender port. */
extern int bcm_compat6522_extender_port_add(
    int unit,
    bcm_compat6522_extender_port_t *extender_port);

/* Get information about a Port Extender port. */
extern int bcm_compat6522_extender_port_get(
    int unit,
    bcm_compat6522_extender_port_t *extender_port);
#endif /* INCLUDE_L3 */

/* VLAN Egress Translate Key Configuration. */
typedef struct bcm_compat6522_vlan_translate_egress_key_config_s {
    uint32 flags;    /*  */
    bcm_vlan_translate_key_t key_type;    /* Lookup Key Type. */
    bcm_vlan_t inner_vlan;    /* Inner VLAN ID to match. */
    bcm_vlan_t outer_vlan;    /* Outer VLAN ID to match. */
    bcm_gport_t port;    /* Port to match on. */
    int otag_preserve;    /* Outer tag preserve to match on. */
    int itag_preserve;    /* Inner tag preserve to match on. */
} bcm_compat6522_vlan_translate_egress_key_config_t;

/* Add an entry to the egress VLAN Translation table and assign VLAN actions. It extends bcm_vlan_egress_action_add API for additional keys. */
extern int bcm_compat6522_vlan_translate_egress_action_extended_add(
    int unit,
    bcm_compat6522_vlan_translate_egress_key_config_t *key_config,
    bcm_vlan_translate_egress_action_set_t *action);

/* Delete an egress translation entry including key types added by bcm_vlan_translate_egress_extended_action_add API. */
extern int bcm_compat6522_vlan_translate_egress_action_extended_delete(
    int unit,
    bcm_compat6522_vlan_translate_egress_key_config_t *key_config);

/* Retrive an egress translation entry including key types added by bcm_vlan_translate_egress_extended_action_add API. */
extern int bcm_compat6522_vlan_translate_egress_action_extended_get(
    int unit,
    bcm_compat6522_vlan_translate_egress_key_config_t *key_config,
    bcm_vlan_translate_egress_action_set_t *action);

/* Attach a flex counter into a given egress VLAN translation table with additional keys. */
extern int bcm_compat6522_vlan_translate_egress_flexctr_extended_attach(
    int unit,
    bcm_compat6522_vlan_translate_egress_key_config_t *key_config,
    bcm_vlan_flexctr_config_t *config);

/* Detach a flex counter from a given egress VLAN translation table with additional keys. */
extern int bcm_compat6522_vlan_translate_egress_flexctr_extended_detach(
    int unit,
    bcm_compat6522_vlan_translate_egress_key_config_t *key_config,
    bcm_vlan_flexctr_config_t *config);

/* Detach all flex counters from a given egress VLAN translation table with additional keys. */
extern int bcm_compat6522_vlan_translate_egress_flexctr_extended_detach_all(
    int unit,
    bcm_compat6522_vlan_translate_egress_key_config_t *key_config);

/* Get the attached flex counters from a given egress VLAN translation table with additional keys. */
extern int bcm_compat6522_vlan_translate_egress_flexctr_extended_get(
    int unit,
    bcm_compat6522_vlan_translate_egress_key_config_t *key_config,
    int array_size,
    bcm_vlan_flexctr_config_t *config_array,
    int *count);

typedef  struct  bcm_compat6522_port_resource_s {
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
} bcm_compat6522_port_resource_t;

/* get the default config for the port */
extern int bcm_compat6522_port_resource_get(
    int unit,
    bcm_gport_t port,
    bcm_compat6522_port_resource_t *match_array);
/* set the default config for the port */
extern int bcm_compat6522_port_resource_set(
    int unit,
    bcm_gport_t port,
    bcm_compat6522_port_resource_t *match_array);
/* set the default speed for the port */
extern int bcm_compat6522_port_resource_speed_set(
    int unit,
    bcm_gport_t port,
    bcm_compat6522_port_resource_t *match_array);
/* get the default speed for the port */
extern int bcm_compat6522_port_resource_speed_get(
    int unit,
    bcm_gport_t port,
    bcm_compat6522_port_resource_t *match_array);
/* Validate a port config */
int bcm_compat6522_port_resource_speed_config_validate(
    int unit,
    bcm_compat6522_port_resource_t *match_array,
    bcm_pbmp_t *pbmp);

/* Flex port api to add /delete multiple ports */
extern int bcm_compat6522_port_resource_speed_multi_set(
    int unit,
    int nport,
    bcm_compat6522_port_resource_t *match_array)
;
extern int bcm_compat6522_port_resource_default_get(
    int unit,
    bcm_gport_t port,
    int flags,
    bcm_compat6522_port_resource_t *match_array);
/*
 * Performs a FlexPort operation changing the port resources for a set of
 * ports.
 */
extern int bcm_compat6522_port_resource_multi_set(
    int unit,
    int nport,
    bcm_compat6522_port_resource_t *match_array);

#endif /* BCM_RPC_SUPPORT */
#endif /* _COMPAT_6522_H_ */
