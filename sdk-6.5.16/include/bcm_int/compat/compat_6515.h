/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-6.5.15 routines
*/

#ifndef _COMPAT_6515_H_
#define _COMPAT_6515_H_

#ifdef BCM_RPC_SUPPORT
#include <bcm/types.h>
#include <bcm/mpls.h>
#include <bcm/l3.h>
#include <bcm/ipmc.h>
#include <bcm/qos.h>
#include <bcm/tunnel.h>
#include <bcm/l2gre.h>
#include <bcm/vxlan.h>
#include <bcm/failover.h>

#ifdef INCLUDE_L3
typedef struct bcm_compat6515_mpls_tunnel_switch_s {
    uint32 flags;                       /* BCM_MPLS_SWITCH_xxx. */
    uint32 flags2;                      /* BCM_MPLS_SWITCH2_xxx. */
    bcm_mpls_label_t label;             /* Incoming label value. */
    bcm_mpls_label_t second_label;      /* Incoming second label. */
    bcm_gport_t port;                   /* Incoming port. */
    bcm_mpls_switch_action_t action;    /* MPLS label action. */
    bcm_mpls_switch_action_t action_if_bos; /* MPLS label action if BOS. */
    bcm_mpls_switch_action_t action_if_not_bos; /* MPLS label action if not BOS. */
    bcm_multicast_t mc_group;           /* P2MP Multicast group. */
    int exp_map;                        /* EXP-map ID. */
    int int_pri;                        /* Internal priority. */
    bcm_policer_t policer_id;           /* Policer ID to be associated with the
                                           incoming label. */
    bcm_vpn_t vpn;                      /* L3 VPN used if action is POP. */
    bcm_mpls_egress_label_t egress_label; /* Outgoing label information. */
    bcm_if_t egress_if;                 /* Outgoing egress object. */
    bcm_if_t ingress_if;                /* Ingress Interface object. */
    int mtu;                            /* MTU. */
    int qos_map_id;                     /* QOS map identifier. */
    bcm_failover_t failover_id;         /* Failover Object Identifier for
                                           protected tunnel. Used for 1+1
                                           protection also */
    bcm_gport_t tunnel_id;              /* Tunnel ID. */
    bcm_gport_t failover_tunnel_id;     /* Failover Tunnel ID. */
    bcm_if_t tunnel_if;                 /* hierarchical interface, relevant for
                                           when action is
                                           BCM_MPLS_SWITCH_ACTION_POP. */
    bcm_gport_t egress_port;            /* Outgoing egress port. */
    uint16 oam_global_context_id;       /* OAM global context id passed from
                                           ingress to egress XGS chip. */
    uint32 class_id;                    /* Class ID */
    int inlif_counting_profile;         /* In LIF counting profile */
    int ecn_map_id;                     /* ECN map identifier */
    int tunnel_term_ecn_map_id;         /* Tunnel termination ecn map identifier */
} bcm_compat6515_mpls_tunnel_switch_t;

extern int bcm_compat6515_mpls_tunnel_switch_add(
    int unit, bcm_compat6515_mpls_tunnel_switch_t *info);

extern int bcm_compat6515_mpls_tunnel_switch_delete(
    int unit, bcm_compat6515_mpls_tunnel_switch_t *info);

extern int bcm_compat6515_mpls_tunnel_switch_get(
    int unit, bcm_compat6515_mpls_tunnel_switch_t *info);

/* IPMC address type. */
typedef struct bcm_compat6515_ipmc_addr_s {
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
                                           for XGS chips based on flags value.
                                           For SBX chips it is the Multicast
                                           Group index */
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
} bcm_compat6515_ipmc_addr_t;

/* Add new IPMC group. */
extern int bcm_compat6515_ipmc_add(
    int unit,
    bcm_compat6515_ipmc_addr_t *data);

/* Find info of an IPMC group. */
extern int bcm_compat6515_ipmc_find(
    int unit,
    bcm_compat6515_ipmc_addr_t *data);

