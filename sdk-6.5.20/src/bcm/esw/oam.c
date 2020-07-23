/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/defs.h>
#include <soc/drv.h>

#include <bcm/oam.h>
#include <bcm/trunk.h>
#include <bcm/error.h>
#include <bcm/field.h>
#include <bcm_int/esw/oam.h>
#include <bcm_int/esw_dispatch.h>
#include <shared/bslenum.h>
#include <shared/bsl.h>

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT) || \
    defined(BCM_VALKYRIE2_SUPPORT)
#include <bcm_int/esw/triumph2.h>
#endif

#if defined(BCM_TRIUMPH2_SUPPORT)
#include <bcm_int/esw/triumph3.h>
#endif

#if defined(BCM_ENDURO_SUPPORT)
#include <bcm_int/esw/enduro.h>
#endif

#if defined(BCM_TRIUMPH3_SUPPORT)
#include <bcm_int/esw/triumph3.h>
#endif

#if defined(BCM_KATANA2_SUPPORT)
#include <bcm_int/esw/katana2.h>
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/trident2plus.h>
#endif

#if defined (BCM_SABER2_SUPPORT)
#include <bcm_int/esw/saber2.h>
#endif

#if defined (BCM_MONTEREY_SUPPORT)
#include <bcm_int/esw/apache.h>
#endif

#if defined (BCM_APACHE_SUPPORT)
#include <bcm_int/esw/apache.h>
#endif

#if defined (BCM_METROLITE_SUPPORT)
#include <bcm_int/esw/metrolite.h>
#endif

/*
 * Macro:
 *     _BCM_OAM_ALLOC
 * Purpose:
 *      Generic memory allocation routine.
 * Parameters:
 *    _ptr_     - Pointer to allocated memory.
 *    _ptype_   - Pointer type.
 *    _size_    - Size of heap memory to be allocated.
 *    _descr_   - Information about this memory allocation.
 */
#define _BCM_OAM_ALLOC(_ptr_,_ptype_,_size_,_descr_)                     \
            do {                                                         \
                if (NULL == (_ptr_)) {                                   \
                   (_ptr_) = (_ptype_ *) sal_alloc((_size_), (_descr_)); \
                }                                                        \
                if((_ptr_) != NULL) {                                    \
                    sal_memset((_ptr_), 0, (_size_));                    \
                }  else {                                                \
                    LOG_ERROR(BSL_LS_BCM_OAM, \
                              (BSL_META("OAM Error: Allocation failure %s\n"), \
                               (_descr_)));                              \
                }                                                        \
            } while (0)

static sal_mutex_t mutex[BCM_MAX_NUM_UNITS];

int bcm_esw_oam_lock(int unit)
{
    if (mutex[unit] == NULL)
    {
        return BCM_E_INIT;
    }

    sal_mutex_take(mutex[unit], sal_mutex_FOREVER);

    return BCM_E_NONE;
}

int bcm_esw_oam_unlock(int unit)
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
 *      bcm_esw_oam_init
 * Purpose:
 *      Initialize the OAM subsystem
 * Parameters:
 *      unit - (IN) Unit number.
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_oam_init(int unit)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT) || \
    defined(BCM_VALKYRIE2_SUPPORT)
    if (soc_feature(unit, soc_feature_oam))
    {
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT) ||\
    defined(BCM_GREYHOUND_SUPPORT)
        if ((SOC_IS_TRIUMPH3(unit)) || SOC_IS_HURRICANE2(unit) ||
            SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
            SOC_IS_GREYHOUND2(unit)) {
            result = bcm_tr3_oam_init(unit);
        } else
#endif
/* KT2 OAM */
#if defined(BCM_KATANA2_SUPPORT)
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                return bcm_sb2_oam_init(unit);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
                result = bcm_kt2_oam_init(unit);
            }else
#endif
        {
            if (mutex[unit] == NULL)
            {
                mutex[unit] = sal_mutex_create("oam.mutex");

                if (mutex[unit] == NULL)
                {
                    return BCM_E_MEMORY;
                }
            }
#if defined(BCM_ENDURO_SUPPORT)
            if (SOC_IS_ENDURO(unit) || SOC_IS_KATANAX(unit))
            {
                result = bcm_en_oam_init(unit);
            } else
#endif
            {
                result = bcm_tr2x_oam_init(unit);
            }

            if (BCM_FAILURE(result))
            {
                sal_mutex_destroy(mutex[unit]);

                mutex[unit] = NULL;
            }
        }
    }
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if(soc_feature(unit, soc_feature_fp_based_oam)) {
        result = bcm_td2p_oam_init(unit);
    }
#endif
    return result;
}

/*
 * Function:
 *      bcm_esw_oam_detach
 * Purpose:
 *      Shut down the OAM subsystem
 * Parameters:
 *      unit - (IN) Unit number.
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_oam_detach(int unit)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT) || \
    defined(BCM_VALKYRIE2_SUPPORT)
    if (soc_feature(unit, soc_feature_oam))
    {
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT) ||\
	defined(BCM_GREYHOUND_SUPPORT)
        if ((SOC_IS_TRIUMPH3(unit)) || SOC_IS_HURRICANE2(unit) ||
	     SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
	     SOC_IS_GREYHOUND2(unit)) {
            result = bcm_tr3_oam_detach(unit);
        } else
#endif
/* KT2 OAM */
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                result = bcm_sb2_oam_detach(unit);
#endif
            }else if (SOC_IS_KATANA2(unit)) {
                result = bcm_kt2_oam_detach(unit);
            }
        } else
#endif
        {
            if (mutex[unit] == NULL)
            {
                return BCM_E_NONE;
            }

            BCM_IF_ERROR_RETURN(bcm_esw_oam_lock(unit));

#if defined(BCM_ENDURO_SUPPORT)
            if (SOC_IS_ENDURO(unit) || SOC_IS_KATANAX(unit))
            {
                result = bcm_en_oam_detach(unit);
            } else
#endif
            {
                result = bcm_tr2x_oam_detach(unit);
            }

            BCM_IF_ERROR_RETURN(bcm_esw_oam_unlock(unit));

            sal_mutex_destroy(mutex[unit]);

            mutex[unit] = NULL;
        }
    }
#endif

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if(soc_feature(unit, soc_feature_fp_based_oam)) {
#if defined (INCLUDE_CCM) || defined (INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
        result = bcm_fp_oam_detach(unit);
#endif /* INCLUDE_CCM || INCLUDE_BHH || INCLUDE_MPLS_LM_DM */
    }
#endif
    return result;
}

/*
 * Function:
 *      bcm_esw_oam_group_create
 * Purpose:
 *      Create or replace an OAM group object
 * Parameters:
 *      unit - (IN) Unit number.
 *      group_info - (IN/OUT) Pointer to an OAM group structure
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_oam_group_create(
    int unit, 
    bcm_oam_group_info_t *group_info)
{
    int result = BCM_E_UNAVAIL;

    if (NULL == group_info) {
        /* Input parameter check. */
        return (BCM_E_PARAM);
    }

#if defined(INCLUDE_CCM) || defined(INCLUDE_BHH)
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_fp_based_oam) && 
        (soc_feature(unit, soc_feature_uc_ccm) || 
         soc_feature(unit, soc_feature_bhh)))
    {
        result = bcm_fp_oam_group_create(unit, group_info);
    }
#endif
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT) || \
    defined(BCM_VALKYRIE2_SUPPORT)
    if (soc_feature(unit, soc_feature_oam))
    {
        /* Check if alarm priority value is in valid range. */
        if ((group_info->lowest_alarm_priority
                < bcmOAMGroupFaultAlarmPriorityDefectsAll)
            || (group_info->lowest_alarm_priority
                > bcmOAMGroupFaultAlarmPriorityDefectsNone)) 
        {
            return BCM_E_PARAM;
        }

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
        if ((SOC_IS_TRIUMPH3(unit)) || SOC_IS_HURRICANE2(unit) ||
	     SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
	     SOC_IS_GREYHOUND2(unit)) {
            result = bcm_tr3_oam_group_create(unit, group_info);
        } else
#endif
#if defined(BCM_KATANA2_SUPPORT)
        if(SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
            result = bcm_sb2_oam_group_create(unit, group_info);
#endif
        }
        else if (SOC_IS_KATANA2(unit)) {
            result = bcm_kt2_oam_group_create(unit, group_info);
        } else
#endif
        {
            BCM_IF_ERROR_RETURN(bcm_esw_oam_lock(unit));

#if defined(BCM_ENDURO_SUPPORT)
        if (SOC_IS_ENDURO(unit) || SOC_IS_KATANAX(unit))
        {
            result = bcm_en_oam_group_create(unit, group_info);
        } else
#endif
            {
                result = bcm_tr2x_oam_group_create(unit, group_info);
            }

            BCM_IF_ERROR_RETURN(bcm_esw_oam_unlock(unit));
        }
    }
#endif

    return result;
}

/*
 * Function:
 *      bcm_esw_oam_group_get
 * Purpose:
 *      Get an OAM group object
 * Parameters:
 *      unit - (IN) Unit number.
 *      group - (IN) The ID of the group object to get
 *      group_info - (OUT) Pointer to an OAM group structure to receive the data
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_oam_group_get(
    int unit, 
    bcm_oam_group_t group, 
    bcm_oam_group_info_t *group_info)
{
    int result = BCM_E_UNAVAIL;

    if (NULL == group_info) {
        /* Input parameter check. */
        return (BCM_E_PARAM);
    }

#if defined(INCLUDE_CCM) || defined(INCLUDE_BHH)
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_fp_based_oam) && 
        (soc_feature(unit, soc_feature_uc_ccm) || 
         soc_feature(unit, soc_feature_bhh)))
    {
        result = bcm_fp_oam_group_get(unit, group, group_info);
    }
#endif
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT) || \
    defined(BCM_VALKYRIE2_SUPPORT)
    if (soc_feature(unit, soc_feature_oam))
    {

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
        if ((SOC_IS_TRIUMPH3(unit)) || SOC_IS_HURRICANE2(unit) || 
            SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
            SOC_IS_GREYHOUND2(unit)) {
            result = bcm_tr3_oam_group_get(unit, group, group_info);
        } else
#endif
/* KT2 OAM  */
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                result = bcm_sb2_oam_group_get(unit, group, group_info);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
                result = bcm_kt2_oam_group_get(unit, group, group_info);
            }
        } else
#endif
        {
            BCM_IF_ERROR_RETURN(bcm_esw_oam_lock(unit));
#if defined(BCM_ENDURO_SUPPORT)
            if (SOC_IS_ENDURO(unit) || SOC_IS_KATANAX(unit))
            {
                result = bcm_en_oam_group_get(unit, group, group_info);
            } else
#endif
            {
                result = bcm_tr2x_oam_group_get(unit, group, group_info);
            }

            BCM_IF_ERROR_RETURN(bcm_esw_oam_unlock(unit));
        }
    }
#endif

    return result;
}

