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
#include <bcm/flowtracker.h>
#include <bcm/collector.h>

#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/trident3.h>

#if defined (BCM_FLOWTRACKER_SUPPORT)
#include <bcm_int/esw/flowtracker/ft_group.h>
#include <bcm_int/esw/flowtracker/ft_export.h>
#include <bcm_int/esw/flowtracker/ft_user.h>
#include <bcm_int/esw/flowtracker/ft_drop.h>
#include <bcm_int/esw/flowtracker/ft_class.h>
#include <bcm_int/esw/flowtracker/ft_udf.h>
#endif /* BCM_FLOWTRACKER_SUPPORT */

static sal_mutex_t mutex[BCM_MAX_NUM_UNITS];

int bcm_esw_flowtracker_lock(int unit)
{
#ifndef INCLUDE_FLOWTRACKER
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (!(soc_feature(unit, soc_feature_flex_flowtracker_ver_1) ||
          soc_feature(unit, soc_feature_flex_flowtracker_ver_2) ||
          soc_feature(unit, soc_feature_flex_flowtracker_ver_3)))  {

        /* Flowtracker feature is not supported */
        return BCM_E_UNAVAIL;
    }
#endif /* BCM_FLOWTACKER_SUPPORT */
#endif /* INCLUDE_FLOWTRACKER */

    if (mutex[unit] == NULL)
    {
        return BCM_E_INIT;
    }

    sal_mutex_take(mutex[unit], sal_mutex_FOREVER);

    return BCM_E_NONE;
}

int bcm_esw_flowtracker_unlock(int unit)
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
 *      bcm_esw_flowtracker_init
 * Purpose:
 *      Initialize the Flowtracker subsystem.
 * Parameters:
 *      unit         - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_init(int unit)
{
    int result = BCM_E_UNAVAIL;

    /* Create mutex */
    if (mutex[unit] == NULL)
    {
        mutex[unit] = sal_mutex_create("ft.mutex");

        if (mutex[unit] == NULL)
        {
            return BCM_E_MEMORY;
        }
    }

    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
#if defined(INCLUDE_FLOWTRACKER)
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
         result = _bcm_th_ft_init(unit);
    } else
#endif /* INCLUDE_FLOWTRACKER */
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_init(unit);
    } else
#endif /* BCM_FLOWTRACKER_SUPPORT */
    {
        /* If flowtracker is not supported then just return. */
        sal_mutex_destroy(mutex[unit]);
        mutex[unit] = NULL;

        return BCM_E_NONE;
    }

    /* If init itself fails, there is no point in having the mutex.
     * Destroy it.
     */
    if (BCM_FAILURE(result))
    {
        sal_mutex_destroy(mutex[unit]);

        mutex[unit] = NULL;
    }

    return result;
}


/* Shut down the Flowtracker subsystem. */
int bcm_esw_flowtracker_detach(int unit)
{
    int result = BCM_E_UNAVAIL;
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
#if defined(INCLUDE_FLOWTRACKER)
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        /* Take lock */
        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
        result= _bcm_th_ft_detach(unit);
        /* Release lock */
        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    } else
#endif /* INCLUDE_FLOWTRACKER */
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        /* Take lock */
        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
        result = bcmi_ft_cleanup(unit);
        /* Release lock */
        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    } else
#endif /* bcmi_ft_export_state_clear */
    {
        /* If FT is not yet supported then do not do anything. */
        return BCM_E_NONE;
    }

    return result;
}

/*
 * Function:
 *      bcm_flowtracker_export_template_validate
 * Purpose:
 *      Validate the template and return the list supported by the
 *      device.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flow_group_id - (IN) Flowtracker flow group software ID.
 *      num_in_export_elements - (IN) Number of export elements intended in the tempate.
 *      in_export_elements - (IN) List of export elements intended to be in the template.
 *      num_out_export_elements - (IN/OUT) Number of export elements in the supportable tempate.
 *      out_export_elements - (OUT) List of export elements in the template supportable by hardware.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_esw_flowtracker_export_template_validate(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    int max_in_export_elements,
    bcm_flowtracker_export_element_info_t *in_export_elements,
    int max_out_export_elements,
    bcm_flowtracker_export_element_info_t *out_export_elements,
    int *actual_out_export_elements)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_export_template_validate(unit,
                                                  flow_group_id,
                                                  max_in_export_elements,
                                                  in_export_elements,
                                                  max_out_export_elements,
                                                  out_export_elements,
                                                  actual_out_export_elements);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_export_template_create
 * Purpose:
 *      Create an export template
 * Parameters:
 *      unit                    - (IN)    BCM device number
 *      options                 - (IN)    Template create options
 *      id                      - (INOUT) Template Id
 *      set_id                  - (IN)    set_id to be used for the template
 *      num_export_elements     - (IN)    Number of elements in the template
 *      list_of_export_elements - (IN)    Export element list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_export_template_create(
        int unit,
        uint32 options,
        bcm_flowtracker_export_template_t *id,
        uint16 set_id,
        int num_export_elements,
        bcm_flowtracker_export_element_info_t *list_of_export_elements)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
#if defined(INCLUDE_FLOWTRACKER)
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_export_template_create(unit,
                                                   options,
                                                   id,
                                                   set_id,
                                                   num_export_elements,
                                                   list_of_export_elements);
    } else
#endif
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_template_create(unit,
                                         options,
                                         id,
                                         set_id,
                                         num_export_elements,
                                         list_of_export_elements);
    } else
#endif /* BCM_FLOWTRACKER_SUPPORT */
    {
        result = BCM_E_UNAVAIL;
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_export_template_get
 * Purpose:
 *      Get a flowtracker export template with ID.
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      id                      - (IN)  Template Id
 *      set_id                  - (OUT) Set Id of the template
 *      max_size                - (IN)  Size of the export element list array
 *      list_of_export_elements - (OUT) Export element list
 *      list_size               - (OUT) Number of elements in the list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_export_template_get(
        int unit,
        bcm_flowtracker_export_template_t id,
        uint16 *set_id,
        int max_size,
        bcm_flowtracker_export_element_info_t *list_of_export_elements,
        int *list_size)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
#if defined(INCLUDE_FLOWTRACKER)
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_export_template_get(unit,
                                                id,
                                                set_id,
                                                max_size,
                                                list_of_export_elements,
                                                list_size);
    } else
#endif
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_template_get(unit,
                                      id,
                                      set_id,
                                      max_size,
                                      list_of_export_elements,
                                      list_size);
    } else
#endif /* BCM_FLOWTRACKER_SUPPORT */
    {
        result = BCM_E_UNAVAIL;
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_export_template_destroy
 * Purpose:
 *      Destroy a flowtracker export template
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      id                      - (IN)  Template Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_export_template_destroy(
        int unit,
        bcm_flowtracker_export_template_t id)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
#if defined(INCLUDE_FLOWTRACKER)
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result= _bcm_th_ft_export_template_destroy(unit,
                id);
    } else
#endif
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_template_destroy(unit, id);
    } else
#endif /* BCM_FLOWTRACKER_SUPPORT */
    {
        result = BCM_E_UNAVAIL;
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    return result;
}


