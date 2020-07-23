/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ft_check.c
 * Purpose:     The purpose of this file is to set flow tracker check methods.
 * Requires:
 */

#include <bcm_int/esw/flowtracker/ft_group.h>
#include <bcm_int/esw/flowtracker/ft_field.h>
#include <bcm_int/esw/flowtracker/ft_drop.h>

#if defined(BCM_FLOWTRACKER_SUPPORT)

/* Global flowtracker checker state structure */
bcmi_ft_flowchecker_sw_state_t
                        bcmi_ft_flowchecker_state[BCM_MAX_NUM_UNITS];

/*
 * Function:
 *   bcmi_ft_flowchecker_get_free_index
 * Purpose:
 *   Get free index of the flowtracker check element.
 * Description:
 *   At present the checks are numbered equal to group
 *   number but there is a provision to increase it to any
 *   number by adding them to the linked list.
 *   But it may not be required as checks can be shared
 *   among various groups and therefore we may not need
 *   explicit check for each group.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   BCM_E_XXX.
 */
static int
bcmi_ft_flowchecker_get_free_index(int unit, int *index)
{
    int i = 0;
    int num_flowcheckers = 0;

    /* Total number of groups in device. */
    num_flowcheckers = BCMI_FT_FLOWCHECKER_MAX_ID(unit) - 1;

    if (*index != -1) {
        i = *index;
        num_flowcheckers = *index;
    }

    /* Got the free index. use it */
    for (; i <= num_flowcheckers; i++) {
        if (!(SHR_BITGET(BCMI_FT_FLOWCHECKER_BITMAP(unit), i))) {
            SHR_BITSET(BCMI_FT_FLOWCHECKER_BITMAP(unit), i);
            *index = i;
            return BCM_E_NONE;
        }
    }

    return BCM_E_RESOURCE;
}

/*
 * Function:
 *   bcmi_ft_flowchecker_idx_is_invalid
 * Purpose:
 *   Check if the flowtracker check index is valid
 * Parameters:
 *   unit - (IN) BCM device id
 *   idx  - (IN) flowchecker index.
 * Returns:
 *   BCM_E_XXX.
 */
static int
bcmi_ft_flowchecker_idx_is_invalid(int unit, int idx)
{
    int num_flowcheckers = 0;

    /* Total number of groups/flowcheckers in device. */
    num_flowcheckers = BCMI_FT_FLOWCHECKER_MAX_ID(unit);

    if (idx >= num_flowcheckers) {
        return BCM_E_PARAM;
    }

    if (BCMI_FT_FLOWCHECKER_BITMAP(unit) == NULL) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
           "Internal Error.\n")));
        return BCM_E_INIT;
    }

    if (!(SHR_BITGET(BCMI_FT_FLOWCHECKER_BITMAP(unit), idx))) {
        return BCM_E_NOT_FOUND;
    }

    if (BCMI_FT_FLOWCHECKER_INFO(unit, idx) == NULL) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
           "Checker id not found.\n")));
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_flowchecker_id_is_invalid
 * Purpose:
 *   Check if the flowtracker check id is valid
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) flowchecker id.
 * Returns:
 *   BCM_E_XXX.
 */
static int
bcmi_ft_flowchecker_id_is_invalid(int unit, int id)
{
    int idx = -1;

    /* validate checker id */
    if (!BCMI_FT_IDX_IS_CHECK(id)) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
           "Invalid flowtracker checker id=0x%x\n"), id));
        return BCM_E_PARAM;
    }

    /* get the index from bitmap */
    idx = BCMI_FT_FLOWCHECKER_INDEX_GET(id);

    /* validate checker index */
    BCM_IF_ERROR_RETURN(
        bcmi_ft_flowchecker_idx_is_invalid(unit, idx));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_check_param_is_allowed
 * Purpose:
 *   param is allowed in checker or not
 * Parameters:
 *   unit      - (IN) BCM device number
 *   param     - (IN) tracking parameter
 * Returns:
 *   BCM_E_NONE - param is allowed
 *   BCM_E_XXX  - param is not allowed
 */
static int
bcmi_ft_check_param_is_allowed(int unit,
                               bcm_flowtracker_tracking_param_type_t c_param)
{
    int rv = BCM_E_PARAM;

    switch (c_param) {
        /* Tracking Params generated from ALU32 */
        case bcmFlowtrackerTrackingParamTypePktCount:
        case bcmFlowtrackerTrackingParamTypeChipDelay:
        case bcmFlowtrackerTrackingParamTypeE2EDelay:
        case bcmFlowtrackerTrackingParamTypeIPATDelay:
        case bcmFlowtrackerTrackingParamTypeIPDTDelay:
        /* TimeStamp related tracking params */
        case bcmFlowtrackerTrackingParamTypeTimestampNewLearn:
        case bcmFlowtrackerTrackingParamTypeTimestampFlowStart:
        case bcmFlowtrackerTrackingParamTypeTimestampFlowEnd:
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1:
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2:
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent3:
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent4:
        /* Special param to expose checker */
        case bcmFlowtrackerTrackingParamTypeFlowtrackerCheck:
        /* Too big param size for check to act */
        case bcmFlowtrackerTrackingParamTypeSrcIPv6:
        case bcmFlowtrackerTrackingParamTypeDstIPv6:
        case bcmFlowtrackerTrackingParamTypeInnerSrcIPv6:
        case bcmFlowtrackerTrackingParamTypeInnerDstIPv6:
            rv = BCM_E_PARAM;
            break;
        /* OTP based check */
        case bcmFlowtrackerTrackingParamTypeDosAttack:
        case bcmFlowtrackerTrackingParamTypeInnerDosAttack:
            if (!soc_feature(unit,
                     soc_feature_flowtracker_ver_1_dos_attack_check)) {
                rv = BCM_E_UNAVAIL;
            } else {
                rv = BCM_E_NONE;
            }
            break;
        case bcmFlowtrackerTrackingParamTypeByteCount:
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
                rv = BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        default:
            rv = BCM_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *   bcmi_ft_check_action_info_sanity
 * Purpose:
 *   validate flowtracker check action information
 * Parameters:
 *   unit  - (IN) BCM device id
 *   index - (IN) checker index
 *   info  - (IN) check action information
 * Returns:
 *   BCM_E_xxx
 * Notes:
 *   index is validated by calling function
 */
static int
bcmi_ft_check_action_info_sanity(int unit, int index,
                                 bcm_flowtracker_check_action_info_t info)
{
    int rv = BCM_E_NONE;
    int num_bits_req = 0;
    int num_operands = 0;
    uint16 temp_ft_type_bmp = 0;
    uint16 action_ft_type_bmp = 0xFFFF;

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

    /* Get number of operands required for the given action */
    rv = bcmi_ft_check_action_num_operands_get(unit,
                info.action, &num_operands);
    BCM_IF_ERROR_RETURN(rv);

    if (num_operands != 0) {

        if (info.param == bcmFlowtrackerTrackingParamTypeNone) {
            return BCM_E_PARAM;
        }

        /* check if param is allowed for action */
        rv = bcmi_ft_check_param_is_allowed(unit, info.param);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                            "param not allowed for action info.\n")));
            return rv;
        }

        if (BCMI_FT_FLOWCHECKER_INFO(unit, index)->flags &
                BCM_FLOWTRACKER_CHECK_MODE_16BIT) {
            rv = _bcm_field_ft_tracking_param_range_get(unit,
                    info.param, info.custom_id, &num_bits_req , NULL, NULL);
            BCM_IF_ERROR_RETURN(rv);

            if ((num_bits_req != 0) && (num_bits_req > 16)) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                "Flowtracker Check mode does not support"
                                " Action param size.\n")));
                return BCM_E_PARAM;
            }
        }

        /* check tracking param is supported in system or not */
        rv = _bcm_field_ft_tracking_param_type_get(unit,
                    info.param, info.custom_id, 0, &action_ft_type_bmp);
        BCM_IF_ERROR_RETURN(rv);

        if ((info.direction < bcmFlowtrackerFlowDirectionNone) ||
                (info.direction >= bcmFlowtrackerFlowDirectionCount)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                            "Flow direction invalid\n")));
            return BCM_E_PARAM;
        }
        if ((info.direction == bcmFlowtrackerFlowDirectionReverse) ||
            (info.direction == bcmFlowtrackerFlowDirectionBidirectional)) {
            if (!(soc_feature(unit,
                            soc_feature_flowtracker_ver_1_bidirectional))) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                "Flow direction unavailable\n")));
                return BCM_E_UNAVAIL;
            }
        }
    }

    if (info.action == bcmFlowtrackerCheckActionUpdateAverageValue) {
        rv = bcmi_ft_tracking_param_weight_hw_val_get(unit, info.weight, NULL);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                            "Incorrect configuration value for"
                            "tracking param weight.\n")));
            return rv;
        }
    }

    /* validate stage of action - Action tracking param must support ft types
     * which is already supported by flowcheck. Also, after addition of action
     * ft types supported by flowcheck must be updated with action ft type.
     */
    temp_ft_type_bmp = (action_ft_type_bmp &
        BCMI_FT_FLOWCHECKER_INFO(unit,index)->check_ft_type_bmp);
    if (temp_ft_type_bmp == 0) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "Action pipeline stage does not match with "
                        "checker stage create.\n")));
        return BCM_E_PARAM;
    } else {
        BCMI_FT_FLOWCHECKER_INFO(unit,index)->check_ft_type_bmp =
            temp_ft_type_bmp;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_check_export_info_sanity
 * Purpose:
 *   validate flowtracker check export information
 * Parameters:
 *   unit  - (IN) BCM device id
 *   index - (IN) checker index
 *   info  - (IN) check export information
 * Returns:
 *   BCM_E_xxx
 * Notes:
 *   index is validated by calling function
 */
static int
bcmi_ft_check_export_info_sanity(int unit, int index,
                                 bcm_flowtracker_check_export_info_t info)
{
    /* No dual operations are supported for export threshold */
    if (bcmi_ft_flowchecker_opr_is_dual(unit, info.operation)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
           "Range Operations are not supported for export.\n")));
        return BCM_E_PARAM;
    }

    /* No global mask operation is supported for export threshold */
    if (info.operation == bcmFlowtrackerCheckOpMask) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
           "Mask Operation is not supported for export.\n")));
        return BCM_E_PARAM;
    }

    if (info.export_check_threshold &&
       (info.operation != bcmFlowtrackerCheckOpGreaterEqual)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
         "Export threshold is only supported with GreaterEqual operation.\n")));
        return BCM_E_CONFIG;
    }

    return BCM_E_NONE;
}

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
/*
 * Function:
 *   bcmi_ft_check_delay_info_sanity
 * Purpose:
 *   validate flowtracker check delay information
 * Parameters:
 *   unit  - (IN) BCM device id
 *   index - (IN) checker index
 *   info  - (IN) check delay information
 * Returns:
 *   BCM_E_xxx
 * Notes:
 *   index is validated by calling function
 */