/*
 * Function:
 *      bcm_esw_oam_group_destroy
 * Purpose:
 *      Destroy an OAM group object.  All OAM endpoints associated
 *      with the group will also be destroyed.
 * Parameters:
 *      unit - (IN) Unit number.
 *      group - (IN) The ID of the OAM group object to destroy
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_oam_group_destroy(
    int unit, 
    bcm_oam_group_t group)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_CCM) || defined(INCLUDE_BHH)
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_fp_based_oam) && 
        (soc_feature(unit, soc_feature_uc_ccm) || 
         soc_feature(unit, soc_feature_bhh)))
    {
        result = bcm_fp_oam_group_destroy(unit, group);
    }
#endif
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT) || \
    defined(BCM_VALKYRIE2_SUPPORT)
    if (soc_feature(unit, soc_feature_oam))
    {

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
        if ((SOC_IS_TRIUMPH3(unit)) || SOC_IS_HURRICANE2(unit) ||
            SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
            SOC_IS_GREYHOUND2(unit)) {
            result = bcm_tr3_oam_group_destroy(unit, group);
        } else
#endif
/* KT2 OAM  */
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                result = bcm_sb2_oam_group_destroy(unit, group);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
                result = bcm_kt2_oam_group_destroy(unit, group);
            }
        } else
#endif
        {
            BCM_IF_ERROR_RETURN(bcm_esw_oam_lock(unit));
#if defined(BCM_ENDURO_SUPPORT)
            if (SOC_IS_ENDURO(unit) || SOC_IS_KATANAX(unit))
            {
                result = bcm_en_oam_group_destroy(unit, group);
            } else
#endif
            {
                result = bcm_tr2x_oam_group_destroy(unit, group);
            }

            BCM_IF_ERROR_RETURN(bcm_esw_oam_unlock(unit));
        }
    }
#endif

    return result;
}

/*
 * Function:
 *      bcm_esw_oam_group_destroy_all
 * Purpose:
 *      Destroy all OAM group objects.  All OAM endpoints will also be
 *      destroyed.
 * Parameters:
 *      unit - (IN) Unit number.
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_oam_group_destroy_all(
    int unit)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_CCM) || defined(INCLUDE_BHH)
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_fp_based_oam) && 
        (soc_feature(unit, soc_feature_bhh)))
    {
        result = bcm_fp_oam_group_destroy_all(unit);
    }
#endif
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT) || \
    defined(BCM_VALKYRIE2_SUPPORT)
    if (soc_feature(unit, soc_feature_oam))
    {
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
        if ((SOC_IS_TRIUMPH3(unit)) || SOC_IS_HURRICANE2(unit) ||
            SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
            SOC_IS_GREYHOUND2(unit)) {
            result = bcm_tr3_oam_group_destroy_all(unit);
        } else
#endif
/* KT2 OAM */
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                result = bcm_sb2_oam_group_destroy_all(unit);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
                result = bcm_kt2_oam_group_destroy_all(unit);
            }
        } else
#endif
        {
            BCM_IF_ERROR_RETURN(bcm_esw_oam_lock(unit));

#if defined(BCM_ENDURO_SUPPORT)
        if (SOC_IS_ENDURO(unit) || SOC_IS_KATANAX(unit))
        {
            result = bcm_en_oam_group_destroy_all(unit);
        } else
#endif
            {
                result = bcm_tr2x_oam_group_destroy_all(unit);
            }

            BCM_IF_ERROR_RETURN(bcm_esw_oam_unlock(unit));
        }
    }
#endif

    return result;
}

/*
 * Function:
 *      bcm_esw_oam_group_traverse
 * Purpose:
 *      Traverse the entire set of OAM groups, calling a specified
 *      callback for each one
 * Parameters:
 *      unit - (IN) Unit number.
 *      cb - (IN) A pointer to the callback function to call for each OAM group
 *      user_data - (IN) Pointer to user data to supply in the callback
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_oam_group_traverse(
    int unit, 
    bcm_oam_group_traverse_cb cb, 
    void *user_data)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_CCM) || defined(INCLUDE_BHH)
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_fp_based_oam) && 
        (soc_feature(unit, soc_feature_uc_ccm) || 
         soc_feature(unit, soc_feature_bhh)))
    {
        result = bcm_fp_oam_group_traverse(unit, cb, user_data);
    }
#endif
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT) || \
    defined(BCM_VALKYRIE2_SUPPORT)
    if (soc_feature(unit, soc_feature_oam))
    {

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
        if ((SOC_IS_TRIUMPH3(unit)) || SOC_IS_HURRICANE2(unit)||
             SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
             SOC_IS_GREYHOUND2(unit)) {
            result = bcm_tr3_oam_group_traverse(unit, cb, user_data);
        } else
#endif
/* KT2 OAM  */
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                result = bcm_sb2_oam_group_traverse(unit, cb, user_data);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
                result = bcm_kt2_oam_group_traverse(unit, cb, user_data);
            }
        } else 
#endif
        {
            BCM_IF_ERROR_RETURN(bcm_esw_oam_lock(unit));
#if defined(BCM_ENDURO_SUPPORT)
            if (SOC_IS_ENDURO(unit) || SOC_IS_KATANAX(unit))
            {
                result = bcm_en_oam_group_traverse(unit, cb, user_data);
            } else
#endif
            {
                result = bcm_tr2x_oam_group_traverse(unit, cb, user_data);
            }

            BCM_IF_ERROR_RETURN(bcm_esw_oam_unlock(unit));
        }
    }
#endif

    return result;
}

/*
 * Function:
 *      bcm_esw_oam_endpoint_create
 * Purpose:
 *      Create or replace an OAM endpoint object
 * Parameters:
 *      unit - (IN) Unit number.
 *      endpoint_info - (IN/OUT) Pointer to an OAM endpoint structure
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_oam_endpoint_create(
    int unit, 
    bcm_oam_endpoint_info_t *endpoint_info)
{
    int result = BCM_E_UNAVAIL;

    if (NULL == endpoint_info) {
        /* Input parameter check. */
        return (BCM_E_PARAM);
    }
#if defined(INCLUDE_CCM) || defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_fp_based_oam) &&
        (soc_feature(unit, soc_feature_uc_ccm) ||
         soc_feature(unit, soc_feature_bhh)    ||
         soc_feature(unit, soc_feature_mpls_lm_dm)))
    {
        result = bcm_fp_oam_endpoint_create(unit, endpoint_info);
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#endif /* INCLUDE_CCM || INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT) || \
    defined(BCM_VALKYRIE2_SUPPORT)
    if (soc_feature(unit, soc_feature_oam))
    {

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
        if ((SOC_IS_TRIUMPH3(unit)) || SOC_IS_HURRICANE2(unit)
            || SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit)
            || SOC_IS_GREYHOUND2(unit)) {
            result = bcm_tr3_oam_endpoint_create(unit, endpoint_info);
        } else
#endif
/* KT2 OAM */
#if defined(BCM_KATANA2_SUPPORT)
        if(SOC_IS_SABER2(unit)){
#if defined (BCM_SABER2_SUPPORT)
            result = bcm_sb2_oam_endpoint_create(unit, endpoint_info);
#endif
        }
        else if (SOC_IS_KATANA2(unit)) {
            result = bcm_kt2_oam_endpoint_create(unit, endpoint_info);
        } else
#endif
        {
            BCM_IF_ERROR_RETURN(bcm_esw_oam_lock(unit));
#if defined(BCM_ENDURO_SUPPORT)
            if (SOC_IS_ENDURO(unit) || SOC_IS_KATANAX(unit))
            {
                result = bcm_en_oam_endpoint_create(unit, endpoint_info);
            } else
#endif
            {
                result = bcm_tr2x_oam_endpoint_create(unit, endpoint_info);
            }

            BCM_IF_ERROR_RETURN(bcm_esw_oam_unlock(unit));
        }
    }
#endif

    return result;
}

/*
 * Function:
 *      bcm_esw_oam_endpoint_get
 * Purpose:
 *      Get an OAM endpoint object
 * Parameters:
 *      unit - (IN) Unit number.
 *      endpoint - (IN) The ID of the endpoint object to get
 *      endpoint_info - (OUT) Pointer to an OAM endpoint structure to receive the data
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_oam_endpoint_get(
    int unit, 
    bcm_oam_endpoint_t endpoint, 
    bcm_oam_endpoint_info_t *endpoint_info)
{
    int result = BCM_E_UNAVAIL;

    if (NULL == endpoint_info) {
        /* Input parameter check. */
        return (BCM_E_PARAM);
    }

#if defined(INCLUDE_CCM) || defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_fp_based_oam) &&
        (soc_feature(unit, soc_feature_uc_ccm) ||
         soc_feature(unit, soc_feature_bhh)    ||
         soc_feature(unit, soc_feature_mpls_lm_dm)))
    {
        result = bcm_fp_oam_endpoint_get(unit, endpoint, endpoint_info);
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#endif /* INCLUDE_CCM || INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT) || \
    defined(BCM_VALKYRIE2_SUPPORT)
    if (soc_feature(unit, soc_feature_oam))
    {

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
        if ((SOC_IS_TRIUMPH3(unit)) || SOC_IS_HURRICANE2(unit)||
            SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
            SOC_IS_GREYHOUND2(unit)) {
            result = bcm_tr3_oam_endpoint_get(unit, endpoint, endpoint_info);
        } else
#endif
/* KT2 OAM */
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                result = bcm_sb2_oam_endpoint_get(unit, endpoint, endpoint_info);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
                result = bcm_kt2_oam_endpoint_get(unit, endpoint, endpoint_info);
            }
        } else
#endif
        {
            BCM_IF_ERROR_RETURN(bcm_esw_oam_lock(unit));
#if defined(BCM_ENDURO_SUPPORT)
            if (SOC_IS_ENDURO(unit) || SOC_IS_KATANAX(unit))
            {
                result = bcm_en_oam_endpoint_get(unit, endpoint, endpoint_info);
            } else
#endif
            {
                result = bcm_tr2x_oam_endpoint_get(unit, endpoint, endpoint_info);
            }
            BCM_IF_ERROR_RETURN(bcm_esw_oam_unlock(unit));
        }
    }
#endif

    return result;
}

/*
 * Function:
 *      bcm_esw_oam_endpoint_destroy
 * Purpose:
 *      Destroy an OAM endpoint object
 * Parameters:
 *      unit - (IN) Unit number.
 *      endpoint - (IN) The ID of the OAM endpoint object to destroy
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_oam_endpoint_destroy(
    int unit, 
    bcm_oam_endpoint_t endpoint)
{
    int result = BCM_E_UNAVAIL;

#if defined(INCLUDE_CCM) || defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_fp_based_oam) &&
        (soc_feature(unit, soc_feature_uc_ccm) ||
         soc_feature(unit, soc_feature_bhh)    ||
         soc_feature(unit, soc_feature_mpls_lm_dm)))
    {
        result = bcm_fp_oam_endpoint_destroy(unit, endpoint);
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#endif /* INCLUDE_CCM || INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT) || \
    defined(BCM_VALKYRIE2_SUPPORT)
    if (soc_feature(unit, soc_feature_oam))
    {
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
        if ((SOC_IS_TRIUMPH3(unit)) || SOC_IS_HURRICANE2(unit) ||
             SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
             SOC_IS_GREYHOUND2(unit)) {
            result = bcm_tr3_oam_endpoint_destroy(unit, endpoint);
        } else
#endif
/* KT2 OAM */
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                result = bcm_sb2_oam_endpoint_destroy(unit, endpoint);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
                result = bcm_kt2_oam_endpoint_destroy(unit, endpoint);
            }
        } else