/*
 * Function:
 *      bcm_esw_flowtracker_collector_create
 * Purpose:
 *      Create a flowtracker collector with given collector info.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      options         - (IN)  Collector create options
 *      id              - (INOUT)  Collector Id
 *      collector_info  - (IN)  Collector info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_collector_create(
        int unit,
        uint32 options,
        bcm_flowtracker_collector_t *collector_id,
        bcm_flowtracker_collector_info_t *collector_info)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
#if defined(INCLUDE_FLOWTRACKER)
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_collector_create(unit, options,
                                             collector_id,
                                             collector_info);
    } else
#endif
    {
        result = BCM_E_UNAVAIL;
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_collector_get
 * Purpose:
 *      Get flowtracker collector information
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      id              - (IN)  Collector Id
 *      collector_info  - (OUT) Collector info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_collector_get(
        int unit,
        bcm_flowtracker_collector_t id,
        bcm_flowtracker_collector_info_t *collector_info)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
#if defined(INCLUDE_FLOWTRACKER)
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_collector_get(unit, id,
                                          collector_info);
    } else
#endif
    {
        result = BCM_E_UNAVAIL;
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    return result;
}


/*
 * Function:
 *      bcm_esw_flowtracker_collector_get_all
 * Purpose:
 *      Get the list of all flowtracker collector Ids configured.
 * Parameters:
 *      unit                 - (IN)  BCM device number
 *      max_size             - (IN)  Size of the collector list array
 *      collector_list       - (OUT) List of collector Ids configured
 *      list_size            - (OUT) NUmber of elements in the list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_collector_get_all(
        int unit,
        int max_size,
        bcm_flowtracker_collector_t *collector_list,
        int *list_size)
{
    int result = BCM_E_UNAVAIL;

    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
#if defined(INCLUDE_FLOWTRACKER)
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_collector_get_all(unit,
                                              max_size,
                                              collector_list,
                                              list_size);
    } else
#endif
    {
        result = BCM_E_UNAVAIL;
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_collector_destroy
 * Purpose:
 *      Destroy a flowtracker collector
 * Parameters:
 *      unit           - (IN)  BCM device number
 *      collector_id   - (IN) Collector Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_collector_destroy(
        int unit,
        bcm_flowtracker_collector_t id)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
#if defined(INCLUDE_FLOWTRACKER)
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result= _bcm_th_ft_collector_destroy(unit, id);
    } else
#endif
    {
        result = BCM_E_UNAVAIL;
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_create
 * Purpose:
 *      Create a flowtracker flow group
 * Parameters:
 *      unit              - (IN)    BCM device number
 *      options           - (IN)    Group create options
 *      id                - (INOUT) Group Id
 *      flow_group_info   - (IN)    Group Info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_create(
        int unit,
        uint32 options,
        bcm_flowtracker_group_t *flow_group_id,
        bcm_flowtracker_group_info_t *flow_group_info)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
#if defined(INCLUDE_FLOWTRACKER)
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_flow_group_create(unit, options,
                                              flow_group_id,
                                              flow_group_info);
    } else
#endif
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_create(unit, options,
                                      flow_group_id,
                                      flow_group_info);
    } else
#endif
    {
        return (BCM_E_UNAVAIL);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_get
 * Purpose:
 *      Get flowtracker flow group information
 * Parameters:
 *      unit              - (IN)  BCM device number
 *      id                - (IN)  Group Id
 *      flow_group_info   - (OUT) Group Info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_group_info_t *flow_group_info)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
#if defined(INCLUDE_FLOWTRACKER)
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result= _bcm_th_ft_flow_group_get(unit, id,
                                          flow_group_info);
    } else
#endif
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_get(unit, id, flow_group_info);
    } else
#endif
    {
        return (BCM_E_UNAVAIL);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_get_all
 * Purpose:
 *      Get list of all flow groups created
 * Parameters:
 *      unit              - (IN)  BCM device number
 *      max_size          - (IN)  Size of the flow group list array
 *      flow_group_list   - (OUT) List of flow groups created
 *      list_size         - (OUT) Number of flow grups in the list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_get_all(
        int unit,
        int max_size,
        bcm_flowtracker_group_t *flow_group_list,
        int *list_size)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
#if defined(INCLUDE_FLOWTRACKER)
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_flow_group_get_all(unit,
                                               max_size,
                                               flow_group_list,
                                               list_size);
    } else
#endif
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_get_all(unit,
                max_size, flow_group_list, list_size);
    } else
#endif
    {
        return (BCM_E_UNAVAIL);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_flow_limit_set
 * Purpose:
 *      Set flow limit on the flow group
 * Parameters:
 *      unit         - (IN)  BCM device number
 *      id           - (IN)  Flow group Id
 *      flow_limit   - (IN) Max number of flows that can be learnt on the group
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_flow_limit_set(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 flow_limit)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
#if defined(INCLUDE_FLOWTRACKER)
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_flow_group_flow_limit_set(unit, id,
                                                      flow_limit);
    } else
#endif
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
         result = bcmi_ft_group_flow_limit_set(unit, id, flow_limit);
    } else
#endif
    {
        return (BCM_E_UNAVAIL);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    return result;
}

/*
 * Function:
 *      bcm_esw_ft_flow_group_flow_limit_get
 * Purpose:
 *      Get flow limit of the flow group
 * Parameters:
 *      unit         - (IN)  BCM device number
 *      id           - (IN)  Flow group Id
 *      flow_limit   - (OUT) Flow limit
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_flow_limit_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 *flow_limit)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
#if defined(INCLUDE_FLOWTRACKER)
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_flow_group_flow_limit_get(unit, id,
                                                      flow_limit);
    } else
#endif
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
         result = bcmi_ft_group_flow_limit_get(unit, id, flow_limit);
    } else
#endif
    {
        return (BCM_E_UNAVAIL);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    return result;
}

/*
 * Function:
 *  bcm_esw_flowtracker_group_stat_modeid_set
 * Purpose:
 *  Set stat mode id to provided flow tracker group.
 * Parameters:
 *  unit         - (IN) BCM device number
 *  id           - (IN) Flow group Id
 *  stat_modeid  - (IN) Stat Mode Id
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_stat_modeid_set(int unit,
                                              bcm_flowtracker_group_t id,
                                              uint32 stat_modeid)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Set stat modeid created using flex stat api */
#if defined(INCLUDE_FLOWTRACKER)
    if (soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
            result = _bcm_th_ft_flow_group_stat_modeid_set(unit, id,
                                                           stat_modeid);
    }
#endif
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    return result;
}

/*
 * Function:
 *  bcm_esw_flowtracker_group_stat_modeid_get
 * Purpose:
 *  Get flow limit of the flow group
 * Parameters:
 *  unit         - (IN)  BCM device number
 *  id           - (IN)  Flow group Id
 *  stat_modeid  - (OUT) Stat Modeid associated to flow tracker group
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_stat_modeid_get(int unit,
                                              bcm_flowtracker_group_t id,
                                              uint32 *stat_modeid)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Get stat modeid created using flex stat api */
#if defined(INCLUDE_FLOWTRACKER)
    if (soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_flow_group_stat_modeid_get(unit, id,
                                                       stat_modeid);
    }
#endif

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_age_timer_set
 * Purpose:
 *      Set aging timer interval in ms on the flow group. Aging interval has to
 *      be configured in steps of 1sec with a minimum of 1sec. Default value of
 *      1 minute.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      id                  - (IN) Flow group Id
 *      aging_interval_ms   - (IN) Aging interval in msec
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_age_timer_set(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 aging_interval_ms)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
#if defined(INCLUDE_FLOWTRACKER)
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_flow_group_age_timer_set(unit, id,
                                                     aging_interval_ms);
    } else
#endif
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
         result = bcmi_ft_group_age_out_set(unit, id, aging_interval_ms);
    } else
#endif
    {
        return (BCM_E_UNAVAIL);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    return result;
}

/*
 * Function:
 *      _bcm_th_ft_flow_group_age_timer_get
 * Purpose:
 *      Get aging timer interval in ms set on the flow group.
 * Parameters:
 *      unit                - (IN)  BCM device number
 *      id                  - (IN)  Flow group Id
 *      aging_interval_ms   - (OUT) Aging interval in msec
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_age_timer_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 *aging_interval_ms)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
#if defined(INCLUDE_FLOWTRACKER)
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_flow_group_age_timer_get(unit, id,
                                                     aging_interval_ms);
    } else
#endif
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
         result = bcmi_ft_group_age_out_get(unit, id, aging_interval_ms);
    } else
#endif
    {
        return (BCM_E_UNAVAIL);
    }


    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    return result;
}

/* Set export trigger information of the flow group with ID. */
int bcm_esw_flowtracker_group_export_trigger_set(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_export_trigger_info_t *export_trigger_info)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     */
#if defined(INCLUDE_FLOWTRACKER)
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_flow_group_export_trigger_set(unit, id,
                                                     export_trigger_info);
    } else
#endif
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
         result = bcmi_ft_group_export_trigger_set(
                unit, id,
                export_trigger_info);
    }
#endif
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    return result;
}

/* Get export trigger information of the flow group with ID. */
int bcm_esw_flowtracker_group_export_trigger_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_export_trigger_info_t *export_trigger_info)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     */
#if defined(INCLUDE_FLOWTRACKER)
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_flow_group_export_trigger_get(unit, id,
                                                     export_trigger_info);
    } else
#endif
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
         result = bcmi_ft_group_export_trigger_get(
                unit, id,
                export_trigger_info);
    }
#endif
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_flow_count_get
 * Purpose:
 *      Get the number of flows learnt in the flow group
 * Parameters:
 *      unit             - (IN)  BCM device number
 *      id               - (IN)  Flow group Id
 *      flow_count       - (OUT) Number of flows learnt
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_flow_count_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 *flow_count)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
#if defined(INCLUDE_FLOWTRACKER)
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_flow_group_flow_count_get(unit, id,
                                                      flow_count);
    }
#endif
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_destroy
 * Purpose:
 *      Destroy a flowtracker flow group
 * Parameters:
 *      unit             - (IN)  BCM device number
 *      id               - (IN)  Flow group Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_destroy(
        int unit,
        bcm_flowtracker_group_t id)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
#if defined(INCLUDE_FLOWTRACKER)
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result= _bcm_th_ft_flow_group_destroy(
                unit, id);
    } else
#endif
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1))  {
        result = bcmi_ft_group_destroy(unit, id);
    } else
