/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        fcoe.c
 * Purpose:     ESW FCOE functions
 */
#include <soc/drv.h>
#include <soc/l3x.h>

#include <bcm/stat.h>
#include <bcm_int/esw/fcoe.h>
#include <bcm/l3.h>
#include <bcm_int/esw/l3.h>

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
#include <bcm_int/esw/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */

/* Flag to check initialized status */
STATIC int fcoe_initialized[BCM_MAX_NUM_UNITS];

/* FCOE module lock */
STATIC sal_mutex_t fcoe_mutex[BCM_MAX_NUM_UNITS] = {NULL};

#define FCOE_INIT_CHECK(unit)                             \
    do {                                                  \
        if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {  \
            return BCM_E_UNIT;                            \
        }                                                 \
        if (!fcoe_initialized[unit]) {                    \
            return BCM_E_INIT;                            \
        }                                                 \
    } while (0)

#define FCOE_LOCK(unit) \
        sal_mutex_take(fcoe_mutex[unit], sal_mutex_FOREVER);

#define FCOE_UNLOCK(unit) \
        sal_mutex_give(fcoe_mutex[unit]);

/*
 * Function:    _bcm_esw_fcoe_free_resources
 * Purpose:     Free FCOE resources
 * Parameters:  unit - SOC unit number
 * Returns:     Nothing
 */
STATIC void
_bcm_esw_fcoe_free_resources(int unit)
{
    if (fcoe_mutex[unit]) {
        sal_mutex_destroy(fcoe_mutex[unit]);
        fcoe_mutex[unit] = NULL;
    }
}

/* De-initialize FCOE module */
int
bcm_esw_fcoe_cleanup(int unit)
{
    if (soc_feature(unit, soc_feature_fcoe)) {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
            BCM_IF_ERROR_RETURN(bcm_td2_fcoe_cleanup(unit));
        }
#endif /* BCM_TRIDENT2_SUPPORT */

        _bcm_esw_fcoe_free_resources(unit);

        fcoe_initialized[unit] = FALSE;

        return BCM_E_NONE;
    }

    return BCM_E_UNAVAIL;
}

/* Initialize FCOE module */
int
bcm_esw_fcoe_init(int unit)
{
    if (soc_feature(unit, soc_feature_fcoe)) {
        if (fcoe_initialized[unit]) {
            BCM_IF_ERROR_RETURN(bcm_esw_fcoe_cleanup(unit));
        }

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
            BCM_IF_ERROR_RETURN(bcm_td2_fcoe_init(unit));
        }
#endif /* BCM_TRIDENT2_SUPPORT */

        if (fcoe_mutex[unit] == NULL) {
            fcoe_mutex[unit] = sal_mutex_create("fcoe mutex");
            if (fcoe_mutex[unit] == NULL) {
                bcm_esw_fcoe_cleanup(unit);
                return BCM_E_MEMORY;
            }
        }

        fcoe_initialized[unit] = TRUE;

        return BCM_E_NONE;
    }

    return BCM_E_UNAVAIL;
}

#ifdef BCM_WARM_BOOT_SUPPORT

int
_bcm_esw_fcoe_sync(int unit)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);
        return _bcm_td2_fcoe_sync(unit);
    }
#endif
    return BCM_E_UNAVAIL;
}

#endif

/* enable/disable FCOE without clearing any FCOE table */
int
bcm_esw_fcoe_enable_set(int unit)
{
    /* temporary return value */
    return BCM_E_UNAVAIL;
}

/* Add an entry into the FCOE routing table */
int
bcm_esw_fcoe_route_add(int unit,  bcm_fcoe_route_t *route)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);
        return bcm_td2_fcoe_route_add(unit, route);
    }
#endif
    return BCM_E_UNAVAIL;
}

/* Delete an nport_id entry from the FCOE routing table */
int
bcm_esw_fcoe_route_delete(int unit,  bcm_fcoe_route_t *route)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);
        return bcm_td2_fcoe_route_delete(unit, route);
    }
#endif
    return BCM_E_UNAVAIL;
}

/* Delete FCOE entries based on Masked Domain prefix (network) */
int
bcm_esw_fcoe_route_delete_by_prefix(int unit, bcm_fcoe_route_t *route)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);
        return bcm_td2_fcoe_route_delete_by_prefix(unit, route);
    }
