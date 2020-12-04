/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    wlan.c
 * Purpose: Manages WLAN functions
 */

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)

#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/hash.h>
#include <soc/util.h>
#include <soc/triumph.h>

#include <bcm/error.h>
#include <bcm/wlan.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/stack.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/api_xlate_port.h>

/*
 * WLAN port state - need to remember where to find the port.
 * Flags indicate the type of the port and hence the tables where the data is.
 * The match attributes are the keys for the EGR_VLAN_XLATE table.
 */
#define _BCM_WLAN_PORT_MATCH_BSSID                    (1 << 0)
#define _BCM_WLAN_PORT_MATCH_BSSID_RADIO              (1 << 1)
#define _BCM_WLAN_PORT_MATCH_TUNNEL                   (1 << 2)
typedef struct _bcm_tr2_wlan_port_info_s {
    uint32 flags;
    bcm_trunk_t tgid; /* Trunk group ID */
    bcm_module_t modid; /* Module id */
    bcm_port_t port; /* Port */
    bcm_mac_t match_bssid;         /* Match BSSID */
    int match_radio;               /* Match radio */      
    bcm_gport_t match_tunnel;      /* Match tunnel */
    bcm_gport_t egress_tunnel;     /* Egress tunnel */
} _bcm_tr2_wlan_port_info_t;

/*
 * Software book keeping for WLAN related information
 */
typedef struct _bcm_tr2_wlan_bookkeeping_s {
    _bcm_tr2_wlan_port_info_t *port_info;   /* VP state */
    int capwap_frag_profile_created; /* Flag to check profile creation */
    soc_profile_reg_t *capwap_frag_profile; /* CAPWAP fragment profile */
    bcm_vlan_t *tunnel_vlan; /* Tunnel VLAN cache */
    SHR_BITDCL *intf_bitmap; /* L3 interfaces used */
    SHR_BITDCL *ip_tnl_bitmap; /* EGR_IP_TUNNEL index bitmap */
    SHR_BITDCL *ip_tnl_is_v6_bitmap; /* Set if above used entry is actually 
                                     double-wide (IPv6) */
    uint8 **vlan_grp_bmp_use_cnt; /* Vlan group bitmap use count per VLAN */
} _bcm_tr2_wlan_bookkeeping_t;

STATIC _bcm_tr2_wlan_bookkeeping_t  _bcm_tr2_wlan_bk_info[BCM_MAX_NUM_UNITS];

/* Flag to check initialized status */
STATIC int wlan_initialized[BCM_MAX_NUM_UNITS];

STATIC sal_mutex_t _wlan_mutex[BCM_MAX_NUM_UNITS] = {NULL};

#define L3_INFO(unit)    (&_bcm_l3_bk_info[unit])
#define WLAN_INFO(_unit_) (&_bcm_tr2_wlan_bk_info[_unit_])
#define CAPWAP_FRAG_PROFILE(_unit_) \
    (_bcm_tr2_wlan_bk_info[_unit_].capwap_frag_profile)
#define TUNNEL_VLAN(_unit_, _tnl_idx_) \
        (_bcm_tr2_wlan_bk_info[_unit_].tunnel_vlan[_tnl_idx_])

#define WLAN_INIT(unit)                                   \
    do {                                                  \
        if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {  \
            return BCM_E_UNIT;                            \
        }                                                 \
        if (!wlan_initialized[unit]) {                    \
            return BCM_E_INIT;                            \
        }                                                 \
    } while (0)

/* 
 * WLAN module lock
 */
#define WLAN_LOCK(unit) \
        sal_mutex_take(_wlan_mutex[unit], sal_mutex_FOREVER);

#define WLAN_UNLOCK(unit) \
        sal_mutex_give(_wlan_mutex[unit]); 

/*
 * EGR_IP_TUNNEL table usage bitmap operations
 */
#define _BCM_WLAN_IP_TNL_USED_GET(_u_, _tnl_) \
        SHR_BITGET(WLAN_INFO(_u_)->ip_tnl_bitmap, (_tnl_))
#define _BCM_WLAN_IP_TNL_USED_SET(_u_, _tnl_) \
        SHR_BITSET(WLAN_INFO((_u_))->ip_tnl_bitmap, (_tnl_))
#define _BCM_WLAN_IP_TNL_USED_CLR(_u_, _tnl_) \
        SHR_BITCLR(WLAN_INFO((_u_))->ip_tnl_bitmap, (_tnl_))

/*
 * EGR_IP_TUNNEL table index is v6 bitmap operations
 */
#define _BCM_WLAN_IP_TNL_IS_V6_GET(_u_, _tnl_) \
        SHR_BITGET(WLAN_INFO(_u_)->ip_tnl_is_v6_bitmap, (_tnl_))
#define _BCM_WLAN_IP_TNL_IS_V6_SET(_u_, _tnl_) \
        SHR_BITSET(WLAN_INFO((_u_))->ip_tnl_is_v6_bitmap, (_tnl_))
#define _BCM_WLAN_IP_TNL_IS_V6_CLR(_u_, _tnl_) \
        SHR_BITCLR(WLAN_INFO((_u_))->ip_tnl_is_v6_bitmap, (_tnl_))

/*
 * L3 interface usage bitmap operations
 */
#define _BCM_WLAN_INTF_USED_GET(_u_, _intf_) \
        SHR_BITGET(WLAN_INFO(_u_)->intf_bitmap, (_intf_))
#define _BCM_WLAN_INTF_USED_SET(_u_, _intf_) \
        SHR_BITSET(WLAN_INFO((_u_))->intf_bitmap, (_intf_))
#define _BCM_WLAN_INTF_USED_CLR(_u_, _intf_) \
        SHR_BITCLR(WLAN_INFO((_u_))->intf_bitmap, (_intf_))

/*
 * Function:
 *      _bcm_tr2_wlan_free_resources
 * Purpose:
 *      Free all allocated tables and memory
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_tr2_wlan_free_resources(int unit)
{
    _bcm_tr2_wlan_bookkeeping_t *wlan_info = WLAN_INFO(unit);
    int i;

    /* Destroy WLAN mutex */
    if (_wlan_mutex[unit]) {
        sal_mutex_destroy(_wlan_mutex[unit]);
        _wlan_mutex[unit] = NULL;
    }

    /* Destroy the VLAN group bitmap use count */
    if (wlan_info->vlan_grp_bmp_use_cnt) {
        for (i = 0; i < BCM_VLAN_COUNT; i++) {
            if (wlan_info->vlan_grp_bmp_use_cnt[i]) {
                sal_free((uint8 *)(wlan_info->vlan_grp_bmp_use_cnt[i]));
                wlan_info->vlan_grp_bmp_use_cnt[i] = NULL;
            }
        }
        sal_free(wlan_info->vlan_grp_bmp_use_cnt);
        wlan_info->vlan_grp_bmp_use_cnt = NULL;
    }

    /* Destroy EGR_IP_TUNNEL is V6 bitmap */
    if (wlan_info->ip_tnl_is_v6_bitmap) {
        sal_free(wlan_info->ip_tnl_is_v6_bitmap);
        wlan_info->ip_tnl_is_v6_bitmap = NULL;
    }

    /* Destroy EGR_IP_TUNNEL usage bitmap */
    if (wlan_info->ip_tnl_bitmap) {
        sal_free(wlan_info->ip_tnl_bitmap);
        wlan_info->ip_tnl_bitmap = NULL;
    }

    /* Destroy EGR_L3_INTF usage bitmap */
    if (wlan_info->intf_bitmap) {
        sal_free(wlan_info->intf_bitmap);
        wlan_info->intf_bitmap = NULL;
    }

    /* Destroy tunnel VLAN cache */
    if (wlan_info->tunnel_vlan) {
        sal_free(wlan_info->tunnel_vlan);
        wlan_info->tunnel_vlan = NULL;
    }

    /* Destroy CAPWAP_FRAG profile */
    if (wlan_info->capwap_frag_profile) {
        if (wlan_info->capwap_frag_profile_created) {
            (void) soc_profile_reg_destroy(unit, wlan_info->capwap_frag_profile);
        }
        sal_free(wlan_info->capwap_frag_profile);
        wlan_info->capwap_frag_profile = NULL;
    }

    /* Destroy WLAN port usage bitmap */ 
    if (wlan_info->port_info) {
        sal_free(wlan_info->port_info);
        wlan_info->port_info = NULL;
    }
}

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC void
_bcm_tr2_wlan_port_flex_stat_recover(int unit, source_vp_entry_t *source_vp, 
                                     int index)
{
    int fs_idx;
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        if (soc_mem_field_valid(unit, SOURCE_VPm, VINTF_CTR_IDXf)) {
            fs_idx = soc_mem_field32_get(unit, SOURCE_VPm, source_vp,
                                         VINTF_CTR_IDXf);
            if (fs_idx) {
                BCM_GPORT_WLAN_PORT_ID_SET(index, index);
                _bcm_esw_flex_stat_reinit_add(unit, _bcmFlexStatTypeGport,
                                              fs_idx, index);
            }
        }
    }
    return;
}

STATIC int
_bcm_tr2_wlan_lport_tpid_recover(int unit, lport_tab_entry_t *lport_tab)
{
    int tpid_enable, index, rv = BCM_E_NONE;
    tpid_enable = soc_LPORT_TABm_field32_get(unit, lport_tab,
                                             OUTER_TPID_ENABLEf);
    for (index = 0; index < 4; index++) {
        if (tpid_enable & (1 << index)) {
            rv = _bcm_fb2_outer_tpid_tab_ref_count_add(unit, index, 1);
            break;
        }
    }
    return rv;
}

STATIC int
_bcm_tr2_wlan_port_associated_data_recover(int unit, int vp, int stable_size)
{
    int idx, nh_index, intf_num, rv = BCM_E_NONE;
    uint32 nh_flags;
    ing_l3_next_hop_entry_t ing_nh_entry;
    egr_l3_next_hop_entry_t egr_nh_entry;
    ing_dvp_table_entry_t ing_dvp_entry;
    bcm_module_t mod_in, mod_out;
    bcm_port_t port_in, port_out, phys_port;
    bcm_trunk_t trunk_id;
    bcm_l3_egress_t nh_info;
    _bcm_port_info_t *info;
    bcm_port_t local_member_array[SOC_MAX_NUM_PORTS];
    int local_member_count;
    uint32 port_flags;

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_DVP_TABLEm, MEM_BLOCK_ANY, 
                                     vp, &ing_dvp_entry));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &ing_dvp_entry, 
                                              NEXT_HOP_INDEXf);
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                     nh_index, &ing_nh_entry));
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                     nh_index, &egr_nh_entry));

    bcm_l3_egress_t_init(&nh_info);

    nh_flags = _BCM_L3_SHR_UPDATE | _BCM_L3_SHR_WRITE_DISABLE | 
               _BCM_L3_SHR_WITH_ID;
    rv = bcm_xgs3_nh_add(unit, nh_flags, &nh_info, &nh_index);
    BCM_IF_ERROR_RETURN(rv);

    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry, 
                                         ENTRY_TYPEf) == 0x2) {
        /* Type is SVP */
        if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry, Tf)) {
            trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit, 
                                                        &ing_nh_entry, TGIDf);
            WLAN_INFO(unit)->port_info[vp].modid = -1;
            WLAN_INFO(unit)->port_info[vp].port = -1;
            WLAN_INFO(unit)->port_info[vp].tgid = trunk_id;
            /* Update each local physical port's SW state */
            if (stable_size == 0) {
                /* When persistent storage is available, this state is recovered
                   in the port module */
                rv = _bcm_esw_trunk_local_members_get(unit, trunk_id,
                        SOC_MAX_NUM_PORTS, local_member_array, &local_member_count);
                BCM_IF_ERROR_RETURN(rv);
                for (idx = 0; idx < local_member_count; idx++) {
                    _bcm_port_info_access(unit, local_member_array[idx], &info);
                    info->vp_count++;
                    BCM_IF_ERROR_RETURN(
                        bcm_esw_port_vlan_member_get(
                            unit, local_member_array[idx], &port_flags));
                    BCM_IF_ERROR_RETURN(
                        bcm_esw_port_vlan_member_set(
                            unit, local_member_array[idx], port_flags));
                }
            }
        } else {
            mod_in = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry, 
                                                      MODULE_IDf);
            port_in = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry, 
                                                       PORT_NUMf);
            rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                        mod_in, port_in, &mod_out, &port_out);
            WLAN_INFO(unit)->port_info[vp].modid = mod_out;
            WLAN_INFO(unit)->port_info[vp].port = port_out;
            WLAN_INFO(unit)->port_info[vp].tgid = -1;
            /* Update the physical port's SW state */
            if (stable_size == 0) {
                /* When persistent storage is available, this state is recovered
                   in the port module */
                phys_port = WLAN_INFO(unit)->port_info[vp].port; 
                if (soc_feature(unit, soc_feature_sysport_remap)) { 
                    BCM_XLATE_SYSPORT_S2P(unit, &phys_port); 
                }
                _bcm_port_info_access(unit, phys_port, &info);
                info->vp_count++;
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_vlan_member_get(
                        unit, phys_port, &port_flags));
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_vlan_member_set(
                        unit, phys_port, port_flags));
            }
        }
    } else {
        return BCM_E_INTERNAL; /* Should never happen */
    }
    intf_num = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh_entry,
                                                WLAN__INTF_NUMf);
    _BCM_WLAN_INTF_USED_SET(unit, intf_num);
    BCM_L3_INTF_USED_SET(unit, intf_num);
    return rv;
}

