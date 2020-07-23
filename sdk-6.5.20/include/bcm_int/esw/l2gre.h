/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains L2GRE definitions internal to the BCM library.
 */


#ifndef _BCM_INT_L2GRE_H
#define _BCM_INT_L2GRE_H

#include <bcm/types.h>
#include <bcm/l2gre.h>
#include <bcm_int/esw/vpn.h>

#if defined(BCM_TRIUMPH3_SUPPORT)

#define _BCM_MAX_NUM_L2GRE_TUNNEL 2048
#define _BCM_L2GRE_VFI_INVALID 0x3FFF

/* 
 * L2GRE TPID types
*/

#define _BCM_L2GRE_TPID_SVP_BASED 0x0
#define _BCM_L2GRE_TPID_VFI_BASED 0x1
#define _BCM_L2GRE_TPID_SGLP_BASED 0x2

/* 
 * L2GRE TUNNEL TERMINATOR MULTICAST STATES
*/

#define _BCM_L2GRE_TUNNEL_TERM_MULTICAST_LEAF 0x1
#define _BCM_L2GRE_TUNNEL_TERM_MULTICAST_BUD 0x2

/* 
 * L2GRE EGR_L3_NEXT_HOP View 
*/
#define _BCM_L2GRE_EGR_NEXT_HOP_L3_VIEW         0x0
#define _BCM_L2GRE_EGR_NEXT_HOP_SDTAG_VIEW  0x2
#define _BCM_L2GRE_EGR_NEXT_HOP_L3MC_VIEW    0x7

/* 
 * L2GRE Lookup Key Types 
 */
#define _BCM_TD2_L2GRE_KEY_TYPE_TUNNEL_VPNID_VFI 0x04
#define _BCM_L2GRE_KEY_TYPE_TUNNEL           0x06
#define _BCM_L2GRE_KEY_TYPE_VPNID_VFI      0x07
#define _BCM_TR3_L2GRE_KEY_TYPE_TUNNEL_VPNID_VFI    0x08
#define _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_DIP    0x0D
#define _BCM_TR3_L2GRE_KEY_TYPE_LOOKUP_DIP    0x1A

/* L2GRE Multicast state */
#define _BCM_L2GRE_MULTICAST_NONE        0x0
#define _BCM_L2GRE_MULTICAST_BUD          0x1
#define _BCM_L2GRE_MULTICAST_LEAF         0x2
#define _BCM_L2GRE_MULTICAST_TRANSIT   0x4

/* L2GRE Tunnel Type */
#define _BCM_L2GRE_TUNNEL_TYPE      0x7

/* L2GRE EGR_VFI Type */
#define _BCM_TR3_L2GRE_KEY_TYPE_LOOKUP_VFI    0x04
#define _BCM_TR3_L2GRE_KEY_TYPE_LOOKUP_VFI_DVP    0x05
#define _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_VFI   0x05
#define _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_VFI_DVP    0x06

/* L2GRE SVP Type */
#define _BCM_L2GRE_SOURCE_VP_TYPE_INVALID    0x0
#define _BCM_L2GRE_SOURCE_VP_TYPE_VFI            0x1
#define _BCM_L2GRE_SOURCE_VP_TYPE_VLAN         0x3


/* L2GRE ING_NEXT_HOP Type */
#define  _BCM_L2GRE_INGRESS_NEXT_HOP_ENTRY_TYPE   0x2

/* L2GRE DVP Type */
#define  _BCM_L2GRE_DEST_VP_TYPE_ACCESS       0x0
#define  _BCM_L2GRE_DEST_VP_ATTRIBUTE_TYPE  0x3
#define  _BCM_L2GRE_DEST_VP_TYPE_NETWORK   0x2

#define  _BCM_L2GRE_INGRESS_DEST_VP_TYPE  0x2
#define  _BCM_L2GRE_EGRESS_DEST_VP_TYPE   0x3


/* 
 * L2GRE EGR_L3_NEXT_HOP View 
*/
#define _BCM_L2GRE_EGR_NEXT_HOP_L3_VIEW         0x0
#define _BCM_L2GRE_EGR_NEXT_HOP_SDTAG_VIEW  0x2
#define _BCM_TD2_L2GRE_EGR_NEXT_HOP_L3MC_VIEW    0x7

/*
 * L2GRE port state - need to remember where to find the port.
 * Flags indicate the type of the port and hence the tables where the data is.
 * The match attributes are the keys for the various hash tables.
 */

#define _BCM_L2GRE_PORT_MATCH_TYPE_NONE         (1 << 0)
#define _BCM_L2GRE_PORT_MATCH_TYPE_VLAN   (1 << 1)
#define _BCM_L2GRE_PORT_MATCH_TYPE_INNER_VLAN (1 << 2)
#define _BCM_L2GRE_PORT_MATCH_TYPE_VLAN_STACKED        (1 << 3)
#define _BCM_L2GRE_PORT_MATCH_TYPE_VLAN_PRI  (1 << 4)
#define _BCM_L2GRE_PORT_MATCH_TYPE_PORT         (1 << 5)
#define _BCM_L2GRE_PORT_MATCH_TYPE_TRUNK       (1 << 6)
#define _BCM_L2GRE_PORT_MATCH_TYPE_VPNID        (1 << 7)
#define _BCM_L2GRE_PORT_MATCH_TYPE_SHARED        (1 << 8)


