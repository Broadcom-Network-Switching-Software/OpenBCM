/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_L2GRE_H__
#define __BCM_L2GRE_H__

#if defined(INCLUDE_L3)

#include <bcm/types.h>
#include <bcm/l3.h>
#include <bcm/tunnel.h>

#if defined(INCLUDE_L3)
/* BCM_L2GRE_VPN_* flags for L2GRE VPN Create. */
#define BCM_L2GRE_VPN_ELINE                 0x00000001 
#define BCM_L2GRE_VPN_ELAN                  0x00000002 
#define BCM_L2GRE_VPN_WITH_ID               0x00000004 
#define BCM_L2GRE_VPN_WITH_VPNID            0x00000008 
#define BCM_L2GRE_VPN_SERVICE_TAGGED        0x00000010 
#define BCM_L2GRE_VPN_SERVICE_VLAN_ADD      0x00000020 
#define BCM_L2GRE_VPN_SERVICE_VLAN_REPLACE  0x00000040 
#define BCM_L2GRE_VPN_SERVICE_VLAN_DELETE   0x00000080 
#define BCM_L2GRE_VPN_SERVICE_VLAN_TPID_REPLACE 0x00000100 
#define BCM_L2GRE_VPN_SERVICE_VLAN_PRI_TPID_REPLACE 0x00000200 
#define BCM_L2GRE_VPN_SERVICE_VLAN_PRI_REPLACE 0x00000400 
#define BCM_L2GRE_VPN_SERVICE_PRI_REPLACE   0x00000800 
#define BCM_L2GRE_VPN_SERVICE_TPID_REPLACE  0x00001000 
#define BCM_L2GRE_VPN_REPLACE               0x00002000 
#define BCM_L2GRE_VPN_UNKNOWN_UCAST_REPLACE 0x00004000 
#define BCM_L2GRE_VPN_UNKNOWN_MCAST_REPLACE 0x00008000 
#define BCM_L2GRE_VPN_BCAST_REPLACE         0x00010000 
#endif

#if defined(INCLUDE_L3)
/* BCM_L2GRE_MULTICAST_TUNNEL_STATE_BUD_* flags. */
#define BCM_L2GRE_MULTICAST_TUNNEL_STATE_BUD_ENABLE 0x00000001 
#define BCM_L2GRE_MULTICAST_TUNNEL_STATE_BUD_DISABLE 0x00000002 
#endif

#if defined(INCLUDE_L3)
/* L2GRE VPN Definitions. */
#define BCM_L2GRE_VPN_INVALID   0xFFFF     /* Invalid L2GRE VPN */
#endif

#if defined(INCLUDE_L3)
/* L2GRE-VPN Config Structure. */
typedef struct bcm_l2gre_vpn_config_s {
    uint32 flags;                       /* BCM_L2GRE_VPN_xxx. */
    bcm_vpn_t vpn;                      /* L2GRE VPN */
    uint32 vpnid;                       /* VPNID */
    uint8 pkt_pri;                      /* Packet Priority */
    uint8 pkt_cfi;                      /* Packet CFI */
    uint16 egress_service_tpid;         /* Service TPID */
    bcm_vlan_t egress_service_vlan;     /* Service VLAN */
    bcm_multicast_t broadcast_group; 
    bcm_multicast_t unknown_unicast_group; 
    bcm_multicast_t unknown_multicast_group; 
    bcm_gport_t match_port_class;       /* local port vlan domain */
} bcm_l2gre_vpn_config_t;
#endif

#if defined(INCLUDE_L3)
/* Initialize L2GRE VPN config structure. */
extern void bcm_l2gre_vpn_config_t_init(
    bcm_l2gre_vpn_config_t *info);
