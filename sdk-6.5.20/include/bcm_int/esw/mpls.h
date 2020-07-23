/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains mpls definitions internal to the BCM library.
 */

#ifndef _BCM_INT_MPLS_H
#define _BCM_INT_MPLS_H

#include <bcm_int/esw/vpn.h>
#include <bcm/mpls.h>

#define _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_MASK    0x300
#define _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS 0x100
#define _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_EGRESS  0x300
#define _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_EGRESS_L2  0x200
#define _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK     0x0ff

#define _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit)  \
    (((soc_feature(unit, soc_feature_egr_ip_tnl_mpls_double_wide) \
    || soc_feature(unit, soc_feature_th3_style_simple_mpls)))? 8 : 4)

#define _BCM_MPLS_EGR_L3_INTF_MPLS_INDEX_OFFSET_MASK(unit)  \
    (((soc_feature(unit, soc_feature_egr_ip_tnl_mpls_double_wide)  \
    || soc_feature(unit, soc_feature_th3_style_simple_mpls)))? 0x7 : 0x3)

/*
 * MPLS port state - need to remember where to find the port.
 * Flags indicate the type of the port and hence the tables where the data is.
 * The match attributes are the keys for the various hash tables.
 */
#define _BCM_MPLS_PORT_MATCH_TYPE_NONE         (1 << 0)
#define _BCM_MPLS_PORT_MATCH_TYPE_VLAN   (1 << 1)
#define _BCM_MPLS_PORT_MATCH_TYPE_INNER_VLAN (1 << 2)
#define _BCM_MPLS_PORT_MATCH_TYPE_VLAN_STACKED        (1 << 3)
#define _BCM_MPLS_PORT_MATCH_TYPE_VLAN_PRI  (1 << 4)
#define _BCM_MPLS_PORT_MATCH_TYPE_PORT         (1 << 5)
#define _BCM_MPLS_PORT_MATCH_TYPE_TRUNK       (1 << 6)
#define _BCM_MPLS_PORT_MATCH_TYPE_LABEL        (1 << 7)
#define _BCM_MPLS_PORT_MATCH_TYPE_LABEL_PORT     (1 << 8)
#define _BCM_MPLS_PORT_MATCH_TYPE_PORT_SUBPORT_PKT_VID (1 << 9)
#define _BCM_MPLS_PORT_MATCH_TYPE_PORT_SUBPORT_PKT_VID_INNER_VLAN (1 << 10)
#define _BCM_MPLS_PORT_MATCH_TYPE_PORT_SUBPORT_PKT_VID_OUTER_VLAN (1 << 11)
#define _BCM_MPLS_PORT_MATCH_TYPE_SHARE (1 << 12)
#define _BCM_MPLS_PORT_MATCH_TYPE_VLAN_TAG   (1 << 13)
#define _BCM_MPLS_PORT_MATCH_TYPE_INNER_VLAN_TAG   (1 << 14)

/*
 * MPLS LABEL ACTION - indicate the ACTION associated with the LABEL 
 */

#define _BCM_MPLS_ACTION_NOOP  0x0
#define _BCM_MPLS_ACTION_PUSH  0x1
#define _BCM_MPLS_ACTION_SWAP  0x2
#define _BCM_MPLS_ACTION_RESERVED  0x3
#define _BCM_MPLS_ACTION_PRESERVED 0x3
#define _BCM_MPLS_ACTION_SEQUENCE 0x4
#define _BCM_MPLS_ACTION_MASK (0xFFFF)
#define _BCM_MPLS_ACTION_LABEL_ALLOC_SHIFT (16)
#define _BCM_MPLS_ACTION_LABEL_ALLOC (1 << _BCM_MPLS_ACTION_LABEL_ALLOC_SHIFT)
#define _BCM_MPLS_ACTION_GET(_f) ((_f) & _BCM_MPLS_ACTION_MASK)

#define _BCM_MPLS_EGRESS_LABEL_PRESERVE(unit, flags) \
            (soc_feature(unit, soc_feature_mpls_swap_label_preserve) && \
            (flags & BCM_MPLS_EGRESS_LABEL_PRESERVE))

