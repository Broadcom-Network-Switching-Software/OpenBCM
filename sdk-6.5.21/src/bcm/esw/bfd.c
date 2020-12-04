/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       bfd.c
 * Purpose:    Bidirectional Forwarding Detection APIs.
 *
 * Notes:      BFD functions will return BCM_E_UAVAIL unless the following
 *             feature is available:  soc_feature_bfd
 */

#include <soc/defs.h>
#include <shared/bsl.h>
#if defined(INCLUDE_BFD)

#include <soc/drv.h>
#include <bcm_int/esw/bfd.h>
#include <bcm/bfd.h>
#include <bcm/error.h>

#if defined(BCM_KATANA_SUPPORT)
#include <bcm_int/esw/katana.h>
#endif /* BCM_KATANA_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT)
#include <bcm_int/esw/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
#include <bcm_int/esw/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */

/*
 * BFD Function Vector Driver
 */
bcm_esw_bfd_drv_t    *bcm_esw_bfd_drv[BCM_MAX_NUM_UNITS] = {0};

/*
 * BFD Lock  
 */
static sal_mutex_t mutex[BCM_MAX_NUM_UNITS];

/*
 * Macro:
 *     BFD_INIT
 * Purpose:
 *     Causes a routine to return BCM_E_UNAVAIL (feature not available), or
 *     BCM_E_INIT (BFD not initialized).
 */
#define BFD_INIT(unit)                            \
    if (!soc_feature(unit, soc_feature_bfd)) {    \
        return BCM_E_UNAVAIL;                     \
    } else if (BCM_ESW_BFD_DRV(unit) == NULL) {   \
        return BCM_E_UNAVAIL;                     \
    } else if (mutex[unit] == NULL) {             \
        return BCM_E_INIT;                        \
    }


#define BFD_LOCK(unit)    sal_mutex_take(mutex[unit], sal_mutex_FOREVER)
#define BFD_UNLOCK(unit)  sal_mutex_give(mutex[unit])


