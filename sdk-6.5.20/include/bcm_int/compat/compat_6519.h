/*
 * $Id: compat_6519.h,v 1.0 2019/04/03
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-6.5.19 routines
 */

#ifndef _COMPAT_6519_H_
#define _COMPAT_6519_H_

#ifdef BCM_RPC_SUPPORT
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
#include <bcm/types.h>
#include <bcm/l2.h>
#include <bcm/cosq.h>

#if defined(INCLUDE_L3)
#include <bcm/flow.h>
#include <bcm/l3.h>
#include <bcm/ipmc.h>
#include <bcm/failover.h>
#endif
#include <bcm/stat.h>
#include <bcm/cosq.h>
#include <bcm/policer.h>
#include <bcm/mirror.h>
#include <bcm/l2.h>
#include <bcm/flexctr.h>
#include <bcm/oam.h>
#include <bcm/time.h>
#include <bcm/port.h>

/*
 * Queue congestion monitor export trigger info. Calculate InterestFactor
 * of each view and export to collector.
 */
typedef struct bcm_compat6519_cosq_burst_monitor_export_trigger_s {
    uint32 export_threshold;    /* Export threshold */
    uint8 drop_weight;          /* Flow drop counter weight */
    uint8 rx_weight;            /* Flow Rx counter weight */
} bcm_compat6519_cosq_burst_monitor_export_trigger_t;

/* Queue congestion monitor flow view Configuration. */
typedef struct bcm_compat6519_cosq_burst_monitor_flow_view_info_s {
    bcm_compat6519_cosq_burst_monitor_export_trigger_t export_param; /* Queue congestion monitor export
                                                                        trigger Info */
    uint64 start_timer_usecs;           /* Start time of Queue congestion
                                           monitor */
    uint64 age_timer_usecs;             /* Replace existing flow view in host
                                           memory based on age timer */
    uint32 scan_interval_usecs;         /* sample/scan Interval */
    uint32 age_threshold;               /* Aging persistent flows if sum of all
                                           sampled rx_byte_count values goes
                                           below threshold */
    uint32 num_samples;                 /* Number of samples captured per view */
    uint32 num_flows;                   /* Number of flows captured per view */
    uint32 num_flows_clear;             /* Clears configured number of flows
                                           with low Rx byte count from total
                                           number of flows, to allow new flows
                                           learning */
} bcm_compat6519_cosq_burst_monitor_flow_view_info_t;

/* Set Queue congestion monitoring flow view configuration. */
extern int bcm_compat6519_cosq_burst_monitor_flow_view_config_set(
        int unit,
        uint32 options,
        bcm_compat6519_cosq_burst_monitor_flow_view_info_t *flow_view_data);

/* Get Queue congestion monitoring flow view configuration. */
extern int bcm_compat6519_cosq_burst_monitor_flow_view_config_get(
        int unit,
        bcm_compat6519_cosq_burst_monitor_flow_view_info_t *flow_view_data);

#if defined(INCLUDE_L3)

typedef struct bcm_compat6519_flow_encap_config_s {
    bcm_vpn_t vpn;                      /* VPN representing a vfi or vrf */
    bcm_gport_t flow_port;              /* flow port id representing a DVP */
    bcm_if_t intf_id;                   /* l3 interface id */
    uint32 dvp_group;                   /* DVP group ID */
    uint32 oif_group;                   /* L3 OUT interface group ID */
    uint32 vnid;                        /* VN_ID. */
    uint8 pri;                          /* service tag priority. */
    uint8 cfi;                          /* service tag cfi */
    uint16 tpid;                        /* service tag tpid */
    bcm_vlan_t vlan;                    /* service VLAN ID. */
    uint32 flags;                       /* BCM_FLOW_ENCAP_FLAG_xxx. */
    uint32 options;                     /* BCM_FLOW_ENCAP_OPTION_xxx. */
    bcm_flow_encap_criteria_t criteria; /* flow encap criteria. */
    uint32 valid_elements;              /* bitmap of valid fields for the encap
                                           action */
    bcm_flow_handle_t flow_handle;      /* flow handle derived from flow name */
    uint32 flow_option;                 /* flow option derived from flow option
                                           name */
    bcm_gport_t src_flow_port;          /* flow port id representing a SVP */
    uint32 class_id;                    /* used by EFP as input key */
} bcm_compat6519_flow_encap_config_t;