static int
bcmi_ft_check_delay_info_sanity(int unit, int index,
                                bcm_flowtracker_check_delay_info_t info)
{
    int delay_offset_max = 0;
    int delay_offset_field_len = 0;

    if ((info.src_ts != 0) || (info.dst_ts != 0)) {

        /* source & destination timestamp cannot be same */
        if (info.src_ts == info.dst_ts) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
               "Source & destination timestamp cannot be same.\n")));
            return BCM_E_PARAM;
        }

        /* validate destination timestamp */
        if ((info.dst_ts == bcmFlowtrackerTimestampSourceLegacy) ||
            (info.dst_ts == bcmFlowtrackerTimestampSourceNTP) ||
            (info.dst_ts == bcmFlowtrackerTimestampSourcePTP) ||
            (info.dst_ts == bcmFlowtrackerTimestampSourceCMIC) ||
            (info.dst_ts ==
                 bcmFlowtrackerTimestampSourceIngressFlowtrackerStage)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
               "Destination timestamp is not valid.\n")));
            return BCM_E_PARAM;
        }
    }

    /* validate gran */
    if (soc_format_field_valid(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                DELAY_GRANULARITYf)) {
        if (info.gran > (1 << soc_format_field_length(unit,
                                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                                DELAY_GRANULARITYf)) - 1) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
               "Delay granularity exceeded max allowed.\n")));
            return BCM_E_PARAM;
        }
    }

    /* validate offset */
    delay_offset_field_len = soc_format_field_length(unit,
                                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                                DELAY_OFFSETf);
    if (delay_offset_field_len) {
        delay_offset_max = ((1 << (delay_offset_field_len - 1)) - 1);
    }

    if ((info.offset > delay_offset_max) ||
        (info.offset < (-delay_offset_max))) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
           "Delay offset %d is not allowed in valid range -%d : +%d.\n"),
            info.offset, delay_offset_max, delay_offset_max));
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

/*
 * Function:
 *   bcmi_ft_check_operation_sanity
 * Purpose:
 *   validate flowtracker check operation
 * Parameters:
 *   unit - (IN) BCM device id
 *   oper - (IN) check operation
 * Returns:
 *   BCM_E_xxx
 */
static int
bcmi_ft_check_operation_sanity(int unit,
                               bcm_flowtracker_check_operation_t oper)
{
    const char *olist[] = BCM_FT_CHECK_OPERATION_STRINGS;

    switch(oper) {
        case bcmFlowtrackerCheckOpEqual:
        case bcmFlowtrackerCheckOpNotEqual:
        case bcmFlowtrackerCheckOpSmaller:
        case bcmFlowtrackerCheckOpSmallerEqual:
        case bcmFlowtrackerCheckOpGreater:
        case bcmFlowtrackerCheckOpGreaterEqual:
        case bcmFlowtrackerCheckOpPass:
        case bcmFlowtrackerCheckOpFail:
        case bcmFlowtrackerCheckOpInRange:
        case bcmFlowtrackerCheckOpOutofRange:
            return BCM_E_NONE;
        case bcmFlowtrackerCheckOpMask:
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
                return BCM_E_NONE;
            } else
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */
            {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                   "Operation %s is not supported.\n"),olist[oper]));
                return BCM_E_PARAM;
            }
        default:
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
               "Operation %s is not supported.\n"),olist[oper]));
            return BCM_E_PARAM;
    }
}

/*
 * Function:
 *   bcmi_ft_check_options_and_flags_sanity
 * Purpose:
 *   validate flowtracker check flags and options input
 * Parameters:
 *   unit    - (IN) BCM device id
 *   options - (IN) options in checker create
 *   flow_check_info - (IN) Flowchecker info
 *   flags   - (IN) checker flags
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_ft_check_options_and_flags_sanity(int unit, uint32 options,
                    bcm_flowtracker_check_info_t *flow_check_info,
                    uint16 *ft_type_bmp)
{
    uint32 flags;
    *ft_type_bmp = BCMI_FT_TYPE_F_ANY;

    flags = flow_check_info->flags;

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        /* do nothing */
    } else
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */
    {
        if ((flags & BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT3) ||
            (flags & BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT4) ||
            (flags & BCM_FLOWTRACKER_CHECK_DATA_CLEAR_ON_PERIODIC_EXPORT)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
               "flags not supported on this device.\n")));
            return BCM_E_PARAM;
        }
    }

    /* check secondary checker cannot be created with id */
    if ((options & BCM_FLOWTRACKER_CHECK_SECOND) &&
        (options & BCM_FLOWTRACKER_CHECK_WITH_ID)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
           "Secondary check is not allowed with "
           "BCM_FLOWTRACKER_CHECK_WITH_ID.\n")));
        return BCM_E_CONFIG;
    }

    /* check event flags are not supported for secondary checks */
    if (options & BCM_FLOWTRACKER_CHECK_SECOND) {
        if ((flags & BCM_FLOWTRACKER_CHECK_DATA_CLEAR_ON_EXPORT) ||
            (flags & BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD) ||
            (flags & BCM_FLOWTRACKER_CHECK_DATA_CLEAR_ON_PERIODIC_EXPORT) ||
            (flags & BCM_FLOWTRACKER_CHECK_UNICAST_FLOW_ONLY) ||
            (flags & BCM_FLOWTRACKER_CHECK_MULTICAST_FLOW_ONLY) ||
            (flags & BCM_FLOWTRACKER_CHECK_MODE_32BIT) ||
            (flags & BCM_FLOWTRACKER_CHECK_MODE_16BIT)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
               "Unsupported flags for secondary check.\n")));
            return BCM_E_CONFIG;
        }
    }

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {

        /* State transition flags is not allowed in second check */
        if ((options & BCM_FLOWTRACKER_CHECK_SECOND) &&
                (flow_check_info->state_transition_flags)) {
            LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                            "State transition flags unsupported"
                            " for secondary check.\n")));
            return BCM_E_CONFIG;
        }

        /* Unicast or Multicast flow flags cannot be together */
        if ((flags & BCM_FLOWTRACKER_CHECK_UNICAST_FLOW_ONLY) &&
                (flags & BCM_FLOWTRACKER_CHECK_MULTICAST_FLOW_ONLY)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                            "flags combination not supported.\n")));
            return BCM_E_CONFIG;
        }

        /* MFT/AMFT/EFT/AEFT ft type support these flags */
        if ((flags & BCM_FLOWTRACKER_CHECK_UNICAST_FLOW_ONLY) ||
                (flags & BCM_FLOWTRACKER_CHECK_MULTICAST_FLOW_ONLY)) {
            *ft_type_bmp = (BCMI_FT_TYPE_F_MFT | BCMI_FT_TYPE_F_AMFT |
                    BCMI_FT_TYPE_F_EFT | BCMI_FT_TYPE_F_AEFT);
        }

        /* AIFT ft type supports State transition flags */
        if (flow_check_info->state_transition_flags) {
            *ft_type_bmp = (BCMI_FT_TYPE_F_AIFT);
        }
    }
#endif

    return BCM_E_NONE;;
}

