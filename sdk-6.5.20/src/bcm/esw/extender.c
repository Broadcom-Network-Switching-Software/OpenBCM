/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: Implements ESW Port Extension APIs
 */

#if defined(INCLUDE_L3)

#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/extender.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/triumph3.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/greyhound.h>

#if (defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT)) && \
    defined(INCLUDE_L3)
/* Flag to check initialized status */
STATIC int extender_initialized[BCM_MAX_NUM_UNITS];

#define EXTENDER_INIT(unit)                               \
    do {                                                  \
        if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {  \
            return BCM_E_UNIT;                            \
        }                                                 \
        if (!extender_initialized[unit]) {                \
            return BCM_E_INIT;                            \
        }                                                 \
    } while (0)

/* 
 * Extender module lock
 */
STATIC sal_mutex_t extender_mutex[BCM_MAX_NUM_UNITS] = {NULL};

#define EXTENDER_LOCK(unit) \
        sal_mutex_take(extender_mutex[unit], sal_mutex_FOREVER);

#define EXTENDER_UNLOCK(unit) \
        sal_mutex_give(extender_mutex[unit]); 

/*
 * Function:
 *      _bcm_esw_extender_free_resources
 * Purpose:
 *      Free all allocated tables and memory
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_esw_extender_free_resources(int unit)
{
    if (extender_mutex[unit]) {
        sal_mutex_destroy(extender_mutex[unit]);
        extender_mutex[unit] = NULL;
    } 
}
#endif /* (BCM_TRIUMPH3_SUPPORT||BCM_GREYHOUND_SUPPORT) && INCLUDE_L3 */

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0     SOC_SCACHE_VERSION(1,0)
#define BCM_WB_DEFAULT_VERSION BCM_WB_VERSION_1_0

/*
 * Function:
 *      _bcm_esw_extender_sync
 * Purpose:
 *      Record Extender module persistent info for Level 2 Warm Boot
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_extender_sync(int unit)
{
#if (defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT)) && \
    defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_port_extension)) {
        if (!SOC_IS_GREYHOUND(unit) && !SOC_IS_HURRICANE3(unit)) {
#if defined(BCM_TRIUMPH3_SUPPORT)
            soc_scache_handle_t scache_handle;
            uint8               *scache_ptr;

            SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_EXTENDER, 0);
            BCM_IF_ERROR_RETURN
                (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                         0, &scache_ptr,
                                         BCM_WB_DEFAULT_VERSION, NULL));

            BCM_IF_ERROR_RETURN(_bcm_tr3_extender_sync(unit, &scache_ptr));
#endif /* defined(BCM_TRIUMPH3_SUPPORT) */
        }
    }
#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */

    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcm_esw_extender_init
 * Purpose:
 *      Init extender module
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_extender_init(int unit)
{
#if (defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT)) && \
     defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_port_extension)) {

#ifdef BCM_WARM_BOOT_SUPPORT
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (!(SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
              SOC_IS_GREYHOUND2(unit))) {
            uint32              required_scache_size;
            soc_scache_handle_t scache_handle;
            uint8               *scache_ptr;
            int                 rv = BCM_E_NONE;

            /* Get the required scache size */
            BCM_IF_ERROR_RETURN(bcm_tr3_extender_required_scache_size_get(unit,
                        &required_scache_size));

            /* Allocate required scache */
            SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_EXTENDER, 0);
            if (required_scache_size > 0) {
                rv = _bcm_esw_scache_ptr_get(unit, scache_handle,
                        (0 == SOC_WARM_BOOT(unit)), required_scache_size,
                        &scache_ptr, BCM_WB_DEFAULT_VERSION, NULL);
                if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                    return rv;
                }
            }
        }
#endif /*if defined(BCM_TRIUMPH3_SUPPORT) */
#endif /* BCM_WARM_BOOT_SUPPORT */

        if (extender_initialized[unit]) {
            BCM_IF_ERROR_RETURN(bcm_esw_extender_cleanup(unit));
        }

        if (!(SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
            SOC_IS_GREYHOUND2(unit))) {
#if defined(BCM_TRIUMPH3_SUPPORT)
            BCM_IF_ERROR_RETURN(bcm_tr3_extender_init(unit));
#endif
        }

        if (extender_mutex[unit] == NULL) {
            extender_mutex[unit] = sal_mutex_create("extender mutex");
            if (extender_mutex[unit] == NULL) {
                _bcm_esw_extender_free_resources(unit);
                return BCM_E_MEMORY;
            }
        }
        extender_initialized[unit] = TRUE;

        return BCM_E_NONE;
    }
