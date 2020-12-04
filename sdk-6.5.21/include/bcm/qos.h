/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_QOS_H__
#define __BCM_QOS_H__

#include <bcm/types.h>
#include <bcm/multicast.h>

/* QoS Configuration Flags. */
#define BCM_QOS_MAP_WITH_ID             0x0001     
#define BCM_QOS_MAP_REPLACE             0x0002     
#define BCM_QOS_MAP_L2                  0x0004     
#define BCM_QOS_MAP_L2_OUTER_TAG        BCM_QOS_MAP_L2 
#define BCM_QOS_MAP_L2_INNER_TAG        0x0008     
#define BCM_QOS_MAP_L2_UNTAGGED         0x0010     
#define BCM_QOS_MAP_L2_VLAN_PCP         0x0020     
#define BCM_QOS_MAP_L3                  0x0040     
#define BCM_QOS_MAP_IPV4                BCM_QOS_MAP_L3 
#define BCM_QOS_MAP_IPV6                0x0080     
#define BCM_QOS_MAP_MPLS                0x0100     
#define BCM_QOS_MAP_ENCAP               0x0200     
#define BCM_QOS_MAP_INGRESS             0x0400     
#define BCM_QOS_MAP_EGRESS              0x0800     
#define BCM_QOS_MAP_MPLS_ELSP           0x1000     
#define BCM_QOS_MAP_MIM_ITAG            0x2000     
#define BCM_QOS_MAP_QUEUE               0x4000     
#define BCM_QOS_MAP_POLICER             0x8000     
#define BCM_QOS_MAP_PACKET_INVALID      0x10000    
#define BCM_QOS_MAP_IGNORE_OFFSET       0x20000    
#define BCM_QOS_MAP_OPCODE              0x40000    
#define BCM_QOS_MAP_L2_ETAG             0x80000    
#define BCM_QOS_MAP_MPLS_SECOND         BCM_QOS_MAP_L2_ETAG 
#define BCM_QOS_MAP_L2_VLAN_ETAG        0x100000   
#define BCM_QOS_MAP_L2_TWO_TAGS         BCM_QOS_MAP_L2_VLAN_ETAG 
#define BCM_QOS_MAP_MPLS_PHP            0x200000   
#define BCM_QOS_MAP_SUBPORT             0x400000   
#define BCM_QOS_MAP_PORT                BCM_QOS_MAP_SUBPORT 
#define BCM_QOS_MAP_L3_L2               0x800000   
#define BCM_QOS_MAP_VFT                 0x1000000  
#define BCM_QOS_MAP_VSAN                0x2000000  
#define BCM_QOS_MAP_IP_MC               BCM_QOS_MAP_VSAN 
#define BCM_QOS_MAP_RCH                 0x2000000  
#define BCM_QOS_MAP_OAM_PCP             0x4000000  
#define BCM_QOS_MAP_OAM_INTPRI          0x8000000  
#define BCM_QOS_MAP_OAM_OUTER_VLAN_PCP  0x10000000 
#define BCM_QOS_MAP_OAM_INNER_VLAN_PCP  0x20000000 
#define BCM_QOS_MAP_OAM_MPLS_EXP        0x40000000 
#define BCM_QOS_MAP_REMARK              0x10000000 
#define BCM_QOS_MAP_ECN                 0x20000000 
#define BCM_QOS_MAP_PHB                 0x40000000 
#define BCM_QOS_MAP_REPLICATION         0x80000000 
#define BCM_QOS_MAP_MIML                BCM_QOS_MAP_MIM_ITAG 
#define BCM_QOS_MAP_ENCAP_INTPRI_COLOR  BCM_QOS_MAP_VFT 
#define BCM_QOS_MAP_INGRESS_COPIED      BCM_QOS_MAP_REMARK /* Use packet QoS fields
                                                      (priority, CFI, DSCP and
                                                      ECN) copied from the
                                                      ingress for QoS remark at
                                                      the egress. */

/* Special qos map values */
#define BCM_QOS_OPCODE_PRESERVE -4         /* Dedicated opcode for QoS preserve. */

