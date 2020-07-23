/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_TUNNEL_H__
#define __BCM_TUNNEL_H__

#if defined(INCLUDE_L3)

#include <bcm/types.h>
#include <bcm/qos.h>
#include <bcm/l3.h>

/* Tunnel flags. */
#define BCM_TUNNEL_TERM_DSCP_TRUST          0x00000001 /* Trust incoming tunnel
                                                          DSCP inner or outer
                                                          based on
                                                          USE_OUTER_DSCP flag. */
#define BCM_TUNNEL_TERM_USE_OUTER_DSCP      0x00000002 /* Use outer header DSCP. */
#define BCM_TUNNEL_TERM_USE_OUTER_TTL       0x00000004 /* Use outer header TTL. */
#define BCM_TUNNEL_TERM_KEEP_INNER_DSCP     0x00000008 /* Keep inner header
                                                          DSCP. */
#define BCM_TUNNEL_TERM_WLAN_REMOTE_TERMINATE 0x00000010 /* Remote WLAN tunnel
                                                          termination */
#define BCM_TUNNEL_TERM_WLAN_SET_ROAM       0x00000020 /* Set wlan ha/fa roaming
                                                          model */
#define BCM_TUNNEL_TERM_TUNNEL_WITH_ID      0x00000040 /* Create tunnel with ID */
#define BCM_TUNNEL_TERM_RPF_FAIL_DROP       0x00000100 /* Drop packet on RPF
                                                          failure. */
#define BCM_TUNNEL_TERM_L3_IIF_MISMATCH_DROP 0x00000200 /* Drop packet if
                                                          expected l3_iif
                                                          doesn't match incoming
                                                          l3_iif. */
#define BCM_TUNNEL_TERM_L3_IIF_MISMATCH_TO_CPU 0x00000400 /* Copy to cpu if
                                                          expected l3_iif
                                                          doesn't match incoming
                                                          l3_iif. */
#define BCM_TUNNEL_TERM_MICRO_SEGMENT_ID    0x00000800 /* Indicate that tunnel
                                                          is an SRv6 micro SID
                                                          tunnel. */
#define BCM_TUNNEL_INIT_USE_INNER_DF        0x00000080 /* Copy DF from inner
                                                          header. Note:flag
                                                          takes precedence over
                                                          ipv4_set_df flag. */
#define BCM_TUNNEL_INIT_IPV4_SET_DF         0x00000100 /* Set DF bit in outer
                                                          header if inner header
                                                          is IPv4. */
#define BCM_TUNNEL_INIT_IPV6_SET_DF         0x00000200 /* Set DF bit in outer
                                                          header if inner header
                                                          is IPv6. */
#define BCM_TUNNEL_INIT_IP4_ID_SET_FIXED    0x00000400 /* Use fixed starting ID
                                                          for IPv4 tunnel */
#define BCM_TUNNEL_INIT_IP4_ID_SET_RANDOM   0x00000800 /* Use random starting ID
                                                          for IPv4 tunnel */
#define BCM_TUNNEL_INIT_WLAN_MTU            0x00001000 /* Set MTU for outgoing
                                                          WLAN tunnel */
#define BCM_TUNNEL_INIT_WLAN_FRAG_ENABLE    0x00002000 /* Enable Fragmentation
                                                          for WLAN Tunnel */
#define BCM_TUNNEL_INIT_WLAN_VLAN_TAGGED    0x00004000 /* Set outgoing WLAN
                                                          tunnel vlan */
#define BCM_TUNNEL_INIT_WLAN_TUNNEL_WITH_ID 0x00008000 /* Create outgoing tunnel
                                                          with ID */
#define BCM_TUNNEL_REPLACE                  0x00010000 /* Update existing
                                                          tunnel. */
#define BCM_TUNNEL_TERM_ETHERNET            0x00020000 /*  Terminate Ethernet
                                                          header after
                                                          terminated IP. */
