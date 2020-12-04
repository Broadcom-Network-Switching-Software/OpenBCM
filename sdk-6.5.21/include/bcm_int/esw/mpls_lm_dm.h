/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef __BCM_INT_ESW_MPLS_LM_DM_H__
#define __BCM_INT_ESW_MPLS_LM_DM_H__
#if defined(INCLUDE_MPLS_LM_DM)

#include <soc/uc_msg.h>
#include <soc/shared/mos_msg_common.h>
#include <soc/shared/mpls_lm_dm_pkt.h>
#include <soc/shared/mpls_lm_dm_msg.h>
#include <soc/shared/mpls_lm_dm_pack.h>

#include <bcm_int/common/rx.h>

#if defined(BCM_TRIUMPH_SUPPORT)
#include <bcm_int/esw/triumph.h>
#endif /* BCM_TRIUMPH_SUPPORT */

/*
 * MPLS_LM_DM Encapsulation Definitions
 *
 * Defines for building the MPLS_LM_DM packet encapsulation
 */

#define MPLS_LM_DM_THREAD_PRI_DFLT           200
#define MPLS_LMDM_EVENT_THREAD_PRIORITY      200

#define _MPLS_LM_DM_UC_MSG_TIMEOUT_USECS    5000000 /* 5 sec for FW response */

/*
 * MPLS_LM_DM Encapsulation Format Header flags
 *
 * Indicates the type of headers/labels present in a MPLS_LM_DM packet.
 * Bottom 4 bits are reserved to indicate the GACH Type (A-E)
 */
/* ACH type Mask */
#define _MPLS_LM_DM_ACH_TYPE_MASK                     0x000F

#define _MPLS_LM_DM_ENCAP_PKT_MPLS_LSP                (1 << 4) 
#define _MPLS_LM_DM_ENCAP_PKT_MPLS_ROUTER_ALERT       (1 << 5) 
#define _MPLS_LM_DM_ENCAP_PKT_MPLS_BOTTOM             (1 << 6) 
#define _MPLS_LM_DM_ENCAP_PKT_PW                      (1 << 7) 
#define _MPLS_LM_DM_ENCAP_PKT_GAL                     (1 << 8) 
#define _MPLS_LM_DM_ENCAP_PKT_G_ACH                   (1 << 9)


#define _MPLS_LM_DM_MPLS_DFLT_TTL      16

#ifndef INCLUDE_BHH
/* ACH - Associated Channel Header */
typedef struct _ach_header_s {
    uint8   f_nibble;        /*  4: First nibble, must be 1 */
    uint8   version;         /*  4: Version */
    uint8   reserved;        /*  8: Reserved */
    uint16  channel_type;    /* 16: Channel Type */
} _ach_header_t;

/* MPLS - Multiprotocol Label Switching Label */
typedef struct _mpls_label_s {
    uint32  label;    /* 20: Label */
    uint8   exp;      /*  3: Experimental, Traffic Class, ECN */
    uint8   s;        /*  1: Bottom of Stack */
    uint8   ttl;      /*  8: Time to Live */
} _mpls_label_t;

/* VLAN Tag - 802.1Q */
typedef struct _vlan_tag_s {
    uint16      tpid;    /* 16: Tag Protocol Identifier */
    struct {
        uint8   prio;    /*  3: Priority Code Point */
        uint8   cfi;     /*  1: Canonical Format Indicator */
        uint16  vid;     /* 12: Vlan Identifier */
    } tci;               /* Tag Control Identifier */
} _vlan_tag_t;

/* L2 Header */
typedef struct _l2_header_t {
    bcm_mac_t    dst_mac;           /* 48: Destination MAC */
    bcm_mac_t    src_mac;           /* 48: Source MAC */
    _vlan_tag_t  vlan_tag;          /* VLAN Tag */
    _vlan_tag_t  vlan_tag_inner;    /* VLAN Tag */
    uint16       etype;             /* 16: Ether Type */
} _l2_header_t;
#endif /* !INCLUDE_BHH */

/*
 * Typedef:
 *     _bcm_fp_oam_mpls_lm_dm_ep_data_s
 * Purpose:
 *     MPLS LM/DM EP data structure.
 */
typedef struct _bcm_fp_oam_mpls_lm_dm_ep_data_s {
    uint8                      in_use;
    bcm_oam_endpoint_t         id;
    uint32                     flags;
    bcm_oam_endpoint_type_t    type;
    bcm_oam_vccv_type_t        vccv_type;
    bcm_gport_t                gport;
    bcm_gport_t                tx_gport;
    uint8                      trunk_index;
    bcm_mac_t                  dst_mac;
    bcm_mac_t                  src_mac;
    bcm_vlan_t                 outer_vlan;
    uint16                     outer_tpid;
    uint8                      outer_pri;
    bcm_vlan_t                 inner_vlan;
    uint16                     inner_tpid;
    uint8                      inner_pri;
    bcm_mpls_label_t           egress_label;
    bcm_if_t                   intf_id;
    bcm_vpn_t                  vpn;
    uint8                      lm_exp;
    bcm_cos_t                  lm_int_pri;
    uint32                     lm_ctr_base_id[BCM_OAM_LM_COUNTER_MAX];
    uint8                      lm_ctr_offset[BCM_OAM_LM_COUNTER_MAX];
    uint8                      lm_ctr_action[BCM_OAM_LM_COUNTER_MAX];
    uint8                      lm_ctr_size;
    uint8                      dm_exp;
    bcm_cos_t                  dm_int_pri;
    uint32                     dm_ctr_base_id[BCM_OAM_LM_COUNTER_MAX];
    uint8                      dm_ctr_offset[BCM_OAM_LM_COUNTER_MAX];
    uint8                      dm_ctr_size;
    uint32                     session_id;
    uint8                      session_num_entries;
    uint16                     hw_base_session_id;
    bcm_oam_pm_profile_t       pm_profile;
} _bcm_fp_oam_mpls_lm_dm_ep_data_t;

#endif /* INCLUDE_MPLS_LM_DM */
#endif /* __BCM_INT_ESW_MPLS_LM_DM_H__ */