#define BCM_QOS_MAX_TUNNEL_EXP  7          /* Maximum tunnel EXP number */

/* QoS map action definitions. */
#define BCM_QOS_MAP_ACTION_DROP (1 << 0)   /* Packet drop */

/* QoS Map structure */
typedef struct bcm_qos_map_s {
    uint8 pkt_pri;                      /* Packet priority */
    uint8 pkt_cfi;                      /* Packet CFI */
    int dscp;                           /* Packet DSCP */
    int exp;                            /* Packet EXP */
    int int_pri;                        /* Internal priority */
    bcm_color_t color;                  /* Color */
    int remark_int_pri;                 /* (internal) remarking priority */
    bcm_color_t remark_color;           /* (internal) remark color */
    int policer_offset;                 /* Offset based on pri/cos to fetch a
                                           policer */
    int queue_offset;                   /* Offset based on int_pri to fetch cosq
                                           for subscriber ports */
    int port_offset;                    /* Offset based on port connection for
                                           indexing into the action table */
    uint8 etag_pcp;                     /* ETAG PCP field */
    uint8 etag_de;                      /* ETAG DE field */
    int counter_offset;                 /* Offset based on priority for indexing
                                           into the loss measurement counter
                                           table */
    int inherited_dscp_exp;             /* Inherited DSCP EXP value */
    uint32 opcode;                      /* Set QOS Map Opcode ID */
    uint8 inner_pkt_pri;                /* Inner Tag Packet priority */
    uint8 inner_pkt_cfi;                /* Inner Tag Packet CFI */
    uint8 int_cn;                       /* Internal congestion notification */
    uint8 pkt_ecn;                      /* Packet explicit congestion
                                           notification */
    uint8 responsive;                   /* Responsive flow (like TCP) */
    uint8 latency_marked;               /* Packet marked with latency congestion */
    int tunnel_exp[BCM_QOS_MAX_TUNNEL_EXP]; /* MPLS tunnel EXP */
    uint32 actions;                     /* QOS map actions */
    uint8 copied_pkt_pri;               /* Packet priority copied from ingress
                                           pipeline */
    uint8 copied_pkt_cfi;               /* Packet CFI copied from ingress
                                           pipeline */
    uint8 copied_pkt_dscp;              /* Packet DSCP copied from ingress
                                           pipeline */
    uint8 copied_pkt_ecn;               /* Packet ECN copied from ingress
                                           pipeline */
} bcm_qos_map_t;

/* qos ingress model type */
typedef enum bcm_qos_ingress_model_type_e {
    bcmQosIngressModelInvalid = 0,      /* qos model invalid */
    bcmQosIngressModelShortpipe = 1,    /* qos model is short pipe */
    bcmQosIngressModelPipe = 2,         /* qos model is pipe */
    bcmQosIngressModelUniform = 3,      /* qos model is uniform */
    bcmQosIngressModelStuck = 4         /* qos model is stuck */
} bcm_qos_ingress_model_type_t;

/* qos ingress ecn model type */
typedef enum bcm_qos_ingress_ecn_model_type_e {
    bcmQosIngressEcnModelInvalid = 0,   /* qos ecn model is invalid */
    bcmQosIngressEcnModelEligible = 1   /* qos ecn model is eligible */
} bcm_qos_ingress_ecn_model_type_t;

/* Ingress QoS model structure */
typedef struct bcm_qos_ingress_model_s {
    bcm_qos_ingress_model_type_t ingress_phb; /* ingress PHB model */
    bcm_qos_ingress_model_type_t ingress_remark; /* ingress remark model */
    bcm_qos_ingress_model_type_t ingress_ttl; /* ingress ttl model */
    bcm_qos_ingress_ecn_model_type_t ingress_ecn; /* ingress ecn model */
} bcm_qos_ingress_model_t;