#endif
    {
        return (BCM_E_UNAVAIL);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_clear
 * Purpose:
 *      Clear a flow group's flow entries.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      id            - (IN)  Flow group Id
 *      flags         - (IN)  Clear params
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_clear(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 flags)
{
    int result = BCM_E_UNAVAIL;

#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result= _bcm_th_ft_group_clear(unit, id, flags);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcm_esw_flowtracker_group_multi_clear(unit, flags, 1, &id);
    }
#endif

    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_collector_add
 * Purpose:
 *      Associate flow group to a collector with an export template.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      flow_group_id   - (IN)  Flow group Id
 *      collector_id    - (IN)  Collector Id
 *      template_id     - (IN)  Template Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_collector_add(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_flowtracker_collector_t collector_id,
        bcm_flowtracker_export_template_t template_id)
{
    int result = BCM_E_UNAVAIL;

    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

#if defined(INCLUDE_FLOWTRACKER)
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_flow_group_collector_add(unit, flow_group_id,
                                                     collector_id, template_id);
    } else
#endif
    {
        result = BCM_E_UNAVAIL;
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_collector_delete
 * Purpose:
 *      Dis-associate flow group from a collector with an export template.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      flow_group_id   - (IN)  Flow group Id
 *      collector_id    - (IN)  Collector Id
 *      template_id     - (IN)  Template Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_collector_delete(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_flowtracker_collector_t collector_id,
        bcm_flowtracker_export_template_t template_id)
{
    int result = BCM_E_UNAVAIL;

    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

#if defined(INCLUDE_FLOWTRACKER)
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_flow_group_collector_delete(unit, flow_group_id,
                                                        collector_id, template_id);
    } else
#endif
    {
        result = BCM_E_UNAVAIL;
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_collector_get_all
 * Purpose:
 *      Get list of all collectors, templates  attached to a flow group
 * Parameters:
 *      unit                - (IN)  BCM device number
 *      flow_group_id       - (IN)  Flow group Id
 *      max_list_size       - (IN)  Size of the list arrays
 *      list_of_collectors  - (OUT) Collector list
 *      list_of_templates   - (OUT) Template list
 *      list_size           - (OUT) Number of elements in the lists
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_collector_get_all(
    int unit, 
    bcm_flowtracker_group_t flow_group_id, 
    int max_list_size, 
    bcm_flowtracker_collector_t *list_of_collectors, 
    bcm_flowtracker_export_template_t *list_of_templates, 
    int *list_size)
{
    int result = BCM_E_UNAVAIL;

    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

#if defined(INCLUDE_FLOWTRACKER)
    /* Learning and exporting features via UC are both 
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result= _bcm_th_ft_flow_group_collector_get_all(
                unit, flow_group_id, max_list_size, list_of_collectors,
                list_of_templates, list_size);
    } else
#endif
    {
        result = BCM_E_UNAVAIL;
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));


    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_collector_attach
 * Purpose:
 *      Associate flow group to a collector with an export template.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      flow_group_id   - (IN)  Flow group Id
 *      collector_id    - (IN)  Collector Id
 *      template_id     - (IN)  Template Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_collector_attach(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_flowtracker_collector_t collector_id,
        int export_profile_id,
        bcm_flowtracker_export_template_t template_id)
{
    int result = BCM_E_UNAVAIL;

    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

#if defined(INCLUDE_FLOWTRACKER)
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export))) {
            result = _bcm_th_ft_group_collector_attach(unit, flow_group_id,
                                                       collector_id,
                                                       export_profile_id,
                                                       template_id);
    } else
#endif
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_collector_attach(unit,
                                                flow_group_id,
                                                collector_id,
                                                export_profile_id,
                                                template_id);

    } else
#endif /* BCM_FLOWTRACKER_SUPPORT */
    {
        result = BCM_E_UNAVAIL;
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_collector_detach
 * Purpose:
 *      Dis-associate flow group from a collector with an export template.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      flow_group_id   - (IN)  Flow group Id
 *      collector_id    - (IN)  Collector Id
 *      template_id     - (IN)  Template Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_collector_detach(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_flowtracker_collector_t collector_id,
        int export_profile_id,
        bcm_flowtracker_export_template_t template_id)
{
    int result = BCM_E_UNAVAIL;

    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

#if defined(INCLUDE_FLOWTRACKER)
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export))) {
            result = _bcm_th_ft_group_collector_detach(unit, flow_group_id,
                                                       collector_id,
                                                       export_profile_id,
                                                       template_id);
    } else
#endif
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_collector_detach(unit,
                                                flow_group_id,
                                                collector_id,
                                                export_profile_id,
                                                template_id);

    } else
#endif /* BCM_FLOWTRACKER_SUPPORT */
    {
        result = BCM_E_UNAVAIL;
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_collector_attach_get_all
 * Purpose:
 *      Get list of all collectors, templates  attached to a flow group
 * Parameters:
 *      unit                - (IN)  BCM device number
 *      flow_group_id       - (IN)  Flow group Id
 *      max_list_size       - (IN)  Size of the list arrays
 *      list_of_collectors  - (OUT) Collector list
 *      list_of_templates   - (OUT) Template list
 *      list_size           - (OUT) Number of elements in the lists
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_collector_attach_get_all(
    int unit, 
    bcm_flowtracker_group_t flow_group_id, 
    int max_list_size, 
    bcm_flowtracker_collector_t *list_of_collectors, 
    int *list_of_export_profile_ids,
    bcm_flowtracker_export_template_t *list_of_templates, 
    int *list_size)
{
    int result = BCM_E_UNAVAIL;

    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

#if defined(INCLUDE_FLOWTRACKER)
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export))) {
            result =
             _bcm_th_ft_group_collector_attach_get_all(unit, flow_group_id,
                                                       max_list_size,
                                                       list_of_collectors,
                                                       list_of_export_profile_ids,
                                                       list_of_templates,
                                                       list_size);
    } else
#endif
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_collector_attach_get_all(unit,
                                                        flow_group_id,
                                                        max_list_size,
                                                        list_of_collectors,
                                                        list_of_export_profile_ids,
                                                        list_of_templates,
                                                        list_size);

    } else
#endif /* BCM_FLOWTRACKER_SUPPORT */
    {
        result = BCM_E_UNAVAIL;
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));


    return result;
}


/*
 * Function:
 *      bcm_esw_flowtracker_group_data_get
 * Purpose:
 *      Get flow data for a given flow key within the given flow group.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      flow_group_id - (IN)  Flow group Id
 *      flow_key      - (IN)  Five tuple that constitutes a flow
 *      flow_data     - (OUT) Data associated with the flow key
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_data_get(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_flowtracker_flow_key_t *flow_key,
        bcm_flowtracker_flow_data_t *flow_data)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
#if defined(INCLUDE_FLOWTRACKER)
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result= _bcm_th_ft_group_data_get(unit, flow_group_id,
                                                   flow_key, flow_data);
    }
#endif
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    return result;
}

void bcmi_esw_flowtracker_sw_dump(int unit) {

#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        bcmi_ft_state_dump(unit, FALSE);
    }
#endif
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_collector_copy_info_get
 * Purpose:
 *      Get flowtracker group's collector copy info.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      id            - (IN)  Flow group Id
 *      info          - (OUT) Collector copy inforamtion.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_esw_flowtracker_group_collector_copy_info_get(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_collector_copy_info_t *info)
{


    int result = BCM_E_UNAVAIL;
#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1) &&
        (soc_feature(unit, soc_feature_flowtracker_ver_1_collector_copy))) {
        result = bcmi_ft_group_collector_copy_get(unit, id, info);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_collector_copy_info_set
 * Purpose:
 *      Set flowtracker group's collector copy info.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      id            - (IN)  Flow group Id
 *      info         - (IN)  Collector copy inforamtion.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_esw_flowtracker_group_collector_copy_info_set(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_collector_copy_info_t info)
{

    int result = BCM_E_UNAVAIL;
#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1) &&
        (soc_feature(unit, soc_feature_flowtracker_ver_1_collector_copy))) {

        result = bcmi_ft_group_collector_copy_set(unit, id, info);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_meter_info_set
 * Purpose:
 *      set meter inforamtion for flowtracker group.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      id            - (IN)  Flow group Id
 *      info          - (IN)  meter info.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */

int
bcm_esw_flowtracker_group_meter_info_set(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_meter_info_t info)
{
    int result = BCM_E_UNAVAIL;
#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1) &&
        (soc_feature(unit, soc_feature_flowtracker_ver_1_metering))) {
        result = bcmi_ft_group_meter_profile_set(unit, id, info);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_meter_info_get
 * Purpose:
 *      Get meter inforamtion for flowtracker group.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      id            - (IN)  Flow group Id
 *      info          - (OUT) meter info.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_esw_flowtracker_group_meter_info_get(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_meter_info_t *info)
{

    int result = BCM_E_UNAVAIL;
#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1) &&
        (soc_feature(unit, soc_feature_flowtracker_ver_1_metering))) {

        result = bcmi_ft_group_meter_profile_get(unit, id, info);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}

/*
 * Function:
 *   bcm_esw_flowtracker_check_create
 * Purpose:
 *   Create flowtracker check.
 * Parameters:
 *   unit          - (IN)  BCM device number
 *   options       - (IN)  Options to create check
 *   check_info    - (IN)  check info.
 *   check_id      - (OUT) Software id of check
 * Returns:
 *   BCM_E_XXX   - BCM error code.
 */
int
bcm_esw_flowtracker_check_create(int unit,
                            uint32 options,
                            bcm_flowtracker_check_info_t check_info,
                            bcm_flowtracker_check_t *check_id)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_check_create(unit, options, check_info, check_id);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}

/*
 * Function:
 *   bcm_esw_flowtracker_check_get
 * Purpose:
 *   Create flowtracker check.
 * Parameters:
 *   unit          - (IN)  BCM device number
 *   check_id      - (IN) Software id of check
 *   check_info    - (OUT)  check info.
 * Returns:
 *   BCM_E_XXX   - BCM error code.
 */
int
bcm_esw_flowtracker_check_get(int unit,
                              bcm_flowtracker_check_t check_id,
                              bcm_flowtracker_check_info_t *check_info)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_check_get(unit, check_id, check_info, NULL);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}


/*
 * Function:
 *   bcm_esw_flowtracker_check_destroy
 * Purpose:
 *   Create flowtracker check.
 * Parameters:
 *   unit        - (IN) BCM device number
 *   check_id    - (IN) Software id of check
 * Returns:
 *   BCM_E_XXX   - BCM error code.
 */
int
bcm_esw_flowtracker_check_destroy(int unit,
                                  bcm_flowtracker_check_t check_id)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_check_destroy(unit, check_id);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}


/* Destroy all the flow checks. */
int
bcm_esw_flowtracker_check_destroy_all(int unit)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_check_destroy_all(unit);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}


/* Traverse through the flow checks and run callback at each valid entry. */
int
bcm_esw_flowtracker_check_traverse(int unit,
                                       bcm_flowtracker_check_traverse_cb cb,
                                       void *user_data)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_check_traverse(unit, cb, user_data);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}

/* Set action information of flow check. */
int
bcm_esw_flowtracker_check_action_info_set(int unit,
                      bcm_flowtracker_check_t check_id,
                      bcm_flowtracker_check_action_info_t info)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_check_action_info_set(unit, check_id, info);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}

/* Get action information of flow check. */
int
bcm_esw_flowtracker_check_action_info_get(int unit,
                  bcm_flowtracker_check_t check_id,
                  bcm_flowtracker_check_action_info_t *info)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_check_action_info_get(unit, check_id, info);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}

/* Set export information of flow check. */
int
bcm_esw_flowtracker_check_export_info_set(int unit,
                      bcm_flowtracker_check_t check_id,
                      bcm_flowtracker_check_export_info_t info)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_check_export_info_set(unit, check_id, info);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}

/* Get export information of flow check. */
int
bcm_esw_flowtracker_check_export_info_get(int unit,
                  bcm_flowtracker_check_t check_id,
         bcm_flowtracker_check_export_info_t *info)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_check_export_info_get(unit, check_id, info);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}

/*
 * Function:
 *   bcm_esw_flowtracker_check_delay_info_set
 * Purpose:
 *   Set delay information to flow checker
 * Parameters:
 *   unit          - (IN)  BCM device number
 *   check_id      - (IN) Software id of check
 *   info          - (IN) Delay Information
 * Returns:
 *   BCM_E_XXX   - BCM error code.
 */
int
bcm_esw_flowtracker_check_delay_info_set(int unit,
                    bcm_flowtracker_check_t check_id,
                    bcm_flowtracker_check_delay_info_t info)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT) & defined(BCM_FLOWTRACKER_V2_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2) &&
       (soc_feature(unit, soc_feature_flowtracker_ver_2_alu_delay))) {
        result = bcmi_ftv2_check_delay_info_set(unit, check_id, info);
    }

    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}