#define BCM_TUNNEL_TERM_UDP_CHECKSUM_ENABLE 0x00040000 /* Drop pkts with
                                                          non-zero UDP checksum. */
#define BCM_TUNNEL_WITH_ID                  0x00080000 /* Add using the
                                                          specified ID. */
#define BCM_TUNNEL_TERM_TUNNEL_DEACTIVATE   0x00100000 /* Use to deactivate
                                                          tunnel terminator. */
#define BCM_TUNNEL_TERM_WIDE                0x00200000 /* Use to create tuunel
                                                          terminator wide mode. */
#define BCM_TUNNEL_TERM_USE_OUTER_PCP       0x00400000 /* Use outer header PCP. */
#define BCM_TUNNEL_TERM_USE_TUNNEL_CLASS    0x00800000 /* Use tunnel class. */
#define BCM_TUNNEL_INIT_GRE_KEY_USE_LB      0x01000000 /* GRE key presented and
                                                          include LB key, used
                                                          with bcmTunnelTypeGRE
                                                          tunnel types. */
#define BCM_TUNNEL_TERM_ECN_MAP             0x02000000 /* Use to set tunnel
                                                          termination ecn map
                                                          id. */
#define BCM_TUNNEL_INIT_USE_OUTER_TTL       BCM_TUNNEL_TERM_USE_OUTER_TTL /* Use outer header TTL. */
#define BCM_TUNNEL_INIT_WIDE                0x08000000 /* Use to create tunnel
                                                          encapsulation wide
                                                          mode. */
#define BCM_TUNNEL_TERM_BUD                 0x10000000 /* Use to create tunnel
                                                          termination for BUD
                                                          node. */
#define BCM_TUNNEL_INIT_ERSPAN_TYPE3        0x20000000 /* Use to create ERSPANv3
                                                          tunnel. */
#define BCM_TUNNEL_TERM_STAT_ENABLE         0x40000000 /* Enable statistics. */
#define BCM_TUNNEL_INIT_STAT_ENABLE         0x80000000 /* Enable statistics. */
#define BCM_TUNNEL_INIT_GRE_WITH_SN         BCM_TUNNEL_TERM_ETHERNET /*  GRE8 with Sequence
                                                          Number, used with
                                                          bcmTunnelTypeGreAnyIn4
                                                          tunnel type. */
#define BCM_TUNNEL_TERM_EM                  BCM_TUNNEL_INIT_USE_INNER_DF /* Tunnel termination on
                                                          EM. */
#define BCM_TUNNEL_INIT_SVTAG_ENABLE        BCM_TUNNEL_INIT_IP4_ID_SET_FIXED /* Enable the SVTAG
                                                          lookup for the tunnel. */
#define BCM_TUNNEL_TERM_USE_OUTER_QOS       BCM_TUNNEL_TERM_USE_OUTER_DSCP /* Use outer header QoS. */
#define BCM_TUNNEL_TERM_CROSS_CONNECT       BCM_TUNNEL_INIT_WIDE /* Create tunnel
                                                          termination P2P L2/L3
                                                          service. */
#define BCM_TUNNEL_TERM_EXTENDED_TERMINATION BCM_TUNNEL_INIT_GRE_KEY_USE_LB /* Used for extended
                                                          termination flow */
#define BCM_TUNNEL_INIT_FIXED_UDP_SRC_PORT  BCM_TUNNEL_TERM_DSCP_TRUST /* Use the source UDP
                                                          port in the tunnel
                                                          initiator. Or use the
                                                          hash entropy on the
                                                          private range [49152,
                                                          65535] by default. */
#define BCM_TUNNEL_INIT_EVPN_ENABLE         BCM_TUNNEL_TERM_KEEP_INNER_DSCP /* Enable EVPN on the
                                                          tunnel initiator. */
