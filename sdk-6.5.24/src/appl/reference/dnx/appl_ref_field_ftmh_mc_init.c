/** \file appl_ref_field_ftmh_init.c
 * 
 *
 * FTMH Programmable mode application procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

 /*
  * Include files.
  * {
  */
/** soc */
#include <soc/schanmsg.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <soc/sand/sand_aux_access.h>
/** shared */
#include <shared/shrextend/shrextend_debug.h>
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <appl/reference/dnx/appl_ref_dynamic_port.h>
#include "appl_ref_field_ftmh_mc_init.h"

/** bcm */
#include <bcm/field.h>
/** sal */
#include <sal/appl/sal.h>
#include <sal/core/libc.h>

/*
 * }
 */

/* *INDENT-OFF* */

/*
 * DEFINEs
 * {
 */

/** IPMF2 Qualifiers info. */
static field_ftmh_mc_qual_info_t field_ftmh_mc_ipmf2_qual_info_array[FTMH_MC_NOF_IPMF2_QUALIFIERS] = {
  /** qual_name  |  qual_id   |   qual_size     |  input_type    |    input_arg  |  qual_offset  */
    {"ftmh_mc_dest_mc_qual", 0, MC_ID_DST_QUAL_LENGTH, {bcmFieldInputTypeLayerAbsolute, 0, MC_ID_DST_QUAL_OFFSET}},
    {"ftmh_mc_dest_mc_const_qual", 0, MC_ID_DST_CONST_QUAL_LENGTH, {bcmFieldInputTypeConst, 0x3, 0}},
    {"ftmh_mc_tc_qual", 0, TC_QUAL_LENGTH,{bcmFieldInputTypeLayerAbsolute, 0, TC_QUAL_OFFSET}},
    {"ftmh_mc_dp_qual", 0, DP_QUAL_LENGTH,{bcmFieldInputTypeLayerAbsolute, 0, DP_QUAL_OFFSET}},
    {"ftmh_mc_aqm_qual", 0, AQM_PROFILE_QUAL_LENGTH,{bcmFieldInputTypeLayerAbsolute, 0, AQM_PROFILE_QUAL_OFFSET}},
    {"ftmh_mc_packet_size_qual", 0, PACKET_SIZE_QUAL_LENGTH,{bcmFieldInputTypeLayerAbsolute, 0, PACKET_SIZE_QUAL_OFFSET}},
    {"ftmh_mc_parsing_offset_valid_qual", 0, PPH_PARSING_OFFSET_VALID_QUAL_LENGTH, {bcmFieldInputTypeConst, 1, 0}},
    {"ftmh_mc_parsing_offset_qual", 3, PPH_PARSING_OFFSET_QUAL_LENGTH,{bcmFieldInputTypeLayerAbsolute, 0, PPH_PARSING_OFFSET_QUAL_OFFSET}},
    {"ftmh_mc_lag_key_qual", 0, LAG_KEY_QUAL_LENGTH,{bcmFieldInputTypeLayerAbsolute, 0, LAG_KEY_QUAL_OFFSET}},
};

/**
 * IPMF2 Action info.
 * For actions, which are not user define, we need only BCM action ID.
 */
static field_ftmh_mc_action_info_t field_ftmh_mc_ipmf2_action_info_array[FTMH_MC_NOF_IPMF2_ACTIONS] = {
/** {action_name}   |  action_id  |    { stage | action_type | size | prefix_size | prefix_value | name  } */
    {"ftmh_mc_dest_mc_act", 0, {0, bcmFieldActionForward, MC_ID_DST_ACTION_LENGTH, 11, 0, {0}}},
    {"ftmh_mc_tc_act", 0, {0, bcmFieldActionPrioIntNew, TC_ACTION_LENGTH, 0, 0, {0}}},
    {"ftmh_mc_dp_act", 0, {0, bcmFieldActionDropPrecedence, DP_ACTION_LENGTH, 0, 0, {0}}},
    {"ftmh_mc_aqm_act", 0, {0, bcmFieldActionAdmitProfile, AQM_PROFILE_ACTION_LENGTH, 2, 0, {0}}},
    {"ftmh_mc_container_act", 0, {0, bcmFieldActionContainer, CONTAINER_ACTION_LENGTH, 9, 0, {0}}},
};


