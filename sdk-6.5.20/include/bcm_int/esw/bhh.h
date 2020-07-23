
/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef __BCM_INT_ESW_BHH_H__
#define __BCM_INT_ESW_BHH_H__
#if defined(INCLUDE_BHH)

#include <soc/uc_msg.h>
#include <soc/shared/mos_msg_common.h>
#include <soc/shared/bhh.h>
#include <soc/shared/bhh_pkt.h>
#include <soc/shared/bhh_msg.h>
#include <soc/shared/bhh_pack.h>

#include <bcm_int/common/rx.h>
#include <bcm/oam.h>

#if defined(BCM_TRIUMPH_SUPPORT)
#include <bcm_int/esw/triumph.h>
#endif /* BCM_TRIUMPH_SUPPORT */

/*
 * BHH Encapsulation Definitions
 *
 * Defines for building the BHH packet encapsulation
 */

#define BHH_THREAD_PRI_DFLT     200
#define BCM_KT_BHH_RX_CHANNEL    1 /* this rx dma channel is shared with BFD */
#define BCM_BHH_ENDPOINT_MAX_MEP_ID_LENGTH 32

/**************************************************************************************/
#define _BHH_UC_MSG_TIMEOUT_USECS          5000000     /* 5 seconds for FW response */

#define BCM_BHH_ENDPOINT_PASSIVE            0x0004     /* Specifies endpoint
                                                          takes passive role */
#define BCM_BHH_ENDPOINT_DEMAND             0x0008     /* Specifies local*/
#define BCM_BHH_ENDPOINT_ENCAP_SET          0x0010     /* Update encapsulation
                                                          on existing BFD
                                                          endpoint */
/*
 * BHH Encapsulation Format Header flags
 *
 * Indicates the type of headers/labels present in a BHH packet.
 */
#define _BHH_ENCAP_PKT_MPLS                    (1 << 0) 
#define _BHH_ENCAP_PKT_MPLS_ROUTER_ALERT       (1 << 1) 
#define _BHH_ENCAP_PKT_MPLS_BOTTOM             (1 << 2) 
#define _BHH_ENCAP_PKT_PW                      (1 << 3) 
#define _BHH_ENCAP_PKT_GAL                     (1 << 4) 
#define _BHH_ENCAP_PKT_G_ACH                   (1 << 5) 
#define _BHH_ENCAP_PKT_BHH                     (1 << 11)


/*
 * Macros to pack uint8, uint16, uint32 in Network byte order
 */
#define _BHH_ENCAP_PACK_U8(_buf, _var)   SHR_BHH_ENCAP_PACK_U8(_buf, _var)
#define _BHH_ENCAP_PACK_U16(_buf, _var)  SHR_BHH_ENCAP_PACK_U16(_buf, _var)
#define _BHH_ENCAP_PACK_U32(_buf, _var)  SHR_BHH_ENCAP_PACK_U32(_buf, _var)

#define _BHH_MAC_ADDR_LENGTH    (sizeof(bcm_mac_t))
#define _BHH_VLAN_MASK    0x00000FFF

#define _BHH_MPLS_MAX_LABELS    4   /* Max MPLS labels in Katana */
#define _BHH_FP_OAM_MPLS_MAX_LABELS    9   /* Max MPLS labels in Apache */

#define _BHH_MPLS_DFLT_TTL      16

/* Group ID + Endpoint Name + Level + Gport + VID(outer + inner) + MPLS label*/
#define _BHH_FP_OAM_HASH_KEY_SIZE (sizeof(bcm_oam_group_t)         +   \
                                   sizeof(uint16)                  +   \
                                   sizeof(int)                     +   \
                                   sizeof(bcm_gport_t)             +   \
                                   sizeof(bcm_vlan_t)              +   \
                                   sizeof(bcm_vlan_t)              +   \
                                   sizeof(bcm_mpls_label_t))

typedef uint8 _bhh_fp_oam_hash_key_t[_BHH_FP_OAM_HASH_KEY_SIZE];

#define _BHH_FP_OAM_INVALID_LMEP_ID     (-1)

#define _BCM_OAM_BHH_DEFAULT_ENCAP_LENGTH 76 /* OLP Tx(34)        + L2(22) +
                                                4 MPLS labels(16) + ACH(4) */