#define BCM_TUNNEL_INIT_ERSPAN_WITH_SN      BCM_TUNNEL_INIT_GRE_WITH_SN /* Enable sequence number
                                                          in the GRE header of a
                                                          ERSPAN tunnel. */

#define BCM_TUNNEL_INIT_SET_DF  \
    (BCM_TUNNEL_INIT_IPV6_SET_DF | \
     BCM_TUNNEL_INIT_IPV4_SET_DF) 

/* Tunnel types. */
typedef enum bcm_tunnel_type_e {
    bcmTunnelTypeNone = 0,              /* Disable IP tunneling on specific
                                           interface. */
    bcmTunnelTypeIp4In4 = 1,            /* RFC 2003/2893: IPv4-in-IPv4 tunnel. */
    bcmTunnelTypeIp6In4 = 2,            /* RFC 2003/2893: IPv6-in-IPv4 tunnel. */
    bcmTunnelTypeIpAnyIn4 = 3,          /* RFC 2003/2893: IPv6/4-in-IPv4 tunnel. */
    bcmTunnelTypeIp4In6 = 4,            /* RFC 2003/2893: IPv4-in-IPv6 tunnel. */
    bcmTunnelTypeIp6In6 = 5,            /* RFC 2003/2893: IPv6-in-IPv6 tunnel. */
    bcmTunnelTypeIpAnyIn6 = 6,          /* RFC 2003/2893: IPv6/4-in-IPv6 tunnel. */
    bcmTunnelTypeGre4In4 = 7,           /* RFC 1701/2784/2890: GRE IPv4-in-IPv4
                                           tunnel. */
    bcmTunnelTypeGre6In4 = 8,           /* RFC 1701/2784/2890: GRE IPv6-in-IPv4
                                           tunnel. */
    bcmTunnelTypeGreAnyIn4 = 9,         /* RFC 1701/2784/2890: GRE
                                           IPv6/4-in-IPv4 tunnel. */
    bcmTunnelTypeGre4In6 = 10,          /* RFC 1701/2784/2890: GRE IPv4-in-IPv6
                                           tunnel. */
    bcmTunnelTypeGre6In6 = 11,          /* RFC 1701/2784/2890: GRE IPv6-in-IPv6
                                           tunnel. */
    bcmTunnelTypeGreAnyIn6 = 12,        /* RFC 1701/2784/2890: GRE
                                           IPv6/4-in-IPv6 tunnel. */
    bcmTunnelTypeIsatap = 13,           /* RFC 4214: Intra-Site Automatic Tunnel
                                           Addressing Protocol. */
    bcmTunnelType6In4Uncheck = 14,      /* RFC 3056: Configured connection of
                                           IPv6 Domains via IPv4. */
    bcmTunnelType6In4 = 15,             /* RFC 3964: Checked 6to4 tunnel if
                                           supported by device otherwise
                                           unchecked 6to4 tunnel. */
    bcmTunnelTypeMpls = 16,             /* RFC 3031: MPLS tunneling. */
    bcmTunnelTypeUdp = 17,              /* UDP tunneling */
    bcmTunnelTypePimSmDr1 = 18,         /* Broadcom PIM SM Tunnel */
    bcmTunnelTypePimSmDr2 = 19,         /* Broadcom PIM SM Tunnel */
    bcmTunnelTypePim6SmDr1 = 20,        /* Broadcom PIM6 SM Tunnel */
    bcmTunnelTypePim6SmDr2 = 21,        /* Broadcom PIM6 SM Tunnel */
    bcmTunnelTypeWlanWtpToAc = 22,      /* IPv4 WLAN WTP to AC tunnel */
    bcmTunnelTypeWlanAcToAc = 23,       /* IPv4 WLAN AC to AC tunnel */
    bcmTunnelTypeWlanWtpToAc6 = 24,     /* IPv6 WLAN WTP to AC tunnel */
    bcmTunnelTypeWlanAcToAc6 = 25,      /* IPv6 WLAN AC to AC tunnel */
    bcmTunnelTypeAutoMulticast = 26,    /* IPV4
                                           draft-ietf-mboned-auto-multicast-09:
                                           AMT */
    bcmTunnelTypeAutoMulticast6 = 27,   /* IPV6
                                           draft-ietf-mboned-auto-multicast-09:
                                           AMT */
    bcmTunnelTypeEtherIp4In4 = 28,      /* EoIP - Fixed encapsulation (2B) */
    bcmTunnelTypeL2Gre = 29,            /* L2-GRE Tunnel */
    bcmTunnelTypeVxlan = 30,            /* VXLAN Tunnel */
    bcmTunnelTypeErspan = 31,           /* Erspan tunnel */
    bcmTunnelTypeRspan = 32,            /* Rspan tunnel */
    bcmTunnelTypeL2EncapExternalCpu = 33, /* External CPU L2 tunnel */
    bcmTunnelTypeL2Flex = 34,           /* Flex L2 tunnel, Flow API only */
    bcmTunnelTypeL3Flex = 35,           /* Flex L3 tunnel, Flow API only */
    bcmTunnelTypeFlex = 36,             /* Flex Generic tunnel, Flow API only */
    bcmTunnelTypeLawfulInterception = 37, /* Lawful Interception tunnel */
    bcmTunnelTypeVxlan6 = 38,           /* VXLAN Tunnel */
    bcmTunnelTypeL2SrcEncap = 39,       /* L2 Source Mac Encapsulation */
    bcmTunnelTypeRoeIn4 = 40,           /* ROE frame Tunnel in IPv4 Header */
    bcmTunnelTypeRoeIn6 = 41,           /* ROE Frame Tunnel in IPv6 Header */
    bcmTunnelTypeUdp6 = 42,             /* Udp Tunnel */
    bcmTunnelTypeVxlanGpe = 43,         /* VXLAN GPE Tunnel */
    bcmTunnelTypeVxlan6Gpe = 44,        /* VXLAN GPE Tunnel */
    bcmTunnelTypeSR6 = 45,              /* SRv6 Tunnel */
    bcmTunnelTypeRspanAdvanced = 46,    /* RSPAN Advanced Tunnel */
    bcmTunnelTypeEsp = 47,              /* ESP tunnel over IPv4 */
    bcmTunnelTypeEsp6 = 48,             /* ESP tunnel over IPv6 */
    bcmTunnelTypeEthSR6 = 49,           /* Ethernet over SRv6 tunnel */
    bcmTunnelTypeEthIn6 = 50,           /* Ethernet over Ipv6 */
    bcmTunnelTypeCount = 51             /* Must be last */
} bcm_tunnel_type_t;

