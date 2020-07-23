/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains VXLAN definitions internal to the BCM library.
 */


#ifndef _BCM_INT_VXLAN_H
#define _BCM_INT_VXLAN_H

#include <bcm/types.h>
#include <bcm/vxlan.h>
#include <bcm_int/esw/vpn.h>

#define _BCM_MAX_NUM_VXLAN_TUNNEL 0x3FFF
#define _BCM_VXLAN_VFI_INVALID 0

/* 
 * VXLAN TPID types
*/

#define _BCM_VXLAN_TPID_SVP_BASED 0x0
#define _BCM_VXLAN_TPID_VFI_BASED 0x1
#define _BCM_VXLAN_TPID_SGLP_BASED 0x2

/* 
 * VXLAN TUNNEL TERMINATOR MULTICAST STATES
*/

#define _BCM_VXLAN_TUNNEL_TERM_MULTICAST_LEAF 0x1
#define _BCM_VXLAN_TUNNEL_TERM_MULTICAST_BUD 0x2

/* 
 * VXLAN EGR_L3_NEXT_HOP View 
*/
#define _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW           0x0
#define _BCM_VXLAN_EGR_NEXT_HOP_SDTAG_VIEW        0x2
#define _BCM_VXLAN_EGR_NEXT_HOP_IFP_ACTION_VIEW   0x6
#define _BCM_VXLAN_EGR_NEXT_HOP_L3MC_VIEW         0x7
#define _BCM_VXLAN_EGR_NEXT_HOP_VXLAN_VIEW        0xA
#define _BCM_VXLAN_EGR_NEXT_HOP_L2OTAG_VIEW       0x15

/* 
 * VXLAN Lookup Key Types 
*/
#define _BCM_VXLAN_KEY_TYPE_TUNNEL           0x08
#define _BCM_VXLAN_KEY_TYPE_VNID_VFI        0x09
#define _BCM_VXLAN_KEY_TYPE_TUNNEL_VNID_VFI    0x0A
#define _BCM_VXLAN_KEY_TYPE_LOOKUP_DIP    0x12
#define _BCM_VXLAN_KEY_TYPE_LOOKUP_VFI   0x08
#define _BCM_VXLAN_KEY_TYPE_LOOKUP_VFI_DVP    0x09

#define _BCM_VXLAN_FLEX_KEY_TYPE_LOOKUP_DIP    0xc
#define _BCM_VXLAN_KEY_TYPE_VNID_VFI_FLEX         0x13
#define _BCM_VXLAN_KEY_TYPE_VNID_VFI_SIP_FLEX     0x1C
#define _BCM_VXLAN_KEY_TYPE_LOOKUP_DIP_IN_MPLS_ENTRY    0x0C
#define _BCM_VXLAN_DATA_TYPE_LOOKUP_DIP_IN_MPLS_ENTRY    0x0C
#define _BCM_VXLAN_DATA_TYPE_VNID_VFI_FLEX        0x1C
#define _BCM_VXLAN_DATA_TYPE_LOOKUP_VFI           0x17
#define _BCM_VXLAN_FLEX_DATA_TYPE_IPV4_SIP        0xF

/* VXLAN Multicast state */
#define _BCM_VXLAN_MULTICAST_NONE        0x0
#define _BCM_VXLAN_MULTICAST_BUD          0x1
#define _BCM_VXLAN_MULTICAST_LEAF         0x2
#define _BCM_VXLAN_MULTICAST_TRANSIT   0x4

/* VXLAN Tunnel Type */
#define _BCM_VXLAN_TUNNEL_TYPE      0x0B

/* VXLAN SVP Type */
#define _BCM_VXLAN_SOURCE_VP_TYPE_INVALID    0x0
#define _BCM_VXLAN_SOURCE_VP_TYPE_VFI            0x1
#define _BCM_VXLAN_SOURCE_VP_TYPE_VLAN         0x3


/* VXLAN ING_NEXT_HOP Type */
#define  _BCM_VXLAN_INGRESS_NEXT_HOP_ENTRY_TYPE   0x2


/* VXLAN DVP Type */
#define  _BCM_VXLAN_DEST_VP_TYPE_ACCESS       0x0
#define  _BCM_VXLAN_INGRESS_DEST_VP_TYPE  0x3
#define  _BCM_VXLAN_EGRESS_DEST_VP_TYPE   0x2

