/*
 * $Id: compat_6513.h,v 2.0 2017/10/14
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-6.5.13 routines
 */

#ifndef _COMPAT_6513_H_
#define _COMPAT_6513_H_

#ifdef	BCM_RPC_SUPPORT
#include <bcm/types.h>
#include <bcm/field.h>

#ifdef INCLUDE_TCB
#include <bcm/cosq.h>
#endif

#if defined(INCLUDE_L3)
#include <bcm/flow.h>

typedef struct bcm_compat6513_flow_encap_config_s {
    bcm_vpn_t vpn;                      /* VPN representing a vfi or vrf */
    bcm_gport_t flow_port;              /* flow port id representing a DVP */
    bcm_if_t intf_id;                   /* l3 interface id */
    uint32 dvp_group;                   /* DVP group ID */
    uint32 oif_group;                   /* L3 OUT interface group ID */
    uint32 vnid;                        /* VN_ID. */
    uint8 pri;                          /* service tag priority. */
    uint8 cfi;                          /* service tag cfi */
    uint16 tpid;                        /* service tag tpid */
    bcm_vlan_t vlan;                    /* service VLAN ID. */
    uint32 flags;                       /* BCM_FLOW_ENCAP_FLAG_xxx. */
    uint32 options;                     /* BCM_FLOW_ENCAP_OPTION_xxx. */
    bcm_flow_encap_criteria_t criteria; /* flow encap criteria. */
    uint32 valid_elements;              /* bitmap of valid fields for the encap
                                           action */
    bcm_flow_handle_t flow_handle;      /* flow handle derived from flow name */
    uint32 flow_option;                 /* flow option derived from flow option
                                           name */
} bcm_compat6513_flow_encap_config_t;
#endif


#if defined(INCLUDE_L3)
/* Add a flow encap rule. */
extern int bcm_compat6513_flow_encap_add(
    int unit, 
    bcm_compat6513_flow_encap_config_t *info, 
    uint32 num_of_fields, 
    bcm_flow_logical_field_t *field);
#endif

#if defined(INCLUDE_L3)
/* Delete a flow encap rule on the given encap key. */
extern int bcm_compat6513_flow_encap_delete(
    int unit, 
    bcm_compat6513_flow_encap_config_t *info, 
    uint32 num_of_fields, 
    bcm_flow_logical_field_t *field);
#endif

#if defined(INCLUDE_L3)
/* Get the encap attributes on the given encap key. */
extern int bcm_compat6513_flow_encap_get(
    int unit, 
    bcm_compat6513_flow_encap_config_t *info, 
    uint32 num_of_fields, 
    bcm_flow_logical_field_t *field);
#endif

#if defined(INCLUDE_L3)
/*
 * L3 Egress Structure.
 *
 * Description of an L3 forwarding destination.
 */
