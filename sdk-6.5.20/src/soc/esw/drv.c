/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * StrataSwitch driver
 */

#include <shared/bsl.h>

#include <stddef.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <sal/core/spl.h>
#include <sal/core/sync.h>
#include <sal/core/boot.h>
#include <sal/core/dpc.h>
#include <sal/appl/sal.h>
#ifdef PLISIM
#include <sal/appl/config.h>
#endif

#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/scache_dictionary.h>
#include <soc/mcm/driver.h>     /* soc_base_driver_table */
#include <soc/cm.h>
#include <soc/mem.h>
#include <soc/error.h>
#include <soc/l2x.h>
#include <soc/l2u.h>
#include <soc/dma.h>
#include <soc/i2c.h>
#include <soc/counter.h>
#include <soc/hercules.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#include <soc/uc_msg.h>
#endif
#ifdef BCM_CMICX_SUPPORT
#include <soc/intr_cmicx.h>
#include <soc/soc_schan_fifo.h>
#include <shared/cmicfw/iproc_m0ssq.h>
#include <shared/cmicfw/iproc_mbox.h>
#include <shared/cmicfw/cmicx_led.h>
#include <shared/cmicfw/cmicx_link.h>
#include <soc/soc_async.h>
#endif
#include <soc/soc_mem_bulk.h>
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)
#include <soc/iproc.h>
#endif
#if defined(BCM_FIREBOLT_SUPPORT)
#include <soc/firebolt.h>
#endif /* BCM_FIREBOLT_SUPPORT */
#if defined(BCM_BRADLEY_SUPPORT)
#include <soc/bradley.h>
#endif /* BCM_BRADLEY_SUPPORT */
#if defined(BCM_TRIUMPH_SUPPORT)
#include <soc/triumph.h>
#include <soc/er_tcam.h>
#endif /* BCM_TRIUMPH_SUPPORT */
#if defined(BCM_ENDURO_SUPPORT)
#include <soc/enduro.h>
#endif /* BCM_ENDURO_SUPPORT */
#if defined(BCM_HURRICANE_SUPPORT)
#include <soc/hurricane.h>
#endif /* BCM_HURRICANE_SUPPORT */
#if defined(BCM_HURRICANE2_SUPPORT)
#include <soc/hurricane2.h>
#endif /* BCM_HURRICANE2_SUPPORT */
#if defined(BCM_HURRICANE3_SUPPORT)
#include <soc/hurricane3.h>
#include <soc/wolfhound2.h>
#endif /* BCM_HURRICANE3_SUPPORT */
#if defined(BCM_GREYHOUND_SUPPORT)
#include <soc/greyhound.h>
#endif /* BCM_GREYHOUND_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
#include <soc/greyhound2.h>
#endif /* BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_FIRELIGHT_SUPPORT)
#include <soc/firelight.h>
#endif /* BCM_FIRELIGHT_SUPPORT */
#if defined(BCM_VALKYRIE_SUPPORT)
#include <soc/valkyrie.h>
#endif /* BCM_VALKYRIE_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <soc/triumph2.h>
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_TRIDENT_SUPPORT)
#include <soc/trident.h>
#endif /* BCM_TRIDENT_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
#include <soc/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_MONTEREY_SUPPORT)
#include <soc/monterey.h>
#endif /* BCM_MONTEREY_SUPPORT */
#if defined(BCM_APACHE_SUPPORT)
#include <soc/apache.h>
#endif /* BCM_APACHE_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT)
#include <soc/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_HELIX4_SUPPORT)
#include <soc/helix4.h>
#endif /* BCM_HELIX4_SUPPORT */
#if defined(BCM_SIRIUS_SUPPORT)
#include <soc/sbx/sbx_drv.h>
#endif
#if defined(BCM_SHADOW_SUPPORT)
#include <soc/shadow.h>
#endif /* BCM_SHADOW_SUPPORT */
#if defined(BCM_KATANA_SUPPORT)
#include <soc/katana.h>
#endif /* BCM_KATANA_SUPPORT */
#if defined(BCM_KATANA2_SUPPORT)
#include <soc/katana2.h>
#include <soc/uc.h>
#endif /* BCM_KATANA2_SUPPORT */
#if defined(BCM_SABER2_SUPPORT)
#include <soc/saber2.h>
#include <soc/shmoo_and28.h>
#include <soc/phy/ddr34.h>
#endif /* BCM_SABER2_SUPPORT */
#if defined(BCM_METROLITE_SUPPORT)
#include <soc/metrolite.h>
#endif /* BCM_METROLITE_SUPPORT */
#if defined(BCM_TOMAHAWK_SUPPORT)
#include <soc/tomahawk.h>
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/tomahawk3.h>
#endif /* BCM_TOMAHAWK3_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
#include <soc/trident3.h>
#endif /* BCM_TRIDENT3_SUPPORT */
#if defined(BCM_HURRICANE4_SUPPORT)
#include <soc/hurricane4.h>
#endif /* BCM_HURRICANE4_SUPPORT */
#if defined(BCM_HELIX5_SUPPORT)
#include <soc/helix5.h>
#endif /* BCM_HELIX5_SUPPORT */
#if defined(BCM_MAVERICK2_SUPPORT)
#include <soc/maverick2.h>
#endif /* BCM_MAVERICK2_SUPPORT */
#if defined(BCM_TOMAHAWK2_SUPPORT)
#include <soc/tomahawk2.h>
#endif /* BCM_TOMAHAWK2_SUPPORT */
#if defined(BCM_FIREBOLT6_SUPPORT)
#include <soc/firebolt6.h>
#endif /* BCM_FIREBOLT6_SUPPORT */
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/tomahawk3.h>
#endif /* BCM_TOMAHAWK3_SUPPORT */

#include <soc/mmuerr.h>
#include <soc/higig.h>
#include <soc/ipoll.h>
#include <soc/phyctrl.h>
#include <soc/dport.h>
#ifdef BCM_RCPU_SUPPORT
#include <soc/rcpu.h>
#endif /* BCM_RCPU_SUPPORT */
#ifdef INCLUDE_KNET
#include <soc/knet.h>
#endif
#include <soc/esw/portctrl.h>
#include <soc/bondoptions.h>
#include <bcm_int/esw/subport.h>
#ifdef INCLUDE_AVS
#include <soc/avs.h>
#endif /* INCLUDE_AVS */

#if defined(CANCUN_SUPPORT)
#include <soc/esw/cancun.h>
#endif /* CANCUN_SUPPORT */

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <soc/td2_td2p.h>
#endif

#ifdef BCM_UTT_SUPPORT
#include <soc/esw/utt.h>
#endif

#ifdef BCM_KATANA_SUPPORT
#define   BCM_SABER_MAX_COUNTER_DIRECTION 2
#define   BCM_SABER_MAX_COUNTER_POOL      8
#endif
extern int _bcm_esw_ibod_sync_recovery_stop(int unit);

#ifdef ALPM_ENABLE
extern int alpm_dist_hitbit_thread_stop(int unit);
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int soc_warmboot_reset(int unit);
#endif

#ifdef BCM_ESW_SUPPORT

#define _SOC_MEM_SKIP_SCRUB(unit, mem, skip) {\
    soc_mem_t skip_mem = mem;\
    skip = (SOC_MEM_INFO(unit, mem).flags & SOC_MEM_SER_FLAGS)? 0 : 1;\
    switch(mem) {\
        case RH_HGT_FLOWSETm:\
        case RH_HGT_FLOWSET_PIPE0m:\
        case RH_HGT_FLOWSET_PIPE1m:\
        case DLB_HGT_FLOWSETm:\
            if ((SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2PLUS(unit)) &&\
                SOC_REG_IS_VALID(unit, ENHANCED_HASHING_CONTROLr)) {\
                uint32 is_rh_hgt = 0;\
                uint32 rval;\
                READ_ENHANCED_HASHING_CONTROLr(unit, &rval);\
                is_rh_hgt = soc_reg_field_get(unit, ENHANCED_HASHING_CONTROLr, rval,\
                                RH_HGT_ENABLEf);\
                skip_mem = (is_rh_hgt == 1)? DLB_HGT_FLOWSETm : RH_HGT_FLOWSETm;\
                skip = (skip_mem == mem)?1:0;\
            }\
            if (SOC_IS_TRIDENT3X(unit)  &&\
                SOC_REG_IS_VALID(unit, ENHANCED_HASHING_CONTROL_2r)) {\
                uint32 is_rh_dlb = 0;\
                uint32 is_hgt_lag = 0;\
                uint32 rval;\
                READ_ENHANCED_HASHING_CONTROL_2r(unit, &rval);\
                is_rh_dlb = soc_reg_field_get(unit, ENHANCED_HASHING_CONTROL_2r, rval,\
                                RH_DLB_SELECTIONf);\
                is_hgt_lag = soc_reg_field_get(unit, ENHANCED_HASHING_CONTROL_2r, rval,\
                                HGT_LAG_FLOWSET_TABLE_CONFIGf);\
                skip = (is_rh_dlb == 0)? 1 :((is_hgt_lag == 0)? 1 : 0);\
            }\
            break;\
       case RH_LAG_FLOWSETm:\
       case RH_LAG_FLOWSET_PIPE0m:\
       case RH_LAG_FLOWSET_PIPE1m:\
            if (SOC_IS_TRIDENT3X(unit)  &&\
                SOC_REG_IS_VALID(unit, ENHANCED_HASHING_CONTROL_2r)) {\
                uint32 is_rh_dlb = 0;\
                uint32 is_hgt_lag = 0;\
                uint32 rval;\
                READ_ENHANCED_HASHING_CONTROL_2r(unit, &rval);\
                is_rh_dlb = soc_reg_field_get(unit, ENHANCED_HASHING_CONTROL_2r, rval,\
                                RH_DLB_SELECTIONf);\
                is_hgt_lag = soc_reg_field_get(unit, ENHANCED_HASHING_CONTROL_2r, rval,\
                                HGT_LAG_FLOWSET_TABLE_CONFIGf);\
                skip = (is_rh_dlb == 0)? 1 :((is_hgt_lag == 1)? 1 : 0);\
            }\
            break;\
        case DLB_HGT_LAG_FLOWSETm:\
        case DLB_HGT_LAG_FLOWSET_PIPE0m:\
        case DLB_HGT_LAG_FLOWSET_PIPE1m:\
            if (SOC_IS_TRIDENT3X(unit)  &&\
                SOC_REG_IS_VALID(unit, ENHANCED_HASHING_CONTROL_2r)) {\
                uint32 is_rh_dlb = 0;\
                uint32 rval;\
                READ_ENHANCED_HASHING_CONTROL_2r(unit, &rval);\
                is_rh_dlb = soc_reg_field_get(unit, ENHANCED_HASHING_CONTROL_2r, rval,\
                                RH_DLB_SELECTIONf);\
                skip = (is_rh_dlb == 1)? 1 : 0;\
            }\
            break;\
        case RH_ECMP_FLOWSETm:\
        case RH_ECMP_FLOWSET_PIPE0m:\
        case RH_ECMP_FLOWSET_PIPE1m:\
        if (SOC_IS_TRIDENT3X(unit)  &&\
                SOC_REG_IS_VALID(unit, ENHANCED_HASHING_CONTROLr)) {\
                uint32 is_rh_dlb = 0;\
                uint32 rval;\
                READ_ENHANCED_HASHING_CONTROLr(unit, &rval);\
                is_rh_dlb = soc_reg_field_get(unit, ENHANCED_HASHING_CONTROLr, rval,\
                                RH_DLB_SELECTIONf);\
                skip = (is_rh_dlb == 1)? 0 : 1;\
            }\
            break;\
        case DLB_ECMP_FLOWSETm:\
        case DLB_ECMP_FLOWSET_PIPE0m:\
        case DLB_ECMP_FLOWSET_PIPE1m:\
        case DLB_ECMP_FLOWSET_MEMBER_PIPE0m:\
        case DLB_ECMP_FLOWSET_MEMBER_PIPE1m:\
        case DLB_ECMP_FLOWSET_MEMBERm:\
            if (SOC_IS_TRIDENT3X(unit)  &&\
                SOC_REG_IS_VALID(unit, ENHANCED_HASHING_CONTROLr)) {\
                uint32 is_rh_dlb = 0;\
                uint32 rval;\
                READ_ENHANCED_HASHING_CONTROLr(unit, &rval);\
                is_rh_dlb = soc_reg_field_get(unit, ENHANCED_HASHING_CONTROLr, rval,\
                                RH_DLB_SELECTIONf);\
                skip = (is_rh_dlb == 1)? 1 : 0;\
            }\
            break;\
        default:\
            break;\
    }\
}

#define _SOC_DRV_MEM_REUSE_MEM_STATE_NO_LOCK(unit, mem) \
{\
    switch(mem) {\
    case VLAN_XLATE_1m: mem = VLAN_XLATEm; break;\
    case EP_VLAN_XLATE_1m: mem = EGR_VLAN_XLATEm; break;\
    case MPLS_ENTRY_1m: mem = MPLS_ENTRYm; break;\
    case VLAN_MACm: \
        if (SOC_IS_TRX(unit) && !soc_feature(unit, soc_feature_ism_memory)) {\
            mem = VLAN_XLATEm;\
        }\
        break;\
    case EGR_VLANm: \
        if (SOC_IS_KATANA2(unit)) {\
            mem = VLAN_TABm;\
        }\
        break;\
    default: break;\
    }\
}

/* Routine to check if this is the mem whose state is being (mapped-to) actually-used by others */
int _SOC_DRV_MEM_IS_REUSED_MEM(int unit, soc_mem_t mem) {\
    switch(mem) {\
    case EGR_VLAN_XLATEm: return TRUE;\
    case MPLS_ENTRYm: return TRUE;\
    case VLAN_XLATEm:\
        if (SOC_IS_TRX(unit)) {\
            return TRUE;\
        }\
        break;\
    case VLAN_TABm:\
        if (SOC_IS_KATANA2(unit)) {\
            return TRUE;\
        }\
        break;\
    default: break;\
    }\
    return FALSE;\
}

void soc_drv_mem_reuse_mem_state(int unit, soc_mem_t *mem,
                                 soc_mem_t *lock_mem)
{
    _SOC_DRV_MEM_REUSE_MEM_STATE_NO_LOCK(unit, *mem);
    *lock_mem = *mem;
    switch(*mem) {
    case L3_ENTRY_ONLYm:
    case L3_ENTRY_IPV4_UNICASTm:
    case L3_ENTRY_IPV6_UNICASTm:
    case L3_ENTRY_IPV6_MULTICASTm:
        if (soc_feature(unit, soc_feature_shared_hash_mem)) {
            *lock_mem = L3_ENTRY_IPV4_MULTICASTm;
        }
        break;
    case L3_ENTRY_ECCm:
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_QUADm:
    case L3_ENTRY_ONLY_SINGLEm:
    case L3_ENTRY_ONLY_DOUBLEm:
    case L3_ENTRY_ONLY_QUADm:
        if (soc_feature(unit, soc_feature_shared_hash_mem)) {
            *lock_mem = L3_ENTRY_ECCm;
        }
        break;
    /* In TH2, VLAN_2_TAB share the same lock with VLAN_TAB*/
    
    case EGR_VLANm:
        if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VRF_IDf)) {
            *lock_mem = VLAN_TABm;
        }
        break;
    default: break;
    }
    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_APACHE(unit)) {
        switch(*mem) {
        case L3_ENTRY_LPm:
        case L3_ENTRY_ISS_LPm:
        case L3_ENTRY_ONLY_ECCm:
            if (soc_feature(unit, soc_feature_shared_hash_mem)) {
                *lock_mem = L3_ENTRY_IPV4_MULTICASTm;
            }
            break;
        default: break;
        }
    }
    if (SOC_IS_TRIDENT3X(unit)) {
        switch (*mem) {
            case EXACT_MATCH_2m:
            case EXACT_MATCH_4m:
            case EXACT_MATCH_2_PIPE0m:
            case EXACT_MATCH_2_PIPE1m:
            case EXACT_MATCH_4_PIPE0m:
            case EXACT_MATCH_4_PIPE1m:
            case EXACT_MATCH_ECCm:
            case EXACT_MATCH_ECC_PIPE0m:
            case EXACT_MATCH_ECC_PIPE1m:
                *lock_mem = EXACT_MATCH_ECCm;
                break;
            case L2_ENTRY_SINGLEm:
            case L2_ENTRY_ECCm:
                *lock_mem = L2Xm;
                break;
            case VLAN_XLATE_1_DOUBLEm:
            case VLAN_XLATE_1_ECCm:
                *lock_mem = VLAN_XLATE_1_SINGLEm;
                break;
            case VLAN_XLATE_2_DOUBLEm:
            case VLAN_XLATE_2_ECCm:
                *lock_mem = VLAN_XLATE_2_SINGLEm;
                break;
            case EGR_VLAN_XLATE_1_DOUBLEm:
            case EGR_VLAN_XLATE_1_ECCm:
                *lock_mem = EGR_VLAN_XLATE_1_SINGLEm;
                break;
            case EGR_VLAN_XLATE_2_DOUBLEm:
            case EGR_VLAN_XLATE_2_ECCm:
                *lock_mem = EGR_VLAN_XLATE_2_SINGLEm;
                break;
            case MPLS_ENTRY_SINGLEm:
            case MPLS_ENTRY_ECCm:
                *lock_mem = MPLS_ENTRYm;
                break;
            case ING_DNAT_ADDRESS_TYPE_ECCm:
                *lock_mem = ING_DNAT_ADDRESS_TYPEm;
                break;
            case ING_VP_VLAN_MEMBERSHIP_ECCm:
                *lock_mem = ING_VP_VLAN_MEMBERSHIPm;
                break;
            case EGR_VP_VLAN_MEMBERSHIP_ECCm:
                *lock_mem = EGR_VP_VLAN_MEMBERSHIPm;
                break;
            case SUBPORT_ID_TO_SGPP_MAP_ECCm:
                *lock_mem = SUBPORT_ID_TO_SGPP_MAPm;
                break;
            default: break;
        }
    }

    if (SOC_IS_TOMAHAWK3(unit)) {
        switch (*mem) {
            case EXACT_MATCH_2m:
            case EXACT_MATCH_4m:
                *lock_mem = EXACT_MATCH_ECCm;
                break;
            default: break;
        }
    }

    if (SOC_IS_TD2P_TT2P(unit) || SOC_IS_APACHE(unit) || SOC_IS_TOMAHAWK(unit)||
        SOC_IS_TOMAHAWKPLUS(unit) || SOC_IS_TOMAHAWK2(unit)) {
        switch (*mem) {
            case L2_ENTRY_ONLY_ECCm:
                *lock_mem = L2Xm;
                break;
            case VLAN_XLATE_ECCm:
                *lock_mem = VLAN_XLATEm;
                break;
            case EGR_VLAN_XLATE_ECCm:
                *lock_mem = EGR_VLAN_XLATEm;
                break;
            default: break;
        }
    }

    if (SOC_IS_TOMAHAWK3(unit)) {
        switch (*mem) {
            case L2_ENTRY_ECCm:
                *lock_mem = L2Xm;
                break;
            default: break;
        }
    }

    return;
}

void soc_drv_mem_sync_mapped_lock(int unit, soc_mem_t mem,
                                  sal_mutex_t lock_mapped)
{
    switch(mem) {
    case EGR_VLAN_XLATEm:
        SOC_MEM_STATE(unit, EP_VLAN_XLATE_1m).lock = lock_mapped;
        break;
    case MPLS_ENTRYm:
        SOC_MEM_STATE(unit, MPLS_ENTRY_1m).lock = lock_mapped;
        break;
    case VLAN_XLATEm:
        if (SOC_IS_TRX(unit)) {
            if (soc_feature(unit, soc_feature_ism_memory)) {
                SOC_MEM_STATE(unit, VLAN_XLATE_1m).lock = lock_mapped;
            } else {
                SOC_MEM_STATE(unit, VLAN_MACm).lock = lock_mapped;
            }
        }
        break;
    case VLAN_TABm:
        if (SOC_IS_KATANA2(unit)) {
            SOC_MEM_STATE(unit, EGR_VLANm).lock = lock_mapped;
        }
        if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VRF_IDf)) {
            SOC_MEM_STATE(unit, EGR_VLANm).lock = lock_mapped;
        }
        break;
    case L3_ENTRY_IPV4_MULTICASTm:
        if (soc_feature(unit, soc_feature_shared_hash_mem)) {
            SOC_MEM_STATE(unit, L3_ENTRY_ONLYm).lock = lock_mapped;
            SOC_MEM_STATE(unit, L3_ENTRY_IPV4_UNICASTm).lock = lock_mapped;
            SOC_MEM_STATE(unit, L3_ENTRY_IPV6_UNICASTm).lock = lock_mapped;
            SOC_MEM_STATE(unit, L3_ENTRY_IPV6_MULTICASTm).lock = lock_mapped;
            if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_APACHE(unit)) {
                SOC_MEM_STATE(unit, L3_ENTRY_LPm).lock = lock_mapped;
                SOC_MEM_STATE(unit, L3_ENTRY_ISS_LPm).lock = lock_mapped;
                SOC_MEM_STATE(unit, L3_ENTRY_ONLY_ECCm).lock = lock_mapped;
            }
        }
        break;
    case L3_ENTRY_ECCm:
        if (soc_feature(unit, soc_feature_shared_hash_mem)) {
            SOC_MEM_STATE(unit, L3_ENTRY_SINGLEm).lock = lock_mapped;
            SOC_MEM_STATE(unit, L3_ENTRY_DOUBLEm).lock = lock_mapped;
            SOC_MEM_STATE(unit, L3_ENTRY_QUADm).lock = lock_mapped;
            SOC_MEM_STATE(unit, L3_ENTRY_ONLY_SINGLEm).lock = lock_mapped;
            SOC_MEM_STATE(unit, L3_ENTRY_ONLY_DOUBLEm).lock = lock_mapped;
            SOC_MEM_STATE(unit, L3_ENTRY_ONLY_QUADm).lock = lock_mapped;
        }
        break;
    case EXACT_MATCH_ECCm:
        if (SOC_IS_TRIDENT3X(unit)) {
            SOC_MEM_STATE(unit, EXACT_MATCH_2m).lock = lock_mapped;
            SOC_MEM_STATE(unit, EXACT_MATCH_4m).lock = lock_mapped;
            SOC_MEM_STATE(unit, EXACT_MATCH_2_PIPE0m).lock = lock_mapped;
            SOC_MEM_STATE(unit, EXACT_MATCH_2_PIPE1m).lock = lock_mapped;
            SOC_MEM_STATE(unit, EXACT_MATCH_4_PIPE0m).lock = lock_mapped;
            SOC_MEM_STATE(unit, EXACT_MATCH_4_PIPE1m).lock = lock_mapped;
            SOC_MEM_STATE(unit, EXACT_MATCH_ECC_PIPE0m).lock = lock_mapped;
            SOC_MEM_STATE(unit, EXACT_MATCH_ECC_PIPE1m).lock = lock_mapped;
        }
        if (SOC_IS_TOMAHAWK3(unit)) {
            SOC_MEM_STATE(unit, EXACT_MATCH_2m).lock = lock_mapped;
            SOC_MEM_STATE(unit, EXACT_MATCH_4m).lock = lock_mapped;
        }
        break;
    case EGR_IP_TUNNELm:
        if (SOC_IS_TOMAHAWK3(unit)) {
            SOC_MEM_STATE(unit, EGR_IP_TUNNEL_MPLSm).lock = lock_mapped;
            SOC_MEM_STATE(unit, EGR_IP_TUNNEL_IPV6m).lock = lock_mapped;
        }
        break;
    default: break;
    }

    if (SOC_IS_TRIDENT3X(unit)) {
        switch (mem) {
            case L2Xm:
                SOC_MEM_STATE(unit, L2_ENTRY_ECCm).lock = lock_mapped;
                SOC_MEM_STATE(unit, L2_ENTRY_SINGLEm).lock = lock_mapped;
                break;
            case VLAN_XLATE_1_SINGLEm:
                SOC_MEM_STATE(unit, VLAN_XLATE_1_ECCm).lock = lock_mapped;
                SOC_MEM_STATE(unit, VLAN_XLATE_1_DOUBLEm).lock = lock_mapped;
                break;
            case VLAN_XLATE_2_SINGLEm:
                SOC_MEM_STATE(unit, VLAN_XLATE_2_ECCm).lock = lock_mapped;
                SOC_MEM_STATE(unit, VLAN_XLATE_2_DOUBLEm).lock = lock_mapped;
                break;
            case EGR_VLAN_XLATE_1_SINGLEm:
                SOC_MEM_STATE(unit, EGR_VLAN_XLATE_1_ECCm).lock = lock_mapped;
                SOC_MEM_STATE(unit, EGR_VLAN_XLATE_1_DOUBLEm).lock = lock_mapped;
                break;
            case EGR_VLAN_XLATE_2_SINGLEm:
                SOC_MEM_STATE(unit, EGR_VLAN_XLATE_2_ECCm).lock = lock_mapped;
                SOC_MEM_STATE(unit, EGR_VLAN_XLATE_2_DOUBLEm).lock = lock_mapped;
                break;
            case MPLS_ENTRYm:
                SOC_MEM_STATE(unit, MPLS_ENTRY_ECCm).lock = lock_mapped;
                SOC_MEM_STATE(unit, MPLS_ENTRY_SINGLEm).lock = lock_mapped;
                break;
            case ING_DNAT_ADDRESS_TYPEm:
                SOC_MEM_STATE(unit, ING_DNAT_ADDRESS_TYPE_ECCm).lock = lock_mapped;
                break;
            case ING_VP_VLAN_MEMBERSHIPm:
                SOC_MEM_STATE(unit, ING_VP_VLAN_MEMBERSHIP_ECCm).lock = lock_mapped;
                break;
            case EGR_VP_VLAN_MEMBERSHIPm:
                SOC_MEM_STATE(unit, EGR_VP_VLAN_MEMBERSHIP_ECCm).lock = lock_mapped;
                break;
            case SUBPORT_ID_TO_SGPP_MAPm:
                SOC_MEM_STATE(unit, SUBPORT_ID_TO_SGPP_MAP_ECCm).lock = lock_mapped;
                break;
            default: break;
        }
    }

    if (SOC_IS_TD2P_TT2P(unit) || SOC_IS_APACHE(unit) || SOC_IS_TOMAHAWK(unit)||
        SOC_IS_TOMAHAWKPLUS(unit) || SOC_IS_TOMAHAWK2(unit)) {
        switch (mem) {
            case L2Xm:
                SOC_MEM_STATE(unit, L2_ENTRY_ONLY_ECCm).lock = lock_mapped;
                break;
            case VLAN_XLATEm:
                SOC_MEM_STATE(unit, VLAN_XLATE_ECCm).lock = lock_mapped;
                break;
            case EGR_VLAN_XLATEm:
                SOC_MEM_STATE(unit, EGR_VLAN_XLATE_ECCm).lock = lock_mapped;
                break;
            default: break;
        }
    }

    if (SOC_IS_TOMAHAWK3(unit)) {
        switch (mem) {
            case L2Xm:
                SOC_MEM_STATE(unit, L2_ENTRY_ECCm).lock = lock_mapped;
                break;
            default: break;
        }
    }

    return;
}
#endif /* BCM_ESW_SUPPORT */

/*
 * Function:
 *      soc_mmu_init
 * Purpose:
 *      Initialize MMU registers
 * Parameters:
 *      unit - unit number
 * Returns:
 *      SOC_E_XXX
 */
int
soc_mmu_init(int unit)
{
    int                 rv;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_mmu_init\n")));

    if (!soc_attached(unit)) {
        return SOC_E_INIT;
    }

    if (SOC_IS_RCPU_ONLY(unit)) {
        return SOC_E_NONE;
    }

    /* Perform Chip-Dependent Initializations */

    rv = SOC_FUNCTIONS(unit)->soc_mmu_init(unit);

    return rv;
}

/*
 * Function:
 *      soc_chip_driver_find
 * Purpose:
 *      Return the soc_driver for a chip if found.
 *      If not found, return NULL.
 * Parameters:
 *      pci_dev_id - PCI dev ID to find (exact match)
 *      pci_rev_id - PCI dev ID to find (exact match)
 * Returns:
 *      Pointer to static driver structure
 */
soc_driver_t *
soc_chip_driver_find(uint16 pci_dev_id, uint8 pci_rev_id)
{
    int                 i;
    soc_driver_t        *d;
    uint16              driver_dev_id;
    uint16              driver_rev_id;

    if (soc_cm_get_id_driver(pci_dev_id, pci_rev_id,
                             &driver_dev_id, &driver_rev_id) < 0) {
        return NULL;
    }

    /*
     * Find driver in table.  In theory any IDs returned by
     * soc_cm_id_to_driver_id() should have a driver in the table.
     */

    for (i = 0; i < SOC_NUM_SUPPORTED_CHIPS; i++) {
        d = soc_base_driver_table[i];
        if ((d != NULL) &&
            (d->block_info != NULL) &&
            (d->pci_device == driver_dev_id) &&
            (d->pci_revision == driver_rev_id)) {
            return d;
        }
    }

    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("soc_chip_driver_find: driver in devid table "
                        "not in soc_base_driver_table\n")));

    return NULL;
}

#if defined(PLISIM) && defined(BCM_KATANA2_SUPPORT)
STATIC char *
_dummy_sysconf_get_property(const char *property)
{
#ifndef NO_SAL_APPL
    return sal_config_get(property);
#else
    return NULL;
#endif
}
STATIC char *
_dummy_config_var_get(soc_cm_dev_t *dev, const char *property)
{
    char  str[SAL_CONFIG_STR_MAX];
    int i=0;
    COMPILER_REFERENCE(dev);
    for (i=0; (property[i] != '.') && (property[i] != '\0');i++) {
         str[i]=property[i];
    }
    str[i]='\0';
    return _dummy_sysconf_get_property(str);
}
#endif /* PLISIM && BCM_KATANA2_SUPPORT */



#if defined(BCM_ESW_SUPPORT) && defined(BCM_IPROC_SUPPORT)
/*
 * Function:
 *      soc_info_bondoptions_adjust
 * Parameters:
 *      unit - Unit number
 * Purpose:
 *      Update soc_control cache based on bond options info.
 *      Also add/remove soc features per SKU capabilities.
 */
STATIC void
soc_info_bondoptions_adjust(int unit)
{
    int i;
    int max_mtu;

    for (i = 0; i < socBondInfoFeatureCount; i++) {

        /* Bond feature is not set, skip */
        if (!SOC_BOND_INFO_FEATURE_GET(unit, i)) {
            continue;
        }

        switch (i) {
        case socBondInfoFeatureEfp512HalfSlice:
            SOC_FEATURE_SET(unit, soc_feature_field_stage_egress_512_half_slice);
            break;
        case socBondInfoFeatureVfp512HalfSlice:
            SOC_FEATURE_SET(unit, soc_feature_field_stage_lookup_512_half_slice);
            break;
        case socBondInfoFeatureIfpSliceHalf:
            SOC_FEATURE_SET(unit, soc_feature_field_stage_half_slice);
#if defined(BCM_APACHE_SUPPORT)
            if (SOC_IS_APACHE(unit)) {
                SOC_FEATURE_SET(unit, soc_feature_field_half_slice_single_tcam);
            }
#endif /* !BCM_APACHE_SUPPORT */
            break;
        case socBondInfoFeatureNoL3Tunnel:
            SOC_FEATURE_SET(unit, soc_feature_no_tunnel);
            break;
        case socBondInfoFeatureNoStaticNat:
            SOC_FEATURE_CLEAR(unit, soc_feature_nat);
            break;
        case socBondInfoFeatureNoCutThru:
            SOC_FEATURE_CLEAR(unit, soc_feature_asf);
            SOC_FEATURE_CLEAR(unit, soc_feature_asf_multimode);
            break;
        case socBondInfoFeatureNoL3:
            SOC_FEATURE_CLEAR(unit, soc_feature_l3);
            if (SOC_IS_GREYHOUND2(unit)) {
                SOC_FEATURE_CLEAR(unit, soc_feature_l3_ip6);
                SOC_FEATURE_CLEAR(unit, soc_feature_l3_entry_key_type);
                SOC_FEATURE_CLEAR(unit, soc_feature_lpm_tcam);
                SOC_FEATURE_CLEAR(unit, soc_feature_ip_mcast);
                SOC_FEATURE_CLEAR(unit, soc_feature_ip_mcast_repl);
                SOC_FEATURE_CLEAR(unit, soc_feature_ipmc_unicast);
                SOC_FEATURE_CLEAR(unit, soc_feature_ipmc_use_configured_dest_mac);
                SOC_FEATURE_CLEAR(unit, soc_feature_urpf);
                SOC_FEATURE_CLEAR(unit, soc_feature_l3mc_use_egress_next_hop);
                SOC_FEATURE_CLEAR(unit, soc_feature_l3_sgv);
                SOC_FEATURE_CLEAR(unit, soc_feature_l3_dynamic_ecmp_group);
                SOC_FEATURE_CLEAR(unit, soc_feature_l3_ingress_interface);
                SOC_FEATURE_CLEAR(unit, soc_feature_l3_iif_zero_invalid);
                SOC_FEATURE_CLEAR(unit, soc_feature_l3_iif_under_4k);
                SOC_FEATURE_SET(unit, soc_feature_l3_no_ecmp);
                SOC_FEATURE_SET(unit, soc_feature_miml_no_l3);
            }
            if (SOC_IS_TOMAHAWK2(unit)) {
                SOC_FEATURE_CLEAR(unit, soc_feature_l3_ip6);
                SOC_FEATURE_CLEAR(unit, soc_feature_l3_entry_key_type);
                SOC_FEATURE_CLEAR(unit, soc_feature_lpm_tcam);
                SOC_FEATURE_CLEAR(unit, soc_feature_ip_mcast);
                SOC_FEATURE_CLEAR(unit, soc_feature_ip_mcast_repl);
                SOC_FEATURE_CLEAR(unit, soc_feature_ipmc_unicast);
                SOC_FEATURE_CLEAR(unit, soc_feature_ipmc_use_configured_dest_mac);
                SOC_FEATURE_CLEAR(unit, soc_feature_urpf);
                SOC_FEATURE_CLEAR(unit, soc_feature_l3mc_use_egress_next_hop);
                SOC_FEATURE_CLEAR(unit, soc_feature_l3_sgv);
                SOC_FEATURE_CLEAR(unit, soc_feature_l3_dynamic_ecmp_group);
                SOC_FEATURE_CLEAR(unit, soc_feature_l3_ingress_interface);
                SOC_FEATURE_CLEAR(unit, soc_feature_l3_iif_zero_invalid);
                SOC_FEATURE_CLEAR(unit, soc_feature_l3_iif_under_4k);
                SOC_FEATURE_CLEAR(unit, soc_feature_ecmp_dlb);
                SOC_FEATURE_CLEAR(unit, soc_feature_ecmp_dlb_optimized);
                SOC_FEATURE_CLEAR(unit, soc_feature_ecmp_dlb_property_force_set);
                SOC_FEATURE_CLEAR(unit, soc_feature_l3_iif_profile);
                SOC_FEATURE_CLEAR(unit, soc_feature_l3_ingress_interface);
                SOC_FEATURE_CLEAR(unit, soc_feature_nat);
                SOC_FEATURE_CLEAR(unit, soc_feature_l2gre);
                SOC_FEATURE_CLEAR(unit, soc_feature_l2gre_default_tunnel);
                SOC_FEATURE_CLEAR(unit, soc_feature_l3_lookup_cmd);
                SOC_FEATURE_CLEAR(unit, soc_feature_l3_shared_defip_table);
                SOC_FEATURE_CLEAR(unit, soc_feature_l3_ip4_options_profile);
                SOC_FEATURE_CLEAR(unit, soc_feature_repl_l3_intf_use_next_hop);
                SOC_FEATURE_CLEAR(unit, soc_feature_l3_extended_host_entry);
                SOC_FEATURE_CLEAR(unit, soc_feature_l3_ecmp_1k_groups);
                SOC_FEATURE_CLEAR(unit, soc_feature_l3_defip_map);
                SOC_FEATURE_CLEAR(unit, soc_feature_l3_host_ecmp_group);
                SOC_FEATURE_CLEAR(unit, soc_feature_niv);
                SOC_FEATURE_CLEAR(unit, soc_feature_vlan_vp);
                SOC_FEATURE_CLEAR(unit, soc_feature_vp_lag);
                SOC_FEATURE_CLEAR(unit, soc_feature_virtual_switching);
                SOC_FEATURE_SET(unit, soc_feature_int_common_init);
            }
            break;
        case socBondInfoFeatureNoEcmp:
            SOC_FEATURE_SET(unit, soc_feature_l3_no_ecmp);
            break;
        case socBondInfoFeatureNoAlpm:
            SOC_FEATURE_CLEAR(unit, soc_feature_alpm);
            break;
        case socBondInfoFeatureNoMim:
            SOC_FEATURE_CLEAR(unit, soc_feature_mim);
            SOC_FEATURE_CLEAR(unit, soc_feature_mim_reserve_default_port);
            SOC_FEATURE_CLEAR(unit, soc_feature_mim_bvid_insertion_control);
            break;
        case socBondInfoFeatureNoMpls:
            SOC_FEATURE_CLEAR(unit, soc_feature_mpls);
            SOC_FEATURE_CLEAR(unit, soc_feature_mpls_enhanced);
            SOC_FEATURE_CLEAR(unit, soc_feature_mpls_entropy);
            SOC_FEATURE_CLEAR(unit, soc_feature_td2p_mpls_entropy_label);
            SOC_FEATURE_CLEAR(unit, soc_feature_mpls_lsr_ecmp);
            SOC_FEATURE_CLEAR(unit, soc_feature_mpls_segment_routing);
            SOC_FEATURE_CLEAR(unit, soc_feature_mpls_failover);
            SOC_FEATURE_CLEAR(unit, soc_feature_flex_ctr_mpls_3_label_count);
            SOC_FEATURE_CLEAR(unit, soc_feature_mpls_exp_to_phb_cng_map);
            if (SOC_IS_TOMAHAWK2(unit)) {
                SOC_FEATURE_CLEAR(unit, soc_feature_mpls_software_failover);
            }
            break;
        case socBondInfoFeatureNoTrill:
            SOC_FEATURE_CLEAR(unit, soc_feature_trill);
            SOC_FEATURE_CLEAR(unit, soc_feature_trill_ttl);
            break;
        case socBondInfoFeatureNoVxlan:
            SOC_FEATURE_CLEAR(unit, soc_feature_vxlan);
            SOC_FEATURE_CLEAR(unit, soc_feature_vrf_aware_vxlan_termination);
            break;
        case socBondInfoFeatureNoVxlanLite:
            SOC_FEATURE_CLEAR(unit, soc_feature_vxlan_lite);
            break;
        case socBondInfoFeatureNoRRCOE:
            SOC_FEATURE_CLEAR(unit, soc_feature_rroce);
            break;
        case socBondInfoFeatureNoPreemption:
            SOC_FEATURE_CLEAR(unit, soc_feature_preemption);
            SOC_FEATURE_CLEAR(unit, soc_feature_preemption_cnt);
            break;
        case socBondInfoFeatureNoTAS:
            SOC_FEATURE_CLEAR(unit, soc_feature_tas);
            break;
        case socBondInfoFeatureNoHSR:
            SOC_FEATURE_CLEAR(unit, soc_feature_tsn_sr_hsr);
            break;
        case socBondInfoFeatureNoPRP:
            SOC_FEATURE_CLEAR(unit, soc_feature_tsn_sr_prp);
            break;
        case socBondInfoFeatureNo8021CB:
            SOC_FEATURE_CLEAR(unit, soc_feature_tsn_sr_802_1cb);
            break;
        case socBondInfoFeatureNoL2Gre:
            SOC_FEATURE_CLEAR(unit, soc_feature_l2gre);
            SOC_FEATURE_CLEAR(unit, soc_feature_td2_l2gre);
            break;
        case socBondInfoFeatureNoRiot:
            SOC_FEATURE_CLEAR(unit, soc_feature_riot);
            break;
        case socBondInfoFeatureNoFpOam:
            SOC_FEATURE_CLEAR(unit, soc_feature_fp_based_oam);
            break;
        case socBondInfoFeatureNoFcoe:
            SOC_FEATURE_CLEAR(unit, soc_feature_fcoe);
            break;
        case socBondInfoFeatureNoTimeSync:
            SOC_FEATURE_CLEAR(unit, soc_feature_timesync_support);
            break;
        case socBondInfoFeatureNoSat:
            SOC_FEATURE_CLEAR(unit, soc_feature_sat);
            break;
        case socBondInfoFeatureNoServiceMeter:
            SOC_FEATURE_CLEAR(unit, soc_feature_global_meter);
            break;
        case socBondInfoFeatureNoOam:
            SOC_FEATURE_CLEAR(unit, soc_feature_oam);
            break;
        case socBondInfoFeatureNoHgProxyCoe:
            SOC_FEATURE_CLEAR(unit, soc_feature_hgproxy_subtag_coe);
            break;
        case socBondInfoFeatureNoSubTagCoe:
            SOC_FEATURE_CLEAR(unit, soc_feature_subtag_coe);
            break;
        case socBondInfoFeatureNoLinkPhy:
            SOC_FEATURE_CLEAR(unit, soc_feature_linkphy_coe);
            break;
        case socBondInfoFeatureNoAvs:
            SOC_FEATURE_CLEAR(unit, soc_feature_avs);
            break;
        case socBondInfoFeatureNoEpRedirectV2:
            SOC_FEATURE_CLEAR(unit, soc_feature_ep_redirect_v2);
#if defined(BCM_APACHE_SUPPORT)
            if (SOC_IS_APACHE(unit)) {
                SOC_FEATURE_CLEAR(unit, soc_feature_egr_mirror_true);
            }
#endif /* !BCM_APACHE_SUPPORT */
            break;
        case socBondInfoFeatureNoOamTrueUpMep:
            
            break;
        case socBondInfoFeatureNoMultiLevelProt:
            SOC_FEATURE_CLEAR(unit, soc_feature_hierarchical_protection);
            break;
        case socBondInfoFeatureNoFpSat:
            SOC_FEATURE_CLEAR(unit, soc_feature_fp_based_sat);
            break;
        case socBondInfoFeatureNoSegmentRouting:
            SOC_FEATURE_CLEAR(unit, soc_feature_mpls_segment_routing);
            break;
        case socBondInfoFeatureHqos3Levels:
            SOC_FEATURE_CLEAR(unit, soc_feature_mmu_hqos_four_level);
            break;
        case socBondInfoFeatureNoPipe_2_3:
            SOC_FEATURE_CLEAR(unit, soc_feature_pipe2_pipe3_disabled);
            break;
        case socBondInfoFeatureNoExactMatch:
            SOC_FEATURE_CLEAR(unit, soc_feature_field_exact_match_support);
            break;
        case socBondInfoFeatureNoEfp:
            SOC_FEATURE_SET(unit, soc_feature_no_efp);
            break;
        case socBondInfoFeatureNoIfp:
            SOC_FEATURE_SET(unit, soc_feature_no_ifp);
            break;
        case socBondInfoFeatureNoVfp:
            SOC_FEATURE_SET(unit, soc_feature_no_vfp);
            break;
        case socBondInfoFeatureNoLpm:
            SOC_FEATURE_CLEAR(unit, soc_feature_lpm_tcam);
            break;
        case socBondInfoFeatureNoUftBank0:
            SOC_FEATURE_CLEAR(unit, soc_feature_uft_bank_0);
            break;
        case socBondInfoFeatureNoUftBank1:
            SOC_FEATURE_CLEAR(unit, soc_feature_uft_bank_1);
            break;
        case socBondInfoFeatureNoUftBank2:
            SOC_FEATURE_CLEAR(unit, soc_feature_uft_bank_2);
            break;
        case socBondInfoFeatureNoUftBank3:
            SOC_FEATURE_CLEAR(unit, soc_feature_uft_bank_3);
            break;
        case socBondInfoFeatureNoMplsFrr:
            SOC_FEATURE_CLEAR(unit, soc_feature_mpls_frr);
            break;
        case socBondInfoFeatureNoDynamicLoadBalancing:
            SOC_FEATURE_CLEAR(unit, soc_feature_ecmp_dlb);
            SOC_FEATURE_CLEAR(unit, soc_feature_ecmp_dlb_optimized);
            SOC_FEATURE_CLEAR(unit, soc_feature_lag_dlb);
            SOC_FEATURE_CLEAR(unit, soc_feature_hg_dlb);
            SOC_FEATURE_CLEAR(unit, soc_feature_hgt_lag_dlb_optimized);
            break;
        case socBondInfoFeatureNoDynamicAlternatePath:
            SOC_FEATURE_CLEAR(unit, soc_feature_dgm);
            break;
#ifdef INCLUDE_PSTATS
        case socBondInfoFeatureNoPstats:
            SOC_FEATURE_CLEAR(unit, soc_feature_pstats);
            break;
#endif /* INCLUDE_PSTATS */
        case socBondInfoFeatureNoTCB:
            SOC_FEATURE_CLEAR(unit, soc_feature_tcb);
            break;
        case socBondInfoFeatureNoTimestamp:
            SOC_FEATURE_CLEAR(unit, soc_feature_timestamp);
            break;
        case socBondInfoFeatureLpm2k:
#if defined(BCM_TOMAHAWK2_SUPPORT)
            if (SOC_IS_TOMAHAWK2(unit)) {
                SOC_FEATURE_CLEAR(unit, soc_feature_urpf);
                SOC_FEATURE_CLEAR(unit, soc_feature_l3_lpm_scaling_enable);
            }
#endif /* BCM_TOMAHAWK2_SUPPORT */
            break;
#if defined(BCM_TOMAHAWK2_SUPPORT)
        case socBondInfoFeatureIfpSlice3k:
            SOC_FEATURE_SET(unit, soc_feature_field_stage_half_slice);
            break;
        case socBondInfoFeatureIfpSlice1k5:
            SOC_FEATURE_SET(unit, soc_feature_field_stage_quarter_slice);
            break;
#endif /* BCM_TOMAHAWK2_SUPPORT */
        case socBondInfoPM4x10QInst0Disable:
            SOC_FEATURE_SET(unit, soc_feature_PM4x10QInst0Disable);
            break;
        case socBondInfoPM4x10QInst0QsgmiiDisable:
            SOC_FEATURE_SET(unit, soc_feature_PM4x10QInst0QsgmiiDisable);
            break;
        case socBondInfoPM4x10QInst0UsxgmiiDisable:
            SOC_FEATURE_SET(unit, soc_feature_PM4x10QInst0UsxgmiiDisable);
            break;
        case socBondInfoPM4x10QInst1Disable:
            SOC_FEATURE_SET(unit, soc_feature_PM4x10QInst1Disable);
            break;
        case socBondInfoPM4x10QInst1QsgmiiDisable:
            SOC_FEATURE_SET(unit, soc_feature_PM4x10QInst1QsgmiiDisable);
            break;
        case socBondInfoPM4x10QInst1UsxgmiiDisable:
            SOC_FEATURE_SET(unit, soc_feature_PM4x10QInst1UsxgmiiDisable);
            break;

#if defined(BCM_FLOWTRACKER_SUPPORT)
        case socBondInfoBskDisable:
            SOC_FEATURE_CLEAR(unit, soc_feature_flex_flowtracker_ver_1);
            SOC_FEATURE_CLEAR(unit, soc_feature_field_flowtracker_support);
            SOC_FEATURE_CLEAR(unit, soc_feature_bscan_ft_fifodma_support);
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_ver_1_metering);
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_ver_1_bidirectional);
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_9banks_ft_table);
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_ver_1_collector_copy);
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_ver_1_ipfix_remote_collector);
            SOC_FEATURE_SET(unit, soc_feature_flowtracker_ver_1_alu32_0_1_banks);
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
            SOC_FEATURE_CLEAR(unit, soc_feature_flex_flowtracker_ver_2);
            SOC_FEATURE_CLEAR(unit,
                   soc_feature_flex_flowtracker_indirect_memory_access);
            SOC_FEATURE_CLEAR(unit, soc_feature_flex_flowtracker_ver_2_check_slice);
            SOC_FEATURE_CLEAR(unit, soc_feature_flex_flowtracker_ver_2_setid_war);
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_ver_2_drop_code);
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_ver_2_chip_e2e_delay);
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_ver_2_alu_delay);
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_ver_2_iat_idt_delay);
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_ver_2_mmu_congestion_level);
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_ver_2_load_balancing);
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_coupled_data_engines);
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_ts_64_sw_war_support);
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_ver_2_ftfp_tcam_atomic_hw_write);
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
            SOC_FEATURE_CLEAR(unit, soc_feature_flex_flowtracker_ver_3);
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_ts_ntp64_support);
            SOC_FEATURE_CLEAR(unit, soc_feature_field_flowtracker_v3_support);
#endif /* BCM_FLOWTRACKER_V3_SUPPORT */
/* Disable all features related to flowtracker */
            break;
        case socBondInfoBskNoPktSampling:
            SOC_FEATURE_CLEAR(unit,
                soc_feature_flowtracker_ver_1_collector_copy);
            break;
        case socBondInfoBskNoTCPBidir:
            SOC_FEATURE_CLEAR(unit,
                soc_feature_flowtracker_ver_1_bidirectional);
            break;
        case socBondInfoBskNoDoSAttackVector:
            SOC_FEATURE_CLEAR(unit,
                soc_feature_flowtracker_ver_1_dos_attack_check);
            break;
        case socBondInfoBskNoHWExportPktBuild:
            SOC_FEATURE_CLEAR(unit,
                soc_feature_flowtracker_ver_1_ipfix_remote_collector);
            break;
        case socBondInfoBskNoMicroFlowPolicer:
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_ver_1_metering);
            break;
        case socBondInfoBskNoALU32Inst2to11:
            SOC_FEATURE_SET(unit, soc_feature_flowtracker_ver_1_alu32_0_1_banks);
            break;
        case socBondInfoBskSessionDepth3Banks:
            SOC_FEATURE_SET(unit, soc_feature_flowtracker_3banks_ft_table);
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_5banks_ft_table);
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_9banks_ft_table);
            break;
        case socBondInfoBskSessionDepth5Banks:
            SOC_FEATURE_SET(unit, soc_feature_flowtracker_5banks_ft_table);
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_3banks_ft_table);
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_9banks_ft_table);
            break;
        case socBondInfoBskSessionDepth9Banks:
            SOC_FEATURE_SET(unit, soc_feature_flowtracker_9banks_ft_table);
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_3banks_ft_table);
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_5banks_ft_table);
            break;
#endif /* BCM_FLOWTRACKER_SUPPORT */
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
        case socBondInfoBskGroupDepthHalfEntries:
            SOC_FEATURE_SET(unit, soc_feature_flowtracker_half_entries_ft_group);
            break;
        case socBondInfoBskNoDropAnalysis:
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_ver_2_drop_code);
            break;
        case socBondInfoBskNoLatencyTracking:
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_ver_2_chip_e2e_delay);
            break;
        case socBondInfoBskNoAluLatencySupport:
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_ver_2_alu_delay);
            break;
        case socBondInfoBskNoIATIDTTracking:
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_ver_2_iat_idt_delay);
            break;
        case socBondInfoBskNoCongestionTracking:
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_ver_2_mmu_congestion_level);
            break;
        case socBondInfoBskNoLoadBalancingTracking:
            SOC_FEATURE_CLEAR(unit, soc_feature_flowtracker_ver_2_load_balancing);
            break;
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
        case socBondInfoBskAggregateDisable:
            SOC_FEATURE_CLEAR(unit, soc_feature_flex_flowtracker_ver_3);
            break;
#endif /* BCM_FLOWTRACKER_V3_SUPPORT */
#ifdef INCLUDE_XFLOW_MACSEC
        case socBondInfoFeatureNoMacsec:
            SOC_FEATURE_CLEAR(unit, soc_feature_xflow_macsec);
            SOC_FEATURE_CLEAR(unit, soc_feature_xflow_macsec_olp);
            SOC_FEATURE_CLEAR(unit, soc_feature_xflow_macsec_svtag);
            SOC_FEATURE_CLEAR(unit, soc_feature_xflow_macsec_inline);
            SOC_FEATURE_CLEAR(unit, soc_feature_xflow_macsec_stat);
            break;
#endif /* INCLUDE_XFLOW_MACSEC */
        case socBondInfoChannelizedSwitchingDisable:
            SOC_FEATURE_CLEAR(unit, soc_feature_subport_forwarding_support);
            break;
        }
    }

    /* Update SOC_INFO() params */
    if ((SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureFamilyMaverick)) ||
            (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureFamilyRanger2Plus))) {
         max_mtu = soc_property_get(unit, spn_MAX_MTU_SIZE, 9416);
         if ((max_mtu != 9416) && (max_mtu != 9600)){
             max_mtu = 9416;
         }
         SOC_INFO(unit).max_mtu = max_mtu;
    }

    if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureFamilyRanger2Plus)) {
        SOC_FEATURE_CLEAR(unit, soc_feature_olp);
    }

    return;
}
#ifdef BCM_TOMAHAWK2_SUPPORT
STATIC int
soc_lpm_tcam_size_adjust(int unit)
{
    soc_persist_t *sop;
    int defip_config;
    int num_ipv6_128b_entries;
    int config_v6_entries = 0;
    int def_num_ipv6_lpm_128B, max_num_ipv6_lpm_128B;

    sop = SOC_PERSIST(unit);
    if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureLpm2k)) {
        SOC_CONTROL(unit)->l3_defip_max_tcams = 1;
    } else if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureLpm8k)) {
        SOC_CONTROL(unit)->l3_defip_max_tcams = 4;
    } else if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureLpm16k)) {
        return SOC_E_NONE;
    } else {
        return SOC_E_NONE;
    }

    max_num_ipv6_lpm_128B = (SOC_CONTROL(unit)->l3_defip_max_tcams *
                             SOC_CONTROL(unit)->l3_defip_tcam_size) >> 1;
    def_num_ipv6_lpm_128B = max_num_ipv6_lpm_128B >> 1;

    if (!(soc_feature(unit, soc_feature_alpm) &&
          soc_property_get(unit, spn_L3_ALPM_ENABLE, 0))) {

        defip_config = soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 1);


        num_ipv6_128b_entries = soc_property_get(unit,
                                        spn_NUM_IPV6_LPM_128B_ENTRIES,
                                        (defip_config ? def_num_ipv6_lpm_128B : 0));

        num_ipv6_128b_entries = num_ipv6_128b_entries +
                                (num_ipv6_128b_entries % 2);
        if (num_ipv6_128b_entries > max_num_ipv6_lpm_128B) {
            LOG_CLI((BSL_META_U(unit,
                            "The specified ipv6_lpm_128b_entries (%d) exceeds max_ipv6_lpm_128b_entries (%d)\n"),
                            num_ipv6_128b_entries, max_num_ipv6_lpm_128B));
            return SOC_E_PARAM;
        }

        config_v6_entries = num_ipv6_128b_entries;
        if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable) &&
            soc_property_get(unit, spn_LPM_SCALING_ENABLE, 0)) {
            num_ipv6_128b_entries = 0;
            if (!soc_property_get(unit, spn_LPM_IPV6_128B_RESERVED, 1)) {
                config_v6_entries = ((config_v6_entries /
                                    SOC_CONTROL(unit)->l3_defip_tcam_size) +
                                    ((config_v6_entries %
                                     SOC_CONTROL(unit)->l3_defip_tcam_size)
                                    ? 1 : 0)) * SOC_CONTROL(unit)->l3_defip_tcam_size;
            }
        }
        sop->memState[L3_DEFIP_PAIR_128m].index_max =
                                          num_ipv6_128b_entries - 1;
        sop->memState[L3_DEFIP_PAIR_128_ONLYm].index_max =
                                          num_ipv6_128b_entries - 1;
        sop->memState[L3_DEFIP_PAIR_128_DATA_ONLYm].index_max =
                                          num_ipv6_128b_entries - 1;
        sop->memState[L3_DEFIP_PAIR_128_HIT_ONLYm].index_max =
                                          num_ipv6_128b_entries - 1;
        sop->memState[L3_DEFIPm].index_max =
                                 (SOC_CONTROL(unit)->l3_defip_max_tcams *
                                 SOC_CONTROL(unit)->l3_defip_tcam_size) -
                                 (num_ipv6_128b_entries * 2) - 1;

        if (soc_feature(unit, soc_feature_l3_1k_defip_table)) {
            sop->memState[L3_DEFIPm].index_max = 1023;
        }

        sop->memState[L3_DEFIP_ONLYm].index_max =
                                      sop->memState[L3_DEFIPm].index_max;
        sop->memState[L3_DEFIP_DATA_ONLYm].index_max =
                                      sop->memState[L3_DEFIPm].index_max;
        sop->memState[L3_DEFIP_HIT_ONLYm].index_max =
                                      sop->memState[L3_DEFIPm].index_max;
        SOC_CONTROL(unit)->l3_defip_index_remap = num_ipv6_128b_entries;
    } else {
        if (soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 1)) {
            /* slightly different processing for v6-128 */
            if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureLpm2k)) {
                LOG_CLI((BSL_META_U(unit,
                            "The specified Lpm2k can not support LPM_128\n")));
                return SOC_E_PARAM;
            }
            num_ipv6_128b_entries = soc_property_get(unit,
                                            spn_NUM_IPV6_LPM_128B_ENTRIES,
                                            def_num_ipv6_lpm_128B);
            num_ipv6_128b_entries = num_ipv6_128b_entries +
                                    (num_ipv6_128b_entries % 2);
            if (soc_tomahawk_alpm_mode_get(unit) == 1 ||
                soc_tomahawk_alpm_mode_get(unit) == 3) {
                num_ipv6_128b_entries = (num_ipv6_128b_entries + 3) / 4 * 4;
            }
            config_v6_entries = num_ipv6_128b_entries;
            sop->memState[L3_DEFIP_PAIR_128m].index_max =
                                            num_ipv6_128b_entries - 1;
            sop->memState[L3_DEFIP_PAIR_128_ONLYm].index_max =
                                            num_ipv6_128b_entries - 1;
            sop->memState[L3_DEFIP_PAIR_128_DATA_ONLYm].index_max =
                                            num_ipv6_128b_entries - 1;
            sop->memState[L3_DEFIP_PAIR_128_HIT_ONLYm].index_max =
                                            num_ipv6_128b_entries - 1;
            sop->memState[L3_DEFIPm].index_max =
                              (SOC_CONTROL(unit)->l3_defip_max_tcams *
                              SOC_CONTROL(unit)->l3_defip_tcam_size) -
                              (num_ipv6_128b_entries * 2) - 1;

            sop->memState[L3_DEFIP_ONLYm].index_max =
                                   sop->memState[L3_DEFIPm].index_max;
            sop->memState[L3_DEFIP_DATA_ONLYm].index_max =
                                   sop->memState[L3_DEFIPm].index_max;
            sop->memState[L3_DEFIP_HIT_ONLYm].index_max =
                                   sop->memState[L3_DEFIPm].index_max;
            SOC_CONTROL(unit)->l3_defip_index_remap =
                                   (num_ipv6_128b_entries / 2) * 2;
        } else {
            sop->memState[L3_DEFIP_PAIR_128m].index_max = -1;
            sop->memState[L3_DEFIP_PAIR_128_ONLYm].index_max = -1;
            sop->memState[L3_DEFIP_PAIR_128_DATA_ONLYm].index_max = -1;
            sop->memState[L3_DEFIP_PAIR_128_HIT_ONLYm].index_max = -1;
        }
    }
    return soc_l3_defip_indexes_init(unit, config_v6_entries);
}

#endif /* BC_TOMAHAWK2_SUPPORT */

/*
 * Function:
 *      soc_mem_config_bondoptions_adjust
 * Parameters:
 *      unit - Unit number
 * Purpose:
 *      Update soc_control cache based on bond options info.
 *      Also add/remove soc features per SKU capabilities.
 */
STATIC int
soc_mem_config_bondoptions_adjust(int unit)
{
    soc_persist_t *sop;
    int uft_size;
    int min_l2_entries = 16;
    int min_l3_entries = 4;
    int max_l2_entries, max_l3_entries;
    int l2_entries, l3_entries;
    int cfg_l2_entries, cfg_l3_entries;
    int shared_l2_banks, shared_l3_banks;
    int shared_bank_size;
    int alpm_enable;

    sop = SOC_PERSIST(unit);
    if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureVrf2k)) {
        sop->memState[VRFm].index_max = 2047;
    } else if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureVrf1k)) {
        sop->memState[VRFm].index_max = 1023;
    } else if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureVrf16)) {
        sop->memState[VRFm].index_max = 15;
    }

    if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureL3Iif8k)) {
        sop->memState[L3_IIFm].index_max = 8191;
    }

    if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureL3Intf8k)) {
        sop->memState[EGR_L3_INTFm].index_max = 8191;
    }


    if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureSVP4k)) {
        sop->memState[SOURCE_VPm].index_max = 4095;
        sop->memState[ING_DVP_TABLEm].index_max = 4095;
        sop->memState[ING_DVP_2_TABLEm].index_max = 4095;
        sop->memState[EGR_DVP_ATTRIBUTEm].index_max = 4095;
        sop->memState[EGR_DVP_ATTRIBUTE_1m].index_max = 4095;
    }

    if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureL3NHop8k)) {
        sop->memState[EGR_L3_NEXT_HOPm].index_max = 8191;
        sop->memState[ING_L3_NEXT_HOPm].index_max = 8191;
        sop->memState[INITIAL_ING_L3_NEXT_HOPm].index_max = 8191;
    } else if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureNextHop16k)){
        sop->memState[EGR_L3_NEXT_HOPm].index_max = 16383;
        sop->memState[ING_L3_NEXT_HOPm].index_max = 16383;
        sop->memState[INITIAL_ING_L3_NEXT_HOPm].index_max = 16383;
    } else if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureNextHop32k)){
        sop->memState[EGR_L3_NEXT_HOPm].index_max = 32767;
        sop->memState[ING_L3_NEXT_HOPm].index_max = 32767;
        sop->memState[INITIAL_ING_L3_NEXT_HOPm].index_max = 32767;
    } else if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureNextHop48k)){
        sop->memState[EGR_L3_NEXT_HOPm].index_max = 49151;
        sop->memState[ING_L3_NEXT_HOPm].index_max = 49151;
        sop->memState[INITIAL_ING_L3_NEXT_HOPm].index_max = 49151;
    } else if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureNextHop64k)) {
        sop->memState[EGR_L3_NEXT_HOPm].index_max = 65535;
        sop->memState[ING_L3_NEXT_HOPm].index_max = 65535;
        sop->memState[INITIAL_ING_L3_NEXT_HOPm].index_max = 65535;
    }
#if defined(BCM_ESW_SUPPORT) && defined(BCM_IPROC_SUPPORT) && defined(BCM_TOMAHAWK2_SUPPORT)
    if (SOC_IS_TOMAHAWK2(unit)) {
        int rv = SOC_E_NONE;
        rv = soc_lpm_tcam_size_adjust(unit);
        if (rv != SOC_E_NONE) {
            return rv;
        }
    }
#endif
    if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureIpmcDisabled)) {
        sop->memState[L3_IPMCm].index_max = 0;
        sop->memState[L3_IPMC_1m].index_max = 0;
        sop->memState[L3_IPMC_REMAPm].index_max = 0;
        sop->memState[EGR_IPMCm].index_max = 0;
    } else if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureIpmc4k)) {
        sop->memState[L3_IPMCm].index_max = 4095;
        sop->memState[L3_IPMC_1m].index_max = 4095;
        sop->memState[L3_IPMC_REMAPm].index_max = 4095;
        sop->memState[EGR_IPMCm].index_max = 4095;
    } else if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureIpmc8k)) {
        sop->memState[L3_IPMCm].index_max = 8191;
        sop->memState[L3_IPMC_1m].index_max = 8191;
        sop->memState[L3_IPMC_REMAPm].index_max = 8191;
        sop->memState[EGR_IPMCm].index_max = 8191;
    } else if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureIpmc16k)) {
        sop->memState[L3_IPMCm].index_max = 16383;
        sop->memState[L3_IPMC_1m].index_max = 16383;
        sop->memState[L3_IPMC_REMAPm].index_max = 16383;
        sop->memState[EGR_IPMCm].index_max = 16383;
    }

    if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureUft128k)) {
        shared_bank_size = 32;
        uft_size = shared_bank_size * 4;
        max_l2_entries =  min_l2_entries + uft_size;
        max_l3_entries =  min_l3_entries + uft_size;

        cfg_l2_entries = soc_property_get(unit, spn_L2_MEM_ENTRIES, -1);
        l2_entries = cfg_l2_entries == -1 ? min_l2_entries * 1024 : cfg_l2_entries;

        if (l2_entries <= min_l2_entries * 1024) { /* Min table size */
            l2_entries = min_l2_entries * 1024;
            shared_l2_banks = 0;
        } else if (l2_entries <= max_l2_entries * 1024) {
            l2_entries -= min_l2_entries * 1024;
            shared_l2_banks = (l2_entries + (shared_bank_size * 1024 - 1)) / (shared_bank_size *
 1024);
            l2_entries = min_l2_entries * 1024 + shared_l2_banks * shared_bank_size * 1024;
        } else {
            LOG_CLI((BSL_META_U(unit,
                            "The specified l2_mem_entries (%d) exceeds 144K\n"),
                            cfg_l2_entries));
            return SOC_E_PARAM;
        }

        cfg_l3_entries = soc_property_get(unit, spn_L3_MEM_ENTRIES, -1);
        l3_entries = cfg_l3_entries == -1 ? (min_l3_entries * 1024 ) : cfg_l3_entries;
        if (l3_entries <= min_l3_entries * 1024) { /* 4k dedicated L3 entries */
            l3_entries = min_l3_entries * 1024;
            shared_l3_banks = 0;
        } else if (l3_entries <= max_l3_entries * 1024) {
            l3_entries -= min_l3_entries * 1024;
            shared_l3_banks = (l3_entries + (shared_bank_size * 1024 - 1)) / (shared_bank_size *
 1024);
            l3_entries = min_l3_entries * 1024 + shared_l3_banks * shared_bank_size * 1024;
        } else {
            LOG_CLI((BSL_META_U(unit,
                            "The specified l3_mem_entries (%d) exceeds 132k\n"),
                            cfg_l3_entries));
            return SOC_E_PARAM;
        }


        alpm_enable = soc_property_get(unit, spn_L3_ALPM_ENABLE, 0);
        if (alpm_enable && soc_feature(unit, soc_feature_alpm)) {

            if (shared_l2_banks + shared_l3_banks > 2) {
                LOG_CLI((BSL_META_U(unit,
                       "A minimum of 2 shared banks is required for ALPM "
                       "when it is enabled\n")));
                return SOC_E_PARAM;
            }
        } else if (shared_l2_banks + shared_l3_banks > 4) {
            return SOC_E_PARAM;
        }

        sop->memState[L2Xm].index_max = l2_entries - 1;
        sop->memState[L2_ENTRY_ONLYm].index_max = l2_entries - 1;
        sop->memState[L2_ENTRY_ONLY_ECCm].index_max = l2_entries - 1;
        sop->memState[L2_HITDA_ONLYm].index_max = l2_entries / 4 - 1;
        sop->memState[L2_HITSA_ONLYm].index_max = l2_entries / 4 - 1;
        sop->memState[L2_ENTRY_LPm].index_max = min_l2_entries*1024 / 4 - 1;
        sop->memState[L2_ENTRY_ISS_LPm].index_max =
             (l2_entries - min_l2_entries*1024) / 4 - 1;

        /* Adjust L3 table size */
        sop->memState[L3_ENTRY_ONLYm].index_max = l3_entries - 1;
        sop->memState[L3_ENTRY_IPV4_UNICASTm].index_max = l3_entries - 1;
        sop->memState[L3_ENTRY_IPV4_MULTICASTm].index_max = l3_entries / 2 - 1;
        sop->memState[L3_ENTRY_IPV6_UNICASTm].index_max = l3_entries/ 2 - 1;
        sop->memState[L3_ENTRY_IPV6_MULTICASTm].index_max = l3_entries / 4 - 1;
        sop->memState[L3_ENTRY_ONLY_ECCm].index_max = l3_entries - 1;
        sop->memState[L3_ENTRY_HIT_ONLYm].index_max = l3_entries / 4 - 1;
        sop->memState[L3_ENTRY_LPm].index_max = min_l3_entries*1024 / 4 - 1;
        sop->memState[L3_ENTRY_ISS_LPm].index_max =
             (l3_entries - min_l3_entries*1024) / 4 - 1;

        /* Adjust ALPM table size */
        if (alpm_enable) {
            /* The check below is sufficient because if ALPM mode is enabled
            * and number of shared banks used is > 2 we return SOC_E_PARAM
            * in code at the top of this function.
            */
            if (shared_l2_banks + shared_l3_banks > 0) {
                sop->memState[L3_DEFIP_ALPM_RAWm].index_max =
                    (SOC_MEM_INFO(unit, L3_DEFIP_ALPM_RAWm).index_max / 2);
                sop->memState[L3_DEFIP_ALPM_IPV4m].index_max =
                    (SOC_MEM_INFO(unit, L3_DEFIP_ALPM_IPV4m).index_max / 2);
                sop->memState[L3_DEFIP_ALPM_IPV4_1m].index_max =
                    (SOC_MEM_INFO(unit, L3_DEFIP_ALPM_IPV4_1m).index_max / 2);
                sop->memState[L3_DEFIP_ALPM_IPV6_64m].index_max =
                    (SOC_MEM_INFO(unit, L3_DEFIP_ALPM_IPV6_64m).index_max / 2);
                sop->memState[L3_DEFIP_ALPM_IPV6_64_1m].index_max =
                    (SOC_MEM_INFO(unit, L3_DEFIP_ALPM_IPV6_64_1m).index_max / 2);
                sop->memState[L3_DEFIP_ALPM_IPV6_128m].index_max =
                    (SOC_MEM_INFO(unit, L3_DEFIP_ALPM_IPV6_128m).index_max / 2);
                sop->memState[L3_DEFIP_ALPM_HIT_ONLYm].index_max =
                    (SOC_MEM_INFO(unit, L3_DEFIP_ALPM_HIT_ONLYm).index_max / 2);
                sop->memState[L3_DEFIP_ALPM_ECCm].index_max =
                    (SOC_MEM_INFO(unit, L3_DEFIP_ALPM_ECCm).index_max / 2);
            }
        } else {
            sop->memState[L3_DEFIP_ALPM_RAWm].index_max = -1;
            sop->memState[L3_DEFIP_ALPM_IPV4m].index_max = -1;
            sop->memState[L3_DEFIP_ALPM_IPV4_1m].index_max = -1;
            sop->memState[L3_DEFIP_ALPM_IPV6_64m].index_max = -1;
            sop->memState[L3_DEFIP_ALPM_IPV6_64_1m].index_max = -1;
            sop->memState[L3_DEFIP_ALPM_IPV6_128m].index_max = -1;
            sop->memState[L3_DEFIP_ALPM_HIT_ONLYm].index_max = -1;
            sop->memState[L3_DEFIP_ALPM_ECCm].index_max = -1;
        }

    } else if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureUft256k)) {
        shared_bank_size = 64;
        uft_size = shared_bank_size * 4;
        max_l2_entries =  min_l2_entries + uft_size;
        max_l3_entries =  min_l3_entries + uft_size;

        cfg_l2_entries = soc_property_get(unit, spn_L2_MEM_ENTRIES, -1);
        l2_entries = cfg_l2_entries == -1 ? min_l2_entries * 1024 : cfg_l2_entries;

        if (l2_entries <= min_l2_entries * 1024) { /* Min table size */
            l2_entries = min_l2_entries * 1024;
            shared_l2_banks = 0;
        } else if (l2_entries <= max_l2_entries * 1024) {
            l2_entries -= min_l2_entries * 1024;
            shared_l2_banks = (l2_entries + (shared_bank_size * 1024 - 1)) / (shared_bank_size *
 1024);
            l2_entries = min_l2_entries * 1024 + shared_l2_banks * shared_bank_size * 1024;
        } else {
            LOG_CLI((BSL_META_U(unit,
                            "The specified l2_mem_entries (%d) exceeds 144K\n"),
                            cfg_l2_entries));
            return SOC_E_PARAM;
        }

        cfg_l3_entries = soc_property_get(unit, spn_L3_MEM_ENTRIES, -1);
        l3_entries = cfg_l3_entries == -1 ? (min_l3_entries * 1024 ) : cfg_l3_entries;
        if (l3_entries <= min_l3_entries * 1024) { /* 4k dedicated L3 entries */
            l3_entries = min_l3_entries * 1024;
            shared_l3_banks = 0;
        } else if (l3_entries <= max_l3_entries * 1024) {
            l3_entries -= min_l3_entries * 1024;
            shared_l3_banks = (l3_entries + (shared_bank_size * 1024 - 1)) / (shared_bank_size *
 1024);
            l3_entries = min_l3_entries * 1024 + shared_l3_banks * shared_bank_size * 1024;
        } else {
            LOG_CLI((BSL_META_U(unit,
                            "The specified l3_mem_entries (%d) exceeds 132k\n"),
                            cfg_l3_entries));
            return SOC_E_PARAM;
        }


        alpm_enable = soc_property_get(unit, spn_L3_ALPM_ENABLE, 0);
        if (alpm_enable && soc_feature(unit, soc_feature_alpm)) {

            if (shared_l2_banks + shared_l3_banks > 2) {
                LOG_CLI((BSL_META_U(unit,
                       "A minimum of 2 shared banks is required for ALPM "
                       "when it is enabled\n")));
                return SOC_E_PARAM;
            }
        } else if (shared_l2_banks + shared_l3_banks > 4) {
            return SOC_E_PARAM;
        }

        sop->memState[L2Xm].index_max = l2_entries - 1;
        sop->memState[L2_ENTRY_ONLYm].index_max = l2_entries - 1;
        sop->memState[L2_ENTRY_ONLY_ECCm].index_max = l2_entries - 1;
        sop->memState[L2_HITDA_ONLYm].index_max = l2_entries / 4 - 1;
        sop->memState[L2_HITSA_ONLYm].index_max = l2_entries / 4 - 1;
        sop->memState[L2_ENTRY_LPm].index_max = min_l2_entries*1024 / 4 - 1;
        sop->memState[L2_ENTRY_ISS_LPm].index_max =
             (l2_entries - min_l2_entries*1024) / 4 - 1;

        /* Adjust L3 table size */
        sop->memState[L3_ENTRY_ONLYm].index_max = l3_entries - 1;
        sop->memState[L3_ENTRY_IPV4_UNICASTm].index_max = l3_entries - 1;
        sop->memState[L3_ENTRY_IPV4_MULTICASTm].index_max = l3_entries / 2 - 1;
        sop->memState[L3_ENTRY_IPV6_UNICASTm].index_max = l3_entries/ 2 - 1;
        sop->memState[L3_ENTRY_IPV6_MULTICASTm].index_max = l3_entries / 4 - 1;
        sop->memState[L3_ENTRY_ONLY_ECCm].index_max = l3_entries - 1;
        sop->memState[L3_ENTRY_HIT_ONLYm].index_max = l3_entries / 4 - 1;
        sop->memState[L3_ENTRY_LPm].index_max = min_l3_entries*1024 / 4 - 1;
        sop->memState[L3_ENTRY_ISS_LPm].index_max =
             (l3_entries - min_l3_entries*1024) / 4 - 1;

        /* Adjust ALPM table size */
        if (alpm_enable) {
            /* The check below is sufficient because if ALPM mode is enabled
            * and number of shared banks used is > 2 we return SOC_E_PARAM
            * in code at the top of this function.
            */
            if (shared_l2_banks + shared_l3_banks > 0) {
                sop->memState[L3_DEFIP_ALPM_RAWm].index_max =
                    (SOC_MEM_INFO(unit, L3_DEFIP_ALPM_RAWm).index_max / 2);
                sop->memState[L3_DEFIP_ALPM_IPV4m].index_max =
                    (SOC_MEM_INFO(unit, L3_DEFIP_ALPM_IPV4m).index_max / 2);
                sop->memState[L3_DEFIP_ALPM_IPV4_1m].index_max =
                    (SOC_MEM_INFO(unit, L3_DEFIP_ALPM_IPV4_1m).index_max / 2);
                sop->memState[L3_DEFIP_ALPM_IPV6_64m].index_max =
                    (SOC_MEM_INFO(unit, L3_DEFIP_ALPM_IPV6_64m).index_max / 2);
                sop->memState[L3_DEFIP_ALPM_IPV6_64_1m].index_max =
                    (SOC_MEM_INFO(unit, L3_DEFIP_ALPM_IPV6_64_1m).index_max / 2);
                sop->memState[L3_DEFIP_ALPM_IPV6_128m].index_max =
                    (SOC_MEM_INFO(unit, L3_DEFIP_ALPM_IPV6_128m).index_max / 2);
                sop->memState[L3_DEFIP_ALPM_HIT_ONLYm].index_max =
                    (SOC_MEM_INFO(unit, L3_DEFIP_ALPM_HIT_ONLYm).index_max / 2);
                sop->memState[L3_DEFIP_ALPM_ECCm].index_max =
                    (SOC_MEM_INFO(unit, L3_DEFIP_ALPM_ECCm).index_max / 2);
            }
        } else {
            sop->memState[L3_DEFIP_ALPM_RAWm].index_max = -1;
            sop->memState[L3_DEFIP_ALPM_IPV4m].index_max = -1;
            sop->memState[L3_DEFIP_ALPM_IPV4_1m].index_max = -1;
            sop->memState[L3_DEFIP_ALPM_IPV6_64m].index_max = -1;
            sop->memState[L3_DEFIP_ALPM_IPV6_64_1m].index_max = -1;
            sop->memState[L3_DEFIP_ALPM_IPV6_128m].index_max = -1;
            sop->memState[L3_DEFIP_ALPM_HIT_ONLYm].index_max = -1;
            sop->memState[L3_DEFIP_ALPM_ECCm].index_max = -1;
        }
   }
   return SOC_E_NONE;
}
#endif /* BCM_ESW_SUPPORT && BCM_IPROC_SUPPORT */



/*
 * Function:
 *      soc_info_config
 * Parameters:
 *      unit - Unit number
 *      soc - soc_control_t associated with this unit
 * Purpose:
 *      Fill in soc_info structure for a newly attached unit.
 *      Generates bitmaps and various arrays based on block and
 *      ports that the hardware has enabled.
 *
 *      This isn't STATIC so that pcid can get at it.
 */

int
soc_info_config(int unit, soc_control_t *soc)
{
    soc_info_t          *si;
    soc_pbmp_t          pbmp_valid;
    soc_pbmp_t          pbmp_xport_xe;
    soc_pbmp_t          pbmp_xport_ge;
    soc_pbmp_t          pbmp_gport_fe;
    soc_pbmp_t          pbmp_xport_cpri;
    soc_pbmp_t          pbmp_roe_compression;
    soc_pbmp_t          pbmp_gport_stack;   /* GE port in Higig Lite mode */
    soc_pbmp_t          pbmp_gx25g;         /* GX port 24 Gbps capable */
    soc_pbmp_t          pbmp_gx21g;         /* GX port 21 Gbps capable */
    soc_pbmp_t          pbmp_gx20g;         /* GX port 20 Gbps capable */
    soc_pbmp_t          pbmp_gx16g;         /* GX port 16 Gbps capable */
    soc_pbmp_t          pbmp_gx13g;         /* GX port 13 Gbps capable */
    soc_pbmp_t          pbmp_gx12g;         /* GX port 12 Gbps capable */
    soc_pbmp_t          pbmp_mxq;           /* MXQ port */
    soc_pbmp_t          pbmp_mxq1g;         /* MXQ port 1 Gbps capable */
    soc_pbmp_t          pbmp_mxq2p5g;       /* MXQ port 2.5 Gbps capable */
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
    soc_pbmp_t          pbmp_mxq10g;        /* MXQ port 10 Gbps capable */
#endif /* (BCM_KATANA_SUPPORT) || (BCM_KATANA2_SUPPORT) */
    soc_pbmp_t          pbmp_mxq13g;        /* MXQ port 13 Gbps capable */
    soc_pbmp_t          pbmp_mxq21g;        /* MXQ port 20,21 Gbps capable */
#if defined(BCM_SHADOW_SUPPORT) || defined(BCM_SABER2_SUPPORT)
    soc_pbmp_t          pbmp_xl;            /* XL port */
#if defined(BCM_SABER2_SUPPORT)
    soc_pbmp_t          pbmp_xl1g;           /* XL port 1 Gbps capable */
    soc_pbmp_t          pbmp_xl2p5g;         /* XL port 2.5 Gbps capable */
#endif
    soc_pbmp_t          pbmp_xl10g;         /* XL port 10 Gbps capable */
#endif
#ifdef BCM_SHADOW_SUPPORT
    soc_pbmp_t          pbmp_xl40g;         /* XL port 40 Gbps capable */
    soc_pbmp_t          pbmp_xl12g;         /* XL port 12 Gbps capable (SCH) */
#endif /* BCM_SHADOW_SUPPORT */
#if defined(BCM_METROLITE_SUPPORT)
    ml_pbmp_t ml_pbmp = {0};
    soc_pbmp_t          pbmp_pp_unsed;
#endif
    soc_pbmp_t          pbmp_xg2p5g;        /* XG port 2.5 Gbps capable */
    soc_pbmp_t          pbmp_xq10g;         /* XQ port 10 Gbps capable */
    soc_pbmp_t          pbmp_xq12g;         /* XQ port 12 Gbps capable */
    soc_pbmp_t          pbmp_xq13g;         /* XQ port 13 Gbps capable */
    soc_pbmp_t          pbmp_xq2p5g;        /* XQ port 2.5 Gbps capable */
    soc_pbmp_t          pbmp_xq;            /* XQ port (any type) */
    soc_pbmp_t          pbmp_hyplite;       /* Hyperlite port */
    soc_pbmp_t          pbmp_tmp;
    soc_pbmp_t          pbmp_disabled;      /* forcefully disabled ports */
    soc_pbmp_t          pbmp_pp;            /* Packet Processing Port */
    uint16              dev_id;
    uint8               rev_id;
    uint16              drv_dev_id;
    uint16               drv_rev_id;
    int                 port, phy_port, max_port, blk, bindex, pno=0, mem;
    char                *bname, *old_bname;
    int                 blktype, old_blktype, port_idx, instance;
    char                instance_string[3];
#ifdef BCM_HURRICANE_SUPPORT
    int                 dxgxs = 0;
    uint32              regval;
#endif /* BCM_HURRICANE_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
    int                 kt_mxqport_index=0;
    int                 kt_mxqport2[4]={27,32,33,34};
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
    int                 mxq7_blk=0;
/* Just keeping below for informatory for short duration */
#if 0
    int                 kt2_mxqport_num=0;
    int                 kt2_mxqport_index=0;
    int                 kt2_mxqports_list[10][4]={
                        { 1, 2, 3, 4},  /* MXQPORT0 */
                        { 5, 6, 7, 8},  /* MXQPORT1 */
                        { 9,10,11,12},  /* MXQPORT2 */
                        {13,14,15,16},  /* MXQPORT3 */
                        {17,18,19,20},  /* MXQPORT4 */
                        {21,22,23,24},  /* MXQPORT5 */
                        {25,35,36,37},  /* MXQPORT6 */
                        {26,38,39,40},  /* MXQPORT7 */
                        {27,32,33,34},  /* MXQPORT8 */
                        {28,29,30,31}   /* MXQPORT9 */
                        };
#endif
    kt2_pbmp_t kt2_pbmp={0};
#endif /* BCM_KATANA2_SUPPORT */
#if defined(BCM_SABER2_SUPPORT)
    sb2_pbmp_t sb2_pbmp={0};
#endif
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) \
    || defined(BCM_TRIDENT2_SUPPORT)
    int                 rv = 0;
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
    static int latency[SOC_MAX_NUM_DEVICES] = {0};
#endif

    si = &soc->info;
    sal_memset((void *)si, 0, sizeof(soc_info_t));

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &drv_dev_id, &drv_rev_id);

    si->driver_type = soc->chip_driver->type;
    si->driver_group = soc_chip_type_map[si->driver_type];
    si->num_cpu_cosq = 8;
    si->num_mmu_base_cosq = 0;
    si->modid_count = 1;
    si->modid_max = 31;    /* See SOC_MODID_MAX(unit) */
    si->modport_max = 31;
    si->modport_max_first = -1;
    si->trunk_bit_pos = 5; /* 6th bit from right. See SOC_TRUNK_BIT_POS(unit) */
    si->port_addr_max = 31;
    si->vlan_xlate_glp_wildcard = 0x3fff; /* For most chips it is 3fff but can vary */
    si->cpu_hg_index = -1;
    si->cpu_hg_pp_port_index = -1;
    si->port_class_max = 64;    /* By default 64 classes are supported */
    si->num_flex_ingress_pools = 0;
    si->num_flex_egress_pools = 0;
    si->size_flex_ingress_pool = 0;
    si->size_flex_egress_pool = 0;
    si->pcie_phy_addr = 0;
    si->sflow_range_max = 24;
    si->nat_id_max = 2048;
    si->fp_hg_port_offset = -1;

    si->max_mtu = 16360;

    SOC_PBMP_CLEAR(pbmp_xport_xe);
    SOC_PBMP_CLEAR(pbmp_xport_cpri);
    SOC_PBMP_CLEAR(pbmp_xport_ge);
    SOC_PBMP_CLEAR(pbmp_gport_fe);
    SOC_PBMP_CLEAR(pbmp_roe_compression);
    SOC_PBMP_CLEAR(pbmp_gport_stack);
    SOC_PBMP_CLEAR(pbmp_disabled);
    SOC_PBMP_CLEAR(pbmp_pp);
    SOC_PBMP_CLEAR(si->linkphy_pbm);
    SOC_PBMP_CLEAR(si->lp.bitmap);
    SOC_PBMP_CLEAR(si->subtag_pbm);
    SOC_PBMP_CLEAR(si->subtag.bitmap);
    SOC_PBMP_CLEAR(si->subtag_allowed_pbm);
    SOC_PBMP_CLEAR(si->linkphy_allowed_pbm);
    SOC_PBMP_CLEAR(si->linkphy_pp_port_pbm);
    SOC_PBMP_CLEAR(si->subtag_pp_port_pbm);
    si->subtag_enabled = 0;
    si->linkphy_enabled = 0;

    /* XGPORT is set to 1G if it is not included in one of the pbmp below */
    SOC_PBMP_CLEAR(pbmp_xg2p5g);

    /* XQPORT is set to 1G if it is not included in one of the pbmp below */
    SOC_PBMP_CLEAR(pbmp_xq2p5g);
    SOC_PBMP_CLEAR(pbmp_xq10g);
    SOC_PBMP_CLEAR(pbmp_xq12g);
    SOC_PBMP_CLEAR(pbmp_xq13g);
    SOC_PBMP_CLEAR(pbmp_xq);

#if defined(BCM_SHADOW_SUPPORT) || defined(BCM_SABER2_SUPPORT)
    /* XLPORT is set to one of the pbmp below */
    SOC_PBMP_CLEAR(pbmp_xl);
    SOC_PBMP_CLEAR(pbmp_xl10g);
#endif
#if defined(BCM_SHADOW_SUPPORT)
    SOC_PBMP_CLEAR(pbmp_xl12g);
    SOC_PBMP_CLEAR(pbmp_xl40g);
#endif /* BCM_SHADOW_SUPPORT */

    /* GXPORT is set to 10G if it is not included in one of the pbmp below */
    SOC_PBMP_CLEAR(pbmp_gx12g);
    SOC_PBMP_CLEAR(pbmp_gx13g);
    SOC_PBMP_CLEAR(pbmp_gx16g);
    SOC_PBMP_CLEAR(pbmp_gx20g);
    SOC_PBMP_CLEAR(pbmp_gx21g);
    SOC_PBMP_CLEAR(pbmp_gx25g);
    SOC_PBMP_CLEAR(pbmp_mxq);
    SOC_PBMP_CLEAR(pbmp_mxq1g);
    SOC_PBMP_CLEAR(pbmp_mxq2p5g);
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
    SOC_PBMP_CLEAR(pbmp_mxq10g);
#endif /* defined(BCM_KATANA_SUPPORT) || (BCM_KATANA2_SUPPORT) */
    SOC_PBMP_CLEAR(pbmp_mxq13g);
    SOC_PBMP_CLEAR(pbmp_mxq21g);
#if defined(BCM_SABER2_SUPPORT)
    SOC_PBMP_CLEAR(pbmp_xl1g);
    SOC_PBMP_CLEAR(pbmp_xl2p5g);
#endif

    SOC_PBMP_CLEAR(si->s_pbm);  /* 10/100/1000/2500 Mbps comboserdes */
    SOC_PBMP_CLEAR(si->gmii_pbm);
    SOC_PBMP_CLEAR(si->lb_pbm);
    SOC_PBMP_CLEAR(si->mmu_pbm);
    SOC_PBMP_CLEAR(pbmp_hyplite);

    SOC_PBMP_CLEAR(si->pbm_ext_mem);
    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        si->max_port_p2m_mapping[port] = -1;
    }

    si->num_pipe = 1;
    /* mmu_lossless will be updated by chip init code, wherever applicable */
    si->mmu_lossless = -1;

    /*
     * Used to implement the SOC_IS_*(unit) macros
     */
    switch (drv_dev_id) {
#if defined(BCM_HERCULES_SUPPORT)
    case BCM5675_DEVICE_ID:
        si->chip = SOC_INFO_CHIP_HERCULES15;
        soc->base_modid = -1;
        si->modid_count = 0;
        break;
#endif /* BCM_HERCULES_SUPPORT */
    case BCM56102_DEVICE_ID:
        si->chip = SOC_INFO_CHIP_FHX;
        si->chip_type = SOC_INFO_CHIP_TYPE_FELIX;
        SOC_PBMP_WORD_SET(pbmp_gport_fe, 0, 0x00ffffff);
        si->modid_max = 63;
        si->hg_offset = 24;     /* SOC_HG_OFFSET */
        break;
    case BCM56304_DEVICE_ID:
        si->chip = SOC_INFO_CHIP_FHX;
        si->chip_type = SOC_INFO_CHIP_TYPE_HELIX;
        si->modid_max = 63;
        si->hg_offset = 24;     /* SOC_HG_OFFSET */
        break;
    case BCM56112_DEVICE_ID:
        si->chip = SOC_INFO_CHIP_FHX;
        si->chip_type = SOC_INFO_CHIP_TYPE_FELIX15;
        SOC_PBMP_WORD_SET(pbmp_gport_fe, 0, 0x00ffffff);
        si->modid_max = 63;
        si->hg_offset = 24;     /* SOC_HG_OFFSET */
        break;
    case BCM56314_DEVICE_ID:
        si->chip = SOC_INFO_CHIP_FHX;
        si->chip_type = SOC_INFO_CHIP_TYPE_HELIX15;
        si->modid_max = 63;
        si->hg_offset = 24;     /* SOC_HG_OFFSET */
        break;
    case BCM56504_DEVICE_ID:
        si->chip = SOC_INFO_CHIP_FIREBOLT;
        si->modid_max = 63;
        si->hg_offset = 24;     /* SOC_HG_OFFSET */
        break;
    case BCM56800_DEVICE_ID:
        switch (dev_id) {
        case BCM56580_DEVICE_ID:
            si->chip = SOC_INFO_CHIP_GOLDWING;
            SOC_CHIP_STRING(unit) = "goldwing";
            break;
        case BCM56700_DEVICE_ID:
        case BCM56701_DEVICE_ID:
            si->chip = SOC_INFO_CHIP_HUMV;
            SOC_CHIP_STRING(unit) = "humv";
            break;
        default:
            si->chip = SOC_INFO_CHIP_BRADLEY;
            break;
        }
        si->modid_max = 127;
        si->num_cpu_cosq = 16;
        break;
#if defined(BCM_RAPTOR_SUPPORT)
    case BCM56218_DEVICE_ID:
        si->chip = SOC_INFO_CHIP_RAPTOR;
        SOC_PBMP_WORD_SET(pbmp_gport_stack, 0, 0x00000006);
        SOC_PBMP_WORD_SET(si->s_pbm, 0, 0x00000006);
        SOC_PBMP_WORD_SET(si->hg2_pbm, 0, 0x00000006);
        SOC_PBMP_WORD_SET(si->gmii_pbm, 0, 0x00000008);
        si->modid_max = 15;     /* See SOC_MODID_MAX(unit) */
        /* 7th bit from right. See SOC_TRUNK_BIT_POS(unit)*/
    si->trunk_bit_pos = 6;
        si->port_addr_max = 63;
        si->hg_offset = 0;      /* SOC_HG_OFFSET */
        break;
    case BCM56224_DEVICE_ID:
        si->chip = SOC_INFO_CHIP_RAVEN;
        SOC_PBMP_WORD_SET(pbmp_gport_stack, 0, 0x00000036);
        SOC_PBMP_WORD_SET(si->s_pbm, 0, 0x00000036);
        SOC_PBMP_WORD_SET(si->hg2_pbm, 0, 0x00000036);
        SOC_PBMP_WORD_SET(si->gmii_pbm, 0, 0x00000008);
        si->modid_max = 15;     /* See SOC_MODID_MAX(unit) */
        /* 7th bit from right. See SOC_TRUNK_BIT_POS(unit)*/
        si->trunk_bit_pos = 6;
        si->port_addr_max = 63;
        si->hg_offset = 0;      /* SOC_HG_OFFSET */
        break;
    case BCM53312_DEVICE_ID:
    case BCM53313_DEVICE_ID:
    case BCM53314_DEVICE_ID:
    case BCM53322_DEVICE_ID:
    case BCM53323_DEVICE_ID:
    case BCM53324_DEVICE_ID:
        si->chip = SOC_INFO_CHIP_HAWKEYE;
        si->modid_max = 0;     /* See SOC_MODID_MAX(unit) */
        /* 7th bit from right. See SOC_TRUNK_BIT_POS(unit)*/
        si->trunk_bit_pos = 6;
        si->port_addr_max = 31;
        si->hg_offset = 0;      /* SOC_HG_OFFSET */
        break;
#endif /* BCM_RAPTOR_SUPPORT */
    case BCM56514_DEVICE_ID:
        si->chip = SOC_INFO_CHIP_FIREBOLT2;
        si->modid_max = 63;
        si->hg_offset = 24;     /* SOC_HG_OFFSET */
        break;
    case BCM56624_DEVICE_ID:
        SOC_PBMP_WORD_SET(si->s_pbm, 0, 0x00000002);
        si->chip = SOC_INFO_CHIP_TRIUMPH;
        si->modid_max = 127;    /* See SOC_MODID_MAX(unit) */
        si->hg_offset = 28;     /* SOC_HG_OFFSET */
        /*
         * 14th bit from right. See SOC_TRUNK_BIT_POS(unit)
         *     Note that it is not together with port/trunk-id.
         */
    si->trunk_bit_pos = 13;
        si->port_addr_max = 63;
        si->num_cpu_cosq = 48;
        if (soc_property_get(unit, spn_MODULE_64PORTS, 0)) {
            si->modport_max = 63;
        } else {
            /* Adjust modid count based on dual-modid config parameter settings.
             * By default, dual-modid is enabled (32-port module mode) to allow
             * compatibility with other devices.
             */
            si->modid_count = 2;
        }
        break;
    case BCM56680_DEVICE_ID:
        SOC_PBMP_WORD_SET(si->s_pbm, 0, 0x00000002);
        si->chip = SOC_INFO_CHIP_VALKYRIE;
        si->modid_max = 127;    /* See SOC_MODID_MAX(unit) */
        si->hg_offset = 28;     /* SOC_HG_OFFSET */
        /*
         * 14th bit from right. See SOC_TRUNK_BIT_POS(unit)
         *     Note that it is not together with port/trunk-id.
         */
        si->trunk_bit_pos = 13;
        si->port_addr_max = 63;
        si->num_cpu_cosq = 48;
        if (soc_property_get(unit, spn_MODULE_64PORTS, 0)) {
            si->modport_max = 63;
        } else {
            /* Adjust modid count based on dual-modid config parameter settings.
             * By default, dual-modid is enabled (32-port module mode) to allow
             * compatibility with other devices.
             */
            si->modid_count = 2;
        }
        break;
    case BCM56820_DEVICE_ID:
        si->chip = SOC_INFO_CHIP_SCORPION;
        si->modid_max = 255;
        si->port_addr_max = 63;
        si->num_cpu_cosq = 32;
        si->trunk_bit_pos = 14;
        si->vlan_xlate_glp_wildcard = 0x7fff; /* glp wildcard value */
        si->sflow_range_max = 16;
        /* SOC_HG_OFFSET = 0, default */
        break;
    case BCM56725_DEVICE_ID:
        si->chip = SOC_INFO_CHIP_CONQUEROR;
        si->modid_max = 255;
        si->port_addr_max = 63;
        si->num_cpu_cosq = 32;
        si->trunk_bit_pos = 14;
        si->vlan_xlate_glp_wildcard = 0x7fff; /* glp wildcard value */
        /* SOC_HG_OFFSET = 0, default */
        break;
    case BCM56634_DEVICE_ID:
        SOC_PBMP_WORD_SET(si->s_pbm, 0, 0x00000002);
        SOC_PBMP_WORD_SET(si->lb_pbm, 1, 0x00400000);
        si->lb_port = 54;
        SOC_PBMP_WORD_SET(si->mmu_pbm, 1, 0x01800000);
        si->chip = SOC_INFO_CHIP_TRIUMPH2;
        si->modid_max = 127;    /* See SOC_MODID_MAX(unit) */
        si->hg_offset = 26;     /* SOC_HG_OFFSET */
        /*
         * 14th bit from right. See SOC_TRUNK_BIT_POS(unit)
         *     Note that it is not together with port/trunk-id.
         */
    si->trunk_bit_pos = 13;
        si->port_addr_max = 63;
        si->num_cpu_cosq = 48;
        if (soc_property_get(unit, spn_MODULE_64PORTS, 0)) {
            si->modport_max = 63;
        } else {
            /* Adjust modid count based on dual-modid config parameter settings.
             * By default, dual-modid is enabled (32-port module mode) to allow
             * compatibility with other devices.
             */
            si->modid_count = 2;
        }
        si->num_time_interface = 1;
        switch (dev_id) {
        case BCM56634_DEVICE_ID:
        case BCM56538_DEVICE_ID:
            if (soc_property_get(unit, spn_BCM56634_48G_4X12, 0) ||
                soc_property_get(unit, spn_BCM56634_48G_2X24, 0) ||
                soc_property_get(unit, spn_BCM56538_48G_4X12, 0) ||
                soc_property_get(unit, spn_BCM56538_48G_2X24, 0)) {
                si->internal_loopback = 1;
            } else {
                si->internal_loopback = 0;
            }
            break;
        case BCM56636_DEVICE_ID:
            if (soc_property_get(unit, spn_BCM56636_24G_6X12, 0) ||
                soc_property_get(unit, spn_BCM56636_2X12_2X24, 0)) {
                si->internal_loopback = 1;
            } else {
                si->internal_loopback = 0;
            }
            break;
        case BCM56638_DEVICE_ID:
            if (soc_property_get(unit, spn_BCM56638_8X12, 0) ||
                soc_property_get(unit, spn_BCM56638_4X12_2X24, 0)) {
                si->internal_loopback = 1;
            } else {
                si->internal_loopback = 0;
            }
            break;
        default:
            si->internal_loopback = 1;
            break;
        }
        break;
    case BCM56524_DEVICE_ID:
        SOC_PBMP_WORD_SET(si->lb_pbm, 1, 0x00400000);
        si->lb_port = 54;
        SOC_PBMP_WORD_SET(si->mmu_pbm, 1, 0x01800000);
        si->chip = SOC_INFO_CHIP_APOLLO;
        si->modid_max = 127;    /* See SOC_MODID_MAX(unit) */
        si->hg_offset = 26;     /* SOC_HG_OFFSET */
        /*
         * 14th bit from right. See SOC_TRUNK_BIT_POS(unit)
         *     Note that it is not together with port/trunk-id.
         */
    si->trunk_bit_pos = 13;
        si->port_addr_max = 63;
        si->num_cpu_cosq = 48;
        if (soc_property_get(unit, spn_MODULE_64PORTS, 0)) {
            si->modport_max = 63;
        } else {
            /* Adjust modid count based on dual-modid config parameter settings.
             * By default, dual-modid is enabled (32-port module mode) to allow
             * compatibility with other devices.
             */
            si->modid_count = 2;
        }
        si->num_time_interface = 1;
        si->internal_loopback = 1;
        break;
    case BCM56685_DEVICE_ID:
        SOC_PBMP_WORD_SET(si->lb_pbm, 1, 0x00400000);
        si->lb_port = 54;
        SOC_PBMP_WORD_SET(si->mmu_pbm, 1, 0x01800000);
        si->chip = SOC_INFO_CHIP_VALKYRIE2;
        si->modid_max = 127;    /* See SOC_MODID_MAX(unit) */
        si->hg_offset = 26;     /* SOC_HG_OFFSET */
        /*
         * 14th bit from right. See SOC_TRUNK_BIT_POS(unit)
         *     Note that it is not together with port/trunk-id.
         */
    si->trunk_bit_pos = 13;
        si->port_addr_max = 63;
        si->num_cpu_cosq = 48;
        if (soc_property_get(unit, spn_MODULE_64PORTS, 0)) {
            si->modport_max = 63;
        } else {
            /* Adjust modid count based on dual-modid config parameter settings.
             * By default, dual-modid is enabled (32-port module mode) to allow
             * compatibility with other devices.
             */
            si->modid_count = 2;
        }
        si->num_time_interface = 1;
        si->internal_loopback = 1;
        break;
#if defined(BCM_ENDURO_SUPPORT)
    case BCM56132_DEVICE_ID:
    case BCM56134_DEVICE_ID:
    case BCM56331_DEVICE_ID:
    case BCM56333_DEVICE_ID:
    case BCM56334_DEVICE_ID:
    case BCM56320_DEVICE_ID:
    case BCM56321_DEVICE_ID:
    case BCM56338_DEVICE_ID:
    case BCM56230_DEVICE_ID:
    case BCM56231_DEVICE_ID:
        if(dev_id == BCM56338_DEVICE_ID) {
            SOC_PBMP_WORD_SET(pbmp_gport_stack, 0, 0xc000000);
        } else if(dev_id == BCM56132_DEVICE_ID) {
            SOC_PBMP_WORD_SET(pbmp_gport_stack, 0, 0x30000000);
        } else if((dev_id == BCM56334_DEVICE_ID) || (dev_id == BCM56320_DEVICE_ID) ||
                  (dev_id == BCM56321_DEVICE_ID)){
            SOC_PBMP_WORD_SET(pbmp_gport_stack, 0, 0x3c000000);
        }
        /* SOC_PBMP_WORD_SET(si->s_pbm, 0, 0x00000002);
        SOC_PBMP_WORD_SET(si->mmu_pbm, 1, 0x01800000); */
        SOC_PBMP_WORD_SET(si->lb_pbm, 0, 0x00000002);
        si->chip = SOC_INFO_CHIP_ENDURO;
        si->modid_max = 63;    /* See SOC_MODID_MAX(unit) */
        si->hg_offset = 26;     /* SOC_HG_OFFSET */
        /*
         * 14th bit from right. See SOC_TRUNK_BIT_POS(unit)
         *     Note that it is not together with port/trunk-id.
         */
        si->trunk_bit_pos = 13;
        /*
         * Note that maximum port number for enduro is 32. But port_addr_max
         *  is defined as 63 to make enduro compatible to other TRX families.
         */
        si->port_addr_max = 63;
        si->num_cpu_cosq = 48;
        si->internal_loopback = 1;
        si->num_time_interface = 1;
        si->fp_hg_port_offset = 4;
        break;
#endif /* BCM_ENDURO_SUPPORT */

#ifdef BCM_SHADOW_SUPPORT
    case BCM88732_DEVICE_ID:
        si->chip = SOC_INFO_CHIP_SHADOW;
        si->modid_max = 255;
        si->port_addr_max = 63;
        si->num_cpu_cosq = 32;
        si->trunk_bit_pos = 14;
        si->pcie_phy_addr = 0x9a;
        break;
#endif /* BCM_SHAODOW_SUPPORT */

#if defined(BCM_HURRICANE_SUPPORT)
    case BCM56140_DEVICE_ID:
    case BCM56142_DEVICE_ID:
    case BCM56143_DEVICE_ID:
    case BCM56144_DEVICE_ID:
    case BCM56146_DEVICE_ID:
    case BCM56147_DEVICE_ID:
    case BCM56149_DEVICE_ID:
        /* Read DXGXS input */
        if (SAL_BOOT_PLISIM) {
            dxgxs=1;
        } else {
            /* At this point, Endianness may or may not be configured. Compensate for it */
            soc_pci_getreg(unit, soc_reg_addr(unit, CMIC_MISC_STATUSr, 0, 0), &regval);
            if ((CMVEC(unit).big_endian_pio ? 1 : 0) !=
                ((soc_pci_read(unit, CMIC_ENDIAN_SELECT) & ES_BIG_ENDIAN_PIO) ? 1 : 0) )
            {
                dxgxs = (regval & 0x01000000) ? 1 : 0;
            } else {
                dxgxs = (regval & 0x01) ? 1 : 0;
            }
        }
        SOC_PBMP_CLEAR(pbmp_gport_stack);
        if(dev_id == BCM56142_DEVICE_ID) {
            if (soc_property_get(unit, spn_BCM5614X_CONFIG, 1) == 0) {
                /* Full Capacity (24x1G + 4xHGd) for testing */
                SOC_PBMP_WORD_SET(pbmp_gport_stack, 0, 0x3c000000);
            } else if (soc_property_get(unit, spn_BCM5614X_CONFIG, 1) == 2) {
                /* 24x1G + 2xHG[13] */
                SOC_PBMP_WORD_SET(pbmp_gport_stack, 0, 0x14000000);
            } else {
                SOC_PBMP_WORD_SET(pbmp_gport_stack, 0, dxgxs ? 0x34000000 : 0x1c000000);
            }
        } else if(dev_id == BCM56143_DEVICE_ID) {
            if (!soc_property_get(unit, spn_BCM5614X_CONFIG, 0)) {
                SOC_PBMP_WORD_SET(pbmp_gport_stack, 0, dxgxs ? 0x0c000000 : 0x30000000);
            }
        }

        si->chip = SOC_INFO_CHIP_HURRICANE;
        si->modid_max = 63;    /* See SOC_MODID_MAX(unit) */
        si->hg_offset = 26;     /* SOC_HG_OFFSET */
        /*
         * 14th bit from right. See SOC_TRUNK_BIT_POS(unit)
         *     Note that it is not together with port/trunk-id.
         */
        si->trunk_bit_pos = 13;
        /*
         * Note that maximum port number for hurricane is 32. But port_addr_max
         *  is defined as 63 to make hurricane compatible to other TRX families.
         */
        si->port_addr_max = 63;
        si->num_cpu_cosq = 8;
        si->internal_loopback = 0;
        si->num_time_interface = 1;
        si->fp_hg_port_offset = 4;
        break;
#endif /* BCM_HURRICANE_SUPPORT */

#if defined(BCM_HURRICANE2_SUPPORT)
    case BCM56150_DEVICE_ID:
    case BCM56151_DEVICE_ID:
    case BCM56152_DEVICE_ID:
    case BCM53342_DEVICE_ID:
    case BCM53343_DEVICE_ID:
    case BCM53344_DEVICE_ID:
    case BCM53346_DEVICE_ID:
    case BCM53347_DEVICE_ID:
    case BCM53333_DEVICE_ID:
    case BCM53334_DEVICE_ID:
    case BCM53393_DEVICE_ID:
    case BCM53394_DEVICE_ID:
        
        si->chip = SOC_INFO_CHIP_HURRICANE2;
        si->modid_max = 63;    /* See SOC_MODID_MAX(unit) */
        si->hg_offset = 26;     /* SOC_HG_OFFSET */
        /*
         * 14th bit from right. See SOC_TRUNK_BIT_POS(unit)
         *     Note that it is not together with port/trunk-id.
         */
        si->trunk_bit_pos = 13;
        /*
         * Note that maximum port number for hurricane is 32. But port_addr_max
         *  is defined as 63 to make hurricane compatible to other TRX families.
         */
        si->port_addr_max = 63;
        si->num_cpu_cosq = 8;
        si->internal_loopback = 0;
        si->num_time_interface = 1;
        si->num_flex_ingress_pools = 8;
        si->num_flex_egress_pools = 8;
        si->pcie_phy_addr = 0x2;
        si->num_ucs = 1;
        si->fp_hg_port_offset = 4;
#ifdef BCM_SBUSDMA_SUPPORT
        soc->max_sbusdma_channels = 3;
#endif
        break;
#endif /* BCM_HURRICANE2_SUPPORT */

#if defined(BCM_GREYHOUND_SUPPORT)
    case BCM53400_DEVICE_ID:
        
        si->chip_type = SOC_INFO_CHIP_TYPE_GREYHOUND;

        si->modid_max = 63;    /* See SOC_MODID_MAX(unit) */
        si->hg_offset = 0;  /* SOC_HG_OFFSET */
        /*
         * 14th bit from right. See SOC_TRUNK_BIT_POS(unit)
         *     Note that it is not together with port/trunk-id.
         */
        si->trunk_bit_pos = 13;
        /*
         * Note that maximum port number for greyhound is 32. But port_addr_max
         *  is defined as 63 to make greyhound compatible to other TRX families.
         */
        si->port_addr_max = 63;
        si->num_cpu_cosq = 8;
        si->internal_loopback = 0;
        si->num_time_interface = 1;
        si->nof_pmqs = 1;
        si->num_ucs = 1;
        si->fp_hg_port_offset = 30;
#ifdef BCM_SBUSDMA_SUPPORT
        soc->max_sbusdma_channels = 3;
#endif
        if ((dev_id & 0xF000) == 0x8000) {
            if ((dev_id & 0x0060) != 0x0000) { /* Elkhound and Bloodhound */
                if ((dev_id & 0x00F6) == 0x0054) { /* 53454/53455 */
                    dev_id &= 0xFFF4; /* use 53454 */
                }
                if ((dev_id & 0x00F6) == 0x0056) { /* 53456/53457 */
                    dev_id &= 0xFFF6; /* use 53456 */
                }
                /* no change on Bloodhound */
            } else {
                dev_id &= 0xFF0F;
            }
        }
        break;
#endif /* BCM_GREYHOUND_SUPPORT */

#if defined(BCM_HURRICANE3_SUPPORT)
        case BCM56160_DEVICE_ID:
        case BCM53540_DEVICE_ID:
            si->chip_type = SOC_INFO_CHIP_TYPE_HURRICANE3;

            if (soc_feature(unit, soc_feature_wh2)) {
                si->modid_max = 63;    /* See SOC_MODID_MAX(unit) */
            } else {
                si->modid_max = 255;    /* See SOC_MODID_MAX(unit) */
            }
            si->hg_offset = 0;  /* SOC_HG_OFFSET */
            /*
             * 14th bit from right. See SOC_TRUNK_BIT_POS(unit)
             *     Note that it is not together with port/trunk-id.
             */
            si->trunk_bit_pos = 14;
            /*
             * Note that maximum port number for hurricane is 32. But port_addr_max
             *  is defined as 63 to make hurricane compatible to other TRX families.
             */
            si->port_addr_max = 63;
            si->num_cpu_cosq = 8;
            si->internal_loopback = 0;
            si->num_time_interface = 1;
            si->nof_pmqs = 3;
            si->vlan_xlate_glp_wildcard = 0x7fff;
            si->fp_hg_port_offset = 32;
            si->num_ucs = 1;
#ifdef BCM_SBUSDMA_SUPPORT
            soc->max_sbusdma_channels = 3;
#endif
            break;
#endif /* BCM_HURRICANE3_SUPPORT */

#if defined(BCM_GREYHOUND2_SUPPORT)
        case BCM53570_DEVICE_ID:
#if defined(BCM_FIRELIGHT_SUPPORT)
        case BCM56070_DEVICE_ID:
#endif /* BCM_FIRELIGHT_SUPPORT */
            si->chip_type = SOC_INFO_CHIP_TYPE_GREYHOUND2;

            si->modid_max = 255;	/* See SOC_MODID_MAX(unit) */
            si->hg_offset = 0;	/* SOC_HG_OFFSET */
            si->fp_hg_port_offset = 66;
            /*
             * 15th bit from right. See SOC_TRUNK_BIT_POS(unit)
             *	   Note that it is not together with port/trunk-id.
             */
            si->trunk_bit_pos = 15;
            /*
             * Note that maximum port number for hurricane is 32. But port_addr_max
             *	is defined as 63 to make hurricane compatible to other TRX families.
             */
            si->port_addr_max = 127;
            si->num_cpu_cosq = 8;
            si->internal_loopback = 0;
            si->num_time_interface = 1;
            si->nof_pmqs = 5;
#if defined(BCM_FIRELIGHT_SUPPORT)
            if ((dev_id == BCM56070_DEVICE_ID) ||
                (dev_id == BCM56071_DEVICE_ID) ||
                (dev_id == BCM56072_DEVICE_ID)) {
                si->max_mtu = 12288;
                si->num_ucs = 2;
#if defined(BCM_TSN_SUPPORT)
                /* TSN_TAF Gate init */
                si->tsn_taf_gate_num = 64;
#endif /* BCM_TSN_SUPPORT */
            } else
#endif
            {
                si->num_ucs = 1;
#if defined(BCM_TSN_SUPPORT)
                /* TSN_TAF Gate init */
                si->tsn_taf_gate_num = 128;
#endif /* BCM_TSN_SUPPORT */
            }
            si->vlan_xlate_glp_wildcard = 0xffff;
            si->port_class_max = 128;  /* egr_vlan_xlate port class max*/
#ifdef BCM_SBUSDMA_SUPPORT
            soc->max_sbusdma_channels = 3;
            soc->tdma_ch = 0;
            soc->tslam_ch = 1;
            soc->desc_ch = 2;
#endif
            break;
#endif /* BCM_GREYHOUND2_SUPPORT */

    case BCM56840_DEVICE_ID:
        if (dev_id == BCM56745_DEVICE_ID || dev_id == BCM56743_DEVICE_ID ||
            dev_id == BCM56746_DEVICE_ID || dev_id == BCM56744_DEVICE_ID ||
            dev_id == BCM56742_DEVICE_ID) {
            si->chip = SOC_INFO_CHIP_TITAN;
        } else {
            si->chip = SOC_INFO_CHIP_TRIDENT;
        }
        si->port_class_max = 128;  /* egr_vlan_xlate port class max*/
        si->modid_max = 255;    /* See SOC_MODID_MAX(unit) */
        si->num_pipe = 2;
        /* si->hg_offset = 0;    SOC_HG_OFFSET */
        /*
         * 16th bit from right. See SOC_TRUNK_BIT_POS(unit)
         *     Note that it is not together with port/trunk-id.
         */
        si->trunk_bit_pos = 15;
        /* port_addr_max has to be 2^n -1 */
        si->port_addr_max = 127;
        si->num_cpu_cosq = 48;
        si->pcie_phy_addr = 0xfa;
        if (soc_property_get(unit, spn_MODULE_64PORTS, 0)) {
            /* Use this property as single modid indicator for now */
            si->modport_max = 127;
        } else {
            /* By default, dual-modid is enabled (32-port module mode) to allow
             * compatibility with other devices. In this mode, port 64 will
             * not be globally addressable, however still allocate 3 modid
             * to allow using mod/port gport to address all ports.
             */
            si->modid_count = 3;
        }
        si->num_time_interface = 1;
        si->vlan_xlate_glp_wildcard = 0xffff; /* glp wildcard value */
        break;

#ifdef BCM_HELIX4_SUPPORT
    case BCM56548H_DEVICE_ID:
    case BCM56548_DEVICE_ID:
    case BCM56547_DEVICE_ID:
    case BCM56346_DEVICE_ID:
    case BCM56345_DEVICE_ID:
    case BCM56344_DEVICE_ID:
    case BCM56342_DEVICE_ID:
    case BCM56340_DEVICE_ID:
    case BCM56049_DEVICE_ID:
    case BCM56048_DEVICE_ID:
    case BCM56047_DEVICE_ID:
    case BCM56042_DEVICE_ID:
    case BCM56041_DEVICE_ID:
    case BCM56040_DEVICE_ID:
        si->chip = SOC_INFO_CHIP_HELIX4;
        si->chip_type = SOC_INFO_CHIP_TYPE_HELIX4;
        si->port_class_max = 128;  /* egr_vlan_xlate port class max*/
        si->modid_max = 255;    /* See SOC_MODID_MAX(unit) */
        /* si->hg_offset = 0;    SOC_HG_OFFSET */
        /*
         * 16th bit from right. See SOC_TRUNK_BIT_POS(unit)
         *      Note that it is not together with port/trunk-id.
         */
        si->trunk_bit_pos = 15;
        si->port_addr_max = 127; /* port_addr_max has to be 2^n -1 */
        si->num_cpu_cosq = 44;
        /* add multiple modid */
        si->num_time_interface = 1;
        si->vlan_xlate_glp_wildcard = 0xffff; /* glp wildcard value */
        si->pcie_phy_addr = 0x5;
#ifdef BCM_SBUSDMA_SUPPORT
        soc->max_sbusdma_channels = 3;
        soc_hx4_blk_counter_config(unit);
#endif /* BCM_SBUSDMA_SUPPORT */
        si->num_ucs = 2;
        if (!soc_feature(unit, soc_feature_global_meter)) {
            si->global_meter_pools = 0;
            si->global_meter_size_of_pool = 0;
            si->global_meter_max_size_of_pool = 0;
            si->num_flex_ingress_pools = 0;
            si->num_flex_egress_pools = 0;
        } else {
            si->global_meter_pools = 8;
            si->global_meter_size_of_pool = 1024;
            si->global_meter_max_size_of_pool = 1024;
            si->num_flex_ingress_pools = 8;
            si->num_flex_egress_pools = 8;
        }
        if (soc_property_get(unit, spn_MODULE_64PORTS, 1)) {
            si->modport_max = 63;
        } else {
            /* Adjust modid count based on dual-modid config parameter settings.
             * By default, dual-modid is enabled (32-port module mode) to allow
             * compatibility with other devices.
             */
            si->modid_count = 2;
        }

        break;
#endif /* BCM_HELIX4_SUPPORT */

    case BCM56640_DEVICE_ID:
    case BCM56643_DEVICE_ID:
    case BCM56644_DEVICE_ID:
    case BCM56648_DEVICE_ID:
    case BCM56649_DEVICE_ID:
    case BCM56540_DEVICE_ID:
    case BCM56541_DEVICE_ID:
    case BCM56542_DEVICE_ID:
    case BCM56543_DEVICE_ID:
    case BCM56544_DEVICE_ID:
    case BCM56545_DEVICE_ID:
    case BCM56546_DEVICE_ID:
    case BCM56044_DEVICE_ID:
    case BCM56045_DEVICE_ID:
    case BCM56046_DEVICE_ID:
        si->chip = SOC_INFO_CHIP_TRIUMPH3;
        si->modid_max = 255;    /* See SOC_MODID_MAX(unit) */
        si->port_class_max = 128;  /* egr_vlan_xlate port class max*/
        /* si->hg_offset = 0;    SOC_HG_OFFSET */
        /*
         * 16th bit from right. See SOC_TRUNK_BIT_POS(unit)
         *     Note that it is not together with port/trunk-id.
         */
        si->trunk_bit_pos = 15;
        si->port_addr_max = 127; /* port_addr_max has to be 2^n -1 */
        si->num_cpu_cosq = (rev_id >= BCM56640_B0_REV_ID) ? 45 : 48;
        /* add multiple modid */
        si->num_time_interface = 1;
        si->vlan_xlate_glp_wildcard = 0xffff; /* glp wildcard value */
        si->pcie_phy_addr = 0x9e;
#ifdef BCM_SBUSDMA_SUPPORT
        soc->max_sbusdma_channels = 3;
#ifdef BCM_TRIUMPH3_SUPPORT
        soc_tr3_blk_counter_config(unit);
#endif /* BCM_TRIUMPH3_SUPPORT */
#endif /* BCM_SBUSDMA_SUPPORT */
        si->global_meter_pools = 8;
        si->global_meter_size_of_pool = 4096;
        si->global_meter_max_size_of_pool = 4096;
        if (dev_id == BCM56540_DEVICE_ID ||
            dev_id == BCM56541_DEVICE_ID ||
            dev_id == BCM56542_DEVICE_ID ||
            dev_id == BCM56543_DEVICE_ID ||
            dev_id == BCM56544_DEVICE_ID ||
            dev_id == BCM56545_DEVICE_ID ||
            dev_id == BCM56546_DEVICE_ID) {
            si->global_meter_size_of_pool = 2048;
        }
        si->global_meter_action_size = 8192;
        if (soc_property_get(unit, spn_MODULE_64PORTS, 1)) {
            si->modport_max = 63;
        } else {
            /* Adjust modid count based on dual-modid config parameter settings.
             * By default, dual-modid is enabled (32-port module mode) to allow
             * compatibility with other devices.
             */
            si->modid_count = 2;
        }
        si->num_ucs = 2;

        break;

#if defined(BCM_KATANA_SUPPORT)
    case BCM56440_DEVICE_ID:
    case BCM56441_DEVICE_ID:
    case BCM56442_DEVICE_ID:
    case BCM56443_DEVICE_ID:
    case BCM56445_DEVICE_ID:
    case BCM56446_DEVICE_ID:
    case BCM56447_DEVICE_ID:
    case BCM56448_DEVICE_ID:
    case BCM56449_DEVICE_ID:
    case BCM56240_DEVICE_ID:
    case BCM56241_DEVICE_ID:
    case BCM56242_DEVICE_ID:
    case BCM56243_DEVICE_ID:
    case BCM56245_DEVICE_ID:
    case BCM56246_DEVICE_ID:
    case BCM55440_DEVICE_ID:
    case BCM55441_DEVICE_ID:
        si->chip = SOC_INFO_CHIP_KATANA;
        si->port_class_max = 128;  /* egr_vlan_xlate port class max*/
        si->modid_max = 255;    /* See SOC_MODID_MAX(unit) */
        si->hg_offset = 26;   /* SOC_HG_OFFSET */
        si->trunk_bit_pos = 15;
        si->port_addr_max = 127;
        si->num_cpu_cosq = 48;
        si->internal_loopback = 1;
        si->num_time_interface = 1;
        si->num_flex_ingress_pools = 8;
        si->num_flex_egress_pools = 8;
        si->pcie_phy_addr = 0x9d;
        if ((dev_id >= BCM56240_DEVICE_ID) &&
            (dev_id <= BCM56246_DEVICE_ID)) {
           si->size_flex_ingress_pool = 512;
           si->size_flex_egress_pool = 256;
        } else {
           si->size_flex_ingress_pool = 2048;
           si->size_flex_egress_pool = 1024;
        }
        si->vlan_xlate_glp_wildcard = 0xffff; /* glp wildcard value */
        si->global_meter_pools = 8;
        si->global_meter_size_of_pool = 1024;
        si->global_meter_max_size_of_pool = 1024;
        if ((dev_id >= BCM56240_DEVICE_ID) &&
            (dev_id <= BCM56246_DEVICE_ID)) {
            si->global_meter_size_of_pool = 256;
        }
        si->global_meter_action_size = 8192;
        if (!(soc_property_get(unit, spn_MODULE_64PORTS, 1))) {
            /* Adjust modid count based on dual-modid config parameter settings.
             * By default, dual-modid is enabled (32-port module mode) to allow
             * compatibility with other devices.
             */
            si->modid_count = 2;
        }
        si->num_ucs = 2;
        /* default DDR3 Configuration */
        /* 16k Col * 1k Row * 8 Banks * 16 bits = 2Gb */
#if 0 /* Katana Only supports 1k Cols and 8 banks */
        soc->ddr3_num_columns = soc_property_get(unit,spn_EXT_RAM_COLUMNS, 1024);
        soc->ddr3_num_banks = soc_property_get(unit,spn_EXT_RAM_BANKS, 8);
#else
        soc->ddr3_num_columns = 1024;
        soc->ddr3_num_banks = 8;
#endif
        soc->ddr3_num_memories = soc_property_get(unit,spn_EXT_RAM_PRESENT, 3);
        soc->ddr3_num_rows = soc_property_get(unit,spn_EXT_RAM_ROWS, 8192);
        soc->ddr3_clock_mhz = soc_property_get(unit, spn_DDR3_CLOCK_MHZ, 800);
        soc->ddr3_mem_grade = soc_property_get(unit, spn_DDR3_MEM_GRADE, 0);

        break;
#endif /* BCM_KATANA_SUPPORT */

#if defined(BCM_KATANA2_SUPPORT)
    case BCM55450_DEVICE_ID:
    case BCM55455_DEVICE_ID:
    case BCM56248_DEVICE_ID:
    case BCM56450_DEVICE_ID:
    case BCM56452_DEVICE_ID:
    case BCM56454_DEVICE_ID:
    case BCM56455_DEVICE_ID:
    case BCM56456_DEVICE_ID:
    case BCM56457_DEVICE_ID:
    case BCM56458_DEVICE_ID:
        si->chip = SOC_INFO_CHIP_KATANA2;
        si->port_class_max = 128;  /* egr_vlan_xlate port class max*/
        si->modid_max = 255;    /* See SOC_MODID_MAX(unit) */
        si->modport_max = 127;
        si->hg_offset = 26;   /* SOC_HG_OFFSET */
        si->trunk_bit_pos = 15; /* 16th bit from right. See SOC_TRUNK_BIT_POS(unit)*/
        si->port_addr_max = 127;
        si->num_cpu_cosq = 48;
        si->internal_loopback = 1;
        si->num_time_interface = 1;
        si->num_flex_ingress_pools = 8;
        si->num_flex_egress_pools = 8;
        si->size_flex_ingress_pool = 2048;
        si->size_flex_egress_pool = 1024;
        si->vlan_xlate_glp_wildcard = 0xffff; /* glp wildcard value */
        si->global_meter_pools = 8;
        si->global_meter_size_of_pool = 1024;
        si->global_meter_max_size_of_pool = 1024;
        si->pcie_phy_addr = 0x5;
        if ((dev_id >= BCM56240_DEVICE_ID) &&
            (dev_id <= BCM56246_DEVICE_ID)) {
            si->global_meter_size_of_pool = 256;
        }

        si->global_meter_action_size = 8192;
#ifdef BCM_SBUSDMA_SUPPORT
        soc->max_sbusdma_channels = 3;
        soc_kt2_blk_counter_config(unit);
#endif
#ifdef PLISIM
        if (SOC_IS_KATANA2(unit) && (sal_boot_flags_get() & BOOT_F_PLISIM)) {
            /* case with pcid.sim or bcm5*_bcmsim.linux */
            if (CMVEC(unit).config_var_get == NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "Assigning dummy config var get \n")));
                CMVEC(unit).config_var_get = _dummy_config_var_get;
            }
        }
#endif
        si->olp_port[0] = 0;
        si->num_ucs = 2;
        si->max_mtu = 12288;
        break;
#endif /* BCM_KATANA2_SUPPORT */
#if defined(BCM_SABER2_SUPPORT)
    case BCM56260_DEVICE_ID:
    case BCM56261_DEVICE_ID:
    case BCM56262_DEVICE_ID:
    case BCM56263_DEVICE_ID:
    case BCM56265_DEVICE_ID:
    case BCM56266_DEVICE_ID:
    case BCM56267_DEVICE_ID:
    case BCM56268_DEVICE_ID:
    case BCM56460_DEVICE_ID:
    case BCM56461_DEVICE_ID:
    case BCM56462_DEVICE_ID:
    case BCM56463_DEVICE_ID:
    case BCM56465_DEVICE_ID:
    case BCM56466_DEVICE_ID:
    case BCM56467_DEVICE_ID:
    case BCM56468_DEVICE_ID:
    case BCM56233_DEVICE_ID:
        si->chip = SOC_INFO_CHIP_KATANA2;
        si->chip_type = SOC_INFO_CHIP_TYPE_SABER2; /* this is being added for saber2 specific stuff  */
        si->modid_max = 255;    /* See SOC_MODID_MAX(unit) */
        si->hg_offset = 26;   /* SOC_HG_OFFSET */
        si->trunk_bit_pos = 15;
        si->port_addr_max = 127;
        si->num_cpu_cosq = 48;
        si->internal_loopback = 1;
        si->num_time_interface = 1;
        si->num_flex_ingress_pools = 8;
        si->num_flex_egress_pools = 8;
        si->size_flex_ingress_pool = 2048;
        si->size_flex_egress_pool = 1024;
        si->vlan_xlate_glp_wildcard = 0xffff; /* glp wildcard value */
        if ((dev_id == BCM56260_DEVICE_ID) ||
                (dev_id == BCM56261_DEVICE_ID) ||
                (dev_id == BCM56262_DEVICE_ID) ||
                (dev_id == BCM56263_DEVICE_ID) ||
                (dev_id == BCM56265_DEVICE_ID) ||
                (dev_id == BCM56266_DEVICE_ID) ||
                (dev_id == BCM56267_DEVICE_ID) ||
                (dev_id == BCM56268_DEVICE_ID)
           ) {
            /* Number of meters reduced to half,
             * for SB2 (compared to SB2+).
             * for SB2+:8K, SB2:4K, Dagger_2:1K
             */
            si->global_meter_size_of_pool = 512;
            si->global_meter_action_size = 4096;
        } else if (dev_id == BCM56233_DEVICE_ID) {
            si->global_meter_size_of_pool = 128;
            si->global_meter_action_size = 1024;
        } else {
            si->global_meter_size_of_pool = 1024;
            si->global_meter_action_size = 8192;
        }
        si->global_meter_pools = 8;
        si->global_meter_max_size_of_pool = 1024;

        si->port_class_max = 128;  /* egr_vlan_xlate port class max*/
#ifdef BCM_SBUSDMA_SUPPORT
        soc->max_sbusdma_channels = 3;
/*        soc_sb2_blk_counter_config(unit); */
#endif
#ifdef PLISIM
        if (sal_boot_flags_get() & BOOT_F_PLISIM) {
            /* case with pcid.sim or bcm5*_bcmsim.linux */
            if (CMVEC(unit).config_var_get == NULL) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                           "Assigning dummy config var get \n")));
                CMVEC(unit).config_var_get = _dummy_config_var_get;
            }
        }
#endif
        si->olp_port[0] = 0;
        /* Dagger2 support 2xR5 Embedded processors
         * Since the mHost 0 is disabled
         * The num_ucs is kept 3 for maintaining exisiting for loops
         */
        si->num_ucs = 3;
        si->max_mtu = 0x3000;

        break;
#endif /* BCM_SABER2_SUPPORT */

#if defined(BCM_METROLITE_SUPPORT)
    case BCM56270_DEVICE_ID:
    case BCM56271_DEVICE_ID:
    case BCM56272_DEVICE_ID:

    case BCM53460_DEVICE_ID:
    case BCM53461_DEVICE_ID:

        si->chip = SOC_INFO_CHIP_KATANA2;
        si->chip_type = SOC_INFO_CHIP_TYPE_METROLITE; /* this is being added for metrolite specific stuff  */
        si->modid_max = 63;    /* See SOC_MODID_MAX(unit) */
        si->hg_offset = 26;   /* SOC_HG_OFFSET */
        si->trunk_bit_pos = 15;
        si->port_addr_max = 31;
        si->num_cpu_cosq = 48;
        si->internal_loopback = 1;
        si->num_time_interface = 1;
        si->num_flex_ingress_pools = 4;
        si->num_flex_egress_pools = 4;
        si->size_flex_ingress_pool = 512;
        si->size_flex_egress_pool = 512;
        si->vlan_xlate_glp_wildcard = 0xffff; /* glp wildcard value */
        si->global_meter_pools = 8;
        si->global_meter_size_of_pool = 512;
        si->global_meter_max_size_of_pool = 512;

        si->global_meter_action_size = 4096;
#ifdef BCM_SBUSDMA_SUPPORT
        soc->max_sbusdma_channels = 3;
        soc->tdma_ch = 0;
        soc->tslam_ch = 1;
        soc->desc_ch = 2;
#endif
#ifdef PLISIM
        if (sal_boot_flags_get() & BOOT_F_PLISIM) {
            /* case with pcid.sim or bcm5*_bcmsim.linux */
            if (CMVEC(unit).config_var_get == NULL) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                           "Assigning dummy config var get \n")));
                CMVEC(unit).config_var_get = _dummy_config_var_get;
            }
        }
#endif
        si->olp_port[0] = 0;
        si->num_ucs = 2;
        si->max_mtu = 0x3000;

        break;
#endif /* BCM_METROLITE_SUPPORT */

    case BCM56850_DEVICE_ID:
    case BCM56860_DEVICE_ID:
        switch (dev_id) {
        case BCM56868_DEVICE_ID:
            si->chip_type = SOC_INFO_CHIP_TYPE_TITAN2PLUS;
            break;
        case BCM56860_DEVICE_ID:
        case BCM56861_DEVICE_ID:
        case BCM56862_DEVICE_ID:
        case BCM56864_DEVICE_ID:
        case BCM56865_DEVICE_ID:
        case BCM56866_DEVICE_ID:
        case BCM56867_DEVICE_ID:
        case BCM56832_DEVICE_ID:
        case BCM56833_DEVICE_ID:
        case BCM56836_DEVICE_ID:
            si->chip = SOC_INFO_CHIP_TRIDENT2PLUS;
            break;
        default:
            if (dev_id == BCM56750_DEVICE_ID) {
                si->chip = SOC_INFO_CHIP_TITAN2;
            } else {
                si->chip = SOC_INFO_CHIP_TRIDENT2;
            }
        }

        si->num_pipe = 2;
        si->port_class_max = 128;  /* egr_vlan_xlate port class max*/
        si->modid_max = 255;    /* See SOC_MODID_MAX(unit) */
        /* si->hg_offset = 0;    SOC_HG_OFFSET */
        /*
         * 16th bit from right. See SOC_TRUNK_BIT_POS(unit)
         *     Note that it is not together with port/trunk-id.
         */
        si->trunk_bit_pos = 15;
        /* port_addr_max has to be 2^n -1 */
        si->port_addr_max = 127;
        if (SOC_IS_TD2P_TT2P(unit)){
            si->num_cpu_cosq = 48;
        } else {
            si->num_cpu_cosq = (rev_id >= BCM56850_A1_REV_ID) ? 44 : 48;
        }
        si->modid_count =
            soc_property_get(unit, spn_MODULE_NUM_MODIDS, 1) == 2 ? 2 : 1;
        si->modport_max = si->modid_count == 1 ? 127 : 63;
        si->num_time_interface = 1;
        si->pcie_phy_addr = 0x1da;
#ifdef BCM_SBUSDMA_SUPPORT
        soc->max_sbusdma_channels = 3;
#endif
        si->vlan_xlate_glp_wildcard = 0xffff; /* glp wildcard value */
        si->num_ucs = 2;
        si->max_mtu = 9416;
        /* Initialize the max subport ports and groups */
        si->max_subport_coe_ports = si->max_subport_coe_groups = 512;
        break;


#ifdef BCM_APACHE_SUPPORT
    case BCM56560_DEVICE_ID:
    case BCM56670_DEVICE_ID:
    case BCM56671_DEVICE_ID:
    case BCM56672_DEVICE_ID:
    case BCM56675_DEVICE_ID:
        si->chip_type = SOC_INFO_CHIP_TYPE_APACHE;
        if (dev_id == BCM56670_DEVICE_ID || dev_id == BCM56671_DEVICE_ID ||
           dev_id ==  BCM56672_DEVICE_ID || dev_id ==  BCM56675_DEVICE_ID) {
            si->chip_type = SOC_INFO_CHIP_TYPE_MONTEREY;
            si->num_time_interface = 2;
        } else {
            si->num_time_interface = 1;
        }
        si->num_pipe = 1;
        si->port_class_max = 128;  /* egr_vlan_xlate port class max*/
        si->modid_max = 255;    /* See SOC_MODID_MAX(unit) */
        /* si->hg_offset = 0;    SOC_HG_OFFSET */
        /*
         * 16th bit from right. See SOC_TRUNK_BIT_POS(unit)
         * Note that it is not together with port/trunk-id.
         */
        si->trunk_bit_pos = 15;
        /* port_addr_max has to be 2^n -1 */
        si->port_addr_max = 127;
        si->num_cpu_cosq = 48;
        si->modid_count =
            soc_property_get(unit, spn_MODULE_NUM_MODIDS, 1) == 2 ? 2 : 1;
        si->modport_max = si->modid_count == 1 ? 127 : 63;

#ifdef BCM_SBUSDMA_SUPPORT
        soc->max_sbusdma_channels = 3;
#endif
        si->vlan_xlate_glp_wildcard = 0xffff; /* glp wildcard value */
        si->num_ucs = 2;
        if (dev_id == BCM56670_DEVICE_ID || dev_id == BCM56671_DEVICE_ID ||
            dev_id == BCM56672_DEVICE_ID ||dev_id ==  BCM56675_DEVICE_ID)
            si->num_ucs = 3;
        si->global_meter_pools = 8;
        si->global_meter_size_of_pool = 2048;
        si->global_meter_max_size_of_pool = 2048;
        si->global_meter_action_size = 16384;

        /* Initialize the max subport ports and groups */
        si->max_subport_coe_ports = si->max_subport_coe_groups = 256;
        si->max_mtu = 12288; /* Apache and FireBolt5 */

        break;
#endif /* BCM_APACHE_SUPPORT */

    case BCM56960_DEVICE_ID:
    case BCM56961_DEVICE_ID:
    case BCM56962_DEVICE_ID:
    case BCM56963_DEVICE_ID:
    case BCM56930_DEVICE_ID:
    case BCM56968_DEVICE_ID:
    case BCM56168_DEVICE_ID:
    case BCM56169_DEVICE_ID:
#if defined(BCM_TOMAHAWKPLUS_SUPPORT)
    case BCM56965_DEVICE_ID:
    case BCM56969_DEVICE_ID:
    case BCM56966_DEVICE_ID:
    case BCM56967_DEVICE_ID:
        if ((BCM56965_DEVICE_ID == dev_id) || (BCM56969_DEVICE_ID == dev_id) ||
           (BCM56966_DEVICE_ID == dev_id) || (BCM56967_DEVICE_ID == dev_id)){
            si->chip_type = SOC_INFO_CHIP_TYPE_TOMAHAWKPLUS;
            si->port_class_max = 256;  /* egr_vlan_xlate port class max*/
        } else
#endif
        {
            if (BCM56968_DEVICE_ID == dev_id){
                si->chip_type = SOC_INFO_CHIP_TYPE_TITANHAWK;
            }
            si->port_class_max = 128;  /* egr_vlan_xlate port class max*/
        }
        si->chip = SOC_INFO_CHIP_TOMAHAWK;
        si->modid_max = 255;    /* See SOC_MODID_MAX(unit) */
        /* si->hg_offset = 0;    SOC_HG_OFFSET */
        /*
         * 16th bit from right. See SOC_TRUNK_BIT_POS(unit)
         *     Note that it is not together with port/trunk-id.
         */
        si->trunk_bit_pos = 16;
        /* port_addr_max has to be 2^n -1 */
        si->port_addr_max = 255;
        si->num_cpu_cosq = 48;
        si->modid_count =
            soc_property_get(unit, spn_MODULE_NUM_MODIDS, 1) == 2 ? 2 : 1;
        /*
         * For Dual modid support on Tomahawk (to address 136 ports),
         * the port split used is (64 + 128(use 72)) between two modids.
         * Two different modport max values are used for each modid.
         */
        si->modport_max = si->modid_count == 1 ? 255 : 127;
        si->modport_max_first = si->modid_count == 1 ? -1 : 63;
        si->sflow_range_max = 28;
        si->num_pipe = 4;
        si->num_xpe = 4;
        si->num_slice = 2;
        si->num_layer = 2;
        si->num_time_interface = 1;
        si->num_ucs = 2;
#ifdef BCM_SBUSDMA_SUPPORT
        soc->max_sbusdma_channels = 3;
#endif
        si->vlan_xlate_glp_wildcard = 0x1ffff; /* glp wildcard value */
        si->max_mtu = 9416;
#ifdef BCM_TOMAHAWK_SUPPORT
        if (((soc_property_get(unit, "th_random_sampling_enable", 0)) == 1) &&
            (soc_feature(unit, soc_feature_th_tflow))) {
            si->th_tflow_enabled = 1;
        } else {
            si->th_tflow_enabled = 0;
        }

        if (soc_feature(unit, soc_feature_th_fp_ctc_manipulate) &&
            (soc_property_get(unit, "th_copy_to_cpu_replace", 0)) == 1) {
            si->th_ctc_replace_enabled = 1;
        } else {
            si->th_ctc_replace_enabled = 0;
        }
#endif /* BCM_TOMAHAWK_SUPPORT */
        break;

    case BCM56970_DEVICE_ID:
    case BCM56971_DEVICE_ID:
    case BCM56972_DEVICE_ID:
    case BCM56974_DEVICE_ID:
    case BCM56975_DEVICE_ID:
        if (BCM56975_DEVICE_ID == dev_id){
            si->chip_type = SOC_INFO_CHIP_TYPE_TITANHAWK2;
        } else
        {
            si->chip_type= SOC_INFO_CHIP_TYPE_TOMAHAWK2;
        }

        si->port_class_max = 256;  /* egr_vlan_xlate port class max*/
        si->modid_max = 255;    /* See SOC_MODID_MAX(unit) */
        /* si->hg_offset = 0;    SOC_HG_OFFSET */
        /*
         * 16th bit from right. See SOC_TRUNK_BIT_POS(unit)
         *     Note that it is not together with port/trunk-id.
         */
        si->trunk_bit_pos = 16;
        /* port_addr_max has to be 2^n -1 */
        si->port_addr_max = 255;
        si->num_cpu_cosq = 48;
        si->modid_count =
            soc_property_get(unit, spn_MODULE_NUM_MODIDS, 1) == 2 ? 2 : 1;
        /*
         * For Dual modid support on Tomahawk (to address 136 ports),
         * the port split used is (64 + 128(use 72)) between two modids.
         * Two different modport max values are used for each modid.
         */
        si->modport_max = si->modid_count == 1 ? 255 : 127;
        si->modport_max_first = si->modid_count == 1 ? -1 : 63;
        si->sflow_range_max = 28;

        si->num_pipe = 4;
        si->num_xpe = 4;
        si->num_slice = 2;
        si->num_sed = 2;
        si->num_layer = 2;
        si->num_time_interface = 1;
        si->num_ucs = 2;
#ifdef BCM_SBUSDMA_SUPPORT
        soc->max_sbusdma_channels = 3;
#ifdef BCM_TOMAHAWK2_SUPPORT
        soc_tomahawk2_mmu_pstats_tbl_config(unit);
#endif
#endif /* BCM_SBUSDMA_SUPPORT */
        si->vlan_xlate_glp_wildcard = 0x1ffff; /* glp wildcard value */
        si->max_mtu = 9416;
        break;

    case BCM56980_DEVICE_ID:
    case BCM56981_DEVICE_ID:
    case BCM56982_DEVICE_ID:
    case BCM56983_DEVICE_ID:
    case BCM56984_DEVICE_ID:
        si->chip_type = SOC_INFO_CHIP_TYPE_TOMAHAWK3;
        si->port_class_max = 144;  /* egr_vlan_xlate port class max*/
        si->modid_max = 0;    /* See SOC_MODID_MAX(unit) */
        /* si->hg_offset = 0;    SOC_HG_OFFSET */
        /*
         * 8th bit from right. MSB denotes if the next 8 bits are trunk number
         * or port number
         */
        si->trunk_bit_pos = 8;
        /* port_addr_max has to be 2^n -1 */
        si->port_addr_max = 255;
        si->num_cpu_cosq = 48;
        si->modid_count =
            soc_property_get(unit, spn_MODULE_NUM_MODIDS, 1) == 2 ? 2 : 1;
        /*
         * For Dual modid support on Tomahawk (to address 136 ports),
         * the port split used is (64 + 128(use 72)) between two modids.
         * Two different modport max values are used for each modid.
         */
        si->modport_max = si->modid_count == 1 ? 255 : 127;
        si->modport_max_first = si->modid_count == 1 ? -1 : 63;
        si->sflow_range_max = 28;
        si->num_pipe = 8;
        si->num_itm = 2;
        si->num_eb = 8;
        si->num_time_interface = 1;
        si->num_ucs = 2;
#ifdef BCM_SBUSDMA_SUPPORT
        soc->max_sbusdma_channels = 3;
#endif
        /*si->vlan_xlate_glp_wildcard = 0x1ffff; *//* glp wildcard value */
        si->max_mtu = 9416;
#ifdef BCM_TOMAHAWK_SUPPORT
        if (((soc_property_get(unit, "th_random_sampling_enable", 0)) == 1) &&
            (soc_feature(unit, soc_feature_th_tflow))) {
            si->th_tflow_enabled = 1;
        } else {
            si->th_tflow_enabled = 0;
        }

        if (soc_feature(unit, soc_feature_th_fp_ctc_manipulate) &&
            (soc_property_get(unit, "th_copy_to_cpu_replace", 0)) == 1) {
            si->th_ctc_replace_enabled = 1;
        } else {
            si->th_ctc_replace_enabled = 0;
        }
#endif /* BCM_TOMAHAWK_SUPPORT */
        break;

    case BCM56870_DEVICE_ID:
    case BCM56873_DEVICE_ID:
        si->chip_type = SOC_INFO_CHIP_TYPE_TRIDENT3;
        si->port_class_max = 256;  /* egr_vlan_xlate port class max*/
        si->modid_max = 255;    /* See SOC_MODID_MAX(unit) */
        /* si->hg_offset = 0;    SOC_HG_OFFSET */
        /*
         * 17th bit from right. See SOC_TRUNK_BIT_POS(unit)
         *     Note that it is not together with port/trunk-id.
         */
        si->trunk_bit_pos = 16;
        /* port_addr_max has to be 2^n -1 */
        si->port_addr_max = 255;
        si->num_cpu_cosq = 48;
        si->modid_count =
            soc_property_get(unit, spn_MODULE_NUM_MODIDS, 1) == 2 ? 2 : 1;
        /*
         * For Dual modid support on Trident3 (to address 136 ports),
         * the port split used is (64 + 128(use 72)) between two modids.
         * Two different modport max values are used for each modid.
         */
        si->modport_max = si->modid_count == 1 ? 255 : 127;
        si->modport_max_first = si->modid_count == 1 ? -1 : 63;

        si->num_pipe = 2;
        si->num_xpe = 1;
        si->num_slice = 1;
        si->num_layer = 1;
        si->num_time_interface = 1;
        si->num_ucs = 2;
#ifdef BCM_SBUSDMA_SUPPORT
        soc->max_sbusdma_channels = 3;
        soc->tdma_ch = 0;
        soc->tslam_ch = 1;
        soc->desc_ch = 2;
#endif
        si->vlan_xlate_glp_wildcard = 0x1ffff; /* glp wildcard value */
            /* Initialize the max subport ports and groups */
        si->max_subport_coe_ports = si->max_subport_coe_groups = 512;
        si->max_mtu = 9416;
        break;

#ifdef BCM_HURRICANE4_SUPPORT
    case BCM56275_DEVICE_ID:
        si->chip_type = SOC_INFO_CHIP_TYPE_HURRICANE4;
        si->port_class_max = 128;  /* egr_vlan_xlate port class max*/
        si->modid_max = 255;    /* See SOC_MODID_MAX(unit) */
        /* si->hg_offset = 0;    SOC_HG_OFFSET */
        /*
         * 16th bit from right. See SOC_TRUNK_BIT_POS(unit)
         *     Note that it is not together with port/trunk-id.
         */
        si->trunk_bit_pos = 16;
        /* port_addr_max has to be 2^n -1 */
        si->port_addr_max = 255;
        si->nof_pmqs = 2;
        si->num_cpu_cosq = 48;
        si->modid_count =
            soc_property_get(unit, spn_MODULE_NUM_MODIDS, 1) == 2 ? 2 : 1;
        /*
         * For Dual modid support on Helix-5 (to address 76 ports),
         * the port split used is (64 + 78(use 76)) between two modids.
         * Two different modport max values are used for each modid.
         */
        si->modport_max = si->modid_count == 1 ? 255 : 127;
        si->modport_max_first = si->modid_count == 1 ? -1 : 63;

        si->num_pipe = 1;
        si->num_xpe = 1;
        si->num_slice = 1;
        si->num_layer = 1;
        si->num_time_interface = 1;
#ifdef BCM_SBUSDMA_SUPPORT
        soc->max_sbusdma_channels = 3;
        soc->tdma_ch = 0;
        soc->tslam_ch = 1;
        soc->desc_ch = 2;
#endif
        si->vlan_xlate_glp_wildcard = 0x1ffff; /* glp wildcard value */
        break;
#endif /* BCM_HURRICANE4_SUPPORT */

#ifdef BCM_FIREBOLT6_SUPPORT
    case BCM56470_DEVICE_ID:
        si->chip_type = SOC_INFO_CHIP_TYPE_FIREBOLT6;
        si->port_class_max = 128;  /* egr_vlan_xlate port class max*/
        si->modid_max = 255;    /* See SOC_MODID_MAX(unit) */
        /* si->hg_offset = 0;    SOC_HG_OFFSET */
        /*
         * 16th bit from right. See SOC_TRUNK_BIT_POS(unit)
         *     Note that it is not together with port/trunk-id.
         */
        si->trunk_bit_pos = 16;
        /* port_addr_max has to be 2^n -1 */
        si->port_addr_max = 255;
        si->num_cpu_cosq = 48;
        si->modid_count =
            soc_property_get(unit, spn_MODULE_NUM_MODIDS, 1) == 2 ? 2 : 1;
        /*
         * For Dual modid support on Helix-5 (to address 76 ports),
         * the port split used is (64 + 78(use 76)) between two modids.
         * Two different modport max values are used for each modid.
         */
        si->modport_max = si->modid_count == 1 ? 255 : 127;
        si->modport_max_first = si->modid_count == 1 ? -1 : 63;

        si->num_pipe = 1;
        si->num_xpe = 1;
        si->num_slice = 1;
        si->num_layer = 1;
        si->num_time_interface = 1;
#ifdef BCM_SBUSDMA_SUPPORT
        soc->max_sbusdma_channels = 3;
        soc->tdma_ch = 0;
        soc->tslam_ch = 1;
        soc->desc_ch = 2;
#endif
        si->vlan_xlate_glp_wildcard = 0x1ffff; /* glp wildcard value */

        /* Initialize the max subport ports and groups */
        si->max_subport_coe_ports = 512;
        si->max_subport_coe_groups = 256;

        break;
#endif /* BCM_FIREBOLT6_SUPPORT */

    case BCM56370_DEVICE_ID:
        si->chip_type = SOC_INFO_CHIP_TYPE_HELIX5;
        si->port_class_max = 128;  /* egr_vlan_xlate port class max*/
        si->modid_max = 255;    /* See SOC_MODID_MAX(unit) */
        /* si->hg_offset = 0;    SOC_HG_OFFSET */
        /*
         * 16th bit from right. See SOC_TRUNK_BIT_POS(unit)
         *     Note that it is not together with port/trunk-id.
         */
        si->trunk_bit_pos = 16;
        /* port_addr_max has to be 2^n -1 */
        si->port_addr_max = 255;
        si->nof_pmqs = 3;
        si->num_cpu_cosq = 48;
        si->modid_count =
            soc_property_get(unit, spn_MODULE_NUM_MODIDS, 1) == 2 ? 2 : 1;
        /*
         * For Dual modid support on Helix-5 (to address 76 ports),
         * the port split used is (64 + 78(use 76)) between two modids.
         * Two different modport max values are used for each modid.
         */
        si->modport_max = si->modid_count == 1 ? 255 : 127;
        si->modport_max_first = si->modid_count == 1 ? -1 : 63;

        si->num_pipe = 1;
        si->num_xpe = 1;
        si->num_slice = 1;
        si->num_layer = 1;
        si->num_time_interface = 1;
#ifdef BCM_SBUSDMA_SUPPORT
        soc->max_sbusdma_channels = 3;
        soc->tdma_ch = 0;
        soc->tslam_ch = 1;
        soc->desc_ch = 2;
#endif
        si->vlan_xlate_glp_wildcard = 0x1ffff; /* glp wildcard value */
        break;
    case BCM56770_DEVICE_ID:
    case BCM56771_DEVICE_ID:
        si->chip_type = SOC_INFO_CHIP_TYPE_MAVERICK2;
        si->port_class_max = 128;  /* egr_vlan_xlate port class max*/
        si->modid_max = 255;    /* See SOC_MODID_MAX(unit) */
        /* si->hg_offset = 0;    SOC_HG_OFFSET */
        /*
         * 16th bit from right. See SOC_TRUNK_BIT_POS(unit)
         *     Note that it is not together with port/trunk-id.
         */
        si->trunk_bit_pos = 16;
        /* port_addr_max has to be 2^n -1 */
        si->port_addr_max = 255;
        si->num_cpu_cosq = 48;
        si->modid_count =
            soc_property_get(unit, spn_MODULE_NUM_MODIDS, 1) == 2 ? 2 : 1;
        /*
         * For Dual modid support on Trident3 (to address 136 ports),
         * the port split used is (64 + 128(use 72)) between two modids.
         * Two different modport max values are used for each modid.
         */
        si->modport_max = si->modid_count == 1 ? 255 : 127;
        si->modport_max_first = si->modid_count == 1 ? -1 : 63;

        si->num_pipe = 1;
        si->num_xpe = 1;
        si->num_slice = 1;
        si->num_layer = 1;
        si->num_time_interface = 1;
        si->num_ucs = 2;
#ifdef BCM_SBUSDMA_SUPPORT
        soc->max_sbusdma_channels = 3;
        soc->tdma_ch = 0;
        soc->tslam_ch = 1;
        soc->desc_ch = 2;
#endif
        si->vlan_xlate_glp_wildcard = 0x1ffff; /* glp wildcard value */
            /* Initialize the max subport ports and groups */
        si->max_subport_coe_ports = si->max_subport_coe_groups = 512;
        si->max_mtu = 9416;
        break;

    default:
        si->chip = 0;
        si->modid_count = 0;
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "soc_info_config: driver device %04x unexpected\n"),
                  drv_dev_id));
        break;
    }

    if (si->internal_loopback) {
        SOC_FEATURE_SET(unit, soc_feature_wlan);
        SOC_FEATURE_SET(unit, soc_feature_egr_mirror_true);
        SOC_FEATURE_SET(unit, soc_feature_mim);
        SOC_FEATURE_SET(unit, soc_feature_internal_loopback);
        SOC_FEATURE_SET(unit, soc_feature_mmu_virtual_ports);
    } else {
        SOC_PBMP_CLEAR(si->lb_pbm);
    }

    /* Enable Egress True Mirroring on devices with EP Redire V2*/
    if (soc_feature(unit, soc_feature_ep_redirect_v2)) {
        SOC_FEATURE_SET(unit, soc_feature_egr_mirror_true);
    }

    /*
     * pbmp_valid is a bitmap of all ports that exist on the unit.  Any
     * port not in this bitmap is thoroughly disregarded by the driver.
     * This is useful when some switch ports are unused (e.g. the IPIC
     * for 5691).
     */
    pbmp_valid = soc_property_get_pbmp(unit, spn_PBMP_VALID, 1);
    if (soc_property_get_str(unit, spn_PBMP_GPORT_STACK) != NULL) {
        pbmp_gport_stack = soc_property_get_pbmp(unit, spn_PBMP_GPORT_STACK, 0);
    }
#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_KATANA_SUPPORT)
    if(SOC_IS_ENDURO(unit) || SOC_IS_KATANAX(unit)) {
        if (soc_property_get_str(unit, spn_PBMP_LOOPBACK) != NULL) {
            si->lb_pbm = soc_property_get_pbmp(unit, spn_PBMP_LOOPBACK, 0);
        }
    }
#endif
#if defined(BCM_KATANA_SUPPORT)
    if (SOC_IS_KATANAX(unit) && !SOC_IS_METROLITE(unit)) {
        if (soc_property_get_str(unit, spn_PBMP_EXT_MEM) != NULL) {
            si->pbm_ext_mem = soc_property_get_pbmp(unit, spn_PBMP_EXT_MEM, 0);
        }
    }
#endif

    /* Defaults for the xport settings */
    SOC_PBMP_WORD_SET(pbmp_xport_ge, 0, 0);
    SOC_PBMP_WORD_SET(pbmp_xport_xe, 0, 0);
    switch (dev_id) {
    case BCM56580_DEVICE_ID:
        SOC_PBMP_WORD_SET(pbmp_xport_ge, 0, 0x0000ffff);
        break;
    case BCM56800_DEVICE_ID:
        SOC_PBMP_WORD_SET(pbmp_xport_xe, 0, 0x000fffff);
        break;
    case BCM56801_DEVICE_ID:
        SOC_PBMP_WORD_SET(pbmp_xport_xe, 0, 0x000003ff);
        break;
    case BCM56820_DEVICE_ID:
        SOC_PBMP_WORD_SET(pbmp_xport_xe, 0, 0x01fffffe);
        break;
    case BCM56821_DEVICE_ID:
        if (!soc_property_get(unit, spn_BCM56821_20X12, 0)) {
            SOC_PBMP_WORD_SET(pbmp_xport_xe, 0, 0x001fffe0);
        }
        break;
    case BCM56822_DEVICE_ID:
        SOC_PBMP_WORD_SET(pbmp_xport_xe, 0, 0x01ffe000);
        break;
    case BCM56825_DEVICE_ID:
        SOC_PBMP_WORD_SET(pbmp_xport_xe, 0, 0x01fe01fe);
        break;
    case BCM56629_DEVICE_ID:
        if (soc_feature(unit, soc_feature_xgport_one_xe_six_ge)) {
            if (!soc_property_get(unit, spn_BCM56629_40GE, 0)) {
                SOC_PBMP_WORD_SET(pbmp_xport_xe, 0, 0x0c004004);
            }
        }
        break;
#ifdef BCM_SHADOW_SUPPORT
    case BCM88732_DEVICE_ID:
        SOC_PBMP_WORD_SET(pbmp_xport_xe, 0, 0x1fe);
        break;
#endif /* BCM_SHADOW_SUPPORT */
    default:
        break;
    }

    pbmp_xport_xe = soc_property_get_pbmp_default(unit, spn_PBMP_XPORT_XE,
                                 pbmp_xport_xe);
    pbmp_xport_ge = soc_property_get_pbmp_default(unit, spn_PBMP_XPORT_GE,
                                 pbmp_xport_ge);
    pbmp_xport_cpri = soc_property_get_pbmp_default(unit, spn_PBMP_XPORT_CPRI,
                                 pbmp_xport_cpri);
    pbmp_roe_compression = soc_property_get_pbmp_default(unit, spn_PBMP_ROE_COMPRESSION,
                                 pbmp_roe_compression);

    switch (dev_id) {
#if defined(BCM_HERCULES_SUPPORT)
    case BCM5676_DEVICE_ID:
        /* 5676 (Herc4) has only ports 1, 3, 6, and 8 (only 0x14b) */
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 2);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 4);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 5);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 7);
        break;
#endif /* BCM_HERCULES_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
    case BCM53301_DEVICE_ID:
        for (port = 16; port <= 27; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        break;
    case BCM53300_DEVICE_ID:
    case BCM56500_DEVICE_ID:
    case BCM56300_DEVICE_ID:
    case BCM56505_DEVICE_ID:
    case BCM56305_DEVICE_ID:
    case BCM56310_DEVICE_ID:
    case BCM56315_DEVICE_ID:
    case BCM56510_DEVICE_ID:
        /*
         * Remove all HG/XE ports
         */
        for (port = 24; port <= 27; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        break;
    case BCM56301_DEVICE_ID:    /* Helix Only */
    case BCM56306_DEVICE_ID:    /* Helix Only */
    case BCM56311_DEVICE_ID:    /* Helix 1.5 Only */
    case BCM56316_DEVICE_ID:    /* Helix 1.5 Only */
    case BCM56501_DEVICE_ID:
    case BCM56506_DEVICE_ID:
    case BCM56511_DEVICE_ID:
    case BCM56516_DEVICE_ID:
        /*
         * Remove all GE ports
         */
        for (port = 0; port <= 23; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        break;
    case BCM56502_DEVICE_ID:
    case BCM56302_DEVICE_ID:
    case BCM56507_DEVICE_ID:
    case BCM56307_DEVICE_ID:
    case BCM56100_DEVICE_ID:
    case BCM56105_DEVICE_ID:
    case BCM56110_DEVICE_ID:
    case BCM56115_DEVICE_ID:
    case BCM56312_DEVICE_ID:
    case BCM56317_DEVICE_ID:
    case BCM56512_DEVICE_ID:
    case BCM56517_DEVICE_ID:
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 26);
        /* passthru */
        /* coverity[fallthrough: FALSE] */
    case BCM56503_DEVICE_ID:
    case BCM56303_DEVICE_ID:
    case BCM56508_DEVICE_ID:
    case BCM56308_DEVICE_ID:
    case BCM56101_DEVICE_ID:
    case BCM56106_DEVICE_ID:
    case BCM56111_DEVICE_ID:
    case BCM56116_DEVICE_ID:
    case BCM56313_DEVICE_ID:
    case BCM56318_DEVICE_ID:
    case BCM56513_DEVICE_ID:
    case BCM56518_DEVICE_ID:
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 27);
        break;
    case BCM53302_DEVICE_ID:
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 24);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 25);
        break;
#endif /* BCM_FIREBOLT_SUPPORT */
#if defined(BCM_RAPTOR_SUPPORT)
    case BCM56218R_DEVICE_ID:
    case BCM56219R_DEVICE_ID:
        SOC_PBMP_WORD_SET(pbmp_valid,
                          0,
                          SOC_PBMP_WORD_GET(pbmp_valid, 0) & 0xffffffc9);
        break;

    case BCM56214R_DEVICE_ID:
    case BCM56215R_DEVICE_ID:
        SOC_PBMP_WORD_SET(pbmp_valid,
                          0,
                          SOC_PBMP_WORD_GET(pbmp_valid, 0) & 0xffffffc9);
        /* passthru */
        /* coverity[fallthrough: FALSE] */
    case BCM56214_DEVICE_ID:
    case BCM56215_DEVICE_ID:
    SOC_PBMP_WORD_SET(pbmp_valid, 1, 0x0);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 30);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 31);
        break;

    case BCM56216_DEVICE_ID:
    case BCM56217_DEVICE_ID:
    SOC_PBMP_WORD_SET(pbmp_valid, 1, 0x0);
        SOC_PBMP_WORD_SET(pbmp_valid,
                          0,
                          SOC_PBMP_WORD_GET(pbmp_valid, 0) & 0x3fffff);
        break;

    case BCM56212_DEVICE_ID:
    case BCM56213_DEVICE_ID:
    SOC_PBMP_WORD_SET(pbmp_valid, 1, 0x0);
        SOC_PBMP_WORD_SET(pbmp_valid,
                          0,
                          SOC_PBMP_WORD_GET(pbmp_valid, 0) & 0x3fff);
        break;

    case BCM53716_DEVICE_ID:
    SOC_PBMP_WORD_SET(pbmp_valid, 1, 0x0);
        SOC_PBMP_WORD_SET(pbmp_valid,
                          0,
                          SOC_PBMP_WORD_GET(pbmp_valid, 0) & 0x3fffc9);
        break;

    case BCM53714_DEVICE_ID:
    SOC_PBMP_WORD_SET(pbmp_valid, 1, 0x0);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 30);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 31);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 1);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 2);
    break;

    case BCM53724_DEVICE_ID:
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 1);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 2);
        SOC_PBMP_CLEAR(pbmp_gport_stack);
        break;
    case BCM53726_DEVICE_ID:
        SOC_PBMP_WORD_SET(pbmp_valid,
                          0,
                          SOC_PBMP_WORD_GET(pbmp_valid, 0) & 0x3ffff9);
        SOC_PBMP_CLEAR(pbmp_gport_stack);
    break;

    case BCM53312_DEVICE_ID:
    case BCM53322_DEVICE_ID:
        SOC_PBMP_WORD_SET(pbmp_valid,
                          0,
                          SOC_PBMP_WORD_GET(pbmp_valid, 0) & 0x2223ff);
        break;

    case BCM53313_DEVICE_ID:
    case BCM53323_DEVICE_ID:
        SOC_PBMP_WORD_SET(pbmp_valid,
                          0,
                          SOC_PBMP_WORD_GET(pbmp_valid, 0) & 0x23ffff);
        break;

    case BCM53314_DEVICE_ID:
    case BCM53324_DEVICE_ID:
        break;

    case BCM53718_DEVICE_ID:
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 1);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 2);
        break;

    case BCM56014_DEVICE_ID:
        SOC_PBMP_WORD_SET(pbmp_valid, 1, 0x0);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 30);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 31);
        /* passthru */
        /* coverity[fallthrough: FALSE] */
    case BCM56018_DEVICE_ID:
        SOC_PBMP_WORD_SET(pbmp_gport_fe, 0, 0xffffffc0);
        SOC_PBMP_WORD_SET(pbmp_gport_fe, 1, 0x003fffff);
        break;

    case BCM56024_DEVICE_ID:
    case BCM56025_DEVICE_ID:
        SOC_PBMP_WORD_SET(pbmp_gport_fe, 0, 0xffffffc0);
        break;

    case BCM56226_DEVICE_ID:
    case BCM56227_DEVICE_ID:
        SOC_PBMP_WORD_SET(pbmp_valid,
                          0,
                          SOC_PBMP_WORD_GET(pbmp_valid, 0) & 0x3fffff);
        break;

    case BCM56228_DEVICE_ID:
    case BCM56229_DEVICE_ID:
        SOC_PBMP_WORD_SET(pbmp_valid,
                          0,
                          SOC_PBMP_WORD_GET(pbmp_valid, 0) & 0x3fff);
        break;

#endif /* BCM_RAPTOR_SUPPORT */
    case BCM56580_DEVICE_ID:
        SOC_PBMP_CLEAR(pbmp_tmp);
        SOC_PBMP_WORD_SET(pbmp_tmp, 0, 0x0000ffff);
        SOC_PBMP_OR(pbmp_xport_ge, pbmp_tmp);
        break;
    case BCM56701_DEVICE_ID:
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 14);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 15);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 16);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 17);
        /* Fall through */
    case BCM56700_DEVICE_ID:
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 8);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 9);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 18);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 19);
        SOC_PBMP_CLEAR(pbmp_xport_xe);
        SOC_PBMP_ASSIGN(pbmp_gx13g, pbmp_valid);
        break;
    case BCM56803_DEVICE_ID:
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 14);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 15);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 16);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 17);
        /* Fall through */
    case BCM56802_DEVICE_ID:
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 8);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 9);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 18);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 19);
        SOC_PBMP_ASSIGN(pbmp_gx13g, pbmp_valid);
        break;
    case BCM56801_DEVICE_ID:
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 18);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 19);
        SOC_PBMP_ASSIGN(pbmp_gx13g, pbmp_valid);
        SOC_PBMP_CLEAR(pbmp_tmp);
        SOC_PBMP_WORD_SET(pbmp_tmp, 0, 0x000ffc00);
        SOC_PBMP_AND(pbmp_gx13g, pbmp_tmp);
        break;
    case BCM56630_DEVICE_ID:
        for (port = 1; port <= 25; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(pbmp_tmp);
        if (soc_property_get(unit, spn_BCM56630_2X12_2X24, 0)) {
            /* 28x1GE + 2x12HG + 2x24HG + loopback */
            SOC_PBMP_PORT_ADD(pbmp_tmp, 26);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 27);
            SOC_PBMP_ASSIGN(pbmp_gx25g, pbmp_tmp);
            SOC_PBMP_CLEAR(pbmp_tmp);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 28);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 29);
            SOC_PBMP_ASSIGN(pbmp_gx13g, pbmp_tmp);
        } else {
            /* 24x1GE + 4x20HG + loopback */
            SOC_PBMP_PORT_ADD(pbmp_tmp, 26);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 27);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 28);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 29);
            if (soc_property_get(unit, spn_HIGIG_MAX_SPEED, 0) == 12000) {
                SOC_PBMP_ASSIGN(pbmp_gx12g, pbmp_tmp);
            } else {
                SOC_PBMP_ASSIGN(pbmp_gx20g, pbmp_tmp);
            }
        }
        for (port = 30; port <= 53; port++) {
            SOC_PBMP_PORT_ADD(pbmp_tmp, port);
        }
        SOC_PBMP_ASSIGN(pbmp_xq, pbmp_tmp);
        SOC_PBMP_AND(pbmp_xq, pbmp_valid);
        SOC_PBMP_ASSIGN(pbmp_hyplite, pbmp_xq);
        break;
    case BCM56634_DEVICE_ID:
    /* Fall through */
    case BCM56538_DEVICE_ID:
        SOC_PBMP_CLEAR(pbmp_tmp);
        if (soc_property_get(unit, spn_BCM56634_48G_4X12, 0) ||
            soc_property_get(unit, spn_BCM56538_48G_4X12, 0)) {
            /* 48x1GE + 4x12HG + 1x12G(loopback) */
            SOC_PBMP_PORT_ADD(pbmp_tmp, 26);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 27);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 28);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 29);
            SOC_PBMP_ASSIGN(pbmp_gx13g, pbmp_tmp);
        } else if (soc_property_get(unit, spn_BCM56634_48G_2X24, 0) ||
                   soc_property_get(unit, spn_BCM56538_48G_2X24, 0)) {
            /* 48x1GE + 2x24HG + 1x12G(loopback) */
            SOC_PBMP_PORT_ADD(pbmp_tmp, 26);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 27);
            SOC_PBMP_ASSIGN(pbmp_gx25g, pbmp_tmp);
            SOC_PBMP_PORT_REMOVE(pbmp_valid, 28);
            SOC_PBMP_PORT_REMOVE(pbmp_valid, 29);
        } else {
            /* 48x1GE + 4x16HG */
            SOC_PBMP_PORT_ADD(pbmp_tmp, 26);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 27);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 28);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 29);
            SOC_PBMP_ASSIGN(pbmp_gx16g, pbmp_tmp);
        }
        SOC_PBMP_CLEAR(pbmp_tmp);
        for (port = 30; port <= 53; port++) {
            SOC_PBMP_PORT_ADD(pbmp_tmp, port);
        }
        SOC_PBMP_ASSIGN(pbmp_xq, pbmp_tmp);
        SOC_PBMP_AND(pbmp_xq, pbmp_valid);
        SOC_PBMP_ASSIGN(pbmp_hyplite, pbmp_xq);
        break;
    case BCM56636_DEVICE_ID:
        for (port = 14; port <= 25; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 43; port <= 49; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 51; port <= 53; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(pbmp_tmp);
        SOC_PBMP_PORT_ADD(pbmp_tmp, 42);
        SOC_PBMP_PORT_ADD(pbmp_tmp, 50);
        if (soc_property_get(unit, spn_BCM56636_2X12_2X24, 0)) {
            SOC_PBMP_ASSIGN(pbmp_xq13g, pbmp_tmp);
            SOC_PBMP_PORT_REMOVE(pbmp_valid, 28);
            SOC_PBMP_PORT_REMOVE(pbmp_valid, 29);
            SOC_PBMP_CLEAR(pbmp_tmp);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 26);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 27);
            SOC_PBMP_ASSIGN(pbmp_gx25g, pbmp_tmp);
        } else if (soc_property_get(unit, spn_BCM56636_24G_6X12, 0)) {
            SOC_PBMP_ASSIGN(pbmp_xq13g, pbmp_tmp);
            SOC_PBMP_CLEAR(pbmp_tmp);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 26);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 27);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 28);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 29);
            SOC_PBMP_ASSIGN(pbmp_gx13g, pbmp_tmp);
        } else {
            SOC_PBMP_ASSIGN(pbmp_xq12g, pbmp_tmp);
            SOC_PBMP_CLEAR(pbmp_tmp);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 26);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 27);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 28);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 29);
            SOC_PBMP_ASSIGN(pbmp_gx16g, pbmp_tmp);
        }
        SOC_PBMP_CLEAR(pbmp_tmp);
        for (port = 30; port <= 53; port++) {
            SOC_PBMP_PORT_ADD(pbmp_tmp, port);
        }
        SOC_PBMP_ASSIGN(pbmp_xq, pbmp_tmp);
        SOC_PBMP_AND(pbmp_xq, pbmp_valid);
        SOC_PBMP_ASSIGN(pbmp_hyplite, pbmp_xq);

    /* Flex-port support */

    /* For each possible controlling port, ... */
    for (port = 30; port <= 50; port += 4) {
        if (!SOC_PBMP_MEMBER(pbmp_valid, port))  continue;

        /* Controlling port is valid port
           => Add corresponding subsidiary ports as valid
        */

        SOC_PBMP_PORT_ADD(pbmp_valid, port + 1);
        SOC_PBMP_PORT_ADD(pbmp_valid, port + 2);
        SOC_PBMP_PORT_ADD(pbmp_valid, port + 3);

        if (SOC_PBMP_MEMBER(pbmp_xq10g, port)
        || SOC_PBMP_MEMBER(pbmp_xq12g, port)
        || SOC_PBMP_MEMBER(pbmp_xq13g, port)
        ) {
            /* Controlling port not in 1G mode
           => Mark corresponding subsidiary ports as disabled
        */

            SOC_PBMP_PORT_ADD(pbmp_disabled, port + 1);
        SOC_PBMP_PORT_ADD(pbmp_disabled, port + 2);
        SOC_PBMP_PORT_ADD(pbmp_disabled, port + 3);
        }
    }

        break;
    case BCM56638_DEVICE_ID:
        for (port = 2; port <= 25; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 31; port <= 37; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 39; port <= 41; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 43; port <= 49; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 51; port <= 53; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(pbmp_tmp);
        SOC_PBMP_PORT_ADD(pbmp_tmp, 30);
        SOC_PBMP_PORT_ADD(pbmp_tmp, 38);
        SOC_PBMP_PORT_ADD(pbmp_tmp, 42);
        SOC_PBMP_PORT_ADD(pbmp_tmp, 50);
        if (soc_property_get(unit, spn_BCM56638_4X12_2X24, 0)) {
            SOC_PBMP_ASSIGN(pbmp_xq13g, pbmp_tmp);
            SOC_PBMP_PORT_REMOVE(pbmp_valid, 28);
            SOC_PBMP_PORT_REMOVE(pbmp_valid, 29);
            SOC_PBMP_CLEAR(pbmp_tmp);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 26);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 27);
            SOC_PBMP_ASSIGN(pbmp_gx25g, pbmp_tmp);
        } else if (soc_property_get(unit, spn_BCM56638_8X12, 0)) {
            SOC_PBMP_ASSIGN(pbmp_xq13g, pbmp_tmp);
            SOC_PBMP_CLEAR(pbmp_tmp);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 26);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 27);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 28);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 29);
            SOC_PBMP_ASSIGN(pbmp_gx13g, pbmp_tmp);
        } else {
            SOC_PBMP_ASSIGN(pbmp_xq12g, pbmp_tmp);
            SOC_PBMP_CLEAR(pbmp_tmp);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 26);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 27);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 28);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 29);
            SOC_PBMP_ASSIGN(pbmp_gx16g, pbmp_tmp);
        }
        SOC_PBMP_CLEAR(pbmp_tmp);
        for (port = 30; port <= 53; port++) {
            SOC_PBMP_PORT_ADD(pbmp_tmp, port);
        }
        SOC_PBMP_ASSIGN(pbmp_xq, pbmp_tmp);
        SOC_PBMP_AND(pbmp_xq, pbmp_valid);
        SOC_PBMP_ASSIGN(pbmp_hyplite, pbmp_xq);

    /* Flex-port support */

    /* For each possible controlling port, ... */
    for (port = 30; port <= 50; port += 4) {
        if (!SOC_PBMP_MEMBER(pbmp_valid, port))  continue;

        /* Controlling port is valid port
           => Add corresponding subsidiary ports as valid
        */

        SOC_PBMP_PORT_ADD(pbmp_valid, port + 1);
        SOC_PBMP_PORT_ADD(pbmp_valid, port + 2);
        SOC_PBMP_PORT_ADD(pbmp_valid, port + 3);

        if (SOC_PBMP_MEMBER(pbmp_xq10g, port)
        || SOC_PBMP_MEMBER(pbmp_xq12g, port)
        || SOC_PBMP_MEMBER(pbmp_xq13g, port)
        ) {
            /* Controlling port not in 1G mode
           => Mark corresponding subsidiary ports as disabled
        */

            SOC_PBMP_PORT_ADD(pbmp_disabled, port + 1);
        SOC_PBMP_PORT_ADD(pbmp_disabled, port + 2);
        SOC_PBMP_PORT_ADD(pbmp_disabled, port + 3);
        }
    }

        break;

    case BCM56639_DEVICE_ID:
        for (port = 34; port <= 37; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 46; port <= 49; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(pbmp_tmp);
        SOC_PBMP_PORT_ADD(pbmp_tmp, 30);
        SOC_PBMP_PORT_ADD(pbmp_tmp, 38);
        SOC_PBMP_PORT_ADD(pbmp_tmp, 42);
        SOC_PBMP_PORT_ADD(pbmp_tmp, 50);
        SOC_PBMP_ASSIGN(pbmp_xq10g, pbmp_tmp);
        if (soc_property_get(unit, spn_BCM56639_28G_7X10, 0)) {
            /* 28x1GE + 7x10G + loopback */
            SOC_PBMP_PORT_REMOVE(pbmp_valid, 26);
            SOC_PBMP_PORT_ADD(pbmp_valid, 46);
            SOC_PBMP_PORT_ADD(pbmp_valid, 47);
            SOC_PBMP_PORT_ADD(pbmp_valid, 48);
            SOC_PBMP_PORT_ADD(pbmp_valid, 49);
        }

        SOC_PBMP_CLEAR(pbmp_tmp);
        for (port = 30; port <= 53; port++) {
            SOC_PBMP_PORT_ADD(pbmp_tmp, port);
        }
        SOC_PBMP_ASSIGN(pbmp_xq, pbmp_tmp);
        SOC_PBMP_AND(pbmp_xq, pbmp_valid);
        SOC_PBMP_ASSIGN(pbmp_hyplite, pbmp_xq);

    /* Flex-port support */

    /* For each possible controlling port, ... */
    for (port = 30; port <= 50; port += 4) {
        if (!SOC_PBMP_MEMBER(pbmp_valid, port))  continue;

        /* Controlling port is valid port
           => Add corresponding subsidiary ports as valid
        */

        SOC_PBMP_PORT_ADD(pbmp_valid, port + 1);
        SOC_PBMP_PORT_ADD(pbmp_valid, port + 2);
        SOC_PBMP_PORT_ADD(pbmp_valid, port + 3);

        if (SOC_PBMP_MEMBER(pbmp_xq10g, port)
        || SOC_PBMP_MEMBER(pbmp_xq12g, port)
        || SOC_PBMP_MEMBER(pbmp_xq13g, port)
        ) {
            /* Controlling port not in 1G mode
           => Mark corresponding subsidiary ports as disabled
        */

            SOC_PBMP_PORT_ADD(pbmp_disabled, port + 1);
        SOC_PBMP_PORT_ADD(pbmp_disabled, port + 2);
        SOC_PBMP_PORT_ADD(pbmp_disabled, port + 3);
        }
    }

        break;
    case BCM56689_DEVICE_ID:
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 1);
        /* Fall through */
    case BCM56685_DEVICE_ID:
        SOC_PBMP_CLEAR(pbmp_tmp);
        SOC_PBMP_PORT_ADD(pbmp_tmp, 26);
        SOC_PBMP_PORT_ADD(pbmp_tmp, 27);
        SOC_PBMP_PORT_ADD(pbmp_tmp, 28);
        SOC_PBMP_PORT_ADD(pbmp_tmp, 29);
        SOC_PBMP_ASSIGN(pbmp_gx12g, pbmp_tmp);
        SOC_PBMP_ASSIGN(pbmp_xq2p5g, pbmp_valid);
        SOC_PBMP_REMOVE(pbmp_xq2p5g, pbmp_gx12g);
        SOC_PBMP_CLEAR(pbmp_tmp);
        for (port = 30; port <= 53; port++) {
            SOC_PBMP_PORT_ADD(pbmp_tmp, port);
        }
        SOC_PBMP_ASSIGN(pbmp_xq, pbmp_tmp);
        SOC_PBMP_AND(pbmp_xq, pbmp_valid);
        SOC_PBMP_ASSIGN(pbmp_hyplite, pbmp_xq);
        break;
    case BCM56520_DEVICE_ID:
        for (port = 1; port <= 29; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(pbmp_tmp);
        for (port = 30; port <= 53; port++) {
            SOC_PBMP_PORT_ADD(pbmp_tmp, port);
        }
        SOC_PBMP_ASSIGN(pbmp_xq, pbmp_tmp);
        SOC_PBMP_AND(pbmp_xq, pbmp_valid);
        SOC_PBMP_ASSIGN(pbmp_hyplite, pbmp_xq);
        break;
    case BCM56521_DEVICE_ID:
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 1);
        for (port = 30; port <= 53; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(pbmp_tmp);
        if (soc_property_get(unit, spn_BCM56521_2X12_2X24, 0)) {
            /* 28x1GE + 2x12HG + 2x24HG + loopback */
            SOC_PBMP_PORT_ADD(pbmp_tmp, 26);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 27);
            SOC_PBMP_ASSIGN(pbmp_gx25g, pbmp_tmp);
            SOC_PBMP_CLEAR(pbmp_tmp);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 28);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 29);
            SOC_PBMP_ASSIGN(pbmp_gx13g, pbmp_tmp);
        } else {
            /* 24x1GE + 4x20HG + loopback */
            SOC_PBMP_PORT_ADD(pbmp_tmp, 26);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 27);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 28);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 29);
            if (soc_property_get(unit, spn_HIGIG_MAX_SPEED, 0) == 12000) {
                SOC_PBMP_ASSIGN(pbmp_gx12g, pbmp_tmp);
            } else {
                SOC_PBMP_ASSIGN(pbmp_gx20g, pbmp_tmp);
            }
        }
        break;
    case BCM56522_DEVICE_ID:
        for (port = 1; port <= 25; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 28; port <= 29; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(pbmp_tmp);
        SOC_PBMP_PORT_ADD(pbmp_tmp, 26);
        SOC_PBMP_PORT_ADD(pbmp_tmp, 27);
        if (soc_property_get(unit, spn_HIGIG_MAX_SPEED, 0) == 12000) {
            SOC_PBMP_ASSIGN(pbmp_gx12g, pbmp_tmp);
        } else {
            SOC_PBMP_ASSIGN(pbmp_gx25g, pbmp_tmp);
        }
        SOC_PBMP_CLEAR(pbmp_tmp);
        for (port = 30; port <= 53; port++) {
            SOC_PBMP_PORT_ADD(pbmp_tmp, port);
        }
        SOC_PBMP_ASSIGN(pbmp_xq, pbmp_tmp);
        SOC_PBMP_AND(pbmp_xq, pbmp_valid);
        SOC_PBMP_ASSIGN(pbmp_hyplite, pbmp_xq);
        break;
    case BCM56524_DEVICE_ID:
    /* Fall through */
    case BCM56534_DEVICE_ID:
        for (port = 1; port <= 25; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(pbmp_tmp);
        if (soc_property_get(unit, spn_BCM56524_2X12_2X24, 0) ||
            soc_property_get(unit, spn_BCM56534_2X12_2X24, 0)) {
            /* 28x1GE + 2x12HG + 2x24HG + loopback */
            SOC_PBMP_PORT_ADD(pbmp_tmp, 26);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 27);
            SOC_PBMP_ASSIGN(pbmp_gx25g, pbmp_tmp);
            SOC_PBMP_CLEAR(pbmp_tmp);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 28);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 29);
            SOC_PBMP_ASSIGN(pbmp_gx13g, pbmp_tmp);
        } else {
            /* 24x1GE + 4x20HG + loopback */
            SOC_PBMP_PORT_ADD(pbmp_tmp, 26);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 27);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 28);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 29);
            if (soc_property_get(unit, spn_HIGIG_MAX_SPEED, 0) == 12000) {
                SOC_PBMP_ASSIGN(pbmp_gx12g, pbmp_tmp);
            } else {
                SOC_PBMP_ASSIGN(pbmp_gx20g, pbmp_tmp);
            }
        }
        SOC_PBMP_CLEAR(pbmp_tmp);
        for (port = 30; port <= 53; port++) {
            SOC_PBMP_PORT_ADD(pbmp_tmp, port);
        }
        SOC_PBMP_ASSIGN(pbmp_xq, pbmp_tmp);
        SOC_PBMP_AND(pbmp_xq, pbmp_valid);
        SOC_PBMP_ASSIGN(pbmp_hyplite, pbmp_xq);
        break;
    case BCM56526_DEVICE_ID:
        for (port = 30; port <= 37; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 42; port <= 45; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 47; port <= 49; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 51; port <= 53; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(pbmp_tmp);
        SOC_PBMP_PORT_ADD(pbmp_tmp, 26);
        SOC_PBMP_PORT_ADD(pbmp_tmp, 27);
        SOC_PBMP_PORT_ADD(pbmp_tmp, 28);
        SOC_PBMP_PORT_ADD(pbmp_tmp, 29);
        if (soc_property_get(unit, spn_BCM56526_2X12_4X16, 0)) {
            SOC_PBMP_ASSIGN(pbmp_gx16g, pbmp_tmp);
            for (port = 38; port <= 41; port++) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }
            SOC_PBMP_CLEAR(pbmp_tmp);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 46);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 50);
            SOC_PBMP_ASSIGN(pbmp_xq12g, pbmp_tmp);
        } else {
            SOC_PBMP_ASSIGN(pbmp_gx13g, pbmp_tmp);
            SOC_PBMP_CLEAR(pbmp_tmp);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 46);
            SOC_PBMP_PORT_ADD(pbmp_tmp, 50);
            SOC_PBMP_ASSIGN(pbmp_xq13g, pbmp_tmp);
        }
        SOC_PBMP_CLEAR(pbmp_tmp);
        for (port = 30; port <= 53; port++) {
            SOC_PBMP_PORT_ADD(pbmp_tmp, port);
        }
        SOC_PBMP_ASSIGN(pbmp_xq, pbmp_tmp);
        SOC_PBMP_AND(pbmp_xq, pbmp_valid);
        SOC_PBMP_ASSIGN(pbmp_hyplite, pbmp_xq);

    /* Flex-port support */

    /* For each possible controlling port, ... */
    for (port = 30; port <= 50; port += 4) {
        if (!SOC_PBMP_MEMBER(pbmp_valid, port))  continue;

        /* Controlling port is valid port
           => Add corresponding subsidiary ports as valid
        */

        SOC_PBMP_PORT_ADD(pbmp_valid, port + 1);
        SOC_PBMP_PORT_ADD(pbmp_valid, port + 2);
        SOC_PBMP_PORT_ADD(pbmp_valid, port + 3);

        if (SOC_PBMP_MEMBER(pbmp_xq10g, port)
        || SOC_PBMP_MEMBER(pbmp_xq12g, port)
        || SOC_PBMP_MEMBER(pbmp_xq13g, port)
        ) {
            /* Controlling port not in 1G mode
           => Mark corresponding subsidiary ports as disabled
        */

            SOC_PBMP_PORT_ADD(pbmp_disabled, port + 1);
        SOC_PBMP_PORT_ADD(pbmp_disabled, port + 2);
        SOC_PBMP_PORT_ADD(pbmp_disabled, port + 3);
        }
    }

        break;

#if defined(BCM_ENDURO_SUPPORT)
    case BCM56132_DEVICE_ID:
        /* 24 + 2 + 2 ports */
        for (port = 30; port <= 31; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        if(dev_id == BCM56132_DEVICE_ID) {
            SOC_PBMP_CLEAR(pbmp_tmp);
            for (port = 2; port <= 25; port++) {
                SOC_PBMP_PORT_ADD(pbmp_tmp, port);
            }
            SOC_PBMP_ASSIGN(pbmp_gport_fe, pbmp_tmp);
        }
        SOC_PBMP_CLEAR(pbmp_tmp);
        for (port = 26; port <= 27; port++) {
            SOC_PBMP_PORT_ADD(pbmp_tmp, port);
        }
        SOC_PBMP_ASSIGN(pbmp_xq2p5g, pbmp_tmp);
        SOC_PBMP_CLEAR(pbmp_tmp);
        for (port = 28; port <= 29; port++) {
            SOC_PBMP_PORT_ADD(pbmp_tmp, port);
        }
        SOC_PBMP_ASSIGN(pbmp_xq13g, pbmp_tmp);
        SOC_PBMP_CLEAR(pbmp_tmp);
        for (port = 26; port <= 29; port++) {
            SOC_PBMP_PORT_ADD(pbmp_tmp, port);
        }
        SOC_PBMP_ASSIGN(pbmp_xq, pbmp_tmp);

        SOC_PBMP_PORT_ADD(pbmp_tmp, 1);
        SOC_PBMP_AND(si->lb_pbm, pbmp_tmp);
        break;
    case BCM56134_DEVICE_ID:
    case BCM56331_DEVICE_ID:
        /* 24 + 4 ports */
        for (port = 30; port <= 31; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        if(dev_id == BCM56134_DEVICE_ID) {
            SOC_PBMP_CLEAR(pbmp_tmp);
            for (port = 2; port <= 25; port++) {
                SOC_PBMP_PORT_ADD(pbmp_tmp, port);
            }
            SOC_PBMP_ASSIGN(pbmp_gport_fe, pbmp_tmp);
        }
        SOC_PBMP_CLEAR(pbmp_tmp);
        for (port = 26; port <= 29; port++) {
            SOC_PBMP_PORT_ADD(pbmp_tmp, port);
        }
        SOC_PBMP_ASSIGN(pbmp_xq2p5g, pbmp_tmp);
        SOC_PBMP_ASSIGN(pbmp_xq, pbmp_tmp);

        SOC_PBMP_PORT_ADD(pbmp_tmp, 1);
        SOC_PBMP_AND(si->lb_pbm, pbmp_tmp);
        break;
    case BCM56334_DEVICE_ID:
    case BCM56320_DEVICE_ID:
    case BCM56321_DEVICE_ID:
        /* 24 + 4 ports */
        for (port = 30; port <= 31; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(pbmp_tmp);
        for (port = 26; port <= 29; port++) {
            SOC_PBMP_PORT_ADD(pbmp_tmp, port);
        }
        SOC_PBMP_ASSIGN(pbmp_xq13g, pbmp_tmp);
        SOC_PBMP_ASSIGN(pbmp_xq, pbmp_tmp);

        SOC_PBMP_PORT_ADD(pbmp_tmp, 1);
        SOC_PBMP_AND(si->lb_pbm, pbmp_tmp);
        break;
    case BCM56333_DEVICE_ID:
        /* 16 ports */
        for (port = 18; port <= 31; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(pbmp_tmp);
        SOC_PBMP_PORT_ADD(pbmp_tmp, 1);
        SOC_PBMP_AND(si->lb_pbm, pbmp_tmp);
        break;
    case BCM56338_DEVICE_ID:
        /* 8 + 2 ports */
        for (port = 10; port <= 25; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 28; port <= 31; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(pbmp_tmp);
        for (port = 26; port <= 27; port++) {
            SOC_PBMP_PORT_ADD(pbmp_tmp, port);
        }
        SOC_PBMP_ASSIGN(pbmp_xq13g, pbmp_tmp);
        SOC_PBMP_ASSIGN(pbmp_xq, pbmp_tmp);

        SOC_PBMP_PORT_ADD(pbmp_tmp, 1);
        SOC_PBMP_AND(si->lb_pbm, pbmp_tmp);
        break;
    case BCM56230_DEVICE_ID:
        /* 12 ports */
        for (port = 14; port <= 31; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(pbmp_tmp);
        SOC_PBMP_PORT_ADD(pbmp_tmp, 1);
        SOC_PBMP_AND(si->lb_pbm, pbmp_tmp);
        break;
    case BCM56231_DEVICE_ID:
        /* 6 ports */
        for (port = 8; port <= 31; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(pbmp_tmp);
        SOC_PBMP_PORT_ADD(pbmp_tmp, 1);
        SOC_PBMP_AND(si->lb_pbm, pbmp_tmp);
        break;
#endif /* BCM_ENDURO_SUPPORT */

#if defined(BCM_HURRICANE2_SUPPORT)
    case BCM56150_DEVICE_ID:
    case BCM56151_DEVICE_ID:
    case BCM56152_DEVICE_ID:
    case BCM53342_DEVICE_ID:
    case BCM53343_DEVICE_ID:
    case BCM53344_DEVICE_ID:
    case BCM53346_DEVICE_ID:
    case BCM53347_DEVICE_ID:
    case BCM53333_DEVICE_ID:
    case BCM53334_DEVICE_ID:
    case BCM53393_DEVICE_ID:
    case BCM53394_DEVICE_ID:
        SOC_FEATURE_SET(unit, soc_feature_logical_port_num);
        if (soc_hu2_port_config_init(unit, dev_id) < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Port config error !!\n")));
            return SOC_E_CONFIG;
        }
        SOC_PBMP_CLEAR(si->lb_pbm);
        break;
#endif /* BCM_HURRICANE2_SUPPORT */

#if defined(BCM_HURRICANE3_SUPPORT)
    case BCM56160_DEVICE_ID:
    case BCM56162_DEVICE_ID:
    case BCM56163_DEVICE_ID:
    case BCM56164_DEVICE_ID:
    case BCM56166_DEVICE_ID:
    case BCM53440_DEVICE_ID:
    case BCM53443_DEVICE_ID:
    case BCM53442_DEVICE_ID:
    case BCM53434_DEVICE_ID:
        SOC_FEATURE_SET(unit, soc_feature_logical_port_num);
        if (soc_hurricane3_port_config_init(unit, dev_id) < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "Port config error !!\n")));
            return SOC_E_CONFIG;
        }
        SOC_PBMP_CLEAR(si->lb_pbm);
        break;
    case BCM53540_DEVICE_ID:
    case BCM53547_DEVICE_ID:
    case BCM53548_DEVICE_ID:
    case BCM53549_DEVICE_ID:
        SOC_FEATURE_SET(unit, soc_feature_logical_port_num);
        if (soc_wolfhound2_port_config_init(unit, dev_id) < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit, "Port config error !!\n")));
            return SOC_E_CONFIG;
        }
        SOC_PBMP_CLEAR(si->lb_pbm);
        break;
#endif /* BCM_HURRICANE3_SUPPORT */


#if defined(BCM_GREYHOUND2_SUPPORT)
    case BCM56170_DEVICE_ID:
    case BCM56172_DEVICE_ID:
    case BCM56174_DEVICE_ID:
    case BCM53570_DEVICE_ID:
    case BCM53575_DEVICE_ID:
        SOC_FEATURE_SET(unit, soc_feature_logical_port_num);
        /* coverity[stack_use_callee : FALSE] */
        /* coverity[stack_use_overflow : FALSE] */
        if (soc_greyhound2_port_config_init(unit, dev_id) < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "Port config error !!\n")));
            return SOC_E_CONFIG;
        }

        if (soc_feature(unit, soc_feature_global_meter)) {
            si->global_meter_pools = 1;
            /* size of SVM_METER_TABLE, divide by 2 for exceed/commit */
            si->global_meter_size_of_pool =
                soc_mem_index_count(unit, SVM_METER_TABLEm) / 2;
            si->global_meter_max_size_of_pool =
                soc_mem_index_count(unit, SVM_METER_TABLEm) / 2;
            si->num_flex_ingress_pools = 0;
            si->num_flex_egress_pools = 0;
            si->global_meter_tcam_size =
                soc_mem_index_count(unit, SVC_METER_OFFSET_TCAMm);
            si->global_meter_offset_mode_max =
                (1 << soc_mem_field_length(unit, SVC_METER_OFFSET_TCAMm,
                                           OFFSET_MODEf));
            si->global_meter_action_size =
                soc_mem_index_count(unit, SVM_POLICY_TABLEm);
        } else {
            si->global_meter_pools = 0;
            si->global_meter_size_of_pool = 0;
            si->global_meter_max_size_of_pool = 0;
            si->num_flex_ingress_pools = 0;
            si->num_flex_egress_pools = 0;
            si->global_meter_tcam_size = 0;
            si->global_meter_offset_mode_max = 0;
            si->global_meter_action_size = 0;
        }
        break;
#endif /* BCM_GREYHOUND2_SUPPORT */

#if defined(BCM_FIRELIGHT_SUPPORT)
    case BCM56070_DEVICE_ID:
    case BCM56071_DEVICE_ID:
    case BCM56072_DEVICE_ID:
        SOC_FEATURE_SET(unit, soc_feature_logical_port_num);
        /* coverity[stack_use_callee : FALSE] */
        /* coverity[stack_use_overflow : FALSE] */
        if (soc_firelight_port_config_init(unit, dev_id) < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "Port config error !!\n")));
            return SOC_E_CONFIG;
        }

        if (soc_feature(unit, soc_feature_global_meter)) {
            si->global_meter_pools = 1;
            /* size of SVM_METER_TABLE, divide by 2 for exceed/commit */
            si->global_meter_size_of_pool =
                soc_mem_index_count(unit, SVM_METER_TABLEm) / 2;
            si->global_meter_max_size_of_pool =
                soc_mem_index_count(unit, SVM_METER_TABLEm) / 2;
            si->num_flex_ingress_pools = 0;
            si->num_flex_egress_pools = 0;
            si->global_meter_tcam_size =
                soc_mem_index_count(unit, SVC_METER_OFFSET_TCAMm);
            si->global_meter_offset_mode_max =
                (1 << soc_mem_field_length(unit, SVC_METER_OFFSET_TCAMm,
                                           OFFSET_MODEf));
            si->global_meter_action_size =
                soc_mem_index_count(unit, SVM_POLICY_TABLEm);
        } else {
            si->global_meter_pools = 0;
            si->global_meter_size_of_pool = 0;
            si->global_meter_max_size_of_pool = 0;
            si->num_flex_ingress_pools = 0;
            si->num_flex_egress_pools = 0;
            si->global_meter_tcam_size = 0;
            si->global_meter_offset_mode_max = 0;
            si->global_meter_action_size = 0;
        }
        break;
#endif /* BCM_FIRELIGHT_SUPPORT */

#if defined(BCM_GREYHOUND_SUPPORT)
    case BCM53400_DEVICE_ID:
    case BCM53401_DEVICE_ID:
    case BCM53402_DEVICE_ID:
    case BCM53403_DEVICE_ID:
    case BCM53404_DEVICE_ID:
    case BCM53405_DEVICE_ID:
    case BCM53365_DEVICE_ID:
    case BCM53406_DEVICE_ID:
    case BCM53369_DEVICE_ID:
    case BCM53408_DEVICE_ID:
    case BCM56060_DEVICE_ID:
    case BCM56062_DEVICE_ID:
    case BCM56063_DEVICE_ID:
    case BCM56064_DEVICE_ID:
    case BCM56065_DEVICE_ID:
    case BCM56066_DEVICE_ID:
    case BCM53454_DEVICE_ID:
    case BCM53456_DEVICE_ID:
    case BCM53422_DEVICE_ID:
    case BCM53424_DEVICE_ID:
    case BCM53426_DEVICE_ID:
        SOC_FEATURE_SET(unit, soc_feature_logical_port_num);
        if (soc_greyhound_port_config_init(unit, dev_id) < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Port config error !!")));
            return SOC_E_CONFIG;
        }
        SOC_PBMP_CLEAR(si->lb_pbm);
        break;
#endif /* BCM_GREYHOUND_SUPPORT */
#if defined(BCM_HURRICANE_SUPPORT)
    case BCM56140_DEVICE_ID:
        if (!soc_property_get(unit, spn_BCM5614X_CONFIG, 0)) {
            for (port = 2; port <= 25; port++) {
                if ((port-2)%4) {
                    SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
                }
            }
        }
        SOC_PBMP_CLEAR(pbmp_tmp);
        for (port = 26; port <= 29; port++) {
            SOC_PBMP_PORT_ADD(pbmp_tmp, port);
            si->port_speed_max[port] = 2500;
        }
        if (!SOC_PBMP_MEMBER(pbmp_valid, 27)) {
            /* Make Port 26 as HG[13] */
            SOC_PBMP_PORT_REMOVE(pbmp_tmp, 26);
            SOC_PBMP_PORT_REMOVE(pbmp_tmp, 27);
            SOC_PBMP_PORT_ADD(pbmp_xq13g, 26);
            si->port_speed_max[26] = 13000;
        }
        if (!SOC_PBMP_MEMBER(pbmp_valid, 29)) {
            /* Make Port 28 as HG[13] */
            SOC_PBMP_PORT_REMOVE(pbmp_tmp, 28);
            SOC_PBMP_PORT_REMOVE(pbmp_tmp, 29);
            SOC_PBMP_PORT_ADD(pbmp_xq13g, 28);
            si->port_speed_max[28] = 13000;
        }
        SOC_PBMP_ASSIGN(pbmp_xq2p5g, pbmp_tmp);
        SOC_PBMP_ASSIGN(pbmp_xq, pbmp_tmp);
        SOC_PBMP_OR(pbmp_xq, pbmp_xq13g);
        SOC_PBMP_CLEAR(si->lb_pbm);
        break;
    case BCM56142_DEVICE_ID:
        SOC_PBMP_CLEAR(pbmp_tmp);
        for (port = 26; port <= 29; port++) {
            /* Full Capacity (24x1G + 4xHGd) for testing */
            SOC_PBMP_PORT_ADD(pbmp_tmp, port);
        }
        if (soc_property_get(unit, spn_BCM5614X_CONFIG, 1) == 2) {
              /* 24x1G + 2HG */
            SOC_PBMP_PORT_REMOVE(pbmp_valid, 27);
            SOC_PBMP_PORT_REMOVE(pbmp_valid, 29);
            SOC_PBMP_PORT_REMOVE(pbmp_tmp, 27);
            SOC_PBMP_PORT_REMOVE(pbmp_tmp, 29);
        } else if (soc_property_get(unit, spn_BCM5614X_CONFIG, 1) != 0) {
             /* 24x1G + 1xHG + 2xHGd (dxgxs=1) or
                24x1G + 2xHGd + 1xHG (dxgxs=0)*/
            SOC_PBMP_PORT_REMOVE(pbmp_valid, dxgxs ? 27 : 29);
            SOC_PBMP_PORT_REMOVE(pbmp_tmp, dxgxs ? 27 : 29);
        }

        SOC_PBMP_ASSIGN(pbmp_xq13g, pbmp_tmp);
        SOC_PBMP_ASSIGN(pbmp_xq, pbmp_tmp);

        /* For HGd ports, 12.768 GB is set as 12G */
        if (dxgxs) {
            si->port_speed_max[28] = 12000;
            si->port_speed_max[29] = 12000;
        } else {
            si->port_speed_max[26] = 12000;
            si->port_speed_max[27] = 12000;
        }

        if (soc_property_get(unit, spn_BCM5614X_CONFIG, 1) == 2) {
            if (SOC_PBMP_MEMBER(pbmp_valid, 26)) {
                si->port_speed_max[26] = 13000;
            }
            if (SOC_PBMP_MEMBER(pbmp_valid, 28)) {
                si->port_speed_max[28] = 13000;
            }
        } else {
            if (SOC_PBMP_MEMBER(pbmp_valid, 27)) {
                si->port_speed_max[26] = 12000;
                si->port_speed_max[27] = 12000;
            } else {
                si->port_speed_max[26] = 13000;
            }
            if (SOC_PBMP_MEMBER(pbmp_valid, 29)) {
                si->port_speed_max[28] = 12000;
                si->port_speed_max[29] = 12000;
            } else {
                si->port_speed_max[28] = 13000;
            }
        }

        for (port = 30; port <= 31; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(si->lb_pbm);
        break;
    case BCM56143_DEVICE_ID:
        if (!soc_property_get(unit, spn_BCM5614X_CONFIG, 0)) {
            /* 24x1G + 2xHGdG + 2x2.5G (dxgxs=1) or
               24x1G + 2x2.5G + 2xHGd (dxgxs=0) */
            SOC_PBMP_CLEAR(pbmp_tmp);
            for (port = 26; port <= 27; port++) {
                SOC_PBMP_PORT_ADD(pbmp_tmp, port);
            }
            if(dxgxs) {
                SOC_PBMP_ASSIGN(pbmp_xq13g, pbmp_tmp);
            } else {
                SOC_PBMP_ASSIGN(pbmp_xq2p5g, pbmp_tmp);
            }
            SOC_PBMP_CLEAR(pbmp_tmp);
            for (port = 28; port <= 29; port++) {
                SOC_PBMP_PORT_ADD(pbmp_tmp, port);
            }
            if(dxgxs) {
                SOC_PBMP_ASSIGN(pbmp_xq2p5g, pbmp_tmp);
            } else {
                SOC_PBMP_ASSIGN(pbmp_xq13g, pbmp_tmp);
            }
            SOC_PBMP_CLEAR(pbmp_tmp);
            for (port = 26; port <= 29; port++) {
                SOC_PBMP_PORT_ADD(pbmp_tmp, port);
            }
            SOC_PBMP_ASSIGN(pbmp_xq, pbmp_tmp);

            for (port = 30; port <= 31; port++) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }
            SOC_PBMP_CLEAR(si->lb_pbm);
        } else {
            /* 24x1G + 4xG/4x2.5G */
            SOC_PBMP_CLEAR(pbmp_tmp);
            for (port = 26; port <= 29; port++) {
                SOC_PBMP_PORT_ADD(pbmp_tmp, port);
            }
            SOC_PBMP_ASSIGN(pbmp_xq2p5g, pbmp_tmp);
            SOC_PBMP_ASSIGN(pbmp_xq, pbmp_tmp);
            si->port_speed_max[26] = 2500;
            si->port_speed_max[27] = 2500;
            si->port_speed_max[28] = 2500;
            si->port_speed_max[29] = 2500;
        }
        break;
    case BCM56144_DEVICE_ID:
        /* 16x1G + 2x2.5G + 2x2.5G */
        for (port = 18; port <= 25; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(pbmp_tmp);
        for (port = 26; port <= 29; port++) {
            SOC_PBMP_PORT_ADD(pbmp_tmp, port);
        }
        SOC_PBMP_ASSIGN(pbmp_xq2p5g, pbmp_tmp);
        SOC_PBMP_ASSIGN(pbmp_xq, pbmp_tmp);
        SOC_PBMP_CLEAR(si->lb_pbm);
        si->port_speed_max[26] = 2500;
        si->port_speed_max[27] = 2500;
        si->port_speed_max[28] = 2500;
        si->port_speed_max[29] = 2500;
        break;
    case BCM56146_DEVICE_ID:
        /* 24xFE + 2x2.5G + 2x2.5G */
        SOC_PBMP_CLEAR(pbmp_tmp);
        for (port = 2; port <= 25; port++) {
            SOC_PBMP_PORT_ADD(pbmp_tmp, port);
            si->port_speed_max[port] = 100;
        }
        SOC_PBMP_ASSIGN(pbmp_gport_fe, pbmp_tmp);

        SOC_PBMP_CLEAR(pbmp_tmp);
        for (port = 26; port <= 29; port++) {
            SOC_PBMP_PORT_ADD(pbmp_tmp, port);
            si->port_speed_max[port] = 2500;
        }
        SOC_PBMP_ASSIGN(pbmp_xq2p5g, pbmp_tmp);
        SOC_PBMP_ASSIGN(pbmp_xq, pbmp_tmp);
        SOC_PBMP_CLEAR(si->lb_pbm);
        break;
    case BCM56147_DEVICE_ID:
        /* 24xFE + 1xHG + 2x2.5G */
        SOC_PBMP_CLEAR(pbmp_tmp);
        for (port = 2; port <= 25; port++) {
            SOC_PBMP_PORT_ADD(pbmp_tmp, port);
            si->port_speed_max[port] = 100;
        }
        SOC_PBMP_ASSIGN(pbmp_gport_fe, pbmp_tmp);

        SOC_PBMP_CLEAR(pbmp_tmp);
        SOC_PBMP_CLEAR(pbmp_xq13g);
        for (port = 26; port <= 29; port++) {
            SOC_PBMP_PORT_ADD(pbmp_tmp, port);
            si->port_speed_max[port] = 2500;
        }
        if (!soc_property_get(unit, spn_BCM5614X_CONFIG, 0)) {
            if (dxgxs) {
                /* Make Port 26 as HG[13] */
                SOC_PBMP_PORT_REMOVE(pbmp_tmp, 26);
                SOC_PBMP_PORT_REMOVE(pbmp_tmp, 27);
                SOC_PBMP_PORT_REMOVE(pbmp_valid, 27);
                SOC_PBMP_PORT_ADD(pbmp_xq13g, 26);
                si->port_speed_max[26] = 13000;
            } else {
                /* Make Port 28 as HG[13] */
                SOC_PBMP_PORT_REMOVE(pbmp_tmp, 28);
                SOC_PBMP_PORT_REMOVE(pbmp_tmp, 29);
                SOC_PBMP_PORT_REMOVE(pbmp_valid, 29);
                SOC_PBMP_PORT_ADD(pbmp_xq13g, 28);
                si->port_speed_max[28] = 13000;
            }
        }
        SOC_PBMP_ASSIGN(pbmp_xq2p5g, pbmp_tmp);
        SOC_PBMP_ASSIGN(pbmp_xq, pbmp_tmp);
        SOC_PBMP_OR(pbmp_xq, pbmp_xq13g);
        SOC_PBMP_CLEAR(si->lb_pbm);
        break;
    case BCM56149_DEVICE_ID:
        /* 24x1G + 2xXAUI + 2xHGd  or
           24x1G + 2xHgd + 2xXAUI */
        SOC_PBMP_CLEAR(pbmp_tmp);
        for (port = 26; port <= 29; port++) {
            SOC_PBMP_PORT_ADD(pbmp_tmp, port);
        }
        SOC_PBMP_ASSIGN(pbmp_xq, pbmp_tmp);
        SOC_PBMP_ASSIGN(pbmp_xq13g, pbmp_tmp);

        SOC_PBMP_CLEAR(pbmp_tmp);
        if (dxgxs) {
            /* 24x1G + 2xHgd + 2xXAUI */
            if (!SOC_PBMP_MEMBER(pbmp_xport_xe, 26) &&
                !SOC_PBMP_MEMBER(pbmp_xport_xe, 27)) {
                si->port_speed_max[26] = 12000;
                si->port_speed_max[27] = 12000;
                SOC_PBMP_WORD_SET(pbmp_gport_stack, 0, 0x0c000000);
            }
            SOC_PBMP_PORT_ADD(pbmp_xport_xe, 28);
            SOC_PBMP_PORT_ADD(pbmp_xport_xe, 29);
        } else {
            /* 24x1G + 2xXAUI + 2xHGd */
            SOC_PBMP_PORT_ADD(pbmp_xport_xe, 26);
            SOC_PBMP_PORT_ADD(pbmp_xport_xe, 27);
            if (!SOC_PBMP_MEMBER(pbmp_xport_xe, 28) &&
                !SOC_PBMP_MEMBER(pbmp_xport_xe, 29)) {
                si->port_speed_max[28] = 12000;
                si->port_speed_max[29] = 12000;
                SOC_PBMP_WORD_SET(pbmp_gport_stack, 0, 0x30000000);
            }
        }

        for (port = 30; port <= 31; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(si->lb_pbm);
        break;
#endif /* BCM_HURRICANE_SUPPORT */

#ifdef BCM_TRIUMPH_SUPPORT
    case BCM56624_DEVICE_ID:
        SOC_PORT_SPEED_MAX_SET(unit, 28, 16000);
        SOC_PORT_SPEED_MAX_SET(unit, 29, 16000);
        SOC_PORT_SPEED_MAX_SET(unit, 30, 16000);
        SOC_PORT_SPEED_MAX_SET(unit, 31, 16000);
        for (port = 2; port <= 7; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        for (port = 14; port <= 19; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        for (port = 26; port <= 27; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        for (port = 32; port <= 36; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        for (port = 43; port <= 47; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        SOC_PBMP_AND(pbmp_hyplite, pbmp_valid);
        break;
    case BCM56626_DEVICE_ID:
        for (port = 32; port <= 53; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PORT_SPEED_MAX_SET(unit, 26, 12000);
        SOC_PORT_SPEED_MAX_SET(unit, 27, 12000);
        SOC_PORT_SPEED_MAX_SET(unit, 28, 16000);
        SOC_PORT_SPEED_MAX_SET(unit, 29, 16000);
        SOC_PORT_SPEED_MAX_SET(unit, 30, 16000);
        SOC_PORT_SPEED_MAX_SET(unit, 31, 16000);
        for (port = 2; port <= 7; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        for (port = 14; port <= 19; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        for (port = 26; port <= 27; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        SOC_PBMP_AND(pbmp_hyplite, pbmp_valid);
        break;
    case BCM56628_DEVICE_ID:
        for (port = 3; port <= 13; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 15; port <= 25; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 32; port <= 53; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PORT_SPEED_MAX_SET(unit, 2, 12000);
        SOC_PORT_SPEED_MAX_SET(unit, 14, 12000);
        SOC_PORT_SPEED_MAX_SET(unit, 26, 12000);
        SOC_PORT_SPEED_MAX_SET(unit, 27, 12000);
        SOC_PORT_SPEED_MAX_SET(unit, 28, 16000);
        SOC_PORT_SPEED_MAX_SET(unit, 29, 16000);
        SOC_PORT_SPEED_MAX_SET(unit, 30, 16000);
        SOC_PORT_SPEED_MAX_SET(unit, 31, 16000);
        for (port = 14; port <= 19; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        for (port = 26; port <= 27; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        SOC_PBMP_AND(pbmp_hyplite, pbmp_valid);
        break;
    case BCM56629_DEVICE_ID:
        if (soc_feature(unit, soc_feature_xgport_one_xe_six_ge)) {
            if (soc_property_get(unit, spn_BCM56629_40GE, 0)) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, 6);
                SOC_PBMP_PORT_REMOVE(pbmp_valid, 7);
                SOC_PBMP_PORT_REMOVE(pbmp_valid, 18);
                SOC_PBMP_PORT_REMOVE(pbmp_valid, 19);
                SOC_PBMP_PORT_REMOVE(pbmp_valid, 35);
                SOC_PBMP_PORT_REMOVE(pbmp_valid, 36);
                SOC_PBMP_PORT_REMOVE(pbmp_valid, 46);
                SOC_PBMP_PORT_REMOVE(pbmp_valid, 47);
            } else {
                for (port = 3; port <= 7; port++) {
                    SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
                }
                for (port = 15; port <= 19; port++) {
                    SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
                }
                for (port = 32; port <= 36; port++) {
                    SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
                }
                for (port = 43; port <= 47; port++) {
                    SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
                }
                SOC_PORT_SPEED_MAX_SET(unit, 2, 10000);
                SOC_PORT_SPEED_MAX_SET(unit, 14, 10000);
                SOC_PORT_SPEED_MAX_SET(unit, 26, 10000);
                SOC_PORT_SPEED_MAX_SET(unit, 27, 10000);
            }
        } else {
            for (port = 32; port <= 53; port++) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }
            SOC_PORT_SPEED_MAX_SET(unit, 26, 12000);
            SOC_PORT_SPEED_MAX_SET(unit, 27, 12000);
        }
        for (port = 2; port <= 7; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        for (port = 14; port <= 19; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        for (port = 26; port <= 27; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        for (port = 32; port <= 36; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        for (port = 43; port <= 47; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        SOC_PBMP_AND(pbmp_hyplite, pbmp_valid);
        SOC_PORT_SPEED_MAX_SET(unit, 28, 12000);
        SOC_PORT_SPEED_MAX_SET(unit, 29, 12000);
        SOC_PORT_SPEED_MAX_SET(unit, 30, 12000);
        SOC_PORT_SPEED_MAX_SET(unit, 31, 12000);
        break;
    case BCM56620_DEVICE_ID:
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 1);
        SOC_PORT_SPEED_MAX_SET(unit, 28, 16000);
        SOC_PORT_SPEED_MAX_SET(unit, 29, 16000);
        SOC_PORT_SPEED_MAX_SET(unit, 30, 16000);
        SOC_PORT_SPEED_MAX_SET(unit, 31, 16000);
        for (port = 2; port <= 7; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        for (port = 14; port <= 19; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        for (port = 26; port <= 27; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        for (port = 32; port <= 36; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        for (port = 43; port <= 47; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        SOC_PBMP_AND(pbmp_hyplite, pbmp_valid);
        break;
    case BCM56684_DEVICE_ID:
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 1);
        /* Fall through */
    case BCM56680_DEVICE_ID:
        SOC_PBMP_CLEAR(pbmp_tmp);
        SOC_PORT_SPEED_MAX_SET(unit, 28, 12000);
        SOC_PORT_SPEED_MAX_SET(unit, 29, 12000);
        SOC_PORT_SPEED_MAX_SET(unit, 30, 12000);
        SOC_PORT_SPEED_MAX_SET(unit, 31, 12000);
        SOC_PBMP_ASSIGN(pbmp_xg2p5g, pbmp_valid);
        SOC_PBMP_PORT_REMOVE(pbmp_xg2p5g, 28);
        SOC_PBMP_PORT_REMOVE(pbmp_xg2p5g, 29);
        SOC_PBMP_PORT_REMOVE(pbmp_xg2p5g, 30);
        SOC_PBMP_PORT_REMOVE(pbmp_xg2p5g, 31);
        for (port = 2; port <= 7; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        for (port = 14; port <= 19; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        for (port = 26; port <= 27; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        for (port = 32; port <= 36; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        for (port = 43; port <= 47; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        SOC_PBMP_AND(pbmp_hyplite, pbmp_valid);
        break;
    case BCM56686_DEVICE_ID:
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 1);
        for (port = 3; port <= 13; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 14; port <= 26; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 32; port <= 36; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 43; port <= 47; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        /* coverity[result_independent_of_operands] */
        SOC_PORT_SPEED_MAX_SET(unit, 2, 10000);
        SOC_PORT_SPEED_MAX_SET(unit, 27, 10000);
        for (port = 2; port <= 7; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        for (port = 26; port <= 27; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        SOC_PBMP_AND(pbmp_hyplite, pbmp_valid);
        break;
#endif /* BCM_TRIUMPH_SUPPORT */
    case BCM56820_DEVICE_ID:
        for (port = 9; port <= 16; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        break;
    case BCM56821_DEVICE_ID:
        for (port = 11; port <= 14; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        if (soc_property_get(unit, spn_BCM56821_20X12, 0)) {
            SOC_PBMP_ASSIGN(pbmp_gx12g, pbmp_valid);
            SOC_PBMP_CLEAR(pbmp_tmp);
            SOC_PBMP_WORD_SET(pbmp_tmp, 0, 0x01ff87fe);
            SOC_PBMP_ASSIGN(pbmp_gx12g, pbmp_tmp);
        } else {
            SOC_PBMP_ASSIGN(pbmp_gx16g, pbmp_valid);
            SOC_PBMP_CLEAR(pbmp_tmp);
            SOC_PBMP_WORD_SET(pbmp_tmp, 0, 0x01e0001e);
            SOC_PBMP_AND(pbmp_gx16g, pbmp_tmp);
        }
        for (port = 9; port <= 16; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        SOC_PBMP_AND(pbmp_hyplite, pbmp_valid);
        break;
    case BCM56822_DEVICE_ID:
        for (port = 1; port <= 4; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        if (soc_property_get(unit, spn_BCM56822_8X16, 0)) {
            SOC_PBMP_ASSIGN(pbmp_gx16g, pbmp_valid);
            SOC_PBMP_CLEAR(pbmp_tmp);
            SOC_PBMP_WORD_SET(pbmp_tmp, 0, 0x00001fe0);
            SOC_PBMP_AND(pbmp_gx16g, pbmp_tmp);
        } else {
            for (port = 5; port <= 6; port++) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }
            SOC_PBMP_ASSIGN(pbmp_gx21g, pbmp_valid);
            SOC_PBMP_CLEAR(pbmp_tmp);
            SOC_PBMP_WORD_SET(pbmp_tmp, 0, 0x00001e00);
            SOC_PBMP_AND(pbmp_gx21g, pbmp_tmp);
            SOC_PBMP_ASSIGN(pbmp_gx16g, pbmp_valid);
            SOC_PBMP_CLEAR(pbmp_tmp);
            SOC_PBMP_WORD_SET(pbmp_tmp, 0, 0x00000180);
            SOC_PBMP_AND(pbmp_gx16g, pbmp_tmp);
        }
        for (port = 9; port <= 16; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        SOC_PBMP_AND(pbmp_hyplite, pbmp_valid);
        break;
    case BCM56725_DEVICE_ID:
        for (port = 25; port <= 28; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        if (soc_property_get(unit, spn_BCM56725_16X16, 0)) {
            for (port = 1; port <= 4; port++) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }
            for (port = 21; port <= 24; port++) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }
            SOC_PBMP_ASSIGN(pbmp_gx16g, pbmp_valid);
            SOC_PBMP_CLEAR(pbmp_tmp);
            SOC_PBMP_WORD_SET(pbmp_tmp, 0, 0x001fffe0);
            SOC_PBMP_AND(pbmp_gx16g, pbmp_tmp);
            break;
        }
        /* Fall through */
    case BCM56823_DEVICE_ID:
        for (port = 1; port <= 6; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 19; port <= 24; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_ASSIGN(pbmp_gx21g, pbmp_valid);
        SOC_PBMP_CLEAR(pbmp_tmp);
        SOC_PBMP_WORD_SET(pbmp_tmp, 0, 0x0001fe00);
        SOC_PBMP_AND(pbmp_gx21g, pbmp_tmp);
        SOC_PBMP_ASSIGN(pbmp_gx16g, pbmp_valid);
        SOC_PBMP_CLEAR(pbmp_tmp);
        SOC_PBMP_WORD_SET(pbmp_tmp, 0, 0x00060180);
        SOC_PBMP_AND(pbmp_gx16g, pbmp_tmp);
        for (port = 9; port <= 16; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        SOC_PBMP_AND(pbmp_hyplite, pbmp_valid);
        break;
    case BCM56825_DEVICE_ID:
        for (port = 26; port <= 28; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_ASSIGN(pbmp_gx21g, pbmp_valid);
        SOC_PBMP_CLEAR(pbmp_tmp);
        SOC_PBMP_WORD_SET(pbmp_tmp, 0, 0x0001fe00);
        SOC_PBMP_AND(pbmp_gx21g, pbmp_tmp);
        for (port = 9; port <= 16; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        SOC_PBMP_AND(pbmp_hyplite, pbmp_valid);
        break;
    case BCM56721_DEVICE_ID:
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 5);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 6);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 19);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 20);
        /* Fall through */
    case BCM56720_DEVICE_ID:
        for (port = 1; port <= 4; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 21; port <= 28; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_ASSIGN(pbmp_gx16g, pbmp_valid);
        SOC_PBMP_CLEAR(pbmp_tmp);
        SOC_PBMP_WORD_SET(pbmp_tmp, 0, 0x001fffe0);
        SOC_PBMP_AND(pbmp_gx16g, pbmp_tmp);
        for (port = 9; port <= 16; port++) {
            SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
        }
        SOC_PBMP_AND(pbmp_hyplite, pbmp_valid);
        break;
#ifdef BCM_TRIDENT_SUPPORT
    case BCM56840_DEVICE_ID:
    case BCM56841_DEVICE_ID:
    case BCM56843_DEVICE_ID:
    case BCM56845_DEVICE_ID:
    case BCM56847_DEVICE_ID:
    case BCM56743_DEVICE_ID:
    case BCM56745_DEVICE_ID:
    case BCM56842_DEVICE_ID:
    case BCM56844_DEVICE_ID:
    case BCM56846_DEVICE_ID:
    case BCM56549_DEVICE_ID:
    case BCM56053_DEVICE_ID:
    case BCM56831_DEVICE_ID:
    case BCM56835_DEVICE_ID:
    case BCM56838_DEVICE_ID:
    case BCM56849_DEVICE_ID:
    case BCM56742_DEVICE_ID:
    case BCM56744_DEVICE_ID:
    case BCM56746_DEVICE_ID:
        SOC_FEATURE_SET(unit, soc_feature_logical_port_num);
        rv = soc_trident_port_config_init(unit, dev_id);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Port config error !!\n")));
            return SOC_E_CONFIG;
        }
        for (port = 66; port <= 73; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        si->cpu_hg_index = 66;
        break;
#endif /* BCM_TRIDENT_SUPPORT */

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
    case BCM56860_DEVICE_ID:
    case BCM56850_DEVICE_ID:
    case BCM56851_DEVICE_ID:
    case BCM56852_DEVICE_ID:
    case BCM56853_DEVICE_ID:
    case BCM56854_DEVICE_ID:
    case BCM56855_DEVICE_ID:
    case BCM56834_DEVICE_ID:
    case BCM56750_DEVICE_ID:
    case BCM56830_DEVICE_ID:
    case BCM56861_DEVICE_ID:
    case BCM56862_DEVICE_ID:
    case BCM56864_DEVICE_ID:
    case BCM56865_DEVICE_ID:
    case BCM56866_DEVICE_ID:
    case BCM56867_DEVICE_ID:
    case BCM56868_DEVICE_ID:
    case BCM56832_DEVICE_ID:
    case BCM56833_DEVICE_ID:
    case BCM56836_DEVICE_ID:
        SOC_FEATURE_SET(unit, soc_feature_logical_port_num);
        if (rev_id >= BCM56850_A1_REV_ID) {
            SOC_FEATURE_SET(unit, soc_feature_cmic_reserved_queues);
        }
        rv = soc_trident2_port_config_init(unit, dev_id, rev_id);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Port config error !!\n")));
            return SOC_E_CONFIG;
        }
        for (port = 108; port <= 129; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        si->cpu_hg_index = 106;
        if (dev_id == BCM56830_DEVICE_ID) {
            SOC_PBMP_ASSIGN(pbmp_xport_xe,pbmp_valid);
        }
        /* Flex counter pools */
        si->num_flex_ingress_pools = 8;
        si->num_flex_egress_pools = 4;
        si->size_flex_ingress_pool = 4096;
        si->size_flex_egress_pool = 4096;

#ifdef BCM_TRIDENT2PLUS_SUPPORT
        /* Initialize subport related pbmp and subtag_pbm */
        if (soc_feature(unit, soc_feature_hgproxy_subtag_coe)
            && soc_feature(unit, soc_feature_egr_lport_tab_profile)) {
            soc_td2p_subport_init(unit);
        }

        if (soc_feature(unit, soc_feature_multi_next_hops_on_port)) {
            si->num_modules = TD2P_MAX_COE_MODULES;
        }
#endif
        break;
#endif /* defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT) */

#ifdef BCM_TOMAHAWK_SUPPORT
    case BCM56960_DEVICE_ID:
    case BCM56961_DEVICE_ID:
    case BCM56962_DEVICE_ID:
    case BCM56963_DEVICE_ID:
    case BCM56930_DEVICE_ID:
    case BCM56968_DEVICE_ID:
#if defined(BCM_TOMAHAWKPLUS_SUPPORT)
    case BCM56965_DEVICE_ID:
    case BCM56969_DEVICE_ID:
    case BCM56966_DEVICE_ID:
    case BCM56967_DEVICE_ID:
#endif /* BCM_TOMAHAWKPLUS_SUPPORT */
    case BCM56168_DEVICE_ID:
    case BCM56169_DEVICE_ID:
        SOC_FEATURE_SET(unit, soc_feature_logical_port_num);
        SOC_FEATURE_SET(unit, soc_feature_sbus_format_v4);
        rv = soc_tomahawk_port_config_init(unit, dev_id);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Port config error !!")));
            return rv;
        }
        si->cpu_hg_index = 136;
        /* Flex counter pools */
        si->num_flex_ingress_pools = 20;
        si->num_flex_egress_pools = 4;
        si->size_flex_ingress_pool = 4096;
        si->size_flex_egress_pool = 4096;
        break;
#endif /* BCM_TOMAHAWK_SUPPORT */

#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
    case BCM56970_DEVICE_ID:
    case BCM56971_DEVICE_ID:
    case BCM56972_DEVICE_ID:
    case BCM56974_DEVICE_ID:
    case BCM56975_DEVICE_ID:
    case BCM56980_DEVICE_ID:
    case BCM56981_DEVICE_ID:
    case BCM56982_DEVICE_ID:
    case BCM56983_DEVICE_ID:
    case BCM56984_DEVICE_ID:
        SOC_FEATURE_SET(unit, soc_feature_logical_port_num);
        SOC_FEATURE_SET(unit, soc_feature_sbus_format_v4);
        if (SOC_IS_TOMAHAWK2(unit)) {
        rv = soc_tomahawk2_port_config_init(unit, dev_id);
        } else {
#ifdef BCM_TOMAHAWK3_SUPPORT
            rv = soc_tomahawk3_port_config_init(unit, dev_id);
#endif /* BCM_TOMAHAWK3_SUPPORT */
        }

        if (rv < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Port config error !!")));
            return rv;
        }

        /* No HiGig on TH3 */
        if (SOC_IS_TOMAHAWK3(unit)) {
            si->cpu_hg_index = -1;
        } else {
            si->cpu_hg_index = 136;
        }

        /* Flex counter pools */
        si->num_flex_ingress_pools = 20;
        si->num_flex_egress_pools = 4;
        if (SOC_IS_TOMAHAWK3(unit)) {
            si->size_flex_ingress_pool = 4096;
        } else {
            si->size_flex_ingress_pool = 8192;
        }
        si->size_flex_egress_pool = 4096;
        /* warmboot: rely on recovery info if flexport enabled */
        SOC_IF_ERROR_RETURN(soc_th_latency_get(unit, &latency[unit]));
        if ((SOC_WARM_BOOT(unit) || SOC_FAST_REBOOT(unit)) && si->flex_eligible) {
            SOC_PBMP_CLEAR(pbmp_xport_xe);
            for (port = 0; port < SOC_MAX_NUM_PORTS; port ++) {
                if ((si->port_l2p_mapping[port] > 0) &&
                    (!IS_HG_PORT(unit, port))) {
                    SOC_PBMP_PORT_ADD(pbmp_xport_xe, port);
                }
            }
        }
        break;
#endif /* BCM_TOMAHAWK2_SUPPORT */

#ifdef BCM_TRIDENT3_SUPPORT
    case BCM56870_DEVICE_ID:
    case BCM56873_DEVICE_ID:
        SOC_FEATURE_SET(unit, soc_feature_logical_port_num);
        SOC_FEATURE_SET(unit, soc_feature_sbus_format_v4);
        rv = soc_trident3_port_config_init(unit, dev_id);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Port config error !!")));
            return rv;
        }
        si->cpu_hg_index = 132;
        /* Flex counter pools */
        si->num_flex_ingress_pools = 20;
        si->num_flex_egress_pools = 4;
        si->size_flex_ingress_pool = 8192;
        si->size_flex_egress_pool = 8192;
        si->sflow_range_max = 28;
        /* Initialize subport related pbmp and subtag_pbm */
        if (soc_feature(unit, soc_feature_hgproxy_subtag_coe)) {
            soc_td3_subport_init(unit);
        }
        if (soc_feature(unit, soc_feature_pstats)) {
            soc_trident3_mmu_pstats_tbl_config(unit);
        }
        if (SOC_WARM_BOOT(unit) && si->flex_eligible) {
            SOC_PBMP_CLEAR(pbmp_xport_xe);
            for (port = 0; port < SOC_MAX_NUM_PORTS; port ++) {
                if ((si->port_l2p_mapping[port] > 0) &&
                    (!IS_HG_PORT(unit, port))) {
                    SOC_PBMP_PORT_ADD(pbmp_xport_xe, port);
                }
            }
        }
        if (soc_feature(unit, soc_feature_multi_next_hops_on_port)) {
            si->num_modules = TD3_MAX_COE_MODULES;
        }
        break;
#endif /* BCM_TRIDENT3_SUPPORT */

#ifdef BCM_MAVERICK2_SUPPORT
    case BCM56770_DEVICE_ID:
    case BCM56771_DEVICE_ID:
        SOC_FEATURE_SET(unit, soc_feature_logical_port_num);
        SOC_FEATURE_SET(unit, soc_feature_sbus_format_v4);
        rv = soc_maverick2_port_config_init(unit, dev_id);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Port config error !!")));
        }
        si->cpu_hg_index = 132;
        /* Flex counter pools */
        si->num_flex_ingress_pools = 20;
        si->num_flex_egress_pools = 4;
        si->size_flex_ingress_pool = 8192;
        si->size_flex_egress_pool = 8192;
        si->sflow_range_max = 28;
        /* Initialize subport related pbmp and subtag_pbm */
        if (soc_feature(unit, soc_feature_hgproxy_subtag_coe)) {
            soc_td3_subport_init(unit);
        }
        if (soc_feature(unit, soc_feature_pstats)) {
            soc_trident3_mmu_pstats_tbl_config(unit);
        }
        if (SOC_WARM_BOOT(unit) && si->flex_eligible) {
            SOC_PBMP_CLEAR(pbmp_xport_xe);
            for (port = 0; port < SOC_MAX_NUM_PORTS; port ++) {
                if ((si->port_l2p_mapping[port] > 0) &&
                    (!IS_HG_PORT(unit, port))) {
                    SOC_PBMP_PORT_ADD(pbmp_xport_xe, port);
                }
            }
        }

        if (soc_feature(unit, soc_feature_multi_next_hops_on_port)) {
            si->num_modules = TD3_MAX_COE_MODULES;
        }
        break;
#endif /* BCM_MAVERICK2_SUPPORT */

#ifdef BCM_SHADOW_SUPPORT
    case BCM88732_DEVICE_ID:

        if (soc_property_get(unit, spn_BCM88732_2X40_1X40, 0)) {
            /* 2x40G port-link + 1x40G interlaken */
            SOC_PBMP_PORT_ADD(pbmp_xl, 1);
            SOC_PBMP_PORT_ADD(pbmp_xl40g, 1);
            SOC_PBMP_PORT_ADD(pbmp_xport_xe, 1);
            for (port = 2; port <= 4; port++) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }
            SOC_PBMP_PORT_ADD(pbmp_xl, 5);
            SOC_PBMP_PORT_ADD(pbmp_xl40g, 5);
            SOC_PBMP_PORT_ADD(pbmp_xport_xe, 5);
            for (port = 6; port <= 8; port++) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }
            SOC_PBMP_PORT_ADD(pbmp_xl, 9);
            SOC_PBMP_PORT_ADD(pbmp_xl40g, 9);
            SOC_PBMP_CLEAR(pbmp_tmp);
            for (port = 10; port <= 16; port++) {
                SOC_PBMP_PORT_ADD(pbmp_tmp, port);
                SOC_PBMP_PORT_ADD(pbmp_xl, port);
            }
            SOC_PBMP_ASSIGN(pbmp_disabled, pbmp_tmp);
        } else if (soc_property_get(unit, spn_BCM88732_2X40_2X40, 0)) {
            /* 2x40G port-link + 2x40G interlaken */
            SOC_PBMP_PORT_ADD(pbmp_xl, 1);
            SOC_PBMP_PORT_ADD(pbmp_xl40g, 1);
            SOC_PBMP_PORT_ADD(pbmp_xport_xe, 1);
            for (port = 2; port <= 4; port++) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }
            SOC_PBMP_PORT_ADD(pbmp_xl, 5);
            SOC_PBMP_PORT_ADD(pbmp_xl40g, 5);
            SOC_PBMP_PORT_ADD(pbmp_xport_xe, 5);
            SOC_PBMP_PORT_ADD(pbmp_xport_xe, 5);
            for (port = 6; port <= 8; port++) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }
            SOC_PBMP_PORT_ADD(pbmp_xl, 9);
            SOC_PBMP_PORT_ADD(pbmp_xl40g, 9);
            SOC_PBMP_CLEAR(pbmp_tmp);
            for (port = 10; port <= 12; port++) {
                SOC_PBMP_PORT_ADD(pbmp_tmp, port);
                SOC_PBMP_PORT_ADD(pbmp_xl, port);
            }
            SOC_PBMP_PORT_ADD(pbmp_xl, 13);
            SOC_PBMP_PORT_ADD(pbmp_xl40g, 13);
            for (port = 14; port <= 16; port++) {
                SOC_PBMP_PORT_ADD(pbmp_tmp, port);
                SOC_PBMP_PORT_ADD(pbmp_xl, port);
            }
            /* SOC_PBMP_ASSIGN(pbmp_disabled, pbmp_tmp); */
        } else if (soc_property_get(unit, spn_BCM88732_2X40_2X40E, 0)) {
            /* 2x40G port-link + 2x40G Ethernet */
            SOC_PBMP_PORT_ADD(pbmp_xl, 1);
            SOC_PBMP_PORT_ADD(pbmp_xl40g, 1);
            SOC_PBMP_PORT_ADD(pbmp_xport_xe, 1);
            for (port = 2; port <= 4; port++) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }
            SOC_PBMP_PORT_ADD(pbmp_xl, 5);
            SOC_PBMP_PORT_ADD(pbmp_xl40g, 5);
            SOC_PBMP_PORT_ADD(pbmp_xport_xe, 5);
            for (port = 6; port <= 8; port++) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }
            SOC_PBMP_PORT_ADD(pbmp_xl, 9);
            SOC_PBMP_PORT_ADD(pbmp_xl40g, 9);
            SOC_PBMP_PORT_ADD(pbmp_xport_xe, 9);
            for (port = 10; port <= 12; port++) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }
            SOC_PBMP_PORT_ADD(pbmp_xl, 13);
            SOC_PBMP_PORT_ADD(pbmp_xl40g, 13);
            SOC_PBMP_PORT_ADD(pbmp_xport_xe, 13);
            for (port = 14; port <= 16; port++) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }
        } else if (soc_property_get(unit, spn_BCM88732_2X40_8X10, 0)) {
            /* 2x40G port-link + 8x10G XFI */
            SOC_PBMP_PORT_ADD(pbmp_xl, 1);
            SOC_PBMP_PORT_ADD(pbmp_xl40g, 1);
            SOC_PBMP_PORT_ADD(pbmp_xport_xe, 1);

            /* Mark ports as disabled*/
            SOC_PBMP_CLEAR(pbmp_tmp);
            for (port = 2; port <= 4; port++) {
                SOC_PBMP_PORT_ADD(pbmp_tmp, port);
                SOC_PBMP_PORT_ADD(pbmp_xl, port);
                SOC_PBMP_PORT_ADD(pbmp_xl40g, 1);
            }
            for (port = 6; port <= 8; port++) {
                SOC_PBMP_PORT_ADD(pbmp_tmp, port);
                SOC_PBMP_PORT_ADD(pbmp_xl, port);
            }
            SOC_PBMP_ASSIGN(pbmp_disabled, pbmp_tmp);


#if 0
            for (port = 2; port <= 4; port++) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }
#endif
            SOC_PBMP_PORT_ADD(pbmp_xl, 5);
            SOC_PBMP_PORT_ADD(pbmp_xl40g, 5);
            SOC_PBMP_PORT_ADD(pbmp_xport_xe, 5);
#if 0
            for (port = 6; port <= 8; port++) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }
#endif
            for (port = 9; port <= 16; port++) {
                SOC_PBMP_PORT_ADD(pbmp_xport_xe, port);
                SOC_PBMP_PORT_ADD(pbmp_xl10g, port);
                SOC_PBMP_PORT_ADD(pbmp_xl, port);
            }
        } else if (soc_property_get(unit, spn_BCM88732_8X10_8X10, 0)) {
            /* 8x10G port-link + 8x10G XFI */
            for (port = 1; port <= 16; port++) {
                SOC_PBMP_PORT_ADD(pbmp_xport_xe, port);
                SOC_PBMP_PORT_ADD(pbmp_xl10g, port);
                SOC_PBMP_PORT_ADD(pbmp_xl, port);
            }
        } else if (soc_property_get(unit, spn_BCM88732_8X10_1X40, 0)) {
            /* 8x10G port-link + 1x40G interlaken */
            for (port = 1; port <= 8; port++) {
                SOC_PBMP_PORT_ADD(pbmp_xport_xe, port);
                SOC_PBMP_PORT_ADD(pbmp_xl10g, port);
                SOC_PBMP_PORT_ADD(pbmp_xl, port);
            }
            SOC_PBMP_PORT_ADD(pbmp_xl, 9);
            SOC_PBMP_PORT_ADD(pbmp_xl40g, 9);
            SOC_PBMP_CLEAR(pbmp_tmp);
            for (port = 10; port <= 16; port++) {
                SOC_PBMP_PORT_ADD(pbmp_tmp, port);
                SOC_PBMP_PORT_ADD(pbmp_xl, port);
            }
            SOC_PBMP_ASSIGN(pbmp_disabled, pbmp_tmp);
        } else if (soc_property_get(unit, spn_BCM88732_1X40_4X10, 0)) {
            /* TDM 18 */
            /* 1x40G port-link 4X10G XAUI Switch link*/
            SOC_PBMP_PORT_ADD(pbmp_xl, 1);
            SOC_PBMP_PORT_ADD(pbmp_xl40g, 1);
            SOC_PBMP_PORT_ADD(pbmp_xport_xe, 1);
            for (port = 2; port <= 8; port++) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }

            /* Switch Panel ports */
            for (port = 9; port < 16; port+=2) {
                SOC_PBMP_PORT_ADD(pbmp_xl, port);
                SOC_PBMP_PORT_ADD(pbmp_xport_xe, port);
                SOC_PBMP_PORT_ADD(pbmp_xl10g, port);
            }
            /* Remove the even ports from the valid ports */
            for (port = 10; port <= 16; port+=2) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }

        } else if (soc_property_get(unit, spn_BCM88732_4X10_4X10, 0)) {
            /* TDM 19 */
            for (port = 1; port <= 4; port++) {
                SOC_PBMP_PORT_ADD(pbmp_xport_xe, port);
                SOC_PBMP_PORT_ADD(pbmp_xl, port);
                SOC_PBMP_PORT_ADD(pbmp_xl10g, port);
            }
            for (port = 5; port <= 8; port++) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }

            /* Switch Panel ports */
            for (port = 9; port < 16; port+=2) {
                SOC_PBMP_PORT_ADD(pbmp_xl, port);
                SOC_PBMP_PORT_ADD(pbmp_xport_xe, port);
                SOC_PBMP_PORT_ADD(pbmp_xl10g, port);
            }
            /* Remove the even ports from the valid ports */
            for (port = 10; port <= 16; port+=2) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }
        } else {
            /* 8x10G port-link + 2x40G interlaken */
            for (port = 1; port <= 8; port++) {
                SOC_PBMP_PORT_ADD(pbmp_xport_xe, port);
                SOC_PBMP_PORT_ADD(pbmp_xl10g, port);
                SOC_PBMP_PORT_ADD(pbmp_xl, port);
            }
            SOC_PBMP_PORT_ADD(pbmp_xl, 9);
            SOC_PBMP_PORT_ADD(pbmp_xl40g, 9);
            SOC_PBMP_CLEAR(pbmp_tmp);
            for (port = 10; port <= 12; port++) {
                SOC_PBMP_PORT_ADD(pbmp_tmp, port);
                SOC_PBMP_PORT_ADD(pbmp_xl, port);
            }
            SOC_PBMP_PORT_ADD(pbmp_xl, 13);
            SOC_PBMP_PORT_ADD(pbmp_xl40g, 13);
            for (port = 14; port <= 16; port++) {
                SOC_PBMP_PORT_ADD(pbmp_tmp, port);
                SOC_PBMP_PORT_ADD(pbmp_xl, port);
            }
            /* SOC_PBMP_ASSIGN(pbmp_disabled, pbmp_tmp); */
        }
        break;
#endif /* BCM_SHADOW_SUPPORT */

#ifdef BCM_HELIX4_SUPPORT
    case BCM56548H_DEVICE_ID:
    case BCM56548_DEVICE_ID:
    case BCM56547_DEVICE_ID:
    case BCM56346_DEVICE_ID:
    case BCM56345_DEVICE_ID:
    case BCM56344_DEVICE_ID:
    case BCM56342_DEVICE_ID:
    case BCM56340_DEVICE_ID:
    case BCM56049_DEVICE_ID:
    case BCM56048_DEVICE_ID:
    case BCM56047_DEVICE_ID:
    case BCM56042_DEVICE_ID:
    case BCM56041_DEVICE_ID:
    case BCM56040_DEVICE_ID:

        SOC_FEATURE_SET(unit, soc_feature_logical_port_num);
        rv = soc_tr3_port_config_init(unit, dev_id);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Port config error !!\n")));
            return SOC_E_CONFIG;
        }
        for (port = 62; port <= 66; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 58; port < 62; port++) {
            SOC_PBMP_PORT_ADD(pbmp_valid, port);
            SOC_PBMP_PORT_ADD(si->lb_pbm, port);
        }
        if (BCM56548H_DEVICE_ID == dev_id) {
            for (port = 46; port <= 53; port++) {
                SOC_PBMP_PORT_REMOVE(pbmp_xport_xe, port);
            }
        }

        
        AXP_PORT(unit, SOC_AXP_NLF_PASSTHRU) = si->lb_port = 61;
        AXP_PORT(unit, SOC_AXP_NLF_SM) = 60;
        AXP_PORT(unit, SOC_AXP_NLF_WLAN_ENCAP) = 58;
        AXP_PORT(unit, SOC_AXP_NLF_WLAN_DECAP) = 59;
        si->cpu_hg_index = 63;
        if (dev_id == BCM56346_DEVICE_ID ||
                dev_id == BCM56345_DEVICE_ID) {
            si->num_flex_ingress_pools = 0;
            si->num_flex_egress_pools = 0;
            si->size_flex_ingress_pool = 0;
            si->size_flex_egress_pool = 0;
            si->global_meter_action_size = 0;
        } else {
            si->num_flex_ingress_pools = 8;
            si->num_flex_egress_pools = 8;
            si->size_flex_ingress_pool = 1024;
            si->size_flex_egress_pool = 1024;
            si->global_meter_action_size = 8192;
        }
        break;
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
    case BCM56640_DEVICE_ID:
    case BCM56643_DEVICE_ID:
    case BCM56644_DEVICE_ID:
    case BCM56648_DEVICE_ID:
    case BCM56649_DEVICE_ID:
    case BCM56540_DEVICE_ID:
    case BCM56541_DEVICE_ID:
    case BCM56542_DEVICE_ID:
    case BCM56543_DEVICE_ID:
    case BCM56544_DEVICE_ID:
    case BCM56545_DEVICE_ID:
    case BCM56546_DEVICE_ID:
    case BCM56044_DEVICE_ID:
    case BCM56045_DEVICE_ID:
    case BCM56046_DEVICE_ID:
        SOC_FEATURE_SET(unit, soc_feature_logical_port_num);
        rv = soc_tr3_port_config_init(unit, dev_id);
        SOC_PBMP_CLEAR(pbmp_xport_xe);
        for (port = 0; port <= 57; port++) {
            if ((10000 == si->port_speed_max[port]) || (100000 == si->port_speed_max[port])) {
                SOC_PBMP_PORT_ADD(pbmp_xport_xe, port);
            }
        }
        pbmp_xport_xe = soc_property_get_pbmp_default(unit,
                                                      spn_PBMP_XPORT_XE,
                                                      pbmp_xport_xe);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Port config error !!\n")));
            return SOC_E_CONFIG;
        }
        for (port = 62; port <= 66; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 58; port < 62; port++) {
            SOC_PBMP_PORT_ADD(si->lb_pbm, port);
            SOC_PBMP_PORT_ADD(pbmp_valid, port);
        }
        
        AXP_PORT(unit, SOC_AXP_NLF_PASSTHRU) = si->lb_port = 61;
        AXP_PORT(unit, SOC_AXP_NLF_SM) = 60;
        AXP_PORT(unit, SOC_AXP_NLF_WLAN_ENCAP) = 58;
        AXP_PORT(unit, SOC_AXP_NLF_WLAN_DECAP) = 59;
        si->cpu_hg_index = 63;
        si->num_flex_ingress_pools = 16;
        si->size_flex_ingress_pool = 8192;
        si->num_flex_egress_pools = 8;
        si->size_flex_egress_pool = 2048;
        if (dev_id == BCM56540_DEVICE_ID ||
            dev_id == BCM56541_DEVICE_ID ||
            dev_id == BCM56542_DEVICE_ID ||
            dev_id == BCM56543_DEVICE_ID ||
            dev_id == BCM56544_DEVICE_ID ||
            dev_id == BCM56545_DEVICE_ID ||
            dev_id == BCM56546_DEVICE_ID ||
            dev_id == BCM56044_DEVICE_ID ||
            dev_id == BCM56045_DEVICE_ID ||
            dev_id == BCM56046_DEVICE_ID) {
            si->num_flex_ingress_pools = 8;
            si->size_flex_ingress_pool = 2048;
            if (dev_id == BCM56044_DEVICE_ID ||
                dev_id == BCM56045_DEVICE_ID ||
                dev_id == BCM56046_DEVICE_ID) {
                si->size_flex_egress_pool = 64;
                si->size_flex_ingress_pool = 256;
            }
            if (dev_id == BCM56543_DEVICE_ID) {
                si->size_flex_ingress_pool = 4096;
            }
        }
        break;
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
    case BCM56440_DEVICE_ID:
    case BCM55440_DEVICE_ID:
    case BCM55441_DEVICE_ID:
    case BCM56445_DEVICE_ID:
        SOC_PBMP_CLEAR(pbmp_gport_stack);
        SOC_PBMP_CLEAR(pbmp_mxq13g);
        SOC_PBMP_CLEAR(pbmp_mxq2p5g);
        SOC_PBMP_CLEAR(pbmp_tmp);
        switch (soc_property_get(unit, spn_BCM5644X_CONFIG, 0)) {
            case 1: /* 24x1G + 2xHG + 8x2.5GE */
                for (port = 25; port <= 26; port++) {
                    SOC_PBMP_PORT_ADD(pbmp_mxq13g, port);
                    SOC_PBMP_PORT_ADD(pbmp_tmp, port);
                }
                for (port = 27; port <= 34; port++) {
                    SOC_PBMP_PORT_ADD(pbmp_mxq2p5g, port);
                    SOC_PBMP_PORT_ADD(pbmp_tmp, port);
                }
                SOC_PBMP_ASSIGN(pbmp_mxq, pbmp_tmp);
                break;

            case 2: /* 24x1G + 4x2.5HGL */
                for (port = 25; port <= 28; port++) {
                    SOC_PBMP_PORT_ADD(pbmp_mxq2p5g, port);
                    SOC_PBMP_PORT_ADD(pbmp_mxq, port);
                    SOC_PBMP_PORT_ADD(pbmp_gport_stack, port);
                }
                for (port = 29; port <= 34; port++) {
                    SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
                }
                break;
            case 3: /* 24x1G + 3xHG + 4x2.5GE */
                for (port = 25; port <= 27; port++) {
                    SOC_PBMP_PORT_ADD(pbmp_mxq13g, port);
                    SOC_PBMP_PORT_ADD(pbmp_mxq, port);
                }
                for (port = 28; port <= 31; port++) {
                    SOC_PBMP_PORT_ADD(pbmp_mxq2p5g, port);
                    SOC_PBMP_PORT_ADD(pbmp_mxq, port);
                }
                for (port = 32; port <= 34; port++) {
                    SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
                }
                break;

            case 4: /* 24x1G + 2xHG + 4x2.5GE + 1xHG*/
                for (port = 25; port <= 26; port++) {
                    SOC_PBMP_PORT_ADD(pbmp_mxq13g, port);
                    SOC_PBMP_PORT_ADD(pbmp_mxq, port);
                }
                SOC_PBMP_PORT_ADD(pbmp_mxq13g, 28);
                SOC_PBMP_PORT_ADD(pbmp_mxq, 28);
                for (port = 29; port <= 31; port++) {
                    SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
                }
                SOC_PBMP_PORT_ADD(pbmp_mxq2p5g, 27);
                SOC_PBMP_PORT_ADD(pbmp_mxq, 27);
                for (port = 32; port <= 34; port++) {
                    SOC_PBMP_PORT_ADD(pbmp_mxq2p5g, port);
                    SOC_PBMP_PORT_ADD(pbmp_mxq, port);
                }
                break;


            case 0: /* 24x1G + 4xHG */
            default:
                for (port = 25; port <= 28; port++) {
                    SOC_PBMP_PORT_ADD(pbmp_mxq13g, port);
                    SOC_PBMP_PORT_ADD(pbmp_mxq, port);
                    SOC_PBMP_PORT_ADD(pbmp_gport_stack, port);
                }
                for (port = 29; port <= 34; port++) {
                    SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
                }
                break;
        }
        for (port = 36; port <= 38; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        if (drv_dev_id == BCM55440_DEVICE_ID) {
            /* Only 17 GE ports */
            for (port = 18; port <= 24; port++) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }
        }
        SOC_PBMP_CLEAR(si->tdm_pbm);
        SOC_PBMP_CLEAR(si->lb_pbm);
        SOC_PBMP_PORT_ADD(si->lb_pbm, 35);
        si->lb_port = 35;
        si->cpu_hg_index = 39;
        break;
    case BCM56441_DEVICE_ID:
    case BCM56446_DEVICE_ID:
        /* 8x1G + 2xHG */
        SOC_PBMP_CLEAR(pbmp_gport_stack);
        SOC_PBMP_CLEAR(pbmp_hyplite);
        SOC_PBMP_CLEAR(pbmp_mxq13g);
        SOC_PBMP_CLEAR(pbmp_mxq2p5g);
        SOC_PBMP_CLEAR(pbmp_mxq);

        /*
         * For 56441/2/3 devices port 1 is always included. The OTP settings
         * are checked in _soc_katana_misc_init and if CES is not enabled then
         * the port is removed there.
         */
        switch (soc_property_get(unit, spn_BCM5644X_CONFIG, 0)) {
            case 2: /* 4x1G + 4x2.5HGL + 1xHG */
                if (dev_id == BCM56441_DEVICE_ID) {
                    /* Port 0 is used for CES */
                    for (port = 2; port <= 8; port++) {
                        SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
                    }
                } else if (dev_id == BCM56446_DEVICE_ID) {
                    for (port = 5; port <= 12; port++) {
                        SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
                    }
                }
                for (port = 13; port <= 26; port++) {
                    SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
                }
                SOC_PBMP_PORT_ADD(pbmp_mxq13g, 27);
                SOC_PBMP_PORT_ADD(pbmp_mxq, 27);
                SOC_PBMP_PORT_ADD(pbmp_gport_stack, 27);
                for (port = 28; port <= 31; port++) {
                     SOC_PBMP_PORT_ADD(pbmp_mxq2p5g, port);
                     SOC_PBMP_PORT_ADD(pbmp_mxq, port);
                     SOC_PBMP_PORT_ADD(pbmp_gport_stack, port);
                }
                for (port = 32; port <= 34; port++) {
                    SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
                }
                break;
            case 1:
                /* (Frontpanel)8xSGMII + (BackPanel) 4*2.5HGL + 4*2.5GE */
                /* 56441 : GPORT1 - 9 to 16 */
                /* 56446 : GPORT0 - 1 to 8  */
                if (dev_id == BCM56441_DEVICE_ID) {
                /* Port 0 is used for CES */
                for (port = 2; port <= 8; port++) {
                         SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
                    }
                } else if (dev_id == BCM56446_DEVICE_ID) {
                    for (port = 9; port <= 16; port++) {
                         SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
                    }
                }
                for (port = 17; port <= 26; port++) {
                     SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
                }
                /* MXQPORT2 - 27,32,33,34 */
                for (kt_mxqport_index = 0 ;
                     kt_mxqport_index < 4 ;
                     kt_mxqport_index++) {
                     port = kt_mxqport2[kt_mxqport_index]; /* 27,32,33,34 */
                     SOC_PBMP_PORT_ADD(pbmp_mxq2p5g, port);
                     SOC_PBMP_PORT_ADD(pbmp_mxq, port);
                }
                /*
                    MXQPORT3 - 28,29,30,31
                 */
                for (port = 28; port <= 31; port++) {
                     SOC_PBMP_PORT_ADD(pbmp_mxq2p5g, port);
                     SOC_PBMP_PORT_ADD(pbmp_mxq, port);
                     SOC_PBMP_PORT_ADD(pbmp_gport_stack, port);
                }
                break;
            case 0: /* 8x1G + 2xHG */
            default:
                /* (Frontpanel)8xSGMII + (BackPanel)2HG[13] */
                /* 56441 : GPORT1 - 9 to 16 */
                /* 56446 : GPORT0 - 1 to 8 */

                if (dev_id == BCM56441_DEVICE_ID) {
                /* Port 0 is used for CES */
                for (port = 2; port <= 8; port++) {
                         SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
                    }
                } else if (dev_id == BCM56446_DEVICE_ID) {
                    for (port = 9; port <= 16; port++) {
                         SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
                    }
                }
                for (port = 17; port <= 26; port++) {
                     SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
                }
                /*
                    MXQPORT2 - 27
                    MXQPORT3 - 28
                */
                for (port = 27; port <= 28; port++) {
                    SOC_PBMP_PORT_ADD(pbmp_mxq13g, port);
                    SOC_PBMP_PORT_ADD(pbmp_mxq, port);
                    SOC_PBMP_PORT_ADD(pbmp_gport_stack, port);
                }
                for (port = 29; port <= 34; port++) {
                    SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
                }
                break;
        }
        for (port = 36; port <= 38; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(si->tdm_pbm);
        SOC_PBMP_CLEAR(si->lb_pbm);
        SOC_PBMP_PORT_ADD(si->lb_pbm, 35);
        si->lb_port = 35;
        si->cpu_hg_index = 39;
        break;
    case BCM56442_DEVICE_ID:
    case BCM56447_DEVICE_ID:
        /* 16x1G */
        SOC_PBMP_CLEAR(pbmp_gport_stack);
        if (dev_id == BCM56442_DEVICE_ID) {
            for (port = 1; port <= 8; port++) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }
        } else if (dev_id == BCM56447_DEVICE_ID) {
            for (port = 17; port <= 24; port++) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }
        }
        for (port = 25; port <= 34; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 36; port <= 38; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(si->lb_pbm);
        SOC_PBMP_PORT_ADD(si->lb_pbm, 35);
        si->lb_port = 35;
        si->cpu_hg_index = 39;
        break;
    case BCM56443_DEVICE_ID:
        /* 8x1G + 2xHG + 8x2.5G*/
        SOC_PBMP_CLEAR(pbmp_gport_stack);
        SOC_PBMP_CLEAR(pbmp_hyplite);
        SOC_PBMP_CLEAR(pbmp_mxq13g);
        SOC_PBMP_CLEAR(pbmp_mxq2p5g);
        SOC_PBMP_CLEAR(pbmp_mxq);
        for (port = 1; port <= 8; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 17; port <= 24; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        switch (soc_property_get(unit, spn_BCM5644X_CONFIG, 0)) {
            case 0: /* 8x1G + 2xHG + 8x2.5G */
                for (port = 25; port <= 26; port++) {
                    SOC_PBMP_PORT_ADD(pbmp_mxq13g, port);
                    SOC_PBMP_PORT_ADD(pbmp_mxq, port);
                }
                for (port = 27; port <= 34; port++) {
                    SOC_PBMP_PORT_ADD(pbmp_mxq2p5g, port);
                    SOC_PBMP_PORT_ADD(pbmp_mxq, port);
                }
                break;
            case 1: /* 8x1G + 2x2.5HGL + 8x2.5G */
                for (port = 25; port <= 26; port++) {
                    SOC_PBMP_PORT_ADD(pbmp_mxq2p5g, port);
                    SOC_PBMP_PORT_ADD(pbmp_mxq, port);
                    SOC_PBMP_PORT_ADD(pbmp_gport_stack, port);
                    SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
                }
                for (port = 27; port <= 34; port++) {
                    SOC_PBMP_PORT_ADD(pbmp_mxq2p5g, port);
                    SOC_PBMP_PORT_ADD(pbmp_mxq, port);
                }
                break;
            default:
                break;
        }
        for (port = 36; port <= 38; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(si->lb_pbm);
        SOC_PBMP_PORT_ADD(si->lb_pbm, 35);
        si->lb_port = 35;
        si->cpu_hg_index = 39;
        break;
    case BCM56449_DEVICE_ID:
        /* 2xHG + 2x2.5HGL */
        SOC_PBMP_CLEAR(pbmp_gport_stack);
        SOC_PBMP_CLEAR(pbmp_mxq13g);
        SOC_PBMP_CLEAR(pbmp_mxq2p5g);
        SOC_PBMP_CLEAR(pbmp_tmp);
        SOC_PBMP_CLEAR(pbmp_hyplite);

        for (port = 1; port <= 24; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        switch (soc_property_get(unit, spn_BCM5644X_CONFIG, 0)) {
            case 0: /* 2xHG + 2x2.5HGL */
                for (port = 27; port <= 28; port++) {
                    SOC_PBMP_PORT_ADD(pbmp_mxq2p5g, port);
                    SOC_PBMP_PORT_ADD(pbmp_mxq, port);
                    SOC_PBMP_PORT_ADD(pbmp_gport_stack, port);
                    SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
                }
                for (port = 25; port <= 26; port++) {
                    SOC_PBMP_PORT_ADD(pbmp_mxq13g, port);
                    SOC_PBMP_PORT_ADD(pbmp_mxq, port);
                }
                for (port = 29; port <= 34; port++) {
                    SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
                }
                break;
            case 1: /* 4x2.5HGL */
                for (port = 25; port <= 28; port++) {
                    SOC_PBMP_PORT_ADD(pbmp_mxq2p5g, port);
                    SOC_PBMP_PORT_ADD(pbmp_mxq, port);
                    SOC_PBMP_PORT_ADD(pbmp_gport_stack, port);
                    SOC_PBMP_PORT_ADD(pbmp_hyplite, port);
                }
                for (port = 29; port <= 34; port++) {
                    SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
                }
                break;
           default:
                break;
        }
        for (port = 36; port <= 38; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(si->tdm_pbm);
        SOC_PBMP_CLEAR(si->lb_pbm);
        SOC_PBMP_PORT_ADD(si->lb_pbm, 35);
        si->lb_port = 35;
        si->cpu_hg_index = 39;
        break;
    case BCM56448_DEVICE_ID:
        /* 24x1G + 4x2.5HGL */
        SOC_PBMP_CLEAR(pbmp_gport_stack);
        SOC_PBMP_CLEAR(pbmp_mxq13g);
        SOC_PBMP_CLEAR(pbmp_mxq2p5g);
        SOC_PBMP_CLEAR(pbmp_tmp);
        for (port = 25; port <= 28; port++) {
            SOC_PBMP_PORT_ADD(pbmp_mxq2p5g, port);
            SOC_PBMP_PORT_ADD(pbmp_mxq, port);
            SOC_PBMP_PORT_ADD(pbmp_gport_stack, port);
        }
        for (port = 29; port <= 34; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 36; port <= 38; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(si->tdm_pbm);
        SOC_PBMP_CLEAR(si->lb_pbm);
        SOC_PBMP_PORT_ADD(si->lb_pbm, 35);
        si->lb_port = 35;
        si->cpu_hg_index = 39;
        break;
    case BCM56241_DEVICE_ID:
        /* 6xGE + 2x2.5G*/
        SOC_PBMP_CLEAR(pbmp_gport_stack);
        SOC_PBMP_CLEAR(pbmp_hyplite);
        SOC_PBMP_CLEAR(pbmp_mxq13g);
        SOC_PBMP_CLEAR(pbmp_mxq2p5g);
        SOC_PBMP_CLEAR(pbmp_mxq);
        for (port = 1; port <= 26; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }

        for (port = 27; port <= 34; port++) {
            SOC_PBMP_PORT_ADD(pbmp_mxq2p5g, port);
            SOC_PBMP_PORT_ADD(pbmp_mxq, port);
            if ((port >= 30) && (port <= 31)) {
                si->port_speed_max[port] = 2500;
            } else {
                si->port_speed_max[port] =1000;
            }
        }

        for (port = 36; port <= 38; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(si->lb_pbm);
        SOC_PBMP_PORT_ADD(si->lb_pbm, 35);
        si->lb_port = 35;
        si->cpu_hg_index = 39;
        break;
    case BCM56240_DEVICE_ID:
    case BCM56245_DEVICE_ID:
        SOC_PBMP_CLEAR(pbmp_gport_stack);
        SOC_PBMP_CLEAR(pbmp_hyplite);
        SOC_PBMP_CLEAR(pbmp_mxq13g);
        SOC_PBMP_CLEAR(pbmp_mxq10g);
        SOC_PBMP_CLEAR(pbmp_mxq2p5g);
        SOC_PBMP_CLEAR(pbmp_mxq);
        for (port = 1; port <= 24; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }

        switch (soc_property_get(unit, spn_BCM5644X_CONFIG, 0)) {
            case 0: /* 2x10G + 2xHG[13] */
                SOC_PBMP_PORT_ADD(pbmp_mxq13g, 25);
                SOC_PBMP_PORT_ADD(pbmp_mxq13g, 26);
                SOC_PBMP_PORT_ADD(pbmp_mxq10g, 27);
                SOC_PBMP_PORT_ADD(pbmp_mxq10g, 28);
                for (port = 25; port <= 28; port++) {
                    SOC_PBMP_PORT_ADD(pbmp_mxq, port);
                }
                for (port = 29; port <= 34; port++) {
                    SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
                }
                break;
            case 1: /* 8x2.5G + 2xHG[13] */
                SOC_PBMP_PORT_ADD(pbmp_mxq13g, 25);
                SOC_PBMP_PORT_ADD(pbmp_mxq, 25);
                SOC_PBMP_PORT_ADD(pbmp_mxq13g, 26);
                SOC_PBMP_PORT_ADD(pbmp_mxq, 26);
                for (port = 27; port <= 34; port++) {
                    SOC_PBMP_PORT_ADD(pbmp_mxq2p5g, port);
                    SOC_PBMP_PORT_ADD(pbmp_mxq, port);
                }
                break;
            default:
                break;
        }
        for (port = 36; port <= 38; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(si->lb_pbm);
        SOC_PBMP_PORT_ADD(si->lb_pbm, 35);
        si->lb_port = 35;
        si->cpu_hg_index = 39;
        break;
    case BCM56242_DEVICE_ID:
    case BCM56246_DEVICE_ID:
        /* 10x2.5G */
        SOC_PBMP_CLEAR(pbmp_gport_stack);
        SOC_PBMP_CLEAR(pbmp_hyplite);
        SOC_PBMP_CLEAR(pbmp_mxq13g);
        SOC_PBMP_CLEAR(pbmp_mxq10g);
        SOC_PBMP_CLEAR(pbmp_mxq2p5g);
        SOC_PBMP_CLEAR(pbmp_mxq);
        for (port = 1; port <= 24; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 25; port <= 34; port++) {
            SOC_PBMP_PORT_ADD(pbmp_mxq2p5g, port);
            SOC_PBMP_PORT_ADD(pbmp_mxq, port);
        }
        for (port = 36; port <= 38; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(si->lb_pbm);
        SOC_PBMP_PORT_ADD(si->lb_pbm, 35);
        si->lb_port = 35;
        si->cpu_hg_index = 39;
        break;
    case BCM56243_DEVICE_ID:
        /* 4x2.5G */
        SOC_PBMP_CLEAR(pbmp_gport_stack);
        SOC_PBMP_CLEAR(pbmp_hyplite);
        SOC_PBMP_CLEAR(pbmp_mxq13g);
        SOC_PBMP_CLEAR(pbmp_mxq10g);
        SOC_PBMP_CLEAR(pbmp_mxq2p5g);
        SOC_PBMP_CLEAR(pbmp_mxq);
        for (port = 1; port <= 27; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 28; port <= 31; port++) {
            SOC_PBMP_PORT_ADD(pbmp_mxq2p5g, port);
            SOC_PBMP_PORT_ADD(pbmp_mxq, port);
        }
        for (port = 32; port <= 34; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        for (port = 36; port <= 38; port++) {
            SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
        }
        SOC_PBMP_CLEAR(si->lb_pbm);
        SOC_PBMP_PORT_ADD(si->lb_pbm, 35);
        si->lb_port = 35;
        si->cpu_hg_index = 39;
        break;
#endif /* BCM_KATANA_SUPPORT */

#ifdef BCM_KATANA2_SUPPORT
    case BCM55450_DEVICE_ID:
    case BCM55455_DEVICE_ID:
    case BCM56248_DEVICE_ID:
    case BCM56450_DEVICE_ID:
    case BCM56452_DEVICE_ID:
    case BCM56454_DEVICE_ID:
    case BCM56455_DEVICE_ID:
    case BCM56456_DEVICE_ID:
    case BCM56457_DEVICE_ID:
    case BCM56458_DEVICE_ID:
        /* Init kt2_pbmp structure with local variable's addresses */
        kt2_pbmp.pbmp_gport_stack=&pbmp_gport_stack;
        kt2_pbmp.pbmp_mxq=&pbmp_mxq;
        kt2_pbmp.pbmp_mxq1g=&pbmp_mxq1g;
        kt2_pbmp.pbmp_mxq2p5g=&pbmp_mxq2p5g;
        kt2_pbmp.pbmp_mxq10g=&pbmp_mxq10g;
        kt2_pbmp.pbmp_mxq13g=&pbmp_mxq13g;
        kt2_pbmp.pbmp_mxq21g=&pbmp_mxq21g;
        kt2_pbmp.pbmp_xport_xe=&pbmp_xport_xe;
        kt2_pbmp.pbmp_valid=&pbmp_valid;
        kt2_pbmp.pbmp_pp=&pbmp_pp;
        kt2_pbmp.pbmp_linkphy=&(si->linkphy_pbm);
        /* Init KT2 related pbmp now */
        soc_katana2_pbmp_init(unit,kt2_pbmp);

        SOC_PBMP_CLEAR(si->tdm_pbm);
        SOC_PBMP_CLEAR(si->lb_pbm);
        SOC_PBMP_PORT_ADD(si->lb_pbm, 41);
        si->lb_port = 41;
        si->cpu_hg_index = 42;
        si->cpu_hg_pp_port_index = 170;
        /* Initialize subport related pbmp linkphy_pbm and subtag_pbm */
        soc_katana2_subport_init(unit);

        break;
#endif /* BCM_KATANA2_SUPPORT */

#ifdef BCM_SABER2_SUPPORT
    case BCM56260_DEVICE_ID:
    case BCM56261_DEVICE_ID:
    case BCM56262_DEVICE_ID:
    case BCM56263_DEVICE_ID:
    case BCM56265_DEVICE_ID:
    case BCM56266_DEVICE_ID:
    case BCM56267_DEVICE_ID:
    case BCM56268_DEVICE_ID:
    case BCM56460_DEVICE_ID:
    case BCM56461_DEVICE_ID:
    case BCM56462_DEVICE_ID:
    case BCM56463_DEVICE_ID:
    case BCM56465_DEVICE_ID:
    case BCM56466_DEVICE_ID:
    case BCM56467_DEVICE_ID:
    case BCM56468_DEVICE_ID:
    case BCM56233_DEVICE_ID:
        sb2_pbmp.pbmp_gport_stack=&pbmp_gport_stack;
        sb2_pbmp.pbmp_mxq=&pbmp_mxq;
        sb2_pbmp.pbmp_mxq1g=&pbmp_mxq1g;
        sb2_pbmp.pbmp_mxq2p5g=&pbmp_mxq2p5g;
        sb2_pbmp.pbmp_mxq10g=&pbmp_mxq10g;
        sb2_pbmp.pbmp_xl=&pbmp_xl;
        sb2_pbmp.pbmp_xl1g=&pbmp_xl1g;
        sb2_pbmp.pbmp_xl2p5g=&pbmp_xl2p5g;
        sb2_pbmp.pbmp_xl10g=&pbmp_xl10g;
        sb2_pbmp.pbmp_xport_ge=&pbmp_xport_ge;
        sb2_pbmp.pbmp_xport_xe=&pbmp_xport_xe;
        sb2_pbmp.pbmp_valid=&pbmp_valid;
        sb2_pbmp.pbmp_pp=&pbmp_pp;
        sb2_pbmp.pbmp_linkphy=&(si->linkphy_pbm);

        /* Init SB2 related pbmp now */
        soc_saber2_pbmp_init(unit,sb2_pbmp);

        SOC_PBMP_CLEAR(si->lb_pbm);
        SOC_PBMP_PORT_ADD(si->lb_pbm, 29);
        si->lb_port = 29;
        si->cpu_hg_index = 30;
        si->cpu_hg_pp_port_index = 96;

        /* Initialize subport related pbmp linkphy_pbm and subtag_pbm */
        if (soc_feature(unit, soc_feature_subtag_coe)) {
            soc_saber2_subport_init(unit);
        }

        break;
#endif /* BCM_SABER2_SUPPORT */

#ifdef BCM_METROLITE_SUPPORT
    case BCM56270_DEVICE_ID:
    case BCM56271_DEVICE_ID:
    case BCM56272_DEVICE_ID:

    case BCM53460_DEVICE_ID:
    case BCM53461_DEVICE_ID:

        /* Clear PP Port Used Pbmp. */
        SOC_PBMP_CLEAR(pbmp_pp_unsed);

        /* Reconfigure global variables based on system config */
        SOC_IF_ERROR_RETURN(
               soc_metrolite_global_param_reconfigure(unit));

        ml_pbmp.pbmp_gport_stack=&pbmp_gport_stack;
        ml_pbmp.pbmp_mxq=&pbmp_mxq;
        ml_pbmp.pbmp_mxq1g=&pbmp_mxq1g;
        ml_pbmp.pbmp_mxq2p5g=&pbmp_mxq2p5g;
        ml_pbmp.pbmp_mxq10g=&pbmp_mxq10g;
        ml_pbmp.pbmp_xl=&pbmp_xl;
        ml_pbmp.pbmp_xl1g=&pbmp_xl1g;
        ml_pbmp.pbmp_xl2p5g=&pbmp_xl2p5g;
        ml_pbmp.pbmp_xl10g=&pbmp_xl10g;
        ml_pbmp.pbmp_xport_ge=&pbmp_xport_ge;
        ml_pbmp.pbmp_xport_xe=&pbmp_xport_xe;
        ml_pbmp.pbmp_valid=&pbmp_valid;
        ml_pbmp.pbmp_pp=&pbmp_pp;
        ml_pbmp.pbmp_linkphy=&(si->linkphy_pbm);

        /* Init ML related pbmp now */
        soc_metrolite_pbmp_init(unit,ml_pbmp,&pbmp_pp_unsed);

        SOC_PBMP_CLEAR(si->lb_pbm);
        SOC_PBMP_PORT_ADD(si->lb_pbm, 13);
        si->lb_port = 13;
        si->cpu_hg_index = 14;
        si->cpu_hg_pp_port_index = 32;

        /* Initialize subport related pbmp linkphy_pbm and subtag_pbm */
        soc_metrolite_subport_init(unit,&pbmp_pp_unsed);

        break;
#endif /* BCM_METROLITE_SUPPORT */

    default:
        /* Devices that use common port config init function for all SKUs */
        switch (drv_dev_id) {
#if defined(BCM_APACHE_SUPPORT)
        case BCM56560_DEVICE_ID:
        case BCM56670_DEVICE_ID:
        case BCM56671_DEVICE_ID:
        case BCM56672_DEVICE_ID:
        case BCM56675_DEVICE_ID:
            SOC_FEATURE_SET(unit, soc_feature_logical_port_num);

#if defined(BCM_MONTEREY_SUPPORT)
            if (dev_id == BCM56670_DEVICE_ID || dev_id == BCM56671_DEVICE_ID ||
                dev_id == BCM56672_DEVICE_ID ||dev_id ==  BCM56675_DEVICE_ID) {
                rv = soc_monterey_port_config_init(unit, dev_id, rev_id);
            } else
#endif
            {
                rv = soc_apache_port_config_init(unit, dev_id, rev_id);
            }
            if (rv < 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Port config error !!\n")));
            }

            if (dev_id == BCM56670_DEVICE_ID || dev_id ==BCM56671_DEVICE_ID ||
                dev_id == BCM56672_DEVICE_ID ||dev_id ==  BCM56675_DEVICE_ID  ) {
                /* Valid port range: [0, 74] */
                for (port = 67; port <= 129; port++) {
                    SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
                }

                si->cpu_hg_index = 67;
            } else {
            /* Valid port range: [0, 74] */
            for (port = 75; port <= 129; port++) {
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }

                si->cpu_hg_index = 75;
            }

            /* Flex counter pools - Base chip defaults */
            si->num_flex_ingress_pools = 8;
            si->num_flex_egress_pools = 4;
            si->size_flex_ingress_pool = 4096;
            si->size_flex_egress_pool = 4096;

            /* Initialize subport related pbmp and subtag_pbm */
            if (soc_feature(unit, soc_feature_hgproxy_subtag_coe)) {
                soc_td2p_subport_init(unit);
            }

            if (soc_feature(unit, soc_feature_multi_next_hops_on_port)) {
                si->num_modules = TD2P_MAX_COE_MODULES;
            }
            break;
#endif /* defined(BCM_APACHE_SUPPORT) */

#ifdef BCM_HURRICANE4_SUPPORT
    case BCM56275_DEVICE_ID:
        SOC_FEATURE_SET(unit, soc_feature_logical_port_num);
        SOC_FEATURE_SET(unit, soc_feature_sbus_format_v4);
        rv = soc_hurricane4_port_config_init(unit, dev_id);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Port config error !!")));
            return rv;
        }
        si->cpu_hg_index = 72;
        /* Flex counter pools */
        si->num_flex_ingress_pools = 20;
        si->num_flex_egress_pools = 4;
        si->size_flex_ingress_pool = 1024;
        si->size_flex_egress_pool = 512;
        si->sflow_range_max = 28;
        si->max_mtu = 12288;

        if (soc_feature(unit, soc_feature_pstats)) {
            soc_hurricane4_mmu_pstats_tbl_config(unit);
        }
        if (SOC_WARM_BOOT(unit) && si->flex_eligible) {
            SOC_PBMP_CLEAR(pbmp_xport_xe);
            for (port = 0; port < SOC_MAX_NUM_PORTS; port ++) {
                if ((si->port_l2p_mapping[port] > 0) &&
                    (!IS_HG_PORT(unit, port))) {
                    SOC_PBMP_PORT_ADD(pbmp_xport_xe, port);
                }
            }
        }

        if (soc_feature(unit, soc_feature_multi_next_hops_on_port)) {
            si->num_modules = TD3_MAX_COE_MODULES;
        }
        break;
#endif /* BCM_HURRICANE4_SUPPORT */

#ifdef BCM_FIREBOLT6_SUPPORT
    case BCM56470_DEVICE_ID:
        SOC_FEATURE_SET(unit, soc_feature_logical_port_num);
        SOC_FEATURE_SET(unit, soc_feature_sbus_format_v4);
        rv = soc_fb6_port_config_init(unit, dev_id);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Port config error !!")));
            return rv;
        }
        si->cpu_hg_index = 72;
        /* Flex counter pools */
        si->num_flex_ingress_pools = 20;
        si->num_flex_egress_pools = 4;
        si->size_flex_ingress_pool = 4096;
        si->size_flex_egress_pool = 8192;
        si->sflow_range_max = 28;
        si->max_mtu = 12288;

#if defined BCM_HGPROXY_COE_SUPPORT
        /* Initialize subport related pbmp and subtag_pbm */
        if (soc_feature(unit, soc_feature_subport_forwarding_support)) {
            rv = soc_fb6_subport_init(unit);
            if (rv < 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_fb6_subport_init config error !!")));
                return rv;
            }

        }
        if (soc_feature(unit, soc_feature_multi_next_hops_on_port)) {
            si->num_modules = TD3_MAX_COE_MODULES;
        }
#endif
        if (soc_feature(unit, soc_feature_pstats)) {
            soc_trident3_mmu_pstats_tbl_config(unit);
        }
        if (soc_feature(unit, soc_feature_efp_meter_noread)) {
            int total_indexes;
            int entry_dw;

            entry_dw = soc_mem_entry_words(unit, EFP_METER_TABLEm);
            total_indexes = soc_mem_index_count(unit, EFP_METER_TABLEm);

            if (si->efp_meter_table == NULL) {
                si->efp_meter_table =
                    sal_alloc((total_indexes * entry_dw * 4),
                    "EFP_METER sw data");

                if (si->efp_meter_table == NULL) {
                    return SOC_E_MEMORY;
                }
                sal_memset(si->efp_meter_table, 0, (total_indexes * entry_dw * 4));
            }
        }
        break;
#endif /* BCM_FIREBOLT6_SUPPORT */

#ifdef BCM_HELIX5_SUPPORT
    case BCM56370_DEVICE_ID:
        SOC_FEATURE_SET(unit, soc_feature_logical_port_num);
        SOC_FEATURE_SET(unit, soc_feature_sbus_format_v4);
        rv = soc_helix5_port_config_init(unit, dev_id);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Port config error !!")));
            return rv;
        }
        si->cpu_hg_index = 72;
        /* Flex counter pools */
        si->num_flex_ingress_pools = 20;
        si->num_flex_egress_pools = 4;
        si->size_flex_ingress_pool = 1024;
        si->size_flex_egress_pool = 2048;
        si->sflow_range_max = 28;
        si->max_mtu = 12288;

        if (soc_feature(unit, soc_feature_pstats)) {
            soc_trident3_mmu_pstats_tbl_config(unit);
        }
        if (SOC_WARM_BOOT(unit) && si->flex_eligible) {
            SOC_PBMP_CLEAR(pbmp_xport_xe);
            for (port = 0; port < SOC_MAX_NUM_PORTS; port ++) {
                if ((si->port_l2p_mapping[port] > 0) &&
                    (!IS_HG_PORT(unit, port))) {
                    SOC_PBMP_PORT_ADD(pbmp_xport_xe, port);
                }
            }
        }

        if (soc_feature(unit, soc_feature_multi_next_hops_on_port)) {
            si->num_modules = TD3_MAX_COE_MODULES;
        }
        break;
#endif /* BCM_HELIX5_SUPPORT */


        default:
            break;
        }

        break;
    }

    /* No emulation/debug only regs when a real chip or plisim */
    if (!SAL_BOOT_RTLSIM && !soc_property_get(unit, spn_EMULATION_REGS, 0)) {
        soc->disabled_reg_flags |= SOC_REG_FLAG_EMULATION;
    }

    si->ipic_port = -1;
    si->ipic_block = -1;
    si->cmic_port = -1;
    si->cmic_block = -1;
    si->fe.min = si->fe.max = -1;
    si->ge.min = si->ge.max = -1;
    si->xe.min = si->xe.max = -1;
    si->ce.min = si->ce.max = -1;
    si->hg.min = si->hg.max = -1;
    si->cpri.min = si->cpri.max = -1;
    si->rsvd4.min = si->rsvd4.max = -1;
    si->roe.min = si->roe.max = -1;
    si->hl.min = si->hl.max = -1;
    si->st.min = si->st.max = -1;
    si->lp.min = si->lp.max = -1;
    si->cl.min = si->cl.max = -1;
    si->c.min = si->c.max = -1;
    si->mmu.min = si->mmu.max = -1;
    si->tdm.min = si->tdm.max = -1;
    si->ether.min = si->ether.max = -1;
    si->port.min = si->port.max = -1;
    si->all.min = si->all.max = -1;
#ifdef BCM_SHADOW_SUPPORT
    si->ms_isec_block[0] = -1;
    si->ms_isec_block[1] = -1;
    si->ms_esec_block[0] = -1;
    si->ms_esec_block[1] = -1;
#endif /*BCM_SHADOW_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
    si->rxlp_block[0] = -1;
    si->rxlp_block[1] = -1;
    si->txlp_block[0] = -1;
    si->txlp_block[1] = -1;
    si->olp_block[0] = -1;
#endif /*BCM_KATANA2_SUPPORT */
    si->macsecport.min = si->macsecport.max = -1;

    for (blk = 0; blk < SOC_MAX_NUM_BLKS; blk++) {
        if (SOC_BLOCK_INFO(unit, blk).type == SOC_BLOCK_TYPE_INVALID) {
            break;
        }
        si->block_port[blk] = REG_PORT_ANY;
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
        if ((SOC_IS_KATANA2(unit) || SOC_IS_HURRICANE2(unit))) {
            if ((SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_RXLP) &&
                (SOC_BLOCK_INFO(unit, blk).number == 0)) {
                si->rxlp_block[0] = blk;
            }
            if ((SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_RXLP) &&
                (SOC_BLOCK_INFO(unit, blk).number == 1)) {
                si->rxlp_block[1] = blk;
            }
            if ((SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_TXLP) &&
                (SOC_BLOCK_INFO(unit, blk).number == 0)) {
                si->txlp_block[0] = blk;
            }
            if ((SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_TXLP) &&
                (SOC_BLOCK_INFO(unit, blk).number == 1)) {
                si->txlp_block[1] = blk;
            }
#if defined(BCM_KATANA2_SUPPORT)
            if ((SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_MXQPORT) &&
                (SOC_BLOCK_INFO(unit, blk).number == 10)) {
                si->olp_block[0] = blk;
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "OLP blk:%d \n"),si->olp_block[0] ));
            }
            if ((SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_MXQPORT) &&
                (SOC_BLOCK_INFO(unit, blk).number == 7)) {
                mxq7_blk = blk;
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "OLP blk:%d \n"),si->olp_block[0] ));
            }
#endif /*BCM_KATANA2_SUPPORT */
        }
#endif
#if defined(BCM_METROLITE_SUPPORT)
        if (SOC_IS_METROLITE(unit)) {
            if ((SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_IECELL) &&
                (SOC_BLOCK_INFO(unit, blk).number == 0)) {
                si->iecell_block[0] = blk;
            }
            if ((SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_IECELL) &&
                (SOC_BLOCK_INFO(unit, blk).number == 1)) {
                si->iecell_block[1] = blk;
            }
            if ((SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_OAMP) &&
                (SOC_BLOCK_INFO(unit, blk).number == 0)) {
                si->oamp_block = blk;
                si->block_valid[blk] = 1;
            }
        } else
#endif
#if defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit)) {
            if ((SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_IECELL) &&
                (SOC_BLOCK_INFO(unit, blk).number == 0)) {
                si->iecell_block[0] = blk;
            }
            if ((SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_OAMP) &&
                (SOC_BLOCK_INFO(unit, blk).number == 0)) {
                si->oamp_block = blk;
                si->block_valid[blk] = 1;
            }
        }
#endif
#if defined(BCM_FIRELIGHT_SUPPORT)
        if (SOC_IS_FIRELIGHT(unit)) {
            if ((SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_MACSEC) &&
                (SOC_BLOCK_INFO(unit, blk).number == 0)) {
                si->macsec_blocks[0] = blk;
            }
        }
#endif
    }

#define ADD_PORT(ptype, port_num) \
            if (!SOC_PBMP_MEMBER(si->ptype.bitmap, port_num)) { \
                si->ptype.port[si->ptype.num++] = port_num; \
                SOC_PBMP_PORT_ADD(si->ptype.bitmap, port_num); \
            } \
            if (si->ptype.min > port_num || si->ptype.min < 0) { \
                si->ptype.min = port_num; \
            } \
            if (si->ptype.max < port_num) {     \
                si->ptype.max = port_num; \
            } \

    max_port = -1;
    for (phy_port = 0; ; phy_port++) {
        blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
        bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, 0);

        if (blk < 0 && bindex < 0) { /* end of regsfile port list */
            break;
        }

        if (soc_feature(unit, soc_feature_logical_port_num)) {
            port = si->port_p2l_mapping[phy_port];
            if (port < 0) { /* not used in user config */
                continue;
            }
            if (port > max_port) {
                max_port = port;
            }
            if (si->flex_port_p2l_mapping[phy_port] == -1) {
                SOC_PBMP_PORT_ADD(si->port.disabled_bitmap, port);
                SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);
            }
        } else {
            port = phy_port;
            max_port = phy_port;
        }

        if (!SOC_PBMP_MEMBER(pbmp_valid, port)) {   /* disabled port */
            int blk_list[2];
            blk_list[0] = SOC_BLOCK_INFO(unit, blk).type;
            blk_list[1] = SOC_BLOCK_TYPE_INVALID;
            if (SOC_BLOCK_IN_LIST(unit, blk_list, SOC_BLK_CPU)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                                     "soc_info_config: "
                                     "cannot disable cpu port\n")));
            } else {
                blk = -1; /* mark as disabled port */
#ifdef BCM_KATANA2_SUPPORT
                /*
                 * On KT2 for configurations which do not use OLP port
                 * setup up the port info.
                 */
                if ((SOC_IS_KATANA2(unit) && (!SOC_IS_SABER2(unit)) &&
                    (port == KT2_OLP_PORT))) {
                    if (!si->olp_port[0] && (si->block_valid[mxq7_blk])) {
                        SOC_DRIVER(unit)->port_info[KT2_OLP_PORT].blk =
                                                                    mxq7_blk;
                        SOC_DRIVER(unit)->port_info[KT2_OLP_PORT].bindex = 3;
                        SOC_PBMP_PORT_ADD(si->block_bitmap[mxq7_blk], port);
                        SOC_PBMP_PORT_REMOVE(si->block_bitmap[si->olp_block[0]],
                                             port);
                    }
                }
#endif
            }
        }
#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_KATANA_SUPPORT) \
        || defined(BCM_SABER2_SUPPORT)
        if(SOC_IS_ENDURO(unit) || SOC_IS_KATANAX(unit)) {
            if(!SOC_IS_SABER2(unit) && SAL_BOOT_QUICKTURN) {
            if (IS_LB_PORT(unit, port)) {
                blk = -1; /* mark as disabled port */
            }
        }
        }
#endif

#if defined(BCM_SHADOW_SUPPORT)
        if(SOC_IS_SHADOW(unit)) {
            if (SOC_PBMP_MEMBER(pbmp_disabled, port) && !IS_IL_PORT(unit, port)) {   /* disabled port */
                SOC_PBMP_PORT_REMOVE(pbmp_valid, port);
            }
        }
#endif
        if (blk < 0) { /* disabled port */
            sal_snprintf(si->port_name[port], sizeof(si->port_name[port]),
                         "?%d", port);
            si->port_offset[port] = port;
            continue;
        }

        blktype = -1;
        bname = "?";
        for (port_idx = 0; port_idx < SOC_DRIVER(unit)->port_num_blktype;
             port_idx++) {
            int skip_blk_port = 0;
            blk = SOC_PORT_IDX_BLOCK(unit, phy_port, port_idx);
            if (SOC_IS_GREYHOUND(unit) &&
                ((SOC_BLOCK_INFO(unit, blk).number & 0xf00) == 0xf00)){
                if(phy_port == 5){
                    SOC_BLOCK_INFO(unit, blk).number &= ~0xf00;
                }
                skip_blk_port = 1;
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "skip the phy_port %d blk %d\n"),
                                        phy_port,blk));
            }
            if (blk < 0) { /* end of block list of each port */
                break;
            }

            old_blktype = blktype;
            old_bname = bname;

            blktype = SOC_BLOCK_INFO(unit, blk).type;
            bname = soc_block_port_name_lookup_ext(blktype, unit);
            switch (blktype) {
            case SOC_BLK_MXQPORT:
                if (!si->port_speed_max[port]) {
                    if (SOC_PBMP_MEMBER(pbmp_mxq21g, port)) {
                        si->port_speed_max[port] = 21000;
                    } else if (SOC_PBMP_MEMBER(pbmp_mxq13g, port)) {
                        si->port_speed_max[port] = 13000;
                    } else if (SOC_PBMP_MEMBER(pbmp_mxq2p5g, port)) {
                        si->port_speed_max[port] = 2500;
                    } else if (SOC_PBMP_MEMBER(pbmp_mxq1g, port)) {
                        si->port_speed_max[port] = 1000;
                    } else {
                        si->port_speed_max[port] = 10000;
                    }
                }
#if defined(BCM_HGPROXY_COE_SUPPORT)
                if (soc_feature(unit, soc_feature_hgproxy_subtag_coe) &&
                    SOC_PBMP_MEMBER(si->subtag_pbm, port)) {
                    pno = si->subtag.num;
                    ADD_PORT(subtag, port);
                }
#endif
#if defined(BCM_KATANA2_SUPPORT)
                if (soc_feature(unit, soc_feature_linkphy_coe) &&
                    SOC_PBMP_MEMBER(si->linkphy_pbm, port)) {
                    pno = si->lp.num;
                    ADD_PORT(lp, port);
                }
                if (soc_feature(unit, soc_feature_subtag_coe) &&
                    SOC_PBMP_MEMBER(si->subtag_pbm, port)) {
                    pno = si->subtag.num;
                    ADD_PORT(subtag, port);
                }
#endif
                if (si->port_speed_max[port] < 10000) { /* GE */
                    pno = si->ge.num;
                    ADD_PORT(ge, port);
                    if (SOC_PBMP_MEMBER(pbmp_gport_stack, port)) {
                        /* GE in Higig Lite Mode. Higig Like stack capable
                         * port */
                        ADD_PORT(st, port);
                        ADD_PORT(hl, port);
#if defined(BCM_KATANA2_SUPPORT)
                        if (SOC_IS_KATANA2(unit) && !SOC_IS_SABER2(unit)) {
                            ADD_PORT(hg, port);
                        }
#endif
                    } else {
                        ADD_PORT(ether, port);
                    }
                } else if (SOC_PBMP_MEMBER(pbmp_xport_xe, port)) { /* XE */
                    pno = si->xe.num;
                    ADD_PORT(xe, port);
                    ADD_PORT(ether, port);
                    bname = "xe";
                } else { /* HG */
                    pno = si->hg.num;
                    ADD_PORT(hg, port);
                    ADD_PORT(st, port);
                }

                ADD_PORT(port, port);
                if (SOC_PBMP_MEMBER(pbmp_mxq, port)) {
                    ADD_PORT(mxq, port);
                }
                if (SOC_PBMP_MEMBER(pbmp_hyplite, port)) {
                    ADD_PORT(hyplite, port);
                }
                ADD_PORT(all, port);
                break;
            case SOC_BLK_EPIC:
                pno = si->fe.num;
                ADD_PORT(fe, port);
                ADD_PORT(ether, port);
                ADD_PORT(port, port);
                ADD_PORT(all, port);
                break;
            case SOC_BLK_GPIC:
            case SOC_BLK_XTPORT:
            case SOC_BLK_QGPORT:
            case SOC_BLK_GPORT:
                if (SOC_PBMP_MEMBER(pbmp_gport_fe, port)) { /* GE  vs FE SMII  */
                    pno = si->fe.num;
                    bname = "fe";
                    ADD_PORT(fe, port);
                } else {
                    if (SOC_IS_TRIUMPH3(unit)) {
                        if (si->port_speed_max[port] == 10000) {
                            pno = si->xe.num;
                            ADD_PORT(xe, port);
                        } else {
                            pno = si->ge.num;
                            ADD_PORT(ge, port);
                        }
                    } else {
                        pno = si->ge.num;
                        ADD_PORT(ge, port);
#if defined(BCM_FIRELIGHT_SUPPORT)
                        if (SOC_IS_FIRELIGHT(unit)) {
                            ADD_PORT(qsgmii, port);
                        }
#endif /* BCM_FIRELIGHT_SUPPORT */
                    }
                }
                if (SOC_PBMP_MEMBER(pbmp_gport_stack, port)) {
                    /* GE in Higig Lite Mode. Higig Like stack capable port */
                    ADD_PORT(st, port);
                    ADD_PORT(hl, port);
                } else {
                    ADD_PORT(ether, port);
                }
                if (SOC_IS_TRIUMPH3(unit)) {
                    ADD_PORT(xt, port);
                }
                ADD_PORT(port, port);
                ADD_PORT(all, port);
                break;
            case SOC_BLK_XWPORT:
                if (si->port_speed_max[port] && si->port_speed_max[port] < 10000) { /* GE */
                    pno = si->ge.num;
                    ADD_PORT(ge, port);
                    ADD_PORT(ether, port);
                    bname = "ge";
                } else if (SOC_PBMP_MEMBER(pbmp_xport_xe, port) ||
                           (si->port_speed_max[port] == 10000)) { /* XE */
                    pno = si->xe.num;
                    ADD_PORT(xe, port);
                    ADD_PORT(ether, port);
                    bname = "xe";
                } else { /* HG */
                    pno = si->hg.num;
                    ADD_PORT(hg, port);
                    ADD_PORT(st, port);
                }
                ADD_PORT(xw, port);
                ADD_PORT(port, port);
                ADD_PORT(all, port);
                break;
            case SOC_BLK_XPIC:
                pno = si->xe.num;
                ADD_PORT(xe, port);
                ADD_PORT(ether, port);
                ADD_PORT(port, port);
                ADD_PORT(all, port);
                break;
            case SOC_BLK_IPIC:
                si->ipic_port = port;
                si->ipic_block = blk;
                /* FALLTHROUGH */
            case SOC_BLK_HPIC:
                pno = si->hg.num;
                ADD_PORT(hg, port);
                ADD_PORT(st, port);
                ADD_PORT(port, port);
                ADD_PORT(all, port);
                break;
            case SOC_BLK_XPORT:
                if (SOC_PBMP_MEMBER(pbmp_xport_xe, port)) { /* XE  vs HG  */
                    pno = si->xe.num;
                    ADD_PORT(xe, port);
                    ADD_PORT(ether, port);
                    bname = "xe";
                } else {
                    pno = si->hg.num;
                    ADD_PORT(hg, port);
                    ADD_PORT(st, port);
                }
                si->port_speed_max[port] = 12000;
                ADD_PORT(port, port);
                ADD_PORT(all, port);
                break;
            case SOC_BLK_CPORT:
                if (si->port_speed_max[port] < 100000) {
                    /* don't use the name from this block */
                    blktype = old_blktype;
                    bname = old_bname;
                    break;
                }
                ADD_PORT(c, port);
                /* CPort blocks like those in TD2+ use the xe port bitmap for
                 * determining whether a port is ethernet or higig.  Others
                 * such as TD2 or TR3 use port speed to determine higig settings
                 */
                if ((soc_feature(unit, soc_feature_cport_clmac) &&
                   SOC_PBMP_MEMBER(pbmp_xport_xe, port)) ||
                   (!soc_feature(unit,soc_feature_cport_clmac) &&
                   ((si->port_speed_max[port] == 100000) ||
                   (si->port_speed_max[port] == 120000)) )) {
                    pno = si->c.num;
                    ADD_PORT(ce, port);
                    ADD_PORT(ether, port);
                    /* name is default to ce by lookup above */
                } else { /* HG */
                    pno = si->hg.num;
                    ADD_PORT(hg, port);
                    ADD_PORT(st, port);
                    bname = "hg";
                }
                ADD_PORT(port, port);
                ADD_PORT(all, port);
                break;
            case SOC_BLK_CPRI:

                if ((SOC_PBMP_MEMBER(pbmp_xport_cpri, port))) {
                     ADD_PORT(all, port);
                     ADD_PORT(port, port);
                     pno = si->roe.num;
                     ADD_PORT(roe, port);
                     if ((SOC_PBMP_MEMBER(pbmp_roe_compression, port))) {
                         SOC_PBMP_PORT_ADD(si->roe_compression, port);

                     }
                     if (soc_property_port_get(unit, port, spn_RSVD4_PORT_INIT_SPEED_ID, -1) != -1) {
                         ADD_PORT(rsvd4, port);
                     } else {
                         ADD_PORT(cpri, port);
                     }
                     bname = "roe";
                }
                     break;
            case SOC_BLK_CXXPORT:
                ADD_PORT(cxx, port);
                /* don't use the name from this block */
                blktype = old_blktype;
                bname = old_bname;

                break;
                /* fall through to case for SOC_BLK_[CX]LPORT */
                /* coverity[fallthrough: FALSE] */
            case SOC_BLK_CLG2PORT:
                if (SOC_BLK_CLG2PORT == blktype) {
                    ADD_PORT(clg2, port);
                }
                /* fall through to case for SOC_BLK_CLPORT */
                /* coverity[fallthrough: FALSE] */
            case SOC_BLK_CLPORT:
#if defined(BCM_APACHE_SUPPORT)
                if (SOC_IS_APACHE(unit) && (!SOC_IS_MONTEREY(unit))) {
                    if (((phy_port == 17) || (phy_port == 53)) &&
                        (si->port_speed_max[port] < 100000)) {
                        blktype = old_blktype;
                        bname = old_bname;
                        continue;
                    }
                }
#endif
                if (SOC_BLK_CLPORT == blktype) {
                    ADD_PORT(cl, port);
                }
                if(SOC_IS_MONTEREY(unit) &&
                    SOC_PBMP_MEMBER(pbmp_xport_cpri, port)) {
                    blktype = old_blktype;
                    bname = old_bname;
                   break ;
                 }

                if ((si->port_speed_max[port] >= 100000) &&
                    (SOC_BLK_CXXPORT != blktype)) {
                    /* Catch both CE and HG ports */
                    ADD_PORT(c, port);
                }
                if (SOC_PBMP_MEMBER(pbmp_xport_xe, port) &&
                    ((si->port_speed_max[port] == 100000) ||
                    (si->port_speed_max[port] == 120000))) { /* CE */
                    pno = si->ce.num;
                    ADD_PORT(ce, port);
                    ADD_PORT(ether, port);
                    bname = "ce";
                    ADD_PORT(port, port);
                    ADD_PORT(all, port);
                    break;
                }

                /* fall through to case for SOC_BLK_XLPORT */
                /* coverity[fallthrough: FALSE] */
            case SOC_BLK_XLPORTB0:
                if (SOC_BLK_XLPORTB0 == blktype) {
                    ADD_PORT(xlb0, port);
                }
                /* fall through to case for SOC_BLK_XLPORT */
                /* coverity[fallthrough: FALSE] */
            case SOC_BLK_XLPORT:

                if(SOC_IS_GREYHOUND(unit) && (skip_blk_port == 1)){
                    si->block_valid[blk] += 1;
                    continue;
                }
                if ((soc_feature(unit, soc_feature_reset_xlport_block_tsc12)) &&
                    (si->port_speed_max[port] >= 100000)) {
                    int block_num = 0, offset = 0;
                    block_num = (phy_port-1)/16;
                    if ( block_num % 2 == 0 ) {
                        offset = 1;
                    } else {
                        offset = -1;
                    }
                    /* set block_valid flag for 3 SOC_BLK_XLPORT in TSC12 */
                    si->block_valid[blk] += 1;
                    si->block_valid[blk+offset] += 1;
                    si->block_valid[blk+2*offset] += 1;
                }
#if defined(BCM_APACHE_SUPPORT)
                if (SOC_IS_APACHE(unit) && (!SOC_IS_MONTEREY(unit))) {
                    if (((phy_port == 17) || (phy_port == 53))) {
                        if ((blktype == SOC_BLK_XLPORT) &&
                            (si->port_speed_max[port] >= 100000)) {
                            if (si->block_port[blk] < 0) {
                                si->block_port[blk] = port;
                            }
                            if (si->block_port[blk + 1] < 0) {
                                si->block_port[blk + 1] = port;
                            }
                            if (si->block_port[blk + 2] < 0) {
                                si->block_port[blk + 2] = port;
                            }
                            si->block_valid[blk] += 1;
                            si->block_valid[blk + 1] += 1;
                            si->block_valid[blk + 2] += 1;
                            /* No need to add in block bitmap */
                            continue;
                        }
                    } else if (SOC_SUCCESS(soc_apache_port_reg_blk_index_get
                                              (unit, port, SOC_BLK_CXXPORT, &blk))) {
                        si->block_valid[blk] += 1;
                        if (si->block_port[blk] < 0) si->block_port[blk] = port;
                        /* Restore old blk value */
                        blk = SOC_PORT_IDX_BLOCK(unit, phy_port, port_idx);
                    }
                }
#endif
                if ((SOC_BLK_XLPORT == blktype) &&
                    (si->port_speed_max[port] >= 100000)) {
                    /* don't use the name from this block */
                    blktype = old_blktype;
                    bname = old_bname;
                    break;
                }
                if (!si->port_speed_max[port]) {
#if defined(BCM_SABER2_SUPPORT)
                    if (SOC_IS_SABER2(unit) &&
                            SOC_PBMP_MEMBER(pbmp_xl1g, port)) {
                        si->port_speed_max[port] = 1000;
                    } else if (SOC_IS_SABER2(unit) &&
                            SOC_PBMP_MEMBER(pbmp_xl2p5g, port)) {
                        si->port_speed_max[port] = 2500;
                    } else if (SOC_IS_SABER2(unit) &&
                            SOC_PBMP_MEMBER(pbmp_xl10g, port)) {
                        if (!(SOC_PBMP_MEMBER(pbmp_xport_xe, port))) {
                            si->port_speed_max[port] = 11000;
                        } else {
                            si->port_speed_max[port] = 10000;
                        }
                    } else
#endif
                    if (SOC_PBMP_MEMBER(pbmp_xport_ge, port)) {
                        si->port_speed_max[port] = 2500;
                    } else if (SOC_PBMP_MEMBER(pbmp_gx25g, port)) {
                        si->port_speed_max[port] = 25000;
                    } else if (SOC_PBMP_MEMBER(pbmp_gx21g, port)) {
                        si->port_speed_max[port] = 21000;
                    } else if (SOC_PBMP_MEMBER(pbmp_gx20g, port)) {
                        si->port_speed_max[port] = 20000;
                    } else if (SOC_PBMP_MEMBER(pbmp_gx16g, port)) {
                        si->port_speed_max[port] = 16000;
                    } else if (SOC_PBMP_MEMBER(pbmp_gx13g, port)) {
                        si->port_speed_max[port] = 13000;
                    } else if (SOC_PBMP_MEMBER(pbmp_gx12g, port)) {
                        si->port_speed_max[port] = 12000;
                    } else {
                        si->port_speed_max[port] = 10000;
                    }
#ifdef BCM_SHADOW_SUPPORT
                    if (SOC_IS_SHADOW(unit)) {
                        if SOC_PBMP_MEMBER(pbmp_xl12g, port) {
                            si->port_speed_max[port] = 12000;
                        } else if (SOC_PBMP_MEMBER(pbmp_xl40g, port)) {
                            si->port_speed_max[port] = 40000;
                        }
                    }
#endif
                }
#ifdef BCM_SABER2_SUPPORT
                if (SOC_IS_SABER2(unit)) {
                    if (SOC_PBMP_MEMBER(pbmp_xport_ge,port)) {
                        pno = si->ge.num;
                        ADD_PORT(ge, port);
                        ADD_PORT(ether, port);
                        bname = "ge";
                    } else if (SOC_PBMP_MEMBER(pbmp_xport_xe, port)) {
                        pno = si->xe.num;
                        ADD_PORT(xe, port);
                        ADD_PORT(ether, port);
                        bname = "xe";
                    } else {
                        pno = si->hg.num;
                        ADD_PORT(hg, port);
                        ADD_PORT(st, port);
                        bname = "hg";
                    }
                } else
#endif
                if (si->port_speed_max[port] < 10000) { /* GE */
#ifdef BCM_GREYHOUND_SUPPORT
                    if (SOC_IS_GREYHOUND(unit) &&
                        si->port_speed_max[port] == 5000) {
                        if (SOC_PBMP_MEMBER(pbmp_xport_ge, port)) { /* GE */
                            pno = si->ge.num;
                            si->port_speed_max[port] = 2500;
                            ADD_PORT(ge, port);
                            ADD_PORT(ether, port);
                            bname = "ge";
                        } else if (SOC_PBMP_MEMBER(pbmp_xport_xe, port)) { /* XE */
                            pno = si->xe.num;
                            ADD_PORT(xe, port);
                            ADD_PORT(ether, port);
                            bname = "xe";
                        } else { /* HG */
                            pno = si->hg.num;
                            ADD_PORT(hg, port);
                            ADD_PORT(st, port);
                        }
                    } else
#endif
                    {
                    pno = si->ge.num;
                    ADD_PORT(ge, port);
                    if (SOC_PBMP_MEMBER(pbmp_gport_stack, port)) {
                        /* GE in Higig Lite Mode. Higig Like stack capable
                         * port */
                        ADD_PORT(st, port);
                        ADD_PORT(hl, port);
                    } else {
                        ADD_PORT(ether, port);
                    }
                    }
                } else if (SOC_PBMP_MEMBER(pbmp_xport_xe, port)) { /* XE */
                    pno = si->xe.num;
                    ADD_PORT(xe, port);
                    ADD_PORT(ether, port);
                    bname = "xe";
#ifdef BCM_TRIUMPH3_SUPPORT
                    if (SOC_IS_TRIUMPH3(unit)) {
                        if (si->port_speed_max[port] == 42000) {
                            si->port_speed_max[port] = 40000;
                        }
                    }
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_SHADOW_SUPPORT
                    if (SOC_IS_SHADOW(unit) &&
                        (
                      soc_property_get(unit, spn_BCM88732_2X40_8X10, 0) ||
                      soc_property_get(unit, spn_BCM88732_1X40_4X10, 0) ||
                      soc_property_get(unit, spn_BCM88732_4X10_4X10, 0))) {
                      /* TDM16 and TDM 17 */
                        if (SOC_PBMP_MEMBER(pbmp_disabled, port)) {
                            SOC_PBMP_PORT_ADD(si->xe.disabled_bitmap, port);
                            SOC_PBMP_PORT_ADD(si->port.disabled_bitmap, port);
                            SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);
                        }
                    }
                } else if (soc_property_get(unit, spn_BCM88732_2X40_2X40, 0) ||
                    soc_property_get(unit, spn_BCM88732_2X40_1X40, 0) ||
                    soc_property_get(unit, spn_BCM88732_8X10_1X40, 0) ||
                    soc_property_get(unit, spn_BCM88732_8X10_2X40, 0)) {
                    /* Interlaken  Modes */
                    pno = si->il.num;
                    ADD_PORT(il, port);
                    bname = "il";
                    if (SOC_PBMP_MEMBER(pbmp_disabled, port)) {
                        SOC_PBMP_PORT_ADD(si->il.disabled_bitmap, port);
                        SOC_PBMP_PORT_ADD(si->port.disabled_bitmap, port);
                        SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);
                    }
#endif /* BCM_SHADOW_SUPPORT */
                }
#ifdef BCM_HURRICANE2_SUPPORT
                else if ((SOC_IS_HURRICANE2(unit)|| SOC_IS_GREYHOUND(unit) ||
                    SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)) &&
                    (SOC_PBMP_MEMBER(pbmp_xport_ge, port))) { /* GE */
                    pno = si->ge.num;
                    si->port_speed_max[port] = 1000;
#ifdef BCM_GREYHOUND_SUPPORT
                    if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
                        SOC_IS_GREYHOUND2(unit)) {
                        si->port_speed_max[port] = 2500;
                    }
#endif
                    ADD_PORT(ge, port);
                    ADD_PORT(ether, port);
                    bname = "ge";
                }
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
                else if (SOC_IS_TRIUMPH3(unit) && (si->port_speed_max[port] == 40000)) {
                    pno = si->xe.num;
                    ADD_PORT(xe, port);
                    ADD_PORT(ether, port);
                    bname = "xe";
                }
#endif /* BCM_TRIUMPH3_SUPPORT */
                else { /* HG */
#if defined(BCM_TOMAHAWK_SUPPORT)
                    if (SOC_IS_TOMAHAWKX(unit)) {
                        if (latency[unit]) { /* multiplex as flag */
                            continue;
                        }

                        /* one time */
                        SOC_IF_ERROR_RETURN(soc_th_latency_get(unit, &latency[unit]));
                        if (latency[unit]) {
                            /* Disable HG ports for low latency modes */
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                               "INFO: HiGig ports not supported in low "
                               "latency modes\n"
                               "ERROR: HiGig ports specified in config.bcm"
                               " ignored\n"
                               "INFO: To enable HG ports configure unit "
                               "for normal latency in config.bcm\n")));
                            continue;
                        }
                        pno = si->hg.num;
                        ADD_PORT(hg, port);
                        ADD_PORT(st, port);
                        /* name is default to hg by lookup above */
                    } else
#endif /*BCM_TOMAHAWK_SUPPORT */
                    {
                        pno = si->hg.num;
                        ADD_PORT(hg, port);
                        ADD_PORT(st, port);
                        /* name is default to hg by lookup above */
                    }
                }
#if defined(BCM_SHADOW_SUPPORT)
                if (SOC_IS_SHADOW(unit)) {
                    if (SOC_PBMP_MEMBER(pbmp_xl, port)) {
                        ADD_PORT(xl, port);
                    }
                } else
#endif /* BCM_SHADOW_SUPPORT */
                {

                    if (!IS_CL_PORT(unit, port) && !IS_CLG2_PORT(unit, port) &&
                        !IS_XLB0_PORT(unit, port)) {
                        ADD_PORT(xl, port);
                    }
                }
                if (!SOC_IS_TRIUMPH3(unit) && !SOC_IS_SABER2(unit)
                    && !SOC_IS_HURRICANE4(unit)
                    && !SOC_IS_FIREBOLT6(unit)
                    && !SOC_IS_HELIX5(unit)) {
                    
                    ADD_PORT(gx, port);
                }
                ADD_PORT(port, port);
                ADD_PORT(all, port);
                break;
            case SOC_BLK_GXPORT:
            case SOC_BLK_GXFPPORT:
                if(SOC_IS_GREYHOUND(unit)&& (skip_blk_port == 1)){
                    si->block_valid[blk] += 1;
                    continue;
                }
#ifdef BCM_HELIX5_SUPPORT
                if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
                    int qmode;
#ifdef BCM_HURRICANE4_SUPPORT
                    if (SOC_IS_HURRICANE4(unit)) {
                        soc_hurricane4_port_qsgmii_mode_get(unit, port, &qmode);
                        if ((si->port_l2p_mapping[port] >= 1) &&
                            (si->port_l2p_mapping[port] <= 24)) {
                            ADD_PORT(egphy, port);
                        }
                    } else
#endif
                    {
                        soc_helix5_port_qsgmii_mode_get(unit, port, &qmode);
                    }
                    if (qmode) {
                        ADD_PORT(qsgmii, port);
                    }
                }
#endif
                if (!si->port_speed_max[port]) {
                    if (SOC_PBMP_MEMBER(pbmp_xport_ge, port)) {
                        si->port_speed_max[port] = 2500;
                    } else if (SOC_PBMP_MEMBER(pbmp_gx25g, port)) {
                        si->port_speed_max[port] = 25000;
                    } else if (SOC_PBMP_MEMBER(pbmp_gx21g, port)) {
                        si->port_speed_max[port] = 21000;
                    } else if (SOC_PBMP_MEMBER(pbmp_gx20g, port)) {
                        si->port_speed_max[port] = 20000;
                    } else if (SOC_PBMP_MEMBER(pbmp_gx16g, port)) {
                        si->port_speed_max[port] = 16000;
                    } else if (SOC_PBMP_MEMBER(pbmp_gx13g, port)) {
                        si->port_speed_max[port] = 13000;
                    } else if (SOC_PBMP_MEMBER(pbmp_gx12g, port)) {
                        si->port_speed_max[port] = 12000;
                    } else {
                        si->port_speed_max[port] = 10000;
                    }
                }
                if (si->port_speed_max[port] < 10000) { /* GE */
                    pno = si->ge.num;
                    ADD_PORT(ge, port);
                    ADD_PORT(ether, port);
                    bname = "ge";
                } else if (SOC_PBMP_MEMBER(pbmp_xport_xe, port)) { /* XE */
                    pno = si->xe.num;
                    ADD_PORT(xe, port);
                    ADD_PORT(ether, port);
                    bname = "xe";
                } else { /* HG */
                    pno = si->hg.num;
                    ADD_PORT(hg, port);
                    ADD_PORT(st, port);
                    /* name is default to hg by lookup above */
                }
                if (!SOC_IS_GREYHOUND(unit)) {
                    /* no xe capability in Greyhound's SOC_BLK_GXPORT */
                    ADD_PORT(gx, port);
                }
                if (SOC_PBMP_MEMBER(pbmp_hyplite, port)) {
                    ADD_PORT(hyplite, port);
                }
                ADD_PORT(port, port);
                ADD_PORT(all, port);
                break;
            case SOC_BLK_XQPORT:
            case SOC_BLK_XGPORT:
                if (SOC_PBMP_MEMBER(pbmp_xport_ge, port)) { /* GE */
                    pno = si->ge.num;
                    ADD_PORT(ge, port);
                    ADD_PORT(ether, port);
                    bname = "ge";
                } else {
                    if (!si->port_speed_max[port]) {
                        if (SOC_PBMP_MEMBER(pbmp_xq12g, port)) {
                            si->port_speed_max[port] = 12000;
                        } else if (SOC_PBMP_MEMBER(pbmp_xq13g, port)) {
                            si->port_speed_max[port] = 13000;
                        } else if (SOC_PBMP_MEMBER(pbmp_xq10g, port)) {
                            si->port_speed_max[port] = 10000;
                        } else if (SOC_PBMP_MEMBER(pbmp_xg2p5g, port) ||
                                   SOC_PBMP_MEMBER(pbmp_xq2p5g, port)) {
                            si->port_speed_max[port] = 2500;
                        } else {
                            si->port_speed_max[port] = 1000;
                        }
                    }
                    if (SOC_PBMP_MEMBER(pbmp_xq12g, port) ||
                        SOC_PBMP_MEMBER(pbmp_xq13g, port) ||
                        SOC_PBMP_MEMBER(pbmp_xq10g, port)) {
                        ADD_PORT(gx, port);
                    }
                    if (si->port_speed_max[port] >= 10000) {
                        if (SOC_PBMP_MEMBER(pbmp_xport_xe, port)) { /* XE */
                            pno = si->xe.num;
                            ADD_PORT(xe, port);
                            ADD_PORT(ether, port);
                            bname = "xe";
                        } else { /* HG */
                            pno = si->hg.num;
                            ADD_PORT(hg, port);
                            ADD_PORT(st, port);
                            bname = "hg";
                        }
                    } else {
                        pno = si->ge.num;
                        ADD_PORT(ge, port);
                        if (SOC_PBMP_MEMBER(pbmp_gport_stack, port)) {
                            /* GE in Higig Lite Mode. Higig Like stack capable
                             * port */
                            ADD_PORT(st, port);
                            ADD_PORT(hl, port);
                        } else {
                            ADD_PORT(ether, port);
                        }
                    }
                }
                if (SOC_PBMP_MEMBER(pbmp_disabled, port)) {
                    SOC_PBMP_PORT_ADD(si->ge.disabled_bitmap, port);
                    SOC_PBMP_PORT_ADD(si->ether.disabled_bitmap, port);
                    SOC_PBMP_PORT_ADD(si->port.disabled_bitmap, port);
                    SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);
                }
                if (SOC_PBMP_MEMBER(pbmp_xq, port)) {
                    ADD_PORT(xq, port);
                } else {
                    ADD_PORT(xg, port);
                }
                if (SOC_PBMP_MEMBER(pbmp_hyplite, port)) {
                    ADD_PORT(hyplite, port);
                }
                ADD_PORT(port, port);
                ADD_PORT(all, port);
                break;
            case SOC_BLK_SPORT:
                pno = si->ge.num;
                ADD_PORT(ge, port);
                ADD_PORT(ether, port);
                ADD_PORT(port, port);
                ADD_PORT(all, port);
                break;
            case SOC_BLK_RDB:
                si->rdb_port = port;
                pno = si->rdbport.num;
                ADD_PORT(rdbport, port);
                ADD_PORT(all, port);
                break;
            case SOC_BLK_BROADSCAN:
                si->fae_port = port;
                pno = si->faeport.num;
                ADD_PORT(faeport, port);
                ADD_PORT(all, port);
                bname = "fae";
                break;
            case SOC_BLK_MACSEC:
                si->macsec_port = port;
                pno = si->macsecport.num;
                ADD_PORT(macsecport, port);
                ADD_PORT(all, port);
                break;
            case SOC_BLK_LBPORT:
                pno = si->lbport.num;
                ADD_PORT(lbport, port);
                ADD_PORT(all, port);
                /* keep legacy setup for device with single loopback port */
                si->lb_port = port;
                SOC_PBMP_PORT_ADD(si->lb_pbm, port);
                break;
            case SOC_BLK_AXP:
                pno = si->axp.num;
                ADD_PORT(axp, port);
                ADD_PORT(all, port);
                bname = "axp";
                break;
            case SOC_BLK_CPIC:
            case SOC_BLK_CMIC:
                pno = 0;
                si->cmic_port = port;
                si->cmic_block = blk;
                SOC_PBMP_PORT_ADD(si->cmic_bitmap, port);
                ADD_PORT(all, port);
                break;
            case SOC_BLK_PGW_CL:
                /* don't use the name from this block */
                blktype = old_blktype;
                bname = old_bname;
                break;
            case SOC_BLK_CDPORT:
                if (SOC_BLK_CDPORT == blktype) {
                    ADD_PORT(cd, port);
                }

                if (SOC_PBMP_MEMBER(pbmp_xport_xe, port)) {

                    if (si->port_speed_max[port] >= 100000 &&
                        si->port_speed_max[port] <= 400000) {

                        ADD_PORT(c, port);
                        /* For 200G and 400G ports, the internal port bitmap
                         * is cde, but externally (on the console) it is still
                         * referred to as cd to keep the convention similar
                         * to xe and ce for users
                         */
                        if (si->port_speed_max[port] == 400000 ||
                            si->port_speed_max[port] == 200000) {
                            pno = si->cde.num;
                            ADD_PORT(cde, port);
                            bname = "cd";
                        } else {
                            pno = si->ce.num;
                            ADD_PORT(ce, port);
                            bname = "ce";
                        }
                    } else { /* XE */
                        pno = si->xe.num;
                        ADD_PORT(xe, port);
                        bname = "xe";
                    }
                    ADD_PORT(ether, port);
                } else {
                    pno = 0;
                }

                ADD_PORT(port, port);
                ADD_PORT(all, port);

                break;
            default:
                pno = 0;
                break;
            }

#ifdef BCM_KATANA2_SUPPORT
            if ((SOC_IS_KATANA2(unit) && (!SOC_IS_SABER2(unit)) && (port == KT2_OLP_PORT))) {
                if (si->olp_port[0] == 1) {
                    si->block_valid[si->olp_block[0]] = 1;
                    SOC_DRIVER(unit)->port_info[KT2_OLP_PORT].blk =
                                                              si->olp_block[0];
                    SOC_DRIVER(unit)->port_info[KT2_OLP_PORT].bindex = 0;
                    si->block_port[si->olp_block[0]] = port;
                        SOC_PBMP_PORT_REMOVE(si->block_bitmap[mxq7_blk], port);
                    SOC_PBMP_PORT_ADD(si->block_bitmap[si->olp_block[0]], port);
                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                 "OLP Port Configuration blk=%d"
                                 "schain=%d cmic=%d\n"),
                                 SOC_DRIVER(unit)->port_info[KT2_OLP_PORT].blk,
                                 SOC_DRIVER(unit)->block_info[si->olp_block[0]].
                                                              schan,
                                 SOC_DRIVER(unit)->block_info[si->olp_block[0]].
                                                              cmic));
               } else {
                    si->block_valid[mxq7_blk] = 1;
                    SOC_DRIVER(unit)->port_info[KT2_OLP_PORT].blk = mxq7_blk;
                    SOC_DRIVER(unit)->port_info[KT2_OLP_PORT].bindex = 3;
                    si->block_port[mxq7_blk] = port;
                    SOC_PBMP_PORT_ADD(si->block_bitmap[mxq7_blk], port);
                    SOC_PBMP_PORT_REMOVE(si->block_bitmap[si->olp_block[0]], port);
                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                 "OLP Port Configuration blk=%d"
                                 "schain=%d cmic=%d\n"),
                                 SOC_DRIVER(unit)->port_info[KT2_OLP_PORT].blk,
                                 SOC_DRIVER(unit)->block_info[mxq7_blk].schan,
                                 SOC_DRIVER(unit)->block_info[mxq7_blk].cmic));
               }
            } else
#endif
            {
                si->block_valid[blk] += 1;
                if (si->block_port[blk] < 0) {
                    si->block_port[blk] = port;
                }
                SOC_PBMP_PORT_ADD(si->block_bitmap[blk], port);
            }
        }

        if (bname[0] == '?') {
            pno = port;
        }
        sal_snprintf(si->port_name[port], sizeof(si->port_name[port]),
                     "%s%d", bname, pno);
        si->port_type[port] = blktype;
        si->port_offset[port] = pno;
#ifdef BCM_SHADOW_SUPPORT
        if (SOC_IS_SHADOW(unit)) {
            /* Add ports 1 through 8 to the MACSEC blocks */
            /* Add ports 9 and 13 to the IL blocks */
            blk = 0;
            while (SOC_BLOCK_INFO(unit, blk).type != -1) {
                if ((SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_MS_ISEC) ||
                    (SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_MS_ESEC)) {
                    if (((SOC_BLOCK_INFO(unit, blk).number == 0) && (port >= 1)
                        && (port <= 4)) ||
                        ((SOC_BLOCK_INFO(unit, blk).number == 1) && (port >= 5)
                        && (port <= 8))) {
                        si->block_valid[blk] += 1;
                        if (si->block_port[blk] < 0) {
                            si->block_port[blk] = port;
                        }
                        SOC_PBMP_PORT_ADD(si->block_bitmap[blk], port);
                    }
                    if ((SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_MS_ISEC) &&
                        (SOC_BLOCK_INFO(unit, blk).number == 0)) {
                        si->ms_isec_block[0] = blk;
                    }
                    if ((SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_MS_ISEC) &&
                        (SOC_BLOCK_INFO(unit, blk).number == 1)) {
                        si->ms_isec_block[1] = blk;
                    }
                    if ((SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_MS_ESEC) &&
                        (SOC_BLOCK_INFO(unit, blk).number == 0)) {
                        si->ms_esec_block[0] = blk;
                    }
                    if ((SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_MS_ESEC) &&
                        (SOC_BLOCK_INFO(unit, blk).number == 1)) {
                        si->ms_esec_block[1] = blk;
                    }
                }
                if (SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_IL) {
                    if (((SOC_BLOCK_INFO(unit, blk).number == 0) && (port == 9))
                        || ((SOC_BLOCK_INFO(unit, blk).number == 1) &&
                        (port == 13))) {
                        if (!SOC_PBMP_MEMBER(si->il.disabled_bitmap, port)) {
                            si->block_valid[blk] += 1;
                        }
                        if (si->block_port[blk] < 0) {
                            si->block_port[blk] = port;
                        }
                        SOC_PBMP_PORT_ADD(si->block_bitmap[blk], port);
                    }
                    if ((SOC_BLOCK_INFO(unit, blk).number == 0)) {
                        si->il_block[0] = blk;
                    }
                    if ((SOC_BLOCK_INFO(unit, blk).number == 1)) {
                        si->il_block[1] = blk;
                    }
                }
                if (SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_CW) {
                    si->block_valid[blk] += 1;
                    if ((SOC_BLOCK_INFO(unit, blk).number == 0)) {
                        si->cw = blk;
                    }
                }
                blk++;
            }
        }
#endif /* BCM_SHADOW_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            if (soc_kt2_linkphy_port_reg_blk_idx_get(unit, port,
                SOC_BLK_TXLP, &blk, NULL) == SOC_E_NONE) {
                /* Is a TXLP Port */
                si->block_valid[blk] += 1;
                if (si->block_port[blk] < 0) {
                    si->block_port[blk] = port;
                }
                SOC_PBMP_PORT_ADD(si->block_bitmap[blk], port);
            }
            if (soc_kt2_linkphy_port_reg_blk_idx_get(unit, port,
                SOC_BLK_RXLP, &blk, NULL) == SOC_E_NONE) {
                /* Is a RXLP Port */
                si->block_valid[blk] += 1;
                if (si->block_port[blk] < 0) {
                    si->block_port[blk] = port;
                }
                SOC_PBMP_PORT_ADD(si->block_bitmap[blk], port);
            }
        }
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_METROLITE_SUPPORT
    if (SOC_IS_METROLITE(unit)) {
        if (soc_ml_iecell_port_reg_blk_idx_get(unit, port,
                SOC_BLK_IECELL, &blk, NULL) == SOC_E_NONE) {
              /* Is a IECELL Port */
            si->block_valid[blk] += 1;
            if (si->block_port[blk] < 0) {
                si->block_port[blk] = port;
            }
            SOC_PBMP_PORT_ADD(si->block_bitmap[blk], port);
       }
   } else
#endif /* BCM_METROLITE_SUPPORT */
#if defined (BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        if (soc_sb2_iecell_port_reg_blk_idx_get(unit, port,
                SOC_BLK_IECELL, &blk, NULL) == SOC_E_NONE) {
              /* Is a IECELL Port */
            si->block_valid[blk] += 1;
            if (si->block_port[blk] < 0) {
                si->block_port[blk] = port;
            }
            SOC_PBMP_PORT_ADD(si->block_bitmap[blk], port);
       }
   }
#endif /* BCM_SABER2_SUPPORT */
#if defined (BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_FIRELIGHT(unit)) {
        if (soc_fl_macsec_port_reg_blk_idx_get(unit, phy_port,
                SOC_BLK_MACSEC, &blk, NULL) == SOC_E_NONE) {
              /* Is a MACSEC Port */
            si->block_valid[blk] += 1;
            if (si->block_port[blk] < 0) {
                si->block_port[blk] = port;
            }
            SOC_PBMP_PORT_ADD(si->block_bitmap[blk], port);
        }
    }
#endif /* BCM_FIRELIGHT_SUPPORT */
    }
    port = max_port + 1;

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        int hp, hgc = 0;
        PBMP_HG_ITER(unit, hp) {
            if (!SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, port), hp)) {
                hgc++;
            }
        }
        if (hgc > _SOC_TR3_MAX_HG_PORT_COUNT) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Port config error (Higig port count[%d] exceeded) !!\n"),
                                  hgc));
            return SOC_E_CONFIG;
        }
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    /* The last ports are the loopback port in the internal MMU ports*/
    if (si->internal_loopback) {
#if defined(BCM_ENDURO_SUPPORT)
        if(SOC_IS_ENDURO(unit)) {
            int port_temp;
            port_temp = port;
            for (port = 1; port <= 29; port++) {
                /* The LB port in Enduro could be port 1 or HG ports, not the last port */
                if (IS_LB_PORT(unit, port)) {
                    ADD_PORT(all, port);
                    si->port_type[port] = SOC_BLK_EPIPE;
                    si->port_offset[port] = 0;
                    sal_snprintf(si->port_name[port], sizeof(si->port_name[port]),
                                 "%s%d", "lb", 0);
                }
            }
            /* Since the LB port in Enduro is port 1, not the last port.
                 recover the port to previous value */
            port = port_temp;
        } else
#endif /* BCM_ENDURO_SUPPORT */
#if defined(BCM_KATANA_SUPPORT)
        if(SOC_IS_SABER2(unit)) {
            int port_temp = port;

            if (!IS_LB_PORT(unit, port) && IS_LB_PORT(unit, port-1)) {
                /* If this is not LB port, Possibly the LB port is added as a
                   regular port in the regfile.. Just check the previous port */
                port_temp -= 1;
            }
            ADD_PORT(all, port_temp);
            si->port_type[port_temp] = SOC_BLK_EPIPE;
            si->port_offset[port_temp] = 0;
            sal_snprintf(si->port_name[port_temp],
                    sizeof(si->port_name[port_temp]), "%s%d", "lb", 0);
            /* Loopback port is the last port. */
            port = port_temp + 1;
        } else if(SOC_IS_KATANAX(unit)) {

            int port_temp;
            port_temp = port;
            for (port = 35; port <= 41; port++) {
                if (IS_LB_PORT(unit, port)) {
                    ADD_PORT(all, port);
                    si->port_type[port] = SOC_BLK_EPIPE;
                    si->port_offset[port] = 0;
                    sal_snprintf(si->port_name[port], sizeof(si->port_name[port]),
                                 "%s%d", "lb", (port - 35));
                }
            }
            port = port_temp;
        } else
#endif /* BCM_KATANA_SUPPORT */
        {
            ADD_PORT(all, port);
            si->port_type[port] = SOC_BLK_EPIPE;
            si->port_offset[port] = 0;
            sal_snprintf(si->port_name[port], sizeof(si->port_name[port]),
                         "%s%d", "lb", 0);
            /* Loopback port is the last port. */
            port++;
        }
    }
    if(!SOC_IS_ENDURO(unit) && !SOC_IS_KATANAX(unit)) {
        if (si->internal_loopback) {
            ADD_PORT(mmu, port);
            si->port_type[port] = SOC_BLK_MMU;
            si->port_offset[port] = 0;
            sal_snprintf(si->port_name[port], sizeof(si->port_name[port]),
                         "%s%d", "mmu", 0);
            port++;
            ADD_PORT(mmu, port);
            si->port_type[port] = SOC_BLK_MMU;
            si->port_offset[port] = 1;
            sal_snprintf(si->port_name[port], sizeof(si->port_name[port]),
                         "%s%d", "mmu", 1);
            port++;
        }
    }
     /* Cache the port count from the above loop.
        NOTE: for future code changes:
              Do not modify the 'port' variable before the below assignment */
    si->port_num = port;

    for (port = 0; port < SOC_MAX_NUM_PP_PORTS; port++) {
        if (SOC_PBMP_MEMBER(pbmp_pp, port)) {
            ADD_PORT(pp, port);
        }
    }
#undef  ADD_PORT

    /* Generate logical to physical and mmu to physical port mapping */
    if (soc_feature(unit, soc_feature_logical_port_num)) {
        for (port = 0; port < phy_port; port++) {
            si->port_l2p_mapping[port] = -1;
            si->port_m2p_mapping[port] = -1;
        }
        for (port = 0; port < phy_port; port++) {
            if (si->port_p2l_mapping[port] != -1) {
                si->port_l2p_mapping[si->port_p2l_mapping[port]] = port;
            }
            if (si->port_p2m_mapping[port] != -1) {
                si->port_m2p_mapping[si->port_p2m_mapping[port]] = port;
            }
        }
        /* Cleanup */
        for (port = 0; port < phy_port; port++) {
            if (si->flex_port_p2l_mapping[port] == -1) {
                si->port_speed_max[si->port_p2l_mapping[port]] = 0;
            }
        }
    }


    /* some things need to be found in the block table */
    si->arl_block = -1;
    si->mmu_block = -1;
    si->mcu_block = -1;

    si->ipipe_block = -1;
    si->ipipe_hi_block = -1;
    si->epipe_block = -1;
    si->epipe_hi_block = -1;
    si->bsafe_block = -1;
    si->esm_block = -1;
    for (blk = 0; blk < SOC_MAX_NUM_OTPC_BLKS; blk++) {
        si->otpc_block[blk] = -1;
    }
    si->axp_block = -1;
    si->top_block = -1;
    si->ism_block = -1;
    si->etu_block = -1;
    si->ibod_block = -1;

    si->igr_block = -1;
    si->egr_block = -1;
    si->bse_block = -1;
    si->cse_block = -1;
    si->hse_block = -1;
    si->iproc_block = -1;
    si->macsec_blocks[0] = -1;
    si->crypto_block = -1;

    sal_memset(si->has_block, 0, sizeof(soc_block_t) * COUNTOF(si->has_block));

    for (blk = 0; blk < SOC_MAX_NUM_CI_BLKS; blk++) {
        si->ci_block[blk] = -1;
    }

    for (blk = 0; SOC_BLOCK_INFO(unit, blk).type >= 0; blk++) {
        blktype = SOC_BLOCK_INFO(unit, blk).type;
        instance = SOC_BLOCK_INFO(unit, blk).number;
        si->has_block[blk] = blktype;
        sal_snprintf(instance_string, sizeof(instance_string), "%d",
                     SOC_BLOCK_INFO(unit, blk).number);
        switch (blktype) {
        case SOC_BLK_ARL:
            si->arl_block = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_MMU:
            si->mmu_block = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_MMU_GLB:
            si->mmu_glb_block = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_MMU_XPE:
            si->mmu_xpe_block = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_MMU_SC:
            si->mmu_sc_block = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_MMU_SED:
            si->mmu_sed_block = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_MMU_ITM:
            si->mmu_itm_block = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_MMU_EB:
            si->mmu_eb_block = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_MCU:
            si->mcu_block = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_IPIPE:
            si->ipipe_block = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_CI:
            si->ci_block[instance] = blk;
            si->block_valid[blk] += 1;
            si->block_port[blk] = (SOC_REG_ADDR_INSTANCE_MASK | instance);
            SOC_PBMP_PORT_ADD(si->block_bitmap[blk], instance);
            break;
        case SOC_BLK_IPIPE_HI:
            si->ipipe_hi_block = blk;
            si->block_valid[blk] += 1;
            SOC_PBMP_OR(si->block_bitmap[blk],PBMP_ST_ALL(unit));
            SOC_PBMP_OR(si->block_bitmap[blk],PBMP_HG_ALL(unit));
            SOC_PBMP_OR(si->block_bitmap[blk],PBMP_CMIC(unit));
            break;
        case SOC_BLK_EPIPE:
            si->epipe_block = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_EPIPE_HI:
            si->epipe_hi_block = blk;
            si->block_valid[blk] += 1;
            SOC_PBMP_OR(si->block_bitmap[blk],PBMP_ST_ALL(unit));
            SOC_PBMP_OR(si->block_bitmap[blk],PBMP_HG_ALL(unit));
            SOC_PBMP_OR(si->block_bitmap[blk],PBMP_CMIC(unit));
            break;
#ifdef BCM_SHADOW_SUPPORT
        case SOC_BLK_BSE: /* Aliased with IL */
            /* coverity[secure_coding] */
            sal_strcpy(si->block_name[blk], "il");
            break;
       case SOC_BLK_CSE: /* Aliased with MS_ISEC */
           sal_strcpy(si->block_name[blk], "ms_isec");
            break;
        case SOC_BLK_HSE: /* Aliased with MS_ESEC */
            sal_strcpy(si->block_name[blk], "ms_esec");
            break;
        case SOC_BLK_SYS: /* Aliased with CW */
            sal_strcpy(si->block_name[blk], "cw");
            break;
#endif /* BCM_SHADOW_SUPPORT */
        case SOC_BLK_BSAFE:
            si->bsafe_block = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_OTPC:
            si->otpc_block[instance] = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_ESM:
            si->esm_block = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_PGW_CL:
            si->pgw_cl_block[instance] = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_MACSEC:
            si->macsec_blocks[0] = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_PORT_GROUP4:
            si->pg4_block[instance] = blk;
            sal_strcpy(instance_string, instance ? "_y" : "_x");
            si->block_port[blk] = (SOC_REG_ADDR_INSTANCE_MASK | instance);
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_PORT_GROUP5:
            si->pg5_block[instance] = blk;
            sal_strcpy(instance_string, instance ? "_y" : "_x");
            si->block_port[blk] = (SOC_REG_ADDR_INSTANCE_MASK | instance);
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_TOP:
            si->top_block = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_SER:
            si->ser_block = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_AVS:
            si->avs_block = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_AXP:
            si->axp_block = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_ISM:
            si->ism_block = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_ETU:
            si->etu_block = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_IBOD:
            si->ibod_block = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_LLS:
            si->lls_block = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_CES:
            si->ces_block = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_CEV:
            si->cev_block = blk;
            si->block_valid[blk] += 1;
            break;
         case SOC_BLK_IPROC:
            si->iproc_block = blk;
            si->block_valid[blk] += 1;
            break;
         case SOC_BLK_CRYPTO:
            si->crypto_block = blk;
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_CDMAC:
            sal_strcpy(si->block_name[blk], "cdmac");
            si->block_valid[blk] += 1;
            break;
        case SOC_BLK_PMQPORT:
        case SOC_BLK_PMQ:
            if(SOC_IS_GREYHOUND(unit)){
                if (si->port_p2l_mapping[2]== -1){
                /* TSC0Q is powered down in some skus,
                it could be tell by checking the valid of physical port 2. */
                    break;
                }
            }
            si->pmq_block[instance] = blk;
            si->block_valid[blk] += 1;
            break;
#ifdef BCM_HURRICANE4_SUPPORT
        case SOC_BLK_QTGPORT:
            si->qtgport_block[instance] = blk;
            si->block_valid[blk] += 1;
            break;
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_HURRICANE3_SUPPORT
        case SOC_BLK_PGW_GE:
            if (SOC_IS_HURRICANE3(unit)) {
                si->pgw_ge_block[instance] = blk;
                sal_strcpy(instance_string, instance ? "_1" : "_0");
                si->block_port[blk] = (SOC_REG_ADDR_INSTANCE_MASK | instance);
                si->block_valid[blk] += 1;
            }
            break;
#endif /* BCM_HURRICANE3_SUPPORT */
#ifdef CANCUN_SUPPORT
        case SOC_BLK_CCH:
            si->block_valid[blk] += 1;
            break;
#endif
        case SOC_BLK_TAF:
            si->taf_block = blk;
            si->block_valid[blk] += 1;
            break;
        }
        sal_snprintf(si->block_name[blk], sizeof(si->block_name[blk]),
                     "%s%s",
                     soc_block_name_lookup_ext(blktype, unit),
                     instance_string);
    }
    si->block_num = blk;

    /*
     * Calculate the mem_block_any array for this configuration
     * The "any" block is just the first one enabled
     */
    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        si->mem_block_any[mem] = -1;
        if (!SOC_MEM_IS_VALID(unit, mem)) {
            continue;
        }
        SOC_MEM_BLOCK_ITER(unit, mem, blk) {
            si->mem_block_any[mem] = blk;
            break;
        }
    }

    switch (drv_dev_id) {
#ifdef BCM_TRIDENT_SUPPORT
    case BCM56840_DEVICE_ID:
        soc_trident_num_cosq_init(unit);
        break;
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    case BCM56860_DEVICE_ID:
    case BCM56850_DEVICE_ID:
        soc_trident2_num_cosq_init(unit);
        break;
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    case BCM56960_DEVICE_ID:
    case BCM56961_DEVICE_ID:
    case BCM56962_DEVICE_ID:
    case BCM56963_DEVICE_ID:
    case BCM56930_DEVICE_ID:
    case BCM56968_DEVICE_ID:
#if defined(BCM_TOMAHAWKPLUS_SUPPORT)
    case BCM56965_DEVICE_ID:
    case BCM56969_DEVICE_ID:
    case BCM56966_DEVICE_ID:
    case BCM56967_DEVICE_ID:
#endif /* BCM_TOMAHAWKPLUS_SUPPORT */
    case BCM56168_DEVICE_ID:
    case BCM56169_DEVICE_ID:
        
        soc_tomahawk_num_cosq_init(unit);
        break;
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWK3_SUPPORT
    case BCM56980_DEVICE_ID:
    case BCM56981_DEVICE_ID:
    case BCM56982_DEVICE_ID:
    case BCM56983_DEVICE_ID:
    case BCM56984_DEVICE_ID:
        PBMP_ALL_ITER(unit, port) {
            soc_tomahawk3_num_cosq_init(unit, port);
        }
        break;
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    case BCM56870_DEVICE_ID:
    case BCM56873_DEVICE_ID:
        soc_trident3_num_cosq_init(unit);
        break;
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    case BCM56275_DEVICE_ID:
        soc_hurricane4_num_cosq_init(unit);
        break;
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    case BCM56370_DEVICE_ID:
        soc_helix5_num_cosq_init(unit);
        break;
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_MAVERICK2_SUPPORT
    case BCM56770_DEVICE_ID:
    case BCM56771_DEVICE_ID:
        soc_maverick2_num_cosq_init(unit);
        break;
#endif /* BCM_MAVERICK2_SUPPORT */
#ifdef BCM_TOMAHAWK2_SUPPORT
    case BCM56970_DEVICE_ID:
    case BCM56971_DEVICE_ID:
    case BCM56972_DEVICE_ID:
    case BCM56974_DEVICE_ID:
    case BCM56975_DEVICE_ID:
        
        soc_tomahawk_num_cosq_init(unit);
        break;
#endif /* BCM_TOMAHAWK2_SUPPORT */

#ifdef BCM_KATANA_SUPPORT
    case BCM56440_DEVICE_ID:
    case BCM56441_DEVICE_ID:
    case BCM56442_DEVICE_ID:
    case BCM56443_DEVICE_ID:
    case BCM56445_DEVICE_ID:
    case BCM56446_DEVICE_ID:
    case BCM56447_DEVICE_ID:
    case BCM56448_DEVICE_ID:
    case BCM56449_DEVICE_ID:
    case BCM56240_DEVICE_ID:
    case BCM56241_DEVICE_ID:
    case BCM56242_DEVICE_ID:
    case BCM56243_DEVICE_ID:
    case BCM56245_DEVICE_ID:
    case BCM56246_DEVICE_ID:
    case BCM55440_DEVICE_ID:
    case BCM55441_DEVICE_ID:
        soc_katana_num_cosq_init(unit);
        break;
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
    case BCM55450_DEVICE_ID:
    case BCM55455_DEVICE_ID:
    case BCM56248_DEVICE_ID:
    case BCM56450_DEVICE_ID:
    case BCM56452_DEVICE_ID:
    case BCM56454_DEVICE_ID:
    case BCM56455_DEVICE_ID:
    case BCM56456_DEVICE_ID:
    case BCM56457_DEVICE_ID:
    case BCM56458_DEVICE_ID:
        SOC_IF_ERROR_RETURN(soc_katana2_num_cosq_init(unit));
        break;
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_SABER2_SUPPORT
    case BCM56260_DEVICE_ID:
    case BCM56261_DEVICE_ID:
    case BCM56262_DEVICE_ID:
    case BCM56263_DEVICE_ID:
    case BCM56265_DEVICE_ID:
    case BCM56266_DEVICE_ID:
    case BCM56267_DEVICE_ID:
    case BCM56268_DEVICE_ID:
    case BCM56233_DEVICE_ID:
    case BCM56460_DEVICE_ID:
    case BCM56461_DEVICE_ID:
    case BCM56462_DEVICE_ID:
    case BCM56463_DEVICE_ID:
    case BCM56465_DEVICE_ID:
    case BCM56466_DEVICE_ID:
    case BCM56467_DEVICE_ID:
    case BCM56468_DEVICE_ID:
        SOC_IF_ERROR_RETURN(soc_saber2_num_cosq_init(unit));
        break;
#endif /* BCM_SABER2_SUPPORT */

#ifdef BCM_METROLITE_SUPPORT
    case BCM56270_DEVICE_ID:
    case BCM56271_DEVICE_ID:
    case BCM56272_DEVICE_ID:

    case BCM53460_DEVICE_ID:
    case BCM53461_DEVICE_ID:

        SOC_IF_ERROR_RETURN(soc_metrolite_num_cosq_init(unit));
        break;
#endif /* BCM_METROLITE_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
    case BCM56560_DEVICE_ID:
        soc_apache_num_cosq_init(unit);
        break;
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_MONTEREY_SUPPORT
    case BCM56670_DEVICE_ID:
    case BCM56671_DEVICE_ID:
    case BCM56672_DEVICE_ID:
    case BCM56675_DEVICE_ID:
        soc_monterey_num_cosq_init(unit);
        break;
#endif /* BCM_APACHE_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
    case BCM56640_DEVICE_ID:
    case BCM56643_DEVICE_ID:
    case BCM56644_DEVICE_ID:
    case BCM56648_DEVICE_ID:
    case BCM56649_DEVICE_ID:
    case BCM56540_DEVICE_ID:
    case BCM56541_DEVICE_ID:
    case BCM56542_DEVICE_ID:
    case BCM56543_DEVICE_ID:
    case BCM56544_DEVICE_ID:
    case BCM56545_DEVICE_ID:
    case BCM56546_DEVICE_ID:
    case BCM56044_DEVICE_ID:
    case BCM56045_DEVICE_ID:
    case BCM56046_DEVICE_ID:
        soc_triumph3_init_num_cosq(unit);
        break;
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_HELIX4_SUPPORT
    case BCM56548H_DEVICE_ID:
    case BCM56548_DEVICE_ID:
    case BCM56547_DEVICE_ID:
    case BCM56346_DEVICE_ID:
    case BCM56345_DEVICE_ID:
    case BCM56344_DEVICE_ID:
    case BCM56342_DEVICE_ID:
    case BCM56340_DEVICE_ID:
    case BCM56049_DEVICE_ID:
    case BCM56048_DEVICE_ID:
    case BCM56047_DEVICE_ID:
    case BCM56042_DEVICE_ID:
    case BCM56041_DEVICE_ID:
    case BCM56040_DEVICE_ID:
        soc_hx4_init_num_cosq(unit);
        break;
#endif
    default:
        break;
    }

    soc_esw_dport_init(unit);


#if defined(BCM_ESW_SUPPORT) && defined(BCM_IPROC_SUPPORT)
    /*
     * Add/Remove features, update soc_control cache based on
     * BondOptions info read from the relevant capability registers
     */
    if (soc_feature(unit, soc_feature_untethered_otp)) {
        soc_info_bondoptions_adjust(unit);
    }
#endif /* BCM_ESW_SUPPORT && BCM_IPROC_SUPPORT */

#ifdef INCLUDE_RCPU
    if (SOC_IS_RCPU_ONLY(unit)) {
        soc_rcpu_cfg_t cfg;
        if (SOC_E_NONE == soc_cm_get_rcpu_cfg(unit, &cfg)) {
            si->rcpu_port = cfg.port;
        } else {
            si->rcpu_port = -1;
        }
    }
#endif /* INCLUDE_RCPU */
    /* Allocate MMU LLS usage map for all valid ports.
     * The code below is moved from soc_attach() to
     * soc_info_config() as soc_attach() is not invoked
     * during subsequent "rc" after first initialization is complete.
     * The move is done to ensure that if the number of ports change
     * due to any config then the MMU LLS usage bmap
     * allocation is accounted for new ports.
     */
#if defined(BCM_KATANA_SUPPORT)
    if (SOC_IS_KATANA(unit)) {
        soc_kt_lls_bmap_alloc(unit);
    }
#endif /* BCM_KATANA_SUPPORT */

#if defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_KATANA2(unit)) {
        soc_kt2_lls_bmap_alloc(unit);
    }
#endif /* BCM_KATANA2_SUPPORT */

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_max_supported_vrf_get
 * Purpose:
 *      Utility routine for reading maximum supported  vrf value for the device.
 * Parameters:
 *      unit - (IN) Unit number.
 *      value  - (OUT) Max vrf value.
 * Returns:
 *      SOC_E_NONE
 */
int
soc_max_supported_vrf_get(int unit, uint16 *value)
{
    int bit_num = 0;
#if defined(BCM_TRIUMPH_SUPPORT)
    uint16              dev_id;
    uint8               rev_id;
#endif /* BCM_TRIUMPH_SUPPORT */

    if (NULL == value) {
        return (SOC_E_PARAM);
    }

#if defined (BCM_ESW_SUPPORT) && defined (BCM_IPROC_SUPPORT)
    if (soc_feature(unit, soc_feature_untethered_otp) &&
        SOC_MEM_IS_VALID(unit, VRFm)) {
        *value = soc_mem_index_max(unit, VRFm);
        return SOC_E_NONE;
    }
#endif

#if defined(BCM_TRIUMPH_SUPPORT)
    soc_cm_get_id(unit, &dev_id, &rev_id);

    if (SOC_IS_TRIUMPH(unit) || SOC_IS_TRIUMPH2(unit)) {
        if (soc_mem_index_count(unit, EXT_IPV6_64_DEFIPm)) {
            bit_num = soc_mem_field_length(unit, EXT_IPV6_64_DEFIPm, VRF_LOf);
        } else {
            bit_num = soc_mem_field_length(unit, L3_IIFm, VRFf);
        }
    } else if (SOC_IS_TRIUMPH3(unit)) {
        bit_num = soc_mem_field_length(unit, L3_IIFm, VRFf);
    } else if (SOC_IS_VALKYRIE(unit)) {
        bit_num = 8; /* Hardcoded to 256 VRFs */
    } else if (SOC_IS_ENDURO(unit)) {
        if ((dev_id == BCM56230_DEVICE_ID) || (dev_id == BCM56231_DEVICE_ID)) {
            bit_num = 5; /* Hardcoded to 32 VRFs for Dagger */
        } else {
            bit_num = 7; /* Hardcoded to 128 VRFs */
        }
    } else if (SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit)) {
        bit_num = soc_mem_field_length(unit, L3_IIFm, VRFf);
    } else if (SOC_IS_MONTEREY(unit)) {
        
        *value = soc_mem_index_max(unit, VRFm);
        return SOC_E_NONE;
    } else if (SOC_IS_TD_TT(unit) || (SOC_IS_KATANAX(unit))) {
        if (dev_id == BCM56233_DEVICE_ID) {
            bit_num = 5; /* Hardcoded to 32 VRFs for Dagger2 */
        } else {
            bit_num = soc_mem_field_length(unit, L3_IIFm, VRFf);
        }
    } else if (SOC_IS_GREYHOUND2(unit)) {
        if (soc_feature(unit, soc_feature_l3)) {
            bit_num = soc_mem_field_length(unit, L3_IIFm, VRF_IDf);
        } else {
            /* to force no VRF support if no L3 feature on some SKUs */
            bit_num = 0;
        }
    }
#endif /* BCM_TRIUMPH_SUPPORT */
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAPTOR_SUPPORT) || \
    defined(BCM_SCORPION_SUPPORT)
    if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit) ||
        SOC_IS_SC_CQ(unit)) {
         bit_num = soc_mem_field_length(unit, VLAN_TABm, VRF_IDf);
    }
#endif  /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_SCORPION_SUPPORT */
    *value = (( 1 << bit_num ) - 1);
    return (SOC_E_NONE);
}


/*
 * Function:
 *      soc_max_supported_addr_class_get
 * Purpose:
 *      Utility routine for reading maximum supported  address
 *      class for the device.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      value  - (OUT) Max address class value.
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
soc_max_supported_addr_class_get(int unit, uint16 *value)
{
    int bit_num = 0;

    if (NULL == value) {
        return (SOC_E_PARAM);
    }
#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
      /* fill in for SHADOW */
    } else
#endif /* BCM_SHADOW_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit)) {
        if (soc_feature(unit, soc_feature_ism_memory)) {
            bit_num = soc_mem_field_length(unit, L2_ENTRY_1m, CLASS_IDf);
        } else {
            bit_num = soc_mem_field_length(unit, L2Xm, CLASS_IDf);
        }
    } else
#endif  /* BCM_TRX_SUPPORT */
#ifdef BCM_ESW_SUPPORT
    if (SOC_MEM_FIELD_VALID(unit, L2Xm, MAC_BLOCK_INDEXf)) {
         bit_num = soc_mem_field_length(unit, L2Xm, MAC_BLOCK_INDEXf);
    }
#endif /* BCM_ESW_SUPPORT */
    *value = (( 1 << bit_num ) - 1);

    return (SOC_E_NONE);
}

/*
 * Function:
 *      soc_max_supported_overlaid_addr_class_get
 * Purpose:
 *      Utility routine for reading maximum supported address
 *      class with overlay for the device.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      value  - (OUT) Max address class value.
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
soc_max_supported_overlaid_addr_class_get(int unit, uint16 *value)
{
    int bit_num = 0;

    if (NULL == value) {
        return (SOC_E_PARAM);
    }

#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, L2Xm, CLASS_IDf)) {
        bit_num = soc_mem_field_length(unit, L2Xm, CLASS_IDf);
        if (SOC_MEM_FIELD_VALID(unit, L2Xm, PRIf)) {
            bit_num += soc_mem_field_length(unit, L2Xm, PRIf);
        }
    }
#endif  /* BCM_TRX_SUPPORT */

    *value = (( 1 << bit_num ) - 1);

    return (SOC_E_NONE);
}

/*
 * Function:
 *      soc_max_supported_extended_addr_class_get
 * Purpose:
 *      Utility routine for reading maximum supported extended address
 *      class for the device.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      value  - (OUT) Max address class value.
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
soc_max_supported_extended_addr_class_get(int unit, uint16 *value)
{
    int bit_num = 0;

    if (NULL == value) {
        return (SOC_E_PARAM);
    }
    if (SOC_MEM_FIELD_VALID(unit, L3_ENTRY_1m, CLASS_IDf)) {
        bit_num = soc_mem_field_length(unit, L3_ENTRY_1m, CLASS_IDf);
    }

    *value = (( 1 << bit_num ) - 1);
    return (SOC_E_NONE);
}

/*
 * Function:
 *      soc_max_supported_intf_class_get
 * Purpose:
 *      Utility routine for reading maximum supported  interface
 *      class for the device.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      value  - (OUT) Maximum  interface class value.
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
soc_max_supported_intf_class_get(int unit, uint16 *value)
{
    int bit_num = 0;

    if (NULL == value) {
        return (SOC_E_PARAM);
    }

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TR_VL(unit)) {
         bit_num = soc_mem_field_length(unit, VLAN_TABm, VLAN_CLASS_IDf);
    }
#endif  /* BCM_TRIUMPH_SUPPORT */
#if defined(BCM_SCORPION_SUPPORT)
    if (SOC_IS_SC_CQ(unit)) {
         bit_num = soc_mem_field_length(unit, VLAN_TABm, CLASS_IDf);
    }
#endif  /* BCM_SCORPION_SUPPORT */
#if defined(BCM_FIREBOLT2_SUPPORT)
    if (SOC_IS_FIREBOLT2(unit)) {
         bit_num = soc_mem_field_length(unit, PORT_TABm, VFP_PORT_GROUP_IDf);
    }
#endif  /* BCM_FIREBOLT2_SUPPORT */
    *value = (( 1 << bit_num ) - 1);
    return (SOC_E_NONE);
}


/*
 * Function:
 *      soc_max_vrf_set
 * Purpose:
 *      Utility routine for configuring max vrf value for the device.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      value  - (IN) Max vrf value.
 * NOTE:  Value must be equal or less than maximum supported vrf id,
 *        otherwise maximum supported vrf id is used.
 *        Value must be power of two - 1, otherwise routine calculates closest
 *        power of 2 - 1 value less than requested one.
 * Returns:
       BCM_E_NONE
 */
int
soc_max_vrf_set(int unit, int value)
{
    uint16 max_supported;   /* Maximum supported vrf id.           */
    uint16 max_vrf_id;      /* Adjusted max vrf value.             */
    uint8  bit_number;      /* Number of bits in configured value. */


    SOC_IF_ERROR_RETURN(soc_max_supported_vrf_get(unit, &max_supported));

    if (value > max_supported) {
       return (SOC_E_PARAM);
    }

    /* Find number of bits required to cover requested value. */
    for (bit_number = 0; bit_number < 16; bit_number++) {
        max_vrf_id = ((1 << bit_number) - 1);
        if (value == max_vrf_id) {
            break;
        }
    }

    if (max_vrf_id != value) {
        return (SOC_E_PARAM);
    }

    SOC_VRF_MAX_SET(unit, max_vrf_id);
    return (SOC_E_NONE);
}

/*
 * Function:
 *      soc_xport_type_verify
 * Purpose:
 *      Check all 10G ports to see if the encapsulation is changed from
 *      the configured selection.  Update the SOC port data structures to
 *      match the HW encapsulation selection.
 * Parameters:
 *      unit - XGS unit #.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      WARM_BOOT mode only
 */
STATIC int
soc_xport_type_verify(int unit)
{
    soc_pbmp_t pbmp_10g_plus;
    soc_port_t port;
    int        mode;
    mac_driver_t *macdp;
#ifdef  BCM_GREYHOUND_SUPPORT
    uint32 hg2_mode = 0;
#endif
#ifdef BCM_HURRICANE3_SUPPORT
    uint32 reg_val = 0;
#endif /* BCM_HURRICANE3_SUPPORT */

    if (!soc_feature(unit, soc_feature_xport_convertible)) {
        /* Nothing to do */
        return SOC_E_NONE;
    }

    if (SOC_USE_PORTCTRL(unit)) {
        /* For portmod devices, hg2 bitmap is recovered
         * in soc_esw_portctr_init */
        return SOC_E_NONE;
    }

    SOC_PBMP_ASSIGN(pbmp_10g_plus, PBMP_XE_ALL(unit));
    SOC_PBMP_OR(pbmp_10g_plus, PBMP_HG_ALL(unit));
    SOC_PBMP_OR(pbmp_10g_plus, PBMP_CE_ALL(unit));

    SOC_PBMP_ITER(pbmp_10g_plus, port) {
        SOC_IF_ERROR_RETURN(soc_mac_probe(unit, port, &macdp));
        
        {
            SOC_IF_ERROR_RETURN(MAC_ENCAP_GET(macdp, unit, port, &mode));
        }

#ifdef  BCM_GREYHOUND_SUPPORT
        if (SOC_IS_GREYHOUND(unit)){
            /* Need more check for MAC mode at IEEE encap mode if the port is
             * actually at HG2-LITE encap mode.
             */
            if (mode == SOC_ENCAP_IEEE){
                SOC_IF_ERROR_RETURN(soc_greyhound_pgw_encap_field_get(unit,
                    port, HIGIG2_MODEf, &hg2_mode));
                mode = (hg2_mode) ? SOC_ENCAP_HIGIG2_LITE : mode;
            }

            if ((IS_XE_PORT(unit, port) && (mode != SOC_ENCAP_IEEE)) ||
                    (IS_HG_PORT(unit, port) && (mode == SOC_ENCAP_IEEE))) {
                soc_xport_type_mode_update(unit, port, mode);
            }
            if (mode == SOC_ENCAP_HIGIG2 || mode == SOC_ENCAP_HIGIG2_LITE) {
                SOC_HG2_ENABLED_PORT_ADD(unit, port);
            }
        } else
#endif  /* GREYHOUND */
#ifdef  BCM_HURRICANE3_SUPPORT
        if (SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)){
            /* Need more check for MAC mode at IEEE encap mode if the port is
             * actually at HG2-LITE encap mode.
             */
            if (mode == SOC_ENCAP_IEEE){
                if (IS_XE_PORT(unit, port)) {
                    if (IS_XL_PORT(unit, port)) {
                        SOC_IF_ERROR_RETURN(
                            READ_PGW_XL_CONFIGr(unit, port, &reg_val));
                        hg2_mode = soc_reg_field_get(unit,
                            PGW_XL_CONFIGr, reg_val, HIGIG2_MODEf);
                    } else if (IS_CL_PORT(unit, port)) {
                        SOC_IF_ERROR_RETURN(
                            READ_PGW_CL_CONFIGr(unit, port, &reg_val));
                        hg2_mode = soc_reg_field_get(unit,
                            PGW_CL_CONFIGr, reg_val, HIGIG2_MODEf);
                    } else {
                        /* This should not happen */
                        return SOC_E_INTERNAL;
                    }
                    mode = (hg2_mode) ? SOC_ENCAP_HIGIG2_LITE : mode;
                }
            }

            if ((IS_XE_PORT(unit, port) && (mode != SOC_ENCAP_IEEE)) ||
                    (IS_HG_PORT(unit, port) && (mode == SOC_ENCAP_IEEE))) {
                soc_xport_type_mode_update(unit, port, mode);
            }
            if (mode == SOC_ENCAP_HIGIG2 || mode == SOC_ENCAP_HIGIG2_LITE) {
                SOC_HG2_ENABLED_PORT_ADD(unit, port);
            }
        } else
#endif  /* HURRICANE3 */
#ifdef  BCM_SABER2_SUPPORT
        if (SOC_IS_SABER2(unit)) {
            /* Need more check for MAC mode at IEEE encap mode if the port is
             * actually at HG2-LITE encap mode.
             */
            uint32 hg2_mode = 0;
            uint32 reg_val = 0;
            if (mode == SOC_ENCAP_IEEE){
                if (SOC_REG_PORT_VALID(unit, XLPORT_CONFIGr, port)) {
                    READ_XLPORT_CONFIGr(unit, port, &reg_val);
                    hg2_mode = soc_reg_field_get(unit,
                            XLPORT_CONFIGr, reg_val, HIGIG2_MODEf);
                    mode = (hg2_mode) ? SOC_ENCAP_HIGIG2_LITE : mode;
                }
            }
            if ((IS_XE_PORT(unit, port) && (mode != SOC_ENCAP_IEEE)) ||
                (IS_HG_PORT(unit, port) && ((mode == SOC_ENCAP_IEEE) ||
                                           (mode == SOC_ENCAP_HIGIG2_LITE)))) {
                soc_sb2_xport_type_update(unit, port, mode);
            }
            if (mode == SOC_ENCAP_HIGIG2 || mode == SOC_ENCAP_HIGIG2_LITE) {
                if (IS_HG_PORT(unit, port)) {
                    SOC_HG2_ENABLED_PORT_ADD(unit, port);
                } else {
                    /* This should not happen */
                    return SOC_E_INTERNAL;
                }
            }
        } else
#endif  /* SABER2 */
#ifdef  BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit) && !SOC_IS_SABER2(unit)) {
            /* Need more check for MAC mode at IEEE encap mode if the port is
             * actually at HG2-LITE encap mode.
             */
            uint32 hg2_mode = 0;
            uint32 reg_val = 0;
            if (mode == SOC_ENCAP_IEEE) {
                if (SOC_REG_PORT_VALID(unit, XPORT_CONFIGr, port)) {
                    SOC_IF_ERROR_RETURN(
                        READ_XPORT_CONFIGr(unit, port, &reg_val));
                    hg2_mode = soc_reg_field_get(unit,
                            XPORT_CONFIGr, reg_val, HIGIG2_MODEf);
                    mode = (hg2_mode) ? SOC_ENCAP_HIGIG2_LITE : mode;
                }
            }

            if ((IS_XE_PORT(unit, port) && (mode != SOC_ENCAP_IEEE)) ||
                (IS_HG_PORT(unit, port) && ((mode == SOC_ENCAP_IEEE) ||
                                           (mode == SOC_ENCAP_HIGIG2_LITE)))) {
                soc_kt2_xport_type_update(unit, port, mode);
            }
            if ((mode == SOC_ENCAP_HIGIG2) || (mode == SOC_ENCAP_HIGIG2_LITE)) {
                if (IS_HG_PORT(unit, port)) {
                    SOC_HG2_ENABLED_PORT_ADD(unit, port);
                } else {
                    /* This should not happen */
                    return SOC_E_INTERNAL;
                }
            }
        } else
#endif  /* KATANA2 */
        {
            if ((IS_XE_PORT(unit, port) || IS_CE_PORT(unit, port))
                 && (mode != SOC_ENCAP_IEEE)) {
                soc_xport_type_update(unit, port, TRUE);
            } else if (IS_HG_PORT(unit, port) && (mode == SOC_ENCAP_IEEE)) {
                soc_xport_type_update(unit, port, FALSE);
            }

            if (mode == SOC_ENCAP_HIGIG2) {
                if (IS_HG_PORT(unit, port)) {
                    SOC_HG2_ENABLED_PORT_ADD(unit, port);
                } else {
                    /* This should not happen */
                    return SOC_E_INTERNAL;
                }
            }
        }
    }

    return SOC_E_NONE;
}

#if defined(BCM_RAVEN_SUPPORT) || defined (BCM_TRIUMPH2_SUPPORT)
/*
 * Function:
 *      soc_stport_type_verify
 * Purpose:
 *      Check all stackable ports to see if the encapsulation is
 *      changed from the configured selection.  Update the SOC port
 *      data structures to match the HW encapsulation selection.
 * Parameters:
 *      unit - XGS unit #.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      WARM_BOOT mode only
 */
STATIC int
soc_stport_type_verify(int unit)
{
    port_tab_entry_t ptab;
    soc_port_t       port;
    soc_info_t       *si = &SOC_INFO(unit);
    soc_reg_t        ehg_rx_reg;
    soc_field_t      port_type;
    uint32           ehg_rx_ctrl = 0;
    int              to_higig = FALSE;
    soc_mem_t        mem;
    ing_physical_port_table_entry_t ptab_entry;
    ing_device_port_entry_t ptab_dev;
    void             *pentry;
    int              port_type_value = 0;

    if (SOC_USE_PORTCTRL(unit)) {
        /* For portmod devices, this is covered in soc_xport_type_update */
        return SOC_E_NONE;
    }

    /* Select correct port table field for the Higig2 configuration. */
    port_type = (SOC_IS_ENDURO(unit) || SOC_IS_TRIUMPH2(unit) ||
                 SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit)  ||
                 SOC_IS_HURRICANE(unit) || SOC_IS_HURRICANE2(unit) ||
                 SOC_IS_TD_TT(unit) ||
                 SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||
                 SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND(unit) ||
                 SOC_IS_GREYHOUND2(unit)) ?
                 PORT_TYPEf : HIGIG_PACKETf;

    ehg_rx_reg =
        SOC_IS_TRIUMPH3(unit) ? PORT_EHG_RX_CONTROLr : EHG_RX_CONTROLr;

    PBMP_PORT_ITER(unit, port) {
        if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
            SOC_IS_GREYHOUND2(unit)) {
            if (IS_XE_PORT(unit, port) || IS_CE_PORT(unit, port) ||
                IS_HG_PORT(unit, port)) {
                /* these cases were handled in soc_xport_type_verify */
                continue;
            }
        }
        /* Handle embedded Higig cases */
        if (SOC_REG_IS_VALID(unit, ehg_rx_reg)) {
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, ehg_rx_reg, port, 0, &ehg_rx_ctrl));

            if (2 == soc_reg_field_get(unit, ehg_rx_reg,
                                       ehg_rx_ctrl, MODEf)) {
                /* Raptor-style stacking -- HIGIG2_LITE */
                if (IS_HG_PORT(unit, port)) {
                    SOC_HG2_ENABLED_PORT_REMOVE(unit, port);
                } else {
                    SOC_PBMP_PORT_ADD(si->hl.bitmap, port);
                }
            }

            if (0 != soc_reg_field_get(unit, ehg_rx_reg,
                                       ehg_rx_ctrl, MODEf)) {
                /* Embedded Higig */
                SOC_PBMP_PORT_ADD(si->st.bitmap, port);
                SOC_PBMP_PORT_REMOVE(si->ether.bitmap, port);
                continue;
            }
            /* Else, fall through to below */
        }
        if (SOC_IS_KATANA2(unit)) {
            mem = ING_PHYSICAL_PORT_TABLEm;
            sal_memset(&ptab_entry, 0, sizeof(ptab_entry));
            pentry = (void *)&ptab_entry;
        } else if (SOC_IS_TRIDENT3X(unit)) {
            mem = ING_DEVICE_PORTm;
            sal_memset(&ptab_dev, 0, sizeof(ptab_dev));
            pentry = (void *)&ptab_dev;
        } else {
            mem = PORT_TABm;
            sal_memset(&ptab, 0, sizeof(ptab));
            pentry = (void *)&ptab;
        }

        if (!IS_HG_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem,
                                             MEM_BLOCK_ANY, port, pentry));
            port_type_value = soc_mem_field32_get(unit, mem, pentry, port_type);
            to_higig = ((0 != port_type_value) &&
                       (_BCM_COE_PORT_TYPE_CASCADED != port_type_value));

            /* ST port settings */
            if (to_higig) {
                SOC_PBMP_PORT_ADD(si->st.bitmap, port);
                SOC_PBMP_PORT_ADD(si->hl.bitmap, port);
                SOC_PBMP_PORT_REMOVE(si->ether.bitmap, port);
                if (!IS_XE_PORT(unit, port)) {
                    SOC_HG2_ENABLED_PORT_ADD(unit, port);
                }
            } else {
                SOC_PBMP_PORT_REMOVE(si->st.bitmap, port);
                SOC_PBMP_PORT_REMOVE(si->hl.bitmap, port);
                SOC_PBMP_PORT_ADD(si->ether.bitmap, port);
                if (!IS_XE_PORT(unit, port)) {
                    SOC_HG2_ENABLED_PORT_REMOVE(unit, port);
                }
            }
        }
    }
    soc_dport_map_update(unit);
    return SOC_E_NONE;
}
#endif /* defined(BCM_RAVEN_SUPPORT) || defined (BCM_TRIUMPH2_SUPPORT) */

/*
 * Function:
 *      soc_port_type_verify
 * Purpose:
 *      Check all ports to see if the encapsulation is changed from
 *      the configured selection.  Update the SOC port data structures to
 *      match the HW encapsulation selection.
 * Parameters:
 *      unit - XGS unit #.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      WARM_BOOT mode only
 */
int
soc_port_type_verify(int unit)
{
    SOC_IF_ERROR_RETURN(soc_xport_type_verify(unit));

#if defined(BCM_RAVEN_SUPPORT) || defined (BCM_TRIUMPH2_SUPPORT)
    /* Raven HG or embedded HG versions here */
    SOC_IF_ERROR_RETURN(soc_stport_type_verify(unit));
#endif /* defined(BCM_RAVEN_SUPPORT) || defined (BCM_TRIUMPH2_SUPPORT) */

    return SOC_E_NONE;
}

#ifdef BCM_CMICM_SUPPORT
int
cmicm_pcie_deemphasis_set(int unit, uint16 phy_addr) {
    uint16  phy_reg_val;
    uint32  cmic_reg_val;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Changing the CMICm PCIe SerDes deemphasis on unit %d\n"), unit));

    /* Change the transmitter setting */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, 0x8610));
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x17, &phy_reg_val));
    phy_reg_val = (phy_reg_val & 0xf0ff) | 0x700;
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x17, phy_reg_val));

    /* Force the PCIe link to retrain */
    cmic_reg_val = 0x1004;
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_PCIE_CFG_ADDRESSr(unit, cmic_reg_val));
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_PCIE_CFG_READ_DATAr(unit, &cmic_reg_val));
    cmic_reg_val &= ~0x4000;
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_PCIE_CFG_WRITE_DATAr(unit, cmic_reg_val));
    cmic_reg_val |= 0x4000;
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_PCIE_CFG_WRITE_DATAr(unit, cmic_reg_val));
    cmic_reg_val &= ~0x4000;
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_PCIE_CFG_WRITE_DATAr(unit, cmic_reg_val));

    /* Wait a short while for the retraining to complete */
    sal_usleep(1000);

    return SOC_E_NONE;
}
#endif /* BCM_CMICM_SUPPORT */

int cmic_pcie_cdr_bw_adj(int unit, uint16 phy_addr) {
    int miim_intr_enb;

    /* Interrupts may not be working yet */
    miim_intr_enb = SOC_CONTROL(unit)->miimIntrEnb;
    SOC_CONTROL(unit)->miimIntrEnb = 0;

    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, 0x8630));
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x13, 0x190));
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x19, 0x191));

    SOC_CONTROL(unit)->miimIntrEnb = miim_intr_enb;

    return SOC_E_NONE;
}

int
cmice_pcie_deemphasis_set(int unit, uint16 phy_addr) {
    uint16  phy_reg_val;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Changing the CMICe PCIe SerDes deemphasis on unit %d\n"), unit));

    /* Change the transmitter setting */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, 0x8610));
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x17, &phy_reg_val));
    phy_reg_val = (phy_reg_val & 0xf0ff) | 0x700;
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x17, phy_reg_val));

    /* Wait a short while */
    sal_usleep(1000);

    return SOC_E_NONE;
}

#ifdef BCM_IPROC_SUPPORT
int iproc_pcie_cdr_bw_adj(int unit, uint16 phy_addr) {
    uint32  cmic_reg_val;

    /* Enable the iProc internal MDIO interface */
    cmic_reg_val = 0;
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_CONTROLr, &cmic_reg_val,
                      MDCDIVf, 0x7f);
    SOC_IF_ERROR_RETURN(WRITE_CHIPCOMMONB_MII_MANAGEMENT_CONTROLr(unit,
                      cmic_reg_val));
    sal_usleep(10000);

    /* Set Block */
    cmic_reg_val = 0;
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, SBf, 0x1);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, TAf, 0x2);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, OPf, 0x1);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, PAf, phy_addr);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, RAf, 0x1f);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, DATAf, 0x8630);
    SOC_IF_ERROR_RETURN(WRITE_CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr(unit,
                      cmic_reg_val));
    sal_usleep(10000);

    /* Write reg 0x13 */
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, RAf, 0x13);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, DATAf, 0x190);
    SOC_IF_ERROR_RETURN(WRITE_CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr(unit,
                      cmic_reg_val));
    sal_usleep(10000);

    /* Write reg 0x19 */
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, RAf, 0x19);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, DATAf, 0x191);
    SOC_IF_ERROR_RETURN(WRITE_CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr(unit,
                      cmic_reg_val));
    sal_usleep(10000);

    return SOC_E_NONE;
}

int
iproc_pcie_deemphasis_set(int unit, uint16 phy_addr) {
    uint16  phy_reg_val;
    uint32  cmic_reg_val;

#ifdef BCM_HURRICANE2_SUPPORT
    /* DeEmphasis not needed for Hurricane2 */
    if (SOC_IS_HURRICANE2(unit)) {
        return SOC_E_NONE;
    }
#endif

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Changing the iProc PCIe SerDes deemphasis on unit %d\n"), unit));

    /* Enable the iProc internal MDIO interface */
    cmic_reg_val = 0;
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_CONTROLr, &cmic_reg_val,
                      MDCDIVf, 0x7f);
    SOC_IF_ERROR_RETURN(WRITE_CHIPCOMMONB_MII_MANAGEMENT_CONTROLr(unit,
                      cmic_reg_val));
    sal_usleep(10000);

    /* Change the transmitter setting */
    cmic_reg_val = 0;
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, SBf, 0x1);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, TAf, 0x2);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, OPf, 0x1);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, PAf, phy_addr);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, RAf, 0x1f);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, DATAf, 0x8610);
    SOC_IF_ERROR_RETURN(WRITE_CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr(unit,
                      cmic_reg_val));
    sal_usleep(10000);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, OPf, 0x2);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, RAf, 0x17);
    SOC_IF_ERROR_RETURN(WRITE_CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr(unit,
                      cmic_reg_val));
    sal_usleep(10000);
    SOC_IF_ERROR_RETURN(READ_CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr(unit,
                      &cmic_reg_val));
    sal_usleep(10000);
    phy_reg_val = soc_reg_field_get(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      cmic_reg_val, DATAf);
    phy_reg_val = (phy_reg_val & 0xf0ff) | 0x700;
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, SBf, 0x1);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, TAf, 0x2);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, OPf, 0x1);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, PAf, phy_addr);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, RAf, 0x17);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, DATAf, phy_reg_val);
    SOC_IF_ERROR_RETURN(WRITE_CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr(unit,
                      cmic_reg_val));

    /* Force the PCIe link to retrain */
    if (soc_cm_get_bus_type(unit) & SOC_DEV_BUS_ALT) {
        /* PCI-1 */
        cmic_reg_val = 0;
        soc_reg_field_set(unit, PAXB_1_CONFIG_IND_ADDRr, &cmic_reg_val,
                          PROTOCOL_LAYERf, 0x2);
        soc_reg_field_set(unit, PAXB_1_CONFIG_IND_ADDRr, &cmic_reg_val,
                          ADDRESSf, 0x4);
        SOC_IF_ERROR_RETURN(WRITE_PAXB_1_CONFIG_IND_ADDRr(unit, cmic_reg_val));
        SOC_IF_ERROR_RETURN(READ_PAXB_1_CONFIG_IND_DATAr(unit, &cmic_reg_val));
        cmic_reg_val &= ~0x4000;
        SOC_IF_ERROR_RETURN(WRITE_PAXB_1_CONFIG_IND_ADDRr(unit, cmic_reg_val));
        cmic_reg_val |= 0x4000;
        SOC_IF_ERROR_RETURN(WRITE_PAXB_1_CONFIG_IND_ADDRr(unit, cmic_reg_val));
        cmic_reg_val &= ~0x4000;
        SOC_IF_ERROR_RETURN(WRITE_PAXB_1_CONFIG_IND_ADDRr(unit, cmic_reg_val));
    } else {
        /* PCI-0 */
        cmic_reg_val = 0;
        soc_reg_field_set(unit, PAXB_0_CONFIG_IND_ADDRr, &cmic_reg_val,
                          PROTOCOL_LAYERf, 0x2);
        soc_reg_field_set(unit, PAXB_0_CONFIG_IND_ADDRr, &cmic_reg_val,
                          ADDRESSf, 0x4);
        SOC_IF_ERROR_RETURN(WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, cmic_reg_val));
        SOC_IF_ERROR_RETURN(READ_PAXB_0_CONFIG_IND_DATAr(unit, &cmic_reg_val));
        cmic_reg_val &= ~0x4000;
        SOC_IF_ERROR_RETURN(WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, cmic_reg_val));
        cmic_reg_val |= 0x4000;
        SOC_IF_ERROR_RETURN(WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, cmic_reg_val));
        cmic_reg_val &= ~0x4000;
        SOC_IF_ERROR_RETURN(WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, cmic_reg_val));
    }
    /* Wait a short while for the retraining to complete */
    sal_usleep(1000);

    return SOC_E_NONE;
}
#endif /* BCM_IPROC_SUPPORT */

#if defined(BCM_HELIX4_SUPPORT)
/*
 * Function:
 *      iproc_pcie_common_clk_set
 * Purpose:
 *      Disable clock compensation events when Helix4 is operated in EHOST mode,
 *      PCIe EP, x2 mode with common clock.
 *      Assumes: BUS type is PCI, Device is Helix4.
 * Parameters:
 *      unit - XGS unit
 *      phy_addr - MDIO Address of the PCIE Phy
 * Returns:
 *      SOC_E_XXX
 */
int iproc_pcie_common_clk_set(int unit, uint16 phy_addr) {
    static uint16   hx4_pcic_link_stat_reg = 0xbc;
    static uint32   hx4_pcic_link_width_mask = 0x03f00000;
    uint32          cmic_reg_val;

    if ((soc_pci_conf_read(unit, hx4_pcic_link_stat_reg) &
         hx4_pcic_link_width_mask) < 0x00200000) {
        /* Helix4 is using a x1 link width */
        return SOC_E_NONE;
    }

    /* All conditions are met.  Apply the change */

    /* Enable the iProc internal MDIO interface */
    cmic_reg_val = 0;
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_CONTROLr, &cmic_reg_val,
                      MDCDIVf, 0x7f);
    SOC_IF_ERROR_RETURN(WRITE_CHIPCOMMONB_MII_MANAGEMENT_CONTROLr(unit,
                      cmic_reg_val));
    sal_usleep(10000);

    /* Set Block to 0x8630 */
    cmic_reg_val = 0;
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, SBf, 0x1);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, TAf, 0x2);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, OPf, 0x1);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, PAf, phy_addr);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, RAf, 0x1f);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, DATAf, 0x8630);
    SOC_IF_ERROR_RETURN(WRITE_CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr(unit,
                      cmic_reg_val));
    sal_usleep(10000);

    /* Write reg 0x1e to disable clock compensation events */
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, RAf, 0x1e);
    soc_reg_field_set(unit, CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr,
                      &cmic_reg_val, DATAf, 0x0801);
    SOC_IF_ERROR_RETURN(WRITE_CHIPCOMMONB_MII_MANAGEMENT_COMMAND_DATAr(unit,
                      cmic_reg_val));
    sal_usleep(10000);

    return SOC_E_NONE;
}
#endif


#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)
STATIC int
soc_pcie_host_intf_init(int unit)
{
    uint32  rval, val;
    if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) {
        if (soc_feature(unit, soc_feature_cmicm)) {
            /* Set the timeout for bus-mastered DMA transactions */
            if (SOC_REG_IS_VALID(unit, CMIC_PCIE_USERIF_TIMEOUTr) &&
                SOC_REG_FIELD_VALID(unit, CMIC_PCIE_USERIF_TIMEOUTr, TIMEOUTf)) {
                val = soc_property_get(unit, spn_PCIE_HOST_INTF_TIMEOUT_USEC,
                                       50000000);
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "HOST_INTF_TIMEOUT_USEC = %u\n"), val));
                SOC_IF_ERROR_RETURN(READ_CMIC_PCIE_USERIF_TIMEOUTr(unit, &rval));
                soc_reg_field_set(unit, CMIC_PCIE_USERIF_TIMEOUTr, &rval, TIMEOUTf,
                                  val);
                /* coverity[result_independent_of_operands] */
                SOC_IF_ERROR_RETURN(WRITE_CMIC_PCIE_USERIF_TIMEOUTr(unit, rval));
            }
            if (SOC_REG_IS_VALID(unit, CMIC_PCIE_USERIF_PURGE_CONTROLr) &&
                SOC_REG_FIELD_VALID(unit, CMIC_PCIE_USERIF_PURGE_CONTROLr,
                          ENABLE_PURGE_IF_USERIF_TIMESOUTf)) {
                val = soc_property_get(unit,
                                       spn_PCIE_HOST_INTF_TIMEOUT_PURGE_ENABLE, 1);
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "HOST_INTF_TIMEOUT_PURGE_ENABLE = %u\n"), val));
                SOC_IF_ERROR_RETURN(
                    READ_CMIC_PCIE_USERIF_PURGE_CONTROLr(unit, &rval));
                soc_reg_field_set(unit, CMIC_PCIE_USERIF_PURGE_CONTROLr, &rval,
                                      ENABLE_PURGE_IF_USERIF_TIMESOUTf, val);
                soc_reg_field_set(unit, CMIC_PCIE_USERIF_PURGE_CONTROLr, &rval,
                                  ENABLE_PURGE_IF_USERIF_RESETf, 1);
                soc_reg_field_set(unit, CMIC_PCIE_USERIF_PURGE_CONTROLr, &rval,
                                  ENABLE_PIO_PURGE_IF_USERIF_RESETf, 1);
                /* coverity[result_independent_of_operands] */
                SOC_IF_ERROR_RETURN(
                    WRITE_CMIC_PCIE_USERIF_PURGE_CONTROLr(unit, rval));
            }
        }

        /* Only support pcie hot swap for all cmicx devices and monterey */
        if (soc_feature(unit, soc_feature_cmicx) ||
            (soc_feature(unit, soc_feature_cmicm) && SOC_IS_MONTEREY(unit))) {
            /* Enable Hot Swap manager to handle CPU hot swap or Warmboot case */
            if (SOC_REG_IS_VALID(unit, PAXB_0_PAXB_HOTSWAP_CTRLr) &&
                SOC_REG_FIELD_VALID(unit, PAXB_0_PAXB_HOTSWAP_CTRLr, ENABLEf)) {
                SOC_IF_ERROR_RETURN(READ_PAXB_0_PAXB_HOTSWAP_CTRLr(unit, &rval));
                soc_reg_field_set(unit, PAXB_0_PAXB_HOTSWAP_CTRLr, &rval, ENABLEf, 1);
                SOC_IF_ERROR_RETURN(
                    WRITE_PAXB_0_PAXB_HOTSWAP_CTRLr(unit, rval));
            }
        }

#ifdef BCM_IPROC_SUPPORT
        /* Disable iProc reset on PCie link down event */
        if (SOC_REG_IS_VALID(unit, PAXB_0_RESET_ENABLE_IN_PCIE_LINK_DOWNr)) {
            SOC_IF_ERROR_RETURN
                     (WRITE_PAXB_0_RESET_ENABLE_IN_PCIE_LINK_DOWNr(unit, 0x0));
        }
#endif
    }
    return SOC_E_NONE;
}
#endif


/*
 * Function:
 *      soc_irq_handler_suspend
 * Purpose:
 *      Interrupt handler returns if suspend flag is set.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      suspend - Boolean, if TRUE, suspend.
 * Returns:
 *      SOC_E_XXX
 */
static int
soc_irq_handler_suspend(int unit, int suspend)
{
    int s;

    s = sal_splhi();
    SOC_CONTROL(unit)->irq_handler_suspend = suspend;
    sal_spl(s);

    return 0;
}

/*
 * Function:
 *      soc_do_init
 * Purpose:
 *      Optionally reset, and initialize a StrataSwitch.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      reset - Boolean, if TRUE, device is reset.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Resets the SOC chip using the SCP reset provided by CMIC.
 *      Fields in soc_control_t are initialized to correspond to a SOC
 *      post-reset.  SOC interrupts are configured for normal chip
 *      operation.
 *
 *      This routine may be called after a device is attached
 *      or whenever a chip reset is required.
 *
 * IMPORTANT NOTE:
 *      Only the quickest, most basic things should be initialized here.
 *      This routine may NOT do anything time consuming like scanning ports,
 *      clearing counters, etc.  Such things should be done as commands in
 *      the rc script.  Otherwise, Verilog and Quickturn simulations take
 *      too long.
 */
int
soc_do_init(int unit, int reset)
{
    soc_control_t       *soc;
    soc_persist_t       *sop;
    soc_mem_t           mem;
    uint32              reg;
    uint16              max_class;
    uint16              mem_clear_chunk_size;
    int                 rv, blk, cache;
    int                 copyno = COPYNO_ALL;
#ifdef BCM_WARM_BOOT_SUPPORT
    int                 offset = 0;
    int                 num_ipv6_128b_entries = 0;
    uint32              rval = 0;
#endif
#if !defined(SOC_NO_NAMES)
#if defined(BCM_ESW_SUPPORT) || \
    defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
    uint8               chk_skip_cache = 0;
    char                mem_name[128];
    char                *mptr;
#endif
#endif
    soc_ipg_t           *soc_ipg;
    soc_pbmp_t          pbmp;
    soc_port_t          port;

    if (!SOC_UNIT_VALID(unit)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_init: unit %d not valid\n"), unit));
        return SOC_E_UNIT;
    }

    soc = SOC_CONTROL(unit);
    sop = SOC_PERSIST(unit);

    if (!(soc->soc_flags & SOC_F_ATTACHED)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_init: unit %d not attached\n"), unit));
        return(SOC_E_UNIT);
    }



#if defined(BCM_RCPU_SUPPORT) && defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_RCPU_ONLY(unit)) {
        soc_rcpu_init(unit);
    }
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        _bcm_esw_ibod_sync_recovery_stop(unit);
    }
#endif

    /* Cancel all DPCs belonging to a specified owner, and then
     * disable a specified owner to execute DPCs.
     * Soc initialization is not done, so DPCs can't be executed.
     * Although the DPCs can't be executed, but they can be reported.
     * After Soc initialization is done, the DPCs will be executed.
     */
    sal_dpc_cancel_and_disable(INT_TO_PTR(unit));

    /* Initialize PCI Host interface */
    if (!SAL_BOOT_QUICKTURN) {
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)
        SOC_IF_ERROR_RETURN(soc_pcie_host_intf_init(unit));

        /* Check for PCI hot swap and recover */
        /* Only support pcie hot swap for all cmicx devices and monterey */
        if (soc_feature(unit, soc_feature_cmicx) ||
            (soc_feature(unit, soc_feature_cmicm) && SOC_IS_MONTEREY(unit))) {
            SOC_IF_ERROR_RETURN(soc_pcie_hot_swap_handling(unit, 0));
        }
#endif
    }

    /***********************************************************************/
    /* If the device has already been initialized before, perform some     */
    /* de-initialization to avoid stomping on existing activities.         */
    /***********************************************************************/
    if (SOC_WARM_BOOT(unit)) {
        pbmp_t empty_pbm;

#if defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_TOMAHAWK_SUPPORT)
        /* Reinit latency filters */
        if (SOC_IS_TOMAHAWKX(unit)) {
            SOC_IF_ERROR_RETURN(soc_th_latency_wb_reinit(unit, 1));
        }
#endif
        /* Cleanup old PCIe link failure based issues */
#if defined(BCM_CMICM_SUPPORT) && defined(BCM_SBUSDMA_SUPPORT)
        if (soc_feature(unit, soc_feature_cmicm) &&
            soc_feature(unit, soc_feature_sbusdma)) {
            int cmc = SOC_PCI_CMC(unit);
            int isValidRead = 0;
            reg = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT1_OFFSET(cmc));
#ifdef BCM_TOMAHAWK2_SUPPORT
            if (SOC_IS_TOMAHAWK2(unit)) {
                /* Tomahawk2 got these obsolete fields removed */
            } else
#endif
#ifdef BCM_SABER2_SUPPORT
            if (SOC_IS_SABER2(unit)) {
                if (soc_reg_field_get(unit, CMIC_RPE_IRQ_STAT1r, reg,
                                  PCIEINTF_NEEDS_CLEANUPf) ||
                        soc_reg_field_get(unit, CMIC_CMC0_IRQ_STAT1r, reg,
                                  PCIE_ECRC_ERR_INTRf)) {
                    isValidRead = 1;
                }
            } else
#endif
            {
                if (soc_reg_field_valid
                        (unit, CMIC_CMC0_IRQ_STAT1r, PCIEINTF_NEEDS_CLEANUPf)) {
                    if (soc_reg_field_get(unit, CMIC_CMC0_IRQ_STAT1r, reg,
                               PCIEINTF_NEEDS_CLEANUPf) ||
                               soc_reg_field_get(unit, CMIC_CMC0_IRQ_STAT1r, reg,
                               PCIE_ECRC_ERR_INTRf)) {
                        isValidRead = 1;
                    }
                } else {
                    if (soc_reg_field_get(unit, CMIC_CMC0_IRQ_STAT1r, reg,
                               PCIE_ECRC_ERR_INTRf)) {
                        isValidRead = 1;
                    }
                }
            }

            if(isValidRead) {
                /* coverity[result_independent_of_operands] */
                SOC_IF_ERROR_RETURN
                    (READ_CMIC_PCIE_USERIF_STATUS_CLRr(unit, &reg));
                soc_reg_field_set(unit, CMIC_PCIE_USERIF_STATUS_CLRr, &reg,
                                  USERIF_TIMEDOUTf, 1);
                soc_reg_field_set(unit, CMIC_PCIE_USERIF_STATUS_CLRr, &reg,
                                  USERIF_RESETf, 1);
                /* coverity[result_independent_of_operands] */
                SOC_IF_ERROR_RETURN
                    (WRITE_CMIC_PCIE_USERIF_STATUS_CLRr(unit, reg));
                reg = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, 0));
                soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &reg,
                                  ABORTf, 1);
                soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, 0), reg);
                reg = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, 1));
                soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &reg,
                                  ABORTf, 1);
                soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, 1), reg);
                reg = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, 2));
                soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &reg,
                                  ABORTf, 1);
                soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, 2), reg);

                SOC_IF_ERROR_RETURN(_soc_mem_sbus_fifo_dma_stop(unit, 0));
                SOC_IF_ERROR_RETURN(_soc_mem_sbus_fifo_dma_stop(unit, 1));
                SOC_IF_ERROR_RETURN(_soc_mem_sbus_fifo_dma_stop(unit, 2));
                SOC_IF_ERROR_RETURN(_soc_mem_sbus_fifo_dma_stop(unit, 3));
            }
            SOC_IF_ERROR_RETURN(READ_CMIC_PCIE_USERIF_PURGE_STATUSr(unit, &reg));
            if (reg) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "PCIE purge clear 0x%08x.\n"), reg));
                /* coverity[result_independent_of_operands] */
                SOC_IF_ERROR_RETURN(WRITE_CMIC_PCIE_USERIF_PURGE_CONTROLr(unit, 0));
            }
        }
#endif /* BCM_CMICM_SUPPORT && BCM_SBUSDMA_SUPPORT */

        if (!SOC_IS_RCPU_ONLY(unit)) {
            rv = soc_dma_abort(unit);

            if (SOC_FAILURE(rv)) {
                SOC_IF_ERROR_RETURN(soc_event_generate(unit,
                                SOC_SWITCH_EVENT_UNCONTROLLED_SHUTDOWN,
                                0, 0, 0));
            }
        }
        /* Assume HW linkscan is running, shut it down to allow
         * PHY probes.  Linkscan will turn it back on when needed. */

        /* Set linkscan enable flag, it will be turned off by the
         * empty bitmap config call. */
        soc->soc_flags |= SOC_F_LSE;
        SOC_PBMP_CLEAR(empty_pbm);

        SOC_IF_ERROR_RETURN
            (soc_linkscan_config(unit, empty_pbm, empty_pbm));
    } else {
        /* Cold Boot case */
#ifdef BCM_WARM_BOOT_SUPPORT
        SOC_IF_ERROR_RETURN(soc_scache_dictionary_alloc(unit));
#if defined(BCM_METROLITE_SUPPORT)
        if (SOC_IS_METROLITE(unit)) {
            SOC_IF_ERROR_RETURN(_soc_ml_flexio_scache_allocate(unit));
        } else
#endif
#ifdef BCM_SABER2_SUPPORT
        if (SOC_IS_SABER2(unit)) {
            SOC_IF_ERROR_RETURN(_soc_saber2_flexio_scache_allocate(unit));
        } else
#endif
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            SOC_IF_ERROR_RETURN(_soc_katana2_flexio_scache_allocate(unit));
        }
#endif
#endif
    }

    /***********************************************************************/
    /* Always be sure device has correct endian configuration before       */
    /* touching registers - device may not have been configured yet.       */
    /***********************************************************************/

    if (!SOC_WARM_BOOT(unit) && !SOC_IS_RCPU_ONLY(unit)) {
        soc_endian_config(unit);
        soc_pci_ep_config(unit, -1);
    }

    /* Stop the UC & UC msging */
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)
    if (!SOC_WARM_BOOT(unit)) {
        int reset_uC = TRUE;

#ifdef BCM_TOMAHAWK3_SUPPORT
        /* For TH3, bring down portmod to make sure that the
         * 400G AN state machine is stopped. Otherwise, it will continue
         * trying to access the serdes when portmod isn't up yet
         */
        if (SOC_IS_TOMAHAWK3(unit)) {
            SOC_IF_ERROR_RETURN(soc_esw_portctrl_deinit(unit));
        }
#endif

        if (soc_feature(unit, soc_feature_uc)) {
#if defined (BCM_IPROC_SUPPORT)
            if (soc_feature(unit, soc_feature_iproc)) {
#if defined (BCM_UC_MHOST_SUPPORT)
                if (!soc_feature(unit, soc_feature_uc_mhost))
#endif /* BCM_UC_MHOST_SUPPORT */
                 {
                     if (soc_cm_get_bus_type(unit) & SOC_AXI_DEV_TYPE) {
                         /* SDK is running iHost on A9 uC, no EAPPS are running,
                          * don't reset the uC
                          */
                         reset_uC = FALSE;
                     }
                 }
            }
#endif /* BCM_IPROC_SUPPORT */
        }
        else {
            reset_uC = FALSE;
        }

        if (reset_uC) {
            soc_cmic_uc_msg_shutdown_halt(unit);
        }
    }
#endif /* BCM_CMICM_SUPPORT || BCM_IPROC_SUPPORT */

    /* Stop iproc m0 subsystem */
#ifdef BCM_CMICX_SUPPORT
    if (!SOC_WARM_BOOT(unit)) {
        if (soc_feature(unit, soc_feature_cmicx) && !(SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM)) {
            /* Stop LED FW */
            soc_cmicx_led_enable(unit, LED_STOP);
            /* Iproc M0 exit */
            soc_iproc_m0_exit(unit);
        }
    }
#endif

    if (soc->soc_flags & SOC_F_INITED) {
        if (!SOC_IS_RCPU_ONLY(unit)) {
            SOC_IF_ERROR_RETURN(soc_dma_abort(unit)); /* Turns off/clean up DMA */
        }
        SOC_IF_ERROR_RETURN(soc_counter_stop(unit)); /* Stop counter collection */
#ifdef INCLUDE_MEM_SCAN
        SOC_IF_ERROR_RETURN(soc_mem_scan_stop(unit)); /* Stop memory scanner */
#endif
#ifdef BCM_SRAM_SCAN_SUPPORT
        SOC_IF_ERROR_RETURN(soc_sram_scan_stop(unit)); /* Stop sram scanner */
#endif
#ifdef  INCLUDE_I2C
        SOC_IF_ERROR_RETURN(soc_i2c_detach(unit)); /* Free up I2C driver mem */
#endif

#ifdef BCM_XGS_SWITCH_SUPPORT
        if (soc_feature(unit, soc_feature_arl_hashed)) {
            /* Stop L2X thread */
            SOC_IF_ERROR_RETURN(soc_l2x_stop(unit));
        }
#ifdef BCM_TRIUMPH3_SUPPORT
        if SOC_IS_TRIUMPH3(unit) {
            SOC_IF_ERROR_RETURN(soc_tr3_l2_bulk_age_stop(unit));
        }
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
        if SOC_IS_TRIDENT2X(unit) {
            SOC_IF_ERROR_RETURN(soc_td2_l2_bulk_age_stop(unit));
        }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWK(unit) || SOC_IS_TOMAHAWK2(unit) ||
            SOC_IS_TRIDENT3X(unit)) {
            SOC_IF_ERROR_RETURN(soc_th_l2_bulk_age_stop(unit));
        }
#endif /* BCM_TOMAHAWK_SUPPORT */
#endif /* BCM_XGS_SWITCH_SUPPORT */

        if (soc->arlShadow != NULL) {
            sal_mutex_take(soc->arlShadowMutex, sal_mutex_FOREVER);
            shr_avl_delete_all(soc->arlShadow);
            sal_mutex_give(soc->arlShadowMutex);
        }

#ifdef ALPM_ENABLE
#if defined(BCM_TOMAHAWK_SUPPORT)
        if (soc_property_get(unit, spn_L3_ALPM_ENABLE, 0) &&
            soc_feature(unit, soc_feature_alpm2)) {
            SOC_IF_ERROR_RETURN(alpm_dist_hitbit_thread_stop(unit));
        }
#endif
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            SOC_IF_ERROR_RETURN(soc_th3_l2_age_stop(unit));

            /* Stop learn thread (if it's running) */
            SOC_IF_ERROR_RETURN(soc_th3_l2_learn_thread_stop(unit));

            /* Free AVL table used for L2 learning */
            if ((soc->l2x_lrn_shadow != NULL) &&
                (soc->l2x_lrn_shadow_mutex != NULL)) {
                sal_mutex_take(soc->l2x_lrn_shadow_mutex, sal_mutex_FOREVER);
                shr_avl_delete_all(soc->l2x_lrn_shadow);
                sal_mutex_give(soc->l2x_lrn_shadow_mutex);

                shr_avl_destroy(soc->l2x_lrn_shadow);
                soc->l2x_lrn_shadow = NULL;
                sal_mutex_destroy(soc->l2x_lrn_shadow_mutex);
                soc->l2x_lrn_shadow_mutex = NULL;
            }

            (void)soc_th3_l2x_appl_callback_entry_delete_all(unit);
            (void)soc_th3_l2x_appl_callback_resources_destroy(unit);
        }
#endif /* BCM_TOMAHAWK3_SUPPORT */

        /* Destroy egress metering mutex. */
        if (soc->egressMeteringMutex != NULL) {
            sal_mutex_destroy(soc->egressMeteringMutex);
            soc->egressMeteringMutex = NULL;
        }

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_KATANA2_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_APACHE_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_KATANAX(unit) || SOC_IS_TRIDENT2X(unit) ||
            SOC_IS_TITAN2X(unit) || SOC_IS_TRIUMPH3(unit)) {
            /* Destroy llsMutex */
            if (soc->llsMutex !=NULL) {
                sal_mutex_destroy(soc->llsMutex);
                soc->llsMutex = NULL;
            }
        }
#endif

#if defined(BCM_TOMAHAWK_SUPPORT)
        if (SOC_IS_TOMAHAWKX(unit)) {
            /* Destroy idb_lock */
            if (soc->idb_lock !=NULL) {
                sal_mutex_destroy(soc->idb_lock);
                soc->idb_lock = NULL;
            }
        }
#endif
        soc->soc_flags &= ~SOC_F_INITED;
    }
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT)
         if (soc_feature(unit, soc_feature_epc_linkflap_recover)) {
            /* Destroy linkscan_modport_map_lock_lock */
            if (soc->linkscan_modport_map_lock !=NULL) {
                sal_mutex_destroy(soc->linkscan_modport_map_lock);
                soc->linkscan_modport_map_lock = NULL;
            }
         }
#endif /* #(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT) */

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        rv = soc_scache_recover(unit);
        if (SOC_FAILURE(rv)) {
            /* Fall back to Level 1 Warm Boot recovery */
            SOC_IF_ERROR_RETURN(soc_stable_size_set(unit, 0));
            SOC_IF_ERROR_RETURN
                (soc_stable_set(unit, _SHR_SWITCH_STABLE_NONE, 0));
            /* Error report */
            SOC_IF_ERROR_RETURN
                (soc_event_generate(unit, SOC_SWITCH_EVENT_STABLE_ERROR,
                                    SOC_STABLE_CORRUPT,
                                    SOC_STABLE_FLAGS(unit), 0));
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "Corrupt stable cache.\n")));
        } else {
            SOC_IF_ERROR_RETURN(soc_mem_cache_scache_get(unit));
            SOC_IF_ERROR_RETURN(soc_scache_dictionary_recover(unit));
#if defined(BCM_METROLITE_SUPPORT)
            if (SOC_IS_METROLITE(unit)) {
                SOC_IF_ERROR_RETURN(_soc_ml_flexio_scache_retrieve(unit));
            } else
#endif
#ifdef BCM_SABER2_SUPPORT
            if (SOC_IS_SABER2(unit)) {
                SOC_IF_ERROR_RETURN(_soc_saber2_flexio_scache_retrieve(unit));
            } else
#endif
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                SOC_IF_ERROR_RETURN(_soc_katana2_flexio_scache_retrieve(unit));
            }
#endif
        }
    }
#endif

    /* Clear previous phy configs if any */
    SOC_IF_ERROR_RETURN(soc_phyctrl_software_deinit(unit));

    /* Suspend IRQ activity during soc_info_config */
    soc_irq_handler_suspend(unit, TRUE);

    /* Set bitmaps according to which type of device it is */
    SOC_IF_ERROR_RETURN(soc_info_config(unit, soc));

    /* Resume IRQ activity */
    soc_irq_handler_suspend(unit, FALSE);

    /* Set feature cache */
    soc_feature_init(unit);
    soc_dcb_unit_init(unit);

    /* Initial LED driver. */
    soc_led_driver_init(unit);

#if defined(BCM_ESW_SUPPORT) && defined(BCM_IPROC_SUPPORT)
    /*
     * Add/Remove features, update soc_control cache based on
     * Bon Options info read from the relevant capability registers
     */
    if (soc_feature(unit, soc_feature_untethered_otp)) {
        soc_info_bondoptions_adjust(unit);
    }
#endif /* BCM_ESW_SUPPORT && BCM_IPROC_SUPPORT */


#ifdef BCM_CB_ABORT_ON_ERR
    SOC_CB_ABORT_ON_ERR(unit) = soc_property_get(unit, spn_CB_ABORT_ON_ERR, 0);
#endif

    if (!SOC_IS_RCPU_ONLY(unit) && !(soc->soc_flags & SOC_F_INITED) &&
        (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) &&
        !(SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM)) {
        uint32 cfg_dw = 0;
        uint8  cap_id = 0, base_ptr = 0;
        soc_info_t *si = &SOC_INFO(unit);
        uint16 pcie_phy_addr = si->pcie_phy_addr;
        uint8 change = SOC_IS_TD2P_TT2P(unit) ? FALSE : TRUE;

        /* Device has not been initialized... */

        /*********************************************************************/
        /* Disable clock compensation Events for Helix4 PCIe in EP, x2 mode  */
        /*********************************************************************/
#if defined(BCM_HELIX4_SUPPORT)
        if (SOC_IS_HELIX4(unit)) {
            SOC_IF_ERROR_RETURN(iproc_pcie_common_clk_set(unit, pcie_phy_addr));
        }
#endif

        /*********************************************************************/
        /* PCIe SerDes Gen1/Gen2 CDR Track Bandwidth Adjustment              */
        /* for Better Jitter Tolerance.                                      */
        /*********************************************************************/
        if (pcie_phy_addr != 0) {
#ifdef BCM_IPROC_SUPPORT
            if (soc_feature(unit, soc_feature_iproc)) {
                SOC_IF_ERROR_RETURN(iproc_pcie_cdr_bw_adj(unit, pcie_phy_addr));
            } else
#endif
            {
                SOC_IF_ERROR_RETURN(cmic_pcie_cdr_bw_adj(unit, pcie_phy_addr));
            }
        }

        if (change) {
            /*********************************************************************/
            /* Check to see if the PCIe SerDes deemphasis needs to be changed    */
            /* based on the advertisement from the root complex                  */
            /*********************************************************************/
            /*Find PCIe capability base */
            base_ptr = soc_pci_conf_read(unit, 0x34) & 0xff;
            while ((base_ptr >= 0x40) && (base_ptr < 0xff)) {
                cfg_dw = soc_pci_conf_read(unit, base_ptr);
                cap_id = cfg_dw & 0xff;
                if (cap_id == 0x10) break;
                base_ptr = (cfg_dw >> 8) & 0xff;
            }
            /* Check for PCIe Gen2 -3.5dB advertisement */
            if ((cap_id == 0x10) &&
                ((soc_pci_conf_read(unit, base_ptr + 0x10) & 0xf0000) == 0x20000) &&
                (soc_pci_conf_read(unit, base_ptr + 0x30) & 0x00010000)) {
                /* Device is operating at Gen2 speeds and RC requested -3.5dB */

                if (pcie_phy_addr != 0) {
#ifdef BCM_IPROC_SUPPORT
                    if (soc_feature(unit, soc_feature_iproc)) {
                        SOC_IF_ERROR_RETURN(iproc_pcie_deemphasis_set(unit, pcie_phy_addr));
                    } else
#endif
#ifdef BCM_CMICM_SUPPORT
                    if (soc_feature(unit, soc_feature_cmicm)) {
                        SOC_IF_ERROR_RETURN(cmicm_pcie_deemphasis_set(unit, pcie_phy_addr));
                    } else
#endif
                    {
                        SOC_IF_ERROR_RETURN(cmice_pcie_deemphasis_set(unit, pcie_phy_addr));
                    }
                }
            }
        }
    }

    /***********************************************************************/
    /* Always enable bursting before doing any more reads or writes        */
    /***********************************************************************/

    if (!SOC_WARM_BOOT(unit) && !SOC_IS_RCPU_ONLY(unit)) {
        soc_pci_burst_enable(unit);
    }

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_iproc_14_tx_arbiter_priority)) {
        SOC_IF_ERROR_RETURN(soc_cmicx_paxb_tx_arbiter_set(unit, PAXB_TX_REQ_SEQ_EN,
                                                      PAXB_TX_ARB_PRIORITY));
    }
#endif

    /***********************************************************************/
    /* Begin initialization from a known state (reset).                    */
    /***********************************************************************/

    /*
     * PHY drivers and ID map
     */

    
#if 0
    if (!SOC_USE_PORTCTRL(unit)) {
    SOC_IF_ERROR_RETURN(soc_phyctrl_software_init(unit));
    }
#else
    SOC_IF_ERROR_RETURN(soc_phyctrl_software_init(unit));
#endif

    /* NB:  The PHY init must be before fusioncore reset, which is next. */

    if (reset && !SOC_WARM_BOOT(unit) && !SOC_IS_RCPU_ONLY(unit)) {
        SOC_IF_ERROR_RETURN(soc_reset(unit));
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)){
        SOC_IF_ERROR_RETURN(soc_warmboot_reset(unit));
    }
#endif

    if (SOC_WARM_BOOT(unit) && soc_feature(unit, soc_feature_esm_support)) {
#ifdef BCM_TRIUMPH_SUPPORT
        SOC_IF_ERROR_RETURN(soc_triumph_esm_init_read_config(unit));
#endif
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit) && SOC_IS_HELIX4(unit)) {
        SOC_IF_ERROR_RETURN(READ_TOP_SWITCH_FEATURE_ENABLEr(unit, &rval));
        if (!(rval & 0x8)) {
            /* Disabling MPLS feature during warm boot for HX4, as it is
               disabled in cold boot by default */
            SOC_FEATURE_CLEAR(unit, soc_feature_mpls);
            SOC_FEATURE_CLEAR(unit, soc_feature_bhh);
        }
    }
#endif

    /* Turn-off soc_feature_riot if feature disabled thru soc property */
    if (soc_feature(unit, soc_feature_riot) &&
       (0 == soc_property_get(unit, spn_RIOT_ENABLE, 0))) {
        SOC_FEATURE_CLEAR(unit, soc_feature_riot);
    }

    /*
     * Update saved chip state to reflect values after reset.
     */
#ifdef BCM_XGS_SUPPORT
    if (SOC_IS_XGS(unit)) {
        SOC_MEM_CLEAR_HW_ACC_SET(unit, 1);
    }
#endif /* BCM_XGS_SUPPORT */
    soc->l3_defip_urpf = 0;
    soc->soc_flags &= (SOC_F_RESET | SOC_F_RCPU_ONLY);
#ifdef BCM_XGS_SUPPORT
    if (SOC_IS_XGS(unit)) {
        SOC_MEM_CLEAR_HW_ACC_SET(unit, 1);
    }
#endif /* BCM_XGS_SUPPORT */
    sop->debugMode = 0;
    soc->pciParityDPC = 0;
    soc->pciFatalDPC = 0;
    soc->stat.ser_err_int = 0;
    soc->stat.ser_err_fifo = 0;
    soc->stat.ser_err_tcam = 0;
    soc->stat.ser_err_nak = 0;
    soc->stat.ser_err_stat = 0;
    soc->stat.ser_err_ecc = 0;
    soc->stat.ser_err_corr = 0;
    soc->stat.ser_err_clear = 0;
    soc->stat.ser_err_restor = 0;
    soc->stat.ser_err_spe = 0;
    soc->stat.ser_err_reg = 0;
    soc->stat.ser_err_mem = 0;
    soc->stat.ser_err_sw = 0;
    if (soc_feature(unit, soc_feature_ser_error_stat)) {
        sal_memset(soc->stat.ser_err, 0, sizeof(soc->stat.ser_err));
        sal_memset(soc->stat.ser_tcam_err, 0, sizeof(soc->stat.ser_tcam_err));
    }
    /*
     * Reassign default IPG since all_pbmp might be changed during
     * init process after soc_reset() with config property change on
     * SKU/option related.
     *
     * Note :
     *  - Default IPG setting is assigned during soc_attach().
     *  - SOC/BCM layer reinit may change all_pbmp and this causes some
     *    port's defult IPG setting is not assigned properly.
     */
    SOC_PBMP_CLEAR(pbmp);
    if (SOC_IS_KATANA2(unit)) {
        for (port =  SOC_INFO(unit).cmic_port;
             port <= SOC_INFO(unit).lb_port;
             port++) {

            SOC_PBMP_PORT_ADD(pbmp, port);
        }
    } else {
        SOC_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
    }

    BCM_PBMP_ITER(pbmp, port) {
        soc_ipg = &sop->ipg[port];
        soc_ipg->hd_10    = SOC_AVERAGE_IPG_IEEE;
        soc_ipg->hd_100   = SOC_AVERAGE_IPG_IEEE;
        soc_ipg->hd_1000  = SOC_AVERAGE_IPG_IEEE;
        soc_ipg->hd_2500  = SOC_AVERAGE_IPG_IEEE;

        soc_ipg->fd_10    = SOC_AVERAGE_IPG_IEEE;
        soc_ipg->fd_100   = SOC_AVERAGE_IPG_IEEE;
        soc_ipg->fd_1000  = SOC_AVERAGE_IPG_IEEE;
        soc_ipg->fd_2500  = SOC_AVERAGE_IPG_IEEE;
        soc_ipg->fd_10000 = SOC_AVERAGE_IPG_IEEE;
        soc_ipg->fd_xe    = SOC_AVERAGE_IPG_IEEE;
        soc_ipg->fd_hg    = SOC_AVERAGE_IPG_HG;
        soc_ipg->fd_hg2   = SOC_AVERAGE_IPG_IEEE;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    /* Recover stored Level 2 Warm Boot cache */
    /* The stable and stable size must be selected first */
    if (SOC_WARM_BOOT(unit)) {
        /* Set urpf mode early for mem cache reload to work properly */
        if (soc_feature(unit, soc_feature_urpf) &&
            (soc_feature(unit, soc_feature_l3_defip_map) ||
             soc_feature(unit, soc_feature_l3_defip_hole))) {
            SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(unit, &reg));
            if (soc_reg_field_get(unit, L3_DEFIP_RPF_CONTROLr,
                                  reg, DEFIP_RPF_ENABLEf)) {
                SOC_CONTROL(unit)->l3_defip_urpf = TRUE;
            }
        }
    }
    if (soc_feature(unit, soc_feature_l3_shared_defip_table) &&
        (SOC_IS_TD2_TT2(unit) || SOC_IS_TRIUMPH3(unit))) {
        SOC_IF_ERROR_RETURN(soc_control_defip_scache_init(unit));
    }

    if (SOC_WARM_BOOT(unit)) {
        if (soc_feature(unit, soc_feature_l3_shared_defip_table) &&
            (SOC_IS_TD2_TT2(unit) || SOC_IS_TRIUMPH3(unit))) {
            num_ipv6_128b_entries = SOC_L3_DEFIP_MAX_128B_ENTRIES(unit);
            rv = soc_control_defip_scache_load(unit, &num_ipv6_128b_entries);
            if (SOC_SUCCESS(rv) && SOC_MEM_IS_VALID(unit, L3_DEFIP_PAIR_128m)) {
                if (num_ipv6_128b_entries != SOC_L3_DEFIP_MAX_128B_ENTRIES(unit)) {
                    /* Taking lock is not required as we are in soc init */
                    soc_defip_tables_resize(unit, num_ipv6_128b_entries);
                }
            }
        }
    }
#ifdef BCM_UTT_SUPPORT
    if (SOC_WARM_BOOT(unit) &&
        soc_feature(unit, soc_feature_utt)) {
        SOC_IF_ERROR_RETURN(soc_utt_ifp_bank_config(unit));
    }
#endif /* BCM_UTT_SUPPORT */
#endif /* BCM_WARM_BOOT_SUPPORT */

    /*
     * Initialize memory table status.
     */
    cache = soc_feature(unit, soc_feature_mem_cache) &&
            soc_property_get(unit, spn_MEM_CACHE_ENABLE,
                             (SAL_BOOT_RTLSIM || SAL_BOOT_XGSSIM) ? 0 : 1);
#if !defined(SOC_NO_NAMES)
#if defined(BCM_ESW_SUPPORT) || \
    defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)

    chk_skip_cache = soc_property_get(unit, spn_MEM_CHECK_NOCACHE_OVERRIDE, 0);
#endif
#endif
    soc->stat.mem_cache_count = 0;
    soc->stat.mem_cache_size = 0;
    soc->stat.mem_cache_vmap_size = 0;
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_HELIX4_SUPPORT) || \
    defined(BCM_KATANA2_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT) || \
    defined(BCM_GREYHOUND_SUPPORT)
    soc->stat.tcam_corrupt_map_size = 0;
#endif

    for (mem = 0; mem < NUM_SOC_MEM; mem++) {

        uint8 skip_mem = 0;
        soc_mem_t act_mem = mem;

        if (!SOC_MEM_IS_VALID(unit, mem)) {
            continue;
        }
        if (soc_mem_index_count(unit, mem) == 0) {
            continue;
        }
        sal_memset(sop->memState[mem].count, 0,
                   sizeof (sop->memState[mem].count));
#ifdef BCM_ESW_SUPPORT
        _SOC_DRV_MEM_REUSE_MEM_STATE_NO_LOCK(unit, act_mem);
#endif
        if (act_mem != mem) {
            continue;
        }
        SOC_MEM_BLOCK_ITER(unit, mem, blk) {
            copyno = blk;
            break;
        }

#ifdef BCM_HELIX5_SUPPORT
        if (SOC_IS_HELIX5(unit) && mem == BSC_AG_AGE_TABLEm) {
            SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_READONLY;
        }
#endif

        if ((SOC_BLOCK_TYPE(unit, copyno) == SOC_BLK_ETU) ||
            (SOC_BLOCK_TYPE(unit, copyno) == SOC_BLK_ESM)) {
            SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_CACHABLE;
            skip_mem = 1;
        }
        /* Skip IESMIF mems when ESM is not present */
        else if (SOC_BLOCK_TYPE(unit, copyno) == SOC_BLK_IPIPE) {
#ifdef BCM_TRIUMPH3_SUPPORT
            if (SOC_IS_TRIUMPH3(unit) && /* Needs update per chip */
                SOC_MEM_ADDR_STAGE_EXTENDED(SOC_MEM_BASE(unit, mem)) == 0x9) {
                if (!soc_feature(unit, soc_feature_esm_support) ||
                     SAL_BOOT_QUICKTURN) {
                    SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_CACHABLE;
                    skip_mem = 1;
                }
            }
#endif /* BCM_TRIUMPH3_SUPPORT */
        }
#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit)) {
            /* Force cache L2 mem */
            if (mem == L2_ENTRY_1m || mem == L2_ENTRY_2m) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE;
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_SER_SPECIAL; 
            }
            if (SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_CACHABLE;
            }
            /* Skip caching unused views */
            if (mem == ESM_PKT_TYPE_ID_ONLYm || mem == ESM_PKT_TYPE_ID_DATA_ONLYm) {
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_CACHABLE;
                skip_mem = 1;
            }
            if (mem == MODPORT_MAP_M0m || mem == MODPORT_MAP_M1m ||
                mem == MODPORT_MAP_M2m || mem == MODPORT_MAP_M3m) {
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_CACHABLE;
                skip_mem = 1;
            }
            if (mem == DLB_LAG_FLOWSETm || mem == DLB_LAG_FLOWSET_TIMESTAMP_PAGEm) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_ENTRY_CLEAR;
            }
            if (mem == DLB_ECMP_FLOWSETm || mem == DLB_ECMP_FLOWSET_TIMESTAMP_PAGEm) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_ENTRY_CLEAR;
            }
            if (mem == MMU_WRED_PORT_SP_SHARED_COUNTm ||
                mem == MMU_WRED_QGROUP_SHARED_COUNTm ||
                mem == MMU_WRED_UC_QUEUE_TOTAL_COUNTm) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_ENTRY_CLEAR;
            }
        }
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
        if (SOC_IS_TD_TT(unit) || SOC_IS_TD2_TT2(unit)) {
            int i;
            static soc_mem_t mskip[] = {
                L3_ENTRY_IPV4_MULTICAST_Xm,
                L3_ENTRY_IPV4_MULTICAST_Ym,
                L3_ENTRY_IPV6_UNICAST_Xm,
                L3_ENTRY_IPV6_UNICAST_Ym
            };
            for (i=0; i<COUNTOF(mskip); i++) {
                if (mem == mskip[i]) {
                    SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_CACHABLE;
                    skip_mem = 1;
                    break;
                }
            }
            /* Force cache L2 mem */
            if (mem == L2Xm) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE;
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_SER_ENTRY_CLEAR;
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_SER_SPECIAL; 
            }
            if (mem == DLB_HGT_FLOWSET_PORTm ) {
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_SER_SPECIAL;
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE;
            }
            if (SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_CACHABLE;
            }
            /* Skip caching unused views */
            if (mem == ING_SNAT_ONLYm || mem == ING_SNAT_DATA_ONLYm ||
                mem == FP_GLOBAL_MASK_TCAMm ||
                mem == FP_GLOBAL_MASK_TCAM_Xm ||
                mem == FP_GLOBAL_MASK_TCAM_Ym ||
                mem == L2_ENTRY_LPm || mem == L3_ENTRY_LPm ||
                mem == VLAN_XLATE_LPm || mem == EGR_VLAN_XLATE_LPm ||
                mem == VLAN_SUBNET_DATA_ONLYm) {
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_CACHABLE;
                skip_mem = 1;
            }
            if (mem == MODPORT_MAP_M0m || mem == MODPORT_MAP_M1m ||
                mem == MODPORT_MAP_M2m || mem == MODPORT_MAP_M3m) {
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_CACHABLE;
                skip_mem = 1;
            }
        }
#endif /* BCM_TRIDENT_SUPPORT */
        if (SOC_IS_TRIDENT3(unit) && soc_feature(unit, soc_feature_alpm2)) {
            if (mem == L3_DEFIP_ALPM_IPV4m || mem == L3_DEFIP_ALPM_IPV4_1m ||
                mem == L3_DEFIP_ALPM_IPV6_64m || mem == L3_DEFIP_ALPM_IPV6_64_1m ||
                mem == L3_DEFIP_ALPM_IPV6_128m || mem == L3_DEFIP_AUX_TABLEm) {
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE;
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
                skip_mem = 1;
            }
        }

#ifdef BCM_TRIDENT2_SUPPORT
        if (SOC_IS_TD2_TT2(unit)) {
            if (mem == L3_DEFIP_ALPM_IPV4m || mem == L3_DEFIP_ALPM_IPV4_1m ||
                mem == L3_DEFIP_ALPM_IPV6_64m || mem == L3_DEFIP_ALPM_IPV6_64_1m ||
                mem == L3_DEFIP_ALPM_IPV6_128m || mem == L3_DEFIP_ALPM_RAWm) {
                if (NUM_PIPE(unit) != 1 &&
                    !soc_property_get(unit, "l3_alpm_cache_enable", 0)) {
                    SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE;
                    SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
                    skip_mem = 1;
                } else {
                    if (soc_feature(unit, soc_feature_alpm_flex_stat) &&
                        soc_property_get(unit, spn_ALPM_FLEX_STAT_SUPPORT, 0)) {
                        if (mem == L3_DEFIP_ALPM_IPV4m ||
                            mem == L3_DEFIP_ALPM_IPV6_64m) {
                            SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE;
                            SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
                            skip_mem = 1;
                        }
                    } else {
                        if (mem == L3_DEFIP_ALPM_IPV4_1m ||
                            mem == L3_DEFIP_ALPM_IPV6_64_1m) {
                            SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE;
                            SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
                            skip_mem = 1;
                        }
                    }
                }
            }
            if (mem == L3_DEFIP_PAIR_128m || mem == L3_DEFIPm) {
                if (NUM_PIPE(unit) != 1 &&
                    !soc_property_get(unit, "l3_alpm_cache_enable", 0)) {
                    /*
                     * For multi-pipe devices, always skip caching for these mems.
                     * For single-pipe devices like Apache, always use caching.
                     */
                    SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE;
                    SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
                    skip_mem = 1;
                }
            }
            if (mem == L3_DEFIP_AUX_TABLEm) {
                if (NUM_PIPE(unit) != 1 &&
                    !soc_property_get(unit, "l3_alpm_cache_enable", 0)) {
                    SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_CACHABLE;
                    skip_mem = 1;
                }
            }

            if (mem == MMU_CTR_MC_DROP_MEM0m || mem == MMU_CTR_MC_DROP_MEM1m ||
            	  mem == MMU_CTR_COLOR_DROP_MEMm || mem == MMU_CTR_ING_DROP_MEMm ||
            	  mem == MMU_CTR_MTRI_DROP_MEMm || mem == MMU_CTR_UC_DROP_MEMm ) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_ENTRY_CLEAR;
                skip_mem = 1;
            }
            if (mem == MMU_THDM_DB_QUEUE_BST_0m || mem == MMU_THDM_DB_QUEUE_BST_1m ||
                mem == MMU_THDM_MCQE_QUEUE_BST_0m || mem == MMU_THDM_MCQE_QUEUE_BST_1m ||
                mem == MMU_THDM_DB_PORTSP_BST_0m || mem == MMU_THDM_DB_PORTSP_BST_1m ||
                mem == MMU_THDM_MCQE_PORTSP_BST_0m || mem == MMU_THDM_MCQE_PORTSP_BST_1m) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_ENTRY_CLEAR;
            }
            if (mem == RH_HGT_FLOWSETm) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_CACHABLE;
            }
            if (mem == DLB_HGT_FLOWSETm || mem == DLB_HGT_FLOWSET_Xm ||
                mem == DLB_HGT_FLOWSET_Ym) {
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_SER_SPECIAL;
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE;
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_CACHABLE;
            }
            if (mem == MMU_WRED_UC_QUEUE_TOTAL_COUNT_X_PIPEm ||
                mem == MMU_WRED_UC_QUEUE_TOTAL_COUNT_Y_PIPEm ||
                mem == MMU_WRED_PORT_SP_SHARED_COUNT_X_PIPEm ||
                mem == MMU_WRED_PORT_SP_SHARED_COUNT_Y_PIPEm ||
                mem == MMU_WRED_QGROUP_SHARED_COUNT_X_PIPEm ||
                mem == MMU_WRED_QGROUP_SHARED_COUNT_Y_PIPEm) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_ENTRY_CLEAR;
            }
        }
        if ((SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2PLUS(unit))) {
            if (mem == UDF_CONDITIONAL_CHECK_TABLE_CAMm) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
            }
            if (mem == MMU_INTFI_XPIPE_FC_MAP_TBL0m ||
                mem == MMU_INTFI_XPIPE_FC_MAP_TBL1m ||
                mem == MMU_INTFI_YPIPE_FC_MAP_TBL0m ||
                mem == MMU_INTFI_YPIPE_FC_MAP_TBL1m) {
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_CACHABLE;
                skip_mem = 1;
            }
        }
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT)
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_VALKYRIE2(unit)) {
            if (mem == MMU_WRED_THD_0_CELLm ||
                mem == MMU_WRED_THD_1_CELLm ||
                mem == MMU_WRED_THD_0_PACKETm ||
                mem == MMU_WRED_THD_1_PACKETm ||
                mem == MMU_WRED_PORT_THD_0_CELLm ||
                mem == MMU_WRED_PORT_THD_1_CELLm ||
                mem == MMU_WRED_PORT_THD_0_PACKETm ||
                mem == MMU_WRED_PORT_THD_1_PACKETm ) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_CACHABLE;
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
            }
        }
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_TRIUMPH_SUPPORT)
        if (SOC_IS_TRIUMPH(unit)) {
            if (mem == L3_DEFIP_DATA_ONLYm) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
            }
            if (mem == L3_DEFIP_128m) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_CACHABLE;
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
            }
            if (mem == MMU_WRED_CFG_CELLm ||
                mem == MMU_WRED_CFG_PACKETm ||
                mem == MMU_WRED_PORT_CFG_CELLm ||
                mem == MMU_WRED_PORT_CFG_PACKETm ||
                mem == MMU_WRED_THD_0_CELLm ||
                mem == MMU_WRED_THD_1_CELLm ||
                mem == MMU_WRED_THD_0_PACKETm ||
                mem == MMU_WRED_THD_1_PACKETm ||
                mem == MMU_WRED_PORT_THD_0_CELLm ||
                mem == MMU_WRED_PORT_THD_1_CELLm ||
                mem == MMU_WRED_PORT_THD_0_PACKETm ||
                mem == MMU_WRED_PORT_THD_1_PACKETm ) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_CACHABLE;
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
            }
        }
#endif /* BCM_TRIUMPH_SUPPORT */
#ifdef  BCM_VALKYRIE_SUPPORT
       if (SOC_IS_VALKYRIE(unit)) {
           if (mem == FP_TCAMm || mem == VFP_TCAMm) {
               SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_CACHABLE;
               SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
           }
       }
#endif /* BCM_VALKYRIE_SUPPORT */
#if defined(BCM_SCORPION_SUPPORT)
        if (SOC_IS_SCORPION(unit)) {
            if (mem == MMU_WRED_THD_0_CELLm ||
                mem == MMU_WRED_THD_1_CELLm ||
                mem == MMU_WRED_PORT_THD_0_CELLm ||
                mem == MMU_WRED_PORT_THD_1_CELLm) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_CACHABLE;
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
            }
        }
#endif /* BCM_SCORPION_SUPPORT */
#if defined(BCM_FIREBOLT2_SUPPORT)
        if (SOC_IS_FIREBOLT2(unit)) {
            switch(mem) {
            case L2Xm:
            case L2_ENTRY_ONLYm:
            case L3_ENTRY_ONLYm:
            case L3_DEFIP_DATA_ONLYm:
            case EFP_TCAMm:
            case FP_TCAMm:
            case VFP_TCAMm:
            case L3_TUNNELm:
            case VLAN_SUBNETm:
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_CACHABLE;
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
                break;
            case MMU_XQ0m:
            case MMU_XQ1m:
            case MMU_XQ2m:
            case MMU_XQ3m:
            case MMU_XQ4m:
            case MMU_XQ5m:
            case MMU_XQ6m:
            case MMU_XQ7m:
            case MMU_XQ8m:
            case MMU_XQ9m:
            case MMU_XQ10m:
            case MMU_XQ11m:
            case MMU_XQ12m:
            case MMU_XQ13m:
            case MMU_XQ14m:
            case MMU_XQ15m:
            case MMU_XQ16m:
            case MMU_XQ17m:
            case MMU_XQ18m:
            case MMU_XQ19m:
            case MMU_XQ20m:
            case MMU_XQ21m:
            case MMU_XQ22m:
            case MMU_XQ23m:
            case MMU_XQ24m:
            case MMU_XQ25m:
            case MMU_XQ26m:
            case MMU_XQ27m:
            case MMU_XQ28m:
            case MMU_CCPm:
            case MMU_CFAPm:
            case MMU_CBPPKTHEADER0m:
            case MMU_CBPPKTHEADER1m:
            case MMU_CBPCELLHEADERm:
            case MMU_CBPDATA0m:
            case MMU_CBPDATA1m:
            case MMU_CBPDATA2m:
            case MMU_CBPDATA3m:
            case MMU_CBPDATA4m:
            case MMU_CBPDATA5m:
            case MMU_CBPDATA6m:
            case MMU_CBPDATA7m:
            case MMU_CBPDATA8m:
            case MMU_CBPDATA9m:
            case MMU_CBPDATA10m:
            case MMU_CBPDATA11m:
            case MMU_CBPDATA12m:
            case MMU_CBPDATA13m:
            case MMU_CBPDATA14m:
            case MMU_CBPDATA15m:
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_ENTRY_CLEAR;
                break;
            default:
                SOC_MEM_INFO(unit, mem).flags &= ~(SOC_MEM_FLAG_CACHABLE | SOC_MEM_SER_FLAGS);
                skip_mem = 1;
            }
        }
#endif /* BCM_FIREBOLT2_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
        if (SOC_IS_TRX(unit)) {
            /* Force cache L2 mem */
            if (mem == L2Xm) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE;
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_SER_ENTRY_CLEAR;
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_SER_SPECIAL; 
            }
            if (SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_CACHABLE;
            }
            /* Skip caching unused views / views which should not be cached */
            switch(mem) {
            case MY_STATION_TCAM_DATA_ONLYm:
            case MY_STATION_TCAM_ENTRY_ONLYm:
            case L2_USER_ENTRY_DATA_ONLYm:
            case L3_DEFIP_DATA_ONLYm:
            case L3_DEFIP_PAIR_128_DATA_ONLYm:
            case L3_DEFIP_128_DATA_ONLYm:
            case L3_DEFIP_128_ONLYm:
            case L3_DEFIP_ONLYm:
            case L3_DEFIP_PAIR_128_ONLYm:
            case L3_ENTRY_ONLYm:
            case L2_BULKm:
            case L2_LEARN_INSERT_FAILUREm:
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_CACHABLE;
                skip_mem = 1;
                break;
            default: break;
            }
        }
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_ENDURO_SUPPORT)
        if(SOC_IS_ENDURO(unit)) {
            if (mem == FP_METER_TABLEm) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE;
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_SER_ENTRY_CLEAR;
            }
            if (SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_CACHABLE;
            }
        }
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit)) {
            if (soc_th_check_cache_skip(unit, mem)) {
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_CACHABLE;
                skip_mem = 1;
            }

            if (mem == MMU_WRED_PORT_SP_SHARED_COUNTm ||
                mem == MMU_WRED_PORT_SP_SHARED_COUNT_XPE0_PIPE0m ||
                mem == MMU_WRED_PORT_SP_SHARED_COUNT_XPE0_PIPE1m ||
                mem == MMU_WRED_PORT_SP_SHARED_COUNT_XPE1_PIPE2m ||
                mem == MMU_WRED_PORT_SP_SHARED_COUNT_XPE1_PIPE3m ||
                mem == MMU_WRED_PORT_SP_SHARED_COUNT_XPE2_PIPE0m ||
                mem == MMU_WRED_PORT_SP_SHARED_COUNT_XPE2_PIPE1m ||
                mem == MMU_WRED_PORT_SP_SHARED_COUNT_XPE3_PIPE2m ||
                mem == MMU_WRED_PORT_SP_SHARED_COUNT_XPE3_PIPE3m) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_ENTRY_CLEAR;
            }
        }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWKPLUS_SUPPORT
        if (SOC_IS_TOMAHAWKPLUS(unit)) {
            if (mem == EGR_FRAGMENT_ID_TABLEm) {
                SOC_MEM_INFO(unit,mem).flags &= ~SOC_MEM_FLAG_SER_CACHE_RESTORE;
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_ENTRY_CLEAR;
            }
        }
#endif
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            if (soc_trident3_check_cache_skip(unit, mem)) {
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_CACHABLE;
                skip_mem = 1;
            }

            if (mem == EXACT_MATCH_LOGICAL_TABLE_SELECTm ||
                mem == EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE0m ||
                mem == EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE1m ||
                mem == IFP_TCAMm || mem == IFP_TCAM_PIPE0m ||
                mem == IFP_TCAM_PIPE1m || mem == IFP_TCAM_WIDEm ||
                mem == IFP_TCAM_WIDE_PIPE0m || mem == IFP_TCAM_WIDE_PIPE1m ||
                mem == EFP_TCAMm || mem == EFP_TCAM_PIPE0m ||
                mem == EFP_TCAM_PIPE1m ||
                mem == EGR_FIELD_EXTRACTION_PROFILE_1_TCAMm ||
                mem == EGR_FIELD_EXTRACTION_PROFILE_2_TCAMm ||
                mem == IP_MULTICAST_TCAMm) {
                SOC_MEM_INFO(unit,mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
            }


            if (SOC_IS_HURRICANE4(unit)) {
                if (mem == CPU_COS_MAPm) {
                    SOC_MEM_INFO(unit,mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
                }
            } else if (mem == CPU_COS_MAPm || mem == VFP_TCAMm ||
                mem == VFP_TCAM_PIPE0m || mem == VFP_TCAM_PIPE1m) {
                SOC_MEM_INFO(unit,mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
            }
            if (mem == L3_DEFIPm ||
                mem == L3_DEFIP_PAIR_128m) {
                SOC_MEM_INFO(unit,mem).flags &= ~SOC_MEM_FLAG_SER_ECC_CORRECTABLE;
            }
            if (mem == DLB_ECMP_FLOWSET_MEMBERm ||
                mem == DLB_ECMP_FLOWSET_MEMBER_PIPE0m ||
                mem == DLB_ECMP_FLOWSET_MEMBER_PIPE1m) {
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_SER_ENTRY_CLEAR;
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE;
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
            }

            if (mem == TCB_THRESHOLD_PROFILE_MAPm ||
                mem == TCB_THRESHOLD_PROFILE_MAP_XPE0m ||
                mem == TCB_THRESHOLD_PROFILE_MAP_Am ||
                mem == TCB_THRESHOLD_PROFILE_MAP_A_XPE0m ||
                mem == TCB_THRESHOLD_PROFILE_MAP_Bm ||
                mem == TCB_THRESHOLD_PROFILE_MAP_B_XPE0m) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_CACHABLE;
            }

            /* Overwrite the SER_RESPONSE from ECC_PARITY to CACHE_RESTORE*/
            if (mem == IP_PARSER0_HME_STAGE_TCAM_ONLY_0m ||
                mem == IP_PARSER0_HME_STAGE_TCAM_NARROW_ONLY_0m ||
                mem == IP_PARSER1_HME_STAGE_TCAM_ONLY_0m ||
                mem == IP_PARSER1_HME_STAGE_TCAM_NARROW_ONLY_0m ||
                mem == IP_PARSER1_HME_STAGE_TCAM_ONLY_1m ||
                mem == IP_PARSER1_HME_STAGE_TCAM_NARROW_ONLY_1m ||
                mem == IP_PARSER1_HME_STAGE_TCAM_NARROW_ONLY_2m ||
                mem == IP_PARSER1_HME_STAGE_TCAM_ONLY_3m ||
                mem == IP_PARSER1_HME_STAGE_TCAM_NARROW_ONLY_3m ||
                mem == IP_PARSER1_HME_STAGE_TCAM_ONLY_4m ||
                mem == IP_PARSER1_HME_STAGE_TCAM_NARROW_ONLY_4m ||
                mem == IP_PARSER2_HME_STAGE_TCAM_ONLY_0m ||
                mem == IP_PARSER2_HME_STAGE_TCAM_NARROW_ONLY_0m ||
                mem == IP_PARSER2_HME_STAGE_TCAM_ONLY_1m ||
                mem == IP_PARSER2_HME_STAGE_TCAM_NARROW_ONLY_1m ||
                mem == IP_PARSER2_HME_STAGE_TCAM_NARROW_ONLY_2m ||
                mem == IP_PARSER2_HME_STAGE_TCAM_NARROW_ONLY_3m ||
                mem == IP_PARSER2_HME_STAGE_TCAM_ONLY_4m ||
                mem == IP_PARSER2_HME_STAGE_TCAM_NARROW_ONLY_4m ||
                mem == FORWARDING_1_LOGICAL_TBL_SEL_TCAMm ||
                mem == FORWARDING_2_LOGICAL_TBL_SEL_TCAMm ||
                mem == FORWARDING_3_LOGICAL_TBL_SEL_TCAMm ||
                mem == TUNNEL_ADAPT_1_LOGICAL_TBL_SEL_TCAMm ||
                mem == TUNNEL_ADAPT_2_LOGICAL_TBL_SEL_TCAMm ||
                mem == TUNNEL_ADAPT_3_LOGICAL_TBL_SEL_TCAMm ||
                mem == TUNNEL_ADAPT_4_LOGICAL_TBL_SEL_TCAMm ||
                mem == EGR_ADAPT_1_LOGICAL_TBL_SEL_TCAMm ||
                mem == EGR_ADAPT_2_LOGICAL_TBL_SEL_TCAMm) {
                    SOC_MEM_INFO(unit,mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
                }

#ifdef BCM_HELIX5_SUPPORT
            if (mem == BSK_FTFP_TCAMm ||
                mem == BSK_FTFP_LTS_LOGICAL_TBL_SEL_TCAMm) {
                SOC_MEM_INFO(unit,mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
            }

            if (mem == BSC_AG_AGE_TABLEm) {
                /* R/O Table */
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_SER_ECC_CORRECTABLE;
            }
#endif
#ifdef BCM_FIREBOLT6_SUPPORT
            if (SOC_IS_FIREBOLT6(unit) &&
               (mem == BSK_FTFP2_TCAMm ||
                mem == BSK_AEFP_TCAMm ||
                mem == BSK_AMFP_TCAMm ||
                mem == BSK_FTFP2_LTS_LOGICAL_TBL_SEL_TCAMm ||
                mem == IP_PARSER2_HME_STAGE_TCAM_ONLY_3m)) {
                SOC_MEM_INFO(unit,mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
            }
#endif
            if (mem == SFLOW_ING_DATA_SOURCEm ||
                (mem == EGR_SEQUENCE_NUMBER_TABLEm &&
                !SOC_IS_HURRICANE4(unit))) {
                SOC_MEM_INFO(unit,mem).flags &= ~SOC_MEM_FLAG_SER_ECC_CORRECTABLE;
                SOC_MEM_INFO(unit,mem).flags |= SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE;
            }

            if (mem == TCB_THRESHOLD_PROFILE_MAPm ||
                mem == TCB_THRESHOLD_PROFILE_MAP_XPE0m) {
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_SER_ENTRY_CLEAR;
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_SER_ECC_CORRECTABLE;
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_CACHABLE;
            }

            if (NUM_PIPE(unit) == 1) {
                if (mem == L2_ENTRY_LPm ||  mem == L3_ENTRY_LPm ||
                    mem == EXACT_MATCH_LPm || mem == EXACT_MATCH_LP_PIPE0m ||
                    mem == MPLS_ENTRY_LPm ||
                    mem == VLAN_XLATE_1_LPm || mem == VLAN_XLATE_2_LPm ||
                    mem == EGR_VLAN_XLATE_1_LPm || mem == EGR_VLAN_XLATE_2_LPm) {
                    SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_SER_ENTRY_CLEAR;
                    SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_SER_ECC_CORRECTABLE;
                    SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_SER_CACHE_RESTORE;
                    SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_CACHABLE;
                    SOC_MEM_INFO(unit,mem).flags |= SOC_MEM_FLAG_SER_SPECIAL;
                }
            }
        }
#endif
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            if (mem == EFP_TCAMm || mem == EFP_TCAM_PIPE0m ||
                mem == EFP_TCAM_PIPE1m || mem == EFP_TCAM_PIPE2m ||
                mem == EFP_TCAM_PIPE3m || mem == VFP_TCAMm ||
                mem == VFP_TCAM_PIPE0m || mem == VFP_TCAM_PIPE1m ||
                mem == VFP_TCAM_PIPE2m || mem == VFP_TCAM_PIPE3m ||
                mem == IFP_TCAMm || mem == IFP_TCAM_PIPE0m ||
                mem == IFP_TCAM_PIPE1m || mem == IFP_TCAM_PIPE2m ||
                mem == IFP_TCAM_PIPE3m || mem == IP_MULTICAST_TCAMm ||
                mem == FP_UDF_TCAMm || mem == FP_UDF_TCAM_PIPE0m ||
                mem == FP_UDF_TCAM_PIPE1m || mem == FP_UDF_TCAM_PIPE2m ||
                mem == FP_UDF_TCAM_PIPE3m || mem == IFP_TCAM_WIDEm ||
                mem == IFP_TCAM_WIDE_PIPE0m || mem == IFP_TCAM_WIDE_PIPE1m ||
                mem == IFP_TCAM_WIDE_PIPE2m || mem == IFP_TCAM_WIDE_PIPE3m ||
                mem == CPU_COS_MAPm || mem == EXACT_MATCH_LOGICAL_TABLE_SELECTm) {
                SOC_MEM_INFO(unit,mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
            }

            if (soc_feature(unit, soc_feature_tcam_scan_engine) &&
                (mem == VFP_TCAMm || mem == VFP_TCAM_PIPE0m ||
                 mem == VFP_TCAM_PIPE1m || mem == VFP_TCAM_PIPE2m ||
                 mem == VFP_TCAM_PIPE3m)) {
                SOC_MEM_INFO(unit,mem).flags &= ~SOC_MEM_FLAG_SER_CACHE_RESTORE;
                SOC_MEM_INFO(unit,mem).flags |= SOC_MEM_FLAG_SER_SPECIAL;
            }

            if (mem == MMU_TCB_BUFFER_CELL_DATA_SCRATCH_UPPERm ||
                mem == MMU_TCB_BUFFER_CELL_DATA_SCRATCH_UPPER_SED0_PIPE0m ||
                mem == MMU_TCB_BUFFER_CELL_DATA_SCRATCH_UPPER_SED0_PIPE1m ||
                mem == MMU_TCB_BUFFER_CELL_DATA_SCRATCH_UPPER_SED0_PIPE2m ||
                mem == MMU_TCB_BUFFER_CELL_DATA_SCRATCH_UPPER_SED0_PIPE3m ||
                mem == MMU_TCB_BUFFER_CELL_DATA_SCRATCH_UPPER_SED1_PIPE0m ||
                mem == MMU_TCB_BUFFER_CELL_DATA_SCRATCH_UPPER_SED1_PIPE1m ||
                mem == MMU_TCB_BUFFER_CELL_DATA_SCRATCH_UPPER_SED1_PIPE2m ||
                mem == MMU_TCB_BUFFER_CELL_DATA_SCRATCH_UPPER_SED1_PIPE3m ||
                mem == MMU_TCB_BUFFER_CELL_DATA_SCRATCH_LOWERm ||
                mem == MMU_TCB_BUFFER_CELL_DATA_SCRATCH_LOWER_SED0_PIPE0m ||
                mem == MMU_TCB_BUFFER_CELL_DATA_SCRATCH_LOWER_SED0_PIPE1m ||
                mem == MMU_TCB_BUFFER_CELL_DATA_SCRATCH_LOWER_SED0_PIPE2m ||
                mem == MMU_TCB_BUFFER_CELL_DATA_SCRATCH_LOWER_SED0_PIPE3m ||
                mem == MMU_TCB_BUFFER_CELL_DATA_SCRATCH_LOWER_SED1_PIPE0m ||
                mem == MMU_TCB_BUFFER_CELL_DATA_SCRATCH_LOWER_SED1_PIPE1m ||
                mem == MMU_TCB_BUFFER_CELL_DATA_SCRATCH_LOWER_SED1_PIPE2m ||
                mem == MMU_TCB_BUFFER_CELL_DATA_SCRATCH_LOWER_SED1_PIPE3m ||
                mem == MMU_TCB_BUFFER_CELL_DATA_UPPERm ||
                mem == MMU_TCB_BUFFER_CELL_DATA_UPPER_XPE0m ||
                mem == MMU_TCB_BUFFER_CELL_DATA_UPPER_XPE1m ||
                mem == MMU_TCB_BUFFER_CELL_DATA_UPPER_XPE2m ||
                mem == MMU_TCB_BUFFER_CELL_DATA_UPPER_XPE3m ||
                mem == MMU_TCB_BUFFER_CELL_DATA_LOWERm ||
                mem == MMU_TCB_BUFFER_CELL_DATA_LOWER_XPE0m ||
                mem == MMU_TCB_BUFFER_CELL_DATA_LOWER_XPE1m ||
                mem == MMU_TCB_BUFFER_CELL_DATA_LOWER_XPE2m ||
                mem == MMU_TCB_BUFFER_CELL_DATA_LOWER_XPE3m) {
                SOC_MEM_INFO(unit,mem).flags |= SOC_MEM_FLAG_SER_PARITY_ENABLE_SKIP;
            }

            if (mem == TCB_THRESHOLD_PROFILE_MAPm ||
                mem == TCB_THRESHOLD_PROFILE_MAP_XPE0m ||
                mem == TCB_THRESHOLD_PROFILE_MAP_XPE1m ||
                mem == TCB_THRESHOLD_PROFILE_MAP_XPE2m ||
                mem == TCB_THRESHOLD_PROFILE_MAP_XPE3m) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_CACHABLE;
            }
        }
#endif
#if defined(BCM_TOMAHAWK3_SUPPORT)
        if (SOC_IS_TOMAHAWK3(unit)) {
            if (mem == MMU_EBMB_PAYLOAD_SLICEm || mem == MMU_EBMB_CCBE_SLICEm ||
                mem == MMU_EBMB_CCBE_SERm || mem == MMU_EBMB_CCBE_SER_PIPE0m||
                mem == MMU_EBMB_CCBE_SER_PIPE1m || mem == MMU_EBMB_CCBE_SER_PIPE2m ||
                mem == MMU_EBMB_CCBE_SER_PIPE3m || mem == MMU_EBMB_CCBE_SER_PIPE4m ||
                mem == MMU_EBMB_CCBE_SER_PIPE5m || mem == MMU_EBMB_CCBE_SER_PIPE6m ||
                mem == MMU_EBMB_CCBE_SER_PIPE7m || mem == MMU_EBMB_PAYLOAD_ITM0_CH0_SERm ||
                mem == MMU_EBMB_PAYLOAD_ITM0_CH0_SER_PIPE0m || mem == MMU_EBMB_PAYLOAD_ITM0_CH0_SER_PIPE1m ||
                mem == MMU_EBMB_PAYLOAD_ITM0_CH0_SER_PIPE2m || mem == MMU_EBMB_PAYLOAD_ITM0_CH0_SER_PIPE3m ||
                mem == MMU_EBMB_PAYLOAD_ITM0_CH0_SER_PIPE4m || mem == MMU_EBMB_PAYLOAD_ITM0_CH0_SER_PIPE5m ||
                mem == MMU_EBMB_PAYLOAD_ITM0_CH0_SER_PIPE6m || mem == MMU_EBMB_PAYLOAD_ITM0_CH0_SER_PIPE7m ||
                mem == MMU_EBMB_PAYLOAD_ITM1_CH0_SER_PIPE0m || mem == MMU_EBMB_PAYLOAD_ITM1_CH0_SER_PIPE1m ||
                mem == MMU_EBMB_PAYLOAD_ITM1_CH0_SER_PIPE2m || mem == MMU_EBMB_PAYLOAD_ITM1_CH0_SER_PIPE3m ||
                mem == MMU_EBMB_PAYLOAD_ITM1_CH0_SER_PIPE4m || mem == MMU_EBMB_PAYLOAD_ITM1_CH0_SER_PIPE5m ||
                mem == MMU_EBMB_PAYLOAD_ITM1_CH0_SER_PIPE6m || mem == MMU_EBMB_PAYLOAD_ITM1_CH0_SER_PIPE7m ||
                mem == MMU_EBMB_PAYLOAD_ITM0_CH1_SERm || mem == MMU_EBMB_PAYLOAD_ITM1_CH1_SERm ||
                mem == MMU_EBMB_PAYLOAD_ITM0_CH1_SER_PIPE0m || mem == MMU_EBMB_PAYLOAD_ITM0_CH1_SER_PIPE1m ||
                mem == MMU_EBMB_PAYLOAD_ITM0_CH1_SER_PIPE2m || mem == MMU_EBMB_PAYLOAD_ITM0_CH1_SER_PIPE3m ||
                mem == MMU_EBMB_PAYLOAD_ITM0_CH1_SER_PIPE4m || mem == MMU_EBMB_PAYLOAD_ITM0_CH1_SER_PIPE5m ||
                mem == MMU_EBMB_PAYLOAD_ITM0_CH1_SER_PIPE6m || mem == MMU_EBMB_PAYLOAD_ITM0_CH1_SER_PIPE7m ||
                mem == MMU_EBMB_PAYLOAD_ITM1_CH1_SER_PIPE0m || mem == MMU_EBMB_PAYLOAD_ITM1_CH1_SER_PIPE1m ||
                mem == MMU_EBMB_PAYLOAD_ITM1_CH1_SER_PIPE2m || mem == MMU_EBMB_PAYLOAD_ITM1_CH1_SER_PIPE3m ||
                mem == MMU_EBMB_PAYLOAD_ITM1_CH1_SER_PIPE4m || mem == MMU_EBMB_PAYLOAD_ITM1_CH1_SER_PIPE5m ||
                mem == MMU_EBMB_PAYLOAD_ITM1_CH1_SER_PIPE6m || mem == MMU_EBMB_PAYLOAD_ITM1_CH1_SER_PIPE7m ||
                mem == MMU_MB_PAYLOAD_SER_CH0m || mem == MMU_MB_PAYLOAD_SER_CH0_ITM0m ||
                mem == MMU_MB_PAYLOAD_SER_CH0_ITM1m || mem == MMU_MB_PAYLOAD_SER_CH1m ||
                mem == MMU_MB_PAYLOAD_SER_CH1_ITM0m || mem == MMU_MB_PAYLOAD_SER_CH1_ITM1m ||
                mem == MMU_MB_PAYLOAD_SER_CH2m || mem == MMU_MB_PAYLOAD_SER_CH2_ITM0m ||
                mem == MMU_MB_PAYLOAD_SER_CH2_ITM1m || mem == MMU_MB_PAYLOAD_SER_CH3m ||
                mem == MMU_MB_PAYLOAD_SER_CH3_ITM0m || mem == MMU_MB_PAYLOAD_SER_CH3_ITM1m) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_VALID;
                }

            if (mem == EFP_TCAMm || mem == EFP_TCAM_PIPE0m ||
                mem == EFP_TCAM_PIPE1m || mem == EFP_TCAM_PIPE2m ||
                mem == EFP_TCAM_PIPE3m || mem == EFP_TCAM_PIPE4m ||
                mem == EFP_TCAM_PIPE5m || mem == EFP_TCAM_PIPE6m ||
                mem == EFP_TCAM_PIPE7m || mem == VFP_TCAMm ||
                mem == VFP_TCAM_PIPE0m || mem == VFP_TCAM_PIPE1m ||
                mem == VFP_TCAM_PIPE2m || mem == VFP_TCAM_PIPE3m ||
                mem == VFP_TCAM_PIPE4m || mem == VFP_TCAM_PIPE5m ||
                mem == VFP_TCAM_PIPE6m || mem == VFP_TCAM_PIPE7m ||
                mem == IFP_TCAMm || mem == IFP_TCAM_PIPE0m ||
                mem == IFP_TCAM_PIPE1m || mem == IFP_TCAM_PIPE2m ||
                mem == IFP_TCAM_PIPE3m || mem == IFP_TCAM_PIPE4m ||
                mem == IFP_TCAM_PIPE5m || mem == IFP_TCAM_PIPE6m ||
                mem == IFP_TCAM_PIPE7m || mem == L3_DEFIP_TCAM_LEVEL1m ||
                mem == FP_UDF_TCAMm || mem == FP_UDF_TCAM_PIPE0m ||
                mem == FP_UDF_TCAM_PIPE1m || mem == FP_UDF_TCAM_PIPE2m ||
                mem == FP_UDF_TCAM_PIPE3m || mem == FP_UDF_TCAM_PIPE4m ||
                mem == FP_UDF_TCAM_PIPE5m || mem == FP_UDF_TCAM_PIPE6m ||
                mem == FP_UDF_TCAM_PIPE7m || mem == L2_USER_ENTRY_ONLYm ||
                mem == MY_STATION_TCAM_ENTRY_ONLYm ||
                mem == EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLYm ||
                mem == EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE0m ||
                mem == EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE1m ||
                mem == EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE2m ||
                mem == EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE3m ||
                mem == EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE4m ||
                mem == EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE5m ||
                mem == EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE6m ||
                mem == EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE7m ||
                mem == EXACT_MATCH_LOGICAL_TABLE_SELECTm ||
                mem == EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE0m ||
                mem == EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE1m ||
                mem == EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE2m ||
                mem == EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE3m ||
                mem == EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE4m ||
                mem == EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE5m ||
                mem == EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE6m ||
                mem == EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE7m ||
                mem == IFP_LOGICAL_TABLE_SELECTm ||
                mem == IFP_LOGICAL_TABLE_SELECT_PIPE0m ||
                mem == IFP_LOGICAL_TABLE_SELECT_PIPE1m ||
                mem == IFP_LOGICAL_TABLE_SELECT_PIPE2m ||
                mem == IFP_LOGICAL_TABLE_SELECT_PIPE3m ||
                mem == IFP_LOGICAL_TABLE_SELECT_PIPE4m ||
                mem == IFP_LOGICAL_TABLE_SELECT_PIPE5m ||
                mem == IFP_LOGICAL_TABLE_SELECT_PIPE6m ||
                mem == IFP_LOGICAL_TABLE_SELECT_PIPE7m ||
                mem == IFP_LOGICAL_TABLE_SELECT_TCAM_ONLYm ||
                mem == IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE0m ||
                mem == IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE1m ||
                mem == IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE2m ||
                mem == IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE3m ||
                mem == IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE4m ||
                mem == IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE5m ||
                mem == IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE6m ||
                mem == IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE7m ||
                mem == CPU_COS_MAPm || mem == CPU_COS_MAP_ONLYm) {
                SOC_MEM_INFO(unit,mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
            }

            if (mem == L3_TUNNEL_SINGLEm || mem == L3_TUNNEL_DOUBLEm ||
                mem == L3_TUNNEL_QUADm || mem == L3_TUNNEL_ECCm ||
                mem == MPLS_ENTRY_SINGLEm || mem == MPLS_ENTRY_ECCm) {
                SOC_MEM_INFO(unit,mem).flags &= ~SOC_MEM_FLAG_SER_SPECIAL;
                SOC_MEM_INFO(unit,mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
            }

            if (mem == MMU_QSCH_L0_WEIGHT_MEMm || mem == MMU_QSCH_L1_WEIGHT_MEMm ||
                mem == MMU_QSCH_L0_WEIGHT_MEM_PIPE0m || mem == MMU_QSCH_L0_WEIGHT_MEM_PIPE1m ||
                mem == MMU_QSCH_L0_WEIGHT_MEM_PIPE2m || mem == MMU_QSCH_L0_WEIGHT_MEM_PIPE3m ||
                mem == MMU_QSCH_L0_WEIGHT_MEM_PIPE4m || mem == MMU_QSCH_L0_WEIGHT_MEM_PIPE5m ||
                mem == MMU_QSCH_L0_WEIGHT_MEM_PIPE6m || mem == MMU_QSCH_L0_WEIGHT_MEM_PIPE7m ||
                mem == MMU_QSCH_L1_WEIGHT_MEM_PIPE0m || mem == MMU_QSCH_L1_WEIGHT_MEM_PIPE1m ||
                mem == MMU_QSCH_L1_WEIGHT_MEM_PIPE2m || mem == MMU_QSCH_L1_WEIGHT_MEM_PIPE3m ||
                mem == MMU_QSCH_L1_WEIGHT_MEM_PIPE4m || mem == MMU_QSCH_L1_WEIGHT_MEM_PIPE5m ||
                mem == MMU_QSCH_L1_WEIGHT_MEM_PIPE6m || mem == MMU_QSCH_L1_WEIGHT_MEM_PIPE7m) {
                SOC_MEM_INFO(unit,mem).null_entry = _soc_mem_entry_reset_value;
            }
        }
#endif

#if !defined(SOC_NO_NAMES)
#if defined(BCM_ESW_SUPPORT) || \
    defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
        if (chk_skip_cache) {
            /* coverity[secure_coding] */
            sal_strcpy(mem_name, "mem_nocache_");
            mptr = &mem_name[sal_strlen(mem_name)];
            sal_strcpy(mptr, SOC_MEM_NAME(unit, mem));
            if (soc_property_get(unit, mem_name, 0)) {
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_CACHABLE;
                skip_mem = 1;
            }
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_IS_TD2_TT2(unit) && (NUM_PIPE(unit) != 1)) {
                if (mem == L3_DEFIP_ALPM_IPV4m || mem == L3_DEFIP_ALPM_IPV4_1m ||
                    mem == L3_DEFIP_ALPM_IPV6_64m || mem == L3_DEFIP_ALPM_IPV6_64_1m ||
                    mem == L3_DEFIP_ALPM_IPV6_128m || mem == L3_DEFIPm ||
                    mem == L3_DEFIP_PAIR_128m || mem == L3_DEFIP_AUX_TABLEm) {
                    /*
                     * ALPM tables are not cached by default.
                     * To avoid impacting the existing behavior, cache should be
                     * enabled explicitly with mem_nocache_xxx=0.
                     */
                    if (!soc_property_get(unit, mem_name, 1)) {
                        SOC_MEM_INFO(unit, mem).flags &=
                            ~SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE;
                        SOC_MEM_INFO(unit, mem).flags &=
                            ~SOC_MEM_FLAG_SER_ECC_CORRECTABLE;
                        SOC_MEM_INFO(unit, mem).flags |=
                            SOC_MEM_FLAG_SER_CACHE_RESTORE;
                        SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_CACHABLE;
                        skip_mem = 0;
                    }
                }
            }
#endif /* BCM_TRIDENT2_SUPPORT */
        }
#endif

#if defined(BCM_ESW_SUPPORT)
        if (SOC_IS_ESW(unit)) {
            /* coverity[secure_coding] */
            sal_strcpy(mem_name, "mem_parity_enable_skip_");
            mptr = &mem_name[sal_strlen(mem_name)];
            sal_strcpy(mptr, SOC_MEM_NAME(unit, mem));
            if (soc_property_get(unit, mem_name, 0)) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_PARITY_ENABLE_SKIP;
                LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "parity_en_skip=1: for mem %s \n"), SOC_MEM_NAME(unit, mem)));
#if defined(SOC_UNIQUE_ACC_TYPE_ACCESS)
                if (soc_feature(unit, soc_feature_unique_acc_type_access) &&
                    (SOC_MEM_UNIQUE_ACC(unit, mem) != NULL)) {
                    int pipe;
                    soc_mem_t mem_p;
                    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                        mem_p = SOC_MEM_UNIQUE_ACC(unit, mem)[pipe];
                        LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "parity_en_skip=1: for mem %s \n"), SOC_MEM_NAME(unit, mem_p)));
                        SOC_MEM_INFO(unit, mem_p).flags |= SOC_MEM_FLAG_SER_PARITY_ENABLE_SKIP;
                    }
                }
#endif /* SOC_UNIQUE_ACC_TYPE_ACCESS */
            }
        }
#endif
#endif /* !SOC_NO_NAMES */
#ifdef BCM_KATANA_SUPPORT
        if (SOC_IS_KATANAX(unit)) {
            if(mem == XPORT_WC_UCMEM_DATAm) {
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_SER_ECC_CORRECTABLE;
                skip_mem = 1;
            }
        }
#endif

#ifdef BCM_SABER2_SUPPORT
        if (SOC_IS_SABER2(unit)) {
            if ((mem == LLS_L1_CONFIGm) || (mem == LLS_L0_CONFIGm)) {
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_CACHABLE;
                skip_mem = 1;
            }
        }
#endif


#if defined(BCM_APACHE_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_APACHE(unit) || (SOC_IS_TRIDENT3X(unit) && NUM_PIPE(unit) == 1)) {
            if ((mem == LLS_S1_CONFIGm) || (mem == LLS_L1_CONFIGm) || (mem == LLS_L0_CONFIGm) || (mem == MMU_ENQ_PBI_DBm)) {
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_CACHABLE;
                skip_mem = 1;
            }
            if (mem == L3_DEFIP_ALPM_IPV4m || mem == L3_DEFIP_ALPM_IPV4_1m ||
                mem == L3_DEFIP_ALPM_IPV6_64m || mem == L3_DEFIP_ALPM_IPV6_64_1m ||
                mem == L3_DEFIP_ALPM_IPV6_128m || mem == L3_DEFIP_ALPM_RAWm ||
                mem == L3_DEFIPm || mem == L3_DEFIP_PAIR_128m ||
                mem == L3_DEFIP_AUX_TABLEm) {
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_SER_SPECIAL;
                if (soc_feature(unit, soc_feature_alpm) &&
                    soc_property_get(unit, spn_L3_ALPM_ENABLE, 0)) {
                    SOC_MEM_INFO(unit, mem).flags &=
                        ~SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE;
                    SOC_MEM_INFO(unit, mem).flags |=
                        SOC_MEM_FLAG_SER_CACHE_RESTORE;
                } else {
                    SOC_MEM_INFO(unit, mem).flags |=
                        SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE;
                }
            }
        }
        if (SOC_IS_APACHE(unit)) {
            if (mem == FP_GM_FIELDSm) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_CACHE_RESTORE;
            }
            if (mem == ING_SNAT_ONLYm) {
                SOC_MEM_INFO(unit, mem).flags &=~ SOC_MEM_FLAG_SER_CACHE_RESTORE;
            }
        }

#endif
#if defined(BCM_MONTEREY_SUPPORT)
        if (SOC_IS_MONTEREY(unit)) {
            if (mem == MMU_MAPPER_X_LSBm) {
                skip_mem = 1;
            }
        }
#endif
#if defined(BCM_MONTEREY_SUPPORT) || defined(BCM_HURRICANE4_SUPPORT)
        if (soc_feature(unit, soc_feature_xflow_macsec) &&
           (mem == ESEC_SC_TABLEm)) {
                SOC_MEM_INFO(unit, mem).flags |=
                    SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE;
                SOC_MEM_INFO(unit, mem).flags &=
                    ~SOC_MEM_FLAG_SER_ECC_CORRECTABLE;
            }
#endif
        if (soc_feature(unit, soc_feature_alpm2) &&
            soc_property_get(unit, spn_L3_ALPM_ENABLE, 0)) {
            if (mem == L3_DEFIP_LEVEL1m ||
                mem == L3_DEFIP_PAIR_LEVEL1m ||
                mem == L3_DEFIP_ALPM_LEVEL2m ||
                mem == L3_DEFIP_ALPM_LEVEL3m ||
                mem == L3_DEFIP_ALPM_LEVEL2_HIT_ONLYm ||
                mem == L3_DEFIP_ALPM_LEVEL3_HIT_ONLYm ||
                mem == L3_DEFIP_LEVEL1_HIT_ONLYm ||
                mem == L3_DEFIPm ||
                mem == L3_DEFIP_PAIR_128m ||
                mem == L3_DEFIP_ALPM_RAWm ||
                mem == L3_DEFIP_ALPM_HIT_ONLYm ||
                mem == L3_DEFIP_HIT_ONLYm ||
                mem == L3_DEFIP_PAIR_128_HIT_ONLYm) {
                skip_mem = 0;
                SOC_MEM_INFO(unit, mem).flags &=
                    ~SOC_MEM_FLAG_SER_SPECIAL;
                SOC_MEM_INFO(unit, mem).flags &=
                    ~SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE;
                SOC_MEM_INFO(unit, mem).flags |=
                    SOC_MEM_FLAG_SER_CACHE_RESTORE;
                SOC_MEM_INFO(unit, mem).flags |=
                    SOC_MEM_FLAG_CACHABLE;
            }
        }
#if defined(BCM_FIRELIGHT_SUPPORT)
        if (SOC_IS_FIRELIGHT(unit)) {
            if ((mem == ESEC_SA_HASH_TABLEm) ||
                (mem == ISEC_SA_HASH_TABLEm)) {
                SOC_MEM_INFO(unit, mem).flags =
                    ~(SOC_MEM_FLAG_CACHABLE | SOC_MEM_SER_FLAGS);
                skip_mem = 1;
            }
            if ((mem == ESEC_MIB_MISCm) ||
                (mem == ESEC_MIB_SC_UNCTRLm) ||
                (mem == ESEC_MIB_SC_CTRLm) ||
                (mem == ESEC_MIB_SCm) ||
                (mem == ESEC_MIB_SAm) ||
                (mem == ISEC_SPTCAM_HIT_COUNTm) ||
                (mem == ISEC_SCTCAM_HIT_COUNTm) ||
                (mem == ISEC_PORT_COUNTERSm) ||
                (mem == ISEC_MIB_SP_UNCTRLm) ||
                (mem == ISEC_MIB_SP_CTRL_1m) ||
                (mem == ISEC_MIB_SP_CTRL_2m) ||
                (mem == ISEC_MIB_SCm) ||
                (mem == ISEC_MIB_SAm)) {
                SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_CACHABLE;
                SOC_MEM_INFO(unit,mem).flags &= ~SOC_MEM_FLAG_SER_CACHE_RESTORE;
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_SER_ENTRY_CLEAR;
            }

            if (mem == ISEC_SC_TABLEm ||
                mem == SUB_PORT_POLICY_TABLEm ||
                mem == SUB_PORT_MAP_TABLEm ||
                mem == ISEC_SC_TCAMm) {
                SOC_MEM_INFO(unit, mem).flags |=
                    SOC_MEM_FLAG_SER_CACHE_RESTORE;
            }
            else if (mem == ESEC_SA_TABLEm ||
                     mem == ISEC_SA_TABLEm ||
                     mem == ISEC_SP_TCAM_KEYm ||
                     mem == ISEC_SP_TCAM_MASKm ||
                     mem == ESEC_SC_TABLEm) {
                     SOC_MEM_INFO(unit, mem).flags |=
                                    SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE;
            }
        }
#endif

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            /* Skipping MMU_ENQ_PBI_DBm, as there is no easy way to not read
             * this memory until the ingress traffic has been disabled Hence
             * skipping the Memory */
            if (SOC_IS_TD2P_TT2P(unit)) {
                if (mem == MMU_ENQ_PBI_DBm) {
                    SOC_MEM_INFO(unit, mem).flags &= ~SOC_MEM_FLAG_CACHABLE;
                    skip_mem = 1;
                }
            }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

        /* Enable cache only if all criteria's are met */
        if (cache && !skip_mem &&
            ((SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_SER_CACHE_RESTORE) ||
             (SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE) ||
             ((SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_CACHABLE) &&
              ((SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_SER_ECC_CORRECTABLE) ||
               (SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_SER_SPECIAL))))) {
            SOC_IF_ERROR_RETURN(soc_mem_cache_set(unit, mem, MEM_BLOCK_ALL, TRUE));
#if defined(BCM_ESW_SUPPORT)
            if (soc_mem_is_dynamic(unit, mem)) {
                SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_DYNAMIC;
            }
#endif
#ifdef BCM_WARM_BOOT_SUPPORT
            SOC_IF_ERROR_RETURN(soc_mem_cache_scache_load(unit, mem, &offset));
#endif
        } else {
            SOC_IF_ERROR_RETURN(soc_mem_cache_set(unit, mem, MEM_BLOCK_ALL, FALSE));
        }
    }
#if defined(BCM_XGS_SUPPORT)
    if (soc_feature(unit, soc_feature_regs_as_mem) &&
        soc_property_get(unit, spn_PARITY_ENABLE, TRUE)) {
        SOC_IF_ERROR_RETURN(soc_ser_reg_cache_init(unit));
    }
#endif /* BCM_XGS_SUPPORT */

    /***********************************************************************/
    /* Configure CMIC PCI registers correctly for driver operation.        */
    /*                                                                     */
    /* NOTE:  When interrupt driven, the internal SOC registers cannot     */
    /*        be accessed until the CMIC interrupts are enabled.           */
    /***********************************************************************/

    if (!SAL_BOOT_PLISIM && !SOC_WARM_BOOT(unit) && !SOC_IS_RCPU_UNIT(unit)) {
#ifdef BCM_CMICX_SUPPORT
        if (!SAL_BOOT_QUICKTURN) {
            uint32 valid;
            SOC_IF_ERROR_RETURN(soc_pcie_fw_status_get(unit, &valid));
            if(!valid) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                        "PCIe firware was not loaded on unit %d\n"), unit));
                return SOC_E_INIT;
            }
        }
#endif
        /*
         * Check that PCI memory space is mapped correctly by running a
         * quick diagnostic on the S-Channel message buffer.
         */

        SOC_IF_ERROR_RETURN(soc_pci_test(unit));
    }

    if (!SOC_IS_RCPU_ONLY(unit)) {
    /*
     * Adjust the CMIC CONFIG register
     */
#ifdef BCM_CMICM_SUPPORT
    if(soc_feature(unit, soc_feature_cmicm)) {
        if (SAL_BOOT_PLISIM) {
            /* Set interrupt polarity to active high */
        }
    } else
#endif
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        /* Do Nothing; prevent incorrect register access */
    } else
#endif
    {
        reg = soc_pci_read(unit, CMIC_CONFIG);

        /*
         * Enable enhanced DMA modes:
         *  Scatter/gather, reload, and unaligned transfers
         *
         * Enable read and write bursts.
         *  Note: very fast CPUs (above ~500 MHz) may combine multiple
         *  memory operations into bursts.  The CMIC will hang if burst
         *  operations are not enabled.
         */

        reg |= (CC_SG_OPN_EN | CC_RLD_OPN_EN | CC_ALN_OPN_EN |
                CC_RD_BRST_EN | CC_WR_BRST_EN);

        if (SAL_BOOT_PLISIM) {
            /* Set interrupt polarity to active high */
            reg &= ~CC_ACT_LOW_INT;
        }

#ifdef BCM_XGS_SWITCH_SUPPORT
        if (SOC_IS_XGS_SWITCH(unit) &&
            !soc_property_get(unit, spn_MDIO_EXTERNAL_MASTER, 0)) {
            reg |= CC_EXT_MDIO_MSTR_DIS;
        }
#endif

        soc_pci_write(unit, CMIC_CONFIG, reg);
    }

    /*
     * Configure DMA channels.
     */
    /* soc_dma_init will be invoked in soc_dma_attach */
    if ((rv = soc_dma_attach(unit, reset)) < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Packet DMA attach failed\n")));
        return SOC_E_INTERNAL;
    }

    } /* !SOC_IS_RCPU_ONLY(unit) */

#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {
        int                 i, j;
        int                 numq = 0, cmc_numq;

        
        /* Enable PCI Bus Error and Parity Error Interrupts */
        /* soc_intr_enable(unit, IRQ_PCI_PARITY_ERR | IRQ_PCI_FATAL_ERR); */
        /* S-Channel Error Interrupt */
        /* soc_intr_enable(unit, IRQ_SCHAN_ERR); */
        /* Link status updates */
        if (!SOC_IS_RCPU_ONLY(unit)) {
            soc_cmicm_intr1_enable(unit, IRQ_CMCx_LINK_STAT_MOD);
        }

        soc_cmic_uc_msg_init(unit);     /* Init the uC area */

        for (i = 0; i < SOC_CMCS_NUM(unit); i++) {
            /* Clear the CPU & ARM queues */
            SHR_BITCLR_RANGE(CPU_ARM_QUEUE_BITMAP(unit, i),
                             0, NUM_CPU_COSQ(unit));
            if (i == SOC_PCI_CMC(unit)) {
                NUM_CPU_ARM_COSQ(unit, i) =
                    soc_property_uc_get(unit, 0, spn_NUM_QUEUES,
                                        NUM_CPU_COSQ(unit));
            } else {
                /* Properties presume it is PCI for first CMC, then the UC's */
                j = (i < SOC_PCI_CMC(unit)) ? (i + 1) : i;
                NUM_CPU_ARM_COSQ(unit, i) =
                    soc_property_uc_get(unit, j, spn_NUM_QUEUES, 0);
            }

            cmc_numq = NUM_CPU_ARM_COSQ(unit, i);
            SHR_BITSET_RANGE(CPU_ARM_QUEUE_BITMAP(unit, i),
                             numq, cmc_numq);
            numq += cmc_numq;

            if (numq > NUM_CPU_COSQ(unit)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                                     "soc_do_init: total cpu and arm cosq %04x unexpected\n"),
                          numq));
            }
        }
    } else
#endif /* CMICM Support */
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        int                 i, j;
        int                 numq = 0, cmc_numq;

        SOC_IF_ERROR_RETURN(soc_cmicx_uc_msg_init(unit));      /* Init the uC area */

        for (i = 0; i < SOC_CMCS_NUM(unit); i++) {
            /* Clear the CPU & ARM queues */
            SHR_BITCLR_RANGE(CPU_ARM_QUEUE_BITMAP(unit, i),
                             0, NUM_CPU_COSQ(unit));
            if (i == SOC_PCI_CMC(unit)) {
                NUM_CPU_ARM_COSQ(unit, i) =
                    soc_property_uc_get(unit, 0, spn_NUM_QUEUES,
                                        NUM_CPU_COSQ(unit));
            } else {
                /* Properties presume it is PCI for first CMC, then the UC's */
                j = (i < SOC_PCI_CMC(unit)) ? (i + 1) : i;
                NUM_CPU_ARM_COSQ(unit, i) =
                    soc_property_uc_get(unit, j, spn_NUM_QUEUES, 0);
            }

            cmc_numq = NUM_CPU_ARM_COSQ(unit, i);
            SHR_BITSET_RANGE(CPU_ARM_QUEUE_BITMAP(unit, i),
                             numq, cmc_numq);
            numq += cmc_numq;

            if (numq > NUM_CPU_COSQ(unit)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                                     "soc_do_init: total cpu and arm cosq %04x unexpected\n"),
                          numq));
            }
        }
    } else
#endif /* CMICX Support */

        {
        if (!SOC_IS_RCPU_ONLY(unit)) {

        /*
         * PCI Bus Error and Parity Error Interrupts
         */
        soc_intr_enable(unit, IRQ_PCI_PARITY_ERR | IRQ_PCI_FATAL_ERR);

        /*
         * S-Channel Error Interrupt
         */
        soc_intr_enable(unit, IRQ_SCHAN_ERR);

        /*
         * S-Channel Operation Complete Interrupt.
         * This interrupt is enabled in soc_schan_op().
         *
         * The SCH_MSG_DONE bit must be cleared after chip reset, since it
         * defaults to 1.  Otherwise, the interrupt will occur as soon as
         * it's enabled.
         */
        soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_MSG_DONE_CLR);
        /*
         * MIIM Operation Complete Interrupt
         * This interrupt is enabled in cmic_miim_read/write().
         *
         * The SCH_MIIM_OP_DONE bit must be cleared after chip reset, since
         * it defaults to 1.  Otherwise, the interrupt will occur as soon as
         * it's enabled.
         */
        soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_MIIM_OP_DONE_CLR);

        /*
         * Link status updates
         */

        soc_intr_enable(unit, IRQ_LINK_STAT_MOD);

        } /* !SOC_IS_RCPU_ONLY(unit) */
    }

    soc->soc_flags |= SOC_F_INITED;

    if (!SOC_IS_RCPU_ONLY(unit)) {

    /***********************************************************************/
    /* It is legal to access SOC internal registers beyond this point.     */
    /*                                                                     */
    /* HOWEVER:                                                            */
    /*                                                                     */
    /* This init routine should perform the utter minimum necessary.       */
    /* soc_misc_init() and soc_mmu_init() are used for further init.       */
    /***********************************************************************/

#ifdef BCM_TRIUMPH_SUPPORT
     if (SOC_IS_TRIUMPH(unit) || SOC_IS_TRIUMPH2(unit)) {
        /* BP needs to be disabled for register tests to pass */
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, AUX_ARB_CONTROL_2r,
                            REG_PORT_ANY, ESM_BP_ENABLEf, 0));
    }
#endif
#ifdef BCM_TRIUMPH2_SUPPORT
    /* Make BIGMAC registers accesible for Triumph2, Apollo and Enduro */
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit) || SOC_IS_HURRICANE(unit)) {
        soc_port_t port;
        uint64              mac_ctrl;
        PBMP_PORT_ITER(unit, port) {
            if (!IS_HG_PORT(unit, port) && !IS_XE_PORT(unit, port)) {
                continue;
            }
            SOC_IF_ERROR_RETURN(READ_MAC_CTRLr(unit, port, &mac_ctrl));
            soc_reg64_field32_set(unit, MAC_CTRLr, &mac_ctrl, TXRESETf, 0);
            soc_reg64_field32_set(unit, MAC_CTRLr, &mac_ctrl, RXRESETf, 0);
            SOC_IF_ERROR_RETURN(WRITE_MAC_CTRLr(unit, port, mac_ctrl));
        }
    }
#endif
#ifdef BCM_SHADOW_SUPPORT
    /* Make sure registers which are actually memories in HW are reset */
    /* Also make the XMAC registers accesible */
    if (SOC_IS_SHADOW(unit)) {
        uint32 rval = 0;
        soc_port_t port;
        PBMP_PORT_ITER(unit, port) {
            if (!IS_HG_PORT(unit, port) && !IS_XE_PORT(unit, port) &&
                !IS_SCH_PORT(unit,port)) {
                continue;
            }
            SOC_IF_ERROR_RETURN(READ_XLPORT_XMAC_CONTROLr(unit, port, &rval));
            soc_reg_field_set(unit, XLPORT_XMAC_CONTROLr, &rval, XMAC_RESETf, 0);
            SOC_IF_ERROR_RETURN(WRITE_XLPORT_XMAC_CONTROLr(unit, port, rval));
        }
        rval = 0;
        soc_reg_field_set(unit, ISW2_MEM_INIT1r, &rval, RCV_IP_STATS_INITf, 1);
        soc_reg_field_set(unit, ISW2_MEM_INIT1r, &rval, RCV_COS_BYTES_INITf, 1);
        soc_reg_field_set(unit, ISW2_MEM_INIT1r, &rval, RCV_COS_PKTS_INITf, 1);
        soc_reg_field_set(unit, ISW2_MEM_INIT1r, &rval, RCV_VLAN_STATS_INITf, 1);
        soc_reg_field_set(unit, ISW2_MEM_INIT1r, &rval, RCV_HIGIG_CMD_STATS_INITf, 1);
        soc_reg_field_set(unit, ISW2_MEM_INIT1r, &rval, RCV_DROP_BYTES_INITf, 1);
        soc_reg_field_set(unit, ISW2_MEM_INIT1r, &rval, RCV_DROP_PKTS_INITf, 1);
        soc_reg_field_set(unit, ISW2_MEM_INIT1r, &rval, RCV_L2_BYTES_INITf, 1);
        soc_reg_field_set(unit, ISW2_MEM_INIT1r, &rval, RCV_L2_PKTS_INITf, 1);
        soc_reg_field_set(unit, ISW2_MEM_INIT1r, &rval, RCV_SKIP_STOP_STATS_INITf, 1);
        soc_reg_field_set(unit, ISW2_MEM_INIT1r, &rval, RCV_DROP_AGG_INITf, 1);
        soc_reg_field_set(unit, ISW2_MEM_INIT1r, &rval, RCV_IPHCKS_INITf, 1);
        SOC_IF_ERROR_RETURN(WRITE_ISW2_MEM_INIT1r(unit, rval));

        rval = 0;
        soc_reg_field_set(unit, ISW2_MEM_INIT2r, &rval, RDBGC0_INITf, 1);
        soc_reg_field_set(unit, ISW2_MEM_INIT2r, &rval, RDBGC1_INITf, 1);
        soc_reg_field_set(unit, ISW2_MEM_INIT2r, &rval, RDBGC2_INITf, 1);
        soc_reg_field_set(unit, ISW2_MEM_INIT2r, &rval, RDBGC3_INITf, 1);
        soc_reg_field_set(unit, ISW2_MEM_INIT2r, &rval, RDBGC4_INITf, 1);
        soc_reg_field_set(unit, ISW2_MEM_INIT2r, &rval, RDBGC5_INITf, 1);
        soc_reg_field_set(unit, ISW2_MEM_INIT2r, &rval, RDBGC6_INITf, 1);
        soc_reg_field_set(unit, ISW2_MEM_INIT2r, &rval, RDBGC7_INITf, 1);
        soc_reg_field_set(unit, ISW2_MEM_INIT2r, &rval, RDBGC8_INITf, 1);
        SOC_IF_ERROR_RETURN(WRITE_ISW2_MEM_INIT2r(unit, rval));
    }
#endif

    } /* !SOC_IS_RCPU_ONLY */

    if (soc_property_get(unit, spn_L2XMSG_SHADOW_HIT_BITS, 1)) {
        soc->l2x_shadow_hit_bits |= L2X_SHADOW_HIT_BITS;
    } else {
        soc->l2x_shadow_hit_bits = 0;
    }
    if (soc_property_get(unit, spn_L2XMSG_SHADOW_HIT_SRC, 0)) {
        soc->l2x_shadow_hit_bits |= L2X_SHADOW_HIT_SRC;
    }
    if (soc_property_get(unit, spn_L2XMSG_SHADOW_HIT_DST, 0)) {
        soc->l2x_shadow_hit_bits |= L2X_SHADOW_HIT_DST;
    }

#ifdef BCM_XGS_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_dual_hash)) {
        SOC_DUAL_HASH_MOVE_MAX(unit) = soc_property_get(unit,
                                       spn_DUAL_HASH_RECURSE_DEPTH,
                                       SOC_MEM_DUAL_HASH_RECURSE_DEPTH);
        SOC_DUAL_HASH_MOVE_MAX_L2X(unit) = soc_property_get(unit,
                                   spn_DUAL_HASH_RECURSE_DEPTH_L2X,
                                   soc_dual_hash_recurse_depth_get(unit, L2Xm));
        SOC_DUAL_HASH_MOVE_MAX_MPLS(unit) = soc_property_get(unit,
                            spn_DUAL_HASH_RECURSE_DEPTH_MPLS,
                            soc_dual_hash_recurse_depth_get(unit, MPLS_ENTRYm));
        SOC_DUAL_HASH_MOVE_MAX_VLAN(unit) = soc_property_get(unit,
                            spn_DUAL_HASH_RECURSE_DEPTH_VLAN,
                            soc_dual_hash_recurse_depth_get(unit, VLAN_XLATEm));
        SOC_DUAL_HASH_MOVE_MAX_EGRESS_VLAN(unit) = soc_property_get(unit,
                        spn_DUAL_HASH_RECURSE_DEPTH_EGRESS_VLAN,
                        soc_dual_hash_recurse_depth_get(unit, EGR_VLAN_XLATEm));
#if defined(INCLUDE_L3)
        SOC_DUAL_HASH_MOVE_MAX_L3X(unit) = soc_property_get(unit,
                       spn_DUAL_HASH_RECURSE_DEPTH_L3X,
                       soc_dual_hash_recurse_depth_get(unit, L3_ENTRY_ONLYm));
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
        SOC_DUAL_HASH_MOVE_MAX_ING_DNAT_ADDRESS_TYPE(unit) = soc_property_get(unit,
                spn_DUAL_HASH_RECURSE_DEPTH_DNAT_POOL,
                soc_dual_hash_recurse_depth_get(unit, ING_DNAT_ADDRESS_TYPEm));
        SOC_DUAL_HASH_MOVE_MAX_ING_VP_VLAN_MEMBER(unit) = soc_property_get(unit,
                spn_DUAL_HASH_RECURSE_DEPTH_ING_VP_VLAN_MEMBER,
                soc_dual_hash_recurse_depth_get(unit, ING_VP_VLAN_MEMBERSHIPm));
        SOC_DUAL_HASH_MOVE_MAX_EGR_VP_VLAN_MEMBER(unit) = soc_property_get(unit,
                spn_DUAL_HASH_RECURSE_DEPTH_EGR_VP_VLAN_MEMBER,
                soc_dual_hash_recurse_depth_get(unit, EGR_VP_VLAN_MEMBERSHIPm));
#endif /* BCM_TRIDENT2_SUPPORT */
    }
    if (soc_feature(unit, soc_feature_ism_memory) || soc_feature(unit, soc_feature_shared_hash_mem)) {
        SOC_MULTI_HASH_MOVE_MAX(unit) = soc_property_get(unit,
                                        spn_MULTI_HASH_RECURSE_DEPTH,
                                        SOC_MEM_MULTI_HASH_RECURSE_DEPTH);
        SOC_MULTI_HASH_MOVE_MAX_L2(unit) = soc_property_get(unit,
                           spn_MULTI_HASH_RECURSE_DEPTH_L2,
                           soc_multi_hash_recurse_depth_get(unit, L2_ENTRY_1m));
        SOC_MULTI_HASH_MOVE_MAX_MPLS(unit) = soc_property_get(unit,
                           spn_MULTI_HASH_RECURSE_DEPTH_MPLS,
                           soc_multi_hash_recurse_depth_get(unit, MPLS_ENTRYm));
        SOC_MULTI_HASH_MOVE_MAX_VLAN(unit) = soc_property_get(unit,
                           spn_MULTI_HASH_RECURSE_DEPTH_VLAN,
                           soc_multi_hash_recurse_depth_get(unit, VLAN_XLATEm));
        SOC_MULTI_HASH_MOVE_MAX_VLAN_1(unit) = soc_property_get(unit,
                           spn_MULTI_HASH_RECURSE_DEPTH_VLAN_1,
                           soc_multi_hash_recurse_depth_get(unit, VLAN_XLATE_1_SINGLEm));
        SOC_MULTI_HASH_MOVE_MAX_VLAN_2(unit) = soc_property_get(unit,
                           spn_MULTI_HASH_RECURSE_DEPTH_VLAN_2,
                           soc_multi_hash_recurse_depth_get(unit, VLAN_XLATE_2_SINGLEm));
        SOC_MULTI_HASH_MOVE_MAX_EGRESS_VLAN(unit) = soc_property_get(unit,
                       spn_MULTI_HASH_RECURSE_DEPTH_EGRESS_VLAN,
                       soc_multi_hash_recurse_depth_get(unit, EGR_VLAN_XLATEm));
        SOC_MULTI_HASH_MOVE_MAX_EGRESS_VLAN_1(unit) = soc_property_get(unit,
                       spn_MULTI_HASH_RECURSE_DEPTH_EGRESS_VLAN_1,
                       soc_multi_hash_recurse_depth_get(unit, EGR_VLAN_XLATE_1_SINGLEm));
        SOC_MULTI_HASH_MOVE_MAX_EGRESS_VLAN_2(unit) = soc_property_get(unit,
                       spn_MULTI_HASH_RECURSE_DEPTH_EGRESS_VLAN_2,
                       soc_multi_hash_recurse_depth_get(unit, EGR_VLAN_XLATE_2_SINGLEm));
        SOC_MULTI_HASH_MOVE_MAX_L3(unit) = soc_property_get(unit,
                           spn_MULTI_HASH_RECURSE_DEPTH_L3,
                           soc_multi_hash_recurse_depth_get(unit, L3_ENTRY_1m));
        SOC_MULTI_HASH_MOVE_MAX_FPEM(unit) = soc_property_get(unit,
                           spn_MULTI_HASH_RECURSE_DEPTH_EXACT_MATCH,
                           soc_multi_hash_recurse_depth_get(unit, EXACT_MATCH_2m));
    }
    if (soc_feature(unit, soc_feature_shared_hash_mem)) {
        /*
         * Chips following TH2 are recommended to use MULTI_MOVE_MODE_BREADTH
         * mode if applicable.
         */
        SOC_MULTI_HASH_MOVE_ALGORITHM(unit) = soc_property_get(unit,
                           "multi_hash_move_algorithm",
                           (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) ?
                           MULTI_MOVE_MODE_BREADTH : MULTI_MOVE_MODE_MIX);
    }
#endif /* BCM_XGS_SWITCH_SUPPORT */

    /* Backwards compatible default */
    SOC_MCAST_ADD_ALL_ROUTER_PORTS(unit) = 1;

#ifdef BCM_XGS_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_arl_hashed)) {
        SOC_IF_ERROR_RETURN(soc_l2x_init(unit));
    }
#endif

    /* Set maximum supported vrf id for the device. */
    SOC_IF_ERROR_RETURN(soc_max_supported_vrf_get(unit, &max_class));
    SOC_VRF_MAX_SET(unit, max_class);
    /* Set maximum supported address class for the device. */
    SOC_IF_ERROR_RETURN(soc_max_supported_addr_class_get(unit, &max_class));
    SOC_ADDR_CLASS_MAX_SET(unit, max_class);

   /* If the feature is supported, set the maximum supported over-laid
      address class for the device. */
    if (soc_feature(unit, soc_feature_overlaid_address_class)) {
        SOC_IF_ERROR_RETURN
            (soc_max_supported_overlaid_addr_class_get(unit, &max_class));
        SOC_OVERLAID_ADDR_CLASS_MAX_SET(unit, max_class);
    }

    /* For the TR3 family of devices, set the extended classID */
    if (soc_feature(unit, soc_feature_extended_address_class)) {
        SOC_IF_ERROR_RETURN
            (soc_max_supported_extended_addr_class_get(unit, &max_class));
        SOC_EXT_ADDR_CLASS_MAX_SET(unit, max_class);
    }

    if (soc_feature(unit, soc_feature_src_mac_group)) {
        SOC_L2X_GROUP_ENABLE_SET(unit, TRUE);
    } else {
        SOC_L2X_GROUP_ENABLE_SET(unit, FALSE);
    }

    /* Set maximum supported address class for the device. */
    SOC_IF_ERROR_RETURN(soc_max_supported_intf_class_get(unit, &max_class));
    SOC_INTF_CLASS_MAX_SET(unit, max_class);

    /* Create egress metering mutex. */
    if ((soc->egressMeteringMutex =
         sal_mutex_create("port_rate_egress_lock")) == NULL) {
        return (SOC_E_MEMORY);
    }

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_KATANA2_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_APACHE_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_KATANAX(unit) || SOC_IS_TRIDENT2X(unit) ||
        SOC_IS_TITAN2X(unit) || SOC_IS_TRIUMPH3(unit)) {
        /* Create llsMutex. */
        if ((soc->llsMutex =
            sal_mutex_create("lls_scheduler_lock")) == NULL) {
            return (SOC_E_MEMORY);
        }
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
        /* intialize port_flush_pbmp */
        SOC_PBMP_CLEAR(soc->port_flush_pbmp);
#endif
    }
#endif

#if defined(BCM_TOMAHAWK_SUPPORT)
        if (SOC_IS_TOMAHAWKX(unit)) {
            /* Create idb_lock */
            if ((soc->idb_lock = sal_mutex_create("idb_lock")) == NULL) {
                return (SOC_E_MEMORY);
            }
        }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT)
         if (soc_feature(unit, soc_feature_epc_linkflap_recover)) {
            /* Create linkscan_modport_map_lock */
            if ((soc->linkscan_modport_map_lock =
                sal_mutex_create("linkscan_modport_map_lock")) == NULL) {
                return (SOC_E_MEMORY);
            }
         }
#endif /* #(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT) */
    if(soc_property_get(unit, spn_MEM_CLEAR_HW_ACCELERATION, 1)) {
        SOC_MEM_CLEAR_USE_DMA_SET(unit, TRUE);
    }

    mem_clear_chunk_size =
        soc_property_get(unit, spn_MEM_CLEAR_CHUNK_SIZE, 4096);
    SOC_MEM_CLEAR_CHUNK_SIZE_SET(unit, mem_clear_chunk_size);

    if (soc_property_get(unit, spn_GPORT, FALSE)) {
          SOC_USE_GPORT_SET(unit, TRUE);
    }
#ifdef BCM_ASSERT_EVENT_ENABLE
    sal_assert_set(soc_event_assert);
#endif /* BCM_ASSERT_EVENT_ENABLE */

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
#ifdef BCM_ISM_SUPPORT
        /* Retreive hash config data */
        if (soc_feature(unit, soc_feature_ism_memory)) {
            uint8 hcfg;
            uint8 offsets[_SOC_ISM_MAX_BANKS], count;

            rv = soc_ism_table_hash_config_get(unit, SOC_ISM_MEM_VLAN_XLATE,
                                               &hcfg);
            rv |= soc_ism_table_hash_config_get(unit, SOC_ISM_MEM_L2_ENTRY,
                                                &hcfg);
            rv |= soc_ism_table_hash_config_get(unit, SOC_ISM_MEM_L3_ENTRY,
                                                &hcfg);
            rv |= soc_ism_table_hash_config_get(unit, SOC_ISM_MEM_EP_VLAN_XLATE,
                                                &hcfg);
            rv |= soc_ism_table_hash_config_get(unit, SOC_ISM_MEM_MPLS, &hcfg);
            if (rv != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Error retreiving ISM hash zero_lsb(s) !!\n")));
                return SOC_E_INTERNAL;
            }
            rv = soc_ism_hash_table_offset_config_get(unit, SOC_ISM_MEM_VLAN_XLATE,
                                                      offsets, &count);
            rv |= soc_ism_hash_table_offset_config_get(unit, SOC_ISM_MEM_L2_ENTRY,
                                                       offsets, &count);
            rv |= soc_ism_hash_table_offset_config_get(unit, SOC_ISM_MEM_L3_ENTRY,
                                                       offsets, &count);
            rv |= soc_ism_hash_table_offset_config_get(unit, SOC_ISM_MEM_EP_VLAN_XLATE,
                                                       offsets, &count);
            rv |= soc_ism_hash_table_offset_config_get(unit, SOC_ISM_MEM_MPLS,
                                                       offsets, &count);
            if (rv != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Error retreiving ISM hash offset(s) !!\n")));
                return SOC_E_INTERNAL;
            }
        }
#endif /* BCM_ISM_SUPPORT */
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_static_repl_head_alloc)) {
        soc->repl_eligible_pbmp = soc_property_get_pbmp_default(unit,
                spn_REPLICATION_ELIGIBLE_PBMP, PBMP_ALL(unit));
    }
#endif

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (!SAL_BOOT_BCMSIM && !SAL_BOOT_QUICKTURN
        && !SAL_BOOT_XGSSIM && !SOC_WARM_BOOT(unit)) {
        if (soc_feature(unit, soc_feature_td2p_ovstb_toggle)) {
            trident2p_ovstb_toggle(unit);
        }
    }
#endif

#if defined(CANCUN_SUPPORT)
    if (soc_feature(unit, soc_feature_cancun)) {
        soc_cancun_init(unit);
    }
    if (soc_feature(unit, soc_feature_flex_flow)) {
        soc_flow_db_init(unit);
    }
#endif
#ifdef INCLUDE_AVS
    if (soc_feature(unit, soc_feature_avs_openloop)) {
        soc_avs_deinit(unit);
        soc_avs_init(unit);
    }
#endif /* INCLUDE_AVS */

    /* Set flex counter feature for new Ranger3+ parts */
#ifdef BCM_TOMAHAWK_SUPPORT
    SOC_IF_ERROR_RETURN(soc_flex_counter_feature_init(unit));
#endif

    /* Start to allow this unit to run DPCs */
    sal_dpc_enable(INT_TO_PTR(unit));

    if (soc_feature(unit,
            soc_feature_l2_fifo_mode_station_move_mac_invalid)) {
        /* For us to enable the station move SW lookup processing,
         * spn_MEM_CACHE_ENABLE config property needs to be enabled
         */
        if (soc_property_get(unit, spn_MEM_CACHE_ENABLE, 1)) {
            soc->l2_fifo_mode_station_move_sw_lookup = 1;
        } else {
            soc->l2_fifo_mode_station_move_sw_lookup = 0;
        }
    } else {
        soc->l2_fifo_mode_station_move_sw_lookup = 0;
    }

    return(SOC_E_NONE);
}

#ifdef BCM_WARM_BOOT_SUPPORT
#define SOC_INTERRUPT_SLEEP_MAX_TIMES  1000
/*
 * Function:
 *      soc_interrupt_service_wait
 * Purpose:
 *      Check interrupt masks on various lines and keep waiting until they
 *      are all serviced.
 * Parameters:
 *      unit - (IN) Device Number
 * Returns:
 *      SOC_E_NONE once all masks are clear, error if some soc call fails
 */
STATIC soc_error_t
soc_interrupt_service_wait(int unit)
{
    uint32    status = 0;
    uint32    count = 0;
    uint8     error_sleep = FALSE;
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICDV2_SUPPORT) || \
    defined(BCM_TRIDENT2_SUPPORT)
    uint32    mask = 0;
    int       cmc = SOC_PCI_CMC(unit);
#endif

    while (1) {
        /* Keep checking the status of the IRQ line until you hit a non-zero
         * value, if non-zero stop the check and sleep to yield cpu for the
         * interrupt handling to complete. If the status is zero at the end
         * of all checks, just return, indicating no interrupts left to be
         * handled.
         */
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICDV2_SUPPORT)
        /* For CMICd/m devices check the relevant IRQ status registers */
        if (soc_feature(unit, soc_feature_cmicd_v2) ||
            soc_feature(unit, soc_feature_cmicm)) {
            status = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT3_OFFSET(cmc));
            mask = SOC_CMCx_IRQ3_MASK(unit, cmc);
            status &= mask;

            /*
            * If the mask is disabled by the interrupt handler
            * try to sleep a while to wait for bcmDpc thread handles ser interrupt
            */
            if (mask == 0) {
                LOG_VERBOSE(BSL_LS_SOC_SER,
                            (BSL_META_U(unit,
                                        "IRQ3 Mask is zero!\n")));
                error_sleep = TRUE;
                goto sleep;
            }
            if (soc_feature(unit, soc_feature_cmicd_v2) &&
                (!status)) {
                status = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT4_OFFSET(cmc));
                mask = SOC_CMCx_IRQ4_MASK(unit, cmc);
                status &= mask;
                if (!SOC_IS_GREYHOUND(unit) && (mask == 0)) {
                    /* Continuous interrupt (not parity error) on GH */
                    LOG_VERBOSE(BSL_LS_SOC_SER,
                                (BSL_META_U(unit,
                                            "IRQ4 Mask is zero!\n")));
                    error_sleep = TRUE;
                    goto sleep;
                }
            }
        }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
        if ((SOC_IS_TRIDENT2(unit) || SOC_IS_TITAN2(unit) ||
             SOC_IS_TD2P_TT2P(unit)) && (!status)) {
            status = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT2_OFFSET(cmc));
            mask = SOC_CMCx_IRQ2_MASK(unit, cmc);
            status &= mask;
            if (mask == 0) {
                LOG_VERBOSE(BSL_LS_SOC_SER,
                            (BSL_META_U(unit,
                                        "IRQ2 Mask is zero!\n")));
                error_sleep = TRUE;
                goto sleep;
            }
        }
#endif /* BCM_TRIDENT2_SUPPORT */

#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICDV2_SUPPORT) || \
    defined(BCM_TRIDENT2_SUPPORT)
sleep:
#endif
        /* If the status not clear yet, sleep a bit, so as to complete the
         * interrupt handling. If they are clear, break.
         */
        if (++count > SOC_INTERRUPT_SLEEP_MAX_TIMES) {
            /* To avoid dead loop */
            break;
        }
        if (status) {
            sal_usleep(1000);
            status = 0;
        } else if (error_sleep) {
            sal_usleep(1000);
            error_sleep = FALSE;
        } else {
            break;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_shutdown
 * Purpose:
 *      Free up SOC resources without touching hardware
 * Parameters:
 *      unit - StrataSwitch unit #
 * Returns:
 *      SOC_E_XXX
 */
int
soc_shutdown(int unit)
{
    soc_control_t       *soc;
    soc_mem_t           mem, act_mem, lock_mem;
    int                 ix;
    int                 cmc;
#ifdef INCLUDE_KNET
    uint8               shutdown_knet = TRUE;
#endif
    int warm_boot;

#ifdef INCLUDE_KNET
    if (SOC_KNET_MODE(unit) &&
        soc_property_get(unit, spn_WARMBOOT_KNET_SHUTDOWN_MODE, 0)) {
        shutdown_knet = FALSE;
    }
#endif

    if (!SOC_UNIT_VALID(unit)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_shutdown: unit %d not valid\n"), unit));
        return SOC_E_UNIT;
    }

    soc = SOC_CONTROL(unit);

    if (!(soc->soc_flags & SOC_F_ATTACHED)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_shutdown: unit %d not attached\n"), unit));
        return(SOC_E_UNIT);
    }

#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {
        soc_cmic_uc_msg_apps_notify(unit, SOC_CMIC_UC_SHUTDOWN_NOHALT);
    }
#endif /* BCM_CMICM_SUPPORT */

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        /* Disable adaptive irq poll. */
        soc->irq_adaptive_poll_enable = 0;
        soc_cmic_uc_msg_apps_notify(unit, SOC_CMIC_UC_SHUTDOWN_NOHALT);
    }
    if (soc_feature(unit, soc_feature_cmicx) && !(SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM)) {
        LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit,"soc_shutdown: iproc m0 exit\n")));
        /* Stop LED FW */
        soc_cmicx_led_enable(unit, LED_STOP);
        /* Iproc M0 exit */
        soc_iproc_m0_exit(unit);
    }
#endif /* BCM_CMICX_SUPPORT */

    if (soc->soc_flags & SOC_F_INITED) {
#ifdef BCM_HERCULES_SUPPORT
        /* Dump the MMU error stats */
        soc_mmu_error_done(unit);

        if (soc->lla_cells_good != NULL) {
            sal_free(soc->lla_cells_good);
            soc->lla_cells_good = NULL;
        }

        if (soc->lla_map != NULL) {
            int port;
            PBMP_PORT_ITER(unit, port) {
               if (soc->lla_map[port] != NULL) {
                   sal_free(soc->lla_map[port]);
               }
            }
            sal_free(soc->lla_map);
            soc->lla_map = NULL;
        }

        if (soc->sbe_disable != NULL) {
            sal_free(soc->sbe_disable);
            soc->sbe_disable = NULL;
        }
#endif

#ifdef  INCLUDE_I2C
        SOC_IF_ERROR_RETURN(soc_i2c_detach(unit)); /* Free up I2C driver mem */
#endif

        if (!SOC_IS_RCPU_ONLY(unit)) {
            /* Free up DMA memory */
            SOC_IF_ERROR_RETURN(soc_dma_detach(unit));
        }

        if (soc->arlShadow != NULL) {
            sal_mutex_take(soc->arlShadowMutex, sal_mutex_FOREVER);
            shr_avl_delete_all(soc->arlShadow);
            sal_mutex_give(soc->arlShadowMutex);
        }

#ifdef BCM_TOMAHAWK3_SUPPORT
        /* Free AVL table used for L2 learning */
        if ((SOC_IS_TOMAHAWK3(unit)) && (soc->l2x_lrn_shadow != NULL)) {
            sal_mutex_take(soc->l2x_lrn_shadow_mutex, sal_mutex_FOREVER);
            shr_avl_delete_all(soc->l2x_lrn_shadow);
            sal_mutex_give(soc->l2x_lrn_shadow_mutex);
        }

        if (SOC_IS_TOMAHAWK3(unit)) {
            (void)soc_th3_l2x_appl_callback_entry_delete_all(unit);
        }
#endif /* BCM_TOMAHAWK3_SUPPORT */

        warm_boot = SOC_WARM_BOOT(unit);
        if (!warm_boot) {
            SOC_WARM_BOOT_START(unit);
        }

        /* Terminate counter module; frees allocated space */
        soc_counter_detach(unit);

        if (!warm_boot) {
            SOC_WARM_BOOT_DONE(unit);
        }

#ifdef BCM_XGS_SWITCH_SUPPORT

        if (soc_property_get(unit, spn_PARITY_ENABLE, TRUE) &&
            SOC_CONTROL(unit)->mem_scache_ptr == NULL) {
            SOC_IF_ERROR_RETURN(soc_system_scrub(unit));

            /* There could be instances where all outstanding interrupts
             * are not serviced yet, we wait here to ensure the interrupt
             * mask on relevant irq lines are reset before we shut down
             * the DPC processing
             */
            if (SOC_FAILURE(soc_interrupt_service_wait(unit))) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                            "soc_shutdown: soc_interrupt_service_wait "
                            "ran into errors!\n")));
            }
        }

        if (soc_feature(unit, soc_feature_arl_hashed)) {
            /* Stop L2X thread */
            SOC_IF_ERROR_RETURN(soc_l2x_stop(unit));
        }
#ifdef BCM_TRIUMPH3_SUPPORT
        if SOC_IS_TRIUMPH3(unit) {
            SOC_IF_ERROR_RETURN(soc_tr3_l2_bulk_age_stop(unit));
        }
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
        if SOC_IS_TRIDENT2X(unit) {
            SOC_IF_ERROR_RETURN(soc_td2_l2_bulk_age_stop(unit));
        }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWK(unit) || SOC_IS_TOMAHAWK2(unit) ||
            SOC_IS_TRIDENT3X(unit)) {
            SOC_IF_ERROR_RETURN(soc_th_l2_bulk_age_stop(unit));
        }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef ALPM_ENABLE
#if defined(BCM_TOMAHAWK_SUPPORT)
        if (soc_property_get(unit, spn_L3_ALPM_ENABLE, 0) &&
            soc_feature(unit, soc_feature_alpm2)) {
            SOC_IF_ERROR_RETURN(alpm_dist_hitbit_thread_stop(unit));
        }
#endif
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    /* Stop L2 learning and aging threads */
    if (SOC_IS_TOMAHAWK3(unit)) {
        SOC_IF_ERROR_RETURN(soc_th3_l2_age_stop(unit));
        SOC_IF_ERROR_RETURN(soc_th3_l2_learn_thread_stop(unit));
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

    if (soc_feature(unit, soc_feature_arl_hashed)) {
        /* Stop L2X thread */
        SOC_IF_ERROR_RETURN(soc_l2x_detach(unit));
    }

#ifdef BCM_SBUSDMA_SUPPORT
        if (SOC_SBUSDMA_DM_INFO(unit)) {
            SOC_IF_ERROR_RETURN(soc_sbusdma_desc_detach(unit));
        }
#endif /* BCM_SBUSDMA_SUPPORT */

#ifdef BCM_CCMDMA_SUPPORT
        SOC_IF_ERROR_RETURN(soc_ccmdma_deinit(unit));
#endif

#ifdef BCM_FIFODMA_SUPPORT
        SOC_IF_ERROR_RETURN(soc_fifodma_deinit(unit));
#endif

#ifdef CANCUN_SUPPORT
        if (soc_feature(unit, soc_feature_flex_flow)) {
            soc_flow_db_deinit(unit);
        }
        if (soc_feature(unit, soc_feature_cancun)) {
            soc_cancun_deinit(unit);
        }
#endif

#endif /* BCM_XGS_SWITCH_SUPPORT */

        soc->soc_flags &= ~SOC_F_INITED;
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        soc_cmic_intr_all_disable(unit);
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            SOC_IF_ERROR_RETURN(soc_trident3_pvt_intr_disable(unit));
        }
#endif /* BCM_TRIDENT3_SUPPORT */

        } else
#endif
#ifdef BCM_CMICM_SUPPORT
        if (soc_feature(unit, soc_feature_cmicm)) {
            soc_cmicm_intr0_disable(unit, ~0);
            soc_cmicm_intr1_disable(unit, ~0);
            soc_cmicm_intr2_disable(unit, ~0);
#ifdef BCM_CMICDV2_SUPPORT
            if (soc_feature(unit, soc_feature_cmicd_v2)) {
                soc_cmicm_intr3_disable(unit, ~0);
                soc_cmicm_intr4_disable(unit, ~0);
                soc_cmicm_intr5_disable(unit, ~0);
#ifdef BCM_CMICDV4_SUPPORT
                if (soc_feature(unit, soc_feature_cmicd_v4)) {
                    soc_cmicm_intr6_disable(unit, ~0);
                }
#endif /* BCM_CMICDV4_SUPPORT */
            }
#endif /* BCM_CMICDV2_SUPPORT */
        } else
#endif
        {
            soc_intr_disable(unit, ~0);
        }

        /* Shutdown polled interrupt mode if active */
        if (soc->soc_flags & SOC_F_POLLED) {
            soc_ipoll_disconnect(unit);
            soc->soc_flags &= ~SOC_F_POLLED;
        }

        /* Detach interrupt handler, if we installed one */
        /* unit # is ISR arg */
        if (soc_cm_interrupt_disconnect(unit) < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_shutdown: could not disconnect interrupt line\n")));
            return SOC_E_INTERNAL;
        }

        /* Disable shutting down unit to run new DPCs and wait until all
         * existing DPCs are completed.
         */
        sal_dpc_disable_and_wait(INT_TO_PTR(unit));

        /*
         * When detaching, take care to free up only resources that were
         * actually allocated, in case we are cleaning up after an attach
         * that failed part way through.
         */
        if (soc->arlBuf != NULL) {
            soc_cm_sfree(unit, (void *)soc->arlBuf);
            soc->arlBuf = NULL;
        }

        if (soc->arlShadow != NULL) {
            shr_avl_destroy(soc->arlShadow);
            soc->arlShadow = NULL;
        }

        if (soc->arlShadowMutex != NULL) {
            sal_mutex_destroy(soc->arlShadowMutex);
            soc->arlShadowMutex = NULL;
        }

#ifdef BCM_TOMAHAWK3_SUPPORT
        /* Free resources allocated for L2 learning */
        if (SOC_IS_TOMAHAWK3(unit)) {
            if (soc->l2x_lrn_shadow != NULL) {
                shr_avl_destroy(soc->l2x_lrn_shadow);
                soc->l2x_lrn_shadow = NULL;
            }

            if (soc->l2x_lrn_shadow_mutex != NULL) {
                sal_mutex_destroy(soc->l2x_lrn_shadow_mutex);
                soc->l2x_lrn_shadow_mutex = NULL;
            }

            (void)soc_th3_l2x_appl_callback_resources_destroy(unit);
        }
#endif /* BCM_TOMAHAWK3_SUPPORT */

#if defined(BCM_ESW_SUPPORT) && defined(BCM_IPROC_SUPPORT)
        /* De-Initialize bond info cache */
        if (soc_feature(unit, soc_feature_untethered_otp)) {
            if (soc_bond_info_deinit(unit) < 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "soc_shutdown: could not de-initialize "
                                      "bond info structure\n")));
                return SOC_E_INTERNAL;
            }
        }
#endif /* BCM_ESW_SUPPORT && BCM_IPROC_SUPPORT */


#ifdef SOC_ROBUST_HASH
        if (soc_feature(unit, soc_feature_robust_hash)) {
            if (soc->soc_robust_hash_config != NULL) {
                sal_free(soc->soc_robust_hash_config);
                soc->soc_robust_hash_config = NULL;
            }
        }
#endif /* SOC_ROBUST_HASH */

#ifdef INCLUDE_MEM_SCAN
        SOC_IF_ERROR_RETURN(soc_mem_scan_stop(unit)); /* Stop memory scanner */
        if (soc->mem_scan_notify) {
            sal_sem_destroy(soc->mem_scan_notify);
        }
#endif
#ifdef BCM_SRAM_SCAN_SUPPORT
        SOC_IF_ERROR_RETURN(soc_sram_scan_stop(unit)); /* Stop memory scanner */
        if (soc->sram_scan_notify) {
            sal_sem_destroy(soc->sram_scan_notify);
        }
#endif
        for (mem = 0; mem < NUM_SOC_MEM; mem++) {
            if (!SOC_MEM_IS_VALID(unit, mem)) {
                continue;
            }

            act_mem = mem;
#ifdef BCM_ESW_SUPPORT
            soc_drv_mem_reuse_mem_state(unit, &act_mem, &lock_mem);
#endif
            /* Deallocate table cache memory, if caching enabled */
            SOC_IF_ERROR_RETURN(soc_mem_cache_set(unit, act_mem, COPYNO_ALL, FALSE));
        }

        /* LOCK used in cache_set, so destroy later */
        for (mem = 0; mem < NUM_SOC_MEM; mem++) {
            if (!SOC_MEM_IS_VALID(unit, mem)) {
                continue;
            }

            act_mem = mem;
            lock_mem = mem;
#ifdef BCM_ESW_SUPPORT
            soc_drv_mem_reuse_mem_state(unit, &act_mem, &lock_mem);
#endif
            if (soc->memState[lock_mem].lock != NULL) {
                sal_mutex_destroy(soc->memState[lock_mem].lock);
                soc->memState[lock_mem].lock = NULL;
            }
#ifdef BCM_ESW_SUPPORT
            /* Share the lock */
            soc_drv_mem_sync_mapped_lock(unit, mem, soc->memState[lock_mem].lock);
#endif
        }
#ifdef BCM_ESW_SUPPORT
        if (SOC_MEM_IS_VALID(unit, ING_DEVICE_PORTm) &&
            !SOC_MEM_IS_VALID(unit, PORT_TABm)) {
            soc->memState[PORT_TABm].lock = soc->memState[ING_DEVICE_PORTm].lock;
        }
#endif

        if (soc->schanMutex) {
            sal_mutex_destroy(soc->schanMutex);
            soc->schanMutex = NULL;
        }

#ifdef BCM_CMICM_SUPPORT
        if (soc->fschanMutex) {
            sal_mutex_destroy(soc->fschanMutex);
            soc->fschanMutex = NULL;
        }
#endif
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)
#ifdef BCM_CCMDMA_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm) ||
        soc_feature(unit, soc_feature_cmicx)) {
        for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit); cmc++) {
            if (soc->ccmDmaIntr[cmc]) {
                sal_sem_destroy(soc->ccmDmaIntr[cmc]);
                soc->ccmDmaIntr[cmc] = NULL;
            }
        }
    }
#endif
#endif
#ifdef BCM_CMICM_SUPPORT
        if (soc_feature(unit, soc_feature_cmicm)) {
            soc_cmic_uc_msg_stop(unit);
        }
#endif
#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx)) {
            soc_cmic_uc_msg_stop(unit);
        }
#endif
        for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit) + 1; cmc++) {
            if (soc->schanIntr[cmc]) {
                sal_sem_destroy(soc->schanIntr[cmc]);
                soc->schanIntr[cmc] = NULL;
            }
        }

        if (soc->miimMutex) {
            sal_mutex_destroy(soc->miimMutex);
            soc->miimMutex = NULL;
        }

        (void)soc_sbusdma_lock_deinit(unit);

#ifdef BCM_CMICM_SUPPORT
        for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit); cmc++) {
            int ch;
            for (ch = 0; ch < N_DMA_CHAN; ch++) {
                if (soc->fifoDmaMutexs[cmc][ch]) {
                    sal_mutex_destroy(soc->fifoDmaMutexs[cmc][ch]);
                    soc->fifoDmaMutexs[cmc][ch] = NULL;
                }
                if (soc->fifoDmaIntrs[cmc][ch]) {
                    sal_sem_destroy(soc->fifoDmaIntrs[cmc][ch]);
                    soc->fifoDmaIntrs[cmc][ch] = NULL;
                }
            }
        }
#endif

        for (ix = 0; ix < 3; ix++) {
             if (soc->memCmdIntr[ix]) {
                 sal_sem_destroy(soc->memCmdIntr[ix]);
                 soc->memCmdIntr[ix] = NULL;
             }
        }

        if (soc->arl_notify) {
            sal_sem_destroy(soc->arl_notify);
            soc->arl_notify = NULL;
        }

#if defined(BCM_TRIUMPH3_SUPPORT)
        if (soc_feature(unit, soc_feature_esm_correction)) {
            SOC_IF_ERROR_RETURN(soc_tr3_esm_recovery_stop(unit));
            if (soc->esm_recovery_notify) {
                sal_sem_destroy(soc->esm_recovery_notify);
                soc->esm_recovery_notify = NULL;
            }
            if (soc->esm_lock) {
                sal_mutex_destroy(soc->esm_lock);
                soc->esm_lock = NULL;
            }
        }
#endif

#ifdef BCM_XGS_SWITCH_SUPPORT
        if (NULL != soc->l2x_notify) {
            sal_sem_destroy(soc->l2x_notify);
            soc->l2x_notify = NULL;
        }
        if (NULL != soc->l2x_del_sync) {
            sal_mutex_destroy(soc->l2x_del_sync);
            soc->l2x_del_sync = NULL;
        }
        if (NULL != soc->l2x_lock) {
            sal_sem_destroy(soc->l2x_lock);
            soc->l2x_lock = NULL;
        }
        soc->l2x_pid = SAL_THREAD_ERROR;
        soc->l2x_interval = 0;
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            /* Clear L2 learning related data */
            soc->l2x_learn_pid = SAL_THREAD_ERROR;
            soc->l2x_learn_interval = 0;

            soc->lrn_cache_threshold = -1;
            soc->lrn_cache_intr_ctl = -1;
            soc->lrn_cache_clr_on_rd = -1;
        }
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TSN_SUPPORT
        if (NULL != soc->l2x_sr_sync) {
            sal_mutex_destroy(soc->l2x_sr_sync);
            soc->l2x_sr_sync = NULL;
        }
#endif /* BCM_TSN_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit) || SOC_IS_TOMAHAWKX(unit) ||
            SOC_IS_TRIDENT3X(unit)) {
            if (NULL != soc->l2x_age_notify) {
                sal_sem_destroy(soc->l2x_age_notify);
                soc->l2x_age_notify = NULL;
            }
            soc->l2x_age_pid = SAL_THREAD_ERROR;
            soc->l2x_age_interval = 0;
            soc->l2x_agetime_adjust_usec = 0;
            soc->l2x_agetime_curr_timeblk = 0;
            soc->l2x_agetime_curr_timeblk_usec = 0;
            soc->l2x_prev_age_timeout = 0;
        }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
        if SOC_IS_TRIDENT2X(unit) {
            if (NULL != soc->l2x_age_notify) {
                sal_sem_destroy(soc->l2x_age_notify);
                soc->l2x_age_notify = NULL;
            }
            if (NULL != soc->l2x_age_sync) {
                sal_sem_destroy(soc->l2x_age_sync);
                soc->l2x_age_sync = NULL;
            }
            soc->l2x_age_pid = SAL_THREAD_ERROR;
            soc->l2x_age_interval = 0;
            soc->l2x_age_enable = 0;
            soc->l2x_sw_aging = 0;
        }
#endif
#endif /* BCM_XGS_SWITCH_SUPPORT */
        /* Destroy L3 module protection mutex. */
        if (NULL != soc->l3x_lock) {
            sal_mutex_destroy(soc->l3x_lock);
            soc->l3x_lock = NULL;
        }

        /* Destroy FP module protection mutex. */
        if (NULL != soc->fp_lock) {
            sal_mutex_destroy(soc->fp_lock);
            soc->fp_lock = NULL;
        }

#ifdef INCLUDE_XFLOW_MACSEC
        if (soc_feature(unit, soc_feature_xflow_macsec)) {
            /* Destroy XFLOW_MACSEC module protection mutex. */
            if (NULL != soc->xflow_macsec_lock) {
                sal_mutex_destroy(soc->xflow_macsec_lock);
                soc->xflow_macsec_lock = NULL;
            }
        }
#endif

        if (soc->ipfixIntr) {
            sal_sem_destroy(soc->ipfixIntr);
            soc->ipfixIntr = NULL;
        }

#if defined(INCLUDE_REGEX)
        if (soc->ftreportIntr) {
            sal_sem_destroy(soc->ftreportIntr);
            soc->ftreportIntr = NULL;
        }
#endif

#ifdef BCM_FLOWTRACKER_EXPORT_FIFO_SUPPORT
        if (soc->ftExportIntr) {
            sal_sem_destroy(soc->ftExportIntr);
            soc->ftExportIntr = NULL;
        }
#endif /* BCM_FLOWTRACKER_EXPORT_FIFO_SUPPORT */
        soc->ctr_evict_interval = 0;
        if (soc->ctrEvictIntr) {
            sal_sem_destroy(soc->ctrEvictIntr);
            soc->ctrEvictIntr = NULL;
        }

        /* Destroy egress metering mutex. */
        if (soc->egressMeteringMutex != NULL) {
            sal_mutex_destroy(soc->egressMeteringMutex);
            soc->egressMeteringMutex = NULL;
        }

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_KATANA2_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_APACHE_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_KATANAX(unit) || SOC_IS_TRIDENT2X(unit) ||
        SOC_IS_TITAN2X(unit) || SOC_IS_TRIUMPH3(unit)) {
            /* Destroy llsMutex */
            if (soc->llsMutex !=NULL) {
                sal_mutex_destroy(soc->llsMutex);
                soc->llsMutex = NULL;
            }
        }
#endif

#if defined(BCM_TOMAHAWK_SUPPORT)
        if (SOC_IS_TOMAHAWKX(unit)) {
            /* Destroy idb_lock */
            if (soc->idb_lock !=NULL) {
                sal_mutex_destroy(soc->idb_lock);
                soc->idb_lock = NULL;
            }
        }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT)
         if (soc_feature(unit, soc_feature_epc_linkflap_recover)) {
            /* Destroy linkscan_modport_map_lock_lock */
            if (soc->linkscan_modport_map_lock !=NULL) {
                sal_mutex_destroy(soc->linkscan_modport_map_lock);
                soc->linkscan_modport_map_lock = NULL;
            }
         }
#endif /* #(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT) */
        if (soc->soc_wc_ucode_dma_buf != NULL) {
            soc_cm_sfree(unit, soc->soc_wc_ucode_dma_buf);
            soc->soc_wc_ucode_dma_buf = NULL;
        }
        if (soc->soc_wc_ucode_dma_buf2 != NULL) {
            soc_cm_sfree(unit, soc->soc_wc_ucode_dma_buf2);
            soc->soc_wc_ucode_dma_buf2 = NULL;
        }

        if (soc->mem_ser_info_lock) {
            sal_mutex_destroy(soc->mem_ser_info_lock);
            soc->mem_ser_info_lock = NULL;
        }

        /* Destroy UDF module protection mutex. */
        if (NULL != soc->udf_lock) {
            sal_mutex_destroy(soc->udf_lock);
            soc->udf_lock = NULL;
        }

        sal_free(SOC_PERSIST(unit));
        SOC_PERSIST(unit) = NULL;

#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx) &&
                        !((SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM))) {
            SOC_IF_ERROR_RETURN(soc_schan_fifo_deinit(unit));
        }
#endif

        sal_free(SOC_CONTROL(unit));
        SOC_CONTROL(unit) = NULL;

        if (--soc_ndev_attached == 0) {
            /* Work done after the last SOC device is detached. */
            /* (currently nothing) */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_shutdown: all units detached\n")));
        }
    }


#ifdef INCLUDE_KNET
    if (shutdown_knet) {
    soc_knet_cleanup();
    }
#endif

    /* Free up the Level 2 warm boot cache */
    soc_scache_detach(unit);

    return SOC_E_NONE;
}

STATIC int
_soc_mem_scrub_read (int unit, soc_mem_t mem, int blk, int index_min, int index_max, void *entry_buffer)
{
    int     ser_flags[SOC_MAX_NUM_PIPES];
    soc_acc_type_t mem_acc_type;
    int pipe_idx = 0;
    int num_pipe = 1;
    int ser_error = 0;
    uint32 ser_err[2];
    soc_timeout_t  to;
    int timeout_usec;
    sal_memset(&ser_flags, 0, sizeof(ser_flags));

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_SHADOW_SUPPORT)
    if ((SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_CAM) &&
        (!(SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_EXT_CAM)) &&
         soc_feature(unit, soc_feature_xy_tcam_direct)) {
            for (pipe_idx = 0; pipe_idx < NUM_PIPE(unit); pipe_idx++) {
                ser_flags[pipe_idx] |= _SOC_SER_FLAG_XY_READ;
        }
    }
#endif

    if (NUM_PIPE(unit) == 2) {
        if (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_IPIPE ||
            SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_EPIPE) {
            mem_acc_type = SOC_MEM_ACC_TYPE(unit, mem);
            switch (mem_acc_type) {
            case _SOC_ACC_TYPE_PIPE_Y:
                num_pipe = 1;
                ser_flags[0] |= (_SOC_SER_FLAG_MULTI_PIPE |
                               _SOC_MEM_ADDR_ACC_TYPE_PIPE_Y);
                if (!soc_mem_dmaable(unit, mem, blk)) {
                    ser_flags[0] |= _SOC_SER_FLAG_NO_DMA;
                }
                break;
            case _SOC_ACC_TYPE_PIPE_X:
                num_pipe = 1;
                ser_flags[0] |= (_SOC_SER_FLAG_MULTI_PIPE |
                               _SOC_MEM_ADDR_ACC_TYPE_PIPE_X);
                if (!soc_mem_dmaable(unit, mem, blk)) {
                    ser_flags[0] |= _SOC_SER_FLAG_NO_DMA;
                }
                break;
            default:
                num_pipe = 2;
                /* For TCAMs, SER engine has not set SOC_MEM_ADDR_ACC_TYPE_PIPE_X
                 * in pipe_x. If read table in specified pipe_x, it will not detect
                 * parity error. So remove the flag to match the SER engine setting.
                 */
                if ((SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_CAM) &&
                    (!(SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_EXT_CAM))) {
                    ser_flags[0] |= _SOC_SER_FLAG_MULTI_PIPE;
                } else {
                    if (SOC_IS_TRIDENT3X(unit)) {
                        ser_flags[0] |= (_SOC_SER_FLAG_MULTI_PIPE |
                                       _SOC_MEM_ADDR_ACC_TYPE_PIPE_0);
                    } else {
                    ser_flags[0] |= (_SOC_SER_FLAG_MULTI_PIPE |
                                   _SOC_MEM_ADDR_ACC_TYPE_PIPE_X);
                }
                }

                if (SOC_IS_TRIDENT3X(unit)) {
                    ser_flags[1] |= (_SOC_SER_FLAG_MULTI_PIPE |
                                   _SOC_MEM_ADDR_ACC_TYPE_PIPE_1);
                } else {
                ser_flags[1] |= (_SOC_SER_FLAG_MULTI_PIPE |
                               _SOC_MEM_ADDR_ACC_TYPE_PIPE_Y);
                }
                if (!soc_mem_dmaable(unit, mem, blk)) {
                    ser_flags[0] |= _SOC_SER_FLAG_NO_DMA;
                    ser_flags[1] |= _SOC_SER_FLAG_NO_DMA;
                }
                break;
            }
        } else {
            num_pipe = 1;
            ser_flags[0] = 0;
            if (!soc_mem_dmaable(unit, mem, blk)) {
                ser_flags[0] |= _SOC_SER_FLAG_NO_DMA;
            }
        }
#if defined(BCM_TRIDENT_SUPPORT)
        if ((SOC_IS_TRIDENT(unit) || SOC_IS_TITAN(unit)) &&
            (mem == EGR_VLANm)) {
            num_pipe = 1;
            ser_flags[0] = _SOC_SER_FLAG_NO_DMA;
        }
#endif
    } else {
        num_pipe = 1;
        if (SOC_IS_APACHE(unit) && (mem == EGR_VLANm)) {
            ser_flags[0] = _SOC_SER_FLAG_NO_DMA;
        } else {
            ser_flags[0] = 0;
        }
        if (!soc_mem_dmaable(unit, mem, blk)) {
            ser_flags[0] |= _SOC_SER_FLAG_NO_DMA;
        }
    }
    if (soc_feature(unit, soc_feature_ser_scrub_delay)){
        timeout_usec = 1000000;
        soc_timeout_init(&to, timeout_usec, 0);
         do  {
              SOC_IF_ERROR_RETURN(READ_SER_ERROR_0r(unit, &ser_err[0]));
              SOC_IF_ERROR_RETURN(READ_SER_ERROR_1r(unit, &ser_err[1]));
              if (!soc_reg_field_get(unit, SER_ERROR_0r, ser_err[0], ERROR_0_VALf) ||
                  !soc_reg_field_get(unit, SER_ERROR_1r, ser_err[1], ERROR_1_VALf)) {
                  ser_error = 0;
              } else {
                  ser_error = 1;
                  sal_usleep(1);
              }
              if (soc_timeout_check(&to)) {
                  return SOC_E_TIMEOUT;
                  break;
              }
        }while (ser_error);
    }
    for (pipe_idx = 0; pipe_idx < num_pipe; pipe_idx++) {
        SOC_IF_ERROR_RETURN(soc_mem_ser_read_range(unit, mem, blk,
                                                    index_min, index_max,
                                                    ser_flags[pipe_idx],
                                                    entry_buffer));
    }

    return SOC_E_NONE;
}



int
soc_system_scrub(int unit)
{
    int mem, blk, copyno = COPYNO_ALL;
    uint32 *table = NULL;
    soc_mem_t act_mem;
    int c = 0, entry_dw, table_size, tmax = 0;
    int index_cnt, index_min, index_max;
#ifdef BCM_ESW_SUPPORT
    int skip = 0;
#endif

    if (soc_feature(unit, soc_feature_ser_system_scrub)) {
        /* do soc_system_scrub */
    } else if (!(SOC_IS_TD_TT(unit) || SOC_IS_ENDURO(unit) ||
                 SOC_IS_SABER2(unit) || SOC_IS_KATANAX(unit) ||
                 SOC_IS_TRIUMPH3(unit)) ||
        soc_property_get(unit, "skip_system_scrub", 0)) {
        return SOC_E_NONE;
    }

    if (soc_feature(unit, soc_feature_tcam_scan_engine)) {
        (void) soc_ser_tcam_scan_engine_enable(unit, TRUE);
    }

    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        act_mem = mem;

        if (!SOC_MEM_IS_VALID(unit, mem)) {
            continue;
        }
        if (soc_mem_index_count(unit, mem) == 0) {
            continue;
        }
        if (SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_SER_PARITY_ENABLE_SKIP) {
            LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                "scrub: skipping mem %s (parity is disabled)\n"),
                SOC_MEM_NAME(unit, mem)));
            continue;
        }
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit) &&
            soc_th3_ser_test_skip_by_at(unit, mem, SOC_MEM_ACC_TYPE(unit, mem))) {
            continue;
        }
#endif
#ifdef BCM_ESW_SUPPORT
        _SOC_DRV_MEM_REUSE_MEM_STATE_NO_LOCK(unit, act_mem);
#endif
        if (act_mem != mem) {
            LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "scrub: skipping mem %s, act_mem %s (act_mem != mem)\n"), SOC_MEM_NAME(unit, mem), SOC_MEM_NAME(unit, act_mem)));
            continue;
        }
        if (SOC_MEM_SER_CORRECTION_TYPE(unit, mem) == 0) {
            LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "scrub: skipping mem %s (ERR_CORRECTION = 0)\n"), SOC_MEM_NAME(unit, mem)));
            continue;
        }
#ifdef BCM_SBUSDMA_SUPPORT
        if (soc_feature(unit, soc_feature_sbusdma)) {
            if (mem == ING_SER_FIFOm || mem == EGR_SER_FIFOm ||
                mem == ISM_SER_FIFOm) {
                LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "scrub: skipping mem %s (SER_FIFOm\n"), SOC_MEM_NAME(unit, mem)));
                continue;
            }
        }
#endif /* BCM_SBUSDMA_SUPPORT */
        SOC_MEM_BLOCK_ITER(unit, mem, blk) {
            copyno = blk;
            break;
        }
        if ((SOC_BLOCK_TYPE(unit, copyno) == SOC_BLK_ETU) ||
            (SOC_BLOCK_TYPE(unit, copyno) == SOC_BLK_ESM)) {
            continue;
        }
        /* Skip IESMIF mems when ESM is not present */
        else if (SOC_BLOCK_TYPE(unit, copyno) == SOC_BLK_IPIPE) {
#ifdef BCM_TRIUMPH3_SUPPORT
            if (SOC_IS_TRIUMPH3(unit) && /* Needs update per chip */
                SOC_MEM_ADDR_STAGE_EXTENDED(SOC_MEM_BASE(unit, mem)) == 0x9) {
                if (!soc_feature(unit, soc_feature_esm_support) ||
                     SAL_BOOT_QUICKTURN) {
                    continue;
                }
            }
#endif /* BCM_TRIUMPH3_SUPPORT */
        }
#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit)) {
            /* Skip unused views */
            if (mem == ESM_PKT_TYPE_ID_ONLYm || mem == ESM_PKT_TYPE_ID_DATA_ONLYm) {
                continue;
            }
            if (mem == MODPORT_MAP_M0m || mem == MODPORT_MAP_M1m ||
                mem == MODPORT_MAP_M2m || mem == MODPORT_MAP_M3m) {
                continue;
            }
        }
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
        if (SOC_IS_TD_TT(unit) || SOC_IS_TD2_TT2(unit)) {
            int i, skip;
            static soc_mem_t mskip[] = {
                L3_ENTRY_IPV4_MULTICAST_Xm,
                L3_ENTRY_IPV4_MULTICAST_Ym,
                L3_ENTRY_IPV6_UNICAST_Xm,
                L3_ENTRY_IPV6_UNICAST_Ym
            };
            skip = 0;
            for (i=0; i<COUNTOF(mskip); i++) {
                if (mem == mskip[i]) {
                    skip = 1;
                    break;
                }
            }
            if (skip) {
                LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "scrub: skipping mem %s (part of mskip list)\n"), SOC_MEM_NAME(unit, mem)));
                continue;
            }
                        /* Skip unused views */
            if (mem == ING_SNAT_ONLYm || mem == ING_SNAT_DATA_ONLYm ||
                mem == FP_GLOBAL_MASK_TCAMm ||
                /* these are NOT unused views and can have parity/ecc errors */
                /* mem == L2_ENTRY_LPm || mem == L3_ENTRY_LPm || */
                /* mem == VLAN_XLATE_LPm || mem == EGR_VLAN_XLATE_LPm || */
                mem == VLAN_SUBNET_DATA_ONLYm || mem == L3_DEFIP_ALPM_RAWm) {
                LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "scrub: skipping mem %s (unused views)\n"), SOC_MEM_NAME(unit, mem)));
                continue;
            }
        }
#if defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_TD2_TT2(unit) && !SOC_IS_APACHE(unit)) {
            if (mem == L3_DEFIP_ALPM_IPV4m || mem == L3_DEFIP_ALPM_IPV4_1m ||
                mem == L3_DEFIP_ALPM_IPV6_64m || mem == L3_DEFIP_ALPM_IPV6_64_1m ||
                mem == L3_DEFIP_ALPM_IPV6_128m) {
                /* We call a separate traverse routine to only access the
                   memory with the right view */
                LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "scrub: skipping mem %s (will call separate traverse routine)\n"), SOC_MEM_NAME(unit, mem)));
                continue;
            }
        }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        /* we can't scrub those memory, they can lead to unnecessary SER corrections */
        if (SOC_IS_TD2P_TT2P(unit) || SOC_IS_APACHE(unit)) {
            if (mem == REPLICATION_FIFO_BANK0m ||
                mem == REPLICATION_FIFO_BANK1m ||
                mem == MMU_ENQ_PBI_DBm) {
                LOG_VERBOSE(BSL_LS_SOC_SER,
                           (BSL_META_U(unit, "scrub: skipping mem %s (will call separate traverse routine)\n"),
                                       SOC_MEM_NAME(unit, mem)));
                continue;
            }
        }
        /*
        * We skip EFP_TCAM table since parity error happens on this table on TD2+
        * is detected by memscan thread now.
        */
        if (SOC_IS_TD2P_TT2P(unit)) {
            if (mem == EFP_TCAMm) {
                LOG_VERBOSE(BSL_LS_SOC_SER,
                           (BSL_META_U(unit, "scrub: skipping mem %s (memscan detect error)\n"),
                                       SOC_MEM_NAME(unit, mem)));
                continue;
            }
        }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#if defined(BCM_APACHE_SUPPORT)
        /* we can't scrub those memory, they can lead to unnecessary SER corrections */
        if (SOC_IS_APACHE(unit)) {
            if((mem == MMU_MAPPER_X_LSBm) || (mem == MMU_MAPPER_Y_LSBm)) {
                LOG_VERBOSE(BSL_LS_SOC_SER,
                           (BSL_META_U(unit, "scrub: skipping mem %s (will call separate traverse routine)\n"),
                                       SOC_MEM_NAME(unit, mem)));
                continue;
            }
        }
#endif /* APACHE_SUPPORT*/
#endif
#endif /* BCM_TRIDENT_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
        if (SOC_IS_TRX(unit)) {
            /* Skip unused views */
            if (mem == MY_STATION_TCAM_DATA_ONLYm || mem == L2_USER_ENTRY_DATA_ONLYm ||
                mem == L3_DEFIP_DATA_ONLYm || mem == L3_DEFIP_PAIR_128_DATA_ONLYm ||
                mem == L3_DEFIP_128_DATA_ONLYm || mem == L3_DEFIP_128_ONLYm ||
                mem == L3_DEFIP_ONLYm || mem == L3_DEFIP_PAIR_128_ONLYm ||
                mem == L3_ENTRY_ONLYm) {
                LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "scrub: skipping mem %s (unused TRX views)\n"), SOC_MEM_NAME(unit, mem)));
                continue;
            }
        }
#endif /* BCM_TRX_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
        if (SOC_IS_KATANAX(unit)) {
            if(mem == XPORT_WC_UCMEM_DATAm) {
                continue;
            }
        }
#endif /* BCM_KATANA_SUPPORT */
#if defined (BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit)) {
            if (mem == RXLP_DEBUG_COUNTER0m || mem == RXLP_DEBUG_COUNTER1m ||
                mem == RXLP_DEBUG_COUNTER2m || mem == RXLP_DEBUG_COUNTER3m ||
                mem == RXLP_DEBUG_COUNTER4m || mem == RXLP_DEBUG_COUNTER5m ||
                mem == RXLP_DEBUG_COUNTER6m || mem == RXLP_DEBUG_COUNTER7m ||
                mem == RXLP_DEBUG_COUNTER8m || mem == TXLP_DEBUG_COUNTER0m ||
                mem == TXLP_DEBUG_COUNTER1m || mem == TXLP_DEBUG_COUNTER2m ||
                mem == TXLP_DEBUG_COUNTER3m || mem == TXLP_DEBUG_COUNTER4m ||
                mem == TXLP_DEBUG_COUNTER5m || mem == TXLP_DEBUG_COUNTER6m ||
                mem == TXLP_DEBUG_COUNTER7m || mem == TXLP_DEBUG_COUNTER8m )  {
                continue;
            }
        }
#endif

#if defined(BCM_TOMAHAWK_SUPPORT)
        if (SOC_IS_TOMAHAWKX(unit)) {
            if (soc_th_check_scrub_skip(unit, mem, 1)) {
                continue;
            }
        }
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            if (soc_td3_check_scrub_skip(unit, mem, 1)) {
                continue;
            }
        }
#endif /* BCM_TRIDENT3_SUPPORT */
#if defined(BCM_GREYHOUND_SUPPORT)
        if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit)) {
            if (mem == VLAN_SUBNET_DATA_ONLYm ||
                mem == MODPORT_MAP_IMm || mem == MODPORT_MAP_EMm ||
                mem == CPU_COS_MAP_ONLYm || mem == L2_ENTRY_ONLYm ||
                mem == L2_USER_ENTRY_ONLYm || mem == VLAN_SUBNET_ONLYm )  {
                continue;
            }
        }
#endif /* BCM_GREYHOUND_SUPPORT */

#ifdef BCM_ESW_SUPPORT
        _SOC_MEM_SKIP_SCRUB(unit, mem, skip);
        if (skip == 1) {
            continue;
        }
#endif

        entry_dw = soc_mem_entry_words(unit, mem);
        index_cnt = soc_mem_index_count(unit, mem);
        table_size = index_cnt * entry_dw * 4;
        index_max = index_cnt - 1;
        index_min = soc_mem_index_min(unit, mem);
        if (table == NULL) {
            table = soc_cm_salloc(unit, table_size, "scrub");
            if (table == NULL) {
                return SOC_E_MEMORY;
            }
            tmax = table_size;
        } else {
            if (table_size > tmax) {
                soc_cm_sfree(unit, table);
                table = soc_cm_salloc(unit, table_size, "scrub");
                if (table == NULL) {
                    return SOC_E_MEMORY;
                }
                tmax = table_size;
            }
        }
        SOC_MEM_BLOCK_ITER(unit, mem, blk) {
#if defined(BCM_TRIUMPH3_SUPPORT)
            if (SOC_IS_TRIUMPH3(unit)) {
                if (mem == PORT_EHG_RX_TUNNEL_DATAm ||
                    mem == PORT_EHG_RX_TUNNEL_MASKm ||
                    mem == PORT_EHG_TX_TUNNEL_DATAm) {
                    if (!_soc_tr3_port_mem_blk_enabled(unit, blk)) {
                        continue;
                    }
                }
            }
#endif /* BCM_TRIUMPH3_SUPPORT */

#if defined(BCM_TOMAHAWK_SUPPORT)
            if (SOC_IS_TOMAHAWKX(unit)) {
                int i, num_inst_to_scrub = 0, acc_type_list[8], rv;
                uint32 ser_flags;
                if (SOC_TH_MEM_CHK_IFP_TCAM(mem) ||
                    SOC_TH_MEM_CHK_IFP_TCAM_WIDE(mem)) {
                    if (soc_th_ifp_tcam_dma_read(unit, mem, blk, table,
                                                 (mem == IFP_TCAMm ||
                                                  mem == IFP_TCAM_WIDEm),
                                                  NULL
                                                ) >= 0) {
                        /* Do nothing - with data */
                        c++;
                        LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "Scrub: %s\n"), SOC_MEM_NAME(unit, mem)));
                    }
                    continue; /* skip subsequent soc_mem_read_range irrespecive of pass, fail */
                }
#ifdef BCM_TOMAHAWK3_SUPPORT
                if (SOC_IS_TOMAHAWK3(unit)) {
                    int skip_mem;
                    /* Check if the memory needs to be skipped due to the
                     * port macro it's associated with being down */
                    rv = soc_th3_skip_obm_mem_if_pm_down(unit, mem, &skip_mem);
                    if (SOC_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_SOC_SER, (BSL_META_U(unit, "PM Down check error: %s\n"), SOC_MEM_NAME(unit, mem)));
                        continue;
                    }

                    /* skip reading this memory because we can't read it if
                     * the PM is down */
                    if (skip_mem) {
                        continue;
                    }

                    rv = soc_th3_check_scrub_info(unit, mem, blk, copyno,
                                                 &num_inst_to_scrub, acc_type_list);
                } else
#endif
                {
                rv = soc_th_check_scrub_info(unit, mem, blk, copyno,
                                             &num_inst_to_scrub, acc_type_list);
                }
                if (SOC_FAILURE(rv)) {
                    continue; /* skip subsequent soc_mem_read_range */
                }
                assert(num_inst_to_scrub <= 8);
                for (i = 0; i < num_inst_to_scrub; i++) {
#ifdef BCM_TOMAHAWK3_SUPPORT
                    if (SOC_IS_TOMAHAWK3(unit) &&
                        soc_th3_ser_test_skip_by_at(unit, mem, acc_type_list[i])) {
                        continue;
                    }
#endif
                    ser_flags = 0;
                    ser_flags |= _SOC_SER_FLAG_DISCARD_READ;
                    ser_flags |= _SOC_SER_FLAG_MULTI_PIPE;
                    ser_flags |= acc_type_list[i];
                    if (!soc_mem_dmaable(unit, mem, blk)) {
                        ser_flags |= _SOC_SER_FLAG_NO_DMA;
                    }
                    if (soc_mem_ser_read_range(unit, mem, blk, index_min,
                                               index_max, ser_flags,
                                               table) >= 0) {
                        c++;
                        LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "Scrub: %s (instance %d)\n"), SOC_MEM_NAME(unit, mem), acc_type_list[i]));
                    }
                }
                if (num_inst_to_scrub) {
                    continue; /* skip subsequent soc_mem_read_range
                                 irrespecive of pass, fail */
                }

                /* In some cases, soc_th_check_scrub_info() sets num_inst_to_scrub with 0,
                 * soc_system_scrub() will not read the memories. For unique memories in multi-pipes,
                 * soc_mem_read_range() will read them in new views with suffix '_PIPEx'.  */
                if (soc_mem_read_range(unit, mem, blk, index_min,
                                              index_max, table) >= 0) {
                      LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "Scrub read mem:%s\n"), SOC_MEM_NAME(unit, mem)));
                }
            } else
#endif /* BCM_TOMAHAWK_SUPPORT */
            {
                if ( _soc_mem_scrub_read(unit, mem, blk, index_min, index_max, table) >= 0) {
                    /* Do nothing */
                    c++;
#if !defined(SOC_NO_NAMES)
#if defined(BCM_ESW_SUPPORT) || \
    defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
                LOG_INFO(BSL_LS_SOC_SER,
                         (BSL_META_U(unit,
                                     "Scrub: %s\n"), SOC_MEM_NAME(unit, mem)));
#endif
#else
                LOG_INFO(BSL_LS_SOC_SOCMEM,
                         (BSL_META_U(unit,
                                     "Scrub: %d\n"), mem));
#endif
                }
            }
        }
    }
    if (table) {
        soc_cm_sfree(unit, table);
        LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "Mem scrub count: %d tmax: %d\n"), c, tmax));
    }
#ifdef ALPM_ENABLE
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit) && !SOC_IS_APACHE(unit)) {
        SOC_IF_ERROR_RETURN(soc_trident2_alpm_scrub(unit));
    }
#endif /* BCM_TRIDENT2_SUPPORT */
#endif /* ALPM_ENABLE */
    return soc_ser_reg_load_scrub(unit, 0);
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef  BCM_HERCULES15_SUPPORT
/*
 * The 5675A0 has some latches in a test path that runs between all
 * of the XAUI macs.  To ensure that the test path is initialized
 * correctly, all macs must be enabled, and a low frequency reference
 * clock must be given to unused ports.
 */
STATIC void
soc_reset_bcm5675_a0(int unit, uint16 dev_id)
{
    uint64      xctrl;
    int         spl;

    COMPILER_REFERENCE(dev_id);

    /*
     * Enable core_clk to all macs.
     * This does not take effect until the next soft reset.
     */
    WRITE_CMIC_CLK_ENABLEr(unit, 0x1ff);

    /* Block interrupts while resetting */
    spl = sal_splhi();
    SOC_CONTROL(unit)->soc_flags |= SOC_F_BUSY;
    soc_pci_write(unit, CMIC_CONFIG,
                  soc_pci_read(unit, CMIC_CONFIG) | CC_RESET_CPS);
    /* Unblock interrupts */
    sal_spl(spl);

    if (SAL_BOOT_QUICKTURN) {
        sal_usleep(10 * MILLISECOND_USEC);
    } else {
        sal_usleep(1 * MILLISECOND_USEC);
    }

    soc_endian_config(unit);    /* reset cleared endian settings */
    /* Block interrupts */
    spl = sal_splhi();
    SOC_CONTROL(unit)->soc_flags &= ~SOC_F_BUSY;
    /* Unblock interrupts */
    sal_spl(spl);

    /*
     * Force all ports (even disabled ones) to run for a little
     * while by clearing iddq and pwrdwn.  The definition of little
     * while depends on the refclock given to external ports, which
     * is assumed here to be at least 100KHz or so
     */
    /* defaults except for iddq=0 pwrdwn=0 */
    COMPILER_64_SET(xctrl, 0, 0x181);
    soc_reg64_write(unit, 0x500201, xctrl);     /* MAC_XGXS_CTRLr */
    soc_reg64_write(unit, 0x600201, xctrl);
    soc_reg64_write(unit, 0x700201, xctrl);
    soc_reg64_write(unit, 0x800201, xctrl);
    soc_reg64_write(unit, 0x100201, xctrl);
    soc_reg64_write(unit, 0x200201, xctrl);
    soc_reg64_write(unit, 0x300201, xctrl);
    soc_reg64_write(unit, 0x400201, xctrl);

    sal_usleep(60);

    /* back to reset defaults: iddq=1 pwrdwn=1 */
    COMPILER_64_SET(xctrl, 0, 0x1e1);
    soc_reg64_write(unit, 0x500201, xctrl);
    soc_reg64_write(unit, 0x600201, xctrl);
    soc_reg64_write(unit, 0x700201, xctrl);
    soc_reg64_write(unit, 0x800201, xctrl);
    soc_reg64_write(unit, 0x100201, xctrl);
    soc_reg64_write(unit, 0x200201, xctrl);
    soc_reg64_write(unit, 0x300201, xctrl);
    soc_reg64_write(unit, 0x400201, xctrl);
}
#endif  /* BCM_HERCULES15_SUPPORT */

#ifdef BCM_XGS3_SWITCH_SUPPORT
/*
 * Reset VCO sequencer.
 */
void
soc_xgxs_lcpll_reset(int unit)
{
    if (soc_feature(unit, soc_feature_xgxs_lcpll)) {
        uint32 val;
        uint16 dev_id;
        uint8 rev_id;

        soc_cm_get_id(unit, &dev_id, &rev_id);

        READ_CMIC_XGXS_PLL_CONTROL_1r(unit, &val);
#if defined(BCM_RAPTOR_SUPPORT)
        if (soc_reg_field_valid
                (unit, CMIC_XGXS_PLL_CONTROL_1r, SEL_DIFF_CLOCKf)) {
            uint32 xtal_sel, default_val = 0;
            default_val = (SOC_IS_RAVEN(unit) &&
                          (rev_id == BCM56224_B0_REV_ID)) ? 1 : 0;
            if (soc_property_get(unit,
                         spn_XGXS_LCPLL_XTAL_REFCLK, default_val) == 0) {
                xtal_sel = 1;
            } else {
                xtal_sel = 0;
            }
            soc_reg_field_set
                (unit, CMIC_XGXS_PLL_CONTROL_1r,
                 &val, SEL_DIFF_CLOCKf, xtal_sel);
        }
#endif /* BCM_RAPTOR_SUPPORT */

        soc_reg_field_set(unit, CMIC_XGXS_PLL_CONTROL_1r, &val, RESETf, 1);
        WRITE_CMIC_XGXS_PLL_CONTROL_1r(unit, val);
        soc_reg_field_set(unit, CMIC_XGXS_PLL_CONTROL_1r, &val, RESETf, 0);
        sal_usleep(50);
        WRITE_CMIC_XGXS_PLL_CONTROL_1r(unit, val);
        sal_usleep(50);
    }
}

/*
 * Check for PLL lock for Bradley
 */
void
_bradley_lcpll_lock_check(int unit)
{
    uint32 val;
    int pll_lock_usec;
    int locked = 0;
    int retry = 3;
    soc_timeout_t to;

    /* Check if first LCPLL locked */
    while (!locked && retry--) {
#if defined(BCM_HB_GW_SUPPORT)
        READ_CMIC_XGXS0_PLL_CONTROL_1r(unit, &val);
        soc_reg_field_set(unit, CMIC_XGXS0_PLL_CONTROL_1r, &val,
                          PLL_SEQSTARTf, 1);
        WRITE_CMIC_XGXS0_PLL_CONTROL_1r(unit, val);
        sal_usleep(100);

        READ_CMIC_XGXS0_PLL_CONTROL_1r(unit, &val);
        soc_reg_field_set(unit, CMIC_XGXS0_PLL_CONTROL_1r, &val,
                          PLLFORCECAPDONE_ENf, 1);
        soc_reg_field_set(unit, CMIC_XGXS0_PLL_CONTROL_1r, &val,
                          PLLFORCECAPDONEf, 1);
        soc_reg_field_set(unit, CMIC_XGXS0_PLL_CONTROL_1r, &val,
                          PLLFORCECAPPASS_ENf, 1);
        soc_reg_field_set(unit, CMIC_XGXS0_PLL_CONTROL_1r, &val,
                          PLLFORCECAPPASSf, 1);
        WRITE_CMIC_XGXS0_PLL_CONTROL_1r(unit, val);
        sal_usleep(100);

        READ_CMIC_XGXS0_PLL_CONTROL_1r(unit, &val);
        soc_reg_field_set(unit, CMIC_XGXS0_PLL_CONTROL_1r, &val,
                          PLL_SEQSTARTf, 0);
        WRITE_CMIC_XGXS0_PLL_CONTROL_1r(unit, val);
        sal_usleep(50);
#endif
        pll_lock_usec = 500000;
        soc_timeout_init(&to, pll_lock_usec, 0);
        while (!soc_timeout_check(&to)) {
            READ_CMIC_XGXS0_PLL_CONTROL_2r(unit, &val);
            locked = soc_reg_field_get(unit, CMIC_XGXS0_PLL_CONTROL_2r,
                                       val, CMIC_XGPLL_LOCKf);
            if (locked) {
                break;
            }
        }
    }

    if (!locked) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "bradley_lcpll_lock_check: LCPLL0 not locked on unit %d "
                              "status = 0x%08x\n"),
                   unit, val));
    }

    /* Now check for second LCPLL */
    locked = 0;
    retry = 3;
    while (!locked && retry--) {
#if defined(BCM_HB_GW_SUPPORT)
        READ_CMIC_XGXS1_PLL_CONTROL_1r(unit, &val);
        soc_reg_field_set(unit, CMIC_XGXS1_PLL_CONTROL_1r, &val,
                          PLL_SEQSTARTf, 1);
        WRITE_CMIC_XGXS1_PLL_CONTROL_1r(unit, val);
        sal_usleep(100);

        READ_CMIC_XGXS1_PLL_CONTROL_1r(unit, &val);
        soc_reg_field_set(unit, CMIC_XGXS1_PLL_CONTROL_1r, &val,
                          PLLFORCECAPDONE_ENf, 1);
        soc_reg_field_set(unit, CMIC_XGXS1_PLL_CONTROL_1r, &val,
                          PLLFORCECAPDONEf, 1);
        soc_reg_field_set(unit, CMIC_XGXS1_PLL_CONTROL_1r, &val,
                          PLLFORCECAPPASS_ENf, 1);
        soc_reg_field_set(unit, CMIC_XGXS1_PLL_CONTROL_1r, &val,
                          PLLFORCECAPPASSf, 1);
        WRITE_CMIC_XGXS1_PLL_CONTROL_1r(unit, val);
        sal_usleep(100);

        READ_CMIC_XGXS1_PLL_CONTROL_1r(unit, &val);
        soc_reg_field_set(unit, CMIC_XGXS1_PLL_CONTROL_1r, &val,
                          PLL_SEQSTARTf, 0);
        WRITE_CMIC_XGXS1_PLL_CONTROL_1r(unit, val);
        sal_usleep(50);
#endif
        pll_lock_usec = 500000;
        soc_timeout_init(&to, pll_lock_usec, 0);
        while (!soc_timeout_check(&to)) {
            READ_CMIC_XGXS1_PLL_CONTROL_2r(unit, &val);
            locked = soc_reg_field_get(unit, CMIC_XGXS1_PLL_CONTROL_2r,
                                            val, CMIC_XGPLL_LOCKf);
            if (locked) {
                break;
            }
        }
    }

    if (!locked) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "bradley_lcpll_lock_check: LCPLL1 not locked on unit %d "
                              "status = 0x%08x\n"),
                   unit, val));
    }

}

#endif /* BCM_XGS3_SWITCH_SUPPORT */

#ifdef BCM_BIGMAC_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
static const struct {
    soc_reg_t mdio_conf_reg;
    int hc_chan;
} tr_xgxs_map[] = {
    {                 INVALIDr, -1, }, /* port  0 */
    {                 INVALIDr, -1, }, /* port  1 */
    { CMIC_XGXS_MDIO_CONFIG_0r,  0, }, /* port  2 */
    { CMIC_XGXS_MDIO_CONFIG_0r,  1, }, /* port  3 */
    { CMIC_XGXS_MDIO_CONFIG_0r,  2, }, /* port  4 */
    { CMIC_XGXS_MDIO_CONFIG_0r,  3, }, /* port  5 */
    { CMIC_XGXS_MDIO_CONFIG_1r,  0, }, /* port  6 */
    { CMIC_XGXS_MDIO_CONFIG_1r,  1, }, /* port  7 */
    {                 INVALIDr, -1, }, /* port  8 */
    {                 INVALIDr, -1, }, /* port  9 */
    {                 INVALIDr, -1, }, /* port 10 */
    {                 INVALIDr, -1, }, /* port 11 */
    {                 INVALIDr, -1, }, /* port 12 */
    {                 INVALIDr, -1, }, /* port 13 */
    { CMIC_XGXS_MDIO_CONFIG_2r,  0, }, /* port 14 */
    { CMIC_XGXS_MDIO_CONFIG_2r,  1, }, /* port 15 */
    { CMIC_XGXS_MDIO_CONFIG_2r,  2, }, /* port 16 */
    { CMIC_XGXS_MDIO_CONFIG_2r,  3, }, /* port 17 */
    { CMIC_XGXS_MDIO_CONFIG_1r,  2, }, /* port 18 */
    { CMIC_XGXS_MDIO_CONFIG_1r,  3, }, /* port 19 */
    {                 INVALIDr, -1, }, /* port 20 */
    {                 INVALIDr, -1, }, /* port 21 */
    {                 INVALIDr, -1, }, /* port 22 */
    {                 INVALIDr, -1, }, /* port 23 */
    {                 INVALIDr, -1, }, /* port 24 */
    {                 INVALIDr, -1, }, /* port 25 */
    { CMIC_XGXS_MDIO_CONFIG_3r,  0, }, /* port 26 */
    { CMIC_XGXS_MDIO_CONFIG_5r,  0, }, /* port 27 */
    { CMIC_XGXS_MDIO_CONFIG_6r, -1, }, /* port 28 */
    { CMIC_XGXS_MDIO_CONFIG_7r, -1, }, /* port 29 */
    { CMIC_XGXS_MDIO_CONFIG_8r, -1, }, /* port 30 */
    { CMIC_XGXS_MDIO_CONFIG_9r, -1, }, /* port 31 */
    { CMIC_XGXS_MDIO_CONFIG_3r,  1, }, /* port 32 */
    { CMIC_XGXS_MDIO_CONFIG_3r,  2, }, /* port 33 */
    { CMIC_XGXS_MDIO_CONFIG_3r,  3, }, /* port 34 */
    { CMIC_XGXS_MDIO_CONFIG_4r,  0, }, /* port 35 */
    { CMIC_XGXS_MDIO_CONFIG_4r,  1, }, /* port 36 */
    {                 INVALIDr, -1, }, /* port 37 */
    {                 INVALIDr, -1, }, /* port 38 */
    {                 INVALIDr, -1, }, /* port 39 */
    {                 INVALIDr, -1, }, /* port 40 */
    {                 INVALIDr, -1, }, /* port 41 */
    {                 INVALIDr, -1, }, /* port 42 */
    { CMIC_XGXS_MDIO_CONFIG_5r,  1, }, /* port 43 */
    { CMIC_XGXS_MDIO_CONFIG_5r,  2, }, /* port 44 */
    { CMIC_XGXS_MDIO_CONFIG_5r,  3, }, /* port 45 */
    { CMIC_XGXS_MDIO_CONFIG_4r,  2, }, /* port 46 */
    { CMIC_XGXS_MDIO_CONFIG_4r,  3, }, /* port 47 */
    {                 INVALIDr, -1, }, /* port 48 */
    {                 INVALIDr, -1, }, /* port 49 */
    {                 INVALIDr, -1, }, /* port 50 */
    {                 INVALIDr, -1, }, /* port 51 */
    {                 INVALIDr, -1, }, /* port 52 */
    {                 INVALIDr, -1, }, /* port 53 */
};
#endif /* BCM_TRIUMPH_SUPPORT */

STATIC int
_soc_xgxs_mdio_setup(int unit, int port)
{
    soc_reg_t reg = INVALIDr;
    uint32 rval;
    uint32 devad = 0;
    int skip = FALSE;

    if (!SOC_PORT_VALID(unit, port))
    {
        return SOC_E_PORT;
    }

    if (SOC_IS_FB_FX_HX(unit)) {
        switch(port) {
        case 24: reg = CMIC_XGXS_MDIO_CONFIG_0r; break;
        case 25: reg = CMIC_XGXS_MDIO_CONFIG_1r; break;
        case 26: reg = CMIC_XGXS_MDIO_CONFIG_2r; break;
        case 27: reg = CMIC_XGXS_MDIO_CONFIG_3r; break;
        default: return SOC_E_PARAM;
        }
    }

#ifdef BCM_GXPORT_SUPPORT
    if (SOC_IS_HB_GW(unit)) {
        switch(port) {
        case  0: reg = CMIC_XGXS_MDIO_CONFIG_0r; break;
        case  1: reg = CMIC_XGXS_MDIO_CONFIG_1r; break;
        case  2: reg = CMIC_XGXS_MDIO_CONFIG_2r; break;
        case  3: reg = CMIC_XGXS_MDIO_CONFIG_3r; break;
        case  4: reg = CMIC_XGXS_MDIO_CONFIG_4r; break;
        case  5: reg = CMIC_XGXS_MDIO_CONFIG_5r; break;
        case  6: reg = CMIC_XGXS_MDIO_CONFIG_6r; break;
        case  7: reg = CMIC_XGXS_MDIO_CONFIG_7r; break;
        case  8: reg = CMIC_XGXS_MDIO_CONFIG_8r; break;
        case  9: reg = CMIC_XGXS_MDIO_CONFIG_9r; break;
        case 10: reg = CMIC_XGXS_MDIO_CONFIG_10r; break;
        case 11: reg = CMIC_XGXS_MDIO_CONFIG_11r; break;
        case 12: reg = CMIC_XGXS_MDIO_CONFIG_12r; break;
        case 13: reg = CMIC_XGXS_MDIO_CONFIG_13r; break;
        case 14: reg = CMIC_XGXS_MDIO_CONFIG_14r; break;
        case 15: reg = CMIC_XGXS_MDIO_CONFIG_15r; break;
        case 16: reg = CMIC_XGXS_MDIO_CONFIG_16r; break;
        case 17: reg = CMIC_XGXS_MDIO_CONFIG_17r; break;
        case 18: reg = CMIC_XGXS_MDIO_CONFIG_18r; break;
        case 19: reg = CMIC_XGXS_MDIO_CONFIG_19r; break;
        default: return SOC_E_PARAM;
        }
    }
#endif

#ifdef BCM_SCORPION_SUPPORT
    if (SOC_IS_SC_CQ(unit)) {
        devad = 5;
        switch(port) {
        case  1: reg = CMIC_XGXS_MDIO_CONFIG_0r; break;
        case  2: reg = CMIC_XGXS_MDIO_CONFIG_1r; break;
        case  3: reg = CMIC_XGXS_MDIO_CONFIG_2r; break;
        case  4: reg = CMIC_XGXS_MDIO_CONFIG_3r; break;
        case  5: reg = CMIC_XGXS_MDIO_CONFIG_4r; break;
        case  6: reg = CMIC_XGXS_MDIO_CONFIG_5r; break;
        case  7: reg = CMIC_XGXS_MDIO_CONFIG_6r; break;
        case  8: reg = CMIC_XGXS_MDIO_CONFIG_7r; break;
        case  9: reg = CMIC_XGXS_MDIO_CONFIG_8r; break;
        case 10: reg = CMIC_XGXS_MDIO_CONFIG_9r; break;
        case 11: reg = CMIC_XGXS_MDIO_CONFIG_10r; break;
        case 12: reg = CMIC_XGXS_MDIO_CONFIG_11r; break;
        case 13: reg = CMIC_XGXS_MDIO_CONFIG_12r; break;
        case 14: reg = CMIC_XGXS_MDIO_CONFIG_13r; break;
        case 15: reg = CMIC_XGXS_MDIO_CONFIG_14r; break;
        case 16: reg = CMIC_XGXS_MDIO_CONFIG_15r; break;
        case 17: reg = CMIC_XGXS_MDIO_CONFIG_16r; break;
        case 18: reg = CMIC_XGXS_MDIO_CONFIG_17r; break;
        case 19: reg = CMIC_XGXS_MDIO_CONFIG_18r; break;
        case 20: reg = CMIC_XGXS_MDIO_CONFIG_19r; break;
        case 21: reg = CMIC_XGXS_MDIO_CONFIG_20r; break;
        case 22: reg = CMIC_XGXS_MDIO_CONFIG_21r; break;
        case 23: reg = CMIC_XGXS_MDIO_CONFIG_22r; break;
        case 24: reg = CMIC_XGXS_MDIO_CONFIG_23r; break;
        case 25:
        case 26:
        case 27:
        case 28:
            reg = CMIC_XGXS_MDIO_CONFIG_24r;
            devad = 0;
            break;
        default: return SOC_E_PARAM;
        }
    }
#endif

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TRIUMPH(unit) || SOC_IS_VALKYRIE(unit)) {
        reg = tr_xgxs_map[port].mdio_conf_reg;
        devad = 5;
        if (reg == INVALIDr) {
            return SOC_E_PARAM;
        }
    }
#endif

#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit)) {
        devad = 5;
        switch(port) {
        case 30: case 31: case 32: case 33:
            reg = CMIC_XGXS_MDIO_CONFIG_0r;
            break;
        case 34: case 35: case 36: case 37:
            reg = CMIC_XGXS_MDIO_CONFIG_1r;
            break;
        case 38: case 39: case 40: case 41:
            reg = CMIC_XGXS_MDIO_CONFIG_2r;
            break;
        case 42: case 43: case 44: case 45:
            reg = CMIC_XGXS_MDIO_CONFIG_3r;
            break;
        case 46: case 47: case 48: case 49:
            reg = CMIC_XGXS_MDIO_CONFIG_4r;
            break;
        case 50: case 51: case 52: case 53:
            reg = CMIC_XGXS_MDIO_CONFIG_5r;
            break;
        case 26: reg = CMIC_XGXS_MDIO_CONFIG_6r; break;
        case 27: reg = CMIC_XGXS_MDIO_CONFIG_7r; break;
        case 28: reg = CMIC_XGXS_MDIO_CONFIG_8r; break;
        case 29: reg = CMIC_XGXS_MDIO_CONFIG_9r; break;
        default: return SOC_E_PARAM;
        }
    }
#endif
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        /*
         * MD_DEVADf and IEEE_DEVICES_IN_PKGf are not used
         * MD_ST uses reset value (clause 22)
         */
        skip = TRUE;
    }
#endif /* BCM_TRIDENT_SUPPORT */

#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        int     blk_idx;

        blk_idx = SOC_PORT_BLOCK(unit, port);
        switch (blk_idx) {
            case 0: reg = CMIC_XGXS_MDIO_CONFIG_0r; break;
            case 1: reg = CMIC_XGXS_MDIO_CONFIG_1r; break;
            case 2: reg = CMIC_XGXS_MDIO_CONFIG_2r; break;
            case 3: reg = CMIC_XGXS_MDIO_CONFIG_3r; break;
            case 4: reg = CMIC_XGXS_MDIO_CONFIG_4r; break;
            case 5: reg = CMIC_XGXS_MDIO_CONFIG_5r; break;
            default: return SOC_E_PARAM;
        }
    }
#endif /* BCM_SHADOW_SUPPORT */

#ifdef BCM_ENDURO_SUPPORT
    if (SOC_IS_ENDURO(unit)) {
        devad = 5;
        switch(port) {
        case 26: reg = CMIC_XGXS_MDIO_CONFIG_0r; break;
        case 27: reg = CMIC_XGXS_MDIO_CONFIG_1r; break;
        case 28: reg = CMIC_XGXS_MDIO_CONFIG_2r; break;
        case 29: reg = CMIC_XGXS_MDIO_CONFIG_3r; break;
        default: return SOC_E_PARAM;
        }
    }
#endif /* BCM_ENDURO_SUPPORT */

#ifdef BCM_HURRICANE1_SUPPORT
    if (SOC_IS_HURRICANE(unit)) {
        devad = 5;
        switch(port) {
        case 26: reg = CMIC_XGXS_MDIO_CONFIG_0r; break;
        case 27: reg = CMIC_XGXS_MDIO_CONFIG_1r; break;
        case 28: reg = CMIC_XGXS_MDIO_CONFIG_2r; break;
        case 29: reg = CMIC_XGXS_MDIO_CONFIG_3r; break;
        default: return SOC_E_PARAM;
        }
    }
#endif /* BCM_HURRICANE_SUPPORT */

#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit) && !SOC_IS_SABER2(unit)) {
        devad = 5;
        switch(port) {
        case 25: reg = TOP_XGXS_MDIO_CONFIG_0r; break;
        case 26: reg = TOP_XGXS_MDIO_CONFIG_1r; break;
        case 27:
        case 32:
        case 33:
        case 34: reg = TOP_XGXS_MDIO_CONFIG_2r; break;
        case 28:
        case 29:
        case 30:
        case 31: reg = TOP_XGXS_MDIO_CONFIG_3r; break;
        default: return SOC_E_PARAM;
        }
     }
#endif /* BCM_KATANA_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        /*
         * MD_DEVADf and IEEE_DEVICES_IN_PKGf are not used
         * MD_ST uses reset value (clause 22)
         */
        skip = TRUE;
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    if (!skip) {
        if (reg == INVALIDr) {
            return SOC_E_UNAVAIL;
        }
#ifdef BCM_CMICM_SUPPORT
        if (soc_feature(unit, soc_feature_cmicm)) {
            /* In CMICm these regs are accessed via SCHAN */
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
            soc_reg_field_set(unit, reg, &rval, MD_DEVADf, devad);
            soc_reg_field_set(unit, reg, &rval, IEEE_DEVICES_IN_PKGf,
                              IS_HG_PORT(unit, port) ? 0x03 : 0x15);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
        } else
#endif /* CMICm Support */
        {
            rval = soc_pci_read
                (unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0));
            soc_reg_field_set(unit, reg, &rval, MD_DEVADf, devad);
            soc_reg_field_set(unit, reg, &rval, IEEE_DEVICES_IN_PKGf,
                              IS_HG_PORT(unit, port) ? 0x03 : 0x15);
            soc_pci_write
                (unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0), rval);
        }
    }
    if (SAL_BOOT_QUICKTURN) {
#ifdef  BCM_FIREBOLT_SUPPORT
        /* Kick the QT phy model */
        if (SOC_IS_FBX(unit)) {
            uint16 rv, kv;
            int phy_addr;

            rv = 0x0800; /* Reset */
            if (IS_HG_PORT(unit,port)) {
                kv = 0x0000; /* Configure for HG mode */
            } else {
                kv = 0x0002; /* Configure for XE mode */
            }
            phy_addr = port + 0x41;
            if (SOC_IS_SCORPION(unit)) {
                /* Skip over CMIC at port 0, use bus 1 instead of 2 */
                phy_addr -= 0x21;
            }
            SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x00, rv));
            SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x00, kv));
        } else
#endif /* BCM_FIREBOLT_SUPPORT */
        {
            SOC_IF_ERROR_RETURN(soc_miim_write(unit, port + 1, 0x00, 0x0000));
        }
    }

    return SOC_E_NONE;
}

#endif /* BCM_BIGMAC_SUPPORT */

#if defined(BCM_HELIX15_SUPPORT) || defined(BCM_FELIX15_SUPPORT) || \
        defined(BCM_FIREBOLT2_SUPPORT)
/*
 *      Get LMD Enable status and update the SOC lmd_pbm.
 */
STATIC int
_xmac_lmd_enable_status_update(int unit)
{

    SOC_PBMP_CLEAR(SOC_LMD_PBM(unit));

    if (soc_feature(unit, soc_feature_lmd)) {
        uint32      dev_cfg;
        uint32      lmd_enable = 0;

        if (SOC_IS_FIREBOLT2(unit)) {
#ifndef EXCLUDE_BCM56324
            
#endif /* EXCLUDE_BCM56324 */
            lmd_enable = soc_property_get(unit, spn_LMD_ENABLE_PBMP,
                                          0);

        } else if (!SAL_BOOT_QUICKTURN) {
            SOC_IF_ERROR_RETURN(READ_BSAFE_GLB_DEV_STATUSr(unit, &dev_cfg));
            if (soc_reg_field_get(unit,
                                  BSAFE_GLB_DEV_STATUSr,
                                  dev_cfg,
                                  PROD_CFG_VLDf)) {

                SOC_IF_ERROR_RETURN
                    (READ_BSAFE_GLB_PROD_CFGr(unit, &dev_cfg));
                lmd_enable = soc_reg_field_get(unit,
                                               BSAFE_GLB_PROD_CFGr,
                                               dev_cfg,
                                               LMD_ENABLEf);
            }
        }

        lmd_enable <<= SOC_HG_OFFSET(unit);
        SOC_PBMP_WORD_SET(SOC_LMD_PBM(unit), 0, lmd_enable);
    }

    return SOC_E_NONE;
}
#endif /* BCM_HELIX15_SUPPORT || BCM_FELIX15_SUPPORT ||
          BCM_FIREBOLT2_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT
/*
 * Function:
 *      soc_reset_bcm56504_a0
 * Purpose:
 *      Special reset sequencing for BCM56504
 *      Setup SBUS block mapping.
 */

STATIC void
soc_reset_bcm56504_a0(int unit)
{
    uint32              val, sbus_val, to_usec;
    val = 0;

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

    /* Reset all blocks */
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);


    /* bring the blocks out of reset */
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XG_PLL_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val,
                      CMIC_BSAFE_CLKGEN_RST_Lf,1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(50);
    soc_xgxs_lcpll_reset(unit);
    sal_usleep(to_usec);

#if defined(BCM_HELIX_SUPPORT) || defined(BCM_FELIX_SUPPORT)
    /* Felix/Helix only */
    if (SOC_IS_FX_HX(unit)) {
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GXP_RST_Lf, 1);
    }
#endif /* BCM_FELIX_SUPPORT || BCM_HELIX_SUPPORT */
    if (soc_reg_field_valid(unit, CMIC_SOFT_RESET_REGr, CMIC_BSAFE_RST_Lf)) {
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_BSAFE_RST_Lf, 1);
    }
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_IP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_EP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_MMU_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GP_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    /*
     * Firebolt/Helix/Felix
     * ring0 [00] : IPIPE[7] -> IPIPE_HI[8]
     * ring1 [01] : EPIPE[9] -> EPIPE_HI[10]
     * ring2 [10] : gport0[0] ->  gport1[1] -> xport0[2] ->
     *              xport1[3] ->  xport2[4] -> xport3[5] -> MMU[6]
     *              gport[12] ->  gport[13] (Felix)
     * ring3 [11] : bsafe[11]
     *
     * 15 14 13 12 11 10 9  8  7  6  5  4  3  2  1 0
     * 0000__1010__1101__0100__0010__1010__1010__1010
     */

    sbus_val = 0x0ad42aaa;
    WRITE_CMIC_SBUS_RING_MAPr(unit, sbus_val);
    /*
     * Min 20 msec delay after LCPLL reset before checking for lock.
     * Lock checking required only if HG/XE ports present.
     */
    if (SOC_PBMP_NOT_NULL(PBMP_XE_ALL(unit)) ||
        SOC_PBMP_NOT_NULL(PBMP_HG_ALL(unit))) {
        soc_xgxs_lcpll_lock_check(unit);
    }

#if defined(BCM_HELIX_SUPPORT) || defined(BCM_FELIX_SUPPORT)
    /*
     * Verify Serdes base address is valid before removing Gigabit reset
     */
    if (!SAL_BOOT_QUICKTURN && SOC_IS_FX_HX(unit)) {
        soc_timeout_t   to;
        uint32          dev_cfg = 0;
        int             rv = SOC_E_NONE;

        soc_timeout_init(&to, 1000000, 1); /* One second timeout */

        for (;;) {
            if ((rv = READ_BSAFE_GLB_DEV_STATUSr(unit, &dev_cfg)) < 0) {
                break;
            }
            if (soc_reg_field_get(unit,
                                  BSAFE_GLB_DEV_STATUSr,
                                  dev_cfg,
                                  PROD_CFG_VLDf) == 1) {
                rv = SOC_E_NONE;
                break;
            }
            if (soc_reg_field_get(unit,
                                  BSAFE_GLB_DEV_STATUSr,
                                  dev_cfg,
                                  INIT_DONEf) == 1) {
                if ((soc_reg_field_get(unit,
                                       BSAFE_GLB_DEV_STATUSr,
                                       dev_cfg, INIT_KEYf) == 0) &&
                    (soc_reg_field_get(unit,
                                       BSAFE_GLB_DEV_STATUSr,
                                       dev_cfg, BUSYf) == 0)) {
                    rv = SOC_E_NONE;
                    break;
                }
                
            }
            if (soc_timeout_check(&to)) {
                rv = SOC_E_TIMEOUT;
                break;
            }
            sal_usleep(to_usec);
        }
        if (rv < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_reset_bcm56504_a0: Device status valid check failed on unit %d\n"),
                       unit));
        }
    }
#endif /* BCM_HELIX_SUPPORT || BCM_FELIX_SUPPORT */
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GX4_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);

#if defined(BCM_HELIX15_SUPPORT) || defined(BCM_FELIX15_SUPPORT) || \
        defined(BCM_FIREBOLT2_SUPPORT)
    /*
     * Extract the XPORT LMD operation mode.
     */
    if (_xmac_lmd_enable_status_update(unit) < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_reset_bcm56504_a0: LMD status update failed on unit %d\n"),
                   unit));
    }
#endif /* BCM_HELIX15_SUPPORT || BCM_FELIX15_SUPPORT ||
          BCM_FIREBOLT2_SUPPORT */
}
#endif /* BCM_FIREBOLT_SUPPORT */

#ifdef BCM_RAPTOR1_SUPPORT
/*
 * Function:
 *      soc_reset_bcm56218_a0
 * Purpose:
 *      Special reset sequencing for BCM56218
 *      Setup SBUS block mapping.
 */

STATIC void
soc_reset_bcm56218_a0(int unit)
{
    uint32              val, to_usec;
    val = 0;

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

    /* Reset all blocks */
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);


    /* bring the blocks out of reset */
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XG_PLL_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_G2P51_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_G2P50_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GX12_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GX2_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(50);
    soc_xgxs_lcpll_reset(unit);
    sal_usleep(to_usec);

    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_FP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_IP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_EP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_MMU_RST_Lf, 1);

    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    /*
     * Raptor
     * ring0 [00] : IPIPE[7] -> IPIPE_HI[8]
     * ring1 [01] : EPIPE[9] -> EPIPE_HI[10]
     * ring2 [10] : gport4[1] -> gport0[2] ->
     *              gport1[3] ->  gport2[4] -> gport3[5] -> MMU[6]
     * ring3 [11] : bsafe[11]
     *
     * 15 14 13 12 11 10 9  8  7  6  5  4  3  2  1 0
     * 0000__XXXX__1101__0100__0010__1010__1010__10XX
     */

    val = 0x0ad42aaa;
    WRITE_CMIC_SBUS_RING_MAPr(unit, val);
    /*
     * Min 20 msec delay after LCPLL reset before checking for lock.
     */
    soc_xgxs_lcpll_lock_check(unit);
}
#endif /* BCM_RAPTOR_SUPPORT */

#if defined(BCM_RAVEN_SUPPORT)
STATIC void
soc_mdio_setup(int unit)
{
    uint32 val;

    READ_CMIC_GFPORT_CLOCK_CONFIGr(unit, &val);
    soc_reg_field_set(unit, CMIC_GFPORT_CLOCK_CONFIGr, &val,
                      SEL_DOZEN_SERDES_0_REF_CLK_SRCf, 3);
    soc_reg_field_set(unit, CMIC_GFPORT_CLOCK_CONFIGr, &val,
                      SEL_DOZEN_SERDES_1_REF_CLK_SRCf, 3);
    soc_reg_field_set(unit, CMIC_GFPORT_CLOCK_CONFIGr, &val,
                      PROG_DOZEN_SERDES_PLLDIV_CTRL_DEFf, 3);
    WRITE_CMIC_GFPORT_CLOCK_CONFIGr(unit, val);

    READ_CMIC_GFPORT_CLOCK_CONFIGr(unit, &val);
    soc_reg_field_set(unit, CMIC_GFPORT_CLOCK_CONFIGr, &val,
                      SEL_DOZEN_SERDES_0_REF_CLK_SRCf, 1);
    soc_reg_field_set(unit, CMIC_GFPORT_CLOCK_CONFIGr, &val,
                      SEL_DOZEN_SERDES_1_REF_CLK_SRCf, 1);
    soc_reg_field_set(unit, CMIC_GFPORT_CLOCK_CONFIGr, &val,
                      PROG_DOZEN_SERDES_PLLDIV_CTRL_DEFf, 0);
    WRITE_CMIC_GFPORT_CLOCK_CONFIGr(unit, val);

    return;
}

/*
 * Function:
 *      soc_reset_bcm56224_a0
 * Purpose:
 *      Special reset sequencing for BCM56224
 *      Setup SBUS block mapping.
 */

STATIC void
soc_reset_bcm56224_a0(int unit)
{
    uint32              val, to_usec;
    val = 0;

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

    /* Reset all blocks */
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);


    /* bring the blocks out of reset */
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XG_PLL_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_G2P51_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_G2P50_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_G2P52_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_G2P53_RST_Lf, 1);
    if (soc_feature(unit, soc_feature_mdio_setup)) {
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GX12_RST_Lf, 0);
    } else {
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GX12_RST_Lf, 1);
    }
    /*soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GX2_RST_Lf, 1);*/
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(50);
    soc_xgxs_lcpll_reset(unit);
    sal_usleep(to_usec);

    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_FP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_IP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_EP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_MMU_RST_Lf, 1);

    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);
    /*
     * Raptor
     * ring0 [00] : IPIPE[7] -> IPIPE_HI[8]
     * ring1 [01] : EPIPE[9] -> EPIPE_HI[10]
     * ring2 [10] : gport4[1] -> gport0[2] ->
     *              gport1[3] ->  gport2[4] -> gport3[5] -> MMU[6]
     * ring3 [11] : bsafe[11]
     *
     * 15 14 13 12 11 10 9  8  7  6  5  4  3  2  1 0
     * 0000__XXXX__1101__0100__0010__1010__1010__10XX
     */

    val = 0x0ad42aaa;
    WRITE_CMIC_SBUS_RING_MAPr(unit, val);
    /*
     * Min 20 msec delay after LCPLL reset before checking for lock.
     */
    soc_xgxs_lcpll_lock_check(unit);
    if (soc_feature(unit, soc_feature_mdio_setup)) {
        soc_mdio_setup(unit);
    }
}
#endif /* BCM_RAVEN_SUPPORT */

#if defined(BCM_HAWKEYE_SUPPORT)
/*
 * Function:
 *      soc_reset_bcm53314_a0
 * Purpose:
 *      Special reset sequencing for BCM53314
 *      Setup SBUS block mapping.
 */

STATIC void
soc_reset_bcm53314_a0(int unit)
{
    uint32  val, rval, to_usec;
    int i, retry = 3, locked = 0;

    to_usec = SAL_BOOT_QUICKTURN ? (500 * MILLISECOND_USEC) :
                                   (25 * MILLISECOND_USEC);

    /* Reset all blocks */
    val = 0;
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);


    /* Bring LCPLL out of reset */
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XG_PLL_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);
    /* Check lock status */
    if ((!SAL_BOOT_PLISIM) && (!SAL_BOOT_QUICKTURN)) {
        for (i = 0; i < retry ; i++) {
            READ_CMIC_XGXS_PLL_STATUSr(unit, &rval);
            locked = soc_reg_field_get(unit, CMIC_XGXS_PLL_STATUSr,
                                            rval, CMIC_XG_PLL_LOCKf);
            if (locked)
                break;
            sal_usleep(to_usec);
        }
        if (!locked) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "LCPLL not locked on unit %d "
                                  "status = 0x%08x\n"),
                       unit, rval));
        }
    }

    /* Bring qsgmii/qgphy out of reset */
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_QSGMII2X0_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_QSGMII2X1_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_QGPHY0_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_QGPHY1_RST_Lf, 1);

    /* Wait for qsgmii/qgphy PLL lock */
    sal_usleep(to_usec);

    /* Bring gport/ip/ep/mmu out of reset */
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_IP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_EP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_MMU_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GP_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);

    sal_usleep(to_usec);
    /*
     * Raptor
     * ring0 [00] : IPIPE[7] -> IPIPE_HI[8]
     * ring1 [01] : EPIPE[9] -> EPIPE_HI[10]
     * ring2 [10] : gport4[1] -> gport0[2] ->
     *              gport1[3] ->  gport2[4] -> gport3[5] -> MMU[6]
     * ring3 [11] : bsafe[11]
     *
     * 15 14 13 12 11 10 9  8  7  6  5  4  3  2  1 0
     * 0000__XXXX__1101__0100__0010__1010__1010__10XX
     */

    val = 0x0ad42aaa;
    WRITE_CMIC_SBUS_RING_MAPr(unit, val);

    /* set UNMANAGED_MODE of CMIC_CHIP_MODE_CONTROL to 0 in the init sequence */
    val = 0x0;
    WRITE_CMIC_CHIP_MODE_CONTROLr(unit, val);

    /* set CMIC_INTR_WAIT_CYCLES to 0 in the init sequence */
    val = 0x0;
    WRITE_CMIC_INTR_WAIT_CYCLESr(unit, val);

    READ_CMIC_QGPHY_QSGMII_CONTROLr(unit, &val);
    soc_reg_field_set(unit, CMIC_QGPHY_QSGMII_CONTROLr, &val, SEL_LEDRAM_SERIAL_DATAf, 1);
    WRITE_CMIC_QGPHY_QSGMII_CONTROLr(unit, val);
}
#endif /* BCM_HAWKEYE_SUPPORT */

#if defined (BCM_BRADLEY_SUPPORT)
/*
 * Function:
 *      soc_reset_bcm56800_a0
 * Purpose:
 *      Special reset sequencing for BCM56800
 *      Setup SBUS block mapping.
 */

STATIC void
soc_reset_bcm56800_a0(int unit)
{
    uint32              val, to_usec;


    val = 0;
    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (20 * MILLISECOND_USEC);

    /* Reset all blocks */
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);

    /* Bring LCPLL blocks out of reset */
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val,
                      CMIC_XG_PLL0_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val,
                      CMIC_XG_PLL1_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val,
                      CMIC_BSAFE_CLKGEN_RST_Lf,1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    /*
     * Min 20 msec delay after LCPLL reset before checking for lock.
     */
    if ((!SAL_BOOT_PLISIM) && (!SAL_BOOT_QUICKTURN)) {
        _bradley_lcpll_lock_check(unit);
    }

    /* Bring GX4 block out of reset after 1 msec after PLLs */
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GX4_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    /*
     * NOTE
     * The remaining blocks will be brought out of
     * reset after Unicore reset.
     */

    /*
     * ring0 [00] : ipipe[1] -> ipipe_x[2] -> ipipe_y[3] ->
     *              epipe[4] -> epipe_x[5] -> epipe_y[6]
     * ring1 [01] : mmu[13]
     * ring2 [10] : bsafe[14]
     * ring3 [11] : gxport[0]
     *
     * 15 14 13 12 11 10 9  8  7  6  5  4  3  2  1  0
     * 0010__0100__0000__0000__0000__0000__0000__0011
     */

    val = 0x24000003;
    WRITE_CMIC_SBUS_RING_MAPr(unit, val);
}
#endif /* BCM_BRADLEY_SUPPORT */

#ifdef BCM_TRIUMPH_SUPPORT
/*
 * Function:
 *      soc_reset_bcm56624_a0
 * Purpose:
 *      Special reset sequencing for BCM56624
 *      Setup SBUS block mapping.
 */

STATIC void
soc_reset_bcm56624_a0(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_tcam_info_t *tcam_info;
    uint32 val, to_usec;

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);


    /* bring the blocks out of reset */
    READ_CMIC_SOFT_RESET_REGr(unit, &val);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XG_PLL0_RST_Lf,
                      1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XG_PLL1_RST_Lf,
                      1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GX8_0_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GX8_1_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GX8_2_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GX9_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XGP0_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XGP1_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XGP2_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XGP3_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XP0_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XP1_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XP2_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XP3_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_SP_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    if (!soc_feature(unit, soc_feature_ignore_cmic_xgxs_pll_status) &&
        !SAL_BOOT_PLISIM && !SAL_BOOT_QUICKTURN) {
        /* LCPLL 0 is for Hyperlite and serdes x8/x9 */
        READ_CMIC_XGXS0_PLL_STATUSr(unit, &val);
        if (!soc_reg_field_get(unit, CMIC_XGXS0_PLL_STATUSr, val,
                               CMIC_XGPLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "LCPLL 0 not locked on unit %d "
                                  "status = 0x%08x\n"), unit, val));
        }

        /* LCPLL 1 is for Unicore */
        READ_CMIC_XGXS1_PLL_STATUSr(unit, &val);
        if (!soc_reg_field_get(unit, CMIC_XGXS1_PLL_STATUSr, val,
                               CMIC_XGPLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "LCPLL 1 not locked on unit %d "
                                  "status = 0x%08x\n"), unit, val));
        }
    }

    if (soc_feature(unit, soc_feature_esm_support)) {
        soc_triumph_esm_init_read_config(unit);
        tcam_info = soc->tcam_info;

        /* Assert reset pin of external TCAM, esm_init will de-assert it */
        READ_CMIC_SOFT_RESET_REGr(unit, &val);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, EXT_TCAM_RSTf, 1);
        WRITE_CMIC_SOFT_RESET_REGr(unit, val);

        /* Program PLL for DDR */
        READ_CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_1r(unit, &val);
        soc_reg_field_set(unit, CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_1r, &val,
                          EN_CMLBUF1f, 1);
        WRITE_CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_1r(unit, val);

        READ_CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_2r(unit, &val);
        soc_reg_field_set(unit, CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_2r, &val,
                          BYPASS_SDMODf, 1);
        WRITE_CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_2r(unit, val);

        if (tcam_info != NULL) {
            soc_triumph_esm_init_set_sram_freq(unit, tcam_info->sram_freq);
        }

        /* Program PLL for TCAM */
        READ_CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_1r(unit, &val);
        soc_reg_field_set(unit, CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_1r, &val,
                          EN_CMLBUF1f, 1);
        WRITE_CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_1r(unit, val);

        READ_CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_2r(unit, &val);
        soc_reg_field_set(unit, CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_2r, &val,
                          BYPASS_SDMODf, 1);
        WRITE_CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_2r(unit, val);

        if (tcam_info != NULL) {
            soc_triumph_esm_init_set_tcam_freq(unit, tcam_info->tcam_freq);
        }
    }

    /*
     * Triumph
     * ring0 [00] : IPIPE[10] -> EPIPE[11]
     * ring1 [01] : MMU[12]
     * ring2 [10] : OTPC[13] -> ESM[14]
     * ring3 [11] : XGPORT0[2] -> XGPORT1[3] -> XGPORT2[4] -> XGPORT3[5] ->
     *              GXPORT0[6] -> GXPORT1[7] -> GXPORT2[8] -> GXPORT3[9] ->
     *              SPORT[1]
     *
     * 15 14 13 12 11 10 9  8  7  6  5  4  3  2  1 0
     * 0010__1001__0000__1111__1111__1111__1111__11XX
     */

    val = 0x290ffffe;
    WRITE_CMIC_SBUS_RING_MAPr(unit, val);

    val = 0x7d0;
    WRITE_CMIC_SBUS_TIMEOUTr(unit, val);

    /* Bring IP, EP, and MMU blocks out of reset */
    READ_CMIC_SOFT_RESET_REGr(unit, &val);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_EP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_IP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_MMU_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    if (soc_feature(unit, soc_feature_esm_support)) {
        /* Bring PVT out of reset */
        READ_CMIC_SOFT_RESET_REGr(unit, &val);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_PVT_RST_Lf,
                          1);
        WRITE_CMIC_SOFT_RESET_REGr(unit, val);

        /* Bring DDR PLL out of reset */
        READ_CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_2r(unit, &val);
        soc_reg_field_set(unit, CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_2r, &val,
                          ARESETf, 0);
        WRITE_CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_2r(unit, val);
        sal_usleep(to_usec);
        soc_reg_field_set(unit, CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_2r, &val,
                          DRESETf, 0);
        WRITE_CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_2r(unit, val);
        sal_usleep(to_usec);
        if (soc->tcam_info != NULL &&
            (!SAL_BOOT_QUICKTURN && !SAL_BOOT_PLISIM && !SAL_BOOT_BCMSIM)) {
            /* Get DDR PLL lock status (bit 1) */
            READ_CMIC_MISC_STATUSr(unit, &val);
            if (!(val & 0x2)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                                     "DDR PLL not locked.\n")));
            }
        }

        /* Bring TCAM PLL out of reset */
        READ_CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_2r(unit, &val);
        soc_reg_field_set(unit, CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_2r, &val,
                          ARESETf, 0);
        WRITE_CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_2r(unit, val);
        sal_usleep(to_usec);
        soc_reg_field_set(unit, CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_2r, &val,
                          DRESETf, 0);
        WRITE_CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_2r(unit, val);
        sal_usleep(to_usec);
        if (soc->tcam_info != NULL &&
            (!SAL_BOOT_QUICKTURN && !SAL_BOOT_PLISIM && !SAL_BOOT_BCMSIM)) {
            /* Get TCAM PLL lock status (bit 2) */
            READ_CMIC_MISC_STATUSr(unit, &val);
            if (!(val & 0x4)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                                     "TCAM PLL not locked.\n")));
            }
        }

        /* Bring ESM block out of reset */
        READ_CMIC_SOFT_RESET_REGr(unit, &val);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_ESM_RST_Lf,
                          1);
        WRITE_CMIC_SOFT_RESET_REGr(unit, val);
        sal_usleep(to_usec);
    }
}
#endif  /* BCM_TRIUMPH_SUPPORT */

#ifdef BCM_TRIUMPH2_SUPPORT
/*
 * Function:
 *      soc_reset_bcm56634_a0
 * Purpose:
 *      Special reset sequencing for BCM56634
 *      Setup SBUS block mapping.
 */

STATIC void
soc_reset_bcm56634_a0(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_tcam_info_t *tcam_info;
    uint32 val, to_usec, val2 = 0;

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);


    /* bring the blocks out of reset */
    READ_CMIC_SOFT_RESET_REGr(unit, &val);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XG_PLL0_RST_Lf,
                      1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XG_PLL1_RST_Lf,
                      1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GX8_0_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GX8_1_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GX8_2_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GX9_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, XQ0_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, XQ1_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, XQ2_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, XQ3_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, XQ4_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, XQ5_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XP0_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XP1_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XP2_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XP3_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GP0_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GP1_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_SP_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    soc_reg_field_set(unit, CMIC_SOFT_RESET_REG_2r, &val2,
                      XQ0_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REG_2r, &val2,
                      XQ1_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REG_2r, &val2,
                      XQ2_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REG_2r, &val2,
                      XQ3_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REG_2r, &val2,
                      XQ4_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REG_2r, &val2,
                      XQ5_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REG_2r, &val2, NS_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REG_2r(unit, val2);
    sal_usleep(to_usec);

    if (soc_feature(unit, soc_feature_esm_support) || SOC_IS_TRIUMPH2(unit)) {
        soc_triumph_esm_init_read_config(unit);
    }
    tcam_info = soc->tcam_info;

    if (tcam_info != NULL) {
        /* Assert reset pin of external TCAM, esm_init will de-assert it */
        READ_CMIC_SOFT_RESET_REGr(unit, &val);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, EXT_TCAM_RSTf, 1);
        WRITE_CMIC_SOFT_RESET_REGr(unit, val);

        /* Program PLL for DDR */
        READ_CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_1r(unit, &val);
        soc_reg_field_set(unit, CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_1r, &val,
                          EN_CMLBUF1f, 1);
        WRITE_CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_1r(unit, val);

        READ_CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_2r(unit, &val);
        soc_reg_field_set(unit, CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_2r, &val,
                          BYPASS_SDMODf, 1);
        WRITE_CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_2r(unit, val);

        soc_triumph_esm_init_set_sram_freq(unit, tcam_info->sram_freq);

        /* Program PLL for TCAM */
        READ_CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_1r(unit, &val);
        soc_reg_field_set(unit, CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_1r, &val,
                          EN_CMLBUF1f, 1);
        WRITE_CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_1r(unit, val);

        READ_CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_2r(unit, &val);
        soc_reg_field_set(unit, CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_2r, &val,
                          BYPASS_SDMODf, 1);
        WRITE_CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_2r(unit, val);

        soc_triumph_esm_init_set_tcam_freq(unit, tcam_info->tcam_freq);
    }


    /*
     * Triumph2
     *
     * map_0 includes blocks 0-7
     * map_1 includes blocks 8-15
     * map_2 includes blocks 16-23
     * map_3 includes blocks 24-31
 */

    val = 0x33022140;
    WRITE_CMIC_SBUS_RING_MAP_0r(unit, val);

    val = 0x00033333;
    WRITE_CMIC_SBUS_RING_MAP_1r(unit, val);

    val = 0x00333333;
    WRITE_CMIC_SBUS_RING_MAP_2r(unit, val);

    val = 0x0;
    WRITE_CMIC_SBUS_RING_MAP_3r(unit, val);

    val = 0x2000;
    WRITE_CMIC_SBUS_TIMEOUTr(unit, val);

    /* Bring IP, EP, and MMU blocks out of reset */
    READ_CMIC_SOFT_RESET_REGr(unit, &val);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_EP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_IP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_MMU_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    if (soc_feature(unit, soc_feature_esm_support)) {
        /* Bring PVT out of reset */
        READ_CMIC_SOFT_RESET_REGr(unit, &val);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_PVT_RST_Lf,
                          1);
        WRITE_CMIC_SOFT_RESET_REGr(unit, val);

        /* Bring DDR PLL out of reset */
        READ_CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_2r(unit, &val);
        soc_reg_field_set(unit, CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_2r, &val,
                          ARESETf, 0);
        WRITE_CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_2r(unit, val);
        sal_usleep(to_usec);
        soc_reg_field_set(unit, CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_2r, &val,
                          DRESETf, 0);
        WRITE_CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_2r(unit, val);
        sal_usleep(to_usec);
        if (soc->tcam_info != NULL &&
            (!SAL_BOOT_QUICKTURN && !SAL_BOOT_PLISIM && !SAL_BOOT_BCMSIM)) {
            /* Get DDR PLL lock status (bit 1) */
            READ_CMIC_MISC_STATUSr(unit, &val);
            if (!(val & 0x2)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                                     "DDR PLL not locked.\n")));
            }
        }

        /* Bring TCAM PLL out of reset */
        READ_CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_2r(unit, &val);
        soc_reg_field_set(unit, CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_2r, &val,
                          ARESETf, 0);
        WRITE_CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_2r(unit, val);
        sal_usleep(to_usec);
        soc_reg_field_set(unit, CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_2r, &val,
                          DRESETf, 0);
        WRITE_CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_2r(unit, val);
        sal_usleep(to_usec);
        if (soc->tcam_info != NULL &&
            (!SAL_BOOT_QUICKTURN && !SAL_BOOT_PLISIM && !SAL_BOOT_BCMSIM)) {
            /* Get TCAM PLL lock status (bit 2) */
            READ_CMIC_MISC_STATUSr(unit, &val);
            if (!(val & 0x4)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                                     "TCAM PLL not locked.\n")));
            }
        }

        /* Bring ESM block out of reset */
        READ_CMIC_SOFT_RESET_REGr(unit, &val);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_ESM_RST_Lf,
                          1);
        WRITE_CMIC_SOFT_RESET_REGr(unit, val);
        sal_usleep(to_usec);
    }

    if (soc_feature(unit, soc_feature_gmii_clkout)) {
        val2 = 0;
        /* select GMII 25MHz clock output */
        soc_reg_field_set(unit, CMIC_MISC_CONTROLr, &val2,
                          CLK_RECOVERY_PRI_SELECTf, 1);
        soc_reg_field_set(unit, CMIC_MISC_CONTROLr, &val2,
                          CLK_RECOVERY_BKUP_SELECTf, 1);
        WRITE_CMIC_MISC_CONTROLr(unit, val2);

        /* Select primary and secondary ports */
        val2 = 0;
        soc_reg_field_set(unit, EGR_L1_CLK_RECOVERY_CTRLr, &val2,
                          PRI_PORT_SELf, soc_property_get(unit,
                          "L1_primary_clk_recovery_port", 0x0));
        soc_reg_field_set(unit, EGR_L1_CLK_RECOVERY_CTRLr, &val2,
                          BKUP_PORT_SELf, soc_property_get(unit,
                          "L1_backup_clk_recovery_port", 0x0));
        if (WRITE_EGR_L1_CLK_RECOVERY_CTRLr(unit, val2) != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "Selecting L1 clock recovery ports failed !!\n")));
        }
    }
}
#endif  /* BCM_TRIUMPH2_SUPPORT */

#ifdef BCM_ENDURO_SUPPORT
/*
 * Function:
 *      soc_reset_bcm56334_a0
 * Purpose:
 *      Special reset sequencing for BCM56334
 *      Setup SBUS block mapping.
 */

STATIC void
soc_reset_bcm56334_a0(int unit)
{
    uint32 val, to_usec;
    uint16              dev_id;
    uint8               rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);


    if (soc_feature(unit, soc_feature_delay_core_pll_lock)) {
        /* put Configuring core-clk to 100MHz */
        READ_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_1r(unit, &val);
        soc_reg_field_set(unit, CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_1r, &val,
                          TEST_SELf, 5);
        soc_reg_field_set(unit, CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_1r, &val,
                          TEST_ENABLEf, 1);
        WRITE_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_1r(unit, val);
        READ_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_2r(unit, &val);
        soc_reg_field_set(unit, CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_2r, &val,
                          M1DIVf, 10);
        WRITE_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_2r(unit, val);
        READ_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r(unit, &val);
        soc_reg_field_set(unit, CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r, &val,
                          NDIV_INTf, 40);
        WRITE_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r(unit, val);
        sal_usleep(to_usec);
    }

    /* bring the blocks out of reset */
    READ_CMIC_SOFT_RESET_REGr(unit, &val);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XG1_PLL_RST_Lf,
                      1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XG0_PLL_RST_Lf,
                      1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val,
                            CMIC_GX8_SERDES_0_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val,
                            CMIC_GX8_SERDES_1_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val,
                            CMIC_GX8_SERDES_2_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GP0_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GP1_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GP2_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XQP0_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XQP1_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XQP2_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XQP3_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, XQ0_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, XQ1_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, XQ2_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, XQ3_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, NS_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_OTPC_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, THERMAL_MON_RESETf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, THERMAL_MON_RESETf, 0);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    val = 0x11122200;
    WRITE_CMIC_SBUS_RING_MAP_0r(unit, val);
    val = 0x00330001;
    WRITE_CMIC_SBUS_RING_MAP_1r(unit, val);
    val = 0x0;
    WRITE_CMIC_SBUS_RING_MAP_2r(unit, val);
    WRITE_CMIC_SBUS_RING_MAP_3r(unit, val);
    WRITE_CMIC_SBUS_RING_MAP_4r(unit, val);
    WRITE_CMIC_SBUS_RING_MAP_5r(unit, val);
    WRITE_CMIC_SBUS_RING_MAP_6r(unit, val);
    WRITE_CMIC_SBUS_RING_MAP_7r(unit, val);

    val = 0x7d0;
    WRITE_CMIC_SBUS_TIMEOUTr(unit, val);

    /* Bring IP, EP, and MMU blocks out of reset */
    READ_CMIC_SOFT_RESET_REGr(unit, &val);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_EP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_IP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_MMU_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    if (soc_feature(unit, soc_feature_delay_core_pll_lock)) {
        /* assert the active bits of cmic_soft_reset_reg then de-assert  */
        READ_CMIC_SOFT_RESET_REGr(unit, &val);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GP0_RST_Lf, 0);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GP1_RST_Lf, 0);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GP2_RST_Lf, 0);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr,
                                &val, CMIC_XQP0_RST_Lf, 0);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr,
                                &val, CMIC_XQP1_RST_Lf, 0);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr,
                                &val, CMIC_XQP2_RST_Lf, 0);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr,
                                &val, CMIC_XQP3_RST_Lf, 0);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_EP_RST_Lf, 0);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_IP_RST_Lf, 0);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_MMU_RST_Lf, 0);
        WRITE_CMIC_SOFT_RESET_REGr(unit, val);
        sal_usleep(to_usec);

        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GP0_RST_Lf, 1);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GP1_RST_Lf, 1);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GP2_RST_Lf, 1);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr,
                                &val, CMIC_XQP0_RST_Lf, 1);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr,
                                &val, CMIC_XQP1_RST_Lf, 1);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr,
                                &val, CMIC_XQP2_RST_Lf, 1);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr,
                                &val, CMIC_XQP3_RST_Lf, 1);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_EP_RST_Lf, 1);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_IP_RST_Lf, 1);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_MMU_RST_Lf, 1);
        WRITE_CMIC_SOFT_RESET_REGr(unit, val);
        sal_usleep(to_usec);
    }

    if (soc_feature(unit, soc_feature_gmii_clkout)) {
        uint32 rval;
        static int16 _L1_port_val_map[] = {
            0, 1, 2, 3, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
            4, 5, 6, 7, 8, 9, 10, 11, 0, 0, 12, 12, 13, 13
        }; /* Note: 24, 25 are invalid, but we are just setting 0 in those cases */

        val = soc_property_get(unit, "L1_primary_clk_recovery_port", 0x0);
        if (val >= COUNTOF(_L1_port_val_map)) {
            return;
        }
        READ_CMIC_MISC_CONTROLr(unit, &rval);
        if ((val <= 3) || ((val >= 8) && (val <= 15)) ||
            (val == 26) || (val == 28)) {
            soc_reg_field_set(unit, CMIC_MISC_CONTROLr, &rval,
                              L1_CLK0_RECOVERY_MUXf, _L1_port_val_map[val]);
        } else {
            soc_reg_field_set(unit, CMIC_MISC_CONTROLr, &rval,
                              L1_CLK1_RECOVERY_MUXf, _L1_port_val_map[val]);
        }

        val = soc_property_get(unit, "L1_backup_clk_recovery_port", 0x0);
        if (val >= COUNTOF(_L1_port_val_map)) {
            return;
        }
        if ((val <= 3) || ((val >= 8) && (val <= 15)) ||
            (val == 26) || (val == 28)) {
            soc_reg_field_set(unit, CMIC_MISC_CONTROLr, &rval,
                              L1_CLK0_RECOVERY_MUXf, _L1_port_val_map[val]);
        } else {
            soc_reg_field_set(unit, CMIC_MISC_CONTROLr, &rval,
                              L1_CLK1_RECOVERY_MUXf, _L1_port_val_map[val]);
        }
        WRITE_CMIC_MISC_CONTROLr(unit, rval);
    }
}
#endif  /* BCM_ENDURO_SUPPORT */

#ifdef BCM_SHADOW_SUPPORT
/*
 * Function:
 *      soc_reset_bcm88732_a0
 * Purpose:
 *      Special reset sequencing for BCM88732
 *      Setup SBUS block mapping.
 */

STATIC void
soc_reset_bcm88732_a0(int unit)
{
    uint32              val, val2, to_usec;
    uint32              rval, addr;
    uint16              dev_id;
    uint8               rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    val = 0;
    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

    /* Reset all blocks */
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);


    /* Set NDIV=140 and PDIV=7 (NDIV_INT<< 3 |  PDIV)*/
    addr = soc_reg_addr(unit, CMIC_XGXS0_PLL_CONTROL_3r, REG_PORT_ANY, 0);
    soc_pci_getreg(unit, addr, &rval);
    soc_reg_field_set(unit, CMIC_XGXS0_PLL_CONTROL_3r, &rval, PLLCTRL95_64f, (140 << 3 | 7));
    soc_pci_write(unit, addr, rval);

    addr = soc_reg_addr(unit, CMIC_XGXS1_PLL_CONTROL_3r, REG_PORT_ANY, 0);
    soc_pci_getreg(unit, addr, &rval);
    soc_reg_field_set(unit, CMIC_XGXS1_PLL_CONTROL_3r, &rval, PLLCTRL95_64f, (140 << 3 | 7));
    soc_pci_write(unit, addr, rval);


    /* Bring the PLLs out of reset */
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, LCPLL2_SYS_RESET_Nf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, LCPLL1_SYS_RESET_Nf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, LCPLL0_SYS_RESET_Nf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    /* Miscellaneous other resets */
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, TEMP_MON_PEAK_RESET_Nf,
                      1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    /*
     * Min 20 msec delay after LCPLL reset before checking for lock.
     */
    if (!soc_feature(unit, soc_feature_ignore_cmic_xgxs_pll_status) &&
        !SAL_BOOT_PLISIM && !SAL_BOOT_QUICKTURN) {
        uint32 val;

        READ_CMIC_XGXS0_PLL_STATUSr(unit, &val);
        if (!(soc_reg_field_get(unit, CMIC_XGXS0_PLL_STATUSr,
                                val, CMIC_XGPLL_LOCKf))) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "LCPLL 0 not locked on unit %d status = 0x%08x\n"),
                       unit, val));
        }

        READ_CMIC_XGXS1_PLL_STATUSr(unit, &val);
        if (!(soc_reg_field_get(unit, CMIC_XGXS1_PLL_STATUSr,
                                val, CMIC_XGPLL_LOCKf))) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "LCPLL 1 not locked on unit %d status = 0x%08x\n"),
                       unit, val));
        }

        READ_CMIC_XGXS2_PLL_STATUSr(unit, &val);
        if (!(soc_reg_field_get(unit, CMIC_XGXS2_PLL_STATUSr,
                                val, CMIC_XGPLL_LOCKf))) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "LCPLL 2 not locked on unit %d status = 0x%08x\n"),
                       unit, val));
        }

    }


    /* Bring port blocks out of reset after 1 msec after PLLs */
    READ_CMIC_SOFT_RESET_REGr(unit, &val);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val,
                      XLPORT0_SYS_RESET_Nf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val,
                      XLPORT1_SYS_RESET_Nf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val,
                      XLPORT2_SYS_RESET_Nf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val,
                      XLPORT3_SYS_RESET_Nf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val,
                      XLPORT4_SYS_RESET_Nf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val,
                      XLPORT5_SYS_RESET_Nf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val,
                      XLPORT0_LOGIC_RESET_Nf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val,
                      XLPORT1_LOGIC_RESET_Nf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val,
                      XLPORT2_LOGIC_RESET_Nf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val,
                      XLPORT3_LOGIC_RESET_Nf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val,
                      XLPORT4_LOGIC_RESET_Nf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val,
                      XLPORT5_LOGIC_RESET_Nf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    val2 = 0;
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REG_2r, &val2,
                      NS_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REG_2r(unit, val2);

    /*
     * Shadow - ring map programming information taken from the regs file.
     */
    val = 0x00321100;
    WRITE_CMIC_SBUS_RING_MAP_0r(unit, val);

    val = 0x10320411;
    WRITE_CMIC_SBUS_RING_MAP_1r(unit, val);

    val = 0x00000532;
    WRITE_CMIC_SBUS_RING_MAP_2r(unit, val);

    val = 0x0;
    WRITE_CMIC_SBUS_RING_MAP_3r(unit, val);

    /* Bring MACSEC and Interlaken blocks out of reset */
    READ_CMIC_SOFT_RESET_REGr(unit, &val);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, MS0_SYS_RESET_Nf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, MS1_SYS_RESET_Nf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, IL0_LOGIC_RESET_Nf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, IL0_SYS_RESET_Nf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, IL1_LOGIC_RESET_Nf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, IL1_SYS_RESET_Nf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    /* Bring PE, PD, and MMU blocks out of reset */
    READ_CMIC_SOFT_RESET_REGr(unit, &val);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, PE0_SYS_RESET_Nf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, PE1_SYS_RESET_Nf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, PE0_LOGIC_RESET_Nf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, PD_SYS_RESET_Nf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, MMU0_SYS_RESET_Nf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);

    /* Bring temperature monitor out of reset */
    READ_CMIC_SOFT_RESET_REGr(unit, &val);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val,
                      TEMP_MON_PEAK_RESET_Nf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);
}
#endif  /* BCM_SHADOW_SUPPORT */


#ifdef BCM_HURRICANE1_SUPPORT
/*
 * Function:
 *      soc_reset_bcm56142_a0
 * Purpose:
 *      Special reset sequencing for BCM56142
 *      Setup SBUS block mapping.
 */

STATIC void
soc_reset_bcm56142_a0(int unit)
{
    uint32 val, to_usec;
    uint16              dev_id;
    uint8               rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);


    /* bring the blocks out of reset */
    READ_CMIC_SOFT_RESET_REGr(unit, &val);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XG0_PLL_RST_Lf,
                      1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val,
                            CMIC_QSGMII2X_SERDES_0_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val,
                            CMIC_QSGMII2X_SERDES_1_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val,
                            CMIC_QSGMII2X_SERDES_2_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GP0_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GP1_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_GP2_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XQP0_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XQP1_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XQP2_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XQP3_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, XQ0_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, XQ1_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, XQ2_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, XQ3_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, THERMAL_MON_RESETf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, THERMAL_MON_RESETf, 0);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

   
   val = 0x11122200;
   WRITE_CMIC_SBUS_RING_MAP_0r(unit, val);
   val = 0x00330001;
   WRITE_CMIC_SBUS_RING_MAP_1r(unit, val);
   val = 0x0;
   WRITE_CMIC_SBUS_RING_MAP_2r(unit, val);
   WRITE_CMIC_SBUS_RING_MAP_3r(unit, val);
   WRITE_CMIC_SBUS_RING_MAP_4r(unit, val);
   WRITE_CMIC_SBUS_RING_MAP_5r(unit, val);
   WRITE_CMIC_SBUS_RING_MAP_6r(unit, val);
   WRITE_CMIC_SBUS_RING_MAP_7r(unit, val);

    val = 0x7d0;
    WRITE_CMIC_SBUS_TIMEOUTr(unit, val);

    /* Bring IP, EP, and MMU blocks out of reset */
    READ_CMIC_SOFT_RESET_REGr(unit, &val);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_EP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_IP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_MMU_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, NS_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

}
#endif  /* BCM_HURRICANE_SUPPORT */

#ifdef BCM_HURRICANE2_SUPPORT
/*
 * Function:
 *      soc_reset_bcm56150_a0
 * Purpose:
 *      Special reset sequencing for BCM56150
 */

STATIC int
soc_reset_bcm56150_a0(int unit)
{
    uint32 rval, to_usec;
    uint16 dev_id;
    uint8 rev_id;

    unsigned ts_ref_freq;
    unsigned ts_idx;
    static const soc_pll_param_t ts_pll[] = {  /* values for 500MHz TSPLL output */
      /*     Fref,  Ndiv_int, Ndiv_frac,  Pdiv, Mdiv, Ka, Ki, Kp, VCO_div2 */
        {12800000,       312,   0x80000,     1,    8,  1,  2,  7,        1},
        {20000000,       200,         0,     1,    8,  2,  3,  8,        1},
        {25000000,       160,         0,     1,    8,  2,  4,  9,        1},
        {32000000,       125,         0,     1,    8,  2,  4,  9,        1},
        {50000000,        80,         0,     1,    8,  3,  4, 10,        1},
        {       0,       160,         0,     1,    8,  2,  4,  9,        1}  /* 25MHz, from internal reference */
    };
    unsigned bs_ref_freq;
    unsigned bs_idx;
    uint32 bs_ndiv_high, bs_ndiv_low;
    static const soc_pll_param_t bs_pll[] = {  /* values for 20MHz BSPLL output */
      /*     Fref,  Ndiv_int,  Ndiv_frac,  Pdiv, Mdiv, Ka, Ki, Kp, VCO_div2 */
        {12800000,       250,          0,     1,  160,  1,  2,  7,        0},
        {20000000,       175,          0,     1,  175,  2,  3,  8,        0},
        {25000000,       140,          0,     1,  175,  2,  4,  9,        0},
        {32000000,       108, 0xc0000000,     1,  174,  2,  4,  9,        0}, /* Ndiv=108.75 */
        {50000000,        70,          0,     1,  175,  3,  4, 10,        0},
        {       0,       140,          0,     1,  175,  2,  4,  9,        0}  /* 25MHz, from internal reference */
    };
#if defined(BCM_HITLESS_RESET_SUPPORT)
    int initialize_broadsync;
    const int broadsync_default_ka = 4, broadsync_default_ki = 1, broadsync_default_kp = 8;
#endif /* BCM_HITLESS_RESET_SUPPORT */

    soc_cm_get_id(unit, &dev_id, &rev_id);

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

    /*
     * SBUS ring and block number:
     * Ring0: cmic -> ip-ep -> cmic
     * Ring1: cmic -> xqport0 -> xqport1-> xqport2 -> xqport3-> cmic
     * Ring2: cmic -> gport0 -> gport1 -> gport2 -> cmic
     * Ring3: cmic -> otpc -> mmu -> cmic
     * Ring 4,5,6,7 unused
     */
    WRITE_CMIC_SBUS_RING_MAP_0_7r(unit, 0x11122200); /* block 7  - 0 */
    WRITE_CMIC_SBUS_RING_MAP_8_15r(unit, 0x00430000); /* block 15 - 8 */
    WRITE_CMIC_SBUS_RING_MAP_16_23r(unit, 0x00005004); /* block 23 - 16 */
    WRITE_CMIC_SBUS_RING_MAP_24_31r(unit, 0x00000000); /* block 31 - 24 */

    /* Bring LCPLL out of reset */
    
    sal_usleep(to_usec);

    /* Use 156.25Mhz external or internal reference clock for LCPLL */
    READ_TOP_PLL_BYP_AND_REFCLK_CONTROLr(unit, &rval);
    soc_reg_field_set(unit, TOP_PLL_BYP_AND_REFCLK_CONTROLr, &rval,
                      LC_PLL1_REFCLK_SELf,
                      soc_property_get(unit, spn_XGXS_LCPLL_XTAL_REFCLK, 0));
    WRITE_TOP_PLL_BYP_AND_REFCLK_CONTROLr(unit, rval);

    READ_TOP_XG_PLL1_CTRL_3r(unit, &rval);
    soc_reg_field_set(unit, TOP_XG_PLL1_CTRL_3r, &rval,
                      CML_BYP_ENf,
                      soc_property_get(unit, spn_XGXS_LCPLL_XTAL_REFCLK, 0));
    WRITE_TOP_XG_PLL1_CTRL_3r(unit, rval);


    /* TSPLL configuration: 500MHz TS_CLK from 20/25/32/50MHz refclk */
    /* CMICd divides by 2 on input, so this is a 250MHz clock to TS logic */

    ts_ref_freq = soc_property_get(unit, spn_PTP_TS_PLL_FREF, 0);  /* 0->internal reference */
    /* Set TS_PLL_CLK_IN_SEL based on reference frequency.  If it is 0, use the internal reference */
    SOC_IF_ERROR_RETURN(READ_TOP_PLL_BYP_AND_REFCLK_CONTROLr(unit,&rval));
    soc_reg_field_set(unit, TOP_PLL_BYP_AND_REFCLK_CONTROLr, &rval, TS_PLL_REFCLK_SELf,
                      (ts_ref_freq != 0));
    WRITE_TOP_PLL_BYP_AND_REFCLK_CONTROLr(unit, rval);

    for (ts_idx = 0; ts_idx < sizeof(ts_pll)/sizeof(ts_pll[0]); ++ts_idx) {
        if (ts_pll[ts_idx].ref_freq == ts_ref_freq) {
            break;
        }
    }
    if (ts_idx == sizeof(ts_pll)/sizeof(ts_pll[0])) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Invalid value for PTP_TS_PLL_REF (%u).  No default PLL params.\n"), ts_ref_freq));
        /* Could check for existence of SOC properties for VCO_DIV2, KA, KI, KP, NDIV_INT, NDIV_FRAC, MDIV, PDIV, and if
           all exist, use them.  For now, just fail.
        */
    } else {
        /* Enable software overwrite of TimeSync PLL settings. */
        SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r(unit, &rval));
        soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, CMIC_TO_TS_PLL_LOADf, 1);
        WRITE_TOP_MISC_CONTROL_1r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_0r(unit,&rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_0r, &rval, VCO_DIV2f,
                          soc_property_get(unit, spn_PTP_TS_VCO_DIV2, ts_pll[ts_idx].vco_div2));
        WRITE_TOP_TS_PLL_CTRL_0r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_4r(unit,&rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_4r, &rval, KAf,
                          soc_property_get(unit, spn_PTP_TS_KA, ts_pll[ts_idx].ka));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_4r, &rval, KIf,
                          soc_property_get(unit, spn_PTP_TS_KI, ts_pll[ts_idx].ki));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_4r, &rval, KPf,
                          soc_property_get(unit, spn_PTP_TS_KP, ts_pll[ts_idx].kp));
        WRITE_TOP_TS_PLL_CTRL_4r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_3r(unit,&rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_3r, &rval, NDIV_INTf,
                          soc_property_get(unit, spn_PTP_TS_PLL_N, ts_pll[ts_idx].ndiv_int));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_3r, &rval, NDIV_FRACf,
                          ts_pll[ts_idx].ndiv_frac);
        WRITE_TOP_TS_PLL_CTRL_3r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_2r(unit,&rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_2r, &rval, PDIVf,
                          soc_property_get(unit, spn_PTP_TS_PLL_PDIV, ts_pll[ts_idx].pdiv));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_2r, &rval, CH0_MDIVf,
                          soc_property_get(unit, spn_PTP_TS_PLL_MNDIV, ts_pll[ts_idx].mdiv));
        WRITE_TOP_TS_PLL_CTRL_2r(unit, rval);

        /* Strobe channel-0 load-enable to set divisors. */
        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_2r(unit,&rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_2r, &rval, LOAD_EN_CH0f, 0);
        WRITE_TOP_TS_PLL_CTRL_2r(unit, rval);
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_2r, &rval, LOAD_EN_CH0f, 1);
        WRITE_TOP_TS_PLL_CTRL_2r(unit, rval);
    }

    /* 500Mhz TSPLL -> 250MHz ref at timestamper, implies 4ns resolution */
    SOC_TIMESYNC_PLL_CLOCK_NS(unit) = (1/250 * 1000); /* clock period in nanoseconds */

    /* Check to see if the BSPLLs have been configured yet.  If so, we leave them alone,
       for hitless reset support.
       Note: On KT2 A0/B0, the BroadSync Bitclock/Heartbeat regs get reset, so we still
       initialize all of BroadSync.
    */
    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_1r(unit,&rval));

#if defined(BCM_HITLESS_RESET_SUPPORT)
    initialize_broadsync =
        (soc_reg_field_get(unit, TOP_BS_PLL0_CTRL_1r,
                           rval, KAf) == broadsync_default_ka
         && soc_reg_field_get(unit, TOP_BS_PLL0_CTRL_1r,
                              rval, KIf) == broadsync_default_ki
         && soc_reg_field_get(unit, TOP_BS_PLL0_CTRL_1r,
                              rval, KPf) == broadsync_default_kp);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "BSPLL regs:  %s\n"), initialize_broadsync ? "Default" : "Configured"));

    if (!initialize_broadsync) {
        /* JIRA CMICD-110
         * On A0/B0, BroadSync Bitclock/Heartbeat divisors/enable registers get reset,
         * though the outputs themselves do not if the registers were written to an even
         * number of times.  Workaround: when the registers are changed, the register
         * should be written twice, and the same value stored in SRAM.  This code will
         * restore the register from SRAM.
         */

        /* BS0 */
        uint32 reg_cache_base = soc_cmic_bs_reg_cache(unit, 0);

        uint32 config = soc_uc_mem_read(unit,
             reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, config));
        uint32 clk_ctrl = soc_uc_mem_read(unit,
             reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, clk_ctrl));
        uint32 heartbeat_ctrl = soc_uc_mem_read(unit,
             reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, heartbeat_ctrl));
        uint32 heartbeat_down = soc_uc_mem_read(unit,
             reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, heartbeat_down_duration));
        uint32 heartbeat_up = soc_uc_mem_read(unit,
             reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, heartbeat_up_duration));

        if (config == 0 && clk_ctrl == 0 && heartbeat_ctrl == 0 && heartbeat_down == 0 && heartbeat_up == 0) {
            /* PLL was initialized, but there is no saved state.  Reset BroadSync. */
            initialize_broadsync = 1;
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "BSPLL Configured, but no BS state to restore\n")));
        } else {
            /* Double-writes, per CMICD-110 WAR */
            WRITE_CMIC_BS0_CONFIGr(unit, config);
            WRITE_CMIC_BS0_CONFIGr(unit, config);
            WRITE_CMIC_BS0_CLK_CTRLr(unit, clk_ctrl);
            WRITE_CMIC_BS0_CLK_CTRLr(unit, clk_ctrl);
            WRITE_CMIC_BS0_HEARTBEAT_CTRLr(unit, heartbeat_ctrl);
            WRITE_CMIC_BS0_HEARTBEAT_CTRLr(unit, heartbeat_ctrl);
            WRITE_CMIC_BS0_HEARTBEAT_DOWN_DURATIONr(unit, heartbeat_down);
            WRITE_CMIC_BS0_HEARTBEAT_DOWN_DURATIONr(unit, heartbeat_down);
            WRITE_CMIC_BS0_HEARTBEAT_UP_DURATIONr(unit, heartbeat_up);
            WRITE_CMIC_BS0_HEARTBEAT_UP_DURATIONr(unit, heartbeat_up);

            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Restored BS0 regs:\n"
                                    "  config: %08x  clk_ctrl: %08x hb_ctrl: %08x\n"
                                    "  hb_up: %08x hb_down: %08x\n"),
                         config, clk_ctrl, heartbeat_ctrl,
                         heartbeat_down, heartbeat_up));

            /* same, but for BS1 */
            reg_cache_base = soc_cmic_bs_reg_cache(unit, 1);

            config = soc_uc_mem_read(unit,
                 reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, config));
            clk_ctrl = soc_uc_mem_read(unit,
                 reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, clk_ctrl));
            heartbeat_ctrl = soc_uc_mem_read(unit,
                 reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, heartbeat_ctrl));
            heartbeat_down = soc_uc_mem_read(unit,
                 reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, heartbeat_down_duration));
            heartbeat_up = soc_uc_mem_read(unit,
                 reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, heartbeat_up_duration));

            WRITE_CMIC_BS1_CONFIGr(unit, config);
            WRITE_CMIC_BS1_CONFIGr(unit, config);
            WRITE_CMIC_BS1_CLK_CTRLr(unit, clk_ctrl);
            WRITE_CMIC_BS1_CLK_CTRLr(unit, clk_ctrl);
            WRITE_CMIC_BS1_HEARTBEAT_CTRLr(unit, heartbeat_ctrl);
            WRITE_CMIC_BS1_HEARTBEAT_CTRLr(unit, heartbeat_ctrl);
            WRITE_CMIC_BS1_HEARTBEAT_DOWN_DURATIONr(unit, heartbeat_down);
            WRITE_CMIC_BS1_HEARTBEAT_DOWN_DURATIONr(unit, heartbeat_down);
            WRITE_CMIC_BS1_HEARTBEAT_UP_DURATIONr(unit, heartbeat_up);
            WRITE_CMIC_BS1_HEARTBEAT_UP_DURATIONr(unit, heartbeat_up);

            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Restored BS1 regs:\n"
                                    "  config: %08x  clk_ctrl: %08x hb_ctrl: %08x\n"
                                    "  hb_up: %08x hb_down: %08x\n"),
                         config, clk_ctrl, heartbeat_ctrl,
                         heartbeat_down, heartbeat_up));
        }
    }

    if (initialize_broadsync)
#endif  /* BCM_HITLESS_RESET_SUPPORT */
    {
        /* as a signal to upper-level code that the BroadSync is newly initialized
         * disable BroadSync0/1 bitclock output.  Checked in time.c / 1588 firmware    */
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(READ_CMIC_BS0_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &rval, BS_CLK_OUTPUT_ENABLEf, 0);
        WRITE_CMIC_BS0_CONFIGr(unit, rval);

        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(READ_CMIC_BS1_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &rval, BS_CLK_OUTPUT_ENABLEf, 0);
        WRITE_CMIC_BS1_CONFIGr(unit, rval);

        /* BSPLL0 has not been configured, so reset/configure both BSPLL0 and BSPLL1 */
        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL0_RST_Lf, 0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL1_RST_Lf, 0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL0_POST_RST_Lf, 0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL1_POST_RST_Lf, 0);
        WRITE_TOP_SOFT_RESET_REG_2r(unit, rval);

        /* Both BSPLLs configured the same, for 20MHz output by default */

        bs_ref_freq = soc_property_get(unit, spn_PTP_BS_FREF, 0);  /* 0->internal reference */
        /* Set BS_PLL_CLK_IN_SEL based on reference frequency.  If it is 0, use the internal reference */
        SOC_IF_ERROR_RETURN(READ_TOP_PLL_BYP_AND_REFCLK_CONTROLr(unit,&rval));
        soc_reg_field_set(unit, TOP_PLL_BYP_AND_REFCLK_CONTROLr, &rval, BS_PLL0_REFCLK_SELf,
                          (bs_ref_freq != 0));
        soc_reg_field_set(unit, TOP_PLL_BYP_AND_REFCLK_CONTROLr, &rval, BS_PLL1_REFCLK_SELf,
                          (bs_ref_freq != 0));
        WRITE_TOP_PLL_BYP_AND_REFCLK_CONTROLr(unit, rval);

        for (bs_idx = 0; bs_idx < sizeof(bs_pll)/sizeof(bs_pll[0]); ++bs_idx) {
            if (bs_pll[bs_idx].ref_freq == bs_ref_freq) {
                break;
            }
        }
        if (bs_idx == sizeof(bs_pll)/sizeof(bs_pll[0])) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Invalid value for PTP_BS_REF (%u).  No default PLL params.\n"), bs_ref_freq));
            /* Could check for existence of SOC properties for VCO_DIV2, KA, KI, KP, NDIV_INT,
               NDIV_FRAC, MDIV, PDIV, and if all exist, use them.  For now, just fail.
            */
        } else {
            SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_3r(unit,&rval));
            soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_3r, &rval, VCODIV2f,
                              soc_property_get(unit, spn_PTP_BS_VCO_DIV2, bs_pll[bs_idx].vco_div2));
            WRITE_TOP_BS_PLL0_CTRL_3r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_3r(unit,&rval));
            soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_3r, &rval, VCODIV2f,
                              soc_property_get(unit, spn_PTP_BS_VCO_DIV2, bs_pll[bs_idx].vco_div2));
            WRITE_TOP_BS_PLL1_CTRL_3r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_1r(unit,&rval));
            soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_1r, &rval, KAf,
                              soc_property_get(unit, spn_PTP_BS_KA, bs_pll[bs_idx].ka));
            soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_1r, &rval, KIf,
                              soc_property_get(unit, spn_PTP_BS_KI, bs_pll[bs_idx].ki));
            soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_1r, &rval, KPf,
                              soc_property_get(unit, spn_PTP_BS_KP, bs_pll[bs_idx].kp));
            soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_1r, &rval, PDIVf,
                              soc_property_get(unit, spn_PTP_BS_PDIV, bs_pll[bs_idx].pdiv));
            WRITE_TOP_BS_PLL0_CTRL_1r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_1r(unit,&rval));
            soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_1r, &rval, KAf,
                              soc_property_get(unit, spn_PTP_BS_KA, bs_pll[bs_idx].ka));
            soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_1r, &rval, KIf,
                              soc_property_get(unit, spn_PTP_BS_KI, bs_pll[bs_idx].ki));
            soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_1r, &rval, KPf,
                              soc_property_get(unit, spn_PTP_BS_KP, bs_pll[bs_idx].kp));
            soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_1r, &rval, PDIVf,
                              soc_property_get(unit, spn_PTP_BS_PDIV, bs_pll[bs_idx].pdiv));
            WRITE_TOP_BS_PLL1_CTRL_1r(unit, rval);

            bs_ndiv_high = ((soc_property_get(unit, spn_PTP_BS_NDIV_INT, bs_pll[bs_idx].ndiv_int) << 8) |
                            ((soc_property_get(unit, spn_PTP_BS_NDIV_FRAC, bs_pll[bs_idx].ndiv_frac) >> 24) & 0xff));
            bs_ndiv_low = (soc_property_get(unit, spn_PTP_BS_NDIV_FRAC, bs_pll[bs_idx].ndiv_frac) << 8);

            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r(unit,&rval));
            soc_reg_field_set(unit, TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r, &rval, BROAD_SYNC0_LCPLL_FBDIV_1f, bs_ndiv_high);
            WRITE_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_0r(unit,&rval));
            soc_reg_field_set(unit, TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_0r, &rval, BROAD_SYNC0_LCPLL_FBDIV_0f, bs_ndiv_low);
            WRITE_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_0r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r(unit,&rval));
            soc_reg_field_set(unit, TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r, &rval, BROAD_SYNC1_LCPLL_FBDIV_1f, bs_ndiv_high);
            WRITE_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_0r(unit,&rval));
            soc_reg_field_set(unit, TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_0r, &rval, BROAD_SYNC1_LCPLL_FBDIV_0f, bs_ndiv_low);
            WRITE_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_0r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_0r(unit,&rval));
            soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_0r, &rval, CH0_MDIVf,
                              soc_property_get(unit, spn_PTP_BS_MNDIV, bs_pll[bs_idx].mdiv));
            WRITE_TOP_BS_PLL0_CTRL_0r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_0r(unit,&rval));
            soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_0r, &rval, CH0_MDIVf,
                              soc_property_get(unit, spn_PTP_BS_MNDIV, bs_pll[bs_idx].mdiv));
            WRITE_TOP_BS_PLL1_CTRL_0r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_1r(unit,&rval));
            soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_1r, &rval, LOAD_EN_CHf, 0);
            WRITE_TOP_BS_PLL0_CTRL_1r(unit, rval);
            soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_1r, &rval, LOAD_EN_CHf, 1);
            WRITE_TOP_BS_PLL0_CTRL_1r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_1r(unit,&rval));
            soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_1r, &rval, LOAD_EN_CHf, 0);
            WRITE_TOP_BS_PLL1_CTRL_1r(unit, rval);
            soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_1r, &rval, LOAD_EN_CHf, 1);
            WRITE_TOP_BS_PLL1_CTRL_1r(unit, rval);
        }

        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL0_RST_Lf, 1);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL1_RST_Lf, 1);
        WRITE_TOP_SOFT_RESET_REG_2r(unit, rval);

        /* JIRA CMICD-110:  See above logic to restore BroadSync Bitclock/Heartbeat. */
        /* Store default values in the SRAM, so they will be restored correctly */
        /* even if SRAM is not used */
#if defined(BCM_HITLESS_RESET_SUPPORT)
        {
            /* store reset values in SRAM for BS0 */
            uint32 reg_cache_base = soc_cmic_bs_reg_cache(unit, 0);
            READ_CMIC_BS0_CONFIGr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, config), rval);
            READ_CMIC_BS0_CLK_CTRLr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, clk_ctrl), rval);
            READ_CMIC_BS0_HEARTBEAT_CTRLr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, heartbeat_ctrl), rval);
            READ_CMIC_BS0_HEARTBEAT_DOWN_DURATIONr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, heartbeat_down_duration), rval);
            READ_CMIC_BS0_HEARTBEAT_UP_DURATIONr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, heartbeat_up_duration), rval);

            /* store reset values in SRAM for BS1 */
            reg_cache_base = soc_cmic_bs_reg_cache(unit, 1);
            READ_CMIC_BS1_CONFIGr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, config), rval);
            READ_CMIC_BS1_CLK_CTRLr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, clk_ctrl), rval);
            READ_CMIC_BS1_HEARTBEAT_CTRLr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, heartbeat_ctrl), rval);
            READ_CMIC_BS1_HEARTBEAT_DOWN_DURATIONr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, heartbeat_down_duration), rval);
            READ_CMIC_BS1_HEARTBEAT_UP_DURATIONr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, heartbeat_up_duration), rval);
        }
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "Leaving BroadSync in holdover\n")));
#endif  /* BCM_HITLESS_RESET_SUPPORT */
    }

#if defined(BCM_HITLESS_RESET_SUPPORT)
    if (initialize_broadsync)
#endif  /* BCM_HITLESS_RESET_SUPPORT */
    {
        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL0_POST_RST_Lf, 1);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL1_POST_RST_Lf, 1);
        WRITE_TOP_SOFT_RESET_REG_2r(unit, rval);
    }

    /* Check for M, A and Plain versions */
    SOC_IF_ERROR_RETURN(READ_TOP_SWITCH_FEATURE_ENABLEr(unit, &rval));
    if (rval & 0x1) {
        /* BOND_1588_ENABLE */
        SOC_FEATURE_SET(unit, soc_feature_timesync_support);
        LOG_CLI((BSL_META_U(unit,
                            "BOND_1588_ENABLE : "
                            "enabling soc_feature_timesync_support\n")));
    }

    /* De-assert LCPLL's post reset */

    /*
     * Bring port blocks out of reset
     */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_XLP0_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_XLP1_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_QSGMII2X2_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_QSGMII2X1_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_QSGMII2X0_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_QSGMII2X2_FIFO_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_QSGMII2X1_FIFO_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_QSGMII2X0_FIFO_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_GP2_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_GP1_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_GP0_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_QGPHY_RST_Lf, 0xf); 
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    sal_usleep(to_usec);

    /* Bring network sync out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_TS_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_SPARE_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_TS_PLL_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
    
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_TS_PLL_POST_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    sal_usleep(to_usec);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_SBUS_TIMEOUTr(unit, 0x7d0));

    /* Bring IP, EP, and MMU blocks out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_EP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_IP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MMU_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    sal_usleep(to_usec);

    /* reset PVTMON */
    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_0r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_0r, &rval, FUNC_MODE_SELf, 0);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_0r, &rval, MEASUREMENT_CALLIBRATIONf, 5);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_0r, &rval, BG_ADJf, 3);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_0r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_TEMP_MON_PEAK_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_TEMP_MON_PEAK_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_SELECTf, 0);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_RESET_Nf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_RESET_Nf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));

    /* LCPLL CML optimization */
    if ((dev_id == BCM56150_DEVICE_ID) || (dev_id == BCM56152_DEVICE_ID) || \
       (dev_id == BCM53346_DEVICE_ID) || (dev_id == BCM53344_DEVICE_ID) || \
       (dev_id == BCM53343_DEVICE_ID) || (dev_id == BCM53342_DEVICE_ID) || \
       (dev_id == BCM53333_DEVICE_ID) || (dev_id == BCM53334_DEVICE_ID)) {
        SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL0_CTRL_3r(unit, &rval));
        soc_reg_field_set(unit, TOP_XG_PLL0_CTRL_3r, &rval, CML_2ED_OUT_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_XG_PLL0_CTRL_3r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL1_CTRL_3r(unit, &rval));
        soc_reg_field_set(unit, TOP_XG_PLL1_CTRL_3r, &rval, CML_2ED_OUT_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_XG_PLL1_CTRL_3r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_3r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_3r, &rval, CML_2ED_OUT_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_3r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_3r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_3r, &rval, CML_2ED_OUT_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_3r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r(unit, &rval));
        soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, CMIC_TO_XG_PLL0_SW_OVWRf, 1);
        soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, CMIC_TO_XG_PLL1_SW_OVWRf, 1);
        soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, CMIC_TO_BS_PLL0_SW_OVWRf, 1);
        soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, CMIC_TO_BS_PLL1_SW_OVWRf, 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_1r(unit, rval));
    }

    if ((dev_id == BCM56151_DEVICE_ID) || (dev_id == BCM53393_DEVICE_ID) || \
       (dev_id == BCM53394_DEVICE_ID) || (dev_id == BCM53347_DEVICE_ID)) {
        SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL1_CTRL_3r(unit, &rval));
        soc_reg_field_set(unit, TOP_XG_PLL1_CTRL_3r, &rval, CML_2ED_OUT_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_XG_PLL1_CTRL_3r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_3r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_3r, &rval, CML_2ED_OUT_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_3r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_3r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_3r, &rval, CML_2ED_OUT_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_3r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r(unit, &rval));
        soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, CMIC_TO_XG_PLL1_SW_OVWRf, 1);
        soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, CMIC_TO_BS_PLL0_SW_OVWRf, 1);
        soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, CMIC_TO_BS_PLL1_SW_OVWRf, 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_1r(unit, rval));
    }

    /* For 53343, power down unused qsgmii */
    if (dev_id == BCM53343_DEVICE_ID) {
        SOC_IF_ERROR_RETURN(READ_TOP_QSGMII2X_CTRLr(unit, &rval));
        soc_reg_field_set(unit, TOP_QSGMII2X_CTRLr, &rval, IDDQf, 0x7);
        SOC_IF_ERROR_RETURN(WRITE_TOP_QSGMII2X_CTRLr(unit, rval));
    }
    /* For 53342, power down unused qsgmii and unused qgphy */
    if (dev_id == BCM53342_DEVICE_ID) {
        SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_0r(unit, &rval));
        soc_reg_field_set(unit, TOP_QGPHY_CTRL_0r, &rval, IDDQ_BIASf, 0xc);
        soc_reg_field_set(unit, TOP_QGPHY_CTRL_0r, &rval, EXT_PWRDOWNf, 0xff00);
        SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_0r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_QSGMII2X_CTRLr(unit, &rval));
        soc_reg_field_set(unit, TOP_QSGMII2X_CTRLr, &rval, IDDQf, 0x3);
        SOC_IF_ERROR_RETURN(WRITE_TOP_QSGMII2X_CTRLr(unit, rval));
    }

    /* For 53343, power down unused qsgmii */
    if (dev_id == BCM53343_DEVICE_ID) {
        SOC_IF_ERROR_RETURN(READ_TOP_QSGMII2X_CTRLr(unit, &rval));
        soc_reg_field_set(unit, TOP_QSGMII2X_CTRLr, &rval, IDDQf, 0x3);
        SOC_IF_ERROR_RETURN(WRITE_TOP_QSGMII2X_CTRLr(unit, rval));
    }
    /* For 53342, power down unused qsgmii and unused qgphy */
    if (dev_id == BCM53342_DEVICE_ID) {
        SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_0r(unit, &rval));
        soc_reg_field_set(unit, TOP_QGPHY_CTRL_0r, &rval, IDDQ_BIASf, 0xc);
        soc_reg_field_set(unit, TOP_QGPHY_CTRL_0r, &rval, EXT_PWRDOWNf, 0xff00);
        SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_0r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_QSGMII2X_CTRLr(unit, &rval));
        soc_reg_field_set(unit, TOP_QSGMII2X_CTRLr, &rval, IDDQf, 0x3);
        SOC_IF_ERROR_RETURN(WRITE_TOP_QSGMII2X_CTRLr(unit, rval));
    }

    return SOC_E_NONE;
}

#endif  /* BCM_HURRICANE2_SUPPORT */

#ifdef BCM_SCORPION_SUPPORT
/*
 * Function:
 *      soc_reset_bcm56820_a0
 * Purpose:
 *      Special reset sequencing for BCM56820
 *      Setup SBUS block mapping.
 */

STATIC void
soc_reset_bcm56820_a0(int unit)
{
    uint32              val, to_usec;
    uint16              dev_id;
    uint8               rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    val = 0;
    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

    /* Reset all blocks */
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);


    if (soc_feature(unit, soc_feature_delay_core_pll_lock) &&
        soc_property_get(unit, "delay_core_pll_lock", 0)) {
        /* Core clock lock */
        READ_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r(unit, &val);
        soc_reg_field_set(unit, CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r, &val,
                          NDIV_INTf, 80);
        WRITE_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r(unit, val);
        soc_reg_field_set(unit, CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r, &val,
                          NDIV_INTf, 76);
        WRITE_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r(unit, val);
        soc_reg_field_set(unit, CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r, &val,
                          NDIV_INTf, 72);
        WRITE_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r(unit, val);
        soc_reg_field_set(unit, CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r, &val,
                          NDIV_INTf, 68);
        WRITE_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r(unit, val);
        soc_reg_field_set(unit, CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r, &val,
                          NDIV_INTf, 66);
        WRITE_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r(unit, val);
        soc_reg_field_set(unit, CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r, &val,
                          NDIV_INTf, 64);
        WRITE_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r(unit, val);

        READ_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_0r(unit, &val);
        soc_reg_field_set(unit, CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_0r, &val,
                          ICPXf, 0xe);
        WRITE_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_0r(unit, val);
        soc_reg_field_set(unit, CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_0r, &val,
                          KVCO_XSf, 0x4);
        WRITE_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_0r(unit, val);

        READ_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_1r(unit, &val);
        soc_reg_field_set(unit, CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_1r, &val,
                          VCO_RNGf, 0);
        WRITE_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_1r(unit, val);

        READ_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r(unit, &val);
        soc_reg_field_set(unit, CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r, &val,
                          NDIV_INTf, 60);
        WRITE_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r(unit, val);
        soc_reg_field_set(unit, CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r, &val,
                          NDIV_INTf, 56);
        WRITE_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r(unit, val);
        soc_reg_field_set(unit, CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r, &val,
                          NDIV_INTf, 52);
        WRITE_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r(unit, val);
        soc_reg_field_set(unit, CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r, &val,
                          NDIV_INTf, 48);
        WRITE_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r(unit, val);
        soc_reg_field_set(unit, CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r, &val,
                          NDIV_INTf, 44);
        WRITE_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r(unit, val);

        READ_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_2r(unit, &val);
        soc_reg_field_set(unit, CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_2r, &val,
                          M1DIVf, 5);
        WRITE_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_2r(unit, val);

        if (dev_id != BCM56820_DEVICE_ID) {
            READ_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r(unit, &val);
            soc_reg_field_set(unit, CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r,
                              &val, NDIV_INTf, 45);
            WRITE_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r(unit, val);
            if ((dev_id == BCM56822_DEVICE_ID) ||
                (dev_id == BCM56823_DEVICE_ID) ||
                (dev_id == BCM56725_DEVICE_ID)) {
                soc_reg_field_set(unit,
                                  CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r,
                                  &val, NDIV_INTf, 49);
                WRITE_CMIC_CORE_PLL0_CTRL_STATUS_REGISTER_3r(unit, val);
            }
        }
        sal_usleep(to_usec);
        if (!SAL_BOOT_QUICKTURN && !SAL_BOOT_PLISIM && !SAL_BOOT_BCMSIM) {
            /* Get Core PLL lock status (bit 0) */
            READ_CMIC_MISC_STATUSr(unit, &val);
            if (!(val & 0x1)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                                     "Core PLL not locked.\n")));
            }
        }
    }

    /* Bring IP2 clock doubler out of reset */
    READ_CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_2r(unit, &val);
    soc_reg_field_set(unit, CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_2r, &val,
                      ARESETf, 0);
    WRITE_CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_2r(unit, val);
    sal_usleep(to_usec);
    soc_reg_field_set(unit, CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_2r, &val,
                      DRESETf, 0);
    WRITE_CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_2r(unit, val);
    sal_usleep(to_usec);
    if (!SAL_BOOT_QUICKTURN && !SAL_BOOT_PLISIM && !SAL_BOOT_BCMSIM) {
        /* Get DDR PLL lock status (bit 1) */
        READ_CMIC_MISC_STATUSr(unit, &val);
        if (!(val & 0x2)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "IP2 clock doubler not locked.\n")));
        }
    }

    /* Bring MMU0 clock doubler out of reset */
    READ_CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_2r(unit, &val);
    soc_reg_field_set(unit, CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_2r, &val,
                      ARESETf, 0);
    WRITE_CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_2r(unit, val);
    sal_usleep(to_usec);
    soc_reg_field_set(unit, CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_2r, &val,
                      DRESETf, 0);
    WRITE_CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_2r(unit, val);
    sal_usleep(to_usec);
    if (!SAL_BOOT_QUICKTURN && !SAL_BOOT_PLISIM && !SAL_BOOT_BCMSIM) {
        /* Get DDR PLL lock status (bit 2) */
        READ_CMIC_MISC_STATUSr(unit, &val);
        if (!(val & 0x4)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "MMU0 clock doubler not locked.\n")));
        }
    }

    /* Bring MMU1 clock doubler out of reset */
    READ_CMIC_CORE_PLL3_CTRL_STATUS_REGISTER_2r(unit, &val);
    soc_reg_field_set(unit, CMIC_CORE_PLL3_CTRL_STATUS_REGISTER_2r, &val,
                      ARESETf, 0);
    WRITE_CMIC_CORE_PLL3_CTRL_STATUS_REGISTER_2r(unit, val);
    sal_usleep(to_usec);
    soc_reg_field_set(unit, CMIC_CORE_PLL3_CTRL_STATUS_REGISTER_2r, &val,
                      DRESETf, 0);
    WRITE_CMIC_CORE_PLL3_CTRL_STATUS_REGISTER_2r(unit, val);
    sal_usleep(to_usec);
    if (!SAL_BOOT_QUICKTURN && !SAL_BOOT_PLISIM && !SAL_BOOT_BCMSIM) {
        /* Get DDR PLL lock status (bit 3) */
        READ_CMIC_MISC_STATUSr(unit, &val);
        if (!(val & 0x8)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "MMU1 clock doubler not locked.\n")));
        }
    }

    /* bring the blocks out of reset */
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, XG_PLL2_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XG_PLL1_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XG_PLL0_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    /*
     * Min 20 msec delay after LCPLL reset before checking for lock.
     */
    if (!soc_feature(unit, soc_feature_ignore_cmic_xgxs_pll_status) &&
        !SAL_BOOT_PLISIM && !SAL_BOOT_QUICKTURN) {
        uint32 val;

        READ_CMIC_XGXS0_PLL_STATUSr(unit, &val);
        if (!(soc_reg_field_get(unit, CMIC_XGXS0_PLL_STATUSr,
                                val, CMIC_XGPLL_LOCKf))) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "LCPLL 0 not locked on unit %d status = 0x%08x\n"),
                       unit, val));
        }

        READ_CMIC_XGXS1_PLL_STATUSr(unit, &val);
        if (!(soc_reg_field_get(unit, CMIC_XGXS1_PLL_STATUSr,
                                val, CMIC_XGPLL_LOCKf))) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "LCPLL 1 not locked on unit %d status = 0x%08x\n"),
                       unit, val));
        }

        READ_CMIC_XGXS2_PLL_STATUSr(unit, &val);
        if (!(soc_reg_field_get(unit, CMIC_XGXS2_PLL_STATUSr,
                                val, CMIC_XGPLL_LOCKf))) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "LCPLL 2 not locked on unit %d status = 0x%08x\n"),
                       unit, val));
        }
    }

    /* Bring blocks out of reset after 1 msec after PLLs */
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, QGP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XGP0_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XGP1_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XGP2_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XGP3_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, val);
    sal_usleep(to_usec);

    /*
     * NOTE
     * The remaining blocks will be brought out of
     * reset after Unicore reset.
     */

    /*
     * Scorpion
     * ring0 [00] : IPIPE[1] -> EPIPE[4]
     * ring1 [01] : MMU[13]
     * ring2 [10] : [14]
     * ring3 [11] : gxport[0] -> qgport[2]
     *
     * 15 14 13 12 11 10 9  8  7  6  5  4  3  2  1 0
     * XX10__01XX__XXXX__XXXX__XXXX__XX00__XX11__0011
     */

    val = 0x24000033;
    WRITE_CMIC_SBUS_RING_MAPr(unit, val);
}
#endif  /* BCM_SCORPION_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
/*
 * Function:
 *      soc_reset_bcm56840_a0
 * Purpose:
 *      Special reset sequencing for BCM56840
 *      Setup SBUS block mapping.
 */
STATIC int
soc_reset_bcm56840_a0(int unit)
{
    uint32 rval, to_usec;
    uint16 dev_id, drv_dev_id, drv_rev_id;
    uint8 rev_id;
    soc_reg_t reg;
    uint32 addr;
    int index;
    static const soc_reg_t pll_ctrl1_reg[] = {
        CMIC_XGXS0_PLL_CONTROL_1r, CMIC_XGXS1_PLL_CONTROL_1r,
        CMIC_XGXS2_PLL_CONTROL_1r, CMIC_XGXS3_PLL_CONTROL_1r
    };
    static const soc_reg_t pll_ctrl2_reg[] = {
        CMIC_XGXS0_PLL_CONTROL_2r, CMIC_XGXS1_PLL_CONTROL_2r,
        CMIC_XGXS2_PLL_CONTROL_2r, CMIC_XGXS3_PLL_CONTROL_2r
    };
    static const soc_reg_t pll_ctrl3_reg[] = {
        CMIC_XGXS0_PLL_CONTROL_3r, CMIC_XGXS1_PLL_CONTROL_3r,
        CMIC_XGXS2_PLL_CONTROL_3r, CMIC_XGXS3_PLL_CONTROL_3r
    };
    static const soc_reg_t pll_ctrl4_reg[] = {
        CMIC_XGXS0_PLL_CONTROL_4r, CMIC_XGXS1_PLL_CONTROL_4r,
        CMIC_XGXS2_PLL_CONTROL_4r, CMIC_XGXS3_PLL_CONTROL_4r
    };
    static const soc_reg_t pll_status_reg[] = {
        CMIC_XGXS0_PLL_STATUSr, CMIC_XGXS1_PLL_STATUSr,
        CMIC_XGXS2_PLL_STATUSr, CMIC_XGXS3_PLL_STATUSr
    };

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &drv_dev_id, &drv_rev_id);

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);


    /* Use 156.25Mhz reference clock for LCPLL */
    if (soc_property_get(unit, spn_XGXS_LCPLL_XTAL_REFCLK, 0)) {
        for (index = 0; index < 4; index++) {
            reg = pll_ctrl1_reg[index];
            addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
            soc_pci_getreg(unit, addr, &rval);
            soc_reg_field_set(unit, reg, &rval, CH0_MDIVf, 20);
            soc_reg_field_set(unit, reg, &rval, CH3_MDIVf, 25);
            soc_reg_field_set(unit, reg, &rval, CH4_MDIVf, 125);
            soc_reg_field_set(unit, reg, &rval, CH5_MDIVf, 25);
            soc_pci_write(unit, addr, rval);

            reg = pll_ctrl3_reg[index];
            addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
            soc_pci_getreg(unit, addr, &rval);
            soc_reg_field_set(unit, reg, &rval, NDIV_INTf, 140);
            soc_pci_write(unit, addr, rval);

            reg = pll_ctrl2_reg[index];
            addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
            soc_pci_getreg(unit, addr, &rval);
            soc_reg_field_set(unit, reg, &rval, KAf, 4);
            soc_reg_field_set(unit, reg, &rval, KIf, 1);
            soc_reg_field_set(unit, reg, &rval, KPf, 9);
            soc_reg_field_set(unit, reg, &rval, PDIVf, 7);
            soc_pci_write(unit, addr, rval);

            reg = pll_ctrl4_reg[index];
            addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
            soc_pci_getreg(unit, addr, &rval);
            soc_reg_field_set(unit, reg, &rval, CML_BYP_ENf, 1);
            soc_reg_field_set(unit, reg, &rval, TESTOUT_ENf, 0);
            soc_reg_field_set(unit, reg, &rval, CML_2ED_OUT_ENf, 0);
            soc_reg_field_set(unit, reg, &rval, TESTOUT2_ENf, 0);
            soc_pci_write(unit, addr, rval);
        }

        READ_CMIC_MISC_CONTROLr(unit, &rval);
        soc_reg_field_set(unit, CMIC_MISC_CONTROLr, &rval,
                          CMIC_TO_XG_PLL0_SW_OVWRf, 1);
        soc_reg_field_set(unit, CMIC_MISC_CONTROLr, &rval,
                          CMIC_TO_XG_PLL1_SW_OVWRf, 1);
        soc_reg_field_set(unit, CMIC_MISC_CONTROLr, &rval,
                          CMIC_TO_XG_PLL2_SW_OVWRf, 1);
        soc_reg_field_set(unit, CMIC_MISC_CONTROLr, &rval,
                          CMIC_TO_XG_PLL3_SW_OVWRf, 1);
        WRITE_CMIC_MISC_CONTROLr(unit, rval);
    }

    /* Bring LCPLL out of reset */
    READ_CMIC_SOFT_RESET_REGr(unit, &rval);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval, CMIC_XG_PLL0_RST_Lf,
                      1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval, CMIC_XG_PLL1_RST_Lf,
                      1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval, CMIC_XG_PLL2_RST_Lf,
                      1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval, CMIC_XG_PLL3_RST_Lf,
                      1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, rval);

    sal_usleep(to_usec);

    /* Wait for LCPLL lock */
    if (!SAL_BOOT_SIMULATION) {
        for (index = 0; index < 4; index++) {
            reg = pll_status_reg[index];
            addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
            soc_pci_getreg(unit, addr, &rval);
            if (!soc_reg_field_get(unit, reg, rval, CMIC_XGPLL_LOCKf)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "LCPLL %d not locked on unit %d "
                                      "status = 0x%08x\n"), index, unit, rval));
            }
        }
    }

    /* De-assert LCPLL's post reset */
    READ_CMIC_SOFT_RESET_REGr(unit, &rval);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval,
                      CMIC_XG_PLL0_POST_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval,
                      CMIC_XG_PLL1_POST_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval,
                      CMIC_XG_PLL2_POST_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval,
                      CMIC_XG_PLL3_POST_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, rval);

    sal_usleep(to_usec);

    /*
     * Bring port blocks out of reset
     * PG0: PORT_GROUP5_X, XLPORT0, XLPORT1, XLPORT2, XLPORT3, XLPORT4
     * PG1: PORT_GROUP4_X, XLPORT5, XLPORT6, XLPORT7, XLPORT8
     * PG2: PORT_GROUP5_Y, XLPORT9, XLPORT10, XLPORT11, XLPORT12, XLPORT13
     * PG3: PORT_GROUP4_Y, XLPORT14, XLPORT15, XLPORT16, XLPORT17
     */
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval, CMIC_PG0_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval, CMIC_PG1_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval, CMIC_PG2_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval, CMIC_PG3_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval,
                      CMIC_TEMP_MON_PEAK_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, rval);

    sal_usleep(to_usec);

    /* Bring network sync out of reset */
    rval = 0;
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REG_2r, &rval, XQ0_HOTSWAP_RST_Lf,
                      1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REG_2r, &rval, XQ1_HOTSWAP_RST_Lf,
                      1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REG_2r, &rval, XQ2_HOTSWAP_RST_Lf,
                      1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REG_2r, &rval, XQ3_HOTSWAP_RST_Lf,
                      1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REG_2r, &rval, XQ4_HOTSWAP_RST_Lf,
                      1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REG_2r, &rval, XQ5_HOTSWAP_RST_Lf,
                      1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REG_2r, &rval, NS_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REG_2r(unit, rval);

    sal_usleep(to_usec);

    /*
     * SBUS ring and block number:
     * ring 0: IP(1)
     * ring 1: EP(2)
     * ring 2: MMU(3)
     * ring 3: PORT_GROUP4_X(6), PORT_GROUP5_X(8), XLPORT0(10)...XLPORT8(18),
     *         LBPORT0(28)
     * ring 4: PORT_GROUP4_Y(7), PORT_GROUP5_Y(9), XLPORT9(19)...XLPORT17(27)
     * ring 5: OTPC(4)
     */
    WRITE_CMIC_SBUS_RING_MAP_0r(unit, 0x43052100); /* block 7  - 0 */
    WRITE_CMIC_SBUS_RING_MAP_1r(unit, 0x33333343); /* block 15 - 8 */
    WRITE_CMIC_SBUS_RING_MAP_2r(unit, 0x44444333); /* block 23 - 16 */
    WRITE_CMIC_SBUS_RING_MAP_3r(unit, 0x00034444); /* block 31 - 24 */
    /* 0x7d0 => 0x13500. There is  soc_schan_op operation timed out if less than 0x13500 */
    WRITE_CMIC_SBUS_TIMEOUTr(unit, 0x13500);

    /* Bring IP, EP, and MMU blocks out of reset */
    READ_CMIC_SOFT_RESET_REGr(unit, &rval);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval, CMIC_EP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval, CMIC_IP_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval, CMIC_MMU_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, rval);

    sal_usleep(to_usec);

    READ_CMIC_THERMAL_MON_CTRLr(unit, &rval);
    soc_reg_field_set(unit, CMIC_THERMAL_MON_CTRLr, &rval, BG_ADJf, 1);
    soc_reg_field_set(unit, CMIC_THERMAL_MON_CTRLr, &rval, VTMON_RSTBf, 1);
    WRITE_CMIC_THERMAL_MON_CTRLr(unit, rval);

    sal_usleep(1000);

    READ_CMIC_SOFT_RESET_REGr(unit, &rval);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval,
                      CMIC_TEMP_MON_PEAK_RST_Lf, 0);
    WRITE_CMIC_SOFT_RESET_REGr(unit, rval);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval,
                      CMIC_TEMP_MON_PEAK_RST_Lf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, rval);

    if (!(drv_rev_id == BCM56840_A0_REV_ID && rev_id < BCM56840_B0_REV_ID)) {
        READ_CMIC_CORE_PLL3_CTRL_STATUS_REGISTER_3r(unit, &rval);
        soc_reg_field_set(unit, CMIC_CORE_PLL3_CTRL_STATUS_REGISTER_3r, &rval,
                          PVTMON_RESET_Nf, 1);
        WRITE_CMIC_CORE_PLL3_CTRL_STATUS_REGISTER_3r(unit, rval);
    }

    if (dev_id == BCM56846_DEVICE_ID || dev_id == BCM56844_DEVICE_ID ||
        dev_id == BCM56842_DEVICE_ID || dev_id == BCM56746_DEVICE_ID ||
        dev_id == BCM56744_DEVICE_ID || dev_id == BCM56549_DEVICE_ID ||
        dev_id == BCM56053_DEVICE_ID || dev_id == BCM56838_DEVICE_ID ||
        dev_id == BCM56831_DEVICE_ID || dev_id == BCM56835_DEVICE_ID ||
        dev_id == BCM56847_DEVICE_ID || dev_id == BCM56849_DEVICE_ID ||
        dev_id == BCM56742_DEVICE_ID) {

        rval = 0;
        soc_reg_field_set(unit, ING_CONFIG_2r, &rval,
                          USE_VLAN_ING_PORT_BITMAPf, 1);
        soc_reg_field_set(unit, ING_CONFIG_2r, &rval, FCOE_IFP_KEY_MODEf, 1);
        soc_reg_field_set(unit, ING_CONFIG_2r, &rval,
                          PROTOCOL_PKT_INDEX_PRECEDENCE_MODEf, 1);
        soc_reg_field_set(unit, ING_CONFIG_2r, &rval, WESP_DRAFT_11f, 1);
        soc_reg_field_set(unit, ING_CONFIG_2r, &rval, ECMP_HASH_16BITSf, 1);
        soc_reg_field_set(unit, ING_CONFIG_2r, &rval,
                          TRILL_TRANSIT_MTU_CHECK_ENHANCEDf, 1);
        soc_reg_field_set(unit, ING_CONFIG_2r, &rval, L2_IS_IS_PARSE_MODEf, 1);
        soc_reg_field_set(unit, ING_CONFIG_2r, &rval,
                          DISABLE_INVALID_RBRIDGE_NICKNAMESf, 1);
        soc_reg_field_set(unit, ING_CONFIG_2r, &rval,
                          TRILL_ALL_ESADI_PARSE_MODEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_ING_CONFIG_2r(unit, rval));

        rval = 0;
        soc_reg_field_set(unit, EGR_CONFIG_2r, &rval, DO_NOT_MOD_TPIDf, 1);
        soc_reg_field_set(unit, EGR_CONFIG_2r, &rval,
                          QCN_CNM_MESSAGE_DRAFT_24f, 1);
        soc_reg_field_set(unit, EGR_CONFIG_2r, &rval,
                          USE_RBRIDGES_NICKNAMES_TABLEf, 1);
        soc_reg_field_set(unit, EGR_CONFIG_2r, &rval, WESP_DRAFT_11f, 1);
        soc_reg_field_set(unit, EGR_CONFIG_2r, &rval,
                          TRILL_L2_IS_IS_PARSE_MODEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_EGR_CONFIG_2r(unit, rval));

        rval = 0;
        soc_reg_field_set(unit, ING_Q_BEGINr, &rval, SW_ENC_DEC_TCAM_KEY_MASKf,
                          1);
        soc_reg_field_set(unit, ING_Q_BEGINr, &rval, SUPPORT_8K_VPf, 1);
        soc_reg_field_set(unit, ING_Q_BEGINr, &rval, DISABLE_CELL_COMPRESSIONf,
                          1);
        SOC_IF_ERROR_RETURN(WRITE_ING_Q_BEGINr(unit, rval));

        rval = 0;
        soc_reg_field_set(unit, EGR_Q_BEGINr, &rval, SW_ENC_DEC_TCAM_KEY_MASKf,
                          1);
        SOC_IF_ERROR_RETURN(WRITE_EGR_Q_BEGINr(unit, rval));
    }
    return SOC_E_NONE;
}
#endif  /* BCM_TRIDENT_SUPPORT */

#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HELIX4_SUPPORT)
/*
 * Function:
 *      soc_amac_init
 * Purpose:
 *      Bring iproc amac out of reset.
 *      Common function for Helix4 and Katana2 devices
 */
STATIC int
soc_amac_init(int unit)
{
    uint32 rval = 0;
    uint32 reg_index = 0;
    soc_reg_t amac_idm_reg[2]={ AMAC_IDM0_IDM_RESET_CONTROLr,
                                AMAC_IDM1_IDM_RESET_CONTROLr };
    soc_reg_t gmac_serdes_reg[2]={ GMAC0_SERDESCONTROLr,
                                   GMAC1_SERDESCONTROLr };

    for (reg_index = 0;  reg_index < COUNTOF(amac_idm_reg); reg_index++) {
         SOC_IF_ERROR_RETURN(soc_iproc_getreg(unit,
                             soc_reg_addr(unit, amac_idm_reg[reg_index],
                             REG_PORT_ANY, 0), &rval));
         if (soc_reg_field_get(unit, amac_idm_reg[reg_index],
                               rval, RESETf) != 0) {
             soc_reg_field_set(unit, AMAC_IDM0_IDM_RESET_CONTROLr,
                               &rval, RESETf, 0);
         }
         SOC_IF_ERROR_RETURN(soc_iproc_setreg(unit,
                             soc_reg_addr(unit, amac_idm_reg[reg_index],
                             REG_PORT_ANY, 0), rval));

         SOC_IF_ERROR_RETURN(soc_iproc_getreg(unit, soc_reg_addr(unit,
                             gmac_serdes_reg[reg_index],
                             REG_PORT_ANY, 0), &rval));
         if (soc_reg_field_get(unit, gmac_serdes_reg[reg_index],
                               rval, REF_TERM_SELf) == 0) {
             soc_reg_field_set(unit, gmac_serdes_reg[reg_index],
                               &rval, REF_TERM_SELf, 1);
         }
#ifdef BCM_HELIX4_SUPPORT
         if (soc_reg_field_get(unit, gmac_serdes_reg[reg_index],
                               rval, REFSELf) == 0) {
             if (SOC_IS_HELIX4(unit)) {
                 soc_reg_field_set(unit, GMAC0_SERDESCONTROLr,
                                   &rval, REFSELf, 1);
             }

         }
#endif
         SOC_IF_ERROR_RETURN(soc_iproc_setreg(unit, soc_reg_addr(unit,
                             gmac_serdes_reg[reg_index],
                             REG_PORT_ANY, 0), rval));
    }
    return SOC_E_NONE;
}
#endif

#ifdef BCM_HELIX4_SUPPORT

/*
 * Function:
 *      soc_reset_bcm56340_a0
 * Purpose:
 *      Special reset sequencing for BCM56340
 *      Setup SBUS block mapping.
 */
STATIC int
soc_reset_bcm56340_a0(int unit)
{
    int rv, val, index;
    uint32 rval, to_usec;
    soc_persist_t *sop = SOC_PERSIST(unit);
    uint16 dev_id;
    uint8 rev_id;
    static const soc_reg_t pll_ctrl0_reg[] = {
        TOP_XG_PLL0_CTRL_0r, TOP_XG_PLL1_CTRL_0r,
        TOP_XG_PLL2_CTRL_0r, TOP_XG_PLL3_CTRL_0r
    };
    static const soc_reg_t pll_ctrl4_reg[] = {
        TOP_XG_PLL0_CTRL_4r, TOP_XG_PLL1_CTRL_4r
    };

    uint16 soc_dev;
    uint8 soc_rev;
    unsigned ts_ref_freq;
    unsigned ts_idx;
    static const soc_pll_param_t ts_pll[] = {  /* values for 500MHz TSPLL output */
        /*     Fref,  Ndiv_int, Ndiv_frac,  Pdiv, Mdiv, Ka, Ki, Kp, VCO_div2 */
        {12800000,       312,   0x80000,     1,    8,  1,  2,  7,        1},
        {20000000,       200,         0,     1,    8,  2,  3,  8,        1},
        {25000000,       160,         0,     1,    8,  2,  4,  9,        1},
        {32000000,       125,         0,     1,    8,  2,  4,  9,        1},
        {50000000,        80,         0,     1,    8,  3,  4, 10,        1},
        {       0,       160,         0,     1,    8,  2,  4,  9,        1}  /* 25MHz, from internal reference */
    };
    unsigned bs_ref_freq;
    unsigned bs_idx;
    static const soc_pll_param_t bs_pll[] = {  /* values for 20MHz BSPLL output */
        /*     Fref,  Ndiv_int,  Ndiv_frac,  Pdiv, Mdiv, Ka, Ki, Kp, VCO_div2 */
        {12800000,       250,          0,     1,  160,  1,  2,  7,        0},
        {20000000,       175,          0,     1,  175,  2,  3,  8,        0},
        {25000000,       140,          0,     1,  175,  2,  4,  9,        0},
        {32000000,       108, 0xc0000000,     1,  174,  2,  4,  9,        0}, /* Ndiv=108.75 */
        {50000000,        70,          0,     1,  175,  3,  4, 10,        0},
        {       0,       140,          0,     1,  175,  2,  4,  9,        0}  /* 25MHz, from internal reference */
    };

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (100 * MILLISECOND_USEC);

    /*
     * SBUS ring and block number:
     * ring 0: IP(1)
     * ring 1: EP(2)
     * ring 2: MMU(3)
     * ring 3: ISM(16)
     * ring 4: SER(17)
     * ring 5: AXP(4)
     * ring 6: XTP0(8)..XTP3(11), XLP0(12), IBOD(18),
               XWP0(13).., XWP2(15)
     * ring 7: OTPC(5), TOP_REGS(6)
     */
    WRITE_CMIC_SBUS_RING_MAP_0_7r(unit, 0x07752100);   /* block 7  - 0 */
    WRITE_CMIC_SBUS_RING_MAP_8_15r(unit, 0x66666666);  /* block 15 - 8 */
    WRITE_CMIC_SBUS_RING_MAP_16_23r(unit, 0x00000643); /* block 23 - 16 */
    WRITE_CMIC_SBUS_RING_MAP_24_31r(unit, 0x00000000); /* block 31 - 24 */


    /* Reconfig XMAC cclk - necessary for Helix4 XL port XMAC FIFO UR */
    soc_cm_get_id(unit, &soc_dev, &soc_rev);
    if ( (BCM56340_DEVICE_ID == soc_dev) || (BCM56041_DEVICE_ID == soc_dev) ||
         (BCM56042_DEVICE_ID == soc_dev) || (BCM56345_DEVICE_ID == soc_dev)) {
        if (SOC_E_NONE != (rv = _soc_hx4_xmac_cclk_config(unit))) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Helix4 XMAC cclk config failed !!\n")));
            return rv;
        }
    }

    if (soc_property_get(unit, spn_XGXS_LCPLL_XTAL_REFCLK, 1)) {
        rval = 0x14;    /* 156.25Mhz */
    } else {
        rval = 0x7d;    /* 25Mhz */
    }
    for (index = 0; index < 2; index++) {
        rv = soc_reg_field32_modify(unit, pll_ctrl4_reg[index],
                                    REG_PORT_ANY, NDIV_INTf, rval);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Modifying TOP_XG_PLL%d_CTRL_4 failed !!\n"),
                       index));
            return rv;
        }
    }

    if (soc_property_get(unit, spn_XGXS_QGPLL_XTAL_REFCLK, 1)) {
        rval = 0x14; /* 156.25Mhz */
    } else {
        rval = 0x7d; /* 25Mhz */
    }
    rv = soc_reg_field32_modify(unit, pll_ctrl4_reg[0],
                                REG_PORT_ANY, NDIV_INTf, rval);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Modifying TOP_XG_PLL0_CTRL_4:QG_PLL failed !!\n")));
        return rv;
    }

    if (soc_property_get(unit, spn_XGXS_WCPLL_XTAL_REFCLK, 1)) {
        rval = 0x14; /* 156.25Mhz */
    } else {
        rval = 0x7d; /* 25Mhz */
    }
    rv = soc_reg_field32_modify(unit, pll_ctrl4_reg[1],
                                REG_PORT_ANY, NDIV_INTf, rval);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Modifying TOP_XG_PLL1_CTRL_4:WC_PLL failed !!\n")));
        return rv;
    }

    for (index = 0; index < 4; index++) {
        rv = soc_reg_field32_modify(unit, pll_ctrl0_reg[index],
                                    REG_PORT_ANY, CH3_MDIVf, 25);
            if (rv < 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Modifying TOP_XG_PLL%d_CTRL_0 failed !!\n"),
                           index));
                return rv;
            }
    }
    /* turn mac_core_clk to 125MHz*/
    rv = soc_reg_field32_modify(unit, pll_ctrl0_reg[0],
            REG_PORT_ANY, CH4_MDIVf, 25);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Modifying TOP_XG_PLL0_CTRL_0 failed !!\n")));
        return rv;
    }

    rv = READ_TOP_MISC_CONTROL_1r(unit, &rval);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Reading TOP_MISC_CONTROL_1 failed !!\n")));
        return rv;
    }
    soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval,
                      CMIC_TO_XG_PLL0_SW_OVWRf, 1);
    soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval,
                      CMIC_TO_XG_PLL1_SW_OVWRf, 1);
    soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval,
                      CMIC_TO_XG_PLL2_SW_OVWRf, 1);
    soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval,
                      CMIC_TO_XG_PLL3_SW_OVWRf, 1);
    soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval,
                      IO_HYS_EN_CTRLf, 1);
    rv = WRITE_TOP_MISC_CONTROL_1r(unit, rval);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Writing TOP_MISC_CONTROL_1 failed !!\n")));
        return rv;
    }

    /* Bring LCPLL out of reset */
    rv = READ_TOP_SOFT_RESET_REG_2r(unit, &rval);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Reading TOP_SOFT_RESET_REG_2 failed !!\n")));
        return rv;
    }
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL0_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL1_RST_Lf, 1);
    rv = WRITE_TOP_SOFT_RESET_REG_2r(unit, rval);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Writing TOP_SOFT_RESET_REG_2 failed !!\n")));
        return rv;
    }
    sal_usleep(to_usec);

    /* Wait for LCPLL lock */
    if (!SAL_BOOT_SIMULATION) {
        rv = READ_TOP_XG_PLL0_STATUSr(unit, &rval);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Reading TOP_XG_PLL0_STATUS failed !!\n")));
            return rv;
        }
        if (!soc_reg_field_get(unit, TOP_XG_PLL0_STATUSr, rval, TOP_XGPLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "LCPLL 0 not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }
        rv = READ_TOP_XG_PLL1_STATUSr(unit, &rval);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Reading TOP_XG_PLL1_STATUS failed !!\n")));
            return rv;
        }
        if (!soc_reg_field_get(unit, TOP_XG_PLL1_STATUSr, rval, TOP_XGPLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "LCPLL 1 not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }
    }

    /* De-assert LCPLL's post reset */
    rv = READ_TOP_SOFT_RESET_REG_2r(unit, &rval);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Reading TOP_SOFT_RESET_REG_2 failed !!\n")));
        return rv;
    }
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL0_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL1_POST_RST_Lf, 1);
    rv = WRITE_TOP_SOFT_RESET_REG_2r(unit, rval);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Writing TOP_SOFT_RESET_REG_2 failed !!\n")));
        return rv;
    }
    sal_usleep(to_usec);


    /* Configure TS PLL. Default of 0 is internal reference */
    ts_ref_freq = soc_property_get(unit, spn_PTP_TS_PLL_FREF, 0);

    for (ts_idx = 0; ts_idx < sizeof(ts_pll)/sizeof(ts_pll[0]); ++ts_idx) {
        if (ts_pll[ts_idx].ref_freq == ts_ref_freq) {
            break;
        }
    }

    if (ts_idx == sizeof(ts_pll)/sizeof(ts_pll[0])) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Invalid value for PTP_TS_PLL_REF (%u)."
                              "No default PLL params.\n"), ts_ref_freq));
    } else {
        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_2r(unit,&rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_2r, &rval, PDIVf,
                          soc_property_get(unit, spn_PTP_TS_PLL_PDIV,
                                           ts_pll[ts_idx].pdiv));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_2r, &rval, CH0_MDIVf,
                          soc_property_get(unit, spn_PTP_TS_PLL_MNDIV,
                                           ts_pll[ts_idx].mdiv));
        WRITE_TOP_TS_PLL_CTRL_2r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_3r(unit,&rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_3r, &rval, NDIV_INTf,
                          soc_property_get(unit, spn_PTP_TS_PLL_N,
                                           ts_pll[ts_idx].ndiv_int));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_3r, &rval, NDIV_FRACf,
                          ts_pll[ts_idx].ndiv_frac);
        WRITE_TOP_TS_PLL_CTRL_3r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_4r(unit,&rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_4r, &rval, KAf,
                          soc_property_get(unit, spn_PTP_TS_KA,
                                           ts_pll[ts_idx].ka));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_4r, &rval, KIf,
                          soc_property_get(unit, spn_PTP_TS_KI,
                                           ts_pll[ts_idx].ki));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_4r, &rval, KPf,
                          soc_property_get(unit, spn_PTP_TS_KP,
                                           ts_pll[ts_idx].kp));

        /* SOC property == 0 => use internal reference of 25MHz */
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_4r, &rval, REFCLK_SELf,
                          (ts_ref_freq != 0));
        WRITE_TOP_TS_PLL_CTRL_4r(unit, rval);

        /* Strobe channel-0 load-enable to set divisors. */
        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_2r(unit,&rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_2r, &rval, LOAD_EN_CH0f, 0);
        WRITE_TOP_TS_PLL_CTRL_2r(unit, rval);
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_2r, &rval, LOAD_EN_CH0f, 1);
        WRITE_TOP_TS_PLL_CTRL_2r(unit, rval);
    }

    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_TS_PLL_RST_Lf, 1);
    WRITE_TOP_SOFT_RESET_REG_2r(unit, rval);

    /* Check for TSPLL lock */
    if (!SAL_BOOT_SIMULATION) {
        sal_usleep(to_usec);

        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_STATUSr(unit, &rval));
        if (!soc_reg_field_get(unit, TOP_TS_PLL_STATUSr, rval, PLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TimeSync PLL not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }
    }

    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_TS_PLL_POST_RST_Lf, 1);
    WRITE_TOP_SOFT_RESET_REG_2r(unit, rval);

    /* Set 250Mhz (implies 4ns resolution) default timesync clock to
       calculate assymetric delays */
    SOC_TIMESYNC_PLL_CLOCK_NS(unit) = (1/250 * 1000); /* clock period in nanoseconds */

    /* as a signal to upper-level code that the BroadSync is newly initialized
     * disable BroadSync0/1 bitclock output.  Checked in time.c / 1588 firmware    */
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_BS0_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &rval, BS_CLK_OUTPUT_ENABLEf, 0);
    WRITE_CMIC_BS0_CONFIGr(unit, rval);

    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_BS1_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &rval, BS_CLK_OUTPUT_ENABLEf, 0);
    WRITE_CMIC_BS1_CONFIGr(unit, rval);

    /* BSPLL0 has not been configured, so reset/configure both BSPLL0 and BSPLL1 */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL0_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL1_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL0_POST_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL1_POST_RST_Lf, 0);
    WRITE_TOP_SOFT_RESET_REG_2r(unit, rval);

    /* Both BSPLLs configured the same, for 20MHz output by default */
    bs_ref_freq = soc_property_get(unit, spn_PTP_BS_FREF, 0);  /* 0->internal reference */
    /* Set BS_PLL_CLK_IN_SEL based on reference frequency.  If it is 0, use the internal reference */
    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_4r(unit,&rval));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_4r, &rval, REFCLK_SELf,
                      (bs_ref_freq != 0));
    WRITE_TOP_BS_PLL0_CTRL_4r(unit, rval);

    for (bs_idx = 0; bs_idx < sizeof(bs_pll)/sizeof(bs_pll[0]); ++bs_idx) {
        if (bs_pll[bs_idx].ref_freq == bs_ref_freq) {
            break;
        }
    }
    if (bs_idx == sizeof(bs_pll)/sizeof(bs_pll[0])) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Invalid value for PTP_BS_REF (%u)."
                              "No default PLL params.\n"), bs_ref_freq));
    } else {
        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_0r(unit,&rval));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_0r, &rval, VCO_DIV2f,
                          soc_property_get(unit, spn_PTP_BS_VCO_DIV2,
                                           bs_pll[bs_idx].vco_div2));
        WRITE_TOP_BS_PLL0_CTRL_0r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_0r(unit,&rval));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_0r, &rval, VCO_DIV2f,
                          soc_property_get(unit, spn_PTP_BS_VCO_DIV2,
                                           bs_pll[bs_idx].vco_div2));
        WRITE_TOP_BS_PLL1_CTRL_0r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_4r(unit,&rval));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_4r, &rval, KAf,
                          soc_property_get(unit, spn_PTP_BS_KA,
                                           bs_pll[bs_idx].ka));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_4r, &rval, KIf,
                          soc_property_get(unit, spn_PTP_BS_KI,
                                           bs_pll[bs_idx].ki));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_4r, &rval, KPf,
                          soc_property_get(unit, spn_PTP_BS_KP,
                                           bs_pll[bs_idx].kp));
        WRITE_TOP_BS_PLL0_CTRL_4r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_4r(unit,&rval));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_4r, &rval, KAf,
                          soc_property_get(unit, spn_PTP_BS_KA,
                                           bs_pll[bs_idx].ka));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_4r, &rval, KIf,
                          soc_property_get(unit, spn_PTP_BS_KI,
                                           bs_pll[bs_idx].ki));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_4r, &rval, KPf,
                          soc_property_get(unit, spn_PTP_BS_KP,
                                           bs_pll[bs_idx].kp));
        WRITE_TOP_BS_PLL1_CTRL_4r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_3r(unit,&rval));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_3r, &rval, NDIV_INTf,
                          soc_property_get(unit, spn_PTP_BS_NDIV_INT,
                                           bs_pll[bs_idx].ndiv_int));
        WRITE_TOP_BS_PLL0_CTRL_3r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_3r(unit,&rval));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_3r, &rval, NDIV_INTf,
                          soc_property_get(unit, spn_PTP_BS_NDIV_INT,
                                           bs_pll[bs_idx].ndiv_int));
        WRITE_TOP_BS_PLL1_CTRL_3r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_2r(unit,&rval));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_2r, &rval, CH0_MDIVf,
                          soc_property_get(unit, spn_PTP_BS_MNDIV,
                                           bs_pll[bs_idx].mdiv));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_2r, &rval, PDIVf,
                          soc_property_get(unit, spn_PTP_BS_PDIV,
                                           bs_pll[bs_idx].pdiv));
        WRITE_TOP_BS_PLL0_CTRL_2r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_2r(unit,&rval));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_2r, &rval, CH0_MDIVf,
                          soc_property_get(unit, spn_PTP_BS_MNDIV,
                                           bs_pll[bs_idx].mdiv));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_2r, &rval, PDIVf,
                          soc_property_get(unit, spn_PTP_BS_PDIV,
                                           bs_pll[bs_idx].pdiv));
        WRITE_TOP_BS_PLL1_CTRL_2r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_2r(unit,&rval));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_2r, &rval, LOAD_EN_CH0f, 0);
        WRITE_TOP_BS_PLL0_CTRL_2r(unit, rval);
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_2r, &rval, LOAD_EN_CH0f, 1);
        WRITE_TOP_BS_PLL0_CTRL_2r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_2r(unit,&rval));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_2r, &rval, LOAD_EN_CH0f, 0);
        WRITE_TOP_BS_PLL1_CTRL_2r(unit, rval);
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_2r, &rval, LOAD_EN_CH0f, 1);
        WRITE_TOP_BS_PLL1_CTRL_2r(unit, rval);
    }

    /* Take BroadSync out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL0_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL1_RST_Lf, 1);
    WRITE_TOP_SOFT_RESET_REG_2r(unit, rval);

    /* Check for BSPLL lock */
    if (!SAL_BOOT_SIMULATION) {
        sal_usleep(to_usec);
        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_STATUSr(unit, &rval));
        if (!soc_reg_field_get(unit, TOP_BS_PLL0_STATUSr, rval, PLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "BroadSync0 PLL not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }
        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_STATUSr(unit, &rval));
        if (!soc_reg_field_get(unit, TOP_BS_PLL1_STATUSr, rval, PLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "BroadSync1 PLL not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }
    }

    /* Deassert Post-Reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL0_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL1_POST_RST_Lf, 1);
    WRITE_TOP_SOFT_RESET_REG_2r(unit, rval);

    soc_cm_get_id(unit, &dev_id, &rev_id);
    val = soc_property_get(unit, "iproc_shutdown", 0);
    if (val) {
        uint32 cpu_mask = 0x1;
        /* Spiral SKUs have a single core, all other SKUs have dual cores */
        if (!(dev_id == BCM56047_DEVICE_ID || dev_id == BCM56048_DEVICE_ID ||
              dev_id == BCM56049_DEVICE_ID)) {
            cpu_mask |= 0x2;
        }
        rv = soc_iproc_shutdown(unit, cpu_mask, val-1);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Putting iproc in shutdown failed !!\n")));
        }
    }

    /* Check for M, A and Plain versions */
    SOC_IF_ERROR_RETURN(READ_TOP_SWITCH_FEATURE_ENABLEr(unit, &rval));
    if (rval & 0x1) {
        /* BOND_1588_ENABLE */
        SOC_FEATURE_SET(unit, soc_feature_timesync_support);
    }
    if (!(rval & 0x8)) {
        /* BOND_MPLS_ENABLE */
        SOC_FEATURE_CLEAR(unit, soc_feature_mpls);
        SOC_FEATURE_CLEAR(unit, soc_feature_bhh);
    }
    if (!(rval & 0x10)) {
        /* BOND_IP_ENABLE */
        SOC_FEATURE_CLEAR(unit, soc_feature_lpm_tcam);
        SOC_FEATURE_CLEAR(unit, soc_feature_ip_mcast);
        SOC_FEATURE_CLEAR(unit, soc_feature_ip_mcast_repl);
        SOC_FEATURE_CLEAR(unit, soc_feature_l3);
        SOC_FEATURE_CLEAR(unit, soc_feature_l3_ip6);
        SOC_FEATURE_CLEAR(unit, soc_feature_l3_lookup_cmd);
        SOC_FEATURE_CLEAR(unit, soc_feature_l3_sgv);
        /* Following features depend on IP feature */
        SOC_FEATURE_CLEAR(unit, soc_feature_subport);
        SOC_FEATURE_CLEAR(unit, soc_feature_subport_enhanced);
        SOC_FEATURE_CLEAR(unit, soc_feature_trill);
        SOC_FEATURE_CLEAR(unit, soc_feature_l2gre);
    }
    /* Adjust memory sizes for the A variants - addendum to soc_hx4_mem_config */
    if (!(rval & 0x10)) {
        /* BOND_IP_ENABLE */
        switch (dev_id) {
            case BCM56346_DEVICE_ID:
            case BCM56345_DEVICE_ID:
            case BCM56344_DEVICE_ID:
            case BCM56342_DEVICE_ID:
            case BCM56340_DEVICE_ID:
                sop->memState[MPLS_ENTRYm].index_max = -1;
                sop->memState[MPLS_ENTRY_1m].index_max = -1;
                sop->memState[MPLS_ENTRY_EXTDm].index_max = -1;
                sop->memState[L3_ENTRY_1m].index_max = -1;
                sop->memState[L3_ENTRY_2m].index_max = -1;
                sop->memState[L3_ENTRY_4m].index_max = -1;
                sop->memState[EGR_IPMCm].index_max = -1;
                sop->memState[EGR_L3_NEXT_HOPm].index_max = -1;
                sop->memState[ING_L3_NEXT_HOPm].index_max = -1;
                sop->memState[INITIAL_ING_L3_NEXT_HOPm].index_max = -1;
                sop->memState[INITIAL_PROT_NHI_TABLEm].index_max = -1;
                sop->memState[L3_IPMCm].index_max = 1023;
                sop->memState[L3_IPMC_1m].index_max = 1023;
                sop->memState[L3_IPMC_REMAPm].index_max = 1023;
                sop->memState[MMU_REPL_GROUPm].index_max = 1023;
                sop->memState[VRFm].index_max = 127;
                sop->memState[L3_DEFIPm].index_max = -1;
                sop->memState[L3_DEFIP_ONLYm].index_max = -1;
                sop->memState[L3_DEFIP_PAIR_128m].index_max = -1;
                break;
            default:
                break;
        }
    }

    /*
     * Bring port blocks out of reset
     */
    rv = READ_TOP_SOFT_RESET_REGr(unit, &rval);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Reading TOP_SOFT_RESET_REG failed !!\n")));
        return rv;
    }
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_XTP0_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_XTP1_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_XTP2_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_XTP3_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_XLP0_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_XWP0_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_XWP1_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_XWP2_RST_Lf, 1);

    rv = WRITE_TOP_SOFT_RESET_REGr(unit, rval);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Writing TOP_SOFT_RESET_REG failed !!\n")));
        return rv;
    }

    sal_usleep(to_usec);

    /* Bring network sync out of reset */
    rv = READ_TOP_SOFT_RESET_REGr(unit, &rval);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Reading TOP_SOFT_RESET_REG failed !!\n")));
        return rv;
    }
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_NS_RST_Lf, 1);
    rv = WRITE_TOP_SOFT_RESET_REGr(unit, rval);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Writing TOP_SOFT_RESET_REG failed !!\n")));
        return rv;
    }

    sal_usleep(to_usec);

    WRITE_CMIC_SBUS_TIMEOUTr(unit, 0x7d0);

    /* Bring IP, EP, ISM, ETU, AXP, and MMU blocks out of reset */
    rv = READ_TOP_SOFT_RESET_REGr(unit, &rval);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Reading TOP_SOFT_RESET_REG failed !!\n")));
        return rv;
    }
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_EP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_IP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MMU_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_ISM_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_AXP_RST_Lf, 1);
    rv = WRITE_TOP_SOFT_RESET_REGr(unit, rval);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Writing TOP_SOFT_RESET_REG failed !!\n")));
        return rv;
    }

    sal_usleep(to_usec);

    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_RESET_Nf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_RESET_Nf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_RESET_Nf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_TEMP_MON_PEAK_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_TEMP_MON_PEAK_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    
    WRITE_CMIC_SBUS_TIMEOUTr(unit, 0x7A120);

    /* Set MDIO IO PAD voltage */
    if (0 != soc_property_get(unit, spn_MDIO_IO_VOLTAGE, 0)) {
        rv = READ_TOP_MISC_CONTROL_1r(unit, &rval);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Reading TOP_MISC_CONTROL_1 failed !!\n")));
            return rv;
        }
        soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval,
                          XG_MDIO0_CL_SELf, 1);
        rv = WRITE_TOP_MISC_CONTROL_1r(unit, rval);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Writing TOP_MISC_CONTROL_1 failed !!\n")));
            return rv;
        }
    }
    /* Bring AMAC out of reset */
    soc_amac_init(unit);

    return SOC_E_NONE;
}
#endif

#ifdef BCM_KATANA_SUPPORT
/*
 * Function:
 *      soc_reset_bcm56440_a0
 * Purpose:
 *      Special reset sequencing for BCM56440
 *      Setup SBUS block mapping.
 */
STATIC int
soc_reset_bcm56440_a0(int unit)
{
    uint32 rval, to_usec;
    uint16 dev_id;
    uint8 rev_id;

    /* TS PLL settings for 250MHZ output */
    unsigned ts_ref_freq = 0;
    unsigned ts_idx=0;

    /* values for 250 MHz TSPLL output */
    static const soc_pll_param_t katana_ts_pll[] = {
    /*     Fref, Ndiv_int, Ndiv_frac,  Pdiv, Mdiv, Ka, Ki, Kp, VCO_div2 */
      {12800000,      310,         0,     1,   16,  1,  2,  7,        1},
      {20000000,      200,         0,     1,   16,  2,  3,  8,        1},
      {25000000,      160,         0,     1,   16,  2,  4,  9,        1},
      {32000000,      125,         0,     1,   16,  2,  4,  9,        1},
      {       0,      160,         0,     1,   16,  2,  4,  9,        1} /*
      25MHZ from internal reference */
    };

    unsigned bs_ref_freq;
    unsigned bs_idx;

    static const soc_pll_param_t katana_bs_pll[] = { /* values for 20 MHz BSPLL output */
    /*     Fref, Ndiv_int, Ndiv_frac,  Pdiv, Mdiv, Ka, Ki, Kp, VCO_div2 */
      {12800000,      300,         0,     1,  192,  1,  2,  7,        1},
      {20000000,      400,         0,     2,  200,  1,  2,  7,        1},
      {25000000,      320,         0,     2,  200,  1,  2,  7,        1},
      {32000000,      375,         0,     3,  200,  1,  2,  7,        1},
      {       0,      320,         0,     2,  200,  2,  4,  9,        1} /*
      25MHZ from internal reference */
    };

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

    /*
     * SBUS ring and block number:
     * Ring0: cmic -> ip-ep -> cmic
     * Ring1: cmic -> mxqport0 -> mxqport1-> mxqport2 -> mxqport3-> cmic
     * Ring2: cmic -> gport0 -> gport1 -> gport2 -> cmic
     * Ring3: cmic -> otpc -> top -> cmic
     * Ring4: cmic -> mmu0 -> mmu1 -> cmic
     * Ring5: cmic -> ci0 -> ci1 -> ci2 -> lls -> cmic
     * Ring6: cmic -> ces -> cmic
     * Ring7: unused
     */
    WRITE_CMIC_SBUS_RING_MAP_0_7r(unit, 0x22034000); /* block 7  - 0 */
    WRITE_CMIC_SBUS_RING_MAP_8_15r(unit, 0x55311112); /* block 15 - 8 */
    WRITE_CMIC_SBUS_RING_MAP_16_23r(unit, 0x00000655); /* block 23 - 16 */
    WRITE_CMIC_SBUS_RING_MAP_24_31r(unit, 0x00000000); /* block 31 - 24 */



    soc_cm_get_id(unit, &dev_id, &rev_id);

    switch (dev_id) {
        case BCM56440_DEVICE_ID: /* 135 MHz Operation */
        case BCM55440_DEVICE_ID:
        case BCM55441_DEVICE_ID:
        case BCM56445_DEVICE_ID:
        case BCM56448_DEVICE_ID:
        case BCM56449_DEVICE_ID:
            SOC_IF_ERROR_RETURN(READ_TOP_MMU_PLL1_CTRL1r(unit, &rval));
            soc_reg_field_set(unit, TOP_MMU_PLL1_CTRL1r, &rval, KPf, 10);
            SOC_IF_ERROR_RETURN(WRITE_TOP_MMU_PLL1_CTRL1r(unit, rval));
            SOC_IF_ERROR_RETURN(READ_TOP_MMU_PLL2_CTRL1r(unit, &rval));
            soc_reg_field_set(unit, TOP_MMU_PLL2_CTRL1r, &rval, KPf, 10);
            SOC_IF_ERROR_RETURN(WRITE_TOP_MMU_PLL2_CTRL1r(unit, rval));
            SOC_IF_ERROR_RETURN(READ_TOP_MMU_PLL3_CTRL1r(unit, &rval));
            soc_reg_field_set(unit, TOP_MMU_PLL3_CTRL1r, &rval, KPf, 10);
            SOC_IF_ERROR_RETURN(WRITE_TOP_MMU_PLL3_CTRL1r(unit, rval));
            SOC_IF_ERROR_RETURN(READ_TOP_MMU_PLL1_CTRL3r(unit, &rval));
            soc_reg_field_set(unit, TOP_MMU_PLL1_CTRL3r, &rval, PLL_CTRL_VCO_DIV2f, 1);
            SOC_IF_ERROR_RETURN(WRITE_TOP_MMU_PLL1_CTRL3r(unit, rval));
            SOC_IF_ERROR_RETURN(READ_TOP_MMU_PLL2_CTRL3r(unit, &rval));
            soc_reg_field_set(unit, TOP_MMU_PLL2_CTRL3r, &rval, PLL_CTRL_VCO_DIV2f, 1);
            SOC_IF_ERROR_RETURN(WRITE_TOP_MMU_PLL2_CTRL3r(unit, rval));
            SOC_IF_ERROR_RETURN(READ_TOP_MMU_PLL3_CTRL3r(unit, &rval));
            soc_reg_field_set(unit, TOP_MMU_PLL3_CTRL3r, &rval, PLL_CTRL_VCO_DIV2f, 1);
            SOC_IF_ERROR_RETURN(WRITE_TOP_MMU_PLL3_CTRL3r(unit, rval));
            break;
        default: /* 90 MHz Operation */
            SOC_IF_ERROR_RETURN(READ_TOP_MMU_PLL1_CTRL3r(unit, &rval));
            soc_reg_field_set(unit, TOP_MMU_PLL1_CTRL3r, &rval, PLL_CTRL_PWM_RATEf, 0);
            SOC_IF_ERROR_RETURN(WRITE_TOP_MMU_PLL1_CTRL3r(unit, rval));
            SOC_IF_ERROR_RETURN(READ_TOP_MMU_PLL2_CTRL3r(unit, &rval));
            soc_reg_field_set(unit, TOP_MMU_PLL2_CTRL3r, &rval, PLL_CTRL_PWM_RATEf, 0);
            SOC_IF_ERROR_RETURN(WRITE_TOP_MMU_PLL2_CTRL3r(unit, rval));
            SOC_IF_ERROR_RETURN(READ_TOP_MMU_PLL3_CTRL3r(unit, &rval));
            soc_reg_field_set(unit, TOP_MMU_PLL3_CTRL3r, &rval, PLL_CTRL_PWM_RATEf, 0);
            SOC_IF_ERROR_RETURN(WRITE_TOP_MMU_PLL3_CTRL3r(unit, rval));
            break;
    }

    SOC_IF_ERROR_RETURN(READ_TOP_SWITCH_FEATURE_ENABLE_2r(unit, &rval));
    if (soc_reg_field_get(unit, TOP_SWITCH_FEATURE_ENABLE_2r, rval, BOND_1588_ENABLEf)) {
        SOC_FEATURE_SET(unit, soc_feature_timesync_support);
    }

    if (soc_reg_field_get(unit, TOP_SWITCH_FEATURE_ENABLE_2r, rval,
                          BOND_CES_ENABLEf)) {
        if ((rev_id != BCM56440_A0_REV_ID) &&
            (soc_property_get(unit, spn_CES_DISABLE, 0))) {
            /* Disable CES */
            SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_2r(unit, &rval));
            soc_reg_field_set(unit, TOP_MISC_CONTROL_2r, &rval,
                              BOND_CES_ENABLE_OVERRIDEf, 1);
            WRITE_TOP_MISC_CONTROL_2r(unit, rval);
        } else {
#if defined(INCLUDE_CES)
            SOC_FEATURE_SET(unit, soc_feature_ces);
#else
            if (rev_id != BCM56440_A0_REV_ID) {
                /* Disable CES */
                SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_2r(unit, &rval));
                soc_reg_field_set(unit, TOP_MISC_CONTROL_2r, &rval,
                                  BOND_CES_ENABLE_OVERRIDEf, 1);
                WRITE_TOP_MISC_CONTROL_2r(unit, rval);
            }
#endif
        }
    }

    /* Bring LCPLL out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG0_PLL_RST_Lf,
                      1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG1_PLL_RST_Lf,
                      1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    sal_usleep(to_usec);

    /* Wait for LCPLL lock */
    if (!SAL_BOOT_SIMULATION) {
        SOC_IF_ERROR_RETURN(READ_TOP_XGXS0_PLL_STATUSr(unit, &rval));

        if (!soc_reg_field_get(unit, TOP_XGXS0_PLL_STATUSr, rval, TOP_XGPLL0_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "LCPLL 0 not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }
        if ((dev_id != BCM56240_DEVICE_ID) && (dev_id != BCM56241_DEVICE_ID) &&
            (dev_id != BCM56242_DEVICE_ID) && (dev_id != BCM56243_DEVICE_ID) &&
            (dev_id != BCM56245_DEVICE_ID) && (dev_id != BCM56246_DEVICE_ID) &&
            (dev_id != BCM56449_DEVICE_ID)) {
            /* Variants that don't use LCPLL-1 */
            SOC_IF_ERROR_RETURN(READ_TOP_XGXS1_PLL_STATUSr(unit, &rval));
            if (!soc_reg_field_get(unit, TOP_XGXS1_PLL_STATUSr, rval, TOP_XGPLL1_LOCKf)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "LCPLL 1 not locked on unit %d "
                                      "status = 0x%08x\n"), unit, rval));
            }
         }
    }

    /* KT-1367: Timesync PLL 250MHz TS_CLK from 25MHz refclk */
    /* NDIV= 100(0x64) MDIV=10(0x1)            */

    /* TSPLL configuration: 250MHz TS_CLK from 12.8/20/25/32MHz refclk */

    ts_ref_freq =  soc_property_get(unit, spn_PTP_TS_PLL_FREF, 0);

    for (ts_idx =0; ts_idx < (sizeof(katana_ts_pll)/sizeof(katana_ts_pll[0]));
        ++ts_idx) {
        if (katana_ts_pll[ts_idx].ref_freq == ts_ref_freq) {
            break;
        };
    }

    if (ts_idx >= (sizeof(katana_ts_pll)/sizeof(katana_ts_pll[0]))) {
        ts_idx = 4; /* wrong config, for now use internal 25MHZ reference */
        ts_ref_freq = 0;
    }


    /* Configure TS PLL */
    /* Set TS_PLL_CLK_IN_SEL based on reference frequency.  If it is 0, use the
     * internal reference */
    SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_2r(unit,&rval));
    soc_reg_field_set(unit, TOP_MISC_CONTROL_2r, &rval, TS_PLL_CLK_IN_SELf,
                      (ts_ref_freq != 0));
    WRITE_TOP_MISC_CONTROL_2r(unit, rval);

    SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_0r(unit,&rval));
    soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_0r, &rval, VCO_DIV2f,
                      1);  /* soc_property_get(unit, spn_TIME_SYNC_VCO_DIV2, 0)); */
    WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_0r(unit, rval);

    SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_1r(unit,&rval));
    soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_1r, &rval, KAf,
                      soc_property_get(unit, spn_PTP_TS_KA, katana_ts_pll[ts_idx].ka));
    soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_1r, &rval, KIf,
                      soc_property_get(unit, spn_PTP_TS_KI, katana_ts_pll[ts_idx].ki));
    soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_1r, &rval, KPf,
                      soc_property_get(unit, spn_PTP_TS_KP, katana_ts_pll[ts_idx].kp));
    WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_1r(unit, rval);

    SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r(unit,&rval));
    soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r, &rval, PDIVf,
                      soc_property_get(unit, spn_PTP_TS_PLL_PDIV,
                      katana_ts_pll[ts_idx].pdiv));
    WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r(unit, rval);
    SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_3r(unit,&rval));

    soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_3r, &rval, NDIV_INTf,
                      soc_property_get(unit, spn_PTP_TS_PLL_N,
                      katana_ts_pll[ts_idx].ndiv_int));
    soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_3r, &rval, NDIV_FRACf,
                      katana_ts_pll[ts_idx].ndiv_frac);
    WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_3r(unit, rval);

    SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_4r(unit,&rval));
    soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_4r, &rval, CH0_MDIVf,
                      soc_property_get(unit, spn_PTP_TS_PLL_MNDIV,
                      katana_ts_pll[ts_idx].mdiv));
    WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_4r(unit, rval);

    /* 250Mhz TS PLL implies 4ns resolution */
     SOC_TIMESYNC_PLL_CLOCK_NS(unit) = (1/250 * 1000); /* clock period in nanoseconds */

    /* Configure BS PLL */
    /* Note: Using 0 to signify internal reference for BroadSync PLL.
    Earlier, 25000000 is used for the same, where we can't distinguish between
    external 25MHz and internal reference */
    bs_ref_freq =  soc_property_get(unit, spn_PTP_BS_FREF, 0);

    /* BSPLL configuration: 20MHz BS_CLK from 12.8/20/25/32MHz Fref or Internal ref:0*/
    for (bs_idx =0; bs_idx < (sizeof(katana_bs_pll)/sizeof(katana_bs_pll[0]));
        ++bs_idx) {
        if (katana_bs_pll[bs_idx].ref_freq == bs_ref_freq) {
            break;
        };
    }

    /* If the property spn_PTP_BS_FREF is set to a value other than
       internal reference(0) or the permitted external fref set(i.e., 12.8/20/25/32MHz),
       configure the BS PLL for internal reference i.e., use the last table entry */
    if (bs_idx >= (sizeof(katana_bs_pll)/sizeof(katana_bs_pll[0]))) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
            (BSL_META_U(unit,
                "Invalid value for PTP_BS_FREF (%u).  Using default PLL params.\n"), bs_ref_freq));

        bs_idx = (sizeof(katana_bs_pll)/sizeof(katana_bs_pll[0]))-1;
        bs_ref_freq = 0;
    }

    SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_2r(unit,&rval));
    soc_reg_field_set(unit, TOP_MISC_CONTROL_2r, &rval, BS_PLL_CLK_IN_SELf,
                          (bs_ref_freq!=0));
    WRITE_TOP_MISC_CONTROL_2r(unit, rval);

    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC_PLL_CTRL_REGISTER_0r(unit,&rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC_PLL_CTRL_REGISTER_0r, &rval, VCO_DIV2f,
                      soc_property_get(unit, spn_PTP_BS_VCO_DIV2, katana_bs_pll[bs_idx].vco_div2));
    WRITE_TOP_BROAD_SYNC_PLL_CTRL_REGISTER_0r(unit, rval);

    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC_PLL_CTRL_REGISTER_1r(unit,&rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC_PLL_CTRL_REGISTER_1r, &rval, KAf,
                      soc_property_get(unit, spn_PTP_BS_KA, katana_bs_pll[bs_idx].ka));
    soc_reg_field_set(unit, TOP_BROAD_SYNC_PLL_CTRL_REGISTER_1r, &rval, KIf,
                      soc_property_get(unit, spn_PTP_BS_KI, katana_bs_pll[bs_idx].ki));
    soc_reg_field_set(unit, TOP_BROAD_SYNC_PLL_CTRL_REGISTER_1r, &rval, KPf,
                      soc_property_get(unit, spn_PTP_BS_KP, katana_bs_pll[bs_idx].kp));
    WRITE_TOP_BROAD_SYNC_PLL_CTRL_REGISTER_1r(unit, rval);

    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC_PLL_CTRL_REGISTER_2r(unit,&rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC_PLL_CTRL_REGISTER_2r, &rval, PDIVf,
                      soc_property_get(unit, spn_PTP_BS_PDIV, katana_bs_pll[bs_idx].pdiv));
    WRITE_TOP_BROAD_SYNC_PLL_CTRL_REGISTER_2r(unit, rval);

    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC_PLL_CTRL_REGISTER_3r(unit,&rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC_PLL_CTRL_REGISTER_3r, &rval, NDIV_INTf,
                      soc_property_get(unit, spn_PTP_BS_NDIV_INT, katana_bs_pll[bs_idx].ndiv_int));
    soc_reg_field_set(unit, TOP_BROAD_SYNC_PLL_CTRL_REGISTER_3r, &rval, NDIV_FRACf,
                      soc_property_get(unit, spn_PTP_BS_NDIV_FRAC, katana_bs_pll[bs_idx].ndiv_frac));
    WRITE_TOP_BROAD_SYNC_PLL_CTRL_REGISTER_3r(unit, rval);

    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC_PLL_CTRL_REGISTER_4r(unit,&rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC_PLL_CTRL_REGISTER_4r, &rval, CH0_MDIVf,
                      soc_property_get(unit, spn_PTP_BS_MNDIV, katana_bs_pll[bs_idx].mdiv));
    WRITE_TOP_BROAD_SYNC_PLL_CTRL_REGISTER_4r(unit, rval);

    /* CES SETUP for external OCXO */
    if (soc_property_get(unit, spn_CES_PLL_REFERENCE_CLOCK_RATE, 25000000)==
    12800000){

      /* Configure CES PLL */
      SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_2r(unit,&rval));
      soc_reg_field_set(unit, TOP_MISC_CONTROL_2r, &rval, CES_PLL_CLK_IN_SELf,
            1);
      WRITE_TOP_MISC_CONTROL_2r(unit, rval);

      SOC_IF_ERROR_RETURN(READ_TOP_CES_PLL_CTRL_REGISTER_0r(unit,&rval));
      soc_reg_field_set(unit, TOP_CES_PLL_CTRL_REGISTER_0r, &rval, VCO_DIV2f,
            1);
      WRITE_TOP_CES_PLL_CTRL_REGISTER_0r(unit, rval);

      SOC_IF_ERROR_RETURN(READ_TOP_CES_PLL_CTRL_REGISTER_1r(unit,&rval));
      soc_reg_field_set(unit, TOP_CES_PLL_CTRL_REGISTER_1r, &rval, KAf,
            1);
      soc_reg_field_set(unit, TOP_CES_PLL_CTRL_REGISTER_1r, &rval, KIf,
            1);
      soc_reg_field_set(unit, TOP_CES_PLL_CTRL_REGISTER_1r, &rval, KPf,
            5);
      WRITE_TOP_CES_PLL_CTRL_REGISTER_1r(unit, rval);


      SOC_IF_ERROR_RETURN(READ_TOP_CES_PLL_CTRL_REGISTER_2r(unit,&rval));
      soc_reg_field_set(unit, TOP_CES_PLL_CTRL_REGISTER_2r, &rval, PDIVf,
            4);
      WRITE_TOP_CES_PLL_CTRL_REGISTER_2r(unit, rval);

      SOC_IF_ERROR_RETURN(READ_TOP_CES_PLL_CTRL_REGISTER_3r(unit,&rval));
      soc_reg_field_set(unit, TOP_CES_PLL_CTRL_REGISTER_3r, &rval, NDIV_INTf,
            625);
      soc_reg_field_set(unit, TOP_CES_PLL_CTRL_REGISTER_3r, &rval, NDIV_FRACf,
            0);
      WRITE_TOP_CES_PLL_CTRL_REGISTER_3r(unit, rval);

      SOC_IF_ERROR_RETURN(READ_TOP_CES_PLL_CTRL_REGISTER_4r(unit,&rval));
      soc_reg_field_set(unit, TOP_CES_PLL_CTRL_REGISTER_4r, &rval, CH0_MDIVf,
            80);
      soc_reg_field_set(unit, TOP_CES_PLL_CTRL_REGISTER_4r, &rval, CH1_MDIVf,
            16);

      WRITE_TOP_CES_PLL_CTRL_REGISTER_4r(unit, rval);


    }




    /* De-assert LCPLL's post reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG0_PLL_POST_RST_Lf,
                      1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG1_PLL_POST_RST_Lf,
                      1);
    if (soc_feature(unit, soc_feature_ces)) {
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_CES_PLL_RST_Lf,
                          1);
    }

    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_TS_PLL_RST_Lf,
                      1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL_RST_Lf,
                      1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    sal_usleep(to_usec);

    if (!SAL_BOOT_SIMULATION) {
        if (soc_feature(unit, soc_feature_ces)) {
            SOC_IF_ERROR_RETURN(READ_TOP_CES_PLL_STATUSr(unit, &rval));
            if (!soc_reg_field_get(unit, TOP_CES_PLL_STATUSr, rval, TOP_CES_PLL_LOCKf)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "CES PLL not locked on unit %d "
                                      "status = 0x%08x\n"), unit, rval));
            }
        }
        SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_STATUSr(unit, &rval));
        if (!soc_reg_field_get(unit, TOP_TIME_SYNC_PLL_STATUSr, rval,
                               TOP_TIME_SYNC_PLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TimeSync PLL not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }

        SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC_PLL_STATUSr(unit, &rval));
        if (!soc_reg_field_get(unit, TOP_BROAD_SYNC_PLL_STATUSr, rval,
                               TOP_BROAD_SYNC_PLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "BroadSync PLL not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }
    }

    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    if (soc_feature(unit, soc_feature_ces)) {
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_CES_PLL_POST_RST_Lf,
                          1);
    }
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_TS_PLL_POST_RST_Lf,
                      1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL_POST_RST_Lf,
                      1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    sal_usleep(to_usec);

    /*
     * Bring port blocks out of reset
     */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ0_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ1_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ2_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ3_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_GP0_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_GP1_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_GP2_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));

    sal_usleep(to_usec);

    /* Bring network sync out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ0_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ1_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ2_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ3_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_NS_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));

    sal_usleep(to_usec);

    SOC_IF_ERROR_RETURN(WRITE_CMIC_SBUS_TIMEOUTr(unit, 0x7d0));

    /* Bring IP, EP, and MMU blocks out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_EP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_IP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MMU_RST_Lf, 1);
    if (soc_feature(unit, soc_feature_ces)) {
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_CES_RST_Lf, 1);
    }
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));

    sal_usleep(to_usec);

    if (((dev_id == BCM56445_DEVICE_ID) && (rev_id == BCM56445_A0_REV_ID)) ||
        ((dev_id == BCM56446_DEVICE_ID) && (rev_id == BCM56446_A0_REV_ID)) ||
        ((dev_id == BCM56447_DEVICE_ID) && (rev_id == BCM56447_A0_REV_ID)) ||
        ((dev_id == BCM56448_DEVICE_ID) && (rev_id == BCM56448_A0_REV_ID)) ) {
        _kt_lls_workaround(unit);
    }
    return SOC_E_NONE;
}
#endif  /* BCM_KATANA_SUPPORT */

#ifdef BCM_KATANA2_SUPPORT
/*
 * Function:
 *      soc_reset_bcm56450_a0
 * Purpose:
 *      Special reset sequencing for BCM56450
 *      Setup SBUS block mapping.
 */
STATIC int
soc_reset_bcm56450_a0(int unit)
{
    uint32 rval, to_usec;
    uint16 dev_id;
    uint8 rev_id;
    uint32 mmu_pkg_enable;

    unsigned ts_ref_freq;
    unsigned ts_idx;
    static const soc_pll_param_t ts_pll[] = {  /* values for 500MHz TSPLL output */
      /*     Fref,  Ndiv_int, Ndiv_frac,  Pdiv, Mdiv, Ka, Ki, Kp, VCO_div2 */
        {12800000,       312,   0x80000,     1,    8,  1,  2,  7,        1},
        {20000000,       200,         0,     1,    8,  2,  3,  8,        1},
        {25000000,       160,         0,     1,    8,  2,  4,  9,        1},
        {32000000,       125,         0,     1,    8,  2,  4,  9,        1},
        {50000000,        80,         0,     1,    8,  3,  4, 10,        1},
        {       0,       160,         0,     1,    8,  2,  4,  9,        1}  /* 25MHz, from internal reference */
    };
    unsigned bs_ref_freq;
    unsigned bs_idx;
    uint32 bs_ndiv_high, bs_ndiv_low;
    static const soc_pll_param_t bs_pll[] = {  /* values for 20MHz BSPLL output */
      /*     Fref,  Ndiv_int,  Ndiv_frac,  Pdiv, Mdiv, Ka, Ki, Kp, VCO_div2 */
        {12800000,       250,          0,     1,  160,  1,  2,  7,        0},
        {20000000,       175,          0,     1,  175,  2,  3,  8,        0},
        {25000000,       140,          0,     1,  175,  2,  4,  9,        0},
        {32000000,       108, 0xc0000000,     1,  174,  2,  4,  9,        0}, /* Ndiv=108.75 */
        {50000000,        70,          0,     1,  175,  3,  4, 10,        0},
        {       0,       140,          0,     1,  175,  2,  4,  9,        0}  /* 25MHz, from internal reference */
    };
#if defined(BCM_HITLESS_RESET_SUPPORT)
    int initialize_broadsync;
    const int broadsync_default_ka = 4, broadsync_default_ki = 1, broadsync_default_kp = 8;
#endif /* BCM_HITLESS_RESET_SUPPORT */

    soc_cm_get_id(unit, &dev_id, &rev_id);

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

    /*
     * SBUS ring and block number:
     * Ring0: cmic -> ip-ep -> cmic
     * Ring1: cmic -> mxqport0 -> mxqport1-> mxqport2 -> mxqport3-> cmic
     * Ring2: cmic -> gport0 -> gport1 -> gport2 -> cmic
     * Ring3: cmic -> otpc -> top -> cmic
     * Ring4: cmic -> mmu0 -> mmu1 -> cmic
     * Ring5: cmic -> ci0 -> ci1 -> ci2 -> lls -> cmic
     * Ring6: cmic -> ces -> cmic
     * Ring7: unused
     */
    WRITE_CMIC_SBUS_RING_MAP_0_7r(unit, 0x66034000); /* block 7  - 0 */
    WRITE_CMIC_SBUS_RING_MAP_8_15r(unit, 0x11112222); /* block 15 - 8 */
    WRITE_CMIC_SBUS_RING_MAP_16_23r(unit, 0x55555553); /* block 23 - 16 */
    WRITE_CMIC_SBUS_RING_MAP_24_31r(unit, 0x00637777); /* block 31 - 24 */

    /* Bring MMUPLL out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_MMU_PLL_INITr(unit, &rval));
    soc_reg_field_set(unit, TOP_MMU_PLL_INITr, &rval,
                      PLL1_RESETBf, 1);
    soc_reg_field_set(unit, TOP_MMU_PLL_INITr, &rval,
                      PLL2_RESETBf, 1);
    soc_reg_field_set(unit, TOP_MMU_PLL_INITr, &rval,
                      PLL3_RESETBf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MMU_PLL_INITr(unit, rval));

    sal_usleep(to_usec);

    /* Wait for MMUPLL lock */
    if (!SAL_BOOT_SIMULATION) {
        SOC_IF_ERROR_RETURN(READ_TOP_MMU_PLL_STATUS0r(unit, &rval));
        if (!soc_reg_field_get(unit, TOP_MMU_PLL_STATUS0r, rval, MMU_PLL1_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "MMUPLL 1 not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }
        if (!soc_reg_field_get(unit, TOP_MMU_PLL_STATUS0r, rval, MMU_PLL2_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "MMUPLL 2 not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }
        SOC_IF_ERROR_RETURN(READ_TOP_MMU_PLL_STATUS1r(unit, &rval));
        if (!soc_reg_field_get(unit, TOP_MMU_PLL_STATUS1r, rval, MMU_PLL3_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "MMUPLL 3 not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }
    }
    SOC_IF_ERROR_RETURN(READ_TOP_MMU_PLL_INITr(unit, &rval));
    soc_reg_field_set(unit, TOP_MMU_PLL_INITr, &rval,
                      PLL1_POST_RESETBf, 1);
    soc_reg_field_set(unit, TOP_MMU_PLL_INITr, &rval,
                      PLL2_POST_RESETBf, 1);
    soc_reg_field_set(unit, TOP_MMU_PLL_INITr, &rval,
                      PLL3_POST_RESETBf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MMU_PLL_INITr(unit, rval));

    /* TSPLL configuration: 500MHz TS_CLK from 20/25/32/50MHz refclk */
    /* CMICd divides by 2 on input, so this is a 250MHz clock to TS logic */

    ts_ref_freq = soc_property_get(unit, spn_PTP_TS_PLL_FREF, 50000000);  /* 50MHz is default.  0->internal reference */
    /* Set TS_PLL_CLK_IN_SEL based on reference frequency.  If it is 0, use the internal reference */
    SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r(unit,&rval));
    soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, TS_PLL_CLK_IN_SELf,
                      (ts_ref_freq != 0));
    WRITE_TOP_MISC_CONTROL_1r(unit, rval);

    for (ts_idx = 0; ts_idx < sizeof(ts_pll)/sizeof(ts_pll[0]); ++ts_idx) {
        if (ts_pll[ts_idx].ref_freq == ts_ref_freq) {
            break;
        }
    }
    if (ts_idx == sizeof(ts_pll)/sizeof(ts_pll[0])) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Invalid value for PTP_TS_PLL_REF (%u).  No default PLL params.\n"), ts_ref_freq));
        /* Could check for existence of SOC properties for VCO_DIV2, KA, KI, KP, NDIV_INT, NDIV_FRAC, MDIV, PDIV, and if
           all exist, use them.  For now, just fail.
        */
    } else {
        SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_0r(unit,&rval));
        soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_0r, &rval, VCO_DIV2f,
                          soc_property_get(unit, spn_PTP_TS_VCO_DIV2, ts_pll[ts_idx].vco_div2));
        WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_0r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_1r(unit,&rval));
        soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_1r, &rval, KAf,
                          soc_property_get(unit, spn_PTP_TS_KA, ts_pll[ts_idx].ka));
        soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_1r, &rval, KIf,
                          soc_property_get(unit, spn_PTP_TS_KI, ts_pll[ts_idx].ki));
        soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_1r, &rval, KPf,
                          soc_property_get(unit, spn_PTP_TS_KP, ts_pll[ts_idx].kp));
        WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_1r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r(unit,&rval));
        soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r, &rval, PDIVf,
                          soc_property_get(unit, spn_PTP_TS_PLL_PDIV, ts_pll[ts_idx].pdiv));
        WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_3r(unit,&rval));
        soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_3r, &rval, NDIV_INTf,
                          soc_property_get(unit, spn_PTP_TS_PLL_N, ts_pll[ts_idx].ndiv_int));
        soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_3r, &rval, NDIV_FRACf,
                          ts_pll[ts_idx].ndiv_frac);
        WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_3r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_4r(unit,&rval));
        soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_4r, &rval, CH0_MDIVf,
                          soc_property_get(unit, spn_PTP_TS_PLL_MNDIV, ts_pll[ts_idx].mdiv));
        WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_4r(unit, rval);
    }

    /* 500Mhz TSPLL -> 250MHz ref at timestamper, implies 4ns resolution */
    SOC_TIMESYNC_PLL_CLOCK_NS(unit) = (1/250 * 1000); /* clock period in nanoseconds */

    /* Check to see if the BSPLLs have been configured yet.  If so, we leave them alone,
       for hitless reset support.
       Note: On KT2 A0/B0, the BroadSync Bitclock/Heartbeat regs get reset, so we still
       initialize all of BroadSync.
    */
    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_1r(unit,&rval));

#if defined(BCM_HITLESS_RESET_SUPPORT)
    initialize_broadsync =
        (soc_reg_field_get(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_1r,
                           rval, KAf) == broadsync_default_ka
         && soc_reg_field_get(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_1r,
                              rval, KIf) == broadsync_default_ki
         && soc_reg_field_get(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_1r,
                              rval, KPf) == broadsync_default_kp);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "BSPLL regs:  %s\n"), initialize_broadsync ? "Default" : "Configured"));

    if (!initialize_broadsync) {
        /* JIRA CMICD-110
         * On A0/B0, BroadSync Bitclock/Heartbeat divisors/enable registers get reset,
         * though the outputs themselves do not if the registers were written to an even
         * number of times.  Workaround: when the registers are changed, the register
         * should be written twice, and the same value stored in SRAM.  This code will
         * restore the register from SRAM.
         */

        /* BS0 */
        uint32 reg_cache_base = soc_cmic_bs_reg_cache(unit, 0);

        uint32 config = soc_uc_mem_read(unit,
             reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, config));
        uint32 clk_ctrl = soc_uc_mem_read(unit,
             reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, clk_ctrl));
        uint32 heartbeat_ctrl = soc_uc_mem_read(unit,
             reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, heartbeat_ctrl));
        uint32 heartbeat_down = soc_uc_mem_read(unit,
             reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, heartbeat_down_duration));
        uint32 heartbeat_up = soc_uc_mem_read(unit,
             reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, heartbeat_up_duration));

        if (config == 0 && clk_ctrl == 0 && heartbeat_ctrl == 0 && heartbeat_down == 0 && heartbeat_up == 0) {
            /* PLL was initialized, but there is no saved state.  Reset BroadSync. */
            initialize_broadsync = 1;
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "BSPLL Configured, but no BS state to restore\n")));
        } else {
            /* Double-writes, per CMICD-110 WAR */
            WRITE_CMIC_BS0_CONFIGr(unit, config);
            WRITE_CMIC_BS0_CONFIGr(unit, config);
            WRITE_CMIC_BS0_CLK_CTRLr(unit, clk_ctrl);
            WRITE_CMIC_BS0_CLK_CTRLr(unit, clk_ctrl);
            WRITE_CMIC_BS0_HEARTBEAT_CTRLr(unit, heartbeat_ctrl);
            WRITE_CMIC_BS0_HEARTBEAT_CTRLr(unit, heartbeat_ctrl);
            WRITE_CMIC_BS0_HEARTBEAT_DOWN_DURATIONr(unit, heartbeat_down);
            WRITE_CMIC_BS0_HEARTBEAT_DOWN_DURATIONr(unit, heartbeat_down);
            WRITE_CMIC_BS0_HEARTBEAT_UP_DURATIONr(unit, heartbeat_up);
            WRITE_CMIC_BS0_HEARTBEAT_UP_DURATIONr(unit, heartbeat_up);

            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Restored BS0 regs:\n"
                                    "  config: %08x  clk_ctrl: %08x hb_ctrl: %08x\n"
                                    "  hb_up: %08x hb_down: %08x\n"),
                         config, clk_ctrl, heartbeat_ctrl,
                         heartbeat_down, heartbeat_up));

            /* same, but for BS1 */
            reg_cache_base = soc_cmic_bs_reg_cache(unit, 1);

            config = soc_uc_mem_read(unit,
                 reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, config));
            clk_ctrl = soc_uc_mem_read(unit,
                 reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, clk_ctrl));
            heartbeat_ctrl = soc_uc_mem_read(unit,
                 reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, heartbeat_ctrl));
            heartbeat_down = soc_uc_mem_read(unit,
                 reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, heartbeat_down_duration));
            heartbeat_up = soc_uc_mem_read(unit,
                 reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, heartbeat_up_duration));

            WRITE_CMIC_BS1_CONFIGr(unit, config);
            WRITE_CMIC_BS1_CONFIGr(unit, config);
            WRITE_CMIC_BS1_CLK_CTRLr(unit, clk_ctrl);
            WRITE_CMIC_BS1_CLK_CTRLr(unit, clk_ctrl);
            WRITE_CMIC_BS1_HEARTBEAT_CTRLr(unit, heartbeat_ctrl);
            WRITE_CMIC_BS1_HEARTBEAT_CTRLr(unit, heartbeat_ctrl);
            WRITE_CMIC_BS1_HEARTBEAT_DOWN_DURATIONr(unit, heartbeat_down);
            WRITE_CMIC_BS1_HEARTBEAT_DOWN_DURATIONr(unit, heartbeat_down);
            WRITE_CMIC_BS1_HEARTBEAT_UP_DURATIONr(unit, heartbeat_up);
            WRITE_CMIC_BS1_HEARTBEAT_UP_DURATIONr(unit, heartbeat_up);

            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Restored BS1 regs:\n"
                                    "  config: %08x  clk_ctrl: %08x hb_ctrl: %08x\n"
                                    "  hb_up: %08x hb_down: %08x\n"),
                         config, clk_ctrl, heartbeat_ctrl,
                         heartbeat_down, heartbeat_up));
        }
    }

    if (initialize_broadsync)
#endif  /* BCM_HITLESS_RESET_SUPPORT */
    {
        /* as a signal to upper-level code that the BroadSync is newly initialized
         * disable BroadSync0/1 bitclock output.  Checked in time.c / 1588 firmware    */
        SOC_IF_ERROR_RETURN(READ_CMIC_BS0_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &rval, BS_CLK_OUTPUT_ENABLEf, 0);
        WRITE_CMIC_BS0_CONFIGr(unit, rval);

        SOC_IF_ERROR_RETURN(READ_CMIC_BS1_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &rval, BS_CLK_OUTPUT_ENABLEf, 0);
        WRITE_CMIC_BS1_CONFIGr(unit, rval);

        /* BSPLL0 has not been configured, so reset/configure both BSPLL0 and BSPLL1 */
        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_3r(unit, &rval));
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_BS0_PLL_RST_Lf, 0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_BS1_PLL_RST_Lf, 0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_BS0_PLL_POST_RST_Lf, 0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_BS1_PLL_POST_RST_Lf, 0);
        WRITE_TOP_SOFT_RESET_REG_3r(unit, rval);

        /* Both BSPLLs configured the same, for 20MHz output by default */

        bs_ref_freq = soc_property_get(unit, spn_PTP_BS_FREF, 50000000);  /* 50MHz is default.  0->internal reference */
        /* Set BS_PLL_CLK_IN_SEL based on reference frequency.  If it is 0, use the internal reference */
        SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r(unit,&rval));
        soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, BS0_PLL_CLK_IN_SELf,
                          (bs_ref_freq != 0));
        soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, BS1_PLL_CLK_IN_SELf,
                          (bs_ref_freq != 0));
        WRITE_TOP_MISC_CONTROL_1r(unit, rval);

        for (bs_idx = 0; bs_idx < sizeof(bs_pll)/sizeof(bs_pll[0]); ++bs_idx) {
            if (bs_pll[bs_idx].ref_freq == bs_ref_freq) {
                break;
            }
        }
        if (bs_idx == sizeof(bs_pll)/sizeof(bs_pll[0])) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Invalid value for PTP_BS_REF (%u).  No default PLL params.\n"), bs_ref_freq));
            /* Could check for existence of SOC properties for VCO_DIV2, KA, KI, KP, NDIV_INT,
               NDIV_FRAC, MDIV, PDIV, and if all exist, use them.  For now, just fail.
            */
        } else {
            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_3r(unit,&rval));
            soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_3r, &rval, VCODIV2f,
                              soc_property_get(unit, spn_PTP_BS_VCO_DIV2, bs_pll[bs_idx].vco_div2));
            WRITE_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_3r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_3r(unit,&rval));
            soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_3r, &rval, VCODIV2f,
                              soc_property_get(unit, spn_PTP_BS_VCO_DIV2, bs_pll[bs_idx].vco_div2));
            WRITE_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_3r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_1r(unit,&rval));
            soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_1r, &rval, KAf,
                              soc_property_get(unit, spn_PTP_BS_KA, bs_pll[bs_idx].ka));
            soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_1r, &rval, KIf,
                              soc_property_get(unit, spn_PTP_BS_KI, bs_pll[bs_idx].ki));
            soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_1r, &rval, KPf,
                              soc_property_get(unit, spn_PTP_BS_KP, bs_pll[bs_idx].kp));
            soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_1r, &rval, PDIVf,
                              soc_property_get(unit, spn_PTP_BS_PDIV, bs_pll[bs_idx].pdiv));
            WRITE_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_1r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_1r(unit,&rval));
            soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_1r, &rval, KAf,
                              soc_property_get(unit, spn_PTP_BS_KA, bs_pll[bs_idx].ka));
            soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_1r, &rval, KIf,
                              soc_property_get(unit, spn_PTP_BS_KI, bs_pll[bs_idx].ki));
            soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_1r, &rval, KPf,
                              soc_property_get(unit, spn_PTP_BS_KP, bs_pll[bs_idx].kp));
            soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_1r, &rval, PDIVf,
                              soc_property_get(unit, spn_PTP_BS_PDIV, bs_pll[bs_idx].pdiv));
            WRITE_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_1r(unit, rval);

            bs_ndiv_high = ((soc_property_get(unit, spn_PTP_BS_NDIV_INT, bs_pll[bs_idx].ndiv_int) << 8) |
                            ((soc_property_get(unit, spn_PTP_BS_NDIV_FRAC, bs_pll[bs_idx].ndiv_frac) >> 24) & 0xff));
            bs_ndiv_low = (soc_property_get(unit, spn_PTP_BS_NDIV_FRAC, bs_pll[bs_idx].ndiv_frac) << 8);

            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r(unit,&rval));
            soc_reg_field_set(unit, TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r, &rval, BROAD_SYNC0_LCPLL_FBDIV_1f, bs_ndiv_high);
            WRITE_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_0r(unit,&rval));
            soc_reg_field_set(unit, TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_0r, &rval, BROAD_SYNC0_LCPLL_FBDIV_0f, bs_ndiv_low);
            WRITE_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_0r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r(unit,&rval));
            soc_reg_field_set(unit, TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r, &rval, BROAD_SYNC1_LCPLL_FBDIV_1f, bs_ndiv_high);
            WRITE_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_0r(unit,&rval));
            soc_reg_field_set(unit, TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_0r, &rval, BROAD_SYNC1_LCPLL_FBDIV_0f, bs_ndiv_low);
            WRITE_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_0r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_0r(unit,&rval));
            soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_0r, &rval, CH1_MDIVf,
                              soc_property_get(unit, spn_PTP_BS_MNDIV, bs_pll[bs_idx].mdiv));
            WRITE_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_0r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_0r(unit,&rval));
            soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_0r, &rval, CH1_MDIVf,
                              soc_property_get(unit, spn_PTP_BS_MNDIV, bs_pll[bs_idx].mdiv));
            WRITE_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_0r(unit, rval);
        }

        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_3r(unit, &rval));
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_BS0_PLL_RST_Lf, 1);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_BS1_PLL_RST_Lf, 1);
        WRITE_TOP_SOFT_RESET_REG_3r(unit, rval);

        /* JIRA CMICD-110:  See above logic to restore BroadSync Bitclock/Heartbeat. */
        /* Store default values in the SRAM, so they will be restored correctly */
        /* even if SRAM is not used */
#if defined(BCM_HITLESS_RESET_SUPPORT)
        {
            /* store reset values in SRAM for BS0 */
            uint32 reg_cache_base = soc_cmic_bs_reg_cache(unit, 0);
            READ_CMIC_BS0_CONFIGr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, config), rval);
            READ_CMIC_BS0_CLK_CTRLr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, clk_ctrl), rval);
            READ_CMIC_BS0_HEARTBEAT_CTRLr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, heartbeat_ctrl), rval);
            READ_CMIC_BS0_HEARTBEAT_DOWN_DURATIONr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, heartbeat_down_duration), rval);
            READ_CMIC_BS0_HEARTBEAT_UP_DURATIONr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, heartbeat_up_duration), rval);

            /* store reset values in SRAM for BS1 */
            reg_cache_base = soc_cmic_bs_reg_cache(unit, 1);
            READ_CMIC_BS1_CONFIGr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, config), rval);
            READ_CMIC_BS1_CLK_CTRLr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, clk_ctrl), rval);
            READ_CMIC_BS1_HEARTBEAT_CTRLr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, heartbeat_ctrl), rval);
            READ_CMIC_BS1_HEARTBEAT_DOWN_DURATIONr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, heartbeat_down_duration), rval);
            READ_CMIC_BS1_HEARTBEAT_UP_DURATIONr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, heartbeat_up_duration), rval);
        }
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "Leaving BroadSync in holdover\n")));

        if (rev_id == BCM56450_A0_REV_ID) {
            /* WAR for reset of TOP_MISC_CONTROL_1.BSn_PLL_CLK_IN_SEL registers */
            /* Note that on A0 there will still be a "hit" if an external reference is used */
            /* But this prevents the wrong value from being persisted after a reset */
            bs_ref_freq = soc_property_get(unit, spn_PTP_BS_FREF, 50000000);  /* 50MHz is default.  0->internal reference */
            /* Reference frequency != 0 => use external reference */
            if (bs_ref_freq != 0) {
                SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r(unit,&rval));
                soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, BS0_PLL_CLK_IN_SELf, 1);
                soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, BS1_PLL_CLK_IN_SELf, 1);
                WRITE_TOP_MISC_CONTROL_1r(unit, rval);
            }
        }
#endif  /* BCM_HITLESS_RESET_SUPPORT */
    }

    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_TS_PLL_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_DDR_PLL0_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    sal_usleep(to_usec);

    if (!SAL_BOOT_SIMULATION) {
        SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_STATUSr(unit, &rval));
        if (!soc_reg_field_get(unit, TOP_TIME_SYNC_PLL_STATUSr, rval,
                               TOP_TIME_SYNC_PLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TimeSync PLL not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }

        SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_PLL_STATUSr(unit, &rval));
        if (!soc_reg_field_get(unit, TOP_BROAD_SYNC0_PLL_STATUSr, rval,
                               TOP_BROAD_SYNC0_PLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "BroadSync0 PLL not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }

        SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_PLL_STATUSr(unit, &rval));
        if (!soc_reg_field_get(unit, TOP_BROAD_SYNC1_PLL_STATUSr, rval,
                               TOP_BROAD_SYNC1_PLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "BroadSync1 PLL not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }

        if (dev_id != BCM56456_DEVICE_ID) {
            SOC_IF_ERROR_RETURN(READ_TOP_DDR_PLL0_STATUSr(unit, &rval));
            if (!soc_reg_field_get(unit, TOP_DDR_PLL0_STATUSr, rval,
                                   DDR_PLL0_LOCKf)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "DDR PLL not locked on unit %d "
                                      "status = 0x%08x\n"), unit, rval));
            }
        }
    }

#if defined(BCM_HITLESS_RESET_SUPPORT)
    if (initialize_broadsync)
#endif  /* BCM_HITLESS_RESET_SUPPORT */
    {
        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_3r(unit, &rval));
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_BS0_PLL_POST_RST_Lf, 1);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_BS1_PLL_POST_RST_Lf, 1);
        WRITE_TOP_SOFT_RESET_REG_3r(unit, rval);
    }

    /* Check for M, A and Plain versions */
    SOC_IF_ERROR_RETURN(READ_TOP_SWITCH_FEATURE_ENABLE_1r(unit, &rval));
    if (rval & 0x10000000) {
        /* BOND_1588_ENABLE */
        SOC_FEATURE_SET(unit, soc_feature_timesync_support);
        LOG_CLI((BSL_META_U(unit,
                            "BOND_1588_ENABLE : "
                            "enabling soc_feature_timesync_support\n")));
    }

    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_TS_PLL_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_DDR_PLL0_POST_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    sal_usleep(to_usec);
    LOG_CLI((BSL_META_U(unit,
                        "###################################################\n")));
    if (SOC_INFO(unit).olp_port[0]) {
        LOG_CLI((BSL_META_U(unit,
                            "OLP PORT IS *USED* \n")));
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "OLP PORT IS *NOT USED* \n")));
        SOC_IF_ERROR_RETURN(READ_TOP_SW_BOND_OVRD_CTRL0r(unit, &rval));
        soc_reg_field_set(unit,TOP_SW_BOND_OVRD_CTRL0r,&rval,
                          OLP_ENABLEf,0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_SW_BOND_OVRD_CTRL0r(unit, rval));
    }
    LOG_CLI((BSL_META_U(unit,
                        "###################################################\n")));

    /*
     * Bring port blocks out of reset
     */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ0_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ1_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ2_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ3_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ4_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ5_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ6_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ7_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ8_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ9_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ10_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));

    sal_usleep(to_usec);

    /* Bring network sync out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ0_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ1_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ2_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ3_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ4_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ5_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ6_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ7_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ8_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ9_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ10_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_NS_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));

    sal_usleep(to_usec);

    SOC_IF_ERROR_RETURN(WRITE_CMIC_SBUS_TIMEOUTr(unit, 0x7d0));

    /* enable packing mode based on soc propoerty before mmu is out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SW_BOND_OVRD_CTRL1r(unit, &rval));

    mmu_pkg_enable = soc_property_get(unit, spn_MMU_MULTI_PACKETS_PER_CELL, 0);
    if(mmu_pkg_enable > ((1 << (soc_reg_field_length(unit, TOP_SW_BOND_OVRD_CTRL1r,
                                                     MMU_PACKING_ENABLEf))) - 1)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_reset_bcm56450_a0: "
                              "specified MMU_MULTI_PACKETS_PER_CELL %d exceeds "
                              "maximum\n"),
                              mmu_pkg_enable));
        return SOC_E_PARAM;
    }
    soc_reg_field_set(unit, TOP_SW_BOND_OVRD_CTRL1r, &rval,
                      MMU_PACKING_ENABLEf, mmu_pkg_enable);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SW_BOND_OVRD_CTRL1r(unit, rval));

    /* Bring IP, EP, and MMU blocks out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_EP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_IP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MMU_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));

    /* Bring AMAC out of reset */
    soc_amac_init(unit);

    sal_usleep(to_usec);
    return SOC_E_NONE;
}
#endif  /* BCM_KATANA2_SUPPORT */

#ifdef BCM_METROLITE_SUPPORT

STATIC int
_soc_metrolite_tspll_bspll_config(int unit)
{
    uint32 rval;
    unsigned ts_ref_freq;
    unsigned ts_idx;

    /* values for 500MHz TSPLL output-f(vco)=3500, chan=0, f(out)=500*/
    static const soc_pll_param_t ts_pll[] = {
    /*     Fref,  Ndiv_int, Ndiv_frac,  Pdiv, Mdiv, Ka, Ki, Kp, VCO_div2 */
      {50000000,      70,         0,     1,    7,  0,  2, 3,        1},
      {       0,      70,         0,     1,    7,  0,  2, 3,        1}
    };

    unsigned bs_ref_freq;
    unsigned bs_idx;
    uint32 bs_ndiv_high, bs_ndiv_low;

    /* values for 20MHz BSPLL output-f(vco) = 3100,channel = 1,f(out) = 20*/
    static const soc_pll_param_t bs_pll[] = {
    /*     Fref,  Ndiv_int,  Ndiv_frac,  Pdiv, Mdiv, Ka, Ki, Kp, VCO_div2 */
      {50000000,      62,          0,     1,  155,  0,  0, 0,        0},
      {       0,      62,          0,     1,  155,  0,  0, 0,        0}
    };

    /* TSPLL configuration: 500MHz TS_CLK from 20/25/32/50MHz refclk */
    /* CMICd divides by 2 on input, so this is a 250MHz clock to TS logic */

    /* 50MHz is default ->internal reference */
    ts_ref_freq = soc_property_get(unit, spn_PTP_TS_PLL_FREF, 0);
    /* Configure TS PLL */
    for (ts_idx = 0; ts_idx < sizeof(ts_pll)/sizeof(ts_pll[0]); ++ts_idx) {
        if (ts_pll[ts_idx].ref_freq == ts_ref_freq) {
            break;
        }
    }
    if (ts_idx == sizeof(ts_pll)/sizeof(ts_pll[0])) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
         "Invalid value for PTP_TS_PLL_REF (%u).  No default PLL params. \
            Using internal clock \n"), ts_ref_freq));
        /* resetting back to internal clock */
        ts_ref_freq = 0;
        ts_idx = ts_idx - 1;
    }

    /* Put TS, BS PLLs to reset before changing PLL control registers */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
        TOP_TS_PLL_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
        TOP_TS_PLL_POST_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_3r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
        TOP_BS0_PLL_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
        TOP_BS1_PLL_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
        TOP_BS0_PLL_POST_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
        TOP_BS1_PLL_POST_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_3r(unit, rval));

    /* TS PLL */
    SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_1r(unit,&rval));
    soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_1r, &rval, FREF_SELf,
                      (ts_ref_freq != 0));
    WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_1r(unit, rval);

    if (SAL_BOOT_QUICKTURN) {
         LOG_VERBOSE(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
              "%s:%d Skipping PLL Configuration \n"), FUNCTION_NAME(),__LINE__));
    } else {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
         (BSL_META_U(unit,
         "%s:%d Continuing PLL Configuration \n"), FUNCTION_NAME(),__LINE__));

        if (SOC_REG_FIELD_VALID(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_0r, VCO_DIV2f)) {
            SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_0r(unit,&rval));
            soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_0r, &rval, VCO_DIV2f,
                 ts_pll[ts_idx].vco_div2);
            WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_0r(unit, rval);
        }

        if (SOC_REG_IS_VALID(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r)) {
            SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r(unit,&rval));
            soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r, &rval, KAf,
                   soc_property_get(unit, spn_PTP_TS_KA, ts_pll[ts_idx].ka));
            soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r, &rval, KIf,
                   soc_property_get(unit, spn_PTP_TS_KI, ts_pll[ts_idx].ki));
            soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r, &rval, KPf,
                   soc_property_get(unit, spn_PTP_TS_KP, ts_pll[ts_idx].kp));
            soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r, &rval,
                   NDIV_FRACf, ts_pll[ts_idx].ndiv_frac);
            WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r(unit, rval);
        }

        if (SOC_REG_IS_VALID(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_3r)) {
            SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_3r(unit,&rval));
            soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_3r, &rval, NDIV_INTf,
                   soc_property_get(unit, spn_PTP_TS_PLL_N,
                                    ts_pll[ts_idx].ndiv_int));
            WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_3r(unit, rval);
        }

        if (SOC_REG_IS_VALID(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r)) {
            SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r(unit,&rval));
            soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r, &rval, PDIVf,
                   soc_property_get(unit, spn_PTP_TS_PLL_PDIV, ts_pll[ts_idx].pdiv));
            soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r, &rval, CH0_MDIVf,
                   soc_property_get(unit, spn_PTP_TS_PLL_MNDIV,
                             ts_pll[ts_idx].mdiv));
            soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r, &rval, CH1_MDIVf,
                   soc_property_get(unit, spn_PTP_TS_PLL_MNDIV,
                             ts_pll[ts_idx].mdiv));
            WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r(unit, rval);
        }
    }

    /* 250Mhz TS PLL implies 4ns resolution */
    SOC_TIMESYNC_PLL_CLOCK_NS(unit) = (1/250 * 1000); /* clock period in nanoseconds */


    /* Configure Broadsync PLL's */
    /* Both BSPLLs configured the same, for 20MHz output by default */

    /* 50MHz is default.  0->internal reference */
    bs_ref_freq = soc_property_get(unit, spn_PTP_BS_FREF, 0);
    for (bs_idx = 0; bs_idx < sizeof(bs_pll)/sizeof(bs_pll[0]); ++bs_idx) {
        if (bs_pll[bs_idx].ref_freq == bs_ref_freq) {
            break;
        }
    }

    if (bs_idx == sizeof(bs_pll)/sizeof(bs_pll[0])) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                    "Invalid value for PTP_BS_REF (%u).  No default PLL params. \
                    Resetting to internal clock\n"), bs_ref_freq));
        bs_idx = bs_idx - 1;
        bs_ref_freq = 0;
    }

    /* bs0 pll - set reference clock */
    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_6r(unit,&rval));
    rval = (bs_ref_freq == 0) ? rval : rval | 1 << 18;
    WRITE_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_6r(unit, rval);

    /* bs1 pll - set reference clock */
    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_6r(unit,&rval));
    rval = (bs_ref_freq == 0) ? rval : rval | 1 << 18;
    WRITE_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_6r(unit, rval);

    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_1r(unit,&rval));

    /* as a signal to upper-level code that the BroadSync is newly initialized
     * disable BroadSync0/1 bitclock output.  Checked in time.c / 1588 firmware
     */
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_BS0_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &rval, BS_CLK_OUTPUT_ENABLEf, 0);
    WRITE_CMIC_BS0_CONFIGr(unit, rval);

    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_BS1_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &rval, BS_CLK_OUTPUT_ENABLEf, 0);
    WRITE_CMIC_BS1_CONFIGr(unit, rval);

    /* bs pll - ndiv, mdiv, channel info */
    bs_ndiv_high = ((soc_property_get(unit, spn_PTP_BS_NDIV_INT, bs_pll[bs_idx].ndiv_int) << 6) |
        ((soc_property_get(unit, spn_PTP_BS_NDIV_FRAC, bs_pll[bs_idx].ndiv_frac) >> 18) & 0x3f));

    bs_ndiv_low = (soc_property_get(unit, spn_PTP_BS_NDIV_FRAC, bs_pll[bs_idx].ndiv_frac) << 14);

    /* bs pll - 0 configuration */
    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r(unit,&rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r, &rval,
                      BROAD_SYNC0_LCPLL_FBDIV_1f, bs_ndiv_high);
    WRITE_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r(unit, rval);

    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_0r(unit,&rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_0r, &rval,
                      BROAD_SYNC0_LCPLL_FBDIV_0f, bs_ndiv_low);
    WRITE_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_0r(unit, rval);

    /* for BS , use channel 0 */
    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_1r(unit,&rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_1r, &rval,
                      CH1_MDIVf, soc_property_get(unit, spn_PTP_BS_MNDIV,
                      bs_pll[bs_idx].mdiv));
    WRITE_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_1r(unit, rval);

    /* bs pll - 1 configuration */
    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r(unit,&rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r, &rval,
                      BROAD_SYNC1_LCPLL_FBDIV_1f, bs_ndiv_high);
    WRITE_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r(unit, rval);

    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_0r(unit,&rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_0r, &rval,
                      BROAD_SYNC1_LCPLL_FBDIV_0f, bs_ndiv_low);
    WRITE_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_0r(unit, rval);

    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_1r(unit,&rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_1r, &rval,
                      CH1_MDIVf, soc_property_get(unit, spn_PTP_BS_MNDIV,
                      bs_pll[bs_idx].mdiv));
    WRITE_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_1r(unit, rval);

    return SOC_E_NONE;
}


STATIC int
_soc_metrolite_tspll_bspll_lock(int unit)
{
    uint32 rval, to_usec;

    /* Wait for pll lock */
    to_usec = 10 * MILLISECOND_USEC;
    sal_usleep(to_usec);
    if (!SAL_BOOT_SIMULATION) {
        SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_STATUSr(unit, &rval));
        if (!soc_reg_field_get(unit, TOP_TIME_SYNC_PLL_STATUSr, rval,
                               TOP_TIME_SYNC_PLL_LOCKf)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                         "TimeSync PLL not locked on unit %d "
                         "status = 0x%08x\n"), unit, rval));
        }

        if (SOC_REG_IS_VALID(unit, TOP_BROAD_SYNC0_PLL_STATUSr)) {
            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_PLL_STATUSr(unit, &rval));
            if (!soc_reg_field_get(unit, TOP_BROAD_SYNC0_PLL_STATUSr, rval,
                        TOP_BROAD_SYNC0_PLL_LOCKf)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                          "BroadSync0 PLL not locked on unit %d "
                          "status = 0x%08x\n"), unit, rval));
            }
        }

        if (SOC_REG_IS_VALID(unit, TOP_BROAD_SYNC1_PLL_STATUSr)) {
            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_PLL_STATUSr(unit, &rval));
            if (!soc_reg_field_get(unit, TOP_BROAD_SYNC1_PLL_STATUSr, rval,
                        TOP_BROAD_SYNC1_PLL_LOCKf)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                          "BroadSync1 PLL not locked on unit %d "
                          "status = 0x%08x\n"), unit, rval));
            }
        }
    }

    return 0;
}

/*
 * Function:
 *      soc_reset_bcm56270_a0
 * Purpose:
 *      Special reset sequencing for BCM56270
 *      Setup SBUS block mapping.
 */
STATIC int
soc_reset_bcm56270_a0(int unit)
{
    uint32 rval, to_usec;
    uint16 dev_id;
    uint8 rev_id;


    soc_cm_get_id(unit, &dev_id, &rev_id);

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

    soc_metrolite_sbus_ring_map_config(unit);
    sal_usleep(to_usec);


   /* Master LCPLL */
    rval = 0x197d2014;
    SOC_IF_ERROR_RETURN(WRITE_TOP_XGXS0_PLL_CONTROL_1r(unit, rval));

    rval = 0x40004015;
    SOC_IF_ERROR_RETURN(WRITE_TOP_XGXS0_PLL_CONTROL_2r(unit, rval));

    rval = 0x897004;
    SOC_IF_ERROR_RETURN(WRITE_TOP_XGXS0_PLL_CONTROL_3r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_XGXS0_PLL_CONTROL_4r(unit, &rval));
    soc_reg_field_set(unit, TOP_XGXS0_PLL_CONTROL_4r, &rval,
            CP1f, 0x1);
    soc_reg_field_set(unit, TOP_XGXS0_PLL_CONTROL_4r, &rval,
            CPf, 0x3);
    soc_reg_field_set(unit, TOP_XGXS0_PLL_CONTROL_4r, &rval,
            CZf, 0x3);
    soc_reg_field_set(unit, TOP_XGXS0_PLL_CONTROL_4r, &rval,
            RPf, 0x3);
    soc_reg_field_set(unit, TOP_XGXS0_PLL_CONTROL_4r, &rval,
            RZf, 0);
    soc_reg_field_set(unit, TOP_XGXS0_PLL_CONTROL_4r, &rval,
            ICPf, 0xA);
    soc_reg_field_set(unit, TOP_XGXS0_PLL_CONTROL_4r, &rval,
            VCO_GAINf, 0xF);
    SOC_IF_ERROR_RETURN(WRITE_TOP_XGXS0_PLL_CONTROL_4r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_XGXS0_PLL_CONTROL_6r(unit, &rval));
    soc_reg_field_set(unit, TOP_XGXS0_PLL_CONTROL_6r, &rval,
            CH2_MDIVf, 0x7);
    soc_reg_field_set(unit, TOP_XGXS0_PLL_CONTROL_6r, &rval,
            PDIVf, 0x1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_XGXS0_PLL_CONTROL_6r(unit, rval));


    /* Broad Sync0 LCPLL */
    rval = 0x00805004;
    SOC_IF_ERROR_RETURN(WRITE_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_3r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_6r(unit,
                &rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_6r, &rval,
            LDO_CTRLf, 0x22);
    SOC_IF_ERROR_RETURN(WRITE_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_6r(unit,
                rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_4r(unit,
                &rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_4r, &rval,
            CP1f, 0x1);
    soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_4r, &rval,
            CPf, 0x3);
    soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_4r, &rval,
            CZf, 0x3);
    soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_4r, &rval,
            RPf, 0x7);
    soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_4r, &rval,
            RZf, 0x2);
    soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_4r, &rval,
            ICPf, 0xA);
    soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_4r, &rval,
            VCO_GAINf, 0xF);
    SOC_IF_ERROR_RETURN(WRITE_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_4r(unit,
                rval));

   /* Broad Sync1 LCPLL */
    rval = 0x00805004;
    SOC_IF_ERROR_RETURN(WRITE_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_3r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_6r(unit,
                &rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_6r, &rval,
            LDO_CTRLf, 0x22);
    SOC_IF_ERROR_RETURN(WRITE_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_6r(unit,
                rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_4r(unit,
                &rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_4r, &rval,
            CP1f, 0x1);
    soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_4r, &rval,
            CPf, 0x3);
    soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_4r, &rval,
            CZf, 0x3);
    soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_4r, &rval,
            RPf, 0x7);
    soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_4r, &rval,
            RZf, 0x2);
    soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_4r, &rval,
            ICPf, 0xA);
    soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_4r, &rval,
            VCO_GAINf, 0xF);
    SOC_IF_ERROR_RETURN(WRITE_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_4r(unit,
                rval));

    /* Top Master LCPLL settings */
    rval = 0x7d0;
    SOC_IF_ERROR_RETURN(WRITE_TOP_MASTER_LCPLL_FBDIV_CTRL_1r(unit, rval));

    /* End of PLL settings. Togle LCPLL soft reset. */
    SOC_IF_ERROR_RETURN(READ_TOP_LCPLL_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_LCPLL_SOFT_RESET_REGr, &rval,
            LCPLL_SOFT_RESETf, 0x1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_LCPLL_SOFT_RESET_REGr(unit, rval));

    sal_usleep(to_usec);
    soc_reg_field_set(unit, TOP_LCPLL_SOFT_RESET_REGr, &rval,
            LCPLL_SOFT_RESETf, 0x0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_LCPLL_SOFT_RESET_REGr(unit, rval));

    sal_usleep(to_usec);

    /* Wait for LCPLL lock */
    if (!SAL_BOOT_SIMULATION) {
        SOC_IF_ERROR_RETURN(READ_TOP_XGXS0_PLL_STATUSr(unit, &rval));

        if (!soc_reg_field_get(unit, TOP_XGXS0_PLL_STATUSr, rval, TOP_XGPLL0_LOCKf)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                       (BSL_META_U(unit,
                       "LCPLL 0 not locked on unit %d "
                       "status = 0x%08x\n"), unit, rval));
        }
    }



/* top_soft_reset_reg */
    /* Bring IP, EP, and MMU blocks out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_ARS_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_AVS_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_NS_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_EP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_IP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MMU_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_PM1_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_PM0_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));


    /* IPIPE and EPIPE configurations */
    SOC_IF_ERROR_RETURN(soc_saber2_hw_reset_control_init(unit));

    /* Metrolite TS and BS PLL configurations */
    SOC_IF_ERROR_RETURN(_soc_metrolite_tspll_bspll_config(unit));

    sal_usleep(to_usec);

    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_TS_PLL_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    sal_usleep(to_usec);

    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_TS_PLL_POST_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    /* Bring BS PLL out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_3r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
        TOP_BS0_PLL_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
        TOP_BS1_PLL_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
        TOP_BS0_PLL_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
        TOP_BS1_PLL_POST_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_3r(unit, rval));

    /* Check the lock status of TS and BS PLL */
    _soc_metrolite_tspll_bspll_lock(unit);

    return SOC_E_NONE;
}
#endif  /* BCM_METROLITE_SUPPORT */


#ifdef BCM_SABER2_SUPPORT
/*
 * Function:
 *      soc_reset_bcm56260_a0
 * Purpose:
 *      Special reset sequencing for BCM56260
 *      Setup SBUS block mapping.
 */
STATIC int
soc_reset_bcm56260_a0(int unit)
{
    uint32 rval, to_usec;
    uint16 dev_id;
    uint8 rev_id;
    uint32 ddr3_clock_mhz = 0;
    uint32 num_rows = 32768;
    uint32 ddr3_mem_grade = 0;
    uint32 ext_pbmp_count=0;
    pbmp_t ext_pbmp;
    soc_port_t port;

    unsigned ts_ref_freq;
    unsigned ts_idx;

    /* values for 500MHz TSPLL output-f(vco)=3500, chan=0, f(out)=500*/
    static const soc_pll_param_t ts_pll[] = {
    /*     Fref,  Ndiv_int, Ndiv_frac,  Pdiv, Mdiv, Ka, Ki, Kp, VCO_div2 */
        {12800000,     625,         0,     2,    8,  0,  2, 3,        1},
        {20000000,     175,         0,     1,    7,  0,  2, 3,        1},
        {25000000,     140,         0,     1,    7,  0,  2, 3,        1},
        {32000000,     125,         0,     1,    8,  0,  2, 3,        1},
        {50000000,      70,         0,     1,    7,  0,  2, 3,        1},
        {       0,      70,         0,     1,    7,  0,  2, 3,        1}
    };
    unsigned bs_ref_freq;
    unsigned bs_idx;
    uint32 bs_ndiv_high, bs_ndiv_low;
    /* values for 20MHz BSPLL output-f(vco) = 3100,channel = 1,f(out) = 20*/
    /* 25MHZ have frequency doubler on */
    static const soc_pll_param_t bs_pll[] = {
    /*     Fref,  Ndiv_int,  Ndiv_frac,  Pdiv, Mdiv, Ka, Ki, Kp, VCO_div2 */
        {12800000,      242,   3145728,     1,  155,  0,  0, 0,        0},
        {20000000,      155,         0,     1,  155,  0,  0, 0,        0},
        {25000000,      62,          0,     1,  155,  0,  0, 0,        0},
        {32000000,      96,   14680064,     1,  155,  0,  0, 0,        0},
        {50000000,      62,          0,     1,  155,  0,  0, 0,        0},
        {       0,      62,          0,     1,  155,  0,  0, 0,        0}
    };

    soc_cm_get_id(unit, &dev_id, &rev_id);

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

    /*
     * SBUS ring and block number:
     * Ring0: cmic -> ip-ep -> cmic
     * Ring1: cmic -> mxqport0 -> mxqport1-> mxqport2 -> mxqport3-> cmic
     * Ring2: cmic -> gport0 -> gport1 -> gport2 -> cmic
     * Ring3: cmic -> otpc -> top -> cmic
     * Ring4: cmic -> mmu0 -> mmu1 -> cmic
     * Ring5: cmic -> ci0 -> ci1 -> ci2 -> lls -> cmic
     * Ring6: cmic -> ces -> cmic
     * Ring7: unused
     */
    WRITE_CMIC_SBUS_RING_MAP_0_7r(unit, 0x66034000); /* block 7  - 0 */
    WRITE_CMIC_SBUS_RING_MAP_8_15r(unit, 0x55312222); /* block 15 - 8 */
    WRITE_CMIC_SBUS_RING_MAP_16_23r(unit, 0x03103775); /* block 23 - 16 */
    WRITE_CMIC_SBUS_RING_MAP_24_31r(unit, 0x0); /* block 31 - 24 */

    SOC_IF_ERROR_RETURN(READ_TOP_XGXS0_PLL_CONTROL_4r(unit, &rval));
    soc_reg_field_set(unit, TOP_XGXS0_PLL_CONTROL_4r, &rval,
            CH1_CLK_ENABLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_XGXS0_PLL_CONTROL_4r(unit, rval));

    /* Reset all mHost processors */
    SOC_IF_ERROR_RETURN(WRITE_MHOST_0_CR5_RST_CTRLr(unit, 0));
    /* uC-1 is removed in Dagger-2 variant */
    if (dev_id != BCM56233_DEVICE_ID) {
        SOC_IF_ERROR_RETURN(WRITE_RTS_MHOST_1_CR5_RST_CTRLr(unit, 0));
    }
    SOC_IF_ERROR_RETURN(WRITE_RTS_MHOST_2_CR5_RST_CTRLr(unit, 0));
    /*Reset the common UARTs */
    if (!(soc_cm_get_bus_type(unit) & SOC_AXI_DEV_TYPE)) {
        SOC_IF_ERROR_RETURN(WRITE_CHIPCOMMONG_UART0_UART_SRRr(unit, 1));
    }
    SOC_IF_ERROR_RETURN(WRITE_CHIPCOMMONG_UART1_UART_SRRr(unit, 1));

    /* Settings for PLLs */
    /* iProc DDR LCPLL */
    /* Configure IPROC DDR PLL only in case of EP mode. */
    if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) {
        rval = 0x00805004;
        SOC_IF_ERROR_RETURN(WRITE_IPROC_WRAP_IPROC_DDR_PLL_CTRL_2r(unit,
                    rval));

        SOC_IF_ERROR_RETURN(READ_IPROC_WRAP_IPROC_DDR_PLL_CTRL_6r(unit,
                    &rval));
        soc_reg_field_set(unit, IPROC_WRAP_IPROC_DDR_PLL_CTRL_6r, &rval,
                LDO_CTRLf, 0x22);
        SOC_IF_ERROR_RETURN(WRITE_IPROC_WRAP_IPROC_DDR_PLL_CTRL_6r(unit,
                    rval));

        SOC_IF_ERROR_RETURN(READ_IPROC_WRAP_IPROC_DDR_PLL_CTRL_3r(unit,
                    &rval));
        soc_reg_field_set(unit, IPROC_WRAP_IPROC_DDR_PLL_CTRL_3r, &rval,
                CP1f, 0x1);
        soc_reg_field_set(unit, IPROC_WRAP_IPROC_DDR_PLL_CTRL_3r, &rval,
                CPf, 0x1);
        soc_reg_field_set(unit, IPROC_WRAP_IPROC_DDR_PLL_CTRL_3r, &rval,
                CZf, 0x3);
        soc_reg_field_set(unit, IPROC_WRAP_IPROC_DDR_PLL_CTRL_3r, &rval,
                RPf, 0);
        soc_reg_field_set(unit, IPROC_WRAP_IPROC_DDR_PLL_CTRL_3r, &rval,
                RZf, 0x2);
        soc_reg_field_set(unit, IPROC_WRAP_IPROC_DDR_PLL_CTRL_3r, &rval,
                ICPf, 0x14);
        soc_reg_field_set(unit, IPROC_WRAP_IPROC_DDR_PLL_CTRL_3r, &rval,
                VCO_GAINf, 0xF);
        SOC_IF_ERROR_RETURN(WRITE_IPROC_WRAP_IPROC_DDR_PLL_CTRL_3r(unit,
                    rval));

        /* Issuing Reset to iProc DDR LCPLL with SW override */
        SOC_IF_ERROR_RETURN(READ_IPROC_WRAP_IPROC_DDR_PLL_CTRL_0r(unit,
                    &rval));
        soc_reg_field_set(unit, IPROC_WRAP_IPROC_DDR_PLL_CTRL_0r, &rval,
                IPROC_DDR_PLL_SW_OVWRf, 0x1);
        SOC_IF_ERROR_RETURN(WRITE_IPROC_WRAP_IPROC_DDR_PLL_CTRL_0r(unit,
                    rval));
        soc_reg_field_set(unit, IPROC_WRAP_IPROC_DDR_PLL_CTRL_0r, &rval,
                IPROC_DDR_PLL_RESETBf, 0);
        soc_reg_field_set(unit, IPROC_WRAP_IPROC_DDR_PLL_CTRL_0r, &rval,
                IPROC_DDR_PLL_POST_RESETBf, 0);
        SOC_IF_ERROR_RETURN(WRITE_IPROC_WRAP_IPROC_DDR_PLL_CTRL_0r(unit,
                    rval));

        sal_usleep(to_usec);
        SOC_IF_ERROR_RETURN(READ_IPROC_WRAP_IPROC_DDR_PLL_CTRL_0r(unit,
                    &rval));
        soc_reg_field_set(unit, IPROC_WRAP_IPROC_DDR_PLL_CTRL_0r, &rval,
                IPROC_DDR_PLL_RESETBf, 1);
        SOC_IF_ERROR_RETURN(WRITE_IPROC_WRAP_IPROC_DDR_PLL_CTRL_0r(unit,
                    rval));
        sal_usleep(to_usec);

        if (!SAL_BOOT_SIMULATION) {
            SOC_IF_ERROR_RETURN(READ_IPROC_WRAP_IPROC_DDR_PLL_STATUSr(unit, &rval));

            if (!soc_reg_field_get(unit, IPROC_WRAP_IPROC_DDR_PLL_STATUSr,
                        rval, IPROC_DDR_PLL_LOCKf)) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                           (BSL_META_U(unit,
                           "IPROC DDR not locked on unit %d "
                           "status = 0x%08x\n"), unit, rval));
            }
        }

        SOC_IF_ERROR_RETURN(READ_IPROC_WRAP_IPROC_DDR_PLL_CTRL_0r(unit,
                    &rval));
        soc_reg_field_set(unit, IPROC_WRAP_IPROC_DDR_PLL_CTRL_0r, &rval,
                IPROC_DDR_PLL_POST_RESETBf, 1);
        SOC_IF_ERROR_RETURN(WRITE_IPROC_WRAP_IPROC_DDR_PLL_CTRL_0r(unit,
                    rval));
        soc_reg_field_set(unit, IPROC_WRAP_IPROC_DDR_PLL_CTRL_0r, &rval,
                IPROC_DDR_PLL_SW_OVWRf, 0x0);
        SOC_IF_ERROR_RETURN(WRITE_IPROC_WRAP_IPROC_DDR_PLL_CTRL_0r(unit,
                    rval));
    }

    /* Master LCPLL */
    rval = 0x00805004;
    SOC_IF_ERROR_RETURN(WRITE_TOP_XGXS0_PLL_CONTROL_3r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_XGXS0_PLL_CONTROL_7r(unit, &rval));
    soc_reg_field_set(unit, TOP_XGXS0_PLL_CONTROL_7r, &rval,
            LDO_CTRLf, 0x22);
    SOC_IF_ERROR_RETURN(WRITE_TOP_XGXS0_PLL_CONTROL_7r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_XGXS0_PLL_CONTROL_4r(unit, &rval));
    soc_reg_field_set(unit, TOP_XGXS0_PLL_CONTROL_4r, &rval,
            CP1f, 0x1);
    soc_reg_field_set(unit, TOP_XGXS0_PLL_CONTROL_4r, &rval,
            CPf, 0x3);
    soc_reg_field_set(unit, TOP_XGXS0_PLL_CONTROL_4r, &rval,
            CZf, 0x3);
    soc_reg_field_set(unit, TOP_XGXS0_PLL_CONTROL_4r, &rval,
            RPf, 0x7);
    soc_reg_field_set(unit, TOP_XGXS0_PLL_CONTROL_4r, &rval,
            RZf, 0x2);
    soc_reg_field_set(unit, TOP_XGXS0_PLL_CONTROL_4r, &rval,
            ICPf, 0xA);
    soc_reg_field_set(unit, TOP_XGXS0_PLL_CONTROL_4r, &rval,
            VCO_GAINf, 0xF);
    SOC_IF_ERROR_RETURN(WRITE_TOP_XGXS0_PLL_CONTROL_4r(unit, rval));

   /* Serdes LCPLL */
    rval = 0x197d0714;
    SOC_IF_ERROR_RETURN(WRITE_TOP_XGXS1_PLL_CONTROL_1r(unit, rval));

    rval = 0x40004015;
    SOC_IF_ERROR_RETURN(WRITE_TOP_XGXS1_PLL_CONTROL_2r(unit, rval));

    rval = 0x897004;
    SOC_IF_ERROR_RETURN(WRITE_TOP_XGXS1_PLL_CONTROL_3r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_XGXS1_PLL_CONTROL_7r(unit, &rval));
    soc_reg_field_set(unit, TOP_XGXS1_PLL_CONTROL_7r, &rval,
            LDO_CTRLf, 0x22);
    SOC_IF_ERROR_RETURN(WRITE_TOP_XGXS1_PLL_CONTROL_7r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_XGXS1_PLL_CONTROL_4r(unit, &rval));
    soc_reg_field_set(unit, TOP_XGXS1_PLL_CONTROL_4r, &rval,
            CP1f, 0x1);
    soc_reg_field_set(unit, TOP_XGXS1_PLL_CONTROL_4r, &rval,
            CPf, 0x3);
    soc_reg_field_set(unit, TOP_XGXS1_PLL_CONTROL_4r, &rval,
            CZf, 0x3);
    soc_reg_field_set(unit, TOP_XGXS1_PLL_CONTROL_4r, &rval,
            RPf, 0x3);
    soc_reg_field_set(unit, TOP_XGXS1_PLL_CONTROL_4r, &rval,
            RZf, 0);
    soc_reg_field_set(unit, TOP_XGXS1_PLL_CONTROL_4r, &rval,
            ICPf, 0xA);
    soc_reg_field_set(unit, TOP_XGXS1_PLL_CONTROL_4r, &rval,
            VCO_GAINf, 0xF);
    SOC_IF_ERROR_RETURN(WRITE_TOP_XGXS1_PLL_CONTROL_4r(unit, rval));

   /* Both BSPLLs configured the same, for 20MHz output by default */
    /* 50MHz is default.  0->internal reference */
    bs_ref_freq = soc_property_get(unit, spn_PTP_BS_FREF, 0);

    /* Set BS_PLL_CLK_IN_SEL based on reference frequency. If it is 0, use the
     * internal reference
     */
    for (bs_idx = 0; bs_idx < sizeof(bs_pll)/sizeof(bs_pll[0]); ++bs_idx) {
        if (bs_pll[bs_idx].ref_freq == bs_ref_freq) {
            break;
        }
    }

    if (bs_idx == sizeof(bs_pll)/sizeof(bs_pll[0])) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                    "Invalid value for PTP_BS_REF (%u).  No default PLL params. Resetting to internal clock\n"),
                    bs_ref_freq));
        bs_idx = bs_idx - 1;
    }

    /* bs pll - set reference clock */

    SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r(unit,&rval));
    soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, BS0_PLL_CLK_IN_SELf,
                      (bs_ref_freq != 0));
    soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, BS1_PLL_CLK_IN_SELf,
                      (bs_ref_freq != 0));
    WRITE_TOP_MISC_CONTROL_1r(unit, rval);

    bs_ndiv_high = ((soc_property_get(unit, spn_PTP_BS_NDIV_INT, bs_pll[bs_idx].ndiv_int) << 6) |
        ((soc_property_get(unit, spn_PTP_BS_NDIV_FRAC, bs_pll[bs_idx].ndiv_frac) >> 18) & 0x3f));

    bs_ndiv_low = (soc_property_get(unit, spn_PTP_BS_NDIV_FRAC, bs_pll[bs_idx].ndiv_frac) << 14);

    /* Broad Sync0 LCPLL */
    rval = 0x00805004;
    SOC_IF_ERROR_RETURN(WRITE_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_3r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_6r(unit,
                &rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_6r, &rval,
            LDO_CTRLf, 0x22);
    SOC_IF_ERROR_RETURN(WRITE_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_6r(unit,
                rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_4r(unit,
                &rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_4r, &rval,
            CP1f, 0x1);
    soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_4r, &rval,
            CPf, 0x3);
    soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_4r, &rval,
            CZf, 0x3);
    soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_4r, &rval,
            RPf, 0x7);
    soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_4r, &rval,
            RZf, 0x2);
    soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_4r, &rval,
            ICPf, 0xA);
    soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_4r, &rval,
            VCO_GAINf, 0xF);
    SOC_IF_ERROR_RETURN(WRITE_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_4r(unit,
                rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r(unit,&rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r, &rval,
                      BROAD_SYNC0_LCPLL_FBDIV_1f, bs_ndiv_high);
    WRITE_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r(unit, rval);

    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_0r(unit,&rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_0r, &rval,
                      BROAD_SYNC0_LCPLL_FBDIV_0f, bs_ndiv_low);
    WRITE_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_0r(unit, rval);

    /* for BS , use channel 1 */
    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_1r(unit,&rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_1r, &rval,
                      CH1_MDIVf, soc_property_get(unit, spn_PTP_BS_MNDIV,
                      bs_pll[bs_idx].mdiv));
    WRITE_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_1r(unit, rval);

    /* for 25MHz, set bit 13 to enable frequency doubler */
    if (bs_ref_freq ==  25000000) {
        SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_3r(unit,&rval));
        rval |= (1<<13);
        WRITE_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_3r(unit, rval);
    }

   /* Broad Sync1 LCPLL */
    rval = 0x00805004;
    SOC_IF_ERROR_RETURN(WRITE_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_3r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_6r(unit,
                &rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_6r, &rval,
            LDO_CTRLf, 0x22);
    SOC_IF_ERROR_RETURN(WRITE_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_6r(unit,
                rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_4r(unit,
                &rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_4r, &rval,
            CP1f, 0x1);
    soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_4r, &rval,
            CPf, 0x3);
    soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_4r, &rval,
            CZf, 0x3);
    soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_4r, &rval,
            RPf, 0x7);
    soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_4r, &rval,
            RZf, 0x2);
    soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_4r, &rval,
            ICPf, 0xA);
    soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_4r, &rval,
            VCO_GAINf, 0xF);
    SOC_IF_ERROR_RETURN(WRITE_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_4r(unit,
                rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r(unit,&rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r, &rval,
                      BROAD_SYNC1_LCPLL_FBDIV_1f, bs_ndiv_high);
    WRITE_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r(unit, rval);

    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_0r(unit,&rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_0r, &rval,
                      BROAD_SYNC1_LCPLL_FBDIV_0f, bs_ndiv_low);
    WRITE_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_0r(unit, rval);

    SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_1r(unit,&rval));
    soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_1r, &rval,
                      CH1_MDIVf, soc_property_get(unit, spn_PTP_BS_MNDIV,
                      bs_pll[bs_idx].mdiv));
    WRITE_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_1r(unit, rval);

    /* for 25MHz, set bit 13 to enable frequency doubler */
    if (bs_ref_freq ==  25000000) {
        SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_3r(unit,&rval));
        rval |= (1<<13);
        WRITE_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_3r(unit, rval);
    }

    /* Top Serdes LCPLL settings */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_TOP_SERDES_LCPLL_FBDIV_CTRL_0r(unit, rval));
    rval = 0x7d0;
    SOC_IF_ERROR_RETURN(WRITE_TOP_SERDES_LCPLL_FBDIV_CTRL_1r(unit, rval));

    /* Top Master LCPLL settings to default reset values */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_TOP_MASTER_LCPLL_FBDIV_CTRL_0r(unit, rval));
    rval = 0xfa0;
    SOC_IF_ERROR_RETURN(WRITE_TOP_MASTER_LCPLL_FBDIV_CTRL_1r(unit, rval));

    /* End of PLL settings. Togle LCPLL soft reset. */
    SOC_IF_ERROR_RETURN(READ_TOP_LCPLL_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_LCPLL_SOFT_RESET_REGr, &rval,
            LCPLL_SOFT_RESETf, 0x1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_LCPLL_SOFT_RESET_REGr(unit, rval));

    sal_usleep(to_usec);
    soc_reg_field_set(unit, TOP_LCPLL_SOFT_RESET_REGr, &rval,
            LCPLL_SOFT_RESETf, 0x0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_LCPLL_SOFT_RESET_REGr(unit, rval));

    sal_usleep(to_usec);

    /* Wait for LCPLL lock */
    if (!SAL_BOOT_SIMULATION) {
        SOC_IF_ERROR_RETURN(READ_TOP_XGXS0_PLL_STATUSr(unit, &rval));

        if (!soc_reg_field_get(unit, TOP_XGXS0_PLL_STATUSr, rval, TOP_XGPLL0_LOCKf)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                       (BSL_META_U(unit,
                       "LCPLL 0 not locked on unit %d "
                       "status = 0x%08x\n"), unit, rval));
        }
        SOC_IF_ERROR_RETURN(READ_TOP_XGXS1_PLL_STATUSr(unit, &rval));
        if (!soc_reg_field_get(unit, TOP_XGXS1_PLL_STATUSr, rval, TOP_XGPLL1_LOCKf)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                     "LCPLL 1 not locked on unit %d "
                     "status = 0x%08x\n"), unit, rval));
        }
    }


    /* Configure TS PLL */
    /* 50MHz is default ->internal reference */
    ts_ref_freq = soc_property_get(unit, spn_PTP_TS_PLL_FREF, 0);

    /* Configure TS PLL */
    for (ts_idx = 0; ts_idx < sizeof(ts_pll)/sizeof(ts_pll[0]); ++ts_idx) {
        if (ts_pll[ts_idx].ref_freq == ts_ref_freq) {
            break;
        }
    }
    if (ts_idx == sizeof(ts_pll)/sizeof(ts_pll[0])) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Invalid value for PTP_TS_PLL_REF (%u).  No default PLL params. Using internal clock \n"),
                              ts_ref_freq));
        /* resetting back to internal clock */
        ts_idx = ts_idx - 1;
    }


    SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r(unit,&rval));
    if (soc_property_get(unit, spn_PTP_TS_PLL_FREF, 0) == 0) {
        soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, TS_PLL_CLK_IN_SELf, 0);
    } else {
        soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, TS_PLL_CLK_IN_SELf, 1);
    }
    WRITE_TOP_MISC_CONTROL_1r(unit, rval);

    /* 250Mhz TS PLL implies 4ns resolution */
    if (SOC_REG_IS_VALID(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r)) {
        SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r(unit,&rval));
        soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r, &rval,
                NDIV_FRACf, ts_pll[ts_idx].ndiv_frac);
        WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r(unit, rval);
    }

    if (SOC_REG_IS_VALID(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r)) {
        SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r(unit,&rval));
        soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r, &rval, PDIVf,
               soc_property_get(unit, spn_PTP_TS_PLL_PDIV, ts_pll[ts_idx].pdiv));
        WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r(unit, rval);
    }

    if (SOC_REG_IS_VALID(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_3r)) {
        SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_3r(unit,&rval));
        soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_3r, &rval, NDIV_INTf,
               soc_property_get(unit, spn_PTP_TS_PLL_N,
                                ts_pll[ts_idx].ndiv_int));
        WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_3r(unit, rval);
    }

    if (SOC_REG_IS_VALID(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r)) {
        SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r(unit,&rval));
        soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r, &rval, CH0_MDIVf,
                soc_property_get(unit, spn_PTP_TS_PLL_MNDIV,
                                 ts_pll[ts_idx].mdiv));
        WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r(unit, rval);
    }

    if (SOC_REG_IS_VALID(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r)) {
        SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r(unit,&rval));
        soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r, &rval, CH1_MDIVf,
                soc_property_get(unit, spn_PTP_TS_PLL_MNDIV,
                                 ts_pll[ts_idx].mdiv));
        WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r(unit, rval);
    }

    SOC_TIMESYNC_PLL_CLOCK_NS(unit) = (1/250 * 1000); /* clock period in nanoseconds */

    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_TS_PLL_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_3r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_BS0_PLL_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_BS1_PLL_RST_Lf, 1);
    WRITE_TOP_SOFT_RESET_REG_3r(unit, rval);

    sal_usleep(to_usec);

    if (!SAL_BOOT_SIMULATION) {
        if (soc_feature(unit, soc_feature_ces)) {
            SOC_IF_ERROR_RETURN(READ_TOP_CES_PLL_STATUSr(unit, &rval));
            if (!soc_reg_field_get(unit, TOP_CES_PLL_STATUSr, rval, TOP_CES_PLL_LOCKf)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                         "CES PLL not locked on unit %d "
                         "status = 0x%08x\n"), unit, rval));
            }
        }
        SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_STATUSr(unit, &rval));
        if (!soc_reg_field_get(unit, TOP_TIME_SYNC_PLL_STATUSr, rval,
                               TOP_TIME_SYNC_PLL_LOCKf)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                         "TimeSync PLL not locked on unit %d "
                         "status = 0x%08x\n"), unit, rval));
        }

        if (SOC_REG_IS_VALID(unit, TOP_BROAD_SYNC0_PLL_STATUSr)) {
            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_PLL_STATUSr(unit, &rval));
            if (!soc_reg_field_get(unit, TOP_BROAD_SYNC0_PLL_STATUSr, rval,
                        TOP_BROAD_SYNC0_PLL_LOCKf)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                          "BroadSync0 PLL not locked on unit %d "
                          "status = 0x%08x\n"), unit, rval));
            }
        }

        if (SOC_REG_IS_VALID(unit, TOP_BROAD_SYNC1_PLL_STATUSr)) {
            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_PLL_STATUSr(unit, &rval));
            if (!soc_reg_field_get(unit, TOP_BROAD_SYNC1_PLL_STATUSr, rval,
                        TOP_BROAD_SYNC1_PLL_LOCKf)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                          "BroadSync1 PLL not locked on unit %d "
                          "status = 0x%08x\n"), unit, rval));
            }
        }
    }

    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_TS_PLL_POST_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_3r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_BS0_PLL_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_BS1_PLL_POST_RST_Lf, 1);
    WRITE_TOP_SOFT_RESET_REG_3r(unit, rval);


    /* Disable differential testout buffers to save power */
    SOC_IF_ERROR_RETURN(READ_TOP_CORE_PLL0_CTRL_REGISTER_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_CORE_PLL0_CTRL_REGISTER_1r, &rval,
            TESTOUT_ENf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_CORE_PLL0_CTRL_REGISTER_1r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_1r, &rval,
            TESTOUT_ENf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_1r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_IPROC_WRAP_IPROC_PLL_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, IPROC_WRAP_IPROC_PLL_CTRL_1r, &rval,
            TESTOUT_ENf, 0);
    SOC_IF_ERROR_RETURN(WRITE_IPROC_WRAP_IPROC_PLL_CTRL_1r(unit, rval));

    /* SKU specific power savings */
    if ((dev_id == BCM56461_DEVICE_ID) || (dev_id == BCM56466_DEVICE_ID) ||
            (dev_id == BCM56261_DEVICE_ID) || (dev_id == BCM56266_DEVICE_ID)) {
        /* Disable USB PHY in CPU disabled SKUs */
        SOC_IF_ERROR_RETURN(READ_IPROC_WRAP_IPROC_STRAP_CTRLr(unit, &rval));
        soc_reg_field_set(unit, IPROC_WRAP_IPROC_STRAP_CTRLr, &rval,
                DISABLE_USB2Hf, 1);
        soc_reg_field_set(unit, IPROC_WRAP_IPROC_STRAP_CTRLr, &rval,
                DISABLE_USB2Df, 1);
        SOC_IF_ERROR_RETURN(WRITE_IPROC_WRAP_IPROC_STRAP_CTRLr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_IPROC_WRAP_USBPHY_CTRL_0r(unit, &rval));
        soc_reg_field_set(unit, IPROC_WRAP_USBPHY_CTRL_0r, &rval, RESETBf, 0);
        soc_reg_field_set(unit, IPROC_WRAP_USBPHY_CTRL_0r, &rval,
                PLL_RESETBf, 0);
        SOC_IF_ERROR_RETURN(WRITE_IPROC_WRAP_USBPHY_CTRL_0r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_IPROC_WRAP_IPROC_DDR_PLL_CTRL_3r(unit,
                    &rval));
        soc_reg_field_set(unit, IPROC_WRAP_IPROC_DDR_PLL_CTRL_3r, &rval,
                CH2_CLK_ENABLEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_IPROC_WRAP_IPROC_DDR_PLL_CTRL_3r(unit,
                    rval));
    }

    /* Disable IPROC DDR when Ext packet buffer is disabled.
     * But keep iproc enabled when iHost is supported.
     */
    if ((dev_id == BCM56466_DEVICE_ID) || (dev_id == BCM56266_DEVICE_ID)) {
        SOC_IF_ERROR_RETURN(READ_IPROC_WRAP_IPROC_STRAP_CTRLr(unit, &rval));
        soc_reg_field_set(unit, IPROC_WRAP_IPROC_STRAP_CTRLr, &rval,
                DISABLE_IPROC_DDRf, 1);
        SOC_IF_ERROR_RETURN(WRITE_IPROC_WRAP_IPROC_STRAP_CTRLr(unit, rval));
    }

    /* Disable iHost in SKUs that does not support iHost */
    if ((dev_id == BCM56461_DEVICE_ID) || (dev_id == BCM56466_DEVICE_ID) ||
            (dev_id == BCM56261_DEVICE_ID) || (dev_id == BCM56266_DEVICE_ID)) {

        SOC_IF_ERROR_RETURN(READ_DMU_PCU_IPROC_CONTROLr(unit, &rval));
        /* Check if iHost is not powered down, then power it down */
        if (soc_reg_field_get(unit,
                        DMU_PCU_IPROC_CONTROLr, rval, IHOST_PWRCTRLI0f) != 0x3)
        {

            SOC_IF_ERROR_RETURN(READ_IHOST_SCU_POWER_STATUSr(unit, &rval));
            soc_reg_field_set(unit, IHOST_SCU_POWER_STATUSr, &rval,
                CPU0_STATUSf, 3);
            SOC_IF_ERROR_RETURN(WRITE_IHOST_SCU_POWER_STATUSr(unit, rval));

            SOC_IF_ERROR_RETURN(READ_DMU_PCU_IPROC_CONTROLr(unit, &rval));
            soc_reg_field_set(unit, DMU_PCU_IPROC_CONTROLr, &rval,
                IHOST_PWRCTRLI0f, 3);
            SOC_IF_ERROR_RETURN(WRITE_DMU_PCU_IPROC_CONTROLr(unit, rval));

            SOC_IF_ERROR_RETURN(READ_CRU_CPU0_POWERDOWNr(unit, &rval));
            soc_reg_field_set(unit, CRU_CPU0_POWERDOWNr, &rval,
                WAIT_FOR_WFIf, 1);
            soc_reg_field_set(unit, CRU_CPU0_POWERDOWNr, &rval,
                START_CPU0_POWERDOWN_SEQf, 1);
            SOC_IF_ERROR_RETURN(WRITE_CRU_CPU0_POWERDOWNr(unit, rval));
        }
     }
    /* Bring port blocks out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ0_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ1_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ2_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ3_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ4_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ5_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_PM_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));

    sal_usleep(to_usec);
    /* Bring network sync out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ0_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ1_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ2_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ3_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ4_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MXQ5_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_PM_HOTSWAP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_NS_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));

    sal_usleep(to_usec);

    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_SBUS_TIMEOUTr(unit, 0x7d0));

    SOC_IF_ERROR_RETURN(READ_TOP_MMU_DDR_PHY_CTRLr(unit, &rval));
    soc_reg_field_set(unit, TOP_MMU_DDR_PHY_CTRLr, &rval, I_PWRONIN_PHYf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MMU_DDR_PHY_CTRLr(unit, rval));
    sal_usleep(1000);

    SOC_IF_ERROR_RETURN(READ_TOP_MMU_DDR_PHY_CTRLr(unit, &rval));
    soc_reg_field_set(unit, TOP_MMU_DDR_PHY_CTRLr, &rval, I_PWROKIN_PHYf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MMU_DDR_PHY_CTRLr(unit, rval));
    sal_usleep(1000);

    SOC_IF_ERROR_RETURN(READ_TOP_MMU_DDR_PHY_CTRLr(unit, &rval));
    soc_reg_field_set(unit, TOP_MMU_DDR_PHY_CTRLr, &rval, I_ISO_PHY_DFIf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MMU_DDR_PHY_CTRLr(unit, rval));
    sal_usleep(1000);

    rval = 0;
    soc_reg_field_set(unit, TOP_CI_PHY_CONTROLr, &rval, DDR_RESET_Nf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_CI_PHY_CONTROLr(unit, rval));
    sal_usleep(1000);

    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_AVS_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_OOBFC0_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_OOBFC1_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));

    /* Bring IP, EP, and MMU blocks out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_EP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_IP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MMU_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));

    /* IPIPE and EPIPE configurations */
    SOC_IF_ERROR_RETURN(soc_saber2_hw_reset_control_init(unit));

    sal_usleep(to_usec);
    if (soc_feature(unit, soc_feature_ddr3)) {
        ddr3_clock_mhz = 800;

        SOC_DDR3_NUM_COLUMNS(unit) = soc_property_get(unit, spn_EXT_RAM_COLUMNS,
                                                      1024);
        SOC_DDR3_NUM_BANKS(unit) = soc_property_get(unit,spn_EXT_RAM_BANKS, 8);
        SOC_DDR3_NUM_MEMORIES(unit) = soc_property_get(unit,spn_EXT_RAM_PRESENT,
                                                       0);
        SOC_DDR3_NUM_ROWS(unit) = soc_property_get(unit,spn_EXT_RAM_ROWS,
                                                       num_rows);
        SOC_DDR3_CLOCK_MHZ(unit) = soc_property_get(unit, spn_DDR3_CLOCK_MHZ,
                                                       ddr3_clock_mhz);
        SOC_DDR3_MEM_GRADE(unit) = soc_property_get(unit, spn_DDR3_MEM_GRADE,
                                                       ddr3_mem_grade);

        ext_pbmp = soc_property_get_pbmp(unit, spn_PBMP_EXT_MEM, 0);
        SOC_PBMP_COUNT(ext_pbmp, ext_pbmp_count);
        if (SOC_DDR3_NUM_MEMORIES(unit)) {
            SOC_IF_ERROR_RETURN(soc_sb2_and28_dram_init_reset(unit));
        }
    }

    for (port = 1; port < 24; port +=4 ) {
        if (SOC_PORT_VALID(unit, port)) {
            SOC_IF_ERROR_RETURN(READ_XPORT_XGXS_CTRLr(unit, port, &rval));
            soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, PWRDWNf, 0);
            SOC_IF_ERROR_RETURN(WRITE_XPORT_XGXS_CTRLr(unit, port, rval));
            soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, RSTB_HWf, 1);
            SOC_IF_ERROR_RETURN(WRITE_XPORT_XGXS_CTRLr(unit, port, rval));
            soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, IDDQf, 0);
            SOC_IF_ERROR_RETURN(WRITE_XPORT_XGXS_CTRLr(unit, port, rval));
            soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, RSTB_PLLf, 1);
            SOC_IF_ERROR_RETURN(WRITE_XPORT_XGXS_CTRLr(unit, port, rval));
            soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, RSTB_MDIOREGSf, 1);
            SOC_IF_ERROR_RETURN(WRITE_XPORT_XGXS_CTRLr(unit, port, rval));
            soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, TXD1G_FIFO_RSTBf, 0xf);
            SOC_IF_ERROR_RETURN(WRITE_XPORT_XGXS_CTRLr(unit, port, rval));
            soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, TXD10G_FIFO_RSTBf, 1);
            SOC_IF_ERROR_RETURN(WRITE_XPORT_XGXS_CTRLr(unit, port, rval));
        }
    }

    return SOC_E_NONE;
}
#endif  /* BCM_SABER2_SUPPORT */

int soc_esw_schan_fifo_init(int unit, int ccmdma)
{
    int rval = SOC_E_NONE;

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        soc_control_t           *soc;
        soc_schan_fifo_config_t cfg;
        int rval;
#ifdef BCM_SOC_ASYNC_SUPPORT
        soc_async_prop_t  prop;

        prop.prio = 40;
        prop.q_size = 50;
        prop.type = p_schan_fifo;
        prop.threads = 1;
#endif
        soc = SOC_CONTROL(unit);
        soc_cmic_intr_all_disable(unit);

        /* Initialize schan fifo */
        /* Read properties */
        soc->schanFifoTimeout =
               soc_property_get(unit, spn_SCHANFIFO_TIMEOUT_USEC,
                                                30000000);
        soc->schanFifoIntrEnb =
               soc_property_get(unit, spn_SCHANFIFO_INTR_ENABLE, 1);

        cfg.intrEnb = soc->schanFifoIntrEnb;
        cfg.dma = ccmdma;
        cfg.timeout = soc->schanFifoTimeout;

#ifdef BCM_SOC_ASYNC_SUPPORT
        rval = soc_schan_fifo_init(unit, &prop, &cfg);
#else
        rval = soc_schan_fifo_init(unit, NULL, &cfg);
#endif
        if (rval != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Schan FIFO Init failed \n")));
            return SOC_E_FAIL;
        }
    }
#endif /* BCM_CMICX_SUPPORT */

return rval;

}

/*
 * Function:
 *      soc_reset_fast_reboot
 * Purpose:
 *      Reset parts of the chip that are required for fast reboot
 */
int
soc_reset_fast_reboot(int unit)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        SOC_IF_ERROR_RETURN(soc_tomahawk3_chip_reset_fast_reboot(unit));
    }
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_reset
 * Purpose:
 *      Reset the unit via the CMIC CPS reset function.
 */

int
soc_reset(int unit)
{
    uint16      dev_id;
    uint8       rev_id;
    int         spl;
    char *mode = NULL;

    if (SOC_IS_RCPU_ONLY(unit)) {
        /* Skip reset process for rcpu only unit */
        return SOC_E_NONE;
    }

    /*
     * Configure endian mode in case the system just came out of reset.
     */

    soc_endian_config(unit);
    soc_pci_ep_config(unit, -1);

    /*
     * Configure bursting in case the system just came out of reset.
     */

    soc_pci_burst_enable(unit);


    /*
     * After setting the reset bit, StrataSwitch PCI registers cannot be
     * accessed for 300 cycles or the CMIC will hang.  This is mostly of
     * concern on the Quickturn simulation but we leave plenty of time.
     */

    soc_cm_get_id(unit, &dev_id, &rev_id);

    /* Suspend IRQ polling while resetting */
    soc_ipoll_pause(unit);

    if (soc_property_get(unit, spn_SOC_SKIP_RESET, 0)) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "soc_init: skipping hard reset\n")));
    }
    else
    {
        /* CPS reset problematic on PLI, the amount of time
         * to wait is not constant and may vary from machine
         * to machine. So, don't do it.
         */
        if (!SAL_BOOT_RTLSIM) {
#ifdef BCM_TOMAHAWK_SUPPORT
            if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
                uint32  rval, xrval;
                int     bn = 0;
                uint32  top_tsc_enable_default = 0xffffffff;

                /*CMIC_SBUS_RING_MAP_0_7 is used to check whether SBUS was configured or not*/
                SOC_IF_ERROR_RETURN(READ_CMIC_SBUS_RING_MAP_0_7r(unit, &rval));

                /* Adjust TOP_TSC_ENABLE enable/disable status only when SBUS configured
                   and the register is ready*/
                if ((rval != 0) && ((READ_TOP_TSC_ENABLEr(unit, &rval)) == SOC_E_NONE)) {
                    /*Calculate different bits number between old and ASIC default value*/
                    xrval = rval ^ top_tsc_enable_default;
                    for (bn = 0; xrval != 0; bn++) {
                        xrval &= (xrval - 1);
                    }

                    /*Do two steps configuration change for the case of different bits number beyond 10*/
                    if (bn > 10) {
                        /*Change half of those different bits to ASIC default value*/
                        bn /= 2;
                        xrval = rval ^ top_tsc_enable_default;
                        do {
                            xrval &= (xrval-1);
                        } while (bn-- > 0);

                        xrval ^= top_tsc_enable_default;
                        SOC_IF_ERROR_RETURN(WRITE_TOP_TSC_ENABLEr(unit, xrval));
                        sal_usleep(100000);
                    }
#ifdef BCM_TOMAHAWK2_SUPPORT
                    /*TH2 has 64 port macros*/
                    if (SOC_REG_IS_VALID(unit, TOP_TSC_ENABLE_1r)){
                        SOC_IF_ERROR_RETURN(READ_TOP_TSC_ENABLE_1r(unit, &rval));
                        xrval = rval ^ top_tsc_enable_default;
                        for (bn = 0; xrval != 0; bn++) {
                            xrval &= (xrval - 1);
                        }

                        /*Do two steps configuration change for the case of different bits number beyond 10*/
                        if (bn > 10) {
                            /*Change half of those different bits to ASIC default value*/
                            bn /= 2;
                            xrval = rval ^ top_tsc_enable_default;
                            do {
                                xrval &= (xrval-1);
                            } while (bn-- > 0);

                            xrval ^= top_tsc_enable_default;
                            SOC_IF_ERROR_RETURN(WRITE_TOP_TSC_ENABLE_1r(unit, xrval));
                            sal_usleep(100000);
                        }
                    }
#endif /*BCM_TOMAHAWK2_SUPPORT*/
                }
            }
#endif

#ifdef BCM_HERCULES_SUPPORT
#ifdef BCM_HERCULES15_SUPPORT
            /* special processing on 5675/6A0 with disabled ports */
            if (SOC_IS_HERCULES15(unit) &&
                rev_id == BCM5675_A0_REV_ID &&
                !SAL_BOOT_QUICKTURN &&
                SOC_PBMP_WORD_GET(PBMP_ALL(unit), 0) != 0x1ff) {
                soc_reset_bcm5675_a0(unit, dev_id);
            }
#endif
            if (SOC_IS_HERCULES(unit)) {
                /* Set clock enable on active ports before soft reset */
                WRITE_CMIC_CLK_ENABLEr(unit,
                        SOC_PBMP_WORD_GET(PBMP_ALL(unit), 0));
            }
#endif
            /* Block interrupts while setting the busy flag */
            spl = sal_splhi();
            SOC_CONTROL(unit)->soc_flags |= SOC_F_BUSY;

            /* Unblock interrupts */
            sal_spl(spl);

#ifdef BCM_CMICM_SUPPORT
            if (soc_feature(unit, soc_feature_cmicm)) {
#ifdef  BCM_TOMAHAWK_SUPPORT
/* For TH+/TH2 the behaviour of IDDQ field of CLPORT_XGXS0_CTRL_REG has changed */
/* Cannot reset all falcon cores at the same time, try to reset 8 cores at a time */
               if (soc_feature(unit, soc_feature_iddq_new_default)) {
                    uint32  rval;
                   /*CMIC_SBUS_RING_MAP_0_7 is used to check whether SBUS was configured or not*/
                   SOC_IF_ERROR_RETURN(READ_CMIC_SBUS_RING_MAP_0_7r(unit, &rval));
                    if (rval != 0) {
                        SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_3r(unit, 0xffffff00));
                        sal_usleep(500);
                        SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_3r(unit, 0xffff0000));
                        sal_usleep(500);
                        SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_3r(unit, 0xff000000));
                        sal_usleep(500);
                        SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_3r(unit, 0x000000));
                        sal_usleep(500);

                        if (SOC_IS_TOMAHAWK2(unit)) {
                            SOC_IF_ERROR_RETURN
                                (WRITE_TOP_SOFT_RESET_REG_4r(unit, 0xffffff00));
                            sal_usleep(500);
                            SOC_IF_ERROR_RETURN
                                (WRITE_TOP_SOFT_RESET_REG_4r(unit, 0xffff0000));
                            sal_usleep(500);
                            SOC_IF_ERROR_RETURN
                                (WRITE_TOP_SOFT_RESET_REG_4r(unit, 0xff000000));
                            sal_usleep(500);
                            SOC_IF_ERROR_RETURN
                                (WRITE_TOP_SOFT_RESET_REG_4r(unit, 0x000000));
                            sal_usleep(500);
                        }
                    }
                }
#endif
                soc_pci_write(unit, CMIC_CPS_RESET_OFFSET, 1);
            } else
#endif
#ifdef BCM_CMICX_SUPPORT
            if (soc_feature(unit, soc_feature_cmicx)) {
                uint32 v;
                if (SOC_IS_TOMAHAWK3(unit)) {
                    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_3r(unit, 0xffffff00));
                    sal_usleep(500);
                    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_3r(unit, 0xffff0000));
                    sal_usleep(500);
                    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_3r(unit, 0xff000000));
                    sal_usleep(500);
                    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_3r(unit, 0x000000));
                    sal_usleep(500);
                }

                if (!SAL_BOOT_SIMULATION || SAL_BOOT_QUICKTURN) {

                    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
                        /* CMICx DMA channels need to be released/aborted properly
                         * and start at clean state by calling call 'soc_cmicx_dma_abort'.
                         * sw_reset_n=0 cannot be used for iHost devices
                         */
                        SOC_IF_ERROR_RETURN(soc_cmicx_dma_abort(unit, 0));
                    } else {
                        /* To maintian compatibility with all
                         * previous devices, write '0' to
                         * SW_RESET_N of DMU_CRU_RESET
                         */
                        v = 2;
                        WRITE_DMU_CRU_RESETr(unit, v);
                    }
                }
            } else
#endif
            {
                soc_pci_write(unit, CMIC_CONFIG,
                              soc_pci_read(unit, CMIC_CONFIG) | CC_RESET_CPS);
            }

            if (soc_feature(unit, soc_feature_reset_delay) && !SAL_BOOT_QUICKTURN) {
                sal_usleep(1000000);
            } else {
                if (SAL_BOOT_QUICKTURN) {
                    sal_usleep(10 * MILLISECOND_USEC);
                } else {
                    sal_usleep(1 * MILLISECOND_USEC);
                }
            }
        }
    }

    /*
     * Sleep some extra time to allow the StrataSwitch chip to finish
     * initializing the ARL tables, which takes 60000 to 70000 cycles.
     * On PLI, sleeping 10us won't make a difference.
     */
    if (SAL_BOOT_QUICKTURN) {
        sal_usleep(250 * MILLISECOND_USEC);
    } else {
        sal_usleep(10 * MILLISECOND_USEC);
    }

    /* On some boards that run at lower freq, reading the CMIC_CONFIG
     * register for the first time after CPS reset returns 0. The SW
     * workaround is to perform an extra read.
     * Eventhough Hu is the only (first) board with this issue, this
     * could be a common Workaround..
     */
    if (!soc_feature(unit, soc_feature_cmicm) && !soc_feature(unit, soc_feature_cmicx)) {
        soc_pci_read(unit, CMIC_CONFIG);
        sal_usleep(1000);
    }

    /* Restore endian mode since the reset cleared it. */

    soc_endian_config(unit);
    soc_pci_ep_config(unit, -1);

    /* Restore bursting */

    soc_pci_burst_enable(unit);


    /* Synchronize cached interrupt mask */
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        soc_cmic_intr_all_disable(unit);
    } else
#endif
#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {
        uint32 addr, rval;
        uint32 msi_en = 1;

        if (soc_feature(unit, soc_feature_iproc) &&
            (soc_cm_get_bus_type(unit) & SOC_DEV_BUS_MSI) == 0) {
            /*
             * For iProc designs, MSI must be turned off in the CMIC
             * to prevent invalid MSI messages from corrupting host
             * CPU memory.
             */
            msi_en = 0;
        }

        addr = CMIC_CMCx_PCIE_MISCEL_OFFSET(SOC_PCI_CMC(unit));
        rval = soc_pci_read(unit, addr);

        soc_reg_field_set(unit, CMIC_CMC0_PCIE_MISCELr,
                            &rval, ENABLE_MSIf, msi_en);


        soc_pci_write(unit, addr, rval);

        soc_cmicm_intr0_disable(unit, ~0);
        soc_cmicm_intr1_disable(unit, ~0);
        soc_cmicm_intr2_disable(unit, ~0);
#ifdef BCM_CMICDV2_SUPPORT
        if (soc_feature(unit, soc_feature_cmicd_v2)) {
            soc_cmicm_intr3_disable(unit, ~0);
            soc_cmicm_intr4_disable(unit, ~0);
            soc_cmicm_intr5_disable(unit, ~0);
#ifdef BCM_CMICDV4_SUPPORT
            if (soc_feature(unit, soc_feature_cmicd_v4)) {
                soc_cmicm_intr6_disable(unit, ~0);
            }
#endif /* BCM_CMICDV4_SUPPORT */
        }
#endif /* BCM_CMICDV2_SUPPORT */
    } else
#endif
    {
        soc_intr_disable(unit, ~0);
    }

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx) &&
                    !((SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM))) {
        int ccmdma = 0;
        ccmdma = soc_property_get(unit, spn_CCM_DMA_ENABLE, 1);
        SOC_IF_ERROR_RETURN(soc_esw_schan_fifo_init(unit, ccmdma));
    }
#endif

    /* Initialize bulk mem API */
    mode = soc_property_get_str(unit, spn_SOC_MEM_BULK_SCHAN_OP_MODE);
    if (mode) {
        SOC_CONTROL(unit)->memBulkSchanPioMode =
                        (sal_strcasecmp(mode , "pio") == 0) ? 1 : 0;
    } else {
        SOC_CONTROL(unit)->memBulkSchanPioMode = 0;
    }
    SOC_IF_ERROR_RETURN(soc_mem_bulk_init(unit));

    /* Block interrupts */
    spl = sal_splhi();
    SOC_CONTROL(unit)->soc_flags &= ~SOC_F_BUSY;
    /* Unblock interrupts */
    sal_spl(spl);

    /* Resume IRQ polling if active */
    soc_ipoll_continue(unit);

#if defined(BCM_CMICM_SUPPORT)
    if (soc_feature(unit, soc_feature_cmicm)) {
        /* Set the timeout for bus-mastered DMA transactions to 50ms */
        if (SOC_REG_IS_VALID(unit, CMIC_PCIE_USERIF_TIMEOUTr) &&
            SOC_REG_FIELD_VALID(unit, CMIC_PCIE_USERIF_TIMEOUTr, TIMEOUTf)) {
            uint32  rval;

            SOC_IF_ERROR_RETURN(READ_CMIC_PCIE_USERIF_TIMEOUTr(unit, &rval));
            soc_reg_field_set(unit, CMIC_PCIE_USERIF_TIMEOUTr, &rval, TIMEOUTf,
                              50000000);
            /* coverity[result_independent_of_operands] */
            SOC_IF_ERROR_RETURN(WRITE_CMIC_PCIE_USERIF_TIMEOUTr(unit, rval));
        }
    }
#endif


#ifdef BCM_FIREBOLT_SUPPORT
#ifdef BCM_RAPTOR1_SUPPORT
    if (SOC_IS_RAPTOR(unit)) {
        soc_reset_bcm56218_a0(unit);
    } else
#endif /* BCM_RAPTOR_SUPPORT */
#ifdef BCM_RAVEN_SUPPORT
    if (SOC_IS_RAVEN(unit)) {
        soc_reset_bcm56224_a0(unit);
    } else
#endif
#ifdef BCM_HAWKEYE_SUPPORT
    if (SOC_IS_HAWKEYE(unit)) {
        soc_reset_bcm53314_a0(unit);
    } else
#endif
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit)) {
        soc_reset_bcm56634_a0(unit);
    } else
#endif
#ifdef BCM_ENDURO_SUPPORT
    if (SOC_IS_ENDURO(unit)) {
        soc_reset_bcm56334_a0(unit);
    } else
#endif
#ifdef BCM_HURRICANE1_SUPPORT
    if (SOC_IS_HURRICANE(unit)) {
        soc_reset_bcm56142_a0(unit);
    } else
#endif
#ifdef BCM_HURRICANE2_SUPPORT
    if (SOC_IS_HURRICANE2(unit)) {
        SOC_IF_ERROR_RETURN(soc_reset_bcm56150_a0(unit));
    } else
#endif
#ifdef BCM_HURRICANE3_SUPPORT
    if (SOC_IS_HURRICANE3(unit)) {
        if ((dev_id & 0xFFF0) == BCM53540_DEVICE_ID) {
            SOC_IF_ERROR_RETURN(soc_wolfhound2_chip_reset(unit));
        } else {
        SOC_IF_ERROR_RETURN(soc_hurricane3_chip_reset(unit));
        }
    } else
#endif
#ifdef BCM_GREYHOUND_SUPPORT
    if (SOC_IS_GREYHOUND(unit)) {
        SOC_IF_ERROR_RETURN(soc_greyhound_chip_reset(unit));
    } else
#endif
#ifdef BCM_GREYHOUND2_SUPPORT
    if (SOC_IS_GREYHOUND2(unit)) {
#ifdef BCM_FIRELIGHT_SUPPORT
        if ((dev_id & 0xFFF0) == BCM56070_DEVICE_ID) {
            SOC_IF_ERROR_RETURN(soc_firelight_chip_reset(unit));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            SOC_IF_ERROR_RETURN(soc_greyhound2_chip_reset(unit));
        }
    } else
#endif
#ifdef BCM_HELIX4_SUPPORT
    if (SOC_IS_HELIX4(unit)) {
        SOC_IF_ERROR_RETURN(soc_reset_bcm56340_a0(unit));
    } else
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        SOC_IF_ERROR_RETURN(soc_triumph3_chip_reset(unit));
    } else
#endif
#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        SOC_IF_ERROR_RETURN(soc_tomahawk2_chip_reset(unit));
    } else
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWK(unit)) {
        SOC_IF_ERROR_RETURN(soc_tomahawk_chip_reset(unit));
    } else
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        SOC_IF_ERROR_RETURN(soc_tomahawk3_chip_reset(unit));
    } else
#endif
#ifdef BCM_MONTEREY_SUPPORT
    /* This should be before APACHE check */
    if (SOC_IS_MONTEREY(unit)) {
        SOC_IF_ERROR_RETURN(soc_monterey_chip_reset(unit));
    } else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3(unit)) {
        SOC_IF_ERROR_RETURN(soc_trident3_chip_reset(unit));
    } else
#endif
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        SOC_IF_ERROR_RETURN(soc_firebolt6_chip_reset(unit));
    } else
#endif
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        SOC_IF_ERROR_RETURN(soc_hurricane4_chip_reset(unit));
    } else
#endif
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        SOC_IF_ERROR_RETURN(soc_helix5_chip_reset(unit));
    } else
#endif
#ifdef BCM_MAVERICK2_SUPPORT
    if (SOC_IS_MAVERICK2(unit)) {
        SOC_IF_ERROR_RETURN(soc_maverick2_chip_reset(unit));
    } else
#endif
#ifdef BCM_APACHE_SUPPORT
    /* This should be before TD2_TT2 check */
    if (SOC_IS_APACHE(unit)) {
        SOC_IF_ERROR_RETURN(soc_apache_chip_reset(unit));
    } else
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        SOC_IF_ERROR_RETURN(soc_trident2_chip_reset(unit));
    } else
#endif
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        SOC_IF_ERROR_RETURN(soc_reset_bcm56840_a0(unit));
    } else
#endif
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
        SOC_IF_ERROR_RETURN(soc_reset_bcm56440_a0(unit));
    } else
#endif
#ifdef BCM_METROLITE_SUPPORT
    if (SOC_IS_METROLITE(unit)) {
        SOC_IF_ERROR_RETURN(soc_reset_bcm56270_a0(unit));
    } else
#endif
#ifdef BCM_SABER2_SUPPORT
    if (SOC_IS_SABER2(unit)) {
        SOC_IF_ERROR_RETURN(soc_reset_bcm56260_a0(unit));
    } else
#endif
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        SOC_IF_ERROR_RETURN(soc_reset_bcm56450_a0(unit));
    } else
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        soc_reset_bcm56624_a0(unit);
    } else
#endif
#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        uint32 rval = 0;
        soc_reg_field_set(unit, CMIC_EB3_VLI_CONFIG_REGISTERr, &rval,
                          EB3_DATA_PARITY_ENABLEf, 1);
        soc_reg_field_set(unit, CMIC_EB3_VLI_CONFIG_REGISTERr, &rval,
                          EB3_ADDR_PARITY_ENABLEf, 1);
        WRITE_CMIC_EB3_VLI_CONFIG_REGISTERr(unit, rval);
        soc_reset_bcm88732_a0(unit);
    } else
#endif
#ifdef BCM_SCORPION_SUPPORT
    if (SOC_IS_SC_CQ(unit)) {
        soc_reset_bcm56820_a0(unit);
    } else
#endif
    if (SOC_IS_FB_FX_HX(unit)) {
        soc_reset_bcm56504_a0(unit);
    } else
#if defined (BCM_BRADLEY_SUPPORT)
    if (SOC_IS_HB_GW(unit)) {
        soc_reset_bcm56800_a0(unit);
    }
#endif /* BCM_BRADLEY_SUPPORT */
#endif /* BCM_FIREBOLT_SUPPORT */

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        soc_port_t port;
        /*
         * For H/W linkscan
         */
#if defined (BCM_CMICM_SUPPORT)
        if (soc_feature(unit, soc_feature_cmicm) &&
            !soc_property_get(unit, spn_MDIO_EXTERNAL_MASTER, 0)) {
            uint32 rval;
            READ_CMIC_MIIM_SCAN_CTRLr(unit, &rval);
            soc_reg_field_set(unit, CMIC_MIIM_SCAN_CTRLr, &rval,
                              OVER_RIDE_EXT_MDIO_MSTR_CNTRLf, 1);
            WRITE_CMIC_MIIM_SCAN_CTRLr(unit, rval);
        }
#endif
        if (!SOC_IS_SC_CQ(unit) && !SOC_IS_TD_TT(unit) &&
            !SOC_IS_FIRELIGHT(unit) &&
            !soc_feature(unit, soc_feature_cmicm)) {
            WRITE_CMIC_MIIM_PORT_TYPE_MAPr(unit,
                        SOC_PBMP_WORD_GET(PBMP_HG_ALL(unit), 0) |
                        SOC_PBMP_WORD_GET(PBMP_XE_ALL(unit), 0));
        }
        if (SAL_BOOT_QUICKTURN) {
            /* Kick the QT phy model */
            PBMP_GE_ITER(unit, port) {
                if (SOC_IS_SCORPION(unit)) {
                    SOC_IF_ERROR_RETURN
                        (soc_miim_write(unit, port - 24, 0x00, 0x140));
                } else {
                    if (SOC_IS_HELIX4(unit)) {
                        SOC_IF_ERROR_RETURN
                            (soc_miim_write(unit, port, 0x00, 0xFFFF));
                    } else if (SOC_IS_TRIDENT3X(unit)) {
                        /* Do Nothing for Trident3 */
                    } else {
                        SOC_IF_ERROR_RETURN
                            (soc_miim_write(unit, port, 0x00, 0x140));
                    }
                }
            }
            PBMP_FE_ITER(unit, port) {
                SOC_IF_ERROR_RETURN(soc_miim_write(unit, port, 0x00, 0x2100));
            }
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#ifdef BCM_BIGMAC_SUPPORT
#ifdef BCM_SCORPION_SUPPORT
#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SC_CQ(unit) && (!SOC_IS_SHADOW(unit))) {
#else /* BCM_SHADOW_SUPPORT */
    if (SOC_IS_SC_CQ(unit)) {
#endif
        soc_port_t port;
        soc_pbmp_t qg_pbmp;
        uint32 val, to_usec;

        to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                       (10 * MILLISECOND_USEC);

        /* Bring Unicore out of reset */
        PBMP_GX_ITER(unit, port) {
            _soc_xgxs_mdio_setup(unit, port);
            SOC_IF_ERROR_RETURN(soc_xgxs_reset(unit, port));
            val = 0;
            soc_reg_field_set(unit, XPORT_XGXS_NEWCTL_REGr,
                              &val, TXD1G_FIFO_RSTBf, 0xf);
            SOC_IF_ERROR_RETURN
                (WRITE_XPORT_XGXS_NEWCTL_REGr(unit, port, val));
        }
        SOC_PBMP_ASSIGN(qg_pbmp, PBMP_GE_ALL(unit));
        SOC_PBMP_REMOVE(qg_pbmp, PBMP_GX_ALL(unit));
        SOC_PBMP_ITER(qg_pbmp, port) {
            _soc_xgxs_mdio_setup(unit, port);
            SOC_IF_ERROR_RETURN(soc_xgxs_reset(unit, port));
            break;
        }

        /* Bring remaining blocks out of reset */
        READ_CMIC_SOFT_RESET_REGr(unit, &val);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_EP_RST_Lf, 1);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_IP_RST_Lf, 1);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_MMU_RST_Lf,
                          1);
        WRITE_CMIC_SOFT_RESET_REGr(unit, val);
        sal_usleep(to_usec);
    } else
#endif /* BCM_SCORPION_SUPPORT */
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TRIUMPH(unit) || SOC_IS_VALKYRIE(unit)) {
        soc_reg_t reg;
        int port, hc_idx;
        int hc_port[6]; /* any port in the hyperlite block */
        uint32 txd1g_map[6], val;

        /* bring unicore out of reset */
        PBMP_GX_ITER(unit, port) {
            _soc_xgxs_mdio_setup(unit, port);
            SOC_IF_ERROR_RETURN(soc_xgxs_reset(unit, port));
            val = 0;
            soc_reg_field_set(unit, XPORT_XGXS_NEWCTL_REGr,
                              &val, TXD1G_FIFO_RSTBf, 0xf);
            SOC_IF_ERROR_RETURN
                (WRITE_XPORT_XGXS_NEWCTL_REGr(unit, port, val));
        }

        /* bring hyperlite out of reset */
        for (hc_idx = 0; hc_idx < 6; hc_idx++) {
            hc_port[hc_idx] = -1;
            txd1g_map[hc_idx] = 0;
        }
        PBMP_XG_ITER(unit, port) {
            switch (tr_xgxs_map[port].mdio_conf_reg) {
            case CMIC_XGXS_MDIO_CONFIG_0r: hc_idx = 0; break;
            case CMIC_XGXS_MDIO_CONFIG_1r: hc_idx = 1; break;
            case CMIC_XGXS_MDIO_CONFIG_2r: hc_idx = 2; break;
            case CMIC_XGXS_MDIO_CONFIG_3r: hc_idx = 3; break;
            case CMIC_XGXS_MDIO_CONFIG_4r: hc_idx = 4; break;
            case CMIC_XGXS_MDIO_CONFIG_5r: hc_idx = 5; break;
            default: continue;
            }
            if (hc_port[hc_idx] == -1) {
                hc_port[hc_idx] = port;
            }
            if (!IS_XE_PORT(unit, port) && !IS_HG_PORT(unit, port)) {
                txd1g_map[hc_idx] |= 1 << tr_xgxs_map[port].hc_chan;
            } else {
                txd1g_map[hc_idx] = 0x0f;
            }
        }
        for (hc_idx = 0; hc_idx < 6; hc_idx++) {
            if (hc_port[hc_idx] == -1) {
                continue;
            }
            port = hc_port[hc_idx];
            _soc_xgxs_mdio_setup(unit, port);
            SOC_IF_ERROR_RETURN(soc_xgxs_reset(unit, port));
            reg = hc_idx != 1 && hc_idx != 4 ? XGPORT_XGXS_NEWCTL_REGr :
                XGPORT_EXTRA_XGXS_NEWCTL_REGr;
            val = 0;
            soc_reg_field_set(unit, reg, &val, TXD1G_FIFO_RSTBf,
                              txd1g_map[hc_idx]);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, val));
        }
    } else
#endif /* BCM_TRIUMPH_SUPPORT */
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit)) {
        int port, hc_idx;
        int hc_port[6]; /* any port in the hyperlite block */
        uint32 txd1g_map[6], val;

        /* bring unicore out of reset */
        PBMP_GX_ITER(unit, port) {
            if (IS_XQ_PORT(unit, port)) {
                /* Register does not exist for XQ ports */
                continue;
            }
            _soc_xgxs_mdio_setup(unit, port);
            SOC_IF_ERROR_RETURN(soc_xgxs_reset(unit, port));
            val = 0;
            soc_reg_field_set(unit, XPORT_XGXS_NEWCTL_REGr,
                              &val, TXD1G_FIFO_RSTBf, 0xf);
            SOC_IF_ERROR_RETURN
                (WRITE_XPORT_XGXS_NEWCTL_REGr(unit, port, val));
        }

        /* bring hyperlite out of reset */
        for (hc_idx = 0; hc_idx < 6; hc_idx++) {
            hc_port[hc_idx] = -1;
            txd1g_map[hc_idx] = 0;
        }
        PBMP_XQ_ITER(unit, port) {
            hc_idx = (port - 30) / 4;

            if (hc_port[hc_idx] == -1) {
                hc_port[hc_idx] = port;
            }
            if (!IS_XE_PORT(unit, port) && !IS_HG_PORT(unit, port)) {
                txd1g_map[hc_idx] |= 1 << ((port - 30) & 0x3);
            } else {
                txd1g_map[hc_idx] = 0x0f;
            }
        }
        for (hc_idx = 0; hc_idx < 6; hc_idx++) {
            if (hc_port[hc_idx] == -1) {
                continue;
            }
            port = hc_port[hc_idx];
            _soc_xgxs_mdio_setup(unit, port);
            SOC_IF_ERROR_RETURN(soc_xgxs_reset(unit, port));
            val = 0;
            soc_reg_field_set(unit, XQPORT_XGXS_NEWCTL_REGr, &val,
                              TXD1G_FIFO_RSTBf, txd1g_map[hc_idx]);
            SOC_IF_ERROR_RETURN(WRITE_XQPORT_XGXS_NEWCTL_REGr(unit, port,
                                                              val));
        }
    } else
#endif /* BCM_TRIUMPH2_SUPPORT */
#ifdef BCM_BRADLEY_SUPPORT
    if (SOC_IS_HB_GW(unit)) {
        soc_port_t port;
        uint32 val, to_usec;

        to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                       (10 * MILLISECOND_USEC);

        /* Bring Unicore out of reset */
        PBMP_GX_ITER(unit, port) {
            SOC_IF_ERROR_RETURN(soc_unicore_reset(unit, port));
        }

        /* Bring remaining blocks out of reset */
        READ_CMIC_SOFT_RESET_REGr(unit, &val);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_BSAFE_RST_Lf, 1);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_IP_RST_Lf, 1);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_EP_RST_Lf, 1);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_MMU_RST_Lf, 1);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val, CMIC_XP_RST_Lf, 1);
        WRITE_CMIC_SOFT_RESET_REGr(unit, val);
        sal_usleep(to_usec);
    } else
#endif /* BCM_BRADLEY_SUPPORT */
#ifdef BCM_ENDURO_SUPPORT
    if (SOC_IS_ENDURO(unit)) {
        soc_port_t port;
        uint32 val;

        /* bring unicore out of reset */
        PBMP_XQ_ITER(unit, port) {
            _soc_xgxs_mdio_setup(unit, port);
            SOC_IF_ERROR_RETURN(soc_xgxs_reset(unit, port));
            val = 0;
            soc_reg_field_set(unit, XQPORT_XGXS_NEWCTL_REGr, &val,
                              TXD1G_FIFO_RSTBf, 0xf);
            SOC_IF_ERROR_RETURN(WRITE_XQPORT_XGXS_NEWCTL_REGr(unit, port, val));
        }
    } else
#endif /* BCM_ENDURO_SUPPORT */
#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        /* setup xgxs mdio */
        /* reset xgxs */
        soc_info_t *si;
        soc_port_t  port;
        int         blk;
        uint32 rval;

        si = &SOC_INFO(unit);
        SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
            port = SOC_BLOCK_PORT(unit, blk);
            _soc_xgxs_mdio_setup(unit, port);
            SOC_IF_ERROR_RETURN(soc_xgxs_reset(unit, port));

            /* Bring XMAC out of reset */
            if (si->port_speed_max[port] < 10000) {
                continue;
            }
            SOC_IF_ERROR_RETURN(READ_XLPORT_XMAC_CONTROLr(unit, port, &rval));
            soc_reg_field_set(unit, XLPORT_XMAC_CONTROLr, &rval, XMAC_RESETf,
                              1);
            SOC_IF_ERROR_RETURN(WRITE_XLPORT_XMAC_CONTROLr(unit, port, rval));
            sal_udelay(10);

            soc_reg_field_set(unit, XLPORT_XMAC_CONTROLr, &rval, XMAC_RESETf,
                              0);
            SOC_IF_ERROR_RETURN(WRITE_XLPORT_XMAC_CONTROLr(unit, port, rval));
        }
    } else
#endif /* BCM_SHADOW_SUPPORT */
#ifdef BCM_HURRICANE1_SUPPORT
    if (SOC_IS_HURRICANE(unit)) {
        soc_port_t port;
        uint32 val;

        /* bring unicore out of reset */
        PBMP_XQ_ITER(unit, port) {
            _soc_xgxs_mdio_setup(unit, port);
            SOC_IF_ERROR_RETURN(soc_xgxs_reset(unit, port));
            val = 0;
            soc_reg_field_set(unit, XQPORT_XGXS_NEWCTL_REGr, &val,
                              TXD1G_FIFO_RSTBf, 0xf);
            SOC_IF_ERROR_RETURN(WRITE_XQPORT_XGXS_NEWCTL_REGr(unit, port, val));
        }
    } else
#endif /* BCM_HURRICANE_SUPPORT */
#ifdef BCM_HURRICANE2_SUPPORT
    if (SOC_IS_HURRICANE2(unit)) {
        SOC_IF_ERROR_RETURN(soc_hurricane2_tsc_reset(unit));
    } else
#endif /* BCM_HURRICANE2_SUPPORT */
#ifdef BCM_TOMAHAWKPLUS_SUPPORT
/* Check for TOMAHAWKPLUS before TOMAHAWK since TH+ is superset */
    if (SOC_IS_TOMAHAWKPLUS(unit)) {
        SOC_IF_ERROR_RETURN(soc_tomahawkplus_port_reset(unit));
    } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        SOC_IF_ERROR_RETURN(soc_tomahawk3_port_reset(unit));
    } else
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        SOC_IF_ERROR_RETURN(soc_tomahawk_port_reset(unit));
    } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        SOC_IF_ERROR_RETURN(soc_monterey_tsc_reset(unit));
    } else
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3(unit)) {
        SOC_IF_ERROR_RETURN(soc_trident3_port_reset(unit));
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        SOC_IF_ERROR_RETURN(soc_firebolt6_port_reset(unit));
    } else
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        SOC_IF_ERROR_RETURN(soc_hurricane4_port_reset(unit));
    } else
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        SOC_IF_ERROR_RETURN(soc_helix5_port_reset(unit));
    } else
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_MAVERICK2_SUPPORT
    if (SOC_IS_MAVERICK2(unit)) {
        SOC_IF_ERROR_RETURN(soc_maverick2_port_reset(unit));
    } else
#endif /* BCM_MAVERICK2_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        SOC_IF_ERROR_RETURN(soc_apache_tsc_reset(unit));
    } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        SOC_IF_ERROR_RETURN(soc_trident2_tsc_reset(unit));
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        soc_info_t *si;
        int rv, blk, port, phy_port, saved_phy_port, saved_port_type;
        uint32 rval;

        si = &SOC_INFO(unit);

        /* Toggle unused Warpcore */
        port = 1; /* borrow logical port 1 for the unused block */
        saved_phy_port = si->port_l2p_mapping[port];
        saved_port_type = si->port_type[port];
        si->port_type[port] = SOC_BLK_XLPORT;
        for (blk = 0; SOC_BLOCK_TYPE(unit, blk) >= 0; blk++) {
            if (SOC_BLOCK_TYPE(unit, blk) != SOC_BLK_XLPORT) {
                continue;
            }
            if (SOC_INFO(unit).block_valid[blk]) {
                continue;
            }
            for (phy_port = 0; ; phy_port++) {
                if (blk == SOC_PORT_BLOCK(unit, phy_port)) {
                    break;
                }
                if (SOC_PORT_BLOCK(unit, phy_port) < 0 &&
                    SOC_PORT_BINDEX(unit, phy_port) < 0) {
                    phy_port = -1;
                    break;
                }
            }
            if (phy_port >= 0) {
                si->port_l2p_mapping[port] = phy_port;
                rv = soc_wc_xgxs_reset(unit, port, 0);
                if (SOC_FAILURE(rv)) {
                    break;
                }
            }
        }
        for (blk = 0; SOC_BLOCK_TYPE(unit, blk) >= 0; blk++) {
            if (SOC_BLOCK_TYPE(unit, blk) != SOC_BLK_XLPORT) {
                continue;
            }
            if (SOC_INFO(unit).block_valid[blk]) {
                continue;
            }
            for (phy_port = 0; ; phy_port++) {
                if (blk == SOC_PORT_BLOCK(unit, phy_port)) {
                    break;
                }
                if (SOC_PORT_BLOCK(unit, phy_port) < 0 &&
                    SOC_PORT_BINDEX(unit, phy_port) < 0) {
                    phy_port = -1;
                    break;
                }
            }
            if (phy_port >= 0) {
                si->port_l2p_mapping[port] = phy_port;
                rv = soc_wc_xgxs_power_down(unit, port, 0);
                if (SOC_FAILURE(rv)) {
                    break;
                }
            }
        }
        si->port_l2p_mapping[port] = saved_phy_port;
        si->port_type[port] = saved_port_type;

        SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
            port = SOC_BLOCK_PORT(unit, blk);
            _soc_xgxs_mdio_setup(unit, port);
            SOC_IF_ERROR_RETURN(soc_wc_xgxs_reset(unit, port, 0));
        }
        SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
            port = SOC_BLOCK_PORT(unit, blk);
            SOC_IF_ERROR_RETURN(soc_wc_xgxs_pll_check(unit, port, 0));
        }

        SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
            port = SOC_BLOCK_PORT(unit, blk);
            /* Bring XMAC out of reset */
            if (si->port_speed_max[port] < 10000) {
                continue;
            }
            SOC_IF_ERROR_RETURN(READ_XLPORT_XMAC_CONTROLr(unit, port, &rval));
            soc_reg_field_set(unit, XLPORT_XMAC_CONTROLr, &rval, XMAC_RESETf,
                              1);
            SOC_IF_ERROR_RETURN(WRITE_XLPORT_XMAC_CONTROLr(unit, port, rval));
            sal_udelay(10);

            soc_reg_field_set(unit, XLPORT_XMAC_CONTROLr, &rval, XMAC_RESETf,
                              0);
            SOC_IF_ERROR_RETURN(WRITE_XLPORT_XMAC_CONTROLr(unit, port, rval));
        }
    } else
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_METROLITE_SUPPORT
        if(SOC_IS_METROLITE(unit)) {
            soc_port_t port;
            int blk;
            uint32  rval;

	    SOC_BLOCK_ITER(unit, blk, SOC_BLK_MXQPORT) {
	        port = SOC_BLOCK_PORT(unit, blk);
		_soc_xgxs_mdio_setup(unit, port);
		SOC_IF_ERROR_RETURN(soc_xgxs_reset(unit, port));

		SOC_IF_ERROR_RETURN(READ_XPORT_XMAC_CONTROLr(unit, port, &rval));
		soc_reg_field_set(unit, XPORT_XMAC_CONTROLr, &rval, XMAC_RESETf,
				1);
		SOC_IF_ERROR_RETURN(WRITE_XPORT_XMAC_CONTROLr(unit, port, rval));
		sal_udelay(10);

		soc_reg_field_set(unit, XPORT_XMAC_CONTROLr, &rval, XMAC_RESETf,
				0);
		SOC_IF_ERROR_RETURN(WRITE_XPORT_XMAC_CONTROLr(unit, port, rval));
	    }
            SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
                port = SOC_BLOCK_PORT(unit, blk);

                SOC_IF_ERROR_RETURN(READ_XLPORT_MAC_CONTROLr(unit, port, &rval));
                soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &rval,
                                  XMAC0_RESETf, 1);
                SOC_IF_ERROR_RETURN(WRITE_XLPORT_MAC_CONTROLr(unit, port, rval));
                sal_udelay(10);
                soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &rval,
                                  XMAC0_RESETf, 0);
                SOC_IF_ERROR_RETURN(WRITE_XLPORT_MAC_CONTROLr(unit, port, rval));

                SOC_IF_ERROR_RETURN(soc_xgxs_reset(unit, port));
            }
        } else
#endif

#ifdef BCM_SABER2_SUPPORT
        if(SOC_IS_SABER2(unit)) {
            soc_port_t port;
            int blk;
            uint32  rval;

            SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
                port = SOC_BLOCK_PORT(unit, blk);

                SOC_IF_ERROR_RETURN(READ_XLPORT_MAC_CONTROLr(unit, port, &rval));
                soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &rval,
                                  XMAC0_RESETf, 1);
                SOC_IF_ERROR_RETURN(WRITE_XLPORT_MAC_CONTROLr(unit, port, rval));
                sal_udelay(10);
                soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &rval,
                                  XMAC0_RESETf, 0);
                SOC_IF_ERROR_RETURN(WRITE_XLPORT_MAC_CONTROLr(unit, port, rval));

                SOC_IF_ERROR_RETURN(soc_xgxs_reset(unit, port));
            }
        } else
#endif
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit)) {
        soc_port_t port;
        int blk;
        uint32 rval;

        SOC_BLOCK_ITER(unit, blk, SOC_BLK_MXQPORT) {
            port = SOC_BLOCK_PORT(unit, blk);
            _soc_xgxs_mdio_setup(unit, port);
            SOC_IF_ERROR_RETURN(soc_xgxs_reset(unit, port));
            /* Bring XMAC out of reset */
            if (SOC_IS_KATANA(unit)) {
                SOC_IF_ERROR_RETURN(READ_XPORT_XGXS_NEWCTL_REGr(unit,port,&rval));
                soc_reg_field_set(unit, XPORT_XGXS_NEWCTL_REGr, &rval, TXD1G_FIFO_RSTBf,
                                  0xf);
                WRITE_XPORT_XGXS_NEWCTL_REGr(unit,port,rval);
            }
            SOC_IF_ERROR_RETURN(READ_XPORT_XMAC_CONTROLr(unit, port, &rval));
            soc_reg_field_set(unit, XPORT_XMAC_CONTROLr, &rval, XMAC_RESETf,
                              1);
            SOC_IF_ERROR_RETURN(WRITE_XPORT_XMAC_CONTROLr(unit, port, rval));
            sal_udelay(10);

            soc_reg_field_set(unit, XPORT_XMAC_CONTROLr, &rval, XMAC_RESETf,
                              0);
            SOC_IF_ERROR_RETURN(WRITE_XPORT_XMAC_CONTROLr(unit, port, rval));
        }
        SOC_BLOCK_ITER(unit, blk, SOC_BLK_GPORT) {
            port = SOC_BLOCK_PORT(unit, blk);
            SOC_IF_ERROR_RETURN(READ_QUAD0_SERDES_CTRLr(unit, port, &rval));
            soc_reg_field_set(unit, QUAD0_SERDES_CTRLr, &rval, RSTB_PLLf, 1);
            SOC_IF_ERROR_RETURN(WRITE_QUAD0_SERDES_CTRLr(unit, port, rval));
        if (SOC_PORT_VALID(unit, port+4)) {
        SOC_IF_ERROR_RETURN(WRITE_QUAD1_SERDES_CTRLr(unit, port+4, rval));
        }
            sal_usleep(100000);
            SOC_IF_ERROR_RETURN(READ_QUAD0_SERDES_CTRLr(unit, port, &rval));
            soc_reg_field_set(unit, QUAD0_SERDES_CTRLr, &rval, HW_RSTLf, 1);
            SOC_IF_ERROR_RETURN(WRITE_QUAD0_SERDES_CTRLr(unit, port, rval));
        if (SOC_PORT_VALID(unit, port+4)) {
        SOC_IF_ERROR_RETURN(WRITE_QUAD1_SERDES_CTRLr(unit, port+4, rval));
        }
            sal_usleep(10000);
            soc_reg_field_set(unit, QUAD0_SERDES_CTRLr, &rval, TXFIFO_RSTLf, 0xf);
            soc_reg_field_set(unit, QUAD0_SERDES_CTRLr, &rval, RSTB_PLLf, 1);
            soc_reg_field_set(unit, QUAD0_SERDES_CTRLr, &rval, RSTB_MDIOREGSf, 1);
            SOC_IF_ERROR_RETURN(WRITE_QUAD0_SERDES_CTRLr(unit, port, rval));
        if (SOC_PORT_VALID(unit, port+4)) {
        SOC_IF_ERROR_RETURN(WRITE_QUAD1_SERDES_CTRLr(unit, port+4, rval));
        }
            rval = 0xff; /* Set UMAC0_RESETf..UMAC7_RESETf */
            SOC_IF_ERROR_RETURN(WRITE_GPORT_UMAC_CONTROLr(unit, port, rval));
            sal_udelay(10);
            rval = 0; /* Reset UMAC0_RESETf..UMAC7_RESETf */
            SOC_IF_ERROR_RETURN(WRITE_GPORT_UMAC_CONTROLr(unit, port, rval));
        }
    } else
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        soc_info_t *si;
        soc_port_t port;
        int phy, lane, blk, block_has_ports, high_speed_port, xgxs0_reset;
        uint32 rval;

        si = &SOC_INFO(unit);
        SOC_BLOCK_ITER(unit, blk, SOC_BLK_CLPORT) {
            block_has_ports = FALSE;
            xgxs0_reset = FALSE;
            high_speed_port = -1;
            for (phy = 0; phy < _SOC_TR3_PHY_PER_PORT_BLOCK; phy++) {
                for (lane = 0; lane < _SOC_TR3_LANES_PER_PHY; lane++) {
                    port = si->port_p2l_mapping[si->blk_fpp[blk] +
                               ((phy * _SOC_TR3_LANES_PER_PHY) + lane)];
                    if (SOC_PORT_VALID(unit, port) &&
                        si->port_speed_max[port] >= 100000) {
                        /* Save the 100G+ speed port becuase we must
                         * init the other two PHYs, even though they don't
                         * match a valid port. */
                        high_speed_port = port;
                    } else if (high_speed_port >= 0) {
                        port = high_speed_port;
                    }
                    if (SOC_PORT_VALID(unit, port)) {
                        SOC_IF_ERROR_RETURN
                            (soc_wc_xgxs_reset(unit, port, phy));
                        block_has_ports |= 1 << phy;
                        if ((0 != phy) && !xgxs0_reset) {
                            /* We must always reset the first WC
                             * in the block */
                            SOC_IF_ERROR_RETURN
                                (soc_wc_xgxs_reset(unit, port, 0));
                        }
                        xgxs0_reset = TRUE;
                        break; /* Only need to reset PHY once */
                    }
                }
            }
            if (!block_has_ports) {
                continue; /* All ports disabled */
            }
            port = SOC_BLOCK_PORT(unit, blk);
            SOC_IF_ERROR_RETURN(READ_PORT_MAC_CONTROLr(unit, port, &rval));
            if ((soc_feature(unit, soc_feature_cmac)) && (high_speed_port >= 0)) {
                soc_reg_field_set(unit, PORT_MAC_CONTROLr, &rval,
                                  CMAC_RESETf, 1);
            }
            if (block_has_ports & 1) {
                soc_reg_field_set(unit, PORT_MAC_CONTROLr, &rval,
                                  XMAC0_RESETf, 1);
            }
            if (block_has_ports & 2) {
                soc_reg_field_set(unit, PORT_MAC_CONTROLr, &rval,
                                  XMAC1_RESETf, 1);
            }
            if (block_has_ports & 4) {
                soc_reg_field_set(unit, PORT_MAC_CONTROLr, &rval,
                                  XMAC2_RESETf, 1);
            }
            SOC_IF_ERROR_RETURN(WRITE_PORT_MAC_CONTROLr(unit, port, rval));
            sal_udelay(10);
            if ((soc_feature(unit, soc_feature_cmac)) && (high_speed_port >= 0)) {
                soc_reg_field_set(unit, PORT_MAC_CONTROLr, &rval,
                                  CMAC_RESETf, 0);
            }
            if (block_has_ports & 1) {
                soc_reg_field_set(unit, PORT_MAC_CONTROLr, &rval,
                                  XMAC0_RESETf, 0);
            }
            if (block_has_ports & 2) {
                soc_reg_field_set(unit, PORT_MAC_CONTROLr, &rval,
                                  XMAC1_RESETf, 0);
            }
            if (block_has_ports & 4) {
                soc_reg_field_set(unit, PORT_MAC_CONTROLr, &rval,
                                  XMAC2_RESETf, 0);
            }
            SOC_IF_ERROR_RETURN(WRITE_PORT_MAC_CONTROLr(unit, port, rval));

            /* Take MLD out of reset */
            SOC_IF_ERROR_RETURN(READ_PORT_MLD_CTRL_REGr(unit, port, &rval));
            soc_reg_field_set(unit, PORT_MLD_CTRL_REGr, &rval,
                              RSTB_MDIOREGSf, 1);
            soc_reg_field_set(unit, PORT_MLD_CTRL_REGr, &rval,
                              RSTB_HWf, 1);
            SOC_IF_ERROR_RETURN(WRITE_PORT_MLD_CTRL_REGr(unit, port, rval));
        }
        SOC_BLOCK_ITER(unit, blk, SOC_BLK_XTPORT) {
            block_has_ports = FALSE;
            xgxs0_reset = FALSE;
            for (phy = 0; phy < _SOC_TR3_PHY_PER_PORT_BLOCK; phy++) {
                for (lane = 0; lane < _SOC_TR3_LANES_PER_PHY; lane++) {
                    port = si->port_p2l_mapping[si->blk_fpp[blk] +
                               ((phy * _SOC_TR3_LANES_PER_PHY) + lane)];
                    if (SOC_PORT_VALID(unit, port)) {
                        SOC_IF_ERROR_RETURN
                            (soc_wc_xgxs_reset(unit, port, phy));
                        block_has_ports |= 1 << phy;
                        if ((0 != phy) && !xgxs0_reset) {
                            /* We must always reset the first WC in the block, even it is invalid*/
                            SOC_IF_ERROR_RETURN
                                (soc_wc_xgxs_reset(unit, port, 0));
                            SOC_IF_ERROR_RETURN
                                (soc_wc_xgxs_pll_check(unit, port, 0));
                        }
                        xgxs0_reset = TRUE;
                        break; /* Only need to reset PHY once */
                    }
                }
            }
            if (!block_has_ports) {
                continue; /* All ports disabled */
            }
            port = SOC_BLOCK_PORT(unit, blk);
            SOC_IF_ERROR_RETURN(READ_PORT_MAC_CONTROLr(unit, port, &rval));
            if (block_has_ports & 1) {
                soc_reg_field_set(unit, PORT_MAC_CONTROLr, &rval,
                                  XMAC0_RESETf, 1);
            }
            if (block_has_ports & 2) {
                soc_reg_field_set(unit, PORT_MAC_CONTROLr, &rval,
                                  XMAC1_RESETf, 1);
            }
            if (block_has_ports & 4) {
                soc_reg_field_set(unit, PORT_MAC_CONTROLr, &rval,
                                  XMAC2_RESETf, 1);
            }
            SOC_IF_ERROR_RETURN(WRITE_PORT_MAC_CONTROLr(unit, port, rval));
            sal_udelay(10);
            if (block_has_ports & 1) {
                soc_reg_field_set(unit, PORT_MAC_CONTROLr, &rval,
                                  XMAC0_RESETf, 0);
            }
            if (block_has_ports & 2) {
                soc_reg_field_set(unit, PORT_MAC_CONTROLr, &rval,
                                  XMAC1_RESETf, 0);
            }
            if (block_has_ports & 4) {
                soc_reg_field_set(unit, PORT_MAC_CONTROLr, &rval,
                                  XMAC2_RESETf, 0);
            }
            SOC_IF_ERROR_RETURN(WRITE_PORT_MAC_CONTROLr(unit, port, rval));
        }
        SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
            block_has_ports = FALSE;
            /* Only one PHY per XLPORT */
            for (lane = 0; lane < _SOC_TR3_LANES_PER_PHY; lane++) {
                port = si->port_p2l_mapping[si->blk_fpp[blk] + lane];
                if (SOC_PORT_VALID(unit, port)) {
                    block_has_ports = TRUE;
                    break;
                }
            }
            if (!block_has_ports) {
                continue; /* All ports disabled */
            }
            port = SOC_BLOCK_PORT(unit, blk);
            SOC_IF_ERROR_RETURN(soc_xgxs_reset(unit, port));
            SOC_IF_ERROR_RETURN(READ_PORT_MAC_CONTROLr(unit, port, &rval));
            soc_reg_field_set(unit, PORT_MAC_CONTROLr, &rval,
                              XMAC0_RESETf, 1);
            SOC_IF_ERROR_RETURN(WRITE_PORT_MAC_CONTROLr(unit, port, rval));
            sal_udelay(10);
            soc_reg_field_set(unit, PORT_MAC_CONTROLr, &rval,
                              XMAC0_RESETf, 0);
            SOC_IF_ERROR_RETURN(WRITE_PORT_MAC_CONTROLr(unit, port, rval));
        }
#ifdef BCM_HELIX4_SUPPORT
        if (SOC_IS_HELIX4(unit)) {
            SOC_BLOCK_ITER(unit, blk, SOC_BLK_XWPORT) {
                block_has_ports = FALSE;
                /* Only one PHY per XWPORT */
                for (lane = 0; lane < _SOC_TR3_LANES_PER_PHY; lane++) {
                    port = si->port_p2l_mapping[si->blk_fpp[blk] + lane];
                    if (SOC_PORT_VALID(unit, port)) {
                        block_has_ports = TRUE;
                        break;
                    }
                }
                if (!block_has_ports) {
                    continue; /* All ports disabled */
                }
                port = SOC_BLOCK_PORT(unit, blk);
                SOC_IF_ERROR_RETURN(soc_xgxs_reset(unit, port));
                SOC_IF_ERROR_RETURN(READ_PORT_MAC_CONTROLr(unit, port, &rval));
                soc_reg_field_set(unit, PORT_MAC_CONTROLr, &rval,
                                  XMAC0_RESETf, 1);
                SOC_IF_ERROR_RETURN(WRITE_PORT_MAC_CONTROLr(unit, port, rval));
                sal_udelay(10);
                soc_reg_field_set(unit, PORT_MAC_CONTROLr, &rval,
                                  XMAC0_RESETf, 0);
                SOC_IF_ERROR_RETURN(WRITE_PORT_MAC_CONTROLr(unit, port, rval));
            }
        }
#endif /* BCM_HELIX4_SUPPORT */
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_GREYHOUND_SUPPORT
        if (SOC_IS_GREYHOUND(unit)) {
            SOC_IF_ERROR_RETURN(soc_greyhound_tsc_reset(unit));
        } else
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_HURRICANE3_SUPPORT
    if (SOC_IS_HURRICANE3(unit)) {
        SOC_IF_ERROR_RETURN(soc_hurricane3_tsc_reset(unit));
    } else
#endif /* BCM_HURRICANE3_SUPPORT */
#ifdef BCM_GREYHOUND2_SUPPORT
    if (SOC_IS_GREYHOUND2(unit)) {
#ifdef BCM_FIRELIGHT_SUPPORT
        if ((dev_id & 0xFFF0) == BCM56070_DEVICE_ID) {
            SOC_IF_ERROR_RETURN(soc_firelight_tsc_reset(unit));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            SOC_IF_ERROR_RETURN(soc_greyhound2_tsc_reset(unit));
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */

    {
        soc_port_t port;

        /* Reset the fusion core for HiGig ports. Does nothing in sim */
        /* Fusioncore must be out of reset before we probe for XGXS PHY.
         * Otherwise, XGXS PHY will not respond.
         */
        PBMP_HG_ITER(unit, port) {
            _soc_xgxs_mdio_setup(unit, port);
            soc_fusioncore_reset(unit, port);
        }
        PBMP_XE_ITER(unit, port) {
            _soc_xgxs_mdio_setup(unit, port);
            soc_fusioncore_reset(unit, port);
        }
    }
#endif /* BCM_BIGMAC_SUPPORT */
    return 0;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      soc_warmboot_reset
 * Purpose:
 *      function used for chip specific process during warmboot
 */

STATIC int
soc_warmboot_reset(int unit) {

#ifdef BCM_GREYHOUND_SUPPORT
    if (SOC_IS_GREYHOUND(unit)){
        soc_greyhound_chip_warmboot_reset(unit);
    }
#endif

#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWK(unit)) {
        soc_tomahawk_chip_warmboot_reset(unit);
    }
#endif

#if defined(BCM_TOMAHAWK2_SUPPORT)
    if (SOC_IS_TOMAHAWK2(unit)) {
        soc_tomahawk2_chip_warmboot_reset(unit);
    }
#endif

    SOC_IF_ERROR_RETURN(soc_port_type_verify(unit));

    return SOC_E_NONE;
}
#endif

/*
 * Function:
 *      soc_detach
 * Purpose:
 *      Detach a SOC device and deallocate all resources allocated.
 * Notes:
 *      The chip is reset prior to detaching in order to quiesce
 *      any DMA, etc. operations that may be in progress.
 */

int
soc_detach(int unit)
{
    soc_control_t   *soc;
    soc_mem_t       mem, act_mem, lock_mem;
    int             ix;
    int             cmc;
    int             do_reset;

    LOG_INFO(BSL_LS_SOC_PCI,
             (BSL_META_U(unit,
                         "Detaching\n")));

    soc = SOC_CONTROL(unit);

    if (NULL == soc) {
        return SOC_E_NONE;
    }

    if (0 == (soc->soc_flags & SOC_F_ATTACHED)) {
        return SOC_E_NONE;
    }

#ifdef BCM_HERCULES_SUPPORT
    /* Dump the MMU error stats */
    soc_mmu_error_done(unit);

    if (soc->lla_cells_good != NULL) {
        sal_free(soc->lla_cells_good);
        soc->lla_cells_good = NULL;
    }

    if (soc->lla_map != NULL) {
        int port;
        PBMP_PORT_ITER(unit, port) {
            if (soc->lla_map[port] != NULL) {
                sal_free(soc->lla_map[port]);
            }
        }
        sal_free(soc->lla_map);
        soc->lla_map = NULL;
    }

    if (soc->sbe_disable != NULL) {
        sal_free(soc->sbe_disable);
        soc->sbe_disable = NULL;
    }
#endif

    /* Free up any memory used by the I2C driver */

#ifdef  INCLUDE_I2C
    SOC_IF_ERROR_RETURN(soc_i2c_detach(unit));
#endif

    /* Disable shutting down unit to run new DPCs and wait until all
     * existing DPCs are completed */
    sal_dpc_disable_and_wait(INT_TO_PTR(unit));

    /*
     * Call soc_init to cancel link scan task, counter DMA task,
     * outstanding DMAs, interrupt generation, and anything else the
     * driver or chip may be doing.
     */

    do_reset = TRUE;
#if defined(PLISIM)
    if (SAL_BOOT_PLISIM) {
        do_reset = FALSE;
    }
#endif /* PLISIM */
    if (do_reset) {
        if (soc_reset_init(unit) < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "could not reset and init soc \n")));
        }
    }


    /* Device specific cleanup/deinit */
    if (SOC_FUNCTIONS(unit)->soc_misc_deinit) {
        if (SOC_FAILURE(SOC_FUNCTIONS(unit)->soc_misc_deinit(unit))) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Misc deinit failed \n")));
            return SOC_E_FAIL;
        }
    }

    /*
     * PHY drivers and ID map
     */

    
#if 0
    if (SOC_USE_PORTCTRL(unit)) {
        SOC_IF_ERROR_RETURN(soc_portctrl_software_deinit(unit));
    } else {
        SOC_IF_ERROR_RETURN(soc_phyctrl_software_deinit(unit));
    }
#else
    if (SOC_USE_PORTCTRL(unit)) {
        SOC_IF_ERROR_RETURN(soc_esw_portctrl_deinit(unit));
    }

    SOC_IF_ERROR_RETURN(soc_phyctrl_software_deinit(unit));
#endif

    if (!SOC_IS_RCPU_ONLY(unit)) {
        /* Free up DMA memory */
        SOC_IF_ERROR_RETURN(soc_dma_detach(unit));
    }

    /* Shutdown polled interrupt mode if active */
    soc_ipoll_disconnect(unit);
    soc->soc_flags &= ~SOC_F_POLLED;

    /* Detach interrupt handler, if we installed one */
    /* unit # is ISR arg */
    if (soc_cm_interrupt_disconnect(unit) < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_detach: could not disconnect interrupt line\n")));
    }

    /*
     * When detaching, take care to free up only resources that were
     * actually allocated, in case we are cleaning up after an attach
     * that failed part way through.
     */

    if (soc->arlBuf != NULL) {
        soc_cm_sfree(unit, (void *)soc->arlBuf);
        soc->arlBuf = NULL;
    }

    if (soc->arlShadow != NULL) {
        shr_avl_destroy(soc->arlShadow);
        soc->arlShadow = NULL;
    }

    if (soc->arlShadowMutex != NULL) {
        sal_mutex_destroy(soc->arlShadowMutex);
        soc->arlShadowMutex = NULL;
    }

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (soc->l2x_lrn_shadow != NULL) {
        shr_avl_destroy(soc->l2x_lrn_shadow);
        soc->l2x_lrn_shadow = NULL;
    }

    if (soc->l2x_lrn_shadow_mutex != NULL) {
        sal_mutex_destroy(soc->l2x_lrn_shadow_mutex);
        soc->l2x_lrn_shadow_mutex = NULL;
    }

    if SOC_IS_TOMAHAWK3(unit) {
        (void)soc_th3_l2x_appl_callback_resources_destroy(unit);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

    if (soc->ipArbiterMutex != NULL) {
        sal_mutex_destroy(soc->ipArbiterMutex);
        soc->ipArbiterMutex = NULL;
    }

    /* Terminate counter module; frees allocated space */

    soc_counter_detach(unit);

    if (soc->counterMutex) {
        sal_mutex_destroy(soc->counterMutex);
        soc->counterMutex = NULL;
    }

    if (soc->evictMutex) {
        sal_mutex_destroy(soc->evictMutex);
        soc->evictMutex = NULL;
    }
#ifdef SOC_ROBUST_HASH
    if (soc_feature(unit, soc_feature_robust_hash)) {
        if (soc->soc_robust_hash_config != NULL) {
            sal_free(soc->soc_robust_hash_config);
            soc->soc_robust_hash_config = NULL;
        }
    }
#endif /* SOC_ROBUST_HASH */


    /* Deallocate L2X module and shadow table resources */

#ifdef BCM_XGS_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_arl_hashed)) {
        SOC_IF_ERROR_RETURN(soc_l2x_detach(unit));
    }
#ifdef BCM_TRIUMPH3_SUPPORT
    if SOC_IS_TRIUMPH3(unit) {
        SOC_IF_ERROR_RETURN(soc_tr3_l2_bulk_age_stop(unit));
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if SOC_IS_TRIDENT2X(unit) {
        SOC_IF_ERROR_RETURN(soc_td2_l2_bulk_age_stop(unit));
    }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWK(unit) || SOC_IS_TOMAHAWK2(unit) ||
        SOC_IS_TRIDENT3X(unit)) {
        SOC_IF_ERROR_RETURN(soc_th_l2_bulk_age_stop(unit));
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef ALPM_ENABLE
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (soc_property_get(unit, spn_L3_ALPM_ENABLE, 0) &&
        soc_feature(unit, soc_feature_alpm2)) {
        SOC_IF_ERROR_RETURN(alpm_dist_hitbit_thread_stop(unit));
    }
#endif
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    /* Stop L2 learning and aging threads */
    if (SOC_IS_TOMAHAWK3(unit)) {
        SOC_IF_ERROR_RETURN(soc_th3_l2_age_stop(unit));
        SOC_IF_ERROR_RETURN(soc_th3_l2_learn_thread_stop(unit));
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_SBUSDMA_SUPPORT
    if (SOC_SBUSDMA_DM_INFO(unit)) {
        SOC_IF_ERROR_RETURN(soc_sbusdma_desc_detach(unit));
    }
#endif /* BCM_SBUSDMA_SUPPORT */
#endif /* BCM_XGS_SWITCH_SUPPORT */

#ifdef INCLUDE_MEM_SCAN
    SOC_IF_ERROR_RETURN(soc_mem_scan_stop(unit)); /* Stop memory scanner */
    if (soc->mem_scan_notify) {
        sal_sem_destroy(soc->mem_scan_notify);
    }
#endif
#ifdef BCM_SRAM_SCAN_SUPPORT
    SOC_IF_ERROR_RETURN(soc_sram_scan_stop(unit)); /* Stop memory scanner */
    if (soc->sram_scan_notify) {
        sal_sem_destroy(soc->sram_scan_notify);
    }
#endif
    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (!SOC_MEM_IS_VALID(unit, mem)) {
            continue;
        }
        act_mem = mem;
#ifdef BCM_ESW_SUPPORT
        soc_drv_mem_reuse_mem_state(unit, &act_mem, &lock_mem);
#endif
        /* Deallocate table cache memory, if caching enabled */
        SOC_IF_ERROR_RETURN(soc_mem_cache_set(unit, act_mem, COPYNO_ALL, FALSE));
    }

    /* LOCK used in cache_set, so destroy later */
    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (!SOC_MEM_IS_VALID(unit, mem)) {
            continue;
        }
        act_mem = mem;
        lock_mem = mem;
#ifdef BCM_ESW_SUPPORT
        soc_drv_mem_reuse_mem_state(unit,&act_mem, &lock_mem);
#endif
        if (soc->memState[lock_mem].lock != NULL) {
            sal_mutex_destroy(soc->memState[lock_mem].lock);
            soc->memState[lock_mem].lock = NULL;
        }
#ifdef BCM_ESW_SUPPORT
        /* Share the lock */
        soc_drv_mem_sync_mapped_lock(unit, mem, soc->memState[lock_mem].lock);
#endif
    }

#ifdef BCM_ESW_SUPPORT
    if (SOC_MEM_IS_VALID(unit, ING_DEVICE_PORTm) &&
        !SOC_MEM_IS_VALID(unit, PORT_TABm)) {
        soc->memState[PORT_TABm].lock = soc->memState[ING_DEVICE_PORTm].lock;
    }
#endif

    if (soc->schanMutex) {
        sal_mutex_destroy(soc->schanMutex);
        soc->schanMutex = NULL;
    }

#ifdef BCM_CMICM_SUPPORT
    if (soc->fschanMutex) {
        sal_mutex_destroy(soc->fschanMutex);
        soc->fschanMutex = NULL;
    }
#ifdef BCM_CCMDMA_SUPPORT
    for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit); cmc++) {
        if (soc->ccmDmaIntr[cmc]) {
            sal_sem_destroy(soc->ccmDmaIntr[cmc]);
            soc->ccmDmaIntr[cmc] = NULL;
        }
    }
#endif
    if (soc_feature(unit, soc_feature_cmicm)) {
        soc_cmic_uc_msg_stop(unit);
    }
#endif
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        soc_cmic_uc_msg_stop(unit);
    }
#endif

    for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit) + 1; cmc++) {
        if (soc->schanIntr[cmc]) {
            sal_sem_destroy(soc->schanIntr[cmc]);
            soc->schanIntr[cmc] = NULL;
        }
    }

    if (soc->miimMutex) {
        sal_mutex_destroy(soc->miimMutex);
        soc->miimMutex = NULL;
    }

    (void)soc_sbusdma_lock_deinit(unit);

#ifdef BCM_CMICM_SUPPORT
    for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit); cmc++) {
        int ch;
        for (ch = 0; ch < N_DMA_CHAN; ch++) {
            if (soc->fifoDmaMutexs[cmc][ch]) {
                sal_mutex_destroy(soc->fifoDmaMutexs[cmc][ch]);
                soc->fifoDmaMutexs[cmc][ch] = NULL;
            }
            if (soc->fifoDmaIntrs[cmc][ch]) {
                sal_sem_destroy(soc->fifoDmaIntrs[cmc][ch]);
                soc->fifoDmaIntrs[cmc][ch] = NULL;
            }
        }
    }
#endif

    if (soc->miimIntr) {
        sal_sem_destroy(soc->miimIntr);
        soc->miimIntr = NULL;
    }

    for (ix = 0; ix < 3; ix++) {
        if (soc->memCmdIntr[ix]) {
            sal_sem_destroy(soc->memCmdIntr[ix]);
            soc->memCmdIntr[ix] = NULL;
        }
    }

    if (soc->arl_notify) {
        sal_sem_destroy(soc->arl_notify);
        soc->arl_notify = NULL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_esm_correction)) {
        SOC_IF_ERROR_RETURN(soc_tr3_esm_recovery_stop(unit));
        if (soc->esm_recovery_notify) {
            sal_sem_destroy(soc->esm_recovery_notify);
            soc->esm_recovery_notify = NULL;
        }
        if (soc->esm_lock) {
                sal_mutex_destroy(soc->esm_lock);
                soc->esm_lock = NULL;
        }
    }
#endif

#ifdef BCM_XGS_SWITCH_SUPPORT
    if (NULL != soc->l2x_notify) {
        sal_sem_destroy(soc->l2x_notify);
        soc->l2x_notify = NULL;
    }
    if (NULL != soc->l2x_del_sync) {
        sal_mutex_destroy(soc->l2x_del_sync);
        soc->l2x_del_sync = NULL;
    }
    if (NULL != soc->l2x_lock) {
        sal_sem_destroy(soc->l2x_lock);
        soc->l2x_lock = NULL;
    }
    soc->l2x_pid = SAL_THREAD_ERROR;
    soc->l2x_interval = 0;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        /* Clear L2 learning related data */
        soc->l2x_learn_pid = SAL_THREAD_ERROR;
        soc->l2x_learn_interval = 0;
        if (NULL != soc->l2x_lrn_shadow_mutex) {
            sal_mutex_destroy(soc->l2x_lrn_shadow_mutex);
            soc->l2x_lrn_shadow_mutex = NULL;
        }

        soc->lrn_cache_threshold = -1;
        soc->lrn_cache_intr_ctl = -1;
        soc->lrn_cache_clr_on_rd = -1;
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TSN_SUPPORT
    if (NULL != soc->l2x_sr_sync) {
        sal_mutex_destroy(soc->l2x_sr_sync);
        soc->l2x_sr_sync = NULL;
    }
#endif /* BCM_TSN_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_TOMAHAWKX(unit) ||
        SOC_IS_TRIDENT3X(unit)) {
        if (NULL != soc->l2x_age_notify) {
            sal_sem_destroy(soc->l2x_age_notify);
            soc->l2x_age_notify = NULL;
        }
        soc->l2x_age_pid = SAL_THREAD_ERROR;
        soc->l2x_age_interval = 0;
        soc->l2x_agetime_adjust_usec = 0;
        soc->l2x_agetime_curr_timeblk = 0;
        soc->l2x_agetime_curr_timeblk_usec = 0;
        soc->l2x_prev_age_timeout = 0;
    }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if SOC_IS_TRIDENT2X(unit) {
        if (NULL != soc->l2x_age_notify) {
            sal_sem_destroy(soc->l2x_age_notify);
            soc->l2x_age_notify = NULL;
        }
        if (NULL != soc->l2x_age_sync) {
            sal_sem_destroy(soc->l2x_age_sync);
            soc->l2x_age_sync = NULL;
        }
        soc->l2x_age_pid = SAL_THREAD_ERROR;
        soc->l2x_age_interval = 0;
        soc->l2x_age_enable = 0;
        soc->l2x_sw_aging = 0;
    }
#endif
#endif /* BCM_XGS_SWITCH_SUPPORT */
    /* Destroy L3 module protection mutex. */
    if (NULL != soc->l3x_lock) {
        sal_mutex_destroy(soc->l3x_lock);
        soc->l3x_lock = NULL;
    }

    /* Destroy FP module protection mutex. */
    if (NULL != soc->fp_lock) {
        sal_mutex_destroy(soc->fp_lock);
        soc->fp_lock = NULL;
    }

#ifdef INCLUDE_XFLOW_MACSEC
        if (soc_feature(unit, soc_feature_xflow_macsec)) {
            /* Destroy XFLOW_MACSEC module protection mutex. */
            if (NULL != soc->xflow_macsec_lock) {
                sal_mutex_destroy(soc->xflow_macsec_lock);
                soc->xflow_macsec_lock = NULL;
            }
        }
#endif

    if (soc->ipfixIntr) {
        sal_sem_destroy(soc->ipfixIntr);
        soc->ipfixIntr = NULL;
    }

#if defined(INCLUDE_REGEX)
        if (soc->ftreportIntr) {
            sal_sem_destroy(soc->ftreportIntr);
            soc->ftreportIntr = NULL;
        }
#endif

#ifdef BCM_FLOWTRACKER_EXPORT_FIFO_SUPPORT
    if (soc->ftExportIntr) {
        sal_sem_destroy(soc->ftExportIntr);
        soc->ftExportIntr = NULL;
    }
#endif /* BCM_FLOWTRACKER_EXPORT_FIFO_SUPPORT */
    soc->ctr_evict_interval = 0;
    if (soc->ctrEvictIntr) {
        sal_sem_destroy(soc->ctrEvictIntr);
        soc->ctrEvictIntr = NULL;
    }

    if (soc->socControlMutex) {
        sal_mutex_destroy(soc->socControlMutex);
        soc->socControlMutex = NULL;
    }

    if (soc->mem_ser_info_lock) {
        sal_mutex_destroy(soc->mem_ser_info_lock);
        soc->mem_ser_info_lock = NULL;
    }

    if (soc->egressMeteringMutex) {
        sal_mutex_destroy(soc->egressMeteringMutex);
        soc->egressMeteringMutex = NULL;
    }

    if (soc->schan_wb_mutex != NULL) {
        sal_mutex_destroy(soc->schan_wb_mutex);
        soc->schan_wb_mutex = NULL;
    }

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_KATANA2_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_APACHE_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_KATANAX(unit) || SOC_IS_TRIDENT2X(unit) ||
        SOC_IS_TITAN2X(unit) || SOC_IS_TRIUMPH3(unit)) {
            /* Destroy llsMutex */
            if (soc->llsMutex !=NULL) {
                sal_mutex_destroy(soc->llsMutex);
                soc->llsMutex = NULL;
            }
        }
#endif

#if defined(BCM_TOMAHAWK_SUPPORT)
        if (SOC_IS_TOMAHAWKX(unit)) {
            /* Destroy idb_lock */
            if (soc->idb_lock !=NULL) {
                sal_mutex_destroy(soc->idb_lock);
                soc->idb_lock = NULL;
            }
        }
#endif
#ifdef INCLUDE_MEM_SCAN
        if (soc->mem_scan_lock != NULL) {
            sal_mutex_destroy(soc->mem_scan_lock);
            soc->mem_scan_lock = NULL;
        }
#endif

    if (soc->ser_err_stat_lock != NULL) {
        sal_mutex_destroy(soc->ser_err_stat_lock);
        soc->ser_err_stat_lock = NULL;
    }

    /* Destroy UDF module protection mutex. */
    if (NULL != soc->udf_lock) {
       sal_mutex_destroy(soc->udf_lock);
       soc->udf_lock = NULL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT)
         if (soc_feature(unit, soc_feature_epc_linkflap_recover)) {
            /* Destroy linkscan_modport_map_lock_lock */
            if (soc->linkscan_modport_map_lock !=NULL) {
                sal_mutex_destroy(soc->linkscan_modport_map_lock);
                soc->linkscan_modport_map_lock = NULL;
            }
         }
#endif /* #(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT) */
    if (soc->tcam_info) {
        sal_free(soc->tcam_info);
    }

    /* MMU LLS usage map */
    for (ix = 0; ix < SOC_MAX_NUM_PORTS; ix++) {
#if defined(BCM_KATANA2_SUPPORT)
        if (soc->port_lls_s0_bmap[ix] != NULL) {
            sal_free(soc->port_lls_s0_bmap[ix]);
        }
#endif /*  BCM_KATANA2_SUPPORT */
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_APACHE_SUPPORT)
        if (soc->port_lls_s1_bmap[ix] != NULL) {
            sal_free(soc->port_lls_s1_bmap[ix]);
        }
#endif /* BCM_KATANA2_SUPPORT  || BCM_APACHE_SUPPORT*/
        if (soc->port_lls_l0_bmap[ix] != NULL) {
            sal_free(soc->port_lls_l0_bmap[ix]);
        }
        if (soc->port_lls_l1_bmap[ix] != NULL) {
            sal_free(soc->port_lls_l1_bmap[ix]);
        }
        if (soc->port_lls_l2_bmap[ix] != NULL) {
        sal_free(soc->port_lls_l2_bmap[ix]);
        }
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (soc->ext_l2_ppa_info != NULL) {
        sal_free(soc->ext_l2_ppa_info);
        soc->ext_l2_ppa_info = NULL;
    }
    if (soc->ext_l2_ppa_vlan != NULL) {
        sal_free(soc->ext_l2_ppa_vlan);
        soc->ext_l2_ppa_vlan = NULL;
    }
#endif /* BCM_TRIUMPH_SUPPORT */

    if (soc->tdm_info != NULL) {
        sal_free(soc->tdm_info);
        soc->tdm_info = NULL;
    }

    if (soc->soc_wc_ucode_dma_buf != NULL) {
        soc_cm_sfree(unit, soc->soc_wc_ucode_dma_buf);
        soc->soc_wc_ucode_dma_buf = NULL;
    }
    if (soc->soc_wc_ucode_dma_buf2 != NULL) {
        soc_cm_sfree(unit, soc->soc_wc_ucode_dma_buf2);
        soc->soc_wc_ucode_dma_buf2 = NULL;
    }

#if defined(BCM_RCPU_SUPPORT) && defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_RCPU_ONLY(unit)) {
        soc_rcpu_deinit(unit);
    }
#endif /* !defined(PLISIM) */
/* In current SDK, l3 defip index is saved in soc_control block.
 * the relative resource has to be released before soc_control
 * block free */
    SOC_IF_ERROR_RETURN(soc_l3_defip_indexes_deinit(unit));

#ifdef BCM_UTT_SUPPORT
    if (soc_feature(unit, soc_feature_utt)) {
        SOC_IF_ERROR_RETURN(soc_utt_deinit(unit));
    }
#endif

#if defined(BCM_FIREBOLT6_SUPPORT)
    if (soc_feature(unit, soc_feature_efp_meter_noread)) {
        if (SOC_INFO(unit).efp_meter_table != NULL) {
            sal_free(SOC_INFO(unit).efp_meter_table);
            SOC_INFO(unit).efp_meter_table = NULL;
        }
    }
#endif /* BCM_FIREBOLT6_SUPPORT */

    sal_free(SOC_PERSIST(unit));
    SOC_PERSIST(unit) = NULL;

    sal_free(SOC_CONTROL(unit));
    SOC_CONTROL(unit) = NULL;


#ifdef BCM_WARM_BOOT_SUPPORT
    /* Second times detach cause SOC_E_CONFIG so not CheckingFalseReturn value*/
    soc_scache_detach(unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

    if (--soc_ndev_attached == 0) {
        /* Work done after the last SOC device is detached. */
        /* (currently nothing) */
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_functions_find
 * Purpose:
 *      Return function array corresponding to the driver
 * Returns:
 *      Pointer to static function array soc_functions_t
 */

STATIC soc_functions_t *
_soc_functions_find(soc_driver_t *drv)
{
    switch (drv->type) {

#ifdef BCM_HERCULES15_SUPPORT
    case SOC_CHIP_BCM5675_A0:  return &soc_hercules15_drv_funs;
#endif

#ifdef BCM_FIREBOLT_SUPPORT
    case SOC_CHIP_BCM56504_A0:  return &soc_firebolt_drv_funs;
    case SOC_CHIP_BCM56504_B0:  return &soc_firebolt_drv_funs;
    case SOC_CHIP_BCM56102_A0:  return &soc_firebolt_drv_funs;
    case SOC_CHIP_BCM56304_B0:  return &soc_firebolt_drv_funs;
    case SOC_CHIP_BCM56112_A0:  return &soc_firebolt_drv_funs;
    case SOC_CHIP_BCM56314_A0:  return &soc_firebolt_drv_funs;
    case SOC_CHIP_BCM56514_A0:  return &soc_firebolt_drv_funs;
#ifdef BCM_RAPTOR_SUPPORT
    case SOC_CHIP_BCM56218_A0:  return &soc_firebolt_drv_funs;
    case SOC_CHIP_BCM56224_A0:  return &soc_firebolt_drv_funs;
    case SOC_CHIP_BCM56224_B0:  return &soc_firebolt_drv_funs;
    case SOC_CHIP_BCM53314_A0:  return &soc_firebolt_drv_funs;
    case SOC_CHIP_BCM53324_A0:  return &soc_firebolt_drv_funs;
#endif
#ifdef BCM_BRADLEY_SUPPORT
    case SOC_CHIP_BCM56800_A0:  return &soc_bradley_drv_funs;
    case SOC_CHIP_BCM56820_A0:  return &soc_bradley_drv_funs;
    case SOC_CHIP_BCM56725_A0:  return &soc_bradley_drv_funs;
#endif
#ifdef BCM_SHADOW_SUPPORT
    case SOC_CHIP_BCM88732_A0:  return &soc_shadow_drv_funs;
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    case SOC_CHIP_BCM56624_A0:  return &soc_triumph_drv_funs;
    case SOC_CHIP_BCM56624_B0:  return &soc_triumph_drv_funs;
#endif
#ifdef BCM_VALKYRIE_SUPPORT
    case SOC_CHIP_BCM56680_A0:  return &soc_valkyrie_drv_funs;
    case SOC_CHIP_BCM56680_B0:  return &soc_valkyrie_drv_funs;
#endif
#ifdef BCM_TRIUMPH2_SUPPORT
    case SOC_CHIP_BCM56634_A0:  return &soc_triumph2_drv_funs;
    case SOC_CHIP_BCM56634_B0:  return &soc_triumph2_drv_funs;
    case SOC_CHIP_BCM56524_A0:  return &soc_triumph2_drv_funs;
    case SOC_CHIP_BCM56524_B0:  return &soc_triumph2_drv_funs;
    case SOC_CHIP_BCM56685_A0:  return &soc_triumph2_drv_funs;
    case SOC_CHIP_BCM56685_B0:  return &soc_triumph2_drv_funs;
#endif
#ifdef BCM_ENDURO_SUPPORT
    case SOC_CHIP_BCM56334_A0:  return &soc_enduro_drv_funs;
    case SOC_CHIP_BCM56334_B0:  return &soc_enduro_drv_funs;
#endif
#ifdef BCM_HURRICANE_SUPPORT
    case SOC_CHIP_BCM56142_A0:  return &soc_hurricane_drv_funs;
#endif
#ifdef BCM_HURRICANE2_SUPPORT
    case SOC_CHIP_BCM56150_A0:  return &soc_hurricane2_drv_funs;
#endif
#ifdef BCM_TRIDENT_SUPPORT
    case SOC_CHIP_BCM56840_A0:  return &soc_trident_drv_funs;
    case SOC_CHIP_BCM56840_B0:  return &soc_trident_drv_funs;
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    case SOC_CHIP_BCM56850_A0:  return &soc_trident2_drv_funs;
    case SOC_CHIP_BCM56860_A0:  return &soc_trident2_drv_funs;
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    case SOC_CHIP_BCM56960_A0:  return &soc_tomahawk_drv_funs;
    case SOC_CHIP_BCM56965_A0:  return &soc_tomahawk_drv_funs;
    case SOC_CHIP_BCM56965_A1:  return &soc_tomahawk_drv_funs;
    case SOC_CHIP_BCM56970_A0:  return &soc_tomahawk_drv_funs;
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    case SOC_CHIP_BCM56980_A0:  return &soc_tomahawk3_drv_funs;
    case SOC_CHIP_BCM56980_B0:  return &soc_tomahawk3_drv_funs;
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    case SOC_CHIP_BCM56870_A0:  return &soc_trident3_drv_funs;
    case SOC_CHIP_BCM56873_A0:  return &soc_trident3_drv_funs;
#endif
#ifdef BCM_HURRICANE4_SUPPORT
    case SOC_CHIP_BCM56275_A0:  return &soc_hurricane4_drv_funs;
#endif
#ifdef BCM_HELIX5_SUPPORT
    case SOC_CHIP_BCM56370_A0:  return &soc_helix5_drv_funs;
#endif
#ifdef BCM_MAVERICK2_SUPPORT
    case SOC_CHIP_BCM56770_A0:  return &soc_maverick2_drv_funs;
    case SOC_CHIP_BCM56771_A0:  return &soc_maverick2_drv_funs;
#endif
#ifdef BCM_FIREBOLT6_SUPPORT
    case SOC_CHIP_BCM56470_A0:  return &soc_firebolt6_drv_funs;
#endif
#ifdef BCM_APACHE_SUPPORT
    case SOC_CHIP_BCM56560_A0: return &soc_apache_drv_funs;
    case SOC_CHIP_BCM56560_B0: return &soc_apache_drv_funs;
#endif
#ifdef BCM_MONTEREY_SUPPORT
    case SOC_CHIP_BCM56670_A0: return &soc_monterey_drv_funs;
    case SOC_CHIP_BCM56670_B0: return &soc_monterey_drv_funs;
    case SOC_CHIP_BCM56670_C0: return &soc_monterey_drv_funs;
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    case SOC_CHIP_BCM56640_A0:  return &soc_triumph3_drv_funs;
    case SOC_CHIP_BCM56640_B0:  return &soc_triumph3_drv_funs;
#endif
#ifdef BCM_HELIX4_SUPPORT
    case SOC_CHIP_BCM56340_A0:  return &soc_helix4_drv_funs;
#endif
#ifdef BCM_KATANA_SUPPORT
    case SOC_CHIP_BCM56440_A0:  return &soc_katana_drv_funs;
    case SOC_CHIP_BCM56440_B0:  return &soc_katana_drv_funs;
#endif
#ifdef BCM_KATANA2_SUPPORT
    case SOC_CHIP_BCM56450_A0:  return &soc_katana2_drv_funs;
    case SOC_CHIP_BCM56450_B0:  return &soc_katana2_drv_funs;
    case SOC_CHIP_BCM56450_B1:  return &soc_katana2_drv_funs;
#endif
#ifdef BCM_GREYHOUND_SUPPORT
    case SOC_CHIP_BCM53400_A0:  return &soc_greyhound_drv_funs;
#endif

#ifdef BCM_HURRICANE3_SUPPORT
    case SOC_CHIP_BCM56160_A0:  return &soc_hurricane3_drv_funs;
    case SOC_CHIP_BCM53540_A0:  return &soc_wolfhound2_drv_funs;
#endif
#ifdef BCM_GREYHOUND2_SUPPORT
    case SOC_CHIP_BCM53570_A0:  return &soc_greyhound2_drv_funs;
    case SOC_CHIP_BCM53570_B0:  return &soc_greyhound2_drv_funs;
#endif
#ifdef BCM_FIRELIGHT_SUPPORT
    case SOC_CHIP_BCM56070_A0:  return &soc_firelight_drv_funs;
#endif /* BCM_FIRELIGHT_SUPPORT */

#ifdef BCM_SABER2_SUPPORT
    case SOC_CHIP_BCM56260_A0:  return &soc_saber2_drv_funs;
    case SOC_CHIP_BCM56260_B0:  return &soc_saber2_drv_funs;
#endif
#ifdef BCM_METROLITE_SUPPORT
    case SOC_CHIP_BCM56270_A0:  return &soc_metrolite_drv_funs;
#endif
#endif
    default:                    return NULL;
    }
}

/* Warning: This function and _soc_mem_lock_map should be used in pairs */
STATIC int
_soc_mem_lock_create(int unit, soc_control_t *soc) {
    soc_mem_t mem;

    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        /*
         * should only create mutexes for valid memories.
         */
        if (!SOC_MEM_IS_VALID(unit, mem)) {
            continue;
        }
        /* Handle many to one state sharing */
        if (soc->memState[mem].lock == NULL) {
            char mem_name[128];
#if !defined(SOC_NO_NAMES)
#if defined(BCM_ESW_SUPPORT) || \
    defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
            sal_strncpy(mem_name, SOC_MEM_NAME(unit, mem),
                        sizeof(mem_name)-1);
#else
            sal_snprintf(mem_name, sizeof(mem_name), "%s%d",
                         "mem_mutex_", mem);
#endif
#else
            sal_snprintf(mem_name, sizeof(mem_name), "%s%d",
                         "mem_mutex_", mem);
#endif
            if ((soc->memState[mem].lock =
                 sal_mutex_create(mem_name)) == NULL) {
                return -1;
            }
            /* Set cache copy pointers to NULL */
            sal_memset(soc->memState[mem].cache,
                       0, sizeof(soc->memState[mem].cache));
        }
    }
    return 0;
}
#ifdef BCM_ESW_SUPPORT
STATIC void
_soc_mem_lock_map(int unit, soc_control_t *soc)
{
    soc_mem_t mem, act_mem, lock_mem;

    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (!SOC_MEM_IS_VALID(unit, mem)) {
            continue;
        }
        act_mem = mem;
        lock_mem = mem;
        soc_drv_mem_reuse_mem_state(unit, &act_mem, &lock_mem);
        /* In function _soc_mem_lock_create, all memory semaphores are created */
        if (mem != lock_mem) {
            if (soc->memState[mem].lock != NULL) {
                sal_mutex_destroy(soc->memState[mem].lock);
                soc->memState[mem].lock = NULL;
            }
            /* We can't add _SOC_MEM_SYNC_MAPPED_LOCK(unit, lock_mem, soc->memState[lock_mem].lock)
            * between next "for" cluase to here.
            * First time, when L3_ENTRY_ONLYm comes, and Lock of L3_ENTRY_IPV4_MULTICASTm is mapped to memories
            * L3_ENTRY_ONLYm/L3_ENTRY_IPV4_UNICASTm/L3_ENTRY_IPV6_UNICASTm/L3_ENTRY_IPV6_MULTICASTm.
            * Second time, when L3_ENTRY_IPV4_UNICASTm comes, Lock of L3_ENTRY_IPV4_MULTICASTm will be destoryed.
            * That's so bad.
            */
        }
    }

    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (!SOC_MEM_IS_VALID(unit, mem)) {
            continue;
        }
        act_mem = mem;
        lock_mem = mem;
        soc_drv_mem_reuse_mem_state(unit, &act_mem, &lock_mem);
        if (mem != lock_mem) {
            soc_drv_mem_sync_mapped_lock(unit, lock_mem, soc->memState[lock_mem].lock);
        }
    }

    if (SOC_MEM_IS_VALID(unit, ING_DEVICE_PORTm) &&
        !SOC_MEM_IS_VALID(unit, PORT_TABm)) {
        soc->memState[PORT_TABm].lock = soc->memState[ING_DEVICE_PORTm].lock;
    }

    return;
}
#endif

/*
 * Function:
 *      soc_interrupt_connect
 * Purpose:
 *      Register the soc interrupt
 * Parameters:
 *      unit - StrataSwitch unit #
 */
static int
soc_interrupt_connect(int unit)
{
    soc_control_t  *soc = SOC_CONTROL(unit);
    int rv = SOC_E_NONE;
    ipoll_handler_t     iph = soc_intr;

    if (soc == NULL) {
        return SOC_E_MEMORY;
    }
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        /* initialize the interrupt framework */
        SOC_IF_ERROR_RETURN(soc_cmic_intr_init(unit));
        /* Disable all interrupts */
        soc_cmic_intr_all_disable(unit);
        iph = soc_cmicx_intr;
    } else
#endif
#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {
        /* Disable interrupts */
        soc_cmicm_intr0_disable(unit, ~0);
        soc_cmicm_intr1_disable(unit, ~0);
        soc_cmicm_intr2_disable(unit, ~0);
#ifdef BCM_CMICDV2_SUPPORT
        if (soc_feature(unit, soc_feature_cmicd_v2)) {
            soc_cmicm_intr3_disable(unit, ~0);
            soc_cmicm_intr4_disable(unit, ~0);
            soc_cmicm_intr5_disable(unit, ~0);
#ifdef BCM_CMICDV4_SUPPORT
            if (soc_feature(unit, soc_feature_cmicd_v4)) {
                soc_cmicm_intr6_disable(unit, ~0);
            }
#endif /* BCM_CMICDV4_SUPPORT */
        }
#endif /* BCM_CMICDV2_SUPPORT */
        if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
            soc_cmicm_cmcx_intr0_disable(unit, SOC_ARM_CMC(unit, 0), ~0);
            soc_cmicm_cmcx_intr0_disable(unit, SOC_ARM_CMC(unit, 1), ~0);
        }
        iph = soc_cmicm_intr;
    } else
#endif
    {
        soc_intr_disable(unit, ~0);
    }

    if (soc_property_get(unit, spn_POLLED_IRQ_MODE, 0)) {
        if (soc_ipoll_connect(unit, iph, INT_TO_PTR(unit)) < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_attach: "
                                  "error initializing polled interrupt mode\n")));
            return SOC_E_INTERNAL;
        }
        soc->soc_flags |= SOC_F_POLLED;
    } else {
        /* unit # is ISR arg */
        if (soc_cm_interrupt_connect(unit, iph, INT_TO_PTR(unit)) < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_attach: could not connect interrupt line\n")));
            return SOC_E_INTERNAL;
        }
    }

    return rv;
}

/*
 * Function:
 *      soc_attach
 * Purpose:
 *      Initialize the soc_control_t structure for a device,
 *      allocating all memory and semaphores required.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      detach - Callback function called on detach.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      No chip initialization is done other than masking all interrupts,
 *      see soc_init or soc_reset_init.
 */
int
soc_attach(int unit)
{
    soc_control_t       *soc;
    soc_persist_t       *sop;
    int                 rv = SOC_E_MEMORY;
    soc_mem_t           mem;
    uint16              dev_id, drv_dev_id, drv_rev_id;
    uint8               rev_id;
    soc_port_t          port;
    int                 ix;
    uint8               table_dma_enable=0;
    uint8               tslam_dma_enable=0;
    int                 enable;
    int                 cmc;
    char                *mode = NULL;
#ifdef BCM_CMICM_SUPPORT
    int                 ch;
#endif

#ifdef BCM_KATANA_SUPPORT
    uint32              saber_pool_size[BCM_SABER_MAX_COUNTER_DIRECTION] =
                        {511, 255};
    soc_mem_t           saber_counter_table[BCM_SABER_MAX_COUNTER_DIRECTION]
                                           [BCM_SABER_MAX_COUNTER_POOL]={
                        {ING_FLEX_CTR_COUNTER_TABLE_0m,
                         ING_FLEX_CTR_COUNTER_TABLE_1m,
                         ING_FLEX_CTR_COUNTER_TABLE_2m,
                         ING_FLEX_CTR_COUNTER_TABLE_3m,
                         ING_FLEX_CTR_COUNTER_TABLE_4m,
                         ING_FLEX_CTR_COUNTER_TABLE_5m,
                         ING_FLEX_CTR_COUNTER_TABLE_6m,
                         ING_FLEX_CTR_COUNTER_TABLE_7m},
                        {EGR_FLEX_CTR_COUNTER_TABLE_0m,
                         EGR_FLEX_CTR_COUNTER_TABLE_1m,
                         EGR_FLEX_CTR_COUNTER_TABLE_2m,
                         EGR_FLEX_CTR_COUNTER_TABLE_3m,
                         EGR_FLEX_CTR_COUNTER_TABLE_4m,
                         EGR_FLEX_CTR_COUNTER_TABLE_5m,
                         EGR_FLEX_CTR_COUNTER_TABLE_6m,
                         EGR_FLEX_CTR_COUNTER_TABLE_7m}};
    uint32              saber_counter_direction=0;
    uint32              saber_counter_pool=0;
#endif
    soc_pbmp_t          pbmp_all;
    SOC_PBMP_CLEAR(pbmp_all);

    LOG_INFO(BSL_LS_SOC_PCI,
             (BSL_META_U(unit,
                         "soc_attach: unit %d\n"), unit));

    /*
     * Allocate soc_control and soc_persist if not already.
     */

    if (SOC_CONTROL(unit) == NULL) {
        SOC_CONTROL(unit) =
            sal_alloc(sizeof (soc_control_t), "soc_control");
        if (SOC_CONTROL(unit) == NULL) {
            return SOC_E_MEMORY;
        }
        sal_memset(SOC_CONTROL(unit), 0, sizeof (soc_control_t));
    }

    SOC_PERSIST(unit) = sal_alloc(sizeof (soc_persist_t), "soc_persist");
    if (NULL == SOC_PERSIST(unit)) {
        return SOC_E_MEMORY;
    }

    sal_memset(SOC_PERSIST(unit), 0, sizeof (soc_persist_t));

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_IF_ERROR_RETURN(soc_stable_attach(unit));
#endif /* BCM_WARM_BOOT_SUPPORT */

    soc = SOC_CONTROL(unit);
    sop = SOC_PERSIST(unit);
    sop->version = 1;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &drv_dev_id, &drv_rev_id);

    /*
     * Instantiate the driver -- Verify chip revision matches driver
     * compilation revision.
     */
    soc->chip_driver = soc_chip_driver_find(dev_id, rev_id);

    if (soc->chip_driver == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_attach: unit %d has no driver "
                              "(device 0x%04x rev 0x%02x)\n"),
                   unit, dev_id, rev_id));
        return SOC_E_UNAVAIL;
    }

    soc->soc_functions = _soc_functions_find(soc->chip_driver);

    if (soc->soc_functions == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_attach: unit %d has no functions\n"),
                              unit));
        return SOC_E_INTERNAL;
    }

    /* Start to allow this unit to run DPCs */
    sal_dpc_enable(INT_TO_PTR(unit));

#ifdef INCLUDE_RCPU
    if (soc_property_get(unit, spn_PCI2EB_OVERRIDE, 0)) {
        soc->remote_cpu = TRUE;
    } else {
        soc->remote_cpu = FALSE;
    }

    /* Is this an RCPU Only unit? */
    if (soc_property_get(unit, spn_RCPU_ONLY, 0) ||
        (soc_cm_get_bus_type(unit) & SOC_RCPU_DEV_TYPE)) {
        soc->remote_cpu = TRUE;
        soc->soc_flags |= SOC_F_RCPU_ONLY;
    }
#endif /* INCLUDE_RCPU */

    soc->soc_rcpu_schan_op = NULL;

    soc->ev_cb_head = NULL;

    /* Set feature cache, since used by mutex creation */
    soc_feature_init(unit);
    SOC_PCI_CMCS_NUM(unit) = soc_property_uc_get(unit, 0, spn_PCI_CMCS_NUM, 1);
    /* Install the Interrupt Handler */
    /* Make sure interrupts are masked before connecting line. */
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        int arm;

        SOC_CMCS_NUM(unit) = 2;

        if (SAL_BOOT_SIMULATION) {
            SOC_PCI_CMCS_NUM(unit) = soc_property_uc_get(unit, 0,
                                                     spn_PCI_CMCS_NUM, 2);
        }

        if ((SOC_PCI_CMCS_NUM(unit) < 1) ||
            (SOC_PCI_CMCS_NUM(unit) > SOC_CMCS_NUM(unit))) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_attach: "
                                  "PCI CMCS NUM %d not between 1 to %d\n"),
                       SOC_PCI_CMCS_NUM(unit), SOC_CMCS_NUM(unit)));
            return SOC_E_PARAM;
        }
        SOC_PCI_CMC(unit) = soc_property_uc_get(unit, 0, spn_PCI_CMC, 0);
        if (SOC_PCI_CMC(unit) >= SOC_CMCS_NUM(unit)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_attach: "
                                  "PCI CMC %d exceeds maximum of %d\n"),
                       SOC_PCI_CMC(unit), SOC_CMCS_NUM(unit)));
            return SOC_E_PARAM;
        }
        arm = 0;
        for (ix = 0; ix < SOC_CMCS_NUM(unit); ix++) {
            if (SOC_PCI_CMC(unit) != ix) {
                SOC_ARM_CMC(unit, arm) = ix;
                arm++;
            }
        }
    } else
#endif
#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {
        int arm;

        /* Initialize PCI/iProc based host irq offset */
        if (soc_cm_get_bus_type(unit) & SOC_AXI_DEV_TYPE) {
            soc_cmicm_ihost_irq_offset_set(unit);
        } else {
            soc_cmicm_ihost_irq_offset_reset(unit);
        }
        SOC_CMCS_NUM(unit) = 3; 

        if ((SOC_PCI_CMCS_NUM(unit) < 1) ||
            (SOC_PCI_CMCS_NUM(unit) > SOC_CMCS_NUM(unit))) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_attach: "
                                  "specified PCI CMCS NUM %d must be within 1 to %d\n"),
                       SOC_PCI_CMCS_NUM(unit), SOC_CMCS_NUM(unit)));
            return SOC_E_PARAM;
        }
        SOC_PCI_CMC(unit) = soc_property_uc_get(unit, 0, spn_PCI_CMC, 0);
        if (SOC_PCI_CMC(unit) >= SOC_CMCS_NUM(unit)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_attach: "
                                  "specified PCI CMC %d exceeds maximum of %d\n"),
                       SOC_PCI_CMC(unit), SOC_CMCS_NUM(unit)));
            return SOC_E_PARAM;
        }
        arm = 0;
        for (ix = 0; ix < SOC_CMCS_NUM(unit); ix++) {
            
            if (SOC_PCI_CMC(unit) != ix) {
                SOC_ARM_CMC(unit, arm) = ix;
                arm++;
            }
        }
    }
#endif

    if (soc->soc_flags & SOC_F_ATTACHED) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_attach: unit %d already attached\n"),
                              unit));
        return(SOC_E_NONE);
    }

    /*
     * Create mutexes first as these are needed even if initialization fails.
     */

    if ((soc->socControlMutex = sal_mutex_create("SOC_CONTROL")) == NULL) {
        goto error;
    }

    if ((soc->mem_ser_info_lock = sal_mutex_create("em_ser_info")) == NULL) {
        goto error;
    }

    if ((soc->schanMutex = sal_mutex_create("SCHAN")) == NULL) {
        goto error;
    }

    if ((soc->ipArbiterMutex = sal_mutex_create("IPARBITER")) == NULL) {
        goto error;
    }

#ifdef BCM_CMICM_SUPPORT
    if (soc_property_get(unit, spn_FSCHAN_ENABLE, 0)) {
        if ((soc->fschanMutex = sal_mutex_create("FSCHAN")) == NULL) {
            goto error;
        }
    } else {
        soc->fschanMutex = NULL;
    }
#endif

    if ((soc->miimMutex = sal_mutex_create("MIIM")) == NULL) {
        goto error;
    }
    if (0 !=  _soc_mem_lock_create(unit, soc)) {
        goto error;
    }

#ifdef INCLUDE_MEM_SCAN
    if ((soc->mem_scan_notify =
         sal_sem_create("memscan timer", sal_sem_BINARY, 0)) == NULL) {
        goto error;
    }

    soc->mem_scan_pid = SAL_THREAD_ERROR;
    soc->mem_scan_interval = 0;
    if ((soc->mem_scan_lock =
        sal_mutex_create("mem_scan_lock")) == NULL) {
        goto error;
    }
#endif

#ifdef BCM_SRAM_SCAN_SUPPORT
    if ((soc->sram_scan_notify =
         sal_sem_create("sramscan timer", sal_sem_BINARY, 0)) == NULL) {
        goto error;
    }

    soc->sram_scan_pid = SAL_THREAD_ERROR;
    soc->sram_scan_interval = 0;
#endif

    if (soc_feature(unit, soc_feature_ser_error_stat)) {
        if ((soc->ser_err_stat_lock =
            sal_mutex_create("ser_err_stat_lock")) == NULL) {
            goto error;
        }
    }

    if (soc_feature(unit, soc_feature_mem_cmd)) {
        soc->memCmdTimeout = soc_property_get(unit,
                                                spn_MEMCMD_TIMEOUT_USEC,
                                                1000000);
        soc->memCmdIntrEnb = soc_property_get(unit,
                                                spn_MEMCMD_INTR_ENABLE, 0);
        for (ix = 0; ix < 3; ix++) {
            if ((soc->memCmdIntr[ix] =
                 sal_sem_create("MemCmd interrupt", sal_sem_BINARY, 0)) == NULL) {
                goto error;
            }
        }
    } else {
        for (ix = 0; ix < 3; ix++) {
            soc->memCmdIntr[ix] = 0;
        }
    }

    if (soc_feature(unit, soc_feature_ipfix)) {
        soc->ipfixIntrEnb =
            soc_property_get(unit, spn_IPFIX_INTR_ENABLE,
                             soc_feature(unit, soc_feature_fifo_dma) ? 1 : 0);
    }

    if (soc_feature(unit, soc_feature_regex)) {
        soc->ftreportIntrEnb =
            soc_property_get(unit, spn_FLOW_TRACKER_INTR_ENABLE, 1);
    }

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit,
                soc_feature_flex_flowtracker_indirect_memory_access)) {
        soc->ftIndirectMemAccessTimeout = soc_property_get(unit,
                    spn_FLOWTRACKER_INDIRECT_MEM_ACCESS_TIMEOUT_USEC, 100000);
    }
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */
#ifdef BCM_FLOWTRACKER_EXPORT_FIFO_SUPPORT
    if (soc_feature(unit, soc_feature_bscan_ft_fifodma_support)) {
        soc->ftExportIntrEnb =
            soc_property_get(unit, spn_FLOWTRACKER_EXPORT_FIFO_INTR_ENABLE, 1);
    }
#endif /* BCM_FLOWTRACKER_EXPORT_FIFO_SUPPORT */

    if (soc_feature(unit, soc_feature_fifo_dma)) {
        soc->l2modDmaIntrEnb =
            soc_property_get(unit, spn_L2MOD_DMA_INTR_ENABLE, 1);
        soc->ctrEvictDmaIntrEnb =
            soc_property_get(unit, "ctr_evict_dma_intr_enable", 1);
    }

    if (soc_feature(unit, soc_feature_multi_sbus_cmds)) {
        soc->sbusCmdSpacing =
            soc_property_get(unit, "multi_sbus_cmds_spacing", -1);
    }

    if ((soc->counterMutex = sal_mutex_create("Counter")) == NULL) {
        goto error;
    }

    if ((soc->evictMutex = sal_mutex_create("Evict")) == NULL) {
        goto error;
    }
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_esm_correction)) {
    if ((soc->esm_recovery_notify = sal_sem_create("etu recovery",
        	                                         sal_sem_BINARY, 0)) == NULL) {
        goto error;
    }
    if ((soc->esm_lock = sal_mutex_create("esm")) == NULL) {
        goto error;
    }

    soc->esm_recovery_pid = SAL_THREAD_ERROR;
    soc->esm_recovery_enable = FALSE;
    }
#endif

    /*
     * Attached flag must be true during initialization.
     * If initialization fails, the flag is cleared by soc_detach (below).
     */

    soc->soc_flags |= SOC_F_ATTACHED;

    soc_family_suffix_update(unit);

    if (soc_ndev_attached++ == 0) {
        int                     chip;

        /* Work to be done before the first SOC device is attached. */
        for (chip = 0; chip < SOC_NUM_SUPPORTED_CHIPS; chip++) {
            /* Call each chip driver's init function */
            if (soc_base_driver_table[chip]->init) {
                (soc_base_driver_table[chip]->init)();
            }
        }
    }

#if defined(BCM_ESW_SUPPORT) && defined(BCM_IPROC_SUPPORT)
    /* Initialize bond info cache */
    if (soc_feature(unit, soc_feature_untethered_otp)) {
        if (soc_bond_info_init(unit) < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_attach: could not initialize bond info structure\n")));
            return SOC_E_INTERNAL;
        }
    }
#endif /* BCM_ESW_SUPPORT && BCM_IPROC_SUPPORT */

#ifdef BCM_TOMAHAWK3_SUPPORT
    /* TH3's port_config_init needs to know if soc_attach is calling it, or
     * if init soc is calling it
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        SOC_IF_ERROR_RETURN(soc_th3_from_soc_attach(unit, 1));
    }
#endif
    /*
     * Set up port bitmaps.  They are also set up on each soc_init so
     * they can be changed from a CLI without rebooting.
     */
    if (0 != (soc_info_config(unit, soc))) {
        goto error;
    }

    /* Connect Interrupt */
    rv = soc_interrupt_connect(unit);
    if (rv != SOC_E_NONE) {
        goto error;
    }

#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)
#ifdef BCM_CCMDMA_SUPPORT
    for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit); cmc++) {
        soc->ccmDmaMutex[cmc] = NULL;
        soc->ccmDmaIntr[cmc] = NULL;
    }
    if (soc_feature(unit, soc_feature_cmicm) ||
           soc_feature(unit, soc_feature_cmicx)) {
        int ccmdma = 0;
        ccmdma = soc_property_get(unit, spn_CCM_DMA_ENABLE, 0);
        if (ccmdma) {
            if (SAL_BOOT_QUICKTURN) {
                soc->ccmDmaTimeout = CCMDMA_TIMEOUT_QT;
            } else {
                soc->ccmDmaTimeout = CCMDMA_TIMEOUT;
            }
            soc->ccmDmaTimeout = soc_property_get(unit, spn_CCMDMA_TIMEOUT_USEC,
                                                    soc->ccmDmaTimeout);
            if (soc->ccmDmaTimeout) {
                for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit); cmc++) {
                    soc->ccmDmaMutex[cmc] = sal_mutex_create("ccmDMA");
                    if (soc->ccmDmaMutex[cmc] == NULL) {
                        goto error;
                    }
                    soc->ccmDmaIntr[cmc] = sal_sem_create("CCMDMA interrupt",
                                                          sal_sem_BINARY, 0);
                    if (soc->ccmDmaIntr[cmc] == NULL) {
                        goto error;
                    }
                    soc->ccmDmaIntrEnb = soc_property_get(unit,
                                                          spn_CCMDMA_INTR_ENABLE, 1);
                }
            }
            soc_ccmdma_init(unit);
        }
    }
#endif

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx) &&
                    !((SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM))) {
        int ccmdma = 0;
        ccmdma = soc_property_get(unit, spn_CCM_DMA_ENABLE, 0);
        if (soc_esw_schan_fifo_init(unit, ccmdma) != SOC_E_NONE) {
            return SOC_E_FAIL;
        }
    }
#endif

#ifdef BCM_FIFODMA_SUPPORT
    soc_fifodma_init(unit);
#endif
    /* Cross-CPU communications with UCs */
    if (soc_feature(unit, soc_feature_cmicm)) {
        soc_cmic_uc_msg_start(unit);
    }
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        soc_cmic_uc_msg_start(unit);
    }
#endif
#endif /* CMICM Support or  BCM_CMICX_SUPPORT*/
    /* Initialize bulk mem API */
    mode = soc_property_get_str(unit, spn_SOC_MEM_BULK_SCHAN_OP_MODE);
    if (mode) {
        soc->memBulkSchanPioMode =
                        (sal_strcasecmp(mode , "pio") == 0) ? 1 : 0;
    } else {
        soc->memBulkSchanPioMode = 0;
    }
    if (soc_mem_bulk_init(unit) != SOC_E_NONE) {
        goto error;
    }

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        SOC_IF_ERROR_RETURN(soc_th3_from_soc_attach(unit, 0));
    }
#endif

#ifdef BCM_ESW_SUPPORT
    _soc_mem_lock_map(unit, soc);
#endif

    soc_dcb_unit_init(unit);

    table_dma_enable = soc_property_get(unit, spn_TABLE_DMA_ENABLE, 1);
    tslam_dma_enable = soc_property_get(unit, spn_TSLAM_DMA_ENABLE, 1);
#ifdef PLISIM
#if defined(BCM_KATANA_SUPPORT)
    if (SOC_IS_KATANAX(unit) && (SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM)) {
       LOG_ERROR(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "WARNING:TABLE DMA is currently not working"
                             " so ignoring its setting %d\n"), table_dma_enable));
       LOG_ERROR(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "WARNING:TSLAM DMA is currently not working"
                             " so ignoring its setting %d\n"), tslam_dma_enable));
       table_dma_enable = 0;
       tslam_dma_enable = 0;
     }
#else
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "INFO:TABLE DMA setting %d \n"),table_dma_enable));
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "INFO:TSLAM DMA setting %d \n"),tslam_dma_enable));
#endif
#endif

    rv = soc_sbusdma_lock_init(unit);
    if (rv != SOC_E_NONE) {
        goto error;
    }

#ifdef INCLUDE_RCPU
    if (SOC_IS_RCPU_ONLY(unit)) {
        table_dma_enable = 0;
        tslam_dma_enable = 0;
    }
#endif /* INCLUDE_RCPU */

    if (soc_feature(unit, soc_feature_table_dma) &&
        table_dma_enable) {
        table_dma_enable = 1;
    } else {
        table_dma_enable = 0;
    }

    if (soc_feature(unit, soc_feature_tslam_dma) &&
        tslam_dma_enable) {
        tslam_dma_enable = 1;
    } else {
        tslam_dma_enable = 0;
    }
    soc->tdma_enb = table_dma_enable;
    soc->tslam_enb = tslam_dma_enable;

#ifdef BCM_CMICM_SUPPORT
    if (SAL_BOOT_QUICKTURN) {
        soc->fifoDmaTimeout = FIFO_TIMEOUT_QT;
    } else {
        soc->fifoDmaTimeout = FIFO_TIMEOUT;
    }
    soc->fifoDmaTimeout = soc_property_get(unit, spn_DMA_DESC_TIMEOUT_USEC,
                                           soc->fifoDmaTimeout);
    if (soc->fifoDmaTimeout) {
        for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit); cmc++) {
            for (ch = 0; ch < N_DMA_CHAN; ch++) {
                soc->fifoDmaMutexs[cmc][ch] = NULL;
                soc->fifoDmaMutexs[cmc][ch] = sal_mutex_create("FifoDMA");
                if (soc->fifoDmaMutexs[cmc][ch] == NULL) {
                    goto error;
                }
                soc->fifoDmaIntrs[cmc][ch] = NULL;
                soc->fifoDmaIntrs[cmc][ch] = sal_sem_create("FIFODMA interrupt",
                                                            sal_sem_BINARY, 0);
                if (soc->fifoDmaIntrs[cmc][ch] == NULL) {
                    goto error;
                }
            }
        }
        soc->fifoDmaIntrEnb = soc_property_get(unit,
                                               spn_L2MOD_DMA_INTR_ENABLE, 1);
    }
#endif

    if ((soc->schan_wb_mutex = sal_mutex_create("SchanWB")) == NULL) {
        goto error;
    }

#ifdef BROADCOM_DEBUG
    /* must be after soc_info_config, due to use of data structures */
    if (!SOC_IS_HELIX4(unit)) {
        _soc_counter_verify(unit);
    }
#endif /* BROADCOM_DEBUG */

    soc_cm_get_id(unit, &dev_id, &rev_id);

    /*
     * Initialize memory index_maxes. Chip specific overrides follow.
     */
    SOC_CONTROL(unit)->tcam_protect_write =
        soc_property_get(unit, "tcam_protect_write", FALSE);
    SOC_CONTROL(unit)->tcam_protect_write_init =
        SOC_CONTROL(unit)->tcam_protect_write;
    enable = soc_property_get(unit, "force_read_through", FALSE);
    SOC_MEM_FORCE_READ_THROUGH_SET(unit, enable);
    enable = soc_property_get(unit, "cache_coherency_check", TRUE);
    SOC_MEM_CACHE_COHERENCY_CHECK_SET(unit, enable);
    SOC_CONTROL(unit)->dma_from_mem_cache =
        soc_property_get(unit, "dma_from_cache", TRUE);
    enable = soc_property_get(unit, "reg_ser_error_assert", FALSE);
    SOC_REG_RETURN_SER_ERROR_SET(unit, enable);
    SOC_SER_CORRECTION_SUPPORT(unit) =
        soc_property_get(unit, spn_PARITY_CORRECTION,
                         (SAL_BOOT_RTLSIM || SAL_BOOT_XGSSIM) ? 0 : 1);
    SOC_SER_COUNTER_CORRECTION(unit) =
        soc_property_get(unit, spn_PARITY_COUNTER_CLEAR, 1) ? 0 : 1;
    SOC_CONTROL(unit)->l2e_ppa = soc_property_get(unit, "tr3_l2e_ppa", 1);
    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (!SOC_MEM_IS_VALID(unit, mem)) {
            sop->memState[mem].index_max = -1;
            continue;
        }
        sop->memState[mem].index_max = SOC_MEM_INFO(unit, mem).index_max;
        if (soc_feature(unit, soc_feature_xy_tcam) &&
            SOC_CONTROL(unit)->tcam_protect_write &&
            (SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_CAM) &&
            (!(SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_EXT_CAM))) {
            switch (mem) {
              case FP_TCAMm:
              case FP_GM_FIELDSm:
              case FP_GLOBAL_MASK_TCAMm:
              case FP_GLOBAL_MASK_TCAM_Xm:
              case FP_GLOBAL_MASK_TCAM_Ym:
              case VFP_TCAMm:
              case EFP_TCAMm:
              case FP_UDF_TCAMm:
#if defined BCM_TOMAHAWK_SUPPORT
              case IFP_TCAMm:
              case IFP_TCAM_PIPE0m:
              case IFP_TCAM_PIPE1m:
              case IFP_TCAM_PIPE2m:
              case IFP_TCAM_PIPE3m:
              case IFP_TCAM_WIDEm:
              case IFP_TCAM_WIDE_PIPE0m:
              case IFP_TCAM_WIDE_PIPE1m:
              case IFP_TCAM_WIDE_PIPE2m:
              case IFP_TCAM_WIDE_PIPE3m:
              case FP_UDF_TCAM_PIPE0m:
              case FP_UDF_TCAM_PIPE1m:
              case FP_UDF_TCAM_PIPE2m:
              case FP_UDF_TCAM_PIPE3m:
              case IFP_LOGICAL_TABLE_SELECTm:
              case IFP_LOGICAL_TABLE_SELECT_PIPE0m:
              case IFP_LOGICAL_TABLE_SELECT_PIPE1m:
              case IFP_LOGICAL_TABLE_SELECT_PIPE2m:
              case IFP_LOGICAL_TABLE_SELECT_PIPE3m:
              case IFP_LOGICAL_TABLE_SELECT_TCAM_ONLYm:
              case IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE0m:
              case IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE1m:
              case IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE2m:
              case IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE3m:
              case EXACT_MATCH_LOGICAL_TABLE_SELECTm:
              case EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE0m:
              case EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE1m:
              case EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE2m:
              case EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE3m:
              case EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLYm:
              case EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE0m:
              case EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE1m:
              case EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE2m:
              case EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE3m:
#endif /* BCM_TOMAHAWK_SUPPORT */
                   break;
              default:
                   if (!(soc_feature(unit, soc_feature_utt) &&
                       (mem == L3_DEFIP_PAIR_128m ||
                        mem == L3_DEFIP_PAIR_128_ONLYm ||
                        mem == L3_DEFIPm ||
                        mem == L3_DEFIP_ONLYm))) {
                       sop->memState[mem].index_max--;
                   }
                   break;
            }
        }
#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit) &&
            !((dev_id == BCM56640_DEVICE_ID) ||
             (dev_id == BCM56643_DEVICE_ID) ||
             (dev_id == BCM56545_DEVICE_ID))) {
            int blk, copyno = COPYNO_ALL;
            SOC_MEM_BLOCK_ITER(unit, mem, blk) {
                copyno = blk;
                break;
            }
            /* Disable unavailable ESM and IESMIF mems */
            if ((SOC_BLOCK_TYPE(unit, copyno) == SOC_BLK_ETU) ||
                ((SOC_BLOCK_TYPE(unit, copyno) == SOC_BLK_IPIPE) &&
                 (SOC_MEM_ADDR_STAGE_EXTENDED(SOC_MEM_BASE(unit, mem))
                    == 0x9))) {
                sop->memState[mem].index_max = -1;
            }
        }
#endif /* BCM_TRIUMPH3_SUPPORT */
    }

    /* Need to know the bypass mode before resolving
     * which memories and features are enabled in this configuration. */
    if (SOC_IS_TRIDENT3X(unit)) {
        SOC_SWITCH_BYPASS_MODE(unit) = SOC_SWITCH_BYPASS_MODE_NONE;
    } else {
    SOC_SWITCH_BYPASS_MODE(unit) = SOC_IS_SC_CQ(unit) || SOC_IS_TD_TT(unit) ?
        soc_property_get(unit, spn_SWITCH_BYPASS_MODE,
                         SOC_SWITCH_BYPASS_MODE_NONE) :
                         SOC_SWITCH_BYPASS_MODE_NONE;
    }

    /* Initialize double-tagged mode to FALSE */
    SOC_DT_MODE(unit) = 0;

#ifdef BCM_GREYHOUND_SUPPORT
    if (SOC_IS_GREYHOUND(unit)){
        if (((dev_id & 0xF000) == 0x8000) && (dev_id != BCM53365_DEVICE_ID)) {
            if ((dev_id & 0x0060) != 0x0000) {
                /* Elkhound and Bloodhound */
                if ((dev_id & 0x00F6) == 0x0054) { /* 53454/53455 */
                    dev_id &= 0xFFF4; /* use 53454 */
                }
                if ((dev_id & 0x00F6) == 0x0056) { /* 53456/53457 */
                    dev_id &= 0xFFF6; /* use 53456*/
                }
                /* no change on Bloodhound */
            } else {
                dev_id &= 0xFF0F;
            }
        }
    }
#endif

    switch (dev_id) {
#ifdef BCM_FIREBOLT_SUPPORT
    case BCM56404_DEVICE_ID:
        soc_helix_mem_config(unit);
        break;
    case BCM53300_DEVICE_ID:
    case BCM53301_DEVICE_ID:
    case BCM53302_DEVICE_ID:
        soc_bcm53300_mem_config(unit);
        break;
#endif /* BCM_FIREBOLT_SUPPORT */
#ifdef BCM_RAPTOR_SUPPORT
     case BCM53716_DEVICE_ID:
        sop->memState[FP_TCAMm].index_max = 255;
        sop->memState[FP_POLICY_TABLEm].index_max = 255;
        sop->memState[FP_METER_TABLEm].index_max = 255;
        sop->memState[FP_COUNTER_TABLEm].index_max = 255;
        break;
     case BCM53718_DEVICE_ID:
     case BCM53714_DEVICE_ID:
        sop->memState[FP_TCAMm].index_max = 511;
        sop->memState[FP_POLICY_TABLEm].index_max = 511;
        sop->memState[FP_METER_TABLEm].index_max = 511;
        sop->memState[FP_COUNTER_TABLEm].index_max = 511;
        break;
     case BCM53724_DEVICE_ID:
     case BCM53726_DEVICE_ID:
        soc_bcm53724_mem_config(unit);
        break;
     case BCM56225_DEVICE_ID:
     case BCM56227_DEVICE_ID:
     case BCM56229_DEVICE_ID:
        soc_bcm56225_mem_config(unit);
        break;
#endif /* BCM_RAPTOR_SUPPORT */
#ifdef BCM_FIREBOLT2_SUPPORT
#ifndef EXCLUDE_BCM56324
    case BCM56322_DEVICE_ID:
    case BCM56324_DEVICE_ID:
#endif /* EXCLUDE_BCM56324 */
    case BCM56510_DEVICE_ID:
    case BCM56511_DEVICE_ID:
    case BCM56512_DEVICE_ID:
    case BCM56513_DEVICE_ID:
    case BCM56514_DEVICE_ID:
    case BCM56516_DEVICE_ID:
    case BCM56517_DEVICE_ID:
    case BCM56518_DEVICE_ID:
    case BCM56519_DEVICE_ID:
        soc_firebolt2_mem_config(unit);
        break;
#endif /* BCM_FIREBOLT2_SUPPORT */
#ifdef BCM_TRIUMPH_SUPPORT
    case BCM56620_DEVICE_ID:
    case BCM56624_DEVICE_ID:
    case BCM56626_DEVICE_ID:
    case BCM56628_DEVICE_ID:
    case BCM56629_DEVICE_ID:
        soc_triumph_esm_init_mem_config(unit);
        break;
#ifdef BCM_TRIUMPH2_SUPPORT
    case BCM56634_DEVICE_ID:
    case BCM56636_DEVICE_ID:
    case BCM56638_DEVICE_ID:
    case BCM56630_DEVICE_ID:
    case BCM56639_DEVICE_ID:
    case BCM56538_DEVICE_ID:
        soc_triumph_esm_init_mem_config(unit);
        soc_triumph2_mem_config(unit);
        break;
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_APOLLO_SUPPORT)
    case BCM56526_DEVICE_ID:
    case BCM56521_DEVICE_ID:
    case BCM56520_DEVICE_ID:
    case BCM56522_DEVICE_ID:
    case BCM56524_DEVICE_ID:
    case BCM56534_DEVICE_ID:
        soc_triumph2_mem_config(unit);
        break;
#endif /* BCM_APOLLO_SUPPORT */
#ifdef BCM_VALKYRIE2_SUPPORT
    case BCM56685_DEVICE_ID:
    case BCM56689_DEVICE_ID:
        soc_triumph2_mem_config(unit);
        break;
#endif /* BCM_VALKYRIE2_SUPPORT */
    case BCM56686_DEVICE_ID:
        sop->memState[FP_TCAMm].index_max = 2047;
        sop->memState[FP_POLICY_TABLEm].index_max = 2047;
        sop->memState[FP_METER_TABLEm].index_max = 2047;
        sop->memState[FP_COUNTER_TABLEm].index_max = 2047;
        sop->memState[FP_GLOBAL_MASK_TCAMm].index_max = 2047;

        sop->memState[EFP_TCAMm].index_max = 255;
        sop->memState[EFP_POLICY_TABLEm].index_max = 255;
        sop->memState[EFP_METER_TABLEm].index_max = 255;
        sop->memState[EFP_COUNTER_TABLEm].index_max = 255;

        sop->memState[VFP_TCAMm].index_max = 255;
        sop->memState[VFP_POLICY_TABLEm].index_max = 255;
        break;
#endif /* BCM_TRIUMPH_SUPPORT */
#ifdef BCM_SCORPION_SUPPORT
    case BCM56820_DEVICE_ID:
    case BCM56821_DEVICE_ID:
    case BCM56822_DEVICE_ID:
    case BCM56823_DEVICE_ID:
    case BCM56825_DEVICE_ID:
    case BCM56720_DEVICE_ID:
    case BCM56721_DEVICE_ID:
    case BCM56725_DEVICE_ID:
#ifdef BCM_SHADOW_SUPPORT
    case BCM88732_DEVICE_ID:
#endif /* BCM_SHADOW_SUPPORT */
        soc_scorpion_mem_config(unit);
        break;
#endif /* BCM_SCORPION_SUPPORT */
#ifdef BCM_ENDURO_SUPPORT
    case BCM56320_DEVICE_ID:
    case BCM56321_DEVICE_ID:
    case BCM56230_DEVICE_ID:
    case BCM56231_DEVICE_ID:
        soc_enduro_mem_config(unit);
        break;
#endif /* BCM_ENDURO_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
    case BCM56240_DEVICE_ID:
    case BCM56241_DEVICE_ID:
    case BCM56242_DEVICE_ID:
    case BCM56243_DEVICE_ID:
    case BCM56245_DEVICE_ID:
    case BCM56246_DEVICE_ID:
        sop->memState[L2Xm].index_max = 16383;
        sop->memState[L2_ENTRY_ONLYm].index_max = 16383;
        sop->memState[L2_HITDA_ONLYm].index_max = 2047;
        sop->memState[L3_ENTRY_ONLYm].index_max = 4095;
        sop->memState[L3_ENTRY_HIT_ONLYm].index_max = 255;
        sop->memState[L3_DEFIPm].index_max = 2047;
        sop->memState[L3_DEFIP_PAIR_128m].index_max = 1023;
        sop->memState[L3_DEFIP_ONLYm].index_max = 2047;
        sop->memState[L3_DEFIP_DATA_ONLYm].index_max = 2047;
        sop->memState[L3_DEFIP_HIT_ONLYm].index_max = 2047;
        sop->memState[L3_DEFIP_PAIR_128_ONLYm].index_max = 1023;
        sop->memState[L3_DEFIP_PAIR_128_DATA_ONLYm].index_max =1023;
        sop->memState[L3_DEFIP_PAIR_128_HIT_ONLYm].index_max = 1023;
        sop->memState[MPLS_ENTRYm].index_max = 1023;
        sop->memState[VLAN_XLATEm].index_max = 4095;
        sop->memState[VLAN_MACm].index_max = 4095;
        sop->memState[FP_COUNTER_TABLEm].index_max = 2047;
        sop->memState[FP_GLOBAL_MASK_TCAMm].index_max = 2047;
        sop->memState[FP_METER_TABLEm].index_max = 2047;
        sop->memState[FP_POLICY_TABLEm].index_max = 2047;
        sop->memState[FP_TCAMm].index_max = 2047;
        sop->memState[MMU_CBPI_0m].index_max = 2751;
        sop->memState[MMU_CBPI_1m].index_max = 2751;
        sop->memState[MMU_CBPI_2m].index_max = 2751;
        sop->memState[MMU_CBPI_3m].index_max = 2751;
        sop->memState[MMU_CBPI_4m].index_max = 2751;
        sop->memState[MMU_CBPI_5m].index_max = 2751;
        sop->memState[MMU_CBPI_6m].index_max = 2751;
        sop->memState[MMU_CBPI_7m].index_max = 2751;
        sop->memState[MMU_CBPI_8m].index_max = 2751;
        sop->memState[MMU_CBPI_9m].index_max = 2751;
        sop->memState[MMU_CBPI_10m].index_max = 2751;
        sop->memState[MMU_CBPI_11m].index_max = 2751;
        sop->memState[LLS_L0_CONFIGm].index_max = 127;
        sop->memState[LLS_L1_CONFIGm].index_max = 255;
        sop->memState[LLS_L2_PARENTm].index_max = 511;
        for(saber_counter_direction=0;
            saber_counter_direction<BCM_SABER_MAX_COUNTER_DIRECTION;
            saber_counter_direction++) {
            for(saber_counter_pool=0;
                saber_counter_pool<BCM_SABER_MAX_COUNTER_POOL;
                saber_counter_pool++) {
                sop->memState[saber_counter_table
                              [saber_counter_direction][saber_counter_pool]].
                              index_max = saber_pool_size
                                          [saber_counter_direction];
            }
        }
        if (soc_kt_mem_config(unit, dev_id) != SOC_E_NONE) {
            goto error;
        }
        break;
    case BCM55441_DEVICE_ID:
        sop->memState[FP_COUNTER_TABLEm].index_max = 2047;
        sop->memState[FP_GLOBAL_MASK_TCAMm].index_max = 2047;
        sop->memState[FP_METER_TABLEm].index_max = 2047;
        sop->memState[FP_POLICY_TABLEm].index_max = 2047;
        sop->memState[FP_TCAMm].index_max = 2047;
        sop->memState[LLS_L0_CONFIGm].index_max = 255;
        sop->memState[LLS_L1_CONFIGm].index_max = 1023;
        sop->memState[LLS_L2_PARENTm].index_max = 1023;
        if (soc_kt_mem_config(unit, dev_id) != SOC_E_NONE) {
            goto error;
        }
        break;
    case BCM56440_DEVICE_ID:
    case BCM56441_DEVICE_ID:
    case BCM56442_DEVICE_ID:
    case BCM56443_DEVICE_ID:
    case BCM56445_DEVICE_ID:
    case BCM56446_DEVICE_ID:
    case BCM56447_DEVICE_ID:
    case BCM56448_DEVICE_ID:
    case BCM56449_DEVICE_ID:
        if (soc_kt_mem_config(unit, dev_id) != SOC_E_NONE) {
            goto error;
        }
        break;
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
    case BCM55450_DEVICE_ID:
    case BCM55455_DEVICE_ID:
    case BCM56248_DEVICE_ID:
    case BCM56450_DEVICE_ID:
    case BCM56452_DEVICE_ID:
    case BCM56454_DEVICE_ID:
    case BCM56455_DEVICE_ID:
    case BCM56456_DEVICE_ID:
    case BCM56457_DEVICE_ID:
    case BCM56458_DEVICE_ID:
        if (soc_kt2_mem_config(unit, dev_id) != SOC_E_NONE) {
            goto error;
        }
        break;
#if defined(BCM_SABER2_SUPPORT)
    case BCM56260_DEVICE_ID:
    case BCM56261_DEVICE_ID:
    case BCM56262_DEVICE_ID:
    case BCM56263_DEVICE_ID:
    case BCM56265_DEVICE_ID:
    case BCM56266_DEVICE_ID:
    case BCM56267_DEVICE_ID:
    case BCM56268_DEVICE_ID:
    case BCM56460_DEVICE_ID:
    case BCM56461_DEVICE_ID:
    case BCM56462_DEVICE_ID:
    case BCM56463_DEVICE_ID:
    case BCM56465_DEVICE_ID:
    case BCM56466_DEVICE_ID:
    case BCM56467_DEVICE_ID:
    case BCM56468_DEVICE_ID:
    case BCM56233_DEVICE_ID:
        if (soc_sb2_mem_config(unit, dev_id) != SOC_E_NONE) {
            goto error;
        }
        break;
#endif
#if defined(BCM_METROLITE_SUPPORT)
    case BCM56270_DEVICE_ID:
    case BCM56271_DEVICE_ID:
    case BCM56272_DEVICE_ID:

    case BCM53460_DEVICE_ID:
    case BCM53461_DEVICE_ID:

        if (soc_ml_mem_config(unit, dev_id) != SOC_E_NONE) {
            goto error;
        }
        break;
#endif
#endif
#ifdef BCM_HURRICANE2_SUPPORT
    case BCM56150_DEVICE_ID:
    case BCM56151_DEVICE_ID:
    case BCM53347_DEVICE_ID:
    case BCM53346_DEVICE_ID:
    case BCM53344_DEVICE_ID:
    case BCM56152_DEVICE_ID:
    case BCM53333_DEVICE_ID:
    case BCM53334_DEVICE_ID:
    case BCM53342_DEVICE_ID:
    case BCM53343_DEVICE_ID:
    case BCM53393_DEVICE_ID:
    case BCM53394_DEVICE_ID:
        if (soc_hu2_mem_config(unit, dev_id) != SOC_E_NONE) {
            goto error;
        }
        break;
#endif
#ifdef BCM_HELIX4_SUPPORT
    case BCM56548H_DEVICE_ID:
    case BCM56548_DEVICE_ID:
    case BCM56547_DEVICE_ID:
    case BCM56346_DEVICE_ID:
    case BCM56345_DEVICE_ID:
    case BCM56344_DEVICE_ID:
    case BCM56342_DEVICE_ID:
    case BCM56340_DEVICE_ID:
    case BCM56049_DEVICE_ID:
    case BCM56048_DEVICE_ID:
    case BCM56047_DEVICE_ID:
    case BCM56042_DEVICE_ID:
    case BCM56041_DEVICE_ID:
    case BCM56040_DEVICE_ID:
        if (soc_hx4_mem_config(unit, dev_id) != SOC_E_NONE) {
            goto error;
        }
        break;
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    case BCM56540_DEVICE_ID:
    case BCM56541_DEVICE_ID:
    case BCM56542_DEVICE_ID:
    case BCM56543_DEVICE_ID:
    case BCM56544_DEVICE_ID:
    case BCM56545_DEVICE_ID:
    case BCM56546_DEVICE_ID:
    case BCM56648_DEVICE_ID:
    case BCM56649_DEVICE_ID:
    case BCM56640_DEVICE_ID:
    case BCM56643_DEVICE_ID:
    case BCM56644_DEVICE_ID:
    case BCM56044_DEVICE_ID:
    case BCM56045_DEVICE_ID:
    case BCM56046_DEVICE_ID:
        soc_triumph3_esm_init_mem_config(unit);
        if (soc_tr3_mem_config(unit, dev_id, rev_id) != SOC_E_NONE) {
            goto error;
        }
        break;
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_TRIDENT_SUPPORT)
    case BCM56838_DEVICE_ID:
    case BCM56847_DEVICE_ID:
    case BCM56835_DEVICE_ID:
    case BCM56831_DEVICE_ID:
    case BCM56742_DEVICE_ID:
        if (soc_trident_mem_config(unit) < 0) {
            goto error;
        }
        break;
#endif /* BCM_TRIDENT_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
    case BCM56860_DEVICE_ID:
    case BCM56850_DEVICE_ID:
    case BCM56851_DEVICE_ID:
    case BCM56852_DEVICE_ID:
    case BCM56853_DEVICE_ID:
    case BCM56854_DEVICE_ID:
    case BCM56855_DEVICE_ID:
    case BCM56834_DEVICE_ID:
    case BCM56750_DEVICE_ID:
    case BCM56830_DEVICE_ID:
    case BCM56861_DEVICE_ID:
    case BCM56862_DEVICE_ID:
    case BCM56864_DEVICE_ID:
    case BCM56865_DEVICE_ID:
    case BCM56866_DEVICE_ID:
    case BCM56867_DEVICE_ID:
    case BCM56868_DEVICE_ID:
    case BCM56832_DEVICE_ID:
    case BCM56833_DEVICE_ID:
    case BCM56836_DEVICE_ID:
        if (soc_trident2_mem_config(unit) < 0) {
            goto error;
        }
        break;
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_GREYHOUND_SUPPORT)
    case BCM53400_DEVICE_ID:
    case BCM53401_DEVICE_ID:
    case BCM53402_DEVICE_ID:
    case BCM53403_DEVICE_ID:
    case BCM53404_DEVICE_ID:
    case BCM53405_DEVICE_ID:
    case BCM53365_DEVICE_ID:
    case BCM53406_DEVICE_ID:
    case BCM53369_DEVICE_ID:
    case BCM53408_DEVICE_ID:
    case BCM56060_DEVICE_ID:
    case BCM56062_DEVICE_ID:
    case BCM56063_DEVICE_ID:
    case BCM56064_DEVICE_ID:
    case BCM56065_DEVICE_ID:
    case BCM56066_DEVICE_ID:
    case BCM53454_DEVICE_ID:
    case BCM53456_DEVICE_ID:
    case BCM53422_DEVICE_ID:
    case BCM53424_DEVICE_ID:
    case BCM53426_DEVICE_ID:
        if (soc_greyhound_mem_config(unit, dev_id) < 0) {
            goto error;
        }
        break;
#endif /* BCM_GREYHOUND_SUPPORT */
#if defined(BCM_TOMAHAWK_SUPPORT)
    case BCM56960_DEVICE_ID:
    case BCM56961_DEVICE_ID:
    case BCM56962_DEVICE_ID:
    case BCM56963_DEVICE_ID:
    case BCM56930_DEVICE_ID:
    case BCM56968_DEVICE_ID:
#if defined(BCM_TOMAHAWKPLUS_SUPPORT)
    case BCM56965_DEVICE_ID:
    case BCM56969_DEVICE_ID:
    case BCM56966_DEVICE_ID:
    case BCM56967_DEVICE_ID:
#endif /* BCM_TOMAHAWKPLUS_SUPPORT */
    case BCM56168_DEVICE_ID:
    case BCM56169_DEVICE_ID:
        if (soc_tomahawk_mem_config(unit) < 0) {
            goto error;
        }
        break;
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TOMAHAWK3_SUPPORT)
    case BCM56980_DEVICE_ID:
    case BCM56981_DEVICE_ID:
    case BCM56982_DEVICE_ID:
    case BCM56983_DEVICE_ID:
    case BCM56984_DEVICE_ID:
        if (soc_tomahawk3_mem_config(unit) < 0) {
            goto error;
        }
        break;
#endif /* BCM_TOMAHAWK3_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
    case BCM56870_DEVICE_ID:
    case BCM56873_DEVICE_ID:
        if (soc_trident3_mem_config(unit) < 0) {
            goto error;
        }
        break;
#endif /* BCM_TRIDENT3_SUPPORT */
#if defined(BCM_MAVERICK2_SUPPORT)
    case BCM56770_DEVICE_ID:
    case BCM56771_DEVICE_ID:
        if (soc_maverick2_mem_config(unit) < 0) {
            goto error;
        }
        break;
#endif /* BCM_MAVERICK2_SUPPORT */
#if defined(BCM_APACHE_SUPPORT)
    case BCM56560_DEVICE_ID:
    case BCM56561_DEVICE_ID:
    case BCM56565_DEVICE_ID:
    case BCM56566_DEVICE_ID:
    case BCM56762_DEVICE_ID:
    case BCM56764_DEVICE_ID:
    case BCM56766_DEVICE_ID:
        if (soc_apache_mem_config(unit) < 0) {
            goto error;
        }
        break;
#endif /* BCM_APACHE_SUPPORT */
#if defined(BCM_HURRICANE3_SUPPORT)
        case BCM56160_DEVICE_ID:
        case BCM56162_DEVICE_ID:
        case BCM56163_DEVICE_ID:
        case BCM56164_DEVICE_ID:
        case BCM56166_DEVICE_ID:
        case BCM53440_DEVICE_ID:
        case BCM53443_DEVICE_ID:
        case BCM53442_DEVICE_ID:
        case BCM53434_DEVICE_ID:
            if (soc_hurricane3_mem_config(unit, dev_id) < 0) {
                goto error;
            }
            break;
        case BCM53540_DEVICE_ID:  /* WH2 */
        case BCM53547_DEVICE_ID:
        case BCM53548_DEVICE_ID:
        case BCM53549_DEVICE_ID:
            if (soc_wolfhound2_mem_config(unit, dev_id) < 0) {
                goto error;
            }
            break;
#endif /* BCM_HURRICANE3_SUPPORT */
#if defined(BCM_TOMAHAWK2_SUPPORT)
    case BCM56970_DEVICE_ID:
    case BCM56971_DEVICE_ID:
    case BCM56972_DEVICE_ID:
    case BCM56974_DEVICE_ID:
    case BCM56975_DEVICE_ID:
        if (soc_tomahawk_mem_config(unit) < 0) {
            goto error;
        }
        break;
#endif /* BCM_TOMAHAWK2_SUPPORT */

#if defined(BCM_GREYHOUND2_SUPPORT)
        case BCM56170_DEVICE_ID:
        case BCM56172_DEVICE_ID:
        case BCM56174_DEVICE_ID:
        case BCM53570_DEVICE_ID:
        case BCM53575_DEVICE_ID:
            if (soc_greyhound2_mem_config(unit, dev_id) < 0) {
                goto error;
            }
            break;
#endif /* BCM_GREYHOUND2_SUPPORT */

#if defined(BCM_FIRELIGHT_SUPPORT)
        case BCM56070_DEVICE_ID:
        case BCM56071_DEVICE_ID:
        case BCM56072_DEVICE_ID:
            if (soc_firelight_mem_config(unit, dev_id) < 0) {
                goto error;
            }
            break;
#endif /* BCM_FIRELIGHT_SUPPORT */

    default:
        switch (drv_dev_id) {
#if defined(BCM_MONTEREY_SUPPORT)
        case BCM56670_DEVICE_ID:
        case BCM56671_DEVICE_ID:
        case BCM56672_DEVICE_ID:
        case BCM56675_DEVICE_ID:
            if (soc_monterey_mem_config(unit) < 0) {
                goto error;
            }
            break;
#endif /* BCM_MONTEREY_SUPPORT */
#if defined(BCM_APACHE_SUPPORT)
        case BCM56560_DEVICE_ID:
            if (soc_apache_mem_config(unit) < 0) {
                goto error;
            }
            break;
#endif /* BCM_APACHE_SUPPORT */
#if defined(BCM_HURRICANE4_SUPPORT)
    case BCM56275_DEVICE_ID:
        if (soc_hurricane4_mem_config(unit) < 0) {
            goto error;
        }
        break;
#endif /* BCM_HURRICANE4_SUPPORT */
#if defined(BCM_HELIX5_SUPPORT)
        case BCM56370_DEVICE_ID:
            if (soc_helix5_mem_config(unit) < 0) {
                goto error;
            }
            break;
#endif /* BCM_HELIX5_SUPPORT */
#if defined(BCM_FIREBOLT6_SUPPORT)
    case BCM56470_DEVICE_ID:
        if (soc_firebolt6_mem_config(unit) < 0) {
            goto error;
        }
        break;
#endif /* BCM_FIREBOLT6_SUPPORT */
        default:
            break;
        }
            break;
    }
#ifdef BCM_ESW_SUPPORT
    if (soc_feature(unit, soc_feature_ipmc_reduced_table_size)) {
        sop->memState[L3_IPMCm].index_max = 2047;
    }
#endif /* BCM_ESW_SUPPORT */

#if defined(BCM_ESW_SUPPORT) && defined(BCM_IPROC_SUPPORT)
    /*
     * Update memState based on the actual table sizes on the SKU using
     * BondOptions info read from the relevant capability registers
     */
    if (soc_feature(unit, soc_feature_untethered_otp)) {
        rv = soc_mem_config_bondoptions_adjust(unit);
        if (rv != SOC_E_NONE) {
            goto error;
        }
    }
#endif /* BCM_ESW_SUPPORT && BCM_IPROC_SUPPORT */


#if defined(BCM_ESW_SUPPORT) || \
    defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
#if !defined(SOC_NO_NAMES)
    if (soc_property_get(unit, spn_MEM_CHECK_MAX_OVERRIDE, 0)) {
        for (mem = 0; mem < NUM_SOC_MEM; mem++) {
            uint32      max;
            uint32      max_adj;
            char        mem_name[128];
            char        *mptr;

            if (!SOC_MEM_IS_VALID(unit, mem)) {
                continue;
            }
            max = sop->memState[mem].index_max;

            /* coverity[secure_coding] */
            sal_strcpy(mem_name, "memmax_");
            mptr = &mem_name[sal_strlen(mem_name)];
            sal_strcpy(mptr, SOC_MEM_NAME(unit, mem));
            max_adj = soc_property_get(unit, mem_name, max);
            if (max_adj == max) {
                sal_strcpy(mptr, SOC_MEM_UFNAME(unit, mem));
                max_adj = soc_property_get(unit, mem_name, max);
            }
            if (max_adj == max) {
                sal_strcpy(mptr, SOC_MEM_UFALIAS(unit, mem));
                max_adj = soc_property_get(unit, mem_name, max);
            }
            sop->memState[mem].index_max = max_adj;

        }
    }
#endif /* !SOC_NO_NAMES */
#endif
    /* Allocate counter module resources */
    if (soc_counter_attach(unit)) {
        goto error;
    }

    /* Allocate MMU LLS usage map */
#if defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_APACHE(unit)) {
if (!SOC_IS_MONTEREY(unit)) {
        if (soc_apache_lls_bmap_alloc(unit) < 0) {
            goto error;
        }
}
    } else
#endif /* BCM_APACHE_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
    if (!SOC_IS_TOMAHAWKX(unit) && !SOC_IS_MONTEREY(unit) && SOC_IS_TD2_TT2(unit)) {
        if (!soc_feature(unit, soc_feature_no_lls)) {
            if (soc_trident2_lls_bmap_alloc(unit) < 0) {
                goto error;
            }
        }
    }
#endif /* BCM_TRIDENT2_SUPPORT */

#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            if (soc_tr3_lls_bmap_alloc(unit) < 0) {
                goto error;
            }
        }
#endif /* BCM_TRIUMPH3_SUPPORT */

    /* Allocate L2X module and shadow table resources */

#ifdef BCM_XGS_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_arl_hashed) &&
        soc_l2x_attach(unit) < 0) {
        goto error;
    }
#endif

    /*
     * Create binary semaphores for interrupt signals, initially empty
     * making us block when we try to "take" on them.  In soc_intr(),
     * when we receive the interrupt, a "give" is performed, which will
     * wake us back up.
     */

    for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit) + 1; cmc++) {
        if ((soc->schanIntr[cmc] =
             sal_sem_create("SCHAN interrupt", sal_sem_BINARY, 0)) == NULL) {
            goto error;
        }
    }

    if ((soc->miimIntr =
         sal_sem_create("MIIM interrupt", sal_sem_BINARY, 0)) == NULL) {
        goto error;
    }

    /* Initialize target device */

    if (SAL_BOOT_QUICKTURN) {
        soc->schanTimeout = SCHAN_TIMEOUT_QT;
        soc->miimTimeout = MIIM_TIMEOUT_QT;
        soc->bistTimeout = BIST_TIMEOUT_QT;
    } else if (SAL_BOOT_PLISIM) {
        soc->schanTimeout = SCHAN_TIMEOUT_PLI;
        soc->miimTimeout = MIIM_TIMEOUT_PLI;
        soc->bistTimeout = BIST_TIMEOUT_PLI;
    } else {
        soc->schanTimeout = SCHAN_TIMEOUT;
        soc->miimTimeout = MIIM_TIMEOUT;
        soc->bistTimeout = BIST_TIMEOUT;
    }

    soc->schanTimeout = soc_property_get(unit, spn_SCHAN_TIMEOUT_USEC,
                                         soc->schanTimeout);
    soc->miimTimeout = soc_property_get(unit, spn_MIIM_TIMEOUT_USEC,
                                        soc->miimTimeout);
    soc->bistTimeout = soc_property_get(unit, spn_BIST_TIMEOUT_MSEC,
                                        soc->bistTimeout);

    soc->schanIntrEnb = soc_property_get(unit, spn_SCHAN_INTR_ENABLE, 1);
    soc->schanIntrBlk = soc_property_get(unit, spn_SCHAN_ERROR_BLOCK_USEC,
                                         250000);

#ifdef INCLUDE_RCPU
    if (SOC_IS_RCPU_ONLY(unit)) {
        /*
         * Use polling mode for RCPU by defalut. If remote interrupt packet is
         * enabled for RCPU MIIM access, the register CMIC_INTR_PKT_PACING_DELAY
         * should be programmed properly to allow continuous generation of
         * interrupt packets.
         */
        soc->miimTimeout = MIIM_TIMEOUT_RCPU;
        soc->miimIntrEnb = soc_property_get(unit, spn_MIIM_INTR_ENABLE, 0);
    } else
#endif /* INCLUDE_RCPU */
    {
        /* use polling mode as default during init. Expect to save time */
        soc->miimIntrEnb = soc_property_get(unit, spn_MIIM_INTR_ENABLE, 0);
    }

    /* arl_notify is also used for Table DMA on 5690 */

    if ((soc->arl_notify =
        sal_sem_create("ARL interrupt", sal_sem_BINARY, 0)) == NULL) {
        goto error;
    }

    soc->arl_notified = 0;

    soc->arlDropWarn = 0;

    soc->arlResyncDelay =
        soc_property_get(unit, spn_ARL_RESYNC_DELAY, 3000000);

#ifdef BCM_XGS_SWITCH_SUPPORT
    if ((soc->l2x_notify =
        sal_sem_create("l2xmsg timer", sal_sem_BINARY, 0)) == NULL) {
        goto error;
    }
    if ((soc->l2x_del_sync =
        sal_mutex_create("l2xmsg delete sync")) == NULL) {
        goto error;
    }
    if ((soc->l2x_lock =
        sal_sem_create("l2xmsg lock", sal_sem_BINARY, 1)) == NULL) {
        goto error;
    }
    soc->l2x_pid = SAL_THREAD_ERROR;
    soc->l2x_interval = 0;
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        /* Clear L2 learning related data */
        soc->l2x_learn_pid = SAL_THREAD_ERROR;
        soc->l2x_learn_interval = 0;
        if (soc->l2x_lrn_shadow != NULL) {
            shr_avl_delete_all(soc->l2x_lrn_shadow);
            shr_avl_destroy(soc->l2x_lrn_shadow);
            soc->l2x_lrn_shadow = NULL;
        }

        if (NULL != soc->l2x_lrn_shadow_mutex) {
            sal_mutex_destroy(soc->l2x_lrn_shadow_mutex);
            soc->l2x_lrn_shadow_mutex = NULL;
        }
        soc->lrn_cache_threshold = -1;
        soc->lrn_cache_intr_ctl = -1;
        soc->lrn_cache_clr_on_rd = -1;
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TSN_SUPPORT
    if ((soc->l2x_sr_sync =
        sal_mutex_create("l2xmsg sr sync")) == NULL) {
        goto error;
    }
#endif /* BCM_TSN_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_KATANA2(unit) ||
        SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        if ((soc->l2x_age_notify =
            sal_sem_create("l2xAge timer", sal_sem_BINARY, 0)) == NULL) {
            goto error;
        }
        soc->l2x_age_pid = SAL_THREAD_ERROR;
        soc->l2x_age_interval = 0;
        soc->l2x_agetime_adjust_usec = 0;
        soc->l2x_agetime_curr_timeblk = 0;
        soc->l2x_agetime_curr_timeblk_usec = 0;
        soc->l2x_prev_age_timeout= 0;
    }
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit)) {
        if ((soc->l2x_age_notify =
            sal_sem_create("l2xAge timer", sal_sem_BINARY, 0)) == NULL) {
            goto error;
        }
        if ((soc->l2x_age_sync =
            sal_sem_create("l2xAge sync", sal_sem_BINARY, 1)) == NULL) {
            goto error;
        }
        soc->l2x_age_pid = SAL_THREAD_ERROR;
        soc->l2x_age_interval = 0;
        soc->l2x_age_enable = 0;
        soc->l2x_sw_aging = 0;
    }
#endif
#endif

    /* Create L3 module protection mutex. */
    if ((soc->l3x_lock = sal_mutex_create("L3 module mutex")) == NULL) {
        goto error;
    }

    /* Create FP module protection mutex. */
    if ((soc->fp_lock = sal_mutex_create("FP module mutex")) == NULL) {
        goto error;
    }

#ifdef INCLUDE_XFLOW_MACSEC
        if (soc_feature(unit, soc_feature_xflow_macsec)) {
            /* Create XFLOW_MACSEC module protection mutex. */
            if ((soc->xflow_macsec_lock =
                sal_mutex_create("XFLOW_MACSEC module mutex")) == NULL) {
                goto error;
            }
        }
#endif

    /* Create UDF module protection mutex. */
    if ((soc->udf_lock = sal_mutex_create("UDF module mutex")) == NULL) {
        goto error;
    }

#ifdef BCM_SBUSDMA_SUPPORT
    if (soc_feature(unit, soc_feature_sbusdma)) {
        if (soc_sbusdma_init(unit, soc_property_get(unit, spn_DMA_DESC_TIMEOUT_USEC, 0),
                                  soc_property_get(unit, spn_DMA_DESC_INTR_ENABLE, 0))) {
            goto error;
        }
    }
#endif

    soc->arl_pid = SAL_THREAD_ERROR;
    soc->arl_dma_cnt0 = 0;
    soc->arl_dma_xfer = 0;
    soc->arl_mbuf_done = 0;
    soc->arl_msg_drop = 0;
    soc->arl_exit = 1;
    soc->arlNextBuf = 0;
    soc->arlRateLimit = soc_property_get(unit, spn_ARL_RATE_LIMIT, 3000);

    if ((soc->ipfixIntr =
         sal_sem_create("IPFIX interrupt", sal_sem_BINARY, 0)) == NULL) {
        goto error;
    }

#if defined(INCLUDE_REGEX)
    if ((soc->ftreportIntr =
         sal_sem_create("Regex interrupt", sal_sem_BINARY, 0)) == NULL) {
        goto error;
    }
#endif
#ifdef BCM_FLOWTRACKER_EXPORT_FIFO_SUPPORT
    if ((soc->ftExportIntr =
         sal_sem_create("FT DMA Export interrupt", sal_sem_BINARY, 0)) == NULL) {
        goto error;
    }
#endif /* BCM_FLOWTRACKER_EXPORT_FIFO_SUPPORT */

    soc->ctr_evict_pid = SAL_THREAD_ERROR;
    soc->ctr_evict_interval = 0;
    soc->ctrEvictIntr = sal_sem_create("Counter eviction interrupt",
                                       sal_sem_BINARY, 0);
    if (soc->ctrEvictIntr == NULL) {
        goto error;
    }

#ifdef BCM_HERCULES_SUPPORT
    soc->mmu_errors = NULL;

    if (SOC_IS_HERCULES(unit)) {
        int port, entry_count;
        if ((soc->lla_cells_good =
             sal_alloc(SOC_MAX_NUM_PORTS * sizeof(uint32),
                       "LLA good cells count")) == NULL) {
            goto error;
        }

        if ((soc->lla_map =
             sal_alloc(SOC_MAX_NUM_PORTS * sizeof(SHR_BITDCL *),
                       "LLA map cache pointers")) == NULL) {
            goto error;
        }

        if ((soc->sbe_disable =
             sal_alloc(SOC_MAX_NUM_PORTS * sizeof(int),
                       "PP SBE intr port disable")) == NULL) {
            goto error;
        }

        entry_count = soc_mem_index_count(unit, MEM_LLAm);
        PBMP_ALL_ITER(unit, port) {
            soc->lla_cells_good[port] = entry_count;
            soc->lla_map[port] = NULL;
            soc->sbe_disable[port] = FALSE;
        }
    } else {
        soc->lla_cells_good = NULL;
        soc->lla_map = NULL;
        soc->sbe_disable = NULL;
    }
#endif

    /* Clear statistics */

    sal_memset(&soc->stat, 0, sizeof (soc->stat));

    /* Initialize link forwarding bitmaps */

    SOC_PBMP_CLEAR(sop->link_fwd);

#ifdef INCLUDE_RCPU
    if (SOC_IS_RCPU_ONLY(unit)) {
        SOC_PBMP_PORT_ADD(sop->link_fwd, CMIC_PORT(unit));
        if (SOC_PORT_VALID(unit, RCPU_PORT(unit))) {
            SOC_PBMP_PORT_ADD(sop->link_fwd, RCPU_PORT(unit));
        }
    }
#endif /* INCLUDE_RCPU */

    soc->link_mask2 = PBMP_ALL(unit);

    soc->soc_link_pause = 0;

#ifdef BCM_RAVEN_SUPPORT
        if (SOC_IS_RAVEN(unit)) {

            /* Assume:
             *   - All ports are internal (max of 128 ports)
             *   - Internal ports require 3 states in the hardware state machine
             *   - CMIC is running at 100MHz
             */
            soc->hw_linkscan_delay_ns = 128 * 3 * 10;
        }
#endif

    /*
     * Configure nominal IPG register settings.
     * By default the IPG should be 96 bit-times.
     */
    if (SOC_IS_KATANA2(unit)) {
        /* SOC_PBMP_ASSIGN(pbmp_all,PBMP_ALL(unit)); */
        for (port =  SOC_INFO(unit).cmic_port;
             port <= SOC_INFO(unit).lb_port;
             port++) {
             SOC_PBMP_PORT_ADD(pbmp_all,port);
        }
    } else {
        SOC_PBMP_ASSIGN(pbmp_all,PBMP_ALL(unit));
    }
    /* PBMP_ALL_ITER(unit, port) { */
    SOC_PBMP_ITER(pbmp_all, port) {
        sop->ipg[port].hd_10    = 96;
        sop->ipg[port].hd_100   = 96;
        sop->ipg[port].hd_1000  = 96;
        sop->ipg[port].hd_2500  = 96;

        sop->ipg[port].fd_10    = 96;
        sop->ipg[port].fd_100   = 96;
        sop->ipg[port].fd_1000  = 96;
        sop->ipg[port].fd_2500  = 96;
        sop->ipg[port].fd_10000 = 96;
        sop->ipg[port].fd_xe    = 96;
        sop->ipg[port].fd_hg    = 64;
        sop->ipg[port].fd_hg2   = 96;
    }

    /*
     * Initialize CES control structs
     */
#if defined(BCM_KATANA_SUPPORT)
    soc->ces_ctrl = (void*)NULL;
    soc->tdm_ctrl = (void*)NULL;
#endif

    if (soc_schan_init(unit) != SOC_E_NONE) {
       goto error;
    }

    LOG_CLI((BSL_META_U(unit,
                        "SOC unit %d attached to %s device %s\n"),
             unit, (soc->soc_flags & SOC_F_RCPU_ONLY) ? "RCPU" : "PCI",
             soc_dev_name(unit)));

#ifdef PORTMOD_SUPPORT
    if (SOC_USE_PORTCTRL(unit)) {
        SOC_IF_ERROR_RETURN(
            soc_esw_portctrl_functions_register(unit, soc->chip_driver));
    }
#endif

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)&& !(SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "reset m0ssq\n")));
        /* Put M0 core in reset */
        soc_iproc_m0ssq_reset(unit);
#ifdef BCM_ESW_SUPPORT
        /* Load default LED and Linkscan firmware. */
        if (SOC_IS_ESW(unit)) {
            soc_cmicx_led_linkscan_default_fw_load(unit);
        }
#endif
    }
#endif

#ifdef BCM_GREYHOUND_SUPPORT
    /* To pause L2 hardware learning when tables are being written. */
    if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit)) {
        if (soc_feature(unit, soc_feature_oam) &&
            soc_property_get(unit, spn_OAM_PAUSE_L2_LEARN_ON_TABLE_WRITE, 0)) {
            soc->l2x_learn_pause_on_table_write = 1;
        }
    }
#endif /* BCM_GREYHOUND_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_property_get(unit, spn_SW_TIMESTAMP_FIFO_ENABLE, 1)) {
        soc->sw_timestamp_fifo_enable = 1;
    }
#endif /* BCM_TRIUMPH2_SUPPORT | BCM_TRIUMPH3_SUPPORT */

    soc->max_num_pipe = soc_property_get(unit, "num_pipe", NUM_PIPE(unit));

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        if (dev_id == BCM56983_DEVICE_ID ||
            soc_property_get(unit, "th3_em_halfchip", 0)) {
            soc->halfchip = 1;
        }
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

    return(SOC_E_NONE);

 error:
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "soc_attach: unit %d failed\n"), unit));

    /* COVERITY
     * Intentional stack usage
     */
    /* coverity[stack_use_return : FALSE] */
    /* coverity[stack_use_callee_max : FALSE] */
    /* coverity[stack_use_overflow : FALSE] */
    soc_detach(unit);           /* Perform necessary clean-ups on error */
    return rv;
}

/*
 * Function:
 *      soc_bpdu_addr_set
 * Purpose:
 *      Set BPDU address
 * Parameters:
 *      unit - StrataSwitch unit #
 *      index - bpdu index
 *      addr - mac address
 * Returns:
 *      SOC_E_XXX - on error
 *      SOC_E_NONE - success
 */
int
soc_bpdu_addr_set(int unit, int index, sal_mac_addr_t addr)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        l2u_entry_t entry;
        uint32 mask[3];
        int     skip_l2u;

        skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);
        if (skip_l2u) {
            return SOC_E_UNAVAIL;
        }

        if ((index < 0) ||
            (index >= soc_mem_index_count(unit, L2_USER_ENTRYm))) {
            return SOC_E_PARAM;
        }
        sal_memset(&entry, 0, sizeof(entry));
        soc_L2_USER_ENTRYm_field32_set(unit, &entry, VALIDf, 1);
        soc_L2_USER_ENTRYm_field32_set(unit, &entry, CPUf, 1);
        soc_L2_USER_ENTRYm_field32_set(unit, &entry, BPDUf, 1);
        soc_mem_mac_addr_set(unit, L2_USER_ENTRYm, &entry, MAC_ADDRf, addr);
        mask[0] = 0xffffffff;
        mask[1] = 0x0000ffff;
        mask[2] = 0;
        soc_L2_USER_ENTRYm_field_set(unit, (uint32 *)&entry, MASKf, mask);
#if defined(BCM_RAVEN_SUPPORT)
        if (soc_mem_field_valid(unit, L2_USER_ENTRYm, RESERVED_MASKf)) {
            soc_mem_field32_set(unit, L2_USER_ENTRYm, &entry, RESERVED_MASKf, 0);
        }
        if (soc_mem_field_valid(unit, L2_USER_ENTRYm, RESERVED_KEYf)) {
            soc_mem_field32_set(unit, L2_USER_ENTRYm, &entry, RESERVED_KEYf, 0);
        }
#endif /* BCM_RAVEN_SUPPORT */
        SOC_IF_ERROR_RETURN(soc_l2u_insert(unit, &entry, index, &index));
        return SOC_E_NONE;
    }
#endif
    return SOC_E_UNAVAIL;
}

int
soc_bpdu_addr_get(int unit, int index, sal_mac_addr_t *addr)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        l2u_entry_t entry;
        int     skip_l2u;

        skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);
        if (skip_l2u) {
            return SOC_E_UNAVAIL;
        }

        if ((index < 0) ||
            (index >= soc_mem_index_count(unit, L2_USER_ENTRYm))) {
            return SOC_E_PARAM;
        }
        SOC_IF_ERROR_RETURN(soc_l2u_get(unit, &entry, index));
        if ((0 == soc_L2_USER_ENTRYm_field32_get(unit, &entry, VALIDf)) ||
             (0 == soc_L2_USER_ENTRYm_field32_get(unit, &entry, BPDUf))) {
            /* Not a valid BPDU entry */
            return SOC_E_NOT_FOUND;
        }
        soc_mem_mac_addr_get(unit, L2_USER_ENTRYm, &entry, MAC_ADDRf, *addr);
        return SOC_E_NONE;
    }
#endif
    return SOC_E_UNAVAIL;
}

/*
 * Function:
 *      soc_esw_hw_qnum_get
 * Purpose:
 *      Get queue to use for PBSMH packet transmission.
 * Parameters:
 *      unit - SOC unit #
 *      port - SOC port #
 *      cos - CoS (from Tx packet)
 *      qnum - (OUT) H/W queue to use
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_hw_qnum_get(int unit, int port, int cos, int *qnum)
{
    switch (SOC_DCB_TYPE(unit)) {
#ifdef BCM_KATANA_SUPPORT
    case 24:
    case 29:
        *qnum = SOC_INFO(unit).port_uc_cosq_base[port] + cos;
        break;
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    case 23:
        *qnum = SOC_INFO(unit).port_uc_cosq_base[port] + cos;
        break;
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
        
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    case 26:
    case 33:
        *qnum = SOC_INFO(unit).port_uc_cosq_base[port] + cos;
        *qnum = soc_td2_logical_qnum_hw_qnum(unit, port, *qnum, 1);
        break;
#endif
#ifdef BCM_APACHE_SUPPORT
    case 35:
        *qnum = SOC_INFO(unit).port_uc_cosq_base[port] + cos;
#ifdef BCM_MONTEREY_SUPPORT
        if (SOC_IS_MONTEREY(unit)) {
            *qnum = soc_monterey_logical_qnum_hw_qnum(unit, port, *qnum, 1);
        } else
#endif
        {
        *qnum = soc_apache_logical_qnum_hw_qnum(unit, port, *qnum, 1);
        }
        break;
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    case 36:
        
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    case 38:
        
#endif

    default:
        *qnum = cos;
        break;
    }
    return SOC_E_NONE;
}

#ifdef INCLUDE_RCPU
/*
 * Function:
 *      soc_rcpu_schan_op_register
 * Purpose:
 *      Registers SCHAN operation routine over RCPU mechanism
 * Parameters:
 *      unit - SOC unit #
 *      f    - (IN) Function to be called for SCHAN operations over RCPU
 * Returns:
 *      SOC_E_XXX
 */
int
soc_rcpu_schan_op_register(int unit, soc_rcpu_schan_op_cb f)
{
    if (!SOC_IS_RCPU_UNIT(unit) && !SOC_IS_RCPU_ONLY(unit)) {
        return SOC_E_UNAVAIL;
    }

    if (NULL == f) {
        return SOC_E_PARAM;
    }

    SOC_CONTROL(unit)->soc_rcpu_schan_op = f;

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_rcpu_schan_op_unregister
 * Purpose:
 *      Unregisters SCHAN operation routine over RCPU mechanism
 *      Schan will be performed via normal flow (PCI or EB)
 * Parameters:
 *      unit - SOC unit #
 * Returns:
 *      SOC_E_XXX
 */
int
soc_rcpu_schan_op_unregister(int unit)
{
    if (!SOC_IS_RCPU_UNIT(unit) && !SOC_IS_RCPU_ONLY(unit)) {
        return SOC_E_UNAVAIL;
    }

    SOC_CONTROL(unit)->soc_rcpu_schan_op = NULL;

    return SOC_E_NONE;
}

#endif /* INCLUDE_RCPU */

#ifdef BCM_COSQ_HIGIG_MAP_DISABLE
static const soc_field_t cos_field[8] = {
    COS0f,  COS1f,  COS2f,  COS3f,  COS4f,  COS5f,  COS6f,  COS7f
};

/*
 * Function:
 *      soc_cosq_higig_map_disable
 * Purpose:
 *      Initialize the identity mapping for ipics
 * Parameters:
 *      unit - SOC unit #
 * Returns:
 *      SOC_E_XXX
 */
int
soc_cosq_stack_port_map_disable(int unit)
{
    soc_port_t  port;
    uint64  val64;
    int         prio;

    COMPILER_64_ZERO(val64);
    for (prio = 0; prio < 8; prio++) {
        soc_reg64_field32_set(unit, ICOS_SELr, &val64,
                              cos_field[prio], prio);
    }
    PBMP_ST_ITER(unit, port) {
        /* map prio0->cos0, prio1->cos1, ... , prio7->cos7 */
        SOC_IF_ERROR_RETURN(WRITE_ICOS_SELr(unit, port, val64));
    }
    if (SOC_IS_XGS3_SWITCH(unit)) {
        SOC_IF_ERROR_RETURN(WRITE_ICOS_SELr(unit, (CMIC_PORT(unit)), val64));
    }
    return SOC_E_NONE;
}
#endif /* BCM_COSQ_HIGIG_MAP_DISABLE */

#ifdef BCM_XGS3_SWITCH_SUPPORT
static const soc_field_t prio_field[8] = {
    PRI0f,  PRI1f,  PRI2f,  PRI3f,  PRI4f,  PRI5f,  PRI6f,  PRI7f
};

/*
 * Function:
 *      soc_cpu_priority_mapping_init
 * Purpose:
 *      Initialize the identity mapping for ipics
 * Parameters:
 *      unit - SOC unit #
 * Returns:
 *      SOC_E_XXX
 */
int
soc_cpu_priority_mapping_init(int unit)
{
    int         prio;
    uint32  val;

    val = 0;
    for (prio = 0; prio < 8; prio++) {
        soc_reg_field_set(unit, CPU_PRIORITY_SELr, &val,
                              prio_field[prio], prio);
    }
    SOC_IF_ERROR_RETURN(WRITE_CPU_PRIORITY_SELr(unit, val));
    return SOC_E_NONE;
}
#endif /* BCM_XGS3_SWITCH_SUPPORT */

int soc_esw_div64(uint64 x, uint32 y, uint32 *result)
{
    uint64 rem;
    uint64 b;
    uint64 res, d;
    uint32 high;

    COMPILER_64_SET(rem, COMPILER_64_HI(x), COMPILER_64_LO(x));
    COMPILER_64_SET(b, 0, y);
    COMPILER_64_SET(d, 0, 1);

    high = COMPILER_64_HI(rem);

    COMPILER_64_ZERO(res);
    if (high >= y) {
        /* NOTE: Follow code is used to handle 64bits result
         *  high /= y;
         *  res = (uint64_t) (high << 32);
         *  rem -= (uint64_t)((high * y) << 32);
         */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("soc_sbx_div64: result > 32bits\n")));
        return SOC_E_PARAM;
    }

    while ((!COMPILER_64_BITTEST(b, 63)) &&
           (COMPILER_64_LT(b, rem)) ) {
        COMPILER_64_ADD_64(b,b);
        COMPILER_64_ADD_64(d,d);
    }

    do {
        if (COMPILER_64_GE(rem, b)) {
            COMPILER_64_SUB_64(rem, b);
            COMPILER_64_ADD_64(res, d);
        }
        COMPILER_64_SHR(b, 1);
        COMPILER_64_SHR(d, 1);
    } while (!COMPILER_64_IS_ZERO(d));

    *result = COMPILER_64_LO(res);

    /*
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "%s: divisor 0x%x%8.8x dividor 0x%x result 0x%x\n"),
                 FUNCTION_NAME(), high, COMPILER_64_LO(x), y, *result));
    */
    return 0;
}


/*
 * Function:
 *      soc_warpcore_firmware_set
 * Purpose:
 *      Load Warpcore PHY firmware using hardware acceleration
 * Parameters:
 *      unit - unit number
 *      port - device port number
 *      array - (IN) Warpcore firmware data stream pointer
 *      datalen - length of firmware data stream
 *      wc_instance - number of the Warpcore in this port block
 *      wc_ucmem_data - Device specific memory for loading WC firmware
 *      wc_ucmem_ctrl - Device specific register for selecting WC port
 * Returns:
 *      SOC_E_XXX
 */
int
soc_warpcore_firmware_set(int unit, int port,
                          uint8 *array, int datalen, int wc_instance,
                          soc_mem_t wc_ucmem_data, soc_reg_t wc_ucmem_ctrl)
{
    soc_control_t *soc;
    int entry_bytes, entry_num;
    int blk, skip_port = 0, alloc_size = 0;
    int count, extra_bytes, i, j;
    uint8 *array_ptr, *dma_buf_ptr = NULL;
    static int arr_pos_le[3] [16]
        = {{15, 14, 13, 12, 11, 10,  9,  8, 7, 6, 5, 4, 3, 2, 1, 0},
           { 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7},
           { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};
    static int arr_pos_be[3] [16]
        = {{12, 13, 14, 15,  8,  9, 10, 11, 4, 5, 6, 7, 0, 1, 2, 3},
            {11, 10,  9,  8, 15, 14, 13, 12, 3, 2, 1, 0, 7, 6, 5, 4},
            {3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12}};
    int *arr_pos;
    int pio, packet, other;
    int phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    uint32  be_host;

    soc = SOC_CONTROL(unit);

    entry_bytes = soc_mem_entry_bytes(unit, wc_ucmem_data);
    entry_num = soc_mem_index_count(unit, wc_ucmem_data);
    if (datalen > (entry_bytes * entry_num)) {
        /* Can't fit all of the firmware into the device load table. */
        return SOC_E_RESOURCE;
    }

    soc_cm_get_endian(unit, &pio, &packet, &other);
    if (other) {
        if (soc_feature(unit, soc_feature_tsce) ||
            soc_feature(unit, soc_feature_tscf)) {
            arr_pos = arr_pos_be[2];
        } else if (SOC_IS_TD2_TT2(unit)) {
            arr_pos = arr_pos_be[1];
        } else {
            arr_pos = arr_pos_be[0];
        }
    } else {
        if (soc_feature(unit, soc_feature_tsce) ||
            soc_feature(unit, soc_feature_tscf)) {
            arr_pos = arr_pos_le[2];
        } else if (SOC_IS_TD2_TT2(unit)) {
            arr_pos = arr_pos_le[1];
        } else {
            arr_pos = arr_pos_le[0];
        }
        /* If platform is keystone big enddian and chip is bcm56545,
         * use the arr_pos_be[0]. Because no swaping in CIMIC table DMA.*/
#if defined(BCM_TRIUMPH3_SUPPORT) && defined(KEYSTONE) && defined(BE_HOST)
        if (SOC_IS_TRIUMPH3(unit)) {
            arr_pos = arr_pos_be[0];
        }
#endif
    }

    /* Overwrite the arr_pos according to host endian */
    if (soc_feature(unit, soc_feature_serdes_firmware_pos_by_host_endian)) {
        /* We need to byte swap on big endian host */
        be_host = 1;
        if (*((uint8 *)&be_host) == 1) {
            be_host = 0;
        }
        if (be_host) {
            arr_pos = arr_pos_be[2];
        } else {
            arr_pos = arr_pos_le[2];
        }
    }

#ifdef BCM_TOMAHAWK_SUPPORT
    if (soc_feature(unit, soc_feature_tsce)) {
        /* Intercept and handle mgmt ports which use TSCE */
        if ((SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) &&
            (SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), (port)))) {
            if (soc->soc_wc_ucode_dma_buf2 == NULL) {
                count = datalen / entry_bytes;
                extra_bytes = datalen % entry_bytes;
                soc->soc_wc_ucode_alloc_size2 = datalen;
                if (extra_bytes != 0) {
                    soc->soc_wc_ucode_alloc_size2 += entry_bytes - extra_bytes;
                }
                soc->soc_wc_ucode_dma_buf2 =
                    soc_cm_salloc(unit, soc->soc_wc_ucode_alloc_size2,
                                  "WC ucode DMA buffer");
                if (soc->soc_wc_ucode_dma_buf2 == NULL) {
                    return SOC_E_MEMORY;
                }

                /*
                 * The byte order in each 128-bit *_WC_UCMEM_DATA entry is:
                 * bit position: (bit 31-0)  - (bit 63-32) - (bit 95-64) - (bit 127-96)
                 * byte offset:  0c 0d 0e 0f - 08 09 0a 0b - 04 05 06 07 - 00 01 02 03
                 */
                array_ptr = array;
                dma_buf_ptr = soc->soc_wc_ucode_dma_buf2;
                for (i = 0; i < count; i++) {
                    for (j = 0; j < 16; j++) {
                        dma_buf_ptr[arr_pos[j]] = array_ptr[j];
                    }
                    array_ptr += entry_bytes;
                    dma_buf_ptr += entry_bytes;
                }
                if (extra_bytes != 0) {
                    sal_memset(dma_buf_ptr, 0, entry_bytes);
                    for (j = 0; j < extra_bytes; j++) {
                        dma_buf_ptr[arr_pos[j]] = array_ptr[j];
                    }
                }
            }
            datalen = soc->soc_wc_ucode_alloc_size2;
            dma_buf_ptr = (uint8*)soc->soc_wc_ucode_dma_buf2;
            skip_port = 1; /* Mark port handled */
        }
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
    /* Fall back to legacy handling */
    if (!skip_port) {
        /* Count the required buffer size */
        count = datalen / entry_bytes;
        extra_bytes = datalen % entry_bytes;
        alloc_size = datalen;
        if (extra_bytes != 0) {
            alloc_size += entry_bytes - extra_bytes;
        }

        /* If the buffer size is different, re-allocate the buffer */
        if (alloc_size != soc->soc_wc_ucode_alloc_size) {
            if (soc->soc_wc_ucode_dma_buf != NULL) {
                soc_cm_sfree(unit, soc->soc_wc_ucode_dma_buf);
                soc->soc_wc_ucode_dma_buf = NULL;
            }

            soc->soc_wc_ucode_alloc_size = alloc_size;
        }

        if (soc->soc_wc_ucode_dma_buf == NULL) {
            soc->soc_wc_ucode_dma_buf =
                soc_cm_salloc(unit, soc->soc_wc_ucode_alloc_size,
                              "WC ucode DMA buffer");
            if (soc->soc_wc_ucode_dma_buf == NULL) {
                return SOC_E_MEMORY;
            }

            /*
             * The byte order in each 128-bit *_WC_UCMEM_DATA entry is:
             * bit position: (bit 31-0)  - (bit 63-32) - (bit 95-64) - (bit 127-96)
             * byte offset:  0c 0d 0e 0f - 08 09 0a 0b - 04 05 06 07 - 00 01 02 03
             *
             * For Trident2:
             * bit position: (bit 31-0)  - (bit 63-32) - (bit 95-64) - (bit 127-96)
             * byte offset:  0b 0a 09 08 - 0f 0e 0d 0c - 03 02 01 00 - 07 06 05 04
             */
            array_ptr = array;
            dma_buf_ptr = (uint8*)soc->soc_wc_ucode_dma_buf;
            for (i = 0; i < count; i++) {
                for (j = 0; j < 16; j++) {
                    dma_buf_ptr[arr_pos[j]] = array_ptr[j];
                }
                array_ptr += entry_bytes;
                dma_buf_ptr += entry_bytes;
            }
            if (extra_bytes != 0) {
                sal_memset(dma_buf_ptr, 0, entry_bytes);
                for (j = 0; j < extra_bytes; j++) {
                    dma_buf_ptr[arr_pos[j]] = array_ptr[j];
                }
            }
        }
        datalen = soc->soc_wc_ucode_alloc_size;
        dma_buf_ptr = (uint8*)soc->soc_wc_ucode_dma_buf;
    }

#if defined(BCM_GREYHOUND_SUPPORT)
    if (SOC_IS_GREYHOUND(unit)){
        SOC_IF_ERROR_RETURN(_soc_gh_sbus_tsc_block(unit,
                        phy_port, wc_ucmem_data, &blk));
    } else
#endif /* BCM_GREYHOUND_SUPPORT */
#if defined(BCM_HURRICANE3_SUPPORT)
    if (SOC_IS_HURRICANE3(unit)){
        SOC_IF_ERROR_RETURN
            (soc_hurricane3_sbus_tsc_block(unit, phy_port, &blk));

        if (SOC_REG_BLOCK_IS(unit, wc_ucmem_ctrl, SOC_BLK_PMQ)) {
            port = blk | SOC_REG_ADDR_BLOCK_ID_MASK;
        }
    } else
#endif /* BCM_GREYHOUND_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)){
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            SOC_IF_ERROR_RETURN
                (soc_firelight_sbus_tsc_block(unit, phy_port, &blk));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            SOC_IF_ERROR_RETURN
                (soc_greyhound2_sbus_tsc_block(unit, phy_port, &blk));
        }

        if (SOC_REG_BLOCK_IS(unit, wc_ucmem_ctrl, SOC_BLK_PMQ)) {
            /* Port orverride for PMQ block register access */
            port = blk | SOC_REG_ADDR_BLOCK_ID_MASK;
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        blk = SOC_PORT_BLOCK(unit, phy_port);
    }

    /* enable parallel bus access */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, wc_ucmem_ctrl, port,
                                ACCESS_MODEf, 1));
    if (soc_reg_field_valid(unit, wc_ucmem_ctrl, INST_SELECTf)) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, wc_ucmem_ctrl, port,
                                    INST_SELECTf, wc_instance));
    }
    if (soc_reg_field_valid(unit, wc_ucmem_ctrl, WR_BROADCASTf)) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, wc_ucmem_ctrl, port,
                                    WR_BROADCASTf,
                                    IS_C_PORT(unit, port) ? 1 : 0));
    }

    SOC_IF_ERROR_RETURN
        (soc_mem_write_range(unit, wc_ucmem_data, blk, 0,
                             datalen / entry_bytes - 1, dma_buf_ptr));

    /* disable parallel bus access, and enable MDIO access */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, wc_ucmem_ctrl, port,
                                ACCESS_MODEf, 0));

    return SOC_E_NONE;
}

STATIC uint32
soc_sbus_mdio_addr(uint32 phy_addr, uint32 phy_reg)
{
    int devad;
    uint32 sbus_mdio_addr;

    /* Extract lane and DEVAD from address extension */
    devad = (phy_reg >> 27) & 0x1f;

    /* Build WC/TSC PHY register address */
    sbus_mdio_addr = (phy_reg & 0xffff);
    sbus_mdio_addr |= ((phy_addr & 0x1f) << 19);
    sbus_mdio_addr |= (devad << 27);

    return sbus_mdio_addr;
}

STATIC uint32
soc_sbus_mdio_lane(uint32 phy_reg)
{
    /* Extract lane from address extension */
    return (phy_reg >> 16) & 0x7;
}

int
soc_sbus_mdio_reg_read(int unit, int port, int blk, int wc_instance,
                       uint32 phy_addr, uint32 phy_reg, uint32 *phy_data,
                       soc_mem_t wc_ucmem_data, soc_reg_t wc_ucmem_ctrl)
{
    int rv = SOC_E_NONE;
    int entry_bytes;
    uint32 ucmem_entry[16];

    entry_bytes = soc_mem_entry_bytes(unit, wc_ucmem_data);
    if (entry_bytes > sizeof(ucmem_entry)) {
        return SOC_E_PARAM;
    }

    sal_memset(ucmem_entry, 0, sizeof(ucmem_entry));

    soc_mem_lock(unit, wc_ucmem_data);

    if (soc_reg_field_valid(unit, wc_ucmem_ctrl, INST_SELECTf)) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, wc_ucmem_ctrl, port,
                                    INST_SELECTf, wc_instance));
    }

    /* Write AER register */
    ucmem_entry[0] = soc_sbus_mdio_addr(phy_addr, 0xffde);
    ucmem_entry[1] = (soc_sbus_mdio_lane(phy_reg) << 16);
    ucmem_entry[2] = 1; /* Write register */
    rv = soc_mem_write(unit, wc_ucmem_data, blk, 0, &ucmem_entry);

    /* Setup PHY register address */
    ucmem_entry[0] = soc_sbus_mdio_addr(phy_addr, phy_reg) | ucmem_entry[1];
    ucmem_entry[2] = 0; /* Read register */
    if (SOC_SUCCESS(rv)) {
        rv = soc_mem_write(unit, wc_ucmem_data, blk, 0, &ucmem_entry);
    }

    /* Read PHY register contents */
    if (SOC_SUCCESS(rv)) {
        rv = soc_mem_read(unit, wc_ucmem_data, blk, 0, &ucmem_entry);
    }

    soc_mem_unlock(unit, wc_ucmem_data);

    if (SOC_IS_APACHE(unit)) {
        *phy_data = ucmem_entry[1];
    } else {
        *phy_data = ucmem_entry[0];
    }

    LOG_INFO(BSL_LS_SOC_MII,
             (BSL_META_U(unit,
                         "soc_sbus_mdio_reg_read[%d]: "
                         "addr=0x%x reg=0x%08x data=0x%04x (%d/%d/%d/%d)\n"),
              unit, phy_addr, phy_reg, *phy_data,
              port, blk, wc_instance, rv));

    return rv;
}

int
soc_sbus_mdio_reg_write(int unit, int port, int blk, int wc_instance,
                        uint32 phy_addr, uint32 phy_reg, uint32 phy_data,
                        soc_mem_t wc_ucmem_data, soc_reg_t wc_ucmem_ctrl)
{
    int rv = SOC_E_NONE;
    int entry_bytes;
    uint32 ucmem_entry[16];

    entry_bytes = soc_mem_entry_bytes(unit, wc_ucmem_data);
    if (entry_bytes > sizeof(ucmem_entry)) {
        return SOC_E_PARAM;
    }

    /* If write mask (upper 16 bits) is empty, add full mask */
    if ((phy_data & 0xffff0000) == 0) {
        phy_data |= 0xffff0000;
    }
    sal_memset(ucmem_entry, 0, sizeof(ucmem_entry));

    soc_mem_lock(unit, wc_ucmem_data);

    if (soc_reg_field_valid(unit, wc_ucmem_ctrl, INST_SELECTf)) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, wc_ucmem_ctrl, port,
                                    INST_SELECTf, wc_instance));
    }

    /* Write AER register */
    ucmem_entry[0] = soc_sbus_mdio_addr(phy_addr, 0xffde);
    ucmem_entry[1] = (soc_sbus_mdio_lane(phy_reg) << 16);
    ucmem_entry[2] = 1; /* Write register */
    rv = soc_mem_write(unit, wc_ucmem_data, blk, 0, &ucmem_entry);

    /* Write PHY register */
    if (SOC_SUCCESS(rv)) {
        ucmem_entry[0] = soc_sbus_mdio_addr(phy_addr, phy_reg) | ucmem_entry[1];
        ucmem_entry[1] = (phy_data << 16);
        rv = soc_mem_write(unit, wc_ucmem_data, blk, 0, &ucmem_entry);
    }

    soc_mem_unlock(unit, wc_ucmem_data);

    LOG_INFO(BSL_LS_SOC_MII,
             (BSL_META_U(unit,
                         "soc_sbus_mdio_reg_write[%d]: "
                         "addr=0x%x reg=0x%08x data=0x%04x (%d/%d/%d/%d)\n"),
              unit, phy_addr, phy_reg, phy_data,
              port, blk, wc_instance, rv));

    return rv;
}

int
soc_sbus_tsc_reg_read(int unit, int port, int blk, uint32 phy_addr,
                      uint32 phy_reg, uint32 *phy_data)
{
    int rv = SOC_E_NONE;
    int pport = SOC_INFO(unit).port_l2p_mapping[port];
    int entry_bytes;
    uint32 ucmem_entry[16];
    soc_mem_t mem = XLPORT_WC_UCMEM_DATAm;
#ifdef BCM_TOMAHAWK3_SUPPORT
    int pm_type;
#endif

#if 0
    /* TSC reg read/write is handled by PortMod internally */
    if (SOC_USE_PORTCTRL(unit)) {
        return SOC_E_INTERNAL;
    }
#endif

    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "soc_sbus_tsc_reg_read"
                          "(%d,%d,%d,0x%x,0x%08x,*phy_data)..\n"),
               unit, port, blk, phy_addr, phy_reg));

    if (soc_feature(unit, soc_feature_tscf)) {
        if (SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_CLPORT &&
            SOC_MEM_IS_VALID(unit, CLPORT_WC_UCMEM_DATAm)) {
            mem = CLPORT_WC_UCMEM_DATAm;
        }
    }

    if (SOC_PORT_BLOCK_TYPE(unit, pport) == SOC_BLK_GPORT &&
        SOC_MEM_IS_VALID(unit, GPORT_WC_UCMEM_DATAm)) {
        mem = GPORT_WC_UCMEM_DATAm;
    }

    if (SOC_PORT_BLOCK_TYPE(unit, pport) == SOC_BLK_CDPORT &&
        SOC_MEM_IS_VALID(unit, CDPORT_TSC_UCMEM_DATAm)) {
        mem = CDPORT_TSC_UCMEM_DATAm;
    }

    if (SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_PMQPORT &&
        SOC_MEM_IS_VALID(unit, PMQPORT_WC_UCMEM_DATAm)) {
        mem = PMQPORT_WC_UCMEM_DATAm;
    }

    entry_bytes = soc_mem_entry_bytes(unit, mem);
    if (entry_bytes > sizeof(ucmem_entry)) {
        return SOC_E_PARAM;
    }

    sal_memset(ucmem_entry, 0, sizeof(ucmem_entry));

    soc_mem_lock(unit, mem);

    /* Expect phy_reg here carried full 32 bits TSC address already. */

    /* assigning TSC register address to wc_ucmem_data[31:0] */
    ucmem_entry[0] = phy_reg & 0xffffffff;
    ucmem_entry[2] = 0; /* for TSC register READ */
    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "  ucmem_data_entry[95:64-63:32-31:0]="
                          "0x%08x-0x%08x-0x%08x\n"),
               ucmem_entry[0], ucmem_entry[1], ucmem_entry[2]));
    SOC_ALLOW_WB_WRITE(unit, soc_mem_write(unit, mem, blk, 0, &ucmem_entry), rv);

    /* read data back from wc_ucmem_data[47:32] */
    if (SOC_SUCCESS(rv)) {
        rv = soc_mem_read(unit, mem, blk, 0, &ucmem_entry);
    }

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_PORTCTRL_FUNCTIONS(unit) &&
        SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_type_get) {

        rv = SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_type_get(unit,
                                                pport, &pm_type);

        if (SOC_FAILURE(rv)) {
            return rv;
        }
        if (pm_type == portmodDispatchTypePm8x50) {
            *phy_data = (ucmem_entry[1] >> 16) & 0xffff;
        } else {
    *phy_data = ucmem_entry[1];
        }
    } else
#endif
    {
        *phy_data = ucmem_entry[1];
    }

    soc_mem_unlock(unit, mem);

    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "soc_sbus_tsc_reg_read: *phy_data=0x%04x,rv=%d\n"),
               *phy_data, rv));

    return rv;

}

int
soc_sbus_tsc_reg_write(int unit, int port, int blk, uint32 phy_addr,
                       uint32 phy_reg, uint32 phy_data)
{
    int rv = SOC_E_NONE;
    int pport = SOC_INFO(unit).port_l2p_mapping[port];
    int entry_bytes;
    uint32 ucmem_entry[16];
    uint32 data, data_mask;
    soc_mem_t mem = XLPORT_WC_UCMEM_DATAm;

#if 0
    /* TSC reg read/write is handled by PortMod internally */
    if (SOC_USE_PORTCTRL(unit)) {
        return SOC_E_INTERNAL;
    }
#endif

    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "soc_sbus_tsc_reg_write"
                          "(%d,%d,%d,0x%x,0x%08x,0x%04x)..\n"),
               unit, port, blk, phy_addr, phy_reg, phy_data));

    if (soc_feature(unit, soc_feature_tscf)) {
        if (SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_CLPORT &&
            SOC_MEM_IS_VALID(unit, CLPORT_WC_UCMEM_DATAm)) {
            mem = CLPORT_WC_UCMEM_DATAm;
        }
    }

    if (SOC_PORT_BLOCK_TYPE(unit, pport) == SOC_BLK_GPORT &&
        SOC_MEM_IS_VALID(unit, GPORT_WC_UCMEM_DATAm)) {
        mem = GPORT_WC_UCMEM_DATAm;
    }

    if (SOC_PORT_BLOCK_TYPE(unit, pport) == SOC_BLK_CDPORT &&
        SOC_MEM_IS_VALID(unit, CDPORT_TSC_UCMEM_DATAm)) {
        mem = CDPORT_TSC_UCMEM_DATAm;
    }

    if (SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_PMQPORT &&
        SOC_MEM_IS_VALID(unit, PMQPORT_WC_UCMEM_DATAm)) {
        mem = PMQPORT_WC_UCMEM_DATAm;
    }

    entry_bytes = soc_mem_entry_bytes(unit, mem);
    if (entry_bytes > sizeof(ucmem_entry)) {
        return SOC_E_PARAM;
    }

    /* If write mask (upper 16 bits) is empty, add full mask */
    if ((phy_data & 0xffff0000) == 0) {
        phy_data |= 0xffff0000;
    }
    sal_memset(ucmem_entry, 0, sizeof(ucmem_entry));

    soc_mem_lock(unit, mem);

    /* Expect phy_reg here carried full 32 bits TSC address already. */

    /* assigning TSC register address to wc_ucmem_data[31:0] and write the
     * data/datamask to to wc_ucmem_data[63:32] */
    ucmem_entry[0] = phy_reg & 0xffffffff;
    /* data : wc_ucmem_data[48:63] datamask : wc_ucmem_data[32:47] */
    data = (phy_data & 0xffff) << 16;
    data_mask = (~phy_data & 0xffff0000) >> 16;
    ucmem_entry[1] = data | data_mask;
    ucmem_entry[2] = 1; /* for TSC register write */
    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "  ucmem_data_entry[95:64-63:32-31:0]="
                          "0x%08x-0x%08x-0x%08x\n"),
               ucmem_entry[0], ucmem_entry[1], ucmem_entry[2]));

    rv = soc_mem_write(unit, mem, blk, 0, &ucmem_entry);

    soc_mem_unlock(unit, mem);

    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "soc_sbus_tsc_reg_write : rv=%d\n"),rv));

    return rv;
}


/*
 * Function:
 *      soc_dump
 * Purpose:
 *      Dump useful information from the soc structure.
 * Parameters:
 *      unit - unit number to dump
 *      pfx - character string to prefix output line.
 * Returns:
 *      SOC_E_XXX
 */

int
soc_dump(int unit, const char *pfx)
{
    soc_control_t       *soc;
    soc_persist_t       *sop;
    soc_stat_t          *stat;
    int                 i;
    uint16              dev_id;
    uint8               rev_id;

    if (!SOC_UNIT_VALID(unit)) {
        return(SOC_E_UNIT);
    }

    soc = SOC_CONTROL(unit);
    sop = SOC_PERSIST(unit);

    stat = SOC_STAT(unit);

    LOG_CLI((BSL_META_U(unit,
                        "%sUnit %d Driver Control Structure:\n"), pfx, unit));

    soc_cm_get_id(unit, &dev_id, &rev_id);

    LOG_CLI((BSL_META_U(unit,
                        "%sChip=%s Rev=0x%02x Driver=%s\n"),
             pfx,
             soc_dev_name(unit),
             rev_id,
             SOC_CHIP_NAME(soc->chip_driver->type)));
    LOG_CLI((BSL_META_U(unit,
                        "%sFlags=0x%x:"),
             pfx, soc->soc_flags));
    if (soc->soc_flags & SOC_F_ATTACHED) {
        LOG_CLI((BSL_META_U(unit,
                            " attached")));
    }
    if (soc->soc_flags & SOC_F_INITED) {
        LOG_CLI((BSL_META_U(unit,
                            " initialized")));
    }
    if (soc->soc_flags & SOC_F_LSE) {
        LOG_CLI((BSL_META_U(unit,
                            " link-scan")));
    }
    if (soc->soc_flags & SOC_F_SL_MODE) {
        LOG_CLI((BSL_META_U(unit,
                            " sl-mode")));
    }
    if (soc->soc_flags & SOC_F_POLLED) {
        LOG_CLI((BSL_META_U(unit,
                            " polled")));
    }
    if (soc->soc_flags & SOC_F_URPF_ENABLED) {
        LOG_CLI((BSL_META_U(unit,
                            " urpf")));
    }
    if (soc->soc_flags & SOC_F_MEM_CLEAR_USE_DMA) {
        LOG_CLI((BSL_META_U(unit,
                            " mem-clear-use-dma")));
    }
    if (soc->soc_flags & SOC_F_IPMCREPLSHR) {
        LOG_CLI((BSL_META_U(unit,
                            " ipmc-repl-shared")));
    }
    if (soc->remote_cpu) {
        LOG_CLI((BSL_META_U(unit,
                            " rcpu")));
    }
    LOG_CLI((BSL_META_U(unit,
                        "; board type 0x%x"), soc->board_type));
    LOG_CLI((BSL_META_U(unit,
                        "\n")));
    LOG_CLI((BSL_META_U(unit,
                        "%s"), pfx));
    soc_cm_dump(unit);

    LOG_CLI((BSL_META_U(unit,
                        "%sDisabled: reg_flags=0x%x mem_flags=0x%x\n"),
             pfx,
             soc->disabled_reg_flags, soc->disabled_mem_flags));
    LOG_CLI((BSL_META_U(unit,
                        "%sSchanOps=%d MMUdbg=%d LinkPause=%d\n"),
             pfx,
             stat->schan_op,
             sop->debugMode, soc->soc_link_pause));
    LOG_CLI((BSL_META_U(unit,
                        "%sCounter: int=%dus per=%dus dmaBuf=%p\n"
                        "%s         EvictInvalidPoolIdCount=%u\n"),
             pfx,
             soc->counter_interval,
             SAL_USECS_SUB(soc->counter_coll_cur, soc->counter_coll_prev),
             (void *)soc->counter_buf32,
             pfx, soc->counter_evict_inval_poolid_count));
    LOG_CLI((BSL_META_U(unit,
                        "%sTimeout: Schan=%d(%dus) MIIM=%d(%dus)\n"),
             pfx,
             stat->err_sc_tmo, soc->schanTimeout,
             stat->err_mii_tmo, soc->miimTimeout));
    LOG_CLI((BSL_META_U(unit,
                        "%sIntr: Total=%d Sc=%d ScErr=%d MMU/ARLErr=%d\n"
                        "%s      LinkStat=%d PCIfatal=%d PCIparity=%d\n"
                        "%s      ARLdrop=%d ARLmbuf=%d ARLxfer=%d ARLcnt0=%d\n"
                        "%s      TableDMA=%d TSLAM-DMA=%d"
                        " CCM-DMA=%d SW=%d"
                        "\n%s      MemCmd[BSE]=%d MemCmd[CSE]=%d MemCmd[HSE]=%d\n"
                        "%s      ChipFunc[0]=%d ChipFunc[1]=%d ChipFunc[2]=%d\n"
                        "%s      ChipFunc[3]=%d ChipFunc[4]=%d\n"
                        "%s      FifoDma[0]=%d FifoDma[1]=%d FifoDma[2]=%d FifoDma[3]=%d\n"
                        "%s      I2C=%d MII=%d StatsDMA=%d Desc=%d Chain=%d PciTimeOut=%d\n"),
             pfx, stat->intr, stat->intr_sc, stat->intr_sce, stat->intr_mmu,
             pfx, stat->intr_ls,
             stat->intr_pci_fe, stat->intr_pci_pe,
             pfx, stat->intr_arl_d, stat->intr_arl_m,
             stat->intr_arl_x, stat->intr_arl_0,
             pfx, stat->intr_tdma, stat->intr_tslam,
             stat->intr_ccmdma, stat->intr_sw,
             pfx, stat->intr_mem_cmd[0],
             stat->intr_mem_cmd[1], stat->intr_mem_cmd[2],
             pfx, stat->intr_chip_func[0], stat->intr_chip_func[1],
             stat->intr_chip_func[2],
             pfx, stat->intr_chip_func[3], stat->intr_chip_func[4],
             pfx, stat->intr_fifo_dma[0], stat->intr_fifo_dma[1],
             stat->intr_fifo_dma[2], stat->intr_fifo_dma[3],
             pfx, stat->intr_i2c, stat->intr_mii, stat->intr_stats,
             stat->intr_desc, stat->intr_chain, stat->pci_cmpl_timeout));
    LOG_CLI((BSL_META_U(unit,
                        "%sError: SDRAM=%d CFAP=%d Fcell=%d MmuSR=%d l2FifoDMA=%d\n"),
             pfx,
             stat->err_sdram, stat->err_cfap,
             stat->err_fcell, stat->err_sr,
             stat->err_l2fifo_dma));
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        LOG_CLI((BSL_META_U(unit,
                        "%sM0: msg=%d resp=%d intr=%d\n"),
             pfx,
             stat->m0_msg, stat->m0_resp,
             stat->m0_intr));

        LOG_CLI((BSL_META_U(unit,
                        "%sUc sw programmable total intr=%d\n"),
             pfx,
             stat->uc_sw_prg_intr));

        LOG_CLI((BSL_META_U(unit,
                        "%sM0 sw programmable intr: u0=%d u1=%d u2=%d u3=%d\n"),
             pfx,
             stat->m0_u0_sw_intr, stat->m0_u1_sw_intr,
             stat->m0_u2_sw_intr, stat->m0_u3_sw_intr));

        LOG_CLI((BSL_META_U(unit,
                        "%sSBUS DMA err_intr_res=%u\n"),
             pfx,
             stat->err_sbusdma_intr_res));

    }
#endif
    if (soc_property_get(unit, spn_PARITY_ENABLE, TRUE)) {
        LOG_CLI((BSL_META_U(unit,
                            "%sSER events(mem=%d reg=%d nak=%d stat=%d ecc=%d "
                            "direct=%d fifo=%d tcam=%d)\n"),
                 pfx,
                 stat->ser_err_mem, stat->ser_err_reg,
                 stat->ser_err_nak, stat->ser_err_stat,
                 stat->ser_err_ecc, stat->ser_err_int,
                 stat->ser_err_fifo, stat->ser_err_tcam));
        if (SOC_SER_CORRECTION_SUPPORT(unit)) {
            LOG_CLI((BSL_META_U(unit,
                                "%sSER corrections(fix=%d clear=%d "
                                "restore=%d special=%d err:%d)\n"),
                     pfx, stat->ser_err_corr, stat->ser_err_clear,
                     stat->ser_err_restor, stat->ser_err_spe,
                     stat->ser_err_sw));
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "%sPKT DMA: dcb=t%d tpkt=%u tbyt=%u rpkt=%u rbyt=%u\n"),
             pfx,
             SOC_DCB_TYPE(unit),
             stat->dma_tpkt, stat->dma_tbyt,
             stat->dma_rpkt, stat->dma_rbyt));
    LOG_CLI((BSL_META_U(unit,
                        "%sDV: List: max-q=%d cur-tq=%d cur-rq=%d dv-size=%d\n"),
             pfx,
             soc->soc_dv_cnt, soc->soc_dv_tx_free_cnt,
             soc->soc_dv_rx_free_cnt, soc->soc_dv_size));
    LOG_CLI((BSL_META_U(unit,
                        "%sDV: Statistics: allocs=%d frees=%d alloc-q=%d\n"),
             pfx, stat->dv_alloc, stat->dv_free, stat->dv_alloc_q));
    if (soc_feature(unit, soc_feature_mem_cache) &&
        soc_property_get(unit, spn_MEM_CACHE_ENABLE, TRUE)) {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_HELIX4_SUPPORT) || \
    defined(BCM_KATANA2_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT) || \
    defined(BCM_GREYHOUND_SUPPORT)
        LOG_CLI((BSL_META_U(unit,
                            "%sMem cache (count=%d size=%d vmap size=%d "
                            "errmap size=%d)\n"),
                 pfx,
                 stat->mem_cache_count, stat->mem_cache_size,
                 stat->mem_cache_vmap_size, stat->tcam_corrupt_map_size));
#else
        LOG_CLI((BSL_META_U(unit,
                            "%sMem cache (count=%d size=%d vmap size=%d)\n"), pfx,
                 stat->mem_cache_count, stat->mem_cache_size,
                 stat->mem_cache_vmap_size));
#endif
    }
#if defined(BCM_XGS_SUPPORT)
    if (soc_feature(unit, soc_feature_regs_as_mem)) {
        int count, size;
        soc_ser_reg_cache_info(unit, &count, &size);
        LOG_CLI((BSL_META_U(unit,
                            "%sReg cache (count=%d size=%d)\n"),
                 pfx, count, size));
    }
#endif /* BCM_XGS_SUPPORT */
    for (i = 0; i < soc->soc_max_channels; i++) {
        sdc_t *sdc = &soc->soc_channels[i];
        char *type;
        switch(sdc->sc_type) {
        case DV_NONE:           type = "--";            break;
        case DV_TX:             type = "TX";            break;
        case DV_RX:             type = "RX";            break;
        default:                type = "*INVALID*";     break;
        }
        LOG_CLI((BSL_META_U(unit,
                            "%sdma-ch-%d %s %s Queue=%d (%p)%s%s%s%s\n"),
                 pfx, i, type,
                 sdc->sc_dv_active ? "Active" : "Idle  ",
                 sdc->sc_q_cnt, (void *)sdc->sc_q,
                 (sdc->sc_flags & SOC_DMA_F_DEFAULT) ? " default" : "",
                 (sdc->sc_flags & SOC_DMA_F_POLL) ? " polled" : " intr",
                 (sdc->sc_flags & SOC_DMA_F_MBM) ? " mbm" : " no-mbm",
                 (sdc->sc_flags & SOC_DMA_F_TX_DROP) ? " tx-drop" : ""));
    }

    return(0);
}

#if defined(BROADCOM_DEBUG)
/*
 * Function:
 *      soc_chip_dump
 * Purpose:
 *      Display driver and chip information
 * Notes:
 *      Pass unit -1 to avoid referencing unit number.
 */

void
soc_chip_dump(int unit, soc_driver_t *d)
{
    soc_info_t          *si;
    int                 i, count = 0;
    soc_port_t          port;
    soc_port_t          port1;
    char                pfmt[SOC_PBMP_FMT_LEN];
    uint16              dev_id;
    uint8               rev_id;
    int                 blk, bindex;
    char                instance_string[3], block_name[14];
#if defined(BCM_KATANA2_SUPPORT)
    soc_field_t wc_xfi_mode_sel_fld[]={WC0_8_XFI_MODE_SELf,WC1_8_XFI_MODE_SELf};
    uint32      wc_xfi_mode_sel_val[2]={0};
    uint32      top_misc_control_1_val = 0;
#endif


    if (d == NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "unit %d: no driver attached\n"), unit));
        return;
    }

    LOG_CLI((BSL_META_U(unit,
                        "driver %s (%s)\n"), SOC_CHIP_NAME(d->type), d->chip_string));
    LOG_CLI((BSL_META_U(unit,
                        "\tregsfile\t\t%s\n"), d->origin));
    LOG_CLI((BSL_META_U(unit,
                        "\tpci identifier\t\tvendor 0x%04x device 0x%04x rev 0x%02x\n"),
             d->pci_vendor, d->pci_device, d->pci_revision));
    LOG_CLI((BSL_META_U(unit,
                        "\tclasses of service\t%d\n"), d->num_cos));
    LOG_CLI((BSL_META_U(unit,
                        "\tmaximums\t\tblock %d ports %d mem_bytes %d\n"),
             SOC_MAX_NUM_BLKS, SOC_MAX_NUM_PORTS, SOC_MAX_MEM_BYTES));

    if (unit < 0) {
        return;
    }
    si = &SOC_INFO(unit);
    for (blk = 0; d->block_info[blk].type >= 0; blk++) {
        sal_snprintf(instance_string, sizeof(instance_string), "%d",
                     d->block_info[blk].number);
        if (d->block_info[blk].type == SOC_BLK_PORT_GROUP4 ||
            d->block_info[blk].type == SOC_BLK_PORT_GROUP5) {
            /* coverity[secure_coding] */
            sal_strcpy(instance_string,
                       d->block_info[blk].number ? "_y" : "_x");
        }
        sal_snprintf(block_name, sizeof(block_name), "%s%s",
                     soc_block_name_lookup_ext(d->block_info[blk].type, unit),
                     instance_string);
        LOG_CLI((BSL_META_U(unit,
                            "\tblk %d\t\t%-14s schan %d cmic %d\n"),
                 blk,
                 block_name,
                 d->block_info[blk].schan,
                 d->block_info[blk].cmic));
    }
    bindex = -1;
    for (port = 0; ; port++) {
        for (i = 0; i < d->port_num_blktype; i++) {
            blk = d->port_info[port * d->port_num_blktype + i].blk;
            bindex = d->port_info[port * d->port_num_blktype + i].bindex;
            if(blk < 0) {
                break;
            }
            if (i == 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "\tport %-3d\t"),
                         soc_feature(unit, soc_feature_logical_port_num) ?
                         si->port_p2l_mapping[port] : port));
            } else {
                LOG_CLI((BSL_META_U(unit,
                                    "\t        \t")));
            }
            LOG_CLI((BSL_META_U(unit,
                                "%s\tblk %d %s%d.%d\n"),
                     soc_block_port_name_lookup_ext(d->block_info[blk].type,
                     unit),
                     blk,
                     soc_block_name_lookup_ext(d->block_info[blk].type, unit),
                     d->block_info[blk].number,
                     bindex));
        }
        if (bindex < 0) {            /* end of list */
            break;
        }
    }

    soc_cm_get_id(unit, &dev_id, &rev_id);
    LOG_CLI((BSL_META_U(unit,
                        "unit %d:\n"), unit));
    LOG_CLI((BSL_META_U(unit,
                        "\tpci\t\t\tdevice %04x rev %02x\n"), dev_id, rev_id));
#ifdef BCM_CMICM_SUPPORT
    LOG_CLI((BSL_META_U(unit,
                        "\tcmc used by pci\t\tCMC%d\n"), SOC_PCI_CMC(unit)));
#endif
    LOG_CLI((BSL_META_U(unit,
                        "\tdriver\t\t\ttype %d (%s) group %d (%s)\n"),
             si->driver_type, SOC_CHIP_NAME(si->driver_type),
             si->driver_group, soc_chip_group_names[si->driver_group]));
    LOG_CLI((BSL_META_U(unit,
                        "\tchip\t\t\t%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s"
                        "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n"),
             SOC_IS_HERCULES15(unit) ? "hercules15 " : "",
             SOC_IS_FIREBOLT(unit) ? "firebolt " : "",
             SOC_IS_FIREBOLT2(unit) ? "firebolt2 " : "",
             SOC_IS_TRIUMPH3(unit) ? "triumph3 " : "",
             SOC_IS_HELIX4(unit) ? "helix4 " : "",
             SOC_IS_KATANA(unit) ? "katana " : "",
             SOC_IS_KATANA2(unit) ? "katana2 " : "",
             SOC_IS_HELIX1(unit) ? "helix " : "",
             SOC_IS_FELIX1(unit) ? "felix " : "",
             SOC_IS_HELIX15(unit) ? "helix15 " : "",
             SOC_IS_FELIX15(unit) ? "felix15 " : "",
             SOC_IS_RAPTOR(unit) ? "raptor " : "",
             SOC_IS_TRIUMPH(unit) ? "triumph " : "",
             SOC_IS_RAVEN(unit) ? "raven " : "",
             SOC_IS_HAWKEYE(unit) ? "hawkeye " : "",
             SOC_IS_SCORPION(unit) ? "scorpion " : "",
             SOC_IS_VALKYRIE(unit) ? "valkyrie " : "",
             SOC_IS_CONQUEROR(unit) ? "conqueror " : "",
             SOC_IS_TRIUMPH2(unit) ? "triumph2 " : "",
             SOC_IS_ENDURO(unit) ? "enduro " : "",
             SOC_IS_TRIDENT(unit) ? "trident " : "",
             SOC_IS_TITAN(unit) ? "titan " : "",
             SOC_IS_TRIDENT2(unit) ? "trident2 " : "",
             SOC_IS_TITAN2(unit) ? "titan2 " : "",
             SOC_IS_TRIDENT3(unit) ? "trident3 " : "",
             SOC_IS_HURRICANE4(unit) ? "hurricane4 " : "",
             SOC_IS_HELIX5(unit) ? "helix5 " : "",
             SOC_IS_MAVERICK2(unit) ? "maverick2 " : "",
             SOC_IS_FIREBOLT6(unit) ? "firebolt6 " : "",
             SOC_IS_TOMAHAWK(unit) ? "tomahawk " : "",
             SOC_IS_TOMAHAWK2(unit) ? "tomahawk2 " : "",
             SOC_IS_TITAN2PLUS(unit) ? "titan2plus " : "",
             SOC_IS_TRIDENT2PLUS(unit) ? "trident2plus " : "",
             SOC_IS_XGS(unit) ? "xgs " : "",
             SOC_IS_XGS_FABRIC(unit) ? "xgs_fabric " : "",
             SOC_IS_XGS3_SWITCH(unit) ? "xgs3_switch " : "",
             SOC_IS_XGS_SWITCH(unit) ? "xgs_switch " : ""));

#define SOC_IS_SHOW(unit, a)                                \
            if (a(unit)) {                                  \
                LOG_CLI((BSL_META_U(unit,                   \
                                    "\t%s\n"), #a));        \
            }

    SOC_IS_SHOW(unit, SOC_IS_DRACO1)
    SOC_IS_SHOW(unit, SOC_IS_DRACO15)
    SOC_IS_SHOW(unit, SOC_IS_DRACO)
    SOC_IS_SHOW(unit, SOC_IS_HERCULES1)
    SOC_IS_SHOW(unit, SOC_IS_LYNX)
    SOC_IS_SHOW(unit, SOC_IS_TUCANA)
    SOC_IS_SHOW(unit, SOC_IS_XGS12_SWITCH)
    SOC_IS_SHOW(unit, SOC_IS_HERCULES15)
    SOC_IS_SHOW(unit, SOC_IS_HERCULES15)
    SOC_IS_SHOW(unit, SOC_IS_HERCULES)
    SOC_IS_SHOW(unit, SOC_IS_FIREBOLT)
    SOC_IS_SHOW(unit, SOC_IS_FB)
    SOC_IS_SHOW(unit, SOC_IS_FB_FX_HX)
    SOC_IS_SHOW(unit, SOC_IS_HB_GW)
    SOC_IS_SHOW(unit, SOC_IS_HBX)
    SOC_IS_SHOW(unit, SOC_IS_FBX)
    SOC_IS_SHOW(unit, SOC_IS_HURRICANE2)
    SOC_IS_SHOW(unit, SOC_IS_GREYHOUND)
    SOC_IS_SHOW(unit, SOC_IS_HELIX)
    SOC_IS_SHOW(unit, SOC_IS_HELIX1)
    SOC_IS_SHOW(unit, SOC_IS_HELIX15)
    SOC_IS_SHOW(unit, SOC_IS_FELIX)
    SOC_IS_SHOW(unit, SOC_IS_FELIX1)
    SOC_IS_SHOW(unit, SOC_IS_FELIX15)
    SOC_IS_SHOW(unit, SOC_IS_RAPTOR)
    SOC_IS_SHOW(unit, SOC_IS_RAVEN)
    SOC_IS_SHOW(unit, SOC_IS_HAWKEYE)
    SOC_IS_SHOW(unit, SOC_IS_FX_HX)
    SOC_IS_SHOW(unit, SOC_IS_GOLDWING)
    SOC_IS_SHOW(unit, SOC_IS_HUMV)
    SOC_IS_SHOW(unit, SOC_IS_BRADLEY)
    SOC_IS_SHOW(unit, SOC_IS_FIREBOLT2)
    SOC_IS_SHOW(unit, SOC_IS_TRIUMPH)
    SOC_IS_SHOW(unit, SOC_IS_ENDURO)
    SOC_IS_SHOW(unit, SOC_IS_HURRICANE)
    SOC_IS_SHOW(unit, SOC_IS_HURRICANEX)
    SOC_IS_SHOW(unit, SOC_IS_TR_VL)
    SOC_IS_SHOW(unit, SOC_IS_VALKYRIE)
    SOC_IS_SHOW(unit, SOC_IS_SCORPION)
    SOC_IS_SHOW(unit, SOC_IS_SC_CQ)
    SOC_IS_SHOW(unit, SOC_IS_CONQUEROR)
    SOC_IS_SHOW(unit, SOC_IS_TRIUMPH2)
    SOC_IS_SHOW(unit, SOC_IS_APOLLO)
    SOC_IS_SHOW(unit, SOC_IS_VALKYRIE2)
    SOC_IS_SHOW(unit, SOC_IS_TRIDENT)
    SOC_IS_SHOW(unit, SOC_IS_TITAN)
    SOC_IS_SHOW(unit, SOC_IS_TD_TT)
    SOC_IS_SHOW(unit, SOC_IS_SHADOW)
    SOC_IS_SHOW(unit, SOC_IS_TRIUMPH3)
    SOC_IS_SHOW(unit, SOC_IS_HELIX4)
    SOC_IS_SHOW(unit, SOC_IS_KATANA)
    SOC_IS_SHOW(unit, SOC_IS_KATANAX)
    SOC_IS_SHOW(unit, SOC_IS_KATANA2)
    SOC_IS_SHOW(unit, SOC_IS_TRIDENT2)
    SOC_IS_SHOW(unit, SOC_IS_TITAN2)
    SOC_IS_SHOW(unit, SOC_IS_TD2_TT2)
    SOC_IS_SHOW(unit, SOC_IS_TOMAHAWK)
    SOC_IS_SHOW(unit, SOC_IS_APACHE)
    SOC_IS_SHOW(unit, SOC_IS_TRIDENT3X)
    SOC_IS_SHOW(unit, SOC_IS_HURRICANE4)
    SOC_IS_SHOW(unit, SOC_IS_HELIX5)
    SOC_IS_SHOW(unit, SOC_IS_MAVERICK2)
    SOC_IS_SHOW(unit, SOC_IS_FIREBOLT6)
    SOC_IS_SHOW(unit, SOC_IS_TRX)
    SOC_IS_SHOW(unit, SOC_IS_XGS)
    SOC_IS_SHOW(unit, SOC_IS_XGS_FABRIC)
    SOC_IS_SHOW(unit, SOC_IS_XGS_SWITCH)
    SOC_IS_SHOW(unit, SOC_IS_XGS12_FABRIC)
    SOC_IS_SHOW(unit, SOC_IS_XGS3_SWITCH)
    SOC_IS_SHOW(unit, SOC_IS_XGS3_FABRIC)
    SOC_IS_SHOW(unit, SOC_IS_ARAD)
    SOC_IS_SHOW(unit, SOC_IS_ARADPLUS)
    SOC_IS_SHOW(unit, SOC_IS_ARDON)
    SOC_IS_SHOW(unit, SOC_IS_ARAD_A0)
    SOC_IS_SHOW(unit, SOC_IS_ARAD_B0)
    SOC_IS_SHOW(unit, SOC_IS_ARAD_B1)
    SOC_IS_SHOW(unit, SOC_IS_ARADPLUS_A0)
    SOC_IS_SHOW(unit, SOC_IS_ARAD_B0_AND_ABOVE)
    SOC_IS_SHOW(unit, SOC_IS_ARAD_B1_AND_BELOW)
    SOC_IS_SHOW(unit, SOC_IS_ARADPLUS_AND_BELOW)
    SOC_IS_SHOW(unit, SOC_IS_ACP)
    SOC_IS_SHOW(unit, SOC_IS_QMX)
    SOC_IS_SHOW(unit, SOC_IS_JERICHO)
    SOC_IS_SHOW(unit, SOC_IS_QMX_A0)
    SOC_IS_SHOW(unit, SOC_IS_JERICHO_A0)
    SOC_IS_SHOW(unit, SOC_IS_FE1600_A0)
    SOC_IS_SHOW(unit, SOC_IS_FE1600_B0)
    SOC_IS_SHOW(unit, SOC_IS_FE1600_B0_AND_ABOVE)
    SOC_IS_SHOW(unit, SOC_IS_FE1600)
    SOC_IS_SHOW(unit, SOC_IS_FE3200)
    SOC_IS_SHOW(unit, SOC_IS_FE3200_A0)
    SOC_IS_SHOW(unit, SOC_IS_DFE_TYPE)
    SOC_IS_SHOW(unit, SOC_IS_FE1600_REDUCED)
    SOC_IS_SHOW(unit, SOC_IS_BCM88754_A0)
    SOC_IS_SHOW(unit, SOC_IS_RELOADING)


    LOG_CLI((BSL_META_U(unit,
                        "\tmax modid\t\t%d\n"), si->modid_max));
    LOG_CLI((BSL_META_U(unit,
                        "\tnum ports\t\t%d\n"), si->port_num));
    LOG_CLI((BSL_META_U(unit,
                        "\tnum modids\t\t%d\n"), si->modid_count));
    LOG_CLI((BSL_META_U(unit,
                        "\tnum blocks\t\t%d\n"), si->block_num));
    LOG_CLI((BSL_META_U(unit,
                        "\tnum pipes\t\t%d\n"), si->num_pipe));
    LOG_CLI((BSL_META_U(unit,
                        "\tFE ports\t%d\t%s (%d:%d)\n"),
             si->fe.num, SOC_PBMP_FMT(si->fe.bitmap, pfmt),
             si->fe.min, si->fe.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tGE ports\t%d\t%s (%d:%d)\n"),
             si->ge.num, SOC_PBMP_FMT(si->ge.bitmap, pfmt),
             si->ge.min, si->ge.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tXE ports\t%d\t%s (%d:%d)\n"),
             si->xe.num, SOC_PBMP_FMT(si->xe.bitmap, pfmt),
             si->xe.min, si->xe.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tCE ports\t%d\t%s (%d:%d)\n"),
             si->ce.num, SOC_PBMP_FMT(si->ce.bitmap, pfmt),
             si->ce.min, si->ce.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tHG ports\t%d\t%s (%d:%d)\n"),
             si->hg.num, SOC_PBMP_FMT(si->hg.bitmap, pfmt),
             si->hg.min, si->hg.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tST ports\t%d\t%s (%d:%d)\n"),
             si->st.num, SOC_PBMP_FMT(si->st.bitmap, pfmt),
             si->st.min, si->st.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tETHER ports\t%d\t%s (%d:%d)\n"),
             si->ether.num, SOC_PBMP_FMT(si->ether.bitmap, pfmt),
             si->ether.min, si->ether.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tPORT ports\t%d\t%s (%d:%d)\n"),
             si->port.num, SOC_PBMP_FMT(si->port.bitmap, pfmt),
             si->port.min, si->port.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tALL ports\t%d\t%s (%d:%d)\n"),
             si->all.num, SOC_PBMP_FMT(si->all.bitmap, pfmt),
             si->all.min, si->all.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tIPIC port\t%d\tblock %d\n"),
             si->ipic_port, si->ipic_block));
    LOG_CLI((BSL_META_U(unit,
                        "\tCMIC port\t%d\t%s block %d\n"),
             si->cmic_port,
             SOC_PBMP_FMT(si->cmic_bitmap, pfmt), si->cmic_block));
    LOG_CLI((BSL_META_U(unit,
                        "\tother blocks\t\tARL %d MMU %d MCU %d\n"),
             si->arl_block, si->mmu_block, si->mcu_block));
    LOG_CLI((BSL_META_U(unit,
                        "\t            \t\tIPIPE %d IPIPE_HI %d EPIPE %d EPIPE_HI %d "
                        "BSAFE %d ESM %d OTPC %d\n"),
             si->ipipe_block, si->ipipe_hi_block,
             si->epipe_block, si->epipe_hi_block,
             si->bsafe_block, si->esm_block, si->otpc_block[0]));

    for (i = 0; i < COUNTOF(si->has_block); i++) {
        if (si->has_block[i]) {
            count++;
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\thas blocks\t%d\t"), count));
    for (i = 0; i < COUNTOF(si->has_block); i++) {
        if (si->has_block[i]) {
            LOG_CLI((BSL_META_U(unit,
                                "%s "),
                     soc_block_name_lookup_ext(si->has_block[i], unit)));
            if ((i) && !(i%6)) {
                LOG_CLI((BSL_META_U(unit,
                                    "\n\t\t\t\t")));
            }
        }
    }
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit) && !SOC_IS_SABER2(unit)) {
            READ_TOP_MISC_CONTROL_1r(unit,&top_misc_control_1_val);
            wc_xfi_mode_sel_val[0] = soc_reg_field_get(
                                       unit, TOP_MISC_CONTROL_1r,
                                       top_misc_control_1_val,
                                       wc_xfi_mode_sel_fld[0]);
            if (wc_xfi_mode_sel_val[0]) {
                LOG_CLI((BSL_META_U(unit,
                                    "\n=========================================")));
                LOG_CLI((BSL_META_U(unit,
                                    "\nATTN: KT2 Device's block MXQ8 in XFI MODE")));
                LOG_CLI((BSL_META_U(unit,
                                    "\nPort 25 mapped to  32 and 36 mapped to 34")));
                LOG_CLI((BSL_META_U(unit,
                                    "\n=========================================\n")));
            }
            wc_xfi_mode_sel_val[1] = soc_reg_field_get(
                                       unit, TOP_MISC_CONTROL_1r,
                                       top_misc_control_1_val,
                                       wc_xfi_mode_sel_fld[1]);
            if (wc_xfi_mode_sel_val[1]) {
                LOG_CLI((BSL_META_U(unit,
                                    "\n=========================================")));
                LOG_CLI((BSL_META_U(unit,
                                    "\nATTN: KT2 Device's block MXQ9 in XFI MODE")));
                LOG_CLI((BSL_META_U(unit,
                                    "\nPort 26 mapped to  29 and 39 mapped to 31")));
                LOG_CLI((BSL_META_U(unit,
                                    "\n=========================================\n")));
            }
        }
#endif
    LOG_CLI((BSL_META_U(unit,
                        "\n")));
    LOG_CLI((BSL_META_U(unit,
                        "\tport names\t\t")));
    for (port = 0; port < si->port_num; port++) {
        if (port > 0 && (port % 5) == 0) {
            LOG_CLI((BSL_META_U(unit,
                                "\n\t\t\t\t")));
        }
        port1 = port;
#if defined(BCM_KATANA2_SUPPORT)
            if (SOC_IS_KATANA2(unit)) {
                switch(port) {
                case 25:
                case 36:
                    if (wc_xfi_mode_sel_val[0]) {
                        port1 =  (port == 25) ? 32 : 34;
                    }
                    break;
                case 26:
                case 39:
                    if (wc_xfi_mode_sel_val[1]) {
                        port1 =  (port == 26) ? 29 : 31;
                    }
                    break;
                default:
                    break;
                }
            }
#endif
        LOG_CLI((BSL_META_U(unit,
                            "%d=%s\t"),
                 port1, si->port_name[port]));
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));
    i = 0;
    for (blk = 0; SOC_BLOCK_INFO(unit, blk).type >= 0; blk++) {
        if (SOC_PBMP_IS_NULL(si->block_bitmap[blk])) {
            continue;
        }
        if (++i == 1) {
            LOG_CLI((BSL_META_U(unit,
                                "\tblock bitmap\t")));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "\n\t\t\t")));
        }
        LOG_CLI((BSL_META_U(unit,
                            "%-2d  %-14s %s (%d ports)"),
                 blk,
                 si->block_name[blk],
                 SOC_PBMP_FMT(si->block_bitmap[blk], pfmt),
                 si->block_valid[blk]));
    }
    if (i > 0) {
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }

    {
        soc_feature_t f;

        LOG_CLI((BSL_META_U(unit,
                            "\tfeatures\t")));
        i = 0;
        for (f = 0; f < soc_feature_count; f++) {
            if (soc_feature(unit, f)) {
                if (++i > 3) {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n\t\t\t")));
                    i = 1;
                }
                LOG_CLI((BSL_META_U(unit,
                                    "%s "), soc_feature_name[f]));
            }
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }
}

#endif /* BROADCOM_DEBUG */
