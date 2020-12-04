/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_NIV_H__
#define __BCM_NIV_H__

#if defined(INCLUDE_L3)

#include <bcm/types.h>

/* BCM_NIV_* flags. */
#define BCM_NIV_PORT_WITH_ID    (1 << 0)   /* create niv port with specified ID */

/* BCM_NIV_* flags. */
#define BCM_NIV_PORT_REPLACE    (1 << 1)   /* Replace existing entry */

/* BCM_NIV_* flags. */
#define BCM_NIV_PORT_MULTICAST  (1 << 2)   /* Create multicast type */

/* BCM_NIV_* flags. */
#define BCM_NIV_PORT_MATCH_NONE (1 << 3)   /* No matching criteria */

/* BCM_NIV_* flags. */
#define BCM_NIV_VNTAG_L_BIT_FORCE_1 (1 << 4)   /* Allow frames head back */

/* BCM_NIV_* flags. */
#define BCM_NIV_PORT_MATCH_DO_NOT_ADD   (1 << 5)   /* Do not add match criteria */

/* NIV port type. */
typedef struct bcm_niv_port_s {
    uint32 flags;                   /* BCM_NIV_PORT_xxx. */
    bcm_gport_t niv_port_id;        /* GPORT identifier. */
    bcm_gport_t port;               /* Physical port / trunk */
    uint16 virtual_interface_id;    /* Virtual Interface Identifier. */
    bcm_vlan_t match_vlan;          /* Outer VLAN ID to match. */
    uint16 match_service_tpid;      /* Ingress SD-tag TPID. */
    uint32 if_class;                /* Interface class ID. */
} bcm_niv_port_t;

/* BCM_NIV_FORWARD_* flags. */
#define BCM_NIV_FORWARD_REPLACE (1 << 1)   /* Replace existing entry */

/* BCM_NIV_FORWARD_* flags. */
#define BCM_NIV_FORWARD_MULTICAST   (1 << 2)   /* Create multicast type */

/* NIV forwarding table. */
typedef struct bcm_niv_forward_s {
    uint32 flags;                   /* BCM_NIV_FORWARD_xxx. */
    uint16 name_space;              /* NIV Namespace. */
    uint16 virtual_interface_id;    /* Virtual Interface Identifier. */
    bcm_gport_t dest_port;          /* Destination Gport. */
    bcm_multicast_t dest_multicast; /* Destination Multicast Group. */
} bcm_niv_forward_t;

/* BCM_NIV_EGRESS_* flags. */
#define BCM_NIV_EGRESS_MULTICAST            (1 << 0)   /* Create multicast type */
#define BCM_NIV_EGRESS_SERVICE_VLAN_ADD     (1 << 1)   /* Add SD-tag */
#define BCM_NIV_EGRESS_SERVICE_VLAN_DELETE  (1 << 2)   /* Delete SD-tag */
#define BCM_NIV_EGRESS_SERVICE_VLAN_REPLACE (1 << 3)   /* Replace SD-tag VLAN ID */
#define BCM_NIV_EGRESS_SERVICE_PRI_REPLACE  (1 << 4)   /* Replace SD-tag
                                                          Priority */
#define BCM_NIV_EGRESS_SERVICE_TPID_REPLACE (1 << 5)   /* Replace SD-tag TPID */
#define BCM_NIV_EGRESS_L3                   (1 << 6)   /* Create L3 egress
                                                          object */
#define BCM_NIV_EGRESS_P_BIT_CLEAR          (1 << 7)   /* Clear the P bit for
                                                          multicast packets */