/*
 * MPLS FORWARDING ACTION - indicate the FORWARDING ACTION associated with the LABEL 
 */
#define _BCM_MPLS_FORWARD_NEXT_HOP  0x0
#define _BCM_MPLS_FORWARD_ECMP_GROUP  0x1
#define _BCM_MPLS_FORWARD_MULTICAST_GROUP  0x2

typedef struct _bcm_tr_mpls_match_port_info_s {
    uint32     flags;
    uint32     index; /* Index of memory where port info is programmed */
    bcm_trunk_t trunk_id; /* Trunk group ID */
    bcm_module_t modid; /* Module id */
    bcm_port_t port; /* Port */
    bcm_vlan_t match_vlan;
    bcm_vlan_t match_inner_vlan;
    bcm_vlan_t match_subport_pkt_vid;
    bcm_mpls_label_t match_label; /* MPLS label */
    int match_count; /* Number of matches from VLAN_XLATE configured */
    bcm_trunk_t fo_trunk_id; /* vp-less failover port Trunk group ID */
    bcm_module_t fo_modid; /* vp-less failover port Module id */
    bcm_port_t fo_port; /* vp-less failover port, flags and label same as primary*/
} _bcm_tr_mpls_match_port_info_t;

typedef struct _bcm_mpls_vpws_vp_map_info_s {
    int             vp1; /* Access VP */
    int             vp2; /* Network VP */
    int             vp3; /* Primary Network VP */
    int             vp_b_1;
    int             vp_b_2;
    int             vp_b_3;
} _bcm_mpls_vpws_vp_map_info_t;

/*
 * Software book keeping for MPLS related information
 */
typedef struct _bcm_tr_mpls_bookkeeping_s {
    int         initialized;        /* Set to TRUE when MPLS module initialized */
    SHR_BITDCL  *vrf_bitmap;        /* VRF bitmap */
    SHR_BITDCL  *vpws_bitmap;       /* VPWS vpn usage bitmap  */
    SHR_BITDCL  *vc_c_bitmap;       /* VC_AND_LABEL index bitmap (1st 4K)*/
    SHR_BITDCL  *vc_nc_bitmap;      /* VC_AND_LABEL index bitmap (2nd 4K)*/
    SHR_BITDCL  *pw_term_bitmap;    /* ING_PW_TERM_COUNTERS index bitmap */
    SHR_BITDCL  *pw_init_bitmap;    /* ING_PW_TERM_COUNTERS index bitmap */
    SHR_BITDCL  *tnl_bitmap;        /* EGR_IP_TUNNEL_MPLS index bitmap */
    SHR_BITDCL  *ip_tnl_bitmap;     /* EGR_IP_TUNNEL index bitmap */
    SHR_BITDCL  *egr_mpls_bitmap;   /* Egress EXP/PRI maps usage bitmap */
    uint32      *egr_mpls_hw_idx;   /* Actual profile number used */
    SHR_BITDCL  *ing_exp_map_bitmap;/* Ingress EXP map usage bitmap */
    SHR_BITDCL  *egr_l2_exp_map_bitmap; /* Egress_L2 EXP map usage bitmap */
#ifdef BCM_APACHE_SUPPORT
    SHR_BITDCL  *ing_exp_to_phb_cng_map_bitmap;/* Ingress EXP to PHB and CNG */
                                               /* map usage bitmap */
    SHR_BITDCL  *egr_exp_to_phb_cng_map_bitmap;/* Egress EXP to PHB and CNG */
                                               /* map usage bitmap */
#endif
    sal_mutex_t mpls_mutex;	        /* Protection mutex. */
    _bcm_tr_mpls_match_port_info_t *match_key; /* Match Key for Dual Hash */
    uint16       *vc_swap_ref_count; /* Reference count for VC_AND_SWAP entries */
    uint16       *egr_tunnel_ref_count; /* Reference count for EGR_MPLS_TUNNEL entries */
    uint16       *ing_qmap_ref_count; /* Reference count for ING_QUEUE_MAP entries */
    _bcm_mpls_vpws_vp_map_info_t       *vpws_vp_map; /* VP map for VPWS */
    int          *trunk_vp_map;     /* VP map for trunk binding */
    SHR_BITDCL  *svp_profile_set_bitmap; /* SOURCE_VP dscp & dot1p prifile set bitmap */
    SHR_BITDCL  *network_vp_bitmap; /* Network side VP bitmap */
#if defined(BCM_TRIDENT3_SUPPORT)
    int *egr_vc_swap_lbl_action; /*store label actions in TD3 to make it work like TD2+*/
#endif
    uint32       *failover_vp; /* failover vp for primary vp */

  /*
   * On some chips (e.g., TR3/TD2/TD2+), EGR_DVP_ATTRIBUTE_1 is overlaid on
   * EGR_MPLS_VC_AND_SWAP_LABEL_TABLE.
   * This is to record the offset of EGR_DVP_ATTRIBUTE_1 table to the start of
   * EGR_MPLS_VC_AND_SWAP_LABEL_TABLE.
   */
    uint32        overlaid_dvp_offset_start;
    uint32        overlaid_dvp_offset_end;
} _bcm_tr_mpls_bookkeeping_t;

