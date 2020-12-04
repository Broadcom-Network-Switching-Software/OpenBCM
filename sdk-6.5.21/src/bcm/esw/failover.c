
/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * ESW failover API
 */

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm/extender.h>
#include <bcm_int/esw/failover.h>
#include <bcm_int/esw_dispatch.h>
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/triumph2.h>
#endif /* BCM_TRIUMPH2_SUPPORT */
#include <bcm_int/esw/failover.h>
#include <bcm/failover.h>
#if defined(BCM_TRIUMPH3_SUPPORT)
#include <bcm_int/esw/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_KATANA2_SUPPORT)
#include <bcm_int/esw/katana2.h>
#endif /* BCM_KATANA2_SUPPORT */
#if defined(BCM_SABER2_SUPPORT)
#include <bcm_int/esw/saber2.h>
#endif /* BCM_SABER2_SUPPORT */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/trident2plus.h>
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#if defined(BCM_TOMAHAWK2_SUPPORT)
#include <bcm_int/esw/tomahawk2.h>
#endif /* defined(BCM_TOMAHAWK2_SUPPORT) */
#if defined(BCM_TRIDENT3_SUPPORT)
#include <bcm_int/esw/trident3.h>
#endif /* BCM_TRIDENT3_SUPPORT */
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <bcm_int/esw/tomahawk3.h>
#endif /* BCM_TOMAHAWK3_SUPPORT */

#ifdef INCLUDE_L3


_bcm_failover_bookkeeping_t  _bcm_failover_bk_info[BCM_MAX_NUM_UNITS] = {{ 0 }};

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_esw_failover_sync
 * Purpose:
 *      Record failover module persisitent info for Level 2 Warm Boot.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_failover_sync(int unit)
{
#if defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        BCM_IF_ERROR_RETURN(bcm_sb2_failover_sync(unit));
    }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        BCM_IF_ERROR_RETURN(bcm_tr2_failover_sync(unit));
    }
#endif
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (soc_feature(unit, soc_feature_hierarchical_protection)) {
        BCM_IF_ERROR_RETURN(bcmi_failover_sync(unit));
    }
#endif
#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_th2_failover_sync(unit));
    }
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN(bcm_td3_failover_sync(unit));
    }
#endif
    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *		bcm_esw_failover_init
 * Purpose:
 *		Init  failover module
 * Parameters:
 *		IN :  unit
 * Returns:
 *		BCM_E_XXX
 */

int
bcm_esw_failover_init(int unit)
{
#ifdef BCM_TRIUMPH2_SUPPORT
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_failover_fixed_nh_offset) && SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_failover_init(unit);
    } else
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_td2p_mpls_linear_protection)) {
        return bcm_td2p_failover_init(unit);
    } else
#endif
    if (soc_feature(unit, soc_feature_failover)) {
#if defined(BCM_TOMAHAWK2_SUPPORT)
        if (SOC_IS_TOMAHAWK2(unit)) {
            return bcm_th2_failover_init(unit);
        } else
#endif
        {
            BCM_IF_ERROR_RETURN (bcm_tr2_failover_init(unit));
#if defined(BCM_SABER2_SUPPORT)
            if (SOC_IS_SABER2(unit)) {
                BCM_IF_ERROR_RETURN(bcm_sb2_failover_init(unit));
            }
#endif
            return BCM_E_NONE;
        }
    }
#endif
    return BCM_E_UNAVAIL;
}


 /* Function:
 *      bcm_failover_cleanup
 * Purpose:
 *      Detach the failover module, clear all HW states
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXXX
 */

int
bcm_esw_failover_cleanup(int unit)
{
#ifdef BCM_TRIUMPH2_SUPPORT
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_failover_fixed_nh_offset) && SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_failover_cleanup(unit);
    } else
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_td2p_mpls_linear_protection)) {
        return bcm_td2p_failover_cleanup(unit);
    } else
#endif
    if (soc_feature(unit, soc_feature_failover)) {
#if defined(BCM_TOMAHAWK2_SUPPORT)
        if (SOC_IS_TOMAHAWK2(unit)) {
            return bcm_th2_failover_cleanup(unit);
        } else
#endif
        {
#if defined(BCM_SABER2_SUPPORT)
            if (SOC_IS_SABER2(unit)) {
                BCM_IF_ERROR_RETURN(bcm_sb2_failover_cleanup(unit));
            }
#endif 
            return bcm_tr2_failover_cleanup(unit);
        }
    }
#endif
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *		bcm_esw_failover_create
 * Purpose:
 *		Create a failover object
 * Parameters:
 *		IN :  unit
 *           IN :  flags
 *           OUT :  failover_id
 * Returns:
 *		BCM_E_XXX
 */

int 
bcm_esw_failover_create(int unit, uint32 flags, bcm_failover_t *failover_id)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIUMPH2_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_td2p_mpls_linear_protection)) {
        rv = bcm_td2p_failover_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcm_td2p_failover_create(unit, flags, failover_id);
            bcm_td2p_failover_unlock(unit);
        }
        return rv;
    } else