/*
 * Function:
 *      _bcm_tr2_wlan_reinit
 * Purpose:
 *      Warm boot recovery for the WLAN software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_tr2_wlan_reinit(int unit)
{
    int rv, i, j, index_min, index_max, mtu_sel, lport_profile_ptr;
    int tpid_index, tnl_idx, stable_size, vvp, vp = 0;
    uint32 entry_type, tunnel_type, flags, grp_bmp, rval;
    uint32 tnl_entry[SOC_MAX_MEM_FIELD_WORDS];
    uint64 rval64, *rval64s[1];
    uint8 *wlan_svp_table_buf = NULL; /* WLAN_SVP_TABLE DMA buffer */
    wlan_svp_table_entry_t *wlan_svp_table;
    uint8 *egr_vlan_xlate_buf = NULL; /* EGR_VLAN_XLATE DMA buffer */
    egr_vlan_xlate_entry_t *egr_vlan_xlate;
    uint8 *egr_ip_tunnel_buf = NULL; /* EGR_IP_TUNNEL DMA buffer */
    egr_ip_tunnel_entry_t *egr_ip_tunnel;
    uint8 *egr_ip_tunnel_ipv6_buf = NULL; /* EGR_IP_TUNNEL_IPV6 DMA buffer */
    egr_ip_tunnel_ipv6_entry_t *egr_ip_tunnel_ipv6;
    uint8 *vbuf = NULL; /* VLAN_TABLE DMA buffer */
    vlan_tab_entry_t *vtab;
    egr_wlan_dvp_entry_t egr_wlan_dvp;
    lport_tab_entry_t lport_profile;
    source_vp_entry_t source_vp;
    bcm_tunnel_initiator_t info;
    _bcm_vp_info_t vp_info;

    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeWlan;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* DMA various tables */
    wlan_svp_table_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES
                                 (unit, WLAN_SVP_TABLEm), 
                                 "WLAN_SVP_TABLE buffer");
    if (NULL == wlan_svp_table_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    index_min = soc_mem_index_min(unit, WLAN_SVP_TABLEm);
    index_max = soc_mem_index_max(unit, WLAN_SVP_TABLEm);
    if ((rv = soc_mem_read_range(unit, WLAN_SVP_TABLEm, MEM_BLOCK_ANY,
                                 index_min, index_max, 
                                 wlan_svp_table_buf)) < 0 ) {
        goto cleanup;
    }
    egr_vlan_xlate_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES
                                       (unit, EGR_VLAN_XLATEm), 
                                       "EGR_VLAN_XLATE buffer");
    if (NULL == egr_vlan_xlate_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    index_min = soc_mem_index_min(unit, EGR_VLAN_XLATEm);
    index_max = soc_mem_index_max(unit, EGR_VLAN_XLATEm);
    if ((rv = soc_mem_read_range(unit, EGR_VLAN_XLATEm, MEM_BLOCK_ANY,
                                 index_min, index_max, 
                                 egr_vlan_xlate_buf)) < 0 ) {
        goto cleanup;
    }
    egr_ip_tunnel_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES
                                       (unit, EGR_IP_TUNNELm), 
                                       "EGR_IP_TUNNEL buffer");
    if (NULL == egr_ip_tunnel_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    index_min = soc_mem_index_min(unit, EGR_IP_TUNNELm);
    index_max = soc_mem_index_max(unit, EGR_IP_TUNNELm);
    if ((rv = soc_mem_read_range(unit, EGR_IP_TUNNELm, MEM_BLOCK_ANY,
                                 index_min, index_max, 
                                 egr_ip_tunnel_buf)) < 0 ) {
        goto cleanup;
    }
    egr_ip_tunnel_ipv6_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES
                                       (unit, EGR_IP_TUNNEL_IPV6m), 
                                       "EGR_IP_TUNNEL_IPV6 buffer");
    if (NULL == egr_ip_tunnel_ipv6_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    index_min = soc_mem_index_min(unit, EGR_IP_TUNNEL_IPV6m);
    index_max = soc_mem_index_max(unit, EGR_IP_TUNNEL_IPV6m);
    if ((rv = soc_mem_read_range(unit, EGR_IP_TUNNEL_IPV6m, MEM_BLOCK_ANY,
                                 index_min, index_max, 
                                 egr_ip_tunnel_ipv6_buf)) < 0 ) {
        goto cleanup;
    }
    vbuf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, VLAN_TABm), 
                         "VLAN_TAB buffer");
    if (NULL == vbuf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    index_min = soc_mem_index_min(unit, VLAN_TABm);
    index_max = soc_mem_index_max(unit, VLAN_TABm);
    if ((rv = soc_mem_read_range(unit, VLAN_TABm, MEM_BLOCK_ANY,
                                 index_min, index_max, vbuf)) < 0 ) {
        goto cleanup;
    }

    flags = 0;

    /* Get the VPs */
    index_min = soc_mem_index_min(unit, EGR_VLAN_XLATEm);
    index_max = soc_mem_index_max(unit, EGR_VLAN_XLATEm);
    for (i = index_min; i <= index_max; i++) {
        egr_vlan_xlate = soc_mem_table_idx_to_pointer
                         (unit, EGR_VLAN_XLATEm, egr_vlan_xlate_entry_t *, 
                         egr_vlan_xlate_buf, i);
        if (soc_EGR_VLAN_XLATEm_field32_get(unit, 
                                            egr_vlan_xlate, VALIDf) == 0) {
            continue;
        }
        entry_type = soc_EGR_VLAN_XLATEm_field32_get(unit, egr_vlan_xlate, 
                                                     ENTRY_TYPEf);
        if ((entry_type != 5) && (entry_type != 6) && (entry_type != 7)) {
            continue; /* Not WLAN_SVP */
        }
        vp = soc_EGR_VLAN_XLATEm_field32_get(unit, egr_vlan_xlate, 
                                             WLAN_SVP__SVPf);
        wlan_svp_table = soc_mem_table_idx_to_pointer
                         (unit, WLAN_SVP_TABLEm, wlan_svp_table_entry_t *, 
                         wlan_svp_table_buf, vp);
        if (soc_WLAN_SVP_TABLEm_field32_get(unit, 
                                            wlan_svp_table, VALIDf) == 0) {
            rv = BCM_E_INTERNAL; /* Should never happen */
            goto cleanup;
        }
        rv = _bcm_vp_used_set(unit, vp, vp_info);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        switch (entry_type) {
        case 5:
            BCM_GPORT_TUNNEL_ID_SET(WLAN_INFO(unit)->port_info[vp].match_tunnel,
                                    soc_EGR_VLAN_XLATEm_field32_get(unit, 
                                    egr_vlan_xlate, WLAN_SVP__TUNNEL_IDf));
            WLAN_INFO(unit)->port_info[vp].flags |= _BCM_WLAN_PORT_MATCH_TUNNEL;
            break;
        case 6:
            soc_mem_mac_addr_get(unit, EGR_VLAN_XLATEm, egr_vlan_xlate, 
                                 WLAN_SVP__BSSIDf, 
                                 WLAN_INFO(unit)->port_info[vp].match_bssid);
            BCM_GPORT_TUNNEL_ID_SET(WLAN_INFO(unit)->port_info[vp].match_tunnel,
                                    soc_EGR_VLAN_XLATEm_field32_get(unit, 
                                    egr_vlan_xlate, WLAN_SVP__EXP_TUNNEL_IDf));
            WLAN_INFO(unit)->port_info[vp].flags |= _BCM_WLAN_PORT_MATCH_BSSID;
            break;
        case 7:
            WLAN_INFO(unit)->port_info[vp].match_radio = 
                soc_EGR_VLAN_XLATEm_field32_get(unit, egr_vlan_xlate, 
                                                WLAN_SVP__RIDf);
            soc_mem_mac_addr_get(unit, EGR_VLAN_XLATEm, egr_vlan_xlate, 
                                 WLAN_SVP__BSSIDf, 
                                 WLAN_INFO(unit)->port_info[vp].match_bssid);
            BCM_GPORT_TUNNEL_ID_SET(WLAN_INFO(unit)->port_info[vp].match_tunnel,
                                    soc_EGR_VLAN_XLATEm_field32_get(unit, 
                                    egr_vlan_xlate, WLAN_SVP__EXP_TUNNEL_IDf));
            WLAN_INFO(unit)->port_info[vp].flags |= 
                                               _BCM_WLAN_PORT_MATCH_BSSID_RADIO;
            break;
        /*
         * COVERITY
         *
         * This default is unreachable. It is kept intentionally as a defensive 
         * default for future development. 
         */ 
        /* coverity[dead_error_begin] */
        default:
            rv = BCM_E_INTERNAL;
            goto cleanup;
            break;
        }
        /* Update LPORT profile entry refererence count */
        lport_profile_ptr = soc_WLAN_SVP_TABLEm_field32_get
                                (unit, wlan_svp_table, LPORT_PROFILE_IDXf);
        rv = _bcm_lport_profile_mem_reference(unit, lport_profile_ptr, 1);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        rv = READ_LPORT_TABm(unit, MEM_BLOCK_ANY, lport_profile_ptr, 
                             &lport_profile);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        /* Get TPID reference count from the LPORT entry */
        rv = _bcm_tr2_wlan_lport_tpid_recover(unit, &lport_profile);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        /* Recover flex stats from the SOURCE_VP table */
        rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &source_vp); 
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        _bcm_tr2_wlan_port_flex_stat_recover(unit, &source_vp, vp);
        /* Get the TUNNEL_VLAN from the EGR_WLAN_DVP table */
        rv = READ_EGR_WLAN_DVPm(unit, MEM_BLOCK_ANY, vp, &egr_wlan_dvp); 
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        tnl_idx = soc_EGR_WLAN_DVPm_field32_get(unit, &egr_wlan_dvp, 
                                                TUNNEL_INDEXf);
        sal_memset(tnl_entry, 0, sizeof(tnl_entry));
        rv = READ_EGR_IP_TUNNELm(unit, MEM_BLOCK_ANY, tnl_idx, tnl_entry);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        entry_type = soc_EGR_IP_TUNNELm_field32_get(unit, tnl_entry, 
                                                    ENTRY_TYPEf);
        if (entry_type == BCM_XGS3_TUNNEL_INIT_V6) {
            tnl_idx *= 2;
        }
        TUNNEL_VLAN(unit, tnl_idx) = soc_EGR_WLAN_DVPm_field32_get(unit, 
                                         &egr_wlan_dvp, TUNNEL_VLAN_IDf);
        /* Get the VLAN validation state from TUNNEL VPs */
        if (WLAN_INFO(unit)->port_info[vp].flags & 
            _BCM_WLAN_PORT_MATCH_TUNNEL) {
            vvp = soc_WLAN_SVP_TABLEm_field32_get(unit, &source_vp, 
                                                  VLAN_VALIDATION_PTRf);
            for (j = BCM_VLAN_MIN; j < BCM_VLAN_MAX; j++) {
                vtab = soc_mem_table_idx_to_pointer(unit, VLAN_TABm, 
                                                   vlan_tab_entry_t *, vbuf, j);
                grp_bmp = soc_mem_field32_get(unit, VLAN_TABm, vtab, 
                                              VLAN_GROUP_BITMAPf);
                if (grp_bmp & (1 << vvp)) {
                    /* Increase use-count for this bit for this VLAN */
                    WLAN_INFO(unit)->vlan_grp_bmp_use_cnt[j][vvp]++;
                }
            }
        }
        /* Recover other port data */
        rv = _bcm_tr2_wlan_port_associated_data_recover(unit, vp, stable_size);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    }

    /* Get the IPV4 tunnel initiators */
    index_min = soc_mem_index_min(unit, EGR_IP_TUNNELm);
    index_max = soc_mem_index_max(unit, EGR_IP_TUNNELm);
    for (i = index_min; i <= index_max; i++) {
        egr_ip_tunnel = soc_mem_table_idx_to_pointer
                         (unit, EGR_IP_TUNNELm, egr_ip_tunnel_entry_t *, 
                         egr_ip_tunnel_buf, i);
        entry_type = soc_EGR_IP_TUNNELm_field32_get(unit, egr_ip_tunnel, 
                                                    ENTRY_TYPEf);
        tunnel_type = soc_EGR_IP_TUNNELm_field32_get(unit, egr_ip_tunnel, 
                                                     TUNNEL_TYPEf);
        if (entry_type != BCM_XGS3_TUNNEL_INIT_V4) {
            continue; /* Not IPv4 */
        }
        if ((tunnel_type != 8) && (tunnel_type != 9)) {
            continue; /* Not CAPWAP */
        }
        bcm_tunnel_initiator_t_init(&info);
        /* Get the CAPWAP fragment profile */
        mtu_sel = soc_EGR_IP_TUNNELm_field32_get(unit, egr_ip_tunnel, 
                                                 CAPWAP_MTU_SELf);
        rv = READ_EGR_CAPWAP_FRAG_CONTROLr(unit, mtu_sel, &rval);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        COMPILER_64_ZERO(rval64);
        COMPILER_64_SET(rval64, 0, rval);
        rval64s[0] = &rval64;
        rv = soc_profile_reg_add(unit, CAPWAP_FRAG_PROFILE(unit), rval64s, 1, 
                                 (uint32 *)&mtu_sel);
        /* Mark tunnel initiator entry as used in the L3 module */
        flags |= _BCM_L3_SHR_UPDATE | _BCM_L3_SHR_WRITE_DISABLE |
                 _BCM_L3_SHR_WITH_ID;
        tnl_idx = i;
        info.type = (tunnel_type == 8) ? bcmTunnelTypeWlanAcToAc : 
                                         bcmTunnelTypeWlanWtpToAc;
        soc_mem_field_get(unit, EGR_IP_TUNNELm, (uint32 *)egr_ip_tunnel, 
                          DIPf, &info.dip);
        soc_mem_field_get(unit, EGR_IP_TUNNELm, (uint32 *)egr_ip_tunnel, 
                          SIPf, &info.sip);
        if (soc_EGR_IP_TUNNELm_field32_get(unit, egr_ip_tunnel, 
                                           VLAN_ASSIGN_POLICYf)) {
            info.flags |= BCM_TUNNEL_INIT_WLAN_VLAN_TAGGED;
            /* Get the TPID state */
            tpid_index = soc_EGR_IP_TUNNELm_field32_get(unit, egr_ip_tunnel, 
                                                        TUNNEL_TPID_INDEXf);
            rv = _bcm_fb2_outer_tpid_tab_ref_count_add(unit, tpid_index, 1);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            rv = _bcm_fb2_outer_tpid_entry_get(unit, &info.tpid, tpid_index);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            info.pkt_pri = soc_EGR_IP_TUNNELm_field32_get(unit, egr_ip_tunnel, 
                                                          NEW_PRIf);
            info.pkt_cfi = soc_EGR_IP_TUNNELm_field32_get(unit, egr_ip_tunnel, 
                                                          NEW_CFIf);

        }
        if (soc_EGR_IP_TUNNELm_field32_get(unit, egr_ip_tunnel, 
                                           IPV4_DF_SELf) == 2) {
            info.flags |= BCM_TUNNEL_INIT_USE_INNER_DF;
        } else if (soc_EGR_IP_TUNNELm_field32_get(unit, egr_ip_tunnel, 
                                           IPV4_DF_SELf) == 1) {
            info.flags |= BCM_TUNNEL_INIT_IPV4_SET_DF;
        }
        info.dscp = soc_EGR_IP_TUNNELm_field32_get(unit, egr_ip_tunnel, DSCPf);
        info.dscp_sel = soc_EGR_IP_TUNNELm_field32_get(unit, egr_ip_tunnel, 
                                                       DSCP_SELf);
        info.ttl = soc_EGR_IP_TUNNELm_field32_get(unit, egr_ip_tunnel, TTLf);
        soc_mem_mac_addr_get(unit, EGR_IP_TUNNELm, egr_ip_tunnel, 
                             DEST_ADDRf, info.dmac);
        info.udp_src_port = soc_EGR_IP_TUNNELm_field32_get(unit, egr_ip_tunnel, 
                                                           L4_SRC_PORTf);
        info.udp_dst_port = soc_EGR_IP_TUNNELm_field32_get(unit, egr_ip_tunnel, 
                                                           L4_DEST_PORTf);
        info.mtu = soc_reg_field_get(unit, EGR_CAPWAP_FRAG_CONTROLr, 
                                     rval, MTUf);
        info.flags |= BCM_TUNNEL_INIT_WLAN_MTU;
        rv = bcm_xgs3_tnl_init_add(unit, flags, &info, &tnl_idx);
        if (tnl_idx != i) {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }
        _BCM_WLAN_IP_TNL_USED_SET(unit, i);
    }

    /* Get the IPV6 tunnel initiators */
    index_min = soc_mem_index_min(unit, EGR_IP_TUNNEL_IPV6m);
    index_max = soc_mem_index_max(unit, EGR_IP_TUNNEL_IPV6m);
    for (i = index_min; i <= index_max; i++) {
        egr_ip_tunnel_ipv6 = soc_mem_table_idx_to_pointer
                      (unit, EGR_IP_TUNNEL_IPV6m, egr_ip_tunnel_ipv6_entry_t *, 
                      egr_ip_tunnel_ipv6_buf, i);
        entry_type = soc_EGR_IP_TUNNEL_IPV6m_field32_get(unit, 
                                              egr_ip_tunnel_ipv6, ENTRY_TYPEf);
        tunnel_type = soc_EGR_IP_TUNNEL_IPV6m_field32_get(unit, 
                                             egr_ip_tunnel_ipv6, TUNNEL_TYPEf);
        if (entry_type != BCM_XGS3_TUNNEL_INIT_V6) {
            continue; /* Not IPv6 */
        }
        if ((tunnel_type != 8) && (tunnel_type != 9)) {
            continue; /* Not CAPWAP */
        }
        bcm_tunnel_initiator_t_init(&info);
        /* Get the CAPWAP fragment profile */
        mtu_sel = soc_EGR_IP_TUNNEL_IPV6m_field32_get(unit, egr_ip_tunnel_ipv6, 
                                                 CAPWAP_MTU_SELf);
        rv = READ_EGR_CAPWAP_FRAG_CONTROLr(unit, mtu_sel, &rval);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        COMPILER_64_ZERO(rval64);
        COMPILER_64_SET(rval64, 0, rval);
        rval64s[0] = &rval64;
        rv = soc_profile_reg_add(unit, CAPWAP_FRAG_PROFILE(unit), rval64s, 1, 
                                 (uint32 *)&mtu_sel);
        /* Mark tunnel initiator entry as used in the L3 module */
        flags |= _BCM_L3_SHR_UPDATE | _BCM_L3_SHR_WRITE_DISABLE |
                 _BCM_L3_SHR_WITH_ID;
        tnl_idx = i;
        info.type = (tunnel_type == 8) ? bcmTunnelTypeWlanAcToAc6 : 
                                         bcmTunnelTypeWlanWtpToAc6;
        soc_mem_ip6_addr_get(unit, EGR_IP_TUNNEL_IPV6m, (uint32 *)
                             egr_ip_tunnel_ipv6, DIPf, info.dip6, 0);
        soc_mem_ip6_addr_get(unit, EGR_IP_TUNNEL_IPV6m, (uint32 *)
                             egr_ip_tunnel_ipv6, SIPf, info.sip6, 0);
        if (soc_EGR_IP_TUNNELm_field32_get(unit, egr_ip_tunnel_ipv6, 
                                           VLAN_ASSIGN_POLICYf)) {
            info.flags |= BCM_TUNNEL_INIT_WLAN_VLAN_TAGGED;
            /* Get the TPID state */
            tpid_index = soc_EGR_IP_TUNNEL_IPV6m_field32_get(unit, 
                             egr_ip_tunnel_ipv6, TUNNEL_TPID_INDEXf);
            rv = _bcm_fb2_outer_tpid_tab_ref_count_add(unit, tpid_index, 1);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            rv = _bcm_fb2_outer_tpid_entry_get(unit, &info.tpid, tpid_index);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            info.pkt_pri = soc_EGR_IP_TUNNEL_IPV6m_field32_get(unit, 
                               egr_ip_tunnel_ipv6, NEW_PRIf);
            info.pkt_cfi = soc_EGR_IP_TUNNEL_IPV6m_field32_get(unit, 
                               egr_ip_tunnel_ipv6, NEW_CFIf);
        }
        if (soc_EGR_IP_TUNNEL_IPV6m_field32_get(unit, egr_ip_tunnel_ipv6, 
                                                IPV6_DF_SELf) == 1) {
            info.flags |= BCM_TUNNEL_INIT_IPV6_SET_DF;
        }
        info.dscp = soc_EGR_IP_TUNNEL_IPV6m_field32_get(unit, 
                        egr_ip_tunnel_ipv6, DSCPf);
        info.dscp_sel = soc_EGR_IP_TUNNEL_IPV6m_field32_get(unit, 
                            egr_ip_tunnel_ipv6, DSCP_SELf);
        info.flow_label = soc_EGR_IP_TUNNEL_IPV6m_field32_get(unit, 
                              egr_ip_tunnel_ipv6, FLOW_LABELf);
        info.ttl = soc_EGR_IP_TUNNEL_IPV6m_field32_get(unit, egr_ip_tunnel_ipv6,
                                                       TTLf);
        soc_mem_mac_addr_get(unit, EGR_IP_TUNNEL_IPV6m, egr_ip_tunnel_ipv6, 
                             DEST_ADDRf, info.dmac);
        info.udp_src_port = soc_EGR_IP_TUNNEL_IPV6m_field32_get(unit, 
                                egr_ip_tunnel_ipv6, L4_SRC_PORTf);
        info.udp_dst_port = soc_EGR_IP_TUNNEL_IPV6m_field32_get(unit, 
                                egr_ip_tunnel_ipv6, L4_DEST_PORTf);
        info.mtu = soc_reg_field_get(unit, EGR_CAPWAP_FRAG_CONTROLr, 
                                     rval, MTUf);
        info.flags |= BCM_TUNNEL_INIT_WLAN_MTU;
        rv = bcm_xgs3_tnl_init_add(unit, flags, &info, &tnl_idx);
        if (tnl_idx != i) {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }
        _BCM_WLAN_IP_TNL_USED_SET(unit, i * 2);
        _BCM_WLAN_IP_TNL_IS_V6_SET(unit, i * 2);
    }