#endif
        {
            BCM_IF_ERROR_RETURN(bcm_esw_oam_lock(unit));
#if defined(BCM_ENDURO_SUPPORT)
            if (SOC_IS_ENDURO(unit) || SOC_IS_KATANAX(unit))
            {
                result = bcm_en_oam_endpoint_destroy(unit, endpoint);
            } else
#endif
            {
                result = bcm_tr2x_oam_endpoint_destroy(unit, endpoint);
            }

            BCM_IF_ERROR_RETURN(bcm_esw_oam_unlock(unit));
        }
    }
#endif

    return result;
}

/*
 * Function:
 *      bcm_esw_oam_endpoint_destroy_all
 * Purpose:
 *      Destroy all OAM endpoint objects associated with a given OAM
 *      group
 * Parameters:
 *      unit - (IN) Unit number.
 *      group - (IN) The OAM group whose endpoints should be destroyed
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_oam_endpoint_destroy_all(
    int unit, 
    bcm_oam_group_t group)
{
    int result = BCM_E_UNAVAIL;

#if defined(INCLUDE_CCM) || defined(INCLUDE_BHH)
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_fp_based_oam) && 
        (soc_feature(unit, soc_feature_bhh)))
    {
        result = bcm_fp_oam_endpoint_destroy_all(unit, group);
    }
#endif
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT) || \
    defined(BCM_VALKYRIE2_SUPPORT)
    if (soc_feature(unit, soc_feature_oam))
    {
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
        if ((SOC_IS_TRIUMPH3(unit)) || SOC_IS_HURRICANE2(unit)||
             SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
             SOC_IS_GREYHOUND2(unit)) {
            result = bcm_tr3_oam_endpoint_destroy_all(unit, group);
        } else
#endif
/* KT2 OAM */
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                result = bcm_sb2_oam_endpoint_destroy_all(unit, group);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
                result = bcm_kt2_oam_endpoint_destroy_all(unit, group);
            }
        } else
#endif
        {
            BCM_IF_ERROR_RETURN(bcm_esw_oam_lock(unit));

#if defined(BCM_ENDURO_SUPPORT)
            if (SOC_IS_ENDURO(unit) || SOC_IS_KATANAX(unit))
            {
                result = bcm_en_oam_endpoint_destroy_all(unit, group);
            } else
#endif
            {
                result = bcm_tr2x_oam_endpoint_destroy_all(unit, group);
            }

            BCM_IF_ERROR_RETURN(bcm_esw_oam_unlock(unit));
        }
    }
#endif

    return result;
}

/*
 * Function:
 *      bcm_esw_oam_endpoint_traverse
 * Purpose:
 *      Traverse the set of OAM endpoints associated with the
 *      specified group, calling a specified callback for each one
 * Parameters:
 *      unit - (IN) Unit number.
 *      group - (IN) The OAM group whose endpoints should be traversed
 *      cb - (IN) A pointer to the callback function to call for each OAM endpoint in the specified group
 *      user_data - (IN) Pointer to user data to supply in the callback
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_oam_endpoint_traverse(
    int unit, 
    bcm_oam_group_t group, 
    bcm_oam_endpoint_traverse_cb cb, 
    void *user_data)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_CCM) || defined(INCLUDE_BHH)
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam) && (soc_feature(unit, soc_feature_bhh)))
        || (soc_feature(unit, soc_feature_uc_ccm)))
    {
        result = bcm_fp_oam_endpoint_traverse(unit, group, cb, user_data);
    }
#endif
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT) || \
    defined(BCM_VALKYRIE2_SUPPORT)
    if (soc_feature(unit, soc_feature_oam))
    {
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
        if ((SOC_IS_TRIUMPH3(unit)) || SOC_IS_HURRICANE2(unit) ||
             SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
             SOC_IS_GREYHOUND2(unit)) {
            result = bcm_tr3_oam_endpoint_traverse(unit, group, cb, user_data);
        } else
#endif
/* KT2 OAM */
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                result = bcm_sb2_oam_endpoint_traverse(unit, group, cb, user_data);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
                result = bcm_kt2_oam_endpoint_traverse(unit, group, cb, user_data);
            }
        } else
#endif
        {
            BCM_IF_ERROR_RETURN(bcm_esw_oam_lock(unit));

#if defined(BCM_ENDURO_SUPPORT)
            if (SOC_IS_ENDURO(unit) || SOC_IS_KATANAX(unit))
            {
                result = bcm_en_oam_endpoint_traverse(unit, group, cb, user_data);
            } else
#endif
            {
                result = bcm_tr2x_oam_endpoint_traverse(unit, group, cb, user_data);
            }

            BCM_IF_ERROR_RETURN(bcm_esw_oam_unlock(unit));
        }
    }
#endif

    return result;
}

/*
 * Function:
 *      bcm_esw_oam_event_register
 * Purpose:
 *      Register a callback for handling OAM events
 * Parameters:
 *      unit - (IN) Unit number.
 *      event_types - (IN) The set of OAM events for which the specified callback should be called
 *      cb - (IN) A pointer to the callback function to call for the specified OAM events
 *      user_data - (IN) Pointer to user data to supply in the callback
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_oam_event_register(
    int unit, 
    bcm_oam_event_types_t event_types, 
    bcm_oam_event_cb cb, 
    void *user_data)
{
    int result = BCM_E_UNAVAIL;

#if defined(INCLUDE_CCM) || defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_fp_based_oam) &&
        (soc_feature(unit, soc_feature_uc_ccm) ||
         soc_feature(unit, soc_feature_bhh)    ||
         soc_feature(unit, soc_feature_mpls_lm_dm)))
    {
        result = bcm_fp_oam_event_register(unit, event_types, cb, user_data);
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#endif /* INCLUDE_CCM || INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT) || \
    defined(BCM_VALKYRIE2_SUPPORT)
    if (soc_feature(unit, soc_feature_oam))
    {
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
        if ((SOC_IS_TRIUMPH3(unit)) || SOC_IS_HURRICANE2(unit)||
             SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
             SOC_IS_GREYHOUND2(unit)) {
            result = bcm_tr3_oam_event_register(unit, event_types, cb, user_data);
        } else
#endif
/* KT2 OAM */
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                result = bcm_sb2_oam_event_register(unit, event_types, cb, user_data);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
                result = bcm_kt2_oam_event_register(unit, event_types, cb, user_data);
            }
        } else
#endif
        {
            BCM_IF_ERROR_RETURN(bcm_esw_oam_lock(unit));

#if defined(BCM_ENDURO_SUPPORT)
            if (SOC_IS_ENDURO(unit) || SOC_IS_KATANAX(unit))
            {
                result = bcm_en_oam_event_register(unit, event_types, cb, user_data);
            } else
#endif
            {
                result = bcm_tr2x_oam_event_register(unit, event_types, cb, user_data);
            }

            BCM_IF_ERROR_RETURN(bcm_esw_oam_unlock(unit));
        }
    }
#endif

    return result;
}

/*
 * Function:
 *      bcm_esw_oam_event_unregister
 * Purpose:
 *      Unregister a callback for handling OAM events
 * Parameters:
 *      unit - (IN) Unit number.
 *      event_types - (IN) The set of OAM events for which the specified callback should not be called
 *      cb - (IN) A pointer to the callback function to unregister from the specified OAM events
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_oam_event_unregister(
    int unit, 
    bcm_oam_event_types_t event_types, 
    bcm_oam_event_cb cb)
{
    int result = BCM_E_UNAVAIL;

#if defined(INCLUDE_CCM) || defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_fp_based_oam) &&
        (soc_feature(unit, soc_feature_uc_ccm) ||
         soc_feature(unit, soc_feature_bhh)    ||
         soc_feature(unit, soc_feature_mpls_lm_dm)))
    {
        result = bcm_fp_oam_event_unregister(unit, event_types, cb);
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#endif /* INCLUDE_CCM || INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT) || \
    defined(BCM_VALKYRIE2_SUPPORT)
    if (soc_feature(unit, soc_feature_oam))
    {
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
        if ((SOC_IS_TRIUMPH3(unit)) || SOC_IS_HURRICANE2(unit) ||
             SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
             SOC_IS_GREYHOUND2(unit)) {
            result = bcm_tr3_oam_event_unregister(unit, event_types, cb);
        } else
#endif
/* KT2 OAM */
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                result = bcm_sb2_oam_event_unregister(unit, event_types, cb);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
                result = bcm_kt2_oam_event_unregister(unit, event_types, cb);
            }
        } else
#endif
        {
            BCM_IF_ERROR_RETURN(bcm_esw_oam_lock(unit));

#if defined(BCM_ENDURO_SUPPORT)
            if (SOC_IS_ENDURO(unit) || SOC_IS_KATANAX(unit))
            {
                result = bcm_en_oam_event_unregister(unit, event_types, cb);
            } else
#endif
            {
                result = bcm_tr2x_oam_event_unregister(unit, event_types, cb);
            }

            BCM_IF_ERROR_RETURN(bcm_esw_oam_unlock(unit));
        }
    }
#endif

    return result;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_esw_oam_sync
 * Purpose:
 *      Unregister a callback for handling OAM events
 * Parameters:
 *      unit - (IN) Unit number.
 *      event_types - (IN) The set of OAM events for which the specified callback should not be called
 *      cb - (IN) A pointer to the callback function to unregister from the specified OAM events
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_esw_oam_sync(int unit)
{
    int result = BCM_E_UNAVAIL;

#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_APACHE_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam)) &&
        (soc_feature(unit, soc_feature_bhh)         ||
         soc_feature(unit, soc_feature_mpls_lm_dm)))
    {
        result = _bcm_fp_oam_sync(unit);
    }
#endif /* BCM_APACHE_SUPPORT */
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT) || \
    defined(BCM_VALKYRIE2_SUPPORT)
    if (soc_feature(unit, soc_feature_oam))
    {

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
        if ((SOC_IS_TRIUMPH3(unit)) || SOC_IS_HURRICANE2(unit)
            || SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit)
            || SOC_IS_GREYHOUND2(unit)) {
            result = _bcm_tr3_oam_sync(unit);
        } else
#endif
/* KT2 OAM */
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                result = _bcm_sb2_oam_sync(unit);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
                result = _bcm_kt2_oam_sync(unit);
            }
        } else
#endif
        {
            BCM_IF_ERROR_RETURN(bcm_esw_oam_lock(unit));

#if defined(BCM_ENDURO_SUPPORT)
            if (SOC_IS_ENDURO(unit) || SOC_IS_KATANAX(unit))
            {
                result = _bcm_en_oam_sync(unit);
            }
            else
#endif
            {
                result = _bcm_tr2x_oam_sync(unit);
            }

            BCM_IF_ERROR_RETURN(bcm_esw_oam_unlock(unit));
        }
    }
#endif

    return result;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *     _bcm_oam_sw_dump
 * Purpose:
 *     Displays oam information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_oam_sw_dump(int unit)
{
#if defined(INCLUDE_CCM) || defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_fp_based_oam))
    {
        _bcm_fp_oam_sw_dump(unit);
    }
#endif
#endif /* INCLUDE_CCM || INCLUDE_BHH || INCLUDE_MPLS_LM_DM */
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT) || \
    defined(BCM_VALKYRIE2_SUPPORT)
    if (soc_feature(unit, soc_feature_oam))
    {
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
        if ((SOC_IS_TRIUMPH3(unit)) || SOC_IS_HURRICANE2(unit)||
            SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
            SOC_IS_GREYHOUND2(unit)) {
            _bcm_tr3_oam_sw_dump(unit);
        } else
#endif
/* KT2 OAM */
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                _bcm_sb2_oam_sw_dump(unit);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
                _bcm_kt2_oam_sw_dump(unit);
            }
        } else