#define _BCM_VXLAN_TUNNEL_TERM_DISABLE 0x0
#define _BCM_VXLAN_TUNNEL_TERM_ENABLE 0x1
#define _BCM_VXLAN_TUNNEL_TERM_UDP_CHECKSUM_ENABLE 0x2
#define _BCM_VXLAN_TUNNEL_TERM_USE_OUTER_DSCP      0x4
#define _BCM_VXLAN_TUNNEL_TERM_USE_OUTER_PCP       0x8

/*
 * VXLAN port state - need to remember where to find the port.
 * Flags indicate the type of the port and hence the tables where the data is.
 * The match attributes are the keys for the various hash tables.
 */

#define _BCM_VXLAN_PORT_MATCH_TYPE_NONE         (1 << 0)
#define _BCM_VXLAN_PORT_MATCH_TYPE_VLAN   (1 << 1)
#define _BCM_VXLAN_PORT_MATCH_TYPE_INNER_VLAN (1 << 2)
#define _BCM_VXLAN_PORT_MATCH_TYPE_VLAN_STACKED        (1 << 3)
#define _BCM_VXLAN_PORT_MATCH_TYPE_VLAN_PRI  (1 << 4)
#define _BCM_VXLAN_PORT_MATCH_TYPE_PORT         (1 << 5)
#define _BCM_VXLAN_PORT_MATCH_TYPE_TRUNK       (1 << 6)
#define _BCM_VXLAN_PORT_MATCH_TYPE_VNID        (1 << 7)
#define _BCM_VXLAN_PORT_MATCH_TYPE_SHARED        (1 << 8)

#define _BCM_VXLAN_PORT_MATCH_NO_UNTAG_OP        (1 << 0)

