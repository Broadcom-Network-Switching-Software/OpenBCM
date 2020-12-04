/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/bslenum.h>
#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/telemetry.h>
#include <bcm/collector.h>

#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/telemetry.h>

static sal_mutex_t mutex[BCM_MAX_NUM_UNITS];

int bcm_esw_telemetry_lock(int unit)
{
    if (mutex[unit] == NULL)
    {
        return BCM_E_INIT;
    }

    sal_mutex_take(mutex[unit], sal_mutex_FOREVER);

    return BCM_E_NONE;
}

int bcm_esw_telemetry_unlock(int unit)
{
    if (mutex[unit] == NULL)
    {
        return BCM_E_INIT;
    }

    if (sal_mutex_give(mutex[unit]) != 0)
    {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_telemetry_init
 * Purpose:
 *      Initialize the Telemetry module.
 * Parameters:
 *      unit         - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_telemetry_init(int unit)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_TELEMETRY)
    if (soc_feature(unit, soc_feature_telemetry)) {
        /* Create mutex */
        if (mutex[unit] == NULL)
        {
            mutex[unit] = sal_mutex_create("telemetry.mutex");

            if (mutex[unit] == NULL)
            {
                return BCM_E_MEMORY;
            }
        }
        result = _bcm_xgs5_telemetry_init(unit);

        /* If init itself fails, there is no point in having the mutex.
         * Destroy it.
         */
        if (BCM_FAILURE(result))
        {
            sal_mutex_destroy(mutex[unit]);
            mutex[unit] = NULL;
        }
    }
#endif /* INCLUDE_TELEMETRY */
    return result;
}

/* Shut down the Telemetry module . */
int bcm_esw_telemetry_detach(int unit)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_TELEMETRY)
    if (soc_feature(unit, soc_feature_telemetry)) {
        /* Take lock */
        BCM_IF_ERROR_RETURN(bcm_esw_telemetry_lock(unit));

        result = _bcm_xgs5_telemetry_detach(unit);
        /* Release lock */
        BCM_IF_ERROR_RETURN(bcm_esw_telemetry_unlock(unit));
    }
#endif /* INCLUDE_TELEMETRY */
    return result;
}

/*
 * Function:
 *      bcm_esw_telemetry_config_set
 * Purpose:
 *      Configures a Telemetry application instance with specified
 *      number of telemetry objects information.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      telemetry_instance  - (IN) The ID of telemetry instance to be created
 *      core                - (IN) FW core on which this instance need to be run
 *      config_count        - (IN)  Number of telemetry objects to be
 *                                     associated with this instance.
 *      telemetry_config_list  - (IN)  Telemetry objects information
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_telemetry_config_set(
        int unit,
        int telemetry_instance,
        int core,
        int config_count,
        bcm_telemetry_config_t *telemetry_config_list)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_TELEMETRY)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_telemetry_lock(unit));

    if (soc_feature(unit, soc_feature_telemetry)) {
        result = _bcm_xgs5_telemetry_instance_config_update(unit,
                telemetry_instance,
                core, config_count,
                telemetry_config_list);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_telemetry_unlock(unit));
#endif /* INCLUDE_TELEMETRY */
    return result;
}

/*
 * Function:
 *      bcm_esw_telemetry_config_get
 * Purpose:
 *      Fetches a Telemetry application instance information.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      telemetry_instance  - (IN) The ID of telemetry instance to be fetched
 *      core                - (IN) FW core on which this instance is being run
 *      max_count           - (IN) Maximum number of telemetry objects need 
 *                                 to be fetched
 *      telemetry_config_list - (OUT)  Telemetry objects information
 *      config_count        - (OUT) Actual number of telemetry objects to be
 *                                 associated with this instance.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_telemetry_config_get(
        int unit,
        int telemetry_instance,
        int core,
        int max_count,
        bcm_telemetry_config_t *telemetry_config_list,
        int *config_count)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_TELEMETRY)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_telemetry_lock(unit));

    if (soc_feature(unit, soc_feature_telemetry)) {
        result = _bcm_xgs5_telemetry_instance_config_get(unit,
                telemetry_instance,
                core, max_count,
                telemetry_config_list,
                config_count);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_telemetry_unlock(unit));
#endif /* INCLUDE_TELEMETRY */
    return result;
}