#endif
#if defined(BCM_ENDURO_SUPPORT)
        if (SOC_IS_ENDURO(unit) || SOC_IS_KATANAX(unit))
        {
            _bcm_en_oam_sw_dump(unit);
        }
        else
#endif
        {
            _bcm_tr2x_oam_sw_dump(unit);
        }
    }
#endif
    return;
}
#endif


int
bcm_esw_oam_loopback_add(int unit, bcm_oam_loopback_t *loopback_ptr)
{
    int result = BCM_E_UNAVAIL;


#if defined(INCLUDE_BHH)
#if defined(BCM_APACHE_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam)) && 
        (soc_feature(unit, soc_feature_bhh)))
    {
        result = bcm_fp_oam_loopback_add(unit, loopback_ptr);
    }
#endif
    if ((soc_feature(unit, soc_feature_oam)) && (soc_feature(unit, soc_feature_bhh)))
    {

#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                return bcm_sb2_oam_loopback_add(unit, loopback_ptr);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
                return bcm_kt2_oam_loopback_add(unit, loopback_ptr);
            }
        }
#endif

#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            return bcm_tr3_oam_loopback_add(unit, loopback_ptr);
        }
#endif

#if defined(BCM_KATANA_SUPPORT)
        BCM_IF_ERROR_RETURN(bcm_esw_oam_lock(unit));

        if (SOC_IS_KATANAX(unit))
        {
            result = bcm_en_oam_loopback_add(unit, loopback_ptr);
        }

        BCM_IF_ERROR_RETURN(bcm_esw_oam_unlock(unit));
#endif 

    }
#endif

    return result;
}

int
bcm_esw_oam_loopback_get(int unit, bcm_oam_loopback_t *loopback_ptr)
{
    int result = BCM_E_UNAVAIL;

#if defined(INCLUDE_BHH)
#if defined(BCM_APACHE_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam)) && 
        (soc_feature(unit, soc_feature_bhh)))
    {
        result = bcm_fp_oam_loopback_get(unit, loopback_ptr);
    }
#endif
    if ((soc_feature(unit, soc_feature_oam)) && (soc_feature(unit, soc_feature_bhh)))
    {

#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                return bcm_sb2_oam_loopback_get(unit, loopback_ptr);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
                return bcm_kt2_oam_loopback_get(unit, loopback_ptr);
            }
        }
#endif 

#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            return bcm_tr3_oam_loopback_get(unit, loopback_ptr);
        }
#endif 

#if defined(BCM_KATANA_SUPPORT)
        BCM_IF_ERROR_RETURN(bcm_esw_oam_lock(unit));

        if (SOC_IS_KATANAX(unit))
        {
            result = bcm_en_oam_loopback_get(unit, loopback_ptr);
        }

        BCM_IF_ERROR_RETURN(bcm_esw_oam_unlock(unit));
#endif

    }
#endif 

    return result;
}

int
bcm_esw_oam_loopback_delete(int unit, bcm_oam_loopback_t *loopback_ptr)
{
    int result = BCM_E_UNAVAIL;

#if defined(INCLUDE_BHH)
#if defined(BCM_APACHE_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam)) && 
        (soc_feature(unit, soc_feature_bhh)))
    {
        result = bcm_fp_oam_loopback_delete(unit, loopback_ptr);
    }
#endif
    if ((soc_feature(unit, soc_feature_oam)) && (soc_feature(unit, soc_feature_bhh)))
    {

#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                return bcm_sb2_oam_loopback_delete(unit, loopback_ptr);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
                return bcm_kt2_oam_loopback_delete(unit, loopback_ptr);
            }
        }
#endif

#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            return bcm_tr3_oam_loopback_delete(unit, loopback_ptr);
        }
#endif

#if defined(BCM_KATANA_SUPPORT) 
        BCM_IF_ERROR_RETURN(bcm_esw_oam_lock(unit));

        if (SOC_IS_KATANAX(unit))
        {
            result = bcm_en_oam_loopback_delete(unit, loopback_ptr);
        }

        BCM_IF_ERROR_RETURN(bcm_esw_oam_unlock(unit));
#endif

    }
#endif

    return result;
}

int 
bcm_esw_oam_loss_add(
    int unit, 
    bcm_oam_loss_t *loss_ptr)
{
    int result = BCM_E_UNAVAIL;

#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_APACHE_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam)) &&
        (soc_feature(unit, soc_feature_bhh)         ||
         soc_feature(unit, soc_feature_mpls_lm_dm)))
    {
        result = bcm_fp_oam_loss_add(unit, loss_ptr);
    }
#endif /* BCM_APACHE_SUPPORT */
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined(INCLUDE_BHH) || defined(INCLUDE_ETH_LM_DM) ||                     \
    defined (INCLUDE_MPLS_LM_DM)
    if ((soc_feature(unit, soc_feature_oam)) && 
        (soc_feature(unit, soc_feature_bhh)||(soc_feature(unit, soc_feature_eth_lm_dm)))) 
    {

#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                return bcm_sb2_oam_loss_add(unit, loss_ptr);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
#if defined(INCLUDE_BHH)
                return bcm_kt2_oam_loss_add(unit, loss_ptr);
#endif                
            }
        }
#endif      /* KATANA2 */

#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_BHH)
        if (SOC_IS_TRIUMPH3(unit)) {
            return bcm_tr3_oam_loss_add(unit, loss_ptr);
        }
#endif      /* TRIUMPH3 */

    }
#endif      /* BHH */

    return result;
}

int 
bcm_esw_oam_loss_delete(
    int unit, 
    bcm_oam_loss_t *loss_ptr)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_APACHE_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam)) &&
        (soc_feature(unit, soc_feature_bhh)         ||
         soc_feature(unit, soc_feature_mpls_lm_dm)))
    {
        result = bcm_fp_oam_loss_delete(unit, loss_ptr);
    }
#endif /* BCM_APACHE_SUPPORT */
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined(INCLUDE_BHH) || defined(INCLUDE_ETH_LM_DM) ||                     \
    defined (INCLUDE_MPLS_LM_DM)
    if ((soc_feature(unit, soc_feature_oam)) && 
        (soc_feature(unit, soc_feature_bhh)||(soc_feature(unit, soc_feature_eth_lm_dm)))) 
    {

#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                return bcm_sb2_oam_loss_delete(unit, loss_ptr);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
#if defined(INCLUDE_BHH)            
                return bcm_kt2_oam_loss_delete(unit, loss_ptr);
#endif                
            }
        } 
#endif      /* KATANA2 */

#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_BHH)
        if (SOC_IS_TRIUMPH3(unit)) {
            return bcm_tr3_oam_loss_delete(unit, loss_ptr);
        } 
#endif      /* TRIUMPH3 */


    }
#endif

    return result;
}

int 
bcm_esw_oam_loss_get(
    int unit, 
    bcm_oam_loss_t *loss_ptr)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_APACHE_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam)) &&
        (soc_feature(unit, soc_feature_bhh)         ||
         soc_feature(unit, soc_feature_mpls_lm_dm)))
    {
        result = bcm_fp_oam_loss_get(unit, loss_ptr);
    }
#endif /* BCM_APACHE_SUPPORT */
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined(INCLUDE_BHH) || defined(INCLUDE_ETH_LM_DM) ||                     \
    defined (INCLUDE_MPLS_LM_DM)
    if ((soc_feature(unit, soc_feature_oam)) && 
        (soc_feature(unit, soc_feature_bhh)||(soc_feature(unit, soc_feature_eth_lm_dm)))) 
    {

#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                return bcm_sb2_oam_loss_get(unit, loss_ptr);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
#if defined(INCLUDE_BHH)            
                return bcm_kt2_oam_loss_get(unit, loss_ptr);
#endif                
            }
        }
#endif      /* KATANA2 */

#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_BHH)
        if (SOC_IS_TRIUMPH3(unit)) {
            return bcm_tr3_oam_loss_get(unit, loss_ptr);
        }
#endif      /* TRIUMPH3 */

    }
#endif      /* BHH */

    return result;
}

int 
bcm_esw_oam_delay_add(
    int unit, 
    bcm_oam_delay_t *delay_ptr)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_APACHE_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam)) &&
        (soc_feature(unit, soc_feature_bhh)         ||
         soc_feature(unit, soc_feature_mpls_lm_dm)))
    {
        result = bcm_fp_oam_delay_add(unit, delay_ptr);
    }
#endif /* BCM_APACHE_SUPPORT */
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined(INCLUDE_BHH) || defined(INCLUDE_ETH_LM_DM) ||                     \
    defined (INCLUDE_MPLS_LM_DM)
    if ((soc_feature(unit, soc_feature_oam)) && 
        (soc_feature(unit, soc_feature_bhh)||(soc_feature(unit, soc_feature_eth_lm_dm)))) 
    {

#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                return bcm_sb2_oam_delay_add(unit, delay_ptr);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
#if defined(INCLUDE_BHH)            
                return bcm_kt2_oam_delay_add(unit, delay_ptr);
#endif                
            }
        }
#endif      /* KATANA2 */

#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_BHH)
        if (SOC_IS_TRIUMPH3(unit)) {
            return bcm_tr3_oam_delay_add(unit, delay_ptr);
        }
#endif      /* TRIUMPH3 */

    }
#endif      /* BHH */

    return result;
}

int 
bcm_esw_oam_delay_delete(
    int unit, 
    bcm_oam_delay_t *delay_ptr)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_APACHE_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam)) &&
        (soc_feature(unit, soc_feature_bhh)         ||
         soc_feature(unit, soc_feature_mpls_lm_dm)))
    {
        result = bcm_fp_oam_delay_delete(unit, delay_ptr);
    }
#endif /* BCM_APACHE_SUPPORT */
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined(INCLUDE_BHH) || defined(INCLUDE_ETH_LM_DM) ||                     \
    defined (INCLUDE_MPLS_LM_DM)
    if ((soc_feature(unit, soc_feature_oam)) && 
        (soc_feature(unit, soc_feature_bhh)||(soc_feature(unit, soc_feature_eth_lm_dm)))) 
    {

#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                return bcm_sb2_oam_delay_delete(unit, delay_ptr);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
#if defined(INCLUDE_BHH)            
                return bcm_kt2_oam_delay_delete(unit, delay_ptr);
#endif
            }
        } 
#endif /* KATANA2 */

#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_BHH)
        if (SOC_IS_TRIUMPH3(unit)) {
            return bcm_tr3_oam_delay_delete(unit, delay_ptr);
        } 
#endif /*TRIUMPH3 */

    }
#endif /* BHH */

    return result;
}

int 
bcm_esw_oam_delay_get(
    int unit, 
    bcm_oam_delay_t *delay_ptr)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_APACHE_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam)) &&
        (soc_feature(unit, soc_feature_bhh)         ||
         soc_feature(unit, soc_feature_mpls_lm_dm)))
    {
        result = bcm_fp_oam_delay_get(unit, delay_ptr);
    }