#endif
    return BCM_E_UNAVAIL;
}

/* Delete FCOE entries that do/don't match a specified L3 interface */
int
bcm_esw_fcoe_route_delete_by_interface(int unit,  bcm_fcoe_route_t *route)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);
        return bcm_td2_fcoe_route_delete_by_interface(unit, route);
    }
#endif
    return BCM_E_UNAVAIL;
}

/* Deletes all FCOE routing table entries */
int
bcm_esw_fcoe_route_delete_all(int unit)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);
        return bcm_td2_fcoe_route_delete_all(unit);
    }
#endif
    return BCM_E_UNAVAIL;
}

/* Look up an FCOE routing table entry based on nport id */
int
bcm_esw_fcoe_route_find(int unit,  bcm_fcoe_route_t *route)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);
        return bcm_td2_fcoe_route_find(unit, route);
    }
#endif
    return BCM_E_UNAVAIL;
}

/* Allocate and configure a VSAN */
int 
bcm_esw_fcoe_vsan_create (int unit, uint32 options, bcm_fcoe_vsan_t *vsan,
                          bcm_fcoe_vsan_id_t *vsan_id)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);
        return bcm_td2_fcoe_vsan_create(unit, options, vsan, vsan_id);
    }
#endif
    return BCM_E_UNAVAIL;
}

/* Destroy a VSAN */
int 
bcm_esw_fcoe_vsan_destroy (int unit,  bcm_fcoe_vsan_id_t vsan_id)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);
        return bcm_td2_fcoe_vsan_destroy(unit, vsan_id);
    }
#endif
    return BCM_E_UNAVAIL;
}

/* Destroy all VSANs */
int 
bcm_esw_fcoe_vsan_destroy_all (int unit)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);
        return bcm_td2_fcoe_vsan_destroy_all(unit);
    }
#endif
    return BCM_E_UNAVAIL;
}

/* Get VSAN controls */
int 
bcm_esw_fcoe_control_vsan_get (int unit,  
                               bcm_fcoe_vsan_id_t vsan_id, 
                               bcm_fcoe_vsan_control_t type, 
                               int *arg)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);
        return bcm_td2_fcoe_control_vsan_get(unit, vsan_id, type, arg);
    }
#endif
    return BCM_E_UNAVAIL;
}

/* Set VSAN controls */
int 
bcm_esw_fcoe_control_vsan_set (int unit, 
                               bcm_fcoe_vsan_id_t vsan_id, 
                               bcm_fcoe_vsan_control_t type, 
                               int arg)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);
        return bcm_td2_fcoe_control_vsan_set(unit, vsan_id, type, arg);
    }
#endif
    return BCM_E_UNAVAIL;
}

/* Get VSAN properties associated with a VSAN ID */
int 
bcm_esw_fcoe_vsan_get (int unit, bcm_fcoe_vsan_id_t vsan_id, 
                       bcm_fcoe_vsan_t *vsan)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_fcoe)) {
        FCOE_INIT_CHECK(unit);
        return bcm_td2_fcoe_vsan_get(unit, vsan_id, vsan);
    }
#endif
    return BCM_E_UNAVAIL;
}

/* Update VSAN properties associated with a VSAN ID */
int 
bcm_esw_fcoe_vsan_set (int unit, bcm_fcoe_vsan_id_t vsan_id, 
                       bcm_fcoe_vsan_t *vsan)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);
        return bcm_td2_fcoe_vsan_set(unit, vsan_id, vsan);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Purpose:
 *      Add FCoE zoning entry
 * Parameters:
 *      unit       - Device unit number
 *      zone       - zone entry
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_esw_fcoe_zone_add(int unit, bcm_fcoe_zone_entry_t *zone)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);

        return bcm_td2_fcoe_zone_add(unit, zone);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Purpose:
 *      Delete Zone check entry
 * Parameters:
 *      unit       - Device unit number
 *      zone       - zone entry to be deleted
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_esw_fcoe_zone_delete(int unit, bcm_fcoe_zone_entry_t *zone)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);

        return bcm_td2_fcoe_zone_delete(unit, zone);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Description:
 *      Delete all zone check entries
 * Parameters:
 *     unit             - device number
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_fcoe_zone_delete_all(int unit)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);

        return bcm_td2_fcoe_zone_delete_all(unit);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Description:
 *      Delete all zone check entries matching VSAN ID
 * Parameters:
 *      unit       - device number
 *      zone       - zone entry with VSAN ID to be deleted
 * Return:
 *      BCM_E_XXX
 */
