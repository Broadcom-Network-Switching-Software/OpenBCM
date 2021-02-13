/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_TRILL_H__
#define __BCM_TRILL_H__

#if defined(INCLUDE_L3)

#include <bcm/types.h>
#include <bcm/multicast.h>
#include <bcm/l3.h>

#if defined(INCLUDE_L3)
/* BCM_TRILL_* flags. */
#define BCM_TRILL_PORT_WITH_ID              (1 << 0)   /* create trill port with
                                                          specified ID */
#define BCM_TRILL_PORT_DROP                 (1 << 1)   /* Drop matching packets */
#define BCM_TRILL_PORT_COPYTOCPU            (1 << 2)   /* Copy matching packets
                                                          to CPU */
#define BCM_TRILL_PORT_NETWORK              (1 << 3)   /* Network facing
                                                          interface */
#define BCM_TRILL_PORT_UNICAST              BCM_TRILL_PORT_NETWORK /* Create Unicast Rbridge */
#define BCM_TRILL_PORT_COUNTED              (1 << 4)   /* Maintain packet/byte
                                                          counts */
#define BCM_TRILL_PORT_REPLACE              (1 << 5)   /* Replace existing entry */
#define BCM_TRILL_PORT_MULTICAST            (1 << 6)   /* Create Root RBridge */
#define BCM_TRILL_PORT_LOCAL                (1 << 7)   /* Create Local RBridge */
#define BCM_TRILL_MULTICAST_ACCESS_TO_NETWORK (1 << 8)   /* Multicast from Access
                                                          to Network */
#define BCM_TRILL_MULTICAST_ESADI           (1 << 9)   /* Use ESADI bit as a
                                                          part of the MC key */
#define BCM_TRILL_MULTICAST_STATIC          (1 << 10)  /* Trill multicast
                                                          entries are marked as
                                                          static, not aged out */
#define BCM_TRILL_PORT_VIRTUAL              (1 << 11)  /* Virtual/pseudo
                                                          nicknames for trill
                                                          multi homing support */
#define BCM_TRILL_PORT_TUNNEL               (1 << 12)  /* Create unicast Rbridge
                                                          without transit
                                                          functionality */
#define BCM_TRILL_PORT_INGRESS              (1 << 13)  /* Create Ingress Rbridge */
#define BCM_TRILL_PORT_EGRESS               (1 << 14)  /* Create Egress Rbridge */
#define BCM_TRILL_MULTICAST_TRANSPARENT_SERVICE (1 << 15)  /* Trill multicast
                                                          transparent service */
#define BCM_TRILL_PORT_TERM_ETHERNET        (1 << 16)  /* Terminate Ethernet
                                                          header after
                                                          terminated Trill */
#define BCM_TRILL_PORT_INGRESS_WIDE         (1 << 17)  /* Create Trill port Wide
                                                          mode */
#define BCM_TRILL_MULTICAST_REPLACE_DYNAMIC (1 << 18)  /* Replace an existing L2
                                                          entry with TRILL
                                                          entry, when table is
                                                          full */
#endif

#if defined(INCLUDE_L3)
/* Trill VPN Flags. */
#define BCM_BCM_TRILL_VPN                   0x00000001 /* TRILL Fine Grained VPN */
#define BCM_BCM_TRILL_VPN_WITH_ID           0x00000002 /* Create VPN with
                                                          specified ID */
#define BCM_BCM_TRILL_VPN_REPLACE           0x00000004 /* Replace attributes of
                                                          an existing VPN */
#define BCM_BCM_TRILL_VPN_ACCESS            0x00000008 /* For mapping vpn ->
                                                          high+low vid */
#define BCM_BCM_TRILL_VPN_NETWORK           0x00000010 /* For mapping high+low
                                                          vid -> vpn */
#define BCM_BCM_TRILL_VPN_INVALID           0x00000020 /* Invalid TRILL VPN */
#define BCM_TRILL_VPN_TRANSPARENT_SERVICE   0x00000040 /* Create VPN with
                                                          transparent service */
#endif

