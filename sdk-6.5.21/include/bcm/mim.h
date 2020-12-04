/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_MIM_H__
#define __BCM_MIM_H__

#if defined(INCLUDE_L3)

#include <bcm/types.h>
#include <bcm/multicast.h>
#include <bcm/l3.h>
#include <bcm/switch.h>

/* MAC-in-MAC VPN Flags. */
#define BCM_MIM_VPN_MIM                     0x00000001 /* ELAN VPN service */
#define BCM_MIM_VPN_WITH_ID                 0x00000002 
#define BCM_MIM_VPN_REPLACE                 0x00000004 
#define BCM_MIM_VPN_MATCH_SERVICE_VLAN_TPID 0x00000008 
#define BCM_MIM_VPN_PBB_TE                  0x00000010 /* P2P service for
                                                          Provider Backbone
                                                          Transport */
#define BCM_MIM_VPN_BVLAN                   0x00000020 /* Creates Backbone VLAN */
#define BCM_MIM_VPN_SERVICE_ENCAP_WITH_ID   0x00000040 /* Used when the
                                                          application provides
                                                          service_encap_id */
#define BCM_MIM_VPN_QOS_SDTAG_NO_ACTION     0x00000080 /* SDTAG priority control
                                                          per VPN */
#define BCM_MIM_VPN_QOS_SDTAG_FIX_PRIORITY  0x00000100 /* SDTAG priority control
                                                          per VPN, fixed
                                                          priority */
#define BCM_MIM_VPN_QOS_SDTAG_MAP_INT_PRIORITY 0x00000200 /* SDTAG priority control
                                                          per VPN, map priority */
#define BCM_MIM_VPN_QOS_ITAG_FIX_PRIORITY   0x00000400 /* ITAG priority control
                                                          per VPN, fixed
                                                          priority */
#define BCM_MIM_VPN_QOS_ITAG_MAP_SDTAG_PRIORITY 0x00000800 /* ITAG priority control
                                                          per VPN, map priority */
#define BCM_MIM_VPN_QOS_INGRESS_MAP_FIX_INT_PRIORITY 0x00001000 /* Fixed internal
                                                          priority based on VFI */
#define BCM_MIM_VPN_QOS_INGRESS_MAP_INT_PRIORITY 0x00002000 /* Map internal priority
                                                          based on VFI */
#define BCM_MIM_VPN_BVID_DROP_NOMATCH       0x00004000 /* Drop pkt if BVID check
                                                          fails */
#define BCM_MIM_VPN_ELINE                   0x00008000 /* Point-to-Point VPN
                                                          service */
#define BCM_MIM_VPN_SERVICE_ENCAP_INGRESS_WIDE 0x00010000 /* create 
                                                          service_encap_id wide
                                                          mode */
#define BCM_MIM_VPN_EGRESS_SERVICE_TPID_REPLACE 0x00020000 /*  Replace TPID in
                                                          SD-TAG on MIM egress */
#define BCM_MIM_VPN_INGRESS_ONLY            0x00040000 /* Create the MiM VPN on
                                                          ingress pipe only
                                                          (InLIF, Spanning tree,
                                                          MC groups) */
#define BCM_MIM_VPN_EGRESS_ONLY             0x00080000 /* Create the MiM VPN on
                                                          egress pipe only (VSI
                                                          to ISID table) */
#define BCM_MIM_VPN_MATCH_SERVICE_SMAC      0x00100000 /* Match entry include
                                                          Source B-MAC address */

