/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        ft_group.c
 * Purpose:     The purpose of this file is to set flow tracker group methods.
 * Requires:
 */

#include <bcm_int/esw/flowtracker/ft_group.h>

#if defined(BCM_FLOWTRACKER_SUPPORT)

/* External Common Group software State. */
extern bcmi_ft_group_sw_info_t **bcmi_ft_group_sw_state[BCM_MAX_NUM_UNITS];

bcmi_ft_flowchecker_sw_state_t bcmi_ft_flowchecker_state[BCM_MAX_NUM_UNITS];

/*
 * Function:
 *     bcmi_ft_flowchecker_get_free_index
 * Purpose:
 *     Get gree index of the flowtracker check element.
 * Description:
 *     At present the checks are numbered equal to group
 *     number but there is a provision to increase it to any
 *     number by adding them to the linked list.
 *     But it may not be required as checks can be shared
 *     among various groups and therefore we may not need
 *     exlicit check for each group.
 * Parameters:
 *     unit - (IN) BCM device id
 *
 * Returns:
 *     iBCM_E_XXX.
 */
int
bcmi_ft_flowchecker_get_free_index(int unit, int *index)
{

    int num_flowcheckers = 0;
    soc_mem_t mem;
    int i = 0;

    /* assign the memory. */
    mem = BSC_KG_GROUP_TABLEm;

    /* Total number of groups in device. */
    num_flowcheckers = soc_mem_index_max(unit, mem);

    if (*index != -1) {
        i = *index;
        num_flowcheckers = *index;
    }

    for (; i<=num_flowcheckers; i++) {
        if (!(SHR_BITGET(BCMI_FT_FLOWCHECKER_BITMAP(unit), i))) {
            /* Got the free index. use it. */
            SHR_BITSET(BCMI_FT_FLOWCHECKER_BITMAP(unit), i);
            *index = i;
            return BCM_E_NONE;
        }
    }

    return BCM_E_RESOURCE;
}

/*
 * Function:
 *    bcmi_ft_flowchecker_init
 * Purpose:
 *    Initialize software state of flowtracker checks.
 * Parameters:
 *     unit - (IN) BCM device id
 *
 * Returns:
 *     BCM_E_XXX.
 */