#if defined(INCLUDE_L3)
/* Trill VPN structure. */
typedef struct bcm_trill_vpn_config_s {
    uint32 flags;                       /* Configuration flags */
    bcm_vpn_t vpn;                      /* VPN ID. */
    bcm_vlan_t high_vid;                /* Outer vlan */
    bcm_vlan_t low_vid;                 /* Inner vlan */
    bcm_multicast_t broadcast_group;    /* Broadcast group */
    bcm_multicast_t unknown_unicast_group; /* Unknown unicast group */
    bcm_multicast_t unknown_multicast_group; /* Unknown multicast group */
} bcm_trill_vpn_config_t;
#endif

#if defined(INCLUDE_L3)
/* TRILL port type. */
typedef struct bcm_trill_port_s {
    bcm_gport_t trill_port_id;          /* GPORT identifier. */
    uint32 flags;                       /* BCM_TRILL_PORT_xxx. */
    int if_class;                       /* Interface class ID. */
    bcm_gport_t port;                   /* Match port and/or egress port. */
    bcm_if_t egress_if;                 /* TRILL egress object. */
    bcm_trill_name_t name;              /* Destination RBridge Nickname. */
    int mtu;                            /* TRILL port MTU. */
    int hopcount;                       /* Unicast Hopcount for TRILL. */
    int multicast_hopcount;             /* Multicast Hopcount for TRILL. */
    bcm_if_t encap_id;                  /* Encap Identifier. */
    bcm_trill_name_t virtual_name;      /* Virtual RBridge Nickname */
    bcm_switch_network_group_t network_group_id; /* Split Horizon network group
                                           identifier. */
    int inlif_counting_profile;         /* In LIF counting profile */
    int outlif_counting_profile;        /* Out LIF counting profile */
} bcm_trill_port_t;
#endif

#if defined(INCLUDE_L3)
/* Initialize the TRILL port structure. */
extern void bcm_trill_port_t_init(
    bcm_trill_port_t *trill_port);
#endif

/* Initialize the TRILL VPN config structure. */
extern void bcm_trill_vpn_config_t_init(
    bcm_trill_vpn_config_t *trill_vpn_config);

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(INCLUDE_L3)
/* Initialize the BCM TRILL subsystem. */
extern int bcm_trill_init(
    int unit);
#endif

#if defined(INCLUDE_L3)
/* Detach the BCM TRILL subsystem. */
extern int bcm_trill_cleanup(
    int unit);
#endif

#if defined(INCLUDE_L3)
/* bcm_trill_port_add adds a TRILL port to TRILL network. */
extern int bcm_trill_port_add(
    int unit, 
    bcm_trill_port_t *trill_port);
#endif

#if defined(INCLUDE_L3)
/* bcm_trill_port_delete deletes a TRILL port from TRILL network. */
extern int bcm_trill_port_delete(
    int unit, 
    bcm_gport_t trill_port_id);
#endif

#if defined(INCLUDE_L3)
/* bcm_trill_port_delete deletes a TRILL port from TRILL network. */
extern int bcm_trill_port_delete_all(
    int unit);
#endif

#if defined(INCLUDE_L3)
/* Get TRILL port information. */
extern int bcm_trill_port_get(
    int unit, 
    bcm_trill_port_t *trill_port);
#endif

#if defined(INCLUDE_L3)
/* Get all TRILL port information. */
extern int bcm_trill_port_get_all(
    int unit, 
    int port_max, 
    bcm_trill_port_t *port_array, 
    int *port_count);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

#if defined(INCLUDE_L3)
/* RBridge TRILL port. */
typedef struct bcm_trill_rbridge_s {
    uint32 flags;           /* BCM_TRILL_RBRIDGE_xxx. */
    bcm_trill_name_t name;  /* RBridge Nickname. */
    bcm_if_t egress_if;     /* TRILL egress object. */
    int mtu;                /* TRILL port MTU. */
    int hopcount;           /* Hopcount for TRILL. */
} bcm_trill_rbridge_t;
#endif

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(INCLUDE_L3)
/* bcm_trill_rbridge_entry_add Add TRILL Rbridge entry. */
extern int bcm_trill_rbridge_entry_add(
    int unit, 
    bcm_trill_rbridge_t *trill_entry);
#endif

#if defined(INCLUDE_L3)
/* bcm_trill_rbridge_entry_delete Add TRILL Rbridge entry. */
extern int bcm_trill_rbridge_entry_delete(
    int unit, 
    bcm_trill_rbridge_t *trill_entry);
