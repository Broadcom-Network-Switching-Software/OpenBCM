/** \file appl_ref_itmh_init.c
 * 
 *
 * ITMH Programmable mode application procedures for BCM.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
#include <bcm_int/dnx/algo/field/algo_field.h>
/** shared */
#include <shared/shrextend/shrextend_debug.h>
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/reference/dnx/appl_ref_itmh_init.h>
/** bcm */
#include <bcm/field.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_context.h>
/** sal */
#include <sal/appl/sal.h>
#include <sal/core/libc.h>
#include <soc/sand/sand_aux_access.h>

/*
 * }
 */

/**
 * ITMH header :
 *        ___________________________  ____  _______  _____  ____  ________________  _____  _________  __________
 * field [ ITMH_BASE_EXTENSION_EXIST ][ TC ][ SNOOP ][ DST ][ DP ][ MIRROR_DISABLE ][ PPH ][ INJ_ASE ][ RESERVED ]
 *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~  ~~~~  ~~~~~~~  ~~~~~  ~~~~  ~~~~~~~~~~~~~~~~  ~~~~~  ~~~~~~~~~  ~~~~~~~~~~
 *  bits [           39:39           ][38:36][ 35:31][30:10][ 9:8][       7:7      ][ 6:5 ][   4:4   ][    3:0   ]
 *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~  ~~~~  ~~~~~~~  ~~~~~  ~~~~  ~~~~~~~~~~~~~~~~  ~~~~~  ~~~~~~~~~  ~~~~~~~~~~
 *      MSB                                                                                                     LSB
 *
 *
 * ITMH-Extension header :
 *        _______  ______  __________
 * field [ VALUE ][ TYPE ][ RESERVED ]
 *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  bits [ 23:2  ][  1:1 ][   0:0    ]
 *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *      MSB                         LSB
 *
 *
 *
 *
 *
 * IPMF2 Key :
 *        __________________  _____________________  ___________________  _______________________  ______________  _______________  ______________________  _____________  __________________
 * field [ ITMH_MIRROR_QUAL ][ ASE_INJ_QUAL_OFFSET ][ ITMH_OUT_LIF_QUAL ][ ITMH_EXT_PRESENT_QUAL ][ ITMH_DP_QUAL ][ ITMH_DST_QUAL ][ ITMH_SNOOP_DATA_QUAL ][ ITMH_TC_QUAL][ITMH_PPH_TYPE_QUAL]
 *        ~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~
 *  bits [          58      ][          57         ][       56:34       ][          33           ][    32:31     ][      30:10    ][        9:5           ][     4:2     ][       1:0        ]
 *        ~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~
 *      MSB                                                                                                                                                                                 LSB
 */

/*
 * DEFINEs
 * {
 */

/* *INDENT-OFF* */

/** IPMF2 Qualifiers info. */
static field_itmh_qual_info_t field_itmh_ipmf2_qual_info_array[ITMH_NOF_IPMF2_QUALIFIERS] = {
  /** qual_name  |  qual_id   |   qual_size     |  input_type    |    input_arg  |  qual_offset  */
    /** Offset of PPH Qualifier for ITMH Header[6:5] (2b) */
    {"pph_type_qual", 0, PPH_TYPE_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, PPH_TYPE_QUAL_OFFSET}},
    /**
     * DP Qualifier for ITMH Header[9:8] (2b)
     * SNOOP_DATA Qualifier for ITMH Header[35:31] (5b)
     * TC Qualifier for ITMH Header[38:36] (3b)
     * DST Qualifier for ITMH Header[30:10] (21b)
     */
    {"dp_dst_snp_tc_qual", 0, DP_DST_SNOOP_TC_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, DP_DST_SNOOP_TC_QUAL_OFFSET}},
    /** EXTENSION PRESENT qualifier in ITMH header [39:39] (1b) */
    {"ext_exist_qual", 0, EXT_PRESENT_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, EXT_PRESENT_QUAL_OFFSET}},
    /** OUT_LIF Qualifier for ITMH-Ext Header[23:2] (22b) */
    {"out_lif_dst_qual", 0, OUT_LIF_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, OUT_LIF_QUAL_OFFSET}},
    /** ASE_INJ Qualifier for ITMH Header[4:4] (1b) */
    {"ase_inj_qual", 0, ASE_INJ_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, ASE_INJ_QUAL_OFFSET}},
    /** MIRROR Qualifier for ITMH Header[7:7] (1b) */
    {"mirror_qual", 0, MIRROR_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, MIRROR_QUAL_OFFSET}},
    {NULL, bcmFieldQualifyRxTrapStrength, 0, {bcmFieldInputTypeMetaData, 0, 0}},
    {"itmh_oam_valid_bit", 0, J2_ITMH_OAM_VALID_BIT_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, J2_ITMH_OAM_VALID_BIT_QUAL_OFFSET}},
    {"itmh_fbr_hdr_set", 0, 4, {bcmFieldInputTypeConst, 0x5, 0}},
    {"itmh_fwd_l_idx", 0, J2_ITMH_FWD_LAYER_INDEX_QUAL_LENGTH,
        {bcmFieldInputTypeConst, 0x0, 0}}
};

