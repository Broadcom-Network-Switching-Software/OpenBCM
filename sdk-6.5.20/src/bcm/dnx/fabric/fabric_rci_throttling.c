/** \file fabric_rci_throttling.c
 * 
 *
 * Fabric RCI throttling implementation for DNX. \n
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FABRIC

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>
#include <bcm/fabric.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/* dbal */
#include <soc/dnx/dbal/dbal.h>

/**
 * \brief - 
 * function for managing RCI throttling  mechanism mode
 *  
 * \param [in] unit -  Unit-ID 
 * \param [in] mode - RCI throttling mechanism mode see 
 *        bcm_fabric_control_rci_throttling_mode_t
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * bcm_fabric_control_rci_throttling_mode_t
 */
shr_error_e
dnx_fabric_rci_throttling_mode_set(
    int unit,
    bcm_fabric_control_rci_throttling_mode_t mode)
{
    dbal_enum_value_field_rci_biasing_mode_e rci_biasing_mode;

    SHR_FUNC_INIT_VARS(unit);

    switch (mode)
    {
        case bcmFabricControlRciThrottlingModeGlobal:
            rci_biasing_mode = DBAL_ENUM_FVAL_RCI_BIASING_MODE_GLOBAL;
            break;

        case bcmFabricControlRciThrottlingModeDeviceBiasing:
            rci_biasing_mode = DBAL_ENUM_FVAL_RCI_BIASING_MODE_DEVICE_BIASING;
            break;

        case bcmFabricControlRciThrottlingModeFlowBiasing:
            
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "RCI throttling mode %d is not supported yet\n", mode);

             /* coverity[unreachable:FALSE]  */
            rci_biasing_mode = DBAL_ENUM_FVAL_RCI_BIASING_MODE_FLOW_BIASING;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "RCI throttling mode %d is invalid\n", mode);
    }

    SHR_IF_ERR_EXIT(dnx_sch_rci_throttling_mode_hw_set(unit, rci_biasing_mode));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * get RCI throttling mode from HW
 */
shr_error_e
dnx_fabric_rci_throttling_mode_get(
    int unit,
    bcm_fabric_control_rci_throttling_mode_t * mode)
{
    dbal_enum_value_field_rci_biasing_mode_e rci_biasing_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sch_rci_throttling_mode_hw_get(unit, &rci_biasing_mode));

    switch (rci_biasing_mode)
    {
        case DBAL_ENUM_FVAL_RCI_BIASING_MODE_GLOBAL:
            *mode = bcmFabricControlRciThrottlingModeGlobal;
            break;

        case DBAL_ENUM_FVAL_RCI_BIASING_MODE_DEVICE_BIASING:
            *mode = bcmFabricControlRciThrottlingModeDeviceBiasing;
            break;

        case DBAL_ENUM_FVAL_RCI_BIASING_MODE_FLOW_BIASING:
            *mode = bcmFabricControlRciThrottlingModeFlowBiasing;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Read invalid RCI throttling mode 0x%x\n", rci_biasing_mode);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - from percentage to threshold
 *        see  bcm_dnx_fabric_rci_biasing_probability_set for
 *        params
 */
static int
dnx_fabric_rci_biasing_percentage_to_threshold(
    int unit,
    uint32 flags,
    bcm_fabric_rci_biasing_probability_info_t * probability_info,
    int probability[BCM_FABRIC_NUM_OF_RCI_LEVELS],
    int threshold[BCM_FABRIC_NUM_OF_RCI_LEVELS + 1])
{
    int rci_level;
    uint32 prob_thresh_max_value;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * probability has BCM_FABRIC_NUM_OF_RCI_LEVELS (7) entries 
     * and probability[i] defines RCI level i+1
     * RCI level 0 is always 0 and is not controlled by user
     */

    /** probability for level 0 is always 0 and is not controlled by user */
    threshold[0] = 0;

    switch (probability_info->probability_type)
    {
        case bcmFabricRciDevBiasingModeDiscard:

            SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                                  DBAL_TABLE_SCH_BW_PROFILE_BIASING_PROBABILITY,
                                                                  DBAL_FIELD_DISCARD_THRESHOLD, 0, 0, 0,
                                                                  DBAL_PREDEF_VAL_MAX_VALUE, &prob_thresh_max_value));

            for (rci_level = 0; rci_level < BCM_FABRIC_NUM_OF_RCI_LEVELS; rci_level++)
            {
                /*
                 * convert from percentage to threshold 
                 * from HW: Selects the Lower Threshold under which credits are discarded.
                 * threshold = probability/100 * maximum
                 */
                threshold[rci_level + 1] = UTILEX_DIV_ROUND_UP(probability[rci_level] * prob_thresh_max_value, 100);
            }
            break;
        case bcmFabricRciFlowBiasingModeUninstall:

            SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                                  DBAL_TABLE_SCH_BW_PROFILE_BIASING_PROBABILITY,
                                                                  DBAL_FIELD_UNINSTALL_THRESHOLD, 0, 0, 0,
                                                                  DBAL_PREDEF_VAL_MAX_VALUE, &prob_thresh_max_value));

            for (rci_level = 0; rci_level < BCM_FABRIC_NUM_OF_RCI_LEVELS; rci_level++)
            {

                /*
                 * convert from percentage to threshold 
                 * from HW: Selects the threshold above which flows are unlinked
                 * threshold = (100 - probability)/100 * maximum
                 */
                threshold[rci_level + 1] =
                    prob_thresh_max_value - UTILEX_DIV_ROUND_UP(probability[rci_level] * prob_thresh_max_value, 100);
            }
            break;
        case bcmFabricRciDevBiasingModeReplicate:

            SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                                  DBAL_TABLE_SCH_BW_PROFILE_BIASING_PROBABILITY,
                                                                  DBAL_FIELD_REPLICATE_THRESHOLD, 0, 0, 0,
                                                                  DBAL_PREDEF_VAL_MAX_VALUE, &prob_thresh_max_value));

            for (rci_level = 0; rci_level < BCM_FABRIC_NUM_OF_RCI_LEVELS; rci_level++)
            {
                /*
                 * convert from percentage to threshold 
                 * from HW: Selects the Higher Threshold above which credits are doubled. 
                 * same max as DISCARD_THRESHOLD 
                 * threshold = (100 - probability)/100 * maximum
                 */
                threshold[rci_level + 1] =
                    prob_thresh_max_value - UTILEX_DIV_ROUND_UP(probability[rci_level] * prob_thresh_max_value, 100);
            }
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "probability type %d is invalid\n", probability_info->probability_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - from threshold to percentage  
 *        see bcm_dnx_fabric_rci_biasing_probability_get for
 *        params
 */
