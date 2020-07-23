/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_EXTENDER_H__
#define __BCM_EXTENDER_H__

#if defined(INCLUDE_L3)

#include <bcm/types.h>

/* extender port match criteria */
typedef enum bcm_extender_port_match_e {
    BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_INVALID = 0, /* Illegal. */
    BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_NONE = 1, /* No source match criteria. */
    BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_PORT_VLAN = 2, /* Match Port + Extender Port VID +
                                           VLAN. */
    BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_PORT_VLAN_STACKED = 3, /* Match Port + Extender Port VID + VLAN
                                           + CVLAN. */
    BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_PORT_CVLAN = 4, /* Match Port + Extended Port VID +
                                           CVLAN. */
    BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_UNTAGGED = 5, /* Match Port + Extender Port VID. */
    BCM_EXTENDER_PORT_MATCH_COUNT = 6   /* Must be last. */
} bcm_extender_port_match_t;

/* BCM_EXTENDER_PORT_* flags. */
#define BCM_EXTENDER_PORT_WITH_ID           0x00000001 /* Create extender port
                                                          with specified ID */
#define BCM_EXTENDER_PORT_REPLACE           0x00000002 /* Replace existing entry */
#define BCM_EXTENDER_PORT_MULTICAST         0x00000004 /* Create multicast type */
#define BCM_EXTENDER_PORT_ENCAP_WITH_ID     0x00000008 /* Use the specified
                                                          encap id to allocate
                                                          and setup
                                                          encapsulation. */
#define BCM_EXTENDER_PORT_EGRESS_TRANSPARENT 0x00000010 /* Use the flag to
                                                          transmit packets
                                                          transparently out of
                                                          this extender port. */
#define BCM_EXTENDER_PORT_DROP              0x00000020 /* Drop egress packets. */
#define BCM_EXTENDER_PORT_ID_ASSIGN_DISABLE 0x00000040 /* Matched traffic is not
                                                          assigned to the
                                                          extender port */
#define BCM_EXTENDER_PORT_INGRESS_WIDE      0x00000080 /* Creates encap id wide
                                                          mode */
#define BCM_EXTENDER_PORT_INITIAL_VLAN      0x00000100 /* Create extender port
                                                          with Initial VLAN */
#define BCM_EXTENDER_PORT_MATCH_NONE        0x00000200 /* Create extender port
                                                          with no matching
                                                          criteria */

/* ETAG PCP and DE fields select. */
typedef enum bcm_extender_pcp_de_select_e {
    BCM_EXTENDER_PCP_DE_SELECT_OUTER_TAG = 0, 
    BCM_EXTENDER_PCP_DE_SELECT_INNER_TAG = 1, 
    BCM_EXTENDER_PCP_DE_SELECT_DEFAULT   = 2, 
    BCM_EXTENDER_PCP_DE_SELECT_PHB       = 3 
} bcm_extender_pcp_de_select_t;

/* Extender port type. */
typedef struct bcm_extender_port_s {
    bcm_extender_port_match_t criteria; /* Match criteria. */
    uint32 flags;                       /* BCM_EXTENDER_PORT_xxx. */
    bcm_gport_t extender_port_id;       /* GPORT identifier. */
    bcm_gport_t port;                   /* Physical port / trunk */
    uint16 extended_port_vid;           /* Extender port VID. */
    bcm_vlan_t match_vlan;              /* Outer VLAN ID to match. */
    bcm_vlan_t match_inner_vlan;        /* Inner VLAN ID to match. */
    bcm_extender_pcp_de_select_t pcp_de_select; /* Selection of PCP and DE fields for
                                           egress ETAG. */
    int qos_map_id;                     /* Qos map id for egress etag mapping
                                           profile. */
    uint8 pcp;                          /* Default PCP field of ETAG. */
    uint8 de;                           /* Default DE field of ETAG. */
    bcm_if_t encap_id;                  /* Encap identifier. */
    bcm_gport_t failover_port_id;       /* Failover id. */
    bcm_failover_t failover_id;         /* Failover object index. */
    bcm_failover_t ingress_failover_id; /* 1+1 protection. */
    bcm_multicast_t failover_mc_group;  /* MC group for 1+1 scheme failover. */
    int inlif_counting_profile;         /* In LIF counting profile */
    int outlif_counting_profile;        /* Out LIF counting profile */
} bcm_extender_port_t;