#endif

#if defined(INCLUDE_L3)
/* bcm_trill_rbridge_entry_get Add TRILL Rbridge entry. */
extern int bcm_trill_rbridge_entry_get(
    int unit, 
    bcm_trill_rbridge_t *trill_entry);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

#if defined(INCLUDE_L3)
/* TRILL Multicast Entry. */
typedef struct bcm_trill_multicast_entry_s {
    uint32 flags;               /* BCM_TRILL_MULTICAST_xxx. */
    bcm_trill_name_t root_name; /* Trill Root Rbridge. */
    bcm_vlan_t c_vlan;          /* Customer Vlan */
    bcm_mac_t c_dmac;           /* Customer Destination MAC Address. */
    bcm_multicast_t group;      /* TRILL Multicast Group */
    bcm_if_t encap_intf;        /* RPF interface for Multipath. */
    bcm_vlan_t c_vlan_inner;    /* low-vlan, when using fine-grained trill, and
                                   customer Ethernet header is double-tagged */
} bcm_trill_multicast_entry_t;
#endif

#if defined(INCLUDE_L3)
/* Initialize the TRILL multicast entry structure. */
extern void bcm_trill_multicast_entry_t_init(
    bcm_trill_multicast_entry_t *trill_multicast_entry);
#endif

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(INCLUDE_L3)
/* Add TRILL Multicast entry */
extern int bcm_trill_multicast_entry_add(
    int unit, 
    bcm_trill_multicast_entry_t *trill_mc_entry);
#endif

#if defined(INCLUDE_L3)
/* Delete TRILL Multicast entry */
extern int bcm_trill_multicast_entry_delete(
    int unit, 
    bcm_trill_multicast_entry_t *trill_mc_entry);
#endif

#if defined(INCLUDE_L3)
/* Get TRILL Multicast entry */
extern int bcm_trill_multicast_entry_get(
    int unit, 
    bcm_trill_multicast_entry_t *trill_mc_entry);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

/* TRILL multicast traverse function prototype. */
#if defined(INCLUDE_L3)
typedef int (*bcm_trill_multicast_entry_traverse_cb)(
    int unit, 
    bcm_trill_multicast_entry_t *trill_mc_entry, 
    void *user_data);
#endif

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(INCLUDE_L3)
/* 
 * Traverse all valid TRILL Multicast entry and call supplied callback
 * routine.
 */
extern int bcm_trill_multicast_entry_traverse(
    int unit, 
    bcm_trill_multicast_entry_traverse_cb cb, 
    void *user_data);
#endif

#if defined(INCLUDE_L3)
/* Delete TRILL Multicast entry */
extern int bcm_trill_multicast_delete_all(
    int unit, 
    bcm_trill_name_t root_name);
#endif

#if defined(INCLUDE_L3)
/* Add Multicast RPF entry */
extern int bcm_trill_multicast_source_add(
    int unit, 
    bcm_trill_name_t root_name, 
    bcm_trill_name_t source_rbridge_name, 
    bcm_gport_t port, 
    bcm_if_t encap_intf);
#endif

#if defined(INCLUDE_L3)
/* Delete Multicast RPF entry */
extern int bcm_trill_multicast_source_delete(
    int unit, 
    bcm_trill_name_t root_name, 
    bcm_trill_name_t source_rbridge_name, 
    bcm_gport_t port, 
    bcm_if_t encap_intf);
#endif

#if defined(INCLUDE_L3)
/* Get Multicast RPF entry */
extern int bcm_trill_multicast_source_get(
    int unit, 
    bcm_trill_name_t root_name, 
    bcm_trill_name_t source_rbridge_name, 
    bcm_gport_t *port);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

/* TRILL multicast RPF traverse function prototype. */
#if defined(INCLUDE_L3)
typedef int (*bcm_trill_multicast_source_traverse_cb)(
    int unit, 
    bcm_trill_name_t root_name, 
    bcm_trill_name_t source_rbridge_name, 
    bcm_gport_t port, 
    void *user_data);
#endif

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(INCLUDE_L3)
/* 
 * Traverse all valid TRILL Multicast RPF entries and call supplied
 * callback routine.
 */
