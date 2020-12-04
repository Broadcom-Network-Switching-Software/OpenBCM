/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_LINK_H__
#define __BCM_LINK_H__

#include <bcm/types.h>
#include <bcm/port.h>

/* bcm_linkscan_handler_t */
typedef void (*bcm_linkscan_handler_t)(
    int unit, 
    bcm_port_t port, 
    bcm_port_info_t *info);

/* bcm_linkscan_port_handler_t */
typedef int (*bcm_linkscan_port_handler_t)(
    int unit, 
    bcm_port_t port, 
    int *state);

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the link scanning subsystem. */
extern int bcm_linkscan_init(
    int unit);

/* Terminate linkscan on the specified unit. */
extern int bcm_linkscan_detach(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_LINKSCAN_INTERVAL_MIN   10000      

#define BCM_LINKSCAN_INTERVAL_DEFAULT   250000     

#ifndef BCM_HIDE_DISPATCHABLE

/* Enable and disable link scanning or set the polling interval. */
extern int bcm_linkscan_enable_set(
    int unit, 
    int us);

/* Enable and disable link scanning or set the polling interval. */
extern int bcm_linkscan_enable_get(
    int unit, 
    int *us);

/* Retrieve if linkscan managing a specified port. */
extern int bcm_linkscan_enable_port_get(
    int unit, 
    bcm_port_t port);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_linkscan_mode_e */
typedef enum bcm_linkscan_mode_e {
    BCM_LINKSCAN_MODE_NONE     = 0, 
    BCM_LINKSCAN_MODE_SW       = 1, 
    BCM_LINKSCAN_MODE_HW       = 2, 
    BCM_LINKSCAN_MODE_OVERRIDE = 3, 
    BCM_LINKSCAN_MODE_COUNT    = 4 
} bcm_linkscan_mode_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get or set the link scanning mode for a port. */
extern int bcm_linkscan_mode_set(
    int unit, 
    bcm_port_t port, 
    int mode);

/* Get or set the link scanning mode for a port. */
extern int bcm_linkscan_mode_set_pbm(
    int unit, 
    bcm_pbmp_t pbm, 
    int mode);

/* Get or set the link scanning mode for a port. */
extern int bcm_linkscan_mode_get(
    int unit, 
    bcm_port_t port, 
    int *mode);

/* Register and unregister link notification callouts. */
extern int bcm_linkscan_register(
    int unit, 
    bcm_linkscan_handler_t f);

/* Register and unregister link notification callouts. */
extern int bcm_linkscan_unregister(
    int unit, 
    bcm_linkscan_handler_t f);

/* Register and unregister link status callout. */
extern int bcm_linkscan_port_register(
    int unit, 
    bcm_port_t port, 
    bcm_linkscan_port_handler_t f);

/* Register and unregister link status callout. */
extern int bcm_linkscan_port_unregister(
    int unit, 
    bcm_port_t port, 
    bcm_linkscan_port_handler_t f);

/* Wait for a set of links to recognize link up. */
extern int bcm_link_wait(
    int unit, 
    bcm_pbmp_t *pbm, 
    int us);

/* Force a transient link down event on a port. */
extern int bcm_link_change(
    int unit, 
    bcm_pbmp_t pbmp);

/* Check for a change in link status on each link. */
extern int bcm_linkscan_update(
    int unit, 
    bcm_pbmp_t pbm);

#if defined(BROADCOM_DEBUG)
/* Display information about linkscan callbacks. */
extern int bcm_linkscan_dump(
    int unit);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_linkscan_trigger_event_e */
typedef enum bcm_linkscan_trigger_event_e {
    BCM_LINKSCAN_TRIGGER_EVENT_FAULT = 0, /* Trigger only link remote fault
                                           checking in Linkscan thread. */
    BCM_LINKSCAN_TRIGGER_EVENT_REMOTE_FAULT = BCM_LINKSCAN_TRIGGER_EVENT_FAULT, /* Trigger link remote fault checking. */
    BCM_LINKSCAN_TRIGGER_EVENT_LOCAL_FAULT = 1 /* Trigger link local fault checking. */
} bcm_linkscan_trigger_event_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get/set the link event from/to Linkscan function. */
extern int bcm_linkscan_trigger_event_set(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    bcm_linkscan_trigger_event_t trigger_event, 
    int enable);

/* Get/set the link event from/to Linkscan function. */
extern int bcm_linkscan_trigger_event_get(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    bcm_linkscan_trigger_event_t trigger_event, 
    int *enable);

/* Get/set the override link state for the specified port. */
extern int bcm_linkscan_override_link_state_set(
    int unit, 
    bcm_port_t port, 
    int link);

/* Get/set the override link state for the specified port. */
extern int bcm_linkscan_override_link_state_get(
    int unit, 
    bcm_port_t port, 
    int *link);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_LINK_H__ */
