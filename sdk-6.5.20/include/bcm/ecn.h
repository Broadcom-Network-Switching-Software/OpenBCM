/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_ECN_H__
#define __BCM_ECN_H__

#include <bcm/types.h>

/* ECN traffic map flag. */
#define BCM_ECN_TRAFFIC_MAP_RESPONSIVE  (1 << 0)   /* 1: Responsive, 0:
                                                      Non-responsive. */

#define BCM_ECN_TRAFFIC_MAP_LATENCY_ECN (1 << 1)   /* Internal congestion
                                                      notification mapping of
                                                      Latency ECN. */

/* ECN traffic map types. */
typedef enum bcm_ecn_traffic_map_type_e {
    bcmEcnTrafficMapTypeIngressPostForward = 0, /* Traffic map at ingress post
                                           forwarding. */
    bcmEcnTrafficMapTypeIngressPreForward = 1, /* Traffic map at ingress pre
                                           forwarding. */
    bcmEcnTrafficMapTypeTunnelInitiator = 2, /* Traffic map of tunnel initiator. */
    bcmEcnTrafficMapTypeCount = 3       /* Always last. Not a usable value. */
} bcm_ecn_traffic_map_type_t;

/* Tunnel types definition for ECN traffic map. */
typedef enum bcm_ecn_traffic_map_tunnel_type_e {
    bcmEcnTrafficMapNonTunnel = 0,  /* None tunnel traffic. */
    bcmEcnTrafficMapL2Tunnel = 1,   /* L2 tunnel traffic, VxLAN and VPLS. */
    bcmEcnTrafficMapL3Tunnel = 2,   /* L3 tunnel traffic, IPinIP, L3GRE and L3
                                       MPLS. */
    bcmEcnTrafficMapMpls = 3,       /* MPLS transit traffic. */
    bcmEcnTrafficMapCount = 4       /* End of list. */
} bcm_ecn_traffic_map_tunnel_type_t;

/* ECN traffic map info. */
typedef struct bcm_ecn_traffic_map_info_s {
    uint32 flags;                       /* BCM_ECN_TRAFFIC_MAP_XXX flag
                                           definitions. */
    uint8 ecn;                          /* ECN value of packet's IP header. */
    int int_cn;                         /* Mapped internal congestion
                                           notification(int_cn) value. */
    uint8 tunnel_ecn;                   /* ECN information of packet's tunnel
                                           header. */
    bcm_ecn_traffic_map_tunnel_type_t tunnel_type; /* Tunnel type. */
    bcm_ecn_traffic_map_type_t type;    /* Traffic map type
                                           bcmEcnTrafficMapTypeXXX to be
                                           configured. */
} bcm_ecn_traffic_map_info_t;

/* ECN marking types. */
typedef enum bcm_ecn_marking_type_e {
    bcmEcnMarkingTypeBufferUsage = 0,   /* Buffer usage based marking. */
    bcmEcnMarkingTypeLatency = 1,       /* Latency based marking. */
    bcmEcnMarkingTypeCount = 2          /* Always last. Not a usable value. */
} bcm_ecn_marking_type_t;

/* ECN type map flags. */
#define BCM_ECN_TYPE_MAP_INTPRI_TO_MARKINGTYPE (0x00000001) /* Mapping from internal
                                                          priority to marking
                                                          type. */

#define BCM_ECN_TYPE_MAP_INGRESS_PRE_FORWARD_MARKING_TYPE_UPDATE (0x80000000) /* Indicates the
                                                          operation on ingress
                                                          pre-forward marking
                                                          type. */

#define BCM_ECN_TYPE_MAP_INGRESS_POST_FORWARD_MARKING_TYPE_UPDATE (0x40000000) /* Indicates the
                                                          operation on ingress
                                                          post-forward marking
                                                          type. */

/* ECN type map structure. */
typedef struct bcm_ecn_type_map_s {
    int int_pri;                        /* Internal priority. */
    bcm_ecn_marking_type_t ing_pre_fwd_marking; /* Marking type at ingress
                                           pre-forwarding. */
    bcm_ecn_marking_type_t ing_post_fwd_marking; /* Marking type at ingress
                                           post-forwarding. */
} bcm_ecn_type_map_t;

/* ECN traffic action types */
#define BCM_ECN_TRAFFIC_ACTION_TYPE_ENQUEUE (0x1)      /* Action of enqueue
                                                          stage. */
#define BCM_ECN_TRAFFIC_ACTION_TYPE_DEQUEUE (0x2)      /* Action of dequeue
                                                          stage. */