/* Remove IPMC group. */
extern int bcm_compat6515_ipmc_remove(
    int unit,
    bcm_compat6515_ipmc_addr_t *data);

#endif /* INCLUDE_L3 */
/* Port Resource Configuration */
typedef struct bcm_compat6515port_resource_s {
    uint32 flags;                   /* BCM_PORT_RESOURCE_XXX */
    bcm_gport_t port;               /* Local logical port number to connect to
                                       the given physical port */
    int physical_port;              /* Local physical port, -1 if the logical to
                                       physical mapping is to be deleted */
    int speed;                      /* Initial speed after FlexPort operation */
    int lanes;                      /* Number of PHY lanes for this physical
                                       port */
    bcm_port_encap_t encap;         /* Encapsulation mode for port */
    bcm_port_phy_fec_t fec_type;    /* fec_type for port */
    int phy_lane_config;            /* serdes pmd lane config for port */
    int link_training;              /* link training on or off */
    int roe_compression;            /* roe_compression enable */
} bcm_compat6515_port_resource_t;


/* Generic port match attribute structure */
typedef struct bcm_compat6515_port_match_info_s {
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
    bcm_vlan_action_set_t *action;      /* Match action */
    uint16 extended_port_vid;           /* Extender port VID */
    bcm_vpn_t vpn;                      /* VPN ID */
    uint16 niv_port_vif;                /* NIV port VIF */
    uint32 isid;
} bcm_compat6515_port_match_info_t;

/* Add a match to an existing port */
extern int bcm_compat6515_port_match_add(
    int unit,
    bcm_gport_t port,
    bcm_compat6515_port_match_info_t *match);

/* Remove a match from an existing port */
extern int bcm_compat6515_port_match_delete(
    int unit,
    bcm_gport_t port,
    bcm_compat6515_port_match_info_t *match);

/* Replace an old match with a new one for an existing port */
extern int bcm_compat6515_port_match_replace(
    int unit,
    bcm_gport_t port,
    bcm_compat6515_port_match_info_t *old_match,
    bcm_compat6515_port_match_info_t *new_match);

/* Get all the matches for an existing port */
extern int bcm_compat6515_port_match_multi_get(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_compat6515_port_match_info_t *match_array,
    int *count);

/* Assign a set of matches to an existing port */
extern int bcm_compat6515_port_match_set(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_compat6515_port_match_info_t *match_array);

#ifdef INCLUDE_L3
typedef struct bcm_compat6515_l3_egress_s {
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
} bcm_compat6515_l3_egress_t;

extern int bcm_compat6515_l3_egress_find(
    int unit,
    bcm_compat6515_l3_egress_t *egr,
    bcm_if_t *intf);

extern int bcm_compat6515_l3_egress_create(
    int unit,
    uint32 flags,
    bcm_compat6515_l3_egress_t *egr,
    bcm_if_t *if_id);

extern int bcm_compat6515_l3_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6515_l3_egress_t *egr);

extern int bcm_compat6515_failover_egress_set(
    int unit,
    bcm_if_t intf,
    bcm_compat6515_l3_egress_t *failover_egr);

extern int bcm_compat6515_failover_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6515_l3_egress_t *failover_egr);

/* Ingress QoS model structure */
typedef struct bcm_compat6515_qos_ingress_model_s {
    bcm_qos_ingress_model_type_t ingress_phb; /* ingress PHB model */
    bcm_qos_ingress_model_type_t ingress_remark; /* ingress remark model */
    bcm_qos_ingress_model_type_t ingress_ttl; /* ingress ttl model */
} bcm_compat6515_qos_ingress_model_t;

