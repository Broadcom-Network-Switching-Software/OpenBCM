/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_IPFIX_H__
#define __BCM_IPFIX_H__

#include <bcm/types.h>

#define BCM_IPFIX_DSCP_MASK_COUNT   64         

typedef enum bcm_ipfix_stage_e {
    BCM_IPFIX_STAGE_INGRESS = 0, 
    BCM_IPFIX_STAGE_EGRESS  = 1 
} bcm_ipfix_stage_t;

/* Flags for bcm_ipfix_data_t.flags. */
#define BCM_IPFIX_DATA_VLAN_TAGGED          0x0001     
#define BCM_IPFIX_DATA_TYPE_L2              0x0002     
#define BCM_IPFIX_DATA_TYPE_IP4             0x0004     
#define BCM_IPFIX_DATA_TYPE_IP6             0x0008     
#define BCM_IPFIX_DATA_REASON_FLOW_TIMEOUT  0x0010     
#define BCM_IPFIX_DATA_REASON_FLOW_END      0x0020     
#define BCM_IPFIX_DATA_REASON_FLOW          0x0040     
#define BCM_IPFIX_DATA_REASON_COUNT         0x0080     
#define BCM_IPFIX_DATA_REASON_TIMESTAMP     0x0100     
#define BCM_IPFIX_DATA_RATE_VIOLATION       0x0200     
#define BCM_IPFIX_DATA_NON_FRAGMENT_PKT_DETECTED 0x0400     
#define BCM_IPFIX_DATA_FRAGMENT_PKT_DETECTED 0x0800     
#define BCM_IPFIX_DATA_TTL_CHANGED_DETECTED 0x1000     
#define BCM_IPFIX_DATA_DROP_DETECTED        0x2000     
#define BCM_IPFIX_DATA_TYPE_MASK            0x000e     
#define BCM_IPFIX_DATA_REASON_MASK          0x01f0     

/* Export data structure */
typedef struct bcm_ipfix_data_s {
    bcm_ipfix_stage_t stage;        /* Ingress or egress. */
    uint32 flags;                   /* Type and reason. */
    bcm_port_t port;                /* port. */
    bcm_vlan_t vlan;                /* L2: VLAN identifier. */
    uint16 vlan_prio;               /* L2: VLAN priority. */
    uint16 ether_type;              /* L2: type in Ethernet II frame or 802.1Q
                                       tag. */
    bcm_mac_t src_mac_addr;         /* L2: source MAC address. */
    bcm_mac_t dst_mac_addr;         /* L2: destination MAC address. */
    bcm_ip_t src_ip4_addr;          /* IPv4: source IP address. */
    bcm_ip_t dst_ip4_addr;          /* IPv4: destination IP address. */
    bcm_ip6_t src_ip6_addr;         /* IPv6: source IP address. */
    bcm_ip6_t dst_ip6_addr;         /* IPv6: destination IP address. */
    uint16 ip_protocol;             /* IPv4: protocol; IPv6: next header. */
    uint16 tos;                     /* IPv4: type of service; IPv6: traffic
                                       class. */
    uint32 ip6_flow_label;          /* IPv6: flow label. */
    uint16 l4_src_port;             /* First 16 bits of L4 header such as TCP
                                       source port or ICMP type and code. */
    uint16 l4_dst_port;             /* Second 16 bits of L4 header such as TCP
                                       destination port. */
    bcm_gport_t source_port;        /* Source port. */
    uint32 start_timestamp;         /* Session information. */
    uint32 last_timestamp;          /* Session information. */
    uint32 byte_count;              /* Session information. */
    uint32 pkt_count;               /* Session information. */
    uint32 ttl;                     /* TTL value of the first packet. */
    uint32 tcp_flags;               /* TCP flags. */
    bcm_gport_t dest_port;          /* Destination. */
    bcm_vlan_t egress_vlan;         /* Egress VLAN. */
    bcm_multicast_t dest_multicast; /* Destination multicast group. */
} bcm_ipfix_data_t;