/* qos egress model type */
typedef enum bcm_qos_egress_model_type_e {
    bcmQosEgressModelUniform = 0,       /* use previous layer qos */
    bcmQosEgressModelPipeNextNameSpace = 1, /* qos egress pipe next name space */
    bcmQosEgressModelPipeMyNameSpace = 2, /* qos egress pipe my name space */
    bcmQosEgressModelInitial = 3        /* use egress initial qos */
} bcm_qos_egress_model_type_t;

/* qos egress ecn model type */
typedef enum bcm_qos_egress_ecn_model_type_e {
    bcmQosEgressEcnModelInvalid = 0,    /* qos ecn model is invalid */
    bcmQosEgressEcnModelEligible = 1    /* qos ecn model is eligible */
} bcm_qos_egress_ecn_model_type_t;

/* egress QoS model structure */
typedef struct bcm_qos_egress_model_s {
    bcm_qos_egress_model_type_t egress_qos; /* egress qos variable */
    bcm_qos_egress_model_type_t egress_ttl; /* egress ttl */
    bcm_qos_egress_ecn_model_type_t egress_ecn; /* layer ecn is eligible */
} bcm_qos_egress_model_t;

/* qos map control */
typedef enum bcm_qos_map_control_type_e {
    bcmQosMapControlL3L2 = 0,           /* QoS map L2 use L3 */
    bcmQosMapControlL2TwoTagsMode = 1,  /* Ethernet Two tags mapping mode */
    bcmQosMapControlMplsPortModeServiceTag = 2 /* PWE tagged mode mapping */
} bcm_qos_map_control_type_t;

/* qos map control two tags mode */
typedef enum bcm_qos_map_control_two_tags_e {
    bcmQosMapControlL2TwoTagsModeOuterOnly = 0, /* QoS map only outer tag in case two
                                           tags */
    bcmQosMapControlL2TwoTagsModeInnerOnly = 1, /* QoS map only inner tag in case two
                                           tags */
    bcmQosMapControlL2TwoTagsModeDoubleTag = 2 /* QoS map only double tags in case two
                                           tags */
} bcm_qos_map_control_two_tags_t;

/* qos map control mpls port mode service tag */
typedef enum bcm_qos_map_control_mpls_port_e {
    bcmQosMapControlMplsPortModeServiceTagDisable = 0, /* disable MPLS port QOS map service tag */
    bcmQosMapControlMplsPortModeServiceTagTypical = 1, /* mpls port service tag typical map */
    bcmQosMapControlMplsPortModeServiceTagOuter = 2 /* mpls port map use only outer service
                                           tag */
} bcm_qos_map_control_mpls_port_t;

/* Initialize the QoS Map structure. */
extern void bcm_qos_map_t_init(
    bcm_qos_map_t *qos_map);

/* Initialize the QoS ingress model structure. */
extern void bcm_qos_ingress_model_t_init(
    bcm_qos_ingress_model_t *qos_ingress_model);

/* Initialize the QoS egress model structure. */
extern void bcm_qos_egress_model_t_init(
    bcm_qos_egress_model_t *qos_egress_model);

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the BCM QoS subsystem. */
extern int bcm_qos_init(
    int unit);

/* Detach the BCM QoS subsystem. */
extern int bcm_qos_detach(
    int unit);

/* Allocates hardware resources for QoS mapping. */
extern int bcm_qos_map_create(
    int unit, 
    uint32 flags, 
    int *map_id);

/* Frees hardware resources for QoS mapping. */
extern int bcm_qos_map_destroy(
    int unit, 
    int map_id);

/* Add a specific mapping entry to a QoS map. */
extern int bcm_qos_map_add(
    int unit, 
    uint32 flags, 
    bcm_qos_map_t *map, 
    int map_id);

/* Get an entire QoS mapping for the given QoS Map ID. */
extern int bcm_qos_map_multi_get(
    int unit, 
    uint32 flags, 
    int map_id, 
    int array_size, 
    bcm_qos_map_t *array, 
    int *array_count);

/* Deletes a specific mapping entry from a QoS map. */
extern int bcm_qos_map_delete(
    int unit, 
    uint32 flags, 
    bcm_qos_map_t *map, 
    int map_id);