cleanup:
    if (wlan_svp_table_buf) {
        soc_cm_sfree(unit, wlan_svp_table_buf);
    }
    if (egr_vlan_xlate_buf) {
        soc_cm_sfree(unit, egr_vlan_xlate_buf);
    }
    if (egr_ip_tunnel_buf) {
        soc_cm_sfree(unit, egr_ip_tunnel_buf);
    }
    if (egr_ip_tunnel_ipv6_buf) {
        soc_cm_sfree(unit, egr_ip_tunnel_ipv6_buf);
    }
    if (vbuf) {
        soc_cm_sfree(unit, vbuf);
    }
    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcm_wlan_init
 * Purpose:
 *      Initialize the WLAN software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr2_wlan_init(int unit)
{
    int i, num_vp, num_tnl, num_intf, rv = BCM_E_NONE;
    _bcm_tr2_wlan_bookkeeping_t *wlan_info = WLAN_INFO(unit);
    soc_reg_t reg;
    uint64 rval64, *rval64s[1];
    uint32 capwap_frag_profile;

    if (!L3_INFO(unit)->l3_initialized) {
        LOG_INFO(BSL_LS_BCM_L3,
                 (BSL_META_U(unit,
                             "L3 module must be initialized first\n")));
        return BCM_E_NONE;
    }
    if (wlan_initialized[unit]) {
        BCM_IF_ERROR_RETURN(bcm_tr2_wlan_detach(unit));
    }
    num_vp = soc_mem_index_count(unit, WLAN_SVP_TABLEm);

    sal_memset(wlan_info, 0, sizeof(_bcm_tr2_wlan_bookkeeping_t));

    /* Create WLAN port usage bitmap */
    if (wlan_info->port_info == NULL) {
        wlan_info->port_info = sal_alloc(sizeof(_bcm_tr2_wlan_port_info_t) 
                                       * num_vp, "wlan_port_info");
        if (wlan_info->port_info == NULL) {
            _bcm_tr2_wlan_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(wlan_info->port_info, 0, sizeof(_bcm_tr2_wlan_port_info_t) 
               * num_vp);

    /* Create profile register cache for EGR_CAPWAP_FRAG_CONTROL */
    reg = EGR_CAPWAP_FRAG_CONTROLr;
    if (NULL == wlan_info->capwap_frag_profile) {
        wlan_info->capwap_frag_profile = sal_alloc(sizeof(soc_profile_reg_t),
                                         "CAPWAP Frag Ctrl Profile Reg");
        if (wlan_info->capwap_frag_profile == NULL) {
            _bcm_tr2_wlan_free_resources(unit);
            return BCM_E_MEMORY;
        }
        soc_profile_reg_t_init(wlan_info->capwap_frag_profile);

        rv = soc_profile_reg_create(unit, &reg, 1, wlan_info->capwap_frag_profile);
        wlan_info->capwap_frag_profile_created = TRUE;
        if (rv < 0) {
            _bcm_tr2_wlan_free_resources(unit);
            return rv;
        }
        COMPILER_64_ZERO(rval64);
        rval64s[0] = &rval64;
        rv = soc_profile_reg_add(unit, CAPWAP_FRAG_PROFILE(unit), rval64s, 1,
                            &capwap_frag_profile);
        if (BCM_FAILURE(rv)) {
            _bcm_tr2_wlan_free_resources(unit);
            return rv;
        }
    }

    /* Allocate tunnel VLAN cache */
    num_tnl = soc_mem_index_count(unit, EGR_IP_TUNNELm);
    if (NULL == wlan_info->tunnel_vlan) {
        wlan_info->tunnel_vlan =
                sal_alloc(sizeof(bcm_vlan_t) * num_tnl, "tunnel vlan cache");
        if (wlan_info->tunnel_vlan == NULL) {
            _bcm_tr2_wlan_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(wlan_info->tunnel_vlan, 0, sizeof(bcm_vlan_t) * num_tnl);

    /* Allocate L3 interface usage bitmap */
    num_intf = soc_mem_index_count(unit, EGR_L3_INTFm);
    if (NULL == wlan_info->intf_bitmap) {
        wlan_info->intf_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(num_intf), "intf_bitmap");
        if (wlan_info->intf_bitmap == NULL) {
            _bcm_tr2_wlan_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(wlan_info->intf_bitmap, 0, SHR_BITALLOCSIZE(num_intf));    

    /* Create EGR_IP_TUNNEL usage bitmap */
    wlan_info->ip_tnl_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(num_tnl), "wlan ip_tnl_bitmap");
    if (wlan_info->ip_tnl_bitmap == NULL) {
        _bcm_tr2_wlan_free_resources(unit);
	return BCM_E_MEMORY;
    }
    sal_memset(wlan_info->ip_tnl_bitmap, 0, SHR_BITALLOCSIZE(num_tnl));

    /* Create EGR_IP_TUNNEL is V6 usage bitmap */
    wlan_info->ip_tnl_is_v6_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(num_tnl), "wlan ip_tnl_bitmap");
    if (wlan_info->ip_tnl_is_v6_bitmap == NULL) {
        _bcm_tr2_wlan_free_resources(unit);
	return BCM_E_MEMORY;
    }
    sal_memset(wlan_info->ip_tnl_is_v6_bitmap, 0, SHR_BITALLOCSIZE(num_tnl));

    /* Allocate the VLAN group bitmap use count per VLAN */
    wlan_info->vlan_grp_bmp_use_cnt = (uint8 **)sal_alloc(BCM_VLAN_COUNT * 
                             sizeof(uint8 *), "VLAN group bitmap pointer list");
    if (wlan_info->vlan_grp_bmp_use_cnt == NULL) {
        _bcm_tr2_wlan_free_resources(unit);
	return BCM_E_MEMORY;
    }
    for (i = 0; i < BCM_VLAN_COUNT; i++) {
        wlan_info->vlan_grp_bmp_use_cnt[i] = (uint8 *)sal_alloc(32 * 
                                            sizeof(uint8), "VLAN group bitmap");
        if (wlan_info->vlan_grp_bmp_use_cnt[i] == NULL) {
            _bcm_tr2_wlan_free_resources(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(wlan_info->vlan_grp_bmp_use_cnt[i], 0, 32 * sizeof(uint8));
    }

    /* Create WLAN mutex */
    if (_wlan_mutex[unit] == NULL) {
        _wlan_mutex[unit] = sal_mutex_create("wlan mutex");
        if (_wlan_mutex[unit] == NULL) {
            _bcm_tr2_wlan_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        /* Warm Boot recovery */
        rv = _bcm_tr2_wlan_reinit(unit); 
    } 
#endif

    /* Enable egress VXLT lookup on the loopback port */
    BCM_IF_ERROR_RETURN(bcm_esw_vlan_control_port_set
                        (unit, 54, bcmVlanTranslateEgressEnable, 1));

    wlan_initialized[unit] = TRUE;
    return rv;
}

/*
 * Function:
 *      _bcm_tr2_wlan_hw_clear
 * Purpose:
 *     Perform HW tables clean up for WLAN module.
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr2_wlan_hw_clear(int unit)
{
    int rv, rv_error = BCM_E_NONE;

    /* Delete all ports */
    rv = bcm_tr2_wlan_port_delete_all(unit);
    if (BCM_FAILURE(rv) && (BCM_E_NONE == rv_error)) {
        rv_error = rv;
    }

    /* Delete all clients */
    rv = bcm_tr2_wlan_client_delete_all(unit);
    if (BCM_FAILURE(rv) && (BCM_E_NONE == rv_error)) {
        rv_error = rv;
    }

    return rv_error;
}

/*
 * Function:
 *      bcm_wlan_detach
 * Purpose:
 *      Detach the WLAN software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr2_wlan_detach(int unit)
{
    int rv = BCM_E_NONE;

    if (wlan_initialized[unit])
    {
        if (0 == SOC_HW_ACCESS_DISABLE(unit)) {
            rv = _bcm_tr2_wlan_hw_clear(unit);
        }

        _bcm_tr2_wlan_free_resources(unit);
        wlan_initialized[unit] = FALSE;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr2_wlan_client_api_to_hw
 * Purpose:
 *      Convert a hardware-independent WLAN client entry to a TR2 HW entry
 * Parameters:
 *      unit - Unit number
 *      hw_entry - (OUT) Triumph 2 HW entry
 *      info - Hardware-independent WLAN client entry
 */
STATIC int
_bcm_tr2_wlan_client_api_to_hw(int unit, mpls_entry_entry_t *hw_entry, 
                               bcm_wlan_client_t *info)
{
    sal_memset(hw_entry, 0, sizeof(mpls_entry_entry_t));
    if ((info->flags & BCM_WLAN_CLIENT_ROAMED_IN) && 
        (info->flags & BCM_WLAN_CLIENT_ROAMED_OUT)) {
        return BCM_E_PARAM;
    }
    if (BCM_MAC_IS_MCAST(info->mac)) {
        return BCM_E_PARAM;
    }
    soc_MPLS_ENTRYm_field32_set(unit, hw_entry, VALIDf, 1);
    soc_MPLS_ENTRYm_field32_set(unit, hw_entry, KEY_TYPEf, 4); /* WLAN MAC */
    soc_mem_mac_addr_set(unit, MPLS_ENTRYm, hw_entry, WLAN_MAC__MAC_ADDRf, 
                         info->mac);
    if (info->flags & BCM_WLAN_CLIENT_ROAMED_IN) {
        int vp;
        if (!BCM_GPORT_IS_WLAN_PORT(info->home_agent) || 
            !BCM_GPORT_IS_WLAN_PORT(info->wtp)) {
            return BCM_E_PARAM;
        } 
        soc_MPLS_ENTRYm_field32_set(unit, hw_entry, WLAN_MAC__RICf, 1);
        vp = BCM_GPORT_WLAN_PORT_ID_GET(info->home_agent);
        soc_MPLS_ENTRYm_field32_set(unit, hw_entry, WLAN_MAC__RIC_HA_VPf, vp);
        vp = BCM_GPORT_WLAN_PORT_ID_GET(info->wtp);
        soc_MPLS_ENTRYm_field32_set(unit, hw_entry, WLAN_MAC__RIC_WTP_VPf, vp);
    }
    if (info->flags & BCM_WLAN_CLIENT_ROAMED_OUT) {
        soc_MPLS_ENTRYm_field32_set(unit, hw_entry, WLAN_MAC__ROCf, 1);
    }
    if (info->flags & BCM_WLAN_CLIENT_AUTHORIZED) {
        soc_MPLS_ENTRYm_field32_set(unit, hw_entry, WLAN_MAC__DOT1X_STATEf, 1);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr2_wlan_client_api_from_hw
 * Purpose:
 *      Convert a TR2 HW entry to a hardware-independent WLAN client entry 
 * Parameters:
 *      unit - Unit number
 *      hw_entry - Triumph 2 HW entry
 *      info - (OUT) Hardware-independent WLAN client entry
 */
STATIC int
_bcm_tr2_wlan_client_api_from_hw(int unit, bcm_wlan_client_t *info, 
                                 mpls_entry_entry_t *hw_entry)
{
    int vp;
    sal_memset(info, 0, sizeof (bcm_wlan_client_t));
    soc_mem_mac_addr_get(unit, MPLS_ENTRYm, hw_entry, WLAN_MAC__MAC_ADDRf, 
                         info->mac);
    if (soc_MPLS_ENTRYm_field32_get(unit, hw_entry, WLAN_MAC__RICf)) {
        info->flags |= BCM_WLAN_CLIENT_ROAMED_IN;
        vp = soc_MPLS_ENTRYm_field32_get(unit, hw_entry, 
                                         WLAN_MAC__RIC_HA_VPf);
        BCM_GPORT_WLAN_PORT_ID_SET(info->home_agent, vp);
        vp = soc_MPLS_ENTRYm_field32_get(unit, hw_entry, 
                                         WLAN_MAC__RIC_WTP_VPf);
        BCM_GPORT_WLAN_PORT_ID_SET(info->wtp, vp);
    }
    if (soc_MPLS_ENTRYm_field32_get(unit, hw_entry, WLAN_MAC__ROCf)) {
        info->flags |= BCM_WLAN_CLIENT_ROAMED_OUT;
    }
    if (soc_MPLS_ENTRYm_field32_get(unit, hw_entry, WLAN_MAC__DOT1X_STATEf)) {
        info->flags |= BCM_WLAN_CLIENT_AUTHORIZED;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_wlan_client_add
 * Purpose:
 *      Add a wireless client to the database
 * Parameters:
 *      unit     - Device Number
 *      info     - (IN/OUT) Wireless Client structure
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr2_wlan_client_add(int unit, bcm_wlan_client_t *info)
{
    int index, rv = BCM_E_UNAVAIL;
    mpls_entry_entry_t client_entry_key, client_entry_lookup;

    WLAN_INIT(unit);

    BCM_IF_ERROR_RETURN
        (_bcm_tr2_wlan_client_api_to_hw(unit, &client_entry_key, info));

    WLAN_LOCK(unit);
    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index, 
                        (void *)&client_entry_key, 
                        (void *)&client_entry_lookup, 0);

    if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
        WLAN_UNLOCK(unit);
        return rv;
    } else if ((rv == BCM_E_NONE) && 
               !(info->flags & BCM_WLAN_CLIENT_REPLACE)) {
        WLAN_UNLOCK(unit);
        return BCM_E_EXISTS;
    }

    rv = soc_mem_insert(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, 
                                       (void *)&client_entry_key); 
    WLAN_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_wlan_client_delete
 * Purpose:
 *      Delete a wireless client from the database
 * Parameters:
 *      unit     - Device Number
 *      mac      - MAC address of client (lookup key)
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr2_wlan_client_delete(int unit, bcm_mac_t mac)
{
    int index, rv = BCM_E_UNAVAIL;
    mpls_entry_entry_t client_entry_key, client_entry_lookup;

    WLAN_INIT(unit);

    sal_memset(&client_entry_key, 0, sizeof(mpls_entry_entry_t));
    sal_memset(&client_entry_lookup, 0, sizeof(mpls_entry_entry_t));

    soc_MPLS_ENTRYm_field32_set(unit, &client_entry_key, VALIDf, 1);
    soc_MPLS_ENTRYm_field32_set(unit, &client_entry_key, KEY_TYPEf, 4); 
    soc_mem_mac_addr_set(unit, MPLS_ENTRYm, &client_entry_key, 
                         WLAN_MAC__MAC_ADDRf, mac); 
    WLAN_LOCK(unit);
    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index, 
                        (void *)&client_entry_key, 
                        (void *)&client_entry_lookup, 0);
    if (BCM_FAILURE(rv)) {
        WLAN_UNLOCK(unit);
        return rv;
    }
    rv = soc_mem_delete(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, 
                        (void *)&client_entry_key);
    WLAN_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_wlan_client_delete_all
 * Purpose:
 *      Delete all wireless clients from the database
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr2_wlan_client_delete_all(int unit)
{
    int i, valid, key_type, index_min, index_max, rv = BCM_E_NONE;
    mpls_entry_entry_t hw_entry;

    WLAN_INIT(unit);

    index_min = soc_mem_index_min(unit, MPLS_ENTRYm);
    index_max = soc_mem_index_max(unit, MPLS_ENTRYm);
    WLAN_LOCK(unit);
    for (i = index_min; i <= index_max; i++) {
        rv = soc_mem_read(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, i, 
                          (void *)&hw_entry);
        if (BCM_FAILURE(rv)) {
            break;
        }
        valid = soc_MPLS_ENTRYm_field32_get(unit, &hw_entry, VALIDf);
        key_type = soc_MPLS_ENTRYm_field32_get(unit, &hw_entry, KEY_TYPEf);
        if (!valid || (key_type != 0x4)) {
            continue;
        }
        soc_MPLS_ENTRYm_field32_set(unit, &hw_entry, VALIDf, 0);
        rv = soc_mem_write(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, i, 
                           (void *)&hw_entry);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    WLAN_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_wlan_client_get
 * Purpose:
 *      Get a wireless client from the database
 * Parameters:
 *      unit     - Device Number
 *      mac      - MAC address of client (lookup key)
 *      info     - (IN/OUT) Wireless Client structure
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr2_wlan_client_get(int unit, bcm_mac_t mac, bcm_wlan_client_t *info)
{
    int index, rv = BCM_E_UNAVAIL;
    mpls_entry_entry_t client_entry_key, client_entry_lookup;

    WLAN_INIT(unit);

    sal_memset(&client_entry_key, 0, sizeof(mpls_entry_entry_t));
    sal_memset(&client_entry_lookup, 0, sizeof(mpls_entry_entry_t));

    soc_MPLS_ENTRYm_field32_set(unit, &client_entry_key, VALIDf, 1);
    soc_MPLS_ENTRYm_field32_set(unit, &client_entry_key, KEY_TYPEf, 4);
    soc_mem_mac_addr_set(unit, MPLS_ENTRYm, &client_entry_key, 
                         WLAN_MAC__MAC_ADDRf, mac); 
    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index, 
                        (void *)&client_entry_key, 
                        (void *)&client_entry_lookup, 0);
    BCM_IF_ERROR_RETURN(rv);
    bcm_wlan_client_t_init(info);
    rv = _bcm_tr2_wlan_client_api_from_hw(unit, info, &client_entry_lookup);
    return rv; 
}

/*
 * Function:
 *      bcm_wlan_client_traverse
 * Purpose:
 *      Walks through the valid WLAN client entries and calls
 *      the user supplied callback function for each entry.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      trav_fn    - (IN) Callback function.
 *      user_data  - (IN) User data to be passed to callback function.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_tr2_wlan_client_traverse(int unit,
                             bcm_wlan_client_traverse_cb cb,
                             void *user_data)
{
    int i, index_min, index_max, rv = BCM_E_NONE;
    int valid, key_type, *buffer = NULL;
    mpls_entry_entry_t *hw_entry;
    bcm_wlan_client_t info;

    WLAN_INIT(unit);

    index_min = soc_mem_index_min(unit, MPLS_ENTRYm);
    index_max = soc_mem_index_max(unit, MPLS_ENTRYm);

    WLAN_LOCK(unit);
    buffer = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, MPLS_ENTRYm), 
                           "wlan client traverse");
    if (NULL == buffer) {
        WLAN_UNLOCK(unit);
        return BCM_E_MEMORY;
    }
    if ((rv = soc_mem_read_range(unit, MPLS_ENTRYm, MEM_BLOCK_ALL,
                                 index_min, index_max, buffer)) < 0 ) {
        soc_cm_sfree(unit, buffer);
        WLAN_UNLOCK(unit);
        return rv;
    }
    for (i = index_min; i <= index_max; i++) {
        hw_entry = soc_mem_table_idx_to_pointer(unit, MPLS_ENTRYm,
                                         mpls_entry_entry_t *, buffer, i);
        valid = soc_MPLS_ENTRYm_field32_get(unit, hw_entry, VALIDf);
        key_type = soc_MPLS_ENTRYm_field32_get(unit, hw_entry, KEY_TYPEf);
        if (!valid || (key_type != 0x4)) {
            continue;
        }
        bcm_wlan_client_t_init(&info);
        rv = _bcm_tr2_wlan_client_api_from_hw(unit, &info, hw_entry);
        if (BCM_FAILURE(rv)) {
            soc_cm_sfree(unit, buffer);
            WLAN_UNLOCK(unit);
            return rv;
        }
        rv = cb(unit, &info, user_data);
        if (BCM_FAILURE(rv)) {
            soc_cm_sfree(unit, buffer);
            WLAN_UNLOCK(unit); 
            return rv;
        }
    }
    soc_cm_sfree(unit, buffer);
    WLAN_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_tr2_wlan_port_resolve
 * Purpose:
 *      Get the modid, port, trunk values for a WLAN port
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr2_wlan_port_resolve(int unit, bcm_gport_t wlan_port_id,
                          bcm_module_t *modid, bcm_port_t *port,
                          bcm_trunk_t *trunk_id, int *id)

{
    int rv = BCM_E_NONE, nh_index, vp;
    ing_l3_next_hop_entry_t ing_nh;
    ing_dvp_table_entry_t dvp;

    WLAN_INIT(unit);

    if (!BCM_GPORT_IS_WLAN_PORT(wlan_port_id)) {
        return (BCM_E_BADID);
    }

    vp = BCM_GPORT_WLAN_PORT_ID_GET(wlan_port_id);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
        return BCM_E_NOT_FOUND;
    }
    BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                                              NEXT_HOP_INDEXf);
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                                      nh_index, &ing_nh));

    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, ENTRY_TYPEf) != 0x2) {
        /* Entry type is not L2 DVP */
        return BCM_E_NOT_FOUND;
    }
    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
        *trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, TGIDf);
    } else {
        /* Only add this to replication set if destination is local */
        *modid = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
        *port = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, PORT_NUMf);
    }
    *id = vp;
    return rv;
}

/*
 * Function:
 *      bcm_tr2_wlan_port_learn_get
 * Purpose:
 *      Get the CML bits for an wlan port
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_wlan_port_learn_get(int unit, bcm_gport_t wlan_port_id, uint32 *flags)
{
    int rv, cml = 0;
    uint32 vp;
    source_vp_entry_t svp;
    int lport_ptr = 0;
    lport_tab_entry_t lport_profile;
    rtag7_port_based_hash_entry_t rtag7_entry;
    void *entries[2];

    WLAN_INIT(unit);

    /* Get the VP index from the gport */
    vp = BCM_GPORT_WLAN_PORT_ID_GET(wlan_port_id);

    /* Be sure the entry is used and is set for VPLS */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeAny)) {
        return BCM_E_NOT_FOUND;
    }
    rv = READ_WLAN_SVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &svp);
    BCM_IF_ERROR_RETURN(rv);

    /* Get lport_profile */
    lport_ptr = soc_WLAN_SVP_TABLEm_field32_get(unit, &svp, 
                                                LPORT_PROFILE_IDXf);
    entries[0] = &lport_profile;
    entries[1] = &rtag7_entry;
    rv = _bcm_lport_profile_entry_get(unit, lport_ptr, 1, entries);
    BCM_IF_ERROR_RETURN(rv);
    cml = soc_LPORT_TABm_field32_get(unit, &lport_profile, CML_FLAGS_NEWf);

    *flags = 0;
    if (!(cml & (1 << 0))) {
       *flags |= BCM_PORT_LEARN_FWD;
    }
    if (cml & (1 << 1)) {
       *flags |= BCM_PORT_LEARN_CPU;
    }
    if (cml & (1 << 2)) {
       *flags |= BCM_PORT_LEARN_PENDING;
    }
    if (cml & (1 << 3)) {
       *flags |= BCM_PORT_LEARN_ARL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr2_wlan_port_learn_set
 * Purpose:
 *      Set the CML bits for an wlan port.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_wlan_port_learn_set(int unit, bcm_gport_t wlan_port_id, uint32 flags)
{
    int cml_old = 0, cml = 0, rv = BCM_E_NONE;
    uint32 vp;
    wlan_svp_table_entry_t svp;
    lport_tab_entry_t lport_profile;
    rtag7_port_based_hash_entry_t rtag7_entry;
    int lport_ptr, old_lport_ptr = -1;
    void *entries[2];

    WLAN_INIT(unit);

    cml = 0;
    if (!(flags & BCM_PORT_LEARN_FWD)) {
       cml |= (1 << 0);
    }
    if (flags & BCM_PORT_LEARN_CPU) {
       cml |= (1 << 1);
    }
    if (flags & BCM_PORT_LEARN_PENDING) {
       cml |= (1 << 2);
    }
    if (flags & BCM_PORT_LEARN_ARL) {
       cml |= (1 << 3);
    }

    /* Get the VP index from the gport */
    vp = BCM_GPORT_WLAN_PORT_ID_GET(wlan_port_id);

    WLAN_LOCK(unit);
    /* Be sure the entry is used and is set for WLAN */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
        WLAN_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    }
    rv = READ_WLAN_SVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (rv < 0) {
        WLAN_UNLOCK(unit);
        return rv;
    }
    /* Get old lport_profile */
    old_lport_ptr = soc_WLAN_SVP_TABLEm_field32_get(unit, &svp, 
                                                    LPORT_PROFILE_IDXf);
    entries[0] = &lport_profile;
    entries[1] = &rtag7_entry;
    rv = _bcm_lport_profile_entry_get(unit, old_lport_ptr, 1, entries);
    if (rv < 0) {
        WLAN_UNLOCK(unit);
        return rv;
    }
    cml_old = soc_LPORT_TABm_field32_get(unit, &lport_profile, CML_FLAGS_NEWf);
    if (cml != cml_old) {
        soc_LPORT_TABm_field32_set(unit, &lport_profile, CML_FLAGS_MOVEf, cml);
        soc_LPORT_TABm_field32_set(unit, &lport_profile, CML_FLAGS_NEWf, cml);
        rv = _bcm_lport_profile_entry_add(unit, entries, 1, 
                                          (uint32 *) &lport_ptr);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            return rv;
        }
        soc_WLAN_SVP_TABLEm_field32_set(unit, &svp, LPORT_PROFILE_IDXf, 
                                        lport_ptr);
        /* coverity[negative_returns : FALSE] */
        rv = WRITE_WLAN_SVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &svp);
        if (BCM_FAILURE(rv)) {
            WLAN_UNLOCK(unit);
            return rv;
        }
        rv = _bcm_lport_profile_entry_delete(unit, old_lport_ptr);
    }
    WLAN_UNLOCK(unit);
    return rv;
}

