/*
* $Id: compat_6521.h,v 1.0 2020/12/02
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2021 Broadcom Inc. All rights reserved.
*
* RPC Compatibility with sdk-6.5.21 routines
*/

#ifndef _COMPAT_6521_H_
#define _COMPAT_6521_H_

#ifdef BCM_RPC_SUPPORT
#if defined(INCLUDE_L3)
#include <bcm/vxlan.h>
#endif /* defined(INCLUDE_L3) */
#include <bcm/port.h>
#include <bcm/mpls.h>
#include <bcm/flow.h>
#include <bcm/types.h>
#if defined(INCLUDE_L3)
#include <bcm/l3.h>
#endif /* defined(INCLUDE_L3) */

#if defined(INCLUDE_BFD)
#include <bcm/bfd.h>
#endif /* defined(INCLUDE_BFD) */




#if defined(INCLUDE_L3)
/* L3 ECMP structure */
typedef struct bcm_compat6521_l3_egress_ecmp_s {
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
} bcm_compat6521_l3_egress_ecmp_t;

/* Resilient ecmp entry */
typedef struct bcm_compat6521_l3_egress_ecmp_resilient_entry_s {
    uint64 hash_key;    /* Hash key. */
    bcm_compat6521_l3_egress_ecmp_t ecmp;    /* ECMP. */
    bcm_if_t intf;    /* L3 interface. */
} bcm_compat6521_l3_egress_ecmp_resilient_entry_t;

/* Replace ECMP resilient entries matching given criteria. */
extern int bcm_compat6521_l3_egress_ecmp_resilient_replace(
    int unit,
    uint32 flags,
    bcm_compat6521_l3_egress_ecmp_resilient_entry_t *match_entry,
    int *num_entries,
    bcm_compat6521_l3_egress_ecmp_resilient_entry_t *replace_entry);

/* Add ECMP resilient entries matching given criteria. */
extern int bcm_compat6521_l3_egress_ecmp_resilient_add(
    int unit,
    uint32 flags,
    bcm_compat6521_l3_egress_ecmp_resilient_entry_t *entry);

/* Delete ECMP resilient entries matching given criteria. */
extern int bcm_compat6521_l3_egress_ecmp_resilient_delete(
    int unit,
    uint32 flags,
    bcm_compat6521_l3_egress_ecmp_resilient_entry_t *entry);

/* Create an Egress ECMP forwarding object. */
extern int bcm_compat6521_l3_ecmp_create(
    int unit,
    uint32 options,
    bcm_compat6521_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_count,
    bcm_l3_ecmp_member_t *ecmp_member_array);

/* Get info about an Egress ECMP forwarding object. */
extern int bcm_compat6521_l3_ecmp_get(
    int unit,
    bcm_compat6521_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_size,
    bcm_l3_ecmp_member_t *ecmp_member_array,
    int *ecmp_member_count);

/* Find an Egress ECMP forwarding object. */
extern int bcm_compat6521_l3_ecmp_find(
    int unit,
    int ecmp_member_count,
    bcm_l3_ecmp_member_t *ecmp_member_array,
    bcm_compat6521_l3_egress_ecmp_t *ecmp_info);

/* Create an Egress ECMP forwarding object. */
extern int bcm_compat6521_l3_egress_ecmp_create(
    int unit,
    bcm_compat6521_l3_egress_ecmp_t *ecmp,
    int intf_count,
    bcm_if_t *intf_array);

/* Destroy an Egress ECMP forwarding object. */
extern int bcm_compat6521_l3_egress_ecmp_destroy(
    int unit,
    bcm_compat6521_l3_egress_ecmp_t *ecmp);

/* Get info about an Egress ECMP forwarding object. */
extern int bcm_compat6521_l3_egress_ecmp_get(
    int unit,
    bcm_compat6521_l3_egress_ecmp_t *ecmp,
    int intf_size,
    bcm_if_t *intf_array,
    int *intf_count);

/* Add an Egress forwarding object to an Egress ECMP forwarding object. */
extern int bcm_compat6521_l3_egress_ecmp_add(
    int unit,
    bcm_compat6521_l3_egress_ecmp_t *ecmp,
    bcm_if_t intf);

/* Delete an Egress forwarding object from an Egress ECMP forwarding object. */
extern int bcm_compat6521_l3_egress_ecmp_delete(
    int unit,
    bcm_compat6521_l3_egress_ecmp_t *ecmp,
    bcm_if_t intf);

/* Find an Egress ECMP forwarding object. */
extern int bcm_compat6521_l3_egress_ecmp_find(
    int unit,
    int intf_count,
    bcm_if_t *intf_array,
    bcm_compat6521_l3_egress_ecmp_t *ecmp);

