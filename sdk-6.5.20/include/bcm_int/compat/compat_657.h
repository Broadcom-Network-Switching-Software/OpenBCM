/*
 * $Id: compat_657.h,v 2.0 2016/12/07
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-6.5.7 routines
 */

#ifndef _COMPAT_657_H_
#define _COMPAT_657_H_

#ifdef	BCM_RPC_SUPPORT
#include <bcm/types.h>
#include <bcm/field.h>

#if defined(INCLUDE_L3)
#include <bcm/mpls.h>

/* Extender forwarding entry type. */
typedef struct bcm_compat657_extender_forward_s {
    uint32 flags;                       /* BCM_extender_FORWARD_xxx. */
    uint16 name_space;                  /* extender Namespace. */
    uint16 extended_port_vid;           /* Extended port VID. */
    bcm_gport_t dest_port;              /* Destination Gport. */
    bcm_multicast_t dest_multicast;     /* Destination Multicast Group. */
    bcm_gport_t extender_forward_id;    /* Downstream forwarding id. */
} bcm_compat657_extender_forward_t;

/* Create extender forwarding entry */
extern int bcm_compat657_extender_forward_add(
    int unit, 
    bcm_compat657_extender_forward_t *extender_forward_entry);

/* Delete extender forwarding entry. */
extern int bcm_compat657_extender_forward_delete(
    int unit, 
    bcm_compat657_extender_forward_t *extender_forward_entry);

/* Get extender forwarding entry information */
extern int bcm_compat657_extender_forward_get(
    int unit, 
    bcm_compat657_extender_forward_t *extender_forward_entry);

/* MPLS tunnel switch structure. */
typedef struct bcm_compat657_mpls_tunnel_switch_s {
    uint32 flags;                       /* BCM_MPLS_SWITCH_xxx. */
    bcm_mpls_label_t label;             /* Incoming label value. */
    bcm_mpls_label_t second_label;      /* Incoming second label. */
    bcm_gport_t port;                   /* Incoming port. */
    bcm_mpls_switch_action_t action;    /* MPLS label action. */
    bcm_mpls_switch_action_t action_if_bos; /* MPLS label action if BOS. */
    bcm_mpls_switch_action_t action_if_not_bos; /* MPLS label action if not BOS. */
    bcm_multicast_t mc_group;           /* P2MP Multicast group. */
    int exp_map;                        /* EXP-map ID. */
    int int_pri;                        /* Internal priority. */
    bcm_policer_t policer_id;           /* Policer ID to be associated with the
                                           incoming label. */
    bcm_vpn_t vpn;                      /* L3 VPN used if action is POP. */
    bcm_mpls_egress_label_t egress_label; /* Outgoing label information. */
    bcm_if_t egress_if;                 /* Outgoing egress object. */
    bcm_if_t ingress_if;                /* Ingress Interface object. */
    int mtu;                            /* MTU. */
    int qos_map_id;                     /* QOS map identifier. */
    bcm_failover_t failover_id;         /* Failover Object Identifier for
                                           protected tunnel. Used for 1+1
                                           protection also */
    bcm_gport_t tunnel_id;              /* Tunnel ID. */
    bcm_gport_t failover_tunnel_id;     /* Failover Tunnel ID. */
    bcm_if_t tunnel_if;                 /* hierarchical interface, relevant for
                                           when action is
                                           BCM_MPLS_SWITCH_ACTION_POP. */
    bcm_gport_t egress_port;            /* Outgoing egress port. */
    uint16 oam_global_context_id;       /* OAM global context id passed from
                                           ingress to egress XGS chip. */
    uint32 class_id;                    /* Class ID */
    int inlif_counting_profile;         /* In LIF counting profile */
    int ecn_map_id;                     /* ECN map identifier */
    int tunnel_term_ecn_map_id;         /* Tunnel termination ecn map identifier */
} bcm_compat657_mpls_tunnel_switch_t;

extern int bcm_compat657_mpls_tunnel_switch_add(
    int unit, 
    bcm_compat657_mpls_tunnel_switch_t *info);

extern int bcm_compat657_mpls_tunnel_switch_create(
    int unit, 
    bcm_compat657_mpls_tunnel_switch_t *info);

extern int bcm_compat657_mpls_tunnel_switch_delete(
    int unit, 
    bcm_compat657_mpls_tunnel_switch_t *info);

extern int bcm_compat657_mpls_tunnel_switch_get(
    int unit, 
    bcm_compat657_mpls_tunnel_switch_t *info);
#endif /* INCLUDE_L3 */
/* Configuration for remote port */
typedef struct bcm_compat657_port_e2efc_remote_port_config_s {
    bcm_module_t remote_module; /* Remote module id that E2EFC applied */
    bcm_port_t remote_port;     /* Remote port id to be configured */
    int xoff_threshold_bytes;   /* Assert backpressure when number of buffers
                                   used is at or above this threshold in bytes */
    int xoff_threshold_packets; /* Assert backpressure when number of buffers
                                   used is at or above this threshold in packets */
    int xon_threshold_bytes;    /* Remove backpressure when number of buffers
                                   used drop below this threshold in bytes */
    int xon_threshold_packets;  /* Remove backpressure when number of buffers
                                   used drop below this threshold in packets */
    int drop_threshold_bytes;   /* Drop packets when number of buffers used hit
                                   this threshold in bytes */
    int drop_threshold_packets; /* Drop packets when number of buffers used hit
                                   this threshold in packets */
} bcm_compat657_port_e2efc_remote_port_config_t;

extern int bcm_compat657_port_e2efc_remote_port_add(
    int unit,
    bcm_compat657_port_e2efc_remote_port_config_t *e2efc_rport_cfg,
    int *rport_handle_id);


extern int bcm_compat657_port_e2efc_remote_port_set(
    int unit,
    int rport_handle_id,
    bcm_compat657_port_e2efc_remote_port_config_t *e2efc_rport_cfg);
extern int bcm_compat657_port_e2efc_remote_port_get(
    int unit,
    int rport_handle_id,
    bcm_compat657_port_e2efc_remote_port_config_t *e2efc_rport_cfg);

#define BCM_COMPAT657_FIELD_QUALIFY_CNT     (675)
#define BCM_COMPAT657_FIELD_USER_NUM_UDFS   (93)

typedef struct bcm_compat657_field_qset_s {
    SHR_BITDCL w[_SHR_BITDCLSIZE(BCM_COMPAT657_FIELD_QUALIFY_CNT +
                                 BCM_COMPAT657_FIELD_USER_NUM_UDFS)];
    SHR_BITDCL udf_map[_SHR_BITDCLSIZE(BCM_COMPAT657_FIELD_USER_NUM_UDFS)];
} bcm_compat657_field_qset_t;

extern int bcm_compat657_field_group_create(
    int unit,
    bcm_compat657_field_qset_t qset,
    int pri,
    bcm_field_group_t *group);

extern int bcm_compat657_field_group_create_id(
    int unit,
    bcm_compat657_field_qset_t qset,
    int pri,
    bcm_field_group_t group);

extern int bcm_compat657_field_qset_data_qualifier_add(
    int unit,
    bcm_compat657_field_qset_t *qset,
    int qual_id);

#endif	/* BCM_RPC_SUPPORT */

#endif	/* !_COMPAT_657_H */