/*
 * Function:
 *   bcmi_ft_check_param_sanity
 * Purpose:
 *   check input options param and check info
 *   and return ft_type of stage it belongs to
 * Parameters:
 *   unit            - (IN) BCM device id
 *   options         - (IN) options in checker create
 *   flow_check_info - (IN) flow checker info
 *   ft_type         - (OUT) Flowtracker Type
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_ft_check_param_sanity(int unit, uint32 options,
                           bcm_flowtracker_check_info_t flow_check_info,
                           uint16 *check_ft_type_bmp)
{
    int rv = BCM_E_NONE;
    int num_bits_req = 0;
    uint32 min_value = 0, max_value = 0;
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    int iter = 0;
#endif
    uint16 flags_ft_type_bmp = 0;
    uint16 param_ft_type_bmp = 0;

    /* check if flags & options provided are supported in system or not */
    rv = bcmi_ft_check_options_and_flags_sanity(unit, options,
                            &flow_check_info, &flags_ft_type_bmp);
    BCM_IF_ERROR_RETURN(rv);

    /* check if operation provided is supported in system or not */
    rv = bcmi_ft_check_operation_sanity(unit, flow_check_info.operation);
    BCM_IF_ERROR_RETURN(rv);

    /* If user has not specified param, only PASS/FAIL operation allowed */
    if (flow_check_info.param == bcmFlowtrackerTrackingParamTypeNone) {
        if ((flow_check_info.operation != bcmFlowtrackerCheckOpPass) &&
                (flow_check_info.operation != bcmFlowtrackerCheckOpFail)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, "Only Pass/Fail"
                            " operation allowed if param is NONE.\n\r")));
            return BCM_E_CONFIG;
        }
    }

    /* check if param provided is supported in system or not */
    rv = _bcm_field_ft_tracking_param_type_get(unit, flow_check_info.param,
                            flow_check_info.custom_id, 0, &param_ft_type_bmp);
    BCM_IF_ERROR_RETURN(rv);

    /* Ft Type cannot be assigned to checker */
    if ((param_ft_type_bmp & flags_ft_type_bmp) == 0) {
        return BCM_E_PARAM;
    } else {
        *check_ft_type_bmp = (param_ft_type_bmp & flags_ft_type_bmp);
    }

    /* check if param is allowed for checker */
    rv = bcmi_ft_check_param_is_allowed(unit, flow_check_info.param);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
           "param not allowed for checker operation\n")));
        return rv;
    }

    /* check if param have valid mix/max values */
    rv = _bcm_field_ft_tracking_param_range_get(unit, flow_check_info.param,
              flow_check_info.custom_id, &num_bits_req, &min_value, &max_value);
    BCM_IF_ERROR_RETURN(rv);

    /* validate min/max values */
    if (flow_check_info.min_value < min_value) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
           "provided min value %d cannot be lesser than allowed value %d.\n"),
                                       flow_check_info.min_value, min_value));
        return BCM_E_PARAM;
    }

    if ((flow_check_info.flags & BCM_FLOWTRACKER_CHECK_MODE_16BIT) &&
            ((num_bits_req != 0) && (num_bits_req > 16))) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "Flowtracker Check mode cannot be supported with"
                        " given parameter size.\n")));
        return BCM_E_PARAM;
    }
    if (flow_check_info.max_value > max_value) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
           "provided max value %d cannot be more than allowed value %d.\n"),
                                    flow_check_info.max_value, max_value));
        return BCM_E_PARAM;
    }

    /* Validate input for drop reason group id vector param */
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        if ((flow_check_info.param ==
             bcmFlowtrackerTrackingParamTypeIngDropReasonGroupIdVector) ||
            (flow_check_info.param ==
             bcmFlowtrackerTrackingParamTypeEgrDropReasonGroupIdVector)) {
            if (flow_check_info.num_drop_reason_group_id >
                    BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,(BSL_META_U(unit,
                   "num_drop_reason_group_id %d cannot be "
                   "more than max value %d.\n"),
                    flow_check_info.num_drop_reason_group_id,
                    BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX));
                return BCM_E_PARAM;
            } else {
                for (iter = 0;
                     iter < flow_check_info.num_drop_reason_group_id; iter++) {
                    BCM_IF_ERROR_RETURN
                        (bcmi_ftv2_drop_reason_group_id_is_invalid(unit,
                                  flow_check_info.drop_reason_group_id[iter]));
                }
            }
        }
    } else
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */
    {
        if (flow_check_info.num_drop_reason_group_id != 0) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,(BSL_META_U(unit,
               "num_drop_reason_group_id should be 0 "
               "for non drop reason group id vector tracking param.\n")));
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_flowchecker_init
 * Purpose:
 *   Initialize software state of flowtracker checks.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_flowchecker_init(int unit)
{
    int num_flowcheckers = 0;
    soc_mem_t mem = BSC_KG_GROUP_TABLEm;

    /* Total number of groups in device. */
    BCMI_FT_FLOWCHECKER_MAX_ID(unit) =
                  num_flowcheckers = soc_mem_index_count(unit, mem);

    /* Allocate space for the flowchecker database structures. */
    BCMI_FT_FLOWCHECKER_STATE(unit) = (bcmi_flowtracker_flowchecker_info_t **)
        sal_alloc((num_flowcheckers *
              sizeof(bcmi_flowtracker_flowchecker_info_t *)),
             "ft_flowchecker_sw_state");
    if (BCMI_FT_FLOWCHECKER_STATE(unit) == NULL) {
        goto error;
    }
    sal_memset(BCMI_FT_FLOWCHECKER_STATE(unit), 0,
            (num_flowcheckers * sizeof(bcmi_ft_flowchecker_sw_state_t *)));

    /* allocate Group bitmap for whole chip. */
    BCMI_FT_FLOWCHECKER_BITMAP(unit) =
        sal_alloc(SHR_BITALLOCSIZE(num_flowcheckers),
                "ft_flowchecker_bitmap");
    if (BCMI_FT_FLOWCHECKER_BITMAP(unit) == NULL) {
        goto error;
    }
    sal_memset(BCMI_FT_FLOWCHECKER_BITMAP(unit), 0,
            SHR_BITALLOCSIZE(num_flowcheckers));

    /* Allocate memory for refcounting the flowchecker indexes. */
    BCMI_FT_FLOWCHECKER_REFCOUNT_STATE(unit) = (uint32 *)
        sal_alloc((num_flowcheckers * sizeof(uint32)),
                "ft_flowchecker_refcount_state");
    if (BCMI_FT_FLOWCHECKER_REFCOUNT_STATE(unit) == NULL) {
        goto error;
    }
    sal_memset(BCMI_FT_FLOWCHECKER_REFCOUNT_STATE(unit), 0,
                       (num_flowcheckers * sizeof(uint32)));

    return BCM_E_NONE;

error:
    bcmi_ft_flowchecker_cleanup(unit);
    return BCM_E_MEMORY;
}

/*
 * Function:
 *   bcmi_ft_flowchecker_cleanup
 * Purpose:
 *   Cleanup software state of flowtracker checks.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   None.
 */
void
bcmi_ft_flowchecker_cleanup(int unit)
{
    int index = 0;
    int num_flowcheckers = 0;

    /* Total number of groups in device. */
    num_flowcheckers = BCMI_FT_FLOWCHECKER_MAX_ID(unit);

    if (BCMI_FT_FLOWCHECKER_REFCOUNT_STATE(unit) != NULL) {
        sal_free(BCMI_FT_FLOWCHECKER_REFCOUNT_STATE(unit));
        BCMI_FT_FLOWCHECKER_REFCOUNT_STATE(unit) = NULL;
    }

    if (BCMI_FT_FLOWCHECKER_BITMAP(unit) != NULL) {
        sal_free(BCMI_FT_FLOWCHECKER_BITMAP(unit));
        BCMI_FT_FLOWCHECKER_BITMAP(unit) = NULL;
    }

    if (BCMI_FT_FLOWCHECKER_STATE(unit) != NULL) {

        /* Remove checkers added first if any */
        for(index = 0; index < num_flowcheckers; index++) {
            if (BCMI_FT_FLOWCHECKER_INFO(unit, index) != NULL) {
                sal_free(BCMI_FT_FLOWCHECKER_INFO(unit, index));
                BCMI_FT_FLOWCHECKER_INFO(unit, index) = NULL;
            }
        }

        sal_free(BCMI_FT_FLOWCHECKER_STATE(unit));
        BCMI_FT_FLOWCHECKER_STATE(unit) = NULL;
    }

    BCMI_FT_FLOWCHECKER_MAX_ID(unit) = 0;
}

