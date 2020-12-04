/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_WLAN_H__
#define __BCM_WLAN_H__

#if defined(INCLUDE_L3)

#include <bcm/types.h>
#include <bcm/multicast.h>
#include <bcm/l3.h>
#include <bcm/tunnel.h>

/* WLAN Port Flags. */
#define BCM_WLAN_PORT_WITH_ID               0x00000001 
#define BCM_WLAN_PORT_REPLACE               0x00000002 
#define BCM_WLAN_PORT_DROP                  0x00000004 
#define BCM_WLAN_PORT_NETWORK               0x00000008 
#define BCM_WLAN_PORT_BSSID                 0x00000010 
#define BCM_WLAN_PORT_BSSID_RADIO           0x00000020 
#define BCM_WLAN_PORT_MATCH_TUNNEL          0x00000040 
#define BCM_WLAN_PORT_EGRESS_CLIENT_MULTICAST 0x00000080 
#define BCM_WLAN_PORT_EGRESS_BSSID          0x00000100 
#define BCM_WLAN_PORT_EGRESS_TUNNEL_REMOTE  0x00000200 
#define BCM_WLAN_PORT_ROAM_ENABLE           0x00000400 

/* WLAN Port structure. */
typedef struct bcm_wlan_port_s {
    uint32 flags;               /* Configuration flags */
    bcm_gport_t wlan_port_id;   /* WLAN port number */
    bcm_gport_t port;           /* Physical port / trunk. */
    int if_class;               /* Interface class ID. */
    bcm_mac_t bssid;            /* BSSID */
    int radio;                  /* Radio ID */
    bcm_gport_t match_tunnel;   /* Ingress Tunnel ID */
    bcm_gport_t egress_tunnel;  /* Egress Tunnel ID */
    uint16 client_multicast;    /* Client multicast bitmap */
    bcm_if_t encap_id;          /* Encap identifier. */
} bcm_wlan_port_t;

/* WLAN Client Database Flags. */
#define BCM_WLAN_CLIENT_REPLACE     0x00000001 
#define BCM_WLAN_CLIENT_ROAMED_IN   0x00000002 
#define BCM_WLAN_CLIENT_ROAMED_OUT  0x00000004 
#define BCM_WLAN_CLIENT_AUTHORIZED  0x00000008 
#define BCM_WLAN_CLIENT_CAPTIVE     0x00000010 

/* WLAN Client structure. */
typedef struct bcm_wlan_client_s {
    uint32 flags;               /* Configuration flags */
    bcm_gport_t wlan_port_id;   /* WLAN port number */
    bcm_mac_t mac;              /* Client MAC Address */
    bcm_gport_t home_agent;     /* The home agent */
    bcm_gport_t wtp;            /* The access point */
} bcm_wlan_client_t;

/* Initialize the WLAN port structure. */
extern void bcm_wlan_port_t_init(
    bcm_wlan_port_t *wlan_port);

/* Initialize the WLAN client structure. */
extern void bcm_wlan_client_t_init(
    bcm_wlan_client_t *wlan_client);

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the BCM WLAN subsystem. */
extern int bcm_wlan_init(
    int unit);

/* Detach the WLAN software module. */
extern int bcm_wlan_detach(
    int unit);

/* Add a new WLAN port or update an existing one. */
extern int bcm_wlan_port_add(
    int unit, 
    bcm_wlan_port_t *info);

/* Delete a WLAN port or all WLAN ports. */
extern int bcm_wlan_port_delete(
    int unit, 
    bcm_gport_t wlan_port_id);

/* Delete a WLAN port or all WLAN ports. */
extern int bcm_wlan_port_delete_all(
    int unit);

/* Get a WLAN port. */
extern int bcm_wlan_port_get(
    int unit, 
    bcm_gport_t wlan_port_id, 
    bcm_wlan_port_t *info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* WLAN port callback function prototype. */
typedef int (*bcm_wlan_port_traverse_cb)(
    int unit, 
    bcm_wlan_port_t *info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse all WLAN ports. */
extern int bcm_wlan_port_traverse(
    int unit, 
    bcm_wlan_port_traverse_cb cb, 
    void *user_data);

/* Add a wireless client to the database. */
extern int bcm_wlan_client_add(
    int unit, 
    bcm_wlan_client_t *info);

/* Delete a client or all clients from the database. */
extern int bcm_wlan_client_delete(
    int unit, 
    bcm_mac_t mac);

/* Delete a client or all clients from the database. */
extern int bcm_wlan_client_delete_all(
    int unit);

/* Get information about a WLAN client. */
extern int bcm_wlan_client_get(
    int unit, 
    bcm_mac_t mac, 
    bcm_wlan_client_t *info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* WLAN client callback function prototype. */
typedef int (*bcm_wlan_client_traverse_cb)(
    int unit, 
    bcm_wlan_client_t *info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse all WLAN clients. */
extern int bcm_wlan_client_traverse(
    int unit, 
    bcm_wlan_client_traverse_cb cb, 
    void *user_data);

/* Set the tunneling initiator parameters for a WLAN tunnel. */
extern int bcm_wlan_tunnel_initiator_create(
    int unit, 
    bcm_tunnel_initiator_t *info);

/* Delete an outgoing WLAN tunnel. */
extern int bcm_wlan_tunnel_initiator_destroy(
    int unit, 
    bcm_gport_t wlan_tunnel_id);

/* Get the tunnel properties for an outgoing WLAN tunnel. */
extern int bcm_wlan_tunnel_initiator_get(
    int unit, 
    bcm_tunnel_initiator_t *info);

#endif /* defined(INCLUDE_L3) */

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_WLAN_H__ */
