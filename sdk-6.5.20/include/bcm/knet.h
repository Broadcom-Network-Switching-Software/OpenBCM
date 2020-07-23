/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_KNET_H__
#define __BCM_KNET_H__

#include <bcm/types.h>
#include <bcm/rx.h>

/* Network interface types */
#define BCM_KNET_NETIF_T_TX_CPU_INGRESS 1          /* Tx packets are sent
                                                      through CPU port ingress
                                                      logic. */
#define BCM_KNET_NETIF_T_TX_LOCAL_PORT  2          /* Tx packets are sent to
                                                      local device port
                                                      bypassing switch logic. */
#define BCM_KNET_NETIF_T_TX_META_DATA   3          /* Tx packets are sent using
                                                      raw meta data as DMA
                                                      descriptors. */

/* Network interface flags */
#define BCM_KNET_NETIF_F_ADD_TAG        0x00000001 /* Add VLAN tag to packets
                                                      sent directly to physical
                                                      port. */
#define BCM_KNET_NETIF_F_RCPU_ENCAP     0x00000002 /* Use RCPU encapsulation for
                                                      packets that enter and
                                                      exit this interface. */
#define BCM_KNET_NETIF_F_KEEP_RX_TAG    0x00000004 /* Do nothing to the tag of
                                                      packets which will be sent
                                                      to this interface. */

#define BCM_KNET_NETIF_NAME_MAX 16         

/* Kernel network interface structure. */
typedef struct bcm_knet_netif_s {
    int id;                             /* Network interface ID. */
    int type;                           /* Network interface type
                                           (BCM_KNET_NETIF_T_XXX). */
    uint32 flags;                       /* BCM_KNET_NETIF_F_XXX flags. */
    bcm_mac_t mac_addr;                 /* MAC address associated with this
                                           network interface. */
    bcm_vlan_t vlan;                    /* VLAN ID associated with this
                                           interface. */
    bcm_port_t port;                    /* Local port associated with this
                                           interface. */
    bcm_cos_queue_t cosq;               /* Cos offset from base queue of port
                                           associated with this interface. */
    char name[BCM_KNET_NETIF_NAME_MAX]; /* Network interface name (assigned by
                                           kernel) */
    uint32 cb_user_data;                /* Netif user data for knet rx cb. */
} bcm_knet_netif_t;

/* Filter types */
#define BCM_KNET_FILTER_T_RX_PKT    1          /* Rx packet filter. */

/* Filter destinstion types */
#define BCM_KNET_DEST_T_NULL        0          /* Null destination (drop
                                                  packet). */
#define BCM_KNET_DEST_T_NETIF       1          /* Send packet to virtual network
                                                  interface. */
#define BCM_KNET_DEST_T_BCM_RX_API  2          /* Send packet to BCM Rx API. */
#define BCM_KNET_DEST_T_CALLBACK    3          /* Send packet to kernel
                                                  call-back function. */

/* Filter flags */
#define BCM_KNET_FILTER_F_STRIP_TAG 0x00000001 /* Strip VLAN tag from packets
                                                  sent to vitual network
                                                  interfaces. */

#define BCM_KNET_FILTER_DESC_MAX    32         

#define BCM_KNET_FILTER_SIZE_MAX    256        

/* Match criteria selection */
#define BCM_KNET_FILTER_M_RAW           0x00000001 /* Match raw packet data */
#define BCM_KNET_FILTER_M_VLAN          0x00000002 /* Match VLAN ID */
#define BCM_KNET_FILTER_M_INGPORT       0x00000004 /* Match local ingress port */
#define BCM_KNET_FILTER_M_SRC_MODPORT   0x00000008 /* Match source module port */
#define BCM_KNET_FILTER_M_SRC_MODID     0x00000010 /* Match source module ID */
#define BCM_KNET_FILTER_M_REASON        0x00000020 /* Match copy-to-CPU reason
                                                      code */
#define BCM_KNET_FILTER_M_FP_RULE       0x00000040 /* Match filter processor
                                                      rule number */