#define BCM_ECN_TRAFFIC_ACTION_TYPE_EGRESS  (0x3)      /* Action of egress
                                                          stage. */
#define BCM_ECN_TRAFFIC_ACTION_TYPE_LATENCY_ECN_DEQUEUE (0x4)      /* Action of Latency ECN
                                                          at dequeue stage. */

/* ECN traffic action flags */
#define BCM_ECN_TRAFFIC_ACTION_NONE         (0x0)      /* Action none */
#define BCM_ECN_TRAFFIC_ACTION_ENQUEUE_WRED_RESPONSIVE (1 << 0)   /* If set, enabled WRED
                                                          responsive dropping.
                                                          Otherwise, enables
                                                          WRED non-responsive
                                                          dropping. */
#define BCM_ECN_TRAFFIC_ACTION_ENQUEUE_MARK_ELIGIBLE (1 << 1)   /* If set, avoids WRED
                                                          dropping when ECN
                                                          marking is enabled. */
#define BCM_ECN_TRAFFIC_ACTION_DEQUEUE_CONGESTION_INT_CN_UPDATE (1 << 2)   /* If set, updates the
                                                          value of int_cn when
                                                          congestion is
                                                          experienced. */
#define BCM_ECN_TRAFFIC_ACTION_DEQUEUE_NON_CONGESTION_INT_CN_UPDATE (1 << 3)   /* If set, updates the
                                                          value of int_cn when
                                                          congestion is not
                                                          experienced. */
#define BCM_ECN_TRAFFIC_ACTION_EGRESS_ECN_MARKING (1 << 4)   /* If set, indicates that
                                                          the packet's ECN bits
                                                          need to be updated. */
#define BCM_ECN_TRAFFIC_ACTION_EGRESS_DROP  (1 << 5)   /* If set, indicates to
                                                          drop packet in egress. */
#define BCM_ECN_TRAFFIC_ACTION_INGRESS_DROP (1 << 6)   /* If set, indicates to
                                                          drop packet in
                                                          ingress. */
#define BCM_ECN_TRAFFIC_ACTION_EGRESS_EXP_MARKING (1 << 7)   /* If set, indicates to
                                                          update EXP. */
#define BCM_ECN_TRAFFIC_ACTION_INGRESS_ECN_MARKING (1 << 8)   /* If set, indicates to
                                                          update ECN. */
#define BCM_ECN_TRAFFIC_ACTION_RESPONSIVE   (1 << 9)   /* If set, indicates to
                                                          set configurations for
                                                          ecn responsive
                                                          traffic. */
#define BCM_ECN_TRAFFIC_ACTION_EGRESS_INT_CN_MARKING (1 << 10)  /* If set, indicates to
                                                          update INT_CN. */
#define BCM_ECN_TRAFFIC_ACTION_ECN_COUNTER_ELIGIBLE (1 << 11)  /* If set, indicates to
                                                          update ECN counter. */
#define BCM_ECN_TRAFFIC_ACTION_DEQUEUE_LATENCY_INT_CN_UPDATE (1 << 12)  /* If set, update int_cn
                                                          when latency
                                                          experienced. */

/* 
 * ECN traffic action configuration.
 * 
 * Contains information required for assigning the actions of ECN
 * traffic.
 */
typedef struct bcm_ecn_traffic_action_config_s {
    uint32 action_type;             /* See BCM_ECN_TRAFFIC_ACTION_TYPE_XXX flag
                                       definitions. */
    uint32 action_flags;            /* See BCM_ECN_TRAFFIC_ACTION_XXX flag
                                       definitions. */
    int int_cn;                     /* Internal congestion notification. */
    bcm_color_t color;              /* Packet color. */
    uint8 ecn;                      /* ECN value of packet;s IP header. */
    uint8 new_ecn;                  /* New ECN value for remarking. */
    int congested_int_cn;           /* New int_cn to be updated when the
                                       congestion is experienced. */
    int non_congested_int_cn;       /* New int_cn to be updated when the
                                       congestion is not experienced. */
    int responsive;                 /* New responsive value to be used in the
                                       egress pipeline. */
    int buffer_high_thd_exceeded;   /* High buffer usage experienced. */
    int buffer_low_thd_exceeded;    /* Low buffer usage experienced. */
    int latency_thd_exceeded;       /* Latency experienced. */
    int congestion_marked;          /* Enable counting congestion marked
                                       packets. */
    int latency_int_cn;             /* New int_cn when latency experienced. */
} bcm_ecn_traffic_action_config_t;

