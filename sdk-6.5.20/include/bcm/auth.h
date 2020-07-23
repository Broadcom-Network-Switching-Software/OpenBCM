/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_AUTH_H__
#define __BCM_AUTH_H__

#include <bcm/types.h>

#define BCM_AUTH_MODE_UNCONTROLLED  0x00000001 
#define BCM_AUTH_MODE_UNAUTH        0x00000002 
#define BCM_AUTH_MODE_AUTH          0x00000004 

#define BCM_AUTH_BLOCK_IN       0x00000008 
#define BCM_AUTH_BLOCK_INOUT    0x00000010 

#define BCM_AUTH_LEARN              0x00000020 
#define BCM_AUTH_IGNORE_LINK        0x00000040 
#define BCM_AUTH_IGNORE_VIOLATION   0x00000080 
#define BCM_AUTH_DROP_UNKNOWN       0x00000100 

#define BCM_AUTH_REASON_UNKNOWN     0x00002000 
#define BCM_AUTH_REASON_LINK        0x00004000 
#define BCM_AUTH_REASON_VIOLATION   0x00008000 

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize bcm_auth API module */
extern int bcm_auth_init(
    int unit);

/* De-configure bcm_auth API module. */
extern int bcm_auth_detach(
    int unit);

/* Sets or gets authorization operating mode */
extern int bcm_auth_mode_set(
    int unit, 
    int port, 
    uint32 mode);

/* Sets or gets authorization operating mode */
extern int bcm_auth_mode_get(
    int unit, 
    int port, 
    uint32 *modep);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Definition for 802.1X callout functions. */
typedef void (*bcm_auth_cb_t)(
    void *cookie, 
    int unit, 
    int port, 
    int reason);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Register a callback function to be called when a port
 * becomes asynchronously unauthorized.
 */
extern int bcm_auth_unauth_callback(
    int unit, 
    bcm_auth_cb_t func, 
    void *cookie);

/* Enable/disable the ability of packets to be sent out a port. */
extern int bcm_auth_egress_set(
    int unit, 
    int port, 
    int enable);

/* Enable/disable the ability of packets to be sent out a port. */
extern int bcm_auth_egress_get(
    int unit, 
    int port, 
    int *enable);

/* Add Switch's MAC address. */
extern int bcm_auth_mac_add(
    int unit, 
    int port, 
    bcm_mac_t mac);

/* Remove Switch's MAC address. */
extern int bcm_auth_mac_delete(
    int unit, 
    int port, 
    bcm_mac_t mac);

/* Remove Switch's MAC address. */
extern int bcm_auth_mac_delete_all(
    int unit, 
    int port);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Features that can be controlled for EAP packets. */
typedef enum bcm_auth_mac_control_e {
    bcmEapControlL2UserAddr = 0,    /* enable L2 User Address frame bypass EAP
                                       Port State Filter and SA Filter. */
    bcmEapControlDHCP = 1,          /* enable DHCP frame bypass EAP Port State
                                       Filter and SA Filter. */
    bcmEapControlARP = 2,           /* enable ARP frame bypass EAP Port State
                                       Filter and SA Filter. */
    bcmEapControlMAC2X = 3,         /* enable(DA=01-80-c2-00-00-22,23,....,2f)
                                       frame bypass EAP Port State Filter and SA
                                       Filter. */
    bcmEapControlGVRP = 4,          /* enable(DA=01-80-c2-00-00-21) frame bypass
                                       EAP Port State FIlter and SA Filter. */
    bcmEapControlGMRP = 5,          /* enable(DA=01-80-c2-00-00-20) frame bypass
                                       EAP Port State FIlter and SA Filter. */
    bcmEapControlMAC1X = 6,         /* enable(DA=01-80-c2-00-00-11,12,....,1f)
                                       frame bypass EAP Port State FIlter and SA
                                       Filter. */
    bcmEapControlAllBridges = 7,    /* enable (DA=01-80-c2-00-00-10) frame
                                       bypass EAP Port State FIlter and SA
                                       Filter. */
    bcmEapControlMAC0X = 8,         /* enable (DA=01-80-c2-00-00-02) or
                                       (DA=01-80-c2-00-00-04,05,....,0f) frame
                                       bypass EAP Port State FIlter and SA
                                       Filter. */
    bcmEapControlMACBPDU = 9        /* enable BPDU frame bypass EAP Port State
                                       FIlter and SA Filter. */
} bcm_auth_mac_control_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Enable or disable to bypass the EAP MAC control type. */
extern int bcm_auth_mac_control_set(
    int unit, 
    bcm_auth_mac_control_t type, 
    uint32 value);

/* Enable or disable to bypass the EAP MAC control type. */
extern int bcm_auth_mac_control_get(
    int unit, 
    bcm_auth_mac_control_t type, 
    uint32 *value);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_AUTH_H__ */