#define BCM_TUNNEL_TYPE_STR \
    "None", \
    "Ip4In4", \
    "Ip6In4", \
    "IpAnyIn4", \
    "Ip4In6", \
    "Ip6In6", \
    "IpAnyIn6", \
    "Gre4In4", \
    "Gre6In4", \
    "GreAnyIn4", \
    "Gre4In6", \
    "Gre6In6", \
    "GreAnyIn6", \
    "Isatap", \
    "6In4Uncheck", \
    "6In4", \
    "Mpls", \
    "Udp", \
    "PimSmDr1", \
    "PimSmDr2", \
    "Pim6SmDr1", \
    "Pim6SmDr2", \
    "WlanWtpToAc", \
    "WlanAcToAc", \
    "WlanWtpToAc6", \
    "WlanAcToAc6", \
    "AutoMulticast", \
    "AutoMulticast6", \
    "EtherIp4In4", \
    "L2Gre", \
    "Vxlan", \
    "Erspan", \
    "Rspan", \
    "L2EncapExternalCpu", \
    "L2Flex", \
    "L3Flex", \
    "Flex", \
    "LawfulInterception", \
    "Vxlan6", \
    "L2SrcEncap", \
    "RoeIn4", \
    "RoeIn6", \
    "Udp6", \
    "VxlanGpe", \
    "Vxlan6Gpe", \
    "SR6", \
    "RspanAdvanced", \
    "Esp", \
    "Esp6", \
    "EthSR6", \
    "EthIn6" 