extern _bcm_tr_mpls_bookkeeping_t  _bcm_tr_mpls_bk_info[BCM_MAX_NUM_UNITS];

extern int _bcm_esw_mpls_port_source_vp_lag_set(
                                    int unit,
                                    bcm_gport_t gport,
                                    int vp_lag_vp);
extern int _bcm_esw_mpls_port_source_vp_lag_clear(
                                    int unit,
                                    bcm_gport_t gport,
                                    int vp_lag_vp);
extern int _bcm_esw_mpls_port_source_vp_lag_get(
                                    int unit,
                                    bcm_gport_t gport,
                                    int *vp_lag_vp);


#ifdef INCLUDE_L3 

extern int _bcm_esw_mpls_label_stat_counter_get(int unit, int sync_mode, 
                                                bcm_mpls_label_t label, 
                                                bcm_gport_t port,
                                                bcm_mpls_stat_t stat, 
                                                uint32 num_entries,
                                                uint32 *counter_indexes, 
                                              bcm_stat_value_t *counter_values);
#endif /* INCLUDE_L3 */                                              

/* MPLS VC_AND_SWAP NextHop Indices List.  */
typedef struct _bcm_mpls_vp_nh_list_s {
    int  vp_nh_idx;
    struct _bcm_mpls_vp_nh_list_s  *link;
}_bcm_mpls_vp_nh_list_t;

/* MPLS Egress NhopList */
typedef struct _bcm_mpls_egr_nhopList_s {
    struct _bcm_mpls_egr_nhopList_s	*link;
    bcm_if_t  egr_if;                        /* Egress Object If */
    _bcm_mpls_vp_nh_list_t	*vp_head_ptr;
}_bcm_mpls_egr_nhopList_t;

typedef struct _bcm_mpls_old_nh_info_s {
    int old_nh_index;
    int old_tpid_idx;
    int old_macda_idx;
    int old_vc_swap_idx;
    bcm_if_t old_tunnel_if;
} _bcm_mpls_old_nh_info_t;

#ifdef BCM_TRIDENT3_SUPPORT
/*EGR_SEQUENCE_NUMBER_TABLE is shared by IP_TUNNEL,VC_SWAP tables.
 *For VC_SWAP table offset starts after IP_TUNNEL range as per cancun register
 *EGR_SEQUENCE_NUMBER_CTRL*/
#define EGR_PW_SEQNUM_VCSWAP_OFFSET(_unit) \
                    (soc_mem_index_count(unit, EGR_IP_TUNNELm))
#endif
/***************************************************
 ***************************************************
 ********                                   ********
 ******                                       ******
 ****    Segment Routing Feature Defines        ****
 ******              START                    ******
 */

/*
 * The below indexes derive how many free entries
 * you want to keep at a give amount of time.
 * you may not want to know everything available
 * as you may not need all and it is too memmory
 * consuming.
 * 8 will work well as you need max 8 single entries
 * to fill largest entry in current h/w configuration.
 */
#define MAX_ENTRY_INDEXES                             8
#define MAX_FREE_ENTRY_INDEXES                        8


