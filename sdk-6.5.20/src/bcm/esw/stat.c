/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom StrataSwitch SNMP Statistics API.
 */

#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/scache_dictionary.h>
#include <soc/counter.h>
#include <soc/ll.h>

#include <bcm/stat.h>
#include <bcm/error.h>

#include <bcm_int/esw/stat.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw/flowcnt.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/triumph2.h>
#ifdef BCM_KATANA2_SUPPORT
#include <bcm_int/esw/katana2.h>
#include <soc/katana2.h>
#endif

#include <bcm_int/esw/portctrl.h>

static unsigned int counter_flags_default;

#define OBJ_TYPE_NUM_ENTRIES   (sizeof(obj2type)/sizeof(obj2type[0]))

#if  defined(BCM_BRADLEY_SUPPORT)  ||  \
     defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
#define  _BCM_HB_GW_TRX_BIT_SET(unit, result, bit) \
        do {                                    \
           if (SOC_IS_HB_GW(unit) || SOC_IS_TRX(unit)) { \
               *result =  bit;                  \
               return BCM_E_NONE;               \
           }                                    \
        } while ((0))
#else
#define  _BCM_HB_GW_TRX_BIT_SET(unit, result, bit) 
#endif /* BRADLEY || TRIUMPH || SCORPION */

#if defined(BCM_RAPTOR_SUPPORT)
#define _BCM_RAPTOR_RAVEN_BIT_SET(unit, result, bit)          \
       do {                                                   \
          if (SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit)) {    \
              *result = (bit);                                \
              return BCM_E_NONE;                              \
          }                                                   \
       } while ((0))
#else
#define _BCM_RAPTOR_RAVEN_BIT_SET(unit, result, bit)
#endif /* RAPTOR */

#if defined(BCM_RAVEN_SUPPORT)
#define _BCM_RAVEN_BIT_SET(unit, result, bit)    \
       do {                                      \
          if (SOC_IS_RAVEN(unit)) {              \
              *result = (bit);                   \
              return BCM_E_NONE;                 \
          }                                      \
       } while ((0))
#else
#define _BCM_RAVEN_BIT_SET(unit, result, bit)
#endif /* RAVEN */

#if  defined(BCM_BRADLEY_SUPPORT)
#define _BCM_HB_GW_BIT_SET_E_PARAM(unit, result) \
       do {                                      \
          if (SOC_IS_HB_GW(unit)) {              \
              return BCM_E_PARAM;                \
          }                                      \
        } while ((0))
#else
#define _BCM_HB_GW_BIT_SET_E_PARAM(unit, result)             
#endif

#if  defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
#define _BCM_TRX_BIT_SET(unit, result, bit)     \
        do {                                    \
           if (SOC_IS_TRX(unit)) {              \
               *result =  bit;                  \
               return BCM_E_NONE;               \
           }                                    \
        } while ((0))

#define _BCM_TRX_BIT_SET_E_PARAM(unit, result)                \
       do {                                                   \
          if (SOC_IS_TRX(unit)) {                             \
              return BCM_E_PARAM;                             \
          }                                                   \
       } while ((0))
#else
#define _BCM_TRX_BIT_SET(unit, result, bit) 
#define _BCM_TRX_BIT_SET_E_PARAM(unit, result)
#endif /* TRIUMPH || SCORPION */  

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_BRADLEY_SUPPORT) || \
    defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
#define  _BCM_FB_HB_GW_TRX_BIT_SET(unit, result, bit) \
        do {                                    \
           if (SOC_IS_HB_GW(unit) || SOC_IS_TRX(unit) || SOC_IS_FB(unit)) { \
               *result =  bit;                  \
               return BCM_E_NONE;               \
           }                                    \
        } while ((0))
#else
#define  _BCM_FB_HB_GW_TRX_BIT_SET(unit, result, bit) 
#endif /* BRADLEY || TRIUMPH || SCORPION || FIREBOLT */

#if defined(BCM_FIREBOLT_SUPPORT)                               
#define _BCM_FB_BIT_SET(unit, result, bit)                      \
       do {                                                     \
          if (SOC_IS_FB(unit)) {                                \
              *result = (bit);                                  \
              return BCM_E_NONE;                                \
          }                                                     \
       } while ((0)) 
#else
#define _BCM_FB_BIT_SET(unit, result, bit)                     
#endif /* FIREBOLT */

#if defined(BCM_FIREBOLT2_SUPPORT)                               
#define _BCM_FB2_BIT_SET(unit, result, bit)                     \
       do {                                                     \
          if (SOC_IS_FIREBOLT2(unit)) {                         \
              *result = (bit);                                  \
              return BCM_E_NONE;                                \
          }                                                     \
       } while ((0)) 
#else
#define _BCM_FB2_BIT_SET(unit, result, bit)                     
#endif /* FIREBOLT2 */

#define _BCM_FBX_BIT_SET(unit, result, bit)                     \
       do {                                                     \
          if (SOC_IS_FBX(unit)) {                               \
              *result = (bit);                                  \
              return BCM_E_NONE;                                \
          }                                                     \
       } while ((0))                                            

#if defined(BCM_TRIDENT2_SUPPORT)                               
#define _BCM_TD2_BIT_SET(unit, result, bit)                     \
       do {                                                     \
          if (SOC_IS_TD2_TT2(unit)) {                           \
              *result = (bit);                                  \
              return BCM_E_NONE;                                \
          }                                                     \
       } while ((0)) 
#else
#define _BCM_TD2_BIT_SET(unit, result, bit)                     
#endif /* TRIDENT2 */

#if defined(BCM_TRIDENT3_SUPPORT)
#define _BCM_TD3_BIT_SET(unit, result, bit)                     \
       do {                                                     \
          if (SOC_IS_TRIDENT3X(unit)) {                          \
              *result = (bit);                                  \
              return BCM_E_NONE;                                \
          }                                                     \
       } while ((0))
#define _BCM_TD3_BIT_SET_E_PARAM(unit, result)                  \
       do {                                                     \
          if (SOC_IS_TRIDENT3X(unit)) {                          \
              return BCM_E_PARAM;                               \
          }                                                     \
       } while ((0))

#else
#define _BCM_TD3_BIT_SET(unit, result, bit)
#define _BCM_TD3_BIT_SET_E_PARAM(unit, result)
#endif /* TRIDENT3 */

#if defined(BCM_TOMAHAWK3_SUPPORT)
#define _BCM_TH3_BIT_SET(unit, result, bit)                     \
       do {                                                     \
          if (SOC_IS_TOMAHAWK3(unit)) {                          \
              *result = (bit);                                  \
              return BCM_E_NONE;                                \
          }                                                     \
       } while ((0))
#define _BCM_TH3_BIT_SET_E_PARAM(unit, result)                  \
       do {                                                     \
          if (SOC_IS_TOMAHAWK3(unit)) {                          \
              return BCM_E_PARAM;                               \
          }                                                     \
       } while ((0))

#else
#define _BCM_TH3_BIT_SET(unit, result, bit)
#define _BCM_TH3_BIT_SET_E_PARAM(unit, result)
#endif /* TOMAHAWK3 */

#if defined(BCM_GREYHOUND_SUPPORT)
#define _BCM_GH_BIT_SET(unit, result, bit)                     \
       do {                                                     \
          if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||  \
              SOC_IS_GREYHOUND2(unit)) {    \
              *result = (bit);                                  \
              return BCM_E_NONE;                                \
          }                                                     \
       } while ((0))
#else
#define _BCM_GH_BIT_SET(unit, result, bit)
#endif /* GREYHOUND */

#if defined(BCM_HURRICANE3_SUPPORT)
#define _BCM_HR3_BIT_SET(unit, result, bit)                     \
       do {                                                     \
          if (SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)) {  \
              *result = (bit);                                  \
              return BCM_E_NONE;                                \
          }                                                     \
       } while ((0))
#else
#define _BCM_HR3_BIT_SET(unit, result, bit)
#endif /* HURRICANE3 */

#define _BCM_DEFAULT_BIT_SET(unit, result, bit)                 \
       do {                                                     \
          *result = (bit);                                      \
          return BCM_E_NONE;                                    \
       } while ((0))

#define _BCM_DEFAULT_BIT_SET_E_PARAM(unit, result)              \
       do {                                                     \
          return BCM_E_PARAM;                                   \
       } while ((0))

#define _DBG_CNT_TX_CHAN        1
#define _DBG_CNT_RX_CHAN        2

#ifdef BCM_XGS3_SWITCH_SUPPORT
/*
 * All drop conditions
 */
#define BCM_DBG_CNT_DROP       (BCM_DBG_CNT_RPORTD | \
                                BCM_DBG_CNT_RIPD4  | \
                                BCM_DBG_CNT_RIPD6  | \
                                BCM_DBG_CNT_PDISC  | \
                                BCM_DBG_CNT_RFILDR | \
                                BCM_DBG_CNT_RDISC  | \
                                BCM_DBG_CNT_RDROP  | \
                                BCM_DBG_CNT_VLANDR)
#endif /* BCM_XGS3_SWITCH_SUPPORT */

/* Oversize packet size threshold (soc_property bcm_stat_jumbo[.unit]=n) */
int **_bcm_stat_ovr_threshold;

/* Port control to include/exclude oversize packet errors */
SHR_BITDCL *_bcm_stat_ovr_control;

STATIC _bcm_stat_extra_counter_t **_bcm_stat_extra_counters;

#define _BCM_STAT_OVR_CONTROL_GET(_u_, _port_) \
        SHR_BITGET(&_bcm_stat_ovr_control[(_u_)], (_port_))
#define _BCM_STAT_OVR_CONTROL_SET(_u_, _port_) \
        SHR_BITSET(&_bcm_stat_ovr_control[(_u_)], (_port_))
#define _BCM_STAT_OVR_CONTROL_CLR(_u_, _port_) \
        SHR_BITCLR(&_bcm_stat_ovr_control[(_u_)], (_port_))

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
STATIC int
_bcm_esw_stat_custom_rsv_add(int unit, bcm_port_t port, bcm_stat_val_t type,
                    bcm_custom_stat_trigger_t trigger);
#endif /* BCM_TRIDENT3_SUPPORT || BCM_TOMAHAWK3_SUPPORT */

void
_bcm_stat_counter_extra_callback(int unit)
{
    soc_reg_t reg;
    uint32 val, diff;
    int rv, i, width;
    _bcm_stat_extra_counter_t *ctr;
    soc_port_t port, port_start, port_end;
    int index;

    if (NULL == _bcm_stat_extra_counters) {
        return;
    }

    for (i = 0; i < _BCM_STAT_EXTRA_COUNTER_COUNT; i++) {
        ctr = &_bcm_stat_extra_counters[unit][i];
        reg = ctr->reg;
        if (reg == INVALIDr) {
            continue;
        }
        if (SOC_REG_INFO(unit, reg).regtype == soc_portreg) {
            port_start = 0;
            port_end = MAX_PORT(unit) - 1;
        } else {
            port_start = port_end = REG_PORT_ANY;
        }
        for (port = port_start; port <= port_end; port++) {
            if (port == REG_PORT_ANY) {
                index = 0;
            } else {
                if (!SOC_PORT_VALID(unit, port)) {
                    continue;
                }
                index = port;
            }
            if (SOC_REG_IS_64(unit, reg)) {
                continue; /* no such case yet */
            } else {
                rv = soc_reg32_get(unit, reg, port, 0, &val);
                if (SOC_FAILURE(rv)) {
                    continue;
                }
                diff = val - ctr->ctr_prev[index];
                /* assume first field is the counter field itself */
                width = SOC_REG_INFO(unit, reg).fields[0].len;
                if (width < 32) {
                    diff &= (1 << width) - 1;
                }
                if (!diff) {
                    continue;
                }
                COMPILER_64_ADD_32(ctr->count64[index], diff);
                ctr->ctr_prev[index] = val;
            }
        }
    }
}

int
_bcm_stat_counter_extra_get(int unit, soc_reg_t reg, soc_port_t port,
                            uint64 *val)
{
    int i;

    if (NULL == _bcm_stat_extra_counters) {
        return BCM_E_INIT;
    }

    for (i = 0; i < _BCM_STAT_EXTRA_COUNTER_COUNT; i++) {
        if(_bcm_stat_extra_counters[unit][i].reg == reg) {
            if (port == REG_PORT_ANY) {
                *val = _bcm_stat_extra_counters[unit][i].count64[0];
            } else {
                *val = _bcm_stat_extra_counters[unit][i].count64[port];
            }
            return BCM_E_NONE;
        }
    }

    COMPILER_64_ZERO(*val);
    return BCM_E_NONE;
}

int
_bcm_stat_counter_non_dma_extra_get(int unit,
                                    soc_counter_non_dma_id_t non_dma_id,
                                    soc_port_t port,
                                    uint64 *val)
{
    COMPILER_64_ZERO(*val);

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && !SOC_IS_HURRICANE(unit)&& 
        !SOC_IS_HURRICANE2(unit) && !SOC_IS_ENDURO(unit)) {
        uint64 val64;
        int ix, rv;
        int max = 1;

        switch (non_dma_id) {
        case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT:
            max = 48;
            break;
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING:
            max = 1;
            break;
        default:
            /* Shouldn't hit this because it's internal */
            return BCM_E_PARAM;
        }

        for (ix = 0; ix < max; ix++) {
            rv = soc_counter_get(unit, port, non_dma_id, ix, &val64);
            if (SOC_SUCCESS(rv)) {
                COMPILER_64_ADD_64(*val, val64);
            } else {
                /* Ran past the maximum index for this port. */
                break;
            }
        }
    }
#endif /* BCM_TRIUMPH_SUPPORT */

    return BCM_E_NONE;
}

/*

 * Function:
 *	_bcm_esw_stat_detach
 * Description:
 *	De-initializes the BCM stat module.
 * Parameters:
 *	unit -  (IN) BCM device number.
 * Returns:
 *	BCM_E_XXX
 */

int
_bcm_esw_stat_detach(int unit)
{
    _bcm_stat_extra_counter_t *ctr;
    int i, free_units;

    if (NULL != _bcm_stat_extra_counters) {
        if (NULL != _bcm_stat_extra_counters[unit]) {
            soc_counter_extra_unregister(unit,
                                         _bcm_stat_counter_extra_callback);

            for (i = 0; i < _BCM_STAT_EXTRA_COUNTER_COUNT; i++) {
                ctr = &_bcm_stat_extra_counters[unit][i];
                if (ctr->count64 != NULL) {
                    sal_free(ctr->count64);
                    ctr->count64 = NULL;
                }
                if (ctr->ctr_prev != NULL) {
                    sal_free(ctr->ctr_prev);
                    ctr->ctr_prev = NULL;
                }
            }
            sal_free(_bcm_stat_extra_counters[unit]);
            _bcm_stat_extra_counters[unit] = NULL;
        }

        free_units = TRUE;
        for (i = 0; i < BCM_MAX_NUM_UNITS; i++) {
            if (((i != unit) && SOC_UNIT_VALID(i)) || 
               (NULL != _bcm_stat_extra_counters[i])) {
                free_units = FALSE;
                break;
            }
        }
        if (free_units) {
            sal_free(_bcm_stat_extra_counters);
            _bcm_stat_extra_counters = NULL;
        }
    }

    if (NULL != _bcm_stat_ovr_threshold) {
        if (NULL != _bcm_stat_ovr_threshold[unit]) {
            sal_free(_bcm_stat_ovr_threshold[unit]);
            _bcm_stat_ovr_threshold[unit] = NULL;
        }

        free_units = TRUE;
        for (i = 0; i < BCM_MAX_NUM_UNITS; i++) {
            if( ((i != unit) && SOC_UNIT_VALID(i)) || 
                (NULL != _bcm_stat_ovr_threshold[i])) {
                free_units = FALSE;
                break;
            }
        }
        if (free_units) {
            sal_free(_bcm_stat_ovr_threshold);
            _bcm_stat_ovr_threshold = NULL;
        }
    }

    /* Delete oversize control */
    if (NULL != _bcm_stat_ovr_control) {
        SHR_BITCLR_RANGE(&_bcm_stat_ovr_control[unit], 0,
                    SHR_BITALLOCSIZE(SOC_MAX_NUM_PORTS));

        free_units = TRUE;
        for (i = 0; i < BCM_MAX_NUM_UNITS; i++) {
            if( ((i != unit) && SOC_UNIT_VALID(i)) || 
                !SHR_BITNULL_RANGE(&_bcm_stat_ovr_control[i], 0,
                      SHR_BITALLOCSIZE(SOC_MAX_NUM_PORTS))) {
                free_units = FALSE;
                break;
            }
        }

        if (free_units) {
            sal_free(_bcm_stat_ovr_control);
            _bcm_stat_ovr_control = NULL;
        }
    }

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_cleanup(unit));
    }
#endif
    if (soc_feature(unit, soc_feature_linkphy_coe)) {
#if defined(BCM_KATANA2_SUPPORT)
        BCM_IF_ERROR_RETURN(bcm_kt2_subport_counter_cleanup(unit));
#endif
    }

#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)){
        BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_cleanup(unit));
    }
#endif    

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_esw_stat_gport_validate
 * Description:
 *      Helper funtion to validate port/gport parameter 
 * Parameters:
 *      unit  - (IN) BCM device number
 *      port_in  - (IN) Port / Gport to validate
 *      port_out - (OUT) Port number if valid. 
 * Return Value:
 *      BCM_E_NONE - Port OK 
 *      BCM_E_INIT - Not initialized
 *      BCM_E_PORT - Port Invalid
 */

STATIC int 
_bcm_esw_stat_gport_validate(int unit, bcm_port_t port_in, bcm_port_t *port_out)
{
    bcm_pbmp_t pbmp_all;

    BCM_PBMP_CLEAR(pbmp_all);
    BCM_PBMP_ASSIGN(pbmp_all, PBMP_ALL(unit));

#ifdef BCM_KATANA2_SUPPORT
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
            _bcm_kt2_subport_pbmp_update(unit, &pbmp_all);
    }
#endif

    if (BCM_GPORT_IS_SET(port_in)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, port_in, port_out));
    } else if (SOC_PORT_VALID(unit, port_in) && 
               ((SOC_PBMP_MEMBER(pbmp_all, port_in) ||
                 SOC_PBMP_MEMBER(PBMP_ALL(unit), port_in)) &&
                (!IS_LB_PORT(unit, port_in) && !IS_RDB_PORT(unit,port_in)))) {
        *port_out = port_in;
    } else {
        return BCM_E_PORT; 
    }

    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_11               SOC_SCACHE_VERSION(1,11)
#define BCM_WB_VERSION_1_10               SOC_SCACHE_VERSION(1,10)
#define BCM_WB_VERSION_1_9                SOC_SCACHE_VERSION(1,9)
#define BCM_WB_VERSION_1_8                SOC_SCACHE_VERSION(1,8)
#define BCM_WB_VERSION_1_7                SOC_SCACHE_VERSION(1,7)
#define BCM_WB_VERSION_1_6                SOC_SCACHE_VERSION(1,6)
#define BCM_WB_VERSION_1_5                SOC_SCACHE_VERSION(1,5)
#define BCM_WB_VERSION_1_4                SOC_SCACHE_VERSION(1,4)
#define BCM_WB_VERSION_1_3                SOC_SCACHE_VERSION(1,3)
#define BCM_WB_VERSION_1_2                SOC_SCACHE_VERSION(1,2)
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_11

static uint16  stat_warmboot_version=0;

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)

STATIC int _bcm_esw_stat_warm_boot_alloc(int unit);

/*
 * Function:
 *      _bcm_esw_stat_warm_boot_recover
 * Purpose:
 *      Recover stat overflow flag states from scache
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_stat_warm_boot_recover(int unit)
{
    uint8 *stat_scache_ptr;
    soc_scache_handle_t scache_handle;
    int stable_size = 0;
    int rv = BCM_E_NONE;
    int alloc_sz = BCM_MAX_NUM_UNITS * SHR_BITALLOCSIZE(SOC_MAX_NUM_PORTS);
    int max_num_ports, scache_sz;

    
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    if (stable_size > 0) {

        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_STAT, 
                              _BCM_STAT_WARM_BOOT_CHUNK_PORTS);
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                     alloc_sz, &stat_scache_ptr, 
                                     BCM_WB_DEFAULT_VERSION, 
                                     &stat_warmboot_version);

        if (BCM_E_NOT_FOUND == rv) {
            return _bcm_esw_stat_warm_boot_alloc(unit);
        } else if (BCM_FAILURE(rv)) {
            return rv;
        }
        if (NULL != stat_scache_ptr) {
            max_num_ports = soc_scache_dictionary_entry_get(unit, 
                                         ssden_SOC_MAX_NUM_PORTS,
                                         SOC_MAX_NUM_PORTS);

            scache_sz = BCM_MAX_NUM_UNITS * SHR_BITALLOCSIZE(max_num_ports);
            SOC_SCACHE_SIZE_MIN_RECOVER(stat_scache_ptr, _bcm_stat_ovr_control,
                                        scache_sz, alloc_sz);
        }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_esw_stat_warm_boot_sync
 * Purpose:
 *      Sync STAT overflow flags states to scache
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_stat_warm_boot_sync(int unit)
{
    uint8 *stat_scache_ptr;
    soc_scache_handle_t scache_handle;
    int stable_size = 0;
    int rv = BCM_E_NONE;
    int alloc_sz = BCM_MAX_NUM_UNITS * SHR_BITALLOCSIZE(SOC_MAX_NUM_PORTS);
    
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    if (stable_size > 0) {

        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_STAT, 
                              _BCM_STAT_WARM_BOOT_CHUNK_PORTS);

        /* Get scache stat module data pointer */
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                     alloc_sz, &stat_scache_ptr, 
                                     BCM_WB_DEFAULT_VERSION, NULL);

        if (BCM_SUCCESS(rv) && (NULL != stat_scache_ptr) && 
            (NULL !=_bcm_stat_ovr_control)) {
            /* Copy stat overflow control flags to scache */
            sal_memcpy(stat_scache_ptr, _bcm_stat_ovr_control, alloc_sz);
        }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_esw_stat_warm_boot_alloc
 * Purpose:
 *      Allocate persistent info memory for STAT module - Level 2 Warm Boot
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_stat_warm_boot_alloc(int unit)
{
    int rv;
    uint8 *stat_scache_ptr;
    soc_scache_handle_t scache_handle;
    int alloc_sz = BCM_MAX_NUM_UNITS * SHR_BITALLOCSIZE(SOC_MAX_NUM_PORTS);
    
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_STAT, 
                          _BCM_STAT_WARM_BOOT_CHUNK_PORTS);

    /* Create scahe pointer */
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                 alloc_sz, &stat_scache_ptr, 
                                 BCM_WB_DEFAULT_VERSION, NULL);

    if (BCM_E_NOT_FOUND == rv) {
        /* Proceed with Level 1 Warm Boot */
        rv = BCM_E_NONE;
    }

    return rv;
}
#endif

int
_bcm_esw_stat_sync_version_above_equal(int unit,uint16 version)
{
   if (stat_warmboot_version >= version) {
       return TRUE;
   }
   return FALSE;
}

int
_bcm_esw_stat_sync_version_below_equal(int unit,uint16 version)
{
   if (stat_warmboot_version <= version) {
       return TRUE;
   } 
   return FALSE;
}