/* Flags for bcm_ipfix_config_t.flags. */
#define BCM_IPFIX_CONFIG_ENABLE_NON_IP      0x00000001 
#define BCM_IPFIX_CONFIG_ENABLE_IP4         0x00000002 
#define BCM_IPFIX_CONFIG_ENABLE_IP6         0x00000004 
#define BCM_IPFIX_CONFIG_TCP_END_DETECT     0x00000008 
#define BCM_IPFIX_CONFIG_RECORD_NON_DISCARD_PKT 0x00000010 
#define BCM_IPFIX_CONFIG_RECORD_DISCARD_PKT 0x00000020 
#define BCM_IPFIX_CONFIG_KEY_IP4_USE_L2     0x00000040 
#define BCM_IPFIX_CONFIG_KEY_IP6_USE_L2     0x00000080 
#define BCM_IPFIX_CONFIG_KEY_SRC_IP         0x00000100 
#define BCM_IPFIX_CONFIG_KEY_DST_IP         0x00000200 
#define BCM_IPFIX_CONFIG_KEY_IP_PROT        0x00000400 
#define BCM_IPFIX_CONFIG_KEY_IP_DSCP        0x00000800 
#define BCM_IPFIX_CONFIG_KEY_IP_ECN         0x00001000 
#define BCM_IPFIX_CONFIG_KEY_L4_SRC_PORT    0x00002000 
#define BCM_IPFIX_CONFIG_KEY_L4_DST_PORT    0x00004000 
#define BCM_IPFIX_CONFIG_KEY_IP6_FLOW       0x00008000 
#define BCM_IPFIX_CONFIG_KEY_ICMP_TYPE      0x00010000 
#define BCM_IPFIX_CONFIG_KEY_ICMP_CODE      0x00020000 
#define BCM_IPFIX_CONFIG_KEY_MACDA          0x00040000 
#define BCM_IPFIX_CONFIG_KEY_MACSA          0x00080000 
#define BCM_IPFIX_CONFIG_KEY_VLAN_ID        0x00100000 
#define BCM_IPFIX_CONFIG_KEY_VLAN_PRI       0x00200000 
#define BCM_IPFIX_CONFIG_KEY_ETHER_TYPE     0x00400000 
#define BCM_IPFIX_CONFIG_KEY_VLAN_TAGGED    0x00800000 
#define BCM_IPFIX_CONFIG_TCP_FLAGS_LAST     0x01000000 
#define BCM_IPFIX_CONFIG_KEY_SOURCE_PORT_OR_INTERFACE 0x02000000 
#define BCM_IPFIX_CONFIG_ENABLE_MASK        0x00000007 
#define BCM_IPFIX_CONFIG_KEY_MASK           0x02ffffc0 

/* IPFIX port configuration structure */
typedef struct bcm_ipfix_config_s {
    uint32 flags;                       /* Fields select and other flags. */
    uint8 dscp_mask[BCM_IPFIX_DSCP_MASK_COUNT]; /* DSCP value translation mapping. */
    bcm_ip_t src_ip4_mask;              /* Mask to map IPv4 address to key
                                           value. */
    bcm_ip_t dst_ip4_mask;              /* Mask to map IPv4 address to key
                                           value. */
    bcm_ip_t tunnel_src_ip4_mask;       /* Mask to map IPv4 address to key
                                           value. */
    bcm_ip_t tunnel_dst_ip4_mask;       /* Mask to map IPv4 address to key
                                           value. */
    bcm_ip6_t src_ip6_mask;             /* Mask to map IPv6 address to key
                                           value. */
    bcm_ip6_t dst_ip6_mask;             /* Mask to map IPv6 address to key
                                           value. */
    bcm_ip6_t tunnel_src_ip6_mask;      /* Mask to map IPv6 address to key
                                           value. */
    bcm_ip6_t tunnel_dst_ip6_mask;      /* Mask to map IPv6 address to key
                                           value. */
    uint32 entry_limit;                 /* Maximum number of flow entry
                                           collected for the port. */
    uint32 min_time;                    /* The flow will not be exported unless
                                           the flow has been established for
                                           more than min_time (unit is 10 ms). */
    uint32 max_time;                    /* The flow will not be exported unless
                                           the flow has been established for
                                           less than max_time (unit is 10 ms). */
    uint32 max_idle_time;               /* The flow will be exported and
                                           terminated if idle for more than
                                           max_idle_time (unit is 10 ms). */
    uint32 sample_rate;                 /* Collect one packet info for every
                                           sample_rate packets. */
} bcm_ipfix_config_t;