int bcmi_ft_flowchecker_init(int unit)
{

    int num_flowcheckers = 0;
    soc_mem_t mem;
    mem = BSC_KG_GROUP_TABLEm;

    /* Total number of groups in device. */
    num_flowcheckers = soc_mem_index_count(unit, mem);

    /* Allocate space for the flowchecker database structures. */
    BCMI_FT_FLOWCHECKER_STATE(unit) = (bcmi_flowtracker_flowchecker_info_t **)
        sal_alloc((num_flowcheckers *
            sizeof(bcmi_flowtracker_flowchecker_info_t *)),
        "flowtracker group sw state");

    if (BCMI_FT_FLOWCHECKER_STATE(unit) == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(BCMI_FT_FLOWCHECKER_STATE(unit), 0,
        (num_flowcheckers * sizeof(bcmi_ft_flowchecker_sw_state_t *)));

    /* allocate Group bitmap for whole chip. */
    BCMI_FT_FLOWCHECKER_BITMAP(unit) =
        sal_alloc(SHR_BITALLOCSIZE(num_flowcheckers),
        "ft_flowchecker_bitmap");

    if (BCMI_FT_FLOWCHECKER_BITMAP(unit) == NULL) {
        return BCM_E_MEMORY;
    }

    sal_memset(BCMI_FT_FLOWCHECKER_BITMAP(unit), 0,
               SHR_BITALLOCSIZE(num_flowcheckers));

    /* Allocate memory for refcounting the flowchecker indexes. */
    bcmi_ft_flowchecker_state[unit].fc_idx_refcount = (uint32 *)
        sal_alloc((num_flowcheckers * sizeof(uint32)),
        "flowchecker refcount state");

    if (bcmi_ft_flowchecker_state[unit].fc_idx_refcount == NULL) {
        return BCM_E_MEMORY;
    }

    sal_memset(bcmi_ft_flowchecker_state[unit].fc_idx_refcount, 0,
        (num_flowcheckers * sizeof(uint32)));

    /* Reserve the 0th index. */
/*    SHR_BITSET(BCMI_FT_FLOWCHECKER_BITMAP(unit), 0);*/

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_ft_flowchecker_cleanup
 * Purpose:
 *    Cleanup software state of flowtracker checks.
 * Parameters:
 *     unit - (IN) BCM device id
 *
 * Returns:
 *     None.
 */

void
bcmi_ft_flowchecker_cleanup(int unit)
{

    if (bcmi_ft_flowchecker_state[unit].fc_idx_refcount != NULL) {
        sal_free(bcmi_ft_flowchecker_state[unit].fc_idx_refcount);
        bcmi_ft_flowchecker_state[unit].fc_idx_refcount = NULL;
    }

    if (BCMI_FT_FLOWCHECKER_BITMAP(unit) != NULL) {
        sal_free(BCMI_FT_FLOWCHECKER_BITMAP(unit));
        BCMI_FT_FLOWCHECKER_BITMAP(unit) = NULL;
    }

    if (BCMI_FT_FLOWCHECKER_STATE(unit) != NULL) {
        sal_free(BCMI_FT_FLOWCHECKER_STATE(unit));
        BCMI_FT_FLOWCHECKER_STATE(unit) = NULL;
    }

}

/*
 * Function:
 *    bcmi_ft_flowchecker_is_invalid
 * Purpose:
 *    Chekc if the flowtracker check index is valid
 * Parameters:
 *     unit - (IN) BCM device id
 *     flowchecker_id - (IN) flowchecker index.
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_flowchecker_is_invalid(int unit, int flowchecker_id)
{

    int num_flowcheckers = 0;

    /* Total number of groups/flowcheckers in device. */
    num_flowcheckers = soc_mem_index_max(unit, BSC_KG_GROUP_TABLEm);

    if (flowchecker_id > num_flowcheckers) {
        return BCM_E_PARAM;
    }

    if (BCMI_FT_FLOWCHECKER_BITMAP(unit) == NULL) {
        return BCM_E_INIT;
    }

    if (!(SHR_BITGET(BCMI_FT_FLOWCHECKER_BITMAP(unit), flowchecker_id))) {
        return BCM_E_NOT_FOUND;
    }

    if (BCMI_FT_FLOWCHECKER_INFO(unit, flowchecker_id) == NULL) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

int
bcmi_ft_check_param_sanity(
            int unit,
            bcm_flowtracker_check_info_t flow_check_info)
{
    uint32 max_value = 0, min_value = 0;
    const char *plist_str[] = BCM_FT_TRACKING_PARAM_STRINGS;

    switch(flow_check_info.param) {
        /* Tracking Params which are not mapped to qualifier. */
        case bcmFlowtrackerTrackingParamTypeNone:
        case bcmFlowtrackerTrackingParamTypePktCount:
        case bcmFlowtrackerTrackingParamTypeByteCount:
        case bcmFlowtrackerTrackingParamTypeTimestampNewLearn:
        case bcmFlowtrackerTrackingParamTypeTimestampFlowStart:
        case bcmFlowtrackerTrackingParamTypeTimestampFlowEnd:
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1:
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2:
        case bcmFlowtrackerTrackingParamTypeCount:
            return BCM_E_PARAM;

        /* Tracking params too big for flow check. */
        case bcmFlowtrackerTrackingParamTypeSrcIPv6:
        case bcmFlowtrackerTrackingParamTypeDstIPv6:
        case bcmFlowtrackerTrackingParamTypeInnerSrcIPv6:
        case bcmFlowtrackerTrackingParamTypeInnerDstIPv6:
            return BCM_E_PARAM;

        case bcmFlowtrackerTrackingParamTypeDosAttack:
        case bcmFlowtrackerTrackingParamTypeInnerDosAttack:
            if (!soc_feature(unit,
                        soc_feature_flowtracker_ver_1_dos_attack_check)) {

                LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
                        (BSL_META_U(unit, "Tracking Param(%s) is not supported in"
                                    " flowcheck.\n"),
                         plist_str[flow_check_info.param]));
                return BCM_E_UNAVAIL;
            }
            break;

        default:
            /* Other tracking params are valid for Flowcheck */
            break;
    }

    /* Tracking params which are mapped to qualifier. */
    BCM_IF_ERROR_RETURN(_bcm_field_ft_tracking_param_range_get(unit,
                   flow_check_info.param, &min_value, &max_value));

    if (flow_check_info.min_value < min_value) {
        return BCM_E_PARAM;
    }

    if (flow_check_info.max_value > max_value) {
        return BCM_E_PARAM;
    }

    if ((flow_check_info.min_value != 0) &&
        (flow_check_info.max_value != 0)) {

        if (flow_check_info.min_value > flow_check_info.max_value) {
            return BCM_E_PARAM;
        }
    }

    if (!flow_check_info.operation) {
        /* No operation is sent. return config error. */
        return BCM_E_CONFIG;
    }


    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_ft_check_create
 * Purpose:
 *    Create flowtracker index from information
 *    provided by user.
 * Parameters:
 *     unit - (IN) BCM device id
 *     options - (IN) options to create flowtracker check.
 *     flow_check_info - (IN) information to create flowtracker check.
 *     flowchecker_id - (OUT) Placeholder for index generated.
 *
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_check_create(
            int unit, uint32 options,
            bcm_flowtracker_check_info_t flow_check_info,
            bcm_flowtracker_check_t *flowchecker_id)
{

    int index = -1;
    bcmi_flowtracker_flowchecker_info_t fc_info;

    /* Check if input params are correct. */
    if (flowchecker_id == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(bcmi_ft_check_param_sanity(unit, flow_check_info));

    if ((options & BCM_FLOWTRACKER_CHECK_SECOND) &&
        (options & BCM_FLOWTRACKER_CHECK_WITH_ID)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "Secondary check is not allowed with BCM_FLOWTRACKER_CHECK_WITH_ID. \n")));
        return BCM_E_CONFIG;
    }

    if (options & BCM_FLOWTRACKER_CHECK_WITH_ID) {

        if (!BCMI_FT_IDX_IS_CHECK(*flowchecker_id)) {
            return BCM_E_PARAM;
        }

        /* get the index from bitmap. */
        index = BCMI_FT_FLOWCHECKER_INDEX_GET(*flowchecker_id);

        if (!(bcmi_ft_flowchecker_is_invalid(unit, index))) {
            return BCM_E_PARAM;
        }
    }

    if (options & BCM_FLOWTRACKER_CHECK_SECOND) {
        if (!BCMI_FT_IDX_IS_CHECK(flow_check_info.primary_check_id)) {
            return BCM_E_PARAM;
        }

        sal_memset(&fc_info, 0, sizeof(bcmi_flowtracker_flowchecker_info_t));

        BCM_IF_ERROR_RETURN(bcmi_ft_flowchecker_get
            (unit, flow_check_info.primary_check_id, &fc_info));

        if (bcmi_ft_flowchecker_opr_is_dual(unit, fc_info.check1.operation)) {
            /* Dual operations used in first check.*/
            return BCM_E_CONFIG;
        }

        if ((flow_check_info.flags &
            BCM_FLOWTRACKER_CHECK_DATA_CLEAR_ON_EXPORT) || (flow_check_info.flags &
            BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD)) {

            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                "Export configuration now not allowed on Secondary Check.\n")));
            return BCM_E_CONFIG;
        }

        if (fc_info.check2.operation) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                "Secondary Check already exists for this primary. \n")));
            return BCM_E_EXISTS;
        }

        if (soc_feature(unit, soc_feature_hx5_ft_32bit_param_update)) {
            /* Not allowed to create a second check if dos attack is updated.*/
            if (((fc_info.check1.param ==
                 bcmFlowtrackerTrackingParamTypeDosAttack) ||
                (fc_info.check1.param ==
                 bcmFlowtrackerTrackingParamTypeInnerDosAttack)) &&
                    (fc_info.action_info.action ==
                     bcmFlowtrackerCheckActionUpdateValue)) {

                return BCM_E_CONFIG;
            }
        }

        index = BCMI_FT_FLOWCHECKER_INDEX_GET(flow_check_info.primary_check_id);

        if ((bcmi_ft_flowchecker_is_invalid(unit, index))) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                "The primary check is not created or invalid.\n")));

            return BCM_E_PARAM;
        }

        /* If Primary is associated with any FT Group, we should not allow
         * added second to it. */
        if (BCMI_FT_FLOWCHECKER_REFCOUNT(unit, index)) {
            return BCM_E_CONFIG;
        }

        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.param   =
            flow_check_info.param;
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.min_value =
            flow_check_info.min_value;
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.max_value =
            flow_check_info.max_value;
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.operation =
            flow_check_info.operation;
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.flags =
            flow_check_info.flags;
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->flags |=
            flow_check_info.flags;

        /* Set flowchecker type in the index. */
        *flowchecker_id =
            BCMI_FT_FLOWCHECKER_INDEX_SET(index, BCM_FT_IDX_TYPE_2ND_CHECK);

        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(bcmi_ft_flowchecker_get_free_index(unit, &index));

    /* Allocate memory for the new flowchecker. */
    BCMI_FT_FLOWCHECKER_INFO(unit, index) =
        (bcmi_flowtracker_flowchecker_info_t *)
        sal_alloc(sizeof(bcmi_flowtracker_flowchecker_info_t),
        "flowtracker info");

    if (BCMI_FT_FLOWCHECKER_INFO(unit, index) == NULL) {
        return BCM_E_MEMORY;
    }

    /* Rather than doing memcpy, we should actually set each element. */
    sal_memset(BCMI_FT_FLOWCHECKER_INFO(unit, index), 0,
        sizeof(bcmi_flowtracker_flowchecker_info_t));


    /* We can also do memset here as the data is same.
    bcmi_ft_flowchecker_info_set(unit, BCMI_FT_FLOWCHECKER_INFO(unit, index),
        flow_check_info);
    */

    BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.param   =
        flow_check_info.param;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.min_value =
        flow_check_info.min_value;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.max_value =
        flow_check_info.max_value;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.operation =
        flow_check_info.operation;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.flags =
        flow_check_info.flags;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->flags |=
        flow_check_info.flags;

    /* Set flowchecker type in the index. */
    *flowchecker_id =
        BCMI_FT_FLOWCHECKER_INDEX_SET(index, BCM_FT_IDX_TYPE_CHECK);

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_ft_check_get
 * Purpose:
 *    Get flowtracker information from the index
 *    provided by user. This is user interfacing method
 *    which will only send particular info which user API exposes.
 * Parameters:
 *     unit - (IN) BCM device id
 *     flowchecker_id - (IN) flowtracker check index.
 *     flow_check_info - (OUT) information to create flowtracker check.
 *
 * Returns:
 *     BCM_E_XXX.
 */