/*
 * Function:
 *      _bcm_esw_stat_sync
 * Purpose:
 *      Record Stat module persisitent info for Level 2 Warm Boot
 * Parameters:
 *      unit  - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_esw_stat_sync(int unit)
{
#if defined(BCM_WARM_BOOT_SUPPORT) && (defined(BCM_TRIUMPH3_SUPPORT) || \
defined(BCM_KATANA_SUPPORT) || defined(BCM_HURRICANE3_SUPPORT))
    _bcm_esw_stat_warm_boot_sync(unit);
#endif
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_sync(unit));
    }
#endif
#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_sync(unit));               
    }
#endif
    return BCM_E_NONE;
}
/*
 * Function:
 *	_bcm_stat_reload (internal)
 * Description:
 *      Reload the STAT structures from the current hardware configuration.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_stat_reload(int unit)
{

    if ((_bcm_stat_ovr_threshold == NULL) || 
        (_bcm_stat_ovr_threshold[unit] == NULL)) {
        /* These were just allocated! */
        return BCM_E_INTERNAL;
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        uint32 value = 0;
        uint64 value64;
        bcm_port_t port;

        PBMP_PORT_ITER(unit, port) {
            if (SOC_PORT_USE_PORTCTRL(unit, port)) {
                BCM_IF_ERROR_RETURN
                    (bcmi_esw_portctrl_cntmaxsize_get(
                        unit, port, (int *)&value));
                _bcm_stat_ovr_threshold[unit][port] = value;
                continue;
            }
#if defined (BCM_TOMAHAWK_SUPPORT) || defined (BCM_APACHE_SUPPORT)
            if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_APACHE(unit) || SOC_IS_TRIDENT3X(unit)) {
                if (IS_CL_PORT(unit, port) || IS_CLG2_PORT(unit, port)) {
                    SOC_IF_ERROR_RETURN
                        (READ_CLPORT_CNTMAXSIZEr(unit, port, &value));
                }
#ifdef BCM_HURRICANE4_SUPPORT
                else if (SOC_IS_HURRICANE4(unit) && IS_EGPHY_PORT(unit,port)) {
                    SOC_IF_ERROR_RETURN
                        (READ_GPORT_CNTMAXSIZEr(unit, port, &value));
                }
#endif
                else {
                    SOC_IF_ERROR_RETURN
                        (READ_XLPORT_CNTMAXSIZEr(unit, port, &value));
                }
                _bcm_stat_ovr_threshold[unit][port] = value;
                continue;
            }
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_APACHE_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_IS_TD2_TT2(unit)) {
                SOC_IF_ERROR_RETURN
                    (READ_PGW_CNTMAXSIZEr(unit, port, &value));
                _bcm_stat_ovr_threshold[unit][port] = value;
                continue;
            }
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT)
            if (SOC_IS_HURRICANE2(unit) || SOC_IS_GREYHOUND(unit) ||
                SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)) {
                if (IS_XL_PORT(unit,port)) {
                    SOC_IF_ERROR_RETURN
                        (READ_XLPORT_CNTMAXSIZEr(unit, port, &value));
#ifdef BCM_GREYHOUND2_SUPPORT
                } else if (IS_CL_PORT(unit, port)) {
                    SOC_IF_ERROR_RETURN
                        (READ_CLPORT_CNTMAXSIZEr(unit, port, &value));
#endif /* BCM_GREYHOUND2_SUPPORT */
                } else {
                    SOC_IF_ERROR_RETURN
                        (READ_GPORT_CNTMAXSIZEr(unit, port, &value));
                }
                _bcm_stat_ovr_threshold[unit][port] = value;
                continue;
            }
#endif /* BCM_HURRICANE2_SUPPORT || BCM_GREYHOUND_SUPPORT */
            if (soc_feature(unit, soc_feature_unified_port)) {
                SOC_IF_ERROR_RETURN
                    (READ_PORT_CNTMAXSIZEr(unit, port, &value));
#if defined(BCM_KATANA2_SUPPORT)
            } else if (SOC_IS_KATANA2(unit) && IS_MXQ_PORT(unit,port)) {
                SOC_IF_ERROR_RETURN
                        (READ_X_GPORT_CNTMAXSIZEr(unit, port, &value));
            } else if (SOC_IS_SABER2(unit) && IS_XL_PORT(unit, port)) {
                SOC_IF_ERROR_RETURN
                        (READ_XLPORT_CNTMAXSIZEr(unit, port, &value));
#endif
            } else if (IS_GE_PORT(unit,port) || IS_FE_PORT(unit,port) ||
                IS_XL_PORT(unit, port) || IS_MXQ_PORT(unit,port)) {
                SOC_IF_ERROR_RETURN
                    (READ_GPORT_CNTMAXSIZEr(unit, port, &value));
            } else {
                SOC_IF_ERROR_RETURN
                    (READ_MAC_CNTMAXSZr(unit, port, &value64));
                COMPILER_64_TO_32_LO(value, value64);
            }

            _bcm_stat_ovr_threshold[unit][port] = value;
        }
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA_SUPPORT) ||\
    defined(BCM_HURRICANE3_SUPPORT)
        /* Recover WB stat overflow flags from scahe */
        BCM_IF_ERROR_RETURN(_bcm_esw_stat_warm_boot_recover(unit));
#endif /* BCM_TRIUMPH3_SUPPORT */
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *	bcm_stat_init
 * Description:
 *	Initializes the BCM stat module.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_NONE - Success.
 *	BCM_E_INTERNAL - Chip access failure.
 */

int
bcm_esw_stat_init(int unit)
{
    pbmp_t		pbmp;
    sal_usecs_t		interval;
    uint32		flags;
    int                 config_threshold;
    int                 i;
    _bcm_stat_extra_counter_t *ctr;
    int alloc_size;
    int free_global_arr = FALSE;
#if defined (BCM_KATANA2_SUPPORT)
    soc_port_t          port; 
#endif


    if (soc_property_get_str(unit, spn_BCM_STAT_PBMP) == NULL) {
#if defined (BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            SOC_PBMP_CLEAR(pbmp); 
            for (port =  SOC_INFO(unit).cmic_port;
                 port <= SOC_INFO(unit).lb_port;
                 port++) {
                 if (SOC_INFO(unit).block_valid[SOC_PORT_BLOCK(unit,port)]) {
                     LOG_VERBOSE(BSL_LS_APPL_SHELL,
                                 (BSL_META_U(unit,
                                             "BlockName:%s \n"),
                                             SOC_BLOCK_NAME(unit, SOC_PORT_BLOCK(unit,port))));
                     SOC_PBMP_PORT_ADD(pbmp, port);
                 }
            }
        } else 
#endif
        {
            SOC_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
            /*
             * Collect stats on CPU port as well if supported
             */
            if (soc_feature(unit, soc_feature_cpuport_stat_dma)) {
                SOC_PBMP_PORT_ADD(pbmp, CMIC_PORT(unit));
            }
        }
    } else {
#if defined (BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            LOG_CLI((BSL_META_U(unit,
                                "After flex-io operation. You might need "
                                "to add new ports for counter collection manually\n")));
        }
#endif
        pbmp = soc_property_get_pbmp(unit, spn_BCM_STAT_PBMP, 0);
    }

    interval = (SAL_BOOT_BCMSIM) ?
               SOC_COUNTER_INTERVAL_SIM_DEFAULT : SOC_COUNTER_INTERVAL_DEFAULT;
    interval = soc_property_get(unit, spn_BCM_STAT_INTERVAL, interval);

    counter_flags_default = SOC_COUNTER_F_DMA;
#if defined(PLISIM)
    if (SAL_BOOT_PLISIM) {
        counter_flags_default = 0;
    }
#endif

    flags =
        soc_property_get(unit, spn_BCM_STAT_FLAGS, counter_flags_default);

    if (SOC_IS_RCPU_ONLY(unit)) {
        flags &= ~SOC_COUNTER_F_DMA;
    }

    if (NULL == _bcm_stat_ovr_threshold) {
        alloc_size = BCM_MAX_NUM_UNITS * sizeof(int *);
        _bcm_stat_ovr_threshold =
            sal_alloc(alloc_size, "device oversize packet thresholds");
        if (NULL == _bcm_stat_ovr_threshold) {
            return (BCM_E_MEMORY);
        }
        sal_memset(_bcm_stat_ovr_threshold, 0, alloc_size);
    }
    alloc_size = SOC_MAX_NUM_PORTS * sizeof(int);
    if (NULL == _bcm_stat_ovr_threshold[unit]) {
        _bcm_stat_ovr_threshold[unit] =
            sal_alloc(alloc_size, "device per-port oversize packet thresholds");
        if (NULL == _bcm_stat_ovr_threshold[unit]) {
            return (BCM_E_MEMORY);
        }
    }
    sal_memset(_bcm_stat_ovr_threshold[unit], 0, alloc_size);
    config_threshold = soc_property_get(unit, spn_BCM_STAT_JUMBO, 1518);
    if ((config_threshold < 1518) || 
        (config_threshold > 0x3fff) ) {
        config_threshold = 1518;
    }

    for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
        _bcm_stat_ovr_threshold[unit][i] = config_threshold;
    }

    /* Initialize Oversize packet control bitmap */
    if (NULL == _bcm_stat_ovr_control) {
        alloc_size = BCM_MAX_NUM_UNITS * SHR_BITALLOCSIZE(SOC_MAX_NUM_PORTS);
        _bcm_stat_ovr_control =
            sal_alloc(alloc_size, "oversize packet error control");
        if (NULL == _bcm_stat_ovr_control) {
            return (BCM_E_MEMORY);
        }
        sal_memset(_bcm_stat_ovr_control, 0, alloc_size);
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_stat_reload(unit));
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        bcm_port_t   port;
        uint64 rval;
        uint32 rval32;
        soc_info_t *si = &SOC_INFO(unit);
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
        if(SOC_IS_TRIDENT3X(unit) || SOC_IS_TOMAHAWK3(unit)) {
            /* Setup for all Rx dropped packets */
            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomReceive0,
                                         bcmDbgCntRPORTD));
            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomReceive0,
                                         bcmDbgCntRIPD4));
            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomReceive0,
                                         bcmDbgCntRIPD6));
            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomReceive0,
                                         bcmDbgCntPDISC));
            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomReceive0,
                                         bcmDbgCntRDROP));
            if (SOC_IS_TRIDENT3X(unit)) {
            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomReceive0,
                                         bcmDbgCntRxVlanMismatch));
            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomReceive0,
                                         bcmDbgCntRxVlanMemberMismatch));

            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomReceive0,
                                         bcmDbgCntRxTpidMismatch));
            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomReceive0,
                                         bcmDbgCntRxPrivateVlanMismatch));
            } else { /* SOC_IS_TOMAHAWK3(unit) */
            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomReceive2,
                                         bcmDbgCntRxVlanMismatch));
            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomReceive2,
                                         bcmDbgCntRxVlanMemberMismatch));
            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomReceive2,
                                         bcmDbgCntRxTpidMismatch));
            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomReceive2,
                                         bcmDbgCntRxPrivateVlanMismatch));
            }

            /* Setup counter for Bridged multicast packets */
            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomReceive1,
                                         bcmDbgCntIMBP));
            /* Setup counter for Multicast (L2+L3) packets that are dropped */
            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomReceive2,
                                         bcmDbgCntRIMDR));
            /* Setup counter for Good IPv6 L3 UC & IPMC packets &  */
            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomTransmit0,
                                         bcmDbgCntTGIPMC6));
            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomTransmit0,
                                         bcmDbgCntTGIP6));
            /* Setup counter for IPv6 L3 UC and IPMC Aged and Drop Packets */
            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomTransmit1,
                                         bcmDbgCntTIPMCD6));
            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomTransmit1,
                                         bcmDbgCntTIPD6));
            /* Setup counter for Good IPv6 IPMC packets */
            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomTransmit2,
                                         bcmDbgCntTGIPMC6));
            /* Setup counter for Dropped packets */
            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomTransmit3,
                                         bcmDbgCntTPKTD));
            /* Setup counter for Good IPv4 & IPv6 L3 UC packets */
            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomTransmit4,
                                         bcmDbgCntTGIP4));
            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomTransmit4,
                                         bcmDbgCntTGIP6));
            /* Setup counter for IPMC drops	*/
            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomTransmit5,
                                         bcmDbgCntTIPMCD4));
            SOC_IF_ERROR_RETURN
                (_bcm_esw_stat_custom_rsv_add(unit, -1, snmpBcmCustomTransmit5,
                                         bcmDbgCntTIPMCD6));
        }
        else
#endif /* BCM_TRIDENT3_SUPPORT || BCM_TOMAHAWK3_SUPPORT */
        {
        SOC_IF_ERROR_RETURN
            (bcm_esw_stat_custom_set(unit, -1, snmpBcmCustomReceive0, 
                                     BCM_DBG_CNT_DROP));
        SOC_IF_ERROR_RETURN
            (bcm_esw_stat_custom_set(unit, -1, snmpBcmCustomReceive1, 
                                     BCM_DBG_CNT_IMBP));
        SOC_IF_ERROR_RETURN
            (bcm_esw_stat_custom_set(unit, -1, snmpBcmCustomReceive2, 
                                     BCM_DBG_CNT_RIMDR));
        SOC_IF_ERROR_RETURN
            (bcm_esw_stat_custom_set(unit, -1, snmpBcmCustomTransmit0, 
                                         BCM_DBG_CNT_TGIPMC6 |
                                         BCM_DBG_CNT_TGIP6));
        SOC_IF_ERROR_RETURN
            (bcm_esw_stat_custom_set(unit, -1, snmpBcmCustomTransmit1, 
                                         BCM_DBG_CNT_TIPMCD6 |
                                         BCM_DBG_CNT_TIPD6));
        SOC_IF_ERROR_RETURN
            (bcm_esw_stat_custom_set(unit, -1, snmpBcmCustomTransmit2, 
                                     BCM_DBG_CNT_TGIPMC6));
        SOC_IF_ERROR_RETURN
            (bcm_esw_stat_custom_set(unit, -1, snmpBcmCustomTransmit3, 
                                     BCM_DBG_CNT_TPKTD));
        SOC_IF_ERROR_RETURN
            (bcm_esw_stat_custom_set(unit, -1, snmpBcmCustomTransmit4, 
                                         BCM_DBG_CNT_TGIP4 |
                                         BCM_DBG_CNT_TGIP6));
        SOC_IF_ERROR_RETURN
            (bcm_esw_stat_custom_set(unit, -1, snmpBcmCustomTransmit5, 
                                         BCM_DBG_CNT_TIPMCD4 |
                                         BCM_DBG_CNT_TIPMCD6));
        }
#ifdef BCM_TRIDENT2_SUPPORT
        if (soc_feature(unit, soc_feature_fcoe)) {
            SOC_IF_ERROR_RETURN(
                bcm_esw_stat_custom_add(unit, 0, snmpBcmCustomReceive3,
                                        bcmDbgCntFcmPortClass3RxFrames));
            SOC_IF_ERROR_RETURN(
                bcm_esw_stat_custom_add(unit, 0, snmpBcmCustomReceive4,
                                        bcmDbgCntFcmPortClass3RxDiscards));
            SOC_IF_ERROR_RETURN(
                bcm_esw_stat_custom_add(unit, 0, snmpBcmCustomReceive5,
                                        bcmDbgCntFcmPortClass2RxFrames));
            SOC_IF_ERROR_RETURN(
                bcm_esw_stat_custom_add(unit, 0, snmpBcmCustomReceive6,
                                        bcmDbgCntFcmPortClass2RxDiscards));
            SOC_IF_ERROR_RETURN(
                bcm_esw_stat_custom_add(unit, 0, snmpBcmCustomTransmit6,
                                        bcmDbgCntFcmPortClass3TxFrames));
            SOC_IF_ERROR_RETURN(
                bcm_esw_stat_custom_add(unit, 0, snmpBcmCustomTransmit7,
                                        bcmDbgCntFcmPortClass2TxFrames));
        }

#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT2PLUS_SUPPORT
        if(soc_feature(unit, soc_feature_egress_failover)) {
            SOC_IF_ERROR_RETURN(
                    bcm_esw_stat_custom_add(unit, 0,snmpBcmCustomTransmit8,
                        bcmDbgCntEgressProtectionDataDrop));
        }
#endif

        PBMP_PORT_ITER(unit, port) {
            if (SOC_PORT_USE_PORTCTRL(unit, port)) {
                BCM_IF_ERROR_RETURN
                    (bcmi_esw_portctrl_cntmaxsize_set
                     (unit, port, _bcm_stat_ovr_threshold[unit][port]));
                continue;
            }
#if defined (BCM_TOMAHAWK_SUPPORT) || defined (BCM_APACHE_SUPPORT)
            if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_APACHE(unit) || SOC_IS_TRIDENT3X(unit)) {
                if (IS_CL_PORT(unit, port) || IS_CLG2_PORT(unit, port)) {
                    SOC_IF_ERROR_RETURN
                        (WRITE_CLPORT_CNTMAXSIZEr
                         (unit, port, _bcm_stat_ovr_threshold[unit][port]));
                }
#ifdef BCM_HURRICANE4_SUPPORT
                else if (SOC_IS_HURRICANE4(unit) && IS_EGPHY_PORT(unit,port)) {
                    SOC_IF_ERROR_RETURN
                        (WRITE_GPORT_CNTMAXSIZEr
                         (unit, port, _bcm_stat_ovr_threshold[unit][port]));
                }
#endif
                else {
                    SOC_IF_ERROR_RETURN
                        (WRITE_XLPORT_CNTMAXSIZEr
                         (unit, port, _bcm_stat_ovr_threshold[unit][port]));
                }
                continue;
            }
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_APACHE_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_IS_TD2_TT2(unit)) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PGW_CNTMAXSIZEr
                     (unit, port, _bcm_stat_ovr_threshold[unit][port]));
                continue;
            }
#endif /* BCM_TRIDENT2_SUPPORT */
#if (defined(BCM_HURRICANE2_SUPPORT) && !defined(_HURRICANE2_DEBUG))
            if (SOC_IS_HURRICANE2(unit)) {
                if (IS_GE_PORT(unit,port)) {
                    SOC_IF_ERROR_RETURN
                        (WRITE_GPORT_CNTMAXSIZEr(unit, port, 
                                                 _bcm_stat_ovr_threshold[unit][port]));
                }
                continue;
            }
#endif
#ifdef BCM_GREYHOUND_SUPPORT
            if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
                SOC_IS_GREYHOUND2(unit)) {
                if (IS_XL_PORT(unit,port)) {
                    SOC_IF_ERROR_RETURN
                        (WRITE_XLPORT_CNTMAXSIZEr(unit, port, 
                                          _bcm_stat_ovr_threshold[unit][port]));
#ifdef BCM_GREYHOUND2_SUPPORT
                } else if (IS_CL_PORT(unit, port)) {
                    SOC_IF_ERROR_RETURN
                        (WRITE_CLPORT_CNTMAXSIZEr(unit, port, 
                                            _bcm_stat_ovr_threshold[unit][port]));
#endif /* BCM_GREYHOUND2_SUPPORT */
                } else {
                    SOC_IF_ERROR_RETURN
                        (WRITE_GPORT_CNTMAXSIZEr(unit, port, 
                                          _bcm_stat_ovr_threshold[unit][port]));
                }
                continue;
            }
#endif

            if (soc_feature(unit, soc_feature_unified_port)) {
                if (!SOC_IS_HURRICANE2(unit) && !SOC_IS_GREYHOUND(unit) &&
                    !SOC_IS_HURRICANE3(unit)) {
                    SOC_IF_ERROR_RETURN
                        (WRITE_PORT_CNTMAXSIZEr(unit, port, 
                                          _bcm_stat_ovr_threshold[unit][port]));
               }
#if defined (BCM_KATANA_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
            } else if ((SOC_IS_KATANAX(unit)) &&
                       IS_MXQ_PORT(unit,port)) {
                SOC_IF_ERROR_RETURN
                        (WRITE_X_GPORT_CNTMAXSIZEr(unit, port,
                                                 _bcm_stat_ovr_threshold[unit][port]));
            } else if (SOC_IS_SABER2(unit) && IS_XL_PORT(unit, port)) {
                        (WRITE_XLPORT_CNTMAXSIZEr(unit, port,
                                        _bcm_stat_ovr_threshold[unit][port]));
#endif
#if defined(BCM_ENDURO_SUPPORT)
            } else if (SOC_IS_ENDURO(unit) && IS_XQ_PORT(unit, port)) {
                SOC_IF_ERROR_RETURN
                    (WRITE_QPORT_CNTMAXSIZEr(unit, port, 
                                             _bcm_stat_ovr_threshold[unit][port]));
#endif /* BCM_ENDURO_SUPPORT */
            } else if (IS_GE_PORT(unit,port) || IS_FE_PORT(unit,port) ||
                IS_XL_PORT(unit, port) || IS_MXQ_PORT(unit,port)) {
                SOC_IF_ERROR_RETURN
                    (WRITE_GPORT_CNTMAXSIZEr(unit, port, 
                                             _bcm_stat_ovr_threshold[unit][port]));
            } else if (IS_IL_PORT(unit, port)) {
                if (!SOC_PBMP_MEMBER(si->il.disabled_bitmap, port)) {
                    SOC_IF_ERROR_RETURN(READ_IL_RX_CONFIGr(unit, port, &rval32));
                    soc_reg_field_set(unit, IL_RX_CONFIGr, &rval32,
                                      IL_RX_MAX_PACKET_SIZEf,
                                      _bcm_stat_ovr_threshold[unit][port]);
                    SOC_IF_ERROR_RETURN(WRITE_IL_RX_CONFIGr(unit, port, rval32));
                }
            } else {
                COMPILER_64_SET(rval, 0, _bcm_stat_ovr_threshold[unit][port]);
                SOC_IF_ERROR_RETURN(WRITE_MAC_CNTMAXSZr(unit, port, rval));
            }

            /*
             * Devices that use 10G MAC + Trimac/Unimac
             */
#if defined(BCM_ENDURO_SUPPORT) || \
    defined(BCM_HURRICANE_SUPPORT)  
            if (IS_XQ_PORT(unit, port) && (SOC_IS_ENDURO(unit) || 
                SOC_IS_HURRICANE(unit) || SOC_IS_HURRICANE2(unit))) {
                SOC_IF_ERROR_RETURN
                    (WRITE_QPORT_CNTMAXSIZEr(unit, port, 
                                             _bcm_stat_ovr_threshold[unit][port]));
            } else