#define BCMI_EGR_IP_TUNNEL_MPLS_START_OF_ENTRY        (1 << 0)
#define BCMI_EGR_IP_TUNNEL_MPLS_ENTRY_USED            (1 << 1)
#define BCMI_EGR_IP_TUNNEL_MPLS_ENTRY_CHECKED         (1 << 2)
#define BCMI_EGR_IP_TUNNEL_MPLS_ENTRY_ZERO            (1 << 3)

/* S/w list of all interfaces pointing to mpls tnl */
typedef struct intf_list_s {
    bcm_if_t intf_num;
    struct intf_list_s *next;
} intf_list_t;

/* s/w state of each mpls tnl entry */
typedef struct bcmi_egr_tnl_mpls_label_entry_s {
    intf_list_t *intf_list; /* associated interface list */
    int flags;              /* flags */
    int num_elements;       /* Number of elements in this tnl */
} bcmi_egr_ip_tnl_mpls_label_entry_t;


typedef struct bcmi_egr_tnl_mpls_tunnel_entry_s {
    bcmi_egr_ip_tnl_mpls_label_entry_t **label_entry; /*Label entries in Tunnel */
} bcmi_egr_ip_tnl_mpls_tunnel_entry_t;


typedef struct bcmi_egr_ip_tnl_free_indexes_s {
    /* free indexes of each entry type */
    int free_entry_indexes[MAX_ENTRY_INDEXES][MAX_FREE_ENTRY_INDEXES];
    /* count of free indexes of each entry type */
    int free_index_count[MAX_ENTRY_INDEXES];
} bcmi_egr_ip_tnl_free_indexes_t;

/***************************************************
 ***************************************************
 ********                                   ********
 ******                                       ******
 ****      Segment Routing Feature Defines      ****
 ******               END                     ******
 */

#define _BCM_MPLS_VPN_TYPE_L3           _BCM_VPN_TYPE_L3
#define _BCM_MPLS_VPN_TYPE_VPWS         _BCM_VPN_TYPE_VPWS
#define _BCM_MPLS_VPN_TYPE_VPLS         _BCM_VPN_TYPE_VFI

#define _BCM_MPLS_VPN_SET(_vpn_, _type_, _id_) \
        _BCM_VPN_SET(_vpn_, _type_, _id_)

#define _BCM_MPLS_VPN_GET(_id_, _type_,  _vpn_) \
        _BCM_VPN_GET(_id_, _type_, _vpn_)

#define _BCM_MPLS_VPN_IS_L3(_vpn_)   _BCM_VPN_IS_L3(_vpn_)

#define _BCM_MPLS_VPN_IS_VPLS(_vpn_) _BCM_VPN_IS_VPLS(_vpn_)

#define _BCM_MPLS_VPN_IS_VPWS(_vpn_) _BCM_VPN_IS_VPWS(_vpn_)

#define _BCM_MPLS_VPN_IS_SET(_vpn_)    \
        (_BCM_MPLS_VPN_IS_L3(_vpn_) \
        || _BCM_MPLS_VPN_IS_VPLS(_vpn_) \
        || _BCM_MPLS_VPN_IS_VPWS(_vpn_))

#define _BCM_MPLS_GPORT_FAILOVER_VPLESS  (1 << 24)
#define _BCM_MPLS_GPORT_FAILOVER_VPLESS_SET(_gp) ((_gp) | _BCM_MPLS_GPORT_FAILOVER_VPLESS)
#define _BCM_MPLS_GPORT_FAILOVER_VPLESS_GET(_gp) ((_gp) & _BCM_MPLS_GPORT_FAILOVER_VPLESS)
#define _BCM_MPLS_GPORT_FAILOVER_VPLESS_CLEAR(_gp) ((_gp) & (~_BCM_MPLS_GPORT_FAILOVER_VPLESS))

/* 
 * L3_IIF table is used to get the VRF & classid and is 
 * carved out as follows:
 *      0 - 4095 : 4K entries indexed by VLAN for non-tunnel/non-mpls packets
 *   4096 - 4607 : 512 entries directly indexed from L3_TUNNEL
 *   4608 - 6655 : 2K entries for MPLS pop & route packet (one for each VRF)
 *   6656 - 8191 : unused
 */
#define _BCM_TR_MPLS_L3_IIF_BASE        4095