STATIC int
_bcm_tr2_wlan_l3_intf_add(int unit, _bcm_l3_intf_cfg_t *if_info)
{
    int i, num_intf;
    egr_l3_intf_entry_t egr_intf;
    bcm_mac_t hw_mac;
    num_intf = soc_mem_index_count(unit, EGR_L3_INTFm);
    for (i = 0; i < num_intf; i++) {
        if (_BCM_WLAN_INTF_USED_GET(unit, i)) {
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_INTFm,  
                                             MEM_BLOCK_ANY, i, &egr_intf));
            soc_mem_mac_addr_get(unit, EGR_L3_INTFm, &egr_intf, 
                                 MAC_ADDRESSf, hw_mac);
            if (SAL_MAC_ADDR_EQUAL(hw_mac, if_info->l3i_mac_addr)) {
                if_info->l3i_index = i;
                return BCM_E_NONE;
            }
        }
    }
    /* Create an interface */
    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_intf_create(unit, if_info));
    _BCM_WLAN_INTF_USED_SET(unit, if_info->l3i_index);
    return BCM_E_NONE;
}

typedef struct _bcm_tr2_ing_nh_info_s {
    int      port;
    int      module;
    int      trunk;
    uint32   mtu;
} _bcm_tr2_ing_nh_info_t;

typedef struct _bcm_tr2_egr_nh_info_s {
    uint8     entry_type;
    uint16    capwap_mc_bitmap;
    int       dvp;
    int       intf_num;
    bcm_mac_t macda;
} _bcm_tr2_egr_nh_info_t;

STATIC int
_bcm_tr2_wlan_nh_info_add(int unit, bcm_wlan_port_t *wlan_port, int vp, int drop,
                           int *nh_index, bcm_port_t *local_port, int *is_local) 
{
    initial_ing_l3_next_hop_entry_t initial_ing_nh;
    ing_l3_next_hop_entry_t ing_nh;
    egr_l3_next_hop_entry_t egr_nh;
    _bcm_tr2_ing_nh_info_t ing_nh_info;
    _bcm_tr2_egr_nh_info_t egr_nh_info;
    bcm_l3_egress_t nh_info;
    _bcm_l3_intf_cfg_t if_info;
    uint32 nh_flags;
    int gport_id, rv;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    _bcm_port_info_t *info;
    uint32 port_flags;

    /* Initialize values */
    sal_memset(&ing_nh_info, 0, sizeof(_bcm_tr2_ing_nh_info_t));
    sal_memset(&egr_nh_info, 0, sizeof(_bcm_tr2_egr_nh_info_t));
    *local_port = 0;
    *is_local = 0;
    ing_nh_info.mtu = 0x3fff; 
    ing_nh_info.port = -1;
    ing_nh_info.module = -1;
    ing_nh_info.trunk = -1;

    egr_nh_info.dvp = vp;
    egr_nh_info.intf_num = -1;
    egr_nh_info.capwap_mc_bitmap = 0;
    egr_nh_info.entry_type = 0x4;

    if (wlan_port->flags & BCM_WLAN_PORT_REPLACE) {
        if ((*nh_index > soc_mem_index_max(unit, EGR_L3_NEXT_HOPm)) ||
            (*nh_index < soc_mem_index_min(unit, EGR_L3_NEXT_HOPm)))  {
            return BCM_E_PARAM;
        }
        /* Read the existing egress next_hop entry */
        rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                          *nh_index, &egr_nh);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        /*
         * Allocate a next-hop entry. By calling bcm_xgs3_nh_add()
         * with _BCM_L3_SHR_WRITE_DISABLE flag, a next-hop index is
         * allocated but nothing is written to hardware. The "nh_info"
         * in this case is not used, so just set to all zeros.
         */
        bcm_l3_egress_t_init(&nh_info);

        nh_flags = _BCM_L3_SHR_MATCH_DISABLE | _BCM_L3_SHR_WRITE_DISABLE;
        rv = bcm_xgs3_nh_add(unit, nh_flags, &nh_info, nh_index);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Resolve the gport */
    rv = _bcm_esw_gport_resolve(unit, wlan_port->port, &mod_out, 
                                &port_out, &trunk_id, &gport_id);
    BCM_IF_ERROR_RETURN(rv);

    if (BCM_GPORT_IS_TRUNK(wlan_port->port)) {
        ing_nh_info.module = -1;
        ing_nh_info.port = -1;
        ing_nh_info.trunk = trunk_id;
        WLAN_INFO(unit)->port_info[vp].modid = -1;
        WLAN_INFO(unit)->port_info[vp].port = -1;
        WLAN_INFO(unit)->port_info[vp].tgid = trunk_id;
    } else {
        ing_nh_info.module = mod_out;
        ing_nh_info.port = port_out;
        ing_nh_info.trunk = -1;
        BCM_IF_ERROR_RETURN(
            _bcm_esw_modid_is_local(unit, mod_out, is_local));
        if (TRUE == *is_local) {
            /* Indicated to calling function that this is a local port */
            *is_local = 1;
            *local_port = ing_nh_info.port;
        }
        WLAN_INFO(unit)->port_info[vp].modid = mod_out;
        WLAN_INFO(unit)->port_info[vp].port = port_out;
        WLAN_INFO(unit)->port_info[vp].tgid = -1;
    }

    if (wlan_port->flags & BCM_WLAN_PORT_EGRESS_CLIENT_MULTICAST) {
        /* CAPWAP_WLAN_MC_BITMAP and MAC_ADDRESS are overlays */
        egr_nh_info.capwap_mc_bitmap = wlan_port->client_multicast;
    }

    /* Write INITIAL_ING_L3_NEXT_HOP entry */
    sal_memset(&initial_ing_nh, 0, sizeof(initial_ing_l3_next_hop_entry_t));
    if (ing_nh_info.trunk == -1) {
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                            &initial_ing_nh, PORT_NUMf, ing_nh_info.port);
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                            &initial_ing_nh, MODULE_IDf, ing_nh_info.module);
    } else {
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                            &initial_ing_nh, Tf, 1);
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                            &initial_ing_nh, TGIDf, ing_nh_info.trunk);
        BCM_GPORT_TRUNK_SET(*local_port, ing_nh_info.trunk);
    }
    rv = soc_mem_write(unit, INITIAL_ING_L3_NEXT_HOPm,
                       MEM_BLOCK_ALL, *nh_index, &initial_ing_nh);
    if (rv < 0) {
        goto cleanup;
    }

    /* Add an L3 interface entry with L2_SWITCH=1 - ref count if exists */
    sal_memset(&if_info, 0, sizeof(_bcm_l3_intf_cfg_t));
    if_info.l3i_flags |= BCM_L3_L2ONLY | BCM_L3_SECONDARY;
    rv = _bcm_tr2_wlan_l3_intf_add(unit, &if_info);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Populate the fields of WLAN::EGR_l3_NEXT_HOP */
    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        ENTRY_TYPEf, egr_nh_info.entry_type);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        WLAN__DVPf, egr_nh_info.dvp);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        WLAN__INTF_NUMf, if_info.l3i_index);

    if (egr_nh_info.capwap_mc_bitmap != 0) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            WLAN__CAPWAP_WLAN_MC_BITMAPf, 
                            egr_nh_info.capwap_mc_bitmap);
    }

    /* Write EGR_L3_NEXT_HOP entry */
    rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                       MEM_BLOCK_ALL, *nh_index, &egr_nh);
    if (rv < 0) {
        goto cleanup;
    }

    /* Write ING_L3_NEXT_HOP entry */
    sal_memset(&ing_nh, 0, sizeof(ing_l3_next_hop_entry_t));
    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, DROPf, drop);
    if (ing_nh_info.trunk == -1) {
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                            &ing_nh, PORT_NUMf, ing_nh_info.port);
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                            &ing_nh, MODULE_IDf, ing_nh_info.module);
    } else {    
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                            &ing_nh, Tf, 1);
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                            &ing_nh, TGIDf, ing_nh_info.trunk);
    }
    if (drop) {
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, DROPf, drop);
    }
    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                       &ing_nh, ENTRY_TYPEf, 0x2); /* L2 DVP */
    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                       &ing_nh, MTU_SIZEf, ing_nh_info.mtu);
    rv = soc_mem_write (unit, ING_L3_NEXT_HOPm,
                       MEM_BLOCK_ALL, *nh_index, &ing_nh);
    if (rv < 0) {
        goto cleanup;
    }

    /* Update the physical port's SW state */
    if (*is_local) {
        bcm_port_t phys_port = WLAN_INFO(unit)->port_info[vp].port; 
        if (soc_feature(unit, soc_feature_sysport_remap)) { 
            BCM_XLATE_SYSPORT_S2P(unit, &phys_port); 
        }
        _bcm_port_info_access(unit, phys_port, &info);
        info->vp_count++;
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_vlan_member_get(
                unit, phys_port, &port_flags));
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_vlan_member_set(
                unit, phys_port, port_flags));
    }

    /* If associated with a trunk, update each local physical port's SW state */
    if (ing_nh_info.trunk != -1) {
        int idx;
        bcm_port_t local_member_array[SOC_MAX_NUM_PORTS];
        int local_member_count;

        rv = _bcm_esw_trunk_local_members_get(unit, trunk_id,
                SOC_MAX_NUM_PORTS, local_member_array, &local_member_count);
        if (rv < 0) {
            goto cleanup;
        }
        for (idx = 0; idx < local_member_count; idx++) {
            _bcm_port_info_access(unit, local_member_array[idx], &info);
            info->vp_count++;
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_vlan_member_get(
                    unit, local_member_array[idx], &port_flags));
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_vlan_member_set(
                    unit, local_member_array[idx], port_flags));
        }
    }

    return rv;

cleanup:
    if (!(wlan_port->flags & BCM_WLAN_PORT_REPLACE)) {
        (void) bcm_xgs3_nh_del(unit, _BCM_L3_SHR_WRITE_DISABLE, *nh_index);
    }
    return rv;
}

STATIC int
_bcm_tr2_wlan_nh_info_delete(int unit, int nh_index)
{
    int rv;
    initial_ing_l3_next_hop_entry_t initial_ing_nh;
    ing_l3_next_hop_entry_t ing_nh;
    egr_l3_next_hop_entry_t egr_nh;

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, 
                                  MEM_BLOCK_ANY, nh_index, &egr_nh));

    if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, 
                                                ENTRY_TYPEf) != 0x4) {
        /* Not a WLAN VP */
        return BCM_E_NOT_FOUND;
    }

    /* Clear EGR_L3_NEXT_HOP entry */
    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN (soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                                   MEM_BLOCK_ALL, nh_index, &egr_nh));

    /* Clear ING_L3_NEXT_HOP entry */
    sal_memset(&ing_nh, 0, sizeof(ing_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN (soc_mem_write (unit, ING_L3_NEXT_HOPm,
                                   MEM_BLOCK_ALL, nh_index, &ing_nh));

    /* Clear INITIAL_ING_L3_NEXT_HOP entry */
    sal_memset(&initial_ing_nh, 0, sizeof(initial_ing_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN (soc_mem_write(unit, INITIAL_ING_L3_NEXT_HOPm,
                                   MEM_BLOCK_ALL, nh_index, &initial_ing_nh));

    /* Free the next-hop entry. */
    rv = bcm_xgs3_nh_del(unit, _BCM_L3_SHR_WRITE_DISABLE, nh_index);
    return rv;
}

STATIC int
_bcm_tr2_wlan_nh_info_get(int unit, bcm_wlan_port_t *wlan_port, int nh_index)
{
    ing_l3_next_hop_entry_t ing_nh;
    bcm_module_t mod_out, mod_in;
    bcm_port_t port_out, port_in;
    bcm_trunk_t trunk_id;

    /* Read the HW entries */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                      nh_index, &ing_nh));

    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, 
                                                ENTRY_TYPEf) == 0x2) {
        if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
            trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, TGIDf);
            BCM_GPORT_TRUNK_SET(wlan_port->port, trunk_id);
        } else {
            mod_in = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
            port_in = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, PORT_NUMf);

            SOC_IF_ERROR_RETURN
                (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                         mod_in, port_in, &mod_out, &port_out));
            BCM_GPORT_MODPORT_SET(wlan_port->port, mod_out, port_out);
        }
    } else {
        return BCM_E_NOT_FOUND;
    }

    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, DROPf)) {
        wlan_port->flags |= BCM_WLAN_PORT_DROP;
    }
	
    return BCM_E_NONE;
}