#endif /* BCM_APACHE_SUPPORT */
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined(INCLUDE_BHH) || defined(INCLUDE_ETH_LM_DM) ||                     \
    defined (INCLUDE_MPLS_LM_DM)
    if ((soc_feature(unit, soc_feature_oam)) && 
        (soc_feature(unit, soc_feature_bhh)||(soc_feature(unit, soc_feature_eth_lm_dm)))) 
    {

#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                return bcm_sb2_oam_delay_get(unit, delay_ptr);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
#if defined(INCLUDE_BHH)            
                return bcm_kt2_oam_delay_get(unit, delay_ptr);
#endif
            }
        } 
#endif /* KATANA2 */

#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_BHH)
        if (SOC_IS_TRIUMPH3(unit)) {
            return bcm_tr3_oam_delay_get(unit, delay_ptr);
        } 
#endif /* TRIUMPH3 */

    }
#endif /* BHH */

    return result;
}

int 
bcm_esw_oam_endpoint_action_set(
    int unit,
    bcm_oam_endpoint_t id,
    bcm_oam_endpoint_action_t *action) 
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_KATANA2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) &&  (SOC_IS_KATANA2(unit))) { 
        if (SOC_IS_SABER2(unit)){
#if defined (BCM_SABER2_SUPPORT)
            result = bcm_sb2_oam_endpoint_action_set(unit, id, action);
#endif
        } else if (SOC_IS_KATANA2(unit)) {
            result = bcm_kt2_oam_endpoint_action_set(unit, id, action); 
        }
    } 
#endif /* BCM_KATANA2_SUPPORT */
    return result;
}

int 
bcm_esw_oam_control_get(
    int unit,
    bcm_oam_control_type_t type,
    uint64            *arg) 
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_KATANA2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) &&  (SOC_IS_KATANA2(unit))) { 
        if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
            result = bcm_sb2_oam_control_get(unit, type, arg); 
#endif
        } else if (SOC_IS_KATANA2(unit)) {
            result = bcm_kt2_oam_control_get(unit, type, arg); 
        }
    } 
#endif /* BCM_KATANA2_SUPPORT */
    return result;
}

int 
bcm_esw_oam_control_set(
    int unit,
    bcm_oam_control_type_t type,
    uint64            arg) 
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_KATANA2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) &&  (SOC_IS_KATANA2(unit))) { 
        if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
            result = bcm_sb2_oam_control_set(unit, type, arg); 
#endif
        } else if (SOC_IS_KATANA2(unit)) {
            result = bcm_kt2_oam_control_set(unit, type, arg); 
        }
    } 
#endif /* BCM_KATANA2_SUPPORT */
    return result;
}

int
bcm_esw_oam_lookup_enable_set(
        int unit, 
        bcm_oam_lookup_type_t type, 
        bcm_oam_condition_t condition,
        int enable)
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_SABER2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) &&  (SOC_IS_SABER2(unit))) {
        result = bcm_sb2_oam_lookup_enable_set(unit, type, condition,
                enable);
    }
#endif /* BCM_SABER2_SUPPORT */
    return result;
}

int
bcm_esw_oam_lookup_enable_multi_set(
        int unit, 
        bcm_oam_lookup_types_t type, 
        bcm_oam_conditions_t condition,
        int enable)
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_SABER2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) &&  (SOC_IS_SABER2(unit))) {
        result = bcm_sb2_oam_lookup_enable_multi_set(unit, type, 
                condition, enable);
    }
#endif /* BCM_SABER2_SUPPORT */
    return result;
}

int
bcm_esw_oam_lookup_enable_get(
        int unit, 
        bcm_oam_lookup_type_t type, 
        bcm_oam_conditions_t *condition)
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_SABER2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) &&  (SOC_IS_SABER2(unit))) {
            result = bcm_sb2_oam_lookup_enable_get(unit, type, condition);
    }
#endif /* BCM_SABER2_SUPPORT */
    return result;
}

int
bcm_esw_oam_pm_profile_create(
        int unit,
        bcm_oam_pm_profile_info_t *profile_info)
{
    int result = BCM_E_UNAVAIL;

#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_APACHE_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam)) &&
        (soc_feature(unit, soc_feature_oam_pm)))
    {
        result = bcm_fp_oam_pm_profile_create(unit, profile_info);
    }
#endif /* BCM_APACHE_SUPPORT */
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined(BCM_KATANA2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) 
            && (soc_feature(unit, soc_feature_oam_pm)) ) {
        if (SOC_IS_SABER2(unit)) {
#if defined(BCM_SABER2_SUPPORT)
            result = bcm_sb2_oam_pm_profile_create(unit, profile_info);
#endif /* BCM_SABER2_SUPPORT */
        } else {  
#if defined(INCLUDE_BHH)
            result = bcm_kt2_oam_pm_profile_create(unit, profile_info);
#endif
        }
    }
#endif

    return result;
}

int
bcm_esw_oam_pm_profile_delete(
        int unit,
        bcm_oam_pm_profile_t profile_id)
{
    int result = BCM_E_UNAVAIL;

#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_APACHE_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam)) &&
        (soc_feature(unit, soc_feature_oam_pm)))
    {
        result = bcm_fp_oam_pm_profile_delete(unit, profile_id);
    }
#endif /* BCM_APACHE_SUPPORT */
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined(BCM_KATANA2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) 
            && (soc_feature(unit, soc_feature_oam_pm))) {
        if (SOC_IS_SABER2(unit)) {
#if defined(BCM_SABER2_SUPPORT)
            result = bcm_sb2_oam_pm_profile_delete(unit, profile_id);
#endif /* BCM_SABER2_SUPPORT */
        } else {
#if defined(INCLUDE_BHH)
            result = bcm_kt2_oam_pm_profile_delete(unit, profile_id);
#endif
        }
    }
#endif

    return result;
}

int
bcm_esw_oam_pm_profile_delete_all(
        int unit)
{
    int result = BCM_E_UNAVAIL;

#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_APACHE_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam)) &&
        (soc_feature(unit, soc_feature_oam_pm)))
    {
        result = bcm_fp_oam_pm_profile_delete_all(unit);
    }
#endif /* BCM_APACHE_SUPPORT */
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined(BCM_KATANA2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) 
            && (soc_feature(unit, soc_feature_oam_pm))) {
        if (SOC_IS_SABER2(unit)) {
#if defined(BCM_SABER2_SUPPORT)
            result = bcm_sb2_oam_pm_profile_delete_all(unit);
#endif /* BCM_SABER2_SUPPORT */
        } else {
#if defined(INCLUDE_BHH)
            result = bcm_kt2_oam_pm_profile_delete_all(unit);
#endif
        }
    }
#endif /* BCM_SABER2_SUPPORT */
    return result;
}

int
bcm_esw_oam_pm_profile_detach(
        int unit,
        bcm_oam_endpoint_t endpoint_id,
        int profile_id)
{
    int result = BCM_E_UNAVAIL;

#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_APACHE_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam)) &&
        (soc_feature(unit, soc_feature_oam_pm)))
    {
        result = bcm_fp_oam_pm_profile_detach(unit, endpoint_id,
                                                   profile_id);
    }
#endif /* BCM_APACHE_SUPPORT */
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined(BCM_KATANA2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) 
            && (soc_feature(unit, soc_feature_oam_pm))){
        if (SOC_IS_SABER2(unit)) {
#if defined(BCM_SABER2_SUPPORT)
            result = bcm_sb2_oam_pm_profile_detach(unit, endpoint_id, 
                                                   profile_id);
#endif  
        } else  {
#if defined(INCLUDE_BHH)
            result = bcm_kt2_oam_pm_profile_detach(unit, endpoint_id, 
                    profile_id);
#endif /* INCLUDE_BHH */
    }
    }
#endif /* BCM_KATANA2_SUPPORT */
    return result;
}

int
bcm_esw_oam_pm_profile_get(
    int unit,
    bcm_oam_pm_profile_info_t *profile_info)
{
    int result = BCM_E_UNAVAIL;

#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_APACHE_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam)) &&
        (soc_feature(unit, soc_feature_oam_pm))) {
        result = bcm_fp_oam_pm_profile_get(unit, profile_info);
    }
#endif /* BCM_APACHE_SUPPORT */
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined(BCM_KATANA2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) 
            && (soc_feature(unit, soc_feature_oam_pm))){
        if (SOC_IS_SABER2(unit)) {
#if defined(BCM_SABER2_SUPPORT)
            result = bcm_sb2_oam_pm_profile_get(unit, profile_info);
#endif    
        } else {
#if defined(INCLUDE_BHH)
            result = bcm_kt2_oam_pm_profile_get(unit, profile_info);
#endif /* INCLUDE_BHH */
    }
    }
#endif /* BCM_KATANA2_SUPPORT */
    return result;
}

int
bcm_esw_oam_pm_profile_traverse(
    int unit,
    bcm_oam_pm_profile_traverse_cb cb,
    void *user_data)
{
    int result = BCM_E_UNAVAIL;

#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_APACHE_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam)) &&
        (soc_feature(unit, soc_feature_oam_pm)))
    {
        result = bcm_fp_oam_pm_profile_traverse(unit, cb, user_data);
    }
#endif /* BCM_APACHE_SUPPORT */
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined(BCM_KATANA2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) 
            && (soc_feature(unit, soc_feature_oam_pm))){
        if (SOC_IS_SABER2(unit)) {
#if defined(BCM_SABER2_SUPPORT)
            result = bcm_sb2_oam_pm_profile_traverse(unit, cb, user_data);
#endif    
        } else {
#if defined(INCLUDE_BHH)
            result = bcm_kt2_oam_pm_profile_traverse(unit, cb, user_data);
#endif /* INCLUDE_BHH */
    }
    }
#endif /* BCM_KATANA2_SUPPORT */
    return result;
}

int
bcm_esw_oam_pm_stats_get(
    int unit,
    bcm_oam_endpoint_t endpoint_id,
    bcm_oam_pm_stats_t *stats_ptr)
{
    int result = BCM_E_UNAVAIL;

#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_APACHE_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam)) &&
        (soc_feature(unit, soc_feature_oam_pm)))
    {
        result = bcm_fp_oam_pm_stats_get(unit, endpoint_id, stats_ptr);
    }
#endif /* BCM_APACHE_SUPPORT */
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined(BCM_KATANA2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) 
            && (soc_feature(unit, soc_feature_oam_pm))) {
        if(SOC_IS_SABER2(unit)) {
#if defined(BCM_SABER2_SUPPORT)
            result = bcm_sb2_oam_pm_stats_get(unit, endpoint_id, stats_ptr);
#endif
        } else {
#if defined(INCLUDE_BHH)
            result = bcm_kt2_oam_pm_stats_get(unit, endpoint_id, stats_ptr);
#endif /* INCLUDE_BHH */
    }
    }
#endif /* BCM_KATANA2_SUPPORT */
    return result;
}

int
bcm_esw_oam_pm_event_register(
    int unit,
    bcm_oam_event_types_t event_types,
    bcm_oam_pm_event_cb cb,
    void *user_data)
{
    int result = BCM_E_UNAVAIL;

#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_APACHE_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam)) &&
        (soc_feature(unit, soc_feature_oam_pm)))
    {
        result = bcm_fp_oam_pm_event_register(unit, event_types, cb,
                user_data);
    }
#endif /* BCM_APACHE_SUPPORT */
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined(BCM_SABER2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) 
        && (soc_feature(unit, soc_feature_oam_pm))
        &&  (SOC_IS_SABER2(unit))) {
        result = bcm_sb2_oam_pm_event_register(unit, event_types, cb, 
                user_data);
    }