int
bcm_esw_fcoe_zone_delete_by_vsan(int unit, bcm_fcoe_zone_entry_t *zone)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);

        return bcm_td2_fcoe_zone_delete_by_vsan(unit, zone);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Description:
 *      Delete all zone check entries matching Source ID
 * Parameters:
 *      unit       - device number
 *      zone       - zone entry with S_ID to be deleted
 * Return:
 *      BCM_E_XXX
 */
int
bcm_esw_fcoe_zone_delete_by_sid(int unit, bcm_fcoe_zone_entry_t *zone)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);

        return bcm_td2_fcoe_zone_delete_by_sid(unit, zone);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Description:
 *      Delete all zone check entries matching Destination ID
 * Parameters:
 *      unit       - device number
 *      zone       - zone entry with D_ID to be deleted
 * Return:
 *      BCM_E_XXX
 */
int
bcm_esw_fcoe_zone_delete_by_did(int unit, bcm_fcoe_zone_entry_t *zone)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);

        return bcm_td2_fcoe_zone_delete_by_did(unit, zone);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Description:
 *      Retrieve Zone info
 * Parameters:
 *      unit       - device number
 *      zone       - zone entry
 * Return:
 *      BCM_E_XXX
 */
int
bcm_esw_fcoe_zone_get(int unit, bcm_fcoe_zone_entry_t *zone)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);

        return bcm_td2_fcoe_zone_get(unit, zone);
    }
#endif
    return BCM_E_UNAVAIL;
}


/* Allocate flex counter and assign counter index/offset for
 * FCOE VSAN ID
 */
int 
bcm_esw_fcoe_vsan_stat_attach (int                 unit,
                               bcm_fcoe_vsan_id_t  vsan,
                               uint32              stat_counter_id)
{
    int rv = BCM_E_UNAVAIL;

    if (soc_feature(unit, soc_feature_advanced_flex_counter)) {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
            FCOE_INIT_CHECK(unit);

            L3_LOCK(unit);
            rv = bcm_td2_fcoe_vsan_stat_attach(unit, vsan, stat_counter_id);
            L3_UNLOCK(unit);
        }
#endif
    }
    return rv;
}

/* Detach flex counter for FCOE VSAN ID  */
int 
bcm_esw_fcoe_vsan_stat_detach (int unit, bcm_fcoe_vsan_id_t  vsan)
{
    int rv = BCM_E_UNAVAIL;

    if (soc_feature(unit, soc_feature_advanced_flex_counter)) {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
            FCOE_INIT_CHECK(unit);

            L3_LOCK(unit);
            rv = bcm_td2_fcoe_vsan_stat_detach(unit, vsan);
            L3_UNLOCK(unit);
        }
#endif
    }
    return rv;
}

/* Get flex counter for FCOE VSAN ID */                          
int 
_bcm_esw_fcoe_vsan_stat_counter_get (int                  unit, 
                                     int                  sync_mode, 
                                     bcm_fcoe_vsan_id_t   vsan,
                                     bcm_fcoe_vsan_stat_t stat, 
                                     uint32               num_entries, 
                                     uint32               *counter_indexes, 
                                     bcm_stat_value_t     *counter_values)
{
    int rv = BCM_E_UNAVAIL;

    if (soc_feature(unit, soc_feature_advanced_flex_counter)) {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
            FCOE_INIT_CHECK(unit);

            L3_LOCK(unit);
            rv = bcm_td2_fcoe_vsan_stat_counter_get(unit, sync_mode, vsan, 
                                                    stat, num_entries,
                                                    counter_indexes, 
                                                    counter_values);
            L3_UNLOCK(unit);
        }
#endif
    }
    return rv;
}

/* Get flex counter for FCOE VSAN ID */                          
int 
bcm_esw_fcoe_vsan_stat_counter_get (int                  unit, 
                                    bcm_fcoe_vsan_id_t   vsan,
                                    bcm_fcoe_vsan_stat_t stat, 
                                    uint32               num_entries, 
                                    uint32               *counter_indexes, 
                                    bcm_stat_value_t     *counter_values)
{
    int rv = BCM_E_UNAVAIL;
    
    rv = _bcm_esw_fcoe_vsan_stat_counter_get(unit, 0, vsan, stat, num_entries,
                                             counter_indexes, counter_values);
    return rv;
}

