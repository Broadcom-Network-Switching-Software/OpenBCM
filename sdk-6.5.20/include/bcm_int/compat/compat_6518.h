/*
 * $Id: compat_6518.h,v 1.0 2019/04/03
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-6.5.18 routines
 */

#ifndef _COMPAT_6518_H_
#define _COMPAT_6518_H_

#ifdef BCM_RPC_SUPPORT
#include <bcm_int/compat/compat_6519.h>
#include <bcm/sat.h>
#include <bcm/types.h>
#include <bcm/mirror.h>
#if defined(INCLUDE_L3)
#include <bcm/tunnel.h>
#include <bcm/l3.h>
#include <bcm/flow.h>
#include <bcm/failover.h>
#include <bcm/vxlan.h>
#endif /* defined(INCLUDE_L3) */
#include <bcm/udf.h>
#include <bcm/ecn.h>
#include <bcm/flexctr.h>
#include <bcm/cosq.h>
#include <bcm/field.h>
#include <bcm/switch.h>
#if defined(INCLUDE_BFD)
#include <bcm/bfd.h>
#endif /* defined(INCLUDE_BFD) */
#include <bcm/vlan.h>
#include <bcm/fcoe.h>
#include <bcm/collector.h>
#include <bcm/l2.h>
#include <bcm/rx.h>
#include <bcm/srv6.h>
#include <bcm/port.h>
#include <bcm/cpri.h>
#include <bcm/oam.h>

/*
 * Mirror destination Structure
 *
 * Contains information required for manipulating mirror destinations.
 */
typedef struct bcm_compat6518_mirror_destination_s {
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
} bcm_compat6518_mirror_destination_t;

/* Create a mirror (destination, encapsulation) pair. */
extern int bcm_compat6518_mirror_destination_create(
    int unit,
    bcm_compat6518_mirror_destination_t *mirror_dest);

/* Get  mirror (destination, encapsulation) pair. */
extern int bcm_compat6518_mirror_destination_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    bcm_compat6518_mirror_destination_t *mirror_dest);

/* Mirror source structure. */
typedef struct bcm_compat6518_mirror_source_s {
    bcm_mirror_source_type_t type;      /* Mirror source type. */
    uint32 flags;                       /* Mirror source flags only with port
                                           source type. */
    bcm_port_t port;                    /* Port/gport id. */
    bcm_pkt_trace_event_t trace_event;  /* Trace event. */
    bcm_pkt_drop_event_t drop_event;    /* Drop event. */
    uint32 sample_profile_id;           /* Sample profile id. */
} bcm_compat6518_mirror_source_t;

/* Add a mirror destination to a mirror source. */
extern int bcm_compat6518_mirror_source_dest_add(
    int unit,
    bcm_compat6518_mirror_source_t *source,
    bcm_gport_t mirror_dest_id);

/* Delete a mirror destination from a mirror source. */
extern int bcm_compat6518_mirror_source_dest_delete(
    int unit,
    bcm_compat6518_mirror_source_t *source,
    bcm_gport_t mirror_dest_id);

/* Delete all mirror destinations applied on a mirror source. */
extern int bcm_compat6518_mirror_source_dest_delete_all(
    int unit,
    bcm_compat6518_mirror_source_t *source);

/* Get all mirror destinations applied on a mirror source. */
extern int bcm_compat6518_mirror_source_dest_get_all(
    int unit,
    bcm_compat6518_mirror_source_t *source,
    int array_size,
    bcm_gport_t *mirror_dest,
    int *count);
#if defined(INCLUDE_L3)

/*
 * L3 Interface Structure.
 *
 * Contains information required for manipulating L3 interfaces.
 */
typedef struct bcm_compat6518_l3_intf_s {
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
} bcm_compat6518_l3_intf_t;

/* Create a new L3 interface. */
extern int bcm_compat6518_l3_intf_create(
    int unit,
    bcm_compat6518_l3_intf_t *intf);

/* Delete an L3 interface */
extern int bcm_compat6518_l3_intf_delete(
    int unit,
    bcm_compat6518_l3_intf_t *intf);

/* Search for L3 interface by MAC address and VLAN. */
extern int bcm_compat6518_l3_intf_find(
    int unit,
    bcm_compat6518_l3_intf_t *intf);

/* Search for L3 interface by VLAN only. */
extern int bcm_compat6518_l3_intf_find_vlan(
    int unit,
    bcm_compat6518_l3_intf_t *intf);

/* Given the L3 interface number, return the interface information. */
extern int bcm_compat6518_l3_intf_get(
    int unit,
    bcm_compat6518_l3_intf_t *intf);

/* Set the tunneling initiator parameters on an L3 interface. */
extern int bcm_compat6518_tunnel_initiator_set(
    int unit,
    bcm_compat6518_l3_intf_t *intf,
    bcm_compat6519_tunnel_initiator_t *tunnel);

/* Set the tunneling initiator parameters on an L3 interface. */
extern int bcm_compat6518_tunnel_initiator_create(
    int unit,
    bcm_compat6518_l3_intf_t *intf,
    bcm_compat6519_tunnel_initiator_t *tunnel);

/* Delete the tunnel association for the given L3 interface. */
extern int bcm_compat6518_tunnel_initiator_clear(
    int unit,
    bcm_compat6518_l3_intf_t *intf);

/* Get the tunnel property for the given L3 interface. */
extern int bcm_compat6518_tunnel_initiator_get(
    int unit,
    bcm_compat6518_l3_intf_t *intf,
    bcm_compat6519_tunnel_initiator_t *tunnel);

typedef struct bcm_compat6518_flow_port_encap_s {
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
} bcm_compat6518_flow_port_encap_t;