#define BCM_VXLAN_DIP_FIELD(decouple_mode, field)                       \
            ((decouple_mode)?(VXLAN_FLEX_IPV4_DIP__##field):(VXLAN_DIP__##field))
#define BCM_VXLAN_VNID_FIELD(decouple_mode, field)                       \
            ((decouple_mode)?(VXLAN_FLEX__##field):(VXLAN_VN_ID__##field))
#define BCM_VXLAN_VFI_FIELD(decouple_mode, field)                       \
            ((decouple_mode)?(VXLAN_VFI_FLEX__##field):(VXLAN_VFI__##field))
#define BCM_VXLAN_TAG_FIELD(decouple_mode, field)                       \
            ((decouple_mode)?(L2_OTAG__##field):(SD_TAG__##field))
#define BCM_VXLAN_TAG_VIEW(decouple_mode)                       \
                ((decouple_mode)?(_BCM_VXLAN_EGR_NEXT_HOP_L2OTAG_VIEW):(_BCM_VXLAN_EGR_NEXT_HOP_SDTAG_VIEW))
#define BCM_VXLAN_SIP_FIELD(decouple_mode, field)                       \
                ((decouple_mode)?(VXLAN_FLEX_IPV4_SIP__##field):(VXLAN_SIP__##field))

/* VXLAN VPN Type */
#define  _BCM_VXLAN_VPN_TYPE_TUNNEL_BASED_VNID       0x1

typedef struct _bcm_td2_vxlan_nh_info_s {
    int      entry_type;
    int      dvp_is_network;
    int      sd_tag_action_present;
    int      sd_tag_action_not_present;
    int      dvp;
    int      intf_num;
    int      sd_tag_vlan;
    int      sd_tag_pri;
    int      sd_tag_cfi;
    int      vc_swap_index;
    int      tpid_index;
    int      is_eline;
    int      dvp_network_group_id;
} _bcm_td2_vxlan_nh_info_t;

typedef struct _bcm_vxlan_match_port_info_s {
    uint32     flags;
    uint32     index; /* Index of memory where port info is programmed */
    bcm_trunk_t trunk_id; /* Trunk group ID */
    bcm_module_t modid; /* Module id */
    bcm_port_t port; /* Port */
    bcm_vlan_t match_vlan;
    bcm_vlan_t match_inner_vlan;
    uint32 match_tunnel_index; /* Tunnel Index */
    int match_count; /* Number of matches from VLAN_XLATE configured */
    int vfi; /* vfi related with vp */
    uint32     flags2;
} _bcm_vxlan_match_port_info_t;

typedef struct _bcm_vxlan_eline_vp_map_info_s {
    int             vp1; /* Access VP */
    int             vp2; /* Network VP */
} _bcm_vxlan_eline_vp_map_info_t;

typedef struct _bcm_vxlan_tunnel_endpoint_s {
     bcm_ip_t      dip; /* Tunnel DIP */
     bcm_ip_t      sip; /* Tunnel SIP */
     uint16         tunnel_state;  /* Multicast Reference Count */
     int           activate_flag; /* Tunnel terminator is deactivated or activated */
     bcm_vlan_t    vlan; /* Out vlan for tunnel termination */
} COMPILER_ATTRIBUTE((packed)) _bcm_vxlan_tunnel_endpoint_t;

typedef struct _bcm_vxlan_vpn_info_s {    
     bcm_ip_t      sip; /* Tunnel SIP */
     uint32        vnid; /* vxlan vnid */ 
     uint8         vxlan_vpn_type; /* indicate vpn type */     
} _bcm_vxlan_vpn_info_t;

/*
 * Software book keeping for VXLAN  related information
 */
typedef struct _bcm_td2_vxlan_bookkeeping_s {
    int         initialized;        /* Set to TRUE when VXLAN module initialized */
    sal_mutex_t    vxlan_mutex;    /* Protection mutex. */
    SHR_BITDCL *vxlan_ip_tnl_bitmap; /* EGR_IP_TUNNEL index bitmap */
    _bcm_vxlan_match_port_info_t *match_key; /* Match Key */
    _bcm_vxlan_tunnel_endpoint_t    *vxlan_tunnel_term;
    _bcm_vxlan_tunnel_endpoint_t    *vxlan_tunnel_init;
    bcm_vlan_t                      *vxlan_vpn_vlan; /* Outer VLAN for VPN */
    _bcm_vxlan_vpn_info_t           *vxlan_vpn_info;
    uint32                          *vfi_vnid_map_count; /* VP-based VFI-VNID mapping count */
} _bcm_td2_vxlan_bookkeeping_t;

typedef struct _bcm_td3_vxlan_vxlate_entry_s {
    int      dtype;
    int      ktype;
    int      vfi;
    bcm_vlan_t      vlan;
    uint32      vnid;
} _bcm_td3_vxlan_vxlate_entry_t;

extern _bcm_td2_vxlan_bookkeeping_t *_bcm_td2_vxlan_bk_info[BCM_MAX_NUM_UNITS];

/* Generic memory allocation routine. */
#define BCMI_VXLAN_ALLOC(_ptr_,_size_,_descr_)                       \
            do {                                                     \
                if ((NULL == (_ptr_))) {                             \
                   _ptr_ = sal_alloc((_size_),(_descr_));            \
                }                                                    \
                if((_ptr_) != NULL) {                                \
                    sal_memset((_ptr_), 0, (_size_));                \
                }                                                    \
            } while (0)

#define _BCM_VXLAN_VPN_INVALID 0xffff
#define BCM_STAT_VXLAN_FLEX_COUNTER_MAX_TABLES 4

/*
 * Do not change - Implementation assumes
 * _BCM_VXLAN_VPN_TYPE_ELINE == _BCM_VXLAN_VPN_TYPE_ELAN
 */
#define _BCM_VXLAN_VPN_TYPE_ELINE         _BCM_VPN_TYPE_VFI
#define _BCM_VXLAN_VPN_TYPE_ELAN          _BCM_VPN_TYPE_VFI

#define _BCM_VXLAN_VPN_SET(_vpn_, _type_, _id_) \
        _BCM_VPN_SET(_vpn_, _type_, _id_)

#define _BCM_VXLAN_VPN_GET(_id_, _type_,  _vpn_) \
        _BCM_VPN_GET(_id_, _type_, _vpn_)

#define _BCM_VXLAN_TUNNEL_ID_IF_INVALID_RETURN(unit, id) \
    do {                                                 \
        if (((id) < 0) || (id) >= (soc_mem_index_count((unit), SOURCE_VPm))) { \
            return (BCM_E_BADID);                        \
        }                                                \
    } while(0)

extern int _bcm_esw_vxlan_trunk_member_add(int unit, bcm_trunk_t trunk_id, 
        int trunk_member_count, bcm_port_t *trunk_member_array);
extern int _bcm_esw_vxlan_trunk_member_delete(int unit, bcm_trunk_t trunk_id, 
        int trunk_member_count, bcm_port_t *trunk_member_array); 
extern int _bcm_esw_vxlan_port_source_vp_lag_set(
                                    int unit,
                                    bcm_gport_t gport,
                                    int vp_lag_vp);
extern int _bcm_esw_vxlan_port_source_vp_lag_clear(
                                    int unit,
                                    bcm_gport_t gport,
                                    int vp_lag_vp);
extern int _bcm_esw_vxlan_port_source_vp_lag_get(
                                    int unit,
                                    bcm_gport_t gport,
                                    int *vp_lag_vp);


#if defined(INCLUDE_L3)

extern int _bcm_td2_vxlan_nexthop_entry_modify(
                                    int unit,
                                    int nh_index,
                                    int drop,
                                    _bcm_td2_vxlan_nh_info_t  *egr_nh_info,
                                    int new_entry_type);

extern int _bcm_td2_vxlan_ecmp_nexthop_entry_modify(
                                    int unit,
                                    int ecmp_index,
                                    int drop,
                                    _bcm_td2_vxlan_nh_info_t egr_nh_info,
                                    int new_entry_type);

extern int _bcm_td2_vxlan_ecmp_port_egress_nexthop_reset(
                                    int unit,
                                    int ecmp_index,
                                    int vp_type,
                                    int vp,
                                    bcm_vpn_t vpn);

extern int _bcm_td2_vxlan_port_egress_nexthop_reset(
                                    int unit,
                                    int nh_index,
                                    int vp_type,
                                    int vp,
                                    bcm_vpn_t vpn);

extern bcm_error_t _bcm_td2_vxlan_dip_stat_counter_get(
                                    int unit,
                                    int sync_mode,
                                    bcm_ip_t vxlan_dip,
                                    bcm_vxlan_dip_stat_t stat,
                                    uint32 num_entries,
                                    uint32 *counter_indexes,
                                    bcm_stat_value_t *counter_values);

extern bcm_error_t  _bcm_td2_vxlan_dip_stat_multi_get(
                                   int                   unit, 
                                   bcm_ip_t              vxlan_dip,
                                   int                   nstat, 
                                   bcm_vxlan_dip_stat_t  *stat_arr,
                                   uint64                *value_arr);



extern bcm_error_t  _bcm_td2_vxlan_dip_stat_multi_get32(
                                    int                  unit, 
                                    bcm_ip_t             vxlan_dip,
                                    int                  nstat, 
                                    bcm_vxlan_dip_stat_t *stat_arr,
                                    uint32               *value_arr);


extern bcm_error_t _bcm_td2_vxlan_dip_stat_counter_set(
                                    int unit,
                                    bcm_ip_t vxlan_dip,
                                    bcm_vxlan_dip_stat_t stat,
                                    uint32 num_entries,
                                    uint32 *counter_indexes,
                                    bcm_stat_value_t *counter_values);

extern bcm_error_t _bcm_esw_vxlan_dip_stat_counter_get(
                                   int                unit,
                                   int                sync_mode,
                                   bcm_ip_t           vxlan_dip,
                                   bcm_vxlan_dip_stat_t  stat,
                                   uint32             num_entries,
                                   uint32             *counter_indexes,
                                   bcm_stat_value_t   *counter_values);


extern bcm_error_t  _bcm_td2_vxlan_dip_stat_multi_set(
                                    int                   unit, 
                                    bcm_ip_t              vxlan_dip,
                                    int                   nstat, 
                                    bcm_vxlan_dip_stat_t  *stat_arr,
                                    uint64                *value_arr);


extern bcm_error_t _bcm_td2_vxlan_dip_stat_multi_set32(
                                   int                   unit, 
                                   bcm_ip_t              vxlan_dip,
                                   int                   nstat, 
                                   bcm_vxlan_dip_stat_t  *stat_arr,
                                   uint32                *value_arr);


extern bcm_error_t _bcm_td2_vxlan_dip_stat_id_get(
                                    int unit,
                                    bcm_ip_t vxlan_dip,
                                    bcm_vxlan_dip_stat_t stat,
                                    uint32 *stat_counter_id);



extern bcm_error_t _bcm_td2_vxlan_dip_stat_detach(
                                    int unit,
                                    bcm_ip_t vxlan_dip);


extern bcm_error_t _bcm_td2_vxlan_dip_stat_attach(
                                    int unit,
                                    bcm_ip_t vxlan_dip,
                                    uint32 stat_counter_id);
#endif
#if defined(INCLUDE_L3)
extern int _bcm_td2_vxlan_higig_ppd_select(int unit, int nh_index, uint32 flags);
extern int _bcm_td2_vxlan_ecmp_higig_ppd_select(int unit, int ecmp_index, uint32 flags);
#endif

#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void _bcm_vxlan_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_esw_vxlan_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#endif	/* !_BCM_INT_VXLAN_H */