typedef struct bcm_compat6513_l3_egress_s {
    uint32 flags;                       /* Interface flags (BCM_L3_TGID,
                                           BCM_L3_L2TOCPU). */
    uint32 flags2;                      /* See BCM_L3_FLAGS2_xxx flag
                                           definitions. */
    bcm_if_t intf;                      /* L3 interface (source MAC, tunnel). */
    bcm_mac_t mac_addr;                 /* Next hop forwarding destination mac. */
    bcm_vlan_t vlan;                    /* Next hop vlan id. */
    bcm_module_t module;
    bcm_port_t port;                    /* Port packet switched to (if
                                           !BCM_L3_TGID). */
    bcm_trunk_t trunk;                  /* Trunk packet switched to (if
                                           BCM_L3_TGID). */
    uint32 mpls_flags;                  /* BCM_MPLS flag definitions. */
    bcm_mpls_label_t mpls_label;        /* MPLS label. */
    bcm_mpls_egress_action_t mpls_action; /* MPLS action. */
    int mpls_qos_map_id;                /* MPLS EXP map ID. */
    int mpls_ttl;                       /* MPLS TTL threshold. */
    uint8 mpls_pkt_pri;                 /* MPLS Packet Priority Value. */
    uint8 mpls_pkt_cfi;                 /* MPLS Packet CFI Value. */
    uint8 mpls_exp;                     /* MPLS Exp. */
    int qos_map_id;                     /* General QOS map id */
    bcm_if_t encap_id;                  /* Encapsulation index. */
    bcm_failover_t failover_id;         /* Failover Object Index. */
    bcm_if_t failover_if_id;            /* Failover Egress Object index. */
    bcm_multicast_t failover_mc_group;  /* Failover Multicast Group. */
    int dynamic_scaling_factor;         /* Scaling factor for dynamic load
                                           balancing thresholds. */
    int dynamic_load_weight;            /* Weight of traffic load in determining
                                           a dynamic load balancing member's
                                           quality. */
    int dynamic_queue_size_weight;      /* Weight of queue size in determining a
                                           dynamic load balancing member's
                                           quality. */
    int intf_class;                     /* L3 interface class ID */
    uint32 multicast_flags;             /* BCM_L3_MULTICAST flag definitions. */
    uint16 oam_global_context_id;       /* OAM global context id passed from
                                           ingress to egress XGS chip. */
    bcm_vntag_t vntag;                  /* VNTAG. */
    bcm_vntag_action_t vntag_action;    /* VNTAG action. */
    bcm_etag_t etag;                    /* ETAG. */
    bcm_etag_action_t etag_action;      /* ETAG action. */
    uint32 flow_handle;                 /* FLOW handle for flex entries. */
    uint32 flow_option_handle;          /* FLOW option handle for flex entries. */
    uint32 flow_label_option_handle;    /* FLOW option handle for egress label
                                           flex entries. */
    bcm_flow_logical_field_t logical_fields[BCM_FLOW_MAX_NOF_LOGICAL_FIELDS]; /* logical fields array for flex
                                           entries. */
    uint32 num_of_fields;               /* number of logical fields. */
    int counting_profile;               /* counting profile. If not required,
                                           set it to
                                           BCM_STAT_LIF_COUNTING_PROFILE_NONE */
    int mpls_ecn_map_id;                /* IP ECN/INT CN to MPLS EXP map ID. */
    bcm_l3_ingress_urpf_mode_t urpf_mode; /* fec rpf mode. */
} bcm_compat6513_l3_egress_t;

/*  Create an Egress forwarding object. */
extern int bcm_compat6513_l3_egress_create(
    int unit, uint32 flags, bcm_compat6513_l3_egress_t *egr, bcm_if_t *intf);

/*  Get an Egress forwarding object. */
extern int bcm_compat6513_l3_egress_get(
    int unit, bcm_if_t intf, bcm_compat6513_l3_egress_t *egr);

/*  Find an egress forwarding object. */
extern int bcm_compat6513_l3_egress_find(
    int unit, bcm_compat6513_l3_egress_t *egr, bcm_if_t *intf);
#endif

#ifdef INCLUDE_TCB
typedef struct bcm_compat6513_cosq_tcb_config_s {
    bcm_cosq_tcb_scope_t scope_type;    /* TCB monitor scope type */
    bcm_gport_t gport;                  /* TCB monitor entity, can be a Unicast
                                           queue gport or a physical port gport */
    bcm_cos_queue_t cosq;               /* Reserved field */
    uint32 trigger_reason;              /* Define the event which could trigger
                                           the capture */
    uint32 pre_freeze_capture_num;      /* Define the capture number after TCB
                                           enter freeze status */
    uint32 pre_freeze_capture_time;     /* Define the capture time after TCB
                                           enter freeze status. Unit is usec */
    uint32 post_sample_probability;     /* The sample probability in
                                           post-trigger phase. Step is 1/16,
                                           valid configure range is 1 to 16, 1
                                           means 1/16 sample, 16 means sample
                                           all */
    uint32 pre_sample_probability;      /* The sample probability in pre-trigger
                                           phase. Step is 1/16, valid configure
                                           range is 1 to 16, 1 means 1/16
                                           sample, 16 means sample all */
} bcm_compat6513_cosq_tcb_config_t;

extern int bcm_compat6513_cosq_tcb_config_get(
    int unit,
    bcm_cosq_buffer_id_t buffer_id,
    bcm_compat6513_cosq_tcb_config_t *config);

extern int bcm_compat6513_cosq_tcb_config_set(
    int unit,
    bcm_cosq_buffer_id_t buffer_id,
    bcm_compat6513_cosq_tcb_config_t *config);
#endif

#endif
#endif	/* !_COMPAT_6513_H */