#endif
            if (soc_feature(unit, soc_feature_unified_port) && 
                soc_feature(unit, soc_feature_flexible_xgport)) {
                    SOC_IF_ERROR_RETURN
                        (WRITE_PORT_CNTMAXSIZEr(unit, port, 
                                     _bcm_stat_ovr_threshold[unit][port]));
            } else if (IS_GX_PORT(unit, port) ||(IS_XG_PORT(unit, port) &&
                        soc_feature(unit, soc_feature_flexible_xgport))) {
                SOC_IF_ERROR_RETURN
                    (WRITE_GPORT_CNTMAXSIZEr(unit, port, 
                                 _bcm_stat_ovr_threshold[unit][port]));
            }
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    if ((!SAL_BOOT_SIMULATION) || (SAL_BOOT_BCMSIM)) {
        if ((!SAL_BOOT_BCMSIM) || (NULL != _bcm_stat_extra_counters)) {
            /* BCMSim initializes with counters cleared, so don't clear
             * for the first init on BCMSim. */
            if (!SOC_HW_RESET(unit)) {
                /* Don't clear counters in Turbo boot mode during first Initialization */
                SOC_IF_ERROR_RETURN(soc_counter_set32_by_port(unit, pbmp, 0));
            }
        }
        if ((!SOC_WARM_BOOT(unit)) &&
            (soc_feature(unit, soc_feature_centralized_counter))) {
            /* Clear all sw maintained counters */
            soc_counter_ctr_reset_to_zero(unit);
        }
        SOC_IF_ERROR_RETURN(soc_counter_start(unit, flags, interval, pbmp));
    }

    if (NULL == _bcm_stat_extra_counters) {
        alloc_size = BCM_MAX_NUM_UNITS * sizeof(_bcm_stat_extra_counter_t *);
        _bcm_stat_extra_counters = sal_alloc(alloc_size, "device extra counters");
        if (NULL == _bcm_stat_extra_counters) {
            return (BCM_E_MEMORY);
        }
        sal_memset(_bcm_stat_extra_counters, 0, alloc_size);
        free_global_arr = TRUE;
    }
    alloc_size = _BCM_STAT_EXTRA_COUNTER_COUNT * sizeof(_bcm_stat_extra_counter_t);
    if (NULL == _bcm_stat_extra_counters[unit]) {
        _bcm_stat_extra_counters[unit] = sal_alloc(alloc_size, "device extra counters");
        if (NULL == _bcm_stat_extra_counters[unit]) {
            if (free_global_arr) {
                sal_free(_bcm_stat_extra_counters);
                _bcm_stat_extra_counters = NULL;
            }
            return (BCM_E_MEMORY);
        }
    }
    sal_memset(_bcm_stat_extra_counters[unit], 0, alloc_size);

    ctr = _bcm_stat_extra_counters[unit];
    ctr->reg = INVALIDr;
    if (SOC_REG_IS_VALID(unit, EGRDROPPKTCOUNTr)) {
        if (ctr->count64 == NULL) {
            ctr->count64 = sal_alloc(MAX_PORT(unit) * sizeof(uint64),
                                     "bcm extra counters");
            if (ctr->count64 == NULL) {
                sal_free(_bcm_stat_extra_counters[unit]);
                _bcm_stat_extra_counters[unit] = NULL;
                if (free_global_arr) {
                    sal_free(_bcm_stat_extra_counters);
                    _bcm_stat_extra_counters = NULL;
                }
                return SOC_E_MEMORY;
            }
            sal_memset(ctr->count64, 0, MAX_PORT(unit) * sizeof(uint64)); 
        }
        if (ctr->ctr_prev == NULL) {
            ctr->ctr_prev = sal_alloc(MAX_PORT(unit) * sizeof(uint32),
                                      "bcm extra counters");
            if (ctr->ctr_prev == NULL) {
                sal_free(ctr->count64);
                ctr->count64 = NULL;
                sal_free(_bcm_stat_extra_counters[unit]);
                _bcm_stat_extra_counters[unit] = NULL;
                if (free_global_arr) {
                    sal_free(_bcm_stat_extra_counters);
                    _bcm_stat_extra_counters = NULL;
                }
                return SOC_E_MEMORY;
            }
            sal_memset(ctr->ctr_prev, 0, MAX_PORT(unit) * sizeof(uint32)); 
        }
        ctr->reg = EGRDROPPKTCOUNTr;
    }
    BCM_IF_ERROR_RETURN(soc_counter_extra_register(unit,
                         _bcm_stat_counter_extra_callback));

#if defined(BCM_WARM_BOOT_SUPPORT) && (defined(BCM_TRIUMPH3_SUPPORT) || \
defined(BCM_KATANA_SUPPORT) || defined(BCM_HURRICANE3_SUPPORT)) 
   if (!SOC_WARM_BOOT(unit)) {
        /* For cold boot, allocate WB scahe STAT module data */
        BCM_IF_ERROR_RETURN(_bcm_esw_stat_warm_boot_alloc(unit));
   }
#endif /* BCM_WARM_BOOT_SUPPORT */

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
if (!SOC_IS_MONTEREY(unit)) {
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_init(unit));
    }
}
#endif
    if (soc_feature(unit, soc_feature_linkphy_coe)) {
#if defined (BCM_KATANA2_SUPPORT)
        BCM_IF_ERROR_RETURN(bcm_kt2_subport_counter_init(unit));
#endif
    }
#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)){
        BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_init(unit));
    }
#endif    
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        bcmi_flex_ctr_portmap_info_init(unit);
    }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

    return BCM_E_NONE;
}
/*
 * Function:
 *	bcm_esw_stat_flex_pool_info_multi_get
 * Description:
 *	Retrieves the flex counter details for a given objectId
 * Parameters:
 *	
 *  unit             : (IN)  UNIT number      
 *  direction        : (IN)  Direction (Ingress/egress) 
 *  num_pools        : (IN)  Passing a 0, then actual_num_pools will return the number of pools that are available. 
 *                         return the number of pools that are available. 
 *                         Passing n will return up to n pools in the return array
 *  flex_pool_stat   : (OUT)  array that provides the pool info
 *  actual_num_pools : (OUT)  Returns actual no of pools available for a given direction
 *
 * Returns:
 *	BCM_E_*
 */

int bcm_esw_stat_flex_pool_info_multi_get(
    int unit,
    bcm_stat_flex_direction_t direction,
    uint32 num_pools,
    bcm_stat_flex_pool_stat_info_t *flex_pool_stat,
    uint32 *actual_num_pools)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return _bcm_esw_stat_flex_pool_info_multi_get(unit,
               direction, num_pools, flex_pool_stat, actual_num_pools);
    } else 
#endif
    { 
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *  _bcm_esw_stat_flex_counter_get
 * Description:
 *  Get the specified statistic of given counter
 *   if sync_mode is set, sync the sw accumulated count
 *   with hw count value first, else return sw count.
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode   - (IN) hwcount is to be synced to sw count
 *      Stat_id          - (IN) Statistics entity ID.
 *      counter_type   - (IN) Counter stat types, I.e. byte/packet
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 * Returns:
 *  BCM_E_*
 */
 int _bcm_esw_stat_flex_counter_get(
     int unit,
     int sync_mode,
     uint32 stat_counter_id,
     bcm_stat_flex_stat_t stat,
     uint32 num_entries,
     uint32 *counter_indexes,
     bcm_stat_value_t *counter_values)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    uint32  index_count=0;
    uint32  byte_flag=0;
#endif

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_advanced_flex_counter)) {
        if (bcmStatFlexStatPackets == stat) {
            byte_flag=0;
        } else if (bcmStatFlexStatBytes == stat) {
            byte_flag=1;
        } else {
            return BCM_E_PARAM;
        }

        for (index_count=0; index_count < num_entries ; index_count++) {
            BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_raw_get(
                unit, sync_mode, stat_counter_id, byte_flag,
                counter_indexes[index_count],
                &counter_values[index_count]));
        }
        return BCM_E_NONE;
    } else
#endif
#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)) {
        if (bcmStatFlexStatPackets == stat) {
            byte_flag=0;
        } else if (bcmStatFlexStatBytes == stat) {
            byte_flag=1;
        } else {
            return BCM_E_PARAM;
        }
        for (index_count=0; index_count < num_entries ; index_count++) {
            BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_counter_raw_get(
                unit, sync_mode, stat_counter_id, byte_flag,
                counter_indexes[index_count],
                &counter_values[index_count]));
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
 *	bcm_esw_stat_flex_counter_get
 * Description:
 *	Get the specified statistic of given counter
 * Parameters:
 *      unit             - (IN) unit number
 *      Stat_id          - (IN) Statistics entity ID.
 *      counter_type   - (IN) Counter stat types, I.e. byte/packet
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 * Returns:
 *	BCM_E_*
 */
 int bcm_esw_stat_flex_counter_get(
     int unit,
     uint32 stat_counter_id,
     bcm_stat_flex_stat_t stat,
     uint32 num_entries,
     uint32 *counter_indexes,
     bcm_stat_value_t *counter_values)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_advanced_flex_counter) ||
        soc_feature(unit, soc_feature_flowcnt)) {
        return _bcm_esw_stat_flex_counter_get(unit, 0, stat_counter_id, 
            stat, num_entries, counter_indexes, counter_values);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *  bcm_esw_stat_flex_counter_sync_get
 * Description:
 *  Get the specified statistic of given counter
 *   sw accumulated counters synced with hw count.
 * Parameters:
 *      unit             - (IN) unit number
 *      Stat_id          - (IN) Statistics entity ID.
 *      counter_type   - (IN) Counter stat types, I.e. byte/packet
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 * Returns:
 *	BCM_E_*
 */
 int bcm_esw_stat_flex_counter_sync_get(
     int unit,
     uint32 stat_counter_id,
     bcm_stat_flex_stat_t stat,
     uint32 num_entries,
     uint32 *counter_indexes,
     bcm_stat_value_t *counter_values)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_advanced_flex_counter) ||
        soc_feature(unit, soc_feature_flowcnt)) {
        return _bcm_esw_stat_flex_counter_get(unit, 1, stat_counter_id,
            stat, num_entries, counter_indexes, counter_values);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *  _bcm_esw_stat_flex_counter_set
 * Description:
 *  Set the specified value to given counter
 * Parameters:
 *      unit             - (IN) unit number
 *      Stat_id          - (IN) Statistics entity ID.
 *      counter_type   - (IN) Counter stat types, I.e. byte/packet
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 * Returns:
 *	BCM_E_*
 */
 int _bcm_esw_stat_flex_counter_set(
     int unit, 
     uint32 stat_counter_id, 
     bcm_stat_flex_stat_t stat, 
     uint32 num_entries, 
     uint32 *counter_indexes, 
     bcm_stat_value_t *counter_values)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    uint32  index_count=0;
    uint32  byte_flag=0;
#endif

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_advanced_flex_counter)) {
        if (bcmStatFlexStatPackets == stat) {
            byte_flag=0;
        } else  if (bcmStatFlexStatBytes == stat) {
            byte_flag=1;
        } else {
            return BCM_E_PARAM;
        }
        /* Check attached or not? */
        for (index_count=0; index_count < num_entries ; index_count++) {
             BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_raw_set(
                                 unit, stat_counter_id, byte_flag,
                                 counter_indexes[index_count],
                                 &counter_values[index_count]));
        } 
        return BCM_E_NONE;
    } else
#endif
#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)) {
        if (bcmStatFlexStatPackets == stat) {
            byte_flag = 0;
        } else  if (bcmStatFlexStatBytes == stat) {
            byte_flag = 1;
        } else {
            return BCM_E_PARAM;
        }
        /* Check attached or not? */
        for (index_count=0; index_count < num_entries ; index_count++) {
             BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_counter_raw_set(
                                 unit, stat_counter_id, byte_flag,
                                 counter_indexes[index_count],
                                 &counter_values[index_count]));
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
 *      bcm_esw_stat_flex_counter_set
 * Description:
 *   Set counter value of given counter
 * Parameters:
 *      unit             - (IN) unit number
 *      Stat_id          - (IN) Statistics entity ID.
 *      counter_type   - (IN) Counter stat types, I.e. byte/packet
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 * Returns:
 *      BCM_E_*
 */
 int bcm_esw_stat_flex_counter_set(
     int unit,
     uint32 stat_counter_id,
     bcm_stat_flex_stat_t stat,
     uint32 num_entries,
     uint32 *counter_indexes,
     bcm_stat_value_t *counter_values)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_advanced_flex_counter) ||
        soc_feature(unit, soc_feature_flowcnt)) {
        return _bcm_esw_stat_flex_counter_set(unit, stat_counter_id,
            stat, num_entries, counter_indexes, counter_values);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_stat_group_mode_attr_class_id_create
 * Description:
 *      Create Customized Stat class identifiers for combining
 *      Counter Attributes.
 *
 * Parameters:
 *   unit                (IN) Unit number.
 *   attr                (IN) Stat Attribute Flex Group Mode
 *   num_of_attr_values  (IN) Number of combined Attributes
 *   attr_values         (IN) Attribute Combined types
 *   class_id            (OUT) Created Class Identifier for the Combined type.
 *
 * Return Value:
 *   BCM_E_XXX
 */

int bcm_esw_stat_group_mode_attr_class_id_create(
    int unit,
    bcm_stat_group_mode_attr_t attr,
    uint32 num_of_attr_values,
    uint32 *attr_values,
    bcm_stat_group_mode_attr_class_id_t * attr_class_id)
{
#ifdef BCM_KATANA2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_stat_attributes_class)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_stat_group_mode_attr_class_id_create(
                    unit, attr, num_of_attr_values, attr_values, attr_class_id));
        return BCM_E_NONE;
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_stat_group_mode_attr_class_id_destroy_
 * Description:
 *      Destroy Customized Stat class identifiers created for combining
 *      Counter Attributes.
 *
 * Parameters:
 *   unit                (IN) Unit number.
 *   class_id            (IN) Created Class Identifier for the Combined type.
 *
 * Return Value:
 *   BCM_E_XXX
 */

int bcm_esw_stat_group_mode_attr_class_id_destroy(
    int unit,
    bcm_stat_group_mode_attr_class_id_t attr_class_id)
{
#ifdef BCM_KATANA2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_stat_attributes_class)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_stat_group_mode_attr_class_id_destroy(
                    unit, attr_class_id));
        return BCM_E_NONE;
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_stat_port_group_create
 * Description:
 *      Create customized group identifiers for combining ports.
 *
 * Parameters:
 *   unit                (IN) Unit number.
 *   num_ports           (IN) Numbder of ports.
 *   gport               (IN) List of gports
 *   group_id           (OUT) Created group Identifier for the combined ports.
 *
 * Return Value:
 *   BCM_E_XXX
 */
int bcm_esw_stat_port_group_create(int unit,
                                   uint32 num_gports,
                                   bcm_gport_t *gport,
                                   uint32 *group_id)
{
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_stat_port_group_support)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_stat_port_group_create(unit,
                    num_gports, gport, group_id));
        return BCM_E_NONE;
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_stat_port_group_destroy
 * Description:
 *      Destroy customized group identifiers for combining ports.
 *
 * Parameters:
 *   unit                (IN) Unit number.
 *   group_id            (IN) Created group Identifier for the combined ports.
 *
 * Return Value:
 *   BCM_E_XXX
 */
int bcm_esw_stat_port_group_destroy(int unit,
        uint32 group_id)
{
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_stat_port_group_support)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_stat_port_group_destroy(unit,
                    group_id));
        return BCM_E_NONE;
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *	bcm_esw_stat_group_mode_id_create
 * Description:
 *	Create Customized Stat Group mode for given Counter Attributes
 * Parameters:
 *	
 *  unit           : (IN)  UNIT number      
 *  flags          : (IN)  STAT_GROUP_MODE_* flags (INGRESS/EGRESS) 
 *  total_counters : (IN)  Total Counters for Stat Flex Group Mode
 *  num_selectors  : (IN)  Number of Selectors for Stat Flex Group Mode
 *  attr_selectors : (IN)  Attribute Selectors for Stat Flex Group Mode
 *  mode_id        : (OUT) Created Mode Identifier for Stat Flex Group Mode
 *
 * Returns:
 *	BCM_E_*
 */
int bcm_esw_stat_group_mode_id_create(
    int unit,
    uint32 flags,
    uint32 total_counters,
    uint32 num_selectors,
    bcm_stat_group_mode_attr_selector_t *attr_selectors,
    uint32 *mode_id)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return _bcm_esw_stat_group_mode_id_create(unit,
               flags,total_counters,num_selectors,attr_selectors,mode_id);
    } else 
#endif
#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)) {
        return _bcm_esw_flowcnt_group_mode_id_create(unit,
               flags,total_counters,num_selectors,attr_selectors,mode_id);
    } else
#endif
    { 
        return BCM_E_UNAVAIL;
    }
}
/*
 * Function:
 *	bcm_esw_stat_group_mode_id_get
 * Description:
 *	Retrieves Customized Stat Group mode Attributes for given mode_id
 * Parameters:
 *	
 *  unit                : (IN)  UNIT number      
 *  mode_id             : (IN) Created Mode Identifier for Stat Flex Group Mode
 *  flags               : (OUT)  STAT_GROUP_MODE_* flags (INGRESS/EGRESS) 
 *  total_counters      : (OUT)  Total Counters for Stat Flex Group Mode
 *  num_selectors       : (IN)  Number of Selectors for Stat Flex Group Mode
 *  attr_selectors      : (OUT)  Attribute Selectors for Stat Flex Group Mode
 *  actual_num_selectors: (OUT) Actual Number of Selectors for Stat Flex 
 *                              Group Mode
 *
 * Returns:
 *	BCM_E_*
 */
int bcm_esw_stat_group_mode_id_get(
    int unit,
    uint32 mode_id,
    uint32 *flags,
    uint32 *total_counters,
    uint32 num_selectors,
    bcm_stat_group_mode_attr_selector_t *attr_selectors,
    uint32 *actual_num_selectors)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return _bcm_esw_stat_group_mode_id_get(unit,
               mode_id, flags, total_counters, 
               num_selectors, attr_selectors, actual_num_selectors);
    } else
#endif
#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)) {
        return _bcm_esw_flowcnt_group_mode_id_get(unit,
               mode_id, flags, total_counters, 
               num_selectors, attr_selectors, actual_num_selectors);
    } else
#endif
    { 
        return BCM_E_UNAVAIL;
    }
}
/*
 * Function:
 *	bcm_esw_stat_group_mode_id_destroy
 * Description:
 *	Destroys Customized Group mode
 * Parameters:
 *	
 *  unit                : (IN)  UNIT number      
 *  mode_id             : (IN) Created Mode Identifier for Stat Flex Group Mode
 *
 * Returns:
 *	BCM_E_*
 */
int bcm_esw_stat_group_mode_id_destroy(
    int unit,
    uint32 mode_id)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return _bcm_esw_stat_group_mode_id_destroy(unit, mode_id);
    } else
#endif
#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)) {
        return _bcm_esw_flowcnt_group_mode_id_destroy(unit, mode_id);
    } else
#endif
    { 
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *	bcm_esw_stat_custom_group_create
 * Description:
 *	Associate an accounting object to customized group mode 
 *
 * Parameters:
 *	
 *  unit                : (IN)  UNIT number      
 *  mode_id             : (IN) Created Mode Identifier for Stat Flex Group Mode
 *  object              : (IN) Accounting object
 *  Stat_counter_id     : (OUT) Stat Counter Id
 *  num_entries         : (OUT) Number of Counter entries created
 *
 * Returns:
 *	BCM_E_*
 */

int bcm_esw_stat_custom_group_create(
    int unit,
    uint32 mode_id,
    bcm_stat_object_t object,
    uint32 *stat_counter_id,
    uint32 *num_entries)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        if(soc_feature(unit, soc_feature_flex_stat_compaction_support)) {
            return BCM_E_UNAVAIL;
        } else {
            return _bcm_esw_stat_custom_group_create(unit,
                    mode_id, object, stat_counter_id, num_entries);
        }
    } else
#endif
#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)) {
        return _bcm_esw_flowcnt_custom_group_create(unit,
               mode_id, object, stat_counter_id, num_entries);
    } else
#endif
    { 
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_stat_group_mode_config_create
 * Description:
 *      Create Customized Stat Group mode for given Counter Attributes
 *    
 * Parameters:
 *    Unit            (IN)  Unit number
 *    bcm_stat_group_mode_id_config_t (IN) Stat Config 
 *   mode_id             : (IN) Created Mode Identifier for Stat Flex Group Mode
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

int bcm_esw_stat_group_mode_id_config_create(
    int unit,
    uint32 options,
    bcm_stat_group_mode_id_config_t *stat_config,
    uint32 num_selectors,
    bcm_stat_group_mode_attr_selector_t *attr_sel,
    uint32 *mode_id)
{
    if ((NULL == stat_config) || (NULL == attr_sel)) {
        return BCM_E_PARAM;
    }
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return _bcm_esw_stat_group_mode_id_config_create(unit, options, 
                        stat_config, num_selectors, attr_sel, mode_id);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_stat_group_get
 * Description:
 *
 * Parameters:
 *    Unit            (IN)  Unit number
 *    object          (IN)  Accounting Object
 *    Group_mode      (IN)  Group Mode
 *    Stat_counter_id (OUT) Stat Counter Id
 *    num_entries     (OUT) Number of Counter entries created
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_stat_group_mode_id_config_get (
            int                   unit,
            uint32                mode_id,
            bcm_stat_group_mode_id_config_t *stat_config,
            uint32 num_selectors,
            bcm_stat_group_mode_attr_selector_t *attr_sel,
            uint32 *actual_num_selectors)
{
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
       return _bcm_esw_stat_group_mode_id_config_get(unit, mode_id, stat_config,
                                 num_selectors, attr_sel, actual_num_selectors);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_stat_group_create
 * Description:
 *      Reserve HW counter resources as per given group mode and acounting
 *      object and make system ready for further stat collection action
 *     
 * Parameters:
 *    Unit            (IN)  Unit number
 *    object          (IN)  Accounting Object
 *    Group_mode      (IN)  Group Mode
 *    Stat_counter_id (OUT) Stat Counter Id
 *    num_entries     (OUT) Number of Counter entries created
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_stat_group_create (
            int                   unit,
            bcm_stat_object_t     object,
            bcm_stat_group_mode_t group_mode,
            uint32                *stat_counter_id,
            uint32                *num_entries)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        
#if defined(BCM_TOMAHAWK_SUPPORT)
        if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
            if (bcmStatObjectIngFieldStageIngress == object) {
                /* IFP flex stats for TH only supported
                 * via field stat APIs
                 */
                return BCM_E_UNAVAIL;
            }
#if defined(BCM_TOMAHAWK3_SUPPORT)
            if (SOC_IS_TOMAHAWK3(unit)) {
                switch (object) {
                    case bcmStatObjectIngVlanXlate:
                    case bcmStatObjectEgrVlanXlate:
                        return BCM_E_UNAVAIL;
                    default:
                        break;
                }
            }
#endif /* BCM_TOMAHAWK3_SUPPORT */
        }
#endif
        if(soc_feature(unit, soc_feature_flex_stat_compaction_support)) {
            return BCM_E_UNAVAIL;
        } else {
            return _bcm_esw_stat_group_create(
                    unit, 
                    object, 
                    group_mode,
                    stat_counter_id,
                    num_entries);
        }
    } else
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit,soc_feature_gport_service_counters)) {
        int i;
        int fs_index;
        struct obj2type_s {
            bcm_stat_object_t obj;
            _bcm_flex_stat_type_t fs_type;
        } obj2type[] = {
            {bcmStatObjectIngPort,     _bcmFlexStatTypeGport},
            {bcmStatObjectIngVlan,     _bcmFlexStatTypeService},
            {bcmStatObjectIngVlanXlate,_bcmFlexStatTypeVxlt},
            {bcmStatObjectIngVfi,      _bcmFlexStatTypeService},
            {bcmStatObjectIngL3Intf,   _bcmFlexStatTypeGport},
            {bcmStatObjectIngVrf,      _bcmFlexStatTypeVrf},
            {bcmStatObjectIngPolicy,   _bcmFlexStatTypeFp},   
            {bcmStatObjectIngMplsVcLabel, _bcmFlexStatTypeGport},
            {bcmStatObjectIngMplsSwitchLabel,_bcmFlexStatTypeMplsLabel},
            {bcmStatObjectEgrPort,     _bcmFlexStatTypeEgressGport},
            {bcmStatObjectEgrVlan,     _bcmFlexStatTypeEgressService},
            {bcmStatObjectEgrVlanXlate,_bcmFlexStatTypeEgrVxlt},
            {bcmStatObjectEgrVfi,      _bcmFlexStatTypeEgressService},
            {bcmStatObjectEgrL3Intf,   _bcmFlexStatTypeEgressGport}
        };

        if (group_mode != bcmStatGroupModeSingle) {
            return BCM_E_PARAM;
        }
        for (i = 0; i < OBJ_TYPE_NUM_ENTRIES; i++) {
            if (obj2type[i].obj == object) {
                break;
            }
        }
        if (i == OBJ_TYPE_NUM_ENTRIES) {
            return BCM_E_PARAM;
        }

        fs_index = _bcm_esw_flex_stat_free_index_assign(unit,
                                         obj2type[i].fs_type);
        if (!fs_index) {
            return BCM_E_PARAM;  
        }
        *stat_counter_id = _BCM_FLEX_STAT_COUNT_ID(obj2type[i].fs_type,
                                      fs_index);

        *num_entries = 1;
        return BCM_E_NONE;
    } else
#endif
#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)) {
        return _bcm_esw_flowcnt_group_create(
                    unit, 
                    object, 
                    group_mode,
                    stat_counter_id,
                    num_entries);
    } else