/*
 * Function:
 *   bcmi_ft_check_create
 * Purpose:
 *   Create flowtracker index from information
 *   provided by user.
 * Parameters:
 *   unit - (IN) BCM device id
 *   options - (IN) options to create flowtracker check.
 *   flow_check_info - (IN) information to create flowtracker check.
 *   flowchecker_id - (OUT) Placeholder for index generated.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_check_create(int unit, uint32 options,
                     bcm_flowtracker_check_info_t flow_check_info,
                     bcm_flowtracker_check_t *flowchecker_id)
{
    int index = -1;
    uint16 ft_type_bmp = 0;
    uint16 temp_ft_type_bmp = 0;
    bcmi_flowtracker_flowchecker_info_t fc_info;

    /* Check if input params are correct. */
    if (flowchecker_id == NULL) {
        return BCM_E_PARAM;
    }

    /* validate checker params */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_check_param_sanity(unit, options,
                                    flow_check_info, &ft_type_bmp));

    if (options & BCM_FLOWTRACKER_CHECK_WITH_ID) {
        if (!BCMI_FT_IDX_IS_CHECK(*flowchecker_id)) {
            return BCM_E_PARAM;
        }

        /* get the index from bitmap. */
        index = BCMI_FT_FLOWCHECKER_INDEX_GET(*flowchecker_id);

        if (!(bcmi_ft_flowchecker_idx_is_invalid(unit, index))) {
            return BCM_E_PARAM;
        }
    }

    if (options & BCM_FLOWTRACKER_CHECK_SECOND) {
        if (!BCMI_FT_IDX_IS_CHECK(flow_check_info.primary_check_id)) {
            return BCM_E_PARAM;
        }

        sal_memset(&fc_info, 0, sizeof(bcmi_flowtracker_flowchecker_info_t));

        BCM_IF_ERROR_RETURN
            (bcmi_ft_flowchecker_get(unit,
                                     flow_check_info.primary_check_id,
                                     &fc_info));

        /* Dual operations used in first check.*/
        if (bcmi_ft_flowchecker_opr_is_dual(unit, fc_info.check1.operation)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                "Dual Operation used in primary check.\n")));
            return BCM_E_CONFIG;
        }

        /* Secondary check already added for provided primary check id */
        if (fc_info.check2.operation) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                "Secondary Check already exists for this primary.\n")));
            return BCM_E_EXISTS;
        }

        /* Not allowed to create a second check if dos attack is updated.*/
        if (soc_feature(unit, soc_feature_hx5_ft_32bit_param_update)) {
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

        if ((bcmi_ft_flowchecker_idx_is_invalid(unit, index))) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                "The primary check is not created or invalid.\n")));
            return BCM_E_PARAM;
        }

        /* If Primary is associated with any FT Group, we should not allow
         * added second to it. */
        if (BCMI_FT_FLOWCHECKER_REFCOUNT(unit, index)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
               "The primary check is already attached to a group.\n")));
            return BCM_E_CONFIG;
        }

        /* validate if secondary check belongs to same stage as primary */
        temp_ft_type_bmp = fc_info.check_ft_type_bmp & ft_type_bmp;
        if (temp_ft_type_bmp == 0) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                            "Primary & Secondary checker cannot be created across"
                            " different pipeline stages.\n")));
            return BCM_E_PARAM;
        } else {
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check_ft_type_bmp =
                temp_ft_type_bmp;
        }

        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.param   =
                                               flow_check_info.param;
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.min_value =
                                               flow_check_info.min_value;
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.max_value =
                                               flow_check_info.max_value;
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.mask_value =
                                               flow_check_info.mask_value;
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.shift_value =
                                               flow_check_info.shift_value;
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.operation =
                                               flow_check_info.operation;
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.flags =
                                               flow_check_info.flags;
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.num_drop_reason_group_id =
                                  flow_check_info.num_drop_reason_group_id;
        sal_memcpy(BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.drop_reason_group_id,
                                  flow_check_info.drop_reason_group_id,
                                 (sizeof(bcm_flowtracker_drop_reason_group_t) *
                                  BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX));
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->flags |=
                                               flow_check_info.flags;
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.custom_id =
                                               flow_check_info.custom_id;

        /* set flowchecker type in the index. */
        *flowchecker_id =
            BCMI_FT_FLOWCHECKER_INDEX_SET(index, BCM_FT_IDX_TYPE_2ND_CHECK);

        return BCM_E_NONE;
    }

    /* get free index */
    BCM_IF_ERROR_RETURN
          (bcmi_ft_flowchecker_get_free_index(unit, &index));

    /* Allocate memory for the new flowchecker. */
    BCMI_FT_FLOWCHECKER_INFO(unit, index) =
        (bcmi_flowtracker_flowchecker_info_t *)
        sal_alloc(sizeof(bcmi_flowtracker_flowchecker_info_t),
        "flowtracker info");

    if (BCMI_FT_FLOWCHECKER_INFO(unit, index) == NULL) {
        SHR_BITCLR(BCMI_FT_FLOWCHECKER_BITMAP(unit), index);
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
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.mask_value =
                                           flow_check_info.mask_value;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.shift_value =
                                           flow_check_info.shift_value;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.operation =
                                           flow_check_info.operation;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.flags =
                                           flow_check_info.flags;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.num_drop_reason_group_id =
                                           flow_check_info.num_drop_reason_group_id;
    sal_memcpy(BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.drop_reason_group_id,
                                   flow_check_info.drop_reason_group_id,
                                  (sizeof(bcm_flowtracker_drop_reason_group_t) *
                                   BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX));
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.state_transition_flags =
                                            flow_check_info.state_transition_flags;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->flags =
                                           flow_check_info.flags;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->check_ft_type_bmp =
                                           ft_type_bmp;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.custom_id =
                                           flow_check_info.custom_id;

    /* set flowchecker type in the index. */
    *flowchecker_id =
            BCMI_FT_FLOWCHECKER_INDEX_SET(index, BCM_FT_IDX_TYPE_CHECK);

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_check_get
 * Purpose:
 *   Get flowtracker information from the index
 *   provided by user. This is user interfacing method
 *   which will only send particular info which user API exposes.
 * Parameters:
 *   unit            - (IN) BCM device id
 *   flowchecker_id  - (IN) flowtracker check index.
 *   flow_check_info - (OUT) information to create flowtracker check.
 * Returns:
 *   BCM_E_XXX.
 */
int bcmi_ft_check_get(int unit,
                      bcm_flowtracker_check_t flowchecker_id,
                      bcm_flowtracker_check_info_t *flow_check_info,
                      uint16 *ft_type_bmp)
{
    int index = 0;

    /* Check if input params are correct */
    if (!flow_check_info) {
        return BCM_E_PARAM;
    }

    /* validate flowchecker id */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_flowchecker_id_is_invalid(unit, flowchecker_id));

    /* get the index from bitmap. */
    index = BCMI_FT_FLOWCHECKER_INDEX_GET(flowchecker_id);

    sal_memset(flow_check_info, 0, sizeof(bcm_flowtracker_check_info_t));

    if (BCMI_FT_IDX_IS_2ND_CHECK(flowchecker_id)) {
        /* Check if second check is configured. */
        if (!BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.operation) {
            return BCM_E_NOT_FOUND;
        }

        flow_check_info->flags =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.flags;
        flow_check_info->param     =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.param;
        flow_check_info->min_value =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.min_value;
        flow_check_info->max_value =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.max_value;
        flow_check_info->mask_value =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.mask_value;
        flow_check_info->shift_value =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.shift_value;
        flow_check_info->operation =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.operation;
        flow_check_info->flags =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.flags;
        flow_check_info->primary_check_id =
            BCMI_FT_FLOWCHECKER_INDEX_SET(index, BCM_FT_IDX_TYPE_CHECK);
        flow_check_info->num_drop_reason_group_id =
          BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.num_drop_reason_group_id;
        sal_memcpy(flow_check_info->drop_reason_group_id,
          BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.drop_reason_group_id,
                (sizeof(bcm_flowtracker_drop_reason_group_t) *
                 BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX));
        flow_check_info->custom_id =
                BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.custom_id;

    } else {
        flow_check_info->flags =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.flags;
        flow_check_info->param     =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.param;
        flow_check_info->min_value =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.min_value;
        flow_check_info->max_value =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.max_value;
        flow_check_info->mask_value =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.mask_value;
        flow_check_info->shift_value =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.shift_value;
        flow_check_info->operation =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.operation;
        flow_check_info->flags =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.flags;
        flow_check_info->num_drop_reason_group_id =
         BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.num_drop_reason_group_id;
        sal_memcpy(flow_check_info->drop_reason_group_id,
           BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.drop_reason_group_id,
                (sizeof(bcm_flowtracker_drop_reason_group_t) *
                 BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX));
        flow_check_info->custom_id =
                BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.custom_id;

    }

    if (ft_type_bmp != NULL) {
        *ft_type_bmp =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check_ft_type_bmp;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_check_traverse
 * Purpose:
 *   Traverse througth all the flowtracker check objects.
 * Parameters:
 *   unit      - (IN) BCM device id
 *   cb        - (IN) Callback
 *   user_data - (IN/OUT) User data which is sent back.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_check_traverse(int unit,
                       bcm_flowtracker_check_traverse_cb cb,
                       void *user_data)
{
    int index = 0, num_flowcheckers = 0;
    bcm_flowtracker_check_t flowchecker_id;
    bcm_flowtracker_check_info_t flow_check_info;

    /* Total number of groups in device. */
    num_flowcheckers = BCMI_FT_FLOWCHECKER_MAX_ID(unit);

    if (cb == NULL) {
        return BCM_E_PARAM;
    }

    for(; index < num_flowcheckers; index++) {

        if (bcmi_ft_flowchecker_idx_is_invalid(unit, index)) {
            continue;
        }
        sal_memset(&flow_check_info, 0, sizeof(bcm_flowtracker_check_info_t));

        flowchecker_id =
            BCMI_FT_FLOWCHECKER_INDEX_SET(index, BCM_FT_IDX_TYPE_CHECK);
        flow_check_info.flags =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.flags;
        flow_check_info.param     =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.param;
        flow_check_info.min_value =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.min_value;
        flow_check_info.max_value =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.max_value;
        flow_check_info.mask_value =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.mask_value;
        flow_check_info.shift_value =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.shift_value;
        flow_check_info.operation =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.operation;
        flow_check_info.num_drop_reason_group_id =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.num_drop_reason_group_id;
        sal_memcpy(flow_check_info.drop_reason_group_id,
                BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.drop_reason_group_id,
                (sizeof(bcm_flowtracker_drop_reason_group_t) *
                 BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX));

        /* Invoke call back routine. */
        (*cb)(unit, flowchecker_id, &flow_check_info, user_data);

        /* Now send second check information. */
        if (!BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.param) {
            continue;
        }

        sal_memset(&flow_check_info, 0, sizeof(bcm_flowtracker_check_info_t));

        flowchecker_id =
            BCMI_FT_FLOWCHECKER_INDEX_SET(index, BCM_FT_IDX_TYPE_2ND_CHECK);

        flow_check_info.flags =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.flags;
        flow_check_info.param     =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.param;
        flow_check_info.min_value =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.min_value;
        flow_check_info.max_value =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.max_value;
        flow_check_info.mask_value =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.mask_value;
        flow_check_info.shift_value =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.shift_value;
        flow_check_info.operation =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.operation;
        flow_check_info.num_drop_reason_group_id =
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.num_drop_reason_group_id;
        sal_memcpy(flow_check_info.drop_reason_group_id,
                BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.drop_reason_group_id,
                (sizeof(bcm_flowtracker_drop_reason_group_t) *
                 BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX));

        flow_check_info.primary_check_id =
            BCMI_FT_FLOWCHECKER_INDEX_SET(index, BCM_FT_IDX_TYPE_CHECK);

        /* Invoke call back routine. */
        (*cb)(unit, flowchecker_id, &flow_check_info, user_data);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_flowchecker_get
 * Purpose:
 *   Get flowtracker information from the index
 *   provided by user. This is internal function
 *   which provides all the internal data.
 * Parameters:
 *   unit            - (IN) BCM device id
 *   flowchecker_id  - (IN) flowtracker check index.
 *   flow_check_info - (OUT) information to create flowtracker check.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_flowchecker_get(int unit,
                        bcm_flowtracker_check_t flowchecker_id,
                        bcmi_flowtracker_flowchecker_info_t *flow_check_info)
{
    int index = 0;

    /* Check if input params are correct. */
    if (!flow_check_info) {
        return BCM_E_PARAM;
    }

    /* If INTERNAL Flowcheck */
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        if (BCMI_FT_IDX_IS_INT_CHECK(flowchecker_id)) {
            return bcmi_ftv2_internal_flowchecker_get(unit,
                                flowchecker_id, flow_check_info);
        }
    }
#endif

    /* validate flowchecker id */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_flowchecker_id_is_invalid(unit, flowchecker_id));

    /* get the index from bitmap. _ft_check_compare*/
    index = BCMI_FT_FLOWCHECKER_INDEX_GET(flowchecker_id);

    flow_check_info->check_ft_type_bmp =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check_ft_type_bmp;
    flow_check_info->flags =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->flags;

    /* Data of first check. */
    flow_check_info->check1.flags =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.flags;
    flow_check_info->check1.param     =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.param;
    flow_check_info->check1.min_value =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.min_value;
    flow_check_info->check1.max_value =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.max_value;
    flow_check_info->check1.mask_value =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.mask_value;
    flow_check_info->check1.shift_value =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.shift_value;
    flow_check_info->check1.operation =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.operation;
    flow_check_info->check1.num_drop_reason_group_id =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.num_drop_reason_group_id;
    sal_memcpy(flow_check_info->check1.drop_reason_group_id,
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.drop_reason_group_id,
            (sizeof(bcm_flowtracker_drop_reason_group_t) *
             BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX));
    flow_check_info->check1.state_transition_flags =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.state_transition_flags;
    flow_check_info->check1.custom_id =
                BCMI_FT_FLOWCHECKER_INFO(unit, index)->check1.custom_id;

    /* Data of second check. */
    flow_check_info->check2.flags =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.flags;
    flow_check_info->check2.param     =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.param;
    flow_check_info->check2.min_value =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.min_value;
    flow_check_info->check2.max_value =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.max_value;
    flow_check_info->check2.mask_value =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.mask_value;
    flow_check_info->check2.shift_value =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.shift_value;
    flow_check_info->check2.operation =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.operation;
    flow_check_info->check2.num_drop_reason_group_id =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.num_drop_reason_group_id;
    sal_memcpy(flow_check_info->check2.drop_reason_group_id,
            BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.drop_reason_group_id,
            (sizeof(bcm_flowtracker_drop_reason_group_t) *
             BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX));
    flow_check_info->check2.custom_id =
                BCMI_FT_FLOWCHECKER_INFO(unit, index)->check2.custom_id;

    /* Action Info */
    flow_check_info->action_info.flags =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.flags;
    flow_check_info->action_info.param =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.param;
    flow_check_info->action_info.action  =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.action;
    flow_check_info->action_info.mask_value  =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.mask_value;
    flow_check_info->action_info.shift_value  =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.shift_value;
    flow_check_info->action_info.weight  =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.weight;
    flow_check_info->action_info.direction =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.direction;
    flow_check_info->action_info.custom_id =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.custom_id;

    /* Export Info */
    flow_check_info->export_info.export_check_threshold =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->
                    export_info.export_check_threshold;
    flow_check_info->export_info.operation =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->export_info.operation;

    /* Delay Info */
    flow_check_info->delay_info.src_ts =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->delay_info.src_ts;
    flow_check_info->delay_info.dst_ts =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->delay_info.dst_ts;
    flow_check_info->delay_info.gran =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->delay_info.gran;
    flow_check_info->delay_info.offset =
        BCMI_FT_FLOWCHECKER_INFO(unit, index)->delay_info.offset;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_check_destroy
 * Purpose:
 *   Destroy flowtracker from the index
 *   provided by user.
 * Parameters:
 *   unit           - (IN) BCM device id
 *   flowchecker_id - (IN) Placeholder for index generated.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_check_destroy(int unit,
                      bcm_flowtracker_check_t flowchecker_id)
{
    int index = 0;

    /* validate flowchecker id */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_flowchecker_id_is_invalid(unit, flowchecker_id));

    /* get the index from bitmap. */
    index = BCMI_FT_FLOWCHECKER_INDEX_GET(flowchecker_id);

    /* For removing checks, they should be removed from FT groups. */
    if (BCMI_FT_FLOWCHECKER_REFCOUNT(unit, index)) {
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
    BCMI_FT_FLOWCHECKER_INFO(unit, index) = NULL;

    /* Clear the bit for flowchecker. */
    SHR_BITCLR(BCMI_FT_FLOWCHECKER_BITMAP(unit), index);

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_check_destroy_all
 * Purpose:
 *   Destroy all flowtrackers from the index
 *   provided by user which are not linked to any group.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_check_destroy_all(int unit)
{
    int index = 0;
    int num_flowcheckers = 0;

    /* Total number of groups in device. */
    num_flowcheckers = BCMI_FT_FLOWCHECKER_MAX_ID(unit);

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
 *   bcmi_ft_check_action_info_set
 * Purpose:
 *   Set action informaiton on the flowtracker check
 * Parameters:
 *   unit - (IN) BCM device id
 *   flowchecker_id - (IN) flowtracker check index.
 *   info - (IN) action information.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_check_action_info_set(int unit,
                              bcm_flowtracker_check_t check_id,
                              bcm_flowtracker_check_action_info_t info)
{
    int index = 0;

    /* validate flowchecker id */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_flowchecker_id_is_invalid(unit, check_id));

    /* validate if it is a secondary flowchecker id */
    if (BCMI_FT_IDX_IS_2ND_CHECK(check_id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
           "Actions can only be set on primary check.\n")));
        return BCM_E_PARAM;
    }

    /* get the index from check id. */
    index = BCMI_FT_FLOWCHECKER_INDEX_GET(check_id);

    /* sanitize action info provided */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_check_action_info_sanity(unit, index, info));

    /* update info in internal structure */
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.flags = info.flags;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.param = info.param;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.action  = info.action;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.mask_value  =
                                                             info.mask_value;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.shift_value  =
                                                             info.shift_value;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.weight = info.weight;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.direction =
                                                             info.direction;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.custom_id =
                                                             info.custom_id;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_check_action_info_get
 * Purpose:
 *   Get action informaiton from the flowtracker check
 * Parameters:
 *   unit - (IN) BCM device id
 *   flowchecker_id - (IN) flowtracker check index.
 *   info - (OUT) action information.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_check_action_info_get(int unit,
                              bcm_flowtracker_check_t check_id,
                              bcm_flowtracker_check_action_info_t *info)
{
    int index = 0;

    /* validate flowchecker id */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_flowchecker_id_is_invalid(unit, check_id));

    /* validate if it is a secondary flowchecker id */
    if (BCMI_FT_IDX_IS_2ND_CHECK(check_id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "Actions can only be retrieved on primary check.\n")));
        return BCM_E_PARAM;
    }

    /* get the index from check id */
    index = BCMI_FT_FLOWCHECKER_INDEX_GET(check_id);

    /* update info in internal structure */
    info->flags = BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.flags;
    info->param = BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.param;
    info->action = BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.action;
    info->mask_value =
               BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.mask_value;
    info->shift_value =
               BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.shift_value;
    info->weight =
               BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.weight;
    info->direction =
               BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.direction;
    info->custom_id =
               BCMI_FT_FLOWCHECKER_INFO(unit, index)->action_info.custom_id;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_check_export_info_set
 * Purpose:
 *   Set export information from the flowtracker check
 * Parameters:
 *   unit           - (IN) BCM device id
 *   flowchecker_id - (IN) flowtracker check id
 *   info           - (IN) export information
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_check_export_info_set(int unit,
                              bcm_flowtracker_check_t check_id,
                              bcm_flowtracker_check_export_info_t info)
{
    int index = 0;

    /* validate flowchecker id */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_flowchecker_id_is_invalid(unit, check_id));

    /* validate if it is a secondary flowchecker id */
    if (BCMI_FT_IDX_IS_2ND_CHECK(check_id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "Export info can only be set on primary check.\n")));
        return BCM_E_PARAM;
    }

    /* get the index from bitmap. */
    index = BCMI_FT_FLOWCHECKER_INDEX_GET(check_id);

    /* sanitize export info provided */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_check_export_info_sanity(unit, index, info));

    /* update info in internal structure */
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->export_info.export_check_threshold =
                                                    info.export_check_threshold;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->export_info.operation =
                                                    info.operation;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_check_export_info_get
 * Purpose:
 *   Get export information from the flowtracker check
 * Parameters:
 *   unit           - (IN) BCM device id
 *   flowchecker_id - (IN) flowtracker check index.
 *   info           - (OUT) export information.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_check_export_info_get(int unit,
                              bcm_flowtracker_check_t check_id,
                              bcm_flowtracker_check_export_info_t *info)
{
    int index = 0;

    /* validate flowchecker id */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_flowchecker_id_is_invalid(unit, check_id));

    /* validate if it is a secondary flowchecker id */
    if (BCMI_FT_IDX_IS_2ND_CHECK(check_id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
          "Export info can only be retrieved from primary check.\n")));
        return BCM_E_PARAM;
    }

    /* get the index from bitmap. */
    index = BCMI_FT_FLOWCHECKER_INDEX_GET(check_id);

    info->export_check_threshold =
      BCMI_FT_FLOWCHECKER_INFO(unit, index)->export_info.export_check_threshold;
    info->operation =
      BCMI_FT_FLOWCHECKER_INFO(unit, index)->export_info.operation;

    return BCM_E_NONE;
}

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
/*
 * Function:
 *   bcmi_ftv2_check_delay_info_set
 * Purpose:
 *   Set delay information from the flowtracker check
 * Parameters:
 *   unit           - (IN) BCM device id
 *   flowchecker_id - (IN) flowtracker check id
 *   info           - (IN) delay information
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ftv2_check_delay_info_set(int unit,
                               bcm_flowtracker_check_t check_id,
                               bcm_flowtracker_check_delay_info_t info)
{
    int index = 0;

    /* validate flowchecker id */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_flowchecker_id_is_invalid(unit, check_id));

    /* validate if it is a secondary flowchecker id */
    if (BCMI_FT_IDX_IS_2ND_CHECK(check_id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                 "Delay info can only be set on primary check.\n")));
        return BCM_E_PARAM;
    }

    /* get the index from bitmap. */
    index = BCMI_FT_FLOWCHECKER_INDEX_GET(check_id);

    /* sanitize delay info provided */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_check_delay_info_sanity(unit, index, info));

    /* update info in internal structure */
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->delay_info.src_ts = info.src_ts;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->delay_info.dst_ts = info.dst_ts;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->delay_info.gran = info.gran;
    BCMI_FT_FLOWCHECKER_INFO(unit, index)->delay_info.offset = info.offset;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv2_check_delay_info_get
 * Purpose:
 *   Get delay information from the flowtracker check
 * Parameters:
 *   unit           - (IN)  BCM device id
 *   flowchecker_id - (IN)  flowtracker check index.
 *   info           - (OUT) delay information.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ftv2_check_delay_info_get(int unit,
                               bcm_flowtracker_check_t check_id,
                               bcm_flowtracker_check_delay_info_t *info)
{
    int index = 0;

    /* validate flowchecker id */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_flowchecker_id_is_invalid(unit, check_id));

    /* validate if it is a secondary flowchecker id */
    if (BCMI_FT_IDX_IS_2ND_CHECK(check_id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
           "Delay info can only be retrieved from primary check.\n")));
        return BCM_E_PARAM;
    }

    /* get the index from bitmap. */
    index = BCMI_FT_FLOWCHECKER_INDEX_GET(check_id);

    /* update info in internal structure */
    info->src_ts = BCMI_FT_FLOWCHECKER_INFO(unit, index)->delay_info.src_ts;
    info->dst_ts = BCMI_FT_FLOWCHECKER_INFO(unit, index)->delay_info.dst_ts;
    info->gran = BCMI_FT_FLOWCHECKER_INFO(unit, index)->delay_info.gran;
    info->offset = BCMI_FT_FLOWCHECKER_INFO(unit, index)->delay_info.offset;

    return BCM_E_NONE;
}
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

