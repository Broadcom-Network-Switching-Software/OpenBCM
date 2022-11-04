/** \file appl_ref_field_ftmh_mc_init.c
 * 
 *
 * FTMH Programmable mode application procedures for DNX.
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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
#include <soc/sand/shrextend/shrextend_debug.h>
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
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
#define PORT_CLASS_ID 0x7

/** Offset of MC ID DESTINATION in FTMH header [67:49] (19b) */
#define MC_ID_DST_OFFSET            49
/** Length of MC ID DESTINATION in FTMH header [67:49] (19b) */
#define MC_ID_DST_LENGTH            19
#define MC_ID_DST_CONST_LENGTH      2
/** Length of user defined MC ID DESTINATION action (21b) */
#define MC_ID_DST_ACTION_LENGTH     (MC_ID_DST_LENGTH + MC_ID_DST_CONST_LENGTH)

/** Offset of LAG key qualifier in FTMH header [103:88] (16b) */
#define LAG_KEY_OFFSET      88
/** Length of LAG key qualifier in FTMH header [103:88] (16b) */
#define LAG_KEY_LENGTH      16

/** Offset of Packet_size qualifier in FTMH header [6:0] (7b) */
#define PACKET_SIZE_OFFSET      0
/** Length of Packet_size qualifier in FTMH header [6:0] (7b) */
#define PACKET_SIZE_LENGTH      7

/** Offset of TC qualifier in FTMH header [16:14] (3b) */
#define TC_OFFSET      14
/** Length of TC qualifier in FTMH header [16:14] (3b) */
#define TC_LENGTH      3

/** Offset of DP qualifier in FTMH header [42:41] (2b) */
#define DP_OFFSET      41
/** Length of DP qualifier in FTMH header [42:41] (2b) */
#define DP_LENGTH      2

/** Offset of AQM Profile qualifier in FTMH header [69] (1b) */
#define AQM_PROFILE_OFFSET        69
/** Length of AQM Profile qualifier in FTMH header [69] (1b) */
#define AQM_PROFILE_LENGTH        1

/** Offset of Parsing start offset qualifier in PPH header [89:83] (7b) */
#define PPH_PARSING_OFFSET_OFFSET     83
/** Length of Parsing start offset qualifier in FTMH header [89:83] (7b) */
#define PPH_PARSING_OFFSET_LENGTH     7
#define VALID_BIT                     1
/** Length of user defined Parsing start offset action (8b) */
#define PPH_PARSING_OFFSET_ACTION_LENGTH    (PPH_PARSING_OFFSET_LENGTH + VALID_BIT)

#define CONTAINER_ACTION_LENGTH    (LAG_KEY_LENGTH + PPH_PARSING_OFFSET_ACTION_LENGTH + PACKET_SIZE_LENGTH)

/*
 * }
 */


/*
 * Global and Static
 * {
 */