/* bcm_ipfix_callback_t */
typedef void (*bcm_ipfix_callback_t)(
    int unit, 
    bcm_ipfix_data_t *info, 
    void *userdata);

/* Flags for bcm_ipfix_rate_t.flags */
#define BCM_IPFIX_RATE_VIOLATION_WITH_ID    0x00000001 
#define BCM_IPFIX_RATE_VIOLATION_REPLACE    0x00000002 
#define BCM_IPFIX_RATE_VIOLATION_DROP       0x00000004 
#define BCM_IPFIX_RATE_VIOLATION_COPY_TO_CPU 0x00000008 
#define BCM_IPFIX_RATE_VIOLATION_DSCP_SET   0x00000010 
#define BCM_IPFIX_RATE_VIOLATION_COLOR_SET  0x00000020 
#define BCM_IPFIX_RATE_VIOLATION_PKT_PRI_SET 0x00000040 
#define BCM_IPFIX_RATE_VIOLATION_INT_PRI_SET 0x00000080 

/* Flow rate meter entry */
typedef struct bcm_ipfix_rate_s {
    bcm_ipfix_rate_id_t rate_id;    /* Flow rate meter id. */
    uint32 flags;                   /* Configuration flags. */
    uint32 count;                   /* Flow count (for get only). */
    uint32 limit;                   /* Max number of good flows to allow. */
    uint32 rate;                    /* Number of good flows per second. */
    uint8 dscp;                     /* New DSCP value. */
    bcm_color_t color;              /* New color value. */
    int pkt_pri;                    /* New packet priority. */
    int int_pri;                    /* New internal priority. */
} bcm_ipfix_rate_t;

/* Flags for bcm_ipfix_mirror_config_t.flags. */
#define BCM_IPFIX_MIRROR_CONFIG_TTL_OFFSET_MAX 0x0001     
#define BCM_IPFIX_MIRROR_CONFIG_FRAGMENT    0x0002     
#define BCM_IPFIX_MIRROR_CONFIG_NON_FRAGMENT 0x0004     

/* IPFIX port mirror configuration */
typedef struct bcm_ipfix_mirror_config_s {
    uint32 flags;           /* Configuration flags. */
    uint32 pkt_count;       /* Number of packets to be mirrored. */
    uint8 tcp_flags_mask;   /* TCP flags mask to determine if the packet will be
                               mirrored. */
    uint8 ttl_offset_max;   /* Max TTL offset allowance to be mirrored. */
} bcm_ipfix_mirror_config_t;

/* Initialize an IPFIX configuration structure. */
extern void bcm_ipfix_config_t_init(
    bcm_ipfix_config_t *config);

/* Initialize an IPFIX rate configuration structure. */
extern void bcm_ipfix_rate_t_init(
    bcm_ipfix_rate_t *rate);