#endif
    { 
        return BCM_E_UNAVAIL;
    }
}
/*
 * Function:
 *      bcm_esw_stat_group_destroy
 * Description:
 *      Release HW counter resources as per given counter id and makes system
 *      unavailable for any further stat collection action
 * Parameters:
 *      unit            - (IN) unit number
 *      Stat_counter_id - (IN) Stat Counter Id
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_stat_group_destroy(
            int    unit,
            uint32 stat_counter_id)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return _bcm_esw_stat_group_destroy(unit,stat_counter_id);
    } else
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit,soc_feature_gport_service_counters)) {
        _bcm_flex_stat_type_t fs_type;
        uint32 fs_inx;

        fs_type = _BCM_FLEX_STAT_TYPE(stat_counter_id);
        fs_inx  = _BCM_FLEX_STAT_COUNT_INX(stat_counter_id);

        return _bcm_esw_flex_stat_count_index_remove(unit,fs_type,
                    fs_inx);
    } else
#endif
#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)) {
        return _bcm_esw_flowcnt_group_destroy(unit, stat_counter_id);

    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_stat_id_get_all
 * Description:
 *      Get all stat ids attached to the stat object.
 * Parameters:
 *      unit            - (IN) unit number
 *      object          - (IN) stat object
 *      stat_max        - (IN) max stat id count
 *      stat_array      - (OUT) array of stat ids
 *      stat_count      - (OUT) actual stat id count
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_stat_id_get_all(
    int unit, 
    bcm_stat_object_t object, 
    int stat_max, 
    uint32 *stat_array, 
    int *stat_count)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return _bcm_esw_stat_id_get_all(
                    unit, 
                    object, 
                    stat_max, 
                    stat_array, 
                    stat_count);
    }
#endif

    return BCM_E_UNAVAIL;
}



/*
 * Function:
 *	bcm_stat_sync
 * Description:
 *	Synchronize software counters with hardware
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_NONE - Success.
 *	BCM_E_INTERNAL - Chip access failure.
 * Notes:
 *	Makes sure all counter hardware activity prior to the call to
 *	bcm_stat_sync is reflected in all bcm_stat_get calls that come
 *	after the call to bcm_stat_sync.
 */

int
bcm_esw_stat_sync(int unit)
{

#if defined(BCM_WARM_BOOT_SUPPORT) && (defined(BCM_TRIUMPH3_SUPPORT) || \
defined(BCM_KATANA_SUPPORT) || defined(BCM_HURRICANE3_SUPPORT))
    if (SOC_WARM_BOOT(unit)) {
        /* Sync WB stat overflow flags to scahe */
        BCM_IF_ERROR_RETURN(_bcm_esw_stat_warm_boot_recover(unit));
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter) &&
        (!soc_feature(unit, soc_feature_centralized_counter))) {
        _bcm_esw_stat_flex_callback(unit);
    }
#endif
    return soc_counter_sync(unit);
}

/*
 * Function:
 *	_bcm_esw_stat_custom_get
 * Description:
 *	Return character name for specified stat value.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	port - zero-based port number
 *      sync_mode - if 1 read hw counter else sw accumualated counter
 *	type - SNMP statistics type (see stat.h)
 *	val  - (OUT) 64 bit statistic counter value
 * Returns:
 *	NULL - invalid SNMP varialble type.
 *	!NULL - pointer to snmp variable name.
 */
STATIC int
_bcm_esw_stat_custom_get_set(int unit, bcm_port_t port, 
                             int sync_mode, int stat_op,
                             bcm_stat_val_t type, uint64 *val)
{
    uint64 count;
    int reg_op = (stat_op == _BCM_STAT_GET)? _BCM_STAT_REG_ADD: 
                                             _BCM_STAT_REG_CLEAR;

    COMPILER_REFERENCE(&count);
    COMPILER_64_ZERO(count);

    switch (type) {
    case snmpBcmCustomReceive0:
        if (SOC_REG_IS_VALID(unit, RDBGC0_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC0_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC0r, &count);
        }
        break;
    case snmpBcmCustomReceive1:
        if (SOC_REG_IS_VALID(unit, RDBGC1_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC1_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC1r, &count);
        }
        break;
    case snmpBcmCustomReceive2:
        if (SOC_REG_IS_VALID(unit, RDBGC2_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC2_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC2r, &count);
        }
        break;
    case snmpBcmCustomReceive3:
        if (SOC_REG_IS_VALID(unit, RDBGC3_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC3_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC3r, &count);
        }
        break;
    case snmpBcmCustomReceive4:
        if (SOC_REG_IS_VALID(unit, RDBGC4_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC4_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC4r, &count);
        }
        break;
    case snmpBcmCustomReceive5:
        if (SOC_REG_IS_VALID(unit, RDBGC5_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC5_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC5r, &count);
        }
        break;
    case snmpBcmCustomReceive6:
        if (SOC_REG_IS_VALID(unit, RDBGC6_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC6_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC6r, &count);
        }
        break;
    case snmpBcmCustomReceive7:
        if (SOC_REG_IS_VALID(unit, RDBGC7_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC7_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC7r, &count);
        }
        break;
    case snmpBcmCustomReceive8:
        if (SOC_REG_IS_VALID(unit, RDBGC8_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC8_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC8r, &count);
        }
        break;
    case snmpBcmCustomTransmit0:
        if (SOC_REG_IS_VALID(unit, TDBGC0_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC0_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC0r, &count);
        }
        break;
    case snmpBcmCustomTransmit1:
        if (SOC_REG_IS_VALID(unit, TDBGC1_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC1_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC1r, &count);
        }
        break;
    case snmpBcmCustomTransmit2:
        if (SOC_REG_IS_VALID(unit, TDBGC2_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC2_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC2r, &count);
        }
        break;
    case snmpBcmCustomTransmit3:
        if (SOC_REG_IS_VALID(unit, TDBGC3_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC3_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC3r, &count);
        }
        break;
    case snmpBcmCustomTransmit4:
        if (SOC_REG_IS_VALID(unit, TDBGC4_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC4_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC4r, &count);
        }
        break;
    case snmpBcmCustomTransmit5:
        if (SOC_REG_IS_VALID(unit, TDBGC5_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC5_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC5r, &count);
        }
        break;
    case snmpBcmCustomTransmit6:
        if (SOC_REG_IS_VALID(unit, TDBGC6_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC6_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC6r, &count);
        }
        break;
    case snmpBcmCustomTransmit7:
        if (SOC_REG_IS_VALID(unit, TDBGC7_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC7_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC7r, &count);
        }
        break;
    case snmpBcmCustomTransmit8:
        if (SOC_REG_IS_VALID(unit, TDBGC8_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC8_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC8r, &count);
        }
        break;
    case snmpBcmCustomTransmit9:
        if (SOC_REG_IS_VALID(unit, TDBGC9_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC9_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC9r, &count);
        }
        break;
    case snmpBcmCustomTransmit10:
        if (SOC_REG_IS_VALID(unit, TDBGC10_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC10_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC10r, &count);
        }
        break;
    case snmpBcmCustomTransmit11:
        if (SOC_REG_IS_VALID(unit, TDBGC11_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC11_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC11r, &count);
        }
        break;
    case snmpBcmCustomTransmit12:
        if (SOC_REG_IS_VALID(unit, TDBGC12_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC12_64r, &count);
        }
        break;
    case snmpBcmCustomTransmit13:
        if (SOC_REG_IS_VALID(unit, TDBGC13_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC13_64r, &count);
        }
        break;
    case snmpBcmCustomTransmit14:
        if (SOC_REG_IS_VALID(unit, TDBGC14_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC14_64r, &count);
        }
        break;
    default:
        return BCM_E_PARAM;
    }
    if (stat_op == _BCM_STAT_GET) {
        *val = count;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *  _bcm_system_level_stat_get_set
 * Description:
 *  Get or clear specified stat value.
 * Parameters:
 *  unit      - StrataSwitch PCI device unit number (driver internal).
 *  sync_mode - if 1 read hw counter else sw accumualated counter
 *  stat_op   - Stat operation descriptor
 *  type      - SNMP statistics type (see stat.h)
 *  val       - (OUT) 64 bit statistic counter value
 * Returns:
 *  BCM_E_XXX
 */
STATIC int
_bcm_system_level_stat_get_set(int unit, int sync_mode, int stat_op,
                               bcm_stat_val_t type, uint64 *val)
{
    int (*counter_get) (int , soc_port_t , soc_reg_t , int , uint64 *);
    uint64 val64_zero;

    COMPILER_64_ZERO(val64_zero);
    counter_get = (sync_mode == 1)? soc_counter_sync_get:soc_counter_get;
    switch (type) {
        case snmpBcmE2EHOLDropPkts:
            if (stat_op == _BCM_STAT_GET) {
                BCM_IF_ERROR_RETURN(counter_get(unit, -1,
                            SOC_COUNTER_NON_DMA_E2E_DROP_COUNT, 0, val));
            } else if (stat_op == _BCM_STAT_CLEAR) {
                BCM_IF_ERROR_RETURN(soc_counter_set(unit, 0,
                            SOC_COUNTER_NON_DMA_E2E_DROP_COUNT, 0,
                                                    val64_zero));
            }
            break;
        default:
            /* To be consistent with return type of
             * _bcm_esw_stat_gport_validate */
            return BCM_E_PORT;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_esw_stat_get_set
 * Description:
 *	Get/Set the specified statistic from the StrataSwitch
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	port - zero-based port number
 *	sync_mode - zero-based port number
 *	stat_op - indicates stat get or clear(set) operation
 *	type - SNMP statistics type (see stat.h)
 *  val - (OUT) 64-bit counter value.
 * Returns:
 *	BCM_E_NONE - Success.
 *	BCM_E_PARAM - Illegal parameter.
 *	BCM_E_BADID - Illegal port number.
 *	BCM_E_INTERNAL - Chip access failure.
 *	BCM_E_UNAVAIL - Counter/variable is not implemented
 *				on this current chip.
 * Notes:
 *	Some counters are implemented on a given port only when it is
 *	operating in a specific mode, for example, 10 or 100, and not 
 *	1000. If the counter is not implemented on a given port, OR, 
 *	on the port given its current operating mode, BCM_E_UNAVAIL
 *	is returned.
 */

int
_bcm_esw_stat_get_set(int unit, bcm_port_t port, 
                      int sync_mode, int stat_op, 
                      bcm_stat_val_t type, uint64 *val)
{
    int rv, rv1;
    soc_mac_mode_t	mode;

    if ((val == NULL) && (stat_op != _BCM_STAT_CLEAR)) {
        return BCM_E_PARAM;
    }

    if (port == BCM_GPORT_INVALID) {
        return _bcm_system_level_stat_get_set(unit, sync_mode,
                                              stat_op, type, val);
    }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_gport_validate(unit, port, &port));
    
    if (soc_feature(unit, soc_feature_stat_xgs3)) {
        rv = _bcm_esw_stat_custom_get_set(unit, port, sync_mode, 
                                          stat_op, type, val);
        if (BCM_E_PARAM != rv) {
            return rv;
        }
    } 

    if (port == CMIC_PORT(unit)) {
    /* Rudimentary CPU statistics -- needs work */
    switch (type) {
    case snmpIfInOctets:
        if (stat_op == _BCM_STAT_GET) {
            COMPILER_64_SET(*val, 0, SOC_CONTROL(unit)->stat.dma_rbyt);
        } else {
            SOC_CONTROL(unit)->stat.dma_rbyt = 0;
        }
        break;
    case snmpIfInUcastPkts:
        if (stat_op == _BCM_STAT_GET) {
            COMPILER_64_SET(*val, 0, SOC_CONTROL(unit)->stat.dma_rpkt);
        } else {
            SOC_CONTROL(unit)->stat.dma_rpkt = 0;
        }
        break;
    case snmpIfOutOctets:
        if (stat_op == _BCM_STAT_GET) {
            COMPILER_64_SET(*val, 0, SOC_CONTROL(unit)->stat.dma_tbyt);
        } else {
            SOC_CONTROL(unit)->stat.dma_tbyt = 0;
        }
        break;
    case snmpIfOutUcastPkts:
        if (stat_op == _BCM_STAT_GET) {
            COMPILER_64_SET(*val, 0, SOC_CONTROL(unit)->stat.dma_tpkt);
        } else {
            SOC_CONTROL(unit)->stat.dma_tpkt = 0;
        }
        break;
    case snmpIfOutDiscards:
        /* Try to get cpu hw counter */
        if (soc_feature(unit, soc_feature_generic_counters)) {
            if (SOC_IS_HURRICANE3(unit)) {
                return _bcm_stat_generic_get_set(unit, port, sync_mode,
                                                 stat_op, type, val);
            }
        }
        break;
    default:
        if (stat_op == _BCM_STAT_GET) {
            COMPILER_64_ZERO(*val);
        }
        /* Try to get cpu hw counter */
        if (soc_feature(unit, soc_feature_generic_counters)) {
            if (SOC_IS_TD_TT(unit)) {
                return _bcm_stat_generic_get_set(unit, port, sync_mode, 
                                                 stat_op, type, val);    
            }
        }
        break;
    }
    return (BCM_E_NONE);
    }

    if (soc_feature(unit, soc_feature_generic_counters)) {
#ifdef BCM_KATANA_SUPPORT
        if (SOC_IS_KATANA(unit)) { 
            if(IS_MXQ_PORT(unit, port)) { 
                return _bcm_stat_generic_get_set(unit, port, sync_mode, 
                                                 stat_op, type, val);
            } else if(IS_GE_PORT(unit, port) || IS_FE_PORT(unit, port)) { 
                return (_bcm_stat_ge_get_set(unit, port, sync_mode, stat_op,
                                             type, val, TRUE));
            } 
        } else 
#endif
#ifdef BCM_HURRICANE2_SUPPORT
	    if (SOC_IS_HURRICANE2(unit) || SOC_IS_GREYHOUND(unit) ||
            SOC_IS_HURRICANE3(unit)) {
		    if(IS_XL_PORT(unit, port)) { 
                return _bcm_stat_generic_get_set(unit, port, sync_mode, 
                                                 stat_op, type, val);
		    } else if(IS_GE_PORT(unit, port) || IS_FE_PORT(unit, port)) { 
                return (_bcm_stat_ge_get_set(unit, port, sync_mode, stat_op,
                                             type, val, TRUE));
		    } 
	    } else 
#endif
#ifdef BCM_SABER2_SUPPORT
	    if (SOC_IS_SABER2(unit)) {
                 if (IS_MXQ_PORT(unit, port)) {
                    return _bcm_stat_mxq_get_set(unit, port, sync_mode,
                                           stat_op, type, val);
                 } else {
                    return _bcm_stat_generic_get_set(unit, port, sync_mode,
                                           stat_op, type, val);
                }
	    } else 
#endif
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit) &&
            (IS_QSGMII_PORT(unit, port) || IS_EGPHY_PORT(unit, port))) {
            return _bcm_stat_ge_get_set(unit, port, sync_mode, stat_op,
                                        type, val, TRUE);
        } else
#endif
#ifdef BCM_HELIX5_SUPPORT
        if (SOC_IS_HELIX5(unit) && IS_QSGMII_PORT(unit, port)) {
            return _bcm_stat_ge_get_set(unit, port, sync_mode, stat_op,
                                        type, val, TRUE);
        } else
#endif
        {   
            if (soc_feature(unit, soc_feature_preempt_mib_support) 
                && SOC_IS_MONTEREY(unit)) {
                return bcmi_stat_preempt_get_set(unit, port, sync_mode,
                                                stat_op, type, val);
            } 
            return _bcm_stat_generic_get_set(unit, port, sync_mode, 
                                             stat_op, type, val);
        }
    }

    /* For ports which have preempt mib counters support */
    if (soc_feature(unit, soc_feature_preempt_mib_support)) {
        if (IS_XL_PORT(unit, port)) {
            return bcmi_stat_preempt_xl_get_set(unit, port, sync_mode,
                                                stat_op, type, val);
        } else if (IS_GE_PORT(unit, port) && !IS_CL_PORT(unit, port)) {
            return (bcmi_stat_preempt_ge_get_set(unit, port, sync_mode, stat_op,
                                                 type, val, TRUE));
        }
    }

    /* CLMIB counters support for CL port */
    if (soc_feature(unit, soc_feature_clmib_support)) {
        if (IS_CL_PORT(unit, port)) {
            return bcmi_stat_clmib_get_set(unit, port, sync_mode,
                                           stat_op, type, val);
        }
    }

    /* For xgs3 gig ports, always use gig get function */
    if ((IS_FE_PORT(unit, port) && (soc_feature(unit, soc_feature_trimac) 
        || soc_feature(unit, soc_feature_unimac))) ||
        (SOC_IS_XGS3_SWITCH(unit) && IS_GE_PORT(unit, port)))  {
        return (_bcm_stat_ge_get_set(unit, port, sync_mode, stat_op, 
                                     type, val, TRUE));
    }

    SOC_IF_ERROR_RETURN(soc_mac_mode_get(unit, port, &mode));

    switch (mode) {
    case SOC_MAC_MODE_10_100:
	return (_bcm_stat_fe_get_set(unit, port, sync_mode,
                                 stat_op, type, val));
    case SOC_MAC_MODE_1000_T:
        return (_bcm_stat_ge_get_set(unit, port, sync_mode, stat_op, 
                                     type, val, TRUE));
    case SOC_MAC_MODE_10000:
        if (IS_XE_PORT(unit, port)) {
            rv = _bcm_stat_xe_get_set(unit, port, sync_mode,
                                      stat_op, type, val);
            if((rv != BCM_E_NONE) && (rv != BCM_E_UNAVAIL)) {
                return rv;
            }
            if (IS_XQ_PORT(unit, port)) {
                return rv;
            } else if (IS_GX_PORT(unit, port)) {
                uint64 val1;
                rv1 = _bcm_stat_ge_get_set(unit, port, sync_mode,
                                           stat_op, type,
                                           &val1, FALSE);
                if((rv1 != BCM_E_NONE) && (rv1 != BCM_E_UNAVAIL)) {
                    return rv1;
                }
                if ((rv == BCM_E_UNAVAIL) && (rv1 == BCM_E_UNAVAIL)) {
                    return BCM_E_UNAVAIL;
                }
                if (stat_op == _BCM_STAT_GET) {
                    COMPILER_64_ADD_64(*val, val1);
                }
            }
            return SOC_E_NONE;
        } else {
            return (_bcm_stat_hg_get_set(unit, port, sync_mode,
                                         stat_op, type, val));
        }
    default:
	assert(0);
    }

    return(BCM_E_NONE);
}

/*
 * Function:
 *	bcm_stat_get
 * Description:
 *	Get the specified statistic from the StrataSwitch
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	port - zero-based port number
 *	type - SNMP statistics type (see stat.h)
 *  val - (OUT) 64-bit counter value.
 * Returns:
 *	BCM_E_NONE - Success.
 *	BCM_E_PARAM - Illegal parameter.
 *	BCM_E_BADID - Illegal port number.
 *	BCM_E_INTERNAL - Chip access failure.
 *	BCM_E_UNAVAIL - Counter/variable is not implemented
 *				on this current chip.
 * Notes:
 *	Some counters are implemented on a given port only when it is
 *	operating in a specific mode, for example, 10 or 100, and not 
 *	1000. If the counter is not implemented on a given port, OR, 
 *	on the port given its current operating mode, BCM_E_UNAVAIL
 *	is returned.
 */

int
_bcm_esw_stat_get(int unit, bcm_port_t port, int sync_mode, 
                  bcm_stat_val_t type, uint64 *val)
{
    return (_bcm_esw_stat_get_set(unit, port, sync_mode, 
                                  _BCM_STAT_GET, type, val));
}


/*
 * Function:
 *	bcm_stat_get
 * Description:
 *	Get the specified statistic from the StrataSwitch
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	port - zero-based port number
 *	type - SNMP statistics type (see stat.h)
 *      val - (OUT) 64-bit counter value.
 * Returns:
 *	BCM_E_NONE - Success.
 *	BCM_E_PARAM - Illegal parameter.
 *	BCM_E_BADID - Illegal port number.
 *	BCM_E_INTERNAL - Chip access failure.
 *	BCM_E_UNAVAIL - Counter/variable is not implemented
 *				on this current chip.
 * Notes:
 *	Some counters are implemented on a given port only when it is
 *	operating in a specific mode, for example, 10 or 100, and not 
 *	1000. If the counter is not implemented on a given port, OR, 
 *	on the port given its current operating mode, BCM_E_UNAVAIL
 *	is returned.
 */

int
bcm_esw_stat_get(int unit, bcm_port_t port, bcm_stat_val_t type, uint64 *val)
{
    /* read software accumulated counters */
    return _bcm_esw_stat_get(unit, port, 0, type, val);
}


/*
 * Function:
 *	bcm_stat_get32
 * Description:
 *	Get the specified statistic from the StrataSwitch
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	port - zero-based port number
 *	type - SNMP statistics type (see stat.h)
 *      val - (OUT) 32-bit counter value.
 * Returns:
 *	BCM_E_NONE - Success.
 *	BCM_E_PARAM - Illegal parameter.
 *	BCM_E_BADID - Illegal port number.
 *	BCM_E_INTERNAL - Chip access failure.
 * Notes:
 *	Same as bcm_stat_get, except converts result to 32-bit.
 */

int
bcm_esw_stat_get32(int unit, bcm_port_t port, bcm_stat_val_t type, uint32 *val)
{
    int			rv;
    uint64		val64;

    rv = bcm_esw_stat_get(unit, port, type, &val64);

    COMPILER_64_TO_32_LO(*val, val64);

    return(rv);
}

/*
 * Function:
 *      bcm_esw_stat_multi_get
 * Purpose:
 *      Get the specified statistics from the device.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) <UNDEF>
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Array of SNMP statistics types defined in bcm_stat_val_t
 *      value_arr - (OUT) Collected statistics values
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_stat_multi_get(int unit, bcm_port_t port, int nstat, 
                       bcm_stat_val_t *stat_arr, uint64 *value_arr)
{
    int stix;

    if (nstat <= 0) {
        return BCM_E_PARAM;
    }

    if ((NULL == stat_arr) || (NULL == value_arr)) {
        return BCM_E_PARAM;
    }

    for (stix = 0; stix < nstat; stix++) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_stat_get(unit, port, stat_arr[stix],
                              &(value_arr[stix])));
    }

    return BCM_E_NONE; 
}

/*
 * Function:
 *      bcm_esw_stat_multi_get32
 * Purpose:
 *      Get the specified statistics from the device.  The 64-bit
 *      values may be truncated to fit.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) <UNDEF>
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Array of SNMP statistics types defined in bcm_stat_val_t
 *      value_arr - (OUT) Collected 32-bit statistics values
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_stat_multi_get32(int unit, bcm_port_t port, int nstat, 
                         bcm_stat_val_t *stat_arr, uint32 *value_arr)
{
    int stix;

    if (nstat <= 0) {
        return BCM_E_PARAM;
    }

    if ((NULL == stat_arr) || (NULL == value_arr)) {
        return BCM_E_PARAM;
    }

    for (stix = 0; stix < nstat; stix++) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_stat_get32(unit, port, stat_arr[stix],
                                &(value_arr[stix])));
    }

    return BCM_E_NONE; 
}

/*
 * Function:
 *    bcm_stat_sync_get
 * Description:
 *    Get the specified statistic from the StrataSwitch
 * Parameters:
 *    unit - StrataSwitch PCI device unit number (driver internal).
 *    port - zero-based port number
 *    type - SNMP statistics type (see stat.h)
 *    val - (OUT) 64-bit counter value.
 * Returns:
 *    BCM_E_NONE - Success.
 *    BCM_E_PARAM - Illegal parameter.
 *    BCM_E_BADID - Illegal port number.
 *    BCM_E_INTERNAL - Chip access failure.
 *    BCM_E_UNAVAIL - Counter/variable is not implemented
 *                    on this current chip.
 * Notes:
 *    Some counters are implemented on a given port only when it is
 *    operating in a specific mode, for example, 10 or 100, and not 
 *    1000. If the counter is not implemented on a given port, OR, 
 *    on the port given its current operating mode, BCM_E_UNAVAIL
 *    is returned.
 */

int
bcm_esw_stat_sync_get(int unit, bcm_port_t port, 
                           bcm_stat_val_t type, uint64 *val)
{
    /* read counter from the device */
    return _bcm_esw_stat_get(unit, port, 1, type, val);
}


/*
 * Function:
 *    bcm_stat_sync_get32
 * Description:
 *    Get the specified statistic from the StrataSwitch
 * Parameters:
 *    unit - StrataSwitch PCI device unit number (driver internal).
 *    port - zero-based port number
 *    type - SNMP statistics type (see stat.h)
 *    val  - (OUT) 32-bit counter value.
 * Returns:
 *    BCM_E_NONE - Success.
 *    BCM_E_PARAM - Illegal parameter.
 *    BCM_E_BADID - Illegal port number.
 *    BCM_E_INTERNAL - Chip access failure.
 * Notes:
 *    Same as bcm_stat_get, except converts result to 32-bit.
 */

int
bcm_esw_stat_sync_get32(int unit, bcm_port_t port, 
                             bcm_stat_val_t type, uint32 *val)
{
    int    rv;
    uint64 val64;

    rv = bcm_esw_stat_sync_get(unit, port, type, &val64);

    COMPILER_64_TO_32_LO(*val, val64);

    return(rv);
}

/*
 * Function:
 *      bcm_esw_stat_sync_multi_get
 * Purpose:
 *      Get the specified statistics from the device.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) <UNDEF>
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Array of SNMP statistics types defined in bcm_stat_val_t
 *      value_arr - (OUT) Collected statistics values
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_stat_sync_multi_get(int unit, bcm_port_t port, int nstat, 
                                 bcm_stat_val_t *stat_arr, uint64 *value_arr)
{
    int stix;

    if (nstat <= 0) {
        return BCM_E_PARAM;
    }

    if ((NULL == stat_arr) || (NULL == value_arr)) {
        return BCM_E_PARAM;
    }

    for (stix = 0; stix < nstat; stix++) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_stat_sync_get(unit, port, stat_arr[stix],
                                        &(value_arr[stix])));
    }

    return BCM_E_NONE; 
}

/*
 * Function:
 *      bcm_esw_stat_sync_multi_get32
 * Purpose:
 *      Get the specified statistics from the device.  The 64-bit
 *      values may be truncated to fit.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) <UNDEF>
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Array of SNMP statistics types defined in bcm_stat_val_t
 *      value_arr - (OUT) Collected 32-bit statistics values
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_stat_sync_multi_get32(int unit, bcm_port_t port,
                                   int nstat, bcm_stat_val_t *stat_arr, 
                                    uint32 *value_arr)
{
    int stix;

    if (nstat <= 0) {
        return BCM_E_PARAM;
    }

    if ((NULL == stat_arr) || (NULL == value_arr)) {
        return BCM_E_PARAM;
    }

    for (stix = 0; stix < nstat; stix++) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_stat_sync_get32(unit, port, stat_arr[stix],
                                          &(value_arr[stix])));
    }

    return BCM_E_NONE; 
}

/*
 * Function:
 *    bcm_esw_stat_direct_get
 * Description:
 *    Get the specified statistic from the StrataSwitch
 * Parameters:
 *    unit - StrataSwitch PCI device unit number (driver internal).
 *    port - zero-based port number
 *    type - SNMP statistics type (see stat.h)
 *    val - (OUT) 64-bit counter value.
 * Returns:
 *    BCM_E_NONE - Success.
 *    BCM_E_PARAM - Illegal parameter.
 *    BCM_E_BADID - Illegal port number.
 *    BCM_E_INTERNAL - Chip access failure.
 *    BCM_E_UNAVAIL - Counter/variable is not implemented
 *                    on this current chip.
 * Notes:
 *    Hardware counter value will be fetched and returned.
 *    Software counter cache will not be updated.
 *    Only RxPFCFrameXXX stat types are currently supported.
 */

int
bcm_esw_stat_direct_get(int unit, bcm_port_t port,
                        bcm_stat_val_t type, uint64 *val)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT) || \
    defined(BCM_TRIDENT3_SUPPORT) || \
    defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TOMAHAWK2_SUPPORT) || \
    defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TRIDENT2(unit) || SOC_IS_TRIDENT2PLUS(unit) ||
        SOC_IS_TRIDENT3X(unit) ||
        SOC_IS_TOMAHAWK(unit) || SOC_IS_TOMAHAWK2(unit) ||
        SOC_IS_TOMAHAWK3(unit)) {
        if (!((type >= snmpBcmRxPFCFrameXonPriority0) &&
              (type <= snmpBcmRxPFCFramePriority7))) {
            return BCM_E_UNAVAIL;
        }
        /* read counter from the device */
        rv = _bcm_esw_stat_get(unit, port, _BCM_SYNC_MODE_NO_CACHE, type, val);
    }