/*
 * Function:
 *   bcm_esw_flowtracker_check_delay_info_get
 * Purpose:
 *   Get delay information to flow checker
 * Parameters:
 *   unit          - (IN)  BCM device number
 *   check_id      - (IN) Software id of check
 *   info          - (OUT) Delay Information
 * Returns:
 *   BCM_E_XXX   - BCM error code.
 */
int
bcm_esw_flowtracker_check_delay_info_get(int unit,
                            bcm_flowtracker_check_t check_id,
                            bcm_flowtracker_check_delay_info_t *info)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT) & defined(BCM_FLOWTRACKER_V2_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2) &&
       (soc_feature(unit, soc_feature_flowtracker_ver_2_alu_delay))) {
        result = bcmi_ftv2_check_delay_info_get(unit, check_id, info);
    }

    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}

/* Add flow check to this flowtracker group */
int
bcm_esw_flowtracker_group_check_add(int unit,
                    bcm_flowtracker_group_t group_id,
                    bcm_flowtracker_check_t check_id)
{

    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_flowchecker_add(unit, group_id, check_id);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;

}

/* Delete flow check from this flowtracker group. */
int
bcm_esw_flowtracker_group_check_delete(int unit,
                    bcm_flowtracker_group_t group_id,
                    bcm_flowtracker_check_t check_id)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_flowchecker_delete(unit, group_id, check_id);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;

}

/* Get all the checks associated with the group. */
int
bcm_esw_flowtracker_group_check_get_all(int unit,
                    bcm_flowtracker_group_t group_id,
                    int max_checks,
                    bcm_flowtracker_check_t *list_of_check_ids,
                    int *num_checks)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_flowtracker_check_get_all(unit, group_id, 1,
                    max_checks, list_of_check_ids, num_checks);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;

}

/* Set tracking parameter for this flowtracker group. */
int
bcm_esw_flowtracker_group_tracking_params_set(int unit,
                                bcm_flowtracker_group_t id,
                                int num_tracking_params,
    bcm_flowtracker_tracking_param_info_t *list_of_tracking_params)
{
    int result = BCM_E_UNAVAIL;

    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
#if defined(INCLUDE_FLOWTRACKER)
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_flow_group_tracking_params_set(unit, id,
                                                     num_tracking_params,
                                                     list_of_tracking_params);
    } else
#endif
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_tracking_param_set(unit, id,
                     num_tracking_params, list_of_tracking_params);
    }
#endif
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}

/* Get flowtracker tracking parameters for this group. */
int
bcm_esw_flowtracker_group_tracking_params_get(int unit,
                                bcm_flowtracker_group_t id,
                                int max_size,
    bcm_flowtracker_tracking_param_info_t *list_of_tracking_params,
                                int *list_size)
{
    int result = BCM_E_UNAVAIL;

    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
#if defined(INCLUDE_FLOWTRACKER)
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_flow_group_tracking_params_get(unit, id,
                                                     max_size,
                                                     list_size,
                                                     list_of_tracking_params);
    } else
#endif
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_tracking_param_get(unit, id, max_size,
                     list_of_tracking_params, list_size);
    }
#endif
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;

}

/*
 * Function:
 *      bcm_esw_flowtracker_group_multi_clear
 * Purpose:
 *      Clear data for multiple groups.
 * Parameters:
 *      unit              - (IN) BCM device number
 *      id                - (IN) Flow group Id
 *      num_groups        - (IN) Number of groups to clear
 *      list_of_group_ids - (IN) List of Groups ids to be cleared.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_esw_flowtracker_group_multi_clear(
    int unit,
    uint32 flags,
    int num_groups,
    bcm_flowtracker_group_t *list_of_group_ids)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_flush_set(unit,
                flags, num_groups, list_of_group_ids);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}


/*
 * Function:
 *      bcm_esw_flowtracker_group_control_set
 * Purpose:
 *      Set value of group control.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      id            - (IN)  Flow group Id
 *      type          - (IN)  type of group control.
 *      arg           - (IN) value of control
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_esw_flowtracker_group_control_set(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_group_control_type_t type,
    int arg)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_flowtracker_group_control_set(unit, id, type, arg);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_control_get
 * Purpose:
 *      Get value of group control.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      id            - (IN)  Flow group Id
 *      type          - (IN)  type of group control.
 *      arg           - (OUT) return value of control
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_esw_flowtracker_group_control_get(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_group_control_type_t type,
    int *arg)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_flowtracker_group_control_get(unit, id, type, arg);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;

}

/*
 * Function:
 *      bcm_esw_flowtracker_group_stat_set
 * Purpose:
 *      Set Group statistics
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      id            - (IN)  Flow group Id
 *      group_stats   - (IN)  Group statistics
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_esw_flowtracker_group_stat_set(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_group_stat_t *group_stat)
{


    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_stat_set(unit, id, group_stat);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_stat_get
 * Purpose:
 *      Get Group statistics
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      id            - (IN)  Flow group Id
 *      group_stats   - (OUT)  Group statistics
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_esw_flowtracker_group_stat_get(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_group_stat_t *group_stat)
{

    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_stat_get(unit, id, group_stat);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}


/*
 * Function:
 *      bcm_flowtracker_export_record_register
 * Purpose:
 *      Register callback routine for Local collector Flowtracker
 *      record export.
 * Parameters:
 *      unit - (IN) Unit number.
 *      collector_id - (IN) Flowtracker collector Software ID.
 *      callback_options - (IN) Flowtracker export callback options.
 *      callback_fn - (IN) <UNDEF>
 *      userdata - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_esw_flowtracker_export_record_register(
    int unit,
    bcm_flowtracker_collector_t collector_id,
    bcm_flowtracker_collector_callback_options_t callback_options,
    bcm_flowtracker_export_record_cb_f callback_fn,
    void *userdata)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_bscan_ft_fifodma_support)) {
        result = bcmi_ft_export_record_register(unit,
                                                collector_id,
                                                callback_options,
                                                callback_fn,
                                                userdata);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}

/*
 * Function:
 *      bcm_flowtracker_export_record_unregister
 * Purpose:
 *      Unregister callback routine for Local collector Flowtracker
 *      record export.
 * Parameters:
 *      unit - (IN) Unit number.
 *      collector_id - (IN) Flowtracker collector Software ID.
 *      callback_options - (IN) Flowtracker export callback options.
 *      callback_fn - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_esw_flowtracker_export_record_unregister(
    int unit,
    bcm_flowtracker_collector_t collector_id,
    bcm_flowtracker_collector_callback_options_t callback_options,
    bcm_flowtracker_export_record_cb_f callback_fn)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_bscan_ft_fifodma_support)) {
        result = bcmi_ft_export_record_unregister(unit,
                                                  collector_id,
                                                  callback_options,
                                                  callback_fn);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}


/*
 * Function:
 *      bcm_esw_flowtracker_user_entry_add
 * Purpose:
 *      Add a user flow entry basis user input key elements. API
 *      expects that all tracking parametrs of type = 'KEY' in the
 *      group are specified.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flow_group_id - (IN) Flowtracker flow group ID.
 *      options - (IN) Options for creting the flowtracker user entry.
 *      num_user_entry_params - (IN) Number of user entry params.
 *      user_entry_param_list - (IN) List of user entry params to add flow entry.
 *      entry_handle - (OUT) User entry handle corresponding to the entry added.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_esw_flowtracker_user_entry_add(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    uint32 options,
    int num_user_entry_params,
    bcm_flowtracker_tracking_param_info_t *user_entry_param_list,
    bcm_flowtracker_user_entry_handle_t *entry_handle)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ftk_user_entry_add(unit,
                                         flow_group_id,
                                         options,
                                         num_user_entry_params,
                                         user_entry_param_list,
                                         entry_handle);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_user_entry_get
 * Purpose:
 *      Fetch user entry info given the entry handle.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry_handle - (IN) Flowtracker user entry handle.
 *      num_user_entry_params - (IN) Number of user entry params.
 *      user_entry_param_list - (IN/OUT) List of user entry params added in the flow entry.
 *      actual_user_entry_params - (OUT) Actual number of params in the user entry.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_esw_flowtracker_user_entry_get(
    int unit,
    bcm_flowtracker_user_entry_handle_t *entry_handle,
    int num_user_entry_params,
    bcm_flowtracker_tracking_param_info_t *user_entry_param_list,
    int *actual_user_entry_params)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ftk_user_entry_get(unit,
                                         entry_handle,
                                         num_user_entry_params,
                                         user_entry_param_list,
                                         actual_user_entry_params);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_user_entry_get_all
 * Purpose:
 *      Fetch all user entries added in a against a given flow group.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flow_group_id - (IN) Flowtracker flow group ID.
 *      num_user_entry_handles - (IN) Number of export elements intended in the tempate.
 *      user_entry_handle_list - (IN/OUT) List of user entry handles corresponding to the user entries added.
 *      actual_user_entry_handles - (OUT) Actual number of user entry handles corresponding to the entries added.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_esw_flowtracker_user_entry_get_all(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    int num_user_entry_handles,
    bcm_flowtracker_user_entry_handle_t *user_entry_handle_list,
    int *actual_user_entry_handles)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ftk_user_entry_get_all(unit,
                                             flow_group_id,
                                             num_user_entry_handles,
                                             user_entry_handle_list,
                                             actual_user_entry_handles);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_user_entry_delete
 * Purpose:
 *      Delete user flow entry that is added earlier.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flow_group_id - (IN) Flowtracker flow group ID.
 *      entry_handle - (IN) User entry handle corresponding to the entry added.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_esw_flowtracker_user_entry_delete(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_flowtracker_user_entry_handle_t *entry_handle)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ftk_user_entry_delete(unit,
                                            flow_group_id,
                                            entry_handle);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_user_entry_delete_all
 * Purpose:
 *      Delete user flow entry that is added earlier.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flow_group_id - (IN) Flowtracker flow group ID.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_esw_flowtracker_user_entry_delete_all(
    int unit,
    bcm_flowtracker_group_t flow_group_id)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ftk_user_entry_delete_all(unit,
                                                flow_group_id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}



/*
 * Function:
 *   bcm_esw_flowtracker_sync
 * Purpose:
 *   Warmboot sync routine for flowtracker
 * Parameters:
 *   unit - (IN) Unit number.
 * Returns:
 *   BCM_E_xxx
 * Notes:
 */