/* BCM_EXTENDER_FORWARD_* flags. */
#define BCM_EXTENDER_FORWARD_REPLACE        0x00000001 /* Replace existing entry */
#define BCM_EXTENDER_FORWARD_MULTICAST      0x00000002 /* Create multicast type */
#define BCM_EXTENDER_FORWARD_UPSTREAM_ONLY  0x00000004 /* If set, the API
                                                          configures upstream
                                                          (ECID registration)
                                                          information only. */
#define BCM_EXTENDER_FORWARD_DOWNSTREAM_ONLY 0x00000008 /* If set, the API
                                                          configures downstream
                                                          information only. */
#define BCM_EXTENDER_FORWARD_WITH_ID        0x00000010 /* Set
                                                          extender_forward_id
                                                          value. Used in case of
                                                          downstream only */
#define BCM_EXTENDER_FORWARD_COPY_TO_CPU    0x00000020 /* Copy to CPU for
                                                          downstream */

/* Extender forwarding entry type. */
typedef struct bcm_extender_forward_s {
    uint32 flags;                       /* BCM_extender_FORWARD_xxx. */
    uint16 name_space;                  /* extender Namespace. */
    uint16 extended_port_vid;           /* Extended port VID. */
    bcm_gport_t dest_port;              /* Destination Gport. */
    bcm_multicast_t dest_multicast;     /* Destination Multicast Group. */
    bcm_gport_t extender_forward_id;    /* Downstream forwarding id. */
    uint32 class_id;                    /* Class id. */
} bcm_extender_forward_t;

/* BCM_EXTENDER_ENCAP_* flags. */
#define BCM_EXTENDER_ENCAP_WITH_ID  0x00000001 /* Add using the specified
                                                  extender_encap_id value */
#define BCM_EXTENDER_ENCAP_REPLACE  0x00000002 /* Replace existing
                                                  extender_encap_id entry */

/* Extender encapsulation entry type. */
typedef struct bcm_extender_encap_s {
    uint32 flags;                   /* BCM_EXTENDER_ENCAP_xxx. */
    uint16 extended_port_vid;       /* Extended port VID - PCID value. */
    bcm_gport_t extender_encap_id;  /* Extender encapsulation id. */
} bcm_extender_encap_t;

/* BCM_EXTENDER_EGRESS_* flags. */
#define BCM_EXTENDER_EGRESS_MULTICAST       (1 << 0)   /* Create multicast type */
#define BCM_EXTENDER_EGRESS_SERVICE_VLAN_ADD (1 << 1)   /* Add SD-tag */
#define BCM_EXTENDER_EGRESS_SERVICE_VLAN_DELETE (1 << 2)   /* Delete SD-tag */
#define BCM_EXTENDER_EGRESS_SERVICE_VLAN_REPLACE (1 << 3)   /* Replace SD-tag VLAN ID */
#define BCM_EXTENDER_EGRESS_SERVICE_PRI_REPLACE (1 << 4)   /* Replace SD-tag
                                                          Priority */
#define BCM_EXTENDER_EGRESS_SERVICE_TPID_REPLACE (1 << 5)   /* Replace SD-tag TPID */
#define BCM_EXTENDER_EGRESS_L3              (1 << 6)   /* Create L3 egress
                                                          object */

/* EXTENDER egress type. */
typedef struct bcm_extender_egress_s {
    uint32 flags;                       /* BCM_EXTENDER_EGRESS_xxx. */
    bcm_gport_t port;                   /* Physical port / trunk. */
    uint16 extended_port_vid;           /* extended port VID field of ETAG. */
    bcm_vlan_t match_vlan;              /* Outer VLAN ID to match. */
    bcm_extender_pcp_de_select_t pcp_de_select; /* Selection of PCP and DE fields for
                                           egress ETAG. */
    int qos_map_id;                     /* Qos map id for egress etag mapping
                                           profile. */
    uint8 pcp;                          /* Default PCP field of ETAG. */
    uint8 de;                           /* Default DE field of ETAG. */
    bcm_if_t egress_if;                 /* Egress object ID. */
    int service_qos_map_id;             /* Egress SD-tag Priority and CFI
                                           mapping profile ID. */
    uint16 service_tpid;                /* Egress SD-tag TPID. */
    bcm_vlan_t service_vlan;            /* Egress SD-tag VLAN ID. */
    uint8 service_pri;                  /* Egress SD-tag default Priority. */
    uint8 service_cfi;                  /* Egress SD-tag default CFI. */
    bcm_if_t intf;                      /* L3 interface. */
    uint32 multicast_flags;             /* BCM_L3_MULTICAST flag definitions. */
} bcm_extender_egress_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the BCM Port Extension module. */
extern int bcm_extender_init(
    int unit);