#define _BCM_OAM_BHH_MAX_ENCAP_LENGTH 96     /* OLP Tx(34)        + L2(22) +
                                                9 MPLS labels(36) + ACH(4) */

/*
 * Typedef:
 *     _bcm_oam_hash_data_t
 * Purpose:
 *     Endpoint hash table data structure.
 */
typedef struct _bhh_fp_oam_ep_data_s {
    int in_use;                             /* Endpoint status */
    bcm_oam_endpoint_type_t type;           /* Endpoint Type */
    bcm_oam_endpoint_t  ep_id;              /* Endpoint ID                   */
    uint8               is_remote;          /* Local or Remote MEP.          */
    uint16              name;               /* Endpoint name.                */
    uint8               level;              /* Maintenance domain level.     */
    bcm_vlan_t          vlan;               /* Vlan membership.              */
    bcm_vlan_t          inner_vlan;         /* C Vlan membership -when double
                                                                       tagged*/
    bcm_gport_t         gport;              /* Sw Generic Port.              */
    uint32              sglp;               /* Hw Src GLP CCM Rx port/trunk. */
    uint32              dglp;               /* Hw Dest GLP for CCM Tx.       */
    bcm_oam_group_t     group_index;        /* Group ID.                     */
    int                 remote_index;       /* RMEP entry hardware index.    */
    int                 lm_counter_index;   /* LM counter index.             */
    uint32              vp;                 /* Virtual Port.                 */
    uint32              flags;              /* Endpoint flags.               */
    uint32              flags2;             /* Endpoint flags2.              */
    uint32              period;             /* CCM interval encoding.        */
    bcm_mpls_label_t    label;              /* incoming inner label for
                                               either LSP or PW      */
    uint8               vlan_pri;           /* VLAN tag priority             */ 
    bcm_if_t            egress_if;          /* egress interface              */
    bcm_cos_t           int_pri;            /* internal priority for
                                               outgoing CCM packets          */
    int                 trunk_index;        /* The trunk port index for this
                                               endpoint */
    int                 outer_tpid;         /* Outer TPID value              */
    int                 inner_tpid;         /* Inner TPID value              */
    bcm_trunk_t         trunk_id;            /* Trunk ID of the gport        */
    uint32              int_flags;           /* Internal flags required for
                                                indicating special cases */
    bcm_mac_t           src_mac_address;    /* The source MAC address associated
                                               with this endpoint */
    bcm_mac_t           dst_mac_address;    /* The destination MAC address
                                               associated with this endpoint */
    uint8               inner_vlan_pri;     /* Inner VLAN tag priority       */
    uint32              uc_session_id;      /* Session Id on UC for this EP */
    bcm_mpls_label_t    egr_label;          /* Outgoing label for BHH */
    uint8               egr_label_exp;      /* Outgoing exp for MPLS Label */
    uint8               egr_label_ttl;      /* Outgoing ttl for MPLS Label */
    bcm_oam_vccv_type_t vccv_type;          /* VCCV pseudowire type          */
    bcm_vpn_t           vpn;
    bcm_oam_pm_profile_t pm_profile_attached;
    /*As FW will not store exact software counters passed during API calls 
      storing in SDK for get operations*/
    uint32 ccm_tx_update_lm_counter_base_id [BCM_OAM_LM_COUNTER_MAX]; 
                                            /* Base LM counter id to be */
                                            /*incremented by Tx CCM packets */
    uint32 ccm_tx_update_lm_counter_offset[BCM_OAM_LM_COUNTER_MAX]; 
                                            /* Offset to the Base LM counter */
                                            /*Id to be incremented by Tx */
                                            /*CCM packets */
    uint8 ccm_tx_counter_size;              /* Number of LM counters to be */
                                            /*incremented by Tx CCM packets */
    uint32 lm_counter_base_id [BCM_OAM_LM_COUNTER_MAX]; 
                                            /* Base LM counter id to be */
                                            /*incremented by Tx LM packets */
    uint32 lm_counter_offset[BCM_OAM_LM_COUNTER_MAX]; 
                                            /* Offset to the Base LM counter */
                                            /*Id to be incremented by Tx */
                                            /*LM packets */
    uint32 lm_counter_action[BCM_OAM_LM_COUNTER_MAX];/*Action assoiciated with 
                                                       each LM counter*/ 
    uint8 lm_counter_size;                  /* Number of LM counters to be */
                                            /*incremented by Tx LM packets */
    uint32 lb_tx_update_lm_counter_base_id [BCM_OAM_LM_COUNTER_MAX]; 
                                            /* Base LM counter id to be */
                                            /*incremented by Tx LB packets */
    uint32 lb_tx_update_lm_counter_offset[BCM_OAM_LM_COUNTER_MAX]; 
                                            /* Offset to the Base LM counter */
                                            /*Id to be incremented by Tx */
                                            /*LB packets */
    uint8 lb_tx_counter_size;              /* Number of LM counters to be */
                                            /*incremented by Tx LB packets */
    uint32 dm_tx_update_lm_counter_base_id [BCM_OAM_LM_COUNTER_MAX]; 
                                            /* Base LM counter id to be */
                                            /*incremented by Tx DM packets */
    uint32 dm_tx_update_lm_counter_offset[BCM_OAM_LM_COUNTER_MAX]; 
                                            /* Offset to the Base LM counter */
                                            /*Id to be incremented by Tx */
                                            /*DM packets */
    uint8 dm_tx_counter_size;              /* Number of LM counters to be */
                                            /*incremented by Tx DM packets */
    uint32 csf_tx_update_lm_counter_base_id [BCM_OAM_LM_COUNTER_MAX];
                                            /* Base LM counter id to be */
                                            /*incremented by Tx CSF packets */
    uint32 csf_tx_update_lm_counter_offset[BCM_OAM_LM_COUNTER_MAX];
                                            /* Offset to the Base LM counter */
                                            /*Id to be incremented by Tx */
                                            /*CSF packets */
    uint8 csf_tx_counter_size;              /* Number of LM counters to be */
                                            /*incremented by Tx CSF packets */
    uint32              hw_session_id;      /* HW provided Session Id */
    uint8               hw_session_num_entries; /* HW provided number of */
                                                /* Session Ids */
    bcm_gport_t         resolved_trunk_gport;/* Resolved Trunk SW Generic Port*/
}_bhh_fp_oam_ep_data_t;

