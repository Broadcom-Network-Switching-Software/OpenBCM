/*
* $Id: compat_6523.h,v 1.0 2021/04/13
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2022 Broadcom Inc. All rights reserved.
*
* RPC Compatibility with sdk-6.5.23 routines
*/

#ifndef _COMPAT_6523_H_
#define _COMPAT_6523_H_

#ifdef BCM_RPC_SUPPORT
#include <bcm_int/compat/compat_6526.h>
#include <bcm_int/compat/compat_6524.h>
#if defined(INCLUDE_L3)
#include <bcm/l2gre.h>
#endif /* defined(INCLUDE_L3) */
#if defined(INCLUDE_L3)
#include <bcm/wlan.h>
#endif /* defined(INCLUDE_L3) */
#if defined(INCLUDE_L3)
#include <bcm/vxlan.h>
#endif /* defined(INCLUDE_L3) */
#if defined(INCLUDE_L3)
#include <bcm/tunnel.h>
#endif /* defined(INCLUDE_L3) */
#include <bcm/trunk.h>
#include <bcm/vlan.h>
#include <bcm/types.h>
#if defined(INCLUDE_L3)
#include <bcm/l3.h>
#include <bcm/flow.h>
#endif /* INCLUDE_L3 */
#include <bcm/switch.h>
#include <bcm/oam.h>
#include <bcm/port.h>
#include <bcm/flowtracker.h>

/* Trunk group attributes structure. */
typedef struct bcm_compat6523_trunk_info_s {
    uint32 flags;                       /* BCM_TRUNK_FLAG_xxx. */
    int psc;                            /* Port selection criteria. */
    bcm_trunk_psc_profile_info_t psc_info; /* Port selection criteria
                                              information. */
    int ipmc_psc;                       /* Port selection criteria for software
                                           IPMC trunk resolution. */
    int dlf_index;                      /* DLF/broadcast port for trunk
                                           group. */
    int mc_index;                       /* Multicast port for trunk group. */
    int ipmc_index;                     /* IPMC port for trunk group. */
    int dynamic_size;                   /* Number of flows for dynamic load
                                           balancing. Valid values are 512, 1k,
                                           doubling up to 32k */
    int dynamic_age;                    /* Inactivity duration, in
                                           microseconds. */
    int dynamic_load_exponent;          /* The exponent used in the
                                           exponentially weighted moving average
                                           calculation of historical member
                                           load. */
    int dynamic_expected_load_exponent; /* The exponent used in the
                                           exponentially weighted moving average
                                           calculation of historical expected
                                           member load. */
    bcm_trunk_t master_tid;             /* Master trunk id. */
    bcm_trunk_dgm_t dgm;                /* DGM properties */
} bcm_compat6523_trunk_info_t;

/* Trunk multiset information structure. */
typedef struct bcm_compat6523_trunk_multiset_info_s {
    bcm_trunk_t tid;                /* Trunk ID */
    bcm_compat6523_trunk_info_t trunk_info;    /* Trunk info structure
                                                  (see \ref bcm_trunk_info_t) */
    int member_count;               /* Number of members in this trunk. */
    int set_result;                 /* Trunk set result (returned error) */
} bcm_compat6523_trunk_multiset_info_t;

/*
 * Get the current attributes and member ports for the specified trunk
 * group.
 */
extern int bcm_compat6523_trunk_get(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6523_trunk_info_t *trunk_info,
    int member_max,
    bcm_trunk_member_t *member_array,
    int *member_count);

/* Specify the ports in a trunk group. */
extern int bcm_compat6523_trunk_set(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6523_trunk_info_t *trunk_info,
    int member_count,
    bcm_trunk_member_t *member_array);

/* Specify and update the ports in multiple trunk groups. */
extern int bcm_compat6523_trunk_multi_set(
    int unit,
    int trunk_count,
    bcm_compat6523_trunk_multiset_info_t *trunk_array,
    int tot_member_count,
    bcm_trunk_member_t *member_array);


#if defined(INCLUDE_L3)
typedef struct bcm_compat6523_flow_match_config_s {
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
    bcm_vlan_t default_vlan;            /* Default VLAN ID to match. */
    bcm_vpn_t default_vpn;              /* Default VPN ID to match. */
} bcm_compat6523_flow_match_config_t;