int
bcm_esw_flowtracker_sync(int unit)
{
    int result = BCM_E_UNAVAIL;

#ifdef BCM_WARM_BOOT_SUPPORT
#if defined(BCM_FLOWTRACKER_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_sync(unit);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif
#endif

    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_template_transmit_config_set
 * Purpose:
 *      Set the template set transmit configuration
 * Parameters:
 *      unit         - (IN) BCM device number
 *      template_id  - (IN) Template Id
 *      collector_id - (IN) Collector Id
 *      config       - (IN) Template transmit config
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_esw_flowtracker_template_transmit_config_set(
                             int unit,
                             bcm_flowtracker_export_template_t template_id,
                             bcm_flowtracker_collector_t collector_id,
                             bcm_flowtracker_template_transmit_config_t *config)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_uc_flowtracker_export)) {
            result = _bcm_th_ft_template_transmit_config_set(unit, template_id,
                                                             collector_id, config);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_template_transmit_config_get
 * Purpose:
 *      Get the template set transmit configuration
 * Parameters:
 *      unit         - (IN)  BCM device number
 *      template_id  - (IN)  Template Id
 *      collector_id - (IN)  Collector Id
 *      config       - (OUT) Template transmit config
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_esw_flowtracker_template_transmit_config_get(
                             int unit,
                             bcm_flowtracker_export_template_t template_id,
                             bcm_flowtracker_collector_t collector_id,
                             bcm_flowtracker_template_transmit_config_t *config)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_uc_flowtracker_export)) {
            result = _bcm_th_ft_template_transmit_config_get(unit,
                                                             template_id,
                                                             collector_id,
                                                             config);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_actions_set
 * Purpose:
 *      Set list of actions on a flow group.
 * Parameters:
 *      unit          - (IN) BCM device number
 *      flow_group_id - (IN) Flow group Id
 *      flags         - (IN) Flags
 *      num_actions   - (IN) Number of actions in the list.
 *      action_list   - (IN) Action list.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_actions_set(
                               int unit,
                               bcm_flowtracker_group_t flow_group_id,
                               uint32 flags,
                               int num_actions,
                               bcm_flowtracker_group_action_info_t *action_list)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_group_actions_set(unit, flow_group_id, flags,
                                              num_actions, action_list);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_actions_get
 * Purpose:
 *      Get list of actions applied on a flow group.
 * Parameters:
 *      unit          - (IN) BCM device number
 *      flow_group_id - (IN) Flow group Id
 *      flags         - (IN) Flags
 *      max_actions   - (IN) Maximum number of actions that can be
 *                           accomodated in the list.
 *      action_list   - (OUT) Action list.
 *      num_actions   - (OUT) Actual number of actions in the list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_actions_get(
                               int unit,
                               bcm_flowtracker_group_t flow_group_id,
                               uint32 flags,
                               int max_actions,
                               bcm_flowtracker_group_action_info_t *action_list,
                               int *num_actions)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_group_actions_get(unit, flow_group_id, flags,
                                              max_actions, action_list,
                                              num_actions);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_elephant_profile_create
 * Purpose:
 *      Create flowtracker elephant profile
 * Parameters:
 *      unit         - (IN)     BCM device number
 *      options      - (IN)     Elephant profile creation options.
 *      profile      - (IN)     Elephant profile information
 *      profile_id   - (IN/OUT) Elephant profile id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_elephant_profile_create(
                                int unit,
                                uint32 options,
                                bcm_flowtracker_elephant_profile_info_t *profile,
                                bcm_flowtracker_elephant_profile_t *profile_id)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_elephant_profile_create(unit, options,
                                                    profile, profile_id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}
/*
 * Function:
 *      bcm_esw_flowtracker_elephant_profile_destroy
 * Purpose:
 *       Destroy a flowtracker elephant profile.
 * Parameters:
 *      unit         - (IN)     BCM device number
 *      profile_id   - (IN/OUT) Elephant profile id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_elephant_profile_destroy(
                                   int unit,
                                   bcm_flowtracker_elephant_profile_t profile_id)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_elephant_profile_destroy(unit, profile_id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_elephant_profile_get
 * Purpose:
 *       Get flowtracker elephant profile information.
 * Parameters:
 *      unit         - (IN)  BCM device number
 *      profile_id   - (IN)  Elephant profile id
 *      profile      - (OUT) Elephant profile information
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_elephant_profile_get(
                                             int unit,
                                             bcm_flowtracker_elephant_profile_t profile_id,
                                             bcm_flowtracker_elephant_profile_info_t *profile)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_elephant_profile_get(unit,
                                                 profile_id,
                                                 profile);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_elephant_profile_get_all
 * Purpose:
 *       Get the list of all flowtracker elephant profiles configured.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      max           - (IN)  Max number of profile information that can be
 *                            accomodated within profile_list.
 *      profile_list  - (OUT) List of Elephant profile Ids configured
 *      count         - (OUT) Actual number of elephant profiles
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_elephant_profile_get_all(
                           int unit,
                           int max,
                           bcm_flowtracker_elephant_profile_t *profile_list,
                           int *count)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_elephant_profile_get_all(unit, max,
                                                     profile_list,
                                                     count);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_elephant_profile_attach
 * Purpose:
 *       Attach a flow group with an elephant profile.
 * Parameters:
 *      unit          - (IN) BCM device number
 *      flow_group_id - (IN) Flow group Id
 *      profile_id    - (IN) Elephant profile id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_elephant_profile_attach(
                                   int unit,
                                   bcm_flowtracker_group_t flow_group_id,
                                   bcm_flowtracker_elephant_profile_t profile_id)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_group_elephant_profile_attach(unit,
                                                          flow_group_id,
                                                          profile_id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_elephant_profile_attach_get
 * Purpose:
 *       Get the elephant profile Id attached with a flow group.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      flow_group_id - (IN)  Flow group Id
 *      profile_id    - (OUT) Elephant profile id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_elephant_profile_attach_get(
                                  int unit,
                                  bcm_flowtracker_group_t flow_group_id,
                                  bcm_flowtracker_elephant_profile_t *profile_id)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_group_elephant_profile_attach_get(unit,
                                                              flow_group_id,
                                                              profile_id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_elephant_profile_detach
 * Purpose:
 *       Detach a flow group from an elephant profile.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      flow_group_id - (IN)  Flow group Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_elephant_profile_detach(
                                           int unit,
                                           bcm_flowtracker_group_t flow_group_id)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_group_elephant_profile_detach(unit,
                                                          flow_group_id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}
#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_esw_flowtracker_sync
 * Purpose:
 *      Warmboot scache sync
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_esw_flowtracker_sync(int unit)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        result = _bcm_th_ft_sync(unit);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      _bcm_esw_flowtracker_ucast_cosq_resolve
 * Purpose:
 *       Convert the user passed cosq value to H/w cosq value
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      cosq          - (IN)  Cosq value passed by the user
 *      hw_cosq       - (OUT) Cosq value to be programmed in the H/w
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
_bcm_esw_flowtracker_ucast_cosq_resolve(int unit, uint32 cosq, int *hw_cosq)
{
    int rv = BCM_E_NONE;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(cosq)) {
        if (SOC_IS_TOMAHAWKX(unit)) {
#if defined(BCM_TOMAHAWK_SUPPORT)
            rv = _bcm_th_cosq_index_resolve(unit, cosq, 0,
                                            _BCM_TH_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                            NULL, hw_cosq, NULL);
#endif /* BCM_TOMAHAWK_SUPPORT */
        } else if (SOC_IS_TRIDENT3(unit)) {
#if defined(BCM_TRIDENT3_SUPPORT)
            rv = _bcm_td3_cosq_index_resolve(unit, cosq, 0,
                                             _BCM_TH_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             NULL, hw_cosq, NULL);
#endif /* BCM_TRIDENT3_SUPPORT */
        } else {
            rv = BCM_E_UNAVAIL;
        }
    } else {
        *hw_cosq = cosq;
        rv = BCM_E_NONE;
    }
    return rv;
}

/*
 * Function:
 *      _bcm_esw_flowtracker_mcast_cosq_resolve
 * Purpose:
 *       Convert the user passed cosq value to H/w cosq value
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      cosq          - (IN)  Cosq value passed by the user
 *      hw_cosq       - (OUT) Cosq value to be programmed in the H/w
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
_bcm_esw_flowtracker_mcast_cosq_resolve(int unit, uint32 cosq, int *hw_cosq)
{
    int rv = BCM_E_UNAVAIL;

    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(cosq)) {
        if (SOC_IS_TOMAHAWKX(unit)) {
#if defined(BCM_TOMAHAWK_SUPPORT)
            rv = _bcm_th_cosq_index_resolve(unit, cosq, 0,
                                            _BCM_TH_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            NULL, hw_cosq, NULL);
#endif /* BCM_TOMAHAWK_SUPPORT */
        } else if (SOC_IS_TRIDENT3(unit)) {
#if defined(BCM_TRIDENT3_SUPPORT)
            rv = _bcm_td3_cosq_index_resolve(unit, cosq, 0,
                                             _BCM_TH_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             NULL, hw_cosq, NULL);
#endif /* BCM_TRIDENT3_SUPPORT */
        } else {
            rv = BCM_E_UNAVAIL;
        }
    } else {
        *hw_cosq = cosq;
        rv = BCM_E_NONE;
    }
    return rv;
}

void _bcm_esw_flowtracker_sw_dump(int unit)
{
#if defined(INCLUDE_FLOWTRACKER)
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) ||
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        _bcm_xgs5_flowtracker_sw_dump(unit);
    }
#endif /* INCLUDE_FLOWTRACKER */
}

/*
 * Function:
 *      _bcm_esw_flowtracker_dump_stats_learn
 * Purpose:
 *      Dump the learn statistics
 * Parameters:
 *      unit             - (IN)    BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
void _bcm_esw_flowtracker_dump_stats_learn(int unit)
{
#if defined(INCLUDE_FLOWTRACKER)
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) ||
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        _bcm_xgs5_flowtracker_dump_stats_learn(unit);
    }
#endif /* INCLUDE_FLOWTRACKER */
}