#endif /* BCM_SABER2_SUPPORT */
    return result;
}

int
bcm_esw_oam_pm_event_unregister(
    int unit,
    bcm_oam_event_types_t event_types,
    bcm_oam_pm_event_cb cb)
{
    int result = BCM_E_UNAVAIL;

#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_APACHE_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam)) &&
        (soc_feature(unit, soc_feature_oam_pm)))
    {
        result = bcm_fp_oam_pm_event_unregister(unit, event_types, cb);
    }
#endif /* BCM_APACHE_SUPPORT */
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined(BCM_SABER2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) 
        && (soc_feature(unit, soc_feature_oam_pm))
        &&  (SOC_IS_SABER2(unit))) {
        result = bcm_sb2_oam_pm_event_unregister(unit, event_types, cb);
    }
#endif /* BCM_SABER2_SUPPORT */
    return result;
}

int
bcm_esw_oam_pm_profile_attach_get(
        int unit, 
        bcm_oam_endpoint_t endpoint_id,
        bcm_oam_pm_profile_t *profile_id)
{
    int result = BCM_E_UNAVAIL;

#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_APACHE_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam)) &&
        (soc_feature(unit, soc_feature_oam_pm)))
    {
        result = bcm_fp_oam_pm_profile_attach_get(unit, endpoint_id,
                                                  profile_id);
    }
#endif /* BCM_APACHE_SUPPORT */
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined(BCM_KATANA2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) 
            && (soc_feature(unit, soc_feature_oam_pm))){
        if (SOC_IS_SABER2(unit)) {
#if defined(BCM_SABER2_SUPPORT)
        result = bcm_sb2_oam_pm_profile_attach_get(unit, endpoint_id, profile_id);
#endif
        } else {
#if defined(INCLUDE_BHH)
            result = bcm_kt2_oam_pm_profile_attach_get(unit, endpoint_id, profile_id);
#endif /* INCLUDE_BHH */
        }
    }
#endif /* BCM_SABER2_SUPPORT */
    return result;
}

int
bcm_esw_oam_pm_profile_attach(
        int unit, 
        bcm_oam_endpoint_t endpoint_id, 
        bcm_oam_pm_profile_t profile_id)
{
    int result = BCM_E_UNAVAIL;

#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_APACHE_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam)) &&
        (soc_feature(unit, soc_feature_oam_pm)))
    {
        result = bcm_fp_oam_pm_profile_attach(unit, endpoint_id, profile_id);
    }
#endif /* BCM_APACHE_SUPPORT */
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined(BCM_KATANA2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) 
            && (soc_feature(unit, soc_feature_oam_pm))) {
        if (SOC_IS_SABER2(unit)) {
#if defined(BCM_SABER2_SUPPORT)
        result = bcm_sb2_oam_pm_profile_attach(unit, endpoint_id, profile_id);
        } else {
#endif    
#if defined(INCLUDE_BHH)
            result = bcm_kt2_oam_pm_profile_attach(unit, endpoint_id, profile_id);
#endif /* INCLUDE_BHH */
        }
    }
#endif /* BCM_SABER2_SUPPORT */
    return result;
}

int
bcm_esw_oam_pm_raw_data_read_done(
        int unit,
        bcm_oam_event_types_t event_types,
        uint32 read_index)
{
    int result = BCM_E_UNAVAIL;

#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_APACHE_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam)) &&
        (soc_feature(unit, soc_feature_oam_pm)))
    {
        result = bcm_fp_oam_pm_raw_data_read_done(unit, event_types, 
                                                  read_index);
    }
#endif /* BCM_APACHE_SUPPORT */
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined(BCM_SABER2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) 
        && (soc_feature(unit, soc_feature_oam_pm))
        &&  (SOC_IS_SABER2(unit))) {
        result = bcm_sb2_oam_pm_raw_data_read_done(unit, event_types, read_index);
    }
#endif /* BCM_SABER2_SUPPORT */
    return result;
}

int
_bcm_oam_olp_fp_hw_index_get(int unit,
                             bcm_field_olp_header_type_t olp_hdr_type,
                             int *hwindex)
{
    if (soc_feature(unit, soc_feature_oam)) {
        if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
            return _bcm_sb2_oam_olp_fp_hw_index_get(unit, olp_hdr_type, hwindex);
#endif
        } else if (SOC_IS_KATANA2(unit)) {
#if defined (BCM_KATANA2_SUPPORT)
            return _bcm_kt2_oam_olp_fp_hw_index_get(unit, olp_hdr_type, hwindex);
#endif
        }
    } else if(soc_feature(unit, soc_feature_fp_based_oam)) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
       return _bcm_td2p_oam_olp_fp_hw_index_get(unit, olp_hdr_type, hwindex);
#endif
    }

    return BCM_E_UNAVAIL;
}

int
_bcm_oam_olp_hw_index_olp_type_get(int unit,
                                   int hwindex,
                                   bcm_field_olp_header_type_t *olp_hdr_type)
{
    if (soc_feature(unit, soc_feature_oam)) {
        if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
            return _bcm_sb2_oam_olp_hw_index_olp_type_get(
                                                   unit, hwindex, olp_hdr_type);
#endif
        } else if (SOC_IS_KATANA2(unit)) {
#if defined (BCM_KATANA2_SUPPORT)
            return _bcm_kt2_oam_olp_hw_index_olp_type_get(
                                                    unit, hwindex, olp_hdr_type);
#endif
        }
    } else if(soc_feature(unit, soc_feature_fp_based_oam)) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
       return _bcm_td2p_oam_olp_hw_index_olp_type_get(
                                                    unit, hwindex, olp_hdr_type);
#endif
    }

    return BCM_E_UNAVAIL;
}

int
bcm_esw_oam_opcode_group_set(
        int unit, 
        bcm_oam_protocol_type_t  proto,
        bcm_oam_opcodes_t opcodes, 
        uint8 opcode_group)
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_SABER2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) 
            &&  (SOC_IS_SABER2(unit))) {
        result = bcm_sb2_oam_opcode_group_set(unit, proto, opcodes, opcode_group);
    }    
#endif /* BCM_SABER2_SUPPORT */
    return result;
}

int
bcm_esw_oam_opcode_group_get(
        int unit, 
        bcm_oam_protocol_type_t  proto,
        bcm_oam_opcode_t opcode, 
        uint8 *opcode_group)
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_SABER2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) 
            &&  (SOC_IS_SABER2(unit))) {
        result = bcm_sb2_oam_opcode_group_get(unit, proto, opcode, opcode_group);
    }    
#endif /* BCM_SABER2_SUPPORT */
    return result;
}
int
bcm_esw_oam_endpoint_gport_egress_attach(int unit,
                                         bcm_oam_endpoint_t endpoint,
                                         bcm_gport_t gport)
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_SABER2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) 
            &&  (SOC_IS_SABER2(unit))) {
        result = bcm_sb2_oam_endpoint_gport_egress_attach(unit, endpoint,
                                                          gport);
    }    
#endif /* BCM_SABER2_SUPPORT */
    return result;
}

int
bcm_esw_oam_endpoint_gport_egress_detach(int unit,
                                         bcm_oam_endpoint_t endpoint,
                                         bcm_gport_t gport)
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_SABER2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) 
            &&  (SOC_IS_SABER2(unit))) {
        result = bcm_sb2_oam_endpoint_gport_egress_detach(unit, endpoint,
                                                          gport);
    }    
#endif /* BCM_SABER2_SUPPORT */
    return result;
}

int
bcm_esw_oam_endpoint_gport_egress_attach_get(int unit,
                                         bcm_oam_endpoint_t endpoint,
                                         int size,
                                         bcm_gport_t *gport,
                                         int *num_gports)
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_SABER2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) 
            &&  (SOC_IS_SABER2(unit))) {
        result = bcm_sb2_oam_endpoint_gport_egress_attach_get(unit,
                                                        endpoint, 
                                                        size, 
                                                        gport, 
                                                        num_gports);
    }    
#endif /* BCM_SABER2_SUPPORT */
    return result;
}

int
bcm_esw_oam_endpoint_egress_intf_egress_attach(int unit,
                                         bcm_oam_endpoint_t endpoint_id,
                                         bcm_if_t egress_intf)
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_SABER2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) 
            &&  (SOC_IS_SABER2(unit))) {
        
        result = bcm_sb2_oam_endpoint_egress_intf_egress_attach(unit,
                endpoint_id, 
                egress_intf);
    }    
#endif /* BCM_SABER2_SUPPORT */
    return result;
}

int
bcm_esw_oam_endpoint_egress_intf_egress_detach(int unit,
                                         bcm_oam_endpoint_t endpoint_id,
                                         bcm_if_t egress_intf)
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_SABER2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) 
            &&  (SOC_IS_SABER2(unit))) {

        result = bcm_sb2_oam_endpoint_egress_intf_egress_detach(unit,
                    endpoint_id,
                    egress_intf);
    }    
#endif /* BCM_SABER2_SUPPORT */
    return result;
}

int
bcm_esw_oam_endpoint_egress_intf_egress_attach_get(int unit,
                                         bcm_oam_endpoint_t endpoint_id,
                                         int max_count,
                                         bcm_if_t *egress_intf,
                                         int *count)
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_SABER2_SUPPORT)
    if ((soc_feature(unit, soc_feature_oam)) 
            &&  (SOC_IS_SABER2(unit))) {

        result = bcm_sb2_oam_endpoint_egress_intf_egress_attach_get(unit,
                    endpoint_id,
                    max_count,
                    egress_intf,
                    count);
    }    
#endif /* BCM_SABER2_SUPPORT */
    return result;
}

/*
 * Function:
 *      _bcm_oam_drop_control_fp_encode
 * Purpose:
 *      Convert a Drop Control mep_type to FP data
 *      and mask
 * Parameters:
 *      unit     - (IN)  Unit number.
 *      mep_type - (IN)  Mep type
 *      data     - (OUT) Data for FP qualifier
 *      mask     - (OUT) Mask for FP qualifier
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_oam_drop_control_fp_encode (int unit,
                                 bcm_field_oam_drop_mep_type_t mep_type,
                                 uint8 *data,
                                 uint8 *mask)
{
#if defined(BCM_APACHE_SUPPORT)
    if (soc_feature(unit, soc_feature_fp_oam_drop_control)) {
        return _bcm_apache_oam_drop_control_fp_encode(mep_type, data, mask);
    }
#endif
    return BCM_E_UNAVAIL;
}
 
/*
 *      bcm_esw_oam_endpoint_faults_multi_get
 * Purpose:
 *      Function to get faults for multiple endpoints per protocl
 *      in a single call.
 * Parameters:
 *      unit                (IN) BCM device number
 *      flags               (IN) Flags is kept unused as of now. 
 *                               This is for any future enhancement
 *      endpoint_protocol   (IN) Protocol type of the endpoints to retrieve faults
 *      max_endpoints       (IN) Number of max endpoint for the protocol
 *      faults              (OUT) Pointer to faults for all endpoints
 *      endpoint_count      (OUT) Pointer to Number of valid endpoints with faults, 
 *                                by get API. 
 *
 * Returns:
 *      BCM_E_NONE          No error
 *      BCM_E_XXXX          Error
 */