#endif
    if (soc_feature(unit, soc_feature_failover)) {
#if defined(BCM_TOMAHAWK2_SUPPORT)
        if (SOC_IS_TOMAHAWK2(unit)) {
            rv = bcm_th2_failover_lock(unit);
            if (rv == BCM_E_NONE) {
                rv = bcm_th2_failover_create(unit, flags, failover_id);
                bcm_th2_failover_unlock(unit);
            }
            return rv;
        } else
#endif
        {
            rv = bcm_tr2_failover_lock (unit);
            if ( rv == BCM_E_NONE ) {
                rv = bcm_tr2_failover_create(unit, flags, failover_id);
                bcm_tr2_failover_unlock (unit);
            }
            return rv;
        }
    }
#endif
    return rv;
}

/*
 * Function:
 *		bcm_esw_failover_destroy
 * Purpose:
 *		Destroy a failover object
 * Parameters:
 *		IN :  unit
 *           IN :  failover_id
 * Returns:
 *		BCM_E_XXX
 */

int 
bcm_esw_failover_destroy(int unit, bcm_failover_t failover_id)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_SABER2_SUPPORT)
    int type;
#endif
#ifdef BCM_TRIUMPH2_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_td2p_mpls_linear_protection)) {
        rv = bcm_td2p_failover_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcm_td2p_failover_destroy(unit, failover_id);
            bcm_td2p_failover_unlock(unit);
        }
        return rv;
    } else
#endif
    if (soc_feature(unit, soc_feature_failover)) {
#if defined(BCM_TOMAHAWK2_SUPPORT)
        if (SOC_IS_TOMAHAWK2(unit)) {
            rv = bcm_th2_failover_lock(unit);
            if (rv == BCM_E_NONE) {
                rv = bcm_th2_failover_destroy(unit, failover_id);
                bcm_th2_failover_unlock(unit);
            }
            return rv;
        } else
#endif
        {
            rv = bcm_tr2_failover_lock (unit);
            if (  rv == BCM_E_NONE ) {
#if defined(BCM_SABER2_SUPPORT)
                _BCM_GET_FAILOVER_TYPE(failover_id, type);
                if (type & _BCM_FAILOVER_1_1_MC_PROTECTION_MODE) {
                    rv = bcm_tr2_mmu_failover_destroy(unit, failover_id);
                    bcm_tr2_failover_unlock (unit);
                } else
#endif
                {
                    rv = bcm_tr2_failover_id_validate ( unit, failover_id );
                    if (  rv == BCM_E_NONE ) {
                        rv = bcm_tr2_failover_destroy(unit, failover_id);
                    }
                    bcm_tr2_failover_unlock (unit);
                }
            }
            return rv;
        }
    }
#endif
    return rv;
}

/*
 * Function:
 *		bcm_esw_failover_set
 * Purpose:
 *		Set a failover object to enable or disable (note that failover object
 *            0 is reserved
 * Parameters:
 *		IN :  unit
 *           IN :  failover_id
 *           IN :  value
 * Returns:
 *		BCM_E_XXX
 */


int 
bcm_esw_failover_set(int unit, bcm_failover_t failover_id, int value)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_SABER2_SUPPORT)
    int type = 0;
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK2_SUPPORT)
    int failover_type=_BCM_FAILOVER_DEFAULT_MODE;
    int local_failover_id=0;
#endif
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    bcm_failover_element_t failover;
#endif

#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_failover) && SOC_IS_KATANA2(unit)) {
        bcm_failover_element_t_init (&failover);
        failover.failover_id = failover_id;
        rv = bcm_tr2_failover_lock (unit);
#if defined(BCM_SABER2_SUPPORT)
        if ((type & _BCM_FAILOVER_1_1_MC_PROTECTION_MODE) &&
                (SOC_IS_SABER2(unit))) {
            if ( rv == BCM_E_NONE ) {
                rv = bcm_sb2_failover_status_set(unit, &failover, value);
                bcm_tr2_failover_unlock (unit);
            }
        } else
#endif
        {
            if ( rv == BCM_E_NONE ) {
                rv = bcm_kt2_failover_status_set(unit, &failover, value);
                bcm_tr2_failover_unlock (unit);
            }
        }
        return rv;
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_feature(unit, soc_feature_failover) && SOC_IS_TRIUMPH3(unit)) {
         bcm_failover_element_t_init (&failover);
         failover.failover_id = failover_id;
         rv = bcm_tr2_failover_lock (unit);
         if ( rv == BCM_E_NONE ) {
               rv = bcm_tr3_failover_status_set(unit, &failover, value);
               bcm_tr2_failover_unlock (unit);
         }
         return rv;
    }
#endif
#ifdef BCM_TRIUMPH2_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_td2p_mpls_linear_protection)) {
        bcm_failover_element_t_init(&failover);
        _BCM_GET_FAILOVER_TYPE(failover_id, failover_type);
        if ((failover_type & _BCM_FAILOVER_INGRESS) ||
            (failover_type & _BCM_FAILOVER_1_1_MC_PROTECTION_MODE)) {
            local_failover_id = failover_id;
            _BCM_GET_FAILOVER_ID(local_failover_id);
            if (failover_type & _BCM_FAILOVER_INGRESS) {
                failover.flags = BCM_FAILOVER_INGRESS;
            } else {
                failover.flags = BCM_FAILOVER_ENCAP;
            }
            failover.failover_id = local_failover_id;
        } else {
            failover.failover_id = failover_id;
        }
        rv = bcm_td2p_failover_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcm_td2p_failover_status_set(unit, &failover, value);
            bcm_td2p_failover_unlock(unit);
        }
        return rv;
    } else
