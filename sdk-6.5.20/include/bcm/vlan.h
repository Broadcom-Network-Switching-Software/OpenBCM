/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_VLAN_H__
#define __BCM_VLAN_H__

#include <bcm/types.h>
#include <bcm/multicast.h>
#include <bcm/policer.h>
#include <bcm/qos.h>
#include <bcm/port.h>

#define BCM_VLAN_VID_DISABLE    BCM_VLAN_INVALID 

/* Initialize a VLAN data information structure. */
typedef struct bcm_vlan_data_s {
    bcm_vlan_t vlan_tag; 
    bcm_pbmp_t port_bitmap; 
    bcm_pbmp_t ut_port_bitmap; 
} bcm_vlan_data_t;

/* Backward compatibility: Do not use. */
typedef bcm_vlan_data_t vlan_data_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the VLAN module. */
extern int bcm_vlan_init(
    int unit);

/* Allocate and configure a VLAN on the BCM device. */
extern int bcm_vlan_create(
    int unit, 
    bcm_vlan_t vid);

/* Deallocate VLAN from the BCM device. */
extern int bcm_vlan_destroy(
    int unit, 
    bcm_vlan_t vid);

/* Destroy all VLANs except the default VLAN. */
extern int bcm_vlan_destroy_all(
    int unit);

/* Add ports to the specified VLAN. */
extern int bcm_vlan_port_add(
    int unit, 
    bcm_vlan_t vid, 
    bcm_pbmp_t pbmp, 
    bcm_pbmp_t ubmp);

/* Remove ports from a specified VLAN. */
extern int bcm_vlan_port_remove(
    int unit, 
    bcm_vlan_t vid, 
    bcm_pbmp_t pbmp);

/* Retrieves a list of the member ports of an existing VLAN. */
extern int bcm_vlan_port_get(
    int unit, 
    bcm_vlan_t vid, 
    bcm_pbmp_t *pbmp, 
    bcm_pbmp_t *ubmp);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flags for bcm_vlan_gport_add. */
#define BCM_VLAN_GPORT_ADD_UNTAGGED         0x00000001 
#define BCM_VLAN_GPORT_ADD_INGRESS_ONLY     0x00000002 
#define BCM_VLAN_GPORT_ADD_EGRESS_ONLY      0x00000004 
#define BCM_VLAN_GPORT_ADD_MEMBER_REPLACE   0x00000008 
#define BCM_VLAN_GPORT_ADD_STP_DISABLE      0x00000000 
#define BCM_VLAN_GPORT_ADD_STP_BLOCK        0x00000010 
#define BCM_VLAN_GPORT_ADD_STP_LEARN        0x00000020 
#define BCM_VLAN_GPORT_ADD_STP_FORWARD      0x00000030 
#define BCM_VLAN_GPORT_ADD_STP_MASK         0x00000030 
#define BCM_VLAN_GPORT_ADD_VP_VLAN_MEMBERSHIP 0x00000080 
#define BCM_VLAN_GPORT_ADD_UNKNOWN_UCAST_DO_NOT_ADD 0x00000100 
#define BCM_VLAN_GPORT_ADD_UNKNOWN_MCAST_DO_NOT_ADD 0x00000200 
#define BCM_VLAN_GPORT_ADD_BCAST_DO_NOT_ADD 0x00000400 
#define BCM_VLAN_GPORT_ADD_TAG_DO_NOT_UPDATE 0x00000800 
#define BCM_VLAN_GPORT_ADD_MEMBER_DO_NOT_UPDATE 0x00001000 
#define BCM_VLAN_GPORT_ADD_SERVICE          0x00002000 
#define BCM_VLAN_GPORT_ADD_EGRESS_STP_DISABLE BCM_VLAN_GPORT_ADD_STP_DISABLE 
#define BCM_VLAN_GPORT_ADD_EGRESS_STP_BLOCK 0x00010000 
#define BCM_VLAN_GPORT_ADD_EGRESS_STP_LEARN 0x00020000 
#define BCM_VLAN_GPORT_ADD_EGRESS_STP_FORWARD 0x00030000 
#define BCM_VLAN_GPORT_ADD_EGRESS_STP_MASK  0x00030000 
#define BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY   0x00040000 
#define BCM_VLAN_GPORT_ADD_STAT_INGRESS_ENABLE 0x00080000 
#define BCM_VLAN_GPORT_ADD_STAT_EGRESS_ENABLE 0x00100000 

/* Obselete flags for bcm_vlan_gport_add. */
#define BCM_VLAN_PORT_UNTAGGED              BCM_VLAN_GPORT_ADD_UNTAGGED 
#define BCM_VLAN_PORT_INGRESS_ONLY          BCM_VLAN_GPORT_ADD_INGRESS_ONLY 
#define BCM_VLAN_PORT_EGRESS_ONLY           BCM_VLAN_GPORT_ADD_EGRESS_ONLY 
#define BCM_VLAN_PORT_MEMBER_REPLACE        BCM_VLAN_GPORT_ADD_MEMBER_REPLACE 
#define BCM_VLAN_PORT_STP_DISABLE           BCM_VLAN_GPORT_ADD_STP_DISABLE 
#define BCM_VLAN_PORT_STP_BLOCK             BCM_VLAN_GPORT_ADD_STP_BLOCK 
#define BCM_VLAN_PORT_STP_LEARN             BCM_VLAN_GPORT_ADD_STP_LEARN 
#define BCM_VLAN_PORT_STP_FORWARD           BCM_VLAN_GPORT_ADD_STP_FORWARD 
#define BCM_VLAN_PORT_STP_MASK              BCM_VLAN_GPORT_ADD_STP_MASK 
#define BCM_VLAN_PORT_VP_VLAN_MEMBERSHIP    BCM_VLAN_GPORT_ADD_VP_VLAN_MEMBERSHIP 
#define BCM_VLAN_PORT_UNKNOWN_UCAST_DO_NOT_ADD BCM_VLAN_GPORT_ADD_UNKNOWN_UCAST_DO_NOT_ADD 
#define BCM_VLAN_PORT_UNKNOWN_MCAST_DO_NOT_ADD BCM_VLAN_GPORT_ADD_UNKNOWN_MCAST_DO_NOT_ADD 
#define BCM_VLAN_PORT_BCAST_DO_NOT_ADD      BCM_VLAN_GPORT_ADD_BCAST_DO_NOT_ADD 

/* Flags for bcm_vlan_gport_extended_delete. */
#define BCM_VLAN_GPORT_UNKNOWN_UCAST_DO_NOT_UPDATE BCM_VLAN_GPORT_ADD_UNKNOWN_UCAST_DO_NOT_ADD 
#define BCM_VLAN_GPORT_UNKNOWN_MCAST_DO_NOT_UPDATE BCM_VLAN_GPORT_ADD_UNKNOWN_MCAST_DO_NOT_ADD 
#define BCM_VLAN_GPORT_BCAST_DO_NOT_UPDATE  BCM_VLAN_GPORT_ADD_BCAST_DO_NOT_ADD 

#ifndef BCM_HIDE_DISPATCHABLE

/* Add a virtual or physical port to the specified VLAN. */
extern int bcm_vlan_gport_add(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_gport_t port, 
    int flags);

/* Remove a virtual or physical port from the specified VLAN. */
extern int bcm_vlan_gport_delete(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_gport_t port);

/* Remove a virtual or physical port from the specified VLAN. */
extern int bcm_vlan_gport_extended_delete(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_gport_t port, 
    int flags);

/* Removes all virtual and physical port from the specified VLAN. */
extern int bcm_vlan_gport_delete_all(
    int unit, 
    bcm_vlan_t vlan);

/* Get a virtual or physical port from the specified VLAN. */
extern int bcm_vlan_gport_get(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_gport_t port, 
    int *flags);

/* Get all virtual and physical ports from the specified VLAN. */
extern int bcm_vlan_gport_get_all(
    int unit, 
    bcm_vlan_t vlan, 
    int array_max, 
    bcm_gport_t *gport_array, 
    int *flags_array, 
    int *array_size);

/* 
 * Returns an array of defined VLANs and their port bitmaps. If by pbmp,
 * then only VLANs which contain at least one of the specified ports are
 * listed.
 */
extern int bcm_vlan_list(
    int unit, 
    bcm_vlan_data_t **listp, 
    int *countp);

/* 
 * Returns an array of defined VLANs and their port bitmaps. If by pbmp,
 * then only VLANs which contain at least one of the specified ports are
 * listed.
 */
extern int bcm_vlan_list_by_pbmp(
    int unit, 
    bcm_pbmp_t ports, 
    bcm_vlan_data_t **listp, 
    int *countp);

/* Destroy a list returned by bcm_vlan_list. */
extern int bcm_vlan_list_destroy(
    int unit, 
    bcm_vlan_data_t *list, 
    int count);

/* Get the default VLAN ID. */
extern int bcm_vlan_default_get(
    int unit, 
    bcm_vlan_t *vid_ptr);

/* Set the default VLAN ID. */
extern int bcm_vlan_default_set(
    int unit, 
    bcm_vlan_t vid);

/* Retrieve the VTABLE STG for the specified VLAN. */
extern int bcm_vlan_stg_get(
    int unit, 
    bcm_vlan_t vid, 
    bcm_stg_t *stg_ptr);

/* Update the VTABLE STG for the specified VLAN. */
extern int bcm_vlan_stg_set(
    int unit, 
    bcm_vlan_t vid, 
    bcm_stg_t stg);

/* 
 * Set/get the spanning tree state for a port in the whole spanning
 * tree group that contains the specified VLAN.
 */
extern int bcm_vlan_stp_get(
    int unit, 
    bcm_vlan_t vid, 
    bcm_port_t port, 
    int *stp_state);

/* 
 * Set/get the spanning tree state for a port in the whole spanning
 * tree group that contains the specified VLAN.
 */
extern int bcm_vlan_stp_set(
    int unit, 
    bcm_vlan_t vid, 
    bcm_port_t port, 
    int stp_state);

/* Set spanning tree state for a port over a VLAN vector. */
extern int bcm_vlan_vector_stp_set(
    int unit, 
    bcm_vlan_vector_t vlan_vector, 
    bcm_port_t port, 
    int stp_state);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a VLAN tag action set structure. */
extern void bcm_vlan_action_set_t_init(
    bcm_vlan_action_set_t *action);

#ifndef BCM_HIDE_DISPATCHABLE

/* Get or set the port's default VLAN tag actions. */
extern int bcm_vlan_port_default_action_get(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_action_set_t *action);

/* Get or set the port's default VLAN tag actions. */
extern int bcm_vlan_port_default_action_set(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_action_set_t *action);

/* Deletes the port's default VLAN tag actions. */
extern int bcm_vlan_port_default_action_delete(
    int unit, 
    bcm_port_t port);

/* Get or set the egress default VLAN tag actions. */
extern int bcm_vlan_port_egress_default_action_get(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_action_set_t *action);

/* Get or set the egress default VLAN tag actions. */
extern int bcm_vlan_port_egress_default_action_set(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_action_set_t *action);

/* Deletes the egress default VLAN tag actions. */
extern int bcm_vlan_port_egress_default_action_delete(
    int unit, 
    bcm_port_t port);

/* 
 * Add protocol-based VLAN with specified action. If the entry already
 * exists, update the action.
 */
extern int bcm_vlan_port_protocol_action_add(
    int unit, 
    bcm_port_t port, 
    bcm_port_frametype_t frame, 
    bcm_port_ethertype_t ether, 
    bcm_vlan_action_set_t *action);

/* Get protocol-based VLAN specified action. */
extern int bcm_vlan_port_protocol_action_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_frametype_t frame, 
    bcm_port_ethertype_t ether, 
    bcm_vlan_action_set_t *action);

/* Delete protocol-based VLAN action. */
extern int bcm_vlan_port_protocol_action_delete(
    int unit, 
    bcm_port_t port, 
    bcm_port_frametype_t frame, 
    bcm_port_ethertype_t ether);

/* Delete all protocol-based VLAN actions. */
extern int bcm_vlan_port_protocol_action_delete_all(
    int unit, 
    bcm_port_t port);

#endif /* BCM_HIDE_DISPATCHABLE */