/**
 * \brief
 *  This function creates user defined qualifier.
 *
 * \param [in] unit     - The unit number.
 * \param [in] name     - Qualifier name.
 * \param [in] size     - Qualifier size.
 * \param [out] qual_id - Qualifier ID.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_ftmh_mc_user_qual_create(
    int unit,
    char *name,
    int size,
    bcm_field_qualify_t * qual_id)
{
    bcm_field_qualifier_info_create_t qual_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_qualifier_info_create_t_init(&qual_info);
    sal_strncpy_s((char *) (qual_info.name), name, sizeof(qual_info.name));
    qual_info.size = size;
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, qual_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function creates user defined action.
 *
 * \param [in] unit         - The unit number.
 * \param [in] name         - Action name.
 * \param [in] type         - Action type.
 * \param [in] stage        - stage.
 * \param [in] size         - Action size.
 * \param [in] prefix_value - Prefix value.
 * \param [out] action_id   - Action ID.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_ftmh_mc_user_action_create(
    int unit,
    char *name,
    bcm_field_action_t type,
    bcm_field_stage_t stage,
    int size,
    int prefix_value,
    bcm_field_action_t * action_id)
{
    bcm_field_action_info_t action_info, action_info_get;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = type;
    sal_strncpy_s((char *) action_info.name, name, sizeof(action_info.name));
    action_info.size = size;
    action_info.stage = stage;
    action_info.prefix_value = prefix_value;

    bcm_field_action_info_t_init(&action_info_get);
    SHR_IF_ERR_EXIT(bcm_field_action_info_get(unit, type, stage, &action_info_get));
    action_info.prefix_size = action_info_get.size - size;

    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, action_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function creates field group in iPMF2, with all
 *  relevant information to it.
 *
 * \param [in] unit         - The unit number.
 * \param [out] fg_id       - Field group ID created.
 * \param [out] attach_info - Attach info.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_ftmh_ipmf2_group_config(
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_group_info_t fg_info;
    bcm_field_qualify_t ftmh_mc_dest_mc_qual, ftmh_mc_dest_mc_const_qual, ftmh_mc_tc_qual, ftmh_mc_dp_qual, ftmh_mc_aqm_qual;
    bcm_field_qualify_t ftmh_mc_packet_size_qual, ftmh_mc_parsing_offset_valid_qual, ftmh_mc_parsing_offset_qual, ftmh_mc_lag_key_qual;
    bcm_field_action_t ftmh_mc_dest_mc_action, ftmh_mc_aqm_action, ftmh_mc_container_action;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create user defined qualifiers and actions
     */
    /** MC ID destination [67:49] (19b) */
    SHR_IF_ERR_EXIT(appl_dnx_field_ftmh_mc_user_qual_create(unit, "dest_mc_id", MC_ID_DST_LENGTH, &ftmh_mc_dest_mc_qual));
    /** MC ID destination const prefix (2b) */
    SHR_IF_ERR_EXIT(appl_dnx_field_ftmh_mc_user_qual_create(unit, "dest_mc_const", MC_ID_DST_CONST_LENGTH, &ftmh_mc_dest_mc_const_qual));
    /** Traffic class [16:14] (3b) */
    SHR_IF_ERR_EXIT(appl_dnx_field_ftmh_mc_user_qual_create(unit, "ftmh_mc_tc", TC_LENGTH, &ftmh_mc_tc_qual));
    /** drop precedence [42:41] (2b) */
    SHR_IF_ERR_EXIT(appl_dnx_field_ftmh_mc_user_qual_create(unit, "ftmh_mc_dp", DP_LENGTH, &ftmh_mc_dp_qual));
    /** AQM profile [69] (1b) */
    SHR_IF_ERR_EXIT(appl_dnx_field_ftmh_mc_user_qual_create(unit, "aqm_profile", AQM_PROFILE_LENGTH, &ftmh_mc_aqm_qual));
    /** Packet size [6:0] (7b) */
    SHR_IF_ERR_EXIT(appl_dnx_field_ftmh_mc_user_qual_create(unit, "ftmh_mc_packet_size", PACKET_SIZE_LENGTH, &ftmh_mc_packet_size_qual));
    /** Parsing start offset valid (1b) */
    SHR_IF_ERR_EXIT(appl_dnx_field_ftmh_mc_user_qual_create(unit, "parsing_offset_valid", VALID_BIT, &ftmh_mc_parsing_offset_valid_qual));
    /** Parsing start offset [89:83] (7b) */
    SHR_IF_ERR_EXIT(appl_dnx_field_ftmh_mc_user_qual_create(unit, "parsing_offset", PPH_PARSING_OFFSET_LENGTH, &ftmh_mc_parsing_offset_qual));
    /** LAG key [103:88] (16b) */
    SHR_IF_ERR_EXIT(appl_dnx_field_ftmh_mc_user_qual_create(unit, "lag_key", LAG_KEY_LENGTH, &ftmh_mc_lag_key_qual));
    SHR_IF_ERR_EXIT(appl_dnx_field_ftmh_mc_user_action_create(unit, "dest_mc", bcmFieldActionForward, bcmFieldStageIngressPMF2, MC_ID_DST_ACTION_LENGTH, 0, &ftmh_mc_dest_mc_action));
    SHR_IF_ERR_EXIT(appl_dnx_field_ftmh_mc_user_action_create(unit, "mc_admit_profile", bcmFieldActionAdmitProfile, bcmFieldStageIngressPMF2, AQM_PROFILE_LENGTH, 0, &ftmh_mc_aqm_action));
    SHR_IF_ERR_EXIT(appl_dnx_field_ftmh_mc_user_action_create(unit, "ftmh_mc_container", bcmFieldActionContainer, bcmFieldStageIngressPMF2, CONTAINER_ACTION_LENGTH, 0, &ftmh_mc_container_action));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) fg_info.name, "FTMH_MC_IPMF2", sizeof(fg_info.name));

    fg_info.nof_quals = 9;
    fg_info.qual_types[0] = ftmh_mc_dest_mc_qual;
    fg_info.qual_types[1] = ftmh_mc_dest_mc_const_qual;
    fg_info.qual_types[2] = ftmh_mc_tc_qual;
    fg_info.qual_types[3] = ftmh_mc_dp_qual;
    fg_info.qual_types[4] = ftmh_mc_aqm_qual;
    fg_info.qual_types[5] = ftmh_mc_packet_size_qual;
    fg_info.qual_types[6] = ftmh_mc_parsing_offset_valid_qual;
    fg_info.qual_types[7] = ftmh_mc_parsing_offset_qual;
    fg_info.qual_types[8] = ftmh_mc_lag_key_qual;

    fg_info.nof_actions = 5;
    fg_info.action_types[0] = ftmh_mc_dest_mc_action;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = bcmFieldActionPrioIntNew;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_types[2] = bcmFieldActionDropPrecedence;
    fg_info.action_with_valid_bit[2] = FALSE;
    fg_info.action_types[3] = ftmh_mc_aqm_action;
    fg_info.action_with_valid_bit[3] = FALSE;
    fg_info.action_types[4] = ftmh_mc_container_action;
    fg_info.action_with_valid_bit[4] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

    bcm_field_group_attach_info_t_init(attach_info);

    attach_info->key_info.nof_quals = fg_info.nof_quals;
    attach_info->payload_info.nof_actions = fg_info.nof_actions;

    attach_info->key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info->key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[0].input_arg = 0;
    attach_info->key_info.qual_info[0].offset = MC_ID_DST_OFFSET;

    attach_info->key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info->key_info.qual_info[1].input_type = bcmFieldInputTypeConst;
    attach_info->key_info.qual_info[1].input_arg = 0x3;

    attach_info->key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info->key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[2].input_arg = 0;
    attach_info->key_info.qual_info[2].offset = TC_OFFSET;

    attach_info->key_info.qual_types[3] = fg_info.qual_types[3];
    attach_info->key_info.qual_info[3].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[3].input_arg = 0;
    attach_info->key_info.qual_info[3].offset = DP_OFFSET;

    attach_info->key_info.qual_types[4] = fg_info.qual_types[4];
    attach_info->key_info.qual_info[4].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[4].input_arg = 0;
    attach_info->key_info.qual_info[4].offset = AQM_PROFILE_OFFSET;

    attach_info->key_info.qual_types[5] = fg_info.qual_types[5];
    attach_info->key_info.qual_info[5].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[5].input_arg = 0;
    attach_info->key_info.qual_info[5].offset = PACKET_SIZE_OFFSET;

    attach_info->key_info.qual_types[6] = fg_info.qual_types[6];
    attach_info->key_info.qual_info[6].input_type = bcmFieldInputTypeConst;
    attach_info->key_info.qual_info[6].input_arg = 1;

    attach_info->key_info.qual_types[7] = fg_info.qual_types[7];
    attach_info->key_info.qual_info[7].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[7].input_arg = 0;
    attach_info->key_info.qual_info[7].offset = PPH_PARSING_OFFSET_OFFSET;

    attach_info->key_info.qual_types[8] = fg_info.qual_types[8];
    attach_info->key_info.qual_info[8].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[8].input_arg = 0;
    attach_info->key_info.qual_info[8].offset = LAG_KEY_OFFSET;

    attach_info->payload_info.action_types[0] = fg_info.action_types[0];
    attach_info->payload_info.action_types[1] = fg_info.action_types[1];
    attach_info->payload_info.action_types[2] = fg_info.action_types[2];
    attach_info->payload_info.action_types[3] = fg_info.action_types[3];
    attach_info->payload_info.action_types[4] = fg_info.action_types[4];

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function creates field group in iPMF3, with all
 *  relevant information to it.
 *
 * \param [in] unit         - The unit number.
 * \param [out] fg_id       - Field group ID created.
 * \param [out] attach_info - Attach info.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_ftmh_ipmf3_group_config(
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_group_info_t fg_info;
    bcm_field_action_t ftmh_mc_system_header_size_action, ftmh_mc_parsing_offset_action, ftmh_mc_lag_key_void_action;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create user defined actions
     */
    SHR_IF_ERR_EXIT(appl_dnx_field_ftmh_mc_user_action_create(unit, "ftmh_system_header_size_adjust", bcmFieldActionSystemHeaderSizeAdjust, bcmFieldStageIngressPMF3, PACKET_SIZE_LENGTH, 0, &ftmh_mc_system_header_size_action));
    SHR_IF_ERR_EXIT(appl_dnx_field_ftmh_mc_user_action_create(unit, "ftmh_parsing_offset", bcmFieldActionParsingStartOffsetRaw, bcmFieldStageIngressPMF3, PPH_PARSING_OFFSET_ACTION_LENGTH, 0, &ftmh_mc_parsing_offset_action));
    SHR_IF_ERR_EXIT(appl_dnx_field_ftmh_mc_user_action_create(unit, "ftmh_mc_container_void", bcmFieldActionVoid, bcmFieldStageIngressPMF3, 1, 0, &ftmh_mc_lag_key_void_action));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;
    sal_strncpy_s((char *) fg_info.name, "FTMH_MC_IPMF3", sizeof(fg_info.name));

    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyContainer;

    fg_info.nof_actions = 4;
    fg_info.action_types[0] = ftmh_mc_system_header_size_action;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = ftmh_mc_parsing_offset_action;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_types[2] = bcmFieldActionTrunkHashKeySet;
    fg_info.action_with_valid_bit[2] = FALSE;
    fg_info.action_types[3] = ftmh_mc_lag_key_void_action;
    fg_info.action_with_valid_bit[3] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

    bcm_field_group_attach_info_t_init(attach_info);

    attach_info->key_info.nof_quals = fg_info.nof_quals;
    attach_info->payload_info.nof_actions = fg_info.nof_actions;

    attach_info->key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info->key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info->key_info.qual_info[0].input_arg = 0;
    attach_info->key_info.qual_info[0].offset = 0;

    attach_info->payload_info.action_types[0] = fg_info.action_types[0];
    attach_info->payload_info.action_types[1] = fg_info.action_types[1];
    attach_info->payload_info.action_types[2] = fg_info.action_types[2];
    attach_info->payload_info.action_types[3] = fg_info.action_types[3];

exit:
    SHR_FUNC_EXIT;
}

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
    bcm_field_context_t context_id_ipmf1,
    bcm_field_context_t context_id_ipmf3)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    bcm_field_presel_entry_id_t presel_entry_id;

    SHR_FUNC_INIT_VARS(unit);

    /*********************
     *  IPMF1 context
     *******************/
    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) (context_info.name), "FTMH_MC_IPMF1", sizeof(context_info.name));

    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF1, &context_info, &context_id_ipmf1));

    bcm_field_context_param_info_t_init(&param_info);
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
    param_info.param_val = bcmFieldSystemHeaderProfileNone;

    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF1, context_id_ipmf1, &param_info));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    /** For iPMF2, iPMF1 presel must be configured. */
    presel_entry_id.presel_id = dnx_data_field.preselector.default_ftmh_mc_presel_id_ipmf1_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.context_id = context_id_ipmf1;
    presel_entry_data.entry_valid = TRUE;

    /** Check that the port is the recycle port. */
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyPortClassPacketProcessing;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = PORT_CLASS_ID;
    presel_entry_data.qual_data[0].qual_mask = 0x7;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    /*********************
     *  IPMF3 context
     *********************/
    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) (context_info.name), "FTMH_MC_IPMF3", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF3, &context_info, &context_id_ipmf3));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);

    presel_entry_id.presel_id = dnx_data_field.preselector.default_ftmh_mc_presel_id_ipmf3_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF3;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.context_id = context_id_ipmf3;
    presel_entry_data.nof_qualifiers = 1;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyContextId;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = context_id_ipmf1;
    presel_entry_data.qual_data[0].qual_mask = 0x3F;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

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
    bcm_field_context_t context_id_ipmf1, context_id_ipmf3;
    bcm_field_group_t fg_id;
    bcm_field_group_attach_info_t attach_info;

    SHR_FUNC_INIT_VARS(unit);
    context_id_ipmf1 = dnx_data_field.context.default_ftmh_mc_context_get(unit);
    context_id_ipmf3 = dnx_data_field.context.default_ftmh_mc_ipmf3_context_get(unit);

    SHR_IF_ERR_EXIT(appl_dnx_field_ftmh_mc_context(unit, context_id_ipmf1, context_id_ipmf3));

    SHR_IF_ERR_EXIT(appl_dnx_field_ftmh_ipmf2_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id_ipmf1, &attach_info));

    SHR_IF_ERR_EXIT(appl_dnx_field_ftmh_ipmf3_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id_ipmf3, &attach_info));

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
    if (!dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_is_pp_used))
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP;
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