/* MAC-in-MAC Port Flags. */
#define BCM_MIM_PORT_WITH_ID                0x00000001 
#define BCM_MIM_PORT_ENCAP_WITH_ID          0x00000002 
#define BCM_MIM_PORT_REPLACE                0x00000004 
#define BCM_MIM_PORT_DROP                   0x00000008 
#define BCM_MIM_PORT_TYPE_ACCESS            0x00000010 
#define BCM_MIM_PORT_TYPE_BACKBONE          0x00000020 
#define BCM_MIM_PORT_TYPE_PEER              0x00000040 
#define BCM_MIM_PORT_MATCH_SERVICE_VLAN_TPID 0x00000080 
#define BCM_MIM_PORT_EGRESS_TUNNEL_SERVICE_REPLACE 0x00000100 
#define BCM_MIM_PORT_EGRESS_LABEL           0x00000200 
#define BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TAGGED 0x00000400 
#define BCM_MIM_PORT_EGRESS_SERVICE_VLAN_ADD 0x00000800 
#define BCM_MIM_PORT_EGRESS_SERVICE_VLAN_REPLACE 0x00001000 
#define BCM_MIM_PORT_EGRESS_SERVICE_VLAN_DELETE 0x00002000 
#define BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TPID_REPLACE 0x00004000 
#define BCM_MIM_PORT_EGRESS_SERVICE_VLAN_PRI_REPLACE 0x00008000 
#define BCM_MIM_PORT_EGRESS_SERVICE_VLAN_PRI_TPID_REPLACE 0x00010000 
#define BCM_MIM_PORT_EGRESS_TUNNEL_DEST_MAC_USE_SERVICE 0x00020000 
#define BCM_MIM_PORT_EXTENDED               0x00040000 /* Use the extended P2P
                                                          service */
#define BCM_MIM_PORT_LOCAL_PROTECTION       0x00080000 /* Local protection
                                                          according to link
                                                          status */
#define BCM_MIM_PORT_SERVICE_ENCAP_WITH_ID  0x00100000 /* Used when the
                                                          application provides
                                                          service_encap_id */
#define BCM_MIM_PORT_FIX_PRIORITY           0x00200000 /* Assign fixed priority */
#define BCM_MIM_PORT_DEFAULT                0x00400000 /* MiM default SVP */
#define BCM_MIM_PORT_SERVICE_ENCAP_INGERSS_WIDE 0x00800000 /* Used when the
                                                          application creates
                                                          service_encap_id wide
                                                          mode */
#define BCM_MIM_PORT_FAILOVER_DROP          0x01000000 /* Be enabled to drop
                                                          data for 1+1
                                                          protection */
#define BCM_MIM_PORT_CASCADED               0x02000000 /* Create hierarchical
                                                          FEC */
#define BCM_MIM_PORT_ENCAP_OVERLAY          0x04000000 /* Create overlay link
                                                          layer */
#define BCM_MIM_PORT_EGRESS_SERVICE_PRI_REPLACE 0x08000000 /* Replace priority in
                                                          SD-TAG on MIM egress */
#define BCM_MIM_PORT_EGRESS_SERVICE_TPID_REPLACE 0x10000000 /* Replace TPID in SD-TAG
                                                          on MIM egress */
#define BCM_MIM_PORT_SHARE                  0x20000000 /* A flag only to be used
                                                          with
                                                          BCM_MIM_PORT_TYPE_PEER
                                                          flag for creating a
                                                          share peer port */
#define BCM_MIM_PORT_PEER_NO_BTAG           0x40000000 /* Control whether add
                                                          B-TAG for peer port
                                                          traffic */
#define BCM_MIM_PORT_NO_LEARN_ENTRY         0x80000000 

/* MAC-in-MAC Port match criteria. */
typedef enum bcm_mim_port_match_e {
    BCM_MIM_PORT_MATCH_INVALID = 0,     /* Illegal. */
    BCM_MIM_PORT_MATCH_PORT = 1, 
    BCM_MIM_PORT_MATCH_PORT_VLAN = 2, 
    BCM_MIM_PORT_MATCH_PORT_VLAN_STACKED = 3, 
    BCM_MIM_PORT_MATCH_TUNNEL_VLAN_SRCMAC = 4, 
    BCM_MIM_PORT_MATCH_LABEL = 5, 
    BCM_MIM_PORT_MATCH_SHARE = 6 
} bcm_mim_port_match_t;

/* MAC-in-MAC VPN structure. */
typedef struct bcm_mim_vpn_config_s {
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
    bcm_vlan_t default_vlan;            /* Requires BROADCOM_PREMIUM license */
} bcm_mim_vpn_config_t;