/*
 * Index 63 of the ING_PRI_CNG_MAP table is a special value
 * in HW indicating no to trust the packet's 802.1p pri/cfi.
 * Here, we use index 62 as an identity mapping
 * (pkt_pri => int_pri) and (pkt_cfi => int_color)
 */
#define _BCM_TR_MPLS_PRI_CNG_MAP_IDENTITY   62
#define _BCM_TR_MPLS_PRI_CNG_MAP_NONE       63
#define _BCM_TR_MPLS_HASH_ELEMENTS 1024

#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void _bcm_mpls_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#if defined(BCM_WARM_BOOT_SUPPORT) && defined(INCLUDE_L3)
extern int _bcm_esw_mpls_sync(int unit);
extern int _bcm_esw_mpls_match_key_recover(int unit);
#ifdef INCLUDE_L3
extern int _bcm_esw_mpls_tunnel_switch_traverse_no_lock(int unit,
               bcm_mpls_tunnel_switch_traverse_cb cb, void *user_data);
#endif
#endif

/*
 * VRF table usage bitmap operations
 */
#define _BCM_MPLS_VRF_USED_GET(_u_, _vrf_) \
        SHR_BITGET(MPLS_INFO(_u_)->vrf_bitmap, (_vrf_))
#define _BCM_MPLS_VRF_USED_SET(_u_, _vrf_) \
        SHR_BITSET(MPLS_INFO((_u_))->vrf_bitmap, (_vrf_))
#define _BCM_MPLS_VRF_USED_CLR(_u_, _vrf_) \
        SHR_BITCLR(MPLS_INFO((_u_))->vrf_bitmap, (_vrf_))

/*
 * VPWS usage bitmap operations (num VPs / 2)
 */
#define _BCM_MPLS_VPWS_USED_GET(_u_, _i_) \
        SHR_BITGET(MPLS_INFO(_u_)->vpws_bitmap, (_i_))

#define _BCM_MPLS_VPWS_USED_SET(_u_, _i_) \
        SHR_BITSET(MPLS_INFO((_u_))->vpws_bitmap, (_i_))
#define _BCM_MPLS_VPWS_USED_CLR(_u_, _i_) \
        SHR_BITCLR(MPLS_INFO((_u_))->vpws_bitmap, (_i_))

/*
 * VC_AND_LABEL (1st 4K - pw_init counters) usage bitmap operations
 */
#define _BCM_MPLS_VC_COUNT_USED_GET(_u_, _vc_) \
        SHR_BITGET(MPLS_INFO(_u_)->vc_c_bitmap, (_vc_))
#define _BCM_MPLS_VC_COUNT_USED_SET(_u_, _vc_) \
        SHR_BITSET(MPLS_INFO((_u_))->vc_c_bitmap, (_vc_))
#define _BCM_MPLS_VC_COUNT_USED_CLR(_u_, _vc_) \
        SHR_BITCLR(MPLS_INFO((_u_))->vc_c_bitmap, (_vc_))

/*
 * VC_AND_LABEL (2nd 4K - no counters) usage bitmap operations
 */
#define _BCM_MPLS_VC_NON_COUNT_USED_GET(_u_, _vc_) \
        SHR_BITGET(MPLS_INFO(_u_)->vc_nc_bitmap, (_vc_))
#define _BCM_MPLS_VC_NON_COUNT_USED_SET(_u_, _vc_) \
        SHR_BITSET(MPLS_INFO((_u_))->vc_nc_bitmap, (_vc_))
#define _BCM_MPLS_VC_NON_COUNT_USED_CLR(_u_, _vc_) \
        SHR_BITCLR(MPLS_INFO((_u_))->vc_nc_bitmap, (_vc_))

/*
 * "Pseudo-wire termination counters" usage bitmap operations
 */
#define _BCM_MPLS_PW_TERM_USED_GET(_u_, _pt_) \
        SHR_BITGET(MPLS_INFO(_u_)->pw_term_bitmap, (_pt_))
#define _BCM_MPLS_PW_TERM_USED_SET(_u_, _pt_) \
        SHR_BITSET(MPLS_INFO((_u_))->pw_term_bitmap, (_pt_))