#endif /* BCM_TRIDENT2_SUPPORT and other applied devices */

    return rv;
}

/*
 * Function:
 *      bcm_esw_stat_direct_multi_get
 * Purpose:
 *      Get the specified statistics from the device.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Port number
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Array of SNMP statistics types defined in bcm_stat_val_t
 *      value_arr - (OUT) Collected statistics values
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_esw_stat_direct_multi_get(int unit, bcm_port_t port, int nstat,
                              bcm_stat_val_t *stat_arr, uint64 *value_arr)
{
    int stix;

    if (nstat <= 0) {
        return BCM_E_PARAM;
    }

    if ((NULL == stat_arr) || (NULL == value_arr)) {
        return BCM_E_PARAM;
    }

    for (stix = 0; stix < nstat; stix++) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_stat_direct_get(unit, port, stat_arr[stix],
                                     &(value_arr[stix])));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_stat_clear
 * Description:
 *	Clear the port based statistics from the StrataSwitch port.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	port - zero-based port number
 * Returns:
 *	BCM_E_NONE - Success.
 *	BCM_E_INTERNAL - Chip access failure.
 */

int 
bcm_esw_stat_clear(int unit, bcm_port_t port)
{
    soc_reg_t           reg;
    pbmp_t		pbm;
    int                 i;

#ifdef BCM_KATANA2_SUPPORT
    /* Allow only physical ports */
#if defined BCM_METROLITE_SUPPORT
    if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
        if (_SOC_IS_PP_PORT_LINKPHY_SUBTAG(unit, port) &&
            !_SOC_IS_PORT_LINKPHY_SUBTAG(unit, port)) {
            return (BCM_E_PORT);
        }
    } else
#endif
    {
        if (soc_feature(unit, soc_feature_linkphy_coe)) {
            if (_BCM_KT2_GPORT_IS_LINKPHY_SUBPORT_PORT(unit, port) ||
                _BCM_KT2_GPORT_IS_LINKPHY_SUBPORT_GROUP(port)) {
                return (BCM_E_PORT);
            }
        }

        if (soc_feature(unit, soc_feature_subtag_coe)) {
            if (_BCM_KT2_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port) ||
                _BCM_KT2_GPORT_IS_SUBTAG_SUBPORT_GROUP(port)) {
                return (BCM_E_PORT);
            }
        }
    }
#endif

    BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_gport_validate(unit, port, &port));

    if (port == CMIC_PORT(unit)) {
        /* Rudimentary CPU statistics -- needs work */
        SOC_CONTROL(unit)->stat.dma_rbyt = 0;
        SOC_CONTROL(unit)->stat.dma_rpkt = 0;
        SOC_CONTROL(unit)->stat.dma_tbyt = 0;
        SOC_CONTROL(unit)->stat.dma_tpkt = 0;

        if (soc_feature(unit, soc_feature_generic_counters)) {
            /* Clear snmpIfOutDiscards CPU HW counter for Hurrican3 */
            if (SOC_IS_HURRICANE3(unit)) {
                return bcm_esw_stat_clear_single(unit, port, snmpIfOutDiscards);
            }
            /* Clear CPU HW counter, only work for TD_TT series chips now */
            if (SOC_IS_TD_TT(unit)) {
                goto clear_hw_counter;
            }
        }
        return (BCM_E_NONE);
    }

clear_hw_counter:
    SOC_PBMP_CLEAR(pbm);
    SOC_PBMP_PORT_ADD(pbm, port);
    BCM_IF_ERROR_RETURN(soc_counter_set32_by_port(unit, pbm, 0));

    if (NULL != _bcm_stat_extra_counters) {
    for (i = 0; i < _BCM_STAT_EXTRA_COUNTER_COUNT; i++) {
        reg = _bcm_stat_extra_counters[unit][i].reg;
        if (reg == INVALIDr) {
            continue;
        }
        if (SOC_REG_INFO(unit, reg).regtype != soc_portreg) {
            continue;
        }
        COMPILER_64_ZERO(_bcm_stat_extra_counters[unit][i].count64[port]);
    }
    }

    return(BCM_E_NONE);
}

/*
 * Function:
 *	bcm_stat_clear_single
 * Description:
 *	Clear the port based statistics from the StrataSwitch port.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	port - zero-based port number
 * Returns:
 *	BCM_E_NONE - Success.
 *	BCM_E_INTERNAL - Chip access failure.
 */

int 
bcm_esw_stat_clear_single(int unit, bcm_port_t port, bcm_stat_val_t stat_id)
{
    return (_bcm_esw_stat_get_set(unit, port, 0, 
                                  _BCM_STAT_CLEAR, stat_id, NULL));
}

#ifdef BCM_XGS3_SWITCH_SUPPORT
#define _DBG_CNTR_IS_VALID(unit, type)                                   \
    ((type >= snmpBcmCustomReceive0 && type <= snmpBcmCustomReceive8) || \
    (type >= snmpBcmCustomTransmit0 && type <= (SOC_IS_FBX(unit) ?  \
    snmpBcmCustomTransmit11 : snmpBcmCustomTransmit14))) 

#define _DBG_CNTR_IS_RSV(type)                                           \
    ((type >= snmpBcmCustomReceive1 && type <= snmpBcmCustomReceive2) || \
     (type >= snmpBcmCustomTransmit0 && type <= snmpBcmCustomTransmit5)) 

#define _DBG_FLAG_IS_VALID(type, flag)                                   \
    (((type == snmpBcmCustomReceive0) && (flag == BCM_DBG_CNT_DROP)) ||  \
     ((type == snmpBcmCustomReceive1) && (flag == BCM_DBG_CNT_IMBP)) ||  \
     ((type == snmpBcmCustomReceive2) && (flag == BCM_DBG_CNT_RIMDR)) || \
     ((type == snmpBcmCustomTransmit0) &&                                \
      (flag == (BCM_DBG_CNT_TGIPMC6 | BCM_DBG_CNT_TGIP6))) ||            \
     ((type == snmpBcmCustomTransmit1) &&                                \
      (flag == (BCM_DBG_CNT_TIPMCD6 | BCM_DBG_CNT_TIPD6))) ||            \
     ((type == snmpBcmCustomTransmit2) &&                                \
      (flag == BCM_DBG_CNT_TGIPMC6)) ||                                  \
     ((type == snmpBcmCustomTransmit3) &&                                \
      (flag == BCM_DBG_CNT_TPKTD)) ||                                    \
     ((type == snmpBcmCustomTransmit4) &&                                \
      (flag == (BCM_DBG_CNT_TGIP4 | BCM_DBG_CNT_TGIP6))) ||              \
     ((type == snmpBcmCustomTransmit5) &&                                \
      (flag == (BCM_DBG_CNT_TIPMCD4 | BCM_DBG_CNT_TIPMCD6)))) 


typedef struct bcm_dbg_cntr_s {
    bcm_stat_val_t   counter;
    soc_reg_t        reg;
    soc_reg_t        select;
} bcm_dbg_cntr_t;


bcm_dbg_cntr_t bcm_dbg_cntr_rx[] = {
    { snmpBcmCustomReceive0,   RDBGC0r,  RDBGC0_SELECTr  },
    { snmpBcmCustomReceive1,   RDBGC1r,  RDBGC1_SELECTr  },
    { snmpBcmCustomReceive2,   RDBGC2r,  RDBGC2_SELECTr  },
    { snmpBcmCustomReceive3,   RDBGC3r,  RDBGC3_SELECTr  },
    { snmpBcmCustomReceive4,   RDBGC4r,  RDBGC4_SELECTr  },
    { snmpBcmCustomReceive5,   RDBGC5r,  RDBGC5_SELECTr  },
    { snmpBcmCustomReceive6,   RDBGC6r,  RDBGC6_SELECTr  },
    { snmpBcmCustomReceive7,   RDBGC7r,  RDBGC7_SELECTr  },
    { snmpBcmCustomReceive8,   RDBGC8r,  RDBGC8_SELECTr  }
};

bcm_dbg_cntr_t bcm_dbg_cntr_tx[] = {
    { snmpBcmCustomTransmit0,  TDBGC0r,  TDBGC0_SELECTr  },
    { snmpBcmCustomTransmit1,  TDBGC1r,  TDBGC1_SELECTr  },
    { snmpBcmCustomTransmit2,  TDBGC2r,  TDBGC2_SELECTr  },
    { snmpBcmCustomTransmit3,  TDBGC3r,  TDBGC3_SELECTr  },
    { snmpBcmCustomTransmit4,  TDBGC4r,  TDBGC4_SELECTr  },
    { snmpBcmCustomTransmit5,  TDBGC5r,  TDBGC5_SELECTr  },
    { snmpBcmCustomTransmit6,  TDBGC6r,  TDBGC6_SELECTr  },
    { snmpBcmCustomTransmit7,  TDBGC7r,  TDBGC7_SELECTr  },
    { snmpBcmCustomTransmit8,  TDBGC8r,  TDBGC8_SELECTr  },
    { snmpBcmCustomTransmit9,  TDBGC9r,  TDBGC9_SELECTr  },
    { snmpBcmCustomTransmit10, TDBGC10r, TDBGC10_SELECTr },
    { snmpBcmCustomTransmit11, TDBGC11r, TDBGC11_SELECTr }
};

#endif /* BCM_XGS3_SWITCH_SUPPORT */

/*
 * Function:
 *      bcm_stat_custom_set 
 * Description:
 *      Set debug counter to count certain packet types.
 * Parameters:
 *      unit  - StrataSwitch PCI device unit number.
 *      port  - Port number, -1 to set all ports. 
 *      type  - SNMP statistics type.
 *      flags - The counter select value (see stat.h for bit definitions). 
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The following debug counters are reserved for maintaining
 *      currently supported SNMP MIB objects:
 *
 *      snmpBcmCustomReceive0 - snmpBcmCustomReceive2
 *      snmpBcmCustomTransmit0 - snmpBcmCustomTransmit5
 */

#define SET(reg, field, flag)						\
	soc_reg_field_set(unit, (reg),					\
		          &ctr_sel, (field), (flags & (flag)) ? 1 : 0)

int
bcm_esw_stat_custom_set(int unit, bcm_port_t port,
		    bcm_stat_val_t type, uint32 flags)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    uint32	ctr_sel = 0;
    int		i;

    if (!SOC_IS_XGS3_SWITCH(unit)) {
        return BCM_E_UNAVAIL;
    }

    if (!_DBG_CNTR_IS_VALID(unit, type)) {
        return BCM_E_PARAM;
    }

    if (_DBG_CNTR_IS_RSV(type)) {
        if (!_DBG_FLAG_IS_VALID(type, flags)) {
            return BCM_E_CONFIG;
        }
    }

    /* port = -1 is valid port in that case */
    if ((port != -1)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_stat_gport_validate(unit, port, &port));
    }

    for (i = 0; i < COUNTOF(bcm_dbg_cntr_rx); i++) {
        if (bcm_dbg_cntr_rx[i].counter == type) {
            if (SOC_IS_FBX(unit)) {
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit,  bcm_dbg_cntr_rx[i].select,
                                                  REG_PORT_ANY, 0, &ctr_sel));
                if (SOC_IS_FBX(unit)) {
                    uint32 mask;

                    if (soc_feature(unit, soc_feature_dbgc_higig_lkup)) {
                        mask = BCM_FB_B0_DBG_CNT_RMASK; 
                    } else if (SOC_IS_HB_GW(unit) || SOC_IS_TRX(unit)) {
                        mask = BCM_HB_DBG_CNT_RMASK; 
                    } else if (SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit) ||
                               SOC_IS_HAWKEYE(unit)){
                        mask = BCM_RP_DBG_CNT_RMASK; 
                    } else {
                        mask = BCM_FB_DBG_CNT_RMASK; 
                    }

                    soc_reg_field_set(unit, bcm_dbg_cntr_rx[i].select, 
                                      &ctr_sel, BITMAPf, flags & mask);
                } 
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, bcm_dbg_cntr_rx[i].select,
                                                  REG_PORT_ANY, 0, ctr_sel));
            }
        }
    }

    for (i = 0; i < COUNTOF(bcm_dbg_cntr_tx); i++) {
        if (bcm_dbg_cntr_tx[i].counter == type) {
            if (SOC_IS_FBX(unit)) {
                uint32 mask;

                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, bcm_dbg_cntr_tx[i].select,
                                                  REG_PORT_ANY, 0, &ctr_sel));

                if (soc_feature(unit, soc_feature_dbgc_higig_lkup)) {
                    mask = BCM_FB_B0_DBG_CNT_TMASK; 
                } else if (SOC_IS_HB_GW(unit)) {
                    mask = BCM_HB_DBG_CNT_TMASK; 
                } else if (SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit) ||
                           SOC_IS_HAWKEYE(unit)){
                    mask = BCM_RP_DBG_CNT_TMASK; 
                } else {
                    mask = BCM_FB_DBG_CNT_TMASK; 
                }

                soc_reg_field_set(unit, bcm_dbg_cntr_tx[i].select, &ctr_sel, 
                                  BITMAPf, flags & mask);

                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, bcm_dbg_cntr_tx[i].select,
                                                  REG_PORT_ANY, 0, ctr_sel));
            }
            else {
                soc_counter_set32_by_reg(unit, bcm_dbg_cntr_tx[i].reg, 0, 0);
            }
            break;
        }
    }

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif  /* BCM_XGS3_SWITCH_SUPPORT */
}

/*
 * Function:
 *      bcm_stat_custom_get 
 * Description:
 *      Get debug counter select value.
 * Parameters:
 *      unit  - StrataSwitch PCI device unit number.
 *      port  - Port number (unused).
 *      type  - SNMP statistics type.
 *      flags - (OUT) The counter select value
 *		      (see stat.h for bit definitions).
 * Returns:
 *      BCM_E_XXX
 */

#define GET(reg, field, flag)						\
	if (soc_reg_field_get(unit, (reg), ctr_sel, (field))) {		\
	    tmp_flags |= (flag);					\
	}

int
bcm_esw_stat_custom_get(int unit, bcm_port_t port,
		    bcm_stat_val_t type, uint32 *flags)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    uint32	ctr_sel;
    int		i;

    if (!SOC_IS_XGS3_SWITCH(unit)) {
        return BCM_E_UNAVAIL;
    }

    if (!_DBG_CNTR_IS_VALID(unit, type)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_gport_validate(unit, port, &port));

    for (i = 0; i < COUNTOF(bcm_dbg_cntr_rx); i++) {
        if (bcm_dbg_cntr_rx[i].counter == type) {
            if (SOC_IS_FBX(unit)) {
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, bcm_dbg_cntr_rx[i].select,
                                                  REG_PORT_ANY, 0, &ctr_sel));
                if (SOC_IS_FBX(unit)) {
                    *flags = soc_reg_field_get(unit, bcm_dbg_cntr_rx[i].select,
                                               ctr_sel, BITMAPf);
                } 
            }
            break;
        }
    }

    for (i = 0; i < COUNTOF(bcm_dbg_cntr_tx); i++) {
        if (bcm_dbg_cntr_tx[i].counter == type) {
            if (SOC_IS_FBX(unit)) {
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, bcm_dbg_cntr_tx[i].select,
                                                  REG_PORT_ANY, 0, &ctr_sel));
                    *flags = soc_reg_field_get(unit, bcm_dbg_cntr_tx[i].select,
                                               ctr_sel, BITMAPf);
	    }
        break;
        }
    }
    return BCM_E_NONE;

#else
    return BCM_E_UNAVAIL;

#endif  /* BCM_XGS3_SWITCH_SUPPORT */
}