#endif

    if (soc_feature(unit, soc_feature_failover)) {
#if defined(BCM_TOMAHAWK2_SUPPORT)
        if (SOC_IS_TOMAHAWK2(unit)) {
            bcm_failover_element_t_init(&failover);
            _BCM_GET_FAILOVER_TYPE(failover_id, failover_type);
            if (failover_type & _BCM_FAILOVER_INGRESS) {
                local_failover_id = failover_id;
                _BCM_GET_FAILOVER_ID(local_failover_id);
                failover.flags = BCM_FAILOVER_INGRESS;
            } else {
                local_failover_id = failover_id;
            }
            failover.failover_id = local_failover_id;
            rv = bcm_th2_failover_lock(unit);
            if (rv == BCM_E_NONE) {
                if (failover_type & _BCM_FAILOVER_1_1_MC_PROTECTION_MODE) {
                    bcm_th2_failover_unlock(unit);
                    return BCM_E_PARAM;
                }
                rv = bcm_th2_failover_status_set(unit, &failover, value);
                bcm_th2_failover_unlock(unit);
            }
            return rv;
        } else
#endif
        {
            rv = bcm_tr2_failover_lock (unit);
            if ( rv == BCM_E_NONE ) {
                rv = bcm_tr2_failover_set(unit, failover_id, value);
                bcm_tr2_failover_unlock (unit);
            }
            return rv;
        }
    }
#endif
    return rv;
}

/*
 * Function:
 *		bcm_esw_failover_get
 * Purpose:
 *		Get the enable status of a failover object
 * Parameters:
 *		IN :  unit
 *           IN :  failover_id
 *           OUT :  value
 * Returns:
 *		BCM_E_XXX
 */


int 
bcm_esw_failover_get(int unit, bcm_failover_t failover_id, int *value)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_SABER2_SUPPORT)
    int type = 0;
#endif
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
        bcm_failover_element_t failover;
#endif
    
#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_failover) && SOC_IS_KATANA2(unit)) {
        bcm_failover_element_t_init (&failover);
        failover.failover_id = failover_id;
        rv = bcm_tr2_failover_lock (unit);
#if defined(BCM_SABER2_SUPPORT)
        _BCM_GET_FAILOVER_TYPE(failover_id, type);
        if ((type & _BCM_FAILOVER_1_1_MC_PROTECTION_MODE) &&
                (SOC_IS_SABER2(unit))) {
            if ( rv == BCM_E_NONE ) {
                rv = bcm_sb2_failover_status_get(unit, &failover, value);
                bcm_tr2_failover_unlock (unit);
            }
        } else
#endif
        {
        if ( rv == BCM_E_NONE ) {
            rv = bcm_kt2_failover_status_get(unit, &failover, value);
            bcm_tr2_failover_unlock (unit);
        }
        }
        return rv;
    }
#endif
    
#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_feature(unit, soc_feature_failover) && SOC_IS_TRIUMPH3(unit)) {
         bcm_failover_element_t_init (&failover);
         failover.failover_id = failover_id;
         rv = bcm_tr2_failover_lock (unit);
         if ( rv == BCM_E_NONE ) {
               rv = bcm_tr3_failover_status_get(unit, &failover, value);
               bcm_tr2_failover_unlock (unit);
         }
         return rv;
    }
#endif
#ifdef BCM_TRIUMPH2_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_td2p_mpls_linear_protection)) {
        int failover_type     = _BCM_FAILOVER_DEFAULT_MODE;
        int local_failover_id = 0;
        bcm_failover_element_t_init (&failover);
        _BCM_GET_FAILOVER_TYPE(failover_id, failover_type);
        if ((failover_type & _BCM_FAILOVER_INGRESS) ||
            (failover_type & _BCM_FAILOVER_1_1_MC_PROTECTION_MODE)) {
            local_failover_id = failover_id;
            _BCM_GET_FAILOVER_ID(local_failover_id);
            if (failover_type & _BCM_FAILOVER_INGRESS) {
                failover.flags = BCM_FAILOVER_INGRESS;
            } else {
                failover.flags = BCM_FAILOVER_ENCAP;
            }
            failover.failover_id = local_failover_id;
        } else {
            failover.failover_id = failover_id;
        }
        rv = bcm_td2p_failover_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcm_td2p_failover_status_get(unit, &failover, value);
            bcm_td2p_failover_unlock(unit);
        }
        return rv;
    } else
#endif

    if (soc_feature(unit, soc_feature_failover)) {
#if defined(BCM_TOMAHAWK2_SUPPORT)
        if (SOC_IS_TOMAHAWK2(unit)) {
            int failover_type     = _BCM_FAILOVER_DEFAULT_MODE;
            int local_failover_id = 0;            
            bcm_failover_element_t_init (&failover);
            _BCM_GET_FAILOVER_TYPE(failover_id, failover_type);
            if (failover_type & _BCM_FAILOVER_INGRESS) {
                local_failover_id = failover_id;
                _BCM_GET_FAILOVER_ID(local_failover_id);
                failover.flags = BCM_FAILOVER_INGRESS;
            } else {
                local_failover_id = failover_id;
            }
            failover.failover_id = local_failover_id;
            rv = bcm_th2_failover_lock(unit);
            if (rv == BCM_E_NONE) {
                rv = bcm_th2_failover_status_get(unit, &failover, value);
                bcm_th2_failover_unlock(unit);
            }
            return rv;
        } else
#endif
        {
            rv = bcm_tr2_failover_lock (unit);
            if ( rv == BCM_E_NONE ) {
                rv = bcm_tr2_failover_get(unit, failover_id, value);
                bcm_tr2_failover_unlock (unit);
            }
            return rv;
        }
    }
