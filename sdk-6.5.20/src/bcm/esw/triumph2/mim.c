/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    mim.c
 * Purpose: Manages MiM functions
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

#if defined BCM_METROLITE_SUPPORT
#include <soc/katana2.h>
#endif

#include <bcm/error.h>
#include <bcm/mim.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw/mim.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/switch.h>
#if defined(BCM_TRIUMPH3_SUPPORT)
#include <bcm_int/esw/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPPORT*/

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm_int/esw/failover.h>

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
#include <bcm_int/esw/policer.h>
#endif
#if defined(BCM_KATANA2_SUPPORT)
#include <bcm_int/esw/katana2.h>
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
#include <bcm_int/esw/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/trident2plus.h>
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
#include <soc/esw/cancun.h>
#include <bcm_int/esw/trident3.h>
#endif
#if defined(BCM_TOMAHAWK2_SUPPORT)
#include <bcm_int/esw/tomahawk2.h>
#endif /* BCM_TOMAHAWK2_SUPPORT */

#define _MIM_EVXLT_OP_ADD         1
#define _MIM_EVXLT_OP_GET         2
#define _MIM_EVXLT_OP_DELETE      3

#define _MIM_OPRI_SEL_FROM_TBL    1

typedef struct _bcm_tr2_ing_nh_info_s {
    int      port;
    int      module;
    int      trunk;
    uint32   mtu;
} _bcm_tr2_ing_nh_info_t;

typedef struct _bcm_tr2_egr_nh_info_s {
    uint8    entry_type;
    uint8    dvp_is_network;
    uint8    sd_tag_action_present;
    uint8    sd_tag_action_not_present;
    int      dvp;
    int      intf_num;
    int      sd_tag_vlan;
    int      sd_tag_pri;
    int      sd_tag_cfi;
    int      macda_index;
    int      tpid_index;
    int      vintf_ctr_idx;
} _bcm_tr2_egr_nh_info_t;

_bcm_tr2_mim_bookkeeping_t  _bcm_tr2_mim_bk_info[BCM_MAX_NUM_UNITS];

/* Flag to check initialized status */
STATIC int mim_initialized[BCM_MAX_NUM_UNITS];

STATIC sal_mutex_t _mim_mutex[BCM_MAX_NUM_UNITS] = {NULL};

#define L3_INFO(unit)    (&_bcm_l3_bk_info[unit])
#define MIM_INFO(_unit_) (&_bcm_tr2_mim_bk_info[_unit_])
#define VPN_ISID(_unit_, _vfi_)  \
        (_bcm_tr2_mim_bk_info[_unit_].vpn_info[_vfi_].isid)

#define MIM_INIT(unit)                                    \
    do {                                                  \
        if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {  \
            return BCM_E_UNIT;                            \
        }                                                 \
        if (!mim_initialized[unit]) {                     \
            return BCM_E_INIT;                            \
        }                                                 \
    } while (0)

/* 
 * MIM module lock
 */
#define MIM_LOCK(unit) \
        sal_mutex_take(_mim_mutex[unit], sal_mutex_FOREVER);

#define MIM_UNLOCK(unit) \
        sal_mutex_give(_mim_mutex[unit]); 

/*
 * L3 interface usage bitmap operations
 */
#define _BCM_MIM_INTF_USED_GET(_u_, _intf_) \
        SHR_BITGET(MIM_INFO(_u_)->intf_bitmap, (_intf_))
#define _BCM_MIM_INTF_USED_SET(_u_, _intf_) \
        SHR_BITSET(MIM_INFO((_u_))->intf_bitmap, (_intf_))
#define _BCM_MIM_INTF_USED_CLR(_u_, _intf_) \
        SHR_BITCLR(MIM_INFO((_u_))->intf_bitmap, (_intf_))

#ifdef BCM_WARM_BOOT_SUPPORT
#define MEM_CHUNK_MAX_ENTRY 1024

#define _BCM_CHUNK_GET_INDEX(unit, mem, mem_index ) \
    (mem_index - (mem_index / MEM_CHUNK_MAX_ENTRY) * MEM_CHUNK_MAX_ENTRY)

#define _BCM_CHUNK_GET_MEM_BUFFER(chunk_buf) \
        ((void*)((int32*)chunk_buf + 1))

#define _BCM_CHUNK_SET_INDEX_OFFSET(chunk_buf, val) \
        (*((int32*)chunk_buf) = val)
#define _BCM_CHUNK_GET_INDEX_OFFSET(chunk_buf) \
        (*((int32*)(chunk_buf)))

STATIC int _bcm_chunk_memory_alloc(int unit, soc_mem_t mem, void **mem_buf);
STATIC int _bcm_chunk_get_mem_entry(int unit, soc_mem_t mem, int index, 
                                    void *chunk_buf, void **mem_entry);
#endif
STATIC int _bcm_tr2_mim_port_delete(int unit, bcm_vpn_t vpn, int vp);
STATIC int _bcm_tr2_mim_egr_vxlt_sd_tag_actions_get(int unit,
                                                    bcm_mim_port_t *mim_port,
                                                    bcm_mim_vpn_config_t *vpn_info,
                                                    uint32 *egr_vlan_xlate_entry);
STATIC int
_bcm_tr2_mim_egr_sd_tag_actions(int unit, bcm_mim_port_t *mim_port,
                                _bcm_tr2_egr_nh_info_t *egr_nh_info);

int _bcm_tr2_mim_egr_vxlt_sd_tag_actions(int unit, bcm_mim_port_t *mim_port,
                                         bcm_mim_vpn_config_t *vpn_info,
                                         uint32 *evxlt_entry);
int _bcm_tr2_mim_egr_vxlt_sd_tag_actions_delete(int unit,
                                uint32 *egr_vlan_xlate_entry);
typedef uint32 MEM_ENTRY_T[SOC_MAX_MEM_FIELD_WORDS];

/*
 * Function:
 *      bcm_mim_enable
 * Purpose:
 *      Enable/disable MiM function.
 * Parameters:
 *      unit - SOC unit number.
 *      enable - TRUE: enable MiM support; FALSE: disable MiM support.
 * Returns:
 *      BCM_E_XXX.
 */
STATIC int
bcm_tr2_mim_enable(int unit, int enable)
{
    int port;
    int idx;
    uint32 ptab[SOC_MAX_MEM_WORDS];
    bcm_pbmp_t port_pbmp;
    soc_mem_t port_mem;
#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_KATANA_SUPPORT)
    uint32 flags;
#endif

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, ING_DEVICE_PORTm)) {
        port_mem = ING_DEVICE_PORTm;
    } else
#endif
    {
        port_mem = PORT_TABm;
    }


    BCM_PBMP_CLEAR(port_pbmp);
    BCM_PBMP_ASSIGN(port_pbmp, PBMP_PORT_ALL(unit));
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit) && soc_feature(unit, soc_feature_flex_port)) {
        BCM_IF_ERROR_RETURN(_bcm_kt2_flexio_pbmp_update(unit, &port_pbmp));
    }
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        _bcm_kt2_subport_pbmp_update(unit, &port_pbmp);
    }
#endif

    PBMP_ITER(port_pbmp, port) {
        /* No MiM lookup on stacking ports. */
        if (IS_ST_PORT(unit, port)) {
            continue;
        }
        /* wait for SDK-95379, modfiy this to unblock others. */
#if defined(BCM_TRIDENT3_SUPPORT)
        if (!SOC_IS_TRIDENT3X(unit))
#endif
        {
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_control_set(unit, port, bcmPortControlMacInMac,
                                      (enable) ? 1 : 0));
        }
    }
    /* Enable MC_TERM_ENABLE on loopback port */
#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_KATANA_SUPPORT)
    if (SOC_IS_ENDURO(unit) || SOC_IS_KATANAX(unit)) {
        PBMP_ALL_ITER(unit, port) {
            if (IS_LB_PORT(unit, port)) {
                BCM_IF_ERROR_RETURN
                    (soc_mem_read(unit, PORT_TABm, MEM_BLOCK_ANY, port, &ptab));
                soc_PORT_TABm_field32_set(unit, &ptab, 
                              MIM_MC_TERM_ENABLEf, (enable) ? 1 : 0);    
                BCM_IF_ERROR_RETURN
                    (soc_mem_write(unit, PORT_TABm, MEM_BLOCK_ALL, port, &ptab));
                /* 
                 * The loopback port is subjected to ingress/egress vlan +
                 * spanning tree check in Enduro.
                 *  - Disable VLAN membership check for loopback port. 
                 *  - Set STG state to forwarding.
                 */
                BCM_IF_ERROR_RETURN
                    (bcm_esw_port_vlan_member_get(unit, port, &flags));
                flags &= ~(BCM_PORT_VLAN_MEMBER_EGRESS);
                BCM_IF_ERROR_RETURN
                    (bcm_esw_port_vlan_member_set(unit, port, flags));
                BCM_IF_ERROR_RETURN
                    (bcm_esw_port_stp_set(unit, port, BCM_STG_STP_FORWARD));
            }
        }
    } else 
#endif /* BCM_ENDURO_SUPPORT */
    {
    if (SOC_IS_TRIUMPH3(unit)) {
        /* Triumph3 nlf passthru loopback port */
        idx = AXP_PORT(unit, SOC_AXP_NLF_PASSTHRU);
#if defined(BCM_TRIDENT_SUPPORT)
    } else if (SOC_IS_TD_TT(unit)) {
        idx = LB_PORT(unit);        
#endif /* BCM_TRIDENT_SUPPORT */ 
    } else {
        idx = 54;
    }
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, port_mem, MEM_BLOCK_ANY, idx, ptab));
    /* For TD3, it is done by c-code. We don't have to configure anything */
#if defined(BCM_TRIDENT3_SUPPORT)
    if (!SOC_IS_TRIDENT3X(unit))
#endif
    {
        soc_mem_field32_set(unit, port_mem, ptab, 
                 MIM_MC_TERM_ENABLEf, (enable) ? 1 : 0);
    }
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, port_mem, MEM_BLOCK_ALL, idx, ptab));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr2_mim_free_resources
 * Purpose:
 *      Free all allocated tables and memory
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_tr2_mim_free_resources(int unit)
{
    _bcm_tr2_mim_bookkeeping_t *mim_info = MIM_INFO(unit);

    if (_mim_mutex[unit]) {
        sal_mutex_destroy(_mim_mutex[unit]);
        _mim_mutex[unit] = NULL;
    } 
    if (mim_info->vpn_info) {
        sal_free(mim_info->vpn_info);
        mim_info->vpn_info = NULL;
    }
    if (mim_info->port_info) {
        sal_free(mim_info->port_info);
        mim_info->port_info = NULL;
    }
    if (mim_info->intf_bitmap) {
        sal_free(mim_info->intf_bitmap);
        mim_info->intf_bitmap = NULL;
    }
}

/*
 * Function:
 *      _bcm_tr2_mim_hw_clear
 * Purpose:
 *     Perform HW tables clean up for MiM module.
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr2_mim_hw_clear(int unit)
{
    int rv, rv_error = BCM_E_NONE;

    rv = bcm_tr2_mim_enable(unit, FALSE);
    if (BCM_FAILURE(rv)) {
        rv_error = rv;
    }

    /* Destroy all VPNs */
    rv = bcm_tr2_mim_vpn_destroy_all(unit);
    if (BCM_FAILURE(rv) && (BCM_E_NONE == rv_error)) {
        rv_error = rv;
    }

    return rv_error;
}

/*
 * Function:
 *      bcm_mim_detach
 * Purpose:
 *      Detach the MIM software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr2_mim_detach(int unit)
{
    int rv = BCM_E_NONE;

    if (mim_initialized[unit] == FALSE) {
        return BCM_E_NONE;
    } 

    if (0 == SOC_HW_ACCESS_DISABLE(unit)) {
        rv = _bcm_tr2_mim_hw_clear(unit);
    }

    _bcm_tr2_mim_free_resources(unit);
    mim_initialized[unit] = FALSE;
    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC void
_bcm_tr2_mim_port_flex_stat_recover(int unit, source_vp_entry_t *source_vp, 
                                    int index)
{
    int fs_idx;
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        if (soc_mem_field_valid(unit, SOURCE_VPm, VINTF_CTR_IDXf)) {
            fs_idx = soc_mem_field32_get(unit, SOURCE_VPm, source_vp,
                                         VINTF_CTR_IDXf);
            if (fs_idx) {
                BCM_GPORT_MIM_PORT_ID_SET(index, index);
                _bcm_esw_flex_stat_reinit_add(unit, _bcmFlexStatTypeGport,
                                              fs_idx, index);
            }
        }
    }
    return;
}

STATIC void
_bcm_tr2_mim_vpn_flex_stat_recover(int unit, vfi_entry_t *vfi, 
                                   int index)
{
    int fs_idx;
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        if (soc_mem_field_valid(unit, VFIm, SERVICE_CTR_IDXf)) {
            fs_idx = soc_mem_field32_get(unit, VFIm, vfi,
                                         SERVICE_CTR_IDXf);
            if (fs_idx) {
                _BCM_MIM_VPN_SET(index, _BCM_MIM_VPN_TYPE_MIM, index);
                _bcm_esw_flex_stat_reinit_add(unit, _bcmFlexStatTypeService,
                                              fs_idx, index);
            }
        }
    }
    return;
}

STATIC int
_bcm_tr2_mim_source_vp_tpid_recover(int unit, source_vp_entry_t *source_vp, int vp)
{
    int tpid_enable, index, rv = BCM_E_NONE;
    if (soc_SOURCE_VPm_field32_get(unit, source_vp, SD_TAG_MODEf) == 1) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            uint32 svp1[SOC_MAX_MEM_WORDS] = {0};
            rv = READ_SVP_ATTRS_1m(unit, MEM_BLOCK_ANY, vp, &svp1);
            if (rv < 0) {
                return rv;
            }
            tpid_enable = soc_SVP_ATTRS_1m_field32_get(unit, svp1, TPID_ENABLEf);
        } else
#endif
        {
            tpid_enable = soc_SOURCE_VPm_field32_get(unit, source_vp, TPID_ENABLEf);
        }
        for (index = 0; index < 4; index++) {
            if (tpid_enable & (1 << index)) {
                rv = _bcm_fb2_outer_tpid_tab_ref_count_add(unit, index, 1);
                break;
            }
        }
    }  
    return rv;
}

STATIC int
_bcm_tr2_mim_egr_vxlt_tpid_recover(int unit,  
                                   uint32 *egr_vlan_xlate)
{
    int tpid_enable, index, rv = BCM_E_NONE;
    soc_mem_t  hw_mem;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        hw_mem = EGR_VLAN_XLATE_1_DOUBLEm;
    } else
#endif
    {
        hw_mem = EGR_VLAN_XLATEm;
    }

    if ((soc_mem_field32_get(unit, hw_mem, egr_vlan_xlate, 
         MIM_ISID__SD_TAG_ACTION_IF_NOT_PRESENTf) == 1) || 
        (soc_mem_field32_get(unit, hw_mem, egr_vlan_xlate,
         MIM_ISID__SD_TAG_ACTION_IF_PRESENTf) == 1)
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
         || (soc_mem_field32_get(unit, hw_mem, egr_vlan_xlate,
         MIM_ISID__SD_TAG_ACTION_IF_PRESENTf) == 0x7)
#endif
         ) {
        /* BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TPID_REPLACE or 
           BCM_MIM_PORT_EGRESS_SERVICE_VLAN_ADD */ 
        tpid_enable = soc_mem_field32_get(unit, hw_mem, egr_vlan_xlate, 
                                                 MIM_ISID__SD_TAG_TPID_INDEXf);
        if (SOC_IS_TRIDENT3X(unit)) {
            for (index = 0; index < 4; index++) {
                if (tpid_enable & (1 << index)) {
                    rv = _bcm_fb2_outer_tpid_tab_ref_count_add(unit, index, 1);
                    break;
                }
            }
        } else {
            rv = _bcm_fb2_outer_tpid_tab_ref_count_add(unit, tpid_enable, 1);
        }
    }  
    return rv;
}

STATIC int
_bcm_tr2_mim_egr_nh_tpid_recover(int unit,  
                                 egr_l3_next_hop_entry_t *egr_l3_next_hop)
{
    int index, tpid_idx, rv = BCM_E_NONE;
    if ((soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_l3_next_hop, 
         SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf) == 1) || 
        (soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_l3_next_hop, 
         SD_TAG__SD_TAG_ACTION_IF_PRESENTf) == 1)
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
         ||(soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_l3_next_hop,
         SD_TAG__SD_TAG_ACTION_IF_PRESENTf) == 0x7)
#endif
         ) {
        /* BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TPID_REPLACE or 
           BCM_MIM_PORT_EGRESS_SERVICE_VLAN_ADD */ 
        tpid_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_l3_next_hop, 
                                                 SD_TAG__SD_TAG_TPID_INDEXf);
        if (SOC_IS_TRIDENT3X(unit)) {
            for (index = 0; index < 4; index++) {
                if (tpid_idx & (1 << index)) {
                    rv = _bcm_fb2_outer_tpid_tab_ref_count_add(unit, index, 1);
                    break;
                }
            }
        } else {
            rv = _bcm_fb2_outer_tpid_tab_ref_count_add(unit, tpid_idx, 1);
        }
    }  
    return rv;
}

#if defined(BCM_TRIDENT3_SUPPORT)

STATIC int
_bcm_td3_mim_flex_evxlt_tpid_recover(int unit,
                                   uint32 *evxlt_entry, int vfi_view)
{
    int tpid_inx;
    soc_mem_t  mem;
    uint32 profile_index;
    int action_present;
    int action_not_present;

    if (vfi_view) {
        mem = EGR_VLAN_XLATE_1_SINGLEm;

        profile_index = soc_mem_field32_get(unit, mem, evxlt_entry,
                                   VFI_ASSIGN_ISID__TAG_ACTION_PROFILE_PTRf);
        (void)_bcm_td3_sd_tag_action_get(unit, profile_index,
                               &action_present, &action_not_present);
        if (action_present == 0x7) {
            tpid_inx = soc_mem_field32_get(unit, mem, evxlt_entry,
                                  VFI_ASSIGN_ISID__OUTER_TPID_INDEXf);
            (void)_bcm_fb2_outer_tpid_tab_ref_count_add(unit,tpid_inx,1);
        }
    } else {
        mem = EGR_VLAN_XLATE_1_DOUBLEm;

        profile_index = soc_mem_field32_get(unit, mem, evxlt_entry,
                                DVP_VFI_ASSIGN_ISID__TAG_ACTION_PROFILE_PTRf);
        (void)_bcm_td3_sd_tag_action_get(unit, profile_index,
                               &action_present, &action_not_present);
        if ((action_not_present == 0x1) || (action_present == 0x1) ||
            (action_present == 0x7)) {
            tpid_inx = soc_mem_field32_get(unit, mem, evxlt_entry,
                                 DVP_VFI_ASSIGN_ISID__OUTER_TPID_INDEXf);
            (void)_bcm_fb2_outer_tpid_tab_ref_count_add(unit,tpid_inx,1);
        }
    }
    return BCM_E_NONE;
}
#endif /* defined(BCM_TRIDENT3_SUPPORT) */

STATIC int
_bcm_tr2_mim_port_associated_data_recover(int unit, int vp, int stable_size)
{
    int idx, nh_index, macda_idx, intf_num, rv = BCM_E_NONE;
    uint32 nh_flags, port_flags;
    ing_l3_next_hop_entry_t ing_nh_entry;
    egr_l3_next_hop_entry_t egr_nh_entry;
    ing_dvp_table_entry_t ing_dvp_entry;
    bcm_module_t mod_in, mod_out;
    bcm_port_t port_in, port_out, phys_port;
    bcm_trunk_t trunk_id;
    bcm_l3_egress_t nh_info;
    _bcm_port_info_t *info;
#if defined(BCM_ESW_SUPPORT) && (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
    bcm_port_t      local_member_array[SOC_MAX_NUM_PP_PORTS];
    int             max_num_ports = SOC_MAX_NUM_PP_PORTS;
#else
    bcm_port_t      local_member_array[SOC_MAX_NUM_PORTS];
    int             max_num_ports = SOC_MAX_NUM_PORTS;
#endif
    int local_member_count;
    int entry_type = 0;
    int is_trunk = 0;

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
        if (soc_feature(unit, soc_feature_generic_dest)) {
            uint32 dt, dv;
            dv = soc_mem_field32_dest_get(unit, ING_L3_NEXT_HOPm,
                    &ing_nh_entry, DESTINATIONf, &dt);
            if (dt == SOC_MEM_FIF_DEST_LAG) {
                /* Trunk group */
                trunk_id = dv & SOC_MEM_FIF_DGPP_TGID_MASK;
                is_trunk = 1;
            } else {
                port_in = dv & SOC_MEM_FIF_DGPP_PORT_MASK;
                mod_in = (dv & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                          SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
            }
        } else {
            if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry, Tf)) {
                trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry, TGIDf);
                is_trunk = 1;
            } else {
                mod_in = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry, MODULE_IDf);
                port_in = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry, PORT_NUMf);
            }
        }

        /* Type is SVP */
        if (is_trunk) {
            MIM_INFO(unit)->port_info[vp].modid = -1;
            MIM_INFO(unit)->port_info[vp].port = -1;
            MIM_INFO(unit)->port_info[vp].tgid = trunk_id;
            /* Update each local physical port's SW state */
            if (stable_size == 0) {
                /* When persistent storage is available, this state is recovered
                   in the port module */
                rv = _bcm_esw_trunk_local_members_get(unit, trunk_id,
                        max_num_ports, local_member_array, &local_member_count);
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
            rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                        mod_in, port_in, &mod_out, &port_out);
            MIM_INFO(unit)->port_info[vp].modid = mod_out;
            MIM_INFO(unit)->port_info[vp].port = port_out;
            MIM_INFO(unit)->port_info[vp].tgid = -1;
            /* Update the physical port's SW state */
            if (stable_size == 0) {
                /* When persistent storage is available, this state is recovered
                   in the port module */
                phys_port = MIM_INFO(unit)->port_info[vp].port; 
                if (soc_feature(unit, soc_feature_sysport_remap)) { 
                    BCM_XLATE_SYSPORT_S2P(unit, &phys_port); 
                }
                _bcm_port_info_access(unit, phys_port, &info);
                info->vp_count++;
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_vlan_member_get(unit, phys_port, &port_flags));
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_vlan_member_set(unit, phys_port, port_flags));
            }
        }
    } else {
        return BCM_E_INTERNAL; /* Should never happen */
    }

#if defined(BCM_TRIDENT3_SUPPORT)
	if (SOC_IS_TRIDENT3X(unit)) {
        entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
			&egr_nh_entry, DATA_TYPEf);
	} else
#endif /* BCM_TRIDENT3_SUPPORT */
	{
	    entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
			&egr_nh_entry, ENTRY_TYPEf);
	}

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_mim_decoupled_mode)) {
        if (entry_type == 21) {
            /* new MIM decoupled mode view - recover TPID reference counts */
            rv = _bcm_td3_flex_egrnh_tpid_recover(unit, &egr_nh_entry);
            BCM_IF_ERROR_RETURN(rv);
        }
    } else
#endif
    {
        if (entry_type == 2) {
            /* SD TAG view - recover TPID reference counts */
            rv = _bcm_tr2_mim_egr_nh_tpid_recover(unit, &egr_nh_entry);
            BCM_IF_ERROR_RETURN(rv);
        }
    }

    if (entry_type == 3) {
        /* MIM view - recover MACDA profile and interface reference counts */
        macda_idx = soc_EGR_L3_NEXT_HOPm_field32_get
                        (unit, &egr_nh_entry, MIM__MAC_DA_PROFILE_INDEXf);
        _bcm_common_profile_mem_ref_cnt_update(unit, EGR_MAC_DA_PROFILEm, 
                                               macda_idx, 1);
        intf_num = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh_entry,
                                                    MIM__INTF_NUMf);
        if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, MIM__ISID_LOOKUP_TYPEf)) {
            if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh_entry,
                                                  MIM__ISID_LOOKUP_TYPEf) == 0) {
                MIM_INFO(unit)->port_info[vp].flags = _BCM_MIM_PORT_TYPE_NETWORK;
            } else {
                MIM_INFO(unit)->port_info[vp].flags = _BCM_MIM_PORT_TYPE_PEER;
            }
        }

        _BCM_MIM_INTF_USED_SET(unit, intf_num);
        BCM_L3_INTF_USED_SET(unit, intf_num);
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        soc_mem_t mem = EGR_DVP_ATTRIBUTEm;
        egr_dvp_attribute_entry_t egr_dvp_attribute;

        sal_memset(&egr_dvp_attribute, 0, sizeof(egr_dvp_attribute_entry_t));
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, vp, &egr_dvp_attribute);
        BCM_IF_ERROR_RETURN(rv);

        if (soc_feature(unit, soc_feature_mim_decoupled_mode)) {
            if (soc_mem_field32_get(unit, mem, &egr_dvp_attribute,
                                     FLEX_MIM__VXLT_CONTROL_IDf) == 0x1) {
                MIM_INFO(unit)->port_info[vp].flags =_BCM_MIM_PORT_TYPE_NETWORK;
            } else if (soc_mem_field32_get(unit, mem, &egr_dvp_attribute,
                                        FLEX_MIM__VXLT_CONTROL_IDf) == 0x2) {
                MIM_INFO(unit)->port_info[vp].flags = _BCM_MIM_PORT_TYPE_PEER;
            }
        } else {
            if (SOC_MEM_FIELD_VALID(unit, mem, COMMON__EVXLT_KEY_SELf)) {
                if (soc_mem_field32_get(unit, mem, &egr_dvp_attribute,
                                         COMMON__EVXLT_KEY_SELf) == 0x1) {
                    MIM_INFO(unit)->port_info[vp].flags = _BCM_MIM_PORT_TYPE_NETWORK;
                } else if (soc_mem_field32_get(unit, mem, &egr_dvp_attribute,
                                            COMMON__EVXLT_KEY_SELf) == 0x2) {
                    MIM_INFO(unit)->port_info[vp].flags = _BCM_MIM_PORT_TYPE_PEER;
                }
            }
        }
    }
#endif
    return rv;
}

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1, 0)
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1, 1)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_1

/*
 * Function:
 *      _bcm_tr2_mim_wb_alloc
 *
 * Purpose:
 *      Alloc persisitent memory for Level 2 Warm Boot scache.
 *
 * Parameters:
 *      unit - (IN) Device Unit Number.
 *
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr2_mim_wb_alloc(int unit)
{
    int alloc_sz = 0, rv = BCM_E_NONE;
    soc_scache_handle_t scache_handle;
    int num_vp = 0;

    uint8 *mim_state;
    int stable_size;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }

    if (soc_feature(unit, soc_feature_vp_lag)) {
        /* Size of Initiator of all VP */
        num_vp = soc_mem_index_count(unit, SOURCE_VPm);
        /*
         * Size of All match_key's flags && index && gport(trunk_id, modid, port) &&
         * match_vlan && match_inner_vlan && match_label &&
         * match_tunnel_srcmac && match_tunnel_vlan.
         */
        alloc_sz += num_vp * (sizeof(uint32) * 2 + sizeof(bcm_gport_t) +
                              sizeof(bcm_vlan_t) * 2 + sizeof(bcm_mpls_label_t) +
                              sizeof(bcm_mac_t) + sizeof(bcm_vlan_t));
    }
#if defined(BCM_TRIDENT_SUPPORT)
    else if (soc_feature(unit, soc_feature_mim_peer_sharing)) {
        /* Size of the flags of all ports */
        num_vp = soc_mem_index_count(unit, SOURCE_VPm);
        alloc_sz += num_vp * sizeof(uint32);
    }
#endif /* BCM_TRIDENT_SUPPORT */

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_MIM, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                 alloc_sz, (uint8**)&mim_state,
                                 BCM_WB_DEFAULT_VERSION, NULL);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr2_mim_wb_recover
 *
 * Purpose:
 *      Recover MIM module info for Level 2 Warm Boot from persisitent memory
 *
 * Warm Boot Version Map:
 *      see _bcm_esw_mim_sync definition
 *
 * Parameters:
 *      unit - (IN) Device Unit Number.
 *
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr2_mim_wb_recover(int unit)
{
    int i, rv = BCM_E_NONE;
    int num_vp = 0;
    int stable_size;
    uint16 recovered_ver = 0;
    bcm_gport_t gport;
    uint8 *mim_state = NULL;
    soc_scache_handle_t scache_handle;
    _bcm_tr2_mim_bookkeeping_t *mim_info;
#if defined(BCM_TRIDENT_SUPPORT)
    int additional_scache_size = 0;
#endif /* BCM_TRIDENT_SUPPORT */

    mim_info = MIM_INFO(unit);

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* Requires extended scache support level-2 warmboot */
    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_MIM, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &mim_state,
                                 BCM_WB_DEFAULT_VERSION, &recovered_ver);
    if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
        return rv;
    } else if (rv == BCM_E_NOT_FOUND) {
        return _bcm_tr2_mim_wb_alloc(unit);
    }

    if (mim_state == NULL) {
        return BCM_E_NONE;
    }

    if (soc_feature(unit, soc_feature_vp_lag)) {
        /*
         * Recover the index && gport(trunk_id, modid, port) && match_vlan &&
         * match_inner_vlan of each Match Key.
         */
        num_vp = soc_mem_index_count(unit, SOURCE_VPm);
        for (i = 0; i < num_vp; i++) {
            sal_memcpy(&(mim_info->port_info[i].flags), mim_state,
                       sizeof(uint32));
            mim_state += sizeof(uint32);
            sal_memcpy(&(mim_info->port_info[i].index), mim_state,
                       sizeof(uint32));
            mim_state += sizeof(uint32);
            sal_memcpy(&gport, mim_state, sizeof(bcm_gport_t));
            if (BCM_GPORT_IS_TRUNK(gport)) {
                mim_info->port_info[i].tgid =
                    BCM_GPORT_TRUNK_GET(gport);
            } else if (BCM_GPORT_IS_MODPORT(gport)) {
                mim_info->port_info[i].modid =
                    BCM_GPORT_MODPORT_MODID_GET(gport);
                mim_info->port_info[i].port =
                    BCM_GPORT_MODPORT_PORT_GET(gport);
            }
            mim_state += sizeof(bcm_gport_t);
            sal_memcpy(&(mim_info->port_info[i].match_vlan), mim_state,
                       sizeof(bcm_vlan_t));
            mim_state += sizeof(bcm_vlan_t);
            sal_memcpy(&(mim_info->port_info[i].match_inner_vlan),
                       mim_state, sizeof(bcm_vlan_t));
            mim_state += sizeof(bcm_vlan_t);
            sal_memcpy(&(mim_info->port_info[i].match_label), mim_state,
                       sizeof(bcm_mpls_label_t));
            mim_state += sizeof(bcm_mpls_label_t);
            sal_memcpy(&(mim_info->port_info[i].match_tunnel_srcmac),
                       mim_state, sizeof(bcm_mac_t));
            mim_state += sizeof(bcm_mac_t);
            sal_memcpy(&(mim_info->port_info[i].match_tunnel_vlan),
                       mim_state, sizeof(bcm_vlan_t));
            mim_state += sizeof(bcm_vlan_t);
        }
    }
#if defined(BCM_TRIDENT_SUPPORT)
    else if (soc_feature(unit, soc_feature_mim_peer_sharing)) {
        num_vp = soc_mem_index_count(unit, SOURCE_VPm);
        if (recovered_ver >= BCM_WB_VERSION_1_1) {
            /* Recover the flags of each port */
            for (i = 0; i < num_vp; i++) {
                sal_memcpy(&(mim_info->port_info[i].flags), mim_state,
                           sizeof(uint32));
                mim_state += sizeof(uint32);
            }
        } else {
            additional_scache_size += num_vp * sizeof(uint32);
        }
    }

    if (additional_scache_size > 0) {
        rv = soc_scache_realloc(unit, scache_handle, additional_scache_size);
        if(BCM_FAILURE(rv)) {
           return rv;
        }
    }
#endif /* BCM_TRIDENT_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr2_mim_sync
 *
 * Purpose:
 *      Record MIM module persistent info for Level 2 Warm Boot
 *
 * Warm Boot Version Map:
 *  WB_VERSION_1_0
 *
 * Parameters:
 *      unit - (IN) Device Unit Number.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_mim_sync(int unit)
{
    int i, num_vp = 0;
    int stable_size;
    bcm_gport_t gport;
    uint8 *mim_state;
    soc_scache_handle_t scache_handle;
    _bcm_tr2_mim_bookkeeping_t *mim_info;

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    mim_info = MIM_INFO(unit);

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* Requires extended scache support level-2 warmboot */
    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_MIM, 0);
    BCM_IF_ERROR_RETURN(
        _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                0, &mim_state, BCM_WB_DEFAULT_VERSION, NULL));

    if (soc_feature(unit, soc_feature_vp_lag)) {
        num_vp = soc_mem_index_count(unit, SOURCE_VPm);
        /*
         * Since we can't recover index && gport(trunk_id, modid, port) &&
         * match_vlan && match_inner_vlan from HW if we add this mim port to
         * VPLAG, so we need to store it for warmboot recovery. Total is 34*16K.
         */
        for (i = 0; i < num_vp; i++) {
            sal_memcpy(mim_state, &(mim_info->port_info[i].flags), sizeof(uint32));
            mim_state += sizeof(uint32);
            sal_memcpy(mim_state, &(mim_info->port_info[i].index), sizeof(uint32));
            mim_state += sizeof(uint32);
            if (mim_info->port_info[i].tgid != -1) {
                BCM_GPORT_TRUNK_SET(gport, mim_info->port_info[i].tgid);
            } else if (mim_info->port_info[i].modid != -1) {
                BCM_GPORT_MODPORT_SET(gport,
                                      mim_info->port_info[i].modid,
                                      mim_info->port_info[i].port);
            } else {
                gport = BCM_GPORT_INVALID;
            }
            sal_memcpy(mim_state, &gport, sizeof(bcm_gport_t));
            mim_state += sizeof(bcm_gport_t);
            sal_memcpy(mim_state, &(mim_info->port_info[i].match_vlan),
                       sizeof(bcm_vlan_t));
            mim_state += sizeof(bcm_vlan_t);
            sal_memcpy(mim_state, &(mim_info->port_info[i].match_inner_vlan),
                       sizeof(bcm_vlan_t));
            mim_state += sizeof(bcm_vlan_t);
            sal_memcpy(mim_state, &(mim_info->port_info[i].match_label),
                       sizeof(bcm_mpls_label_t));
            mim_state += sizeof(bcm_mpls_label_t);
            sal_memcpy(mim_state, &(mim_info->port_info[i].match_tunnel_srcmac),
                       sizeof(bcm_mac_t));
            mim_state += sizeof(bcm_mac_t);
            sal_memcpy(mim_state, &(mim_info->port_info[i].match_tunnel_vlan),
                       sizeof(bcm_vlan_t));
            mim_state += sizeof(bcm_vlan_t);
        }
    }
#if defined(BCM_TRIDENT_SUPPORT)
    else if (soc_feature(unit, soc_feature_mim_peer_sharing)) {
        /* Store the flags of each port */
        num_vp = soc_mem_index_count(unit, SOURCE_VPm);
        for (i = 0; i < num_vp; i++) {
            sal_memcpy(mim_state,
                       &(mim_info->port_info[i].flags),sizeof(uint32));
            mim_state += sizeof(uint32);
        }
    }
#endif /* BCM_TRIDENT_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr2_mim_reinit
 * Purpose:
 *      Warm boot recovery for the MIM software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_tr2_mim_reinit(int unit)
{
    int rv, i, index_min, index_max, stable_size;
    uint32 vfi_entry[SOC_MAX_MEM_WORDS] = {0};
    uint32 vfi_1_entry[SOC_MAX_MEM_WORDS] = {0};
    int vfi, isid, index, tpid_enable, vp = 0;
    uint32 key_type;
    uint8 *mpls_entry_buf = NULL; /* MPLS_ENTRY DMA buffer */
    MEM_ENTRY_T *mpls_entry;
    uint8 *vlan_xlate_buf = NULL; /* VLAN_XLATE DMA buffer */
    void * vlan_xlate;
    uint8 *egr_vlan_xlate_buf = NULL; /* EGR_VLAN_XLATE DMA buffer */
    void * egr_vlan_xlate;
    uint8 *source_vp_buf = NULL; /* SOURCE_VP DMA buffer */
    source_vp_entry_t *source_vp;
    uint8 *source_trunk_map_buf = NULL; /* SOURCE_TRUNK_MAP DMA buffer */
    source_trunk_map_table_entry_t *source_trunk_map;
    soc_mem_t mpls_mem = MPLS_ENTRYm;
    soc_mem_t xlate_mem, egr_xlate_mem = EGR_VLAN_XLATEm;
    int         isid_key_type;
    void    *mem_entry[1], *mem_buf[1];
    _bcm_vp_info_t vp_info;
    soc_mem_t vfi_mem;
    soc_mem_t vfi_1_mem;

    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeMim;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* If Level 1 of limited Level 2, proceed only if SOC_WARM_BOOT_MIM
     * is set */
    if ((stable_size == 0) || SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
        if (!SOC_WARM_BOOT_IS_MIM(unit)) {
            return BCM_E_NONE;
        }
    }

#if defined(BCM_TRIUMPH3_SUPPORT)    
    if (SOC_IS_TRIUMPH3(unit)) {
        mpls_mem =  MPLS_ENTRY_EXTDm;
        xlate_mem = VLAN_XLATE_EXTDm;
        isid_key_type = 0x18;
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
#if defined(BCM_TRIDENT3_SUPPORT)
       if (soc_feature(unit, soc_feature_base_valid)) {
           egr_xlate_mem = EGR_VLAN_XLATE_1_DOUBLEm;
           xlate_mem = VLAN_XLATE_1_DOUBLEm;
       } else
#endif
       {
           xlate_mem = VLAN_XLATEm;
       }
       isid_key_type = 0x2;
    }

    /* Allocate Chunk Memory buffer for various tables */
    mem_buf[0] = &mpls_entry_buf;
    rv = _bcm_chunk_memory_alloc(unit, mpls_mem, mem_buf[0]);
    if (BCM_FAILURE(rv) || (NULL == mpls_entry_buf)) {
       goto cleanup;
    }

    mem_buf[0] = &vlan_xlate_buf;
    rv = _bcm_chunk_memory_alloc(unit, xlate_mem, mem_buf[0]);
    if (BCM_FAILURE(rv) || (NULL == vlan_xlate_buf)) {
       goto cleanup;
    }

    mem_buf[0] = &egr_vlan_xlate_buf;
    rv = _bcm_chunk_memory_alloc(unit, egr_xlate_mem, mem_buf[0]);
    if (BCM_FAILURE(rv) || (NULL == egr_vlan_xlate_buf)) {
       goto cleanup;
    }

    mem_buf[0] = &source_vp_buf;
    rv = _bcm_chunk_memory_alloc(unit, SOURCE_VPm, mem_buf[0]);
    if (BCM_FAILURE(rv) || (NULL == source_vp_buf)) {
       goto cleanup;
    }

    mem_buf[0] = &source_trunk_map_buf;
    rv = _bcm_chunk_memory_alloc(unit, SOURCE_TRUNK_MAP_TABLEm, mem_buf[0]);
    if (BCM_FAILURE(rv) || (NULL == source_trunk_map_buf)) {
       goto cleanup;
    }

    
    /* Get all valid VFIs (VPNs) */
    index_min = soc_mem_index_min(unit, mpls_mem);
    index_max = soc_mem_index_max(unit, mpls_mem);
    for (i = index_min; i <= index_max; i++) {
        mem_entry[0] = &mpls_entry;
        rv = _bcm_chunk_get_mem_entry(unit, mpls_mem, i ,
                                      mpls_entry_buf, mem_entry[0]);
        if (BCM_FAILURE(rv) || (NULL == mpls_entry)) {
            goto cleanup;
        }

#if defined(BCM_TRIUMPH3_SUPPORT)    
        if (SOC_IS_TRIUMPH3(unit)) {
            if (soc_mem_field32_get(unit, mpls_mem, mpls_entry, VALID_0f) == 0 
            && soc_mem_field32_get(unit, mpls_mem, mpls_entry, VALID_1f) == 0) {
                continue;
            }
            
            if (soc_mem_field32_get(unit, mpls_mem, mpls_entry, KEY_TYPE_0f) 
                                            != isid_key_type) {
                /* MIM_ISID */
                continue;
            }
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
        {
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_base_valid)) {
                if (soc_mem_field32_get(unit, mpls_mem, mpls_entry, BASE_VALID_0f) != 3) {
                    continue;
                }
                if (soc_mem_field32_get(unit, mpls_mem, mpls_entry, BASE_VALID_1f) != 7) {
                    continue;
                }
            } else
#endif
            {
                if (soc_mem_field32_get(unit, mpls_mem, mpls_entry, VALIDf) == 0) {
                    continue;
                }
            }

#if defined(BCM_TRIDENT_SUPPORT)
            if (soc_feature(unit, soc_feature_mim_peer_sharing) &&
                (soc_mem_field32_get(unit, mpls_mem,
                                     mpls_entry, KEY_TYPEf) == 0x3)) {
                    /* MIM_ISID_SVP */
                    vp = soc_mem_field32_get(unit, mpls_mem,
                                             mpls_entry, MIM_ISID__SVPf);
                    MIM_INFO(unit)->port_info[vp].vfi_count++;
                    continue;
            }
#endif /* BCM_TRIDENT_SUPPORT */

            if (soc_mem_field32_get(unit, mpls_mem, mpls_entry, KEY_TYPEf) 
                                            != isid_key_type) {
                /* MIM_ISID */
                continue;
            }
        }
        vfi = soc_mem_field32_get(unit, mpls_mem, mpls_entry, MIM_ISID__VFIf);
        isid = soc_mem_field32_get(unit, mpls_mem, mpls_entry, MIM_ISID__ISIDf);
        if ((stable_size == 0) || SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
            rv = _bcm_vfi_alloc_with_id(unit, VFIm, _bcmVfiTypeMim, vfi);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        } else {
            if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
                /* VFI bitmap is recovered by virtual_init */
                continue;
            }
        }
        VPN_ISID(unit, vfi) = isid;

#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_MEM_FIELD_VALID(unit, VFIm, SD_TAG_MODEf)) {
            vfi_mem = VFIm;
            vfi_1_mem = VFI_ATTRS_1m;
        } else
#endif
        {
            vfi_mem = VFI_1m;
            vfi_1_mem = VFI_1m;
        }
        /* Retrieve TPID reference counts if applicable */
        rv = soc_mem_read(unit, vfi_mem,
                          MEM_BLOCK_ANY, vfi, vfi_entry);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        
        rv = soc_mem_read(unit, vfi_1_mem,
                          MEM_BLOCK_ANY, vfi, vfi_1_entry);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        if (soc_mem_field32_get(unit, vfi_mem, vfi_entry, SD_TAG_MODEf) == 1) {
            tpid_enable = soc_mem_field32_get(unit, vfi_1_mem, vfi_1_entry, 
                                                 TPID_ENABLEf);
            for (index = 0; index < 4; index++) {
                if (tpid_enable & (1 << index)) {
                    rv = _bcm_fb2_outer_tpid_tab_ref_count_add(unit, index, 1);
                    break;
                }
            }
        }  
        /* Retrieve flex stats for the VFI */
        rv = READ_VFIm(unit, MEM_BLOCK_ANY, vfi, vfi_entry);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        _bcm_tr2_mim_vpn_flex_stat_recover(unit, (void *)vfi_entry, vfi);
    }

    /* Get all virtual ports from VLAN_XLATE */
    index_min = soc_mem_index_min(unit, xlate_mem);
    index_max = soc_mem_index_max(unit, xlate_mem);
    for (i = index_min; i <= index_max; i++) {
        mem_entry[0] = &vlan_xlate;
        rv = _bcm_chunk_get_mem_entry(unit, xlate_mem, i ,
                                      vlan_xlate_buf, mem_entry[0]);
        if (BCM_FAILURE(rv) || (NULL == vlan_xlate)) {
            goto cleanup;
        }
 
#if defined(BCM_TRIUMPH3_SUPPORT)    
        if (SOC_IS_TRIUMPH3(unit)) {
            if (soc_mem_field32_get(unit, xlate_mem, vlan_xlate, VALID_0f) &&
              soc_mem_field32_get(unit, xlate_mem, vlan_xlate, VALID_1f) == 0) {
                continue;
            }
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
        {
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_base_valid)) {
                if (soc_mem_field32_get(unit, xlate_mem,
                             vlan_xlate, BASE_VALID_0f) != 3) {
                    continue;
                }
                if (soc_mem_field32_get(unit, xlate_mem,
                             vlan_xlate, BASE_VALID_1f) != 7) {
                    continue;
                }
            } else
#endif
            {
                if (soc_mem_field32_get(unit, xlate_mem,
                             vlan_xlate, VALIDf) == 0) {
                    continue;
                }
            }
        }

        if (soc_mem_field32_get(unit, xlate_mem, vlan_xlate, MPLS_ACTIONf) != 1) {
            continue; /* Entry is not for an SVP */
        }

#if defined(BCM_TRIUMPH3_SUPPORT)    
        if (SOC_IS_TRIUMPH3(unit)) {
        key_type = soc_mem_field32_get(unit, xlate_mem, vlan_xlate, 
                                       KEY_TYPE_0f);
            if ((key_type != TR3_VLXLT_X_HASH_KEY_TYPE_OVID) &&
                    (key_type != TR3_VLXLT_X_HASH_KEY_TYPE_IVID_OVID)) {
                continue;
            }
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
        {
            key_type = soc_mem_field32_get(unit, xlate_mem, vlan_xlate, 
                                              KEY_TYPEf);
            if ((key_type != TR_VLXLT_HASH_KEY_TYPE_OVID) &&
                    (key_type != TR_VLXLT_HASH_KEY_TYPE_IVID_OVID)) {
                continue;
            }
        }

        vp = soc_mem_field32_get(unit, xlate_mem, vlan_xlate, SOURCE_VPf);
        if ((stable_size == 0) || SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
            vp_info.flags &= ~(_BCM_VP_INFO_NETWORK_PORT);
            rv = _bcm_vp_used_set(unit, vp, vp_info);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        } else {
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
                /* VFI bitmap is recovered by virtual_init */
                continue;
            }
        }
        switch (key_type) {
        case TR_VLXLT_HASH_KEY_TYPE_OVID:
#if defined(BCM_TRIUMPH3_SUPPORT)    
        case TR3_VLXLT_X_HASH_KEY_TYPE_OVID:
#endif /* BCM_TRIUMPH3_SUPPORT */

            MIM_INFO(unit)->port_info[vp].flags |= 
                                     _BCM_MIM_PORT_TYPE_ACCESS_PORT_VLAN;
            MIM_INFO(unit)->port_info[vp].match_vlan = 
                soc_mem_field32_get(unit, xlate_mem, vlan_xlate, OVIDf);
            _bcm_tr2_mim_port_match_count_adjust(unit, vp, 1);
            break;
        case TR_VLXLT_HASH_KEY_TYPE_IVID_OVID:
#if defined(BCM_TRIUMPH3_SUPPORT)    
        case TR3_VLXLT_X_HASH_KEY_TYPE_IVID_OVID:
#endif /* BCM_TRIUMPH3_SUPPORT */

            MIM_INFO(unit)->port_info[vp].flags |= 
                             _BCM_MIM_PORT_TYPE_ACCESS_PORT_VLAN_STACKED;
            MIM_INFO(unit)->port_info[vp].match_vlan = 
                soc_mem_field32_get(unit, xlate_mem, vlan_xlate, OVIDf);
            MIM_INFO(unit)->port_info[vp].match_inner_vlan = 
                soc_mem_field32_get(unit, xlate_mem, vlan_xlate, IVIDf);
            _bcm_tr2_mim_port_match_count_adjust(unit, vp, 1);
            break;
            /* coverity[dead_error_begin] */
        default:
            break; /* Should never happen */
        }
        /* Recover TPID reference counts */
        mem_entry[0] = &source_vp;
        rv = _bcm_chunk_get_mem_entry(unit, SOURCE_VPm, vp ,
                                      source_vp_buf, mem_entry[0]);
        if (BCM_FAILURE(rv) || (NULL == source_vp)) {
            goto cleanup;
        }

        rv = _bcm_tr2_mim_source_vp_tpid_recover(unit, source_vp, vp);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        /* Recover port flex stats */
        _bcm_tr2_mim_port_flex_stat_recover(unit, source_vp, vp);
        /* Recover other port data */
        rv = _bcm_tr2_mim_port_associated_data_recover(unit, vp, stable_size);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    }

    /* Get all virtual ports from MPLS_ENTRY */
    index_min = soc_mem_index_min(unit, mpls_mem);
    index_max = soc_mem_index_max(unit, mpls_mem);
    for (i = index_min; i <= index_max; i++) {
        mem_entry[0] = &mpls_entry;
        rv = _bcm_chunk_get_mem_entry(unit, mpls_mem, i ,
                                      mpls_entry_buf, mem_entry[0]);
        if (BCM_FAILURE(rv) || (NULL == mpls_entry)) {
            goto cleanup;
        }

#if defined(BCM_TRIUMPH3_SUPPORT)    
        if (SOC_IS_TRIUMPH3(unit)) {
            if (soc_mem_field32_get(unit, mpls_mem, mpls_entry, VALID_0f) == 0 
            && soc_mem_field32_get(unit, mpls_mem, mpls_entry, VALID_1f) == 0) {
                continue;
            }
            key_type = soc_mem_field32_get(unit, mpls_mem, mpls_entry, 
                                           KEY_TYPE_0f);
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
        {
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_base_valid)) {
                if (soc_mem_field32_get(unit, mpls_mem, mpls_entry, BASE_VALID_0f) != 3) {
                    continue;
                }
                if (soc_mem_field32_get(unit, mpls_mem, mpls_entry, BASE_VALID_1f) != 7) {
                    continue;
                }
            } else
#endif
            {
                if (soc_mem_field32_get(unit, mpls_mem, mpls_entry, VALIDf) == 0) {
                    continue;
                }
            }
            key_type = soc_mem_field32_get(unit, mpls_mem, mpls_entry, 
                                                           KEY_TYPEf);
        }

        if ((soc_mem_field32_get(unit, mpls_mem, mpls_entry, MPLS_ACTION_IF_BOSf) 
                                         != 1) && (key_type == 0)) {
            continue; /* Not an SVP */
        }
        switch (key_type) {
        case 0: /* MPLS Label */
#if defined(BCM_TRIUMPH3_SUPPORT)
        case 17:
        case 19:
#endif /* BCM_TRIUMPH3_SUPPORT */
            vp = soc_mem_field32_get(unit, mpls_mem, mpls_entry, SOURCE_VPf);
            if ((stable_size == 0) || SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
                vp_info.flags &= ~(_BCM_VP_INFO_NETWORK_PORT);
                rv = _bcm_vp_used_set(unit, vp, vp_info);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }
            } else {
                if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
                    /* VFI bitmap is recovered by virtual_init */
                    continue;
                }
            }
            MIM_INFO(unit)->port_info[vp].flags |= 
                             _BCM_MIM_PORT_TYPE_ACCESS_LABEL;
            MIM_INFO(unit)->port_info[vp].match_label = 
                soc_mem_field32_get(unit, mpls_mem, mpls_entry, MPLS_LABELf);
            break;
        case 1: /* MIM NVP */
#if defined(BCM_TRIUMPH3_SUPPORT)
        case 23:
#endif /* BCM_TRIUMPH3_SUPPORT */
            vp = soc_mem_field32_get(unit, mpls_mem, mpls_entry, MIM_NVP__SVPf);
            if ((stable_size != 0) && !SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
                if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
                    /* VFI bitmap is recovered by virtual_init */
                    continue;
                }
            }
            soc_mem_mac_addr_get(unit, mpls_mem, mpls_entry, MIM_NVP__BMACSAf, 
                             MIM_INFO(unit)->port_info[vp].match_tunnel_srcmac);
            MIM_INFO(unit)->port_info[vp].match_tunnel_vlan = 
                soc_mem_field32_get(unit, mpls_mem, mpls_entry, MIM_NVP__BVIDf);

            if (SOC_MEM_FIELD_VALID(unit, mpls_mem, MIM_NVP__ISID_LOOKUP_TYPEf)) {
                if (soc_mem_field32_get(unit, mpls_mem, mpls_entry, 
                                                MIM_NVP__ISID_LOOKUP_TYPEf)) {
                    MIM_INFO(unit)->port_info[vp].flags |= _BCM_MIM_PORT_TYPE_PEER;
                    vp_info.flags &= ~(_BCM_VP_INFO_NETWORK_PORT);
                } else {
                    MIM_INFO(unit)->port_info[vp].flags |= 
                                                        _BCM_MIM_PORT_TYPE_NETWORK;
                    vp_info.flags |= _BCM_VP_INFO_NETWORK_PORT;
                }
            }
            rv = _bcm_vp_used_set(unit, vp, vp_info);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            break;
        default:
            continue; /* Handled elsewhere */
        }
        /* Recover TPID reference counts */
        mem_entry[0] = &source_vp;
        rv = _bcm_chunk_get_mem_entry(unit, SOURCE_VPm, vp ,
                                      source_vp_buf, mem_entry[0]);
        if (BCM_FAILURE(rv) || (NULL == source_vp)) {
            goto cleanup;
        }

        rv = _bcm_tr2_mim_source_vp_tpid_recover(unit, source_vp, vp);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        /* Recover port flex stats */
        _bcm_tr2_mim_port_flex_stat_recover(unit, source_vp, vp);
        /* Recover other port data */
        rv = _bcm_tr2_mim_port_associated_data_recover(unit, vp, stable_size);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    }

    /* Get all virtual ports from SOURCE_TRUNK_MAP */
    index_min = soc_mem_index_min(unit, SOURCE_TRUNK_MAP_TABLEm);
    index_max = soc_mem_index_max(unit, SOURCE_TRUNK_MAP_TABLEm);
    for (i = index_min; i <= index_max; i++) {
        bcm_module_t modid;
        bcm_port_t port;
        bcm_trunk_t tgid;
        mem_entry[0] = &source_trunk_map;
        rv = _bcm_chunk_get_mem_entry(unit, SOURCE_TRUNK_MAP_TABLEm, i ,
                           source_trunk_map_buf, mem_entry[0]);
        if (BCM_FAILURE(rv) || (NULL == source_trunk_map)) {
            goto cleanup;
        }

        vp = soc_SOURCE_TRUNK_MAP_TABLEm_field32_get(unit, source_trunk_map, 
                                                     SOURCE_VPf);
        if (vp == 0) {
            continue;
        }
        /* If VP has already been discovered, no need to proceed further */
        /* This can happen for the trunk access case */
        if (MIM_INFO(unit)->port_info[vp].flags != 0) {
            continue;
        }
        if ((stable_size == 0) || SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
            vp_info.flags &= ~(_BCM_VP_INFO_NETWORK_PORT);
            rv = _bcm_vp_used_set(unit, vp, vp_info);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        } else {
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
                /* VFI bitmap is recovered by virtual_init */
                continue;
            }
        }
        if (soc_feature(unit, soc_feature_vp_lag)) {
            modid = SOC_IS_ENDURO(unit) ? i / 64 : i / SOC_PORT_ADDR_MAX(unit);
            port = SOC_IS_ENDURO(unit) ? i % 64 : i % SOC_PORT_ADDR_MAX(unit);
            rv = _bcm_esw_trunk_port_property_get(unit, modid, port, &tgid);
            if (BCM_FAILURE(rv) && (tgid != -1)) {
                goto cleanup;
            }
            if (tgid == -1) {
                MIM_INFO(unit)->port_info[vp].flags |=
                    _BCM_MIM_PORT_TYPE_ACCESS_PORT;
                MIM_INFO(unit)->port_info[vp].index = i;
            } else {
                MIM_INFO(unit)->port_info[vp].flags |=
                    _BCM_MIM_PORT_TYPE_ACCESS_PORT_TRUNK;
                MIM_INFO(unit)->port_info[vp].index = tgid;
            }
            /* Recover TPID reference counts */
            mem_entry[0] = &source_vp;
            rv = _bcm_chunk_get_mem_entry(unit, SOURCE_VPm, vp ,
                                          source_vp_buf, mem_entry[0]);
            if (BCM_FAILURE(rv) || (NULL == source_vp)) {
                goto cleanup;
            }

            rv = _bcm_tr2_mim_source_vp_tpid_recover(unit, source_vp, vp);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
        /* Recover port flex stats */
        _bcm_tr2_mim_port_flex_stat_recover(unit, source_vp, vp);
        /* Recover other port data */
        rv = _bcm_tr2_mim_port_associated_data_recover(unit, vp, stable_size);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    } 

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_mim_decoupled_mode)) {
        soc_mem_t evxlt_mem;

        /* first traverse egr_vlan_xlate_1_double */
        evxlt_mem = EGR_VLAN_XLATE_1_DOUBLEm;
        index_min = soc_mem_index_min(unit, evxlt_mem);
        index_max = soc_mem_index_max(unit, evxlt_mem);
        for (i = index_min; i <= index_max; i++) {
            mem_entry[0] = &egr_vlan_xlate;
            rv = _bcm_chunk_get_mem_entry(unit, evxlt_mem, i ,
                                egr_vlan_xlate_buf, mem_entry[0]);
            if (BCM_FAILURE(rv) || (NULL == egr_vlan_xlate)) {
                goto cleanup;
            }
            if (soc_mem_field32_get(unit, evxlt_mem,
                     egr_vlan_xlate, BASE_VALID_0f) != 3) {
                continue;
            }
            if (soc_mem_field32_get(unit, evxlt_mem,
                     egr_vlan_xlate, BASE_VALID_1f) != 7) {
                continue;
            }
            if (soc_mem_field32_get(unit, evxlt_mem, egr_vlan_xlate,
                                            KEY_TYPEf) != 13) {
                continue; /* Not DVP_VFI_ASSIGN_ISID */
            }
            rv = _bcm_td3_mim_flex_evxlt_tpid_recover(unit, egr_vlan_xlate,
                      FALSE);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }

        /* now traverse egr_vlan_xlate_1_single */
        evxlt_mem = EGR_VLAN_XLATE_1_SINGLEm;
        index_min = soc_mem_index_min(unit, evxlt_mem);
        index_max = soc_mem_index_max(unit, evxlt_mem);
        for (i = index_min; i <= index_max; i++) {
            mem_entry[0] = &egr_vlan_xlate;
            rv = _bcm_chunk_get_mem_entry(unit, evxlt_mem, i ,
                                egr_vlan_xlate_buf, mem_entry[0]);
            if (BCM_FAILURE(rv) || (NULL == egr_vlan_xlate)) {
                goto cleanup;
            }
            if (soc_mem_field32_get(unit, evxlt_mem,
                     egr_vlan_xlate, BASE_VALIDf) != 1) {
                continue;
            }
           if (soc_mem_field32_get(unit, evxlt_mem, egr_vlan_xlate,
                                            KEY_TYPEf) != 19) {
               continue; /* Not VFI_ASSIGN_ISID */
           }
           rv = _bcm_td3_mim_flex_evxlt_tpid_recover(unit, egr_vlan_xlate,
                     TRUE);
           if (BCM_FAILURE(rv)) {
               goto cleanup;
           }
       }
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
        /* Recover TPID reference counts from EGR_VLAN_XLATE */
        index_min = soc_mem_index_min(unit, egr_xlate_mem);
        index_max = soc_mem_index_max(unit, egr_xlate_mem);
        for (i = index_min; i <= index_max; i++) {
            mem_entry[0] = &egr_vlan_xlate;
            rv = _bcm_chunk_get_mem_entry(unit, egr_xlate_mem, i ,
                                egr_vlan_xlate_buf, mem_entry[0]);
            if (BCM_FAILURE(rv) || (NULL == egr_vlan_xlate)) {
                goto cleanup;
            }
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_base_valid)) {
                if (soc_mem_field32_get(unit, egr_xlate_mem,
                             egr_vlan_xlate, BASE_VALID_0f) != 3) {
                    continue;
                }
                if (soc_mem_field32_get(unit, egr_xlate_mem,
                             egr_vlan_xlate, BASE_VALID_1f) != 7) {
                    continue;
                }
            } else
#endif
            {
                if (soc_mem_field32_get(unit, egr_xlate_mem,
                             egr_vlan_xlate, VALIDf) == 0) {
                    continue;
                }
            }
#if defined(BCM_TRIUMPH3_SUPPORT)
            if (SOC_IS_TRIUMPH3(unit)) {
                if (soc_mem_field32_get(unit, egr_xlate_mem, egr_vlan_xlate,
                                                    KEY_TYPEf) != 3) {
                    continue; /* Not MIM_ISID_DVP */
                }
            } else
#endif /* BCM_TRIUMPH3_SUPPORT */
            {
#if defined(BCM_TRIDENT3_SUPPORT)
                if (soc_feature(unit, soc_feature_base_valid)) {
                    if (soc_mem_field32_get(unit, egr_xlate_mem, egr_vlan_xlate,
                                                        DATA_TYPEf) != 4) {
                        continue; /* Not ISID_DVP_XLATE */
                    }
                } else
#endif
                {
                    if (soc_mem_field32_get(unit, egr_xlate_mem, egr_vlan_xlate,
                                                        ENTRY_TYPEf) != 4) {
                        continue; /* Not ISID_DVP_XLATE */
                    }
                }

            }
            rv = _bcm_tr2_mim_egr_vxlt_tpid_recover(unit, egr_vlan_xlate);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    }

    /* Recover L2 scache */
    rv = _bcm_tr2_mim_wb_recover(unit);

cleanup:
    if (mpls_entry_buf) {
        soc_cm_sfree(unit, mpls_entry_buf);
    }
    if (vlan_xlate_buf) {
        soc_cm_sfree(unit, vlan_xlate_buf);
    }
    if (egr_vlan_xlate_buf) {
        soc_cm_sfree(unit, egr_vlan_xlate_buf);
    }
    if (source_vp_buf) {
        soc_cm_sfree(unit, source_vp_buf);
    }
    if (source_trunk_map_buf) {
        soc_cm_sfree(unit, source_trunk_map_buf);
    }
    return rv;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcm_mim_init
 * Purpose:
 *      Initialize the MIM software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr2_mim_init(int unit)
{
    int num_vfi, num_vp, num_intf, rv = BCM_E_NONE;
    _bcm_tr2_mim_bookkeeping_t *mim_info = MIM_INFO(unit);


    if (!L3_INFO(unit)->l3_initialized) {
        LOG_INFO(BSL_LS_BCM_L3,
                 (BSL_META_U(unit,
                             "L3 module must be initialized first\n")));
        return BCM_E_NONE;
    }

#if defined(BCM_TRIDENT3_SUPPORT)
   /* the feature is first set by cancun version binding. Further
    * qualified here by the config variable.
    */
    if (soc_property_get(unit, spn_MIM_DECOUPLED_MODE, 0) &&
        soc_feature(unit, soc_feature_mim_decoupled_mode)) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit, EGR_FLEX_CONFIGr,
                                                REG_PORT_ANY,
                                                FLEX_MIM_ENABLEf,
                                                1));
    } else {
        SOC_FEATURE_CLEAR(unit, soc_feature_mim_decoupled_mode);
    }
#endif /* defined(BCM_TRIDENT3_SUPPORT) */

    

    if (mim_initialized[unit]) {
        BCM_IF_ERROR_RETURN(bcm_tr2_mim_detach(unit));
    }
    num_vfi = soc_mem_index_count(unit, VFIm);
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    num_intf = soc_mem_index_count(unit, EGR_L3_INTFm);

    sal_memset(mim_info, 0, sizeof(_bcm_tr2_mim_bookkeeping_t));

    if (mim_info->vpn_info == NULL) {
        mim_info->vpn_info = sal_alloc(sizeof(_bcm_tr2_vpn_info_t) 
                                       * num_vfi, "mim_vpn_info");
        if (mim_info->vpn_info == NULL) {
            _bcm_tr2_mim_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(mim_info->vpn_info, 0, sizeof(_bcm_tr2_vpn_info_t) * num_vfi);

    if (mim_info->port_info == NULL) {
        mim_info->port_info = sal_alloc(sizeof(_bcm_tr2_mim_port_info_t) 
                                       * num_vp, "mim_port_info");
        if (mim_info->port_info == NULL) {
            _bcm_tr2_mim_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(mim_info->port_info, 0, sizeof(_bcm_tr2_mim_port_info_t) * num_vp);

    if (_mim_mutex[unit] == NULL) {
        _mim_mutex[unit] = sal_mutex_create("mim mutex");
        if (_mim_mutex[unit] == NULL) {
            _bcm_tr2_mim_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }

    if (NULL == mim_info->intf_bitmap) {
        mim_info->intf_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(num_intf), "intf_bitmap");
        if (mim_info->intf_bitmap == NULL) {
            _bcm_tr2_mim_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(mim_info->intf_bitmap, 0, SHR_BITALLOCSIZE(num_intf));    

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        /* Warm Boot recovery */
        rv = _bcm_tr2_mim_reinit(unit); 
    } else {
        rv = _bcm_tr2_mim_wb_alloc(unit);
    }

    if (BCM_FAILURE(rv)) {
        _bcm_tr2_mim_free_resources(unit);
        return rv;
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    if(!SOC_WARM_BOOT(unit)) {
        /* Enable MiM */
        rv = bcm_tr2_mim_enable(unit, TRUE);
        if (BCM_FAILURE(rv)) {
            _bcm_tr2_mim_free_resources(unit);
            return rv;
        }
    }

    mim_initialized[unit] = TRUE;
    return rv;
}

#if defined(BCM_TRIDENT3_SUPPORT)

STATIC int
_bcm_td3_mim_flex_evxlt_tag_action_add(int unit, bcm_mim_port_t *mim_port,
                                     bcm_mim_vpn_config_t *info,
                                     uint32 *evxlt_entry)
{
    int rv = BCM_E_NONE, tpid_index = -1;
    int sd_default_action = 0, sd_tag_action = 0;
    soc_mem_t evxlt_mem;
    uint32 profile_index = 0;
    int hw_idx;

    if (mim_port != NULL) {
        evxlt_mem = EGR_VLAN_XLATE_1_DOUBLEm;

        if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_ADD) {
            if (!BCM_VLAN_VALID(mim_port->egress_service_vlan)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                                        DVP_VFI_ASSIGN_ISID__OVIDf,
                                        mim_port->egress_service_vlan);
            sd_default_action = 0x1; /* ADD */
        }
        if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TPID_REPLACE) {
            if (!BCM_VLAN_VALID(mim_port->egress_service_vlan)) {
                return BCM_E_PARAM;
            }
            /* REPLACE_VID_TPID */
            soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                                        DVP_VFI_ASSIGN_ISID__OVIDf,
                                        mim_port->egress_service_vlan);
            sd_tag_action = 0x1; /* Replace Vid and TPID */
        } else if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_REPLACE) {
            if (!BCM_VLAN_VALID(mim_port->egress_service_vlan)) {
                return BCM_E_PARAM;
            }
            /* REPLACE_VID_ONLY */
            soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                                        DVP_VFI_ASSIGN_ISID__OVIDf,
                                        mim_port->egress_service_vlan);
            sd_tag_action = 0x2; /* Replace Vid only */
        } else if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_DELETE) {
            sd_tag_action = 0x3; /* DELETE */
        } else if (mim_port->flags &
                   BCM_MIM_PORT_EGRESS_SERVICE_VLAN_PRI_TPID_REPLACE) {
            soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                                DVP_VFI_ASSIGN_ISID__OPRIf,
                                mim_port->egress_service_pri);
            soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                                DVP_VFI_ASSIGN_ISID__OCFIf,
                                mim_port->egress_service_cfi);
            soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                                DVP_VFI_ASSIGN_ISID__OPRI_OCFI_SELf,
                                _MIM_OPRI_SEL_FROM_TBL);
            soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                                DVP_VFI_ASSIGN_ISID__OVIDf,
                                mim_port->egress_service_vlan);
            sd_tag_action = 0x4; /* REPLACE vid/pri/tpid */
        } else if (mim_port->flags &
                   BCM_MIM_PORT_EGRESS_SERVICE_VLAN_PRI_REPLACE) {
            soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                                DVP_VFI_ASSIGN_ISID__OPRIf,
                                mim_port->egress_service_pri);
            soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                                DVP_VFI_ASSIGN_ISID__OCFIf,
                                mim_port->egress_service_cfi);
            soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                                DVP_VFI_ASSIGN_ISID__OPRI_OCFI_SELf,
                                _MIM_OPRI_SEL_FROM_TBL);
            soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                                DVP_VFI_ASSIGN_ISID__OVIDf,
                                mim_port->egress_service_vlan);
            sd_tag_action = 0x5; /* REPLACE vid/pri */
        } else if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_PRI_REPLACE) {
            soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                                DVP_VFI_ASSIGN_ISID__OPRIf,
                                mim_port->egress_service_pri);
            soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                                DVP_VFI_ASSIGN_ISID__OCFIf,
                                mim_port->egress_service_cfi);
            soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                                DVP_VFI_ASSIGN_ISID__OPRI_OCFI_SELf,
                                _MIM_OPRI_SEL_FROM_TBL);
            sd_tag_action = 0x6;
        } else if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_TPID_REPLACE) {
            sd_tag_action = 0x7;
        }
        BCM_IF_ERROR_RETURN(_bcm_td3_sd_tag_action_profile_get(unit,
            sd_tag_action, sd_default_action, &profile_index));
        soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                  DVP_VFI_ASSIGN_ISID__TAG_ACTION_PROFILE_PTRf, profile_index);

        if ((mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_ADD) ||
            (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_TPID_REPLACE) ||
            (mim_port->flags &
             BCM_MIM_PORT_EGRESS_SERVICE_VLAN_PRI_TPID_REPLACE) ||
            (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TPID_REPLACE)) {
            /* TPID value is used */
            rv = _bcm_fb2_outer_tpid_entry_add(unit,
                                           mim_port->egress_service_tpid,
                                           &tpid_index);
            BCM_IF_ERROR_RETURN(rv);
            soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                                        DVP_VFI_ASSIGN_ISID__OUTER_TPID_INDEXf,
                                        tpid_index);
        }
    } else if (info != NULL) {
        evxlt_mem = EGR_VLAN_XLATE_1_SINGLEm;

        if (info->flags & BCM_MIM_VPN_QOS_SDTAG_NO_ACTION) {
            sd_tag_action = 0;
        } else if (info->flags & BCM_MIM_VPN_QOS_SDTAG_FIX_PRIORITY) {
            /* from_table: Use {PCP,DE} in the table */
            soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                    VFI_ASSIGN_ISID__OPRI_OCFI_SELf, 1);
            soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                    VFI_ASSIGN_ISID__OPRIf, info->int_pri & 0x7);
            soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                    VFI_ASSIGN_ISID__OCFIf, (info->int_pri >> 0x3) & 0x1);
            sd_tag_action = 6;
        } else if (info->flags & BCM_MIM_VPN_QOS_SDTAG_MAP_INT_PRIORITY) {
            rv = _bcm_tr2_qos_id2idx(unit, info->qos_map_id, &hw_idx);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

            /* from_phb: mapping pointer */
            soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                        VFI_ASSIGN_ISID__OPRI_OCFI_SELf, 2);
            soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                        VFI_ASSIGN_ISID__OPRI_OCFI_MAPPING_PTRf, hw_idx);
            sd_tag_action = 6;
        } else if (info->flags & BCM_MIM_VPN_EGRESS_SERVICE_TPID_REPLACE) {
            rv = _bcm_fb2_outer_tpid_entry_add(unit,
                                               info->egress_service_tpid,
                                               &tpid_index);
            BCM_IF_ERROR_RETURN(rv);
            soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                                            VFI_ASSIGN_ISID__OUTER_TPID_INDEXf,
                                            tpid_index);

            /* REPLACE_TPID_ONLY */
            sd_tag_action = 0x7;
        }
        if (sd_tag_action) {
            BCM_IF_ERROR_RETURN(_bcm_td3_sd_tag_action_profile_get(unit,
                 sd_tag_action, sd_default_action, &profile_index));
        }
        soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                 VFI_ASSIGN_ISID__TAG_ACTION_PROFILE_PTRf, profile_index);
    }

    return rv;
}

STATIC int
_bcm_td3_mim_flex_evxlt_tag_action_get (int unit, bcm_mim_port_t *mim_port,
                                     bcm_mim_vpn_config_t *info,
                                     uint32 *evxlt_entry,
                                     uint32 *nh_entry)
{
    uint32 profile_index;
    int action_present;
    int action_not_present;
    soc_mem_t evxlt_mem;
    int tpid_idx = 0;
    int ovid = 0;
    int pri = 0;
    int cfi = 0;
    int opri_sel = 0;
    int hw_idx = 0;

    /* Populate the API structure */
    if (info != NULL) {
        evxlt_mem = EGR_VLAN_XLATE_1_SINGLEm;
        profile_index = soc_mem_field32_get(unit, evxlt_mem, evxlt_entry,
                                   VFI_ASSIGN_ISID__TAG_ACTION_PROFILE_PTRf);
        tpid_idx = soc_mem_field32_get(unit, evxlt_mem, evxlt_entry,
                         VFI_ASSIGN_ISID__OUTER_TPID_INDEXf);
        ovid = soc_mem_field32_get(unit, evxlt_mem, evxlt_entry,
                         VFI_ASSIGN_ISID__OVIDf);
        cfi  = soc_mem_field32_get(unit, evxlt_mem, evxlt_entry,
                                VFI_ASSIGN_ISID__OCFIf);
        pri  = soc_mem_field32_get(unit, evxlt_mem, evxlt_entry,
                                VFI_ASSIGN_ISID__OPRIf);
        opri_sel = soc_mem_field32_get(unit, evxlt_mem, evxlt_entry,
                    VFI_ASSIGN_ISID__OPRI_OCFI_SELf);

        (void)_bcm_td3_sd_tag_action_get(unit, profile_index,
                               &action_present, &action_not_present);
        if (action_present == 6) {
            if (opri_sel == 1) {
                info->int_pri = pri | (cfi << 3);
                info->flags |= BCM_MIM_VPN_QOS_SDTAG_FIX_PRIORITY;
            } else if (opri_sel == 2) {
                hw_idx = soc_mem_field32_get(unit, evxlt_mem, evxlt_entry,
                        VFI_ASSIGN_ISID__OPRI_OCFI_MAPPING_PTRf);
                BCM_IF_ERROR_RETURN(_bcm_tr2_qos_idx2id(unit, hw_idx,
                         _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS, &info->qos_map_id));
                info->flags |= BCM_MIM_VPN_QOS_SDTAG_MAP_INT_PRIORITY;
            }
        } else if (action_present == 0x7) {
            tpid_idx = soc_mem_field32_get(unit, evxlt_mem, evxlt_entry,
                                    VFI_ASSIGN_ISID__OUTER_TPID_INDEXf);
            _bcm_fb2_outer_tpid_entry_get(unit, &info->egress_service_tpid,
                                          tpid_idx);
            info->flags |= BCM_MIM_VPN_EGRESS_SERVICE_TPID_REPLACE;
        }
    } else if (mim_port != NULL) {
        if (evxlt_entry != NULL) {
            evxlt_mem = EGR_VLAN_XLATE_1_DOUBLEm;

            profile_index = soc_mem_field32_get(unit, evxlt_mem, evxlt_entry,
                               DVP_VFI_ASSIGN_ISID__TAG_ACTION_PROFILE_PTRf);
            tpid_idx = soc_mem_field32_get(unit, evxlt_mem, evxlt_entry,
                         DVP_VFI_ASSIGN_ISID__OUTER_TPID_INDEXf);
            ovid = soc_mem_field32_get(unit, evxlt_mem, evxlt_entry,
                         DVP_VFI_ASSIGN_ISID__OVIDf);
            cfi  = soc_mem_field32_get(unit, evxlt_mem, evxlt_entry,
                                DVP_VFI_ASSIGN_ISID__OCFIf);
            pri  = soc_mem_field32_get(unit, evxlt_mem, evxlt_entry,
                                DVP_VFI_ASSIGN_ISID__OPRIf);
        } else if (nh_entry != NULL) {
            profile_index = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                          nh_entry, L2_OTAG__TAG_ACTION_PROFILE_PTRf);
            tpid_idx = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                          nh_entry, L2_OTAG__TPID_INDEXf);
            ovid = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                          nh_entry, L2_OTAG__VIDf);
            cfi  = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, nh_entry,
                                L2_OTAG__DEf);
            pri  = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, nh_entry,
                                L2_OTAG__PCPf);
        } else {
            return BCM_E_INTERNAL;
        }
        (void)_bcm_td3_sd_tag_action_get(unit, profile_index,
                               &action_present, &action_not_present);
        if (action_present) {
            mim_port->flags |= BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TAGGED;
        }

        if ((action_not_present == 0x1) || (action_present == 0x1)) {
            /* If SD tag action is ADD or REPLACE_VID_TPID, the tpid
             * index of the entry is valid. Get the tpid index for later.
             */
            mim_port->egress_service_vlan = ovid;

           if (action_not_present) {
               mim_port->flags |= BCM_MIM_PORT_EGRESS_SERVICE_VLAN_ADD;
               mim_port->flags |= BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TAGGED;
           }
           if (action_present) {
               mim_port->flags |= BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TPID_REPLACE;
           }
           _bcm_fb2_outer_tpid_entry_get(unit, &mim_port->egress_service_tpid,
                 tpid_idx);
        } else if (action_present == 0x2) { /* REPLACE_VID_ONLY */
            mim_port->flags |= BCM_MIM_PORT_EGRESS_SERVICE_VLAN_REPLACE;
            mim_port->egress_service_vlan = ovid;
        } else if (action_present == 0x3) { /* DELETE */
            mim_port->flags |= BCM_MIM_PORT_EGRESS_SERVICE_VLAN_DELETE;
        } else if (action_present == 0x4) {
            mim_port->flags |= BCM_MIM_PORT_EGRESS_SERVICE_VLAN_PRI_TPID_REPLACE;
            mim_port->egress_service_vlan = ovid;
            mim_port->egress_service_pri = pri;
            mim_port->egress_service_cfi = cfi;
            _bcm_fb2_outer_tpid_entry_get(unit, &mim_port->egress_service_tpid,
                 tpid_idx);
        } else if (action_present == 0x5) {
            mim_port->flags |= BCM_MIM_PORT_EGRESS_SERVICE_VLAN_PRI_REPLACE;
            mim_port->egress_service_vlan = ovid;
            mim_port->egress_service_pri = pri;
            mim_port->egress_service_cfi = cfi;
        } else if (action_present == 0x6) {
            mim_port->flags |= BCM_MIM_PORT_EGRESS_SERVICE_PRI_REPLACE;
            mim_port->egress_service_pri = pri;
            mim_port->egress_service_cfi = cfi;
        } else if (action_present == 0x7) {
            mim_port->flags |= BCM_MIM_PORT_EGRESS_SERVICE_TPID_REPLACE;
            _bcm_fb2_outer_tpid_entry_get(unit, &mim_port->egress_service_tpid,
                 tpid_idx);
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_td3_mim_flex_evxlt_tag_action_delete (int unit,
                                           uint32 profile_index,
                                           int tpid_idx)
{
    int action_present;
    int action_not_present;

    if (!profile_index) {
        return BCM_E_NONE;
    }
    (void)_bcm_td3_sd_tag_action_get(unit, profile_index,
                           &action_present, &action_not_present);

    if ((action_not_present == 0x1) || (action_present == 0x1) ||
        (action_present == 0x4) ||
        (action_present == 0x7)) {
        /* Delete old TPID */
        if (tpid_idx != -1) {
            (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_idx);
        }
    }
    BCM_IF_ERROR_RETURN(_bcm_trx_egr_vlan_action_profile_entry_delete(unit,
                  profile_index));
    return BCM_E_NONE;
}

STATIC int
_bcm_td3_mim_untagged_payload_vlan_assign(int unit,
             bcm_mim_vpn_config_t *info, bcm_mim_vpn_config_t *old_info,
             bcm_mim_port_t *mim_port, int vp,
             int vfi, bcm_vlan_t vlan)
{
    uint32 vxlt_entry[SOC_MAX_MEM_WORDS];
    soc_mem_t vxlt_mem;
    bcm_vlan_action_set_t action;
    uint32 profile_idx = 0;
    int index;
    int rv;
    int isid;

    if (info == NULL && mim_port == NULL) {
        return BCM_E_INTERNAL;
    }

    sal_memset(vxlt_entry, 0, sizeof(vxlt_entry));
    /* ISID -> vlan */
    if (info != NULL) {
        vxlt_mem = VLAN_XLATE_2_SINGLEm;
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, BASE_VALIDf, 1);
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, KEY_TYPEf, 24);
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, DATA_TYPEf, 20);

        if (info->flags & BCM_MIM_VPN_REPLACE) {
            soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, MIM_ISID__ISIDf,
                            old_info->lookup_id);
            rv = soc_mem_search(unit, vxlt_mem, MEM_BLOCK_ANY, &index,
                            vxlt_entry, vxlt_entry, 0);
            BCM_IF_ERROR_RETURN(rv);
            profile_idx = soc_mem_field32_get(unit, vxlt_mem, vxlt_entry,
                        MIM_ISID__ING_TAG_ACTION_PROFILE_IDXf);
        }
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, MIM_ISID__ISIDf,
                            info->lookup_id);
        /* if vlan is not valid, don't perform vlan action, but
         * still need to add vxlt entry, otherwise tunnel miss hit
         */
        if (info->default_vlan) {
            if (!profile_idx) {
                bcm_vlan_action_set_t_init(&action);
                action.ut_outer           = bcmVlanActionAdd;
                action.ut_outer_pkt_prio  = bcmVlanActionAdd;
                action.ut_outer_cfi       = bcmVlanActionAdd;
                action.ot_outer           = bcmVlanActionReplace;
                action.it_outer           = bcmVlanActionAdd;
                action.dt_outer           = bcmVlanActionReplace;
                rv = _bcm_trx_vlan_action_profile_entry_add(unit, &action,
                              &profile_idx);
                BCM_IF_ERROR_RETURN(rv);
            }
            soc_mem_field32_set(unit, vxlt_mem, vxlt_entry,
                        MIM_ISID__ING_TAG_ACTION_PROFILE_IDXf, profile_idx);
            soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, MIM_ISID__OVIDf,
                        info->default_vlan & 0xfff);
            soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, MIM_ISID__OCFIf,
                        (info->default_vlan >> 12) & 0x1);
            soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, MIM_ISID__OPRIf,
                        (info->default_vlan >> 13) & 0x7);
        } else {
            soc_mem_field32_set(unit, vxlt_mem, vxlt_entry,
                        MIM_ISID__ING_TAG_ACTION_PROFILE_IDXf, 0);
            soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, MIM_ISID__OVIDf,
                        info->default_vlan & 0xfff);
            soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, MIM_ISID__OCFIf,
                        (info->default_vlan >> 12) & 0x1);
            soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, MIM_ISID__OPRIf,
                        (info->default_vlan >> 13) & 0x7);
            if (profile_idx) {
                rv = _bcm_trx_vlan_action_profile_entry_delete(unit,
                          profile_idx);
                BCM_IF_ERROR_RETURN(rv);
            }
        }
        rv = soc_mem_insert(unit, vxlt_mem, MEM_BLOCK_ALL, vxlt_entry);
        /* replaced entry with same key will have BCM_E_EXISTS */
        if (BCM_FAILURE(rv) && (rv != BCM_E_EXISTS)) {
            return rv;
        } else if ((rv == BCM_E_EXISTS) &&
                   (!(info->flags & BCM_MIM_VPN_REPLACE))) {
            return rv;
        } else if (BCM_SUCCESS(rv) && (info->flags & BCM_MIM_VPN_REPLACE)) {
            /* if replace, delete the old entry which has different key */
            soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, MIM_ISID__ISIDf,
                            old_info->lookup_id);
            rv = soc_mem_delete(unit, vxlt_mem, MEM_BLOCK_ALL, vxlt_entry);
            BCM_IF_ERROR_RETURN(rv);
        }

    } else if (mim_port != NULL) {
        vxlt_mem = VLAN_XLATE_2_DOUBLEm;
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, BASE_VALID_1f, 7);
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, KEY_TYPEf, 23);
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, DATA_TYPEf, 20);
        if (mim_port->flags & BCM_MIM_PORT_REPLACE) {
            soc_mem_field32_set(unit, vxlt_mem, vxlt_entry,
                     MIM_BMACSA_BVID_ISID__ISIDf, VPN_ISID(unit, vfi));
            soc_mem_field32_set(unit, vxlt_mem, vxlt_entry,
                MIM_BMACSA_BVID_ISID__OUTER_VLANf,
                MIM_INFO(unit)->port_info[vp].match_tunnel_vlan);
            soc_mem_mac_addr_set(unit, vxlt_mem, vxlt_entry,
                MIM_BMACSA_BVID_ISID__MAC_SAf,
                MIM_INFO(unit)->port_info[vp].match_tunnel_srcmac);
            rv = soc_mem_search(unit, vxlt_mem, MEM_BLOCK_ANY, &index,
                            vxlt_entry, vxlt_entry, 0);
            BCM_IF_ERROR_RETURN(rv);
            profile_idx = soc_mem_field32_get(unit, vxlt_mem, vxlt_entry,
                        MIM_BMACSA_BVID_ISID__ING_TAG_ACTION_PROFILE_IDXf);
        }
        if (mim_port->flags & BCM_MIM_PORT_EGRESS_TUNNEL_SERVICE_REPLACE) {
            isid = mim_port->egress_tunnel_service & 0xffffff;
        } else {
            isid = VPN_ISID(unit, vfi);
        }
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry,
                     MIM_BMACSA_BVID_ISID__ISIDf, isid);
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry,
                MIM_BMACSA_BVID_ISID__OUTER_VLANf, mim_port->match_tunnel_vlan);
        soc_mem_mac_addr_set(unit, vxlt_mem, vxlt_entry,
                MIM_BMACSA_BVID_ISID__MAC_SAf, mim_port->match_tunnel_srcmac);
        if (vlan) {
            if (!profile_idx) {
                bcm_vlan_action_set_t_init(&action);
                action.ut_outer           = bcmVlanActionAdd;
                action.ut_outer_pkt_prio  = bcmVlanActionAdd;
                action.ut_outer_cfi       = bcmVlanActionAdd;
                action.ot_outer           = bcmVlanActionReplace;
                action.it_outer           = bcmVlanActionAdd;
                action.dt_outer           = bcmVlanActionReplace;
                rv = _bcm_trx_vlan_action_profile_entry_add(unit, &action,
                              &profile_idx);
                BCM_IF_ERROR_RETURN(rv);
            }
            soc_mem_field32_set(unit, vxlt_mem, vxlt_entry,
                        MIM_BMACSA_BVID_ISID__ING_TAG_ACTION_PROFILE_IDXf,
                        profile_idx);
            soc_mem_field32_set(unit, vxlt_mem, vxlt_entry,
                            MIM_BMACSA_BVID_ISID__OVIDf, vlan & 0xfff);
            soc_mem_field32_set(unit, vxlt_mem, vxlt_entry,
                            MIM_BMACSA_BVID_ISID__OCFIf, (vlan >> 12) & 0x1);
            soc_mem_field32_set(unit, vxlt_mem, vxlt_entry,
                            MIM_BMACSA_BVID_ISID__OPRIf, (vlan >> 13) & 0x7);
        } else {
            soc_mem_field32_set(unit, vxlt_mem, vxlt_entry,
                        MIM_BMACSA_BVID_ISID__ING_TAG_ACTION_PROFILE_IDXf, 0);
            soc_mem_field32_set(unit, vxlt_mem, vxlt_entry,
                            MIM_BMACSA_BVID_ISID__OVIDf, 0);
            soc_mem_field32_set(unit, vxlt_mem, vxlt_entry,
                            MIM_BMACSA_BVID_ISID__OCFIf, 0);
            soc_mem_field32_set(unit, vxlt_mem, vxlt_entry,
                            MIM_BMACSA_BVID_ISID__OPRIf, 0);
            if (profile_idx) {
                rv = _bcm_trx_vlan_action_profile_entry_delete(unit,
                          profile_idx);
                BCM_IF_ERROR_RETURN(rv);
            }
        }
        rv = soc_mem_insert(unit, vxlt_mem, MEM_BLOCK_ALL, vxlt_entry);
        /* replaced entry with same key will have BCM_E_EXISTS */
        if (BCM_FAILURE(rv) && (rv != BCM_E_EXISTS)) {
            return rv;
        } else if ((rv == BCM_E_EXISTS) &&
                   (!(mim_port->flags & BCM_MIM_PORT_REPLACE))) {
            return rv;
        } else if (BCM_SUCCESS(rv) &&
                   (mim_port->flags & BCM_MIM_PORT_REPLACE)) {
            /* if replace, delete the old entry which has different key */
            soc_mem_field32_set(unit, vxlt_mem, vxlt_entry,
                     MIM_BMACSA_BVID_ISID__ISIDf, VPN_ISID(unit, vfi));
            soc_mem_field32_set(unit, vxlt_mem, vxlt_entry,
                MIM_BMACSA_BVID_ISID__OUTER_VLANf,
                MIM_INFO(unit)->port_info[vp].match_tunnel_vlan);
            soc_mem_mac_addr_set(unit, vxlt_mem, vxlt_entry,
                MIM_BMACSA_BVID_ISID__MAC_SAf,
                MIM_INFO(unit)->port_info[vp].match_tunnel_srcmac);
            rv = soc_mem_delete(unit, vxlt_mem, MEM_BLOCK_ALL, vxlt_entry);
            BCM_IF_ERROR_RETURN(rv);
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_td3_mim_untagged_payload_vlan_get(int unit,
             bcm_mim_vpn_config_t *info, bcm_mim_port_t *mim_port, int isid)
{
    uint32 vxlt_entry[SOC_MAX_MEM_WORDS];
    soc_mem_t vxlt_mem;
    bcm_vlan_action_set_t action;
    uint32 profile_idx = 0;
    int index;
    int rv;
    int cfi;
    int pri;
    int ovid;

    sal_memset(vxlt_entry, 0, sizeof(vxlt_entry));
    bcm_vlan_action_set_t_init(&action);

    /* ISID -> vlan */
    if (info != NULL) {
        vxlt_mem = VLAN_XLATE_2_SINGLEm;
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, BASE_VALIDf, 1);
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, KEY_TYPEf, 24);
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, DATA_TYPEf, 20);
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, MIM_ISID__ISIDf,
                            info->lookup_id);
        rv = soc_mem_search(unit, vxlt_mem, MEM_BLOCK_ANY, &index,
                            vxlt_entry, vxlt_entry, 0);
        BCM_IF_ERROR_RETURN(rv);
        profile_idx = soc_mem_field32_get(unit, vxlt_mem, vxlt_entry,
                        MIM_ISID__ING_TAG_ACTION_PROFILE_IDXf);
        _bcm_trx_vlan_action_profile_entry_get(unit, &action, profile_idx);
        if (action.ut_outer == bcmVlanActionAdd) {
            ovid = soc_mem_field32_get(unit, vxlt_mem, vxlt_entry,
                                       MIM_ISID__OVIDf);
            cfi = soc_mem_field32_get(unit, vxlt_mem, vxlt_entry,
                                       MIM_ISID__OCFIf);
            pri = soc_mem_field32_get(unit, vxlt_mem, vxlt_entry,
                                       MIM_ISID__OPRIf);
            info->default_vlan = (pri << 13) | (cfi << 12) | ovid;
        }
    } else if (mim_port != NULL) {
        vxlt_mem = VLAN_XLATE_2_DOUBLEm;
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, BASE_VALID_1f, 7);
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, KEY_TYPEf, 23);
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, DATA_TYPEf, 20);
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry,
              MIM_BMACSA_BVID_ISID__ISIDf, isid);
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry,
              MIM_BMACSA_BVID_ISID__OUTER_VLANf, mim_port->match_tunnel_vlan);
        soc_mem_mac_addr_set(unit, vxlt_mem, vxlt_entry,
              MIM_BMACSA_BVID_ISID__MAC_SAf, mim_port->match_tunnel_srcmac);

        rv = soc_mem_search(unit, vxlt_mem, MEM_BLOCK_ANY, &index,
                            vxlt_entry, vxlt_entry, 0);
        BCM_IF_ERROR_RETURN(rv);
        profile_idx = soc_mem_field32_get(unit, vxlt_mem, vxlt_entry,
                        MIM_BMACSA_BVID_ISID__ING_TAG_ACTION_PROFILE_IDXf);
        _bcm_trx_vlan_action_profile_entry_get(unit, &action, profile_idx);
        if (action.ut_outer == bcmVlanActionAdd) {
            ovid = soc_mem_field32_get(unit, vxlt_mem, vxlt_entry,
                                       MIM_BMACSA_BVID_ISID__OVIDf);
            cfi = soc_mem_field32_get(unit, vxlt_mem, vxlt_entry,
                                       MIM_BMACSA_BVID_ISID__OCFIf);
            pri = soc_mem_field32_get(unit, vxlt_mem, vxlt_entry,
                                       MIM_BMACSA_BVID_ISID__OPRIf);
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_td3_mim_untagged_payload_vlan_delete(int unit,
             int isid, int vp)
{
    uint32 vxlt_entry[SOC_MAX_MEM_WORDS];
    soc_mem_t vxlt_mem;
    bcm_vlan_action_set_t action;
    uint32 profile_idx = 0;
    int index;
    int rv;

    sal_memset(vxlt_entry, 0, sizeof(vxlt_entry));
    bcm_vlan_action_set_t_init(&action);

    /* ISID -> vlan */
    if (!vp) {
        vxlt_mem = VLAN_XLATE_2_SINGLEm;
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, BASE_VALIDf, 1);
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, KEY_TYPEf, 24);
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, DATA_TYPEf, 20);
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, MIM_ISID__ISIDf, isid);
        rv = soc_mem_search(unit, vxlt_mem, MEM_BLOCK_ANY, &index,
                            vxlt_entry, vxlt_entry, 0);
        BCM_IF_ERROR_RETURN(rv);
        profile_idx = soc_mem_field32_get(unit, vxlt_mem, vxlt_entry,
                        MIM_ISID__ING_TAG_ACTION_PROFILE_IDXf);
        rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
        BCM_IF_ERROR_RETURN(rv);
        rv = soc_mem_delete(unit, vxlt_mem, MEM_BLOCK_ALL, vxlt_entry);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        vxlt_mem = VLAN_XLATE_2_DOUBLEm;
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, BASE_VALID_1f, 7);
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, KEY_TYPEf, 23);
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry, DATA_TYPEf, 20);
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry,
              MIM_BMACSA_BVID_ISID__ISIDf, isid);
        soc_mem_field32_set(unit, vxlt_mem, vxlt_entry,
              MIM_BMACSA_BVID_ISID__OUTER_VLANf,
              MIM_INFO(unit)->port_info[vp].match_tunnel_vlan);
        soc_mem_mac_addr_set(unit, vxlt_mem, vxlt_entry,
              MIM_BMACSA_BVID_ISID__MAC_SAf,
              MIM_INFO(unit)->port_info[vp].match_tunnel_srcmac);
        rv = soc_mem_search(unit, vxlt_mem, MEM_BLOCK_ANY, &index,
                            vxlt_entry, vxlt_entry, 0);
        BCM_IF_ERROR_RETURN(rv);
        profile_idx = soc_mem_field32_get(unit, vxlt_mem, vxlt_entry,
                        MIM_BMACSA_BVID_ISID__ING_TAG_ACTION_PROFILE_IDXf);
        rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
        BCM_IF_ERROR_RETURN(rv);
        rv = soc_mem_delete(unit, vxlt_mem, MEM_BLOCK_ALL, vxlt_entry);
        BCM_IF_ERROR_RETURN(rv);
    }
    return BCM_E_NONE;
}
STATIC int
_bcm_td3_mim_flex_vpn_egr_vxlt_op(int unit, bcm_mim_vpn_config_t *info,
        int vfi, int operation)
{
    uint32 evxlt_entry[SOC_MAX_MEM_WORDS];
    soc_mem_t evxlt_mem;
    uint32 old_profile_index;
    int rv;
    int index;
    int old_tpid_idx = -1;

    sal_memset(evxlt_entry, 0, sizeof(evxlt_entry));

    evxlt_mem = EGR_VLAN_XLATE_1_SINGLEm;
    soc_mem_field32_set(unit, evxlt_mem, evxlt_entry, DATA_TYPEf, 23);
    soc_mem_field32_set(unit, evxlt_mem, evxlt_entry, KEY_TYPEf, 19);
    soc_mem_field32_set(unit, evxlt_mem, evxlt_entry, BASE_VALIDf, 1);
    soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                    VFI_ASSIGN_ISID__VFIf, vfi);

    if (operation == _MIM_EVXLT_OP_GET) {
        rv = soc_mem_search(unit, evxlt_mem, MEM_BLOCK_ANY, &index,
                    evxlt_entry, evxlt_entry, 0);
        BCM_IF_ERROR_RETURN(rv);

        rv = _bcm_td3_mim_flex_evxlt_tag_action_get(unit, NULL, info,
                                                  evxlt_entry, NULL);
        BCM_IF_ERROR_RETURN(rv);
        return BCM_E_NONE;
    } else if (operation == _MIM_EVXLT_OP_DELETE) {
        rv = soc_mem_search(unit, evxlt_mem, MEM_BLOCK_ANY, &index,
                        evxlt_entry, evxlt_entry, 0);
        BCM_IF_ERROR_RETURN(rv);
        old_profile_index = soc_mem_field32_get(unit, evxlt_mem, evxlt_entry,
                                   VFI_ASSIGN_ISID__TAG_ACTION_PROFILE_PTRf);
        old_tpid_idx = soc_mem_field32_get(unit, evxlt_mem, evxlt_entry,
                                   VFI_ASSIGN_ISID__OUTER_TPID_INDEXf);
        if (old_profile_index) {
            rv = _bcm_td3_mim_flex_evxlt_tag_action_delete(unit,
                     old_profile_index, old_tpid_idx);
            BCM_IF_ERROR_RETURN(rv);
        }
        rv = soc_mem_delete(unit, evxlt_mem, MEM_BLOCK_ANY,
                    evxlt_entry);
        BCM_IF_ERROR_RETURN(rv);
        return BCM_E_NONE;
    }

    /* operation ==  _MIM_EVXLT_OP_ADD */
    soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                    VFI_ASSIGN_ISID__ISIDf, info->lookup_id);

    old_profile_index = 0;
    rv = soc_mem_search(unit, evxlt_mem, MEM_BLOCK_ANY, &index,
                    evxlt_entry, evxlt_entry, 0);
    if (rv == SOC_E_NONE) {
        if (!((info->flags & BCM_MIM_VPN_WITH_ID) &&
             (info->flags & BCM_MIM_VPN_REPLACE))) {
            return BCM_E_EXISTS;
        }
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;
    }

    if ((info->flags & BCM_MIM_VPN_WITH_ID) &&
        (info->flags & BCM_MIM_VPN_REPLACE)) {

        old_profile_index = soc_mem_field32_get(unit, evxlt_mem, evxlt_entry,
                               VFI_ASSIGN_ISID__TAG_ACTION_PROFILE_PTRf);
        old_tpid_idx = soc_mem_field32_get(unit, evxlt_mem, evxlt_entry,
                                   VFI_ASSIGN_ISID__OUTER_TPID_INDEXf);
    }

    BCM_IF_ERROR_RETURN(_bcm_td3_mim_flex_evxlt_tag_action_add(unit,
         NULL, info, evxlt_entry));
    rv = soc_mem_insert(unit, evxlt_mem, MEM_BLOCK_ALL, evxlt_entry);

    /* replaced entry with same key will have BCM_E_EXISTS */
    if (BCM_FAILURE(rv) && (rv != BCM_E_EXISTS)) {
        return rv;
    } else if ((rv == BCM_E_EXISTS) &&
               (!(info->flags & BCM_MIM_VPN_REPLACE))) {
        return rv;
    }
    if ((info->flags & BCM_MIM_VPN_WITH_ID) &&
        (info->flags & BCM_MIM_VPN_REPLACE) ) {
        if (old_profile_index) {
            BCM_IF_ERROR_RETURN(_bcm_td3_mim_flex_evxlt_tag_action_delete(unit,
                 old_profile_index, old_tpid_idx));
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_td3_mim_flex_port_egr_vxlt_op(int unit, bcm_mim_port_t *mim_port,
        int vfi, int vp, int operation, uint32 *ret_entry)
{
    uint32 evxlt_entry[SOC_MAX_MEM_WORDS];
    soc_mem_t evxlt_mem;
    int index;
    int isid;
    int rv;
    uint32 old_profile_index = 0;
    int old_tpid_idx = -1;

    sal_memset(evxlt_entry, 0, sizeof(evxlt_entry));

    evxlt_mem = EGR_VLAN_XLATE_1_DOUBLEm;
    soc_mem_field32_set(unit, evxlt_mem, evxlt_entry, BASE_VALID_0f, 0x3);
    soc_mem_field32_set(unit, evxlt_mem, evxlt_entry, BASE_VALID_1f, 0x7);
    soc_mem_field32_set(unit, evxlt_mem, evxlt_entry, DATA_TYPEf, 23);
    soc_mem_field32_set(unit, evxlt_mem, evxlt_entry, KEY_TYPEf, 13);
    soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                            DVP_VFI_ASSIGN_ISID__VFIf, vfi);
    soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                        DVP_VFI_ASSIGN_ISID__DVPf, vp);
    if (operation == _MIM_EVXLT_OP_ADD) {
        rv = soc_mem_search(unit, evxlt_mem, MEM_BLOCK_ANY, &index,
                    evxlt_entry, evxlt_entry, 0);
        if (rv == SOC_E_NONE) {
            if (!((mim_port->flags & BCM_MIM_PORT_WITH_ID) &&
                 (mim_port->flags & BCM_MIM_PORT_REPLACE))) {
                return BCM_E_EXISTS;
            }
        } else if (rv != SOC_E_NOT_FOUND) {
            return rv;
        }
        if ((mim_port->flags & BCM_MIM_PORT_WITH_ID) &&
            (mim_port->flags & BCM_MIM_PORT_REPLACE)) {
            old_profile_index = soc_mem_field32_get(unit, evxlt_mem,
                  evxlt_entry, DVP_VFI_ASSIGN_ISID__TAG_ACTION_PROFILE_PTRf);
            old_tpid_idx = soc_mem_field32_get(unit, evxlt_mem, evxlt_entry,
                                   DVP_VFI_ASSIGN_ISID__OUTER_TPID_INDEXf);
        }

        if (mim_port->flags & BCM_MIM_PORT_EGRESS_TUNNEL_SERVICE_REPLACE) {
            soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                                DVP_VFI_ASSIGN_ISID__ISIDf,
                                (mim_port->egress_tunnel_service & 0xffffff));
        } else {
            soc_mem_field32_set(unit, evxlt_mem, evxlt_entry,
                            DVP_VFI_ASSIGN_ISID__ISIDf, VPN_ISID(unit, vfi));
        }

        if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TAGGED) {
            BCM_IF_ERROR_RETURN
                (_bcm_td3_mim_flex_evxlt_tag_action_add(unit, mim_port, NULL,
                                                  evxlt_entry));
        }
        rv = soc_mem_insert(unit, evxlt_mem, MEM_BLOCK_ALL, evxlt_entry);
        if (SOC_FAILURE(rv) && (rv != SOC_E_EXISTS)) {
            return rv;
        } else if ((rv == BCM_E_EXISTS) &&
                   (!(mim_port->flags & BCM_MIM_PORT_REPLACE))) {
            return rv;
        }
        if ((mim_port->flags & BCM_MIM_PORT_WITH_ID) &&
            (mim_port->flags & BCM_MIM_PORT_REPLACE)) {
            if (old_profile_index) {
                rv = _bcm_td3_mim_flex_evxlt_tag_action_delete(unit,
                          old_profile_index, old_tpid_idx);
                BCM_IF_ERROR_RETURN(rv);
            }
        }
    } else if (operation == _MIM_EVXLT_OP_GET) {
        rv = soc_mem_search(unit, evxlt_mem, MEM_BLOCK_ANY, &index,
                        evxlt_entry, evxlt_entry, 0);
        BCM_IF_ERROR_RETURN(rv);

        isid = soc_mem_field32_get(unit, evxlt_mem, evxlt_entry,
                               DVP_VFI_ASSIGN_ISID__ISIDf);
        if (isid != VPN_ISID(unit, vfi)) {
            mim_port->egress_tunnel_service = isid;
            mim_port->flags |= BCM_MIM_PORT_EGRESS_TUNNEL_SERVICE_REPLACE;
        }

        rv = _bcm_td3_mim_flex_evxlt_tag_action_get(unit, mim_port, NULL,
                                                  evxlt_entry, NULL);
        BCM_IF_ERROR_RETURN(rv);

    } else if (operation == _MIM_EVXLT_OP_DELETE) {
        rv = soc_mem_search(unit, evxlt_mem, MEM_BLOCK_ANY, &index,
                        evxlt_entry, evxlt_entry, 0);
        BCM_IF_ERROR_RETURN(rv);
        if (ret_entry != NULL) {
            sal_memcpy(ret_entry, evxlt_entry, sizeof(evxlt_entry));
        }
        old_profile_index = soc_mem_field32_get(unit, evxlt_mem, evxlt_entry,
                               DVP_VFI_ASSIGN_ISID__TAG_ACTION_PROFILE_PTRf);
        old_tpid_idx = soc_mem_field32_get(unit, evxlt_mem, evxlt_entry,
                                   DVP_VFI_ASSIGN_ISID__OUTER_TPID_INDEXf);
        if (old_profile_index) {
            rv = _bcm_td3_mim_flex_evxlt_tag_action_delete(unit,
                      old_profile_index, old_tpid_idx);
            BCM_IF_ERROR_RETURN(rv);
        }
        rv = soc_mem_delete(unit, evxlt_mem, MEM_BLOCK_ANY, evxlt_entry);
        BCM_IF_ERROR_RETURN(rv);

    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td3_mim_l2_nh_otag_set(int unit, _bcm_tr2_egr_nh_info_t *egr_nh_info,
             egr_l3_next_hop_entry_t *egr_nh, bcm_mim_port_t *mim_port,
             int is_local, int *old_tpid_idx)
{
    uint32 profile_index = 0;
    int action_not_present = 0;
    int action_present = 0;

    if (mim_port->flags & BCM_MIM_PORT_REPLACE) {
        egr_nh_info->entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                        egr_nh, DATA_TYPEf);
        /* Be sure that the existing entry type is correct */
        if (egr_nh_info->entry_type != 21) { /* != OTAG */
            return BCM_E_PARAM;
        }
        profile_index = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh,
                               L2_OTAG__TAG_ACTION_PROFILE_PTRf);
        (void)_bcm_td3_sd_tag_action_get(unit, profile_index,
                           &action_present, &action_not_present);
        if ((action_not_present == 0x1) || (action_present == 0x1) ||
            (action_present == 0x4) || (action_present == 0x7)) {
            /* If SD tag action is ADD or REPLACE_VID_TPID, the tpid
             * index of the entry getting replaced is valid. Save
             * the old tpid index to be deleted later.
             */
            if (old_tpid_idx != NULL) {
                *old_tpid_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh,
                                    L2_OTAG__TPID_INDEXf);
            }
        }
    }
    egr_nh_info->entry_type = 21; /* OTAG */

    /* Populate EGR_L3_NEXT_HOP entry */
    if (!(mim_port->flags & BCM_MIM_PORT_REPLACE)) {
        sal_memset(egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
    }

    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                        DATA_TYPEf, egr_nh_info->entry_type);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                        L2_OTAG__DVPf, egr_nh_info->dvp);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                        L2_OTAG__EDIT_CTRL_IDf, 1);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                        L2_OTAG__FLOW_SELECT_CTRL_IDf, 8);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                        L2_OTAG__NEXT_PTR_TYPEf, 1);

    /* HG header PPD2 */
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                        L2_OTAG__HG_HDR_SELf, 1);

    if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TAGGED) {

        /* Prepare egress SD tag entry (including parameter checks) */
        BCM_IF_ERROR_RETURN (_bcm_tr2_mim_egr_sd_tag_actions(unit, mim_port,
             egr_nh_info));

        if (egr_nh_info->sd_tag_vlan != -1) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                    egr_nh, L2_OTAG__VIDf, egr_nh_info->sd_tag_vlan);
        }
        if (egr_nh_info->tpid_index != -1) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                egr_nh, L2_OTAG__TPID_INDEXf, egr_nh_info->tpid_index);
        }
        if (egr_nh_info->sd_tag_pri != -1) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                egr_nh, L2_OTAG__PCPf, egr_nh_info->sd_tag_pri);
        }
        if (egr_nh_info->sd_tag_cfi != -1) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                egr_nh, L2_OTAG__DEf, egr_nh_info->sd_tag_cfi);
        }
        if ((egr_nh_info->sd_tag_pri != -1) ||
            (egr_nh_info->sd_tag_cfi != -1)) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                egr_nh, L2_OTAG__PCP_DE_SELf, 1);
        }

        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                     L2_OTAG__HG_MODIFY_ENABLEf, (is_local) ? 1 : 0);
        BCM_IF_ERROR_RETURN(_bcm_td3_sd_tag_action_profile_get(unit,
                    egr_nh_info->sd_tag_action_present,
                    egr_nh_info->sd_tag_action_not_present, &profile_index));
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                        L2_OTAG__TAG_ACTION_PROFILE_PTRf, profile_index);
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_td3_mim_flex_dvp_set(int unit, int vp, bcm_mim_port_t *mim_port)
{
    int rv = BCM_E_NONE;
    egr_dvp_attribute_entry_t  egr_dvp_attribute;
    int network_group = 0;
    soc_mem_t mem = EGR_DVP_ATTRIBUTEm;

    if (mim_port->flags & BCM_MIM_PORT_TYPE_ACCESS) {
        return BCM_E_NONE;
    }
    sal_memset(&egr_dvp_attribute, 0, sizeof(egr_dvp_attribute_entry_t));
    network_group = mim_port->network_group_id;
    soc_mem_field32_set(unit, mem, &egr_dvp_attribute,
                ENTRY_TYPEf, 13);
    rv = _bcm_validate_splithorizon_network_group(unit,
                mim_port->flags & BCM_MIM_PORT_TYPE_BACKBONE, &network_group);
    BCM_IF_ERROR_RETURN(rv);
    soc_mem_field32_set(unit, mem, &egr_dvp_attribute,
                FLEX_MIM__DVP_NETWORK_GROUPf, network_group);

    if (mim_port->flags & BCM_MIM_PORT_TYPE_PEER) {
        soc_mem_field32_set(unit, mem, &egr_dvp_attribute,
               FLEX_MIM__VXLT_CONTROL_IDf, 0x2);
    } else if (mim_port->flags & BCM_MIM_PORT_TYPE_BACKBONE) {
        soc_mem_field32_set(unit, mem, &egr_dvp_attribute,
                FLEX_MIM__VXLT_CONTROL_IDf, 0x1);
    }

    if (mim_port->flags & BCM_MIM_PORT_FIX_PRIORITY) {
        soc_mem_field32_set(unit, mem, &egr_dvp_attribute,
            FLEX_MIM__QOS_ACTION_SELf, 1);
        soc_mem_field32_set(unit, mem, &egr_dvp_attribute,
            FLEX_MIM__QOS_VALUEf, mim_port->int_pri & 0xf);
    }
    soc_mem_field32_set(unit, mem, &egr_dvp_attribute,
            FLEX_MIM__FLOW_SELECT_CTRL_IDf, 0x1);

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, vp, &egr_dvp_attribute);
    BCM_IF_ERROR_RETURN(rv);

    return rv;
}

STATIC int
_bcm_td3_mim_flex_dvp_get(int unit, int vp, bcm_mim_port_t *mim_port)
{
    int rv = BCM_E_NONE;
    egr_dvp_attribute_entry_t  egr_dvp_attribute;
    soc_mem_t mem = EGR_DVP_ATTRIBUTEm;
    int ctrl_id;
    int qos_sel;

    sal_memset(&egr_dvp_attribute, 0, sizeof(egr_dvp_attribute_entry_t));
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, vp, &egr_dvp_attribute);
    BCM_IF_ERROR_RETURN(rv);

    ctrl_id = soc_mem_field32_get(unit, mem, &egr_dvp_attribute,
                FLEX_MIM__VXLT_CONTROL_IDf);
    if (ctrl_id == 0x1) {
        mim_port->flags |= BCM_MIM_PORT_TYPE_BACKBONE;
    } else if (ctrl_id == 0x2) {
        mim_port->flags |= BCM_MIM_PORT_TYPE_PEER;
    }

    qos_sel = soc_mem_field32_get(unit, mem, &egr_dvp_attribute,
            FLEX_MIM__QOS_ACTION_SELf);
    if (qos_sel == 1) {
        mim_port->flags |= BCM_MIM_PORT_FIX_PRIORITY;
    }

    return BCM_E_NONE;
}

#endif /* defined(BCM_TRIDENT3_SUPPORT) */

/*
 * Function:
 *      bcm_mim_vpn_create
 * Purpose:
 *      Create a VPN instance
 * Parameters:
 *      unit  - (IN)  Device Number
 *      info  - (IN/OUT) VPN configuration info
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr2_mim_vpn_create(int unit, bcm_mim_vpn_config_t *info)
{   
    int index, rv = BCM_E_PARAM;
    vfi_entry_t vfi_entry;
    vfi_1_entry_t vfi_1_entry;
    uint32 mpls_entry[SOC_MAX_MEM_WORDS] = {0};
    bcm_mim_vpn_config_t old_info;
    uint32 egr_vlan_xlate_entry[SOC_MAX_MEM_WORDS];
    int vfi, bc_group = 0, umc_group = 0, uuc_group = 0, num_vfi;
    int bc_group_type, umc_group_type, uuc_group_type;
    soc_mem_t mpls_mem = MPLS_ENTRYm;
    bcm_vpn_t mim_vpn_min=0;
    int hw_idx;
    int proto_pkt_idx;
    soc_mem_t egr_xlate_mem;
#if defined(BCM_TRIDENT3_SUPPORT)
    uint32 vfi_attrs_1_entry[SOC_MAX_MEM_WORDS] = {0};
#endif

    MIM_INIT(unit);

    /* default vlan only apply to decoupled mode */
#if defined(BCM_TRIDENT3_SUPPORT)
    if (!soc_feature(unit, soc_feature_mim_decoupled_mode)) {
        if (info->default_vlan) {
            LOG_WARN(BSL_LS_BCM_MIM, (BSL_META_U(unit,
                "default vlan only apply to decoupled mode\n")));
            return BCM_E_PARAM;
        }
    }
#endif

    num_vfi = soc_mem_index_count(unit, VFIm);

    bcm_mim_vpn_config_t_init(&old_info);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        egr_xlate_mem = EGR_VLAN_XLATE_1_DOUBLEm;
    } else
#endif
    {
        egr_xlate_mem = EGR_VLAN_XLATEm;
    }

    /* For Point-to_point service, multicast groups are not valid */
    if (info->flags & BCM_MIM_VPN_ELINE) { 

        /* For point-to-point vpn groups are not valid */
        if ((info->broadcast_group != 0) || 
            (info->unknown_unicast_group != 0) || 
            (info->unknown_multicast_group != 0)){
            return BCM_E_PARAM; 
        }    
        
    } else {

        /* Check that the groups are valid. */

        bc_group_type = _BCM_MULTICAST_TYPE_GET(info->broadcast_group);
        bc_group = _BCM_MULTICAST_ID_GET(info->broadcast_group);
        umc_group_type = _BCM_MULTICAST_TYPE_GET(info->unknown_multicast_group);
        umc_group = _BCM_MULTICAST_ID_GET(info->unknown_multicast_group);
        uuc_group_type = _BCM_MULTICAST_TYPE_GET(info->unknown_unicast_group);
        uuc_group = _BCM_MULTICAST_ID_GET(info->unknown_unicast_group);

        if ((bc_group_type != _BCM_MULTICAST_TYPE_MIM) ||
            (umc_group_type != _BCM_MULTICAST_TYPE_MIM) ||
            (uuc_group_type != _BCM_MULTICAST_TYPE_MIM) ||
            (bc_group >= soc_mem_index_count(unit, L3_IPMCm)) ||
            (umc_group >= soc_mem_index_count(unit, L3_IPMCm)) ||
            (uuc_group >= soc_mem_index_count(unit, L3_IPMCm))) {
            return BCM_E_PARAM;
        }
    }
    /* validate parameter protocol_pkt */
    if (SOC_MEM_FIELD_VALID(unit, VFIm, PROTOCOL_PKT_INDEXf)) {
        rv = _bcm_xgs3_protocol_packet_actions_validate(unit,
                               &info->protocol_pkt);
        if (BCM_FAILURE(rv) && (rv != BCM_E_UNAVAIL)) {
            return rv;
        }
    }
    /* Validate lookup id value */
#if defined(BCM_TRIUMPH3_SUPPORT)    
    if (SOC_IS_TRIUMPH3(unit)) {
        mpls_mem = MPLS_ENTRY_EXTDm;
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
        mpls_mem = MPLS_ENTRYm;
    }

    if ( info->lookup_id >= 0 ) {
        /* coverity[large_shift : FALSE] */
        if (!SOC_MEM_FIELD32_VALUE_FIT(unit, mpls_mem, MIM_ISID__ISIDf, 
                                      info->lookup_id)) {
            LOG_WARN(BSL_LS_BCM_MIM,
                     (BSL_META_U(unit,
                                 "lookup_id value exceeds 0x%x \n"),
                                 SOC_MEM_FIELD32_VALUE_MAX(unit, mpls_mem, MIM_ISID__ISIDf)));
            return BCM_E_PARAM;
        }
    }


    MIM_LOCK(unit);
    if (info->flags & BCM_MIM_VPN_WITH_ID) {
            _BCM_MIM_VPN_SET(mim_vpn_min, _BCM_MIM_VPN_TYPE_MIM, 0);
            if (info->vpn < mim_vpn_min || 
                info->vpn > (mim_vpn_min + num_vfi - 1) ) {
                MIM_UNLOCK(unit);
                return BCM_E_PARAM;
            }

        /* Check if VFI exists and handle the replace case */
        _BCM_MIM_VPN_GET(vfi, _BCM_MIM_VPN_TYPE_MIM, info->vpn);
        if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
            if ((info->flags & BCM_MIM_VPN_REPLACE)) {

                /* do not allow P2P -> P2M or P2M -> P2P */
                /* Read the VFI table to check if point-to-point vpn */
                sal_memset(&vfi_entry, 0, sizeof(vfi_entry_t));
                rv = READ_VFIm(unit, MEM_BLOCK_ALL, vfi, &vfi_entry);
                if (BCM_FAILURE(rv)) {
                    MIM_UNLOCK(unit);
                    return rv;
                }

                /* ELINE */
                if (soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf)) {
                   if (!(info->flags & BCM_MIM_VPN_ELINE)) {
                       MIM_UNLOCK(unit);
                       return BCM_E_PARAM;
                   }   
                } else { /* ELAN */
                   if (!(info->flags & BCM_MIM_VPN_MIM)) {
                       MIM_UNLOCK(unit);
                       return BCM_E_PARAM;
                   }   
                }
                
                /* Get old ISID lookup id to delete from mpls and xlate table */
                rv = bcm_tr2_mim_vpn_get( unit, info->vpn, &old_info);
                if (BCM_FAILURE(rv)) {
                    MIM_UNLOCK(unit);
                    return rv;
                }
            } else {
                MIM_UNLOCK(unit);
                return BCM_E_EXISTS;
            }
        } else {
           rv = _bcm_vfi_alloc_with_id(unit, VFIm, _bcmVfiTypeMim, vfi);
           if (rv < 0) {
               MIM_UNLOCK(unit);
               return rv;
           }
        }
    } else {
        /* Allocate a free VFI entry */
        rv = _bcm_vfi_alloc(unit, VFIm, _bcmVfiTypeMim, &vfi);
        if (rv < 0) {
            MIM_UNLOCK(unit);
            return rv;
        }
    }
    VPN_ISID(unit, vfi) = info->lookup_id;

    /* Commit the VFI entry to HW */
    BCM_IF_ERROR_RETURN(READ_VFIm(unit, MEM_BLOCK_ALL, vfi, &vfi_entry));
    if (!(info->flags & BCM_MIM_VPN_ELINE)) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_generic_dest)) {
            soc_mem_field32_dest_set(unit, VFIm, &vfi_entry,
                UMC_DESTINATIONf, SOC_MEM_FIF_DEST_IPMC, umc_group);
            soc_mem_field32_dest_set(unit, VFIm, &vfi_entry,
                UUC_DESTINATIONf, SOC_MEM_FIF_DEST_IPMC, uuc_group);
            soc_mem_field32_dest_set(unit, VFIm, &vfi_entry,
                BC_DESTINATIONf, SOC_MEM_FIF_DEST_IPMC, bc_group);
        } else
#endif
        {
            soc_VFIm_field32_set(unit, &vfi_entry, UMC_INDEXf, umc_group);
            soc_VFIm_field32_set(unit, &vfi_entry, UUC_INDEXf, uuc_group);
            soc_VFIm_field32_set(unit, &vfi_entry, BC_INDEXf, bc_group);
        }
#if  defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            uint32 profile_idx = 0;
            rv = _bcm_td3_def_vfi_profile_add(unit,
                info->flags & BCM_MIM_VPN_ELINE, &profile_idx);
            if (BCM_E_NONE == rv) {
                soc_VFIm_field32_set(unit, &vfi_entry, VFI_PROFILE_PTRf, profile_idx);
            }
        }
#endif
    }

    /* If ELINE service set the PT2PT field */
    if (info->flags & BCM_MIM_VPN_ELINE) {
        soc_VFIm_field32_set(unit, &vfi_entry, PT2PT_ENf, 0x1);
    }

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
                                                defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) || SOC_IS_APACHE(unit)) {
        rv = _bcm_esw_add_policer_to_table(unit, info->policer_id, VFIm, 0,
                                                    &vfi_entry);
        if (rv < 0) {
            MIM_UNLOCK(unit);
            return rv;
        }
    }
#endif /* BCM_KATANA_SUPPORT  or BCM_TRIUMPH3_SUPPORT or BCM_APACHE_SUPPORT */

    if (info->flags & BCM_MIM_VPN_BVID_DROP_NOMATCH) {
        soc_VFIm_field32_set(unit, &vfi_entry, BVIDf, info->tunnel_vlan & 0xfff);
    }

    if (info->flags & BCM_MIM_VPN_QOS_INGRESS_MAP_INT_PRIORITY) {
        if (!info->qos_map_id) {
            /* reset to default */
            hw_idx = 0;
        } else {
            rv = _bcm_tr2_qos_id2idx(unit, info->qos_map_id, &hw_idx);
            if (BCM_FAILURE(rv)) {
                MIM_UNLOCK(unit);
                return rv;
            }
        }
        soc_mem_field32_set(unit, VFIm, &vfi_entry, TRUST_DOT1P_PTRf, hw_idx);

        if (SOC_IS_TRIUMPH3(unit) || SOC_IS_TRIDENT(unit) ||
            SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
            soc_mem_field32_set(unit, VFIm, &vfi_entry,
                                PHB2_USE_INNER_DOT1Pf, info->int_pri & 0x1);
        } else {
            soc_mem_field32_set(unit, VFIm, &vfi_entry,
                                USE_INNER_PRIf, info->int_pri & 0x1);
        }
    }

    if (info->flags & BCM_MIM_VPN_QOS_INGRESS_MAP_FIX_INT_PRIORITY) {
        /** index 1024 to 1040 are reserved for fixed priority mapping */
        soc_mem_field32_set(unit, VFIm, &vfi_entry,
                            TRUST_DOT1P_PTRf, info->int_pri + 16);
        if (SOC_IS_TRIUMPH3(unit) || SOC_IS_TRIDENT(unit) || SOC_IS_TRIDENT2X(unit)) { 
            soc_mem_field32_set(unit, VFIm, &vfi_entry, PHB2_USE_INNER_DOT1Pf, 0);
        } else {
            soc_mem_field32_set(unit, VFIm, &vfi_entry, USE_INNER_PRIf, 0);
        } 

        if (SOC_IS_ENDURO(unit) || SOC_IS_TRIUMPH2(unit)) {
            /** set fix priority at location 1024 + fixed priority */
            ing_pri_cng_map_entry_t imap;
            rv = READ_ING_PRI_CNG_MAPm(unit, MEM_BLOCK_ANY,
                (1024 + info->int_pri), &imap);
            if (BCM_FAILURE(rv)) {
                MIM_UNLOCK(unit);
                return rv;
            }

            soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, &imap, PRIf, info->int_pri);
            rv = WRITE_ING_PRI_CNG_MAPm(unit, MEM_BLOCK_ANY,
                (1024 + info->int_pri), &imap);
            if (BCM_FAILURE(rv)) {
                MIM_UNLOCK(unit);
                return rv;
            }
        }
    }
    if (SOC_MEM_FIELD_VALID (unit,VFIm, PROTOCOL_PKT_INDEXf)) {
        if (info->flags & BCM_MIM_VPN_REPLACE) {
            proto_pkt_idx = soc_VFIm_field32_get(unit, &vfi_entry,
                                                 PROTOCOL_PKT_INDEXf);
        } else {
            proto_pkt_idx = -1;
        }
        rv = _bcm_xgs3_protocol_pkt_ctrl_set(unit, proto_pkt_idx,
                                             &info->protocol_pkt,
                                             &proto_pkt_idx);
        if (BCM_SUCCESS(rv)) {
            soc_VFIm_field32_set(unit, &vfi_entry, PROTOCOL_PKT_INDEXf,
                                 proto_pkt_idx);
        } else if (rv != BCM_E_UNAVAIL) {
            return rv;
        }
    }
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_mim_decoupled_mode)) {
        /* use flex QoS */
        if (info->flags & BCM_MIM_VPN_QOS_INGRESS_MAP_INT_PRIORITY) {
            if (info->qos_map_id) {
                soc_VFIm_field32_set(unit, &vfi_entry, PHB_CTRL_IDf, 2);
            } else {
                soc_VFIm_field32_set(unit, &vfi_entry, PHB_CTRL_IDf, 0);
            }
        }
    }
#endif

    rv = WRITE_VFIm(unit, MEM_BLOCK_ALL, vfi, &vfi_entry);

    if (rv < 0) {
        MIM_UNLOCK(unit);
        return rv;
    }

    /* Check for incoming SD-tag match */
    if (info->flags & BCM_MIM_VPN_MATCH_SERVICE_VLAN_TPID) {
        sal_memset(&vfi_1_entry, 0, sizeof(vfi_1_entry_t));
        rv = _bcm_fb2_outer_tpid_entry_add(unit, info->match_service_tpid, &index);
        if (rv < 0) {
            MIM_UNLOCK(unit);
            return rv;
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_MEM_FIELD_VALID(unit, VFIm, SD_TAG_MODEf)) {
            rv = READ_VFIm(unit, MEM_BLOCK_ALL, vfi, &vfi_entry);
            if (rv < 0) {
                MIM_UNLOCK(unit);
                return rv;
            }
            soc_VFIm_field32_set(unit, &vfi_entry, SD_TAG_MODEf, 1);
            rv = WRITE_VFIm(unit, MEM_BLOCK_ALL, vfi, &vfi_entry);
            if (rv < 0) {
                MIM_UNLOCK(unit);
                return rv;
            }
            rv = READ_VFI_ATTRS_1m(unit, MEM_BLOCK_ALL, vfi, &vfi_attrs_1_entry);
            if (rv < 0) {
                MIM_UNLOCK(unit);
                return rv;
            }
            soc_VFI_ATTRS_1m_field32_set(unit, vfi_attrs_1_entry, TPID_ENABLEf, (1 << index));
            rv = WRITE_VFI_ATTRS_1m(unit, MEM_BLOCK_ALL, vfi, &vfi_attrs_1_entry);
            if (rv < 0) {
                MIM_UNLOCK(unit);
                return rv;
            }
        } else
#endif
        {
            soc_VFI_1m_field32_set(unit, &vfi_1_entry, SD_TAG_MODEf, 1);
            soc_VFI_1m_field32_set(unit, &vfi_1_entry, TPID_ENABLEf, (1 << index));
            rv = WRITE_VFI_1m(unit, MEM_BLOCK_ALL, vfi, &vfi_1_entry);
            if (rv < 0) {
                MIM_UNLOCK(unit);
                return rv;
            }
        }
    }

    if ( info->lookup_id >= 0 ) {
        /* Program the ISID to VFI mapping */
        sal_memset(&mpls_entry, 0, sizeof(mpls_entry));
#if defined(BCM_TRIUMPH3_SUPPORT)    
        if (SOC_IS_TRIUMPH3(unit)) {
            soc_mem_field32_set(unit, mpls_mem, &mpls_entry, KEY_TYPE_0f, 0x18);
            soc_mem_field32_set(unit, mpls_mem, &mpls_entry, KEY_TYPE_1f, 0x18);
            soc_mem_field32_set(unit, mpls_mem, &mpls_entry, VALID_0f, 0x1);
            soc_mem_field32_set(unit, mpls_mem, &mpls_entry, VALID_1f, 0x1);
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
        {
            soc_mem_field32_set(unit, mpls_mem, &mpls_entry, KEY_TYPEf, 0x2);
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_base_valid)) {
                soc_mem_field32_set(unit, mpls_mem, &mpls_entry, BASE_VALID_0f, 3); 
                soc_mem_field32_set(unit, mpls_mem, &mpls_entry, BASE_VALID_1f, 7); 
                soc_mem_field32_set(unit, mpls_mem, &mpls_entry, DATA_TYPEf, 0x2);
            } else
#endif
            {
                soc_mem_field32_set(unit, mpls_mem, &mpls_entry, VALIDf, 0x1);
            }
        }
        /* key */
        soc_mem_field32_set(unit, mpls_mem, &mpls_entry, MIM_ISID__ISIDf, info->lookup_id);
        /* data */
        soc_mem_field32_set(unit, mpls_mem, &mpls_entry, MIM_ISID__VFIf, vfi);
        rv = soc_mem_search(unit, mpls_mem, MEM_BLOCK_ANY, &index,
                            &mpls_entry, &mpls_entry, 0);
        if (rv == SOC_E_NONE) {
            if ( (info->flags & BCM_MIM_VPN_REPLACE) && (info->lookup_id == old_info.lookup_id)) {
                /*replace with the same lookup_id*/
            } else {
                _bcm_vfi_free(unit, _bcmVfiTypeMim, vfi);
                MIM_UNLOCK(unit);
                return BCM_E_EXISTS;
            }
        } else if (rv != SOC_E_NOT_FOUND) {
            _bcm_vfi_free(unit, _bcmVfiTypeMim, vfi);
            MIM_UNLOCK(unit);
            return rv;
        }
    
        if ((info->flags & BCM_MIM_VPN_WITH_ID) &&
            (info->flags & BCM_MIM_VPN_REPLACE) &&
            (old_info.lookup_id >= 0)) {
            /* Delete old lookup ISID from mpls entry table*/
            soc_mem_field32_set(unit, mpls_mem, &mpls_entry, MIM_ISID__ISIDf, old_info.lookup_id);   
            rv = soc_mem_delete(unit, mpls_mem, MEM_BLOCK_ALL, &mpls_entry);
            if (BCM_FAILURE(rv)) {
                MIM_UNLOCK(unit);
                return rv;
            } 
        }
        soc_mem_field32_set(unit, mpls_mem, &mpls_entry, MIM_ISID__ISIDf, info->lookup_id);
        rv = soc_mem_insert(unit, mpls_mem, MEM_BLOCK_ALL, &mpls_entry);
        if (rv < 0) {
            if ((info->flags & BCM_MIM_VPN_WITH_ID) &&
                (info->flags & BCM_MIM_VPN_REPLACE) &&
                (old_info.lookup_id >= 0)) {
                    /* Re-insert old lookup ISID into mpls entry table */
                    soc_mem_field32_set(unit, mpls_mem, &mpls_entry, MIM_ISID__ISIDf, old_info.lookup_id);
                    soc_mem_insert(unit, mpls_mem, MEM_BLOCK_ALL, &mpls_entry);
            }
            MIM_UNLOCK(unit);
            return rv;
        }

#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_mim_decoupled_mode)) {
            rv = _bcm_td3_mim_untagged_payload_vlan_assign(unit,info, &old_info,
                      NULL, 0, 0, 0);
            if (BCM_FAILURE(rv)) {
                if (!((info->flags & BCM_MIM_VPN_WITH_ID) &&
                     (info->flags & BCM_MIM_VPN_REPLACE))) {
                    _bcm_vfi_free(unit, _bcmVfiTypeMim, vfi);
                }
                MIM_UNLOCK(unit);
                return rv;
            }

            rv = _bcm_td3_mim_flex_vpn_egr_vxlt_op(unit, info, vfi,
                      _MIM_EVXLT_OP_ADD);
            if (BCM_FAILURE(rv)) {
                if (!((info->flags & BCM_MIM_VPN_WITH_ID) &&
                     (info->flags & BCM_MIM_VPN_REPLACE))) {
                    _bcm_vfi_free(unit, _bcmVfiTypeMim, vfi);
                }
                MIM_UNLOCK(unit);
                return rv;
            }
        } else
#endif
        {
            /* Program the VFI to ISID mapping */
            sal_memset(egr_vlan_xlate_entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
    #if defined(BCM_TRIUMPH3_SUPPORT)
            if (SOC_IS_TRIUMPH3(unit)) {
                soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                                                KEY_TYPEf, 0x2); /* MIM_ISID */
            } else
    #endif /* BCM_TRIUMPH3_SUPPORT */
            {
    #if defined(BCM_TRIDENT3_SUPPORT)
                if (soc_feature(unit, soc_feature_base_valid)) {
                    soc_mem_field32_set(unit, egr_xlate_mem,
                        egr_vlan_xlate_entry, DATA_TYPEf, 0x3);
                    soc_mem_field32_set(unit, egr_xlate_mem,
                        egr_vlan_xlate_entry, KEY_TYPEf, 0x3);
                } else
    #endif
                {
                    soc_mem_field32_set(unit, egr_xlate_mem,
                        egr_vlan_xlate_entry, ENTRY_TYPEf, 0x3);
                }
            }
    #if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_base_valid)) {
                soc_mem_field32_set(unit, egr_xlate_mem,
                       egr_vlan_xlate_entry, BASE_VALID_0f, 0x3);
                soc_mem_field32_set(unit, egr_xlate_mem,
                       egr_vlan_xlate_entry, BASE_VALID_1f, 0x7);
            } else
    #endif
            {
                soc_mem_field32_set(unit, egr_xlate_mem,
                    egr_vlan_xlate_entry, VALIDf, 0x1);
            }
            /* MIIM_ISID key */
            soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry, MIM_ISID__VFIf, vfi);
            /* MIIM_ISID data */
            soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry, MIM_ISID__ISIDf,
                                            info->lookup_id);

            rv = soc_mem_search(unit, egr_xlate_mem, MEM_BLOCK_ANY, &index,
                                egr_vlan_xlate_entry, egr_vlan_xlate_entry, 0);
            if (rv == SOC_E_NONE) {
                if (!((info->flags & BCM_MIM_VPN_WITH_ID) &&
                     (info->flags & BCM_MIM_VPN_REPLACE))) {
                    _bcm_vfi_free(unit, _bcmVfiTypeMim, vfi);
                    MIM_UNLOCK(unit);
                    return BCM_E_EXISTS;
                }
            } else if (rv != SOC_E_NOT_FOUND) {
                _bcm_vfi_free(unit, _bcmVfiTypeMim, vfi);
                MIM_UNLOCK(unit);
                return rv;
            }

            if ((info->flags & BCM_MIM_VPN_WITH_ID) &&
                (info->flags & BCM_MIM_VPN_REPLACE) &&
                (old_info.lookup_id >= 0)){
                /* Delete old lookup ISID from EGR_VLAN_XLATE entry */
                rv = _bcm_tr2_mim_egr_vxlt_sd_tag_actions_delete(unit, egr_vlan_xlate_entry);
                if (BCM_FAILURE(rv)) {
                    MIM_UNLOCK(unit);
                    return rv;
                }

                rv = soc_mem_delete(unit, egr_xlate_mem, MEM_BLOCK_ALL, egr_vlan_xlate_entry);
                if (BCM_FAILURE(rv)) {
                    MIM_UNLOCK(unit);
                    return rv;
                }
            }

            if (info->flags & BCM_MIM_VPN_QOS_SDTAG_NO_ACTION) {
                soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                        MIM_ISID__SD_TAG_ACTION_IF_PRESENTf, 0);
            }

            if (info->flags & BCM_MIM_VPN_QOS_SDTAG_FIX_PRIORITY) {
                soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                    MIM_ISID__SD_TAG_DOT1P_PRI_SELECTf, 0);
                soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                    MIM_ISID__SD_TAG_ACTION_IF_PRESENTf, 0x5);
                soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                    MIM_ISID__NEW_PRIf, info->int_pri & 0x7);
                soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                    MIM_ISID__NEW_CFIf, ((info->int_pri >> 0x3) & 0x1));
            }

            if (info->flags & BCM_MIM_VPN_QOS_SDTAG_MAP_INT_PRIORITY) {
                rv = _bcm_tr2_qos_id2idx(unit, info->qos_map_id, &hw_idx);
                if (BCM_FAILURE(rv)) {
                    MIM_UNLOCK(unit);
                    return rv;
                }
                soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                    MIM_ISID__SD_TAG_DOT1P_PRI_SELECTf, 0x1);
                soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                    MIM_ISID__SD_TAG_ACTION_IF_PRESENTf, 0x5);
                soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                    MIM_ISID__DOT1P_MAPPING_PTRf, hw_idx);
            }

            if (info->flags & BCM_MIM_VPN_QOS_ITAG_FIX_PRIORITY) {
                soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                    MIM_ISID__ISID_DOT1P_PRI_SELECTf, 0);
                soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                    MIM_ISID__NEW_PRIf, info->int_pri & 0x7);
                soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                    MIM_ISID__NEW_CFIf, ((info->int_pri >> 0x3) & 0x1));
                soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                    MIM_ISID__NEW_CFIf, ((info->int_pri >> 0x3) & 0x1));
            }

            if (info->flags & BCM_MIM_VPN_QOS_ITAG_MAP_SDTAG_PRIORITY) {
                soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                    MIM_ISID__ISID_DOT1P_PRI_SELECTf, 0x1);
            }

            if (info->flags & BCM_MIM_VPN_EGRESS_SERVICE_TPID_REPLACE) {
                if (soc_feature(unit, soc_feature_evt_replace_tpid_only)) {
                    rv = _bcm_tr2_mim_egr_vxlt_sd_tag_actions(unit, NULL,
                                                              info,
                                                              egr_vlan_xlate_entry);
                    if (BCM_FAILURE(rv)) {
                        MIM_UNLOCK(unit);
                        return rv;
                    }
                } else {
                    MIM_UNLOCK(unit);
                    return BCM_E_UNAVAIL;
                }
            }

            rv = soc_mem_insert(unit, egr_xlate_mem, MEM_BLOCK_ALL, egr_vlan_xlate_entry);
            if (rv < 0) {
                MIM_UNLOCK(unit);
                return rv;
            }
        }
    } else if ((info->flags & BCM_MIM_VPN_REPLACE) &&
               (old_info.lookup_id >= 0)) { 
        sal_memset(&mpls_entry, 0, sizeof(mpls_entry));
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            soc_mem_field32_set(unit, mpls_mem, &mpls_entry, KEY_TYPE_0f, 0x18);
            soc_mem_field32_set(unit, mpls_mem, &mpls_entry, KEY_TYPE_1f, 0x18);
            soc_mem_field32_set(unit, mpls_mem, &mpls_entry, VALID_0f, 0x1);
            soc_mem_field32_set(unit, mpls_mem, &mpls_entry, VALID_1f, 0x1);
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
        {
    	    soc_mem_field32_set(unit, mpls_mem, &mpls_entry, KEY_TYPEf, 0x2);
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_base_valid)) {
                soc_mem_field32_set(unit, mpls_mem, &mpls_entry, BASE_VALID_0f, 3); 
                soc_mem_field32_set(unit, mpls_mem, &mpls_entry, BASE_VALID_1f, 7); 
            } else
#endif
            {
                soc_mem_field32_set(unit, mpls_mem, &mpls_entry, VALIDf, 0x1);
            }
	    }
        /* key */
        soc_mem_field32_set(unit, mpls_mem, &mpls_entry, MIM_ISID__ISIDf, old_info.lookup_id);
        /* data */
        soc_mem_field32_set(unit, mpls_mem, &mpls_entry, MIM_ISID__VFIf, vfi);
        soc_mem_delete(unit, mpls_mem, MEM_BLOCK_ANY, &mpls_entry);

#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_mim_decoupled_mode)) {
            rv = _bcm_td3_mim_untagged_payload_vlan_delete(unit,
                                        old_info.lookup_id, 0);
            if (BCM_FAILURE(rv)) {
                MIM_UNLOCK(unit);
                return rv;
            }
        } else
#endif
        {
            /* Program the VFI to ISID mapping */
            sal_memset(egr_vlan_xlate_entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
    #if defined(BCM_TRIUMPH3_SUPPORT)
            if (SOC_IS_TRIUMPH3(unit)) {
                soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                                                  KEY_TYPEf, 0x2); /* MIM_ISID */
            } else
    #endif /* BCM_TRIUMPH3_SUPPORT */
            {
    #if defined(BCM_TRIDENT3_SUPPORT)
                if (soc_feature(unit, soc_feature_base_valid)) {
                    soc_mem_field32_set(unit, egr_xlate_mem,
                        egr_vlan_xlate_entry, DATA_TYPEf, 0x3);
                    soc_mem_field32_set(unit, egr_xlate_mem,
                        egr_vlan_xlate_entry, KEY_TYPEf, 0x3);
                } else
    #endif
                {
                    soc_mem_field32_set(unit, egr_xlate_mem,
                        egr_vlan_xlate_entry, ENTRY_TYPEf, 0x3);
                }
            }
    #if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_base_valid)) {
                soc_mem_field32_set(unit, egr_xlate_mem,
                       egr_vlan_xlate_entry, BASE_VALID_0f, 0x3);
                soc_mem_field32_set(unit, egr_xlate_mem,
                       egr_vlan_xlate_entry, BASE_VALID_1f, 0x7);
            } else
    #endif
            {
                soc_mem_field32_set(unit, egr_xlate_mem,
                    egr_vlan_xlate_entry, VALIDf, 0x1);
            }

            /* MIIM_ISID key */
            soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry, MIM_ISID__VFIf, vfi);
            /* MIIM_ISID data */
            soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry, MIM_ISID__ISIDf,
    				        old_info.lookup_id);
            rv = _bcm_tr2_mim_egr_vxlt_sd_tag_actions_delete(unit, egr_vlan_xlate_entry);
            if (BCM_FAILURE(rv)) {
                MIM_UNLOCK(unit);
                return rv;
            }
            soc_mem_delete(unit, egr_xlate_mem, MEM_BLOCK_ALL, egr_vlan_xlate_entry);
        }
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    MIM_UNLOCK(unit);

    _BCM_MIM_VPN_SET(info->vpn, _BCM_MIM_VPN_TYPE_MIM, vfi);
    return rv;
}

/*
 * Function:
 *      bcm_mim_vpn_destroy
 * Purpose:
 *      Delete a VPN instance
 * Parameters:
 *      unit - Device Number
 *      vpn - VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr2_mim_vpn_destroy(int unit, bcm_mim_vpn_t vpn)
{
    int vfi, rv, tpid_en, i;
    uint32 vfi_1_entry[SOC_MAX_MEM_WORDS] = {0};
    vfi_entry_t vfi_entry;
    uint32 mpls_entry[SOC_MAX_MEM_WORDS] = {0};
    uint32 egr_vlan_xlate_entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mpls_mem = MPLS_ENTRYm;
    int num_vfi;
    bcm_vpn_t mim_vpn_min=0;
    int ref_count, proto_pkt_idx;
    soc_mem_t egr_xlate_mem;
    soc_mem_t vfi_mem;
    soc_mem_t vfi_1_mem;
    uint32 vfi_entry_data[SOC_MAX_MEM_WORDS] = {0};
    int index;

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    int policer=0;
#endif 
    MIM_INIT(unit);
    MIM_LOCK(unit);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        egr_xlate_mem = EGR_VLAN_XLATE_1_DOUBLEm;
    } else
#endif
    {
        egr_xlate_mem = EGR_VLAN_XLATEm;
    }

    num_vfi = soc_mem_index_count(unit, VFIm);
    _BCM_MIM_VPN_SET(mim_vpn_min, _BCM_MIM_VPN_TYPE_MIM, 0);
    if ( vpn < mim_vpn_min || vpn > (mim_vpn_min+num_vfi-1) ) {
        MIM_UNLOCK(unit);
        return BCM_E_PARAM;
    }

    _BCM_MIM_VPN_GET(vfi, _BCM_MIM_VPN_TYPE_MIM, vpn);
    if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
        MIM_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    }

    /* Delete all the MIM ports on this VPN */
    rv = bcm_tr2_mim_port_delete_all(unit, vpn);
    if (BCM_FAILURE(rv)) {
        MIM_UNLOCK(unit);
        return rv;
    } 
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_global_meter)) {
        rv = _bcm_esw_get_policer_from_table(unit,
                VFIm, vfi, &vfi_entry, &policer, 0); 
        if(BCM_SUCCESS(rv)) {
             _bcm_esw_policer_decrement_ref_count(unit, policer);
        } else {
            MIM_UNLOCK(unit);
            return rv;
        }
    }
#endif /* BCM_KATANA_SUPPORT or BCM_TRIUMPH3_SUPPORT */
    if (SOC_MEM_FIELD_VALID(unit, VFIm, PROTOCOL_PKT_INDEXf)) {
        rv = READ_VFIm(unit, MEM_BLOCK_ALL, vfi, &vfi_entry);
        if (BCM_FAILURE(rv)) {
            MIM_UNLOCK(unit);
            return rv;
        }
        proto_pkt_idx = soc_VFIm_field32_get(unit, &vfi_entry,
                                             PROTOCOL_PKT_INDEXf);
        rv = _bcm_prot_pkt_ctrl_ref_count_get(unit, proto_pkt_idx,
                                              &ref_count);
        if (BCM_FAILURE(rv)) {
            MIM_UNLOCK(unit);
            return rv;
        }
        if (ref_count > 0) {
            rv = _bcm_prot_pkt_ctrl_delete(unit, proto_pkt_idx);
            if (BCM_FAILURE(rv)) {
                MIM_UNLOCK(unit);
                return rv;
            }
        }
    }

    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        /* Release Service counter, if any */
        _bcm_esw_flex_stat_handle_free(unit, _bcmFlexStatTypeService,
                                       vpn);
    }
    sal_memset(&mpls_entry, 0, sizeof(mpls_entry));
#if defined(BCM_TRIUMPH3_SUPPORT)    
    if (SOC_IS_TRIUMPH3(unit)) {
        mpls_mem = MPLS_ENTRY_EXTDm;
        soc_mem_field32_set(unit, mpls_mem, &mpls_entry, KEY_TYPE_0f, 0x18);
        soc_mem_field32_set(unit, mpls_mem, &mpls_entry, KEY_TYPE_1f, 0x18);
        soc_mem_field32_set(unit, mpls_mem, &mpls_entry, VALID_0f, 0x1);
        soc_mem_field32_set(unit, mpls_mem, &mpls_entry, VALID_1f, 0x1);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
        mpls_mem = MPLS_ENTRYm;
        soc_mem_field32_set(unit, mpls_mem, &mpls_entry, KEY_TYPEf, 0x2);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            soc_mem_field32_set(unit, mpls_mem, &mpls_entry, BASE_VALID_0f, 3);
            soc_mem_field32_set(unit, mpls_mem, &mpls_entry, BASE_VALID_1f, 7);
            soc_mem_field32_set(unit, mpls_mem, &mpls_entry, DATA_TYPEf, 0x2);
        } else
#endif
        {
            soc_mem_field32_set(unit, mpls_mem, &mpls_entry, VALIDf, 0x1);
        }

    }
    soc_mem_field32_set(unit, mpls_mem, &mpls_entry, MIM_ISID__ISIDf, VPN_ISID(unit, vfi));
    rv = soc_mem_delete(unit, mpls_mem, MEM_BLOCK_ANY, &mpls_entry);
    if (BCM_FAILURE(rv)) {
        MIM_UNLOCK(unit);
        return rv;
    } 

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_mim_decoupled_mode)) {
        rv = _bcm_td3_mim_flex_vpn_egr_vxlt_op(unit, NULL, vfi,
                      _MIM_EVXLT_OP_DELETE);
        if (BCM_FAILURE(rv)) {
            MIM_UNLOCK(unit);
            return rv;
        }
        rv = _bcm_td3_mim_untagged_payload_vlan_delete(unit,
                        VPN_ISID(unit, vfi), 0);
        if (BCM_FAILURE(rv)) {
            MIM_UNLOCK(unit);
            return rv;
        }
    } else
#endif
    {
        sal_memset(egr_vlan_xlate_entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                                            KEY_TYPEf, 0x2); /* MIM_ISID */
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
        {
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_base_valid)) {
                soc_mem_field32_set(unit, egr_xlate_mem,
                    egr_vlan_xlate_entry, DATA_TYPEf, 0x3);
                soc_mem_field32_set(unit, egr_xlate_mem,
                    egr_vlan_xlate_entry, KEY_TYPEf, 0x3);
            } else
#endif
            {
                soc_mem_field32_set(unit, egr_xlate_mem,
                    egr_vlan_xlate_entry, ENTRY_TYPEf, 0x3);
            }
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            soc_mem_field32_set(unit, egr_xlate_mem,
                   egr_vlan_xlate_entry, BASE_VALID_0f, 0x3);
            soc_mem_field32_set(unit, egr_xlate_mem,
                   egr_vlan_xlate_entry, BASE_VALID_1f, 0x7);
        } else
#endif
        {
            soc_mem_field32_set(unit, egr_xlate_mem,
                egr_vlan_xlate_entry, VALIDf, 0x1);
        }

        /* MIIM_ISID key */
        soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry, MIM_ISID__VFIf, vfi);
        rv = soc_mem_search(unit, egr_xlate_mem, MEM_BLOCK_ANY, &index,
                            egr_vlan_xlate_entry, egr_vlan_xlate_entry, 0);
        rv = _bcm_tr2_mim_egr_vxlt_sd_tag_actions_delete(unit, egr_vlan_xlate_entry);
        if (BCM_FAILURE(rv)) {
            MIM_UNLOCK(unit);
            return rv;
        }
        rv = soc_mem_delete(unit, egr_xlate_mem, MEM_BLOCK_ANY, egr_vlan_xlate_entry);
        if (BCM_FAILURE(rv)) {
            MIM_UNLOCK(unit);
            return rv;
        }
    }
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, VFIm, SD_TAG_MODEf)) {
        vfi_mem = VFIm;
        vfi_1_mem = VFI_ATTRS_1m;
    } else
#endif
    {
        vfi_mem = VFI_1m;
        vfi_1_mem = VFI_1m;
    }
    rv = soc_mem_read(unit, vfi_mem, MEM_BLOCK_ANY, vfi, vfi_1_entry);
    if (BCM_FAILURE(rv)) {
        MIM_UNLOCK(unit);
        return rv;
    }
    rv = soc_mem_read(unit, vfi_1_mem, MEM_BLOCK_ANY, vfi, vfi_entry_data);
    if (BCM_FAILURE(rv)) {
        MIM_UNLOCK(unit);
        return rv;
    }

    /* Delete TPID if applicable */
    if (soc_mem_field32_get(unit, vfi_mem, vfi_1_entry, SD_TAG_MODEf)) {
        tpid_en = soc_mem_field32_get(unit, vfi_1_mem, vfi_entry_data, TPID_ENABLEf);
        for (i = 0; i < 4; i++) {
            if (tpid_en & (1 << i)) {
               (void)_bcm_fb2_outer_tpid_entry_delete(unit, i);
                break;
            }
        }
    }
    sal_memset(vfi_1_entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
    rv = soc_mem_write(unit, vfi_1_mem, MEM_BLOCK_ALL, vfi, vfi_1_entry);
    if (BCM_FAILURE(rv)) {
        MIM_UNLOCK(unit);
        return rv;
    } 

    VPN_ISID(unit, vfi) = 0;
    (void) _bcm_vfi_free(unit, _bcmVfiTypeMim, vfi);

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    MIM_UNLOCK(unit);
    return rv;
}

STATIC int
_bcm_tr2_mim_vpn_np_destroy(int unit)
{
    int vp, rv = BCM_E_NONE;
    uint32 num_vp;
    source_vp_entry_t *psvp;
    uint8 *source_vp_buf = NULL;
    int network_port = 0;
    _bcm_vp_info_t vp_info;

    MIM_INIT(unit);

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    source_vp_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, SOURCE_VPm),
                                 "SOURCE_VP buffer");
    if (source_vp_buf == NULL) {
        goto done;
    }
    rv = soc_mem_read_range(unit, SOURCE_VPm, MEM_BLOCK_ANY, 
                           0, num_vp-1, source_vp_buf); 
    if (rv < 0) { 
        goto done; 
    }

    for (vp = 0; vp < num_vp; vp++) {
        network_port = FALSE;
        psvp = soc_mem_table_idx_to_pointer 
                (unit, SOURCE_VPm, source_vp_entry_t *, source_vp_buf, vp);
        if (soc_SOURCE_VPm_field32_get( unit, psvp, ENTRY_TYPEf) != 0) {
            if(_bcm_vp_info_get(unit, vp, &vp_info) == BCM_E_NONE) {
                if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
                    network_port = TRUE;
                }
            }

            if(network_port) {
                if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
                    rv = _bcm_tr2_mim_port_delete(unit, 0, vp);
                    if (rv < 0) {
                        goto done;
                    }
                }
            }
        }
    }
done:
    if ( source_vp_buf != NULL ){
        soc_cm_sfree( unit, source_vp_buf); 
    }
    return rv;
}

/*
 * Function:
 *      bcm_mim_vpn_destroy_all
 * Purpose:
 *      Delete al VPN instances
 * Parameters:
 *      unit - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_mim_vpn_destroy_all(int unit)
{
    int num_vfi, i, rv = BCM_E_NONE;
    bcm_vpn_t vpn;

    MIM_INIT(unit);
    num_vfi = soc_mem_index_count(unit, VFIm);
    for (i = 0; i < num_vfi; i++) {
        if (_bcm_vfi_used_get(unit, i, _bcmVfiTypeMim)) {
            _BCM_MIM_VPN_SET(vpn, _BCM_MIM_VPN_TYPE_MIM, i);
            rv = bcm_tr2_mim_vpn_destroy(unit, vpn);
            if (rv < 0) {
                goto done;
            }
        }
    }
    rv = _bcm_tr2_mim_vpn_np_destroy(unit);
done:
    return rv;
}

/*
 * Function:
 *      bcm_mim_vpn_get
 * Purpose:
 *      Get information about a VPN instance
 * Parameters:
 *      unit  - (IN)  Device Number
 *      vpn   - (IN)  VPN instance ID
 *      info  - (OUT) VPN configuration info
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr2_mim_vpn_get(int unit, bcm_mim_vpn_t vpn, 
                    bcm_mim_vpn_config_t *info)
{
    int vfi, tpid_en, i;
    vfi_entry_t vfi_entry;
    uint32 vfi_entry_data[SOC_MAX_MEM_WORDS] = {0};
    uint32 vfi_1_entry[SOC_MAX_MEM_WORDS] = {0};
    int num_vfi;
    bcm_vpn_t mim_vpn_min=0;
    int is_eline = 0;
    int vpn_type;
    int proto_pkt_idx, rv;
    int bc_index = 0;
    int uuc_index = 0;
    int umc_index = 0;
    soc_mem_t vfi_mem;
    soc_mem_t vfi_1_mem;
    soc_mem_t evx_mem = EGR_VLAN_XLATEm;
    uint32 egr_vlan_xlate_entry[SOC_MAX_MEM_WORDS] = {0};
    int index;
    uint8 new_pri = 0;
    int hw_idx;

    MIM_INIT(unit);
    num_vfi = soc_mem_index_count(unit, VFIm);

    _BCM_MIM_VPN_SET(mim_vpn_min, _BCM_MIM_VPN_TYPE_MIM, 0);
    if ( vpn < mim_vpn_min || vpn > (mim_vpn_min+num_vfi-1) ) {
        return BCM_E_PARAM;
    }

    _BCM_MIM_VPN_GET(vfi, _BCM_MIM_VPN_TYPE_MIM, vpn);

    if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
        return BCM_E_NOT_FOUND;
    }
   
    /* Initialize VPN configuration information */
    bcm_mim_vpn_config_t_init(info);
    info->vpn = vpn; 

    /* check if ELINE vpn service */
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, VFIm, MEM_BLOCK_ANY, vfi, 
                                     &vfi_entry));

    is_eline = soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf);

    if (is_eline) {
        vpn_type = BCM_MIM_VPN_ELINE;
    } else {
        vpn_type = BCM_MIM_VPN_MIM;
    }

    info->flags |= vpn_type;

    /* if point-to_point vpn BC_INDEX, UUC_INDEX, UMC_INDEX not relevant */
    if (!is_eline) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_generic_dest)) {
            uint32 dt;

            bc_index = soc_mem_field32_dest_get(unit, VFIm,
                            &vfi_entry, BC_DESTINATIONf, &dt);
            if (dt != SOC_MEM_FIF_DEST_IPMC) {
                return BCM_E_INTERNAL;
            }

            uuc_index = soc_mem_field32_dest_get(unit, VFIm,
                            &vfi_entry, UUC_DESTINATIONf, &dt);
            if (dt != SOC_MEM_FIF_DEST_IPMC) {
                return BCM_E_INTERNAL;
            }

            umc_index = soc_mem_field32_dest_get(unit, VFIm,
                            &vfi_entry, UMC_DESTINATIONf, &dt);
            if (dt != SOC_MEM_FIF_DEST_IPMC) {
                return BCM_E_INTERNAL;
            }
        } else
#endif
        {
            bc_index = soc_VFIm_field32_get(unit, &vfi_entry, BC_INDEXf);
            uuc_index = soc_VFIm_field32_get(unit, &vfi_entry, UUC_INDEXf);
            umc_index = soc_VFIm_field32_get(unit, &vfi_entry, UMC_INDEXf);
        }

        _BCM_MULTICAST_GROUP_SET(info->broadcast_group,
                            _BCM_MULTICAST_TYPE_MIM, bc_index);
        _BCM_MULTICAST_GROUP_SET(info->unknown_unicast_group,
                            _BCM_MULTICAST_TYPE_MIM, uuc_index);
        _BCM_MULTICAST_GROUP_SET(info->unknown_multicast_group,
                            _BCM_MULTICAST_TYPE_MIM, umc_index);
    }

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_global_meter)) {
         _bcm_esw_get_policer_from_table(unit, VFIm, vfi, &vfi_entry, 
                                                    &info->policer_id, 1); 
    }
#endif /* BCM_KATANA_SUPPORT or BCM_TRIUMPH3_SUPPORT */
    info->lookup_id = VPN_ISID(unit, vfi);
    if (SOC_MEM_FIELD_VALID(unit,VFIm, PROTOCOL_PKT_INDEXf)) {
        proto_pkt_idx = soc_VFIm_field32_get(unit, &vfi_entry,
                                             PROTOCOL_PKT_INDEXf);
        rv = _bcm_xgs3_protocol_pkt_ctrl_get(unit, proto_pkt_idx,
                                             &info->protocol_pkt);
        if (BCM_FAILURE(rv) && (rv != BCM_E_UNAVAIL)) {
            return rv;
        }
    }

    if (SOC_MEM_FIELD_VALID(unit, VFIm, TRUST_DOT1P_PTRf)) {
        hw_idx = soc_mem_field32_get(unit, VFIm, &vfi_entry, TRUST_DOT1P_PTRf);
        rv = _bcm_tr2_qos_idx2id(unit, hw_idx,
                   _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP, &info->qos_map_id);
        if (BCM_SUCCESS(rv)) {
            info->flags |= BCM_MIM_VPN_QOS_INGRESS_MAP_INT_PRIORITY;
        } else {
            info->qos_map_id = 0;
        }
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, VFIm, SD_TAG_MODEf)) {
        vfi_mem = VFIm;
        vfi_1_mem = VFI_ATTRS_1m;
        evx_mem = EGR_VLAN_XLATE_1_DOUBLEm;
    } else
#endif
    {
        vfi_mem = VFI_1m;
        vfi_1_mem = VFI_1m;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, vfi_mem,
                      MEM_BLOCK_ANY, vfi, vfi_entry_data));
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, vfi_1_mem,
                      MEM_BLOCK_ANY, vfi, vfi_1_entry));
    if (soc_mem_field32_get(unit, vfi_mem, vfi_entry_data, SD_TAG_MODEf)) {
        info->flags |= BCM_MIM_VPN_MATCH_SERVICE_VLAN_TPID;
        tpid_en = soc_mem_field32_get(unit, vfi_1_mem, &vfi_1_entry, TPID_ENABLEf);
        for (i = 0; i < 4; i++) {
            if (tpid_en & (1 << i)) {
                _bcm_fb2_outer_tpid_entry_get(unit, 
                                      &info->match_service_tpid, i);
                break;
            }
        }
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_mim_decoupled_mode)) {
        rv = _bcm_td3_mim_flex_vpn_egr_vxlt_op(unit, info, vfi,
                  _MIM_EVXLT_OP_GET);
        BCM_IF_ERROR_RETURN(rv);
        rv = _bcm_td3_mim_untagged_payload_vlan_get(unit,info, NULL, 0);
        BCM_IF_ERROR_RETURN(rv);
    } else
#endif
    {
        sal_memset(egr_vlan_xlate_entry, 0, sizeof(egr_vlan_xlate_entry));
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            soc_mem_field32_set(unit, evx_mem, egr_vlan_xlate_entry,
                                KEY_TYPEf, 0x2); /* MIM_ISID */
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            soc_mem_field32_set(unit, evx_mem, egr_vlan_xlate_entry, DATA_TYPEf, 0x3);
            soc_mem_field32_set(unit, evx_mem, egr_vlan_xlate_entry, KEY_TYPEf, 0x3);
        } else
#endif
        {
            soc_mem_field32_set(unit, evx_mem, egr_vlan_xlate_entry,
                                ENTRY_TYPEf, 0x3);
        }
        if (soc_feature(unit, soc_feature_base_valid)) {
            soc_mem_field32_set(unit, evx_mem, egr_vlan_xlate_entry, BASE_VALID_0f, 0x3);
            soc_mem_field32_set(unit, evx_mem, egr_vlan_xlate_entry, BASE_VALID_1f, 0x7);
        } else {
            soc_mem_field32_set(unit, evx_mem, egr_vlan_xlate_entry, VALIDf, 0x1);
        }
        /* MIIM_ISID key */
        soc_mem_field32_set(unit, evx_mem, egr_vlan_xlate_entry, MIM_ISID__VFIf, vfi);
        rv = soc_mem_search(unit, evx_mem, MEM_BLOCK_ANY, &index,
                            egr_vlan_xlate_entry, egr_vlan_xlate_entry, 0);
        if (rv == SOC_E_NONE)
        {
            new_pri =
                soc_mem_field32_get(unit, evx_mem, &egr_vlan_xlate_entry,
                                                MIM_ISID__NEW_PRIf);
            new_pri |=
                ((soc_mem_field32_get(unit, evx_mem, &egr_vlan_xlate_entry,
                                                  MIM_ISID__NEW_CFIf) & 0x1) << 3);
            if (new_pri != 0) {
                if (soc_mem_field32_get(unit, evx_mem, &egr_vlan_xlate_entry,
                   MIM_ISID__SD_TAG_ACTION_IF_PRESENTf) == 0) {
                    info->flags |= BCM_MIM_VPN_QOS_ITAG_FIX_PRIORITY;
                    info->int_pri = new_pri;
                }
            }

            rv = _bcm_tr2_mim_egr_vxlt_sd_tag_actions_get(unit, NULL, info,
                                                          egr_vlan_xlate_entry);
        }
    }
    return rv;
}

/*
 * Function:
 *      bcm_mim_vpn_traverse
 * Purpose:
 *      Get information about a VPN instance
 * Parameters:
 *      unit  - (IN)  Device Number
 *      cb    - (IN)  User-provided callback
 *      info  - (IN/OUT) Cookie
 * Returns:
 *      BCM_E_XXX
 */

int bcm_tr2_mim_vpn_traverse(int unit, bcm_mim_vpn_traverse_cb cb, 
                             void *user_data)
{
    int i, index_min, index_max, rv = BCM_E_NONE;
    int vpn;
    bcm_mim_vpn_config_t info;

    MIM_INIT(unit);

    index_min = soc_mem_index_min(unit, VFIm);
    index_max = soc_mem_index_max(unit, VFIm);

    MIM_LOCK(unit);
    for (i = index_min; i <= index_max; i++) {
        if (!_bcm_vfi_used_get(unit, i, _bcmVfiTypeMim)) {
            continue;
        }
        bcm_mim_vpn_config_t_init(&info);
        _BCM_MIM_VPN_SET(vpn, _BCM_MIM_VPN_TYPE_MIM, i);
        rv = bcm_tr2_mim_vpn_get(unit, vpn, &info);
        if (BCM_FAILURE(rv)) {
            MIM_UNLOCK(unit);
            return rv;
        }
        rv = cb(unit, &info, user_data);
        if (BCM_FAILURE(rv)) {
            MIM_UNLOCK(unit); 
            return rv;
        }
    }
    MIM_UNLOCK(unit);
    return rv;
}

STATIC int
_bcm_tr2_mim_l3_intf_add(int unit, _bcm_l3_intf_cfg_t *if_info)
{
    int i, num_intf;
    egr_l3_intf_entry_t egr_intf;
    bcm_mac_t hw_mac;
    num_intf = soc_mem_index_count(unit, EGR_L3_INTFm);
    for (i = 0; i < num_intf; i++) {
        if (_BCM_MIM_INTF_USED_GET(unit, i)) {
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
    _BCM_MIM_INTF_USED_SET(unit, if_info->l3i_index);
    return BCM_E_NONE;
}


STATIC int
_bcm_tr2_mim_egr_sd_tag_actions(int unit, bcm_mim_port_t *mim_port, 
                                _bcm_tr2_egr_nh_info_t *egr_nh_info)
{
    int rv = BCM_E_NONE, tpid_index = -1;
    if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_ADD) {
        if (mim_port->egress_service_vlan >= BCM_VLAN_INVALID) { 
            if (soc_feature(unit, soc_feature_vp_sharing)) {
                egr_nh_info->sd_tag_vlan = BCM_VLAN_INVALID;    
            } else {
                return  BCM_E_PARAM;
            }
        }

        if (egr_nh_info->sd_tag_vlan != BCM_VLAN_INVALID) {
            egr_nh_info->sd_tag_vlan = mim_port->egress_service_vlan; 
        }
        egr_nh_info->sd_tag_action_not_present = 0x1; /* ADD */
    }
    if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TPID_REPLACE) {
        if (mim_port->egress_service_vlan >= BCM_VLAN_INVALID) { 
            if (soc_feature(unit, soc_feature_vp_sharing)) {
                egr_nh_info->sd_tag_vlan = BCM_VLAN_INVALID;    
            } else {
                return  BCM_E_PARAM;
            }
        }
        /* REPLACE_VID_TPID */
        if (egr_nh_info->sd_tag_vlan != BCM_VLAN_INVALID) {
            egr_nh_info->sd_tag_vlan = mim_port->egress_service_vlan; 
        }
        egr_nh_info->sd_tag_action_present = 0x1;
    } else if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_REPLACE) {
        if (mim_port->egress_service_vlan >= BCM_VLAN_INVALID) { 
            if (soc_feature(unit, soc_feature_vp_sharing)) {
                egr_nh_info->sd_tag_vlan = BCM_VLAN_INVALID;    
            } else {
                return  BCM_E_PARAM;
            }
        }
        /* REPLACE_VID_ONLY */
        if (egr_nh_info->sd_tag_vlan != BCM_VLAN_INVALID) {
            egr_nh_info->sd_tag_vlan = mim_port->egress_service_vlan; 
        }
        egr_nh_info->sd_tag_action_present = 0x2;
    } else if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_DELETE) {
        egr_nh_info->sd_tag_action_present = 0x3; /* DELETE */
    } else if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_PRI_TPID_REPLACE) {
        if (mim_port->egress_service_vlan >= BCM_VLAN_INVALID) { 
            if (soc_feature(unit, soc_feature_vp_sharing)) {
                egr_nh_info->sd_tag_vlan = BCM_VLAN_INVALID;    
            } else {
                return  BCM_E_PARAM;
            }
        }
        /*  REPLACE_VID_PRI_TPID */
        if (egr_nh_info->sd_tag_vlan != BCM_VLAN_INVALID) {
            egr_nh_info->sd_tag_vlan = mim_port->egress_service_vlan; 
        }
        egr_nh_info->sd_tag_pri = mim_port->egress_service_pri;
        egr_nh_info->sd_tag_cfi = mim_port->egress_service_cfi;
        egr_nh_info->sd_tag_action_present = 0x4;
    } else if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_PRI_REPLACE ) {
        if (mim_port->egress_service_vlan >= BCM_VLAN_INVALID) { 
            if (soc_feature(unit, soc_feature_vp_sharing)) {
                egr_nh_info->sd_tag_vlan = BCM_VLAN_INVALID;    
            } else {
                return  BCM_E_PARAM;
            }
        }
        /* REPLACE_VID_PRI ONLY */
        if (egr_nh_info->sd_tag_vlan != BCM_VLAN_INVALID) {
            egr_nh_info->sd_tag_vlan = mim_port->egress_service_vlan; 
        }
        egr_nh_info->sd_tag_pri = mim_port->egress_service_pri;
        egr_nh_info->sd_tag_cfi = mim_port->egress_service_cfi;
        egr_nh_info->sd_tag_action_present = 0x5;
    }
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
      else if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_PRI_REPLACE) {
        egr_nh_info->sd_tag_pri = mim_port->egress_service_pri;
        egr_nh_info->sd_tag_cfi = mim_port->egress_service_cfi;
        egr_nh_info->sd_tag_action_present = 0x6;
    } else if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_TPID_REPLACE) {
        /* TPID value is used */
        rv = _bcm_fb2_outer_tpid_entry_add(unit, mim_port->egress_service_tpid, 
                                           &tpid_index);
        BCM_IF_ERROR_RETURN(rv);
        egr_nh_info->tpid_index = tpid_index;
        egr_nh_info->sd_tag_action_present = 0x7;
    } 
#endif
    if ((mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_ADD)
        || (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TPID_REPLACE)
        || (mim_port->flags &
                        BCM_MIM_PORT_EGRESS_SERVICE_VLAN_PRI_TPID_REPLACE))
    {
        /* TPID value is used */
        rv = _bcm_fb2_outer_tpid_entry_add(unit, mim_port->egress_service_tpid, 
                                           &tpid_index);
        BCM_IF_ERROR_RETURN(rv);
        egr_nh_info->tpid_index = tpid_index;
    }

    /* Set fixed priority */
    if (mim_port->flags & BCM_MIM_PORT_FIX_PRIORITY) {
        if (mim_port->flags & BCM_MIM_PORT_TYPE_BACKBONE) {
            egr_nh_info->sd_tag_pri = mim_port->int_pri & 0x7;
            egr_nh_info->sd_tag_cfi = ((mim_port->int_pri >> 0x3) & 0x1);
        }
    }

    return rv;
}

int
_bcm_tr2_mim_egr_vxlt_sd_tag_actions(int unit, bcm_mim_port_t *mim_port,
                                     bcm_mim_vpn_config_t *vpn_info,
                                     uint32 *evxlt_entry)
{
    int rv = BCM_E_NONE, tpid_index = -1;
    int sd_default_action = 0, sd_tag_action = 0;
    soc_mem_t egr_xlate_mem;
    
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            egr_xlate_mem = EGR_VLAN_XLATE_1_DOUBLEm;
        } else
#endif
        {
            egr_xlate_mem = EGR_VLAN_XLATEm;
        }

    if (mim_port != NULL) {
        if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_ADD) {
            if (!BCM_VLAN_VALID(mim_port->egress_service_vlan)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, egr_xlate_mem, evxlt_entry, 
                                        MIM_ISID__SD_TAG_VIDf, 
                                        mim_port->egress_service_vlan);
            sd_default_action = 0x1; /* ADD */
        }
        if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TPID_REPLACE) {
            if (!BCM_VLAN_VALID(mim_port->egress_service_vlan)) {
                return BCM_E_PARAM;
            }
            /* REPLACE_VID_TPID */
            soc_mem_field32_set(unit, egr_xlate_mem, evxlt_entry, 
                                        MIM_ISID__SD_TAG_VIDf, 
                                        mim_port->egress_service_vlan); 
            sd_tag_action = 0x1; /* Replace Vid and TPID */
        } else if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_REPLACE) {
            if (!BCM_VLAN_VALID(mim_port->egress_service_vlan)) {
                return BCM_E_PARAM;
            }
            /* REPLACE_VID_ONLY */
            soc_mem_field32_set(unit, egr_xlate_mem, evxlt_entry, 
                                        MIM_ISID__SD_TAG_VIDf, 
                                        mim_port->egress_service_vlan); 
            sd_tag_action = 0x2; /* Replace Vid only */
        } else if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_DELETE) {
            sd_tag_action = 0x3; /* DELETE */
        } 
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
          else if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_PRI_REPLACE) {
            soc_mem_field32_set(unit, egr_xlate_mem, evxlt_entry,
                                        MIM_ISID__NEW_PRIf,
                                        mim_port->egress_service_pri);
            soc_mem_field32_set(unit, egr_xlate_mem, evxlt_entry,
                                        MIM_ISID__NEW_CFIf,
                                        mim_port->egress_service_cfi);
            sd_tag_action = 0x6;
          } else if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_PRI_REPLACE) {
            soc_mem_field32_set(unit, egr_xlate_mem, evxlt_entry,
                                        MIM_ISID__NEW_PRIf,
                                        mim_port->egress_service_pri);
            soc_mem_field32_set(unit, egr_xlate_mem, evxlt_entry,
                                        MIM_ISID__NEW_CFIf,
                                        mim_port->egress_service_cfi);
            soc_mem_field32_set(unit, egr_xlate_mem, evxlt_entry,
                                        MIM_ISID__SD_TAG_REMARK_CFIf, 1);
            soc_mem_field32_set(unit, egr_xlate_mem, evxlt_entry, 
                                        MIM_ISID__SD_TAG_VIDf, 
                                        mim_port->egress_service_vlan); 
            sd_tag_action = 0x5;
        } else if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_TPID_REPLACE) {
            /* TPID value is used */
            rv = _bcm_fb2_outer_tpid_entry_add(unit, mim_port->egress_service_tpid, 
                                           &tpid_index);
            BCM_IF_ERROR_RETURN(rv);
            soc_mem_field32_set(unit, egr_xlate_mem, evxlt_entry,
                                        MIM_ISID__SD_TAG_TPID_INDEXf,
                                        tpid_index);
            sd_tag_action = 0x7;
        }
#endif
        soc_mem_field32_set(unit, egr_xlate_mem, evxlt_entry,
                                    MIM_ISID__SD_TAG_ACTION_IF_NOT_PRESENTf,
                                    sd_default_action);
        soc_mem_field32_set(unit, egr_xlate_mem, evxlt_entry, 
                                    MIM_ISID__SD_TAG_ACTION_IF_PRESENTf, 
                                    sd_tag_action);

        if ((mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_ADD) ||
            (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TPID_REPLACE)) {
        /* TPID value is used */
            rv = _bcm_fb2_outer_tpid_entry_add(unit, mim_port->egress_service_tpid, 
                                           &tpid_index);
            BCM_IF_ERROR_RETURN(rv);
            soc_mem_field32_set(unit, egr_xlate_mem, evxlt_entry, 
                                        MIM_ISID__SD_TAG_TPID_INDEXf, 
                                        tpid_index);
        }
    } else if (vpn_info != NULL) {
        if (vpn_info->flags & BCM_MIM_VPN_EGRESS_SERVICE_TPID_REPLACE) {
            rv = _bcm_fb2_outer_tpid_entry_add(unit, vpn_info->egress_service_tpid,
                                               &tpid_index);
            BCM_IF_ERROR_RETURN(rv);
            soc_mem_field32_set(unit, egr_xlate_mem, evxlt_entry,
                                            MIM_ISID__SD_TAG_TPID_INDEXf,
                                            tpid_index);

            /* REPLACE_TPID_ONLY */
            sd_tag_action = 0x7;
            soc_mem_field32_set(unit, egr_xlate_mem, evxlt_entry,
                                            MIM_ISID__SD_TAG_ACTION_IF_PRESENTf,
                                            sd_tag_action);
        }
    }

    return rv;
}

int
_bcm_tr2_mim_egr_vxlt_sd_tag_actions_delete(
    int unit, uint32 *egr_vlan_xlate_entry)
{
    int action_present, action_not_present, tpid_idx;
    soc_mem_t egr_xlate_mem;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        egr_xlate_mem = EGR_VLAN_XLATE_1_DOUBLEm;
    } else
#endif
    {
        egr_xlate_mem = EGR_VLAN_XLATEm;
    }    

    /* Populate the API structure */
    action_present =
        soc_mem_field32_get(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                                        MIM_ISID__SD_TAG_ACTION_IF_PRESENTf);
    action_not_present =
        soc_mem_field32_get(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                                      MIM_ISID__SD_TAG_ACTION_IF_NOT_PRESENTf);

    if ((action_not_present == 0x1) || (action_present == 0x1) ||
        ((action_present == 0x7) &&
          soc_feature(unit, soc_feature_evt_replace_tpid_only))) {
        /* If SD tag action is ADD or REPLACE_VID_TPID, the tpid
         * index of the entry is valid. Get the tpid index for later.
         */
        tpid_idx =
            soc_mem_field32_get(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                                            MIM_ISID__SD_TAG_TPID_INDEXf);
        /* Delete old TPID */
        if (tpid_idx != -1) {
            (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_idx);
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_tr2_mim_failover_check
 * Purpose:
 *    Check for failover enable - mim port
 * Parameters:
 *    unit     : (IN) Device Unit Number
 *    mim_port : (IN) mim port information
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_bcm_tr2_mim_failover_check(int unit, bcm_mim_port_t *mim_port)
{
    int failover_vp = -1;
    int l3mc_index  = -1;
    int vp_min      = -1;
    /* Get egress next-hop index from Failover MPLS gport */
    if (_BCM_MULTICAST_IS_SET(mim_port->failover_mc_group)) {
        l3mc_index = _BCM_MULTICAST_ID_GET(mim_port->failover_mc_group);
        if ((l3mc_index >= 0) &&
            (l3mc_index < soc_mem_index_count(unit, L3_IPMCm))) {
             return BCM_E_NONE;
        } else {
              return BCM_E_PARAM;
        }
    } else {
        failover_vp = BCM_GPORT_MIM_PORT_ID_GET(mim_port->failover_gport_id);
        if (failover_vp == -1) {
             return BCM_E_PARAM;
        }
        if (soc_feature(unit, soc_feature_mim_reserve_default_port) &&
            soc_property_get(unit, spn_RESERVE_MIM_DEFAULT_SVP, 0)) {
            vp_min = 2;
        } else {
            vp_min = 1;
        }
        if ((failover_vp >= vp_min) &&
            (failover_vp < soc_mem_index_count(unit, SOURCE_VPm))) {
             return BCM_E_NONE;
        } else {
             return BCM_E_PARAM;
        }
    }
    return BCM_E_PARAM;
}

STATIC int
_bcm_tr2_mim_l2_nh_info_add(int unit, bcm_mim_port_t *mim_port, int vp, int drop,
                           int *nh_index, bcm_port_t *local_port, int *is_local) 
{
    initial_ing_l3_next_hop_entry_t initial_ing_nh;
    ing_l3_next_hop_entry_t ing_nh;
    egr_l3_next_hop_entry_t egr_nh;
    egr_mac_da_profile_entry_t mac_da_ent;
    void *macda;
    soc_mem_t mem;
    _bcm_tr2_ing_nh_info_t ing_nh_info;
    _bcm_tr2_egr_nh_info_t egr_nh_info;
    bcm_l3_egress_t nh_info;
    _bcm_l3_intf_cfg_t if_info;
    uint32 nh_flags;
    int gport_id, rv;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int action_present, action_not_present;
    int old_tpid_idx = -1, old_macda_idx = -1;
    void *entries[1];
    uint64 temp_mac;
    _bcm_port_info_t *info;
    int failover_nh_index;
    uint32 failover_vp;
    ing_dvp_table_entry_t failover_dvp;
    uint32 protection_flags=0;
    uint32 port_flags;
    int network_group=0;
#ifdef BCM_TRIUMPH3_SUPPORT
    uint32 mtu_profile_index = 0;
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_KATANA2_SUPPORT)
    int my_modid = 0, is_local_subport = FALSE;
    int pp_port;
    int min_subport = SOC_INFO(unit).pp_port_index_min; 
    int max_subport = SOC_INFO(unit).pp_port_index_max;
#endif

    /* Initialize values */
    *local_port = 0;
    *is_local = 0;
    ing_nh_info.mtu = 0x3fff; 
    ing_nh_info.port = -1;
    ing_nh_info.module = -1;
    ing_nh_info.trunk = -1;

    egr_nh_info.dvp = vp;
    if (soc_feature(unit,
                soc_feature_multiple_split_horizon_group)) {
        network_group = mim_port->network_group_id;
        rv = _bcm_validate_splithorizon_network_group(unit,
                mim_port->flags & BCM_MIM_PORT_TYPE_BACKBONE,
                &network_group);
        BCM_IF_ERROR_RETURN(rv);
    }
    egr_nh_info.dvp_is_network =
        (mim_port->flags & BCM_MIM_PORT_TYPE_BACKBONE) ? 1 : 0;

    egr_nh_info.sd_tag_action_present = 0;
    egr_nh_info.sd_tag_action_not_present = 0;
    egr_nh_info.intf_num = -1;
    egr_nh_info.sd_tag_vlan = -1;
    egr_nh_info.sd_tag_pri = -1;
    egr_nh_info.sd_tag_cfi = -1;
    egr_nh_info.macda_index = -1;
    egr_nh_info.tpid_index = -1;
    egr_nh_info.vintf_ctr_idx = -1;

    if (mim_port->flags & BCM_MIM_PORT_REPLACE) {
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
    rv = _bcm_esw_gport_resolve(unit, mim_port->port, &mod_out, 
                                &port_out, &trunk_id, &gport_id);
    BCM_IF_ERROR_RETURN(rv);

#if defined(BCM_KATANA2_SUPPORT)
    if (BCM_GPORT_IS_SET(mim_port->port) &&
        _BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit,
            mim_port->port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
        if (_bcm_kt2_mod_is_coe_mod_check(unit, mod_out) == BCM_E_NONE) {
            rv = _bcm_kt2_modport_to_pp_port_get(
                     unit, mod_out, port_out, &pp_port);
            BCM_IF_ERROR_RETURN(rv);
#if defined BCM_METROLITE_SUPPORT
            if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                if (_SOC_IS_PP_PORT_LINKPHY_SUBTAG(unit, pp_port)) {
                    is_local_subport = TRUE;
                    *is_local = 1;
                    *local_port = pp_port;
                }
            } else
#endif
            {
                if ((pp_port >= min_subport) &&
                    (pp_port <= max_subport)) {
                    is_local_subport = TRUE;
                    *is_local = 1;
                    *local_port = pp_port;
                }
            }
        }
    }

#endif
    if (BCM_GPORT_IS_TRUNK(mim_port->port)) {
        ing_nh_info.module = -1;
        ing_nh_info.port = -1;
        ing_nh_info.trunk = trunk_id;
        MIM_INFO(unit)->port_info[vp].modid = -1;
        MIM_INFO(unit)->port_info[vp].port = -1;
        MIM_INFO(unit)->port_info[vp].tgid = trunk_id;
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

        MIM_INFO(unit)->port_info[vp].modid = mod_out;
        MIM_INFO(unit)->port_info[vp].port = port_out;
        MIM_INFO(unit)->port_info[vp].tgid = -1;
    }

    if ((mim_port->flags & BCM_MIM_PORT_TYPE_BACKBONE) || 
        (mim_port->flags & BCM_MIM_PORT_TYPE_PEER)) {
        /* Deal with backbone/peer ports (ingress and egress) */
        /* Use the MIM view of EGR_L3_NEXT_HOP */
        if (mim_port->flags & BCM_MIM_PORT_REPLACE) {

#if defined(BCM_TRIDENT3_SUPPORT)
            if (SOC_IS_TRIDENT3X(unit)) {
                egr_nh_info.entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                    &egr_nh, DATA_TYPEf);
            } else
#endif /* BCM_TRIDENT3_SUPPORT */
            {
                egr_nh_info.entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                    &egr_nh, ENTRY_TYPEf);
            }

            /* Be sure that the existing entry is programmed to MIM */
            if (egr_nh_info.entry_type != 0x3) { /* != MIM */
                return BCM_E_PARAM;
            }
            /* Remember old MAC DA profile index */
            old_macda_idx = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                 MIM__MAC_DA_PROFILE_INDEXf);
            /* Get Flexible Counter index */
            if (SOC_IS_TRIUMPH2(unit) || SOC_IS_VALKYRIE2(unit)
                || SOC_IS_TRIDENT(unit)) {
                egr_nh_info.vintf_ctr_idx =
                    soc_EGR_L3_NEXT_HOPm_field32_get
                            (unit, &egr_nh, VINTF_CTR_IDXf);
            }                
        }
        egr_nh_info.entry_type = 0x3;
 
        mem = EGR_MAC_DA_PROFILEm;
        macda = (void *)&mac_da_ent;
        sal_memset(macda, 0, sizeof(egr_mac_da_profile_entry_t));
        /* Add MAC DA profile */
        soc_mem_mac_addr_set(unit, mem, macda, 
                             MAC_ADDRESSf, mim_port->egress_tunnel_dstmac);

        entries[0] = macda;
        rv = _bcm_mac_da_profile_entry_add(unit, entries, 1,
                                           (uint32 *) &egr_nh_info.macda_index);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        /* Add MACSA to an L3 interface entry - ref count if exists */
        if (!BCM_VLAN_VALID(mim_port->egress_tunnel_vlan)) {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        sal_memset(&if_info, 0, sizeof(_bcm_l3_intf_cfg_t));
        SAL_MAC_ADDR_TO_UINT64(mim_port->egress_tunnel_srcmac, temp_mac);
        SAL_MAC_ADDR_FROM_UINT64(if_info.l3i_mac_addr, temp_mac);
        if_info.l3i_vid = mim_port->egress_tunnel_vlan;
        rv = _bcm_tr2_mim_l3_intf_add(unit, &if_info);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        /* Populate the fields of MIM::EGR_l3_NEXT_HOP */
        if (!(mim_port->flags & BCM_MIM_PORT_REPLACE)) {
            sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                DATA_TYPEf, egr_nh_info.entry_type);
        } else
#endif /* BCM_TRIDENT3_SUPPORT */
        {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                ENTRY_TYPEf, egr_nh_info.entry_type);
        }        
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            MIM__DVPf, egr_nh_info.dvp);
        if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
            /* 
             * Setting DVP_IS_ENNI field for PEER type ports and resetting for 
             * BACKBONE ports. Resetting is done to support scenarios where
             * PEER ports are replaced by BACKBONE ports  
             */
            if (!SOC_IS_TRIDENT3X(unit)) {
                if(mim_port->flags & BCM_MIM_PORT_TYPE_PEER) {
                   soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                   &egr_nh, MIM__DVP_IS_ENNIf, 1);
                } else {
                   soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                       &egr_nh, MIM__DVP_IS_ENNIf, 0);
                }
            }    
        } 
        
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            MIM__BVID_VALIDf, 1);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            MIM__INTF_NUMf, if_info.l3i_index);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            MIM__BVIDf, mim_port->egress_tunnel_vlan);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            MIM__HG_HDR_SELf, 1); /* HG 2 */
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            MIM__MAC_DA_PROFILE_INDEXf, egr_nh_info.macda_index);
        if (mim_port->flags & BCM_MIM_PORT_TYPE_PEER) {
            if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, MIM__ISID_LOOKUP_TYPEf)) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                MIM__ISID_LOOKUP_TYPEf, 1); /* Peer */
            }
#ifdef BCM_TRIDENT2PLUS_SUPPORT
            if (soc_feature(unit, soc_feature_mim_bvid_insertion_control)) {
                uint32 regval;
                BCM_IF_ERROR_RETURN(READ_EGR_CONFIGr(unit, &regval));
                if (soc_reg_field_get(unit, EGR_CONFIGr, regval,
                            HONOR_NEXTHOP_BVID_VALIDf)) {
                    if (mim_port->flags & BCM_MIM_PORT_PEER_NO_BTAG) {
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                            MIM__BVID_VALIDf, 0);
                    }
                }
            }
#endif
        }
        if (mim_port->flags & BCM_MIM_PORT_EGRESS_TUNNEL_DEST_MAC_USE_SERVICE) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                MIM__ADD_ISID_TO_MACDAf, 1); 
        }

/* should be only for entry_type 2
        if (mim_port->flags & BCM_MIM_PORT_FIX_PRIORITY) {
            BCM_IF_ERROR_RETURN
                (_bcm_tr2_mim_egr_sd_tag_actions(unit, mim_port, &egr_nh_info));
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
		                  SD_TAG__SD_TAG_DOT1P_PRI_SELECTf, 0);
            if (egr_nh_info.sd_tag_pri != -1) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                    &egr_nh, SD_TAG__NEW_PRIf, 
                                    egr_nh_info.sd_tag_pri);
            }
            if (egr_nh_info.sd_tag_cfi != -1) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                    &egr_nh, SD_TAG__NEW_CFIf, 
                                    egr_nh_info.sd_tag_cfi);
            }
        }
*/
    } else if (mim_port->flags & BCM_MIM_PORT_TYPE_ACCESS){
        /* Deal with access ports */
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_mim_decoupled_mode)) {
            rv = _bcm_td3_mim_l2_nh_otag_set(unit, &egr_nh_info,
                      &egr_nh, mim_port, *is_local, &old_tpid_idx);
            BCM_IF_ERROR_RETURN(rv);
        } else
#endif
        {
            /* Use the SD_TAG view of EGR_L3_NEXT_HOP */
            if (mim_port->flags & BCM_MIM_PORT_REPLACE) {
#if defined(BCM_TRIDENT3_SUPPORT)
                if (SOC_IS_TRIDENT3X(unit)) {
                    egr_nh_info.entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                        &egr_nh, DATA_TYPEf);
                } else
#endif /* BCM_TRIDENT3_SUPPORT */
                {
                    egr_nh_info.entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                        &egr_nh, ENTRY_TYPEf);
                }
                /* Be sure that the existing entry is programmed to SD-tag */
                if (egr_nh_info.entry_type != 0x2) { /* != SD-tag */
                    return BCM_E_PARAM;
                }
                action_present =
                    soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                     SD_TAG__SD_TAG_ACTION_IF_PRESENTf);
                action_not_present =
                    soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                     SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf);
                if ((action_not_present == 0x1) || (action_present == 0x1)) {
                    /* If SD tag action is ADD or REPLACE_VID_TPID, the tpid
                     * index of the entry getting replaced is valid. Save
                     * the old tpid index to be deleted later.
                     */
                    old_tpid_idx =
                        soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                         SD_TAG__SD_TAG_TPID_INDEXf);
                    /* Get Flexible Counter index */
                    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_VALKYRIE2(unit)
                       || SOC_IS_TRIDENT(unit)) {
        	  	        egr_nh_info.vintf_ctr_idx =
     	  	                soc_EGR_L3_NEXT_HOPm_field32_get
                                    (unit, &egr_nh, VINTF_CTR_IDXf);
     	  	    }
                }
            }
            egr_nh_info.entry_type = 0x2; /* SD-tag */

            /* Populate SD_TAG::EGR_L3_NEXT_HOP entry */
            if (!(mim_port->flags & BCM_MIM_PORT_REPLACE)) {
                sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
            }
#if defined(BCM_TRIDENT3_SUPPORT)
            if (SOC_IS_TRIDENT3X(unit)) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                    DATA_TYPEf, egr_nh_info.entry_type);
            } else
#endif /* BCM_TRIDENT3_SUPPORT */
            {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                    ENTRY_TYPEf, egr_nh_info.entry_type);
            }

            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                SD_TAG__DVPf, egr_nh_info.dvp);
            /* HG header PPD2 */
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                SD_TAG__HG_HDR_SELf, 1);

            if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TAGGED) {
                /* Prepare egress SD tag entry (including parameter checks) */
                BCM_IF_ERROR_RETURN
                    (_bcm_tr2_mim_egr_sd_tag_actions(unit, mim_port, &egr_nh_info));

                /* Populate SD-TAG attributes in HW */
                if (egr_nh_info.sd_tag_vlan != -1) {
                    if (soc_feature(unit, soc_feature_vp_sharing)) {
                        if (SOC_MEM_FIELD_VALID(unit, EGR_L3_NEXT_HOPm, SD_TAG__SD_TAG_NOT_VALIDf)) {
                            if (egr_nh_info.sd_tag_vlan == BCM_VLAN_INVALID) {
                                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                               &egr_nh, SD_TAG__SD_TAG_NOT_VALIDf, 1);
                            } else {
                                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                               &egr_nh, SD_TAG__SD_TAG_NOT_VALIDf, 0);
                                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                    &egr_nh, SD_TAG__SD_TAG_VIDf, egr_nh_info.sd_tag_vlan);
                            }
                        } else {
                            if (egr_nh_info.sd_tag_vlan < BCM_VLAN_INVALID) {
                                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                    &egr_nh, SD_TAG__SD_TAG_VIDf, egr_nh_info.sd_tag_vlan);
                            }
                        }
                    } else {
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                        &egr_nh, SD_TAG__SD_TAG_VIDf,
                                        egr_nh_info.sd_tag_vlan);
                   }
                }
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                SD_TAG__SD_TAG_ACTION_IF_PRESENTf,
                                egr_nh_info.sd_tag_action_present);
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                    SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf,
                                    egr_nh_info.sd_tag_action_not_present);
                if (egr_nh_info.tpid_index != -1) {
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                        &egr_nh, SD_TAG__SD_TAG_TPID_INDEXf,
                                        egr_nh_info.tpid_index);
                }


                if (egr_nh_info.sd_tag_pri != -1) {
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                        &egr_nh, SD_TAG__NEW_PRIf,
                                        egr_nh_info.sd_tag_pri);
                }

                if (egr_nh_info.sd_tag_cfi != -1) {
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                        &egr_nh, SD_TAG__NEW_CFIf,
                                        egr_nh_info.sd_tag_cfi);
                }

#if defined(BCM_KATANA2_SUPPORT)
                if (is_local_subport) {
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                SD_TAG__HG_MODIFY_ENABLEf, 1);
                } else
#endif

                {
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                SD_TAG__HG_MODIFY_ENABLEf, (*is_local) ? 1 : 0);
                }
            }
        }
    }

    /* Network Group ID can be set irrespective of port type */
    if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
        if (!SOC_IS_TRIDENT3X(unit)) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                &egr_nh, MIM__DVP_NETWORK_GROUPf,
                                network_group);
        }
    } else {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                &egr_nh, MIM__DVP_IS_NETWORK_PORTf,
                egr_nh_info.dvp_is_network);
    }
    
    /* Write INITIAL_ING_L3_NEXT_HOP entry */
    sal_memset(&initial_ing_nh, 0, sizeof(initial_ing_l3_next_hop_entry_t));
    if (soc_feature(unit, soc_feature_generic_dest)) {
        if (ing_nh_info.trunk == -1) {
            soc_mem_field32_dest_set(unit, INITIAL_ING_L3_NEXT_HOPm, &initial_ing_nh,
                DESTINATIONf, SOC_MEM_FIF_DEST_DGPP,
                ing_nh_info.module << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS | ing_nh_info.port);
        } else {
            soc_mem_field32_dest_set(unit, INITIAL_ING_L3_NEXT_HOPm, &initial_ing_nh,
                DESTINATIONf, SOC_MEM_FIF_DEST_LAG, ing_nh_info.trunk);
        }
    } else {
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
    }
    rv = soc_mem_write(unit, INITIAL_ING_L3_NEXT_HOPm,
                       MEM_BLOCK_ALL, *nh_index, &initial_ing_nh);
    if (rv < 0) {
        goto cleanup;
    }

    /* Configure drop bits */
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_mim_decoupled_mode)) {
        if ((mim_port->flags & BCM_MIM_PORT_TYPE_BACKBONE) ||
            (mim_port->flags & BCM_MIM_PORT_TYPE_PEER)) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, MIM__BC_DROPf,
                            drop ? 1 : 0);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, MIM__UUC_DROPf,
                            drop ? 1 : 0);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, MIM__UMC_DROPf,
                            drop ? 1 : 0);
        } else {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L2_OTAG__BC_DROPf, drop ? 1 : 0);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L2_OTAG__UUC_DROPf, drop ? 1 : 0);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L2_OTAG__UMC_DROPf, drop ? 1 : 0);
        }
    } else
#endif
    {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, SD_TAG__BC_DROPf,
                            drop ? 1 : 0);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, SD_TAG__UUC_DROPf,
                            drop ? 1 : 0);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, SD_TAG__UMC_DROPf,
                            drop ? 1 : 0);
    }

    /* Set Flexible Counter index */
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_VALKYRIE2(unit)
        || SOC_IS_TRIDENT(unit)) {
        if (egr_nh_info.vintf_ctr_idx != -1) {
            soc_EGR_L3_NEXT_HOPm_field32_set
                    (unit, &egr_nh, VINTF_CTR_IDXf, egr_nh_info.vintf_ctr_idx);
        }
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
    if (soc_feature(unit, soc_feature_generic_dest)) {
        if (ing_nh_info.trunk == -1) {
            soc_mem_field32_dest_set(unit, ING_L3_NEXT_HOPm, &ing_nh,
                DESTINATIONf, SOC_MEM_FIF_DEST_DGPP,
                ing_nh_info.module << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS | ing_nh_info.port);
        } else {
            soc_mem_field32_dest_set(unit, ING_L3_NEXT_HOPm, &ing_nh,
                DESTINATIONf, SOC_MEM_FIF_DEST_LAG, ing_nh_info.trunk);
        }
    } else {
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
    }
    if (drop) {
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, DROPf, drop);
    }
    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                       &ing_nh, ENTRY_TYPEf, 0x2); /* L2 DVP */
    if (SOC_MEM_FIELD_VALID(unit, ING_L3_NEXT_HOPm, MTU_SIZEf)) {
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                &ing_nh, MTU_SIZEf, 0x3fff);
    }
#ifdef BCM_TRIUMPH3_SUPPORT
    else if (SOC_MEM_FIELD_VALID(unit, ING_L3_NEXT_HOPm, 
                 DVP_ATTRIBUTE_1_INDEXf)) {
        BCM_IF_ERROR_RETURN(
          _bcm_tr3_mtu_profile_index_get(unit, 0x3fff, &mtu_profile_index));
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, 
                      DVP_ATTRIBUTE_1_INDEXf, mtu_profile_index);
    }
#endif /*BCM_TRIUMPH3_SUPPORT */
    rv = soc_mem_write (unit, ING_L3_NEXT_HOPm,
                       MEM_BLOCK_ALL, *nh_index, &ing_nh);
    if (rv < 0) {
        goto cleanup;
    }

    /* Update the physical port's SW state */
#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        if (is_local_subport) {
            _bcm_port_info_access(unit,
                MIM_INFO(unit)->port_info[vp].port, &info);
            info->vp_count++;
            rv = bcm_esw_port_vlan_member_get(
                     unit, MIM_INFO(unit)->port_info[vp].port, &port_flags);
            if (rv < 0) {
                goto cleanup;
            }
            rv = bcm_esw_port_vlan_member_set(
                    unit, MIM_INFO(unit)->port_info[vp].port, port_flags);
            if (rv < 0) {
                goto cleanup;
            }
        }
    }
#endif
    if (*is_local) {
        bcm_port_t phys_port = MIM_INFO(unit)->port_info[vp].port; 
        if (soc_feature(unit, soc_feature_sysport_remap)) { 
            BCM_XLATE_SYSPORT_S2P(unit, &phys_port); 
        }
        _bcm_port_info_access(unit, phys_port, &info);
        info->vp_count++;
        rv = bcm_esw_port_vlan_member_get(unit, phys_port, &port_flags);
        if (rv < 0) {
            goto cleanup;
        }
        rv = bcm_esw_port_vlan_member_set(unit, phys_port, port_flags);
        if (rv < 0) {
            goto cleanup;
        }
    }

    /* If associated with a trunk, update each local physical port's SW state */
    if (ing_nh_info.trunk != -1) {
#if defined(BCM_ESW_SUPPORT) && (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
        bcm_port_t      local_member_array[SOC_MAX_NUM_PP_PORTS];
        int             max_num_ports = SOC_MAX_NUM_PP_PORTS;
#else
        bcm_port_t      local_member_array[SOC_MAX_NUM_PORTS];
        int             max_num_ports = SOC_MAX_NUM_PORTS;
#endif
        int local_member_count;
        int idx;

        rv = _bcm_esw_trunk_local_members_get(unit, trunk_id,
                max_num_ports, local_member_array, &local_member_count);
        if (rv < 0) {
            goto cleanup;
        }
        for (idx = 0; idx < local_member_count; idx++) {
            _bcm_port_info_access(unit, local_member_array[idx], &info);
            info->vp_count++;
            rv = bcm_esw_port_vlan_member_get(
                     unit, local_member_array[idx], &port_flags);
            if (rv < 0) {
                goto cleanup;
            }
            rv = bcm_esw_port_vlan_member_set(
                     unit, local_member_array[idx], port_flags);
            if (rv < 0) {
                goto cleanup;
            }
        }
    }

#if defined(BCM_ENDURO_SUPPORT)
    if (!SOC_IS_ENDURO(unit)) {
#endif
    if (BCM_SUCCESS(_bcm_tr2_mim_failover_check(unit, mim_port))) {
         /* Get egress next-hop index from Failover MIM gport */
         failover_vp = BCM_GPORT_MIM_PORT_ID_GET(mim_port->failover_gport_id);
         rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, failover_vp, 
                                  &failover_dvp);
         if (rv < 0) {
             goto cleanup;
         }
         failover_nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &failover_dvp,
                                                            NEXT_HOP_INDEXf);
         if (_BCM_MULTICAST_IS_SET(mim_port->failover_mc_group)) {
              protection_flags |= _BCM_FAILOVER_1_PLUS_PROTECTION;
         }
         rv = _bcm_esw_failover_prot_nhi_create(unit, protection_flags, (uint32) *nh_index, 
                              failover_nh_index, mim_port->failover_mc_group, mim_port->failover_id);
         if (rv < 0) {
             goto cleanup;
         }
    }
#if defined(BCM_ENDURO_SUPPORT)
    }
#endif
    /* Delete old TPID, MAC indexes */
    if (old_macda_idx != -1) {
        rv = _bcm_mac_da_profile_entry_delete(unit, old_macda_idx);
        BCM_IF_ERROR_RETURN(rv);
    }
    if (old_tpid_idx != -1) {
        (void)_bcm_fb2_outer_tpid_entry_delete(unit, old_tpid_idx);
    }
    return rv;

cleanup:
    if (!(mim_port->flags & BCM_MIM_PORT_REPLACE)) {
        (void) bcm_xgs3_nh_del(unit, _BCM_L3_SHR_WRITE_DISABLE, *nh_index);
    }
    if (egr_nh_info.tpid_index != -1) {
        (void) _bcm_fb2_outer_tpid_entry_delete(unit, egr_nh_info.tpid_index);
    }
    if (egr_nh_info.macda_index != -1) {
        (void) _bcm_mac_da_profile_entry_delete(unit, egr_nh_info.macda_index);
    }
    return rv;
}

STATIC int
_bcm_tr2_mim_l2_nh_info_delete(int unit, int nh_index)
{
    int rv, old_macda_idx = -1;
    int action_present = 0, action_not_present = 0, old_tpid_idx = -1;
    initial_ing_l3_next_hop_entry_t initial_ing_nh;
    ing_l3_next_hop_entry_t ing_nh;
    egr_l3_next_hop_entry_t egr_nh;
    int entry_type = 0;

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, 
                                  MEM_BLOCK_ANY, nh_index, &egr_nh));
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, 
                                  MEM_BLOCK_ANY, nh_index, &ing_nh));
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, INITIAL_ING_L3_NEXT_HOPm, 
                                  MEM_BLOCK_ANY, nh_index, &initial_ing_nh));

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_mim_decoupled_mode)) {
        int profile_index;

        entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                            &egr_nh, DATA_TYPEf);
        if (entry_type == 21) {  /* access port */
            profile_index = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                               L2_OTAG__TAG_ACTION_PROFILE_PTRf);
            (void)_bcm_td3_sd_tag_action_get(unit, profile_index,
                               &action_present,
                               &action_not_present);
            if ((action_not_present == 0x1) || (action_present == 0x1) ||
                (action_present == 0x7)) {
                /* If SD tag action is ADD or REPLACE_VID_TPID, the tpid
                 * index of the entry is valid. Save
                 * the old tpid index to be deleted later.
                 */
                old_tpid_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                 L2_OTAG__TPID_INDEXf);
            }
        } else if (entry_type == 3) { /* network port */
            /* Network or peer port */
            old_macda_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                               MIM__MAC_DA_PROFILE_INDEXf);
        } else {
            return BCM_E_NOT_FOUND;
        }
    } else
#endif
    {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                &egr_nh, DATA_TYPEf);
        } else
#endif /* BCM_TRIDENT3_SUPPORT */
        {
            entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                &egr_nh, ENTRY_TYPEf);
        }

        if (entry_type == 0x2) {
            /* Access port */
            action_present =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                         SD_TAG__SD_TAG_ACTION_IF_PRESENTf);
            action_not_present =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                         SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf);
            if ((action_not_present == 0x1) || (action_present == 0x1)
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
                || (action_present == 0x7)
#endif
               ) {
                /* If SD tag action is ADD or REPLACE_VID_TPID, the tpid
                 * index of the entry is valid. Save
                 * the old tpid index to be deleted later.
                 */
                old_tpid_idx =
                    soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                 SD_TAG__SD_TAG_TPID_INDEXf);
            }
        } else if (entry_type == 0x3) {
            /* Network or peer port */
            old_macda_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                         MIM__MAC_DA_PROFILE_INDEXf);
        } else {
            return BCM_E_NOT_FOUND;
        }
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

#if defined(BCM_ENDURO_SUPPORT)
    if (!SOC_IS_ENDURO(unit)) {
#endif
        if (soc_feature(unit, soc_feature_failover))  {
            /* Delete the protection next-hop (if applicable) */
            rv = _bcm_esw_failover_prot_nhi_cleanup(unit, nh_index);
            if ((rv != BCM_E_NOT_FOUND) && (rv != BCM_E_NONE)) {
                return rv;
            }
        }
#if defined(BCM_ENDURO_SUPPORT)
    }
#endif
    /* Delete old TPID */
    if (old_tpid_idx != -1) {
        (void) _bcm_fb2_outer_tpid_entry_delete(unit, old_tpid_idx);
    }

    /* Delete old MAC profile reference */
    if (old_macda_idx != -1) {
        rv = _bcm_mac_da_profile_entry_delete(unit, old_macda_idx);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Free the next-hop entry. */
    rv = bcm_xgs3_nh_del(unit, _BCM_L3_SHR_WRITE_DISABLE, nh_index);
    return rv;
}

/*
 * Function:
 *      __bcm_tr2_mim_match_trunk_add
 * Purpose:
 *      Assign SVP of an MIM Trunk port
 * Parameters:
 *      unit    - (IN) Device Number
 *      tgid - (IN) Trunk group Id
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_tr2_mim_match_trunk_add(int unit, bcm_trunk_t tgid, int vp)
{
    bcm_module_t my_modid;
#if defined(BCM_ESW_SUPPORT) && (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
    bcm_port_t      local_member_array[SOC_MAX_NUM_PP_PORTS];
    int             max_num_ports = SOC_MAX_NUM_PP_PORTS;
#else
    bcm_port_t      local_member_array[SOC_MAX_NUM_PORTS];
    int             max_num_ports = SOC_MAX_NUM_PORTS;
#endif

    int local_member_count;
    int p;
    int rv = BCM_E_NONE;
    int rv_cleanup;
    int stm_index;


    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));

    BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, tgid,
                max_num_ports, local_member_array, &local_member_count));

    for (p = 0; p < local_member_count; p++) {
        rv = _bcm_esw_src_mod_port_table_index_get(unit, my_modid,
                local_member_array[p], &stm_index);
        if (BCM_FAILURE(rv)) {
            goto trunk_cleanup;
        }
        rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                stm_index, SOURCE_VPf, vp);
        if (BCM_FAILURE(rv)) {
            goto trunk_cleanup;
        }

        if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm, SVP_VALIDf)) {
            rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                        stm_index, SVP_VALIDf, 1);
            if (BCM_FAILURE(rv)) {
                goto trunk_cleanup;
            }
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        if(SOC_IS_TRIDENT3X(unit)) {
            rv = _bcm_esw_port_tab_set(
                     unit, local_member_array[p],
                    _BCM_CPU_TABS_NONE, PORT_OPERATIONf, 0x1); /* L2_SVP */
        } else
#endif
        {
            rv = soc_mem_field32_modify(unit, PORT_TABm, local_member_array[p],
                    PORT_OPERATIONf, 0x1); /* L2_SVP */
        }
        if (BCM_FAILURE(rv)) {
            goto trunk_cleanup;
        }
    }

    return BCM_E_NONE;

  trunk_cleanup:
    for (;p >= 0; p--) {
        rv_cleanup = _bcm_esw_src_mod_port_table_index_get(unit, my_modid,
                local_member_array[p], &stm_index);
        if (BCM_SUCCESS(rv_cleanup)) {
            (void) soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                    stm_index, SOURCE_VPf, 0);
#if defined(BCM_TRIDENT3_SUPPORT)
            if(SOC_IS_TRIDENT3X(unit)) {
                (void)_bcm_esw_port_tab_set(
                         unit, local_member_array[p],
                        _BCM_CPU_TABS_NONE, PORT_OPERATIONf, 0);
            } else
#endif
            {
                (void) soc_mem_field32_modify(unit, PORT_TABm, local_member_array[p],
                        PORT_OPERATIONf, 0);
            }
        }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_tr_mim_match_trunk_delete
 * Purpose:
 *      Remove SVP of an MIM Trunk port
 * Parameters:
 *      unit    - (IN) Device Number
 *      tgid - (IN) Trunk group Id
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_tr_mim_match_trunk_delete(int unit, bcm_trunk_t tgid, int vp)
{
    bcm_module_t my_modid;
#if defined(BCM_ESW_SUPPORT) && (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
    bcm_port_t      local_member_array[SOC_MAX_NUM_PP_PORTS];
    int             max_num_ports = SOC_MAX_NUM_PP_PORTS;
#else
    bcm_port_t      local_member_array[SOC_MAX_NUM_PORTS];
    int             max_num_ports = SOC_MAX_NUM_PORTS;
#endif

    int local_member_count;
    int p;
    int rv = BCM_E_NONE;
    int rv_cleanup;
    int stm_index;

    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));

    BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, tgid,
                max_num_ports, local_member_array, &local_member_count));

    for (p = 0; p < local_member_count; p++) {
        rv = _bcm_esw_src_mod_port_table_index_get(unit, my_modid,
                local_member_array[p], &stm_index);
        if (BCM_FAILURE(rv)) {
            goto trunk_cleanup;
        }
        rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                stm_index, SOURCE_VPf, 0);
        if (BCM_FAILURE(rv)) {
            goto trunk_cleanup;
        }

        if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm, SVP_VALIDf)) {
            rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                    stm_index, SVP_VALIDf, 0);
            if (BCM_FAILURE(rv)) {
                goto trunk_cleanup;
            }
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        if(SOC_IS_TRIDENT3X(unit)) {
            rv = _bcm_esw_port_tab_set(
                     unit, local_member_array[p],
                    _BCM_CPU_TABS_NONE, PORT_OPERATIONf, 0); /* L2_SVP */
        } else
#endif
        {
            rv = soc_mem_field32_modify(unit, PORT_TABm, local_member_array[p],
                    PORT_OPERATIONf, 0x0); /* L2_SVP */
        }
        if (BCM_FAILURE(rv)) {
            goto trunk_cleanup;
        }
    }

    return BCM_E_NONE;

  trunk_cleanup:
    for (;p >= 0; p--) {
        rv_cleanup = _bcm_esw_src_mod_port_table_index_get(unit, my_modid,
                local_member_array[p], &stm_index);
        if (BCM_SUCCESS(rv_cleanup)) {
            (void) soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                    stm_index, SOURCE_VPf, vp);
#if defined(BCM_TRIDENT3_SUPPORT)
            if(SOC_IS_TRIDENT3X(unit)) {
                (void)_bcm_esw_port_tab_set(
                         unit, local_member_array[p],
                        _BCM_CPU_TABS_NONE, PORT_OPERATIONf, 1); /* L2_SVP */
            } else
#endif
            {
                (void) soc_mem_field32_modify(unit, PORT_TABm, local_member_array[p],
                        PORT_OPERATIONf, 1);
            }
        }
    }

    return rv;
}

int
_bcm_tr2_mim_match_add(int unit, bcm_mim_port_t *mim_port,
                                   int vp, bcm_mim_vpn_t vpn)
{
    int rv = BCM_E_NONE, i, gport_id;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
#if defined(BCM_KATANA2_SUPPORT)
    int my_modid = 0, is_local_subport = FALSE, pp_port;
    int min_subport = SOC_INFO(unit).pp_port_index_min;
    int max_subport = SOC_INFO(unit).pp_port_index_max;
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    int    vfi = 0;
    _bcm_vp_info_t vp_info;
#endif
    soc_mem_t  xlate_mem;
    soc_mem_t port_mem;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, ING_DEVICE_PORTm)) {
        port_mem = ING_DEVICE_PORTm;
    } else
#endif
    {
        port_mem = PORT_TABm;
    }
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
       xlate_mem = VLAN_XLATE_1_DOUBLEm;
    } else
#endif
    {
       xlate_mem = VLAN_XLATEm;
    }

    if (mim_port->criteria == BCM_MIM_PORT_MATCH_SHARE) {
        /* There is no criteria to be configurated for shared vp,
               * only updating software state to indicate this is a shared vp */
        MIM_INFO(unit)->port_info[vp].flags |= _BCM_MIM_PORT_TYPE_ACCESS_SHARED;
        return BCM_E_NONE;
    }

    rv = _bcm_esw_gport_resolve(unit, mim_port->port, &mod_out,
                                &port_out, &trunk_id, &gport_id);
    BCM_IF_ERROR_RETURN(rv);

#if defined(BCM_KATANA2_SUPPORT)
    if (BCM_GPORT_IS_SET(mim_port->port) &&
        _BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit,
            mim_port->port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
        if (_bcm_kt2_mod_is_coe_mod_check(unit, mod_out) == BCM_E_NONE) {
            rv = _bcm_kt2_modport_to_pp_port_get(
                    unit, mod_out, port_out, &pp_port);
#if defined BCM_METROLITE_SUPPORT
            if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                if (_SOC_IS_PP_PORT_LINKPHY_SUBTAG(unit, pp_port)) {
                    is_local_subport = TRUE;
                }
            } else
#endif
            {
                if ((pp_port >= min_subport) &&
                    (pp_port <= max_subport)) {
                    is_local_subport = TRUE;
                }
            }
        }
    }
#endif


    if ((mim_port->criteria == BCM_MIM_PORT_MATCH_PORT_VLAN) ||
        (mim_port->criteria == BCM_MIM_PORT_MATCH_PORT_VLAN_STACKED)) {
        uint32 vent[SOC_MAX_MEM_WORDS];
        int xlate_index = -1;
        uint32 return_vent[SOC_MAX_MEM_WORDS];

        sal_memset(vent, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            soc_mem_field32_set(unit, xlate_mem,
                         vent, BASE_VALID_0f, 0x3); 
            soc_mem_field32_set(unit, xlate_mem,
                         vent, BASE_VALID_1f, 0x7);
        } else
#endif
        {
            soc_mem_field32_set(unit, xlate_mem, vent, VALIDf, 1);
        }
        soc_mem_field32_set(unit, xlate_mem, vent, MPLS_ACTIONf, 0x1); /* SVP */
        soc_mem_field32_set(unit, xlate_mem, vent, DISABLE_VLAN_CHECKSf, 1);
        soc_mem_field32_set(unit, xlate_mem, vent, SOURCE_VPf, vp);
#if defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit) ||
            SOC_IS_TRIDENT3X(unit)) {
            soc_mem_field32_set(unit, xlate_mem, vent, SOURCE_TYPEf, 1);
            soc_mem_field32_set(unit, xlate_mem, vent, VLAN_ACTION_VALIDf, 1);
        }
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
        if (vp_info.flags & _BCM_VP_INFO_SHARED_PORT) {
            _BCM_MIM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);
            soc_mem_field32_set(unit, xlate_mem, vent, XLATE__VFIf, vfi);
        }
#endif
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            soc_mem_field32_set(unit, xlate_mem, vent, SOURCE_TYPEf, 1);
        }
#endif /* BCM_KATANA2_SUPPORT */

        if (mim_port->criteria == BCM_MIM_PORT_MATCH_PORT_VLAN) {
            if (!BCM_VLAN_VALID(mim_port->match_vlan)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, xlate_mem, vent, KEY_TYPEf,
                                TR_VLXLT_HASH_KEY_TYPE_OVID);
            if (soc_mem_field_valid(unit, xlate_mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, xlate_mem, vent, DATA_TYPEf,
                                    TR_VLXLT_HASH_KEY_TYPE_OVID);
            }
            soc_mem_field32_set(unit, xlate_mem, vent, OVIDf,
                                        mim_port->match_vlan);
            if (!(MIM_INFO(unit)->port_info[vp].flags & 
                _BCM_MIM_PORT_TYPE_ACCESS_SHARED)) {
                MIM_INFO(unit)->port_info[vp].flags |= 
                                         _BCM_MIM_PORT_TYPE_ACCESS_PORT_VLAN;
                MIM_INFO(unit)->port_info[vp].match_vlan = mim_port->match_vlan;
            }
        } else {
            if (!BCM_VLAN_VALID(mim_port->match_vlan) || 
                !BCM_VLAN_VALID(mim_port->match_inner_vlan)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, xlate_mem, vent, KEY_TYPEf,
                                        TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
            soc_mem_field32_set(unit, xlate_mem, vent, OVIDf,
                                        mim_port->match_vlan);
            soc_mem_field32_set(unit, xlate_mem, vent, IVIDf,
                                        mim_port->match_inner_vlan);
            if (!(MIM_INFO(unit)->port_info[vp].flags & 
                _BCM_MIM_PORT_TYPE_ACCESS_SHARED)) {
                MIM_INFO(unit)->port_info[vp].flags |= 
                                 _BCM_MIM_PORT_TYPE_ACCESS_PORT_VLAN_STACKED;
                MIM_INFO(unit)->port_info[vp].match_vlan = mim_port->match_vlan;
                MIM_INFO(unit)->port_info[vp].match_inner_vlan = 
                                 mim_port->match_inner_vlan;
            }
        }

        if (BCM_GPORT_IS_TRUNK(mim_port->port)) {
            soc_mem_field32_set(unit, xlate_mem, vent, Tf, 1);
            soc_mem_field32_set(unit, xlate_mem, vent, TGIDf, trunk_id);
        } else {
            soc_mem_field32_set(unit, xlate_mem, vent, MODULE_IDf, mod_out);
            soc_mem_field32_set(unit, xlate_mem, vent, PORT_NUMf, port_out);
        }
        rv = soc_mem_search(unit, xlate_mem, MEM_BLOCK_ANY, &xlate_index,
                            vent, return_vent, 0);
        if (BCM_E_NONE == rv) {
            if (mim_port->flags & BCM_MIM_PORT_REPLACE) {
                BCM_IF_ERROR_RETURN(
                    soc_mem_write(unit,
                                  xlate_mem, MEM_BLOCK_ALL, xlate_index, vent));
            } else {
                return BCM_E_EXISTS;
            }
        } else if (BCM_E_NOT_FOUND == rv) {
            if (mim_port->flags & BCM_MIM_PORT_REPLACE) {
                return BCM_E_NOT_FOUND;
            }
            rv = soc_mem_insert(unit, xlate_mem, MEM_BLOCK_ALL, vent);
            BCM_IF_ERROR_RETURN(rv);
            _bcm_tr2_mim_port_match_count_adjust(unit, vp, 1);
        } else {
            return rv;
        }
    } else if (mim_port->criteria == BCM_MIM_PORT_MATCH_PORT) {
        if (mim_port->flags & BCM_MIM_PORT_REPLACE) {
            if (!((MIM_INFO(unit)->port_info[vp].flags & _BCM_MIM_PORT_TYPE_ACCESS_PORT_TRUNK) ||
                (MIM_INFO(unit)->port_info[vp].flags & _BCM_MIM_PORT_TYPE_ACCESS_PORT))) {
                BCM_IF_ERROR_RETURN(BCM_E_NOT_FOUND);
            }

            if (MIM_INFO(unit)->port_info[vp].flags & _BCM_MIM_PORT_TYPE_ACCESS_PORT_TRUNK) {
                int trunk_id = 0;;
                trunk_id = MIM_INFO(unit)->port_info[vp].index;
                rv = _bcm_tr_mim_match_trunk_delete(unit, trunk_id, vp);
                BCM_IF_ERROR_RETURN(rv);
            }

            if (MIM_INFO(unit)->port_info[vp].flags & _BCM_MIM_PORT_TYPE_ACCESS_PORT) {
                int port= 0;
                rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm, 
                                            MIM_INFO(unit)->port_info[vp].index, 
                                            SOURCE_VPf, 0);        
                BCM_IF_ERROR_RETURN(rv);

                if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm, SVP_VALIDf)) {
                    rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                                MIM_INFO(unit)->port_info[vp].index, 
                                                SVP_VALIDf, 0);
                    BCM_IF_ERROR_RETURN(rv);
                }

                port = MIM_INFO(unit)->port_info[vp].index & SOC_PORT_ADDR_MAX(unit);
#if defined(BCM_TRIDENT3_SUPPORT)
                if(SOC_IS_TRIDENT3X(unit)) {
                    rv = _bcm_esw_port_tab_set(
                         unit, port, _BCM_CPU_TABS_NONE,
                         PORT_OPERATIONf, 0x0); /* NORMAL */
                } else
#endif
                {
                    rv = soc_mem_field32_modify(unit, port_mem, port,
                                            PORT_OPERATIONf, 0x0); /* NORMAL */
                }
                BCM_IF_ERROR_RETURN(rv);
            }
        }

        if (BCM_GPORT_IS_TRUNK(mim_port->port)) {
            rv = _bcm_tr2_mim_match_trunk_add(unit, trunk_id, vp);
            if (rv >= 0) {
                MIM_INFO(unit)->port_info[vp].flags |= 
                    _BCM_MIM_PORT_TYPE_ACCESS_PORT_TRUNK;
                MIM_INFO(unit)->port_info[vp].index = trunk_id;
            }
            BCM_IF_ERROR_RETURN(rv);
        } else {
            int is_local;

            BCM_IF_ERROR_RETURN
                ( _bcm_esw_modid_is_local(unit, mod_out, &is_local));

            /* Get index to source trunk map table */
            BCM_IF_ERROR_RETURN(
                _bcm_esw_src_mod_port_table_index_get(unit, mod_out,
                     port_out, &i));

            rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                        i, SOURCE_VPf, vp);
            BCM_IF_ERROR_RETURN(rv);

            if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm, SVP_VALIDf)) {
                rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                            i, SVP_VALIDf, 1);
                BCM_IF_ERROR_RETURN(rv);
            }

            if (is_local) {
#if defined(BCM_TRIDENT3_SUPPORT)
                if(SOC_IS_TRIDENT3X(unit)) {
                    rv = _bcm_esw_port_tab_set(
                         unit, port_out, _BCM_CPU_TABS_NONE,
                         PORT_OPERATIONf, 0x1); /* L2_SVP2_SVP */
                } else
#endif
                {
                    rv = soc_mem_field32_modify(unit, port_mem, port_out,
                                        PORT_OPERATIONf, 0x1); /* L2_SVP */
                }
                BCM_IF_ERROR_RETURN(rv);
            }
#ifdef BCM_KATANA2_SUPPORT
            if (soc_feature(unit, soc_feature_linkphy_coe) ||
                soc_feature(unit, soc_feature_linkphy_coe)) {
                if (is_local_subport) {
                    rv = soc_mem_field32_modify(unit, port_mem, port_out,
                                            PORT_OPERATIONf, 0x1); /* L2_SVP */
                    BCM_IF_ERROR_RETURN(rv);
                }
            }
#endif
            MIM_INFO(unit)->port_info[vp].flags |= 
                             _BCM_MIM_PORT_TYPE_ACCESS_PORT;
            MIM_INFO(unit)->port_info[vp].index = i;
        }

    } else if ((mim_port->criteria == BCM_MIM_PORT_MATCH_LABEL)) {
        uint32 ment[SOC_MAX_MEM_WORDS];
        int index = -1;
        uint32 return_ment[SOC_MAX_MEM_WORDS];
        soc_mem_t mpls_mem = MPLS_ENTRYm;

        sal_memset(ment, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
        if (BCM_GPORT_IS_TRUNK(mim_port->port)) {
            soc_mem_field32_set(unit, mpls_mem, ment, Tf, 1);
            soc_mem_field32_set(unit, mpls_mem, ment, TGIDf, trunk_id);
        } else {
            soc_mem_field32_set(unit, mpls_mem, ment, MODULE_IDf, mod_out);
            soc_mem_field32_set(unit, mpls_mem, ment, PORT_NUMf, port_out);
        }
        soc_mem_field32_set(unit, mpls_mem, ment, MPLS_LABELf,
                                    mim_port->match_label);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            soc_mem_field32_set(unit, mpls_mem, ment, BASE_VALID_0f, 3); 
            soc_mem_field32_set(unit, mpls_mem, ment, BASE_VALID_1f, 7); 
        } else
#endif
        {
            soc_mem_field32_set(unit, mpls_mem, ment, VALIDf, 0x1);
        }
        soc_mem_field32_set(unit, mpls_mem, ment, SOURCE_VPf, vp);
        soc_mem_field32_set(unit, mpls_mem, ment, MPLS_ACTION_IF_BOSf,
                                    0x1); /* L2 SVP */
        soc_mem_field32_set(unit, mpls_mem, ment, MPLS_ACTION_IF_NOT_BOSf,
                                    0x0); /* INVALID */
        rv = soc_mem_search(unit, mpls_mem, MEM_BLOCK_ANY, &index,
                            ment, return_ment, 0);
        if (BCM_E_NONE == rv) {
            if (mim_port->flags & BCM_MIM_PORT_REPLACE) {
                BCM_IF_ERROR_RETURN(
                    soc_mem_write(unit,
                                  mpls_mem, MEM_BLOCK_ALL, index, ment));
            } else {
                return BCM_E_EXISTS;
            }
        } else if (BCM_E_NOT_FOUND == rv) {
            if (mim_port->flags & BCM_MIM_PORT_REPLACE) {
                return BCM_E_NOT_FOUND;
            }
            rv = soc_mem_insert(unit, mpls_mem, MEM_BLOCK_ALL, ment);
            BCM_IF_ERROR_RETURN(rv);
        } else {
            return rv;
        }
        MIM_INFO(unit)->port_info[vp].flags |= 
                         _BCM_MIM_PORT_TYPE_ACCESS_LABEL;
        MIM_INFO(unit)->port_info[vp].match_label = mim_port->match_label;

    } else if ((mim_port->criteria == BCM_MIM_PORT_MATCH_TUNNEL_VLAN_SRCMAC)) {
        uint32 ment[SOC_MAX_MEM_WORDS];
        uint64 temp_mac;
        int index = -1;
        uint32 return_ment[SOC_MAX_MEM_WORDS];
        soc_mem_t mpls_mem = MPLS_ENTRYm;

        if (!BCM_VLAN_VALID(mim_port->match_tunnel_vlan)) {
            return BCM_E_PARAM;
        }

        sal_memset(ment, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
        soc_mem_field32_set(unit, mpls_mem, ment, KEY_TYPEf, 1); /* MIM NVP */
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_mem_field_valid(unit, mpls_mem, DATA_TYPEf)) {
            soc_mem_field32_set(unit, mpls_mem, ment, DATA_TYPEf, 1);
        }
#endif
        if (BCM_GPORT_IS_TRUNK(mim_port->port)) {
            soc_mem_field32_set(unit, mpls_mem, ment, MIM_NVP__Tf, 1);
            soc_mem_field32_set(unit, mpls_mem, ment, MIM_NVP__TGIDf, trunk_id);
        } else {
            soc_mem_field32_set(unit, mpls_mem, ment, MIM_NVP__MODULE_IDf, mod_out);
            soc_mem_field32_set(unit, mpls_mem, ment, MIM_NVP__PORT_NUMf, port_out);
        }
        if (mim_port->flags & BCM_MIM_PORT_TYPE_PEER) {
            soc_mem_field32_set(unit, mpls_mem, ment, 
                                        MIM_NVP__ISID_LOOKUP_TYPEf, 1);
            MIM_INFO(unit)->port_info[vp].flags |= 
                             _BCM_MIM_PORT_TYPE_PEER;
#if defined(BCM_TRIDENT_SUPPORT)
            if (soc_feature(unit, soc_feature_mim_peer_sharing) &&
                (mim_port->flags & BCM_MIM_PORT_SHARE)) {
                    MIM_INFO(unit)->port_info[vp].flags |=
                        _BCM_MIM_PORT_TYPE_SHARE;

            }
#endif /* BCM_TRIDENT_SUPPORT */
        } else {
            MIM_INFO(unit)->port_info[vp].flags |= 
                             _BCM_MIM_PORT_TYPE_NETWORK;
        }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_ingress_failover)) {
            int local_failover_id = 0;
            int prot_type         = 0;
            if (mim_port->flags & BCM_MIM_PORT_FAILOVER_DROP) {
                soc_mem_field32_set(unit, mpls_mem, ment,
                                            MIM_NVP__DROP_DATA_ENABLEf, 1);
            } else {
                soc_mem_field32_set(unit, mpls_mem, ment,
                                            MIM_NVP__DROP_DATA_ENABLEf, 0);
            }

            if (mim_port->ingress_failover_id != 0) {
                local_failover_id = mim_port->ingress_failover_id;
                _BCM_GET_FAILOVER_TYPE(local_failover_id, prot_type);
                _BCM_GET_FAILOVER_ID(local_failover_id);

                if ((prot_type & _BCM_FAILOVER_INGRESS) &&
                    (BCM_SUCCESS(_bcm_td2p_failover_ingress_id_validate(
                    unit, local_failover_id)))) {
                    soc_mem_field32_set(unit, mpls_mem, ment,
                                                MIM_NVP__RX_PROT_GROUPf,
                                                local_failover_id);
                }
            } else {
                soc_mem_field32_set(unit, mpls_mem, ment,
                                            MIM_NVP__RX_PROT_GROUPf, 0);
            }
         }
#endif
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            soc_mem_field32_set(unit, mpls_mem, ment, BASE_VALID_0f, 3); 
            soc_mem_field32_set(unit, mpls_mem, ment, BASE_VALID_1f, 7); 
        } else
#endif
        {
            soc_mem_field32_set(unit, mpls_mem, ment, VALIDf, 0x1);
        }
        soc_mem_field32_set(unit, mpls_mem, ment, MIM_NVP__SVPf, vp);
        soc_mem_field32_set(unit, mpls_mem, ment, MIM_NVP__BVIDf,
                                    mim_port->match_tunnel_vlan);
        soc_mem_mac_addr_set(unit, mpls_mem, ment, MIM_NVP__BMACSAf, 
                             mim_port->match_tunnel_srcmac);
        rv = soc_mem_search(unit, mpls_mem, MEM_BLOCK_ANY, &index,
                            ment, return_ment, 0);
        if (BCM_E_NONE == rv) {
            if (mim_port->flags & BCM_MIM_PORT_REPLACE) {
                BCM_IF_ERROR_RETURN(
                    soc_mem_write(unit,
                                  mpls_mem, MEM_BLOCK_ALL, index, ment));
            } else {
                return BCM_E_EXISTS;
            }
        } else if (BCM_E_NOT_FOUND == rv) {
            if (mim_port->flags & BCM_MIM_PORT_REPLACE) {
                return BCM_E_NOT_FOUND;
            }
            rv = soc_mem_insert(unit, mpls_mem, MEM_BLOCK_ALL, ment);
            BCM_IF_ERROR_RETURN(rv);
        } else {
            return rv;
        }
        SAL_MAC_ADDR_TO_UINT64(mim_port->match_tunnel_srcmac, temp_mac);
        SAL_MAC_ADDR_FROM_UINT64
           (MIM_INFO(unit)->port_info[vp].match_tunnel_srcmac, temp_mac); 
        MIM_INFO(unit)->port_info[vp].match_tunnel_vlan = 
                         mim_port->match_tunnel_vlan;
    }
    return rv;
}

int
_bcm_tr2_mim_match_delete(int unit, int vp, bcm_mim_port_t mim_port)
{
    int port, rv;
    int is_local;
    bcm_module_t mod_out;
    soc_mem_t  xlate_mem;
    soc_mem_t port_mem;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, ING_DEVICE_PORTm)) {
        port_mem = ING_DEVICE_PORTm;
    } else
#endif
    {
        port_mem = PORT_TABm;
    }
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
       xlate_mem = VLAN_XLATE_1_DOUBLEm;
    } else
#endif
    {
       xlate_mem = VLAN_XLATEm;
    }

    if (MIM_INFO(unit)->port_info[vp].flags & 
        _BCM_MIM_PORT_TYPE_ACCESS_PORT_VLAN) {
        uint32 vent[SOC_MAX_MEM_WORDS];
        sal_memset(vent, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            soc_mem_field32_set(unit, xlate_mem,
                         vent, BASE_VALID_0f, 0x3); 
            soc_mem_field32_set(unit, xlate_mem,
                         vent, BASE_VALID_1f, 0x7);
        } else
#endif
        {
            soc_mem_field32_set(unit, xlate_mem, vent, VALIDf, 1);
        }

        soc_mem_field32_set(unit, xlate_mem, vent, KEY_TYPEf, 
                                    TR_VLXLT_HASH_KEY_TYPE_OVID);
        if (soc_mem_field_valid(unit, xlate_mem, DATA_TYPEf)) {
            soc_mem_field32_set(unit, xlate_mem, vent, DATA_TYPEf,
                                TR_VLXLT_HASH_KEY_TYPE_OVID);
        }
        soc_mem_field32_set(unit, xlate_mem, vent, OVIDf,
                                    MIM_INFO(unit)->port_info[vp].match_vlan);
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_KATANA2_SUPPORT)

        if (SOC_IS_TRIDENT2X(unit) || SOC_IS_KATANA2(unit) || 
            SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
            soc_mem_field32_set(unit, xlate_mem, vent, SOURCE_TYPEf, 1);
        }
#endif /* BCM_TRIDENT2_SUPPORT  || BCM_KATANA2_SUPPORT */
        if (MIM_INFO(unit)->port_info[vp].modid != -1) {
            soc_mem_field32_set(unit, xlate_mem, vent, MODULE_IDf, 
                                        MIM_INFO(unit)->port_info[vp].modid);
            soc_mem_field32_set(unit, xlate_mem, vent, PORT_NUMf, 
                                        MIM_INFO(unit)->port_info[vp].port);
        } else {
            soc_mem_field32_set(unit, xlate_mem, vent, Tf, 1);
            soc_mem_field32_set(unit, xlate_mem, vent, TGIDf, 
                                        MIM_INFO(unit)->port_info[vp].tgid);
        }
        rv = soc_mem_delete(unit, xlate_mem, MEM_BLOCK_ANY, vent);
        if (rv != BCM_E_NOT_FOUND) {
            _bcm_tr2_mim_port_match_count_adjust(unit, vp, -1);
            return rv; /* Match has been deleted by bcm_port_match_delete */
        }

    } else if (MIM_INFO(unit)->port_info[vp].flags & 
               _BCM_MIM_PORT_TYPE_ACCESS_PORT_VLAN_STACKED) {
        uint32 vent[SOC_MAX_MEM_WORDS];
        sal_memset(vent, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

        soc_mem_field32_set(unit, xlate_mem, vent, KEY_TYPEf, 
                            TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
        if (soc_mem_field_valid(unit, xlate_mem, DATA_TYPEf)) {
            soc_mem_field32_set(unit, xlate_mem, vent, DATA_TYPEf,
                                TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
        }
        soc_mem_field32_set(unit, xlate_mem, vent, OVIDf,
                                    MIM_INFO(unit)->port_info[vp].match_vlan);
        soc_mem_field32_set(unit, xlate_mem, vent, IVIDf,
                              MIM_INFO(unit)->port_info[vp].match_inner_vlan);
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
        soc_mem_field32_set(unit, xlate_mem, vent, SOURCE_TYPEf, 1);
        if (SOC_IS_TRIDENT2X(unit) || SOC_IS_KATANA2(unit) || 
            SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
            soc_mem_field32_set(unit, xlate_mem, vent, SOURCE_TYPEf, 1);
        }
#endif /* BCM_TRIDENT2_SUPPORT  || BCM_KATANA2_SUPPORT */

        if (MIM_INFO(unit)->port_info[vp].modid != -1) {
            soc_mem_field32_set(unit, xlate_mem, vent, MODULE_IDf, 
                                        MIM_INFO(unit)->port_info[vp].modid);
            soc_mem_field32_set(unit, xlate_mem, vent, PORT_NUMf, 
                                        MIM_INFO(unit)->port_info[vp].port);
        } else {
            soc_mem_field32_set(unit, xlate_mem, vent, Tf, 1);
            soc_mem_field32_set(unit, xlate_mem, vent, TGIDf, 
                                        MIM_INFO(unit)->port_info[vp].tgid);
        }
        rv = soc_mem_delete(unit, xlate_mem, MEM_BLOCK_ANY, vent);
        BCM_IF_ERROR_RETURN(rv);   

    } else if (MIM_INFO(unit)->port_info[vp].flags & 
               _BCM_MIM_PORT_TYPE_ACCESS_PORT) {
        rv = _bcm_esw_src_modid_port_get(unit,
                                         MIM_INFO(unit)->port_info[vp].index,
                                         &mod_out, &port);
        BCM_IF_ERROR_RETURN(rv);
        rv = _bcm_esw_modid_is_local(unit, mod_out, &is_local);
        BCM_IF_ERROR_RETURN(rv);

        rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm, 
                                    MIM_INFO(unit)->port_info[vp].index, 
                                    SOURCE_VPf, 0);        
        BCM_IF_ERROR_RETURN(rv);

        if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm, SVP_VALIDf)) {
            rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                        MIM_INFO(unit)->port_info[vp].index, 
                                        SVP_VALIDf, 0);

            BCM_IF_ERROR_RETURN(rv);
        }

        if (is_local) {
#if defined(BCM_TRIDENT3_SUPPORT)
            if(SOC_IS_TRIDENT3X(unit)) {
                rv = _bcm_esw_port_tab_set(
                     unit, port, _BCM_CPU_TABS_NONE,
                         PORT_OPERATIONf, 0x0); /* NORMAL */
            } else
#endif
            {
                rv = soc_mem_field32_modify(unit, port_mem, port,
                                        PORT_OPERATIONf, 0x0); /* NORMAL */
                BCM_IF_ERROR_RETURN(rv);
           }
        }

    } else if(MIM_INFO(unit)->port_info[vp].flags & 
              _BCM_MIM_PORT_TYPE_ACCESS_PORT_TRUNK) {
        int trunk_id;
        trunk_id = MIM_INFO(unit)->port_info[vp].index;
        rv = _bcm_tr_mim_match_trunk_delete(unit, trunk_id, vp);
        BCM_IF_ERROR_RETURN(rv);
    } else if (MIM_INFO(unit)->port_info[vp].flags & 
               _BCM_MIM_PORT_TYPE_ACCESS_LABEL) {
        uint32 ment[SOC_MAX_MEM_WORDS];
        soc_mem_t mpls_mem = MPLS_ENTRYm;

        sal_memset(ment, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
        if (MIM_INFO(unit)->port_info[vp].modid != -1) {
            soc_mem_field32_set(unit, mpls_mem, ment, MODULE_IDf, 
                                        MIM_INFO(unit)->port_info[vp].modid);
            soc_mem_field32_set(unit, mpls_mem, ment, PORT_NUMf, 
                                        MIM_INFO(unit)->port_info[vp].port);
        } else {
            soc_mem_field32_set(unit, mpls_mem, ment, Tf, 1);
            soc_mem_field32_set(unit, mpls_mem, ment, TGIDf, 
                                        MIM_INFO(unit)->port_info[vp].tgid);
        }
        soc_mem_field32_set(unit, mpls_mem, ment, MPLS_LABELf,
                                    MIM_INFO(unit)->port_info[vp].match_label);
        rv = soc_mem_delete(unit, mpls_mem, MEM_BLOCK_ANY, ment);
        BCM_IF_ERROR_RETURN(rv);
    } else if (MIM_INFO(unit)->port_info[vp].flags & 
                   _BCM_MIM_PORT_TYPE_ACCESS_SHARED) {
        uint32 vent[SOC_MAX_MEM_WORDS];
        bcm_module_t mod_out = -1;
        bcm_port_t port_out = -1;
        int    gport_id =-1;
        bcm_trunk_t trunk_id;

        sal_memset(vent, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

        soc_mem_field32_set(unit, xlate_mem, vent, SOURCE_TYPEf, 1);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            soc_mem_field32_set(unit, xlate_mem,
                         vent, BASE_VALID_0f, 0x3); 
            soc_mem_field32_set(unit, xlate_mem,
                         vent, BASE_VALID_1f, 0x7);
        } else
#endif
        {
            soc_mem_field32_set(unit, xlate_mem, vent, VALIDf, 1);
        }
        if (BCM_MIM_PORT_MATCH_PORT_VLAN == mim_port.criteria) {
            soc_mem_field32_set(unit, xlate_mem, vent, KEY_TYPEf, 
                                        TR_VLXLT_HASH_KEY_TYPE_OVID);
            soc_mem_field32_set(unit, xlate_mem, vent, XLATE__OVIDf,
                                        mim_port.match_vlan);
        } else if (BCM_MIM_PORT_MATCH_PORT_VLAN_STACKED == mim_port.criteria) {
            soc_mem_field32_set(unit, xlate_mem, vent, KEY_TYPEf, 
                                        TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
            soc_mem_field32_set(unit, xlate_mem, vent, XLATE__OVIDf,
                                        mim_port.match_vlan);
            soc_mem_field32_set(unit, xlate_mem, vent, XLATE__IVIDf,
                                        mim_port.match_inner_vlan);
        } else {
            return BCM_E_UNAVAIL;
        }

        rv = _bcm_esw_gport_resolve(unit, mim_port.port, &mod_out,
                                    &port_out, &trunk_id, &gport_id);
        BCM_IF_ERROR_RETURN(rv);
        
        if (BCM_GPORT_IS_TRUNK(mim_port.port)) {
            soc_mem_field32_set(unit, xlate_mem, vent, Tf, 1);
            soc_mem_field32_set(unit, xlate_mem, vent, TGIDf, trunk_id);
        } else {
            soc_mem_field32_set(unit, xlate_mem, vent, MODULE_IDf, mod_out);
            soc_mem_field32_set(unit, xlate_mem, vent, PORT_NUMf, port_out);
        }

        rv = soc_mem_delete(unit, xlate_mem, MEM_BLOCK_ANY, vent);
        BCM_IF_ERROR_RETURN(rv);
        _bcm_tr2_mim_port_match_count_adjust(unit, vp, -1);
    } else if (MIM_INFO(unit)->port_info[vp].flags & 
               (_BCM_MIM_PORT_TYPE_NETWORK | _BCM_MIM_PORT_TYPE_PEER)) {
        uint32 ment[SOC_MAX_MEM_WORDS];
        soc_mem_t mpls_mem = MPLS_ENTRYm;

        sal_memset(ment, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
        soc_mem_field32_set(unit, mpls_mem, ment, KEY_TYPEf, 1); /* MIM NVP */
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            soc_mem_field32_set(unit, mpls_mem, ment, BASE_VALID_0f, 3); 
            soc_mem_field32_set(unit, mpls_mem, ment, BASE_VALID_1f, 7); 
        } else
#endif
        {
            soc_mem_field32_set(unit, mpls_mem, ment, VALIDf, 0x1);
        }
        soc_mem_field32_set(unit, mpls_mem, ment, MIM_NVP__BVIDf,
                               MIM_INFO(unit)->port_info[vp].match_tunnel_vlan);
        soc_mem_mac_addr_set(unit, mpls_mem, ment, MIM_NVP__BMACSAf, 
                             MIM_INFO(unit)->port_info[vp].match_tunnel_srcmac);
        rv = soc_mem_delete(unit, mpls_mem, MEM_BLOCK_ALL, ment);
        BCM_IF_ERROR_RETURN(rv);
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_tr2_mim_peer_port_config_add(int unit, bcm_mim_port_t *mim_port, 
                                  int vp, bcm_mim_vpn_t vpn,
                                  bcm_mim_vpn_config_t *vpn_info) {
    uint32 ment[SOC_MAX_MEM_WORDS];
    uint32 egr_vlan_xlate_entry[SOC_MAX_MEM_WORDS];
    int index, vfi, rv = BCM_E_NONE;
    soc_mem_t egr_xlate_mem;
    soc_mem_t mpls_mem = MPLS_ENTRYm;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        egr_xlate_mem = EGR_VLAN_XLATE_1_DOUBLEm;
    } else
#endif
    {
        egr_xlate_mem = EGR_VLAN_XLATEm;
    }

    /* Need an ISID-SVP entry in MPLS_ENTRY */
    _BCM_MIM_VPN_GET(vfi, _BCM_MIM_VPN_TYPE_MIM, vpn);
    sal_memset(ment, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

    soc_mem_field32_set(unit, mpls_mem, ment, KEY_TYPEf, 3); /* MIM_ISID_SVP */
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        soc_mem_field32_set(unit, mpls_mem, ment, BASE_VALID_0f, 3); 
        soc_mem_field32_set(unit, mpls_mem, ment, BASE_VALID_1f, 7); 
        soc_mem_field32_set(unit, mpls_mem, ment, DATA_TYPEf, 3);
    } else
#endif
    {
        soc_mem_field32_set(unit, mpls_mem, ment, VALIDf, 0x1);
    }
    if (mim_port->flags & BCM_MIM_PORT_EGRESS_TUNNEL_SERVICE_REPLACE) {
        soc_mem_field32_set(unit, mpls_mem, ment, MIM_ISID__ISIDf,
            (mim_port->egress_tunnel_service & 0xffffff));
    } else {
        soc_mem_field32_set(unit, mpls_mem, ment, MIM_ISID__ISIDf,
                                    VPN_ISID(unit, vfi));
    }
    soc_mem_field32_set(unit, mpls_mem, ment, MIM_ISID__SVPf, vp);
    soc_mem_field32_set(unit, mpls_mem, ment, MIM_ISID__VFIf, vfi);
    rv = soc_mem_search(unit, mpls_mem, MEM_BLOCK_ANY, &index,
                        ment, ment, 0);
    if (rv == SOC_E_NONE) {
        if (!(mim_port->flags & BCM_MIM_PORT_REPLACE)) {
            return BCM_E_EXISTS;
        }
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;
    }
    rv = soc_mem_insert(unit, mpls_mem, MEM_BLOCK_ALL, ment);
    if (rv < 0 && rv != BCM_E_EXISTS) {
        return rv;
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_mim_decoupled_mode)) {
        rv = _bcm_td3_mim_flex_port_egr_vxlt_op(unit, mim_port, vfi, vp,
                 _MIM_EVXLT_OP_ADD, NULL);
        BCM_IF_ERROR_RETURN(rv);
        rv = _bcm_td3_mim_untagged_payload_vlan_assign(unit, NULL, NULL,
                  mim_port, vp, vfi, vpn_info->default_vlan);
        BCM_IF_ERROR_RETURN(rv);
    } else
#endif
    {
        /* Also need the reverse entry in the egress */
        sal_memset(egr_vlan_xlate_entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
        soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry, MIM_ISID__VFIf,
                                        vfi);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            soc_mem_field32_set(unit, egr_xlate_mem,
                   egr_vlan_xlate_entry, BASE_VALID_0f, 0x3);
            soc_mem_field32_set(unit, egr_xlate_mem,
                   egr_vlan_xlate_entry, BASE_VALID_1f, 0x7);
            soc_mem_field32_set(unit, egr_xlate_mem,
                egr_vlan_xlate_entry, DATA_TYPEf, 0x4);
            soc_mem_field32_set(unit, egr_xlate_mem,
                egr_vlan_xlate_entry, KEY_TYPEf, 0x4);
        } else
#endif
        {
            soc_mem_field32_set(unit, egr_xlate_mem,
                egr_vlan_xlate_entry, VALIDf, 0x1);
            soc_mem_field32_set(unit, egr_xlate_mem,
                egr_vlan_xlate_entry, ENTRY_TYPEf, 0x4);
        }

        soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                                        MIM_ISID__DVPf, vp);

        rv = soc_mem_search(unit, egr_xlate_mem, MEM_BLOCK_ANY, &index,
                            egr_vlan_xlate_entry, egr_vlan_xlate_entry, 0);
        if (rv == SOC_E_NONE) {
            if (!(mim_port->flags & BCM_MIM_PORT_REPLACE)) {
                return BCM_E_EXISTS;
            }
        } else if (rv != SOC_E_NOT_FOUND) {
            return rv;
        }
        if (mim_port->flags & BCM_MIM_PORT_EGRESS_TUNNEL_SERVICE_REPLACE) {
            soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                MIM_ISID__ISIDf,
                (mim_port->egress_tunnel_service & 0xffffff));
        } else {
            soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                                MIM_ISID__ISIDf, VPN_ISID(unit, vfi));
        }

        /* Deal with egress SD tag actions */
        if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TAGGED) {
            BCM_IF_ERROR_RETURN
                (_bcm_tr2_mim_egr_vxlt_sd_tag_actions(unit, mim_port, NULL,
                                                      egr_vlan_xlate_entry));
        }

        rv = soc_mem_insert(unit, egr_xlate_mem, MEM_BLOCK_ALL, egr_vlan_xlate_entry);
        if (rv < 0 && rv != BCM_E_EXISTS) {
            return rv;
        }
    }
#if defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit, soc_feature_mim_peer_sharing) &&
        !(mim_port->flags & BCM_MIM_PORT_REPLACE)) {
        MIM_INFO(unit)->port_info[vp].vfi_count++;
    }
#endif /* BCM_TRIDENT_SUPPORT */

    return BCM_E_NONE;
}

 /* Function:
 *      _bcm_tr2_mim_vpn_is_eline
 * Purpose:
 *      Find if given MIM VPN is ELINE
 * Parameters:
 *      unit     - Device Number
 *      vpn      - MIM VPN
 *      is_eline - Flag 
 * Returns:
 *      BCM_E_XXXX
 * Assumes:
 *      l2vpn is valid
 */

STATIC int
_bcm_tr2_mim_vpn_is_eline( int unit, bcm_mim_vpn_t vpn, int *is_eline)
{
    vfi_entry_t vfi_entry;
    bcm_vpn_t mim_vpn_min=0;
    int vfi_index = -1;
    int num_vfi = 0;


     num_vfi = soc_mem_index_count(unit, VFIm);

    _BCM_MIM_VPN_SET(mim_vpn_min, _BCM_MIM_VPN_TYPE_MIM, 0);

    if (vpn < mim_vpn_min || vpn > (mim_vpn_min + num_vfi - 1) ) {
        return BCM_E_PARAM;
    }

    _BCM_MIM_VPN_GET(vfi_index, _BCM_MIM_VPN_TYPE_MIM, vpn);

    if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeMim)) {
        return BCM_E_NOT_FOUND;
    }

    /* Read the VFI table to check if point-to-point vpn */
    BCM_IF_ERROR_RETURN
        (READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    if (soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf)) {
        *is_eline = 1;  /* ELINE */
    }
    
    return BCM_E_NONE;
}

 /* Function:
 *      _bcm_tr2_mim_eline_port_add
 * Purpose:
 *      Add a port to VFI table for Point-to-Point(ELINE) service 
 * Parameters:
 *      unit     - Device Number
 *      vpn      - MIM VPN
 *      mim_port - MIM port
 * Returns:
 *      BCM_E_XXXX
 */

STATIC int
_bcm_tr2_mim_eline_port_add(int unit, 
                            int current_vp,
                            bcm_mim_vpn_t vpn, 
                            bcm_mim_port_t *mim_port)
{

    vfi_entry_t vfi_entry;
    source_vp_entry_t svp0, svp1;
    bcm_vpn_t mim_vpn_min=0;
    int vfi_index = -1;
    int num_vfi = 0;
    int vp0 = -1; /* virtual port 1 */
    int vp1 = -1; /* virtual port 2 */
    uint32 vp0_valid = 0, vp1_valid = 0;
    soc_field_t field = INVALIDf;
    int rv = BCM_E_NONE;


    /*
     * 1. check if vpn is valid
     * 2. get vfi index
     * 3. Read VFI entry from the VFI table.
     * 4. if PT2PTf is set
     * 5. get VP0 and VP1f fields 
     * 6. If both are set and not REPLACE flag
     *    return error - BCM_E_FULL
     * 7. Add it to vp0 or vp1 
     */
            
     num_vfi = soc_mem_index_count(unit, VFIm);

    _BCM_MIM_VPN_SET(mim_vpn_min, _BCM_MIM_VPN_TYPE_MIM, 0);

    if (vpn < mim_vpn_min || vpn > (mim_vpn_min + num_vfi - 1) ) {
        return BCM_E_PARAM;
    }

    _BCM_MIM_VPN_GET(vfi_index, _BCM_MIM_VPN_TYPE_MIM, vpn);

    if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeMim)) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN
        (READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    if (soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf)) {
        vp0 = soc_VFIm_field32_get(unit, &vfi_entry, VP_0f);
        vp1 = soc_VFIm_field32_get(unit, &vfi_entry, VP_1f);
    } else {
        return BCM_E_PARAM;
    }

    if (_bcm_vp_used_get(unit, vp0, _bcmVpTypeMim)) {
        BCM_IF_ERROR_RETURN (
           READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp0, &svp0));
        if (soc_SOURCE_VPm_field32_get(unit, &svp0, ENTRY_TYPEf) == 0x1) {
            vp0_valid = 1;
        }
    }

    if (_bcm_vp_used_get(unit, vp1, _bcmVpTypeMim)) {
        BCM_IF_ERROR_RETURN (
           READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp1, &svp1));
        if (soc_SOURCE_VPm_field32_get(unit, &svp1, ENTRY_TYPEf) == 0x1) {
            vp1_valid = 1;
        }
    }

    /*
     * if both vp0 and vp1 are valid
     * if port flags PORT_REPLACE is not set.
     * return BCM_E_FULL only 2 ports are required
     * for a point-to-point(ELINE) service    
     */
    if (vp0_valid && vp1_valid) {
        if ((mim_port->flags & BCM_MIM_PORT_REPLACE)) {
            if ((current_vp == vp0) || (current_vp == vp1)) {
                 return BCM_E_NONE;
            } else {
                return BCM_E_NOT_FOUND;
            }
        } else {
            return BCM_E_FULL;
        } 
    } else if (vp0_valid && !vp1_valid) {
        if (mim_port->flags & BCM_MIM_PORT_REPLACE) {
            if (current_vp != vp0) {
                return BCM_E_NOT_FOUND;
            }
        }
        field = VP_1f;
    } else if (!vp0_valid && vp1_valid) {
        if (mim_port->flags & BCM_MIM_PORT_REPLACE) {
            if (current_vp != vp1) {
                return BCM_E_NOT_FOUND;
            }
        }
        field = VP_0f;
    } else if (!vp0_valid && !vp1_valid) {
        if (mim_port->flags & BCM_MIM_PORT_REPLACE) {
            return BCM_E_NOT_FOUND;
        }
        field = VP_0f;
    }        

    if (field != INVALIDf) {
        soc_VFIm_field32_set(unit, &vfi_entry, field, current_vp);
        rv =  WRITE_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry);
    }

    return rv;

}

 /* Function:
 *      _bcm_tr2_mim_eline_port_delete
 * Purpose:
 *      Delete a port from VFI table for Point-to-Point(ELINE) service 
 * Parameters:
 *      unit     - Device Number
 *      vpn      - MIM VPN
 *      vp       - MIM virtual port
 * Returns:
 *      BCM_E_XXXX
 */

STATIC int
_bcm_tr2_mim_eline_port_delete(int unit, 
                               bcm_mim_vpn_t vpn, 
                               int vp)
{

    vfi_entry_t vfi_entry;
    bcm_vpn_t mim_vpn_min=0;
    int vfi_index = -1;
    int num_vfi = 0;
    int vp0 = -1; /* virtual port 1 */
    int vp1 = -1; /* virtual port 2 */


     num_vfi = soc_mem_index_count(unit, VFIm);

    _BCM_MIM_VPN_SET(mim_vpn_min, _BCM_MIM_VPN_TYPE_MIM, 0);

    if (vpn < mim_vpn_min || vpn > (mim_vpn_min + num_vfi - 1) ) {
        return BCM_E_PARAM;
    }

    _BCM_MIM_VPN_GET(vfi_index, _BCM_MIM_VPN_TYPE_MIM, vpn);

    if (!(_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeMim))) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN
        (READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    if (soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf)) {
        vp0 = soc_VFIm_field32_get(unit, &vfi_entry, VP_0f);
        vp1 = soc_VFIm_field32_get(unit, &vfi_entry, VP_1f);
    } else {
        return BCM_E_PARAM;
    }

    if (vp == vp0) {
        soc_VFIm_field32_set(unit, &vfi_entry, VP_0f, 0);
    } else if (vp == vp1) {
        soc_VFIm_field32_set(unit, &vfi_entry, VP_1f, 0);
    } else {
        return BCM_E_PARAM;
    }

    return WRITE_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry);

}

STATIC int
_bcm_tr2_mim_peer_port_config_delete(int unit, int vp, bcm_mim_vpn_t vpn)
{
    uint32 ment[SOC_MAX_MEM_WORDS];
    uint32 egr_vlan_xlate_entry[SOC_MAX_MEM_WORDS];
    int vfi, rv = BCM_E_NONE;
    soc_mem_t egr_xlate_mem;
    soc_mem_t mpls_mem = MPLS_ENTRYm;
    int isid = -1, index = -1;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        egr_xlate_mem = EGR_VLAN_XLATE_1_DOUBLEm;
    } else
#endif
    {
        egr_xlate_mem = EGR_VLAN_XLATEm;
    }

    _BCM_MIM_VPN_GET(vfi, _BCM_MIM_VPN_TYPE_MIM, vpn);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_mim_decoupled_mode)) {
        int rv;

        rv = _bcm_td3_mim_untagged_payload_vlan_delete(unit,
                     VPN_ISID(unit, vfi), vp);
        BCM_IF_ERROR_RETURN(rv);
        sal_memset(egr_vlan_xlate_entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
        rv = _bcm_td3_mim_flex_port_egr_vxlt_op(unit, NULL, vfi, vp,
                 _MIM_EVXLT_OP_DELETE, egr_vlan_xlate_entry);
        BCM_IF_ERROR_RETURN(rv);
        isid = soc_mem_field32_get(unit, EGR_VLAN_XLATE_1_DOUBLEm,
                 egr_vlan_xlate_entry, DVP_VFI_ASSIGN_ISID__ISIDf);
    } else
#endif
    {
        sal_memset(egr_vlan_xlate_entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            soc_mem_field32_set(unit, egr_xlate_mem,
                egr_vlan_xlate_entry, DATA_TYPEf, 0x4);
            soc_mem_field32_set(unit, egr_xlate_mem,
                egr_vlan_xlate_entry, KEY_TYPEf, 0x4);
        } else
#endif
        {
            soc_mem_field32_set(unit, egr_xlate_mem,
                egr_vlan_xlate_entry, ENTRY_TYPEf, 0x4);
        }
        soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry,
            MIM_ISID__VFIf, vfi);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            soc_mem_field32_set(unit, egr_xlate_mem,
                   egr_vlan_xlate_entry, BASE_VALID_0f, 0x3);
            soc_mem_field32_set(unit, egr_xlate_mem,
                   egr_vlan_xlate_entry, BASE_VALID_1f, 0x7);
        } else
#endif
        {
            soc_mem_field32_set(unit, egr_xlate_mem,
                egr_vlan_xlate_entry, VALIDf, 0x1);
        }
        soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                                        MIM_ISID__DVPf, vp);

        rv = soc_mem_search(unit, egr_xlate_mem, MEM_BLOCK_ANY, &index,
                            egr_vlan_xlate_entry, egr_vlan_xlate_entry, 0);
        BCM_IF_ERROR_RETURN(rv);

        isid = soc_mem_field32_get(unit, egr_xlate_mem, egr_vlan_xlate_entry,
            MIM_ISID__ISIDf);
    }

    /* Delete the ISID-SVP entry in MPLS_ENTRY */
    sal_memset(ment, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
    soc_mem_field32_set(unit, mpls_mem, ment, KEY_TYPEf, 3); /* MIM_ISID_SVP */
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        soc_mem_field32_set(unit, mpls_mem, ment, BASE_VALID_0f, 3); 
        soc_mem_field32_set(unit, mpls_mem, ment, BASE_VALID_1f, 7); 
    } else
#endif
    {
        soc_mem_field32_set(unit, mpls_mem, ment, VALIDf, 0x1);
    }
    soc_mem_field32_set(unit, mpls_mem, ment, MIM_ISID__ISIDf, isid);
    soc_mem_field32_set(unit, mpls_mem, ment, MIM_ISID__SVPf, vp);
    rv = soc_mem_delete(unit, mpls_mem, MEM_BLOCK_ANY, ment);
    BCM_IF_ERROR_RETURN(rv);
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_mim_decoupled_mode)) {
    } else
#endif
    {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
        /*delete TP_ID from software*/
        BCM_IF_ERROR_RETURN(
            _bcm_tr2_mim_egr_vxlt_sd_tag_actions_delete(unit,
                                                    egr_vlan_xlate_entry));
#endif
        /* Delete the reverse entry in the egress */
        rv = soc_mem_delete(unit, egr_xlate_mem, MEM_BLOCK_ANY,
                            egr_vlan_xlate_entry);
    }
#if defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit, soc_feature_mim_peer_sharing)) {
        MIM_INFO(unit)->port_info[vp].vfi_count--;
    }
#endif /* BCM_TRIDENT_SUPPORT */

    return BCM_E_NONE;
}

#if defined(BCM_TRIDENT3_SUPPORT)
/*
 * Function:
 *      _bcm_tr2_mim_dvp_set
 * Purpose:
 *      Set Egress DVP tables
 * Parameters:
 *      IN :  Unit
 *      IN :  vp
 *      IN :  mim_port
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr2_mim_dvp_set(int unit, int vp, bcm_mim_port_t *mim_port)
{
    int rv = BCM_E_NONE;
    egr_dvp_attribute_entry_t  egr_dvp_attribute;
    int network_group = 0;
    soc_mem_t mem = EGR_DVP_ATTRIBUTEm;

    sal_memset(&egr_dvp_attribute, 0, sizeof(egr_dvp_attribute_entry_t));
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, vp, &egr_dvp_attribute);
    BCM_IF_ERROR_RETURN(rv);
    if (SOC_MEM_FIELD_VALID(unit, mem, COMMON__DVP_NETWORK_GROUPf)) {

        network_group = mim_port->network_group_id;
        rv = _bcm_validate_splithorizon_network_group(unit,
                mim_port->flags & BCM_MIM_PORT_TYPE_BACKBONE, &network_group);
        BCM_IF_ERROR_RETURN(rv);
        soc_mem_field32_set(unit, mem, &egr_dvp_attribute,
                COMMON__DVP_NETWORK_GROUPf, network_group);
    }

    if (SOC_MEM_FIELD_VALID(unit, mem, COMMON__EVXLT_KEY_SELf)) {
        if (mim_port->flags & BCM_MIM_PORT_TYPE_BACKBONE) {
            soc_mem_field32_set(unit, mem, &egr_dvp_attribute,
                    COMMON__EVXLT_KEY_SELf, 0x1);
        } else if (mim_port->flags & BCM_MIM_PORT_TYPE_PEER) {
            soc_mem_field32_set(unit, mem, &egr_dvp_attribute,
                   COMMON__EVXLT_KEY_SELf, 0x2);
        }
    }

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, vp, &egr_dvp_attribute);
    BCM_IF_ERROR_RETURN(rv);

    return rv;
}

STATIC int
_bcm_tr2_mim_dvp_get(int unit, int vp, bcm_mim_port_t *mim_port)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem = EGR_DVP_ATTRIBUTEm;
    egr_dvp_attribute_entry_t egr_dvp_attribute;

    sal_memset(&egr_dvp_attribute, 0, sizeof(egr_dvp_attribute_entry_t));
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, vp, &egr_dvp_attribute);
    BCM_IF_ERROR_RETURN(rv);

    if (SOC_MEM_FIELD_VALID(unit, mem, COMMON__EVXLT_KEY_SELf)) {
        if (soc_mem_field32_get(unit, mem, &egr_dvp_attribute,
                                 COMMON__EVXLT_KEY_SELf) == 0x1) {
            mim_port->flags |= BCM_MIM_PORT_TYPE_BACKBONE;
        } else if (soc_mem_field32_get(unit, mem, &egr_dvp_attribute,
                                        COMMON__EVXLT_KEY_SELf) == 0x2) {
            mim_port->flags |= BCM_MIM_PORT_TYPE_PEER;
        }
    }

    return rv;
}
#endif

/*
 * Function:
 *      bcm_mim_port_add
 * Purpose:
 *      Create a VPN instance
 * Parameters:
 *      unit  - (IN)  Device Number
 *      vpn   - (IN)  VPN id
 *      mim_port  - (IN/OUT) MIM port configuration info
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr2_mim_port_add(int unit, bcm_mim_vpn_t vpn, bcm_mim_port_t *mim_port)
{
    int drop, mode, is_local = 0, rv = BCM_E_PARAM, nh_index = 0;
    int old_tpid_enable = 0, tpid_enable = 0, tpid_index;
    bcm_port_t local_port;
    int i, vp = -1, num_vp, ipmc_id, vfi = -1;
    source_vp_entry_t svp;
    ing_dvp_table_entry_t dvp;
    vfi_entry_t vfi_entry;
    bcm_multicast_t mc_group = 0;
    int failover_vp = 0;
    int cml_default_enable=0, cml_default_new=0, cml_default_move=0;
    ing_pri_cng_map_entry_t imap;
    ing_untagged_phb_entry_t phb;
    int is_eline = 0;
    int network_group=0;
#if defined(BCM_KATANA2_SUPPORT)
    int my_modid = 0, mod_out =0, id = -1;
    bcm_port_t port_out = -1;
    bcm_trunk_t trunk_id = -1;
    int is_local_subport = FALSE;
    int min_subport = SOC_INFO(unit).pp_port_index_min; 
    int max_subport = SOC_INFO(unit).pp_port_index_max;
#endif
    bcm_mim_vpn_config_t vpn_info;
    int mim_use_default_reserve_svp = 0;
    _bcm_vp_info_t vp_info;
#if defined(BCM_TRIDENT3_SUPPORT)
    uint32 svp1[SOC_MAX_MEM_WORDS] = {0};
#endif

    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeMim;

    MIM_INIT(unit);

    rv = bcm_xgs3_l3_egress_mode_get(unit, &mode);
    BCM_IF_ERROR_RETURN(rv);
    if (!mode) {
        LOG_INFO(BSL_LS_BCM_L3,
                 (BSL_META_U(unit,
                             "L3 egress mode must be set first\n")));
        return BCM_E_DISABLED;
    }

    if (!(mim_port->flags & BCM_MIM_PORT_TYPE_BACKBONE)) {
        if (!_BCM_MIM_VPN_IS_SET(vpn)) {
            return BCM_E_PARAM;
        }
    }

    if ((SOC_IS_TRIUMPH2(unit) ||
        SOC_IS_TRIDENT3(unit) )&&
        !(mim_port->flags & BCM_MIM_PORT_TYPE_BACKBONE)) {
        if (vpn != _BCM_MIM_VPN_INVALID) {
            /* check if ELINE vpn service */
            BCM_IF_ERROR_RETURN(_bcm_tr2_mim_vpn_is_eline(unit, vpn, &is_eline));
        }
    }

    /* BCM_MIM_PORT_DEFAULT is used on TR2 B0, setting mim_port_id to
     * _BCM_MIM_DEFAULT_PORT
     */
     if (soc_feature(unit, soc_feature_mim_reserve_default_port) &&
        soc_property_get(unit, spn_RESERVE_MIM_DEFAULT_SVP, 0) &&
        (mim_port->flags & BCM_MIM_PORT_DEFAULT)) {

        BCM_GPORT_MIM_PORT_ID_SET(mim_port->mim_port_id,
                                _BCM_MIM_DEFAULT_PORT);
        vp = BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id);
        mim_use_default_reserve_svp = 1;
     }

    /* Allocate a new VP or use provided one */
    if (mim_port->criteria == BCM_MIM_PORT_MATCH_SHARE) {
        if (!soc_feature(unit, soc_feature_vp_sharing)) {
            return BCM_E_UNAVAIL;
        }
        if (mim_port->flags & BCM_MIM_PORT_TYPE_ACCESS) {
            vp_info.flags |= _BCM_VP_INFO_SHARED_PORT;
        } else {
            return BCM_E_UNAVAIL;
        }
    }

    if (mim_port->flags & BCM_MIM_PORT_SHARE) {
        if (!soc_feature(unit, soc_feature_mim_peer_sharing)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_TRIDENT_SUPPORT)
        else if (!(mim_port->flags & BCM_MIM_PORT_TYPE_PEER)) {
            return BCM_E_PARAM;
        }
#endif /* BCM_TRIDENT_SUPPORT */
    }

    bcm_mim_vpn_config_t_init(&vpn_info);
    if (vpn != _BCM_MIM_VPN_INVALID) {
        rv = bcm_tr2_mim_vpn_get(unit, vpn, &vpn_info);
        if (rv < 0 && !((rv == BCM_E_NOT_FOUND) && (mim_port->flags &
            BCM_MIM_PORT_TYPE_BACKBONE))) {
            goto port_cleanup;
        }
    }

    MIM_LOCK(unit);

    if (mim_port->flags & BCM_MIM_PORT_WITH_ID) {
        if (!BCM_GPORT_IS_MIM_PORT((mim_port->mim_port_id))) {
            MIM_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        vp = BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id);
        if (vp >= soc_mem_index_count(unit, SOURCE_VPm)) {
            MIM_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMim) &&
           (!mim_use_default_reserve_svp)) {
            if (!(mim_port->flags & BCM_MIM_PORT_REPLACE)) {
#if defined(BCM_TRIDENT_SUPPORT)
                if (soc_feature(unit, soc_feature_mim_peer_sharing) &&
                    (mim_port->flags & BCM_MIM_PORT_TYPE_PEER)) {
                    if (!(MIM_INFO(unit)->port_info[vp].flags &
                        _BCM_MIM_PORT_TYPE_SHARE)) {
                        MIM_UNLOCK(unit);
                        return BCM_E_EXISTS;
                    }

                    /* for share peer vp */
                    rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp);
                    if (rv < 0) {
                        MIM_UNLOCK(unit);
                        return rv;
                    }
                    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                                                              NEXT_HOP_INDEXf);

                    rv = _bcm_tr2_mim_peer_port_config_add(unit, mim_port,
                                                           vp, vpn, &vpn_info);
                    if (rv < 0) {
                        MIM_UNLOCK(unit);
                        return rv;
                    }

                    mim_port->encap_id = nh_index + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);

                    goto port_cleanup;
                 } else
#endif /* BCM_TRIDENT_SUPPORT */
                 {
                    MIM_UNLOCK(unit);
                    return BCM_E_EXISTS;
                 }
            }
        } else {
            if ((mim_port->flags & BCM_MIM_PORT_TYPE_ACCESS) || 
                    (mim_port->flags & BCM_MIM_PORT_TYPE_PEER)) {
                vp_info.flags &= ~(_BCM_VP_INFO_NETWORK_PORT);
            } else {
                vp_info.flags |= _BCM_VP_INFO_NETWORK_PORT;
            }

            rv = _bcm_vp_used_set(unit, vp, vp_info);
            if (rv < 0) {
                MIM_UNLOCK(unit);
                return rv;
            }
        }
    } else if (!mim_use_default_reserve_svp) {
        /* allocate a new VP index */
        num_vp = soc_mem_index_count(unit, SOURCE_VPm);
        if ((mim_port->flags & BCM_MIM_PORT_TYPE_ACCESS) || 
                (mim_port->flags & BCM_MIM_PORT_TYPE_PEER)) {
            vp_info.flags &= ~(_BCM_VP_INFO_NETWORK_PORT);
        } else {
            vp_info.flags |= _BCM_VP_INFO_NETWORK_PORT;
        }

        rv = _bcm_vp_alloc(unit, 0, (num_vp - 1), 1, SOURCE_VPm, vp_info, &vp);
        if (rv < 0) {
            MIM_UNLOCK(unit);
            return rv;
        }
    }

    /* Deal with the different kinds of VPs */
    if (mim_port->flags & BCM_MIM_PORT_TYPE_ACCESS) {
        /* Deal with access VPs */
        if (vpn == _BCM_MIM_VPN_INVALID) {
            if (mim_port->criteria == BCM_MIM_PORT_MATCH_SHARE) {
                vfi = -1;
            } else {
                MIM_UNLOCK(unit);
                return BCM_E_PARAM;
            }
        } else {
            _BCM_MIM_VPN_GET(vfi, _BCM_MIM_VPN_TYPE_MIM, vpn);
     
            if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
                MIM_UNLOCK(unit);
                return BCM_E_NOT_FOUND;
            }
        }
    } else if (mim_port->flags & BCM_MIM_PORT_TYPE_PEER) {
        /* Deal with peer VPs - tunnel and associated VPN */
        if (vpn == _BCM_MIM_VPN_INVALID) {
            MIM_UNLOCK(unit);
            return BCM_E_PARAM;
        }
         _BCM_MIM_VPN_GET(vfi, _BCM_MIM_VPN_TYPE_MIM, vpn);
        if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
            MIM_UNLOCK(unit);
            return BCM_E_NOT_FOUND;
        }
    }

    /* Program, the next hop, matching and egress tables */
    if (mim_port->flags & BCM_MIM_PORT_REPLACE) {
        /* For existing access ports, NH entry already exists */
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            rv = READ_SVP_ATTRS_1m(unit, MEM_BLOCK_ANY, vp, &svp1);
            if (rv < 0) {
                MIM_UNLOCK(unit);
                return rv;
            }
        }
#endif
        rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
        if (rv < 0) {
            MIM_UNLOCK(unit);
            return rv;
        }
        rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp);
        if (rv < 0) {
            MIM_UNLOCK(unit);
            return rv;
        }
        nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                                                  NEXT_HOP_INDEXf);
        if (soc_SOURCE_VPm_field32_get(unit, &svp, SD_TAG_MODEf)) {
            /* SD-tag mode, save the old TPID enable bits */
#if defined(BCM_TRIDENT3_SUPPORT)
            if (SOC_IS_TRIDENT3X(unit)) {
                old_tpid_enable = soc_SVP_ATTRS_1m_field32_get(unit, &svp1,
                                                            TPID_ENABLEf);
            } else
#endif
            {
                old_tpid_enable = soc_SOURCE_VPm_field32_get(unit, &svp,
                                                         TPID_ENABLEf);
            }
        }
    } else {
        sal_memset(&svp, 0, sizeof(source_vp_entry_t));
        sal_memset(&dvp, 0, sizeof(ing_dvp_table_entry_t));
    }

    /* if ELINE service check if it valid to add the port */
    if (is_eline) { 
        rv = _bcm_tr2_mim_eline_port_add(unit, vp, vpn, mim_port);
        if (rv < 0) {
            MIM_UNLOCK(unit);
            return rv;
        }

    }

    /* Program next hop entry using existing or new index */
    drop = (mim_port->flags & BCM_MIM_PORT_DROP) ? 1 : 0;
    rv = _bcm_tr2_mim_l2_nh_info_add(unit, mim_port, vp, drop,
                                    &nh_index, &local_port, &is_local);
    if (rv < 0) {
        goto port_cleanup;
    }

    /* Program the VP tables */
    if (mim_port->flags & BCM_MIM_PORT_MATCH_SERVICE_VLAN_TPID) {
        rv = _bcm_fb2_outer_tpid_entry_add(unit, mim_port->match_service_tpid,
                &tpid_index);
        if (rv < 0) {
            goto port_cleanup;
        }
        tpid_enable = (1 << tpid_index);
        soc_SOURCE_VPm_field32_set(unit, &svp, SD_TAG_MODEf, 1);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            soc_SVP_ATTRS_1m_field32_set(unit, &svp1, TPID_ENABLEf, tpid_enable);
        } else
#endif
        {
            soc_SOURCE_VPm_field32_set(unit, &svp, TPID_ENABLEf, tpid_enable);
        }
    } else {
        if (mim_port->flags & BCM_MIM_PORT_TYPE_BACKBONE) {
            soc_SOURCE_VPm_field32_set(unit, &svp, SD_TAG_VFI_ENABLEf, 1);
#if defined(BCM_TRIDENT2_SUPPORT)
            if (soc_mem_field_valid(unit, SOURCE_VPm, TPID_SOURCEf)) {
                soc_SOURCE_VPm_field32_set(unit, &svp, TPID_SOURCEf, 1);
            }
#endif /* BCM_TRIDENT2_SUPPORT */
        } else {
            soc_SOURCE_VPm_field32_set(unit, &svp, SD_TAG_MODEf, 0);
        }
    }
    soc_SOURCE_VPm_field32_set(unit, &svp, CLASS_IDf,
                               mim_port->if_class);
    if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, TRUST_OUTER_DOT1Pf)) {
        soc_SOURCE_VPm_field32_set(unit, &svp, TRUST_OUTER_DOT1Pf, 1);
    }
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
                                                defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) || SOC_IS_APACHE(unit)) {
        rv = _bcm_esw_add_policer_to_table(unit, mim_port->policer_id,
                                         SOURCE_VPm, 0 , &svp);
        BCM_IF_ERROR_RETURN(rv);
    }
#endif /* BCM_KATANA_SUPPORT  or BCM_TRIUMPH3_SUPPORT or BCM_APACHE_SUPPORT */

    if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
        network_group = mim_port->network_group_id;
        rv = _bcm_validate_splithorizon_network_group(unit,
                mim_port->flags & BCM_MIM_PORT_TYPE_BACKBONE,
                &network_group);
        BCM_IF_ERROR_RETURN(rv);
        soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_GROUPf,
                                   network_group);
    } else {
       if ((mim_port->flags & BCM_MIM_PORT_TYPE_ACCESS) || 
           (mim_port->flags & BCM_MIM_PORT_TYPE_PEER)) {
            soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_PORTf, 0);
       } else {
            soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_PORTf, 1);
       }
    }

    /* Assign fixed internal priority per VP or trust inner tag */
    if ((mim_port->flags & BCM_MIM_PORT_FIX_PRIORITY) &&
        (mim_port->flags & BCM_MIM_PORT_TYPE_ACCESS)) {
    
        sal_memset(&imap, 0, sizeof(ing_pri_cng_map_entry_t));
        sal_memset(&phb, 0, sizeof(ing_untagged_phb_entry_t));
        if (SOC_IS_ENDURO(unit) || SOC_IS_TRIUMPH2(unit)) {
            soc_SOURCE_VPm_field32_set(unit, &svp, TRUST_DOT1P_PTRf,
                                mim_port->int_pri + 16);
            soc_SOURCE_VPm_field32_set(unit, &svp, USE_INNER_PRIf, 0);
            soc_SOURCE_VPm_field32_set(unit, &svp, TRUST_OUTER_DOT1Pf, 0);

            BCM_IF_ERROR_RETURN(READ_ING_PRI_CNG_MAPm(unit, MEM_BLOCK_ANY,
                                        1024 + mim_port->int_pri, &imap));
            soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, &imap, PRIf,
                                mim_port->int_pri);
            BCM_IF_ERROR_RETURN(WRITE_ING_PRI_CNG_MAPm(unit, MEM_BLOCK_ANY,
                                         1024 + mim_port->int_pri, &imap)); 
        }
        
        if (SOC_IS_TRIUMPH3(unit)) {
            soc_mem_field32_set(unit, ING_UNTAGGED_PHBm,
                                &phb, PRIf, mim_port->int_pri);
            BCM_IF_ERROR_RETURN(WRITE_ING_UNTAGGED_PHBm(unit, MEM_BLOCK_ANY,
                                                  mim_port->int_pri, &phb));
            soc_SOURCE_VPm_field32_set(unit, &svp, USE_INNER_PRIf, 1);
            soc_SOURCE_VPm_field32_set(unit, &svp, TRUST_DOT1P_PTRf,
                                                   mim_port->int_pri);
        }
    }

    if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
#ifdef BCM_RIOT_SUPPORT
        /* Not setting dvp as network port for RIOT as all
         * routed packet gets dropped if overlay SVP and DVP
         * both are network ports.
         * SVP and DVP as network ports is a valid used case
         * but reg file says that this field
         * is set when DVP is a TRILL port */
        if ((BCMI_RIOT_IS_ENABLED(unit)) &&
                ((mim_port->flags & BCM_MIM_PORT_TYPE_BACKBONE) ||
                 (mim_port->flags & BCM_MIM_PORT_TYPE_PEER))) {
            /* explicitly setting it to zero. */
            if(!(soc_property_get(unit, spn_USE_ALL_SPLITHORIZON_GROUPS, 0))) {
                network_group = 0;
            }
        }
#endif
        if (soc_mem_field_valid(unit, ING_DVP_TABLEm, NETWORK_GROUPf)) {
            soc_ING_DVP_TABLEm_field32_set(unit, &dvp, NETWORK_GROUPf,
                                           network_group);
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_mim_decoupled_mode)) {
            rv = _bcm_td3_mim_flex_dvp_set(unit, vp, mim_port);
            if (rv < 0) {
                goto port_cleanup;
            }
        } else {
            if(SOC_IS_TRIDENT3X(unit)) {
                rv = _bcm_tr2_mim_dvp_set(unit, vp, mim_port);

                if (rv < 0) {
                    goto port_cleanup;
                }
            }
        }
#endif
    }


    /* Program the matching criteria */
    if (mim_port->flags & BCM_MIM_PORT_REPLACE) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            rv = WRITE_SVP_ATTRS_1m(unit, MEM_BLOCK_ANY, vp, &svp1);
            if (rv < 0) {
                goto port_cleanup;
            }
        }
#endif
        rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
        if (rv < 0) {
            goto port_cleanup;
        }

        if (soc_mem_field_valid(unit, ING_DVP_TABLEm, NETWORK_PORTf)) {
#ifdef BCM_RIOT_SUPPORT
            if (BCMI_RIOT_IS_ENABLED(unit)) {
                soc_ING_DVP_TABLEm_field32_set(unit, &dvp, NETWORK_PORTf, 0);
            } else
#endif
            {
                soc_ING_DVP_TABLEm_field32_set(
                    unit, &dvp, NETWORK_PORTf,
                    (mim_port->flags & BCM_MIM_PORT_TYPE_BACKBONE) ? 1 : 0);
            }
        }

        rv = WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp);
        if (rv < 0) {
            goto port_cleanup;
        }

        rv = _bcm_vp_ing_dvp_to_ing_dvp2_2(unit, &dvp, vp, network_group);
        if (rv < 0) {
            goto port_cleanup;
        }

        rv = _bcm_tr2_mim_match_add(unit, mim_port, vp, vpn);
        if (rv < 0) {
            goto port_cleanup;
        }
        if (mim_port->flags & BCM_MIM_PORT_TYPE_PEER) {
#if defined(BCM_TRIUMPH3_SUPPORT)
            if (SOC_IS_TRIUMPH3(unit)) {
                /* need to add support for Triumph3 */
            } else 
#endif /* BCM_TRIUMPH3_SUPPORT */
            {
                rv = _bcm_tr2_mim_peer_port_config_add(unit, mim_port, 
                                                       vp, vpn, &vpn_info);
            }
        }
        if (rv < 0) {
            goto port_cleanup;
        }
    } else {
        /* Link in the newly allocated next-hop entry */
        soc_ING_DVP_TABLEm_field32_set(unit, &dvp, NEXT_HOP_INDEXf,
                                       nh_index);

        if (soc_mem_field_valid(unit, ING_DVP_TABLEm, NETWORK_PORTf)) {
#ifdef BCM_RIOT_SUPPORT
            if (BCMI_RIOT_IS_ENABLED(unit)) {
                soc_ING_DVP_TABLEm_field32_set(unit, &dvp, NETWORK_PORTf, 0);
            } else
#endif
            {
                soc_ING_DVP_TABLEm_field32_set(
                    unit, &dvp, NETWORK_PORTf,
                    (mim_port->flags & BCM_MIM_PORT_TYPE_BACKBONE) ? 1 : 0);
            }
        }

        rv = WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp);
        if (rv < 0) {
            goto port_cleanup;
        }

        rv = _bcm_vp_ing_dvp_to_ing_dvp2_2(unit, &dvp, vp, network_group);
        if (rv < 0) {
            goto port_cleanup;
        }

        /* Initialize the SVP parameters */
        soc_SOURCE_VPm_field32_set(unit, &svp,
                                   ENTRY_TYPEf, 0x1); /* L2 VP */
        if ((vfi != -1) && (mim_port->criteria != BCM_MIM_PORT_MATCH_SHARE)) {
            soc_SOURCE_VPm_field32_set(unit, &svp, VFIf, vfi);
        }


        /*
         * if ELINE VPN service and the port type is access 
         * learning to be disabled
         */
        if (!((mim_port->flags & BCM_MIM_PORT_TYPE_ACCESS) && is_eline)) {
            /* Set the CML */
            rv = _bcm_vp_default_cml_mode_get (unit, 
                               &cml_default_enable, &cml_default_new, 
                               &cml_default_move);
             if (rv < 0) {
                 goto port_cleanup;
             }

            if (cml_default_enable) {
                /* Set the CML to default values */
                soc_SOURCE_VPm_field32_set(unit, &svp, 
                                           CML_FLAGS_NEWf, cml_default_new);
                soc_SOURCE_VPm_field32_set(unit, &svp,
                                           CML_FLAGS_MOVEf, cml_default_move);
            } else {
                /* Set the CML to PVP_CML_SWITCH by default (hw learn and forward) */
                soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_NEWf, 0x8);
                soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_MOVEf, 0x8);
            }
        }

        if (soc_mem_field_valid(unit, SOURCE_VPm, DISABLE_VLAN_CHECKSf)) {
            if (!soc_feature(unit, soc_feature_vlan_vfi_membership)) {
                soc_SOURCE_VPm_field32_set(unit, &svp, DISABLE_VLAN_CHECKSf, 1);
            }
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            rv = WRITE_SVP_ATTRS_1m(unit, MEM_BLOCK_ANY, vp, &svp1);
            if (rv < 0) {
                goto port_cleanup;
            }
        }
#endif
        rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
        if (rv < 0) {
            goto port_cleanup;
        }

#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            if ((vpn != _BCM_MIM_VPN_INVALID) &&
                ((mim_port->flags & BCM_MIM_PORT_TYPE_BACKBONE) ||
                 (mim_port->flags & BCM_MIM_PORT_TYPE_PEER))) {
                if ( BCM_E_NONE != soc_cancun_app_dest_entry_set (unit,
                    SOURCE_VPm, vp, ENTRY_TYPEf,
                    CANCUN_APP__SOURCE_VP__No_Control_Word, 0x1)) {
                    LOG_ERROR(BSL_LS_BCM_MIM,
                       (BSL_META_U(unit,"SOURCE_VP cancun app cfg error\n")));
                }
            }
        }
#endif

        /* Add the port to the VFI broadcast replication list */
#if defined(BCM_KATANA2_SUPPORT)
        if (soc_feature(unit, soc_feature_linkphy_coe) ||
            soc_feature(unit, soc_feature_subtag_coe)) {
            rv  =_bcm_esw_gport_resolve(unit, mim_port->port, &mod_out, 
                                   &port_out, &trunk_id, &id);
            if (rv < 0) {
                goto port_cleanup;
            }
            rv = bcm_esw_stk_my_modid_get(unit, &my_modid);
            if (rv < 0) {
                goto port_cleanup;
            }
            if (BCM_GPORT_IS_SET(mim_port->port) &&
                _BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit,
                    mim_port->port)) {
                if (_bcm_kt2_mod_is_coe_mod_check(unit, mod_out) == BCM_E_NONE) {
                    rv = _bcm_kt2_modport_to_pp_port_get(
                             unit, mod_out, port_out, &port_out);
                    if (rv < 0) {
                        goto port_cleanup;
                    }
#if defined BCM_METROLITE_SUPPORT
                    if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                        if (_SOC_IS_PP_PORT_LINKPHY_SUBTAG(unit, port_out)) {
                            is_local_subport = TRUE;
                        }
                    } else
#endif
                    {
                        if ((port_out >= min_subport ) &&
                            (port_out <= max_subport)) {
                            is_local_subport = TRUE;
                        }
                    }
                }
            }
            if ((is_local_subport) && (vfi != -1)) {
                /* Get IPMC index from HW */
                rv = READ_VFIm(unit, MEM_BLOCK_ANY, vfi, &vfi_entry);
                if (rv < 0) {
                    goto port_cleanup;
                }
#if defined(BCM_TRIDENT3_SUPPORT)
                if (soc_feature(unit, soc_feature_generic_dest)) {
                    uint32 dt = SOC_MEM_FIF_DEST_INVALID;
                    ipmc_id = soc_mem_field32_dest_get(unit, VFIm,
                            &vfi_entry, BC_DESTINATIONf, &dt);
                    if (dt != SOC_MEM_FIF_DEST_IPMC) {
                        rv = BCM_E_INTERNAL;
                        goto port_cleanup;
                    }
                } else
#endif
                {
                    ipmc_id = soc_VFIm_field32_get(unit,
                        &vfi_entry, BC_INDEXf);
                }
                _BCM_MULTICAST_GROUP_SET(mc_group,
                    _BCM_MULTICAST_TYPE_MIM, ipmc_id);
                rv = bcm_esw_multicast_egress_add(unit, mc_group,
                                             mim_port->port, nh_index + 
                                             BCM_XGS3_DVP_EGRESS_IDX_MIN(unit));
                if (rv < 0) {
                    goto port_cleanup;
                }
            }
        }
#endif
        if ((is_local || BCM_GPORT_IS_TRUNK(mim_port->port)) && 
            (vfi != -1) && (!is_eline)) {
            bcm_gport_t local_gport;

            /* Get IPMC index from HW */
            rv = READ_VFIm(unit, MEM_BLOCK_ANY, vfi, &vfi_entry);
            if (rv < 0) {
                goto port_cleanup;
            }
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_generic_dest)) {
                uint32 dt = SOC_MEM_FIF_DEST_INVALID;
                ipmc_id = soc_mem_field32_dest_get(unit, VFIm,
                            &vfi_entry, BC_DESTINATIONf, &dt);
                if (dt != SOC_MEM_FIF_DEST_IPMC) {
                    rv = BCM_E_INTERNAL;
                    goto port_cleanup;
                }
            } else
#endif
            {
                ipmc_id = soc_VFIm_field32_get(unit,
                    &vfi_entry, BC_INDEXf);
            }
            _BCM_MULTICAST_GROUP_SET(mc_group, _BCM_MULTICAST_TYPE_MIM, ipmc_id);
            if (is_local) {
                /* Convert system local_port to physical local_port */
                if (soc_feature(unit, soc_feature_sysport_remap)) {
                    BCM_XLATE_SYSPORT_S2P(unit, &local_port);
                }
                rv = bcm_esw_port_gport_get(unit, local_port, &local_gport);
                if (BCM_FAILURE(rv)) {
                    goto port_cleanup;
                }
            } else {
                local_gport = mim_port->port; /* Trunk gport */
            }
#if defined(BCM_ENDURO_SUPPORT)
            if (SOC_IS_ENDURO(unit)) {
                rv = bcm_esw_multicast_egress_add(unit, mc_group,
                                             local_gport, nh_index);
            } else 
#endif /* BCM_ENDURO_SUPPORT */
            {
                rv = bcm_esw_multicast_egress_add(unit, mc_group,
                                             local_gport, nh_index + 
                                             BCM_XGS3_DVP_EGRESS_IDX_MIN(unit));
            }
            if (rv < 0) {
                goto port_cleanup;
            }
        }

        /* Configure failover VP */
        if (BCM_SUCCESS(_bcm_tr2_mim_failover_check(unit, mim_port))) {
            failover_vp = BCM_GPORT_MIM_PORT_ID_GET(mim_port->failover_gport_id);
            if (failover_vp == vp) {
                rv = BCM_E_PORT;
                goto port_cleanup;
            }
            if (!_bcm_vp_used_get(unit, failover_vp, _bcmVpTypeMim)) {
                /* coverity[assigned_value] */
                rv = BCM_E_NOT_FOUND;
            }
        }

        /*
         * Match entries cannot be replaced, instead, callers
         * need to delete the existing entry and re-add with the
         * new match parameters.
         */
#if defined(BCM_TRIUMPH3_SUPPORT)    
        if (SOC_IS_TRIUMPH3(unit)) {
            rv = _bcm_tr3_mim_match_add(unit, mim_port, vp);
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
        {
            rv = _bcm_tr2_mim_match_add(unit, mim_port, vp, vpn);
        }

        if (rv < 0) {
            bcm_gport_t local_gport;
            if (is_local && !is_eline) {
                (void) bcm_esw_port_gport_get(unit, local_port, &local_gport);
#if defined(BCM_ENDURO_SUPPORT)
                if (SOC_IS_ENDURO(unit)) {
                    (void) bcm_esw_multicast_egress_delete(unit, mc_group, 
                                                       local_gport, nh_index);
                } else 
#endif /* BCM_ENDURO_SUPPORT */
                {
                    (void) bcm_esw_multicast_egress_delete(unit, mc_group, 
                                                       local_gport, nh_index + 
                                                       BCM_XGS3_DVP_EGRESS_IDX_MIN(unit));
                }
            }
#if defined(BCM_KATANA2_SUPPORT)
            if (soc_feature(unit, soc_feature_linkphy_coe) ||
                soc_feature(unit, soc_feature_subtag_coe)) {
                if (is_local_subport) {
                    (void) bcm_esw_multicast_egress_delete(unit, mc_group, 
                                               mim_port->port, nh_index + 
                                               BCM_XGS3_DVP_EGRESS_IDX_MIN(unit));
                }
            }
#endif
        } else {
            /* For peer ports, create entries in ingress and egress 
               VLAN translate tables */
            if (mim_port->flags & BCM_MIM_PORT_TYPE_PEER) {
#if defined(BCM_TRIUMPH3_SUPPORT)
                if (SOC_IS_TRIUMPH3(unit)) {
                    rv = _bcm_tr3_mim_peer_port_config_add(unit, mim_port,
                                                           vp, vpn);
                } else 
#endif /* BCM_TRIUMPH3_SUPPORT */
                {
                    rv = _bcm_tr2_mim_peer_port_config_add(unit, mim_port, 
                                                           vp, vpn, &vpn_info);
                }
            }
            /* Set the MIM port ID */
            BCM_GPORT_MIM_PORT_ID_SET(mim_port->mim_port_id, vp);
            mim_port->encap_id = nh_index + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        }
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

port_cleanup:
    if (rv < 0) {
        if (tpid_enable) {
            (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
        }
        if (!(mim_port->flags & BCM_MIM_PORT_REPLACE)) {
            if (!mim_use_default_reserve_svp && (vp != -1)) {
                (void) _bcm_vp_free(unit, _bcmVpTypeMim, 1, vp);
            }
            _bcm_tr2_mim_l2_nh_info_delete(unit, nh_index);
        }
        if (vp > 0) {
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
                                                    defined(BCM_APACHE_SUPPORT)
            if (SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) || SOC_IS_APACHE(unit)) {
            /* reset the policer entry to 0 and decrement reference count */
                _bcm_esw_reset_policer_from_table(unit, mim_port->policer_id, 
                                               SOURCE_VPm, vp, &svp); 
            }
#endif /* BCM_KATANA_SUPPORT  or BCM_TRIUMPH3_SUPPORT or BCM_APACHE_SUPPORT */
            sal_memset(&svp, 0, sizeof(source_vp_entry_t));
            (void)WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
#if defined(BCM_TRIDENT3_SUPPORT)
            sal_memset(&svp1, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
            if (SOC_IS_TRIDENT3X(unit)) {
                (void)WRITE_SVP_ATTRS_1m(unit, MEM_BLOCK_ANY, vp, &svp1);
            }
#endif
            (void)_bcm_vp_ing_dvp_config(unit, _bcmVpIngDvpConfigClear, vp,
                    ING_DVP_CONFIG_INVALID_VP_TYPE, 
                    ING_DVP_CONFIG_INVALID_INTF_ID, 
                    ING_DVP_CONFIG_INVALID_PORT_TYPE);
        }
    }
    if (old_tpid_enable) {
        for (i = 0; i < 4; i++) {
            if (old_tpid_enable & (1 << i)) {
                (void) _bcm_fb2_outer_tpid_entry_delete(unit, i);
                break;
            }
        }
    }
    MIM_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_tr2_mim_egress_dvp_reset
 * Purpose:
 *      Reet Egress DVP tables
 * Parameters:
 *      IN :  Unit
 *           IN :  vp
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr2_mim_egress_dvp_reset(int unit, int vp)
{
    int rv=BCM_E_NONE;
    egr_dvp_attribute_entry_t  egr_dvp_attribute;
    egr_dvp_attribute_1_entry_t  egr_dvp_attribute_1;

    if (SOC_MEM_IS_VALID(unit, EGR_DVP_ATTRIBUTEm)) {
        sal_memset(&egr_dvp_attribute, 0, sizeof(egr_dvp_attribute_entry_t));
        rv = soc_mem_write(unit, EGR_DVP_ATTRIBUTEm,
                                      MEM_BLOCK_ALL, vp, &egr_dvp_attribute);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (SOC_MEM_IS_VALID(unit, EGR_DVP_ATTRIBUTE_1m)) {
        sal_memset(&egr_dvp_attribute_1, 0, sizeof(egr_dvp_attribute_1_entry_t));
        rv = soc_mem_write(unit, EGR_DVP_ATTRIBUTE_1m,
                                      MEM_BLOCK_ALL, vp, &egr_dvp_attribute_1);
    }

    return rv;
}


STATIC int
_bcm_tr2_mim_port_delete(int unit, bcm_vpn_t vpn, int vp)
{
    int rv = BCM_E_NONE;
    int nh_index = 0, vfi, ipmc_id, is_local = FALSE;
    source_vp_entry_t svp;
    ing_dvp_table_entry_t dvp;
    ing_l3_next_hop_entry_t ing_nh;
    vfi_entry_t vfi_entry;
    bcm_module_t modid;
    bcm_port_t port;
    bcm_trunk_t tgid;
    bcm_multicast_t mc_group = 0;
    bcm_gport_t local_gport = BCM_GPORT_INVALID;
    _bcm_port_info_t *info;
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    int policer_id=0;
#endif
#if defined(BCM_KATANA2_SUPPORT)
    bcm_port_t pp_port = -1;
    int is_local_subport = FALSE;
    int min_subport = SOC_INFO(unit).pp_port_index_min;
    int max_subport = SOC_INFO(unit).pp_port_index_max;
#endif
    int is_eline = 0;
    int mim_reserve_svp_delete = 0;
    int old_tpid_enable, i;
    bcm_mim_port_t mim_port;
    uint32 port_flags;
    int is_trunk = 0;
#if defined(BCM_TRIDENT3_SUPPORT)
    uint32 svp1[SOC_MAX_MEM_WORDS] = {0};
#endif
    int tpid_enable_length = 0;

    /* We can not delete shared VP's match in this API, so if the
     * VP is using by match, do not delete this vp */
    if (MIM_INFO(unit)->port_info[vp].flags & _BCM_MIM_PORT_TYPE_ACCESS_SHARED) {
        if(MIM_INFO(unit)->port_info[vp].match_count >= 1) {
            return BCM_E_BUSY;
        }
    } else {
        if(MIM_INFO(unit)->port_info[vp].match_count > 1) {
            return BCM_E_BUSY;
        }
    }

#if defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit, soc_feature_mim_peer_sharing) &&
        (MIM_INFO(unit)->port_info[vp].flags & _BCM_MIM_PORT_TYPE_SHARE)) {
        if (MIM_INFO(unit)->port_info[vp].vfi_count > 1) {
            rv = _bcm_tr2_mim_peer_port_config_delete(unit, vp, vpn);
            return rv;
        }
    }
#endif /* BCM_TRIDENT_SUPPORT */

    _BCM_MIM_VPN_GET(vfi, _BCM_MIM_VPN_TYPE_MIM, vpn);

    if (!(MIM_INFO(unit)->port_info[vp].flags & _BCM_MIM_PORT_TYPE_NETWORK)) {
        if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
            return BCM_E_NOT_FOUND;
        }
    }

    if (SOC_IS_TRIUMPH2(unit) && (!(MIM_INFO(unit)->port_info[vp].flags & _BCM_MIM_PORT_TYPE_NETWORK))) {
        /* check if point-to-point vpn */
        BCM_IF_ERROR_RETURN(_bcm_tr2_mim_vpn_is_eline(unit, vpn, &is_eline));
    }
    rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp);
    BCM_IF_ERROR_RETURN(rv);
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);

#if defined(BCM_TRIUMPH3_SUPPORT)    
    if (SOC_IS_TRIUMPH3(unit)) {
        rv = _bcm_tr3_mim_match_delete(unit, vp);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
        _bcm_vp_info_t vp_info;
        BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
        if (!(vp_info.flags & _BCM_VP_INFO_SHARED_PORT)) {
            rv = _bcm_tr2_mim_match_delete(unit, vp, mim_port);
        }
    }

    if (rv < 0) {
        return rv;
    }

    /* Check if this port is on the local unit */
    rv = soc_mem_read(unit, ING_L3_NEXT_HOPm,
                      MEM_BLOCK_ANY, nh_index, &ing_nh);
    BCM_IF_ERROR_RETURN(rv);

    if (!(MIM_INFO(unit)->port_info[vp].flags & _BCM_MIM_PORT_TYPE_NETWORK)) {
        if (soc_feature(unit, soc_feature_generic_dest)) {
            uint32 dt, dv;
            dv = soc_mem_field32_dest_get(unit, ING_L3_NEXT_HOPm,
                    &ing_nh, DESTINATIONf, &dt);
            if (dt == SOC_MEM_FIF_DEST_LAG) {
                /* Trunk group */
                tgid = dv & SOC_MEM_FIF_DGPP_TGID_MASK;
                is_trunk = 1;
            } else {
                port = dv & SOC_MEM_FIF_DGPP_PORT_MASK;
                modid = (dv & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                          SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
            }
        } else {
            if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
                tgid = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, TGIDf);
                is_trunk = 1;
            } else {
                modid = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
                port = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, PORT_NUMf);
            }
        }

        if (!is_trunk) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_modid_is_local(unit, modid, &is_local));
            if (TRUE == is_local) {
                /* Convert system port to physical port */
                if (soc_feature(unit, soc_feature_sysport_remap)) {
                    BCM_XLATE_SYSPORT_S2P(unit, &port);
                }
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_gport_get(unit, port, &local_gport));
            }
#if defined(BCM_KATANA2_SUPPORT)
            if (soc_feature(unit, soc_feature_linkphy_coe) ||
                soc_feature(unit, soc_feature_subtag_coe)) {
                /* For LinkPHY/SubportPktTag pp_ports, check if the mod is a valid one */
                if (_bcm_kt2_mod_is_coe_mod_check(unit, modid) == BCM_E_NONE) {
                    BCM_IF_ERROR_RETURN(
                        _bcm_kt2_modport_to_pp_port_get(unit, modid,
                            port, &pp_port));
#if defined BCM_METROLITE_SUPPORT
                    if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                        if (_SOC_IS_PP_PORT_LINKPHY_SUBTAG(unit, pp_port)) {
                            _BCM_KT2_SUBPORT_PORT_ID_SET(local_gport, pp_port);
                            if (BCM_PBMP_MEMBER(
                                        SOC_INFO(unit).linkphy_pp_port_pbm, pp_port)) {
                                _BCM_KT2_SUBPORT_PORT_TYPE_SET(local_gport,
                                        _BCM_KT2_SUBPORT_TYPE_LINKPHY);
                            } else if (BCM_PBMP_MEMBER(
                                        SOC_INFO(unit).subtag_pp_port_pbm, pp_port)) {
                                _BCM_KT2_SUBPORT_PORT_TYPE_SET(local_gport,
                                        _BCM_KT2_SUBPORT_TYPE_SUBTAG);
                            } else {
                                return BCM_E_PORT;
                            }
                            /* If this port is on the local unit and is not a network port,
                             *                              *remove from replication list
                             *                                                           * Get IPMC index from HW */
                            BCM_IF_ERROR_RETURN(
                                    READ_VFIm(unit, MEM_BLOCK_ANY, vfi, &vfi_entry));
                            ipmc_id =
                                soc_VFIm_field32_get(unit, &vfi_entry, BC_INDEXf);
                            _BCM_MULTICAST_GROUP_SET(mc_group,
                                    _BCM_MULTICAST_TYPE_MIM, ipmc_id);

                            BCM_IF_ERROR_RETURN(
                                    bcm_esw_multicast_egress_delete(unit, mc_group,
                                        local_gport,
                                        nh_index + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit)));
                            is_local_subport = TRUE;
                        }
                    } else
#endif
                        /*For KT2, pp_port >=42 are used for LinkPHY/SubportPktTag subport.
                         * Get the subport info associated with the pp_port and form the
                         * subport_gport */
                    {
                        if ((pp_port >= min_subport) &&
                            (pp_port <= max_subport)) {
                            _BCM_KT2_SUBPORT_PORT_ID_SET(local_gport, pp_port);
                            if (BCM_PBMP_MEMBER(
                                        SOC_INFO(unit).linkphy_pp_port_pbm, pp_port)) {
                                _BCM_KT2_SUBPORT_PORT_TYPE_SET(local_gport,
                                        _BCM_KT2_SUBPORT_TYPE_LINKPHY);
                            } else if (BCM_PBMP_MEMBER(
                                        SOC_INFO(unit).subtag_pp_port_pbm, pp_port)) {
                                _BCM_KT2_SUBPORT_PORT_TYPE_SET(local_gport,
                                        _BCM_KT2_SUBPORT_TYPE_SUBTAG);
                            } else {
                                return BCM_E_PORT;
                            }
                            /* If this port is on the local unit and is not a network port, 
                             *remove from replication list 
                             * Get IPMC index from HW */
                            BCM_IF_ERROR_RETURN(
                                    READ_VFIm(unit, MEM_BLOCK_ANY, vfi, &vfi_entry));
                            ipmc_id =
                                soc_VFIm_field32_get(unit, &vfi_entry, BC_INDEXf);
                            _BCM_MULTICAST_GROUP_SET(mc_group,
                                    _BCM_MULTICAST_TYPE_MIM, ipmc_id);

                            BCM_IF_ERROR_RETURN(
                                    bcm_esw_multicast_egress_delete(unit, mc_group, 
                                        local_gport,
                                        nh_index + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit)));
                            is_local_subport = TRUE;
                        }
                        /* If this port is on the local unit and is not a network port, 
                        *remove from replication list 
                        * Get IPMC index from HW */
                        BCM_IF_ERROR_RETURN(
                            READ_VFIm(unit, MEM_BLOCK_ANY, vfi, &vfi_entry));
#if defined(BCM_TRIDENT3_SUPPORT)
                        if (soc_feature(unit, soc_feature_generic_dest)) {
                            uint32 dt = SOC_MEM_FIF_DEST_INVALID;
                            ipmc_id = soc_mem_field32_dest_get(unit, VFIm,
                                    &vfi_entry, BC_DESTINATIONf, &dt);
                            if (dt != SOC_MEM_FIF_DEST_IPMC) {
                                  return BCM_E_INTERNAL;
                            }
                        } else
#endif
                        {
                            ipmc_id = soc_VFIm_field32_get(unit,
                                          &vfi_entry, BC_INDEXf);
                        }
                        ipmc_id = soc_VFIm_field32_get(unit,
                                &vfi_entry, BC_INDEXf);
                        _BCM_MULTICAST_GROUP_SET(mc_group,
                            _BCM_MULTICAST_TYPE_MIM, ipmc_id);

                        BCM_IF_ERROR_RETURN(
                            bcm_esw_multicast_egress_delete(unit, mc_group, 
                                local_gport,
                                nh_index + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit)));
                        is_local_subport = TRUE;
                    }
                }
            }
#endif /* BCM_KATANA2_SUPPORT */
        } else {
            BCM_GPORT_TRUNK_SET(local_gport, tgid);
        }
       /*
        *  If point-to-point vpn service, BC_INDEXf
        *  UC_INDEXf are overlaid with VP_0f, VP_1f. 
        *  and are not relevant. skip multicast operations
        *  If this port is on the local unit and is not  
        *  a network port, remove from replication list
        */    

        if (((TRUE == is_local) || BCM_GPORT_IS_TRUNK(local_gport)) && 
            (!is_eline)) {
            /* Get IPMC index from HW */
            rv = READ_VFIm(unit, MEM_BLOCK_ANY, vfi, &vfi_entry);
            BCM_IF_ERROR_RETURN(rv);
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_generic_dest)) {
                 uint32 dt = SOC_MEM_FIF_DEST_INVALID;
                 ipmc_id = soc_mem_field32_dest_get(unit, VFIm,
                            &vfi_entry, BC_DESTINATIONf, &dt);
                 if (dt != SOC_MEM_FIF_DEST_IPMC) {
                      return BCM_E_INTERNAL;
                 }
            } else
#endif
            {
                ipmc_id = soc_VFIm_field32_get(unit,
                    &vfi_entry, BC_INDEXf);
            }
            _BCM_MULTICAST_GROUP_SET(mc_group, _BCM_MULTICAST_TYPE_MIM, ipmc_id);
#if defined(BCM_ENDURO_SUPPORT)            
            if (SOC_IS_ENDURO(unit)) {
                rv = bcm_esw_multicast_egress_delete(unit, mc_group, 
                                                     local_gport, nh_index);
            } else 
#endif /* BCM_ENDURO_SUPPORT */
            {
                rv = bcm_esw_multicast_egress_delete(unit, mc_group, 
                                                     local_gport, 
                                       nh_index + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit));
            }
            BCM_IF_ERROR_RETURN(rv);
        }
    }
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_global_meter)) {
        rv = _bcm_esw_get_policer_from_table(unit, SOURCE_VPm, vp, &svp, 
                                                      &policer_id, 0); 
        if(BCM_SUCCESS(rv)) {
             _bcm_esw_policer_decrement_ref_count(unit, policer_id);
        } else {
             return rv;
        }
    }
#endif /* BCM_KATANA_SUPPORT or BCM_TRIUMPH3_SUPPORT */

    /* if Point-to-Point vpn, clear the vp in VFI table */
    if (is_eline) {
        rv = _bcm_tr2_mim_eline_port_delete(unit, vpn, vp); 
        BCM_IF_ERROR_RETURN(rv);
    }
 
    /* Clear the SVP and DVP table entries */
    BCM_IF_ERROR_RETURN
        (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN(READ_SVP_ATTRS_1m(unit,
                          MEM_BLOCK_ANY, vp, &svp1));
        old_tpid_enable = soc_SVP_ATTRS_1m_field32_get(unit, &svp1,
                                           TPID_ENABLEf);
        tpid_enable_length = soc_mem_field_length(unit,
                             SVP_ATTRS_1m, TPID_ENABLEf);
    } else
#endif
    {
        old_tpid_enable = soc_SOURCE_VPm_field32_get(unit, &svp,
                                         TPID_ENABLEf);
        tpid_enable_length = soc_mem_field_length(unit,
                             SOURCE_VPm, TPID_ENABLEf);
    }

    if (old_tpid_enable) {
        for (i= 0; i < tpid_enable_length; i++) {
            if (old_tpid_enable & (1 << i)) {
                BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_entry_delete(unit, i));
            }
        }
    }
    sal_memset(&svp, 0, sizeof(source_vp_entry_t));
    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
    BCM_IF_ERROR_RETURN(rv);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        sal_memset(&svp1, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
        BCM_IF_ERROR_RETURN(WRITE_SVP_ATTRS_1m(unit, MEM_BLOCK_ANY, vp, &svp1));
    }
#endif

    sal_memset(&dvp, 0, sizeof(ing_dvp_table_entry_t));
    rv = _bcm_vp_ing_dvp_config(unit, _bcmVpIngDvpConfigClear, vp,
            ING_DVP_CONFIG_INVALID_VP_TYPE, 
            ING_DVP_CONFIG_INVALID_INTF_ID, 
            ING_DVP_CONFIG_INVALID_PORT_TYPE);
    BCM_IF_ERROR_RETURN(rv);

    /* Clear the next-hop table entries */
    rv = _bcm_tr2_mim_l2_nh_info_delete(unit, nh_index);
    BCM_IF_ERROR_RETURN(rv);

    /* For peer ports, delete entries in ingress and egress 
       VLAN translate tables */
    if (MIM_INFO(unit)->port_info[vp].flags & _BCM_MIM_PORT_TYPE_PEER) {
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            rv = _bcm_tr3_mim_peer_port_config_delete(unit, vp, vpn);
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
        {
            rv = _bcm_tr2_mim_peer_port_config_delete(unit, vp, vpn);
        }
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Update the physical port's SW state */
#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {

        if (is_local_subport && (MIM_INFO(unit)->port_info[vp].tgid == -1)) {
            _bcm_port_info_access(unit,
                MIM_INFO(unit)->port_info[vp].port, &info);
            info->vp_count--;
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_vlan_member_get(
                    unit, MIM_INFO(unit)->port_info[vp].port, &port_flags));
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_vlan_member_set(
                    unit, MIM_INFO(unit)->port_info[vp].port, port_flags));
        }
    }
#endif
    if (soc_feature(unit, soc_feature_generic_dest)) {
        uint32 dt, dv;
        dv = soc_mem_field32_dest_get(unit, ING_L3_NEXT_HOPm,
                &ing_nh, DESTINATIONf, &dt);
        if (dt == SOC_MEM_FIF_DEST_DGPP) {
            modid = (dv & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                        SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
            BCM_IF_ERROR_RETURN(
                _bcm_esw_modid_is_local(unit, modid, &is_local));
        }
    } else {
        if (!soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
            modid = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
            BCM_IF_ERROR_RETURN(
                _bcm_esw_modid_is_local(unit, modid, &is_local));
        }
    }
    if (is_local && (MIM_INFO(unit)->port_info[vp].tgid == -1)) {
        bcm_port_t phys_port = MIM_INFO(unit)->port_info[vp].port; 
        if (soc_feature(unit, soc_feature_sysport_remap)) { 
            BCM_XLATE_SYSPORT_S2P(unit, &phys_port); 
        }
        _bcm_port_info_access(unit, phys_port, &info);
        info->vp_count--;
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_vlan_member_get(unit, phys_port, &port_flags));
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_vlan_member_set(unit, phys_port, port_flags));
    }

    /* If associated with a trunk, update each local physical port's SW state */
    if (MIM_INFO(unit)->port_info[vp].tgid != -1) {
        int idx;
#if defined(BCM_ESW_SUPPORT) && (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
        bcm_port_t      local_member_array[SOC_MAX_NUM_PP_PORTS];
        int             max_num_ports = SOC_MAX_NUM_PP_PORTS;
#else
        bcm_port_t      local_member_array[SOC_MAX_NUM_PORTS];
        int             max_num_ports = SOC_MAX_NUM_PORTS;
#endif
        int local_member_count;

        bcm_trunk_t trunk_id = MIM_INFO(unit)->port_info[vp].tgid;
        BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, trunk_id,
                    max_num_ports, local_member_array, &local_member_count));
        for (idx = 0; idx < local_member_count; idx++) {
            _bcm_port_info_access(unit, local_member_array[idx], &info);
            info->vp_count--;
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_vlan_member_get(
                    unit, local_member_array[idx], &port_flags));
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_vlan_member_set(
                    unit, local_member_array[idx], port_flags));
        }
    }

    /* Clear the SW state */
    sal_memset(&(MIM_INFO(unit)->port_info[vp]), 0, 
               sizeof(_bcm_tr2_mim_port_info_t));

    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        bcm_gport_t gport;

        /* Release Service counter, if any */
        BCM_GPORT_MIM_PORT_ID_SET(gport, vp);
        _bcm_esw_flex_stat_handle_free(unit, _bcmFlexStatTypeGport, gport);
    }

    /* if spn_RESERVE_MIM_DEFAULT_SVP is set, mim svp 1 memory is already
     * allocated during init time, skipping svp 1 memory free
     */
    if(soc_feature(unit, soc_feature_mim_reserve_default_port) &&
       soc_property_get(unit, spn_RESERVE_MIM_DEFAULT_SVP, 0) &&
       (vp == _BCM_MIM_DEFAULT_PORT)) {
       mim_reserve_svp_delete = 1;
    }

    if (!mim_reserve_svp_delete) {
        BCM_IF_ERROR_RETURN(_bcm_tr2_mim_egress_dvp_reset(unit, vp));
        /* Free the VP */
        (void) _bcm_vp_free(unit, _bcmVpTypeMim, 1, vp);
    }
    return rv;
}

/*
 * Function:
 *      bcm_mim_port_delete
 * Purpose:
 *      Delete an mim port from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      mim_port_id - (IN) mim port information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_mim_port_delete(int unit, bcm_vpn_t vpn, bcm_gport_t mim_port_id)
{
    int vp, rv;

    MIM_INIT(unit);
    if (!BCM_GPORT_IS_MIM_PORT(mim_port_id)) {
        return BCM_E_PORT;
    }
    vp = BCM_GPORT_MIM_PORT_ID_GET(mim_port_id);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
        return BCM_E_NOT_FOUND;
    }
    if (!(MIM_INFO(unit)->port_info[vp].flags & _BCM_MIM_PORT_TYPE_NETWORK)) {
        if (!_BCM_MIM_VPN_IS_SET(vpn)) {
            return BCM_E_PARAM;
        }
    }
    MIM_LOCK(unit);
    rv = _bcm_tr2_mim_port_delete(unit, vpn, vp);

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    MIM_UNLOCK(unit);
    return rv;
}

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
/*
  * Function:
  *      _bcm_mim_share_vp_get
  * Purpose:
  *      Get shared VPs info per VPN
  * Parameters:
  *      unit       - (IN) Device Number
  *      vpn        - (IN) VPN that shared vp belong to
  *      vp_bitmap  - (OUT) shared vp bitmap
  * Returns:
  *      BCM_E_XXX
  */
int
_bcm_mim_share_vp_get(int unit, bcm_vpn_t vpn, SHR_BITDCL *vp_bitmap)
{
    void *vent;
    soc_mem_t mem;
    int index_min = 0;
    int index_max = 0;
    uint8 *xlate_buf = NULL;
    int buf_size = 0;
    int i = 0;
    int rv = BCM_E_NONE;
    int vfi = -1;
    int vp = -1;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
       mem = VLAN_XLATE_1_DOUBLEm;
    } else
#endif
    {
       mem = VLAN_XLATEm;
    }

    if(NULL == vp_bitmap) {
        return BCM_E_PARAM;
    }

    if (!_BCM_MIM_VPN_IS_SET(vpn)) {
        return BCM_E_PARAM;
    }

    _BCM_MIM_VPN_GET(vfi, _BCM_MIM_VPN_TYPE_MIM, vpn);
    if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
        return BCM_E_NOT_FOUND;
    }

    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);

    buf_size = SOC_MEM_TABLE_BYTES(unit, mem);
    xlate_buf = soc_cm_salloc(unit, buf_size, "VLAN_XLATE buffer");
    if (NULL == xlate_buf) {
        return BCM_E_MEMORY;
    }
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, index_min, index_max, xlate_buf);
    if (rv < 0) {
        soc_cm_sfree(unit, xlate_buf);
        return rv;
    }

    for (i = index_min; i <= index_max; i++) {
        vent = soc_mem_table_idx_to_pointer(unit,
            mem, void *, xlate_buf, i);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            if (soc_mem_field32_get(unit, mem,
                         vent, BASE_VALID_0f) != 3) {
                continue;
            }
            if (soc_mem_field32_get(unit, mem,
                         vent, BASE_VALID_1f) != 7) {
                continue;
            }
        } else
#endif
        {
            if (soc_mem_field32_get(unit, mem,
                         vent, VALIDf) == 0) {
                continue;
            }
        }

        if (soc_mem_field32_get(unit, mem, vent, XLATE__MPLS_ACTIONf) != 0x1) {
            continue;
        }

        if (soc_mem_field32_get(unit, mem, vent, XLATE__VFIf) != vfi) {
            continue;
        }

        vp = soc_mem_field32_get(unit, mem, vent, XLATE__SOURCE_VPf);
        SHR_BITSET(vp_bitmap, vp);
    }

    (void)soc_cm_sfree(unit, xlate_buf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mim_share_vp_delete
 * Purpose:
 *      Delete match criteria of shared VPs
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN that shared vp belong to
 *      vp      - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_mim_share_vp_delete(int unit, bcm_vpn_t vpn, int vp)
{
    void *vent;
    soc_mem_t mem;
    int index_min = 0;
    int index_max = 0;
    uint8 *xlate_buf = NULL;
    int buf_size = 0;
    int i = 0;
    int rv = BCM_E_NONE;
    int vfi = -1;
    int source_vp = -1;

    if (!_BCM_MIM_VPN_IS_SET(vpn)) {
        return BCM_E_PARAM;
    }

    _BCM_MIM_VPN_GET(vfi, _BCM_MIM_VPN_TYPE_MIM, vpn);
    if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
        return BCM_E_NOT_FOUND;
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
       mem = VLAN_XLATE_1_DOUBLEm;
    } else
#endif
    {
       mem = VLAN_XLATEm;
    }

    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);

    buf_size = SOC_MEM_TABLE_BYTES(unit, mem);
    xlate_buf = soc_cm_salloc(unit, buf_size, "VLAN_XLATE buffer");
    if (NULL == xlate_buf) {
        return BCM_E_MEMORY;
    }
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, index_min, index_max, xlate_buf);
    if (rv < 0) {
        soc_cm_sfree(unit, xlate_buf);
        return rv;
    }

    for (i = index_min; i <= index_max; i++) {
        vent = soc_mem_table_idx_to_pointer(unit,
            mem, void *, xlate_buf, i);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            if (soc_mem_field32_get(unit, mem,
                         vent, BASE_VALID_0f) != 3) {
                continue;
            }
            if (soc_mem_field32_get(unit, mem,
                         vent, BASE_VALID_1f) != 7) {
                continue;
            }
        } else
#endif
        {
            if (soc_mem_field32_get(unit, mem,
                         vent, VALIDf) == 0) {
                continue;
            }
        }

        if (soc_mem_field32_get(unit, mem, vent, XLATE__MPLS_ACTIONf) != 0x1) {
            continue;
        }

        if (soc_mem_field32_get(unit, mem, vent, XLATE__VFIf) != vfi) {
            continue;
        }

        source_vp = soc_mem_field32_get(unit, mem, vent, XLATE__SOURCE_VPf);
        if (vp != source_vp) {
            continue;
        }

        rv = soc_mem_delete_index(unit, mem, MEM_BLOCK_ALL, i);
        if (rv < 0) {
            soc_cm_sfree(unit, xlate_buf);
            return rv;
        }
        _bcm_tr2_mim_port_match_count_adjust(unit, source_vp, -1);
    }

    (void)soc_cm_sfree(unit, xlate_buf);

    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *      bcm_mim_port_delete_all
 * Purpose:
 *      Delete all mim ports from a VPN
 * Parameters:
 *      unit - Device Number
 *      vpn - VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_mim_port_delete_all(int unit, bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    uint32 vfi, vp, num_vp;
    source_vp_entry_t *psvp; 
    uint8 *source_vp_buf = NULL; 
    int network_port = 0;
    _bcm_vp_info_t vp_info;

    MIM_INIT(unit);

    if (!_BCM_MIM_VPN_IS_SET(vpn)) {
        return BCM_E_PARAM;
    }

    _BCM_MIM_VPN_GET(vfi, _BCM_MIM_VPN_TYPE_MIM, vpn);
    if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
        rv =  BCM_E_NOT_FOUND;
        goto done;
    }
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    source_vp_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, SOURCE_VPm), 
                                 "SOURCE_VP buffer");
     
    if (NULL == source_vp_buf) { 
        return BCM_E_MEMORY; 
    } 

    rv = soc_mem_read_range(unit, SOURCE_VPm, MEM_BLOCK_ANY, 
                           0, num_vp-1, source_vp_buf); 
    if (rv < 0) { 
        goto done; 
    } 

    for (vp = 0; vp < num_vp; vp++) { 
        network_port = FALSE;
        psvp = soc_mem_table_idx_to_pointer 
                (unit, SOURCE_VPm, source_vp_entry_t *, source_vp_buf, vp); 
        if (soc_SOURCE_VPm_field32_get(unit, psvp, ENTRY_TYPEf) != 0) {
            /* Network ports do not belong to a certain VPN uniquely,
             * it should be ignored. */
            BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
            if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
                network_port = TRUE;
            }
            /* Shared access ports do not belong to a certain VPN uniquely,
                      * Remove both software and hardware status of the given VPN. */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if ((!network_port) && (vp_info.flags & _BCM_VP_INFO_SHARED_PORT)) {
                if (MIM_INFO(unit)->port_info[vp].match_count > 0) {
                    BCM_IF_ERROR_RETURN(_bcm_mim_share_vp_delete(unit, vpn, vp));
                }
            }
#endif
            if((vfi == soc_SOURCE_VPm_field32_get(unit, psvp, VFIf)) &&
               (!network_port)) { 
                rv = _bcm_tr2_mim_port_delete(unit, vpn, vp); 
            }
            if (rv < 0) { 
                goto done; 
            } 
        }
    } 

    done: 
    if ( source_vp_buf != NULL ){
        soc_cm_sfree( unit, source_vp_buf); 
    }
    return rv; 
}

STATIC int
_bcm_tr2_mim_l2_nh_info_get(int unit, bcm_mim_port_t *mim_port, int nh_index)
{
    ing_l3_next_hop_entry_t ing_nh;
    egr_l3_next_hop_entry_t egr_nh, failover_egr_nh;
    egr_l3_intf_entry_t egr_intf;
    egr_mac_da_profile_entry_t macda;
    int action_present, action_not_present;
    int intf_num = -1, macda_idx = -1, tpid_idx = -1;
    bcm_module_t mod_out, mod_in;
    bcm_port_t port_out, port_in;
    bcm_trunk_t trunk_id;
    bcm_failover_t failover_id;
    int failover_nh_index = 0;
    int failover_vp;
    bcm_multicast_t  failover_mc_group = 0;
#if defined(BCM_KATANA2_SUPPORT)
    int is_local_subport = 0;
    bcm_port_t pp_port;
    int min_subport = SOC_INFO(unit).pp_port_index_min;
    int max_subport = SOC_INFO(unit).pp_port_index_max;
#endif
    int entry_type = 0;
    int is_trunk = 0;

    /* Read the HW entries */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                      nh_index, &ing_nh));
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                      nh_index, &egr_nh));

    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, 
                                                ENTRY_TYPEf) == 0x2) {
        if (soc_feature(unit, soc_feature_generic_dest)) {
            uint32 dt, dv;
            dv = soc_mem_field32_dest_get(unit, ING_L3_NEXT_HOPm,
                    &ing_nh, DESTINATIONf, &dt);
            if (dt == SOC_MEM_FIF_DEST_LAG) {
                /* Trunk group */
                trunk_id = dv & SOC_MEM_FIF_DGPP_TGID_MASK;
                is_trunk = 1;
            } else {
                port_in = dv & SOC_MEM_FIF_DGPP_PORT_MASK;
                mod_in = (dv & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                          SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
            }
        } else {
            if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
                trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, TGIDf);
                is_trunk = 1;
            } else {
                mod_in = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
                port_in = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, PORT_NUMf);
            }
        }

        if (is_trunk) {
            BCM_GPORT_TRUNK_SET(mim_port->port, trunk_id);
        } else {
#if defined(BCM_KATANA2_SUPPORT)
            is_local_subport = 0;
            if (soc_feature(unit, soc_feature_linkphy_coe) ||
                soc_feature(unit, soc_feature_subtag_coe)) {
                if (_bcm_kt2_mod_is_coe_mod_check(unit, mod_in) == BCM_E_NONE) {
                    BCM_IF_ERROR_RETURN(_bcm_kt2_modport_to_pp_port_get(
                        unit, mod_in, port_in, &pp_port));
#if defined BCM_METROLITE_SUPPORT
                    if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                        if (_SOC_IS_PP_PORT_LINKPHY_SUBTAG(unit, pp_port)) {
                            is_local_subport = 1;
                        }
                    } else
#endif
                    {
                        if ((pp_port >= min_subport) &&
                            (pp_port <= max_subport)) {
                            is_local_subport = 1;
                        }
                    }
                }
            }
            if (is_local_subport) {
                _BCM_KT2_SUBPORT_PORT_ID_SET(mim_port->port, pp_port);
                if (BCM_PBMP_MEMBER(SOC_INFO(unit).linkphy_pp_port_pbm,
                    pp_port)) {
                    _BCM_KT2_SUBPORT_PORT_TYPE_SET(mim_port->port,
                        _BCM_KT2_SUBPORT_TYPE_LINKPHY);
                } else if (BCM_PBMP_MEMBER(SOC_INFO(unit).subtag_pp_port_pbm,
                    pp_port)) {
                    _BCM_KT2_SUBPORT_PORT_TYPE_SET(mim_port->port,
                        _BCM_KT2_SUBPORT_TYPE_SUBTAG);
                } else {
                    return BCM_E_PORT;
                }
            } else
#endif
            {
                SOC_IF_ERROR_RETURN(_bcm_esw_stk_modmap_map(unit,
                    BCM_STK_MODMAP_GET, mod_in, port_in, &mod_out, &port_out));
                BCM_GPORT_MODPORT_SET(mim_port->port, mod_out, port_out);
            }
        }
    } else {
        return BCM_E_NOT_FOUND;
    }

#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                &egr_nh, DATA_TYPEf);
        } else
#endif /* BCM_TRIDENT3_SUPPORT */
        {
            entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                &egr_nh, ENTRY_TYPEf);
        }

    if (entry_type == 0x2) {
        action_present =
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                     SD_TAG__SD_TAG_ACTION_IF_PRESENTf);
        if (action_present) {
            mim_port->flags |= BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TAGGED;
        }
        action_not_present =
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                     SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf);

        if ((action_not_present == 0x1) || (action_present == 0x1)) {
            /* If SD tag action is ADD or REPLACE_VID_TPID, the tpid
             * index of the entry is valid. Get the tpid index for later.
             */
            tpid_idx =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                 SD_TAG__SD_TAG_TPID_INDEXf);
            mim_port->egress_service_vlan =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, SD_TAG__SD_TAG_VIDf);
            if (action_not_present) {
                mim_port->flags |= BCM_MIM_PORT_EGRESS_SERVICE_VLAN_ADD;
            }
            if (action_present) {
                mim_port->flags |= 
                    BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TPID_REPLACE;
            }
            _bcm_fb2_outer_tpid_entry_get(unit, &mim_port->egress_service_tpid, tpid_idx);
        } else if (action_present == 0x2) { /* REPLACE_VID_ONLY */
            mim_port->flags |= BCM_MIM_PORT_EGRESS_SERVICE_VLAN_REPLACE;
            mim_port->egress_service_vlan =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, SD_TAG__SD_TAG_VIDf);
        } else if (action_present == 0x3) { /* DELETE */
            mim_port->flags |= BCM_MIM_PORT_EGRESS_SERVICE_VLAN_DELETE;
        } else if (action_present == 0x4) { /* REPLACE VID TPID PRI ONLY */
            mim_port->flags |= 
                BCM_MIM_PORT_EGRESS_SERVICE_VLAN_PRI_TPID_REPLACE;
            mim_port->egress_service_vlan =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, SD_TAG__SD_TAG_VIDf);
            mim_port->egress_service_pri =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, SD_TAG__NEW_PRIf);
            mim_port->egress_service_cfi =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, SD_TAG__NEW_CFIf);
            tpid_idx =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                 SD_TAG__SD_TAG_TPID_INDEXf);
            _bcm_fb2_outer_tpid_entry_get(unit, &mim_port->egress_service_tpid, tpid_idx);
        } else if (action_present == 0x5) { /* REPLACE VID PRI ONLY */
            mim_port->flags |= 
                BCM_MIM_PORT_EGRESS_SERVICE_VLAN_PRI_REPLACE;
            mim_port->egress_service_vlan =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, SD_TAG__SD_TAG_VIDf);
            mim_port->egress_service_pri =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, SD_TAG__NEW_PRIf);
            mim_port->egress_service_cfi =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, SD_TAG__NEW_CFIf);
        }
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
          else if (action_present == 0x6) {
            mim_port->flags |= 
                BCM_MIM_PORT_EGRESS_SERVICE_PRI_REPLACE;
            mim_port->egress_service_pri =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, SD_TAG__NEW_PRIf);
            mim_port->egress_service_cfi =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, SD_TAG__NEW_CFIf);
        } else if (action_present == 0x7){
            mim_port->flags |= 
                    BCM_MIM_PORT_EGRESS_SERVICE_TPID_REPLACE;
            tpid_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                 SD_TAG__SD_TAG_TPID_INDEXf);
            _bcm_fb2_outer_tpid_entry_get(unit, &mim_port->egress_service_tpid, tpid_idx); 
        }
#endif
    } else if (entry_type == 0x3) {
        /* Backbone / peer VP */
        /* Get the tunnel encap attributes */
        mim_port->egress_tunnel_vlan =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, MIM__BVIDf);
        intf_num = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, 
                                                    MIM__INTF_NUMf);
        macda_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, 
                                                    MIM__MAC_DA_PROFILE_INDEXf);
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_INTFm, MEM_BLOCK_ANY, 
                                          intf_num, &egr_intf));
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_MAC_DA_PROFILEm,  
                                         MEM_BLOCK_ANY, macda_idx, &macda));
        soc_mem_mac_addr_get(unit, EGR_L3_INTFm, &egr_intf, MAC_ADDRESSf, 
                             mim_port->egress_tunnel_srcmac); 
        soc_mem_mac_addr_get(unit, EGR_MAC_DA_PROFILEm, &macda, MAC_ADDRESSf, 
                             mim_port->egress_tunnel_dstmac);
        if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, MIM__ISID_LOOKUP_TYPEf)) {
            if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                 MIM__ISID_LOOKUP_TYPEf) == 0) {
                mim_port->flags |= BCM_MIM_PORT_TYPE_BACKBONE;
            } else {
                mim_port->flags |= BCM_MIM_PORT_TYPE_PEER;
            }
        }
        if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                             MIM__ADD_ISID_TO_MACDAf) == 1) {
            mim_port->flags |= BCM_MIM_PORT_EGRESS_TUNNEL_DEST_MAC_USE_SERVICE;
        }
    } else if (entry_type == 21) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_mim_decoupled_mode)) {
            int rv;

            rv = _bcm_td3_mim_flex_evxlt_tag_action_get(unit, mim_port, NULL,
                                                  NULL, (uint32 *)&egr_nh);
            BCM_IF_ERROR_RETURN(rv);
        }
#endif
    }

    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, DROPf)) {
        mim_port->flags |= BCM_MIM_PORT_DROP;
    }

#if defined(BCM_ENDURO_SUPPORT)
    if (!SOC_IS_ENDURO(unit)) {
#endif 
    if (soc_feature(unit, soc_feature_failover)) {
        /* Get failover info */
        BCM_IF_ERROR_RETURN(
             _bcm_esw_failover_prot_nhi_get (unit, nh_index, 
                        &failover_id, &failover_nh_index, &failover_mc_group));

        mim_port->failover_id = failover_id;
        if (failover_mc_group != 0) {
            _BCM_MULTICAST_GROUP_SET(mim_port->failover_mc_group,
                                     _BCM_MULTICAST_TYPE_MIM,
                                     _BCM_MULTICAST_ID_GET(failover_mc_group));
        }
        if (failover_nh_index > 0) {
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                             failover_nh_index, &failover_egr_nh));
#if defined(BCM_TRIDENT3_SUPPORT)
            if (SOC_IS_TRIDENT3X(unit)) {
                entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                    &failover_egr_nh, DATA_TYPEf);
            } else
#endif /* BCM_TRIDENT3_SUPPORT */
            {
                entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                    &failover_egr_nh, ENTRY_TYPEf);
            }

            if (entry_type == 0x3) {
                failover_vp = soc_EGR_L3_NEXT_HOPm_field32_get(unit, 
                                                               &failover_egr_nh, 
                                                               MIM__DVPf);
            } else if (entry_type == 0x2) {
                failover_vp = soc_EGR_L3_NEXT_HOPm_field32_get(unit, 
                                                               &failover_egr_nh, 
                                                               SD_TAG__DVPf);
            } else if (entry_type == 21) {
                failover_vp = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                                  &failover_egr_nh, L2_OTAG__DVPf);
            } else {
                return BCM_E_INTERNAL; /* should not get here */
            }
            BCM_GPORT_MIM_PORT_ID_SET(mim_port->failover_gport_id, failover_vp);
        }
    }

#if defined(BCM_ENDURO_SUPPORT)
    }
#endif
    return BCM_E_NONE;
}

STATIC int
_bcm_tr2_mim_match_get(int unit, bcm_mim_port_t *mim_port, int vp)
{
    int modid_count, rv = BCM_E_NONE;
    bcm_module_t mod_in, mod_out;
    bcm_port_t port_in, port_out;
    bcm_trunk_t trunk_id;
    uint64 temp_mac;

    if ((MIM_INFO(unit)->port_info[vp].flags & 
         _BCM_MIM_PORT_TYPE_ACCESS_PORT_VLAN) || 
        (MIM_INFO(unit)->port_info[vp].flags & 
         _BCM_MIM_PORT_TYPE_ACCESS_PORT_VLAN_STACKED)) {

        if (MIM_INFO(unit)->port_info[vp].match_count == 0) {
            return BCM_E_NONE;
        }
        mim_port->flags |= BCM_MIM_PORT_TYPE_ACCESS;
        mim_port->match_vlan = MIM_INFO(unit)->port_info[vp].match_vlan;

        if ((MIM_INFO(unit)->port_info[vp].flags & 
             _BCM_MIM_PORT_TYPE_ACCESS_PORT_VLAN_STACKED)) {
            mim_port->match_inner_vlan = 
                MIM_INFO(unit)->port_info[vp].match_inner_vlan;
            mim_port->criteria = BCM_MIM_PORT_MATCH_PORT_VLAN_STACKED; 
        } else {
            mim_port->criteria = BCM_MIM_PORT_MATCH_PORT_VLAN;
        }

        if ((MIM_INFO(unit)->port_info[vp].tgid != -1)) {
            trunk_id = MIM_INFO(unit)->port_info[vp].tgid;
            BCM_GPORT_TRUNK_SET(mim_port->port, trunk_id);
        } else {
            mod_in = MIM_INFO(unit)->port_info[vp].modid;
            port_in = MIM_INFO(unit)->port_info[vp].port;

            rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    mod_in, port_in, &mod_out, &port_out);
            BCM_GPORT_MODPORT_SET(mim_port->port, mod_out, port_out);
        }

    } else if (MIM_INFO(unit)->port_info[vp].flags & 
               _BCM_MIM_PORT_TYPE_ACCESS_PORT) {
        mim_port->flags |= (BCM_MIM_PORT_TYPE_ACCESS);
        mim_port->criteria = BCM_MIM_PORT_MATCH_PORT;
        modid_count = SOC_MODID_MAX(unit) + 1;

        if ( SOC_IS_TRIUMPH3(unit) ) {
            int src_trk_idx=0; /*Source Trunk table index.*/
            source_trunk_map_modbase_entry_t modbase_entry;
        
            BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &mod_in));
            src_trk_idx = MIM_INFO(unit)->port_info[vp].index % modid_count;
        
            BCM_IF_ERROR_RETURN
                (soc_mem_read(unit, SOURCE_TRUNK_MAP_MODBASEm,
            MEM_BLOCK_ANY, mod_in, &modbase_entry));
        
            port_in = src_trk_idx - soc_mem_field32_get(unit,
                    SOURCE_TRUNK_MAP_MODBASEm,
                    &modbase_entry, BASEf);
        } else {
            port_in = MIM_INFO(unit)->port_info[vp].index % modid_count;
            mod_in = MIM_INFO(unit)->port_info[vp].index / modid_count;
        }

        rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                mod_in, port_in, &mod_out, &port_out);
        BCM_GPORT_MODPORT_SET(mim_port->port, mod_out, port_out);

    } else if (MIM_INFO(unit)->port_info[vp].flags & 
               _BCM_MIM_PORT_TYPE_ACCESS_LABEL) {
        mim_port->flags |= (BCM_MIM_PORT_TYPE_ACCESS);
        mim_port->criteria = BCM_MIM_PORT_MATCH_LABEL;
        mim_port->match_label = MIM_INFO(unit)->port_info[vp].match_label;

        if ((MIM_INFO(unit)->port_info[vp].tgid != -1)) {
            trunk_id = MIM_INFO(unit)->port_info[vp].tgid;
            BCM_GPORT_TRUNK_SET(mim_port->port, trunk_id);
        } else {
            mod_in = MIM_INFO(unit)->port_info[vp].modid;
            port_in = MIM_INFO(unit)->port_info[vp].port;

            rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    mod_in, port_in, &mod_out, &port_out);
            BCM_GPORT_MODPORT_SET(mim_port->port, mod_out, port_out);
        }

    } else if (MIM_INFO(unit)->port_info[vp].flags & 
               (_BCM_MIM_PORT_TYPE_NETWORK | _BCM_MIM_PORT_TYPE_PEER)) {
        mim_port->criteria = BCM_MIM_PORT_MATCH_TUNNEL_VLAN_SRCMAC;
        if (MIM_INFO(unit)->port_info[vp].flags & _BCM_MIM_PORT_TYPE_PEER) {
            mim_port->flags |= BCM_MIM_PORT_TYPE_PEER;
#if defined(BCM_TRIDENT_SUPPORT)
            if (soc_feature(unit, soc_feature_mim_peer_sharing) &&
                (MIM_INFO(unit)->port_info[vp].flags &
                 _BCM_MIM_PORT_TYPE_SHARE)) {
                    mim_port->flags |= BCM_MIM_PORT_SHARE;
            }
#endif /* BCM_TRIDENT_SUPPORT */
        } else {
            mim_port->flags |= BCM_MIM_PORT_TYPE_BACKBONE;
        }

        SAL_MAC_ADDR_TO_UINT64
           (MIM_INFO(unit)->port_info[vp].match_tunnel_srcmac, temp_mac);
        SAL_MAC_ADDR_FROM_UINT64(mim_port->match_tunnel_srcmac, temp_mac);
        mim_port->match_tunnel_vlan = 
            MIM_INFO(unit)->port_info[vp].match_tunnel_vlan; 

        if ((MIM_INFO(unit)->port_info[vp].tgid != -1)) {
            trunk_id = MIM_INFO(unit)->port_info[vp].tgid;
            BCM_GPORT_TRUNK_SET(mim_port->port, trunk_id);
        } else {
            mod_in = MIM_INFO(unit)->port_info[vp].modid;
            port_in = MIM_INFO(unit)->port_info[vp].port;

            rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    mod_in, port_in, &mod_out, &port_out);
            BCM_GPORT_MODPORT_SET(mim_port->port, mod_out, port_out);
        }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_ingress_failover)) {
            uint32 ment[SOC_MAX_MEM_WORDS];
            int                index = -1;
            soc_mem_t mpls_mem = MPLS_ENTRYm;

            sal_memset(ment, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
            soc_mem_field32_set(unit, mpls_mem, ment, KEY_TYPEf, 1);
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_base_valid)) {
                soc_mem_field32_set(unit, mpls_mem, ment, BASE_VALID_0f, 3); 
                soc_mem_field32_set(unit, mpls_mem, ment, BASE_VALID_1f, 7); 
            } else
#endif
            {
                soc_mem_field32_set(unit, mpls_mem, ment, VALIDf, 0x1);
            }
            soc_mem_field32_set(unit, mpls_mem, ment, MIM_NVP__BVIDf,
                MIM_INFO(unit)->port_info[vp].match_tunnel_vlan);
            soc_mem_mac_addr_set(unit, mpls_mem, ment, MIM_NVP__BMACSAf,
                MIM_INFO(unit)->port_info[vp].match_tunnel_srcmac);
            BCM_IF_ERROR_RETURN(soc_mem_search(unit, mpls_mem, MEM_BLOCK_ANY,
                                               &index, ment, ment, 0));
            if (soc_mem_field32_get(unit, mpls_mem, ment,
                                            MIM_NVP__DROP_DATA_ENABLEf)) {
                mim_port->flags |= BCM_MIM_PORT_FAILOVER_DROP;
            }
            mim_port->ingress_failover_id =
                soc_mem_field32_get(unit, mpls_mem, ment,
                                            MIM_NVP__RX_PROT_GROUPf);
            if (mim_port->ingress_failover_id != 0) {
                _BCM_ENCAP_TYPE_IN_FAILOVER_ID(mim_port->ingress_failover_id,
                                               _BCM_FAILOVER_INGRESS);
            }
         }
#endif
    } else if (MIM_INFO(unit)->port_info[vp].flags & 
               _BCM_MIM_PORT_TYPE_ACCESS_SHARED) {
        mim_port->flags |= BCM_MIM_PORT_TYPE_ACCESS;
        mim_port->criteria = BCM_MIM_PORT_MATCH_SHARE;
    }
    return rv;
}

STATIC int
_bcm_tr2_mim_egr_vxlt_sd_tag_actions_get(
    int unit, bcm_mim_port_t *mim_port, bcm_mim_vpn_config_t *vpn_info,
    uint32 *egr_vlan_xlate_entry)
{
    int action_present, action_not_present, tpid_idx;
    soc_mem_t egr_xlate_mem;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        egr_xlate_mem = EGR_VLAN_XLATE_1_DOUBLEm;
    } else
#endif
    {
        egr_xlate_mem = EGR_VLAN_XLATEm;
    }

    /* Populate the API structure */
    action_present =
        soc_mem_field32_get(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                                        MIM_ISID__SD_TAG_ACTION_IF_PRESENTf);
    action_not_present =
        soc_mem_field32_get(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                                       MIM_ISID__SD_TAG_ACTION_IF_NOT_PRESENTf);
    if (mim_port != NULL) {
        if (action_present) {
            mim_port->flags |= BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TAGGED;
        }

    if ((action_not_present == 0x1) || (action_present == 0x1)) {
        /* If SD tag action is ADD or REPLACE_VID_TPID, the tpid
         * index of the entry is valid. Get the tpid index for later.
         */
        tpid_idx =
            soc_mem_field32_get(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                                            MIM_ISID__SD_TAG_TPID_INDEXf);
        mim_port->egress_service_vlan =
            soc_mem_field32_get(unit, egr_xlate_mem, egr_vlan_xlate_entry, 
                                            MIM_ISID__SD_TAG_VIDf);
        if (action_not_present) {
            mim_port->flags |= BCM_MIM_PORT_EGRESS_SERVICE_VLAN_ADD;
        }
        if (action_present) {
            mim_port->flags |= 
                BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TPID_REPLACE;
        }
        _bcm_fb2_outer_tpid_entry_get(unit, &mim_port->egress_service_tpid, tpid_idx);
    } else if (action_present == 0x2) { /* REPLACE_VID_ONLY */
        mim_port->flags |= BCM_MIM_PORT_EGRESS_SERVICE_VLAN_REPLACE;
        mim_port->egress_service_vlan =
            soc_mem_field32_get(unit, egr_xlate_mem, egr_vlan_xlate_entry, 
                                            MIM_ISID__SD_TAG_VIDf);
    } else if (action_present == 0x3) { /* DELETE */
        mim_port->flags |= BCM_MIM_PORT_EGRESS_SERVICE_VLAN_DELETE;
    }
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
      else if (action_present == 0x6) {
        mim_port->flags |= 
                BCM_MIM_PORT_EGRESS_SERVICE_PRI_REPLACE;
        mim_port->egress_service_pri =
            soc_mem_field32_get(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                                MIM_ISID__NEW_PRIf);
        mim_port->egress_service_cfi =
            soc_mem_field32_get(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                                MIM_ISID__NEW_CFIf);
    } else if (action_present == 0x7) {
        mim_port->flags |= 
                    BCM_MIM_PORT_EGRESS_SERVICE_TPID_REPLACE;
        tpid_idx = soc_mem_field32_get(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                                       MIM_ISID__SD_TAG_TPID_INDEXf);
        _bcm_fb2_outer_tpid_entry_get(unit, &mim_port->egress_service_tpid, tpid_idx);
    }
#endif
    } else if (vpn_info != NULL) {
        if (action_present == 0x7) {
            vpn_info->flags |= BCM_MIM_VPN_EGRESS_SERVICE_TPID_REPLACE;
            tpid_idx = soc_mem_field32_get(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                                           MIM_ISID__SD_TAG_TPID_INDEXf);
            _bcm_fb2_outer_tpid_entry_get(unit, &vpn_info->egress_service_tpid,
                                          tpid_idx);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr2_mim_peer_port_config_get
 * Purpose:
 *      Get the information of peer port
 * Parameters:
 *      unit     - (IN) Device Number
 *      mim_port - (IN/OUT) MIM port information
 *      vp       - (IN) Virtual Port
 *      vpn      - (IN) VPN instance ID
 */
STATIC int
_bcm_tr2_mim_peer_port_config_get(int unit, bcm_mim_port_t *mim_port,
                                  int vp, bcm_mim_vpn_t vpn)
{
    uint32 egr_vlan_xlate_entry[SOC_MAX_MEM_WORDS];
    int index = -1;
    int vfi   = -1;
    int isid  = -1;
    int rv    = BCM_E_NONE;
    soc_mem_t egr_xlate_mem;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        egr_xlate_mem = EGR_VLAN_XLATE_1_DOUBLEm;
    } else
#endif
    {
        egr_xlate_mem = EGR_VLAN_XLATEm;
    }

    _BCM_MIM_VPN_GET(vfi, _BCM_MIM_VPN_TYPE_MIM, vpn);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_mim_decoupled_mode)) {
        rv = _bcm_td3_mim_flex_port_egr_vxlt_op(unit, mim_port, vfi, vp,
                 _MIM_EVXLT_OP_GET, NULL);
        BCM_IF_ERROR_RETURN(rv);

        if (mim_port->flags & BCM_MIM_PORT_EGRESS_TUNNEL_SERVICE_REPLACE) {
            isid = mim_port->egress_tunnel_service & 0xffffff;
        } else {
            isid = VPN_ISID(unit, vfi);
        }
        rv = _bcm_td3_mim_untagged_payload_vlan_get(unit, NULL, mim_port, isid);
        BCM_IF_ERROR_RETURN(rv);
    } else
#endif
    {
        sal_memset(egr_vlan_xlate_entry, 0, sizeof(egr_vlan_xlate_entry));
        soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry, MIM_ISID__VFIf, vfi);

#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry, BASE_VALID_0f, 0x3);
            soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry, BASE_VALID_1f, 0x7);
            soc_mem_field32_set(unit, egr_xlate_mem,
                egr_vlan_xlate_entry, DATA_TYPEf, 0x4);
            soc_mem_field32_set(unit, egr_xlate_mem,
                egr_vlan_xlate_entry, KEY_TYPEf, 0x4);
        } else
#endif
        {
            soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry, ENTRY_TYPEf, 0x4);
            soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry, VALIDf, 0x1);
        }
        soc_mem_field32_set(unit, egr_xlate_mem, egr_vlan_xlate_entry, MIM_ISID__DVPf, vp);

        rv = soc_mem_search(unit, egr_xlate_mem, MEM_BLOCK_ANY, &index,
                            egr_vlan_xlate_entry, egr_vlan_xlate_entry, 0);
        BCM_IF_ERROR_RETURN(rv);

        isid = soc_mem_field32_get(unit, egr_xlate_mem, egr_vlan_xlate_entry,
                                   MIM_ISID__ISIDf);
        if (isid != VPN_ISID(unit, vfi)) {
            mim_port->egress_tunnel_service = isid;
            mim_port->flags |= BCM_MIM_PORT_EGRESS_TUNNEL_SERVICE_REPLACE;
        }

        BCM_IF_ERROR_RETURN(
            _bcm_tr2_mim_egr_vxlt_sd_tag_actions_get(unit, mim_port, NULL,
                                                     egr_vlan_xlate_entry));
    }
    return rv;
}

STATIC int
_bcm_tr2_mim_port_get(int unit, bcm_vpn_t vpn, int vp,
                        bcm_mim_port_t *mim_port)
{
    int i, nh_index, tpid_enable = 0, rv = BCM_E_NONE;
    ing_dvp_table_entry_t dvp;
    source_vp_entry_t svp;
#if defined(BCM_TRIDENT3_SUPPORT)
    uint32 svp1[SOC_MAX_MEM_WORDS] = {0};
#endif

    /* Initialize the structure */
    bcm_mim_port_t_init(mim_port);
    BCM_GPORT_MIM_PORT_ID_SET(mim_port->mim_port_id, vp);

    BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);

    if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
        if (soc_mem_field_valid(unit, ING_DVP_TABLEm, NETWORK_GROUPf)) {
            mim_port->network_group_id = soc_ING_DVP_TABLEm_field32_get(
                    unit, &dvp, NETWORK_GROUPf);
        }
    }

    /* Get the match parameters */
    rv = _bcm_tr2_mim_match_get(unit, mim_port, vp);
    BCM_IF_ERROR_RETURN(rv);

    /* Get the next-hop parameters */
    rv = _bcm_tr2_mim_l2_nh_info_get(unit, mim_port, nh_index);
    BCM_IF_ERROR_RETURN(rv);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_mim_decoupled_mode)) {
        rv = _bcm_td3_mim_flex_dvp_get(unit, vp, mim_port);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        if (SOC_IS_TRIDENT3X(unit)) {
            rv = _bcm_tr2_mim_dvp_get(unit, vp, mim_port);
            BCM_IF_ERROR_RETURN(rv);
        }
    }
#endif

    mim_port->encap_id = nh_index + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);

    /* For peer ports, get the peer information if configured */
    if (MIM_INFO(unit)->port_info[vp].flags & _BCM_MIM_PORT_TYPE_PEER) {
        if (!SOC_IS_TRIUMPH3(unit)) {
            rv = _bcm_tr2_mim_peer_port_config_get(unit, mim_port, vp, vpn);
            BCM_IF_ERROR_RETURN(rv);
        }
    }

    /* Fill in SVP parameters */
    BCM_IF_ERROR_RETURN (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN(READ_SVP_ATTRS_1m(unit, MEM_BLOCK_ANY, vp, &svp1));

        if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
            if (soc_mem_field_valid(unit, SOURCE_VPm, NETWORK_GROUPf)) {
                mim_port->network_group_id = soc_SOURCE_VPm_field32_get(unit,
                    &svp, NETWORK_GROUPf);
            }
        }
    }
#endif
    mim_port->if_class = soc_SOURCE_VPm_field32_get(unit, &svp, CLASS_IDf);
    if (soc_SOURCE_VPm_field32_get(unit, &svp, SD_TAG_MODEf)) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            tpid_enable = soc_SVP_ATTRS_1m_field32_get(unit, &svp1, TPID_ENABLEf);
        } else
#endif
        {
            tpid_enable = soc_SOURCE_VPm_field32_get(unit, &svp, TPID_ENABLEf);
        }
        if (tpid_enable) {
            mim_port->flags |= BCM_MIM_PORT_MATCH_SERVICE_VLAN_TPID;
            for (i = 0; i < 4; i++) {
                if (tpid_enable & (1 << i)) {
                    _bcm_fb2_outer_tpid_entry_get(unit, 
                        &mim_port->match_service_tpid, i);
                }
            }
        }
    }
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_global_meter)) {
        rv = _bcm_esw_get_policer_from_table(unit, SOURCE_VPm, vp, &svp, 
                                                      &mim_port->policer_id, 1);
    }
#endif
    return rv;
}

/*
 * Function:
 *      bcm_mim_port_get
 * Purpose:
 *      Get an mim port
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      mim_port  - (IN/OUT) mim port information
 */
int
bcm_tr2_mim_port_get(int unit, bcm_vpn_t vpn, bcm_mim_port_t *mim_port)
{
    int vp;

    MIM_INIT(unit);

    if (!_BCM_MIM_VPN_IS_SET(vpn)) {
        return BCM_E_PARAM;
    }
    if (!BCM_GPORT_IS_MIM_PORT(mim_port->mim_port_id)) {
        return BCM_E_PORT;
    }
    vp = BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
        return BCM_E_NOT_FOUND;
    }
    return _bcm_tr2_mim_port_get(unit, vpn, vp, mim_port);
}

/*
 * Function:
 *      bcm_mim_port_get_all
 * Purpose:
 *      Get all the mim ports from a VPN
 * Parameters:
 *      unit     - (IN) Device Number
 *      vpn      - (IN) VPN instance ID
 *      port_max   - (IN) Maximum number of interfaces in array
 *      port_array - (OUT) Array of mpls ports
 *      port_count - (OUT) Number of interfaces returned in array
 */
int
bcm_tr2_mim_port_get_all(int unit, bcm_vpn_t vpn, int port_max,
                        bcm_mim_port_t *port_array, int *port_count)
{
    int vp = 0, rv = BCM_E_NONE;
    uint32 vfi = -1, num_vp = 0;
    source_vp_entry_t svp;
    int network_port = 0;
    _bcm_vp_info_t vp_info;
    SHR_BITDCL *share_vp_bitmap = NULL;

    MIM_INIT(unit);

    if (!_BCM_MIM_VPN_IS_SET(vpn)) {
        return BCM_E_PARAM;
    }

    *port_count = 0;

    _BCM_MIM_VPN_GET(vfi, _BCM_MIM_VPN_TYPE_MIM, vpn);

    if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
        rv = BCM_E_NOT_FOUND;
        goto done;
    }

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    share_vp_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vp), "share_vp_bitmap");
    if (share_vp_bitmap == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(share_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vp_sharing)) {
        rv = _bcm_mim_share_vp_get(unit, vpn, share_vp_bitmap);
        if (rv < 0) {
            goto done;
        }
    }
#endif

    for (vp = 0; vp < num_vp; vp++) {
        if (*port_count == port_max) {
            break;
        }
        rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
        if (rv < 0) {
            goto done;
        }

        if (soc_SOURCE_VPm_field32_get( unit, &svp, ENTRY_TYPEf) == 1) {
            /* Network ports do not belong to a certain VPN uniquely, 
             * it should be ignored. */
            BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
            if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
                network_port = TRUE;
            }

            if(((vfi == soc_SOURCE_VPm_field32_get(unit, &svp, VFIf)) &&
                (!network_port))
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
                || (SHR_BITGET(share_vp_bitmap, vp)
                    && (vp_info.flags & _BCM_VP_INFO_SHARED_PORT))
#endif
               ) {
                rv = _bcm_tr2_mim_port_get(unit, vpn, vp, 
                                        &port_array[*port_count]);
                if ((rv < 0) && rv != BCM_E_NOT_FOUND) {
                    goto done;
                }
                if (rv != BCM_E_NOT_FOUND) {
                    BCM_GPORT_MIM_PORT_ID_SET
                        (port_array[*port_count].mim_port_id, vp);
                    (*port_count)++;
                }
            }
        }
    }
done:
    if(share_vp_bitmap != NULL) {
        sal_free(share_vp_bitmap);
    }
    return (*port_count > 0)? BCM_E_NONE:rv;
}

/*
 * Function:
 *      _bcm_tr2_mim_port_resolve
 * Purpose:
 *      Get the modid, port, trunk values for a MIM port
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr2_mim_port_resolve(int unit, bcm_gport_t mim_port_id,
                          bcm_module_t *modid, bcm_port_t *port,
                          bcm_trunk_t *trunk_id, int *id)

{
    int rv = BCM_E_NONE, nh_index, vp;
    ing_l3_next_hop_entry_t ing_nh;
    ing_dvp_table_entry_t dvp;

    MIM_INIT(unit);

    if (!BCM_GPORT_IS_MIM_PORT(mim_port_id)) {
        return (BCM_E_BADID);
    }

    vp = BCM_GPORT_MIM_PORT_ID_GET(mim_port_id);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
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

    if (soc_feature(unit, soc_feature_generic_dest)) {
        uint32 dt, dv;
        dv = soc_mem_field32_dest_get(unit, ING_L3_NEXT_HOPm,
                &ing_nh, DESTINATIONf, &dt);
        if (dt == SOC_MEM_FIF_DEST_LAG) {
            /* Trunk group */
            *trunk_id = dv & SOC_MEM_FIF_DGPP_TGID_MASK;
        } else {
            *port = dv & SOC_MEM_FIF_DGPP_PORT_MASK;
            *modid = (dv & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                      SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
        }
    } else {
        if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
            *trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, TGIDf);
        } else {
            *modid = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
            *port = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, PORT_NUMf);
        }
    }
    *id = vp;
    return rv;
}

/*
 * Function:
 *      bcm_tr2_mim_port_learn_get
 * Purpose:
 *      Get the CML bits for an mim port
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_mim_port_learn_get(int unit, bcm_gport_t mim_port_id, uint32 *flags)
{
    int rv, vp, cml = 0;
    source_vp_entry_t svp;

    MIM_INIT(unit);

    /* Get the VP index from the gport */
    vp = BCM_GPORT_MIM_PORT_ID_GET(mim_port_id);
    if (vp == -1) {
        return BCM_E_PARAM;
    }

    MEM_LOCK(unit, SOURCE_VPm);
    /* Be sure the entry is used and is set for VPLS */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeAny)) {
        MEM_UNLOCK(unit, SOURCE_VPm);
        return BCM_E_NOT_FOUND;
    }
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (rv < 0) {
        MEM_UNLOCK(unit, SOURCE_VPm);
        return rv;
    }
    if (soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf) != 1) { /* L2 VP */
        MEM_UNLOCK(unit, SOURCE_VPm);
        return BCM_E_NOT_FOUND;
    }
    MEM_UNLOCK(unit, SOURCE_VPm);
    cml = soc_SOURCE_VPm_field32_get(unit, &svp, CML_FLAGS_NEWf);

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
 *      bcm_tr2_mim_port_learn_set
 * Purpose:
 *      Set the CML bits for an mim port.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_mim_port_learn_set(int unit, bcm_gport_t mim_port_id, uint32 flags)
{
    int vp, cml = 0, rv = BCM_E_NONE;
    source_vp_entry_t svp;

    MIM_INIT(unit);

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
    vp = BCM_GPORT_MIM_PORT_ID_GET(mim_port_id);
    if (vp == -1) {
        return BCM_E_PARAM;
    }

    MEM_LOCK(unit, SOURCE_VPm);
    /* Be sure the entry is used and is set for MIM */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeAny)) {
        MEM_UNLOCK(unit, SOURCE_VPm);
        return BCM_E_NOT_FOUND;
    }
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (rv < 0) {
        MEM_UNLOCK(unit, SOURCE_VPm);
        return rv;
    }
    if (soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf) != 1) { /* L2 VP */
        MEM_UNLOCK(unit, SOURCE_VPm);
        return BCM_E_NOT_FOUND;
    }
    soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_MOVEf, cml);
    soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_NEWf, cml);
    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
    MEM_UNLOCK(unit, SOURCE_VPm);
    return rv;
}

int
_bcm_esw_mim_flex_stat_index_set(int unit, bcm_gport_t port, int fs_idx,
                                 uint32 flags)
{
    int rv, vp, nh_index;
    source_vp_entry_t svp;
    ing_dvp_table_entry_t dvp;
    egr_l3_next_hop_entry_t next_hop;

    vp = BCM_GPORT_MIM_PORT_ID_GET(port);
    if (vp == -1) {
        return BCM_E_PARAM;
    }

    rv = BCM_E_NONE;

    MIM_LOCK(unit);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
        MIM_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    } else {
        /* Ingress side */
        if (flags & _BCM_FLEX_STAT_HW_INGRESS) {
            rv = soc_mem_read(unit, SOURCE_VPm, MEM_BLOCK_ANY, vp, &svp);
            if (BCM_SUCCESS(rv)) {
                if (soc_mem_field_valid(unit, SOURCE_VPm, USE_VINTF_CTR_IDXf)) {
                    soc_mem_field32_set(unit, SOURCE_VPm, &svp, USE_VINTF_CTR_IDXf,
                                        fs_idx > 0 ? 1 : 0);
                }
                soc_mem_field32_set(unit, SOURCE_VPm, &svp, VINTF_CTR_IDXf,
                                fs_idx);
                rv = soc_mem_write(unit, SOURCE_VPm, MEM_BLOCK_ALL, vp, &svp);
            }
        }
        if (flags & _BCM_FLEX_STAT_HW_EGRESS) {
            if (BCM_SUCCESS(rv)) {
                /* Egress side */
                rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp);
            }
            if (BCM_SUCCESS(rv)) {
                nh_index = soc_mem_field32_get(unit, ING_DVP_TABLEm, &dvp,
                                               NEXT_HOP_INDEXf);
                rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                                  nh_index, &next_hop);
                if (BCM_SUCCESS(rv)) {
                    if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                            USE_VINTF_CTR_IDXf)) {
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &next_hop,
                                            USE_VINTF_CTR_IDXf,
                                            fs_idx > 0 ? 1 : 0);
                    }
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &next_hop,
                                    VINTF_CTR_IDXf, fs_idx);
                    rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ALL,
                                       nh_index, &next_hop);
                }
            }
        }
    }

    MIM_UNLOCK(unit);
    return rv;
}

int
_bcm_tr2_mim_svp_field_set(int unit, bcm_gport_t vp, 
                           soc_field_t field, int value)
{
    int rv = BCM_E_NONE;
    source_vp_entry_t svp;

    MIM_LOCK(unit);
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (rv < 0) {
        MIM_UNLOCK(unit);
        return rv;
    }
    if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, field)) {
        soc_SOURCE_VPm_field32_set(unit, &svp, field, value);
    } else {
        MIM_UNLOCK(unit);
        return BCM_E_UNAVAIL;
    }
    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
    MIM_UNLOCK(unit);
    return rv;
}

int 
_bcm_tr2_mim_port_phys_gport_get(int unit, int vp, bcm_gport_t *gp)
{
    if (MIM_INFO(unit)->port_info[vp].modid != -1) {
        BCM_GPORT_MODPORT_SET(*gp, MIM_INFO(unit)->port_info[vp].modid, 
                              MIM_INFO(unit)->port_info[vp].port);
    } else {
        BCM_GPORT_TRUNK_SET(*gp, MIM_INFO(unit)->port_info[vp].tgid);
    }
    return BCM_E_NONE;
}

void 
_bcm_tr2_mim_port_match_count_adjust(int unit, int vp, int step)
{
    MIM_INFO(unit)->port_info[vp].match_count += step;
}

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *     _bcm_mim_sw_dump
 * Purpose:
 *     Displays mim information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_mim_sw_dump(int unit)
{
    int i, num_vp, num_vfi;
    uint32 mac[2];

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information MIM - Unit %d\n"), unit));
    LOG_CLI((BSL_META_U(unit,
                        "  VPN Info    : \n")));

    num_vfi = soc_mem_index_count(unit, VFIm);
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    for (i = 0; i < num_vfi; i++) {
        if (VPN_ISID(unit, i) != 0) {
            LOG_CLI((BSL_META_U(unit,
                                "VFI = %x    ISID=%x\n"), i, VPN_ISID(unit, i)));
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n  Port Info    : \n")));
    for (i = 0; i < num_vp; i++) {
        if ((MIM_INFO(unit)->port_info[i].tgid == 0) && 
            (MIM_INFO(unit)->port_info[i].modid == 0) &&
            (MIM_INFO(unit)->port_info[i].port == 0)) {
            continue;
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n  MiM port vp = %d\n"), i));
        LOG_CLI((BSL_META_U(unit,
                            "Flags = %x\n"), MIM_INFO(unit)->port_info[i].flags));
        LOG_CLI((BSL_META_U(unit,
                            "Index = %x\n"), MIM_INFO(unit)->port_info[i].index));
        LOG_CLI((BSL_META_U(unit,
                            "TGID = %d\n"), MIM_INFO(unit)->port_info[i].tgid));
        LOG_CLI((BSL_META_U(unit,
                            "Modid = %d\n"), MIM_INFO(unit)->port_info[i].modid));
        LOG_CLI((BSL_META_U(unit,
                            "Port = %d\n"), MIM_INFO(unit)->port_info[i].port));
        LOG_CLI((BSL_META_U(unit,
                            "Match VLAN = %d\n"), 
                 MIM_INFO(unit)->port_info[i].match_vlan));
        LOG_CLI((BSL_META_U(unit,
                            "Match Inner VLAN = %d\n"), 
                 MIM_INFO(unit)->port_info[i].match_inner_vlan));
        LOG_CLI((BSL_META_U(unit,
                            "Match Label = %d\n"), 
                 MIM_INFO(unit)->port_info[i].match_label));
	SAL_MAC_ADDR_TO_UINT32
            (MIM_INFO(unit)->port_info[i].match_tunnel_srcmac, mac);
	LOG_CLI((BSL_META_U(unit,
                            "Match tunnel SrcMac = %x %x\n"), mac[1], mac[0]));
        LOG_CLI((BSL_META_U(unit,
                            "Match tunnel VLAN = %d\n"), 
                 MIM_INFO(unit)->port_info[i].match_tunnel_vlan));
        LOG_CLI((BSL_META_U(unit,
                            "Match Count = %d\n"), 
                 MIM_INFO(unit)->port_info[i].match_count));
        LOG_CLI((BSL_META_U(unit,
                            "Vfi Count = %d\n"),
                 MIM_INFO(unit)->port_info[i].vfi_count));

    }
    return;
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_chunk_memory_alloc
 * Purpose: 
 *      Allocate Chunk memory for set of memory entries.
 *      Chunk memory is allocated with offset at the beginning of the
 *      memory block.
 *
 *      +--------------+ [0]
 *      |  entry offset|
 *      +--------------+ [4]
 *      |  memory      |
 *      |     entries  |
 *      |              |
 *      +--------------+ [MEM_CHUNK_MAX_ENTRY * mem_entry size]
 * Parameters: 
 *      unit       - (IN) bcm device
 *      mem        - (IN) table memory
 *      mem_buffer - (OUT) Allocated memory chunk buffer
 *
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_chunk_memory_alloc(int unit, soc_mem_t mem, void **mem_buf)
{
    int entry_size;

    /* Validate parameters */
    if (!SOC_MEM_IS_VALID(unit, mem)) {
        return BCM_E_PARAM;
    }
 
    /* Allocate chunk memory */ 
    entry_size = soc_mem_entry_words(unit, mem) * sizeof(uint32);
    *mem_buf = soc_cm_salloc(unit, ((entry_size * MEM_CHUNK_MAX_ENTRY) +
                           sizeof(int32*)), "Memory Chunk Buffer");
    if (NULL == *mem_buf) {
        return BCM_E_MEMORY;
    }

    /* Initialize offset */
    _BCM_CHUNK_SET_INDEX_OFFSET(*mem_buf, -1); 

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_chunk_memory_read
 * Purpose: 
 *      Read and replenish chunk buffer for range of memory indexes
 *
 * Parameters: 
 *      unit       - (IN) bcm device
 *      mem        - (IN) table memory
 *      chunk      - (IN) chunk
 *      chunk_buf  - (IN) memory chunk buffer
 *
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 *    For generic usage, add atomic memory locks to update chunk buffer.
 */
STATIC int
_bcm_chunk_memory_read(int unit, soc_mem_t mem, int chunk, void *chunk_buf)
{
    int rv, entry_index_min, entry_index_max;
    void *mem_buf = _BCM_CHUNK_GET_MEM_BUFFER(chunk_buf);

    /* clear chunk buffer before refill */    
    sal_memset(chunk_buf, 0, ((soc_mem_entry_words(unit, mem) * sizeof(uint32) * 
                               MEM_CHUNK_MAX_ENTRY) + sizeof(int32*)));

    /* Read chunk of entries */
    entry_index_min = chunk * MEM_CHUNK_MAX_ENTRY;
    entry_index_max = entry_index_min + MEM_CHUNK_MAX_ENTRY - 1;
    if (entry_index_max > soc_mem_index_max(unit, mem)) {
        entry_index_max = soc_mem_index_max(unit, mem);
    }

    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                            entry_index_min, entry_index_max, mem_buf);
    if (BCM_FAILURE(rv)) {
        return rv;   
    }
    
    _BCM_CHUNK_SET_INDEX_OFFSET(chunk_buf, entry_index_min); 

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_chunk_get_mem_entry
 * Purpose: 
 *      Get memory entry from the memory chunk buffer. For the given index, memory chunk buffer is
 *      refilled with the range of memory index entries.
 *
 * Parameters: 
 *      unit       - (IN) bcm device
 *      mem        - (IN) table memory
 *      index      - (IN) index
 *      mem_buffer - (OUT) Allocated memory chunk buffer
 *
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_chunk_get_mem_entry(int unit, soc_mem_t mem, int index, 
                         void *chunk_buf, void **mem_entry)
{
    int rv, chunk, offset;
    void *mem_buf;

    /* Validate parameters */
    if (!SOC_MEM_IS_VALID(unit, mem) || (NULL == chunk_buf)) {
        return BCM_E_PARAM;
    }

    chunk = index / MEM_CHUNK_MAX_ENTRY;

    offset = _BCM_CHUNK_GET_INDEX_OFFSET(chunk_buf);

    if ((offset == -1) || 
        !(index >=  offset && index < (offset + MEM_CHUNK_MAX_ENTRY))) {
         /* Replenish chunk buffer*/
         rv = _bcm_chunk_memory_read(unit, mem, chunk, chunk_buf);
        if (BCM_FAILURE(rv)) {
            return rv;   
        }
    }
    mem_buf = _BCM_CHUNK_GET_MEM_BUFFER(chunk_buf);

    *mem_entry = soc_mem_table_idx_to_pointer(unit, mem, void*, mem_buf, 
                    _BCM_CHUNK_GET_INDEX(unit, mem, index));

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#if defined(BCM_TRIDENT2_SUPPORT)
/*
 * Function:
 *      _bcm_td2_mim_match_vp_replace
 * Purpose:
 *      Replace VP value in MIM VP's match entry.
 * Parameters:
 *      unit   - (IN) SOC unit number.
 *      vp     - (IN) MIM VP whose match entry is being replaced.
 *      new_vp - (IN) New VP value.
 *      old_vp - (OUT) Old VP value.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_td2_mim_match_vp_replace(int unit, int vp, int new_vp, int *old_vp)
{
    int                rv = BCM_E_NONE;
    soc_mem_t          mem;
    int                key_type = bcmVlanTranslateKeyInvalid;
    uint32             svp_valid;
    int                entry_index;
    int                port, my_modid, stm_idx, i;
    int                local_port[SOC_MAX_NUM_PORTS];
    int                local_port_count = 0;
    uint32 vent[SOC_MAX_MEM_WORDS], old_vent[SOC_MAX_MEM_WORDS];
    source_trunk_map_table_entry_t stm_entry;
    uint32 ment[SOC_MAX_MEM_WORDS], old_ment[SOC_MAX_MEM_WORDS];
    _bcm_tr2_mim_port_info_t *mkey = NULL;

    mkey = &(MIM_INFO(unit)->port_info[vp]);

    switch (mkey->flags) {
        case _BCM_MIM_PORT_TYPE_ACCESS_PORT_VLAN:
            if (key_type == bcmVlanTranslateKeyInvalid) {
                key_type = bcmVlanTranslateKeyPortOuter;
            }
            /* Fall through */
        case _BCM_MIM_PORT_TYPE_ACCESS_PORT_VLAN_STACKED:
            if (key_type == bcmVlanTranslateKeyInvalid) {
                key_type = bcmVlanTranslateKeyPortDouble;
            }

#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_base_valid)) {
               mem = VLAN_XLATE_1_DOUBLEm;
            } else
#endif
            {
               mem = VLAN_XLATEm;
            }

            if (mkey->modid != -1) {
                BCM_GPORT_MODPORT_SET(port, mkey->modid, mkey->port);
            } else {
                BCM_GPORT_TRUNK_SET(port, mkey->tgid);
            }
            /* Construct lookup key */
            sal_memset(vent, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
            rv = _bcm_trx_vlan_translate_entry_assemble(
                     unit, vent, port, key_type,
                     mkey->match_inner_vlan, mkey->match_vlan);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            soc_mem_lock(unit, mem);
            /* Lookup existing entry */
            rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL,
                                &entry_index, vent, old_vent, 0);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, mem);
                return rv;
            }
            /* Replace entry's VP value */
            *old_vp = soc_mem_field32_get(unit, mem, old_vent, SOURCE_VPf);
            soc_mem_field32_set(unit, mem, old_vent, SOURCE_VPf, new_vp);

            /* Insert new entry */
            rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, old_vent);
            if (rv == SOC_E_EXISTS) {
                rv = BCM_E_NONE;
            } else {
                soc_mem_unlock(unit, mem);
                return BCM_E_INTERNAL;
            }
            soc_mem_unlock(unit, mem);
            break;
        case _BCM_MIM_PORT_TYPE_ACCESS_PORT:
            mem = SOURCE_TRUNK_MAP_TABLEm;

            soc_mem_lock(unit, mem);

            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                              mkey->index, &stm_entry);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, mem);
                return rv;
            }
            /* Resolve SVP Mode */
            if (soc_mem_field_valid(unit, mem, SVP_VALIDf)) {
                svp_valid = soc_mem_field32_get(unit, mem,
                                                &stm_entry, SVP_VALIDf);
                if (svp_valid == 0) {
                    soc_mem_unlock(unit, mem);
                    return BCM_E_INTERNAL;
                }
            }
            *old_vp = soc_mem_field32_get(unit, mem, &stm_entry, SOURCE_VPf);
            rv = soc_mem_field32_modify(unit, mem, mkey->index,
                                        SOURCE_VPf, new_vp);
            soc_mem_unlock(unit, mem);
            break;
        case _BCM_MIM_PORT_TYPE_ACCESS_PORT_TRUNK:
            mem = SOURCE_TRUNK_MAP_TABLEm;

            /* Get module id for unit. */
            rv = bcm_esw_stk_my_modid_get(unit, &my_modid);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

            rv = _bcm_esw_trunk_local_members_get(
                     unit, mkey->tgid, SOC_MAX_NUM_PORTS,
                     local_port, &local_port_count);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

            soc_mem_lock(unit, mem);
            for (i = 0; i < local_port_count; i++) {
                /* Get index to source trunk map table */
                rv = _bcm_esw_src_mod_port_table_index_get(unit, my_modid,
                                                           local_port[i],
                                                           &stm_idx);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, mem);
                    return rv;
                }

                rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                  stm_idx, &stm_entry);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, mem);
                    return rv;
                }

                /* Resolve SVP Mode */
                if (soc_mem_field_valid(unit, mem, SVP_VALIDf)) {
                    svp_valid = soc_mem_field32_get(unit, mem,
                                                    &stm_entry, SVP_VALIDf);
                    if (svp_valid == 0) {
                        soc_mem_unlock(unit, mem);
                        return BCM_E_INTERNAL;
                    }
                }
                *old_vp = soc_mem_field32_get(unit, mem,
                                              &stm_entry, SOURCE_VPf);
                rv = soc_mem_field32_modify(unit, mem, stm_idx,
                                            SOURCE_VPf, new_vp);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, mem);
                    return rv;
                }
            }

            soc_mem_unlock(unit, mem);
            break;
        case _BCM_MIM_PORT_TYPE_ACCESS_LABEL:
            mem = MPLS_ENTRYm;
            sal_memset(ment, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
            sal_memset(old_ment, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
            soc_mem_lock(unit, mem);
            if (mkey->modid != -1) {
                soc_mem_field32_set(unit, mem, ment, MODULE_IDf, mkey->modid);
                soc_mem_field32_set(unit, mem, ment, PORT_NUMf, mkey->port);
            } else {
                soc_mem_field32_set(unit, mem, ment, Tf, 1);
                soc_mem_field32_set(unit, mem, ment, TGIDf, mkey->tgid);
            }
            soc_mem_field32_set(
                unit, mem, ment, MPLS_LABELf, mkey->match_label);
            rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &entry_index,
                                ment, old_ment, 0);
            if (rv == BCM_E_NOT_FOUND) {
                rv = BCM_E_PARAM;
            }
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, mem);
                return rv;
            }
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_base_valid)) {
                if (soc_mem_field32_get(unit, mem, old_ment, BASE_VALID_0f) != 3) {
                    soc_mem_unlock(unit, mem);
                    return BCM_E_INTERNAL;
                }
                if (soc_mem_field32_get(unit, mem, old_ment, BASE_VALID_1f) != 7) {
                    soc_mem_unlock(unit, mem);
                    return BCM_E_INTERNAL;
                }
            } else
#endif
            {
                if (!soc_mem_field32_get(unit, mem, old_ment, VALIDf)) {
                    soc_mem_unlock(unit, mem);
                    return BCM_E_INTERNAL;
                }

            }

            /* Replace entry's VP value */
            *old_vp = soc_mem_field32_get(unit, mem, old_ment,
                                                  SOURCE_VPf);
            soc_mem_field32_set(unit, mem, old_ment,
                                        SOURCE_VPf, new_vp);
            /* Insert new entry */
            rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, old_ment);
            if (rv == SOC_E_EXISTS) {
                rv = BCM_E_NONE;
            } else {
                soc_mem_unlock(unit, mem);
                return BCM_E_INTERNAL;
            }
            soc_mem_unlock(unit, mem);
            break;
        case _BCM_MIM_PORT_TYPE_NETWORK:
        case _BCM_MIM_PORT_TYPE_PEER:
            mem = MPLS_ENTRYm;
            sal_memset(ment, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
            sal_memset(old_ment, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

            soc_mem_lock(unit, mem);

            soc_mem_field32_set(unit, mem, ment, KEY_TYPEf, 0x1);/* MIM NVP */
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, ment, DATA_TYPEf, 0x1);
            }
            soc_mem_field32_set(unit, mem, ment, MIM_NVP__BVIDf,
                                        mkey->match_tunnel_vlan);
            soc_mem_mac_addr_set(unit, mem, ment, MIM_NVP__BMACSAf,
                                 mkey->match_tunnel_srcmac);
            rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY,
                                &entry_index, ment, old_ment, 0);
            if (rv == BCM_E_NOT_FOUND) {
                rv = BCM_E_PARAM;
            }

            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, mem);
                return rv;
            }

#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_base_valid)) {
                if (soc_mem_field32_get(unit, mem, old_ment, BASE_VALID_0f) != 3) {
                    soc_mem_unlock(unit, mem);
                    return BCM_E_INTERNAL;
                }
                if (soc_mem_field32_get(unit, mem, old_ment, BASE_VALID_1f) != 7) {
                    soc_mem_unlock(unit, mem);
                    return BCM_E_INTERNAL;
                }
            } else
#endif
            {
                if (!soc_mem_field32_get(unit, mem, old_ment, VALIDf)) {
                    soc_mem_unlock(unit, mem);
                    return BCM_E_INTERNAL;
                }

            }
            /* Replace entry's VP value */
            *old_vp = soc_mem_field32_get(unit, mem, old_ment,
                                                  MIM_NVP__SVPf);
            soc_mem_field32_set(unit, mem, old_ment,
                                        MIM_NVP__SVPf, new_vp);
            /* Insert new entry */
            rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, old_ment);
            if (rv == SOC_E_EXISTS) {
                rv = BCM_E_NONE;
            } else {
                soc_mem_unlock(unit, mem);
                return BCM_E_INTERNAL;
            }

            soc_mem_unlock(unit, mem);
            break;
        default:
            return BCM_E_PARAM;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td2_mim_port_source_vp_lag_set
 * Purpose:
 *      Set source VP LAG for a MIM virtual port.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      gport     - (IN) MIM virtual port GPORT ID.
 *      vp_lag_vp - (IN) VP representing the VP LAG.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_td2_mim_port_source_vp_lag_set(int unit, bcm_gport_t gport,
                                    int vp_lag_vp)
{
    int vp, old_vp;
    int rv = BCM_E_NONE;

    if (!BCM_GPORT_IS_MIM_PORT(gport)) {
        return BCM_E_PARAM;
    }

    vp = BCM_GPORT_MIM_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
        return BCM_E_PARAM;
    }

    /* Set source VP LAG by replacing the SVP field in MIM VP's
     * match entry with the VP value representing the VP LAG.
     */
    rv = _bcm_td2_mim_match_vp_replace(unit, vp, vp_lag_vp, &old_vp);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_mim_port_source_vp_lag_set
 * Purpose:
 *      Set source VP LAG for a MIM virtual port.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      gport     - (IN) MIM virtual port GPORT ID.
 *      vp_lag_vp - (IN) VP representing the VP LAG.
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_td2_mim_port_source_vp_lag_set(int unit, bcm_gport_t gport,
                                   int vp_lag_vp)
{
    int rv = BCM_E_NONE;

    MIM_INIT(unit);
    MIM_LOCK(unit);
    rv = _bcm_td2_mim_port_source_vp_lag_set(unit, gport, vp_lag_vp);
    MIM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_td2_mim_port_source_vp_lag_clear
 * Purpose:
 *      Clear source VP LAG for a MIM virtual port.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      gport     - (IN) MIM virtual port GPORT ID.
 *      vp_lag_vp - (IN) VP representing the VP LAG.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_td2_mim_port_source_vp_lag_clear(int unit, bcm_gport_t gport,
                                      int vp_lag_vp)
{
    int vp, old_vp;
    int rv = BCM_E_NONE;

    if (!BCM_GPORT_IS_MIM_PORT(gport)) {
        return BCM_E_PARAM;
    }

    vp = BCM_GPORT_MIM_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
        return BCM_E_PARAM;
    }

    /* Clear source VP LAG by replacing the SVP field in MIM VP's
     * match entry with the original VP value.
     */
    rv = _bcm_td2_mim_match_vp_replace(unit, vp, vp, &old_vp);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Check that the old VP value matches the VP value representing
     * the VP LAG or has been restored.
     */
    if ((old_vp != vp_lag_vp) && (old_vp != vp)) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_mim_port_source_vp_lag_clear
 * Purpose:
 *      Clear source VP LAG for a MIM virtual port.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      gport     - (IN) MIM virtual port GPORT ID.
 *      vp_lag_vp - (IN) VP representing the VP LAG.
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_td2_mim_port_source_vp_lag_clear(int unit, bcm_gport_t gport,
                                     int vp_lag_vp)
{
    int rv = BCM_E_NONE;

    MIM_INIT(unit);
    MIM_LOCK(unit);
    rv = _bcm_td2_mim_port_source_vp_lag_clear(unit, gport, vp_lag_vp);
    MIM_UNLOCK(unit);

    return rv;
}


/*
 * Function:
 *      _bcm_td2_mim_port_source_vp_lag_get
 * Purpose:
 *      Get source VP LAG for a MIM virtual port.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      gport     - (IN) MIM virtual port GPORT ID.
 *      vp_lag_vp - (OUT) VP representing the VP LAG.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_td2_mim_port_source_vp_lag_get(int unit, bcm_gport_t gport,
                                    int *vp_lag_vp)
{
    int                rv = BCM_E_NONE;
    soc_mem_t          mem;
    int                key_type = bcmVlanTranslateKeyInvalid;
    bcm_gport_t        vp;
    uint32             svp_valid;
    int                entry_index;
    int                port, my_modid, stm_idx;
    int                local_port[SOC_MAX_NUM_PORTS];
    int                local_port_count = 0;
    uint32 vent[SOC_MAX_MEM_WORDS], old_vent[SOC_MAX_MEM_WORDS];
    source_trunk_map_table_entry_t stm_entry;
    uint32 ment[SOC_MAX_MEM_WORDS], old_ment[SOC_MAX_MEM_WORDS];
    _bcm_tr2_mim_port_info_t *mkey = NULL;


    if (!BCM_GPORT_IS_MIM_PORT(gport)) {
        return BCM_E_PARAM;
    }

    vp = BCM_GPORT_MIM_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
        return BCM_E_PARAM;
    }

    mkey = &(MIM_INFO(unit)->port_info[vp]);

    switch(mkey->flags) {
        case _BCM_MIM_PORT_TYPE_ACCESS_PORT_VLAN:
            if (key_type == bcmVlanTranslateKeyInvalid) {
                key_type = bcmVlanTranslateKeyPortOuter;
            }
            /* Fall through */
        case _BCM_MIM_PORT_TYPE_ACCESS_PORT_VLAN_STACKED:
            if (key_type == bcmVlanTranslateKeyInvalid) {
                key_type = bcmVlanTranslateKeyPortDouble;
            }

#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_base_valid)) {
               mem = VLAN_XLATE_1_DOUBLEm;
            } else
#endif
            {
               mem = VLAN_XLATEm;
            }

            if (mkey->modid != -1) {
                BCM_GPORT_MODPORT_SET(port, mkey->modid, mkey->port);
            } else {
                BCM_GPORT_TRUNK_SET(port, mkey->tgid);
            }
            /* Construct lookup key */
            sal_memset(vent, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
            rv = _bcm_trx_vlan_translate_entry_assemble(
                     unit, vent, port, key_type,
                     mkey->match_inner_vlan, mkey->match_vlan);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            /* Lookup existing entry */
            rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL,
                                &entry_index, vent, old_vent, 0);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            /* Get VP value representing VP LAG */
            *vp_lag_vp = soc_mem_field32_get(unit, mem, old_vent,
                                                     SOURCE_VPf);
            break;
        case _BCM_MIM_PORT_TYPE_ACCESS_PORT:
            mem = SOURCE_TRUNK_MAP_TABLEm;

            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                              mkey->index, &stm_entry);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            /* Resolve SVP Mode */
            if (soc_mem_field_valid(unit, mem, SVP_VALIDf)) {
                svp_valid = soc_mem_field32_get(unit, mem,
                                                &stm_entry, SVP_VALIDf);
                if (svp_valid == 0) {
                    return BCM_E_INTERNAL;
                }
            }
            /* Get VP value representing VP LAG */
            *vp_lag_vp = soc_mem_field32_get(unit, mem,
                                             &stm_entry, SOURCE_VPf);
            break;
        case _BCM_MIM_PORT_TYPE_ACCESS_PORT_TRUNK:
            mem = SOURCE_TRUNK_MAP_TABLEm;

            /* Get module id for unit. */
            rv = bcm_esw_stk_my_modid_get(unit, &my_modid);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

            rv = _bcm_esw_trunk_local_members_get(
                     unit, mkey->tgid, SOC_MAX_NUM_PORTS,
                     local_port, &local_port_count);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

            /* Get index to source trunk map table,
             * Only get it from the first member in LAG
             */
            rv = _bcm_esw_src_mod_port_table_index_get(
                     unit, my_modid, local_port[0], &stm_idx);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, stm_idx, &stm_entry);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

            /* Resolve SVP Mode */
            if (soc_mem_field_valid(unit, mem, SVP_VALIDf)) {
                svp_valid = soc_mem_field32_get(unit, mem,
                                                &stm_entry, SVP_VALIDf);
                if (svp_valid == 0) {
                    return BCM_E_INTERNAL;
                }
            }
            /* Get VP value representing VP LAG */
            *vp_lag_vp = soc_mem_field32_get(unit, mem,
                                             &stm_entry, SOURCE_VPf);
            break;
        case _BCM_MIM_PORT_TYPE_ACCESS_LABEL:
            mem = MPLS_ENTRYm;
            sal_memset(ment, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
            sal_memset(old_ment, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
            if (mkey->modid != -1) {
                soc_mem_field32_set(unit, mem, ment, MODULE_IDf, mkey->modid);
                soc_mem_field32_set(unit, mem, ment, PORT_NUMf, mkey->port);
            } else {
                soc_mem_field32_set(unit, mem, ment, Tf, 1);
                soc_mem_field32_set(unit, mem, ment, TGIDf, mkey->tgid);
            }
            soc_mem_field32_set(
                unit, mem, ment, MPLS_LABELf, mkey->match_label);
            rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &entry_index,
                                ment, old_ment, 0);
            if (rv == BCM_E_NOT_FOUND) {
                rv = BCM_E_PARAM;
            }

            if (BCM_FAILURE(rv)) {
                return rv;
            }
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_base_valid)) {
                if (soc_mem_field32_get(unit, mem, old_ment, BASE_VALID_0f) != 3) {
                    return BCM_E_INTERNAL;
                }
                if (soc_mem_field32_get(unit, mem, old_ment, BASE_VALID_1f) != 7) {
                    return BCM_E_INTERNAL;
                }
            } else
#endif
            {
                if (!soc_mem_field32_get(unit, mem, old_ment, VALIDf)) {
                    return BCM_E_INTERNAL;
                }
            }
            /* Get VP value representing VP LAG */
            *vp_lag_vp = soc_mem_field32_get(unit, mem,
                                             old_ment, SOURCE_VPf);
            break;
        case _BCM_MIM_PORT_TYPE_NETWORK:
        case _BCM_MIM_PORT_TYPE_PEER:
            mem = MPLS_ENTRYm;
            sal_memset(ment, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
            sal_memset(old_ment, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

            soc_mem_field32_set(unit, mem, ment, KEY_TYPEf, 0x1);/* MIM NVP */

            soc_mem_field32_set(unit, mem, ment, MIM_NVP__BVIDf,
                                        mkey->match_tunnel_vlan);
            soc_mem_mac_addr_set(unit, mem, ment, MIM_NVP__BMACSAf,
                                 mkey->match_tunnel_srcmac);
            rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY,
                                &entry_index, ment, old_ment, 0);
            if (rv == BCM_E_NOT_FOUND) {
                rv = BCM_E_PARAM;
            }

            if (BCM_FAILURE(rv)) {
                return rv;
            }

            if (!soc_mem_field32_get(unit, mem, old_ment, VALIDf)) {
                return BCM_E_INTERNAL;
            }
            /* Get VP value representing VP LAG */
            *vp_lag_vp = soc_mem_field32_get(unit, mem,
                                             old_ment, MIM_NVP__SVPf);

            break;
        default:
            return BCM_E_PARAM;
    }

    if (!_bcm_vp_used_get(unit, *vp_lag_vp, _bcmVpTypeVpLag)) {
        return BCM_E_INTERNAL;
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2_mim_port_source_vp_lag_get
 * Purpose:
 *      Get source VP LAG for a MIM virtual port.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      gport     - (IN) MIM virtual port GPORT ID.
 *      vp_lag_vp - (OUT) VP representing the VP LAG.
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_td2_mim_port_source_vp_lag_get(int unit, bcm_gport_t gport,
                                   int *vp_lag_vp)
{
    int rv = BCM_E_NONE;

    MIM_INIT(unit);
    MIM_LOCK(unit);
    rv = _bcm_td2_mim_port_source_vp_lag_get(unit, gport, vp_lag_vp);
    MIM_UNLOCK(unit);

    return rv;
}

#endif /* BCM_TRIDENT2_SUPPORT */

#endif /* BCM_TRIUMPH2_SUPPORT */
