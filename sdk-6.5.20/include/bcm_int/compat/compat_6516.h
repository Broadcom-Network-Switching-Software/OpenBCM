/*
 * $Id: compat_6516.h,v 1.0 2019/04/03
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-6.5.16 routines
 */

#ifndef _COMPAT_6516_H_
#define _COMPAT_6516_H_

#ifdef BCM_RPC_SUPPORT
#include <bcm_int/compat/compat_6519.h>
#include <bcm_int/compat/compat_6518.h>
#include <bcm/types.h>
#include <bcm/trunk.h>
#include <bcm/flow.h>
#include <bcm/ecn.h>
#if defined(INCLUDE_L3)
#include <bcm/l3.h>
#include <bcm/tunnel.h>
#include <bcm/ipmc.h>
#include <bcm/failover.h>
#include <bcm/mim.h>
#include <bcm/vxlan.h>
#endif /* INCLUDE_L3 */
#include <bcm/vlan.h>
#include <bcm/field.h>
#include <bcm/fcoe.h>
#include <bcm/init.h>
#include <bcm/l2.h>
#include <bcm/port.h>
#include <bcm_int/compat/compat_6517.h>

/* Flowtracker collector information. */
typedef struct bcm_compat6516_flowtracker_collector_info_s {
    bcm_flowtracker_collector_type_t collector_type; /* Flowtracker Collector type. Remote vs
                                                        Local */
    bcm_flowtracker_collector_transport_type_t transport_type; /* Transport type used for exporting
                                                                  flow data to the collector. This
                                                                  identifies the usable fields within
                                                                  the encap structure member defined
                                                                  below. */
    bcm_flowtracker_collector_eth_header_t eth; /* Ethernet encapsulation of the packet
                                                   sent to collector. */
    bcm_flowtracker_collector_ipv4_header_t ipv4; /* IPv4 encapsulation of the packet sent
                                                     to collector. */
    bcm_flowtracker_collector_ipv6_header_t ipv6; /* IPv6 encapsulation of the packet sent
                                                     to collector. */
    bcm_flowtracker_collector_udp_header_t udp; /* UDP encapsulation of the packet sent
                                                   to collector. */
    uint16 max_packet_length;           /* The maximum packet length of an
                                           export packet that can be sent to
                                           this collector. */
} bcm_compat6516_flowtracker_collector_info_t;


/* Create a flowtracker collector with given collector info. */
extern int bcm_compat6516_flowtracker_collector_create(
        int unit,
        uint32 options,
        bcm_flowtracker_collector_t *collector_id,
        bcm_compat6516_flowtracker_collector_info_t *collector_info);

/* Get flowtracker collector information with ID. */
extern int bcm_compat6516_flowtracker_collector_get(
        int unit,
        bcm_flowtracker_collector_t id,
        bcm_compat6516_flowtracker_collector_info_t *collector_info);


/* Flowtracker check information. */
typedef struct bcm_compat6516_flowtracker_check_info_s {
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
    bcm_flowtracker_check_operation_t operation; /* Operation to be performed for this
                                                    check. */
    bcm_flowtracker_check_t primary_check_id; /* primary check id to associate second
                                                 check for aggregated checks on same
                                                 flow. */
} bcm_compat6516_flowtracker_check_info_t;

/* Create Flow check and return software ID. */
extern int bcm_compat6516_flowtracker_check_create(
        int unit,
        uint32 options,
        bcm_compat6516_flowtracker_check_info_t check_info,
        bcm_flowtracker_check_t *check_id);

/* Get information of flow check. */
extern int bcm_compat6516_flowtracker_check_get(
        int unit,
        bcm_flowtracker_check_t check_id,
        bcm_compat6516_flowtracker_check_info_t *check_info);


/* Actions related information for Flow check. */
typedef struct bcm_compat6516_flowtracker_check_action_info_s {
    uint32 flags;                       /* Action flags. */
    bcm_flowtracker_tracking_param_type_t param; /* The attribute of flow on which the
                                                    check is performed. */
    bcm_flowtracker_check_action_t action; /* Action to be performed if check
                                              passes. */
} bcm_compat6516_flowtracker_check_action_info_t;

/* Set action information of flow check. */
extern int bcm_compat6516_flowtracker_check_action_info_set(
        int unit,
        bcm_flowtracker_check_t check_id,
        bcm_compat6516_flowtracker_check_action_info_t info);

/* Get action information of flow check. */
extern int bcm_compat6516_flowtracker_check_action_info_get(
        int unit,
        bcm_flowtracker_check_t check_id,
        bcm_compat6516_flowtracker_check_action_info_t *info);


/* Flowtracker tracking parameter information. */
typedef struct bcm_compat6516_flowtracker_tracking_param_info_s {
    uint32 flags;                       /* Flags for tracking parameters. */
    bcm_flowtracker_tracking_param_type_t param; /* Type of tracking parameter. */
    bcm_flowtracker_tracking_param_user_data_t tracking_data; /* Element data for the tracking param
                                                                 to be used to add a flow. */
    bcm_flowtracker_tracking_param_mask_t mask; /* Mask to select granular information
                                                   from tracking parameter. By default,
                                                   mask is set to all enabled. */
    bcm_flowtracker_check_t check_id;   /* Flowtracker check to be used to
                                           tracking flow check data. */
    bcm_udf_id_t udf_id;                /* UDF Id associated with the tracking
                                           param. */
} bcm_compat6516_flowtracker_tracking_param_info_t;

/* Set tracking parameter for this flowtracker group. */
extern int bcm_compat6516_flowtracker_group_tracking_params_set(
        int unit,
        bcm_flowtracker_group_t id,
        int num_tracking_params,
        bcm_compat6516_flowtracker_tracking_param_info_t *list_of_tracking_params);

/* Get flowtracker tracking parameters for this group. */
extern int bcm_compat6516_flowtracker_group_tracking_params_get(
        int unit,
        bcm_flowtracker_group_t id,
        int max_size,
        bcm_compat6516_flowtracker_tracking_param_info_t *list_of_tracking_params,
        int *list_size);

/*
 * Add a user flow entry basis user input key elements. API expects that
 * all tracking parametrs of type = 'KEY' in the group are specified.
 */
extern int bcm_compat6516_flowtracker_user_entry_add(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        uint32 options,
        int num_user_entry_params,
        bcm_compat6516_flowtracker_tracking_param_info_t *user_entry_param_list,
        bcm_flowtracker_user_entry_handle_t *entry_handle);

/* Fetch user entry info given the entry handle. */
extern int bcm_compat6516_flowtracker_user_entry_get(
        int unit,
        bcm_flowtracker_user_entry_handle_t *entry_handle,
        int num_user_entry_params,
        bcm_compat6516_flowtracker_tracking_param_info_t *user_entry_param_list,
        int *actual_user_entry_params);



/* Collector information. */
typedef struct bcm_compat6516_collector_info_s {
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
    bcm_collector_ipfix_header_t ipfix; /* IPFIX encapsulation of the packet
                                           sent to collector. */
    bcm_collector_protobuf_header_t protobuf; /* Protobuf header information. */
} bcm_compat6516_collector_info_t;

/* Create a collector with given collector info. */
extern int bcm_compat6516_collector_create(
        int unit,
        uint32 options,
        bcm_collector_t *collector_id,
        bcm_compat6516_collector_info_t *collector_info);

/* Get collector information with ID. */
extern int bcm_compat6516_collector_get(
        int unit,
        bcm_collector_t id,
        bcm_compat6516_collector_info_t *collector_info);

/* L2 Station address info. */
typedef struct bcm_compat6516_l2_station_s {
    uint32 flags;                       /* BCM_L2_STATION_xxx flags. */
    int priority;                       /* Entry priority. */
    bcm_mac_t dst_mac;                  /* Destination MAC address to match. */
    bcm_mac_t dst_mac_mask;             /* Destination MAC address mask value. */
    bcm_vlan_t vlan;                    /* VLAN to match. */
    bcm_vlan_t vlan_mask;               /* VLAN mask value. */
    bcm_port_t src_port;                /* Ingress port to match. */
    bcm_port_t src_port_mask;           /* Ingress port mask value. */
    bcm_tsn_pri_map_t taf_gate_primap;  /* TAF (Time Aware Filtering) gate
                                           priority mapping */
} bcm_compat6516_l2_station_t;