#define BCM_KNET_FILTER_M_ERROR         0x00000080 /* Match error bit */

/* Kernel packet filter structure. */
typedef struct bcm_knet_filter_s {
    int id;                             /* Filter ID. */
    int type;                           /* Filter type (BCM_KNET_FILTER_T_XXX). */
    uint32 flags;                       /* BCM_KNET_FILTER_F_XXX flags. */
    int priority;                       /* Filter priority, lower value has
                                           higher priority, value of RX channels
                                           are reserved to bind filter to
                                           corresponding RX channel by default. */
    int dest_type;                      /* Filter destination type. */
    int dest_id;                        /* Filter destination ID. */
    int dest_proto;                     /* If non-zero this value overrides the
                                           default protocol type when matching
                                           packet is passed to network stack. */
    int mirror_type;                    /* Mirror destination type. */
    int mirror_id;                      /* Mirror destination ID. */
    int mirror_proto;                   /* If non-zero this value overrides the
                                           default protocol type when matching
                                           packet is passed to network stack. */
    uint32 match_flags;                 /* BCM_KNET_FILTER_M_XXX flags. */
    char desc[BCM_KNET_FILTER_DESC_MAX]; /* Filter description (optional) */
    bcm_vlan_t m_vlan;                  /* VLAN ID to match. */
    bcm_port_t m_ingport;               /* Local ingress port to match. */
    int m_src_modport;                  /* Source module port to match. */
    int m_src_modid;                    /* Source module ID to match. */
    bcm_rx_reasons_t m_reason;          /* Copy-to-CPU reason to match. */
    int m_fp_rule;                      /* Filter processor rule to match. */
    int raw_size;                       /* Size of valid raw data and mask. */
    uint8 m_raw_data[BCM_KNET_FILTER_SIZE_MAX]; /* Raw data to match. */
    uint8 m_raw_mask[BCM_KNET_FILTER_SIZE_MAX]; /* Raw data mask for match. */
    uint32 cb_user_data;                /* Filter user data for knet rx cb. */
} bcm_knet_filter_t;

/* Initialize a bcm_knet_netif_t structure. */
extern void bcm_knet_netif_t_init(
    bcm_knet_netif_t *netif);

/* Initialize a bcm_knet_filter_t structure. */
extern void bcm_knet_filter_t_init(
    bcm_knet_filter_t *filter);

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the kernel networking subsystem. */
extern int bcm_knet_init(
    int unit);

/* Clean up the kernel networking subsystem. */
extern int bcm_knet_cleanup(
    int unit);

/* Create a kernel network interface. */
extern int bcm_knet_netif_create(
    int unit, 
    bcm_knet_netif_t *netif);

/* Destroy a kernel network interface. */
extern int bcm_knet_netif_destroy(
    int unit, 
    int netif_id);

/* Get a kernel network interface configuration. */
extern int bcm_knet_netif_get(
    int unit, 
    int netif_id, 
    bcm_knet_netif_t *netif);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Kernel network interface traverse callback */
typedef int (*bcm_knet_netif_traverse_cb)(
    int unit, 
    bcm_knet_netif_t *netif, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse kernel network interface objects */
extern int bcm_knet_netif_traverse(
    int unit, 
    bcm_knet_netif_traverse_cb trav_fn, 
    void *user_data);

/* Create a kernel packet filter. */
extern int bcm_knet_filter_create(
    int unit, 
    bcm_knet_filter_t *filter);

/* Destroy a kernel packet filter. */
extern int bcm_knet_filter_destroy(
    int unit, 
    int filter_id);

/* Get a kernel packet filter configuration. */
extern int bcm_knet_filter_get(
    int unit, 
    int filter_id, 
    bcm_knet_filter_t *filter);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Kernel packet filter traverse callback */
typedef int (*bcm_knet_filter_traverse_cb)(
    int unit, 
    bcm_knet_filter_t *filter, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse kernel packet filter objects */
extern int bcm_knet_filter_traverse(
    int unit, 
    bcm_knet_filter_traverse_cb trav_fn, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_KNET_H__ */
