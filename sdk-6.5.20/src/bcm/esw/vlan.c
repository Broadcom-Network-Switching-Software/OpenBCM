/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Module: VLAN management
 *
 * Purpose:
 *     These routines manage the VTABLE entries as well as VLAN-related
 *     fields in the PTABLE.
 *
 *     The bcm layer keeps the VTABLE and PTABLE coherent in terms of
 *     what ports belong to what VLANs and which ports are untagged.
 */

#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/hash.h>

#include <sal/core/boot.h>
#include <bcm/error.h>
#include <bcm/vlan.h>

#include <bcm_int/common/lock.h>
#include <bcm_int/control.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/stg.h>
#include <bcm_int/esw/firebolt.h>
#if defined(BCM_TRX_SUPPORT)
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/mpls.h>
#include <bcm_int/esw/trx.h>
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/common/multicast.h>
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_TRIDENT_SUPPORT)
#include <bcm_int/esw/trident.h>
#endif /* BCM_TRIDENT_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
#include <soc/trident2.h>
#include <bcm_int/esw/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <soc/td2_td2p.h>
#include <bcm_int/esw/trident2plus.h>
#include <bcm_int/esw/vpn.h>
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
#include <soc/trident3.h>
#include <soc/esw/cancun.h>
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw/flow.h>
#endif /* BCM_TRIDENT3_SUPPORT */
#if defined(BCM_HGPROXY_COE_SUPPORT)
#include <bcm_int/esw/xgs5.h>
#endif /* BCM_HGPROXY_COE_SUPPORT */
#if defined(BCM_ENDURO_SUPPORT)
#include <bcm_int/esw/enduro.h>
#endif /* BCM_ENDURO_SUPPORT */
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/mim.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/multicast.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/ipmc.h>

#ifdef BCM_TRIUMPH3_SUPPORT
#include <bcm_int/esw/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_KATANA2_SUPPORT)
#include <soc/katana2.h>
#include <bcm_int/esw/katana2.h>
#include <bcm_int/esw/subport.h>
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_HURRICANE3_SUPPORT
#include <bcm_int/esw/hurricane3.h>
#include <soc/hurricane3.h>
#endif /* BCM_HURRICANE3_SUPPORT */
#ifdef BCM_GREYHOUND2_SUPPORT
#include <bcm_int/esw/greyhound2.h>
#include <soc/greyhound2.h>
#endif /* BCM_GREYHOUND2_SUPPORT */
#ifdef BCM_TOMAHAWK3_SUPPORT
#include <bcm_int/esw/tomahawk3.h>
#include <bcm_int/tomahawk3_dispatch.h>
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_HURRICANE2_SUPPORT
#include <soc/hurricane2.h>
#endif /* BCM_HURRICANE2_SUPPORT */

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw/policer.h>


bcm_vlan_info_t vlan_info[BCM_MAX_NUM_UNITS];

#ifndef BCM_VLAN_VID_MAX
#define BCM_VLAN_VID_MAX        4095
#endif

#define CHECK_INIT(unit)                                        \
        if (!vlan_info[unit].init)                              \
            return BCM_E_INIT

STATIC int _bcm_vlan_port_add(int unit, bcm_vlan_t vid, pbmp_t pbmp,
                           pbmp_t ubmp, pbmp_t ing_pbmp, int flags);
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
STATIC
bcm_error_t _bcm_esw_vlan_stat_get_table_info(
            int                        unit,
            bcm_vlan_t                 vlan,
            uint32                     *num_of_tables,
            bcm_stat_flex_table_info_t *table_info);
static 
bcm_error_t _bcm_esw_vlan_translate_stat_get_table_info(
            int                        unit, 
            bcm_gport_t                port,
            bcm_vlan_translate_key_t   key_type,
            bcm_vlan_t                 outer_vlan,
            bcm_vlan_t                 inner_vlan,
            uint32                     *num_of_tables,
            bcm_stat_flex_table_info_t *table_info);
static 
bcm_error_t _bcm_esw_vlan_translate_egress_stat_get_table_info(
            int                        unit, 
            int                        port_class,
            bcm_vlan_t                 outer_vlan,
            bcm_vlan_t                 inner_vlan,
            uint32                     *num_of_tables,
            bcm_stat_flex_table_info_t *table_info);
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int _bcm_vlan_reinit(int unit);
#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define BCM_WB_VERSION_1_2                SOC_SCACHE_VERSION(1,2)
#define BCM_WB_VERSION_1_3                SOC_SCACHE_VERSION(1,3)
#define BCM_WB_VERSION_1_4                SOC_SCACHE_VERSION(1,4)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_4
#else
#define    _bcm_vlan_reinit(unit)     (BCM_E_NONE)
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * For BCM internal use to set vlan create default flood mode.
 * Follow the same locking convention as other members in the structure
 */
/*
 * Set the default PFM to be used by vlan_create
 * Called from bcm_switch_control_set
 */
int
_bcm_esw_vlan_flood_default_set(int unit, bcm_vlan_mcast_flood_t mode)
{
    bcm_vlan_info_t             *vi = &vlan_info[unit];

    vi->flood_mode = mode;

    return BCM_E_NONE;
}

/*
 * Get the default PFM used by vlan_create
 * Called from bcm_switch_control_get, bcm_vlan_create
 */
int
_bcm_esw_vlan_flood_default_get(int unit, bcm_vlan_mcast_flood_t *mode)
{
    bcm_vlan_info_t             *vi = &vlan_info[unit];

    *mode = vi->flood_mode;

    return BCM_E_NONE;
}

/*
 * Set the PFM to be used in the Module Header for L2 packets when 
 * EGR_CONFIG.STATIC_MH_PFM == 1. When STATIC_MH_PFM==0, the PFM in 
 * the MH comes from the Vlan table.
 */
int
_bcm_esw_higig_flood_l2_set(int unit, bcm_vlan_mcast_flood_t mode)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    uint32              rval;

    if (soc_feature(unit, soc_feature_static_pfm)) {
        if (mode >= BCM_VLAN_MCAST_FLOOD_COUNT) {
            return BCM_E_PARAM;
        }
        SOC_IF_ERROR_RETURN(READ_EGR_CONFIG_1r(unit, &rval));
        soc_reg_field_set(unit, EGR_CONFIG_1r, &rval, MH_PFMf, mode);
        SOC_IF_ERROR_RETURN(WRITE_EGR_CONFIG_1r(unit, rval));
        return BCM_E_NONE;
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Get the PFM to be used in the Module Header for L2 packets when 
 * EGR_CONFIG.STATIC_MH_PFM == 1.
 */
int
_bcm_esw_higig_flood_l2_get(int unit, bcm_vlan_mcast_flood_t *mode)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    uint32              rval;

    if (soc_feature(unit, soc_feature_static_pfm)) {
        SOC_IF_ERROR_RETURN(READ_EGR_CONFIG_1r(unit, &rval));
        *mode = soc_reg_field_get(unit, EGR_CONFIG_1r, rval, MH_PFMf);
        return BCM_E_NONE;
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *   _bcm_esw_vlan_xlate_key_type_value_get
 * Purpose:
 *   get the hardware specific vlan translation key type value from 
 *   the common vlan translation key type for VLAN_XLATE table
 *
 * Parameters:
 *    key_type -       input, common key type 
 *    key_type_value - output, hardware specific key type value 
 */
int
_bcm_esw_vlan_xlate_key_type_value_get(int unit, 
                     int key_type, int *key_type_value)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {
        switch (key_type) {
        case VLXLT_HASH_KEY_TYPE_IVID_OVID:
            *key_type_value = TD2_VLXLT_HASH_KEY_TYPE_IVID_OVID;
            break;
        case VLXLT_HASH_KEY_TYPE_OTAG:
            *key_type_value = TD2_VLXLT_HASH_KEY_TYPE_OTAG;
            break;
        case VLXLT_HASH_KEY_TYPE_ITAG:
            *key_type_value = TD2_VLXLT_HASH_KEY_TYPE_ITAG;
            break;
        case VLXLT_HASH_KEY_TYPE_VLAN_MAC:
            *key_type_value = TD2_VLXLT_HASH_KEY_TYPE_VLAN_MAC;
            break;
        case VLXLT_HASH_KEY_TYPE_OVID:
            *key_type_value = TD2_VLXLT_HASH_KEY_TYPE_OVID;
            break;
        case VLXLT_HASH_KEY_TYPE_IVID:
            *key_type_value = TD2_VLXLT_HASH_KEY_TYPE_IVID;
            break;
        case VLXLT_HASH_KEY_TYPE_PRI_CFI:
            *key_type_value = TD2_VLXLT_HASH_KEY_TYPE_PRI_CFI;
            break;
        case VLXLT_HASH_KEY_TYPE_HPAE:
            *key_type_value = TD2_VLXLT_HASH_KEY_TYPE_HPAE;
            break;
        case VLXLT_HASH_KEY_TYPE_VIF:
            *key_type_value = TD2_VLXLT_HASH_KEY_TYPE_VIF;
            break;
        case VLXLT_HASH_KEY_TYPE_VIF_VLAN:
            *key_type_value = TD2_VLXLT_HASH_KEY_TYPE_VIF_VLAN;
            break;
        case VLXLT_HASH_KEY_TYPE_VIF_CVLAN:
            *key_type_value = TD2_VLXLT_HASH_KEY_TYPE_VIF_CVLAN;
            break;
        case VLXLT_HASH_KEY_TYPE_VIF_OTAG:
            *key_type_value = TD2_VLXLT_HASH_KEY_TYPE_VIF_OTAG;
            break;
        case VLXLT_HASH_KEY_TYPE_VIF_ITAG: 
            *key_type_value = TD2_VLXLT_HASH_KEY_TYPE_VIF_ITAG;
            break;
        case VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT:
            *key_type_value = TD2_VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT;
            break;
        case VLXLT_HASH_KEY_TYPE_L2GRE_DIP:
            *key_type_value = TD2_VLXLT_HASH_KEY_TYPE_L2GRE_DIP;
            break;
        case VLXLT_HASH_KEY_TYPE_IVID_OVID_VSAN:
            *key_type_value = TD2_VLXLT_HASH_KEY_TYPE_IVID_OVID_VSAN;
            break;
        case VLXLT_HASH_KEY_TYPE_IVID_VSAN:
            *key_type_value = TD2_VLXLT_HASH_KEY_TYPE_IVID_VSAN;
            break;
        case VLXLT_HASH_KEY_TYPE_OVID_VSAN:
            *key_type_value = TD2_VLXLT_HASH_KEY_TYPE_OVID_VSAN;
            break;
        case VLXLT_HASH_KEY_TYPE_VXLAN_DIP:
            *key_type_value = TD2_VLXLT_HASH_KEY_TYPE_VXLAN_DIP;
            break;
        default: 
            *key_type_value = 0;
            return BCM_E_UNAVAIL;
        }
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        switch (key_type) {
            case VLXLT_HASH_KEY_TYPE_IVID_OVID:
                *key_type_value = TR3_VLXLT_HASH_KEY_TYPE_IVID_OVID;
                break;
            case VLXLT_HASH_KEY_TYPE_OTAG:
                *key_type_value = TR3_VLXLT_HASH_KEY_TYPE_OTAG;
                break;
            case VLXLT_HASH_KEY_TYPE_ITAG:
                *key_type_value = TR3_VLXLT_HASH_KEY_TYPE_ITAG;
                break;
            case VLXLT_HASH_KEY_TYPE_VLAN_MAC:
                *key_type_value = TR3_VLXLT_HASH_KEY_TYPE_VLAN_MAC;
                break;
            case VLXLT_HASH_KEY_TYPE_OVID:
                *key_type_value = TR3_VLXLT_HASH_KEY_TYPE_OVID;
                break;
            case VLXLT_HASH_KEY_TYPE_IVID:
                *key_type_value = TR3_VLXLT_HASH_KEY_TYPE_IVID;
                break;
            case VLXLT_HASH_KEY_TYPE_PRI_CFI:
                *key_type_value = TR3_VLXLT_HASH_KEY_TYPE_PRI_CFI;
                break;
            case VLXLT_HASH_KEY_TYPE_HPAE:
                *key_type_value = TR3_VLXLT_HASH_KEY_TYPE_HPAE;
                break;
            case VLXLT_HASH_KEY_TYPE_VIF:
                *key_type_value = TR3_VLXLT_HASH_KEY_TYPE_VIF;
                break;
            case VLXLT_HASH_KEY_TYPE_VIF_VLAN:
                *key_type_value = TR3_VLXLT_HASH_KEY_TYPE_VIF_VLAN;
                break;
            case VLXLT_HASH_KEY_TYPE_VIF_CVLAN:
                *key_type_value = TR3_VLXLT_HASH_KEY_TYPE_VIF_CVLAN;
                break;
            case VLXLT_HASH_KEY_TYPE_VIF_OTAG:
                *key_type_value = TR3_VLXLT_HASH_KEY_TYPE_VIF_OTAG;
                break;
            case VLXLT_HASH_KEY_TYPE_VIF_ITAG: 
                *key_type_value = TR3_VLXLT_HASH_KEY_TYPE_VIF_ITAG;
                break;
            case VLXLT_HASH_KEY_TYPE_L2GRE_DIP:
                *key_type_value = TR3_VLXLT_HASH_KEY_TYPE_L2GRE_DIP;
                break;
            default: 
                *key_type_value = 0;
                return BCM_E_UNAVAIL;
        }
    } else
#endif
#if defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_KATANA2(unit)) {
        switch (key_type) {
        case VLXLT_HASH_KEY_TYPE_IVID_OVID:
            *key_type_value = KT2_VLXLT_HASH_KEY_TYPE_IVID_OVID;
            break;
        case VLXLT_HASH_KEY_TYPE_OTAG:
            *key_type_value = KT2_VLXLT_HASH_KEY_TYPE_OTAG;
            break;
        case VLXLT_HASH_KEY_TYPE_ITAG:
            *key_type_value = KT2_VLXLT_HASH_KEY_TYPE_ITAG;
            break;
        case VLXLT_HASH_KEY_TYPE_VLAN_MAC:
            *key_type_value = KT2_VLXLT_HASH_KEY_TYPE_VLAN_MAC;
            break;
        case VLXLT_HASH_KEY_TYPE_OVID:
            *key_type_value = KT2_VLXLT_HASH_KEY_TYPE_OVID;
            break;
        case VLXLT_HASH_KEY_TYPE_IVID:
            *key_type_value = KT2_VLXLT_HASH_KEY_TYPE_IVID;
            break;
        case VLXLT_HASH_KEY_TYPE_PRI_CFI:
            *key_type_value = KT2_VLXLT_HASH_KEY_TYPE_PRI_CFI;
            break;
        case VLXLT_HASH_KEY_TYPE_HPAE:
            *key_type_value = KT2_VLXLT_HASH_KEY_TYPE_HPAE;
            break;
        case VLXLT_HASH_KEY_TYPE_VIF:
            *key_type_value = KT2_VLXLT_HASH_KEY_TYPE_VIF;
            break;
        case VLXLT_HASH_KEY_TYPE_VIF_VLAN:
            *key_type_value = KT2_VLXLT_HASH_KEY_TYPE_VIF_VLAN;
            break;
        case VLXLT_HASH_KEY_TYPE_VIF_CVLAN:
            *key_type_value = KT2_VLXLT_HASH_KEY_TYPE_VIF_CVLAN;
            break;
        case VLXLT_HASH_KEY_TYPE_VIF_OTAG:
            *key_type_value = KT2_VLXLT_HASH_KEY_TYPE_VIF_OTAG;
            break;
        case VLXLT_HASH_KEY_TYPE_VIF_ITAG: 
            *key_type_value = KT2_VLXLT_HASH_KEY_TYPE_VIF_ITAG;
            break;
        case VLXLT_HASH_KEY_TYPE_LLVID:
            *key_type_value = KT2_VLXLT_HASH_KEY_TYPE_LLTAG_VID;
            break;
        case VLXLT_HASH_KEY_TYPE_LLVID_OVID:
            *key_type_value = KT2_VLXLT_HASH_KEY_TYPE_LLVID_OVID;
            break;
        case  VLXLT_HASK_KEY_TYPE_LLVID_IVID:
            *key_type_value = KT2_VLXLT_HASH_KEY_TYPE_LLVID_IVID;
            break;    
        default: 
            *key_type_value = 0;
            return BCM_E_UNAVAIL;
        }
    } else
#endif /* BCM_KATANA2_SUPPORT */
#if defined(BCM_HURRICANE2_SUPPORT)
    if (SOC_IS_HURRICANE2(unit)) {
        switch (key_type) {
            case VLXLT_HASH_KEY_TYPE_IVID_OVID:
                *key_type_value = HR2_VLXLT_HASH_KEY_TYPE_IVID_OVID;
                break;
            case VLXLT_HASH_KEY_TYPE_OTAG:
                *key_type_value = HR2_VLXLT_HASH_KEY_TYPE_OTAG;
                break;
            case VLXLT_HASH_KEY_TYPE_ITAG:
                *key_type_value = HR2_VLXLT_HASH_KEY_TYPE_ITAG;
                break;
            case VLXLT_HASH_KEY_TYPE_VLAN_MAC:
                *key_type_value = HR2_VLXLT_HASH_KEY_TYPE_VLAN_MAC;
                break;
            case VLXLT_HASH_KEY_TYPE_OVID:
                *key_type_value = HR2_VLXLT_HASH_KEY_TYPE_OVID;
                break;
            case VLXLT_HASH_KEY_TYPE_IVID:
                *key_type_value = HR2_VLXLT_HASH_KEY_TYPE_IVID;
                break;
            case VLXLT_HASH_KEY_TYPE_PRI_CFI:
                *key_type_value = HR2_VLXLT_HASH_KEY_TYPE_PRI_CFI;
                break;
            case VLXLT_HASH_KEY_TYPE_HPAE:
                *key_type_value = HR2_VLXLT_HASH_KEY_TYPE_HPAE;
                break;
            case VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID_OVID:
                *key_type_value = HR2_VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID_OVID;
                break;
            case VLXLT_HASH_KEY_TYPE_PAYLOAD_OTAG:
                *key_type_value = HR2_VLXLT_HASH_KEY_TYPE_PAYLOAD_OTAG;
                break;
            case VLXLT_HASH_KEY_TYPE_PAYLOAD_ITAG:
                *key_type_value = HR2_VLXLT_HASH_KEY_TYPE_PAYLOAD_ITAG;
                break;
            case VLXLT_HASH_KEY_TYPE_PAYLOAD_OVID:
                *key_type_value = HR2_VLXLT_HASH_KEY_TYPE_PAYLOAD_OVID;
                break;
            case VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID:
                *key_type_value = HR2_VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID;
                break;
            default:
                *key_type_value = 0;
                return BCM_E_UNAVAIL;
        }
    } else
#endif /* BCM_HURRICANE2_SUPPORT */

#if defined(BCM_HURRICANE3_SUPPORT)
    if (SOC_IS_HURRICANE3(unit)) {
        switch (key_type){
            case VLXLT_HASH_KEY_TYPE_IVID_OVID:
                *key_type_value = HR3_VLXLT_HASH_KEY_TYPE_IVID_OVID;
                break;
            case VLXLT_HASH_KEY_TYPE_OTAG:
                *key_type_value = HR3_VLXLT_HASH_KEY_TYPE_OTAG;
                break;
            case VLXLT_HASH_KEY_TYPE_ITAG:
                *key_type_value = HR3_VLXLT_HASH_KEY_TYPE_ITAG;
                break;
            case VLXLT_HASH_KEY_TYPE_VLAN_MAC:
                *key_type_value = HR3_VLXLT_HASH_KEY_TYPE_VLAN_MAC;
                break;
            case VLXLT_HASH_KEY_TYPE_OVID:
                *key_type_value = HR3_VLXLT_HASH_KEY_TYPE_OVID;
                break;
            case VLXLT_HASH_KEY_TYPE_IVID:
                *key_type_value = HR3_VLXLT_HASH_KEY_TYPE_IVID;
                break;
            case VLXLT_HASH_KEY_TYPE_PRI_CFI:
                *key_type_value = HR3_VLXLT_HASH_KEY_TYPE_PRI_CFI;
                break;
            case VLXLT_HASH_KEY_TYPE_HPAE:
                *key_type_value = HR3_VLXLT_HASH_KEY_TYPE_HPAE;
                break;
            case VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID_OVID:
                *key_type_value = HR3_VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID_OVID;
                break;
            case VLXLT_HASH_KEY_TYPE_PAYLOAD_OTAG:
                *key_type_value = HR3_VLXLT_HASH_KEY_TYPE_PAYLOAD_OTAG;
                break;
            case VLXLT_HASH_KEY_TYPE_PAYLOAD_ITAG:
                *key_type_value = HR3_VLXLT_HASH_KEY_TYPE_PAYLOAD_ITAG;
                break;
            case VLXLT_HASH_KEY_TYPE_PAYLOAD_OVID:
                *key_type_value = HR3_VLXLT_HASH_KEY_TYPE_PAYLOAD_OVID;
                break;
            case VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID:
                *key_type_value = HR3_VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID;
                break;
            default: 
                *key_type_value = 0;
                return BCM_E_UNAVAIL;
        }
    } else
#endif /* BCM_HURRICANE3_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        switch (key_type){
            case VLXLT_HASH_KEY_TYPE_IVID_OVID:
                *key_type_value = GH2_VLXLT_HASH_KEY_TYPE_IVID_OVID;
                break;
            case VLXLT_HASH_KEY_TYPE_OTAG:
                *key_type_value = GH2_VLXLT_HASH_KEY_TYPE_OTAG;
                break;
            case VLXLT_HASH_KEY_TYPE_ITAG:
                *key_type_value = GH2_VLXLT_HASH_KEY_TYPE_ITAG;
                break;
            case VLXLT_HASH_KEY_TYPE_VLAN_MAC:
                *key_type_value = GH2_VLXLT_HASH_KEY_TYPE_VLAN_MAC;
                break;
            case VLXLT_HASH_KEY_TYPE_OVID:
                *key_type_value = GH2_VLXLT_HASH_KEY_TYPE_OVID;
                break;
            case VLXLT_HASH_KEY_TYPE_IVID:
                *key_type_value = GH2_VLXLT_HASH_KEY_TYPE_IVID;
                break;
            case VLXLT_HASH_KEY_TYPE_PRI_CFI:
                *key_type_value = GH2_VLXLT_HASH_KEY_TYPE_PRI_CFI;
                break;
            case VLXLT_HASH_KEY_TYPE_HPAE:
                *key_type_value = GH2_VLXLT_HASH_KEY_TYPE_HPAE;
                break;
            case VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID_OVID:
                *key_type_value = GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID_OVID;
                break;
            case VLXLT_HASH_KEY_TYPE_PAYLOAD_OTAG:
                *key_type_value = GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_OTAG;
                break;
            case VLXLT_HASH_KEY_TYPE_PAYLOAD_ITAG:
                *key_type_value = GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_ITAG;
                break;
            case VLXLT_HASH_KEY_TYPE_PAYLOAD_OVID:
                *key_type_value = GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_OVID;
                break;
            case VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID:
                *key_type_value = GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID;
                break;
            case VLXLT_HASH_KEY_TYPE_VNID:
                *key_type_value = GH2_VLXLT_HASH_KEY_TYPE_VNID;
                break;
            default:
                *key_type_value = 0;
                return BCM_E_UNAVAIL;
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        switch (key_type) {
            case VLXLT_HASH_KEY_TYPE_IVID_OVID:
                *key_type_value = TR_VLXLT_HASH_KEY_TYPE_IVID_OVID;
                break;
            case VLXLT_HASH_KEY_TYPE_OTAG:
                *key_type_value = TR_VLXLT_HASH_KEY_TYPE_OTAG;
                break;
            case VLXLT_HASH_KEY_TYPE_ITAG:
                *key_type_value = TR_VLXLT_HASH_KEY_TYPE_ITAG;
                break;
            case VLXLT_HASH_KEY_TYPE_VLAN_MAC:
                *key_type_value = TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC;
                break;
            case VLXLT_HASH_KEY_TYPE_OVID:
                *key_type_value = TR_VLXLT_HASH_KEY_TYPE_OVID;
                break;
            case VLXLT_HASH_KEY_TYPE_IVID:
                *key_type_value = TR_VLXLT_HASH_KEY_TYPE_IVID;
                break;
            case VLXLT_HASH_KEY_TYPE_PRI_CFI:
                *key_type_value = TR_VLXLT_HASH_KEY_TYPE_PRI_CFI;
                break;
            case VLXLT_HASH_KEY_TYPE_HPAE:
                *key_type_value = TR_VLXLT_HASH_KEY_TYPE_HPAE;
                break;
            case VLXLT_HASH_KEY_TYPE_VIF:
                *key_type_value = TR_VLXLT_HASH_KEY_TYPE_VIF;
                break;
            case VLXLT_HASH_KEY_TYPE_VIF_VLAN:
                *key_type_value = TR_VLXLT_HASH_KEY_TYPE_VIF_VLAN;
                break;
            case VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT:
                *key_type_value = TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT;
                break;
            default: 
                *key_type_value = 0;
                return BCM_E_UNAVAIL;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *   _bcm_esw_vlan_xlate_key_type_get
 * Purpose:
 *   get the common vlan translation key type from hardware specific 
 *   vlan translation key type value for VLAN_XLATE table
 *
 * Parameters:
 *    key_type_value - input, hardware specific key type value
 *    key_type -       output, common key type
 */
int
_bcm_esw_vlan_xlate_key_type_get(int unit, 
                     int key_type_value, int *key_type)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {
        switch (key_type_value) {
        case TD2_VLXLT_HASH_KEY_TYPE_IVID_OVID:
            *key_type = VLXLT_HASH_KEY_TYPE_IVID_OVID;
            break;
        case TD2_VLXLT_HASH_KEY_TYPE_OTAG:
            *key_type = VLXLT_HASH_KEY_TYPE_OTAG;
            break;
        case TD2_VLXLT_HASH_KEY_TYPE_ITAG:
            *key_type = VLXLT_HASH_KEY_TYPE_ITAG;
            break;
        case TD2_VLXLT_HASH_KEY_TYPE_VLAN_MAC:
            *key_type = VLXLT_HASH_KEY_TYPE_VLAN_MAC;
            break;
        case TD2_VLXLT_HASH_KEY_TYPE_OVID:
            *key_type = VLXLT_HASH_KEY_TYPE_OVID;
            break;
        case TD2_VLXLT_HASH_KEY_TYPE_IVID:
            *key_type = VLXLT_HASH_KEY_TYPE_IVID;
            break;
        case TD2_VLXLT_HASH_KEY_TYPE_PRI_CFI:
            *key_type = VLXLT_HASH_KEY_TYPE_PRI_CFI;
            break;
        case TD2_VLXLT_HASH_KEY_TYPE_HPAE:
            *key_type = VLXLT_HASH_KEY_TYPE_HPAE;
            break;
        case TD2_VLXLT_HASH_KEY_TYPE_VIF:
            *key_type = VLXLT_HASH_KEY_TYPE_VIF;
            break;
        case TD2_VLXLT_HASH_KEY_TYPE_VIF_VLAN:
            *key_type = VLXLT_HASH_KEY_TYPE_VIF_VLAN;
            break;
        case TD2_VLXLT_HASH_KEY_TYPE_VIF_CVLAN:
            *key_type = VLXLT_HASH_KEY_TYPE_VIF_CVLAN;
            break;
        case TD2_VLXLT_HASH_KEY_TYPE_VIF_OTAG:
            *key_type = VLXLT_HASH_KEY_TYPE_VIF_OTAG;
            break;
        case TD2_VLXLT_HASH_KEY_TYPE_VIF_ITAG: 
            *key_type = VLXLT_HASH_KEY_TYPE_VIF_ITAG;
            break;
        case TD2_VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT:
            *key_type = VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT;
            break;
        case TD2_VLXLT_HASH_KEY_TYPE_L2GRE_DIP:
            *key_type = VLXLT_HASH_KEY_TYPE_L2GRE_DIP;
            break;
        case TD2_VLXLT_HASH_KEY_TYPE_IVID_OVID_VSAN:
            *key_type = VLXLT_HASH_KEY_TYPE_IVID_OVID_VSAN;
            break;
        case TD2_VLXLT_HASH_KEY_TYPE_IVID_VSAN:
            *key_type = VLXLT_HASH_KEY_TYPE_IVID_VSAN;
            break;
        case TD2_VLXLT_HASH_KEY_TYPE_OVID_VSAN:
            *key_type = VLXLT_HASH_KEY_TYPE_OVID_VSAN;
            break;
        case TD2_VLXLT_HASH_KEY_TYPE_VXLAN_DIP:
            *key_type = VLXLT_HASH_KEY_TYPE_VXLAN_DIP;
            break;
        default: 
            *key_type = 0;
            return BCM_E_UNAVAIL;
        }
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        switch (key_type_value) {
            case TR3_VLXLT_HASH_KEY_TYPE_IVID_OVID:
                *key_type = VLXLT_HASH_KEY_TYPE_IVID_OVID;
                break;
            case TR3_VLXLT_HASH_KEY_TYPE_OTAG:
                *key_type = VLXLT_HASH_KEY_TYPE_OTAG;
                break;
            case TR3_VLXLT_HASH_KEY_TYPE_ITAG:
                *key_type = VLXLT_HASH_KEY_TYPE_ITAG;
                break;
            case TR3_VLXLT_HASH_KEY_TYPE_VLAN_MAC:
                *key_type = VLXLT_HASH_KEY_TYPE_VLAN_MAC;
                break;
            case TR3_VLXLT_HASH_KEY_TYPE_OVID:
                *key_type = VLXLT_HASH_KEY_TYPE_OVID;
                break;
            case TR3_VLXLT_HASH_KEY_TYPE_IVID:
                *key_type = VLXLT_HASH_KEY_TYPE_IVID;
                break;
            case TR3_VLXLT_HASH_KEY_TYPE_PRI_CFI:
                *key_type = VLXLT_HASH_KEY_TYPE_PRI_CFI;
                break;
            case TR3_VLXLT_HASH_KEY_TYPE_HPAE:
                *key_type = VLXLT_HASH_KEY_TYPE_HPAE;
                break;
            case TR3_VLXLT_HASH_KEY_TYPE_VIF:
                *key_type = VLXLT_HASH_KEY_TYPE_VIF;
                break;
            case TR3_VLXLT_HASH_KEY_TYPE_VIF_VLAN:
                *key_type = VLXLT_HASH_KEY_TYPE_VIF_VLAN;
                break;
            case TR3_VLXLT_HASH_KEY_TYPE_VIF_CVLAN:
                *key_type = VLXLT_HASH_KEY_TYPE_VIF_CVLAN;
                break;
            case TR3_VLXLT_HASH_KEY_TYPE_VIF_OTAG:
                *key_type = VLXLT_HASH_KEY_TYPE_VIF_OTAG;
                break;
            case TR3_VLXLT_HASH_KEY_TYPE_VIF_ITAG: 
                *key_type = VLXLT_HASH_KEY_TYPE_VIF_ITAG;
                break;
            case TR3_VLXLT_HASH_KEY_TYPE_L2GRE_DIP:
                *key_type = TR3_VLXLT_HASH_KEY_TYPE_L2GRE_DIP;
                break;
            default: 
                *key_type = 0;
                return BCM_E_UNAVAIL;
        }
    } else
#endif
#if defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_KATANA2(unit)) {
        switch (key_type_value) {
        case KT2_VLXLT_HASH_KEY_TYPE_IVID_OVID:
            *key_type = VLXLT_HASH_KEY_TYPE_IVID_OVID;
            break;
        case KT2_VLXLT_HASH_KEY_TYPE_OTAG:
            *key_type = VLXLT_HASH_KEY_TYPE_OTAG;
            break;
        case KT2_VLXLT_HASH_KEY_TYPE_ITAG:
            *key_type = VLXLT_HASH_KEY_TYPE_ITAG;
            break;
        case KT2_VLXLT_HASH_KEY_TYPE_VLAN_MAC:
            *key_type = VLXLT_HASH_KEY_TYPE_VLAN_MAC;
            break;
        case KT2_VLXLT_HASH_KEY_TYPE_OVID:
            *key_type = VLXLT_HASH_KEY_TYPE_OVID;
            break;
        case KT2_VLXLT_HASH_KEY_TYPE_IVID:
            *key_type = VLXLT_HASH_KEY_TYPE_IVID;
            break;
        case KT2_VLXLT_HASH_KEY_TYPE_PRI_CFI:
            *key_type = VLXLT_HASH_KEY_TYPE_PRI_CFI;
            break;
        case KT2_VLXLT_HASH_KEY_TYPE_HPAE:
            *key_type = VLXLT_HASH_KEY_TYPE_HPAE;
            break;
        case KT2_VLXLT_HASH_KEY_TYPE_VIF:
            *key_type = VLXLT_HASH_KEY_TYPE_VIF;
            break;
        case KT2_VLXLT_HASH_KEY_TYPE_VIF_VLAN:
            *key_type = VLXLT_HASH_KEY_TYPE_VIF_VLAN;
            break;
        case KT2_VLXLT_HASH_KEY_TYPE_VIF_CVLAN:
            *key_type = VLXLT_HASH_KEY_TYPE_VIF_CVLAN;
            break;
        case KT2_VLXLT_HASH_KEY_TYPE_VIF_OTAG:
            *key_type = VLXLT_HASH_KEY_TYPE_VIF_OTAG;
            break;
        case KT2_VLXLT_HASH_KEY_TYPE_VIF_ITAG: 
            *key_type = VLXLT_HASH_KEY_TYPE_VIF_ITAG;
            break;
        case KT2_VLXLT_HASH_KEY_TYPE_LLTAG_VID:
            *key_type = VLXLT_HASH_KEY_TYPE_LLVID;
            break;
        case KT2_VLXLT_HASH_KEY_TYPE_LLVID_IVID:
            *key_type = VLXLT_HASK_KEY_TYPE_LLVID_IVID;
            break;
        case KT2_VLXLT_HASH_KEY_TYPE_LLVID_OVID:
            *key_type = VLXLT_HASH_KEY_TYPE_LLVID_OVID;
            break;
        default: 
            *key_type = 0;
            return BCM_E_UNAVAIL;
        }
    } else
#endif /* BCM_KATANA2_SUPPORT */
#if defined(BCM_HURRICANE2_SUPPORT)
    if (SOC_IS_HURRICANE2(unit)) {
        switch (key_type_value) {
            case HR2_VLXLT_HASH_KEY_TYPE_IVID_OVID:
                *key_type = VLXLT_HASH_KEY_TYPE_IVID_OVID;
                break;
            case HR2_VLXLT_HASH_KEY_TYPE_OTAG:
                *key_type = VLXLT_HASH_KEY_TYPE_OTAG;
                break;
            case HR2_VLXLT_HASH_KEY_TYPE_ITAG:
                *key_type = VLXLT_HASH_KEY_TYPE_ITAG;
                break;
            case HR2_VLXLT_HASH_KEY_TYPE_VLAN_MAC:
                *key_type = VLXLT_HASH_KEY_TYPE_VLAN_MAC;
                break;
            case HR2_VLXLT_HASH_KEY_TYPE_OVID:
                *key_type = VLXLT_HASH_KEY_TYPE_OVID;
                break;
            case HR2_VLXLT_HASH_KEY_TYPE_IVID:
                *key_type = VLXLT_HASH_KEY_TYPE_IVID;
                break;
            case HR2_VLXLT_HASH_KEY_TYPE_PRI_CFI:
                *key_type = VLXLT_HASH_KEY_TYPE_PRI_CFI;
                break;
            case HR2_VLXLT_HASH_KEY_TYPE_HPAE:
                *key_type = VLXLT_HASH_KEY_TYPE_HPAE;
                break;
            case HR2_VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID_OVID:
                *key_type = VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID_OVID;
                break;
            case HR2_VLXLT_HASH_KEY_TYPE_PAYLOAD_OTAG:
                *key_type = VLXLT_HASH_KEY_TYPE_PAYLOAD_OTAG;
                break;
            case HR2_VLXLT_HASH_KEY_TYPE_PAYLOAD_ITAG:
                *key_type = VLXLT_HASH_KEY_TYPE_PAYLOAD_ITAG;
                break;
            case HR2_VLXLT_HASH_KEY_TYPE_PAYLOAD_OVID:
                *key_type= VLXLT_HASH_KEY_TYPE_PAYLOAD_OVID;
                break;
            case HR2_VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID:
                *key_type = VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID;
                break;
            default:
                *key_type = 0;
                return BCM_E_UNAVAIL;
        }
    } else
#endif /* BCM_HURRICANE2_SUPPORT */
#if defined(BCM_HURRICANE3_SUPPORT)
    if (SOC_IS_HURRICANE3(unit)) {
        switch (key_type_value){
            case HR3_VLXLT_HASH_KEY_TYPE_IVID_OVID:
                *key_type = VLXLT_HASH_KEY_TYPE_IVID_OVID;
                break;
            case HR3_VLXLT_HASH_KEY_TYPE_OTAG:
                *key_type = VLXLT_HASH_KEY_TYPE_OTAG;
                break;
            case HR3_VLXLT_HASH_KEY_TYPE_ITAG:
                *key_type = VLXLT_HASH_KEY_TYPE_ITAG;
                break;
            case HR3_VLXLT_HASH_KEY_TYPE_VLAN_MAC:
                *key_type = VLXLT_HASH_KEY_TYPE_VLAN_MAC;
                break;
            case HR3_VLXLT_HASH_KEY_TYPE_OVID:
                *key_type = VLXLT_HASH_KEY_TYPE_OVID;
                break;
            case HR3_VLXLT_HASH_KEY_TYPE_IVID:
                *key_type = VLXLT_HASH_KEY_TYPE_IVID;
                break;
            case HR3_VLXLT_HASH_KEY_TYPE_PRI_CFI:
                *key_type = VLXLT_HASH_KEY_TYPE_PRI_CFI;
                break;
            case HR3_VLXLT_HASH_KEY_TYPE_HPAE:
                *key_type = VLXLT_HASH_KEY_TYPE_HPAE;
                break;
            case HR3_VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID_OVID:
                *key_type = VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID_OVID;
                break;
            case HR3_VLXLT_HASH_KEY_TYPE_PAYLOAD_OTAG:
                *key_type = VLXLT_HASH_KEY_TYPE_PAYLOAD_OTAG;
                break;
            case HR3_VLXLT_HASH_KEY_TYPE_PAYLOAD_ITAG:
                *key_type = VLXLT_HASH_KEY_TYPE_PAYLOAD_ITAG;
                break;
            case HR3_VLXLT_HASH_KEY_TYPE_PAYLOAD_OVID:
                *key_type= VLXLT_HASH_KEY_TYPE_PAYLOAD_OVID;
                break;
            case HR3_VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID:
                *key_type = VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID;
                break;
            default:
                *key_type = 0;
                return BCM_E_UNAVAIL;
        }
    } else
#endif /* BCM_HURRICANE3_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        switch (key_type_value){
            case GH2_VLXLT_HASH_KEY_TYPE_IVID_OVID:
                *key_type = VLXLT_HASH_KEY_TYPE_IVID_OVID;
                break;
            case GH2_VLXLT_HASH_KEY_TYPE_OTAG:
                *key_type = VLXLT_HASH_KEY_TYPE_OTAG;
                break;
            case GH2_VLXLT_HASH_KEY_TYPE_ITAG:
                *key_type = VLXLT_HASH_KEY_TYPE_ITAG;
                break;
            case GH2_VLXLT_HASH_KEY_TYPE_VLAN_MAC:
                *key_type = VLXLT_HASH_KEY_TYPE_VLAN_MAC;
                break;
            case GH2_VLXLT_HASH_KEY_TYPE_OVID:
                *key_type = VLXLT_HASH_KEY_TYPE_OVID;
                break;
            case GH2_VLXLT_HASH_KEY_TYPE_IVID:
                *key_type = VLXLT_HASH_KEY_TYPE_IVID;
                break;
            case GH2_VLXLT_HASH_KEY_TYPE_PRI_CFI:
                *key_type = VLXLT_HASH_KEY_TYPE_PRI_CFI;
                break;
            case GH2_VLXLT_HASH_KEY_TYPE_HPAE:
                *key_type = VLXLT_HASH_KEY_TYPE_HPAE;
                break;
            case GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID_OVID:
                *key_type = VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID_OVID;
                break;
            case GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_OTAG:
                *key_type = VLXLT_HASH_KEY_TYPE_PAYLOAD_OTAG;
                break;
            case GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_ITAG:
                *key_type = VLXLT_HASH_KEY_TYPE_PAYLOAD_ITAG;
                break;
            case GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_OVID:
                *key_type= VLXLT_HASH_KEY_TYPE_PAYLOAD_OVID;
                break;
            case GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID:
                *key_type = VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID;
                break;
            case GH2_VLXLT_HASH_KEY_TYPE_VNID:
                *key_type = VLXLT_HASH_KEY_TYPE_VNID;
                break;
            default:
                *key_type = 0;
                return BCM_E_UNAVAIL;
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        switch (key_type_value) {
            case TR_VLXLT_HASH_KEY_TYPE_IVID_OVID:
                *key_type = VLXLT_HASH_KEY_TYPE_IVID_OVID;
                break;
            case TR_VLXLT_HASH_KEY_TYPE_OTAG:
                *key_type = VLXLT_HASH_KEY_TYPE_OTAG;
                break;
            case TR_VLXLT_HASH_KEY_TYPE_ITAG:
                *key_type = VLXLT_HASH_KEY_TYPE_ITAG;
                break;
            case TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC:
                *key_type = VLXLT_HASH_KEY_TYPE_VLAN_MAC;
                break;
            case TR_VLXLT_HASH_KEY_TYPE_OVID:
                *key_type = VLXLT_HASH_KEY_TYPE_OVID;
                break;
            case TR_VLXLT_HASH_KEY_TYPE_IVID:
                *key_type = VLXLT_HASH_KEY_TYPE_IVID;
                break;
            case TR_VLXLT_HASH_KEY_TYPE_PRI_CFI:
                *key_type = VLXLT_HASH_KEY_TYPE_PRI_CFI;
                break;
            case TR_VLXLT_HASH_KEY_TYPE_HPAE:
                *key_type = VLXLT_HASH_KEY_TYPE_HPAE;
                break;
            case TR_VLXLT_HASH_KEY_TYPE_VIF:
                *key_type = VLXLT_HASH_KEY_TYPE_VIF;
                break;
            case TR_VLXLT_HASH_KEY_TYPE_VIF_VLAN:
                *key_type = VLXLT_HASH_KEY_TYPE_VIF_VLAN;
                break;
            case TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT:
                *key_type = VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT;
                break;
            default: 
                *key_type = 0;
                return BCM_E_UNAVAIL;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *   _bcm_esw_pt_vtkey_type_value_get
 * Purpose:
 * get the hardware specific port table vlan translation key type value
 * from the common vlan translation key type for PORT_TABLE VT_KEY_TYPE
 * field. In Triumph3, key type value in VLAN_XLATE differs from 
 * PORT_TABLE's VT_KEY_TYPE(_2) field
 *
 * Parameters:
 *    key_type -       input, common key type
 *    key_type_value - output, hardware specific PORT_TABLE key type value
 */
int
_bcm_esw_pt_vtkey_type_value_get(int unit, 
                     int key_type, int *key_type_value)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        switch (key_type) {
            case VLXLT_HASH_KEY_TYPE_IVID_OVID:
                *key_type_value = TR3_PT_XLT_KEY_TYPE_IVID_OVID;
                break;
            case VLXLT_HASH_KEY_TYPE_OTAG:
                *key_type_value = TR3_PT_XLT_KEY_TYPE_OTAG;
                break;
            case VLXLT_HASH_KEY_TYPE_ITAG:
                *key_type_value = TR3_PT_XLT_KEY_TYPE_ITAG;
                break;
            case VLXLT_HASH_KEY_TYPE_VLAN_MAC:
                *key_type_value = TR3_PT_XLT_KEY_TYPE_VLAN_MAC;
                break;
            case VLXLT_HASH_KEY_TYPE_OVID:
                *key_type_value = TR3_PT_XLT_KEY_TYPE_OVID;
                break;
            case VLXLT_HASH_KEY_TYPE_IVID:
                *key_type_value = TR3_PT_XLT_KEY_TYPE_IVID;
                break;
            case VLXLT_HASH_KEY_TYPE_PRI_CFI:
                *key_type_value = TR3_PT_XLT_KEY_TYPE_PRI_CFI;
                break;
            case VLXLT_HASH_KEY_TYPE_HPAE:
                *key_type_value = TR3_PT_XLT_KEY_TYPE_HPAE;
                break;
            case VLXLT_HASH_KEY_TYPE_VIF:
                *key_type_value = TR3_PT_XLT_KEY_TYPE_VIF;
                break;
            case VLXLT_HASH_KEY_TYPE_VIF_VLAN:
                *key_type_value = TR3_PT_XLT_KEY_TYPE_VIF_VLAN;
                break;
            case VLXLT_HASH_KEY_TYPE_VIF_CVLAN:
                *key_type_value = TR3_PT_XLT_KEY_TYPE_VIF_CVLAN;
                break;
            case VLXLT_HASH_KEY_TYPE_VIF_OTAG:
                *key_type_value = TR3_PT_XLT_KEY_TYPE_VIF_OTAG;
                break;
            case VLXLT_HASH_KEY_TYPE_VIF_ITAG: 
                *key_type_value = TR3_PT_XLT_KEY_TYPE_VIF_ITAG;
                break;
            default: 
                *key_type_value = 0;
                return BCM_E_UNAVAIL;
        }
    } else
#endif
    {
        /* for other switches, PORT_TABLE key type value is same as the
         * VLAN_XLATE key type value.
         */
        return _bcm_esw_vlan_xlate_key_type_value_get(unit, key_type,
                                                      key_type_value);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *   _bcm_esw_pt_vtkey_type_get
 * Purpose:
 * get the common vlan translation key type from hardware specific 
 * PORT_TABLE vlan translation key type value
 *
 * Parameters:
 *    key_type_value - input, common key type
 *    key_type - output, hardware specific PORT_TABLE key type value
 */
int
_bcm_esw_pt_vtkey_type_get(int unit, 
                     int key_type_value, int *key_type)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        switch (key_type_value) {
            case TR3_PT_XLT_KEY_TYPE_IVID_OVID:
                *key_type = VLXLT_HASH_KEY_TYPE_IVID_OVID;
                break;
            case TR3_PT_XLT_KEY_TYPE_OTAG:
                *key_type = VLXLT_HASH_KEY_TYPE_OTAG;
                break;
            case TR3_PT_XLT_KEY_TYPE_ITAG:
                *key_type = VLXLT_HASH_KEY_TYPE_ITAG;
                break;
            case TR3_PT_XLT_KEY_TYPE_VLAN_MAC:
                *key_type = VLXLT_HASH_KEY_TYPE_VLAN_MAC;
                break;
            case TR3_PT_XLT_KEY_TYPE_OVID:
                *key_type = VLXLT_HASH_KEY_TYPE_OVID;
                break;
            case TR3_PT_XLT_KEY_TYPE_IVID:
                *key_type = VLXLT_HASH_KEY_TYPE_IVID;
                break;
            case TR3_PT_XLT_KEY_TYPE_PRI_CFI:
                *key_type = VLXLT_HASH_KEY_TYPE_PRI_CFI;
                break;
            case TR3_PT_XLT_KEY_TYPE_HPAE:
                *key_type = VLXLT_HASH_KEY_TYPE_HPAE;
                break;
            case TR3_PT_XLT_KEY_TYPE_VIF:
                *key_type = VLXLT_HASH_KEY_TYPE_VIF;
                break;
            case TR3_PT_XLT_KEY_TYPE_VIF_VLAN:
                *key_type = VLXLT_HASH_KEY_TYPE_VIF_VLAN;
                break;
            case TR3_PT_XLT_KEY_TYPE_VIF_CVLAN:
                *key_type = VLXLT_HASH_KEY_TYPE_VIF_CVLAN;
                break;
            case TR3_PT_XLT_KEY_TYPE_VIF_OTAG:
                *key_type = VLXLT_HASH_KEY_TYPE_VIF_OTAG;
                break;
            case TR3_PT_XLT_KEY_TYPE_VIF_ITAG: 
                *key_type = VLXLT_HASH_KEY_TYPE_VIF_ITAG;
                break;
            default: 
                *key_type = 0;
                return BCM_E_UNAVAIL;
        }
    } else
#endif
    {
        /* For other devices, key type in VLAN_XLATE is same as in PORT_TABLE*/
        return _bcm_esw_vlan_xlate_key_type_get(unit, key_type_value,
                                                key_type);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *   _bcm_esw_egr_vtkey_type_value_get
 * Purpose:
 * get the hardware specific EGR_VLAN_CONTROL_1 egress vlan
 * translation key type value from the common key type
 *
 * Parameters:
 *    key_type -       input, common key type
 *    key_type_value - output, hardware specific
 *                     EGR_VLAN_CONTROL_1 table VT_ENABLE/VT2_ENABLE fields
 */
int
_bcm_esw_egr_vtkey_type_value_get(int unit,
                     int key_type, int *key_type_value)
{
    switch (key_type) {
        case bcmVlanTranslateEgressKeyPortGroupDouble:
            /* For devices where feature 'egr_vlan_xlate_second_lookup' is enabled,
             * VT_ENABLEf is a 2-bit field and explicit second lookup is allowed.
             * This key_type is an alias of the existing key_type
             * bcmVlanTranslateEgressKeyVlanPort in term of functionality,
             * on these devices.*/
            if (!(SOC_IS_APACHE(unit) &&
                soc_feature(unit, soc_feature_egr_vlan_xlate_second_lookup))) {
                return BCM_E_UNAVAIL;
            }
            /* passthru */
            /* coverity[fallthrough: FALSE] */
        case bcmVlanTranslateEgressKeyVlanPort:
            /* Note: This is the default key type for first lookup */
            *key_type_value = EVXLT_KEY_TYPE_PORT_GROUP_VLAN_DOUBLE;
            break;
        case bcmVlanTranslateEgressKeyPortDouble:
            if (soc_feature(unit, soc_feature_egr_vlan_xlate_second_lookup)) {
                *key_type_value = EVXLT_KEY_TYPE_PORT_VLAN_DOUBLE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmVlanTranslateEgressKeyInvalid:
            *key_type_value = EVXLT_KEY_TYPE_NOOP;
            break;
        default:
            *key_type_value = 0;
            return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *   _bcm_esw_dvp_vtkey_type_value_get
 * Purpose:
 * get the hardware specific EGR_DVP_ATTRIBUTE egress vlan
 * translation key type value from the common key type
 *
 * Parameters:
 *    key_type -       input, common key type
 *    key_type_value - output, hardware specific EGR_DVP_ATTRIBUTE key type value
 */
int
_bcm_esw_dvp_vtkey_type_value_get(int unit, 
                     int key_type, int *key_type_value)
{
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_egr_vlan_xlate_key_on_dvp)) {
        if (key_type_value == NULL) {
            return BCM_E_PARAM;
        }
        switch (key_type) {
            case bcmVlanTranslateEgressKeyVpn:
                *key_type_value = TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_VFI;
                break;
            case bcmVlanTranslateEgressKeyVpnGport:
                *key_type_value = TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_DVP_VFI;
                break;
            case bcmVlanTranslateEgressKeyVpnGportGroup:
                *key_type_value = TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_GID_VFI;
                break;
            case bcmVlanTranslateEgressKeyVlanPort:
                *key_type_value = TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_PORT_UL_VLAN;
                break;
            case bcmVlanTranslateEgressKeyVlanGport:
                *key_type_value = TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_DVP_OL_VLAN;
                break;
            case bcmVlanTranslateEgressKeyInvalid:
                *key_type_value = TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_NOOP;
                break;
            default: 
                *key_type_value = 0;
                return BCM_E_PARAM;
        }
        return BCM_E_NONE;
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *   _bcm_esw_egr_vtkey_type_get
 * Purpose:
 * Get the common vlan translation key type from the hardware specific
 * EGR_VLAN_CONTROL_1 vlan translation key type value VT_ENABLE/VT2_ENABLE
 *
 * Parameters:
 *    key_type_value - input, common key type
 *    key_type - output, hardware specific EGR_VLAN_CONTROL_1 key type value
 */
int
_bcm_esw_egr_vtkey_type_get(int unit,
                     int key_type_value, int *key_type)
{
        if (key_type == NULL) {
            return BCM_E_PARAM;
        }
        switch (key_type_value) {
            case EVXLT_KEY_TYPE_PORT_GROUP_VLAN_DOUBLE:
                /* For devices where feature 'egr_vlan_xlate_second_lookup' is
                 * enabled, VT_ENABLEf is a 2-bit field and explicit second
                 * lookup is allowed. This key_type_value maps to both key_types.
                 * Returning bcmVlanTranslateEgressKeyPortGroupDouble for those
                 * devices where soc feature is enabled as it clears the
                 * ambiguity in the naming of existing enums. Otherwise,
                 * bcmVlanTranslateEgressKeyVlanPort is returned as before */
                if (SOC_IS_APACHE(unit) &&
                    soc_feature(unit, soc_feature_egr_vlan_xlate_second_lookup)) {
                    *key_type = bcmVlanTranslateEgressKeyPortGroupDouble;
                } else {
                    *key_type = bcmVlanTranslateEgressKeyVlanPort;
                }
                break;
            case EVXLT_KEY_TYPE_PORT_VLAN_DOUBLE:
                *key_type = bcmVlanTranslateEgressKeyPortDouble;
                break;
            case EVXLT_KEY_TYPE_NOOP:
                *key_type = bcmVlanTranslateEgressKeyInvalid;
                break;
            default:
                *key_type = 0;
                return BCM_E_PARAM;
        }
        return BCM_E_NONE;
}

/*
 * Function:
 *   _bcm_esw_dvp_vtkey_type_get
 * Purpose:
 * get the common vlan translation key type from the hardware specific 
 * EGR_DVP_ATTRIBUTE vlan translation key type value 
 *
 * Parameters:
 *    key_type_value - input, common key type
 *    key_type - output, hardware specific EGR_DVP_ATTRIBUTE key type value
 */
int
_bcm_esw_dvp_vtkey_type_get(int unit, 
                     int key_type_value, int *key_type)
{
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_egr_vlan_xlate_key_on_dvp)) {
        if (key_type == NULL) {
            return BCM_E_PARAM;
        }
        switch (key_type_value) {
            case TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_VFI:
                *key_type = bcmVlanTranslateEgressKeyVpn;
                break;
            case TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_DVP_VFI:
                *key_type = bcmVlanTranslateEgressKeyVpnGport;
                break;
            case TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_GID_VFI:
                *key_type = bcmVlanTranslateEgressKeyVpnGportGroup;
                break;
            case TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_NOOP:
                *key_type = bcmVlanTranslateEgressKeyInvalid;
                break;
            case TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_PORT_UL_VLAN:
                *key_type = bcmVlanTranslateEgressKeyVlanPort;
                break;
            case TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_DVP_OL_VLAN:
                *key_type = bcmVlanTranslateEgressKeyVlanGport;
                break;
            default: 
                *key_type = 0;
                return BCM_E_PARAM;
        }
        return BCM_E_NONE;
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * MACRO:
 *      _BCM_VBMP_INSERT
 * Purpose:
 *      Add a VLAN ID to a vbmap
 */
#define _BCM_VBMP_INSERT(_bmp_, _vid_)    SHR_BITSET(((_bmp_).w), (_vid_))

/*
 * Macro:
 *      _BCM_VBMP_REMOVE
 * Purpose:
 *      Delete a VLAN ID from a vbmp
 */
#define _BCM_VBMP_REMOVE(_bmp_, _vid_)    SHR_BITCLR(((_bmp_).w), (_vid_))

/*
 * Macro :
 *      _BCM_VBMP_LOOKUP
 * Purpose:
 *      Return TRUE if a VLAN ID exists in a vbmp, FALSE otherwise
 */
#define _BCM_VBMP_LOOKUP(_bmp_, _vid_)    SHR_BITGET(((_bmp_).w), (_vid_))

/*
 * Function:
 *      _bcm_vbmp_destroy
 * Purpose:
 *      Free all memory used by a active VLANs bitmap
 */

int
_bcm_vbmp_destroy(vbmp_t *bmp)
{
    if (bmp->w != NULL) {
        sal_free(bmp->w);
        bmp->w = NULL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_vbmp_init
 * Purpose:
 *      Create new valid vlans bitmap.
 */

int
_bcm_vbmp_init(vbmp_t *bmp)
{
    int alloc_size;

    /* Deinit previously allocated bitmap. */
    _bcm_vbmp_destroy(bmp);

    alloc_size = SHR_BITALLOCSIZE(BCM_VLAN_COUNT);
    bmp->w = sal_alloc(alloc_size, "Active vlans bitmap");
    if (NULL == bmp->w) {
        return (BCM_E_MEMORY);
    }
    sal_memset(bmp->w, 0, alloc_size);

    return BCM_E_NONE;
}

/*
 * Check for valid VLAN. If easy reload, check software state;
 * otherwise, check hardware entry
 */
int _bcm_vlan_valid_check(int unit, int table, vlan_tab_entry_t *vt, 
                     bcm_vlan_t vid) 
{
#if defined(BCM_EASY_RELOAD_SUPPORT)                          
    if (SOC_IS_RELOADING(unit)) {
        vbmp_t vbmp;
        
        if (! ((table == EGR_VLANm) || (table == VLAN_TABm || table == VLAN_2_TABm))) {
            return BCM_E_PARAM;
        }
        vbmp = (table == EGR_VLANm) ? vlan_info[unit].egr_vlan_bmp :
               vlan_info[unit].vlan_tab_bmp;
        if (! _BCM_VBMP_LOOKUP(vbmp, vid)) {       
            return BCM_E_NOT_FOUND;                             
        }
    } else                                                    
#endif                                                        
    {                                                         
        if (!soc_mem_field32_get(unit, table, vt, VALIDf)) {  
            return BCM_E_NOT_FOUND;                             
        }
    }
    return BCM_E_NONE;
}
    
#if defined(BCM_EASY_RELOAD_SUPPORT)                          
/*
 * Set valid VLAN as software state for easy reload
 */
int _bcm_vlan_valid_set(int unit, int table, bcm_vlan_t vid, int val)
{
    vbmp_t vbmp;
    if (! ((table == EGR_VLANm) || (table == VLAN_TABm || table == VLAN_2_TABm))) {
        return BCM_E_PARAM;
    }
    vbmp = (table == EGR_VLANm) ? vlan_info[unit].egr_vlan_bmp :
           vlan_info[unit].vlan_tab_bmp;
    
    if (val) {
        _BCM_VBMP_INSERT(vbmp, vid);
    } else {
        _BCM_VBMP_REMOVE(vbmp, vid);
    }
    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *      _bcm_vlan_control_trust_gport
 * Purpose:
 *      Validate if a given VLAN control is capable of parsing GPORT internaly 
 * Parameters:
 *      type -  VLAN control to be checked 
 * Returns:
 *      BCM_E_NONE - control is capable of parsing GPORT.
 *      BCM_E_INTERNAL - GPORT for the control should be parsed by the API
 */
STATIC int
_bcm_vlan_control_trust_gport(bcm_vlan_control_port_t type)
{
    int                     itter;
    bcm_vlan_control_port_t types_arr[] = {
        bcmVlanPortUseInnerPri,
        bcmVlanPortUseOuterPri,
        bcmVlanPortTranslateKeyFirst,
        bcmVlanPortTranslateEgressKey
        };

   for (itter = 0; itter < COUNTOF(types_arr); itter ++) {
       if (types_arr[itter] == type) {
           return BCM_E_NONE;
       }
   }

   return BCM_E_INTERNAL;
}


/*
 * Function:
 *      _bcm_vlan_1st (internal)
 * Purpose:
 *      Return the first defined VLAN ID that is not the default VLAN.
 */

STATIC INLINE bcm_vlan_t
_bcm_vlan_1st(int unit)
{
    int idx;
#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_MONTEREY_SUPPORT)
    bcm_vlan_t          olp_vlan = BCM_VLAN_INVALID;
    uint8 feature_xflow_macsec_olp = 0;
    if (soc_feature(unit, soc_feature_xflow_macsec_olp)) {
        olp_vlan = soc_property_get(unit, spn_XFLOW_MACSEC_OLP_VLAN, BCM_VLAN_INVALID);
        feature_xflow_macsec_olp = 1;
    }
#endif
    for (idx = BCM_VLAN_MIN; idx < BCM_VLAN_COUNT; idx++) {
#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_MONTEREY_SUPPORT)
        if ((idx == olp_vlan) && (feature_xflow_macsec_olp)) {
            continue;
        }
#endif
        if ((idx != vlan_info[unit].defl) &&
            (_BCM_VBMP_LOOKUP(vlan_info[unit].bmp, idx))) {
            return (idx);
        }
    }
    return BCM_VLAN_INVALID;
}

/*
 * Function:
 *      bcm_vlan_init
 * Purpose:
 *      Initialize the VLAN module.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 */

int
bcm_esw_vlan_init(int unit)
{
    bcm_vlan_info_t             *vi = &vlan_info[unit];
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_WARM_BOOT_SUPPORT)
    int    qm_size = 0;
#endif
    int rv = BCM_E_NONE;
#ifdef BCM_WARM_BOOT_SUPPORT
    soc_scache_handle_t scache_handle;
    uint8 *vlan_scache_ptr;
    uint32 vlan_scache_size;
#else
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_KATANA_SUPPORT
    vlan_subnet_entry_t *vstab=NULL;
    vlan_subnet_entry_t *vsnull=NULL;
    vlan_subnet_entry_t *vstabp=NULL;
    int index=0;
    int vsbytes=0;
#endif
    int vlan_auto_stack_soc;
#ifdef BCM_TRX_SUPPORT
    int i, j, range_max_idx = 0, range_alloc_size = 0;
#endif /* BCM_TRX_SUPPORT */
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_vlan_init(unit);
    }
#endif

    vlan_auto_stack_soc = soc_property_get(unit, spn_VLAN_AUTO_STACK, 0);
    if (vlan_auto_stack_soc == 1) {
        vi->vlan_auto_stack = 1;
    } else if (vlan_auto_stack_soc == 2) {
        vi->vlan_auto_stack = 0;
    } else {
#ifdef BCM_VLAN_NO_AUTO_STACK
        vi->vlan_auto_stack = 0;
#else
        vi->vlan_auto_stack = 1;
#endif /* BCM_VLAN_NO_AUTO_STACK */
    }

    if (SOC_WARM_BOOT(unit)) {
        return(_bcm_vlan_reinit(unit));
    } else {
       
        bcm_vlan_data_t         vd;
        bcm_pbmp_t              temp_pbmp;

        /*
         * Initialize hardware tables
         */

        BCM_PBMP_CLEAR(temp_pbmp);
        BCM_PBMP_ASSIGN(temp_pbmp, PBMP_ALL(unit));
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit) && soc_feature(unit, soc_feature_flex_port)) {
            BCM_IF_ERROR_RETURN(_bcm_kt2_flexio_pbmp_update(unit, &temp_pbmp));
        }
#endif
        vd.vlan_tag = BCM_VLAN_DEFAULT;
        BCM_PBMP_ASSIGN(vd.port_bitmap, temp_pbmp);
        BCM_PBMP_REMOVE(vd.port_bitmap, PBMP_LB(unit));
        BCM_PBMP_REMOVE(vd.port_bitmap, PBMP_RDB_ALL(unit));
        BCM_PBMP_REMOVE(vd.port_bitmap, PBMP_FAE_ALL(unit));
        BCM_PBMP_ASSIGN(vd.ut_port_bitmap, temp_pbmp);
        BCM_PBMP_REMOVE(vd.ut_port_bitmap, PBMP_CMIC(unit));
        BCM_PBMP_REMOVE(vd.ut_port_bitmap, PBMP_LB(unit));
        BCM_PBMP_REMOVE(vd.ut_port_bitmap, PBMP_RDB_ALL(unit));
        BCM_PBMP_REMOVE(vd.ut_port_bitmap, PBMP_FAE_ALL(unit));
        BCM_PBMP_REMOVE(vd.ut_port_bitmap, PBMP_MACSEC_ALL(unit));

        /*
         * A compile-time application policy may prefer to not add
         * Ethernet or CPU ports to the default VLAN.
         */

#ifdef  BCM_VLAN_NO_DEFAULT_ETHER
        BCM_PBMP_REMOVE(vd.port_bitmap, PBMP_E_ALL(unit));
        BCM_PBMP_REMOVE(vd.ut_port_bitmap, PBMP_E_ALL(unit));
#endif  /* BCM_VLAN_NO_DEFAULT_ETHER */
#ifdef  BCM_VLAN_NO_DEFAULT_CPU
        BCM_PBMP_REMOVE(vd.port_bitmap, PBMP_CMIC(unit));
#endif  /* BCM_VLAN_NO_DEFAULT_CPU */

        if (!vi->vlan_auto_stack) {
            BCM_PBMP_REMOVE(vd.port_bitmap, SOC_PBMP_STACK_CURRENT(unit));
            BCM_PBMP_REMOVE(vd.ut_port_bitmap, SOC_PBMP_STACK_CURRENT(unit));
        }

#if defined(BCM_RCPU_SUPPORT)
        if (SOC_IS_RCPU_ONLY(unit) && vlan_info[unit].init) {
            bcm_vlan_t idx;
            soc_rcpu_cfg_t cfg;

            if (SOC_E_NONE != soc_cm_get_rcpu_cfg(unit, &cfg)) {
                cfg.vlan = BCM_VLAN_INVALID;
            }

            /* Destroy previous VLANs */
            for (idx = BCM_VLAN_MIN; idx < BCM_VLAN_COUNT; idx++) {
                if ((idx != vlan_info[unit].defl) &&
                    (idx != cfg.vlan) && 
                    (_BCM_VBMP_LOOKUP(vlan_info[unit].bmp, idx))) {
                    BCM_IF_ERROR_RETURN
                        (mbcm_driver[unit]->mbcm_vlan_destroy(unit, idx));
                }
            }
        }
#endif  /* BCM_RCPU_SUPPORT */

#ifdef BCM_MCAST_FLOOD_DEFAULT
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_flood_default_set(unit, BCM_MCAST_FLOOD_DEFAULT));
#else
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_flood_default_set(unit,
                                             BCM_VLAN_MCAST_FLOOD_UNKNOWN));
#endif

#if defined (BCM_EASY_RELOAD_SUPPORT)
        BCM_IF_ERROR_RETURN(_bcm_vbmp_init(&vi->egr_vlan_bmp));
        BCM_IF_ERROR_RETURN(_bcm_vbmp_init(&vi->vlan_tab_bmp));
#endif

        BCM_IF_ERROR_RETURN
            (mbcm_driver[unit]->mbcm_vlan_init(unit, &vd));


#ifdef BCM_TRIUMPH2_SUPPORT
        if (soc_feature(unit, soc_feature_vlan_queue_map)) {
            if (vi->qm_bmp != NULL) {
                sal_free(vi->qm_bmp);
            }
            if (vi->qm_it_bmp != NULL) {
                sal_free(vi->qm_it_bmp);
            }

            qm_size = SHR_BITALLOCSIZE
                (1 << soc_mem_field_length(unit, VLAN_PROFILE_TABm,
                                           PHB2_DOT1P_MAPPING_PTRf));
            vi->qm_bmp = sal_alloc(qm_size, "VLAN queue mapping bitmap");
            if (vi->qm_bmp == NULL) {
                return BCM_E_MEMORY;
            }
            sal_memset(vi->qm_bmp, 0, qm_size); 
            vi->qm_it_bmp = sal_alloc(qm_size, "VLAN queue mapping bitmap");
            if (vi->qm_it_bmp == NULL) {
                return BCM_E_MEMORY;
            }
        }
#endif

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_vlan_vp)) {
#ifdef BCM_ENDURO_SUPPORT
            if (SOC_IS_ENDURO(unit)) {
                BCM_IF_ERROR_RETURN(bcm_enduro_vlan_virtual_init(unit));
            } else
#endif /* BCM_ENDURO_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(bcm_tr2_vlan_virtual_init(unit));
            } 
        }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */

#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3) 
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
            BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_init(unit));
        } else
#endif
        if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership) ||
            soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) {
            BCM_IF_ERROR_RETURN(bcm_td_vp_group_init(unit));
        }
#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */

#ifdef BCM_KATANA_SUPPORT

    /* ###################################################################### */
    /* Temporary Work around for katata chip to initialize vlan_subnet table  */
    /* with zero entries                                                      */
    /* ###################################################################### */

    if (SOC_IS_KATANA(unit)) {
        vsnull = soc_mem_entry_null(unit, VLAN_SUBNETm);
        vsbytes = soc_mem_entry_words(unit, VLAN_SUBNETm);
        vsbytes = WORDS2BYTES(vsbytes);
        vstab = soc_cm_salloc(unit, 
                              soc_mem_index_count(unit, VLAN_SUBNETm) * 
                              sizeof(*vstab), "vlan_subnet");
        if (vstab == NULL) {
            return BCM_E_MEMORY;
        }
        rv = soc_mem_read_range(unit, VLAN_SUBNETm, MEM_BLOCK_ANY,
                                soc_mem_index_min(unit, VLAN_SUBNETm), 
                                soc_mem_index_max(unit, VLAN_SUBNETm), vstab);
        if (BCM_FAILURE(rv)) {
            soc_cm_sfree(unit, vstab);
            return rv;
        }
        for(index = soc_mem_index_min(unit, VLAN_SUBNETm); 
            index < soc_mem_index_count(unit, VLAN_SUBNETm); 
            index++) {
            vstabp = soc_mem_table_idx_to_pointer(unit,
                            VLAN_SUBNETm, vlan_subnet_entry_t *,
                            vstab, index);
            sal_memcpy(vstabp,vsnull,vsbytes);
        }
        rv = soc_mem_write_range(unit, VLAN_SUBNETm, MEM_BLOCK_ALL,
                                 soc_mem_index_min(unit, VLAN_SUBNETm), 
                                 soc_mem_index_max(unit, VLAN_SUBNETm), vstab);
        soc_cm_sfree(unit, vstab);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }
#endif

        /*
         * Initialize software structures
         */
        vi->defl = BCM_VLAN_DEFAULT;

#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(unit)) {
            if (soc_feature(unit, soc_feature_vlan_translation_range)) {
                if (vi->vlan_range_no_act_array != NULL) {
                    sal_free(vi->vlan_range_no_act_array);
                }
                range_max_idx = soc_mem_index_count(unit, ING_VLAN_RANGEm);
                range_alloc_size = sizeof(vlan_range_bk_t) * range_max_idx;
                vi->vlan_range_no_act_array = sal_alloc(range_alloc_size,
                                                        "VLAN range BK array");
                if (vi->vlan_range_no_act_array == NULL) {
                    return BCM_E_MEMORY;
                }
                for (i = 0; i < range_max_idx; i++) {
                    for (j = 0; j < BCM_VLAN_RANGE_NUM; j++) {
                        vi->vlan_range_no_act_array[i].vlan_low[j] =
                            BCM_VLAN_INVALID;
                    }
                }

                if (vi->vlan_range_inner_no_act_array != NULL) {
                    sal_free(vi->vlan_range_inner_no_act_array);
                }
                range_alloc_size = sizeof(vlan_range_bk_t) * range_max_idx;
                vi->vlan_range_inner_no_act_array = sal_alloc(range_alloc_size,
                                                    "VLAN range BK array");
                if (vi->vlan_range_inner_no_act_array == NULL) {
                    return BCM_E_MEMORY;
                }
                for (i = 0; i < range_max_idx; i++) {
                    for (j = 0; j < BCM_VLAN_RANGE_NUM; j++) {
                        vi->vlan_range_inner_no_act_array[i].vlan_low[j] =
                            BCM_VLAN_INVALID;
                    }
                }

                if (vi->vlan_range_no_act_ref_cnt_array != NULL) {
                    sal_free(vi->vlan_range_no_act_ref_cnt_array);
                }
                range_alloc_size = sizeof(vlan_range_bk_ref_cnt_t) * range_max_idx;
                vi->vlan_range_no_act_ref_cnt_array =
                       sal_alloc(range_alloc_size, "VLAN range ref cnt array");
                if (vi->vlan_range_no_act_ref_cnt_array == NULL) {
                    return BCM_E_MEMORY;
                }
                for (i = 0; i < range_max_idx; i++) {
                    vi->vlan_range_no_act_ref_cnt_array[i].ref_cnt = 0;
                }

                if (vi->vlan_range_inner_no_act_ref_cnt_array != NULL) {
                    sal_free(vi->vlan_range_inner_no_act_ref_cnt_array);
                }
                range_alloc_size = sizeof(vlan_range_bk_ref_cnt_t) * range_max_idx;
                vi->vlan_range_inner_no_act_ref_cnt_array =
                        sal_alloc(range_alloc_size, "VLAN range inner ref cnt array");
                if (vi->vlan_range_inner_no_act_ref_cnt_array == NULL) {
                    return BCM_E_MEMORY;
                }
                for (i = 0; i < range_max_idx; i++) {
                    vi->vlan_range_inner_no_act_ref_cnt_array[i].ref_cnt = 0;
                }

            }
        }
#endif /* BCM_TRX_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT
        /* Warm boot level 2 cache size */
        /* Type for 'flood_mode' is an enum, which values fit in uint8 */
        vlan_scache_size = sizeof(vi->defl);/* Default VLAN */
        vlan_scache_size += sizeof(vi->old_egr_xlate_cnt); /* VLAN XLATE old_cnt */
        vlan_scache_size += sizeof(uint8);  /* Flood mode */
        if (soc_feature(unit, soc_feature_vlan_queue_map)) {
            vlan_scache_size += (qm_size * 2);  /* (qm_bmp, qm_it_bmp) */
        }

        if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership) ||
            soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) {
            vlan_scache_size += sizeof(uint8) * 2;  /* vp group unmanaged flags */
        }

        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_1) {
            /* VLAN info: VP Multicast control mode */
            vlan_scache_size += sizeof(vi->vp_mode);
        }

#ifdef BCM_TRIDENT3_SUPPORT
        if (soc_feature(unit, soc_feature_pvlan_on_vfi)) {
            if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
                vlan_scache_size += sizeof(vi->shared_vlan_enable);
            }
            vi->shared_vlan_enable = 0;
        }
#endif

#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(unit)) {
            if (soc_feature(unit, soc_feature_vlan_translation_range)) {
                if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_3) {
                    range_max_idx = soc_mem_index_count(unit, ING_VLAN_RANGEm);
                    range_alloc_size = sizeof(vlan_range_bk_t) * range_max_idx;
                    vlan_scache_size += range_alloc_size;
                }
                if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_4) {
                    /* For version 1_4, also account for the new inner
                       vlan range and the ref counts added */
                    range_max_idx = soc_mem_index_count(unit, ING_VLAN_RANGEm);
                    range_alloc_size = sizeof(vlan_range_bk_t) * range_max_idx;
                    vlan_scache_size += range_alloc_size;
                    range_alloc_size = sizeof(vlan_range_bk_ref_cnt_t) * range_max_idx;
                    vlan_scache_size += (range_alloc_size * 2);
                }
            }
        }
#endif /* BCM_TRX_SUPPORT */

        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_VLAN, 0);
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                     vlan_scache_size,
                                     &vlan_scache_ptr, 
                                     BCM_WB_DEFAULT_VERSION, NULL);
        if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
            return rv;
        }
        rv = BCM_E_NONE;
#endif /* BCM_WARM_BOOT_SUPPORT */


        /* In case bcm_vlan_init is called more than once */
        BCM_IF_ERROR_RETURN(_bcm_vbmp_init(&vi->bmp));
        _BCM_VBMP_INSERT(vi->bmp, vd.vlan_tag);

        BCM_IF_ERROR_RETURN(_bcm_vbmp_init(&vi->pre_cfg_bmp));

#if defined (BCM_EASY_RELOAD_SUPPORT)
        _BCM_VBMP_INSERT(vi->egr_vlan_bmp, vd.vlan_tag);
        _BCM_VBMP_INSERT(vi->vlan_tab_bmp, vd.vlan_tag);
#endif

        /* Free vlan translation arrays if any. */
        if (NULL != vi->egr_trans) {
            sal_free(vi->egr_trans);
            vi->egr_trans = NULL;
        }
        if (NULL != vi->ing_trans) {
            sal_free(vi->ing_trans);
            vi->ing_trans = NULL;
        }

        vi->count = 1;

    }

    vi->init = TRUE;

#if defined(BCM_RCPU_SUPPORT)
    if (SOC_IS_RCPU_ONLY(unit)) {
        soc_rcpu_cfg_t cfg;
        if (SOC_E_NONE == soc_cm_get_rcpu_cfg(unit, &cfg) && 
            BCM_VLAN_DEFAULT != cfg.vlan) {
            /* 
             * RCPU vlan should have been created in remote device, sync 
             * software status.
             */
            _BCM_VBMP_INSERT(vlan_info[unit].bmp, cfg.vlan);
            vlan_info[unit].count++;
        }
    }
#endif  /* BCM_RCPU_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_esw_vlan_detach
 * Purpose:
 *      De-initialize the VLAN module.
 * Parameters:
 *      unit - (IN) BCM device number.
 * Returns:
 *      BCM_E_XXX
 */

int 
bcm_esw_vlan_detach(int unit)
{
    bcm_vlan_info_t *vi = &vlan_info[unit];
    int rv = BCM_E_NONE;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_vlan_detach(unit);
    }
#endif

    BCM_LOCK(unit);

    _bcm_vbmp_destroy(&vi->bmp);
    rv = _bcm_vbmp_destroy(&vi->pre_cfg_bmp);
    if (BCM_FAILURE(rv)) {
        BCM_UNLOCK(unit);
        return (rv);
    }
#if defined (BCM_EASY_RELOAD_SUPPORT)
    rv = _bcm_vbmp_destroy(&vi->egr_vlan_bmp);
    if (BCM_FAILURE(rv)) {
        BCM_UNLOCK(unit);
        return (rv);
    }
    rv = _bcm_vbmp_destroy(&vi->vlan_tab_bmp);
    if (BCM_FAILURE(rv)) {
        BCM_UNLOCK(unit);
        return (rv);
    }
#endif

    if (vi->egr_trans != NULL) {
        sal_free(vi->egr_trans);
        vi->egr_trans = NULL;
    }
    if (vi->ing_trans != NULL) {
        sal_free(vi->ing_trans);
        vi->ing_trans = NULL;
    }

    vi->init = 0;

#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        rv = _bcm_esw_flex_stat_detach(unit, _bcmFlexStatTypeService);
    }
    if (soc_feature(unit, soc_feature_vlan_queue_map)) {
        sal_free(vi->qm_bmp);
        vi->qm_bmp = NULL;
        sal_free(vi->qm_it_bmp);
        vi->qm_it_bmp  = NULL;
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
    || defined(BCM_RAVEN_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_ctrl)) {
        rv = _bcm_xgs3_vlan_profile_detach(unit);
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || \
          BCM_RAVEN_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_vlan_vp)) {
#ifdef BCM_ENDURO_SUPPORT
            if (SOC_IS_ENDURO(unit)) {
                rv = bcm_enduro_vlan_virtual_detach(unit);
                if (BCM_FAILURE(rv)) {
                    BCM_UNLOCK(unit);
                    return (rv);
                }
            } else
#endif /* BCM_ENDURO_SUPPORT */
            {
                rv = bcm_tr2_vlan_virtual_detach(unit);
                if (BCM_FAILURE(rv)) {
                    BCM_UNLOCK(unit);
                    return (rv);
                }
            } 
        }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */

#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3) 
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        rv = bcm_td2p_vp_group_detach(unit);
        if (BCM_FAILURE(rv)) {
            BCM_UNLOCK(unit);
            return (rv);
        }
    } else
#endif
    if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership) ||
        soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) {
        rv = bcm_td_vp_group_detach(unit);
        if (BCM_FAILURE(rv)) {
            BCM_UNLOCK(unit);
            return (rv);
        }
    }
#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */

#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(unit)) {
            if (soc_feature(unit, soc_feature_vlan_translation_range)) {
                if (vi->vlan_range_no_act_array != NULL) {
                    sal_free(vi->vlan_range_no_act_array);
                    vi->vlan_range_no_act_array = NULL;
                }
                if (vi->vlan_range_inner_no_act_array != NULL) {
                    sal_free(vi->vlan_range_inner_no_act_array);
                    vi->vlan_range_inner_no_act_array = NULL;
                }
                if (vi->vlan_range_no_act_ref_cnt_array != NULL) {
                    sal_free(vi->vlan_range_no_act_ref_cnt_array);
                    vi->vlan_range_no_act_ref_cnt_array = NULL;
                }
                if (vi->vlan_range_inner_no_act_ref_cnt_array != NULL) {
                    sal_free(vi->vlan_range_inner_no_act_ref_cnt_array);
                    vi->vlan_range_inner_no_act_ref_cnt_array = NULL;
                }
            }
        }
#endif /* BCM_TRX_SUPPORT */

    BCM_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_vlan_create
 * Purpose:
 *      Main body of bcm_vlan_create; assumes locking already done;
 */

STATIC int
_bcm_vlan_create(int unit, bcm_vlan_t vid)
{
    bcm_stg_t   stg_defl;
    int         rv;
    rv = bcm_esw_stg_default_get(unit, &stg_defl);
    if (rv == BCM_E_UNAVAIL) {
        stg_defl = -1;
    } else if (rv < 0) {
        return rv;
    }

#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        return BCM_E_NONE;
    }
#endif /* BCM_SHADOW_SUPPORT */

    if (_BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vid)) {
        return BCM_E_EXISTS;
    }

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_vlan_create(unit, vid));

    _BCM_VBMP_INSERT(vlan_info[unit].bmp, vid);
    SHR_BITCLR(vlan_info[unit].pre_cfg_bmp.w,vid);
    vlan_info[unit].count++;

    if (stg_defl >= 0) {
        /* Must be after v bitmap insert */
        BCM_IF_ERROR_RETURN
            (bcm_esw_stg_vlan_add(unit, stg_defl, vid));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_create
 * Purpose:
 *      Allocate and configure a VLAN on StrataSwitch.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vid - VLAN ID to create.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 *      BCM_E_EXISTS - VLAN ID already in use.
 * Notes:
 *      VLAN is placed in the default STG and can be reassigned later.
 */

int
bcm_esw_vlan_create(int unit, bcm_vlan_t vid)
{
    int                 rv;

    LOG_VERBOSE(BSL_LS_BCM_VLAN,
                (BSL_META_U(unit,
                            "VLAN %d: create vid %d\n"),
                 unit, vid));

    CHECK_INIT(unit);
    VLAN_CHK_ID(unit, vid);
    
    /* Check if valid vlan id */
    if (vid == BCM_VLAN_NONE) {
      return BCM_E_PARAM;
    }

    BCM_LOCK(unit);
    rv = _bcm_vlan_create(unit, vid);

    if (vlan_info[unit].vlan_auto_stack) {
        if (rv == BCM_E_NONE) {
            soc_pbmp_t  stacked, empty_pbmp;

            SOC_PBMP_ASSIGN(stacked, SOC_PBMP_STACK_CURRENT(unit));
            SOC_PBMP_CLEAR(empty_pbmp);

            rv = _bcm_vlan_port_add(unit, vid, stacked, empty_pbmp, stacked, 0);
        }
    }
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_vlan_destroy
 * Purpose:
 *      Main body of bcm_vlan_destroy; assumes locking done.
 */

STATIC int
_bcm_vlan_destroy(int unit, bcm_vlan_t vid)
{
    bcm_stg_t           stg;
    int                 rv;
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    bcm_multicast_t     mc_group;
    vlan_tab_entry_t    vtab;
    int                 bc_index, umc_index, uuc_index;
#endif

    /* Cannot destroy default VLAN */
    if (vid == vlan_info[unit].defl) {
        return BCM_E_BADID;
    }

    if (0 == _BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vid)) {
        return BCM_E_NOT_FOUND;
    }

#if defined(BCM_RCPU_SUPPORT)
    if (SOC_IS_RCPU_ONLY(unit)) {
        soc_rcpu_cfg_t cfg;
        if ((SOC_E_NONE == soc_cm_get_rcpu_cfg(unit, &cfg)) && 
            (vid == cfg.vlan)) {
            return BCM_E_BADID;
        }
    }
#endif  /* BCM_RCPU_SUPPORT */

    /* Remove VLAN from its spanning tree group */
    BCM_IF_ERROR_RETURN
        (bcm_esw_vlan_stg_get(unit, vid, &stg));

    rv = _bcm_stg_vlan_destroy(unit, stg, vid);
    if (rv < 0 && rv != BCM_E_UNAVAIL) {
        return rv;
    }

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    /* If virtual port switching is enabled, destroy the
     * BC, UMC, and UUC multicast groups.
     */
    if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int)vid, &vtab));

        if (soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
            bc_index = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, BC_IDXf);
            if (BCM_SUCCESS(
                _bcm_tr_multicast_ipmc_group_type_get(unit, bc_index,
                                                       &mc_group))) {
                BCM_IF_ERROR_RETURN(bcm_esw_multicast_destroy(unit, mc_group));
            }

            umc_index = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, UMC_IDXf);
            if (umc_index != bc_index) {
                if (BCM_SUCCESS(
                    _bcm_tr_multicast_ipmc_group_type_get(unit, umc_index,
                                                           &mc_group))) {
                    BCM_IF_ERROR_RETURN(bcm_esw_multicast_destroy(unit, mc_group));
                 }
            }

            uuc_index = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, UUC_IDXf);
            if ((uuc_index != bc_index) && (uuc_index != umc_index)) {
                if (BCM_SUCCESS(
                    _bcm_tr_multicast_ipmc_group_type_get(unit, uuc_index,
                                                           &mc_group))) {
                    BCM_IF_ERROR_RETURN(bcm_esw_multicast_destroy(unit, mc_group));
                }
            }
        }
    }
#endif

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_vlan_destroy(unit, vid));

    _BCM_VBMP_REMOVE(vlan_info[unit].bmp, vid);

    vlan_info[unit].count--;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_destroy
 * Purpose:
 *      Deallocate VLAN from StrataSwitch.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vid - VLAN ID to affect.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_INTERNAL          Chip access failure.
 *      BCM_E_BADID           Cannot remove default VLAN
 *      BCM_E_NOT_FOUND VLAN ID not in use.
 * Notes:
 *      None.
 */

int
bcm_esw_vlan_destroy(int unit, bcm_vlan_t vid)
{
    int         rv;

    LOG_VERBOSE(BSL_LS_BCM_VLAN,
                (BSL_META_U(unit,
                            "VLAN %d: destroy vid %d\n"),
                 unit, vid));

    CHECK_INIT(unit);
    VLAN_CHK_ID(unit, vid);
#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_MONTEREY_SUPPORT)
    if (soc_feature(unit, soc_feature_xflow_macsec_olp)) {
        if (vid == soc_property_get(unit, spn_XFLOW_MACSEC_OLP_VLAN, BCM_VLAN_INVALID)) {
            return BCM_E_PARAM;
        }
    }
#endif
    BCM_LOCK(unit);
    rv = _bcm_vlan_destroy(unit, vid);
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_vlan_destroy_all
 * Purpose:
 *      Destroy all VLANs except the default VLAN
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 * Notes:
 *      None.
 */

int
bcm_esw_vlan_destroy_all(int unit)
{
    int                 rv = BCM_E_NONE;
    bcm_vlan_t          vid;

    LOG_VERBOSE(BSL_LS_BCM_VLAN,
                (BSL_META_U(unit,
                            "VLAN %d: destroy all VLANs\n"),
                 unit));

    CHECK_INIT(unit);

    BCM_LOCK(unit);

while ((vid = _bcm_vlan_1st(unit)) != BCM_VLAN_INVALID) {
        if ((rv = bcm_esw_vlan_destroy(unit, vid)) < 0) {
            break;
        }
    }

#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit) && !SOC_IS_SABER2(unit)) {
        /* Clear parity status on L2 bulk operation */
        if (BCM_FAILURE(rv)) {
            BCM_UNLOCK(unit);
            return (rv);
        }
        rv = soc_reg_field32_modify(unit, L2_BULK_CONTROLr,
                                    REG_PORT_ANY, STARTf, 0);
        if (BCM_FAILURE(rv)) {
            BCM_UNLOCK(unit);
            return (rv);
        }
        rv = soc_reg_field32_modify(unit, L2_BULK_CONTROLr,
                                    REG_PORT_ANY, COMPLETEf, 0);
    }
#endif
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_vlan_port_add
 * Purpose:
 *      Main part of bcm_vlan_port_add; assumes locking already done.
 */

STATIC int
_bcm_vlan_port_add(int unit, bcm_vlan_t vid, pbmp_t pbmp, pbmp_t ubmp,
                   pbmp_t ing_pbmp, int flags)
{
    pbmp_t              vlan_pbmp, vlan_ubmp, vlan_ing_pbmp, orig_pbmp;
    pbmp_t              stacked;

    /* No such thing as untagged CPU */
    SOC_PBMP_REMOVE(ubmp, PBMP_CMIC(unit));

    /* Remove internal loopback port from vlan port bitmaps.
     * Hardware does not perform ingress and egress VLAN membership checks 
     * on internal loopback port. Also, on Triumph2, the port bitmaps in
     * VLAN and EGR_VLAN tables do not include the internal loopback port.
     */
    SOC_PBMP_REMOVE(pbmp, PBMP_LB(unit));
    SOC_PBMP_REMOVE(ubmp, PBMP_LB(unit));
    SOC_PBMP_REMOVE(ing_pbmp, PBMP_LB(unit)); 

    /*
     * Illegal to have untagged stack ports; remove inactive ports from bitmap
     */
    SOC_PBMP_STACK_ACTIVE_GET(unit, stacked);
    SOC_PBMP_REMOVE(ubmp, stacked);
    SOC_PBMP_REMOVE(pbmp, SOC_PBMP_STACK_INACTIVE(unit));
    SOC_PBMP_REMOVE(ubmp, SOC_PBMP_STACK_INACTIVE(unit));
    SOC_PBMP_REMOVE(ing_pbmp, SOC_PBMP_STACK_INACTIVE(unit));

    /* Don't add ports that are already there */
    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_vlan_port_get(unit, vid, &vlan_pbmp,
                                               &vlan_ubmp, &vlan_ing_pbmp));
    SOC_PBMP_ASSIGN(orig_pbmp, pbmp);
    COMPILER_REFERENCE(orig_pbmp);
    /* But do allow for making the port tagged/untagged if its already added to the vlan */
    SOC_PBMP_XOR(vlan_ubmp, ubmp);
    SOC_PBMP_REMOVE(vlan_pbmp, vlan_ubmp);
    SOC_PBMP_REMOVE(pbmp, vlan_pbmp);
    SOC_PBMP_REMOVE(ubmp, vlan_pbmp);
    SOC_PBMP_REMOVE(ing_pbmp, vlan_ing_pbmp);

    if (SOC_PBMP_NOT_NULL(pbmp) || SOC_PBMP_NOT_NULL(ubmp) ||
        SOC_PBMP_NOT_NULL(ing_pbmp)) {
        BCM_IF_ERROR_RETURN
            (mbcm_driver[unit]->mbcm_vlan_port_add
             (unit, vid, pbmp, ubmp, ing_pbmp));
    }
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_egress_membership_l3_only) &&
        (flags & BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY)) {
        if (SOC_PBMP_NOT_NULL(pbmp)) {
            SOC_PBMP_CLEAR(ing_pbmp);
            SOC_PBMP_CLEAR(ubmp);
            BCM_IF_ERROR_RETURN
                (_bcm_xgs3_vlan_table_port_remove(unit, vid, pbmp, ubmp,
                                                  ing_pbmp, VLAN_TABLE(unit)));
        }
    }
#endif /* BCM_TRIUMPH3_SUPPORT || BCM_HURRICANE3_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf) &&
        BCM_VLAN_VALID(vid)) {

        vlan_tab_entry_t vtab;
        int ipmc_group[3] = {0, 0, 0};
        int do_not_add[3] = {0, 0, 0};
        int              i;
        int              rv;
        bcm_multicast_t  mc_group;
        pbmp_t           l2_pbmp, l3_pbmp;

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int)vid, &vtab));

        if (soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
            /* If virtual port switching is enabled, update the 
             * L2_BITMAP in the corresponding MC group
             */

            ipmc_group[0] = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, BC_IDXf);
            ipmc_group[1] = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, UMC_IDXf);
            ipmc_group[2] = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, UUC_IDXf);

            if (flags & BCM_VLAN_GPORT_ADD_BCAST_DO_NOT_ADD) {
                do_not_add[0] = 1;
            }
            if (flags & BCM_VLAN_GPORT_ADD_UNKNOWN_MCAST_DO_NOT_ADD) {
                do_not_add[1] = 1;
            }
            if (flags & BCM_VLAN_GPORT_ADD_UNKNOWN_UCAST_DO_NOT_ADD) {
                do_not_add[2] = 1;
            }

            if ((ipmc_group[0] == ipmc_group[1]) && (do_not_add[0] != do_not_add[1])) {
                return BCM_E_PARAM;
            }
            if ((ipmc_group[0] == ipmc_group[2]) && (do_not_add[0] != do_not_add[2])) {
                return BCM_E_PARAM;
            }
            if ((ipmc_group[1] == ipmc_group[2]) && (do_not_add[1] != do_not_add[2])) {
                return BCM_E_PARAM;
            }

            for (i = 0; i < 3; i++) {
                if (do_not_add[i]) {
                    continue;
                }
                /* index:0 is reserved, check if multicast group is used */
                rv = _bcm_tr_multicast_ipmc_group_type_get(unit, ipmc_group[i], &mc_group);
                if (rv < 0) {
                    /* if index is used, ensure error code is checked */
                    if (ipmc_group[i] != 0) {
                        return rv;
                    }
                    continue;
                }

                BCM_IF_ERROR_RETURN(_bcm_esw_multicast_ipmc_read(unit, ipmc_group[i],
                                                                 &l2_pbmp, 
                                                                 &l3_pbmp));
                SOC_PBMP_OR(l2_pbmp, orig_pbmp);
                BCM_IF_ERROR_RETURN(_bcm_esw_multicast_ipmc_write(unit, 
                                             ipmc_group[i], l2_pbmp, l3_pbmp, TRUE));
            }
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_port_add
 * Purpose:
 *      Add ports to the specified vlan.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vid - VLAN ID to add port to as a member.
 *      pbmp - port bitmap for members of VLAN
 *      ubmp - untagged members of VLAN
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 *      BCM_E_NOT_FOUND - VLAN ID not in use.
 */

int
bcm_esw_vlan_port_add(int unit, bcm_vlan_t vid, pbmp_t pbmp, pbmp_t ubmp)
{
    int                 rv = BCM_E_NONE;
    bcm_pbmp_t          pbm, ubm;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_vlan_port_add(unit, vid, pbmp, ubmp);
    }
#endif

    CHECK_INIT(unit);
    VLAN_CHK_ID(unit, vid);

    BCM_PBMP_CLEAR(pbm); 
    BCM_PBMP_CLEAR(ubm); 
 
     /* we need to check if pbmp and ubmp are valid as well */
     BCM_PBMP_ASSIGN(pbm,PBMP_ALL(unit));
     BCM_PBMP_ASSIGN(ubm,PBMP_ALL(unit));

#ifdef BCM_KATANA2_SUPPORT 
     if (SOC_IS_KATANA2(unit) && (soc_feature(unit, soc_feature_linkphy_coe) ||
                                     soc_feature(unit, soc_feature_subtag_coe))) {
         _bcm_kt2_subport_pbmp_update(unit, &pbm);
         _bcm_kt2_subport_pbmp_update(unit, &ubm);
     }
#endif 
 
     BCM_PBMP_AND(pbm,pbmp);
     BCM_PBMP_AND(ubm,ubmp);
 
     if(!(BCM_PBMP_EQ(pbm, pbmp) && BCM_PBMP_EQ(ubm, ubmp))) {
         return BCM_E_PARAM;
     }
 
    LOG_VERBOSE(BSL_LS_BCM_VLAN,
                (BSL_META_U(unit,
                            "VLAN %d: port add: vid %d. pbm 0x%x. utpbm 0x%x.\n"),
                 unit, vid, SOC_PBMP_WORD_GET(pbmp, 0),
                 SOC_PBMP_WORD_GET(ubmp, 0)));

    BCM_LOCK(unit);
    rv = _bcm_vlan_port_add(unit, vid, pbmp, ubmp, pbmp, 0);
    BCM_UNLOCK(unit);

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {

        if (BCM_SUCCESS(rv)) {
            soc_info_t *si = &SOC_INFO(unit);
            soc_port_t port;

            SOC_PBMP_ITER(si->subtag_allowed_pbm, port) {
                if (SOC_PBMP_MEMBER(pbmp, port)) {
                    /* Clear the membership for channelized ports. */
                    rv = bcm_esw_port_vlan_member_set(unit, port, 0);

                    if (BCM_FAILURE(rv)) {
                        (void)bcm_esw_vlan_port_remove(unit, vid, pbmp);
                    }
                }
            }
        }
    }

#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

    return rv;
}

/*
 * Function:
 *      _bcm_vlan_port_remove
 * Purpose:
 *      Main part of bcm_vlan_port_remove; assumes locking already done.
 */

STATIC int
_bcm_vlan_port_remove(int unit, bcm_vlan_t vid, pbmp_t pbmp, pbmp_t ubmp,
                      pbmp_t ing_pbmp, soc_field_t *dlf_type)
{
    pbmp_t vlan_pbmp, vlan_ubmp, vlan_ing_pbmp, orig_pbmp;
    int    rv = BCM_E_NONE;

    /* Don't remove ports that are not there */

    BCM_LOCK(unit);

    rv = (mbcm_driver[unit]->mbcm_vlan_port_get
          (unit, vid, &vlan_pbmp, &vlan_ubmp, &vlan_ing_pbmp));
    if (rv < 0) {
        BCM_UNLOCK(unit);
        return rv;
    }

    SOC_PBMP_ASSIGN(orig_pbmp, pbmp);
    if (!soc_mem_field_valid(unit, VLAN_TABLE(unit), ING_PORT_BITMAPf) &&
        !(soc_feature(unit, soc_feature_vlan_vfi_membership))) {
        SOC_PBMP_OR(vlan_pbmp, vlan_ing_pbmp);
        SOC_PBMP_AND(pbmp, vlan_pbmp);
    }
    COMPILER_REFERENCE(orig_pbmp);

    rv = (mbcm_driver[unit]->mbcm_vlan_port_remove(unit, vid, pbmp, ubmp, 
                                                   ing_pbmp));
    if (rv < 0) {
        BCM_UNLOCK(unit);
        return rv;
    }

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf) &&
        BCM_VLAN_VALID(vid)) {

        vlan_tab_entry_t vtab;
        soc_field_t grp_type[3] = {BC_IDXf, UMC_IDXf, UUC_IDXf};
        int              i;
        int              mc_index;
        bcm_multicast_t  mc_group;
        pbmp_t           l2_pbmp, l3_pbmp;
        soc_field_t * group_type;

        if (dlf_type == NULL) {
            group_type = grp_type;
        } else {
            group_type = dlf_type;
        } 

        rv = soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int)vid, &vtab);
        if (BCM_FAILURE(rv)) {
            BCM_UNLOCK(unit);
            return (rv);
        }

        if (soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
            /* If virtual port switching is enabled, update the 
             * L2_BITMAP in the corresponding MC group
             */

            for (i = 0; i < 3; i++) {
                if (group_type[i] == 0) {
                    continue;
                }
                mc_index = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab,
                        group_type[i]);

                /* index:0 is reserved, check if multicast group is used */
                rv = _bcm_tr_multicast_ipmc_group_type_get(unit, mc_index,
                        &mc_group);
                if (rv < 0) {
                    if (mc_index != 0) {
                        BCM_UNLOCK(unit);
                        return rv;
                    } 
                    continue;
                }

                rv = _bcm_esw_multicast_ipmc_read(unit, mc_index,
                        &l2_pbmp, &l3_pbmp);
                if (rv < 0) {
                    BCM_UNLOCK(unit);
                    return rv;
                }

                BCM_PBMP_REMOVE(l2_pbmp, orig_pbmp);

                rv = _bcm_esw_multicast_ipmc_write(unit, mc_index,
                        l2_pbmp, l3_pbmp, TRUE);
                if (rv < 0) {
                    BCM_UNLOCK(unit);
                    return rv;
                }
            }
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */

    BCM_UNLOCK(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_port_remove
 * Purpose:
 *      Remove ports from a specified vlan.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vid - VLAN ID to remove port(s) from.
 *      pbmp - port bitmap for ports to remove.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 *      BCM_E_NOT_FOUND - VLAN ID not in use.
 * Notes:
 *      None.
 */

int
bcm_esw_vlan_port_remove(int unit, bcm_vlan_t vid, pbmp_t pbmp)
{
    int                 rv = BCM_E_NONE;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_vlan_port_remove(unit, vid, pbmp);
    }
#endif

    LOG_VERBOSE(BSL_LS_BCM_VLAN,
                (BSL_META_U(unit,
                            "VLAN %d: port remove: vid %d. pbm 0x%x.\n"),
                 unit, vid, SOC_PBMP_WORD_GET(pbmp, 0)));

    CHECK_INIT(unit);
    VLAN_CHK_ID(unit, vid);
    /* As we are passing only one pbmp parameter to API bcm_vlan_port_remove,
     * the expected result is to remove same pbmp from PORT_BITMAPf, UT_BITMAPf
     * and ING_PORT_BITMAPf. Hence, same pbmp is passed as argument for ubmp
     * and ing_pbmp to _bcm_vlan_port_remove*/
    rv = _bcm_vlan_port_remove(unit, vid, pbmp, pbmp, pbmp, NULL);

    return rv;
}

/*
 * Function:
 *      _bcm_vlan_port_get
 * Purpose:
 *      Retrieves a list of the member ports of an existing VLAN.
 */
STATIC int
_bcm_vlan_port_get(int unit, bcm_vlan_t vid, pbmp_t *pbmp, pbmp_t *ubmp,
                   pbmp_t *ing_pbmp)
{
    int                 rv;

    BCM_LOCK(unit);
    rv = mbcm_driver[unit]->mbcm_vlan_port_get
        (unit, vid, pbmp, ubmp, ing_pbmp);

    LOG_VERBOSE(BSL_LS_BCM_VLAN,
                (BSL_META_U(unit,
                            "VLAN %d: bcm_vlan_port_get: vid %d. pbm 0x%x upbm 0x%x.\n"),
                 unit, vid, SOC_PBMP_WORD_GET(*ing_pbmp, 0),
                 SOC_PBMP_WORD_GET(*ubmp, 0)));

    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_vlan_port_get
 * Purpose:
 *      Retrieves a list of the member ports of an existing VLAN.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vid - VLAN ID to lookup.
 *      tag_pbmp - (output) member port bitmap (ignored if NULL)
 *      untag_pbmp - (output) untagged port bitmap (ignored if NULL)
 * Returns:
 *      BCM_E_NONE - Success (port bitmaps filled in).
 *      BCM_E_INTERNAL - Chip access failure.
 *      BCM_E_NOT_FOUND - No such VLAN defined.
 */

int
bcm_esw_vlan_port_get(int unit, bcm_vlan_t vid, pbmp_t *pbmp, pbmp_t *ubmp)
{
    pbmp_t ing_pbmp;

    CHECK_INIT(unit);
    VLAN_CHK_ID(unit, vid);

    if (0 == _BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vid)) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_vlan_port_get(unit, vid, pbmp, ubmp, &ing_pbmp));

    SOC_PBMP_OR(*pbmp, ing_pbmp);

    return BCM_E_NONE;

}

/*
 * Function:
 *      _bcm_vlan_list
 * Purpose:
 *      Main body of bcm_vlan_list() and bcm_vlan_list_by_pbmp().
 *      Assumes locking already done.
 * Parameters:
 *      list_all - if TRUE, lists all ports and ignores list_pbmp.
 *      list_pbmp - if list_all is FALSE, lists only VLANs containing
 *              any of the ports in list_pbmp.
 */

STATIC int
_bcm_vlan_list(int unit, bcm_vlan_data_t **listp, int *countp,
               int list_all, pbmp_t list_pbmp)
{
    bcm_vlan_data_t     *list;
    int                 count, i, rv;
    int                 idx;
    int                 valid_count;
    pbmp_t              pbmp, ubmp, tbmp;
#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_MONTEREY_SUPPORT)
    bcm_vlan_t          olp_vlan = BCM_VLAN_INVALID;
#endif

    *countp = 0;
    *listp = NULL;

    if (!list_all && SOC_PBMP_IS_NULL(list_pbmp)) {     /* Empty list */
        return BCM_E_NONE;
    }

    count = vlan_info[unit].count;

    if (count == 0) {
        return BCM_E_NONE;                      /* Empty list */
    }

    if ((list = sal_alloc(count * sizeof (list[0]), "vlan_list")) == NULL) {
        return BCM_E_MEMORY;
    }

    i = 0;
    valid_count = 0;

#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_MONTEREY_SUPPORT)
    if (soc_feature(unit, soc_feature_xflow_macsec_olp)) {
        olp_vlan = soc_property_get(unit, spn_XFLOW_MACSEC_OLP_VLAN, BCM_VLAN_INVALID);
    }
#endif

    for (idx = BCM_VLAN_MIN; idx < BCM_VLAN_COUNT; idx++) {
        if (0 == _BCM_VBMP_LOOKUP(vlan_info[unit].bmp, idx)) {
            continue;
        }

#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_MONTEREY_SUPPORT)
        if (olp_vlan == idx) {
            count -= 1;
            continue;
        }
#endif
        rv = bcm_esw_vlan_port_get(unit, idx, &pbmp, &ubmp);
        if (BCM_FAILURE(rv)) {
            sal_free(list);
            return rv;
        }
        valid_count++;

        SOC_PBMP_ASSIGN(tbmp, list_pbmp);
        SOC_PBMP_AND(tbmp, pbmp);
        if (list_all || SOC_PBMP_NOT_NULL(tbmp)) {
            list[i].vlan_tag = idx;
            BCM_PBMP_ASSIGN(list[i].port_bitmap, pbmp);
            BCM_PBMP_ASSIGN(list[i].ut_port_bitmap, ubmp);
            i++;

        }
        if (valid_count == count)   {
            break;
        }
    } 
    assert(!list_all || i == count);  /* If list_all, make sure all listed */

    *countp = i;
    *listp = list;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_list
 * Purpose:
 *      Returns an array of all defined VLANs and their port bitmaps.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *      listp - Place where pointer to return array will be stored,
 *              which will be NULL if there are zero VLANs defined.
 *      countp - Place where number of entries in array will be stored,
 *              which will be 0 if there are zero VLANs defined.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 *      BCM_E_MEMORY - Out of system memory.
 * Notes:
 *      The caller is responsible for freeing the memory that is
 *      returned, using bcm_vlan_list_destroy.
 */

int
bcm_esw_vlan_list(int unit, bcm_vlan_data_t **listp, int *countp)
{
    int         rv;
    pbmp_t      empty_pbm;

    CHECK_INIT(unit);

    SOC_PBMP_CLEAR(empty_pbm);
    BCM_LOCK(unit);
    rv = _bcm_vlan_list(unit, listp, countp, TRUE, empty_pbm);
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_esw_vlan_list_by_pbmp
 * Purpose:
 *      Returns an array of defined VLANs and port bitmaps.
 *      Only VLANs that containing any of the specified ports are listed.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *      pbmp - Bitmap of ports
 *      listp - Place where pointer to return array will be stored,
 *              which will be NULL if there are zero VLANs defined.
 *      countp - Place where number of entries in array will be stored,
 *              which will be 0 if there are zero VLANs defined.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_INTERNAL          Chip access failure.
 *      BCM_E_MEMORY            Out of system memory.
 * Notes:
 *      The caller is responsible for freeing the memory that is
 *      returned, using bcm_vlan_list_destroy.
 */

int
bcm_esw_vlan_list_by_pbmp(int unit, pbmp_t pbmp,
                          bcm_vlan_data_t **listp, int *countp)
{
    int         rv;

    CHECK_INIT(unit);

    BCM_LOCK(unit);
    rv = _bcm_vlan_list(unit, listp, countp, FALSE, pbmp);
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_vlan_list_destroy
 * Purpose:
 *      Destroy a list returned by bcm_vlan_list.
 *      Also works for the zero-VLAN case (NULL list).
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *      list - List returned by bcm_vlan_list
 *      count - Count returned by bcm_vlan_list
 * Returns:
 *      BCM_E_NONE              Success.
 * Notes:
 *      None.
 */

int
bcm_esw_vlan_list_destroy(int unit, bcm_vlan_data_t *list, int count)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(count);

    if (list != NULL) {
        sal_free(list);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_default_get
 * Purpose:
 *      Get the default VLAN ID
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vid_ptr - (OUT) Target to receive the VLAN ID.
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_esw_vlan_default_get(int unit, bcm_vlan_t *vid_ptr)
{
    *vid_ptr = vlan_info[unit].defl;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_vlan_default_set
 * Purpose:
 *      Main part of bcm_vlan_default_set; assumes locking already done.
 */

STATIC int
_bcm_vlan_default_set(int unit, bcm_vlan_t vid)
{
    if (!BCM_VLAN_VALID(vid)) {
        return BCM_E_PARAM;
    }

    if (0 == _BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vid)) {
        return BCM_E_NOT_FOUND;
    }

    vlan_info[unit].defl = vid;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_default_set
 * Purpose:
 *      Set the default VLAN ID
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vid - The new default VLAN
 * Returns:
 *      BCM_E_NONE - Success.
 * Notes:
 *      The new default VLAN must already exist.
 */

int
bcm_esw_vlan_default_set(int unit, bcm_vlan_t vid)
{
    int                 rv;

    LOG_VERBOSE(BSL_LS_BCM_VLAN,
                (BSL_META_U(unit,
                            "VLAN %d: dflt vid set: %d\n"),
                 unit, vid));

    CHECK_INIT(unit);

    BCM_LOCK(unit);
    rv = _bcm_vlan_default_set(unit, vid);

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_vlan_stg_get
 * Purpose:
 *      Retrieve the VTABLE STG for the specified vlan
 * Parameters:
 *      unit - StrataSwitch PCI device unit number.
 *      vid - VLAN ID.
 *      stg_ptr - (OUT) Pointer to store stgid for return.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_vlan_stg_get(int unit, bcm_vlan_t vid, bcm_stg_t *stg_ptr)
{
    int         rv;

    CHECK_INIT(unit);

    BCM_LOCK(unit);
    rv = mbcm_driver[unit]->mbcm_vlan_stg_get(unit, vid, stg_ptr);
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_vlan_stg_set
 * Purpose:
 *      Update the VTABLE STG for the specified vlan
 * Parameters:
 *      unit  - StrataSwitch PCI device unit number
 *      vid - VLAN ID
 *      stg - New spanning tree group number for VLAN
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_vlan_stg_set(int unit, bcm_vlan_t vid, bcm_stg_t stg)
{
    int         rv;

    CHECK_INIT(unit);

    BCM_LOCK(unit);
    rv = bcm_esw_stg_vlan_add(unit, stg, vid);
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_vlan_stp_set
 * Purpose:
 *      Main part of bcm_vlan_stp_set; assumes locking already done.
 */

STATIC int
_bcm_vlan_stp_set(int unit, bcm_vlan_t vid, bcm_port_t port, int stp_state)
{
    bcm_stg_t   stgid;

    BCM_IF_ERROR_RETURN(bcm_esw_vlan_stg_get(unit, vid, &stgid));
    BCM_IF_ERROR_RETURN(bcm_esw_stg_stp_set(unit, stgid, port, stp_state));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_stp_set
 * Purpose:
 *      Set the spanning tree state for a port in the whole spanning
 *      tree group that contains the specified VLAN.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *      vid - VLAN ID
 *      port - Port
 *      stp_state - State to set
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_vlan_stp_set(int unit, bcm_vlan_t vid,
                            bcm_port_t port, int stp_state)
{
    int         rv;

#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3) 
    int         vp = -1;
    /* Extender */
    if (soc_feature(unit, soc_feature_port_extension) && 
        BCM_GPORT_IS_EXTENDER_PORT(port)) {
        vp = BCM_GPORT_EXTENDER_PORT_ID_GET(port);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
            return BCM_E_NOT_FOUND;
        }
    }
    
    if (vp != -1) {
        if (soc_feature(unit, soc_feature_ing_vp_vlan_membership) &&
            soc_feature(unit, soc_feature_egr_vp_vlan_membership)) {
            return bcm_td2_vp_vlan_stp_set(unit, vp, vid, stp_state);
        }
        return BCM_E_UNAVAIL;
    }
#endif

    BCM_LOCK(unit);
    rv = _bcm_vlan_stp_set(unit, vid, port, stp_state);
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_vlan_stp_get
 * Purpose:
 *      Main part of bcm_vlan_stp_get; assumes locking already done.
 */

STATIC int
_bcm_vlan_stp_get(int unit, bcm_vlan_t vid,
                       bcm_port_t port, int *stp_state)
{
    bcm_stg_t   stgid;

    BCM_IF_ERROR_RETURN(bcm_esw_vlan_stg_get(unit, vid, &stgid));
    BCM_IF_ERROR_RETURN(bcm_esw_stg_stp_get(unit, stgid, port, stp_state));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_stp_get
 * Purpose:
 *      Get the spanning tree state for a port in the whole spanning
 *      tree group that contains the specified VLAN.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number.
 *      vid - VLAN ID.
 *      port - Port
 *      stp_state - (OUT) State to return.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_vlan_stp_get(int unit, bcm_vlan_t vid,
                            bcm_port_t port, int *stp_state)
{
    int         rv;

#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    int         vp = -1;
    /* Extender */
    if (soc_feature(unit, soc_feature_port_extension) && 
        BCM_GPORT_IS_EXTENDER_PORT(port)) {
        vp = BCM_GPORT_EXTENDER_PORT_ID_GET(port);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
            return BCM_E_NOT_FOUND;
        }
    }
    
    if (vp != -1) {
        if (soc_feature(unit, soc_feature_ing_vp_vlan_membership) &&
            soc_feature(unit, soc_feature_egr_vp_vlan_membership)) {
            return bcm_td2_vp_vlan_stp_get(unit, vp, vid, stp_state);
        }
        return BCM_E_UNAVAIL;
    }
#endif

    BCM_LOCK(unit);
    rv = _bcm_vlan_stp_get(unit, vid, port, stp_state);
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_esw_vlan_policer_get
 * Purpose:
 *     Retrieve the Policer ID accociated for the specified physical port.
 * Parameters:
 *     Unit                  - (IN) unit number 
 *     vlan                  - (IN) VLAN 
 *     policer               - (OUT) policer Id 
 * Returns:
 *     BCM_E_XXX 
 */
int bcm_esw_vlan_policer_get(
    int unit,
    bcm_vlan_t vlan,
    bcm_policer_t *policer)
{
    int rv = BCM_E_NONE;
    vlan_tab_entry_t   vt;
    
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    VLAN_CHK_ID(unit, vlan);
    BCM_IF_ERROR_RETURN(_check_global_meter_init(unit));
    *policer = 0;
    rv = _bcm_esw_get_policer_from_table(unit, VLAN_TABm, vlan, &vt, 
                                                 policer, 0); 
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_VLAN,
                  (BSL_META_U(unit,
                              "Unable to read vlan table entry\n")));
        return (rv);
    }
    return rv;

}

/*
 * Function:
 *     bcm_esw_vlan_policer_set
 * Purpose:
 *     Set the Policer ID accociated for the specified physical port. 
 * Parameters:
 *     Unit                  - (IN) unit number 
 *     vlan                  - (IN) VLAN 
 *     policer               - (IN) policer Id 
 * Returns:
 *     BCM_E_XXX 
 */
int bcm_esw_vlan_policer_set(
    int unit,
    bcm_vlan_t vlan,
    bcm_policer_t policer)
{
    int rv = BCM_E_NONE;
    vlan_tab_entry_t   vt;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    VLAN_CHK_ID(unit, vlan);
    if (!soc_feature(unit, soc_feature_global_meter)) {
        return BCM_E_UNAVAIL;
    }
    BCM_IF_ERROR_RETURN(_check_global_meter_init(unit));
    rv = _bcm_esw_policer_validate(unit, &policer);
    if (BCM_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_BCM_VLAN,
                (BSL_META_U(unit,
                            "Invalid policer id passed: %x \n"),
                 policer));
        return (rv);
    }
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) || SOC_IS_APACHE(unit)) {
        int offset_mode = 0;
        offset_mode = (((policer) & BCM_POLICER_GLOBAL_METER_MODE_MASK) >>
                BCM_POLICER_GLOBAL_METER_MODE_SHIFT);

        if ((offset_mode == 0) && (policer != 0)) {
            /*
             * Not a Global meter policer, 
             * policer zero is used to detach already configured policer
             */
            return (BCM_E_PARAM);
        } 
    } 
#endif
    soc_mem_lock(unit, VLAN_TABm);
    rv = soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, vlan, &vt);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_VLAN,
                  (BSL_META_U(unit,
                              "Unable to read vlan table entry\n")));
        soc_mem_unlock(unit, VLAN_TABm);
        return (rv);
    }

    rv = _bcm_esw_add_policer_to_table(unit, policer,
                                           VLAN_TABm, vlan, (void *)(&vt));
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_VLAN,
                  (BSL_META_U(unit,
                              "Unable to add policer to vlan table entry\n")));
        soc_mem_unlock(unit, VLAN_TABm);
        return (rv);
    }
    rv = soc_mem_write(unit, VLAN_TABm, MEM_BLOCK_ANY, vlan, &vt);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_VLAN,
                  (BSL_META_U(unit,
                              "Unable to add policer to vlan table entry\n")));
        soc_mem_unlock(unit, VLAN_TABm);
        return (rv);
    }
    soc_mem_unlock(unit, VLAN_TABm);
    return rv;
} 
/*
 * Port-based VLAN actions
 */
/* 
 * Function:
 *      bcm_vlan_port_default_action_set
 * Purpose: 
 *      Set the port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (IN) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_port_default_action_set(int unit, bcm_port_t port,
                                     bcm_vlan_action_set_t *action)
{   
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        rv = bcm_tomahawk3_vlan_port_default_action_set(unit, port, action);
    } else
#endif
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        bcm_port_t  local_port;

#ifdef BCM_HGPROXY_COE_SUPPORT
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            (soc_feature(unit, soc_feature_channelized_switching))) &&
            BCM_GPORT_IS_SET(port) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
            local_port = port;
        } else
#endif
        {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_port_gport_validate(unit, port, &local_port));
        }

        PORT_LOCK(unit);
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
        if (SOC_IS_KATANA2(unit) || SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
            rv = _bcm_trx_vlan_port_default_action_profile_set(unit,
                     local_port, action);
        } else
#endif
        {
            rv = _bcm_trx_vlan_port_default_action_set(unit,
                     local_port, action);
        }
        PORT_UNLOCK(unit);
    }
#endif /* BCM_TRX_SUPPORT */

    return rv;
}

/*  
 * Function:
 *      bcm_vlan_port_default_action_get
 * Purpose:
 *      Get the port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (OUT) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_port_default_action_get(int unit, bcm_port_t port,
                                     bcm_vlan_action_set_t *action)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        rv = bcm_tomahawk3_vlan_port_default_action_get(unit, port, action);
    } else
#endif
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        bcm_port_t  local_port;

#ifdef BCM_HGPROXY_COE_SUPPORT
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            (soc_feature(unit, soc_feature_channelized_switching))) &&
            BCM_GPORT_IS_SET(port) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
            local_port = port;
        } else
#endif
        {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_port_gport_validate(unit, port, &local_port));
        }

        bcm_vlan_action_set_t_init(action);
        PORT_LOCK(unit);
        rv = _bcm_trx_vlan_port_default_action_get(unit, local_port, action);
        PORT_UNLOCK(unit);
    }
#endif /* BCM_TRIUMPH_SUPPORT */

    return rv;
}


/*  
 * Function:
 *      bcm_vlan_port_default_action_delete
 * Purpose:
 *      Delete the port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_port_default_action_delete(int unit, bcm_port_t port)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        rv = bcm_tomahawk3_vlan_port_default_action_delete(unit, port);
    } else
#endif
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        bcm_port_t  local_port;

        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_gport_validate(unit, port, &local_port));

        PORT_LOCK(unit);
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
        if (SOC_IS_KATANA2(unit) || SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
            /* Increase ref count of default profile */
            rv = _bcm_port_vlan_protocol_data_entry_reference(unit, 0, 16);
            if (BCM_FAILURE(rv)) {
                PORT_UNLOCK(unit);
                return rv;
            }
            rv = _bcm_trx_vlan_port_default_action_profile_delete(unit, local_port);
        } else
#endif
        {
            rv = _bcm_trx_vlan_port_default_action_delete(unit, local_port);
        }
        PORT_UNLOCK(unit);
    }
#endif /* BCM_TRX_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_vlan_port_egress_default_action_set
 * Purpose:
 *      Set the egress port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (IN) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_port_egress_default_action_set(int unit,
                                            bcm_port_t port,
                                            bcm_vlan_action_set_t *action)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        rv =
         bcm_tomahawk3_vlan_port_egress_default_action_set(unit, port, action);
    } else
#endif
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && ((soc_feature(unit, soc_feature_vlan_action)) ||
        (soc_feature(unit, soc_feature_fast_egr_vlan_action)) || 
        (soc_feature(unit, soc_feature_egr_vlan_action)))) {
        bcm_port_t  local_port; 

        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_gport_validate(unit, port, &local_port));

        rv = _bcm_trx_vlan_port_egress_default_action_set(unit,
                                                          local_port, action);
    }
#endif /* BCM_TRX_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_vlan_port_egress_default_action_get
 * Purpose:
 *      Get the egress port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (OUT) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */     
int
bcm_esw_vlan_port_egress_default_action_get(int unit,
                                            bcm_port_t port,
                                            bcm_vlan_action_set_t *action)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        rv =
         bcm_tomahawk3_vlan_port_egress_default_action_get(unit, port, action);
    } else
#endif
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && ((soc_feature(unit, soc_feature_vlan_action)) ||
        (soc_feature(unit, soc_feature_fast_egr_vlan_action)) || 
        (soc_feature(unit, soc_feature_egr_vlan_action)))) {
        bcm_port_t  local_port;

        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_gport_validate(unit, port, &local_port));

        bcm_vlan_action_set_t_init(action);
        rv = _bcm_trx_vlan_port_egress_default_action_get(unit, local_port, 
                                                          action);
    }
#endif /* BCM_TRX_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_vlan_port_egress_default_action_delete
 * Purpose:
 *      delete the egress port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 * Returns:
 *      BCM_E_XXX
 */     
int
bcm_esw_vlan_port_egress_default_action_delete(int unit, bcm_port_t port)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        rv = bcm_tomahawk3_vlan_port_egress_default_action_delete(unit, port);
    } else
#endif
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        bcm_port_t  local_port;

        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_gport_validate(unit, port, &local_port));

        rv = _bcm_trx_vlan_port_egress_default_action_delete(unit, local_port);

        /* The port is using the default profile */
        _bcm_trx_egr_vlan_action_profile_entry_increment
            (unit, BCM_TRX_EGR_VLAN_ACTION_PROFILE_DEFAULT);
    }
#endif /* BCM_TRX_SUPPORT */

    return rv;
}

/*
 * Protocol-based VLAN actions
 */

/*
 * Function   :
 *      bcm_vlan_port_protocol_action_add
 * Description   :
 *      Add protocol based VLAN with specified action.
 *      If the entry already exists, update the action.
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      port      (IN) Port number
 *      frame     (IN) Frame type
 *      ether     (IN) 16 bit ether type
 *      action    (IN) Action for outer tag and inner tag
 * Note:
 *    Program VLAN_PROTOCOL_DATAm and VLAN_PROTOCOLm.
 */
int
bcm_esw_vlan_port_protocol_action_add(int unit,
                                      bcm_port_t port,
                                      bcm_port_frametype_t frame,
                                      bcm_port_ethertype_t ether,
                                      bcm_vlan_action_set_t *action)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (!SOC_IS_TOMAHAWK3(unit))
#endif
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        bcm_port_t  local_port; 

        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_gport_validate(unit, port, &local_port));

        if (IS_HG_PORT(unit, local_port)){
            return SOC_E_PORT;
        }

        PORT_LOCK(unit);
        rv =  _bcm_trx_vlan_port_protocol_action_add(unit, local_port, frame,
                                                     ether, action);
        PORT_UNLOCK(unit);
    }
#endif /* BCM_TRX_SUPPORT */
    return (rv);
}
/*
 * Function   :
 *      bcm_vlan_port_protocol_action_get
 * Description   :
 *      Get protocol based VLAN with specified action.
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      port      (IN) Port number
 *      frame     (IN) Frame type
 *      ether     (IN) 16 bit ether type
 *      action    (OUT) Action for outer and inner tag
 * Note:
 */
int
bcm_esw_vlan_port_protocol_action_get(int unit,
                                      bcm_port_t port,
                                      bcm_port_frametype_t frame,
                                      bcm_port_ethertype_t ether,
                                      bcm_vlan_action_set_t *action)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (!SOC_IS_TOMAHAWK3(unit))
#endif
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        bcm_port_t  local_port; 

        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_gport_validate(unit, port, &local_port));

        bcm_vlan_action_set_t_init(action);
        PORT_LOCK(unit);
        rv = _bcm_trx_vlan_port_protocol_action_get(unit, local_port, frame,
                                                     ether, action);
        PORT_UNLOCK(unit);
        return rv;
    }
#endif /* BCM_TRX_SUPPORT */

    return (rv);
}


/*
 * Function   :
 *      bcm_vlan_port_protocol_action_delete
 * Description   :
 *      Delete protocol based VLAN action.
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      port      (IN) Port number
 *      frame     (IN) Frame type
 *      ether     (IN) 16 bit ether type
 * Note:
 *    Program VLAN_PROTOCOL_DATAm and VLAN_PROTOCOLm.
 */
int
bcm_esw_vlan_port_protocol_action_delete(int unit,bcm_port_t port,
                                         bcm_port_frametype_t frame,
                                         bcm_port_ethertype_t ether)
{
    int rv = BCM_E_UNAVAIL; 
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (!SOC_IS_TOMAHAWK3(unit))
#endif
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        bcm_port_t  local_port;

        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_gport_validate(unit, port, &local_port));


        PORT_LOCK(unit);
        rv = _bcm_trx_vlan_port_protocol_delete(unit, local_port, frame, ether);
        PORT_UNLOCK(unit);
        return rv;
    }
#endif /* BCM_TRX_SUPPORT */
    return (rv);
}


/*
 * Function   :
 *      bcm_vlan_port_protocol_action_delete_all
 * Description   :
 *      Delete all protocol based VLAN actiona.
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      port      (IN) Port number
 * Note:
 *    Program VLAN_PROTOCOL_DATAm and VLAN_PROTOCOLm.
 */
int
bcm_esw_vlan_port_protocol_action_delete_all(int unit,bcm_port_t port)
{
    int rv = BCM_E_UNAVAIL; 

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (!SOC_IS_TOMAHAWK3(unit))
#endif
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        bcm_port_t  local_port; 

        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_gport_validate(unit, port, &local_port));

        PORT_LOCK(unit);
        rv = _bcm_trx_vlan_port_protocol_delete_all(unit, local_port);
        PORT_UNLOCK(unit);
    }
#endif /* BCM_TRX_SUPPORT */
    return (rv);
}

/*
 * Function   :
 *      bcm_esw_vlan_port_protocol_action_traverse
 * Description   :
 *      Traverse over vlan port protocol actions. 
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      cb        (IN) User provided call back function
 *      user_data (IN) User provided data
 * Note:
 *    Program VLAN_PROTOCOL_DATAm and VLAN_PROTOCOLm.
 */
int
bcm_esw_vlan_port_protocol_action_traverse(int unit, 
                                bcm_vlan_port_protocol_action_traverse_cb cb,
                                           void *user_data)
{
    int rv = BCM_E_UNAVAIL; 
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (!SOC_IS_TOMAHAWK3(unit))
#endif
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        PORT_LOCK(unit);
        rv = _bcm_trx_vlan_port_protocol_action_traverse(unit, cb, user_data);
        PORT_UNLOCK(unit);
    }
#endif /* BCM_TRX_SUPPORT */
    return (rv);
}


/*
 * Mac based vlans
 */
int
bcm_esw_vlan_mac_add(int unit, bcm_mac_t mac, bcm_vlan_t vid, int prio)
{

    VLAN_CHK_ID(unit, vid);
    if (prio < BCM_PRIO_MIN || prio > BCM_PRIO_MAX) {
        return BCM_E_PARAM;
    }

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        bcm_vlan_action_set_t action;

        if (!soc_feature(unit, soc_feature_vlan_action)
            || !soc_feature(unit, soc_feature_mac_based_vlan)) {
            return BCM_E_UNAVAIL;
        }
        prio &= ~BCM_PRIO_DROP_FIRST;

        if ((prio & ~BCM_VLAN_XLATE_PRIO_MASK) != 0) {
            return BCM_E_PARAM;
        }

        bcm_vlan_action_set_t_init(&action);
        action.new_outer_vlan = vid;
        action.new_inner_vlan = 0;
        action.priority = prio;
        action.dt_outer_prio = bcmVlanActionReplace;
        action.ot_outer_prio = bcmVlanActionReplace;
        action.it_outer      = bcmVlanActionAdd;
        action.it_inner_prio = bcmVlanActionNone;
        action.ut_outer      = bcmVlanActionAdd;
        if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            action.ut_outer_pkt_prio = bcmVlanActionAdd;
        }

#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            return _bcm_td3_vlan_mac_action_add(unit, mac, &action);
        } else
#endif
        {
        return _bcm_trx_vlan_mac_action_add(unit, mac, &action);
    }
    }
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        vlan_xlate_entry_t vxent;
        prio &= ~BCM_PRIO_DROP_FIRST;

        if ((prio & ~BCM_VLAN_XLATE_PRIO_MASK) != 0) {
            return BCM_E_PARAM;
        }

        sal_memset(&vxent, 0, sizeof(vxent));
        soc_mem_mac_addr_set(unit, VLAN_XLATEm, (uint32 *)(&vxent), 
                       VLAN_MAC__MAC_ADDRf, mac);
        soc_VLAN_XLATEm_field32_set(unit, &vxent, VLAN_MAC__OVIDf, vid);
        soc_VLAN_XLATEm_field32_set(unit, &vxent, VLAN_MAC__OPRIf, prio);
        soc_VLAN_XLATEm_field32_set(unit, &vxent, VALIDf, 1);

        return soc_mem_insert(unit, VLAN_XLATEm, MEM_BLOCK_ALL, &vxent);
    } else
#endif  /* BCM_TRIUMPH3_SUPPORT */
    {
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        vlan_mac_entry_t  vment;
        prio &= ~BCM_PRIO_DROP_FIRST;

        if ((prio & ~BCM_VLAN_XLATE_PRIO_MASK) != 0) {
            return BCM_E_PARAM;
        }

        sal_memset(&vment, 0, sizeof(vment));
        soc_VLAN_MACm_mac_addr_set(unit, &vment, MAC_ADDRf, mac);
        soc_VLAN_MACm_field32_set(unit, &vment, VLAN_IDf, vid);
        soc_VLAN_MACm_field32_set(unit, &vment, PRIf, prio);
        soc_VLAN_MACm_field32_set(unit, &vment, VALIDf, 1);

        return soc_mem_insert(unit, VLAN_MACm, MEM_BLOCK_ALL, &vment);
    }
#endif
    }
    return BCM_E_UNAVAIL;
}

int
bcm_esw_vlan_mac_delete(int unit, bcm_mac_t mac)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        if (!soc_feature(unit, soc_feature_vlan_action)
            || !soc_feature(unit, soc_feature_mac_based_vlan)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            return _bcm_td3_vlan_mac_delete(unit, mac);
        } else
#endif
        {
        return _bcm_trx_vlan_mac_delete(unit, mac);
    }
    }
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        vlan_xlate_entry_t vxent;
        int     rv;

        sal_memset(&vxent, 0, sizeof(vxent));
        soc_mem_mac_addr_set(unit, VLAN_XLATEm, (uint32 *)(&vxent),
                       VLAN_MAC__MAC_ADDRf, mac);
        rv = soc_mem_delete(unit, VLAN_XLATEm, MEM_BLOCK_ALL, &vxent);
        if (rv == SOC_E_NOT_FOUND) {
            rv = SOC_E_NONE;
        }
        return rv;
    } else
#endif  /* BCM_TRIUMPH3_SUPPORT */
    {
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        vlan_mac_entry_t  vment;
        int     rv;

        sal_memset(&vment, 0, sizeof(vment));
        soc_VLAN_MACm_mac_addr_set(unit, &vment, MAC_ADDRf, mac);
        rv = soc_mem_delete(unit, VLAN_MACm, MEM_BLOCK_ALL, &vment);
        if (rv == SOC_E_NOT_FOUND) {
            rv = SOC_E_NONE;
        }
        return rv;
    }
#endif
    }
    return BCM_E_UNAVAIL;
}


int
bcm_esw_vlan_mac_delete_all(int unit)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        if (!soc_feature(unit, soc_feature_vlan_action)
            || !soc_feature(unit, soc_feature_mac_based_vlan)) {            
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            return _bcm_td3_vlan_mac_delete_all(unit);
        } else
#endif
        {
        return _bcm_trx_vlan_mac_delete_all(unit);
    }
    }
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return soc_mem_clear(unit, VLAN_XLATEm, MEM_BLOCK_ALL, 0);
    } else
#endif  /* BCM_TRIUMPH3_SUPPORT */
    {
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        return soc_mem_clear(unit, VLAN_MACm, MEM_BLOCK_ALL, 0);
    }
#endif
    }
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vlan_mac_action_add
 * Purpose:
 *     Add an association from MAC address to VLAN actions, which are
 *             used for VLAN tag/s assignment to untagged packets.
 *      
 * Parameters:
 *      unit    - (IN) BCM device number.
 *      mac     - (IN) Mac address. 
 *      action    (IN) Action for outer and inner tag
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_mac_action_add(int unit, bcm_mac_t mac, bcm_vlan_action_set_t *action)
{
#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)
        && soc_feature(unit, soc_feature_mac_based_vlan)) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            return _bcm_td3_vlan_mac_action_add(unit, mac, action);
        } else
#endif
        {
        return _bcm_trx_vlan_mac_action_add(unit, mac, action);
    }
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vlan_mac_action_get
 * Purpose:
 *    Retrive an association from MAC address to VLAN actions, which
 *    are used for VLAN tag assignment to untagged packets.
 *      
 * Parameters:
 *      unit    - (IN) BCM device number.
 *      mac     - (IN) Mac address. 
 *      action    (OUT) Action for outer and inner tag
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_mac_action_get(int unit, bcm_mac_t mac, bcm_vlan_action_set_t *action)
{
#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)
        && soc_feature(unit, soc_feature_mac_based_vlan)) {        

        bcm_vlan_action_set_t_init(action);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            return _bcm_td3_vlan_mac_action_get(unit, mac, action);
        } else
#endif
        {
        return _bcm_trx_vlan_mac_action_get(unit, mac, action);
    }
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vlan_mac_action_delete
 * Purpose:
 *    Remove an association from MAC address to VLAN actions, which
 *    are used for VLAN tag assignment to untagged packets.
 *      
 * Parameters:
 *      unit    - (IN) BCM device number.
 *      mac     - (IN) Mac address. 
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_mac_action_delete(int unit, bcm_mac_t mac)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)
        && soc_feature(unit, soc_feature_mac_based_vlan)) {        
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            return _bcm_td3_vlan_mac_delete(unit, mac);
        } else
#endif
        {
        return _bcm_trx_vlan_mac_delete(unit, mac);
    }
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vlan_mac_action_delete_all
 * Purpose:
 *    Remove all MAC addresses to VLAN actions associations.
 *    Mac to VLAN actions are used for VLAN tag assignment to untagged packets.
 *      
 * Parameters:
 *      unit    - (IN) BCM device number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_mac_action_delete_all(int unit)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)
        && soc_feature(unit, soc_feature_mac_based_vlan)) {        
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            return _bcm_td3_vlan_mac_delete_all(unit);
        } else
#endif
        {
        return _bcm_trx_vlan_mac_delete_all(unit);
    }
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function   :
 *      bcm_vlan_mac_action_traverse
 * Description   :
 *      Traverse over vlan mac actions, which are used for VLAN
 *      tag/s assignment to untagged packets.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) Call back function
 *      user_data       (IN) User provided data to pass to a call back
 */
int 
bcm_esw_vlan_mac_action_traverse(int unit, 
                                 bcm_vlan_mac_action_traverse_cb cb, 
                                 void *user_data)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)
        && soc_feature(unit, soc_feature_mac_based_vlan)) {        
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            return _bcm_td3_vlan_mac_action_traverse(unit, cb, user_data);
        } else
#endif
        {
        return _bcm_trx_vlan_mac_action_traverse(unit, cb, user_data);
    }
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}

#ifdef BCM_FIREBOLT_SUPPORT
#ifdef BCM_WARM_BOOT_SUPPORT 
STATIC int
_bcm_fb_vlan_translate_reload(int unit, int xtable)
{
    soc_mem_t           mem;
    int                 addvid, index_min, index_max, index_count, i, rv;
    uint32              *vtcachep, vtcache;
    vlan_xlate_entry_t  *vtentries, *vtent;
    uint32              ve_valid, ve_port, ve_vid, ve_add;

    if (!soc_feature(unit, soc_feature_vlan_translation)) {
        return BCM_E_NONE;
    }

    switch (xtable) {
    case BCM_VLAN_XLATE_ING:
        mem = VLAN_XLATEm;
        addvid = 0;
        break;
    case BCM_VLAN_XLATE_EGR:
        mem = EGR_VLAN_XLATEm;
        addvid = -1;
        break;
    case BCM_VLAN_XLATE_DTAG:
        mem = VLAN_XLATEm;
        addvid = 1;
        break;
    default:
        return BCM_E_INTERNAL;
    }

    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);
    index_count = soc_mem_index_count(unit, mem);

    soc_mem_lock(unit, mem);

    /* get vt cache, allocate if necessary */
    if (mem == EGR_VLAN_XLATEm) {
        vtcachep = vlan_info[unit].egr_trans;
    } else {
        vtcachep = vlan_info[unit].ing_trans;
    }
    if (vtcachep == NULL) {
        vtcachep = sal_alloc(sizeof(*vtcachep) * (index_max+1),
                             "vlan trans cache");
        if (vtcachep == NULL) {
            soc_mem_unlock(unit, mem);
            return BCM_E_MEMORY;
        }
        sal_memset(vtcachep, 0, sizeof(*vtcachep) * (index_max+1));
        if (mem == EGR_VLAN_XLATEm) {
            vlan_info[unit].egr_trans = vtcachep;
        } else {
            vlan_info[unit].ing_trans = vtcachep;
        }
    }

    vtentries = soc_cm_salloc(unit, index_count * sizeof(*vtentries),
                              "vlan trans dma");
    if (vtentries == NULL) {
        soc_mem_unlock(unit, mem);
        return BCM_E_MEMORY;
    }
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, index_min, index_max,
                            vtentries);
    if (rv < 0) {
        soc_cm_sfree(unit, vtentries);
        sal_free(vtcachep);
        if (mem == EGR_VLAN_XLATEm) {
            vlan_info[unit].egr_trans = NULL;
        } else {
            vlan_info[unit].ing_trans = NULL;
        }
        soc_mem_unlock(unit, mem);
        return rv;
    }
    for (i = 0; i < index_count; i++) {
        vtent = soc_mem_table_idx_to_pointer(unit, mem, vlan_xlate_entry_t *,
                                             vtentries, i);
        vtcache = 0;
        ve_valid = soc_mem_field32_get(unit, mem, vtent, VALIDf);
        if (ve_valid) {
            if (soc_mem_field_valid(unit, mem, KEY_TYPEf) && 
                (soc_mem_field32_get(unit, mem, vtent, KEY_TYPEf) != 0)) {
                continue;
            } 
            if (soc_mem_field_valid(unit, mem, ENTRY_TYPEf) && 
                (soc_mem_field32_get(unit, mem, vtent, ENTRY_TYPEf) != 0)) {
                continue;
            }
            BCM_VTCACHE_VALID_SET(vtcache, 1);
            if (soc_mem_field_valid(unit, mem, DST_PORTf)) {
                ve_port = soc_mem_field32_get(unit, mem, vtent, DST_PORTf);
            } else if (soc_mem_field_valid(unit, mem, PORT_NUMf)) {
                ve_port = soc_mem_field32_get(unit, mem, vtent, PORT_NUMf);
            } else {
                ve_port = soc_mem_field32_get(unit, mem, vtent, PORTf);
            }
            BCM_VTCACHE_PORT_SET(vtcache, ve_port);
            ve_vid = soc_mem_field32_get(unit, mem, vtent, OLD_VLAN_IDf);
            BCM_VTCACHE_VID_SET(vtcache, ve_vid);
            if (addvid >= 0) {
                if (soc_mem_field_valid(unit, mem, ADD_VIDf)) {
                    ve_add = soc_mem_field32_get(unit, mem, vtent, ADD_VIDf);
                    BCM_VTCACHE_ADD_SET(vtcache, ve_add);
                }
            }
        }
        vtcachep[index_min + i] = vtcache;
    }
    soc_cm_sfree(unit, vtentries);
    soc_mem_unlock(unit, mem);
    return BCM_E_NONE;
}
#else
#define _bcm_fb_vlan_translate_reload(unit, xtable)    (BCM_E_NONE)
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif /* BCM_FIREBOLT_SUPPORT */

/*
 * Function:
 *      bcm_esw_vlan_translate_get
 * Purpose:
 *      Get vlan translation
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - port numebr
 *      old_vid - Old VLAN ID to has translation for
 *      new_vid - New VLAN ID that packet will get
 *      prio    - Priority
 * Returns:
 *      BCM_E_NONE - Translation found, new_vid nad prio will have the values.
 *      BCM_E_NOT_FOUND - Translation does not exist
 *      BCM_E_XXX  - Other error
 * Notes:
 *      None.
 */

int 
bcm_esw_vlan_translate_get (int unit, bcm_port_t port, bcm_vlan_t old_vid,
                            bcm_vlan_t *new_vid, int *prio)
{

    bcm_module_t        modid;
    bcm_trunk_t         tgid = -1;
    int                 id = -1;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    CHECK_INIT(unit);
    VLAN_CHK_ID(unit, old_vid);
    if (NULL == new_vid) {
        return BCM_E_PARAM;
    }
    if (NULL == prio) {
        return BCM_E_PARAM;
    }
    
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        bcm_gport_t gport;
        bcm_vlan_action_set_t action;

        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
        if (BCM_GPORT_IS_SET(port)) {
            gport = port;
        } else {
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_gport_get(unit, port, &gport));
        }
        bcm_vlan_action_set_t_init(&action);

        BCM_IF_ERROR_RETURN
            (_bcm_trx_vlan_translate_action_get(unit, gport,
                                               bcmVlanTranslateKeyPortOuter,
                                               old_vid, 0, &action));

        if (bcmVlanActionReplace == action.ot_outer){
            *new_vid = action.new_outer_vlan;
            *prio = action.priority;
            return BCM_E_NONE;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_trx_vlan_translate_action_get(unit, gport, 
                                               bcmVlanTranslateKeyPortInner,
                                               0, old_vid, &action));
        if (bcmVlanActionAdd == action.it_outer) {
            *new_vid = action.new_inner_vlan;
            *prio = action.priority;
            return BCM_E_NONE;
        }

        return BCM_E_NOT_FOUND;
    }
#endif /* BCM_TRX_SUPPORT */

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if (-1 != tgid || -1 != id) {
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_stk_my_modid_get(unit, &modid));
    }

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_vlan_translate_get(unit, port, old_vid, new_vid, prio,
                                          BCM_VLAN_XLATE_ING);
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    return BCM_E_UNAVAIL;
}



/*
 * Function:
 *      bcm_esw_vlan_translate_add
 * Purpose:
 *      Add vlan translation
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - port numebr
 *      old_vid - Old VLAN ID to has translation for
 *      new_vid - New VLAN ID that packet will get
 *      prio    - Priority
 * Returns:
 *      BCM_E_NONE - Translation found, new_vid nad prio will have the values.
 *      BCM_E_NOT_FOUND - Translation does not exist
 *      BCM_E_XXX  - Other error
 * Notes:
 *      None.
 */
int
bcm_esw_vlan_translate_add(int unit, int port, bcm_vlan_t old_vid,
                           bcm_vlan_t new_vid, int prio)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid = -1;
    int                 id = -1; 

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    CHECK_INIT(unit);
    VLAN_CHK_ID(unit, old_vid);
    VLAN_CHK_ID(unit, new_vid);

    /* XGS does not support translation of VLAN ID 0 */
    if (old_vid == 0) {
        return BCM_E_PARAM;
    }

    if ((prio != -1) && ((prio & BCM_PRIO_MASK) > BCM_PRIO_MAX)) {
        return BCM_E_PARAM;
    }

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        bcm_vlan_action_set_t action;
        bcm_gport_t gport;

        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
        if (BCM_GPORT_IS_SET(port)) {
            gport = port;
        } else {
            if (!SOC_PORT_VALID(unit, port)) { 
                return BCM_E_PORT; 
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_gport_get(unit, port, &gport));
        }

        /* add an entry for inner-tagged packets */
        bcm_vlan_action_set_t_init(&action);
        action.new_outer_vlan = new_vid;
        action.priority = prio;
        action.it_outer = bcmVlanActionAdd;
        action.it_inner = bcmVlanActionDelete;
        action.it_inner_prio = bcmVlanActionNone;

        BCM_IF_ERROR_RETURN
            (_bcm_trx_vlan_translate_action_add(unit, gport, 
                                               bcmVlanTranslateKeyPortInner,
                                               0, old_vid, &action));

        /* add an entry for outer-tagged packets */
        bcm_vlan_action_set_t_init(&action);
        action.new_outer_vlan = new_vid;
        action.priority = prio;
        action.ot_outer      = bcmVlanActionReplace;
        action.ot_outer_prio = bcmVlanActionReplace;
        action.dt_outer      = bcmVlanActionReplace;
        action.dt_outer_prio = bcmVlanActionReplace;

        BCM_IF_ERROR_RETURN
            (_bcm_trx_vlan_translate_action_add(unit, gport,
                                               bcmVlanTranslateKeyPortOuter,
                                               old_vid, 0, &action));
        return BCM_E_NONE;
    }
#endif /* BCM_TRX_SUPPORT */

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if (-1 != tgid || -1 != id) {
            return BCM_E_PORT;
        }
    } else {
        if (SOC_IS_RAVEN(unit)) {
           if (port != -1) {
              if (!SOC_PORT_VALID(unit, port)) {
                  return BCM_E_PORT;
              }
           }
        } else {
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_stk_my_modid_get(unit, &modid));
    }

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_vlan_translate_add(unit, port, old_vid, new_vid, prio,
                                          BCM_VLAN_XLATE_ING);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vlan_translate_delete
 * Purpose:
 *      Delete vlan translation
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - port numebr
 *      old_vid - Old VLAN ID to delete translation for
 * Returns:
 *      BCM_E_NONE - Translation found, new_vid nad prio will have the values.
 *      BCM_E_NOT_FOUND - Translation does not exist
 *      BCM_E_XXX  - Other error
 * Notes:
 *      None.
 */

int
bcm_esw_vlan_translate_delete(int unit, int port, bcm_vlan_t old_vid)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid = -1;
    int                 id = -1;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    CHECK_INIT(unit);
    VLAN_CHK_ID(unit, old_vid);

    /* XGS does not support translation of VLAN ID 0 */
    if (old_vid == 0) {
        return BCM_E_PARAM;
    }

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        bcm_gport_t gport;
        int         rv1, rv2;

        rv1 = rv2 = BCM_E_NONE;
        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
        if (BCM_GPORT_IS_SET(port)) {
            gport = port;
        } else {
            if (!SOC_PORT_VALID(unit, port)) { 
                return BCM_E_PORT; 
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_gport_get(unit, port, &gport));
        }
        rv1 = _bcm_trx_vlan_translate_action_delete(unit, gport, 
                                                  bcmVlanTranslateKeyPortInner,
                                                  0, old_vid);
        if (BCM_SUCCESS(rv1) || (rv1 == BCM_E_NOT_FOUND)) {
            rv2 = _bcm_trx_vlan_translate_action_delete(unit, gport, 
                                                  bcmVlanTranslateKeyPortOuter,
                                                  old_vid, 0);
        } else {
            return (rv1);
        }

        if (BCM_SUCCESS(rv1) && (rv2 == BCM_E_NOT_FOUND)) {
            return (rv1);
        }
        return (rv2);
    }
#endif /* BCM_TRX_SUPPORT */

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if (-1 != tgid || -1 != id) {
            return BCM_E_PORT;
        }
    } else {
        if (SOC_IS_RAVEN(unit)) {
            if (port != -1) {
                if (!SOC_PORT_VALID(unit, port)) {
                    return BCM_E_PORT; 
                }
            }
        } else {
            if (!SOC_PORT_VALID(unit, port)) {
                  return BCM_E_PORT;
            }
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_stk_my_modid_get(unit, &modid));
    }

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_vlan_translate_delete(unit, port, old_vid,
                                             BCM_VLAN_XLATE_ING);
    }
#endif

    return BCM_E_UNAVAIL;
}


int
bcm_esw_vlan_translate_delete_all(int unit)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    CHECK_INIT(unit);

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
        return _bcm_trx_vlan_translate_action_delete_all(unit);
    }
#endif /* BCM_TRX_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        int rv;

        rv = _bcm_fb_vlan_translate_delete(unit, -1, BCM_VLAN_NONE,
                                             BCM_VLAN_XLATE_ING);
        if (BCM_SUCCESS(rv) || (rv == BCM_E_NOT_FOUND)) {
            return BCM_E_NONE;
        }

        return rv;
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function   :
 *      bcm_vlan_translate_action_add
 * Description   :
 *      Add an entry to ingress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) Generic port
 *      key_type        (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 *      action          (IN) Action for outer and inner tag
 */
int
bcm_esw_vlan_translate_action_add(int unit,
                                  bcm_gport_t port,
                                  bcm_vlan_translate_key_t key_type,
                                  bcm_vlan_t outer_vlan,
                                  bcm_vlan_t inner_vlan,
                                  bcm_vlan_action_set_t *action)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    CHECK_INIT(unit);

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        return _bcm_trx_vlan_translate_action_add(unit, port, 
                                                 key_type, outer_vlan,
                                                 inner_vlan, action);
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function   :
 *      bcm_vlan_translate_action_get
 * Description   :
 *      Get an entry to ingress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) Generic port
 *      key_type        (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 *      action          (OUT) Action for outer and inner tag
 */
int 
bcm_esw_vlan_translate_action_get (int unit, bcm_gport_t port,
                                   bcm_vlan_translate_key_t key_type,
                                   bcm_vlan_t outer_vlan,
                                   bcm_vlan_t inner_vlan,
                                   bcm_vlan_action_set_t *action)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    CHECK_INIT(unit);

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        bcm_vlan_action_set_t_init(action);
        return _bcm_trx_vlan_translate_action_get(unit, port, key_type, 
                                                 outer_vlan, inner_vlan, 
                                                 action);
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}


/*
 * Function   :
 *      _bcm_trx_vlan_translate_action_traverse_int_cb
 * Description   :
 *      Internal call back function to call user provided call back 
 *      entries in Triumph and call given call back.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      trvs_info       (IN) Traverse structure that contain all relevant info
 */
int 
_bcm_esw_vlan_translate_action_traverse_int_cb(int unit, void *trv_info, 
                                               int *stop)
{
    _bcm_vlan_translate_traverse_t          *trvs_str;
    _translate_action_traverse_cb_t   *usr_cb_st;

    if ((NULL == trv_info) || (NULL == stop)){
        return BCM_E_PARAM;
    }
    trvs_str = (_bcm_vlan_translate_traverse_t *)trv_info;
    usr_cb_st = (_translate_action_traverse_cb_t *)trvs_str->user_cb_st;

    *stop = FALSE;
    return usr_cb_st->usr_cb(unit, trvs_str->gport, trvs_str->key_type,
                             trvs_str->outer_vlan, trvs_str->inner_vlan,
                             trvs_str->action, trvs_str->user_data);  
}

/*
 * Function   :
 *      _bcm_esw_vlan_translate_egress_action_traverse_int_cb
 * Description   :
 *      Internal call back function to call user provided call back
 *      entries in Triumph and call given call back.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      trvs_info       (IN) Traverse structure that contain all relevant info
 */
int 
_bcm_esw_vlan_translate_egress_action_traverse_int_cb(int unit, void *trv_info,
                                                      int *stop)
{
    _bcm_vlan_translate_traverse_t              *trvs_str;
    _translate_egress_action_traverse_cb_t      *usr_cb_st;

    if ((NULL == trv_info) || (NULL == stop)){
        return BCM_E_PARAM;
    }
    trvs_str = (_bcm_vlan_translate_traverse_t *)trv_info;
    usr_cb_st = (_translate_egress_action_traverse_cb_t *)trvs_str->user_cb_st;

    *stop = FALSE;

    return usr_cb_st->usr_cb(unit, trvs_str->port_class,
                             trvs_str->outer_vlan, 
                             trvs_str->inner_vlan, 
                             trvs_str->action,
                             trvs_str->user_data);
}

/*
 * Function   :
 *      _bcm_esw_vlan_translate_egress_action_extended_traverse_int_cb
 * Description   :
 *      Internal call back function to call user provided call back
 *      entries in Triumph and call given call back.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      trvs_info       (IN) Traverse structure that contain all relevant info
 */
#ifdef BCM_TRIDENT3_SUPPORT
int
_bcm_esw_vlan_translate_egress_action_extended_traverse_int_cb(int unit, void *trv_info,
                                                      int *stop)
{
    _bcm_vlan_translate_traverse_t              *trvs_str;
    _translate_egress_action_extended_traverse_cb_t      *usr_cb_st;
    bcm_vlan_translate_egress_key_config_t key;
    bcm_vlan_translate_egress_action_set_t action;
    bcm_vlan_translate_egress_key_config_t_init(&key);
    bcm_vlan_translate_egress_action_set_t_init(&action);

    if ((NULL == trv_info) || (NULL == stop)){
        return BCM_E_PARAM;
    }
    trvs_str = (_bcm_vlan_translate_traverse_t *)trv_info;
    usr_cb_st = (_translate_egress_action_extended_traverse_cb_t *)trvs_str->user_cb_st;

    *stop = FALSE;

    key.port       = trvs_str->port_class;
    key.outer_vlan = trvs_str->outer_vlan;
    key.inner_vlan = trvs_str->inner_vlan;
    key.key_type = trvs_str->key_type;
    action.vlan_action = *trvs_str->action;

    return usr_cb_st->usr_cb(unit,
                             &key,
                             &action,
                             trvs_str->user_data);
}
#endif /* BCM_TRIDENT3_SUPPORT */
/*
 * Function   :
 *      _bcm_esw_vlan_translate_action_range_traverse_int_cb
 * Description   :
 *      Internal call back function to call user provided call back 
 *      entries in Triumph and call given call back.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      trvs_info       (IN) Traverse structure that contain all relevant info
 */
int 
_bcm_esw_vlan_translate_action_range_traverse_int_cb(int unit, void *trv_info,
                                                     int *stop)
{
    _bcm_vlan_translate_traverse_t  *trvs_str;
    _translate_action_range_traverse_cb_t *usr_cb_st;

    if ((NULL == trv_info) || (NULL == stop)){
        return BCM_E_PARAM;
    }
    trvs_str = (_bcm_vlan_translate_traverse_t *)trv_info;
    usr_cb_st = (_translate_action_range_traverse_cb_t *)trvs_str->user_cb_st;

    *stop = FALSE;
    return usr_cb_st->usr_cb(unit, trvs_str->gport, trvs_str->outer_vlan, 
                             trvs_str->outer_vlan_high, trvs_str->inner_vlan, 
                             trvs_str->inner_vlan_high, trvs_str->action, 
                             trvs_str->user_data);  
}

/*
 * Function   :
 *      _bcm_esw_vlan_translate_range_traverse_int_cb
 * Description   :
 *      Internal call back function to call user provided call back 
 *      entries and call given call back.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      trvs_info       (IN) Traverse structure that contain all relevant info
 */
int 
_bcm_esw_vlan_translate_range_traverse_int_cb(int unit, void *trv_info,
                                                     int *stop)
{
    _bcm_vlan_translate_traverse_t  *trvs_str;
    _translate_range_traverse_cb_t *usr_cb_st;

    if ((NULL == trv_info) || (NULL == stop)){
        return BCM_E_PARAM;
    }
    trvs_str = (_bcm_vlan_translate_traverse_t *)trv_info;
    usr_cb_st = (_translate_range_traverse_cb_t *)trvs_str->user_cb_st;

    *stop = FALSE;
    if ((trvs_str->key_type == bcmVlanTranslateKeyPortOuter) &&
            (trvs_str->action->dt_outer == bcmVlanActionReplace) &&
            (trvs_str->action->dt_inner == bcmVlanActionNone) &&
            (trvs_str->action->ot_outer == bcmVlanActionReplace) &&
            (trvs_str->action->ot_inner == bcmVlanActionNone)) {
        return usr_cb_st->usr_cb(unit, trvs_str->gport, trvs_str->outer_vlan, 
                trvs_str->outer_vlan_high, 
                trvs_str->action->new_outer_vlan,
                trvs_str->action->priority, trvs_str->user_data);  
    } else {
        return BCM_E_NONE;
    }
}

/*
 * Function   :
 *      _bcm_esw_vlan_translate_traverse_int_cb
 * Description   :
 *      Internal call back function to call user provided call back 
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      trvs_info       (IN) Traverse structure that contain all relevant info
 */
int 
_bcm_esw_vlan_translate_traverse_int_cb(int unit, void *trv_info, int *stop)
{
    _bcm_vlan_translate_traverse_t  *trvs_str;
    _translate_traverse_cb_t        *usr_cb_st;
    bcm_vlan_t                      old_vlan, new_vlan;

    if ((NULL == trv_info) || (NULL == stop)){
        return BCM_E_PARAM;
    }
    trvs_str = (_bcm_vlan_translate_traverse_t *)trv_info;
    usr_cb_st = (_translate_traverse_cb_t *)trvs_str->user_cb_st;
    if (BCM_VLAN_INVALID != trvs_str->outer_vlan) {
        old_vlan = trvs_str->outer_vlan;
    } else {
        old_vlan = trvs_str->inner_vlan;
    }

    if (BCM_VLAN_INVALID != trvs_str->action->new_outer_vlan) {
        new_vlan = trvs_str->action->new_outer_vlan;
    } else {
        new_vlan = trvs_str->action->new_inner_vlan;
    }

    *stop = FALSE;
    return usr_cb_st->usr_cb(unit, trvs_str->gport, old_vlan, new_vlan, 
                             trvs_str->action->priority, trvs_str->user_data); 
}


/*
 * Function   :
 *      _bcm_esw_vlan_translate_egress_traverse_int_cb
 * Description   :
 *      Internal call back function to call user provided call back 
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      trvs_info       (IN) Traverse structure that contain all relevant info
 */
int 
_bcm_esw_vlan_translate_egress_traverse_int_cb(int unit, void *trv_info, 
                                               int *stop)
{
    _bcm_vlan_translate_traverse_t  *trvs_str;
    _translate_egress_traverse_cb_t *usr_cb_st;
    bcm_vlan_t                      old_vlan, new_vlan;
    bcm_gport_t gport = 0;

    if ((NULL == trv_info) || (NULL == stop)){
        return BCM_E_PARAM;
    }
    trvs_str = (_bcm_vlan_translate_traverse_t *)trv_info;
    usr_cb_st = (_translate_egress_traverse_cb_t *)trvs_str->user_cb_st;
    if (BCM_VLAN_INVALID != trvs_str->outer_vlan) {
        old_vlan = trvs_str->outer_vlan;
    } else {
        old_vlan = trvs_str->inner_vlan;
    }

    if (BCM_VLAN_INVALID != trvs_str->action->new_outer_vlan) {
        new_vlan = trvs_str->action->new_outer_vlan;
    } else {
        new_vlan = trvs_str->action->new_inner_vlan;
    }

    *stop = FALSE;
    if (!BCM_GPORT_IS_SET(trvs_str->port_class)) {
    if (bcm_esw_port_gport_get(unit, trvs_str->port_class, &gport) != 
        BCM_E_NONE) {
        return BCM_E_PORT; 
    }
    }

    return usr_cb_st->usr_cb(unit, gport, old_vlan, new_vlan, 
                             trvs_str->action->priority, trvs_str->user_data); 
}

/*
 * Function   :
 *      _bcm_esw_vlan_dtag_traverse_int_cb
 * Description   :
 *      Internal call back function to call user provided call back 
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      trvs_info       (IN) Traverse structure that contain all relevant info
 */
int 
_bcm_esw_vlan_dtag_traverse_int_cb(int unit, void *trv_info, int *stop)
{
    _bcm_vlan_translate_traverse_t  *trvs_str;
    _dtag_traverse_cb_t             *usr_cb_st;
    bcm_vlan_t                      old_vlan, new_vlan;

    if ((NULL == trv_info) || (NULL == stop)){
        return BCM_E_PARAM;
    }
    trvs_str = (_bcm_vlan_translate_traverse_t *)trv_info;
    usr_cb_st = (_dtag_traverse_cb_t *)trvs_str->user_cb_st;
    if (BCM_VLAN_INVALID != trvs_str->outer_vlan) {
        old_vlan = trvs_str->outer_vlan;
    } else {
        old_vlan = trvs_str->inner_vlan;
    }

    if (BCM_VLAN_INVALID != trvs_str->action->new_outer_vlan) {
        new_vlan = trvs_str->action->new_outer_vlan;
    } else {
        new_vlan = trvs_str->action->new_inner_vlan;
    }
    
    *stop = FALSE;
    return usr_cb_st->usr_cb(unit, trvs_str->gport, old_vlan, new_vlan, 
                             trvs_str->action->priority, trvs_str->user_data); 
}

/*
 * Function   :
 *      _bcm_esw_vlan_dtag_range_traverse_int_cb
 * Description   :
 *      Internal call back function to call user provided call back 
 *      entries and call given call back.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      trvs_info       (IN) Traverse structure that contain all relevant info
 */
int 
_bcm_esw_vlan_dtag_range_traverse_int_cb(int unit, void *trv_info,
                                                     int *stop)
{
    _bcm_vlan_translate_traverse_t  *trvs_str;
    _dtag_range_traverse_cb_t *usr_cb_st;

    if ((NULL == trv_info) || (NULL == stop)){
        return BCM_E_PARAM;
    }
    trvs_str = (_bcm_vlan_translate_traverse_t *)trv_info;
    usr_cb_st = (_dtag_range_traverse_cb_t *)trvs_str->user_cb_st;

    *stop = FALSE;
    if ((trvs_str->key_type == bcmVlanTranslateKeyPortInner) &&
            (trvs_str->action->it_outer == bcmVlanActionAdd) &&
            (trvs_str->action->it_inner == bcmVlanActionNone)) {
        return usr_cb_st->usr_cb(unit, trvs_str->gport, trvs_str->inner_vlan, 
                trvs_str->inner_vlan_high, 
                trvs_str->action->new_outer_vlan,
                trvs_str->action->priority, trvs_str->user_data);  
    } else {
        return BCM_E_NONE;
    }
}

/*
 * Function:
 *     _bcm_esw_vlan_translate_entry_valid
 * Description:
 *      Check if given Vlan Translate entry is Valid
 * Parameters:
 *      unit         device number
 *      mem          memory to operate on
 *      vent         entry read from HW
 *      valid       (OUT) Entry valid indicator
 * Return:
 *     BCM_E_XXX
 */

STATIC int
_bcm_esw_vlan_translate_entry_valid(int unit, soc_mem_t mem, uint32 *vent, int *valid)
{
    uint32      fval = 0;

    if (INVALIDm == mem) {
        return BCM_E_MEMORY;
    }

    if ((NULL == vent) || (NULL == valid)) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TRX_SUPPORT) || defined (BCM_FIREBOLT_SUPPORT)
    if (SOC_IS_FBX(unit)) {

#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit) && (mem == VLAN_XLATE_EXTDm)) {
            /* both VALID_0 and VALID_1 fields need to be set */
            if ((!soc_mem_field32_get(unit, mem, vent, VALID_0f)) ||
                (!soc_mem_field32_get(unit, mem, vent, VALID_1f))) {
                *valid = FALSE;
                return (BCM_E_NONE);
            }

            /* qualify with key type: odd number */
            fval = soc_mem_field32_get(unit, mem, vent, KEY_TYPE_0f);
            fval &= 0x1;
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            if (mem == VLAN_XLATE_1_DOUBLEm ||
                mem == VLAN_XLATE_2_DOUBLEm ||
                mem == EGR_VLAN_XLATE_1_DOUBLEm ||
                mem == EGR_VLAN_XLATE_2_DOUBLEm) {
                /* both VALID_0 and VALID_1 fields need to be set */
                if ((!soc_mem_field32_get(unit, mem, vent, BASE_VALID_0f)) ||
                    (!soc_mem_field32_get(unit, mem, vent, BASE_VALID_1f))) {
                    *valid = FALSE;
                } else {
                    *valid = TRUE;
                }
            } else if (mem == VLAN_XLATE_1_SINGLEm ||
                mem == VLAN_XLATE_2_SINGLEm ||
                mem == EGR_VLAN_XLATE_1_SINGLEm ||
                mem == EGR_VLAN_XLATE_2_SINGLEm) {
                fval = soc_mem_field32_get(unit, mem, vent, BASE_VALIDf);
                *valid = fval ? TRUE : FALSE;
            } else {
                fval = soc_mem_field32_get(unit, mem, vent, VALIDf);
                *valid = fval ? TRUE : FALSE;
            }
            return (BCM_E_NONE);
        } else
#endif /* BCM_TRIDENT3_SUPPORT */
        {
            fval = soc_mem_field32_get(unit, mem, vent, VALIDf);
        }
    } 
#endif /* BCM_TRX_SUPPORT || BCM_FIREBOLT_SUPPORT */

    *valid = fval ? TRUE : FALSE;

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_esw_vlan_translate_entry_parse
 * Description:
 *      Parser function to parse vlan translate entry into truverse structure
 *      will call per chip parser
 * Parameters:
 *      unit         device number
 *      mem          memory to operate on
 *      vent        entry read from HW
 *      trvs_info    Structure to fill 
 * Return:
 *     BCM_E_XXX
 */

STATIC int
_bcm_esw_vlan_translate_entry_parse(int unit, soc_mem_t mem, uint32 *vent, 
                                    _bcm_vlan_translate_traverse_t *trvs_info)
{
#ifdef BCM_TRX_SUPPORT

    if (SOC_IS_TRX(unit)) {
        return _bcm_trx_vlan_translate_parse(unit, mem, vent, trvs_info);
    }
#endif /* BCM_TRX_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_vlan_translate_parse(unit, mem, vent, trvs_info);
    } 
#endif /* BCM_FIREBOLT_SUPPORT */
    return (BCM_E_UNAVAIL);
}

/*
 * Function   :
 *      _bcm_esw_vlan_translate_traverse_mem
 * Description   :
 *      Traverse over all translate entries and call parse function 
 *      based on the unit
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) Call back function
 *      user_data       (IN) User provided data to pass to a call back
 */
int 
_bcm_esw_vlan_translate_traverse_mem(int unit, soc_mem_t mem, 
                                   _bcm_vlan_translate_traverse_t *trvs_info)
{
    /* Indexes to iterate over memories, chunks and entries */
    int             chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int             buf_size, chunksize, chnk_end;
    /* Buffer to store chunk of L2 table we currently work on */
    uint32          *vt_tbl_chnk;
    uint32          *vent;
    int             valid, stop, rv = BCM_E_NONE;
    
    if (INVALIDm == mem) {
        return BCM_E_MEMORY;
    }
    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    chunksize = soc_property_get(unit, spn_VLANDELETE_CHUNKS,
                                 VLAN_MEM_CHUNKS_DEFAULT);

    buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
    vt_tbl_chnk = soc_cm_salloc(unit, buf_size, "vlan translate traverse");
    if (NULL == vt_tbl_chnk) {
        return BCM_E_MEMORY;
    }

    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem); 
         chnk_idx <= mem_idx_max; 
         chnk_idx += chunksize) {
        sal_memset((void *)vt_tbl_chnk, 0, buf_size);

        chnk_idx_max = 
            ((chnk_idx + chunksize) <= mem_idx_max) ? 
            chnk_idx + chunksize - 1: mem_idx_max;

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, vt_tbl_chnk);
        if (SOC_FAILURE(rv)) {
            break;
        }
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx ++) {

            vent = 
                soc_mem_table_idx_to_pointer(unit, mem, uint32 *, 
                                             vt_tbl_chnk, ent_idx);
            rv = _bcm_esw_vlan_translate_entry_valid(unit, mem, vent, &valid);
            if (BCM_FAILURE(rv)) {
                break;
            }
            if (!valid) {
                continue;
            }
            rv = _bcm_esw_vlan_translate_entry_parse(unit, mem, vent, trvs_info);
            if (rv == BCM_E_NOT_FOUND) { /* for entries not exposed to user */ 
                continue;
            } else if (BCM_FAILURE(rv)) {
                break;
            }
            rv = trvs_info->int_cb(unit, trvs_info, &stop);
            if (BCM_FAILURE(rv)) {
                break;
            }
            /* 
               Only get cb function that will use internal traverse should mark
               stop condition as true upon a match 
             */
            if (stop) {     
                break;
            }
        }
    }
    soc_cm_sfree(unit, vt_tbl_chnk);
    return rv;        
}



/*
 * Function   :
 *      bcm_vlan_translate_traverse
 * Description   :
 *      Traverse over all translate entries and call given call back with 
 *      new vid and prio.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) Call back function
 *      user_data       (IN) User provided data to pass to a call back
 */
int
bcm_esw_vlan_translate_traverse(int unit, bcm_vlan_translate_traverse_cb cb, 
                            void *user_data)
{

        
    _bcm_vlan_translate_traverse_t      trvs_st;
    bcm_vlan_action_set_t               action;
    _translate_traverse_cb_t            usr_cb_st;
    int  rv;
    soc_mem_t                           mem = INVALIDm;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return (BCM_E_UNAVAIL);
    }
#endif

    if (!cb) {
        return (BCM_E_PARAM);
    }

    sal_memset(&trvs_st, 0, sizeof(_bcm_vlan_translate_traverse_t));
    sal_memset(&action, 0, sizeof(bcm_vlan_action_set_t));
    sal_memset(&usr_cb_st, 0, sizeof(_translate_traverse_cb_t));

    usr_cb_st.usr_cb = cb;

    trvs_st.user_data = user_data;
    trvs_st.action = &action;
    trvs_st.user_cb_st = (void *)&usr_cb_st;
    trvs_st.int_cb = _bcm_esw_vlan_translate_traverse_int_cb;

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        mem = VLAN_XLATEm;
        if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
            mem = VLAN_XLATE_1_DOUBLEm;
        }
        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        if (!soc_feature(unit, soc_feature_vlan_translation)) {
            return BCM_E_UNAVAIL;
        }
        mem = VLAN_XLATEm;
        if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
            mem = VLAN_XLATE_1_DOUBLEm;
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */
    if (INVALIDm == mem) {
        return BCM_E_UNAVAIL;
    }
    rv = _bcm_esw_vlan_translate_traverse_mem(unit, mem, &trvs_st);
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        /* for the extended table */
        mem = VLAN_XLATE_EXTDm;
        rv = _bcm_esw_vlan_translate_traverse_mem(unit, mem, &trvs_st);
    }
#endif
    return rv;
}


/*
 * Function   :
 *      bcm_vlan_translate_egress_traverse
 * Description   :
 *      Traverse over all translate entries and call given call back with 
 *      new vid and prio.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) Call back function
 *      user_data       (IN) User provided data to pass to a call back
 */
int
bcm_esw_vlan_translate_egress_traverse(int unit, 
                                   bcm_vlan_translate_egress_traverse_cb cb, 
                                   void *user_data)
{

        
    _bcm_vlan_translate_traverse_t      trvs_st;
    bcm_vlan_action_set_t               action;
    _translate_egress_traverse_cb_t     usr_cb_st;
    soc_mem_t                           mem = INVALIDm;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return (BCM_E_UNAVAIL);
    }
#endif

    if (!cb) {
        return (BCM_E_PARAM);
    }

    sal_memset(&trvs_st, 0, sizeof(_bcm_vlan_translate_traverse_t));
    sal_memset(&action, 0, sizeof(bcm_vlan_action_set_t));
    sal_memset(&usr_cb_st, 0, sizeof(_translate_egress_traverse_cb_t));

    usr_cb_st.usr_cb = cb;

    trvs_st.user_data = user_data;
    trvs_st.action = &action;
    trvs_st.user_cb_st = (void *)&usr_cb_st;
    trvs_st.int_cb = _bcm_esw_vlan_translate_egress_traverse_int_cb;


#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        soc_control_t           *sc = SOC_CONTROL(unit);
        
    if (!soc_feature(unit, soc_feature_vlan_action)) {
        return BCM_E_UNAVAIL;
    }

        /* If port class is changed system should not allow */
        /* To use this API since it leads to confused outcome */
        if (sc->soc_flags & SOC_F_XLATE_EGR_BLOCKED) {
            return BCM_E_CONFIG;
        }
        mem = EGR_VLAN_XLATEm;
        if (SOC_MEM_IS_VALID(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
            mem = EGR_VLAN_XLATE_1_DOUBLEm;
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        if (!soc_feature(unit, soc_feature_vlan_translation)) {
            return BCM_E_UNAVAIL;
        }
        mem = EGR_VLAN_XLATEm;
        if (SOC_MEM_IS_VALID(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
            mem = EGR_VLAN_XLATE_1_DOUBLEm;
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    if (INVALIDm == mem) {
        return BCM_E_UNAVAIL;
    }
    return _bcm_esw_vlan_translate_traverse_mem(unit, mem, &trvs_st);
}

/*
 * Function   :
 *      bcm_vlan_translate_egress_traverse
 * Description   :
 *      Traverse over all translate entries and call given call back with 
 *      new vid and prio.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) Call back function
 *      user_data       (IN) User provided data to pass to a call back
 */
int
bcm_esw_vlan_dtag_traverse(int unit, 
                       bcm_vlan_dtag_traverse_cb cb, 
                       void *user_data)
{
    _bcm_vlan_translate_traverse_t      trvs_st;
    bcm_vlan_action_set_t               action;
    _dtag_traverse_cb_t                 usr_cb_st;
    soc_mem_t                           mem = INVALIDm;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return (BCM_E_UNAVAIL);
    }
#endif

    if (!cb) {
        return (BCM_E_PARAM);
    }

    sal_memset(&trvs_st, 0, sizeof(_bcm_vlan_translate_traverse_t));
    sal_memset(&action, 0, sizeof(bcm_vlan_action_set_t));
    sal_memset(&usr_cb_st, 0, sizeof(_dtag_traverse_cb_t));

    usr_cb_st.usr_cb = cb;

    trvs_st.user_data = user_data;
    trvs_st.action = &action;
    trvs_st.user_cb_st = (void *)&usr_cb_st;
    trvs_st.int_cb = _bcm_esw_vlan_dtag_traverse_int_cb;

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        mem = VLAN_XLATEm;
        if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
            mem = VLAN_XLATE_1_DOUBLEm;
        }
        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        if (!soc_feature(unit, soc_feature_vlan_translation)) {
            return BCM_E_UNAVAIL;
        }
        mem = VLAN_XLATEm;
        if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
            mem = VLAN_XLATE_1_DOUBLEm;
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    if (INVALIDm == mem) {
        return BCM_E_UNAVAIL;
    }
    return _bcm_esw_vlan_translate_traverse_mem(unit, mem, &trvs_st);
}

/*
 * Function   :
 *      bcm_vlan_translate_action_traverse
 * Description   :
 *      Traverse over all translate entries and call given callback with 
 *      action structure
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) Call back function
 *      user_data       (IN) User provided data to pass to a call back
 */
int 
bcm_esw_vlan_translate_action_traverse(int unit, 
                                   bcm_vlan_translate_action_traverse_cb cb, 
                                   void *user_data)
{
    _bcm_vlan_translate_traverse_t      trvs_st;
    bcm_vlan_action_set_t               action;
    _translate_action_traverse_cb_t     usr_cb_st;
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return (BCM_E_UNAVAIL);
    }
#endif

    if (!cb) {
        return (BCM_E_PARAM);
    }

    sal_memset(&trvs_st, 0, sizeof(_bcm_vlan_translate_traverse_t));
    sal_memset(&action, 0, sizeof(bcm_vlan_action_set_t));
    sal_memset(&usr_cb_st, 0, sizeof(_translate_action_traverse_cb_t));

    usr_cb_st.usr_cb = cb;

    trvs_st.user_data = user_data;
    trvs_st.action = &action;
    trvs_st.user_cb_st = (void *)&usr_cb_st;
    
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        soc_mem_t mem = VLAN_XLATEm;
        if (SOC_IS_TRIDENT3X(unit)) {
            mem = VLAN_XLATE_1_DOUBLEm;
        }
        trvs_st.int_cb = _bcm_esw_vlan_translate_action_traverse_int_cb;
        rv = _bcm_esw_vlan_translate_traverse_mem(unit, mem, &trvs_st);
    }
#endif /* BCM_TRX_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        trvs_st.int_cb = _bcm_esw_vlan_translate_action_traverse_int_cb;
        /* for the extended table */
        rv = _bcm_esw_vlan_translate_traverse_mem(unit, VLAN_XLATE_EXTDm, 
                                &trvs_st);
    }
#endif

    return rv;
}

/*
 * Function   :
 *      bcm_vlan_translate_egress_action_traverse
 * Description   :
 *      Traverse over all translate entries and call given callback with 
 *      action structure
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) Call back function
 *      user_data       (IN) User provided data to pass to a call back
 */
int 
bcm_esw_vlan_translate_egress_action_traverse(int unit, 
                            bcm_vlan_translate_egress_action_traverse_cb cb, 
                            void *user_data)
{
    _bcm_vlan_translate_traverse_t          trvs_st;
    bcm_vlan_action_set_t                   action;
    _translate_egress_action_traverse_cb_t  usr_cb_st;
#ifdef BCM_TRX_SUPPORT
    soc_mem_t mem = EGR_VLAN_XLATEm;

    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
        mem = EGR_VLAN_XLATE_1_DOUBLEm;
    }
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return (BCM_E_UNAVAIL);
    }
#endif

    if (!cb) {
        return (BCM_E_PARAM);
    }

    sal_memset(&trvs_st, 0, sizeof(_bcm_vlan_translate_traverse_t));
    sal_memset(&action, 0, sizeof(bcm_vlan_action_set_t));
    sal_memset(&usr_cb_st, 0, sizeof(_translate_egress_action_traverse_cb_t));

    usr_cb_st.usr_cb = cb;

    trvs_st.user_data = user_data;
    trvs_st.action = &action;
    trvs_st.user_cb_st = (void *)&usr_cb_st;
    
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        trvs_st.int_cb = _bcm_esw_vlan_translate_egress_action_traverse_int_cb;
        BCM_IF_ERROR_RETURN(
            _bcm_esw_vlan_translate_traverse_mem(unit, mem, &trvs_st));
#if defined(BCM_TRIDENT3_SUPPORT) && defined(INCLUDE_L3)
        if (SOC_MEM_IS_VALID(unit, EGR_VLAN_XLATE_2_DOUBLEm)) {
            /* Now traverse EVXLT_2 */
            mem = EGR_VLAN_XLATE_2_DOUBLEm;
            return _bcm_esw_vlan_translate_traverse_mem(unit, mem, &trvs_st);
        }
#endif
        return BCM_E_NONE;
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcm_vlan_translate_action_delete
 * Purpose:
 *      Delete a vlan translate lookup entry.
 * Parameters:
 *      unit            (IN) BCM unit number
 *      port            (IN) Generic port 
 *      key_type        (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 */
int
bcm_esw_vlan_translate_action_delete(int unit,
                                     bcm_gport_t port,
                                     bcm_vlan_translate_key_t key_type,
                                     bcm_vlan_t outer_vlan,
                                     bcm_vlan_t inner_vlan)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return (BCM_E_UNAVAIL);
    }
#endif

    CHECK_INIT(unit);

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        return _bcm_trx_vlan_translate_action_delete(unit, port, 
                                                    key_type, outer_vlan,
                                                    inner_vlan);
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}

int
bcm_esw_vlan_translate_range_add(int unit, int port, bcm_vlan_t old_vid_low,
                                 bcm_vlan_t old_vid_high, bcm_vlan_t new_vid,
                                 int int_prio)
{
    bcm_gport_t     gport;
    
    CHECK_INIT(unit);
    VLAN_CHK_ID(unit, old_vid_low);
    VLAN_CHK_ID(unit, old_vid_high);
    VLAN_CHK_ID(unit, new_vid);
    if (old_vid_high < old_vid_low) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        gport = port;
    } else {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_gport_get(unit, port, &gport));
    }

    if (soc_feature(unit, soc_feature_vlan_translation_range)) {
#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(unit)) {
            bcm_vlan_action_set_t action;
            
            /* add vlan range and translate entry for inner-tagged packets */
            bcm_vlan_action_set_t_init(&action);
            action.new_outer_vlan = new_vid;
            action.priority = int_prio;
            action.it_outer = bcmVlanActionAdd;
            action.it_inner = bcmVlanActionDelete;
            action.it_inner_prio = bcmVlanActionNone;

            BCM_IF_ERROR_RETURN
                (_bcm_trx_vlan_translate_action_range_add(unit, gport,
                                        BCM_VLAN_INVALID, BCM_VLAN_INVALID, 
                                        old_vid_low, old_vid_high, 
                                        &action));

            

            /* Add vlan translate entry for outer-tagged packets.
             * This translate entry uses the range established above
             * for free on devices which only support one range space
             * between inner and outer tagged packets. */
            bcm_vlan_action_set_t_init(&action);
            action.new_outer_vlan = new_vid;
            action.priority      = int_prio;
            action.ot_outer      = bcmVlanActionReplace;
            action.ot_outer_prio = bcmVlanActionReplace;
            action.dt_outer      = bcmVlanActionReplace;
            action.dt_outer_prio = bcmVlanActionReplace;

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) 
            if (soc_feature(unit, soc_feature_vlan_xlate_dtag_range)) {
                BCM_IF_ERROR_RETURN
                    (_bcm_trx_vlan_translate_action_range_add(unit, gport,
                                        old_vid_low, old_vid_high, 
                                        BCM_VLAN_INVALID, BCM_VLAN_INVALID, 
                                        &action));
            } else
#endif
            {
                BCM_IF_ERROR_RETURN
                    (_bcm_trx_vlan_translate_action_add(unit, gport,
                                        bcmVlanTranslateKeyPortOuter,
                                        old_vid_low, BCM_VLAN_INVALID,
                                        &action));
            }
            return BCM_E_NONE;
        }
#endif /* BCM_TRX_SUPPORT */
    }

    return BCM_E_UNAVAIL;
}

int bcm_esw_vlan_translate_range_get (int unit, bcm_port_t port,
                                      bcm_vlan_t old_vlan_low,
                                      bcm_vlan_t old_vlan_high,
                                      bcm_vlan_t *new_vid, int *prio)
{
    bcm_gport_t     gport;

    CHECK_INIT(unit);
    VLAN_CHK_ID(unit, old_vlan_low);
    VLAN_CHK_ID(unit, old_vlan_high);

    if (BCM_GPORT_IS_SET(port)) {
        gport = port;
    } else {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_gport_get(unit, port, &gport));
    }

    if ((NULL == new_vid) || NULL == prio ){
        return BCM_E_PARAM;
    }
    if (old_vlan_high < old_vlan_low) {
        return BCM_E_PARAM;
    }

    if (soc_feature(unit, soc_feature_vlan_translation_range)) {
#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(unit)) {
            bcm_vlan_action_set_t action;
            
            bcm_vlan_action_set_t_init(&action);
            BCM_IF_ERROR_RETURN
                (_bcm_trx_vlan_translate_action_range_get(unit, gport,
                                        old_vlan_low, old_vlan_high, 
                                        BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                        &action));
            if (BCM_VLAN_INVALID != action.new_outer_vlan) {
                *new_vid = action.new_outer_vlan;
                *prio = action.priority;
                return BCM_E_NONE;
            } 

            BCM_IF_ERROR_RETURN
                (_bcm_trx_vlan_translate_action_range_get(unit, gport,
                                        BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                        old_vlan_low, old_vlan_high, 
                                        &action));

            if (BCM_VLAN_INVALID != action.new_outer_vlan) {
                *new_vid = action.new_outer_vlan;
                *prio = action.priority;
                return BCM_E_NONE;
            }
        
            return BCM_E_NOT_FOUND;
        }
#endif /* BCM_TRX_SUPPORT */
    }

    return BCM_E_UNAVAIL;
}


int
bcm_esw_vlan_translate_range_delete(int unit, int port,
                                    bcm_vlan_t old_vid_low,
                                    bcm_vlan_t old_vid_high)
{
    bcm_gport_t gport;

    CHECK_INIT(unit);
    VLAN_CHK_ID(unit, old_vid_low);
    VLAN_CHK_ID(unit, old_vid_high);
    if (old_vid_high < old_vid_low) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        gport = port;
    } else {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_gport_get(unit, port, &gport));
    }

    if (soc_feature(unit, soc_feature_vlan_translation_range)) {
#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(unit)) {
            int rv = BCM_E_NONE;
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
            if (soc_feature(unit, soc_feature_vlan_xlate_dtag_range)) {
                /* Delete outer vlan translate entry and range */
                rv = _bcm_trx_vlan_translate_action_range_delete(unit, gport,
                                        old_vid_low, old_vid_high,
                                        BCM_VLAN_INVALID, BCM_VLAN_INVALID); 
            } else
#endif
            {
                /* Delete outer vlan translate entry */
                rv = _bcm_trx_vlan_translate_action_delete(unit, gport,
                                               bcmVlanTranslateKeyPortOuter,
                                               old_vid_low, BCM_VLAN_INVALID);
            }

            /* Delete inner vlan translate entry and range */
            if (BCM_SUCCESS(rv) || (rv == BCM_E_NOT_FOUND) ) {
            BCM_IF_ERROR_RETURN
                (_bcm_trx_vlan_translate_action_range_delete(unit, gport,
                                        BCM_VLAN_INVALID, BCM_VLAN_INVALID, 
                                        old_vid_low, old_vid_high));
            } else {
                return (rv);
            }
            
            return (BCM_E_NONE);
        }
#endif /* BCM_TRX_SUPPORT */
    }

    return BCM_E_UNAVAIL;
}

int
bcm_esw_vlan_translate_range_delete_all(int unit)
{
    if (soc_feature(unit, soc_feature_vlan_translation_range)) {
#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(unit)) {
            return _bcm_trx_vlan_translate_action_range_delete_all(unit);
        }
#endif /* BCM_TRX_SUPPORT */
    }

    return BCM_E_UNAVAIL;
}

int 
bcm_esw_vlan_dtag_range_add(int unit, int port,
                            bcm_vlan_t old_vid_low, 
                            bcm_vlan_t old_vid_high,
                            bcm_vlan_t new_vid, int int_prio)
{
    bcm_gport_t gport;

    CHECK_INIT(unit);
    VLAN_CHK_ID(unit, old_vid_low);
    VLAN_CHK_ID(unit, old_vid_high);
    VLAN_CHK_ID(unit, new_vid);
    if (old_vid_high < old_vid_low) {
        return BCM_E_PARAM;
    }
    if ((int_prio < BCM_PRIO_MIN) || (int_prio > BCM_PRIO_MAX)) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        gport = port;
    } else {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_gport_get(unit, port, &gport));
    }

    if (soc_feature(unit, soc_feature_vlan_translation_range)) {
#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(unit)) {
            bcm_vlan_action_set_t action;

            bcm_vlan_action_set_t_init(&action);
            action.new_outer_vlan = new_vid;
            action.priority = int_prio;
            /* For inner tagged packets, set the outer tag action and outer
             * packet prio action to ADD.
             * For all other packet types, the action is initialized to NONE.
             */
            action.it_outer = bcmVlanActionAdd;
            if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
                action.it_outer_pkt_prio = bcmVlanActionAdd;
            } else {
                action.it_outer_pkt_prio = bcmVlanActionNone;
            }
            return _bcm_trx_vlan_translate_action_range_add(unit, gport,
                                          BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                          old_vid_low, old_vid_high,
                                          &action);
        }
#endif /* BCM_TRX_SUPPORT */
    }

    return BCM_E_UNAVAIL;
}


int 
bcm_esw_vlan_dtag_range_get (int unit, bcm_port_t port,
                             bcm_vlan_t old_vid_low,
                             bcm_vlan_t old_vid_high,
                             bcm_vlan_t *new_vid,
                             int *prio)
{
    bcm_gport_t     gport;

    CHECK_INIT(unit);
    VLAN_CHK_ID(unit, old_vid_low);
    VLAN_CHK_ID(unit, old_vid_high);
    
    if ((NULL == new_vid) || NULL == prio ){
        return BCM_E_PARAM;
    }
    if (old_vid_high < old_vid_low) {
        return BCM_E_PARAM;
    }
    if (BCM_GPORT_IS_SET(port)) {
        gport = port;
    } else {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_gport_get(unit, port, &gport));
    }

    if (soc_feature(unit, soc_feature_vlan_translation_range)) {
#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(unit)) {
            bcm_vlan_action_set_t action;

            bcm_vlan_action_set_t_init(&action);
            BCM_IF_ERROR_RETURN
                (_bcm_trx_vlan_translate_action_range_get(unit, gport,
                                        BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                        old_vid_low, old_vid_high, 
                                        &action));
            if (BCM_VLAN_INVALID != action.new_outer_vlan) {
                *new_vid = action.new_outer_vlan;
                *prio = action.priority;
                return BCM_E_NONE;
            }

            return BCM_E_NOT_FOUND;
        }
#endif /* BCM_TRX_SUPPORT */
    }

    return BCM_E_UNAVAIL;
}


int 
bcm_esw_vlan_dtag_range_delete(int unit, int port, 
                               bcm_vlan_t old_vid_low,
                               bcm_vlan_t old_vid_high)
{
    bcm_gport_t     gport;

    CHECK_INIT(unit);
    VLAN_CHK_ID(unit, old_vid_low);
    VLAN_CHK_ID(unit, old_vid_high);
    if (old_vid_high < old_vid_low) {
        return BCM_E_PARAM;
    }
    if (BCM_GPORT_IS_SET(port)) {
        gport = port;
    } else {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_gport_get(unit, port, &gport));
    }

    if (soc_feature(unit, soc_feature_vlan_translation_range)) {
#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(unit)) {
            return _bcm_trx_vlan_translate_action_range_delete(unit, gport,
                                          BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                          old_vid_low, old_vid_high);
        }
#endif /* BCM_TRX_SUPPORT */
    }

    return BCM_E_UNAVAIL;
}

int 
bcm_esw_vlan_dtag_range_delete_all(int unit)
{
    if (soc_feature(unit, soc_feature_vlan_translation_range)) {
#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(unit)) {
            return _bcm_trx_vlan_translate_action_range_delete_all(unit);
        }
#endif /* BCM_TRX_SUPPORT */
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function :
 *      bcm_vlan_translate_action_range_add
 * Description :
 *   Add an entry to the VLAN Translation table, which assigns
 *   VLAN actions for packets matching within the VLAN range(s).
 * Parameters :
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress gport (generic port)
 *      outer_vlan_low  (IN) Outer VLAN ID Low
 *      outer_vlan_high (IN) Outer VLAN ID High
 *      inner_vlan_low  (IN) Inner VLAN ID Low
 *      inner_vlan_high (IN) Inner VLAN ID High
 *      action          (IN) Action for outer and inner tag
 *
 * Notes :
 *   For translation of double-tagged packets, specify a valid
 *   VLAN ID value for outer_vlan_low/high and inner_vlan_low/high.
 *   For translation of single outer-tagged packets, specify a
 *   valid VLAN ID for outer_vlan_low/high and BCM_VLAN_INVALID
 *   for inner_vlan_low/high. For translation of single inner-tagged
 *   packets, specify a valid VLAN ID for inner_vlan_low/high and
 *   BCM_VLAN_INVALID for outer_vlan_low/high.
 *   If action is NULL, do not add entry to VLAN translation table,
 *   user need to call bcm_vlan_translate_action_add with proper vlan_low
 *   to specify VLAN actions.
 */

int
bcm_esw_vlan_translate_action_range_add(int unit, bcm_gport_t port,
                                        bcm_vlan_t outer_vlan_low,
                                        bcm_vlan_t outer_vlan_high,
                                        bcm_vlan_t inner_vlan_low,
                                        bcm_vlan_t inner_vlan_high,
                                        bcm_vlan_action_set_t *action)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    if (!soc_feature(unit, soc_feature_vlan_translation_range)) {
        return BCM_E_UNAVAIL;
    }

    CHECK_INIT(unit);
    /* BCM_VLAN_INVALID is valid input for this API */
    if (BCM_VLAN_INVALID != outer_vlan_low)
        VLAN_CHK_ID(unit, outer_vlan_low);
    if (BCM_VLAN_INVALID != outer_vlan_high)
        VLAN_CHK_ID(unit, outer_vlan_high);
    if (BCM_VLAN_INVALID != inner_vlan_low)
        VLAN_CHK_ID(unit, inner_vlan_low);
    if (BCM_VLAN_INVALID != inner_vlan_high)
        VLAN_CHK_ID(unit, inner_vlan_high);

    if (port == BCM_GPORT_INVALID)
        return BCM_E_PORT;
    
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        return _bcm_trx_vlan_translate_action_range_add(unit, port,
                                             outer_vlan_low, outer_vlan_high,
                                             inner_vlan_low, inner_vlan_high, 
                                             action);
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}


/*
 * Function :
 *      bcm_esw_vlan_translate_action_range_get
 * Description :
 *   Get an entry to the VLAN Translation table, which assigns
 *   VLAN actions for packets matching within the VLAN range(s).
 * Parameters :
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress gport (generic port)
 *      outer_vlan_low  (IN) Outer VLAN ID Low
 *      outer_vlan_high (IN) Outer VLAN ID High
 *      inner_vlan_low  (IN) Inner VLAN ID Low
 *      inner_vlan_high (IN) Inner VLAN ID High
 *      action          (OUT) Action for outer and inner tag
 *
 * Notes :
 *   For translation of double-tagged packets, specify a valid
 *   VLAN ID value for outer_vlan_low/high and inner_vlan_low/high.
 *   For translation of single outer-tagged packets, specify a
 *   valid VLAN ID for outer_vlan_low/high and BCM_VLAN_INVALID
 *   for inner_vlan_low/high. For translation of single inner-tagged
 *   packets, specify a valid VLAN ID for inner_vlan_low/high and
 *   BCM_VLAN_INVALID for outer_vlan_low/high.
 */
int 
bcm_esw_vlan_translate_action_range_get (int unit, bcm_gport_t port,
                                         bcm_vlan_t outer_vlan_low,
                                         bcm_vlan_t outer_vlan_high,
                                         bcm_vlan_t inner_vlan_low,
                                         bcm_vlan_t inner_vlan_high,
                                         bcm_vlan_action_set_t *action)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    if (!soc_feature(unit, soc_feature_vlan_translation_range)) {
        return BCM_E_UNAVAIL;
    }

    CHECK_INIT(unit);
    /* BCM_VLAN_INVALID is valid input for this API */
    if (BCM_VLAN_INVALID != outer_vlan_low)
        VLAN_CHK_ID(unit, outer_vlan_low);
    if (BCM_VLAN_INVALID != outer_vlan_high)
        VLAN_CHK_ID(unit, outer_vlan_high);
    if (BCM_VLAN_INVALID != inner_vlan_low)
        VLAN_CHK_ID(unit, inner_vlan_low);
    if (BCM_VLAN_INVALID != inner_vlan_high)
        VLAN_CHK_ID(unit, inner_vlan_high);
    if (NULL == action)
        return BCM_E_PARAM;
    
    if (port == BCM_GPORT_INVALID)
        return BCM_E_PORT;
        
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        bcm_vlan_action_set_t_init(action);
        return _bcm_trx_vlan_translate_action_range_get(unit, port,
                                             outer_vlan_low, outer_vlan_high,
                                             inner_vlan_low, inner_vlan_high, 
                                             action);
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function :
 *      bcm_vlan_translate_action_range_delete
 * Description :
 *   Delete an entry from the VLAN Translation table for the 
 *   specified VLAN range(s).
 * Parameters :
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress gport (generic port)
 *      outer_vlan_low  (IN) Outer VLAN ID Low
 *      outer_vlan_high (IN) Outer VLAN ID High
 *      inner_vlan_low  (IN) Inner VLAN ID Low
 *      inner_vlan_high (IN) Inner VLAN ID High
 *
 * Notes :
 *   For translation of double-tagged packets, specify a valid
 *   VLAN ID value for outer_vlan_low/high and inner_vlan_low/high.
 *   For translation of single outer-tagged packets, specify a
 *   valid VLAN ID for outer_vlan_low/high and BCM_VLAN_INVALID
 *   for inner_vlan_low/high. For translation of single inner-tagged
 *   packets, specify a valid VLAN ID for inner_vlan_low/high and
 *   BCM_VLAN_INVALID for outer_vlan_low/high.
 */

int
bcm_esw_vlan_translate_action_range_delete(int unit, bcm_gport_t port,
                                           bcm_vlan_t outer_vlan_low,
                                           bcm_vlan_t outer_vlan_high,
                                           bcm_vlan_t inner_vlan_low,
                                           bcm_vlan_t inner_vlan_high)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    if (!soc_feature(unit, soc_feature_vlan_translation_range)) {
        return BCM_E_UNAVAIL;
    }

    CHECK_INIT(unit);
    /* BCM_VLAN_INVALID is valid input for this API */
    if (BCM_VLAN_INVALID != outer_vlan_low)
        VLAN_CHK_ID(unit, outer_vlan_low);
    if (BCM_VLAN_INVALID != outer_vlan_high)
        VLAN_CHK_ID(unit, outer_vlan_high);
    if (BCM_VLAN_INVALID != inner_vlan_low)
        VLAN_CHK_ID(unit, inner_vlan_low);
    if (BCM_VLAN_INVALID != inner_vlan_high)
        VLAN_CHK_ID(unit, inner_vlan_high);
    
    if (port == BCM_GPORT_INVALID)
        return BCM_E_PORT;

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        return _bcm_trx_vlan_translate_action_range_delete(unit, port,
                                             outer_vlan_low, outer_vlan_high,
                                             inner_vlan_low, inner_vlan_high);
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function :
 *      bcm_vlan_translate_action_range_delete_all
 * Description :
 *     Delete all VLAN range entries from the VLAN Translation table.
 * Parameters :
 *      unit            (IN) BCM unit number
 */

int
bcm_esw_vlan_translate_action_range_delete_all(int unit)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    if (!soc_feature(unit, soc_feature_vlan_translation_range)) {
        return BCM_E_UNAVAIL;
    }

    CHECK_INIT(unit);

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        return _bcm_trx_vlan_translate_action_range_delete_all(unit);
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}


/*
 * Function :
 *      bcm_vlan_translate_action_range_traverse
 * Description :
 *   Traverses over VLAN Translation table and call provided callback 
 *   with valid entries.
 * Parameters :
 *      unit            (IN) BCM unit number
 *      cb              (IN) User callback function
 *      user_data       (IN) Pointer to user specific data
 * Return:
 *
 *      BCM_E_XXX
 */
int 
bcm_esw_vlan_translate_action_range_traverse(int unit,
    bcm_vlan_translate_action_range_traverse_cb cb, void *user_data)
{
    _bcm_vlan_translate_traverse_t          trvs_st;
    bcm_vlan_action_set_t                   action;
    _translate_action_range_traverse_cb_t   usr_cb_st;
    
    if (!soc_feature(unit, soc_feature_vlan_translation_range)) {
        return BCM_E_UNAVAIL;
    }
    if (!cb) {
        return (BCM_E_PARAM);
    }

    sal_memset(&trvs_st, 0, sizeof(_bcm_vlan_translate_traverse_t));
    sal_memset(&action, 0, sizeof(bcm_vlan_action_set_t));
    sal_memset(&usr_cb_st, 0, sizeof(_translate_action_range_traverse_cb_t));

    usr_cb_st.usr_cb = cb;

    trvs_st.user_data = user_data;
    trvs_st.action = &action;
    trvs_st.user_cb_st = (void *)&usr_cb_st;
    trvs_st.int_cb = _bcm_esw_vlan_translate_action_range_traverse_int_cb;

#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit)) {
        return _bcm_trx_vlan_translate_action_range_traverse(unit, &trvs_st);
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}


/*
 * Function :
 *      bcm_vlan_translate_range_traverse
 * Description :
 *   Traverses over VLAN translate table and call provided callback 
 *   with valid entries.
 * Parameters :
 *      unit            (IN) BCM unit number
 *      cb              (IN) User callback function
 *      user_data       (IN) Pointer to user specific data
 * Return:
 *
 *      BCM_E_XXX
 */
int 
bcm_esw_vlan_translate_range_traverse(int unit, 
                                  bcm_vlan_translate_range_traverse_cb cb,
                                  void *user_data)
{

    _bcm_vlan_translate_traverse_t          trvs_st;
    bcm_vlan_action_set_t                   action;
    _translate_range_traverse_cb_t          usr_cb_st;
    
    if (!soc_feature(unit, soc_feature_vlan_translation_range)) {
        return BCM_E_UNAVAIL;
    }
    if (!cb) {
        return (BCM_E_PARAM);
    }

    sal_memset(&trvs_st, 0, sizeof(_bcm_vlan_translate_traverse_t));
    sal_memset(&action, 0, sizeof(bcm_vlan_action_set_t));
    sal_memset(&usr_cb_st, 0, sizeof(_translate_range_traverse_cb_t));

    usr_cb_st.usr_cb = cb;

    trvs_st.user_data = user_data;
    trvs_st.action = &action;
    trvs_st.user_cb_st = (void *)&usr_cb_st;
    

#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit)) {
        trvs_st.int_cb = _bcm_esw_vlan_translate_range_traverse_int_cb;  
        return _bcm_trx_vlan_translate_action_range_traverse(unit, &trvs_st);
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function :
 *      bcm_vlan_dtag_range_traverse
 * Description :
 *   Traverses over VLAN double tagging table and call provided callback 
 *   with valid entries.
 * Parameters :
 *      unit            (IN) BCM unit number
 *      cb              (IN) User callback function
 *      user_data       (IN) Pointer to user specific data
 * Return:
 *
 *      BCM_E_XXX
 */
int 
bcm_esw_vlan_dtag_range_traverse(int unit, bcm_vlan_dtag_range_traverse_cb cb,
                             void *user_data)
{

    _bcm_vlan_translate_traverse_t          trvs_st;
    bcm_vlan_action_set_t                   action;
    _dtag_range_traverse_cb_t               usr_cb_st;
    
    if (!soc_feature(unit, soc_feature_vlan_translation_range)) {
        return BCM_E_UNAVAIL;
    }
    if (!cb) {
        return (BCM_E_PARAM);
    }

    sal_memset(&trvs_st, 0, sizeof(_bcm_vlan_translate_traverse_t));
    sal_memset(&action, 0, sizeof(bcm_vlan_action_set_t));
    sal_memset(&usr_cb_st, 0, sizeof(_dtag_range_traverse_cb_t));

    usr_cb_st.usr_cb = cb;

    trvs_st.user_data = user_data;
    trvs_st.action = &action;
    trvs_st.user_cb_st = (void *)&usr_cb_st;
    
#if defined(BCM_TRX_SUPPORT)
    trvs_st.int_cb = _bcm_esw_vlan_dtag_range_traverse_int_cb;  
    if (SOC_IS_TRX(unit)) {
        return _bcm_trx_vlan_translate_action_range_traverse(unit, &trvs_st);
    }
#endif /* BCM_TRX_SUPPORT */
    
    return BCM_E_UNAVAIL;
}


int
bcm_esw_vlan_translate_egress_add(int unit, int port, bcm_vlan_t old_vid,
                                  bcm_vlan_t new_vid, int prio)
{

    bcm_module_t        modid = -1;
    bcm_trunk_t         tgid = -1;
    int                 id = -1;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    CHECK_INIT(unit);
    VLAN_CHK_ID(unit, old_vid);
    VLAN_CHK_ID(unit, new_vid);

    /* XGS does not support translation of VLAN ID 0 */
    if (old_vid == 0) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        if (!BCM_GPORT_IS_TUNNEL(port)) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        }
        if ((-1 != tgid || -1 != id) && !BCM_GPORT_IS_TUNNEL(port)) {
            return BCM_E_PORT;
        }
    } else {
        if (SOC_IS_RAVEN(unit)) {
            if (port != -1) {
                if (!SOC_PORT_VALID(unit, port)) {
                    return BCM_E_PORT; 
                }
            }
        } else {
            if (!SOC_PORT_VALID(unit, port)) {
                  return BCM_E_PORT;
            }
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_stk_my_modid_get(unit, &modid));
    }

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        bcm_vlan_action_set_t   action;
        soc_control_t           *sc = SOC_CONTROL(unit);
        bcm_vlan_info_t         *vi = &vlan_info[unit];
        int                     rv = BCM_E_NONE;
        
        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }

        /* If port class is changed system should not allow */
        /* To use this API since it leads to confused outcome */
        if (sc->soc_flags & SOC_F_XLATE_EGR_BLOCKED) {
            return BCM_E_CONFIG;
        }

        bcm_vlan_action_set_t_init(&action);
        action.new_outer_vlan = new_vid;
        action.new_inner_vlan = 0;
        action.priority = prio;
        action.ot_outer = bcmVlanActionReplace;
        action.dt_outer = bcmVlanActionReplace;

        rv = _bcm_trx_vlan_translate_egress_action_add(unit, port, 
                                                        old_vid, 0, 0,
                                                        &action);
        /* In case of usage of this API on Triumph and newer devices */
        /* port parameter is pretended to be a port_class therefore */
        /* port_class will remain unchanged as long as there are entries */
        /* in egress vlan translation table added using this API */
        if (BCM_SUCCESS(rv)) {
            sc->soc_flags |= SOC_F_PORT_CLASS_BLOCKED;
            vi->old_egr_xlate_cnt++;
#ifdef BCM_WARM_BOOT_SUPPORT
            SOC_CONTROL_LOCK(unit);
            SOC_CONTROL(unit)->scache_dirty = 1;
            SOC_CONTROL_UNLOCK(unit);
#endif
        }

        return rv;
    }
#endif /* BCM_TRX_SUPPORT */


#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_vlan_translate_add(unit, port, old_vid, new_vid, prio,
                                          BCM_VLAN_XLATE_EGR);
    }
#endif

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcm_vlan_translate_egress_get
 * Purpose:
 *      Get vlan egress translation
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - port numebr
 *      old_vid - Old VLAN ID to has translation for
 *      new_vid - New VLAN ID that packet will get
 *      prio    - Priority
 * Returns:
 *      BCM_E_NONE - Translation found, new_vid nad prio will have the values.
 *      BCM_E_NOT_FOUND - Translation does not exist
 *      BCM_E_XXX  - Other error
 * Notes:
 *      None.
 */
int bcm_esw_vlan_translate_egress_get (int unit, bcm_port_t port,
                                       bcm_vlan_t old_vid,
                                       bcm_vlan_t *new_vid,
                                       int *prio)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid = -1;
    int                 id = -1;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    CHECK_INIT(unit);
    VLAN_CHK_ID(unit, old_vid);

    /* XGS does not support translation of VLAN ID 0 */
    if (old_vid == 0) {
        return BCM_E_PARAM;
    }

    if ((NULL == new_vid) ||(NULL == prio)){
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        if (!BCM_GPORT_IS_TUNNEL(port)) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        }
        if ((-1 != tgid || -1 != id) && !BCM_GPORT_IS_TUNNEL(port)) {
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_stk_my_modid_get(unit, &modid));
    }

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        bcm_vlan_action_set_t   action;
        soc_control_t           *sc = SOC_CONTROL(unit);
        
        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }

        /* If port class is changed system should not allow */
        /* To use this API since it leads to confused outcome */
        if (sc->soc_flags & SOC_F_XLATE_EGR_BLOCKED) {
            return BCM_E_CONFIG;
        }

        bcm_vlan_action_set_t_init(&action);
        BCM_IF_ERROR_RETURN(
            _bcm_trx_vlan_translate_egress_action_get(unit, port, old_vid,
                                                      0, 0, &action));
        *new_vid = action.new_outer_vlan;
        *prio = action.priority;

        return BCM_E_NONE;
    }
#endif /* BCM_TRX_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_vlan_translate_get(unit, port, old_vid, new_vid,
                                          prio, BCM_VLAN_XLATE_EGR);
    }
#endif

    return BCM_E_UNAVAIL;
}

int
bcm_esw_vlan_translate_egress_delete(int unit, int port, bcm_vlan_t old_vid)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid = -1;
    int                 id = -1;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    CHECK_INIT(unit);
    VLAN_CHK_ID(unit, old_vid);

    if (BCM_GPORT_IS_SET(port)) {
        if (!BCM_GPORT_IS_TUNNEL(port)) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        }
        if ((-1 != tgid || -1 != id) && !BCM_GPORT_IS_TUNNEL(port)) {
            return BCM_E_PORT;
        }
    } else {
        if (SOC_IS_RAVEN(unit)) {
            if (port != -1) {
                if (!SOC_PORT_VALID(unit, port)) {
                    return BCM_E_PORT; 
                }
            }
        } else {
            if (!SOC_PORT_VALID(unit, port)) {
                  return BCM_E_PORT;
            }
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_stk_my_modid_get(unit, &modid));
    }

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {

        /* XGS does not support translation of VLAN ID 0 */
        if (old_vid == 0) {
            return BCM_E_PARAM;
        }

        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        } else {
            soc_control_t           *sc = SOC_CONTROL(unit);
            bcm_vlan_info_t         *vi = &vlan_info[unit];
            int                     rv = BCM_E_NONE;

            /* If port class is changed system should not allow */
            /* To use this API since it leads to confused outcome */
            if (sc->soc_flags & SOC_F_XLATE_EGR_BLOCKED) {
                return BCM_E_CONFIG;
            }

            rv = _bcm_trx_vlan_translate_egress_action_delete(unit, port,
                                                           old_vid, 0, 0);

            /* In case of usage of this API on Triumph and newer devices */
            /* port parameter is pretended to be a port_class therefore */
            /* port_class operation will be permited only when last entry */
            /* in egress vlan translation table removed using this API */
            if (BCM_SUCCESS(rv)) {
                vi->old_egr_xlate_cnt--;
#ifdef BCM_WARM_BOOT_SUPPORT
                SOC_CONTROL_LOCK(unit);
                SOC_CONTROL(unit)->scache_dirty = 1;
                SOC_CONTROL_UNLOCK(unit);
#endif
                if (0 == vi->old_egr_xlate_cnt) {
                    sc->soc_flags &= ~SOC_F_PORT_CLASS_BLOCKED;
                }
            }
            return rv;
        }
    }
#endif /* BCM_TRX_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_vlan_translate_delete(unit, port, old_vid,
                                             BCM_VLAN_XLATE_EGR);
    }
#endif

    return BCM_E_UNAVAIL;
}

int
bcm_esw_vlan_translate_egress_delete_all(int unit)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        } else {
            soc_control_t           *sc = SOC_CONTROL(unit);
            bcm_vlan_info_t         *vi = &vlan_info[unit];
            int                     rv = BCM_E_NONE;

            /* If port class is changed system should not allow */
            /* To use this API since it leads to confused outcome */
            if (sc->soc_flags & SOC_F_XLATE_EGR_BLOCKED) {
                return BCM_E_CONFIG;
            }

            rv = _bcm_trx_vlan_translate_egress_action_delete_all(unit);

            /* In case of usage of this API on Triumph and newer devices */
            /* port parameter is pretended to be a port_class therefore */
            /* port_class operation will be permited when all entries */
            /* in egress vlan translation table removed using this API */
            if (BCM_SUCCESS(rv)) {
                sc->soc_flags &= ~SOC_F_PORT_CLASS_BLOCKED;
                vi->old_egr_xlate_cnt = 0;
#ifdef BCM_WARM_BOOT_SUPPORT
                SOC_CONTROL_LOCK(unit);
                SOC_CONTROL(unit)->scache_dirty = 1;
                SOC_CONTROL_UNLOCK(unit);
#endif
            }
            return rv;
        }
    }
#endif /* BCM_TRX_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        int rv;
        rv = _bcm_fb_vlan_translate_delete(unit, -1, BCM_VLAN_NONE,
                                             BCM_VLAN_XLATE_EGR);
        if (BCM_SUCCESS(rv) || (rv == BCM_E_NOT_FOUND)) {
            return BCM_E_NONE;
        }

        return rv;
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function   :
 *      _bcm_esw_vlan_translate_egress_action_add
 * Description   :
 *      Helper function to converge the two APIs
 *      bcm_vlan_translate_egress_action_add and
 *      bcm_vlan_translate_egress_gport_action_add
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port_class      (IN) Group ID of ingress port
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 *      action          (IN) Action for outer and inner tag
 */
static int
_bcm_esw_vlan_translate_egress_action_add(int unit, int port_class,
                                          bcm_vlan_t outer_vlan,
                                          bcm_vlan_t inner_vlan,
                                          bcm_vlan_translate_key_t key_type,
                                          bcm_vlan_action_set_t *action,
                                          int handle_modport_gport)
{   
    bcm_module_t        modid;
    bcm_trunk_t         tgid = -1;
    int                 id = -1; 

    CHECK_INIT(unit);
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3) 
    if (soc_feature(unit, soc_feature_vp_sharing)) { 
        int num_vfi = 0;
        bcm_vpn_t vpn_min = 0, vpn_max = 0;
        if (outer_vlan > BCM_VLAN_MAX) {
            num_vfi = soc_mem_index_count(unit, VFIm);
            _BCM_VPN_SET(vpn_min, _BCM_VPN_TYPE_VFI, 0);
            vpn_max = vpn_min + num_vfi - 1;
            if (outer_vlan > vpn_max) {
                return (BCM_E_PARAM);
            } else if ((outer_vlan >= vpn_min) &&
                       (outer_vlan <= vpn_max)) {
                if (inner_vlan != BCM_VLAN_INVALID) {
                    return BCM_E_PARAM;
                }
            } else {
                return BCM_E_PARAM;
            }
        } else {
            VLAN_CHK_ID(unit, inner_vlan);
        }
    } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT &&  INCLUDE_L3 */
    {
        VLAN_CHK_ID(unit, outer_vlan);
        VLAN_CHK_ID(unit, inner_vlan);
    }

    if (NULL == action) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port_class)) {
        if (!BCM_GPORT_IS_TUNNEL(port_class) &&
            !BCM_GPORT_IS_MIM_PORT(port_class) && 
            !BCM_GPORT_IS_NIV_PORT(port_class) && 
            !BCM_GPORT_IS_MPLS_PORT(port_class) &&
            !BCM_GPORT_IS_VLAN_PORT(port_class) &&
            !BCM_GPORT_IS_VXLAN_PORT(port_class) &&
            !BCM_GPORT_IS_EXTENDER_PORT(port_class) &&
            !BCM_GPORT_IS_L2GRE_PORT(port_class) &&
            !BCM_GPORT_IS_FLOW_PORT(port_class) &&
            !(handle_modport_gport &&
              (BCM_GPORT_IS_MODPORT(port_class) ||
              BCM_GPORT_IS_SUBPORT_PORT(port_class))))
        {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port_class, &modid, &port_class, 
                                       &tgid, &id));
        }
        if ((-1 != tgid || -1 != id) && !BCM_GPORT_IS_TUNNEL(port_class) &&
            !BCM_GPORT_IS_MIM_PORT(port_class) && 
            !BCM_GPORT_IS_NIV_PORT(port_class) &&
            !BCM_GPORT_IS_MPLS_PORT(port_class) &&
            !BCM_GPORT_IS_VLAN_PORT(port_class) &&
            !BCM_GPORT_IS_EXTENDER_PORT(port_class) &&
            !BCM_GPORT_IS_VXLAN_PORT(port_class) &&
            !BCM_GPORT_IS_FLOW_PORT(port_class) &&
            !BCM_GPORT_IS_L2GRE_PORT(port_class))
        {
            return BCM_E_PORT;
        }
    } else 
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    if (inner_vlan == BCM_VLAN_INVALID) {
        BCM_IF_ERROR_RETURN
                   (bcm_td2p_dvp_group_validate(unit, port_class));
    } else 
#endif /* BCM_TRIDENT2PLUS_SUPPORT &&  INCLUDE_L3 */
    {
        if (!SOC_PORT_CLASS_VALID(unit, port_class)) { 
            return BCM_E_PORT; 
        }
    }

#if defined(BCM_TRIDENT3_SUPPORT) && defined(INCLUDE_L3)
    if (action->flags & BCM_VLAN_ACTION_SET_EGRESS_TUNNEL_OVID) {
        if (!SOC_MEM_IS_VALID(unit, EGR_VLAN_XLATE_2_DOUBLEm)) {
            return BCM_E_UNAVAIL;
        }
        if (inner_vlan != BCM_VLAN_NONE) {
            return BCM_E_PARAM;
        }
        if (BCM_GPORT_IS_SET(port_class) &&
            !BCM_GPORT_IS_MODPORT(port_class)) {
            return BCM_E_PARAM;
        }
    }
#endif

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        return _bcm_trx_vlan_translate_egress_action_add(unit, port_class, 
                                             outer_vlan, inner_vlan, 
                                             key_type, action);
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function   :
 *      _bcm_esw_vlan_translate_egress_action_get
 * Description   :
 *      Helper function to converge the APIs
 *      bcm_vlan_translate_egress_action_get and
 *      bcm_vlan_translate_egress_gport_action_get
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port_class      (IN) Group ID of ingress port
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 *      action          (OUT) Action for outer and inner tag
 */
static int
_bcm_esw_vlan_translate_egress_action_get (int unit, int port_class,
                                          bcm_vlan_t outer_vlan,
                                          bcm_vlan_t inner_vlan,
                                          bcm_vlan_translate_key_t key_type,
                                          bcm_vlan_action_set_t *action,
                                          int handle_modport_gport)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid = -1;
    int                 id = -1; 

    CHECK_INIT(unit);
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vp_sharing)) { 
        int num_vfi = 0;
        bcm_vpn_t vpn_min = 0, vpn_max = 0;
        if (outer_vlan > BCM_VLAN_MAX) {
            num_vfi = soc_mem_index_count(unit, VFIm);
            _BCM_VPN_SET(vpn_min, _BCM_VPN_TYPE_VFI, 0);
            vpn_max = vpn_min + num_vfi - 1;
            if (outer_vlan > vpn_max) {
                return (BCM_E_PARAM);
            } else if ((outer_vlan >= vpn_min) &&
                       (outer_vlan <= vpn_max)) {
                if (inner_vlan != BCM_VLAN_INVALID) {
                    return BCM_E_PARAM;
                }
            } else {
                return BCM_E_PARAM;
            }
        } else {
            VLAN_CHK_ID(unit, inner_vlan);
        }
    } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT && INCLUDE_L3 */
    {
        VLAN_CHK_ID(unit, outer_vlan);
        VLAN_CHK_ID(unit, inner_vlan);
    }

    if (NULL == action) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port_class)) {
        if (!BCM_GPORT_IS_TUNNEL(port_class) &&
            !BCM_GPORT_IS_MIM_PORT(port_class) &&
            !BCM_GPORT_IS_NIV_PORT(port_class) &&
            !BCM_GPORT_IS_MPLS_PORT(port_class) &&
            !BCM_GPORT_IS_VLAN_PORT(port_class) &&
            !BCM_GPORT_IS_VXLAN_PORT(port_class) &&
            !BCM_GPORT_IS_EXTENDER_PORT(port_class) &&
            !BCM_GPORT_IS_L2GRE_PORT(port_class) &&
            !BCM_GPORT_IS_FLOW_PORT(port_class) &&
            !(handle_modport_gport &&
              (BCM_GPORT_IS_MODPORT(port_class) ||
              BCM_GPORT_IS_SUBPORT_PORT(port_class))))
        {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port_class, &modid, &port_class, 
                                       &tgid, &id));
        }
        if ((-1 != tgid || -1 != id) && !BCM_GPORT_IS_TUNNEL(port_class) &&
            !BCM_GPORT_IS_MIM_PORT(port_class) &&
            !BCM_GPORT_IS_NIV_PORT(port_class) &&
            !BCM_GPORT_IS_EXTENDER_PORT(port_class) &&
            !BCM_GPORT_IS_MPLS_PORT(port_class) &&
            !BCM_GPORT_IS_VLAN_PORT(port_class) && 
            !BCM_GPORT_IS_VXLAN_PORT(port_class) &&
            !BCM_GPORT_IS_FLOW_PORT(port_class) &&
            !BCM_GPORT_IS_L2GRE_PORT(port_class)) 
        {
            return BCM_E_PORT;
        }
    } else {
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
        if (inner_vlan == BCM_VLAN_INVALID) {
            BCM_IF_ERROR_RETURN
                       (bcm_td2p_dvp_group_validate(unit, port_class));
        } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT && INCLUDE_L3 */
        if (!SOC_PORT_CLASS_VALID(unit, port_class)) { 
            return BCM_E_PORT; 
        }
    }

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        return _bcm_trx_vlan_translate_egress_action_get(unit, port_class, 
                                                        outer_vlan, inner_vlan, 
                                                        key_type, action);
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_esw_vlan_translate_egress_action_delete
 * Purpose:
 *      Helper function to converge the APIs
 *      bcm_vlan_translate_egress_action_delete and
 *      bcm_vlan_translate_egress_gport_action_delete
 * Parameters:
 *      unit            (IN) BCM unit number
 *      port_class      (IN) Group ID of ingress port
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 */
int
_bcm_esw_vlan_translate_egress_action_delete(int unit, int port_class,
                                             bcm_vlan_t outer_vlan,
                                             bcm_vlan_t inner_vlan,
                                             bcm_vlan_translate_key_t key_type,
                                             int handle_modport_gport)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid = -1;
    int                 id = -1; 

    CHECK_INIT(unit); 
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3) 
    if (soc_feature(unit, soc_feature_vp_sharing)) { 
        int num_vfi = 0;
        bcm_vpn_t vpn_min = 0, vpn_max = 0;
        if (outer_vlan > BCM_VLAN_MAX) {
            num_vfi = soc_mem_index_count(unit, VFIm);
            _BCM_VPN_SET(vpn_min, _BCM_VPN_TYPE_VFI, 0);
            vpn_max = vpn_min + num_vfi - 1;
            if (outer_vlan > vpn_max) {
                return (BCM_E_PARAM);
            } else if ((outer_vlan >= vpn_min) &&
                       (outer_vlan <= vpn_max)) {
                if (inner_vlan != BCM_VLAN_INVALID) {
                    return BCM_E_PARAM;
                }
            } else {
                return BCM_E_PARAM;
            }
        }  else {
            VLAN_CHK_ID(unit, inner_vlan);
        }
    } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT && INCLUDE_L3 */
    {
        VLAN_CHK_ID(unit, outer_vlan);
        VLAN_CHK_ID(unit, inner_vlan);
    }

    if (BCM_GPORT_IS_SET(port_class)) {
        if (!BCM_GPORT_IS_TUNNEL(port_class) &&
            !BCM_GPORT_IS_MIM_PORT(port_class) &&
            !BCM_GPORT_IS_NIV_PORT(port_class) &&
            !BCM_GPORT_IS_EXTENDER_PORT(port_class) &&
            !BCM_GPORT_IS_MPLS_PORT(port_class) &&
            !BCM_GPORT_IS_VLAN_PORT(port_class) &&
            !BCM_GPORT_IS_VXLAN_PORT(port_class) &&
            !BCM_GPORT_IS_L2GRE_PORT(port_class) &&
            !BCM_GPORT_IS_FLOW_PORT(port_class) &&
            !(handle_modport_gport &&
              (BCM_GPORT_IS_MODPORT(port_class) ||
              BCM_GPORT_IS_SUBPORT_PORT(port_class))))
        {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port_class, &modid, &port_class, 
                                       &tgid, &id));
        }
        if ((-1 != tgid || -1 != id) && !BCM_GPORT_IS_TUNNEL(port_class) &&
            !BCM_GPORT_IS_MIM_PORT(port_class) &&
            !BCM_GPORT_IS_NIV_PORT(port_class) &&
            !BCM_GPORT_IS_EXTENDER_PORT(port_class) &&
            !BCM_GPORT_IS_MPLS_PORT(port_class) &&
            !BCM_GPORT_IS_VLAN_PORT(port_class) &&
            !BCM_GPORT_IS_VXLAN_PORT(port_class) &&
            !BCM_GPORT_IS_FLOW_PORT(port_class) &&
            !BCM_GPORT_IS_L2GRE_PORT(port_class)) 
        {
            return BCM_E_PORT;
        }
    } else {
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3) 
        if (inner_vlan == BCM_VLAN_INVALID) {
            BCM_IF_ERROR_RETURN
                       (bcm_td2p_dvp_group_validate(unit, port_class));
        } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT && INCLUDE_L3 */
        if (!SOC_PORT_CLASS_VALID(unit, port_class)) { 
            return BCM_E_PORT; 
        }
    }

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        return _bcm_trx_vlan_translate_egress_action_delete(unit,
                                                           port_class,
                                                           outer_vlan,
                                                           inner_vlan, key_type);
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function   :
 *      bcm_vlan_translate_egress_action_add
 * Description   :
 *      Add an entry to egress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port_class      (IN) Group ID of ingress port
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 *      action          (IN) Action for outer and inner tag
 */
int
bcm_esw_vlan_translate_egress_action_add(int unit, int port_class,
                                         bcm_vlan_t outer_vlan,
                                         bcm_vlan_t inner_vlan,
                                         bcm_vlan_action_set_t *action)
{
    int handle_modport_gport = 0;
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
    return _bcm_esw_vlan_translate_egress_action_add(unit, port_class,
            outer_vlan, inner_vlan, 0, action, handle_modport_gport);
}

/*
 * Function   :
 *      bcm_vlan_translate_egress_action_get
 * Description   :
 *      Get an entry to egress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port_class      (IN) Group ID of ingress port
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 *      action          (OUT) Action for outer and inner tag
 */
int
bcm_esw_vlan_translate_egress_action_get (int unit, int port_class,
                                          bcm_vlan_t outer_vlan,
                                          bcm_vlan_t inner_vlan,
                                          bcm_vlan_action_set_t *action)
{
    int handle_modport_gport = 0;
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    bcm_vlan_action_set_t_init(action);
    return _bcm_esw_vlan_translate_egress_action_get(unit, port_class,
            outer_vlan, inner_vlan, 0, action, handle_modport_gport);

}

/*
 * Function:
 *      bcm_vlan_translate_egress_action_delete
 * Purpose:
 *      Delete an egress vlan translate lookup entry.
 * Parameters:
 *      unit            (IN) BCM unit number
 *      port_class      (IN) Group ID of ingress port
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 */
int
bcm_esw_vlan_translate_egress_action_delete(int unit, int port_class,
                                            bcm_vlan_t outer_vlan,
                                            bcm_vlan_t inner_vlan)
{
    int handle_modport_gport = 0;
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
    return _bcm_esw_vlan_translate_egress_action_delete(unit, port_class,
            outer_vlan, inner_vlan, 0, handle_modport_gport);
}

/*
 * Function   :
 *      bcm_esw_vlan_translate_egress_gport_action_add
 * Description   :
 *      Add an entry to egress VLAN translation table based on gport
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) Egress gport
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 *      action          (IN) Action for outer and inner tag
 */
int
bcm_esw_vlan_translate_egress_gport_action_add (int unit, bcm_gport_t port,
                                                bcm_vlan_t outer_vlan,
                                                bcm_vlan_t inner_vlan,
                                                bcm_vlan_action_set_t *action)
{
    int handle_modport_gport = 0;
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
    if (soc_feature(unit, soc_feature_egr_vxlate_supports_dgpp)) {
        handle_modport_gport = 1;
    }
    return _bcm_esw_vlan_translate_egress_action_add(unit, port,
            outer_vlan, inner_vlan, 0, action, handle_modport_gport);
}

/*
 * Function   :
 *      bcm_esw_vlan_translate_egress_gport_action_get
 * Description   :
 *      Get a gport based entry from egress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) Egress gport
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 *      action          (OUT) Action for outer and inner tag
 */
int 
bcm_esw_vlan_translate_egress_gport_action_get (int unit, bcm_gport_t port,
                                                bcm_vlan_t outer_vlan,
                                                bcm_vlan_t inner_vlan,
                                                bcm_vlan_action_set_t *action)
{
    int handle_modport_gport = 0;
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
    if (soc_feature(unit, soc_feature_egr_vxlate_supports_dgpp)) {
        handle_modport_gport = 1;
    }

    bcm_vlan_action_set_t_init(action);
    return _bcm_esw_vlan_translate_egress_action_get(unit, port,
            outer_vlan, inner_vlan, 0, action, handle_modport_gport);
}

/*
 * Function:
 *      bcm_esw_vlan_translate_egress_gport_action_delete
 * Purpose:
 *      Delete a gport based egress vlan translate entry.
 * Parameters:
 *      unit            (IN) BCM unit number
 *      port            (IN) Egress gport
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 */
int
bcm_esw_vlan_translate_egress_gport_action_delete(int unit, bcm_gport_t port,
                                                  bcm_vlan_t outer_vlan,
                                                  bcm_vlan_t inner_vlan)
{
    int handle_modport_gport = 0;
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
    if (soc_feature(unit, soc_feature_egr_vxlate_supports_dgpp)) {
        handle_modport_gport = 1;
    }
    return _bcm_esw_vlan_translate_egress_action_delete(unit, port,
            outer_vlan, inner_vlan, 0, handle_modport_gport);
}

/*
 * Function   :
 *      bcm_esw_vlan_translate_egress_action_extended_add
 * Description   :
 *      Add an entry to egress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      key             (IN) Key fields for egress translate lookup
 *      action          (IN) Actions for egress translate
 */
int
bcm_esw_vlan_translate_egress_action_extended_add (int unit,
                           bcm_vlan_translate_egress_key_config_t *key,
                           bcm_vlan_translate_egress_action_set_t *action)
{
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        int handle_modport_gport = 0;
        if (soc_feature(unit, soc_feature_egr_vxlate_supports_dgpp)) {
            handle_modport_gport = 1;
        }
        return _bcm_esw_vlan_translate_egress_action_add (unit,
             key->port, key->outer_vlan, key->inner_vlan, key->key_type,
             &action->vlan_action, handle_modport_gport);
    }
#endif /* BCM_TRIDENT3_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function   :
 *      bcm_esw_vlan_translate_egress_action_extended_get
 * Description   :
 *      Get a gport based entry from egress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      key             (IN) Key fields for egress translate lookup
 *      action          (OUT) Action for outer and inner tag
 */
int
bcm_esw_vlan_translate_egress_action_extended_get (int unit,
                           bcm_vlan_translate_egress_key_config_t *key,
                           bcm_vlan_translate_egress_action_set_t *action)
{
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        int handle_modport_gport = 0;
        if (soc_feature(unit, soc_feature_egr_vxlate_supports_dgpp)) {
            handle_modport_gport = 1;
        }
        bcm_vlan_translate_egress_action_set_t_init(action);
        return _bcm_esw_vlan_translate_egress_action_get(unit,
                key->port, key->outer_vlan, key->inner_vlan, key->key_type,
                &action->vlan_action, handle_modport_gport);
    }
#endif /* BCM_TRIDENT3_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vlan_translate_egress_gport_action_delete
 * Purpose:
 *      Delete a gport based egress vlan translate entry.
 * Parameters:
 *      unit            (IN) BCM unit number
 *      key             (IN) Key fields for egress translate lookup
 */
int
bcm_esw_vlan_translate_egress_action_extended_delete (int unit,
                           bcm_vlan_translate_egress_key_config_t *key)
{
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        int handle_modport_gport = 0;
        if (soc_feature(unit, soc_feature_egr_vxlate_supports_dgpp)) {
            handle_modport_gport = 1;
        }
        return _bcm_esw_vlan_translate_egress_action_delete(unit,
                 key->port, key->outer_vlan, key->inner_vlan, key->key_type,
                 handle_modport_gport);
    }
#endif /* BCM_TRIDENT3_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function   :
 *      bcm_esw_vlan_translate_egress_action_extended_traverse
 * Description   :
 *      Traverse over all translate entries and call given callback with
 *      action structure
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) Call back function
 *      user_data       (IN) User provided data to pass to a call back
 */
int
bcm_esw_vlan_translate_egress_action_extended_traverse(int unit,
                       bcm_vlan_translate_egress_action_extended_traverse_cb cb,
                       void *user_data)
{
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        _bcm_vlan_translate_traverse_t          trvs_st;
        bcm_vlan_action_set_t                   action;
        _translate_egress_action_extended_traverse_cb_t  usr_cb_st;
        soc_mem_t mem = EGR_VLAN_XLATE_1_DOUBLEm;

        if (!cb) {
            return (BCM_E_PARAM);
        }

        sal_memset(&trvs_st, 0, sizeof(_bcm_vlan_translate_traverse_t));
        sal_memset(&action, 0, sizeof(bcm_vlan_action_set_t));
        sal_memset(&usr_cb_st, 0, sizeof(_translate_egress_action_extended_traverse_cb_t));

        usr_cb_st.usr_cb = cb;

        trvs_st.user_data = user_data;
        trvs_st.action = &action;
        trvs_st.user_cb_st = (void *)&usr_cb_st;

        trvs_st.int_cb = _bcm_esw_vlan_translate_egress_action_extended_traverse_int_cb;
        BCM_IF_ERROR_RETURN(
            _bcm_esw_vlan_translate_traverse_mem(unit, mem, &trvs_st));

        /* Now traverse EVXLT_2 */
        mem = EGR_VLAN_XLATE_2_DOUBLEm;
        return _bcm_esw_vlan_translate_traverse_mem(unit, mem, &trvs_st);
        return BCM_E_NONE;
    }
#endif /* BCM_TRIDENT3_SUPPORT */
    return BCM_E_UNAVAIL;
}


int
bcm_esw_vlan_dtag_add(int unit, int port, bcm_vlan_t inner_vid,
                      bcm_vlan_t outer_vid, int prio)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid = -1;
    int                 id = -1;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    CHECK_INIT(unit);
    VLAN_CHK_ID(unit, inner_vid);
    VLAN_CHK_ID(unit, outer_vid);

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        bcm_vlan_action_set_t action;
        bcm_gport_t gport;

        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
        if (BCM_GPORT_IS_SET(port)) {
            gport = port;
        } else {
            if (!SOC_PORT_VALID(unit, port)) { 
                return BCM_E_PORT; 
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_gport_get(unit, port, &gport));
        }

        bcm_vlan_action_set_t_init(&action);
        action.new_outer_vlan = outer_vid;
        action.priority = prio;
        /* For inner tagged packets, set the outer tag action and outer pkt 
         * priority action to ADD.
         * For all other packet types, the action is initialized to NONE.
         */
        action.it_outer = bcmVlanActionAdd;
        if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            action.it_outer_pkt_prio = bcmVlanActionAdd;
        } else {
            action.it_outer_pkt_prio = bcmVlanActionNone;
        }
        return _bcm_trx_vlan_translate_action_add(unit, gport,
                                                 bcmVlanTranslateKeyPortInner,
                                                 0, inner_vid, &action);
    }
#endif /* BCM_TRX_SUPPORT */

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if (-1 != tgid || -1 != id) {
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_stk_my_modid_get(unit, &modid));
    }

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_vlan_translate_add(unit, port, inner_vid, outer_vid,
                                          prio, BCM_VLAN_XLATE_DTAG);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vlan_dtag_get
 * Purpose:
 *      Get vlan translation for double tagging
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - port numebr
 *      old_vid - Old VLAN ID to has translation for
 *      new_vid - New VLAN ID that packet will get
 *      prio    - Priority
 * Returns:
 *      BCM_E_NONE - Translation found, new_vid and prio will have the values.
 *      BCM_E_NOT_FOUND - Translation does not exist
 *      BCM_E_XXX  - Other error
 * Notes:
 *      None.
 */
int 
bcm_esw_vlan_dtag_get (int unit, bcm_port_t port,
                       bcm_vlan_t old_vid,
                       bcm_vlan_t *new_vid,
                       int *prio)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid = -1;
    int                 id = -1;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    CHECK_INIT(unit);
    VLAN_CHK_ID(unit, old_vid);

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        bcm_gport_t gport;
        bcm_vlan_action_set_t action;

        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
        if (BCM_GPORT_IS_SET(port)) {
            gport = port;
        } else {
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_gport_get(unit, port, &gport));
        }
        bcm_vlan_action_set_t_init(&action);
        BCM_IF_ERROR_RETURN(
            _bcm_trx_vlan_translate_action_get(unit, gport,
                                                 bcmVlanTranslateKeyPortInner,
                                               0, old_vid, &action));
        if (bcmVlanActionAdd == action.it_outer) {
            *new_vid = action.new_outer_vlan;
            *prio = action.priority;

            return BCM_E_NONE;
        } 

        return BCM_E_NOT_FOUND;
    }
#endif /* BCM_TRX_SUPPORT */

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if (-1 != tgid || -1 != id) {
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_stk_my_modid_get(unit, &modid));
    }

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_vlan_translate_get(unit, port, old_vid, new_vid,
                                          prio, BCM_VLAN_XLATE_DTAG);
    }
#endif

    return BCM_E_UNAVAIL;
}


int
bcm_esw_vlan_dtag_delete(int unit, int port, bcm_vlan_t vid)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid = -1;
    int                 id = -1;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    CHECK_INIT(unit);
    VLAN_CHK_ID(unit, vid);

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        bcm_gport_t gport;

        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
        if (BCM_GPORT_IS_SET(port)) {
            gport = port;
        } else {
            if (!SOC_PORT_VALID(unit, port)) { 
                return BCM_E_PORT; 
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_gport_get(unit, port, &gport));
        }

        return _bcm_trx_vlan_translate_action_delete(unit, gport,
                                                    bcmVlanTranslateKeyPortInner,
                                                    0, vid);
    }
#endif /* BCM_TRX_SUPPORT */

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if (-1 != tgid || -1 != id) {
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_stk_my_modid_get(unit, &modid));
    }

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_vlan_translate_delete(unit, port, vid,
                                             BCM_VLAN_XLATE_DTAG);
    }
#endif

    return BCM_E_UNAVAIL;
}

int
bcm_esw_vlan_dtag_delete_all(int unit)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    CHECK_INIT(unit);

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
        return _bcm_trx_vlan_translate_action_delete_all(unit);
    }
#endif /* BCM_TRX_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        int rv; 

        rv = _bcm_fb_vlan_translate_delete(unit, -1, BCM_VLAN_NONE,
                                             BCM_VLAN_XLATE_DTAG);
        if (BCM_SUCCESS(rv) || (BCM_E_NOT_FOUND == rv)) {
            return BCM_E_NONE;
        }

        return rv;
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * IP based vlans
 */

#ifdef BCM_XGS3_SWITCH_SUPPORT
static void
_ip6_prefix_to_mask(int prefix, bcm_ip6_t mask)
{
    int i;

    sal_memset(mask, 0, sizeof(bcm_ip6_t));
    /* Construct the mask from the prefix */
    for (i = 0; prefix > 8; prefix -= 8, i++) {
        mask[i] = 0xff;
    }
    mask[i] = 0xff << (8 - prefix);
}

static int
_ip6_mask_to_prefix(bcm_ip6_t mask)
{
    int i;
    int prefix;
    for (i = 0, prefix = 0; mask[i] == 0xff; i++) {
        prefix += 8;
    }
    switch(mask[i]) {
        case 0x80:
            prefix += 1;
            break;
        case 0xc0:
            prefix += 2;
            break;
        case 0xe0:
            prefix += 3;
            break;
        case 0xf0:
            prefix += 4;
            break;
        case 0xf8:
            prefix += 5;
            break;
        case 0xfc:
            prefix += 6;
            break;
        case 0xfe:
            prefix += 7;
            break;
        case 0:
            break;
        default:
            return(-1);
    }
    return prefix;
}

static void
_ip4cat(uint8 * buf, bcm_ip_t ip)
{
    int i;
    for (i = 3; i > -1; i--) {
        buf[i] = ip & 0x00ff;
        ip >>= 8;
    }
}

/*
 * On XGS3, VLAN_SUBNET.IP_ADDR stores only a top half (64 bits) of
 * the IPV6 address. Comparing it against a full length of a parameter
 * will have certainly result in a mismatch. There should be a
 * special type for a stored part of the address; meanwhile, a
 * symbolic constant is used.
 */

#define VLAN_SUBNET_IP_ADDR_LENGTH 8

/* Macro to copy bcm_ip6_t addr into uint32[] for soc_mem_field_set call */
#define VLAN_IP6_TO_HW_FORMAT(_ip, _reg)        \
    do { \
        _reg[0] = (_ip[4] << 24) | (_ip[5] << 16) | (_ip[6] << 8) | _ip[7]; \
        _reg[1] = (_ip[0] << 24) | (_ip[1] << 16) | (_ip[2] << 8) | _ip[3]; \
    } while (0)

/* Macro to copy uint32[] addr values from soc_mem_field_get into bcm_ip6_t */
#define VLAN_HW_FORMAT_TO_IP6(_reg, _ip)        \
    do { \
       _ip[3] = (_reg[1]) & 0xff;               \
       _ip[2] = (_reg[1] >> 8) & 0xff;          \
       _ip[1] = (_reg[1] >> 16) & 0xff;         \
       _ip[0] = (_reg[1] >> 24) & 0xff;         \
       _ip[7] = (_reg[0]) & 0xff;               \
       _ip[6] = (_reg[0] >> 8) & 0xff;          \
       _ip[5] = (_reg[0] >> 16) & 0xff;         \
       _ip[4] = (_reg[0] >> 24) & 0xff;         \
    } while (0)

static int
_xgs3_vlan_ipv6_add(int unit,
                    bcm_ip6_t ip, bcm_ip6_t mask, bcm_vlan_t vid, int prio)
{
    int i, imin, imax, nent, vsbytes;
    int match = -1, empty = -1, insert = -1;
    int rv;
    vlan_subnet_entry_t * vstab, * vsnull, *vstabp;
    int prefix;
    uint32 hw_ip[2], hw_nm[2];

    VLAN_CHK_ID(unit, vid);
    VLAN_CHK_PRIO(unit, prio);

    VLAN_IP6_TO_HW_FORMAT(ip, hw_ip);
    VLAN_IP6_TO_HW_FORMAT(mask, hw_nm);

    imin = soc_mem_index_min(unit, VLAN_SUBNETm);
    imax = soc_mem_index_max(unit, VLAN_SUBNETm);
    nent = soc_mem_index_count(unit, VLAN_SUBNETm);
    vsbytes = soc_mem_entry_words(unit, VLAN_SUBNETm);
    vsbytes = WORDS2BYTES(vsbytes);

    prefix = _ip6_mask_to_prefix(mask);
    if (prefix < 0) {
        return BCM_E_PARAM;
    }

    vstab = soc_cm_salloc(unit, nent * sizeof(*vstab), "vlan_subnet");
    if (vstab == NULL) {
        return BCM_E_MEMORY;
    }

    vsnull = soc_mem_entry_null(unit, VLAN_SUBNETm);

    soc_mem_lock(unit, VLAN_SUBNETm);
    rv = soc_mem_read_range(unit, VLAN_SUBNETm, MEM_BLOCK_ANY,
                            imin, imax, vstab);
    if (rv < 0) {
        soc_mem_unlock(unit, VLAN_SUBNETm);
        soc_cm_sfree(unit, vstab);
        return rv;
    }

    for(i = 0; i < nent; i++) {
        uint32 enm[2], eip[2];
        bcm_ip6_t temp_nm;
        int epx;
        vstabp = soc_mem_table_idx_to_pointer(unit,
                        VLAN_SUBNETm, vlan_subnet_entry_t *,
                        vstab, i);
        if (sal_memcmp(vstabp, vsnull, vsbytes) == 0) {
            empty = i;
            break;
        }
        soc_mem_field_get(unit, VLAN_SUBNETm,
                (uint32 *) vstabp, MASKf, (uint32 *) enm);

        /* copy mask into bcm_ip6_t */
        sal_memset(temp_nm, 0, sizeof(bcm_ip6_t));
        VLAN_HW_FORMAT_TO_IP6(enm, temp_nm);

        epx = _ip6_mask_to_prefix(temp_nm);
        if (epx < 0) {
            soc_mem_unlock(unit, VLAN_SUBNETm);
            soc_cm_sfree(unit, vstab);
            return BCM_E_PARAM;
        }
        if ((epx < prefix) && (insert < 0)) {
            insert = i;
        }
        if (sal_memcmp(hw_nm, enm, VLAN_SUBNET_IP_ADDR_LENGTH) != 0) {
            continue;
        }
        soc_mem_field_get(unit, VLAN_SUBNETm,
                (uint32 *) vstabp, IP_ADDRf, (uint32 *) eip);

        if (sal_memcmp(hw_ip, eip, VLAN_SUBNET_IP_ADDR_LENGTH) == 0) {
            match = i;
        }
    }

    if (match >= 0) {   /* found an exact match */
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                            vlan_subnet_entry_t *, vstab, match);
        soc_VLAN_SUBNETm_field32_set(unit, vstabp, VLAN_IDf, vid);
        soc_VLAN_SUBNETm_field32_set(unit, vstabp, PRIf, prio);
        soc_VLAN_SUBNETm_field32_set(unit, vstabp, VALIDf, 1);
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
        if (soc_mem_field_valid(unit, VLAN_SUBNETm, RESERVED_MASKf)) {
            soc_mem_field32_set(unit, VLAN_SUBNETm, vstabp, RESERVED_MASKf, 0);
        }
        if (soc_mem_field_valid(unit, VLAN_SUBNETm, RESERVED_KEYf)) {
            soc_mem_field32_set(unit, VLAN_SUBNETm, vstabp, RESERVED_KEYf, 0);
        }
#endif /* BCM_RAVEN_SUPPORT || BCM_SCORPION_SUPPORT */
        rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ALL, match, vstabp);
    } else if (empty < 0) {
        rv = BCM_E_FULL;
    } else {
        if (insert < 0) {
            insert = empty;
        }

        for (i = empty; i > insert; i--) {
            vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                            vlan_subnet_entry_t *, vstab, (i - 1));
            rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ANY, i, vstabp);
            if (rv < 0) {
                break;
            }
            rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ANY, i - 1, &vsnull);
            if (rv < 0) {
                break;
            }
        }
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                        vlan_subnet_entry_t *, vstab, insert);
        sal_memset(vstabp, 0, vsbytes);
        soc_VLAN_SUBNETm_field32_set(unit, vstabp, VLAN_IDf, vid);
        soc_VLAN_SUBNETm_field32_set(unit, vstabp, PRIf, prio);
        soc_VLAN_SUBNETm_field32_set(unit, vstabp, VALIDf, 1);
        soc_mem_field_set(unit, VLAN_SUBNETm,
                (uint32 *) vstabp, IP_ADDRf, (uint32 *) hw_ip);
        soc_mem_field_set(unit, VLAN_SUBNETm,
                (uint32 *) vstabp, MASKf, (uint32 *) hw_nm);
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
        if (soc_mem_field_valid(unit, VLAN_SUBNETm, RESERVED_MASKf)) {
            soc_mem_field32_set(unit, VLAN_SUBNETm, vstabp, RESERVED_MASKf, 0);
        }
        if (soc_mem_field_valid(unit, VLAN_SUBNETm, RESERVED_KEYf)) {
            soc_mem_field32_set(unit, VLAN_SUBNETm, vstabp, RESERVED_KEYf, 0);
        }
#endif /* BCM_RAVEN_SUPPORT || BCM_SCORPION_SUPPORT */

        if (rv >= 0) {
            rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ALL,
                    insert, vstabp);
        }
    }

    soc_mem_unlock(unit, VLAN_SUBNETm);
    soc_cm_sfree(unit, vstab);
    return rv;
}

static int
_xgs3_vlan_ipv6_delete(int unit, bcm_ip6_t ip, bcm_ip6_t mask)
{
    int i, imin, imax, nent, vsbytes;
    int match = -1, empty = -1;
    int rv;
    vlan_subnet_entry_t * vstab, * vsnull,  *vstabp;
    uint32 hw_ip[2], hw_nm[2];

    VLAN_IP6_TO_HW_FORMAT(ip, hw_ip);
    VLAN_IP6_TO_HW_FORMAT(mask, hw_nm);
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        hw_ip[0] &= hw_nm[0];
        hw_ip[1] &= hw_nm[1];
    }
#endif /* BCM_TRIDENT_SUPPORT */

    imin = soc_mem_index_min(unit, VLAN_SUBNETm);
    imax = soc_mem_index_max(unit, VLAN_SUBNETm);
    nent = soc_mem_index_count(unit, VLAN_SUBNETm);
    vsbytes = soc_mem_entry_words(unit, VLAN_SUBNETm);
    vsbytes = WORDS2BYTES(vsbytes);
    vstab = soc_cm_salloc(unit, nent * sizeof(*vstab), "vlan_subnet");

    if (vstab == NULL) {
        return BCM_E_MEMORY;
    }

    vsnull = soc_mem_entry_null(unit, VLAN_SUBNETm);

    soc_mem_lock(unit, VLAN_SUBNETm);
    rv = soc_mem_read_range(unit, VLAN_SUBNETm, MEM_BLOCK_ANY,
                            imin, imax, vstab);
    if (rv < 0) {
        soc_mem_unlock(unit, VLAN_SUBNETm);
        soc_cm_sfree(unit, vstab);
        return rv;
    }

    for(i = 0; i < nent; i++) {
        uint32 enm[2], eip[2];
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                        vlan_subnet_entry_t *, vstab, i);
        if (sal_memcmp(vstabp, vsnull, vsbytes) == 0) {
            empty = i;
            break;
        }
        soc_mem_field_get(unit, VLAN_SUBNETm,
                (uint32 *) vstabp, MASKf, (uint32 *) enm);
        if (sal_memcmp(hw_nm, enm, VLAN_SUBNET_IP_ADDR_LENGTH) != 0) {
            continue;
        }
        soc_mem_field_get(unit, VLAN_SUBNETm,
                (uint32 *) vstabp, IP_ADDRf, (uint32 *) eip);
        if (sal_memcmp(hw_ip, eip, VLAN_SUBNET_IP_ADDR_LENGTH) == 0) {
            match = i;
        }
    }

    if (match < 0) {
        soc_mem_unlock(unit, VLAN_SUBNETm);
        soc_cm_sfree(unit, vstab);
        return BCM_E_NOT_FOUND;
    }

    if (empty == -1) {
        empty = nent;
    }

    for (i = match; (i < empty - 1) && (rv == BCM_E_NONE); i++) {
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                        vlan_subnet_entry_t *, vstab, (i + 1));
        rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ANY, i, vstabp);
    }

    if (rv >= 0) {
        rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ANY, empty - 1, vsnull);
    }

    soc_mem_unlock(unit, VLAN_SUBNETm);
    soc_cm_sfree(unit, vstab);
    return rv;
}

static int
_xgs3_vlan_ipv6_delete_all(int unit)
{
    int i, imax;
    int rv = BCM_E_NONE;
    vlan_subnet_entry_t * vsnull = soc_mem_entry_null(unit, VLAN_SUBNETm);

    soc_mem_lock(unit, VLAN_SUBNETm);
    imax = soc_mem_index_max(unit, VLAN_SUBNETm);

    for (i = soc_mem_index_min(unit, VLAN_SUBNETm); i <= imax; i++) {
        rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ALL, i, vsnull);
        if (rv < 0) {
            break;
        }
    }

    soc_mem_unlock(unit, VLAN_SUBNETm);
    return rv;
}
#endif

/*
 * Vlan selection based on IPv4 addresses
 */

/*
 * Function:
 *      bcm_vlan_ip4_add
 * Purpose:
 *      Add an IPv4 subnet lookup to select vlan and priority for
 *      untagged packets
 * Parameters:
 *      unit -          device number
 *      ipaddr -        IPv4 address
 *      netmask -       network mask of ipaddr to match
 *      vid -           VLAN number to give matching packets
 *      prio -          priority to give matching packets
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      This API is superseded by bcm_vlan_ip_add.
 */

int
bcm_esw_vlan_ip4_add(int unit, bcm_ip_t ipaddr, bcm_ip_t netmask,
                     bcm_vlan_t vid, int prio)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_XGS3_FABRIC(unit)) {
        bcm_ip6_t ip6addr;
        bcm_ip6_t ip6mask;
        uint8 * ptr;

#ifdef BCM_SCORPION_SUPPORT
        if (SOC_IS_SC_CQ(unit) &&
            !soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
#endif

#ifdef BCM_TRIDENT_SUPPORT
        if (SOC_IS_TD_TT(unit) &&
            !soc_feature(unit, soc_feature_ip_subnet_based_vlan)) {
            return BCM_E_UNAVAIL;
        }
#endif

        /* Construct ipv6 mask/address from ipv4 parameters.
         * Only use the upper 64-bits of ipv6 mask/address.
         */
        ptr = ip6addr;
        sal_memset(ptr, 0, sizeof(ip6addr));
        sal_memcpy(ptr, "\xff\xff\x00\x00", 4);
        _ip4cat(ptr + 4, ipaddr);

        ptr = ip6mask;
        sal_memset(ptr, 0, sizeof(ip6mask));
        sal_memcpy(ptr, "\xff\xff\xff\xff", 4);
        _ip4cat(ptr + 4, netmask);

        return _xgs3_vlan_ipv6_add(unit, ip6addr, ip6mask, vid, prio);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_vlan_ip4_delete
 * Purpose:
 *      Delete an IPv4 subnet lookup entry.
 * Parameters:
 *      unit -          device number
 *      ipaddr -        IPv4 address
 *      netmask -       network mask of ipaddr to match
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      This API is superseded by bcm_vlan_ip_delete.
 */

int
bcm_esw_vlan_ip4_delete(int unit, bcm_ip_t ipaddr, bcm_ip_t netmask)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_XGS3_FABRIC(unit)) {
        bcm_ip6_t ip6addr;
        bcm_ip6_t ip6mask;
        uint8 * ptr;

#ifdef BCM_SCORPION_SUPPORT
        if (SOC_IS_SC_CQ(unit) &&
            !soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
#endif

#ifdef BCM_TRIDENT_SUPPORT
        if (SOC_IS_TD_TT(unit) &&
            !soc_feature(unit, soc_feature_ip_subnet_based_vlan)) {
            return BCM_E_UNAVAIL;
        }
#endif

        /* Construct ipv6 mask/address from ipv4 parameters */
        ptr = ip6addr;
        sal_memset(ptr, 0, sizeof(ip6addr));
        sal_memcpy(ptr, "\xff\xff\x00\x00", 4);
        _ip4cat(ptr + 4, ipaddr);

        ptr = ip6mask;
        sal_memset(ptr, 0, sizeof(ip6mask));
        sal_memcpy(ptr, "\xff\xff\xff\xff", 4);
        _ip4cat(ptr + 4, netmask);

        return _xgs3_vlan_ipv6_delete(unit, ip6addr, ip6mask);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_vlan_ip4_delete_all
 * Purpose:
 *      Delete all IPv4 subnet lookup entries.
 * Parameters:
 *      unit -          device number
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      This API is superseded by bcm_vlan_ip_delete_all.
 */

int
bcm_esw_vlan_ip4_delete_all(int unit)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_XGS3_FABRIC(unit)) {
#ifdef BCM_SCORPION_SUPPORT
        if (SOC_IS_SC_CQ(unit) &&
            !soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
#endif

#ifdef BCM_TRIDENT_SUPPORT
        if (SOC_IS_TD_TT(unit) &&
            !soc_feature(unit, soc_feature_ip_subnet_based_vlan)) {
            return BCM_E_UNAVAIL;
        }
#endif

        return _xgs3_vlan_ipv6_delete_all(unit);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Vlan selection based on IPv6 addresses
 */

/*
 * Function:
 *      _bcm_vlan_ip6_add (internal)
 * Purpose:
 *      Add an IPv6 subnet lookup to select vlan and priority for
 *      untagged packets
 * Parameters:
 *      unit -          device number
 *      ipaddr -        IPv6 address
 *      prefix -        network prefix of ipaddr to match
 *      vlan -          VLAN number to give matching packets
 *      prio -          priority to give matching packets
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Valid prefix lengths may be limited to no more than 64
 */

STATIC int
_bcm_vlan_ip6_add(int unit,
                  bcm_ip6_t ipaddr,
                  int prefix,
                  bcm_vlan_t vlan,
                  int prio)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_XGS3_FABRIC(unit)) {
        bcm_ip6_t mask;

        if (prefix > 64) {
            return BCM_E_PARAM;
        }

        _ip6_prefix_to_mask(prefix, mask);
        return _xgs3_vlan_ipv6_add(unit, ipaddr, mask, vlan, prio);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_vlan_ip6_delete (internal)
 * Purpose:
 *      Delete an IPv6 subnet lookup entry
 * Parameters:
 *      unit -          device number
 *      ipaddr -        IPv6 address
 *      prefix -        network prefix of ipaddr to match
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_vlan_ip6_delete(int unit,
                     bcm_ip6_t ipaddr,
                     int prefix)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_XGS3_FABRIC(unit)) {
        bcm_ip6_t mask;

        if (prefix > 64) {
            return BCM_E_PARAM;
        }

        _ip6_prefix_to_mask(prefix, mask);
        return _xgs3_vlan_ipv6_delete(unit, ipaddr, mask);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_vlan_ip6_delete_all (internal)
 * Purpose:
 *      Delete all IPv6 subnet lookup entries
 * Parameters:
 *      unit -          device number
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_vlan_ip6_delete_all(int unit)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_XGS3_FABRIC(unit)) {
        return _xgs3_vlan_ipv6_delete_all(unit);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Vlan selection based on unified IPv4/IPv6 information structure.
 */

/*
 * Function:
 *      bcm_vlan_ip_add
 * Purpose:
 *      Add a subnet lookup to select vlan and priority for
 *      untagged packets
 * Parameters:
 *      unit -          device number
 *      vlan_ip -       structure specifying IP address and other info
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_vlan_ip_add(int unit, bcm_vlan_ip_t *vlan_ip)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        int rv; 

        if (soc_feature(unit, soc_feature_vlan_action)
            && soc_feature(unit, soc_feature_ip_subnet_based_vlan)) {
            bcm_vlan_action_set_t action;

            bcm_vlan_action_set_t_init(&action);
            action.new_outer_vlan = vlan_ip->vid;
            action.new_inner_vlan = 0;
            action.priority = vlan_ip->prio;
            action.dt_outer_prio = bcmVlanActionReplace;
            action.ot_outer_prio = bcmVlanActionReplace;
            action.it_outer      = bcmVlanActionAdd;
            action.it_inner_prio = bcmVlanActionNone;
            action.ut_outer      = bcmVlanActionAdd;

            soc_mem_lock(unit, VLAN_SUBNETm);
            rv =  _bcm_trx_vlan_ip_action_add(unit, vlan_ip, &action);
            soc_mem_unlock(unit, VLAN_SUBNETm);

            return rv;
        } else {
            return BCM_E_UNAVAIL;  /* Latency bypass */
        }
    }
#endif /* BCM_TRX_SUPPORT */

    return (vlan_ip->flags & BCM_VLAN_SUBNET_IP6)
        ? _bcm_vlan_ip6_add(unit, vlan_ip->ip6, vlan_ip->prefix,
                vlan_ip->vid, vlan_ip->prio)
        : bcm_esw_vlan_ip4_add(unit, vlan_ip->ip4, vlan_ip->mask,
                vlan_ip->vid, vlan_ip->prio);
}

/*
 * Function:
 *      bcm_vlan_ip_delete
 * Purpose:
 *      Delete a subnet lookup entry.
 * Parameters:
 *      unit -          device number
 *      vlan_ip -       structure specifying IP address and other info
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_vlan_ip_delete(int unit, bcm_vlan_ip_t *vlan_ip)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        int rv;

        if (soc_feature(unit, soc_feature_vlan_action)
            && soc_feature(unit, soc_feature_ip_subnet_based_vlan)) {
            soc_mem_lock(unit, VLAN_SUBNETm);
            rv = _bcm_trx_vlan_ip_delete(unit, vlan_ip);
            soc_mem_unlock(unit, VLAN_SUBNETm);

            return rv;
        } else {
            return BCM_E_UNAVAIL;  /* Latency bypass */
        }
    }
#endif /* BCM_TRX_SUPPORT */

    return (vlan_ip->flags & BCM_VLAN_SUBNET_IP6)
        ? _bcm_vlan_ip6_delete(unit, vlan_ip->ip6, vlan_ip->prefix)
        : bcm_esw_vlan_ip4_delete(unit, vlan_ip->ip4, vlan_ip->mask);
}

/*
 * Function:
 *      bcm_vlan_ip_delete_all
 * Purpose:
 *      Delete all subnet lookup entries.
 * Parameters:
 *      unit -          device number
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_vlan_ip_delete_all(int unit)
{
    int rv;

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        if (soc_feature(unit, soc_feature_vlan_action)
            && soc_feature(unit, soc_feature_ip_subnet_based_vlan)) {
            soc_mem_lock(unit, VLAN_SUBNETm);
            rv = _bcm_trx_vlan_ip_delete_all(unit);
            soc_mem_unlock(unit, VLAN_SUBNETm);

            return rv;
        } else {
            return BCM_E_UNAVAIL;  /* Latency bypass */
        }
    }
#endif /* BCM_TRX_SUPPORT */

    if ((rv = _bcm_vlan_ip6_delete_all(unit)) == BCM_E_UNAVAIL) {
        rv = BCM_E_NONE;
    }
    if (rv == BCM_E_NONE) {
        rv = bcm_esw_vlan_ip4_delete_all(unit);
    }

    return rv;
}

/*
 * Function:
 *      bcm_vlan_ip_action_add
 * Purpose:
 *      Add a subnet lookup to select vlan action for untagged packets
 * Parameters:
 *      unit    -   device number
 *      vlan_ip -   structure specifying IP address and other info
 *      action  -   structure VLAN tag actions
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_vlan_ip_action_add(int unit, bcm_vlan_ip_t *vlan_ip,
                           bcm_vlan_action_set_t *action)
{
#ifdef BCM_TRX_SUPPORT
    int rv;
        
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)
        && soc_feature(unit, soc_feature_ip_subnet_based_vlan)) {
        soc_mem_lock(unit, VLAN_SUBNETm);
        rv = _bcm_trx_vlan_ip_action_add(unit, vlan_ip, action);
        soc_mem_unlock(unit, VLAN_SUBNETm);

        return rv;
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_vlan_ip_action_get
 * Purpose:
 *      Get an action for subnet 
 * Parameters:
 *      unit    -   (IN) device number
 *      vlan_ip -   (IN) structure specifying IP address and other info
 *      action  -   (OUT) structure VLAN tag actions
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_vlan_ip_action_get(int unit, bcm_vlan_ip_t *vlan_ip,
                           bcm_vlan_action_set_t *action)
{
#ifdef BCM_TRX_SUPPORT
    int rv; 

    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)
        && soc_feature(unit, soc_feature_ip_subnet_based_vlan)) {

        bcm_vlan_action_set_t_init(action);
        soc_mem_lock(unit, VLAN_SUBNETm);
        rv = _bcm_trx_vlan_ip_action_get(unit, vlan_ip, action);
        soc_mem_unlock(unit, VLAN_SUBNETm);

        return rv;
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vlan_ip_action_delete
 * Purpose:
 *      Delete an action for subnet 
 * Parameters:
 *      unit    -   (IN) device number
 *      vlan_ip -   (IN) structure specifying IP address and other info
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_vlan_ip_action_delete(int unit, bcm_vlan_ip_t *vlan_ip)
{
#ifdef BCM_TRX_SUPPORT
    int rv; 

    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)
        && soc_feature(unit, soc_feature_ip_subnet_based_vlan)) {
        soc_mem_lock(unit, VLAN_SUBNETm);
        rv = _bcm_trx_vlan_ip_delete(unit, vlan_ip);
        soc_mem_unlock(unit, VLAN_SUBNETm);

        return rv;
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vlan_ip_action_delete_all
 * Purpose:
 *      Delete all actions for all subnets
 * Parameters:
 *      unit    -   (IN) device number
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_vlan_ip_action_delete_all(int unit)
{
#ifdef BCM_TRX_SUPPORT
    int rv; 

    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)
        && soc_feature(unit, soc_feature_ip_subnet_based_vlan)) {
        soc_mem_lock(unit, VLAN_SUBNETm);
        rv = _bcm_trx_vlan_ip_delete_all(unit);
        soc_mem_unlock(unit, VLAN_SUBNETm);

        return rv;
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}
/*
 * Function:
 *      bcm_esw_vlan_ip_action_traverse
 * Purpose:
 *      Delete all actions for all subnets
 * Parameters:
 *      unit    -   (IN) device number
 *      cb      -   (IN) call back function 
 *      user_data - (IN) a pointer to user data
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_ip_action_traverse(int unit, bcm_vlan_ip_action_traverse_cb cb, 
                                void *user_data)
{
#ifdef BCM_TRX_SUPPORT
    int rv;

    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)
        && soc_feature(unit, soc_feature_ip_subnet_based_vlan)) {
        soc_mem_lock(unit, VLAN_SUBNETm);
        rv = _bcm_trx_vlan_ip_action_traverse(unit, cb, user_data);
        soc_mem_unlock(unit, VLAN_SUBNETm);

        return rv;
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * IMPLEMENTATION NOTES
 *
 * bcm_vlan_ip4_* should be implemented in FB/ER as adding
 * entries to the VLAN_SUBNET tables with:
 *      VLAN_SUBNET.IP_ADDR = 0xFFFF0000 <ip4_addr>
 *      VLAN_SUBNET.MASK = 0xFFFFFFFF <ip4_netmask>
 *      VLAN_SUBNET.VALID = 1
 * The VLAN_SUBNET entries must be sorted in order from
 * longest prefix match to shortest prefix match.
 */

/*
 * Function:
 *     bcm_vlan_control_set
 *
 * Purpose:
 *     Set miscellaneous VLAN-specific chip options
 *
 * Parameters:
 *     unit - StrataSwitch PCI device unit number (driver internal).
 *     type - A value from bcm_vlan_control_t enumeration list
 *     arg  - state whose meaning is dependent on 'type'
 *
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_INTERNAL - Chip access failure.
 *     BCM_E_UNAVAIL  - type not supported on unit
 */

int
bcm_esw_vlan_control_set(int unit, bcm_vlan_control_t type, int arg)
{
    bcm_pbmp_t e_pbmp;

#if defined(BCM_HGPROXY_COE_SUPPORT)
    int i;
    bcm_gport_t gport;
#endif /* BCM_HGPROXY_COE_SUPPORT */

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_vlan_control_set(unit, type, arg);
    }
#endif

    LOG_VERBOSE(BSL_LS_BCM_VLAN,
                (BSL_META_U(unit,
                            "VLAN %d: control set: type %d, arg %d\n"),
                 unit, type, arg));

    BCM_PBMP_CLEAR(e_pbmp);
    BCM_PBMP_ASSIGN(e_pbmp, PBMP_E_ALL(unit));
#ifdef BCM_KATANA2_SUPPORT
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        _bcm_kt2_subport_pbmp_update (unit, &e_pbmp);
    }
    if (SOC_IS_KATANA2(unit) && soc_feature(unit, soc_feature_flex_port)) {
        BCM_IF_ERROR_RETURN(_bcm_kt2_flexio_pbmp_update(unit, &e_pbmp));
    }
#endif

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit) && (!SOC_IS_XGS3_FABRIC(unit) ||
        SOC_IS_TOMAHAWK2(unit))) {
        bcm_port_t  port;
        uint32      vlan_ctrl;
        uint32      data;
        int         oval;

        PBMP_E_ITER(unit, port) {
            break;
        }

        switch (type) {
        case bcmVlanDropUnknown:
            return bcm_esw_switch_control_set(unit,
                                              bcmSwitchUnknownVlanToCpu,
                                              arg ? 0 : 1);

        case bcmVlanShared:
            if (!SOC_REG_FIELD_VALID(unit, EGR_CONFIGr, USE_LEARN_VIDf) ||
                !SOC_REG_FIELD_VALID(unit, VLAN_CTRLr, USE_LEARN_VIDf)) {
                return BCM_E_UNAVAIL;
            }
            arg = (arg ? 1 : 0);
            SOC_IF_ERROR_RETURN(READ_VLAN_CTRLr(unit, &vlan_ctrl));
            data = vlan_ctrl;
            soc_reg_field_set(unit, VLAN_CTRLr, &vlan_ctrl,
                              USE_LEARN_VIDf, arg);
            if (data != vlan_ctrl) {
                SOC_IF_ERROR_RETURN(WRITE_VLAN_CTRLr(unit, vlan_ctrl));
                if (SOC_IS_FBX(unit)) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_field32_modify(unit, EGR_CONFIGr, port,
                                                USE_LEARN_VIDf, arg));
                }
            }
            return BCM_E_NONE;
        case bcmVlanSharedID:
            if (!SOC_REG_FIELD_VALID(unit, EGR_CONFIGr, LEARN_VIDf) ||
                !SOC_REG_FIELD_VALID(unit, VLAN_CTRLr, LEARN_VIDf)) {
                return BCM_E_UNAVAIL;
            }
            SOC_IF_ERROR_RETURN(READ_VLAN_CTRLr(unit, &vlan_ctrl));
            data = vlan_ctrl;
            soc_reg_field_set(unit, VLAN_CTRLr, &vlan_ctrl,
                              LEARN_VIDf, arg);
            if (data != vlan_ctrl) {
                SOC_IF_ERROR_RETURN(WRITE_VLAN_CTRLr(unit, vlan_ctrl));
                if (SOC_IS_FBX(unit)) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_field32_modify(unit, EGR_CONFIGr, port,
                                                LEARN_VIDf, arg));
                }
            }
            return BCM_E_NONE;
        case bcmVlanPreferIP4:
            arg = (arg ? 1 : 0);
            SOC_IF_ERROR_RETURN
                (_bcm_esw_port_config_get(unit, port,
                                          _bcmPortVlanPrecedence, &oval));
            if (arg != oval) {
                PBMP_ITER(e_pbmp, port) {
                    SOC_IF_ERROR_RETURN
                        (_bcm_esw_port_config_set(unit, port,
                                              _bcmPortVlanPrecedence, arg));
                }
#if defined(BCM_HGPROXY_COE_SUPPORT)
                BCM_HGPROXY_SUBTAG_COE_PORT_ITER(i, gport) {
                    SOC_IF_ERROR_RETURN
                        (_bcm_esw_port_config_set(unit, gport,
                                                  _bcmPortVlanPrecedence, arg));
                }
#endif /* BCM_HGPROXY_COE_SUPPORT */
            }
            return BCM_E_NONE;

        case bcmVlanPreferMAC:
            if (soc_feature(unit, soc_feature_mac_based_vlan)) {
                arg = (arg ? 0 : 1);
                SOC_IF_ERROR_RETURN
                    (_bcm_esw_port_config_get(unit, port,
                                              _bcmPortVlanPrecedence, &oval));
                if (arg != oval) {
                    PBMP_ITER(e_pbmp, port) {
                        SOC_IF_ERROR_RETURN
                            (_bcm_esw_port_config_set(unit, port,
                                                  _bcmPortVlanPrecedence, arg));
                    }
#if defined(BCM_HGPROXY_COE_SUPPORT)
                    BCM_HGPROXY_SUBTAG_COE_PORT_ITER(i, gport) {
                        SOC_IF_ERROR_RETURN
                            (_bcm_esw_port_config_set(unit, gport,
                                                      _bcmPortVlanPrecedence,
                                                      arg));
                    }
#endif /* BCM_HGPROXY_COE_SUPPORT */

                }
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;

        case bcmVlanTranslate:
            if (soc_feature(unit, soc_feature_vlan_translation)) {
                arg = (arg ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (_bcm_esw_port_config_get(unit, port,
                                              _bcmPortVlanTranslate, &oval));
                if (arg != oval) {
                    PBMP_ITER(e_pbmp, port) {
#ifdef BCM_TRIDENT2PLUS_SUPPORT
                        if (soc_feature(unit, soc_feature_egr_vlan_control_is_memory)) {
                            if (soc_feature(unit, soc_feature_egr_all_profile)) {
                                BCM_IF_ERROR_RETURN
                                    (bcm_esw_port_egr_lport_field_set(unit, port,
                                    EGR_VLAN_CONTROL_1m, VT_ENABLEf, arg));
                            } else {
                            egr_vlan_control_1_entry_t entry;
                            SOC_IF_ERROR_RETURN(
                                        READ_EGR_VLAN_CONTROL_1m(unit,
                                        MEM_BLOCK_ANY, port, &entry));
                                soc_EGR_VLAN_CONTROL_1m_field32_set(unit,
                                        &entry, VT_ENABLEf, arg);
                            SOC_IF_ERROR_RETURN(
                                        WRITE_EGR_VLAN_CONTROL_1m(unit,
                                        MEM_BLOCK_ANY, port, &entry));
                            }
                        } else
#endif
                        {
                            SOC_IF_ERROR_RETURN
                                (soc_reg_field32_modify(unit, EGR_VLAN_CONTROL_1r,
                                                        port, VT_ENABLEf, arg));
                        }
                        SOC_IF_ERROR_RETURN
                            (_bcm_esw_port_config_set(unit, port,
                                                      _bcmPortVlanTranslate, arg));
                    }
#if defined(BCM_HGPROXY_COE_SUPPORT)
                    BCM_HGPROXY_SUBTAG_COE_PORT_ITER(i, gport) {
                        SOC_IF_ERROR_RETURN
                            (_bcm_esw_port_config_set(unit, gport,
                                                      _bcmPortVlanTranslate,
                                                      arg));
                    }
#endif /* BCM_HGPROXY_COE_SUPPORT */
                }
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;

        case bcmVlanPreferEgressTranslate:
            arg = (arg ? 1 : 0);
#ifdef BCM_TRX_SUPPORT
            if (SOC_IS_TRX(unit)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit,
                    SOC_IS_TRIDENT3X(unit) ? EGR_CONFIG_1_64r : EGR_CONFIG_1r,
                    port, DISABLE_VT_IF_IFP_CHANGE_VLANf, arg));
                return BCM_E_NONE;
            }
#endif
            return BCM_E_UNAVAIL;
        default:
            return BCM_E_UNAVAIL;
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#ifdef BCM_XGS12_SWITCH_SUPPORT
    if (SOC_IS_XGS12_SWITCH(unit)) {
        bcm_port_t          port; 
        uint64              r64, o64;
        switch (type) {
        case bcmVlanDropUnknown:
            PBMP_E_ITER(unit, port) {
                SOC_IF_ERROR_RETURN(READ_CPU_CONTROLr(unit, port, &r64));
                o64 = r64;
                soc_reg64_field32_set(unit, CPU_CONTROLr, &r64,
                                      UVLAN_TOCPUf, arg ? 0 : 1);
                if (COMPILER_64_NE(r64, o64)) {
                    SOC_IF_ERROR_RETURN(WRITE_CPU_CONTROLr(unit, port, r64));
                }
            }
            return BCM_E_NONE;
        case bcmVlanShared:
            PBMP_E_ITER(unit, port) {
                SOC_IF_ERROR_RETURN(READ_VLAN_CONTROLr(unit, port, &r64));
                o64 = r64;
                soc_reg64_field32_set(unit, VLAN_CONTROLr, &r64,
                                      USE_LEARN_VIDf, arg ? 1 : 0);
                if (COMPILER_64_NE(r64, o64)) {
                    SOC_IF_ERROR_RETURN(WRITE_VLAN_CONTROLr(unit, port, r64));
                }
            }
            return BCM_E_NONE;
        case bcmVlanSharedID:
            PBMP_E_ITER(unit, port) {
                SOC_IF_ERROR_RETURN(READ_VLAN_CONTROLr(unit, port, &r64));
                o64 = r64;
                soc_reg64_field32_set(unit, VLAN_CONTROLr, &r64,
                                      LEARN_VIDf, arg & 0xfff);
                if (COMPILER_64_NE(r64, o64)) {
                    SOC_IF_ERROR_RETURN(WRITE_VLAN_CONTROLr(unit, port, r64));
                }
            }
            return BCM_E_NONE;
        default:
            break;
        }
    }
#endif /* BCM_XGS12_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_vlan_control_get
 *
 * Purpose:
 *     Get miscellaneous VLAN-specific chip options
 *
 * Parameters:
 *     unit - StrataSwitch PCI device unit number (driver internal).
 *     type - A value from bcm_vlan_control_t enumeration list
 *     arg  - (OUT) state whose meaning is dependent on 'type'
 *
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_PARAM    - arg points to NULL
 *     BCM_E_INTERNAL - Chip access failure.
 *     BCM_E_UNAVAIL  - type not supported on unit
 */

int
bcm_esw_vlan_control_get(int unit, bcm_vlan_control_t type, int *arg)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_vlan_control_get(unit, type, arg);
    }
#endif

    if (arg == 0) {
        return BCM_E_PARAM;
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit) && (!SOC_IS_XGS3_FABRIC(unit) ||
        SOC_IS_TOMAHAWK2(unit))) {
        int                 port;
        uint32              vlan_ctrl;

        PBMP_E_ITER(unit, port) {
            break;
        }
        switch (type) {

        case bcmVlanDropUnknown:
            {
                int ret_val;
                ret_val = bcm_esw_switch_control_get(unit,
                                                  bcmSwitchUnknownVlanToCpu,
                                                  arg);
                if (ret_val == BCM_E_NONE) {
                    *arg = (*arg) ? 0 : 1;
                }
                return ret_val;
            }
        case bcmVlanShared:
            if (!SOC_REG_FIELD_VALID(unit, EGR_CONFIGr, USE_LEARN_VIDf) ||
                !SOC_REG_FIELD_VALID(unit, VLAN_CTRLr, USE_LEARN_VIDf)) {
                return BCM_E_UNAVAIL;
            }
            SOC_IF_ERROR_RETURN(READ_VLAN_CTRLr(unit, &vlan_ctrl));
            *arg = soc_reg_field_get(unit, VLAN_CTRLr,
                                     vlan_ctrl, USE_LEARN_VIDf);
            return BCM_E_NONE;

        case bcmVlanSharedID:
            if (!SOC_REG_FIELD_VALID(unit, EGR_CONFIGr, LEARN_VIDf) ||
                !SOC_REG_FIELD_VALID(unit, VLAN_CTRLr, LEARN_VIDf)) {
                return BCM_E_UNAVAIL;
            }
            SOC_IF_ERROR_RETURN(READ_VLAN_CTRLr(unit, &vlan_ctrl));
            *arg = soc_reg_field_get(unit, VLAN_CTRLr,
                                     vlan_ctrl, LEARN_VIDf);
            return BCM_E_NONE;

        case bcmVlanPreferIP4:
           return (_bcm_esw_port_config_get(unit, port,
                                            _bcmPortVlanPrecedence, arg));
        case bcmVlanPreferMAC:
            SOC_IF_ERROR_RETURN
                (_bcm_esw_port_config_get(unit, port,
                                          _bcmPortVlanPrecedence, arg));
            *arg = !*arg;
            return BCM_E_NONE;
        case bcmVlanTranslate:
            if (soc_feature(unit, soc_feature_vlan_translation)) {
                return (_bcm_esw_port_config_get(unit, port,
                                                 _bcmPortVlanTranslate, arg));
            } else {
                return BCM_E_UNAVAIL;
            }
        case bcmVlanPreferEgressTranslate:
#ifdef BCM_TRX_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                uint64  rval64;
                COMPILER_64_ZERO(rval64);
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, EGR_CONFIG_1_64r,
                                    REG_PORT_ANY, 0, &rval64));
                *arg = soc_reg64_field32_get(unit, EGR_CONFIG_1_64r, rval64,
                            DISABLE_VT_IF_IFP_CHANGE_VLANf);
                return BCM_E_NONE;
            } else if (SOC_IS_TRX(unit)) {
                uint32 egr_cfg;
                SOC_IF_ERROR_RETURN(READ_EGR_CONFIG_1r(unit, &egr_cfg));
                *arg = soc_reg_field_get(unit, EGR_CONFIG_1r, egr_cfg, 
                                         DISABLE_VT_IF_IFP_CHANGE_VLANf);
                return BCM_E_NONE;
            }
#endif
            return BCM_E_UNAVAIL;
        default:
            return BCM_E_UNAVAIL;
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#ifdef BCM_XGS12_SWITCH_SUPPORT
    if (SOC_IS_XGS12_SWITCH(unit)) {
        int                 port;
        uint64              r64;
        switch (type) {
        case bcmVlanDropUnknown:
            COMPILER_64_ZERO(r64);
            PBMP_E_ITER(unit, port) {
                SOC_IF_ERROR_RETURN(READ_CPU_CONTROLr(unit, port, &r64));
                break;
            }
            *arg = !soc_reg64_field32_get(unit, CPU_CONTROLr, r64,
                                          UVLAN_TOCPUf);
            return BCM_E_NONE;
        case bcmVlanShared:
            COMPILER_64_ZERO(r64);
            PBMP_E_ITER(unit, port) {
                SOC_IF_ERROR_RETURN(READ_VLAN_CONTROLr(unit, port, &r64));
                break;
            }
            *arg = soc_reg64_field32_get(unit, VLAN_CONTROLr, r64,
                                         USE_LEARN_VIDf);
            return BCM_E_NONE;
        case bcmVlanSharedID:
            COMPILER_64_ZERO(r64);
            PBMP_E_ITER(unit, port) {
                SOC_IF_ERROR_RETURN(READ_VLAN_CONTROLr(unit, port, &r64));
                break;
            }
            *arg = soc_reg64_field32_get(unit, VLAN_CONTROLr, r64,
                                         LEARN_VIDf);
            return BCM_E_NONE;
        default:
            return BCM_E_UNAVAIL;
        }
    }
#endif /* BCM_XGS12_SWITCH_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vlan_control_port_proxy_validate
 * Description:
 *      Checks that vlan port control is valid for a PROXY GPORT.
 *      Resolves port to expected port value for the given port control type.
 * Parameters:
 *      unit      - (IN) Device number
 *      port      - (IN) Port to resolve
 *      type      - (IN) Vlan control type
 *      port_out  - (OUT) Returns port (gport or BCM format) that
 *                  is expected for corresponding vlan control type.
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
bcm_esw_vlan_control_port_proxy_validate(int unit, bcm_port_t port,
                                         bcm_vlan_control_port_t type,
                                         bcm_port_t *port_out)
{
    int rv = BCM_E_PORT;
    int support = FALSE;

    if (!soc_feature(unit, soc_feature_proxy_port_property)) {
        return BCM_E_PORT;
    }

    switch(type) {
    case bcmVlanTranslateIngressEnable:
    case bcmVlanPortTranslateKeyFirst:
    case bcmVlanPortTranslateKeySecond: 
        support = TRUE;
        break;
    default:
        support = FALSE;
        break;
    }

    if (support) {
        *port_out = port;
        rv = BCM_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *      bcm_esw_vlan_control_port_niv_validate
 * Description:
 *      Checks that vlan port control is valid for a NIV GPORT.
 *      Resolves port to expected port value for the given port control type.
 * Parameters:
 *      unit      - (IN) Device number
 *      port      - (IN) Port to resolve
 *      type      - (IN) Vlan control type
 *      port_out  - (OUT) Returns port (gport or BCM format) that
 *                  is expected for corresponding vlan control type.
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
bcm_esw_vlan_control_port_niv_validate(int unit, bcm_port_t port,
                                       bcm_vlan_control_port_t type,
                                       bcm_port_t *port_out)
{
#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3)
    int rv = BCM_E_PORT;
    int support = FALSE;

    if (!soc_feature(unit, soc_feature_niv)) {
        return BCM_E_PORT;
    }

    switch(type) {
    case bcmVlanPortTranslateKeyFirst:
        support = TRUE;
        break;
    default:
        support = FALSE;
        break;
    }

    if (support) {
        *port_out = port;
        rv = BCM_E_NONE;
    }

    return rv;
#else
    return BCM_E_PORT;
#endif
}

/*
 * Function:
 *      bcm_esw_vlan_control_port_flow_validate
 * Description:
 *      Checks that vlan port control is valid for a FLOW GPORT.
 *      Resolves port to expected port value for the given port control type.
 * Parameters:
 *      unit      - (IN) Device number
 *      port      - (IN) Port to resolve
 *      type      - (IN) Vlan control type
 *      port_out  - (OUT) Returns port (gport or BCM format) that
 *                  is expected for corresponding vlan control type.
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
bcm_esw_vlan_control_port_flow_validate(int unit, bcm_port_t port,
                                       bcm_vlan_control_port_t type,
                                       bcm_port_t *port_out)
{
#if defined(BCM_TRIDENT3_SUPPORT) && defined(INCLUDE_L3)
    int rv = BCM_E_PORT;
    int support = FALSE;

    if (!soc_feature(unit, soc_feature_flex_flow)) {
        return BCM_E_PORT;
    }

    switch(type) {
    case bcmVlanPortTranslateEgressKey:
    case bcmVlanPortUseInnerPri:
        support = TRUE;
        break;
    default:
        support = FALSE;
        break;
    }

    if (support) {
        *port_out = port;
        rv = BCM_E_NONE;
    }

    return rv;
#else
    return BCM_E_PORT;
#endif
}

#if defined(BCM_TRX_SUPPORT) 
/*
 * Function:
 *      bcm_esw_vlan_control_port_niv_resolve
 * Description:
 *      Resolve NIV GPORT to a BCM local physical port.
 * Parameters:
 *      unit      - (IN) Device number
 *      port      - (IN) Port to resolve
 *      port_out  - (OUT) Returns BCM local physical port
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
bcm_esw_vlan_control_port_niv_resolve(int unit, bcm_port_t port,
                                      bcm_port_t *port_out)
{
#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3)
    bcm_niv_port_t niv_port;
    bcm_module_t modid;
    bcm_port_t local_port;
    bcm_trunk_t trunk_id;
    int gport_id;
    int is_local;
    bcm_niv_egress_t niv_egress;
    int              rv = BCM_E_NONE;
    bcm_gport_t      gport;
    int              count;

    bcm_niv_port_t_init(&niv_port);
    niv_port.niv_port_id = port;

    BCM_IF_ERROR_RETURN(bcm_esw_niv_port_get(unit, &niv_port));
    if (niv_port.flags & BCM_NIV_PORT_MATCH_NONE) {
        bcm_niv_egress_t_init(&niv_egress);
        rv = bcm_trident_niv_egress_get(unit, niv_port.niv_port_id,
                                        1, &niv_egress, &count);
        if (BCM_FAILURE(rv)) {
            return BCM_E_PARAM;
        }
        
        if (niv_egress.flags & BCM_NIV_EGRESS_MULTICAST) {
            return BCM_E_PARAM;
        } else {
            gport = niv_egress.port;
        }
    } else {
        gport = niv_port.port;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, gport, 
                                &modid, &local_port,
                                &trunk_id, &gport_id));

    BCM_IF_ERROR_RETURN(_bcm_esw_modid_is_local(unit, modid, &is_local));
    if (!is_local) {
        return BCM_E_PORT;

    }

    *port_out = local_port;

    return BCM_E_NONE;
#else
    return BCM_E_PORT;
#endif
}
#endif /*BCM_TRX_SUPPORT*/
/*
 * Function:
 *      bcm_esw_vlan_control_port_validate
 * Description:
 *      Checks that port is valid and that control is supported
 *      for the given port type.  Resolves port to expected
 *      port value for the given vlan control type.
 * Parameters:
 *      unit      - (IN) Device number
 *      port      - (IN) Port to resolve
 *      type      - (IN) Vlan control type
 *      port_out  - (OUT) Returns port (gport or BCM format) that
 *                  is expected for corresponding vlan control type.
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
bcm_esw_vlan_control_port_validate(int unit, bcm_port_t port,
                                   bcm_vlan_control_port_t type,
                                   bcm_port_t *port_out)
{
    bcm_module_t  modid;
    bcm_port_t  port_id;
    bcm_trunk_t  tgid;
    int  id;
#if defined(BCM_KATANA2_SUPPORT)
    int my_modid = 0, pp_port;
    int min_subport = SOC_INFO(unit).pp_port_index_min;
    int max_subport = SOC_INFO(unit).pp_port_index_max;
#endif
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
    int is_local_subport = FALSE;
#endif

    if (BCM_GPORT_IS_SET(port)) {
        if (BCM_GPORT_IS_PROXY(port)) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_vlan_control_port_proxy_validate(unit, port,
                                                          type, port_out));
        } else if (BCM_GPORT_IS_NIV_PORT(port)) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_vlan_control_port_niv_validate(unit, port,
                                                        type, port_out));
        } else if (BCM_GPORT_IS_FLOW_PORT(port)) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_vlan_control_port_flow_validate(unit, port,
                                                          type, port_out));
        } else if (BCM_GPORT_IS_SUBPORT_PORT(port)) {

#if defined(BCM_HGPROXY_COE_SUPPORT)
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            (soc_feature(unit, soc_feature_channelized_switching))) &&
            BCM_GPORT_IS_SET(port) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {

            if(_bcm_xgs5_subport_coe_gport_local(unit, port)) {
                is_local_subport = TRUE;
            }

            if (is_local_subport == FALSE) {
                return BCM_E_PORT;
            }

            /*For HGPROXY_COE, subport is needed for configuring
             *port and lport tables.
             */
            *port_out = port;

        } else 
#endif
#if defined(BCM_KATANA2_SUPPORT)
            if (_BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit, port)) {
                BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, port,
                    &modid, &port_id, &tgid, &id));
                    pp_port = BCM_GPORT_SUBPORT_PORT_GET(port);
                BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
                if ((_bcm_kt2_mod_is_coe_mod_check(unit, modid) == BCM_E_NONE) ||
                    (soc_feature(unit, soc_feature_general_cascade) &&
                     SOC_PBMP_MEMBER(SOC_INFO(unit).general_pp_port_pbm, pp_port))) {
#if defined BCM_METROLITE_SUPPORT
                    if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                        if (_SOC_IS_PP_PORT_LINKPHY_SUBTAG(unit, pp_port)) {
                            is_local_subport = 1;
                            *port_out = pp_port;
                        }
                    } else
#endif
                    {
                        if ((pp_port >= min_subport) &&
                            (pp_port <= max_subport)) {
                            is_local_subport = 1;
                            *port_out = pp_port;
                        }
                    }
                }
                if (is_local_subport == 0) {
                    return BCM_E_PORT;
                }
            } else
#endif
            {
                return BCM_E_PORT;
            }
        } else {
            /* Other GPORT types */
            if (BCM_SUCCESS(_bcm_vlan_control_trust_gport(type))) {
                *port_out = port;
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_gport_resolve(unit, port, &modid, &port_id,
                                            &tgid, &id));
                if ((id != -1) || (tgid != -1)) {
                    return BCM_E_PORT;
                }
                *port_out = port_id;
            }
        }

    } else {

#ifdef BCM_KATANA2_SUPPORT
        if ((soc_feature(unit, soc_feature_linkphy_coe) &&
             BCM_PBMP_MEMBER(SOC_INFO(unit).linkphy_pp_port_pbm, port)) ||
            (soc_feature(unit, soc_feature_subtag_coe) &&
             BCM_PBMP_MEMBER(SOC_INFO(unit).subtag_pp_port_pbm, port))) {
        } else
#endif
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        *port_out = port;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_vlan_control_port_set(int unit, int port,
                           bcm_vlan_control_port_t type, int arg)
{
#if defined(BCM_FIREBOLT_SUPPORT)
    if (SOC_IS_FBX(unit)) {
        uint32 data;
        uint32 evc;
        soc_reg_t egr_register;
        int key_type_value;
#ifdef BCM_TRIDENT2PLUS_SUPPORT
        soc_field_t field = VT_ENABLEf;
        soc_mem_t egr_mem;
        egr_mem = EGR_VLAN_CONTROL_1m;
#endif
        egr_register = EGR_VLAN_CONTROL_1r;

        switch (type) {
        case bcmVlanPortPreferIP4:
            arg = (arg ? 1 : 0);
            return (_bcm_esw_port_config_set(unit, port,
                                             _bcmPortVlanPrecedence, arg));

        case bcmVlanPortPreferMAC:
            arg = (arg ? 0 : 1);
            return (_bcm_esw_port_config_set(unit, port,
                                             _bcmPortVlanPrecedence, arg));

        case bcmVlanTranslateIngressEnable:
            if (soc_feature(unit, soc_feature_vlan_translation)) {
                arg = (arg ? 1 : 0);
                return (_bcm_esw_port_config_set(unit, port,
                                                 _bcmPortVlanTranslate, arg));
            }
            return BCM_E_UNAVAIL;

        case bcmVlanTranslateIngressMissDrop:
            if (soc_feature(unit, soc_feature_vlan_translation)) {
                arg = (arg ? 1 : 0);
                return (_bcm_esw_port_config_set(unit, port,
                                                _bcmPortVTMissDrop, arg));
            }
            return BCM_E_UNAVAIL;

        case bcmVlanTranslateEgressEnable:
            if (soc_feature(unit, soc_feature_vlan_translation)) {
                arg = (arg ? 1 : 0);
#ifdef BCM_TRIDENT2PLUS_SUPPORT
                if (SOC_MEM_IS_VALID(unit, egr_mem)) {
                    egr_vlan_control_1_entry_t entry;
                    if (soc_feature(unit, soc_feature_egr_all_profile)
#if defined(BCM_HGPROXY_COE_SUPPORT)
                        ||((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
                        (soc_feature(unit, soc_feature_channelized_switching))) &&
                        BCM_GPORT_IS_SET(port) &&
                            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port))
#endif
                        ) {
                        SOC_IF_ERROR_RETURN(
                            bcm_esw_port_egr_lport_field_set(unit, port,
                            EGR_VLAN_CONTROL_1m, VT_ENABLEf, arg));
                        return BCM_E_NONE;
                    }
                    SOC_IF_ERROR_RETURN
                            (soc_mem_read(unit, egr_mem, MEM_BLOCK_ANY, port, &entry));
                    data = soc_mem_field32_get(unit, egr_mem, &entry, VT_ENABLEf);
                    if (data != arg) {
                        soc_mem_field32_set(unit, egr_mem, &entry, VT_ENABLEf, arg);
                        return (soc_mem_write(unit, egr_mem, MEM_BLOCK_ANY, port, &entry));
                    }
                } else
#endif
                {
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, egr_register, port, 0, &evc));
                    data = evc;
                    soc_reg_field_set(unit, egr_register, &evc, VT_ENABLEf, arg);
                    if (evc != data) {
                        return (soc_reg32_set(unit, egr_register, port, 0, evc));
                    }
                }
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;

        case bcmVlanTranslateEgressMissDrop:
            if (soc_feature(unit, soc_feature_vlan_translation)) {
                arg = (arg ? 1 : 0);
#ifdef BCM_TRIDENT2PLUS_SUPPORT
                if (SOC_MEM_IS_VALID(unit, egr_mem)) {
                    egr_vlan_control_1_entry_t entry;
                    if (soc_feature(unit, soc_feature_egr_all_profile)
#if defined(BCM_HGPROXY_COE_SUPPORT)
                        || ((soc_feature(unit, soc_feature_hgproxy_subtag_coe)
                        || soc_feature(unit, soc_feature_channelized_switching))
                        && BCM_GPORT_IS_SET(port) &&
                            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port))
#endif
                        ) {
                        SOC_IF_ERROR_RETURN(
                            bcm_esw_port_egr_lport_field_set(unit, port,
                            EGR_VLAN_CONTROL_1m, VT_MISS_DROPf, arg));
                        return BCM_E_NONE;
                    }
                    SOC_IF_ERROR_RETURN
                            (soc_mem_read(unit, egr_mem, MEM_BLOCK_ANY, port, &entry));
                    data = soc_mem_field32_get(unit, egr_mem, &entry, VT_MISS_DROPf);
                    if (data != arg) {
                        soc_mem_field32_set(unit, egr_mem, &entry, VT_MISS_DROPf, arg);
                        return (soc_mem_write(unit, egr_mem, MEM_BLOCK_ANY, port, &entry));
                    }
                } else
#endif
                {
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, egr_register, port, 0, &evc));
                    data = evc;
                    soc_reg_field_set(unit, egr_register, &evc, VT_MISS_DROPf, arg);
                    if (evc != data) {
                        return (soc_reg32_set(unit, egr_register, port, 0, evc));
                    }
                }
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;

        case bcmVlanTranslateEgressMissUntaggedDrop:
            if ((soc_feature(unit, soc_feature_untagged_vt_miss)) &&
                (soc_feature(unit,soc_feature_vlan_translation))) {
                arg = (arg ? 1 : 0);
#ifdef BCM_TRIDENT2PLUS_SUPPORT
                if (SOC_MEM_IS_VALID(unit, egr_mem)) {
                    egr_vlan_control_1_entry_t entry;
                    if (soc_feature(unit, soc_feature_egr_all_profile)
#if defined(BCM_HGPROXY_COE_SUPPORT)
                        || ((soc_feature(unit, soc_feature_hgproxy_subtag_coe)
                        || soc_feature(unit, soc_feature_channelized_switching))
                        && BCM_GPORT_IS_SET(port) &&
                            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port))
#endif
                        ) {
                        SOC_IF_ERROR_RETURN(
                            bcm_esw_port_egr_lport_field_set(unit, port,
                            EGR_VLAN_CONTROL_1m, VT_MISS_UT_DROPf, arg));
                        return BCM_E_NONE;
                    }
                    SOC_IF_ERROR_RETURN
                            (soc_mem_read(unit, egr_mem, MEM_BLOCK_ANY, port, &entry));
                    data = soc_mem_field32_get(unit, egr_mem, &entry, VT_MISS_UT_DROPf);
                    if (data != arg) {
                        soc_mem_field32_set(unit, egr_mem, &entry, VT_MISS_UT_DROPf, arg);
                        SOC_IF_ERROR_RETURN
                            (soc_mem_write(unit, egr_mem, MEM_BLOCK_ANY, port, &entry));
                    }
                } else
#endif
                {
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, egr_register, port, 0, &evc));
                    data = evc;
                    soc_reg_field_set(unit, egr_register, &evc, VT_MISS_UT_DROPf,
                                      arg);
                    if (evc != data) {
                        SOC_IF_ERROR_RETURN
                            (soc_reg32_set(unit, egr_register, port, 0, evc));
                    }
                }
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;

        case bcmVlanTranslateEgressMissUntag:
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) || \
            defined(BCM_RAVEN_SUPPORT)
#ifdef BCM_TRIDENT2PLUS_SUPPORT
            if (soc_feature(unit, soc_feature_egr_vlan_control_is_memory)) {
                egr_vlan_control_1_entry_t entry;
                if (SOC_MEM_FIELD_VALID(unit, egr_mem, VT_MISS_UNTAGf)) {
                    arg = (arg ? 1 : 0);
                    if (soc_feature(unit, soc_feature_egr_all_profile)
#if defined(BCM_HGPROXY_COE_SUPPORT)
                        || ((soc_feature(unit, soc_feature_hgproxy_subtag_coe)
                        || soc_feature(unit, soc_feature_channelized_switching))
                        && BCM_GPORT_IS_SET(port) &&
                            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port))
#endif
                        ) {
                        SOC_IF_ERROR_RETURN(
                            bcm_esw_port_egr_lport_field_set(unit, port,
                            EGR_VLAN_CONTROL_1m, VT_MISS_UNTAGf, arg));
                        if (SOC_IS_TRIDENT3X(unit)) {
                            SOC_IF_ERROR_RETURN(_bcm_esw_egr_port_tab_set(unit,
                                     port,VXLT_CTRL_IDf, arg));
                        }
                        return BCM_E_NONE;
                    }
                    SOC_IF_ERROR_RETURN(soc_mem_read(unit, egr_mem,
                                    MEM_BLOCK_ANY, port, &entry));
                    soc_mem_field32_set(unit, egr_mem, &entry,
                                            VT_MISS_UNTAGf, arg);
                    return soc_mem_write(unit, EGR_VLAN_CONTROL_1m,
                                        MEM_BLOCK_ANY, port, &entry);
                }
            } else
#endif
            {
                if (SOC_REG_FIELD_VALID(unit, egr_register, VT_MISS_UNTAGf)) {
                    arg = (arg ? 1 : 0);
                    return soc_reg_field32_modify(unit, egr_register, port,
                                                  VT_MISS_UNTAGf, arg);
                }
            }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRIUMPH_SUPPORT || BCM_RAPTOR_SUPPORT */
            return BCM_E_UNAVAIL;

        case bcmVlanLookupMACEnable:
            arg = (arg ? 1 : 0);
            return (_bcm_esw_port_config_set(unit, port,
                                             _bcmPortLookupMACEnable, arg));

        case bcmVlanLookupIPEnable:
            arg = (arg ? 1 : 0);
            return (_bcm_esw_port_config_set(unit, port,
                                             _bcmPortLookupIPEnable, arg));

        case bcmVlanPortUseInnerPri:
            arg = (arg ? 1 : 0);
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
            if (BCM_GPORT_IS_MIM_PORT(port) || BCM_GPORT_IS_MPLS_PORT(port) ||
                BCM_GPORT_IS_VXLAN_PORT(port)) {
                return (_bcm_tr2_svp_field_set(unit, port, 
                                               USE_INNER_PRIf, arg));
            }
#endif
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
 || defined(BCM_RAVEN_SUPPORT)
            if (SOC_IS_FIREBOLT2(unit) || SOC_IS_TRX(unit) ||
                SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
                if (BCM_GPORT_IS_SET(port)) {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_local_get(unit, port, &port));
                }
                return (_bcm_esw_port_config_set(unit, port,
                                                 _bcmPortUseInnerPri, arg));
            }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */
            return BCM_E_UNAVAIL;

        case bcmVlanPortUseOuterPri:
            arg = (arg ? 1 : 0);
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
            if (BCM_GPORT_IS_MPLS_PORT(port) || BCM_GPORT_IS_MIM_PORT(port)) {
                int rv = BCM_E_NONE;
                rv = _bcm_tr2_svp_field_set(unit, port,
                                            TRUST_OUTER_DOT1Pf, arg);
                if (rv == BCM_E_UNAVAIL) {
                    return BCM_E_PORT;
                } else {
                    return rv;
                }
            }
#endif
#if defined(BCM_TRX_SUPPORT)
            if (SOC_IS_TRX(unit)) {
                if (BCM_GPORT_IS_SET(port)) {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_local_get(unit, port, &port));
                }
                return (_bcm_esw_port_config_set(unit, port,
                                                 _bcmPortUseOuterPri, arg));
            }
#endif /* BCM_TRX_SUPPORT */
            return BCM_E_UNAVAIL;

        case bcmVlanPortVerifyOuterTpid:
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAPTOR_SUPPORT) \
 || defined(BCM_TRX_SUPPORT)
            if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit) ||
                SOC_IS_TRX(unit) || SOC_IS_HAWKEYE(unit)) {
                arg = (arg ? 1 : 0);
                return (_bcm_esw_port_config_set(unit, port,
                                                 _bcmPortVerifyOuterTpid, arg));
            }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAPTOR_SUPPORT || BCM_TRX_SUPPORT */
            return BCM_E_UNAVAIL;

        case bcmVlanPortOuterTpidSelect:
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) \
 || defined(BCM_TRX_SUPPORT)
            if ((SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit) || 
                SOC_IS_TRX(unit) || SOC_IS_HAWKEYE(unit)) &&
                !SOC_IS_SHADOW(unit)) {
                if (arg != BCM_PORT_OUTER_TPID &&
                    arg != BCM_VLAN_OUTER_TPID) {
                    return BCM_E_PARAM;
                }
#ifdef BCM_TRIDENT2PLUS_SUPPORT
                if (soc_feature(unit, soc_feature_egr_vlan_control_is_memory)) {
                    egr_vlan_control_1_entry_t entry;
                    soc_field_t tpid_sel_f = OUTER_TPID_VALIDf;
                    if (SOC_MEM_FIELD_VALID(unit, EGR_VLAN_CONTROL_1m,
                                            OUTER_TPID_SELf)) {
                        tpid_sel_f = OUTER_TPID_SELf;
                    }
                    if (soc_feature(unit, soc_feature_egr_all_profile)
#if defined(BCM_HGPROXY_COE_SUPPORT)
                        || ((soc_feature(unit, soc_feature_hgproxy_subtag_coe)
                        || soc_feature(unit, soc_feature_channelized_switching))
                        && BCM_GPORT_IS_SET(port) &&
                            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port))
#endif
                        ) {
                        uint32 value = arg;
                        if (tpid_sel_f == OUTER_TPID_VALIDf) {
                            value = (arg==BCM_PORT_OUTER_TPID) ? 1 : 0;
                        }
                        SOC_IF_ERROR_RETURN(
                            bcm_esw_port_egr_lport_field_set(unit, port,
                            EGR_VLAN_CONTROL_1m, tpid_sel_f, value));
                        return BCM_E_NONE;
                    }
                    SOC_IF_ERROR_RETURN(
                            READ_EGR_VLAN_CONTROL_1m(unit, MEM_BLOCK_ANY, port, 
                            &entry));
                    data = soc_EGR_VLAN_CONTROL_1m_field32_get(unit, &entry,
                                                               tpid_sel_f);
                    if (arg != data) {
                        soc_EGR_VLAN_CONTROL_1m_field32_set(unit, &entry,
                                                            tpid_sel_f, arg);
                        SOC_IF_ERROR_RETURN(
                                WRITE_EGR_VLAN_CONTROL_1m(unit, MEM_BLOCK_ANY, 
                                port, &entry));
                    }
                } else
#endif
                {
                    SOC_IF_ERROR_RETURN
                        (READ_EGR_VLAN_CONTROL_1r(unit, port, &evc));
                    data = evc;
                    soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &evc,
                                          OUTER_TPID_SELf, arg);
                    if (evc != data) {
                        SOC_IF_ERROR_RETURN
                            (WRITE_EGR_VLAN_CONTROL_1r(unit, port, evc));
                    }
                }
                return BCM_E_NONE;
            }

#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */
            return BCM_E_UNAVAIL;

        case bcmVlanPortTranslateKeyFirst:
        case bcmVlanPortTranslateKeySecond:
#if defined(BCM_TRX_SUPPORT) 
            if (SOC_IS_TRX(unit)) {
                _bcm_port_config_t vt_key_type;
                _bcm_port_config_t vt_key_port;
                int rv, key_type = 0, use_port = 0;
                switch (arg) {
                    case bcmVlanTranslateKeyPortDouble:
                        use_port = 1;
                        /* fall through */
                    case bcmVlanTranslateKeyDouble:
                        key_type = VLXLT_HASH_KEY_TYPE_IVID_OVID;
                        break;
                    case bcmVlanTranslateKeyPortOuterTag:
                        use_port = 1;
                        /* fall through */
                    case bcmVlanTranslateKeyOuterTag:
                        key_type = VLXLT_HASH_KEY_TYPE_OTAG;
                        break;
                    case bcmVlanTranslateKeyPortInnerTag:
                        use_port = 1;
                        /* fall through */
                    case bcmVlanTranslateKeyInnerTag:
                        key_type = VLXLT_HASH_KEY_TYPE_ITAG;
                        break;
                    case bcmVlanTranslateKeyPortOuter:
                        use_port = 1;
                        /* fall through */
                    case bcmVlanTranslateKeyOuter:
                        key_type = VLXLT_HASH_KEY_TYPE_OVID;
                        break;
                    case bcmVlanTranslateKeyPortInner:
                        use_port = 1;
                        /* fall through */
                    case bcmVlanTranslateKeyInner:
                        key_type = VLXLT_HASH_KEY_TYPE_IVID;
                        break;
                    case bcmVlanTranslateKeyPortOuterPri:
                        use_port = 1;
                        /* fall through */
                    case bcmVlanTranslateKeyOuterPri:
                        key_type = VLXLT_HASH_KEY_TYPE_PRI_CFI;
                        break;
                    case bcmVlanTranslateKeyPortPonTunnel:
                        use_port = 1;
                        key_type = VLXLT_HASH_KEY_TYPE_LLVID;
                        break;
                    case bcmVlanTranslateKeyPortPonTunnelOuter:
                        use_port = 1;
                        key_type = VLXLT_HASH_KEY_TYPE_LLVID_OVID;
                        break;
                    case bcmVlanTranslateKeyPortPonTunnelInner:
                        key_type = VLXLT_HASK_KEY_TYPE_LLVID_IVID;
                        use_port = 1;
                        break;
                    case bcmVlanTranslateKeyPortCapwapPayloadDouble:
                        use_port = 1;
                        /* fall through */
                    case bcmVlanTranslateKeyCapwapPayloadDouble:
                        key_type = VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID_OVID;
                        break;
                    case bcmVlanTranslateKeyPortCapwapPayloadOuter:
                        use_port = 1;
                        /* fall through */
                    case bcmVlanTranslateKeyCapwapPayloadOuter:
                        key_type = VLXLT_HASH_KEY_TYPE_PAYLOAD_OVID;
                        break;
                    case bcmVlanTranslateKeyPortCapwapPayloadInner:
                        use_port = 1;
                        /* fall through */
                    case bcmVlanTranslateKeyCapwapPayloadInner:
                        key_type =VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID;
                        break;
                    case bcmVlanTranslateKeyPortCapwapPayloadOuterTag:
                        use_port = 1;
                        /* fall through */
                    case bcmVlanTranslateKeyCapwapPayloadOuterTag:
                        key_type = VLXLT_HASH_KEY_TYPE_PAYLOAD_OTAG;
                        break;
                    case bcmVlanTranslateKeyPortCapwapPayloadInnerTag:
                        use_port = 1;
                        /* fall through */
                    case bcmVlanTranslateKeyCapwapPayloadInnerTag:
                        key_type = VLXLT_HASH_KEY_TYPE_PAYLOAD_ITAG;
                        break;
                    case bcmVlanTranslateKeyPortVxlanVnid:
                        use_port = 1;
                        /* fall through */
                    case bcmVlanTranslateKeyVxlanVnid:
                        key_type = VLXLT_HASH_KEY_TYPE_VNID;
                        break;
#if defined(BCM_TRIDENT2_SUPPORT)
                    case bcmVlanTranslateKeyPortGroupDouble:
                        use_port = 3; /* port_group */
                        key_type = VLXLT_HASH_KEY_TYPE_IVID_OVID;
                        break;
                    case bcmVlanTranslateKeyPortGroupOuterTag:
                        use_port = 3; /* port_group */
                        key_type = VLXLT_HASH_KEY_TYPE_OTAG;
                        break;
                    case bcmVlanTranslateKeyPortGroupInnerTag:
                        use_port = 3; /* port_group */
                        key_type = VLXLT_HASH_KEY_TYPE_ITAG;
                        break;
                    case bcmVlanTranslateKeyPortGroupOuter:
                        use_port = 3; /* port_group */
                        key_type = VLXLT_HASH_KEY_TYPE_OVID;
                        break;
                    case bcmVlanTranslateKeyPortGroupInner:
                        use_port = 3; /* port_group */
                        key_type = VLXLT_HASH_KEY_TYPE_IVID;
                        break;
                    case bcmVlanTranslateKeyPortGroupOuterPri:
                        use_port = 3; /* port_group */
                        key_type = VLXLT_HASH_KEY_TYPE_PRI_CFI;
                        break;
#endif /* BCM_TRIDENT2_SUPPORT */
                    case bcmVlanTranslateKeyMacPort:
                        use_port = 1; /* Port */
                        key_type = VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT;
                        break;
                    case bcmVlanTranslateKeyMacVirtualPort:
                        use_port = 2; /* Virtual Port */
                        key_type = VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT;
                        break;
                    case bcmVlanTranslateKeyMacPortGroup:
                        use_port = 3; /* Port Group */
                        key_type = VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT;
                        break;
                    default:
                        return BCM_E_PARAM;
                }

                if (BCM_GPORT_IS_NIV_PORT(port)) {
                    switch (arg) {
                    case bcmVlanTranslateKeyPortOuterTag:
                        key_type = VLXLT_HASH_KEY_TYPE_VIF_OTAG;
                        break;
    
                    case bcmVlanTranslateKeyPortInnerTag:
                        key_type = VLXLT_HASH_KEY_TYPE_VIF_ITAG;
                        break;
    
                    case bcmVlanTranslateKeyPortOuter:
                        key_type = VLXLT_HASH_KEY_TYPE_VIF_VLAN;
                        break;
    
                    case bcmVlanTranslateKeyPortInner:
                        key_type = VLXLT_HASH_KEY_TYPE_VIF_CVLAN;
                        break;
    
                    default:
                        return BCM_E_PARAM;
                    }

                    BCM_IF_ERROR_RETURN
                        (bcm_esw_vlan_control_port_niv_resolve(unit,
                                                               port, &port));
                    use_port = 1;
                }

                if ((use_port == 3) &&
                    !(soc_feature(unit, soc_feature_port_group_for_ivxlt))) {
                    return BCM_E_UNAVAIL;
                }

                if (type == bcmVlanPortTranslateKeyFirst) {
                    vt_key_type = _bcmPortVTKeyTypeFirst;
                    vt_key_port = _bcmPortVTKeyPortFirst;
                } else {
                    vt_key_type = _bcmPortVTKeyTypeSecond;
                    vt_key_port = _bcmPortVTKeyPortSecond;
                }
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_pt_vtkey_type_value_get(unit,
                             key_type,&key_type_value));

                rv = _bcm_esw_port_config_set(unit, port,
                                              vt_key_type, 
                                              key_type_value);
                if (rv < 0) {
                    return rv;
                }
                return (_bcm_esw_port_config_set(unit, port,
                                                 vt_key_port, 
                                                 use_port));
            } 
#endif /* BCM_TRX_SUPPORT */
            return BCM_E_UNAVAIL;

#if defined(BCM_KATANA2_SUPPORT)
        case bcmVlanPortOamUseXlatedInnerVlan:
            if (SOC_IS_KATANA2(unit) && (soc_feature(unit, soc_feature_oam))) {
                arg = (arg ? 1 : 0);
                return (_bcm_esw_port_config_set(unit, port,
                                          _bcmPortOamUseXlatedInnerVlan, arg));
            } 
            return BCM_E_UNAVAIL;
            break;
#endif /* BCM_KATANA2_SUPPORT */

        case bcmVlanPortTranslateEgressKey:
        case bcmVlanPortTranslateEgressKeyFirst:
        case bcmVlanPortTranslateEgressKeySecond:
            if ((arg < bcmVlanTranslateEgressKeyInvalid) || 
                    (arg >= bcmVlanTranslateEgressKeyCount)) {
                return BCM_E_PARAM;
            }

#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
            if (bcmVlanPortTranslateEgressKeySecond == type) {
                if (soc_feature(unit, soc_feature_egr_vlan_xlate_second_lookup)) {
                    field = VT2_ENABLEf;
                } else {
                    return BCM_E_UNAVAIL;
                }
            }

            if (soc_feature(unit, soc_feature_egr_vlan_xlate_key_on_dvp)) {
                if (BCM_GPORT_IS_VXLAN_PORT(port) || BCM_GPORT_IS_MIM_PORT(port)
                     || BCM_GPORT_IS_MPLS_PORT(port) || BCM_GPORT_IS_L2GRE_PORT(port)) {
                    if (bcmVlanPortTranslateEgressKeySecond == type) {
                        /* Second EVxlate lookup is not supported with VPs */
                        return BCM_E_UNAVAIL;
                    }

                    BCM_IF_ERROR_RETURN
                        (_bcm_esw_dvp_vtkey_type_value_get(unit, arg,
                                                           &key_type_value));
                    return (bcm_td2p_dvp_vlan_xlate_key_set(unit, port,
                                                                  key_type_value));
                } else if (BCM_GPORT_IS_FLOW_PORT(port)) {
                    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIDENT3_SUPPORT
                    if (soc_feature(unit,soc_feature_flex_flow)) {
                        rv = bcmi_esw_flow_dvp_vlan_xlate_key_set(unit, 
                                     port, arg);
                    }
#endif
                    return rv;
                }
            }
#endif /* BCM_TRIDENT2PLUS_SUPPORT && INCLUDE_L3 */  

            BCM_IF_ERROR_RETURN
                (_bcm_esw_egr_vtkey_type_value_get(unit, arg,
                                                   &key_type_value));

#ifdef BCM_TRIDENT2PLUS_SUPPORT
            if (soc_feature(unit, soc_feature_egr_vlan_control_is_memory)) {
                egr_vlan_control_1_entry_t entry;
                    if (soc_feature(unit, soc_feature_egr_all_profile)
#if defined(BCM_HGPROXY_COE_SUPPORT)
                        || ((soc_feature(unit, soc_feature_hgproxy_subtag_coe)
                        || soc_feature(unit, soc_feature_channelized_switching))
                        && BCM_GPORT_IS_SET(port) &&
                            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port))
#endif
                        ) {
                    SOC_IF_ERROR_RETURN(
                            bcm_esw_port_egr_lport_field_set(unit, port,
                            EGR_VLAN_CONTROL_1m, field, key_type_value));
                    return BCM_E_NONE;
                }
                if (BCM_GPORT_IS_LOCAL(port)) {
                    port = BCM_GPORT_LOCAL_GET(port);
                }
                if (!SOC_PORT_VALID(unit, port)) {
                    return BCM_E_PORT;
                }

                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, egr_mem, MEM_BLOCK_ANY, port, &entry));
                data = soc_mem_field32_get(unit, egr_mem, &entry, field);

                if (data != key_type_value) {
                    soc_mem_field32_set(unit, egr_mem, &entry, field, key_type_value);
                    return (soc_mem_write(unit, egr_mem, MEM_BLOCK_ANY, port, &entry));
                }
                return BCM_E_NONE;
            } else 
#endif
            {
                /* 
                 * We reach here for devices where EGR_VLAN_CONTROL_1 is a register;
                 * There's nothing to set for them as VT_ENABLEf in the register 
                 * would have been programmed through bcmVlanTranslateEgressEnable
                 */
                return BCM_E_NONE;
            }
        default:
            return BCM_E_UNAVAIL;
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */
    return BCM_E_UNAVAIL;
}

int
bcm_esw_vlan_control_port_set(int unit, bcm_port_t port,
                              bcm_vlan_control_port_t type, int arg)
{
    bcm_pbmp_t temp_pbmp;
#if defined(BCM_HGPROXY_COE_SUPPORT)
    int i;
    bcm_gport_t gport;
#endif /* BCM_HGPROXY_COE_SUPPORT */

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_vlan_control_port_set(unit, port, type, arg);
    }
#endif

    if (port == -1) {
        BCM_PBMP_CLEAR(temp_pbmp);
        BCM_PBMP_ASSIGN(temp_pbmp, PBMP_E_ALL(unit));

#ifdef BCM_KATANA2_SUPPORT
        if (soc_feature(unit, soc_feature_linkphy_coe) ||
            soc_feature(unit, soc_feature_subtag_coe)) {
            _bcm_kt2_subport_pbmp_update (unit, &temp_pbmp);
        }
        if (SOC_IS_KATANA2(unit) && soc_feature(unit, soc_feature_flex_port)) {
            BCM_IF_ERROR_RETURN(_bcm_kt2_flexio_pbmp_update(unit, &temp_pbmp));
        }
#endif

        PBMP_ITER(temp_pbmp, port) {
            BCM_IF_ERROR_RETURN
                (_bcm_vlan_control_port_set(unit, port, type, arg));
        }

#if defined(BCM_HGPROXY_COE_SUPPORT)
        BCM_HGPROXY_SUBTAG_COE_PORT_ITER(i, gport) {
            BCM_IF_ERROR_RETURN
                (_bcm_vlan_control_port_set(unit, gport, type, arg));
        }
#endif /* BCM_HGPROXY_COE_SUPPORT */
    } else {
        BCM_IF_ERROR_RETURN
            (bcm_esw_vlan_control_port_validate(unit, port, type, &port));
        return _bcm_vlan_control_port_set(unit, port, type, arg);
    }

    return BCM_E_NONE;
}

int
bcm_esw_vlan_control_port_get(int unit, bcm_port_t port,
                              bcm_vlan_control_port_t type, int * arg)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_vlan_control_port_get(unit, port, type, arg);
    }
#endif

    if (arg == 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (bcm_esw_vlan_control_port_validate(unit, port, type, &port));

#if defined(BCM_FIREBOLT_SUPPORT)
    if (SOC_IS_FBX(unit)) {
        uint32 evc;
        soc_reg_t egr_register;
        int key_type_value;
#ifdef BCM_TRIDENT2PLUS_SUPPORT
        soc_field_t field = VT_ENABLEf;
        soc_mem_t egr_mem;
        egr_mem = EGR_VLAN_CONTROL_1m;
#endif
        egr_register = EGR_VLAN_CONTROL_1r;

        switch (type) {
        case bcmVlanPortPreferIP4:
            return _bcm_esw_port_config_get(unit, port, _bcmPortVlanPrecedence,
                                            arg);

        case bcmVlanPortPreferMAC:
            SOC_IF_ERROR_RETURN
                (_bcm_esw_port_config_get(unit, port, _bcmPortVlanPrecedence,
                                            arg));
            *arg = !*arg;
            return BCM_E_NONE;

        case bcmVlanTranslateIngressEnable:
            if (soc_feature(unit, soc_feature_vlan_translation)) {
                return (_bcm_esw_port_config_get(unit, port,
                                                 _bcmPortVlanTranslate, arg));
            }
            return BCM_E_UNAVAIL;

        case bcmVlanTranslateIngressMissDrop:
            if (soc_feature(unit, soc_feature_vlan_translation)) {
                return (_bcm_esw_port_config_get(unit, port,
                                                 _bcmPortVTMissDrop, arg));
            }
            return BCM_E_UNAVAIL;

        case bcmVlanTranslateEgressEnable:
            if (soc_feature(unit, soc_feature_vlan_translation)) {
#ifdef BCM_TRIDENT2PLUS_SUPPORT
                if (SOC_MEM_IS_VALID(unit, egr_mem)) {
                    egr_vlan_control_1_entry_t entry;
                    if (soc_feature(unit, soc_feature_egr_all_profile)
#if defined(BCM_HGPROXY_COE_SUPPORT)
                        || ((soc_feature(unit, soc_feature_hgproxy_subtag_coe)
                        || soc_feature(unit, soc_feature_channelized_switching))
                        && BCM_GPORT_IS_SET(port) &&
                            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port))
#endif
                        ) {
                        SOC_IF_ERROR_RETURN(
                            bcm_esw_port_egr_lport_field_get(unit, port,
                            EGR_VLAN_CONTROL_1m, VT_ENABLEf, (uint32 *)arg));
                        return BCM_E_NONE;
                    }
                    SOC_IF_ERROR_RETURN
                            (soc_mem_read(unit, egr_mem, MEM_BLOCK_ANY, port, &entry));
                    *arg = soc_mem_field32_get(unit, egr_mem, &entry, VT_ENABLEf);
                } else
#endif
                {
                    SOC_IF_ERROR_RETURN
                            (soc_reg32_get(unit, egr_register, port, 0, &evc));
                    *arg = soc_reg_field_get(unit, egr_register, evc, VT_ENABLEf);
                }
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;

        case bcmVlanTranslateEgressMissDrop:
            if (soc_feature(unit, soc_feature_vlan_translation)) {
#ifdef BCM_TRIDENT2PLUS_SUPPORT
                if (SOC_MEM_IS_VALID(unit, egr_mem)) {
                    egr_vlan_control_1_entry_t entry;
                    if (soc_feature(unit, soc_feature_egr_all_profile)
#if defined(BCM_HGPROXY_COE_SUPPORT)
                        || ((soc_feature(unit, soc_feature_hgproxy_subtag_coe)
                        || soc_feature(unit, soc_feature_channelized_switching))
                        && BCM_GPORT_IS_SET(port) &&
                            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port))
#endif
                        ) {
                        SOC_IF_ERROR_RETURN(
                            bcm_esw_port_egr_lport_field_get(unit, port,
                            EGR_VLAN_CONTROL_1m, VT_MISS_DROPf, (uint32 *)arg));
                        return BCM_E_NONE;
                    }
                    SOC_IF_ERROR_RETURN
                            (soc_mem_read(unit, egr_mem, MEM_BLOCK_ANY, port, &entry));
                    *arg = soc_mem_field32_get(unit, egr_mem, &entry, VT_MISS_DROPf);
                } else
#endif
                {
                    SOC_IF_ERROR_RETURN
                            (soc_reg32_get(unit, egr_register, port, 0, &evc));
                    *arg = soc_reg_field_get(unit, egr_register, evc,
                                                         VT_MISS_DROPf);
                }
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;

        case bcmVlanTranslateEgressMissUntaggedDrop:
            if ((soc_feature(unit, soc_feature_untagged_vt_miss)) &&
                (soc_feature(unit,soc_feature_vlan_translation))) {
#ifdef BCM_TRIDENT2PLUS_SUPPORT
                if (SOC_MEM_IS_VALID(unit, egr_mem)) {
                    egr_vlan_control_1_entry_t entry;
                    if (soc_feature(unit, soc_feature_egr_all_profile)
#if defined(BCM_HGPROXY_COE_SUPPORT)
                        || ((soc_feature(unit, soc_feature_hgproxy_subtag_coe)
                        || soc_feature(unit, soc_feature_channelized_switching))
                        && BCM_GPORT_IS_SET(port) &&
                            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port))
#endif
                        ) {
                        SOC_IF_ERROR_RETURN(
                            bcm_esw_port_egr_lport_field_get(unit, port,
                            EGR_VLAN_CONTROL_1m,
                                            VT_MISS_UT_DROPf, (uint32 *)arg));
                        return BCM_E_NONE;
                    }
                    SOC_IF_ERROR_RETURN
                            (soc_mem_read(unit, egr_mem, MEM_BLOCK_ANY, port, &entry));
                    *arg = soc_mem_field32_get(unit, egr_mem, &entry, VT_MISS_UT_DROPf);
                } else
#endif
                {
                    SOC_IF_ERROR_RETURN
                            (soc_reg32_get(unit, egr_register, port, 0, &evc));
                    *arg = soc_reg_field_get(unit, egr_register, evc, VT_MISS_UT_DROPf);
                }
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;

        case bcmVlanTranslateEgressMissUntag:
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) || \
            defined(BCM_RAVEN_SUPPORT)
#ifdef BCM_TRIDENT2PLUS_SUPPORT
            if (SOC_MEM_IS_VALID(unit, egr_mem)) {
                egr_vlan_control_1_entry_t entry;
                if (SOC_MEM_FIELD_VALID(unit, egr_mem, VT_MISS_UNTAGf)) {
                    if (soc_feature(unit, soc_feature_egr_all_profile)
#if defined(BCM_HGPROXY_COE_SUPPORT)
                        || ((soc_feature(unit, soc_feature_hgproxy_subtag_coe)
                        || soc_feature(unit, soc_feature_channelized_switching))
                        && BCM_GPORT_IS_SET(port) &&
                            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port))
#endif
                        ) {
                        SOC_IF_ERROR_RETURN(
                            bcm_esw_port_egr_lport_field_get(unit, port,
                            EGR_VLAN_CONTROL_1m,
                                            VT_MISS_UNTAGf, (uint32 *)arg));
                        return BCM_E_NONE;
                    }
                    SOC_IF_ERROR_RETURN
                            (soc_mem_read(unit, egr_mem, MEM_BLOCK_ANY, port, &entry));
                    *arg = soc_mem_field32_get(unit, egr_mem, &entry, VT_MISS_UNTAGf);
                    return BCM_E_NONE; 
                }
            } else
#endif
            {
                if (SOC_REG_FIELD_VALID(unit, egr_register, VT_MISS_UNTAGf)) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, egr_register, port, 0, &evc));
                    *arg = soc_reg_field_get(unit, egr_register, evc,
                                             VT_MISS_UNTAGf);
                    return BCM_E_NONE; 
                }
            }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */
            return BCM_E_UNAVAIL;

        case bcmVlanLookupMACEnable:
            return _bcm_esw_port_config_get(unit, port,
                                            _bcmPortLookupMACEnable, arg);
        case bcmVlanLookupIPEnable:
            return _bcm_esw_port_config_get(unit, port,
                                            _bcmPortLookupIPEnable, arg);
        case bcmVlanPortUseInnerPri:
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
            if (BCM_GPORT_IS_MIM_PORT(port) || BCM_GPORT_IS_MPLS_PORT(port) ||
                BCM_GPORT_IS_VXLAN_PORT(port)) {
                return (_bcm_tr2_svp_field_get(unit, port, 
                                               USE_INNER_PRIf, arg));
            }
#endif
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
 || defined(BCM_RAVEN_SUPPORT)
            if (SOC_IS_FIREBOLT2(unit) || SOC_IS_TRX(unit) ||
                SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit) ||
                SOC_IS_HURRICANEX(unit)) {
                if (BCM_GPORT_IS_SET(port)) {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_local_get(unit, port, &port));
                }
                return (_bcm_esw_port_config_get(unit, port,
                                                _bcmPortUseInnerPri, arg));
            }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */
            return BCM_E_UNAVAIL;
        case bcmVlanPortUseOuterPri:
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
            if (BCM_GPORT_IS_MPLS_PORT(port) || BCM_GPORT_IS_MIM_PORT(port)) {
                return (_bcm_tr2_svp_field_get(unit, port, 
                                               TRUST_OUTER_DOT1Pf, arg));
            }
#endif
#if defined(BCM_TRX_SUPPORT)
            if (SOC_IS_TRX(unit)) {
                if (BCM_GPORT_IS_SET(port)) {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_local_get(unit, port, &port));
                }
                return (_bcm_esw_port_config_get(unit, port,
                                                _bcmPortUseOuterPri, arg));
            }
#endif /* BCM_TRX_SUPPORT */
            return BCM_E_UNAVAIL;
        case bcmVlanPortVerifyOuterTpid:
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAPTOR_SUPPORT) \
 || defined(BCM_TRX_SUPPORT)
            if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit) ||
                SOC_IS_TRX(unit) || SOC_IS_HAWKEYE(unit) ||
                SOC_IS_HURRICANEX(unit)) {
                return (_bcm_esw_port_config_get(unit, port,
                                                _bcmPortVerifyOuterTpid, arg));
            }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAPTOR_SUPPORT || BCM_TRX_SUPPORT */
            return BCM_E_UNAVAIL;
        case bcmVlanPortOuterTpidSelect:

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) \
 || defined(BCM_TRX_SUPPORT)
            if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit) ||
                SOC_IS_TRX(unit) || SOC_IS_HAWKEYE(unit) ||
                SOC_IS_HURRICANEX(unit)) {
#ifdef BCM_TRIDENT2PLUS_SUPPORT
                if (soc_feature(unit, soc_feature_egr_vlan_control_is_memory)) {
                    egr_vlan_control_1_entry_t entry;
                    soc_field_t tpid_sel_f = OUTER_TPID_VALIDf;
                    if (SOC_MEM_FIELD_VALID(unit, EGR_VLAN_CONTROL_1m,
                                            OUTER_TPID_SELf)) {
                        tpid_sel_f = OUTER_TPID_SELf;
                    }
                    if (soc_feature(unit, soc_feature_egr_all_profile)
#if defined(BCM_HGPROXY_COE_SUPPORT)
                        || ((soc_feature(unit, soc_feature_hgproxy_subtag_coe)
                        || soc_feature(unit, soc_feature_channelized_switching))
                        && BCM_GPORT_IS_SET(port) &&
                            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port))
#endif
                        ) {
                        uint32 value;
                        SOC_IF_ERROR_RETURN(
                            bcm_esw_port_egr_lport_field_get(unit, port,
                            EGR_VLAN_CONTROL_1m,
                            tpid_sel_f, &value));
                        *arg = value;
                        if (tpid_sel_f == OUTER_TPID_VALIDf) {
                            *arg = value==1 ? BCM_PORT_OUTER_TPID :
                                   BCM_VLAN_OUTER_TPID;
                        }
                        return BCM_E_NONE;
                    }
                    SOC_IF_ERROR_RETURN(
                            READ_EGR_VLAN_CONTROL_1m(unit, MEM_BLOCK_ANY, port, &entry));
                    *arg = soc_EGR_VLAN_CONTROL_1m_field32_get(unit, &entry,
                                                               tpid_sel_f);
                } else
#endif
                {
                    SOC_IF_ERROR_RETURN
                        (READ_EGR_VLAN_CONTROL_1r(unit, port, &evc));
                    *arg = soc_reg_field_get(unit, EGR_VLAN_CONTROL_1r,
                                             evc, OUTER_TPID_SELf);
                }
                return BCM_E_NONE;
            }

#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */
            return BCM_E_UNAVAIL;
        case bcmVlanPortTranslateKeyFirst:
        case bcmVlanPortTranslateKeySecond:
#if defined(BCM_TRX_SUPPORT)
            if (SOC_IS_TRX(unit)) {
                _bcm_port_config_t vt_key_type;
                _bcm_port_config_t vt_key_port;
                int key_type = 0, use_port = 0;
                int key_type_value;

                if (type == bcmVlanPortTranslateKeyFirst) {
                    vt_key_type = _bcmPortVTKeyTypeFirst;
                    vt_key_port = _bcmPortVTKeyPortFirst;
                } else {
                    vt_key_type = _bcmPortVTKeyTypeSecond;
                    vt_key_port = _bcmPortVTKeyPortSecond;
                }

                if (BCM_GPORT_IS_NIV_PORT(port)) {
                    BCM_IF_ERROR_RETURN
                        (bcm_esw_vlan_control_port_niv_resolve(unit, port,
                                                               &port));
                }

                BCM_IF_ERROR_RETURN
                    (_bcm_esw_port_config_get(unit, port,
                                              vt_key_type, 
                                              &key_type_value));

                BCM_IF_ERROR_RETURN
                    (_bcm_esw_pt_vtkey_type_get(unit,
                             key_type_value,&key_type));

                BCM_IF_ERROR_RETURN
                    (_bcm_esw_port_config_get(unit, port,
                                              vt_key_port, 
                                              &use_port));
                switch (key_type) {
                case VLXLT_HASH_KEY_TYPE_IVID_OVID:
                    if (use_port == 3) {
                        *arg = bcmVlanTranslateKeyPortGroupDouble;
                    } else {
                        *arg = (use_port) ? bcmVlanTranslateKeyPortDouble :
                                            bcmVlanTranslateKeyDouble;
                    }
                    break;
                case VLXLT_HASH_KEY_TYPE_OTAG:
                    if (use_port == 3) {
                        *arg = bcmVlanTranslateKeyPortGroupOuterTag;
                    } else {
                        *arg = (use_port) ? bcmVlanTranslateKeyPortOuterTag :
                                            bcmVlanTranslateKeyOuterTag;
                    }
                    break;
                case VLXLT_HASH_KEY_TYPE_ITAG:
                    if (use_port == 3) {
                        *arg = bcmVlanTranslateKeyPortGroupInnerTag;
                    } else {
                        *arg = (use_port) ? bcmVlanTranslateKeyPortInnerTag :
                                            bcmVlanTranslateKeyInnerTag;
                    }
                    break;
                case VLXLT_HASH_KEY_TYPE_OVID:
                    if (use_port == 3) {
                        *arg = bcmVlanTranslateKeyPortGroupOuter;
                    } else {
                        *arg = (use_port) ? bcmVlanTranslateKeyPortOuter :
                                            bcmVlanTranslateKeyOuter;
                    }
                    break;
                case VLXLT_HASH_KEY_TYPE_IVID:
                    if (use_port == 3) {
                        *arg = bcmVlanTranslateKeyPortGroupInner;
                    } else {
                        *arg = (use_port) ? bcmVlanTranslateKeyPortInner :
                                            bcmVlanTranslateKeyInner;
                    }
                    break;
                case VLXLT_HASH_KEY_TYPE_PRI_CFI:
                    if (use_port == 3) {
                        *arg = bcmVlanTranslateKeyPortGroupOuterPri;
                    } else {
                        *arg = (use_port) ? bcmVlanTranslateKeyPortOuterPri :
                                            bcmVlanTranslateKeyOuterPri;
                    }
                    break;
                case VLXLT_HASH_KEY_TYPE_VIF_OTAG:
                    *arg = (use_port) ? bcmVlanTranslateKeyPortOuterTag :
                                        bcmVlanTranslateKeyOuterTag;
                    break;
                case VLXLT_HASH_KEY_TYPE_VIF_ITAG:
                    *arg = (use_port) ? bcmVlanTranslateKeyPortInnerTag :
                                        bcmVlanTranslateKeyInnerTag;
                    break;
                case VLXLT_HASH_KEY_TYPE_VIF_VLAN:
                    *arg = (use_port) ? bcmVlanTranslateKeyPortOuter:
                                        bcmVlanTranslateKeyOuter;
                    break;
                case VLXLT_HASH_KEY_TYPE_VIF_CVLAN:
                    *arg = (use_port) ? bcmVlanTranslateKeyPortInner:
                                        bcmVlanTranslateKeyInner;
                    break;
                case VLXLT_HASH_KEY_TYPE_LLVID:
                    *arg = (use_port) ? bcmVlanTranslateKeyPortPonTunnel:
                                        bcmVlanTranslateKeyInvalid;
                    break;
                case VLXLT_HASH_KEY_TYPE_LLVID_OVID:
                    *arg = (use_port) ? bcmVlanTranslateKeyPortPonTunnelOuter:
                                        bcmVlanTranslateKeyInvalid;
                    break;
                case VLXLT_HASK_KEY_TYPE_LLVID_IVID:
                    *arg = (use_port) ? bcmVlanTranslateKeyPortPonTunnelOuter:
                                        bcmVlanTranslateKeyPortPonTunnelInner;
                    break;
                case VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID_OVID:
                    *arg = (use_port) ? bcmVlanTranslateKeyPortCapwapPayloadDouble:
                                        bcmVlanTranslateKeyCapwapPayloadDouble;
                    break;
                case VLXLT_HASH_KEY_TYPE_PAYLOAD_OTAG:
                    *arg = (use_port) ? bcmVlanTranslateKeyPortCapwapPayloadOuterTag:
                                        bcmVlanTranslateKeyCapwapPayloadOuterTag;
                    break;
                case VLXLT_HASH_KEY_TYPE_PAYLOAD_ITAG:
                    *arg = (use_port) ? bcmVlanTranslateKeyPortCapwapPayloadInnerTag:
                                        bcmVlanTranslateKeyCapwapPayloadInnerTag;
                    break;
                case VLXLT_HASH_KEY_TYPE_PAYLOAD_OVID:
                    *arg = (use_port) ? bcmVlanTranslateKeyPortCapwapPayloadOuter:
                                        bcmVlanTranslateKeyCapwapPayloadOuter;
                    break;
                case VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID:
                    *arg = (use_port) ? bcmVlanTranslateKeyPortCapwapPayloadInner:
                                        bcmVlanTranslateKeyCapwapPayloadInner;
                    break;
                case VLXLT_HASH_KEY_TYPE_VNID:
                    *arg = (use_port) ? bcmVlanTranslateKeyPortVxlanVnid:
                                        bcmVlanTranslateKeyVxlanVnid;
                    break;
                case VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT:
                    if (use_port == 1) {
                        *arg = bcmVlanTranslateKeyMacPort;
                    } else if (use_port == 2) {
                        *arg = bcmVlanTranslateKeyMacVirtualPort;
                    } else if (use_port == 3) {
                        *arg = bcmVlanTranslateKeyMacPortGroup;
                    }
                    break;
                default:
                    *arg = bcmVlanTranslateKeyInvalid;
                    break;
                }
                return BCM_E_NONE;
            } 
#endif /* BCM_TRX_SUPPORT */
            return BCM_E_UNAVAIL;
#if defined(BCM_KATANA2_SUPPORT)
        case bcmVlanPortOamUseXlatedInnerVlan:
            if (SOC_IS_KATANA2(unit) && (soc_feature(unit, soc_feature_oam))) {
                return (_bcm_esw_port_config_get(unit, port,
                                          _bcmPortOamUseXlatedInnerVlan, arg));
            } 
            return BCM_E_UNAVAIL;
            break;
#endif /* BCM_KATANA2_SUPPORT */  
        case bcmVlanPortTranslateEgressKey:
        case bcmVlanPortTranslateEgressKeyFirst:
        case bcmVlanPortTranslateEgressKeySecond:

#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
            if (bcmVlanPortTranslateEgressKeySecond == type) {
                if (soc_feature(unit, soc_feature_egr_vlan_xlate_second_lookup)) {
                    field = VT2_ENABLEf;
                } else {
                    return BCM_E_UNAVAIL;
                }
            }

            if (soc_feature(unit, soc_feature_egr_vlan_xlate_key_on_dvp)) {
                int key_type_value;

                if (BCM_GPORT_IS_VXLAN_PORT(port) || BCM_GPORT_IS_MIM_PORT(port)
                    || BCM_GPORT_IS_MPLS_PORT(port) || BCM_GPORT_IS_L2GRE_PORT(port)) {
                    if (bcmVlanPortTranslateEgressKeySecond == type) {
                        /* Second EVxlate lookup is not supported with VPs */
                        return BCM_E_UNAVAIL;
                    }
                    BCM_IF_ERROR_RETURN
                        (bcm_td2p_dvp_vlan_xlate_key_get(unit, port, 
                                                         &key_type_value));  
                    return (_bcm_esw_dvp_vtkey_type_get(unit, 
                                              key_type_value, arg));
                } else if (BCM_GPORT_IS_FLOW_PORT(port)) {
                    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIDENT3_SUPPORT
                    if (soc_feature(unit,soc_feature_flex_flow)) {
                        rv = bcmi_esw_flow_dvp_vlan_xlate_key_get(unit,
                                     port, arg);
                    }
#endif
                   return rv;
                }
            }
#endif /* BCM_TRIDENT2PLUS_SUPPORT &&  INCLUDE_L3 */  

            if (soc_feature(unit, soc_feature_vlan_translation)) {
#ifdef BCM_TRIDENT2PLUS_SUPPORT
                if (soc_feature(unit, soc_feature_egr_vlan_control_is_memory)) {
                    egr_vlan_control_1_entry_t entry;
                    if (soc_feature(unit, soc_feature_egr_all_profile)
#if defined(BCM_HGPROXY_COE_SUPPORT)
                        || ((soc_feature(unit, soc_feature_hgproxy_subtag_coe)
                        || soc_feature(unit, soc_feature_channelized_switching))
                        && BCM_GPORT_IS_SET(port) &&
                            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port))
#endif
                        ) {
                        SOC_IF_ERROR_RETURN(
                                bcm_esw_port_egr_lport_field_get(unit, port,
                                EGR_VLAN_CONTROL_1m,
                                    field, (uint32 *)(&key_type_value)));
                    } else {
                        if (BCM_GPORT_IS_LOCAL(port)) { 
                            port = BCM_GPORT_LOCAL_GET(port);
                        }
                        if (!SOC_PORT_VALID(unit, port)) {
                            return BCM_E_PORT;
                        }

                        SOC_IF_ERROR_RETURN
                            (soc_mem_read(unit, egr_mem, MEM_BLOCK_ANY, port, &entry));
                        key_type_value = soc_mem_field32_get(unit, egr_mem, &entry, field);
                    }
                } else
#endif
                {
                    if (BCM_GPORT_IS_LOCAL(port)) { 
                        port = BCM_GPORT_LOCAL_GET(port);
                    }
                    if (!SOC_PORT_VALID(unit, port)) {
                        return BCM_E_PORT;
                    }

                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, egr_register, port, 0, &evc));
                    key_type_value = soc_reg_field_get(unit, egr_register, evc, VT_ENABLEf);
                }
                return (_bcm_esw_egr_vtkey_type_get(unit, key_type_value, arg));
            }
            return BCM_E_UNAVAIL;

        default:
            return BCM_E_UNAVAIL;
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */
    return BCM_E_UNAVAIL;
}

#if defined(BCM_XGS_SWITCH_SUPPORT)
/****************************************************************
 *
 * Stack port updating of VLAN tables
 */

/* Boolean: returns TRUE if bitmap changed. */
STATIC int
_stk_vlan_bitmap_update(int unit, bcm_pbmp_t remove_ports,
                        bcm_pbmp_t old_ports, bcm_pbmp_t *new_ports)
{
    SOC_PBMP_ASSIGN(*new_ports, old_ports);
    SOC_PBMP_OR(*new_ports, SOC_PBMP_STACK_CURRENT(unit));
    SOC_PBMP_REMOVE(*new_ports, remove_ports);

    return SOC_PBMP_NEQ(*new_ports, old_ports);
}

STATIC void
_xgs_vlan_bitmap_get(int unit, vlan_tab_entry_t *vt, bcm_pbmp_t *ports)
{
    SOC_PBMP_CLEAR(*ports);

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        soc_mem_pbmp_field_get(unit, VLAN_TABLE(unit), vt, PORT_BITMAPf, ports);
    }
#endif
}

STATIC void
_xgs_vlan_bitmap_set(int unit, vlan_tab_entry_t *vt, bcm_pbmp_t ports)
{
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        soc_mem_pbmp_field_set(unit, VLAN_TABLE(unit), vt, PORT_BITMAPf, &ports);
    }
#endif
}
#ifdef BCM_TRIUMPH3_SUPPORT
STATIC void
_xgs_vlan_ing_bitmap_get(int unit, vlan_tab_entry_t *vt, bcm_pbmp_t *ports)
{
    SOC_PBMP_CLEAR(*ports);

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        if (SOC_MEM_FIELD_VALID(unit, VLAN_TABLE(unit), ING_PORT_BITMAPf)) {
            soc_mem_pbmp_field_get(unit, VLAN_TABLE(unit), vt, ING_PORT_BITMAPf,
                                   ports);
        }
    }
#endif
}

STATIC void
_xgs_vlan_ing_bitmap_set(int unit, vlan_tab_entry_t *vt, bcm_pbmp_t ports)
{
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        if (SOC_MEM_FIELD_VALID(unit, VLAN_TABLE(unit), ING_PORT_BITMAPf)) {
            soc_mem_pbmp_field_set(unit, VLAN_TABLE(unit), vt, ING_PORT_BITMAPf,
                                   &ports);
        } 
    }
#endif
}
#endif /*BCM_TRIUMPH3_SUPPORT*/
#endif /* BCM_XGS_SWITCH_SUPPORT */

STATIC int
_xgs_vlan_stk_update(int unit, int vid, bcm_pbmp_t add_ports,
                     bcm_pbmp_t remove_ports)
{
#if defined(BCM_XGS_SWITCH_SUPPORT)

    vlan_tab_entry_t vt;
    bcm_pbmp_t old_ports, new_ports;
    int pbmp_update=0;
#ifdef BCM_TRIUMPH3_SUPPORT
    int ing_pbmp_update=0;
    bcm_pbmp_t old_ing_ports, new_ing_ports;
#endif /* BCM_TRIUMPH3_SUPPORT*/

    /* Read in table entry.  Assume valid as given VID */
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, VLAN_TABLE(unit), MEM_BLOCK_ANY, vid, &vt));

    _xgs_vlan_bitmap_get(unit, &vt, &old_ports);

    /* See if changed */
    pbmp_update =
            _stk_vlan_bitmap_update(unit, remove_ports, old_ports, &new_ports);
    if (pbmp_update) {
        _xgs_vlan_bitmap_set(unit, &vt, new_ports);
    }

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        _xgs_vlan_ing_bitmap_get(unit, &vt, &old_ing_ports);
        ing_pbmp_update = _stk_vlan_bitmap_update(unit, remove_ports, old_ing_ports, &new_ing_ports);
        if (ing_pbmp_update) {
           _xgs_vlan_ing_bitmap_set(unit, &vt, new_ing_ports);
        }
    }

    if (pbmp_update || ing_pbmp_update)
#else
    if (pbmp_update)
#endif
    {
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, VLAN_TABLE(unit), MEM_BLOCK_ANY, vid, &vt));
    }

    /* FIREBOLT has an EGR VLAN table;
       in general, this should match VLAN_TAB */
    if (SOC_IS_FBX(unit) && soc_feature(unit, soc_feature_egr_vlan_check) ) {
#if defined(BCM_FIREBOLT_SUPPORT)
        egr_vlan_entry_t evt;
        bcm_pbmp_t old_ports, new_ports;

        /* Read in EGR_VLAN table entry.  */
        SOC_IF_ERROR_RETURN(READ_EGR_VLANm(unit, MEM_BLOCK_ANY, vid, &evt));

        if (_bcm_vlan_valid_check(unit, EGR_VLANm, (void *) &evt, 
                                  vid) == BCM_E_NONE) {
            soc_mem_pbmp_field_get(unit, EGR_VLANm, &evt, PORT_BITMAPf,
                                   &old_ports);

            /* See if changed */
            if (_stk_vlan_bitmap_update(unit, remove_ports,
                                        old_ports, &new_ports)) {
                soc_mem_pbmp_field_set(unit, EGR_VLANm, &evt, PORT_BITMAPf,
                                       &new_ports);
                SOC_IF_ERROR_RETURN(WRITE_EGR_VLANm(unit, MEM_BLOCK_ANY,
                                                    vid, &evt));
            }
        }
#endif /* BCM_FIREBOLT_SUPPORT */
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        BCM_IF_ERROR_RETURN(
            bcm_td2p_vlan_vfi_mbrship_port_set(unit, vid, TRUE,
                                               TRUE, add_ports));
        BCM_IF_ERROR_RETURN(
            bcm_td2p_vlan_vfi_mbrship_port_set(unit, vid, TRUE,
                                               FALSE, remove_ports));
        if (pbmp_update) {
            BCM_IF_ERROR_RETURN(
                    bcm_td2p_vlan_vfi_mbrship_port_set(unit, vid, FALSE,
                                                       TRUE, add_ports));
            BCM_IF_ERROR_RETURN(
                    bcm_td2p_vlan_vfi_mbrship_port_set(unit, vid, FALSE,
                                                       FALSE, remove_ports));
        }
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

#endif /* BCM_XGS_SWITCH_SUPPORT */

    return BCM_E_NONE;
}

STATIC int
_esw_vlan_stk_update(int unit, int vid, bcm_pbmp_t add_ports,
                     bcm_pbmp_t remove_ports)
{
    LOG_VERBOSE(BSL_LS_BCM_VLAN,
                (BSL_META_U(unit,
                            "VLAN %d: stk update internal: vid %d, add 0x%x, "
                             "rmv 0x%x\n"), unit, vid,
                 SOC_PBMP_WORD_GET(add_ports, 0),
                 SOC_PBMP_WORD_GET(remove_ports, 0)));

    if (SOC_IS_XGS_SWITCH(unit)) {
        return _xgs_vlan_stk_update(unit, vid, add_ports, remove_ports);
    }

    return BCM_E_NONE;  /* Ignore where not supported. */
}

STATIC void
_vlan_mem_lock(int unit)
{
#ifdef  BCM_XGS12_SWITCH_SUPPORT
    if (SOC_IS_XGS12_SWITCH(unit)) {
        soc_mem_lock(unit, VLAN_TABm);
    }
#endif

#ifdef  BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        soc_mem_lock(unit, VLAN_TABm);
        soc_mem_lock(unit, EGR_VLANm);
    }
#endif
}

STATIC void
_vlan_mem_unlock(int unit)
{
#ifdef  BCM_XGS12_SWITCH_SUPPORT
    if (SOC_IS_XGS12_SWITCH(unit)) {
        soc_mem_unlock(unit, VLAN_TABm);
    }
#endif

#ifdef  BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        soc_mem_unlock(unit, EGR_VLANm);
        soc_mem_unlock(unit, VLAN_TABm);
    }
#endif
}



int
_bcm_esw_vlan_untag_update(int unit, bcm_port_t port, int untag)
{
    int rv = BCM_E_NONE;
    bcm_pbmp_t port_pbmp, port_ubmp, vlan_pbmp, vlan_ubmp, vlan_ing_pbmp;
    int vid;

    if (!soc_feature(unit, soc_feature_xport_convertible) && !SOC_IS_RAVEN(unit)) {
        return BCM_E_NONE;
    }

    SOC_PBMP_CLEAR(port_pbmp);
    SOC_PBMP_CLEAR(port_ubmp);

    SOC_PBMP_PORT_SET(port_pbmp, port);
    if (!untag) {
        SOC_PBMP_PORT_SET(port_ubmp, port);
    }

    _vlan_mem_lock(unit);
    vid = vlan_info[unit].defl;
    rv = mbcm_driver[unit]->mbcm_vlan_port_get
        (unit, vid, &vlan_pbmp, &vlan_ubmp, &vlan_ing_pbmp);

    if (rv >= 0) {
        if (untag || SOC_PBMP_MEMBER(vlan_pbmp, port)) {
            /* port_pbmp is a mask for the other two arguments */
            BCM_PBMP_AND(vlan_ing_pbmp, port_pbmp);
            rv = mbcm_driver[unit]->mbcm_vlan_port_add
                (unit, vid, port_pbmp, port_ubmp, vlan_ing_pbmp);
        }
    }
    _vlan_mem_unlock(unit);

    return rv;
}



int
_bcm_esw_vlan_stk_update(int unit, uint32 flags)
{
    int rv = BCM_E_NONE;

    if (vlan_info[unit].vlan_auto_stack) {
        bcm_pbmp_t add_ports, remove_ports;
        int vid;
        int vlan_count = 0;

        LOG_VERBOSE(BSL_LS_BCM_VLAN,
                    (BSL_META_U(unit,
                                "VLAN %d: stk_update: flags 0x%x\n"),
                     unit, flags));

        /*
         * Force all stack ports in all VLANs and assume BLOCK registers
         * take care of DLFs and BCast; remove ports no longer stacking.
         */
        SOC_PBMP_ASSIGN(add_ports, SOC_PBMP_STACK_CURRENT(unit));

        SOC_PBMP_ASSIGN(remove_ports, SOC_PBMP_STACK_PREVIOUS(unit));
        SOC_PBMP_REMOVE(remove_ports, SOC_PBMP_STACK_CURRENT(unit));

        _vlan_mem_lock(unit);

        for (vid = BCM_VLAN_MIN;vid < BCM_VLAN_COUNT; vid++) {
            if (0 == _BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vid)) {
                continue;
            }
            vlan_count++;
            rv = _esw_vlan_stk_update(unit, vid, add_ports, remove_ports);
            if (BCM_FAILURE(rv)) {
                break;
            }
            if (vlan_info[unit].count == vlan_count) {
                break;
            }
        }
        _vlan_mem_unlock(unit);
    }
    return rv;
}


/*
 * Function:
 *      bcm_vlan_mcast_flood_set
 * Purpose:
 *      Set the VLAN Multicast flood mode (aka PFM - port filter mode)
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vid  - VLAN to change the flood setting on.
 *      mode - Multicast flood mode
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 * Notes:
 *      Available on XGS3 only.
 */
int
bcm_esw_vlan_mcast_flood_set(int unit,
                             bcm_vlan_t vlan,
                             bcm_vlan_mcast_flood_t mode)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_vlan_mc_flood_ctrl)) {
        CHECK_INIT(unit);
#if defined(BCM_TRIUMPH_SUPPORT) && defined(BCM_MPLS_SUPPORT) && \
    defined(INCLUDE_L3)
        if (_BCM_MPLS_VPN_IS_VPLS(vlan)) {
            if (soc_feature(unit, soc_feature_vfi_mc_flood_ctrl)) {
                return bcm_tr_mpls_mcast_flood_set(unit, vlan, mode);
            } else {
                return rv;
            }
        } 
#endif
        VLAN_CHK_ID(unit, vlan);

        if (_BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vlan)) {
            rv = _bcm_xgs3_vlan_mcast_flood_set(unit, vlan, mode);
        } else {
            rv = BCM_E_PARAM;
        }
    }
#endif
    return rv;
}

/*
 * Function:
 *      bcm_vlan_mcast_flood_get
 * Purpose:
 *      Get the VLAN Multicast flood mode (aka PFM - port filter mode)
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vid  - VLAN to get the flood setting for.
 *      mode - Multicast flood mode
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 * Notes:
 *      Available on XGS3 only.
 */
int
bcm_esw_vlan_mcast_flood_get(int unit,
                             bcm_vlan_t vlan,
                             bcm_vlan_mcast_flood_t *mode)
{
   int rv = BCM_E_UNAVAIL;
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_vlan_mc_flood_ctrl)) {
        CHECK_INIT(unit);
#if defined(BCM_TRIUMPH_SUPPORT) && defined(BCM_MPLS_SUPPORT) && \
    defined(INCLUDE_L3)
        if (_BCM_MPLS_VPN_IS_VPLS(vlan)) {
            if (soc_feature(unit, soc_feature_vfi_mc_flood_ctrl)) {
                return bcm_tr_mpls_mcast_flood_get(unit, vlan, mode);
            } else {
                return rv;
            }
        } 
#endif
        VLAN_CHK_ID(unit, vlan);
        if (_BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vlan)) {
            rv = _bcm_xgs3_vlan_mcast_flood_get(unit, vlan, mode);
        } else {
            rv = BCM_E_PARAM;
        }
    }
#endif
    return rv;
}


/*
 * Function:
 *      bcm_esw_vlan_block_set
 * Purpose:
 *      Set per VLAN configuration.
 * Parameters:
 *      unit    - (IN) BCM device number.
 *      vid     - (IN) VLAN to get the flood setting for.
 *      block   - (IN) VLAN block structure
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_vlan_block_set(int unit, bcm_vlan_t vid, bcm_vlan_block_t *block)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TRX_SUPPORT
    CHECK_INIT(unit);
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vfi_combo_profile) &&
        _BCM_VPN_VFI_IS_SET(vid)) {
        int vfi = 0;
        bcm_vpn_t vpn_min = 0;
        int num_vfi = soc_mem_index_count(unit, VFIm);

        _BCM_VPN_SET(vpn_min, _BCM_VPN_TYPE_VFI, 0);
        if ((vid > (vpn_min+num_vfi-1)) || (vid < vpn_min)) {
            return (BCM_E_PARAM);
        }

        _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vid);
        if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeAny)) {
            return BCM_E_NOT_FOUND;
        }

        rv = bcmi_td2p_vlan_block_set(unit, vid, block);
    } else
#endif
    {
    VLAN_CHK_ID(unit, vid);

    if (SOC_MEM_IS_VALID(unit, VLAN_PROFILE_2m))  {
        if (_BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vid)) {
            BCM_LOCK(unit);
            rv = _bcm_trx_vlan_block_set(unit, vid, block);
            BCM_UNLOCK(unit);
        } else {
            rv = BCM_E_PARAM;
        }
    }
    }
#endif /* BCM_TRX_SUPPORT */
    return rv;
}

/*
 * Function:
 *      bcm_esw_vlan_block_get
 * Purpose:
 *      Get per VLAN block configuration.
 * Parameters:
 *      unit    - (IN) BCM device number.
 *      vid     - (IN) VLAN to get the flood setting for.
 *      control - (OUT) VLAN control structure
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_vlan_block_get(int unit, bcm_vlan_t vid, bcm_vlan_block_t *block)
{
    int rv = BCM_E_UNAVAIL;

    if (NULL == block) {
        return (BCM_E_PARAM);
    }

#ifdef BCM_TRX_SUPPORT
    CHECK_INIT(unit);
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vfi_combo_profile) &&
        _BCM_VPN_VFI_IS_SET(vid)) {
        int vfi = 0;
        bcm_vpn_t vpn_min = 0;
        int num_vfi = soc_mem_index_count(unit, VFIm);

        _BCM_VPN_SET(vpn_min, _BCM_VPN_TYPE_VFI, 0);
        if ((vid > (vpn_min+num_vfi-1)) || (vid < vpn_min)) {
            return (BCM_E_PARAM);
        }

        _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vid);
        if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeAny)) {
            return BCM_E_NOT_FOUND;
        }

        rv = bcmi_td2p_vlan_block_get(unit, vid, block);
    } else
#endif
    {
    VLAN_CHK_ID(unit, vid);
    if (SOC_MEM_IS_VALID(unit, VLAN_PROFILE_2m)) {
        if (_BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vid)) {
            BCM_LOCK(unit);
            rv = _bcm_trx_vlan_block_get(unit, vid, block);
            BCM_UNLOCK(unit);
        } else {
            rv = BCM_E_PARAM;
        }
    }
    }
#endif /* BCM_TRX_SUPPORT */
    return rv;
}

/*
 * Function:
 *      bcm_esw_vlan_control_vlan_selective_set
 * Purpose:
 *      Set per VLAN configuration.
 * Parameters:
 *      unit    - (IN) StrataSwitch PCI device unit number (driver internal).
 *      vid     - (IN) VLAN to get the flood setting for.
 *      valid_fields - (IN) Valid fields for VLAN control structure, BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 *      control - (IN) VLAN control structure
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_vlan_control_vlan_selective_set(int unit, bcm_vlan_t vlan,
                              uint32 valid_fields, bcm_vlan_control_vlan_t *control)
{
    int rv = BCM_E_UNAVAIL;


    if (NULL == control) {
        return (BCM_E_PARAM);
    }

#if defined(BCM_XGS_SWITCH_SUPPORT)
    if (control->flags & BCM_VLAN_L2_LOOKUP_DISABLE) {
        return BCM_E_UNAVAIL;
    }
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_vlan_control_vlan_selective_set(unit,
                                                             vlan,
                                                             valid_fields,
                                                             control);
    }
#endif

#ifdef BCM_XGS3_SWITCH_SUPPORT
#if (defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT) || \
     defined(BCM_GREYHOUND2_SUPPORT)) && defined(INCLUDE_L3)
    if (_BCM_VPN_VFI_IS_SET(vlan)) {
        int vfi = 0;
        bcm_vpn_t vpn_min = 0;
        int max_vfi = soc_mem_index_max(unit, VFIm);

        _BCM_VPN_SET(vpn_min, _BCM_VPN_TYPE_VFI, 0);
        if ((vlan > (vpn_min + max_vfi)) || (vlan < vpn_min)) {
            return (BCM_E_PARAM);
        }

        _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vlan);
#ifdef BCM_GREYHOUND2_SUPPORT
        if (soc_feature(unit, soc_feature_vxlan_lite_riot)) {
            if (!bcmi_gh2_vxlan_vfi_used_get(unit, vfi)) {
                return BCM_E_NOT_FOUND;
            }
        } else
#endif /* BCM_GREYHOUND2_SUPPORT */
        if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeAny)) {
            return BCM_E_NOT_FOUND;
        }
    } else
#endif /* TRIDENT2PLUS || TOMAHAWK || GREYHOUND2 */
    {
        VLAN_CHK_ID(unit, vlan);
    }

    if ((control->vrf > SOC_VRF_MAX(unit))) {
        return (BCM_E_PARAM);
    }

    if (soc_feature(unit, soc_feature_vlan_ctrl)) {
        BCM_LOCK(unit);
        rv = _bcm_xgs3_vlan_control_vlan_set(unit, vlan, valid_fields, control);
        BCM_UNLOCK(unit);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_esw_vlan_control_vlan_selective_get
 * Purpose:
 *      Get per VLAN configuration.
 * Parameters:
 *      unit     - (IN) StrataSwitch PCI device unit number (driver internal).
 *      vid      - (IN) VLAN to get the flood setting for.
 *      valid_fields - (IN) Valid fields for VLAN control structure, BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 *      control  - (OUT) VLAN control structure
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_vlan_control_vlan_selective_get(int unit, bcm_vlan_t vlan,
                              uint32 valid_fields, bcm_vlan_control_vlan_t *control)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_vlan_control_vlan_selective_get(unit,
                                                             vlan,
                                                             valid_fields,
                                                             control);
    }
#endif

    if (NULL == control) {
        return (BCM_E_PARAM);
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
#if (defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT) || \
     defined(BCM_GREYHOUND2_SUPPORT)) && defined(INCLUDE_L3)
    if (_BCM_VPN_VFI_IS_SET(vlan)) {
        int vfi = 0;
        bcm_vpn_t vpn_min = 0;
        int max_vfi = soc_mem_index_max(unit, VFIm);

        _BCM_VPN_SET(vpn_min, _BCM_VPN_TYPE_VFI, 0);
        if ((vlan > (vpn_min + max_vfi)) || (vlan < vpn_min)) {
            return (BCM_E_PARAM);
        }

        _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vlan);
#ifdef BCM_GREYHOUND2_SUPPORT
        if (soc_feature(unit, soc_feature_vxlan_lite_riot)) {
            if (!bcmi_gh2_vxlan_vfi_used_get(unit, vfi)) {
                return BCM_E_NOT_FOUND;
            }
        } else
#endif /* BCM_GREYHOUND2_SUPPORT */
        if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeAny)) {
            return BCM_E_NOT_FOUND;
        }
    } else
#endif /* TRIDENT2PLUS || TOMAHAWK || GREYHOUND2 */
    {
        VLAN_CHK_ID(unit, vlan);
    }

    if (soc_feature(unit, soc_feature_vlan_ctrl)) {
        BCM_LOCK(unit);
        rv = _bcm_xgs3_vlan_control_vlan_get(unit, vlan, valid_fields, control);
        BCM_UNLOCK(unit);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_esw_vlan_control_vlan_set
 * Purpose:
 *      Set per VLAN configuration.
 * Parameters:
 *      unit    - (IN) StrataSwitch PCI device unit number (driver internal).
 *      vid     - (IN) VLAN to get the flood setting for.
 *      control - (IN) VLAN control structure
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_vlan_control_vlan_set(int unit, bcm_vlan_t vid,
/* coverity[pass_by_value] */
                              bcm_vlan_control_vlan_t control)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_XGS3_SWITCH_SUPPORT
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_vlan_control_vlan_set(unit, vid, control);
    }
#endif
    CHECK_INIT(unit);

    if (soc_feature(unit, soc_feature_vlan_ctrl)) {
        uint32 valid_fields = BCM_VLAN_CONTROL_VLAN_ALL_MASK;

        /* API semantic validation */
        if (!_BCM_VPN_VFI_IS_SET(vid)) {
            VLAN_CHK_ID(unit, vid);
        }
        if (control.vrf > SOC_VRF_MAX(unit)) {
            return (BCM_E_PARAM);
        }

        /*
         * On some devices not all fields are supported
         * Raven does not support Forwarding database support.
         */
        if (soc_feature(unit, soc_feature_forwarding_db_no_support)) {
           if (control.forwarding_vlan) {
               return BCM_E_UNAVAIL;
           }
           valid_fields &= ~BCM_VLAN_CONTROL_VLAN_FORWARDING_VLAN_MASK;
        }
        /* 
         * some device can't support VLAN based VRF due to L3_IIFm can't 
         * be indexed by VID.
         */
        if (soc_feature(unit, soc_feature_no_vlan_vrf)) {
            if (control.vrf != 0) {
                return BCM_E_UNAVAIL;
            }
            valid_fields &= ~BCM_VLAN_CONTROL_VLAN_VRF_MASK;
        }
        rv = bcm_esw_vlan_control_vlan_selective_set(
                        unit, vid, valid_fields, &control);
    }

#if defined(BCM_BRADLEY_SUPPORT)
    else if (SOC_IS_HB_GW(unit)) {
        bcm_vlan_control_vlan_t vctrl;
        vlan_tab_entry_t vt;

        /* Check for unsupported parameters */
        sal_memset(&vctrl, 0, sizeof(vctrl));
        if (control.forwarding_vlan > BCM_VLAN_MAX) {
            return BCM_E_PARAM;
        }
        vctrl.forwarding_vlan = control.forwarding_vlan;
        if (sal_memcmp(&vctrl, &control, sizeof(vctrl)) != 0) {
            return BCM_E_PARAM;
        }

        soc_mem_lock(unit, VLAN_TABm);
        rv = soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, vid, &vt);
        if (BCM_SUCCESS(rv)) {
            soc_mem_field32_set(unit, VLAN_TABm, &vt, FID_IDf,
                                control.forwarding_vlan);
            rv = soc_mem_write(unit, VLAN_TABm, MEM_BLOCK_ANY, vid, &vt);
        }
        soc_mem_unlock(unit, VLAN_TABm);
    }
#endif /* BCM_BRADLEY_SUPPORT */
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return rv;
}

/*
 * Function:
 *      bcm_esw_vlan_control_vlan_get
 * Purpose:
 *      Get per VLAN configuration.
 * Parameters:
 *      unit    - (IN) StrataSwitch PCI device unit number (driver internal).
 *      vid     - (IN) VLAN to get the flood setting for.
 *      control - (OUT) VLAN control structure
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_vlan_control_vlan_get(int unit, bcm_vlan_t vid,
                              bcm_vlan_control_vlan_t *control)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_vlan_control_vlan_get(unit, vid, control);
    }
#endif

    if (NULL == control) {
        return (BCM_E_PARAM);
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
    CHECK_INIT(unit);

    if (soc_feature(unit, soc_feature_vlan_ctrl)) {
        uint32 valid_fields = BCM_VLAN_CONTROL_VLAN_ALL_MASK;

        /* 
         * some device can't support VLAN based VRF due to L3_IIFm can't 
         * be indexed by VID
         */
        if (soc_feature(unit, soc_feature_no_vlan_vrf)) {
            valid_fields &= ~BCM_VLAN_CONTROL_VLAN_VRF_MASK;
        }
        rv = bcm_esw_vlan_control_vlan_selective_get(unit, vid, valid_fields,
                                                     control);
    }
#if defined(BCM_BRADLEY_SUPPORT)
    else if (SOC_IS_HB_GW(unit)) {
        vlan_tab_entry_t vt;

        soc_mem_lock(unit, VLAN_TABm);
        rv = soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, vid, &vt);
        if (BCM_SUCCESS(rv)) {
            control->forwarding_vlan = soc_mem_field32_get(unit, VLAN_TABm,
                                                           &vt, FID_IDf);
            if (!SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VRF_IDf)) {
               control->vrf = 0;
            }
        }
        soc_mem_unlock(unit, VLAN_TABm);
    }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return rv;
}

/*
 *   Function
 *      bcm_esw_vlan_vector_flags_set
 *   Purpose
 *      Set a one or more VLAN control vlan flags on a vlan_vector on this unit
 *   Parameters
 *      (in) int unit = unit number
 *      (in) bcm_vlan_vector_t vlan_vector = Vlan vector for values to be set
 *      (in) uint32 flags_mask
 *      (in) uint32 flags_value
 *   Returns
 *      bcm_error_t = BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *   Notes
 */
int
bcm_esw_vlan_vector_flags_set(int unit,
                              bcm_vlan_vector_t vlan_vector,
                              uint32 flags_mask,
                              uint32 flags_value)
{
    int rv;

    bcm_vlan_t              vid;
    bcm_vlan_control_vlan_t control;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_vlan_vector_flags_set(unit, vlan_vector,
                                                   flags_mask, flags_value);
    }
#endif

    /* non-supported vector flags */
    if (flags_mask & (BCM_VLAN_USE_FABRIC_DISTRIBUTION | BCM_VLAN_COSQ_ENABLE)) {
        return BCM_E_PARAM;
    }

    /* Optimistically assume success from here */
    rv = BCM_E_NONE;

    for (vid = BCM_VLAN_MIN + 1; vid < BCM_VLAN_MAX; vid++) {

       if (BCM_VLAN_VEC_GET(vlan_vector, vid)) {

          rv = bcm_esw_vlan_control_vlan_get(unit, vid, &control);

          control.flags = (~flags_mask & control.flags) | (flags_mask & flags_value);

          if (BCM_SUCCESS(rv)) {
              rv = bcm_esw_vlan_control_vlan_set(unit, vid, control);
          }
       }
    }
    return rv;
}

/*
 * Function:
 *      bcm_vlan_cross_connect_add
 * Purpose:
 *      Add a VLAN cross connect entry
 * Parameters:
 *      unit       - Device unit number
 *      outer_vlan - Outer vlan ID
 *      inner_vlan - Inner vlan ID
 *      port_1     - First port in the cross-connect
 *      port_2     - Second port in the cross-connect
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_cross_connect_add(int unit, 
                               bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan,
                               bcm_gport_t port_1, bcm_gport_t port_2)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_vlan_cross_connect_add(
                    unit, outer_vlan, inner_vlan, port_1, port_2);
    }
#endif
#ifdef BCM_TRX_SUPPORT
    
    if ((port_1 == BCM_GPORT_INVALID) || (port_2 == BCM_GPORT_INVALID)) {
        return BCM_E_PORT;
    }
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_l2_cross_connect_add(unit, outer_vlan, inner_vlan,
                                           port_1, port_2);
    }
#endif
    if (SOC_IS_TRX(unit)) {
        return bcm_tr_l2_cross_connect_add(unit, outer_vlan, inner_vlan,
                                           port_1, port_2);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_vlan_cross_connect_delete
 * Purpose:
 *      Delete a VLAN cross connect entry
 * Parameters:
 *      unit       - Device unit number
 *      outer_vlan - Outer vlan ID
 *      inner_vlan - Inner vlan ID
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_cross_connect_delete(int unit,
                                  bcm_vlan_t outer_vlan, 
                                  bcm_vlan_t inner_vlan)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_vlan_cross_connect_delete(
                                unit, outer_vlan, inner_vlan);
    }
#endif
#ifdef BCM_TRX_SUPPORT
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_l2_cross_connect_delete(unit, outer_vlan, inner_vlan);
    }
#endif
    if (SOC_IS_TRX(unit)) {
        return bcm_tr_l2_cross_connect_delete(unit, outer_vlan, inner_vlan);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_vlan_cross_connect_delete_all
 * Purpose:
 *      Delete all VLAN cross connect entries
 * Parameters:
 *      unit       - Device unit number
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_cross_connect_delete_all(int unit)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_vlan_cross_connect_delete_all(unit);
    }
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_l2_cross_connect_delete_all(unit); 
    }
#endif
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        return bcm_tr_l2_cross_connect_delete_all(unit);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_vlan_cross_connect_traverse
 * Purpose:
 *      Walks through the valid cross connect entries and calls 
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
bcm_esw_vlan_cross_connect_traverse(int unit,
                                    bcm_vlan_cross_connect_traverse_cb cb,
                                    void *user_data)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_vlan_cross_connect_traverse(unit, cb, user_data);
    }
#endif
#ifdef BCM_TRX_SUPPORT
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_l2_cross_connect_traverse(unit, cb, user_data);
    }
#endif
    if (SOC_IS_TRX(unit)) {
        return bcm_tr_l2_cross_connect_traverse(unit, cb, user_data);
    }
#endif
    return BCM_E_UNAVAIL;
}

#if defined(BCM_TRIUMPH2_SUPPORT)
STATIC int
_bcm_xgs3_vlan_table_hw_fs_set(int unit, bcm_vlan_t vid,
                               int fs_idx, int table)
{
    vlan_tab_entry_t    vt;
    int                 rv;

    /* Upper layer already checks that vid is valid */
    soc_mem_lock(unit, table);

    rv = soc_mem_read(unit, table, MEM_BLOCK_ANY, (int) vid, &vt);

    if (BCM_SUCCESS(rv) && !soc_mem_field32_get(unit, table, &vt, VALIDf)) {
        rv = BCM_E_NOT_FOUND;
    }

    if (BCM_SUCCESS(rv)) {
        soc_mem_field32_set(unit, table, &vt, SERVICE_CTR_IDXf, fs_idx);

        rv = soc_mem_write(unit, table, MEM_BLOCK_ALL, (int) vid, &vt);
    }

    soc_mem_unlock(unit, table);

    return rv;
}

int
_bcm_esw_vlan_flex_stat_index_set(int unit, bcm_vlan_t vlan, int fs_idx,
                                   uint32 flags)
{
    int rv = BCM_E_NONE;
    int rv1= BCM_E_NONE;

    BCM_LOCK(unit); /* To access the VLAN existence bitmap */
    if (_BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vlan)) {
        if (flags & _BCM_FLEX_STAT_HW_INGRESS) {
            rv = _bcm_xgs3_vlan_table_hw_fs_set(unit, vlan, fs_idx, VLAN_TABm);
        }
        if (flags & _BCM_FLEX_STAT_HW_EGRESS) {
            rv1 = _bcm_xgs3_vlan_table_hw_fs_set(unit, vlan,
                                                fs_idx, EGR_VLANm);
        }
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    BCM_UNLOCK(unit);
    if (BCM_SUCCESS(rv1)) {
        return rv;
    } else {
        return rv1;
    }
}

STATIC int
_bcm_esw_vlan_stat_param_verify(int unit, bcm_vlan_t vlan, bcm_cos_t cos)
{
#if defined(INCLUDE_L3)
    int vfi;
    int rv = BCM_E_NOT_FOUND;
#endif /* INCLUDE_L3 */

#if defined(INCLUDE_L3)
    /* 
     * Is it a VPN? Both VPLS and VPN have the same VLAN encoding hence looping
     * in booth the MIM and VPLS tables
     */
    if (_BCM_VPN_IS_VPLS(vlan) || _BCM_VPN_IS_MIM(vlan)) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
        if (SOC_IS_TOMAHAWK3(unit)) {
            rv = BCM_E_UNAVAIL;
        } else
#endif
        {
            if (soc_feature(unit, soc_feature_mpls)) {
                _BCM_MPLS_VPN_GET(vfi, _BCM_MPLS_VPN_TYPE_VPLS, vlan);
                if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMpls)) {
                    rv = BCM_E_NONE;
                }
            } else {
                rv = BCM_E_UNAVAIL;
            }

            if ((rv != BCM_E_NONE) && soc_feature(unit, soc_feature_mim)) {
                _BCM_MIM_VPN_GET(vfi, _BCM_MIM_VPN_TYPE_MIM, vlan);
                if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
                    rv =  BCM_E_NONE;
                } else {
                    rv = BCM_E_NOT_FOUND;
                }
            }
        }
        BCM_IF_ERROR_RETURN (rv);
    } else
#endif /* INCLUDE_L3 */
    {
        VLAN_CHK_ID(unit, vlan);
    }
    if (cos != BCM_COS_INVALID) {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_esw_vlan_stat_param_valid(int unit, bcm_vlan_t vlan, bcm_cos_t cos)
{
    CHECK_INIT(unit);
    if (!soc_feature(unit, soc_feature_gport_service_counters)) {
        return BCM_E_UNAVAIL;
    }
    return _bcm_esw_vlan_stat_param_verify(unit,vlan,cos);
}

typedef _bcm_flex_stat_t (*_bcm_vlan_stat_to_flex_stat_f)(bcm_vlan_stat_t
                                                          stat);

STATIC _bcm_flex_stat_t
_bcm_esw_vlan_stat_to_flex_stat(bcm_vlan_stat_t stat)
{
    _bcm_flex_stat_t flex_stat;

    switch (stat) {
    case bcmVlanStatIngressPackets:
        flex_stat = _bcmFlexStatIngressPackets;
        break;
    case bcmVlanStatIngressBytes:
        flex_stat = _bcmFlexStatIngressBytes;
        break;
    case bcmVlanStatEgressPackets:
        flex_stat = _bcmFlexStatEgressPackets;
        break;
    case bcmVlanStatEgressBytes:
        flex_stat = _bcmFlexStatEgressBytes;
        break;
    default:
        flex_stat = _bcmFlexStatNum;
    }

    return flex_stat;
}

/* Requires "idx" variable */
#define BCM_VLAN_VALUE_ARRAY_VALID(unit, nstat, value_arr) \
    for (idx = 0; idx < nstat; idx++) { \
        if (NULL == value_arr + idx) { \
            return (BCM_E_PARAM); \
        } \
    }

STATIC int
_bcm_vlan_stat_array_convert(int unit, int nstat, bcm_vlan_stat_t *stat_arr, 
                             _bcm_flex_stat_t *fs_arr,
                             _bcm_vlan_stat_to_flex_stat_f vs2fs_f)
{
    int idx;

    if ((nstat <= 0) || (nstat > _bcmFlexStatNum)) {
        return BCM_E_PARAM;
    }

    if (NULL == stat_arr) {
        return (BCM_E_PARAM);
    }

    for (idx = 0; idx < nstat; idx++) {
        fs_arr[idx] = (*vs2fs_f)(stat_arr[idx]);
    }
    return BCM_E_NONE;
}

#endif /* BCM_TRIUMPH2_SUPPORT */

/*
 * Function:
 *      bcm_esw_vlan_stat_enable_set
 * Purpose:
 *      Enable/disable collection of statistics on the indicated VLAN.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN Id.
 *      enable - (IN) Non-zero to enable counter collection, zero to disable.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_vlan_stat_enable_set(int unit, bcm_vlan_t vlan, int enable)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    uint32                     num_of_tables=0;
    uint32                     num_stat_counter_ids=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    bcm_stat_object_t          object=bcmStatObjectIngPort;
    uint32                     stat_counter_id[
                                       BCM_STAT_FLEX_COUNTER_MAX_DIRECTION]={0};
    uint32                     num_entries=0;
    int                        index=0;

#endif
 if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_stat_param_valid(unit, vlan, BCM_COS_INVALID));
    return _bcm_esw_flex_stat_enable_set(unit, _bcmFlexStatTypeService,
                             _bcm_esw_vlan_flex_stat_hw_index_set,
                                         NULL, vlan, enable,0);
 }
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    BCM_IF_ERROR_RETURN(_bcm_esw_vlan_stat_get_table_info(
                        unit,vlan,&num_of_tables,&table_info[0]));
    if (enable ) {
        for(index=0;index < num_of_tables ;index++) {
            if(table_info[index].direction == bcmStatFlexDirectionIngress) {
               BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_ingress_object(
                                   unit,table_info[index].table,
                                   table_info[index].index,NULL,&object));
            } else {
                BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_egress_object(
                                    unit,table_info[index].table,
                                    table_info[index].index,NULL,&object));
            }
            BCM_IF_ERROR_RETURN(bcm_esw_stat_group_create(
                                unit,object,bcmStatGroupModeSingle,
                                &stat_counter_id[table_info[index].direction],
                                &num_entries));
            BCM_IF_ERROR_RETURN(bcm_esw_vlan_stat_attach(
                                unit,vlan,
                                stat_counter_id[table_info[index].direction]));
        }
        return BCM_E_NONE;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_counter_id(
                            unit, num_of_tables,&table_info[0],
                            &num_stat_counter_ids,&stat_counter_id[0]));
        if ((stat_counter_id[bcmStatFlexDirectionIngress] == 0) &&
            (stat_counter_id[bcmStatFlexDirectionEgress] == 0)) {
             return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(bcm_esw_vlan_stat_detach(unit,vlan));
        if (stat_counter_id[bcmStatFlexDirectionIngress] != 0) {
            BCM_IF_ERROR_RETURN(bcm_esw_stat_group_destroy(
                                unit,
                                stat_counter_id[bcmStatFlexDirectionIngress]));
        }
        if (stat_counter_id[bcmStatFlexDirectionEgress] != 0) {
            BCM_IF_ERROR_RETURN(bcm_esw_stat_group_destroy(
                                unit,
                                stat_counter_id[bcmStatFlexDirectionEgress]));
        }
        return BCM_E_NONE;
    }
#else
    return BCM_E_UNAVAIL;
#endif
#else
    return BCM_E_UNAVAIL;
#endif
}
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
/*
 * Function:
 *      _bcm_esw_vlan_stat_get_table_info
 * Description:
 *      Provides relevant flex table information(table-name,index with
 *      direction)  for given vlan.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vlan             - (IN) VLAN Id.
 *      num_of_tables    - (OUT) Number of flex counter tables
 *      table_info       - (OUT) Flex counter tables information
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC
bcm_error_t _bcm_esw_vlan_stat_get_table_info(
            int                        unit,
            bcm_vlan_t                 vlan,
            uint32                     *num_of_tables,
            bcm_stat_flex_table_info_t *table_info)
{
#if defined(INCLUDE_L3)
    uint32 vfi=0;
    bcm_error_t rv = BCM_E_NOT_FOUND;
#endif
    (*num_of_tables) = 0;
    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN (_bcm_esw_vlan_stat_param_verify(
                         unit, vlan, BCM_COS_INVALID));
    if ((_BCM_MPLS_VPN_IS_SET(vlan)) || (_BCM_MIM_VPN_IS_SET(vlan))) {
#if defined(INCLUDE_L3)
#if defined(BCM_TOMAHAWK3_SUPPORT)
        if (SOC_IS_TOMAHAWK3(unit)) {
            return BCM_E_UNAVAIL;
        }
#endif
        /* since the VLAN encoding is same for MIM and MPLS looping in both */
        if (soc_feature(unit, soc_feature_mpls)) {
            _BCM_MPLS_VPN_GET(vfi, _BCM_MPLS_VPN_TYPE_VPLS, vlan);
            if ((_bcm_vfi_used_get(unit,vfi,_bcmVfiTypeMpls))) {
                table_info[*num_of_tables].table=VFIm;
                table_info[*num_of_tables].index=vfi;
                table_info[*num_of_tables].direction=bcmStatFlexDirectionIngress;
                (*num_of_tables)++;
                table_info[*num_of_tables].table=EGR_VFIm;
                table_info[*num_of_tables].index=vfi;
                table_info[*num_of_tables].direction=bcmStatFlexDirectionEgress;
                (*num_of_tables)++;
                rv = BCM_E_NONE;
            }
        }
        if ((rv == BCM_E_NOT_FOUND) && (soc_feature(unit, soc_feature_mim))) {
            _BCM_MIM_VPN_GET(vfi, _BCM_MIM_VPN_TYPE_MIM, vlan);
            if ((_bcm_vfi_used_get(unit,vfi,_bcmVfiTypeMim))) {
                 table_info[*num_of_tables].table=VFIm;
                 table_info[*num_of_tables].index=vfi;
                 table_info[*num_of_tables].direction=bcmStatFlexDirectionIngress;
                 (*num_of_tables)++;
                 table_info[*num_of_tables].table=EGR_VFIm;
                 table_info[*num_of_tables].index=vfi;
                 table_info[*num_of_tables].direction=bcmStatFlexDirectionEgress;
                 (*num_of_tables)++;
                 rv = BCM_E_NONE;
            }
        }
        return rv;
#else
               return BCM_E_UNAVAIL;
#endif
    } else {
        if (_BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vlan)) {
            table_info[*num_of_tables].table=VLAN_TABm;
            table_info[*num_of_tables].index=vlan;
            table_info[*num_of_tables].direction=bcmStatFlexDirectionIngress;
            (*num_of_tables)++;
#if defined(BCM_TOMAHAWK_SUPPORT)
            if (SOC_MEM_IS_VALID(unit, EGR_VLANm))
#endif
            {
                table_info[*num_of_tables].table=EGR_VLANm;
                table_info[*num_of_tables].index=vlan;
                table_info[*num_of_tables].direction=bcmStatFlexDirectionEgress;
                (*num_of_tables)++;
            }
            return BCM_E_NONE;
        }
        return BCM_E_NOT_FOUND;
    }
    /* Must not hit */
    return BCM_E_NOT_FOUND;
}
#endif
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
/*
 * Function:
 *      _bcm_esw_vlan_stat_attach
 * Description:
 *      Attach counter entries to the given vlan
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vlan             - (IN) VLAN Id.
 *      stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC bcm_error_t _bcm_esw_vlan_stat_attach(
            int            unit,
            bcm_vlan_t    vlan,
            uint32      stat_counter_id)
{
    soc_mem_t                  table=0;
    bcm_stat_flex_direction_t  direction=bcmStatFlexDirectionIngress;
    uint32                     pool_number=0;
    uint32                     base_index=0;
    bcm_stat_flex_mode_t       offset_mode=0;
    bcm_stat_object_t          object=bcmStatObjectIngPort;
    bcm_stat_group_mode_t      group_mode= bcmStatGroupModeSingle;
    uint32                     count=0;
    uint32                     actual_num_tables=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    _bcm_esw_stat_get_counter_id_info(
                  unit, stat_counter_id,
                  &group_mode,&object,&offset_mode,&pool_number,&base_index);
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_object(unit,object,&direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_group(unit,group_mode));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_table_info(
                        unit,object,1,&actual_num_tables,&table,&direction));

    BCM_IF_ERROR_RETURN(_bcm_esw_vlan_stat_get_table_info(
                        unit, vlan,&num_of_tables,&table_info[0]));
    for (count=0; count < num_of_tables ; count++) {
         if ((table_info[count].direction == direction) &&
             (table_info[count].table == table)) {
              if (direction == bcmStatFlexDirectionIngress) {
                  return _bcm_esw_stat_flex_attach_ingress_table_counters(
                         unit,table_info[count].table,table_info[count].index,
                         offset_mode, base_index, pool_number);
              } else {
                  return _bcm_esw_stat_flex_attach_egress_table_counters(
                         unit,table_info[count].table,table_info[count].index,
                         0, offset_mode,base_index,pool_number);
              } 
         }
    }
    return BCM_E_NOT_FOUND;
}
#endif

/*
 * Function:
 *      bcm_esw_vlan_stat_attach
 * Description:
 *      Attach counter entries to the given vlan
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vlan             - (IN) VLAN Id.
 *      stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_vlan_stat_attach(
            int         unit,
            bcm_vlan_t  vlan,
            uint32      stat_counter_id)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return _bcm_esw_vlan_stat_attach(unit,vlan,stat_counter_id);
    } else
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit,soc_feature_gport_service_counters)) {
        _bcm_flex_stat_type_t fs_type;
        uint32 fs_inx;
        uint32 flag;

        fs_type = _BCM_FLEX_STAT_TYPE(stat_counter_id);
        fs_inx  = _BCM_FLEX_STAT_COUNT_INX(stat_counter_id);

        if (!((fs_type == _bcmFlexStatTypeService ||
            fs_type == _bcmFlexStatTypeEgressService) && fs_inx)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_stat_param_valid(unit, vlan, BCM_COS_INVALID));

        flag = fs_type==_bcmFlexStatTypeService? _BCM_FLEX_STAT_HW_INGRESS:
                         _BCM_FLEX_STAT_HW_EGRESS;
        return _bcm_esw_flex_stat_enable_set(unit, fs_type,
                             _bcm_esw_vlan_flex_stat_hw_index_set,
                                      INT_TO_PTR(flag),
                                      vlan, TRUE,fs_inx);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
/*
 * Function:
 *      _bcm_esw_vlan_stat_detach
 * Description:
 *      Detach counter entries to the given GPORT
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vlan             - (IN) VLAN Id.
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC bcm_error_t _bcm_esw_vlan_stat_detach(
            int            unit,
            bcm_vlan_t    vlan)
{
    uint32                     count=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    bcm_error_t                rv = BCM_E_NONE;
    bcm_error_t                err_code[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION] = {BCM_E_NONE};

    BCM_IF_ERROR_RETURN(_bcm_esw_vlan_stat_get_table_info(
                        unit, vlan,&num_of_tables,&table_info[0]));

    for (count=0; count < num_of_tables ; count++) {
         if (table_info[count].direction == bcmStatFlexDirectionIngress) {
             rv = _bcm_esw_stat_flex_detach_ingress_table_counters(
                        unit, table_info[count].table, table_info[count].index);
             if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionIngress]) {
                err_code[bcmStatFlexDirectionIngress] = rv;
             }
         } else {
             rv = _bcm_esw_stat_flex_detach_egress_table_counters(
                        unit, 0, table_info[count].table, table_info[count].index);

             if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionEgress]) {
                err_code[bcmStatFlexDirectionEgress] = rv;
             }
         }
    }
    
    BCM_STAT_FLEX_DETACH_RETURN(err_code)
}

/*
 * Function:
 *      _bcm_esw_vlan_stat_detach_with_id
 * Description:
 *      Detach counter entries to the given vlan with a given stat counter id
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vlan             - (IN) VLAN Id
 *      stat_counter_id  - (IN) Stat Counter ID. 
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC bcm_error_t _bcm_esw_vlan_stat_detach_with_id(
            int	        unit,
            bcm_vlan_t  vlan,
            uint32      stat_counter_id)
{
    uint32                     count=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    bcm_error_t                rv = BCM_E_NONE;
    bcm_error_t                err_code[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION] = {BCM_E_NONE};
    bcm_stat_flex_direction_t direction=bcmStatFlexDirectionIngress;
    uint32                    pool_number=0;
    uint32                    base_index=0;
    bcm_stat_flex_mode_t      offset_mode=0;
    bcm_stat_object_t         object=bcmStatObjectIngPort;
    bcm_stat_group_mode_t     group_mode= bcmStatGroupModeSingle;
    uint32                     actual_num_tables=0;
    soc_mem_t                 table=0;
    
    
    _bcm_esw_stat_get_counter_id_info(
                  unit, stat_counter_id,
                  &group_mode,&object,&offset_mode,&pool_number,&base_index);
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_object(unit,object,&direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_group(unit,group_mode));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_table_info(
                        unit,object,1,&actual_num_tables,&table,&direction));

    BCM_IF_ERROR_RETURN(_bcm_esw_vlan_stat_get_table_info(
                        unit, vlan,&num_of_tables,&table_info[0]));

    for (count=0; count < num_of_tables ; count++) {
        if((table_info[count].direction == direction) &&
             (table_info[count].table == table)) {
            if(direction == bcmStatFlexDirectionIngress) {
                rv = _bcm_esw_stat_flex_detach_ingress_table_counters(
                           unit, table_info[count].table, table_info[count].index);
                if (BCM_E_NONE != rv &&
                    BCM_E_NONE == err_code[bcmStatFlexDirectionIngress]) {
                    err_code[bcmStatFlexDirectionIngress] = rv;
                }
            } else {
                rv = _bcm_esw_stat_flex_detach_egress_table_counters(
                           unit, 0, table_info[count].table, table_info[count].index);
                if (BCM_E_NONE != rv &&
                    BCM_E_NONE == err_code[bcmStatFlexDirectionEgress]) {
                    err_code[bcmStatFlexDirectionEgress] = rv;
                }
            }
        }
    }
    
    BCM_STAT_FLEX_DETACH_RETURN(err_code)
}
#endif

/*
 * Function:
 *      bcm_esw_vlan_stat_detach
 * Description:
 *      Detach counter entries to the given GPORT
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vlan             - (IN) VLAN Id.
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_vlan_stat_detach(
            int         unit,
            bcm_vlan_t  vlan)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return _bcm_esw_vlan_stat_detach(unit,vlan);
    } else
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit,soc_feature_gport_service_counters)) {
        uint32 flag;
        int rv;

        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_stat_param_valid(unit, vlan, BCM_COS_INVALID));

        flag = _BCM_FLEX_STAT_HW_INGRESS;
        rv = _bcm_esw_flex_stat_enable_set(unit, _bcmFlexStatTypeService,
                             _bcm_esw_vlan_flex_stat_hw_index_set,
                                      INT_TO_PTR(flag),
                                      vlan, FALSE, 1);
        flag = _BCM_FLEX_STAT_HW_EGRESS;
        rv = _bcm_esw_flex_stat_enable_set(unit,_bcmFlexStatTypeEgressService,
                             _bcm_esw_vlan_flex_stat_hw_index_set,
                                      INT_TO_PTR(flag),
                                      vlan, FALSE, 1);
        return rv;
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_vlan_stat_detach_with_id
 * Description:
 *      Detach counter entries to the given vlan with a given stat counter id
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vlan             - (IN) VLAN Id
 *      stat_counter_id  - (IN) Stat Counter ID. 
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t bcm_esw_vlan_stat_detach_with_id(
            int         unit,
            bcm_vlan_t  vlan,
            uint32      stat_counter_id)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return _bcm_esw_vlan_stat_detach_with_id(unit, vlan, stat_counter_id);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}


#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
/*
 * Function:
 *      _bcm_esw_vlan_stat_counter_get
 * Description:
 *      retrieve set of counter statistic values for the given vlan
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) hwcount is to be synced to sw count 
 *      vlan             - (IN) VLAN ID
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC bcm_error_t _bcm_esw_vlan_stat_counter_get( 
            int              unit, 
            int              sync_mode, 
            bcm_vlan_t       vlan, 
            bcm_vlan_stat_t  stat,
            uint32           num_entries,
            uint32           *counter_indexes,
            bcm_stat_value_t *counter_values)
{
    uint32                     table_count=0;
    uint32                     index_count=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_direction_t  direction=bcmStatFlexDirectionIngress;
    uint32                     byte_flag=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    if ((NULL == counter_indexes) || (NULL == counter_values)) {
        return BCM_E_PARAM;
    }

    switch(stat) {
    case bcmVlanStatIngressPackets:
    case bcmVlanStatEgressPackets:
         byte_flag=0;
         break;
    case bcmVlanStatIngressBytes:
    case bcmVlanStatEgressBytes:
         byte_flag=1;
         break;
    default:
         return BCM_E_PARAM;
    }
    if ((stat == bcmVlanStatIngressPackets) ||
        (stat == bcmVlanStatIngressBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress;
    }
    
    BCM_IF_ERROR_RETURN(_bcm_esw_vlan_stat_get_table_info(
                        unit, vlan,&num_of_tables,&table_info[0]));

    for (table_count=0; table_count < num_of_tables ; table_count++) {
         if (table_info[table_count].direction == direction) {
             for (index_count=0; index_count < num_entries ; index_count++) {
                  BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_get(
                                      unit, sync_mode,
                                      table_info[table_count].index,
                                      table_info[table_count].table,
                                      0,
                                      byte_flag,
                                      counter_indexes[index_count],
                                      &counter_values[index_count]));
             }
         }
    }
    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *      _bcm_esw_vlan_flex_stat_counter_get
 * Description:  
 *      retrieve set of counter statistic values for the given vlan
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) hwcount is to be synced to sw count 
 *      vlan             - (IN) VLAN ID
 *      stat             - (IN) Type of the counter to retrieve  
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int  
_bcm_esw_vlan_flex_stat_counter_get(int              unit,
                                    int              sync_mode,
                                    bcm_vlan_t       vlan,
                                    bcm_vlan_stat_t  stat,
                                    uint32           num_entries,
                                    uint32           *counter_indexes,
                                    bcm_stat_value_t *counter_values)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return _bcm_esw_vlan_stat_counter_get(unit, sync_mode, vlan, stat,
                      num_entries, counter_indexes, counter_values);
    } else
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit,soc_feature_gport_service_counters)) {
        uint64 val;
        int rv = BCM_E_NONE;

        rv =  _bcm_esw_flex_stat_get(unit, 0,
               ((stat == bcmVlanStatIngressPackets) ||
                (stat == bcmVlanStatIngressBytes)) ?
                 _bcmFlexStatTypeService:_bcmFlexStatTypeEgressService,
                vlan,
                _bcm_esw_vlan_stat_to_flex_stat(stat), &val);

        if ((stat == bcmVlanStatIngressPackets) ||
            (stat == bcmVlanStatEgressPackets)) {
            counter_values->packets = COMPILER_64_LO(val);
        } else {
            COMPILER_64_SET(counter_values->bytes,
                      COMPILER_64_HI(val),
                      COMPILER_64_LO(val));
        }
        return rv;

    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}
/*
 * Function:
 *      bcm_esw_vlan_stat_counter_get
 * Description:  
 *      retrieve set of counter statistic values for the given vlan
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vlan             - (IN) VLAN ID
 *      stat             - (IN) Type of the counter to retrieve  
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_vlan_stat_counter_get(int              unit,
                              bcm_vlan_t       vlan,
                              bcm_vlan_stat_t  stat,
                              uint32           num_entries,
                              uint32           *counter_indexes,
                              bcm_stat_value_t *counter_values)
{
    return _bcm_esw_vlan_flex_stat_counter_get(unit, 0, vlan, stat, num_entries,
                                               counter_indexes, counter_values);
}


/*
 * Function:
 *      bcm_esw_vlan_stat_counter_sync_get
 * Description:  
 *      retrieve set of counter statistic values for the given vlan
 *      sw accumulated counters synced with hw count.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vlan             - (IN) VLAN ID
 *      stat             - (IN) Type of the counter to retrieve  
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_vlan_stat_counter_sync_get(int              unit,
                                   bcm_vlan_t       vlan,
                                   bcm_vlan_stat_t  stat,
                                   uint32           num_entries,
                                   uint32           *counter_indexes,
                                   bcm_stat_value_t *counter_values)
{
    return _bcm_esw_vlan_flex_stat_counter_get(unit, 1, vlan, stat, num_entries,
                                               counter_indexes, counter_values);
}

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
/*
 * Function:
 *      _bcm_esw_vlan_stat_counter_set
 * Description:
 *      set counter statistic values for the given VLAN
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) VLAN ID
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (IN) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC bcm_error_t _bcm_esw_vlan_stat_counter_set( 
            int                 unit, 
            bcm_vlan_t       vlan,
            bcm_vlan_stat_t  stat,
            uint32           num_entries,
            uint32           *counter_indexes,
            bcm_stat_value_t *counter_values)
{
    uint32                     table_count=0;
    uint32                     index_count=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_direction_t  direction=bcmStatFlexDirectionIngress;
    uint32                     byte_flag=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    if ((NULL == counter_indexes) || (NULL == counter_values)) {
        return BCM_E_PARAM;
    }

    switch(stat) {
    case bcmVlanStatIngressPackets:
    case bcmVlanStatEgressPackets:
         byte_flag=0;
         break;
    case bcmVlanStatIngressBytes:
    case bcmVlanStatEgressBytes:
         byte_flag=1;
         break;
    default:
         return BCM_E_PARAM;
    }
    if ((stat == bcmVlanStatIngressPackets) ||
        (stat == bcmVlanStatIngressBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress;
    }
    
    BCM_IF_ERROR_RETURN(_bcm_esw_vlan_stat_get_table_info(
                        unit, vlan,&num_of_tables,&table_info[0]));

    for (table_count=0; table_count < num_of_tables ; table_count++) {
         if (table_info[table_count].direction == direction) {
             for (index_count=0; index_count < num_entries ; index_count++) {
                  BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_set(
                                     unit,
                                     table_info[table_count].index,
                                     table_info[table_count].table,
                                     0,
                                     byte_flag,
                                     counter_indexes[index_count],
                                     &counter_values[index_count]));
             }
         }
    }
    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *      bcm_esw_vlan_stat_counter_set
 * Description:
 *      set counter statistic values for the given VLAN
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) VLAN ID
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (IN) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_vlan_stat_counter_set(
            int              unit,
            bcm_vlan_t       vlan,
            bcm_vlan_stat_t  stat,
            uint32           num_entries,
            uint32           *counter_indexes,
            bcm_stat_value_t *counter_values)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return _bcm_esw_vlan_stat_counter_set(unit,vlan,stat,
                      num_entries,counter_indexes,counter_values);
    } else
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit,soc_feature_gport_service_counters)) {
        uint64 val;
        int rv = BCM_E_NONE;

        if ((stat == bcmVlanStatIngressPackets) ||
            (stat == bcmVlanStatEgressPackets)) {
            COMPILER_64_SET(val,0,counter_values->packets);
        } else {
            COMPILER_64_SET(val,
                      COMPILER_64_HI(counter_values->bytes),
                      COMPILER_64_LO(counter_values->bytes));
        }
        rv =  _bcm_esw_flex_stat_set(unit,
               ((stat == bcmVlanStatIngressPackets) ||
                (stat == bcmVlanStatIngressBytes)) ?
                 _bcmFlexStatTypeService:_bcmFlexStatTypeEgressService,
                vlan,
                _bcm_esw_vlan_stat_to_flex_stat(stat), val);

        return rv;

    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_vlan_stat_id_get
 * Description:
 *      Get stat counter id associated with given vlan
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vlan             - (IN) VLAN ID
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      Stat_counter_id  - (OUT) Stat Counter ID
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_vlan_stat_id_get(
            int              unit,
            bcm_vlan_t       vlan,
            bcm_vlan_stat_t  stat,
            uint32           *stat_counter_id)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    bcm_stat_flex_direction_t  direction=bcmStatFlexDirectionIngress;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    uint32                     index=0;
    uint32                     num_stat_counter_ids=0;

    if ((stat == bcmVlanStatIngressPackets) ||
        (stat == bcmVlanStatIngressBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress;
    }
    BCM_IF_ERROR_RETURN(_bcm_esw_vlan_stat_get_table_info(
                        unit,vlan,&num_of_tables,&table_info[0]));
    for (index=0; index < num_of_tables ; index++) {
         if (table_info[index].direction == direction)
             return _bcm_esw_stat_flex_get_counter_id(
                                  unit, 1, &table_info[index],
                                  &num_stat_counter_ids,stat_counter_id);
    }
    return BCM_E_NOT_FOUND;
#else
    return BCM_E_UNAVAIL;
#endif
}



/*
 * Function:
 *      _bcm_esw_vlan_stat_get
 * Purpose:
 *      Extract per-VLAN statistics from the chip.
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.
 * 
 * Parameters:
 *      unit      - (IN) Unit number.
 *      sync_mode - (IN) hwcount is to be synced to sw count
 *      vlan      - (IN) VLAN Id.
 *      cos       - (IN) COS or priority
 *      stat      - (IN) Type of the counter to retrieve.
 *      val       - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_esw_vlan_stat_get(int unit, int sync_mode, 
                       bcm_vlan_t vlan, bcm_cos_t cos, 
                       bcm_vlan_stat_t stat, uint64 *val)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};
#endif

  if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_stat_param_valid(unit, vlan, cos));
    return _bcm_esw_flex_stat_get(unit, sync_mode, 
                                  _bcmFlexStatTypeService, vlan,
                                  _bcm_esw_vlan_stat_to_flex_stat(stat),
                                  val);
  }
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    BCM_IF_ERROR_RETURN (_bcm_esw_vlan_stat_param_verify(
                         unit, vlan, cos));
    BCM_IF_ERROR_RETURN(_bcm_esw_vlan_flex_stat_counter_get(unit, sync_mode, 
                                                            vlan, stat, 1, 
                                                            &counter_indexes, 
                                                            &counter_values));
    if ((stat == bcmVlanStatIngressPackets) ||
        (stat == bcmVlanStatEgressPackets)) {
         COMPILER_64_SET(*val,
                         COMPILER_64_HI(counter_values.packets64),
                         COMPILER_64_LO(counter_values.packets64));
    } else {
         COMPILER_64_SET(*val,
                         COMPILER_64_HI(counter_values.bytes),
                         COMPILER_64_LO(counter_values.bytes));
    }
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
#else
    return BCM_E_UNAVAIL;
#endif
}


/*
 * Function:
 *      bcm_esw_vlan_stat_get
 * Purpose:
 *      Extract per-VLAN statistics from the chip.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN Id.
 *      cos - (IN) COS or priority
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_vlan_stat_get(int unit, bcm_vlan_t vlan, bcm_cos_t cos, 
                      bcm_vlan_stat_t stat, uint64 *val)
{
    int rv = BCM_E_NONE;

    rv = _bcm_esw_vlan_stat_get(unit, 0, vlan, cos, stat, val);

    return rv;
}


/*
 * Function:
 *      bcm_esw_vlan_stat_sync_get
 * Purpose:
 *      Extract per-VLAN statistics from the chip.
 *      sw accumulated counters synced with hw count.
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN Id.
 *      cos - (IN) COS or priority
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_vlan_stat_sync_get(int unit, bcm_vlan_t vlan, bcm_cos_t cos, 
                           bcm_vlan_stat_t stat, uint64 *val)
{
    int rv = BCM_E_NONE;

    rv = _bcm_esw_vlan_stat_get(unit, 1, vlan, cos, stat, val);

    return rv;
}

/*
 * Function:
 *      _bcm_esw_vlan_stat_get32
 * Purpose:
 *      Extract per-VLAN statistics from the chip.
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.
 *
 * Parameters:
 *      unit      - (IN) Unit number.
 *      sync_mode - (IN) hwcount is to be synced to sw count
 *      vlan      - (IN) VLAN Id.
 *      cos       - (IN) COS or priority
 *      stat      - (IN) Type of the counter to retrieve.
 *      val       - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_esw_vlan_stat_get32(int unit, int sync_mode, 
                         bcm_vlan_t vlan, bcm_cos_t cos, 
                         bcm_vlan_stat_t stat, uint32 *val)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};
#endif
 if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_stat_param_valid(unit, vlan, cos));
    return _bcm_esw_flex_stat_get32(unit, sync_mode, 
                                    _bcmFlexStatTypeService, vlan,
                                    _bcm_esw_vlan_stat_to_flex_stat(stat), val);
  }
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    BCM_IF_ERROR_RETURN (_bcm_esw_vlan_stat_param_verify(
                         unit, vlan, cos));
    BCM_IF_ERROR_RETURN(_bcm_esw_vlan_flex_stat_counter_get(unit, sync_mode, 
                                                            vlan, stat, 1, 
                                                            &counter_indexes, 
                                                            &counter_values));
    if ((stat == bcmVlanStatIngressPackets) ||
        (stat == bcmVlanStatEgressPackets)) {
         *val = counter_values.packets;
    } else {
         /* Ignoring Hi bytes value */
         *val = COMPILER_64_LO(counter_values.bytes);
    }
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_esw_vlan_stat_get32
 * Purpose:
 *      Extract per-VLAN statistics from the chip.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN Id.
 *      cos - (IN) COS or priority
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_vlan_stat_get32(int unit, bcm_vlan_t vlan, bcm_cos_t cos, 
                        bcm_vlan_stat_t stat, uint32 *val)
{
    int rv = BCM_E_NONE;

    rv = _bcm_esw_vlan_stat_get32(unit, 0, vlan, cos, stat, val);

    return rv;
}

/*
 * Function:
 *      bcm_esw_vlan_stat_sync_get32
 * Purpose:
 *      Extract per-VLAN statistics from the chip.
 *      sw accumulated counters synced with hw count.
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN Id.
 *      cos - (IN) COS or priority
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_vlan_stat_sync_get32(int unit, bcm_vlan_t vlan, bcm_cos_t cos, 
                             bcm_vlan_stat_t stat, uint32 *val)
{
    int rv = BCM_E_NONE;

    rv = _bcm_esw_vlan_stat_get32(unit, 1, vlan, cos, stat, val);

    return rv;
}

/*
 * Function:
 *      bcm_esw_vlan_stat_set
 * Purpose:
 *      Set the specified statistic to the indicated value for the
 *      specified VLAN.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN Id.
 *      cos - (IN) COS or priority
 *      stat - (IN) Type of the counter to set.
 *      val - (IN) New counter value.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_vlan_stat_set(int unit, bcm_vlan_t vlan, bcm_cos_t cos, 
                      bcm_vlan_stat_t stat, uint64 val)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};
#endif
  if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_stat_param_valid(unit, vlan, cos));
    return _bcm_esw_flex_stat_set(unit, _bcmFlexStatTypeService, vlan,
                           _bcm_esw_vlan_stat_to_flex_stat(stat), val);
  }
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if ((stat == bcmVlanStatIngressPackets) ||
        (stat == bcmVlanStatEgressPackets)) {
         counter_values.packets = COMPILER_64_LO(val);
    } else {
         COMPILER_64_SET(counter_values.bytes,
                         COMPILER_64_HI(val),
                         COMPILER_64_LO(val));
    }
    BCM_IF_ERROR_RETURN (_bcm_esw_vlan_stat_param_verify(
                         unit, vlan, cos));
    BCM_IF_ERROR_RETURN(bcm_esw_vlan_stat_counter_set(
                        unit,vlan, stat, 1, &counter_indexes, &counter_values));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_esw_vlan_stat_set32
 * Purpose:
 *      Set the specified statistic to the indicated value for the
 *      specified VLAN.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN Id.
 *      cos - (IN) COS or priority
 *      stat - (IN) Type of the counter to set.
 *      val - (IN) New counter value.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_vlan_stat_set32(int unit, bcm_vlan_t vlan, bcm_cos_t cos, 
                        bcm_vlan_stat_t stat, uint32 val)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};
#endif

  if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_stat_param_valid(unit, vlan, cos));
    return _bcm_esw_flex_stat_set32(unit, _bcmFlexStatTypeService, vlan,
                           _bcm_esw_vlan_stat_to_flex_stat(stat), val);
  }
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if ((stat == bcmVlanStatIngressPackets) ||
        (stat == bcmVlanStatEgressPackets)) {
         counter_values.packets = val;
    } else {
         /* Ignoring high value */
         COMPILER_64_SET(counter_values.bytes,0,val);
    }
    BCM_IF_ERROR_RETURN (_bcm_esw_vlan_stat_param_verify(
                         unit, vlan, cos));
    BCM_IF_ERROR_RETURN(bcm_esw_vlan_stat_counter_set(
                        unit,vlan, stat, 1, &counter_indexes, &counter_values));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_esw_vlan_stat_multi_get
 * Purpose:
 *      Get 64-bit counter value for multiple VLAN statistic types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN Id.
 *      cos - (IN) COS or priority
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (OUT) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_vlan_stat_multi_get(int unit, bcm_vlan_t vlan, bcm_cos_t cos, 
                            int nstat, bcm_vlan_stat_t *stat_arr, 
                            uint64 *value_arr)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    _bcm_flex_stat_t fs_arr[_bcmFlexStatNum]; /* Normalize stats */
    int idx;
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};
#endif

  if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_stat_param_valid(unit, vlan, cos));
    BCM_IF_ERROR_RETURN
        (_bcm_vlan_stat_array_convert(unit, nstat, stat_arr, fs_arr,
                                      &_bcm_esw_vlan_stat_to_flex_stat));
    BCM_VLAN_VALUE_ARRAY_VALID(unit, nstat, value_arr);

    return _bcm_esw_flex_stat_multi_get(unit, _bcmFlexStatTypeService, vlan,
                                        nstat, fs_arr, value_arr);
  }
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    BCM_IF_ERROR_RETURN (_bcm_esw_vlan_stat_param_verify(
                         unit, vlan, cos));
    for (idx=0;idx < nstat ; idx ++) {
         BCM_IF_ERROR_RETURN(bcm_esw_vlan_stat_counter_get(
                      unit, vlan, stat_arr[idx], 1, 
                      &counter_indexes, &counter_values));
         if ((stat_arr[idx] == bcmVlanStatIngressPackets) ||
             (stat_arr[idx] == bcmVlanStatEgressPackets)) {
              COMPILER_64_SET(value_arr[idx],
                              COMPILER_64_HI(counter_values.packets64),
                              COMPILER_64_LO(counter_values.packets64));
         } else {
              COMPILER_64_SET(value_arr[idx],
                              COMPILER_64_HI(counter_values.bytes),
                              COMPILER_64_LO(counter_values.bytes));
         }
    }
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif

#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_esw_vlan_stat_multi_get32
 * Purpose:
 *      Get lower 32-bit counter value for multiple VLAN statistic
 *      types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN Id.
 *      cos - (IN) COS or priority
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (OUT) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_vlan_stat_multi_get32(int unit, bcm_vlan_t vlan, bcm_cos_t cos, 
                              int nstat,  bcm_vlan_stat_t *stat_arr, 
                              uint32 *value_arr)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    _bcm_flex_stat_t fs_arr[_bcmFlexStatNum]; /* Normalize stats */
    int idx;
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};
#endif

  if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_stat_param_valid(unit, vlan, cos));
    BCM_IF_ERROR_RETURN
        (_bcm_vlan_stat_array_convert(unit, nstat, stat_arr, fs_arr,
                                      &_bcm_esw_vlan_stat_to_flex_stat));
    BCM_VLAN_VALUE_ARRAY_VALID(unit, nstat, value_arr);

    return _bcm_esw_flex_stat_multi_get32(unit, _bcmFlexStatTypeService, vlan,
                                          nstat, fs_arr, value_arr);
  }
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    BCM_IF_ERROR_RETURN (_bcm_esw_vlan_stat_param_verify(
                         unit, vlan, cos));
    for (idx=0;idx < nstat ; idx ++) {
         BCM_IF_ERROR_RETURN(bcm_esw_vlan_stat_counter_get(
                             unit, vlan, stat_arr[idx], 1, 
                             &counter_indexes, &counter_values));
         if ((stat_arr[idx] == bcmVlanStatIngressPackets) ||
             (stat_arr[idx] == bcmVlanStatEgressPackets)) {
              value_arr[idx] = counter_values.packets;
         } else {
              value_arr[idx] = COMPILER_64_LO(counter_values.bytes);
         }
    }
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_esw_vlan_stat_multi_set
 * Purpose:
 *      Set 64-bit counter value for multiple VLAN statistic types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN Id.
 *      cos - (IN) COS or priority
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) New statistics descriptors array
 *      value_arr - (IN) New counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_vlan_stat_multi_set(int unit, bcm_vlan_t vlan, bcm_cos_t cos, 
                            int nstat, bcm_vlan_stat_t *stat_arr, 
                            uint64 *value_arr)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    _bcm_flex_stat_t fs_arr[_bcmFlexStatNum]; /* Normalize stats */
    int idx;
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};
#endif

  if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_stat_param_valid(unit, vlan, cos));
    BCM_IF_ERROR_RETURN
        (_bcm_vlan_stat_array_convert(unit, nstat, stat_arr, fs_arr,
                                      &_bcm_esw_vlan_stat_to_flex_stat));
    BCM_VLAN_VALUE_ARRAY_VALID(unit, nstat, value_arr);

    return _bcm_esw_flex_stat_multi_set(unit, _bcmFlexStatTypeService, vlan,
                                        nstat, fs_arr, value_arr);
  }
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    BCM_IF_ERROR_RETURN (_bcm_esw_vlan_stat_param_verify(
                         unit, vlan, cos));
    for (idx=0;idx < nstat ; idx ++) {
         if ((stat_arr[idx] == bcmVlanStatIngressPackets) ||
             (stat_arr[idx] == bcmVlanStatEgressPackets)) {
              counter_values.packets = COMPILER_64_LO(value_arr[idx]);
         } else {
              COMPILER_64_SET(counter_values.bytes,
                              COMPILER_64_HI(value_arr[idx]),
                              COMPILER_64_LO(value_arr[idx]));
         }
         BCM_IF_ERROR_RETURN(bcm_esw_vlan_stat_counter_set(
                             unit, vlan, stat_arr[idx], 1, 
                             &counter_indexes, &counter_values));
    }
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif

#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_esw_vlan_stat_multi_set32
 * Purpose:
 *      Set lower 32-bit counter value for multiple VLAN statistic
 *      types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN Id.
 *      cos - (IN) COS or priority
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) New statistics descriptors array
 *      value_arr - (IN) New counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_vlan_stat_multi_set32(int unit, bcm_vlan_t vlan, bcm_cos_t cos, 
                              int nstat, bcm_vlan_stat_t *stat_arr, 
                              uint32 *value_arr)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    _bcm_flex_stat_t fs_arr[_bcmFlexStatNum]; /* Normalize stats */
    int idx;
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};
#endif

  if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_stat_param_valid(unit, vlan, cos));
    BCM_IF_ERROR_RETURN
        (_bcm_vlan_stat_array_convert(unit, nstat, stat_arr, fs_arr,
                                      &_bcm_esw_vlan_stat_to_flex_stat));
    BCM_VLAN_VALUE_ARRAY_VALID(unit, nstat, value_arr);

    return _bcm_esw_flex_stat_multi_set32(unit, _bcmFlexStatTypeService,
                                          vlan, nstat, fs_arr, value_arr);
  }
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    BCM_IF_ERROR_RETURN (_bcm_esw_vlan_stat_param_verify(
                         unit, vlan, cos));
    for (idx=0;idx < nstat ; idx ++) {
         if ((stat_arr[idx] == bcmVlanStatIngressPackets) ||
             (stat_arr[idx] == bcmVlanStatEgressPackets)) {
              counter_values.packets = value_arr[idx];
         } else {
              COMPILER_64_SET(counter_values.bytes,0,value_arr[idx]);
         }
         BCM_IF_ERROR_RETURN(bcm_esw_vlan_stat_counter_set(
                             unit, vlan, stat_arr[idx], 1, 
                             &counter_indexes, &counter_values));
    }
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif

#else
    return BCM_E_UNAVAIL;
#endif
}

#if defined(BCM_TRIUMPH2_SUPPORT)
STATIC int
_bcm_esw_vlan_translate_flex_stat_hw_index_set(int unit,
                                     _bcm_flex_stat_handle_t handle,
                                               int fs_idx, void *cookie)
{
    vlan_xlate_entry_t  vent, res_vent;
    int rv, idx;
    soc_mem_t mem = VLAN_XLATEm;
    
    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }
    _BCM_FLEX_STAT_HANDLE_COPY(vent, handle);

    soc_mem_lock(unit, mem);
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL, &idx,
                         &vent, &res_vent, 0);
    if (BCM_SUCCESS(rv)) {
        soc_mem_field32_set(unit, mem, &res_vent,
                            VINTF_CTR_IDXf, fs_idx);
        if (SOC_MEM_FIELD_VALID(unit, mem, USE_VINTF_CTR_IDXf)) {
            soc_mem_field32_set(unit, mem, &res_vent,
                                USE_VINTF_CTR_IDXf, fs_idx > 0 ? 1 : 0);
        }
        rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, &res_vent);
        if (BCM_SUCCESS(rv)) {
            /* We found it already, but didn't overwrite it?  The mem lock
             * must have failed.  Not good. */
            return BCM_E_INTERNAL;
        } else if (BCM_E_EXISTS == rv) {
            rv = BCM_E_NONE;
        } /* Else retain failing rv value */
    }
    soc_mem_unlock(unit, mem);
    return rv;
}

STATIC _bcm_flex_stat_t
_bcm_esw_vlan_translate_stat_to_flex_stat(bcm_vlan_stat_t stat)
{
    _bcm_flex_stat_t flex_stat;

    switch (stat) {
    case bcmVlanStatIngressPackets:
        flex_stat = _bcmFlexStatIngressPackets;
        break;
    case bcmVlanStatIngressBytes:
        flex_stat = _bcmFlexStatIngressBytes;
        break;
    /* No egress stats for ingress translations */
    default:
        flex_stat = _bcmFlexStatNum;
    }

    return flex_stat;
}

STATIC int
_bcm_esw_vlan_translate_stat_param_valid(int unit, bcm_gport_t port,
                                         bcm_vlan_translate_key_t key_type,
                                         bcm_vlan_t outer_vlan,
                                         bcm_vlan_t inner_vlan,
                                         _bcm_flex_stat_handle_t *vxlt_fsh)
{
    vlan_xlate_entry_t vent;

    CHECK_INIT(unit);

    if (!soc_feature(unit, soc_feature_gport_service_counters)) {
        return BCM_E_UNAVAIL;
    }

    sal_memset(&vent, 0, sizeof(vlan_xlate_entry_t));
    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_translate_entry_assemble(unit, &vent,
                                                port, key_type,
                                                inner_vlan, outer_vlan));

    /* Fill handle for VTS info */
    _BCM_FLEX_STAT_HANDLE_CLEAR(*vxlt_fsh);
    /* This next step is a bit abusive, but we don't have an accessor
     * which captures the full key description in the VLAN_XLATE table. */
    _BCM_FLEX_STAT_HANDLE_COPY(*vxlt_fsh, vent);

    return BCM_E_NONE;
}

#endif /* BCM_TRIUMPH2_SUPPORT */

/*
 * Function:
 *      bcm_esw_vlan_translate_stat_enable_set
 * Purpose:
 *      Enable/disable collection of statistics on the indicated
 *      ingress VLAN translation.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Generic port
 *      key_type - (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      enable - (IN) Non-zero to enable counter collection, zero to disable.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_vlan_translate_stat_enable_set(int unit, bcm_gport_t port,
                                       bcm_vlan_translate_key_t key_type,
                                       bcm_vlan_t outer_vlan,
                                       bcm_vlan_t inner_vlan,
                                       int enable)
{
#if defined(BCM_TRIUMPH2_SUPPORT)|| defined(BCM_TRIDENT_SUPPORT)
    _bcm_flex_stat_handle_t vxlt_fsh;
#endif    
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    uint32                     num_of_tables=0;
    uint32                     num_stat_counter_ids=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    bcm_stat_object_t          object=bcmStatObjectIngPort;
    uint32                     stat_counter_id[
                                       BCM_STAT_FLEX_COUNTER_MAX_DIRECTION]={0};
    uint32                     num_entries=0;
    int                        index=0;
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_translate_stat_get_table_info(
                        unit,port,key_type,
                        outer_vlan,inner_vlan,
                        &num_of_tables,&table_info[0]));
        if (enable ) {
            for(index=0;index < num_of_tables ;index++) {
                if(table_info[index].direction == bcmStatFlexDirectionIngress) {
                   BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_ingress_object(
                                       unit,table_info[index].table,
                                       table_info[index].index,NULL,&object));
                } else {
                    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_egress_object(
                                        unit,table_info[index].table,
                                        table_info[index].index,NULL,&object));
                }
                BCM_IF_ERROR_RETURN(bcm_esw_stat_group_create(
                                    unit,object,bcmStatGroupModeSingle,
                                    &stat_counter_id[table_info[index].direction],
                                    &num_entries));
                BCM_IF_ERROR_RETURN(bcm_esw_vlan_translate_stat_attach(
                                    unit,port,key_type,outer_vlan,inner_vlan,
                                    stat_counter_id[table_info[index].direction]));
            }
            return BCM_E_NONE;
        } else {
            BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_counter_id(
                            unit, num_of_tables,&table_info[0],
                            &num_stat_counter_ids,&stat_counter_id[0]));
            if ((stat_counter_id[bcmStatFlexDirectionIngress] == 0) &&
                (stat_counter_id[bcmStatFlexDirectionEgress] == 0)) {
                 return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN(bcm_esw_vlan_translate_stat_detach(
                            unit,port,key_type,outer_vlan,inner_vlan));
            if (stat_counter_id[bcmStatFlexDirectionIngress] != 0) {
                BCM_IF_ERROR_RETURN(bcm_esw_stat_group_destroy(
                                    unit,
                                    stat_counter_id[bcmStatFlexDirectionIngress]));
            }
            if (stat_counter_id[bcmStatFlexDirectionEgress] != 0) {
                BCM_IF_ERROR_RETURN(bcm_esw_stat_group_destroy(
                                    unit,
                                    stat_counter_id[bcmStatFlexDirectionEgress]));
            }
            return BCM_E_NONE;
        }
    } else 
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit,soc_feature_gport_service_counters)) {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_translate_stat_param_valid(unit, port, key_type,
                                                          outer_vlan, inner_vlan,
                                                          &vxlt_fsh));

        return _bcm_esw_flex_stat_ext_enable_set(unit, _bcmFlexStatTypeVxlt,
                                 _bcm_esw_vlan_translate_flex_stat_hw_index_set,
                                                 NULL, vxlt_fsh, enable,0);
    } else
#endif
#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)) {
        return _bcm_hr3_vlan_translate_stat_enable_set(unit, port, key_type,
                   outer_vlan, inner_vlan, enable);

    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}    

#if defined(BCM_TRIUMPH3_SUPPORT)
static 
bcm_error_t _bcm_esw_vlan_translate_convert_common_fields(
            int         unit,
            soc_mem_t   from_table,
            soc_mem_t   to_table,
            soc_field_t view_field,
            char        *view_name,
            void        *from_entry,
            void        *to_entry)
{
   soc_mem_info_t   *memp_from;
   uint32           key_type=0;
   int              f=0;
#if !defined(SOC_NO_NAMES)
   soc_mem_info_t   *memp_to;
   uint32           fval[SOC_MAX_MEM_FIELD_WORDS];
   soc_field_info_t *fieldp_to;
   soc_field_info_t *fieldp_from;
#endif /* !SOC_NO_NAMES */

   char             view_name_with_colon[64];

   /* sal_memset(to_entry, 0, sizeof(vlan_xlate_extd_entry_t)); */
   memp_from = &SOC_MEM_INFO(unit, from_table);
   key_type= soc_mem_field32_get(unit, from_table,
                                 (uint32 *)from_entry , view_field);
   if (sal_strcmp(memp_from->views[key_type],view_name) != 0) {
       return BCM_E_PARAM;
   }
   sal_memset(to_entry, 0, soc_mem_entry_bytes(unit,to_table));
   sal_sprintf(view_name_with_colon,"%s:",view_name);
   for (f = memp_from->nFields - 1; f >= 0; f--) {
#if !defined(SOC_NO_NAMES)
        memp_to = &SOC_MEM_INFO(unit, to_table);
        fieldp_from = &memp_from->fields[f];
        /* LOG_CLI((BSL_META_U(unit,
                               ":%d:%s: ==>"), f,soc_fieldnames[fieldp_from->field])); */
        if ((strstr(soc_fieldnames[fieldp_from->field],
                    view_name_with_colon) != NULL) ||
            (strstr(soc_fieldnames[fieldp_from->field], ":") == NULL)) {
             if (soc_mem_field_valid(unit,
                                     to_table,fieldp_from->field)) {
                 SOC_FIND_FIELD(fieldp_from->field,
                                memp_to->fields,
                                memp_to->nFields,
                                fieldp_to);
                 if (fieldp_to->len < fieldp_from->len) {
                    LOG_CLI((BSL_META_U(unit,
                                        "Warning:%d:%s: Mismatch in length %d:%d ==>"), 
                             f,soc_fieldnames[fieldp_from->field],
                             fieldp_from->len,fieldp_to->len)); 
                 } else if (fieldp_to->len > fieldp_from->len) {
                     sal_memset(fval,0,((fieldp_to->len-1) >> 5)+1);
                     LOG_CLI((BSL_META_U(unit,
                                         "Warning:%d:%s: More lengthy field %d:%d==>"), 
                              f,soc_fieldnames[fieldp_from->field],
                              fieldp_from->len,fieldp_to->len)); 
                 }
                 soc_mem_field_get(unit, from_table,
                                   (uint32 *)from_entry ,
                                   fieldp_from->field,fval);
                 soc_mem_field_set(unit,to_table,
                                   (uint32 *)to_entry ,
                                   fieldp_from->field, fval);
                 /* LOG_CLI((BSL_META_U(unit,
                                        "Matched \n"))); */
             } else {
                 /* LOG_CLI((BSL_META_U(unit,
                                        "Not Matched \n"))); */
             }
        } else {
            /* LOG_CLI((BSL_META_U(unit,
                                   "Skipped \n"))); */
        }
#endif /* !SOC_NO_NAMES */
   }
   return BCM_E_NONE;
}
static 
bcm_error_t _bcm_esw_vlan_translate_convert_to_extd_entry(
            int                     unit,
            vlan_xlate_entry_t      *vlan_xlate_entry,
            vlan_xlate_extd_entry_t *vlan_xlate_extd_entry)
{
   uint32 key_type=0;
   uint32 fval[SOC_MAX_MEM_FIELD_WORDS];
   uint32 new_key_type=0;

   BCM_IF_ERROR_RETURN(_bcm_esw_vlan_translate_convert_common_fields(
                       unit, 
                       VLAN_XLATEm, VLAN_XLATE_EXTDm,KEY_TYPEf,"XLATE",
                       (void *)vlan_xlate_entry,
                       (void *)vlan_xlate_extd_entry));

   /* Take care about special fields */
   soc_mem_field_get(unit, VLAN_XLATEm,
                     (uint32 *)vlan_xlate_entry , VALIDf,fval);
   soc_mem_field_set(unit, VLAN_XLATE_EXTDm,
                     (uint32 *)vlan_xlate_extd_entry, VALID_0f, fval);
   soc_mem_field_set(unit, VLAN_XLATE_EXTDm,
                     (uint32 *)vlan_xlate_extd_entry, VALID_1f, fval);
   fval[0] = 0;
   fval[1] = 0;
   soc_mem_field_get(unit,VLAN_XLATEm,(uint32 *)vlan_xlate_entry,KEYf,fval);
   soc_mem_field_set(unit,VLAN_XLATE_EXTDm,(uint32 *)vlan_xlate_extd_entry,
                     XLATE__KEY_0f, fval); /* Default KEY_0f */

   soc_mem_field_get(unit, VLAN_XLATEm, (uint32 *)vlan_xlate_entry , 
                     KEY_TYPEf,&key_type);
   new_key_type=key_type+1;
   soc_mem_field_set(unit,VLAN_XLATE_EXTDm,(uint32 *)vlan_xlate_extd_entry,
                       KEY_TYPE_0f, &new_key_type);
   soc_mem_field_set(unit,VLAN_XLATE_EXTDm,(uint32 *)vlan_xlate_extd_entry,
                       KEY_TYPE_1f, &new_key_type);

   fval[0] = 0;
   fval[1] = 0;
   soc_mem_field_get(unit, VLAN_XLATEm,(uint32 *)vlan_xlate_entry, DATAf,fval);
   soc_mem_field_set(unit, VLAN_XLATE_EXTDm, 
                     (uint32 *)vlan_xlate_extd_entry, XLATE__DATA_0f, fval);
   return BCM_E_NONE;
}
static 
bcm_error_t _bcm_esw_vlan_translate_convert_to_regular_entry(
            int                     unit,
            vlan_xlate_extd_entry_t *vlan_xlate_extd_entry,
            vlan_xlate_entry_t      *vlan_xlate_entry)
{
   uint32 key_type=0;
   uint32 fval[SOC_MAX_MEM_FIELD_WORDS];
   uint32 new_key_type=0;

   BCM_IF_ERROR_RETURN(_bcm_esw_vlan_translate_convert_common_fields(
                       unit, 
                       VLAN_XLATE_EXTDm, VLAN_XLATEm,KEY_TYPE_0f,"XLATE",
                       (void *)vlan_xlate_extd_entry,
                       (void *)vlan_xlate_entry));
   /* Take care about special fields */

   fval[0] = 0;
   fval[1] = 0;
   soc_mem_field_get(unit, VLAN_XLATE_EXTDm,
                     (uint32 *)vlan_xlate_extd_entry , VALID_0f,fval);
   soc_mem_field_set(unit, VLAN_XLATEm,(uint32 *)vlan_xlate_entry,
                     VALIDf, fval);

   fval[0] = 0;
   fval[1] = 0;
   soc_mem_field_get(unit, VLAN_XLATE_EXTDm,
                     (uint32 *)vlan_xlate_extd_entry , XLATE__KEY_0f,fval);
   soc_mem_field_set(unit, VLAN_XLATEm,(uint32 *)vlan_xlate_entry,
                     KEYf, fval);

   soc_mem_field_get(unit,VLAN_XLATE_EXTDm, (uint32 *)vlan_xlate_extd_entry,
                     KEY_TYPE_0f, &key_type);
   new_key_type = key_type - 1;
   soc_mem_field_set(unit, VLAN_XLATEm, (uint32 *)vlan_xlate_entry ,
                     KEY_TYPEf, &new_key_type);

   fval[0] = 0;
   fval[1] = 0;
   soc_mem_field_get(unit, VLAN_XLATE_EXTDm, (uint32 *)vlan_xlate_extd_entry, 
                     XLATE__DATA_0f,fval);
   fval[1] &= 0x3f;
   soc_mem_field_set(unit, VLAN_XLATEm, (uint32 *)vlan_xlate_entry, DATAf,fval);

   return BCM_E_NONE;
}
#endif
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
/*
 * Function:
 *      _bcm_esw_vlan_translate_stat_get_table_info
 * Description:
 *      Provides relevant flex table information(table-name,index with
 *      direction)  for given ingress vlan translation
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) Generic port
 *      key_type         - (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan       - (IN) Packet outer VLAN ID
 *      inner_vlan       - (IN) Packet inner VLAN ID
 *      num_of_tables    - (OUT) Number of flex counter tables
 *      table_info       - (OUT) Flex counter tables information
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
static bcm_error_t _bcm_esw_vlan_translate_stat_get_table_info(
                   int                        unit, 
                   bcm_gport_t                port,
                   bcm_vlan_translate_key_t   key_type,
                   bcm_vlan_t                 outer_vlan,
                   bcm_vlan_t                 inner_vlan,
                   uint32                     *num_of_tables,
                   bcm_stat_flex_table_info_t *table_info)
{
    bcm_error_t        rv=BCM_E_NONE;
    uint32             vent[SOC_MAX_MEM_FIELD_WORDS];
    uint32             vlan_xlate_entry[SOC_MAX_MEM_FIELD_WORDS];

    int                index=0;
#if defined(BCM_TRIUMPH3_SUPPORT)
    vlan_xlate_extd_entry_t vlan_xlate_extd_entry={{0}};
#endif
    soc_mem_t               mem = VLAN_XLATEm;

    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }

    (*num_of_tables) = 0;

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }
    sal_memset(vent, 0, sizeof(vent));
    sal_memset(vlan_xlate_entry, 0, sizeof(vlan_xlate_entry));
    BCM_IF_ERROR_RETURN (_bcm_trx_vlan_translate_entry_assemble(
                         unit, vent, port, key_type, inner_vlan,outer_vlan));
    soc_mem_lock(unit, mem);
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL,&index,vent,
                        vlan_xlate_entry,0);
    soc_mem_unlock(unit, mem);
    if (SOC_IS_KATANAX(unit)) {
    if (BCM_SUCCESS(rv)) {
        table_info[*num_of_tables].table=mem;
        table_info[*num_of_tables].index=index;
        table_info[*num_of_tables].direction=bcmStatFlexDirectionIngress;
        (*num_of_tables)++;
    }
#if defined(BCM_TRIUMPH3_SUPPORT)
    } else if SOC_IS_TD2_TT2(unit) {
        if (BCM_SUCCESS(rv)) {
            table_info[*num_of_tables].table=mem;
            table_info[*num_of_tables].index=index;
            table_info[*num_of_tables].direction=bcmStatFlexDirectionIngress;
            (*num_of_tables)++;
        }
    } else if SOC_IS_TRIUMPH3(unit) {
        if (BCM_SUCCESS(rv)) {
            BCM_IF_ERROR_RETURN(_bcm_esw_vlan_translate_convert_to_extd_entry(
                                unit, (void *)vlan_xlate_entry,
                                &vlan_xlate_extd_entry));
            BCM_IF_ERROR_RETURN(soc_mem_delete(
                                unit, VLAN_XLATEm, MEM_BLOCK_ANY,
                                (void *)vlan_xlate_entry));
            BCM_IF_ERROR_RETURN(soc_mem_insert(
                                unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ALL,
                                (void *)&vlan_xlate_extd_entry));
        } else {
           /* Possibly entry has already been moved to VLAN_XLATE_EXTD table */
           BCM_IF_ERROR_RETURN(_bcm_esw_vlan_translate_convert_to_extd_entry(
                               unit, (void *)vent, &vlan_xlate_extd_entry));
        }
        BCM_IF_ERROR_RETURN(soc_mem_search(unit, VLAN_XLATE_EXTDm, 
                            MEM_BLOCK_ALL,&index,
                            (void *)&vlan_xlate_extd_entry,
                            (void *)&vlan_xlate_extd_entry,0));
        table_info[*num_of_tables].table=VLAN_XLATE_EXTDm;
        table_info[*num_of_tables].index=index;
        table_info[*num_of_tables].direction=bcmStatFlexDirectionIngress;
        (*num_of_tables)++;
        return BCM_E_NONE;
#endif
    } 
    return rv;
}
#endif

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
/*
 * Function:
 *      _bcm_esw_vlan_translate_stat_attach
 * Description:
 *      attach counters entries for the given ingress vlan translation table.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      Port             - (IN) GPORT (global port) identifier  
 *      key_type         - (IN) VLAN translation key type  
 *      outer_vlan       - (IN) Outer VLAN ID or tag  
 *      inner_vlan       - (IN) Inner VLAN ID or tag  
 *      Stat_counter_id  - (IN) Stat Counter ID.                   
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC bcm_error_t _bcm_esw_vlan_translate_stat_attach(
            int                      unit,
            bcm_gport_t              port,
            bcm_vlan_translate_key_t key_type,
            bcm_vlan_t               outer_vlan,
            bcm_vlan_t               inner_vlan,
            uint32                   stat_counter_id)
{
    soc_mem_t                  table=0;
    bcm_stat_flex_direction_t  direction=bcmStatFlexDirectionIngress;
    uint32                     pool_number=0;
    uint32                     base_index=0;
    bcm_stat_flex_mode_t       offset_mode=0;
    bcm_stat_object_t          object=bcmStatObjectIngPort;
    bcm_stat_group_mode_t      group_mode= bcmStatGroupModeSingle;
    uint32                     count=0;
    uint32                     actual_num_tables=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    _bcm_esw_stat_get_counter_id_info(
                  unit, stat_counter_id,
                  &group_mode,&object,&offset_mode,&pool_number,&base_index);
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_object(unit,object,&direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_group(unit,group_mode));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_table_info(
                        unit,object,1,&actual_num_tables,&table,&direction));
    /* When key_type is bcmVlanTranslateKeyPortOuter, 
       outer_vlan should be old vlan_id.Inner Vlan value is discarded */
    /* When key_type is bcmVlanTranslateKeyPortInn, 
       inner_vlan should be old vlan_id.Outer vlan value is discarded */
    /*
    if (key_type == bcmVlanTranslateKeyPortOuter) {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_translate_stat_get_table_info(
                            unit, port,key_type,inner_vlan,0,
                            &num_of_tables,&table_info[0]));
    } else if(key_type ==  bcmVlanTranslateKeyPortInner) {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_translate_stat_get_table_info(
                            unit, port,key_type,0,inner_vlan,
                            &num_of_tables,&table_info[0]));
    } else {
          return BCM_E_PARAM;
    }
    */
    BCM_IF_ERROR_RETURN(_bcm_esw_vlan_translate_stat_get_table_info(
                            unit, port,key_type,outer_vlan,inner_vlan,
                            &num_of_tables,&table_info[0]));
    for (count=0; count < num_of_tables ; count++) {
         if ((table_info[count].direction == direction) &&
             (table_info[count].table == table) ) {
              if (direction == bcmStatFlexDirectionIngress) {
                  return _bcm_esw_stat_flex_attach_ingress_table_counters(
                        unit, table_info[count].table, table_info[count].index,
                        offset_mode, base_index, pool_number);
              } else {
                  return _bcm_esw_stat_flex_attach_egress_table_counters(
                         unit, table_info[count].table, table_info[count].index,
                         0, offset_mode, base_index, pool_number);
              } 
         }
    }
    return BCM_E_NOT_FOUND;
}
#endif

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
/*
 * Function:
 *      _tr2_vlan_translate_stat_attach
 * Description:
 *      attach counters entries for the given ingress vlan translation table.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      Port             - (IN) GPORT (global port) identifier
 *      key_type         - (IN) VLAN translation key type
 *      outer_vlan       - (IN) Outer VLAN ID or tag
 *      inner_vlan       - (IN) Inner VLAN ID or tag
 *      Stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC bcm_error_t _tr2_vlan_translate_stat_attach(
            int                      unit,
            bcm_gport_t              port,
            bcm_vlan_translate_key_t key_type,
            bcm_vlan_t               outer_vlan,
            bcm_vlan_t               inner_vlan,
            uint32                   stat_counter_id,
            int                      attached)
{

    _bcm_flex_stat_type_t fs_type;
    uint32 fs_inx;
    _bcm_flex_stat_handle_t vxlt_fsh;

    fs_type = _BCM_FLEX_STAT_TYPE(stat_counter_id);
    fs_inx  = _BCM_FLEX_STAT_COUNT_INX(stat_counter_id);

    if (!((fs_type == _bcmFlexStatTypeVxlt) && fs_inx)) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_translate_stat_param_valid(unit, port, key_type,
                                              outer_vlan, inner_vlan,
                                                  &vxlt_fsh));
    
    return _bcm_esw_flex_stat_ext_enable_set(unit, fs_type,
                             _bcm_esw_vlan_translate_flex_stat_hw_index_set,
                             INT_TO_PTR(_BCM_FLEX_STAT_HW_INGRESS), 
                             vxlt_fsh, attached,
                             fs_inx);
}
#endif

/*
 * Function:
 *      bcm_esw_vlan_translate_stat_attach
 * Description:
 *      attach counters entries for the given ingress vlan translation table.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      Port             - (IN) GPORT (global port) identifier
 *      key_type         - (IN) VLAN translation key type
 *      outer_vlan       - (IN) Outer VLAN ID or tag
 *      inner_vlan       - (IN) Inner VLAN ID or tag
 *      Stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_vlan_translate_stat_attach(
            int                      unit,
            bcm_gport_t              port,
            bcm_vlan_translate_key_t key_type,
            bcm_vlan_t               outer_vlan,
            bcm_vlan_t               inner_vlan,
            uint32                   stat_counter_id)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return _bcm_esw_vlan_translate_stat_attach(unit,port,
               key_type,outer_vlan,inner_vlan,stat_counter_id);
    } else
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit,soc_feature_gport_service_counters)) {
        return _tr2_vlan_translate_stat_attach(unit,port,key_type,
                   outer_vlan,inner_vlan,stat_counter_id,TRUE);
    } else
#endif
#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)) {
        return _bcm_hr3_vlan_translate_stat_attach(unit,port,key_type,
                   outer_vlan,inner_vlan,stat_counter_id);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }

}

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
/*
 * Function:
 *      _bcm_esw_vlan_translate_stat_detach
 * Description:
 *      Detach counters entries for the given ingress vlan translation table.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      Port             - (IN) GPORT (global port) identifier  
 *      key_type         - (IN) VLAN translation key type  
 *      outer_vlan       - (IN) Outer VLAN ID or tag  
 *      inner_vlan       - (IN) Inner VLAN ID or tag  
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC bcm_error_t _bcm_esw_vlan_translate_stat_detach(
            int                      unit,
            bcm_gport_t              port,
            bcm_vlan_translate_key_t key_type,
            bcm_vlan_t               outer_vlan,
            bcm_vlan_t               inner_vlan)
{
    uint32                     count=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    bcm_error_t                rv = BCM_E_NONE;
    bcm_error_t                err_code[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION] = {BCM_E_NONE};
#if defined(BCM_TRIUMPH3_SUPPORT)
    vlan_xlate_entry_t         vlan_xlate_entry={{0}};
    vlan_xlate_extd_entry_t    vlan_xlate_extd_entry={{0}};
#endif

    BCM_IF_ERROR_RETURN(_bcm_esw_vlan_translate_stat_get_table_info(
                        unit, port,key_type,outer_vlan,inner_vlan,
                        &num_of_tables,&table_info[0]));

    for (count=0; count < num_of_tables ; count++) {
         if (table_info[count].direction == bcmStatFlexDirectionIngress) {
             rv = _bcm_esw_stat_flex_detach_ingress_table_counters(
                         unit, table_info[count].table,table_info[count].index);
#if defined(BCM_TRIUMPH3_SUPPORT)
             if (SOC_IS_TRIUMPH3(unit)) {
                 BCM_IF_ERROR_RETURN(soc_mem_read(
                                     unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ANY,
                                     table_info[count].index,
                                     (void *)&vlan_xlate_extd_entry));
                 if (soc_mem_field32_get(unit,VLAN_XLATE_EXTDm,
                                         (uint32 *)&vlan_xlate_extd_entry,
                                         XLATE__SVC_METER_INDEXf) == 0) {
                     BCM_IF_ERROR_RETURN(
                        _bcm_esw_vlan_translate_convert_to_regular_entry(
                         unit,&vlan_xlate_extd_entry,&vlan_xlate_entry));
                     BCM_IF_ERROR_RETURN(soc_mem_delete(
                                         unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ANY,
                                         (void *)&vlan_xlate_extd_entry));
                     BCM_IF_ERROR_RETURN(soc_mem_insert(
                                         unit, VLAN_XLATEm, MEM_BLOCK_ALL,
                                         (void *)&vlan_xlate_entry));
                 }
             }
#endif
             if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionIngress]) {
                err_code[bcmStatFlexDirectionIngress] = rv;
             }
         } else {
             rv = _bcm_esw_stat_flex_detach_egress_table_counters(
                        unit, 0, table_info[count].table,table_info[count].index);
             if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionEgress]) {
                err_code[bcmStatFlexDirectionEgress] = rv;
             }
         }
    }
    
    BCM_STAT_FLEX_DETACH_RETURN(err_code)
}
#endif

/*
 * Function:
 *      bcm_esw_vlan_translate_stat_detach
 * Description:
 *      Detach counters entries for the given ingress vlan translation table.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      Port             - (IN) GPORT (global port) identifier
 *      key_type         - (IN) VLAN translation key type
 *      outer_vlan       - (IN) Outer VLAN ID or tag
 *      inner_vlan       - (IN) Inner VLAN ID or tag
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_vlan_translate_stat_detach(
            int                      unit,
            bcm_gport_t              port,
            bcm_vlan_translate_key_t key_type,
            bcm_vlan_t               outer_vlan,
            bcm_vlan_t               inner_vlan)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return _bcm_esw_vlan_translate_stat_detach(unit,port,
               key_type,outer_vlan,inner_vlan);
    } else
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit,soc_feature_gport_service_counters)) {
        return _tr2_vlan_translate_stat_attach(unit,port,key_type,
                   outer_vlan,inner_vlan,
                   _BCM_FLEX_STAT_COUNT_ID(_bcmFlexStatTypeVxlt,1),FALSE);
    } else
#endif
#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)) {
        return _bcm_hr3_vlan_translate_stat_detach(unit,port,key_type,
                   outer_vlan,inner_vlan);
    } else
#endif 
    {
        return BCM_E_UNAVAIL;
    }
}

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
/*
 * Function:
 *      _bcm_esw_vlan_translate_stat_counter_get
 * Description:
 *      Get counter values from ingress vlan translation table
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) hwcount is to be synced to sw count 
 *      Port             - (IN) GPORT (global port) identifier  
 *      key_type         - (IN) VLAN translation key type  
 *      outer_vlan       - (IN) Outer VLAN ID or tag  
 *      inner_vlan       - (IN) Inner VLAN ID or tag  
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC bcm_error_t _bcm_esw_vlan_translate_stat_counter_get(
            int                      unit,
            int                      sync_mode,
            bcm_gport_t              port,
            bcm_vlan_translate_key_t key_type,
            bcm_vlan_t               outer_vlan,
            bcm_vlan_t               inner_vlan,
            bcm_vlan_stat_t          stat,
            uint32                   num_entries,
            uint32                   *counter_indexes,
            bcm_stat_value_t         *counter_values)
{
    uint32                          table_count=0;
    uint32                          index_count=0;
    uint32                          num_of_tables=0;
    bcm_stat_flex_direction_t       direction=bcmStatFlexDirectionIngress;
    uint32                          byte_flag=0;
    bcm_stat_flex_table_info_t      table_info[
                                    BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    if ((stat == bcmVlanStatIngressPackets) ||
        (stat == bcmVlanStatIngressBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress;
    }
    if ((stat == bcmVlanStatIngressPackets) ||
        (stat == bcmVlanStatEgressPackets)) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }
    
    BCM_IF_ERROR_RETURN(_bcm_esw_vlan_translate_stat_get_table_info(
                        unit, port,key_type,outer_vlan,inner_vlan,
                        &num_of_tables,&table_info[0]));

    for (table_count=0; table_count < num_of_tables ; table_count++) {
        if (table_info[table_count].direction == direction) {
            for (index_count=0; index_count < num_entries ; index_count++) {
                 BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_get(
                                     unit, sync_mode, 
                                     table_info[table_count].index,
                                     table_info[table_count].table, 0,
                                     byte_flag,
                                     counter_indexes[index_count],
                                     &counter_values[index_count]));
            }
        }
    }
    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *      _bcm_esw_vlan_xslate_stat_counter_get
 * Description:
 *      Get counter values from ingress vlan translation table
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) hwcount is to be synced to sw count 
 *      Port             - (IN) GPORT (global port) identifier
 *      key_type         - (IN) VLAN translation key type
 *      outer_vlan       - (IN) Outer VLAN ID or tag
 *      inner_vlan       - (IN) Inner VLAN ID or tag
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int  
bcm_esw_vlan_xslate_stat_counter_get(int                      unit,
                                     int                      sync_mode,
                                     bcm_gport_t              port,
                                     bcm_vlan_translate_key_t key_type,
                                     bcm_vlan_t               outer_vlan,
                                     bcm_vlan_t               inner_vlan,
                                     bcm_vlan_stat_t          stat,
                                     uint32                   num_entries,
                                     uint32                   *counter_indexes,
                                     bcm_stat_value_t         *counter_values)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return _bcm_esw_vlan_translate_stat_counter_get(unit, sync_mode, 
                                                        port, key_type,
                                                        outer_vlan, inner_vlan,
                                                        stat,num_entries,
                                                        counter_indexes,
                                                        counter_values);
    } else
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit,soc_feature_gport_service_counters)) {
        uint64 val;
        _bcm_flex_stat_handle_t vxlt_fsh;
        int rv = BCM_E_NONE;

        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_translate_stat_param_valid(unit, port, key_type,
                                                  outer_vlan, inner_vlan,
                                                  &vxlt_fsh));
        rv =  _bcm_esw_flex_stat_ext_get(unit, 0, _bcmFlexStatTypeVxlt,
                vxlt_fsh, _bcm_esw_vlan_translate_stat_to_flex_stat(stat), 
                &val);

        if (stat == bcmVlanStatIngressPackets) {
            counter_values->packets = COMPILER_64_LO(val);
        } else {
            COMPILER_64_SET(counter_values->bytes,
                      COMPILER_64_HI(val),
                      COMPILER_64_LO(val));
        }
        return rv;

    } else
#endif
#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)) {
        return _bcm_hr3_vlan_translate_stat_counter_get(unit, sync_mode, 
                                                        port, key_type,
                                                        outer_vlan, inner_vlan,
                                                        stat,num_entries,
                                                        counter_indexes,
                                                        counter_values);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}


/*
 * Function:
 *      bcm_esw_vlan_translate_stat_counter_get
 * Description:
 *      Get   counter values from ingress vlan translation table
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      Port             - (IN) GPORT (global port) identifier
 *      key_type         - (IN) VLAN translation key type
 *      outer_vlan       - (IN) Outer VLAN ID or tag
 *      inner_vlan       - (IN) Inner VLAN ID or tag
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int  bcm_esw_vlan_translate_stat_counter_get(
            int                      unit,
            bcm_gport_t              port,
            bcm_vlan_translate_key_t key_type,
            bcm_vlan_t               outer_vlan,
            bcm_vlan_t               inner_vlan,
            bcm_vlan_stat_t          stat,
            uint32                   num_entries,
            uint32                   *counter_indexes,
            bcm_stat_value_t         *counter_values)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
    return bcm_esw_vlan_xslate_stat_counter_get(unit, 0,  port, key_type,
                                                outer_vlan, inner_vlan,
                                                stat, num_entries,
                                                counter_indexes,
                                                counter_values);
}


/*
 * Function:
 *      bcm_esw_vlan_translate_stat_counter_sync_get
 * Description:
 *      Get counter values from ingress vlan translation table
 *      sw accumulated counters synced with hw count.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      Port             - (IN) GPORT (global port) identifier
 *      key_type         - (IN) VLAN translation key type
 *      outer_vlan       - (IN) Outer VLAN ID or tag
 *      inner_vlan       - (IN) Inner VLAN ID or tag
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_vlan_translate_stat_counter_sync_get(
            int                      unit,
            bcm_gport_t              port,
            bcm_vlan_translate_key_t key_type,
            bcm_vlan_t               outer_vlan,
            bcm_vlan_t               inner_vlan,
            bcm_vlan_stat_t          stat,
            uint32                   num_entries,
            uint32                   *counter_indexes,
            bcm_stat_value_t         *counter_values)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
    return bcm_esw_vlan_xslate_stat_counter_get(unit, 1,  port, key_type,
                                                outer_vlan, inner_vlan,
                                                stat, num_entries,
                                                counter_indexes,
                                                counter_values);
}

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
/* Function:
 *      _bcm_esw_vlan_translate_stat_counter_set
 * Description:
 *      Set counter values into a ingress vlan translation table
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      Port             - (IN) GPORT (global port) identifier  
 *      key_type         - (IN) VLAN translation key type  
 *      outer_vlan       - (IN) Outer VLAN ID or tag  
 *      inner_vlan       - (IN) Inner VLAN ID or tag  
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (IN) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC bcm_error_t _bcm_esw_vlan_translate_stat_counter_set(
            int                      unit,
            bcm_gport_t              port,
            bcm_vlan_translate_key_t key_type,
            bcm_vlan_t               outer_vlan,
            bcm_vlan_t               inner_vlan,
            bcm_vlan_stat_t          stat,
            uint32                   num_entries,
            uint32                   *counter_indexes,
            bcm_stat_value_t         *counter_values)
{
    uint32                          table_count=0;
    uint32                          index_count=0;
    uint32                          num_of_tables=0;
    bcm_stat_flex_direction_t       direction=bcmStatFlexDirectionIngress;
    uint32                          byte_flag=0;
    bcm_stat_flex_table_info_t      table_info[
                                    BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    if ((stat == bcmVlanStatIngressPackets) ||
        (stat == bcmVlanStatIngressBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress;
    }
    if ((stat == bcmVlanStatIngressPackets) ||
        (stat == bcmVlanStatEgressPackets)) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }
    
    BCM_IF_ERROR_RETURN(_bcm_esw_vlan_translate_stat_get_table_info(
                        unit, port,key_type,outer_vlan,inner_vlan,
                        &num_of_tables,&table_info[0]));

    for (table_count=0; table_count < num_of_tables ; table_count++) {
         if (table_info[table_count].direction == direction) {
             for (index_count=0; index_count < num_entries ; index_count++) {
                  BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_set(
                                      unit, table_info[table_count].index,
                                      table_info[table_count].table, 0,
                                      byte_flag,
                                      counter_indexes[index_count],
                                      &counter_values[index_count]));
             }
         }
    }
    return BCM_E_NONE;
}
#endif

/* Function:
 *      bcm_esw_vlan_translate_stat_counter_set
 * Description:
 *      Set counter values into a ingress vlan translation table
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      Port             - (IN) GPORT (global port) identifier
 *      key_type         - (IN) VLAN translation key type
 *      outer_vlan       - (IN) Outer VLAN ID or tag
 *      inner_vlan       - (IN) Inner VLAN ID or tag
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (IN) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_vlan_translate_stat_counter_set(
            int                      unit,
            bcm_gport_t              port,
            bcm_vlan_translate_key_t key_type,
            bcm_vlan_t               outer_vlan,
            bcm_vlan_t               inner_vlan,
            bcm_vlan_stat_t          stat,
            uint32                   num_entries,
            uint32                   *counter_indexes,
            bcm_stat_value_t         *counter_values)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return _bcm_esw_vlan_translate_stat_counter_set(unit,port,key_type,
                      outer_vlan,inner_vlan,stat,num_entries,
                      counter_indexes,counter_values);
    } else
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit,soc_feature_gport_service_counters)) {
        uint64 val;
        _bcm_flex_stat_handle_t vxlt_fsh;
        int rv = BCM_E_NONE;

        if (stat == bcmVlanStatIngressPackets) {
            COMPILER_64_SET(val,0,counter_values->packets);
        } else {
            COMPILER_64_SET(val,
                      COMPILER_64_HI(counter_values->bytes),
                      COMPILER_64_LO(counter_values->bytes));
        }
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_translate_stat_param_valid(unit, port, key_type,
                                                  outer_vlan, inner_vlan,
                                                  &vxlt_fsh));
        rv =  _bcm_esw_flex_stat_ext_set(unit, _bcmFlexStatTypeVxlt,
                vxlt_fsh, _bcm_esw_vlan_translate_stat_to_flex_stat(stat),val);
        return rv;

    } else
#endif
#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)) {
        return _bcm_hr3_vlan_translate_stat_counter_set(unit,port,key_type,
                      outer_vlan,inner_vlan,stat,num_entries,
                      counter_indexes,counter_values);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }

}

/*
 * Function:
 *      bcm_esw_vlan_translate_stat_id_get
 * Description:
 *      Get stat counter id associated with given ingress vlan
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      Port             - (IN) GPORT (global port) identifier  
 *      key_type         - (IN) VLAN translation key type  
 *      outer_vlan       - (IN) Outer VLAN ID or tag  
 *      inner_vlan       - (IN) Inner VLAN ID or tag  
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      Stat_counter_id  - (OUT) Stat Counter ID
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_vlan_translate_stat_id_get(
            int                      unit,
            bcm_gport_t              port,
            bcm_vlan_translate_key_t key_type,
            bcm_vlan_t               outer_vlan,
            bcm_vlan_t               inner_vlan,
            bcm_vlan_stat_t          stat,
            uint32                   *stat_counter_id)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    bcm_stat_flex_direction_t  direction=bcmStatFlexDirectionIngress;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    uint32                     index=0;
    uint32                     num_stat_counter_ids=0;
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_advanced_flex_counter)) {
        if ((stat == bcmVlanStatIngressPackets) ||
            (stat == bcmVlanStatIngressBytes)) {
             direction = bcmStatFlexDirectionIngress;
        } else {
             direction = bcmStatFlexDirectionEgress;
        }
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_translate_stat_get_table_info(
                            unit, port,key_type,outer_vlan,inner_vlan,
                            &num_of_tables,&table_info[0]));
        for (index=0; index < num_of_tables ; index++) {
             if (table_info[index].direction == direction)
                 return _bcm_esw_stat_flex_get_counter_id(
                                      unit, 1, &table_info[index],
                                      &num_stat_counter_ids,stat_counter_id);
        }
        return BCM_E_NOT_FOUND;
    } else
#endif
#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)) {
        return _bcm_hr3_vlan_translate_stat_id_get(unit, port, key_type,
                    outer_vlan, inner_vlan, stat, stat_counter_id);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      _bcm_esw_vlan_translate_stat_get
 * Purpose:
 *      Extract ingress VLAN translation statistics from the chip.
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.
 *
 * Parameters:
 *      unit        - (IN) Unit number.
 *      sync_mode   - (IN) Unit number.
 *      port        - (IN) Generic port
 *      key_type    - (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan  - (IN) Packet outer VLAN ID
 *      inner_vlan  - (IN) Packet inner VLAN ID
 *      stat        - (IN) Type of the counter to retrieve.
 *      val         - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
_bcm_esw_vlan_translate_stat_get(int unit, int sync_mode, bcm_gport_t port,
                                 bcm_vlan_translate_key_t key_type,
                                 bcm_vlan_t outer_vlan,
                                 bcm_vlan_t inner_vlan,
                                 bcm_vlan_stat_t stat,
                                 uint64 *val)
{
#if defined(BCM_TRIUMPH2_SUPPORT)|| defined(BCM_TRIDENT_SUPPORT)
    _bcm_flex_stat_handle_t vxlt_fsh;
#endif
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};
#endif

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
  
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_translate_stat_param_valid(unit, port, key_type,
                                                  outer_vlan, inner_vlan,
                                                  &vxlt_fsh));
        return _bcm_esw_flex_stat_ext_get(unit, sync_mode, _bcmFlexStatTypeVxlt,
                                      vxlt_fsh,
                           _bcm_esw_vlan_translate_stat_to_flex_stat(stat),
                                      val);
    } else
#endif
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_advanced_flex_counter) ||
        soc_feature(unit, soc_feature_flowcnt)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_vlan_xslate_stat_counter_get(unit, sync_mode, 
                                                 port, key_type,
                                                 outer_vlan,
                                                 inner_vlan, 
                                                 stat, 1, 
                                                 &counter_indexes,
                                                 &counter_values));

        if ((stat == bcmVlanStatIngressPackets) ||
            (stat == bcmVlanStatEgressPackets)) {
            COMPILER_64_SET(*val,
                         COMPILER_64_HI(counter_values.packets64),
                         COMPILER_64_LO(counter_values.packets64));
        } else {
            COMPILER_64_SET(*val,
                         COMPILER_64_HI(counter_values.bytes),
                         COMPILER_64_LO(counter_values.bytes));
        }
        return BCM_E_NONE;
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_vlan_translate_stat_get
 * Purpose:
 *      Extract ingress VLAN translation statistics from the chip.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Generic port
 *      key_type - (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_vlan_translate_stat_get(int unit, bcm_gport_t port,
                                bcm_vlan_translate_key_t key_type,
                                bcm_vlan_t outer_vlan,
                                bcm_vlan_t inner_vlan,
                                bcm_vlan_stat_t stat,
                                uint64 *val)
{
    int rv = BCM_E_NONE;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    rv = _bcm_esw_vlan_translate_stat_get(unit, 0, port, key_type, outer_vlan,
                                          inner_vlan, stat, val);

    return rv;
}

/*
 * Function:
 *      bcm_esw_vlan_translate_stat_sync_get
 * Purpose:
 *      Extract ingress VLAN translation statistics from the chip.
 *      sw accumulated counters synced with hw count.
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Generic port
 *      key_type - (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_vlan_translate_stat_sync_get(int unit, bcm_gport_t port,
                                     bcm_vlan_translate_key_t key_type,
                                     bcm_vlan_t outer_vlan,
                                     bcm_vlan_t inner_vlan,
                                     bcm_vlan_stat_t stat,
                                     uint64 *val)
{
    int rv = BCM_E_NONE;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    rv = _bcm_esw_vlan_translate_stat_get(unit, 1, port, key_type, outer_vlan,
                                          inner_vlan, stat, val);

    return rv;
}

/*
 * Function:
 *      _bcm_esw_vlan_translate_stat_get32
 * Purpose:
 *      Extract ingress VLAN translation statistics from the chip.
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.
 *
 * Parameters:
 *      unit       - (IN) Unit number.
 *      sync_mode  - (IN) hwcount is to be synced to sw count
 *      port       - (IN) Generic port
 *      key_type   - (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      stat       - (IN) Type of the counter to retrieve.
 *      val        - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
_bcm_esw_vlan_translate_stat_get32(int unit, int sync_mode, bcm_gport_t port,
                                   bcm_vlan_translate_key_t key_type,
                                   bcm_vlan_t outer_vlan,
                                   bcm_vlan_t inner_vlan,
                                   bcm_vlan_stat_t stat,
                                   uint32 *val)
{
#if defined(BCM_TRIUMPH2_SUPPORT)|| defined(BCM_TRIDENT_SUPPORT)
    _bcm_flex_stat_handle_t vxlt_fsh;
#endif
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};
#endif

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit, soc_feature_gport_service_counters)) {        
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_translate_stat_param_valid(unit, port, key_type,
                                                      outer_vlan, inner_vlan,
                                                      &vxlt_fsh));
        return _bcm_esw_flex_stat_ext_get32(unit, sync_mode, _bcmFlexStatTypeVxlt,
                                        vxlt_fsh,
                           _bcm_esw_vlan_translate_stat_to_flex_stat(stat),
                                         val);
    } else
#endif        
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_advanced_flex_counter) ||
        soc_feature(unit, soc_feature_flowcnt)) {
        BCM_IF_ERROR_RETURN(bcm_esw_vlan_xslate_stat_counter_get(unit, sync_mode, 
                                                             port, key_type,
                                                             outer_vlan,
                                                             inner_vlan, stat,
                                                             1, 
                                                             &counter_indexes, 
                                                             &counter_values));
        if ((stat == bcmVlanStatIngressPackets) ||
            (stat == bcmVlanStatEgressPackets)) {
             *val = counter_values.packets;
        } else {
             /* Ignoring Hi bytes value */
             *val = COMPILER_64_LO(counter_values.bytes);
        }
        return BCM_E_NONE;
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_vlan_translate_stat_get32
 * Purpose:
 *      Extract ingress VLAN translation statistics from the chip.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Generic port
 *      key_type - (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_vlan_translate_stat_get32(int unit, bcm_gport_t port,
                                  bcm_vlan_translate_key_t key_type,
                                  bcm_vlan_t outer_vlan,
                                  bcm_vlan_t inner_vlan,
                                  bcm_vlan_stat_t stat,
                                  uint32 *val)
{
    int rv = BCM_E_NONE;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    rv = _bcm_esw_vlan_translate_stat_get32(unit, 0, port, key_type, outer_vlan,
                                            inner_vlan, stat, val);
    return rv;
}

/*
 * Function:
 *      bcm_esw_vlan_translate_stat_sync_get32
 * Purpose:
 *      Extract ingress VLAN translation statistics from the chip.
 *      sw accumulated counters synced with hw count.
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Generic port
 *      key_type - (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_vlan_translate_stat_sync_get32(int unit, bcm_gport_t port,
                                       bcm_vlan_translate_key_t key_type,
                                       bcm_vlan_t outer_vlan,
                                       bcm_vlan_t inner_vlan,
                                       bcm_vlan_stat_t stat,
                                       uint32 *val)
{
    int rv = BCM_E_NONE;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    rv = _bcm_esw_vlan_translate_stat_get32(unit, 1, port, key_type, outer_vlan,
                                            inner_vlan, stat, val);
    return rv;
}

/*
 * Function:
 *      bcm_esw_vlan_translate_stat_set
 * Purpose:
 *      Set the specified statistic to the indicated value for the
 *      specified ingress VLAN translation.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Generic port
 *      key_type - (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      stat - (IN) Type of the counter to set.
 *      val - (IN) New counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_vlan_translate_stat_set(int unit, bcm_gport_t port,
                                bcm_vlan_translate_key_t key_type,
                                bcm_vlan_t outer_vlan,
                                bcm_vlan_t inner_vlan,
                                bcm_vlan_stat_t stat,
                                uint64 val)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    _bcm_flex_stat_handle_t vxlt_fsh;
#endif    
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_translate_stat_param_valid(unit, port, key_type,
                                                  outer_vlan, inner_vlan,
                                                  &vxlt_fsh));
        return _bcm_esw_flex_stat_ext_set(unit, _bcmFlexStatTypeVxlt,
                                      vxlt_fsh,
                           _bcm_esw_vlan_translate_stat_to_flex_stat(stat), val);
    } else
#endif    
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_advanced_flex_counter) ||
        soc_feature(unit, soc_feature_flowcnt)) {    
        if ((stat == bcmVlanStatIngressPackets) ||
            (stat == bcmVlanStatEgressPackets)) {
             counter_values.packets = COMPILER_64_LO(val);
        } else {
             COMPILER_64_SET(counter_values.bytes,
                             COMPILER_64_HI(val),
                             COMPILER_64_LO(val));
        }
        BCM_IF_ERROR_RETURN(bcm_esw_vlan_translate_stat_counter_set(
                            unit,port,key_type,outer_vlan,inner_vlan,
                            stat, 1, &counter_indexes, &counter_values));
        return BCM_E_NONE;
    } else
#endif
    {
       return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_vlan_translate_stat_set32
 * Purpose:
 *      Set the specified statistic to the indicated value for the
 *      specified ingress VLAN translation.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Generic port
 *      key_type - (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      stat - (IN) Type of the counter to set.
 *      val - (IN) New counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_vlan_translate_stat_set32(int unit, bcm_gport_t port,
                                  bcm_vlan_translate_key_t key_type,
                                  bcm_vlan_t outer_vlan,
                                  bcm_vlan_t inner_vlan,
                                  bcm_vlan_stat_t stat,
                                  uint32 val)
{
#if defined(BCM_TRIUMPH2_SUPPORT)|| defined(BCM_TRIDENT_SUPPORT)
    _bcm_flex_stat_handle_t vxlt_fsh;
#endif    
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
          
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_translate_stat_param_valid(unit, port, key_type,
                                                  outer_vlan, inner_vlan,
                                                  &vxlt_fsh));
        return _bcm_esw_flex_stat_ext_set32(unit, _bcmFlexStatTypeVxlt,
                                        vxlt_fsh,
                           _bcm_esw_vlan_translate_stat_to_flex_stat(stat), val);
    } else 
#endif    
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_advanced_flex_counter) ||
        soc_feature(unit, soc_feature_flowcnt)) {
        if ((stat == bcmVlanStatIngressPackets) ||
            (stat == bcmVlanStatEgressPackets)) {
            counter_values.packets = val;
        } else {
            /* Ignoring high value */
            COMPILER_64_SET(counter_values.bytes,0,val);
        }
        BCM_IF_ERROR_RETURN(bcm_esw_vlan_translate_stat_counter_set(
                        unit,port,key_type,outer_vlan,inner_vlan,
                        stat, 1, &counter_indexes, &counter_values));
        return BCM_E_NONE;
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_vlan_translate_stat_multi_get
 * Purpose:
 *      Get 64-bit counter value for multiple ingress VLAN translation
 *      statistic types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Generic port
 *      key_type - (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (OUT) Collected counters values
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_vlan_translate_stat_multi_get(int unit, bcm_gport_t port,
                                      bcm_vlan_translate_key_t key_type,
                                      bcm_vlan_t outer_vlan,
                                      bcm_vlan_t inner_vlan,
                                      int nstat,
                                      bcm_vlan_stat_t *stat_arr,
                                      uint64 *value_arr)
{
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    _bcm_flex_stat_handle_t vxlt_fsh;
    _bcm_flex_stat_t fs_arr[_bcmFlexStatNum]; /* Normalize stats */
    int idx;
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_translate_stat_param_valid(unit, port, key_type,
                                                  outer_vlan, inner_vlan,
                                                  &vxlt_fsh));
        BCM_IF_ERROR_RETURN
            (_bcm_vlan_stat_array_convert(unit, nstat, stat_arr,
                   fs_arr, &_bcm_esw_vlan_translate_stat_to_flex_stat));
        BCM_VLAN_VALUE_ARRAY_VALID(unit, nstat, value_arr);

        return _bcm_esw_flex_stat_ext_multi_get(unit, _bcmFlexStatTypeVxlt,
                                            vxlt_fsh,
                                        nstat, fs_arr, value_arr);
    } else 
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_advanced_flex_counter) ||
        soc_feature(unit, soc_feature_flowcnt)) {
        for (idx=0;idx < nstat ; idx ++) {
             BCM_IF_ERROR_RETURN(bcm_esw_vlan_translate_stat_counter_get(
                             unit,port,key_type,outer_vlan,inner_vlan,
                             stat_arr[idx],1,&counter_indexes,&counter_values));
            if ((stat_arr[idx] == bcmVlanStatIngressPackets) ||
                (stat_arr[idx] == bcmVlanStatEgressPackets)) {
                COMPILER_64_SET(value_arr[idx],
                              COMPILER_64_HI(counter_values.packets64),
                              COMPILER_64_LO(counter_values.packets64));
            } else {
                COMPILER_64_SET(value_arr[idx],
                              COMPILER_64_HI(counter_values.bytes),
                              COMPILER_64_LO(counter_values.bytes));
            }
        }
        return BCM_E_NONE;
    } else
#endif
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_vlan_translate_stat_multi_get32
 * Purpose:
 *      Get lower 32-bit counter value for multiple ingress VLAN
 *      translation statistic types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Generic port
 *      key_type - (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (OUT) Collected counters values
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_vlan_translate_stat_multi_get32(int unit, bcm_gport_t port,
                                        bcm_vlan_translate_key_t key_type,
                                        bcm_vlan_t outer_vlan,
                                        bcm_vlan_t inner_vlan,
                                        int nstat,
                                        bcm_vlan_stat_t *stat_arr,
                                        uint32 *value_arr)
{
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    _bcm_flex_stat_handle_t vxlt_fsh;
    _bcm_flex_stat_t fs_arr[_bcmFlexStatNum]; /* Normalize stats */
    int idx;
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_translate_stat_param_valid(unit, port, key_type,
                                                  outer_vlan, inner_vlan,
                                                  &vxlt_fsh));
        BCM_IF_ERROR_RETURN
            (_bcm_vlan_stat_array_convert(unit, nstat, stat_arr,
                   fs_arr, &_bcm_esw_vlan_translate_stat_to_flex_stat));
        BCM_VLAN_VALUE_ARRAY_VALID(unit, nstat, value_arr);

        return _bcm_esw_flex_stat_ext_multi_get32(unit, _bcmFlexStatTypeVxlt,
                                              vxlt_fsh,
                                        nstat, fs_arr, value_arr);
    } else
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_advanced_flex_counter) ||
        soc_feature(unit, soc_feature_flowcnt)) {
        for (idx=0;idx < nstat ; idx ++) {
             BCM_IF_ERROR_RETURN(bcm_esw_vlan_translate_stat_counter_get(
                                 unit,port,key_type,outer_vlan,inner_vlan,
                                 stat_arr[idx],1,&counter_indexes,&counter_values));
             if ((stat_arr[idx] == bcmVlanStatIngressPackets) ||
                 (stat_arr[idx] == bcmVlanStatEgressPackets)) {
                  value_arr[idx] = counter_values.packets;
             } else {
                  value_arr[idx] = COMPILER_64_LO(counter_values.bytes);
             }
        }
        return BCM_E_NONE;
    }
#endif
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_vlan_translate_stat_multi_set
 * Purpose:
 *      Set 64-bit counter value for multiple ingress VLAN translation
 *      statistic types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Generic port
 *      key_type - (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) New statistics descriptors array
 *      value_arr - (IN) New counters values
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_vlan_translate_stat_multi_set(int unit, bcm_gport_t port,
                                      bcm_vlan_translate_key_t key_type,
                                      bcm_vlan_t outer_vlan,
                                      bcm_vlan_t inner_vlan,
                                      int nstat,
                                      bcm_vlan_stat_t *stat_arr,
                                      uint64 *value_arr)
{
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    _bcm_flex_stat_handle_t vxlt_fsh;
    _bcm_flex_stat_t fs_arr[_bcmFlexStatNum]; /* Normalize stats */
    int idx;
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    if (soc_feature(unit, soc_feature_gport_service_counters)) {      
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_translate_stat_param_valid(unit, port, key_type,
                                                      outer_vlan, inner_vlan,
                                                      &vxlt_fsh));
        BCM_IF_ERROR_RETURN
            (_bcm_vlan_stat_array_convert(unit, nstat, stat_arr,
                       fs_arr, &_bcm_esw_vlan_translate_stat_to_flex_stat));
        BCM_VLAN_VALUE_ARRAY_VALID(unit, nstat, value_arr);
    
        return _bcm_esw_flex_stat_ext_multi_set(unit, _bcmFlexStatTypeVxlt,
                                                vxlt_fsh,
                                            nstat, fs_arr, value_arr);
    } else    
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_advanced_flex_counter) ||
        soc_feature(unit, soc_feature_flowcnt)) {    
        for (idx=0;idx < nstat ; idx ++) {
             if ((stat_arr[idx] == bcmVlanStatIngressPackets) ||
                 (stat_arr[idx] == bcmVlanStatEgressPackets)) {
                  counter_values.packets = COMPILER_64_LO(value_arr[idx]);
             } else {
                  COMPILER_64_SET(counter_values.bytes,
                                  COMPILER_64_HI(value_arr[idx]),
                                  COMPILER_64_LO(value_arr[idx]));
             }
             BCM_IF_ERROR_RETURN(bcm_esw_vlan_translate_stat_counter_set(
                                 unit,port,key_type,outer_vlan,inner_vlan,
                                 stat_arr[idx],1,&counter_indexes,&counter_values));
        }
        return BCM_E_NONE;
    } else
#endif
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_vlan_translate_stat_multi_set32
 * Purpose:
 *      Set lower 32-bit counter value for multiple ingress VLAN
 *      translation statistic types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Generic port
 *      key_type - (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) New statistics descriptors array
 *      value_arr - (IN) New counters values
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_vlan_translate_stat_multi_set32(int unit, bcm_gport_t port, 
                                        bcm_vlan_translate_key_t key_type,
                                        bcm_vlan_t outer_vlan,
                                        bcm_vlan_t inner_vlan,
                                        int nstat,
                                        bcm_vlan_stat_t *stat_arr,
                                        uint32 *value_arr)
{
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    _bcm_flex_stat_handle_t vxlt_fsh;
    _bcm_flex_stat_t fs_arr[_bcmFlexStatNum]; /* Normalize stats */
    int idx;
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    if (soc_feature(unit, soc_feature_gport_service_counters)) {      
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_translate_stat_param_valid(unit, port, key_type,
                                                      outer_vlan, inner_vlan,
                                                      &vxlt_fsh));
        BCM_IF_ERROR_RETURN
            (_bcm_vlan_stat_array_convert(unit, nstat, stat_arr,
                       fs_arr, &_bcm_esw_vlan_translate_stat_to_flex_stat));
        BCM_VLAN_VALUE_ARRAY_VALID(unit, nstat, value_arr);
    
        return _bcm_esw_flex_stat_ext_multi_set32(unit, _bcmFlexStatTypeVxlt,
                                                  vxlt_fsh,
                                            nstat, fs_arr, value_arr);
    } else
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_advanced_flex_counter) ||
        soc_feature(unit, soc_feature_flowcnt)) {    
        for (idx=0;idx < nstat ; idx ++) {
             if ((stat_arr[idx] == bcmVlanStatIngressPackets) ||
                 (stat_arr[idx] == bcmVlanStatEgressPackets)) {
                  counter_values.packets = value_arr[idx];
             } else {
                  COMPILER_64_SET(counter_values.bytes,0,value_arr[idx]);
             }
             BCM_IF_ERROR_RETURN(bcm_esw_vlan_translate_stat_counter_set(
                                 unit,port,key_type,outer_vlan,inner_vlan,
                                 stat_arr[idx],1,&counter_indexes,&counter_values));
        }
        return BCM_E_NONE;
    } else
#endif
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

#if defined(BCM_TRIUMPH2_SUPPORT)
STATIC int
_bcm_esw_vlan_translate_egress_flex_stat_hw_index_set(int unit,
                                     _bcm_flex_stat_handle_t handle,
                                               int fs_idx, void *cookie)
{
    egr_vlan_xlate_entry_t  vent, res_vent;
    int rv, idx;
    
    _BCM_FLEX_STAT_HANDLE_COPY(vent, handle);

    soc_mem_lock(unit, EGR_VLAN_XLATEm);
    rv = soc_mem_search(unit, EGR_VLAN_XLATEm, MEM_BLOCK_ALL, &idx, 
                         &vent, &res_vent, 0);
    if (BCM_SUCCESS(rv)) {
        soc_mem_field32_set(unit, EGR_VLAN_XLATEm, &res_vent,
                            VINTF_CTR_IDXf, fs_idx);
        if (SOC_MEM_FIELD_VALID(unit, EGR_VLAN_XLATEm, USE_VINTF_CTR_IDXf)) {
            soc_mem_field32_set(unit, EGR_VLAN_XLATEm, &res_vent,
                                USE_VINTF_CTR_IDXf, fs_idx > 0 ? 1 : 0);
        }
        rv = soc_mem_insert(unit, EGR_VLAN_XLATEm, MEM_BLOCK_ALL, &res_vent);
        if (BCM_SUCCESS(rv)) {
            /* We found it already, but didn't overwrite it?  The mem lock
             * must have failed.  Not good. */
            return BCM_E_INTERNAL;
        } else if (BCM_E_EXISTS == rv) {
            rv = BCM_E_NONE;
        } /* Else retain failing rv value */
    }
    soc_mem_unlock(unit, EGR_VLAN_XLATEm);
    return rv;
}

STATIC _bcm_flex_stat_t
_bcm_esw_vlan_translate_egress_stat_to_flex_stat(bcm_vlan_stat_t stat)
{
    _bcm_flex_stat_t flex_stat;

    /* No ingress stats for egress translations */
    switch (stat) {
    case bcmVlanStatEgressPackets:
        flex_stat = _bcmFlexStatEgressPackets;
        break;
    case bcmVlanStatEgressBytes:
        flex_stat = _bcmFlexStatEgressBytes;
        break;
    default:
        flex_stat = _bcmFlexStatNum;
    }

    return flex_stat;
}

STATIC int
_bcm_esw_vlan_translate_egress_stat_param_valid(int unit, int port_class, 
                                                bcm_vlan_t outer_vlan,
                                                bcm_vlan_t inner_vlan,
                                         _bcm_flex_stat_handle_t *vxlt_fsh)
{
    egr_vlan_xlate_entry_t  vent;
    bcm_module_t        modid;
    bcm_trunk_t         tgid = -1;
    int                 id = -1; 

    CHECK_INIT(unit);

    if (!soc_feature(unit, soc_feature_gport_service_counters)) {
        return BCM_E_UNAVAIL;
    }

    if (BCM_GPORT_IS_SET(port_class)) {
        if (!BCM_GPORT_IS_TUNNEL(port_class)) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port_class, &modid, &port_class, 
                                       &tgid, &id));
        }
        if ((-1 != tgid || -1 != id) && !BCM_GPORT_IS_TUNNEL(port_class)) {
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_CLASS_VALID(unit, port_class)) { 
            return BCM_E_PORT; 
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_translate_egress_entry_assemble(unit, &vent,
                                                       port_class,
                                                       outer_vlan,
                                                       inner_vlan,
                                                       0));

    /* Fill handle for VTS info */
    _BCM_FLEX_STAT_HANDLE_CLEAR(*vxlt_fsh);
    /* This next step is a bit abusive, but we don't have an accessor
     * which captures the full key description in the VLAN_XLATE table. */
    _BCM_FLEX_STAT_HANDLE_COPY(*vxlt_fsh, vent);

    return BCM_E_NONE;
}

#endif /* BCM_TRIUMPH2_SUPPORT */

/*
 * Function:
 *      bcm_esw_vlan_translate_egress_stat_enable_set
 * Purpose:
 *      Enable/disable collection of statistics on the indicated
 *      egress VLAN translation.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port_class - (IN) Group ID of ingress port
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      enable - (IN) Non-zero to enable counter collection, zero to disable.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_vlan_translate_egress_stat_enable_set(int unit, int port_class, 
                                              bcm_vlan_t outer_vlan,
                                              bcm_vlan_t inner_vlan,
                                              int enable)
{
#if defined(BCM_TRIUMPH2_SUPPORT)|| defined(BCM_TRIDENT_SUPPORT)
    _bcm_flex_stat_handle_t vxlt_fsh;
#endif    
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    uint32                     num_of_tables=0;
    uint32                     num_stat_counter_ids=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    bcm_stat_object_t          object=bcmStatObjectIngPort;
    uint32                     stat_counter_id[
                                       BCM_STAT_FLEX_COUNTER_MAX_DIRECTION]={0};
    uint32                     num_entries=0;
    int                        index=0;
#endif
 
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit,soc_feature_gport_service_counters)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_translate_egress_stat_param_valid(unit, port_class,
                                                      outer_vlan, inner_vlan,
                                                      &vxlt_fsh));
        return _bcm_esw_flex_stat_ext_enable_set(unit, _bcmFlexStatTypeVxlt,
                        _bcm_esw_vlan_translate_egress_flex_stat_hw_index_set,
                                                 NULL, vxlt_fsh, enable,0);
    } else
#endif        
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_translate_egress_stat_get_table_info(
                            unit,port_class,
                            outer_vlan,inner_vlan,
                            &num_of_tables,&table_info[0]));
        if (enable) {
            for(index=0;index < num_of_tables ;index++) {
                if(table_info[index].direction == bcmStatFlexDirectionIngress) {
                   BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_ingress_object(
                                       unit,table_info[index].table,
                                       table_info[index].index,NULL,&object));
                } else {
                    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_egress_object(
                                        unit,table_info[index].table,
                                        table_info[index].index,NULL,&object));
                }
                BCM_IF_ERROR_RETURN(bcm_esw_stat_group_create(
                                    unit,object,bcmStatGroupModeSingle,
                                    &stat_counter_id[table_info[index].direction],
                                    &num_entries));
                BCM_IF_ERROR_RETURN(bcm_esw_vlan_translate_egress_stat_attach(
                                    unit,port_class,outer_vlan,inner_vlan,
                                    stat_counter_id[table_info[index].direction]));
            }
            return BCM_E_NONE;
        } else {
            BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_counter_id(
                                unit, num_of_tables,&table_info[0],
                                &num_stat_counter_ids,&stat_counter_id[0]));
            if ((stat_counter_id[bcmStatFlexDirectionIngress] == 0) &&
                (stat_counter_id[bcmStatFlexDirectionEgress] == 0)) {
                 return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN(bcm_esw_vlan_translate_egress_stat_detach(
                                unit,port_class,outer_vlan,inner_vlan));
            if (stat_counter_id[bcmStatFlexDirectionIngress] != 0) {
                BCM_IF_ERROR_RETURN(bcm_esw_stat_group_destroy(
                                    unit,
                                    stat_counter_id[bcmStatFlexDirectionIngress]));
            }
            if (stat_counter_id[bcmStatFlexDirectionEgress] != 0) {
                BCM_IF_ERROR_RETURN(bcm_esw_stat_group_destroy(
                                    unit,
                                    stat_counter_id[bcmStatFlexDirectionEgress]));
            }
            return BCM_E_NONE;
        }
    } else
#endif
#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)) {
        return _bcm_hr3_vlan_translate_egress_stat_enable_set(unit, port_class,
                   outer_vlan, inner_vlan, enable);

    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }}
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
/*
 * Function:
 *      _bcm_esw_vlan_translate_egress_stat_get_table_info
 * Description:
 *      Provides relevant flex table information(table-name,index with
 *      direction)  for given egress vlan translation
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port_class       - (IN)    Port class 
 *      outer_vlan       - (IN)    Outer VLAN ID 
 *      inner_vlan       - (IN)    Inner VLAN ID  
 *      num_of_tables    - (OUT) Number of flex counter tables
 *      table_info       - (OUT) Flex counter tables information
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
static 
bcm_error_t _bcm_esw_vlan_translate_egress_stat_get_table_info(
            int                        unit, 
            int                        port_class,
            bcm_vlan_t                 outer_vlan,
            bcm_vlan_t                 inner_vlan,
            uint32                     *num_of_tables,
            bcm_stat_flex_table_info_t *table_info)
{
    soc_mem_t               mem = EGR_VLAN_XLATEm;
    bcm_error_t            rv=BCM_E_NONE;
    uint32                  vent[SOC_MAX_MEM_FIELD_WORDS];
    uint32                  return_vent[SOC_MAX_MEM_FIELD_WORDS];
    int                    index=0;
    bcm_module_t           modid=0;
    bcm_trunk_t            tgid = -1;
    int                    id = -1;
    int                    handle_modport_gport = 0;

    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
        mem = EGR_VLAN_XLATE_1_DOUBLEm;
    }
    if (soc_feature(unit, soc_feature_egr_vxlate_supports_dgpp)) {
        handle_modport_gport = 1;
    }

    (*num_of_tables) = 0;

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }
    if (BCM_GPORT_IS_SET(port_class)) {
        if (!BCM_GPORT_IS_TUNNEL(port_class) &&
            !BCM_GPORT_IS_VLAN_PORT(port_class) &&
            !BCM_GPORT_IS_MIM_PORT(port_class) &&
            !BCM_GPORT_IS_NIV_PORT(port_class) &&
            !BCM_GPORT_IS_MPLS_PORT(port_class) &&
            !BCM_GPORT_IS_VXLAN_PORT(port_class) &&
            !BCM_GPORT_IS_EXTENDER_PORT(port_class) &&
            !BCM_GPORT_IS_L2GRE_PORT(port_class) &&
            !(handle_modport_gport &&
              (BCM_GPORT_IS_MODPORT(port_class) ||
              BCM_GPORT_IS_SUBPORT_PORT(port_class)))) {
            BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(
                                unit, port_class, &modid, 
                                &port_class,&tgid, &id));
        }
        if ((-1 != tgid || -1 != id) &&
            !BCM_GPORT_IS_TUNNEL(port_class) &&
            !BCM_GPORT_IS_VLAN_PORT(port_class) &&
            !BCM_GPORT_IS_MIM_PORT(port_class) &&
            !BCM_GPORT_IS_NIV_PORT(port_class) &&
            !BCM_GPORT_IS_MPLS_PORT(port_class) &&
            !BCM_GPORT_IS_VXLAN_PORT(port_class) &&
            !BCM_GPORT_IS_EXTENDER_PORT(port_class) &&
            !BCM_GPORT_IS_L2GRE_PORT(port_class) ) {
            return BCM_E_PORT;
        }
    } else  {
        if (!SOC_PORT_CLASS_VALID(unit, port_class)) {
            return BCM_E_PORT;
        }
    }
    sal_memset(vent, 0, sizeof(vent));
    sal_memset(return_vent, 0, sizeof(return_vent));
    BCM_IF_ERROR_RETURN (_bcm_trx_vlan_translate_egress_entry_assemble(
                         unit, vent, port_class, outer_vlan, inner_vlan, 0));
    soc_mem_lock(unit, mem);
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL, &index, vent, return_vent, 0);
    if (BCM_SUCCESS(rv)) {
        table_info[*num_of_tables].table=mem;
        table_info[*num_of_tables].index=index;
        table_info[*num_of_tables].direction=bcmStatFlexDirectionEgress;
        (*num_of_tables)++;
    }
    soc_mem_unlock(unit, mem);
    return rv;
}
#endif

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
/*
 * Function:
 *      _bcm_esw_vlan_translate_egress_stat_attach
 * Description:
 *      Attach counters for the given egress vlan translation table
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port_class       - (IN)    Port class 
 *      outer_vlan       - (IN)    Outer VLAN ID 
 *      inner_vlan       - (IN)    Inner VLAN ID  
 *      Stat_counter_id  -  (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC bcm_error_t _bcm_esw_vlan_translate_egress_stat_attach(
            int        unit,
            int        port_class,
            bcm_vlan_t outer_vlan,
            bcm_vlan_t inner_vlan,
            uint32     stat_counter_id)
{
    soc_mem_t                  table=0;
    bcm_stat_flex_direction_t  direction=bcmStatFlexDirectionIngress;
    uint32                     pool_number=0;
    uint32                     base_index=0;
    bcm_stat_flex_mode_t       offset_mode=0;
    bcm_stat_object_t          object=bcmStatObjectIngPort;
    bcm_stat_group_mode_t      group_mode= bcmStatGroupModeSingle;
    uint32                     count=0;
    uint32                     actual_num_tables=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    _bcm_esw_stat_get_counter_id_info(
                  unit, stat_counter_id,
                  &group_mode,&object,&offset_mode,&pool_number,&base_index);
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_object(unit,object,&direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_group(unit,group_mode));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_table_info(
                        unit,object,1,&actual_num_tables,&table,&direction));

    BCM_IF_ERROR_RETURN(_bcm_esw_vlan_translate_egress_stat_get_table_info(
                        unit, port_class,outer_vlan,inner_vlan,
                        &num_of_tables,&table_info[0]));
    for (count=0; count < num_of_tables ; count++) {
         if ((table_info[count].direction == direction) &&
             (table_info[count].table == table) ) {
              if (direction == bcmStatFlexDirectionIngress) {
                  return _bcm_esw_stat_flex_attach_ingress_table_counters(
                         unit, table_info[count].table, table_info[count].index,
                         offset_mode, base_index, pool_number);
              } else {
                  return _bcm_esw_stat_flex_attach_egress_table_counters(
                         unit, table_info[count].table, table_info[count].index,
                         0, offset_mode, base_index, pool_number);
              } 
         }
    }
    return BCM_E_NOT_FOUND;
}
#endif

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
STATIC bcm_error_t _tr2_vlan_translate_egress_stat_attach(
            int        unit,
            int        port_class,
            bcm_vlan_t outer_vlan,
            bcm_vlan_t inner_vlan,
            uint32     stat_counter_id,
            int        attached)
{
    _bcm_flex_stat_type_t fs_type;
    _bcm_flex_stat_handle_t vxlt_fsh;
    uint32 fs_inx;

    fs_type = _BCM_FLEX_STAT_TYPE(stat_counter_id);
    fs_inx  = _BCM_FLEX_STAT_COUNT_INX(stat_counter_id);

    if (!((fs_type == _bcmFlexStatTypeEgrVxlt) && fs_inx)) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_translate_egress_stat_param_valid(unit, port_class,
                                                  outer_vlan, inner_vlan,
                                                  &vxlt_fsh));
    return _bcm_esw_flex_stat_ext_enable_set(unit, _bcmFlexStatTypeEgrVxlt,
                    _bcm_esw_vlan_translate_egress_flex_stat_hw_index_set,
                                        INT_TO_PTR(_BCM_FLEX_STAT_HW_EGRESS),
                                            vxlt_fsh, attached,fs_inx);
}
#endif

/*
 * Function:
 *      bcm_esw_vlan_translate_egress_stat_attach
 * Description:
 *      Attach counters for the given egress vlan translation table
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port_class       - (IN) Port class
 *      outer_vlan       - (IN) Outer VLAN ID
 *      inner_vlan       - (IN) Inner VLAN ID
 *      Stat_counter_id  -  (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_vlan_translate_egress_stat_attach(
            int        unit,
            int        port_class,
            bcm_vlan_t outer_vlan,
            bcm_vlan_t inner_vlan,
            uint32     stat_counter_id)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return _bcm_esw_vlan_translate_egress_stat_attach(unit,
                port_class,outer_vlan,inner_vlan,stat_counter_id);
    } else
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit,soc_feature_gport_service_counters)) {
        return _tr2_vlan_translate_egress_stat_attach(unit,
                port_class,outer_vlan,inner_vlan,stat_counter_id,TRUE);
    } else
#endif
#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)) {
        return _bcm_hr3_vlan_translate_egress_stat_attach(unit,
                port_class,outer_vlan,inner_vlan,stat_counter_id);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
/*
 * Function:
 *      _bcm_esw_vlan_translate_egress_stat_detach
 * Description:
 *      Detach counters for the given egress vlan translation table
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port_class       - (IN)    Port class 
 *      outer_vlan       - (IN)    Outer VLAN ID 
 *      inner_vlan       - (IN)    Inner VLAN ID  
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC bcm_error_t _bcm_esw_vlan_translate_egress_stat_detach(
            int unit,
            int port_class,
            bcm_vlan_t outer_vlan,
            bcm_vlan_t inner_vlan)
{
    uint32                     count=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    bcm_error_t                rv = BCM_E_NONE;
    bcm_error_t                err_code[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION] = {BCM_E_NONE};

    BCM_IF_ERROR_RETURN(_bcm_esw_vlan_translate_egress_stat_get_table_info(
                        unit, port_class,outer_vlan,inner_vlan,
                        &num_of_tables,&table_info[0]));

    for (count=0; count < num_of_tables ; count++) {
         if (table_info[count].direction == bcmStatFlexDirectionIngress) {
             rv = _bcm_esw_stat_flex_detach_ingress_table_counters(
                        unit, table_info[count].table, table_info[count].index);
             if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionIngress]) {
                err_code[bcmStatFlexDirectionIngress] = rv;
             }
         } else {
             rv = _bcm_esw_stat_flex_detach_egress_table_counters(
                        unit, 0, table_info[count].table, table_info[count].index);
             if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionEgress]) {
                err_code[bcmStatFlexDirectionEgress] = rv;
             }
         }
    }
    
    BCM_STAT_FLEX_DETACH_RETURN(err_code)
}
#endif

/*
 * Function:
 *      bcm_esw_vlan_translate_egress_stat_detach
 * Description:
 *      Detach counters for the given egress vlan translation table
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port_class       - (IN) Port class
 *      outer_vlan       - (IN) Outer VLAN ID
 *      inner_vlan       - (IN) Inner VLAN ID
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_vlan_translate_egress_stat_detach(
            int unit,
            int port_class,
            bcm_vlan_t outer_vlan,
            bcm_vlan_t inner_vlan)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return _bcm_esw_vlan_translate_egress_stat_detach(unit,
                port_class,outer_vlan,inner_vlan);
    } else
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit,soc_feature_gport_service_counters)) {
        return _tr2_vlan_translate_egress_stat_attach(unit,
                port_class,outer_vlan,inner_vlan,
                _BCM_FLEX_STAT_COUNT_ID(_bcmFlexStatTypeEgrVxlt,1),FALSE);
    } else
#endif
#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)) {
        return _bcm_hr3_vlan_translate_egress_stat_detach(unit,
                port_class,outer_vlan,inner_vlan);
    } else
#endif 
    {
        return BCM_E_UNAVAIL;
    }

}

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
/*
 * Function:
 *      _bcm_esw_vlan_translate_egress_stat_counter_get
 * Description:
 *      Get counter values from egress vlan translation table
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) hwcount is to be synced to sw count 
 *      port_class       - (IN)    Port class 
 *      outer_vlan       - (IN)    Outer VLAN ID 
 *      inner_vlan       - (IN)    Inner VLAN ID  
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC bcm_error_t _bcm_esw_vlan_translate_egress_stat_counter_get(
            int              unit,
            int              sync_mode,
            int              port_class,
            bcm_vlan_t       outer_vlan,
            bcm_vlan_t       inner_vlan,
            bcm_vlan_stat_t  stat,
            uint32           num_entries,
            uint32           *counter_indexes,
            bcm_stat_value_t *counter_values)
{
    uint32                     table_count=0;
    uint32                     index_count=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_direction_t  direction=bcmStatFlexDirectionIngress;
    uint32                     byte_flag=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    if ((stat == bcmVlanStatIngressPackets) ||
        (stat == bcmVlanStatIngressBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress;
    }
    if ((stat == bcmVlanStatIngressPackets) ||
        (stat == bcmVlanStatEgressPackets)) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }
    
    BCM_IF_ERROR_RETURN(_bcm_esw_vlan_translate_egress_stat_get_table_info(
                        unit, port_class,outer_vlan,inner_vlan,
                        &num_of_tables,&table_info[0]));

    for (table_count=0; table_count < num_of_tables ; table_count++) {
         if (table_info[table_count].direction == direction) {
             for (index_count=0; index_count < num_entries ; index_count++) {
                  BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_get(
                                      unit, sync_mode, 
                                      table_info[table_count].index,
                                      table_info[table_count].table, 0,
                                      byte_flag,
                                      counter_indexes[index_count],
                                      &counter_values[index_count]));
             }
         }
    }
    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *      _bcm_esw_vlan_xslate_egress_stat_counter_get
 * Description:
 *      Get counter values from egress vlan translation table
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) hwcount is to be synced to sw count 
 *      port_cLass       - (IN) Port class
 *      outer_vlan       - (IN) Outer VLAN ID
 *      inner_vlan       - (IN) Inner VLAN ID
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int  
_bcm_esw_vlan_xslate_egress_stat_counter_get(
                                            int              unit,
                                            int              sync_mode,
                                            int              port_class,
                                            bcm_vlan_t       outer_vlan,
                                            bcm_vlan_t       inner_vlan,
                                            bcm_vlan_stat_t  stat,
                                            uint32           num_entries,
                                            uint32           *counter_indexes,
                                            bcm_stat_value_t *counter_values)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return _bcm_esw_vlan_translate_egress_stat_counter_get(unit, sync_mode,
                port_class,outer_vlan,inner_vlan,stat,
                      num_entries,counter_indexes,counter_values);
    } else
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit,soc_feature_gport_service_counters)) {
        uint64 val;
        _bcm_flex_stat_handle_t vxlt_fsh;
        int rv = BCM_E_NONE;

        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_translate_egress_stat_param_valid(unit, port_class,
                                         outer_vlan, inner_vlan, &vxlt_fsh)); 

        rv =  _bcm_esw_flex_stat_ext_get(unit, 0, _bcmFlexStatTypeEgrVxlt,
                vxlt_fsh,
                _bcm_esw_vlan_translate_egress_stat_to_flex_stat(stat), &val);

        if (stat == bcmVlanStatEgressPackets) {
            counter_values->packets = COMPILER_64_LO(val);
        } else {
            COMPILER_64_SET(counter_values->bytes,
                      COMPILER_64_HI(val),
                      COMPILER_64_LO(val));
        }
        return rv;

    } else
#endif
#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)) {
        return _bcm_hr3_vlan_translate_egress_stat_counter_get(unit, sync_mode,
                    port_class, outer_vlan, inner_vlan, stat, num_entries,
                    counter_indexes, counter_values);
    } else
#endif 
    {
        return BCM_E_UNAVAIL;
    }
}


/*
 * Function:
 *      bcm_esw_vlan_translate_egress_stat_counter_get
 * Description:
 *      Get counter values from egress vlan translation table
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port_class       - (IN) Port class
 *      outer_vlan       - (IN) Outer VLAN ID
 *      inner_vlan       - (IN) Inner VLAN ID
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_vlan_translate_egress_stat_counter_get(
                                              int              unit,
                                              int              port_class,
                                              bcm_vlan_t       outer_vlan,
                                              bcm_vlan_t       inner_vlan,
                                              bcm_vlan_stat_t  stat,
                                              uint32           num_entries,
                                              uint32           *counter_indexes,
                                              bcm_stat_value_t *counter_values)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
    return _bcm_esw_vlan_xslate_egress_stat_counter_get(unit, 0, port_class,
                                                     outer_vlan, inner_vlan,
                                                     stat, num_entries,
                                                     counter_indexes,
                                                     counter_values);      
}


/*
 * Function:
 *      bcm_esw_vlan_translate_egress_stat_counter_sync_get
 * Description:
 *      Get counter values from egress vlan translation table
 *      sw accumulated counters synced with hw count.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port_class       - (IN) Port class
 *      outer_vlan       - (IN) Outer VLAN ID
 *      inner_vlan       - (IN) Inner VLAN ID
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_vlan_translate_egress_stat_counter_sync_get(
                                            int              unit,
                                            int              port_class,
                                            bcm_vlan_t       outer_vlan,
                                            bcm_vlan_t       inner_vlan,
                                            bcm_vlan_stat_t  stat,
                                            uint32           num_entries,
                                            uint32           *counter_indexes,
                                            bcm_stat_value_t *counter_values)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
    return _bcm_esw_vlan_xslate_egress_stat_counter_get(unit, 1, port_class,
                                                     outer_vlan, inner_vlan,
                                                     stat, num_entries,
                                                     counter_indexes,
                                                     counter_values);      
}

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
/*
 * Function:
 *      _bcm_esw_vlan_translate_egress_stat_counter_set
 * Description:
 *      Set counter values from egress vlan translation table . 
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port_class       - (IN)    Port class 
 *      outer_vlan       - (IN)    Outer VLAN ID 
 *      inner_vlan       - (IN)    Inner VLAN ID  
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (IN) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC bcm_error_t _bcm_esw_vlan_translate_egress_stat_counter_set(
            int              unit,
            int              port_class,
            bcm_vlan_t       outer_vlan,
            bcm_vlan_t       inner_vlan,
            bcm_vlan_stat_t  stat,
            uint32           num_entries,
            uint32           *counter_indexes,
            bcm_stat_value_t *counter_values)
{
    uint32                     table_count=0;
    uint32                     index_count=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_direction_t  direction=bcmStatFlexDirectionIngress;
    uint32                     byte_flag=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    if ((stat == bcmVlanStatIngressPackets) ||
        (stat == bcmVlanStatIngressBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress;
    }
    if ((stat == bcmVlanStatIngressPackets) ||
        (stat == bcmVlanStatEgressPackets)) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }
    
    BCM_IF_ERROR_RETURN(_bcm_esw_vlan_translate_egress_stat_get_table_info(
                        unit, port_class,outer_vlan,inner_vlan,
                        &num_of_tables,&table_info[0]));

    for (table_count=0; table_count < num_of_tables ; table_count++) {
         if (table_info[table_count].direction == direction) {
             for (index_count=0; index_count < num_entries ; index_count++) {
                  BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_set(
                                      unit, table_info[table_count].index,
                                      table_info[table_count].table, 0,
                                      byte_flag,
                                      counter_indexes[index_count],
                                      &counter_values[index_count]));
             }
         }
    }
    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *      bcm_esw_vlan_translate_egress_stat_counter_set
 * Description:
 *      Set counter values from egress vlan translation table .
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port_class       - (IN) Port class
 *      outer_vlan       - (IN) Outer VLAN ID
 *      inner_vlan       - (IN) Inner VLAN ID
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (IN) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_vlan_translate_egress_stat_counter_set(
            int              unit,
            int              port_class,
            bcm_vlan_t       outer_vlan,
            bcm_vlan_t       inner_vlan,
            bcm_vlan_stat_t  stat,
            uint32           num_entries,
            uint32           *counter_indexes,
            bcm_stat_value_t *counter_values)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return _bcm_esw_vlan_translate_egress_stat_counter_set(unit,
                port_class,outer_vlan,inner_vlan,stat,
                      num_entries,counter_indexes,counter_values);
    } else
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit,soc_feature_gport_service_counters)) {
        uint64 val;
        _bcm_flex_stat_handle_t vxlt_fsh;
        int rv = BCM_E_NONE;

        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_translate_egress_stat_param_valid(unit, port_class,
                                         outer_vlan, inner_vlan, &vxlt_fsh));
        if (stat == bcmVlanStatEgressPackets) {
            COMPILER_64_SET(val,0,counter_values->packets);
        } else {
            COMPILER_64_SET(val,
                      COMPILER_64_HI(counter_values->bytes),
                      COMPILER_64_LO(counter_values->bytes));
        }

        rv =  _bcm_esw_flex_stat_ext_set(unit,_bcmFlexStatTypeEgrVxlt,
                vxlt_fsh,
                _bcm_esw_vlan_translate_egress_stat_to_flex_stat(stat), val);

        return rv;

    } else
#endif
#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)) {
        return _bcm_hr3_vlan_translate_egress_stat_counter_set(unit,
                port_class,outer_vlan,inner_vlan,stat,
                      num_entries,counter_indexes,counter_values);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_vlan_translate_egress_stat_id_get
 * Description:
 *      Get stat counter id associated with given egress vlan
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port_class       - (IN)    Port class 
 *      outer_vlan       - (IN)    Outer VLAN ID 
 *      inner_vlan       - (IN)    Inner VLAN ID  
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      Stat_counter_id  - (OUT) Stat Counter ID
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_vlan_translate_egress_stat_id_get(
            int             unit,
            int         port_class,
            bcm_vlan_t         outer_vlan,
            bcm_vlan_t      inner_vlan,
            bcm_vlan_stat_t stat, 
            uint32          *stat_counter_id)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    bcm_stat_flex_direction_t  direction=bcmStatFlexDirectionIngress;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    uint32                     index=0;
    uint32                     num_stat_counter_ids=0;
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_advanced_flex_counter)) {
        if ((stat == bcmVlanStatIngressPackets) ||
            (stat == bcmVlanStatIngressBytes)) {
             direction = bcmStatFlexDirectionIngress;
        } else {
             direction = bcmStatFlexDirectionEgress;
        }
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_translate_egress_stat_get_table_info(
                            unit, port_class,outer_vlan,inner_vlan,
                            &num_of_tables,&table_info[0]));
    
        for (index=0; index < num_of_tables ; index++) {
             if (table_info[index].direction == direction)
                 return _bcm_esw_stat_flex_get_counter_id(
                                      unit, 1, &table_info[index],
                                      &num_stat_counter_ids,stat_counter_id);
        }
        return BCM_E_NOT_FOUND;
    } else
#endif
#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)) {
        return _bcm_hr3_vlan_translate_egress_stat_id_get(unit, port_class,
                    outer_vlan, inner_vlan, stat, stat_counter_id);
    } else
#endif 
    {   
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      _bcm_esw_vlan_translate_egress_stat_get
 * Purpose:
 *      Extract egress VLAN translation statistics from the chip.
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      sync_mode  - (IN) hwcount is to be synced to sw count
 *      port_class - (IN) Group ID of ingress port
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
_bcm_esw_vlan_translate_egress_stat_get(int unit, int sync_mode, 
                                        int port_class, 
                                        bcm_vlan_t outer_vlan,
                                        bcm_vlan_t inner_vlan,
                                        bcm_vlan_stat_t stat,
                                        uint64 *val)
{
#if defined(BCM_TRIUMPH2_SUPPORT)|| defined(BCM_TRIDENT_SUPPORT)
    _bcm_flex_stat_handle_t vxlt_fsh;
#endif    
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) ||\
    defined(BCM_HURRICANE3_SUPPORT)
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};
#endif

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_translate_egress_stat_param_valid(unit, port_class,
                                                  outer_vlan, inner_vlan,
                                                  &vxlt_fsh));
        return _bcm_esw_flex_stat_ext_get(unit, sync_mode, _bcmFlexStatTypeVxlt,
                                      vxlt_fsh,
               _bcm_esw_vlan_translate_egress_stat_to_flex_stat(stat),
                                      val);
    } else
#endif        
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_advanced_flex_counter) ||
        soc_feature(unit, soc_feature_flowcnt)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_vlan_xslate_egress_stat_counter_get(
                        unit, sync_mode, port_class, outer_vlan, inner_vlan,
                        stat, 1, &counter_indexes, &counter_values));
        if ((stat == bcmVlanStatIngressPackets) ||
            (stat == bcmVlanStatEgressPackets)) {
             COMPILER_64_SET(*val,0,counter_values.packets);
        } else {
             COMPILER_64_SET(*val,
                             COMPILER_64_HI(counter_values.bytes),
                             COMPILER_64_LO(counter_values.bytes));
        }
        return BCM_E_NONE;
    } else 
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_vlan_translate_egress_stat_get
 * Purpose:
 *      Extract egress VLAN translation statistics from the chip.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port_class - (IN) Group ID of ingress port
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_vlan_translate_egress_stat_get(int unit, int port_class, 
                                       bcm_vlan_t outer_vlan,
                                       bcm_vlan_t inner_vlan,
                                       bcm_vlan_stat_t stat,
                                       uint64 *val)
{
    int rv = BCM_E_NONE;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
    rv = _bcm_esw_vlan_translate_egress_stat_get(unit, 0, port_class,
                                                 outer_vlan, inner_vlan,
                                                 stat, val);
    return rv;
}

/*
 * Function:
 *      bcm_esw_vlan_translate_egress_stat_sync_get
 * Purpose:
 *      Extract egress VLAN translation statistics from the chip.
 *      sw accumulated counters synced with hw count.
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      port_class - (IN) Group ID of ingress port
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_vlan_translate_egress_stat_sync_get(int unit, int port_class, 
                                            bcm_vlan_t outer_vlan,
                                            bcm_vlan_t inner_vlan,
                                            bcm_vlan_stat_t stat,
                                            uint64 *val)
{
    int rv = BCM_E_NONE;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
    rv = _bcm_esw_vlan_translate_egress_stat_get(unit, 1, port_class,
                                                 outer_vlan, inner_vlan,
                                                 stat, val);
    return rv;
}

/*
 * Function:
 *      _bcm_esw_vlan_translate_egress_stat_get32
 * Purpose:
 *      Extract egress VLAN translation statistics from the chip.
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      sync_mode  - (IN) hwcount is to be synced to sw count
 *      port_class - (IN) Group ID of ingress port
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
_bcm_esw_vlan_translate_egress_stat_get32(int unit, int sync_mode, 
                                          int port_class, 
                                          bcm_vlan_t outer_vlan,
                                          bcm_vlan_t inner_vlan,
                                          bcm_vlan_stat_t stat,
                                          uint32 *val)
{
#if defined(BCM_TRIUMPH2_SUPPORT)|| defined(BCM_TRIDENT_SUPPORT)
    _bcm_flex_stat_handle_t vxlt_fsh;
#endif    
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};
#endif

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit, soc_feature_gport_service_counters)) { 
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_translate_egress_stat_param_valid(unit, port_class,
                                                  outer_vlan, inner_vlan,
                                                  &vxlt_fsh));
        return _bcm_esw_flex_stat_ext_get32(unit, sync_mode, 
                    _bcmFlexStatTypeVxlt, vxlt_fsh,
                _bcm_esw_vlan_translate_egress_stat_to_flex_stat(stat),
                                        val);
    } else
#endif        
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_advanced_flex_counter) ||
        soc_feature(unit, soc_feature_flowcnt)) {    
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xslate_egress_stat_counter_get(
                        unit, sync_mode, port_class, outer_vlan, inner_vlan,
                        stat, 1, &counter_indexes, &counter_values));
        if ((stat == bcmVlanStatIngressPackets) ||
            (stat == bcmVlanStatEgressPackets)) {
             *val = counter_values.packets;
        } else {
             /* Ignoring Hi bytes value */
             *val = COMPILER_64_LO(counter_values.bytes);
        }
        return BCM_E_NONE;
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}


/*
 * Function:
 *      bcm_esw_vlan_translate_egress_stat_get32
 * Purpose:
 *      Extract egress VLAN translation statistics from the chip.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port_class - (IN) Group ID of ingress port
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_vlan_translate_egress_stat_get32(int unit, int port_class, 
                                         bcm_vlan_t outer_vlan,
                                         bcm_vlan_t inner_vlan,
                                         bcm_vlan_stat_t stat,
                                         uint32 *val)
{
    int rv = BCM_E_NONE;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
    rv = _bcm_esw_vlan_translate_egress_stat_get32(unit, 0, port_class,
                                                   outer_vlan, inner_vlan,
                                                   stat, val);
    return rv;
}

/*
 * Function:
 *      bcm_esw_vlan_translate_egress_stat_sync_get32
 * Purpose:
 *      Extract egress VLAN translation statistics from the chip.
 *      sw accumulated counters synced with hw count.
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      port_class - (IN) Group ID of ingress port
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_vlan_translate_egress_stat_sync_get32(int unit, int port_class,
                                              bcm_vlan_t outer_vlan,
                                              bcm_vlan_t inner_vlan,
                                              bcm_vlan_stat_t stat,
                                              uint32 *val)
{
    int rv = BCM_E_NONE;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
    rv = _bcm_esw_vlan_translate_egress_stat_get32(unit, 1, port_class,
                                                   outer_vlan, inner_vlan,
                                                   stat, val);
    return rv;
}

/*
 * Function:
 *      bcm_esw_vlan_translate_egress_stat_set
 * Purpose:
 *      Set the specified statistic to the indicated value for the
 *      specified egress VLAN translation.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port_class - (IN) Group ID of ingress port
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      stat - (IN) Type of the counter to set.
 *      val - (IN) New counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_vlan_translate_egress_stat_set(int unit, 
                                       int port_class,
                                       bcm_vlan_t outer_vlan,
                                       bcm_vlan_t inner_vlan,
                                       bcm_vlan_stat_t stat,
                                       uint64 val)
{
#if defined(BCM_TRIUMPH2_SUPPORT)|| defined(BCM_TRIDENT_SUPPORT)
    _bcm_flex_stat_handle_t vxlt_fsh;
#endif    
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_translate_egress_stat_param_valid(unit, port_class,
                                                  outer_vlan, inner_vlan,
                                                  &vxlt_fsh));
        return _bcm_esw_flex_stat_ext_set(unit, _bcmFlexStatTypeVxlt,
                                      vxlt_fsh,
                _bcm_esw_vlan_translate_egress_stat_to_flex_stat(stat),
                                      val);
    } else
#endif        
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_advanced_flex_counter) ||
        soc_feature(unit, soc_feature_flowcnt)) {
        if ((stat == bcmVlanStatIngressPackets) ||
            (stat == bcmVlanStatEgressPackets)) {
             counter_values.packets = COMPILER_64_LO(val);
        } else {
             COMPILER_64_SET(counter_values.bytes,
                             COMPILER_64_HI(val),
                             COMPILER_64_LO(val));
        }
        BCM_IF_ERROR_RETURN(bcm_esw_vlan_translate_egress_stat_counter_set(
                            unit,port_class,outer_vlan,inner_vlan,
                            stat, 1, &counter_indexes, &counter_values));
        return BCM_E_NONE;
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_vlan_translate_egress_stat_set32
 * Purpose:
 *      Set the specified statistic to the indicated value for the
 *      specified egress VLAN translation.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port_class - (IN) Group ID of ingress port
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      stat - (IN) Type of the counter to set.
 *      val - (IN) New counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_vlan_translate_egress_stat_set32(int unit, int port_class, 
                                         bcm_vlan_t outer_vlan,
                                         bcm_vlan_t inner_vlan,
                                         bcm_vlan_stat_t stat,
                                         uint32 val)
{
#if defined(BCM_TRIUMPH2_SUPPORT)|| defined(BCM_TRIDENT_SUPPORT)
    _bcm_flex_stat_handle_t vxlt_fsh;
#endif    
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_translate_egress_stat_param_valid(unit, port_class,
                                                  outer_vlan, inner_vlan,
                                                  &vxlt_fsh));
        return _bcm_esw_flex_stat_ext_set32(unit, _bcmFlexStatTypeVxlt,
                                        vxlt_fsh,
                _bcm_esw_vlan_translate_egress_stat_to_flex_stat(stat),
                                        val);
    } else
#endif        
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_advanced_flex_counter) ||
        soc_feature(unit, soc_feature_flowcnt)) {
        if ((stat == bcmVlanStatIngressPackets) ||
            (stat == bcmVlanStatEgressPackets)) {
            counter_values.packets = val;
        } else {
             /* Ignoring high value */
             COMPILER_64_SET(counter_values.bytes,0,val);
        }
        BCM_IF_ERROR_RETURN(bcm_esw_vlan_translate_egress_stat_counter_set(
                            unit,port_class,outer_vlan,inner_vlan,
                            stat, 1, &counter_indexes, &counter_values));
        return BCM_E_NONE;
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_vlan_translate_egress_stat_multi_get
 * Purpose:
 *      Get 64-bit counter value for multiple egress VLAN translation
 *      statistic types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port_class - (IN) Group ID of ingress port
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (OUT) Collected counters values
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_vlan_translate_egress_stat_multi_get(int unit, int port_class, 
                                             bcm_vlan_t outer_vlan,
                                             bcm_vlan_t inner_vlan,
                                             int nstat,
                                             bcm_vlan_stat_t *stat_arr,
                                             uint64 *value_arr)
{
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    _bcm_flex_stat_handle_t vxlt_fsh;
    _bcm_flex_stat_t fs_arr[_bcmFlexStatNum]; /* Normalize stats */
    int idx;
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
    if (soc_feature(unit, soc_feature_gport_service_counters)) {  
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_translate_egress_stat_param_valid(unit, port_class,
                                                      outer_vlan, inner_vlan,
                                                      &vxlt_fsh));
        BCM_IF_ERROR_RETURN
            (_bcm_vlan_stat_array_convert(unit, nstat, stat_arr, fs_arr,
                       &_bcm_esw_vlan_translate_egress_stat_to_flex_stat));
        BCM_VLAN_VALUE_ARRAY_VALID(unit, nstat, value_arr);
    
        return _bcm_esw_flex_stat_ext_multi_get(unit, _bcmFlexStatTypeVxlt,
                                                vxlt_fsh,
                                            nstat, fs_arr, value_arr);
    } else
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_advanced_flex_counter) ||
        soc_feature(unit, soc_feature_flowcnt)) {
        for (idx=0;idx < nstat ; idx ++) {
             BCM_IF_ERROR_RETURN(bcm_esw_vlan_translate_egress_stat_counter_get(
                                 unit,port_class,outer_vlan,inner_vlan,
                                 stat_arr[idx],1,&counter_indexes,&counter_values));
             if ((stat_arr[idx] == bcmVlanStatIngressPackets) ||
                 (stat_arr[idx] == bcmVlanStatEgressPackets)) {
    
                  COMPILER_64_SET(value_arr[idx],
                                  COMPILER_64_HI(counter_values.packets64),
                                  COMPILER_64_LO(counter_values.packets64));
             } else {
                  COMPILER_64_SET(value_arr[idx],
                                  COMPILER_64_HI(counter_values.bytes),
                                  COMPILER_64_LO(counter_values.bytes));
             }
        }
        return BCM_E_NONE;
    } else
#endif
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_vlan_translate_egress_stat_multi_get32
 * Purpose:
 *      Get lower 32-bit counter value for multiple egress VLAN
 *      translation statistic types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port_class - (IN) Group ID of ingress port
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (OUT) Collected counters values
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_vlan_translate_egress_stat_multi_get32(int unit, int port_class, 
                                               bcm_vlan_t outer_vlan,
                                               bcm_vlan_t inner_vlan,
                                               int nstat,
                                               bcm_vlan_stat_t *stat_arr,
                                               uint32 *value_arr)
{
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    _bcm_flex_stat_handle_t vxlt_fsh;
    _bcm_flex_stat_t fs_arr[_bcmFlexStatNum]; /* Normalize stats */
    int idx;
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
  
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_translate_egress_stat_param_valid(unit, port_class,
                                                      outer_vlan, inner_vlan,
                                                      &vxlt_fsh));
        BCM_IF_ERROR_RETURN
            (_bcm_vlan_stat_array_convert(unit, nstat, stat_arr, fs_arr,
                       &_bcm_esw_vlan_translate_egress_stat_to_flex_stat));
        BCM_VLAN_VALUE_ARRAY_VALID(unit, nstat, value_arr);
        
        return _bcm_esw_flex_stat_ext_multi_get32(unit, _bcmFlexStatTypeVxlt,
                                                  vxlt_fsh,
                                            nstat, fs_arr, value_arr);
    } else
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_advanced_flex_counter) ||
        soc_feature(unit, soc_feature_flowcnt)) {
        for (idx=0;idx < nstat ; idx ++) {
             BCM_IF_ERROR_RETURN(bcm_esw_vlan_translate_egress_stat_counter_get(
                                 unit,port_class,outer_vlan,inner_vlan,
                                 stat_arr[idx],1,&counter_indexes,&counter_values));
             if ((stat_arr[idx] == bcmVlanStatIngressPackets) ||
                 (stat_arr[idx] == bcmVlanStatEgressPackets)) {
                  value_arr[idx] = counter_values.packets;
             } else {
                  value_arr[idx] = COMPILER_64_LO(counter_values.bytes);
             }
        }
    return BCM_E_NONE;
    }
#endif
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_vlan_translate_egress_stat_multi_set
 * Purpose:
 *      Set 64-bit counter value for multiple egress VLAN translation
 *      statistic types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port_class - (IN) Group ID of ingress port
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) New statistics descriptors array
 *      value_arr - (IN) New counters values
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_vlan_translate_egress_stat_multi_set(int unit, int port_class, 
                                             bcm_vlan_t outer_vlan,
                                             bcm_vlan_t inner_vlan,
                                             int nstat,
                                             bcm_vlan_stat_t *stat_arr,
                                             uint64 *value_arr)
{
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    _bcm_flex_stat_handle_t vxlt_fsh;
    _bcm_flex_stat_t fs_arr[_bcmFlexStatNum]; /* Normalize stats */
    int idx;
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT) 
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_translate_egress_stat_param_valid(unit, port_class,
                                                      outer_vlan, inner_vlan,
                                                      &vxlt_fsh));
        BCM_IF_ERROR_RETURN
            (_bcm_vlan_stat_array_convert(unit, nstat, stat_arr, fs_arr,
                       &_bcm_esw_vlan_translate_egress_stat_to_flex_stat));
        BCM_VLAN_VALUE_ARRAY_VALID(unit, nstat, value_arr);
    
        return _bcm_esw_flex_stat_ext_multi_set(unit, _bcmFlexStatTypeVxlt,
                                                vxlt_fsh,
                                            nstat, fs_arr, value_arr);
    } else
#endif        
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_advanced_flex_counter) ||
        soc_feature(unit, soc_feature_flowcnt)) {
        for (idx=0;idx < nstat ; idx ++) {
             if ((stat_arr[idx] == bcmVlanStatIngressPackets) ||
                 (stat_arr[idx] == bcmVlanStatEgressPackets)) {
                  counter_values.packets = COMPILER_64_LO(value_arr[idx]);
             } else {
                  COMPILER_64_SET(counter_values.bytes,
                                  COMPILER_64_HI(value_arr[idx]),
                                  COMPILER_64_LO(value_arr[idx]));
             }
             BCM_IF_ERROR_RETURN(bcm_esw_vlan_translate_egress_stat_counter_set(
                                 unit,port_class,outer_vlan,inner_vlan,
                                 stat_arr[idx],1,&counter_indexes,&counter_values));
        }
        return BCM_E_NONE;
    } else
#endif
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_vlan_translate_egress_stat_multi_set32
 * Purpose:
 *      Set lower 32-bit counter value for multiple egress VLAN
 *      translation statistic types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port_class - (IN) Group ID of ingress port
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) New statistics descriptors array
 *      value_arr - (IN) New counters values
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_vlan_translate_egress_stat_multi_set32(int unit, int port_class,
                                               bcm_vlan_t outer_vlan,
                                               bcm_vlan_t inner_vlan,
                                               int nstat,
                                               bcm_vlan_stat_t *stat_arr,
                                               uint32 *value_arr)
{
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    _bcm_flex_stat_handle_t vxlt_fsh;
    _bcm_flex_stat_t fs_arr[_bcmFlexStatNum]; /* Normalize stats */
    int idx;
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_translate_egress_stat_param_valid(unit, port_class,
                                                      outer_vlan, inner_vlan,
                                                      &vxlt_fsh));
        BCM_IF_ERROR_RETURN
            (_bcm_vlan_stat_array_convert(unit, nstat, stat_arr, fs_arr,
                       &_bcm_esw_vlan_translate_egress_stat_to_flex_stat));
        BCM_VLAN_VALUE_ARRAY_VALID(unit, nstat, value_arr);
    
        return _bcm_esw_flex_stat_ext_multi_set32(unit, _bcmFlexStatTypeVxlt,
                                                  vxlt_fsh,
                                            nstat, fs_arr, value_arr);
    } else
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_advanced_flex_counter) ||
        soc_feature(unit, soc_feature_flowcnt)) {
        for (idx=0;idx < nstat ; idx ++) {
             if ((stat_arr[idx] == bcmVlanStatIngressPackets) ||
                 (stat_arr[idx] == bcmVlanStatEgressPackets)) {
                  counter_values.packets = value_arr[idx];
             } else {
                  COMPILER_64_SET(counter_values.bytes,0,value_arr[idx]);
             }
             BCM_IF_ERROR_RETURN(bcm_esw_vlan_translate_egress_stat_counter_set(
                                 unit,port_class,outer_vlan,inner_vlan,
                                 stat_arr[idx],1,&counter_indexes,&counter_values));
        }
        return BCM_E_NONE;
    } else
#endif
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *     _bcm_vlan_queue_map_id_check
 * Description:
 *     To check the status of the specified qmid
 * Parameters:
 *     unit            device number
 *     qmid            queue mapping identifier
 * Return:
 *     BCM_E_UNAVAIL   queue mapping feature is not supported for this device
 *     BCM_E_INIT      vlan component is not initialized on this device
 *     BCM_E_BADID     specified qmid is not in valid range
 *     BCM_E_NOT_FOUND spefified qmid is in valid range but not in used
 *     BCM_E_NONE      specified qmid is in used
 */
STATIC int
_bcm_vlan_queue_map_id_check(int unit,
                             int qmid)
{
    int size;

    if (!soc_feature(unit, soc_feature_vlan_queue_map)) {
        return BCM_E_UNAVAIL;
    }

    CHECK_INIT(unit);

    size = 1 << soc_mem_field_length(unit, VLAN_PROFILE_TABm,
                                     PHB2_DOT1P_MAPPING_PTRf);

    if (qmid < 0 || qmid >= size) {
        return BCM_E_BADID;
    }

    if (!SHR_BITGET(vlan_info[unit].qm_bmp, qmid)) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/* Create a VLAN queue map entry. */
int
bcm_esw_vlan_queue_map_create(int unit,
                              uint32 flags,
                              int *qmid)
{
    bcm_vlan_info_t *vi;
    int size, bmp_size, free_qmid, i;
    uint32 bits, tag_flags;
    int rv;

    if (!soc_feature(unit, soc_feature_vlan_queue_map)) {
        return BCM_E_UNAVAIL;
    }

    CHECK_INIT(unit);

    vi = &vlan_info[unit];
    size = 1 << soc_mem_field_length(unit, VLAN_PROFILE_TABm,
                                     PHB2_DOT1P_MAPPING_PTRf);

    if (flags & BCM_VLAN_QUEUE_MAP_REPLACE) {
        if (!(flags & BCM_VLAN_QUEUE_MAP_WITH_ID)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(_bcm_vlan_queue_map_id_check(unit, *qmid));
    }
    tag_flags = flags & (BCM_VLAN_QUEUE_MAP_INNER_PKT_PRI |
                         BCM_VLAN_QUEUE_MAP_OUTER_PKT_PRI);
    if (tag_flags != BCM_VLAN_QUEUE_MAP_OUTER_PKT_PRI &&
        tag_flags != BCM_VLAN_QUEUE_MAP_INNER_PKT_PRI) {
        return BCM_E_PARAM;
    }

    if (!(flags & BCM_VLAN_QUEUE_MAP_REPLACE)) {
        if (flags & BCM_VLAN_QUEUE_MAP_WITH_ID) {
            rv = _bcm_vlan_queue_map_id_check(unit, *qmid);
            /* make sure the id is in the valid range and not taken */
            if (rv != BCM_E_NOT_FOUND) {
                return BCM_E_PARAM;
            } 
            SHR_BITSET(vi->qm_bmp, *qmid);
        } else {
            free_qmid = size;
            bmp_size = _SHR_BITDCLSIZE(size);
            /* find the index of first zero from qm_bmp */
            for (i = 0; i < bmp_size; i++) {
                if (vi->qm_bmp[i] == 0xffffffff) {
                    continue;
                }
                bits = vi->qm_bmp[i];
                bits &= (bits << 1) | 0x00000001;
                bits &= (bits << 2) | 0x00000003;
                bits &= (bits << 4) | 0x0000000f;
                bits &= (bits << 8) | 0x000000ff;
                bits &= (bits << 16) | 0x0000ffff;
                free_qmid = (i << 5) + _shr_popcount(bits);
                break;
            }
            if (free_qmid >= size) {
                return BCM_E_RESOURCE;
            }
            SHR_BITSET(vi->qm_bmp, free_qmid);
            *qmid = free_qmid;
        }
    }

    if (flags & BCM_VLAN_QUEUE_MAP_INNER_PKT_PRI) {
        SHR_BITSET(vi->qm_it_bmp, *qmid);
    } else {
        SHR_BITCLR(vi->qm_it_bmp, *qmid);
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return BCM_E_NONE;
}

/* Delete a VLAN queue map entry. */
int
bcm_esw_vlan_queue_map_destroy(int unit,
                               int qmid)
{
    int pkt_pri, cfi;

    if (!soc_feature(unit, soc_feature_vlan_queue_map)) {
        return BCM_E_UNAVAIL;
    }
    BCM_IF_ERROR_RETURN(_bcm_vlan_queue_map_id_check(unit, qmid));

    for (pkt_pri = 0; pkt_pri <=7; pkt_pri++) {
        for (cfi = 0; cfi <= 1; cfi++) {
            BCM_IF_ERROR_RETURN(bcm_esw_vlan_queue_map_set(unit,
                                        qmid, pkt_pri, cfi, 0, 0));
        }
    }

    SHR_BITCLR(vlan_info[unit].qm_bmp, qmid);

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return BCM_E_NONE;
}

/* Delete all VLAN queue map entries. */
int
bcm_esw_vlan_queue_map_destroy_all(int unit)
{
    int size, qmid;

    if (!soc_feature(unit, soc_feature_vlan_queue_map)) {
        return BCM_E_UNAVAIL;
    }

    CHECK_INIT(unit);

    size = 1 << soc_mem_field_length(unit, VLAN_PROFILE_TABm,
                                     PHB2_DOT1P_MAPPING_PTRf);

    for (qmid = 0; qmid < size; qmid++) {
        if (SHR_BITGET(vlan_info[unit].qm_bmp, qmid)) {
            BCM_IF_ERROR_RETURN(bcm_esw_vlan_queue_map_destroy(unit, qmid));
        }
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return BCM_E_NONE;
}

/* Set a VLAN queue map entry. */
int
bcm_esw_vlan_queue_map_set(int unit,
                           int qmid,
                           int pkt_pri,
                           int cfi,
                           int queue,
                           int color)
{
    phb2_cos_map_entry_t entry;
    int index;

    if (!soc_feature(unit, soc_feature_vlan_queue_map)) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(_bcm_vlan_queue_map_id_check(unit, qmid));

    if (pkt_pri < 0 || pkt_pri > 7 || cfi < 0 || cfi > 1) {
        return BCM_E_PARAM;
    }

    if (queue < 0 ||
        queue >= (1 << soc_mem_field_length(unit, PHB2_COS_MAPm, COSf))) {
        return BCM_E_PARAM;
    }

    /* For Triumph2 and Enduro, the entry index is:
     * {PHB2_DOT1P_MAPPING_PTR(4-bit), 802.1 priority(3-bit), CFI/DE}.
     * From Trident onward, the index takes format of:
     * {802.1p_priority[2:0], 802.1p_cfi, PHB2_DOT1P_MAPPING_PTR}.
     */
  
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit)) {
        index = (qmid << 4) | (pkt_pri << 1) | cfi;
    } else
#endif
#ifdef BCM_ENDURO_SUPPORT
    if (SOC_IS_ENDURO(unit)) {
        index = (qmid << 4) | (pkt_pri << 1) | cfi;
    } else
#endif
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
        index = (pkt_pri << 6) | (cfi << 5) | (qmid << 1); /* Wired */
    } else
#endif
    {
        index = (pkt_pri << 5) | (cfi << 4) | qmid;
    }

    sal_memset(&entry, 0, sizeof(entry));
    soc_mem_field32_set(unit, PHB2_COS_MAPm, &entry, COSf, queue);

    if (soc_feature(unit, soc_feature_vlan_multicast_queue_map)) {
        soc_mem_field32_set(unit, PHB2_COS_MAPm, &entry, MC_COS2f, queue);
    }

    BCM_IF_ERROR_RETURN(soc_mem_write(unit, PHB2_COS_MAPm, MEM_BLOCK_ANY,
                                      index,
                                      &entry));

#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
        index = ((pkt_pri << 6) | (cfi << 5) | (qmid << 1)) + 1; /* Wireless */
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, PHB2_COS_MAPm, MEM_BLOCK_ANY,
                        index, &entry));
    }
#endif

    return BCM_E_NONE;
}

/* Get a VLAN queue map entry. */
int
bcm_esw_vlan_queue_map_get(int unit,
                           int qmid,
                           int pkt_pri,
                           int cfi,
                           int *queue,
                           int *color)
{
    phb2_cos_map_entry_t entry;
    int index;

    if (!soc_feature(unit, soc_feature_vlan_queue_map)) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(_bcm_vlan_queue_map_id_check(unit, qmid));

    if (pkt_pri < 0 || pkt_pri > 7 || cfi < 0 || cfi > 1) {
        return BCM_E_PARAM;
    }

    if (queue == NULL || color == NULL) {
        return BCM_E_PARAM;
    }

#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit)) {
        index = (qmid << 4) | (pkt_pri << 1) | cfi;
    } else
#endif
#ifdef BCM_ENDURO_SUPPORT
    if (SOC_IS_ENDURO(unit)) {
        index = (qmid << 4) | (pkt_pri << 1) | cfi;
    } else
#endif
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
        index = (pkt_pri << 6) | (cfi << 5) | (qmid << 1); /* Wired */
    } else
#endif
    {
        index = (pkt_pri << 5) | (cfi << 4) | qmid;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, PHB2_COS_MAPm, MEM_BLOCK_ANY,
                                     index,
                                     &entry));
    *queue = soc_mem_field32_get(unit, PHB2_COS_MAPm, &entry, COSf);
    *color = 0;

    return BCM_E_NONE;
}

/* Attach a queue map object to a VLAN or VFI. */
int
bcm_esw_vlan_queue_map_attach(int unit,
                              bcm_vlan_t vlan,
                              int qmid)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    vfi_entry_t vfi_entry;
    int use_inner_tag;
#endif /* BCM_TRIUMHP2_SUPPORT */
    if (!soc_feature(unit, soc_feature_vlan_queue_map)) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(_bcm_vlan_queue_map_id_check(unit, qmid));

#ifdef BCM_TRIUMPH2_SUPPORT
    VLAN_CHK_ID(unit, vlan);

    use_inner_tag = SHR_BITGET(vlan_info[unit].qm_it_bmp, qmid) ? 1 : 0;
    BCM_IF_ERROR_RETURN
        (_bcm_tr2_vlan_qmid_set(unit, vlan, qmid, use_inner_tag));

    if (SOC_MEM_IS_VALID(unit, VFIm)) {
        BCM_IF_ERROR_RETURN(READ_VFIm(unit, MEM_BLOCK_ANY, vlan, &vfi_entry));
        soc_mem_field32_set(unit, VFIm, &vfi_entry, PHB2_DOT1P_MAPPING_PTRf, qmid);
        soc_mem_field32_set(unit, VFIm, &vfi_entry, PHB2_USE_INNER_DOT1Pf,
                            use_inner_tag);
        soc_mem_field32_set(unit, VFIm, &vfi_entry, PHB2_ENABLEf, 1);
        BCM_IF_ERROR_RETURN(WRITE_VFIm(unit, MEM_BLOCK_ANY, vlan, &vfi_entry));
    }
#endif /* BCM_TRIUMHP2_SUPPORT */

    return BCM_E_NONE;
}

/* Get the queue map object which is attached to a VLAN or VFI. */
int
bcm_esw_vlan_queue_map_attach_get(int unit,
                                  bcm_vlan_t vlan,
                                  int *qmid)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    int use_inner_tag;
#endif /* BCM_TRIUMHP2_SUPPORT */

    if (!soc_feature(unit, soc_feature_vlan_queue_map)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_TRIUMPH2_SUPPORT
    CHECK_INIT(unit);

    VLAN_CHK_ID(unit, vlan);

    if (qmid == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_tr2_vlan_qmid_get(unit, vlan, qmid, &use_inner_tag));
#endif /* BCM_TRIUMHP2_SUPPORT */

    return BCM_E_NONE;
}

/* Detach a queue map object from a VLAN or VFI. */
int
bcm_esw_vlan_queue_map_detach(int unit,
                              bcm_vlan_t vlan)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    vfi_entry_t vfi_entry;
#endif /* BCM_TRIUMHP2_SUPPORT */

    if (!soc_feature(unit, soc_feature_vlan_queue_map)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_TRIUMPH2_SUPPORT
    CHECK_INIT(unit);

    VLAN_CHK_ID(unit, vlan);

    BCM_IF_ERROR_RETURN(_bcm_tr2_vlan_qmid_set(unit, vlan, -1, 0));

    if (SOC_MEM_IS_VALID(unit, VFIm)) {
        BCM_IF_ERROR_RETURN(READ_VFIm(unit, MEM_BLOCK_ANY, vlan, &vfi_entry));
        soc_mem_field32_set(unit, VFIm, &vfi_entry, PHB2_DOT1P_MAPPING_PTRf, 0);
        soc_mem_field32_set(unit, VFIm, &vfi_entry, PHB2_USE_INNER_DOT1Pf, 0);
        soc_mem_field32_set(unit, VFIm, &vfi_entry, PHB2_ENABLEf, 0);
        BCM_IF_ERROR_RETURN(WRITE_VFIm(unit, MEM_BLOCK_ANY, vlan, &vfi_entry));
    }
#endif /* BCM_TRIUMHP2_SUPPORT */

    return BCM_E_NONE;
}

/* Detach queue map objects from all VLAN or VFI. */
int
bcm_esw_vlan_queue_map_detach_all(int unit)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    vfi_entry_t vfi_entry;
    int vlan;
#endif /* BCM_TRIUMHP2_SUPPORT */

    if (!soc_feature(unit, soc_feature_vlan_queue_map)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_TRIUMPH2_SUPPORT
    CHECK_INIT(unit);

    for (vlan = BCM_VLAN_MIN; vlan < BCM_VLAN_COUNT; vlan++) {
        if (_BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vlan)) {
            BCM_IF_ERROR_RETURN(_bcm_tr2_vlan_qmid_set(unit, vlan, -1, 0));

            if (!SOC_MEM_IS_VALID(unit, VFIm)) {
                continue;
            }

            BCM_IF_ERROR_RETURN(READ_VFIm(unit, MEM_BLOCK_ANY, vlan,
                                          &vfi_entry));
            soc_mem_field32_set(unit, VFIm, &vfi_entry,
                                PHB2_DOT1P_MAPPING_PTRf, 0);
            soc_mem_field32_set(unit, VFIm, &vfi_entry,
                                PHB2_USE_INNER_DOT1Pf, 0);
            soc_mem_field32_set(unit, VFIm, &vfi_entry, PHB2_ENABLEf, 0);
            BCM_IF_ERROR_RETURN(WRITE_VFIm(unit, MEM_BLOCK_ANY, vlan,
                                           &vfi_entry));
        }
    }
#endif /* BCM_TRIUMHP2_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_vlan_vfi_validate
 * Purpose:
 *      Validate a VLAN/VFI
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vlan_vfi - VLAN/VFI ID to validate.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_PARAM - Invalid value.
 */
int
bcmi_esw_vlan_vfi_validate(int unit, bcm_vlan_t vlan_vfi)
{

#if defined(INCLUDE_L3) && defined(BCM_TRX_SUPPORT)

    /* For vlan validate range, for vfi ensure that one of the vpn
       types matches */
    if((soc_feature(unit, soc_feature_vlan_vfi_membership)) &&
       _BCM_VPN_VFI_IS_SET(vlan_vfi)) {       

        _BCM_VPN_GET(vlan_vfi, _BCM_VPN_TYPE_VFI, vlan_vfi);
        if(_bcm_vfi_used_get(unit, vlan_vfi, _bcmVfiTypeMpls))
            return BCM_E_NONE;
        else if(_bcm_vfi_used_get(unit, vlan_vfi, _bcmVfiTypeMim))
            return BCM_E_NONE;
        else if(_bcm_vfi_used_get(unit, vlan_vfi, _bcmVfiTypeL2Gre))
            return BCM_E_NONE;
        else if(_bcm_vfi_used_get(unit, vlan_vfi, _bcmVfiTypeVxlan))
            return BCM_E_NONE;
        else if(_bcm_vfi_used_get(unit, vlan_vfi, _bcmVfiTypeFlow))
            return BCM_E_NONE;
        else {
            return BCM_E_PARAM;
        }
#if defined(BCM_TRIUMPH3_SUPPORT)
    } else if ((soc_feature(unit, soc_feature_vlan_vfi)) &&
        _BCM_VPN_VFI_IS_SET(vlan_vfi)) {
        _BCM_VPN_GET(vlan_vfi, _BCM_VPN_TYPE_VFI, vlan_vfi);
        if (_bcm_vfi_used_get(unit, vlan_vfi, _bcmVfiTypeVlan)) {
            return BCM_E_NONE;
        } else {
            return BCM_E_PARAM;
        }
#endif /* BCM_TRIUMPH3_SUPPORT */
    } else {
        VLAN_CHK_ID(unit, vlan_vfi);
    }

#else
        VLAN_CHK_ID(unit, vlan_vfi);
#endif

    return BCM_E_NONE;
}

#define MOD_PORT_TO_GLP(unit, modid, port, glp)   \
    do {    \
        if (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_APACHE(unit)) { \
            glp = ((0xFF & modid) << 7) | (0x7F & port);    \
        } else {    \
            glp = ((0xFF & modid) << 8) | (0xFF & port);    \
        }   \
    } while(0)

/*
 * Function:
 *      bcm_vlan_gport_add
 * Purpose:
 *      Add a Gport to the specified vlan. Adds WLAN/VLAN/NIV/Extender ports to
 *      broadcast, multicast and unknown unicast groups.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vlan - VLAN ID to add port to as a member.
 *      port - Gport ID
 *      flags - BCM_VLAN_PORT_XXX
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 *      BCM_E_NOT_FOUND - VLAN ID not in use.
 */
int
bcm_esw_vlan_gport_add(int unit, bcm_vlan_t vlan, bcm_gport_t port, 
                       int flags)
{
    int rv = BCM_E_NONE;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_vlan_gport_add(unit, vlan, port, flags);
    }
#endif

    CHECK_INIT(unit);

    /* VxLAN passes in a VFI, hence check VLAN for non-VxLan cases */
    BCM_IF_ERROR_RETURN(bcmi_esw_vlan_vfi_validate(unit, vlan));
    
    if (port == BCM_GPORT_INVALID)
        return BCM_E_PORT;

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_wlan) && BCM_GPORT_IS_WLAN_PORT(port)) {
        int i, vp, mc_idx, gport_id, mod_local, group = 0;
        vlan_tab_entry_t vtab;
        bcm_wlan_port_t wlan_port;
        bcm_module_t mod_out;
        bcm_port_t port_out;
        bcm_trunk_t trunk_id;
        bcm_gport_t local_gport;
        bcm_if_t encap_id;
        bcm_pbmp_t vlan_pbmp, vlan_ubmp, pbmp, l3_pbmp;
        soc_field_t group_type[3] = {BC_IDXf, UMC_IDXf, UUC_IDXf};

        vp = BCM_GPORT_WLAN_PORT_ID_GET(port);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
            return BCM_E_NOT_FOUND;
        }
        if (flags != 0) {
            return BCM_E_PARAM;
        }
        sal_memset(&vtab, 0, sizeof(vlan_tab_entry_t));

        soc_mem_lock(unit, VLAN_TABm);

        if ((rv = soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int)vlan, 
             &vtab)) < 0) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }
        if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VALIDf)) {
            soc_mem_unlock(unit, VLAN_TABm);
            return BCM_E_NOT_FOUND;
        }

        /* Get the VP attributes (need physical port) and encap id */
        if ((rv = bcm_esw_wlan_port_get(unit, port, &wlan_port)) < 0) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }

        /* Enable VP switching on the VLAN */
        if (!SHR_BITGET(vlan_info[unit].vp_mode, vlan) && 
            SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {
            if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
               soc_mem_field32_set(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf, 1);
               if ((rv = soc_mem_write(unit, VLAN_TABm, MEM_BLOCK_ALL, (int)vlan, 
                    &vtab)) < 0) {
                   soc_mem_unlock(unit, VLAN_TABm);
                   return rv;
               }
               /* Also need to copy the physical port members to the L2_BITMAP of
                * the IPMC entry for each group once we've gone virtual */
               rv = mbcm_driver[unit]->mbcm_vlan_port_get
                   (unit, vlan, &vlan_pbmp, &vlan_ubmp, NULL);
                                                       
               if (rv < 0) {
                   soc_mem_unlock(unit, VLAN_TABm);
                   return rv;
               }
               /* Deal with each group */
               for (i = 0; i < 3; i++) {
                   mc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab,
                                                group_type[i]);
                   rv = _bcm_esw_multicast_ipmc_read(unit, mc_idx, &pbmp, 
                                                     &l3_pbmp);
                   if (rv < 0) {
                       soc_mem_unlock(unit, VLAN_TABm);
                       return rv;
                   }
                   rv = _bcm_esw_multicast_ipmc_write(unit, mc_idx, vlan_pbmp, 
                                                      l3_pbmp, TRUE);
                   if (rv < 0) {
                       soc_mem_unlock(unit, VLAN_TABm);
                       return rv;
                   }
               }   
            }
        }

        /* Check if wlan_port.port is a local port or trunk */
        if (BCM_GPORT_IS_TRUNK(wlan_port.port)) {
            trunk_id = BCM_GPORT_TRUNK_GET(wlan_port.port);
            rv = _bcm_trunk_id_validate(unit, trunk_id);
            if (BCM_FAILURE(rv)) {
                  soc_mem_unlock(unit, VLAN_TABm);
                  return (BCM_E_PORT);
            }
            local_gport = wlan_port.port;
        } else {
            rv = _bcm_esw_gport_resolve(unit, wlan_port.port, &mod_out, &port_out,
                              &trunk_id, &gport_id); 
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return (BCM_E_PORT);
            }
            rv = _bcm_esw_modid_is_local(unit, mod_out, &mod_local);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }
            if (TRUE != mod_local) {
                /* Only add this to replication set if destination is local */
               soc_mem_unlock(unit, VLAN_TABm);
               return BCM_E_PORT;
            }
            /* Convert system local_port to physical local_port */
            if (soc_feature(unit, soc_feature_sysport_remap)) {
                BCM_XLATE_SYSPORT_S2P(unit, &port_out);
            }
            rv = bcm_esw_port_gport_get(unit, port_out, &local_gport);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }        
        }
        rv = bcm_esw_multicast_wlan_encap_get(unit, group, local_gport,
                                              port, &encap_id);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }

        /* Add the VP to the BC group */
        mc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, BC_IDXf);
        _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_WLAN, mc_idx);
        rv = bcm_esw_multicast_egress_add(unit, group, local_gport, encap_id); 
        if (rv < 0) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }

        /* Add the VP to the UMC group */
        mc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, UMC_IDXf);
        _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_WLAN, mc_idx);
        rv = bcm_esw_multicast_egress_add(unit, group, local_gport, encap_id); 
        if (rv < 0) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }

        /* Add the VP to the UUC group */
        mc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, UUC_IDXf);
        _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_WLAN, mc_idx);
        rv = bcm_esw_multicast_egress_add(unit, group, local_gport, encap_id); 

        soc_mem_unlock(unit, VLAN_TABm);

    } else if (soc_feature(unit, soc_feature_vlan_vp) && BCM_GPORT_IS_VLAN_PORT(port)) {
#if defined(BCM_ENDURO_SUPPORT)
        if (SOC_IS_ENDURO(unit)) {
            rv = bcm_enduro_vlan_vp_add(unit, vlan, port, flags);
        } else 
#endif /* BCM_ENDURO_SUPPORT */
        {
            /* COVERITY
             * up to this point, coverity indicates 7604 bytes stack usage.
             * After this call, it becomes 8220 bytes which exceeds the coverity
             * default check limit 8192 bytes.
             */
            /* coverity[stack_use_overflow : FALSE] */
            rv = bcm_tr2_vlan_gport_add(unit, vlan, port, flags);
        }
    } else if (soc_feature(unit, soc_feature_niv) &&
            BCM_GPORT_IS_NIV_PORT(port)) {
        rv = bcm_tr2_vlan_gport_add(unit, vlan, port, flags);
    } else if (soc_feature(unit, soc_feature_port_extension) &&
            BCM_GPORT_IS_EXTENDER_PORT(port)) {
        rv = bcm_tr2_vlan_gport_add(unit, vlan, port, flags);
    } else if (soc_feature(unit, soc_feature_vp_lag) &&
            BCM_GPORT_IS_TRUNK(port)) {
        rv = bcm_tr2_vlan_gport_add(unit, vlan, port, flags);
    } else if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
               (soc_feature(unit, soc_feature_channelized_switching))) &&
            BCM_GPORT_IS_SUBPORT_PORT(port)) {
        rv = bcm_tr2_vlan_gport_add(unit, vlan, port, flags);
    } else if (soc_feature(unit, soc_feature_vxlan) &&
            BCM_GPORT_IS_VXLAN_PORT(port)) {
        rv = bcm_tr2_vlan_gport_add(unit, vlan, port, flags);
    } else if (soc_feature(unit, soc_feature_flex_flow) &&
            BCM_GPORT_IS_FLOW_PORT(port)) {
        rv = bcm_tr2_vlan_gport_add(unit, vlan, port, flags);
    } else if (soc_feature(unit, soc_feature_l2gre) &&
            BCM_GPORT_IS_L2GRE_PORT(port)) {
        rv = bcm_tr2_vlan_gport_add(unit, vlan, port, flags);
    } else if (soc_feature(unit, soc_feature_mim) &&
            BCM_GPORT_IS_MIM_PORT(port)) {
        rv = bcm_tr2_vlan_gport_add(unit, vlan, port, flags);
    } else if (soc_feature(unit, soc_feature_mpls) &&
            BCM_GPORT_IS_MPLS_PORT(port)) {
        rv = bcm_tr2_vlan_gport_add(unit, vlan, port, flags);
    } else if (BCM_GPORT_IS_VP_GROUP(port)) {
#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3)
        int vpg = BCM_GPORT_VP_GROUP_GET(port);

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_vlan_vfi_membership)){
            bcm_stg_t stg;

            if ((flags & BCM_VLAN_PORT_INGRESS_ONLY)) {
                BCM_IF_ERROR_RETURN(bcm_td2p_ing_vp_group_unmanaged_set(unit,TRUE));
                BCM_IF_ERROR_RETURN(
                    bcm_td2p_vlan_vp_group_set(unit, vlan, FALSE, vpg, TRUE));

                BCM_IF_ERROR_RETURN(
                    bcm_td2p_vp_group_vlan_vpn_stg_get(unit, vlan, FALSE, &stg));
                if (stg != BCM_STG_DEFAULT) {
                    BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_stp_state_set(unit, stg,
                        port, FALSE, _bcm_td2_flags_to_stp_state(unit, flags, FALSE)));
                }
            }

            if ((flags & BCM_VLAN_PORT_EGRESS_ONLY)) {
                BCM_IF_ERROR_RETURN(bcm_td2p_egr_vp_group_unmanaged_set(unit,TRUE));
                BCM_IF_ERROR_RETURN(
                    bcm_td2p_vlan_vp_group_set(unit, vlan, TRUE, vpg, TRUE));

                BCM_IF_ERROR_RETURN(
                    bcm_td2p_vp_group_vlan_vpn_stg_get(unit, vlan, TRUE, &stg));
                if (stg != BCM_STG_DEFAULT) {
                    BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_stp_state_set(unit, stg,
                        port, TRUE, _bcm_td2_flags_to_stp_state(unit, flags, TRUE)));
                }

                if (!_BCM_VPN_VFI_IS_SET(vlan)) {
                    BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_ut_state_set(unit,
                            vlan, vpg, (flags & BCM_VLAN_GPORT_ADD_UNTAGGED)));
                }
#if defined(BCM_TRIDENT3_SUPPORT)
                else if (SOC_MEM_IS_VALID(unit, EGR_VLAN_VFI_UNTAGm)) {
                    BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_ut_state_set(unit,
                            vlan, vpg, (flags & BCM_VLAN_GPORT_ADD_UNTAGGED)));
                }
#endif
            }
        } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
        {
            if ((flags & BCM_VLAN_PORT_INGRESS_ONLY) && 
                    soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership)) {
                BCM_IF_ERROR_RETURN(bcm_td_ing_vp_group_unmanaged_set(unit,TRUE));
                BCM_IF_ERROR_RETURN(bcm_td_vlan_vp_group_set(
                            unit, VLAN_TABLE(unit), vlan, vpg, TRUE));
            }
            if ((flags & BCM_VLAN_PORT_EGRESS_ONLY) &&
                    soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) {
                BCM_IF_ERROR_RETURN(bcm_td_egr_vp_group_unmanaged_set(unit,TRUE));
                BCM_IF_ERROR_RETURN(bcm_td_vlan_vp_group_set(
                            unit, EGR_VLANm, vlan, vpg, TRUE));
            }
        }
#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */
    } else
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
    {
        bcm_pbmp_t pbmp, ubmp, ing_pbmp;
        bcm_port_t local_port;

#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_egr_vp_vlan_membership)) {
            if ((flags & BCM_VLAN_GPORT_ADD_EGRESS_ONLY) &&
                 (flags & BCM_VLAN_GPORT_ADD_VP_VLAN_MEMBERSHIP) &&
                         _BCM_VPN_VFI_IS_SET(vlan)) {
                if(!BCM_GPORT_IS_MODPORT(port)) {
                    return BCM_E_PARAM;
                }
                rv = bcm_tr2_vlan_gport_add(unit, vlan, port, flags);
                return rv;
            }
        }
#endif
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_vlan_vfi_membership) &&
            (flags & BCM_VLAN_GPORT_ADD_VP_VLAN_MEMBERSHIP)) {
                if (!BCM_GPORT_IS_MODPORT(port)) {
                    return BCM_E_PARAM;
                }

                MOD_PORT_TO_GLP(unit, BCM_GPORT_MODPORT_MODID_GET(port),
                    BCM_GPORT_MODPORT_PORT_GET(port), port);
                rv = bcm_td2_vp_vlan_membership_add(unit, port, vlan, flags);
                return rv;
            }
#endif

        /* If Ingress/Egress only flags are specified and 'ING_PORT_BITMAPf' is
           not a valid field in 'VLAN_TABm' AND if the
           'soc_feature_vlan_vfi_membership' is FALSE, return error. If the
           feature is TRUE, the mentioned field comes from the profiled
           'ING_VLAN_VFI_MEMBERSHIPm' table*/
        if (flags & (BCM_VLAN_PORT_INGRESS_ONLY | BCM_VLAN_PORT_EGRESS_ONLY)) {
            if (!soc_mem_field_valid(unit, VLAN_TABLE(unit), ING_PORT_BITMAPf) &&
                !(soc_feature(unit, soc_feature_vlan_vfi_membership))) {
                return BCM_E_PARAM;
            }
        }
        if ((!soc_feature(unit, soc_feature_vlan_egress_membership_l3_only)) &&
            (flags & BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY)) {
            return (BCM_E_UNAVAIL);
        }
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HURRICANE3_SUPPORT)
        if ((flags & BCM_VLAN_GPORT_ADD_EGRESS_ONLY) &&
            (flags & BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY)) {
            return BCM_E_PARAM;
        }
#endif /* BCM_TRIUMPH3_SUPPORT || BCM_HURRICANE3_SUPPORT */

        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, port, &local_port));

        if (flags & (BCM_VLAN_PORT_EGRESS_ONLY | BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY)) {
            BCM_PBMP_CLEAR(ing_pbmp);
        } else {
            BCM_PBMP_PORT_SET(ing_pbmp, local_port);
        }
        if (flags & BCM_VLAN_PORT_INGRESS_ONLY) {
            BCM_PBMP_CLEAR(pbmp);
        } else {
            BCM_PBMP_PORT_SET(pbmp, local_port);
        }
        if (flags & BCM_VLAN_PORT_UNTAGGED) {
            BCM_PBMP_PORT_SET(ubmp, local_port);
        } else {
            BCM_PBMP_CLEAR(ubmp);
        }
        if (SOC_IS_FBX(unit)) {
            BCM_LOCK(unit);
            rv = _bcm_vlan_port_add(unit, vlan, pbmp, ubmp, ing_pbmp, flags);
            BCM_UNLOCK(unit);
        } else {  /* don't support virtual port*/
            BCM_PBMP_OR(pbmp, ing_pbmp);
            rv = bcm_esw_vlan_port_add(unit, vlan, pbmp, ubmp);
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_vlan_gport_delete
 * Purpose:
 *      Delete a Gport from the specified vlan.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vlan - VLAN ID to add port to as a member.
 *      port - Gport ID
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 *      BCM_E_NOT_FOUND - VLAN ID not in use.
 */
int
bcm_esw_vlan_gport_delete(int unit, bcm_vlan_t vlan, bcm_gport_t port)
{
    int rv = BCM_E_NONE;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_vlan_gport_delete(unit, vlan, port);
    }
#endif

    CHECK_INIT(unit);

    /* VxLAN passes in a VFI, hence check VLAN for non-VxLan cases */
    BCM_IF_ERROR_RETURN(bcmi_esw_vlan_vfi_validate(unit, vlan));
    
    if (port == BCM_GPORT_INVALID)
        return BCM_E_PORT;

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_wlan) && BCM_GPORT_IS_WLAN_PORT(port)) {
        int vp, bc_idx,mc_idx,uuc_idx, gport_id, group = 0, mod_local;
        vlan_tab_entry_t vtab;
        bcm_wlan_port_t wlan_port;
        bcm_module_t mod_out;
        bcm_port_t port_out;
        bcm_trunk_t trunk_id;
        bcm_gport_t local_gport;
        bcm_if_t encap_id;

        vp = BCM_GPORT_WLAN_PORT_ID_GET(port);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
            return BCM_E_NOT_FOUND;
        }
        sal_memset(&vtab, 0, sizeof(vlan_tab_entry_t));

        soc_mem_lock(unit, VLAN_TABm);

        if ((rv = soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int)vlan, 
             &vtab)) < 0) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }
        if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VALIDf)) {
            soc_mem_unlock(unit, VLAN_TABm);
            return BCM_E_NOT_FOUND;
        }

        /* Get the VP attributes (need physical port) and encap id */
        if ((rv = bcm_esw_wlan_port_get(unit, port, &wlan_port)) < 0) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }
        /* Check if wlan_port.port is a local port or trunk */
        if (BCM_GPORT_IS_TRUNK(wlan_port.port)) {
            trunk_id = BCM_GPORT_TRUNK_GET(wlan_port.port);
            rv = _bcm_trunk_id_validate(unit, trunk_id);
            if (BCM_FAILURE(rv)) {
                  soc_mem_unlock(unit, VLAN_TABm);
                  return (BCM_E_PORT);
            }
            local_gport = wlan_port.port;
        } else {
            rv = _bcm_esw_gport_resolve(unit, wlan_port.port, &mod_out, &port_out,
                                &trunk_id, &gport_id); 
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return (BCM_E_PORT);
            }
            rv = _bcm_esw_modid_is_local(unit, mod_out, &mod_local);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }
            if (TRUE != mod_local) {
                /* Only add this to replication set if destination is local */
               soc_mem_unlock(unit, VLAN_TABm);
               return BCM_E_PORT;
            }
            /* Convert system local_port to physical local_port */
            if (soc_feature(unit, soc_feature_sysport_remap)) {
                BCM_XLATE_SYSPORT_S2P(unit, &port_out);
            }
            rv = bcm_esw_port_gport_get(unit, port_out, &local_gport);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }        
        }
        rv = bcm_esw_multicast_wlan_encap_get(unit, group, local_gport,
                                              port, &encap_id);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }

        /* Remove from the WLAN MC group */
        if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {
           if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
               /* No group exists - this is an error*/
               soc_mem_unlock(unit, VLAN_TABm);
               return BCM_E_PORT;
           }
        }

        bc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, BC_IDXf);
        _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_WLAN, bc_idx);
        rv = bcm_esw_multicast_egress_delete(unit, group, local_gport, 
                                              encap_id); 

        mc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, UMC_IDXf);
        if (mc_idx != bc_idx) {
            _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_WLAN, mc_idx);
            rv = bcm_esw_multicast_egress_delete(unit, group, local_gport, 
                                              encap_id);
        }

        uuc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, UUC_IDXf);
        if ((uuc_idx != bc_idx) && (uuc_idx != mc_idx)) {
            _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_WLAN, uuc_idx);
            rv = bcm_esw_multicast_egress_delete(unit, group, local_gport, 
                                              encap_id);
        }

        soc_mem_unlock(unit, VLAN_TABm);

    } else if (soc_feature(unit, soc_feature_vlan_vp) && BCM_GPORT_IS_VLAN_PORT(port)) {
#if defined(BCM_ENDURO_SUPPORT)
        if (SOC_IS_ENDURO(unit)) {
            rv = bcm_enduro_vlan_vp_delete(unit, vlan, port);
        } else 
#endif /* BCM_ENDURO_SUPPORT */
        {
            rv = bcm_tr2_vlan_gport_delete(unit, vlan, port);
        }
    } else if (soc_feature(unit, soc_feature_niv) &&
            BCM_GPORT_IS_NIV_PORT(port)) {
        rv = bcm_tr2_vlan_gport_delete(unit, vlan, port);
    } else if (soc_feature(unit, soc_feature_port_extension) &&
            BCM_GPORT_IS_EXTENDER_PORT(port)) {
        rv = bcm_tr2_vlan_gport_delete(unit, vlan, port);
    } else if (soc_feature(unit, soc_feature_vp_lag) &&
            BCM_GPORT_IS_TRUNK(port)) {
        rv = bcm_tr2_vlan_gport_delete(unit, vlan, port);
    } else if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
               (soc_feature(unit, soc_feature_channelized_switching))) &&
            BCM_GPORT_IS_SUBPORT_PORT(port)) {
        rv = bcm_tr2_vlan_gport_delete(unit, vlan, port);
    } else if (soc_feature(unit, soc_feature_vxlan) &&
            BCM_GPORT_IS_VXLAN_PORT(port)) {
        rv = bcm_tr2_vlan_gport_delete(unit, vlan, port);
    } else if (soc_feature(unit, soc_feature_flex_flow) &&
            BCM_GPORT_IS_FLOW_PORT(port)) {
        rv = bcm_tr2_vlan_gport_delete(unit, vlan, port);
    } else if (soc_feature(unit, soc_feature_l2gre) &&
            BCM_GPORT_IS_L2GRE_PORT(port)) {
        rv = bcm_tr2_vlan_gport_delete(unit, vlan, port);
    } else if (soc_feature(unit, soc_feature_mim) &&
                BCM_GPORT_IS_MIM_PORT(port)) {
        rv = bcm_tr2_vlan_gport_delete(unit, vlan, port);
    } else if (soc_feature(unit, soc_feature_mpls) &&
            BCM_GPORT_IS_MPLS_PORT(port)) {
        rv = bcm_tr2_vlan_gport_delete(unit, vlan, port);
    } else if (BCM_GPORT_IS_VP_GROUP(port)) {
#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3) 
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_vlan_vfi_membership)){
            rv = bcm_td2p_vlan_vp_group_set(unit, vlan,
                    FALSE, BCM_GPORT_VP_GROUP_GET(port), FALSE);
            BCM_IF_ERROR_RETURN(rv);

            rv = bcm_td2p_vlan_vp_group_set(unit, vlan,
                TRUE, BCM_GPORT_VP_GROUP_GET(port), FALSE);
            BCM_IF_ERROR_RETURN(rv);
        } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
        {
            if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership)) {
                rv = bcm_td_vlan_vp_group_set(unit,VLAN_TABLE(unit),vlan,
                        BCM_GPORT_VP_GROUP_GET(port),FALSE);
                BCM_IF_ERROR_RETURN(rv);
            }
            if (soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) {
                rv = bcm_td_vlan_vp_group_set(unit,EGR_VLANm,vlan,
                        BCM_GPORT_VP_GROUP_GET(port), FALSE);
                BCM_IF_ERROR_RETURN(rv);
            }
        }
#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */
    } else
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
    {
        bcm_pbmp_t pbmp;
        bcm_port_t local_port;
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
        uint32 filter_flags = 0;
#endif

        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, port, &local_port));
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_egr_vp_vlan_membership)) {
            if (_BCM_VPN_VFI_IS_SET(vlan) && BCM_GPORT_IS_MODPORT(port)) {
                BCM_IF_ERROR_RETURN(bcm_esw_port_vlan_member_get(unit, local_port, &filter_flags));
                if ((filter_flags & BCM_PORT_VLAN_MEMBER_EGRESS) &&
                            (filter_flags & BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP)) {
                    rv = bcm_tr2_vlan_gport_delete(unit, vlan, port);
                    return rv;
                }
            }
        }
#endif
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_vlan_vfi_membership) &&
            BCM_GPORT_IS_MODPORT(port)) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_vlan_member_get(unit, local_port, &filter_flags));
            if (filter_flags & BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP) {
                MOD_PORT_TO_GLP(unit, BCM_GPORT_MODPORT_MODID_GET(port),
                    BCM_GPORT_MODPORT_PORT_GET(port), port);
                rv = bcm_td2_vp_vlan_membership_delete(unit, port, vlan);
                return rv;
            }
        }
#endif
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_PORT_ADD(pbmp, local_port);
        rv = _bcm_vlan_port_remove(unit, vlan, pbmp, pbmp, pbmp, NULL);
    }
    return rv;
}

/*
 * Function:
 *      bcm_vlan_gport_extended_delete
 * Purpose:
 *      Delete a Gport from the specified vlan.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vlan - VLAN ID to add port to as a member.
 *      port - Gport ID
 *      flags -BCM_VLAN_PORT_XXX
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 *      BCM_E_NOT_FOUND - VLAN ID not in use.
 */
int
bcm_esw_vlan_gport_extended_delete(int unit, bcm_vlan_t vlan, 
                                      bcm_gport_t port, int flags)
{
    int rv = BCM_E_NONE;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return
            bcm_tomahawk3_vlan_gport_extended_delete(unit, vlan, port, flags);
    }
#endif

    CHECK_INIT(unit);
    BCM_IF_ERROR_RETURN(bcmi_esw_vlan_vfi_validate(unit, vlan));
    
    if (port == BCM_GPORT_INVALID)
        return BCM_E_PORT;

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_wlan) && BCM_GPORT_IS_WLAN_PORT(port)) {
        int vp, bc_idx,mc_idx,uuc_idx, gport_id, group = 0, mod_local;
        vlan_tab_entry_t vtab;
        bcm_wlan_port_t wlan_port;
        bcm_module_t mod_out;
        bcm_port_t port_out;
        bcm_trunk_t trunk_id;
        bcm_gport_t local_gport;
        bcm_if_t encap_id;

        vp = BCM_GPORT_WLAN_PORT_ID_GET(port);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
            return BCM_E_NOT_FOUND;
        }
        sal_memset(&vtab, 0, sizeof(vlan_tab_entry_t));

        soc_mem_lock(unit, VLAN_TABm);

        if ((rv = soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int)vlan, 
             &vtab)) < 0) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }
        if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VALIDf)) {
            soc_mem_unlock(unit, VLAN_TABm);
            return BCM_E_NOT_FOUND;
        }

        /* Get the VP attributes (need physical port) and encap id */
        if ((rv = bcm_esw_wlan_port_get(unit, port, &wlan_port)) < 0) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }
        /* Check if wlan_port.port is a local port or trunk */
        if (BCM_GPORT_IS_TRUNK(wlan_port.port)) {
            trunk_id = BCM_GPORT_TRUNK_GET(wlan_port.port);
            rv = _bcm_trunk_id_validate(unit, trunk_id);
            if (BCM_FAILURE(rv)) {
                  soc_mem_unlock(unit, VLAN_TABm);
                  return (BCM_E_PORT);
            }
            local_gport = wlan_port.port;
        } else {
            rv = _bcm_esw_gport_resolve(unit, wlan_port.port, &mod_out, &port_out,
                                &trunk_id, &gport_id); 
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return (BCM_E_PORT);
            }
            rv = _bcm_esw_modid_is_local(unit, mod_out, &mod_local);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }
            if (TRUE != mod_local) {
                /* Only add this to replication set if destination is local */
               soc_mem_unlock(unit, VLAN_TABm);
               return BCM_E_PORT;
            }
            /* Convert system local_port to physical local_port */
            if (soc_feature(unit, soc_feature_sysport_remap)) {
                BCM_XLATE_SYSPORT_S2P(unit, &port_out);
            }
            rv = bcm_esw_port_gport_get(unit, port_out, &local_gport);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }        
        }
        rv = bcm_esw_multicast_wlan_encap_get(unit, group, local_gport,
                                              port, &encap_id);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }

        /* Remove from the WLAN MC group */
        if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {
           if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
               /* No group exists - this is an error*/
               soc_mem_unlock(unit, VLAN_TABm);
               return BCM_E_PORT;
           }
        }

        bc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, BC_IDXf);
        _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_WLAN, bc_idx);
        rv = bcm_esw_multicast_egress_delete(unit, group, local_gport, 
                                              encap_id); 

        mc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, UMC_IDXf);
        if (mc_idx != bc_idx) {
            _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_WLAN, mc_idx);
            rv = bcm_esw_multicast_egress_delete(unit, group, local_gport, 
                                              encap_id);
        }

        uuc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, UUC_IDXf);
        if ((uuc_idx != bc_idx) && (uuc_idx != mc_idx)) {
            _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_WLAN, uuc_idx);
            rv = bcm_esw_multicast_egress_delete(unit, group, local_gport, 
                                              encap_id);
        }

        soc_mem_unlock(unit, VLAN_TABm);

    } else if (soc_feature(unit, soc_feature_vlan_vp) && BCM_GPORT_IS_VLAN_PORT(port)) {
#if defined(BCM_ENDURO_SUPPORT)
        if (SOC_IS_ENDURO(unit)) {
            rv = bcm_enduro_vlan_vp_delete(unit, vlan, port);
        } else 
#endif /* BCM_ENDURO_SUPPORT */
        {
            rv = bcm_tr2_vlan_gport_delete(unit, vlan, port);
        }
    } else if (soc_feature(unit, soc_feature_niv) &&
            BCM_GPORT_IS_NIV_PORT(port)) {
        rv = bcm_tr2_vlan_gport_delete(unit, vlan, port);
    } else if (soc_feature(unit, soc_feature_port_extension) &&
            BCM_GPORT_IS_EXTENDER_PORT(port)) {
        rv = bcm_tr2_vlan_gport_delete(unit, vlan, port);
    } else if (soc_feature(unit, soc_feature_vp_lag) &&
            BCM_GPORT_IS_TRUNK(port)) {
        rv = bcm_tr2_vlan_gport_delete(unit, vlan, port);
    } else if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
               (soc_feature(unit, soc_feature_channelized_switching))) &&
            BCM_GPORT_IS_SUBPORT_PORT(port)) {
        rv = bcm_tr2_vlan_gport_delete(unit, vlan, port);
    } else if (soc_feature(unit, soc_feature_vxlan) &&
               BCM_GPORT_IS_VXLAN_PORT(port)) {
        rv = bcm_tr2_vlan_gport_delete(unit, vlan, port);
    } else if (soc_feature(unit, soc_feature_flex_flow) &&
               BCM_GPORT_IS_FLOW_PORT(port)) {
        rv = bcm_tr2_vlan_gport_delete(unit, vlan, port);
    } else if (soc_feature(unit, soc_feature_l2gre) &&
               BCM_GPORT_IS_L2GRE_PORT(port)) {
        rv = bcm_tr2_vlan_gport_delete(unit, vlan, port);
    } else if (soc_feature(unit, soc_feature_mim) &&
               BCM_GPORT_IS_MIM_PORT(port)) {
        rv = bcm_tr2_vlan_gport_delete(unit, vlan, port);
    } else if (soc_feature(unit, soc_feature_mpls) &&
            BCM_GPORT_IS_MPLS_PORT(port)) {
        rv = bcm_tr2_vlan_gport_delete(unit, vlan, port);
    } else if (BCM_GPORT_IS_VP_GROUP(port)) {
#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3) 
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_vlan_vfi_membership)){
            rv = bcm_td2p_vlan_vp_group_set(unit, vlan,
                FALSE, BCM_GPORT_VP_GROUP_GET(port), FALSE);
            BCM_IF_ERROR_RETURN(rv);

            rv = bcm_td2p_vlan_vp_group_set(unit, vlan,
                TRUE, BCM_GPORT_VP_GROUP_GET(port), FALSE);
            BCM_IF_ERROR_RETURN(rv);
        } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
        {
            if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership)) {
                rv = bcm_td_vlan_vp_group_set(unit,VLAN_TABm,vlan,
                        BCM_GPORT_VP_GROUP_GET(port),FALSE);
                BCM_IF_ERROR_RETURN(rv);
            }
            if (soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) {
                rv = bcm_td_vlan_vp_group_set(unit,EGR_VLANm,vlan,
                        BCM_GPORT_VP_GROUP_GET(port), FALSE);
                BCM_IF_ERROR_RETURN(rv);
            }
        }
#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */
    } else
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
    {
        bcm_pbmp_t ing_pbmp, pbmp, ubmp;
        bcm_port_t local_port;
        int bc_idx, umc_idx, uuc_idx;
        int bc_do_not_update, umc_do_not_update, uuc_do_not_update;
        vlan_tab_entry_t vtab;
        soc_field_t group_types[3] = {0,0,0};

        /* If Ingress/Egress only flags are specified and 'ING_PORT_BITMAPf' is
           not a valid field in 'VLAN_TABm' AND if the
           'soc_feature_vlan_vfi_membership' is FALSE, return error. If the
           feature is TRUE, the mentioned field comes from the profiled
           'ING_VLAN_VFI_MEMBERSHIPm' table*/
        if (flags & (BCM_VLAN_PORT_INGRESS_ONLY | BCM_VLAN_PORT_EGRESS_ONLY)) {
            if (!soc_mem_field_valid(unit, VLAN_TABLE(unit), ING_PORT_BITMAPf) &&
                !(soc_feature(unit, soc_feature_vlan_vfi_membership))) {
                return BCM_E_PARAM;
            }
        }

        if (flags & BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY) {
            if (!soc_feature(unit, soc_feature_vlan_egress_membership_l3_only)) {
                return BCM_E_PARAM;
            }
        }

        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, port, &local_port));

        BCM_IF_ERROR_RETURN
            (READ_VLAN_TABm(unit, MEM_BLOCK_ANY, vlan, &vtab));

        if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {
            bc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, BC_IDXf);
            umc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, UMC_IDXf);
            uuc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, UUC_IDXf);

            if (flags & BCM_VLAN_GPORT_BCAST_DO_NOT_UPDATE) {
                bc_do_not_update = 1;
            } else {
                bc_do_not_update = 0;
            }
            if (flags & BCM_VLAN_GPORT_UNKNOWN_MCAST_DO_NOT_UPDATE) {
                umc_do_not_update = 1;
            } else {
                umc_do_not_update = 0;
            }
            if (flags & BCM_VLAN_GPORT_UNKNOWN_UCAST_DO_NOT_UPDATE) {
                uuc_do_not_update = 1;
            } else {
                uuc_do_not_update = 0;
            }

            if ((bc_idx == umc_idx) && (bc_do_not_update != umc_do_not_update)) {
                return BCM_E_PARAM;
            }
            if ((bc_idx == uuc_idx) && (bc_do_not_update != uuc_do_not_update)) {
                return BCM_E_PARAM;
            }
            if ((umc_idx == uuc_idx) && (umc_do_not_update != uuc_do_not_update)) {
                return BCM_E_PARAM;
            }

            if (!bc_do_not_update) {
                group_types[0] = BC_IDXf;
            }
            if ((!umc_do_not_update) && (bc_idx != umc_idx)) {
                group_types[1] = UMC_IDXf;
            }
            if ((!uuc_do_not_update) && (uuc_idx != umc_idx) && (uuc_idx != bc_idx)) {
                group_types[2] = UUC_IDXf;
            }
        }
        if (flags & 
            (BCM_VLAN_PORT_EGRESS_ONLY | BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY)) {
            BCM_PBMP_CLEAR(ing_pbmp);
        } else {
            BCM_PBMP_PORT_SET(ing_pbmp, local_port);
        }
        if (flags & BCM_VLAN_PORT_INGRESS_ONLY) {
            BCM_PBMP_CLEAR(pbmp);
        } else {
            BCM_PBMP_PORT_SET(pbmp, local_port);
        }
        if (flags & BCM_VLAN_PORT_UNTAGGED) {
            BCM_PBMP_PORT_SET(ubmp, local_port);
        } else {
            BCM_PBMP_CLEAR(ubmp);
        }
        rv = _bcm_vlan_port_remove(unit, vlan, pbmp, ubmp, ing_pbmp, group_types);
    }
    return rv;
}

/*
 * Function:
 *      bcm_vlan_gport_delete_all
 * Purpose:
 *      Delete all Gports from the specified vlan.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vlan - VLAN ID to add port to as a member.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 *      BCM_E_NOT_FOUND - VLAN ID not in use.
 */
int
bcm_esw_vlan_gport_delete_all(int unit, bcm_vlan_t vlan)
{
    int rv = BCM_E_NONE;
    bcm_pbmp_t pbmp, ubmp;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_vlan_gport_delete_all(unit, vlan);
    }
#endif

    CHECK_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vlan_vfi) &&
        _BCM_VPN_VFI_IS_SET(vlan)) {
        int vfi = 0;
        _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vlan);
        if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeVlan)) {
            return bcm_tr2_vlan_gport_delete_all(unit, vlan);
        }
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        if (_BCM_VPN_VFI_IS_SET(vlan)) {
            int vfi = 0;
            int vfi_max = soc_mem_index_max(unit, VFIm);

            _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vlan);
            if (vfi < 0 || vfi > vfi_max) {
                return (BCM_E_PARAM);
            }

            if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeAny)) {
                return BCM_E_NOT_FOUND;
            }
        } else {
            VLAN_CHK_ID(unit, vlan);
        }
         
        if (_BCM_VPN_VFI_IS_SET(vlan) || !SOC_IS_MONTEREY(unit)) {
            BCM_IF_ERROR_RETURN(bcm_td2_ing_vp_vlan_membership_delete_all(unit, vlan));
            BCM_IF_ERROR_RETURN(bcm_td2_egr_vp_vlan_membership_delete_all(unit, vlan));
        }
        if (_BCM_VPN_VFI_IS_SET(vlan)) {
            bcm_pbmp_t ing_pbmp;

            if (bcm_td2p_ing_vp_group_unmanaged_get(unit)) {
                BCM_IF_ERROR_RETURN(
                    bcm_td2p_vlan_vp_group_set(unit, vlan, FALSE, -1, FALSE));
            } else {
                BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_delete_all(unit, vlan, FALSE));
            }
            if (bcm_td2p_egr_vp_group_unmanaged_get(unit)) {
                BCM_IF_ERROR_RETURN(
                    bcm_td2p_vlan_vp_group_set(unit, vlan, TRUE, -1, FALSE));
            } else {
                BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_delete_all(unit, vlan, TRUE));
            }
            BCM_PBMP_CLEAR(pbmp);
            BCM_PBMP_CLEAR(ubmp);
            BCM_PBMP_CLEAR(ing_pbmp);
            BCM_IF_ERROR_RETURN(_bcm_vlan_port_get(unit, vlan, &pbmp, &ubmp,
                 &ing_pbmp));
            BCM_IF_ERROR_RETURN(_bcm_vlan_port_remove(unit, vlan, pbmp, ubmp,
                 ing_pbmp, NULL));

            return BCM_E_NONE;
        }
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

    VLAN_CHK_ID(unit, vlan);

    soc_mem_lock(unit, VLAN_TABm);

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)

    if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {

        int mc_idx,bc_idx,uuc_idx;
        bcm_multicast_t mc_group;
        vlan_tab_entry_t vtab;

        sal_memset(&vtab, 0, sizeof(vlan_tab_entry_t));

        if ((rv = soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int)vlan, 
             &vtab)) < 0) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }
        if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VALIDf)) {
            soc_mem_unlock(unit, VLAN_TABm);
            return BCM_E_NOT_FOUND;
        }

        if (soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
            /* Remove all VP members and delete the group */

            bc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, BC_IDXf);
            rv = _bcm_tr_multicast_ipmc_group_type_get(unit, bc_idx,
                                                            &mc_group);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }
            if (_BCM_MULTICAST_IS_WLAN(mc_group)) {

                rv = bcm_esw_multicast_egress_delete_all(unit, mc_group); 
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, VLAN_TABm);
                    return rv;
                }
                rv = bcm_esw_multicast_destroy(unit, mc_group);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, VLAN_TABm);
                    return rv;
                }

                mc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, UMC_IDXf);
                if (mc_idx != bc_idx) {
                    rv = _bcm_tr_multicast_ipmc_group_type_get(unit, mc_idx,
                                                              &mc_group);
                    if (BCM_FAILURE(rv)) {
                         soc_mem_unlock(unit, VLAN_TABm);
                         return rv;
                    }
                     
                    rv = bcm_esw_multicast_egress_delete_all(unit, mc_group); 
                    if (BCM_FAILURE(rv)) {
                        soc_mem_unlock(unit, VLAN_TABm);
                        return rv;
                    }
                    
                    rv = bcm_esw_multicast_destroy(unit, mc_group);
                    if (BCM_FAILURE(rv)) {
                        soc_mem_unlock(unit, VLAN_TABm);
                        return rv;
                    }
                }
                
                uuc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, UUC_IDXf);
                if ((bc_idx != uuc_idx) && (mc_idx != uuc_idx)) {
                    rv = _bcm_tr_multicast_ipmc_group_type_get(unit, uuc_idx,
                                                              &mc_group);
                    if (BCM_FAILURE(rv)) {
                        soc_mem_unlock(unit, VLAN_TABm);
                        return rv;
                    }
                    
                    rv = bcm_esw_multicast_egress_delete_all(unit, mc_group); 
                    if (BCM_FAILURE(rv)) {
                        soc_mem_unlock(unit, VLAN_TABm);
                        return rv;
                    }
                    rv = bcm_esw_multicast_destroy(unit, mc_group);
                    if (BCM_FAILURE(rv)) {
                        soc_mem_unlock(unit, VLAN_TABm);
                        return rv;
                    }
                }

                if (!SHR_BITGET(vlan_info[unit].vp_mode, vlan)) {
                    soc_mem_field32_set(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf, 0);
                }
                
                _bcm_xgs3_vlan_mcast_idx_set(unit, &vtab, BC_IDXf, 0);
                _bcm_xgs3_vlan_mcast_idx_set(unit, &vtab, UMC_IDXf, 0);
                _bcm_xgs3_vlan_mcast_idx_set(unit, &vtab, UUC_IDXf, 0);
                if ((rv = soc_mem_write(unit, VLAN_TABm, MEM_BLOCK_ALL,
                                (int)vlan, &vtab)) < 0) {
                    soc_mem_unlock(unit, VLAN_TABm);
                    return rv;
                }
            } else if (_BCM_MULTICAST_IS_VLAN(mc_group)) {
#if defined(BCM_ENDURO_SUPPORT)
                if (SOC_IS_ENDURO(unit)) {
                    rv = bcm_enduro_vlan_vp_delete_all(unit, vlan);
                } else 
#endif /* BCM_ENDURO_SUPPORT */
                {
                    rv = bcm_tr2_vlan_gport_delete_all(unit, vlan);
                }
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, VLAN_TABm);
                    return rv;
                }
            } else if (_BCM_MULTICAST_IS_NIV(mc_group) ||
                    _BCM_MULTICAST_IS_EXTENDER(mc_group)) {
                rv = bcm_tr2_vlan_gport_delete_all(unit, vlan);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, VLAN_TABm);
                    return rv;
                }
            } else {
                /* Invalid multicast group type */
                soc_mem_unlock(unit, VLAN_TABm);
                return BCM_E_INTERNAL;
            }

            /* check if it is vp group unmanaged mode */
#if defined(BCM_TRIDENT_SUPPORT) 
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(unit, soc_feature_vlan_vfi_membership)){
                if (bcm_td2p_ing_vp_group_unmanaged_get(unit)) {
                    /* clear the vp group bitmap for this vlan */
                    rv = bcm_td2p_vlan_vp_group_set(unit, vlan, FALSE, -1, FALSE);
                    if (rv != BCM_E_NONE) {
                        soc_mem_unlock(unit, VLAN_TABm);
                        return rv;
                    }
                 }
                if (bcm_td2p_egr_vp_group_unmanaged_get(unit)) {
                    /* clear the vp group bitmap for this vlan */
                    rv = bcm_td2p_vlan_vp_group_set(unit, vlan, TRUE, -1, FALSE);
                    if (rv != BCM_E_NONE) {
                        soc_mem_unlock(unit, VLAN_TABm);
                        return rv;
                    }
                }
            } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
            {
                if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership) &&
                        bcm_td_ing_vp_group_unmanaged_get(unit)) {
                    /* clear the vp group bitmap for this vlan */
                    rv = bcm_td_vlan_vp_group_set(unit,VLAN_TABLE(unit),vlan, -1,FALSE);
                    if (rv != BCM_E_NONE) {
                        soc_mem_unlock(unit, VLAN_TABm);
                        return rv;
                    }
                }
                if (soc_feature(unit, soc_feature_vp_group_egress_vlan_membership) &&
                        bcm_td_egr_vp_group_unmanaged_get(unit)) {
                    /* clear the vp group bitmap for this vlan */
                    rv = bcm_td_vlan_vp_group_set(unit,EGR_VLANm,vlan, -1,FALSE);
                    if (rv != BCM_E_NONE) {
                        soc_mem_unlock(unit, VLAN_TABm);
                        return rv;
                    }
                }
            }
#endif /* BCM_TRIDENT_SUPPORT */
        }
    } 
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */

    /* Now remove all local physical ports */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    rv = bcm_esw_vlan_port_get(unit, vlan, &pbmp, &ubmp);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, VLAN_TABm);
        return rv;
    }
    rv = bcm_esw_vlan_port_remove(unit, vlan, pbmp);
    soc_mem_unlock(unit, VLAN_TABm);

    return rv;
}

int
bcm_esw_vlan_gport_get(int unit, bcm_vlan_t vlan, bcm_gport_t port, 
                       int *flags)
{
    int rv = BCM_E_NONE;
    bcm_pbmp_t pbmp, ubmp, ing_pbmp;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_vlan_gport_get(unit, vlan, port, flags);
    }
#endif

    CHECK_INIT(unit);

    /* VFI types like VxLAN passes in a VFI and not VLAN, hence check one
       or the other */
    BCM_IF_ERROR_RETURN(bcmi_esw_vlan_vfi_validate(unit, vlan));
    
    if (port == BCM_GPORT_INVALID)
        return BCM_E_PORT;

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_wlan) && BCM_GPORT_IS_WLAN_PORT(port)) {
        int idx, vp, mc_idx, gport_id, group = 0, mod_local;
        vlan_tab_entry_t vtab;
        bcm_wlan_port_t wlan_port;
        bcm_module_t mod_out;
        bcm_port_t port_out=0;
        bcm_trunk_t trunk_id;
        bcm_port_t trunk_member_port[SOC_MAX_NUM_PORTS];
        bcm_gport_t local_gport;
        bcm_if_t encap_id;
        bcm_if_t *if_array = NULL;
        int alloc_size, if_count, if_cur, if_max, match, trunk_local_ports = 0;

        vp = BCM_GPORT_WLAN_PORT_ID_GET(port);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
            return BCM_E_NOT_FOUND;
        }
        sal_memset(&vtab, 0, sizeof(vlan_tab_entry_t));

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int)vlan, &vtab));

        if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VALIDf)) {
            return BCM_E_NOT_FOUND;
        }

        /* Get the VP attributes (need physical port) and encap id */
        BCM_IF_ERROR_RETURN(bcm_esw_wlan_port_get(unit, port, &wlan_port));
        /* Check if wlan_port.port is a local port or trunk */
        if (BCM_GPORT_IS_TRUNK(wlan_port.port)) {
            trunk_id = BCM_GPORT_TRUNK_GET(wlan_port.port);
            rv = _bcm_trunk_id_validate(unit, trunk_id);
            if (BCM_FAILURE(rv)) {
                  return (BCM_E_PORT);
            }
            rv = _bcm_esw_trunk_local_members_get(unit, trunk_id,
                    SOC_MAX_NUM_PORTS, trunk_member_port, &trunk_local_ports);
            if (BCM_FAILURE(rv)) {
                  return (BCM_E_PORT);
            }
            local_gport = wlan_port.port;
        } else {
            rv = _bcm_esw_gport_resolve(unit, wlan_port.port, &mod_out, &port_out,
                              &trunk_id, &gport_id); 
            if (BCM_FAILURE(rv)) {
                return (BCM_E_PORT);
            }
            rv = _bcm_esw_modid_is_local(unit, mod_out, &mod_local);
            BCM_IF_ERROR_RETURN(rv);
            if (TRUE != mod_local) {
                /* Only add this to replication set if destination is local */
               return BCM_E_PORT;
            }
            /* Convert system local_port to physical local_port */
            if (soc_feature(unit, soc_feature_sysport_remap)) {
                BCM_XLATE_SYSPORT_S2P(unit, &port_out);
            }
            rv = bcm_esw_port_gport_get(unit, port_out, &local_gport);
            if (BCM_FAILURE(rv)) {
                return rv;
            }        
        }
        BCM_IF_ERROR_RETURN
            (bcm_esw_multicast_wlan_encap_get(unit, group, local_gport,
                                              port, &encap_id));

        /* Search the WLAN MC group */
        if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {
            if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
               /* No group exists - so not found */
               return BCM_E_NOT_FOUND;
            }
        }
        mc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, BC_IDXf);
        _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_WLAN, mc_idx);

        /* If trunk, iterate over all local ports in the trunk and search for
         * a match on any local port */
        if_max = soc_mem_index_count(unit, EGR_L3_INTFm) + 
                 soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
        alloc_size = if_max * sizeof(int);
        if_array = sal_alloc(alloc_size, "temp IPMC repl interface array");
        if (if_array == NULL) {
            return BCM_E_MEMORY;
        }
        match = FALSE;
        if (BCM_GPORT_IS_TRUNK(wlan_port.port)) {
            for (idx = 0; idx < trunk_local_ports; idx++) {
                rv = bcm_esw_ipmc_egress_intf_get(unit, mc_idx, 
                                                  trunk_member_port[idx], 
                                                  if_max, if_array, &if_count);
                if (BCM_SUCCESS(rv)) {
                    for (if_cur = 0; if_cur < if_count; if_cur++) {
                        if (if_array[if_cur] == encap_id) {
                            match = TRUE;
                            *flags = 0;
                            break;
                        }
                    }
                    if (match) {
                        break;
                    }
                }
            }
        } else {
            rv = bcm_esw_ipmc_egress_intf_get(unit, mc_idx, port_out, if_max,
                                              if_array, &if_count);
            if (BCM_SUCCESS(rv)) {
                for (if_cur = 0; if_cur < if_count; if_cur++) {
                    if (if_array[if_cur] == encap_id) {
                        match = TRUE;
                        *flags = 0;
                        break;
                    }
                }
            }
        }
#ifdef BCM_TRIUMPH3_SUPPORT 
        /* Triumph3 WLAN axp port to match replication interface */
        if (SOC_IS_TRIUMPH3(unit) &&
           _BCM_MULTICAST_IS_WLAN(group)) {
                port_out = AXP_PORT(unit, SOC_AXP_NLF_WLAN_ENCAP);
                rv = bcm_esw_ipmc_egress_intf_get(unit, mc_idx, port_out, if_max,
                                          if_array, &if_count);
            if (BCM_SUCCESS(rv)) {
                match = FALSE;
                for (if_cur = 0; if_cur < if_count; if_cur++) {
                    if (if_array[if_cur] == encap_id) {
                        match = TRUE;
                        *flags = 0;
                        break;
                    }
                }

            }
        } 
#endif /* BCM_TRIUMPH3_SUPPORT */

        if (!match) {
            rv = BCM_E_NOT_FOUND;
        }
        sal_free(if_array);

    } else if (soc_feature(unit, soc_feature_vlan_vp) && BCM_GPORT_IS_VLAN_PORT(port)) {
#if defined(BCM_ENDURO_SUPPORT)
        if (SOC_IS_ENDURO(unit)) {
            rv = bcm_enduro_vlan_vp_get(unit, vlan, port, flags);
        } else 
#endif /* BCM_ENDURO_SUPPORT */
        {
            /* coverity[stack_use_callee] */
            /* coverity[stack_use_overflow] */
            rv = bcm_tr2_vlan_gport_get(unit, vlan, port, flags);
        }

    } else if (soc_feature(unit, soc_feature_niv) &&
            BCM_GPORT_IS_NIV_PORT(port)) {
        rv = bcm_tr2_vlan_gport_get(unit, vlan, port, flags);
    } else if (soc_feature(unit, soc_feature_port_extension) &&
            BCM_GPORT_IS_EXTENDER_PORT(port)) {
        rv = bcm_tr2_vlan_gport_get(unit, vlan, port, flags);
    } else if (soc_feature(unit, soc_feature_vp_lag) &&
            BCM_GPORT_IS_TRUNK(port)) {
        rv = bcm_tr2_vlan_gport_get(unit, vlan, port, flags);
    } else if (soc_feature(unit, soc_feature_vxlan) &&
            BCM_GPORT_IS_VXLAN_PORT(port)) {
        rv = bcm_tr2_vlan_gport_get(unit, vlan, port, flags);
    } else if (soc_feature(unit, soc_feature_flex_flow) &&
            BCM_GPORT_IS_FLOW_PORT(port)) {
        rv = bcm_tr2_vlan_gport_get(unit, vlan, port, flags);
    } else if (soc_feature(unit, soc_feature_l2gre) &&
            BCM_GPORT_IS_L2GRE_PORT(port)) {
        rv = bcm_tr2_vlan_gport_get(unit, vlan, port, flags);
    } else if (soc_feature(unit, soc_feature_mim) &&
            BCM_GPORT_IS_MIM_PORT(port)) {
        rv = bcm_tr2_vlan_gport_get(unit, vlan, port, flags);
    } else if (soc_feature(unit, soc_feature_mpls) &&
            BCM_GPORT_IS_MPLS_PORT(port)) {
        rv = bcm_tr2_vlan_gport_get(unit, vlan, port, flags);
    } else if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
               (soc_feature(unit, soc_feature_channelized_switching))) &&
            BCM_GPORT_IS_SUBPORT_PORT(port)) {
        rv = bcm_tr2_vlan_gport_get(unit, vlan, port, flags);
    } else if (BCM_GPORT_IS_VP_GROUP(port)) {
#if defined(BCM_TRIDENT_SUPPORT)
        int enable;
        int vpg = BCM_GPORT_VP_GROUP_GET(port);

        *flags=0;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
            bcm_stg_t stg;
            int ut_state;
            int hw_stp_state;

            BCM_IF_ERROR_RETURN(
                bcm_td2p_vlan_vp_group_get(unit, vlan, FALSE, vpg, &enable));
            if (enable) {
                *flags = BCM_VLAN_PORT_INGRESS_ONLY;

                BCM_IF_ERROR_RETURN(
                    bcm_td2p_vp_group_vlan_vpn_stg_get(unit, vlan, FALSE, &stg));
                BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_stp_state_get(
                                    unit, stg, port, FALSE, &hw_stp_state));
                *flags |= _bcm_td2_stp_state_to_flags(unit, hw_stp_state, FALSE);
            }

            BCM_IF_ERROR_RETURN(
                bcm_td2p_vlan_vp_group_get(unit, vlan, TRUE, vpg, &enable));
            if (enable) {
                *flags |= BCM_VLAN_PORT_EGRESS_ONLY;

                BCM_IF_ERROR_RETURN(
                    bcm_td2p_vp_group_vlan_vpn_stg_get(unit, vlan, TRUE, &stg));
                BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_stp_state_get(
                                    unit, stg, port, TRUE, &hw_stp_state));
                *flags |= _bcm_td2_stp_state_to_flags(unit, hw_stp_state, TRUE);

                if (!_BCM_VPN_VFI_IS_SET(vlan)) {
                    BCM_IF_ERROR_RETURN(
                        bcm_td2p_vp_group_ut_state_get(unit, port, vlan, &ut_state));
                    if (ut_state) {
                        *flags |= BCM_VLAN_GPORT_ADD_UNTAGGED;
                    }
                }
#if defined(BCM_TRIDENT3_SUPPORT)
                else if (SOC_MEM_IS_VALID(unit, EGR_VLAN_VFI_UNTAGm)) {
                    BCM_IF_ERROR_RETURN(
                        bcm_td2p_vp_group_ut_state_get(unit, port, vlan, &ut_state));
                    if (ut_state) {
                        *flags |= BCM_VLAN_GPORT_ADD_UNTAGGED;
                    }
                }
#endif
            }
        } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
        {
            if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership)) {
                BCM_IF_ERROR_RETURN(bcm_td_vlan_vp_group_get(
                            unit, VLAN_TABLE(unit), vlan, vpg, &enable));
                if (enable) {
                    *flags = BCM_VLAN_PORT_INGRESS_ONLY;
                }
            }
            if (soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) {
                BCM_IF_ERROR_RETURN(bcm_td_vlan_vp_group_get(
                            unit, EGR_VLANm, vlan,vpg, &enable));
                if (enable) {
                    *flags |= BCM_VLAN_PORT_EGRESS_ONLY;
                }
            }
        }
#endif /* BCM_TRIDENT_SUPPORT */

    } else
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */

    {
        bcm_port_t local_port;
        uint32 local_flags;
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
        uint32 filter_flags = 0;
#endif

        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, port, &local_port));
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_egr_vp_vlan_membership)) {
            if (_BCM_VPN_VFI_IS_SET(vlan) && BCM_GPORT_IS_MODPORT(port)) {
                BCM_IF_ERROR_RETURN(bcm_esw_port_vlan_member_get(unit, local_port, &filter_flags));
                if ((filter_flags & BCM_PORT_VLAN_MEMBER_EGRESS) &&
                            (filter_flags & BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP)) {
                    rv = bcm_tr2_vlan_gport_get(unit, vlan, port, flags);
                    return rv;
                }
            }
        }
#endif
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_vlan_vfi_membership) &&
            BCM_GPORT_IS_MODPORT(port)) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_vlan_member_get(unit, local_port, &filter_flags));
            if (filter_flags & BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP) {
                MOD_PORT_TO_GLP(unit, BCM_GPORT_MODPORT_MODID_GET(port),
                    BCM_GPORT_MODPORT_PORT_GET(port), port);
                rv = bcm_td2_vp_vlan_membership_get(unit, port, vlan, flags);
                return rv;
            }
        }
#endif
        /* Deal with local physical ports */
        rv = _bcm_vlan_port_get(unit, vlan, &pbmp, &ubmp, &ing_pbmp);
        local_flags = 0;
        if (!BCM_PBMP_MEMBER(pbmp, local_port)) {
            local_flags |= BCM_VLAN_PORT_INGRESS_ONLY;
        }
        if (!BCM_PBMP_MEMBER(ing_pbmp, local_port)) {
            local_flags |= BCM_VLAN_PORT_EGRESS_ONLY;
        }
        if (local_flags ==
            (BCM_VLAN_PORT_INGRESS_ONLY | BCM_VLAN_PORT_EGRESS_ONLY)) {
            return BCM_E_NOT_FOUND;
        }
        if (BCM_PBMP_MEMBER(ubmp, local_port)) {
            local_flags |= BCM_VLAN_PORT_UNTAGGED;
        }
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HURRICANE3_SUPPORT)
        if (soc_feature(unit, soc_feature_vlan_egress_membership_l3_only) &&
            (local_flags & BCM_VLAN_PORT_EGRESS_ONLY) && BCM_VLAN_VALID(vlan)) {
            bcm_pbmp_t vlan_tab_pbmp;
            BCM_IF_ERROR_RETURN
                (_bcm_xgs3_vlan_table_port_get(unit, vlan, &vlan_tab_pbmp,
                                                NULL, NULL, VLAN_TABLE(unit)));
            if (!BCM_PBMP_MEMBER(vlan_tab_pbmp, local_port)) {
                local_flags &= ~BCM_VLAN_PORT_EGRESS_ONLY;
                local_flags |= BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY;
            }
        }
#endif /* BCM_TRIUMPH3_SUPPORT || BCM_HURRICANE3_SUPPORT */
        *flags = local_flags;
    }

    return rv;
}

int
bcm_esw_vlan_gport_get_all(int unit, bcm_vlan_t vlan, int array_max, 
                           bcm_gport_t *gport_array, int *flags_array, 
                           int* array_size)
{
    bcm_pbmp_t          pbmp, ubmp, ing_pbmp, bmp;
    bcm_port_t          port, gport;
    bcm_module_t        modid;
    int                 port_cnt = 0, is_vfi = 0;
    _bcm_gport_dest_t   gport_dest; 
    vlan_tab_entry_t    vtab;
    uint32              local_flags;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return
            bcm_tomahawk3_vlan_gport_get_all(
                unit, vlan, array_max, gport_array, flags_array, array_size);
    }
#endif

    CHECK_INIT(unit);

    if (NULL == array_size || array_max < 0) {
        return BCM_E_PARAM;
    }

    if ((array_max > 0) && (NULL == gport_array)) {
        return BCM_E_PARAM;
    }

    /*
     * VFI types like VxLAN passes in a VFI and not VLAN,
     * hence check one or the other
     */
    BCM_IF_ERROR_RETURN(bcmi_esw_vlan_vfi_validate(unit, vlan));

    if(_BCM_VPN_VFI_IS_SET(vlan)) {
        is_vfi = 1;
    }

#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vlan_vfi) && is_vfi) {
        return bcm_tr2_vlan_gport_get_all(unit, vlan, array_max,
                            gport_array, flags_array, array_size);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    if(!is_vfi) {

        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int)vlan, &vtab));
        if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VALIDf)) {
            return BCM_E_NOT_FOUND;
        }

        port_cnt = 0;

        BCM_IF_ERROR_RETURN
            (_bcm_vlan_port_get(unit, vlan, &pbmp, &ubmp, &ing_pbmp));
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid));

        /* Prepare a superset of bitmaps to itterate over */
        BCM_PBMP_ASSIGN(bmp, pbmp);
        BCM_PBMP_OR(bmp, ubmp);
        BCM_PBMP_OR(bmp, ing_pbmp);

        BCM_PBMP_ITER(bmp, port) {
            if ((port_cnt == array_max) && (gport_array)) {
                break;
            }
            gport_dest.port = port;
            gport_dest.modid = modid;
            gport_dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_construct(unit, &gport_dest, &gport));
            local_flags = 0;
            if (!BCM_PBMP_MEMBER(pbmp, port)) {
                local_flags |= BCM_VLAN_PORT_INGRESS_ONLY;
            }
            if (!BCM_PBMP_MEMBER(ing_pbmp, port)) {
                local_flags |= BCM_VLAN_PORT_EGRESS_ONLY;
            }
            if (local_flags ==
                (BCM_VLAN_PORT_INGRESS_ONLY | BCM_VLAN_PORT_EGRESS_ONLY)) {
                continue;
            }
            if (flags_array) {
                if (BCM_PBMP_MEMBER(ubmp, port)) {
                    local_flags |= BCM_VLAN_PORT_UNTAGGED;
                }
                flags_array[port_cnt] = local_flags;
            }
            if (gport_array) {
                gport_array[port_cnt] = gport;
            }
            port_cnt++;
        }
    } /* !is_vfi */

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    /* Check existance of virtual ports */
    if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf) && !is_vfi) {
        int get_all_from_tr2 = 0;
        if (soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf) &&
            (port_cnt < array_max)) {
            int mc_idx;
            bcm_multicast_t mc_group;

            mc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, BC_IDXf);
            BCM_IF_ERROR_RETURN
                (_bcm_tr_multicast_ipmc_group_type_get(unit, mc_idx,
                                                       &mc_group));
            if (_BCM_MULTICAST_IS_WLAN(mc_group)) {
                int i, local_size;

                if (gport_array) {
                    BCM_IF_ERROR_RETURN(
                            bcm_esw_multicast_egress_get(unit, mc_group,
                                array_max - port_cnt, (gport_array + port_cnt),
                                NULL, &local_size));
                    for (i = 0; i < local_size; i++) {
                        if ((port_cnt == array_max) && (gport_array)) {
                            break;
                        }
                        if (flags_array) {
                            flags_array[port_cnt] = BCM_VLAN_PORT_UNTAGGED;
                        }
                        port_cnt++;
                    }
                }
            } else if (_BCM_MULTICAST_IS_VLAN(mc_group)) {
                if (gport_array) {
#if defined(BCM_ENDURO_SUPPORT)
                    if (SOC_IS_ENDURO(unit)) {
                        int local_size;
                        if (flags_array) {
                            BCM_IF_ERROR_RETURN
                                (bcm_enduro_vlan_vp_get_all
                                 (unit, vlan, array_max - port_cnt,
                                  &gport_array[port_cnt], &flags_array[port_cnt],
                                  &local_size));
                        } else {
                            BCM_IF_ERROR_RETURN
                                (bcm_enduro_vlan_vp_get_all
                                 (unit, vlan, array_max - port_cnt,
                                  &gport_array[port_cnt], NULL, &local_size));
                        }
                        port_cnt += local_size;
                    } else
#endif /* BCM_ENDURO_SUPPORT */
                    {
                        int local_size;
                        if (flags_array) {
                            BCM_IF_ERROR_RETURN
                                (bcm_tr2_vlan_gport_get_all
                                 (unit, vlan, array_max - port_cnt,
                                  &gport_array[port_cnt], &flags_array[port_cnt],
                                  &local_size));
                        } else {
                            BCM_IF_ERROR_RETURN
                                (bcm_tr2_vlan_gport_get_all
                                 (unit, vlan, array_max - port_cnt,
                                  &gport_array[port_cnt], NULL, &local_size));
                        }
                        port_cnt += local_size;
                    }
                }
                get_all_from_tr2 = 1;
            } else if (_BCM_MULTICAST_IS_NIV(mc_group) ||
                    _BCM_MULTICAST_IS_EXTENDER(mc_group)) {
                int local_size;
                if (gport_array) {
                    if (flags_array) {
                        BCM_IF_ERROR_RETURN
                            (bcm_tr2_vlan_gport_get_all
                             (unit, vlan, array_max - port_cnt,
                              &gport_array[port_cnt], &flags_array[port_cnt],
                              &local_size));
                    } else {
                        BCM_IF_ERROR_RETURN
                            (bcm_tr2_vlan_gport_get_all
                             (unit, vlan, array_max - port_cnt,
                              &gport_array[port_cnt], NULL, &local_size));
                    }
                    port_cnt += local_size;
                }
                get_all_from_tr2 = 1;
            } else {
                /* Unrecognized multicast type */
                return BCM_E_INTERNAL;
            }
        }

        if (!get_all_from_tr2 &&
                (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_TRIDENT3X(unit) ||
                 SOC_IS_APACHE(unit))) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            BCM_IF_ERROR_RETURN(
                bcm_td2p_vp_vlan_membership_get_all(unit, vlan,
                array_max, gport_array, flags_array, &port_cnt));
#endif
        }
    }
#if defined(BCM_TRIDENT_SUPPORT)
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (is_vfi) {
        if (soc_feature(unit, soc_feature_ing_vp_vlan_membership) ||
            soc_feature(unit, soc_feature_egr_vp_vlan_membership)) {
            BCM_IF_ERROR_RETURN(bcm_td2p_vp_vlan_membership_get_all(unit, vlan,
                        array_max, gport_array, flags_array, &port_cnt));
        }
    }

    if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        BCM_IF_ERROR_RETURN(bcm_td2p_vlan_vp_group_get_all(unit,vlan,
                    array_max, gport_array,flags_array,&port_cnt));
    } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT2(unit) ||
                SOC_IS_APACHE(unit)) {
        if (soc_feature(unit, soc_feature_ing_vp_vlan_membership) ||
            soc_feature(unit, soc_feature_egr_vp_vlan_membership)) {
            BCM_IF_ERROR_RETURN(bcm_td2_vp_vlan_membership_get_all(unit, vlan,
                        array_max, gport_array, flags_array, &port_cnt));
        }
    } else
#endif
    {
        BCM_IF_ERROR_RETURN(bcm_td_vlan_vp_group_get_all(unit,vlan,
                    array_max, gport_array,flags_array,&port_cnt));
    }
#endif /* BCM_TRIDENT_SUPPORT */
#endif /* BCM_TRIUMPH2_SUPPORT  && INCLUDE_L3 */

    /* If all attempts found nothing return not found */
    if (!port_cnt) {
        return BCM_E_NOT_FOUND;
    }

    *array_size = port_cnt;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_vlan_port_create
 * Purpose:
 *      Create a VLAN virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan_port - (IN/OUT) Pointer to a VLAN virtual port structure. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_esw_vlan_port_create(int unit,
                         bcm_vlan_port_t *vlan_port)
{
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vlan_vp)) {
#if defined(BCM_ENDURO_SUPPORT)
        if (SOC_IS_ENDURO(unit)) {
            return bcm_enduro_vlan_vp_create(unit, vlan_port);
        } else 
#endif /* BCM_ENDURO_SUPPORT */ 
        {
            return bcm_tr2_vlan_vp_create(unit, vlan_port);
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vlan_port_destroy
 * Purpose:
 *      Destroy a VLAN virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) VLAN virtual port GPORT ID. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_esw_vlan_port_destroy(int unit,
                         bcm_gport_t gport)
{
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vlan_vp)) {
#if defined(BCM_ENDURO_SUPPORT)
        if (SOC_IS_ENDURO(unit)) {
            return bcm_enduro_vlan_vp_destroy(unit, gport);
        } else 
#endif /* BCM_ENDURO_SUPPORT */
        {
            return bcm_tr2_vlan_vp_destroy(unit, gport);
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vlan_port_find
 * Purpose:
 *      Get VLAN virtual port info.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan_port - (IN/OUT) Pointer to VLAN virtual port structure. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_esw_vlan_port_find(int unit,
                       bcm_vlan_port_t *vlan_port)
{
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vlan_vp)) {
#if defined(BCM_ENDURO_SUPPORT)
        if (SOC_IS_ENDURO(unit)) {
            return bcm_enduro_vlan_vp_find(unit, vlan_port);
        } else 
#endif /* BCM_ENDURO_SUPPORT */        
        {
            return bcm_tr2_vlan_vp_find(unit, vlan_port);
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_esw_vlan_port_source_vp_lag_set
 * Purpose:
 *      Set source VP LAG for a VLAN virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) VLAN virtual port GPORT ID. 
 *      vp_lag_vp - (IN) VP representing the VP LAG. 
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_vlan_port_source_vp_lag_set(int unit, bcm_gport_t gport,
        int vp_lag_vp)
{
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vlan_vp) &&
            soc_feature(unit, soc_feature_vp_lag)) {
        return bcm_td2_vlan_vp_source_vp_lag_set(unit, gport, vp_lag_vp);
    }
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_esw_vlan_port_source_vp_lag_clear
 * Purpose:
 *      Clear source VP LAG for a VLAN virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) VLAN virtual port GPORT ID. 
 *      vp_lag_vp - (IN) VP representing the VP LAG. 
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_vlan_port_source_vp_lag_clear(int unit, bcm_gport_t gport,
        int vp_lag_vp)
{
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vlan_vp) &&
            soc_feature(unit, soc_feature_vp_lag)) {
        return bcm_td2_vlan_vp_source_vp_lag_clear(unit, gport, vp_lag_vp);
    }
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_esw_vlan_port_source_vp_lag_get
 * Purpose:
 *      Get source VP LAG for a VLAN virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) VLAN virtual port GPORT ID. 
 *      vp_lag_vp - (OUT) VP representing the VP LAG. 
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_vlan_port_source_vp_lag_get(int unit, bcm_gport_t gport,
        int *vp_lag_vp)
{
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vlan_vp) &&
            soc_feature(unit, soc_feature_vp_lag)) {
        return bcm_td2_vlan_vp_source_vp_lag_get(unit, gport, vp_lag_vp);
    }
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

#define VALID_VLAN_ID(vid)                                              \
    ((((vid) >= 0) && ((vid) <= BCM_VLAN_MAX)) || (vid == BCM_VLAN_INVALID))

/*
 * Function:
 *     _bcm_esw_vlan_system_reserved_set
 * Purpose:
 *     Set ingress or egress system reserved VLAN
 * Parameters:
 *     unit  - Device unit number
 *     arg   - VLAN
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_esw_vlan_system_reserved_set(int unit, int arg)
{
    int rv = BCM_E_UNAVAIL;
    if (VALID_VLAN_ID(arg)) {
#ifdef BCM_TRX_SUPPORT
        if (soc_feature(unit, soc_feature_system_reserved_vlan)) {
            rv = _bcm_trx_system_reserved_vlan_ing_set(unit, arg);
            if (SOC_SUCCESS(rv)) {
                rv = _bcm_trx_system_reserved_vlan_egr_set(unit, arg);
            }
        }
#endif
    } else {
        rv = BCM_E_PARAM;
    }
        
    return rv;
}

/*
 * Function:
 *     _bcm_esw_vlan_system_reserved_get
 * Purpose:
 *     Get ingress or egress system reserved VLAN
 * Parameters:
 *     unit  - Device unit number
 *     arg   - pointer to VLAN
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_esw_vlan_system_reserved_get(int unit, int *arg)
{
    int rv = BCM_E_UNAVAIL;

    if (arg != NULL) {
#ifdef BCM_TRX_SUPPORT
        if (soc_feature(unit, soc_feature_system_reserved_vlan)) {
            rv = _bcm_trx_system_reserved_vlan_ing_get(unit, arg);
        }
#endif
    } else {
        rv = BCM_E_PARAM;
    }
    return rv;
}

/*
 * Function:
 *     _bcm_vlan_count_get
 * Purpose:
 *     Retrieves the number of VLAN created in the system
 * Parameters:
 *     unit - Device unit number
 *     arg  - Vlan Count
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_vlan_count_get(int unit, int *arg)
{
    bcm_vlan_info_t    *vi = &vlan_info[unit];

    if (arg == NULL) {
        return (BCM_E_PARAM);
    }

    if (vi->init != TRUE) {
        return (BCM_E_INIT);
    }

    *arg = vi->count;
    return (BCM_E_NONE);
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * _bcm_vlan_reinit
 *
 * Recover VLAN state from hardware (reload)
 */
STATIC int
_bcm_vlan_reinit(int unit)
{
    bcm_vlan_info_t *vi = &vlan_info[unit];
    int rv;
    soc_scache_handle_t scache_handle;
    uint8 *vlan_scache_ptr;
    uint8  flood_mode;
    int    qm_num = 0;
    int    qm_bmp_size;
    int    i;
#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3)
    uint8 ing_vp_group_flag = 0;
    uint8 egr_vp_group_flag = 0;
#endif
    uint16 recovered_ver = BCM_WB_VERSION_1_0;
    int additional_scache_size = 0;
#ifdef BCM_TRX_SUPPORT
    int j;
    int range_max_idx = 0, range_alloc_size = 0;
#endif /* BCM_TRX_SUPPORT */

    /* Clean up existing vlan_info */
    vi->init = TRUE;
    BCM_IF_ERROR_RETURN
        (_bcm_vlan_cleanup(unit));

#ifdef BCM_MCAST_FLOOD_DEFAULT
    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_flood_default_set(unit, BCM_MCAST_FLOOD_DEFAULT));
#else
    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_flood_default_set(unit,
                                         BCM_VLAN_MCAST_FLOOD_UNKNOWN));
#endif

    /* VLAN Queue Map */
#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_vlan_queue_map)) {
        int    qm_size;

        if (vi->qm_bmp != NULL) {
            sal_free(vi->qm_bmp);
        }
        if (vi->qm_it_bmp != NULL) {
            sal_free(vi->qm_it_bmp);
        }

        qm_num = 1 << soc_mem_field_length(unit, VLAN_PROFILE_TABm,
                                           PHB2_DOT1P_MAPPING_PTRf);

        qm_size = SHR_BITALLOCSIZE(qm_num);
        vi->qm_bmp = sal_alloc(qm_size, "VLAN queue mapping bitmap");
        if (vi->qm_bmp == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(vi->qm_bmp, 0, qm_size); 
        vi->qm_it_bmp = sal_alloc(qm_size, "VLAN queue mapping bitmap");
        if (vi->qm_it_bmp == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(vi->qm_it_bmp, 0, qm_size); 
    }
#endif

    if (SOC_IS_SHADOW(unit)) { /* Shadow doesn't implement VLAN module */
        return BCM_E_NONE;
    }

    /*
     * Initialize software structures
     */
    vi->defl = BCM_VLAN_DEFAULT;
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        if (soc_feature(unit, soc_feature_vlan_translation_range)) {
            if (vi->vlan_range_no_act_array != NULL) {
                sal_free(vi->vlan_range_no_act_array);
            }
            range_max_idx = soc_mem_index_count(unit, ING_VLAN_RANGEm);
            range_alloc_size = sizeof(vlan_range_bk_t) * range_max_idx;
            vi->vlan_range_no_act_array = sal_alloc(range_alloc_size,
                                                    "VLAN range BK array");
            if (vi->vlan_range_no_act_array == NULL) {
                return BCM_E_MEMORY;
            }

            if (vi->vlan_range_inner_no_act_array != NULL) {
                sal_free(vi->vlan_range_inner_no_act_array);
            }
            range_alloc_size = sizeof(vlan_range_bk_t) * range_max_idx;
            vi->vlan_range_inner_no_act_array = sal_alloc(range_alloc_size,
                                                    "VLAN range BK array");
            if (vi->vlan_range_inner_no_act_array == NULL) {
                return BCM_E_MEMORY;
            }

            if (vi->vlan_range_no_act_ref_cnt_array != NULL) {
                sal_free(vi->vlan_range_no_act_ref_cnt_array);
            }

            range_alloc_size = sizeof(vlan_range_bk_ref_cnt_t) * range_max_idx;
            vi->vlan_range_no_act_ref_cnt_array =
                   sal_alloc(range_alloc_size, "VLAN range ref cnt array");
            if (vi->vlan_range_no_act_ref_cnt_array == NULL) {
                return BCM_E_MEMORY;
            }

            if (vi->vlan_range_inner_no_act_ref_cnt_array != NULL) {
                sal_free(vi->vlan_range_inner_no_act_ref_cnt_array);
            }
            range_alloc_size = sizeof(vlan_range_bk_ref_cnt_t) * range_max_idx;
            vi->vlan_range_inner_no_act_ref_cnt_array =
                sal_alloc(range_alloc_size, "VLAN range innr ref cnt array");
            if (vi->vlan_range_inner_no_act_ref_cnt_array == NULL) {
                return BCM_E_MEMORY;
            }
        }
    }
#endif /* BCM_TRX_SUPPORT */

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_VLAN, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &vlan_scache_ptr, 
                                 BCM_WB_DEFAULT_VERSION, &recovered_ver);
    if (rv == SOC_E_NOT_FOUND) {
        /* Use non-Level 2 case */
    } else if (SOC_FAILURE(rv)) {
        return rv;
    } else {
        /* Success, recover default VLAN */
        sal_memcpy(&vi->defl, vlan_scache_ptr, sizeof(vi->defl));
        vlan_scache_ptr += sizeof(vi->defl);

        /* VLAN XLATE OLD counter */
        sal_memcpy(&vi->old_egr_xlate_cnt, vlan_scache_ptr, 
                   sizeof(vi->old_egr_xlate_cnt));
        vlan_scache_ptr += sizeof(vi->old_egr_xlate_cnt);

        /* Flood mode */
        /* Type for 'flood_mode' is an enum, which values fit in uint8 */
        sal_memcpy(&flood_mode, vlan_scache_ptr, sizeof(flood_mode));
        vi->flood_mode = flood_mode;
        vlan_scache_ptr += sizeof(flood_mode);

        /* VLAN Queue Map */
        if (soc_feature(unit, soc_feature_vlan_queue_map)) {
            qm_bmp_size = _SHR_BITDCLSIZE(qm_num);
            for (i = 0; i < qm_bmp_size; i++) {
                sal_memcpy(&vi->qm_bmp[i], vlan_scache_ptr,
                           sizeof(SHR_BITDCL));
                vlan_scache_ptr += sizeof(SHR_BITDCL);
            }
            for (i = 0; i < qm_bmp_size; i++) {
                sal_memcpy(&vi->qm_it_bmp[i], vlan_scache_ptr,
                           sizeof(SHR_BITDCL));
                vlan_scache_ptr += sizeof(SHR_BITDCL);
            }
        }

#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3)
        /* recover vp group unmanaged flags */
        if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership) ||
            soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) {

            sal_memcpy(&ing_vp_group_flag, vlan_scache_ptr, 
                                     sizeof(ing_vp_group_flag));
            vlan_scache_ptr += sizeof(ing_vp_group_flag);
            sal_memcpy(&egr_vp_group_flag, vlan_scache_ptr, 
                                     sizeof(egr_vp_group_flag));
            vlan_scache_ptr += sizeof(egr_vp_group_flag);
        }
#endif

        if (recovered_ver >= BCM_WB_VERSION_1_1) {
            /* recover VLAN VP Multicast control mode */
            sal_memcpy(&vi->vp_mode, vlan_scache_ptr, 
                       sizeof(vi->vp_mode));
            vlan_scache_ptr += sizeof(vi->vp_mode);
        } else {
            additional_scache_size += sizeof(vi->vp_mode);
        }

#ifdef BCM_TRIDENT3_SUPPORT
        if (soc_feature(unit, soc_feature_pvlan_on_vfi)) {
            if (recovered_ver >= BCM_WB_VERSION_1_2) {
                sal_memcpy(&vi->shared_vlan_enable, vlan_scache_ptr,
                       sizeof(vi->shared_vlan_enable));
                vlan_scache_ptr += sizeof(vi->shared_vlan_enable);
            } else {
                uint64 val64 = 0;
                SOC_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &val64));
                vi->shared_vlan_enable =
                    soc_reg64_field32_get(unit, ING_CONFIG_64r, val64, SVL_ENABLEf);
                additional_scache_size += sizeof(vi->shared_vlan_enable);
            }
        }
#endif

#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(unit)) {
            if (soc_feature(unit, soc_feature_vlan_translation_range)) {
                if (recovered_ver >= BCM_WB_VERSION_1_3) {
                    range_alloc_size = sizeof(vlan_range_bk_t) * range_max_idx;
                    sal_memcpy(vi->vlan_range_no_act_array, vlan_scache_ptr,
                               range_alloc_size);
                    vlan_scache_ptr += range_alloc_size;
                } else {
                    for (i = 0; i < range_max_idx; i++) {
                        for (j = 0; j < BCM_VLAN_RANGE_NUM; j++) {
                            vi->vlan_range_no_act_array[i].vlan_low[j] =
                                BCM_VLAN_INVALID;
                        }
                    }
                    range_max_idx = soc_mem_index_count(unit, ING_VLAN_RANGEm);
                    range_alloc_size = sizeof(vlan_range_bk_t) * range_max_idx;
                    additional_scache_size += range_alloc_size;
                }

                if (recovered_ver >= BCM_WB_VERSION_1_4) {
                    range_max_idx = soc_mem_index_count(unit, ING_VLAN_RANGEm);
                    range_alloc_size = sizeof(vlan_range_bk_t) * range_max_idx;
                    sal_memcpy(vi->vlan_range_inner_no_act_array, vlan_scache_ptr,
                               range_alloc_size);
                    vlan_scache_ptr += range_alloc_size;

                    range_alloc_size = sizeof(vlan_range_bk_ref_cnt_t) * range_max_idx;
                    sal_memcpy(vi->vlan_range_no_act_ref_cnt_array, vlan_scache_ptr,
                               range_alloc_size);
                    vlan_scache_ptr += range_alloc_size;

                    sal_memcpy(vi->vlan_range_inner_no_act_ref_cnt_array, vlan_scache_ptr,
                               range_alloc_size);
                    vlan_scache_ptr += range_alloc_size;
                } else {
                    for (i = 0; i < range_max_idx; i++) {
                        for (j = 0; j < BCM_VLAN_RANGE_NUM; j++) {
                            vi->vlan_range_inner_no_act_array[i].vlan_low[j] =
                                BCM_VLAN_INVALID;
                        }
                        vi->vlan_range_no_act_ref_cnt_array[i].ref_cnt = 0;
                        vi->vlan_range_inner_no_act_ref_cnt_array[i].ref_cnt = 0;
                    }

                    range_max_idx = soc_mem_index_count(unit, ING_VLAN_RANGEm);
                    range_alloc_size = sizeof(vlan_range_bk_t) * range_max_idx;
                    additional_scache_size += range_alloc_size;
                    range_alloc_size = sizeof(vlan_range_bk_ref_cnt_t) * range_max_idx;
                    additional_scache_size += (range_alloc_size * 2);
                }
            }
        }
#endif /* BCM_TRX_SUPPORT */

        if (additional_scache_size > 0) {
            BCM_IF_ERROR_RETURN(
                soc_scache_realloc(unit, scache_handle, additional_scache_size));
        }
    }

    vi->count = 0;
    vi->init = TRUE;

    BCM_IF_ERROR_RETURN
        (_bcm_vbmp_init(&vi->bmp));
    BCM_IF_ERROR_RETURN(_bcm_vbmp_init(&vi->pre_cfg_bmp));

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_vlan_reload(unit, &vi->bmp, &vi->count));

    if (0 == _BCM_VBMP_LOOKUP(vi->bmp, vi->defl)) {
        LOG_WARN(BSL_LS_BCM_VLAN,
                 (BSL_META_U(unit,
                             "Warm Reboot: Default VLAN %4d does not exist!\n"),
                  vi->defl));
    }

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit) && !SOC_IS_TRX(unit)) {
        /* Recreate software state for vlan translation */
        
        /*
         * NOTE: Skipped for TRX.
         * VLAN translation for TRX happens in
         * _bcm_trx_vlan_action_profile_init().  This routine is
         * called by the PORT module.
         *
         * TRX uses its own data cache tables ing_action_profile[],
         * egr_action_profileuse[], rather than 'ing_trans', 'egr_trans' in
         * the common vlan_info[].
         */
        BCM_IF_ERROR_RETURN
            (_bcm_fb_vlan_translate_reload(unit, BCM_VLAN_XLATE_ING));
        BCM_IF_ERROR_RETURN
            (_bcm_fb_vlan_translate_reload(unit, BCM_VLAN_XLATE_EGR));
    }
#endif

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vlan_vp)) {
#ifdef BCM_ENDURO_SUPPORT
        if (SOC_IS_ENDURO(unit)) {
            BCM_IF_ERROR_RETURN(bcm_enduro_vlan_virtual_init(unit));
        } else
#endif /* BCM_ENDURO_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(bcm_tr2_vlan_virtual_init(unit));
        } 
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */

#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3) 
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        (void)bcm_td2p_ing_vp_group_unmanaged_set(unit, ing_vp_group_flag);
        (void)bcm_td2p_egr_vp_group_unmanaged_set(unit, egr_vp_group_flag);
        BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_init(unit));
    } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
    if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership) ||
        soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) {
        (void)bcm_td_ing_vp_group_unmanaged_set(unit,ing_vp_group_flag);
        (void)bcm_td_egr_vp_group_unmanaged_set(unit,egr_vp_group_flag);
        BCM_IF_ERROR_RETURN(bcm_td_vp_group_init(unit));
        
    }
#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_vlan_cleanup
 * Purpose:
 *      Free up resources without touching hardware
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
_bcm_vlan_cleanup(int unit)
{
    bcm_vlan_info_t *vi;

    CHECK_INIT(unit);

    BCM_LOCK(unit);
    vi = &vlan_info[unit];
    _bcm_vbmp_destroy(&vi->bmp);
    if (vi->qm_bmp != NULL) {
        sal_free(vi->qm_bmp);
    }
    if (vi->qm_it_bmp != NULL) {
        sal_free(vi->qm_it_bmp);
    }
    if (vi->egr_trans != NULL) {
        sal_free(vi->egr_trans);
    }
    if (vi->ing_trans != NULL) {
        sal_free(vi->ing_trans);
    }
    sal_memset(vi, 0, sizeof(bcm_vlan_info_t));
    BCM_UNLOCK(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_vlan_sync
 * Purpose:
 *      Record VLAN module persisitent info for Level 2 Warm Boot
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_esw_vlan_sync(int unit)
{
    bcm_vlan_info_t     *vi = &vlan_info[unit];
    soc_scache_handle_t scache_handle;
    uint8 *vlan_scache_ptr;
    uint8  flood_mode;
    int    qm_bmp_size;
    int    i;
#ifdef BCM_TRX_SUPPORT
    int    range_max_idx = 0, range_alloc_size = 0;
#endif /* BCM_TRX_SUPPORT */

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return _bcm_th3_vlan_sync(unit);
    }
#endif

    CHECK_INIT(unit);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_VLAN, 0);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &vlan_scache_ptr, 
                                 BCM_WB_DEFAULT_VERSION, NULL));
    /* Default VLAN */
    sal_memcpy(vlan_scache_ptr, &vi->defl, sizeof(vi->defl));
    vlan_scache_ptr += sizeof(vi->defl);

    /* VLAN XLATE OLD counter */
    sal_memcpy(vlan_scache_ptr, &vi->old_egr_xlate_cnt, 
               sizeof(vi->old_egr_xlate_cnt));
    vlan_scache_ptr += sizeof(vi->old_egr_xlate_cnt);

    /* Flood mode */
    /* Type for 'flood_mode' is an enum, which values fit in uint8 */
    flood_mode = vlan_info[unit].flood_mode;
    sal_memcpy(vlan_scache_ptr, &flood_mode, sizeof(flood_mode));
    vlan_scache_ptr += sizeof(flood_mode);

    /* VLAN Queue Map */
    if (soc_feature(unit, soc_feature_vlan_queue_map)) {
        qm_bmp_size = _SHR_BITDCLSIZE
            (1 << soc_mem_field_length(unit, VLAN_PROFILE_TABm,
                                       PHB2_DOT1P_MAPPING_PTRf));
        for (i = 0; i < qm_bmp_size; i++) {
            sal_memcpy(vlan_scache_ptr, &vi->qm_bmp[i], sizeof(SHR_BITDCL));
            vlan_scache_ptr += sizeof(SHR_BITDCL);
        }
        for (i = 0; i < qm_bmp_size; i++) {
            sal_memcpy(vlan_scache_ptr, &vi->qm_it_bmp[i], sizeof(SHR_BITDCL));
            vlan_scache_ptr += sizeof(SHR_BITDCL);
        }
    }

#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3) 
    /* vp group unmanaged flags */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        uint8 flag = 0;

        flag = (uint8)bcm_td2p_ing_vp_group_unmanaged_get(unit);
        sal_memcpy(vlan_scache_ptr, &flag, sizeof(flag));
        vlan_scache_ptr += sizeof(flag);
        flag = (uint8)bcm_td2p_egr_vp_group_unmanaged_get(unit);
        sal_memcpy(vlan_scache_ptr, &flag, sizeof(flag));
        vlan_scache_ptr += sizeof(flag);
    } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
    if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership) ||
        soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) {
        uint8 flag = 0;

        flag = (uint8)bcm_td_ing_vp_group_unmanaged_get(unit);
        sal_memcpy(vlan_scache_ptr, &flag, sizeof(flag));
        vlan_scache_ptr += sizeof(flag);
        flag = (uint8)bcm_td_egr_vp_group_unmanaged_get(unit);
        sal_memcpy(vlan_scache_ptr, &flag, sizeof(flag));
        vlan_scache_ptr += sizeof(flag);
    }
#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */

    if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_1) {
        /* sync VLAN info VP Multicast control mode */
        sal_memcpy(vlan_scache_ptr, &vi->vp_mode, 
                   sizeof(vi->vp_mode));
        vlan_scache_ptr += sizeof(vi->vp_mode);
    }

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_pvlan_on_vfi)) {
        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
            sal_memcpy(vlan_scache_ptr, &vi->shared_vlan_enable,
                   sizeof(vi->shared_vlan_enable));
            vlan_scache_ptr += sizeof(vi->shared_vlan_enable);
        }
    }
#endif

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        if (soc_feature(unit, soc_feature_vlan_translation_range)) {
            if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_3) {
                range_max_idx = soc_mem_index_count(unit, ING_VLAN_RANGEm);
                range_alloc_size = sizeof(vlan_range_bk_t) * range_max_idx;
                sal_memcpy(vlan_scache_ptr, vi->vlan_range_no_act_array,
                           range_alloc_size);
                vlan_scache_ptr += range_alloc_size;
            }

            if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_4) {
                range_max_idx = soc_mem_index_count(unit, ING_VLAN_RANGEm);
                range_alloc_size = sizeof(vlan_range_bk_t) * range_max_idx;
                sal_memcpy(vlan_scache_ptr, vi->vlan_range_inner_no_act_array,
                           range_alloc_size);
                vlan_scache_ptr += range_alloc_size;

                range_alloc_size = sizeof(vlan_range_bk_ref_cnt_t) * range_max_idx;
                sal_memcpy(vlan_scache_ptr, vi->vlan_range_no_act_ref_cnt_array,
                           range_alloc_size);
                vlan_scache_ptr += range_alloc_size;

                range_alloc_size = sizeof(vlan_range_bk_ref_cnt_t) * range_max_idx;
                sal_memcpy(vlan_scache_ptr, vi->vlan_range_inner_no_act_ref_cnt_array,
                           range_alloc_size);
                vlan_scache_ptr += range_alloc_size;
            }
        }
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */


#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     _bcm_vlan_compar
 * Purpose:
 *     Compares two VLAN IDs.  Used by sorting utility routine
 * Parameters:
 *     a - Pointer to first VLAN ID of type bcm_vlan_t to compare
 *     b - Pointer to second VLAN ID of type bcm_vlan_t to compare
 * Returns:
 *     0 If vlan1 == vlan2
 *    -1 If vlan1 <  vlan2
 *     1 If vlan1 >  vlan2
 */
int
_bcm_vlan_compar(void *a, void *b)
{
    bcm_vlan_t vlan1, vlan2;

    vlan1 = *(bcm_vlan_t *)a;
    vlan2 = *(bcm_vlan_t *)b;

    return ( (vlan1 < vlan2) ? -1 : ((vlan1 > vlan2) ? 1 : 0));
}

/*
 * Function:
 *     _bcm_vlan_sw_dump
 * Purpose:
 *     Displays VLAN software structure information.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_vlan_sw_dump(int unit)
{
    int             i;
    bcm_vlan_info_t *vi = &vlan_info[unit];
    int             vlan_count = 0;
#ifdef BCM_FIREBOLT_SUPPORT
    int             ixmem, index_min, count;
    soc_mem_t       mem;
    uint32          *vtcachep, vtcache;
#endif /* BCM_FIREBOLT_SUPPORT */

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information VLAN\n")));
    LOG_CLI((BSL_META_U(unit,
                        "  Init       : %4d\n"), vi->init));
    LOG_CLI((BSL_META_U(unit,
                        "  Default    : %4d\n"), vi->defl));
    LOG_CLI((BSL_META_U(unit,
                        "  Flood Mode : %4d\n"), vi->flood_mode));
    LOG_CLI((BSL_META_U(unit,
                        "  Count      : %4d\n"), vi->count));

    /* Print VLAN list sorted */
    LOG_CLI((BSL_META_U(unit,
                        "  List       :")));
    for (i = BCM_VLAN_MIN; i < BCM_VLAN_COUNT; i++) {
        if (0 == _BCM_VBMP_LOOKUP(vi->bmp, i)){
            continue;
        }
        if ((vlan_count > 0) && (!(vlan_count % 10))) {
            LOG_CLI((BSL_META_U(unit,
                                "\n              ")));
        }
        LOG_CLI((BSL_META_U(unit,
                            " %4d"), i));
        vlan_count++;
        if (vi->count == vlan_count) {
            break;
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

#ifdef BCM_FIREBOLT_SUPPORT
    if (soc_feature(unit, soc_feature_vlan_translation)) {
        LOG_CLI((BSL_META_U(unit,
                            "\n  VLAN translation       :")));
        
        for (ixmem = 0; ixmem < 2; ixmem++) {
            if (ixmem == 1) {
                mem = EGR_VLAN_XLATEm;
                vtcachep = vlan_info[unit].egr_trans;
                if (vtcachep == NULL) {
                    continue;
                }
                LOG_CLI((BSL_META_U(unit,
                                    "\n\n  Egress       :")));
            } else {
                mem = VLAN_XLATEm;
                if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
                    mem = VLAN_XLATE_1_DOUBLEm;
                }
                vtcachep = vlan_info[unit].ing_trans;
                if (vtcachep == NULL) {
                    continue;
                }
                LOG_CLI((BSL_META_U(unit,
                                    "\n  Ingress       :")));
            }

            if (!SOC_MEM_IS_VALID(unit, mem)) {
                continue;
            }

            index_min = soc_mem_index_min(unit, mem);
            count = soc_mem_index_count(unit, mem);
            for (i = 0; i < count; i++) {
                vtcache = vtcachep[index_min + i];
                if (BCM_VTCACHE_VALID_GET(vtcache)) {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n    Entry %d:  Port %s, VLAN 0x%03x%s"),
                             i,
                             SOC_PORT_NAME(unit, BCM_VTCACHE_PORT_GET(vtcache)),
                             BCM_VTCACHE_VID_GET(vtcache),
                             BCM_VTCACHE_ADD_GET(vtcache) ? ", Add" : ""));
                }
            }
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }
#endif /* BCM_FIREBOLT_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
    if (SOC_IS_FIREBOLT2(unit) || SOC_IS_TRX(unit) ||
        SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
        _bcm_fb2_vlan_profile_sw_dump(unit);
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAPTOR_SUPPORT */ 

#if defined(BCM_TRX_SUPPORT)
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        _bcm_th3_vlan_sw_tag_action_profile_dump(unit);
    } else
#endif
    if (SOC_IS_TRX(unit)) {
        _bcm_trx_vlan_sw_dump(unit);
    }
#endif /* BCM_TRX_SUPPORT */

    /* VLAN Queue Map */
    if (soc_feature(unit, soc_feature_vlan_queue_map)) {
        int qm_num;
        int count = 0;
        int inner;

        LOG_CLI((BSL_META_U(unit,
                            "\nSW Information VLAN Queue Map\n")));
        qm_num = 1 << soc_mem_field_length(unit, VLAN_PROFILE_TABm,
                                           PHB2_DOT1P_MAPPING_PTRf);

        if (vi->qm_bmp != NULL) {
            for (i = 0; i < qm_num; i++) {
                if (!SHR_BITGET(vlan_info[unit].qm_bmp, i)) {
                    continue;
                }
                if ((count > 0) && (!(count % 10))) {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n              ")));
                }
                if (!SHR_BITGET(vlan_info[unit].qm_it_bmp, i)) {
                    inner = 0;
                } else {
                    inner = 1;
                }
                LOG_CLI((BSL_META_U(unit,
                                    " %2d(%1d)"), i, inner));
                count++;
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
        }
    }

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vlan_vp)) {
#ifdef BCM_ENDURO_SUPPORT
        if (SOC_IS_ENDURO(unit)) {
            bcm_enduro_vlan_vp_sw_dump(unit);
        } else
#endif /* BCM_ENDURO_SUPPORT */
        {
            bcm_tr2_vlan_vp_sw_dump(unit);
        } 
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */

#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3)
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_vfi_membership)){
        bcm_td2p_vp_group_sw_dump(unit);
    } else
#endif
    if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership) ||
            soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) {
        bcm_td_vp_group_sw_dump(unit);
    }
#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#if defined(INCLUDE_L3)
/*
 * Function:
 *      bcm_esw_vlan_vpn_create
 * Purpose:
 *      Create a VPN instance
 * Parameters:
 *      unit  - (IN)  Device Number
 *      info  - (IN/OUT) VPN configuration info
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_vpn_create(int unit, bcm_vlan_vpn_config_t *info)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    int rv = BCM_E_PARAM;
    vfi_entry_t vfi_entry;
    int vfi_index = -1;
    int bc_group = 0, umc_group = 0, uuc_group = 0;
    int bc_group_type = 0, umc_group_type = 0, uuc_group_type = 0;

    if (!soc_feature(unit, soc_feature_vlan_vfi)) {
        return BCM_E_UNAVAIL;
    }

    if (info == NULL) {
        return BCM_E_PARAM;
    }

    if (!(info->flags & (BCM_VLAN_VPN_ELINE | BCM_VLAN_VPN_ELAN))) {
        return BCM_E_PARAM;
    }

    /*Allocate VFI*/
    if (info->flags & BCM_VLAN_VPN_WITH_ID) {
        rv = _bcm_tr3_vlan_vpn_is_valid(unit, info->vpn);
        if (BCM_E_NONE == rv) {
            return BCM_E_EXISTS;
        } else if (BCM_E_NOT_FOUND != rv) {
            return rv;
        }

        _BCM_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, info->vpn);
        BCM_IF_ERROR_RETURN(_bcm_vfi_alloc_with_id(unit, VFIm, _bcmVfiTypeVlan, vfi_index));
    } else {
        BCM_IF_ERROR_RETURN(_bcm_vfi_alloc(unit, VFIm, _bcmVfiTypeVlan, &vfi_index));
        _BCM_VPN_SET(info->vpn, _BCM_VPN_TYPE_VFI, vfi_index);
    }

    /*Initial and configure VFI*/
    BCM_IF_ERROR_RETURN(READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));
    if (info->flags & BCM_VLAN_VPN_ELINE) {
        soc_VFIm_field32_set(unit, &vfi_entry, PT2PT_ENf, 0x1);
    } else if (info->flags & BCM_VLAN_VPN_ELAN) {
        /* Check that the groups are valid. */
        bc_group_type = _BCM_MULTICAST_TYPE_GET(info->broadcast_group);
        bc_group = _BCM_MULTICAST_ID_GET(info->broadcast_group);
        umc_group_type = _BCM_MULTICAST_TYPE_GET(info->unknown_multicast_group);
        umc_group = _BCM_MULTICAST_ID_GET(info->unknown_multicast_group);
        uuc_group_type = _BCM_MULTICAST_TYPE_GET(info->unknown_unicast_group);
        uuc_group = _BCM_MULTICAST_ID_GET(info->unknown_unicast_group);

        if ((bc_group_type != _BCM_MULTICAST_TYPE_VLAN) ||
            (umc_group_type != _BCM_MULTICAST_TYPE_VLAN) ||
            (uuc_group_type != _BCM_MULTICAST_TYPE_VLAN) ||
            (bc_group >= soc_mem_index_count(unit, L3_IPMCm)) ||
            (umc_group >= soc_mem_index_count(unit, L3_IPMCm)) ||
            (uuc_group >= soc_mem_index_count(unit, L3_IPMCm))) {
            (void) _bcm_vfi_free(unit, _bcmVfiTypeVlan, vfi_index);
            return BCM_E_PARAM;
        }

        /* Commit the entry to HW */
        soc_VFIm_field32_set(unit, &vfi_entry, UMC_INDEXf, umc_group);
        soc_VFIm_field32_set(unit, &vfi_entry, UUC_INDEXf, uuc_group);
        soc_VFIm_field32_set(unit, &vfi_entry, BC_INDEXf, bc_group);
    }

    /*Write VFI to ASIC*/
    rv = WRITE_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry);
    if (rv < 0) {
        (void) _bcm_vfi_free(unit, _bcmVfiTypeVlan, vfi_index);
    }

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif /* BCM_TRIUMPH3_SUPPORT */
}

/*
 * Function:
 *              bcm_esw_vlan_vpn_destroy
 * Purpose:
 *              Delete a VPN instance
 * Parameters:
 *              unit - Device Number
 *              vpn - VPN instance ID
 * Returns:
 *              BCM_E_XXX
 */
int bcm_esw_vlan_vpn_destroy(int unit, bcm_vpn_t vpn)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    int vfi = 0;

    if (!soc_feature(unit, soc_feature_vlan_vfi)) {
        return BCM_E_UNAVAIL;
    }

    /* Get vfi index */
    BCM_IF_ERROR_RETURN(_bcm_tr3_vlan_vpn_is_valid(unit, vpn));
    _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);

    /* Delete all VLAN ports on this VPN */
    BCM_IF_ERROR_RETURN(_bcm_tr3_vlan_vp_destroy_all(unit, vpn));

    /* Reset VFI table */
    (void) _bcm_vfi_free(unit, _bcmVfiTypeVlan, vfi);

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif /* BCM_TRIUMPH3_SUPPORT */
}

/* Function:
*          bcm_esw_vlan_vpn_destroy_all
* Purpose:
*          Delete all VPN instances
* Parameters:
*          unit         - Device Number
* Returns:
*          BCM_E_XXXX
*/
int bcm_esw_vlan_vpn_destroy_all(int unit)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    int num_vfi = 0, idx = 0;
    bcm_vpn_t vpn = 0;

    if (!soc_feature(unit, soc_feature_vlan_vfi)) {
        return BCM_E_UNAVAIL;
    }

    /* Destroy all VLAN VPNs */
    num_vfi = soc_mem_index_count(unit, VFIm);
    for (idx = 0; idx < num_vfi; idx++) {
        if (_bcm_vfi_used_get(unit, idx, _bcmVfiTypeVlan)) {
            _BCM_VPN_SET(vpn, _BCM_VPN_TYPE_VFI, idx);
            BCM_IF_ERROR_RETURN(bcm_esw_vlan_vpn_destroy(unit, vpn));
        }
    }

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif /* BCM_TRIUMPH3_SUPPORT */
}

/* Function:
*          bcm_esw_vlan_vpn_get
* Purpose:
*          Get VPN information
* Parameters:
*          unit   - (IN) Device Number
*          vpn    - (IN) VLAN VPN
*          info   - (OUT) VLAN VPN Config
* Returns:
*          BCM_E_XXXX
*/
int
bcm_esw_vlan_vpn_get(int unit, bcm_vpn_t vpn, bcm_vlan_vpn_config_t *info)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    int vfi_index = -1;
    vfi_entry_t vfi_entry;
    uint8 isEline=0;

    if (!soc_feature(unit, soc_feature_vlan_vfi)) {
        return BCM_E_UNAVAIL;
    }

    if (info == NULL) {
        return BCM_E_PARAM;
    }

    /*Get vfi table*/
    BCM_IF_ERROR_RETURN(_bcm_tr3_vlan_vpn_is_eline(unit, vpn, &isEline));
    _BCM_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, vpn);
    BCM_IF_ERROR_RETURN(READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    /* Get info associated with vfi table */
    if (isEline) {
        info->flags |= BCM_VLAN_VPN_ELINE;
    } else {
        info->flags |= BCM_VLAN_VPN_ELAN;

        _BCM_MULTICAST_GROUP_SET(info->broadcast_group,
                 _BCM_MULTICAST_TYPE_VLAN,
                 soc_VFIm_field32_get(unit, &vfi_entry, BC_INDEXf));
        _BCM_MULTICAST_GROUP_SET(info->unknown_unicast_group,
                 _BCM_MULTICAST_TYPE_VLAN,
                 soc_VFIm_field32_get(unit, &vfi_entry, UUC_INDEXf));
        _BCM_MULTICAST_GROUP_SET(info->unknown_multicast_group,
                 _BCM_MULTICAST_TYPE_VLAN,
                 soc_VFIm_field32_get(unit, &vfi_entry, UMC_INDEXf));
    }

    _BCM_VPN_SET(info->vpn, _BCM_VPN_TYPE_VFI, vfi_index);

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif /* BCM_TRIUMPH3_SUPPORT */
}

/*
 * Function:
 *      bcm_esw_vlan_vpn_traverse
 * Purpose:
 *      VPN instance Traverse
 * Parameters:
 *      unit  - (IN)  Device Number
 *      cb    - (IN)  User-provided callback
 *      user_data  - (IN/OUT) Cookie
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_vpn_traverse(int unit, bcm_vlan_vpn_traverse_cb cb,
                             void *user_data)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    int idx, num_vfi;
    int vpn;
    bcm_vlan_vpn_config_t info;

    if (!soc_feature(unit, soc_feature_vlan_vfi)) {
        return BCM_E_UNAVAIL;
    }

    if (cb == NULL) {
        return BCM_E_PARAM;
    }

    /* VLAN VPNs */
    num_vfi = soc_mem_index_count(unit, VFIm);
    for (idx = 0; idx < num_vfi; idx++) {
        if (_bcm_vfi_used_get(unit, idx, _bcmVfiTypeVlan)) {
            _BCM_VPN_SET(vpn, _BCM_VPN_TYPE_VFI, idx);
            bcm_vlan_vpn_config_t_init(&info);
            BCM_IF_ERROR_RETURN(bcm_esw_vlan_vpn_get(unit, vpn, &info));
            BCM_IF_ERROR_RETURN(cb(unit, &info, user_data));
        }
    }

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif /* BCM_TRIUMPH3_SUPPORT */
}
#endif

/*
 * Function:
 *      bcm_vlan_match_action_add
 * Purpose:
 *      Add action for match criteria for Vlan assignment.
 * Parameters:
 *      unit  - (IN)  Device Number
 *      options - (IN) Option from BCM_VLAN_MATCH_ACTION_XXX
 *      match_info - (IN) Match criteria info
 *      action_set - (IN) Action Set.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_match_action_add(int unit, uint32 options,
                          bcm_vlan_match_info_t *match_info,
                          bcm_vlan_action_set_t *action_set)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        if (!soc_feature(unit, soc_feature_vlan_action)
                || !soc_feature(unit, soc_feature_mac_based_vlan)) {
            return BCM_E_UNAVAIL;
        }
        return _bcm_trx_vlan_match_action_add(unit, options,
                BCM_GPORT_INVALID, match_info, action_set);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vlan_match_action_get
 * Purpose:
 *      Get action for given match criteria.
 * Parameters:
 *      unit  - (IN)  Device Number
 *      match_info - (IN) Match criteria info
 *      action_set - (IN) Action Set.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_match_action_get(int unit, bcm_vlan_match_info_t *match_info,
                          bcm_vlan_action_set_t *action_set)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        if (!soc_feature(unit, soc_feature_vlan_action)
                || !soc_feature(unit, soc_feature_mac_based_vlan)) {
            return BCM_E_UNAVAIL;
        }
        return _bcm_trx_vlan_match_action_get(unit,
                BCM_GPORT_INVALID, match_info, action_set);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vlan_match_action_multi_get
 * Purpose:
 *      Get all actions and match criteria for given vlan match.
 * Parameters:
 *      unit  - (IN)  Device Number
 *      match - (IN) vlan match.
 *      size  - (IN) Number of elements in match info array.
 *      match_info_array - (OUT) Match criteria info array.
 *      action_set_array - (OUT) Action Set array.
 *      count - (OUT) Valid Match criteria info count.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_match_action_multi_get(int unit, bcm_vlan_match_t match,
                             int size, bcm_vlan_match_info_t *match_info_array,
                             bcm_vlan_action_set_t *action_set, int *count)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        if (!soc_feature(unit, soc_feature_vlan_action)
                || !soc_feature(unit, soc_feature_mac_based_vlan)) {
            return BCM_E_UNAVAIL;
        }
        return _bcm_trx_vlan_match_action_multi_get(unit, match,
                BCM_GPORT_INVALID, size, match_info_array,
                action_set, count);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vlan_match_action_delete
 * Purpose:
 *      Remove action for a match criteria
 * Parameters:
 *      unit  - (IN)  Device Number
 *      match_info - (IN) Match criteria info.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_match_action_delete(int unit, bcm_vlan_match_info_t *match_info)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        if (!soc_feature(unit, soc_feature_vlan_action)
                || !soc_feature(unit, soc_feature_mac_based_vlan)) {
            return BCM_E_UNAVAIL;
        }
        return _bcm_trx_vlan_match_action_delete(unit,
                BCM_GPORT_INVALID, match_info);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vlan_match_action_delete_all
 * Purpose:
 *      Remove all actions for vlan match.
 * Parameters:
 *      unit  - (IN)  Device Number
 *      match - (IN) Vlan Match
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_match_action_delete_all(int unit, bcm_vlan_match_t match)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        if (!soc_feature(unit, soc_feature_vlan_action)
                || !soc_feature(unit, soc_feature_mac_based_vlan)) {
            return BCM_E_UNAVAIL;
        }
        return _bcm_trx_vlan_match_action_delete_all(unit,
                match, BCM_GPORT_INVALID);
    }
#endif

    return BCM_E_UNAVAIL;
}


#ifdef BCM_TRX_SUPPORT
/*
 * Function:
 *      _bcm_vlan_match_action_multi_get
 * Purpose:
 *      Get all action and match criteria for given vlan match/port.
 * Parameters:
 *      unit       (IN)  Device Number
 *      port       (IN)  gport number
 *      size       (IN)  Number of elements in match_info_array
 *      match_array (OUT)  match info array
 *      start      (IN) Start index in match_array to be filled.
 *      count      (OUT) Actual number of elements in match_array
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_vlan_match_action_multi_get(int unit, bcm_gport_t port, int size,
        bcm_port_match_info_t *match_array, int start, int *count)
{
    int rv, i;
    int ctr;
    int valid_count = 0;
    bcm_vlan_match_t match;
    bcm_vlan_match_info_t *vlan_match_array = NULL;
    bcm_vlan_action_set_t *action_set = NULL;

    /* Proceed only if VLAN_MACx is present */
    if (!SOC_MEM_IS_VALID(unit, VLAN_MACm)) {
        return BCM_E_NONE;
    }

    if (size == 0) {
        match = BCM_VLAN_MATCH_INVALID;
        rv = _bcm_trx_vlan_match_action_multi_get(unit, match,
                port, size, NULL, NULL, &valid_count);
        BCM_IF_ERROR_RETURN(rv);

        *count = (start + valid_count);
        return BCM_E_NONE;
    }

    /* Allocate mem */
    vlan_match_array = sal_alloc(sizeof(bcm_vlan_match_info_t) * size,
            "Vlan match info array");
    if (vlan_match_array == NULL) {
        return BCM_E_MEMORY;
    }
    action_set = sal_alloc(sizeof (bcm_vlan_action_set_t) * size,
            "Vlan action set");
    if (action_set == NULL) {
        sal_free(vlan_match_array);
        return BCM_E_MEMORY;
    }

    match = BCM_VLAN_MATCH_INVALID;
    rv = _bcm_trx_vlan_match_action_multi_get(unit, match,
            port, size, vlan_match_array, action_set, &valid_count);
    if (rv == BCM_E_NONE) {
        for (i = 0, ctr = start;
                (size > ctr) && (i < valid_count); i++, ctr++) {
            switch(vlan_match_array[i].match) {
                case BCM_VLAN_MATCH_MAC_PORT:
                case BCM_VLAN_MATCH_MAC_PORT_CLASS:
                    if (vlan_match_array[i].match == BCM_VLAN_MATCH_MAC_PORT) {
                        match_array->match = BCM_PORT_MATCH_MAC_PORT;
                        match_array->port = vlan_match_array[i].port;
                    } else if (vlan_match_array[i].match ==
                            BCM_VLAN_MATCH_MAC_PORT_CLASS) {
                        match_array->match = BCM_PORT_MATCH_MAC_PORT_CLASS;
                        match_array->port_class =
                            vlan_match_array[i].port_class;
                    }
                    sal_memcpy(match_array->src_mac,
                            vlan_match_array[i].src_mac, sizeof (bcm_mac_t));

                    if (match_array->action != NULL) {
                        sal_memcpy(match_array->action, &action_set[i],
                                sizeof(bcm_vlan_action_set_t));
                    }
                    match_array++;
                    break;
                default:
                    continue;
            }
        }
    }

    *count = (start + valid_count);

    if (vlan_match_array != NULL) {
        sal_free(vlan_match_array);
    }
    if (action_set != NULL) {
        sal_free(action_set);
    }

    return BCM_E_NONE;
}
#endif