STATIC int
_bcm_tr2_wlan_match_get(int unit, bcm_wlan_port_t *wlan_port, int vp)
{
    int rv = BCM_E_NONE;
    uint64 temp;

    if (WLAN_INFO(unit)->port_info[vp].flags & 
         _BCM_WLAN_PORT_MATCH_TUNNEL) {

        wlan_port->flags |= BCM_WLAN_PORT_MATCH_TUNNEL;
        wlan_port->match_tunnel = WLAN_INFO(unit)->port_info[vp].match_tunnel;

    } else if (WLAN_INFO(unit)->port_info[vp].flags & 
               _BCM_WLAN_PORT_MATCH_BSSID_RADIO) {
        wlan_port->flags |= BCM_WLAN_PORT_BSSID_RADIO;

        SAL_MAC_ADDR_TO_UINT64
           (WLAN_INFO(unit)->port_info[vp].match_bssid, temp);
        SAL_MAC_ADDR_FROM_UINT64(wlan_port->bssid, temp);
        wlan_port->radio = WLAN_INFO(unit)->port_info[vp].match_radio;
        wlan_port->match_tunnel = WLAN_INFO(unit)->port_info[vp].match_tunnel;

    } else if (WLAN_INFO(unit)->port_info[vp].flags & 
               _BCM_WLAN_PORT_MATCH_BSSID) {
        wlan_port->flags |= BCM_WLAN_PORT_BSSID;

        SAL_MAC_ADDR_TO_UINT64
           (WLAN_INFO(unit)->port_info[vp].match_bssid, temp);
        SAL_MAC_ADDR_FROM_UINT64(wlan_port->bssid, temp);
        wlan_port->match_tunnel = WLAN_INFO(unit)->port_info[vp].match_tunnel;
    }
    return rv;
}

STATIC int
_bcm_tr2_wlan_match_delete(int unit, int vp)
{
    int rv = BCM_E_NONE;
    egr_vlan_xlate_entry_t vent;
    uint32 tunnel;

    sal_memset(&vent, 0, sizeof(egr_vlan_xlate_entry_t));

    if (WLAN_INFO(unit)->port_info[vp].flags & _BCM_WLAN_PORT_MATCH_BSSID) {
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, ENTRY_TYPEf, 0x6);
        soc_mem_mac_addr_set(unit, EGR_VLAN_XLATEm, &vent, WLAN_SVP__BSSIDf, 
                             WLAN_INFO(unit)->port_info[vp].match_bssid);

    } else if (WLAN_INFO(unit)->port_info[vp].flags & _BCM_WLAN_PORT_MATCH_BSSID_RADIO) {
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, ENTRY_TYPEf, 0x7);
        soc_mem_mac_addr_set(unit, EGR_VLAN_XLATEm, &vent, WLAN_SVP__BSSIDf, 
                             WLAN_INFO(unit)->port_info[vp].match_bssid);
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, WLAN_SVP__RIDf, 
                                        WLAN_INFO(unit)->port_info[vp].match_radio);

    } else if (WLAN_INFO(unit)->port_info[vp].flags & _BCM_WLAN_PORT_MATCH_TUNNEL) {
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, ENTRY_TYPEf, 0x5);
        tunnel = BCM_GPORT_TUNNEL_ID_GET(WLAN_INFO(unit)->port_info[vp].match_tunnel);
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, WLAN_SVP__TUNNEL_IDf, 
                                        tunnel);
    }
    rv = soc_mem_delete(unit, EGR_VLAN_XLATEm, MEM_BLOCK_ALL, &vent);
    return rv;
}

STATIC int
_bcm_tr2_wlan_match_add(int unit, bcm_wlan_port_t *wlan_port, int vp)
{
    int rv = BCM_E_NONE;
    egr_vlan_xlate_entry_t vent;
    uint32 tunnel;
    uint64 temp;

    sal_memset(&vent, 0, sizeof(egr_vlan_xlate_entry_t));
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, VALIDf, 1);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, WLAN_SVP__SVPf, vp);

    if (wlan_port->flags & BCM_WLAN_PORT_BSSID) {
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, ENTRY_TYPEf, 0x6);
        soc_mem_mac_addr_set(unit, EGR_VLAN_XLATEm, &vent, WLAN_SVP__BSSIDf, 
                             wlan_port->bssid);
        tunnel = BCM_GPORT_TUNNEL_ID_GET(wlan_port->match_tunnel);
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, WLAN_SVP__EXP_TUNNEL_IDf, 
                                        tunnel);
        WLAN_INFO(unit)->port_info[vp].flags |= _BCM_WLAN_PORT_MATCH_BSSID;
        SAL_MAC_ADDR_TO_UINT64(wlan_port->bssid, temp);
        SAL_MAC_ADDR_FROM_UINT64
           (WLAN_INFO(unit)->port_info[vp].match_bssid, temp); 
        WLAN_INFO(unit)->port_info[vp].match_tunnel = wlan_port->match_tunnel;

    } else if (wlan_port->flags & BCM_WLAN_PORT_BSSID_RADIO) {
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, ENTRY_TYPEf, 0x7);
        soc_mem_mac_addr_set(unit, EGR_VLAN_XLATEm, &vent, WLAN_SVP__BSSIDf, 
                             wlan_port->bssid);
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, WLAN_SVP__RIDf, 
                                        wlan_port->radio);
        tunnel = BCM_GPORT_TUNNEL_ID_GET(wlan_port->match_tunnel);
        if (tunnel > (1 << soc_mem_field_length(unit, EGR_VLAN_XLATEm, 
                                               WLAN_SVP__EXP_TUNNEL_IDf)) - 1) {
            return BCM_E_PARAM;
        }
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, WLAN_SVP__EXP_TUNNEL_IDf, 
                                        tunnel);
        WLAN_INFO(unit)->port_info[vp].flags |= _BCM_WLAN_PORT_MATCH_BSSID_RADIO;
        SAL_MAC_ADDR_TO_UINT64(wlan_port->bssid, temp);
        SAL_MAC_ADDR_FROM_UINT64
           (WLAN_INFO(unit)->port_info[vp].match_bssid, temp);
        WLAN_INFO(unit)->port_info[vp].match_radio = wlan_port->radio;
        WLAN_INFO(unit)->port_info[vp].match_tunnel = wlan_port->match_tunnel;

    } else if (wlan_port->flags & BCM_WLAN_PORT_MATCH_TUNNEL) {
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, ENTRY_TYPEf, 0x5);
        tunnel = BCM_GPORT_TUNNEL_ID_GET(wlan_port->match_tunnel);
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, WLAN_SVP__TUNNEL_IDf, 
                                        tunnel);
        WLAN_INFO(unit)->port_info[vp].flags |= _BCM_WLAN_PORT_MATCH_TUNNEL;
        WLAN_INFO(unit)->port_info[vp].match_tunnel = wlan_port->match_tunnel;
    }
    rv = soc_mem_insert(unit, EGR_VLAN_XLATEm, MEM_BLOCK_ALL, &vent);
    return rv;
}

/*
 * Function:
 *      bcm_wlan_port_add
 * Purpose:
 *      Create or replace a WLAN port
 * Parameters:
 *      unit  - (IN)  Device Number
 *      wlan_port  - (IN/OUT) WLAN port configuration info
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      For AC-to-AC VPs, the match_tunnel and egress_tunnel must have the same
 *      ID. That way we can use a single VP for both directions. This constrains
 *      the application to use the lower half of the tunnel initiators.
 */
int 
bcm_tr2_wlan_port_add(int unit, bcm_wlan_port_t *wlan_port)
{
    int drop, mode, is_local = 0, rv = BCM_E_PARAM, nh_index = 0;
    bcm_port_t local_port;
    bcm_module_t my_modid;
    int vp, num_vp, lport_ptr = -1;
    int tpid_enable = 0, tpid_index;
    wlan_svp_table_entry_t svp;
    ing_dvp_table_entry_t dvp;
    egr_wlan_dvp_entry_t wlan_dvp;
    lport_tab_entry_t lport_profile;
    rtag7_port_based_hash_entry_t rtag7_entry;
    void *entries[2];
    uint32 tunnel;
    int cml_default_enable=0, cml_default_new=0, cml_default_move=0;
    _bcm_vp_info_t vp_info;

    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeWlan;

    WLAN_INIT(unit);

    rv = bcm_xgs3_l3_egress_mode_get(unit, &mode);
    BCM_IF_ERROR_RETURN(rv);
    if (!mode) {
        LOG_INFO(BSL_LS_BCM_L3,
                 (BSL_META_U(unit,
                             "L3 egress mode must be set first\n")));
        return BCM_E_DISABLED;
    }

    if (!BCM_GPORT_IS_TUNNEL(wlan_port->match_tunnel)) {
        return BCM_E_BADID;
    }

    WLAN_LOCK(unit);
    /* Allocate a new VP or use provided one */
    if (wlan_port->flags & BCM_WLAN_PORT_WITH_ID) {
        if (!BCM_GPORT_IS_WLAN_PORT((wlan_port->wlan_port_id))) {
            WLAN_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        vp = BCM_GPORT_WLAN_PORT_ID_GET(wlan_port->wlan_port_id);
        if (vp >= soc_mem_index_count(unit, WLAN_SVP_TABLEm)) {
            WLAN_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
            if (!(wlan_port->flags & BCM_WLAN_PORT_REPLACE)) {
                WLAN_UNLOCK(unit);
                return BCM_E_EXISTS;
            }
        } else {
            rv = _bcm_vp_used_set(unit, vp, vp_info);
            if (rv < 0) {
                WLAN_UNLOCK(unit);
                return rv;
            }
        }
    } else {
        /* allocate a new VP index */
        num_vp = soc_mem_index_count(unit, WLAN_SVP_TABLEm);
        rv = _bcm_vp_alloc(unit, 0, (num_vp - 1), 1, WLAN_SVP_TABLEm, 
                           vp_info, &vp);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            return rv;
        }
    }

    /* Program, the next hop, matching and egress tables */
    sal_memset(&lport_profile, 0, sizeof(lport_tab_entry_t));
    sal_memset(&rtag7_entry, 0, sizeof(rtag7_port_based_hash_entry_t));

    if (wlan_port->flags & BCM_WLAN_PORT_REPLACE) {
        /* For existing ports, NH entry already exists */
        rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            return rv;
        }
        nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                                                  NEXT_HOP_INDEXf);
        /* Read the existing SVP entry for potential modifications */
        rv = READ_WLAN_SVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &svp);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            return rv;
        }
    } else {
        sal_memset(&svp, 0, sizeof(wlan_svp_table_entry_t));
        sal_memset(&dvp, 0, sizeof(ing_dvp_table_entry_t));
        sal_memset(&wlan_dvp, 0, sizeof(egr_wlan_dvp_entry_t));
        /* Allocate a default profile entry for SVPs */
        rv = _bcm_fb2_outer_tpid_entry_add(unit, 0x8100, &tpid_index);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            goto port_cleanup;
        }
        tpid_enable = (1 << tpid_index);
        soc_LPORT_TABm_field32_set(unit, &lport_profile, PORT_VIDf, 0x1);
        soc_LPORT_TABm_field32_set(unit, &lport_profile,  
                                   TRUST_INCOMING_VIDf, 0x1);
        soc_LPORT_TABm_field32_set(unit, &lport_profile,  
                                   TRUST_OUTER_DOT1Pf, 0x1);
        soc_LPORT_TABm_field32_set(unit, &lport_profile,  
                                   OUTER_TPID_ENABLEf, tpid_enable);
        soc_LPORT_TABm_field32_set(unit, &lport_profile,  
                                   MAC_BASED_VID_ENABLEf, 0x1);
        if (!(wlan_port->flags & BCM_WLAN_PORT_NETWORK)) {
            soc_LPORT_TABm_field32_set(unit, &lport_profile,  
                                       PORT_BRIDGEf, 0x1);
        }
        soc_LPORT_TABm_field32_set(unit, &lport_profile, 
                                   SUBNET_BASED_VID_ENABLEf, 0x1);
        /* Set the CML */
        rv = _bcm_vp_default_cml_mode_get (unit, 
                           &cml_default_enable, &cml_default_new, 
                           &cml_default_move);
         if (rv < 0) {
            WLAN_UNLOCK(unit);
            goto port_cleanup;
         }

        if (cml_default_enable) {
            /* Set the CML to default values */
           soc_LPORT_TABm_field32_set(unit, &lport_profile, CML_FLAGS_NEWf, cml_default_new);
           soc_LPORT_TABm_field32_set(unit, &lport_profile, CML_FLAGS_MOVEf, cml_default_move);
        } else {
            /* Set the CML to PVP_CML_SWITCH by default (hw learn and forward) */
            soc_LPORT_TABm_field32_set(unit, &lport_profile, CML_FLAGS_NEWf, 0x8);
            soc_LPORT_TABm_field32_set(unit, &lport_profile, CML_FLAGS_MOVEf, 0x8);
        }

        soc_LPORT_TABm_field32_set(unit, &lport_profile, PRI_MAPPINGf, 
                                   0xfac688);
        soc_LPORT_TABm_field32_set(unit, &lport_profile, CFI_0_MAPPINGf, 0);
        soc_LPORT_TABm_field32_set(unit, &lport_profile, CFI_1_MAPPINGf, 1);
        soc_LPORT_TABm_field32_set(unit, &lport_profile, V4L3_ENABLEf, 0x1);
        soc_LPORT_TABm_field32_set(unit, &lport_profile, V6L3_ENABLEf, 0x1);
        soc_LPORT_TABm_field32_set(unit, &lport_profile, V4IPMC_ENABLEf, 0x1);
        soc_LPORT_TABm_field32_set(unit, &lport_profile, V6IPMC_ENABLEf, 0x1);
        soc_LPORT_TABm_field32_set(unit, &lport_profile, IPMC_DO_VLANf, 0x1);
        soc_LPORT_TABm_field32_set(unit, &lport_profile, FILTER_ENABLEf, 0x1);
        soc_LPORT_TABm_field32_set(unit, &lport_profile, VFP_ENABLEf, 0x1);
        /* Allocate a fixed high index for the PORT_FIELD_SEL */
        soc_LPORT_TABm_field32_set(unit, &lport_profile, 
                                   FP_PORT_FIELD_SEL_INDEXf, 
                                   soc_mem_index_max(unit, FP_PORT_FIELD_SELm));
        /* The vt_key_types must be set to 0x3 or 0x7 for WLAN */
        soc_LPORT_TABm_field32_set(unit, &lport_profile, VT_KEY_TYPEf,
                                   TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC);
        soc_LPORT_TABm_field32_set(unit, &lport_profile, VT_KEY_TYPE_2f,
                                   TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC); 
        /* Program MY_MODID for the LPORT entry */
        rv = bcm_esw_stk_my_modid_get(unit, &my_modid);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            goto port_cleanup;
        }
        soc_LPORT_TABm_field32_set(unit, &lport_profile, MY_MODIDf, my_modid);
        entries[0] = &lport_profile;
        entries[1] = &rtag7_entry;
        rv = _bcm_lport_profile_entry_add(unit, entries, 1, 
                                          (uint32 *) &lport_ptr);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            goto port_cleanup;
        }
        soc_WLAN_SVP_TABLEm_field32_set(unit, &svp, LPORT_PROFILE_IDXf, 
                                        lport_ptr);
    }

    /* Program next hop entry using existing or new index */
    drop = (wlan_port->flags & BCM_WLAN_PORT_DROP) ? 1 : 0;
    rv = _bcm_tr2_wlan_nh_info_add(unit, wlan_port, vp, drop,
                                   &nh_index, &local_port, &is_local);
    if (rv < 0) {
        WLAN_UNLOCK(unit);
        goto port_cleanup;
    }

    /* Program the VP tables */
    if (wlan_port->flags & BCM_WLAN_PORT_NETWORK) {
        soc_WLAN_SVP_TABLEm_field32_set(unit, &svp, SH_PRUNE_ENABLEf, 1);
        soc_EGR_WLAN_DVPm_field32_set(unit, &wlan_dvp, SH_PRUNE_ENABLEf, 1);
    }

    /* Select HG2 PPD2 header for remote egress */
    soc_EGR_WLAN_DVPm_field32_set(unit, &wlan_dvp, HG_HDR_SELf, 1);
    
    /* Program the matching criteria */
    if (wlan_port->flags & BCM_WLAN_PORT_REPLACE) {
        rv = WRITE_WLAN_SVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &svp);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            goto port_cleanup;
        }
    } else {
        /* Link in the newly allocated next-hop entry */
        soc_ING_DVP_TABLEm_field32_set(unit, &dvp, NEXT_HOP_INDEXf,
                                       nh_index);
        rv = WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            goto port_cleanup;
        }

        if ((wlan_port->flags & BCM_WLAN_PORT_BSSID) || 
            (wlan_port->flags & BCM_WLAN_PORT_BSSID_RADIO)) {
            soc_mem_mac_addr_set(unit, EGR_WLAN_DVPm, &wlan_dvp, BSSIDf, 
                                 wlan_port->bssid);
            if ((wlan_port->flags & BCM_WLAN_PORT_EGRESS_BSSID)) {
                soc_EGR_WLAN_DVPm_field32_set(unit, &wlan_dvp, 
                                              CAPWAP_BSSID_ENf, 1);
            }
        }
        if (wlan_port->flags & BCM_WLAN_PORT_BSSID_RADIO) {
            if (wlan_port->radio > 
                (1 << soc_mem_field_length(unit, EGR_WLAN_DVPm, RIDf)) - 1) {
                WLAN_UNLOCK(unit);
                rv = BCM_E_PARAM;
                goto port_cleanup;
            } 
            soc_EGR_WLAN_DVPm_field32_set(unit, &wlan_dvp, RIDf, 
                                          wlan_port->radio);
        }
        if (!BCM_GPORT_IS_TUNNEL(wlan_port->egress_tunnel)) {
            WLAN_UNLOCK(unit);
            rv = BCM_E_BADID;
            goto port_cleanup;
        }
        tunnel = BCM_GPORT_TUNNEL_ID_GET(wlan_port->egress_tunnel);
        /* Program the tunnel VLAN from the cached state */
        soc_EGR_WLAN_DVPm_field32_set(unit, &wlan_dvp, TUNNEL_VLAN_IDf, 
                                      TUNNEL_VLAN(unit, tunnel));
        if (_BCM_WLAN_IP_TNL_IS_V6_GET(unit, tunnel)) {
            tunnel >>= 1;
        }
        soc_EGR_WLAN_DVPm_field32_set(unit, &wlan_dvp, TUNNEL_INDEXf, tunnel); 
        rv = WRITE_EGR_WLAN_DVPm(unit, MEM_BLOCK_ALL, vp, &wlan_dvp);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            goto port_cleanup;
        }

        /* Initialize the SVP parameters */
        soc_WLAN_SVP_TABLEm_field32_set(unit, &svp, VLAN_IDf, 0x1);
        soc_WLAN_SVP_TABLEm_field32_set(unit, &svp, VALIDf, 0x1);
        rv = WRITE_WLAN_SVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &svp);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            goto port_cleanup;
        }

        /*
         * Match entries cannot be replaced, instead, callers
         * need to delete the existing entry and re-add with the
         * new match parameters.
         */
        rv = _bcm_tr2_wlan_match_add(unit, wlan_port, vp);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            goto port_cleanup;
        } else {
            /* Set the WLAN port ID */
            BCM_GPORT_WLAN_PORT_ID_SET(wlan_port->wlan_port_id, vp);
            wlan_port->encap_id = nh_index;
        }
    }
    WLAN_UNLOCK(unit);