/*
 * Function:
 *      _bcm_esw_flowtracker_dump_stats_export
 * Purpose:
 *      Dump the export statistics
 * Parameters:
 *      unit             - (IN)    BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
void _bcm_esw_flowtracker_dump_stats_export(int unit)
{
#if defined(INCLUDE_FLOWTRACKER)
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) ||
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        _bcm_xgs5_flowtracker_dump_stats_export(unit);
    }
#endif /* INCLUDE_FLOWTRACKER */
}

/*
 * Function:
 *      _bcm_esw_flowtracker_dump_stats_age
 * Purpose:
 *      Dump the age statistics
 * Parameters:
 *      unit             - (IN)    BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
void _bcm_esw_flowtracker_dump_stats_age(int unit)
{
#if defined(INCLUDE_FLOWTRACKER)
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) ||
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        _bcm_xgs5_flowtracker_dump_stats_age(unit);
    }
#endif /* INCLUDE_FLOWTRACKER */
}

/*
 * Function:
 *      _bcm_esw_flowtracker_dump_stats_elephant
 * Purpose:
 *      Dump the elephant statistics
 * Parameters:
 *      unit             - (IN)    BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
void _bcm_esw_flowtracker_dump_stats_elephant(int unit)
{
#if defined(INCLUDE_FLOWTRACKER)
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) ||
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        _bcm_xgs5_flowtracker_dump_stats_elephant(unit);
    }
#endif /* INCLUDE_FLOWTRACKER */
}

void _bcm_esw_qcm_sw_dump(int unit)
{
#if defined(INCLUDE_FLOWTRACKER)
    if ((soc_feature(unit, soc_feature_uc_flowtracker_learn) ||
         soc_feature(unit, soc_feature_uc_flowtracker_export)) ||
         soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
         if (soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0)) {
             _bcm_xgs5_qcm_sw_dump(unit);
         }
    }
#endif /* INCLUDE_FLOWTRACKER */
}

/*
 * Function:
 *      _bcm_esw_qcm_stats_enable
 * Purpose:
 *      Enable statistics update
 * Parameters:
 *      unit             - (IN)    BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
void _bcm_esw_qcm_stats_enable(int unit, uint8 enable)
{
#if defined(INCLUDE_FLOWTRACKER)
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) ||
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        if (soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0)) {
            _bcm_xgs5_qcm_stats_enable(unit, enable);
        }
    }
#endif /* INCLUDE_FLOWTRACKER */
}


/*
 * Function:
 *      _bcm_esw_qcm_dump_stats_learn
 * Purpose:
 *      Dump the learn statistics
 * Parameters:
 *      unit             - (IN)    BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
void _bcm_esw_qcm_dump_stats_learn(int unit)
{
#if defined(INCLUDE_FLOWTRACKER)
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) ||
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        if (soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0)) {
            _bcm_xgs5_qcm_dump_stats_learn(unit);
        }
    }
#endif /* INCLUDE_FLOWTRACKER */
}

/*
 * Function:
 *      _bcm_esw_qcm_dump_stats_export
 * Purpose:
 *      Dump the export statistics
 * Parameters:
 *      unit             - (IN)    BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
void _bcm_esw_qcm_dump_stats_export(int unit)
{
#if defined(INCLUDE_FLOWTRACKER)
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) ||
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        if (soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0)) {
            _bcm_xgs5_qcm_dump_stats_export(unit);
        }
    }
#endif /* INCLUDE_FLOWTRACKER */
}

/*
 * Function:
 *      _bcm_esw_qcm_dump_stats_age
 * Purpose:
 *      Dump the age statistics
 * Parameters:
 *      unit             - (IN)    BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
void _bcm_esw_qcm_dump_stats_age(int unit)
{
#if defined(INCLUDE_FLOWTRACKER)
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) ||
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        if (soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0)) {
            _bcm_xgs5_qcm_dump_stats_age(unit);
        }
    }
#endif /* INCLUDE_FLOWTRACKER */
}

/*
 *
 * Function:
 *      _bcm_esw_qcm_dump_stats_scan
 * Purpose:
 *      Dump the scan statistics
 * Parameters:
 *      unit             - (IN)    BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
void _bcm_esw_qcm_dump_stats_scan(int unit)
{
#if defined(INCLUDE_FLOWTRACKER)
   if (soc_feature(unit, soc_feature_uc_flowtracker_learn) ||
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
       if (soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0)) {
            _bcm_xgs5_qcm_dump_stats_scan(unit);
        }
    }
#endif /* INCLUDE_FLOWTRACKER */
}

/*
 * Function:
 *   bcm_esw_flowtracker_chip_debug_info_set
 * Purpose:
 *   Set chip debug information.
 * Parameters:
 *   unit             - (IN)    BCM device number
 *   num_debug_info   - (OUT) actunal number of parameters
 *   info             - (OUT) memory to send parameters.
 * Returns:
 *   BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_chip_debug_info_set(
        int unit,
        int num_debug_info,
        bcm_flowtracker_chip_debug_info_t *info)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_chip_debug_info_set(unit, num_debug_info, info);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* BCM_FLOWTRACKER_SUPPORT */

    return result;
}


/*
 * Function:
 *   bcm_esw_flowtracker_chip_debug_info_get
 * Purpose:
 *   Get chip debug information.
 * Parameters:
 *   unit             - (IN)    BCM device number
 *   max_debug_info   - (INOUT) maximum number that user wants.
 *   info             - (OUT) memory to send parameters.
 *   num_debug_info   - (OUT) actunal number of parameters
 * Returns:
 *   BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_chip_debug_info_get(
        int unit,
        int max_debug_info,
        bcm_flowtracker_chip_debug_info_t *info,
        int *num_debug_info)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_chip_debug_info_get(unit, max_debug_info,
            info, num_debug_info);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* BCM_FLOWTRACKER_SUPPORT */

    return result;
}