static int
dnx_fabric_rci_biasing_threshold_to_percentage(
    int unit,
    uint32 flags,
    bcm_fabric_rci_biasing_probability_info_t * probability_info,
    int threshold[BCM_FABRIC_NUM_OF_RCI_LEVELS + 1],
    int probability[BCM_FABRIC_NUM_OF_RCI_LEVELS])
{
    int rci_level;
    uint32 prob_thresh_max_value;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * probability has BCM_FABRIC_NUM_OF_RCI_LEVELS (7) entries 
     * and probability[i] defines RCI level i+1
     * RCI level 0 is always 0 and is not controlled by user
     */

    switch (probability_info->probability_type)
    {
        case bcmFabricRciDevBiasingModeDiscard:
            SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                                  DBAL_TABLE_SCH_BW_PROFILE_BIASING_PROBABILITY,
                                                                  DBAL_FIELD_DISCARD_THRESHOLD, 0, 0, 0,
                                                                  DBAL_PREDEF_VAL_MAX_VALUE, &prob_thresh_max_value));

            for (rci_level = 0; rci_level < BCM_FABRIC_NUM_OF_RCI_LEVELS; rci_level++)
            {
                /*
                 * convert from percentage to threshold 
                 * from HW: Selects the Lower Threshold under which credits are discarded. 
                 * threshold = probability/100 * maximum => probability = threshold/maximum * 100
                 */
                probability[rci_level] = UTILEX_DIV_ROUND(threshold[rci_level + 1] * 100, prob_thresh_max_value);
            }
            break;
        case bcmFabricRciFlowBiasingModeUninstall:
            SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                                  DBAL_TABLE_SCH_BW_PROFILE_BIASING_PROBABILITY,
                                                                  DBAL_FIELD_UNINSTALL_THRESHOLD, 0, 0, 0,
                                                                  DBAL_PREDEF_VAL_MAX_VALUE, &prob_thresh_max_value));

            for (rci_level = 0; rci_level < BCM_FABRIC_NUM_OF_RCI_LEVELS; rci_level++)
            {
                /*
                 * convert from percentage to threshold 
                 * from HW: Selects the threshold above which flows are unlinked
                 * threshold = (100 - probability)/100 * maximum => probability = 100 - threshold/maximum * 100
                 */
                probability[rci_level] = 100 - UTILEX_DIV_ROUND(threshold[rci_level + 1] * 100, prob_thresh_max_value);
            }
            break;
        case bcmFabricRciDevBiasingModeReplicate:
            SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                                  DBAL_TABLE_SCH_BW_PROFILE_BIASING_PROBABILITY,
                                                                  DBAL_FIELD_REPLICATE_THRESHOLD, 0, 0, 0,
                                                                  DBAL_PREDEF_VAL_MAX_VALUE, &prob_thresh_max_value));

            for (rci_level = 0; rci_level < BCM_FABRIC_NUM_OF_RCI_LEVELS; rci_level++)
            {
                /*
                 * convert from percentage to threshold 
                 * from HW: Selects the Higher Threshold above which credits are doubled. 
                 * same max as DISCARD_THRESHOLD
                 * threshold = (100 - probability)/100 * maximum => probability = 100 - threshold/maximum * 100
                 */
                probability[rci_level] = 100 - UTILEX_DIV_ROUND(threshold[rci_level + 1] * 100, prob_thresh_max_value);
            }
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "probability type %d is invalid\n", probability_info->probability_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify arguments of 
 *        bcm_dnx_fabric_rci_biasing_probability_set/get
 */