/* Detach the Port Extension module. */
extern int bcm_extender_cleanup(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize the Port Extender port structure. */
extern void bcm_extender_port_t_init(
    bcm_extender_port_t *extender_port);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a Port Extender port. */
extern int bcm_extender_port_add(
    int unit, 
    bcm_extender_port_t *extender_port);

/* Delete a Port Extender port. */
extern int bcm_extender_port_delete(
    int unit, 
    bcm_gport_t extender_port_id);

/* Delete all Port Extender ports. */
extern int bcm_extender_port_delete_all(
    int unit);

/* Get information about a Port Extender port. */
extern int bcm_extender_port_get(
    int unit, 
    bcm_extender_port_t *extender_port);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Extender port traverse callback. */
typedef int (*bcm_extender_port_traverse_cb)(
    int unit, 
    bcm_extender_port_t *extender_port, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse all Port Extender ports. */
extern int bcm_extender_port_traverse(
    int unit, 
    bcm_extender_port_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize the forwarding entry structure. */
extern void bcm_extender_forward_t_init(
    bcm_extender_forward_t *extender_forward_entry);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add a downstream forwarding entry. */
extern int bcm_extender_forward_add(
    int unit, 
    bcm_extender_forward_t *extender_forward_entry);

/* Delete a downstream forwarding entry. */
extern int bcm_extender_forward_delete(
    int unit, 
    bcm_extender_forward_t *extender_forward_entry);

/* Delete all downstream forwarding entries. */
extern int bcm_extender_forward_delete_all(
    int unit);

/* Get information about a downstream forwarding entry. */
extern int bcm_extender_forward_get(
    int unit, 
    bcm_extender_forward_t *extender_forward_entry);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Extender forwarding entry traverse callback. */
typedef int (*bcm_extender_forward_traverse_cb)(
    int unit, 
    bcm_extender_forward_t *extender_forward_entry, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse all downstream forwarding entries. */
extern int bcm_extender_forward_traverse(
    int unit, 
    bcm_extender_forward_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize the extender encapsulation entry structure. */
extern void bcm_extender_encap_t_init(
    bcm_extender_encap_t *extender_encap);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create an encapsulation entry. */
extern int bcm_extender_encap_create(
    int unit, 
    bcm_extender_encap_t *extender_encap);

/* Destroy extender encapsulation entry. */
extern int bcm_extender_encap_destroy(
    int unit, 
    bcm_gport_t *extender_encap_id);

/* Destroy all extender encapsulation entries. */
extern int bcm_extender_encap_destroy_all(
    int unit);

/* Get information extender encapsulation. */
extern int bcm_extender_encap_get(
    int unit, 
    bcm_extender_encap_t *extender_encap);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Extender port encapsulation entry traverse callback. */
typedef int (*bcm_extender_encap_traverse_cb)(
    int unit, 
    bcm_extender_encap_t *extender_encap, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse all extender encapsulation entries. */
extern int bcm_extender_encap_traverse(
    int unit, 
    bcm_extender_encap_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize the Extender egress object structure. */
extern void bcm_extender_egress_t_init(
    bcm_extender_egress_t *extender_egress);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a Extender egress object and add it to a Extender port. */
extern int bcm_extender_egress_add(
    int unit, 
    bcm_gport_t extender_port, 
    bcm_extender_egress_t *extender_egress);

/* Delete a Extender egress object from a Extender port and destroy it. */
extern int bcm_extender_egress_delete(
    int unit, 
    bcm_gport_t extender_port, 
    bcm_extender_egress_t *extender_egress);

/* 
 * Create a set of Extender egress objects and add them to a Extender
 * port.
 */
extern int bcm_extender_egress_set(
    int unit, 
    bcm_gport_t extender_port, 
    int array_size, 
    bcm_extender_egress_t *extender_egress_array);

/* Get Extender egress objects associated with a Extender port. */
extern int bcm_extender_egress_get(
    int unit, 
    bcm_gport_t extender_port, 
    bcm_extender_egress_t *extender_egress);

/* Get Extender egress objects associated with a Extender port. */
extern int bcm_extender_egress_get_all(
    int unit, 
    bcm_gport_t extender_port, 
    int array_size, 
    bcm_extender_egress_t *extender_egress_array, 
    int *count);

/* Delete all Extender egress objects associated with a Extender port. */
extern int bcm_extender_egress_delete_all(
    int unit, 
    bcm_gport_t extender_port);

#endif /* defined(INCLUDE_L3) */

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_EXTENDER_H__ */
