/*
 * $Id: compat_6514.h,v 2.0 2018/04/09
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-6.5.14 routines
 */

#ifndef _COMPAT_6514_H_
#define _COMPAT_6514_H_

#ifdef BCM_RPC_SUPPORT
#include <bcm_int/compat/compat_6518.h>
#include <bcm/types.h>
#include <bcm/field.h>
#include <bcm/cosq.h>
#include <bcm/mpls.h>
#include <bcm/l3.h>
#include <bcm/ipmc.h>
#include <bcm_int/compat/compat_6516.h>

/* Device-independent L2 address structure. */
typedef struct bcm_compat6514_l2_addr_s {
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
} bcm_compat6514_l2_addr_t;

extern int bcm_compat6514_l2_addr_add(int unit,
                                      bcm_compat6514_l2_addr_t *l2addr);

extern int bcm_compat6514_l2_addr_multi_add(int unit,
                                            bcm_compat6514_l2_addr_t *l2addr, int count);

extern int bcm_compat6514_l2_addr_multi_delete(int unit,
                                               bcm_compat6514_l2_addr_t *l2addr, int count);

extern int bcm_compat6514_l2_addr_get(int unit, bcm_mac_t mac_addr, bcm_vlan_t vid,
                                      bcm_compat6514_l2_addr_t *l2addr);

extern int bcm_compat6514_l2_conflict_get(int unit, bcm_compat6514_l2_addr_t *addr,
                                          bcm_compat6514_l2_addr_t *cf_array, int cf_max,
                                          int *cf_count);

extern int bcm_compat6514_l2_replace(int unit, uint32 flags,
                                     bcm_compat6514_l2_addr_t *match_addr,
                                     bcm_module_t new_module,
                                     bcm_port_t new_port, bcm_trunk_t new_trunk);

extern int bcm_compat6514_l2_replace_match(int unit, uint32 flags,
                                           bcm_compat6514_l2_addr_t *match_addr,
                                           bcm_compat6514_l2_addr_t *mask_addr,
                                           bcm_compat6514_l2_addr_t *replace_addr,
                                           bcm_compat6514_l2_addr_t *replace_mask_addr);

extern int bcm_compat6514_l2_stat_get(int unit,
                                      bcm_compat6514_l2_addr_t *l2addr,
                                      bcm_l2_stat_t stat, uint64 *val);

extern int bcm_compat6514_l2_stat_get32(int unit,
                                        bcm_compat6514_l2_addr_t *l2addr,
                                        bcm_l2_stat_t stat, uint32 *val);

extern int bcm_compat6514_l2_stat_set(int unit,
                                      bcm_compat6514_l2_addr_t *l2addr,
                                      bcm_l2_stat_t stat, uint64 val);

extern int bcm_compat6514_l2_stat_set32(int unit,
                                        bcm_compat6514_l2_addr_t *l2addr,
                                        bcm_l2_stat_t stat, uint32 val);

extern int bcm_compat6514_l2_stat_enable_set(int unit,
                                             bcm_compat6514_l2_addr_t *l2addr,
                                             int enable);
/* This structure contains the configuration of a VLAN. */
typedef struct bcm_compat6514_vlan_control_vlan_s {
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
} bcm_compat6514_vlan_control_vlan_t;

/*Set or retrieve current vlan properties selectively. */
extern int bcm_compat6514_vlan_control_vlan_selective_get(
    int unit,
    bcm_vlan_t vlan,
    uint32 valid_fields,
    bcm_compat6514_vlan_control_vlan_t *control);

/*Set or retrieve current vlan properties selectively. */
extern int bcm_compat6514_vlan_control_vlan_selective_set(
    int unit,
    bcm_vlan_t vlan,
    uint32 valid_fields,
    bcm_compat6514_vlan_control_vlan_t *control);

/*Set or retrieve current vlan properties. */
extern int bcm_compat6514_vlan_control_vlan_get(
    int unit,
    bcm_vlan_t vlan,
    bcm_compat6514_vlan_control_vlan_t *control);

/*Set or retrieve current vlan properties. */
extern int bcm_compat6514_vlan_control_vlan_set(
    int unit,
    bcm_vlan_t vlan,
    bcm_compat6514_vlan_control_vlan_t control);

/* Generic port match attribute structure */
typedef struct bcm_compat6514_port_match_info_s {
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
    bcm_compat6516_vlan_action_set_t *action;      /* Match action */
    uint16 extended_port_vid;           /* Extender port VID */
    bcm_vpn_t vpn;                      /* VPN ID */
    uint16 niv_port_vif;                /* NIV port VIF */
    uint32 isid;
} bcm_compat6514_port_match_info_t;

