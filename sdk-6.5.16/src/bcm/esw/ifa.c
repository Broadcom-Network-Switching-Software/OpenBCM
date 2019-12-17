/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * IFA - In-band Flow Analyzer Monitoring Embedded Application APP interface
 * Purpose: API to configure IFA embedded app interface.
 */

#include <shared/bslenum.h>
#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <bcm/error.h>
#include <bcm/ifa.h>

#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/trident3.h>

static sal_mutex_t mutex[BCM_MAX_NUM_UNITS];

int bcm_esw_ifa_lock(int unit)
{
    if (mutex[unit] == NULL) {
        return BCM_E_INIT;
    }

    sal_mutex_take(mutex[unit], sal_mutex_FOREVER);
    return BCM_E_NONE;
}

int bcm_esw_ifa_unlock(int unit)
{
    if (mutex[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (sal_mutex_give(mutex[unit]) != 0) {
        return BCM_E_INTERNAL;
    }
    return BCM_E_NONE;
}

/* Initialize the IFA app. */
int bcm_esw_ifa_init(int unit)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_IFA)
    /* Create mutex */
    if (mutex[unit] == NULL) {
        mutex[unit] = sal_mutex_create("ifa.mutex");
        if (mutex[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
    if ((SOC_IS_TRIDENT3(unit)) || (SOC_IS_MAVERICK2(unit))) {
        result = _bcm_td3_ifa_init(unit);
    }
#endif  /* BCM_TRIDENT3_SUPPORT and BCM_MAVERICK2_SUPPORT */

    /* If init itself fails, there is no point in having the mutex.
     * Destroy it.
     */
    if (BCM_FAILURE(result)) {
        sal_mutex_destroy(mutex[unit]);
        mutex[unit] = NULL;
    }
#endif  /* INCLUDE_IFA */
    return result;
}

/* Shut down the IFA app. */
int bcm_esw_ifa_detach(int unit)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_IFA)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_lock(unit));

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
    if ((SOC_IS_TRIDENT3(unit)) || (SOC_IS_MAVERICK2(unit))) {
        result = _bcm_td3_ifa_detach(unit);
    }
#endif  /* BCM_TRIDENT3_SUPPORT and BCM_MAVERICK2_SUPPORT */

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_unlock(unit));
#endif  /* INCLUDE_IFA */
    return result;
}

/*
 * Function:
 *      bcm_esw_ifa_collector_set
 * Purpose:
 *      Create a ifa collector with given collector info.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      options         - (IN)  Collector create options
 *      collector_info  - (IN)  Collector info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */

int bcm_esw_ifa_collector_set(int unit,
        uint32 options,
        bcm_ifa_collector_info_t *collector_info)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_IFA)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_lock(unit));

    /* IFA app APIs.  */
    if (soc_feature(unit, soc_feature_ifa)) {
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
        if ((SOC_IS_TRIDENT3(unit)) || (SOC_IS_MAVERICK2(unit))) {
            result = _bcm_td3_ifa_collector_set(unit, options,
                    collector_info);
        }
#endif  /* BCM_TRIDENT3_SUPPORT and BCM_MAVERICK2_SUPPORT */
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_unlock(unit));
#endif  /* INCLUDE_IFA */
    return result;
}

/*
 * Function:
 *      bcm_esw_ifa_collector_get
 * Purpose:
 *      Get ifa collector information
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      collector_info  - (OUT) Collector info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */

int bcm_esw_ifa_collector_get(int unit,
        bcm_ifa_collector_info_t *collector_info)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_IFA)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_lock(unit));

    /* IFA app APIs.  */
    if (soc_feature(unit, soc_feature_ifa)) {
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
        if ((SOC_IS_TRIDENT3(unit)) || (SOC_IS_MAVERICK2(unit))) {
            result = _bcm_td3_ifa_collector_get(unit,
                    collector_info);
        }
#endif  /* BCM_TRIDENT3_SUPPORT and BCM_MAVERICK2_SUPPORT */
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_unlock(unit));
#endif  /* INCLUDE_IFA */
    return result;
}

int bcm_esw_ifa_config_info_set(int unit,
        uint32 options,
        bcm_ifa_config_info_t *config_data)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_IFA)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_lock(unit));

    /* IFA app APIs.  */
    if (soc_feature(unit, soc_feature_ifa)) {
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
        if ((SOC_IS_TRIDENT3(unit)) || (SOC_IS_MAVERICK2(unit))) {
            result = _bcm_td3_ifa_config_set(unit, options,
                    config_data);
        }
#endif  /* BCM_TRIDENT3_SUPPORT and BCM_MAVERICK2_SUPPORT */
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_unlock(unit));
#endif  /* INCLUDE_IFA */
    return result;
}

int bcm_esw_ifa_config_info_get(int unit,
        bcm_ifa_config_info_t *config_data)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_IFA)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_lock(unit));

    /* IFA app APIs.  */
    if (soc_feature(unit, soc_feature_ifa)) {
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
        if ((SOC_IS_TRIDENT3(unit)) || (SOC_IS_MAVERICK2(unit))) {
            result = _bcm_td3_ifa_config_get(unit,
                    config_data);
        }
#endif  /* BCM_TRIDENT3_SUPPORT and BCM_MAVERICK2_SUPPORT */
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_unlock(unit));
#endif  /* INCLUDE_IFA */
    return result;
}

/*
 * Function:
 *      bcm_esw_ifa_stat_info_get
 * Purpose:
 *      Get ifa statistics information data
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      stat_data       - (OUT) Statistics information data
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_ifa_stat_info_get(int unit,
        bcm_ifa_stat_info_t *stat_data)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_IFA)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_lock(unit));

    /* IFA app APIs.  */
    if (soc_feature(unit, soc_feature_ifa)) {
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
        if ((SOC_IS_TRIDENT3(unit)) || (SOC_IS_MAVERICK2(unit))) {
            result = _bcm_td3_ifa_stat_get(unit,
                    stat_data);
        }
#endif  /* BCM_TRIDENT3_SUPPORT and BCM_MAVERICK2_SUPPORT */
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_unlock(unit));
#endif  /* INCLUDE_IFA */
    return result;
}

/*
 * Function:
 *      bcm_esw_ifa_stat_info_set
 * Purpose:
 *      Get ifa statistics information data
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      stat_data       - (OUT) Statistics information data
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_ifa_stat_info_set(int unit,
        bcm_ifa_stat_info_t *stat_data)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_IFA)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_lock(unit));

    /* IFA app APIs.  */
    if (soc_feature(unit, soc_feature_ifa)) {
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
        if ((SOC_IS_TRIDENT3(unit)) || (SOC_IS_MAVERICK2(unit))) {
            result = _bcm_td3_ifa_stat_set(unit,
                    stat_data);
        }
#endif  /* BCM_TRIDENT3_SUPPORT and BCM_MAVERICK2_SUPPORT */
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_unlock(unit));
#endif  /* INCLUDE_IFA */
    return result;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_esw_ifa_sync
 * Purpose:
 *      Warmboot scache sync
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_esw_ifa_sync(int unit)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_IFA)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_lock(unit));

    if (soc_feature(unit, soc_feature_ifa)) {
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
        if ((SOC_IS_TRIDENT3(unit)) || (SOC_IS_MAVERICK2(unit))) {
            result = _bcm_td3_ifa_sync(unit);
        }
#endif  /* BCM_TRIDENT3_SUPPORT and BCM_MAVERICK2_SUPPORT */
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_unlock(unit));
#endif /* INCLUDE_IFA */
    return result;
}
#endif /* BCM_WARM_BOOT_SUPPORT */