port_cleanup:
    if (rv < 0) {
        if (tpid_enable) {
            (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
        }
        if (!(wlan_port->flags & BCM_WLAN_PORT_REPLACE)) {
            (void) _bcm_vp_free(unit, _bcmVpTypeWlan, 1, vp);
            _bcm_tr2_wlan_nh_info_delete(unit, nh_index);
        }
        if (lport_ptr != -1) {
            (void) _bcm_lport_profile_entry_delete(unit, lport_ptr);
        }
    }
    return rv;
}

STATIC int
_bcm_tr2_wlan_port_delete(int unit, int vp)
{
    int rv = BCM_E_NONE;
    int nh_index = 0;
    int lport_ptr = -1;
    int is_local, i, tpid_enable = 0;
    wlan_svp_table_entry_t svp;
    ing_dvp_table_entry_t dvp;
    egr_wlan_dvp_entry_t wlan_dvp;
    lport_tab_entry_t lport_profile;
    rtag7_port_based_hash_entry_t rtag7_entry;
    _bcm_port_info_t *info;
    void *entries[2];

    rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp);
    BCM_IF_ERROR_RETURN(rv);
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);

    rv = _bcm_tr2_wlan_match_delete(unit, vp);
    BCM_IF_ERROR_RETURN(rv);

    /* Delete LPORT profile and its associated TPID entry */
    rv = READ_WLAN_SVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &svp);
    BCM_IF_ERROR_RETURN(rv);
    lport_ptr = soc_WLAN_SVP_TABLEm_field32_get(unit, &svp, LPORT_PROFILE_IDXf);
    entries[0] = &lport_profile;
    entries[1] = &rtag7_entry;
    rv = _bcm_lport_profile_entry_get(unit, lport_ptr, 1, entries);
    BCM_IF_ERROR_RETURN(rv);
    tpid_enable = soc_LPORT_TABm_field32_get(unit, &lport_profile, 
                                             OUTER_TPID_ENABLEf);
    if (tpid_enable) {
        for (i = 0; i < 4; i++) {
            if (tpid_enable & (1 << i)) {
                (void) _bcm_fb2_outer_tpid_entry_delete(unit, i);
                break;
            }
        }
    }
    rv = _bcm_lport_profile_entry_delete(unit, lport_ptr);
    BCM_IF_ERROR_RETURN(rv);

    /* Clear the SVP and DVP table entries */
    sal_memset(&svp, 0, sizeof(wlan_svp_table_entry_t));
    rv = WRITE_WLAN_SVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &svp);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(&dvp, 0, sizeof(ing_dvp_table_entry_t));
    rv = WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(&wlan_dvp, 0, sizeof(egr_wlan_dvp_entry_t));
    rv = WRITE_EGR_WLAN_DVPm(unit, MEM_BLOCK_ALL, vp, &wlan_dvp);
    BCM_IF_ERROR_RETURN(rv);

    /* Clear the next-hop table entries */
    rv = _bcm_tr2_wlan_nh_info_delete(unit, nh_index);
    BCM_IF_ERROR_RETURN(rv);

    /* Update the physical port's SW state */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_modid_is_local(unit, WLAN_INFO(unit)->port_info[vp].modid, 
                                &is_local));
    if (is_local && (WLAN_INFO(unit)->port_info[vp].tgid == -1)) {
        bcm_port_t phys_port = WLAN_INFO(unit)->port_info[vp].port; 
        if (soc_feature(unit, soc_feature_sysport_remap)) { 
            BCM_XLATE_SYSPORT_S2P(unit, &phys_port); 
        }
        _bcm_port_info_access(unit, phys_port, &info);
        info->vp_count--;
    }

    /* If associated with a trunk, update each local physical port's SW state */
    if (WLAN_INFO(unit)->port_info[vp].tgid != -1) {
        int idx;
        bcm_port_t local_member_array[SOC_MAX_NUM_PORTS];
       int local_member_count;

        bcm_trunk_t trunk_id = WLAN_INFO(unit)->port_info[vp].tgid;

        BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, trunk_id,
                    SOC_MAX_NUM_PORTS, local_member_array, &local_member_count));
        for (idx = 0; idx < local_member_count; idx++) {
            _bcm_port_info_access(unit, local_member_array[idx], &info);
            info->vp_count--;
        }
    }

    /* Clear the SW state */
    sal_memset(&(WLAN_INFO(unit)->port_info[vp]), 0, 
               sizeof(_bcm_tr2_wlan_port_info_t));

    /* Release Service counter, if any */
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        bcm_gport_t gport;

        BCM_GPORT_WLAN_PORT_ID_SET(gport, vp);
        _bcm_esw_flex_stat_handle_free(unit, _bcmFlexStatTypeGport, gport);
    }

    /* Free the VP */
    (void) _bcm_vp_free(unit, _bcmVpTypeWlan, 1, vp);
    return rv;
}

/*
 * Function:
 *      bcm_wlan_port_delete
 * Purpose:
 *      Delete a wlan port
 * Parameters:
 *      unit       - (IN) Device Number
 *      wlan_port_id - (IN) wlan port information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_wlan_port_delete(int unit, bcm_gport_t wlan_port_id)
{
    int vp, rv;

    WLAN_INIT(unit);

    vp = BCM_GPORT_WLAN_PORT_ID_GET(wlan_port_id);
    if (vp == -1) {
        return BCM_E_PARAM;
    }

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
        return BCM_E_NOT_FOUND;
    }
    WLAN_LOCK(unit);
    rv = _bcm_tr2_wlan_port_delete(unit, vp);
    WLAN_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_wlan_port_delete_all
 * Purpose:
 *      Delete all wlan ports
 * Parameters:
 *      unit       - (IN) Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_wlan_port_delete_all(int unit)
{
    int rv = BCM_E_NONE;
    uint32 vp, num_vp;
    source_vp_entry_t svp;

    WLAN_INIT(unit);

    num_vp = soc_mem_index_count(unit, WLAN_SVP_TABLEm);
    for (vp = 0; vp < num_vp; vp++) {
        rv = READ_WLAN_SVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &svp);
        if (rv < 0) {
            goto done;
        }
        if (soc_WLAN_SVP_TABLEm_field32_get(unit, &svp, VALIDf) != 0) {
            WLAN_LOCK(unit);
            rv = _bcm_tr2_wlan_port_delete(unit, vp);
            WLAN_UNLOCK(unit);
            if (rv < 0) {
                goto done;
            }
        }
    }
done:
    return rv;
}

STATIC int
_bcm_tr2_wlan_port_get(int unit, int vp, bcm_wlan_port_t *wlan_port)
{
    int egress_tunnel, nh_index, rv = BCM_E_NONE;
    ing_dvp_table_entry_t dvp;
    egr_wlan_dvp_entry_t wlan_dvp;

    /* Initialize the structure */
    bcm_wlan_port_t_init(wlan_port);
    BCM_GPORT_WLAN_PORT_ID_SET(wlan_port->wlan_port_id, vp);

    BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);

    /* Fill the encap ID */
    wlan_port->encap_id = nh_index + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);

    /* Get the match parameters */
    rv = _bcm_tr2_wlan_match_get(unit, wlan_port, vp);
    BCM_IF_ERROR_RETURN(rv);

    /* Get the next-hop parameters */
    rv = _bcm_tr2_wlan_nh_info_get(unit, wlan_port, nh_index);
    BCM_IF_ERROR_RETURN(rv);

    /* Fill in egress parameters */
    BCM_IF_ERROR_RETURN(READ_EGR_WLAN_DVPm(unit, MEM_BLOCK_ANY, vp, &wlan_dvp));
    egress_tunnel = soc_EGR_WLAN_DVPm_field32_get(unit, &wlan_dvp, 
                                                  TUNNEL_INDEXf);
    BCM_GPORT_TUNNEL_ID_SET(wlan_port->egress_tunnel, egress_tunnel);
    if (soc_EGR_WLAN_DVPm_field32_get(unit, &wlan_dvp, SH_PRUNE_ENABLEf)) {
        wlan_port->flags |= BCM_WLAN_PORT_NETWORK;
    }
    if (soc_EGR_WLAN_DVPm_field32_get(unit, &wlan_dvp, CAPWAP_BSSID_ENf)) {
        wlan_port->flags |= BCM_WLAN_PORT_EGRESS_BSSID;
    }
    return rv;
}

/*
 * Function:
 *      bcm_wlan_port_get
 * Purpose:
 *      Get a wlan port
 * Parameters:
 *      unit       - (IN) Device Number
 *      wlan_port  - (IN/OUT) wlan port information
 */
int
bcm_tr2_wlan_port_get(int unit, bcm_gport_t wlan_port_id, bcm_wlan_port_t *wlan_port)
{
    int vp;

    WLAN_INIT(unit);

    vp = BCM_GPORT_WLAN_PORT_ID_GET(wlan_port_id);
    if (vp == -1) {
        return BCM_E_PARAM;
    }

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
        return BCM_E_NOT_FOUND;
    }
    return _bcm_tr2_wlan_port_get(unit, vp, wlan_port);
}

/*
 * Function:
 *      bcm_wlan_port_traverse
 * Purpose:
 *      Walks through the valid WLAN port entries and calls
 *      the user supplied callback function for each entry.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      trav_fn    - (IN) Callback function.
 *      user_data  - (IN) User data to be passed to callback function.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_tr2_wlan_port_traverse(int unit,
                           bcm_wlan_port_traverse_cb cb,
                           void *user_data)
{
    int i, index_min, index_max, rv = BCM_E_NONE;
    int valid, wlan_port_id, *buffer = NULL;
    wlan_svp_table_entry_t *hw_entry;
    bcm_wlan_port_t info;

    WLAN_INIT(unit);

    index_min = soc_mem_index_min(unit, WLAN_SVP_TABLEm);
    index_max = soc_mem_index_max(unit, WLAN_SVP_TABLEm);

    WLAN_LOCK(unit);
    buffer = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, WLAN_SVP_TABLEm),
                           "wlan port traverse");
    if (NULL == buffer) {
        WLAN_UNLOCK(unit);
        return BCM_E_MEMORY;
    }
    if ((rv = soc_mem_read_range(unit, WLAN_SVP_TABLEm, MEM_BLOCK_ALL,
                                 index_min, index_max, buffer)) < 0 ) {
        soc_cm_sfree(unit, buffer);
        WLAN_UNLOCK(unit);
        return rv;
    }
    for (i = index_min; i <= index_max; i++) {
        hw_entry = soc_mem_table_idx_to_pointer(unit, WLAN_SVP_TABLEm,
                                         wlan_svp_table_entry_t *, buffer, i);
        valid = soc_WLAN_SVP_TABLEm_field32_get(unit, hw_entry, VALIDf);
        if (!valid) {
            continue;
        }
        bcm_wlan_port_t_init(&info);
        BCM_GPORT_WLAN_PORT_ID_SET(wlan_port_id, i);
        rv = bcm_tr2_wlan_port_get(unit, wlan_port_id, &info);
        if (BCM_FAILURE(rv)) {
            soc_cm_sfree(unit, buffer);
            WLAN_UNLOCK(unit);
            return rv;
        }
        rv = cb(unit, &info, user_data);
        if (BCM_FAILURE(rv)) {
            soc_cm_sfree(unit, buffer);
            WLAN_UNLOCK(unit); 
            return rv;
        }
    }
    soc_cm_sfree(unit, buffer);
    WLAN_UNLOCK(unit);
    return rv;
}


int
_bcm_esw_wlan_flex_stat_index_set(int unit, bcm_gport_t port, int fs_idx,uint32 flags)
{
    int rv, nh_index;
    ing_dvp_table_entry_t dvp;
    uint32 vp;

    vp = BCM_GPORT_WLAN_PORT_ID_GET(port);

    WLAN_LOCK(unit);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
        WLAN_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    } else {
        rv = BCM_E_NONE;
        /* Ingress side */
        if (flags & _BCM_FLEX_STAT_HW_INGRESS) {
            rv = soc_mem_field32_modify(unit, SOURCE_VPm, vp, VINTF_CTR_IDXf, 
                                    fs_idx);
        }
        if (flags & _BCM_FLEX_STAT_HW_EGRESS) {
            if (BCM_SUCCESS(rv)) {
                rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp);
                nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, 
                                                NEXT_HOP_INDEXf);
                /* Egress side */
                if (BCM_SUCCESS(rv)) {
                    rv = soc_mem_field32_modify(unit, EGR_L3_NEXT_HOPm, 
                                            nh_index,
                                            VINTF_CTR_IDXf, fs_idx);
                }
            }
        }
    }

    WLAN_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_tr2_wlan_tunnel_init_add
 * Purpose:
 *      Add tunnel initiator entry to hw. 
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      idx      - (IN)Index to insert tunnel initiator.
 *      info     - (IN)Tunnel initiator entry info.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr2_wlan_tunnel_init_add(int unit, int idx, bcm_tunnel_initiator_t *info)
{
    uint32 tnl_entry[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to write hw entry */
    soc_mem_t mem;                        /* Tunnel initiator table memory */  
    int df_val;                           /* Don't fragment encoding       */
    int ipv6;                             /* IPv6 tunnel initiator         */
    int entry_type = 0;                   /* HW entry type IPv4/IPv6/MPLS  */
    int hw_type_code = 0;                 /* Tunnel type.                  */
    int old_capwap_frag_profile = 0;      /* Old profile pointer           */
    int capwap_frag_profile = -1;         /* New profile pointer           */
    int old_tpid_index = -1;              /* Old TPID index                */
    int tpid_index = -1;                  /* TPID index                    */
    int rv = BCM_E_NONE;                  /* Return value                  */
    uint64 rval64, *rval64s[1];           /* For the profile register      */
    uint32 rval = 0;                      /* Register value                */

    ipv6 = _BCM_TUNNEL_OUTER_HEADER_IPV6(info->type);

    /* Tunnel VLAN needs to be programmed in the EGR_WLAN_DVP table. 
     * We need to cache it so that the bcm_wlan_port_add API can
     * program it in the correct location. It is always cached against
     * the index to EGR_IP_TUNNEL for both V4 and V6 for simplicity. */
    TUNNEL_VLAN(unit, idx) = info->vlan;

    /* Get table memory. */
    if (ipv6) {
        idx >>= 1;
        mem = EGR_IP_TUNNEL_IPV6m;
    } else {
        mem = EGR_IP_TUNNELm; 
    }

    /* Zero write buffer. */
    sal_memset(tnl_entry, 0, sizeof(tnl_entry));

    /* If replacing existing entry, first read the entry to get old 
       profile pointer and TPID */
    if (info->flags & BCM_TUNNEL_REPLACE) {
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx, tnl_entry);
        BCM_IF_ERROR_RETURN(rv);
        old_capwap_frag_profile = soc_mem_field32_get(unit, mem, tnl_entry, 
                                                      CAPWAP_MTU_SELf);
        old_tpid_index = soc_mem_field32_get(unit, mem, tnl_entry, 
                                             TUNNEL_TPID_INDEXf);
        if (!(info->flags & BCM_TUNNEL_INIT_WLAN_VLAN_TAGGED)) {
            soc_mem_field32_set(unit, mem, tnl_entry, VLAN_ASSIGN_POLICYf, 0);
        }
    }

    /* Set destination address. */
    soc_mem_field_set(unit, mem, tnl_entry, DIPf, 
                ((ipv6) ? (uint32 *)info->dip6 : (uint32 *)&info->dip));

    /* Set source address. */
    soc_mem_field_set(unit, mem, tnl_entry, SIPf,
                ((ipv6) ? (uint32 *)info->sip6 : (uint32 *)&info->sip));

    if (!ipv6) {
        /* IP tunnel hdr DF bit for IPv4. */
        df_val = 0;
        if (info->flags & BCM_TUNNEL_INIT_USE_INNER_DF) {
            df_val |= 0x2;
        } else if (info->flags & BCM_TUNNEL_INIT_IPV4_SET_DF) {
            df_val |= 0x1;
        }
        soc_mem_field32_set(unit, mem, tnl_entry, IPV4_DF_SELf, df_val);
    }

    if (ipv6) {
        /* IP tunnel hdr DF bit for IPv6. */
        df_val = (info->flags & BCM_TUNNEL_INIT_IPV6_SET_DF) ? 0x1 : 0;
        soc_mem_field32_set(unit, mem, tnl_entry, IPV6_DF_SELf, df_val);
    }

    /* Set DSCP value.  */
    soc_mem_field32_set(unit, mem, tnl_entry, DSCPf, info->dscp);

    /* Tunnel header DSCP select. */
    soc_mem_field32_set(unit, mem, tnl_entry, DSCP_SELf, info->dscp_sel);

    /* Set TTL. */
    soc_mem_field32_set(unit, mem, tnl_entry, TTLf, info->ttl);

    /* Set tunnel type. */
    BCM_IF_ERROR_RETURN (_bcm_trx_tnl_type_to_hw_code(unit, info->type,
                                                      &hw_type_code,
                                                      &entry_type));

    soc_mem_field32_set(unit, mem, tnl_entry, TUNNEL_TYPEf, hw_type_code);

    /* Set flow label. */
    if (ipv6) {
        soc_mem_field32_set(unit, mem, tnl_entry, FLOW_LABELf,
                            info->flow_label);
    }

    /* Set entry type. */
    soc_mem_field32_set(unit, mem, tnl_entry, ENTRY_TYPEf, entry_type);

    /* Set destination mac address. */
    soc_mem_mac_addr_set(unit, mem, &tnl_entry, DEST_ADDRf, info->dmac);

    /* L4 fields */
    soc_mem_field32_set(unit, mem, tnl_entry, L4_SRC_PORTf,
                        info->udp_src_port);
    soc_mem_field32_set(unit, mem, tnl_entry, L4_DEST_PORTf,
                        info->udp_dst_port);

    /* L2 fields */
    soc_mem_mac_addr_set(unit, mem, &tnl_entry, MACSAf, info->smac);
    if (info->flags & BCM_TUNNEL_INIT_WLAN_VLAN_TAGGED) {
        soc_mem_field32_set(unit, mem, tnl_entry, VLAN_ASSIGN_POLICYf, 1);
        soc_mem_field32_set(unit, mem, tnl_entry, NEW_PRIf, info->pkt_pri);
        soc_mem_field32_set(unit, mem, tnl_entry, NEW_CFIf, info->pkt_cfi);
        rv = _bcm_fb2_outer_tpid_entry_add(unit, info->tpid, &tpid_index);
        soc_mem_field32_set(unit, mem, tnl_entry, TUNNEL_TPID_INDEXf, 
                            tpid_index);
        if (BCM_FAILURE(rv)) {
            goto tnl_cleanup;
        }
    }

    /* CAPWAP MTU */
    if (info->flags & BCM_TUNNEL_INIT_WLAN_MTU && info->mtu > 0) {
        uint32 first = (((info->mtu - 128) / 128) > 15) ? 15 : 
                        ((info->mtu - 128) / 128);
        soc_reg_field_set(unit, EGR_CAPWAP_FRAG_CONTROLr, &rval, MTUf, 
                          info->mtu);
        soc_reg_field_set(unit, EGR_CAPWAP_FRAG_CONTROLr, &rval,  
                          FIRST_FRAG_SIZEf, first);
        COMPILER_64_ZERO(rval64);
        COMPILER_64_SET(rval64, 0, rval);
        rval64s[0] = &rval64;
        rv = soc_profile_reg_add(unit, CAPWAP_FRAG_PROFILE(unit), rval64s, 1, 
                                 (uint32 *)&capwap_frag_profile);
        if (BCM_FAILURE(rv)) {
            goto tnl_cleanup;
        }
        soc_mem_field32_set(unit, mem, tnl_entry, CAPWAP_MTU_SELf,
                            capwap_frag_profile);
    }

    /* Write buffer to hw. */
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx, tnl_entry);

    if (BCM_FAILURE(rv)) {
        goto tnl_cleanup;
    }
    if (old_tpid_index != -1) {
        (void)_bcm_fb2_outer_tpid_entry_delete(unit, old_tpid_index);
    }
    if (old_capwap_frag_profile != 0) {
        rv = soc_profile_reg_delete(unit, CAPWAP_FRAG_PROFILE(unit), 
                                    old_capwap_frag_profile);
    }