#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}


/* Function:
 *      bcm_extender_cleanup
 * Purpose:
 *      Detach extender module, clear all HW states
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_esw_extender_cleanup(int unit)
{
#if (defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT)) && \
    defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_port_extension)) {

        if (!(SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
            SOC_IS_GREYHOUND2(unit))) {
#if defined(BCM_TRIUMPH3_SUPPORT)
            BCM_IF_ERROR_RETURN(bcm_tr3_extender_cleanup(unit));
#endif
        }
        _bcm_esw_extender_free_resources(unit);

        extender_initialized[unit] = FALSE;

        return BCM_E_NONE;
    }
#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/* Function:
 *      bcm_esw_extender_port_add
 * Purpose:
 *      Create a Port Extender port.
 * Parameters:
 *      unit - (IN) SOC unit Number
 *      extender_port - (IN/OUT) Extender port information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_esw_extender_port_add(int unit, bcm_extender_port_t *extender_port)
{
    if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
        SOC_IS_GREYHOUND2(unit)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_port_extension)) {

        int rv;

        EXTENDER_INIT(unit);

        EXTENDER_LOCK(unit);

        rv = bcm_tr3_extender_port_add(unit, extender_port);

        EXTENDER_UNLOCK(unit);

        return rv;
    }
#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/* Function:
 *      bcm_esw_extender_port_delete
 * Purpose:
 *      Delete a Port Extender port
 * Parameters:
 *      unit - (IN) SOC unit number
 *      extender_port_id - (IN) Extender port ID
 * Returns:
 *      BCM_E_XXX
 */
int bcm_esw_extender_port_delete(int unit, bcm_gport_t extender_port_id)
{
    if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
        SOC_IS_GREYHOUND2(unit)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_port_extension)) {
        int rv;

        EXTENDER_INIT(unit);

        EXTENDER_LOCK(unit);

        rv = bcm_tr3_extender_port_delete(unit, extender_port_id);

        EXTENDER_UNLOCK(unit);

        return rv;
    }
#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/* Function:
 *      bcm_esw_extender_port_delete_all
 * Purpose:
 *      Delete all Port Extender ports
 * Parameters:
 *      unit - (IN) SOC unit number
 * Returns:
 *      BCM_E_XXX
 */
int bcm_esw_extender_port_delete_all(int unit)
{
    if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
        SOC_IS_GREYHOUND2(unit)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_port_extension)) {
        int rv;

        EXTENDER_INIT(unit);

        EXTENDER_LOCK(unit);

        rv = bcm_tr3_extender_port_delete_all(unit);

        EXTENDER_UNLOCK(unit);

        return rv;
    }
#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/* Function:
 *      bcm_esw_extender_port_get
 * Purpose:
 *      Get info about a Port Extender port
 * Parameters:
 *      unit - (IN) SOC unit number
 *      extender_port - (IN/OUT) Extender port information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_esw_extender_port_get(int unit, bcm_extender_port_t *extender_port)
{
    if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
        SOC_IS_GREYHOUND2(unit)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_port_extension)) {
        int rv;

        EXTENDER_INIT(unit);

        EXTENDER_LOCK(unit);

        rv = bcm_tr3_extender_port_get(unit, extender_port);

        EXTENDER_UNLOCK(unit);

        return rv;
    }
#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/* Function:
 *      bcm_esw_extender_port_traverse
 * Purpose:
 *      Traverse all valid Port Extender ports and call the
 *      supplied callback routine.
 * Parameters:
 *      unit      - Device Number
 *      cb        - User callback function, called once per Extender Port.
 *      user_data - cookie
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_extender_port_traverse(int unit,
                          bcm_extender_port_traverse_cb cb,
                          void *user_data)
{
    if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
        SOC_IS_GREYHOUND2(unit)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_port_extension)) {
        int rv;

        EXTENDER_INIT(unit);

        EXTENDER_LOCK(unit);

        rv = bcm_tr3_extender_port_traverse(unit, cb, user_data);

        EXTENDER_UNLOCK(unit);

        return rv;
    }