extern int bcm_compat6523_flow_match_add(
    int unit,
    bcm_compat6523_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcm_compat6523_flow_match_delete(
    int unit,
    bcm_compat6523_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcm_compat6523_flow_match_get(
    int unit,
    bcm_compat6523_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcm_compat6523_flow_match_flexctr_object_set(
    int unit,
    bcm_compat6523_flow_match_config_t *info,
    uint32 value);

extern int bcm_compat6523_flow_match_flexctr_object_get(
    int unit,
    bcm_compat6523_flow_match_config_t *info,
    uint32 *value);

extern int bcm_compat6523_flow_match_stat_attach(
    int unit,
    bcm_compat6523_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 stat_counter_id);

extern int bcm_compat6523_flow_match_stat_detach(
    int unit,
    bcm_compat6523_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcm_compat6523_flow_match_stat_id_get(
    int unit,
    bcm_compat6523_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 *stat_counter_id);

#endif /* INCLUDE_L3 */


#if defined(INCLUDE_L3)
/* Flow encap config structure */
typedef struct bcm_compat6523_flow_encap_config_s {
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
    int vlan_pri_map_id;  /* VLAN priority and CFI map ID. */
} bcm_compat6523_flow_encap_config_t;

/* Add a flow encap entry for L2 tunnel or L3 tunnel. */
extern int bcm_compat6523_flow_encap_add(
    int unit,
    bcm_compat6523_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Delete a flow encap entry based on the given key.  */
extern int bcm_compat6523_flow_encap_delete(
    int unit,
    bcm_compat6523_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Get a flow encap entry based on the given key.  */
extern int bcm_compat6523_flow_encap_get(
    int unit,
    bcm_compat6523_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Set the flex counter object value to the encap entry. */
extern int bcm_compat6523_flow_encap_flexctr_object_set(
    int unit,
    bcm_compat6523_flow_encap_config_t *info,
    uint32 value);

/* Get the flex counter object value from the encap entry. */
extern int bcm_compat6523_flow_encap_flexctr_object_get(
    int unit,
    bcm_compat6523_flow_encap_config_t *info,
    uint32 *value);

/* Attach counter entries to the encap entry. */
extern int bcm_compat6523_flow_encap_stat_attach(
    int unit,
    bcm_compat6523_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 stat_counter_id);

/* Detach the counter entries from the encap entry. */
extern int bcm_compat6523_flow_encap_stat_detach(
    int unit,
    bcm_compat6523_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Get stat counter ID associated to the encap entry. */
extern int bcm_compat6523_flow_encap_stat_id_get(
    int unit,
    bcm_compat6523_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 *stat_counter_id);

#endif /* defined(INCLUDE_L3) */


#if defined(INCLUDE_L3)
/* Flow tunneling terminator structure. */
typedef struct bcm_compat6523_flow_tunnel_terminator_s {
    uint32 flags;           /* Configuration flags. Use BCM_TUNNEL flags */
    uint32 multicast_flag;  /* VXLAN Multicast trigger flags. */
    bcm_vrf_t vrf;          /* Virtual router instance. */
    bcm_ip_t sip;           /* SIP for tunnel header match. */
    bcm_ip_t dip;           /* DIP for tunnel header match. */
    bcm_ip_t sip_mask;      /* Source IP mask. */
    bcm_ip_t dip_mask;      /* Destination IP mask. */
    bcm_ip6_t sip6;         /* SIP for tunnel header match (IPv6). */
    bcm_ip6_t dip6;         /* DIP for tunnel header match (IPv6). */
    bcm_ip6_t sip6_mask;    /* Source IP mask (IPv6). */
    bcm_ip6_t dip6_mask;    /* Destination IP mask (IPv6). */
    uint32 udp_dst_port;    /* UDP dst port for UDP packets. */
    uint32 udp_src_port;    /* UDP src port for UDP packets */
    bcm_tunnel_type_t type; /* Tunnel type */
    bcm_vlan_t vlan;        /* The VLAN ID for IPMC lookup */
    uint32 protocol;        /* Protocol type */
    uint32 valid_elements;  /* bitmap of valid fields */
    uint32 flow_handle;     /* flow handle */
    uint32 flow_option;     /* flow option */
    bcm_vlan_t vlan_mask;   /* The VLAN ID mask. */
    uint32 class_id;        /* Class ID as a key in VFP. */
    bcm_pbmp_t term_pbmp;   /* Allowed port bitmap for tunnel termination. */
} bcm_compat6523_flow_tunnel_terminator_t;

/* Create a flow tunnel terminator match. */
extern int bcm_compat6523_flow_tunnel_terminator_create(
    int unit,
    bcm_compat6523_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Delete a tunnel termination entry. */
extern int bcm_compat6523_flow_tunnel_terminator_destroy(
    int unit,
    bcm_compat6523_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Get a tunnel termination entry. */
extern int bcm_compat6523_flow_tunnel_terminator_get(
    int unit,
    bcm_compat6523_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcm_compat6523_flow_tunnel_terminator_flexctr_object_set(
    int unit,
    bcm_compat6523_flow_tunnel_terminator_t *info,
    uint32 value);

extern int bcm_compat6523_flow_tunnel_terminator_flexctr_object_get(
    int unit,
    bcm_compat6523_flow_tunnel_terminator_t *info,
    uint32 *value);

extern int bcm_compat6523_flow_tunnel_terminator_stat_attach(
    int unit,
    bcm_compat6523_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 stat_counter_id);

extern int bcm_compat6523_flow_tunnel_terminator_stat_detach(
    int unit,
    bcm_compat6523_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcm_compat6523_flow_tunnel_terminator_stat_id_get(
    int unit,
    bcm_compat6523_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 *stat_counter_id);

#endif /* defined(INCLUDE_L3) */

/* Packet integrity match attribute structure. */
typedef struct bcm_compat6523_switch_pkt_integrity_match_s {
    uint8 tunnel_processing_results_1;  /* Tunnel processing results 1. */
    uint8 tunnel_processing_results_1_mask; /* Tunnel processing results 1 mask. */
    uint8 mpls_flow_type;               /* Mpls flow type. */
    uint8 mpls_flow_type_mask;          /* Mpls flow type mask. */
    uint8 my_station_1_hit;             /* My station 1 hit. */
    uint8 my_station_1_hit_mask;        /* My station 1 hit mask. */
    uint8 l4_valid;                     /* L4 valid. */
    uint8 l4_valid_mask;                /* L4 valid mask. */
    uint8 icmp_type;                    /* Icmp type. */
    uint8 icmp_type_mask;               /* Icmp type mask. */
    uint8 ip_last_protocol;             /* Ip last protocol. */
    uint8 ip_last_protocol_mask;        /* Ip last protocol mask. */
    uint16 tcp_hdr_len_and_flags;       /* Tcp hdr len and flags. */
    uint16 tcp_hdr_len_and_flags_mask;  /* Tcp hdr len and flags mask. */
    uint8 fixed_hve_result_0;           /* Fixed hve result 0. */
    uint8 fixed_hve_result_0_mask;      /* Fixed hve result 0 mask. */
    uint8 fixed_hve_result_1;           /* Fixed hve result 1. */
    uint8 fixed_hve_result_1_mask;      /* Fixed hve result 1 mask. */
    uint8 fixed_hve_result_2;           /* Fixed hve result 2. */
    uint8 fixed_hve_result_2_mask;      /* Fixed hve result 2 mask. */
    uint8 fixed_hve_result_3;           /* Fixed hve result 3. */
    uint8 fixed_hve_result_3_mask;      /* Fixed hve result 3 mask. */
    uint8 fixed_hve_result_4;           /* Fixed hve result 4. */
    uint8 fixed_hve_result_4_mask;      /* Fixed hve result 4 mask. */
    uint8 fixed_hve_result_5;           /* Fixed hve result 5. */
    uint8 fixed_hve_result_5_mask;      /* Fixed hve result 5 mask. */
    uint8 flex_hve_result_0;            /* Flex hve result 0. */
    uint8 flex_hve_result_0_mask;       /* Flex hve result 0 mask. */
    uint8 flex_hve_result_1;            /* Flex hve result 1. */
    uint8 flex_hve_result_1_mask;       /* Flex hve result 1 mask. */
    uint8 flex_hve_result_2;            /* Flex hve result 2. */
    uint8 flex_hve_result_2_mask;       /* Flex hve result 2 mask. */
} bcm_compat6523_switch_pkt_integrity_match_t;

/* Add an entry into packet integrity check table. */
extern int bcm_compat6523_switch_pkt_integrity_check_add(
    int unit,
    uint32 options,
    bcm_compat6523_switch_pkt_integrity_match_t *match,
    bcm_switch_pkt_control_action_t *action,
    int priority);

/* Delete an entry from packet integrity check table. */
extern int bcm_compat6523_switch_pkt_integrity_check_delete(
    int unit,
    bcm_compat6523_switch_pkt_integrity_match_t *match);

/* Get an entry from packet integrity check table. */
extern int bcm_compat6523_switch_pkt_integrity_check_get(
    int unit,
    bcm_compat6523_switch_pkt_integrity_match_t *match,
    bcm_switch_pkt_control_action_t *action,
    int *priority);

#if defined(INCLUDE_L3)
/* L3 ECMP structure */
typedef struct bcm_compat6523_l3_egress_ecmp_s {
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
} bcm_compat6523_l3_egress_ecmp_t;

/* Resilient ecmp entry */
typedef struct bcm_compat6523_l3_egress_ecmp_resilient_entry_s {
    uint64 hash_key;            /* Hash key. */
    bcm_compat6523_l3_egress_ecmp_t ecmp;  /* ECMP. */
    bcm_if_t intf;              /* L3 interface. */
} bcm_compat6523_l3_egress_ecmp_resilient_entry_t;

/* Create an Egress ECMP forwarding object. */
extern int bcm_compat6523_l3_ecmp_create(
    int unit,
    uint32 options,
    bcm_compat6523_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_count,
    bcm_l3_ecmp_member_t *ecmp_member_array);

/* Get info about an Egress ECMP forwarding object. */
extern int bcm_compat6523_l3_ecmp_get(
    int unit,
    bcm_compat6523_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_size,
    bcm_l3_ecmp_member_t *ecmp_member_array,
    int *ecmp_member_count);

/* Find an Egress ECMP forwarding object. */
extern int bcm_compat6523_l3_ecmp_find(
    int unit,
    int ecmp_member_count,
    bcm_l3_ecmp_member_t *ecmp_member_array,
    bcm_compat6523_l3_egress_ecmp_t *ecmp_info);

/* Create an Egress ECMP forwarding object. */
extern int bcm_compat6523_l3_egress_ecmp_create(
    int unit,
    bcm_compat6523_l3_egress_ecmp_t *ecmp,
    int intf_count,
    bcm_if_t *intf_array);

/* Destroy an Egress ECMP forwarding object. */
extern int bcm_compat6523_l3_egress_ecmp_destroy(
    int unit,
    bcm_compat6523_l3_egress_ecmp_t *ecmp);

/* Get info about an Egress ECMP forwarding object. */
extern int bcm_compat6523_l3_egress_ecmp_get(
    int unit,
    bcm_compat6523_l3_egress_ecmp_t *ecmp,
    int intf_size,
    bcm_if_t *intf_array,
    int *intf_count);

/* Add an Egress forwarding object to an Egress ECMP forwarding object. */
extern int bcm_compat6523_l3_egress_ecmp_add(
    int unit,
    bcm_compat6523_l3_egress_ecmp_t *ecmp,
    bcm_if_t intf);

/*
 * Delete an Egress forwarding object from an Egress ECMP forwarding
 * object.
 */
extern int bcm_compat6523_l3_egress_ecmp_delete(
    int unit,
    bcm_compat6523_l3_egress_ecmp_t *ecmp,
    bcm_if_t intf);

/* Find an Egress ECMP forwarding object. */
extern int bcm_compat6523_l3_egress_ecmp_find(
    int unit,
    int intf_count,
    bcm_if_t *intf_array,
    bcm_compat6523_l3_egress_ecmp_t *ecmp);

/*
 * Update an ECMP table that was already allocated, used in cases where
 * an ECMP have more than one table.
 */
extern int bcm_compat6523_l3_egress_ecmp_tunnel_priority_set(
    int unit,
    bcm_compat6523_l3_egress_ecmp_t *ecmp,
    int intf_count,
    bcm_if_t *intf_array);

/* Replace ECMP resilient entries matching given criteria. */
extern int bcm_compat6523_l3_egress_ecmp_resilient_replace(
    int unit,
    uint32 flags,
    bcm_compat6523_l3_egress_ecmp_resilient_entry_t *match_entry,
    int *num_entries,
    bcm_compat6523_l3_egress_ecmp_resilient_entry_t *replace_entry);

/* Add ECMP resilient entries matching given criteria. */
extern int bcm_compat6523_l3_egress_ecmp_resilient_add(
    int unit,
    uint32 flags,
    bcm_compat6523_l3_egress_ecmp_resilient_entry_t *entry);

/* Delete ECMP resilient entries matching given criteria. */
extern int bcm_compat6523_l3_egress_ecmp_resilient_delete(
    int unit,
    uint32 flags,
    bcm_compat6523_l3_egress_ecmp_resilient_entry_t *entry);

/*
 * L3 Information Structure.
 *
 * Contains information about L3 hardware capacity and utilization.
 *
 * Note that in chips that implement the longest prefix match route table
 * as a trie, one IP route may consume up to 128 table entries in the
 * worst case.
 */
typedef struct bcm_compat6523_l3_info_s {
    int l3info_max_vrf;                 /* Maximum number of virtual routers
                                           allowed. */
    int l3info_used_vrf;                /* Number of virtual routers created so
                                           far. */
    int l3info_max_intf;                /* Maximum L3 interfaces the chip
                                           supports. */
    int l3info_max_intf_group;          /* Maximum L3 interface groups the chip
                                           supports. */
    int l3info_max_host;                /* L3 host table size(unit is IPv4
                                           unicast). */
    int l3info_max_route;               /* L3 route table size (unit is IPv4
                                           route). */
    int l3info_max_ecmp_groups;         /* Maximum ECMP groups allowed. */
    int l3info_max_ecmp;                /* Maximum ECMP paths allowed. */
    int l3info_used_intf;               /* L3 interfaces used. */
    int l3info_used_host;               /* L3 host entries used. */
    int l3info_used_route;              /* L3 route entries used. */
    int l3info_max_lpm_block;           /* Maximum LPM blocks. */
    int l3info_used_lpm_block;          /* LPM blocks used. */
    int l3info_max_l3;                  /* Superseded by l3info_max_host. */
    int l3info_max_defip;               /* Superseded by l3info_max_route. */
    int l3info_used_l3;                 /* Superseded by l3info_occupied_host. */
    int l3info_used_defip;              /* Superseded by l3info_occupied_route. */
    int l3info_max_nexthop;             /* Maximum NextHops. */
    int l3info_used_nexthop;            /* NextHops used. */
    int l3info_max_tunnel_init;         /* Maximum IPV4 tunnels that can be
                                           initiated. */
    int l3info_used_tunnel_init;        /* Number of active IPV4 tunnels
                                           initiated. */
    int l3info_max_tunnel_term;         /* Maximum IPV4 tunnels that can be
                                           terminated. */
    int l3info_used_tunnel_term;        /* Number of active IPV4 tunnels
                                           terminated. */
    int l3info_used_host_ip4;           /* L3 host entries used by IPv4. */
    int l3info_used_host_ip6;           /* L3 host entries used by IPv6. */
} bcm_compat6523_l3_info_t;

/* Get the status of hardware tables. */
extern int bcm_compat6523_l3_info(
    int unit,
    bcm_compat6523_l3_info_t *l3info);
#endif /* INCLUDE_L3 */

/* Switch drop event monitor structure. */
typedef struct bcm_compat6523_switch_drop_event_mon_s {
    bcm_pkt_drop_event_t drop_event;    /* Drop event. */
    uint32 actions;                     /* BCM_SWITCH_MON_ACTION_XXX actions
                                           definitions. */
} bcm_compat6523_switch_drop_event_mon_t;

/*
 * Set monitor configuration for drop events defined in
 * <bcm/types.h>.
 */
extern int bcm_compat6523_switch_drop_event_mon_set(
    int unit,
    bcm_compat6523_switch_drop_event_mon_t *monitor);

/*
 * Get monitor configuration for drop events defined in
 * <bcm/types.h>.
 */
extern int bcm_compat6523_switch_drop_event_mon_get(
    int unit,
    bcm_compat6523_switch_drop_event_mon_t *monitor);

#if defined(INCLUDE_L3)
/* L3 tunneling initiator. */
typedef struct bcm_compat6523_tunnel_initiator_s {
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
    bcm_mac_t smac;    /* WLAN outer MAC */
    int mtu;    /* WLAN MTU */
    bcm_vlan_t vlan;    /* Tunnel VLAN */
    uint16 tpid;    /* Tunnel TPID */
    uint8 pkt_pri;    /* Tunnel priority */
    uint8 pkt_cfi;    /* Tunnel CFI */
    uint16 ip4_id;    /* IPv4 ID. */
    bcm_if_t l3_intf_id;    /* l3 Interface ID. */
    uint16 span_id;    /* Erspan Span ID. */
    uint32 aux_data;    /* Tunnel associated data. */
    int outlif_counting_profile;    /* Out LIF counting profile */
    bcm_encap_access_t encap_access;    /* Encapsulation Access stage */
    uint8 hw_id;    /* Unique identifier of an ERSPAN engine within a system */
    uint16 switch_id;    /* Identifies a source switch at the receiving end */
    uint16 class_id;    /* Class ID of the RSPAN advanced tunnel */
    bcm_qos_egress_model_t egress_qos_model;    /* egress qos and ttl model */
    int qos_map_id;    /* general remark profile */
    uint8 ecn;    /* Tunnel header ECN value. */
    bcm_tunnel_flow_label_select_t flow_label_sel;    /* Tunnel header flow label select. */
    bcm_tunnel_dscp_ecn_select_t dscp_ecn_sel;    /* Tunnel header DSCP and ECN select. */
    int dscp_ecn_map;    /* DSCP and ECN map ID. */
    int stat_cmd;    /* Counter ID associated with the the tunnel. */
    uint8 counter_command_id;    /* Counter interface associated with the tunnel. */
    int estimated_encap_size;    /* Esitimated encapsulation size expected by this object including the next interfaces that are pointed from it. */
} bcm_compat6523_tunnel_initiator_t;

/* Set the tunneling initiator parameters on an L3 interface. */
extern int bcm_compat6523_tunnel_initiator_set(
    int unit,
    bcm_l3_intf_t *intf,
    bcm_compat6523_tunnel_initiator_t *tunnel);

/* Set the tunneling initiator parameters on an L3 interface. */
extern int bcm_compat6523_tunnel_initiator_create(
    int unit,
    bcm_l3_intf_t *intf,
    bcm_compat6523_tunnel_initiator_t *tunnel);

/* Get the tunnel property for the given L3 interface. */
extern int bcm_compat6523_tunnel_initiator_get(
    int unit,
    bcm_l3_intf_t *intf,
    bcm_compat6523_tunnel_initiator_t *tunnel);

/* Create VXLAN Tunnel Initiator.  */
extern int bcm_compat6523_vxlan_tunnel_initiator_create(
    int unit,
    bcm_compat6523_tunnel_initiator_t *info);

/* Get  VXLAN Tunnel Initiator.  */
extern int bcm_compat6523_vxlan_tunnel_initiator_get(
    int unit,
    bcm_compat6523_tunnel_initiator_t *info);

/* Set the tunneling initiator parameters for a WLAN tunnel. */
extern int bcm_compat6523_wlan_tunnel_initiator_create(
    int unit,
    bcm_compat6523_tunnel_initiator_t *info);

/* Get the tunnel properties for an outgoing WLAN tunnel. */
extern int bcm_compat6523_wlan_tunnel_initiator_get(
    int unit,
    bcm_compat6523_tunnel_initiator_t *info);

/* Create L2GRE Tunnel Initiator.  */
extern int bcm_compat6523_l2gre_tunnel_initiator_create(
    int unit,
    bcm_compat6523_tunnel_initiator_t *info);

/* Get  L2GRE Tunnel Initiator.  */
extern int bcm_compat6523_l2gre_tunnel_initiator_get(
    int unit,
    bcm_compat6523_tunnel_initiator_t *info);


#endif /* defined(INCLUDE_L3) */

/* Port Configuration structure. */
typedef struct bcm_compat6523_port_config_s {
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
} bcm_compat6523_port_config_t;

/* Retrieved the port configuration for the specified device. */
extern int bcm_compat6523_port_config_get(
    int unit,
    bcm_compat6523_port_config_t *config);

/* Five tuple can be outer/inner SRC IP, DST IP, L4 SRC PORT, L4 DST PORT and IP PROTOCOL that constitutes a flow. */
typedef struct bcm_compat6523_flowtracker_flow_key_s {
    bcm_ip_addr_t src_ip;    /*  */
    bcm_ip_addr_t dst_ip;    /*  */
    bcm_l4_port_t l4_src_port;    /*  */
    bcm_l4_port_t l4_dst_port;    /*  */
    uint8 ip_protocol;    /*  */
    bcm_ip_addr_t inner_src_ip;    /*  */
    bcm_ip_addr_t inner_dst_ip;    /*  */
    bcm_l4_port_t inner_l4_src_port;    /*  */
    bcm_l4_port_t inner_l4_dst_port;    /*  */
    uint8 inner_ip_protocol;    /*  */
    uint32 vxlan_network_id;    /*  */
    uint8 custom[BCM_FLOWTRACKER_CUSTOM_KEY_MAX_LENGTH];    /*  */
    bcm_port_t in_port;    /*  */
} bcm_compat6523_flowtracker_flow_key_t;

/* Given a flow key, get the associated data */
extern int bcm_compat6523_flowtracker_group_data_get(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_compat6523_flowtracker_flow_key_t *flow_key,
    bcm_flowtracker_flow_data_t *flow_data);

/* Add a new static flow to the flow group. */
extern int bcm_compat6523_flowtracker_static_flow_add(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_compat6523_flowtracker_flow_key_t *flow_key);

/* Delete an existing static flow from the flow group. */
extern int bcm_compat6523_flowtracker_static_flow_delete(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_compat6523_flowtracker_flow_key_t *flow_key);

/* Get all static flows installed for a given flow group. */
extern int bcm_compat6523_flowtracker_static_flow_get_all(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    int max_size,
    bcm_compat6523_flowtracker_flow_key_t *flow_key_arr,
    int *list_size);

/* OAM loopback object. */
typedef struct bcm_compat6523_oam_loopback_s {
    uint32 flags; 
    bcm_oam_endpoint_t id;              /* Endpoint ID of Local MEP. */
    bcm_oam_endpoint_t remote_id;       /* Endpoint ID of Remote MEP. */
    int period;                         /* Frequency of loopback tx period in
                                           milliseconds or pps (packets/sec)
                                           depending on flags. Zero indicates
                                           single shot, -1 indicates disabling
                                           loopback packets transmission */
    int ttl;                            /* TTL value to use in loopback message. */
    bcm_oam_loopback_discovery_record_t discovered_me; /* Last ME discovered using loopback
                                           discovery. */
    uint32 rx_count;                    /* Count of received packets since
                                           loopback add or last get. */
    uint32 tx_count;                    /* Count of transmitted packets since
                                           loopback add or last get. */
    uint32 drop_count;                  /* Count of dropped packets since
                                           loopback add or last get. */
    uint32 unexpected_response;         /* Count of unexpected response packets
                                           since loopback add or last get. */
    uint32 out_of_sequence;             /* Count of out of sequence packets. */
    uint32 local_mipid_missmatch;       /* Count of local mipid missmatch
                                           packets since loopback add or last
                                           get. */
    uint32 remote_mipid_missmatch;      /* Count of remote mipid missmatch
                                           packets since loopback add or last
                                           get. */
    uint32 invalid_target_mep_tlv;      /* Count of invalid target mep TLV
                                           packets since loopback add or last
                                           get. */
    uint32 invalid_mep_tlv_subtype;     /* Count of invalid mep TLV subtype
                                           packets since loopback add or last
                                           get. */
    uint32 invalid_tlv_offset;          /* Count of invalid TLV offset packets
                                           since loopback add or last get. */
    bcm_mac_t peer_da_mac_address;      /* MAC DA in loopback injection in case
                                           remote_id is not specified */
    int num_tlvs;                       /* Number of TLV */
    int invalid_tlv_count;              /* Count of invalid packets received
                                           (incorrect TLV) */
    int ctf_id;                         /* Ctf id number */
    int gtf_id;                         /* Gtf id number */
    bcm_oam_tlv_t tlvs[BCM_OAM_MAX_NUM_TLVS_FOR_LBM]; /* TLV contents of LBM */
    uint32 lb_tx_update_lm_counter_base_id[BCM_OAM_LM_COUNTER_MAX]; /* The base index of the LM counter to
                                           be updated by Tx LB packets */
    uint32 lb_tx_update_lm_counter_offset[BCM_OAM_LM_COUNTER_MAX]; /* Offset to the Base LM counter Id to
                                           be incremented by Tx LB packets */
    uint8 lb_tx_update_lm_counter_size; /* Number of LM counters to be
                                           incremented by Tx LB packets */
    bcm_cos_t int_pri;                  /* Optional: priority fields on ITMH
                                           header */
    uint8 pkt_pri;                      /* Optional: priority fields on outer
                                           VLAN (PCP + DEI). -1 will ignore this
                                           field */
    uint8 inner_pkt_pri;                /* Optional: priority fields on inner
                                           VLAN (PCP + DEI) , when applicable.
                                           -1 will ignore this field */
} bcm_compat6523_oam_loopback_t;

/* Add an OAM loopback object */
extern int bcm_compat6523_oam_loopback_add(
    int unit, 
    bcm_compat6523_oam_loopback_t *loopback_ptr);

/* Get an OAM loopback object */
extern int bcm_compat6523_oam_loopback_get(
    int unit, 
    bcm_compat6523_oam_loopback_t *loopback_ptr);

/* Delete an OAM loopback object */
extern int bcm_compat6523_oam_loopback_delete(
    int unit, 
    bcm_compat6523_oam_loopback_t *loopback_ptr);

/* OAM TST TX object. */
typedef struct bcm_compat6523_oam_tst_tx_s {
    bcm_oam_endpoint_t endpoint_id;     /* Endpoint ID of Local MEP. */
    int gtf_id; 
    uint32 flags; 
    bcm_mac_t peer_da_mac_address;      /* MAC DA in TST injection. */
    bcm_oam_tlv_t tlv; 
} bcm_compat6523_oam_tst_tx_t;

/* Add an OAM TST tx object */
extern int bcm_compat6523_oam_tst_tx_add(
    int unit, 
    bcm_compat6523_oam_tst_tx_t *tst_ptr);

/* Get an OAM TST tx object */
extern int bcm_compat6523_oam_tst_tx_get(
    int unit, 
    bcm_compat6523_oam_tst_tx_t *tst_ptr);

/* Delete an OAM TST tx object */
extern int bcm_compat6523_oam_tst_tx_delete(
    int unit, 
    bcm_compat6523_oam_tst_tx_t *tst_ptr);

/*
 * OAM group object.
 */
typedef struct bcm_compat6523_oam_group_info_s {
    uint32 flags;
    bcm_oam_group_t id;
    uint8 name[BCM_OAM_GROUP_NAME_LENGTH]; /* This is equivalent to the Maintenance
                                           Association ID (MAID) in 802.1ag.
                                           Note that this is not a
                                           null-terminated string but an array
                                           of bytes.  To avoid memory
                                           corruption, do not use string copy to
                                           populate this field. */
    uint8 rx_name[BCM_OAM_GROUP_NAME_LENGTH]; /* Additional MAID used for received OAM
                                           packets. It is used when the flag
                                           BCM_OAM_GROUP_RX_NAME is set.  When
                                           used, the received MAID values are
                                           compared to this MAID instead of the
                                           'name' field. */
    uint32 faults;                      /* Fault flags */
    uint32 persistent_faults;           /* Persistent fault flags */
    uint32 clear_persistent_faults;     /* Persistent fault flags to clear on a
                                           'get' */
    bcm_oam_group_fault_alarm_defect_priority_t lowest_alarm_priority; /* Generate fault alarm for this
                                           maintenance endpoint when defects of
                                           greater than or equal to this
                                           priority value are detected on this
                                           maintenance endpoint */
    int group_name_index;               /* Pointer to first extended data entry */
} bcm_compat6523_oam_group_info_t;

/* Create or replace an OAM group object. */
extern int bcm_compat6523_oam_group_create(
    int unit,
    bcm_compat6523_oam_group_info_t *group_info);

/* Get an OAM group object. */
extern int bcm_compat6523_oam_group_get(
    int unit,
    bcm_oam_group_t group,
    bcm_compat6523_oam_group_info_t *group_info);

/*
 * OAM endpoint object.
 */
typedef struct bcm_compat6523_oam_endpoint_info_s {
    uint32 flags;
    uint32 flags2;
    uint32 opcode_flags;                /* OAM opcode flags */
    uint32 lm_flags;                    /* OAM loss measurment flags */
    bcm_oam_endpoint_t id;
    bcm_oam_endpoint_type_t type;
    bcm_oam_group_t group;              /* The ID of the group to which this
                                           endpoint belongs */
    uint16 name;                        /* A unique identifier of the endpoint
                                           within the group.  This is equivalent
                                           to the MEPID in 802.1ag. */
    bcm_oam_endpoint_t local_id;        /* Used by remote endpoints only. */
    int level;                          /* This is equivalent to the Maintenance
                                           Domain Level (MDL) in 802.1ag. */
    int ccm_period;                     /* For local endpoints, this is the CCM
                                           transmission period in ms.  For
                                           remote endpoints, this is the period
                                           on which CCM timeout is based. */
    bcm_vlan_t vlan;                    /* The VLAN associated with this
                                           endpoint */
    bcm_vlan_t inner_vlan;              /* Configure with CVID for two-tag
                                           operation or set to 0 for one-tag */
    bcm_gport_t gport;                  /* The gport associated with this
                                           endpoint */
    bcm_gport_t tx_gport;               /* TX gport associated with this
                                           endpoint */
    int trunk_index;                    /* The trunk port index for this
                                           endpoint */
    bcm_if_t intf_id;                   /* Interface Identifier */
    bcm_mpls_label_t mpls_label;        /* The MPLS label associated with
                                           packets received from the Peer
                                           Endpoint */
    bcm_mpls_egress_label_t egress_label; /* The MPLS outgoing label information
                                           for the Local Endpoint */
    bcm_oam_mpls_network_info_t mpls_network_info; /* MPLS networks information */
    bcm_mac_t dst_mac_address;          /* The destination MAC address
                                           associated with this endpoint */
    bcm_mac_t src_mac_address;          /* The source MAC address associated
                                           with this endpoint */
    uint8 pkt_pri;                      /* Egress marking for outgoing CCMs */
    uint8 inner_pkt_pri;                /* Packet priority of inner encpsulated
                                           packet */
    uint16 inner_tpid;                  /* Tpid of inner encpsulated packet */
    uint16 outer_tpid;                  /* Tpid of outer encpsulated packet */
    bcm_cos_t int_pri;                  /* Egress queuing for outgoing CCMs. For
                                           DownMEPs, this represents the index
                                           from the base queue. For UpMEPs, this
                                           represents the internal priority
                                           itself. */
    uint8 cpu_qid;                      /* CPU queue for CCMs */
    uint8 pri_map[BCM_OAM_INTPRI_MAX];  /* Priority mapping for LM counter table */
    uint32 faults;                      /* Fault flags */
    uint32 persistent_faults;           /* Persistent fault flags */
    uint32 clear_persistent_faults;     /* Persistent fault flags to clear on a
                                           'get' */
    int ing_map;                        /* Ingress QoS map profile */
    int egr_map;                        /* Egress QoS map profile */
    uint8 ms_pw_ttl;                    /* TTL used for multi-segment pseudowire */
    uint8 port_state;                   /* Endpoint port status */
    uint8 interface_state;              /* Endpoint interface status */
    bcm_oam_vccv_type_t vccv_type;      /* VCCV pseudowire type */
    bcm_vpn_t vpn;                      /* VPN id */
    int lm_counter_base_id;             /* Counter id associated with the mep */
    uint8 lm_counter_if;                /* Counter interface associated with the
                                           mep */
    uint8 loc_clear_threshold;          /* Number of packets required to reset
                                           the Loss-of-Continuity status per end
                                           point */
    bcm_oam_timestamp_format_t timestamp_format; /* DM time stamp format -
                                           NTP/IEEE1588(PTP) */
    bcm_oam_tlv_t tlvs[BCM_OAM_MAX_NUM_TLVS_FOR_DM]; /* TLV contents of DM */
    uint16 subport_tpid;                /* Tpid of subport Vlan in case of
                                           triple vlan encapsulation */
    bcm_gport_t remote_gport;           /* Local endpoint CCMs are forwarded to
                                           this remote unit for processing. */
    bcm_gport_t mpls_out_gport;         /* Out gport used for TX counting by BHH
                                           endpoints. */
    int sampling_ratio;                 /* 0 - No packets sampled to the CPU.
                                           1-8 - Count of packets (with events)
                                           that need to arrive before one is
                                           sampled to the CPU. */
    uint32 lm_payload_offset;           /* Offset in bytes from TOP of the MPLS
                                           label stack from where the payload
                                           starts for byte count computation. */
    uint32 lm_cos_offset;               /* Offset of the Label from top of the
                                           stack which gives the EXP value for
                                           deriving the COS value - valid values
                                           are 0/1/2. */
    bcm_oam_lm_counter_type_t lm_ctr_type; /* BYTE/PACKET. */
    bcm_oam_lm_counter_size_t lm_ctr_sample_size; /* Valid values are 32/64. */
    uint32 pri_map_id;                  /* OAM priority map id. */
    int lm_ctr_pool_id;                 /* The loss measurement counter pool id
                                           from which the counters should be
                                           allocated. Valid values are 0/1/2. */
    uint32 ccm_tx_update_lm_counter_base_id[BCM_OAM_LM_COUNTER_MAX]; /* The base index of the LM counter to
                                           be updated by Tx CCM packets */
    uint32 ccm_tx_update_lm_counter_offset[BCM_OAM_LM_COUNTER_MAX]; /* Offset to the Base LM counter Id to
                                           be incremented by Tx CCM packets */
    uint8 ccm_tx_update_lm_counter_action[BCM_OAM_LM_COUNTER_MAX]; /* LM Counter action (type
                                           bcm_oam_lm_counter_action_t) to be
                                           used by TX CCM packets */
    uint8 ccm_tx_update_lm_counter_size; /* Number of LM counters to be
                                           incremented by Tx CCM packets */
    uint32 session_id;                  /* OAM session id for packet processing
                                           in BTE. In FP based OAM - This will
                                           indicate flex counter base ID */
    uint8 session_num_entries;          /* Number of entries that can result in
                                           same session ID. In FP based OAM -
                                           This will indicate number of flex
                                           counter entries corresponding to same
                                           OAM session */
    uint8 lm_count_profile;             /* LM count profile for this endpoint.
                                           It will be 1 or 0 for selecting one
                                           of the two OAM LM count profiles. */
    int measurement_period;             /* SLR measurement period in
                                           milliseconds. */
    uint32 test_id;                     /* Test ID to differentiate each SLM
                                           session when there are multiple SLM
                                           sessions running between a pair of
                                           MEPs. */
    uint8 mpls_exp;                     /* EXP on which BHH will be running */
    bcm_oam_endpoint_t action_reference_id; /* Reference endpoint id whose actions
                                           will be used on the new created
                                           endpoint. Default value:
                                           BCM_OAM_ENDPOINT_INVALID. API call
                                           will set the parameter to
                                           BCM_OAM_ENDPOINT_INVALID */
    bcm_oam_profile_t acc_profile_id;   /* Used by accelerated endpoints. */
    bcm_oam_endpoint_memory_type_t endpoint_memory_type; /* Type of default endpoint memory */
    int punt_good_packet_period;        /* OAM good packets sampling period.
                                           Every punt_good_packet_period
                                           milliseconds, a single packet is
                                           punted to the CPU */
    uint16 extra_data_index;            /* Pointer to first extra data entry
                                           used per MEP for additional features */
    bcm_oam_endpoint_signal_t rx_signal; /* Expected Signal Indication */
    bcm_oam_endpoint_signal_t tx_signal; /* Transmitted Signal Indication */
} bcm_compat6523_oam_endpoint_info_t;

/* Create or replace an OAM endpoint object. */
extern int bcm_compat6523_oam_endpoint_create(
    int unit,
    bcm_compat6523_oam_endpoint_info_t *endpoint_info);

/* Get an OAM endpoint object. */
extern int bcm_compat6523_oam_endpoint_get(
    int unit,
    bcm_oam_endpoint_t endpoint,
    bcm_compat6523_oam_endpoint_info_t *endpoint_info);

/* Generic vlan match attribute structure */
typedef struct bcm_compat6523_vlan_match_info_s {
    bcm_vlan_match_t match;    /* Match criteria */
    bcm_gport_t port;    /* Match port */
    bcm_mac_t src_mac;    /* Match source mac address */
    uint32 port_class;    /* Match port class */
} bcm_compat6523_vlan_match_info_t;

/* Add action for match criteria for Vlan assignment. */
extern int bcm_compat6523_vlan_match_action_add(
    int unit,
    uint32 options,
    bcm_compat6523_vlan_match_info_t *match_info,
    bcm_vlan_action_set_t *action_set);

/* Get action for match criteria. */
extern int bcm_compat6523_vlan_match_action_get(
    int unit,
    bcm_compat6523_vlan_match_info_t *match_info,
    bcm_vlan_action_set_t *action_set);

/* Get all actions and match criteria for given vlan match. */
extern int bcm_compat6523_vlan_match_action_multi_get(
    int unit,
    bcm_vlan_match_t match,
    int size,
    bcm_compat6523_vlan_match_info_t *match_info_array,
    bcm_vlan_action_set_t *action_set_array,
    int *count);

/* Remove action for a match criteria */
extern int bcm_compat6523_vlan_match_action_delete(
    int unit,
    bcm_compat6523_vlan_match_info_t *match_info);


#if defined(INCLUDE_L3)
/* L2GRE Gport  */
typedef struct bcm_compat6523_l2gre_port_s {
    bcm_gport_t l2gre_port_id;    /* GPORT identifier. */
    uint32 flags;    /* BCM_L2GRE_PORT_xxx. */
    uint32 if_class;    /* Interface class ID. */
    uint16 int_pri;    /* Internal Priority */
    uint8 pkt_pri;    /* Packet Priority */
    uint8 pkt_cfi;    /* Packet CFI */
    uint16 egress_service_tpid;    /* Service Vlan TPID Value */
    bcm_vlan_t egress_service_vlan;    /* SD-TAG Vlan ID. */
    uint16 mtu;    /* MTU */
    bcm_gport_t match_port;    /* Match port / trunk */
    bcm_l2gre_port_match_t criteria;    /* Match criteria */
    bcm_vlan_t match_vlan;    /* Outer VLAN ID to match. */
    bcm_vlan_t match_inner_vlan;    /* Inner VLAN ID to match. */
    bcm_gport_t egress_tunnel_id;    /* Tunnel Initiator ID */
    bcm_gport_t match_tunnel_id;    /* Tunnel Terminator ID */
    bcm_if_t egress_if;    /* L2GRE egress object. */
    bcm_switch_network_group_t network_group_id;    /* Split Horizon network group identifier. */
} bcm_compat6523_l2gre_port_t;

/* bcm_l2gre_port_add adds a Access or Network L2GRE port */
extern int bcm_compat6523_l2gre_port_add(
    int unit,
    bcm_vpn_t l2vpn,
    bcm_compat6523_l2gre_port_t *l2gre_port);

/* Get L2GRE port information. */
extern int bcm_compat6523_l2gre_port_get(
    int unit,
    bcm_vpn_t l2vpn,
    bcm_compat6523_l2gre_port_t *l2gre_port);

/* Get all L2GRE port information. */
extern int bcm_compat6523_l2gre_port_get_all(
    int unit,
    bcm_vpn_t l2vpn,
    int port_max,
    bcm_compat6523_l2gre_port_t *port_array,
    int *port_count);

/* VXLAN Gport  */
typedef struct bcm_compat6523_vxlan_port_s {
    bcm_gport_t vxlan_port_id;    /* GPORT identifier. */
    uint32 flags;    /* BCM_VXLAN_PORT_xxx. */
    uint32 if_class;    /* Interface class ID. */
    uint16 int_pri;    /* Internal Priority */
    uint8 pkt_pri;    /* Packet Priority */
    uint8 pkt_cfi;    /* Packet CFI */
    uint16 egress_service_tpid;    /* Service Vlan TPID Value */
    bcm_vlan_t egress_service_vlan;    /* SD-TAG Vlan ID. */
    uint16 mtu;    /* MTU */
    bcm_gport_t match_port;    /* Match port / trunk */
    bcm_vxlan_port_match_t criteria;    /* Match criteria */
    bcm_vlan_t match_vlan;    /* Outer VLAN ID to match. */
    bcm_vlan_t match_inner_vlan;    /* Inner VLAN ID to match. */
    bcm_gport_t egress_tunnel_id;    /* Tunnel Initiator ID */
    bcm_gport_t match_tunnel_id;    /* Tunnel Terminator ID */
    bcm_if_t egress_if;    /* VXLAN egress object. */
    bcm_switch_network_group_t network_group_id;    /* Split Horizon network group identifier. */
    uint32 vnid;    /* VXLAN packet VNID. */
    int qos_map_id;    /* QoS Mapped priority */
    uint8 tunnel_pkt_pri;    /* Tunnel Packet Priority */
    uint8 tunnel_pkt_cfi;    /* Tunnel Packet CFI */
    uint32 flags2;    /* BCM_VXLAN_PORT_FLAGS2_xxx. */
    bcm_vpn_t default_vpn;    /* default vxlan vpn */
} bcm_compat6523_vxlan_port_t;

/* bcm_vxlan_port_add adds a Access or Network VXLAN port */
extern int bcm_compat6523_vxlan_port_add(
    int unit,
    bcm_vpn_t l2vpn,
    bcm_compat6523_vxlan_port_t *vxlan_port);

/* Get VXLAN port information. */
extern int bcm_compat6523_vxlan_port_get(
    int unit,
    bcm_vpn_t l2vpn,
    bcm_compat6523_vxlan_port_t *vxlan_port);

/* Get all VXLAN port information. */
extern int bcm_compat6523_vxlan_port_get_all(
    int unit,
    bcm_vpn_t l2vpn,
    int port_max,
    bcm_compat6523_vxlan_port_t *port_array,
    int *port_count);


#endif /* defined(INCLUDE_L3) */
#endif /* BCM_RPC_SUPPORT */
#endif /* _COMPAT_6523_H_ */