/* NIV egress type. */
typedef struct bcm_niv_egress_s {
    uint32 flags;                   /* BCM_NIV_EGRESS_xxx */
    bcm_gport_t port;               /* Physical port / trunk */
    uint16 virtual_interface_id;    /* Virtual Interface Identifier */
    bcm_vlan_t match_vlan;          /* Outer VLAN ID to match */
    bcm_if_t egress_if;             /* Egress object ID */
    uint16 service_tpid;            /* Egress SD-tag TPID */
    bcm_vlan_t service_vlan;        /* Egress SD-tag VLAN ID */
    uint8 service_pri;              /* Egress SD-tag default Priority */
    uint8 service_cfi;              /* Egress SD-tag default CFI */
    int service_qos_map_id;         /* Egress SD-tag Priority and CFI mapping
                                       profile ID */
    bcm_if_t intf;                  /* L3 interface */
    uint32 multicast_flags;         /* BCM_L3_MULTICAST flag definitions */
} bcm_niv_egress_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the BCM NIV subsystem */
extern int bcm_niv_init(
    int unit);

/* Detach the NIV software module. */
extern int bcm_niv_cleanup(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize the NIV port structure. */
extern void bcm_niv_port_t_init(
    bcm_niv_port_t *niv_port);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a NIV port. */
extern int bcm_niv_port_add(
    int unit, 
    bcm_niv_port_t *niv_port);

/* Delete a NIV port. */
extern int bcm_niv_port_delete(
    int unit, 
    bcm_gport_t niv_port_id);

/* Delete all NIV ports. */
extern int bcm_niv_port_delete_all(
    int unit);

/* Get information about a NIV port. */
extern int bcm_niv_port_get(
    int unit, 
    bcm_niv_port_t *niv_port);

#endif /* BCM_HIDE_DISPATCHABLE */

/* NIV port traverse function prototype. */
typedef int (*bcm_niv_port_traverse_cb)(
    int unit, 
    bcm_niv_port_t *niv_port, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse all NIV ports. */
extern int bcm_niv_port_traverse(
    int unit, 
    bcm_niv_port_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize the IV forwarding entry structure. */
extern void bcm_niv_forward_t_init(
    bcm_niv_forward_t *iv_fwd_entry);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add an IV forwarding entry. */
extern int bcm_niv_forward_add(
    int unit, 
    bcm_niv_forward_t *iv_fwd_entry);

/* Delete an IV forwarding entry. */
extern int bcm_niv_forward_delete(
    int unit, 
    bcm_niv_forward_t *iv_fwd_entry);

/* Delete all IV forwarding entries. */
extern int bcm_niv_forward_delete_all(
    int unit);

/* Get information about an IV forwarding entry. */
extern int bcm_niv_forward_get(
    int unit, 
    bcm_niv_forward_t *iv_fwd_entry);

#endif /* BCM_HIDE_DISPATCHABLE */

/* NIV forward traverse function prototype. */
typedef int (*bcm_niv_forward_traverse_cb)(
    int unit, 
    bcm_niv_forward_t *iv_fwd_entry, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse all IV forwarding entries. */
extern int bcm_niv_forward_traverse(
    int unit, 
    bcm_niv_forward_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize the NIV egress object structure. */
extern void bcm_niv_egress_t_init(
    bcm_niv_egress_t *niv_egress);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a NIV egress object and add it to a NIV port. */
extern int bcm_niv_egress_add(
    int unit, 
    bcm_gport_t niv_port, 
    bcm_niv_egress_t *niv_egress);

/* Delete a NIV egress object from a NIV port and destroy it. */
extern int bcm_niv_egress_delete(
    int unit, 
    bcm_gport_t niv_port, 
    bcm_niv_egress_t *niv_egress);

/* Delete all NIV egress objects associated with a NIV port. */
extern int bcm_niv_egress_delete_all(
    int unit, 
    bcm_gport_t niv_port);

/* Create a set of NIV egress objects and add them to a NIV port. */
extern int bcm_niv_egress_set(
    int unit, 
    bcm_gport_t niv_port, 
    int array_size, 
    bcm_niv_egress_t *niv_egress_array);

/* Get NIV egress objects associated with a NIV port. */
extern int bcm_niv_egress_get(
    int unit, 
    bcm_gport_t niv_port, 
    int array_size, 
    bcm_niv_egress_t *niv_egress_array, 
    int *count);

#endif /* defined(INCLUDE_L3) */

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_NIV_H__ */