/*
 * Function:
 *      bcm_esw_bfd_init
 * Purpose:
 *      Initialize the BFD subsystem
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_bfd_init(int unit)
{
    int result = BCM_E_UNAVAIL;

    if (soc_feature(unit, soc_feature_bfd)) {
        if (SAL_BOOT_PLISIM) {
            return SOC_E_NONE;
        }

        if (mutex[unit] == NULL) {
            mutex[unit] = sal_mutex_create("bfd.mutex");

            if (mutex[unit] == NULL) {
                return BCM_E_MEMORY;
            }
        }

#if defined(BCM_KATANA_SUPPORT)
        if (SOC_IS_KATANAX(unit)) {
            result = bcmi_kt_bfd_init(unit);
        } else
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            result = bcmi_tr3_bfd_init(unit);
        } else
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit) ||
            SOC_IS_TOMAHAWKX(unit)) {
            result = bcmi_td2_bfd_init(unit);
        } else
#endif
        {
            LOG_CLI((BSL_META_U(unit,
                                "Not implemented.\n")));
            result = BCM_E_INTERNAL;
        }

        if (BCM_FAILURE(result)) {
            sal_mutex_destroy(mutex[unit]);
            mutex[unit] = NULL;
        }
    }

    return result;
}

/*
 * Function:
 *      bcm_esw_bfd_detach
 * Purpose:
 *      Shut down the BFD subsystem
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_bfd_detach(int unit)
{
    int result = BCM_E_UNAVAIL;

    if (soc_feature(unit, soc_feature_bfd)) {

        if (mutex[unit] == NULL) {
            return BCM_E_NONE;
        }

        BFD_LOCK(unit);

        if ((BCM_ESW_BFD_DRV(unit) != NULL) &&
            (BCM_ESW_BFD_DRV(unit)->detach != NULL)) {
            result = BCM_ESW_BFD_DRV(unit)->detach(unit);
        }
        
        BFD_UNLOCK(unit);

        sal_mutex_destroy(mutex[unit]);
        mutex[unit] = NULL;
    }

    return result;
}

/*
 * Function:
 *      bcm_esw_bfd_endpoint_create
 * Purpose:
 *      Create or replace an BFD endpoint object
 * Parameters:
 *      unit          - (IN) Unit number.
 *      endpoint_info - (IN/OUT) Pointer to an BFD endpoint structure
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_bfd_endpoint_create(int unit, 
                            bcm_bfd_endpoint_info_t *endpoint_info)
{
    int result = BCM_E_UNAVAIL;

    BFD_INIT(unit);

    BFD_LOCK(unit);

    if (BCM_ESW_BFD_DRV(unit)->endpoint_create != NULL) {
        result = BCM_ESW_BFD_DRV(unit)->endpoint_create(unit, endpoint_info);
    }
        
    BFD_UNLOCK(unit);

    return result;
}

/*
 * Function:
 *      bcm_esw_bfd_endpoint_get
 * Purpose:
 *      Get an BFD endpoint object
 * Parameters:
 *      unit          - (IN) Unit number.
 *      endpoint      - (IN) The ID of the endpoint object to get
 *      endpoint_info - (OUT) Pointer to an BFD endpoint structure to
 *                      receive the data
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_bfd_endpoint_get(int unit, 
                         bcm_bfd_endpoint_t endpoint, 
                         bcm_bfd_endpoint_info_t *endpoint_info)
{
    int result = BCM_E_UNAVAIL;

    BFD_INIT(unit);

    BFD_LOCK(unit);

    if (BCM_ESW_BFD_DRV(unit)->endpoint_get != NULL) {
        result = BCM_ESW_BFD_DRV(unit)->endpoint_get(unit, endpoint,
                                                     endpoint_info);
    }
        
    BFD_UNLOCK(unit);

    return result;
}

/*
 * Function:
 *      bcm_esw_bfd_tx_start
 * Purpose:
 *      Start all BFD endpoint TX PDUs
 * Parameters:
 *      unit - (IN) Unit number.
 * results:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_bfd_tx_start(int unit)
{
    int result = BCM_E_UNAVAIL;

    BFD_INIT(unit);

    BFD_LOCK(unit);

    if (BCM_ESW_BFD_DRV(unit)->tx_start != NULL) {
        result = BCM_ESW_BFD_DRV(unit)->tx_start(unit);
    }

    BFD_UNLOCK(unit);

    return result;
}

/*
 * Function:
 *      bcm_esw_bfd_tx_stop
 * Purpose:
 *      Stop all BFD endpoint TX PDUs
 * Parameters:
 *      unit - (IN) Unit number.
 * results:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_bfd_tx_stop(int unit)
{
    int result = BCM_E_UNAVAIL;

    BFD_INIT(unit);

    BFD_LOCK(unit);

    if (BCM_ESW_BFD_DRV(unit)->tx_stop != NULL) {
        result = BCM_ESW_BFD_DRV(unit)->tx_stop(unit);
    }

    BFD_UNLOCK(unit);

    return result;
}

/*
 * Function:
 *      bcm_esw_bfd_endpoint_destroy
 * Purpose:
 *      Destroy a BFD endpoint object
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the BFD endpoint object to destroy
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_bfd_endpoint_destroy(int unit, 
                             bcm_bfd_endpoint_t endpoint)
{
    int result = BCM_E_UNAVAIL;

    BFD_INIT(unit);

    BFD_LOCK(unit);

    if (BCM_ESW_BFD_DRV(unit)->endpoint_destroy != NULL) {
        result = BCM_ESW_BFD_DRV(unit)->endpoint_destroy(unit, endpoint);
    }

    BFD_UNLOCK(unit);

    return result;
}

/*
 * Function:
 *      bcm_esw_bfd_endpoint_destroy_all
 * Purpose:
 *      Destroy all BFD endpoint objects
 * Parameters:
 *      unit - (IN) Unit number.
 * results:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_bfd_endpoint_destroy_all(int unit)
{
    int result = BCM_E_UNAVAIL;

    BFD_INIT(unit);

    BFD_LOCK(unit);

    if (BCM_ESW_BFD_DRV(unit)->endpoint_destroy_all != NULL) {
        result = BCM_ESW_BFD_DRV(unit)->endpoint_destroy_all(unit);
    }

    BFD_UNLOCK(unit);

    return result;
}

/*
 * Function:
 *      bcm_esw_bfd_endpoint_poll
 * Purpose:
 *      Poll a BFD endpoint object
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the BFD endpoint object to poll
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_bfd_endpoint_poll(int unit, 
                          bcm_bfd_endpoint_t endpoint)
{
    int result = BCM_E_UNAVAIL;

    BFD_INIT(unit);

    BFD_LOCK(unit);

    if (BCM_ESW_BFD_DRV(unit)->endpoint_poll!= NULL) {
        result = BCM_ESW_BFD_DRV(unit)->endpoint_poll(unit, endpoint);
    }

    BFD_UNLOCK(unit);

    return result;
}

/*
 * Function:
 *      bcm_esw_bfd_event_register
 * Purpose:
 *      Register a callback for handling BFD events
 * Parameters:
 *      unit        - (IN) Unit number.
 *      event_types - (IN) The set of BFD events for which
 *                    the specified callback should be called
 *      cb          - (IN) A pointer to the callback function to call
 *                    for the specified BFD events
 *      user_data   - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_bfd_event_register(int unit, 
                           bcm_bfd_event_types_t event_types, 
                           bcm_bfd_event_cb cb, 
                           void *user_data)
{
    int result = BCM_E_UNAVAIL;

    BFD_INIT(unit);

    BFD_LOCK(unit);

    if (BCM_ESW_BFD_DRV(unit)->event_register != NULL) {
        result = BCM_ESW_BFD_DRV(unit)->event_register(unit,
                                                       event_types,
                                                       cb, user_data);
    }
        
    BFD_UNLOCK(unit);

    return result;
}

/*
 * Function:
 *      bcm_esw_bfd_event_unregister
 * Purpose:
 *      Unregister a callback for handling BFD events
 * Parameters:
 *      unit        - (IN) Unit number.
 *      event_types - (IN) The set of BFD events for which the specified
 *                    callback should not be called
 *      cb          - (IN) A pointer to the callback function to
 *                    unregister from the specified BFD events
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_bfd_event_unregister(int unit, 
                             bcm_bfd_event_types_t event_types, 
                             bcm_bfd_event_cb cb)
{
    int result = BCM_E_UNAVAIL;

    BFD_INIT(unit);

    BFD_LOCK(unit);

    if (BCM_ESW_BFD_DRV(unit)->event_unregister != NULL) {
        result = BCM_ESW_BFD_DRV(unit)->event_unregister(unit,
                                                         event_types, cb);
    }
        
    BFD_UNLOCK(unit);

    return result;
}

/*
 * Function:
 *      bcm_esw_bfd_endpoint_stat_get
 * Purpose:
 *      Get BFD endpoint stats
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the endpoint object to get stats for
 *      ctr_info - (IN/OUT) Pointer to endpoint count structure to
 *                 receive the data
 *      clear    - (IN) If set, clear stats after read
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_bfd_endpoint_stat_get(int unit, 
                              bcm_bfd_endpoint_t endpoint, 
                              bcm_bfd_endpoint_stat_t *ctr_info, 
                              uint32 options)
{
    int result = BCM_E_UNAVAIL;

    BFD_INIT(unit);

    BFD_LOCK(unit);

    if (BCM_ESW_BFD_DRV(unit)->endpoint_stat_get != NULL) {
        result = BCM_ESW_BFD_DRV(unit)->endpoint_stat_get(unit, endpoint,
                                                          ctr_info, options);
    }
        
    BFD_UNLOCK(unit);

    return result;
}
/*
 * Function:
 *      bcm_esw_bfd_auth_sha1_set
 * Purpose:
 *      Set SHA1 authentication entry
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the SHA1 entry to configure
 *      sha1  - (IN) Pointer to SHA1 info structure
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_bfd_auth_sha1_set(int unit, 
                          int index, 
                          bcm_bfd_auth_sha1_t *sha1)
{
    int result = BCM_E_UNAVAIL;

    BFD_INIT(unit);

    BFD_LOCK(unit);

    if (BCM_ESW_BFD_DRV(unit)->auth_sha1_set != NULL) {
        result = BCM_ESW_BFD_DRV(unit)->auth_sha1_set(unit, index, sha1);
    }
        
    BFD_UNLOCK(unit);

    return result;
}

/*
 * Function:
 *      bcm_esw_bfd_auth_sha1_get
 * Purpose:
 *      Get SHA1 authentication entry
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the SHA1 entry to retrieve 
 *      sha1  - (IN/OUT) Pointer to SHA1 info structure to receive the data
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_bfd_auth_sha1_get(int unit, 
                          int index, 
                          bcm_bfd_auth_sha1_t *sha1)
{
    int result = BCM_E_UNAVAIL;

    BFD_INIT(unit);

    BFD_LOCK(unit);

    if (BCM_ESW_BFD_DRV(unit)->auth_sha1_get != NULL) {
        result = BCM_ESW_BFD_DRV(unit)->auth_sha1_get(unit, index, sha1);
    }
        
    BFD_UNLOCK(unit);

    return result;
}
/*
 * Function:
 *      bcm_esw_bfd_auth_simple_password_set
 * Purpose:
 *      Set Simple Password authentication entry
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the Simple Password entry to configure
 *      sp    - (IN) Pointer to Simple Password info structure
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_bfd_auth_simple_password_set(int unit, 
                                     int index, 
                                     bcm_bfd_auth_simple_password_t *sp)
{
    int result = BCM_E_UNAVAIL;

    BFD_INIT(unit);

    BFD_LOCK(unit);

    if (BCM_ESW_BFD_DRV(unit)->auth_simple_password_set != NULL) {
        result = BCM_ESW_BFD_DRV(unit)->auth_simple_password_set(unit,
                                                                 index, sp);
    }
        
    BFD_UNLOCK(unit);

    return result;
}

/*
 * Function:
 *      bcm_esw_bfd_auth_simple_password_get
 * Purpose:
 *      Get Simple Password authentication entry
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the Simple Password entry to retrieve
 *      sp    - (IN/OUT) Pointer to Simple Password info structure
 *               to receive the data
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_bfd_auth_simple_password_get(int unit, 
                                     int index, 
                                     bcm_bfd_auth_simple_password_t *sp)
{
    int result = BCM_E_UNAVAIL;

    BFD_INIT(unit);

    BFD_LOCK(unit);

    if (BCM_ESW_BFD_DRV(unit)->auth_simple_password_get != NULL) {
        result = BCM_ESW_BFD_DRV(unit)->auth_simple_password_get(unit,
                                                                 index, sp);
    }
        
    BFD_UNLOCK(unit);

    return result;
}

/*
 * Function:
 *      bcm_esw_bfd_status_multi_get
 * Purpose:
 *      Get the BFD status
 * Parameters:
 *      unit  - (IN) Unit number.
 *      status_arr - (OUT) Index of the BFD Status
 *      count      - (OUT) Pointer to the No of endpoints
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_esw_bfd_status_multi_get(int unit, int max_endpoints,
                             bcm_bfd_status_t *status_arr,
                             int *count)
{
    int result = BCM_E_UNAVAIL;

    BFD_INIT(unit);

    BFD_LOCK(unit);

    if (BCM_ESW_BFD_DRV(unit)->bfd_status_multi_get != NULL) {
        result = BCM_ESW_BFD_DRV(unit)->bfd_status_multi_get(unit, max_endpoints,
                                                             status_arr, count);
    }

    BFD_UNLOCK(unit);

    return result;
}

/*
 * Function:
 *      bcm_esw_bfd_discard_stat_set
 * Purpose:
 *      Reset the BFD discard statistics
 * Parameters:
 *      unit  - (IN) Unit number.
 *      discarded_info - (OUT) Pointer to discard info
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_esw_bfd_discard_stat_set(int unit,
                             bcm_bfd_discard_stat_t *discarded_info)
{
   int result = BCM_E_UNAVAIL;

    BFD_INIT(unit);

    BFD_LOCK(unit);

    if (BCM_ESW_BFD_DRV(unit)->bfd_discard_stat_set != NULL) {
        result = BCM_ESW_BFD_DRV(unit)->bfd_discard_stat_set(unit, discarded_info);
    }

    BFD_UNLOCK(unit);

    return result;
}

/*
 * Function:
 *      bcm_esw_bfd_discard_stat_get
 * Purpose:
 *      Get the BFD discard statistics
 * Parameters:
 *      unit  - (IN) Unit number.
 *      discarded_info - (OUT) Pointer to discard info
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_esw_bfd_discard_stat_get(int unit,
                             bcm_bfd_discard_stat_t *discarded_info)
{
   int result = BCM_E_UNAVAIL;

    BFD_INIT(unit);

    BFD_LOCK(unit);

    if (BCM_ESW_BFD_DRV(unit)->bfd_discard_stat_get != NULL) {
        result = BCM_ESW_BFD_DRV(unit)->bfd_discard_stat_get(unit, discarded_info);
    }

    BFD_UNLOCK(unit);

    return result;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_esw_bfd_sync
 * Purpose:
 *      Warm boot BFD sync 
 * Parameters:
 *      unit - (IN) Unit number.
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_esw_bfd_sync(int unit)
{
    int result = BCM_E_UNAVAIL;
    BFD_INIT(unit);

    BFD_LOCK(unit);
    if (BCM_ESW_BFD_DRV(unit)->bfd_sync != NULL) {
        result = BCM_ESW_BFD_DRV(unit)->bfd_sync(unit);
    }
    BFD_UNLOCK(unit);
    return result;
}

/*
 * Function:
 *      _bcm_esw_bfd_wb_downgrade_config_set
 * Purpose:
 *       BFD warmboot Downgrade configuration set
 * Parameters:
 *      unit - (IN) Unit number.
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_esw_bfd_wb_downgrade_config_set(int unit, uint32 warmboot_ver)
{
    int result = BCM_E_UNAVAIL;
    BFD_INIT(unit);

    BFD_LOCK(unit);
    if (BCM_ESW_BFD_DRV(unit)->_bfd_wb_downgrade_config_set != NULL) {
        result = BCM_ESW_BFD_DRV(unit)->_bfd_wb_downgrade_config_set(unit, warmboot_ver);
    }
    BFD_UNLOCK(unit);
    return result;
}
#endif /* BCM_WARM_BOOT_SUPPORT */ 

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *     _bcm_bfd_sw_dump
 * Purpose:
 *     Displays BFD information 
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_bfd_sw_dump(int unit)
{
    if (!soc_feature(unit, soc_feature_bfd) ||
        (BCM_ESW_BFD_DRV(unit)== NULL) || (mutex[unit] == NULL)) {
        return;
    }

    if ((BCM_ESW_BFD_DRV(unit) == NULL) || (mutex[unit] == NULL)) {
        LOG_CLI((BSL_META_U(unit, "BFD was not initialized.\n")));
        return;
    }
    BFD_LOCK(unit);

    if (BCM_ESW_BFD_DRV(unit)->bfd_sw_dump != NULL) {
        BCM_ESW_BFD_DRV(unit)->bfd_sw_dump(unit);
    }

    BFD_UNLOCK(unit);
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */ 

/*
 * Function:
 *      _bcm_bfd_dump_trace
 * Purpose:
 *      Dump the BFD stack trace
 * Parameters:
 *      unit  - (IN) Unit number.
 * Returns:
 *      None
 * Notes:
 */
void _bcm_bfd_dump_trace(int unit)
{
    if (!soc_feature(unit, soc_feature_bfd)) {
        LOG_CLI((BSL_META_U(unit, "BFD not supported.\n")));
        return;
    }

    if ((BCM_ESW_BFD_DRV(unit) == NULL) || (mutex[unit] == NULL)) {
        LOG_CLI((BSL_META_U(unit, "BFD not initialized.\n")));
        return;
    }
    BFD_LOCK(unit);

    if (BCM_ESW_BFD_DRV(unit)->bfd_dump_trace != NULL) {
        BCM_ESW_BFD_DRV(unit)->bfd_dump_trace(unit);
    }

    BFD_UNLOCK(unit);
}

#else /* INCLUDE_BFD */
typedef int bcm_esw_bfd_not_empty; /* Make ISO compilers happy. */
#endif  /* INCLUDE_BFD */