/*
 * Function:
 *   bcmi_ft_flowchecker_list_length_get
 * Purpose:
 *   Get length of flowchecker list in group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   head - (IN/OUT) Head pointer of the list.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_flowchecker_list_length_get(int unit,
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
 *   bcmi_ft_flowchecker_list_add
 * Purpose:
 *   Add this flowtracker into the group.
 * Parameters:
 *   unit           - (IN) BCM device id
 *   head           - (INOUT) Head pointer of the list.
 *   flowchecker_id - (IN) flowtracker check index.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_flowchecker_list_add(int unit,
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
            return BCM_E_EXISTS;
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
 *   bcmi_ft_flowchecker_list_delete
 * Purpose:
 *   Delete this flowtracker check from group list.
 * Parameters:
 *   unit - (IN) BCM device id
 *   head - (INOUT) Head pointer of the list.
 *   flowchecker_id - (IN) flowtracker check index.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_flowchecker_list_delete(int unit,
                                bcmi_ft_flowchecker_list_t **head,
                                bcm_flowtracker_check_t flowchecker_id)
{
    bcmi_ft_flowchecker_list_t *temp = NULL;
    bcmi_ft_flowchecker_list_t *prev = NULL;

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
 *   bcmi_ft_check_parts_get
 * Purpose:
 *   Get check parts for given primary check.
 * Parameters:
 *   unit - (IN) BCM device id
 *   flowchecker_id - (IN) flowtracker check index.
 *   check_ids - (OUT) Pointer to check Ids
 *   num_parts - (OUT) Number of parts
 * Returns:
 *   BCM_E_XXX.
 */