#endif
    return rv;
}

/*
 * Function:
 *		bcm_esw_failover_status_set
 * Purpose:
 *		Set a failover object to enable or disable (note that failover object
 *            0 is reserved
 * Parameters:
 *		IN :  unit
 *           IN :  failover
 *           IN :  value
 * Returns:
 *		BCM_E_XXX
 */


int 
bcm_esw_failover_status_set(int unit,
                                      bcm_failover_element_t *failover,
                                      int enable)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_KATANA2_SUPPORT
    if (soc_feature(unit, soc_feature_failover) && SOC_IS_KATANA2(unit)) {
        rv = bcm_tr2_failover_lock (unit);
#if defined(BCM_SABER2_SUPPORT)
        if ((failover->flags & BCM_FAILOVER_ENCAP) &&
                (SOC_IS_SABER2(unit))) {
            if ( rv == BCM_E_NONE ) {
                rv = bcm_sb2_failover_status_set(unit, failover, enable);
                bcm_tr2_failover_unlock (unit);
            }
        } else  
#endif
        {
            if ( rv == BCM_E_NONE ) {
                rv = bcm_kt2_failover_status_set(unit, failover, enable);
                bcm_tr2_failover_unlock (unit);
            }
        }
        return rv;
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_feature(unit, soc_feature_failover) && SOC_IS_TRIUMPH3(unit)) {
         rv = bcm_tr2_failover_lock (unit);
         if ( rv == BCM_E_NONE ) {
               rv = bcm_tr3_failover_status_set(unit, failover, enable);
               bcm_tr2_failover_unlock (unit);
         }
         return rv;
    }
#endif
#ifdef BCM_TRIUMPH2_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_td2p_mpls_linear_protection)) {
        rv = bcm_td2p_failover_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcm_td2p_failover_status_set(unit, failover, enable);
            bcm_td2p_failover_unlock(unit);
        }
        return rv;
    } else
#endif
    if (soc_feature(unit, soc_feature_failover)) {
#if defined(BCM_TOMAHAWK2_SUPPORT)
        if (SOC_IS_TOMAHAWK2(unit)) {
            rv = bcm_th2_failover_lock(unit);
            if (rv == BCM_E_NONE) {
                rv = bcm_th2_failover_status_set(unit, failover, enable);
                bcm_th2_failover_unlock(unit);
            }
            return rv;
        } else
#endif
        {
            rv = bcm_tr2_failover_lock (unit);
            if ( rv == BCM_E_NONE ) {
                rv = bcm_tr2_failover_set(unit, failover->failover_id, enable);
                bcm_tr2_failover_unlock (unit);
            }
            return rv;
        }
    }
#endif
    return rv;
}

/*
 * Function:
 *		bcm_esw_failover_status_get
 * Purpose:
 *		Get the enable status of a failover object
 * Parameters:
 *		IN :  unit
 *           IN :  failover
 *           OUT :  value
 * Returns:
 *		BCM_E_XXX
 */


int 
bcm_esw_failover_status_get(int unit,
                                      bcm_failover_element_t *failover,
                                      int *value)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_KATANA2_SUPPORT
    if (soc_feature(unit, soc_feature_failover) && SOC_IS_KATANA2(unit)) {
        rv = bcm_tr2_failover_lock (unit);
#if defined(BCM_SABER2_SUPPORT)
        if ((failover->flags & BCM_FAILOVER_ENCAP) &&
                (SOC_IS_SABER2(unit))) {
            if ( rv == BCM_E_NONE ) {
                rv = bcm_sb2_failover_status_get(unit, failover, value);
                bcm_tr2_failover_unlock (unit);
            }
        } else  
#endif
        if ( rv == BCM_E_NONE ) {
            rv = bcm_kt2_failover_status_get(unit, failover, value);
            bcm_tr2_failover_unlock (unit);
        }
        return rv;
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_feature(unit, soc_feature_failover) && SOC_IS_TRIUMPH3(unit)) {
         rv = bcm_tr2_failover_lock (unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr3_failover_status_get(unit, failover, value);
              bcm_tr2_failover_unlock (unit);
         }
         return rv;
    }
#endif
#ifdef BCM_TRIUMPH2_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_td2p_mpls_linear_protection)) {
        rv = bcm_td2p_failover_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcm_td2p_failover_status_get(unit, failover, value);
            bcm_td2p_failover_unlock(unit);
        }
        return rv;
    } else
#endif

    if (soc_feature(unit, soc_feature_failover)) {
#if defined(BCM_TOMAHAWK2_SUPPORT)
        if (SOC_IS_TOMAHAWK2(unit)) {
            rv = bcm_th2_failover_lock(unit);
            if (rv == BCM_E_NONE) {
                rv = bcm_th2_failover_status_get(unit, failover, value);
                bcm_th2_failover_unlock(unit);
            }
            return rv;
        } else
#endif
        {
            rv = bcm_tr2_failover_lock (unit);
            if ( rv == BCM_E_NONE ) {
                rv = bcm_tr2_failover_get(unit, failover->failover_id, value);
                bcm_tr2_failover_unlock (unit);
            }
            return rv;
        }
    }