#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/* Function:
 *      bcm_esw_extender_forward_add
 * Purpose:
 *      Add a downstream forwarding entry
 * Parameters:
 *      unit - (IN) Device Number
 *      extender_forward_entry - (IN) Forwarding entry
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_extender_forward_add(int unit,
        bcm_extender_forward_t *extender_forward_entry)
{
    int rv = BCM_E_UNAVAIL;

#if (defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT)) && \
    defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_port_extension)) {

        EXTENDER_INIT(unit);

        EXTENDER_LOCK(unit);

        if (soc_feature(unit, soc_feature_ism_memory)) {
#if defined(BCM_TRIUMPH3_SUPPORT)
            rv = bcm_tr3_extender_forward_add(unit, extender_forward_entry);
#endif /* BCM_TRIUMPH3_SUPPORT */
        } else {
            if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
                SOC_IS_GREYHOUND2(unit)) {
#ifdef BCM_GREYHOUND_SUPPORT
                rv = bcm_gh_extender_forward_add(unit, extender_forward_entry);
#endif /* BCM_GREYHOUND_SUPPORT */
            } else {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_PE_SUPPORT)
                rv = bcm_td2_extender_forward_add(unit, extender_forward_entry);
#endif /* BCM_TRIDENT2_SUPPORT || BCM_PE_SUPPORT */
            }
        }

        EXTENDER_UNLOCK(unit);

        return rv;
    }
#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */

    return rv;
}

/* Function:
 *      bcm_esw_extender_forward_delete
 * Purpose:
 *	Delete a downstream forwarding entry
 * Parameters:
 *      unit - (IN) Device Number
 *      extender_forward_entry - (IN) Forwarding entry
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_extender_forward_delete(int unit,
        bcm_extender_forward_t *extender_forward_entry)
{
    int rv = BCM_E_UNAVAIL;

#if (defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT)) && \
    defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_port_extension)) {

        EXTENDER_INIT(unit);

        EXTENDER_LOCK(unit);

        if (soc_feature(unit, soc_feature_ism_memory)) {
#if defined(BCM_TRIUMPH3_SUPPORT) 
            rv = bcm_tr3_extender_forward_delete(unit, extender_forward_entry);
#endif /* BCM_TRIUMPH3_SUPPORT */
        } else {
            if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
                SOC_IS_GREYHOUND2(unit)) {
#ifdef BCM_GREYHOUND_SUPPORT
                rv = bcm_gh_extender_forward_delete(unit, extender_forward_entry);
#endif /* BCM_GREYHOUND_SUPPORT */
            } else {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_PE_SUPPORT)
                rv = bcm_td2_extender_forward_delete(unit, extender_forward_entry);
#endif /* BCM_TRIDENT2_SUPPORT || BCM_PE_SUPPORT */
            }
        }

        EXTENDER_UNLOCK(unit);

        return rv;
    }
#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */

    return rv;
}

/* Function:
 *      bcm_esw_extender_forward_delete_all
 * Purpose:
 *      Delete all downstream forwarding entries
 * Parameters:
 *      unit - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_extender_forward_delete_all(int unit)
{
    int rv = BCM_E_UNAVAIL;

#if (defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT)) && \
    defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_port_extension)) {

        EXTENDER_INIT(unit);

        EXTENDER_LOCK(unit);

        if (soc_feature(unit, soc_feature_ism_memory)) {
#if defined(BCM_TRIUMPH3_SUPPORT) 
            rv = bcm_tr3_extender_forward_delete_all(unit);
#endif /* BCM_TRIUMPH3_SUPPORT */
        } else {
            if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
                SOC_IS_GREYHOUND2(unit)) {
#ifdef BCM_GREYHOUND_SUPPORT
                rv = bcm_gh_extender_forward_delete_all(unit);
#endif /* BCM_GREYHOUND_SUPPORT */
            } else {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_PE_SUPPORT)
                rv = bcm_td2_extender_forward_delete_all(unit);