/**
 * IPMF2 Action info.
 * For actions, which are not user define, we need only BCM action ID.
 * The User Defined Action IDs are used only for initializing, because of that their
 * value are set to zero.
 * Their values will be generated by bcm_field_action_create().
 */
static field_itmh_action_info_t field_itmh_ipmf2_action_info_array[ITMH_NOF_IPMF2_ACTIONS] = {
/** {action_name}   |  action_id  |    { stage | action_type | size | prefix_size | prefix_value | name  } */
    {NULL, bcmFieldActionPphPresentSet, {0}, FALSE},
    {NULL, bcmFieldActionPrioIntNew, {0}, FALSE},
    /**
     * Set the snoop_data msb to be a constant value of 0x0007f,
     * to set the snoop strength to be bigger then 0.
     * The encoding of this value is as follow:
     *  - bits 0:8 are corresponding to snoop_code
     *  - bits 9:11 are for snoop_strength
     */
    {"snoop_data_act", 0, {0, bcmFieldActionSnoopRaw, SNOOP_DATA_ACTION_LENGTH,
        15, 0x0007F, {0}}, FALSE},
    {"destination_act", 0, {0, bcmFieldActionForward, DST_ACTION_LENGTH,
        11, 0, {0}}, FALSE},
    {NULL, bcmFieldActionDropPrecedenceRaw, {0}, FALSE},
    {"ext_exist_act", 0, {0, bcmFieldActionVoid, EXT_EXIST_ACTION_LENGTH,
        31, 0, {0}}, FALSE},
    {"out_lif_act", 0, {0, bcmFieldActionVoid, OUT_LIF_ACTION_LENGTH,
        10, 0, {0}}, FALSE},
    {"udh3_1bit_ase_act", 0, {0, bcmFieldActionUDHData3, ASE_INJ_ACTION_LENGTH,
        31, 0, {0}}, FALSE},
    {"mirror_command_act", 0, {0, bcmFieldActionVoid, MIRROR_ACTION_LENGTH,
        31, 0x00000000, {0}}, FALSE},
    {"fwd_act_str_act", 0, {0, bcmFieldActionVoid, ITMH_FWD_STR_ACTION_LENGTH,
        28, 0x0, {0}}, FALSE},
    {NULL, bcmFieldActionFabricHeaderSetRaw, {0}, TRUE},
    {NULL, bcmFieldActionForwardingLayerIndex, {0}, FALSE}
};

/**
 * Array with all needed information about actions, which are using FEMs.
 */