#endif
    return rv;
}

/*
 * Function:
 *    _bcm_esw_failover_id_check
 * Purpose:
 *    Check Failover identifier
 * Parameters:
 *    IN :  unit
 *    IN :  failover_id
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_esw_failover_id_check(int unit, bcm_failover_t failover_id)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_failover)) {
        rv = bcm_tr2_failover_id_check(unit, failover_id);
    }
#endif
    return rv;
}

/*
 * Function:
 *		_bcm_esw_failover_mpls_check
 * Purpose:
 *		Check Failover for MPLS Port
 * Parameters:
 *		IN :  unit
 *           IN :  mpls_port
 * Returns:
 *		BCM_E_XXX
 */


int
_bcm_esw_failover_mpls_check(int unit, bcm_mpls_port_t  *mpls_port)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_failover) &&
        (SOC_IS_TRIUMPH3(unit) || SOC_IS_KATANA2(unit) ||
         SOC_IS_TRIDENT2PLUS(unit) || (SOC_IS_APACHE(unit)) ||
         soc_feature(unit,soc_feature_failover_mpls_check))) {
         rv = bcm_tr3_failover_mpls_check (unit, mpls_port);
         return rv;
    }
#endif

#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_failover)) {
         rv = bcm_tr2_failover_mpls_check (unit, mpls_port);
         return rv;
    }
#endif
    return rv;
}

/*
 * Function:
 *		_bcm_esw_failover_egr_check
 * Purpose:
 *		Check Failover for Egress Object
 * Parameters:
 *		IN :  unit
 *           IN :  Egress Object
 * Returns:
 *		BCM_E_XXX
 */


int
_bcm_esw_failover_egr_check(int unit, bcm_l3_egress_t  *egr)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_failover) &&
        (SOC_IS_TRIUMPH3(unit) || SOC_IS_KATANA2(unit) ||
         SOC_IS_TRIDENT2PLUS(unit) ||
             soc_feature(unit, soc_feature_egress_failover))) {
         rv = bcm_tr3_failover_egr_check (unit, egr);
         return rv;
    }
#endif

#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_failover)) {
         rv = bcm_tr2_failover_egr_check (unit, egr);
         return rv;
    }
#endif
    return rv;
}

/*
 * Function:
 *      _bcm_esw_failover_extender_check
 * Purpose:
 *      Check Failover for EXTENDER Port
 * Parameters:
 *      IN :  unit
 *      IN :  extender_port
 * Returns:
 *      BCM_E_XXX
 */


int
_bcm_esw_failover_extender_check(int unit, 
                                 bcm_extender_port_t *extender_port)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_failover) &&
        (SOC_IS_TRIUMPH3(unit) || SOC_IS_TD2_TT2(unit))) {
        rv = bcm_tr3_failover_extender_check(unit, extender_port);
        return rv;
    }
#endif

    return rv;
}

/*
 * Function:
 *      _bcm_esw_failover_prot_nhi_create
 * Purpose:
 *      Create Protection next_hop
 * Parameters:
 *      IN :  unit
  *          IN :  Next_Hop_Index
 *           IN :  Egress Object
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_esw_failover_multi_level_prot_nhi_create(int unit, int index, int prot_nh_index,
                                     bcm_multicast_t mc_group,
                                     bcm_failover_t failover_id)
{
    int rv = BCM_E_UNAVAIL;

    if (!(soc_feature(unit, soc_feature_hierarchical_protection))) {
	    return rv;
    }
#if defined(BCM_MULTI_LEVEL_FAILOVER_SUPPORT)
        BCM_IF_ERROR_RETURN(bcmi_failover_multi_level_prot_nhi_create(unit, index));
        rv = bcmi_failover_multi_level_prot_nhi_set(unit, index,
                prot_nh_index, mc_group, failover_id );
        return rv;
#endif
    return BCM_E_NONE;
}

int
bcmi_esw_failover_multi_level_prot_nhi_get(int unit, int nh_index,
        bcm_failover_t  *failover_id, int  *prot_nh_index,
        bcm_multicast_t  *mc_group)
{
    int rv = BCM_E_UNAVAIL;

    if (!(soc_feature(unit, soc_feature_hierarchical_protection))) {
        return rv;
    }

#if defined(BCM_MULTI_LEVEL_FAILOVER_SUPPORT)
        rv = bcmi_failover_multi_level_prot_nhi_get(unit, nh_index,
                failover_id, prot_nh_index,  mc_group);
#endif
    return rv;
}

int
bcmi_esw_failover_multi_level_prot_nhi_cleanup (int unit, int nh_index)
{

    int rv = BCM_E_UNAVAIL;

    if (!(soc_feature(unit, soc_feature_hierarchical_protection))) {
        return rv;
    }
#if defined(BCM_MULTI_LEVEL_FAILOVER_SUPPORT)
        rv = bcmi_failover_multi_level_prot_nhi_cleanup(unit, nh_index);
#endif

    return rv;
}


/*
 * Function:
 *		_bcm_esw_failover_prot_nhi_create
 * Purpose:
 *		Create Protection next_hop
 * Parameters:
 *		IN :  unit
  *          IN :  Next_Hop_Index
 *           IN :  Egress Object
 * Returns:
 *		BCM_E_XXX
 */