extern int bcm_compat6518_flow_port_encap_set(
    int unit,
    bcm_compat6518_flow_port_encap_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcm_compat6518_flow_port_encap_get(
    int unit,
    bcm_compat6518_flow_port_encap_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

typedef struct bcm_compat6518_mpls_tunnel_encap_s {
    bcm_gport_t tunnel_id;              /* Tunnel ID. */
    int num_labels;                     /* Number of labels to be added. */
    bcm_mpls_egress_label_t label_array[BCM_MPLS_EGRESS_LABEL_MAX]; /* Label info. */
} bcm_compat6518_mpls_tunnel_encap_t;

extern int bcm_compat6518_mpls_tunnel_encap_create(
    int unit,
    uint32 options,
    bcm_compat6518_mpls_tunnel_encap_t *tunnel_encap);

extern int bcm_compat6518_mpls_tunnel_encap_get(
    int unit,
    bcm_compat6518_mpls_tunnel_encap_t *tunnel_encap);

/* MPLS port type <= 6.5.18. */
typedef struct bcm_compat6518_mpls_port_s {
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
} bcm_compat6518_mpls_port_t;

extern int bcm_compat6518_mpls_port_add(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6518_mpls_port_t *mpls_port);

extern int bcm_compat6518_mpls_port_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6518_mpls_port_t *mpls_port);

extern int bcm_compat6518_mpls_port_get_all(
    int unit,
    bcm_vpn_t vpn,
    int port_max,
    bcm_compat6518_mpls_port_t *port_array,
    int *port_count);

#endif /* defined(INCLUDE_L3) */

/* Packet format based UDF specification structure. */
typedef struct bcm_compat6518_udf_pkt_format_info_s {
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
} bcm_compat6518_udf_pkt_format_info_t;

/* Create a packet format entry */
extern int bcm_compat6518_udf_pkt_format_create(
    int unit,
    bcm_udf_pkt_format_options_t options,
    bcm_compat6518_udf_pkt_format_info_t *pkt_format,
    bcm_udf_pkt_format_id_t *pkt_format_id);

/* Retrieve packet format info given the packet format Id */
extern int bcm_compat6518_udf_pkt_format_info_get(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_compat6518_udf_pkt_format_info_t *pkt_format);

/* QoS Map structure */
typedef struct bcm_compat6518_qos_map_s {
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
    uint8 inner_pkt_pri;        /* Inner Tag Packet priority */
    uint8 inner_pkt_cfi;        /* Inner Tag Packet CFI */
    uint8 int_cn;               /* Internal congestion notification. */
    uint8 pkt_ecn;              /* Packet explicit congestion notification. */
    uint8 responsive;           /* Responsive flow(like TCP). */
    uint8 latency_marked;       /* Packet marked with latency congestion. */
} bcm_compat6518_qos_map_t;

/* bcm_qos_map_add */
extern int bcm_compat6518_qos_map_add(
    int unit,
    uint32 flags,
    bcm_compat6518_qos_map_t *map,
    int map_id);

/* bcm_qos_map_multi_get */
extern int bcm_compat6518_qos_map_multi_get(
    int unit,
    uint32 flags,
    int map_id,
    int array_size,
    bcm_compat6518_qos_map_t *array,
    int *array_count);

/* bcm_qos_map_delete */
extern int bcm_compat6518_qos_map_delete(
    int unit,
    uint32 flags,
    bcm_compat6518_qos_map_t *map,
    int map_id);

/* ECN traffic action configuration.
 *
 * Contains information required for assigning the actions of ECN traffic.
 */
typedef struct bcm_compat6518_ecn_traffic_action_config_s {
    uint32 action_type;    /* See BCM_ECN_TRAFFIC_ACTION_TYPE_XXX flag definitions. */
    uint32 action_flags;    /* See BCM_ECN_TRAFFIC_ACTION_XXX flag definitions. */
    int int_cn;    /* Internal congestion notification. */
    bcm_color_t color;    /* Packet color. */
    uint8 ecn;    /* ECN value of packet;s IP header. */
    uint8 new_ecn;    /* New ECN value for remarking. */
    int congested_int_cn;    /* New int_cn to be updated when the congestion is experienced. */
    int non_congested_int_cn;    /* New int_cn to be updated when the congestion is not experienced. */
    int responsive;    /* New responsive value to be used in the egress pipeline. */
} bcm_compat6518_ecn_traffic_action_config_t;

/* Assign/Retrieve the actions to the specified ECN traffic.  */
extern int bcm_compat6518_ecn_traffic_action_config_get(
    int unit,
    bcm_compat6518_ecn_traffic_action_config_t *ecn_config);

/* Assign/Retrieve the actions to the specified ECN traffic.  */
extern int bcm_compat6518_ecn_traffic_action_config_set(
    int unit,
    bcm_compat6518_ecn_traffic_action_config_t *ecn_config);

/* ECN traffic map info. */
typedef struct bcm_compat6518_ecn_traffic_map_info_s {
    uint32 flags;    /* BCM_ECN_TRAFFIC_MAP_XXX flag definitions. */
    uint8 ecn;    /* ECN value of packet's IP header. */
    int int_cn;    /* Mapped internal congestion notification(int_cn) value. */
    uint8 tunnel_ecn;    /* ECN information of packet's tunnel header. */
    bcm_ecn_traffic_map_tunnel_type_t tunnel_type;    /* Tunnel type. */
} bcm_compat6518_ecn_traffic_map_info_t;

/* To set/get the mapped internal congestion notification (int_cn). */
extern int bcm_compat6518_ecn_traffic_map_get(
    int unit,
    bcm_compat6518_ecn_traffic_map_info_t *map);

/* To set/get the mapped internal congestion notification (int_cn). */
extern int bcm_compat6518_ecn_traffic_map_set(
    int unit,
    bcm_compat6518_ecn_traffic_map_info_t *map);

#if defined(INCLUDE_L3)
/* MPLS ECN Map Structure. */
typedef struct bcm_compat6518_ecn_map_s {
    uint32 action_flags;    /* action flags for ECN map. */
    int int_cn;    /* Internal congestion. */
    uint8 inner_ecn;    /* IP ECN value in payload. */
    uint8 ecn;    /* IP ECN value. */
    uint8 exp;    /* Mpls EXP value. */
    uint8 new_ecn;    /* New ECN value. */
    uint8 new_exp;    /* New EXP value. */
    uint32 nonip_action_flags;    /* action flags for Non-ip ECN map. */
} bcm_compat6518_ecn_map_t;

/* To set an ECN mapping entry in an ECN mapping profile. */
extern int bcm_compat6518_ecn_map_set(
    int unit,
    uint32 options,
    int ecn_map_id,
    bcm_compat6518_ecn_map_t *ecn_map);

/* To get an ECN mapping entry from an ECN mapping profile. */
extern int bcm_compat6518_ecn_map_get(
    int unit,
    int ecn_map_id,
    bcm_compat6518_ecn_map_t *ecn_map);

/* Per DLB configuration of monitoring parameters. */
typedef struct bcm_compat6518_l3_ecmp_dlb_mon_cfg_s {
    uint32 sample_rate;    /* Rate of sampling DLB monitored packets */
    int action;    /* Either BCM_L3_ECMP_DLB_MON_COPY_TO_CPU or BCM_L3_ECMP_DLB_MON_MIRROR or BCM_L3_ECMP_DLB
_MON_COPY_TO_CPU_AND_MIRROR or BCM_L3_ECMP_DLB_MON_ACTION_NONE or BCM_L3_ECMP_DLB_MON_TRACE */
    int enable;    /* Enable monitoring of DLB associated with the ECMP group. 1 = Enable monitoring for DLB
associated with ecmp, 0 = Do not monitor DLB */
} bcm_compat6518_l3_ecmp_dlb_mon_cfg_t;

/* Configure monitoring parameters for a DLB corresponding to a ECMP interface */
extern int bcm_compat6518_l3_ecmp_dlb_mon_config_set(
    int unit,
    bcm_if_t ecmp_intf,
    bcm_compat6518_l3_ecmp_dlb_mon_cfg_t *dlb_mon_cfg);

/* Retrieve monitoring parameters for a DLB corresponding to a ECMP interface */
extern int bcm_compat6518_l3_ecmp_dlb_mon_config_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6518_l3_ecmp_dlb_mon_cfg_t *dlb_mon_cfg);

#endif /* defined(INCLUDE_L3) */
#if defined(INCLUDE_BFD)
/* BFD endpoint object. */
typedef struct bcm_compat6518_bfd_endpoint_info_s {
    uint32 flags;    /* Control flags. */
    bcm_bfd_endpoint_t id;    /* Endpoint identifier. */
    bcm_bfd_endpoint_t remote_id;    /* Remote endpoint identifier. */
    bcm_bfd_tunnel_type_t type;    /* Type of BFD encapsulation. */
    bcm_gport_t gport;    /* Gport identifier. */
    bcm_gport_t tx_gport;    /* TX gport associated with this endpoint. */
    bcm_gport_t remote_gport;    /* Gport identifier of trapping destination: OAMP or CPU, local or remote. */
    int bfd_period;    /* For local endpoints, this is the BFD transmission period in ms. */
    bcm_vpn_t vpn;    /* VPN. */
    uint8 vlan_pri;    /* VLAN tag priority. */
    uint8 inner_vlan_pri;    /* Inner VLAN tag priority. */
    bcm_vrf_t vrf_id;    /* Vrf identifier. */
    bcm_ip_t dst_ip_addr;    /* Destination IPv4 address. */
    bcm_ip6_t dst_ip6_addr;    /* Destination IPv6 address. */
    bcm_ip_t src_ip_addr;    /* Source IPv4 address. */
    bcm_ip6_t src_ip6_addr;    /* Source IPv6 address. */
    uint8 ip_tos;    /* IPv4 Tos / IPv6 Traffic Class. */
    uint8 ip_ttl;    /* IPv4 TTL / IPv6 Hop Count. */
    bcm_ip_t inner_dst_ip_addr;    /* Inner destination IPv4 address. */
    bcm_ip6_t inner_dst_ip6_addr;    /* Inner destination IPv6 address. */
    bcm_ip_t inner_src_ip_addr;    /* Inner source IPv4 address. */
    bcm_ip6_t inner_src_ip6_addr;    /* Inner source IPv6 address. */
    uint8 inner_ip_tos;    /* Inner IPv4 Tos / IPv6 Traffic Class. */
    uint8 inner_ip_ttl;    /* Inner IPv4 TTL / IPv6 Hop Count. */
    uint32 udp_src_port;    /* UDP source port for Ipv4, Ipv6. */
    bcm_mpls_label_t label;    /* Incoming inner label. */
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
    bcm_vlan_t pkt_vlan_id;    /* TX Packet Vlan Id. */
    bcm_vlan_t rx_pkt_vlan_id;    /* RX Packet Vlan Id. */
    bcm_mpls_label_t gal_label;    /* MPLS GAL label. */
    uint32 faults;    /* Fault flags. */
    uint32 flags2;    /* Second set of control flags. */
    int ipv6_extra_data_index;    /* Pointer to first extended data entry */
    int punt_good_packet_period;    /* BFD good packets sampling period. Every punt_good_packet_period milliseconds, a single packet is punted to the CPU */
    uint8 bfd_period_cluster;    /* Tx period group. All MEPs in a group must be created with the same Tx period. Modifying the Tx period in one MEP affects the others. */
} bcm_compat6518_bfd_endpoint_info_t;