/* Add an entry to L2 Station Table. */
int bcm_compat6516_l2_station_add(
        int unit,
        int *station_id,
        bcm_compat6516_l2_station_t *station);

/* Get L2 station entry detail from Station Table. */
int bcm_compat6516_l2_station_get(
        int unit,
        int station_id,
        bcm_compat6516_l2_station_t *station);

/* Device-independent L2 address structure. */
typedef struct bcm_compat6516_l2_addr_s {
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
    bcm_multicast_t l2mc_group;         /* XGS: index in L2MC table. */
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
} bcm_compat6516_l2_addr_t;

extern int bcm_compat6516_l2_addr_add(int unit,
                                      bcm_compat6516_l2_addr_t *l2addr);
extern int bcm_compat6516_l2_addr_multi_add(int unit,
                                            bcm_compat6516_l2_addr_t *l2addr, int count);
extern int bcm_compat6516_l2_addr_multi_delete(int unit,
                                               bcm_compat6516_l2_addr_t *l2addr, int count);
extern int bcm_compat6516_l2_addr_get(int unit, bcm_mac_t mac_addr, bcm_vlan_t vid,
                                      bcm_compat6516_l2_addr_t *l2addr);
extern int bcm_compat6516_l2_conflict_get(int unit, bcm_compat6516_l2_addr_t *addr,
                                          bcm_compat6516_l2_addr_t *cf_array, int cf_max,
                                          int *cf_count);
extern int bcm_compat6516_l2_replace(int unit, uint32 flags,
                                     bcm_compat6516_l2_addr_t *match_addr,
                                     bcm_module_t new_module,
                                     bcm_port_t new_port, bcm_trunk_t new_trunk);
extern int bcm_compat6516_l2_replace_match(int unit, uint32 flags,
                                           bcm_compat6516_l2_addr_t *match_addr,
                                           bcm_compat6516_l2_addr_t *mask_addr,
                                           bcm_compat6516_l2_addr_t *replace_addr,
                                           bcm_compat6516_l2_addr_t *replace_mask_addr);
extern int bcm_compat6516_l2_stat_get(int unit,
                                      bcm_compat6516_l2_addr_t *l2addr,
                                      bcm_l2_stat_t stat, uint64 *val);
extern int bcm_compat6516_l2_stat_get32(int unit,
                                        bcm_compat6516_l2_addr_t *l2addr,
                                        bcm_l2_stat_t stat, uint32 *val);
extern int bcm_compat6516_l2_stat_set(int unit,
                                      bcm_compat6516_l2_addr_t *l2addr,
                                      bcm_l2_stat_t stat, uint64 val);
extern int bcm_compat6516_l2_stat_set32(int unit,
                                        bcm_compat6516_l2_addr_t *l2addr,
                                        bcm_l2_stat_t stat, uint32 val);
extern int bcm_compat6516_l2_stat_enable_set(int unit,
                                             bcm_compat6516_l2_addr_t *l2addr,
                                             int enable);

/* Trunk group attributes structure. */
typedef struct bcm_compat6516_trunk_info_s {
    uint32 flags;                       /* BCM_TRUNK_FLAG_xxx. */
    int psc;                            /* Port selection criteria. */
    bcm_trunk_psc_profile_info_t psc_info; /* Port selection criteria information. */
    int ipmc_psc;                       /* Port selection criteria for software
                                           IPMC trunk resolution. */
    int dlf_index;                      /* DLF/broadcast port for trunk group. */
    int mc_index;                       /* Multicast port for trunk group. */
    int ipmc_index;                     /* IPMC port for trunk group. */
    int dynamic_size;                   /* Number of flows for dynamic load
                                           balancing. Valid values are 512, 1k,
                                           doubling up to 32k */
    int dynamic_age;                    /* Inactivity duration, in microseconds. */
    int dynamic_load_exponent;          /* The exponent used in the
                                           exponentially weighted moving average
                                           calculation of historical member
                                           load. */
    int dynamic_expected_load_exponent; /* The exponent used in the
                                           exponentially weighted moving average
                                           calculation of historical expected
                                           member load. */
    bcm_trunk_t master_tid;             /* Master trunk id. */
} bcm_compat6516_trunk_info_t;

/* Get the current parameters for the specified trunk group. */
extern int bcm_compat6516_trunk_get(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6516_trunk_info_t *t_data,
    int member_max,
    bcm_trunk_member_t *member_array,
    int *member_count);

/* Set member ports of a trunk group. */
extern int bcm_compat6516_trunk_set(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6516_trunk_info_t *trunk_info,
    int member_count,
    bcm_trunk_member_t *member_array);

/* ECN traffic map info. */
typedef struct bcm_compat6516_ecn_traffic_map_info_t {
    uint32 flags;   /* BCM_ECN_TRAFFIC_MAP_XXX flag definitions. */
    uint8 ecn;      /* ECN value of packet's IP header. */
    int int_cn;     /* Mapped internal congestion notification(int_cn) value. */
} bcm_compat6516_ecn_traffic_map_info_t;

extern int bcm_compat6516_ecn_traffic_map_get(
    int unit,
    bcm_compat6516_ecn_traffic_map_info_t *map);

extern int bcm_compat6516_ecn_traffic_map_set(
    int unit,
    bcm_compat6516_ecn_traffic_map_info_t *map);

#if defined(INCLUDE_L3)
/* L3 tunneling initiator. */
typedef struct bcm_compat6516_tunnel_initiator_s {
    uint32 flags;                       /* Configuration flags. */
    bcm_tunnel_type_t type;             /* Tunnel type. */
    int ttl;                            /* Tunnel header TTL. */
    bcm_mac_t dmac;                     /* Destination MAC address. */
    bcm_ip_t dip;                       /* Tunnel header DIP (IPv4). */
    bcm_ip_t sip;                       /* Tunnel header SIP (IPv4). */
    bcm_ip6_t sip6;                     /* Tunnel header SIP (IPv6). */
    bcm_ip6_t dip6;                     /* Tunnel header DIP (IPv6). */
    uint32 flow_label;                  /* Tunnel header flow label (IPv6). */
    bcm_tunnel_dscp_select_t dscp_sel;  /* Tunnel header DSCP select. */
    int dscp;                           /* Tunnel header DSCP value. */
    int dscp_map;                       /* DSCP-map ID. */
    bcm_gport_t tunnel_id;              /* Tunnel ID */
    uint16 udp_dst_port;                /* Destination UDP port */
    uint16 udp_src_port;                /* Source UDP port */
    bcm_mac_t smac;                     /* WLAN outer MAC */
    int mtu;                            /* WLAN MTU */
    bcm_vlan_t vlan;                    /* Tunnel VLAN */
    uint16 tpid;                        /* Tunnel TPID */
    uint8 pkt_pri;                      /* Tunnel priority */
    uint8 pkt_cfi;                      /* Tunnel CFI */
    uint16 ip4_id;                      /* IPv4 ID. */
    bcm_if_t l3_intf_id;                /* l3 Interface ID. */
    uint16 span_id;                     /* Erspan Span ID. */
    uint32 aux_data;                    /* Tunnel associated data. */
    int outlif_counting_profile;        /* Out LIF counting profile */
    bcm_encap_access_t encap_access;    /* Encapsulation Access stage */
    uint8 hw_id;                        /* Unique identifier of an ERSPAN engine
                                           within a system */
    uint16 switch_id;                   /* Identifies a source switch at the
                                           receiving end */
    uint16 class_id;                    /* Class ID of the RSPAN advanced tunnel */
    bcm_qos_egress_model_t egress_qos_model; /* egress qos and ttl model */
    int qos_map_id;                     /* general remark profile */
} bcm_compat6516_tunnel_initiator_t;

