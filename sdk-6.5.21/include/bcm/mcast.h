/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_MCAST_H__
#define __BCM_MCAST_H__

#include <bcm/types.h>

/* BCM_MCAST_XXX Flags. */
#define BCM_MCAST_USE_FABRIC_DISTRIBUTION   0x00000001 /* Use specified fabric
                                                          distribution class. */

/* Device-independent L2 multicast address. */
typedef struct bcm_mcast_addr_s {
    bcm_mac_t mac;                      /* 802.3 MAC address. */
    bcm_vlan_t vid;                     /* VLAN identifier. */
    bcm_cos_t cos_dst;                  /* COS based on destination address. */
    bcm_pbmp_t pbmp;                    /* Port bitmap. */
    bcm_pbmp_t ubmp;                    /* Untagged port bitmap. */
    uint32 l2mc_index;                  /* XGS: L2MC index. */
    uint32 flags;                       /* See BCM_MCAST_XXX flag definitions. */
    bcm_fabric_distribution_t distribution_class; /* Fabric Distribution Class. */
} bcm_mcast_addr_t;

/* Values returned by bcm_mcast_join and bcm_mcast_leave routines. */
#define BCM_MCAST_JOIN_ADDED        1          
#define BCM_MCAST_JOIN_UPDATED      2          
#define BCM_MCAST_LEAVE_DELETED     3          
#define BCM_MCAST_LEAVE_UPDATED     4          
#define BCM_MCAST_LEAVE_NOTFOUND    5          

/* Initialize a bcm_mcast_addr_t data type. */
extern void bcm_mcast_addr_t_init(
    bcm_mcast_addr_t *mcaddr, 
    bcm_mac_t mac, 
    bcm_vlan_t vid);

/* Backward compatibility. */
#define bcm_mcast_addr_init     bcm_mcast_addr_t_init 

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the L2 multicast module. */
extern int bcm_mcast_init(
    int unit);

/* Add/Remove an entry in the multicast table. */
extern int bcm_mcast_addr_add(
    int unit, 
    bcm_mcast_addr_t *mcaddr);

/* Add/Remove an entry in the multicast table. */
extern int bcm_mcast_addr_remove(
    int unit, 
    bcm_mac_t mac, 
    bcm_vlan_t vid);

/* 
 * Get untagged and tagged membership port bit maps for a Multicast
 * group.
 */
extern int bcm_mcast_port_get(
    int unit, 
    bcm_mac_t mac, 
    bcm_vlan_t vid, 
    bcm_mcast_addr_t *mcaddr);

/* Add a given port to the membership of a given multicast group. */
extern int bcm_mcast_join(
    int unit, 
    bcm_mac_t mcMacAddr, 
    bcm_vlan_t vlanId, 
    int srcPort, 
    bcm_mcast_addr_t *mcaddr, 
    bcm_pbmp_t *allRouterBmp);

/* Remove a given port from the membership of a given multicast group. */
extern int bcm_mcast_leave(
    int unit, 
    bcm_mac_t mcMacAddr, 
    bcm_vlan_t vlanId, 
    int srcPort);

/* 
 * Add/Update an entry in the multicast table with l2mc index provided.
 * Remove an entry in the multicast table, l2mc index not needed.
 */
extern int bcm_mcast_addr_remove_w_l2mcindex(
    int unit, 
    bcm_mcast_addr_t *mcaddr);

/* 
 * Add/Update an entry in the multicast table with l2mc index provided.
 * Remove an entry in the multicast table, l2mc index not needed.
 */
extern int bcm_mcast_addr_add_w_l2mcindex(
    int unit, 
    bcm_mcast_addr_t *mcaddr);

/* Add/Remove ports to/from an existing entry in the multicast table. */
extern int bcm_mcast_port_remove(
    int unit, 
    bcm_mcast_addr_t *mcaddr);

/* Add/Remove ports to/from an existing entry in the multicast table. */
extern int bcm_mcast_port_add(
    int unit, 
    bcm_mcast_addr_t *mcaddr);

/* Provides the maximum multicast index that this fabric can handle. */
extern int bcm_mcast_bitmap_max_get(
    int unit, 
    int *max_index);

/* Set/Get/Delete the L2 multicast forwarding port bitmap. */
extern int bcm_mcast_bitmap_set(
    int unit, 
    int mc_idx, 
    bcm_port_t in_port, 
    bcm_pbmp_t pbmp);

/* Set/Get/Delete the L2 multicast forwarding port bitmap. */
extern int bcm_mcast_bitmap_get(
    int unit, 
    int mc_idx, 
    bcm_port_t in_port, 
    bcm_pbmp_t *pbmp);

/* Set/Get/Delete the L2 multicast forwarding port bitmap. */
extern int bcm_mcast_bitmap_del(
    int unit, 
    int mc_idx, 
    bcm_port_t in_port, 
    bcm_pbmp_t pbmp);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_MCAST_GPORT_IS_BITMAP_ID(gport)  _SHR_MCAST_GPORT_IS_BITMAP_ID(gport) 
#define BCM_MCAST_GPORT_IS_GROUP_ID(gport)  _SHR_MCAST_GPORT_IS_GROUP_ID(gport) 
#define BCM_MCAST_GPORT_GROUP_ID_SET(gport, mcast_id)  _SHR_MCAST_GPORT_GROUP_ID_SET(gport, mcast_id) 
#define BCM_MCAST_GPORT_BITMAP_ID_SET(gport, bmp_id)  _SHR_MCAST_GPORT_BITMAP_ID_SET(gport, bmp_id) 
#define BCM_MCAST_GPORT_GROUP_ID_GET(gport)  _SHR_MCAST_GPORT_GROUP_ID_GET(gport) 
#define BCM_MCAST_GPORT_BITMAP_ID_GET(gport)  _SHR_MCAST_GPORT_BITMAP_ID_GET(gport) 

#endif /* __BCM_MCAST_H__ */