/*
 * Function:
 *      _bcm_stat_custom_to_bit 
 * Description:
 *      Calculate the bit that should be turned on for specific trigger 
 * Parameters:
 *      unit  - StrataSwitch PCI device unit number.
 *      chan  - flag to indicate RX or TX DBG counter
 *      trigger - The counter select value
 *		      (see stat.h for bit definitions).
 *      result - [OUT] - bit position that should be turned on.
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_stat_custom_to_bit(int unit, int chan, bcm_custom_stat_trigger_t trigger, 
                        uint32 *result)
{
    if (chan == _DBG_CNT_RX_CHAN) {

    /*  RX Part */

    switch (trigger) {

    case bcmDbgCntRIPD4:
        _BCM_TD3_BIT_SET(unit, result, 108);
        _BCM_TH3_BIT_SET(unit, result, 64);
        _BCM_DEFAULT_BIT_SET(unit, result, 0);
        break;
    case bcmDbgCntRIPC4:
        _BCM_TD3_BIT_SET(unit, result, 96);
        _BCM_TH3_BIT_SET(unit, result, 65);
        _BCM_DEFAULT_BIT_SET(unit, result, 1);
        break;
    case bcmDbgCntRIPHE4:
        _BCM_TD3_BIT_SET(unit, result, 109);
        _BCM_TH3_BIT_SET(unit, result, 66);
        _BCM_DEFAULT_BIT_SET(unit, result, 2);
        break;
    case bcmDbgCntIMRP4:
        _BCM_TD3_BIT_SET(unit, result, 97);
        _BCM_TH3_BIT_SET(unit, result, 67);
        _BCM_DEFAULT_BIT_SET(unit, result, 3);
        break;
    case bcmDbgCntRIPD6:
        _BCM_TD3_BIT_SET(unit, result, 110);
        _BCM_TH3_BIT_SET(unit, result, 68);
        _BCM_DEFAULT_BIT_SET(unit, result, 4);
        break;
    case bcmDbgCntRIPC6:
        _BCM_TD3_BIT_SET(unit, result, 98);
        _BCM_TH3_BIT_SET(unit, result, 69);
        _BCM_DEFAULT_BIT_SET(unit, result, 5);
        break;
    case bcmDbgCntRIPHE6:
        _BCM_TD3_BIT_SET(unit, result, 111);
        _BCM_TH3_BIT_SET(unit, result, 70);
        _BCM_DEFAULT_BIT_SET(unit, result, 6);
        break;
    case bcmDbgCntIMRP6:
        _BCM_TD3_BIT_SET(unit, result, 99);
        _BCM_TH3_BIT_SET(unit, result, 71);
        _BCM_DEFAULT_BIT_SET(unit, result, 7);
        break;
    case bcmDbgCntRDISC:
        _BCM_TD3_BIT_SET_E_PARAM(unit, result);
        _BCM_TH3_BIT_SET_E_PARAM(unit, result);
        _BCM_DEFAULT_BIT_SET(unit, result, 8);
        break;
    case bcmDbgCntRUC:
        _BCM_TD3_BIT_SET(unit, result, 104);
        _BCM_TH3_BIT_SET(unit, result, 94);
        _BCM_DEFAULT_BIT_SET(unit, result, 9);
        break;
    case bcmDbgCntRPORTD:
        _BCM_TD3_BIT_SET(unit, result, 114);
        _BCM_TH3_BIT_SET(unit, result, 91);
        _BCM_DEFAULT_BIT_SET(unit, result, 10);
        break;
    case bcmDbgCntPDISC:
        _BCM_TH3_BIT_SET(unit, result, 84);
        _BCM_DEFAULT_BIT_SET(unit, result, 11);
        break;
    case bcmDbgCntIMBP:
        _BCM_TD3_BIT_SET(unit, result, 105);
        _BCM_TH3_BIT_SET(unit, result, 79);
        _BCM_DEFAULT_BIT_SET(unit, result, 12);
        break;
    case bcmDbgCntRFILDR:
        _BCM_TD3_BIT_SET(unit, result, 93);
        _BCM_TH3_BIT_SET(unit, result, 35); /* RSEL_DROP_VECTOR.IFP_DROP */
        _BCM_DEFAULT_BIT_SET(unit, result, 13);
        break;
    case bcmDbgCntRIMDR:
        _BCM_TD3_BIT_SET(unit, result, 118); /* MULTICAST_PBM_ZERO */
        _BCM_TH3_BIT_SET(unit, result, 90);
        _BCM_DEFAULT_BIT_SET(unit, result, 14);
        break;
    case bcmDbgCntRDROP:
        _BCM_TD3_BIT_SET(unit, result, 115);
        _BCM_TH3_BIT_SET(unit, result, 85);
        _BCM_DEFAULT_BIT_SET(unit, result, 15);
        break;
    case bcmDbgCntIRPSE:
        _BCM_TD3_BIT_SET(unit, result, 0);
        _BCM_TH3_BIT_SET(unit, result, 0); /* DISCARD_PROC_DROP_VECTOR.CONTROL_FRAME */
        _BCM_DEFAULT_BIT_SET(unit, result, 16);
        break;
    case bcmDbgCntIRHOL:
        _BCM_FBX_BIT_SET(unit, result, 17);
        _BCM_DEFAULT_BIT_SET_E_PARAM(unit, result);
        break;
    case bcmDbgCntIRIBP:
        _BCM_FBX_BIT_SET(unit, result, 18);
        _BCM_DEFAULT_BIT_SET_E_PARAM(unit, result);
        break;
    case bcmDbgCntDSL3HE:
        _BCM_TD3_BIT_SET(unit, result, 1); /* L3_DOS_ATTACK */
        _BCM_TH3_BIT_SET(unit, result, 8);
        _BCM_HB_GW_BIT_SET_E_PARAM(unit, result);
        _BCM_DEFAULT_BIT_SET(unit, result, 19);
        break;
    case bcmDbgCntIUNKHDR:
        _BCM_TD3_BIT_SET(unit, result, 106);
        _BCM_HB_GW_TRX_BIT_SET(unit, result, 19);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntDSL4HE:
        _BCM_TD3_BIT_SET(unit, result, 9); /* L4_DOS_ATTACK */
        _BCM_TH3_BIT_SET(unit, result, 9);
        _BCM_HB_GW_BIT_SET_E_PARAM(unit, result);
        _BCM_DEFAULT_BIT_SET(unit, result, 20);
        break;
    case bcmDbgCntIMIRROR:
        _BCM_TD3_BIT_SET(unit, result, 107);
        _BCM_HB_GW_TRX_BIT_SET(unit, result, 20);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntDSICMP:
        _BCM_TH3_BIT_SET(unit, result, 77); /* DSICMP */
        _BCM_TD3_BIT_SET_E_PARAM(unit, result);
        _BCM_DEFAULT_BIT_SET(unit, result, 21);
        break;
    case bcmDbgCntDSFRAG:
        _BCM_TH3_BIT_SET(unit, result, 76); /* DSFRAG */
        _BCM_TD3_BIT_SET_E_PARAM(unit, result);
        _BCM_DEFAULT_BIT_SET(unit, result, 22);
        break;
    case bcmDbgCntMTUERR:
        _BCM_TD3_BIT_SET(unit, result, 116); /* MTU_CHECK_FAIL */
        _BCM_TH3_BIT_SET(unit, result, 82);
        _BCM_DEFAULT_BIT_SET(unit, result, 23);
        break;
    case bcmDbgCntRTUN:
        _BCM_TD3_BIT_SET(unit, result, 103);
        _BCM_TH3_BIT_SET(unit, result, 92);
        _BCM_DEFAULT_BIT_SET(unit, result, 24);
        break;
    case bcmDbgCntRTUNE:
        _BCM_TH3_BIT_SET(unit, result, 93);
        _BCM_TD3_BIT_SET_E_PARAM(unit, result);
        _BCM_DEFAULT_BIT_SET(unit, result, 25);
        break;
    case bcmDbgCntVLANDR:
        /*
         * Replaced by bcmDbgCntRxVlanMismatch bcmDbgCntRxVlanMemberMismatch
         * bcmDbgCntRxTpidMismatch bcmDbgCntRxPrivateVlanMismatch for TD3 and TH3
         */
        _BCM_TD3_BIT_SET_E_PARAM(unit, result);
        _BCM_TH3_BIT_SET_E_PARAM(unit, result);
        _BCM_DEFAULT_BIT_SET(unit, result, 26);
        break;
    case bcmDbgCntRHGUC:
        _BCM_TD3_BIT_SET(unit, result, 101);
        _BCM_TH3_BIT_SET_E_PARAM(unit, result);
        _BCM_FB_BIT_SET(unit, result, 27);
        _BCM_TRX_BIT_SET(unit, result, 32);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRHGMC:
        _BCM_TD3_BIT_SET(unit, result, 102);
        _BCM_TH3_BIT_SET_E_PARAM(unit, result);
        _BCM_FB_BIT_SET(unit, result, 28);
        _BCM_TRX_BIT_SET(unit, result, 33);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntMPLS:                      
        return BCM_E_PARAM;
    case bcmDbgCntMACLMT:
        _BCM_TRX_BIT_SET_E_PARAM(unit, result);
        if (soc_feature(unit, soc_feature_mac_learn_limit)) {
            *result = 29;   /* Value = 0x20000000 */
            return BCM_E_NONE;
        } else {
            return BCM_E_PARAM;
        }
        break;
    case bcmDbgCntMPLSERR:
        return BCM_E_PARAM;
    case bcmDbgCntURPFERR:
        return BCM_E_PARAM;
    case bcmDbgCntHGHDRE:
        _BCM_TD3_BIT_SET(unit, result, 6);
        _BCM_HB_GW_TRX_BIT_SET(unit, result, 27);
        _BCM_FB2_BIT_SET(unit, result, 29);
        _BCM_RAPTOR_RAVEN_BIT_SET(unit, result, 27);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntMCIDXE:
        _BCM_TD3_BIT_SET(unit, result, 50);
        _BCM_HB_GW_TRX_BIT_SET(unit, result, 28);
        _BCM_RAPTOR_RAVEN_BIT_SET(unit, result, 28);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntLAGLUP:
        _BCM_TD3_BIT_SET(unit, result, 100);
        _BCM_TH3_BIT_SET(unit, result, 80);
        _BCM_HB_GW_TRX_BIT_SET(unit, result, 29);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntLAGLUPD:
        _BCM_TD3_BIT_SET(unit, result, 112);
        _BCM_TH3_BIT_SET(unit, result, 81);
        _BCM_HB_GW_TRX_BIT_SET(unit, result, 30);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntPARITYD:
        _BCM_TD3_BIT_SET(unit, result, 113);
        _BCM_TH3_BIT_SET(unit, result, 83);
        _BCM_HB_GW_TRX_BIT_SET(unit, result, 31);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntVFPDR:
        _BCM_TD3_BIT_SET(unit, result, 41); /* VXLT_DROP_VECTOR.VFP */
        _BCM_TH3_BIT_SET(unit, result, 58); /* PKT_PROC_DROP_VECTOR.VFP */
        _BCM_FB2_BIT_SET(unit, result, 30);
        _BCM_TRX_BIT_SET(unit, result, 35);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntURPF:
        _BCM_TD3_BIT_SET(unit, result, 90); /* URPF_CHECK_FAIL */
        _BCM_TH3_BIT_SET_E_PARAM(unit, result);
        _BCM_FB2_BIT_SET(unit, result, 31);
        _BCM_TRX_BIT_SET(unit, result, 34);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntDSTDISCARDDROP:
        /*
         * Replaced by bcmDbgCntRxL2DstDiscardDrop and
         *  bcmDbgCntRxL3DstDiscardDrop for TD3 and TH3
         */
        _BCM_TH3_BIT_SET_E_PARAM(unit, result);
        _BCM_TD3_BIT_SET_E_PARAM(unit, result);
        _BCM_TRX_BIT_SET(unit, result, 36);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntCLASSBASEDMOVEDROP:
        if (soc_feature(unit, soc_feature_class_based_learning)) {
            _BCM_TD3_BIT_SET(unit, result, 62); /* CLASS_BASED_SM */
            _BCM_TRX_BIT_SET(unit, result, 37);
        }
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntMACLMT_NODROP:
        _BCM_TRX_BIT_SET(unit, result, 38);
        _BCM_TH3_BIT_SET_E_PARAM(unit, result);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntMACSAEQUALMACDA:
        _BCM_TD3_BIT_SET(unit, result, 8); /* MACSA_EQUALS_DA */
        _BCM_TH3_BIT_SET(unit, result, 11);
        _BCM_TRX_BIT_SET(unit, result, 39);
        _BCM_RAVEN_BIT_SET(unit, result, 30);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
     case bcmDbgCntMACLMT_DROP:
        _BCM_TH3_BIT_SET_E_PARAM(unit, result);
        _BCM_TRX_BIT_SET(unit, result, 40);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntVntagRxError:
        _BCM_GH_BIT_SET(unit, result, 41);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntNivRxForwardingError:
        _BCM_GH_BIT_SET(unit, result, 42);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntIfpTriggerPfcRxDrop:
        _BCM_HR3_BIT_SET(unit, result, 43);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntFcmPortClass2RxDiscards:
        _BCM_TD2_BIT_SET(unit, result, 56);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntFcmPortClass2RxFrames:
        _BCM_TD3_BIT_SET(unit, result, 123); /* FCOE_CLASS2_RX_FRAMES */
        _BCM_TD2_BIT_SET(unit, result, 57);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntFcmPortClass3RxDiscards:
        _BCM_TD2_BIT_SET(unit, result, 58);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntFcmPortClass3RxFrames:
        _BCM_TD3_BIT_SET(unit, result, 124); /* FCOE_CLASS3_RX_FRAMES */
        _BCM_TD2_BIT_SET(unit, result, 59);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntNonHGoE:
        _BCM_TD2_BIT_SET(unit, result, 60);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntHGoE:
        _BCM_TD2_BIT_SET(unit, result, 61);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxHigigLoopbackDrop:
        _BCM_TD3_BIT_SET(unit, result, 2);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxHigigUnknownOpcodeDrop:
        _BCM_TD3_BIT_SET(unit, result, 5);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxIpInIpDrop:
        _BCM_TD3_BIT_SET(unit, result, 16);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxMimDrop:
        _BCM_TD3_BIT_SET(unit, result, 17);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxTunnelInterfaceError:
        _BCM_TD3_BIT_SET(unit, result, 18);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxMplsControlWordInvalid:
        _BCM_TD3_BIT_SET(unit, result, 20);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxMplsGalNotBosDrop:
        _BCM_TD3_BIT_SET(unit, result, 21);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxMplsPayloadInvalid:
        _BCM_TD3_BIT_SET(unit, result, 22);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxMplsEntropyDrop:
        _BCM_TD3_BIT_SET(unit, result, 23);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxMplsLabelLookupMiss:
        _BCM_TD3_BIT_SET(unit, result, 24);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxMplsActionInvalid:
        _BCM_TD3_BIT_SET(unit, result, 25);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxTunnelTtlError:
        _BCM_TD3_BIT_SET(unit, result, 28);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxTunnelShimError:
        _BCM_TD3_BIT_SET(unit, result, 29);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxTunnelObjectValidationFail:
        _BCM_TD3_BIT_SET(unit, result, 30);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxVlanMismatch:
        _BCM_TH3_BIT_SET(unit, result, 62); /* INVALID_VLAN */
        _BCM_TD3_BIT_SET(unit, result, 32);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxVlanMemberMismatch:
        _BCM_TH3_BIT_SET(unit, result, 60); /* ENIFILTER */
        _BCM_TD3_BIT_SET(unit, result, 33);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxTpidMismatch:
        _BCM_TH3_BIT_SET(unit, result, 61); /* INVALID_TPID */
        _BCM_TD3_BIT_SET(unit, result, 34);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxPrivateVlanMismatch:
        _BCM_TH3_BIT_SET(unit, result, 63); /* PVLAN_VID_MISMATCH */
        _BCM_TD3_BIT_SET(unit, result, 35);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxMacIpBindError:
        _BCM_TD3_BIT_SET(unit, result, 36);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxVlanTranslate2LookupMiss:
        _BCM_TD3_BIT_SET(unit, result, 37);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxL3TunnelLookupMiss:
        _BCM_TD3_BIT_SET(unit, result, 38);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxMplsLookupMiss:
        _BCM_TD3_BIT_SET(unit, result, 39);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxVlanTranslate1LookupMiss:
        _BCM_TD3_BIT_SET(unit, result, 40);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxFcoeVftDrop:
        _BCM_TD3_BIT_SET(unit, result, 42);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxFcoeSrcBindError:
        _BCM_TD3_BIT_SET(unit, result, 43);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxFcoeSrcFpmaError:
        _BCM_TD3_BIT_SET(unit, result, 44);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxVfiP2pDrop:
        _BCM_TD3_BIT_SET(unit, result, 45);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxStgDrop:
        _BCM_TD3_BIT_SET(unit, result, 49);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxCompositeError:
        _BCM_TD3_BIT_SET(unit, result, 51);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxBpduDrop:
        _BCM_TD3_BIT_SET(unit, result, 52);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxProtocolDrop:
        _BCM_TD3_BIT_SET(unit, result, 53);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxUnknownMacSaDrop:
        _BCM_TD3_BIT_SET(unit, result, 54);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxSrcRouteDrop:
        _BCM_TD3_BIT_SET(unit, result, 55);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxL2SrcDiscardDrop:
        _BCM_TD3_BIT_SET(unit, result, 56);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxL2SrcStaticMoveDrop:
        _BCM_TD3_BIT_SET(unit, result, 57);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxL2DstDiscardDrop:
        _BCM_TH3_BIT_SET(unit, result, 14); /* L2DST_DISCARD */
        _BCM_TD3_BIT_SET(unit, result, 58);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxL3DisableDrop:
        _BCM_TD3_BIT_SET(unit, result, 59);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxMacSaEqual0Drop:
        _BCM_TD3_BIT_SET(unit, result, 60);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxVlanCrossConnectOrNonUnicastDrop:
        _BCM_TD3_BIT_SET(unit, result, 61);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxTimeSyncDrop:
        _BCM_TD3_BIT_SET(unit, result, 63);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxIpmcDrop:
        _BCM_TD3_BIT_SET(unit, result, 64);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxMyStationDrop:
        _BCM_TD3_BIT_SET(unit, result, 65);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxPrivateVlanVpFilterDrop:
        _BCM_TD3_BIT_SET(unit, result, 66);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxL3DstDiscardDrop:
        _BCM_TH3_BIT_SET(unit, result, 17); /* LDST_DISCARD */
        _BCM_TD3_BIT_SET(unit, result, 68);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxTunnelDecapEcnError:
        _BCM_TD3_BIT_SET(unit, result, 69);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxL3SrcDiscardDrop:
        _BCM_TD3_BIT_SET(unit, result, 71);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxFcoeZoneLookupMiss:
        _BCM_TD3_BIT_SET(unit, result, 72);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxL3TtlError:
        _BCM_TD3_BIT_SET(unit, result, 73);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxL3HeaderError:
        _BCM_TD3_BIT_SET(unit, result, 74);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxL2HeaderError:
        _BCM_TD3_BIT_SET(unit, result, 75);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxL3DstLookupDrop:
        _BCM_TD3_BIT_SET(unit, result, 77);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxL2TtlError:
        _BCM_TD3_BIT_SET(unit, result, 78);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxL2RpfError:
        _BCM_TD3_BIT_SET(unit, result, 79);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxTagUntagDiscardDrop:
        _BCM_TD3_BIT_SET(unit, result, 82);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxStormControlDrop:
        _BCM_TD3_BIT_SET(unit, result, 84);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxFcoeZoneError:
        _BCM_TD3_BIT_SET(unit, result, 85);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxFieldChangeL2NoRedirectDrop:
        _BCM_TD3_BIT_SET(unit, result, 86);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxNextHopDrop:
        _BCM_TD3_BIT_SET(unit, result, 89);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxNatDrop:
        _BCM_TD3_BIT_SET(unit, result, 91);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntIngressProtectionDataDrop:
        _BCM_TD3_BIT_SET(unit, result, 92);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxSrcKnockoutDrop:
        _BCM_TD3_BIT_SET(unit, result, 119);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxMplsSeqNumberError:
        _BCM_TD3_BIT_SET(unit, result, 120);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxBlockMaskDrop:
        _BCM_TD3_BIT_SET(unit, result, 121);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntRxDlbRhResolutionError:
        _BCM_TD3_BIT_SET(unit, result, 122);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;

    default:
        return BCM_E_PARAM;
    }
 
   return BCM_E_NONE;

   } else if  (chan == _DBG_CNT_TX_CHAN) {
        
    /* TX Part */

    switch (trigger) {

    case bcmDbgCntTGIP4:
        _BCM_TD3_BIT_SET(unit, result, 40);
        _BCM_DEFAULT_BIT_SET(unit, result, 0);
        break;
    case bcmDbgCntTIPD4:
        _BCM_TD3_BIT_SET(unit, result, 35);
        _BCM_DEFAULT_BIT_SET(unit, result, 1);
        break;
    case bcmDbgCntTGIPMC4:
        _BCM_TD3_BIT_SET(unit, result, 41);
        _BCM_DEFAULT_BIT_SET(unit, result, 2);
        break;
    case bcmDbgCntTIPMCD4:
        _BCM_TD3_BIT_SET(unit, result, 36);
        _BCM_DEFAULT_BIT_SET(unit, result, 3);
        break;
    case bcmDbgCntTGIP6:
        _BCM_TD3_BIT_SET(unit, result, 44);
        _BCM_DEFAULT_BIT_SET(unit, result, 4);
        break;
    case bcmDbgCntTIPD6:
        _BCM_TD3_BIT_SET(unit, result, 37);
        _BCM_DEFAULT_BIT_SET(unit, result, 5);
        break;
    case bcmDbgCntTGIPMC6:
        _BCM_TD3_BIT_SET(unit, result, 45);
        _BCM_DEFAULT_BIT_SET(unit, result, 6);
        break;
    case bcmDbgCntTIPMCD6:
        _BCM_TD3_BIT_SET(unit, result, 38);
        _BCM_DEFAULT_BIT_SET(unit, result, 7);
        break;
    case bcmDbgCntTTNL:
        _BCM_TD3_BIT_SET(unit, result, 46);
        _BCM_DEFAULT_BIT_SET(unit, result, 8);
        break;
    case bcmDbgCntTTNLE:
        _BCM_TD3_BIT_SET(unit, result, 28);
        _BCM_DEFAULT_BIT_SET(unit, result, 9);
        break;
    case bcmDbgCntTTTLD:
        _BCM_TD3_BIT_SET(unit, result, 2);
        _BCM_DEFAULT_BIT_SET(unit, result, 10);
        break;
    case bcmDbgCntTCFID:
        _BCM_TD3_BIT_SET(unit, result, 30);
        _BCM_DEFAULT_BIT_SET(unit, result, 11);
        break;
    case bcmDbgCntTVLAN:
        _BCM_TD3_BIT_SET(unit, result, 42);
        _BCM_DEFAULT_BIT_SET(unit, result, 12);
        break;
    case bcmDbgCntTVLAND:
        /*
         * Replaced by bcmDbgCntTxFwdDomainNotFoundDrop for TD3
         */
        _BCM_TD3_BIT_SET_E_PARAM(unit, result);
        _BCM_DEFAULT_BIT_SET(unit, result, 13);
        break;
    case bcmDbgCntTVXLTMD:
        _BCM_TD3_BIT_SET(unit, result, 0); /* ADAPT_1_MISS_DROP */
        _BCM_DEFAULT_BIT_SET(unit, result, 14);
        break;
    case bcmDbgCntTSTGD:
        _BCM_TD3_BIT_SET(unit, result, 5); /* STG_BLOCK_DROP */
        _BCM_DEFAULT_BIT_SET(unit, result, 15);
        break;
    case bcmDbgCntTAGED:
        _BCM_TD3_BIT_SET(unit, result, 14);
        _BCM_TH3_BIT_SET_E_PARAM(unit, result);
        _BCM_DEFAULT_BIT_SET(unit, result, 16);
        break;
    case bcmDbgCntTL2MCD:
        _BCM_TD3_BIT_SET(unit, result, 39);
        _BCM_DEFAULT_BIT_SET(unit, result, 17);
        break;
    case bcmDbgCntTPKTD:
        /*
         * Although exact HW field absent in TD3 but functionality
         * achieved by mapping it to TDBGC3 reg.
         */
        _BCM_DEFAULT_BIT_SET(unit, result, 18);
        break;
    case bcmDbgCntTMIRR:
        _BCM_TD3_BIT_SET(unit, result, 47);
        _BCM_DEFAULT_BIT_SET(unit, result, 19);
        break;
    case bcmDbgCntTSIPL:
        _BCM_TH3_BIT_SET(unit, result, 20);
        _BCM_FB_HB_GW_TRX_BIT_SET(unit, result, 20);
        _BCM_RAVEN_BIT_SET(unit, result, 20);
        _BCM_DEFAULT_BIT_SET_E_PARAM(unit, result);
        break;
    case bcmDbgCntTHGUC:
        _BCM_TH3_BIT_SET(unit, result, 21);
        _BCM_TD3_BIT_SET(unit, result, 48);
        _BCM_FB_HB_GW_TRX_BIT_SET(unit, result, 21);
        _BCM_DEFAULT_BIT_SET_E_PARAM(unit, result);
        break;
    case bcmDbgCntTHGMC:
        _BCM_TH3_BIT_SET(unit, result, 22);
        _BCM_TD3_BIT_SET(unit, result, 49);
        _BCM_FB_HB_GW_TRX_BIT_SET(unit, result, 22);
        _BCM_DEFAULT_BIT_SET_E_PARAM(unit, result);
        break;
    case bcmDbgCntTHIGIG2:
        _BCM_TH3_BIT_SET(unit, result, 23);
        _BCM_TD3_BIT_SET(unit, result, 27);
        _BCM_HB_GW_TRX_BIT_SET(unit, result, 23);
        _BCM_DEFAULT_BIT_SET_E_PARAM(unit, result);
        break;
    case bcmDbgCntTHGI:
        _BCM_TH3_BIT_SET(unit, result, 24);
        _BCM_TD3_BIT_SET(unit, result, 26);
        _BCM_HB_GW_TRX_BIT_SET(unit, result, 24);
        _BCM_DEFAULT_BIT_SET_E_PARAM(unit, result);
        break;
    case bcmDbgCntTL2_MTU:
        _BCM_TH3_BIT_SET(unit, result, 25);
        _BCM_TD3_BIT_SET(unit, result, 15);
        _BCM_HB_GW_TRX_BIT_SET(unit, result, 25);
        _BCM_DEFAULT_BIT_SET_E_PARAM(unit, result);
        break;
    case bcmDbgCntTPARITY_ERR:
        _BCM_TH3_BIT_SET(unit, result, 26);
        _BCM_TD3_BIT_SET(unit, result, 16);
        _BCM_HB_GW_TRX_BIT_SET(unit, result, 26);
        _BCM_DEFAULT_BIT_SET_E_PARAM(unit, result);
        break;
    case bcmDbgCntTIP_LEN_FAIL:
        _BCM_TH3_BIT_SET(unit, result, 27);
        _BCM_HB_GW_TRX_BIT_SET(unit, result, 27);
        _BCM_DEFAULT_BIT_SET_E_PARAM(unit, result);
        break;
    case bcmDbgCntTMTUD:
        _BCM_DEFAULT_BIT_SET_E_PARAM(unit, result);
        break;
    case bcmDbgCntTSLLD:
        _BCM_TD3_BIT_SET(unit, result, 29);
        _BCM_DEFAULT_BIT_SET_E_PARAM(unit, result);
        break;
    case bcmDbgCntTL2MPLS:
        _BCM_DEFAULT_BIT_SET_E_PARAM(unit, result);
        break;
    case bcmDbgCntTL3MPLS:
        _BCM_DEFAULT_BIT_SET_E_PARAM(unit, result);
        break;
    case bcmDbgCntTMPLS:
        _BCM_DEFAULT_BIT_SET_E_PARAM(unit, result);
        break;
    case bcmDbgCntTMODIDTOOLARGEDROP:
        _BCM_TH3_BIT_SET(unit, result, 28); /* TMODID_GRT_31 */
        _BCM_TRX_BIT_SET(unit, result, 28);
        _BCM_DEFAULT_BIT_SET_E_PARAM(unit, result);
        break;
    case bcmDbgCntPKTMODTOOLARGEDROP:
        _BCM_TD3_BIT_SET(unit, result, 19);
        _BCM_TH3_BIT_SET(unit, result, 29); /* TEP_ADDITIONS_TOO_LARGE */
        _BCM_TRX_BIT_SET(unit, result, 29);
        _BCM_DEFAULT_BIT_SET_E_PARAM(unit, result);
        break;
    case bcmDbgCntEcnTxError:
        _BCM_GH_BIT_SET(unit, result, 30);
        _BCM_DEFAULT_BIT_SET_E_PARAM(unit, result);
        break;
    case bcmDbgCntNivTxPrune:
        _BCM_GH_BIT_SET(unit, result, 31);
        _BCM_DEFAULT_BIT_SET_E_PARAM(unit, result);
        break;
    case bcmDbgCntFcmPortClass2TxFrames:
        _BCM_TD3_BIT_SET(unit, result, 50);
        _BCM_TD2_BIT_SET(unit, result, 31);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntFcmPortClass3TxFrames:
        _BCM_TD3_BIT_SET(unit, result, 51);
        _BCM_TD2_BIT_SET(unit, result, 32);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntEgressProtectionDataDrop:
        _BCM_TD3_BIT_SET(unit, result, 32);
        _BCM_TD2_BIT_SET(unit, result, 37);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntTxFwdDomainNotFoundDrop:
        _BCM_TD3_BIT_SET(unit, result, 3);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntTxNotFwdDomainMemberDrop:
        _BCM_TD3_BIT_SET(unit, result, 4);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntTxIpmcL2SrcPortPruneDrop:
        _BCM_TD3_BIT_SET(unit, result, 6);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntTxNonUnicastPruneDrop:
        _BCM_TD3_BIT_SET(unit, result, 7);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntTxSvpRemoveDrop:
        _BCM_TD3_BIT_SET(unit, result, 8);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntTxVplagPruneDrop:
        _BCM_TD3_BIT_SET(unit, result, 9);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntTxSplitHorizonDrop:
        _BCM_TD3_BIT_SET(unit, result, 10);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntTxMmuPurgeDrop:
        _BCM_TD3_BIT_SET(unit, result, 12);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntTxStgDisableDrop:
        _BCM_TD3_BIT_SET(unit, result, 13);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntTxEgressPortDrop:
        _BCM_TD3_BIT_SET(unit, result, 17);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntTxEgressFilterDrop:
        _BCM_TD3_BIT_SET(unit, result, 18);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;
    case bcmDbgCntTxVisibilityDrop:
        _BCM_TD3_BIT_SET(unit, result, 23);
        _BCM_DEFAULT_BIT_SET_E_PARAM (unit, result);
        break;

    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;

    }
    return BCM_E_PARAM;
}

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
STATIC int
_bcm_td3_stat_custom_change_update(int unit, int sel_idx,
                        bcm_custom_stat_trigger_t trigger, int value)
{
    uint32      ctr_sel, sel_bmp, ctrl_bmp[2] = {0};

    /* Defined in TDBGC_TRIGGERS, EP_EGR_DROP_VECTOR, EGR_STATS_FORMAT */
    if (trigger == bcmDbgCntTPKTD) {
        ctrl_bmp[0] = 0xffffffff;
        ctrl_bmp[1] = 0xff;
    }

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, bcm_dbg_cntr_tx[sel_idx].select,
        REG_PORT_ANY, 0, &ctr_sel));
    sel_bmp = soc_reg_field_get(unit, bcm_dbg_cntr_tx[sel_idx].select,
        ctr_sel, BITMAPf);

    sel_bmp = (value) ? (sel_bmp | ctrl_bmp[0]) :
        (sel_bmp & (~ctrl_bmp[0]));

    soc_reg_field_set(unit, bcm_dbg_cntr_tx[sel_idx].select,
        &ctr_sel, BITMAPf, sel_bmp);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, bcm_dbg_cntr_tx[sel_idx].select,
        REG_PORT_ANY, 0, ctr_sel));

    SOC_IF_ERROR_RETURN(READ_TDBGC_SELECT_2r(unit, sel_idx, &ctr_sel));
    sel_bmp = soc_reg_field_get(unit, TDBGC_SELECT_2r,
        ctr_sel, BITMAPf);

    sel_bmp = (value) ? (sel_bmp | ctrl_bmp[1]) :
        (sel_bmp & (~ctrl_bmp[1]));

    soc_reg_field_set(unit, TDBGC_SELECT_2r,
        &ctr_sel, BITMAPf, sel_bmp);
    return (WRITE_TDBGC_SELECT_2r(unit, sel_idx, ctr_sel));

}