int bcmi_ft_check_get(
            int unit,
            bcm_flowtracker_check_t flowchecker_id,
            bcm_flowtracker_check_info_t *flow_check_info)
{

    int index;

    /* Check if input params are correct. */
    if (!flow_check_info) {
        return BCM_E_PARAM;
    }

    if (!BCMI_FT_IDX_IS_CHECK(flowchecker_id)) {
        return BCM_E_PARAM;
    }

    /* get the index from bitmap. */
    index = BCMI_FT_FLOWCHECKER_INDEX_GET(flowchecker_id);

    if (bcmi_ft_flowchecker_is_invalid(unit, index)) {
        return BCM_E_PARAM;
    }

    sal_memset(flow_check_info, 0, sizeof(bcm_flowtracker_check_info_t));

    if (BCMI_FT_IDX_IS_2ND_CHECK(flowchecker_id)) {
        flow_check_info->param     =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.param;
        flow_check_info->min_value =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.min_value;
        flow_check_info->max_value =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.max_value;
        flow_check_info->operation =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.operation;
        flow_check_info->flags =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.flags;

        if (BCMI_FT_FLOWCHECKER_INFO(unit, index)->flags &
                BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT2) {
            flow_check_info->flags |=
                BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT2;
        }
        flow_check_info->primary_check_id =
            BCMI_FT_FLOWCHECKER_INDEX_SET(index, BCM_FT_IDX_TYPE_CHECK);

    } else {

        flow_check_info->param     =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.param;
        flow_check_info->min_value =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.min_value;
        flow_check_info->max_value =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.max_value;
        flow_check_info->operation =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.operation;
        flow_check_info->flags =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.flags;

        if (BCMI_FT_FLOWCHECKER_INFO(unit, index)->flags &
            BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1) {
            flow_check_info->flags |=
                BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1;
        }

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_ft_check_traverse
 * Purpose:
 *    Traverse througth all the flowtracker check objects.
 * Parameters:
 *     unit - (IN) BCM device id
 *     cb   - (IN) Callback
 *     user_data - (INOUT) User data which is sent back.
 *
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_check_traverse(
            int unit,
            bcm_flowtracker_check_traverse_cb cb,
            void *user_data)
{
    bcm_flowtracker_check_t flowchecker_id;
    bcm_flowtracker_check_info_t flow_check_info;
    int index = 0;
    int num_flowcheckers = 0;
    soc_mem_t mem;
    mem = BSC_KG_GROUP_TABLEm;

    /* Total number of groups in device. */
    num_flowcheckers = soc_mem_index_count(unit, mem);

    if (cb == NULL) {
        return BCM_E_PARAM;
    }

    for(; index<num_flowcheckers; index++) {

        if (bcmi_ft_flowchecker_is_invalid(unit, index)) {
            continue;
        }
        sal_memset(&flow_check_info, 0, sizeof(bcm_flowtracker_check_info_t));

        flowchecker_id =
            BCMI_FT_FLOWCHECKER_INDEX_SET(index, BCM_FT_IDX_TYPE_CHECK);

        flow_check_info.param     =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.param;
        flow_check_info.min_value =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.min_value;
        flow_check_info.max_value =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.max_value;
        flow_check_info.operation =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.operation;
        flow_check_info.flags =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.flags;

        /* Invoke call back routine. */
        (*cb)(unit, flowchecker_id, &flow_check_info, user_data);

        /* Now send second check information. */
        if (!BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.param) {
            continue;
        }

        sal_memset(&flow_check_info, 0, sizeof(bcm_flowtracker_check_info_t));

        flowchecker_id =
            BCMI_FT_FLOWCHECKER_INDEX_SET(index, BCM_FT_IDX_TYPE_2ND_CHECK);

        flow_check_info.param     =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.param;
        flow_check_info.min_value =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.min_value;
        flow_check_info.max_value =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.max_value;
        flow_check_info.operation =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.operation;
        flow_check_info.flags =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.flags;

        flow_check_info.primary_check_id =
            BCMI_FT_FLOWCHECKER_INDEX_SET(index, BCM_FT_IDX_TYPE_CHECK);

        /* Invoke call back routine. */
        (*cb)(unit, flowchecker_id, &flow_check_info, user_data);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_ft_check_get
 * Purpose:
 *    Get flowtracker information from the index
 *    provided by user. This is internal function
 *    which provides all the internal data.
 * Parameters:
 *     unit - (IN) BCM device id
 *     flowchecker_id - (IN) flowtracker check index.
 *     flow_check_info - (OUT) information to create flowtracker check.
 *
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_flowchecker_get(
            int unit,
            bcm_flowtracker_check_t flowchecker_id,
            bcmi_flowtracker_flowchecker_info_t *flow_check_info)
{

    int index;

    /* Check if input params are correct. */
    if (!flow_check_info) {
        return BCM_E_PARAM;
    }
    if (!BCMI_FT_IDX_IS_CHECK(flowchecker_id)) {
        return BCM_E_PARAM;
    }

    /* get the index from bitmap. */
    index = BCMI_FT_FLOWCHECKER_INDEX_GET(flowchecker_id);

    if (bcmi_ft_flowchecker_is_invalid(unit, index)) {
        return BCM_E_PARAM;
    }

    /* Get the flags. */
    flow_check_info->flags =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->flags;

    /* Data of first check. */
    flow_check_info->check1.param     =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.param;
    flow_check_info->check1.min_value =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.min_value;
    flow_check_info->check1.max_value =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.max_value;
    flow_check_info->check1.operation =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.operation;
    flow_check_info->check1.flags =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.flags;

    /* Data of second check. */
    flow_check_info->check2.param     =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.param;
    flow_check_info->check2.min_value =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.min_value;
    flow_check_info->check2.max_value =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.max_value;
    flow_check_info->check2.operation =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.operation;
    flow_check_info->check2.flags =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.flags;

    flow_check_info->action_info.param =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.param;
    flow_check_info->action_info.action  =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.action;


    flow_check_info->export_info.export_check_threshold =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->
            export_info.export_check_threshold;

    flow_check_info->export_info.operation =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->export_info.operation;

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_ft_check_destroy
 * Purpose:
 *    Destroy flowtracker from the index
 *    provided by user.
 * Parameters:
 *     unit - (IN) BCM device id
 *     flowchecker_id - (IN) Placeholder for index generated.
 *
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_check_destroy(
            int unit,
            bcm_flowtracker_check_t flowchecker_id)
{

    int index;

    if (!BCMI_FT_IDX_IS_CHECK(flowchecker_id)) {
        return BCM_E_PARAM;
    }

    /* get the index from bitmap. */
    index = BCMI_FT_FLOWCHECKER_INDEX_GET(flowchecker_id);

    BCM_IF_ERROR_RETURN(bcmi_ft_flowchecker_is_invalid(unit, index));

    if (BCMI_FT_FLOWCHECKER_REFCOUNT(unit, index)) {
        /* For removing checks, they should be removed from FT groups. */
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "The check is still attached to FT groups.\n")));
        return BCM_E_BUSY;
    }

    if (BCMI_FT_IDX_IS_2ND_CHECK(flowchecker_id)) {
        /* Check if second check is configured. */
        if (!BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.operation) {
            return BCM_E_NOT_FOUND;
        }

        sal_memset((void *) (&(BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2)),
            0, sizeof(bcm_flowtracker_check_info_t));

        /* Second flowchecker is deleted. return. */
        return BCM_E_NONE;
    } else {
        /* Check if second check is configured. */
        if (BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.operation) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                            "Second Check is associated\n")));
            return BCM_E_BUSY;
        }
    }

    /* Free the flowchecker sw state. */
    sal_free(BCMI_FT_FLOWCHECKER_INFO(unit, index));

    /* Clear the bit for flowchecker. */
    SHR_BITCLR(BCMI_FT_FLOWCHECKER_BITMAP(unit), index);

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_ft_check_destroy_all
 * Purpose:
 *    Destroy all flowtrackers from the index
 *    provided by user which are not linked to any group.
 * Parameters:
 *     unit - (IN) BCM device id
 *
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_check_destroy_all(int unit)
{
    int index = 0;
    int num_flowcheckers = 0;
    soc_mem_t mem;
    mem = BSC_KG_GROUP_TABLEm;

    /* Total number of groups in device. */
    num_flowcheckers = soc_mem_index_count(unit, mem);

    for(index = 0; index < num_flowcheckers; index++) {

        /*
         * If we are not able to destroy then either it does not exists
         * or may be it is linked to group. both cases, we just bypass.
         */

        (void) bcmi_ft_check_destroy(unit, BCMI_FT_FLOWCHECKER_INDEX_SET( \
                    index, BCM_FT_IDX_TYPE_2ND_CHECK));
        (void) bcmi_ft_check_destroy(unit, BCMI_FT_FLOWCHECKER_INDEX_SET( \
                    index, BCM_FT_IDX_TYPE_CHECK));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_ft_check_action_info_set
 * Purpose:
 *    Set action informaiton on the flowtracker check
 * Parameters:
 *     unit - (IN) BCM device id
 *     flowchecker_id - (IN) flowtracker check index.
 *     info - (IN) action information.
 *
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_check_action_info_set(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_flowtracker_check_action_info_t info)
{
    int index = 0;

    if (!BCMI_FT_IDX_IS_CHECK(check_id)) {
        return BCM_E_PARAM;
    }

    if (BCMI_FT_IDX_IS_2ND_CHECK(check_id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "Actions can only be set on primary check.\n")));

        return BCM_E_PARAM;
    }


    /* get the index from bitmap. */
    index = BCMI_FT_FLOWCHECKER_INDEX_GET(check_id);

    if (bcmi_ft_flowchecker_is_invalid(unit, index)) {
        return BCM_E_PARAM;
    }

    /*
     * If the check is created on Dos Attacks and it is
     * dual operation on check 1 then raise error.
     */
    if (soc_feature(unit, soc_feature_hx5_ft_32bit_param_update)) {

        if ((info.action == bcmFlowtrackerCheckActionUpdateValue) &&
            ((BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.param ==
                bcmFlowtrackerTrackingParamTypeDosAttack) ||
            (BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.param ==
                bcmFlowtrackerTrackingParamTypeInnerDosAttack))) {

            if (bcmi_ft_flowchecker_opr_is_dual(unit,
                (BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.operation))) {
                return BCM_E_CONFIG;
            }
            /* Not allowed to create second check if dos vector is updated. */
            if (BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.param) {
                return BCM_E_CONFIG;
            }
        }
    }

    BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.param = info.param;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.action  = info.action;

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_ft_check_action_info_get
 * Purpose:
 *    Get action informaiton from the flowtracker check
 * Parameters:
 *     unit - (IN) BCM device id
 *     flowchecker_id - (IN) flowtracker check index.
 *     info - (OUT) action information.
 *
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_check_action_info_get(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_flowtracker_check_action_info_t *info)
{
    int index = 0;

    if (!BCMI_FT_IDX_IS_CHECK(check_id)) {
        return BCM_E_PARAM;
    }

    if (BCMI_FT_IDX_IS_2ND_CHECK(check_id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "Actions can only be retrieved on primary check.\n")));

        return BCM_E_PARAM;
    }

    /* get the index from bitmap. */
    index = BCMI_FT_FLOWCHECKER_INDEX_GET(check_id);

    if (bcmi_ft_flowchecker_is_invalid(unit, index)) {
        return BCM_E_PARAM;
    }

    info->param = BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.param;
    info->action  = BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.action;

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_ft_check_export_info_set
 * Purpose:
 *    set export informaiton from the flowtracker check
 * Parameters:
 *     unit - (IN) BCM device id
 *     flowchecker_id - (IN) flowtracker check index.
 *     info - (IN) export information.
 *
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_check_export_info_set(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_flowtracker_check_export_info_t info)
{
    int index = 0;

    if (!BCMI_FT_IDX_IS_CHECK(check_id)) {
        return BCM_E_PARAM;
    }

    if (BCMI_FT_IDX_IS_2ND_CHECK(check_id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "Export info can only be set on primary check.\n")));

        return BCM_E_PARAM;
    }

    /* get the index from bitmap. */
    index = BCMI_FT_FLOWCHECKER_INDEX_GET(check_id);

    if (bcmi_ft_flowchecker_is_invalid(unit, index)) {
        return BCM_E_PARAM;
    }

    if (bcmi_ft_flowchecker_opr_is_dual(unit, info.operation)) {
        /* No dual operations are supported for export threshold.*/
        return BCM_E_PARAM;
    }

    if (info.export_check_threshold &&
        (info.operation != bcmFlowtrackerCheckOpGreaterEqual)) {

        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "Export threshold is only supported with GreaterEqual operation.\n")));

        return BCM_E_CONFIG;
    }


    BCMI_FT_FLOWCHECKER_INFO(unit, index)->export_info.export_check_threshold =
        info.export_check_threshold;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->export_info.operation =
        info.operation;

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_ft_check_export_info_get
 * Purpose:
 *    Get export informaiton from the flowtracker check
 * Parameters:
 *     unit - (IN) BCM device id
 *     flowchecker_id - (IN) flowtracker check index.
 *     info - (OUT) export information.
 *
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_check_export_info_get(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_flowtracker_check_export_info_t *info)
{
    int index = 0;

    if (!BCMI_FT_IDX_IS_CHECK(check_id)) {
        return BCM_E_PARAM;
    }

    if (BCMI_FT_IDX_IS_2ND_CHECK(check_id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "Export info can only be retrieved from primary check.\n")));

        return BCM_E_PARAM;
    }

    /* get the index from bitmap. */
    index = BCMI_FT_FLOWCHECKER_INDEX_GET(check_id);

    if (bcmi_ft_flowchecker_is_invalid(unit, index)) {
        return BCM_E_PARAM;
    }

    info->export_check_threshold =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->
        export_info.export_check_threshold;

    info->operation =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->export_info.operation;

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_ft_flowchecker_list_length_get
 * Purpose:
 *    Add this flowtracker into the group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     head - (INOUT) Head pointer of the list.
 *
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_flowchecker_list_length_get(
        int unit,
        bcmi_ft_flowchecker_list_t **head)
{

   int length = 0;
    bcmi_ft_flowchecker_list_t *temp = NULL;

    if (!head) {
        return 0;
    }

    temp = (*head);

    while(temp) {
        length++;
        temp = temp->next;
    }

    return length;
}
/*
 * Function:
 *    bcmi_ft_flowchecker_list_add
 * Purpose:
 *    Add this flowtracker into the group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     head - (INOUT) Head pointer of the list.
 *     flowchecker_id - (IN) flowtracker check index.
 *
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_flowchecker_list_add(
        int unit,
        bcmi_ft_flowchecker_list_t **head,
        bcm_flowtracker_check_t flowchecker_id)
{
    bcmi_ft_flowchecker_list_t *temp_node = NULL;
    bcmi_ft_flowchecker_list_t *node_iter = NULL;
    bcmi_ft_flowchecker_list_t *node_prev = NULL;

    /* Create node */
    temp_node = (bcmi_ft_flowchecker_list_t *)
                    sal_alloc(sizeof(bcmi_ft_flowchecker_list_t),
                        "flowchecker list Node");
    if (temp_node == NULL) {
        return BCM_E_MEMORY;
    }
    temp_node->flowchecker_id = flowchecker_id;
    temp_node->next = NULL;

    node_prev = NULL;
    node_iter = (*head);
    while (node_iter != NULL) {
        if (node_iter->flowchecker_id == flowchecker_id) {
            sal_free(temp_node);
            return BCM_E_NONE;
        }
        node_prev = node_iter;
        node_iter = node_iter->next;
    }

    /* Insert */
    if (node_prev != NULL) {
        temp_node->next = node_prev->next;
        node_prev->next = temp_node;
    } else {
        temp_node->next = (*head);
        (*head) = temp_node;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_ft_flowchecker_list_delete
 * Purpose:
 *    Delete this flowtracker check from group list.
 * Parameters:
 *     unit - (IN) BCM device id
 *     head - (INOUT) Head pointer of the list.
 *     flowchecker_id - (IN) flowtracker check index.
 *
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_flowchecker_list_delete(
        int unit,
        bcmi_ft_flowchecker_list_t **head,
        bcm_flowtracker_check_t flowchecker_id)
{

    bcmi_ft_flowchecker_list_t *temp, *prev;
#if 0
    int index = 0;

    if (!BCMI_FT_IDX_IS_CHECK(flowchecker_id)) {
        return BCM_E_NONE;
    }

    /* get the index from bitmap. */
    index = BCMI_FT_FLOWCHECKER_INDEX_GET(flowchecker_id);

    if (bcmi_ft_flowchecker_is_invalid(unit, index)) {
        return BCM_E_NONE;
    }
#endif
    if (!head || (!(*head))) {
        return BCM_E_NOT_FOUND;
    }

    prev = temp = (*head);

    while(temp) {
        if (temp->flowchecker_id == flowchecker_id) {
            if (temp == (*head)) {
                (*head) = temp->next;
            } else {
                prev->next = temp->next;
            }
            sal_free(temp);
            temp = NULL;
            return BCM_E_NONE;
        }
        prev = temp;
        temp = temp->next;
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *    bcmi_ft_check_parts_get
 * Purpose:
 *    Get check parts for given primary check.
 * Parameters:
 *     unit - (IN) BCM device id
 *     flowchecker_id - (IN) flowtracker check index.
 *     check_ids - (OUT) Pointer to check Ids
 *     num_parts - (OUT) Number of parts
 *
 * Returns:
 *     BCM_E_XXX.
 */

int
bcmi_ft_check_parts_get(
        int unit,
        bcm_flowtracker_check_t flowchecker_id,
        bcm_flowtracker_check_t *check_ids,
        int *num_parts)
{
    int part = 0;
    int index = 0;
    bcmi_flowtracker_flowchecker_info_t fc_info;
    sal_memset(&fc_info, 0, sizeof(bcmi_flowtracker_flowchecker_info_t));

    BCM_IF_ERROR_RETURN(bcmi_ft_flowchecker_get
            (unit, flowchecker_id, &fc_info));

    /* Adding Primary */
    check_ids[part++] = flowchecker_id;

    /* get the index from bitmap. */
    index = BCMI_FT_FLOWCHECKER_INDEX_GET(flowchecker_id);

    /* Adding Secondary */
    if (BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.operation) {
        check_ids[part++] =
            BCMI_FT_FLOWCHECKER_INDEX_SET(index, BCM_FT_IDX_TYPE_2ND_CHECK);
    }

    *num_parts = part;

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_ft_group_flowchecker_add
 * Purpose:
 *    Add this flowtracker check into group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id  - (IN) flowtracker group index.
 *     flowchecker_id - (IN) flowtracker check index.
 *
 * Returns:
 *     BCM_E_XXX.
 */

int
bcmi_ft_group_flowchecker_add(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_check_t flowchecker_id)
{
    int part = 0;
    int index;
    int rv = BCM_E_NONE;
    int num_parts = 0;
    bcm_flowtracker_check_t check_ids[2];
    bcmi_ft_flowchecker_list_t **head = NULL;

    /* validate flow group index. */
    if (bcmi_ft_group_is_invalid(unit, id)) {
        return (BCM_E_PARAM);
    }

    if (!BCMI_FT_IDX_IS_CHECK(flowchecker_id)) {
        return BCM_E_PARAM;
    }

    /* get the index from bitmap. */
    index = BCMI_FT_FLOWCHECKER_INDEX_GET(flowchecker_id);

    /* Validate flow checker index. */
    if (bcmi_ft_flowchecker_is_invalid(unit, index)) {
        return BCM_E_PARAM;
    }

    if (BCMI_FT_GROUP_IS_BUSY(unit, id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, "Adding check to"
                        " installed Group (%d) is not allowed. \n"), id));
        return BCM_E_CONFIG;
    }
    if (!BCMI_FT_IDX_IS_PRIMARY_CHECK(flowchecker_id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, "Given check"
                        " is not primary\n")));
        return BCM_E_PARAM;
    }

    /* set Check timestamp */
    rv = bcmi_ft_group_check_ts_set(unit, id, flowchecker_id,
            bcmFlowtrackerFlowCheckTsSet);
    BCM_IF_ERROR_RETURN(rv);

    /* Get associated flowchecks */
    rv = bcmi_ft_check_parts_get(unit, flowchecker_id, check_ids, &num_parts);
    BCM_IF_ERROR_RETURN(rv);

    /* Add to flowchecker list of group */
    head = &(BCMI_FT_GROUP_FLOWCHECKER_HEAD(unit, id));
    for (part = 0; (part < num_parts) && BCM_SUCCESS(rv); part++) {
        rv = bcmi_ft_flowchecker_list_add(unit, head, check_ids[part]);
    }

    if (BCM_FAILURE(rv)) {
        for (part = 0; part < num_parts; part++) {
            bcmi_ft_flowchecker_list_delete(unit, head, check_ids[part]);
        }

        return rv;
    }

    BCMI_FT_FLOWCHECKER_REFCOUNT_INC(unit, index);

    return rv;
}