/* MAC-in-MAC Port structure. */
typedef struct bcm_mim_port_s {
    bcm_gport_t mim_port_id;            /* Interface id (VP). */
    uint32 flags;                       /* Configuration flags */
    int if_class;                       /* Interface class ID. */
    bcm_gport_t port;                   /* Physical port / trunk. */
    bcm_mim_port_match_t criteria;      /* Match criteria. */
    bcm_vlan_t match_vlan;              /* S-Tag. */
    bcm_vlan_t match_inner_vlan;        /* C-Tag. */
    bcm_mpls_label_t match_label;       /* MPLS label. */
    bcm_vlan_t match_tunnel_vlan;       /* B-domain VID. */
    bcm_mac_t match_tunnel_srcmac;      /* Source MAC address. */
    uint16 match_service_tpid;          /* Incoming SD tag TPID */
    bcm_vlan_t egress_tunnel_vlan;      /* B-domain VID. */
    bcm_mac_t egress_tunnel_srcmac;     /* Source MAC address. */
    bcm_mac_t egress_tunnel_dstmac;     /* Dest MAC address. */
    uint32 egress_tunnel_service;       /* Egress I-SID translation. */
    uint16 egress_service_tpid;         /* SD tag. */
    bcm_vlan_t egress_service_vlan;     /* SD tag. */
    uint8 egress_service_pri;           /* SD tag pri. */
    uint8 egress_service_cfi;           /* SD tag cfi. */
    bcm_if_t encap_id;                  /* Encap identifier. */
    bcm_failover_t failover_id;         /* Failover Object identifier. */
    bcm_gport_t failover_gport_id;      /* Failover MiM Port identifier. */
    bcm_policer_t policer_id;           /* Policer ID to be associated with the
                                           MiM gport */
    bcm_multicast_t failover_mc_group;  /* MC group used for bi-cast. */
    bcm_failover_t ingress_failover_id; /* 1+1 protection. */
    bcm_if_t service_encap_id;          /* Service encap Identifier */
    uint8 int_pri;                      /* Fixed priority */
    bcm_switch_network_group_t network_group_id; /* Split Horizon network group
                                           identifier */
    int inlif_counting_profile;         /* In LIF counting profile */
    int outlif_counting_profile;        /* Out LIF counting profile */
} bcm_mim_port_t;

/* Initialize the MAC-in-MAC VPN config structure. */
extern void bcm_mim_vpn_config_t_init(
    bcm_mim_vpn_config_t *mim_vpn_config);

/* Initialize the MAC-in-MAC port structure. */
extern void bcm_mim_port_t_init(
    bcm_mim_port_t *mim_port);

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the BCM MAC-in-MAC subsystem */
extern int bcm_mim_init(
    int unit);

/* Detach the MAC-in-MAC software module. */
extern int bcm_mim_detach(
    int unit);

/* Create a MAC-in-MAC VPN. */
extern int bcm_mim_vpn_create(
    int unit, 
    bcm_mim_vpn_config_t *info);

/* Destroy a MAC-in-MAC VPN. */
extern int bcm_mim_vpn_destroy(
    int unit, 
    bcm_mim_vpn_t vpn);

/* Destroy a MAC-in-MAC VPN. */
extern int bcm_mim_vpn_destroy_all(
    int unit);