/* L2GRE Multicast state */
#define _BCM_L2GRE_MULTICAST_NONE       0x0
#define _BCM_L2GRE_MULTICAST_BUD         0x1
#define _BCM_L2GRE_MULTICAST_LEAF        0x2
#define _BCM_L2GRE_MULTICAST_TRANSIT   0x4

typedef struct _bcm_tr3_l2gre_nh_info_s {
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
} _bcm_tr3_l2gre_nh_info_t;

typedef struct _bcm_l2gre_match_port_info_s {
    uint32     flags;
    uint32     index; /* Index of memory where port info is programmed */
    bcm_trunk_t trunk_id; /* Trunk group ID */
    bcm_module_t modid; /* Module id */
    bcm_port_t port; /* Port */
    bcm_vlan_t match_vlan;
    bcm_vlan_t match_inner_vlan;
    uint32 match_vpnid; /* VPNID */
    uint32 match_tunnel_index; /* Tunnel Index */
    int match_count; /* Number of matches from VLAN_XLATE configured */
} _bcm_l2gre_match_port_info_t;

typedef struct _bcm_l2gre_eline_vp_map_info_s {
    int             vp1; /* Access VP */
    int             vp2; /* Network VP */
} _bcm_l2gre_eline_vp_map_info_t;

typedef struct _bcm_l2gre_tunnel_endpoint_s {
     bcm_ip_t      dip; /* Tunnel DIP */
     bcm_ip_t      sip; /* Tunnel SIP */
     uint16         tunnel_state;  /* Multicast Reference Count */
} COMPILER_ATTRIBUTE((packed)) _bcm_l2gre_tunnel_endpoint_t;

/*
 * Software book keeping for L2GRE  related information
 */
typedef struct _bcm_tr3_l2gre_bookkeeping_s {
    int         initialized;        /* Set to TRUE when L2GRE module initialized */
    sal_mutex_t    l2gre_mutex;    /* Protection mutex. */
    SHR_BITDCL *l2gre_ip_tnl_bitmap; /* EGR_IP_TUNNEL index bitmap */
    _bcm_l2gre_match_port_info_t *match_key; /* Match Key */
    _bcm_l2gre_tunnel_endpoint_t    *l2gre_tunnel_term;
    _bcm_l2gre_tunnel_endpoint_t    *l2gre_tunnel_init;
} _bcm_tr3_l2gre_bookkeeping_t;

extern _bcm_tr3_l2gre_bookkeeping_t *_bcm_tr3_l2gre_bk_info[BCM_MAX_NUM_UNITS];
extern void _bcm_l2gre_sw_dump(int unit);

/* Generic memory allocation routine. */
#define BCM_TR3_L2GRE_ALLOC(_ptr_,_size_,_descr_)                      \
            do {                                                     \
                if ((NULL == (_ptr_))) {                             \
                   _ptr_ = sal_alloc((_size_),(_descr_));            \
                }                                                    \
                if((_ptr_) != NULL) {                                \
                    sal_memset((_ptr_), 0, (_size_));                \
                }                                                    \
            } while (0)

#define _BCM_L2GRE_VPN_INVALID 0xffff

#define BCM_STAT_L2GRE_FLEX_COUNTER_MAX_TABLES 4
/*
 * Do not change - Implementation assumes
 * _BCM_L2GRE_VPN_TYPE_ELINE == _BCM_L2GRE_VPN_TYPE_ELAN
 */
#define _BCM_L2GRE_VPN_TYPE_ELINE         _BCM_VPN_TYPE_VFI
#define _BCM_L2GRE_VPN_TYPE_ELAN          _BCM_VPN_TYPE_VFI

#define _BCM_L2GRE_VPN_SET(_vpn_, _type_, _id_) \
        _BCM_VPN_SET(_vpn_, _type_, _id_)

#define _BCM_L2GRE_VPN_GET(_id_, _type_, _vpn_) \
        _BCM_VPN_GET(_id_, _type_, _vpn_)

extern int _bcm_esw_l2gre_trunk_member_add(int unit, bcm_trunk_t trunk_id, 
        int trunk_member_count, bcm_port_t *trunk_member_array);
extern int _bcm_esw_l2gre_trunk_member_delete(int unit, bcm_trunk_t trunk_id, 
        int trunk_member_count, bcm_port_t *trunk_member_array); 

#endif /* BCM_TRIUMPH3_SUPPORT */

#if defined(BCM_WARM_BOOT_SUPPORT) && defined(INCLUDE_L3)
extern int _bcm_esw_l2gre_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT && INCLUDE_L3 */

#if defined(INCLUDE_L3)
extern int _bcm_esw_l2gre_port_source_vp_lag_set(
               int unit, bcm_gport_t gport, int vp_lag_vp);
extern int _bcm_esw_l2gre_port_source_vp_lag_clear(
               int unit, bcm_gport_t gport, int vp_lag_vp);
extern int _bcm_esw_l2gre_port_source_vp_lag_get(
               int unit, bcm_gport_t gport, int *vp_lag_vp);
#endif /* INCLUDE_L3 */
#endif	/* !_BCM_INT_L2GRE_H */

