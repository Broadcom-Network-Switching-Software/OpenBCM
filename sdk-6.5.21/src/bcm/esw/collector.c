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
#include <bcm/collector.h>

#include <bcm_int/esw/collector.h>

static sal_mutex_t mutex[BCM_MAX_NUM_UNITS];

int bcm_esw_collector_lock(int unit)
{
    if (mutex[unit] == NULL)
    {
        return BCM_E_INIT;
    }

    sal_mutex_take(mutex[unit], sal_mutex_FOREVER);

    return BCM_E_NONE;
}

int bcm_esw_collector_unlock(int unit)
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
 *      bcm_esw_collector_init
 * Purpose:
 *      Initialize the Collector module.
 * Parameters:
 *      unit         - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_collector_init(int unit)
{

    int result = BCM_E_UNAVAIL;
#if defined(BCM_COLLECTOR_SUPPORT)
    if (soc_feature(unit, soc_feature_collector)) {
        /* Create mutex */
        if (mutex[unit] == NULL)
        {
            mutex[unit] = sal_mutex_create("collector.mutex");

            if (mutex[unit] == NULL)
            {
                return BCM_E_MEMORY;
            }
        }
        result = _bcm_xgs5_collector_init(unit);
         /* If init itself fails, there is no point in having the mutex.
         * Destroy it.
         */
        if (BCM_FAILURE(result))
        {
            sal_mutex_destroy(mutex[unit]);

            mutex[unit] = NULL;
        }
    }
#endif /* BCM_COLLECTOR_SUPPORT */

    return result;
}

/* Shut down the Collector module . */
int bcm_esw_collector_detach(int unit)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_COLLECTOR_SUPPORT)
    if (soc_feature(unit, soc_feature_collector)) {
        /* Take lock */
        BCM_IF_ERROR_RETURN(bcm_esw_collector_lock(unit));

        result = _bcm_xgs5_collector_detach(unit);
        /* Release lock */
        BCM_IF_ERROR_RETURN(bcm_esw_collector_unlock(unit));
    }
#endif /* BCM_COLLECTOR_SUPPORT */
    
    return result;
}

/*
 * Function:
 *      bcm_esw_collector_export_profile_create
 * Purpose:
 *      Creates a collector export profile with given export profile info.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      options         - (IN)  Create options
 *      export_profile  - (INOUT)  Export profile Id
 *      export_profile_info  - (IN)  Export profile info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_collector_export_profile_create(
        int unit,
        uint32 options,
        int *export_profile,
        bcm_collector_export_profile_t *export_profile_info)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_COLLECTOR_SUPPORT)
   /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_lock(unit));

    if (soc_feature(unit, soc_feature_collector)) {
        result = _bcm_xgs5_collector_export_profile_create(unit, options,
                export_profile,
                export_profile_info);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_unlock(unit));
#endif /* BCM_COLLECTOR_SUPPORT */
    return result;
}

/*
 * Function:
 *      bcm_esw_collector_export_profile_get
 * Purpose:
 *      Gets collector export profile information
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      id              - (IN)  Export profile Id
 *      export_profile_info  - (OUT)  Export profile info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_collector_export_profile_get(
        int unit,
        int export_profile,
        bcm_collector_export_profile_t *export_profile_info)
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_COLLECTOR_SUPPORT)
   /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_lock(unit));

    if (soc_feature(unit, soc_feature_collector)) {
        result = _bcm_xgs5_collector_export_profile_get(unit,
                export_profile,
                export_profile_info);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_unlock(unit));
#endif /* BCM_COLLECTOR_SUPPORT */
    return result;
}

/*
 * Function:
 *      bcm_esw_collector_export_profile_ids_get_all
 * Purpose:
 *      Gets the list of all collector export profile Ids configured.
 * Parameters:
 *      unit                 - (IN)  BCM device number
 *      max_size             - (IN)  Size of the collector export profile IDs
 *      export_profile_ids_list - (OUT) List of collector export profile Ids configured
 *      list_size            - (OUT) NUmber of elements in the above list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_collector_export_profile_ids_get_all(
                                     int unit,
                                     int max_size,
                                     int *export_profile_ids_list,
                                     int *list_size)
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_COLLECTOR_SUPPORT)
   /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_lock(unit));

    if (soc_feature(unit, soc_feature_collector)) {
        result = _bcm_xgs5_collector_export_profile_ids_get_all(unit,
                max_size,
                export_profile_ids_list,
                list_size);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_unlock(unit));
#endif /* BCM_COLLECTOR_SUPPORT */
    return result;
}