STATIC int
_bcm_td3_stat_custom_change(int unit, bcm_stat_val_t type,
                        bcm_custom_stat_trigger_t trigger, int value)
{
    uint32      ctr_sel, ctrl_bmp, result;
    int         i;

    for (i = 0; i < COUNTOF(bcm_dbg_cntr_rx); i++) {
        if (bcm_dbg_cntr_rx[i].counter == type) {

           if (trigger == bcmDbgCntRIMDR) {
               /* Unknown Multicast is counted in PFM counter */
               SOC_IF_ERROR_RETURN(READ_RDBGC_SELECT_3r(unit, i, &ctr_sel));
               ctrl_bmp = soc_reg_field_get(unit, RDBGC_SELECT_3r,
                                            ctr_sel, BITMAPf);
               ctrl_bmp = (value) ? (ctrl_bmp | (1 << 16)) :    /* add */
                   (ctrl_bmp & (~(1 << 16)));  /* delete */

               soc_reg_field_set(unit, RDBGC_SELECT_3r,
                       &ctr_sel, BITMAPf, ctrl_bmp);
               SOC_IF_ERROR_RETURN(WRITE_RDBGC_SELECT_3r(unit, i, ctr_sel));
           }

            SOC_IF_ERROR_RETURN(_bcm_stat_custom_to_bit
                                (unit, _DBG_CNT_RX_CHAN, trigger, &result));

           if (result < 32) {
               SOC_IF_ERROR_RETURN(soc_reg32_get(unit, bcm_dbg_cntr_rx[i].select,
                                                 REG_PORT_ANY, 0, &ctr_sel));

               ctrl_bmp = soc_reg_field_get(unit, bcm_dbg_cntr_rx[i].select,
                                            ctr_sel, BITMAPf);
               ctrl_bmp = (value) ? (ctrl_bmp | (1 << result)) :    /* add */
                   (ctrl_bmp & (~(1 << result)));  /* delete */

               soc_reg_field_set(unit, bcm_dbg_cntr_rx[i].select,
                                 &ctr_sel, BITMAPf, ctrl_bmp);
               return soc_reg32_set(unit, bcm_dbg_cntr_rx[i].select,
                                    REG_PORT_ANY, 0, ctr_sel);
           } else if (result < 64) {
               SOC_IF_ERROR_RETURN(READ_RDBGC_SELECT_2r(unit, i, &ctr_sel));
               ctrl_bmp = soc_reg_field_get(unit, RDBGC_SELECT_2r,
                                            ctr_sel, BITMAPf);
               result -= 32;
               ctrl_bmp = (value) ? (ctrl_bmp | (1 << result)) :    /* add */
                   (ctrl_bmp & (~(1 << result)));  /* delete */

               soc_reg_field_set(unit, RDBGC_SELECT_2r,
                       &ctr_sel, BITMAPf, ctrl_bmp);
               return (WRITE_RDBGC_SELECT_2r(unit, i, ctr_sel));
           }  else if (result < 96) {
               SOC_IF_ERROR_RETURN(READ_RDBGC_SELECT_3r(unit, i, &ctr_sel));
               ctrl_bmp = soc_reg_field_get(unit, RDBGC_SELECT_3r,
                                            ctr_sel, BITMAPf);
               result -= 64;
               ctrl_bmp = (value) ? (ctrl_bmp | (1 << result)) :    /* add */
                   (ctrl_bmp & (~(1 << result)));  /* delete */

               soc_reg_field_set(unit, RDBGC_SELECT_3r,
                       &ctr_sel, BITMAPf, ctrl_bmp);
               return (WRITE_RDBGC_SELECT_3r(unit, i, ctr_sel));
           }  else if (result < 128) {
               SOC_IF_ERROR_RETURN(READ_RDBGC_SELECT_4r(unit, i, &ctr_sel));
               ctrl_bmp = soc_reg_field_get(unit, RDBGC_SELECT_4r,
                                            ctr_sel, BITMAPf);
               result -= 96;
               ctrl_bmp = (value) ? (ctrl_bmp | (1 << result)) :    /* add */
                   (ctrl_bmp & (~(1 << result)));  /* delete */

               soc_reg_field_set(unit, RDBGC_SELECT_4r,
                       &ctr_sel, BITMAPf, ctrl_bmp);
               return (WRITE_RDBGC_SELECT_4r(unit, i, ctr_sel));
           } else {
               return BCM_E_PARAM;
           }
        }
    }

    for (i = 0; i < COUNTOF(bcm_dbg_cntr_tx); i++) {
        if (bcm_dbg_cntr_tx[i].counter == type) {
            SOC_IF_ERROR_RETURN(_bcm_stat_custom_to_bit(unit,
            _DBG_CNT_TX_CHAN, trigger, &result));

            if (SOC_IS_TRIDENT3X(unit) && trigger == bcmDbgCntTPKTD) {
                return  _bcm_td3_stat_custom_change_update(unit, i, trigger, value);
            }

            if (result < 32) {
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, bcm_dbg_cntr_tx[i].select,
                                                  REG_PORT_ANY, 0, &ctr_sel));
                ctrl_bmp = soc_reg_field_get(unit, bcm_dbg_cntr_tx[i].select,
                                             ctr_sel, BITMAPf);

                ctrl_bmp = (value) ? (ctrl_bmp | (1 << result)) :    /* add */
                    (ctrl_bmp & (~(1 << result)));  /* delete */

                if(SOC_IS_TRIDENT3X(unit)) {
                    /* Special handlings for TD3 */
                    if (trigger == bcmDbgCntTSTGD) {
                    /* STG drop is divided into two part, original bit 5 only cover STG block drop,
                       bit 13 cover STP disable drop */
                        ctrl_bmp = (value) ? (ctrl_bmp | (1 << 13)) :  /* add */
                             (ctrl_bmp & (~(1 << 13)));  /* delete */
                    }
                    if (trigger == bcmDbgCntTVXLTMD) {
                        /*
                         * bcmDbgCntTVXLTMD covers EgressVlanTranslate1 lookup
                         *  miss drop and EgressVlanTranslate2 lookup miss drop
                         */
                        ctrl_bmp = (value) ? (ctrl_bmp | (1 << 1)) :  /* add */
                             (ctrl_bmp & (~(1 << 1)));  /* delete */

                    }
                }
                soc_reg_field_set(unit, bcm_dbg_cntr_tx[i].select,
                                  &ctr_sel, BITMAPf, ctrl_bmp);
                return soc_reg32_set(unit, bcm_dbg_cntr_tx[i].select,
                                     REG_PORT_ANY, 0, ctr_sel);
            } else if (result < 64) {
                 SOC_IF_ERROR_RETURN(READ_TDBGC_SELECT_2r(unit, i, &ctr_sel));
                ctrl_bmp = soc_reg_field_get(unit, TDBGC_SELECT_2r,
                                             ctr_sel, BITMAPf);
                result -= 32;
                ctrl_bmp = (value) ? (ctrl_bmp | (1 << result)) :    /* add */
                    (ctrl_bmp & (~(1 << result)));  /* delete */

                soc_reg_field_set(unit, TDBGC_SELECT_2r,
                                  &ctr_sel, BITMAPf, ctrl_bmp);
                return (WRITE_TDBGC_SELECT_2r(unit, i, ctr_sel));
            } else {
                return BCM_E_PARAM;
            }
        }
    }

    return BCM_E_PARAM;
}

STATIC int
_bcm_td3_stat_custom_check(int unit, bcm_stat_val_t type,
                      bcm_custom_stat_trigger_t trigger, int *result)
{
   uint32       ctr_sel, res;
   int          i;

   for (i = 0; i < COUNTOF(bcm_dbg_cntr_rx); i++) {
       if (bcm_dbg_cntr_rx[i].counter == type) {
           SOC_IF_ERROR_RETURN(
               _bcm_stat_custom_to_bit(unit, _DBG_CNT_RX_CHAN, trigger, &res));

           if (res < 32) {
               SOC_IF_ERROR_RETURN(soc_reg32_get(unit, bcm_dbg_cntr_rx[i].select,
                                                 REG_PORT_ANY, 0, &ctr_sel));
               *result = ((ctr_sel & (1 << res)) != 0);
               return BCM_E_NONE;
           } else if (res < 64) {
               SOC_IF_ERROR_RETURN(READ_RDBGC_SELECT_2r(unit, i, &ctr_sel));
               res -= 32;
               *result = ((ctr_sel & (1 << res)) != 0);
               return BCM_E_NONE;
           }  else if (res < 96) {
               SOC_IF_ERROR_RETURN(READ_RDBGC_SELECT_3r(unit, i, &ctr_sel));
               res -= 64;
               *result = ((ctr_sel & (1 << res)) != 0);
               return BCM_E_NONE;
           }  else if (res < 128) {
               SOC_IF_ERROR_RETURN(READ_RDBGC_SELECT_4r(unit, i, &ctr_sel));
               res -= 96;
               *result = ((ctr_sel & (1 << res)) != 0);
               return BCM_E_NONE;
           } else {
               return BCM_E_PARAM;
           }
       }
   }

   for (i = 0; i < COUNTOF(bcm_dbg_cntr_tx); i++) {
        if (bcm_dbg_cntr_tx[i].counter == type) {
           SOC_IF_ERROR_RETURN(
               _bcm_stat_custom_to_bit(unit,  _DBG_CNT_TX_CHAN, trigger, &res));
            if (res < 32) {
               SOC_IF_ERROR_RETURN(soc_reg32_get(unit, bcm_dbg_cntr_tx[i].select,
                                                 REG_PORT_ANY, 0, &ctr_sel));
               *result = ((ctr_sel & (1 << res)) != 0);
               return BCM_E_NONE;
            }
            else if (res < 64) {
               SOC_IF_ERROR_RETURN(READ_TDBGC_SELECT_2r(unit, i, &ctr_sel));
               res -= 32;
               *result = ((ctr_sel & (1 << res)) != 0);
               return BCM_E_NONE;
            }
            else {
               return BCM_E_PARAM;
            }
       }
   }
   return BCM_E_PARAM;
}
#endif /* BCM_TRIDENT3_SUPPORT || BCM_TOMAHAWK3_SUPPORT */

/*
 * Function:
 *      _bcm_stat_custom_change
 * Description:
 *      Add a certain packet type to debug counter to count 
 * Parameters:
 *      unit  - StrataSwitch PCI device unit number.
 *      port  - Port number (unused).
 *      type  - SNMP statistics type.
 *      trigger - The counter select value
 *		      (see stat.h for bit definitions).
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_stat_custom_change(int unit, bcm_port_t port, bcm_stat_val_t type,
                        bcm_custom_stat_trigger_t trigger, int value)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
   uint32	ctr_sel, ctrl_bmp, result, mask;
   int		i;
#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
   uint64	ctr_sel_64, ctrl_bmp_64;
   int rdbgc_sel2_64 = SOC_REG_IS_VALID(unit, RDBGC_SELECT_2_64r);
#endif /* BCM_TRIUMPH_SUPPORT || BCM_SCORPION_SUPPORT */

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
   if (SOC_IS_TRIDENT3X(unit) || SOC_IS_TOMAHAWK3(unit)) {
      return(_bcm_td3_stat_custom_change(unit, type, trigger, value));
   }
#endif /* BCM_TRIDENT3_SUPPORT || BCM_TOMAHAWK3_SUPPORT */

   for (i = 0; i < COUNTOF(bcm_dbg_cntr_rx); i++) {
       if (bcm_dbg_cntr_rx[i].counter == type) {
           SOC_IF_ERROR_RETURN(
               _bcm_stat_custom_to_bit(unit, _DBG_CNT_RX_CHAN, trigger, &result));
              
           if (result < 32) {
               SOC_IF_ERROR_RETURN(soc_reg32_get(unit, bcm_dbg_cntr_rx[i].select,
                                                 REG_PORT_ANY, 0, &ctr_sel));

               ctrl_bmp = soc_reg_field_get(unit, bcm_dbg_cntr_rx[i].select, 
                                            ctr_sel, BITMAPf);
               ctrl_bmp = (value) ? (ctrl_bmp | (1 << result)) :    /* add */
                   (ctrl_bmp & (~(1 << result)));  /* delete */

               soc_reg_field_set(unit, bcm_dbg_cntr_rx[i].select, 
                                 &ctr_sel, BITMAPf, ctrl_bmp);
               return soc_reg32_set(unit, bcm_dbg_cntr_rx[i].select,
                                    REG_PORT_ANY, 0, ctr_sel);
           }
#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
           else if (SOC_IS_TRX(unit) && (result < 65)) {
               /*
                * result < 65 check is required because the only device (Apache)
                * having RDBGC_SELECT_2_64 register has its width as 33 bits
                * and not 64 bits (in which case the check should be (result<96)
                */
               if (rdbgc_sel2_64) {
                   uint64 tmp;
                   SOC_IF_ERROR_RETURN(READ_RDBGC_SELECT_2_64r(unit, i, 
                               &ctr_sel_64));
                   ctrl_bmp_64 = soc_reg64_field_get(unit, RDBGC_SELECT_2_64r,
                           ctr_sel_64, BITMAPf);
                   result -= 32;
                   if (value) {      /* add */
                       COMPILER_64_SET(tmp, 0, 1);
                       COMPILER_64_SHL(tmp, result);
                       COMPILER_64_OR(ctrl_bmp_64, tmp);
                   } else {          /* delete */
                       COMPILER_64_SET(tmp, 0, 1);
                       COMPILER_64_SHL(tmp, result);
                       COMPILER_64_NOT(tmp);
                       COMPILER_64_AND(ctrl_bmp_64, tmp);
                   }

                   soc_reg64_field_set(unit, RDBGC_SELECT_2_64r,
                           &ctr_sel_64, BITMAPf, ctrl_bmp_64);
                   return (WRITE_RDBGC_SELECT_2_64r(unit, i, ctr_sel_64));
               } else if (result < 64) {
                   SOC_IF_ERROR_RETURN(READ_RDBGC_SELECT_2r(unit, i, &ctr_sel));
                   ctrl_bmp = soc_reg_field_get(unit, RDBGC_SELECT_2r,
                           ctr_sel, BITMAPf);
                   result -= 32;
                   ctrl_bmp = (value) ? (ctrl_bmp | (1 << result)) :    /* add */
                       (ctrl_bmp & (~(1 << result)));  /* delete */

                   soc_reg_field_set(unit, RDBGC_SELECT_2r,
                           &ctr_sel, BITMAPf, ctrl_bmp);
                   return (WRITE_RDBGC_SELECT_2r(unit, i, ctr_sel));
               }
           }
#endif /* BCM_TRIUMPH_SUPPORT || BCM_SCORPION_SUPPORT */
           else {
               return BCM_E_PARAM;
           }
       }
   }

   for (i = 0; i < COUNTOF(bcm_dbg_cntr_tx); i++) {
       if (bcm_dbg_cntr_tx[i].counter == type) {
           SOC_IF_ERROR_RETURN(
               _bcm_stat_custom_to_bit(unit, _DBG_CNT_TX_CHAN, trigger, &result));
           
           if (result < 32) {
               SOC_IF_ERROR_RETURN(soc_reg32_get(unit, bcm_dbg_cntr_tx[i].select,
                                                 REG_PORT_ANY, 0, &ctr_sel));
               ctrl_bmp = soc_reg_field_get(unit, bcm_dbg_cntr_tx[i].select, 
                                            ctr_sel, BITMAPf);

               if (value) { /* called from stat_custom_add */
                   mask = (1 << result);
                   ctrl_bmp |= mask;
               } else {     /* called from stat_custom_delete */
                   mask = ~(1 << result);
                   ctrl_bmp &= mask;
               }
               soc_reg_field_set(unit, bcm_dbg_cntr_tx[i].select, 
                                 &ctr_sel, BITMAPf, ctrl_bmp);
               return soc_reg32_set(unit, bcm_dbg_cntr_tx[i].select,
                                    REG_PORT_ANY, 0, ctr_sel);
           }
#if defined(BCM_TRIDENT2_SUPPORT)
           else if (SOC_IS_TD2_TT2(unit) && result < 64) {
               SOC_IF_ERROR_RETURN(READ_TDBGC_SELECT_2r(unit, i, &ctr_sel));
               ctrl_bmp = soc_reg_field_get(unit, TDBGC_SELECT_2r,
                                            ctr_sel, BITMAPf);
               result -= 32;
               ctrl_bmp = (value) ? (ctrl_bmp | (1 << result)) :    /* add */
                   (ctrl_bmp & (~(1 << result)));  /* delete */

               soc_reg_field_set(unit, TDBGC_SELECT_2r,
                                 &ctr_sel, BITMAPf, ctrl_bmp);
               return (WRITE_TDBGC_SELECT_2r(unit, i, ctr_sel));
           }
#endif /* BCM_TRIDENT2_SUPPORT */
           else {
               return BCM_E_PARAM;
           }
       }
   }
   
#endif /* BCM_XGS3_SWITCH_SUPPORT */
   return BCM_E_PARAM;
}

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
/*
 * Function:
 *      _bcm_esw_stat_custom_rsv_add
 * Description:
 *      Add a certain packet type to SDK reserved debug counter to count
 * Parameters:
 *      unit  - StrataSwitch PCI device unit number.
 *      port  - Port number (unused).
 *      type  - SNMP statistics type.
 *      trigger - The counter select value
 *		      (see stat.h for bit definitions).
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_stat_custom_rsv_add(int unit, bcm_port_t port, bcm_stat_val_t type,
                    bcm_custom_stat_trigger_t trigger)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT

   if (!SOC_IS_XGS3_SWITCH(unit)) {
       return BCM_E_UNAVAIL;
   }

   if (!_DBG_CNTR_IS_VALID(unit, type)) {
       return BCM_E_PARAM;
   }
    /* port = -1 is valid port in that case */
    if ((port != -1)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_stat_gport_validate(unit, port, &port));
    }
   return _bcm_stat_custom_change(unit, port, type, trigger, 1);