/* Create or update an BFD endpoint object */
extern int bcm_compat6518_bfd_endpoint_create(
    int unit,
    bcm_compat6518_bfd_endpoint_info_t *endpoint_info);

/* Get an BFD endpoint object. */
extern int bcm_compat6518_bfd_endpoint_get(
    int unit,
    bcm_bfd_endpoint_t endpoint,
    bcm_compat6518_bfd_endpoint_info_t *endpoint_info);
#endif /* defined(INCLUDE_BFD) */

/* OAM action key. */
typedef struct bcm_compat6518_oam_action_key_s {
    uint32 flags; 
    bcm_oam_opcode_t opcode; 
    bcm_oam_dest_mac_type_t dest_mac_type; 
    bcm_oam_match_type_t endpoint_type; 
    uint8 inject; 
} bcm_compat6518_oam_action_key_t;

/* Function return action. */
extern int bcm_compat6518_oam_profile_action_get(
    int unit, 
    uint32 flags, 
    bcm_oam_profile_t profile_id, 
    bcm_compat6518_oam_action_key_t *oam_action_key, 
    bcm_oam_action_result_t *oam_action_result);

/* Function return action. */
extern int bcm_compat6518_oam_profile_action_set(
    int unit, 
    uint32 flags, 
    bcm_oam_profile_t profile_id, 
    bcm_compat6518_oam_action_key_t *oam_action_key, 
    bcm_oam_action_result_t *oam_action_result);

/*
 * Flex counter index operation structure.
 *
 * This data structure lists the operation of a flex counter index.
 * Flex counter index would be generated as below:
 *      value0 = (SEL(object0) >> shift0) & ((1 << mask_size0) - 1)).
 *      value1 = (SEL(object1) & ((1 << mask_size1) - 1)) << shift1.
 *      index = (value1 | value0).
 */