/* Get flex counter for FCOE VSAN ID */                          
int 
bcm_esw_fcoe_vsan_stat_counter_sync_get (int                  unit, 
                                         bcm_fcoe_vsan_id_t   vsan,
                                         bcm_fcoe_vsan_stat_t stat, 
                                         uint32               num_entries, 
                                         uint32               *counter_indexes, 
                                         bcm_stat_value_t     *counter_values)
{
    int rv = BCM_E_UNAVAIL;
    
    rv = _bcm_esw_fcoe_vsan_stat_counter_get(unit, 1, vsan, stat, num_entries,
                                             counter_indexes, counter_values);
    return rv;
}

/* Set flex counter for FCOE VSAN ID */                               
int 
bcm_esw_fcoe_vsan_stat_counter_set (int                  unit, 
                                    bcm_fcoe_vsan_id_t   vsan,
                                    bcm_fcoe_vsan_stat_t stat, 
                                    uint32               num_entries, 
                                    uint32               *counter_indexes, 
                                    bcm_stat_value_t     *counter_values)
{
    int rv = BCM_E_UNAVAIL;

    if (soc_feature(unit, soc_feature_advanced_flex_counter)) {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
            FCOE_INIT_CHECK(unit);

            L3_LOCK(unit);
            rv = bcm_td2_fcoe_vsan_stat_counter_set(unit, vsan, stat, num_entries,
                                                    counter_indexes, 
                                                    counter_values);
            L3_UNLOCK(unit);
        }
#endif
    }
    return rv;
}

/* Get stat counter ID for FCOE VSAN ID */                               
int 
bcm_esw_fcoe_vsan_stat_id_get (int                  unit,
                               bcm_fcoe_vsan_id_t   vsan,
                               bcm_fcoe_vsan_stat_t stat, 
                               uint32               *stat_counter_id)
{
    int rv = BCM_E_UNAVAIL;

    if (soc_feature(unit, soc_feature_advanced_flex_counter)) {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
            FCOE_INIT_CHECK(unit);

            L3_LOCK(unit);
            rv = bcm_td2_fcoe_vsan_stat_id_get(unit, vsan, stat, 
                                               stat_counter_id);
            L3_UNLOCK(unit);
        }
#endif
    }
    return rv;
}
                               

/* Allocate flex counter and assign counter index/offset for  
 * FCOE Route
 */
int 
bcm_esw_fcoe_route_stat_attach (int                 unit,
                                bcm_fcoe_route_t    *route,
                                uint32              stat_counter_id)
{
    int rv = BCM_E_UNAVAIL;

    if (soc_feature(unit, soc_feature_advanced_flex_counter)) {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
            FCOE_INIT_CHECK(unit);

            L3_LOCK(unit);
            rv = bcm_td2_fcoe_route_stat_attach(unit, route, stat_counter_id);
            L3_UNLOCK(unit);
        }
#endif
    }
    return rv;
}

/* Detach flex counter for FCOE Route */                                
int 
bcm_esw_fcoe_route_stat_detach (int unit, bcm_fcoe_route_t *route)
{
    int rv = BCM_E_UNAVAIL;

    if (soc_feature(unit, soc_feature_advanced_flex_counter)) {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
            FCOE_INIT_CHECK(unit);

            L3_LOCK(unit);
            rv = bcm_td2_fcoe_route_stat_detach(unit, route);
            L3_UNLOCK(unit);
        }
#endif
    }
    return rv;
}

/* Get flex counter for FCOE Route */                                
int 
_bcm_esw_fcoe_route_stat_counter_get (int                   unit, 
                                      int                   sync_mode, 
                                      bcm_fcoe_route_t      *route,
                                      bcm_fcoe_route_stat_t stat, 
                                      uint32                num_entries, 
                                      uint32                *counter_indexes, 
                                      bcm_stat_value_t      *counter_values)
{
    int rv = BCM_E_UNAVAIL;

    if (soc_feature(unit, soc_feature_advanced_flex_counter)) {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
            FCOE_INIT_CHECK(unit);

            L3_LOCK(unit);
            rv = bcm_td2_fcoe_route_stat_counter_get(unit, 0, route, stat, 
                                                     num_entries,
                                                     counter_indexes, 
                                                     counter_values);
            L3_UNLOCK(unit);
        }
#endif
    }
    return rv;
}