static int
dnx_fabric_rci_biasing_probability_param_verify(
    int unit,
    uint32 flags,
    bcm_fabric_rci_biasing_probability_info_t * probability_info,
    int *probability)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(probability_info, _SHR_E_PARAM, "probability_info");
    SHR_NULL_CHECK(probability, _SHR_E_PARAM, "probability");

    switch (probability_info->probability_type)
    {
        case bcmFabricRciDevBiasingModeDiscard:
        case bcmFabricRciDevBiasingModeReplicate:
                /** valid type  */
            break;

        case bcmFabricRciFlowBiasingModeUninstall:
            
            SHR_ERR_EXIT(_SHR_E_PARAM, "probability type %d is not supported yet\n",
                         probability_info->probability_type);
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "probability type %d is invalid\n", probability_info->probability_type);
    }

    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "No flag is supported\n");

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Verify arguments of 
 *        bcm_dnx_fabric_rci_biasing_probability_set
 */
static int
dnx_fabric_rci_biasing_probability_set_verify(
    int unit,
    uint32 flags,
    bcm_fabric_rci_biasing_probability_info_t * probability_info,
    int probability[BCM_FABRIC_NUM_OF_RCI_LEVELS])
{
    int rci_level;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_fabric_rci_biasing_probability_param_verify(unit, flags, probability_info, probability));

    for (rci_level = 0; rci_level < BCM_FABRIC_NUM_OF_RCI_LEVELS; rci_level++)
    {
        if (probability[rci_level] > 100 || probability[rci_level] < 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid percentage %d for rci_level %d\n", probability[rci_level], rci_level);
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - writing to HW for 
 *        bcm_dnx_fabric_rci_biasing_probability_set
 *        see  bcm_dnx_fabric_rci_biasing_probability_set for
 *        params
 */
static int
dnx_fabric_rci_biasing_probability_hw_set(
    int unit,
    uint32 flags,
    bcm_fabric_rci_biasing_probability_info_t * probability_info,
    int threshold[BCM_FABRIC_NUM_OF_RCI_LEVELS + 1])
{
    uint32 entry_handle_id;
    int rci_level, core, core_i;
    dbal_fields_e field_name;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking core from gport */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_core_get(unit, ALGO_GPM_GPORT_CORE_F_NO_CORE_ALL, probability_info->gport,
                                                  &core));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_BW_PROFILE_BIASING_PROBABILITY, &entry_handle_id));

    switch (probability_info->probability_type)
    {
        case bcmFabricRciDevBiasingModeDiscard:
            field_name = DBAL_FIELD_DISCARD_THRESHOLD;
            break;
        case bcmFabricRciFlowBiasingModeUninstall:
            field_name = DBAL_FIELD_UNINSTALL_THRESHOLD;
            break;
        case bcmFabricRciDevBiasingModeReplicate:
            field_name = DBAL_FIELD_REPLICATE_THRESHOLD;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "probability type %d is invalid\n", probability_info->probability_type);
    }

    DNXCMN_CORES_ITER(unit, core, core_i)
    {
        for (rci_level = 0; rci_level < BCM_FABRIC_NUM_OF_RCI_LEVELS + 1; rci_level++)
        {

            /** Setting key fields */
            /** flow_id will be validated in dbal */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_i);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RCI_LEVEL, rci_level);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_FABRIC, probability_info->is_remote);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_HIGH_PRIO, probability_info->is_high_prio);

            /** Setting value fields */
            dbal_entry_value_field32_set(unit, entry_handle_id, field_name, INST_SINGLE, threshold[rci_level]);

            /** Performing the action */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief - getting thresholds from HW for 
 *        bcm_dnx_fabric_rci_biasing_probability_get
 *        see  bcm_dnx_fabric_rci_biasing_probability_get for
 *        params
 */
