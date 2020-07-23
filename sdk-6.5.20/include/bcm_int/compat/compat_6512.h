/*
 * $Id: compat_6512.h,v 3.0 2018/02/27
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-6.5.12 routines
 */

#ifndef _COMPAT_6512_H_
#define _COMPAT_6512_H_

#ifdef	BCM_RPC_SUPPORT
#include <bcm/types.h>
#include <bcm/field.h>
#include <bcm/fcoe.h>
#include <bcm/l2.h>

#if defined(INCLUDE_L3)
#include <bcm/vxlan.h>

/* MPLS ECN Map Structure. */
typedef struct bcm_compat6512_vxlan_vpn_config_s {
    uint32 flags;                       /* BCM_VXLAN_VPN_xxx. */
    bcm_vpn_t vpn;                      /* VXLAN VPN */
    uint32 vnid;                        /* VNID */
    uint8 pkt_pri;                      /* Packet Priority */
    uint8 pkt_cfi;                      /* Packet CFI */
    uint16 egress_service_tpid;         /* Service TPID */
    bcm_vlan_t egress_service_vlan;     /* Service VLAN */
    bcm_multicast_t broadcast_group; 
    bcm_multicast_t unknown_unicast_group; 
    bcm_multicast_t unknown_multicast_group; 
    bcm_vlan_protocol_packet_ctrl_t protocol_pkt; 
    bcm_vlan_t vlan;                    /* Outer VLAN */
    bcm_gport_t match_port_class;       /* local port vlan domain */
} bcm_compat6512_vxlan_vpn_config_t;

/* Get vxlan vpn */
extern int bcm_compat6512_vxlan_vpn_get(int unit, bcm_vpn_t l2vpn,
                                     bcm_compat6512_vxlan_vpn_config_t *info);

/* Create vxlan vpn */
extern int bcm_compat6512_vxlan_vpn_create(int unit,
                                     bcm_compat6512_vxlan_vpn_config_t *info);
#endif

/* VLAN/VSAN Translation Action Set. */
typedef struct bcm_compat6512_fcoe_vsan_action_set_s {
    uint32 flags;                       /* BCM_FCOE_VSAN_ACTION_* */
    bcm_fcoe_vsan_id_t new_vsan;        /* new VSAN ID to replace with */
    uint8 new_vsan_pri;                 /* new VSAN priority to replace with */
    bcm_fcoe_vsan_vft_t vft;            /* remaining fields of VFT header */
    bcm_if_t ingress_if;                /* TBD */
    bcm_fcoe_vsan_action_t dt_vsan;     /* Vsan action when double tagged */
    bcm_fcoe_vsan_action_t dt_vsan_pri; /* VSAN Priority action when double
                                           tagged */
    bcm_fcoe_vsan_action_t it_vsan;     /* Vsan action when single inner tagged */
    bcm_fcoe_vsan_action_t it_vsan_pri; /* Vsan Priority action when single
                                           inner tagged */
    bcm_fcoe_vsan_action_t ot_vsan;     /* Vsan action when single outer tagged */
    bcm_fcoe_vsan_action_t ot_vsan_pri; /* Vsan Priority action when single
                                           outer tagged */
    bcm_fcoe_vsan_action_t ut_vsan;     /* Vsan action when un-tagged */
    bcm_fcoe_vsan_action_t ut_vsan_pri; /* Vsan Priority action when un-tagged */
} bcm_compat6512_fcoe_vsan_action_set_t;

/* Add VSAN VLAN action */
extern int bcm_compat6512_fcoe_vsan_translate_action_add(
    int unit, bcm_fcoe_vsan_translate_key_config_t *key,
    bcm_compat6512_fcoe_vsan_action_set_t *action);