typedef struct bcm_compat6518_flexctr_index_operation_s {
    bcm_flexctr_object_t object[BCM_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Flex counter object array. */
    uint32 quant_id[BCM_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Quantization ID. */
    uint8 mask_size[BCM_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Object mask size array. */
    uint8 shift[BCM_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Object shift array. */
} bcm_compat6518_flexctr_index_operation_t;

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
typedef struct bcm_compat6518_flexctr_value_operation_s {
    bcm_flexctr_value_select_t select;  /* Select counter value. */
    bcm_flexctr_object_t object[BCM_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Flex counter object array. */
    uint32 quant_id[BCM_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Quantization ID. */
    uint8 mask_size[BCM_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Object mask size array. */
    uint8 shift[BCM_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Object shift array. */
    bcm_flexctr_value_operation_type_t type; /* Counter value operation type. */
} bcm_compat6518_flexctr_value_operation_t;

/*
 * Flex counter trigger structure.
 *
 * This data structure lists the properties of the flex counter trigger.
 */
typedef struct bcm_compat6518_flexctr_trigger_s {
    bcm_flexctr_trigger_type_t trigger_type; /* Trigger type. */
    uint8 period_num;                   /* Time trigger period number. */
    bcm_flexctr_trigger_interval_t interval; /* Time trigger interval scale. */
    bcm_flexctr_object_t object;        /* Flex counter object. */
    uint16 object_value_start;          /* Conditional trigger start value. */
    uint16 object_value_stop;           /* Conditional trigger stop value. */
    uint16 object_value_mask;           /* Conditional trigger mask. */
} bcm_compat6518_flexctr_trigger_t;

/*
 * Flex counter action structure.
 *
 * This data structure lists the properties of a flex counter action.
 */
typedef struct bcm_compat6518_flexctr_action_s {
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
    bcm_compat6518_flexctr_index_operation_t index_operation; /* Flex counter index operation
                                           structure. */
    bcm_compat6518_flexctr_value_operation_t operation_a; /* Flex counter value A operation
                                           structure. */
    bcm_compat6518_flexctr_value_operation_t operation_b; /* Flex counter value B operation
                                           structure. */
    bcm_compat6518_flexctr_trigger_t trigger;      /* Flex counter trigger structure. */
} bcm_compat6518_flexctr_action_t;

/* Flex counter action creation function. */
extern int bcm_compat6518_flexctr_action_create(
    int unit,
    int options,
    bcm_compat6518_flexctr_action_t *action,
    uint32 *stat_counter_id);

/* Flex counter action get function. */
extern int bcm_compat6518_flexctr_action_get(
    int unit,
    uint32 stat_counter_id,
    bcm_compat6518_flexctr_action_t *action);

/*
 * Flex counter quantization structure.
 *
 * This data structure lists the properties of a flex counter
 * quantization.
 * Each quantization instance generates a range check result.
 */
typedef struct bcm_compat6518_flexctr_quantization_s {
    bcm_flexctr_object_t object;        /* Flex counter object. */
    bcm_pbmp_t ports;                   /* Flex counter ports. */
    uint32 range_check_min[BCM_FLEXCTR_QUANTIZATION_RANGE_SIZE]; /* Range check min value array. */
    uint32 range_check_max[BCM_FLEXCTR_QUANTIZATION_RANGE_SIZE]; /* Range check max value array. */
    uint8 range_num;                    /* Total range check number */
} bcm_compat6518_flexctr_quantization_t;

/* Flex counter quantization creation function. */
extern int bcm_compat6518_flexctr_quantization_create(
    int unit,
    int options,
    bcm_compat6518_flexctr_quantization_t *quantization,
    uint32 *quant_id);

/* Flex counter quantization get function. */
extern int bcm_compat6518_flexctr_quantization_get(
    int unit,
    uint32 quant_id,
    bcm_compat6518_flexctr_quantization_t *quantization);

/* EBST data buffer info structure */
typedef struct bcm_compat6518_cosq_ebst_data_entry_s {
    uint64 timestamp;                   /* Timestamp */
    uint32 bytes;                       /* Buffer occupancy level in bytes */
} bcm_compat6518_cosq_ebst_data_entry_t;

/* Get EBST stat for a given stat Id. */
extern int bcm_compat6518_cosq_ebst_data_get(
    int unit,
    bcm_cosq_object_id_t *object_id,
    bcm_bst_stat_id_t bid,
    int array_size,
    bcm_compat6518_cosq_ebst_data_entry_t *entry_array,
    int *count);

/*
 * Packet control action structure, common for integrity check API and
 * protocol control API.
 */
typedef struct bcm_compat6518_switch_pkt_control_action_t_s {
    uint8 discard;          /* Packet is discarded. */
    uint8 copy_to_cpu;      /* Packet is copied to CPU. */
    uint8 flood;            /* Packet is flooded to the forwarding domain. */
    uint8 bpdu;             /* Packet is BPDU. */
    uint8 opaque_ctrl_id;   /* Opaque control ID. */
} bcm_compat6518_switch_pkt_control_action_t;

/* Packet protocol match structure. */
typedef struct bcm_compat6518_switch_pkt_protocol_match_s {
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
} bcm_compat6518_switch_pkt_protocol_match_t;

/* An API to add a protocol control entry. */
extern int bcm_compat6518_switch_pkt_protocol_control_add(
    int unit,
    uint32 options,
    bcm_compat6518_switch_pkt_protocol_match_t *match,
    bcm_compat6518_switch_pkt_control_action_t *action,
    int priority);

/* An API to get action and priority for a protocol control entry. */
extern int bcm_compat6518_switch_pkt_protocol_control_get(
    int unit,
    bcm_compat6518_switch_pkt_protocol_match_t *match,
    bcm_compat6518_switch_pkt_control_action_t *action,
    int *priority);

/* An API to delete a protocol control entry. */
extern int bcm_compat6518_switch_pkt_protocol_control_delete(
    int unit,
    bcm_compat6518_switch_pkt_protocol_match_t *match);

/* Add an entry into packet integrity check table. */
extern int bcm_compat6518_switch_pkt_integrity_check_add(
    int unit,
    uint32 options,
    bcm_switch_pkt_integrity_match_t *match,
    bcm_compat6518_switch_pkt_control_action_t *action,
    int priority);

/* Get an entry from packet integrity check table. */
extern int bcm_compat6518_switch_pkt_integrity_check_get(
    int unit,
    bcm_switch_pkt_integrity_match_t *match,
    bcm_compat6518_switch_pkt_control_action_t *action,
    int *priority);

/* Initialize a VLAN tag action set structure. */
typedef struct bcm_compat6518_vlan_action_set_s {
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
    bcm_vlan_action_t outer_tag;        /* Outer-tag action for all tagged
                                          packets. */
    bcm_vlan_action_t inner_tag;        /* Inner_tag action for all tagged
                                          packets. */
    bcm_vlan_t forwarding_domain;       /* Forwarding domain (VLAN or VPN). */
    int inner_qos_map_id;               /* Map ID of inner QOS profile */
    int outer_qos_map_id;               /* Map ID of outer QOS profile. */
} bcm_compat6518_vlan_action_set_t;

/*Get or set the port's default VLAN tag actions. */
extern int bcm_compat6518_vlan_port_default_action_get(
    int unit,
    bcm_port_t port,
    bcm_compat6518_vlan_action_set_t *action);

/*Get or set the port's default VLAN tag actions. */
extern int bcm_compat6518_vlan_port_default_action_set(
    int unit,
    bcm_port_t port,
    bcm_compat6518_vlan_action_set_t *action);

/*Get or set the egress default VLAN tag actions. */
extern int bcm_compat6518_vlan_port_egress_default_action_get(
    int unit,
    bcm_port_t port,
    bcm_compat6518_vlan_action_set_t *action);

/*Get or set the egress default VLAN tag actions. */
extern int bcm_compat6518_vlan_port_egress_default_action_set(
    int unit,
    bcm_port_t port,
    bcm_compat6518_vlan_action_set_t *action);

/*
 * Add protocol based VLAN with specified action. If the entry already
 * exists, update the action.
 */
extern int bcm_compat6518_vlan_port_protocol_action_add(
    int unit,
    bcm_port_t port,
    bcm_port_frametype_t frame,
    bcm_port_ethertype_t ether,
    bcm_compat6518_vlan_action_set_t *action);

/*Get protocol based VLAN action. */
extern int bcm_compat6518_vlan_port_protocol_action_get(
    int unit,
    bcm_port_t port,
    bcm_port_frametype_t frame,
    bcm_port_ethertype_t ether,
    bcm_compat6518_vlan_action_set_t *action);

/*
 * Add an association from MAC address to VLAN actions, which are
 *             used for VLAN tag/s assignment to untagged packets.
 */
extern int bcm_compat6518_vlan_mac_action_add(
    int unit,
    bcm_mac_t mac,
    bcm_compat6518_vlan_action_set_t *action);

/*
 * Retrive an association from MAC address to VLAN actions, which
 *             are used for VLAN tag assignment to untagged packets.
 */
extern int bcm_compat6518_vlan_mac_action_get(
    int unit,
    bcm_mac_t mac,
    bcm_compat6518_vlan_action_set_t *action);

/*Add an entry to the VLAN Translation table and assign VLAN actions. */
extern int bcm_compat6518_vlan_translate_action_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6518_vlan_action_set_t *action);

/*Add an entry to the VLAN Translation table and assign VLAN actions. */
extern int bcm_compat6518_vlan_translate_action_create(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6518_vlan_action_set_t *action);

/*Get an entry to the VLAN Translation table and assign VLAN actions. */
extern int bcm_compat6518_vlan_translate_action_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6518_vlan_action_set_t *action);

/*
 * Add an entry to the egress VLAN Translation table and assign VLAN
 * actions.
 */
extern int bcm_compat6518_vlan_translate_egress_action_add(
    int unit,
    int port_class,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6518_vlan_action_set_t *action);

/*
 * Get an entry to the egress VLAN Translation table and assign VLAN
 * actions.
 */
extern int bcm_compat6518_vlan_translate_egress_action_get(
    int unit,
    int port_class,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6518_vlan_action_set_t *action);

/*
 * Add an entry based on gport and VLANs to the egress VLAN Translation
 * table and assign VLAN actions.
 */
extern int bcm_compat6518_vlan_translate_egress_gport_action_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6518_vlan_action_set_t *action);

/*
 * Get VLAN actions for an entry based on gport and VLANS in the egress
 * VLAN Translation table.
 */
extern int bcm_compat6518_vlan_translate_egress_gport_action_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6518_vlan_action_set_t *action);


/*bcm_vlan_translate_action_range_add */
extern int bcm_compat6518_vlan_translate_action_range_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan_low,
    bcm_vlan_t outer_vlan_high,
    bcm_vlan_t inner_vlan_low,
    bcm_vlan_t inner_vlan_high,
    bcm_compat6518_vlan_action_set_t *action);

/*bcm_vlan_translate_action_range_get */
extern int bcm_compat6518_vlan_translate_action_range_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan_low,
    bcm_vlan_t outer_vlan_high,
    bcm_vlan_t inner_vlan_low,
    bcm_vlan_t inner_vlan_high,
    bcm_compat6518_vlan_action_set_t *action);

/*Set a VLAN translation ID instance with tag actions */
extern int bcm_compat6518_vlan_translate_action_id_set(
    int unit,
    uint32 flags,
    int action_id,
    bcm_compat6518_vlan_action_set_t *action);

/*Get tag actions from a VLAN translation ID instance */
extern int bcm_compat6518_vlan_translate_action_id_get(
    int unit,
    uint32 flags,
    int action_id,
    bcm_compat6518_vlan_action_set_t *action);

/*
 * Add an association from IP subnet to VLAN actions to use for assigning
 * VLAN tag actions to untagged packets.
 */
extern int bcm_compat6518_vlan_ip_action_add(
    int unit,
    bcm_vlan_ip_t *vlan_ip,
    bcm_compat6518_vlan_action_set_t *action);

/*
 * Get an association from IP subnet to VLAN actions to use for assigning
 * VLAN tag actions to untagged packets.
 */
extern int bcm_compat6518_vlan_ip_action_get(
    int unit,
    bcm_vlan_ip_t *vlan_ip,
    bcm_compat6518_vlan_action_set_t *action);

/*Add action for match criteria for Vlan assignment. */
extern int bcm_compat6518_vlan_match_action_add(
    int unit,
    uint32 options,
    bcm_vlan_match_info_t *match_info,
    bcm_compat6518_vlan_action_set_t *action_set);

/*Get action for given match criteria. */
extern int bcm_compat6518_vlan_match_action_get(
    int unit,
    bcm_vlan_match_info_t *match_info,
    bcm_compat6518_vlan_action_set_t *action_set);

/*Get all actions and match criteria for given vlan match. */
extern int bcm_compat6518_vlan_match_action_multi_get(
    int unit,
    bcm_vlan_match_t match,
    int size,
    bcm_vlan_match_info_t *match_info_array,
    bcm_compat6518_vlan_action_set_t *action_set_array,
    int *count);

/*Add VLAN actions to a field entry. */
extern int bcm_compat6518_field_action_vlan_actions_add(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_compat6518_vlan_action_set_t *vlan_action_set);

/*Retrieve VLAN actions configured to a field entry. */
extern int bcm_compat6518_field_action_vlan_actions_get(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_compat6518_vlan_action_set_t *vlan_action_set);

/* VLAN/VSAN Translation Action Set. */
typedef struct bcm_compat6518_fcoe_vsan_action_set_t {
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
    bcm_compat6518_vlan_action_set_t vlan_action;  /* VLAN actions */
} bcm_compat6518_fcoe_vsan_action_set_t;

/* Add VSAN translation action */
extern int bcm_compat6518_fcoe_vsan_translate_action_add(
    int unit,
    bcm_fcoe_vsan_translate_key_config_t *key,
    bcm_compat6518_fcoe_vsan_action_set_t *action);

/* Generic port match attribute structure */
typedef struct bcm_compat6518_port_match_info_s {
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
    bcm_compat6518_vlan_action_set_t *action;
                                        /* Match action */
    uint16 extended_port_vid;           /* Extender port VID */
    bcm_vpn_t vpn;                      /* VPN ID */
    uint16 niv_port_vif;                /* NIV port VIF */
    uint32 isid;
    bcm_mac_t src_mac;                  /* Source Mac Address */
    uint32 port_class;                  /* Port Class Id */
}bcm_compat6518_port_match_info_t;

/* Add a match to an existing port */
extern int bcm_compat6518_port_match_add(
    int unit,
    bcm_gport_t port,
    bcm_compat6518_port_match_info_t *match);

/* Remove a match from an existing port */
extern int bcm_compat6518_port_match_delete(
    int unit,
    bcm_gport_t port,
    bcm_compat6518_port_match_info_t *match);

/* Replace an old match with a new one for an existing port */
extern int bcm_compat6518_port_match_replace(
    int unit,
    bcm_gport_t port,
    bcm_compat6518_port_match_info_t *old_match,
    bcm_compat6518_port_match_info_t *new_match);

/* Get all the matches for an existing port */
extern int bcm_compat6518_port_match_multi_get(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_compat6518_port_match_info_t *match_array,
    int *count);

/* Assign a set of matches to an existing port */
extern int bcm_compat6518_port_match_set(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_compat6518_port_match_info_t *match_array);

/* Port Configuration structure. */
typedef struct bcm_compat6518_port_config_s {
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
} bcm_compat6518_port_config_t;

/* Retrieved the port configuration for the specified device. */
extern int bcm_compat6518_port_config_get(
    int unit,
    bcm_compat6518_port_config_t *config);

/* Offsets of payload, timestamp and sequence number in user defined header */
typedef struct bcm_compat6518_sat_header_user_define_offsets_s {
    int payload_offset;    /* Payload offset */
    int timestamp_offset;    /* Timestamp offset */
    int seq_number_offset;    /* Sequence number offset */
    int crc_byte_offset;    /* offset of CRC covered byte */
} bcm_compat6518_sat_header_user_define_offsets_t;

/* Sat ctf packet config */
typedef struct bcm_compat6518_sat_ctf_packet_info_s {
    bcm_sat_header_type_t sat_header_type;    /* Sat header type */
    bcm_sat_payload_t payload;    /* Sat payload */
    uint32 flags;    /*  */
    bcm_compat6518_sat_header_user_define_offsets_t offsets;    /* Offsets of payload, timestamp and sequence number in user defined header */
} bcm_compat6518_sat_ctf_packet_info_t;

/* Set ctf packet format. */
extern int bcm_compat6518_sat_ctf_packet_config(
    int unit,
    bcm_sat_ctf_t ctf_id,
    bcm_compat6518_sat_ctf_packet_info_t *packet_info);

typedef struct bcm_compat6518_info_s {
    uint32 vendor;      /* PCI values used usually. */
    uint32 device;      /* PCI values used usually. */
    uint32 revision;    /* PCI values used usually. */
    uint32 capability;
    int num_pipes;      /* Number of pipes in the device. */
    int num_pp_pipes;   /* Number of packet processing pipes in the device. */
} bcm_compat6518_info_t;

/* Get information about a BCM unit. */
extern int bcm_compat6518_info_get(
    int unit,
    bcm_compat6518_info_t *info);

/*
 * IPFIX header in the encap used to send the data packet to the remote
 * collector.
 */
typedef struct bcm_compat6518_collector_ipfix_header_s {
    int version;                        /* Ipfix Protocol Version. */
    int initial_sequence_num;           /* Ipfix initial sequence number. */
} bcm_compat6518_collector_ipfix_header_t;

/* Collector information. */
typedef struct bcm_compat6518_collector_info_s {
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
    bcm_compat6518_collector_ipfix_header_t ipfix; /* IPFIX encapsulation of the packet
                                           sent to collector. */
    bcm_collector_protobuf_header_t protobuf; /* Protobuf header information. */
    bcm_collector_timestamp_source_t src_ts; /* Timestamp source info. */
} bcm_compat6518_collector_info_t;

/* Create a collector with given collector info. */
extern int bcm_compat6518_collector_create(
        int unit,
        uint32 options,
        bcm_collector_t *collector_id,
        bcm_compat6518_collector_info_t *collector_info);

/* Get collector information with ID. */
extern int bcm_compat6518_collector_get(
        int unit,
        bcm_collector_t id,
        bcm_compat6518_collector_info_t *collector_info);

/* IFA statistics information data. */
typedef struct bcm_compat6518_ifa_stat_info_s {
    uint32 rx_pkt_cnt;                  /* Number of packet recieved in FW. */
    uint32 tx_pkt_cnt;                  /* Number of packet transmitted from FW. */
    uint32 ifa_no_config_drop;          /* Number of pkt dropped due to missing
                                           configuration. */
    uint32 ifa_collector_not_present_drop; /* Number of pkt dropped due to missing
                                              collector configuration. */
    uint32 ifa_hop_cnt_invalid_drop;    /* Number of pkt dropped due to Hop
                                           count and Hop limit are out of order. */
    uint32 ifa_int_hdr_len_invalid_drop; /* Number of pkt dropped due to maximum
                                            length and current length are out of
                                            order. */
    uint32 ifa_pkt_size_invalid_drop;   /* Number of pkt dropped due to invalid
                                           packet size. */
    uint32 rx_pkt_length_exceed_max_drop_count; /* Number of Rx packets dropped due to
                                                   its length exceeding the max length
                                                   configured using the config property
                                                   ifa_rx_pkt_max_length. */
    uint32 rx_pkt_parse_error_drop_count; /* Number of Rx packets dropped due to
                                             errors in parsing. */
    uint32 rx_pkt_unknown_namespace_drop_count; /* Number of Rx packets dropped as the
                                                   namespace is unknown. */
    uint32 rx_pkt_excess_rate_drop_count; /* Number of Rx packets dropped as the
                                             incoming rate is too high to process. */
    uint32 tx_record_count;             /* Number of IFA records exported to
                                           collector. */
    uint32 tx_pkt_failure_count;        /* Number of Tx packets that could not
                                           be exported due to some failure. */
} bcm_compat6518_ifa_stat_info_t;

/* In-band flow analyzer - IFA statistics information */
extern int bcm_compat6518_ifa_stat_info_set(
    int unit,
    bcm_compat6518_ifa_stat_info_t *stat_data);

/* In-band flow analyzer - IFA statistics information */
extern int bcm_compat6518_ifa_stat_info_get(
    int unit,
    bcm_compat6518_ifa_stat_info_t *stat_data);

/* This structure contains the configuration of a VLAN. */
typedef struct bcm_compat6518_vlan_control_vlan_s {
    bcm_vrf_t vrf;    /*  */
    bcm_vlan_t forwarding_vlan;    /* Shared VLAN ID. */
    bcm_if_t ingress_if;    /* Mapped Ingress interface. */
    uint16 outer_tpid;    /*  */
    uint32 flags;    /*  */
    bcm_vlan_mcast_flood_t ip6_mcast_flood_mode;    /*  */
    bcm_vlan_mcast_flood_t ip4_mcast_flood_mode;    /*  */
    bcm_vlan_mcast_flood_t l2_mcast_flood_mode;    /*  */
    int if_class;    /*  */
    bcm_vlan_forward_t forwarding_mode;    /*  */
    bcm_vlan_urpf_mode_t urpf_mode;    /*  */
    bcm_cos_queue_t cosq;    /*  */
    int qos_map_id;    /* index to int_pri->queue offset mapping profile */
    bcm_fabric_distribution_t distribution_class;    /* Fabric Distribution Class. */
    bcm_multicast_t broadcast_group;    /* Group to handle broadcast frames */
    bcm_multicast_t unknown_multicast_group;    /* Group to handle unknown multicast frames */
    bcm_multicast_t unknown_unicast_group;    /* Group to handle unknown unicast frames */
    bcm_multicast_t trill_nonunicast_group;    /* Group to handle trill-domain nonunicast frames */
    bcm_trill_name_t source_trill_name;    /* Source RBridge nickname per VLAN */
    int trunk_index;    /* Trunk index for static PSC */
    bcm_vlan_protocol_packet_ctrl_t protocol_pkt;    /* Protocol packet control per VLAN */
    int nat_realm_id;    /* Realm id of interface that this vlan maps to */
    int l3_if_class;    /* L3IIF class id. */
    bcm_vlan_vp_mc_ctrl_t vp_mc_ctrl;    /* VP replication control, Auto, Enable, Disable */
    int aging_cycles;    /* number of aging meta-cycles */
    int entropy_id;    /* aging profile ID */
    bcm_vpn_t vpn;    /* vpn */
    bcm_vlan_t egress_vlan;    /* egress outer vlan */
    uint32 learn_flags;    /* Learn control flags for VLAN-based (BCM_VLAN_LEARN_CONTROL_XXX) */
    uint32 sr_flags;    /* Flags for Seamless Redundancy: BCM_VLAN_CONTROL_SR_FLAG_xxx */
    uint32 flags2;    /* BCM_VLAN_FLAGS2_XXX */
    uint32 ingress_stat_id;    /* Object statistics id ingress. */
    int ingress_stat_pp_profile;    /* Statistic profile ingress. */
    uint32 egress_stat_id;    /* Object statistics id egress. */
    int egress_stat_pp_profile;    /* Statistic profile egress. */
    bcm_compat6518_vlan_action_set_t egress_action;    /* Egress VLAN actions. */
    bcm_vlan_l2_view_t l2_view;    /* L2 table view. */
    int egress_class_id;    /* Class ID for EFP qualifier. */
    int egress_opaque_ctrl_id;    /* Egress opaque control ID. */
    int ingress_opaque_ctrl_id;    /* Ingress opaque control ID. */
} bcm_compat6518_vlan_control_vlan_t;

/* Set or retrieve current VLAN properties selectively. */
extern int bcm_compat6518_vlan_control_vlan_selective_get(
    int unit,
    bcm_vlan_t vlan,
    uint32 valid_fields,
    bcm_compat6518_vlan_control_vlan_t *control);

/* Set or retrieve current VLAN properties selectively. */
extern int bcm_compat6518_vlan_control_vlan_selective_set(
    int unit,
    bcm_vlan_t vlan,
    uint32 valid_fields,
    bcm_compat6518_vlan_control_vlan_t *control);

/* Set or retrieve current VLAN properties. */
extern int bcm_compat6518_vlan_control_vlan_get(
    int unit,
    bcm_vlan_t vlan,
    bcm_compat6518_vlan_control_vlan_t *control);

/* Set or retrieve current VLAN properties. */
extern int bcm_compat6518_vlan_control_vlan_set(
    int unit,
    bcm_vlan_t vlan,
    bcm_compat6518_vlan_control_vlan_t control);

/* Port Resource Configuration */
typedef struct bcm_compat6518_port_resource_s {
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
} bcm_compat6518_port_resource_t;

/*
 * Perform a FlexPort operation changing the port resources for a given
 * logical port.
 */
extern int bcm_compat6518_port_resource_set(
    int unit,
    bcm_gport_t port,
    bcm_compat6518_port_resource_t *resource);

/* Get the port resource configuration. */
extern int bcm_compat6518_port_resource_get(
    int unit,
    bcm_gport_t port,
    bcm_compat6518_port_resource_t *resource);

/*
 * Get suggested default resource values prior to calling
 * bcm_port_resource_set.
 */
extern int bcm_compat6518_port_resource_default_get(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    bcm_compat6518_port_resource_t *resource);

/*
 * Performs a FlexPort operation changing the port resources for a set of
 * ports.
 */
extern int bcm_compat6518_port_resource_multi_set(
    int unit,
    int nport,
    bcm_compat6518_port_resource_t *resource);

/*
 * Validate a port's speed configuration on the BCM56980 by fetching the
 * affected ports.
 */
extern int bcm_compat6518_port_resource_speed_config_validate(
    int unit,
    bcm_compat6518_port_resource_t *resource,
    bcm_pbmp_t *pbmp);

/* Get/Modify the speed for a given port. */
extern int bcm_compat6518_port_resource_speed_get(
    int unit,
    bcm_gport_t port,
    bcm_compat6518_port_resource_t *resource);

/* Get/Modify the speed for a given port. */
extern int bcm_compat6518_port_resource_speed_set(
    int unit,
    bcm_gport_t port,
    bcm_compat6518_port_resource_t *resource);

/* Modify the Port Speeds for a set of ports in a single Port Macro. */
extern int bcm_compat6518_port_resource_speed_multi_set(
    int unit,
    int nport,
    bcm_compat6518_port_resource_t *resource);

/* Specify hints to be used during group creation. */
typedef struct bcm_compat6518_field_hint_s {
    bcm_field_hint_type_t hint_type;    /* Field hints of type
                                           bcm_field_hint_type_t. */
    bcm_field_qualify_t qual;           /* Field qualifier bcmFieldQualifyXXX. */
    uint32 max_values;                  /* To figure out the width of the
                                           compression output. */
    uint32 start_bit;                   /* Builds the range in conjunction with
                                           end_bit which will indicate that in
                                           the
                                                                   full mask any
                                           bits outside of that range are
                                           guaranteed to be 0. */
    uint32 end_bit;                     /* End bit of a mask for the qualifier
                                           in hint structure. */
    uint32 flags;                       /* Flags. */
    uint32 dosattack_event_flags;       /* Dos Attack Events Flags. */
    uint32 max_group_size;              /* Max size of the group. */
    int priority;                       /* Group priority. */
    bcm_udf_id_t udf_id;                /* UDF ID. */
    bcm_field_group_t group_id;         /* Original Group ID. */
} bcm_compat6518_field_hint_t;

/* Associate a hint structure to the hint id. */
extern int bcm_compat6518_field_hints_add(
    int unit,
    bcm_field_hintid_t hint_id,
    bcm_compat6518_field_hint_t *hint);

/* Get the complete hint structure from the hint id. */
extern int bcm_compat6518_field_hints_get(
    int unit,
    bcm_field_hintid_t hint_id,
    bcm_compat6518_field_hint_t *hint);

/* Remove a hint structure from the hint id. */
extern int bcm_compat6518_field_hints_delete(
    int unit,
    bcm_field_hintid_t hint_id,
    bcm_compat6518_field_hint_t *hint);
/*
 * Field entry configuration structure. Used to create a field entry with
 * specific attributes.
 */
typedef struct bcm_compat6518_field_entry_config_s {
    uint32 flags;               /* Field entry create flags
                                   BCM_FIELD_ENTRY_CREATE_XXX. */
    bcm_field_entry_t entry_id; /* Field entry. */
    bcm_field_group_t group_id; /* Group Id. */
    bcm_field_aset_t aset;      /* Default entry Action Set (currently
                                   applicable only for default entries). */
    int priority;               /* Entry priority. */
} bcm_compat6518_field_entry_config_t;

extern int bcm_compat6518_field_entry_config_create(
    int unit,
    bcm_compat6518_field_entry_config_t *entry_config);

#if defined(INCLUDE_L3)
/*
 * L3 Egress Structure.
 *
 * Description of an L3 forwarding destination.
 */
typedef struct bcm_compat6518_l3_egress_s {
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
    int mtu;                            /* MTU. */
    int nat_realm_id;                   /* NAT destination realm ID. */
    bcm_qos_egress_model_t egress_qos_model; /* egress qos and ttl model */
} bcm_compat6518_l3_egress_t;

/* Create an Egress forwarding object. */
extern int bcm_compat6518_l3_egress_create(
    int unit,
    uint32 flags,
    bcm_compat6518_l3_egress_t *egr,
    bcm_if_t *if_id);

/* Get an Egress forwarding object. */
extern int bcm_compat6518_l3_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6518_l3_egress_t *egr);

/* Find an interface pointing to an Egress forwarding object. */
extern int bcm_compat6518_l3_egress_find(
    int unit,
    bcm_compat6518_l3_egress_t *egr,
    bcm_if_t *intf);

/* Retrieve valid allocation for a given number of FECs. */
extern int bcm_compat6518_l3_egress_allocation_get(
    int unit,
    uint32 flags,
    bcm_compat6518_l3_egress_t *egr,
    uint32 nof_members,
    bcm_if_t *if_id);

/*
 * Set a failover egress object with fixed offset to protect primary NH
 * intf.
 */
extern int bcm_compat6518_failover_egress_set(
    int unit,
    bcm_if_t intf,
    bcm_compat6518_l3_egress_t *failover_egr);

/*
 * Get a failover egress object with fixed offset to protect primary NH
 * intf.
 */
extern int bcm_compat6518_failover_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6518_l3_egress_t *failover_egr);
#endif /* defined(INCLUDE_L3) */