/*
 * Function:
 *      bcm_esw_collector_export_profile_destroy
 * Purpose:
 *      Destroys a collector export profile
 * Parameters:
 *      unit           - (IN)  BCM device number
 *      export_profile_id - (IN) Collector export profile Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_collector_export_profile_destroy(
        int unit,
        int export_profile_id)
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_COLLECTOR_SUPPORT)
   /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_lock(unit));

    if (soc_feature(unit, soc_feature_collector)) {
        result = _bcm_xgs5_collector_export_profile_destroy(unit,
                export_profile_id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_unlock(unit));
#endif /* BCM_COLLECTOR_SUPPORT */
    return result;
}

/*
 * Function:
 *      bcm_esw_collector_create
 * Purpose:
 *      Creates a collector with given collector info.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      options         - (IN)  Collector create options
 *      id              - (INOUT)  Collector Id
 *      collector_info  - (IN)  Collector info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_collector_create(
        int unit,
        uint32 options,
        bcm_collector_t *collector_id,
        bcm_collector_info_t *collector_info)
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_COLLECTOR_SUPPORT)
   /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_lock(unit));

    if (soc_feature(unit, soc_feature_collector)) {
        result = _bcm_xgs5_collector_create(unit, options, collector_id,
                collector_info);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_unlock(unit));
#endif /* BCM_COLLECTOR_SUPPORT */

    return result;
}

/*
 * Function:
 *      bcm_esw_collector_get
 * Purpose:
 *      Gets collector information
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      collector_id    - (IN)  Collector Id
 *      collector_info  - (OUT) Collector info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_collector_get(
        int unit,
        bcm_collector_t collector_id,
        bcm_collector_info_t *collector_info)
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_COLLECTOR_SUPPORT)
   /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_lock(unit));

    if (soc_feature(unit, soc_feature_collector)) {
        result = _bcm_xgs5_collector_get(unit, collector_id,
                collector_info);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_unlock(unit));
#endif /* BCM_COLLECTOR_SUPPORT */

    return result;
}

/*
 * Function:
 *      bcm_esw_collector_get_all
 * Purpose:
 *      Gets the list of all collector Ids configured.
 * Parameters:
 *      unit                 - (IN)  BCM device number
 *      max_size             - (IN)  Size of the collector list array
 *      collector_list       - (OUT) List of collector Ids configured
 *      list_size            - (OUT) NUmber of elements in the list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_collector_get_all(
                                     int unit,
                                     int max_size,
                                     bcm_collector_t *collector_list,
                                     int *list_size)
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_COLLECTOR_SUPPORT)
   /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_lock(unit));

    if (soc_feature(unit, soc_feature_collector)) {
        result = _bcm_xgs5_collector_ids_get_all(unit, max_size,
                collector_list, list_size);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_unlock(unit));
#endif /* BCM_COLLECTOR_SUPPORT */
    return result;
}

/*
 * Function:
 *      bcm_esw_collector_destroy
 * Purpose:
 *      Destroys a collector
 * Parameters:
 *      unit           - (IN)  BCM device number
 *      collector_id   - (IN) Collector Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_collector_destroy(
        int unit,
        bcm_collector_t collector_id)
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_COLLECTOR_SUPPORT)
   /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_lock(unit));

    if (soc_feature(unit, soc_feature_collector)) {
        result = _bcm_xgs5_collector_destroy(unit, collector_id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_unlock(unit));
#endif /* BCM_COLLECTOR_SUPPORT */

    return result;
}

/*
 * Function:
 *     _bcm_esw_collector_export_profile_ref_count_update
 * Purpose:
 *     Increment/Decrement reference count for an export profile
 * Parameters:
 *     unit          - (IN) BCM device number
 *     id            - (IN) Export profile Id
 *     update        - (IN) Reference count update (+/-)
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_esw_collector_export_profile_ref_count_update(int unit, int id, int update)
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_COLLECTOR_SUPPORT)
   /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_lock(unit));

    if (soc_feature(unit, soc_feature_collector)) {
        result = _bcm_xgs5_collector_export_profile_ref_count_update(unit, id,
                                                                    update);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_unlock(unit));
#endif /* BCM_COLLECTOR_SUPPORT */

    return result;
}

/*
 * Function:
 *     _bcm_esw_collector_ref_count_update
 * Purpose:
 *     Increment/Decrement reference count for a collector
 * Parameters:
 *     unit          - (IN) BCM device number
 *     id            - (IN) Collector Id
 *     update        - (IN) Reference count update (+/-)
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_esw_collector_ref_count_update(int unit, bcm_collector_t id, int update)
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_COLLECTOR_SUPPORT)
   /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_lock(unit));

    if (soc_feature(unit, soc_feature_collector)) {
        result = _bcm_xgs5_collector_ref_count_update(unit, id, update);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_unlock(unit));
#endif /* BCM_COLLECTOR_SUPPORT */

    return result;
}

/*
 * Function:
 *     _bcm_esw_collector_ref_count_get
 * Purpose:
 *     Get reference count for a collector
 * Parameters:
 *     unit          - (IN) BCM device number
 *     id            - (IN) Collector Id
 * Returns:
 *     reference count value
 */