/* Get flex counter for FCOE Route */                                
int 
bcm_esw_fcoe_route_stat_counter_get (int                   unit, 
                                     bcm_fcoe_route_t      *route,
                                     bcm_fcoe_route_stat_t stat, 
                                     uint32                num_entries, 
                                     uint32                *counter_indexes, 
                                     bcm_stat_value_t      *counter_values)
{
    int rv = BCM_E_UNAVAIL;
    
    rv = _bcm_esw_fcoe_route_stat_counter_get(unit, 0, route, stat, num_entries,
                                              counter_indexes, counter_values);
    return rv;
}

/* Get flex counter for FCOE Route */                                
int 
bcm_esw_fcoe_route_stat_counter_sync_get (int                   unit, 
                                          bcm_fcoe_route_t      *route,
                                          bcm_fcoe_route_stat_t stat, 
                                          uint32                num_entries, 
                                          uint32               *counter_indexes, 
                                          bcm_stat_value_t      *counter_values)
{
    int rv = BCM_E_UNAVAIL;
    
    rv = _bcm_esw_fcoe_route_stat_counter_get(unit, 1, route, stat, num_entries,
                                             counter_indexes, counter_values);
    return rv;
}

/* Set flex counter for FCOE Route */
int 
bcm_esw_fcoe_route_stat_counter_set (int                   unit, 
                                     bcm_fcoe_route_t      *route,
                                     bcm_fcoe_route_stat_t stat, 
                                     uint32                num_entries, 
                                     uint32                *counter_indexes, 
                                     bcm_stat_value_t      *counter_values)
{
    int rv = BCM_E_UNAVAIL;

    if (soc_feature(unit, soc_feature_advanced_flex_counter)) {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
            FCOE_INIT_CHECK(unit);

            L3_LOCK(unit);
            rv = bcm_td2_fcoe_route_stat_counter_set(unit, route, stat, num_entries,
                                                     counter_indexes, 
                                                     counter_values);
            L3_UNLOCK(unit);
        }
#endif
    }
    return rv;
}
       
/* Get stat counter ID for FCOE Route */       
int 
bcm_esw_fcoe_route_stat_id_get (int                   unit,
                                bcm_fcoe_route_t      *route,
                                bcm_fcoe_route_stat_t stat, 
                                uint32                *stat_counter_id)
{
    int rv = BCM_E_UNAVAIL;

    if (soc_feature(unit, soc_feature_advanced_flex_counter)) {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
            FCOE_INIT_CHECK(unit);

            L3_LOCK(unit);
            rv = bcm_td2_fcoe_route_stat_id_get(unit, route, stat, stat_counter_id);
            L3_UNLOCK(unit);
        }
#endif
    }
    return rv;
}

/*
 * Description:
 *      Set FC header type
 * Parameters:
 *      unit       - (IN) device number
 *      flags      - (IN) BCM_FCOE_FC_HEADER_TYPE_INGRESS/EGRESS
 *      r_ctl      - (IN) [0-255] value of r_ctl to modify
 *      hdr_type   - (IN) Header type to force this value of r_ctl to use
 * Return:
 *      BCM_E_XXX
 */
 int
 bcm_esw_fcoe_fc_header_type_set(int unit, uint32 flags, uint8 r_ctl, 
                                 bcm_fcoe_fc_header_type_t hdr_type)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);

        return bcm_td2_fcoe_fc_header_type_set(unit, flags, r_ctl, hdr_type);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Description:
 *      Retrieve FC header type
 * Parameters:
 *      unit       - (IN) device number
 *      flags      - (IN) BCM_FCOE_FC_HEADER_TYPE_INGRESS/EGRESS
 *      r_ctl      - (IN) [0-255] value of r_ctl to retrieve
 *      hdr_type   - (OUT) Header type that this value of r_ctl is forced to
 * Return:
 *      BCM_E_XXX
 */
 int
 bcm_esw_fcoe_fc_header_type_get(int unit, uint32 flags, uint8 r_ctl, 
                                 bcm_fcoe_fc_header_type_t *hdr_type)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);

        return bcm_td2_fcoe_fc_header_type_get(unit, flags, r_ctl, hdr_type);
    }
