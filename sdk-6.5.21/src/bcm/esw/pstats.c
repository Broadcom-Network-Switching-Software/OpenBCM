/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Packetized Statistic
 * Purpose: API to set Packetized Statistic registers.
 */

#if defined(INCLUDE_PSTATS)

#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>

#include <bcm/error.h>
#include <bcm/pstats.h>

#include <bcm_int/esw/pstats.h>
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw/tomahawk2.h>
#if defined(BCM_HURRICANE4_SUPPORT)
#include <bcm_int/esw/hurricane4.h>
#endif
#include <bcm_int/esw/tomahawk3.h>
#include <bcm_int/esw_dispatch.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

_bcm_pstats_unit_driver_t *_bcm_pstats_unit_driver[BCM_MAX_NUM_UNITS];


/*
 * Function:
 *      _bcm_pstats_deinit
 * Purpose:
 *      Deinitialize PSTATS driver functions.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
void _bcm_pstats_deinit(int unit)
{
    _bcm_pstats_unit_driver_t   *pstats_driver;

    pstats_driver = _BCM_PSTATS_UNIT_DRIVER(unit);
    if (pstats_driver == NULL) {
        return;
    }

#if defined(BCM_HURRICANE4_SUPPORT)
    if (SOC_IS_HURRICANE4(unit)) {
        (void )bcm_hr4_pstats_deinit(unit);
    } else
#endif

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        (void )bcm_td3_pstats_deinit(unit);
    } else
#endif

#if defined(BCM_TOMAHAWK2_SUPPORT)
    if (SOC_IS_TOMAHAWK2(unit)) {
        (void )bcm_th2_pstats_deinit(unit);
    } else
#endif
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        (void )bcm_th3_pstats_deinit(unit);
    }
#endif

    if (NULL != pstats_driver->pstats_lock) {
        sal_mutex_destroy(pstats_driver->pstats_lock);
    }

    if (NULL != pstats_driver) {
        sal_free(pstats_driver);
    }
    _BCM_PSTATS_UNIT_DRIVER(unit) = NULL;

    return;
}


/*
 * Function:
 *      _bcm_pstats_init
 * Purpose:
 *      Initialize PSTATS driver functions.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_pstats_init(int unit)
{
    _bcm_pstats_unit_driver_t   *pstats_driver;
    int                         pstats_supported = 0;
    int                         rv = BCM_E_NONE;
    _bcm_pstats_deinit(unit);
    pstats_driver = _BCM_PSTATS_UNIT_DRIVER(unit);

    if (NULL == pstats_driver) {
        pstats_driver = sal_alloc(sizeof(_bcm_pstats_unit_driver_t), "pstats_unit_driver");
        if (pstats_driver == NULL) {
            return BCM_E_MEMORY;
        }
    }

    sal_memset(pstats_driver, 0, sizeof(_bcm_pstats_unit_driver_t));

    _BCM_PSTATS_UNIT_DRIVER(unit) = pstats_driver;

    if (NULL == pstats_driver->pstats_lock) {
        pstats_driver->pstats_lock = sal_mutex_create("pstats_lock");
    }

    if (NULL == pstats_driver->pstats_lock) {
        (void)_bcm_pstats_deinit(unit);
        return BCM_E_MEMORY;
    }

#if defined(BCM_HURRICANE4_SUPPORT)
    if (SOC_IS_HURRICANE4(unit)) {
        rv = bcm_hr4_pstats_init(unit);
        if (BCM_FAILURE(rv)) {
            (void)_bcm_pstats_deinit(unit);
            return rv;
        }
        pstats_supported = 1;
    } else
#endif

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        rv = bcm_td3_pstats_init(unit);
        if (BCM_FAILURE(rv)) {
            (void)_bcm_pstats_deinit(unit);
            return rv;
        }
        pstats_supported = 1;
    } else
#endif

#if defined(BCM_TOMAHAWK2_SUPPORT)
    if (SOC_IS_TOMAHAWK2(unit)) {
        rv = bcm_th2_pstats_init(unit);
        if (BCM_FAILURE(rv)) {
            (void)_bcm_pstats_deinit(unit);
            return rv;
        }
        pstats_supported = 1;
    } else
#endif

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        rv = bcm_th3_pstats_init(unit);
        if (BCM_FAILURE(rv)) {
            (void)_bcm_pstats_deinit(unit);
            return rv;
        }
        pstats_supported = 1;
    }
#endif

    if (pstats_supported == 0) {
        return BCM_E_UNAVAIL;
    }
    return rv;
}
/*
 * Function:
 *      bcm_esw_pstats_session_create
 * Purpose:
 *      Create a PSTATS session instance for SDK management.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      options - (IN) BCM_PSTATS_OPTIONS_REPLACE: replace existing.
 *                     BCM_PSTATS_OPTIONS_WITH_ID: session_id argument is given.
 *      array_count - (IN) Number of elements in array.
 *      element_array - (IN) PSTATS session element array.
 *      session_id - (INOUT) id of a PSTATS session created via this API.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_pstats_session_create(int unit, int options, int array_count,
                              bcm_pstats_session_element_t  *element_array,
                              bcm_pstats_session_id_t *session_id)
{
    int rv = BCM_E_NONE;
    if (!soc_feature(unit, soc_feature_pstats)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_pstats_unit_driver[unit]->session_create == NULL) {
        return BCM_E_UNAVAIL;
    }

    PSTATS_LOCK(unit);
    rv = _bcm_pstats_unit_driver[unit]->session_create(unit, options,
                                                       array_count,
                                                       element_array,
                                                       session_id);
    PSTATS_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_esw_pstats_session_destroy
 * Purpose:
 *      Destroy an existing PSTATS session.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      session_id - (IN) id of a PSTATS session created via the
 *                        bcm_pstats_session_create
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_pstats_session_destroy(int unit,
                               bcm_pstats_session_id_t session_id)
{
    int rv = BCM_E_NONE;
    if (!soc_feature(unit, soc_feature_pstats)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_pstats_unit_driver[unit]->session_destroy == NULL) {
        return BCM_E_UNAVAIL;
    }

    PSTATS_LOCK(unit);
    rv = _bcm_pstats_unit_driver[unit]->session_destroy(unit, session_id);
    PSTATS_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_esw_pstats_session_get
 * Purpose:
 *      Get an existing PSTATS session info.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      session_id - (IN) id of a PSTATS session created via the
 *                        bcm_pstats_session_create
 *      array_max - (IN) Maximum number of elements in array.
 *      element_array - (OUT) session elemnts array of a PSTATS session.
 *      array_count - (OUT) Number of elements in array.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_pstats_session_get(int unit,
                           bcm_pstats_session_id_t session_id,
                           int array_max,
                           bcm_pstats_session_element_t *element_array,
                           int *array_count)
{
    int rv = BCM_E_NONE;
    if (!soc_feature(unit, soc_feature_pstats)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_pstats_unit_driver[unit]->session_get == NULL) {
        return BCM_E_UNAVAIL;
    }

    PSTATS_LOCK(unit);
    rv = _bcm_pstats_unit_driver[unit]->session_get(unit, session_id, array_max,
                                                    element_array,
                                                    array_count);
    PSTATS_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_esw_pstats_data_sync
 * Purpose:
 *      Start a pstats sync operation.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_pstats_data_sync(int unit)
{
    int rv = BCM_E_NONE;
    if (!soc_feature(unit, soc_feature_pstats)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_pstats_unit_driver[unit]->data_sync == NULL) {
        return BCM_E_UNAVAIL;
    }

    PSTATS_LOCK(unit);
    rv = _bcm_pstats_unit_driver[unit]->data_sync(unit);
    PSTATS_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_esw_pstats_session_data_get
 * Purpose:
 *      Get an existing PSTATS session data.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      session_id - (IN) id of a PSTATS session created via the
 *                        bcm_pstats_session_create
 *      timestamp - (OUT) time stamp info.
 *      array_max - (IN) Maximum number of elements in array.
 *      data - (OUT) array of use count data.
 *      array_count - (OUT) Number of elements in array.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_pstats_session_data_get(int unit, bcm_pstats_session_id_t session_id,
                                bcm_pstats_timestamp_t *timestamp,
                                int array_max,
                                bcm_pstats_data_t *data_array,
                                int *array_count)
{
    int rv = BCM_E_NONE;
    if (!soc_feature(unit, soc_feature_pstats)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_pstats_unit_driver[unit]->session_data_get == NULL) {
        return BCM_E_UNAVAIL;
    }

    PSTATS_LOCK(unit);
    rv = _bcm_pstats_unit_driver[unit]->session_data_get(unit, session_id,
                                                         FALSE,
                                                         timestamp,
                                                         array_max,
                                                         data_array,
                                                         array_count);
    PSTATS_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_esw_pstats_session_data_sync_get
 * Purpose:
 *      Get an existing PSTATS session data.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      session_id - (IN) id of a PSTATS session created via the
 *                        bcm_pstats_session_create
 *      timestamp - (OUT) time stamp info.
 *      array_max - (IN) Maximum number of elements in array.
 *      data - (OUT) array of use count data.
 *      array_count - (OUT) Number of elements in array.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_pstats_session_data_sync_get(int unit, bcm_pstats_session_id_t session_id,
                                     bcm_pstats_timestamp_t *timestamp,
                                     int array_max,
                                     bcm_pstats_data_t *data_array,
                                     int *array_count)
{
    int rv = BCM_E_NONE;
    if (!soc_feature(unit, soc_feature_pstats)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_pstats_unit_driver[unit]->session_data_get == NULL) {
        return BCM_E_UNAVAIL;
    }

    PSTATS_LOCK(unit);
    rv = _bcm_pstats_unit_driver[unit]->session_data_get(unit, session_id,
                                                         TRUE,
                                                         timestamp,
                                                         array_max,
                                                         data_array,
                                                         array_count);
    PSTATS_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_pstats_session_data_clear
 * Purpose:
 *      Clear an existing PSTATS session hw use counts and sw use counts buffer.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      session_id - (IN) id of a PSTATS session created via the
 *                        bcm_pstats_session_create.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_pstats_session_data_clear(int unit, bcm_pstats_session_id_t session_id)
{
    int rv = BCM_E_NONE;
    if (!soc_feature(unit, soc_feature_pstats)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_pstats_unit_driver[unit]->session_data_clear == NULL) {
        return BCM_E_UNAVAIL;
    }

    PSTATS_LOCK(unit);
    rv = _bcm_pstats_unit_driver[unit]->session_data_clear(unit, session_id);
    PSTATS_UNLOCK(unit);
    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_esw_pstats_scache_size_get
 * Purpose:
 *      Get scache size of PSTATS module persisitent info for Level 2 Warm Boot.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      scache_size - scache size.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_pstats_scache_size_get(int unit, uint32 *scache_size)
{
    if (!soc_feature(unit, soc_feature_pstats)) {
        return BCM_E_UNAVAIL;
    }
    if (_bcm_pstats_unit_driver[unit]->scache_size_get == NULL) {
        return BCM_E_UNAVAIL;
    }
    return _bcm_pstats_unit_driver[unit]->scache_size_get(unit, scache_size);
}
/*
 * Function:
 *      _bcm_esw_pstats_reinit
 * Purpose:
 *      Reinit PSTATS module persisitent info for Level 2 Warm Boot.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      scache_ptr - scache pointer
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_pstats_reinit(int unit, uint8 **scache_ptr)
{
    if (!soc_feature(unit, soc_feature_pstats)) {
        return BCM_E_UNAVAIL;
    }
    if (_bcm_pstats_unit_driver[unit]->pstats_reinit == NULL) {
        return BCM_E_UNAVAIL;
    }
    return _bcm_pstats_unit_driver[unit]->pstats_reinit(unit, scache_ptr);
}
/*
 * Function:
 *      _bcm_esw_pstats_sync
 * Purpose:
 *      Record PSTATS module persisitent info for Level 2 Warm Boot.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_pstats_sync(int unit, uint8 **scache_ptr)
{
    if (!soc_feature(unit, soc_feature_pstats)) {
        return BCM_E_UNAVAIL;
    }
    if (_bcm_pstats_unit_driver[unit]->pstats_sync == NULL) {
        return BCM_E_UNAVAIL;
    }
    return _bcm_pstats_unit_driver[unit]->pstats_sync(unit, scache_ptr);
}
#endif

/*
 * Function:
 *      bcm_esw_pstats_session_traverse
 * Purpose:
 *      Traverse through the pstats sessions and run callback at each session.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      cb - (IN) bcm_pstats_session_traverse_cb.
 *      user_data - (IN) User data to be passed to callback function.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_pstats_session_traverse(int unit, bcm_pstats_session_traverse_cb cb,
                            void *user_data)
{
    int rv = BCM_E_NONE;
    if (!soc_feature(unit, soc_feature_pstats)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_pstats_unit_driver[unit]->session_traverse == NULL) {
        return BCM_E_UNAVAIL;
    }

    PSTATS_LOCK(unit);
    rv = _bcm_pstats_unit_driver[unit]->session_traverse(unit, cb, user_data);
    PSTATS_UNLOCK(unit);
    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *     bcm_esw_pstats_sw_dump
 * Purpose:
 *     Displays PSTATS information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_esw_pstats_sw_dump(int unit)
{
    if (!soc_feature(unit, soc_feature_pstats)) {
        return;
    }
    if (_bcm_pstats_unit_driver[unit]->pstats_sw_dump == NULL) {
        return;
    }
    return _bcm_pstats_unit_driver[unit]->pstats_sw_dump(unit);
}
#endif

#else  /* INCLUDE_PSTATS */
typedef int bcm_esw_pstats_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PSTATS */