int
_bcm_esw_failover_prot_nhi_create(int unit, uint32 flags,
                                              int nh_index, int prot_nh_index,
                                              bcm_multicast_t mc_group,
                                              bcm_failover_t failover_id)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_SABER2_SUPPORT)
    int type;
    bcm_failover_element_t failover;
#endif

#ifdef BCM_MULTI_LEVEL_FAILOVER_SUPPORT
    if(soc_feature(unit, soc_feature_hierarchical_protection)) {
        if (_BCM_FAILOVER_IS_MULTI_LEVEL(failover_id)) {
            rv = bcmi_esw_failover_multi_level_prot_nhi_create(unit, nh_index,
                     prot_nh_index, mc_group, failover_id);
            return rv;
        }
    }
#endif

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_failover) &&
        (SOC_IS_TRIUMPH3(unit) || SOC_IS_KATANA2(unit))) {
#if defined(BCM_SABER2_SUPPORT)
        _BCM_GET_FAILOVER_TYPE(failover_id, type);
        if ((type & _BCM_FAILOVER_1_1_MC_PROTECTION_MODE) &&
                SOC_IS_SABER2(unit)) {
            _BCM_GET_FAILOVER_ID(failover_id);
            rv = bcm_sb2_failover_prot_nhi_set(unit, flags, nh_index, 
                    prot_nh_index, mc_group, failover_id );
                bcm_failover_element_t_init (&failover);
                failover.failover_id = failover_id;
                rv = bcm_tr2_failover_lock (unit);
                if ( rv == BCM_E_NONE ) {
                    rv = bcm_sb2_failover_status_set(unit, &failover, FALSE);
                    bcm_tr2_failover_unlock (unit);
                }
            return rv;
        } else
#endif
        {

            BCM_IF_ERROR_RETURN(bcm_tr2_failover_prot_nhi_create( unit, nh_index ));
            rv = bcm_tr3_failover_prot_nhi_set(unit, flags, nh_index, 
                    prot_nh_index, mc_group, failover_id );
            return rv;
        }
    }
#endif

#ifdef BCM_TRIUMPH2_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_td2p_mpls_linear_protection)) {
        BCM_IF_ERROR_RETURN(bcm_tr2_failover_prot_nhi_create(unit, nh_index));
        rv = bcm_td2p_failover_prot_nhi_set(unit, flags, nh_index, 
                prot_nh_index, mc_group, failover_id );
        return rv;
    } else
#endif
    if (soc_feature(unit, soc_feature_failover)) {
        BCM_IF_ERROR_RETURN(bcm_tr2_failover_prot_nhi_create( unit, nh_index ));
        rv = bcm_tr2_failover_prot_nhi_set(unit, nh_index,
                         prot_nh_index, failover_id );
        return rv;
    }
#endif
    return rv;
}

/*
 * Function:
 *		_bcm_esw_failover_prot_nhi_cleanup
 * Purpose:
 *		Cleanup protection Next_hop
 * Parameters:
 *		IN :  unit
  *          IN :  Next_Hop_Index
 * Returns:
 *		BCM_E_XXX
 */


int
_bcm_esw_failover_prot_nhi_cleanup(int unit, int nh_index)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_MULTI_LEVEL_FAILOVER_SUPPORT
    if(soc_feature(unit, soc_feature_hierarchical_protection)) {
        bcm_failover_t failoverid;
        _BCM_FAILOVER_MULTI_LEVEL_FAILOVER_ID_GET(unit, nh_index, failoverid);
        if (_BCM_FAILOVER_IS_MULTI_LEVEL(failoverid)) {
            rv = bcmi_esw_failover_multi_level_prot_nhi_cleanup(unit, nh_index);

            return rv;
        } else if (bcmi_l3_nh_multi_count_get(unit, (nh_index - 1)) > 1) {
            return BCM_E_NONE;
        }
    }
#endif

#ifdef BCM_KATANA2_SUPPORT
    if (soc_feature(unit, soc_feature_failover) && SOC_IS_KATANA2(unit)) {
        rv = bcm_kt2_failover_prot_nhi_cleanup  (unit, nh_index);
        return rv;
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_feature(unit, soc_feature_failover) && SOC_IS_TRIUMPH3(unit)) {
         rv = bcm_tr3_failover_prot_nhi_cleanup  (unit, nh_index);
         return rv;
    }
#endif

#ifdef BCM_TRIUMPH2_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_td2p_mpls_linear_protection)) {
        rv = bcm_td2p_failover_prot_nhi_cleanup(unit, nh_index);
        return rv;
    } else
#endif
    if (soc_feature(unit, soc_feature_failover)) {
         rv = bcm_tr2_failover_prot_nhi_cleanup  (unit, nh_index);
         return rv;
    }
#endif
    return rv;
}

/*
 * Function:
 *		_bcm_esw_failover_prot_nhi_get
 * Purpose:
 *		Cleanup protection Next_hop
 * Parameters:
 *		IN :  unit
  *          IN :  Next_Hop_Index
 * Returns:
 *		BCM_E_XXX
 */