/* L3 tunneling terminator. */
typedef struct bcm_compat6515_tunnel_terminator_s {
    uint32 flags;                       /* Configuration flags. */
    uint32 multicast_flag;              /* VXLAN Multicast trigger flags. */
    bcm_vrf_t vrf;                      /* Virtual router instance. */
    bcm_ip_t sip;                       /* SIP for tunnel header match. */
    bcm_ip_t dip;                       /* DIP for tunnel header match. */
    bcm_ip_t sip_mask;                  /* Source IP mask. */
    bcm_ip_t dip_mask;                  /* Destination IP mask. */
    bcm_ip6_t sip6;                     /* SIP for tunnel header match (IPv6). */
    bcm_ip6_t dip6;                     /* DIP for tunnel header match (IPv6). */
    bcm_ip6_t sip6_mask;                /* Source IP mask (IPv6). */
    bcm_ip6_t dip6_mask;                /* Destination IP mask (IPv6). */
    uint32 udp_dst_port;                /* UDP dst port for UDP packets. */
    uint32 udp_src_port;                /* UDP src port for UDP packets */
    bcm_tunnel_type_t type;             /* Tunnel type */
    bcm_pbmp_t pbmp;                    /* Port bitmap for this tunnel */
    bcm_vlan_t vlan;                    /* The VLAN ID for IPMC lookup or WLAN
                                           tunnel */
    bcm_gport_t remote_port;            /* Remote termination */
    bcm_gport_t tunnel_id;              /* Tunnel id */
    int if_class;                       /* L3 interface class this tunnel. */
    bcm_multicast_t failover_mc_group;  /* MC group used for bi-cast. */
    bcm_failover_t ingress_failover_id; /* 1+1 protection. */
    bcm_failover_t failover_id;         /* Failover Object Identifier for
                                           protected tunnel. */
    bcm_gport_t failover_tunnel_id;     /* Failover Tunnel ID. */
    bcm_if_t tunnel_if;                 /* hierarchical interface. */
    int tunnel_class;                   /* Tunnel class id for VFP match. */
    int qos_map_id;                     /* QoS DSCP map this tunnel. */
    int inlif_counting_profile;         /* In LIF counting profile */
    int tunnel_term_ecn_map_id;         /* Tunnel termination ecn map id. */
    bcm_compat6515_qos_ingress_model_t ingress_qos_model; /* ingress qos and ttl model */
} bcm_compat6515_tunnel_terminator_t;

extern int bcm_compat6515_l2gre_tunnel_terminator_get(
    int unit,
    bcm_compat6515_tunnel_terminator_t *info);

extern int bcm_compat6515_l2gre_tunnel_terminator_create(
    int unit,
    bcm_compat6515_tunnel_terminator_t *info);

extern int bcm_compat6515_l2gre_tunnel_terminator_update(
    int unit,
    bcm_compat6515_tunnel_terminator_t *info);

extern int bcm_compat6515_tunnel_terminator_add(
    int unit,
    bcm_compat6515_tunnel_terminator_t *info);

extern int bcm_compat6515_tunnel_terminator_create(
    int unit,
    bcm_compat6515_tunnel_terminator_t *info);

extern int bcm_compat6515_tunnel_terminator_delete(
    int unit,
    bcm_compat6515_tunnel_terminator_t *info);

extern int bcm_compat6515_tunnel_terminator_update(
    int unit,
    bcm_compat6515_tunnel_terminator_t *info);

extern int bcm_compat6515_tunnel_terminator_get(
    int unit,
    bcm_compat6515_tunnel_terminator_t *info);

extern int bcm_compat6515_vxlan_tunnel_terminator_get(
    int unit,
    bcm_compat6515_tunnel_terminator_t *info);

extern int bcm_compat6515_vxlan_tunnel_terminator_create(
    int unit,
    bcm_compat6515_tunnel_terminator_t *info);

extern int bcm_compat6515_vxlan_tunnel_terminator_update(
    int unit,
    bcm_compat6515_tunnel_terminator_t *info);

#endif /* INCLUDE_L3 */
#endif /* BCM_RPC_SUPPORT */
#endif /* !_COMPAT_6515_H */