#define _BCM_MPLS_PW_TERM_USED_CLR(_u_, _pt_) \
        SHR_BITCLR(MPLS_INFO((_u_))->pw_term_bitmap, (_pt_))

/*
 * "Pseudo-wire Initiation counters" usage bitmap operations
 */
#define _BCM_MPLS_PW_INIT_USED_GET(_u_, _pt_) \
        SHR_BITGET(MPLS_INFO(_u_)->pw_init_bitmap, (_pt_))
#define _BCM_MPLS_PW_INIT_USED_SET(_u_, _pt_) \
        SHR_BITSET(MPLS_INFO((_u_))->pw_init_bitmap, (_pt_))
#define _BCM_MPLS_PW_INIT_USED_CLR(_u_, _pt_) \
        SHR_BITCLR(MPLS_INFO((_u_))->pw_init_bitmap, (_pt_))

/*
 * EGR_IP_TUNNEL_MPLS table usage bitmap operations
 * (there are 4 MPLS entries for each EGR_IP_TUNNEL_MPLS entry)
 */
#define _BCM_MPLS_TNL_USED_GET(_u_, _tnl_) \
        SHR_BITGET(MPLS_INFO(_u_)->tnl_bitmap, (_tnl_))
#define _BCM_MPLS_TNL_USED_SET(_u_, _tnl_) \
        SHR_BITSET(MPLS_INFO((_u_))->tnl_bitmap, (_tnl_))
#define _BCM_MPLS_TNL_USED_CLR(_u_, _tnl_) \
        SHR_BITCLR(MPLS_INFO((_u_))->tnl_bitmap, (_tnl_))
#define _BCM_MPLS_TNL_USED_RANGE_GET(_u_, _tnl_, _bit_count_, _result_) \
        SHR_BITTEST_RANGE(MPLS_INFO(_u_)->tnl_bitmap, _tnl_, _bit_count_, _result_)

/*
 * EGR_IP_TUNNEL_IP table usage bitmap operations
 */
#define _BCM_MPLS_IP_TNL_USED_GET(_u_, _tnl_) \
        SHR_BITGET(MPLS_INFO(_u_)->ip_tnl_bitmap, (_tnl_))
#define _BCM_MPLS_IP_TNL_USED_SET(_u_, _tnl_) \
        SHR_BITSET(MPLS_INFO((_u_))->ip_tnl_bitmap, (_tnl_))
#define _BCM_MPLS_IP_TNL_USED_CLR(_u_, _tnl_) \
        SHR_BITCLR(MPLS_INFO((_u_))->ip_tnl_bitmap, (_tnl_))
/*
 * Egress PRI->EXP/DOT1P map usage bitmap operations
 */
#define _BCM_EGR_MPLS_MAP_USED_GET(_u_, _map_) \
        SHR_BITGET(MPLS_INFO(_u_)->egr_mpls_bitmap, (_map_))
#define _BCM_EGR_MPLS_MAP_USED_SET(_u_, _map_) \
        SHR_BITSET(MPLS_INFO((_u_))->egr_mpls_bitmap, (_map_))
#define _BCM_EGR_MPLS_MAP_USED_CLR(_u_, _map_) \
        SHR_BITCLR(MPLS_INFO((_u_))->egr_mpls_bitmap, (_map_))

/*
 * Ingress EXP->DOT1P map usage bitmap operations
 */
#define _BCM_MPLS_ING_EXP_MAP_USED_GET(_u_, _map_) \
        SHR_BITGET(MPLS_INFO(_u_)->ing_exp_map_bitmap, (_map_))
#define _BCM_MPLS_ING_EXP_MAP_USED_SET(_u_, _map_) \
        SHR_BITSET(MPLS_INFO((_u_))->ing_exp_map_bitmap, (_map_))
#define _BCM_MPLS_ING_EXP_MAP_USED_CLR(_u_, _map_) \
        SHR_BITCLR(MPLS_INFO((_u_))->ing_exp_map_bitmap, (_map_))
/*
 * Egress EXP->DOT1P map usage bitmap operations
 */
