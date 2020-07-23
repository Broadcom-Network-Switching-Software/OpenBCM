/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_STG_H__
#define __BCM_STG_H__

#include <shared/port.h>
#include <bcm/types.h>

/* bcm_stg_stp_e */
typedef enum bcm_stg_stp_e {
    BCM_STG_STP_DISABLE = _SHR_PORT_STP_DISABLE, /* Disabled. */
    BCM_STG_STP_BLOCK = _SHR_PORT_STP_BLOCK, /* BPDUs/no learns. */
    BCM_STG_STP_LISTEN = _SHR_PORT_STP_LISTEN, /* BPDUs/no learns. */
    BCM_STG_STP_LEARN = _SHR_PORT_STP_LEARN, /* BPDUs/learns. */
    BCM_STG_STP_FORWARD = _SHR_PORT_STP_FORWARD, /* Normal operation. */
    BCM_STG_STP_COUNT = _SHR_PORT_STP_COUNT 
} bcm_stg_stp_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Initialize the Spanning Tree Group (STG) module to its initial
 * configuration.
 */
extern int bcm_stg_init(
    int unit);

/* 
 * Destroy all STGs and initialize the Spanning Tree Group (STG) module
 * to its initial configuration.
 */
extern int bcm_stg_clear(
    int unit);

/* Designate the default STG ID for the chip. */
extern int bcm_stg_default_set(
    int unit, 
    bcm_stg_t stg);

/* Return the current default STG ID for the chip. */
extern int bcm_stg_default_get(
    int unit, 
    bcm_stg_t *stg_ptr);

/* Add a VLAN to a specified STG. */
extern int bcm_stg_vlan_add(
    int unit, 
    bcm_stg_t stg, 
    bcm_vlan_t vid);

/* Remove a VLAN from a Spanning Tree Group. */
extern int bcm_stg_vlan_remove(
    int unit, 
    bcm_stg_t stg, 
    bcm_vlan_t vid);

/* Remove all VLANs from a Spanning Tree Group. */
extern int bcm_stg_vlan_remove_all(
    int unit, 
    bcm_stg_t stg);

/* Generates a list of VLANs in a specified STG. */
extern int bcm_stg_vlan_list(
    int unit, 
    bcm_stg_t stg, 
    bcm_vlan_t **list, 
    int *count);

/* Destroy a list returned by \ref bcm_stg_vlan_list . */
extern int bcm_stg_vlan_list_destroy(
    int unit, 
    bcm_vlan_t *list, 
    int count);

/* Create a new STG, using a new STG ID. */
extern int bcm_stg_create(
    int unit, 
    bcm_stg_t *stg_ptr);

/* Create a new STG, using a specific STG ID. */
extern int bcm_stg_create_id(
    int unit, 
    bcm_stg_t stg);

/* Destroys the specified STG. */
extern int bcm_stg_destroy(
    int unit, 
    bcm_stg_t stg);

/* Generates a list of IDs for currently defined STGs. */
extern int bcm_stg_list(
    int unit, 
    bcm_stg_t **list, 
    int *count);

/* 
 * Destroys a previously returned list of STG IDs; the STGs themselves
 * are not affected.
 */
extern int bcm_stg_list_destroy(
    int unit, 
    bcm_stg_t *list, 
    int count);

/* 
 * Set the Spanning Tree Protocol state of a port/VP in the specified
 * STG.
 *  VP GROUP STP state need to be setted per direction, Please use \ref
 * bcm_vlan_gport_add for this.
 */
extern int bcm_stg_stp_set(
    int unit, 
    bcm_stg_t stg, 
    bcm_port_t port, 
    int stp_state);

/* 
 * Return the Spanning Tree Protocol state of a port in the specified
 * STG.
 */
extern int bcm_stg_stp_get(
    int unit, 
    bcm_stg_t stg, 
    bcm_port_t port, 
    int *stp_state);

/* 
 * Return the maximum number of STGs that the underlying hardware can
 * support
 */
extern int bcm_stg_count_get(
    int unit, 
    int *max_stg);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_STG_H__ */