/*
 * Function:
 *    bcmi_ft_group_flowchecker_delete
 * Purpose:
 *    Delete this flowtracker check from group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id  - (IN) flowtracker group index.
 *     flowchecker_id - (IN) flowtracker check index.
 *
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_group_flowchecker_delete(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_check_t flowchecker_id)
{
    int rv = BCM_E_NONE;
    int index;
    int part = 0;
    int num_parts = 0;
    bcmi_ft_flowchecker_list_t **head = NULL;
    bcm_flowtracker_check_t check_ids[2];

    /* validate flow group index. */
    if (bcmi_ft_group_is_invalid(unit, id)) {
        return (BCM_E_PARAM);
    }

    if (!BCMI_FT_IDX_IS_CHECK(flowchecker_id)) {
        return BCM_E_PARAM;
    }

    /* get the index from bitmap. */
    index = BCMI_FT_FLOWCHECKER_INDEX_GET(flowchecker_id);

    /* Validate flow checker index. */
    if (bcmi_ft_flowchecker_is_invalid(unit, index)) {
        return BCM_E_PARAM;
    }

    if (BCMI_FT_GROUP_IS_BUSY(unit, id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, "Please uninstall"
                        " Group (%d). operation is not allowed. \n"), id));
        return BCM_E_CONFIG;
    }

    if (!BCMI_FT_IDX_IS_PRIMARY_CHECK(flowchecker_id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, "Given check"
                        " is not primary\n")));
        return BCM_E_PARAM;
    }

    /* reset Check timestamps */
    rv = bcmi_ft_group_check_ts_set(unit, id, flowchecker_id,
            bcmFlowtrackerFlowCheckTsReset);
    BCM_IF_ERROR_RETURN(rv);

    /* Get associated flowchecks */
    rv = bcmi_ft_check_parts_get(unit, flowchecker_id, check_ids, &num_parts);
    BCM_IF_ERROR_RETURN(rv);

    head = &(BCMI_FT_GROUP_FLOWCHECKER_HEAD(unit, id));
    for (part = 0; (part < num_parts) && (BCM_SUCCESS(rv)); part++) {
        rv = bcmi_ft_flowchecker_list_delete(unit, head, check_ids[part]);
    }

    if (BCM_SUCCESS(rv)) {
        BCMI_FT_FLOWCHECKER_REFCOUNT_DEC(unit, index);
    }

    return rv;
}