/*
 * Flex counter group action structure.
 *
 * This data structure lists the properties of a flex counter group
 * action.
 */
typedef struct bcm_compat6518_flexctr_group_action_s {
    bcm_flexctr_source_t source;        /* Flex counter source. */
    bcm_pbmp_t ports;                   /* Flex counter ports. */
    uint32 stat_counter_id[BCM_FLEXCTR_GROUP_ACTION_SIZE]; /* Flex counter group action array. */
    uint8 action_num;                   /* Total group action array number */
} bcm_compat6518_flexctr_group_action_t;

/* Flex counter group action creation function. */
extern int bcm_compat6518_flexctr_group_action_create(
    int unit,
    int options,
    bcm_compat6518_flexctr_group_action_t *group_action,
    uint32 *group_stat_counter_id);

/* Flex counter group action get function. */
extern int bcm_compat6518_flexctr_group_action_get(
    int unit,
    uint32 group_stat_counter_id,
    bcm_compat6518_flexctr_group_action_t *group_action);

typedef struct bcm_compat6518_field_destination_match_s {
    int priority;                   /* Match priority. */
    bcm_gport_t gport;              /* Outgoing mod port or trunk. */
    uint32 gport_mask;              /* Outgoing mod port or trunk mask. */
    bcm_multicast_t mc_group;       /* Multicast group. */
    uint32 mc_group_mask;           /* Multicast group mask. */
    bcm_port_encap_t port_encap;    /* Incoming port encapsulation. */
    uint8 port_encap_mask;          /* Incoming port encapsulation mask. */
    bcm_color_t color;              /* Color. */
    uint8 color_mask;               /* Color mask. */
    bcm_color_t elephant_color;     /* Elephant color. */
    uint8 elephant_color_mask;      /* Elephant color mask. */
    uint8 elephant;                 /* Flowtracker elephant flow indicator. */
    uint8 elephant_mask;            /* Elephant mask. */
    uint8 elephant_notify;          /* Indicator that flow exceeds the elephant
                                       threshold byte count and becomes an
                                       elephant flow. */
    uint8 elephant_notify_mask;     /* elephant_notify mask. */
    int int_pri;                    /* Internal priority. */
    uint32 int_pri_mask;            /* Internal priority mask. */
    int opaque_object_2;            /* Opaque object 2 assigned in ingress
                                       field. */
    uint32 opaque_object_2_mask;    /* Opaque object 2 mask. */
} bcm_compat6518_field_destination_match_t;