#endif

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(INCLUDE_L3)
/* 
 * Create or update L2GRE VPN.
 *  When create a L2GRE VPN:
 * If use BCM_L2GRE_VPN_WITH_VPNID, both VFI and VNID will be created.
 * If not use flag BCM_L2GRE_VPN_WITH_VPNID, only VFI will be created.
 *  When update an exist L2GRE VPN(BCM_L2GRE_VPN_REPLACE should be used):
 * If use both BCM_L2GRE_VPN_REPLACE and BCM_L2GRE_VPN_WITH_VPNID, both
 * VFI and VNID will be created.
 * If only use BCM_L2GRE_VPN_REPLACE, the VNID will be removed.
 */
extern int bcm_l2gre_vpn_create(
    int unit, 
    bcm_l2gre_vpn_config_t *info);
#endif

#if defined(INCLUDE_L3)
/* Destroy L2GRE VPN. */
extern int bcm_l2gre_vpn_destroy(
    int unit, 
    bcm_vpn_t l2vpn);
#endif

#if defined(INCLUDE_L3)
/* Destroy L2GRE VPN. */
extern int bcm_l2gre_vpn_destroy_all(
    int unit);
#endif

#if defined(INCLUDE_L3)
/* Get L2GRE VPN. */
extern int bcm_l2gre_vpn_get(
    int unit, 
    bcm_vpn_t l2vpn, 
    bcm_l2gre_vpn_config_t *info);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

/* L2GRE vpn callback function prototype */
#if defined(INCLUDE_L3)
typedef int(*bcm_l2gre_vpn_traverse_cb)(
    int unit, 
    bcm_l2gre_vpn_config_t *info, 
    void *user_data);
#endif

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(INCLUDE_L3)
/* 
 * Traverse all valid L2GRE VPN entries and call the supplied callback
 * routine.
 */
extern int bcm_l2gre_vpn_traverse(
    int unit, 
    bcm_l2gre_vpn_traverse_cb cb, 
    void *user_data);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

#if defined(INCLUDE_L3)
/* L2GRE port match criteria. */
typedef enum bcm_l2gre_port_match_e {
    BCM_L2GRE_PORT_MATCH_INVALID = 0,   /* Illegal. */
    BCM_L2GRE_PORT_MATCH_NONE = 1,      /* No source match criteria. */
    BCM_L2GRE_PORT_MATCH_PORT = 2,      /* {Module, Port} or Trunk. */
    BCM_L2GRE_PORT_MATCH_PORT_VLAN = 3, /* Mod/port/trunk + outer VLAN. */
    BCM_L2GRE_PORT_MATCH_PORT_INNER_VLAN = 4, /* Mod/port/trunk + inner VLAN. */
    BCM_L2GRE_PORT_MATCH_PORT_VLAN_STACKED = 5, /* Mod/port/trunk + outer/inner VLAN. */
    BCM_L2GRE_PORT_MATCH_VLAN_PRI = 6,  /* Mod/port/trunk + VLAN-PRI + VLAN-CFI. */
    BCM_L2GRE_PORT_MATCH_VPNID = 7,     /* Match L2GRE VPNID */
    BCM_L2GRE_PORT_MATCH_SHARE = 8,     /* Shared logical port */
    BCM_L2GRE_PORT_MATCH_PORT_ITAG = 9, /* Mod/port/trunk + inner TAG. */
    BCM_L2GRE_PORT_MATCH_COUNT = 10     /* Must be last. */
} bcm_l2gre_port_match_t;
#endif

#if defined(INCLUDE_L3)
/* BCM_L2GRE_* flags. */
#define BCM_L2GRE_PORT_WITH_ID              (1 << 0)   /* create L2GRE port with
                                                          specified ID */
#define BCM_L2GRE_PORT_DROP                 (1 << 1)   /* Drop matching packets */
#define BCM_L2GRE_PORT_NETWORK              (1 << 2)   /* Network facing
                                                          interface */
#define BCM_L2GRE_PORT_COUNTED              (1 << 3)   /* Maintain packet/byte
                                                          counts */