/*
 * Function:
 *  bcm_esw_flowtracker_drop_reason_group_create
 * Purpose:
 *  Create groupid for provided bitmap of drop reasons.
 * Parameters:
 *  unit         - (IN) BCM device number
 *  drop_reasons - (IN) Drop reason Bitmap
 *  id           - (OUT) Drop reason group id
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_drop_reason_group_create(int unit,
                                    uint32 flags,
                                    bcm_flowtracker_drop_reasons_t drop_reasons,
                                    bcm_flowtracker_drop_reason_group_t *id)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT) & defined(BCM_FLOWTRACKER_V2_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2) &&
       (soc_feature(unit, soc_feature_flowtracker_ver_2_drop_code))) {
        result =
            bcmi_ftv2_drop_reason_group_create(unit, flags, drop_reasons, id);
    }

    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}

/*
 * Function:
 *  bcm_esw_flowtracker_drop_reason_group_delete
 * Purpose:
 *  Delete drop reason group id.
 * Parameters:
 *  unit  - (IN) BCM device number
 *  id    - (IN) Drop Reason Group Id
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_drop_reason_group_delete(int unit,
                                         bcm_flowtracker_drop_reason_group_t id)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT) & defined(BCM_FLOWTRACKER_V2_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2) &&
       (soc_feature(unit, soc_feature_flowtracker_ver_2_drop_code))) {
        result = bcmi_ftv2_drop_reason_group_delete(unit, id);
    }

    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}

/*
 * Function:
 *  bcm_esw_flowtracker_drop_reason_group_get
 * Purpose:
 *  Get drop reason bitmap from provided group id.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  id            - (IN) Drop Reason Group Id
 *  drop_reasons  - (OUT) Drop Reason Bitmap
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_drop_reason_group_get(int unit,
                                   bcm_flowtracker_drop_reason_group_t id,
                                   bcm_flowtracker_drop_reasons_t *drop_reasons)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT) & defined(BCM_FLOWTRACKER_V2_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2) &&
       (soc_feature(unit, soc_feature_flowtracker_ver_2_drop_code))) {
        result = bcmi_ftv2_drop_reason_group_get(unit, id, drop_reasons);
    }

    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif

    return result;
}

/*
 * Function:
 * bcm_esw_flowtracker_aggregate_class_create
 * Purpose:
 *  Create Flowtracker aggregate class.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  options       - (IN) Options
 *  group_type    - (IN) Flowtracker Group Type
 *  aggregate_class_info - (IN) Aggregate class info.
 *  aggregate_class - (IN/OUT) Aggregate Class.
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_aggregate_class_create(
    int unit,
    uint32 options,
    bcm_flowtracker_group_type_t group_type,
    bcm_flowtracker_aggregate_class_info_t *aggregate_class_info,
    uint32 *aggregate_class)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

        result = bcmi_ftv3_aggregate_class_create(unit, options,
                group_type, aggregate_class_info, aggregate_class);

        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    }
#endif

    return result;
}

/*
 * Function:
 * bcm_esw_flowtracker_aggregate_class_get
 * Purpose:
 *  Get Flowtracker Aggregate class info.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  aggregate_class - (IN) Aggregate Class.
 *  group_type    - (IN) Flowtracker Group Type
 *  aggregate_class_info - (OUT) Aggregate class info.
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_aggregate_class_get(
    int unit,
    uint32 aggregate_class,
    bcm_flowtracker_group_type_t group_type,
    bcm_flowtracker_aggregate_class_info_t *aggregate_class_info)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

        result = bcmi_ftv3_aggregate_class_get(unit, aggregate_class,
                group_type, aggregate_class_info);

        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    }
#endif

    return result;
}

/*
 * Function:
 * bcm_esw_flowtracker_aggregate_class_get_all
 * Purpose:
 *  Create Flowtracker aggregate class.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  group_type    - (IN) Flowtracker Group Type
 *  size          - (IN) Size of aggregate_class_array
 *  aggregate_class_array - (OUT) To be populated with
 *                       aggregate classes configured in
 *                       the system.
 *  actual_size   - (OUT) Actual number of aggregate
 *                       classes.
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_aggregate_class_get_all(
    int unit,
    bcm_flowtracker_group_type_t group_type,
    int size,
    uint32 *aggregate_class_array,
    int *actual_size)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

        result = bcmi_ftv3_aggregate_class_get_all(unit,
                group_type, size, aggregate_class_array, actual_size);

        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    }
#endif

    return result;
}

/*
 * Function:
 *  bcm_esw_flowtracker_aggregate_class_destroy
 * Purpose:
 *  Get Flowtracker Aggregate class info.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  aggregate_class - (IN) Aggregate Class.
 *  group_type    - (IN) Flowtracker Group Type
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_aggregate_class_destroy(
    int unit,
    uint32 aggregate_class,
    bcm_flowtracker_group_type_t group_type)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

        result = bcmi_ftv3_aggregate_class_destroy(unit,
                aggregate_class, group_type);

        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    }
#endif

    return result;
}

/*
 * Function:
 *  bcm_esw_flowtracker_periodic_export_config_set
 * Purpose:
 *  Set Periodic export configurations.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  ft_type_flags - (IN) Flowtracker type Flags.
 *  config        - (IN) Periodic export config.
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_periodic_export_config_set(
    int unit,
    uint32 ft_type_flags,
    bcm_flowtracker_periodic_export_config_t *config)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

        result = bcmi_ft_periodic_export_config_set(unit,
                        ft_type_flags, config);

        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    }
#endif

    return result;
}

/*
 * Function:
 *  bcm_esw_flowtracker_periodic_export_config_get
 * Purpose:
 *  Get Periodic export configurations.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  ft_type_flags - (IN) Flowtracker type Flags.
 *  config        - (OUT) Periodic export config.
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_periodic_export_config_get(
    int unit,
    uint32 ft_type_flags,
    bcm_flowtracker_periodic_export_config_t *config)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

        result = bcmi_ft_periodic_export_config_get(unit,
                        ft_type_flags, config);

        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    }
#endif

    return result;
}

/*
 * Function:
 *  bcm_esw_flowtracker_control_set
 * Purpose:
 *  Set global flowtracker controls.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  type          - (IN) Flowtracker control type
 *  arg           - (IN) Argument
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_control_set(
    int unit,
    bcm_flowtracker_control_type_t type,
    int arg)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

       result = bcmi_ft_control_set(unit, type, arg);

        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    }
#endif

    return result;
}

/*
 * Function:
 *  bcm_esw_flowtracker_control_get
 * Purpose:
 *  Get global flowtracker controls.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  type          - (IN) Flowtracker control type
 *  arg           - (OUT) Argument
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_control_get(
    int unit,
    bcm_flowtracker_control_type_t type,
    int *arg)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

       result = bcmi_ft_control_get(unit, type, arg);

        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    }
#endif

    return result;
}

/*
 * Function:
 *  bcm_esw_flowtracker_egress_class_source_set
 * Purpose:
 *  Set default egress class
 * Parameters:
 *  unit          - (IN) BCM device number
 *  options       - (IN) Options
 *  source_type   - (IN) Class source
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_egress_class_source_set(
    int unit,
    uint32 options,
    bcm_flowtracker_class_source_type_t source_type)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

        result = bcmi_ftv2_egr_class_source_set(unit,
                        options, source_type);

        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    }
#endif

    return result;
}

/*
 * Function:
 *  bcm_esw_flowtracker_egress_class_source_get
 * Purpose:
 *  Set default egress class
 * Parameters:
 *  unit          - (IN) BCM device number
 *  options       - (IN) Options
 *  source_type   - (OUT) Class source
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_egress_class_source_get(
    int unit,
    uint32 options,
    bcm_flowtracker_class_source_type_t *source_type)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

        result = bcmi_ftv2_egr_class_source_get(unit,
                        options, source_type);

        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    }
#endif

    return result;
}

/*
 * Function:
 *  bcm_esw_flowtracker_group_record_traverse
 * Purpose:
 *  Traverse flowtracker group records
 * Parameters:
 *  unit          - (IN) BCM device number
 *  group_type    - (IN) Flowtracker Group Id
 *  type          - (IN) Group record content type
 *  cb            - (IN) Callback function
 *  user_data     - (IN) User data
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_record_traverse(
    int unit,
    bcm_flowtracker_group_t group_id,
    bcm_flowtracker_group_record_type_t type,
    bcm_flowtracker_group_record_traverse_cb cb,
    void *user_data)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

        result = bcmi_ft_group_record_traverse(unit,
                group_id, type, cb, user_data);

        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    }
#endif

    return result;
}

/*
 * Function:
 *  bcm_esw_flowtracker_export_record_data_info_get
 * Purpose:
 *  Get the decoded data for the export record
 * Parameters:
 *  unit          - (IN) BCM device number
 *  record        - (IN) Export Record
 *  max_elements  - (IN) Size of array of export_element_info and data_info
 *  export_element_info - (IN) Export Elements used to decode record
 *  data_info     - (OUT) decoded Data info per export element
 *  count         - (OUT) Number of valid data_info
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_export_record_data_info_get(
    int unit,
    bcm_flowtracker_export_record_t *record,
    int max_elements,
    bcm_flowtracker_export_element_info_t *export_element_info,
    bcm_flowtracker_export_record_data_info_t *data_info,
    int *count)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

        result = bcmi_ft_export_record_data_info_get(unit,
                record, max_elements, export_element_info,
                data_info, count);

        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    }
#endif

    return result;
}

/*
 * Function:
 *  bcm_esw_flowtracker_udf_tracking_param_set
 * Purpose:
 *  To set Flowtracker Tracking parameter UDF information.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  flags         - (IN) Flags
 *  param         - (IN) Flowtracker Ttrackng param
 *  custom_id     - (IN) Custom Identifier
 *  udf_info      - (IN) Udf object information
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_udf_tracking_param_set(
    int unit,
    uint32 flags,
    bcm_flowtracker_tracking_param_type_t param,
    uint16 custom_id,
    bcm_flowtracker_udf_info_t *udf_info)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

        result = bcmi_ft_udf_tracking_param_set(unit,
                flags, param, custom_id, udf_info);

        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    }
#endif

    return result;
}

/*
 * Function:
 *  bcm_esw_flowtracker_udf_tracking_param_get
 * Purpose:
 *  To get Flowtracker Tracking param UDF information.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  flags         - (IN) Flags
 *  param         - (IN) Flowtracker Ttrackng param
 *  custom_id     - (IN) Custom Identifier
 *  udf_info      - (OUT) Udf object information
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_udf_tracking_param_get(
    int unit,
    uint32 flags,
    bcm_flowtracker_tracking_param_type_t param,
    uint16 custom_id,
    bcm_flowtracker_udf_info_t *udf_info)
{
    int result = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));

        result = bcmi_ft_udf_tracking_param_get(unit,
                flags, param, custom_id, udf_info);

        BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
    }
#endif

    return result;
}

#if defined(BCM_FLOWTRACKER_SUPPORT)
/*
 * Function:
 *     bcmi_ft_group_hw_install
 * Purpose:
 *     Install hardware resource for the group.
 * Description:
 *     Install flowtracker group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_ft_group_hw_install(
    int unit,
    bcm_flowtracker_group_t id)
{
    int result = BCM_E_UNAVAIL;

    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_hw_install(unit, id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}

/*
 * Function:
 *     bcmi_ft_group_hw_uninstall
 * Purpose:
 *     Uninstall hardware resource for the group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_ft_group_hw_uninstall(
    int unit,
    bcm_flowtracker_group_t id)

{
    int result = BCM_E_UNAVAIL;

    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_hw_uninstall(unit, id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}

/*
 * Function:
 *     bcmi_esw_ft_group_extraction_mode_set
 * Purpose:
 *     Set extraction mode.
 * Parameters:
 *     unit - (IN) BCM device id
 *     key_or_data - (IN) key/data
 *     id   - (IN) FT group id.
 *     mode - (IN) Mode of the extration key/data.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_ft_group_extraction_mode_set(
              int unit,
              int key_or_data,
              bcm_flowtracker_group_t id,
              bcmi_ft_group_key_data_mode_t mode)
{
    int result = BCM_E_UNAVAIL;

    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_extraction_mode_set(unit, key_or_data, id, mode);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}

/*
 * Function:
 *     bcmi_esw_ft_group_extraction_mode_get
 * Purpose:
 *     Get extraction mode.
 * Parameters:
 *     unit - (IN) BCM device id
 *     key_or_data - (IN) key/data
 *     id   - (IN) FT group id.
 *     mode - (OUT) Mode of the extration key/data.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_ft_group_extraction_mode_get(
              int unit,
              int key_or_data,
              bcm_flowtracker_group_t id,
              bcmi_ft_group_key_data_mode_t *mode)
{
    int result = BCM_E_UNAVAIL;

    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_extraction_mode_get(unit, key_or_data, id, mode);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}

/*
 * Function:
 *     bcmi_esw_ft_group_extraction_hw_info_set
 * Purpose:
 *     Set hardware extraction info in group sw state.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     key_or_data - (IN) key/data
 *     num_elements - (IN) Number of elements of hw extraction.
 *     elements - (IN) list of hw extrator info.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_ft_group_extraction_hw_info_set(
              int unit,
              bcm_flowtracker_group_t id,
              int key_or_data,
              int num_elements,
              bcmi_ft_hw_extractor_info_t *elements)
{
    int result = BCM_E_UNAVAIL;

    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_extraction_hw_info_set(unit, id, key_or_data,
                                                      num_elements, elements);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}


/*
 * Function:
 *     bcmi_esw_ft_group_extraction_hw_info_get
 * Purpose:
 *     Get hardware extraction info in group sw state.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     key_or_data - (IN) key/data
 *     max_elements - (IN) Max elements that user asked for.
 *     elements - (OUT) list of hw extrator info.
 *     num_elements - (OUT) Number of elements of hw extraction.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_ft_group_extraction_hw_info_get(
              int unit,
              int key_or_data,
              bcm_flowtracker_group_t id,
              int max_elements,
              bcmi_ft_hw_extractor_info_t *elements,
              int *num_elements)
{
    int result = BCM_E_UNAVAIL;

    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_extraction_hw_info_get(unit, key_or_data, id,
                                                      max_elements, elements,
                                                      num_elements);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}


/*
 * Function:
 *     bcmi_esw_ft_group_extraction_alu_info_set
 * Purpose:
 *     Set Alu extraction info in group sw state.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     num_alu_info - (IN) Number of ALU/lod info structure.
 *     alu_info  - (IN) list if ALU/load information.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_ft_group_extraction_alu_info_set(
              int unit,
              bcm_flowtracker_group_t id,
              int key_or_data,
              int num_alu_info,
              bcmi_ft_group_alu_info_t *alu_info)
{
    int result = BCM_E_UNAVAIL;

    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_extraction_alu_info_set(unit, id, key_or_data,
                                                      num_alu_info, alu_info);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}

/*
 * Function:
 *     bcmi_esw_ft_group_extraction_alu_info_get
 * Purpose:
 *     Get Alu extraction info of group sw state.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     max_alu_info - (IN) max information user asked for.
 *     alu_info  - (OUT) list if ALU/load information.
 *     num_alu_info - (OUT) Number of ALU/lod info structure.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_ft_group_extraction_alu_info_get(
              int unit,
              bcm_flowtracker_group_t id,
              int key_or_data,
              int max_alu_info,
              bcmi_ft_group_alu_info_t *alu_info,
              int *num_alu_info)
{
    int result = BCM_E_UNAVAIL;

    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_extraction_alu_info_get(unit, id, key_or_data,
                                                      max_alu_info, alu_info,
                                                      num_alu_info);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}

/*
 * Function:
 *     bcmi_esw_ft_group_param_update
 * Purpose:
 *     Update group state parameters.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     param - (IN) Group's parameters.
 *     opr   - (IN) operation to be performed on data.
 *     arg  - (IN) argument value.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_ft_group_param_update(int unit,
                           bcm_flowtracker_group_t id,
                           bcmi_ft_group_params_t param,
                           bcmi_ft_group_params_operation_t opr,
                           int arg)
{
    int result = BCM_E_UNAVAIL;

    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_param_update(unit, id, param, opr, arg);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}

/*
 * Function:
 *     bcmi_esw_ft_group_param_retrieve
 * Purpose:
 *     Get group state parameters.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     param - (IN) Group's parameters.
 *     arg  - (OUT) argument value.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_ft_group_param_retrieve(int unit,
                        bcm_flowtracker_group_t id,
                        bcmi_ft_group_params_t param,
                        int *arg)

{
    int result = BCM_E_UNAVAIL;

    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_param_retrieve(unit, id, param, arg);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}

/*
 * Function:
 *     bcmi_esw_ft_group_entry_add_check
 * Purpose:
 *     Check Flowtracker Group before adding it to FTFP Entry.
 * Parameters:
 *     unit       - (IN) BCM device id
 *     group_type - (IN) Flowtracker Group Type
 *     id         - (IN) FT group id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_ft_group_entry_add_check(int unit,
                        bcm_flowtracker_group_type_t group_type,
                        bcm_flowtracker_group_t id)
{
    int result = BCM_E_UNAVAIL;

    /* Take Lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_entry_add_check(unit,
                group_type, id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}

/*
 * Function:
 *   bcmi_esw_ft_check_action_num_operands_get
 * Purpose:
 *   Get number of operands required for given check action.
 * Parameters:
 *   unit     - (IN) BCM device id
 *   action   - (IN) flowcheck action
 *   num_operands - (OUT) Number of operands required
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_esw_ft_check_action_num_operands_get(int unit,
    bcm_flowtracker_check_action_t action,
    int *num_operands)
{
     int result = BCM_E_UNAVAIL;

    /* Take Lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_check_action_num_operands_get(unit,
                    action, num_operands);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}

/*
 * Function:
 *  bcmi_esw_ft_group_tracking_param_mask_status_get
 * Purpose:
 *   Get mask status of tracking param key mask.
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   tp_info           - (IN) Tracking param info.
 *   qual_size         - (IN) Size of corresponding qualifier.
 *   mask_present      - (OUT) Is mask present
 *   partial_mask      - (OUT) Mask position where mask is not 0xFFFF.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_esw_ft_group_tracking_param_mask_status_get(int unit,
    bcm_flowtracker_tracking_param_info_t *tp_info,
    uint16 qual_size,
    int *mask_present,
    int *partial_mask)
{
    int result = BCM_E_UNAVAIL;

    /* Take Lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_tracking_param_mask_status_get(unit,
                tp_info, qual_size, mask_present, partial_mask);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}

/*
 * Function:
 *  bcmi_esw_ft_type_support_check
 * Purpose:
 *   Check the support for ft type
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   ft_type           - (IN) FT Type
 *   support           - (OUT) Supported
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_esw_ft_type_support_check(int unit,
    bcmi_ft_type_t ft_type,
    int *support)
{
    int result = BCM_E_UNAVAIL;

    /* Take Lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_type_support_check(unit, ft_type, support);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}

/*
 * Function:
 *   bcmi_esw_ft_group_state_transition_type_get
 * Purpose:
 *   Get group state transition  type.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 *   st_type - (OUT) State transition Type.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_esw_ft_group_state_transition_type_get(int unit,
                 bcm_flowtracker_group_t id,
                 bcm_flowtracker_state_transition_type_t *st_type)
{
    int result = BCM_E_UNAVAIL;

    /* Take Lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        result = bcmi_ft_group_state_transition_type_get(unit, id, st_type);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
/*
 * Function:
 *     bcmi_esw_ftv3_aggregate_class_entry_add_check
 * Purpose:
 *     Check Aggregate Class before adding it to FTFP Entry.
 * Parameters:
 *     unit            - (IN) BCM device id
 *     aggregate_class - (IN) Aggregate Class Id
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_ftv3_aggregate_class_entry_add_check(
    int unit,
    uint32 aggregate_class)
{
    int result = BCM_E_UNAVAIL;

    /* Take Lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        result = bcmi_ftv3_aggregate_class_entry_add_check(unit,
                aggregate_class);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}

/*
 * Function:
 *     bcmi_esw_ftv3_user_entry_entry_add_check
 * Purpose:
 *     Check User entry before adding it to FTFP Entry.
 * Parameters:
 *     unit       - (IN) BCM device id
 *     group_type - (IN) Flowtracker Group Type.
 *     id         - (IN) FT group id.
 *     flow_index - (IN) Flowtracker Index for User entry.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_ftv3_user_entry_entry_add_check(
    int unit,
    bcm_flowtracker_group_type_t group_type,
    bcm_flowtracker_group_t id,
    int flow_index)
{
    int result = BCM_E_UNAVAIL;

    /* Take Lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        result = bcmi_ftv3_user_entry_entry_add_check(unit,
                group_type, id, flow_index);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}
/*
 * Function:
 *     bcmi_esw_ftv3_user_entry_ref_update
 * Purpose:
 *     Update reference count for User entry.
 * Parameters:
 *     unit       - (IN) BCM device id
 *     group_type - (IN) FT group type
 *     flow_index - (IN) Flowtracker Index for User entry.
 *     count      - (IN) Change in reference count.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_ftv3_user_entry_ref_update(
    int unit,
    bcm_flowtracker_group_type_t group_type,
    int flow_index,
    int count)
{
    int result = BCM_E_UNAVAIL;

    /* Take Lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        result = bcmi_ftv3_user_entry_ref_update(unit,
                group_type, flow_index, count);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}

/*
 * Function:
 *     bcmi_esw_ftv3_aggregate_class_ref_update
 * Purpose:
 *     Update reference count for Aggregate class.
 * Parameters:
 *     unit            - (IN) BCM device id
 *     aggregate_class - (IN) Aggregate Class Id.
 *     count           - (IN) Change in reference count.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_ftv3_aggregate_class_ref_update(
    int unit,
    uint32 aggregate_class,
    int count)
{
    int result = BCM_E_UNAVAIL;

    /* Take Lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        result = bcmi_ftv3_aggregate_class_ref_update(unit,
                aggregate_class, count);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}

#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

/*
 * Function:
 *   bcmi_esw_ft_group_checks_bitmap_get
 * Purpose:
 *   Get flowtracker check in ALU16 bitmap.
 * Parameters:
 *   unit       - (IN) BCM device number.
 *  id         - (IN) Flowtracker Group Id.
 *  num_checks - (IN) Number of flowchecks.
 *  check_list - (IN) List of Flowchecks
 *  check_bmp_data - (OUT) Data bitmap
 *  check_bmp_mask - (OUT) Mask bitmap.
 * Returns:
 *    BCM_E_XXX
 */