static int
dnx_fabric_rci_biasing_probability_hw_get(
    int unit,
    uint32 flags,
    bcm_fabric_rci_biasing_probability_info_t * probability_info,
    int threshold[BCM_FABRIC_NUM_OF_RCI_LEVELS + 1])
{
    uint32 entry_handle_id;
    int rci_level, core;
    dbal_fields_e field_name;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking core from gport */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_core_get(unit, ALGO_GPM_GPORT_CORE_F_NO_CORE_ALL, probability_info->gport,
                                                  &core));

    switch (probability_info->probability_type)
    {
        case bcmFabricRciDevBiasingModeDiscard:
            field_name = DBAL_FIELD_DISCARD_THRESHOLD;
            break;
        case bcmFabricRciFlowBiasingModeUninstall:
            field_name = DBAL_FIELD_UNINSTALL_THRESHOLD;
            break;
        case bcmFabricRciDevBiasingModeReplicate:
            field_name = DBAL_FIELD_REPLICATE_THRESHOLD;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "probability type %d is invalid\n", probability_info->probability_type);
    }

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_BW_PROFILE_BIASING_PROBABILITY, &entry_handle_id));

    for (rci_level = 0; rci_level < BCM_FABRIC_NUM_OF_RCI_LEVELS + 1; rci_level++)
    {

        /** Setting key fields */
        /*
         * flow_id will be validated in dbal 
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RCI_LEVEL, rci_level);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_FABRIC, probability_info->is_remote);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_HIGH_PRIO, probability_info->is_high_prio);

        /** Setting pointers value to receive the fields */
        dbal_value_field32_request(unit, entry_handle_id, field_name, INST_SINGLE, (uint32 *) &(threshold[rci_level]));

        /** Preforming the action after this call the pointers that we set in field32_request() */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/*
 * API
 */

/**
 * \brief - 
 * General function for managing RCI mechanism probabilities 
 * info for all modes (DeviceBiasing/FlowBiasing)
 *  
 * \param [in] unit -  Unit-ID 
 * \param [in] flags - Not used
 * \param [in] probability_info - the struct that hold the 
 *        profile info we want to edit (local/remote,HP/LP
 *        probability type) gport in the struct must be
 *        BCM_COSQ_GPORT_IS_CORE, see struct info
 * \param [in] probability[] - the probability array per RCI 
 *        level we want to set.
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * bcm_fabric_rci_biasing_probability_info_t
 */
int
bcm_dnx_fabric_rci_biasing_probability_set(
    int unit,
    uint32 flags,
    bcm_fabric_rci_biasing_probability_info_t * probability_info,
    int probability[BCM_FABRIC_NUM_OF_RCI_LEVELS])
{
    int probability_threshold[BCM_FABRIC_NUM_OF_RCI_LEVELS + 1];
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verify API arguments */
    SHR_INVOKE_VERIFY_DNX(dnx_fabric_rci_biasing_probability_set_verify(unit, flags, probability_info, probability));

    /** convert to HW threshold   */
    SHR_IF_ERR_EXIT(dnx_fabric_rci_biasing_percentage_to_threshold
                    (unit, flags, probability_info, probability, probability_threshold));

    /** setting HW */
    SHR_IF_ERR_EXIT(dnx_fabric_rci_biasing_probability_hw_set(unit, flags, probability_info, probability_threshold));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * General function for managing RCI mechanism probabilities 
 * info for all modes (DeviceBiasing/FlowBiasing)
 *  
 * \param [in] unit -  Unit-ID 
 * \param [in] flags - Not used
 * \param [in] probability_info - the struct that hold the 
 *        profile info we want to get (local/remote,HP/LP
 *        probability type) gport in the struct must be
 *        BCM_COSQ_GPORT_IS_CORE, see struct info.
 * \param [out] probability[] - the output probability array per
 *        RCI level.
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * bcm_fabric_rci_biasing_probability_info_t
 */
int
bcm_dnx_fabric_rci_biasing_probability_get(
    int unit,
    uint32 flags,
    bcm_fabric_rci_biasing_probability_info_t * probability_info,
    int probability[BCM_FABRIC_NUM_OF_RCI_LEVELS])
{
    int probability_threshold[BCM_FABRIC_NUM_OF_RCI_LEVELS + 1];

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_fabric_rci_biasing_probability_param_verify(unit, flags, probability_info, probability));

    /** getting info from HW */
    SHR_IF_ERR_EXIT(dnx_fabric_rci_biasing_probability_hw_get(unit, flags, probability_info, probability_threshold));

    /** convert from HW threshold to  */
    SHR_IF_ERR_EXIT(dnx_fabric_rci_biasing_threshold_to_percentage
                    (unit, flags, probability_info, probability_threshold, probability));

exit:
    SHR_FUNC_EXIT;
}
