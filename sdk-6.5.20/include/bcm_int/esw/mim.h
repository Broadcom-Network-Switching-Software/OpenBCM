/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains mim definitions internal to the BCM library.
 */

#ifndef _BCM_INT_MIM_H
#define _BCM_INT_MIM_H

#include <bcm/mim.h>
#include <bcm_int/esw/vpn.h>

#define _BCM_MIM_VPN_TYPE_MIM         _BCM_VPN_TYPE_VFI

#define _BCM_IS_MIM_VPN(_vpn_)        _BCM_VPN_IS_MIM(_vpn_)

#define _BCM_MIM_VPN_IS_SET(_vpn_)    _BCM_VPN_IS_MIM(_vpn_)

#define _BCM_MIM_VPN_SET(_vpn_, _type_, _id_) \
        _BCM_VPN_SET(_vpn_, _type_, _id_)

#define _BCM_MIM_VPN_GET(_id_, _type_,  _vpn_) \
        _BCM_VPN_GET(_id_, _type_, _vpn_)

#define _BCM_MIM_VPN_INVALID 0xffff

#define _BCM_MIM_PORT_TYPE_NETWORK                    (1 << 0)
#define _BCM_MIM_PORT_TYPE_ACCESS_PORT                (1 << 1)
#define _BCM_MIM_PORT_TYPE_ACCESS_PORT_VLAN           (1 << 2)
#define _BCM_MIM_PORT_TYPE_ACCESS_PORT_VLAN_STACKED   (1 << 3)
#define _BCM_MIM_PORT_TYPE_ACCESS_LABEL               (1 << 4)
#define _BCM_MIM_PORT_TYPE_PEER                       (1 << 5)
#define _BCM_MIM_PORT_TYPE_ACCESS_PORT_TRUNK          (1 << 6)
#define _BCM_MIM_PORT_TYPE_ACCESS_SHARED              (1 << 7)
#define _BCM_MIM_PORT_TYPE_SHARE                      (1 << 8)

/* on TR2 B0, default port for mim is 1 */
#define _BCM_MIM_DEFAULT_PORT                         (1)

/*
 * MIM VPN state - need to remember the ISID
 */
typedef struct _bcm_tr2_vpn_info_s {
    int isid;                /* The ISID value */
} _bcm_tr2_vpn_info_t;

/*
 * MIM port state - need to remember where to find the port.
 * Flags indicate the type of the port and hence the tables where the data is.
 * The index is used for the SOURCE_TRUNK_MAP table or for the
 * MPLS_STATION_TCAM.
 * The match attributes are the keys for the various hash tables.
 */
typedef struct _bcm_tr2_mim_port_info_s {
    uint32 flags;
    uint32 index; /* Index of memory where port info is programmed */
    bcm_trunk_t tgid; /* Trunk group ID */
    bcm_module_t modid; /* Module id */
    bcm_port_t port; /* Port */
    bcm_vlan_t match_vlan; /* S-Tag */
    bcm_vlan_t match_inner_vlan; /* C-Tag */
    bcm_mpls_label_t match_label; /* MPLS label */
    bcm_mac_t match_tunnel_srcmac; /* NVP src BMAC */
    bcm_vlan_t match_tunnel_vlan; /* NVP BVID */
    int match_count; /* Number of matches from VLAN_XLATE configured */
    int vfi_count; /* Number of VFI sharing the same peer vp */
} _bcm_tr2_mim_port_info_t;

/*
 * Software book keeping for MIM related information
 */
typedef struct _bcm_tr2_mim_bookkeeping_s {
    _bcm_tr2_vpn_info_t *vpn_info;         /* VPN state */
    _bcm_tr2_mim_port_info_t *port_info;   /* VP state */
    SHR_BITDCL  *intf_bitmap;             /* L3 interfaces used */
} _bcm_tr2_mim_bookkeeping_t;

extern _bcm_tr2_mim_bookkeeping_t  _bcm_tr2_mim_bk_info[BCM_MAX_NUM_UNITS];

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_mim_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */


#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
extern int _bcm_tr2_mim_egr_vxlt_sd_tag_actions(int unit,
                                                bcm_mim_port_t *mim_port,
                                                bcm_mim_vpn_config_t *vpn_info,
                                                uint32 *evxlt_entry);
extern int _bcm_tr2_mim_match_trunk_add(int unit, bcm_trunk_t tgid, int vp);
extern int _bcm_tr2_mim_egr_vxlt_sd_tag_actions_delete(
               int unit, uint32 *egr_vlan_xlate_entry);

extern int _bcm_tr3_mim_peer_port_config_add(int unit, 
                        bcm_mim_port_t *mim_port, int vp, bcm_mim_vpn_t vpn);
extern int _bcm_tr_mim_match_trunk_delete(int unit, bcm_trunk_t tgid, int vp);

extern int _bcm_tr3_mim_peer_port_config_delete(int unit, int vp, 
                        bcm_mim_vpn_t vpn);
extern int _bcm_tr3_mim_match_add(int unit, bcm_mim_port_t *mim_port, int vp);
extern int _bcm_tr3_mim_match_delete(int unit, int vp);

#endif /* BCM_TRIUMPH3_SUPPORT */

#if defined(INCLUDE_L3)
extern int _bcm_esw_mim_port_source_vp_lag_set(
               int unit, bcm_gport_t gport, int vp_lag_vp);
extern int _bcm_esw_mim_port_source_vp_lag_clear(
               int unit, bcm_gport_t gport, int vp_lag_vp);
extern int _bcm_esw_mim_port_source_vp_lag_get(
               int unit, bcm_gport_t gport, int *vp_lag_vp);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_esw_mim_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#endif /* INCLUDE_L3 */

#endif	/* !_BCM_INT_MIM_H */