/*
 * Function:
 *      bcm_esw_telemetry_export_config_set
 * Purpose:
 *      Configures the Telemetry instance export information.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      telemetry_instance  - (IN) The ID of telemetry instance to be associated
 *      collector_id        - (IN) Colletor ID with which telemery instance
 *                                 needs to be associated
 *      export_profile_id   - (IN) Colletor export profile ID with which
 *                                 telemery instance needs to be associated
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_telemetry_export_config_set(
        int unit,
        int telemetry_instance,
        bcm_collector_t collector_id,
        int export_profile_id)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_TELEMETRY)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_telemetry_lock(unit));

    if (soc_feature(unit, soc_feature_telemetry)) {
        result = _bcm_xgs5_telemetry_instance_export_config_update(unit,
                telemetry_instance,
                collector_id,
                export_profile_id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_telemetry_unlock(unit));
#endif /* INCLUDE_TELEMETRY */
    return result;
}

/*
 * Function:
 *      bcm_esw_telemetry_export_config_get
 * Purpose:
 *      Fetches the Telemetry instance export information.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      telemetry_instance  - (IN) The ID of telemetry instance whose details
 *                                 needs to be fetched
 *      collector_id        - (OUT) Pointer to colletor ID with which telemery
 *                                  instance has been associated
 *      export_profile_id   - (OUT) Pointer to colletor export profile ID with
 *                                  which telemery instance has been associated
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_telemetry_export_config_get(
        int unit,
        int telemetry_instance,
        bcm_collector_t *collector_id,
        int *export_profile_id)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_TELEMETRY)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_telemetry_lock(unit));

    if (soc_feature(unit, soc_feature_telemetry)) {
        result = _bcm_xgs5_telemetry_instance_export_config_get(unit,
                telemetry_instance,
                collector_id,
                export_profile_id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_telemetry_unlock(unit));
#endif /* INCLUDE_TELEMETRY */
    return result;
}

/*
 * Function:
 *      bcm_esw_telemetry_export_config_delete
 * Purpose:
 *      Deletes the Telemetry instance export information.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      telemetry_instance  - (IN) The ID of telemetry instance whose
 *                                 association needs to be deleted
 *      collector_id        - (OUT) Pointer to colletor ID with which telemery
 *                                  instance has been associated
 *      export_profile_id   - (OUT) Pointer to colletor export profile ID with
 *                                  which telemery instance has been associated
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_telemetry_export_config_delete(
        int unit,
        int telemetry_instance,
        bcm_collector_t collector_id,
        int export_profile_id)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_TELEMETRY)
    bcm_collector_t coll_id;
    int profile_id;

    /* Check if the instance is mapped to the correct collector and export
     * profile
     */
    BCM_IF_ERROR_RETURN(
             bcm_esw_telemetry_export_config_get(unit, telemetry_instance,
                                                 &coll_id, &profile_id));
    if ((collector_id != coll_id) || (export_profile_id != profile_id)) {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                  (BSL_META_U(unit,
                                "TELEMETRY(unit %d) Error: Invalid collector_id(%d)"
                                " or export_profile_id(%d) \n"),
                   unit, collector_id, export_profile_id));
        return BCM_E_NOT_FOUND;
    }

    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_telemetry_lock(unit));

    if (soc_feature(unit, soc_feature_telemetry)) {
        result = _bcm_xgs5_telemetry_instance_export_config_update(unit,
                telemetry_instance,
                BCM_TELEMETRY_INVALID_INDEX,
                BCM_TELEMETRY_INVALID_INDEX);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_telemetry_unlock(unit));
#endif /* INCLUDE_TELEMETRY */
    return result;
}