extern int
bcmi_esw_ft_group_checks_bitmap_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 num_checks,
        bcm_field_flowtrackercheck_t *check_list,
        uint32 *check_bmp_data,
        uint32 *check_bmp_mask)
{
    int result = BCM_E_UNAVAIL;

    /* Take Lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {

        result = bcmi_ft_group_checks_bitmap_get(unit,
                id, num_checks, check_list, check_bmp_data,
                check_bmp_mask);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}

/*
 * Function:
 *   bcmi_esw_ft_group_checks_list_get
 * Purpose:
 *   Get list of flowtracker check from ALU16 bitmap.
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   id         - (IN) Flowtracker Group Id.
 *   check_bmp_data - (IN) Data bitmap.
 *   check_bmp_mask - (IN) Mask bitmap.
 *   num_checks - (IN) Number of flowchecks.
 *   check_list - (OUT) List of Flowchecks.
 *   actual_num_checks - (OUT) actual number of flowchecks.
 * Returns:
 *   BCM_E_XXX
 */
extern int
bcmi_esw_ft_group_checks_list_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 check_bmp_data,
        uint32 check_bmp_mask,
        uint32 num_checks,
        bcm_field_flowtrackercheck_t *check_list,
        uint32 *actual_num_checks)
{
    int result = BCM_E_UNAVAIL;

    /* Take Lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {

        result = bcmi_ft_group_checks_list_get(unit,
                id, check_bmp_data, check_bmp_mask, num_checks,
                check_list, actual_num_checks);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));

    return result;
}

#endif /* BCM_FLOWTRACKER_SUPPORT */