/*
 * Function:
 *    bcmi_ft_group_flowchecker_get_all
 * Purpose:
 *    Get all the flowtracker checks from group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id  - (IN) flowtracker group index.
 *     max_checks - (IN) max number of flowtracker check ids to be sent..
 *     list_of_check_ids - (IN) list of the flowtracker check ids.
 *     num_checks - NUmber of checks actually sent.
 *
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_flowtracker_check_get_all(
    int unit,
    bcm_flowtracker_group_t id,
    int skip_non_primary,
    int max_checks,
    bcm_flowtracker_check_t *list_of_check_ids,
    int *num_checks)
{

    bcmi_ft_flowchecker_list_t *temp;
    int only_count = 0;
    int i = 0;

    /* validate flow group index. */
    if (bcmi_ft_group_is_invalid(unit, id)) {
        return (BCM_E_PARAM);
    }

    /* If the max_checks is non zero and list == NULL, return
     * BCM_E_PARAM.
     */
    if (max_checks != 0 && list_of_check_ids == NULL) {
        return BCM_E_PARAM;
    }

    /* If max_alu_info == 0 and list_of_export_elements == NULL,
     * return the number of export elements in list_size
     * variable.
     */
    if (max_checks == 0 && list_of_check_ids == NULL) {
        only_count = 1;
    }

    temp = BCMI_FT_GROUP_FLOWCHECKER_HEAD(unit, id);

    while (temp) {
        /*
         * Go till end and start putting ids in array.
         * The limit is given by max_checks.
         */
        if ((!only_count) && (max_checks != -1) && (i < max_checks)) {
            if ((skip_non_primary == TRUE) &&
                    (!BCMI_FT_IDX_IS_PRIMARY_CHECK(temp->flowchecker_id))) {
                temp = temp->next;
                continue;
            }
            list_of_check_ids[i] =  temp->flowchecker_id;
        }

        i++;
        temp = temp->next;
    }

    *num_checks = i;

    return BCM_E_NONE;
}