/* Device-independent L2 address structure. */
typedef struct bcm_compat6512_l2_addr_s {
    uint32 flags;                       /* BCM_L2_xxx flags. */
    uint32 flags2;                      /* BCM_L2_FLAGS2_xxx flags. */
    uint32 station_flags;               /* BCM_L2_STATION_xxx flags. */
    bcm_mac_t mac;                      /* 802.3 MAC address. */
    bcm_vlan_t vid;                     /* VLAN identifier. */
    int port;                           /* Zero-based port number. */
    int modid;                          /* XGS: modid. */
    bcm_trunk_t tgid;                   /* Trunk group ID. */
    bcm_cos_t cos_dst;                  /* COS based on dst addr. */
    bcm_cos_t cos_src;                  /* COS based on src addr. */
    bcm_multicast_t l2mc_group;         /* XGS: index in L2MC table. */
    bcm_if_t egress_if;                 /* XGS: index in Next Hop Tables. Used
                                           it with BCM_L2_FLAGS2_ROE_NHI flag */
    bcm_pbmp_t block_bitmap;            /* XGS: blocked egress bitmap. */
    int auth;                           /* Used if auth enabled on port. */
    int group;                          /* Group number for FP. */
    bcm_fabric_distribution_t distribution_class; /* Fabric Distribution Class. */
    int encap_id;                       /* out logical interface */
    int age_state;                      /* Age state of the entry */
    uint32 flow_handle;                 /* FLOW handle for flex entries. */
    uint32 flow_option_handle;          /* FLOW option handle for flex entries. */
    bcm_flow_logical_field_t logical_fields[BCM_FLOW_MAX_NOF_LOGICAL_FIELDS]; /* logical fields array for flex
                                           entries. */
    uint32 num_of_fields;               /* number of logical fields. */
    bcm_pbmp_t sa_source_filter_pbmp;   /* Source port filter bitmap for this SA */
    bcm_tsn_flowset_t tsn_flowset;      /* Time-Sensitive Networking: associated
                                           flow set */
    bcm_tsn_sr_flowset_t sr_flowset;    /* Seamless Redundancy: associated flow
                                           set */
    bcm_policer_t policer_id;           /* Base policer to be used */
    bcm_tsn_pri_map_t taf_gate_primap;  /* TAF (Time Aware Filtering) gate
                                           priority mapping */
} bcm_compat6512_l2_addr_t;

extern int bcm_compat6512_l2_addr_add(int unit,
                                      bcm_compat6512_l2_addr_t *l2addr);
extern int bcm_compat6512_l2_addr_multi_add(int unit,
                                            bcm_compat6512_l2_addr_t *l2addr, int count);
extern int bcm_compat6512_l2_addr_multi_delete(int unit,
                                               bcm_compat6512_l2_addr_t *l2addr, int count);
extern int bcm_compat6512_l2_addr_get(int unit, bcm_mac_t mac_addr, bcm_vlan_t vid,
                                      bcm_compat6512_l2_addr_t *l2addr);
extern int bcm_compat6512_l2_conflict_get(int unit, bcm_compat6512_l2_addr_t *addr,
                                          bcm_compat6512_l2_addr_t *cf_array, int cf_max,
                                          int *cf_count);
extern int bcm_compat6512_l2_replace(int unit, uint32 flags,
                                     bcm_compat6512_l2_addr_t *match_addr,
                                     bcm_module_t new_module,
                                     bcm_port_t new_port, bcm_trunk_t new_trunk);
extern int bcm_compat6512_l2_replace_match(int unit, uint32 flags,
                                           bcm_compat6512_l2_addr_t *match_addr,
                                           bcm_compat6512_l2_addr_t *mask_addr,
                                           bcm_compat6512_l2_addr_t *replace_addr,
                                           bcm_compat6512_l2_addr_t *replace_mask_addr);
extern int bcm_compat6512_l2_stat_get(int unit,
                                      bcm_compat6512_l2_addr_t *l2addr,
                                      bcm_l2_stat_t stat, uint64 *val);
extern int bcm_compat6512_l2_stat_get32(int unit,
                                        bcm_compat6512_l2_addr_t *l2addr,
                                        bcm_l2_stat_t stat, uint32 *val);
extern int bcm_compat6512_l2_stat_set(int unit,
                                      bcm_compat6512_l2_addr_t *l2addr,
                                      bcm_l2_stat_t stat, uint64 val);
extern int bcm_compat6512_l2_stat_set32(int unit,
                                        bcm_compat6512_l2_addr_t *l2addr,
                                        bcm_l2_stat_t stat, uint32 val);
extern int bcm_compat6512_l2_stat_enable_set(int unit,
                                             bcm_compat6512_l2_addr_t *l2addr,
                                             int enable);
#endif
#endif	/* !_COMPAT_6512_H */