/* BHH Group data */
typedef struct _bhh_fp_oam_group_data_s {
    int         in_use;                         /* Group status */
    uint32      flags;                          /* Group Flags */
    int         group_id;                       /* Group id */
    int         lmep_id;                        /* LMEP IDx */
    uint32      faults;                         /* Group Faults */
    uint8       name[BCM_OAM_GROUP_NAME_LENGTH];/* Group name */
} _bhh_fp_oam_group_data_t;


/* BHH LM/DM information */
typedef struct  _bhh_oam_lm_dm_info_s {
    uint32 counter_base_id [BCM_OAM_LM_COUNTER_MAX]; 
                                            /* Base LM counter id to be */
                                            /*incremented by Tx packets */
    uint32 counter_offset[BCM_OAM_LM_COUNTER_MAX]; 
                                            /* Offset to the Base LM counter */
                                            /*Id to be incremented by Tx */
                                            /*packets */
    int    counter_action[BCM_OAM_LM_COUNTER_MAX];/* Counter action */
    uint32 byte_count_offset[BCM_OAM_LM_COUNTER_MAX]; /* Byte count offset*/
    uint8 counter_size;              /* Number of LM counters to be */
                                     /*incremented by Tx packets */
    uint8 ts_mode;   /* Timestamp mode */
    uint8 oam_replacement_offset;   /*Offset to placement of counter/timestamp*/
} _bhh_oam_lm_dm_info_t;

/* ACH TLV Header */
typedef struct _ach_tlv_s {
    struct {
        uint16  length;      /* 16: Length (octets) of following TLVs */
        uint16  reserved;    /* 16: Reserved, must be 0 */
    } header;
    uint8 tlv[BCM_BHH_ENDPOINT_MAX_MEP_ID_LENGTH];
} _ach_tlv_t;

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
    bcm_mac_t    dst_mac;        /* 48: Destination MAC */
    bcm_mac_t    src_mac;        /* 48: Source MAC */
    _vlan_tag_t  vlan_tag;       /* VLAN Tag */
    _vlan_tag_t  vlan_tag_inner; /* Inner VLAN Tag */
    uint16       etype;          /* 16: Ether Type */
} _l2_header_t;


#endif 
#endif