/*
 * Function:
 *    bcmi_ft_check_compare
 * Purpose:
 *    Compare two flowtracker checks.
 * Parameters:
 *    unit - (IN) BCM device id
 *    check_id1 - (IN) first flowtracker check index.
 *    check_id2 - (IN) Second flowtracker check index.
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_check_compare(
        int unit,
        bcm_flowtracker_check_t check_id1,
        bcm_flowtracker_check_t check_id2)
{

    bcmi_flowtracker_flowchecker_info_t fc_info1, fc_info2;


    sal_memset(&fc_info1, 0, sizeof(bcmi_flowtracker_flowchecker_info_t));
    sal_memset(&fc_info2, 0, sizeof(bcmi_flowtracker_flowchecker_info_t));

    /* If both flow checks are same then return match. */
    if (check_id1 == check_id2) {
        return BCM_E_NONE;
    }

    /* Validate flow checker index. */
    if (bcmi_ft_flowchecker_is_invalid(unit,
        BCMI_FT_FLOWCHECKER_INDEX_GET(check_id1))) {

        return BCM_E_NOT_FOUND;
    }

    /* Validate flow checker index. */
    if (bcmi_ft_flowchecker_is_invalid(unit,
        BCMI_FT_FLOWCHECKER_INDEX_GET(check_id2))) {

        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN(bcmi_ft_flowchecker_get
            (unit, check_id1, &fc_info1));

    BCM_IF_ERROR_RETURN(bcmi_ft_flowchecker_get
            (unit, check_id2, &fc_info2));

    /* Compare the flow checks and if same then return MATCH. */
    if (!(sal_memcmp(&fc_info1, &fc_info2,
            sizeof(bcmi_flowtracker_flowchecker_info_t)))) {
       return BCM_E_NONE;
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *    bcmi_ft_check_list_compare
 * Purpose:
 *    Compare all the checks in group list.
 * Parameters:
 *    unit - (IN) BCM device id
 *    head - (IN) Head of the flowchecker list.
 *    check_id - (IN) flowtracker check index.
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_check_list_compare(
        int unit,
        bcmi_ft_flowchecker_list_t **head,
        bcm_flowtracker_check_t check_id)
{

    bcmi_ft_flowchecker_list_t *temp;

    if ((head == NULL) || (*head == NULL)) {
        return BCM_E_NOT_FOUND;
    }

    temp = *head;

    while (temp != NULL) {
        if (!(bcmi_ft_check_compare(unit, check_id, temp->flowchecker_id))) {
            return BCM_E_NONE;
        }
        temp = temp->next;
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *    bcmi_ft_group_check_validate
 * Purpose:
 *    Validate Flowcheck if given configuration is allowed
 *    based in options params.
 * Parameters:
 *    unit - (IN) BCM device id
 *    optiosn - (IN) Match criteria
 *    id - (IN) Flowtracker group
 *    check_id - (IN) flowtracker check
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_group_check_validate(
        int unit,
        uint32 options,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_check_t check_id)
{
    int index;
    int found = FALSE;
    bcmi_ft_flowchecker_list_t *head = NULL;
    bcmi_ft_flowchecker_list_t *temp = NULL;

    if (!BCMI_FT_IDX_IS_CHECK(check_id)) {
        return BCM_E_PARAM;
    }
    index = BCMI_FT_FLOWCHECKER_INDEX_GET(check_id);
    if (bcmi_ft_flowchecker_is_invalid(unit, index)) {
        return BCM_E_PARAM;
    }

    if (options & BCMI_FT_GROUP_CHECK_MATCH_PRIMARY) {
        if (!BCMI_FT_IDX_IS_PRIMARY_CHECK(check_id)) {
            return BCM_E_PARAM;
        }
    }

    if(options & BCMI_FT_GROUP_CHECK_MATCH_GROUP) {
        /* Search for CheckId in the Group */
        if (bcmi_ft_group_is_invalid(unit, id)) {
            return BCM_E_NOT_FOUND;
        }
        head = BCMI_FT_GROUP_FLOWCHECKER_HEAD(unit, id);
        if (!head) {
            return BCM_E_NOT_FOUND;
        }
        temp = (head);
        while(temp) {
            if (temp->flowchecker_id == check_id) {
                found = TRUE;
                break;
            }
            temp = temp->next;
        }
        if (found == FALSE) {
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}


#else /* BCM_FLOWTRCAKER_SUPPORT*/
int bcmi_ft_check_not_empty;
#endif /* BCM_FLOWTRCAKER_SUPPORT */