/* Associates a GPORT with a QoS map. */
extern int bcm_qos_port_map_set(
    int unit, 
    bcm_gport_t port, 
    int ing_map, 
    int egr_map);

/* 
 * Retrieves the configured QoS mapping matching a type for the given
 * GPORT.
 */
extern int bcm_qos_port_map_type_get(
    int unit, 
    bcm_gport_t port, 
    uint32 flags, 
    int *map_id);

/* Retrieves the configured QoS mapping for the given GPORT. */
extern int bcm_qos_port_map_get(
    int unit, 
    bcm_gport_t port, 
    int *ing_map, 
    int *egr_map);

/* Associate a port, vid with an ingress and egress QoS mapping. */
extern int bcm_qos_port_vlan_map_set(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_t vid, 
    int ing_map, 
    int egr_map);

/* Get the QoS map ID for a port, vid. */
extern int bcm_qos_port_vlan_map_get(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_t vid, 
    int *ing_map, 
    int *egr_map);

/* Get the list of all QoS Map IDs. */
extern int bcm_qos_multi_get(
    int unit, 
    int array_size, 
    int *map_ids_array, 
    int *flags_array, 
    int *array_count);

/* Set QOS Map control properties */
extern int bcm_qos_map_control_set(
    int unit, 
    uint32 map_id, 
    uint32 flags, 
    bcm_qos_map_control_type_t type, 
    int arg);

/* Get QOS Map control properties */
extern int bcm_qos_map_control_get(
    int unit, 
    uint32 map_id, 
    uint32 flags, 
    bcm_qos_map_control_type_t type, 
    int *arg);

/* bcm_qos_map_id_get_by_profile */
extern int bcm_qos_map_id_get_by_profile(
    int unit, 
    uint32 flags, 
    int profile, 
    int *map_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* QoS control types */
typedef enum bcm_qos_control_type_e {
    bcmQosControlMplsIngressSwapRemarkProfile = 0, /* Egress QoS map Id for MPLS ingress
                                           swap cases. */
    bcmQosControlMplsExplicitNullIngressQosProfile = 1, /* Ingress QOS profile on MPLS Explicit
                                           NULL label. */
    bcmQosControlMplsExplicitNullIngressPhbModel = 2, /* Ingress QOS PHB model of MPLS
                                           Explicit NULL label. */
    bcmQosControlMplsExplicitNullIngressRemarkModel = 3, /* Ingress QOS Remark model of MPLS
                                           Explicit NULL label. */
    bcmQosControlMplsExplicitNullIngressTtlModel = 4, /* Ingress QOS TTL model of MPLS
                                           Explicit NULL label. */
    bcmQosControlMplsIngressSwapTtlModel = 5, /* TTL Model for MPLS ingress swap and
                                           encapsulation cases. */
    bcmQosControlMplsIngressSwapTtl = 6, /* TTL value for MPLS ingress swap and
                                           encapsulation cases. */
    bcmQosControlMplsIngressPopQoSPreserve = 7, /* MPLS ingress pop DSCP preserve. */
    bcmQosControlMplsEgressForwardQoSPreserve = 8, /* MPLS egress forward exp preserve. */
    bcmQosControl__Count = 9            /* Always last. Not a usable value. */
} bcm_qos_control_type_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/get QoS control properties per the control type. */
extern int bcm_qos_control_set(
    int unit, 
    uint32 flags, 
    bcm_qos_control_type_t type, 
    int arg);

/* bcm_qos_control_get */
extern int bcm_qos_control_get(
    int unit, 
    uint32 flags, 
    bcm_qos_control_type_t type, 
    int *arg);

/* Requires BROADCOM_PREMIUM license */
extern int bcm_qos_vpn_map_set(
    int unit, 
    bcm_vpn_t vpn, 
    int ing_map, 
    int egr_map);

/* Requires BROADCOM_PREMIUM license */
extern int bcm_qos_vpn_map_type_get(
    int unit, 
    bcm_vpn_t vpn, 
    uint32 flags, 
    int *map_id);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_QOS_H__ */