STATIC int
bcmi_ft_check_parts_get(int unit,
                        bcm_flowtracker_check_t flowchecker_id,
                        bcm_flowtracker_check_t *check_ids,
                        int *num_parts)
{
    int part = 0, index = 0;
    bcmi_flowtracker_flowchecker_info_t fc_info;

    sal_memset(&fc_info, 0, sizeof(bcmi_flowtracker_flowchecker_info_t));

    BCM_IF_ERROR_RETURN
        (bcmi_ft_flowchecker_get(unit,
                                 flowchecker_id, &fc_info));

    /* Adding primary */
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
 *   bcmi_ft_group_check_add_check
 * Purpose:
 *   Verify flowcheck before adding it to FT Group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id  - (IN) flowtracker group index.
 *   flowchecker_id - (IN) flowtracker check index.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_group_check_add_check(int unit,
                              bcm_flowtracker_group_t id,
                              bcm_flowtracker_check_t flowchecker_id)
{
    uint16 group_ft_type_bmp = 0;
    bcm_flowtracker_group_type_t group_type;
    bcmi_flowtracker_flowchecker_info_t fc_info;

    sal_memset(&fc_info, 0, sizeof(bcmi_flowtracker_flowchecker_info_t));

    if (!BCMI_FT_IDX_IS_PRIMARY_CHECK(flowchecker_id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, "Given check"
                        " is not primary\n")));
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(bcmi_ft_flowchecker_get
                        (unit, flowchecker_id, &fc_info));

    group_type = BCMI_FT_GROUP_TYPE_GET(id);
    BCMI_FT_GROUP_FT_TYPE_BMP_GET(group_type, group_ft_type_bmp);

    if (!(group_ft_type_bmp & fc_info.check_ft_type_bmp)) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit, "FlowCheck (%d) does not belong"
                            " flowtracker group(%d) stage."),
                 flowchecker_id, id));
        return BCM_E_CONFIG;
    }

    if ((fc_info.action_info.direction == bcmFlowtrackerFlowDirectionReverse) ||
        (fc_info.action_info.direction == bcmFlowtrackerFlowDirectionBidirectional)) {

        if (BCMI_FT_GROUP_FTFP_DATA(unit, id).direction !=
                bcmFlowtrackerFlowDirectionBidirectional) {
            LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                    (BSL_META_U(unit, "FlowCheck (%d) direction not supported"
                                " for flowtracker group(%d).\r\n"),
                     flowchecker_id, id));
            return BCM_E_CONFIG;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_flowchecker_add
 * Purpose:
 *   Add this flowtracker check into group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id  - (IN) flowtracker group index.
 *   flowchecker_id - (IN) flowtracker check index.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_group_flowchecker_add(int unit,
                              bcm_flowtracker_group_t id,
                              bcm_flowtracker_check_t flowchecker_id)
{
    int rv = BCM_E_NONE;
    int num_parts = 0;
    int part = 0, index = 0;
    bcm_flowtracker_check_t check_ids[2];
    bcmi_ft_flowchecker_list_t **head = NULL;

    /* validate flow group id. */
    if(bcmi_ft_group_is_invalid(unit, id)) {
        return (BCM_E_PARAM);
    }

    /* validate flowchecker id */
    BCM_IF_ERROR_RETURN
       (bcmi_ft_flowchecker_id_is_invalid(unit, flowchecker_id));

    /* get the index from bitmap. */
    index = BCMI_FT_FLOWCHECKER_INDEX_GET(flowchecker_id);

    if (BCMI_FT_GROUP_IS_BUSY(unit, id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, "Adding check to"
           " installed Group (%d) is not allowed.\n"), id));
        return BCM_E_CONFIG;
    }

    /* Check if flowcheck can be added to group */
    rv = bcmi_ft_group_check_add_check(unit, id, flowchecker_id);
    BCM_IF_ERROR_RETURN(rv);

    /* Get associated flowchecks */
    rv = bcmi_ft_check_parts_get(unit, flowchecker_id, check_ids, &num_parts);
    BCM_IF_ERROR_RETURN(rv);

    /* Add to flowchecker list of group */
    head = &(BCMI_FT_GROUP_FLOWCHECKER_HEAD(unit, id));
    for (part = 0; (part < num_parts) && BCM_SUCCESS(rv); part++) {
        rv = bcmi_ft_flowchecker_list_add(unit, head, check_ids[part]);
    }

    if (BCM_FAILURE(rv) && (rv != BCM_E_EXISTS)) {
        goto cleanup;
    }

    /* If added succesfully for first time, incr ref count and
     * set triggers. */
    if (rv != BCM_E_EXISTS) {
        if (!BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
            rv = bcmi_ft_group_check_ts_set(unit, id,
                    flowchecker_id, bcmFlowtrackerFlowCheckTsSet);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }

        BCMI_FT_FLOWCHECKER_REFCOUNT_INC(unit, index);
    }

    return rv;