/* Initialize the ECN type map structure. */
extern void bcm_ecn_type_map_t_init(
    bcm_ecn_type_map_t *ecn_type_map);

#ifndef BCM_HIDE_DISPATCHABLE

/* To get the ECN type mapping. */
extern int bcm_ecn_type_map_get(
    int unit, 
    uint32 flags, 
    bcm_ecn_type_map_t *map);

/* To set the ECN type mapping. */
extern int bcm_ecn_type_map_set(
    int unit, 
    uint32 flags, 
    bcm_ecn_type_map_t *map);

#endif /* BCM_HIDE_DISPATCHABLE */

/* ECN type map traverse callback */
typedef int (*bcm_ecn_type_map_traverse_cb)(
    int unit, 
    uint32 flags, 
    bcm_ecn_type_map_t *map, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse all the specified created ECN type maps. */
extern int bcm_ecn_type_map_traverse(
    int unit, 
    uint32 flags, 
    bcm_ecn_type_map_traverse_cb cb, 
    void *user_data);

/* 
 * To configure the value of responsive indication of the IP protocol
 * value.
 */
extern int bcm_ecn_responsive_protocol_get(
    int unit, 
    uint8 ip_proto, 
    int *responsive);

/* 
 * To configure the value of responsive indication of the IP protocol
 * value.
 */
extern int bcm_ecn_responsive_protocol_set(
    int unit, 
    uint8 ip_proto, 
    int responsive);

/* To set/get the mapped internal congestion notification (int_cn). */
extern int bcm_ecn_traffic_map_get(
    int unit, 
    bcm_ecn_traffic_map_info_t *map);

/* To set/get the mapped internal congestion notification (int_cn). */
extern int bcm_ecn_traffic_map_set(
    int unit, 
    bcm_ecn_traffic_map_info_t *map);

/* Assign/Retrieve the actions to the specified ECN traffic. */
extern int bcm_ecn_traffic_action_config_get(
    int unit, 
    bcm_ecn_traffic_action_config_t *ecn_config);

/* Assign/Retrieve the actions to the specified ECN traffic. */
extern int bcm_ecn_traffic_action_config_set(
    int unit, 
    bcm_ecn_traffic_action_config_t *ecn_config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize an ECN traffic map information structure. */
extern void bcm_ecn_traffic_map_info_t_init(
    bcm_ecn_traffic_map_info_t *ecn_map);

/* Initialize an ECN traffic action configure structure. */
extern void bcm_ecn_traffic_action_config_t_init(
    bcm_ecn_traffic_action_config_t *ecn_action);

/* ECN map flags. */
#define BCM_ECN_MAP_WITH_ID                 0x00000001 /* ECN map ID is
                                                          specified by users */
#define BCM_ECN_MAP_INGRESS                 0x00000002 /* create an ingress map
                                                          used in conjunction
                                                          with map type */
#define BCM_ECN_MAP_EGRESS                  0x00000004 /* create an egress map
                                                          used in conjunction
                                                          with map type */
#define BCM_ECN_MAP_MPLS                    0x00000008 /* create an MPLS type of
                                                          map */
#define BCM_ECN_MAP_MPLS_INT_CN_TO_EXP      0x00000010 /* create an INT_CN to
                                                          EXP map */
#define BCM_ECN_MAP_TUNNEL_TERM             0x00000020 /* create a tunnel term
                                                          ecn map */
#define BCM_ECN_MAP_TUNNEL_INIT_IP_PAYLOAD  0x00000040 /* Create tunnel
                                                          initiator for IP
                                                          payload ECN map. */
#define BCM_ECN_MAP_TUNNEL_INIT_NON_IP_PAYLOAD 0x00000080 /* Create tunnel
                                                          initiator for non-IP
                                                          payload ECN map. */
#define BCM_ECN_MAP_LATENCY_ECN             0x00000100 /* Create a Latency ECN
                                                          map. */

#if defined(INCLUDE_L3)
/* MPLS ECN Map Structure. */
typedef struct bcm_ecn_map_s {
    uint32 action_flags;        /* action flags for ECN map. */
    int int_cn;                 /* Internal congestion. */
    uint8 inner_ecn;            /* IP ECN value in payload. */
    uint8 ecn;                  /* IP ECN value. */
    uint8 exp;                  /* Mpls EXP value. */
    uint8 new_ecn;              /* New ECN value. */
    uint8 new_exp;              /* New EXP value. */
    uint32 nonip_action_flags;  /* action flags for Non-ip ECN map. */
    int new_int_cn;             /* New internal congestion notification value. */
    int mmu_queue_id;           /* MMU queue ID. */
    int latency_ecn_en;         /* Enable/disable Latency ECN. */
    int latency_ecn_threshold;  /* Latency ECN timestamp threshold in
                                   nanoseconds. */
} bcm_ecn_map_t;
#endif

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(INCLUDE_L3)
/* To create an ECN mapping profile. */
extern int bcm_ecn_map_create(
    int unit, 
    uint32 flags, 
    int *ecn_map_id);
#endif

#if defined(INCLUDE_L3)
/* To destroy an ECN mapping profile. */
extern int bcm_ecn_map_destroy(
    int unit, 
    int ecn_map_id);
#endif

#if defined(INCLUDE_L3)
/* To set an ECN mapping entry in an ECN mapping profile. */
extern int bcm_ecn_map_set(
    int unit, 
    uint32 options, 
    int ecn_map_id, 
    bcm_ecn_map_t *ecn_map);
#endif

#if defined(INCLUDE_L3)
/* To get an ECN mapping entry from an ECN mapping profile. */
extern int bcm_ecn_map_get(
    int unit, 
    int ecn_map_id, 
    bcm_ecn_map_t *ecn_map);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

#if defined(INCLUDE_L3)
/* To init an ECN mapping structure. */
extern void bcm_ecn_map_t_init(
    bcm_ecn_map_t *ecn_map);
#endif

/* ECN port map flags. */
#define BCM_ECN_EGRESS_PORT_ECN_TO_EXP_MAP  0x00000001 /* Indicate an ECN_TO_EXP
                                                          map */
#define BCM_ECN_EGRESS_PORT_INT_CN_TO_EXP_MAP 0x00000002 /* Indicate an
                                                          INT_CN_TO_EXP map */
#define BCM_ECN_INGRESS_PORT_TUNNEL_TERM_MAP 0x00000004 /* Indicate a tunnel term
                                                          ECN map */
#define BCM_ECN_EGRESS_PORT_LATENCY_ECN_MAP 0x00000008 /* Indicates a Latency
                                                          ECN map */

#if defined(INCLUDE_L3)
/* MPLS ecn port Map Structure. */
typedef struct bcm_ecn_port_map_s {
    uint32 flags;   /* flags for ECN port map. */
    int ecn_map_id; /* Ecn map id. */
} bcm_ecn_port_map_t;
#endif

#if defined(INCLUDE_L3)
/* To initialize an ecn port map structure. */
extern void bcm_ecn_port_map_t_init(
    bcm_ecn_port_map_t *ecn_map);
#endif

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(INCLUDE_L3)
/* To set/get the egress ecn_map_id on the specific port. */
extern int bcm_ecn_port_map_get(
    int unit, 
    bcm_gport_t port, 
    bcm_ecn_port_map_t *ecn_map);
#endif

#if defined(INCLUDE_L3)
/* To set/get the egress ecn_map_id on the specific port. */
extern int bcm_ecn_port_map_set(
    int unit, 
    bcm_gport_t port, 
    bcm_ecn_port_map_t *ecn_map);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

#if defined(INCLUDE_L3)
/* ecn dscp map mode */
typedef enum bcm_ecn_dscp_map_mode_e {
    bcmEcnDscpMapModeDraft = 0,     /* Draft mode */
    bcmEcnDscpMapModeRfc3168 = 1,   /* Mode defined in RFC3168 */
    bcmEcnDscpMapModeCount = 2      /* End of list */
} bcm_ecn_dscp_map_mode_t;
#endif

#if defined(INCLUDE_L3)
/* ecn map mode information */
typedef struct bcm_ecn_map_mode_s {
    int dscp;                           /* value of -1 to change all entries */
    bcm_ecn_dscp_map_mode_t dscp_map_mode; /* ecn map mode setting. */
} bcm_ecn_map_mode_t;
#endif

#if defined(INCLUDE_L3)
/* To initialize an ecn mapping mode structure. */
extern void bcm_ecn_map_mode_t_init(
    bcm_ecn_map_mode_t *ecn_map_mode);
#endif

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(INCLUDE_L3)
/* Configure the ECN mapping mode. */
extern int bcm_ecn_map_mode_set(
    int unit, 
    bcm_ecn_map_mode_t *ecn_map_mode);
#endif

#if defined(INCLUDE_L3)
/* Retrieve the ECN mapping mode. */
extern int bcm_ecn_map_mode_get(
    int unit, 
    bcm_ecn_map_mode_t *ecn_map_mode);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_ECN_H__ */