#endif /* BCM_TRIDENT2_SUPPORT || BCM_PE_SUPPORT */
            }
        }

        EXTENDER_UNLOCK(unit);

        return rv;
    }
#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */

    return rv;
}

/* Function:
 *      bcm_esw_extender_forward_get
 * Purpose:
 *      Get downstream forwarding entry
 * Parameters:
 *      unit - (IN) Device Number
 *      extender_forward_entry - (IN/OUT) Forwarding entry 
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_extender_forward_get(int unit,
        bcm_extender_forward_t *extender_forward_entry)
{
    int rv = BCM_E_UNAVAIL;

#if (defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT)) && \
    defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_port_extension)) {

        EXTENDER_INIT(unit);

        EXTENDER_LOCK(unit);

        if (soc_feature(unit, soc_feature_ism_memory)) {
#if defined(BCM_TRIUMPH3_SUPPORT) 
            rv = bcm_tr3_extender_forward_get(unit, extender_forward_entry);
#endif /* BCM_TRIUMPH3_SUPPORT */
        } else {
            if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
                SOC_IS_GREYHOUND2(unit)) {
#ifdef BCM_GREYHOUND_SUPPORT
                rv = bcm_gh_extender_forward_get(unit, extender_forward_entry);
#endif /* BCM_GREYHOUND_SUPPORT */
            } else {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_PE_SUPPORT)
                rv = bcm_td2_extender_forward_get(unit, extender_forward_entry);
#endif /* BCM_TRIDENT2_SUPPORT BCM_PE_SUPPORT */
            }
        }

        EXTENDER_UNLOCK(unit);

        return rv;
    }
#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */

    return rv;
}

/* Function:
 *      bcm_esw_extender_forward_traverse
 * Purpose:
 *      Traverse all valid downstream forwarding entries and call the
 *      supplied callback routine.
 * Parameters:
 *      unit      - Device Number
 *      cb        - User callback function, called once per forwarding entry.
 *      user_data - cookie
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_extender_forward_traverse(int unit,
                             bcm_extender_forward_traverse_cb cb,
                             void *user_data)
{
    int rv = BCM_E_UNAVAIL;

#if (defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT)) && \
    defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_port_extension)) {

        EXTENDER_INIT(unit);

        EXTENDER_LOCK(unit);

        if (soc_feature(unit, soc_feature_ism_memory)) {
#if defined(BCM_TRIUMPH3_SUPPORT) 
            rv = bcm_tr3_extender_forward_traverse(unit, cb, user_data);
#endif /* BCM_TRIUMPH3_SUPPORT */
        } else {
            if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
                SOC_IS_GREYHOUND2(unit)) {
#ifdef BCM_GREYHOUND_SUPPORT
                rv = bcm_gh_extender_forward_traverse(unit, cb, user_data);
#endif /* BCM_GREYHOUND_SUPPORT */
            } else {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_PE_SUPPORT)
                rv = bcm_td2_extender_forward_traverse(unit, cb, user_data);
#endif /* BCM_TRIDENT2_SUPPORT || BCM_PE_SUPPORT */
            }
        }

        EXTENDER_UNLOCK(unit);

        return rv;
    }
#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */

    return rv;
}