/* Add a match to an existing port */
extern int bcm_compat6514_port_match_add(
    int unit,
    bcm_gport_t port,
    bcm_compat6514_port_match_info_t *match);

/* Remove a match from an existing port */
extern int bcm_compat6514_port_match_delete(
    int unit,
    bcm_gport_t port,
    bcm_compat6514_port_match_info_t *match);

/* Replace an old match with a new one for an existing port */
extern int bcm_compat6514_port_match_replace(
    int unit,
    bcm_gport_t port,
    bcm_compat6514_port_match_info_t *old_match,
    bcm_compat6514_port_match_info_t *new_match);

/* Get all the matches for an existing port */
extern int bcm_compat6514_port_match_multi_get(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_compat6514_port_match_info_t *match_array,
    int *count);

/* Assign a set of matches to an existing port */
extern int bcm_compat6514_port_match_set(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_compat6514_port_match_info_t *match_array);

#ifdef INCLUDE_L3
/* IPMC address type. */
typedef struct bcm_compat6514_ipmc_addr_s {
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
} bcm_compat6514_ipmc_addr_t;

/* Add new IPMC group. */
extern int bcm_compat6514_ipmc_add(
    int unit,
    bcm_compat6514_ipmc_addr_t *data);

/* Find info of an IPMC group. */
extern int bcm_compat6514_ipmc_find(
    int unit,
    bcm_compat6514_ipmc_addr_t *data);

/* Remove IPMC group. */
extern int bcm_compat6514_ipmc_remove(
    int unit,
    bcm_compat6514_ipmc_addr_t *data);

/*
 * L3 Egress Structure.
 *
 * Description of an L3 forwarding destination.
 */
typedef struct bcm_compat6514_l3_egress_s {
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
} bcm_compat6514_l3_egress_t;

extern int bcm_compat6514_l3_egress_create(
    int unit,
    uint32 flags,
    bcm_compat6514_l3_egress_t *egr,
    bcm_if_t *if_id);

extern int bcm_compat6514_l3_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6514_l3_egress_t *egr);

extern int bcm_compat6514_l3_egress_find(
    int unit,
    bcm_compat6514_l3_egress_t *egr,
    bcm_if_t *intf);

extern int bcm_compat6514_failover_egress_set(
    int unit,
    bcm_if_t intf,
    bcm_compat6514_l3_egress_t *failover_egr);

extern int bcm_compat6514_failover_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6514_l3_egress_t *failover_egr);

typedef struct bcm_compat6514_mpls_egress_label_s {
    uint32 flags;                       /* BCM_MPLS_EGRESS_LABEL_xxx. */
    bcm_mpls_label_t label;
    int qos_map_id;                     /* EXP map ID. */
    uint8 exp;
    uint8 ttl;
    uint8 pkt_pri;
    uint8 pkt_cfi;
    bcm_if_t tunnel_id;                 /* Tunnel Interface. */
    bcm_if_t l3_intf_id;                /* l3 Interface ID. */
    bcm_mpls_egress_action_t action;    /* MPLS label action, relevant when
                                           BCM_MPLS_EGRESS_LABEL_ACTION_VALID is
                                           set. */
    int ecn_map_id;                     /* Ecn map id for IP ECN to EXP mapping. */
    int int_cn_map_id;                  /* Ecn map id for INT_CN to EXP mapping. */
    bcm_failover_t egress_failover_id;  /* Failover object index for Egress
                                           Protection. */
    bcm_if_t egress_failover_if_id;     /* Failover MPLS Tunnel identifier for
                                           Egress Protection. */
    int outlif_counting_profile;        /* Out LIF counting profile */
    bcm_mpls_special_label_push_type_t spl_label_push_type; /* Special label push type */
    bcm_encap_access_t encap_access;    /* Encapsulation Access stage */
} bcm_compat6514_mpls_egress_label_t;

typedef struct bcm_compat6514_mpls_tunnel_switch_s {
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
    bcm_compat6514_mpls_egress_label_t egress_label; /* Outgoing label information. */
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
} bcm_compat6514_mpls_tunnel_switch_t;

extern int bcm_compat6514_mpls_tunnel_switch_add(
    int unit, bcm_compat6514_mpls_tunnel_switch_t *info);

extern int bcm_compat6514_mpls_tunnel_switch_delete(
    int unit, bcm_compat6514_mpls_tunnel_switch_t *info);

extern int bcm_compat6514_mpls_tunnel_switch_get(
    int unit, bcm_compat6514_mpls_tunnel_switch_t *info);
#endif

#endif
#endif /* !_COMPAT_6514_H */