/* Add a field destination entry. */
extern int bcm_compat6518_field_destination_entry_add(
    int unit,
    uint32 options,
    bcm_compat6518_field_destination_match_t *match,
    bcm_field_destination_action_t *action);

/* Get a field destination entry. */
extern int bcm_compat6518_field_destination_entry_get(
    int unit,
    bcm_compat6518_field_destination_match_t *match,
    bcm_field_destination_action_t *action);

/* Delete a field destination entry. */
extern int bcm_compat6518_field_destination_entry_delete(
    int unit,
    bcm_compat6518_field_destination_match_t *match);

/* Attach the statistic counter ID to a field destination entry. */
extern int bcm_compat6518_field_destination_stat_attach(
    int unit,
    bcm_compat6518_field_destination_match_t *match,
    uint32 stat_counter_id);

/* Get the statistic counter ID attached to a field destination entry. */
extern int bcm_compat6518_field_destination_stat_id_get(
    int unit,
    bcm_compat6518_field_destination_match_t *match,
    uint32 *stat_counter_id);

/* Detach the statistic counter ID attached to a field destination entry. */
extern int bcm_compat6518_field_destination_stat_detach(
    int unit,
    bcm_compat6518_field_destination_match_t *match);

/* Set the flex counter object value for a field destination entry. */
extern int bcm_compat6518_field_destination_flexctr_object_set(
    int unit,
    bcm_compat6518_field_destination_match_t *match,
    uint32 value);