#endif
    return BCM_E_UNAVAIL;
}

/* Set parameters for FCOE L3 interface */
int
bcm_esw_fcoe_intf_config_set(int unit, uint32 flags, bcm_if_t intf, 
                             bcm_fcoe_intf_config_t *cfg)
{
#if (defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)) && defined(INCLUDE_L3)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);
        return bcm_td2_fcoe_intf_config_set(unit, flags, intf, cfg);
    }
#endif

    return BCM_E_UNAVAIL;
}

/* Get parameters for FCOE L3 interface */
int
bcm_esw_fcoe_intf_config_get(int unit, uint32 flags, bcm_if_t intf, 
                             bcm_fcoe_intf_config_t *cfg)
{
#if (defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)) && defined(INCLUDE_L3)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);
        return bcm_td2_fcoe_intf_config_get(unit, flags, intf, cfg);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Description:
 *      Add VSAN translate action
 * Parameters:
 *      unit       - (IN) device number
 *      key        - (IN) translation key configuration
 *      action     - (IN) translate action
 * Return:
 *      BCM_E_XXX
 */
int 
bcm_esw_fcoe_vsan_translate_action_add (
                                    int unit, 
                                    bcm_fcoe_vsan_translate_key_config_t *key, 
                                    bcm_fcoe_vsan_action_set_t *action)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);

        return bcm_td2_fcoe_vsan_translate_action_add(unit, key, action);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Description:
 *      Delete VSAN translate action
 * Parameters:
 *      unit       - (IN) device number
 *      key        - (IN) translation key configuration
 * Return:
 *      BCM_E_XXX
 */
int 
bcm_esw_fcoe_vsan_translate_action_delete (
                                    int unit, 
                                    bcm_fcoe_vsan_translate_key_config_t *key) 
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);

        return bcm_td2_fcoe_vsan_translate_action_delete(unit, key);
    }
#endif
    return BCM_E_UNAVAIL;
}

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
void 
_bcm_esw_fcoe_sw_dump(int unit)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        _bcm_td2_fcoe_sw_dump(unit);
    }
#endif
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

/*
 * Description:
 *      Traverse zone entries and call callback function
 * Parameters:
 *      unit       - (IN) device number
 *      flags      - (IN) 
 *      trav_fn    - (IN) user callback function
 *      user_data  - (IN) user data for callback function(optional)
 * Return:
 *      BCM_E_XXX
 */
int
bcm_esw_fcoe_zone_traverse (int unit,
                            uint32 flags,
                            bcm_fcoe_zone_traverse_cb trav_fn,
                            void *user_data)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);

        return bcm_td2_fcoe_zone_traverse(unit, flags, trav_fn, user_data);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Description:
 *      Traverse vsan entries and call callback function 
 * Parameters:
 *      unit       - (IN) device number
 *      flags      - (IN) 
 *      trav_fn    - (IN) user callback function
 *      user_data  - (IN) user data for callback function(optional)
 * Return:
 *      BCM_E_XXX
 */
int
bcm_esw_fcoe_vsan_traverse (int unit,
                            uint32 flags,
                            bcm_fcoe_vsan_traverse_cb trav_fn, 
                            void *user_data)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);

        return bcm_td2_fcoe_vsan_traverse(unit, flags, trav_fn, user_data);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Description:
 *      Traverse route entries and call callback function
 * Parameters:
 *      unit       - (IN) device number
 *      flags      - (IN) route types (BCM_FCOE_HOST_ROUTE, 
 *                        BCM_FCOE_SOURCE_ROUTE, BCM_FCOE_DOMAIN_ROUTE),
 *                        can have all 3 types in the flags.
 *      trav_fn    - (IN) user callback function
 *      user_data  - (IN) user data for callback function(optional)
 * Return:
 *      BCM_E_XXX
 */
int
bcm_esw_fcoe_route_traverse (int unit,
                             uint32 flags,
                             bcm_fcoe_route_traverse_cb trav_fn,
                             void *user_data)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        FCOE_INIT_CHECK(unit);

        return bcm_td2_fcoe_route_traverse(unit, flags, trav_fn, user_data);
    }
#endif
    return BCM_E_UNAVAIL;
}


