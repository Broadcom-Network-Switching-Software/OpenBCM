/*
 * $Id: compat_659.h,v 1.0 2017/04/13
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-6.5.9 routines
 */

#ifndef _COMPAT_659_H_
#define _COMPAT_659_H_

#ifdef	BCM_RPC_SUPPORT
#include <bcm_int/compat/compat_6518.h>
#include <bcm/types.h>
#include <bcm/field.h>
#include <bcm/cosq.h>

#if defined(INCLUDE_L3)
#include <bcm/vlan.h>

/* Layer 2 Logical port type */
typedef struct bcm_compat659_vlan_port_s {
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
} bcm_compat659_vlan_port_t;

/*
 * Create a layer 2 logical port.  Places the ID in the logical port
 * descriptor if WITH_ID flag is not provided, uses the ID in the logical
 * port descriptor if WITH_ID flag specified.
 */
extern int bcm_compat659_vlan_port_create(int unit,
                                          bcm_compat659_vlan_port_t *vlan_port);

/* Get/find a layer 2 logical port given the GPORT id or match criteria. */
extern int bcm_compat659_vlan_port_find(int unit,
                                        bcm_compat659_vlan_port_t *vlan_port);


#endif /* INCLUDE_L3 */

/* This structure contains the configuration of a VLAN. */
typedef struct bcm_compat659_vlan_control_vlan_s {
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
} bcm_compat659_vlan_control_vlan_t;

/* Set per VLAN configuration */
extern int bcm_compat659_vlan_control_vlan_selective_set(int unit,
                                          bcm_vlan_t vlan, uint32 valid_fields,
                                    bcm_compat659_vlan_control_vlan_t *control);
/* Get per VLAN configuration */
extern int bcm_compat659_vlan_control_vlan_selective_get(int unit,
                                          bcm_vlan_t vlan, uint32 valid_fields,
                                    bcm_compat659_vlan_control_vlan_t *control);

extern int bcm_compat659_vlan_control_vlan_set(int unit, bcm_vlan_t vlan,
                                     bcm_compat659_vlan_control_vlan_t control);

extern int bcm_compat659_vlan_control_vlan_get(int unit, bcm_vlan_t vlan,
                                    bcm_compat659_vlan_control_vlan_t *control);


/* For Virtual output queues (system ports) */
typedef struct bcm_compat659_cosq_gport_discard_s {
    uint32 flags; 
    int min_thresh;         /* Queue depth in bytes to begin dropping. */
    int max_thresh;         /* Queue depth in bytes to drop all packets. */
    int drop_probability;   /* Drop probability at max threshold. */
    int gain;               /* Determines the smoothing that should be applied. */
    int ecn_thresh;         /* Queue depth in bytes to stop marking and start
                               dropping. */
    int refresh_time;       /* Actual average refresh time. */
} bcm_compat659_cosq_gport_discard_t;

extern int bcm_compat659_cosq_gport_discard_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_compat659_cosq_gport_discard_t *discard);

extern int bcm_compat659_cosq_gport_discard_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_compat659_cosq_gport_discard_t *discard);

extern int bcm_compat659_cosq_gport_discard_extended_set(
    int unit, 
    bcm_cosq_object_id_t *id, 
    bcm_compat659_cosq_gport_discard_t *discard);

extern int bcm_compat659_cosq_gport_discard_extended_get(
    int unit, 
    bcm_cosq_object_id_t *id, 
    bcm_compat659_cosq_gport_discard_t *discard);

#endif
#endif	/* !_COMPAT_659_H */