/* Initialize an IPFIX mirror configuration structure. */
extern void bcm_ipfix_mirror_config_t_init(
    bcm_ipfix_mirror_config_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Register/Unregister a callback routine for BCM IPFIX export
 * information.
 */
extern int bcm_ipfix_register(
    int unit, 
    bcm_ipfix_callback_t callback, 
    void *userdata);

/* 
 * Register/Unregister a callback routine for BCM IPFIX export
 * information.
 */
extern int bcm_ipfix_unregister(
    int unit, 
    bcm_ipfix_callback_t callback, 
    void *userdata);

/* Set/Get the IPFIX configuration of the specified port. */
extern int bcm_ipfix_config_set(
    int unit, 
    bcm_ipfix_stage_t stage, 
    bcm_port_t port, 
    bcm_ipfix_config_t *config);

/* Set/Get the IPFIX configuration of the specified port. */
extern int bcm_ipfix_config_get(
    int unit, 
    bcm_ipfix_stage_t stage, 
    bcm_port_t port, 
    bcm_ipfix_config_t *config);

/* Create an IPFIX flow rate meter entry. */
extern int bcm_ipfix_rate_create(
    int unit, 
    bcm_ipfix_rate_t *rate_info);

/* Destroy an IPFIX flow rate meter entry. */
extern int bcm_ipfix_rate_destroy(
    int unit, 
    bcm_ipfix_rate_id_t rate_id);

/* Get IPFIX flow rate meter entry for the specified ID. */
extern int bcm_ipfix_rate_get(
    int unit, 
    bcm_ipfix_rate_t *rate_info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* IPFIX flow rate meter entry traverse callback function */
typedef int (*bcm_ipfix_rate_traverse_cb)(
    int unit, 
    bcm_ipfix_rate_t *rate_info, 
    void *userdata);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse through IPFIX flow rate meter entries. */
extern int bcm_ipfix_rate_traverse(
    int unit, 
    bcm_ipfix_rate_traverse_cb cb, 
    void *userdata);

/* Destroy all IPFIX flow rate meter entries. */
extern int bcm_ipfix_rate_destroy_all(
    int unit);

/* Add a mirror destination to the IPFIX flow rate meter entry. */
extern int bcm_ipfix_rate_mirror_add(
    int unit, 
    bcm_ipfix_rate_id_t rate_id, 
    bcm_gport_t mirror_dest_id);

/* Delete a mirror destination from the IPFIX flow rate meter entry. */
extern int bcm_ipfix_rate_mirror_delete(
    int unit, 
    bcm_ipfix_rate_id_t rate_id, 
    bcm_gport_t mirror_dest_id);

/* 
 * Delete all mirror destination associated with the IPFIX flow rate
 * meter entry.
 */
extern int bcm_ipfix_rate_mirror_delete_all(
    int unit, 
    bcm_ipfix_rate_id_t rate_id);

/* Get all mirror destination from the IPFIX flow rate meter entry. */
extern int bcm_ipfix_rate_mirror_get(
    int unit, 
    bcm_ipfix_rate_id_t rate_id, 
    int mirror_dest_size, 
    bcm_gport_t *mirror_dest_id, 
    int *mirror_dest_count);

/* Set IPFIX mirror control configuration of the specified port. */
extern int bcm_ipfix_mirror_config_set(
    int unit, 
    bcm_ipfix_stage_t stage, 
    bcm_gport_t port, 
    bcm_ipfix_mirror_config_t *config);

/* Get IPFIX mirror control configuration of the specified port. */
extern int bcm_ipfix_mirror_config_get(
    int unit, 
    bcm_ipfix_stage_t stage, 
    bcm_gport_t port, 
    bcm_ipfix_mirror_config_t *config);

/* Add an IPFIX mirror destination to the specified port. */
extern int bcm_ipfix_mirror_port_dest_add(
    int unit, 
    bcm_ipfix_stage_t stage, 
    bcm_gport_t port, 
    bcm_gport_t mirror_dest_id);

/* Delete an IPFIX mirror destination from the specified port. */
extern int bcm_ipfix_mirror_port_dest_delete(
    int unit, 
    bcm_ipfix_stage_t stage, 
    bcm_gport_t port, 
    bcm_gport_t mirror_dest_id);

/* Delete all IPFIX mirror destination from the specified port. */
extern int bcm_ipfix_mirror_port_dest_delete_all(
    int unit, 
    bcm_ipfix_stage_t stage, 
    bcm_gport_t port);

/* Get all IPFIX mirror destination of the specified port */
extern int bcm_ipfix_mirror_port_dest_get(
    int unit, 
    bcm_ipfix_stage_t stage, 
    bcm_gport_t port, 
    int mirror_dest_size, 
    bcm_gport_t *mirror_dest_id, 
    int *mirror_dest_count);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_IPFIX_H__ */