/* L3 tunneling terminator. */
typedef struct bcm_tunnel_terminator_s {
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
    bcm_vlan_t vlan;                    /* The VLAN ID or L3 ingress interface
                                           ID for IPMC lookup or WLAN tunnel */
    bcm_vlan_t fwd_vlan;                /* VLAN ID for forwarding terminated IP
                                           Tunnel */
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
    bcm_qos_ingress_model_t ingress_qos_model; /* ingress qos and ttl model */
    int priority;                       /* Entry priority. */
    int subport_pbmp_profile_id;        /* profile id for allowed subports on
                                           this tunnel. */
    bcm_if_t egress_if;                 /* Tunnel egress interface */
    bcm_vrf_t default_vrf;              /* default virtual router instance. */
} bcm_tunnel_terminator_t;

/* bcm_tunnel_dscp_ecn_select_e */
typedef enum bcm_tunnel_dscp_ecn_select_e {
    bcmTunnelDscpEcnAssign = 0, /* Set outer IP header DSCP and ECN to tunnel
                                   initiator DSCP and ECN value. */
    bcmTunnelDscpEcnMap = 1,    /* Use DSCP and ECN value from DSCP and ECN map. */
    bcmTunnelDscpEcnCount = 2   /* Unused always last. */
} bcm_tunnel_dscp_ecn_select_t;

/* bcm_tunnel_dscp_select_e */
typedef enum bcm_tunnel_dscp_select_e {
    bcmTunnelDscpAssign = 0,    /* Set outer IP header DSCP to tunnel initiator
                                   DSCP value. */
    bcmTunnelDscpPacket = 1,    /* Copy packet DSCP to outer header. */
    bcmTunnelDscpMap = 2,       /* Use DSCP value from DSCP map. */
    bcmTunnelDscpCount = 3      /* Unused always last. */
} bcm_tunnel_dscp_select_t;

/* bcm_tunnel_flow_label_select_e */
typedef enum bcm_tunnel_flow_label_select_e {
    bcmTunnelFlowLabelAssign = 0,   /* Set outer IPv6 header flow label to
                                       tunnel initiator flow label value. */
    bcmTunnelFlowLabelEntropy = 1,  /* Set outer IPv6 header flow label to
                                       entropy value. */
    bcmTunnelFlowLabelCount = 2     /* Unused always last. */
} bcm_tunnel_flow_label_select_t;

/* L3 tunneling initiator. */
typedef struct bcm_tunnel_initiator_s {
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
    uint8 ecn;                          /* Tunnel header ECN value. */
    bcm_tunnel_flow_label_select_t flow_label_sel; /* Tunnel header flow label select. */
    bcm_tunnel_dscp_ecn_select_t dscp_ecn_sel; /* Tunnel header DSCP and ECN select. */
    int dscp_ecn_map;                   /* DSCP and ECN map ID. */
    int stat_cmd;                       /* Counter ID associated with the the
                                           tunnel. */
    uint8 counter_command_id;           /* Counter interface associated with the
                                           tunnel. */
} bcm_tunnel_initiator_t;

/* L3 Tunnel Configuration Structure. */
typedef struct bcm_tunnel_config_s {
    int ip4_id;             /* Global ID. */
    uint32 ms_pimsm_hdr1;   /* Most significant 32 bits of PIMSM-DR1 header */
    uint32 ls_pimsm_hdr1;   /* Least significant 32 bits of PIMSM-DR1 header. */
    uint32 ms_pimsm_hdr2;   /* Most significant 32 bits of PIMSM-DR2 header. */
    uint32 ls_pimsm_hdr2;   /* Least significant 32 bits of PIMSM-DR2 header. */
} bcm_tunnel_config_t;