extern int bcm_trill_multicast_source_traverse(
    int unit, 
    bcm_trill_multicast_source_traverse_cb cb, 
    void *user_data);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

#if defined(INCLUDE_L3)
/* TRILL multicast adjacency entry. */
typedef struct bcm_trill_multicast_adjacency_s {
    bcm_gport_t port;       /* Match Trill network port. */
    uint32 flags;           /* BCM_TRILL_MULTICAST_ADJACENCY_xxx. */
    bcm_if_t encap_intf;    /* Neighbor interface. */
} bcm_trill_multicast_adjacency_t;
#endif

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(INCLUDE_L3)
/* Add Multicast adjacency entry */
extern int bcm_trill_multicast_adjacency_add(
    int unit, 
    bcm_trill_multicast_adjacency_t *trill_multicast_adjacency);
#endif

#if defined(INCLUDE_L3)
/* Delete Multicast adjacency entry */
extern int bcm_trill_multicast_adjacency_delete(
    int unit, 
    bcm_trill_multicast_adjacency_t *trill_multicast_adjacency);
#endif

#if defined(INCLUDE_L3)
/* Delete all Multicast adjacency entry */
extern int bcm_trill_multicast_adjacency_delete_all(
    int unit);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

/* Trill multicast adjacency callback function prototype. */
#if defined(INCLUDE_L3)
typedef int (*bcm_trill_multicast_adjacency_traverse_cb)(
    int unit, 
    bcm_trill_multicast_adjacency_t *info, 
    void *user_data);
#endif

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(INCLUDE_L3)
/* 
 * Traverse all valid TRILL Multicast adjacency entries and call supplied
 * callback routine.
 */
extern int bcm_trill_multicast_adjacency_traverse(
    int unit, 
    bcm_trill_multicast_adjacency_traverse_cb cb, 
    void *user_data);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

#if defined(INCLUDE_L3)
/* TRILL statistics counters. */
typedef enum bcm_trill_stat_e {
    bcmTrillInPkts = 0, 
    bcmTrillOutPkts = 1, 
    bcmTrillErrorPkts = 2, 
    bcmTrillNameMissPkts = 3, 
    bcmTrillRpfFailPkts = 4, 
    bcmTrillTtlFailPkts = 5 
} bcm_trill_stat_t;
#endif

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(INCLUDE_L3)
/* Get TRILL Stats. */
extern int bcm_trill_stat_get(
    int unit, 
    bcm_port_t port, 
    bcm_trill_stat_t stat, 
    uint64 *val);
#endif

#if defined(INCLUDE_L3)
/* Get TRILL Stats. */
extern int bcm_trill_stat_get32(
    int unit, 
    bcm_port_t port, 
    bcm_trill_stat_t stat, 
    uint32 *val);
#endif

#if defined(INCLUDE_L3)
/* Clear TRILL Stats. */
extern int bcm_trill_stat_clear(
    int unit, 
    bcm_port_t port, 
    bcm_trill_stat_t stat);
#endif

#if defined(INCLUDE_L3)
/* Create a TRILL VPN. */
extern int bcm_trill_vpn_create(
    int unit, 
    bcm_trill_vpn_config_t *info);
#endif

#if defined(INCLUDE_L3)
/* Destroy a TRILL VPN. */
extern int bcm_trill_vpn_destroy(
    int unit, 
    bcm_vpn_t vpn);
#endif

#if defined(INCLUDE_L3)
/* Destroy a TRILL VPN. */
extern int bcm_trill_vpn_destroy_all(
    int unit);
#endif

#if defined(INCLUDE_L3)
/* Get a TRILL VPN. */
extern int bcm_trill_vpn_get(
    int unit, 
    bcm_vpn_t vpn, 
    bcm_trill_vpn_config_t *info);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

/* Trill VPN callback function prototype. */
#if defined(INCLUDE_L3)
typedef int (*bcm_trill_vpn_traverse_cb)(
    int unit, 
    bcm_trill_vpn_config_t *info, 
    void *user_data);
#endif

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(INCLUDE_L3)
/* bcm_trill_vpn_traverse */
extern int bcm_trill_vpn_traverse(
    int unit, 
    bcm_trill_vpn_traverse_cb cb, 
    void *user_data);
#endif

#endif /* defined(INCLUDE_L3) */

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_TRILL_H__ */