/*
 * Function:
 *      _bcm_esw_extender_port_source_vp_lag_set
 * Purpose:
 *      Set source VP LAG for an Extender virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) Extender virtual port GPORT ID. 
 *      vp_lag_vp - (IN) VP representing the VP LAG. 
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_extender_port_source_vp_lag_set(int unit, bcm_gport_t gport,
        int vp_lag_vp)
{
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_port_extension) &&
        soc_feature(unit, soc_feature_vp_lag)) {
        int rv;

        EXTENDER_INIT(unit);

        EXTENDER_LOCK(unit);

        rv = bcm_td2_extender_port_source_vp_lag_set(unit, gport, vp_lag_vp);

        EXTENDER_UNLOCK(unit);

        return rv;
    }
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_esw_extender_port_source_vp_lag_clear
 * Purpose:
 *      Clear source VP LAG for an Extender virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) Extender virtual port GPORT ID. 
 *      vp_lag_vp - (IN) VP representing the VP LAG. 
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_extender_port_source_vp_lag_clear(int unit, bcm_gport_t gport,
        int vp_lag_vp)
{
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_port_extension) &&
        soc_feature(unit, soc_feature_vp_lag)) {
        int rv;

        EXTENDER_INIT(unit);

        EXTENDER_LOCK(unit);

        rv = bcm_td2_extender_port_source_vp_lag_clear(unit, gport, vp_lag_vp);

        EXTENDER_UNLOCK(unit);

        return rv;
    }
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_esw_extender_port_source_vp_lag_get
 * Purpose:
 *      Get source VP LAG for an Extender virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) Extender virtual port GPORT ID. 
 *      vp_lag_vp - (OUT) VP representing the VP LAG. 
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_extender_port_source_vp_lag_get(int unit, bcm_gport_t gport,
        int *vp_lag_vp)
{
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_port_extension) &&
        soc_feature(unit, soc_feature_vp_lag)) {
        int rv;

        EXTENDER_INIT(unit);

        EXTENDER_LOCK(unit);

        rv = bcm_td2_extender_port_source_vp_lag_get(unit, gport, vp_lag_vp);

        EXTENDER_UNLOCK(unit);

        return rv;
    }
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_esw_extender_port_match_add
 * Purpose:
 *      Add a match to an existing vp.
 * Parameters:
 *      unit -             (IN) SOC unit number. 
 *      port -             (IN) port of match criteria.
 *      extended_port_vid -(IN) extended_port_vid of match criteria.
 *      vlan -             (IN) vlan of match criteria.
 *      vp -               (IN) the vp of match destination.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_extender_port_match_add(int unit, bcm_gport_t port,
                                 uint16 extended_port_vid,
                                 bcm_vlan_t vlan, int vp)
{
#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_port_extension)) {

        int rv;

        EXTENDER_INIT(unit);

        EXTENDER_LOCK(unit);

        rv = _bcm_tr3_extender_port_match_add(unit, port, extended_port_vid,
                                              vlan, vp);

        EXTENDER_UNLOCK(unit);

        return rv;
    }
#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */
    
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      _bcm_esw_extender_port_match_delete
 * Purpose:
 *      delete a match from an existing vp.
 * Parameters:
 *      unit -             (IN) SOC unit number. 
 *      port -             (IN) port of match criteria.
 *      extended_port_vid -(IN) extended_port_vid of match criteria.
 *      vlan -             (IN) vlan of match criteria.
 *      vp -               (IN) the vp of match destination.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_extender_port_match_delete(int unit, bcm_gport_t port,
                                    uint16 extended_port_vid,
                                    bcm_vlan_t vlan, int vp)
{
#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_port_extension)) {

        int rv;

        EXTENDER_INIT(unit);

        EXTENDER_LOCK(unit);

        rv = _bcm_tr3_extender_port_match_delete(unit, port, extended_port_vid,
                                                 vlan, vp);

        EXTENDER_UNLOCK(unit);

        return rv;
    }
#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */
    
    return BCM_E_UNAVAIL;
}