/* Get the flex counter object value of a field destination entry. */
extern int bcm_compat6518_field_destination_flexctr_object_get(
    int unit,
    bcm_compat6518_field_destination_match_t *match,
    uint32 *value);

/* L2 Station address info. */
typedef struct bcm_compat6518_l2_station_s {
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
    bcm_l2_forward_domain_type_t forward_domain_type; /* L2 forwarding domain type. */
    uint8 forward_domain_type_mask;     /* L2 forwarding domain type mask. */
    bcm_vlan_t inner_vlan;              /* Inner VLAN to match. */
    bcm_vlan_t inner_vlan_mask;         /* Inner VLAN mask value. */
    bcm_vlan_t vfi;                     /* Port default virtual forwarding
                                           instance to match. */
    bcm_vlan_t vfi_mask;                /* Port default virtual forwarding
                                           instance mask. */
} bcm_compat6518_l2_station_t;

/* Add an entry to L2 Station Table. */
int bcm_compat6518_l2_station_add(
        int unit,
        int *station_id,
        bcm_compat6518_l2_station_t *station);

/* Get L2 station entry detail from Station Table. */
int bcm_compat6518_l2_station_get(
        int unit,
        int station_id,
        bcm_compat6518_l2_station_t *station);

/* Rx cosq mapping. */
typedef struct bcm_compat6518_rx_cosq_mapping_s {
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
    uint8 flex_key1;                   /* Flex key 1. */
    uint8 flex_key1_mask;              /* Mask for flex key 1. */
    bcm_rx_pkt_trace_reasons_t trace_reasons; /* Copy to CPU trace event reasons
                                           bitmap. */
    bcm_rx_pkt_trace_reasons_t trace_reasons_mask; /* Mask for copy to CPU trace event
                                           reasons bitmap. */
    bcm_pkt_drop_event_t drop_event;    /* Drop event. */
    bcm_pkt_drop_event_t drop_event_mask; /* Mask for drop event. */
    int priority;                       /* Entry priority. */
} bcm_compat6518_rx_cosq_mapping_t;

/* Map packets to a CPU COS queue. */
extern int bcm_compat6518_rx_cosq_mapping_extended_set(
    int unit,
    uint32 options,
    bcm_compat6518_rx_cosq_mapping_t *rx_cosq_mapping);

/* Get packets to CPU COS queue mappings. */
extern int bcm_compat6518_rx_cosq_mapping_extended_get(
    int unit,
    bcm_compat6518_rx_cosq_mapping_t *rx_cosq_mapping);

/* Add a CPU COS map. */
extern int bcm_compat6518_rx_cosq_mapping_extended_add(
    int unit,
    uint32 options,
    bcm_compat6518_rx_cosq_mapping_t *rx_cosq_mapping);

/* Delete a CPU COS map. */
extern int bcm_compat6518_rx_cosq_mapping_extended_delete(
    int unit,
    bcm_compat6518_rx_cosq_mapping_t *rx_cosq_mapping);

/* Structure that is used in SRv6 SRH Base initiator APIs, for managing device behavior at SRv6 Tunnel Encapsulation */
typedef struct bcm_compat6518_srv6_srh_base_initiator_info_s {
    uint32 flags;    /* BCM_SRV6_SRH_BASE_INITIATOR_XXX */
    bcm_gport_t tunnel_id;    /* Tunnel SRv6 SRH base object ID */
    int nof_sids;    /* Number of SIDs in the SID list */
    int qos_map_id;    /* QOS map identifier */
    int ttl;    /* Tunnel header TTL */
    int dscp;    /* Tunnel header DSCP value */
    bcm_qos_egress_model_t egress_qos_model;    /* Egress qos and ttl model */
    bcm_if_t next_encap_id;    /* Next encapsulation ID */
} bcm_compat6518_srv6_srh_base_initiator_info_t;

/* Create/Get/Delete/Traverse SRv6 SRH Base properties */
extern int bcm_compat6518_srv6_srh_base_initiator_create(
    int unit,
    bcm_compat6518_srv6_srh_base_initiator_info_t *info);

/* Create/Get/Delete/Traverse SRv6 SRH Base properties */
extern int bcm_compat6518_srv6_srh_base_initiator_get(
    int unit,
    bcm_compat6518_srv6_srh_base_initiator_info_t *info);