tnl_cleanup:
    if (BCM_FAILURE(rv)) {
        if (tpid_index != -1) {
            (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
        }
        if (capwap_frag_profile != -1) {
            (void)soc_profile_reg_delete(unit, CAPWAP_FRAG_PROFILE(unit), 
                                        capwap_frag_profile);
        }
    }
    return rv;
}

/*
 * Function:
 *      bcm_wlan_tunnel_initiator_create
 * Purpose:
 *      Create an outgong CAPWAP tunnel
 * Parameters:
 *      unit    - (IN) Device Number
 *      info    - (IN/OUT) Tunnel initiator structure
 */
int
bcm_tr2_wlan_tunnel_initiator_create(int unit, bcm_tunnel_initiator_t *info)
{
    int tnl_idx, rv = BCM_E_NONE;
    uint32 flags = 0;

    WLAN_INIT(unit);

    /* Input parameters check. */
    if (NULL == info) {
        return (BCM_E_PARAM);
    }   
    if ((info->type != bcmTunnelTypeWlanWtpToAc) && 
        (info->type != bcmTunnelTypeWlanAcToAc) && 
        (info->type != bcmTunnelTypeWlanWtpToAc6) && 
        (info->type != bcmTunnelTypeWlanAcToAc6)) {
        return BCM_E_PARAM;
    }
    if (!BCM_TTL_VALID(info->ttl)) {
        return BCM_E_PARAM;
    }
    if (info->dscp_sel > 2 || info->dscp_sel < 0) {
        return BCM_E_PARAM;
    }
    if (info->dscp > 63 || info->dscp < 0) {
        return BCM_E_PARAM;
    }
    if (_BCM_TUNNEL_OUTER_HEADER_IPV6(info->type)) {
        if (info->flow_label > (1 << 20)) {
            return BCM_E_PARAM;
        }
    }
    if (!BCM_VLAN_VALID(info->vlan)) {
        return BCM_E_PARAM;
    }
    if (BCM_MAC_IS_MCAST(info->smac) || BCM_MAC_IS_ZERO(info->smac)) {
        return BCM_E_PARAM;
    }
    if (info->flags & BCM_TUNNEL_INIT_WLAN_TUNNEL_WITH_ID) {
        if (!BCM_GPORT_IS_TUNNEL(info->tunnel_id)) {
            return BCM_E_PARAM;
        }
        flags |= _BCM_L3_SHR_WITH_ID;
        tnl_idx = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
        if (info->flags & BCM_TUNNEL_REPLACE) {
            if (!_BCM_WLAN_IP_TNL_USED_GET(unit, tnl_idx)) {
                return BCM_E_PARAM;
            }
            flags |= _BCM_L3_SHR_UPDATE;
        }
    }
    if (info->flags & BCM_TUNNEL_INIT_WLAN_MTU) {
        if (info->mtu < 0) {
            return BCM_E_PARAM;
        }
    }
    
    WLAN_LOCK(unit);
    /* Allocate either existing or new tunnel initiator entry */
    flags |= _BCM_L3_SHR_MATCH_DISABLE | _BCM_L3_SHR_WRITE_DISABLE |
             _BCM_L3_SHR_SKIP_INDEX_ZERO;
    rv = bcm_xgs3_tnl_init_add(unit, flags, info, &tnl_idx);
    if (BCM_FAILURE(rv)) {
        WLAN_UNLOCK(unit);
        return rv;
    }
    if (!(info->flags & BCM_TUNNEL_INIT_WLAN_TUNNEL_WITH_ID)) {
        BCM_GPORT_TUNNEL_ID_SET(info->tunnel_id, tnl_idx);
    }

    /* Write the entry to HW */
    rv = _bcm_tr2_wlan_tunnel_init_add(unit, tnl_idx, info);
    if (BCM_FAILURE(rv)) {
        flags = _BCM_L3_SHR_WRITE_DISABLE;
        (void) bcm_xgs3_tnl_init_del(unit, flags, tnl_idx);
        WLAN_UNLOCK(unit);
        return rv;
    }

    /* Update usage bitmaps appropriately */
    /* tnl_idx is always the index to EGR_IP_TUNNEL */
    /* If writing to EGR_IP_TUNNEL_IPV6, need to divide by 2 */
    _BCM_WLAN_IP_TNL_USED_SET(unit, tnl_idx);
    if (_BCM_TUNNEL_OUTER_HEADER_IPV6(info->type)) {
        _BCM_WLAN_IP_TNL_IS_V6_SET(unit, tnl_idx);
   }
    WLAN_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_tr2_wlan_tunnel_init_get
 * Purpose:
 *      Get a tunnel initiator entry from hw. 
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      idx      - (IN)Index to read tunnel initiator.
 *      info     - (OUT)Tunnel initiator entry info.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr2_wlan_tunnel_init_get(int unit, int idx, bcm_tunnel_initiator_t *info,
                              int *capwap_frag_profile, int *tpid_index)
{
    uint32 tnl_entry[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to read hw entry.  */
    soc_mem_t mem;                         /* Tunnel initiator table memory.*/  
    int df_val;                            /* Don't fragment encoded value. */
    int tnl_type;                          /* Tunnel type.                  */
    uint32 entry_type = BCM_XGS3_TUNNEL_INIT_V4;/* Entry type (IPv4/IPv6/MPLS)*/
    uint32 rval;                           /* Generic register value */

    /* Get table memory. */
    mem = EGR_IP_TUNNELm; 

    /* Initialize the buffer. */
    sal_memset(tnl_entry, 0, sizeof(tnl_entry));

    /* Get tunnel VLAN from cache */
    if (TUNNEL_VLAN(unit, idx) != 0) {
        info->vlan = TUNNEL_VLAN(unit, idx);
    }
    
    /* First read the entry to get the type */
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx, tnl_entry));

    /* Get entry_type. */
    entry_type = soc_mem_field32_get(unit, mem, tnl_entry, ENTRY_TYPEf); 

    /* VLAN tag added or not */
    if (soc_mem_field32_get(unit, mem, tnl_entry, VLAN_ASSIGN_POLICYf)) {
        info->flags |= BCM_TUNNEL_INIT_WLAN_VLAN_TAGGED;
    }

    if (BCM_XGS3_TUNNEL_INIT_V6 == entry_type) {
        mem = EGR_IP_TUNNEL_IPV6m; 
        idx >>= 1;    /* Each record takes two slots. */
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx, tnl_entry)); 
    }

    /* Get profiled data */
    *capwap_frag_profile = soc_mem_field32_get(unit, mem, tnl_entry, 
                                               CAPWAP_MTU_SELf);
    *tpid_index = soc_mem_field32_get(unit, mem, tnl_entry, 
                                      TUNNEL_TPID_INDEXf);

    /* Parse hw buffer. */
    if (BCM_XGS3_TUNNEL_INIT_V4 == entry_type) {
        /* Get destination ip. */
        info->dip = soc_mem_field32_get(unit, mem, tnl_entry, DIPf); 

        /* Get source ip. */
        info->sip = soc_mem_field32_get(unit, mem, tnl_entry, SIPf);
    } else if (BCM_XGS3_TUNNEL_INIT_V6 == entry_type) {
        /* Get destination ip. */
        soc_mem_field_get(unit, mem, tnl_entry, DIPf, (uint32 *)info->dip6);

        /* Get source ip. */
        soc_mem_field_get(unit, mem, tnl_entry, SIPf, (uint32 *)info->sip6);
    }

    /* Tunnel header DSCP select. */
    info->dscp_sel = soc_mem_field32_get(unit, mem, tnl_entry, DSCP_SELf);

    /* Tunnel header DSCP value. */
    info->dscp = soc_mem_field32_get(unit, mem, tnl_entry, DSCPf);

    /* IP tunnel hdr DF bit for IPv4 */
    df_val = soc_mem_field32_get(unit, mem, tnl_entry, IPV4_DF_SELf);
    if (0x2 <= df_val) {
        info->flags |= BCM_TUNNEL_INIT_USE_INNER_DF;  
    } else if (0x1 == df_val) {
        info->flags |= BCM_TUNNEL_INIT_IPV4_SET_DF;  
    }

    /* IP tunnel hdr DF bit for IPv6 */
    if (soc_mem_field32_get(unit, mem, tnl_entry, IPV6_DF_SELf)) {
        info->flags |= BCM_TUNNEL_INIT_IPV6_SET_DF;  
    }

    /* Get TTL. */
    info->ttl = soc_mem_field32_get(unit, mem, tnl_entry, TTLf);

    /* Get tunnel type. */
    tnl_type = soc_mem_field32_get(unit, mem, tnl_entry, TUNNEL_TYPEf);

    /* Translate hw tunnel type into API encoding. */
    BCM_IF_ERROR_RETURN(_bcm_trx_tnl_hw_code_to_type(unit, tnl_type,
                                                     entry_type,
                                                     &info->type));

    /* Get flow label for IPv6 */
    if (BCM_XGS3_TUNNEL_INIT_V6 == entry_type) {
        info->flow_label = 
            soc_mem_field32_get(unit, mem, tnl_entry, FLOW_LABELf);
    }

    /* Get L4 fields */
    info->udp_dst_port = soc_mem_field32_get(unit, mem, tnl_entry, 
                                                 L4_DEST_PORTf);
    info->udp_src_port = soc_mem_field32_get(unit, mem, tnl_entry, 
                                                 L4_SRC_PORTf);

    /* Get mac addresses */
    soc_mem_mac_addr_get(unit, mem, tnl_entry, DEST_ADDRf, info->dmac);
    soc_mem_mac_addr_get(unit, mem, tnl_entry, MACSAf, info->smac);

    /* Get other L2 fields */
    if (info->flags & BCM_TUNNEL_INIT_WLAN_VLAN_TAGGED) {
        info->pkt_pri = soc_mem_field32_get(unit, mem, tnl_entry, NEW_PRIf);
        info->pkt_cfi = soc_mem_field32_get(unit, mem, tnl_entry, NEW_CFIf);
        BCM_IF_ERROR_RETURN(READ_EGR_OUTER_TPIDr(unit, *tpid_index, &rval));
        info->tpid = soc_reg_field_get(unit, EGR_OUTER_TPIDr, rval, TPIDf);    
    }

    /* Get the MTU */
    BCM_IF_ERROR_RETURN
        (READ_EGR_CAPWAP_FRAG_CONTROLr(unit, *capwap_frag_profile, &rval));
    info->mtu = soc_reg_field_get(unit, EGR_CAPWAP_FRAG_CONTROLr, rval, MTUf); 
    info->flags |= BCM_TUNNEL_INIT_WLAN_MTU;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_wlan_tunnel_initiator_destroy
 * Purpose:
 *      Destroy an outgong CAPWAP tunnel
 * Parameters:
 *      unit           - (IN) Device Number
 *      wlan_tunnel_id - (IN) Tunnel ID (Gport)
 */