static field_itmh_fem_action_info_t field_itmh_ipmf2_fem_action_info_array[ITMH_NOF_IPMF2_FEMS] = {
/** nof_conditions | condition_indexes | nof_extractions | nof_map_bits | array_id | fem_position_in_array | fem_input | condition_msb | fem_condition_info */
    /** OUT-LIF action */
    {8, {2, 3, 6, 7, 10, 11, 14, 15}, 1, 22, {1, 2},
      /*
       * bcm_field_fem_action_info_t fem_action_info
       */
      {
        /*
         * bcm_field_fem_input_info_t fem_input:
         *   Set input_offset     to 32 (This is the offset within key template)
         *   Set overriding_fg_id to 0
         */
        {32, 0},
        3, {{0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}},
        /** bcm_action  |  extraction_info */
        {{bcmFieldActionOutVport0Raw, {
            /** src_type                 |       offset | force_value */
            {bcmFieldFemExtractionOutputSourceTypeKey, 2, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 3, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 4, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 5, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 6, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 7, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 8, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 9, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 10, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 11, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 12, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 13, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 14, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 15, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 16, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 17, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 18, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 19, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 20, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 21, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 22, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 23, 0}}, 0}}}},
    /**
     * MIRROR action
     * Set the mirror_code_verify to be 0x1E0, because we have 32 mirror command where the '0' command
     * is mapped to 480th of 512 mirror_profiles.
     */
    {8, {1, 3, 5, 7, 9, 11, 13, 15}, 1, 9, {1, 3},
      {
        /*
         * bcm_field_fem_input_info_t fem_input:
         *   Set input_offset     to 32 (This is the offset within key template)
         *   Set overriding_fg_id to 0
         */
        {32, 0},
        28, {{0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}},
        {{bcmFieldActionMirrorIngressRaw, {
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1}},0}}}},
    /** TrapRaw action */
    {15, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}, 1, 13, {1, 4}, {{32, 0}, 29,
        {{0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}},
        {{bcmFieldActionTrapRaw, {
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1}}, 0}}}}
};

/**
 * Array, which contains information per field group. Like:
 * fg_id, fg_type, quals, actions, fem_info.
 */
static field_itmh_fg_info_t itmh_fg_info_array[ITMH_NOF_FG] = {
    /** IPMF2 field group info */
    {
        ITMH_IPMF2_FG_ID,   /** Field group ID */
        bcmFieldStageIngressPMF2,   /** Field stage */
        bcmFieldGroupTypeDirectExtraction,  /** Field group type */
        ITMH_NOF_IPMF2_QUALIFIERS,  /** Number of tested qualifiers */
        ITMH_NOF_IPMF2_ACTIONS, /** Number of tested actions */
        field_itmh_ipmf2_qual_info_array,   /** Qualifiers info */
        field_itmh_ipmf2_action_info_array, /** Actions info */
        field_itmh_ipmf2_fem_action_info_array  /** FEM actions info */
    }
};

/*
 * }
 */

/* *INDENT-ON* */

/*
 * MACROs
 * {
 */
/*
 * }
 */

/**
 * \brief
 *  This function is used to create a context and configure
 *  the relevant program for the testing.
 *
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage on which context will be create.
 * \param [in] context_id - Context id to be created.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
field_itmh_context(
    int unit,
    bcm_field_context_t context_id)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    bcm_field_presel_entry_id_t presel_entry_id;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) (context_info.name), "ITMH_IPMF1", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF1, &context_info, &context_id));
    bcm_field_context_param_info_t_init(&param_info);
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
    param_info.param_val = bcmFieldSystemHeaderProfileFtmh;
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF1, context_id, &param_info));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    /** For iPMF2, iPMF1 presel must be configured. */
    presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_presel_id_ipmf1_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.context_id = context_id;
    presel_entry_data.entry_valid = TRUE;

    /** Check that the FWD type is ITMH. */
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeTm;
    presel_entry_data.qual_data[0].qual_mask = 0x1F;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    bcm_field_context_param_info_t_init(&param_info);
    /** Context in IPMF2 stage will configure bytes to remove 1 layer. */
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerForwardingOffset1, 0);
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, context_id, &param_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Used to configure the user qualifiers.
 *
 * \param [in] unit - The unit number.
 * \param [in] itmh_qual_info - Contains all need information
 *                              for one user qualifier to be created.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
field_itmh_user_qual_config(
    int unit,
    field_itmh_qual_info_t * itmh_qual_info)
{
    bcm_field_qualifier_info_create_t qual_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_qualifier_info_create_t_init(&qual_info);
    sal_strncpy_s((char *) (qual_info.name), itmh_qual_info->name, sizeof(qual_info.name));
    qual_info.size = itmh_qual_info->qual_length;
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &(itmh_qual_info->qual_id)));

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
 * \param [in] itmh_action_info - Contains all need information
 *                                for one user action to be created.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