/* Tunnel DSCP Map Structure. */
typedef struct bcm_tunnel_dscp_map_s {
    int priority;       /* Internal priority. */
    bcm_color_t color;  /* Color. */
    int dscp;           /* DSCP value. */
} bcm_tunnel_dscp_map_t;

/* tunnel_terminator_config_key */
typedef struct bcm_tunnel_terminator_config_key_s {
    uint32 flags;           /*  Configuration flags. */
    bcm_ip_t dip;           /*  DIP for tunnel header match. */
    bcm_ip_t dip_mask;      /*  Destination IP mask. */
    bcm_ip6_t dip6;         /*  DIP(IPv6) for tunnel header match. */
    bcm_ip6_t dip6_mask;    /*  Destination IP mask (IPv6). */
    bcm_tunnel_type_t type; /*  Tunnel type. */
    uint16 udp_dst_port;    /* Destination UDP port */
    bcm_vrf_t vrf;          /* Virtual router instance. */
} bcm_tunnel_terminator_config_key_t;

/* tunnel_terminator_config_action */
typedef struct bcm_tunnel_terminator_config_action_s {
    int tunnel_class;   /* Tunnel class id for VTEP match. */
} bcm_tunnel_terminator_config_action_t;

/* tunnel_terminator_config_traverse_info */
typedef struct bcm_tunnel_terminator_config_traverse_info_s {
    uint32 flags;   /* Flags for tunnel terminator config. */
} bcm_tunnel_terminator_config_traverse_info_t;

/* bcm_tunnel_initiator_traverse_cb */
typedef int (*bcm_tunnel_initiator_traverse_cb)(
    int unit, 
    bcm_tunnel_initiator_t *info, 
    void *user_data);

/* bcm_tunnel_terminator_traverse_cb */
typedef int (*bcm_tunnel_terminator_traverse_cb)(
    int unit, 
    bcm_tunnel_terminator_t *info, 
    void *user_data);