/* Get a MAC-in-MAC VPN. */
extern int bcm_mim_vpn_get(
    int unit, 
    bcm_mim_vpn_t vpn, 
    bcm_mim_vpn_config_t *info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* MIM VPN callback function prototype. */
typedef int (*bcm_mim_vpn_traverse_cb)(
    int unit, 
    bcm_mim_vpn_config_t *info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* bcm_mim_vpn_traverse */
extern int bcm_mim_vpn_traverse(
    int unit, 
    bcm_mim_vpn_traverse_cb cb, 
    void *user_data);

/* Add a MAC-in-MAC port to a VPN. */
extern int bcm_mim_port_add(
    int unit, 
    bcm_mim_vpn_t vpn, 
    bcm_mim_port_t *mim_port);

/* Delete a MAC-in-MAC port from a VPN. */
extern int bcm_mim_port_delete(
    int unit, 
    bcm_mim_vpn_t vpn, 
    bcm_gport_t mim_port_id);

/* Delete a MAC-in-MAC port from a VPN. */
extern int bcm_mim_port_delete_all(
    int unit, 
    bcm_mim_vpn_t vpn);

/* Get a MAC-in-MAC port from a VPN. */
extern int bcm_mim_port_get(
    int unit, 
    bcm_mim_vpn_t vpn, 
    bcm_mim_port_t *mim_port);

/* Get all MAC-in-MAC ports from a VPN. */
extern int bcm_mim_port_get_all(
    int unit, 
    bcm_mim_vpn_t vpn, 
    int port_max, 
    bcm_mim_port_t *port_array, 
    int *port_count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Types of statistics that are maintained per MIM interface. */
typedef enum bcm_mim_stat_e {
    bcmMimStatIngressPackets = 0,   /* Packets that ingress on MIM interface */
    bcmMimStatIngressBytes = 1,     /* Bytes that ingress on MIM interface */
    bcmMimStatEgressPackets = 2,    /* Packets that egress on MIM interface */
    bcmMimStatEgressBytes = 3       /* Bytes that egress on MIM interface */
} bcm_mim_stat_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* attach counters entries to the given MiM I-SID entry */
extern int bcm_mim_lookup_id_stat_attach(
    int unit, 
    int lookup_id, 
    uint32 stat_counter_id);

/* detach counters entries to the given MiM I-SID entry */
extern int bcm_mim_lookup_id_stat_detach(
    int unit, 
    int lookup_id);

/* Get counter statistic values for specific MiM I-SID entry. */
extern int bcm_mim_lookup_id_stat_counter_get(
    int unit, 
    int lookup_id, 
    bcm_mim_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* 
 * Force an immediate counter update and retrieve counter statistic
 * values
 * for specific MiM I-SID entry.
 */
extern int bcm_mim_lookup_id_stat_counter_sync_get(
    int unit, 
    int lookup_id, 
    bcm_mim_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Set counter statistic values for specific MiM I-SID entry */
extern int bcm_mim_lookup_id_stat_counter_set(
    int unit, 
    int lookup_id, 
    bcm_mim_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Get stat counter id associated with given MiM I-SID entry */
extern int bcm_mim_lookup_id_stat_id_get(
    int unit, 
    int lookup_id, 
    bcm_mim_stat_t stat, 
    uint32 *stat_counter_id);

/* Attach counters entries to the given MiM VPN entry */
extern int bcm_mim_vpn_stat_attach(
    int unit, 
    bcm_mim_vpn_t vpn, 
    uint32 stat_counter_id);

/* Detach counters entries to the given MiM VPN entry */
extern int bcm_mim_vpn_stat_detach(
    int unit, 
    bcm_mim_vpn_t vpn);

/* Get counter statistic values for specific MiM VPN entry */
extern int bcm_mim_vpn_stat_counter_get(
    int unit, 
    bcm_mim_vpn_t vpn, 
    bcm_mim_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* 
 * Force an immediate counter update and retrieve counter statistic
 * values for
 * specific MiM VPN entry
 */
extern int bcm_mim_vpn_stat_counter_sync_get(
    int unit, 
    bcm_mim_vpn_t vpn, 
    bcm_mim_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Set counter statistic values for specific MiM VPN entry */
extern int bcm_mim_vpn_stat_counter_set(
    int unit, 
    bcm_mim_vpn_t vpn, 
    bcm_mim_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Get stat counter id associated with given MiM VPN entry */
extern int bcm_mim_vpn_stat_id_get(
    int unit, 
    bcm_mim_vpn_t vpn, 
    bcm_mim_stat_t stat, 
    uint32 *stat_counter_id);

#endif /* defined(INCLUDE_L3) */

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_MIM_H__ */