int
bcm_tr2_wlan_tunnel_initiator_destroy(int unit, bcm_gport_t wlan_tunnel_id) 
{
    int tnl_idx, rv = BCM_E_NONE;
    uint32 flags = 0;
    int capwap_frag_profile;               /* CAPWAP frag profile pointer */
    int tpid_index;                        /* TPID index */
    bcm_tunnel_initiator_t info;           /* Tunnel initiator structure */

    WLAN_INIT(unit);

    /* Input parameters check. */
    if (!BCM_GPORT_IS_TUNNEL(wlan_tunnel_id)) {
        return BCM_E_PARAM;
    }
    tnl_idx = BCM_GPORT_TUNNEL_ID_GET(wlan_tunnel_id);
    if (!_BCM_WLAN_IP_TNL_USED_GET(unit, tnl_idx)) {
        return BCM_E_PARAM;
    }

    bcm_tunnel_initiator_t_init(&info);

    WLAN_LOCK(unit);
    /* Get the entry first */
    rv = _bcm_tr2_wlan_tunnel_init_get(unit, tnl_idx, &info, 
                                       &capwap_frag_profile, &tpid_index);
    if (BCM_FAILURE(rv)) {
        WLAN_UNLOCK(unit);
        return rv;
    }

    /* Destroy the profiles */
    (void)_bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
    if (capwap_frag_profile != 0) {
        rv = soc_profile_reg_delete(unit, CAPWAP_FRAG_PROFILE(unit), 
                                    capwap_frag_profile);
        if (BCM_FAILURE(rv)) {
            WLAN_UNLOCK(unit);
            return rv;
        }
    }

    /* Destroy the tunnel entry */
    (void) bcm_xgs3_tnl_init_del(unit, flags, tnl_idx);

    /* Update usage bitmaps appropriately */
    /* tnl_idx is always the index to EGR_IP_TUNNEL */
    /* If writing to EGR_IP_TUNNEL_IPV6, need to divide by 2 */
    _BCM_WLAN_IP_TNL_USED_CLR(unit, tnl_idx);
    if (_BCM_TUNNEL_OUTER_HEADER_IPV6(info.type)) {
        _BCM_WLAN_IP_TNL_IS_V6_CLR(unit, tnl_idx);
    }
    TUNNEL_VLAN(unit, tnl_idx) = 0;
    WLAN_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_wlan_tunnel_initiator_get
 * Purpose:
 *      Get an outgong CAPWAP tunnel
 * Parameters:
 *      unit    - (IN) Device Number
 *      info    - (IN/OUT) Tunnel initiator structure
 */
int
bcm_tr2_wlan_tunnel_initiator_get(int unit, bcm_tunnel_initiator_t *info)
{
    int tnl_idx, rv = BCM_E_NONE;
    int capwap_frag_profile;               /* CAPWAP frag profile pointer */
    int tpid_index;                        /* TPID index */

    WLAN_INIT(unit);

    /* Input parameters check. */
    if (info == NULL) {
        return BCM_E_PARAM;
    }
    if (!BCM_GPORT_IS_TUNNEL(info->tunnel_id)) {
        return BCM_E_PARAM;
    }
    tnl_idx = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
    if (!_BCM_WLAN_IP_TNL_USED_GET(unit, tnl_idx)) {
        return BCM_E_NOT_FOUND;
    }

    /* Get the entry */
    rv = _bcm_tr2_wlan_tunnel_init_get(unit, tnl_idx, info, 
                                       &capwap_frag_profile, &tpid_index);
    return rv;
}

/*
 * Function:
 *      bcm_tunnel_terminator_vlan_set 
 * Purpose:
 *      Set the allowed VLANs for a WLAN tunnel (AC2AC only)
 * Parameters:
 *      unit    - (IN)SOC unit number. 
 *      tunnel  - (IN)Tunnel ID.
 *      vlan_vec - (IN)VLAN vector
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_tunnel_terminator_vlan_set(int unit, bcm_gport_t tunnel, 
                                   bcm_vlan_vector_t vlan_vec)
{
    int i, tnl_idx, rv = BCM_E_NONE;
    uint32 grp_bmp = 0, grp_bmp_union = 0, count = 0, diff = 0;
    uint32 *buf;
    vlan_tab_entry_t *vtab;    
    bcm_vlan_vector_t vlan_vec_old;
    int num_vp = 0, vp = -1, vvp = -1;
    wlan_svp_table_entry_t svp;

    WLAN_INIT(unit);

    /* Input parameters check. */
    if (!BCM_GPORT_IS_TUNNEL(tunnel)) {
        return BCM_E_PARAM;
    }
    tnl_idx = BCM_GPORT_TUNNEL_ID_GET(tunnel);
    if (!_BCM_WLAN_IP_TNL_USED_GET(unit, tnl_idx)) {
        return BCM_E_PARAM;
    }
    if (BCM_VLAN_VEC_GET(vlan_vec, BCM_VLAN_MAX) || 
        BCM_VLAN_VEC_GET(vlan_vec, BCM_VLAN_MIN)) {
        return BCM_E_PARAM;
    }

    WLAN_LOCK(unit);

    /* First get existing VLAN vector */
    BCM_VLAN_VEC_ZERO(vlan_vec_old);
    rv = bcm_tr2_tunnel_terminator_vlan_get(unit, tunnel, &vlan_vec_old);
    if (BCM_FAILURE(rv)) {
        WLAN_UNLOCK(unit);
        return rv;
    }

    /* Check if old == new and if old vector is empty */
    for (i = BCM_VLAN_MIN; i < BCM_VLAN_MAX; i++) {
        if (BCM_VLAN_VEC_GET(vlan_vec_old, i) != 
            BCM_VLAN_VEC_GET(vlan_vec, i)) {
            diff = 1;
            if (BCM_VLAN_VEC_GET(vlan_vec_old, i)) {
                count++;
                break;
            }
        }
    }
    if (!diff) {
        WLAN_UNLOCK(unit);
        return rv;
    }

    /* Find the SVP corresponding to this tunnel */
    num_vp = soc_mem_index_count(unit, WLAN_SVP_TABLEm);
    for (i = 0; i < num_vp; i++) {
        if ((WLAN_INFO(unit)->port_info[i].match_tunnel == tunnel) && 
            (WLAN_INFO(unit)->port_info[i].flags & 
             _BCM_WLAN_PORT_MATCH_TUNNEL)) {
            /* Found the matching VP  - get the VLAN_VALIDATION_PTR */
            BCM_IF_ERROR_RETURN
                (soc_mem_read(unit, WLAN_SVP_TABLEm, MEM_BLOCK_ANY, i, &svp));
            if (!soc_WLAN_SVP_TABLEm_field32_get(unit, &svp, VALIDf)) {
                WLAN_UNLOCK(unit);
                return BCM_E_INTERNAL;
            }
            vp = i;
            vvp = soc_WLAN_SVP_TABLEm_field32_get(unit, &svp, 
                                                  VLAN_VALIDATION_PTRf);
            break;
        }
    }
    if (vp == -1) {
        WLAN_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    }

    /* DMA the VLAN table */
    buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, VLAN_TABm), 
                        "vlan_table");
    if (buf == NULL) {
        WLAN_UNLOCK(unit);
        return BCM_E_MEMORY;
    }
    soc_mem_lock(unit, VLAN_TABm);
    if ((rv = soc_mem_read_range(unit, VLAN_TABm, MEM_BLOCK_ANY,
                                 BCM_VLAN_MIN, BCM_VLAN_MAX, buf)) < 0) {
        goto vlan_set_end;
    }

    if (count == 0) {
        /* Old vector is empty, allocate a new bit in VLAN_GROUP_BITMAP */
        /* Otherwise use the existing VLAN_VALIDATION_PTR from the SVP */
        for (i = BCM_VLAN_MIN; i < BCM_VLAN_MAX; i++) {
            vtab = soc_mem_table_idx_to_pointer(unit, VLAN_TABm, 
                                                vlan_tab_entry_t *, buf, i);
            if (!soc_mem_field32_get(unit, VLAN_TABm, vtab, VALIDf)) {
                if (BCM_VLAN_VEC_GET(vlan_vec, i)) {
                    rv = BCM_E_NOT_FOUND;
                    goto vlan_set_end;
                } else {
                    continue;                                    
                } 
            }
            grp_bmp = soc_mem_field32_get(unit, VLAN_TABm, vtab, 
                                          VLAN_GROUP_BITMAPf);
            grp_bmp_union |= grp_bmp;
        }
        if (grp_bmp_union == 0xFFFFFFFF) {
            rv = BCM_E_RESOURCE;
            goto vlan_set_end;
        }
        /* Use the first available zero bit - skip vvp == 0 */
        for (i = 1; i < soc_mem_field_length(unit, VLAN_TABm, 
                                             VLAN_GROUP_BITMAPf); i++) {
            if (((grp_bmp_union >> i) & 0x1) == 0) {
                vvp = i;
                break;
            }
        }
    } 
 
    /* Write the VLAN_GROUP_BITMAP for all applicable VLANs */
    for (i = BCM_VLAN_MIN; i < BCM_VLAN_MAX; i++) {
        vtab = soc_mem_table_idx_to_pointer(unit, VLAN_TABm, vlan_tab_entry_t *,
                                            buf, i);
        grp_bmp = soc_mem_field32_get(unit, VLAN_TABm, vtab, 
                                      VLAN_GROUP_BITMAPf);
        if (BCM_VLAN_VEC_GET(vlan_vec, i)) {
            grp_bmp |= (1 << vvp);
            /* Increase use-count for this bit for this VLAN */
            WLAN_INFO(unit)->vlan_grp_bmp_use_cnt[i][vvp]++;
        } else {
            /* Decrease use-count for this bit for this VLAN */
            if (BCM_VLAN_VEC_GET(vlan_vec_old, i)) {
                WLAN_INFO(unit)->vlan_grp_bmp_use_cnt[i][vvp]--;
            }
            if (WLAN_INFO(unit)->vlan_grp_bmp_use_cnt[i][vvp] == 0) {
                grp_bmp &= ~(1 << vvp);
            }
        }
        soc_VLAN_TABm_field32_set(unit, vtab, VLAN_GROUP_BITMAPf, grp_bmp);
    }
    if ((rv = soc_mem_write_range(unit, VLAN_TABm, MEM_BLOCK_ALL,
                                  BCM_VLAN_MIN, BCM_VLAN_MAX, buf)) < 0) {
        goto vlan_set_end;
    }
    soc_WLAN_SVP_TABLEm_field32_set(unit, &svp, VLAN_VALIDATION_PTRf, vvp);
    rv = WRITE_WLAN_SVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &svp);
vlan_set_end:
    soc_mem_unlock(unit, VLAN_TABm);
    WLAN_UNLOCK(unit);
    soc_cm_sfree(unit, buf);
    return rv;
}

/*
 * Function:
 *      bcm_tunnel_terminator_vlan_get 
 * Purpose:
 *      Get the allowed VLANs for a WLAN tunnel (AC2AC only)
 * Parameters:
 *      unit    - (IN)SOC unit number. 
 *      tunnel  - (IN)Tunnel ID.
 *      vlan_vec - (OUT)VLAN vector
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_tunnel_terminator_vlan_get(int unit, bcm_gport_t tunnel, 
                                   bcm_vlan_vector_t *vlan_vec)
{
    int i, tnl_id, rv = BCM_E_NONE;
    uint32 grp_bmp = 0;
    vlan_tab_entry_t *vtab;    
    uint32 *buf;
    int num_vp = 0, vp = -1, vvp = -1;
    wlan_svp_table_entry_t svp;

    WLAN_INIT(unit);

    /* Input parameters check. */
    if (!BCM_GPORT_IS_TUNNEL(tunnel)) {
        return BCM_E_PARAM;
    }
    tnl_id = BCM_GPORT_TUNNEL_ID_GET(tunnel);
    if (!_BCM_WLAN_IP_TNL_USED_GET(unit, tnl_id)) {
        return BCM_E_PARAM;
    }
    if (vlan_vec == NULL) {
        return BCM_E_PARAM;
    }

    /* Search the VP that corresponds to this tunnel */
    num_vp = soc_mem_index_count(unit, WLAN_SVP_TABLEm);
    for (i = 0; i < num_vp; i++) {
        if ((WLAN_INFO(unit)->port_info[i].match_tunnel == tunnel) && 
            (WLAN_INFO(unit)->port_info[i].flags & 
             _BCM_WLAN_PORT_MATCH_TUNNEL)) {
            /* Found the matching VP  - get the VLAN_VALIDATION_PTR */
            BCM_IF_ERROR_RETURN
                (soc_mem_read(unit, WLAN_SVP_TABLEm, MEM_BLOCK_ANY, i, &svp));
            if (!soc_WLAN_SVP_TABLEm_field32_get(unit, &svp, VALIDf)) {
                return BCM_E_INTERNAL;
            }
            vp = i;
            vvp = soc_WLAN_SVP_TABLEm_field32_get(unit, &svp, 
                                                  VLAN_VALIDATION_PTRf);
            break;
        }
    }
    if (vp == -1) {
        return BCM_E_NOT_FOUND;
    }
 
    /* Compare against the VLAN_GROUP_BITMAP */
    buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, VLAN_TABm), 
                        "vlan_table");
    if (buf == NULL) {
        return BCM_E_MEMORY;
    }
    if ((rv = soc_mem_read_range(unit, VLAN_TABm, MEM_BLOCK_ANY,
                                 BCM_VLAN_MIN, BCM_VLAN_MAX, buf)) < 0) {
        soc_cm_sfree(unit, buf);
        return rv;
    }
    for (i = BCM_VLAN_MIN; i < BCM_VLAN_MAX; i++) {
        vtab = soc_mem_table_idx_to_pointer(unit, VLAN_TABm, vlan_tab_entry_t *,
                                            buf, i);
        if (!soc_mem_field32_get(unit, VLAN_TABm, vtab, VALIDf)) {
            continue;                                     
        }
        grp_bmp = soc_mem_field32_get(unit, VLAN_TABm, vtab, 
                                      VLAN_GROUP_BITMAPf);
        if ((grp_bmp >> vvp) & 0x1) {
            /* VLAN is allowed to come in on this AC 2 AC tunnel */
            BCM_VLAN_VEC_SET((*vlan_vec), i);
        }
    }
    soc_cm_sfree(unit, buf);
    return rv;
}


/*
 * Helper functions for setting/getting a field in the LPORT table and
 * update corresponding reference in the WLAN_SVP_TABLE.
 * Assumes locks are taken outside.
 */
int
bcm_tr2_wlan_lport_field_set(int unit, bcm_gport_t wlan_port_id,
                             soc_field_t field, int value)
{
    int value_old = 0, rv = BCM_E_NONE;
    uint32 vp;
    wlan_svp_table_entry_t svp;
    lport_tab_entry_t lport_profile;
    rtag7_port_based_hash_entry_t rtag7_entry;
    int lport_ptr, old_lport_ptr = -1;
    void *entries[2];

    /* Check valid LPORT field */
    if (!SOC_MEM_FIELD_VALID(unit, LPORT_TABm, field)) {
        return BCM_E_UNAVAIL;
    }

    /* Get the VP index from the gport */
    vp = BCM_GPORT_WLAN_PORT_ID_GET(wlan_port_id);

    /* Be sure the entry is used and is set for WLAN */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
        return BCM_E_BADID;
    }
    BCM_IF_ERROR_RETURN(READ_WLAN_SVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &svp));

    /* Get old lport_profile */
    old_lport_ptr = soc_WLAN_SVP_TABLEm_field32_get(unit, &svp, 
                                                    LPORT_PROFILE_IDXf);
    entries[0] = &lport_profile;
    entries[1] = &rtag7_entry;
    BCM_IF_ERROR_RETURN
        (_bcm_lport_profile_entry_get(unit, old_lport_ptr, 1, entries));
    value_old = soc_LPORT_TABm_field32_get(unit, &lport_profile, field);
    if (value != value_old) {
        soc_LPORT_TABm_field32_set(unit, &lport_profile, field, value);
        BCM_IF_ERROR_RETURN(_bcm_lport_profile_entry_add(unit, entries, 1, 
                                                        (uint32 *) &lport_ptr));
        soc_WLAN_SVP_TABLEm_field32_set(unit, &svp, LPORT_PROFILE_IDXf, 
                                        lport_ptr);
        /* coverity[negative_returns : FALSE] */
        BCM_IF_ERROR_RETURN
            (WRITE_WLAN_SVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &svp));
        BCM_IF_ERROR_RETURN
            (_bcm_lport_profile_entry_delete(unit, old_lport_ptr));
    }
    return rv;
}

int
bcm_tr2_wlan_lport_field_get(int unit, bcm_gport_t wlan_port_id, 
                             soc_field_t field, int *value)
{
    int rv = BCM_E_NONE;
    uint32 vp;
    wlan_svp_table_entry_t svp;
    lport_tab_entry_t lport_profile;
    rtag7_port_based_hash_entry_t rtag7_entry;
    int lport_ptr;
    void *entries[2];

    /* Check valid LPORT field */
    if (!SOC_MEM_FIELD_VALID(unit, LPORT_TABm, field)) {
        return BCM_E_UNAVAIL;
    }

    /* Get the VP index from the gport */
    vp = BCM_GPORT_WLAN_PORT_ID_GET(wlan_port_id);

    /* Be sure the entry is used and is set for WLAN */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
        return BCM_E_BADID;
    }
    BCM_IF_ERROR_RETURN(READ_WLAN_SVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &svp));

    /* Get old lport_profile */
    lport_ptr = soc_WLAN_SVP_TABLEm_field32_get(unit, &svp, LPORT_PROFILE_IDXf);
    entries[0] = &lport_profile;
    entries[1] = &rtag7_entry;
    BCM_IF_ERROR_RETURN
        (_bcm_lport_profile_entry_get(unit, lport_ptr, 1, entries));
    *value = soc_LPORT_TABm_field32_get(unit, &lport_profile, field);
    return rv;
}

int
_bcm_tr2_wlan_port_set(int unit, bcm_gport_t wlan_port_id, soc_field_t field,
                       uint32 value)
{
    int rv = BCM_E_NONE;
    WLAN_LOCK(unit);
    rv = bcm_tr2_wlan_lport_field_set(unit, wlan_port_id, field, value);
    WLAN_UNLOCK(unit);
    return rv;
}

int
bcm_tr2_wlan_port_untagged_vlan_set(int unit, bcm_gport_t port, bcm_vlan_t vid)
{
    int old_profile, profile, rv = BCM_E_NONE;
    bcm_vlan_action_set_t action;
    WLAN_LOCK(unit);
    /* Get current default tag action entry */
    rv = bcm_tr2_wlan_lport_field_get(unit, port, TAG_ACTION_PROFILE_PTRf, 
                                      &old_profile);
    if (rv < 0) {
        WLAN_UNLOCK(unit);
        return rv;
    }
    bcm_vlan_action_set_t_init(&action);
    _bcm_trx_vlan_action_profile_entry_get(unit, &action, old_profile);

    /* Add new tag action entry */
    action.new_outer_vlan = vid;
    rv = bcm_tr2_wlan_lport_field_get(unit, port,
                                      PORT_PRIf, &(action.priority));
    if (rv < 0) {
        WLAN_UNLOCK(unit);
        return rv;
    }
    rv = _bcm_trx_vlan_action_profile_entry_add(unit, &action, (uint32*) &profile);
    if (rv < 0) {
        WLAN_UNLOCK(unit);
        return rv;
    }
    /* Set the port VID value */
    rv = bcm_tr2_wlan_lport_field_set(unit, port, PORT_VIDf, vid);
    if (rv < 0) {
        WLAN_UNLOCK(unit);
        return rv;
    }
    /* Point to the new profile */
    rv = bcm_tr2_wlan_lport_field_set(unit, port, TAG_ACTION_PROFILE_PTRf,
                                      profile);
    if (rv < 0) {
        WLAN_UNLOCK(unit);
        return rv;
    }
    /* Delete the old profile */
    rv = _bcm_trx_vlan_action_profile_entry_delete(unit, old_profile);
    WLAN_UNLOCK(unit);
    return rv;
}

int
bcm_tr2_wlan_port_untagged_vlan_get(int unit, bcm_gport_t port, 
                                    bcm_vlan_t *vid_ptr)
{
    int value, rv = BCM_E_NONE;
    rv = bcm_tr2_wlan_lport_field_get(unit, port, PORT_VIDf, &value);
    *vid_ptr = (bcm_vlan_t)value;
    return rv;
}

int
bcm_tr2_wlan_port_untagged_prio_set(int unit, bcm_gport_t port, int prio)
{
    int rv = BCM_E_NONE;
    if (prio > 7) {
        return BCM_E_PARAM;
    }
    rv = _bcm_tr2_wlan_port_set(unit, port, PORT_PRIf, prio);
    return rv;
}

int
bcm_tr2_wlan_port_untagged_prio_get(int unit, bcm_gport_t port, 
                                    int *prio_ptr)
{
    int value, rv = BCM_E_NONE;
    rv = bcm_tr2_wlan_lport_field_get(unit, port, PORT_PRIf, &value);
    *prio_ptr = value;
    return rv;
}
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