int
_bcm_esw_failover_prot_nhi_get (int unit, int nh_index,
         bcm_failover_t *failover_id, int *prot_nh_index, int *multicast_group)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_MULTI_LEVEL_FAILOVER_SUPPORT
    if(soc_feature(unit, soc_feature_hierarchical_protection)) {
        bcm_failover_t failoverid;
        if (nh_index > 0) {
            _BCM_FAILOVER_MULTI_LEVEL_FAILOVER_ID_GET(
                unit, nh_index, failoverid);
            if (_BCM_FAILOVER_IS_MULTI_LEVEL(failoverid)) {
			    rv = bcmi_esw_failover_multi_level_prot_nhi_get
                    (unit, nh_index, failover_id, prot_nh_index,
                    multicast_group);
                return rv;
            } else {
               if (bcmi_l3_nh_multi_count_get(unit, (nh_index - 1)) > 1) {
                   return BCM_E_NONE;
               }
            }
        }
    }
#endif

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_failover) &&
        (SOC_IS_TRIUMPH3(unit) || SOC_IS_KATANA2(unit))) {
         rv = bcm_tr3_failover_prot_nhi_get (unit, nh_index, 
                        failover_id, prot_nh_index, multicast_group);
         return rv;
    }
#endif

#ifdef BCM_TRIUMPH2_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_td2p_mpls_linear_protection)) {
        rv = bcm_td2p_failover_prot_nhi_get(unit, nh_index, 
                failover_id, prot_nh_index, multicast_group);
        return rv;
    } else
#endif
    if (soc_feature(unit, soc_feature_failover)) {
         rv = bcm_tr2_failover_prot_nhi_get  (unit, nh_index, 
                        failover_id, prot_nh_index);
         return rv;
    }
#endif
    return rv;
}

/*
 * Function:
 *		_bcm_esw_failover_prot_nhi_update
 * Purpose:
 *		Update protection Next_hop
 * Parameters:
 *		IN :  unit
  *          IN :  Next_Hop_Index
 * Returns:
 *		BCM_E_XXX
 */

int
_bcm_esw_failover_prot_nhi_update (int unit,
                                                int old_nh_index,
                                                int new_nh_index)

{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_failover) &&
        (SOC_IS_TRIUMPH3(unit) || SOC_IS_KATANA2(unit))) {
        rv = bcm_tr3_failover_prot_nhi_update (unit,
                old_nh_index, new_nh_index);
        return rv;
    }
#endif

#ifdef BCM_TRIUMPH2_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_td2p_mpls_linear_protection)) {
        rv = bcm_td2p_failover_prot_nhi_update(unit,
                old_nh_index, new_nh_index);
        return rv;
    } else
#endif
    if (soc_feature(unit, soc_feature_failover)) {
        rv = bcm_tr2_failover_prot_nhi_update (unit,
                old_nh_index, new_nh_index);
        return rv;
    }
#endif
    return rv;
}

/*
 * Function:
 *    _bcm_esw_failover_ecmp_prot_nhi_create
 * Purpose:
 *    Create Protection next_hop for a ecmp member
 * Parameters:
 *    IN :  unit
 *    IN :  ecmp index /dvp group pointer
 *    IN :  member offset from base ptr
 *    IN :  Next_Hop_Index
 *    IN :  failover identifier
 *    IN :  Protect Object
 * Returns:
 *    BCM_E_XXX
 */

int
_bcm_esw_failover_ecmp_prot_nhi_create(int unit, int ecmp, int index,
                                       int nh_index, bcm_failover_t failover_id,
                                       int prot_nh_index)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_ecmp_failover)) {
        rv = bcm_tr2_failover_ecmp_prot_nhi_set(unit, ecmp, index, nh_index,
                                                failover_id, prot_nh_index);
    }
#endif

    return rv;
}

/*
 * Function:
 *   _bcm_esw_failover_ecmp_prot_nhi_cleanup
 * Purpose:
 *    Cleanup protection Next_hop  for a ecmp member
 * Parameters:
 *    IN :  unit
 *    IN :  ecmp index /dvp group pointer
 *    IN :  member offset from base ptr
 *    IN :  Next_Hop_Index
 * Returns:
 *    BCM_E_XXX
 */

int
_bcm_esw_failover_ecmp_prot_nhi_cleanup(int unit, int ecmp, int index, 
                                        int nh_index)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_ecmp_failover)) {
        rv = bcm_tr2_failover_ecmp_prot_nhi_cleanup(unit, ecmp, index, 
                                                    nh_index);
    }
#endif

    return rv;
}

/*
 * Function:
 *    _bcm_esw_failover_ecmp_prot_nhi_get
 * Purpose:
 *    Cleanup protection Next_hop  for a ecmp member
 * Parameters:
 *    IN :  unit
 *    IN :  ecmp index /dvp group pointer
 *    IN :  member offset from base ptr
 *    IN :  Next_Hop_Index
 *    OUT :  failover identifier
 *    OUT :  Protect Object
 * Returns:
 *    BCM_E_XXX
 */

int
_bcm_esw_failover_ecmp_prot_nhi_get(int unit, int ecmp, int index, 
                                    int nh_index, bcm_failover_t *failover_id, 
                                    int *prot_nh_index)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_ecmp_failover)) {
        rv = bcm_tr2_failover_ecmp_prot_nhi_get(unit, ecmp, index, nh_index, 
                                                failover_id, prot_nh_index);
    }