/* bcm_tunnel_terminator_config_traverse_cb */
typedef int (*bcm_tunnel_terminator_config_traverse_cb)(
    int unit, 
    bcm_tunnel_terminator_config_key_t *info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set the tunneling initiator parameters on an L3 interface. */
extern int bcm_tunnel_initiator_set(
    int unit, 
    bcm_l3_intf_t *intf, 
    bcm_tunnel_initiator_t *tunnel);

/* Set the tunneling initiator parameters on an L3 interface. */
extern int bcm_tunnel_initiator_create(
    int unit, 
    bcm_l3_intf_t *intf, 
    bcm_tunnel_initiator_t *tunnel);

/* Delete the tunnel association for the given L3 interface. */
extern int bcm_tunnel_initiator_clear(
    int unit, 
    bcm_l3_intf_t *intf);

/* Get the tunnel property for the given L3 interface. */
extern int bcm_tunnel_initiator_get(
    int unit, 
    bcm_l3_intf_t *intf, 
    bcm_tunnel_initiator_t *tunnel);

/* Traverse Tunnel Initiator Info */
extern int bcm_tunnel_initiator_traverse(
    int unit, 
    bcm_tunnel_initiator_traverse_cb cb, 
    void *user_data);

/* Delete specific IP tunnel initiator. */
extern int bcm_tunnel_initiator_destroy(
    int unit, 
    bcm_gport_t tunnel_id);

/* Add a tunnel terminator for DIP-SIP key. */
extern int bcm_tunnel_terminator_add(
    int unit, 
    bcm_tunnel_terminator_t *info);

/* Add a tunnel terminator for DIP-SIP key. */
extern int bcm_tunnel_terminator_create(
    int unit, 
    bcm_tunnel_terminator_t *info);

/* Delete a tunnel terminator for DIP-SIP key. */
extern int bcm_tunnel_terminator_delete(
    int unit, 
    bcm_tunnel_terminator_t *info);

/* Update a tunnel terminator for DIP-SIP key. */
extern int bcm_tunnel_terminator_update(
    int unit, 
    bcm_tunnel_terminator_t *info);

/* Get a tunnel terminator for DIP-SIP key. */
extern int bcm_tunnel_terminator_get(
    int unit, 
    bcm_tunnel_terminator_t *info);

/* Traverse Tunnel Terminator Info */
extern int bcm_tunnel_terminator_traverse(
    int unit, 
    bcm_tunnel_terminator_traverse_cb cb, 
    void *user_data);

/* Traverse Tunnel Terminator Info */
extern int bcm_tunnel_terminator_config_traverse(
    int unit, 
    bcm_tunnel_terminator_config_traverse_info_t additional_info, 
    bcm_tunnel_terminator_config_traverse_cb cb, 
    void *user_data);

/* Set the valid payload VLANs for this WLAN tunnel. */
extern int bcm_tunnel_terminator_vlan_set(
    int unit, 
    bcm_gport_t tunnel, 
    bcm_vlan_vector_t vlan_vec);

/* Get the valid payload VLANs for this WLAN tunnel. */
extern int bcm_tunnel_terminator_vlan_get(
    int unit, 
    bcm_gport_t tunnel, 
    bcm_vlan_vector_t *vlan_vec);

/* Attach counters entries to the given tunnel. */
extern int bcm_tunnel_terminator_stat_attach(
    int unit, 
    bcm_tunnel_terminator_t *terminator, 
    uint32 stat_counter_id);

/* Detach counters entries to the given tunnel. */
extern int bcm_tunnel_terminator_stat_detach(
    int unit, 
    bcm_tunnel_terminator_t *terminator);

/* Get stat counter id associated with given tunnel. */
extern int bcm_tunnel_terminator_stat_id_get(
    int unit, 
    bcm_tunnel_terminator_t *terminator, 
    uint32 *stat_counter_id);

/* Set flex counter object value for the given tunnel. */
extern int bcm_tunnel_terminator_flexctr_object_set(
    int unit, 
    bcm_tunnel_terminator_t *terminator, 
    uint32 value);

/* Get flex counter object value for the given tunnel. */
extern int bcm_tunnel_terminator_flexctr_object_get(
    int unit, 
    bcm_tunnel_terminator_t *terminator, 
    uint32 *value);

/* Create a tunnel DSCP map instance. */
extern int bcm_tunnel_dscp_map_create(
    int unit, 
    uint32 flags, 
    int *dscp_map_id);

/* Destroy a tunnel DSCP map instance. */
extern int bcm_tunnel_dscp_map_destroy(
    int unit, 
    int dscp_map_id);

/* 
 * Get the { internal priority, color }-to-DSCP mapping in the specified
 * DSCP map.
 */
extern int bcm_tunnel_dscp_map_get(
    int unit, 
    int dscp_map_id, 
    bcm_tunnel_dscp_map_t *dscp_map);

/* 
 * Set the { internal priority, color }-to-DSCP mapping in the specified
 * DSCP map.
 */
extern int bcm_tunnel_dscp_map_set(
    int unit, 
    int dscp_map_id, 
    bcm_tunnel_dscp_map_t *dscp_map);

/* Set port egress { internal priority, color }-to-DSCP mapping. */
extern int bcm_tunnel_dscp_map_port_set(
    int unit, 
    bcm_port_t port, 
    bcm_tunnel_dscp_map_t *dscp_map);

/* Get port egress { internal priority, color }-to-DSCP mapping. */
extern int bcm_tunnel_dscp_map_port_get(
    int unit, 
    bcm_port_t port, 
    bcm_tunnel_dscp_map_t *dscp_map);

/* Set or get the global chip tunneling configuration. */
extern int bcm_tunnel_config_get(
    int unit, 
    bcm_tunnel_config_t *tconfig);

/* Set or get the global chip tunneling configuration. */
extern int bcm_tunnel_config_set(
    int unit, 
    bcm_tunnel_config_t *tconfig);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a bcm_tunnel_initiator_t. */
extern void bcm_tunnel_initiator_t_init(
    bcm_tunnel_initiator_t *tunnel_init);

/* Initialize a bcm_tunnel_terminator_t structure. */
extern void bcm_tunnel_terminator_t_init(
    bcm_tunnel_terminator_t *tunnel_term);

/* Initialize a bcm_tunnel_config_t structure. */
extern void bcm_tunnel_config_t_init(
    bcm_tunnel_config_t *tconfig);

/* Initialize a L3 tunnel bcm_tunnel_dscp_map_t structure. */
extern void bcm_tunnel_dscp_map_t_init(
    bcm_tunnel_dscp_map_t *dscp_info);

/* 
 * Initialize a tunnel L3 tunnel bcm_tunnel_terminator_config_key_t
 * structure.
 */
extern void bcm_tunnel_terminator_config_key_t_init(
    bcm_tunnel_terminator_config_key_t *config_key);

/* 
 * Initialize a tunnel L3 tunnel bcm_tunnel_terminator_config_action_t
 * structure.
 */
extern void bcm_tunnel_terminator_config_action_t_init(
    bcm_tunnel_terminator_config_action_t *config_action);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add tunnel terminator config entries. */
extern int bcm_tunnel_terminator_config_add(
    int unit, 
    uint32 flags, 
    bcm_tunnel_terminator_config_key_t *config_key, 
    bcm_tunnel_terminator_config_action_t *config_action);

/* Get tunnel terminator config entries. */
extern int bcm_tunnel_terminator_config_get(
    int unit, 
    bcm_tunnel_terminator_config_key_t *config_key, 
    bcm_tunnel_terminator_config_action_t *config_action);

/* Delete tunnel terminator config entries. */
extern int bcm_tunnel_terminator_config_delete(
    int unit, 
    bcm_tunnel_terminator_config_key_t *config_key);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * This structure aggregates all elements that we need to configure l3
 * tunnel extention capability
 */
typedef struct bcm_l3_tunnel_term_extn_s {
    uint8 extn_enable;              /* This field specifes if l3 tunnel
                                       termination extension is to be enabled or
                                       is enabled */
    uint8 sip_mask_enable;          /* This field specifes if l3 tunnel
                                       termination extension sip mask is to be
                                       enabled or is enabled */
    bcm_tunnel_type_t tnl_type1;    /* This field represents the l3 tunnel type
                                       to match */
    bcm_tunnel_type_t tnl_type2;    /* This field represents the l3 tunnel type
                                       to match, alternatively */
    uint16 dip_mask;                /* This field specifes l3 tunnel termination
                                       extension - dip mask to apply to ingress
                                       tunneled packet before matching */
} bcm_l3_tunnel_term_extn_t;

/* initialize a bcm_l3_tunnel_term_extn_t structure. */
extern void bcm_l3_tunnel_term_extn_t_init(
    bcm_l3_tunnel_term_extn_t *extn_attribute);

#ifndef BCM_HIDE_DISPATCHABLE

/* return the configured l3 tunnel termination global attributes. */
extern int bcm_l3_tunnel_term_global_cfg_get(
    int unit, 
    bcm_l3_tunnel_term_extn_t *extn_attribute);

/* configure l3 tunnel termination global attributes. */
extern int bcm_l3_tunnel_term_global_cfg_set(
    int unit, 
    bcm_l3_tunnel_term_extn_t *extn_attribute);

#endif /* defined(INCLUDE_L3) */

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_TUNNEL_H__ */