/* Create/Get/Delete/Traverse SRv6 SRH Base properties */
extern int bcm_compat6518_srv6_srh_base_initiator_delete(
    int unit,
    bcm_compat6518_srv6_srh_base_initiator_info_t *info);


/* CPRI Encap Information */
typedef struct bcm_compat6518_cpri_encap_info_s {
    uint32 flags;    /* CPRI Encap Info flags */
    uint16 flow_id;    /*  Flow ID  */
    bcm_cpri_roe_subtype_t subtype;    /* RoE subtype */
    bcm_cpri_hdr_encap_type_t hdr_type;    /* CPRI header encapsulation type */
    int roe_ordering_info_index;    /* RoE ordering Info index */
    int macda_index;    /* Index to MAC DA address */
    int macsa_index;    /* Index to MAC SA address */
    bcm_cpri_vlan_type_t vlan_type;    /* CPRI Vlan type */
    int vlan0_idx;    /* VLAN 0 Table index */
    int vlan1_idx;    /*  VLAN 1 table index */
    int vlan0_priority;    /*  VLAN 0 priority */
    int vlan1_priority;    /*  VLAN 1 priority */
    uint8 vlan_ethertype_idx;    /*  VLAN EtherType index */
    bcm_cpri_roe_opcode_t opcode;    /* RoE opcode */
    bcm_cpri_roe_frame_format_t type;    /*  RoE Frame Header Formatt */
    uint8 version;    /*  Header Version used in RSVD3 */
    uint32 ecpri_pc_id;    /*  PC_ID of eCPRI header, 16 bits for both eCPRI8 and eCPRI12 */
    uint32 ecpri_msg_type;    /*  Message Type eCPRI header, 8 bits only */
} bcm_compat6518_cpri_encap_info_t;

/* Configure/Retrieve the encapsulation header info for a given AxC ID. */
extern int bcm_compat6518_cpri_port_encap_set(
    int unit,
    bcm_gport_t port,
    int queue_num,
    bcm_compat6518_cpri_encap_info_t *encap_config);

/* Configure/Retrieve the encapsulation header info for a given AxC ID. */
extern int bcm_compat6518_cpri_port_encap_get(
    int unit,
    bcm_gport_t port,
    int queue_num,
    bcm_compat6518_cpri_encap_info_t *encap_config);

/* Device-independent L2 egress structure. */
typedef struct bcm_compat6518_l2_egress_s {
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
} bcm_compat6518_l2_egress_t;

/* Create an l2 egress entry. */
extern int bcm_compat6518_l2_egress_create(
    int unit,
    bcm_compat6518_l2_egress_t *egr);

/* Get the config of an l2 egress entry with specified encap_id. */
extern int bcm_compat6518_l2_egress_get(
    int unit,
    bcm_if_t encap_id,
    bcm_compat6518_l2_egress_t *egr);

/* Find the encap_id with a given egress config */
extern int bcm_compat6518_l2_egress_find(
    int unit,
    bcm_compat6518_l2_egress_t *egr,
    bcm_if_t *encap_id);

#if defined(INCLUDE_L3)
/* L3 tunneling terminator. */
typedef struct bcm_compat6518_tunnel_terminator_s {
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
} bcm_compat6518_tunnel_terminator_t;

/* Add a tunnel terminator for DIP-SIP key. */
extern int bcm_compat6518_tunnel_terminator_add(
    int unit,
    bcm_compat6518_tunnel_terminator_t *info);

/* Add a tunnel terminator for DIP-SIP key. */
extern int bcm_compat6518_tunnel_terminator_create(
    int unit,
    bcm_compat6518_tunnel_terminator_t *info);

/* Delete a tunnel terminator for DIP-SIP key. */
extern int bcm_compat6518_tunnel_terminator_delete(
    int unit,
    bcm_compat6518_tunnel_terminator_t *info);

/* Update a tunnel terminator for DIP-SIP key. */
extern int bcm_compat6518_tunnel_terminator_update(
    int unit,
    bcm_compat6518_tunnel_terminator_t *info);

/* Get a tunnel terminator for DIP-SIP key. */
extern int bcm_compat6518_tunnel_terminator_get(
    int unit,
    bcm_compat6518_tunnel_terminator_t *info);

/* Create VXLAN Tunnel terminator. */
extern int bcm_compat6518_vxlan_tunnel_terminator_create(
    int unit,
    bcm_compat6518_tunnel_terminator_t *info);

/* Update multicast/active state per VXLAN Tunnel terminator. */
extern int bcm_compat6518_vxlan_tunnel_terminator_update(
    int unit,
    bcm_compat6518_tunnel_terminator_t *info);

/* Get VXLAN Tunnel terminator. */
extern int bcm_compat6518_vxlan_tunnel_terminator_get(
    int unit,
    bcm_compat6518_tunnel_terminator_t *info);

/* Create L2GRE Tunnel terminator. */
extern int bcm_compat6518_l2gre_tunnel_terminator_create(
    int unit,
    bcm_compat6518_tunnel_terminator_t *info);

/* Update L2GRE Tunnel terminator. */
extern int bcm_compat6518_l2gre_tunnel_terminator_update(
    int unit,
    bcm_compat6518_tunnel_terminator_t *info);

/* Get L2GRE Tunnel terminator. */
extern int bcm_compat6518_l2gre_tunnel_terminator_get(
    int unit,
    bcm_compat6518_tunnel_terminator_t *info);

#endif /* defined(INCLUDE_L3) */

/*
 * Flex counter configuration structure. Used to attach or detach the
 * flex counter to field entry.
 */
typedef struct bcm_compat6518_field_flexctr_config_s {
    uint32 flexctr_action_id;   /* Flex counter action ID. */
    uint32 counter_index;       /* Index to the counters associated to the flex
                                   counter action. */
} bcm_compat6518_field_flexctr_config_t;

/* Attach the flexctr for a given field entry. */
extern int bcm_compat6518_field_entry_flexctr_attach(
    int unit,
    bcm_field_entry_t entry,
    bcm_compat6518_field_flexctr_config_t *flexctr_cfg);

/* Detach the flexctr for a given field entry. */
extern int bcm_compat6518_field_entry_flexctr_detach(
    int unit,
    bcm_field_entry_t entry,
    bcm_compat6518_field_flexctr_config_t *flexctr_cfg);

/* PP statistic profile. */
typedef struct bcm_compat6518_stat_pp_profile_info_s {
    int counter_command_id;    /* Command id (interface id). */
    int stat_object_type;    /* Statistic object type in range 0-BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES-1 */
    int meter_command_id;    /* Relevant only for egress */
    int meter_qos_map_id;    /* Relevant only for egress */
    uint8 is_meter_enable;    /* Allow metering */
    uint8 is_fp_cs_var;    /* Enable PMF2 context selection */
} bcm_compat6518_stat_pp_profile_info_t;

/* Map {statistic command, statistic profile} to the profile properties */
extern int bcm_compat6518_stat_pp_profile_create(
    int unit,
    int flags,
    bcm_stat_counter_interface_type_t engine_source,
    int *stat_pp_profile,
    bcm_compat6518_stat_pp_profile_info_t *stat_pp_profile_info);

/* Get profile properties according to stat_pp_profile */
extern int bcm_compat6518_stat_pp_profile_get(
    int unit,
    int stat_pp_profile,
    bcm_compat6518_stat_pp_profile_info_t *stat_pp_profile_info);

#if defined(INCLUDE_L3)
/* VXLAN Gport  */
typedef struct bcm_compat6518_vxlan_port_s {
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
} bcm_compat6518_vxlan_port_t;

/* bcm_vxlan_port_add adds a Access or Network VXLAN port */
extern int bcm_compat6518_vxlan_port_add(
    int unit,
    bcm_vpn_t l2vpn,
    bcm_compat6518_vxlan_port_t *vxlan_port);

/* Get VXLAN port information. */
extern int bcm_compat6518_vxlan_port_get(
    int unit,
    bcm_vpn_t l2vpn,
    bcm_compat6518_vxlan_port_t *vxlan_port);

/* Get all VXLAN port information. */
extern int bcm_compat6518_vxlan_port_get_all(
    int unit,
    bcm_vpn_t l2vpn,
    int port_max,
    bcm_compat6518_vxlan_port_t *port_array,
    int *port_count);

#endif /* defined(INCLUDE_L3) */

#endif /* BCM_RPC_SUPPORT */
#endif /* !_COMPAT_6518_H */