/* Update an ECMP table that was already allocated, used in cases where an ECMP have more than one table. */
extern int bcm_compat6521_l3_egress_ecmp_tunnel_priority_set(
    int unit,
    bcm_compat6521_l3_egress_ecmp_t *ecmp,
    int intf_count,
    bcm_if_t *intf_array);


#endif /* defined(INCLUDE_L3) */
#if defined(INCLUDE_L3)
/* structure of MPLS esi object. */
typedef struct bcm_compat6521_mpls_esi_info_s {
    uint32 flags;    /* BCM_MPLS_ESI_xxx. */
    bcm_gport_t src_port;    /* GPORT of incoming eth segment's port. */
    uint32 out_class_id;    /* Multi-homing peer class. */
    bcm_mpls_label_t esi_label;    /* Will be set on the ESI label. */
} bcm_compat6521_mpls_esi_info_t;

/* Add Egress special labels. */
extern int bcm_compat6521_mpls_esi_encap_add(
    int unit,
    bcm_compat6521_mpls_esi_info_t *esi_info);

/* Get esi encap id. */
extern int bcm_compat6521_mpls_esi_encap_get(
    int unit,
    bcm_compat6521_mpls_esi_info_t *esi_info);

/* Delete esi encap id values. */
extern int bcm_compat6521_mpls_esi_encap_delete(
    int unit,
    bcm_compat6521_mpls_esi_info_t *esi_info);

/* Flow tunneling initiator structure. */
typedef struct bcm_compat6521_flow_tunnel_initiator_s {
    uint32 flags;    /* Configuration flags. */
    bcm_tunnel_type_t type;    /* Tunnel type. */
    int ttl;    /* Tunnel header TTL. */
    bcm_mac_t dmac;    /* Destination MAC address. */
    bcm_ip_t dip;    /* Tunnel header DIP (IPv4). */
    bcm_ip_t sip;    /* Tunnel header SIP (IPv4). */
    bcm_ip6_t sip6;    /* Tunnel header SIP (IPv6). */
    bcm_ip6_t dip6;    /* Tunnel header DIP (IPv6). */
    uint32 flow_label;    /* Tunnel header flow label (IPv6). */
    bcm_tunnel_dscp_select_t dscp_sel;    /* Tunnel header DSCP select. */
    int dscp;    /* Tunnel header DSCP value. */
    int dscp_map;    /* DSCP-map ID. */
    bcm_gport_t tunnel_id;    /* Tunnel ID */
    uint16 udp_dst_port;    /* Destination UDP port */
    uint16 udp_src_port;    /* Source UDP port */
    bcm_mac_t smac;    /* Src MAC */
    bcm_vlan_t vlan;    /* Tunnel VLAN */
    uint16 tpid;    /* Tunnel TPID */
    uint8 pkt_pri;    /* Tunnel priority */
    uint8 pkt_cfi;    /* Tunnel CFI */
    uint16 ip4_id;    /* IPv4 ID. */
    bcm_if_t l3_intf_id;    /* l3 Interface ID. */
    bcm_gport_t flow_port;    /* Flow port ID */
    uint32 valid_elements;    /* bitmap of valid fields */
    uint32 flow_handle;    /* flow handle */
    uint32 flow_option;    /* flow option */
    bcm_tunnel_dscp_ecn_select_t dscp_ecn_sel;    /* Tunnel header DSCP and ECN select. */
    int dscp_ecn_map;    /* DSCP and ECN map ID. */
    uint8 ecn;    /* Tunnel header ECN value */
    bcm_tunnel_pri_select_t pri_sel;    /* Tunnel header priority and CFI select. */
} bcm_compat6521_flow_tunnel_initiator_t;