extern int bcm_compat6516_l2gre_tunnel_initiator_create(
    int unit,
    bcm_compat6516_tunnel_initiator_t *info);

extern int bcm_compat6516_l2gre_tunnel_initiator_get(
    int unit,
    bcm_compat6516_tunnel_initiator_t *info);

extern int bcm_compat6516_tunnel_initiator_set(
    int unit,
    bcm_compat6517_l3_intf_t *intf,
    bcm_compat6516_tunnel_initiator_t *tunnel);

extern int bcm_compat6516_tunnel_initiator_create(
    int unit,
    bcm_compat6517_l3_intf_t *intf,
    bcm_compat6516_tunnel_initiator_t *tunnel);

extern int bcm_compat6516_tunnel_initiator_get(
    int unit,
    bcm_compat6517_l3_intf_t *intf,
    bcm_compat6516_tunnel_initiator_t *tunnel);

extern int bcm_compat6516_vxlan_tunnel_initiator_create(
    int unit,
    bcm_compat6516_tunnel_initiator_t *info);

extern int bcm_compat6516_vxlan_tunnel_initiator_get(
    int unit,
    bcm_compat6516_tunnel_initiator_t *info);

extern int bcm_compat6516_wlan_tunnel_initiator_create(
    int unit,
    bcm_compat6516_tunnel_initiator_t *info);

extern int bcm_compat6516_wlan_tunnel_initiator_get(
    int unit,
    bcm_compat6516_tunnel_initiator_t *info);

typedef struct bcm_compat6516_flow_port_encap_s {
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
} bcm_compat6516_flow_port_encap_t;