/** IPMF3 Qualifiers info. */
static field_ftmh_mc_qual_info_t field_ftmh_mc_ipmf3_qual_info_array[FTMH_MC_NOF_IPMF3_QUALIFIERS] = {
  /** qual_name  |  qual_id   |   qual_size     |  input_type    |    input_arg  |  qual_offset  */
    {NULL, bcmFieldQualifyContainer, 32, {bcmFieldInputTypeMetaData, 0, 0}},
};

/**
 * IPMF3 Action info.
 * For actions, which are not user define, we need only BCM action ID.
 */
static field_ftmh_mc_action_info_t field_ftmh_mc_ipmf3_action_info_array[FTMH_MC_NOF_IPMF3_ACTIONS] = {
/** {action_name}   |  action_id  |    { stage | action_type | size | prefix_size | prefix_value | name  } */
    {"ftmh_mc_system_header_size_act", 0, {0, bcmFieldActionSystemHeaderSizeAdjust, PACKET_SIZE_ACTION_LENGTH, 0, 0, {0}}},
    {"ftmh_mc_parsing_offset_act", 0, {0, bcmFieldActionParsingStartOffsetRaw, 8, 0, 0, {0}}},
    {"ftmh_mc_lag_key_act", 0, {0, bcmFieldActionTrunkHashKeySet, 16, 0, 0, {0}}},
    {"ftmh_mc_lag_key_void_act", 0, {0, bcmFieldActionVoid, 1, 0, 0, {0}}}
};


/**
 * Array, which contains information per field group. Like:
 * fg_id, fg_type, qulas, actions, fem_info.
 */
static field_ftmh_mc_fg_info_t ftmh_mc_fg_info_array[FTMH_MC_NOF_FG] = {
    /** IPMF2 field group info */
    {
        0,   /** Field group ID */
        bcmFieldStageIngressPMF2,   /** Field stage */
        bcmFieldGroupTypeDirectExtraction,  /** Field group type */
        FTMH_MC_NOF_IPMF2_QUALIFIERS,  /** Number of tested qualifiers */
        FTMH_MC_NOF_IPMF2_ACTIONS, /** Number of tested actions */
        field_ftmh_mc_ipmf2_qual_info_array,   /** Qualifiers info */
        field_ftmh_mc_ipmf2_action_info_array, /** Actions info */
    },
    /** IPMF3 field group info */
    {
        0,   /** Field group ID */
        bcmFieldStageIngressPMF3,   /** Field stage */
        bcmFieldGroupTypeDirectExtraction,  /** Field group type */
        FTMH_MC_NOF_IPMF3_QUALIFIERS,  /** Number of tested qualifiers */
        FTMH_MC_NOF_IPMF3_ACTIONS, /** Number of tested actions */
        field_ftmh_mc_ipmf3_qual_info_array,   /** Qualifiers info */
        field_ftmh_mc_ipmf3_action_info_array, /** Actions info */
    }
};


/**
 * \brief
 *  This function is used to create a context and configure
 *  the relevant program for the testing.
 *
 * \param [in] unit - The unit number.
 * \param [in] context_id       - Context id to be created.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_ftmh_mc_context(
    int unit,
    bcm_field_context_t context_id)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    bcm_field_presel_entry_id_t presel_entry_id;

    SHR_FUNC_INIT_VARS(unit);

    /*********************
     *  IPMF1 context
 */
    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) (context_info.name), "FTMH_MC_IPMF1", sizeof(context_info.name));

    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF1, &context_info, &context_id));

    bcm_field_context_param_info_t_init(&param_info);
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
    param_info.param_val = bcmFieldSystemHeaderProfileNone;

    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF1, context_id, &param_info));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    /** For iPMF2, iPMF1 presel must be configured. */
    presel_entry_id.presel_id = dnx_data_field.preselector.default_ftmh_mc_presel_id_ipmf1_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.context_id = context_id;
    presel_entry_data.entry_valid = TRUE;

    /** Check that the port is the recycle port. */
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyPortClassPacketProcessing;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = PORT_CLASS_ID;
    presel_entry_data.qual_data[0].qual_mask = 0x7;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    /*********************
     *  IPMF3 context
 */
    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) (context_info.name), "FTMH_MC_IPMF3", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF3, &context_info, &context_id));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);

    presel_entry_id.presel_id = dnx_data_field.preselector.default_ftmh_mc_presel_id_ipmf3_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF3;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.context_id = context_id;
    presel_entry_data.nof_qualifiers = 1;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyContextId;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = context_id;
    presel_entry_data.qual_data[0].qual_mask = 0x3F;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