/* Add a flow encap rule. */
extern int bcm_compat6519_flow_encap_add(
    int unit,
    bcm_compat6519_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Delete a flow encap rule on the given encap key. */
extern int bcm_compat6519_flow_encap_delete(
    int unit,
    bcm_compat6519_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Get the encap attributes on the given encap key. */
extern int bcm_compat6519_flow_encap_get(
    int unit,
    bcm_compat6519_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* flow port encap config structure. */
typedef struct bcm_compat6519_flow_port_encap_s {
    bcm_gport_t flow_port;          /* flow port id identifies DVP. */
    uint32 options;                 /* BCM_FLOW_PORT_ENCAP_xxx. */
    uint32 flags;                   /* BCM_FLOW_PORT_ENCAP_FLAG_xxx. */
    uint32 class_id;                /* class id. */
    uint16 mtu;                     /* MTU. */
    uint32 network_group;           /* network group ID */
    uint32 dvp_group;               /* DVP group ID */
    uint8 pri;                      /* service tag priority. */
    uint8 cfi;                      /* service tag cfi */
    uint16 tpid;                    /* service tag tpid */
    bcm_vlan_t vlan;                /* service VLAN ID. */
    bcm_if_t egress_if;             /* egress object */
    uint32 valid_elements;          /* bitmap of valid fields for hardware
                                       parameters */
    bcm_flow_handle_t flow_handle;  /* flow handle */
    uint32 flow_option;             /* flow option id */
    uint16 ip4_id;                  /* IPv4 ID */
    int vlan_pri_map_id;            /* Remarking map ID for the payload VLAN
                                       priority and CFI. */
    bcm_gport_t dst_port;           /* Destination port / trunk. */
} bcm_compat6519_flow_port_encap_t;

/*! Bind the egress object and tunnel initiator with the DVP. */
extern int bcm_compat6519_flow_port_encap_set(
    int unit,
    bcm_compat6519_flow_port_encap_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Get the encap configuration info for the given DVP. */
extern int bcm_compat6519_flow_port_encap_get(
    int unit,
    bcm_compat6519_flow_port_encap_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);


/* Flow tunneling terminator structure. */
typedef struct bcm_compat6519_flow_tunnel_terminator_s {
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
} bcm_compat6519_flow_tunnel_terminator_t;

/* Create a flow tunnel terminator match. */
extern int bcm_compat6519_flow_tunnel_terminator_create(
    int unit,
    bcm_compat6519_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Delete a tunnel termination entry. */
extern int bcm_compat6519_flow_tunnel_terminator_destroy(
    int unit,
    bcm_compat6519_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Get a tunnel termination entry. */
extern int bcm_compat6519_flow_tunnel_terminator_get(
    int unit,
    bcm_compat6519_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* MPLS port type <= 6.5.19. */
typedef struct bcm_compat6519_mpls_port_s {
    bcm_gport_t mpls_port_id;           /* GPORT identifier. */
    uint32 flags;                       /* BCM_MPLS_PORT_xxx. */
    uint32 flags2;                      /* BCM_MPLS_PORT2_xxx. */
    int if_class;                       /* Interface class ID. */
    int exp_map;                        /* Incoming EXP map ID. */
    int int_pri;                        /* Internal priority. */
    uint8 pkt_pri;                      /* Packet priority. */
    uint8 pkt_cfi;                      /* Packet CFI. */
    uint16 service_tpid;                /* Service VLAN TPID value. */
    bcm_gport_t port;                   /* Match port and/or egress port. */
    bcm_mpls_port_match_t criteria;     /* Match criteria. */
    bcm_vlan_t match_vlan;              /* Outer VLAN ID to match. */
    bcm_vlan_t match_inner_vlan;        /* Inner VLAN ID to match. */
    bcm_mpls_label_t match_label;       /* VC label to match. */
    bcm_if_t egress_tunnel_if;          /* MPLS tunnel egress object. */
    bcm_mpls_egress_label_t egress_label; /* Outgoing VC label. */
    int mtu;                            /* MPLS port MTU. */
    bcm_vlan_t egress_service_vlan;     /* Service VLAN to Add/Replace. */
    uint32 pw_seq_number;               /* Initial-value of Pseudo-wire Sequence
                                           number */
    bcm_if_t encap_id;                  /* Encap Identifier. */
    bcm_failover_t ingress_failover_id; /* Ingress Failover Object Identifier. */
    bcm_gport_t ingress_failover_port_id; /* Ingress Failover MPLS Port
                                           Identifier. */
    bcm_failover_t failover_id;         /* Failover Object Identifier. */
    bcm_gport_t failover_port_id;       /* Failover MPLS Port Identifier. */
    bcm_policer_t policer_id;           /* Policer ID to be associated with the
                                           MPLS gport */
    bcm_multicast_t failover_mc_group;  /* MC group used for bi-cast. */
    bcm_failover_t pw_failover_id;      /* Failover Object Identifier for
                                           Redundant PW. */
    bcm_gport_t pw_failover_port_id;    /* Redundant PW port Identifier. */
    bcm_mpls_port_control_channel_type_t vccv_type; /* Indicate VCCV Control Channel */
    bcm_switch_network_group_t network_group_id; /* Split Horizon network group
                                           identifier */
    bcm_vlan_t match_subport_pkt_vid;   /* LLTAG VLAN ID to match. */
    bcm_gport_t tunnel_id;              /* Tunnel Id pointing to ingress LSP. */
    bcm_gport_t per_flow_queue_base;    /* Base queue of the per flow queue set.
                                           Actual queue is decided based on
                                           internal priority and qos map. */
    int qos_map_id;                     /* QOS map identifier. */
    bcm_failover_t egress_failover_id;  /* Failover object index for Egress
                                           Protection */
    bcm_gport_t egress_failover_port_id; /* Failover MPLS Port identifier for
                                           Egress Protection */
    int ecn_map_id;                     /* ECN map identifier. */
    uint32 class_id;                    /* Class ID */
    uint32 egress_class_id;             /* Egress Class ID */
    int inlif_counting_profile;         /* In LIF counting profile */
    bcm_mpls_egress_label_t egress_tunnel_label; /* A mpls tunnel to be binded with the
                                           pwe */
    int nof_service_tags;               /* Number of Service-Tags expected on
                                           Native-ETH header */
    bcm_qos_ingress_model_t ingress_qos_model; /* ingress qos&ttl model */
    bcm_mpls_label_t context_label;     /* Ingress match - context label. */
    bcm_if_t ingress_if;                /* Ingress match - context interface. */
    int port_group;                     /* Port group to match. */
} bcm_compat6519_mpls_port_t;

extern int bcm_compat6519_mpls_port_add(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6519_mpls_port_t *mpls_port);

extern int bcm_compat6519_mpls_port_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6519_mpls_port_t *mpls_port);

extern int bcm_compat6519_mpls_port_get_all(
    int unit,
    bcm_vpn_t vpn,
    int port_max,
    bcm_compat6519_mpls_port_t *port_array,
    int *port_count);
    
/* L3 Host Structure. */
typedef struct bcm_compat6519_l3_host_s {
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
} bcm_compat6519_l3_host_t;

/* Look up an L3 host table entry based on IP address. */
extern int bcm_compat6519_l3_host_find(
    int unit,
    bcm_compat6519_l3_host_t *info);

/* Add an entry into the L3 switching table. */
extern int bcm_compat6519_l3_host_add(
    int unit,
    bcm_compat6519_l3_host_t *info);

/* Delete an entry from the L3 host table. */
extern int bcm_compat6519_l3_host_delete(
    int unit,
    bcm_compat6519_l3_host_t *ip_addr);

/*
 * Deletes L3 entries that match or do not match a specified L3 interface
 * number
 */
extern int bcm_compat6519_l3_host_delete_by_interface(
    int unit,
    bcm_compat6519_l3_host_t *info);

/* Deletes all L3 host table entries. */
extern int bcm_compat6519_l3_host_delete_all(
    int unit,
    bcm_compat6519_l3_host_t *info);

/* Attach counters entries to the given L3 host entry */
extern int bcm_compat6519_l3_host_stat_attach(
    int unit,
    bcm_compat6519_l3_host_t *info,
    uint32 stat_counter_id);

/* Detach  counters entries to the given L3 host entry */
extern int bcm_compat6519_l3_host_stat_detach(
    int unit,
    bcm_compat6519_l3_host_t *info);

/* Get the specified counter statistic for a L3 host entry. */
extern int bcm_compat6519_l3_host_stat_counter_get(
    int unit,
    bcm_compat6519_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

/*
 * Force an immediate counter update and retrieve the specified counter
 * statistic for a L3 host entry.
 */
extern int bcm_compat6519_l3_host_stat_counter_sync_get(
    int unit,
    bcm_compat6519_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

/* Set the specified counter statistic for a L3 host entry */
extern int bcm_compat6519_l3_host_stat_counter_set(
    int unit,
    bcm_compat6519_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

/* Provide stat counter ids associated with given L3 host entry */
extern int bcm_compat6519_l3_host_stat_id_get(
    int unit,
    bcm_compat6519_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 *stat_counter_id);

/* L3 Route Structure. */
typedef struct bcm_compat6519_l3_route_s {
    uint32 l3a_flags;                   /* See BCM_L3_xxx flag definitions. */
    uint32 l3a_flags2;                  /* See BCM_L3_FLAGS2_xxx flag
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
} bcm_compat6519_l3_route_t;

/* Delete L3 entries based on IP prefix (network). */
extern int bcm_compat6519_l3_host_delete_by_network(
    int unit,
    bcm_compat6519_l3_route_t *ip_addr);

/* Add an IP route to the L3 route table. */
extern int bcm_compat6519_l3_route_add(
    int unit,
    bcm_compat6519_l3_route_t *info);

/* Delete an IP route from the DEFIP table. */
extern int bcm_compat6519_l3_route_delete(
    int unit,
    bcm_compat6519_l3_route_t *info);

/* Delete routes based on matching or non-matching L3 interface number. */
extern int bcm_compat6519_l3_route_delete_by_interface(
    int unit,
    bcm_compat6519_l3_route_t *info);

/* Delete all routes. */
extern int bcm_compat6519_l3_route_delete_all(
    int unit,
    bcm_compat6519_l3_route_t *info);

/* Look up a route given the network and netmask. */
extern int bcm_compat6519_l3_route_get(
    int unit,
    bcm_compat6519_l3_route_t *info);

/* Given a network, return all the paths for this route. */
extern int bcm_compat6519_l3_route_multipath_get(
    int unit,
    bcm_compat6519_l3_route_t *the_route,
    bcm_compat6519_l3_route_t *path_array,
    int max_path,
    int *path_count);

/* Set flex counter object value for the given L3 unicast route. */
extern int bcm_compat6519_l3_route_flexctr_object_set(
    int unit,
    bcm_compat6519_l3_route_t *info,
    uint32 value);

/* Get flex counter object value for the given L3 unicast route. */
extern int bcm_compat6519_l3_route_flexctr_object_get(
    int unit,
    bcm_compat6519_l3_route_t *info,
    uint32 *value);

/* Get L3 route statistics. */
extern int bcm_compat6519_l3_route_stat_get(
    int unit,
    bcm_compat6519_l3_route_t *route,
    bcm_l3_stat_t stat,
    uint64 *val);

/* Get L3 route statistics. */
extern int bcm_compat6519_l3_route_stat_get32(
    int unit,
    bcm_compat6519_l3_route_t *route,
    bcm_l3_stat_t stat,
    uint32 *val);

/* Set L3 route statistics. */
extern int bcm_compat6519_l3_route_stat_set(
    int unit,
    bcm_compat6519_l3_route_t *route,
    bcm_l3_stat_t stat,
    uint64 val);

/* Set L3 route statistics. */
extern int bcm_compat6519_l3_route_stat_set32(
    int unit,
    bcm_compat6519_l3_route_t *route,
    bcm_l3_stat_t stat,
    uint32 val);

/* Enable/disable collection of statistics for the indicated L3 route. */
extern int bcm_compat6519_l3_route_stat_enable_set(
    int unit,
    bcm_compat6519_l3_route_t *route,
    int enable);

/* Attach counters entries to the given L3 route index */
extern int bcm_compat6519_l3_route_stat_attach(
    int unit,
    bcm_compat6519_l3_route_t *info,
    uint32 stat_counter_id);

/* Detach  counters entries to the given L3 route index. */
extern int bcm_compat6519_l3_route_stat_detach(
    int unit,
    bcm_compat6519_l3_route_t *info);

/* Get counter statistic values for a l3 route index. */
extern int bcm_compat6519_l3_route_stat_counter_get(
    int unit,
    bcm_compat6519_l3_route_t *info,
    bcm_l3_route_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

/* 
 * Force an immediate counter update and retrieve counter statistic
 * values for a l3 route index.
 */
extern int bcm_compat6519_l3_route_stat_counter_sync_get(
    int unit,
    bcm_compat6519_l3_route_t *info,
    bcm_l3_route_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

/* Set counter statistic values for a l3 route index */
extern int bcm_compat6519_l3_route_stat_counter_set(
    int unit,
    bcm_compat6519_l3_route_t *info,
    bcm_l3_route_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

/* Get multiple counter statistic values for multiple l3 route index. */
extern int bcm_compat6519_l3_route_stat_multi_get(
    int unit,
    bcm_compat6519_l3_route_t *info,
    int nstat,
    bcm_l3_route_stat_t *stat_arr,
    uint64 *value_arr);

/* Get 32bit multiple counter statistic values for multiple l3 route index. */
extern int bcm_compat6519_l3_route_stat_multi_get32(
    int unit,
    bcm_compat6519_l3_route_t *info,
    int nstat,
    bcm_l3_route_stat_t *stat_arr,
    uint32 *value_arr);

/* Set multiple counter statistic values for multiple l3 route index. */
extern int bcm_compat6519_l3_route_stat_multi_set(
    int unit,
    bcm_compat6519_l3_route_t *info,
    int nstat,
    bcm_l3_route_stat_t *stat_arr,
    uint64 *value_arr);

/* Set 32bit multiple counter statistic values for multiple l3 route index. */
extern int bcm_compat6519_l3_route_stat_multi_set32(
    int unit,
    bcm_compat6519_l3_route_t *info,
    int nstat,
    bcm_l3_route_stat_t *stat_arr,
    uint32 *value_arr);

/* Get stat counter ID associated with given L3 route index. */
extern int bcm_compat6519_l3_route_stat_id_get(
    int unit,
    bcm_compat6519_l3_route_t *info,
    bcm_l3_route_stat_t stat,
    uint32 *stat_counter_id);

/* Find a longest prefix matched route given an IP address. */
extern int bcm_compat6519_l3_route_find(
    int unit,
    bcm_compat6519_l3_host_t *host,
    bcm_compat6519_l3_route_t *route);

/* Find a longest prefix matched route given the ip and netmask. */
extern int bcm_compat6519_l3_subnet_route_find(
    int unit,
    bcm_compat6519_l3_route_t *input,
    bcm_compat6519_l3_route_t *route);

/* IPMC address structure. */
typedef struct bcm_compat6519_ipmc_addr_s {
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
} bcm_compat6519_ipmc_addr_t;

/* Add a new entry to IPMC table. */
extern int bcm_compat6519_ipmc_add(
    int unit,
    bcm_compat6519_ipmc_addr_t *data);

/* Find info about an IPMC entry. */
extern int bcm_compat6519_ipmc_find(
    int unit,
    bcm_compat6519_ipmc_addr_t *data);

/* Remove an entry from IPMC table. */
extern int bcm_compat6519_ipmc_remove(
    int unit,
    bcm_compat6519_ipmc_addr_t *data);

/* Attach counters entries to the given IPMC group. */
extern int bcm_compat6519_ipmc_stat_attach(
    int unit,
    bcm_compat6519_ipmc_addr_t *info,
    uint32 stat_counter_id);

/* Detach counters entries to the given IPMC group. */
extern int bcm_compat6519_ipmc_stat_detach(
    int unit,
    bcm_compat6519_ipmc_addr_t *info);

/* Get counter statistic values for an IPMC group. */
extern int bcm_compat6519_ipmc_stat_counter_get(
    int unit,
    bcm_compat6519_ipmc_addr_t *info,
    bcm_ipmc_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

/*
 * Force an immediate counter update and retrieve counter statistic
 * values for an IPMC group.
 */
 extern int bcm_compat6519_ipmc_stat_counter_sync_get(
    int unit,
    bcm_compat6519_ipmc_addr_t *info,
    bcm_ipmc_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

/* Set counter statistic values for an IPMC group. */
extern int bcm_compat6519_ipmc_stat_counter_set(
    int unit,
    bcm_compat6519_ipmc_addr_t *info,
    bcm_ipmc_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

/* Get multiple counter statistic values for multiple IPMC group. */
extern int bcm_compat6519_ipmc_stat_multi_get(
    int unit,
    bcm_compat6519_ipmc_addr_t *info,
    int nstat,
    bcm_ipmc_stat_t *stat_arr,
    uint64 *value_arr);

/* Get 32bit multiple counter statistic values for multiple IPMC group. */
extern int bcm_compat6519_ipmc_stat_multi_get32(
    int unit,
    bcm_compat6519_ipmc_addr_t *info,
    int nstat,
    bcm_ipmc_stat_t *stat_arr,
    uint32 *value_arr);

/* Set multiple counter statistic values for multiple IPMC group. */
extern int bcm_compat6519_ipmc_stat_multi_set(
    int unit,
    bcm_compat6519_ipmc_addr_t *info,
    int nstat,
    bcm_ipmc_stat_t *stat_arr,
    uint64 *value_arr);

/* Set 32bit multiple counter statistic values for multiple IPMC group. */
extern int bcm_compat6519_ipmc_stat_multi_set32(
    int unit,
    bcm_compat6519_ipmc_addr_t *info,
    int nstat,
    bcm_ipmc_stat_t *stat_arr,
    uint32 *value_arr);

/* Get stat counter ID associated with given IPMC group. */
extern int bcm_compat6519_ipmc_stat_id_get(
    int unit,
    bcm_compat6519_ipmc_addr_t *info,
    bcm_ipmc_stat_t stat,
    uint32 *stat_counter_id);

/* Add a new configurational entry. */
extern int bcm_compat6519_ipmc_config_add(
    int unit,
    bcm_compat6519_ipmc_addr_t *config);

/* Find a configurational entry. */
extern int bcm_compat6519_ipmc_config_find(
    int unit,
    bcm_compat6519_ipmc_addr_t *config);

/* Remove a configurational entry */
extern int bcm_compat6519_ipmc_config_remove(
    int unit,
    bcm_compat6519_ipmc_addr_t *config);

/* L3 Egress Structure.

@p Description of an L3 forwarding destination.
 */
typedef struct bcm_compat6519_l3_egress_s {
    uint32 flags;    /* Interface flags (BCM_L3_TGID, BCM_L3_L2TOCPU). */
    uint32 flags2;    /* See BCM_L3_FLAGS2_xxx flag definitions. */
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
} bcm_compat6519_l3_egress_t;

/* Create an Egress forwarding object. */
extern int bcm_compat6519_l3_egress_create(
    int unit,
    uint32 flags,
    bcm_compat6519_l3_egress_t *egr,
    bcm_if_t *if_id);

/* Get an Egress forwarding object. */
extern int bcm_compat6519_l3_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6519_l3_egress_t *egr);

/* Find an interface pointing to an Egress forwarding object. */
extern int bcm_compat6519_l3_egress_find(
    int unit,
    bcm_compat6519_l3_egress_t *egr,
    bcm_if_t *intf);

/* Retrieve valid allocation for a given number of FECs. */
extern int bcm_compat6519_l3_egress_allocation_get(
    int unit,
    uint32 flags,
    bcm_compat6519_l3_egress_t *egr,
    uint32 nof_members,
    bcm_if_t *if_id);

/* Set a failover egress object with fixed offset to protect primary NH intf. */
extern int bcm_compat6519_failover_egress_set(
    int unit,
    bcm_if_t intf,
    bcm_compat6519_l3_egress_t *failover_egr);

/* Get a failover egress object with fixed offset to protect primary NH intf. */
extern int bcm_compat6519_failover_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6519_l3_egress_t *failover_egr);

/* L3 Ingress Structure.

@p Description of an L3 Ingress interface.
 */
typedef struct bcm_compat6519_l3_ingress_s {
    uint32 flags;    /* Interface flags. */
    bcm_vrf_t vrf;    /* Virtual router id. */
    bcm_l3_ingress_urpf_mode_t urpf_mode;    /* URPF mode setting for L3 Ingress Interface. */
    int intf_class;    /* Classification class ID. */
    bcm_vlan_t ipmc_intf_id;    /* IPMC L2 distribution Vlan. */
    int qos_map_id;    /* QoS DSCP priority map. */
    int ip4_options_profile_id;    /* IP4 Options handling Profile ID */
    int nat_realm_id;    /* Realm id of the interface for NAT */
    int tunnel_term_ecn_map_id;    /* Tunnel termination ecn map id */
    uint32 intf_class_route_disable;    /* routing enablers bit map in rif profile */
    int oam_default_profile;    /* OAM profile for RIF */
    uint32 hash_layers_disable;    /* load balancing disable hash layers bit map using the BCM_HASH_LAYER_XXX_DISABLE flags */
    int opaque_ctrl_id;    /* Opaque control ID. */
} bcm_compat6519_l3_ingress_t;

/* Create L3 Ingress Interface object. */
extern int bcm_compat6519_l3_ingress_create(
    int unit,
    bcm_compat6519_l3_ingress_t *ing_intf,
    bcm_if_t *intf_id);

/* Get an Ingress Interface object. */
extern int bcm_compat6519_l3_ingress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6519_l3_ingress_t *ing_intf);

/* Find an interface pointing to an Ingress Interface object. */
extern int bcm_compat6519_l3_ingress_find(
    int unit,
    bcm_compat6519_l3_ingress_t *ing_intf,
    bcm_if_t *intf_id);

#endif /* INCLUDE_L3 */

/* Packet protocol match structure. */
typedef struct bcm_compat6519_switch_pkt_protocol_match_s {
    uint8 l2_iif_opaque_ctrl_id;        /* L2 iif based control id. */
    uint8 l2_iif_opaque_ctrl_id_mask;   /* L2 iif based control id mask. */
    bcm_vlan_t vfi;                     /* Virtual forwarding instance. */
    bcm_vlan_t vfi_mask;                /* VFI mask. */
    bcm_mac_t macda;                    /* Destination MAC address. */
    bcm_mac_t macda_mask;               /* Destination MAC address mask. */
    uint16 ethertype;                   /* Ethernet type. */
    uint16 ethertype_mask;              /* Ethernet type mask. */
    uint16 arp_rarp_opcode;             /* ARP/RARP packet operation code. */
    uint16 arp_rarp_opcode_mask;        /* ARP/RARP packet operation code mask. */
    uint8 icmp_type;                    /* ICMP type. */
    uint8 icmp_type_mask;               /* ICMP type mask. */
    uint8 igmp_type;                    /* IGMP type. */
    uint8 igmp_type_mask;               /* IGMP type mask. */
    uint8 l4_valid;                     /* If set, layer 4 header present. */
    uint8 l4_valid_mask;                /* L4_valid mask. */
    uint16 l4_src_port;                 /* Layer 4 source port. */
    uint16 l4_src_port_mask;            /* Layer 4 source port mask. */
    uint16 l4_dst_port;                 /* Layer 4 destination port. */
    uint16 l4_dst_port_mask;            /* Layer 4 destination port mask. */
    uint16 l5_bytes_0_1;                /* First 2 bytes of layer 5 header. */
    uint16 l5_bytes_0_1_mask;           /* L5_bytes_0_1 mask. */
    uint16 outer_l5_bytes_0_1;          /* Outer l5 bytes 0 1. */
    uint16 outer_l5_bytes_0_1_mask;     /* Outer l5 bytes 0 1 mask. */
    uint8 ip_last_protocol;             /* Protocol from IP extension header if
                                           present, otherwise from IP header. */
    uint8 ip_last_protocol_mask;        /* Ip_last_protocol mask. */
    uint8 fixed_hve_result_set_1;       /* Fixed HVE result 1. */
    uint8 fixed_hve_result_set_1_mask;  /* Fixed HVE result 1 mask. */
    uint8 fixed_hve_result_set_5;       /* Fixed HVE result 5. */
    uint8 fixed_hve_result_set_5_mask;  /* Fixed HVE result 5 mask. */
    uint8 flex_hve_result_set_1;        /* Flex HVE result 1. */
    uint8 flex_hve_result_set_1_mask;   /* Flex HVE result 1 mask. */
    uint8 tunnel_processing_results_1;  /* Tunnel processing results 1. */
    uint8 tunnel_processing_results_1_mask; /* Tunnel processing results 1 mask. */
    uint8 vfp_opaque_ctrl_id;           /* VFP opaque ctrl id. */
    uint8 vfp_opaque_ctrl_id_mask;      /* VFP opaque ctrl id mask. */
    uint8 vlan_xlate_opaque_ctrl_id;    /* VLAN_XLATE opaque ctrl id: Bit 0 :
                                           VLAN_XLATE drop; Bit 1 : VLAN_XLATE
                                           copy to CPU; Bits 2-3: The highest 2
                                           bits of VLAN_XLATE opaque control id. */
    uint8 vlan_xlate_opaque_ctrl_id_mask; /* vlan_xlate_opaque_ctrl_id - Mask. */
    uint8 vlan_xlate_opaque_ctrl_id_1;  /* VLAN_XLATE opaque ctrl id_1: Bits
                                           0-3: The lowest 4 bits of VLAN_XLATE
                                           opaque control id. */
    uint8 vlan_xlate_opaque_ctrl_id_1_mask; /* vlan_xlate_opaque_ctrl_id_1 - Mask. */
} bcm_compat6519_switch_pkt_protocol_match_t;

/* An API to add a protocol control entry. */
extern int bcm_compat6519_switch_pkt_protocol_control_add(
    int unit,
    uint32 options,
    bcm_compat6519_switch_pkt_protocol_match_t *match,
    bcm_switch_pkt_control_action_t *action,
    int priority);

/* An API to get action and priority for a protocol control entry. */
extern int bcm_compat6519_switch_pkt_protocol_control_get(
    int unit,
    bcm_compat6519_switch_pkt_protocol_match_t *match,
    bcm_switch_pkt_control_action_t *action,
    int *priority);

/* An API to delete a protocol control entry. */
extern int bcm_compat6519_switch_pkt_protocol_control_delete(
    int unit,
    bcm_compat6519_switch_pkt_protocol_match_t *match);

/* Learning events distibution. */
typedef struct bcm_compat6519_l2_addr_distribute_s {
    uint32 flags;    /* BCM_L2_ADDR_DIST_XXX flags. */
    bcm_vlan_t vid;    /* VLAN or VPN identifier. */
} bcm_compat6519_l2_addr_distribute_t;

/* Per VSI set which events (learn, station move (transplant), aged-out)
to inform CPU about. */
extern int bcm_compat6519_l2_addr_msg_distribute_set(
    int unit,
    bcm_compat6519_l2_addr_distribute_t *distribution);

/* Per VSI, get which events (learn, station move (transplant), aged-out)
to inform CPU about. */
extern int bcm_compat6519_l2_addr_msg_distribute_get(
    int unit,
    bcm_compat6519_l2_addr_distribute_t *distribution);

/*
 * Mirror destination Structure
 *
 * Contains information required for manipulating mirror destinations.
 */
typedef struct bcm_compat6519_mirror_destination_s {
    bcm_gport_t mirror_dest_id;         /* Unique mirror destination and
                                           encapsulation identifier. */
    uint32 flags;                       /* See BCM_MIRROR_DEST_xxx flag
                                           definitions. */
    bcm_gport_t gport;                  /* Mirror destination. */
    uint8 version;                      /* IP header version. */
    uint8 tos;                          /* Traffic Class/Tos byte. */
    uint8 ttl;                          /* Hop limit. */
    bcm_ip_t src_addr;                  /* Tunnel source ip address (IPv4). */
    bcm_ip_t dst_addr;                  /* Tunnel destination ip address (IPv4). */
    bcm_ip6_t src6_addr;                /* Tunnel source ip address (IPv6). */
    bcm_ip6_t dst6_addr;                /* Tunnel destination ip address (IPv6). */
    uint32 flow_label;                  /* IPv6 header flow label field. */
    bcm_mac_t src_mac;                  /* L2 source mac address. */
    bcm_mac_t dst_mac;                  /* L2 destination mac address. */
    uint16 tpid;                        /* L2 header outer TPID. */
    bcm_vlan_t vlan_id;                 /* Vlan id. */
    bcm_trill_name_t trill_src_name;    /* TRILL source bridge nickname */
    bcm_trill_name_t trill_dst_name;    /* TRILL destination bridge nickname */
    int trill_hopcount;                 /* TRILL hop count */
    uint16 niv_src_vif;                 /* Source Virtual Interface of NIV tag */
    uint16 niv_dst_vif;                 /* Destination Virtual Interface of NIV
                                           tag */
    uint32 niv_flags;                   /* NIV flags BCM_MIRROR_NIV_XXX */
    uint16 gre_protocol;                /* L3 GRE header protocol */
    bcm_policer_t policer_id;           /* policer_id */
    int stat_id;                        /* stat_id */
    int stat_id2;                       /* stat_id2 for second counter pointer */
    bcm_if_t encap_id;                  /* Encapsulation index */
    bcm_if_t tunnel_id;                 /* IP tunnel for encapsulation. Valid
                                           only if BCM_MIRROR_DEST_TUNNEL_IP_GRE
                                           is set */
    uint16 span_id;                     /* SPAN-ID. Valid only if
                                           BCM_MIRROR_DEST_TUNNEL_WITH_SPAN_ID
                                           is set */
    uint8 pkt_prio;                     /* L2 header PCP */
    uint32 sample_rate_dividend;        /* The probability of mirroring a packet
                                           is: sample_rate_dividend >=
                                           sample_rate_divisor ? 1 :
                                           sample_rate_dividend /
                                           sample_rate_divisor */
    uint32 sample_rate_divisor;
    uint8 int_pri;                      /* Internal Priority */
    uint16 etag_src_vid;                /* Extended (source) port vlan id */
    uint16 etag_dst_vid;                /* Extended (destination) port vlan id */
    uint16 udp_src_port;                /* UDP source port */
    uint16 udp_dst_port;                /* UDP destination port */
    uint32 egress_sample_rate_dividend; /* The probability of outbound mirroring
                                           a packet from the destination is
                                           sample_rate_dividend >=
                                           sample_rate_divisor ? 1 :
                                           sample_rate_dividend /
                                           sample_rate_divisor */
    uint32 egress_sample_rate_divisor;
    uint8 recycle_context;              /* recycle context of egress originating
                                           packets */
    uint16 packet_copy_size;            /*  If non zero, the copied packet will
                                           be truncated to the first
                                           packet_copy_size . Current supported
                                           values for DNX are 0, 64, 128, 192 */
    uint16 egress_packet_copy_size;     /* If non zero and the packet is copied
                                           from the egress, the packet will be
                                           truncated to the first
                                           packet_copy_size . Current supported
                                           values for DNX are 0, 256. */
    bcm_mirror_pkt_header_updates_t packet_control_updates;
    bcm_mirror_psc_t rtag;              /* specify RTAG HASH algorithm used for
                                           shared-id mirror destination */
    uint8 df;                           /* Set the do not fragment bit of IP
                                           header in mirror encapsulation */
    uint8 truncate;                     /* Setting truncate would result in
                                           mirroring a truncated frame */
    uint16 template_id;                 /* Template ID for IPFIX HDR */
    uint32 observation_domain;          /* Observation domain for IPFIX HDR */
    uint32 is_recycle_strict_priority;  /* Recycle priority (1-lossless, 0-high) */
    int ext_stat_id[BCM_MIRROR_EXT_STAT_ID_COUNT]; /* ext_stat_id to support statistic
                                           interface for mirror packets. */
    uint32 flags2;                      /* See BCM_MIRROR_DEST_FLAGS2_xxx flag
                                           definitions. */
    uint32 vni;                         /* Virtual Network Interface ID. */
    uint32 gre_seq_number;              /* Sequence number value used in GRE
                                           header. If no value is provided,
                                           gre_seq_number will start with 0.
                                           Valid only if
                                           BCM_MIRROR_DEST_TUNNEL_IP_GRE is set. */
    bcm_mirror_pkt_erspan_encap_t erspan_header; /* ERSPAN encapsulation header fields.
                                           Valid only if only
                                           BCM_MIRROR_DEST_TUNNEL_IP_GRE is set. */
    uint32 initial_seq_number;          /* Starting seq number in SFLOW or PSAMP
                                           header */
    bcm_mirror_psamp_fmt2_meta_data_t meta_data_type; /* Type of item in 'meta_data' */
    uint16 meta_data;                   /* Actual value of class id or meta
                                           data, based on 'meta_data_type' */
    int ext_stat_valid;                 /* Validates the ext_stat. */
    int ext_stat_type_id[BCM_MIRROR_EXT_STAT_ID_COUNT]; /* Type ids for statistic interface. */
    uint16 ipfix_version;               /* IPFIX version number in PSAMP
                                           encapsulation. */
    uint16 psamp_epoch;                 /* PSAMP epoch field. */
    int cosq;                           /* Queue id that mirror copy is enqueued
                                           into. */
    uint8 cfi;                          /* CFI of VLAN tag. */
    bcm_mirror_timestamp_mode_t timestamp_mode; /* Set timestamp mode for mirror
                                           encapsulation. */
    bcm_mirror_multi_dip_group_t multi_dip_group; /* Multiple destination IP group. */
    int drop_group_bmp;
    bcm_port_t second_pass_src_port;    /* Source port in the second pass. */
    bcm_mirror_truncate_mode_t encap_truncate_mode; /* Truncate mode for mirror
                                           encapsulation packets. */
    int encap_truncate_profile_id;      /* Truncate length profile ID for mirror
                                           encapsulation packets. */
} bcm_compat6519_mirror_destination_t;

/* Create a mirror (destination, encapsulation) pair. */
extern int bcm_compat6519_mirror_destination_create(
    int unit,
    bcm_compat6519_mirror_destination_t *mirror_dest);

/* Get  mirror (destination, encapsulation) pair. */
extern int bcm_compat6519_mirror_destination_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    bcm_compat6519_mirror_destination_t *mirror_dest);

/* Mirror payload zeroing profile information */
typedef struct bcm_compat6519_mirror_payload_zero_offsets_s {
    int l2_offset;          /* offset from the end of the known L2 headers to
                               start zeroing */
    int l3_offset;          /* offset from the end of the known L3 headers to
                               start zeroing for non-UDP packets */
    int udp_offset;         /* offset from the end of the known L3 headers to
                               start zeroing for UDP packets */
} bcm_compat6519_mirror_payload_zero_offsets_t;

/* Create mirror zero payload offset profile. */
extern int bcm_compat6519_mirror_payload_zero_profile_create(
    int unit,
    bcm_compat6519_mirror_payload_zero_offsets_t *mirror_payload_offset_info,
    uint32 flags,
    uint32 *profile_index);

/* Mirror source structure. */
typedef struct bcm_compat6519_mirror_source_s {
    bcm_mirror_source_type_t type;    /* Mirror source type. */
    uint32 flags;    /* Mirror source flags only with port source type. */
    bcm_port_t port;    /* Port/gport id. */
    bcm_pkt_trace_event_t trace_event;    /* Trace event. */
    bcm_pkt_drop_event_t drop_event;    /* Drop event. */
    int mod_profile_id;    /* Mirror-on-drop profile id. */
    uint32 sample_profile_id;    /* Sample profile id. */
} bcm_compat6519_mirror_source_t;

/* Add a mirror destination to a mirror source. */
extern int bcm_compat6519_mirror_source_dest_add(
    int unit,
    bcm_compat6519_mirror_source_t *source,
    bcm_gport_t mirror_dest_id);

/* Delete a mirror destination from a mirror source. */
extern int bcm_compat6519_mirror_source_dest_delete(
    int unit,
    bcm_compat6519_mirror_source_t *source,
    bcm_gport_t mirror_dest_id);

/* Delete all mirror destinations applied on a mirror source. */
extern int bcm_compat6519_mirror_source_dest_delete_all(
    int unit,
    bcm_compat6519_mirror_source_t *source);

/* Get all mirror destinations applied on a mirror source. */
extern int bcm_compat6519_mirror_source_dest_get_all(
    int unit,
    bcm_compat6519_mirror_source_t *source,
    int array_size,
    bcm_gport_t *mirror_dest,
    int *count);

/* Flowtracker tracking parameter information. */
typedef struct bcm_compat6519_flowtracker_tracking_param_info_s {
    uint32 flags;                       /* Flags for tracking parameters. */
    bcm_flowtracker_tracking_param_type_t param; /* Type of tracking parameter. */
    bcm_flowtracker_tracking_param_user_data_t tracking_data; /* Element data for the tracking param
                                           to be used to add a flow. */
    bcm_flowtracker_tracking_param_mask_t mask; /* Mask to select granular information
                                           from tracking parameter. By default,
                                           mask is set to all enabled. */
    bcm_flowtracker_check_t check_id;   /* Flowtracker check to be used to
                                           tracking flow check data. */
    bcm_flowtracker_timestamp_source_t src_ts; /* Timestamp source info, valid in case
                                           of timestamp and delay related
                                           tracking params. */
    bcm_udf_id_t udf_id;                /* UDF Id associated with the tracking
                                           param. */
} bcm_compat6519_flowtracker_tracking_param_info_t;

/* Stats of the Flows on the group. */
typedef struct bcm_compat6519_flowtracker_group_stat_s {
    uint64 flow_exceeded_count;         /* Count of flows not tracked in this
                                           group after maximum number of flows
                                           configured for this group are already
                                           learnt. */
    uint64 flow_missed_count;           /* Count of Aged out flows in the group. */
    uint64 flow_aged_out_count;         /* Count of flows which are Aged out in
                                           the group. */
    uint64 flow_learnt_count;           /* Count of flows learnt on the group. */
    uint64 flow_meter_exceeded_count;   /* Count of flows which exceeded the
                                           metering limits in the group. */
} bcm_compat6519_flowtracker_group_stat_t;

/* Actions related information for Flow check. */
typedef struct bcm_compat6519_flowtracker_check_action_info_s {
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
} bcm_compat6519_flowtracker_check_action_info_t;

/* Set tracking parameter for this flowtracker group. */
extern int bcm_compat6519_flowtracker_group_tracking_params_set(
    int unit,
    bcm_flowtracker_group_t id,
    int num_tracking_params,
    bcm_compat6519_flowtracker_tracking_param_info_t *list_of_tracking_params);

/* Get flowtracker tracking parameters for this group. */
extern int bcm_compat6519_flowtracker_group_tracking_params_get(
    int unit,
    bcm_flowtracker_group_t id,
    int max_size,
    bcm_compat6519_flowtracker_tracking_param_info_t *list_of_tracking_params,
    int *list_size);

/* Set flow group's statistics. */
extern int bcm_compat6519_flowtracker_group_stat_set(
    int unit,
    bcm_flowtracker_group_t group_id,
    bcm_compat6519_flowtracker_group_stat_t *group_stats);

/* Get flow group's statistics. */
extern int bcm_compat6519_flowtracker_group_stat_get(
    int unit,
    bcm_flowtracker_group_t group_id,
    bcm_compat6519_flowtracker_group_stat_t *group_stats);

/* Initialize a flow check action information structure. */
extern int bcm_compat6519_flowtracker_check_action_info_set(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_compat6519_flowtracker_check_action_info_t info);

/* Initialize a flow check action information structure. */
extern int bcm_compat6519_flowtracker_check_action_info_get(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_compat6519_flowtracker_check_action_info_t *info);

/*
 * Add a user flow entry basis user input key elements. API expects that
 * all tracking parametrs of type = 'KEY' in the group are specified.
 */
extern int bcm_compat6519_flowtracker_user_entry_add(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    uint32 options,
    int num_user_entry_params,
    bcm_compat6519_flowtracker_tracking_param_info_t *user_entry_param_list,
    bcm_flowtracker_user_entry_handle_t *entry_handle);

/* Fetch user entry info given the entry handle. */
extern int bcm_compat6519_flowtracker_user_entry_get(
    int unit,
    bcm_flowtracker_user_entry_handle_t *entry_handle,
    int num_user_entry_params,
    bcm_compat6519_flowtracker_tracking_param_info_t *user_entry_param_list,
    int *actual_user_entry_params);

/* Flowtracker export trigger information. */
typedef struct bcm_compat6519_flowtracker_export_trigger_info_s {
    SHR_BITDCL trigger_bmp[_SHR_BITDCLSIZE(bcmFlowtrackerExportTriggerCount)];
                                        /* The bitmap of export triggers which
                                           will result in the export of the
                                           IPFIX packets. */
    uint32 interval_usecs;              /* If one of the export triggers is
                                           bcmFlowtrackerExportTriggerTimer,
                                           this provides the interval for the
                                           timer. */
} bcm_compat6519_flowtracker_export_trigger_info_t;

/* Set flow group's export triggers. */
extern int bcm_compat6519_flowtracker_group_export_trigger_set(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_compat6519_flowtracker_export_trigger_info_t *export_trigger_info);

/* Get flow group's export triggers. */
extern int bcm_compat6519_flowtracker_group_export_trigger_get(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_compat6519_flowtracker_export_trigger_info_t *export_trigger_info);

/*
 * IPFIX header in the encap used to send the data packet to the remote
 * collector.
 */
typedef struct bcm_compat6519_collector_ipfix_header_s {
    int version;                        /* Ipfix Protocol Version. */
    int initial_sequence_num;           /* Ipfix initial sequence number. */
    bcm_collector_ipfix_observation_domain_t observation_domain_id; /* IPFIX observation domain from which
                                           this collector is receiving the
                                           export data.. */
} bcm_compat6519_collector_ipfix_header_t;

/* Collector information. */
typedef struct bcm_compat6519_collector_info_s {
    bcm_collector_type_t collector_type; /* Collector type. Remote vs Local */
    bcm_collector_transport_type_t transport_type; /* Transport type used for exporting
                                                      data to the collector. This
                                                      identifies the usable fields within
                                                      the encap structure member defined
                                                      below. */
    bcm_collector_eth_header_t eth;     /* Ethernet encapsulation of the packet
                                           sent to collector. */
    bcm_collector_ipv4_header_t ipv4;   /* IPv4 encapsulation of the packet sent
                                           to collector. */
    bcm_collector_ipv6_header_t ipv6;   /* IPv6 encapsulation of the packet sent
                                           to collector. */
    bcm_collector_udp_header_t udp;     /* UDP encapsulation of the packet sent
                                           to collector. */
    bcm_compat6519_collector_ipfix_header_t ipfix; /* IPFIX encapsulation of the packet
                                           sent to collector. */
    bcm_collector_protobuf_header_t protobuf; /* Protobuf header information. */
    bcm_collector_timestamp_source_t src_ts; /* Timestamp source info. */
} bcm_compat6519_collector_info_t;

/* Create a collector with given collector info. */
extern int bcm_compat6519_collector_create(
        int unit,
        uint32 options,
        bcm_collector_t *collector_id,
        bcm_compat6519_collector_info_t *collector_info);

/* Get collector information with ID. */
extern int bcm_compat6519_collector_get(
        int unit,
        bcm_collector_t id,
        bcm_compat6519_collector_info_t *collector_info);

/* Device-independent L2 address structure. */
typedef struct bcm_compat6519_l2_addr_s {
    uint32 flags;                       /* BCM_L2_xxx flags. */
    uint32 flags2;                      /* BCM_L2_FLAGS2_xxx flags. */
    uint32 station_flags;               /* BCM_L2_STATION_xxx flags. */
    bcm_mac_t mac;                      /* 802.3 MAC address. */
    bcm_vlan_t vid;                     /* VLAN identifier. */
    int port;                           /* Zero-based port number. */
    int modid;                          /* XGS: modid. */
    bcm_trunk_t tgid;                   /* Trunk group ID. */
    bcm_cos_t cos_dst;                  /* COS based on dst addr. */
    bcm_cos_t cos_src;                  /* COS based on src addr. */
    bcm_multicast_t l2mc_group;         /* XGS: index in L2MC table */
    bcm_if_t egress_if;                 /* XGS: index in Next Hop Tables. Used
                                           it with BCM_L2_FLAGS2_ROE_NHI flag */
    bcm_multicast_t l3mc_group;         /* XGS: index in L3_IPMC table. Use it
                                           with BCM_L2_FLAGS2_L3_MULTICAST. */
    bcm_pbmp_t block_bitmap;            /* XGS: blocked egress bitmap. */
    int auth;                           /* Used if auth enabled on port. */
    int group;                          /* Group number for FP. */
    bcm_fabric_distribution_t distribution_class; /* Fabric Distribution Class. */
    int encap_id;                       /* out logical interface */
    int age_state;                      /* Age state of the entry */
    uint32 flow_handle;                 /* FLOW handle for flex entries. */
    uint32 flow_option_handle;          /* FLOW option handle for flex entries. */
    bcm_flow_logical_field_t logical_fields[BCM_FLOW_MAX_NOF_LOGICAL_FIELDS]; /* logical fields array for flex
                                           entries. */
    uint32 num_of_fields;               /* number of logical fields. */
    bcm_pbmp_t sa_source_filter_pbmp;   /* Source port filter bitmap for this SA */
    bcm_tsn_flowset_t tsn_flowset;      /* Time-Sensitive Networking: associated
                                           flow set */
    bcm_tsn_sr_flowset_t sr_flowset;    /* Seamless Redundancy: associated flow
                                           set */
    bcm_policer_t policer_id;           /* Base policer to be used */
    bcm_tsn_pri_map_t taf_gate_primap;  /* TAF (Time Aware Filtering) gate
                                           priority mapping */
    uint32 stat_id;                     /* Object statistics ID */
    int stat_pp_profile;                /* Statistics profile */
    uint16 gbp_src_id;                  /* GBP Source ID */
} bcm_compat6519_l2_addr_t;

extern int bcm_compat6519_l2_addr_add(int unit,
                                      bcm_compat6519_l2_addr_t *l2addr);
extern int bcm_compat6519_l2_addr_multi_add(int unit,
                                            bcm_compat6519_l2_addr_t *l2addr, int count);
extern int bcm_compat6519_l2_addr_multi_delete(int unit,
                                               bcm_compat6519_l2_addr_t *l2addr, int count);
extern int bcm_compat6519_l2_addr_get(int unit, bcm_mac_t mac_addr, bcm_vlan_t vid,
                                      bcm_compat6519_l2_addr_t *l2addr);
extern int bcm_compat6519_l2_conflict_get(int unit, bcm_compat6519_l2_addr_t *addr,
                                          bcm_compat6519_l2_addr_t *cf_array, int cf_max,
                                          int *cf_count);
extern int bcm_compat6519_l2_replace(int unit, uint32 flags,
                                     bcm_compat6519_l2_addr_t *match_addr,
                                     bcm_module_t new_module,
                                     bcm_port_t new_port, bcm_trunk_t new_trunk);
extern int bcm_compat6519_l2_replace_match(int unit, uint32 flags,
                                           bcm_compat6519_l2_addr_t *match_addr,
                                           bcm_compat6519_l2_addr_t *mask_addr,
                                           bcm_compat6519_l2_addr_t *replace_addr,
                                           bcm_compat6519_l2_addr_t *replace_mask_addr);
extern int bcm_compat6519_l2_stat_get(int unit,
                                      bcm_compat6519_l2_addr_t *l2addr,
                                      bcm_l2_stat_t stat, uint64 *val);
extern int bcm_compat6519_l2_stat_get32(int unit,
                                        bcm_compat6519_l2_addr_t *l2addr,
                                        bcm_l2_stat_t stat, uint32 *val);
extern int bcm_compat6519_l2_stat_set(int unit,
                                      bcm_compat6519_l2_addr_t *l2addr,
                                      bcm_l2_stat_t stat, uint64 val);
extern int bcm_compat6519_l2_stat_set32(int unit,
                                        bcm_compat6519_l2_addr_t *l2addr,
                                        bcm_l2_stat_t stat, uint32 val);
extern int bcm_compat6519_l2_stat_enable_set(int unit,
                                             bcm_compat6519_l2_addr_t *l2addr,
                                             int enable);
extern int bcm_compat6519_l2_addr_by_struct_get(int unit, bcm_compat6519_l2_addr_t *l2addr);
extern int bcm_compat6519_l2_addr_by_struct_delete(int unit, bcm_compat6519_l2_addr_t *l2addr);

#define BCM_COMPAT6519_FLEXCTR_OPERATION_OBJECT_SIZE   2          /* Operation object array
                                                          size number. */

/*
 * Flex counter index operation structure.
 *
 * This data structure lists the operation of a flex counter index.
 * Flex counter index would be generated as below:
 *      value0 = (SEL(object0) >> shift0) & ((1 << mask_size0) - 1)).
 *      value1 = (SEL(object1) & ((1 << mask_size1) - 1)) << shift1.
 *      index = (value1 | value0).
 */
typedef struct bcm_compat6519_flexctr_index_operation_s {
    bcm_flexctr_object_t object[BCM_COMPAT6519_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Flex counter object array. */
    uint32 quant_id[BCM_COMPAT6519_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Quantization ID. */
    uint8 mask_size[BCM_COMPAT6519_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Object mask size array. */
    uint8 shift[BCM_COMPAT6519_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Object shift array. */
    uint32 object_id[BCM_COMPAT6519_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Flex counter object ID. */
} bcm_compat6519_flexctr_index_operation_t;

/*
 * Flex counter value operation structure.
 *
 * This data structure lists the operation of a flex counter update
 * value.
 * Flex counter update value would be generated as below:
 *      value0 = (SEL(object0) >> shift0) & ((1 << mask_size0) - 1)).
 *      value1 = (SEL(object1) & ((1 << mask_size1) - 1)) << shift1.
 *      value = (value1 | value0).
 *      New_counter = UPDATE_TYPE(old_counter, value).
 */
typedef struct bcm_compat6519_flexctr_value_operation_s {
    bcm_flexctr_value_select_t select;  /* Select counter value. */
    bcm_flexctr_object_t object[BCM_COMPAT6519_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Flex counter object array. */
    uint32 quant_id[BCM_COMPAT6519_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Quantization ID. */
    uint8 mask_size[BCM_COMPAT6519_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Object mask size array. */
    uint8 shift[BCM_COMPAT6519_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Object shift array. */
    bcm_flexctr_value_operation_type_t type; /* Counter value operation type. */
    uint32 object_id[BCM_COMPAT6519_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Flex counter object ID. */
} bcm_compat6519_flexctr_value_operation_t;

/*
 * Flex counter trigger structure.
 *
 * This data structure lists the properties of the flex counter trigger.
 */
typedef struct bcm_compat6519_flexctr_trigger_s {
    bcm_flexctr_trigger_type_t trigger_type; /* Trigger type. */
    uint8 period_num;                   /* Time trigger period number. */
    bcm_flexctr_trigger_interval_t interval; /* Time trigger interval scale. */
    bcm_flexctr_object_t object;        /* Flex counter object. */
    uint16 object_value_start;          /* Conditional trigger start value. */
    uint16 object_value_stop;           /* Conditional trigger stop value. */
    uint16 object_value_mask;           /* Conditional trigger mask. */
    uint16 interval_shift;              /* Time trigger interval shift. */
    uint16 interval_size;               /* Time trigger interval size. */
    uint32 object_id;                   /* Flex counter object ID. */
} bcm_compat6519_flexctr_trigger_t;

/*
 * Flex counter action structure.
 *
 * This data structure lists the properties of a flex counter action.
 */
typedef struct bcm_compat6519_flexctr_action_s {
    uint32 flags;                       /* Action flags. */
    bcm_flexctr_source_t source;        /* Flex counter source. */
    bcm_pbmp_t ports;                   /* Flex counter ports. */
    int hint;                           /* Flex counter hint. */
    bcm_flexctr_drop_count_mode_t drop_count_mode; /* Counter drop mode for functional
                                           packet drops. */
    int exception_drop_count_enable;    /* Enable to count on hardware exception
                                           drops. */
    int egress_mirror_count_enable;     /* Enable to count egress mirrored
                                           packets also. */
    bcm_flexctr_counter_mode_t mode;    /* Action mode. */
    uint32 index_num;                   /* Total counter index number. */
    bcm_compat6519_flexctr_index_operation_t index_operation; /* Flex counter index operation
                                           structure. */
    bcm_compat6519_flexctr_value_operation_t operation_a; /* Flex counter value A operation
                                           structure. */
    bcm_compat6519_flexctr_value_operation_t operation_b; /* Flex counter value B operation
                                           structure. */
    bcm_compat6519_flexctr_trigger_t trigger;      /* Flex counter trigger structure. */
} bcm_compat6519_flexctr_action_t;

/* Flex counter action creation function. */
extern int bcm_compat6519_flexctr_action_create(
    int unit,
    int options,
    bcm_compat6519_flexctr_action_t *action,
    uint32 *stat_counter_id);

/* Flex counter action get function. */
extern int bcm_compat6519_flexctr_action_get(
    int unit,
    uint32 stat_counter_id,
    bcm_compat6519_flexctr_action_t *action);

/* Flowtracker export template elements information. */
typedef struct bcm_compat6519_flowtracker_export_element_info_s {
    uint32 flags;                       /* See
                                           BCM_FLOWTRACKER_EXPORT_ELEMENT_FLAGS_XXX. */
    bcm_flowtracker_export_element_type_t element; /* Type of export element. */
    uint32 data_size;                   /* Size of information element in the
                                           export record in bytes. If the
                                           data_size is given as 0, then the
                                           default RFC size is used. */
    uint16 info_elem_id;                /* Information element to be used, when
                                           the template set is exported. */
    bcm_flowtracker_check_t check_id;   /* Flowtracker Check Id for exporting
                                           check data. */
} bcm_compat6519_flowtracker_export_element_info_t;

/* Validate the template and return the list supported by the device. */
extern int bcm_compat6519_flowtracker_export_template_validate(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    int max_in_export_elements,
    bcm_compat6519_flowtracker_export_element_info_t *in_export_elements,
    int max_out_export_elements,
    bcm_compat6519_flowtracker_export_element_info_t *out_export_elements,
    int *actual_out_export_elements);

/* Create a flowtracker export template. */
extern int bcm_compat6519_flowtracker_export_template_create(
    int unit,
    uint32 options,
    bcm_flowtracker_export_template_t *id,
    uint16 set_id,
    int num_export_elements,
    bcm_compat6519_flowtracker_export_element_info_t *list_of_export_elements);

/* Get a flowtracker export template. */
extern int bcm_compat6519_flowtracker_export_template_get(
    int unit,
    bcm_flowtracker_export_template_t id,
    uint16 *set_id,
    int max_size,
    bcm_compat6519_flowtracker_export_element_info_t *list_of_export_elements,
    int *list_size);

/* Truncate length profile structure. */
typedef struct bcm_compat6519_mirror_truncate_length_profile_s {
    int adjustment_length;    /* Mirror payload adjustment length after truncation. */
} bcm_compat6519_mirror_truncate_length_profile_t;

/* Create the truncate length profile. */
extern int bcm_compat6519_mirror_truncate_length_profile_create(
    int unit,
    uint32 options,
    int *profile_id,
    bcm_compat6519_mirror_truncate_length_profile_t *profile);

/* Get the truncate length profile entry with a given profile id. */
extern int bcm_compat6519_mirror_truncate_length_profile_get(
    int unit,
    int profile_id,
    bcm_compat6519_mirror_truncate_length_profile_t *profile);

/* OAM delay object. */
typedef struct bcm_compat6519_oam_delay_s {
    uint32 flags;    /*  */
    int delay_id;    /* Oam Delay ID. */
    bcm_oam_endpoint_t id;    /* Endpoint ID of Local MEP. */
    bcm_oam_endpoint_t remote_id;    /* Endpoint ID of Remote MEP. */
    int period;    /* Frequency of delay tx period in milliseconds. Zero indicates single shot, -1 indicates disabling delay packets transmission */
    bcm_time_spec_t delay_min;    /* Minimal recorded delay to remote peer. Reset when read. */
    bcm_time_spec_t delay_max;    /* Maximul recorded delay to remote peer. Reset when read. */
    bcm_time_spec_t delay;    /* Last recorded delay to remote peer. */
    bcm_time_spec_t txf;    /* Time of transmit in forward direction. */
    bcm_time_spec_t rxf;    /* Time of reception in forward direction. */
    bcm_time_spec_t txb;    /* Time of transmit in backward direction. */
    bcm_time_spec_t rxb;    /* Time of reception in backward direction. */
    uint8 pkt_pri;    /* Egress marking for outgoing DM messages */
    uint8 pkt_pri_bitmap;    /* Bitmap of packet priorities which should be measured for DM. A value of zero is the equivalent of all ones. */
    bcm_cos_t int_pri;    /* Egress queuing for outgoing DM messages */
    uint32 rx_oam_packets;    /* Count of OAM packets received by this endpoint */
    uint32 tx_oam_packets;    /* Count of OAM packets transmitted by this endpoint */
    bcm_oam_timestamp_format_t timestamp_format;    /* OAM timestamp type */
    bcm_mac_t peer_da_mac_address;    /* MAC DA in DMM injection in case peer endpoint is not configured in remote_id */
    uint32 pm_id;    /* Performance measurement ID in BTE */
    uint32 dm_tx_update_lm_counter_base_id[BCM_OAM_LM_COUNTER_MAX];    /* The base index of the LM counter to be updated by Tx DM packets */
    uint32 dm_tx_update_lm_counter_offset[BCM_OAM_LM_COUNTER_MAX];    /* Offset to the Base LM counter Id to be incremented by Tx DM packets */
    uint32 dm_tx_update_lm_counter_byte_offset[BCM_OAM_LM_COUNTER_MAX];    /* Byte offset in the Tx DM packet from which to increment the LM byte counter */
    uint8 dm_tx_update_lm_counter_size;    /* Number of LM counters to be incremented by Tx DM packets */
    bcm_oam_tlv_t tlvs[BCM_OAM_MAX_NUM_TLVS_FOR_DM];    /* TLV contents of DM */
} bcm_compat6519_oam_delay_t;

/* Add an OAM delay object */
extern int bcm_compat6519_oam_delay_add(
    int unit,
    bcm_compat6519_oam_delay_t *delay_ptr);

/* Get an OAM delay object */
extern int bcm_compat6519_oam_delay_get(
    int unit,
    bcm_compat6519_oam_delay_t *delay_ptr);

/* Delete an OAM delay object */
extern int bcm_compat6519_oam_delay_delete(
    int unit,
    bcm_compat6519_oam_delay_t *delay_ptr);

/* Structure for Time of the day values. */
typedef struct bcm_compat6519_time_tod_s {
    bcm_time_format_t time_format;    /* Time format for counter */
    bcm_time_spec_t ts;    /* time of the day values. */
    uint64 ts_adjustment_counter_ns;    /* tod value adjustment in nanoseconds. */
} bcm_compat6519_time_tod_t;

/* Set time of the day values at various stages in ASIC. */
extern int bcm_compat6519_time_tod_set(
    int unit,
    uint32 stages,
    bcm_compat6519_time_tod_t *tod);

/* Get time of the day values at various stages in ASIC. */
extern int bcm_compat6519_time_tod_get(
    int unit,
    uint32 stages,
    bcm_compat6519_time_tod_t *tod);

#if defined(INCLUDE_L3)
/* L3 tunneling initiator. */
typedef struct bcm_compat6519_tunnel_initiator_s {
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
} bcm_compat6519_tunnel_initiator_t;

/* Set the tunneling initiator parameters on an L3 interface. */
extern int bcm_compat6519_tunnel_initiator_set(
    int unit,
    bcm_l3_intf_t *intf,
    bcm_compat6519_tunnel_initiator_t *tunnel);

/* Set the tunneling initiator parameters on an L3 interface. */
extern int bcm_compat6519_tunnel_initiator_create(
    int unit,
    bcm_l3_intf_t *intf,
    bcm_compat6519_tunnel_initiator_t *tunnel);

/* Get the tunnel property for the given L3 interface. */
extern int bcm_compat6519_tunnel_initiator_get(
    int unit,
    bcm_l3_intf_t *intf,
    bcm_compat6519_tunnel_initiator_t *tunnel);

/* Create VXLAN Tunnel Initiator.  */
extern int bcm_compat6519_vxlan_tunnel_initiator_create(
    int unit,
    bcm_compat6519_tunnel_initiator_t *info);

/* Get  VXLAN Tunnel Initiator.  */
extern int bcm_compat6519_vxlan_tunnel_initiator_get(
    int unit,
    bcm_compat6519_tunnel_initiator_t *info);

/* Set the tunneling initiator parameters for a WLAN tunnel. */
extern int bcm_compat6519_wlan_tunnel_initiator_create(
    int unit,
    bcm_compat6519_tunnel_initiator_t *info);

/* Get the tunnel properties for an outgoing WLAN tunnel. */
extern int bcm_compat6519_wlan_tunnel_initiator_get(
    int unit,
    bcm_compat6519_tunnel_initiator_t *info);

/* Create L2GRE Tunnel Initiator.  */
extern int bcm_compat6519_l2gre_tunnel_initiator_create(
    int unit,
    bcm_compat6519_tunnel_initiator_t *info);

/* Get  L2GRE Tunnel Initiator.  */
extern int bcm_compat6519_l2gre_tunnel_initiator_get(
    int unit,
    bcm_compat6519_tunnel_initiator_t *info);

/* L3 tunneling terminator. */
typedef struct bcm_compat6519_tunnel_terminator_s {
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
} bcm_compat6519_tunnel_terminator_t;

/* Add a tunnel terminator for DIP-SIP key. */
extern int bcm_compat6519_tunnel_terminator_add(
    int unit,
    bcm_compat6519_tunnel_terminator_t *info);

/* Add a tunnel terminator for DIP-SIP key. */
extern int bcm_compat6519_tunnel_terminator_create(
    int unit,
    bcm_compat6519_tunnel_terminator_t *info);

/* Delete a tunnel terminator for DIP-SIP key. */
extern int bcm_compat6519_tunnel_terminator_delete(
    int unit,
    bcm_compat6519_tunnel_terminator_t *info);

/* Update a tunnel terminator for DIP-SIP key. */
extern int bcm_compat6519_tunnel_terminator_update(
    int unit,
    bcm_compat6519_tunnel_terminator_t *info);

/* Get a tunnel terminator for DIP-SIP key. */
extern int bcm_compat6519_tunnel_terminator_get(
    int unit,
    bcm_compat6519_tunnel_terminator_t *info);

/* Attach counters entries to the given tunnel. */
extern int bcm_compat6519_tunnel_terminator_stat_attach(
    int unit,
    bcm_compat6519_tunnel_terminator_t *terminator,
    uint32 stat_counter_id);

/* Detach counters entries to the given tunnel. */
extern int bcm_compat6519_tunnel_terminator_stat_detach(
    int unit,
    bcm_compat6519_tunnel_terminator_t *terminator);

/* Get stat counter id associated with given tunnel. */
extern int bcm_compat6519_tunnel_terminator_stat_id_get(
    int unit,
    bcm_compat6519_tunnel_terminator_t *terminator,
    uint32 *stat_counter_id);

/* Set flex counter object value for the given tunnel. */
extern int bcm_compat6519_tunnel_terminator_flexctr_object_set(
    int unit,
    bcm_compat6519_tunnel_terminator_t *terminator,
    uint32 value);

/* Get flex counter object value for the given tunnel. */
extern int bcm_compat6519_tunnel_terminator_flexctr_object_get(
    int unit,
    bcm_compat6519_tunnel_terminator_t *terminator,
    uint32 *value);

/* Create VXLAN Tunnel terminator. */
extern int bcm_compat6519_vxlan_tunnel_terminator_create(
    int unit,
    bcm_compat6519_tunnel_terminator_t *info);

/* Update multicast/active state per VXLAN Tunnel terminator. */
extern int bcm_compat6519_vxlan_tunnel_terminator_update(
    int unit,
    bcm_compat6519_tunnel_terminator_t *info);

/* Get VXLAN Tunnel terminator. */
extern int bcm_compat6519_vxlan_tunnel_terminator_get(
    int unit,
    bcm_compat6519_tunnel_terminator_t *info);

/* Create L2GRE Tunnel terminator. */
extern int bcm_compat6519_l2gre_tunnel_terminator_create(
    int unit,
    bcm_compat6519_tunnel_terminator_t *info);

/* Update L2GRE Tunnel terminator. */
extern int bcm_compat6519_l2gre_tunnel_terminator_update(
    int unit,
    bcm_compat6519_tunnel_terminator_t *info);

/* Get L2GRE Tunnel terminator. */
extern int bcm_compat6519_l2gre_tunnel_terminator_get(
    int unit,
    bcm_compat6519_tunnel_terminator_t *info);

#endif /* defined(INCLUDE_L3) */


/* PP statistic profile. */
typedef struct bcm_compat6519_stat_pp_profile_info_s {
    int counter_command_id;    /* Command id (interface id). */
    int stat_object_type;    /* Statistic object type in range 0-BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES-1 */
    int meter_command_id;    /* Relevant only for egress */
    int meter_qos_map_id;    /* Relevant only for egress */
    uint8 is_meter_enable;    /* Allow metering */
    uint8 is_fp_cs_var;    /* Enable PMF2 context selection */
    int ingress_tunnel_metadata_size;    /* Ingress metadata size in buffer (in bits) for terminated headers */
    int ingress_tunnel_metadata_start_bit;    /* Ingress metadata start bit in buffer for terminated headers */
    int ingress_forward_metadata_size;    /* Ingress metadata size in buffer (in bits) for forwarding header */
    int ingress_forward_metadata_start_bit;    /* Ingress metadata start bit in buffer for forwarding header */
    int ingress_fwd_plus_one_metadata_size;    /* Ingress metadata size in buffer (in bits) for forwarding plus one header */
    int ingress_fwd_plus_one_metadata_start_bit;    /* Ingress metadata start bit in buffer for forwarding plus one header */
} bcm_compat6519_stat_pp_profile_info_t;

/* Map {statistic command, statistic profile} to the profile properties */
extern int bcm_compat6519_stat_pp_profile_create(
    int unit,
    int flags,
    bcm_stat_counter_interface_type_t engine_source,
    int *stat_pp_profile,
    bcm_compat6519_stat_pp_profile_info_t *stat_pp_profile_info);

/* Get profile properties according to stat_pp_profile */
extern int bcm_compat6519_stat_pp_profile_get(
    int unit,
    int stat_pp_profile,
    bcm_compat6519_stat_pp_profile_info_t *stat_pp_profile_info);

/* This structure contain the latency data that was measured in the ingress for a packet */
typedef struct bcm_compat6519_cosq_max_latency_pkts_s {
    bcm_gport_t dest_gport;    /* will return modport gport */
    bcm_cos_queue_t cosq;    /* cosq */
    uint64 latency;    /* latency measured */
    uint32 latency_flow;    /* latency flow */
} bcm_compat6519_cosq_max_latency_pkts_t;

/* get cosq ingress congestion resource statistics */
extern int bcm_compat6519_cosq_max_latency_pkts_get(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    int max_count,
    bcm_compat6519_cosq_max_latency_pkts_t *max_latency_pkts,
    int *actual_count);

/* Structure holds configuration per policer database. */
typedef struct bcm_compat6519_policer_database_config_s {
    int expansion_enable;    /* Allowed to ingress database 0. if enabled, expansion size, filter and offsets will be  made according API bcm_policer_expansion_groups_set. */
    int is_single_bucket;    /* Single or dual bucket mode. */
} bcm_compat6519_policer_database_config_t;

/* API create a policer database (sw create). database represent the meter processor belong to one of the meter commands.It gets as input database handler which gather the key parameters: core, scope, database_id */
extern int bcm_compat6519_policer_database_create(
    int unit,
    int flags,
    int policer_database_handle,
    bcm_compat6519_policer_database_config_t *config);

/* Structure holds the policer expansion groups, according the forwarding types. */
typedef struct bcm_compat6519_policer_expansion_group_s {
    bcm_policer_expansion_group_types_config_t config[bcmPolicerFwdTypeMax];    /*  */
} bcm_compat6519_policer_expansion_group_t;

/* API Defines groups of L2 Fwd-Types and decide if to count them and in which offset in the expansion to count them. */
extern int bcm_compat6519_policer_expansion_groups_set(
    int unit,
    int flags,
    bcm_core_t core_id,
    bcm_compat6519_policer_expansion_group_t *expansion_group);

/* API gets the expansion groups of L2 Fwd-Types. */
extern int bcm_compat6519_policer_expansion_groups_get(
    int unit,
    int flags,
    bcm_core_t core_id,
    bcm_compat6519_policer_expansion_group_t *expansion_group);

/* Flowtracker check information. */
typedef struct bcm_compat6519_flowtracker_check_info_s {
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
} bcm_compat6519_flowtracker_check_info_t;

/* Create a flowtracker flow group. */
extern int bcm_compat6519_flowtracker_check_create(
    int unit,
    uint32 options,
    bcm_compat6519_flowtracker_check_info_t check_info,
    bcm_flowtracker_check_t *check_id);

/* Get a flowtracker flow check. */
extern int bcm_compat6519_flowtracker_check_get(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_compat6519_flowtracker_check_info_t *check_info);

/*
 * Group configuration structure. Used to create a field group with
 * specific attributes.
 */
typedef struct bcm_compat6519_field_group_config_s {
    uint32 flags;                       /* Group create flags
                                           BCM_FIELD_GROUP_CREATE_XXX. */
    bcm_field_qset_t qset;              /* Group qualifier set. */
    int priority;                       /* Group priority. */
    bcm_field_group_mode_t mode;        /* Group mode. */
    bcm_pbmp_t ports;                   /* Group member ports. */
    bcm_field_group_t group;            /* Group. */
    bcm_field_aset_t aset;              /* Group action set */
    bcm_field_presel_set_t preselset;   /* Group preselector set */
    bcm_field_presel_set_t group_ref;   /* Reference Field group for operation
                                           set by the Flag. */
    int max_entry_priorities;           /* Number of entries priorities in the
                                           group. 0 means unlimited. */
    bcm_field_hintid_t hintid;          /* Hints for Group Creation. */
    int action_res_id;                  /* Action resolution id for Group. */
    uint8 name[BCM_FIELD_MAX_NAME_LEN]; /* Name for Group. */
    bcm_field_group_cycle_t cycle;      /* Create Field Group Cycle. */
    uint32 pgm_bmp;                     /* Bitmap of program-ids which the
                                           current field group will be pointing
                                           to. */
    uint32 share_id;                    /* Group share ID. Groups created with
                                           same share ID may share TCAM banks. */
    bcm_field_presel_group_t presel_group; /* Presel Group ID. */
} bcm_compat6519_field_group_config_t;

/* Create a field group with specific attributes. */
extern int bcm_compat6519_field_group_config_create(
    int unit,
    bcm_compat6519_field_group_config_t *group_config);

/* To verify whether a group can be created with a given qset and mode */
extern int bcm_compat6519_field_group_config_validate(
    int unit,
    bcm_compat6519_field_group_config_t *group_config,
    bcm_field_group_mode_t *mode);

/* Port Configuration structure. */
typedef struct bcm_compat6519_port_config_s {
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
} bcm_compat6519_port_config_t;

/* Retrieved the port configuration for the specified device. */
extern int bcm_compat6519_port_config_get(
    int unit,
    bcm_compat6519_port_config_t *config);

#endif /* BCM_RPC_SUPPORT */
#endif /* !_COMPAT_6519_H */