/* vlan_port_protocol_action_traverse_cb */
typedef int (*bcm_vlan_port_protocol_action_traverse_cb)(
    int unit, 
    bcm_port_t port, 
    bcm_port_frametype_t frame, 
    bcm_port_ethertype_t ether, 
    bcm_vlan_action_set_t *action, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Iterates over all protocol-based VLAN actions and executes a given
 * callback function.
 */
extern int bcm_vlan_port_protocol_action_traverse(
    int unit, 
    bcm_vlan_port_protocol_action_traverse_cb cb, 
    void *user_data);

/* 
 * Add an association from MAC address to VLAN to use for assigning a
 * VLAN tag
 * to untagged packets.
 */
extern int bcm_vlan_mac_add(
    int unit, 
    bcm_mac_t mac, 
    bcm_vlan_t vid, 
    int prio);

/* Remove an association from MAC address to VLAN. */
extern int bcm_vlan_mac_delete(
    int unit, 
    bcm_mac_t mac);

/* Remove all associations from MAC addresses to VLAN. */
extern int bcm_vlan_mac_delete_all(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* vlan_mac_action_traverse_cb */
typedef int (*bcm_vlan_mac_action_traverse_cb)(
    int unit, 
    bcm_mac_t mac, 
    bcm_vlan_action_set_t *action, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Traverse over VLAN MAC actions, which are used for VLAN
 * tag/s assignment to untagged packets.
 */
extern int bcm_vlan_mac_action_traverse(
    int unit, 
    bcm_vlan_mac_action_traverse_cb cb, 
    void *user_data);

/* 
 * Add an association from MAC address to VLAN actions to use for
 * assigning VLAN tag actions to untagged packets.
 */
extern int bcm_vlan_mac_action_add(
    int unit, 
    bcm_mac_t mac, 
    bcm_vlan_action_set_t *action);

/* 
 * Retrieve an association from MAC address to VLAN actions, which
 * are used for VLAN tag assignment to untagged packets.
 */
extern int bcm_vlan_mac_action_get(
    int unit, 
    bcm_mac_t mac, 
    bcm_vlan_action_set_t *action);

/* 
 * Remove an association from MAC address to VLAN actions, which are used
 * for VLAN tag assignment to untagged packets.
 */
extern int bcm_vlan_mac_action_delete(
    int unit, 
    bcm_mac_t mac);

/* 
 * Remove all MAC addresses to VLAN actions associations.
 * MAC-to-VLAN actions are used for VLAN tag assignment to untagged
 * packets.
 */
extern int bcm_vlan_mac_action_delete_all(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * VLAN translation selection
 * 
 * For chips supporting VLAN translation on a HiGig port, use the
 * following to construct a value to pass to
 * bcm_vlan_translate_add/delete routines as "port."
 */
#define BCM_VLAN_TRANSLATE_PORTMOD_MOD_SHIFT 16         
#define BCM_VLAN_TRANSLATE_PORTMOD_PORT_MASK \
    ((1 << BCM_VLAN_TRANSLATE_PORTMOD_MOD_SHIFT) - 1) 
#define BCM_VLAN_TRANSLATE_PORTMOD(port, mod_id)  \
    ((port) | (((mod_id) + 1) << BCM_VLAN_TRANSLATE_PORTMOD_MOD_SHIFT)) 

#ifndef BCM_HIDE_DISPATCHABLE

/* Add an entry to the VLAN Translation table. */
extern int bcm_vlan_translate_add(
    int unit, 
    int port, 
    bcm_vlan_t old_vid, 
    bcm_vlan_t new_vid, 
    int prio);

/* Get the VLAN translation for a given port and source VLAN ID. */
extern int bcm_vlan_translate_get(
    int unit, 
    int port, 
    bcm_vlan_t old_vid, 
    bcm_vlan_t *new_vid, 
    int *prio);

/* Delete an entry or entries from the VLAN Translation table. */
extern int bcm_vlan_translate_delete(
    int unit, 
    int port, 
    bcm_vlan_t old_vid);

/* Remove all entries from the VLAN Translation table. */
extern int bcm_vlan_translate_delete_all(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Key types for vlan translation lookup. */
typedef enum bcm_vlan_translate_key_e {
    bcmVlanTranslateKeyInvalid = 0,     /* Invalid Key Type. */
    bcmVlanTranslateKeyDouble = 1,      /* Use O-VID[11:0] and I-VID[11:0]. */
    bcmVlanTranslateKeyOuter = 2,       /* Use O-VID[11:0] */
    bcmVlanTranslateKeyInner = 3,       /* Use I-VID[11:0] */
    bcmVlanTranslateKeyOuterTag = 4,    /* Use O-TAG[15:0] */
    bcmVlanTranslateKeyInnerTag = 5,    /* Use I-TAG[15:0] */
    bcmVlanTranslateKeyOuterPri = 6,    /* Use (VLAN-PRI, VLAN-CFI =>
                                           O-TAG[15:12]) */
    bcmVlanTranslateKeyPortDouble = 7,  /* Use Port, O-VID[11:0] and
                                           I-VID[11:0]. */
    bcmVlanTranslateKeyPortOuter = 8,   /* Use Port, O-VID[11:0] */
    bcmVlanTranslateKeyPortInner = 9,   /* Use Port, I-VID[11:0] */
    bcmVlanTranslateKeyPortOuterTag = 10, /* Use Port, O-TAG[15:0] */
    bcmVlanTranslateKeyPortInnerTag = 11, /* Use Port, I-TAG[15:0] */
    bcmVlanTranslateKeyPortOuterPri = 12, /* Use Port, (VLAN-PRI, VLAN-CFI =>
                                           O-TAG[15:12]) */
    bcmVlanTranslateKeyDoubleVsan = 13, /* Use I-VID[11:0], O-VID[11:0] and
                                           VSAN-ID[11:0] */
    bcmVlanTranslateKeyInnerVsan = 14,  /* Use I-VID[11:0] and VSAN-ID[11:0] */
    bcmVlanTranslateKeyOuterVsan = 15,  /* Use O-VID[11:0] and VSAN-ID[11:0] */
    bcmVlanTranslateKeyPortPonTunnel = 16, /* Use Port,  LLVID[11:0] */
    bcmVlanTranslateKeyPortPonTunnelOuter = 17, /* Use Port,  LLVID[11:0] and
                                           O-VID[11:0] */
    bcmVlanTranslateKeyPortPonTunnelInner = 18, /* Use  Port LLVID[11:0] and I-VID[11:0] */
    bcmVlanTranslateKeyCapwapPayloadDouble = 19, /* Use O-VID[11:0] and I-VID[11:0] in
                                           CAPWAP wireless payload */
    bcmVlanTranslateKeyCapwapPayloadOuter = 20, /* Use O-VID[11:0] in CAPWAP wireless
                                           payload */
    bcmVlanTranslateKeyCapwapPayloadInner = 21, /* Use I-VID[11:0] in CAPWAP wireless
                                           payload */
    bcmVlanTranslateKeyCapwapPayloadOuterTag = 22, /* Use O-TAG[15:0] in CAPWAP wireless
                                           payload */
    bcmVlanTranslateKeyCapwapPayloadInnerTag = 23, /* Use I-TAG[15:0] in CAPWAP wireless
                                           payload */
    bcmVlanTranslateKeyPortCapwapPayloadDouble = 24, /* Use port, O-VID[11:0] and I-VID[11:0]
                                           in CAPWAP wireless payload */
    bcmVlanTranslateKeyPortCapwapPayloadOuter = 25, /* Use Port, O-VID[11:0] in CAPWAP
                                           wireless payload */
    bcmVlanTranslateKeyPortCapwapPayloadInner = 26, /* Use Port, I-VID[11:0] in CAPWAP
                                           wireless payload */
    bcmVlanTranslateKeyPortCapwapPayloadOuterTag = 27, /* Use Port, O-TAG[15:0] in CAPWAP
                                           wireless payload */
    bcmVlanTranslateKeyPortCapwapPayloadInnerTag = 28, /* Use Port, I-TAG[15:0] in CAPWAP
                                           wireless payload */
    bcmVlanTranslateKeyPortGroupDouble = 29, /* Use Port-Group, O-VID[11:0] and
                                           I-VID[11:0] */
    bcmVlanTranslateKeyPortGroupOuter = 30, /* Use Port-Group, O-VID[11:0] */
    bcmVlanTranslateKeyPortGroupInner = 31, /* Use Port-Group, I-VID[11:0] */
    bcmVlanTranslateKeyPortGroupOuterTag = 32, /* Use Port-Group, O-TAG[15:0] */
    bcmVlanTranslateKeyPortGroupInnerTag = 33, /* Use Port-Group, I-TAG[15:0] */
    bcmVlanTranslateKeyPortGroupOuterPri = 34, /* Use Port-Group, (VLAN-PRI, VLAN-CFI
                                           => O-TAG[15:12]) */
    bcmVlanTranslateKeyVxlanVnid = 35,  /* Use VNID[23:0] in VXLAN header */
    bcmVlanTranslateKeyPortVxlanVnid = 36, /* Use Port and VNID[23:0] in VXLAN
                                           header */
    bcmVlanTranslateKeyMacPort = 37,    /* Use Mac Address[47:0] and Port */
    bcmVlanTranslateKeyMacVirtualPort = 38, /* Use Mac Address[47:0] + Port */
    bcmVlanTranslateKeyMacPortGroup = 39, /* Use Mac Address[47:0] and
                                           Port-Group[11:0] */
    bcmVlanTranslateEgressKeyVxlanSubChannel = 40 /* Use DGPP, O-VID[11:0] and I-VID[11:0]
                                           for VxLAN encap. */
} bcm_vlan_translate_key_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Add an entry to the VLAN Translation table and assign VLAN actions. */
extern int bcm_vlan_translate_action_add(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_action_set_t *action);

/* Add an entry to the VLAN Translation table and assign VLAN actions. */
extern int bcm_vlan_translate_action_create(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_action_set_t *action);

/* 
 * Get the assigned VLAN actions for the given port, key type, and VLAN
 * tags.
 */
extern int bcm_vlan_translate_action_get(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_action_set_t *action);

/* Delete an entry from the VLAN Translation table. */
extern int bcm_vlan_translate_action_delete(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan);

/* Add an entry to the egress VLAN Translation table. */
extern int bcm_vlan_translate_egress_add(
    int unit, 
    int port, 
    bcm_vlan_t old_vid, 
    bcm_vlan_t new_vid, 
    int prio);

/* 
 * Get the VLAN egress translation for the specified port and source VLAN
 * ID.
 */
extern int bcm_vlan_translate_egress_get(
    int unit, 
    int port, 
    bcm_vlan_t old_vid, 
    bcm_vlan_t *new_vid, 
    int *prio);

/* Remove an entry or entries from the VLAN egress Translation table. */
extern int bcm_vlan_translate_egress_delete(
    int unit, 
    int port, 
    bcm_vlan_t old_vid);

/* Remove all entries from the egress VLAN Translation table. */
extern int bcm_vlan_translate_egress_delete_all(
    int unit);

/* 
 * Add an entry to the egress VLAN Translation table and assign VLAN
 * actions.
 */
extern int bcm_vlan_translate_egress_action_add(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_action_set_t *action);

/* 
 * Get the assigned VLAN actions for egress VLAN translation on the given
 * port class and VLAN tags.
 */
extern int bcm_vlan_translate_egress_action_get(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_action_set_t *action);

/* Delete an entry from the egress VLAN Translation table. */
extern int bcm_vlan_translate_egress_action_delete(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan);

/* 
 * Add an entry based on gport to the egress VLAN Translation table and
 * assign VLAN actions.
 */
extern int bcm_vlan_translate_egress_gport_action_add(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_action_set_t *action);

/* 
 * Get the assigned VLAN actions for egress VLAN translation on the given
 * gport and VLAN tags.
 */
extern int bcm_vlan_translate_egress_gport_action_get(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_action_set_t *action);

/* Delete a gport based entry from the egress VLAN Translation table. */
extern int bcm_vlan_translate_egress_gport_action_delete(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan);

/* Add an entry to the VLAN Translation table for double-tagging. */
extern int bcm_vlan_dtag_add(
    int unit, 
    int port, 
    bcm_vlan_t old_vid, 
    bcm_vlan_t new_vid, 
    int prio);

/* 
 * Get the VLAN translation for double tagging on a given port and source
 * VLAN ID.
 */
extern int bcm_vlan_dtag_get(
    int unit, 
    int port, 
    bcm_vlan_t old_vid, 
    bcm_vlan_t *new_vid, 
    int *prio);

/* Remove an entry from the VLAN Translation table for double-tagging. */
extern int bcm_vlan_dtag_delete(
    int unit, 
    int port, 
    bcm_vlan_t old_vid);

/* Remove all entries from the VLAN Translation table for double-tagging. */
extern int bcm_vlan_dtag_delete_all(
    int unit);

/* Add a VLAN range to the VLAN Translation table. */
extern int bcm_vlan_translate_range_add(
    int unit, 
    int port, 
    bcm_vlan_t old_vid_low, 
    bcm_vlan_t old_vid_high, 
    bcm_vlan_t new_vid, 
    int int_prio);

/* Get the VLAN translation for a given port and VLAN range. */
extern int bcm_vlan_translate_range_get(
    int unit, 
    int port, 
    bcm_vlan_t old_vid_low, 
    bcm_vlan_t old_vid_high, 
    bcm_vlan_t *new_vid, 
    int *int_prio);

/* Delete a VLAN range from the VLAN Translation table. */
extern int bcm_vlan_translate_range_delete(
    int unit, 
    int port, 
    bcm_vlan_t old_vid_low, 
    bcm_vlan_t old_vid_high);

/* Remove all VLAN ranges from the VLAN Translation table. */
extern int bcm_vlan_translate_range_delete_all(
    int unit);

/* Add VLAN range to the VLAN Translation table for double-tagging. */
extern int bcm_vlan_dtag_range_add(
    int unit, 
    int port, 
    bcm_vlan_t old_vid_low, 
    bcm_vlan_t old_vid_high, 
    bcm_vlan_t new_vid, 
    int int_prio);

/* 
 * Get the VLAN translation for double tagging on a given port
 * and VLAN range.
 */
extern int bcm_vlan_dtag_range_get(
    int unit, 
    int port, 
    bcm_vlan_t old_vid_low, 
    bcm_vlan_t old_vid_high, 
    bcm_vlan_t *new_vid, 
    int *prio);

/* 
 * Remove a VLAN range from the VLAN Translation table for
 * double-tagging.
 */
extern int bcm_vlan_dtag_range_delete(
    int unit, 
    int port, 
    bcm_vlan_t old_vid_low, 
    bcm_vlan_t old_vid_high);

/* 
 * Remove all VLAN ranges from the VLAN Translation table for
 * double-tagging.
 */
extern int bcm_vlan_dtag_range_delete_all(
    int unit);

/* 
 * Add an entry to the VLAN Translation table, which assigns VLAN actions
 * for packets matching within the VLAN range(s).
 */
extern int bcm_vlan_translate_action_range_add(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_t outer_vlan_low, 
    bcm_vlan_t outer_vlan_high, 
    bcm_vlan_t inner_vlan_low, 
    bcm_vlan_t inner_vlan_high, 
    bcm_vlan_action_set_t *action);

/* 
 * Get assigned VLAN actions from VLAN Translation table for the
 * specified VLAN range(s).
 */
extern int bcm_vlan_translate_action_range_get(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_t outer_vlan_low, 
    bcm_vlan_t outer_vlan_high, 
    bcm_vlan_t inner_vlan_low, 
    bcm_vlan_t inner_vlan_high, 
    bcm_vlan_action_set_t *action);

/* 
 * Delete an entry from the VLAN Translation table for the specified VLAN
 * range(s).
 */
extern int bcm_vlan_translate_action_range_delete(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_t outer_vlan_low, 
    bcm_vlan_t outer_vlan_high, 
    bcm_vlan_t inner_vlan_low, 
    bcm_vlan_t inner_vlan_high);

/* Delete all VLAN range entries from the VLAN Translation table. */
extern int bcm_vlan_translate_action_range_delete_all(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* vlan_translate_egress_action_traverse_cb */
typedef int (*bcm_vlan_translate_egress_action_traverse_cb)(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_action_set_t *action, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Iterate over VLAN egress translate table and calls user-provided
 * callback for every valid entry.
 */
extern int bcm_vlan_translate_egress_action_traverse(
    int unit, 
    bcm_vlan_translate_egress_action_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* vlan_translate_action_traverse_cb */
typedef int (*bcm_vlan_translate_action_traverse_cb)(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_action_set_t *action, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Iterate over VLAN translation table and calls user-provided callback
 * for every valid entry.
 */
extern int bcm_vlan_translate_action_traverse(
    int unit, 
    bcm_vlan_translate_action_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* vlan_translate_traverse_cb */
typedef int (*bcm_vlan_translate_traverse_cb)(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_t old_vlan, 
    bcm_vlan_t new_vlan, 
    int prio, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Iterate over VLAN translation table and calls user-provided callback
 * for every valid entry.
 */
extern int bcm_vlan_translate_traverse(
    int unit, 
    bcm_vlan_translate_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* vlan_translate_egress_traverse_cb */
typedef int (*bcm_vlan_translate_egress_traverse_cb)(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_t old_vlan, 
    bcm_vlan_t new_vlan, 
    int prio, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Iterate over VLAN egress translation table and calls user-provided
 * callback for every valid entry.
 */
extern int bcm_vlan_translate_egress_traverse(
    int unit, 
    bcm_vlan_translate_egress_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* vlan_dtag_traverse_cb */
typedef int (*bcm_vlan_dtag_traverse_cb)(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_t old_vlan, 
    bcm_vlan_t new_vlan, 
    int prio, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Iterate over VLAN double tagging table and calls user-provided
 * callback for every valid entry.
 */
extern int bcm_vlan_dtag_traverse(
    int unit, 
    bcm_vlan_dtag_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* vlan_translate_action_range_traverse_cb */
typedef int (*bcm_vlan_translate_action_range_traverse_cb)(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_t outer_vlan_low, 
    bcm_vlan_t outer_vlan_high, 
    bcm_vlan_t inner_vlan_low, 
    bcm_vlan_t inner_vlan_high, 
    bcm_vlan_action_set_t *action, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Iterate over VLAN ranges table and calls user-provided callback for
 * every valid range.
 */
extern int bcm_vlan_translate_action_range_traverse(
    int unit, 
    bcm_vlan_translate_action_range_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_vlan_translate_range_traverse_cb */
typedef int (*bcm_vlan_translate_range_traverse_cb)(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_t old_vlan_low, 
    bcm_vlan_t old_vlan_high, 
    bcm_vlan_t new_vlan, 
    int prio, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Iterate over VLAN ranges table and calls user-provided callback for
 * every valid range.
 */
extern int bcm_vlan_translate_range_traverse(
    int unit, 
    bcm_vlan_translate_range_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_vlan_dtag_range_traverse_cb */
typedef int (*bcm_vlan_dtag_range_traverse_cb)(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_t old_vlan_low, 
    bcm_vlan_t old_vlan_high, 
    bcm_vlan_t new_vlan, 
    int prio, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Iterate over VLAN double tagging range table and calls user-provided
 * callback for every valid range.
 */
extern int bcm_vlan_dtag_range_traverse(
    int unit, 
    bcm_vlan_dtag_range_traverse_cb cb, 
    void *user_data);

/* Create a VLAN translation ID instance. */
extern int bcm_vlan_translate_action_id_create(
    int unit, 
    uint32 flags, 
    int *action_id);

/* Destroy a VLAN translation ID instance. */
extern int bcm_vlan_translate_action_id_destroy(
    int unit, 
    uint32 flags, 
    int action_id);

/* Destroy all VLAN translation ID instances. */
extern int bcm_vlan_translate_action_id_destroy_all(
    int unit, 
    uint32 flags);

/* Set a VLAN translation ID instance with tag actions. */
extern int bcm_vlan_translate_action_id_set(
    int unit, 
    uint32 flags, 
    int action_id, 
    bcm_vlan_action_set_t *action);

/* Get tag actions from a VLAN translation ID instance. */
extern int bcm_vlan_translate_action_id_get(
    int unit, 
    uint32 flags, 
    int action_id, 
    bcm_vlan_action_set_t *action);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_vlan_translate_action_class_s */
typedef struct bcm_vlan_translate_action_class_s {
    uint32 flags;                       /* BCM_VLAN_ACTION_SET_XXX */
    uint32 vlan_edit_class_id;          /* VLAN edit class ID. See
                                           bcm_vlan_port_translation_set */
    bcm_port_tag_format_class_t tag_format_class_id; /* VLAN-tag format ID */
    int vlan_translation_action_id;     /* Action ID */
} bcm_vlan_translate_action_class_t;

/* 
 * Initialize a structure that holds both the key and the info for
 * translate action class configuration.
 */
extern void bcm_vlan_translate_action_class_t_init(
    bcm_vlan_translate_action_class_t *action_class);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Set an action ID for a specified combination of packet format ID and
 * VLAN translation profile.
 */
extern int bcm_vlan_translate_action_class_set(
    int unit, 
    bcm_vlan_translate_action_class_t *action_class);

/* 
 * Get the action ID that is configured for a specified combination of
 * packet format ID and VLAN edit profile.
 */
extern int bcm_vlan_translate_action_class_get(
    int unit, 
    bcm_vlan_translate_action_class_t *action_class);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flags for the unified IPv4/IPv6 bcm_vlan_ip_t type. */
#define BCM_VLAN_SUBNET_IP6     (1 << 14)  

/* Unified IPv4/IPv6 type. */
typedef struct bcm_vlan_ip_s {
    uint32 flags; 
    bcm_ip_t ip4; 
    bcm_ip_t mask; 
    bcm_ip6_t ip6; 
    int prefix; 
    bcm_vlan_t vid; 
    int prio; 
} bcm_vlan_ip_t;

/* Initialize the bcm_vlan_ip_t structure. */
extern void bcm_vlan_ip_t_init(
    bcm_vlan_ip_t *vlan_ip);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Add an association from IP subnet to VLAN to use for assigning a VLAN
 * tag to untagged packets.
 */
extern int bcm_vlan_ip_add(
    int unit, 
    bcm_vlan_ip_t *vlan_ip);

/* Remove an association from IP subnet to VLAN. */
extern int bcm_vlan_ip_delete(
    int unit, 
    bcm_vlan_ip_t *vlan_ip);

/* Remove all associations from IP subnet to VLAN. */
extern int bcm_vlan_ip_delete_all(
    int unit);

/* 
 * Add an association from IP subnet to VLAN actions to use for assigning
 * VLAN tag actions to untagged packets.
 */
extern int bcm_vlan_ip_action_add(
    int unit, 
    bcm_vlan_ip_t *vlan_ip, 
    bcm_vlan_action_set_t *action);

/* 
 * Get an association from IP subnet to VLAN actions that used for
 * assigning VLAN tag actions to untagged packets.
 */
extern int bcm_vlan_ip_action_get(
    int unit, 
    bcm_vlan_ip_t *vlan_ip, 
    bcm_vlan_action_set_t *action);

/* 
 * Deletes an association from IP subnet to VLAN actions that used for
 * assigning VLAN tag actions to untagged packets.
 */
extern int bcm_vlan_ip_action_delete(
    int unit, 
    bcm_vlan_ip_t *vlan_ip);

/* Deletes all associations from IP subnet to VLAN actions */
extern int bcm_vlan_ip_action_delete_all(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* vlan_ip_action_traverse_cb */
typedef int (*bcm_vlan_ip_action_traverse_cb)(
    int unit, 
    bcm_vlan_ip_t *vlan_ip, 
    bcm_vlan_action_set_t *action, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Iterates over all associations from IP subnet to VLAN actions and
 * executes provided callback function.
 */
extern int bcm_vlan_ip_action_traverse(
    int unit, 
    bcm_vlan_ip_action_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_vlan_control_t */
typedef enum bcm_vlan_control_e {
    bcmVlanDropUnknown = 0,             /* Drop unknown/FFF VLAN pkts or send to
                                           CPU. */
    bcmVlanPreferIP4 = 1,               /* Prefer IP Subnet VLAN selection. */
    bcmVlanPreferMAC = 2,               /* Prefer MAC VLAN selection. */
    bcmVlanShared = 3,                  /* Shared vs. Independent VLAN Learning. */
    bcmVlanSharedID = 4,                /* Shared Learning VLAN ID. */
    bcmVlanTranslate = 5,               /* Chip is in VLAN translate mode. */
    bcmVlanPreferEgressTranslate = 7,   /* Do egress translation even if ingress
                                           FP changes the outer/inner VLAN
                                           tag(s). */
    bcmVlanIndependentStp = 9,          /* Directly set port,vlan stp state,
                                           igoring STG in bcm_vlan_stp_set(). */
    bcmVlanMemberMismatchLearn = 20,    /* Packets' incoming port is not the
                                           member of the VLAN are learned when
                                           set to 1. */
    bcmVlanMemberMismatchToCpu = 21,    /* Packets' incoming port is not the
                                           member of the VLAN are sent to CPU
                                           when set to 1. */
    bcmVlanNativeVplsServiceTaggedMiss = 22, /* Configure Action for Native-AC lookup
                                           miss in the case of VPLS Service
                                           tagged. */
    bcmVlanControlTranslateIngressMissDrop = 23, /* Packet drop on ingress VLAN
                                           translation miss. */
    bcmVlanControlTranslateIngressMissCopyToCpu = 24, /* Packet copy to CPU on ingress VLAN
                                           translation miss. */
    bcmVlanControlTranslateEgressMissDrop = 25, /* Packet drop on egress VLAN
                                           translation miss. */
    bcmVlanControlCount = 26            /*  Always last. */
} bcm_vlan_control_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/get miscellaneous VLAN-specific chip options. */
extern int bcm_vlan_control_get(
    int unit, 
    bcm_vlan_control_t type, 
    int *arg);

/* Set/get miscellaneous VLAN-specific chip options. */
extern int bcm_vlan_control_set(
    int unit, 
    bcm_vlan_control_t type, 
    int arg);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Key types for egress vlan translation lookup. */
typedef enum bcm_vlan_translate_egress_key_e {
    bcmVlanTranslateEgressKeyInvalid = 0, /* Invalid Key Type. */
    bcmVlanTranslateEgressKeyVpn = 1,   /* Use VPN ID. */
    bcmVlanTranslateEgressKeyVpnGport = 2, /* Use VPN ID and GPORT. */
    bcmVlanTranslateEgressKeyVpnGportGroup = 3, /* Use VPN ID and GPORT group. */
    bcmVlanTranslateEgressKeyVlanPort = 4, /* Use VLAN and PORT. */
    bcmVlanTranslateEgressKeyPortGroupDouble = 5, /* Use port-group ID, OVID, IVID. */
    bcmVlanTranslateEgressKeyPortDouble = 6, /* Use port, OVID, IVID. */
    bcmVlanTranslateEgressKeyVlanGport = 7, /* Use VLAN and GPORT. */
    bcmVlanTranslateEgressKeyVpnPortGroup = 8, /* Use VPN ID and port-group ID. */
    bcmVlanTranslateEgressKeySrcGport = 9, /* Use source GPORT. */
    bcmVlanTranslateEgressKeyCount = 10 /* Egress VLAN translation key count.
                                           Always the last. */
} bcm_vlan_translate_egress_key_t;

/* bcm_vlan_control_port_t */
typedef enum bcm_vlan_control_port_e {
    bcmVlanPortPreferIP4 = 0, 
    bcmVlanPortPreferMAC = 1, 
    bcmVlanTranslateIngressEnable = 2, 
    bcmVlanTranslateIngressHitDrop = 3, 
    bcmVlanTranslateIngressMissDrop = 4, 
    bcmVlanTranslateEgressEnable = 5, 
    bcmVlanTranslateEgressMissDrop = 6, 
    bcmVlanTranslateEgressMissUntaggedDrop = 7, 
    bcmVlanLookupMACEnable = 8, 
    bcmVlanLookupIPEnable = 9, 
    bcmVlanPortUseInnerPri = 10, 
    bcmVlanPortUseOuterPri = 11, 
    bcmVlanPortVerifyOuterTpid = 12, 
    bcmVlanPortOuterTpidSelect = 13, 
    bcmVlanPortTranslateKeyFirst = 14, 
    bcmVlanPortTranslateKeySecond = 15, 
    bcmVlanTranslateEgressMissUntag = 16, 
    bcmVlanPortIgnorePktTag = 17, 
    bcmVlanPortUntaggedDrop = 18, 
    bcmVlanPortPriTaggedDrop = 19, 
    bcmVlanPortDoubleLookupEnable = 20, 
    bcmVlanPortLookupTunnelEnable = 21, 
    bcmVlanPortIgnoreInnerPktTag = 22, 
    bcmVlanPortJoinAllVlan = 23, 
    bcmVlanPortOamUseXlatedInnerVlan = 24, 
    bcmVlanPortTranslateEgressKey = 25, 
    bcmVlanPortTranslateEgressKeyFirst = 26, 
    bcmVlanPortTranslateEgressKeySecond = 27, 
    bcmVlanPortPayloadTagsDelete = 28, 
    bcmVlanPortTranslateEgressVnidEnable = 29, 
    bcmVlanPortTranslateEgressL2TunnelEnable = 30, /* Enable egress adaptation for L2
                                           tunnel decapsulation flow. */
    bcmVlanPortTranslateEgressL2TunnelMissDrop = 31 /* Enable to drop packets if egress
                                           adaptation lookup misses for L2
                                           tunnel decapsulation flow. */
} bcm_vlan_control_port_t;

/* Selectors for bcmVlanPortOuterTpidSelect */
#define BCM_PORT_OUTER_TPID     0          /* Use egress port TPID as outer
                                              TPID. */
#define BCM_VLAN_OUTER_TPID     1          /* Use VLAN TPID as outer TPID. */

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/get miscellaneous port-specific VLAN options. */
extern int bcm_vlan_control_port_get(
    int unit, 
    int port, 
    bcm_vlan_control_port_t type, 
    int *arg);

/* Set/get miscellaneous port-specific VLAN options. */
extern int bcm_vlan_control_port_set(
    int unit, 
    int port, 
    bcm_vlan_control_port_t type, 
    int arg);

#endif /* BCM_HIDE_DISPATCHABLE */

/* VLAN multicast flood modes. */
typedef enum bcm_vlan_mcast_flood_e {
    BCM_VLAN_MCAST_FLOOD_ALL = _SHR_PORT_MCAST_FLOOD_ALL, 
    BCM_VLAN_MCAST_FLOOD_UNKNOWN = _SHR_PORT_MCAST_FLOOD_UNKNOWN, 
    BCM_VLAN_MCAST_FLOOD_NONE = _SHR_PORT_MCAST_FLOOD_NONE, 
    BCM_VLAN_MCAST_FLOOD_COUNT = _SHR_PORT_MCAST_FLOOD_COUNT 
} bcm_vlan_mcast_flood_t;

#define BCM_VLAN_MCAST_FLOOD_STR \
{ \
    "MCAST_FLOOD_ALL", \
    "MCAST_FLOOD_UNKNOWN", \
    "MCAST_FLOOD_NONE", \
    ""  \
}

#ifndef BCM_HIDE_DISPATCHABLE

/* Set or retrieve the current VLAN multicast flood behavior. */
extern int bcm_vlan_mcast_flood_get(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_vlan_mcast_flood_t *mode);

/* Set or retrieve the current VLAN multicast flood behavior. */
extern int bcm_vlan_mcast_flood_set(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_vlan_mcast_flood_t mode);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Per VLAN forwarding behavior. */
typedef enum bcm_vlan_forward_e {
    bcmVlanForwardBridging = 0,         /* Switching based on MAC and VLAN. */
    bcmVlanForwardSingleCrossConnect = 1, /* Switching based on outer VLAN. */
    bcmVlanForwardDoubleCrossConnect = 2 /* Switching based on outer+inner VLAN. */
} bcm_vlan_forward_t;

/* Per VLAN URPF Mode setting. */
typedef enum bcm_vlan_urpf_mode_e {
    bcmVlanUrpfDisable = 0, /* Disable unicast RPF. */
    bcmVlanUrpfLoose = 1,   /* Loose mode Unicast RPF. */
    bcmVlanUrpfStrict = 2   /* Strict mode Unicast RPF. */
} bcm_vlan_urpf_mode_t;

/* Per VLAN VP Mode control. */
typedef enum bcm_vlan_vp_mc_ctrl_e {
    bcmVlanVPMcControlAuto = 0,     /* VP Multicast replication auto control */
    bcmVlanVPMcControlEnable = 1,   /* Enable VP Multicast replication */
    bcmVlanVPMcControlDisable = 2   /* Disable VP Multicast replication */
} bcm_vlan_vp_mc_ctrl_t;

#define BCM_VLAN_PROTO_PKT_TOCPU_ENABLE     0x00000001 
#define BCM_VLAN_PROTO_PKT_FORWARD_ENABLE   0x00000002 
#define BCM_VLAN_PROTO_PKT_DROP_ENABLE      0x00000004 
#define BCM_VLAN_PROTO_PKT_FLOOD_ENABLE     0x00000008 

/* 
 * Per VLAN Protocol Packet control. A protocol packet type is copied to
 * CPU if the packet control value is set to VLAN_PROTO_PKT_TOCPU_ENABLE.
 * Additionally the control can be set to one of
 * VLAN_PROTO_PKT_FORWARD_ENABLE, VLAN_PROTO_PKT_DROP_ENABLE, or
 * VLAN_PROTO_PKT_FLOOD_ENABLE to forward, drop or flood in VLAN
 * respectively.
 */
typedef struct bcm_vlan_protocol_packet_ctrl_s {
    int mmrp_action;                    /* MMRP packet action */
    int srp_action;                     /* SRP packet action */
    int arp_reply_action;               /* ARP reply packet action */
    int arp_request_action;             /* ARP request packet action */
    int nd_action;                      /* ND packet action */
    int dhcp_action;                    /* DHCP packet action */
    int igmp_report_leave_action;       /* IGMP Report(v1,v2,v3)/Leave(v2)
                                           packet action */
    int igmp_query_action;              /* IGMP Query packet action */
    int igmp_unknown_msg_action;        /* Unknown IGMP message packet action */
    int mld_report_done_action;         /* MLD Report(v1,v2)/Done(v1) packet
                                           action */
    int mld_query_action;               /* MLD Query packet action */
    int ip4_rsvd_mc_action;             /* IPv4 reserved multicast packet
                                           (DIP=224.0.0.X) action */
    int ip6_rsvd_mc_action;             /* IPv6 reserved multicast packet
                                           (DIP=ff0X:0:0:0:0:0:0:0) action */
    int ip4_mc_router_adv_action;       /* IPv4 multicast router
                                           advertisement/solicitation/termination
                                           (ICMP types 151,152,153) packet
                                           action */
    int ip6_mc_router_adv_action;       /* IPv6 multicast router
                                           advertisement/solicitation/termination
                                           (ICMP types 151,152,153) packet
                                           action */
    int flood_action_according_to_pfm;  /* Use PFM rules for flooding
                                           IGMP/MLD/IPv4,IPv6 Reserved Multicast */
} bcm_vlan_protocol_packet_ctrl_t;

#define BCM_VLAN_LEARN_DISABLE              0x00000001 
#define BCM_VLAN_UNKNOWN_IP6_MCAST_TOCPU    0x00000002 
#define BCM_VLAN_UNKNOWN_IP4_MCAST_TOCPU    0x00000004 
#define BCM_VLAN_IP4_DISABLE                0x00000008 
#define BCM_VLAN_IP6_DISABLE                0x00000010 
#define BCM_VLAN_IP4_MCAST_DISABLE          0x00000020 
#define BCM_VLAN_IP6_MCAST_DISABLE          0x00000040 
#define BCM_VLAN_IP4_MCAST_L2_DISABLE       0x00000080 
#define BCM_VLAN_IP6_MCAST_L2_DISABLE       0x00000100 
#define BCM_VLAN_MPLS_DISABLE               0x00000200 
#define BCM_VLAN_COSQ_ENABLE                0x00000400 
#define BCM_VLAN_IGMP_SNOOP_DISABLE         0x00000800 
#define BCM_VLAN_PIM_SNOOP_DISABLE          0x00001000 
#define BCM_VLAN_USE_FABRIC_DISTRIBUTION    0x00002000 
#define BCM_VLAN_ICMP_REDIRECT_TOCPU        0x00004000 
#define BCM_VLAN_UNKNOWN_UCAST_TOCPU        0x00008000 
#define BCM_VLAN_UNKNOWN_UCAST_DROP         0x00010000 
#define BCM_VLAN_NON_UCAST_TOCPU            0x00020000 
#define BCM_VLAN_NON_UCAST_DROP             0x00040000 
#define BCM_VLAN_L2_LOOKUP_DISABLE          0x00080000 
#define BCM_VLAN_L3_VRF_GLOBAL_DISABLE      0x00100000 
#define BCM_VLAN_MIM_TERM_DISABLE           0x00200000 
#define BCM_VLAN_L2_CLASS_ID_ONLY           0x00400000 
#define BCM_VLAN_L3_URPF_DEFAULT_ROUTE_CHECK_DISABLE 0x00800000 
#define BCM_VLAN_TRILL_IGMP_SNOOP_DISABLE   0x01000000 
#define BCM_VLAN_POLICER_DISABLE            0x02000000 
#define BCM_VLAN_UNKNOWN_SMAC_DROP          0x04000000 
#define BCM_VLAN_IPMC_DO_VLAN_DISABLE       0x08000000 
#define BCM_VLAN_L3_CLASS_ID                0x10000000 
#define BCM_VLAN_IPMC4_UCAST_ENABLE         0x20000000 
#define BCM_VLAN_IPMC6_UCAST_ENABLE         0x40000000 
#define BCM_VLAN_SRC_DST_NAT_REALM_ID       0x80000000 

#define BCM_VLAN_FLAGS2_MEMBER_INGRESS_DISABLE 0x00000001 /* Disable ingress
                                                          membership check for
                                                          this VPN */
#define BCM_VLAN_FLAGS2_MEMBER_EGRESS_DISABLE 0x00000002 /* Disable egress
                                                          membership check for
                                                          this VPN */
#define BCM_VLAN_FLAGS2_IVL                 0x00000004 /* Configure VSI to use
                                                          IVL key format
                                                          (vid,vsi,mac) */
#define BCM_VLAN_FLAGS2_WIRED_COS_MAP_SELECT 0x00000008 /* Select the wired cos
                                                          map */
#define BCM_VLAN_FLAGS2_WIRELESS_COS_MAP_SELECT 0x00000010 /* Select the wireless
                                                          cos map */
#define BCM_VLAN_FLAGS2_UNKNOWN_DEST        0x00000020 /* Configure the default
                                                          destination with
                                                          unknown_dest */

/* Valid fields bitmask for vlan control structure. */
#define BCM_VLAN_CONTROL_VLAN_VRF_MASK      0x00000001 
#define BCM_VLAN_CONTROL_VLAN_FORWARDING_VLAN_MASK 0x00000002 
#define BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK 0x00000004 
#define BCM_VLAN_CONTROL_VLAN_OUTER_TPID_MASK 0x00000008 
#define BCM_VLAN_CONTROL_VLAN_IP6_MCAST_FLOOD_MODE_MASK 0x00000010 
#define BCM_VLAN_CONTROL_VLAN_IP4_MCAST_FLOOD_MODE_MASK 0x00000020 
#define BCM_VLAN_CONTROL_VLAN_L2_MCAST_FLOOD_MODE_MASK 0x00000040 
#define BCM_VLAN_CONTROL_VLAN_IF_CLASS_MASK 0x00000080 
#define BCM_VLAN_CONTROL_VLAN_FORWARDING_MODE_MASK 0x00000100 
#define BCM_VLAN_CONTROL_VLAN_URPF_MODE_MASK 0x00000200 
#define BCM_VLAN_CONTROL_VLAN_COSQ_MASK     0x00000400 
#define BCM_VLAN_CONTROL_VLAN_QOS_MAP_ID_MASK 0x00000800 
#define BCM_VLAN_CONTROL_VLAN_DISTRIBUTION_CLASS_MASK 0x00001000 
#define BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK 0x00002000 
#define BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK 0x00004000 
#define BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK 0x00008000 
#define BCM_VLAN_CONTROL_VLAN_TRILL_NONUNICAST_GROUP_MASK 0x00010000 
#define BCM_VLAN_CONTROL_VLAN_SOURCE_TRILL_NAME_MASK 0x00020000 
#define BCM_VLAN_CONTROL_VLAN_TRUNK_INDEX_MASK 0x00040000 
#define BCM_VLAN_CONTROL_VLAN_PROTOCOL_PKT_MASK 0x00080000 
#define BCM_VLAN_CONTROL_VLAN_NAT_REALM_ID_MASK 0x00100000 
#define BCM_VLAN_CONTROL_VLAN_L3_IF_CLASS_MASK 0x00200000 
#define BCM_VLAN_CONTROL_VLAN_VP_MCAST_MASK 0x00400000 
#define BCM_VLAN_CONTROL_VLAN_SR_FLAGS_MASK 0x00800000 
#define BCM_VLAN_CONTROL_VLAN_EGRESS_ACTION_MASK 0x01000000 
#define BCM_VLAN_CONTROL_VLAN_L2_VIEW_MASK  0x02000000 
#define BCM_VLAN_CONTROL_VLAN_EGRESS_CLASS_ID_MASK 0x04000000 
#define BCM_VLAN_CONTROL_VLAN_EGRESS_OPAQUE_CTRL_ID_MASK 0x08000000 
#define BCM_VLAN_CONTROL_VLAN_INGRESS_OPAQUE_CTRL_ID_MASK 0x10000000 

/* Enable all fields in vlan control structure. */
#define BCM_VLAN_CONTROL_VLAN_ALL_MASK  0xffffffff 

/* 
 * Flags for Vlan-based learn mode
 * 
 * This call takes flags to turn on and off mutually-independent actions
 * that should be taken when a packet is received with an unknown source
 * address, or source lookup failure (SLF) on a per-VLAN basis.
 * 
 * BCM_VLAN_LEARN_CONTROL_ENABLE: Enable the VLAN-based Learn will
 * override per-port learning (The default behavior when it is set: "Do
 * not hardware learning", "Do not copy to CPU", "Drop packet" and "No
 * pending learn").
 */
#define BCM_VLAN_LEARN_CONTROL_ENABLE   0x80000000 
#define BCM_VLAN_LEARN_CONTROL_ARL      0x01       
#define BCM_VLAN_LEARN_CONTROL_CPU      0x02       
#define BCM_VLAN_LEARN_CONTROL_FWD      0x04       
#define BCM_VLAN_LEARN_CONTROL_PENDING  0x08       

/* L2 table view per VLAN. */
typedef enum bcm_vlan_l2_view_e {
    bcmVlanL2ViewWide = 0,      /* L2 table wide view per VLAN. */
    bcmVlanL2ViewNarrow = 1,    /* L2 table narrow view per VLAN. */
    bcmVlanL2ViewCount = 2      /* Always last. */
} bcm_vlan_l2_view_t;

/* This structure contains the configuration of a VLAN. */
typedef struct bcm_vlan_control_vlan_s {
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
    bcm_vlan_action_set_t egress_action; /* Egress VLAN actions. */
    bcm_vlan_l2_view_t l2_view;         /* L2 table view. */
    int egress_class_id;                /* Class ID for EFP qualifier. */
    int egress_opaque_ctrl_id;          /* Egress opaque control ID. */
    int ingress_opaque_ctrl_id;         /* Ingress opaque control ID. */
    bcm_gport_t unknown_dest;           /* Gport for default destination, valid
                                           when BCM_VLAN_FLAGS2_UNKNOWN_DEST is
                                           set */
} bcm_vlan_control_vlan_t;

/* Initialize the bcm_vlan_control_vlan_t structure. */
extern void bcm_vlan_control_vlan_t_init(
    bcm_vlan_control_vlan_t *data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set or retrieve current VLAN properties selectively. */
extern int bcm_vlan_control_vlan_selective_get(
    int unit, 
    bcm_vlan_t vlan, 
    uint32 valid_fields, 
    bcm_vlan_control_vlan_t *control);

/* Set or retrieve current VLAN properties selectively. */
extern int bcm_vlan_control_vlan_selective_set(
    int unit, 
    bcm_vlan_t vlan, 
    uint32 valid_fields, 
    bcm_vlan_control_vlan_t *control);

/* Set or retrieve current VLAN properties. */
extern int bcm_vlan_control_vlan_get(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_vlan_control_vlan_t *control);

/* Set or retrieve current VLAN properties. */
extern int bcm_vlan_control_vlan_set(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_vlan_control_vlan_t control);

/* Set vlan_control_vlan flags over a VLAN vector. */
extern int bcm_vlan_vector_flags_set(
    int unit, 
    bcm_vlan_vector_t vlan_vector, 
    uint32 flags_mask, 
    uint32 flags_value);

#endif /* BCM_HIDE_DISPATCHABLE */

/* This structure contains the configuration of a VLAN. */
typedef struct bcm_vlan_block_s {
    bcm_pbmp_t known_multicast; 
    bcm_pbmp_t unknown_multicast; 
    bcm_pbmp_t unknown_unicast; 
    bcm_pbmp_t broadcast; 
    bcm_pbmp_t unicast; 
} bcm_vlan_block_t;

/* Initialize the bcm_vlan_block_s structure. */
extern void bcm_vlan_block_t_init(
    bcm_vlan_block_t *data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set VLAN block properties. */
extern int bcm_vlan_block_get(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_vlan_block_t *vlan_block);

/* Set VLAN block properties. */
extern int bcm_vlan_block_set(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_vlan_block_t *vlan_block);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a VLAN data information structure. */
extern void bcm_vlan_data_t_init(
    bcm_vlan_data_t *vlan_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure VLAN cross-connect. */
extern int bcm_vlan_cross_connect_add(
    int unit, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_gport_t port_1, 
    bcm_gport_t port_2);

/* Configure VLAN cross-connect. */
extern int bcm_vlan_cross_connect_delete(
    int unit, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan);

/* Configure VLAN cross-connect. */
extern int bcm_vlan_cross_connect_delete_all(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_vlan_cross_connect_traverse_cb */
typedef int (*bcm_vlan_cross_connect_traverse_cb)(
    int unit, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_gport_t port_1, 
    bcm_gport_t port_2, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure VLAN cross-connect. */
extern int bcm_vlan_cross_connect_traverse(
    int unit, 
    bcm_vlan_cross_connect_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Types of statistics that are maintained per VLAN. */
typedef enum bcm_vlan_stat_e {
    bcmVlanStatPackets = 0,             /* Packets that have hit the VLAN
                                           (forward/drop, L2/L3,
                                           unicast/multicast/broadcast/flood) */
    bcmVlanStatIngressPackets = bcmVlanStatPackets, /* Packets that ingress on the VLAN */
    bcmVlanStatBytes = 1,               /* Bytes that have hit the VLAN
                                           (forward/drop, L2/L3,
                                           unicast/multicast/broadcast/flood) */
    bcmVlanStatIngressBytes = bcmVlanStatBytes, /* Bytes that ingress on the VLAN */
    bcmVlanStatEgressPackets = 2,       /* Packets that egress on the VLAN */
    bcmVlanStatEgressBytes = 3,         /* Bytes that egress on the VLAN */
    bcmVlanStatForwardedPackets = 4,    /* Packets forwarded on the VLAN (L2/L3,
                                           unicast/multicast/broadcast/flood) */
    bcmVlanStatForwardedBytes = 5,      /* Bytes forwarded on the VLAN (L2/L3,
                                           unicast/multicast/broadcast/flood) */
    bcmVlanStatDropPackets = 6,         /* Packets dropped on the VLAN (L2/L3,
                                           unicast/multicast/broadcast/flood) */
    bcmVlanStatDropBytes = 7,           /* Bytes dropped on the VLAN (L2/L3,
                                           unicast/multicast/broadcast/flood) */
    bcmVlanStatUnicastPackets = 8,      /* L2 unicast packets forwarded on the
                                           VLAN */
    bcmVlanStatUnicastBytes = 9,        /* L2 unicast bytes forwarded on the
                                           VLAN */
    bcmVlanStatUnicastDropPackets = 10, /* L2 unicast packets dropped on the
                                           VLAN */
    bcmVlanStatUnicastDropBytes = 11,   /* L2 unicast bytes dropped on the VLAN */
    bcmVlanStatNonUnicastPackets = 12,  /* L2 multicast packets forwarded on the
                                           VLAN */
    bcmVlanStatNonUnicastBytes = 13,    /* L2 multicast bytes forwarded on the
                                           VLAN */
    bcmVlanStatNonUnicastDropPackets = 14, /* L2 non-unicast packets dropped on the
                                           VLAN */
    bcmVlanStatNonUnicastDropBytes = 15, /* L2 non-unicast bytes dropped on the
                                           VLAN */
    bcmVlanStatL3Packets = 16,          /* Packets delivered to L3 for
                                           forwarding on the VLAN */
    bcmVlanStatL3Bytes = 17,            /* Bytes delivered to L3 for forwarding
                                           on the VLAN */
    bcmVlanStatL3DropPackets = 18,      /* Packets delivered to L3 for dropping
                                           on the VLAN */
    bcmVlanStatL3DropBytes = 19,        /* Bytes delivered to L3 for dropping on
                                           the VLAN */
    bcmVlanStatFloodPackets = 20,       /* L2 flood packets forwarded on the
                                           VLAN */
    bcmVlanStatFloodBytes = 21,         /* L2 flood bytes forwarded on the VLAN */
    bcmVlanStatFloodDropPackets = 22,   /* L2 flood packets dropped on the VLAN */
    bcmVlanStatFloodDropBytes = 23,     /* L2 flood bytes dropped on the VLAN */
    bcmVlanStatGreenPackets = 24,       /* Green packets forwarded on the VLAN */
    bcmVlanStatGreenBytes = 25,         /* Green bytes forwarded on the VLAN */
    bcmVlanStatYellowPackets = 26,      /* Yellow packets forwarded on the VLAN */
    bcmVlanStatYellowBytes = 27,        /* Yellow bytes forwared on the VLAN */
    bcmVlanStatRedPackets = 28,         /* Red packets forwarded on the VLAN */
    bcmVlanStatRedBytes = 29,           /* Red bytes forwarded on the VLAN */
    bcmVlanStatCount = 30               /* Always last. Not a usable value. */
} bcm_vlan_stat_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Extract per-VLAN statistics from the chip. */
extern int bcm_vlan_stat_get(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_cos_t cos, 
    bcm_vlan_stat_t stat, 
    uint64 *val);

/* 
 * Force an immediate counter update and retrieve per-VLAN statistics
 * from
 * the chip.
 */
extern int bcm_vlan_stat_sync_get(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_cos_t cos, 
    bcm_vlan_stat_t stat, 
    uint64 *val);

/* Extract per-VLAN statistics from the chip. */
extern int bcm_vlan_stat_get32(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_cos_t cos, 
    bcm_vlan_stat_t stat, 
    uint32 *val);

/* 
 * Force an immediate counter update and retrieve per-VLAN statistics
 * from
 * the chip.
 */
extern int bcm_vlan_stat_sync_get32(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_cos_t cos, 
    bcm_vlan_stat_t stat, 
    uint32 *val);

/* 
 * Set the specified statistic to the indicated value for the specified
 * VLAN.
 */
extern int bcm_vlan_stat_set(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_cos_t cos, 
    bcm_vlan_stat_t stat, 
    uint64 val);

/* 
 * Set the specified statistic to the indicated value for the specified
 * VLAN.
 */
extern int bcm_vlan_stat_set32(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_cos_t cos, 
    bcm_vlan_stat_t stat, 
    uint32 val);

/* Get stat counter ID associated with given VLAN. */
extern int bcm_vlan_stat_id_get(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_vlan_stat_t stat, 
    uint32 *stat_counter_id);

/* Enable/disable collection of statistics on the indicated VLAN. */
extern int bcm_vlan_stat_enable_set(
    int unit, 
    bcm_vlan_t vlan, 
    int enable);

/* Get counter value for multiple VLAN statistic types. */
extern int bcm_vlan_stat_multi_get(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_cos_t cos, 
    int nstat, 
    bcm_vlan_stat_t *stat_arr, 
    uint64 *value_arr);

/* Get counter value for multiple VLAN statistic types. */
extern int bcm_vlan_stat_multi_get32(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_cos_t cos, 
    int nstat, 
    bcm_vlan_stat_t *stat_arr, 
    uint32 *value_arr);

/* Set counter value for multiple VLAN statistic types. */
extern int bcm_vlan_stat_multi_set(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_cos_t cos, 
    int nstat, 
    bcm_vlan_stat_t *stat_arr, 
    uint64 *value_arr);

/* Set counter value for multiple VLAN statistic types. */
extern int bcm_vlan_stat_multi_set32(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_cos_t cos, 
    int nstat, 
    bcm_vlan_stat_t *stat_arr, 
    uint32 *value_arr);

/* Extract ingress VLAN translation statistics from the chip. */
extern int bcm_vlan_translate_stat_get(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_stat_t stat, 
    uint64 *val);

/* 
 * Force an immediate counter update and retrieve ingress VLAN
 * translation statistics from the chip.
 */
extern int bcm_vlan_translate_stat_sync_get(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_stat_t stat, 
    uint64 *val);

/* Extract ingress VLAN translation statistics from the chip. */
extern int bcm_vlan_translate_stat_get32(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_stat_t stat, 
    uint32 *val);

/* 
 * Force an immediate counter update and retrieve ingress VLAN
 * translation statistics from the chip.
 */
extern int bcm_vlan_translate_stat_sync_get32(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_stat_t stat, 
    uint32 *val);

/* 
 * Get stat counter ID associated with given ingress VLAN translation
 * table.
 */
extern int bcm_vlan_translate_stat_id_get(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_stat_t stat, 
    uint32 *stat_counter_id);

/* 
 * Set the specified statistic to the indicated value for the specified
 * ingress VLAN translation.
 */
extern int bcm_vlan_translate_stat_set(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_stat_t stat, 
    uint64 val);

/* 
 * Set the specified statistic to the indicated value for the specified
 * ingress VLAN translation.
 */
extern int bcm_vlan_translate_stat_set32(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_stat_t stat, 
    uint32 val);

/* 
 * Enable/disable collection of statistics on the indicated ingress VLAN
 * translation.
 */
extern int bcm_vlan_translate_stat_enable_set(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    int enable);

/* 
 * Get 64-bit counter value for multiple ingress VLAN translation
 * statistic types.
 */
extern int bcm_vlan_translate_stat_multi_get(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    int nstat, 
    bcm_vlan_stat_t *stat_arr, 
    uint64 *value_arr);

/* 
 * Get 64-bit counter value for multiple ingress VLAN translation
 * statistic types.
 */
extern int bcm_vlan_translate_stat_multi_get32(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    int nstat, 
    bcm_vlan_stat_t *stat_arr, 
    uint32 *value_arr);

/* 
 * Set 64-bit counter value for multiple ingress VLAN translation
 * statistic types.
 */
extern int bcm_vlan_translate_stat_multi_set(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    int nstat, 
    bcm_vlan_stat_t *stat_arr, 
    uint64 *value_arr);

/* 
 * Set 64-bit counter value for multiple ingress VLAN translation
 * statistic types.
 */
extern int bcm_vlan_translate_stat_multi_set32(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    int nstat, 
    bcm_vlan_stat_t *stat_arr, 
    uint32 *value_arr);

/* Extract egress VLAN translation statistics from the chip. */
extern int bcm_vlan_translate_egress_stat_get(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_stat_t stat, 
    uint64 *val);

/* 
 * Force an immediate counter update and retrieve egress VLAN
 * translation statistics from the chip.
 */
extern int bcm_vlan_translate_egress_stat_sync_get(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_stat_t stat, 
    uint64 *val);

/* Extract egress VLAN translation statistics from the chip. */
extern int bcm_vlan_translate_egress_stat_get32(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_stat_t stat, 
    uint32 *val);

/* 
 * Force an immediate counter update and retrieve egress VLAN
 * translation statistics from the chip.
 */
extern int bcm_vlan_translate_egress_stat_sync_get32(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_stat_t stat, 
    uint32 *val);

/* 
 * Get stat counter ID associated with given egress VLAN translation
 * table.
 */
extern int bcm_vlan_translate_egress_stat_id_get(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_stat_t stat, 
    uint32 *stat_counter_id);

/* 
 * Set the specified statistic to the indicated value for the specified
 * egress VLAN translation.
 */
extern int bcm_vlan_translate_egress_stat_set(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_stat_t stat, 
    uint64 val);

/* 
 * Set the specified statistic to the indicated value for the specified
 * egress VLAN translation.
 */
extern int bcm_vlan_translate_egress_stat_set32(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_stat_t stat, 
    uint32 val);

/* 
 * Enable/disable collection of statistics on the indicated egress VLAN
 * translation.
 */
extern int bcm_vlan_translate_egress_stat_enable_set(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    int enable);

/* 
 * Get 64-bit counter value for multiple egress VLAN translation
 * statistic types.
 */
extern int bcm_vlan_translate_egress_stat_multi_get(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    int nstat, 
    bcm_vlan_stat_t *stat_arr, 
    uint64 *value_arr);

/* 
 * Get 64-bit counter value for multiple egress VLAN translation
 * statistic types.
 */
extern int bcm_vlan_translate_egress_stat_multi_get32(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    int nstat, 
    bcm_vlan_stat_t *stat_arr, 
    uint32 *value_arr);

/* 
 * Set 64-bit counter value for multiple egress VLAN translation
 * statistic types.
 */
extern int bcm_vlan_translate_egress_stat_multi_set(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    int nstat, 
    bcm_vlan_stat_t *stat_arr, 
    uint64 *value_arr);

/* 
 * Set 64-bit counter value for multiple egress VLAN translation
 * statistic types.
 */
extern int bcm_vlan_translate_egress_stat_multi_set32(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    int nstat, 
    bcm_vlan_stat_t *stat_arr, 
    uint32 *value_arr);

/* 
 * Get 64-bit or 32-bit counter value for multiple GPORT or VLAN/port
 * statistic types.
 */
extern int bcm_vlan_port_stat_multi_get(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_port_t port, 
    bcm_cos_t cos, 
    int nstat, 
    bcm_vlan_stat_t *stat_arr, 
    uint64 *value_arr);

/* 
 * Get 64-bit or 32-bit counter value for multiple GPORT or VLAN/port
 * statistic types.
 */
extern int bcm_vlan_port_stat_multi_get32(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_port_t port, 
    bcm_cos_t cos, 
    int nstat, 
    bcm_vlan_stat_t *stat_arr, 
    uint32 *value_arr);

/* Manipulate the Policer ID associated with the specified VLAN. */
extern int bcm_vlan_policer_get(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_policer_t *policer_id);

/* Manipulate the Policer ID associated with the specified VLAN. */
extern int bcm_vlan_policer_set(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_policer_t policer_id);

/* 
 * Set 64-bit or 32-bit counter value for multiple GPORT or VLAN/port
 * statistic types.
 */
extern int bcm_vlan_port_stat_multi_set(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_port_t port, 
    bcm_cos_t cos, 
    int nstat, 
    bcm_vlan_stat_t *stat_arr, 
    uint64 *value_arr);

/* 
 * Set 64-bit or 32-bit counter value for multiple GPORT or VLAN/port
 * statistic types.
 */
extern int bcm_vlan_port_stat_multi_set32(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_port_t port, 
    bcm_cos_t cos, 
    int nstat, 
    bcm_vlan_stat_t *stat_arr, 
    uint32 *value_arr);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flags for logical layer 2 port. */
#define BCM_VLAN_PORT_REPLACE               0x00000001 /* Replace existing
                                                          entry. */
#define BCM_VLAN_PORT_WITH_ID               0x00000002 /* Add using the
                                                          specified ID. */
#define BCM_VLAN_PORT_INNER_VLAN_PRESERVE   0x00000004 /* Preserve the inner
                                                          VLAN tag (by default
                                                          it is stripped). */
#define BCM_VLAN_PORT_INNER_VLAN_ADD        0x00000008 /* Add the port vid to
                                                          untagged and
                                                          pri-tagged packets. */
#define BCM_VLAN_PORT_EGRESS_UNTAGGED       0x00000010 /* Indicates tag is
                                                          stripped on logical
                                                          port egress */
#define BCM_VLAN_PORT_EGRESS_VLAN16         0x00000020 /* Indicates a 16-bit
                                                          VLAN tag (not the
                                                          default 3-bit
                                                          priority, 1-bit CFI,
                                                          and 12-bit VLAN tag)
                                                          is added on logical
                                                          port egress */
#define BCM_VLAN_PORT_ENCAP_WITH_ID         0x00000040 /* Use the specified
                                                          encap id to allocate
                                                          and setup
                                                          encapsulation. */
#define BCM_VLAN_PORT_NETWORK               0x00000080 /* Core facing vlan port.
                                                          This is used for
                                                          split-horizon checks.
                                                          Packets forwarded from
                                                          core-facing port to
                                                          another core-facing
                                                          port is discarded. */
#define BCM_VLAN_PORT_OUTER_VLAN_PRESERVE   0x00000100 /* Preserve the outer
                                                          VLAN tag (by default
                                                          it is stripped). */
#define BCM_VLAN_PORT_EXTENDED              0x00000200 /* Use the extended P2P
                                                          service. */
#define BCM_VLAN_PORT_TRANSPARENT           0x00000400 /* Use the transparent
                                                          mode to pass through
                                                          traffic from PON port
                                                          to NNI port. */
#define BCM_VLAN_PORT_CREATE_INGRESS_ONLY   0x00000800 /* Indicates Ingress
                                                          settings */
#define BCM_VLAN_PORT_CREATE_EGRESS_ONLY    0x00001000 /* Indicates Egress
                                                          settings */
#define BCM_VLAN_PORT_LOCAL_PROTECTION      0x00002000 /* Local protection
                                                          according to link
                                                          status */
#define BCM_VLAN_PORT_FORWARD_GROUP         0x00004000 /* Use forwarding group */
#define BCM_VLAN_PORT_EGRESS_PROTECTION     0x00008000 /* Egress Protection
                                                          object */
#define BCM_VLAN_PORT_VSI_BASE_VID          0x00010000 /* VSI assignment
                                                          mode.VSI equal to VSI
                                                          in LIF table + VID */
#define BCM_VLAN_PORT_CASCADED              0x00020000 /* Creates a VLAN port
                                                          object of type FEC
                                                          that is pointed by
                                                          another VLAN port
                                                          object */
#define BCM_VLAN_PORT_INGRESS_WIDE          0x00040000 /* Creates a wide ingress
                                                          lif that can hold
                                                          user's data. */
#define BCM_VLAN_PORT_VLAN_TRANSLATION      0x00080000 /* Creates a vlan
                                                          translation only
                                                          egress lif. */
#define BCM_VLAN_PORT_EGRESS_PON_TUNNEL_NONE 0x00100000 /* This flag indicates
                                                          that LLTAG is not
                                                          added when a packet
                                                          egress out from this
                                                          VLAN port. */
#define BCM_VLAN_PORT_NATIVE                0x00200000 /* Creates a native vlan
                                                          port. */
#define BCM_VLAN_PORT_DEFAULT               0x00400000 /* Creates default vlan
                                                          editing object. */
#define BCM_VLAN_PORT_USE_SGLP_TPID         0x00800000 /* Use TPID based on
                                                          Source global logical
                                                          port (SGLP) for
                                                          parsing the packet. */
#define BCM_VLAN_PORT_EGRESS_ARP_POINTED    0x01000000 /* Create an ARP pointed
                                                          vlan port. */
#define BCM_VLAN_PORT_STAT_INGRESS_ENABLE   0x02000000 /* Enable statistics on
                                                          ingress side. */
#define BCM_VLAN_PORT_STAT_EGRESS_ENABLE    0x04000000 /* Enable statistics on
                                                          egress side. */
#define BCM_VLAN_PORT_ALLOC_SYMMETRIC       0x08000000 /* Indicate that VLAN
                                                          Port ID allocation
                                                          (ingress or egress)
                                                          will return a GPORT
                                                          which is free in both
                                                          ingress and egress. */
#define BCM_VLAN_PORT_RECYCLE               0x10000000 /* Creates ingress only
                                                          control lif. */
#define BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS 0x20000000 /* Creates an egress vlan
                                                          translation lif with
                                                          two vlan tags. */

/* Logical layer 2 port match criteria */
typedef enum bcm_vlan_port_match_e {
    BCM_VLAN_PORT_MATCH_INVALID = 0,    /* Illegal. */
    BCM_VLAN_PORT_MATCH_NONE = 1,       /* No source match criteria. */
    BCM_VLAN_PORT_MATCH_PORT = 2,       /* {Module, Port} or Trunk. */
    BCM_VLAN_PORT_MATCH_PORT_VLAN = 3,  /* Mod/port/trunk + outer VLAN or SVLAN. */
    BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED = 4, /* Mod/port/trunk + outer/inner VLAN. */
    BCM_VLAN_PORT_MATCH_PORT_VLAN16 = 5, /* Mod/port/trunk + 16-bit outer VLAN
                                           tag. */
    BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN = 6, /* Mod/port/trunk + outer VLAN + outer
                                           PCP. */
    BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED = 7, /* Mod/port/trunk + outer/inner VLAN +
                                           outer/inner PCP. */
    BCM_VLAN_PORT_MATCH_PORT_INITIAL_VLAN = 8, /* Mod/port/trunk + Initial-VID */
    BCM_VLAN_PORT_MATCH_PORT_TUNNEL = 9, /* Mod/port/trunk + Tunnel Value. */
    BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN = 10, /* Mod/port/trunk + Tunnel Value + outer
                                           VLAN. */
    BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED = 11, /* Mod/port/trunk + Tunnel Value + outer
                                           VLAN + inner VLAN. */
    BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN = 12, /* Mod/port/trunk + Tunnel Value + outer
                                           PCP + outer VLAN. */
    BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_STACKED = 13, /* Mod/port/trunk + Tunnel Value + outer
                                           PCP + outer VLAN + inner VLAN. */
    BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_ETHERTYPE = 14, /* Mod/port/trunk + Tunnel Value + Outer
                                           PCP + outer VLAN + EtherType. */
    BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_VLAN_ETHERTYPE = 15, /* Mod/port/trunk + Tunnel Value + Outer
                                           PCP + outer VLAN + Inner VLAN +
                                           EtherType. */
    BCM_VLAN_PORT_MATCH_PORT_TUNNEL_ETHERTYPE = 16, /* Mod/port/trunk + Tunnel Value +
                                           EtherType. */
    BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_ETHERTYPE = 17, /* Mod/port/trunk + Tunnel Value + outer
                                           VLAN + EtherType. */
    BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED_ETHERTYPE = 18, /* Mod/port/trunk + Tunnel Value + outer
                                           VLAN + inner VLAN + EtherType. */
    BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP = 19, /* Mod/port/trunk + Tunnel Value + outer
                                           PCP. */
    BCM_VLAN_PORT_MATCH_PORT_UNTAGGED = 20, /* Mod/port/trunk for untagged packets. */
    BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL = 21, /* Mod/port/trunk + LLTAG VLAN ID[11:0]. */
    BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_VLAN = 22, /* Mod/port/trunk + LLTAG VLAN ID[11:0] 
                                           + Outer VLAN. */
    BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_INNER_VLAN = 23, /* Mod/port/trunk + LLTAG VLAN ID[11:0]
                                           + Inner VLAN. */
    BCM_VLAN_PORT_MATCH_PORT_INNER_VLAN = 24, /* Mod/port/trunk + Inner VLAN. */
    BCM_VLAN_PORT_MATCH_PORT_INNER_VLAN16 = 25, /* Mod/port/trunk + 16-bit Inner VLAN
                                           tag. */
    BCM_VLAN_PORT_MATCH_PORT_CVLAN = 26, /* Mod/port/trunk + CVLAN. */
    BCM_VLAN_PORT_MATCH_NAMESPACE_VSI = 27, /* Namespace + VSI. */
    BCM_VLAN_PORT_MATCH_NAMESPACE_PORT = 28, /* Namespace + port. */
    BCM_VLAN_PORT_MATCH_PORT_SVLAN_STACKED = 29, /* Mod/port/trunk + outer/inner both
                                           SVLAN. */
    BCM_VLAN_PORT_MATCH_PORT_CVLAN_STACKED = 30, /* Mod/port/trunk + outer/inner both
                                           CVLAN. */
    BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN = 31, /* Mod/port/trunk + Tunnel Value +
                                           outer/inner both CVLAN. */
    BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN_STACKED = 32, /* Mod/port/trunk + Tunnel Value +
                                           outer/inner both CVLAN. */
    BCM_VLAN_PORT_MATCH_PORT_TUNNEL_SVLAN_STACKED = 33, /* Mod/port/trunk + Tunnel Value +
                                           outer/inner both SVLAN. */
    BCM_VLAN_PORT_MATCH_PORT_TUNNEL_UNTAGGED = 34, /* Mod/port/trunk + Tunnel Value +
                                           untagged packets. */
    BCM_VLAN_PORT_MATCH_COUNT = 35      /* Must be last. */
} bcm_vlan_port_match_t;

/* Layer 2 Logical port type */
typedef struct bcm_vlan_port_s {
    bcm_vlan_port_match_t criteria;     /* Match criteria. */
    uint32 flags;                       /* BCM_VLAN_PORT_xxx. */
    bcm_vlan_t vsi;                     /* Populated for bcm_vlan_port_find only */
    bcm_vlan_t match_vlan;              /* Outer VLAN ID to match. */
    bcm_vlan_t match_inner_vlan;        /* Inner VLAN ID to match. */
    int match_pcp;                      /* Outer PCP ID to match. */
    bcm_tunnel_id_t match_tunnel_value; /* Tunnel value to match. */
    bcm_port_ethertype_t match_ethertype; /* Ethernet type value to match. */
    bcm_gport_t port;                   /* Gport: local or remote Physical or
                                           logical gport. */
    uint8 pkt_pri;                      /* Service tag priority. */
    uint8 pkt_cfi;                      /* Service tag cfi. */
    uint16 egress_service_tpid;         /* Service VLAN TPID value. */
    bcm_vlan_t egress_vlan;             /* Egress Outer VLAN or SD-TAG VLAN ID. */
    bcm_vlan_t egress_inner_vlan;       /* Egress Inner VLAN. */
    bcm_tunnel_id_t egress_tunnel_value; /* Egress Tunnel value. */
    bcm_if_t encap_id;                  /* Encapsulation Index. */
    int qos_map_id;                     /* QoS Map Index. */
    bcm_policer_t policer_id;           /* Policer ID */
    bcm_policer_t egress_policer_id;    /* Egress Policer ID */
    bcm_failover_t failover_id;         /* Failover Object Index. */
    bcm_gport_t failover_port_id;       /* Failover VLAN Port Identifier. */
    bcm_gport_t vlan_port_id;           /* GPORT identifier */
    bcm_multicast_t failover_mc_group;  /* MC group used for bi-cast. */
    bcm_failover_t ingress_failover_id; /* 1+1 protection. */
    bcm_failover_t egress_failover_id;  /* Failover object index for Egress
                                           Protection */
    bcm_gport_t egress_failover_port_id; /* Failover VLAN Port Identifier for
                                           Egress Protection */
    bcm_switch_network_group_t ingress_network_group_id; /* Split Horizon ingress network group
                                           identifier */
    bcm_switch_network_group_t egress_network_group_id; /* Split Horizon egress network group
                                           identifier */
    int inlif_counting_profile;         /* In LIF counting profile */
    int outlif_counting_profile;        /* Out LIF counting profile */
    uint32 if_class;                    /* Egress Interface class ID */
    bcm_gport_t tunnel_id;              /* Pointer from Native AC to next entry
                                           in EEDB list. */
    uint32 group;                       /* flush group id to be used by l2 flush
                                           mechanism */
    bcm_gport_t ingress_failover_port_id; /* Failover VLAN Port Identifier for
                                           ingress Protection. */
    uint16 class_id;                    /* Class ID of the source port */
    uint32 match_class_id;              /* Match Namespace. */
    bcm_qos_ingress_model_t ingress_qos_model; /* ingress qos and ttl model */
    bcm_qos_egress_model_t egress_qos_model; /* egress qos and ttl model */
} bcm_vlan_port_t;

/* Initialize the VLAN port structure. */
extern void bcm_vlan_port_t_init(
    bcm_vlan_port_t *vlan_port);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a layer 2 logical port. */
extern int bcm_vlan_port_create(
    int unit, 
    bcm_vlan_port_t *vlan_port);

/* Destroy a layer 2 logical port. */
extern int bcm_vlan_port_destroy(
    int unit, 
    bcm_gport_t gport);

/* Get/find a layer 2 logical port given the GPORT ID or match criteria. */
extern int bcm_vlan_port_find(
    int unit, 
    bcm_vlan_port_t *vlan_port);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_vlan_port_translation_s */
typedef struct bcm_vlan_port_translation_s {
    uint32 flags;               /* BCM_VLAN_ACTION_SET_XXX */
    bcm_gport_t gport;          /* gport ID */
    uint32 new_outer_vlan;      /* New outer VLAN ID to use in vlan translation */
    uint32 new_inner_vlan;      /* New inner VLAN ID to use in vlan translation */
    uint32 vlan_edit_class_id;  /* VLAN editing profile */
} bcm_vlan_port_translation_t;

/* 
 * Initialize a structure that holds both the key and the info for VLAN
 * port translation configuration.
 */
extern void bcm_vlan_port_translation_t_init(
    bcm_vlan_port_translation_t *vlan_port_translation);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set translation information for a VLAN port. */
extern int bcm_vlan_port_translation_set(
    int unit, 
    bcm_vlan_port_translation_t *vlan_port_translation);

/* Get translation information from a VLAN port. */
extern int bcm_vlan_port_translation_get(
    int unit, 
    bcm_vlan_port_translation_t *vlan_port_translation);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flags for bcm_vlan_queue_map_create(). */
#define BCM_VLAN_QUEUE_MAP_WITH_ID          0x0001     
#define BCM_VLAN_QUEUE_MAP_REPLACE          0x0002     
#define BCM_VLAN_QUEUE_MAP_INNER_PKT_PRI    0x0004     
#define BCM_VLAN_QUEUE_MAP_OUTER_PKT_PRI    0x0008     

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a VLAN queue map entry. */
extern int bcm_vlan_queue_map_create(
    int unit, 
    uint32 flags, 
    int *qmid);

/* Delete a VLAN queue map entry. */
extern int bcm_vlan_queue_map_destroy(
    int unit, 
    int qmid);

/* Delete all VLAN queue map entries. */
extern int bcm_vlan_queue_map_destroy_all(
    int unit);

/* Set a VLAN queue map entry. */
extern int bcm_vlan_queue_map_set(
    int unit, 
    int qmid, 
    int pkt_pri, 
    int cfi, 
    int queue, 
    int color);

/* Get a VLAN queue map entry. */
extern int bcm_vlan_queue_map_get(
    int unit, 
    int qmid, 
    int pkt_pri, 
    int cfi, 
    int *queue, 
    int *color);

/* Attach a queue map object to a VLAN or VFI. */
extern int bcm_vlan_queue_map_attach(
    int unit, 
    bcm_vlan_t vlan, 
    int qmid);

/* Get the queue map object which is attached to a VLAN or VFI. */
extern int bcm_vlan_queue_map_attach_get(
    int unit, 
    bcm_vlan_t vlan, 
    int *qmid);

/* Detach a queue map object from a VLAN or VFI. */
extern int bcm_vlan_queue_map_detach(
    int unit, 
    bcm_vlan_t vlan);

/* Detach queue map objects from all VLAN and VFI. */
extern int bcm_vlan_queue_map_detach_all(
    int unit);

/* Attach   counter entries to the given VLAN. */
extern int bcm_vlan_stat_attach(
    int unit, 
    bcm_vlan_t vlan, 
    uint32 stat_counter_id);

/* Detach   counter entries to the given VLAN. */
extern int bcm_vlan_stat_detach(
    int unit, 
    bcm_vlan_t vlan);

/* Detach counter entries to the given VLAN with a given stat counter id. */
extern int bcm_vlan_stat_detach_with_id(
    int unit, 
    bcm_vlan_t vlan, 
    uint32 stat_counter_id);

/* 
 * Get the specified statistic to the indicated value for the specified
 * VLAN
 */
extern int bcm_vlan_stat_counter_get(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_vlan_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* 
 * Get the specified statistic to the indicated value for the specified
 * VLAN
 */
extern int bcm_vlan_stat_counter_sync_get(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_vlan_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* 
 * Set the specified statistic to the indicated value for the specified
 * VLAN
 */
extern int bcm_vlan_stat_counter_set(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_vlan_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* 
 * Attach   counters entries for the given ingress VLAN translation
 * table.
 */
extern int bcm_vlan_translate_stat_attach(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    uint32 stat_counter_id);

/* 
 * Detach   counters entries for the given ingress VLAN translation
 * table.
 */
extern int bcm_vlan_translate_stat_detach(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan);

/* Get counter values from ingress VLAN translation table. */
extern int bcm_vlan_translate_stat_counter_get(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* 
 * Force an immediate counter update and retrieve counter values from
 * ingress
 * VLAN translation table.
 */
extern int bcm_vlan_translate_stat_counter_sync_get(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Set counter values into a ingress VLAN translation table */
extern int bcm_vlan_translate_stat_counter_set(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Attach counters for the given egress VLAN translation table. */
extern int bcm_vlan_translate_egress_stat_attach(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    uint32 stat_counter_id);

/* Detach counters for the given egress VLAN translation table. */
extern int bcm_vlan_translate_egress_stat_detach(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan);

/* Get counter values from egress VLAN translation table. */
extern int bcm_vlan_translate_egress_stat_counter_get(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* 
 * Force an immediate counter update and retrieve counter values from
 * egress
 * VLAN translation table.
 */
extern int bcm_vlan_translate_egress_stat_counter_sync_get(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Set counter values from egress VLAN translation table. */
extern int bcm_vlan_translate_egress_stat_counter_set(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flex counter configuration structure in VLAN module. */
typedef struct bcm_vlan_flexctr_config_s {
    uint32 stat_counter_id; /* Stat counter ID. */
    uint32 object_value;    /* Object value. */
} bcm_vlan_flexctr_config_t;

/* Initialize flex counter configuration structure in VLAN module. */
extern void bcm_vlan_flexctr_config_t_init(
    bcm_vlan_flexctr_config_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Attach the stat counter ID and object value into a given ingress VLAN
 * translation table.
 */
extern int bcm_vlan_translate_flexctr_attach(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_flexctr_config_t *config);

/* 
 * Detach the stat counter ID and object value from a given ingress VLAN
 * translation table.
 */
extern int bcm_vlan_translate_flexctr_detach(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_flexctr_config_t *config);

/* 
 * Detach all stat counter IDs and object values from a given ingress
 * VLAN translation table.
 */
extern int bcm_vlan_translate_flexctr_detach_all(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan);

/* 
 * Get the stat counter ID and object value from a given ingress VLAN
 * translation table.
 */
extern int bcm_vlan_translate_flexctr_get(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    int array_size, 
    bcm_vlan_flexctr_config_t *config_array, 
    int *count);

/* 
 * Attach the stat counter ID and object value into a given egress VLAN
 * translation table.
 */
extern int bcm_vlan_translate_egress_flexctr_attach(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_flexctr_config_t *config);

/* 
 * Detach the stat counter ID and object value from a given egress VLAN
 * translation table.
 */
extern int bcm_vlan_translate_egress_flexctr_detach(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_flexctr_config_t *config);

/* 
 * Detach all stat counter IDs and object values from a given egress VLAN
 * translation table.
 */
extern int bcm_vlan_translate_egress_flexctr_detach_all(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan);

/* 
 * Get the stat counter ID and object value from a given egress VLAN
 * translation table.
 */
extern int bcm_vlan_translate_egress_flexctr_get(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    int array_size, 
    bcm_vlan_flexctr_config_t *config_array, 
    int *count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Seamless Redundancy flags for this VLAN */
#define BCM_VLAN_CONTROL_SR_FLAG_ENABLE     0x80000000 /* SR enabled on this
                                                          VLAN */
#define BCM_VLAN_CONTROL_SR_FLAG_LAN_ID     0x00000001 /* Set if LAN B.
                                                          Otherwise LAN A. */
#define BCM_VLAN_CONTROL_SR_FLAG_ENABLE_WITH_LAN_ID_A (BCM_VLAN_CONTROL_SR_FLAG_ENABLE) 
#define BCM_VLAN_CONTROL_SR_FLAG_ENABLE_WITH_LAN_ID_B (BCM_VLAN_CONTROL_SR_FLAG_ENABLE |\
                                                       BCM_VLAN_CONTROL_SR_FLAG_LAN_ID) 

/* VLAN action flags */
#define BCM_VLAN_ACTION_SET_WITH_ID         0x0001     /* Add VLAN action
                                                          instance using a
                                                          specific ID */
#define BCM_VLAN_ACTION_SET_REPLACE         0x0002     /* Replace data in an
                                                          existing VLAN action
                                                          instance */
#define BCM_VLAN_ACTION_SET_INGRESS         0x0004     /* Add VLAN action
                                                          instance only on the
                                                          ingress side */
#define BCM_VLAN_ACTION_SET_EGRESS          0x0008     /* Add VLAN action
                                                          instance only on the
                                                          egress side */
#define BCM_VLAN_ACTION_SET_TPID_ALLOW_CREATE 0x0010     /* Create a TPID profile
                                                          if TPIDs can't match
                                                          an existing one. */
#define BCM_VLAN_ACTION_SET_TPID_ALLOW_DESTROY 0x0020     /* Destroy a TPID profile
                                                          using specifi TPIDs */
#define BCM_VLAN_ACTION_SET_OUTER_VID_UNTAGGED 0x0040     /* Set the packet format
                                                          after EVE to untagged */
#define BCM_VLAN_ACTION_SET_VLAN_CHECKS_DISABLE 0x0080     /* Disable spanning tree
                                                          and ingress port VLAN
                                                          membership checks */
#define BCM_VLAN_ACTION_SET_SR_ENABLE       0x00000100 /* Enable Seamless
                                                          Redundancy(SR) */
#define BCM_VLAN_ACTION_SET_SR_LAN_ID       0x00000200 /* SR LAN ID. Set if LAN
                                                          B. Otherwise LAN A */
#define BCM_VLAN_ACTION_SET_SR_ENABLE_WITH_LAN_ID_A (BCM_VLAN_ACTION_SET_SR_ENABLE) 
#define BCM_VLAN_ACTION_SET_SR_ENABLE_WITH_LAN_ID_B (BCM_VLAN_ACTION_SET_SR_ENABLE |\
                                                     BCM_VLAN_ACTION_SET_SR_LAN_ID) 
#define BCM_VLAN_ACTION_SET_DUMMY_VP        0x00000400 /* Configure VLAN
                                                          Translation entry with
                                                          dummy VP */
#define BCM_VLAN_ACTION_SET_EGRESS_DISABLE_MEMBERSHIP 0x00000800 /* Post EVE disable VLAN
                                                          membership. */
#define BCM_VLAN_ACTION_SET_EGRESS_ENABLE_INNER_MEMBERSHIP 0x00001000 /* Post EVE enable VLAN
                                                          membership use Inner
                                                          VLAN */
#define BCM_VLAN_ACTION_SET_EGRESS_DISABLE_OUTER_TAG_REMOVAL 0x00002000 /* Post EVE disable
                                                          removal of Outer VLAN
                                                          tag */
#define BCM_VLAN_ACTION_SET_EGRESS_DISABLE_MIRRORING 0x00004000 /* Post EVE disable
                                                          mirroring. */
#define BCM_VLAN_ACTION_SET_EGRESS_ENABLE_INNER_MIRRORING 0x00008000 /* Post EVE enable
                                                          mirroring use Inner
                                                          VLAN */
#define BCM_VLAN_ACTION_SET_EGRESS_TUNNEL_OVID 0x00010000 /* Apply action to Tunnel
                                                          Outer VLAN tag. */
#define BCM_VLAN_ACTION_SET_DROP            0x00020000 /* Packet drop on VLAN
                                                          translation hit. */
#define BCM_VLAN_ACTION_SET_COPY_TO_CPU     0x00040000 /* Packet copy to CPU on
                                                          VLAN translation hit. */
#define BCM_VLAN_ACTION_SET_INNER_QOS_MAP_ID 0x00080000 /* Configures the inner
                                                          QOS map pointer on
                                                          VLAN translation hit. */
#define BCM_VLAN_ACTION_SET_OUTER_QOS_MAP_ID 0x00100000 /* Configures the outer
                                                          QOS map pointer on
                                                          VLAN translation hit. */
#define BCM_VLAN_ACTION_SET_HIGIG3_EXTENSION_ADD 0x00200000 /* Add a HIGIG3 extension
                                                          header. */

/* Gport info struct */
typedef struct bcm_vlan_gport_info_s {
    uint32 flags;       /* BCM_VLAN_PORT_GPORT_XXX */
    bcm_vlan_t vlan; 
    bcm_vlan_t vsi; 
    bcm_gport_t gport; 
} bcm_vlan_gport_info_t;

/* Init structure bcm_vlan_gport_info_t */
extern void bcm_vlan_gport_info_t_init(
    bcm_vlan_gport_info_t *info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Get a virtual or physical port from the specified VLAN. */
extern int bcm_vlan_gport_info_get(
    int unit, 
    bcm_vlan_gport_info_t *vlan_gport_info);

#endif /* BCM_HIDE_DISPATCHABLE */

#if defined(INCLUDE_L3)
/* BCM_VLAN_VPN_* flags for VPN Create. */
#define BCM_VLAN_VPN_ELINE      0x00000001 
#define BCM_VLAN_VPN_ELAN       0x00000002 
#define BCM_VLAN_VPN_WITH_ID    0x00000004 
#endif

#if defined(INCLUDE_L3)
/* VPN Config Structure. */
typedef struct bcm_vlan_vpn_config_s {
    uint32 flags;                       /* BCM_VXLAN_VPN_xxx. */
    bcm_vpn_t vpn; 
    bcm_multicast_t broadcast_group; 
    bcm_multicast_t unknown_unicast_group; 
    bcm_multicast_t unknown_multicast_group; 
} bcm_vlan_vpn_config_t;
#endif

#if defined(INCLUDE_L3)
/* Initialize VPN config structure. */
extern void bcm_vlan_vpn_config_t_init(
    bcm_vlan_vpn_config_t *info);
#endif

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(INCLUDE_L3)
/* Create or update VPN. */
extern int bcm_vlan_vpn_create(
    int unit, 
    bcm_vlan_vpn_config_t *info);
#endif

#if defined(INCLUDE_L3)
/* Destroy VPN. */
extern int bcm_vlan_vpn_destroy(
    int unit, 
    bcm_vpn_t vpn);
#endif

#if defined(INCLUDE_L3)
/* Destroy VPN. */
extern int bcm_vlan_vpn_destroy_all(
    int unit);
#endif

#if defined(INCLUDE_L3)
/* Get VPN information. */
extern int bcm_vlan_vpn_get(
    int unit, 
    bcm_vpn_t vpn, 
    bcm_vlan_vpn_config_t *info);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

/* Vpn callback function prototype */
#if defined(INCLUDE_L3)
typedef int(*bcm_vlan_vpn_traverse_cb)(
    int unit, 
    bcm_vlan_vpn_config_t *info, 
    void *user_data);
#endif

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(INCLUDE_L3)
/* Traverse all valid VPN entries and call the supplied callback routine. */
extern int bcm_vlan_vpn_traverse(
    int unit, 
    bcm_vlan_vpn_traverse_cb cb, 
    void *user_data);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

#if defined(INCLUDE_L3)
/* BCM_VLAN_PORT_SERVICE_* flags. */
#define BCM_VLAN_PORT_SERVICE_TAGGED        0x00200000 /* Service tag mode */
#define BCM_VLAN_PORT_SERVICE_VLAN_ADD      0x00400000 /* Add SD-tag */
#define BCM_VLAN_PORT_SERVICE_VLAN_REPLACE  0x00800000 /* Replace VLAN not TPID */
#define BCM_VLAN_PORT_SERVICE_VLAN_DELETE   0x01000000 /* Delete VLAN Tag */
#define BCM_VLAN_PORT_SERVICE_VLAN_TPID_REPLACE 0x02000000 /* Replace VLAN and TPID */
#define BCM_VLAN_PORT_SERVICE_VLAN_PRI_TPID_REPLACE 0x04000000 /* Replace VLAN, VLAN-PRI
                                                          and TPID */
#define BCM_VLAN_PORT_SERVICE_VLAN_PRI_REPLACE 0x08000000 /* Replace VLAN and
                                                          VLAN-PRI */
#define BCM_VLAN_PORT_SERVICE_PRI_REPLACE   0x10000000 /* Replace VLAN-PRI only */
#define BCM_VLAN_PORT_SERVICE_TPID_REPLACE  0x20000000 /* Replace TPID only */
#endif

/* Structure of VLAN port traverse object. */
typedef struct bcm_vlan_port_traverse_info_s {
    uint32 flags;   /* BCM_VLAN_PORT_xxx. */
} bcm_vlan_port_traverse_info_t;

/* VLAN port traverse callback function prototype. */
typedef int (*bcm_vlan_port_traverse_cb)(
    int unit, 
    bcm_vlan_port_t *vlan_port, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse routine for VLAN port. */
extern int bcm_vlan_port_traverse(
    int unit, 
    bcm_vlan_port_traverse_info_t *additional_info, 
    bcm_vlan_port_traverse_cb trav_fn, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Generic match criteria to assign Vlan. */
typedef enum bcm_vlan_match_e {
    BCM_VLAN_MATCH_INVALID = 0,         /* Invalid */
    BCM_VLAN_MATCH_MAC_PORT = 1,        /* Match on mac address and module/port
                                           or trunk or virtual port */
    BCM_VLAN_MATCH_MAC_PORT_CLASS = 2,  /* Match on Mac Address and Port-Group */
    BCM_VALN_MATCH_COUNT = 3            /* Must be Last */
} bcm_vlan_match_t;

/* Generic vlan match attribute structure */
typedef struct bcm_vlan_match_info_s {
    bcm_vlan_match_t match; /* Match criteria */
    bcm_gport_t port;       /* Match port */
    bcm_mac_t src_mac;      /* Match source mac address */
    uint32 port_class;      /* Match port class */
} bcm_vlan_match_info_t;

/* Initialize Vlan Match info structure. */
extern void bcm_vlan_match_info_t_init(
    bcm_vlan_match_info_t *vlan_match_info);

#define BCM_VLAN_MATCH_ACTION_REPLACE   (1 << 0)   /* Option to replace action
                                                      for match criteria */

#ifndef BCM_HIDE_DISPATCHABLE

/* Add action for match criteria for Vlan assignment. */
extern int bcm_vlan_match_action_add(
    int unit, 
    uint32 options, 
    bcm_vlan_match_info_t *match_info, 
    bcm_vlan_action_set_t *action_set);

/* Get action for match criteria. */
extern int bcm_vlan_match_action_get(
    int unit, 
    bcm_vlan_match_info_t *match_info, 
    bcm_vlan_action_set_t *action_set);

/* Get all actions and match criteria for given vlan match. */
extern int bcm_vlan_match_action_multi_get(
    int unit, 
    bcm_vlan_match_t match, 
    int size, 
    bcm_vlan_match_info_t *match_info_array, 
    bcm_vlan_action_set_t *action_set_array, 
    int *count);

/* Remove action for a match criteria */
extern int bcm_vlan_match_action_delete(
    int unit, 
    bcm_vlan_match_info_t *match_info);

/* Remove all actions for vlan match. */
extern int bcm_vlan_match_action_delete_all(
    int unit, 
    bcm_vlan_match_t match);

#endif /* BCM_HIDE_DISPATCHABLE */

/* VLAN Egress Translate Key Configuration. */
typedef struct bcm_vlan_translate_egress_key_config_s {
    uint32 flags; 
    bcm_vlan_translate_key_t key_type;  /* Lookup Key Type. */
    bcm_vlan_t inner_vlan;              /* Inner VLAN ID to match. */
    bcm_vlan_t outer_vlan;              /* Outer VLAN ID to match. */
    bcm_gport_t port;                   /* Port to match on. */
} bcm_vlan_translate_egress_key_config_t;

/* VLAN Egress Translation Action Set. */
typedef struct bcm_vlan_translate_egress_action_set_s {
    uint32 flags; 
    bcm_vlan_action_set_t vlan_action;  /* VLAN actions. */
} bcm_vlan_translate_egress_action_set_t;

/* bcm_vlan_translate_egress_action_extended_traverse_cb */
typedef int (*bcm_vlan_translate_egress_action_extended_traverse_cb)(
    int unit, 
    bcm_vlan_translate_egress_key_config_t *key_config, 
    bcm_vlan_translate_egress_action_set_t *action, 
    void *user_data);

/* Initialize a vlan_translate_egress_key_config_t structure. */
extern void bcm_vlan_translate_egress_key_config_t_init(
    bcm_vlan_translate_egress_key_config_t *key_config);

/* Initialize a vlan_translate_egress_action_set_t structure. */
extern void bcm_vlan_translate_egress_action_set_t_init(
    bcm_vlan_translate_egress_action_set_t *action);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Add an entry to the egress VLAN Translation table and assign VLAN
 * actions. It extends bcm_vlan_egress_action_add API for additional
 * keys.
 */
extern int bcm_vlan_translate_egress_action_extended_add(
    int unit, 
    bcm_vlan_translate_egress_key_config_t *key_config, 
    bcm_vlan_translate_egress_action_set_t *action);

/* 
 * Delete an egress translation entry including key types added by
 * bcm_vlan_translate_egress_extended_action_add API.
 */
extern int bcm_vlan_translate_egress_action_extended_delete(
    int unit, 
    bcm_vlan_translate_egress_key_config_t *key_config);

/* 
 * Retrive an egress translation entry including key types added by
 * bcm_vlan_translate_egress_extended_action_add API.
 */
extern int bcm_vlan_translate_egress_action_extended_get(
    int unit, 
    bcm_vlan_translate_egress_key_config_t *key_config, 
    bcm_vlan_translate_egress_action_set_t *action);

/* 
 * Traverse through the all egress translation entries including key
 * types added by bcm_vlan_translate_egress_extended_action_add API.
 */
extern int bcm_vlan_translate_egress_action_extended_traverse(
    int unit, 
    bcm_vlan_translate_egress_action_extended_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_VLAN_H__ */