#define BCM_L2GRE_PORT_REPLACE              (1 << 4)   /* Replace existing entry */
#define BCM_L2GRE_PORT_INT_PRI_SET          (1 << 5)   /* Use Specified pri for
                                                          internal priority */
#define BCM_L2GRE_PORT_INT_PRI_MAP          (1 << 6)   /* Use specified exp_map
                                                          for internal priority */
#define BCM_L2GRE_PORT_EGRESS_TUNNEL        (1 << 7)   /* Specified Egress
                                                          tunnel is valid */
#define BCM_L2GRE_PORT_SERVICE_TAGGED       (1 << 8)   /* Service tag mode */
#define BCM_L2GRE_PORT_SERVICE_VLAN_ADD     (1 << 9)   /* Add SD-tag */
#define BCM_L2GRE_PORT_SERVICE_VLAN_REPLACE (1 << 10)  /* Replace VLAN not TPID */
#define BCM_L2GRE_PORT_SERVICE_VLAN_DELETE  (1 << 11)  /* Delete VLAN Tag */
#define BCM_L2GRE_PORT_SERVICE_VLAN_TPID_REPLACE (1 << 12)  /* Replace VLAN and TPID */
#define BCM_L2GRE_PORT_SERVICE_VLAN_PRI_TPID_REPLACE (1 << 13)  /* Replace VLAN, VLAN-PRI
                                                          and TPID */
#define BCM_L2GRE_PORT_SERVICE_VLAN_PRI_REPLACE (1 << 14)  /* Replace VLAN and
                                                          VLAN-PRI */
#define BCM_L2GRE_PORT_SERVICE_PRI_REPLACE  (1 << 15)  /* Replace VLAN-PRI only */
#define BCM_L2GRE_PORT_SERVICE_TPID_REPLACE (1 << 16)  /* Replace TPID only */
#define BCM_L2GRE_PORT_MULTICAST            (1 << 19)  /* Create Multicast-only
                                                          L2GRE Tunnel */
#define BCM_L2GRE_PORT_DEFAULT              (1 << 20)  /* Create L2GRE Default
                                                          Network Port */
#define BCM_L2GRE_PORT_ENABLE_VLAN_CHECKS   (1 << 21)  /* Enable vlan check,
                                                          only for access VP
                                                          derived from
                                                          VLAN_XLATE */
#endif

#if defined(INCLUDE_L3)
/* L2GRE Gport */
typedef struct bcm_l2gre_port_s {
    bcm_gport_t l2gre_port_id;          /* GPORT identifier. */
    uint32 flags;                       /* BCM_L2GRE_PORT_xxx. */
    uint32 if_class;                    /* Interface class ID. */
    uint16 int_pri;                     /* Internal Priority */
    uint8 pkt_pri;                      /* Packet Priority */
    uint8 pkt_cfi;                      /* Packet CFI */
    uint16 egress_service_tpid;         /* Service Vlan TPID Value */
    bcm_vlan_t egress_service_vlan;     /* SD-TAG Vlan ID. */
    uint16 mtu;                         /* MTU */
    bcm_gport_t match_port;             /* Match port / trunk */
    bcm_l2gre_port_match_t criteria;    /* Match criteria */
    bcm_vlan_t match_vlan;              /* Outer VLAN ID to match. */
    bcm_vlan_t match_inner_vlan;        /* Inner VLAN ID to match. */
    bcm_gport_t egress_tunnel_id;       /* Tunnel Initiator ID */
    bcm_gport_t match_tunnel_id;        /* Tunnel Terminator ID */
    bcm_if_t egress_if;                 /* L2GRE egress object. */
    bcm_switch_network_group_t network_group_id; /* Split Horizon network group
                                           identifier. */
} bcm_l2gre_port_t;
#endif

#if defined(INCLUDE_L3)
/* Initialize L2GRE port structure. */
extern void bcm_l2gre_port_t_init(
    bcm_l2gre_port_t *l2gre_port);