#define _BCM_MPLS_EGR_EXP_MAP_USED_GET(_u_, _map_) \
              SHR_BITGET(MPLS_INFO(_u_)->egr_l2_exp_map_bitmap, (_map_))
#define _BCM_MPLS_EGR_EXP_MAP_USED_SET(_u_, _map_) \
              SHR_BITSET(MPLS_INFO((_u_))->egr_l2_exp_map_bitmap, (_map_))
#define _BCM_MPLS_EGR_EXP_MAP_USED_CLR(_u_, _map_) \
              SHR_BITCLR(MPLS_INFO((_u_))->egr_l2_exp_map_bitmap, (_map_))
#ifdef BCM_APACHE_SUPPORT
/*
 *  * Ingress EXP->PBH and CNG map usage bitmap operations
 */
#define _BCM_MPLS_ING_EXP_TO_PHB_CNG_MAP_USED_GET(_u_, _map_) \
        SHR_BITGET(MPLS_INFO(_u_)->ing_exp_to_phb_cng_map_bitmap, (_map_))
#define _BCM_MPLS_ING_EXP_TO_PHB_CNG_MAP_USED_SET(_u_, _map_) \
        SHR_BITSET(MPLS_INFO((_u_))->ing_exp_to_phb_cng_map_bitmap, (_map_))
#define _BCM_MPLS_ING_EXP_TO_PHB_CNG_MAP_USED_CLR(_u_, _map_) \
        SHR_BITCLR(MPLS_INFO((_u_))->ing_exp_to_phb_cng_map_bitmap, (_map_))
/*
 *  * Egress EXP->PBH and CNG map usage bitmap operations
 */
#define _BCM_MPLS_EGR_EXP_TO_PHB_CNG_MAP_USED_GET(_u_, _map_) \
              SHR_BITGET(MPLS_INFO(_u_)->egr_exp_to_phb_cng_map_bitmap, (_map_))
#define _BCM_MPLS_EGR_EXP_TO_PHB_CNG_MAP_USED_SET(_u_, _map_) \
            SHR_BITSET(MPLS_INFO((_u_))->egr_exp_to_phb_cng_map_bitmap, (_map_))
#define _BCM_MPLS_EGR_EXP_TO_PHB_CNG_MAP_USED_CLR(_u_, _map_) \
            SHR_BITCLR(MPLS_INFO((_u_))->egr_exp_to_phb_cng_map_bitmap, (_map_))
#endif
/*
 * SVP profile set usage bitmap operations
 */
#define _BCM_MPLS_SVP_PROFILE_SET_USED_GET(_u_, _vp_) \
        SHR_BITGET(MPLS_INFO(_u_)->svp_profile_set_bitmap, (_vp_))
#define _BCM_MPLS_SVP_PROFILE_SET_USED_SET(_u_, _vp_) \
        SHR_BITSET(MPLS_INFO((_u_))->svp_profile_set_bitmap, (_vp_))
#define _BCM_MPLS_SVP_PROFILE_SET_USED_CLR(_u_, _vp_) \
        SHR_BITCLR(MPLS_INFO((_u_))->svp_profile_set_bitmap, (_vp_))

/*
 * Network-port VP bitmap operations
 */
#define _BCM_MPLS_NETWORK_VP_USED_GET(_u_, _vp_) \
        SHR_BITGET(MPLS_INFO(_u_)->network_vp_bitmap, (_vp_))
#define _BCM_MPLS_NETWORK_VP_USED_SET(_u_, _vp_) \
        SHR_BITSET(MPLS_INFO((_u_))->network_vp_bitmap, (_vp_))
#define _BCM_MPLS_NETWORK_VP_USED_CLR(_u_, _vp_) \
        SHR_BITCLR(MPLS_INFO((_u_))->network_vp_bitmap, (_vp_))

#define _BCM_MPLS_CLEANUP(_rv_) \
       if ( (_rv_) < 0) { \
           goto cleanup; \
       }
extern int _egr_qos_id2hw_idx (int unit,int qos_id,int *hw_inx);
extern int _egr_qos_hw_idx2id (int unit, int hw_idx, int *map_id);

#define _BCM_VC_SWAP_TBL_FLEX_DATA_TYPE 5
#endif	/* !_BCM_INT_MPLS_H */