int
_bcm_esw_collector_ref_count_get(int unit, bcm_collector_t id)
{
    int result = 0;
#if defined(BCM_COLLECTOR_SUPPORT)
   /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_lock(unit));

    if (soc_feature(unit, soc_feature_collector)) {
        result = _bcm_xgs5_collector_ref_count_get(unit, id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_unlock(unit));
#endif /* BCM_COLLECTOR_SUPPORT */

    return result;
}

#if defined(BCM_COLLECTOR_SUPPORT)
/*
 * Function:
 *     _bcm_esw_collector_user_update
 * Purpose:
 *     Add user for a collector. 
 *     Currently one user is expected per collector.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     id            - (IN) Collector Id
 *     user          - (IN) The user type of calling app
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_esw_collector_user_update(int unit, bcm_collector_t id, _bcm_collector_export_app_t user)
{
    int result = BCM_E_UNAVAIL;
   /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_lock(unit));

    if (soc_feature(unit, soc_feature_collector)) {
        result = _bcm_xgs5_collector_user_update(unit, id, user);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_unlock(unit));

    return result;
}

/*
 * Function:
 *     _bcm_esw_collector_check_user_is_other 
 * Purpose:
 *     Check if collector ID is used by other user.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     id            - (IN) Collector Id
 *     my_user       - (IN) The user type of calling app
 * Returns:
 *     BCM_E_XXX
 */
uint8
_bcm_esw_collector_check_user_is_other(int unit, bcm_collector_t id, _bcm_collector_export_app_t my_user)
{
    int result = BCM_E_UNAVAIL;

   /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_lock(unit));

    if (soc_feature(unit, soc_feature_collector)) {
        result = _bcm_xgs5_collector_check_user_is_other(unit, id, my_user);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_unlock(unit));

    return result;
}

#endif /* BCM_COLLECTOR_SUPPORT */

/*
 * Function:
 *      bcm_collector_export_record_register
 * Purpose:
 *      Register callback routine for Local collector Flowtracker
 *      record export.
 * Parameters:
 *      unit - (IN) Unit number.
 *      collector_id - (IN) collector Software ID.
 *      callback_options - (IN) export callback options.
 *      callback_fn - (IN) <UNDEF>
 *      userdata - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_esw_collector_export_record_register(
    int unit,
    bcm_collector_t collector_id,
    bcm_collector_callback_options_t callback_options,
    bcm_collector_export_record_cb_f callback_fn,
    void *userdata)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_COLLECTOR_SUPPORT)
   /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_lock(unit));

    if (soc_feature(unit, soc_feature_collector)) {
        result = _bcm_xgs5_collector_export_record_register(unit, collector_id,
                callback_options, callback_fn, userdata);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_unlock(unit));
#endif /* BCM_COLLECTOR_SUPPORT */

    return result;
}

/*
 * Function:
 *      bcm_collector_export_record_unregister
 * Purpose:
 *      Unregister callback routine for Local collector
 *      record export.
 * Parameters:
 *      unit - (IN) Unit number.
 *      collector_id - (IN) collector Software ID.
 *      callback_options - (IN) export callback options.
 *      callback_fn - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_esw_collector_export_record_unregister(
    int unit,
    bcm_collector_t collector_id,
    bcm_collector_callback_options_t callback_options,
    bcm_collector_export_record_cb_f callback_fn)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_COLLECTOR_SUPPORT)
   /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_lock(unit));

    if (soc_feature(unit, soc_feature_collector)) {
        result = _bcm_xgs5_collector_export_record_unregister(unit, collector_id,
                callback_options, callback_fn);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_unlock(unit));
#endif /* BCM_COLLECTOR_SUPPORT */

    return result;
}


/*
 * Function:
 *      _bcm_esw_collector_sw_dump
 * Purpose:
 *      Dumps collector module information
 * Parameters:
 *      unit           - (IN)  BCM device number
 * Returns: void
 */
void _bcm_esw_collector_sw_dump(int unit)
{
#if defined(BCM_COLLECTOR_SUPPORT)

    if (soc_feature(unit, soc_feature_collector)) {
        _bcm_xgs5_collector_sw_dump(unit);
    }
#endif /* BCM_COLLECTOR_SUPPORT */

}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_esw_collector_sync
 * Purpose:
 *      Warmboot scache sync
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_esw_collector_sync(int unit)
{
    int result = BCM_E_UNAVAIL;
 #if defined(BCM_COLLECTOR_SUPPORT)
   /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_lock(unit));

    if (soc_feature(unit, soc_feature_collector)) {
        result = _bcm_xgs5_collector_sync(unit);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_collector_unlock(unit));
#endif /* BCM_COLLECTOR_SUPPORT */
   
    return result;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