#endif

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(INCLUDE_L3)
/* Initialize the BCM L2GRE subsystem. */
extern int bcm_l2gre_init(
    int unit);
#endif

#if defined(INCLUDE_L3)
/* Detach the BCM L2GRE subsystem. */
extern int bcm_l2gre_cleanup(
    int unit);
#endif

#if defined(INCLUDE_L3)
/* bcm_l2gre_port_add adds a Access or Network L2GRE port */
extern int bcm_l2gre_port_add(
    int unit, 
    bcm_vpn_t l2vpn, 
    bcm_l2gre_port_t *l2gre_port);
#endif

#if defined(INCLUDE_L3)
/* bcm_l2gre_port_delete L2GRE port from L2GRE network. */
extern int bcm_l2gre_port_delete(
    int unit, 
    bcm_vpn_t l2vpn, 
    bcm_gport_t l2gre_port_id);
#endif

#if defined(INCLUDE_L3)
/* bcm_l2gre_port_delete L2GRE port from L2GRE network. */
extern int bcm_l2gre_port_delete_all(
    int unit, 
    bcm_vpn_t l2vpn);
#endif

#if defined(INCLUDE_L3)
/* Get L2GRE port information. */
extern int bcm_l2gre_port_get(
    int unit, 
    bcm_vpn_t l2vpn, 
    bcm_l2gre_port_t *l2gre_port);
#endif

#if defined(INCLUDE_L3)
/* Get all L2GRE port information. */
extern int bcm_l2gre_port_get_all(
    int unit, 
    bcm_vpn_t l2vpn, 
    int port_max, 
    bcm_l2gre_port_t *port_array, 
    int *port_count);
#endif

#if defined(INCLUDE_L3)
/* Create L2GRE Tunnel Initiator. */
extern int bcm_l2gre_tunnel_initiator_create(
    int unit, 
    bcm_tunnel_initiator_t *info);
#endif

#if defined(INCLUDE_L3)
/* Delete L2GRE Tunnel Initiator. */
extern int bcm_l2gre_tunnel_initiator_destroy(
    int unit, 
    bcm_gport_t l2gre_tunnel_id);
#endif

#if defined(INCLUDE_L3)
/* Destroy all L2GRE Tunnel Initiators */
extern int bcm_l2gre_tunnel_initiator_destroy_all(
    int unit);
#endif

#if defined(INCLUDE_L3)
/* Get  L2GRE Tunnel Initiator. */
extern int bcm_l2gre_tunnel_initiator_get(
    int unit, 
    bcm_tunnel_initiator_t *info);
#endif

#if defined(INCLUDE_L3)
/* Traverse L2GRE Tunnel initiator. */
extern int bcm_l2gre_tunnel_initiator_traverse(
    int unit, 
    bcm_tunnel_initiator_traverse_cb cb, 
    void *user_data);
#endif

#if defined(INCLUDE_L3)
/* Create L2GRE Tunnel terminator. */
extern int bcm_l2gre_tunnel_terminator_create(
    int unit, 
    bcm_tunnel_terminator_t *info);
#endif

#if defined(INCLUDE_L3)
/* Update L2GRE Tunnel terminator. */
extern int bcm_l2gre_tunnel_terminator_update(
    int unit, 
    bcm_tunnel_terminator_t *info);
#endif

#if defined(INCLUDE_L3)
/* Delete L2GRE Tunnel terminator. */
extern int bcm_l2gre_tunnel_terminator_destroy(
    int unit, 
    bcm_gport_t l2gre_tunnel_id);
#endif

#if defined(INCLUDE_L3)
/* Destroy all L2GRE Tunnel terminators */
extern int bcm_l2gre_tunnel_terminator_destroy_all(
    int unit);
#endif