int 
bcm_esw_oam_endpoint_faults_multi_get(
        int unit,
        uint32 flags,
        bcm_oam_protocol_type_t endpoint_protocol,
        uint32 max_endpoints,
        bcm_oam_endpoint_fault_t *faults,
        uint32 *endpoint_count)
{
    int result = BCM_E_UNAVAIL;

#if (defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)) && defined(INCLUDE_BHH) \
    && defined(INCLUDE_OAM_FAULTS_MULTI_GET)

    if (soc_feature(unit, soc_feature_oam) &&
        soc_feature(unit, soc_feature_faults_multi_ep_get)) {
        if (SOC_IS_SABER2(unit)) {
            result = bcm_sb2_oam_endpoint_faults_multi_get(
                         unit,
                         flags,
                         endpoint_protocol,
                         max_endpoints,
                         faults,
                         endpoint_count);
        } else if (SOC_IS_KATANA2(unit)) {
            result = bcm_kt2_oam_endpoint_faults_multi_get(
                         unit,
                        flags,
                        endpoint_protocol,
                        max_endpoints,
                       faults,
                       endpoint_count);
        } else if (SOC_IS_TRIUMPH3(unit)) {
            result = bcm_tr3_oam_endpoint_faults_multi_get(
                         unit,
                        flags,
                        endpoint_protocol,
                        max_endpoints,
                        faults,
                        endpoint_count);

        } else if (SOC_IS_KATANA(unit)) {
            result = bcm_en_oam_endpoint_faults_multi_get(
                         unit,
                         flags,
                         endpoint_protocol,
                         max_endpoints,
                         faults,
                         endpoint_count);
        }
    }
#endif /* BCM_KATANA_SUPPORT */

    return result; 
}

int 
bcm_esw_oam_opcodes_count_profile_create(
        int unit, 
        uint8 *lm_count_profile)
{
    int result = BCM_E_UNAVAIL; 
#if defined(BCM_METROLITE_SUPPORT)
        if(SOC_IS_METROLITE(unit)) {
        result = bcm_ml_oam_opcodes_count_profile_create(
            unit,
            lm_count_profile);
        }
#endif
    return result;
}

int 
bcm_esw_oam_opcodes_count_profile_delete(
        int unit, 
        uint8 lm_count_profile)
{
    int result = BCM_E_UNAVAIL; 
#if defined(BCM_METROLITE_SUPPORT)
    if(SOC_IS_METROLITE(unit)) {
    result = bcm_ml_oam_opcodes_count_profile_delete(
        unit, 
        lm_count_profile); 
    }
#endif
    return result;
}

int 
bcm_esw_oam_opcodes_count_profile_get(
        int unit, 
        uint8 lm_count_profile, 
        uint8 count_enable, 
        bcm_oam_opcodes_t *opcodes_bitmap)
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_METROLITE_SUPPORT)
    if(SOC_IS_METROLITE(unit)) {
    result = bcm_ml_oam_opcodes_count_profile_get(
        unit, 
        lm_count_profile, 
        count_enable, 
        opcodes_bitmap); 
    }
#endif  
    return result;
}

int 
bcm_esw_oam_opcodes_count_profile_set(
        int unit, 
        uint8 lm_count_profile, 
        uint8 count_enable, 
        bcm_oam_opcodes_t *opcodes_bitmap)
{
    int result = BCM_E_UNAVAIL; 
#if defined(BCM_METROLITE_SUPPORT)
    if(SOC_IS_METROLITE(unit)) {
    result = bcm_ml_oam_opcodes_count_profile_set(
        unit, 
        lm_count_profile, 
        count_enable, 
        opcodes_bitmap); 
    }
#endif
    return result;
}

int 
bcm_esw_oam_trunk_ports_add(
        int unit, 
        bcm_gport_t trunk_gport, 
        int max_ports, 
        bcm_gport_t *port_arr)
{
    int result = BCM_E_UNAVAIL; 
#if defined (INCLUDE_CCM) || defined (INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if((SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_MONTEREY(unit)) &&
       soc_feature(unit, soc_feature_fp_based_oam)) {
    result = bcm_fp_oam_trunk_ports_add(
             unit,
             trunk_gport,
             max_ports,
             port_arr);
    }
#endif
#endif

#if defined(BCM_KATANA2_SUPPORT)
        if(SOC_IS_SABER2(unit)){
#if defined (BCM_SABER2_SUPPORT)
            result = bcm_sb2_oam_trunk_ports_add(unit, trunk_gport, max_ports, port_arr);
#endif
        }
        else if (SOC_IS_KATANA2(unit)) {
            result = bcm_kt2_oam_trunk_ports_add(unit, trunk_gport, max_ports, port_arr);
        }
#endif

#if defined (INCLUDE_BHH)
    if (SOC_IS_KATANA(unit)) {
#if defined(BCM_KATANA_SUPPORT)
        result = bcm_en_oam_trunk_ports_add(unit, trunk_gport,
                                            max_ports, port_arr);
#endif /* BCM_KATANA_SUPPORT */
    } else if (SOC_IS_TRIUMPH3(unit)) {
#if defined(BCM_TRIUMPH3_SUPPORT)
        result = bcm_tr3_oam_trunk_ports_add(unit, trunk_gport,
                                             max_ports, port_arr);
#endif /* BCM_TRIUMPH3_SUPPORT */
    }
#endif /* INCLUDE_BHH */
    return result;
}

int 
bcm_esw_oam_trunk_ports_delete(
        int unit, 
        bcm_gport_t trunk_gport, 
        int max_ports, 
        bcm_gport_t *port_arr)
{
    int result = BCM_E_UNAVAIL; 
#if defined (INCLUDE_CCM) || defined (INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if(SOC_IS_TRIDENT2PLUS(unit) &&
       soc_feature(unit, soc_feature_fp_based_oam)) {
    result = bcm_fp_oam_trunk_ports_delete(
             unit,
             trunk_gport,
             max_ports,
             port_arr);
    }
#endif
#endif

#if defined(BCM_KATANA2_SUPPORT)
        if(SOC_IS_SABER2(unit)){
#if defined (BCM_SABER2_SUPPORT)
            result = bcm_sb2_oam_trunk_ports_delete(unit, trunk_gport, max_ports, port_arr);
#endif
        }
        else if (SOC_IS_KATANA2(unit)) {
            result = bcm_kt2_oam_trunk_ports_delete(unit, trunk_gport, max_ports, port_arr);
        }
#endif

#if defined (INCLUDE_BHH)
    if (SOC_IS_KATANA(unit)) {
#if defined(BCM_KATANA_SUPPORT)
        result = bcm_en_oam_trunk_ports_delete(unit, trunk_gport,
                                               max_ports, port_arr);
#endif /* BCM_KATANA_SUPPORT */
    } else if (SOC_IS_TRIUMPH3(unit)) {
#if defined(BCM_TRIUMPH3_SUPPORT)
        result = bcm_tr3_oam_trunk_ports_delete(unit, trunk_gport,
                                                max_ports, port_arr);
#endif /* BCM_TRIUMPH3_SUPPORT */
    }
#endif /* INCLUDE_BHH */
    return result;
}

int 
bcm_esw_oam_trunk_ports_get(
        int unit, 
        bcm_gport_t trunk_gport, 
        int max_ports, 
        bcm_gport_t *port_arr,
        int *port_count)
{
    int result = BCM_E_UNAVAIL; 
#if defined (INCLUDE_CCM) || defined (INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if(SOC_IS_TRIDENT2PLUS(unit) &&
       soc_feature(unit, soc_feature_fp_based_oam)) {
    result = bcm_fp_oam_trunk_ports_get(
             unit,
             trunk_gport,
             max_ports,
             port_arr,
             port_count);
    }
#endif
#endif

#if defined (INCLUDE_BHH)
    if (SOC_IS_KATANA(unit)) {
#if defined(BCM_KATANA_SUPPORT)
        result = bcm_en_oam_trunk_ports_get(unit, trunk_gport, max_ports,
                                            port_arr, port_count);
#endif /* BCM_KATANA_SUPPORT */
    } else if (SOC_IS_TRIUMPH3(unit)) {
#if defined(BCM_TRIUMPH3_SUPPORT)
        result = bcm_tr3_oam_trunk_ports_get(unit, trunk_gport, max_ports,
                                             port_arr, port_count);
#endif /* BCM_TRIUMPH3_SUPPORT */
    }
#endif /* INCLUDE_BHH */
   return result;
}

/*
 * Function:
 *      bcm_esw_oam_csf_add
 * Purpose:
 *      Start CSF PDU transmission
 * Parameters:
 *      unit    - (IN)    Unit number.
 *      csf_ptr - (INOUT) CSF object
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_oam_csf_add(int unit, bcm_oam_csf_t *csf_ptr)
{
#if defined(INCLUDE_BHH)
    if (!soc_feature(unit, soc_feature_bhh)) {
        return BCM_E_UNAVAIL;
    }

    if (SOC_IS_SABER2(unit)) {
#if defined(BCM_SABER2_SUPPORT)
        return bcm_sb2_oam_csf_add(unit, csf_ptr);
#endif /* BCM_KATANA2_SUPPORT */
    } else if (SOC_IS_KATANA2(unit)) {
#if defined(BCM_KATANA2_SUPPORT)
        return bcm_kt2_oam_csf_add(unit, csf_ptr);
#endif /* BCM_KATANA2_SUPPORT */
    } else if (SOC_IS_TRIUMPH3(unit)) {
#if defined(BCM_TRIUMPH3_SUPPORT)
        return bcm_tr3_oam_csf_add(unit, csf_ptr);
#endif /* BCM_TRIUMPH3_SUPPORT */
    } else if (SOC_IS_KATANA(unit)) {
#if defined(BCM_KATANA_SUPPORT)
        return bcm_en_oam_csf_add(unit, csf_ptr);
#endif /* BCM_KATANA_SUPPORT */
    } else if (soc_feature(unit, soc_feature_fp_based_oam)) {
#if defined(BCM_APACHE_SUPPORT)
        return _bcm_fp_oam_csf_add(unit, csf_ptr);
#endif /* BCM_APACHE_SUPPORT */
    }