exit:
    SHR_FUNC_EXIT;
}


/**
 * \brief
 *  Used to configure the user qualifiers.
 *
 * \param [in] unit - The unit number.
 * \param [in] ftmh_mc_qual_info - Contains all need information
 *                              for one user qualifier to be created.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_ftmh_mc_user_qual_config(
    int unit,
    field_ftmh_mc_qual_info_t * ftmh_mc_qual_info)
{
    bcm_field_qualifier_info_create_t qual_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_qualifier_info_create_t_init(&qual_info);
    sal_strncpy_s((char *) (qual_info.name), ftmh_mc_qual_info->name, sizeof(qual_info.name));
    qual_info.size = ftmh_mc_qual_info->qual_length;
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &(ftmh_mc_qual_info->qual_id)));

exit:
    SHR_FUNC_EXIT;
}


/**
 * \brief
 *  Used to configure the user defined actions.
 *
 * \param [in] unit - The unit number.
 * \param [in] field_stage - There is no meaning of stage in
 *                           user actions, but it should be parsed to the structure.
 * \param [in] ftmh_mc_action_info - Contains all need information
 *                                for one user action to be created.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_ftmh_mc_user_action_config(
    int unit,
    bcm_field_stage_t field_stage,
    field_ftmh_mc_action_info_t * ftmh_mc_action_info)
{
    bcm_field_action_info_t action_info;
    bcm_field_action_info_t action_info_get;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_action_info_t_init(&action_info);
    SHR_IF_ERR_EXIT(bcm_field_action_info_get(unit, ftmh_mc_action_info->action_info.action_type, field_stage, &action_info_get));

    action_info.action_type = ftmh_mc_action_info->action_info.action_type;
    sal_strncpy_s((char *) action_info.name, ftmh_mc_action_info->name, sizeof(action_info.name));

    action_info.prefix_size = action_info_get.size - ftmh_mc_action_info->action_info.size;
    action_info.size = ftmh_mc_action_info->action_info.size;
    action_info.stage = field_stage;
    action_info.prefix_value = ftmh_mc_action_info->action_info.prefix_value;

    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &(ftmh_mc_action_info->action_id)));

exit:
    SHR_FUNC_EXIT;
}


/**
 * \brief
 *  This function creates field group, with all
 *  relevant information to it and adding it to the context.
 *
 * \param [in] unit - The unit number.
 * \param [in] ftmh_mc_fg_info - Contains all need information
 *                            for one group to be created.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_ftmh_mc_group_config(
    int unit,
    field_ftmh_mc_fg_info_t * ftmh_mc_fg_info)
{
    int qual_index, action_index;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_t context_id;

    SHR_FUNC_INIT_VARS(unit);

    context_id = dnx_data_field.context.default_ftmh_mc_context_get(unit);

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);

    fg_info.fg_type = ftmh_mc_fg_info->fg_type;
    fg_info.stage = ftmh_mc_fg_info->stage;
    if (fg_info.stage == bcmFieldStageIngressPMF2)
    {
        sal_strncpy_s((char *) (fg_info.name), "FTMH_MC_IPMF2", sizeof(fg_info.name));
    }
    else
    {
        sal_strncpy_s((char *) (fg_info.name), "FTMH_MC_IPMF3", sizeof(fg_info.name));
    }
    fg_info.nof_quals = ftmh_mc_fg_info->nof_quals;
    fg_info.nof_actions = ftmh_mc_fg_info->nof_actions;
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    /**
     * Iterate over all qualifiers for current field group. Using field_ftmh_mc_user_qual_config()
     * to create all user define qualifiers internal function.
     */
    for (qual_index = 0; qual_index < fg_info.nof_quals; qual_index++)
    {
        if (ftmh_mc_fg_info->ftmh_mc_qual_info[qual_index].name != NULL)
        {
            SHR_IF_ERR_EXIT(appl_dnx_field_ftmh_mc_user_qual_config
                            (unit, &(ftmh_mc_fg_info->ftmh_mc_qual_info[qual_index])));
        }
        fg_info.qual_types[qual_index] = ftmh_mc_fg_info->ftmh_mc_qual_info[qual_index].qual_id;
        attach_info.key_info.qual_types[qual_index] = ftmh_mc_fg_info->ftmh_mc_qual_info[qual_index].qual_id;
        attach_info.key_info.qual_info[qual_index].input_type =
            ftmh_mc_fg_info->ftmh_mc_qual_info[qual_index].qual_attach_info.input_type;
        attach_info.key_info.qual_info[qual_index].input_arg =
            ftmh_mc_fg_info->ftmh_mc_qual_info[qual_index].qual_attach_info.input_arg;
        attach_info.key_info.qual_info[qual_index].offset =
            ftmh_mc_fg_info->ftmh_mc_qual_info[qual_index].qual_attach_info.offset;
    }

    /**
     * Iterate over all actions for current field group. For those, which are user define,
     * use field_ftmh_mc_user_action_config() internal function.
     */
    for (action_index = 0; action_index < fg_info.nof_actions; action_index++)
    {
        if (ftmh_mc_fg_info->ftmh_mc_action_info[action_index].name != NULL)
        {
            SHR_IF_ERR_EXIT(appl_dnx_field_ftmh_mc_user_action_config
                            (unit, fg_info.stage, &(ftmh_mc_fg_info->ftmh_mc_action_info[action_index])));
        }
        fg_info.action_types[action_index] = ftmh_mc_fg_info->ftmh_mc_action_info[action_index].action_id;
        fg_info.action_with_valid_bit[action_index] = FALSE;
        attach_info.payload_info.action_types[action_index] =
            ftmh_mc_fg_info->ftmh_mc_action_info[action_index].action_id;

    }

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &(ftmh_mc_fg_info->fg_id)));

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, ftmh_mc_fg_info->fg_id, context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}