#else
   return BCM_E_UNAVAIL;

#endif /* BCM_XGS3_SWITCH_SUPPORT */
}
#endif /* BCM_TRIDENT3_SUPPORT || BCM_TOMAHAWK3_SUPPORT */



/*
 * Function:
 *      bcm_esw_stat_custom_add 
 * Description:
 *      Add a certain packet type to debug counter to count 
 * Parameters:
 *      unit  - StrataSwitch PCI device unit number.
 *      port  - Port number (unused).
 *      type  - SNMP statistics type.
 *      trigger - The counter select value
 *		      (see stat.h for bit definitions).
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_stat_custom_add(int unit, bcm_port_t port, bcm_stat_val_t type,
                    bcm_custom_stat_trigger_t trigger)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT

   if (!SOC_IS_XGS3_SWITCH(unit)) {
       return BCM_E_UNAVAIL;
   }

   if (!_DBG_CNTR_IS_VALID(unit, type)) {
       return BCM_E_PARAM;
   }
   if (_DBG_CNTR_IS_RSV(type)) {
       return BCM_E_CONFIG;
   }
   BCM_IF_ERROR_RETURN(
       _bcm_esw_stat_gport_validate(unit, port, &port));

   return _bcm_stat_custom_change(unit, port, type, trigger, 1);  
#else
   return BCM_E_UNAVAIL;

#endif /* BCM_XGS3_SWITCH_SUPPORT */
}
/*
 * Function:
 *      bcm_esw_stat_custom_delete 
 * Description:
 *      Deletes a certain packet type from debug counter  
 * Parameters:
 *      unit  - StrataSwitch PCI device unit number.
 *      port  - Port number (unused).
 *      type  - SNMP statistics type.
 *      trigger - The counter select value
 *		      (see stat.h for bit definitions).
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_stat_custom_delete(int unit, bcm_port_t port,bcm_stat_val_t type, 
                       bcm_custom_stat_trigger_t trigger)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT

   if (!SOC_IS_XGS3_SWITCH(unit)) {
       return BCM_E_UNAVAIL;
   }

   if (!_DBG_CNTR_IS_VALID(unit, type)) {
       return BCM_E_PARAM;
   }

   if (_DBG_CNTR_IS_RSV(type)) {
      return BCM_E_CONFIG;
   }

   BCM_IF_ERROR_RETURN(
       _bcm_esw_stat_gport_validate(unit, port, &port));

   return _bcm_stat_custom_change(unit, port, type, trigger, 0);  
#else
   return BCM_E_UNAVAIL;

#endif /* BCM_XGS3_SWITCH_SUPPORT */
}

/*
 * Function:
 *      bcm_esw_stat_custom_delete_all
 * Description:
 *      Deletes a all packet types from debug counter  
 * Parameters:
 *      unit  - StrataSwitch PCI device unit number.
 *      port  - Port number (unused).
 *      type  - SNMP statistics type.
 *
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_stat_custom_delete_all(int unit, bcm_port_t port,bcm_stat_val_t type)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT

   int  i;
   int  res;
   
   for (i = 0; i < bcmDbgCntNum; i++) {
        res = bcm_esw_stat_custom_delete(unit, port, type, i);
        if (res != BCM_E_NONE && res != BCM_E_UNAVAIL && 
            res != BCM_E_NOT_FOUND && res != BCM_E_PARAM) {
            return res;
        }
   }
   return BCM_E_NONE;
#else
   return BCM_E_UNAVAIL;

#endif /* BCM_XGS3_SWITCH_SUPPORT */

}

/*
 * Function:
 *      bcm_esw_stat_custom_check
 * Description:
 *      Check if certain packet types is part of debug counter  
 * Parameters:
 *      unit  - StrataSwitch PCI device unit number.
 *      port  - Port number (unused).
 *      type  - SNMP statistics type.
 *      trigger - The counter select value
 *		      (see stat.h for bit definitions).
 *      result - [OUT] result of a query. 0 if positive , -1 if negative
 *
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_stat_custom_check(int unit, bcm_port_t port, bcm_stat_val_t type, 
                      bcm_custom_stat_trigger_t trigger, int *result)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
   uint32	ctr_sel, ctrl_bmp, res;
   int		i;
#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
   uint64	ctr_sel_64, ctrl_bmp_64;
   int  rdbgc_sel2_64 = SOC_REG_IS_VALID(unit, RDBGC_SELECT_2_64r);
#endif /* BCM_TRIUMPH_SUPPORT || BCM_SCORPION_SUPPORT */

   if (!SOC_IS_XGS3_SWITCH(unit)) {
       return BCM_E_UNAVAIL;
   }

   if (!_DBG_CNTR_IS_VALID(unit, type)) {
       return BCM_E_PARAM;
   }

   BCM_IF_ERROR_RETURN(
       _bcm_esw_stat_gport_validate(unit, port, &port));
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
   if (SOC_IS_TRIDENT3X(unit) || SOC_IS_TOMAHAWK3(unit)) {
       return(_bcm_td3_stat_custom_check(unit, type, trigger, result));
   }
#endif /* BCM_TRIDENT3_SUPPORT || BCM_TOMAHAWK3_SUPPORT */
   for (i = 0; i < COUNTOF(bcm_dbg_cntr_rx); i++) {
       if (bcm_dbg_cntr_rx[i].counter == type) {
           SOC_IF_ERROR_RETURN(
               _bcm_stat_custom_to_bit(unit, _DBG_CNT_RX_CHAN, trigger, &res));
           if (res < 32) {
               SOC_IF_ERROR_RETURN(soc_reg32_get(unit, bcm_dbg_cntr_rx[i].select,
                                                 REG_PORT_ANY, 0, &ctr_sel));
               ctrl_bmp = soc_reg_field_get(unit, bcm_dbg_cntr_rx[i].select, 
                                            ctr_sel, BITMAPf);
               *result = ((ctrl_bmp & (1 << res)) != 0); 
               return BCM_E_NONE;
           }
#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
           else if (SOC_IS_TRX(unit) && (res < 65)) {
               /*
                * res < 65 check is required because the only device (Apache)
                * having RDBGC_SELECT_2_64 register has its width as 33 bits
                * and not 64 bits (in which case the check should be (res<96)
                */
               if (rdbgc_sel2_64) {
                   uint64 tmp;
                   SOC_IF_ERROR_RETURN(READ_RDBGC_SELECT_2_64r(unit, i, 
                               &ctr_sel_64));
                   ctrl_bmp_64 = soc_reg64_field_get(unit, RDBGC_SELECT_2_64r,
                           ctr_sel_64, BITMAPf);
                   res -= 32;
                   COMPILER_64_SET(tmp, 0, 1);
                   COMPILER_64_SHL(tmp, res);
                   COMPILER_64_AND(ctrl_bmp_64, tmp);
                   *result = !(COMPILER_64_IS_ZERO(ctrl_bmp_64));
                   return BCM_E_NONE;
               } else if (res < 64) {
                   SOC_IF_ERROR_RETURN(READ_RDBGC_SELECT_2r(unit, i, &ctr_sel));
                   ctrl_bmp = soc_reg_field_get(unit, RDBGC_SELECT_2r,
                           ctr_sel, BITMAPf);
                   *result = ((ctrl_bmp & (1 << (res - 32))) != 0);
                   return BCM_E_NONE;
               }
           }
#endif /* BCM_TRIUMPH_SUPPORT || BCM_SCORPION_SUPPORT */
           else {
               return BCM_E_PARAM;
           }
       }
   }

   for (i = 0; i < COUNTOF(bcm_dbg_cntr_tx); i++) {
       if (bcm_dbg_cntr_tx[i].counter == type) {
           SOC_IF_ERROR_RETURN(
               _bcm_stat_custom_to_bit(unit,  _DBG_CNT_TX_CHAN, trigger, &res));

           if (res < 32) {
               SOC_IF_ERROR_RETURN(soc_reg32_get(unit, bcm_dbg_cntr_tx[i].select,
                                                 REG_PORT_ANY, 0, &ctr_sel));
               ctrl_bmp = soc_reg_field_get(unit, bcm_dbg_cntr_tx[i].select, 
                                            ctr_sel, BITMAPf);

               *result = ((ctrl_bmp & (1 << res)) != 0); 
               return BCM_E_NONE;
           }
#if defined(BCM_TRIDENT2_SUPPORT)
           else if (SOC_IS_TD2_TT2(unit) && res < 64) {
               SOC_IF_ERROR_RETURN(READ_TDBGC_SELECT_2r(unit, i, &ctr_sel));
               ctrl_bmp = soc_reg_field_get(unit, TDBGC_SELECT_2r,
                                            ctr_sel, BITMAPf);
               *result = ((ctrl_bmp & (1 << (res - 32))) != 0);
               return BCM_E_NONE;
           }
#endif /* BCM_TRIDENT2_SUPPORT */
           else {
               return BCM_E_PARAM;
           }
       }
   }
   return BCM_E_PARAM;
#else 
   return BCM_E_UNAVAIL;

#endif /* BCM_XGS3_SWITCH_SUPPORT */
}


/*
 * Function:
 *
 *	_bcm_esw_stat_ovr_error_control_set
 *
 * Description:
 *
 *  Sets Oversize error count flag to include/exclude oversized valid packets
 *  Oversize packet count increments  (ROVRr, TOVRr), when the packet size 
 *  exceeds port cnt size (PORT_CNTSIZEr).
 *  This function controls the inclusion of oversize packet counts as part of 
 *  of packet errors. 
 *
 *  Caution: By default the cotrol flag will be TRUE to include
 *  oversize  valid packets as oversize errors. 
 *  The users can turn off this flag to exclude oversize packet errors,
 *  which will differ from MIB RFC, which states that any packet greater 
 *  than the size of 1518 are considered as oversize errors. 
 *
 * Parameters:
 * 
 *      unit - unit number
 *      port - local port number
 *	    value - oversize error control flag
 *
 * Returns:
 *
 *      BCM_E_NONE
 */
int
_bcm_esw_stat_ovr_error_control_set(int unit, bcm_port_t port, int value)
{
    if (_bcm_stat_ovr_control == NULL) {
        return BCM_E_INIT;
    }

    if (port == CMIC_PORT(unit)) { /* Not supported on CMIC */
        return BCM_E_PORT;
    }

    if (value) {
        /* Set ovr control bit map for the port */
        _BCM_STAT_OVR_CONTROL_SET(unit, port);
    } else  {
        /* Set ovr control bit map for the port */
        _BCM_STAT_OVR_CONTROL_CLR(unit, port);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_esw_stat_ovr_error_control_get
 * Description:
 *	Get _bcm_stat_ovr_threshold for the given unit and port.
 * Parameters:
 *      unit - unit number
 *      port - local port number
 *	value - (OUT) pointer to oversize threshold
 * Returns:
 *      BCM_E_NONE
 */
int
_bcm_esw_stat_ovr_error_control_get(int unit, bcm_port_t port, int *value)
{
    if (_bcm_stat_ovr_control == NULL) {
        return BCM_E_INIT;
    }

    if (port == CMIC_PORT(unit)) { /* Not supported on CMIC */
        return BCM_E_PORT;
    }

    /* Get ovr control bit for the port */
    *value = ((_BCM_STAT_OVR_CONTROL_GET(unit, port)) ? 1 : 0);

    return BCM_E_NONE;
}


/*
 * Function:
 *	_bcm_esw_stat_ovr_threshold_set
 * Description:
 *	Set _bcm_stat_ovr_threshold for the given unit and port.
 * Parameters:
 *      unit - unit number
 *      port - local port number
 *	value - oversize threshold
 * Returns:
 *      BCM_E_NONE
 */
int
_bcm_esw_stat_ovr_threshold_set(int unit, bcm_port_t port, int value)
{
    if ((_bcm_stat_ovr_threshold == NULL) || 
        (_bcm_stat_ovr_threshold[unit] == NULL)) {
        return BCM_E_INIT;
    }

    if (port == CMIC_PORT(unit)) { /* Not supported on CMIC */
        return BCM_E_PORT;
    }

    _bcm_stat_ovr_threshold[unit][port] = value;

    /* Call Port Control module */
    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        return (bcmi_esw_portctrl_cntmaxsize_set(unit, port, value));
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        uint64 value64;
        int blk;
        bcm_pbmp_t blk_bitmap;
        bcm_port_t pix;

#if defined (BCM_TOMAHAWK_SUPPORT) || defined (BCM_APACHE_SUPPORT)
        if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_APACHE(unit) || SOC_IS_TRIDENT3X(unit)) {
            if (IS_CL_PORT(unit, port) || IS_CLG2_PORT(unit, port)) {
                SOC_IF_ERROR_RETURN(WRITE_CLPORT_CNTMAXSIZEr(unit, port, value));
            } else
#if defined(BCM_HURRICANE4_SUPPORT)
            if (SOC_IS_HURRICANE4(unit) && IS_EGPHY_PORT(unit, port)) {
                SOC_IF_ERROR_RETURN(WRITE_GPORT_CNTMAXSIZEr(unit, port, value));
            } else
#endif
            {
                SOC_IF_ERROR_RETURN(WRITE_XLPORT_CNTMAXSIZEr(unit, port, value));
            }
        } else
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_APACHE_SUPPORT */

        if (SOC_IS_TD2_TT2(unit)) {
            SOC_IF_ERROR_RETURN(WRITE_PGW_CNTMAXSIZEr(unit, port, value));
        } else if (soc_feature(unit, soc_feature_unified_port) && 
                !SOC_IS_HURRICANE2(unit) && !SOC_IS_GREYHOUND(unit) &&
                !SOC_IS_HURRICANE3(unit) && !SOC_IS_GREYHOUND2(unit)) {
            SOC_IF_ERROR_RETURN
                    (WRITE_PORT_CNTMAXSIZEr(unit, port, value));
#if defined (BCM_KATANA_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
        } else if (SOC_IS_KATANAX(unit) && IS_MXQ_PORT(unit,port)) {
            SOC_IF_ERROR_RETURN
                    (WRITE_X_GPORT_CNTMAXSIZEr(unit, port, value));
#endif
#if defined (BCM_SABER2_SUPPORT)
            } else if (SOC_IS_SABER2(unit) && IS_XL_PORT(unit, port)) {
                SOC_IF_ERROR_RETURN
                    (WRITE_XLPORT_CNTMAXSIZEr(unit, port, value));
#endif

#if defined (BCM_ENDURO_SUPPORT) || defined(BCM_HURRICANE_SUPPORT)
        } else if (IS_XQ_PORT(unit, port) && (SOC_IS_ENDURO(unit) || 
            SOC_IS_HURRICANE(unit) || SOC_IS_HURRICANE2(unit))) {
            SOC_IF_ERROR_RETURN
                    (WRITE_QPORT_CNTMAXSIZEr(unit, port, value));
#endif
        } else if (IS_XL_PORT(unit, port) || IS_MXQ_PORT(unit,port)) {
            if ((SOC_IS_HURRICANE2(unit) || SOC_IS_GREYHOUND(unit) || 
                SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)) && 
                    IS_XL_PORT(unit, port)) {
                SOC_IF_ERROR_RETURN
                        (WRITE_XLPORT_CNTMAXSIZEr(unit, port, value));
            } else {
                SOC_IF_ERROR_RETURN
                        (WRITE_GPORT_CNTMAXSIZEr(unit, port, value));
            }
#ifdef BCM_GREYHOUND2_SUPPORT
        } else if (IS_CL_PORT(unit, port) && SOC_IS_GREYHOUND2(unit)) {
                SOC_IF_ERROR_RETURN
                        (WRITE_CLPORT_CNTMAXSIZEr(unit, port, value));
#endif
        } else if (IS_GE_PORT(unit,port) || IS_FE_PORT(unit,port)) {
            SOC_IF_ERROR_RETURN(WRITE_GPORT_CNTMAXSIZEr(unit, port, value));
            if (!(soc_feature(unit, soc_feature_unified_port)) && \
                !(soc_feature(unit, soc_feature_unimac))) {
                if (SOC_REG_IS_VALID(unit, FRM_LENGTHr)) {
                    SOC_IF_ERROR_RETURN(WRITE_FRM_LENGTHr(unit, port, value));
                }
            }

            /* On some devices, this is per-PIC register */
            blk = SOC_PORT_BLOCK(unit, port);
            blk_bitmap = SOC_BLOCK_BITMAP(unit, blk);
            PBMP_ITER(blk_bitmap, pix) {
                _bcm_stat_ovr_threshold[unit][pix] = value;
            }
        } else {
            COMPILER_64_SET(value64, 0, value);
            SOC_IF_ERROR_RETURN(WRITE_MAC_CNTMAXSZr(unit, port, value64));

            if (IS_GX_PORT(unit, port)) { 
                SOC_IF_ERROR_RETURN(WRITE_GPORT_CNTMAXSIZEr(unit, port, value)); 
            } 
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_esw_stat_ovr_threshold_get
 * Description:
 *	Get _bcm_stat_ovr_threshold for the given unit and port.
 * Parameters:
 *      unit - unit number
 *      port - local port number
 *	value - (OUT) pointer to oversize threshold
 * Returns:
 *      BCM_E_NONE
 */
int
_bcm_esw_stat_ovr_threshold_get(int unit, bcm_port_t port, int *value)
{
    if ((_bcm_stat_ovr_threshold == NULL) || 
        (_bcm_stat_ovr_threshold[unit] == NULL)) {
        return BCM_E_INIT;
    }

    if (port == CMIC_PORT(unit)) { /* Not supported on CMIC */
        return BCM_E_PORT;
    }

    *value = _bcm_stat_ovr_threshold[unit][port];

    return BCM_E_NONE;
}

int
_bcm_stat_reg_op(int unit, bcm_port_t port, 
                 int sync_mode, int reg_op,
                 soc_reg_t ctr_reg, uint64 *val)
{
    REG_MATH_DECL; /* Required for use of the REG_* macros */
    uint64 clear_val;
    
    switch (reg_op) {
    case _BCM_STAT_REG_ADD:    
        REG_ADD(unit, port, sync_mode, ctr_reg, *val);
      break;  
    case _BCM_STAT_REG_SUB:    
        REG_SUB(unit, port, sync_mode, ctr_reg, *val);
      break;  
    case _BCM_STAT_REG_CLEAR:    
        COMPILER_64_ZERO(clear_val);
        REG_CLEAR(unit, port, ctr_reg, clear_val);
      break;  
    default:
      break;  
    }
    return BCM_E_NONE;

}

int
_bcm_stat_reg_idx_op(int unit, bcm_port_t port, 
                     int sync_mode, int reg_op,
                     soc_reg_t ctr_reg, int ar_idx,
                     uint64 *val)
{
    REG_MATH_DECL; /* Required for use of the REG_* macros */
    
    switch (reg_op) {
    case _BCM_STAT_REG_ADD:    
        REG_ADD_IDX(unit, port, sync_mode, ctr_reg, *val, ar_idx);
      break;  
    case _BCM_STAT_REG_SUB:    
        REG_SUB_IDX(unit, port, sync_mode, ctr_reg, *val, ar_idx);
      break;  
    case _BCM_STAT_REG_CLEAR:    
        REG_CLEAR_IDX(unit, port, sync_mode, ctr_reg, *val, ar_idx);
      break;  
    default:
      break;  
    }
    return BCM_E_NONE;

}

/*
 * Function:
 *      bcm_esw_stat_group_dump_all
 * Description:
 *      Display all important information about all stat object  and groups
 *      Calls common function _bcm_esw_stat_group_dump_info()
 *
 * Parameters:
 *      unit  - (IN) unit number
 * Return Value:
 *      None
 * Notes:
 */
int
bcm_esw_stat_group_dump_all(int unit)
{
#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)) {
        return _bcm_esw_flowcnt_group_dump_info(unit, TRUE, 0, 0);
    } else
#endif
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    _bcm_esw_stat_group_dump_info(unit, TRUE, 0, 0);
#endif
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_stat_group_dump
 * Description:
 *      Display important information about specified stat object and group
 *      Calls common function _bcm_esw_stat_group_dump_info()
 * 
 * Parameters:
 *      unit        - (IN) unit number
 *      object      - (IN)  Accounting Object
 *      Group_mode  - (IN)  Group Mode
 * Return Value:
 *      None
 * Notes:
 */
int
bcm_esw_stat_group_dump(int unit, bcm_stat_object_t object,
                        bcm_stat_group_mode_t group_mode)
{
#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowcnt)) {
        _bcm_esw_flowcnt_group_dump_info(unit, FALSE, object, group_mode);
        return BCM_E_NONE;
    }
#endif
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    _bcm_esw_stat_group_dump_info(unit, FALSE, object, group_mode);
#endif
    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_stat_custom_group_id_create
 * Description:
 *	Associate an accounting object to customized group mode. 
 *	User can allocate pool,base index to accounting objects.
 *
 * Parameters:
 *	
 *  unit                : (IN)  UNIT number      
 *  mode_id             : (IN)  Created Mode Identifier for Stat Flex Group Mode
 *  object              : (IN)  Accounting object
 *  pool_id             : (IN)  Pool Id
 *  base_idx            : (IN)  Base Index in a pool
 *  counter_info        : (OUT) Stat counter info
 *
 * Returns:
 *	BCM_E_*
 */
int bcm_esw_stat_custom_group_id_create (int  unit,
        uint32                         mode_id,
        bcm_stat_object_t              object,
        uint32                         pool_id,
        uint32                         base_idx,
        bcm_stat_custom_counter_info_t *counter_info)
{
#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_stat_compaction_support)) {
        return _bcm_esw_stat_custom_group_id_create(unit,
               mode_id, object, pool_id, base_idx, counter_info);
    } else
#endif
    { 
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *  bcm_esw_stat_custom_counter_id_move	
 * Description:
 *  The function is used to move the old set of counters to new
 *  free location provided by user. The new free location can
 *  be within the same pool as old counters or a new pool.
 *
 * Parameters:
 *	
 *  unit                 : (IN)     UNIT number      
 *  idx_action           : (IN)     counter base index action
 *  counter_info_old     : (IN)     Old Counter information
 *  counter_info_new     : (IN/OUT) New Counter information
 *
 * Returns:
 *	BCM_E_*
 */
int bcm_esw_stat_custom_counter_id_move(int unit,
        bcm_stat_custom_base_index_action_t idx_action,
        bcm_stat_custom_counter_info_t counter_info_old,
        bcm_stat_custom_counter_info_t *counter_info_new)
{
#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_stat_compaction_support)) {
        return _bcm_stat_flex_counter_id_move(unit, idx_action,
                counter_info_old, counter_info_new);
    } else
#endif
    { 
        return BCM_E_UNAVAIL;
    }
}

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *     _bcm_stat_sw_dump
 * Purpose:
 *     Displays STAT software structure information.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_stat_sw_dump(int unit)
{
    int             i;

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information STAT - Unit %d\n"), unit));

    if ((_bcm_stat_ovr_threshold != NULL) &&
        (_bcm_stat_ovr_threshold[unit] != NULL)) {
        LOG_CLI((BSL_META_U(unit,
                            "     Oversize Packet Byte Threshold:\n")));
        for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
            LOG_CLI((BSL_META_U(unit,
                                "       Port %2d : %4d\n"), i,
                     _bcm_stat_ovr_threshold[unit][i]));
        }
    }

    return;
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

/*
 * Function:
 *     _bcm_stat_flexctr_pool_info_dump 
 * Purpose:
 *     Displays flex counter pool software database.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_stat_flexctr_pool_info_sw_dump(int unit)
{
#if defined(BCM_KATANA2_SUPPORT)
    if(soc_feature(unit, soc_feature_flex_stat_compaction_support)) {
        _bcm_stat_flex_dump_pool_info(unit);
    }
#endif
    return;
}