#endif /* INCLUDE_BHH */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_oam_csf_get
 * Purpose:
 *      Get CSF info
 * Parameters:
 *      unit    - (IN)  Unit number.
 *      csf_ptr - (OUT) CSF object
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_oam_csf_get(int unit, bcm_oam_csf_t *csf_ptr)
{
#if defined(INCLUDE_BHH)
    if (!soc_feature(unit, soc_feature_bhh)) {
        return BCM_E_UNAVAIL;
    }

    if (SOC_IS_SABER2(unit)) {
#if defined(BCM_SABER2_SUPPORT)
        return bcm_sb2_oam_csf_get(unit, csf_ptr);
#endif /* BCM_KATANA2_SUPPORT */
    } else if (SOC_IS_KATANA2(unit)) {
#if defined(BCM_KATANA2_SUPPORT)
        return bcm_kt2_oam_csf_get(unit, csf_ptr);
#endif /* BCM_KATANA2_SUPPORT */
    } else if (SOC_IS_TRIUMPH3(unit)) {
#if defined(BCM_TRIUMPH3_SUPPORT)
        return bcm_tr3_oam_csf_get(unit, csf_ptr);
#endif /* BCM_TRIUMPH3_SUPPORT */
    } else if (SOC_IS_KATANA(unit)) {
#if defined(BCM_KATANA_SUPPORT)
        return bcm_en_oam_csf_get(unit, csf_ptr);
#endif /* BCM_KATANA_SUPPORT */
    } else if (soc_feature(unit, soc_feature_fp_based_oam)) {
#if defined(BCM_APACHE_SUPPORT)
        return _bcm_fp_oam_csf_get(unit, csf_ptr);
#endif /* BCM_APACHE_SUPPORT */
    }

#endif /* INCLUDE_BHH */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_oam_csf_delete
 * Purpose:
 *      Stop CSF PDU transmission
 * Parameters:
 *      unit    - (IN  Unit number.
 *      csf_ptr - (IN) CSF object
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_oam_csf_delete(int unit, bcm_oam_csf_t *csf_ptr)
{
#if defined(INCLUDE_BHH)
    if (!soc_feature(unit, soc_feature_bhh)) {
        return BCM_E_UNAVAIL;
    }

    if (SOC_IS_SABER2(unit)) {
#if defined(BCM_SABER2_SUPPORT)
        return bcm_sb2_oam_csf_delete(unit, csf_ptr);
#endif /* BCM_KATANA2_SUPPORT */
    } else if (SOC_IS_KATANA2(unit)) {
#if defined(BCM_KATANA2_SUPPORT)
        return bcm_kt2_oam_csf_delete(unit, csf_ptr);
#endif /* BCM_KATANA2_SUPPORT */
    } else if (SOC_IS_TRIUMPH3(unit)) {
#if defined(BCM_TRIUMPH3_SUPPORT)
        return bcm_tr3_oam_csf_delete(unit, csf_ptr);
#endif /* BCM_TRIUMPH3_SUPPORT */
    } else if (SOC_IS_KATANA(unit)) {
#if defined(BCM_KATANA_SUPPORT)
        return bcm_en_oam_csf_delete(unit, csf_ptr);
#endif /* BCM_KATANA_SUPPORT */
    } else if (soc_feature(unit, soc_feature_fp_based_oam)) {
#if defined(BCM_APACHE_SUPPORT)
        return _bcm_fp_oam_csf_delete(unit, csf_ptr);
#endif /* BCM_APACHE_SUPPORT */
    }

#endif /* INCLUDE_BHH */
    return BCM_E_UNAVAIL;
}

#if defined (INCLUDE_BHH)
int _bcm_oam_bhh_appl_callback(int unit,
                                    int uC,
                                    soc_cmic_uc_shutdown_stage_t stage,
                                    void *user_data) {
    int result = BCM_E_UNAVAIL;

    if (soc_feature(unit, soc_feature_bhh) &&
        (stage == SOC_CMIC_UC_SHUTDOWN_HALTED)) {
        if (SOC_IS_SABER2(unit)) {
            result = _bcm_sb2_oam_bhh_appl_callback (unit, uC, stage,
                                                     user_data);
        } else if (SOC_IS_KATANA2(unit)) {
            result = _bcm_kt2_oam_bhh_appl_callback (unit, uC, stage,
                                                     user_data);
        } else if (SOC_IS_APACHE(unit)) {
            result = _bcm_fp_oam_bhh_appl_callback (unit, uC, stage,
                                                     user_data);
        } else if (SOC_IS_TRIUMPH3(unit)) {
            result = _bcm_tr3_oam_bhh_appl_callback (unit, uC, stage,
                                                     user_data);
        } else if (SOC_IS_KATANA(unit)) {
            result = _bcm_en_oam_bhh_appl_callback (unit, uC, stage,
                                                     user_data);
        }
    }

    return result;

}
#endif

#if defined (INCLUDE_ETH_LM_DM)
int _bcm_oam_eth_lm_dm_appl_callback(int unit,
                                    int uC,
                                    soc_cmic_uc_shutdown_stage_t stage,
                                    void *user_data) {
    int result = BCM_E_UNAVAIL;

    if (soc_feature(unit, soc_feature_eth_lm_dm) &&
        (stage == SOC_CMIC_UC_SHUTDOWN_HALTED)) {
        if (SOC_IS_SABER2(unit)) {
            result = _bcm_sb2_oam_eth_lm_dm_appl_callback (unit, uC, stage,
                                                     user_data);
        }
    }

    return result;

}
#endif

#if defined (INCLUDE_MPLS_LM_DM)
int _bcm_oam_mpls_lm_dm_appl_callback(int unit,
                                    int uC,
                                    soc_cmic_uc_shutdown_stage_t stage,
                                    void *user_data) {
    int result = BCM_E_UNAVAIL;

    /* soc feature mpls_lm_dm is present only for Apache, not for Saber2 */
    if (stage == SOC_CMIC_UC_SHUTDOWN_HALTED) {
        if (SOC_IS_SABER2(unit)) {
            result = _bcm_sb2_oam_mpls_lm_dm_appl_callback (unit, uC, stage,
                                                     user_data);
        }else if ((SOC_IS_APACHE(unit)) && 
            soc_feature(unit, soc_feature_mpls_lm_dm)) {
            result = _bcm_fp_oam_mpls_lm_dm_appl_callback (unit, uC, stage,
                                                     user_data);
        } 
    }

    return result;

}
#endif

/*
 * Function:
 *      _bcm_esw_oam_lmep_trunk_tx_config_recover
 * Purpose:
 *     Recover OAM local endpoint trunk member index configuration.
 * Parameters:
 *     unit        - (IN) BCM device number
 *     port_id     - (IN) Port number for which trunk_index needs to be recovered
 *     trunk_id    - (IN) Trunk id for which member index is to be recovered
 *     trunk_index - (Out) Pointer to derive trunk member index
 * Returns:
 *     BCM_E_XXX
 */
    int
_bcm_esw_oam_lmep_tx_trunk_config_recover(int unit, 
        bcm_trunk_t             trunk_id, 
        bcm_port_t              port_id,
        int                     *trunk_index)
{
    bcm_trunk_member_t      *member_array = NULL; /* Trunk member array */
    int                     member_count = 0;     /* No. of member ports in trunk */
    bcm_trunk_info_t        trunk_info;
    int                     index;
    bcm_gport_t             gport;        /* Gport value. */
    int rv;

    /* Get count of ports in this trunk. */
    rv = bcm_esw_trunk_get(unit, trunk_id, NULL, 0,
            NULL, &member_count);

    if(BCM_FAILURE(rv)) {
        return (BCM_E_NONE);
    }

    if (0 == member_count) {
        /* No members have been added to the trunk group yet */
        return (BCM_E_NONE);
    }

    BCM_IF_ERROR_RETURN(bcm_esw_port_gport_get(unit, port_id, &gport));

    _BCM_OAM_ALLOC(member_array, bcm_trunk_member_t,
            sizeof(bcm_trunk_member_t) * member_count, "Trunk info");
    if (NULL == member_array) {
        return (BCM_E_MEMORY);
    }

    /* Get Trunk Info for the Trunk ID. */
    rv = bcm_esw_trunk_get(unit, trunk_id, &trunk_info,
            member_count, member_array, &member_count);
    if (BCM_FAILURE(rv)) {
        sal_free(member_array);
        return (BCM_E_NONE);
    }
    /* Iterate to get the trunk member index for the gport */
    for(index = 0;index < member_count; index++) {

        if(member_array[index].gport == gport) {
            *trunk_index = index;
            break;

        }
    }

    sal_free(member_array);

    return (BCM_E_NONE);
}

int bcm_esw_oam_mpls_tp_channel_type_tx_set(int unit, bcm_oam_mpls_tp_channel_type_t channel_type, int value)
{

    int result = BCM_E_UNAVAIL;
#if defined (INCLUDE_BHH)
#if defined(BCM_APACHE_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam)) &&
        (soc_feature(unit, soc_feature_bhh)))
    {
        result = bcm_fp_oam_mpls_tp_channel_type_tx_set(unit, channel_type, value);
    }
#endif
    if ((soc_feature(unit, soc_feature_oam)) && (soc_feature(unit, soc_feature_bhh)))
    {
/* KT2 OAM */
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                result = bcm_sb2_oam_mpls_tp_channel_type_tx_set(unit, channel_type, value);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
                result = bcm_kt2_oam_mpls_tp_channel_type_tx_set(unit, channel_type, value);
            }
        }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            result = bcm_tr3_oam_mpls_tp_channel_type_tx_set(unit, channel_type, value);
        }
#endif
#if defined(BCM_KATANA_SUPPORT)
        BCM_IF_ERROR_RETURN(bcm_esw_oam_lock(unit));
        if (SOC_IS_KATANAX(unit))
        {
                result = bcm_en_oam_mpls_tp_channel_type_tx_set(unit, channel_type, value);
        }
        BCM_IF_ERROR_RETURN(bcm_esw_oam_unlock(unit));
#endif
    }
#endif

    return result;
}

int bcm_esw_oam_mpls_tp_channel_type_tx_get(int unit, bcm_oam_mpls_tp_channel_type_t channel_type, int *value)
{

    int result = BCM_E_UNAVAIL;
#if defined (INCLUDE_BHH)
#if defined(BCM_APACHE_SUPPORT)
    if ((soc_feature(unit, soc_feature_fp_based_oam)) &&
        (soc_feature(unit, soc_feature_bhh)))
    {
        result = bcm_fp_oam_mpls_tp_channel_type_tx_get(unit, channel_type, value);
    }
#endif
    if ((soc_feature(unit, soc_feature_oam)) && (soc_feature(unit, soc_feature_bhh)))
    {
/* KT2 OAM */
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                result = bcm_sb2_oam_mpls_tp_channel_type_tx_get(unit, channel_type, value);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
                result = bcm_kt2_oam_mpls_tp_channel_type_tx_get(unit, channel_type, value);
            }
        }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            result = bcm_tr3_oam_mpls_tp_channel_type_tx_get(unit, channel_type, value);
        }
#endif
#if defined(BCM_KATANA_SUPPORT)
        BCM_IF_ERROR_RETURN(bcm_esw_oam_lock(unit));
        if (SOC_IS_KATANAX(unit))
        {
                result = bcm_en_oam_mpls_tp_channel_type_tx_get(unit, channel_type, value);
        }
        BCM_IF_ERROR_RETURN(bcm_esw_oam_unlock(unit));
#endif
    }
#endif

    return result;
}

int bcm_esw_oam_upmep_cosq_set(int unit, bcm_oam_upmep_pdu_type_t upmep_pdu_type,
                           bcm_cos_queue_t cosq)
{

    int result = BCM_E_UNAVAIL;
    if (soc_feature(unit, soc_feature_oam))
    {
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                result = bcm_sb2_oam_upmep_cosq_set(unit, upmep_pdu_type, cosq);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
                result = bcm_kt2_oam_upmep_cosq_set(unit, upmep_pdu_type, cosq);
            }
        }
#endif

    }
    return result;
}

int bcm_esw_oam_upmep_cosq_get(int unit, bcm_oam_upmep_pdu_type_t upmep_pdu_type,
                           bcm_cos_queue_t *cosq)
{

    int result = BCM_E_UNAVAIL;
    if (soc_feature(unit, soc_feature_oam))
    {
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#if defined (BCM_SABER2_SUPPORT)
                result = bcm_sb2_oam_upmep_cosq_get(unit, upmep_pdu_type, cosq);
#endif
            } else if (SOC_IS_KATANA2(unit)) {
                result = bcm_kt2_oam_upmep_cosq_get(unit, upmep_pdu_type, cosq);
            }
        }
#endif

    }
    return result;
}