#if defined(INCLUDE_L3)
/* Get L2GRE Tunnel terminator. */
extern int bcm_l2gre_tunnel_terminator_get(
    int unit, 
    bcm_tunnel_terminator_t *info);
#endif

#if defined(INCLUDE_L3)
/* Traverse L2GRE Tunnel terminator. */
extern int bcm_l2gre_tunnel_terminator_traverse(
    int unit, 
    bcm_tunnel_terminator_traverse_cb cb, 
    void *user_data);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

/* Ingress and Egress statistics maintained per L2gre. */
typedef enum bcm_l2gre_stat_e {
    bcmL2greInPackets = 0,  /* Packets that ingress on the L2gre */
    bcmL2greOutPackets = 1, /* Packets that egress on the L2gre */
    bcmL2greInBytes = 2,    /* Bytes that ingress on the L2gre */
    bcmL2greOutBytes = 3    /* Bytes that egress on the L2gre */
} bcm_l2gre_stat_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Attach counters entries to the given l2gre vp/vpn index */
extern int bcm_l2gre_stat_attach(
    int unit, 
    bcm_gport_t port, 
    bcm_vpn_t vpn, 
    uint32 stat_counter_id);

/* Detach  counters entries to the given l2gre vp/vpn index. */
extern int bcm_l2gre_stat_detach(
    int unit, 
    bcm_gport_t port, 
    bcm_vpn_t vpn);

/* Get counter statistic values for a l2gre vp/vpn index. */
extern int bcm_l2gre_stat_counter_get(
    int unit, 
    bcm_gport_t port, 
    bcm_vpn_t vpn, 
    bcm_l2gre_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* 
 * Force an immediate counter update and retrieve counter statistic
 * values for
 * a l2gre vp/vpn index.
 */
extern int bcm_l2gre_stat_counter_sync_get(
    int unit, 
    bcm_gport_t port, 
    bcm_vpn_t vpn, 
    bcm_l2gre_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Set counter statistic values for a l2gre vp/vpn index */
extern int bcm_l2gre_stat_counter_set(
    int unit, 
    bcm_gport_t port, 
    bcm_vpn_t vpn, 
    bcm_l2gre_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Get multiple counter statistic values for multiple l2gre vp/vpn index. */
extern int bcm_l2gre_stat_multi_get(
    int unit, 
    bcm_gport_t port, 
    bcm_vpn_t vpn, 
    int nstat, 
    bcm_l2gre_stat_t *stat_arr, 
    uint64 *value_arr);

/* 
 * Get 32bit multiple counter statistic values for multiple l2gre vp/vpn
 * index.
 */
extern int bcm_l2gre_stat_multi_get32(
    int unit, 
    bcm_gport_t port, 
    bcm_vpn_t vpn, 
    int nstat, 
    bcm_l2gre_stat_t *stat_arr, 
    uint32 *value_arr);

/* Set multiple counter statistic values for multiple l2gre vp/vpn index. */
extern int bcm_l2gre_stat_multi_set(
    int unit, 
    bcm_gport_t port, 
    bcm_vpn_t vpn, 
    int nstat, 
    bcm_l2gre_stat_t *stat_arr, 
    uint64 *value_arr);

/* 
 * Set 32bit multiple counter statistic values for multiple l2gre vp/vpn
 * index.
 */
extern int bcm_l2gre_stat_multi_set32(
    int unit, 
    bcm_gport_t port, 
    bcm_vpn_t vpn, 
    int nstat, 
    bcm_l2gre_stat_t *stat_arr, 
    uint32 *value_arr);

/* Get stat counter ID associated with given l2gre vp/vpn index. */
extern int bcm_l2gre_stat_id_get(
    int unit, 
    bcm_gport_t port, 
    bcm_vpn_t vpn, 
    bcm_l2gre_stat_t stat, 
    uint32 *stat_counter_id);

#endif /* defined(INCLUDE_L3) */

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_L2GRE_H__ */