#endif

    return rv;
}

/*
 * Function:
 *    _bcm_esw_failover_ecmp_prot_nhi_update
 * Purpose:
 *    Update protection Next_hop  for a ecmp member
 * Parameters:
 *    IN :  unit
 *    IN :  ecmp index /dvp group pointer
 *    IN :  Next_Hop_Index
 * Returns:
 *    BCM_E_XXX
 */

int
_bcm_esw_failover_ecmp_prot_nhi_update(int unit, int ecmp, int index, 
                                       int old_nh_index, int new_nh_index)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_ecmp_failover)) {
        rv = bcm_tr2_failover_ecmp_prot_nhi_set(unit, ecmp, index,
                                                old_nh_index, 
                                                BCM_FAILOVER_INVALID, 
                                                new_nh_index);
    }
#endif

    return rv;
}

/*
 * Function:
 *    bcm_esw_failover_multi_level_attach
 * Purpose:
 *    Attaches multiple levels of failover together.
 * Parameters:
 *    unit                 - (IN)bcm device id
 *    multi_level_failover - (IN)iformation strcut to attch multiple levels.
 * Returns:
 *    BCM_E_XXX
 */
int
bcm_esw_failover_multi_level_attach(
    int unit,
    bcm_failover_multi_level_t multi_level_failover)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_MULTI_LEVEL_FAILOVER_SUPPORT
    rv = bcm_td2p_failover_lock(unit);
    if (rv == BCM_E_NONE) {
        rv = bcmi_failover_multi_level_attach(unit, multi_level_failover);
        bcm_td2p_failover_unlock(unit);
    }
#endif

    return rv;
}
/*
 * Function:
 *    bcmi_esw_failover_dependent_free_resources
 * Purpose:
 *    frees l3 dependednt resources from other modules.
 * Parameters:
 *    unit                 - (IN)bcm device id
 * Returns:
 *    NONE
 */
void
bcmi_esw_failover_dependent_free_resources(int unit)
{
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    bcmi_failover_dependent_free_resources(unit);
#endif
}

/* Failover with Fixed NH Offset */
int
bcm_esw_failover_egress_set(int unit, bcm_if_t intf,
                            bcm_l3_egress_t *failover_egr)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_failover_fixed_nh_offset) && SOC_IS_TOMAHAWK3(unit)) {
         return bcm_th3_failover_egress_set(unit, intf, failover_egr);
    }
#endif

    return rv;
}

int
bcm_esw_failover_egress_get(int unit, bcm_if_t intf,
                            bcm_l3_egress_t *failover_egr)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_failover_fixed_nh_offset) && SOC_IS_TOMAHAWK3(unit)) {
         return bcm_th3_failover_egress_get(unit, intf, failover_egr);
    }
#endif

    return rv;
}

int
bcm_esw_failover_egress_clear(int unit, bcm_if_t intf)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_failover_fixed_nh_offset) && SOC_IS_TOMAHAWK3(unit)) {
         return bcm_th3_failover_egress_clear(unit, intf);
    }
#endif

    return rv;
}

int
bcm_esw_failover_egress_status_set(int unit, bcm_if_t intf, int enable)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_failover_fixed_nh_offset) && SOC_IS_TOMAHAWK3(unit)) {
         return bcm_th3_failover_egress_status_set(unit, intf, enable);
    }
#endif

    return rv;
}

int
bcm_esw_failover_egress_status_get(int unit, bcm_if_t intf, int *enable)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_failover_fixed_nh_offset) && SOC_IS_TOMAHAWK3(unit)) {
         return bcm_th3_failover_egress_status_get(unit, intf, enable);
    }
#endif

    return rv;
}

/*
 * Function:
 *    bcm_esw_failover_egress_protection_get
 * Purpose:
 *    Get the protection next hop interface for the primary next hop interface.
 * Parameters:
 *    unit      : (IN) Device unit.
 *    intf      : (IN) Primary NH interface.
 *    prot_intf : (OUT) Protection NH interface for the primary NH interface.
 * Returns:
 *    BCM_E_XXX
 */
int
bcm_esw_failover_egress_protection_get(
    int unit,
    bcm_if_t intf,
    bcm_if_t *prot_intf)
{
    return BCM_E_UNAVAIL;
}

#else   /* INCLUDE_L3 */
typedef int bcm_esw_failover_not_empty; /* Make ISO compilers happy. */
#endif  /* INCLUDE_L3 */

int
bcm_esw_failover_ring_config_get(int unit,
                                             bcm_failover_ring_t *failover_ring)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_failover) &&
        (SOC_IS_TRIUMPH3(unit) || SOC_IS_KATANA2(unit))) {
         return bcm_tr3_failover_ring_config_get(unit, failover_ring);
    }
#endif

    return rv;
}

int
bcm_esw_failover_ring_config_set(int unit,
                                            bcm_failover_ring_t *failover_ring)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA2_SUPPORT)

    if (soc_feature(unit, soc_feature_failover) &&
        (SOC_IS_TRIUMPH3(unit) || SOC_IS_KATANA2(unit))) {
         return bcm_tr3_failover_ring_config_set(unit, failover_ring);
    }
#endif

    return rv;
}