/* Create a flow tunnel initiator object */
extern int bcm_compat6521_flow_tunnel_initiator_create(
    int unit,
    bcm_compat6521_flow_tunnel_initiator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Get the flow tunnel initiator object */
extern int bcm_compat6521_flow_tunnel_initiator_get(
    int unit,
    bcm_compat6521_flow_tunnel_initiator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);


#endif /* defined(INCLUDE_L3) */

#if defined(INCLUDE_BFD)
/* BFD RX discard stats structure */
typedef struct bcm_compat6521_bfd_discard_stat_s {
    uint32 bfd_ver_err;
    uint32 bfd_len_err;
    uint32 bfd_detect_mult;
    uint32 bfd_my_disc;
    uint32 bfd_p_f_bit;
    uint32 bfd_m_bit;
    uint32 bfd_auth_type_mismatch;
    uint32 bfd_auth_simple_err;
    uint32 bfd_auth_m_sha1_err;
    uint32 bfd_sess_mismatch;
} bcm_compat6521_bfd_discard_stat_t;

extern int bcm_compat6521_bfd_discard_stat_get(
    int unit,
    bcm_compat6521_bfd_discard_stat_t *discarded_info);

extern int bcm_compat6521_bfd_discard_stat_set(
    int unit,
    bcm_compat6521_bfd_discard_stat_t *discarded_info);
#endif /* defined(INCLUDE_BFD) */

/* port mapping information */
typedef struct bcm_compat6521_port_extender_mapping_info_s {
    bcm_gport_t pp_port;    /* Mapped Local PP port */
    bcm_tunnel_id_t tunnel_id;    /* Tunnel-ID */
    bcm_gport_t phy_port;    /* Physical Port connected to interface x channel */
    bcm_vlan_t vlan;    /* VLAN-ID */
    uint32 user_define_value;    /* User define value from header */
} bcm_compat6521_port_extender_mapping_info_t;

/* Configure/retrieve port extender mapping from different inputs to Local pp port. */
extern int bcm_compat6521_port_extender_mapping_info_set(
    int unit,
    uint32 flags,
    bcm_port_extender_mapping_type_t type,
    bcm_compat6521_port_extender_mapping_info_t *mapping_info);

/* Configure/retrieve port extender mapping from different inputs to Local pp port. */
extern int bcm_compat6521_port_extender_mapping_info_get(
    int unit,
    uint32 flags,
    bcm_port_extender_mapping_type_t type,
    bcm_compat6521_port_extender_mapping_info_t *mapping_info);

/* Mirror source structure. */
typedef struct bcm_compat6521_mirror_source_s {
    bcm_mirror_source_type_t type;      /* Mirror source type. */
    uint32 flags;                       /* Mirror source flags. See
                                           MIRROR_SOURCE_xxx flag definitions. */
    bcm_port_t port;                    /* Port/gport id. */
    bcm_pkt_trace_event_t trace_event;  /* Trace event. */
    bcm_pkt_drop_event_t drop_event;    /* Drop event. */
    int mod_profile_id;                 /* Mirror-on-drop profile id. */
    uint32 sample_profile_id;           /* Sample profile id. */
    int ep_recirc_profile_id;           /* EP Recirculate profile id. */
} bcm_compat6521_mirror_source_t;

/* Add a mirror destination to a mirror source. */
extern int bcm_compat6521_mirror_source_dest_add(
    int unit,
    bcm_compat6521_mirror_source_t *source,
    bcm_gport_t mirror_dest_id);

/* Delete a mirror destination from a mirror source. */
extern int bcm_compat6521_mirror_source_dest_delete(
    int unit,
    bcm_compat6521_mirror_source_t *source,
    bcm_gport_t mirror_dest_id);

/* Delete all mirror destinations applied on a mirror source. */
extern int bcm_compat6521_mirror_source_dest_delete_all(
    int unit,
    bcm_compat6521_mirror_source_t *source);

/* Get all mirror destinations applied on a mirror source. */
extern int bcm_compat6521_mirror_source_dest_get_all(
    int unit,
    bcm_compat6521_mirror_source_t *source,
    int array_size,
    bcm_gport_t *mirror_dest,
    int *count);

#if defined(INCLUDE_L3)
/* VXLAN Network Domain Config Structure. */
typedef struct bcm_compat6521_vxlan_network_domain_config_s {
    uint32 flags;    /* BCM_VXLAN_NETWORK_DOMAIN_CONFIG_XXX. */
    uint32 vsi;    /* Virtual switch instance. Populated for l2 mapping only. */
    bcm_vrf_t vrf;    /* Virtual router instance. Populated for l3 mapping only. */
    uint32 network_domain;    /* Network identifier. Used to distinct between multiple network. */
    uint32 vni;    /* VXLAN packet VNID. */
    int qos_map_id;    /* QoS Mapped priority */
    bcm_gport_t vlan_port_id;    /* Vlan gport identifier */
} bcm_compat6521_vxlan_network_domain_config_t;

/* Add a local identifier (vsi or vrf) with a VNI mapping. */
extern int bcm_compat6521_vxlan_network_domain_config_add(
    int unit,
    bcm_compat6521_vxlan_network_domain_config_t *config);

/* Get local identifier (vsi or vrf) with VNI mapping. */
extern int bcm_compat6521_vxlan_network_domain_config_get(
    int unit,
    bcm_compat6521_vxlan_network_domain_config_t *config);

/* Remove local identifier (vsi or vrf) with VNI mapping. */
extern int bcm_compat6521_vxlan_network_domain_config_remove(
    int unit,
    bcm_compat6521_vxlan_network_domain_config_t *config);


#endif /* defined(INCLUDE_L3) */
#endif /* BCM_RPC_SUPPORT */
#endif /* _COMPAT_6521_H_ */