/**
 * \brief
 *   This function initializes the FTMH MC programmable mode application.
 *   This function sets all required HW configuration for FTMH MC
 *   processing.
 *   The function configures field database of direct extraction
 *   mode in order to extract all relevant FTMH information form the header.
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e
appl_dnx_field_ftmh_mc_init(
    int unit)
{
    int fg_index;
    bcm_field_context_t context_id;

    SHR_FUNC_INIT_VARS(unit);
    context_id = dnx_data_field.context.default_ftmh_mc_context_get(unit);

    SHR_IF_ERR_EXIT(appl_dnx_field_ftmh_mc_context(unit, context_id));
    /**
     * Iterate over 2 DE field groups and create them one by one.
     * Parsing the global array "ftmh_fg_info_array[]" with all needed
     * information for both field groups.
     */
    for (fg_index = 0; fg_index < FTMH_MC_NOF_FG; fg_index++)
    {
        SHR_IF_ERR_EXIT(appl_dnx_field_ftmh_mc_group_config(unit, &ftmh_mc_fg_info_array[fg_index]));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see appl_dnx_field_ftmh_mc.h*/
shr_error_e
appl_dnx_field_ftmh_mc_cb(
    int unit,
    int *dynamic_flags)
{
    uint8 is_default_image = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    *dynamic_flags = 0;

    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_default_image));

    if (!is_default_image)
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP;
    }

    if (!dnx_data_field.init.ftmh_mc_get(unit))
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP | UTILEX_SEQ_STEP_F_VERBOSE;
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