cleanup:
    for (part = 0; part < num_parts; part++) {
        bcmi_ft_flowchecker_list_delete(unit, head, check_ids[part]);
    }
    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_flowchecker_delete
 * Purpose:
 *   Delete this flowtracker check from group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id  - (IN) flowtracker group index.
 *   flowchecker_id - (IN) flowtracker check index.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_group_flowchecker_delete(int unit,
                                 bcm_flowtracker_group_t id,
                                 bcm_flowtracker_check_t flowchecker_id)
{
    int rv = BCM_E_NONE;
    int index;
    int part = 0;
    int num_parts = 0;
    bcm_flowtracker_check_t check_ids[2];
    bcmi_ft_flowchecker_list_t **head = NULL;

    /* validate flow group index. */
    if (bcmi_ft_group_is_invalid(unit, id)) {
        return (BCM_E_PARAM);
    }

    /* validate flowchecker id */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_flowchecker_id_is_invalid(unit, flowchecker_id));

    /* get the index from bitmap. */
    index = BCMI_FT_FLOWCHECKER_INDEX_GET(flowchecker_id);

    if (BCMI_FT_GROUP_IS_BUSY(unit, id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, "Please uninstall"
           " Group (%d). operation is not allowed.\n"), id));
        return BCM_E_CONFIG;
    }

    if (!BCMI_FT_IDX_IS_PRIMARY_CHECK(flowchecker_id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, "Given check"
                        " is not primary.\n")));
        return BCM_E_PARAM;
    }

    /* Get associated flowchecks */
    rv = bcmi_ft_check_parts_get(unit, flowchecker_id, check_ids, &num_parts);
    BCM_IF_ERROR_RETURN(rv);

    head = &(BCMI_FT_GROUP_FLOWCHECKER_HEAD(unit, id));
    for (part = 0; (part < num_parts) && (BCM_SUCCESS(rv)); part++) {
        rv = bcmi_ft_flowchecker_list_delete(unit, head, check_ids[part]);
    }
    BCM_IF_ERROR_RETURN(rv);

    /* reset check timestamps */
    rv = bcmi_ft_group_check_ts_set(unit, id, flowchecker_id,
                                    bcmFlowtrackerFlowCheckTsReset);
    BCM_IF_ERROR_RETURN(rv);

    BCMI_FT_FLOWCHECKER_REFCOUNT_DEC(unit, index);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_flowchecker_get_all
 * Purpose:
 *   Get all the flowtracker checks from group.
 * Parameters:
 *   unit              - (IN) BCM device id
 *   id                - (IN) flowtracker group index.
 *   max_checks        - (IN) max number of flowtracker check ids to be sent..
 *   list_of_check_ids - (IN) list of the flowtracker check ids.
 *   num_checks        - NUmber of checks actually sent.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_flowtracker_check_get_all(int unit,
                               bcm_flowtracker_group_t id,
                               int skip_non_primary,
                               int max_checks,
                               bcm_flowtracker_check_t *list_of_check_ids,
                               int *num_checks)
{
    int i = 0, only_count = 0;
    bcmi_ft_flowchecker_list_t *temp;

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
 *   bcmi_ft_check_compare
 * Purpose:
 *   Compare two flowtracker checks.
 * Parameters:
 *   unit      - (IN) BCM device id
 *   check_id1 - (IN) first flowtracker check index.
 *   check_id2 - (IN) Second flowtracker check index.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_check_compare(int unit,
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
    if (bcmi_ft_flowchecker_idx_is_invalid(unit,
            BCMI_FT_FLOWCHECKER_INDEX_GET(check_id1))) {
        return BCM_E_NOT_FOUND;
    }

    /* Validate flow checker index. */
    if (bcmi_ft_flowchecker_idx_is_invalid(unit,
            BCMI_FT_FLOWCHECKER_INDEX_GET(check_id2))) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN
        (bcmi_ft_flowchecker_get(unit, check_id1, &fc_info1));

    BCM_IF_ERROR_RETURN
        (bcmi_ft_flowchecker_get(unit, check_id2, &fc_info2));

    /* Compare the flow checks and if same then return MATCH. */
    if (!(sal_memcmp(&fc_info1, &fc_info2,
                sizeof(bcmi_flowtracker_flowchecker_info_t)))) {
       return BCM_E_NONE;
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *   bcmi_ft_check_list_compare
 * Purpose:
 *   Compare all the checks in group list.
 * Parameters:
 *   unit     - (IN) BCM device id
 *   head     - (IN) Head of the flowchecker list.
 *   check_id - (IN) flowtracker check index.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_check_list_compare(int unit,
                           bcmi_ft_flowchecker_list_t **head,
                           bcm_flowtracker_check_t check_id)
{
    bcmi_ft_flowchecker_list_t *temp = NULL;

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
 *   bcmi_ft_group_check_validate
 * Purpose:
 *   Validate Flowcheck if given configuration is allowed
 *   based in options params.
 * Parameters:
 *   unit     - (IN) BCM device id
 *   options  - (IN) Match criteria
 *   id       - (IN) Flowtracker group
 *   check_id - (IN) flowtracker check
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_group_check_validate(int unit,
                             uint32 options,
                             bcm_flowtracker_group_t id,
                             bcm_flowtracker_check_t check_id)
{
    int found = FALSE;
    bcmi_ft_flowchecker_list_t *head = NULL;
    bcmi_ft_flowchecker_list_t *temp = NULL;

    /* validate flowchecker id */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_flowchecker_id_is_invalid(unit, check_id));

    if (options & BCMI_FT_GROUP_CHECK_MATCH_PRIMARY) {
        if (!BCMI_FT_IDX_IS_PRIMARY_CHECK(check_id)) {
            return BCM_E_PARAM;
        }
    }

    /* Search for checkid in the group */
    if(options & BCMI_FT_GROUP_CHECK_MATCH_GROUP) {

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

/*
 * Function:
 *   bcmi_ft_check_ft_type_bmp_set
 * Purpose:
 *   Compute ft_type_bmp for the Flowcheck based on check paramters.
 * Parameters:
 *   unit     - (IN) BCM device id
 *   fc_info  - (IN/OUT) Flowtracker check info
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_check_ft_type_bmp_set(int unit,
                              bcmi_flowtracker_flowchecker_info_t *fc_info)
{
    int rv = BCM_E_NONE;
    uint16 temp_ft_type_bmp = 0xFFFF;
    uint16 check_ft_type_bmp = 0xFFFF;

    if (fc_info == NULL) {
        return BCM_E_NONE;
    }

    /* Primary Check */
    if (fc_info->check1.param != bcmFlowtrackerTrackingParamTypeNone) {
        rv = _bcm_field_ft_tracking_param_type_get(unit,
                fc_info->check1.param, fc_info->check1.custom_id,
                0, &temp_ft_type_bmp);
    }
    BCM_IF_ERROR_RETURN(rv);
    check_ft_type_bmp &= temp_ft_type_bmp;

    /* Second Check */
    if (fc_info->check2.param != bcmFlowtrackerTrackingParamTypeNone) {
        rv = _bcm_field_ft_tracking_param_type_get(unit,
                fc_info->check2.param, fc_info->check2.custom_id,
                0, &temp_ft_type_bmp);
    }
    BCM_IF_ERROR_RETURN(rv);
    check_ft_type_bmp &= temp_ft_type_bmp;

    /* Action Info */
    if (fc_info->action_info.param != bcmFlowtrackerTrackingParamTypeNone) {
        rv = _bcm_field_ft_tracking_param_type_get(unit,
                fc_info->action_info.param, fc_info->action_info.custom_id,
                0, &temp_ft_type_bmp);
    }
    BCM_IF_ERROR_RETURN(rv);
    check_ft_type_bmp &= temp_ft_type_bmp;

    /* Flags */
    if ((fc_info->flags & BCM_FLOWTRACKER_CHECK_UNICAST_FLOW_ONLY) ||
            (fc_info->flags & BCM_FLOWTRACKER_CHECK_MULTICAST_FLOW_ONLY)) {
        temp_ft_type_bmp = (BCMI_FT_TYPE_F_MFT | BCMI_FT_TYPE_F_AMFT |
                BCMI_FT_TYPE_F_EFT | BCMI_FT_TYPE_F_AEFT);

        BCM_IF_ERROR_RETURN(rv);
        check_ft_type_bmp &= temp_ft_type_bmp;
    }

    /* AIFT ft type supports State transition flags */
    if (fc_info->check1.state_transition_flags) {
        temp_ft_type_bmp = (BCMI_FT_TYPE_F_AIFT);
    }
    BCM_IF_ERROR_RETURN(rv);

    check_ft_type_bmp &= temp_ft_type_bmp;

    fc_info->check_ft_type_bmp = check_ft_type_bmp;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_check_action_num_operands_get
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
bcmi_ft_check_action_num_operands_get(int unit,
                bcm_flowtracker_check_action_t action,
                int *num_operands)
{
    switch(action) {
        case bcmFlowtrackerCheckActionNone:
        case bcmFlowtrackerCheckActionCounterIncr:
        case bcmFlowtrackerCheckActionCounterIncrByPktBytes:
            *num_operands = 0;
            break;
        case bcmFlowtrackerCheckActionCounterIncrByValue:
        case bcmFlowtrackerCheckActionUpdateLowerValue:
        case bcmFlowtrackerCheckActionUpdateHigherValue:
        case bcmFlowtrackerCheckActionUpdateValue:
        case bcmFlowtrackerCheckActionUpdateAverageValue:
            *num_operands = 1;
            break;
        default:
            *num_operands = 0;
            break;
    }

    return BCM_E_NONE;
}

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
/*
 * Function:
 *   bcmi_ftv2_group_internal_flowcheck_create
 * Purpose:
 *   Create Internal flowcheck for the group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) Flowtracker Group.
 *   check_alu_info - (IN) alu_info to derive check data
 *   int_check_id - (OUT) Created Internal Flowcheck Id
 * Returns:
 *   None.
 */
int
bcmi_ftv2_group_internal_flowcheck_create(int unit,
                bcm_flowtracker_group_t id,
                bcm_flowtracker_tracking_param_type_t param,
                bcmi_ft_group_alu_info_t *check_alu_info,
                bcm_flowtracker_check_t *int_check_id)
{
    int rv = BCM_E_NONE;
    int upper16b_pos = 0, fc_idx = 0;
    bcmi_flowtracker_flowchecker_info_t *fc_info = NULL;

    BCMI_FT_ALLOC(fc_info,
            sizeof (bcmi_flowtracker_flowchecker_info_t),
            "InternalFlowCheck");
    if (fc_info == NULL) {
        return BCM_E_MEMORY;
    }

    if (soc_feature(unit, soc_feature_flowtracker_ts_64_sw_war_support)) {
        upper16b_pos =
            BCMI_FT_GROUP_EXT_HW_INFO_VFP_OPAQUEID2_START_BIT(unit, id);
    } else {
        upper16b_pos = BCMI_FT_TS_NTP64_UPPER16_LOC;
    }

    switch(param) {
        case bcmFlowtrackerTrackingParamTypeTimestampFlowStart:
            fc_idx = BCMI_FT_INT_CHECK_FLOWSTART_IDX;

            fc_info->check1.param = param;
            fc_info->check1.operation = bcmFlowtrackerCheckOpPass;
            fc_info->action_info.action = bcmFlowtrackerCheckActionUpdateValue;
            fc_info->check_ft_type_bmp |= BCMI_FT_TYPE_F_IFT;
            break;

        case bcmFlowtrackerTrackingParamTypeTimestampFlowEnd:
            fc_idx = BCMI_FT_INT_CHECK_FLOWEND_IDX;

            fc_info->check1.param = param;
            fc_info->check1.operation = bcmFlowtrackerCheckOpPass;
            fc_info->action_info.action = bcmFlowtrackerCheckActionUpdateValue;
            fc_info->check_ft_type_bmp |= BCMI_FT_TYPE_F_IFT;
            break;

        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1:
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2:
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent3:
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent4:

            if (param == bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1) {
                fc_idx = BCMI_FT_INT_CHECK_CHECKEVENT1_IDX;
            } else if (param == bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2) {
                fc_idx = BCMI_FT_INT_CHECK_CHECKEVENT2_IDX;
            } else if (param == bcmFlowtrackerTrackingParamTypeTimestampCheckEvent3) {
                fc_idx = BCMI_FT_INT_CHECK_CHECKEVENT3_IDX;
            } else {
                fc_idx = BCMI_FT_INT_CHECK_CHECKEVENT4_IDX;
            }

            rv = bcmi_ft_flowchecker_get(unit,
                        check_alu_info->flowchecker_id, fc_info);
            if (BCM_FAILURE(rv)) {
                sal_free(fc_info);
                return rv;
            }

            /* reset action, export and delay info */
            sal_memset(&fc_info->action_info, 0,
                    sizeof (bcm_flowtracker_check_action_info_t));
            sal_memset(&fc_info->export_info, 0,
                    sizeof (bcm_flowtracker_check_export_info_t));
            sal_memset(&fc_info->delay_info, 0,
                    sizeof (bcm_flowtracker_check_delay_info_t));

            fc_info->action_info.param = param;
            fc_info->action_info.action = bcmFlowtrackerCheckActionUpdateValue;
            if ((upper16b_pos % 32) != 0) {
                fc_info->action_info.shift_value = 16;
            }
            break;

        default:
            break;
    }

    if (BCMI_FT_GROUP(unit, id)->int_fc_info_arr[fc_idx]) {
        BCMI_FT_FREE(BCMI_FT_GROUP(unit, id)->int_fc_info_arr[fc_idx]);
    }
    BCMI_FT_GROUP(unit, id)->int_fc_info_arr[fc_idx] = fc_info;

    fc_idx = BCMI_FT_FLOWCHECKER_INDEX_SET(fc_idx, BCM_FT_IDX_TYPE_INTERNAL);
    *int_check_id = BCMI_FT_INT_CHECK_GROUP_IDX_SET(fc_idx, id);

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv2_internal_flowchecker_get
 * Purpose:
 *   Get Flowcheck info (for internal check only)
 * Parameters:
 *   unit - (IN) BCM device id
 *   flowchecker_id - (IN) Check Id
 *   fc_info - (OUT) Flowcheck Info
 * Returns:
 *   None.
 */
int
bcmi_ftv2_internal_flowchecker_get(int unit,
                    bcm_flowtracker_check_t flowchecker_id,
                    bcmi_flowtracker_flowchecker_info_t *fc_info)
{
    int fc_idx = 0;
    bcm_flowtracker_group_t id;

    if (!BCMI_FT_IDX_IS_INT_CHECK(flowchecker_id)) {
        return BCM_E_NONE;
    }

    fc_idx = BCMI_FT_INT_CHECK_IDX_GET(flowchecker_id);
    id = BCMI_FT_INT_CHECK_GROUP_IDX_GET(flowchecker_id);

    if (BCMI_FT_GROUP(unit, id)->int_fc_info_arr[fc_idx] != NULL) {
        sal_memcpy(fc_info, BCMI_FT_GROUP(unit, id)->int_fc_info_arr[fc_idx],
                        sizeof (bcmi_flowtracker_flowchecker_info_t));
    }

    return BCM_E_NONE;
}
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

#else /* BCM_FLOWTRACKER_SUPPORT*/
typedef int bcmi_ft_check_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_FLOWTRACKER_SUPPORT */