field_itmh_user_action_config(
    int unit,
    bcm_field_stage_t field_stage,
    field_itmh_action_info_t * itmh_action_info)
{
    bcm_field_action_info_t action_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_action_info_t_init(&action_info);

    action_info.action_type = itmh_action_info->action_info.action_type;
    sal_strncpy_s((char *) (action_info.name), itmh_action_info->name, sizeof(action_info.name));
    action_info.size = itmh_action_info->action_info.size;
    action_info.stage = field_stage;
    action_info.prefix_size = itmh_action_info->action_info.prefix_size;
    action_info.prefix_value = itmh_action_info->action_info.prefix_value;

    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &(itmh_action_info->action_id)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function is configuring all needed FEM actions.
 *
 * \param [in] unit - The unit number.
 * \param [in] fg_id - The field group ID, to which the FEM action
 *                     should be added.
 * \param [in] action_id - ID of the action, to be parsed to extraction info.
 *                         Used only in case of user defined action "ITMH_OUT_LIF_MC_DST_ACTION".
 * \param [in] itmh_fem_action_info - Contains all need information
 *                                    for one FEM action to be constructed.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
field_itmh_fem_action_config(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_action_t action_id,
    field_itmh_fem_action_info_t * itmh_fem_action_info)
{
    int mapping_bits_index, conditions_index, extractions_index;
    bcm_field_fem_action_info_t fem_action_info;
    int *condition_id;
    bcm_field_action_priority_t action_priority;

    SHR_FUNC_INIT_VARS(unit);

    condition_id = itmh_fem_action_info->condition_ids;

    bcm_field_fem_action_info_t_init(&fem_action_info);

    fem_action_info.fem_input.input_offset = itmh_fem_action_info->fem_action_info.fem_input.input_offset;
    fem_action_info.condition_msb = itmh_fem_action_info->fem_action_info.condition_msb;
    for (conditions_index = 0; conditions_index < itmh_fem_action_info->nof_conditions; conditions_index++)
    {
        fem_action_info.fem_condition[condition_id[conditions_index]].extraction_id =
            itmh_fem_action_info->fem_action_info.fem_condition[conditions_index].extraction_id;
        fem_action_info.fem_condition[condition_id[conditions_index]].is_action_valid =
            itmh_fem_action_info->fem_action_info.fem_condition[conditions_index].is_action_valid;
    }
    for (extractions_index = 0; extractions_index < itmh_fem_action_info->nof_extractions; extractions_index++)
    {
        fem_action_info.fem_extraction[extractions_index].action_type =
            itmh_fem_action_info->fem_action_info.fem_extraction[extractions_index].action_type;
        fem_action_info.fem_extraction[extractions_index].increment =
            itmh_fem_action_info->fem_action_info.fem_extraction[extractions_index].increment;
        for (mapping_bits_index = 0; mapping_bits_index < itmh_fem_action_info->nof_mapping_bits; mapping_bits_index++)
        {
            fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].offset =
                itmh_fem_action_info->fem_action_info.fem_extraction[extractions_index].
                output_bit[mapping_bits_index].offset;
            fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].forced_value =
                itmh_fem_action_info->fem_action_info.fem_extraction[extractions_index].
                output_bit[mapping_bits_index].forced_value;
            fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].source_type =
                itmh_fem_action_info->fem_action_info.fem_extraction[extractions_index].
                output_bit[mapping_bits_index].source_type;
        }
    }
    action_priority = BCM_FIELD_ACTION_POSITION(itmh_fem_action_info->field_array_n_position.array_id,
                                                itmh_fem_action_info->field_array_n_position.fem_position_in_array);
    SHR_IF_ERR_EXIT(bcm_field_fem_action_add(unit, 0, fg_id, action_priority, &fem_action_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function creates field group, with all
 *  relevant information to it and adding it to the context.
 *
 * \param [in] unit - The unit number.
 * \param [in] itmh_fg_info - Contains all need information
 *                            for one group to be created.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
field_itmh_group_config(
    int unit,
    field_itmh_fg_info_t * itmh_fg_info,
    bcm_field_context_t context_id)
{
    int qual_index, action_index, fem_index;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);

    fg_info.fg_type = itmh_fg_info->fg_type;
    fg_info.stage = itmh_fg_info->stage;
    sal_strncpy((char *) (fg_info.name), "ITMH", sizeof(fg_info.name));
    fg_info.nof_quals = itmh_fg_info->nof_quals;
    fg_info.nof_actions = itmh_fg_info->nof_actions;
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    /**
     * Iterate over all qualifiers for current field group. Using field_itmh_user_qual_config()
     * to create all user define qualifiers internal function.
     */
    for (qual_index = 0; qual_index < fg_info.nof_quals; qual_index++)
    {
        if (itmh_fg_info->itmh_qual_info[qual_index].name != NULL)
        {
            SHR_IF_ERR_EXIT(field_itmh_user_qual_config(unit, &(itmh_fg_info->itmh_qual_info[qual_index])));
        }

        fg_info.qual_types[qual_index] = itmh_fg_info->itmh_qual_info[qual_index].qual_id;
        attach_info.key_info.qual_types[qual_index] = itmh_fg_info->itmh_qual_info[qual_index].qual_id;
        attach_info.key_info.qual_info[qual_index].input_type =
            itmh_fg_info->itmh_qual_info[qual_index].qual_attach_info.input_type;
        attach_info.key_info.qual_info[qual_index].input_arg =
            itmh_fg_info->itmh_qual_info[qual_index].qual_attach_info.input_arg;
        attach_info.key_info.qual_info[qual_index].offset =
            itmh_fg_info->itmh_qual_info[qual_index].qual_attach_info.offset;
    }

    /**
     * Iterate over all actions for current field group. For those, which are user define,
     * use field_itmh_user_action_config() internal function.
     */
    for (action_index = 0; action_index < fg_info.nof_actions; action_index++)
    {
        if (itmh_fg_info->itmh_action_info[action_index].name != NULL)
        {
            SHR_IF_ERR_EXIT(field_itmh_user_action_config
                            (unit, fg_info.stage, &(itmh_fg_info->itmh_action_info[action_index])));
        }
        fg_info.action_types[action_index] = itmh_fg_info->itmh_action_info[action_index].action_id;
        attach_info.payload_info.action_types[action_index] = itmh_fg_info->itmh_action_info[action_index].action_id;
        fg_info.action_with_valid_bit[action_index] =
            itmh_fg_info->itmh_action_info[action_index].action_with_valid_bit;
    }

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &(itmh_fg_info->fg_id)));

    /** Configuring the FEM actions. */
    if (fg_info.stage == bcmFieldStageIngressPMF2)
    {
        for (fem_index = 0; fem_index < ITMH_NOF_IPMF2_FEMS; fem_index++)
        {
            SHR_IF_ERR_EXIT(field_itmh_fem_action_config
                            (unit, itmh_fg_info->fg_id, itmh_fg_info->itmh_action_info[6].action_id,
                             &(itmh_fg_info->itmh_fem_action_info[fem_index])));
        }
    }

    /** Attach the created field group to the context. */
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, itmh_fg_info->fg_id, context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function sets all required HW configuration for ITMH
 *  processing when working in ITMH programmable mode.
 *  Configures 1 field database of direct extraction
 *  mode in order to extract all itmh information form the
 *  header and extension header.
 *  The header information is used to perform actions (such as
 *  mirror, snoop command, change traffic class, drop
 *  precedence) In adding the new header profile is set
 *  according to the type (unicast/multicast).
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e
appl_dnx_field_itmh_init(
    int unit)
{
    int fg_index;
    bcm_field_context_t context_id;
    SHR_FUNC_INIT_VARS(unit);

    context_id = dnx_data_field.context.default_itmh_context_get(unit);

    SHR_IF_ERR_EXIT(field_itmh_context(unit, context_id));

    /**
     * Parsing the global array "itmh_fg_info_array[]" with all needed
     * information for the field group.
     */
    for (fg_index = 0; fg_index < ITMH_NOF_FG; fg_index++)
    {
        SHR_IF_ERR_EXIT(field_itmh_group_config(unit, &itmh_fg_info_array[fg_index], context_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