/* Function:
 *      bcm_esw_extender_egress_add
 * Purpose:
 *      Add a Extender egress object to a Extender port
 * Parameters:
 *      unit - (IN) SOC unit Number
 *      extender_port - (IN) Extender port
 *      extender_egress - (IN/OUT) Extender egress object, the egress_if field is output.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_extender_egress_add(int unit, bcm_gport_t extender_port,
    bcm_extender_egress_t *extender_egress)
{
#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
    if ((soc_feature(unit, soc_feature_port_extension))
        && (soc_feature(unit, soc_feature_port_extender_vp_sharing))) {
        int rv;

        EXTENDER_INIT(unit);

        EXTENDER_LOCK(unit);

        rv = bcm_tr3_extender_egress_add(unit, extender_port,
                extender_egress);

        EXTENDER_UNLOCK(unit);

        return rv;
    }
#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/* Function:
 *      bcm_esw_extender_egress_delete
 * Purpose:
 *      Delete a Extender egress object to a Extender port
 * Parameters:
 *      unit - (IN) SOC unit Number
 *      extender_port - (IN) Extender port
 *      extender_egress - (IN) Extender egress object.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_extender_egress_delete(int unit, bcm_gport_t extender_port,
    bcm_extender_egress_t *extender_egress)
{
#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
    if ((soc_feature(unit, soc_feature_port_extension))
        && (soc_feature(unit, soc_feature_port_extender_vp_sharing))) {
        int rv;

        EXTENDER_INIT(unit);

        EXTENDER_LOCK(unit);

        rv = bcm_tr3_extender_egress_delete(unit, extender_port,
                extender_egress);

        EXTENDER_UNLOCK(unit);

        return rv;
    }
#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/* Function:
 *      bcm_esw_extender_egress_set
 * Purpose:
 *      Delete existing egress object for a Extender port and add
 *      an array of extender egress objects to the Extender port
 * Parameters:
 *      unit - (IN) SOC unit Number
 *      extender_port - (IN) Extender port
 *      extender_egress_array - (IN) Array of Extender egress objects
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_extender_egress_set(int unit, bcm_gport_t extender_port,
        int array_size, bcm_extender_egress_t *extender_egress_array)
{
#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
    if ((soc_feature(unit, soc_feature_port_extension))
        && (soc_feature(unit, soc_feature_port_extender_vp_sharing))) {
        int rv;

        EXTENDER_INIT(unit);

        EXTENDER_LOCK(unit);

        rv = bcm_tr3_extender_egress_set(unit, extender_port,
                array_size, extender_egress_array);

        EXTENDER_UNLOCK(unit);

        return rv;
    }
#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/* Function:
 *      bcm_esw_extender_egress_get
 * Purpose:
 *      get an Extender egress object for Extender port
 * Parameters:
 *      unit - (IN) SOC unit Number
 *      extender_port - (IN) Extender port
 *      extender_egress - (IN/OUT) Extender egress object
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_extender_egress_get(int unit, bcm_gport_t extender_port,
        bcm_extender_egress_t *extender_egress)
{
#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
    if ((soc_feature(unit, soc_feature_port_extension))
        && (soc_feature(unit, soc_feature_port_extender_vp_sharing))) {
        int rv;

        EXTENDER_INIT(unit);

        EXTENDER_LOCK(unit);

        rv = bcm_tr3_extender_egress_get(unit, extender_port,
               extender_egress);

        EXTENDER_UNLOCK(unit);

        return rv;
    }
#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/* Function:
 *      bcm_esw_extender_egress_get_all
 * Purpose:
 *      Get array of Extender egress objects for Extender port
 * Parameters:
 *      unit - (IN) SOC unit Number
 *      extender_port - (IN) Extender port
 *      array_size - (IN) Size of Extender egress objects array.
 *      extender_egress_array - (OUT) Array of Extender egress objects.
 *      count - (OUT) Number of Extender egress objects returned.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_extender_egress_get_all(int unit, bcm_gport_t extender_port,
        int array_size, bcm_extender_egress_t *extender_egress_array, int *count)
{
#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
    if ((soc_feature(unit, soc_feature_port_extension))
        && (soc_feature(unit, soc_feature_port_extender_vp_sharing))) {
        int rv;

        EXTENDER_INIT(unit);

        EXTENDER_LOCK(unit);

        rv = bcm_tr3_extender_egress_get_all(unit, extender_port,
                array_size, extender_egress_array, count);

        EXTENDER_UNLOCK(unit);

        return rv;
    }
#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/* Function:
 *      bcm_esw_extender_egress_delete_all
 * Purpose:
 *      Delete all Extender egress objects for an Extender Port
 * Parameters:
 *      unit - (IN) SOC unit Number
 *      extender_port - (IN) Extender port
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_extender_egress_delete_all(int unit, bcm_gport_t extender_port)
{
    return bcm_esw_extender_egress_set(unit, extender_port, 0, NULL);
}

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     _bcm_extender_sw_dump
 * Purpose:
 *     Displays extender module information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_extender_sw_dump(int unit)
{
#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_port_extension)) {
        if (!(SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
            SOC_IS_GREYHOUND2(unit))) {
            bcm_tr3_extender_sw_dump(unit);
        }
    }
#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#else   /* INCLUDE_L3 */
typedef int bcm_esw_extender_not_empty; /* Make ISO compilers happy. */
#endif  /* INCLUDE_L3 */