/*
 * Function:
 *      bcm_esw_telemetry_config_set
 * Purpose:
 *      Configures Telemetry system ID information.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      system_id_len       - (IN) length of telemetry system ID
 *      system_id           - (IN) System ID value
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_telemetry_system_id_set(
        int unit, int system_id_len,
        uint8 *system_id)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_TELEMETRY)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_telemetry_lock(unit));

    if (soc_feature(unit, soc_feature_telemetry)) {
        result = _bcm_xgs5_telemetry_system_id_set(unit,
                system_id_len,
                system_id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_telemetry_unlock(unit));
#endif /* INCLUDE_TELEMETRY */
    return result;
}

/*
 * Function:
 *      bcm_esw_telemetry_system_id_get
 * Purpose:
 *      Fetches Telemetry system ID information.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      max_system_id_len   - (IN) Max length of telemetry system ID
 *      system_id_len       - (IN) System ID configured length
 *      system_id           - (IN) System ID value
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_telemetry_system_id_get(
        int unit, int max_system_id_len,
        int *system_id_len,
        uint8 *system_id)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_TELEMETRY)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_telemetry_lock(unit));

    if (soc_feature(unit, soc_feature_telemetry)) {
        result = _bcm_xgs5_telemetry_system_id_get(unit,
                max_system_id_len,
                system_id_len,
                system_id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_telemetry_unlock(unit));
#endif /* INCLUDE_TELEMETRY */
    return result;
}

/*
 * Function:
 *      bcm_esw_telemetry_instance_export_stats_get
 * Purpose:
 *      Get the telemetry export statistics.
 * Parameters:
 *      unit                - (IN)  BCM device number
 *      telemetry_instance  - (IN)  Telemetry instance ID
 *      collector_id        - (IN)  Collector ID
 *      stats               - (OUT) Export statistics.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_telemetry_instance_export_stats_get(
                                    int unit,
                                    int telemetry_instance,
                                    bcm_collector_t collector_id,
                                    bcm_telemetry_instance_export_stats_t *stats)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_TELEMETRY)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_telemetry_lock(unit));

    if (soc_feature(unit, soc_feature_telemetry)) {
        result = bcm_xgs5_telemetry_instance_export_stats_get(unit,
                                                              telemetry_instance,
                                                              collector_id,
                                                              stats);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_telemetry_unlock(unit));
#endif /* INCLUDE_TELEMETRY */
    return result;
}

/*
 * Function:
 *      bcm_esw_telemetry_instance_export_stats_set
 * Purpose:
 *      Set the telemetry export statistics.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      telemetry_instance  - (IN) Telemetry instance ID
 *      collector_id        - (IN) Collector ID
 *      stats               - (IN) Export statistics.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_telemetry_instance_export_stats_set(
                                    int unit,
                                    int telemetry_instance,
                                    bcm_collector_t collector_id,
                                    bcm_telemetry_instance_export_stats_t *stats)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_TELEMETRY)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_telemetry_lock(unit));

    if (soc_feature(unit, soc_feature_telemetry)) {
        result = bcm_xgs5_telemetry_instance_export_stats_set(unit,
                                                              telemetry_instance,
                                                              collector_id,
                                                              stats);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_telemetry_unlock(unit));
#endif /* INCLUDE_TELEMETRY */
    return result;
}

/*
 * Function:
 *      _bcm_esw_telemetry_sw_dump
 * Purpose:
 *      Dumps telemetry module information
 * Parameters:
 *      unit           - (IN)  BCM device number
 * Returns: void
 */
void _bcm_esw_telemetry_sw_dump(int unit)
{
#if defined(INCLUDE_TELEMETRY)

    if (soc_feature(unit, soc_feature_telemetry)) {
        _bcm_xgs5_telemetry_sw_dump(unit);
    }
#endif /* INCLUDE_COLLECTOR */

}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_esw_telemetry_sync
 * Purpose:
 *      Warmboot scache sync for telemetry module
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_esw_telemetry_sync(int unit)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_TELEMETRY)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_telemetry_lock(unit));

    if (soc_feature(unit, soc_feature_telemetry)) {
        result = _bcm_xgs5_telemetry_sync(unit);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_telemetry_unlock(unit));
#endif /* INCLUDE_TELEMETRY */
    return result;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