extern int bcm_compat6516_flow_port_encap_set(
    int unit,
    bcm_compat6516_flow_port_encap_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcm_compat6516_flow_port_encap_get(
    int unit,
    bcm_compat6516_flow_port_encap_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

typedef struct bcm_compat6516_flow_tunnel_initiator_s {
    uint32 flags;                       /* Configuration flags. */
    bcm_tunnel_type_t type;             /* Tunnel type. */
    int ttl;                            /* Tunnel header TTL. */
    bcm_mac_t dmac;                     /* Destination MAC address. */
    bcm_ip_t dip;                       /* Tunnel header DIP (IPv4). */
    bcm_ip_t sip;                       /* Tunnel header SIP (IPv4). */
    bcm_ip6_t sip6;                     /* Tunnel header SIP (IPv6). */
    bcm_ip6_t dip6;                     /* Tunnel header DIP (IPv6). */
    uint32 flow_label;                  /* Tunnel header flow label (IPv6). */
    bcm_tunnel_dscp_select_t dscp_sel;  /* Tunnel header DSCP select. */
    int dscp;                           /* Tunnel header DSCP value. */
    int dscp_map;                       /* DSCP-map ID. */
    bcm_gport_t tunnel_id;              /* Tunnel ID */
    uint16 udp_dst_port;                /* Destination UDP port */
    uint16 udp_src_port;                /* Source UDP port */
    bcm_mac_t smac;                     /* Src MAC */
    bcm_vlan_t vlan;                    /* Tunnel VLAN */
    uint16 tpid;                        /* Tunnel TPID */
    uint8 pkt_pri;                      /* Tunnel priority */
    uint8 pkt_cfi;                      /* Tunnel CFI */
    uint16 ip4_id;                      /* IPv4 ID. */
    bcm_if_t l3_intf_id;                /* l3 Interface ID. */
    bcm_gport_t flow_port;              /* Flow port ID */
    uint32 valid_elements;              /* bitmap of valid fields */
    uint32 flow_handle;                 /* flow handle */
    uint32 flow_option;                 /* flow option */
} bcm_compat6516_flow_tunnel_initiator_t;

extern int bcm_compat6516_flow_tunnel_initiator_create(
    int unit,
    bcm_compat6516_flow_tunnel_initiator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcm_compat6516_flow_tunnel_initiator_get(
    int unit,
    bcm_compat6516_flow_tunnel_initiator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

typedef struct bcm_compat6516_flow_match_config_s {
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
} bcm_compat6516_flow_match_config_t;

extern int bcm_compat6516_flow_match_add(
    int unit,
    bcm_compat6516_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcm_compat6516_flow_match_delete(
    int unit,
    bcm_compat6516_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcm_compat6516_flow_match_get(
    int unit,
    bcm_compat6516_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

typedef struct bcm_compat6516_l3_egress_s {
    uint32 flags;                       /* Interface flags (BCM_L3_TGID,
                                           BCM_L3_L2TOCPU). */
    uint32 flags2;                      /* See BCM_L3_FLAGS2_xxx flag
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
} bcm_compat6516_l3_egress_t;

extern int bcm_compat6516_l3_egress_create(
    int unit,
    uint32 flags,
    bcm_compat6516_l3_egress_t *egr,
    bcm_if_t *if_id);

extern int bcm_compat6516_l3_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6516_l3_egress_t *egr);

extern int bcm_compat6516_l3_egress_find(
    int unit,
    bcm_compat6516_l3_egress_t *egr,
    bcm_if_t *intf);

extern int bcm_compat6516_failover_egress_set(
    int unit,
    bcm_if_t intf,
    bcm_compat6516_l3_egress_t *failover_egr);

extern int bcm_compat6516_failover_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6516_l3_egress_t *failover_egr);

typedef struct bcm_compat6516_l3_host_s {
    uint32 l3a_flags;                   /* See BCM_L3_xxx flag definitions. */
    uint32 l3a_flags2;                  /* See BCM_L3_FLAGS2_xxx flag
                                           definitions. */
    bcm_vrf_t l3a_vrf;                  /* Virtual router instance. */
    bcm_ip_t l3a_ip_addr;               /* Destination host IP address (IPv4). */
    bcm_ip6_t l3a_ip6_addr;             /* Destination host IP address (IPv6). */
    bcm_cos_t l3a_pri;                  /* New priority in packet. */
    bcm_if_t l3a_intf;                  /* L3 intf associated with this address. */
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
} bcm_compat6516_l3_host_t;

extern int bcm_compat6516_l3_host_find(
    int unit,
    bcm_compat6516_l3_host_t *info);

extern int bcm_compat6516_l3_host_add(
    int unit,
    bcm_compat6516_l3_host_t *info);

extern int bcm_compat6516_l3_host_delete(
    int unit,
    bcm_compat6516_l3_host_t *ip_addr);

extern int bcm_compat6516_l3_host_delete_by_interface(
    int unit,
    bcm_compat6516_l3_host_t *info);

extern int bcm_compat6516_l3_host_delete_all(
    int unit,
    bcm_compat6516_l3_host_t *info);

extern int bcm_compat6516_l3_host_stat_attach(
    int unit,
    bcm_compat6516_l3_host_t *info,
    uint32 stat_counter_id);

extern int bcm_compat6516_l3_host_stat_detach(
    int unit,
    bcm_compat6516_l3_host_t *info);

extern int bcm_compat6516_l3_host_stat_counter_get(
    int unit,
    bcm_compat6516_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

extern int bcm_compat6516_l3_host_stat_counter_sync_get(
    int unit,
    bcm_compat6516_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

extern int bcm_compat6516_l3_host_stat_counter_set(
    int unit,
    bcm_compat6516_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

extern int bcm_compat6516_l3_host_stat_id_get(
    int unit,
    bcm_compat6516_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 *stat_counter_id);

typedef struct bcm_compat6516_l3_route_s {
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
} bcm_compat6516_l3_route_t;

extern int bcm_compat6516_l3_host_delete_by_network(
    int unit,
    bcm_compat6516_l3_route_t *ip_addr);

extern int bcm_compat6516_l3_route_add(
    int unit,
    bcm_compat6516_l3_route_t *info);

extern int bcm_compat6516_l3_route_delete(
    int unit,
    bcm_compat6516_l3_route_t *info);

extern int bcm_compat6516_l3_route_delete_by_interface(
    int unit,
    bcm_compat6516_l3_route_t *info);

extern int bcm_compat6516_l3_route_delete_all(
    int unit,
    bcm_compat6516_l3_route_t *info);

extern int bcm_compat6516_l3_route_get(
    int unit,
    bcm_compat6516_l3_route_t *info);

extern int bcm_compat6516_l3_route_multipath_get(
    int unit,
    bcm_compat6516_l3_route_t *the_route,
    bcm_compat6516_l3_route_t *path_array,
    int max_path,
    int *path_count);

extern int bcm_compat6516_l3_route_stat_get(
    int unit,
    bcm_compat6516_l3_route_t *route,
    bcm_l3_stat_t stat,
    uint64 *val);

extern int bcm_compat6516_l3_route_stat_get32(
    int unit,
    bcm_compat6516_l3_route_t *route,
    bcm_l3_stat_t stat,
    uint32 *val);

extern int bcm_compat6516_l3_route_stat_set(
    int unit,
    bcm_compat6516_l3_route_t *route,
    bcm_l3_stat_t stat,
    uint64 val);

extern int bcm_compat6516_l3_route_stat_set32(
    int unit,
    bcm_compat6516_l3_route_t *route,
    bcm_l3_stat_t stat,
    uint32 val);

extern int bcm_compat6516_l3_route_stat_enable_set(
    int unit,
    bcm_compat6516_l3_route_t *route,
    int enable);

extern int bcm_compat6516_l3_route_stat_attach(
    int unit,
    bcm_compat6516_l3_route_t *info,
    uint32 stat_counter_id);

extern int bcm_compat6516_l3_route_stat_detach(
    int unit,
    bcm_compat6516_l3_route_t *info);

extern int bcm_compat6516_l3_route_stat_counter_get(
    int unit,
    bcm_compat6516_l3_route_t *info,
    bcm_l3_route_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

extern int bcm_compat6516_l3_route_stat_counter_sync_get(
    int unit,
    bcm_compat6516_l3_route_t *info,
    bcm_l3_route_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

extern int bcm_compat6516_l3_route_stat_counter_set(
    int unit,
    bcm_compat6516_l3_route_t *info,
    bcm_l3_route_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

extern int bcm_compat6516_l3_route_stat_multi_get(
    int unit,
    bcm_compat6516_l3_route_t *info,
    int nstat,
    bcm_l3_route_stat_t *stat_arr,
    uint64 *value_arr);

extern int bcm_compat6516_l3_route_stat_multi_get32(
    int unit,
    bcm_compat6516_l3_route_t *info,
    int nstat,
    bcm_l3_route_stat_t *stat_arr,
    uint32 *value_arr);

extern int bcm_compat6516_l3_route_stat_multi_set(
    int unit,
    bcm_compat6516_l3_route_t *info,
    int nstat,
    bcm_l3_route_stat_t *stat_arr,
    uint64 *value_arr);

extern int bcm_compat6516_l3_route_stat_multi_set32(
    int unit,
    bcm_compat6516_l3_route_t *info,
    int nstat,
    bcm_l3_route_stat_t *stat_arr,
    uint32 *value_arr);

extern int bcm_compat6516_l3_route_stat_id_get(
    int unit,
    bcm_compat6516_l3_route_t *info,
    bcm_l3_route_stat_t stat,
    uint32 *stat_counter_id);

extern int bcm_compat6516_l3_route_find(
    int unit,
    bcm_compat6516_l3_host_t *host,
    bcm_compat6516_l3_route_t *route);

extern int bcm_compat6516_l3_subnet_route_find(
    int unit,
    bcm_compat6516_l3_route_t *input,
    bcm_compat6516_l3_route_t *route);

typedef struct bcm_compat6516_ipmc_addr_s {
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
} bcm_compat6516_ipmc_addr_t;

extern int bcm_compat6516_ipmc_add(
    int unit,
    bcm_compat6516_ipmc_addr_t *data);

extern int bcm_compat6516_ipmc_find(
    int unit,
    bcm_compat6516_ipmc_addr_t *data);

extern int bcm_compat6516_ipmc_remove(
    int unit,
    bcm_compat6516_ipmc_addr_t *data);

extern int bcm_compat6516_ipmc_stat_attach(
    int unit,
    bcm_compat6516_ipmc_addr_t *info,
    uint32 stat_counter_id);

extern int bcm_compat6516_ipmc_stat_detach(
    int unit,
    bcm_compat6516_ipmc_addr_t *info);

extern int bcm_compat6516_ipmc_stat_counter_get(
    int unit,
    bcm_compat6516_ipmc_addr_t *info,
    bcm_ipmc_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

extern int bcm_compat6516_ipmc_stat_counter_sync_get(
    int unit,
    bcm_compat6516_ipmc_addr_t *info,
    bcm_ipmc_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

extern int bcm_compat6516_ipmc_stat_counter_set(
    int unit,
    bcm_compat6516_ipmc_addr_t *info,
    bcm_ipmc_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values);

extern int bcm_compat6516_ipmc_stat_multi_get(
    int unit,
    bcm_compat6516_ipmc_addr_t *info,
    int nstat,
    bcm_ipmc_stat_t *stat_arr,
    uint64 *value_arr);

extern int bcm_compat6516_ipmc_stat_multi_get32(
    int unit,
    bcm_compat6516_ipmc_addr_t *info,
    int nstat,
    bcm_ipmc_stat_t *stat_arr,
    uint32 *value_arr);

extern int bcm_compat6516_ipmc_stat_multi_set(
    int unit,
    bcm_compat6516_ipmc_addr_t *info,
    int nstat,
    bcm_ipmc_stat_t *stat_arr,
    uint64 *value_arr);

extern int bcm_compat6516_ipmc_stat_multi_set32(
    int unit,
    bcm_compat6516_ipmc_addr_t *info,
    int nstat,
    bcm_ipmc_stat_t *stat_arr,
    uint32 *value_arr);

extern int bcm_compat6516_ipmc_stat_id_get(
    int unit,
    bcm_compat6516_ipmc_addr_t *info,
    bcm_ipmc_stat_t stat,
    uint32 *stat_counter_id);

#endif /* INCLUDE_L3 */

/* Initialize a VLAN tag action set structure. */
typedef struct bcm_compat6516_vlan_action_set_s {
    bcm_vlan_t new_outer_vlan;          /* New outer VLAN for Add/Replace
                                           actions. */
    bcm_vlan_t new_inner_vlan;          /* New inner VLAN for Add/Replace
                                           actions. */
    uint8 new_inner_pkt_prio;           /* New inner packet priority for
                                           Add/Replace actions. */
    uint8 new_outer_cfi;                /* New outer packet CFI for Add/Replace
                                           actions. */
    uint8 new_inner_cfi;                /* New inner packet CFI for Add/Replace
                                           actions. */
    bcm_if_t ingress_if;                /* L3 Ingress Interface. */
    int priority;                       /* Internal or packet priority. */
    bcm_vlan_action_t dt_outer;         /* Outer-tag action for double-tagged
                                           packets. */
    bcm_vlan_action_t dt_outer_prio;    /* Outer-priority-tag action for
                                           double-tagged packets. */
    bcm_vlan_action_t dt_outer_pkt_prio; /* Outer packet priority action for
                                           double-tagged packets. */
    bcm_vlan_action_t dt_outer_cfi;     /* Outer packet CFI action for
                                           double-tagged packets. */
    bcm_vlan_action_t dt_inner;         /* Inner-tag action for double-tagged
                                           packets. */
    bcm_vlan_action_t dt_inner_prio;    /* Inner-priority-tag action for
                                           double-tagged packets. */
    bcm_vlan_action_t dt_inner_pkt_prio; /* Inner packet priority action for
                                           double-tagged packets. */
    bcm_vlan_action_t dt_inner_cfi;     /* Inner packet CFI action for
                                           double-tagged packets. */
    bcm_vlan_action_t ot_outer;         /* Outer-tag action for
                                           single-outer-tagged packets. */
    bcm_vlan_action_t ot_outer_prio;    /* Outer-priority-tag action for
                                           single-outer-tagged packets. */
    bcm_vlan_action_t ot_outer_pkt_prio; /* Outer packet priority action for
                                           single-outer-tagged packets. */
    bcm_vlan_action_t ot_outer_cfi;     /* Outer packet CFI action for
                                           single-outer-tagged packets. */
    bcm_vlan_action_t ot_inner;         /* Inner-tag action for
                                           single-outer-tagged packets. */
    bcm_vlan_action_t ot_inner_pkt_prio; /* Inner packet priority action for
                                           single-outer-tagged packets. */
    bcm_vlan_action_t ot_inner_cfi;     /* Inner packet CFI action for
                                           single-outer-tagged packets. */
    bcm_vlan_action_t it_outer;         /* Outer-tag action for
                                           single-inner-tagged packets. */
    bcm_vlan_action_t it_outer_pkt_prio; /* Outer packet priority action for
                                           single-inner-tagged packets. */
    bcm_vlan_action_t it_outer_cfi;     /* Outer packet CFI action for
                                           single-inner-tagged packets. */
    bcm_vlan_action_t it_inner;         /* Inner-tag action for
                                           single-inner-tagged packets. */
    bcm_vlan_action_t it_inner_prio;    /* Inner-priority-tag action for
                                           single-inner-tagged packets. */
    bcm_vlan_action_t it_inner_pkt_prio; /* Inner packet priority action for
                                           single-inner-tagged packets. */
    bcm_vlan_action_t it_inner_cfi;     /* Inner packet CFI action for
                                           single-inner-tagged packets. */
    bcm_vlan_action_t ut_outer;         /* Outer-tag action for untagged
                                           packets. */
    bcm_vlan_action_t ut_outer_pkt_prio; /* Outer packet priority action for
                                           untagged packets. */
    bcm_vlan_action_t ut_outer_cfi;     /* Outer packet CFI action for untagged
                                           packets. */
    bcm_vlan_action_t ut_inner;         /* Inner-tag action for untagged
                                           packets. */
    bcm_vlan_action_t ut_inner_pkt_prio; /* Inner packet priority action for
                                           untagged packets. */
    bcm_vlan_action_t ut_inner_cfi;     /* Inner packet CFI action for untagged
                                           packets. */
    bcm_vlan_pcp_action_t outer_pcp;    /* Outer tag's pcp field action of
                                           outgoing packets. */
    bcm_vlan_pcp_action_t inner_pcp;    /* Inner tag's pcp field action of
                                           outgoing packets. */
    bcm_policer_t policer_id;           /* Base policer to be used */
    uint16 outer_tpid;                  /* New outer-tag's tpid field for modify
                                           action */
    uint16 inner_tpid;                  /* New inner-tag's tpid field for modify
                                           action */
    bcm_vlan_tpid_action_t outer_tpid_action; /* Action of outer-tag's tpid field */
    bcm_vlan_tpid_action_t inner_tpid_action; /* Action of inner-tag's tpid field */
    int action_id;                      /* Action Set index */
    uint32 class_id;                    /* Class ID */
    bcm_tsn_pri_map_t taf_gate_primap;  /* TAF (Time Aware Filtering) gate
                                           priority mapping */
    uint32 flags;                       /* BCM_VLAN_ACTION_SET_xxx. */
} bcm_compat6516_vlan_action_set_t;

/* This structure contains the configuration of a VLAN. */
typedef struct bcm_compat6516_vlan_control_vlan_s {
    bcm_vrf_t vrf;
    bcm_vlan_t forwarding_vlan;         /* Shared VLAN ID. */
    bcm_if_t ingress_if;                /* Mapped Ingress interface. */
    uint16 outer_tpid;
    uint32 flags;
    bcm_vlan_mcast_flood_t ip6_mcast_flood_mode;
    bcm_vlan_mcast_flood_t ip4_mcast_flood_mode;
    bcm_vlan_mcast_flood_t l2_mcast_flood_mode;
    int if_class;
    bcm_vlan_forward_t forwarding_mode;
    bcm_vlan_urpf_mode_t urpf_mode;
    bcm_cos_queue_t cosq;
    int qos_map_id;                     /* index to int_pri->queue offset
                                           mapping profile */
    bcm_fabric_distribution_t distribution_class; /* Fabric Distribution Class. */
    bcm_multicast_t broadcast_group;    /* Group to handle broadcast frames */
    bcm_multicast_t unknown_multicast_group; /* Group to handle unknown multicast
                                           frames */
    bcm_multicast_t unknown_unicast_group; /* Group to handle unknown unicast
                                           frames */
    bcm_multicast_t trill_nonunicast_group; /* Group to handle trill-domain
                                           nonunicast frames */
    bcm_trill_name_t source_trill_name; /* Source RBridge nickname per VLAN */
    int trunk_index;                    /* Trunk index for static PSC */
    bcm_vlan_protocol_packet_ctrl_t protocol_pkt; /* Protocol packet control per VLAN */
    int nat_realm_id;                   /* Realm id of interface that this vlan
                                           maps to */
    int l3_if_class;                    /* L3IIF class id. */
    bcm_vlan_vp_mc_ctrl_t vp_mc_ctrl;   /* VP replication control, Auto, Enable,
                                           Disable */
    int aging_cycles;                   /* number of aging meta-cycles */
    int entropy_id;                     /* aging profile ID */
    bcm_vpn_t vpn;                      /* vpn */
    bcm_vlan_t egress_vlan;             /* egress outer vlan */
    uint32 learn_flags;                 /* Learn control flags for VLAN-based
                                           (BCM_VLAN_LEARN_CONTROL_XXX) */
    uint32 sr_flags;                    /* Flags for Seamless Redundancy:
                                           BCM_VLAN_CONTROL_SR_FLAG_xxx */
    uint32 flags2;                      /* BCM_VLAN_FLAGS2_XXX */
    uint32 ingress_stat_id;             /* Object statistics id ingress. */
    int ingress_stat_pp_profile;        /* Statistic profile ingress. */
    uint32 egress_stat_id;              /* Object statistics id egress. */
    int egress_stat_pp_profile;         /* Statistic profile egress. */
} bcm_compat6516_vlan_control_vlan_t;

/*Get or set the port's default VLAN tag actions. */
extern int bcm_compat6516_vlan_port_default_action_get(
    int unit,
    bcm_port_t port,
    bcm_compat6516_vlan_action_set_t *action);

/*Get or set the port's default VLAN tag actions. */
extern int bcm_compat6516_vlan_port_default_action_set(
    int unit,
    bcm_port_t port,
    bcm_compat6516_vlan_action_set_t *action);

/*Get or set the egress default VLAN tag actions. */
extern int bcm_compat6516_vlan_port_egress_default_action_get(
    int unit,
    bcm_port_t port,
    bcm_compat6516_vlan_action_set_t *action);

/*Get or set the egress default VLAN tag actions. */
extern int bcm_compat6516_vlan_port_egress_default_action_set(
    int unit,
    bcm_port_t port,
    bcm_compat6516_vlan_action_set_t *action);

/*
 * Add protocol based VLAN with specified action. If the entry already
 * exists, update the action.
 */
extern int bcm_compat6516_vlan_port_protocol_action_add(
    int unit,
    bcm_port_t port,
    bcm_port_frametype_t frame,
    bcm_port_ethertype_t ether,
    bcm_compat6516_vlan_action_set_t *action);

/*Get protocol based VLAN action. */
extern int bcm_compat6516_vlan_port_protocol_action_get(
    int unit,
    bcm_port_t port,
    bcm_port_frametype_t frame,
    bcm_port_ethertype_t ether,
    bcm_compat6516_vlan_action_set_t *action);

/*
 * Add an association from MAC address to VLAN actions, which are
 *             used for VLAN tag/s assignment to untagged packets.
 */
extern int bcm_compat6516_vlan_mac_action_add(
    int unit,
    bcm_mac_t mac,
    bcm_compat6516_vlan_action_set_t *action);

/*
 * Retrive an association from MAC address to VLAN actions, which
 *             are used for VLAN tag assignment to untagged packets.
 */
extern int bcm_compat6516_vlan_mac_action_get(
    int unit,
    bcm_mac_t mac,
    bcm_compat6516_vlan_action_set_t *action);

/*Add an entry to the VLAN Translation table and assign VLAN actions. */
extern int bcm_compat6516_vlan_translate_action_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6516_vlan_action_set_t *action);

/*Add an entry to the VLAN Translation table and assign VLAN actions. */
extern int bcm_compat6516_vlan_translate_action_create(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6516_vlan_action_set_t *action);

/*Get an entry to the VLAN Translation table and assign VLAN actions. */
extern int bcm_compat6516_vlan_translate_action_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6516_vlan_action_set_t *action);

/*
 * Add an entry to the egress VLAN Translation table and assign VLAN
 * actions.
 */
extern int bcm_compat6516_vlan_translate_egress_action_add(
    int unit,
    int port_class,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6516_vlan_action_set_t *action);

/*
 * Get an entry to the egress VLAN Translation table and assign VLAN
 * actions.
 */
extern int bcm_compat6516_vlan_translate_egress_action_get(
    int unit,
    int port_class,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6516_vlan_action_set_t *action);

/*
 * Add an entry based on gport and VLANs to the egress VLAN Translation
 * table and assign VLAN actions.
 */
extern int bcm_compat6516_vlan_translate_egress_gport_action_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6516_vlan_action_set_t *action);

/*
 * Get VLAN actions for an entry based on gport and VLANS in the egress
 * VLAN Translation table.
 */
extern int bcm_compat6516_vlan_translate_egress_gport_action_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6516_vlan_action_set_t *action);


/*bcm_vlan_translate_action_range_add */
extern int bcm_compat6516_vlan_translate_action_range_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan_low,
    bcm_vlan_t outer_vlan_high,
    bcm_vlan_t inner_vlan_low,
    bcm_vlan_t inner_vlan_high,
    bcm_compat6516_vlan_action_set_t *action);

/*bcm_vlan_translate_action_range_get */
extern int bcm_compat6516_vlan_translate_action_range_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan_low,
    bcm_vlan_t outer_vlan_high,
    bcm_vlan_t inner_vlan_low,
    bcm_vlan_t inner_vlan_high,
    bcm_compat6516_vlan_action_set_t *action);

/*Set a VLAN translation ID instance with tag actions */
extern int bcm_compat6516_vlan_translate_action_id_set(
    int unit,
    uint32 flags,
    int action_id,
    bcm_compat6516_vlan_action_set_t *action);

/*Get tag actions from a VLAN translation ID instance */
extern int bcm_compat6516_vlan_translate_action_id_get(
    int unit,
    uint32 flags,
    int action_id,
    bcm_compat6516_vlan_action_set_t *action);

/*
 * Add an association from IP subnet to VLAN actions to use for assigning
 * VLAN tag actions to untagged packets.
 */
extern int bcm_compat6516_vlan_ip_action_add(
    int unit,
    bcm_vlan_ip_t *vlan_ip,
    bcm_compat6516_vlan_action_set_t *action);

/*
 * Get an association from IP subnet to VLAN actions to use for assigning
 * VLAN tag actions to untagged packets.
 */
extern int bcm_compat6516_vlan_ip_action_get(
    int unit,
    bcm_vlan_ip_t *vlan_ip,
    bcm_compat6516_vlan_action_set_t *action);

/*Set or retrieve current vlan properties selectively. */
extern int bcm_compat6516_vlan_control_vlan_selective_get(
    int unit,
    bcm_vlan_t vlan,
    uint32 valid_fields,
    bcm_compat6516_vlan_control_vlan_t *control);

/*Set or retrieve current vlan properties selectively. */
extern int bcm_compat6516_vlan_control_vlan_selective_set(
    int unit,
    bcm_vlan_t vlan,
    uint32 valid_fields,
    bcm_compat6516_vlan_control_vlan_t *control);

/*Set or retrieve current vlan properties. */
extern int bcm_compat6516_vlan_control_vlan_get(
    int unit,
    bcm_vlan_t vlan,
    bcm_compat6516_vlan_control_vlan_t *control);

/*Set or retrieve current vlan properties. */
extern int bcm_compat6516_vlan_control_vlan_set(
    int unit,
    bcm_vlan_t vlan,
    bcm_compat6516_vlan_control_vlan_t control);

/*Add action for match criteria for Vlan assignment. */
extern int bcm_compat6516_vlan_match_action_add(
    int unit,
    uint32 options,
    bcm_vlan_match_info_t *match_info,
    bcm_compat6516_vlan_action_set_t *action_set);

/*Get action for given match criteria. */
extern int bcm_compat6516_vlan_match_action_get(
    int unit,
    bcm_vlan_match_info_t *match_info,
    bcm_compat6516_vlan_action_set_t *action_set);

/*Get all actions and match criteria for given vlan match. */
extern int bcm_compat6516_vlan_match_action_multi_get(
    int unit,
    bcm_vlan_match_t match,
    int size,
    bcm_vlan_match_info_t *match_info_array,
    bcm_compat6516_vlan_action_set_t *action_set_array,
    int *count);

/*Add VLAN actions to a field entry. */
extern int bcm_compat6516_field_action_vlan_actions_add(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_compat6516_vlan_action_set_t *vlan_action_set);

/*Retrieve VLAN actions configured to a field entry. */
extern int bcm_compat6516_field_action_vlan_actions_get(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_compat6516_vlan_action_set_t *vlan_action_set);

/* VLAN/VSAN Translation Action Set. */
typedef struct bcm_compat6516_fcoe_vsan_action_set_t {
    uint32 flags;                       /* BCM_FCOE_VSAN_ACTION_* */
    bcm_fcoe_vsan_id_t new_vsan;        /* new VSAN ID to replace with */
    uint8 new_vsan_pri;                 /* new VSAN priority to replace with */
    bcm_fcoe_vsan_vft_t vft;            /* remaining fields of VFT header */
    bcm_if_t ingress_if;                /* TBD */
    bcm_fcoe_vsan_action_t dt_vsan;     /* Vsan action when double tagged */
    bcm_fcoe_vsan_action_t dt_vsan_pri; /* VSAN Priority action when double
                                           tagged */
    bcm_fcoe_vsan_action_t it_vsan;     /* Vsan action when single inner tagged */
    bcm_fcoe_vsan_action_t it_vsan_pri; /* Vsan Priority action when single
                                           inner tagged */
    bcm_fcoe_vsan_action_t ot_vsan;     /* Vsan action when single outer tagged */
    bcm_fcoe_vsan_action_t ot_vsan_pri; /* Vsan Priority action when single
                                           outer tagged */
    bcm_fcoe_vsan_action_t ut_vsan;     /* Vsan action when un-tagged */
    bcm_fcoe_vsan_action_t ut_vsan_pri; /* Vsan Priority action when un-tagged */
    bcm_compat6516_vlan_action_set_t vlan_action;  /* VLAN actions */
} bcm_compat6516_fcoe_vsan_action_set_t;

/* Add VSAN translation action */
extern int bcm_compat6516_fcoe_vsan_translate_action_add(
    int unit,
    bcm_fcoe_vsan_translate_key_config_t *key,
    bcm_compat6516_fcoe_vsan_action_set_t *action);
/* BCM Information structure. */
typedef struct bcm_compat6516_info_s {
    uint32 vendor;      /* PCI values used usually. */
    uint32 device;      /* PCI values used usually. */
    uint32 revision;    /* PCI values used usually. */
    uint32 capability;
    int num_pipes;      /* Number of pipes in the device. */
} bcm_compat6516_info_t;

/* Get information about a BCM unit. */
extern int bcm_compat6516_info_get(int unit, bcm_compat6516_info_t *info);

/* Generic port match attribute structure */
typedef struct bcm_compat6516_port_match_info_s {
    bcm_port_match_t match;             /* Match criteria */
    bcm_gport_t port;                   /* Match port */
    bcm_vlan_t match_vlan;              /* Outer VLAN ID to match */
    bcm_vlan_t match_vlan_max;          /* Maximum VLAN ID in range to match */
    bcm_vlan_t match_inner_vlan;        /* Inner VLAN ID to match */
    bcm_vlan_t match_inner_vlan_max;    /* Maximum Inner VLAN ID in range to
                                           match */
    bcm_vlan_t match_tunnel_vlan;       /* B-domain VID */
    bcm_mac_t match_tunnel_srcmac;      /* B-domain source MAC address */
    bcm_mpls_label_t match_label;       /* MPLS label */
    uint32 flags;                       /* BCM_PORT_MATCH_XXX flags */
    bcm_tunnel_id_t match_pon_tunnel;   /* PON Tunnel value to match. */
    bcm_port_ethertype_t match_ethertype; /* Ethernet type value to match */
    int match_pcp;                      /* Outer PCP ID to match */
    bcm_compat6516_vlan_action_set_t *action;
                                        /* Match action */
    uint16 extended_port_vid;           /* Extender port VID */
    bcm_vpn_t vpn;                      /* VPN ID */
    uint16 niv_port_vif;                /* NIV port VIF */
    uint32 isid;
    bcm_mac_t src_mac;                  /* Source Mac Address */
    uint32 port_class;                  /* Port Class Id */
}bcm_compat6516_port_match_info_t;

/* Add a match to an existing port */
extern int bcm_compat6516_port_match_add(
    int unit,
    bcm_gport_t port,
    bcm_compat6516_port_match_info_t *match);

/* Remove a match from an existing port */
extern int bcm_compat6516_port_match_delete(
    int unit,
    bcm_gport_t port,
    bcm_compat6516_port_match_info_t *match);

/* Replace an old match with a new one for an existing port */
extern int bcm_compat6516_port_match_replace(
    int unit,
    bcm_gport_t port,
    bcm_compat6516_port_match_info_t *old_match,
    bcm_compat6516_port_match_info_t *new_match);

/* Get all the matches for an existing port */
extern int bcm_compat6516_port_match_multi_get(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_compat6516_port_match_info_t *match_array,
    int *count);

/* Assign a set of matches to an existing port */
extern int bcm_compat6516_port_match_set(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_compat6516_port_match_info_t *match_array);

/* speed_ability supported by a port */
typedef struct bcm_compat6516_port_speed_ability_s {
    uint32 speed;                       /* port speed in mbps */
    uint32 resolved_num_lanes;          /* autoneg resolved speed num lane */
    bcm_port_phy_fec_t fec_type;        /* fec_type for port */
    bcm_port_medium_t medium;           /* medium type of a port */
    bcm_port_phy_pause_t pause;         /* pause */
    bcm_port_phy_channel_t channel;     /* short/long channel for a port */
    bcm_port_autoneg_mode_t an_mode;    /* autoneg mode */
} bcm_compat6516_port_speed_ability_t;

/*Set the local port advertisement for autonegotiation*/
extern int bcm_compat6516_port_autoneg_ability_advert_set(int unit,
                                            bcm_port_t port,
                                            int num_ability,
                                            bcm_compat6516_port_speed_ability_t *match_array);

/*Retrieve the advert port abilities*/
extern int bcm_compat6516_port_autoneg_ability_advert_get(int unit,
                                     bcm_port_t port,
                                     int max_num_ability,
                                     bcm_compat6516_port_speed_ability_t *match_array,
                                     int *actual_num_ability);

/*Retrieve the local port abilities*/
extern int bcm_compat6516_port_speed_ability_local_get(int unit, bcm_port_t port,
                                     int max_num_ability, bcm_compat6516_port_speed_ability_t *match_array,
                                     int *actual_num_ability);

/*Retrieve the remote port abilities*/
extern int bcm_compat6516_port_autoneg_ability_remote_get(int unit, bcm_port_t port,
                                     int max_num_ability, bcm_compat6516_port_speed_ability_t *match_array,
                                     int *actual_num_ability);

/*Port phy tx fir*/
typedef struct bcm_compat6516_port_phy_tx_s {
    int pre2;
    int pre;
    int main;
    int post;
    int post2;
    int post3;
    bcm_port_phy_tx_tap_mode_t tx_tap_mode;
    bcm_port_phy_signalling_mode_t signalling_mode;
} bcm_compat6516_port_phy_tx_t;

/*Set port phy tx fir*/
extern int bcm_compat6516_port_phy_tx_set(
    int unit,
    bcm_port_t port,
    bcm_compat6516_port_phy_tx_t *match);

/*Get port phy tx fir*/
extern int bcm_compat6516_port_phy_tx_get(
    int unit,
    bcm_port_t port,
    bcm_compat6516_port_phy_tx_t *match);


/* Rx cosq mapping. */
typedef struct bcm_compat6516_rx_cosq_mapping_s {
    uint32 flags;                       /* flags */
    int index;                          /* Index into COSQ mapping table (0 is
                                           highest match priority) */
    bcm_rx_reasons_t reasons;           /* packet reasons bitmap */
    bcm_rx_reasons_t reasons_mask;      /* mask for packet reasons bitmap */
    uint8 int_prio;                     /* internal priority value */
    uint8 int_prio_mask;                /* mask for internal priority value */
    uint32 packet_type;                 /* packet type bitmap */
    uint32 packet_type_mask;            /* mask for packet type bitmap */
    bcm_cos_queue_t cosq;               /* cosq value */
} bcm_compat6516_rx_cosq_mapping_t;

/* Map packets to a CPU COS queue. */
extern int bcm_compat6516_rx_cosq_mapping_extended_set(
    int unit,
    uint32 options,
    bcm_compat6516_rx_cosq_mapping_t *rx_cosq_mapping);

/* Get packets to CPU COS queue mappings. */
extern int bcm_compat6516_rx_cosq_mapping_extended_get(
    int unit,
    bcm_compat6516_rx_cosq_mapping_t *rx_cosq_mapping);

/*
 * Group configuration structure. Used to create a field group with
 * specific attributes.
 */
typedef struct bcm_compat6516_field_group_config_s {
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
} bcm_compat6516_field_group_config_t;

/* Create a field group with specific attributes. */
extern int bcm_compat6516_field_group_config_create(
    int unit,
    bcm_compat6516_field_group_config_t *group_config);

/* To verify whether a group can be created with a given qset and mode */
extern int bcm_compat6516_field_group_config_validate(
    int unit,
    bcm_compat6516_field_group_config_t *group_config,
    bcm_field_group_mode_t *mode);

/*
 * Mirror destination Structure
 *
 * Contains information required for manipulating mirror destinations.
 */
typedef struct bcm_compat6516_mirror_destination_s {
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
} bcm_compat6516_mirror_destination_t;

/* Create a mirror (destination, encapsulation) pair. */
extern int bcm_compat6516_mirror_destination_create(
    int unit,
    bcm_compat6516_mirror_destination_t *mirror_dest);

/* Get  mirror (destination, encapsulation) pair. */
extern int bcm_compat6516_mirror_destination_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    bcm_compat6516_mirror_destination_t *mirror_dest);

/* QoS Map structure */
typedef struct bcm_compat6516_qos_map_s {
    uint8 pkt_pri;              /* Packet priority */
    uint8 pkt_cfi;              /* Packet CFI */
    int dscp;                   /* Packet DSCP */
    int exp;                    /* Packet EXP */
    int int_pri;                /* Internal priority */
    bcm_color_t color;          /* Color */
    int remark_int_pri;         /* (internal) remarking priority */
    bcm_color_t remark_color;   /* (internal) remark color */
    int policer_offset;         /* Offset based on pri/cos to fetch a policer */
    int queue_offset;           /* Offset based on int_pri to fetch cosq for
                                   subscriber ports */
    int port_offset;            /* Offset based on port connection for indexing
                                   into the action table */
    uint8 etag_pcp;             /* ETAG PCP field */
    uint8 etag_de;              /* ETAG DE field */
    int counter_offset;         /* Offset based on priority for indexing into
                                   the loss measurement counter table */
    int inherited_dscp_exp;     /* Inherited DSCP EXP value */
    uint32 opcode;              /* Set QOS Map Opcode ID */
} bcm_compat6516_qos_map_t;

/* bcm_qos_map_add */
extern int bcm_compat6516_qos_map_add(
    int unit,
    uint32 flags,
    bcm_compat6516_qos_map_t *map,
    int map_id);

/* bcm_qos_map_multi_get */
extern int bcm_compat6516_qos_map_multi_get(
    int unit,
    uint32 flags,
    int map_id,
    int array_size,
    bcm_compat6516_qos_map_t *array,
    int *array_count);

/* bcm_qos_map_delete */
extern int bcm_compat6516_qos_map_delete(
    int unit,
    uint32 flags,
    bcm_compat6516_qos_map_t *map,
    int map_id);

/* L3 NAT egress info. */
typedef struct bcm_compat6516_l3_nat_egress_s {
    uint32 flags;               /* See BCM_L3_NAT_EGRESS_XXX flag definitions. */
    bcm_l3_nat_id_t nat_id;     /* Index into packet edit table. */
    bcm_ip_t sip_addr;          /* Translated source IP. */
    bcm_ip_t sip_addr_mask;     /* Bits of source IP prefix to translate. */
    uint16 src_port;            /* Source l4 port for NAPT */
    bcm_ip_t dip_addr;          /* Translated dest IP. */
    bcm_ip_t dip_addr_mask;     /* Bits of dest IP prefix to translate. */
    uint16 dst_port;            /* Destination l4 port for NAPT */
} bcm_compat6516_l3_nat_egress_t;

/* Add an L3 NAT egress entry. */
extern int bcm_compat6516_l3_nat_egress_add(
    int unit,
    bcm_compat6516_l3_nat_egress_t *nat_info);

/* Get an L3 NAT egress entry. */
extern int bcm_compat6516_l3_nat_egress_get(
    int unit,
    bcm_compat6516_l3_nat_egress_t *nat_info);

/* L3 NAT ingress info. */
typedef struct bcm_compat6516_l3_nat_ingress_s {
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
} bcm_compat6516_l3_nat_ingress_t;

/* Add an ingress NAT table entry. */
extern int bcm_compat6516_l3_nat_ingress_add(
    int unit,
    bcm_compat6516_l3_nat_ingress_t *nat_info);

/* Delete an ingress NAT table entry. */
extern int bcm_compat6516_l3_nat_ingress_delete(
    int unit,
    bcm_compat6516_l3_nat_ingress_t *nat_info);

/* Find an ingress NAT table entry. */
extern int bcm_compat6516_l3_nat_ingress_find(
    int unit,
    bcm_compat6516_l3_nat_ingress_t *nat_info);

/* Delete all the ingress NAT table entries specified by flags. */
extern int bcm_compat6516_l3_nat_ingress_delete_all(
    int unit,
    bcm_compat6516_l3_nat_ingress_t *nat_info);

#if defined(INCLUDE_L3)
typedef struct bcm_compat6516_mim_vpn_config_s {
    uint32 flags;                       /* Configuration flags */
    bcm_mim_vpn_t vpn;                  /* VPN ID. */
    int lookup_id;                      /* The I-SID value */
    uint16 match_service_tpid;          /* Incoming SD tag TPID */
    bcm_multicast_t broadcast_group;    /* Broadcast group */
    bcm_multicast_t unknown_unicast_group; /* Unknown unicast group */
    bcm_multicast_t unknown_multicast_group; /* Unknown multicast group */
    bcm_policer_t policer_id;           /* Policer ID to be used */
    bcm_if_t service_encap_id;          /* Service encap Identifier */
    uint8 int_pri;                      /* Fixed internal priority */
    int qos_map_id;                     /* Mapped priority */
    bcm_vlan_t tunnel_vlan;             /* B-domain VID */
    int inlif_counting_profile;         /* In LIF counting profile */
    int outlif_counting_profile;        /* Out LIF counting profile */
    bcm_vlan_protocol_packet_ctrl_t protocol_pkt; /* Protocol packet control */
    uint16 egress_service_tpid;         /* egress SD tag TPID */
    bcm_mac_t match_service_smac;       /* Source B-MAC address */
} bcm_compat6516_mim_vpn_config_t;

extern int bcm_compat6516_mim_vpn_create(int unit,
                                         bcm_compat6516_mim_vpn_config_t *info);
extern int bcm_compat6516_mim_vpn_get(int unit, bcm_vpn_t l2vpn,
                                         bcm_compat6516_mim_vpn_config_t *info);

#endif /* end if defined(INCLUDE_L3) */

#if defined(INCLUDE_L3)
typedef struct bcm_compat6516_vxlan_port_s {
    bcm_gport_t vxlan_port_id;          /* GPORT identifier. */
    uint32 flags;                       /* BCM_VXLAN_PORT_xxx. */
    uint32 if_class;                    /* Interface class ID. */
    uint16 int_pri;                     /* Internal Priority */
    uint8 pkt_pri;                      /* Packet Priority */
    uint8 pkt_cfi;                      /* Packet CFI */
    uint16 egress_service_tpid;         /* Service Vlan TPID Value */
    bcm_vlan_t egress_service_vlan;     /* SD-TAG Vlan ID. */
    uint16 mtu;                         /* MTU */
    bcm_gport_t match_port;             /* Match port / trunk */
    bcm_vxlan_port_match_t criteria;    /* Match criteria */
    bcm_vlan_t match_vlan;              /* Outer VLAN ID to match. */
    bcm_vlan_t match_inner_vlan;        /* Inner VLAN ID to match. */
    bcm_gport_t egress_tunnel_id;       /* Tunnel Initiator ID */
    bcm_gport_t match_tunnel_id;        /* Tunnel Terminator ID */
    bcm_if_t egress_if;                 /* VXLAN egress object. */
    bcm_switch_network_group_t network_group_id; /* Split Horizon network group
                                           identifier. */
    uint32 vnid;                        /* VXLAN packet VNID. */
    int qos_map_id;                     /* QoS Mapped priority */
    uint8 tunnel_pkt_pri;               /* Tunnel Packet Priority */
    uint8 tunnel_pkt_cfi;               /* Tunnel Packet CFI */
} bcm_compat6516_vxlan_port_t;

extern int bcm_compat6516_vxlan_port_add(int unit, bcm_vpn_t l2vpn,
                                         bcm_compat6516_vxlan_port_t *vxlan_port);
extern int bcm_compat6516_vxlan_port_get(int unit, bcm_vpn_t l2vpn,
                                         bcm_compat6516_vxlan_port_t *vxlan_port);
extern int bcm_compat6516_vxlan_port_get_all(int unit, bcm_vpn_t l2vpn,
                                             int port_max,
                                             bcm_compat6516_vxlan_port_t *port_array,
                                             int *port_count);

#endif /* end if defined(INCLUDE_L3) */

typedef struct bcm_compat6516_l2_ring_s {
    uint32 flags;                   /* flags */
    bcm_vlan_vector_t vlan_vector;  /* List of Vlans. */
    bcm_gport_t port0;              /* VLAN gport */
    bcm_gport_t port1;              /* VLAN gport */
    bcm_vpn_vector_t vpn_vector;    /* List of VPN IDs. */
} bcm_compat6516_l2_ring_t;

extern int bcm_compat6516_l2_ring_replace(int unit,
                                          bcm_compat6516_l2_ring_t *l2_ring);

#endif /* BCM_RPC_SUPPORT */
#endif /* !_COMPAT_6516_H */
