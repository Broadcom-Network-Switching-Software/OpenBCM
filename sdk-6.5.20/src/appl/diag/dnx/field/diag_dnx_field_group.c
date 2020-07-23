/** \file diag_dnx_field_group.c
 *
 * Diagnostics procedures, for DNX, for 'group' operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDDIAGSDNX
/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "diag_dnx_field_group.h"
#include <bcm_int/dnx/field/field_group.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_context_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_qualifier_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/tcam/tcam_handler.h>
#include "diag_dnx_field_utils.h"

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/** Get the number of cores from DNX data. */
#define DIAG_DNX_FIELD_GROUP_NOF_CORES  dnx_data_device.general.nof_cores_get(unit)
/*
 * }
 */
/*
 * TYPEDEFs
 * {
 */
/*
 * }
 */
/*
 * MACROs
 * {
 */
/*
 * Options
 * {
 */
/*
 * }
 */
/*
 * }
 */
/* *INDENT-OFF* */
/*
 * Global and Static
 * {
 */
static shr_error_e
diag_dnx_field_group_info_init(int unit)
{
    bcm_field_group_t fg_id;
    bcm_field_group_info_t fg_info;
    void *dest_char;
    bcm_field_action_info_t action_info;
    bcm_field_action_t action_prefix_test_epmf_tc_action;
    bcm_field_action_t action_prefix_test_epmf_ace_key_action;
    bcm_field_group_attach_info_t attach_info;
    int ii;
    SHR_FUNC_INIT_VARS(unit);


    /* Add TCAM field group */
    bcm_field_group_info_t_init(&fg_info);

    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "field_group_info_diag_tcam", sizeof(fg_info.name));
    fg_info.nof_quals = 3;
    fg_info.nof_actions = 3;


    fg_info.qual_types[0] = bcmFieldQualifyGeneratedTtl;
    fg_info.qual_types[1] = bcmFieldQualifyIntPriority;
    fg_info.qual_types[2] = bcmFieldQualifyPacketIsIEEE1588;

    fg_info.action_types[0] = bcmFieldActionAdmitProfile;
    fg_info.action_with_valid_bit[0] = TRUE;
    fg_info.action_types[1] = bcmFieldActionPrioIntNew;
    fg_info.action_with_valid_bit[1] = TRUE;
    fg_info.action_types[2] = bcmFieldActionPphPresentSet;
    fg_info.action_with_valid_bit[2] = TRUE;

    fg_id = 70;
    SHR_IF_ERR_EXIT(bcm_field_group_add
                    (unit, (BCM_FIELD_FLAG_MSB_RESULT_ALIGN | BCM_FIELD_FLAG_WITH_ID), &fg_info, &fg_id));

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for(ii=0; ii< fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
        attach_info.key_info.qual_info[ii].input_type = bcmFieldInputTypeMetaData;
    }
    for(ii=0; ii<fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, 0 /*default context*/, &attach_info));

    /* Add CONST field group */
    bcm_field_group_info_t_init(&fg_info);

    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionPrioIntNew;
    action_info.prefix_size = 3;
    action_info.prefix_value = 0x1;
    action_info.size = 0;
    action_info.stage = bcmFieldStageEgress;
    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &action_prefix_test_epmf_tc_action));

    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionAceEntryId;
    action_info.prefix_size = 22;
    action_info.prefix_value = 0x2;
    action_info.size = 0;
    action_info.stage = bcmFieldStageEgress;
    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &action_prefix_test_epmf_ace_key_action));

    fg_info.fg_type = bcmFieldGroupTypeConst;
    fg_info.nof_quals = 0;
    fg_info.stage = bcmFieldStageEgress;
    fg_info.nof_actions = 2;
    fg_info.action_types[0] = action_prefix_test_epmf_tc_action;
    fg_info.action_with_valid_bit[0]=FALSE;
    fg_info.action_types[1] = action_prefix_test_epmf_ace_key_action;
    fg_info.action_with_valid_bit[1]=FALSE;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "field_group_info_diag_const", sizeof(fg_info.name));

    fg_id = 71;
    SHR_IF_ERR_EXIT(bcm_field_group_add
                    (unit, (BCM_FIELD_FLAG_MSB_RESULT_ALIGN | BCM_FIELD_FLAG_WITH_ID), &fg_info, &fg_id));
exit:
    SHR_FUNC_EXIT;
}


/**
 *  'Help' description for Group display (shell commands).
 */
static sh_sand_man_t Field_group_list_man = {
    .brief = "'Group'- displays the basic info about a specific field group or range of FGs "
             "on a certain stage (filter by field group id and field stage)",
    .full = "'Group' display for a range of field groups or field stage. Range is specified via 'group'.\r\n"
            "If no 'group' is specified then '0'-'nof_fgs-1' is assumed (0-127).\r\n"
            "If only one value is specified for group then this 'single value' range is assumed.\r\n"
            "If no stage is specified then 'IPMF1'-'EPMF' is assumed (0-3).\r\n",
    .synopsis = "[group=<lower value>-<upper value>]"
                "[stage=<<IPMF1 | IPMF2 | ... | IFWD2>-<<IPMF1 | IPMF2 | ... | IFWD2>>]",
    .examples = "group=0-20 stage=IPMF1-IPMF3",
};
static sh_sand_option_t Field_group_list_options[] = {
    {DIAG_DNX_FIELD_OPTION_GROUP,  SAL_FIELD_TYPE_UINT32, "Lowest-highest field group to get its info",       "MIN-MAX",       (void *)Field_group_enum_table, "MIN-MAX"},
    {DIAG_DNX_FIELD_OPTION_STAGE,  SAL_FIELD_TYPE_UINT32, "Lowest-highest field stage to get key info for",   "STAGE_LOWEST-STAGE_HIGHEST",    (void *)Field_group_diag_stage_enum_table, "STAGE_LOWEST-STAGE_HIGHEST"},
    {NULL}
};

/**
 *  'Help' description for Group display (shell commands).
 */
static sh_sand_man_t Field_group_info_man = {
    .brief = "'Group'- displays the full info about a specific field group (filter by field group id)",
    .full = "'Group' display info for a specific field group.\r\n"
      "If no 'group' ('0'-'nof_fgs-1' --> (0-127)) is specified then an error will occur.\r\n",
    .synopsis = "[group=<lower value>-<upper value>]",
    .examples = "group=0\n"
                "group=71\n"
                "group=70",
    .init_cb =  diag_dnx_field_group_info_init,
};
static sh_sand_option_t Field_group_info_options[] = {
    {DIAG_DNX_FIELD_OPTION_GROUP,          SAL_FIELD_TYPE_UINT32, "Field group to get its info",           NULL   ,(void *)Field_group_enum_table, "MIN-MAX"},
    {NULL}
};
/*
 * }
 */
/* *INDENT-ON* */

/**
 * \brief
 *   This function prepares and sets common basic information about a specific FG, like
 *   id, name, stage and type.
 * \param [in] unit - The unit number.
 * \param [in] fg_iter - FG ID for which information will be shown
 * \param [in] fg_info_p - Pointer, which contains information about the
 *  the given Field Group: name, stage and type.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_info_set(
    int unit,
    dnx_field_group_t fg_iter,
    dnx_field_group_full_info_t * fg_info_p,
    prt_control_t * prt_ctr)
{
    /** Sting to store the converted FG Type. */
    char converted_fg_type[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    bcm_field_stage_t bcm_stage;

    SHR_FUNC_INIT_VARS(unit);

    /** Set the FG ID. */
    PRT_CELL_SET("%d", fg_iter);
    /**
     * Check if Field Group name is being provided, in case no set N/A as default value,
     * in other case set the provided name.
     */
    if (sal_strncmp(fg_info_p->group_basic_info.name, "", sizeof(fg_info_p->group_basic_info.name)))
    {
        PRT_CELL_SET("%s", fg_info_p->group_basic_info.name);
    }
    else
    {
        PRT_CELL_SET("N/A");
    }

    /** Convert and set the FG Type. */
    SHR_IF_ERR_EXIT(diag_dnx_field_utils_group_type_conversion
                    (unit, fg_info_p->group_basic_info.fg_type, converted_fg_type));
    PRT_CELL_SET("%s", converted_fg_type);

    /** Convert DNX to BCM Field Stage */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, fg_info_p->group_basic_info.field_stage, &bcm_stage));
    PRT_CELL_SET("%s", dnx_field_bcm_stage_text(bcm_stage));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets qualifier info for a given qual used by a specific FG, like
 *   name, size and lsb.
 * \param [in] unit - The unit number.
 * \param [in] qual_iter - Id of the current qualifier.
 * \param [in] fg_info_p - Pointer, from which information for the
 *  current qualifier will be retrieved.
 * \param [in] qual_is_packed - Whether the qualifier is packed.
 * \param [in] qual_packing_first - If the qual is packed, if it is the first qualifer in the packed bunch.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_info_qual_print(
    int unit,
    int qual_iter,
    dnx_field_group_full_info_t * fg_info_p,
    int qual_is_packed,
    int qual_packing_first,
    prt_control_t * prt_ctr)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Set current qualifier info. */
    PRT_CELL_SET("%s", dnx_field_dnx_qual_text
                 (unit, fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].qual_type));
    PRT_CELL_SET("%d", fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size);
    if (fg_info_p->group_basic_info.fg_type == DNX_FIELD_GROUP_TYPE_KBP)
    {
        int qual_size_on_key;
        qual_size_on_key = fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size;
        if ((qual_size_on_key % 8) != 0)
        {
            qual_size_on_key += 8 - (qual_size_on_key % 8);
        }
        if (qual_is_packed == FALSE || qual_packing_first)
        {
            PRT_CELL_SET("%d", qual_size_on_key);
        }
        else
        {
            PRT_CELL_SET("-");
        }

        if (fg_info_p->group_basic_info.qual_is_ranged[qual_iter])
        {
            PRT_CELL_SET("%s", "Yes");
        }
        else
        {
            PRT_CELL_SET("%s", "No");
        }

        if (qual_is_packed)
        {
            PRT_CELL_SET("%s", "Yes");
        }
        else
        {
            PRT_CELL_SET("%s", "No");
        }
    }
    else
    {
        PRT_CELL_SET("%d", fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].lsb);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets qualifier info for a given qual used by a specific DE FG, like
 *   name, size and lsb.
 * \param [in] unit - The unit number.
 * \param [in] qual_iter - Id of the current qualifier.
 * \param [in] qual_size - Size of the current qualifier.
 * \param [in] qual_lsb - Lsb of the current qualifier,
 *  default should be '0'. Only in cases when the current qualifier
 *  is shared between two actions the value can be different.
 * \param [in] qual_msb - Msb of the current qualifier,
 *  default should be '0'. Only in cases when the current qualifier
 *  is shared between two actions the value can be different
 * \param [in] fg_info_p - Pointer, from which information for the
 *  current qualifier will be retrieved.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_info_de_qual_print(
    int unit,
    int qual_iter,
    uint8 qual_size,
    uint8 qual_lsb,
    uint8 qual_msb,
    dnx_field_group_full_info_t * fg_info_p,
    prt_control_t * prt_ctr)
{
    int qual_additional_offset;

    SHR_FUNC_INIT_VARS(unit);

    qual_additional_offset = 0;

    /** Set current qualifier info. */
    if (qual_lsb != 0 || qual_msb != 0)
    {
        PRT_CELL_SET("%s [%d:%d]", dnx_field_dnx_qual_text
                     (unit, fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].qual_type), qual_lsb,
                     qual_msb);

        qual_additional_offset = qual_lsb;
    }
    else
    {
        PRT_CELL_SET("%s", dnx_field_dnx_qual_text
                     (unit, fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].qual_type));
    }

    PRT_CELL_SET("%d", qual_size);
    PRT_CELL_SET("%d", (fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].lsb + qual_additional_offset));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets action info for a given action used by a specific FG, like
 *   name, size and offset.
 * \param [in] unit - The unit number.
 * \param [in] fg_iter - Field Group, which action belongs to.
 * \param [in] action_iter - Id of the current action.
 * \param [in] fg_info_p - Pointer, from which information for the
 *  current action will be retrieved.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_info_action_print(
    int unit,
    dnx_field_group_t fg_iter,
    int action_iter,
    dnx_field_group_full_info_t * fg_info_p,
    prt_control_t * prt_ctr)
{
    int action_offset;
    unsigned int action_size;
    dnx_field_action_t base_dnx_action;
    bcm_field_action_t bcm_action;
    dnx_field_action_t dnx_action;
    int is_user_defined;

    SHR_FUNC_INIT_VARS(unit);

    action_offset = 0;

    dnx_action = fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[action_iter].dnx_action;
    SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm
                    (unit, fg_info_p->group_basic_info.field_stage, dnx_action, &bcm_action));
    /**
     * Check if the action is user defined, then we should get its base action.
     */
    is_user_defined = (DNX_ACTION_CLASS(dnx_action) == DNX_FIELD_ACTION_CLASS_USER);
    if (is_user_defined)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_base_action
                        (unit, fg_info_p->group_basic_info.field_stage, dnx_action, &base_dnx_action));
    }
    else
    {
        base_dnx_action = DBAL_ENUM_FVAL_IPMF1_ACTION_INVALID;
    }

    /** Get action size for the current action. */
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, fg_info_p->group_basic_info.field_stage,
                                                  fg_info_p->group_full_info.
                                                  actions_payload_info.actions_on_payload_info[action_iter].dnx_action,
                                                  &action_size));

    /** Take the offset for the current action. */
    SHR_IF_ERR_EXIT(dnx_field_group_action_offset_get(unit, fg_iter,
                                                      fg_info_p->group_full_info.
                                                      actions_payload_info.actions_on_payload_info[action_iter].
                                                      dnx_action, &action_offset));

    /** Set current action information. */
    PRT_CELL_SET("%s", dnx_field_dnx_action_text(unit,
                                                 fg_info_p->group_full_info.
                                                 actions_payload_info.actions_on_payload_info[action_iter].dnx_action));

    /** If the action is user defined, then we print the base action type. */
    if (is_user_defined)
    {
        PRT_CELL_SET("%s (UDA ID: %d)", dnx_field_dnx_action_text(unit, base_dnx_action),
                     (bcm_action - BCM_FIELD_FIRST_USER_ACTION_ID));
    }
    else
    {
        PRT_CELL_SET("%s", "N/A");
    }

    if (fg_info_p->group_basic_info.fg_type != DNX_FIELD_GROUP_TYPE_CONST)
    {
        PRT_CELL_SET("%d", action_size);
        PRT_CELL_SET("%d", action_offset);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets action info for a given action used by a specific DE FG, like
 *   name, size and offset.
 * \param [in] unit - The unit number.
 * \param [in] fg_iter - Field Group, which action belongs to.
 * \param [in] action_iter - Id of the current action.
 * \param [in] action_size - Id of the current action.
 * \param [in] action_lsb - Lsb of the current action,
 *  default should be '0'. Only in cases when the current action
 *  is shared between two qualifiers the value can be different.
 * \param [in] action_msb - Msb of the current action,
 *  default should be '0'. Only in cases when the current action
 *  is shared between two qualifiers the value can be different
 * \param [in] fg_info_p - Pointer, from which information for the
 *  current action will be retrieved.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_info_de_action_print(
    int unit,
    dnx_field_group_t fg_iter,
    int action_iter,
    unsigned int action_size,
    unsigned int action_lsb,
    unsigned int action_msb,
    dnx_field_group_full_info_t * fg_info_p,
    prt_control_t * prt_ctr)
{
    int action_offset;
    int action_additional_offset;
    dnx_field_action_t base_dnx_action;
    bcm_field_action_t bcm_action;
    dnx_field_action_t dnx_action;
    int is_user_defined;

    SHR_FUNC_INIT_VARS(unit);

    action_offset = 0;
    action_additional_offset = 0;

    dnx_action = fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[action_iter].dnx_action;
    SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm
                    (unit, fg_info_p->group_basic_info.field_stage, dnx_action, &bcm_action));
    /**
     * Check if the action is user defined, then we should get its base action.
     */
    is_user_defined = (DNX_ACTION_CLASS(dnx_action) == DNX_FIELD_ACTION_CLASS_USER);
    if (is_user_defined)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_base_action
                        (unit, fg_info_p->group_basic_info.field_stage, dnx_action, &base_dnx_action));
    }
    else
    {
        base_dnx_action = DBAL_ENUM_FVAL_IPMF1_ACTION_INVALID;
    }

    /** Take the offset for the current action. */
    SHR_IF_ERR_EXIT(dnx_field_group_action_offset_get(unit, fg_iter,
                                                      fg_info_p->group_full_info.
                                                      actions_payload_info.actions_on_payload_info[action_iter].
                                                      dnx_action, &action_offset));

    if (fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[action_iter].dont_use_valid_bit)
    {
        if (action_lsb != 0 || action_msb != 0)
        {
            /** Set current action information. */
            PRT_CELL_SET("%s [%d:%d]", dnx_field_dnx_action_text(unit,
                                                                 fg_info_p->group_full_info.
                                                                 actions_payload_info.actions_on_payload_info
                                                                 [action_iter].dnx_action), action_lsb, action_msb);
            action_additional_offset = action_lsb;
        }
        else
        {
            /** Set current action information. */
            PRT_CELL_SET("%s", dnx_field_dnx_action_text(unit,
                                                         fg_info_p->group_full_info.
                                                         actions_payload_info.actions_on_payload_info[action_iter].
                                                         dnx_action));
        }
    }
    else
    {
        if (action_lsb == 0 && action_msb != 0)
        {
            /** Set current action information. */
            PRT_CELL_SET("%s [%d:%d] + condition", dnx_field_dnx_action_text(unit,
                                                                             fg_info_p->group_full_info.
                                                                             actions_payload_info.actions_on_payload_info
                                                                             [action_iter].dnx_action), action_lsb,
                         action_msb);
        }
        else if (action_lsb != 0 && action_msb > action_size)
        {
            /** Set current action information. */
            PRT_CELL_SET("%s [%d:%d]", dnx_field_dnx_action_text(unit,
                                                                 fg_info_p->group_full_info.
                                                                 actions_payload_info.actions_on_payload_info
                                                                 [action_iter].dnx_action), action_lsb, action_msb);
            action_additional_offset = action_lsb;
        }
        else
        {
            /** Set current action information. */
            PRT_CELL_SET("%s + condition", dnx_field_dnx_action_text(unit,
                                                                     fg_info_p->group_full_info.
                                                                     actions_payload_info.actions_on_payload_info
                                                                     [action_iter].dnx_action));
        }
    }

    /** If the action is user defined, then we print the base action type. */
    if (is_user_defined)
    {
        PRT_CELL_SET("%s (UDA ID: %d)", dnx_field_dnx_action_text(unit, base_dnx_action),
                     (bcm_action - BCM_FIELD_FIRST_USER_ACTION_ID));
    }
    else
    {
        PRT_CELL_SET("%s", "N/A");
    }

    PRT_CELL_SET("%d", action_size);
    PRT_CELL_SET("%d", (action_offset + action_additional_offset));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function presents contexts to which the given FG has being attached.
 *   Presents Context ID and Name.
 * \param [in] unit - The unit number.
 * \param [in] fg_iter - FG ID for which contexts will be presented.
 * \param [in] field_stage - Field stage on which the FG was created.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_info_context_print(
    int unit,
    dnx_field_group_t fg_iter,
    dnx_field_stage_e field_stage,
    sh_sand_control_t * sand_control)
{
    int cs_iter;
    int do_cs_display;
    char context_name[DBAL_MAX_STRING_LENGTH];
    dnx_field_context_t context_id_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Retrieve all valid contexts for the current FG. */
    SHR_IF_ERR_EXIT(dnx_field_group_context_id_arr_get(unit, fg_iter, context_id_arr));

    PRT_TITLE_SET("Contexts for FG %d", fg_iter);
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");

    do_cs_display = FALSE;

    /** Iterate over all valid context for the current Field Group and present brief info about them (ID, name). */
    for (cs_iter = 0; cs_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS &&
         context_id_arr[cs_iter] != DNX_FIELD_CONTEXT_ID_INVALID; cs_iter++)
    {
        /** Take the name for current context from the SW state. */
        sal_memset(context_name, 0, sizeof(context_name));
        SHR_IF_ERR_EXIT(dnx_field_context_sw.context_info.name.
                        value.stringget(unit, context_id_arr[cs_iter], field_stage, context_name));
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%d", context_id_arr[cs_iter]);
        /**
         * Check if Context name is being provided, in case no set N/A as default value,
         * in other case set the provided name.
         */
        if (sal_strncmp(context_name, "", sizeof(context_name)))
        {
            PRT_CELL_SET("%s", context_name);
        }
        else
        {
            PRT_CELL_SET("N/A");
        }

        do_cs_display = TRUE;
    }
    if (do_cs_display)
    {
        PRT_COMMITX;
    }
    else
    {
        /*
         * If nothing was printed then just clear loaded PRT memory.
         * This could be left to 'exit' but it is clearer here.
         */
        PRT_FREE;
        LOG_CLI_EX("\r\n" "NO allocated Context was found on specified FG ID = %d!! %s%s%s\r\n\n", fg_iter,
                   EMPTY, EMPTY, EMPTY);
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function presents contexts to which the given External FG has being attached.
 *   Presents Context ID and Name.
 * \param [in] unit - The unit number.
 * \param [in] fg_iter - FG ID for which contexts will be presented.
 * \param [in] field_stage - Field stage on which the FG was created.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_info_external_context_print(
    int unit,
    dnx_field_group_t fg_iter,
    dnx_field_stage_e field_stage,
    sh_sand_control_t * sand_control)
{
    int cs_iter;
    int do_cs_display;
    dnx_field_context_apptype_info_t apptype_info;
    bcm_field_AppType_t apptype;
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id;
    dnx_field_context_t context_id_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Retrieve all valid contexts for the current FG. */
    SHR_IF_ERR_EXIT(dnx_field_group_context_id_arr_get(unit, fg_iter, context_id_arr));

    PRT_TITLE_SET("Contexts for FG %d", fg_iter);
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");

    do_cs_display = FALSE;

    /** Iterate over all valid context for the current Field Group and present brief info about them (ID, name). */
    for (cs_iter = 0; cs_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS &&
         context_id_arr[cs_iter] != DNX_FIELD_CONTEXT_ID_INVALID; cs_iter++)
    {
        opcode_id = context_id_arr[cs_iter];
        SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_apptype_dnx_to_bcm(unit, opcode_id, &apptype));
        SHR_IF_ERR_EXIT(dnx_field_context_apptype_info_get(unit, apptype, &apptype_info));
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%d (AT)", apptype);
        /**
         * Check if Context name is being provided, in case no set N/A as default value,
         * in other case set the provided name.
         */
        if (sal_strncmp(apptype_info.name, "", sizeof(apptype_info.name)))
        {
            PRT_CELL_SET("%s", apptype_info.name);
        }
        else
        {
            PRT_CELL_SET("N/A");
        }

        do_cs_display = TRUE;
    }
    if (do_cs_display)
    {
        PRT_COMMITX;
    }
    else
    {
        /*
         * If nothing was printed then just clear loaded PRT memory.
         * This could be left to 'exit' but it is clearer here.
         */
        PRT_FREE;
        LOG_CLI_EX("\r\n" "NO allocated Context was found on specified FG ID = %d!! %s%s%s\r\n\n", fg_iter,
                   EMPTY, EMPTY, EMPTY);
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares and sets key and payload information about a specific FG.
 *   Like Qualifiers/Actions name, size and lsb will be presented.
 * \param [in] unit - The unit number.
 * \param [in] fg_iter - FG ID for which key and payload information will be presented.
 * \param [in] fg_info_p - Pointer, which contains information about the
 *  key and payload of the given Field Group.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_info_key_and_payload_print(
    int unit,
    dnx_field_group_t fg_iter,
    dnx_field_group_full_info_t * fg_info_p,
    sh_sand_control_t * sand_control)
{
    int qual_action_iter;
    int max_num_qual_actions;
    int qual_left, action_left;
    uint8 qual_is_packed[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG] = { 0 };
    uint8 qual_is_packed_first[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG] = { 0 };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Used as flags to indicate if we have any quals/actions to print. */
    qual_left = action_left = TRUE;
    /** The max number of qualifiers and actions per field group (32). */
    max_num_qual_actions =
        MAX(dnx_data_field.group.nof_quals_per_fg_get(unit), dnx_data_field.group.nof_action_per_fg_get(unit));

    PRT_TITLE_SET("Key & Payload Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qualifier");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Size");
    if (fg_info_p->group_basic_info.fg_type == DNX_FIELD_GROUP_TYPE_KBP)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Size on Key");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Ranged");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Packed");
    }
    else
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Lsb");
    }
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, " ");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Base Action");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Lsb");

    if (fg_info_p->group_basic_info.fg_type == DNX_FIELD_GROUP_TYPE_KBP)
    {
        int qual_index;
        dbal_tables_e table_id;
        dbal_fields_e qual_field_id;
        uint8 is_packed;
        uint8 qualis_packed_prev = FALSE;
        SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_iter, &table_id));
        for (qual_index = 0; qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
             fg_info_p->group_full_info.key_template.key_qual_map[qual_index].qual_type !=
             DNX_FIELD_QUAL_TYPE_INVALID; qual_index++)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual
                            (unit, DNX_FIELD_STAGE_EXTERNAL,
                             fg_info_p->group_full_info.key_template.key_qual_map[qual_index].qual_type,
                             &qual_field_id));
            SHR_IF_ERR_EXIT(dbal_table_field_is_packed_get(unit, table_id, qual_field_id, &is_packed));
            if (is_packed || qualis_packed_prev)
            {
                qual_is_packed[qual_index] = TRUE;
            }
            if (is_packed && (qualis_packed_prev == FALSE))
            {
                qual_is_packed_first[qual_index] = TRUE;
            }
            qualis_packed_prev = is_packed;
        }
    }

    /** Iterate over maximum number of qualifiers and actions per FG, to retrieve information about these, which are valid. */
    for (qual_action_iter = 0; qual_action_iter < max_num_qual_actions; qual_action_iter++)
    {
        /**
         * Check if the qual_action_iter is still in the qualifiers range (DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) and if the
         * current qualifier is invalid (DNX_FIELD_QUAL_TYPE_INVALID), we should set the qual_left to FALSE,
         * which will indicate that we should stop printing any info related to qualifiers.
         */
        if (qual_action_iter < dnx_data_field.group.nof_quals_per_fg_get(unit)
            && fg_info_p->group_full_info.key_template.key_qual_map[qual_action_iter].qual_type ==
            DNX_FIELD_QUAL_TYPE_INVALID)
        {
            qual_left = FALSE;
        }
        /**
         * Check if the qual_action_iter is still in the actions range (DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG) and if the
         * current action is invalid (DNX_FIELD_ACTION_INVALID), we should set the action_left to FALSE,
         * which will indicate that we should stop printing any info related to actions.
         */
        if (qual_action_iter < dnx_data_field.group.nof_action_per_fg_get(unit)
            && fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[qual_action_iter].dnx_action ==
            DNX_FIELD_ACTION_INVALID)
        {
            action_left = FALSE;
        }
        /**
         * In case we reached the end of both qualifiers and actions arrays,
         * which means that qual_left and action_left flags are being set to FALSE.
         * We have to break and to stop printing information for both.
         */
        if (!qual_left && !action_left)
        {
            break;
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        /**
         * If we still have any valid qualifiers (qual_left), then print information about them.
         * Otherwise skip the cells, which are relevant to the qualifiers,
         * as it is mentioned in the 'else' statement.
         */
        if (qual_left)
        {
            /** Print the qualifiers info. */
            SHR_IF_ERR_EXIT(diag_dnx_field_group_info_qual_print(unit, qual_action_iter, fg_info_p,
                                                                 qual_is_packed[qual_action_iter],
                                                                 qual_is_packed_first[qual_action_iter], prt_ctr));
        }
        else
        {
            if (fg_info_p->group_basic_info.fg_type == DNX_FIELD_GROUP_TYPE_KBP)
            {
                /**
                 * Skip 5 cells (Qualifier, Qual Size, Qual Size on Key, Qual Ranged, Qual Packed)
                 * about qualifier info if no more valid qualifiers were found,
                 * but we still have valid actions to be printed.
                 */
                PRT_CELL_SKIP(5);
            }
            else
            {
                /**
                 * Skip 3 cells (Qualifier, Size, Lsb) about qualifier info if no more valid qualifiers were found,
                 * but we still have valid actions to be printed.
                 */
                PRT_CELL_SKIP(3);
            }
        }

        /** Set an empty column to separate qualifiers and actions. */
        PRT_CELL_SET(" ");

        /**
         * If we still have any valid actions (action_left), then print information about them.
         * Otherwise skip the cells, which are relevant to the actions,
         * as it is mentioned in the 'else' statement.
         */
        if (action_left)
        {
            /** Print the actions info. */
            SHR_IF_ERR_EXIT(diag_dnx_field_group_info_action_print
                            (unit, fg_iter, qual_action_iter, fg_info_p, prt_ctr));
        }
        else
        {
            /**
             * Skip 3 cells (Action, Size, Lsb) about action info if no more valid actions were found,
             * but we still have valid qualifiers to be printed.
             */
            PRT_CELL_SKIP(4);
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnx_field_group_const_info_payload_print(
    int unit,
    dnx_field_group_t fg_iter,
    dnx_field_group_full_info_t * fg_info_p,
    sh_sand_control_t * sand_control)
{
    int action_iter;
    int max_num_actions;
    int action_left;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Used as flags to indicate if we have any actions to print. */
    action_left = TRUE;
    /** The max number of actions per field group (32). */
    max_num_actions = dnx_data_field.group.nof_action_per_fg_get(unit);

    PRT_TITLE_SET("Payload Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Base Action");

    /** Iterate over maximum number of actions per FG, to retrieve information about these, which are valid. */
    for (action_iter = 0; action_iter < max_num_actions; action_iter++)
    {
        /**
         * Check if the action_iter is still in the actions range (DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG) and if the
         * current action is invalid (DNX_FIELD_ACTION_INVALID), we should set the action_left to FALSE,
         * which will indicate that we should stop printing any info related to actions.
         */
        if (action_iter < dnx_data_field.group.nof_action_per_fg_get(unit)
            && fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[action_iter].dnx_action ==
            DNX_FIELD_ACTION_INVALID)
        {
            action_left = FALSE;
        }

        /**
         * If we still have any valid actions (action_left), then print information about them.
         */
        if (action_left)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            /** Print the actions info. */
            SHR_IF_ERR_EXIT(diag_dnx_field_group_info_action_print(unit, fg_iter, action_iter, fg_info_p, prt_ctr));
        }
        else    /* (!action_left) */
        {
            /**
             * In case we reached the end of actions arrays,
             * which means that action_left flags is being set to FALSE.
             * We have to break and to stop printing information.
             */
            break;
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares and sets key and payload information about a specific DE FG.
 *   Like Qualifiers/Actions name, size and lsb will be presented.
 * \param [in] unit - The unit number.
 * \param [in] fg_iter - FG ID for which key and payload information will be presented.
 * \param [in] fg_info_p - Pointer, which contains information about the
 *  key and payload of the given Field Group.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_info_de_key_and_payload_print(
    int unit,
    dnx_field_group_t fg_iter,
    dnx_field_group_full_info_t * fg_info_p,
    sh_sand_control_t * sand_control)
{
    int qual_iter, action_iter;
    int max_num_qual_actions;
    int qual_left, action_left;
    uint8 print_new_row;
    unsigned int action_size;
    uint8 qual_size;
    int qual_total_size, action_total_size;
    int qual_size_residue, action_size_residue;
    int qual_current_size, action_current_size;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Used as flags to indicate if we have any quals/actions to print. */
    qual_left = action_left = TRUE;
    /** The max number of qualifiers and actions per field group (32). */
    max_num_qual_actions =
        MAX(dnx_data_field.group.nof_quals_per_fg_get(unit), dnx_data_field.group.nof_action_per_fg_get(unit));

    /** Action and qualifier sizes. */
    action_size = qual_size = 0;
    /** Sum of action and qualifier sizes. */
    action_total_size = qual_total_size = 0;
    /** Will store the residue of the action sizes in case there is any. */
    action_size_residue = qual_size_residue = 0;
    action_current_size = qual_current_size = 0;

    PRT_TITLE_SET("Key & Payload Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qualifier");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Lsb");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, " ");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Base Action");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Lsb");

    print_new_row = TRUE;

    /** Iterate over maximum number of qualifiers and actions per FG, to retrieve information about these, which are valid. */
    for (qual_iter = 0, action_iter = 0; (qual_iter < max_num_qual_actions) && (action_iter < max_num_qual_actions);)
    {
        /**
         * Check if the qual_action_iter is still in the qualifiers range (DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) and if the
         * current qualifier is invalid (DNX_FIELD_QUAL_TYPE_INVALID), we should set the qual_left to FALSE,
         * which will indicate that we should stop printing any info related to qualifiers.
         */
        if (qual_iter < dnx_data_field.group.nof_quals_per_fg_get(unit)
            && fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].qual_type == DNX_FIELD_QUAL_TYPE_INVALID)
        {
            qual_left = FALSE;
        }
        /**
         * Check if the qual_action_iter is still in the actions range (DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG) and if the
         * current action is invalid (DNX_FIELD_ACTION_INVALID), we should set the action_left to FALSE,
         * which will indicate that we should stop printing any info related to actions.
         */
        if (action_iter < dnx_data_field.group.nof_action_per_fg_get(unit)
            && fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[action_iter].dnx_action ==
            DNX_FIELD_ACTION_INVALID)
        {
            action_left = FALSE;
        }
        /**
         * In case we reached the end of both qualifiers and actions arrays,
         * which means that qual_left and action_left flags are being set to FALSE.
         * We have to break and to stop printing information for both.
         */
        if (!qual_left && !action_left)
        {
            break;
        }

        /** Get the size of the current qualifier. */
        if (qual_left)
        {
            qual_size = fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size;
        }

        /** Get the size of the current action. */
        if (action_left)
        {
            /** Get action size for the current action. */
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, fg_info_p->group_basic_info.field_stage,
                                                          fg_info_p->group_full_info.
                                                          actions_payload_info.actions_on_payload_info[action_iter].
                                                          dnx_action, &action_size));

            /** In case we have valid bit, we should add 1 bit to the actions size. */
            if (!
                (fg_info_p->group_full_info.actions_payload_info.
                 actions_on_payload_info[action_iter].dont_use_valid_bit))
            {
                action_size += 1;
            }
        }

        /**
         * In case the qual_size is lower than the actions_size, it means that we have more then
         * one qualifier in the key, from where the action will take its value.
         */
        if (qual_size < action_size && qual_left)
        {
            /** Calculate the total qualifiers size. */
            qual_total_size += qual_size;

            /**
             * Add new row with underscore separator in case we have to print
             * qualifier and action one same row. Otherwise add rows without separators.
             */
            if (print_new_row)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            }
            else
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            }

            /**
             * In case total size of qualifiers is lower than size of the current action, it means
             * that qualifiers are still related to the current action.
             */
            if (qual_total_size < action_size)
            {
                /** Print the qualifiers info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_group_info_de_qual_print
                                (unit, qual_iter, qual_size, 0, 0, fg_info_p, prt_ctr));

                /** Set an empty column to separate qualifiers and actions. */
                PRT_CELL_SET(" ");

                if (print_new_row && action_left)
                {
                    /** Print the actions info. */
                    SHR_IF_ERR_EXIT(diag_dnx_field_group_info_de_action_print
                                    (unit, fg_iter, action_iter, action_size, 0, 0, fg_info_p, prt_ctr));

                    print_new_row = FALSE;
                }
                else
                {
                    PRT_CELL_SKIP(3);
                }

                /** Increase the qualifier iterator. */
                qual_iter++;
            }
            /**
             * If the total size of the qualifiers get bigger than size of the current action, it means
             * that we need to divide the qualifier to two parts. Where the first part will be the MSB of
             * the current action and the second part will be the LSB of the next action.
             */
            else if (qual_total_size > action_size)
            {
                /** Calculate the bits, which will be shown as MSB for the first action. */
                qual_current_size = action_size - (qual_total_size - qual_size);

                /** Print the qualifiers info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_group_info_de_qual_print
                                (unit, qual_iter, qual_current_size, 0, (qual_current_size - 1), fg_info_p, prt_ctr));

                /** Set an empty column to separate qualifiers and actions. */
                PRT_CELL_SET(" ");

                /** Skip the action related cells. */
                PRT_CELL_SKIP(3);

                /** Add row with separator on which the next qualifier and action will be presented. */
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

                /** Calculate the residue of the qualifier, which will be shown as LSB of the next action. */
                qual_size_residue = qual_total_size - action_size;

                /** Print the qualifiers info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_group_info_de_qual_print
                                (unit, qual_iter, qual_size_residue, qual_current_size, (qual_size - 1), fg_info_p,
                                 prt_ctr));

                /** Set an empty column to separate qualifiers and actions. */
                PRT_CELL_SET(" ");

                /** Increase the action iterator to get to the next action. */
                action_iter++;

                /** Get action size for the next action. */
                SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, fg_info_p->group_basic_info.field_stage,
                                                              fg_info_p->group_full_info.
                                                              actions_payload_info.actions_on_payload_info[action_iter].
                                                              dnx_action, &action_size));

                /** In case we have valid bit, we should add 1 bit to the actions size. */
                if (!
                    (fg_info_p->group_full_info.actions_payload_info.
                     actions_on_payload_info[action_iter].dont_use_valid_bit))
                {
                    action_size += 1;
                }

                /** Print the actions info for the next action. */
                SHR_IF_ERR_EXIT(diag_dnx_field_group_info_de_action_print
                                (unit, fg_iter, action_iter, action_size, 0, 0, fg_info_p, prt_ctr));

                /** Increase the qualifier iterator. */
                qual_iter++;

                /** Set the total size of the qualifiers to be equal to the residue of the last qualifier. */
                qual_total_size = qual_size_residue;
            }
            /**
             * If total size of the qualifiers becomes equal to current action size, just
             * print the current qualifier and skip the cells related to the action.
             */
            else
            {
                /** Print the qualifiers info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_group_info_de_qual_print
                                (unit, qual_iter, qual_size, 0, 0, fg_info_p, prt_ctr));

                /** Set an empty column to separate qualifiers and actions. */
                PRT_CELL_SET(" ");

                /** Skip the cells related to the action. */
                PRT_CELL_SKIP(3);

                /** Increase the action and qualifier iterators. */
                action_iter++;
                qual_iter++;
                /** Clear the total size of the qualifiers. */
                qual_total_size = 0;
                print_new_row = TRUE;
            }
        }
        /**
         * In case the qual_size is bigger than the actions_size, it means that we have more then
         * one action, which will take its values from the current qualifier.
         */
        else if (qual_size > action_size && action_left)
        {
            /** Calculate the total size of the actions. */
            action_total_size += action_size;

            /**
             * Add new row with underscore separator in case we have to print
             * qualifier and action one same row. Otherwise add rows without separators.
             */
            if (print_new_row)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            }
            else
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            }

            if (print_new_row && qual_left)
            {
                /** Print the qualifiers info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_group_info_qual_print(unit, qual_iter, fg_info_p, 0, 0, prt_ctr));

                print_new_row = FALSE;
            }
            else
            {
                PRT_CELL_SKIP(3);
            }

            /** Set an empty column to separate qualifiers and actions. */
            PRT_CELL_SET(" ");

            /**
             * In case total size of actions is lower than size of the current qualifier, it means
             * that actions are still getting its values from the current qualifier.
             */
            if (action_total_size < qual_size)
            {
                /** Print the actions info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_group_info_de_action_print
                                (unit, fg_iter, action_iter, action_size, 0, 0, fg_info_p, prt_ctr));

                /** Increase the action iterator. */
                action_iter++;
            }
            /**
             * If the total size of the actions get bigger than size of the current qualifier, it means
             * that we need to divide the action to two parts. Where the first part will be the MSB of
             * the current qualifier and the second part will be the LSB of the next qualifier.
             */
            else if (action_total_size > qual_size)
            {
                /** Calculate the action bits, which will be shown as MSB for the first qualifier. */
                action_current_size = qual_size - (action_total_size - action_size);

                /** Print the actions info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_group_info_de_action_print
                                (unit, fg_iter, action_iter, action_current_size, 0, (action_current_size - 1),
                                 fg_info_p, prt_ctr));

                /** Increase the qualifier iteration to get to the next qualifier. */
                qual_iter++;

                /** Add row with separator on which the next qualifier and action will be presented. */
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

                /** Print the next qualifier info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_group_info_qual_print(unit, qual_iter, fg_info_p, 0, 0, prt_ctr));

                /** Set an empty column to separate qualifiers and actions. */
                PRT_CELL_SET(" ");

                /** Calculate the residue of the action, which will be shown as LSB of the next qualifier. */
                action_size_residue = action_total_size - qual_size;

                /** Print the actions info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_group_info_de_action_print
                                (unit, fg_iter, action_iter, action_size_residue, action_current_size,
                                 (action_size - 1), fg_info_p, prt_ctr));

                /** Increase the action iterator. */
                action_iter++;
                /** Set the total size of the actions to be equal to the residue of the last action. */
                action_total_size = action_size_residue;
            }
            /**
             * If total size of the actions becomes equal to current qualifier size, just
             * print the current action.
             */
            else
            {
                /** Print the actions info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_group_info_de_action_print
                                (unit, fg_iter, action_iter, action_size, 0, 0, fg_info_p, prt_ctr));

                /** Increase the action and qualifier iterators. */
                action_iter++;
                qual_iter++;
                /** Clear the total size of the actions. */
                action_total_size = 0;
                print_new_row = TRUE;
            }
        }
        else
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

            /**
             * If we still have any valid qualifiers (qual_left), then print information about them.
             * Otherwise skip the cells, which are relevant to the qualifiers,
             * as it is mentioned in the 'else' statement.
             */
            if (qual_left)
            {
                /** Print the qualifiers info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_group_info_qual_print(unit, qual_iter, fg_info_p, 0, 0, prt_ctr));

                qual_iter++;
            }
            else
            {
                /**
                 * Skip 3 cells (Qualifier, Size, Lsb) about qualifier info if no more valid qualifiers were found,
                 * but we still have valid actions to be printed.
                 */
                PRT_CELL_SKIP(3);
            }

            /** Set an empty column to separate qualifiers and actions. */
            PRT_CELL_SET(" ");

            /**
             * If we still have any valid actions (action_left), then print information about them.
             * Otherwise skip the cells, which are relevant to the actions,
             * as it is mentioned in the 'else' statement.
             */
            if (action_left)
            {
                /** Print the actions info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_group_info_de_action_print
                                (unit, fg_iter, action_iter, action_size, 0, 0, fg_info_p, prt_ctr));

                action_iter++;
            }
            else
            {
                /**
                 * Skip 3 cells (Action, Size, Lsb) about action info if no more valid actions were found,
                 * but we still have valid qualifiers to be printed.
                 */
                PRT_CELL_SKIP(3);
            }

            print_new_row = TRUE;
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares and sets information regarding entries of a specific TCAM FG.
 *   Presents number of entries per TCAM bank.
 * \param [in] unit - The unit number.
 * \param [in] bank_entries_count - Array per core, which contains number of allocated entries per TCAM bank,
 *  returned by field_group_info_print().
 * \param [in] banks_bmp_p - Contains information about used TCAM banks by a specific FG,
 *  returned by field_group_info_print().
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_tcam_info_entry_print(
    int unit,
    uint32 bank_entries_count[2][DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS],
    uint32 *banks_bmp_p,
    sh_sand_control_t * sand_control)
{
    int bank_id_iter;
    int total_num_of_entries[2];
    int core;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(total_num_of_entries, 0, sizeof(total_num_of_entries));

    PRT_TITLE_SET("FG Entry Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Banks");

    /** Iterate over all TCAM banks to check which of them is being used by the current FG. */
    for (bank_id_iter = 0; bank_id_iter < DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS; bank_id_iter++)
    {
        /** In case we have a match in the banks bitmap, a column with the used bank ID will be added. */
        if (SHR_BITGET(banks_bmp_p, bank_id_iter))
        {
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ID %d", bank_id_iter);
        }
    }
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Total");
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Nof entries");
    /** Iterate over all TCAM banks to check which of them is being used by the current FG. */
    for (bank_id_iter = 0; bank_id_iter < DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS; bank_id_iter++)
    {
        /**
         * In case we have a match in the banks bitmap, information about how much entries
         * in this bank have been allocated will be present (taken from bank_entries_count array).
         */
        if (SHR_BITGET(banks_bmp_p, bank_id_iter))
        {
            int nof_entries_per_core[2];
            for (core = 0; core < DIAG_DNX_FIELD_GROUP_NOF_CORES; core++)
            {
                nof_entries_per_core[core] = bank_entries_count[core][bank_id_iter];
                /**
                 * Calculate the total number of entries on all used banks.
                 * Will be exposed in the end of the Entry Info table.
                 */
                total_num_of_entries[core] += bank_entries_count[core][bank_id_iter];
            }
            if (DIAG_DNX_FIELD_GROUP_NOF_CORES == 1)
            {
                PRT_CELL_SET("core_0: %d\ncore_1: N/A", nof_entries_per_core[0]);
            }
            else
            {
                PRT_CELL_SET("core_0: %d\ncore_1: %d", nof_entries_per_core[0], nof_entries_per_core[1]);
            }
        }
    }

    if (DIAG_DNX_FIELD_GROUP_NOF_CORES == 1)
    {
        PRT_CELL_SET("core_0: %d\ncore_1: N/A", total_num_of_entries[0]);
    }
    else
    {
        PRT_CELL_SET("core_0: %d\ncore_1: %d", total_num_of_entries[0], total_num_of_entries[1]);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares and sets information regarding entries of a specific External FG.
 *   Presents number of entries per DB_ID.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - FG id, which entries will be shown.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_external_info_entry_print(
    int unit,
    dnx_field_group_t fg_id,
    sh_sand_control_t * sand_control)
{
    int total_num_of_entries;
    dbal_tables_e dbal_table_id;
    uint32 db_id;
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    total_num_of_entries = 0;

    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));

    /** External (KBP) field groups can only have entries after the KBP device lock. */
    SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
    if (DNX_KBP_STATUS_IS_LOCKED(kbp_mngr_status))
    {
        SHR_IF_ERR_EXIT(dbal_db.tables_properties.nof_entries.get(unit, dbal_table_id, &total_num_of_entries));
    }

    PRT_TITLE_SET("FG Entry Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "DB_ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Nof entries");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, dbal_table_id, &db_id, NULL));
    PRT_CELL_SET("%d", db_id);
    PRT_CELL_SET("%d", total_num_of_entries);

    PRT_COMMITX;

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares and sets information regarding entries of a specific EXEM FG.
 *   Presents number of entries per FG.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - FG id, which entries will be shown.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_exem_info_entry_print(
    int unit,
    dnx_field_group_t fg_id,
    sh_sand_control_t * sand_control)
{
    int total_num_of_entries;
    dbal_tables_e dbal_table_id;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(dbal_db.tables_properties.nof_entries.get(unit, dbal_table_id, &total_num_of_entries));

    PRT_TITLE_SET("FG Entry Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Nof entries");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    PRT_CELL_SET("%d", total_num_of_entries);

    PRT_COMMITX;

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares and sets information regarding Access profile, Banks and
 *   prefix of a specific TCAM or DirectTable FG.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - FG ID to retrieve its dbal table.
 * \param [in] fg_info_p - Pointer, which contains information for the
 *  given FG, in this function we will retrieve the Access Profile from it.
 * \param [out] bank_entries_count - Array per core, which which will store
 *  number of allocated entries per TCAM bank.
 * \param [out] banks_bmp_p - Will store information about used
 *  TCAM banks by a specific TCAM FG,
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_tcam_banks_info_print(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p,
    uint32 bank_entries_count[2][DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS],
    uint32 *banks_bmp_p,
    sh_sand_control_t * sand_control)
{
    int bank_id_iter;
    /** String to store used TCAM bank IDs. */
    char bank_ids_string[2][DIAG_DNX_FIELD_UTILS_STR_SIZE];
    uint32 tcam_handler_id;
    dbal_tables_e dbal_table_id;
    int core;
    dnx_field_tcam_access_fg_params_t fg_params;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FG %d Bank Info", fg_id);
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Access Profile");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Bank IDs");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Prefix");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Prefix Size");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    /** Set the Access Profile value. */
    PRT_CELL_SET("%d", fg_info_p->app_db_id);

    /** Retrieve return the DBAL table that was created for given TCAM Field Group. */
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
    /** Get the relevant TCAM handler of the returned dbal_table_id. */
    SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_get(unit, dbal_table_id, &tcam_handler_id));
    /** Retrieve information about all entries, which are being add to the given FG, and used TCAM banks. */
    for (core = 0; core < DIAG_DNX_FIELD_GROUP_NOF_CORES; core++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entries_info
                        (unit, core, tcam_handler_id, bank_entries_count[core], banks_bmp_p));

        sal_strncpy(bank_ids_string[core], "", sizeof(bank_ids_string[core]));
        /** Iterate over all TCAM banks and print those, which are used by the current FG. */
        for (bank_id_iter = 0; bank_id_iter < DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS; bank_id_iter++)
        {
            if (SHR_BITGET(banks_bmp_p, bank_id_iter))
            {
                /** String to store the current bank_id_iter. */
                char banks_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
                if (!sal_strncmp(bank_ids_string[0], "", sizeof(bank_ids_string[0])))
                {
                    DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%d", bank_id_iter);
                    sal_snprintf(banks_buff, sizeof(banks_buff), "%d", bank_id_iter);
                }
                else
                {
                    DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, ", %d", bank_id_iter);
                    sal_snprintf(banks_buff, sizeof(banks_buff), ", %d", bank_id_iter);
                }
                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(bank_ids_string[core], "%s", banks_buff);
                sal_strncat_s(bank_ids_string[core], banks_buff, sizeof(bank_ids_string[core]));
            }
        }
    }

    if (DIAG_DNX_FIELD_GROUP_NOF_CORES == 1)
    {
        if (!sal_strncmp(bank_ids_string[0], "", sizeof(bank_ids_string[0])))
        {
            sal_strncpy(bank_ids_string[0], "N/A", sizeof(bank_ids_string[0]));
        }
        sal_strncpy(bank_ids_string[1], "N/A", sizeof(bank_ids_string[1]));
    }
    else
    {
        if (!sal_strncmp(bank_ids_string[0], "", sizeof(bank_ids_string[0])))
        {
            sal_strncpy(bank_ids_string[0], "N/A", sizeof(bank_ids_string[0]));
        }
        if (!sal_strncmp(bank_ids_string[1], "", sizeof(bank_ids_string[1])))
        {
            sal_strncpy(bank_ids_string[1], "N/A", sizeof(bank_ids_string[1]));
        }
    }
    PRT_CELL_SET("core_0: %s\ncore_1: %s", bank_ids_string[0], bank_ids_string[1]);

    /** Get TCAM handler information. */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, tcam_handler_id, &fg_params));
    if (fg_params.prefix_value == DNX_FIELD_TCAM_PREFIX_VAL_AUTO)
    {
        PRT_CELL_SET("%s", "Not Allocated");
    }
    else
    {
        /** Buffer to store the five bits of the prefix_value. Which will be presented in binary format. */
        char prefix_value_binary_buffer[6];
        sal_itoa(prefix_value_binary_buffer, fg_params.prefix_value, 2, 0, fg_params.prefix_size);
        PRT_CELL_SET("0b%s", prefix_value_binary_buffer);
    }
    PRT_CELL_SET("%d", fg_params.prefix_size);

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays basic information for given Field Group,
 *   like ID, Name, Type, Stage, Key size and Payload size.
 *   Used for DirectExtraction and StateTable field groups.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Field Group ID for which information will be presented.
 * \param [in] fg_info_p - Pointer, which contains information about the given field
 *  group. Like context, key and payload info.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_basic_info_print(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p,
    sh_sand_control_t * sand_control)
{
    uint32 payload_size_in_bits;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FIELD GROUP Full Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Stage");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Payload Size");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    /** Prepares and sets common basic information about a specific FG, like id, name, stage and type. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_info_set(unit, fg_id, fg_info_p, prt_ctr));

    /** Calculates the "used" payload_size, by the user. */
    SHR_IF_ERR_EXIT(dnx_field_group_calc_actions_bit_size(unit, fg_info_p->group_basic_info.field_stage,
                                                          fg_info_p->group_full_info.
                                                          actions_payload_info.actions_on_payload_info,
                                                          &payload_size_in_bits));

    /** Set Key and Payload sizes. */
    PRT_CELL_SET("%d", fg_info_p->group_full_info.key_template.key_size_in_bits);
    PRT_CELL_SET("%d", payload_size_in_bits);

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays basic information for given TCAM or DT TCAM field group,
 *   like ID, Name, Type, Stage, Key size and Payload size.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Field Group ID for which information will be presented.
 * \param [in] fg_info_p - Pointer, which contains information about the given field
 *  group. Like name, stage, key and payload info.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_tcam_basic_info_print(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p,
    sh_sand_control_t * sand_control)
{
    uint32 payload_size_in_bits;
    uint32 key_size_in_bits;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FIELD GROUP Full Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Stage");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Payload Size");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    /** Prepares and sets common basic information about a specific FG, like id, name, stage and type. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_info_set(unit, fg_id, fg_info_p, prt_ctr));

    /**
     * In case of TCAM and DT TCAM FGs, the key_length_type (80, 160, 320 bits) and
     * action_length_type (32, 64, 128 bits) will be presented.
     */
    SHR_IF_ERR_EXIT(diag_dnx_field_utils_key_size_conversion
                    (unit, fg_info_p->group_full_info.key_length_type, &key_size_in_bits));
    SHR_IF_ERR_EXIT(diag_dnx_field_utils_action_size_conversion
                    (unit, fg_info_p->group_full_info.action_length_type, &payload_size_in_bits));
    PRT_CELL_SET("%d", key_size_in_bits);
    PRT_CELL_SET("%d", payload_size_in_bits);

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays basic information for given External field group,
 *   like ID, Name, Type, Stage, Key size and Payload size.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Field Group ID for which information will be presented.
 * \param [in] fg_info_p - Pointer, which contains information about the given field
 *  group. Like name, stage, key and payload info.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_external_basic_info_print(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p,
    sh_sand_control_t * sand_control)
{
    uint32 payload_size_in_bits;
    uint32 external_payload_size;
    uint32 byte_align_key_size;
    uint32 external_key_size;
    uint32 max_num_quals;
    uint32 qual_iter;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    external_key_size = 0;
    max_num_quals = dnx_data_field.group.nof_quals_per_fg_get(unit);;

    PRT_TITLE_SET("FIELD GROUP Full Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Stage");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Payload Size");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    /** Prepares and sets common basic information about a specific FG, like id, name, stage and type. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_info_set(unit, fg_id, fg_info_p, prt_ctr));

    /** In case of External FGs, the key_size and action_size should be presented byte aligned. */
    for (qual_iter = 0; qual_iter < max_num_quals &&
         fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].qual_type !=
         DNX_FIELD_QUAL_TYPE_INVALID; qual_iter++)
    {
        byte_align_key_size = fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size;
        if ((byte_align_key_size % 8) != 0)
        {
            byte_align_key_size += 8 - (byte_align_key_size % 8);
        }
        external_key_size += byte_align_key_size;
    }

    /** Calculates the "used" payload_size, by the user. */
    SHR_IF_ERR_EXIT(dnx_field_group_calc_actions_bit_size(unit, fg_info_p->group_basic_info.field_stage,
                                                          fg_info_p->group_full_info.
                                                          actions_payload_info.actions_on_payload_info,
                                                          &payload_size_in_bits));
    SHR_IF_ERR_EXIT(dnx_kbp_valid_result_width(unit, payload_size_in_bits, &external_payload_size));

    PRT_CELL_SET("%d", external_key_size);
    PRT_CELL_SET("%d", external_payload_size);

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays Field Group full info for given TCAM field group,
 *   specified by the caller.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Field Group ID for which information will be presented.
 * \param [in] fg_info_p - Pointer, which contains information about the given field
 *  group. Like context, key and payload info. Displays info about entries and bank usage.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_tcam_info_cb(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p,
    sh_sand_control_t * sand_control)
{
    uint32 bank_entries_count[2][DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS];
    uint32 banks_bmp[2];

    SHR_FUNC_INIT_VARS(unit);

    /** Displays basic information for given TCAM field group, like ID, Name, Type, Stage, Key size and Payload size. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_tcam_basic_info_print(unit, fg_id, fg_info_p, sand_control));

    /** Prepare and print a table, which contains all context info about the current field group. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_info_context_print
                    (unit, fg_id, fg_info_p->group_basic_info.field_stage, sand_control));

    /** Prepare and print a table, which contains all key and payload info about the current field group. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_info_key_and_payload_print(unit, fg_id, fg_info_p, sand_control));

    /** Prepares and sets information regarding Access profile, Banks and prefix of a specific TCAM FG. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_tcam_banks_info_print
                    (unit, fg_id, fg_info_p, bank_entries_count, banks_bmp, sand_control));

    /** Prepares and sets information regarding entries of a specific TCAM FG. Presents number of entries per TCAM bank. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_tcam_info_entry_print(unit, bank_entries_count, banks_bmp, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays Field Group full info for given Direct Table field group,
 *   specified by the caller.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Field Group ID for which information will be presented.
 * \param [in] fg_info_p - Pointer, which contains information about the given field
 *  group. Like context, key and payload info.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_dt_tcam_info_cb(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p,
    sh_sand_control_t * sand_control)
{
    uint32 bank_entries_count[2][DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS];
    uint32 banks_bmp[2];

    SHR_FUNC_INIT_VARS(unit);

    /** Displays basic information for given DT TCAM field group, like ID, Name, Type, Stage, Key size and Payload size. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_tcam_basic_info_print(unit, fg_id, fg_info_p, sand_control));

    /** Prepare and print a table, which contains all context info about the current field group. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_info_context_print
                    (unit, fg_id, fg_info_p->group_basic_info.field_stage, sand_control));

    /** Prepare and print a table, which contains all key and payload info about the current field group. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_info_key_and_payload_print(unit, fg_id, fg_info_p, sand_control));

    /** Prepares and sets information regarding Access profile and Banks of a specific TCAM DirectTable FG. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_tcam_banks_info_print
                    (unit, fg_id, fg_info_p, bank_entries_count, banks_bmp, sand_control));

    /** Prepares and sets information regarding entries of a specific TCAM DirectTable FG. Presents number of entries per TCAM bank. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_tcam_info_entry_print(unit, bank_entries_count, banks_bmp, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays Field Group full info for given Direct Extraction field group,
 *   specified by the caller.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Field Group ID for which information will be presented.
 * \param [in] fg_info_p - Pointer, which contains information about the given field
 *  group. Like context, key and payload info.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_de_info_cb(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Call internal function to present information for the given DE Field Group. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_basic_info_print(unit, fg_id, fg_info_p, sand_control));

    /** Prepare and print a table, which contains all context info about the current field group. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_info_context_print
                    (unit, fg_id, fg_info_p->group_basic_info.field_stage, sand_control));

    /**
     * Prepare and print a table, which contains all key and payload info about the current field group.
     * Like Qualifiers/Actions name, size and lsb will be presented.
     */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_info_de_key_and_payload_print(unit, fg_id, fg_info_p, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays Field Group full info for given Const field group,
 *   specified by the caller.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Field Group ID for which information will be presented.
 * \param [in] fg_info_p - Pointer, which contains information about the given field
 *  group. Like context, key and payload info.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_const_info_cb(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Call internal function to present information for the given DE Field Group. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_basic_info_print(unit, fg_id, fg_info_p, sand_control));

    /** Prepare and print a table, which contains all context info about the current field group. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_info_context_print
                    (unit, fg_id, fg_info_p->group_basic_info.field_stage, sand_control));

    /**
     * Prepare and print a table, which contains all key and payload info about the current field group.
     * Like Qualifiers/Actions name, size and lsb will be presented.
     */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_const_info_payload_print(unit, fg_id, fg_info_p, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays Field Group full info for given State Table field group,
 *   specified by the caller.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Field Group ID for which information will be presented.
 * \param [in] fg_info_p - Pointer, which contains information about the given field
 *  group. Like context, key and payload info.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_st_info_cb(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Call internal function to present information for the given ST Field Group. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_basic_info_print(unit, fg_id, fg_info_p, sand_control));

    /** Prepare and print a table, which contains all context info about the current field group. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_info_context_print
                    (unit, fg_id, fg_info_p->group_basic_info.field_stage, sand_control));

    /**
     * Prepare and print a table, which contains all key and payload info about the current field group.
     * Like Qualifiers/Actions name, size and lsb will be presented.
     */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_info_key_and_payload_print(unit, fg_id, fg_info_p, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays Field Group full info for given External field group,
 *   specified by the caller.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Field Group ID for which information will be presented.
 * \param [in] fg_info_p - Pointer, which contains information about the given field
 *  group. Like context, key and payload info. Displays info about entries.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_external_info_cb(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Displays basic information for given External field group, like ID, Name, Type, Stage, Key size and Payload size. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_external_basic_info_print(unit, fg_id, fg_info_p, sand_control));

    /** Prepare and print a table, which contains all context info about the current field group. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_info_external_context_print
                    (unit, fg_id, fg_info_p->group_basic_info.field_stage, sand_control));

    /** Prepare and print a table, which contains all key and payload info about the current field group. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_info_key_and_payload_print(unit, fg_id, fg_info_p, sand_control));

    /** Prepares and sets information regarding entries of a specific External FG. Presents number of entries per DB_ID. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_external_info_entry_print(unit, fg_id, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays Field Group full info for given EXEM field group,
 *   specified by the caller.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Field Group ID for which information will be presented.
 * \param [in] fg_info_p - Pointer, which contains information about the given field
 *  group. Like context, key and payload info. Displays info about entries.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_exem_info_cb(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Call internal function to present information for the given ST Field Group. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_basic_info_print(unit, fg_id, fg_info_p, sand_control));

    /** Prepare and print a table, which contains all context info about the current field group. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_info_context_print
                    (unit, fg_id, fg_info_p->group_basic_info.field_stage, sand_control));

    /**
     * Prepare and print a table, which contains all key and payload info about the current field group.
     * Like Qualifiers/Actions name, size and lsb will be presented.
     */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_info_key_and_payload_print(unit, fg_id, fg_info_p, sand_control));

    /** Prepares and sets information regarding entries of a specific EXEM FG. Presents number of entries per FG. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_exem_info_entry_print(unit, fg_id, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays Field Group full info for given MDB DT field group,
 *   specified by the caller.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Field Group ID for which information will be presented.
 * \param [in] fg_info_p - Pointer, which contains information about the given field
 *  group. Like context, key and payload info. Displays info about entries.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_mdb_dt_info_cb(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Call internal function to present information for the given ST Field Group. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_basic_info_print(unit, fg_id, fg_info_p, sand_control));

    /** Prepare and print a table, which contains all context info about the current field group. */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_info_context_print
                    (unit, fg_id, fg_info_p->group_basic_info.field_stage, sand_control));

    /**
     * Prepare and print a table, which contains all key and payload info about the current field group.
     * Like Qualifiers/Actions name, size and lsb will be presented.
     */
    SHR_IF_ERR_EXIT(diag_dnx_field_group_info_key_and_payload_print(unit, fg_id, fg_info_p, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays Field Groups per group ID or range of IDs and stage (optional), specified by the caller.
 * \param [in] unit - The unit number.
 * \param [in] args -
 *   Null terminated string. Contains 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_list_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 is_allocated;
    int fg_id_index;
    dnx_field_group_t fg_id_lower, fg_id_upper;
    dnx_field_stage_e stage_lower, stage_upper;
    dnx_field_stage_e stage_index;
    int do_display;
    dnx_field_group_full_info_t *fg_info = NULL;
    unsigned int payload_size_in_bits;
    uint32 key_size_in_bits;

    uint32 external_payload_size;
    uint32 byte_align_key_size;
    uint32 external_key_size;
    uint32 max_num_quals;
    uint32 qual_iter;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_GROUP, fg_id_lower, fg_id_upper);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_STAGE, stage_lower, stage_upper);

    SHR_ALLOC(fg_info, sizeof(dnx_field_group_full_info_t), "fg_info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    PRT_TITLE_SET("FIELD_GROUP");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FG ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Stage");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Payload Size");

    max_num_quals = dnx_data_field.group.nof_quals_per_fg_get(unit);
    payload_size_in_bits = 0;
    do_display = FALSE;

    /** Iterate over all stages and check if they are FP stages. */
    for (stage_index = stage_lower; stage_index <= stage_upper; stage_index++)
    {
        dnx_field_stage_e field_stage;
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                        (unit, Field_group_diag_stage_enum_table[stage_index].string, &field_stage));

        /** Iterate over all field groups and check, which are allocated. */
        for (fg_id_index = fg_id_lower; fg_id_index <= fg_id_upper; fg_id_index++)
        {
            /**
             * Check if the specified FG ID or rage of FG IDs are allocated.
             */
            SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id_index, &is_allocated));
            if (!is_allocated)
            {
                continue;
            }
            /** Get information about the current Field Group. */
            SHR_IF_ERR_EXIT(dnx_field_group_get(unit, fg_id_index, fg_info));

            /**
             * Check if the current stage iterator value is same as the allocated FG stage.
             * In case no, continue to the next allocated FG.
             */
            if (field_stage != fg_info->group_basic_info.field_stage)
            {
                continue;
            }

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

            /** Prepares and sets common basic information about a specific FG, like id, name, stage and type. */
            SHR_IF_ERR_EXIT(diag_dnx_field_group_info_set(unit, fg_id_index, fg_info, prt_ctr));

            switch (fg_info->group_basic_info.fg_type)
            {
                case DNX_FIELD_GROUP_TYPE_TCAM:
                case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
                {
                    /**
                     * In case of TCAM or DT TCAM FG, the key_length_type (80, 160, 320 bits) and
                     * action_length_type (32, 64, 128 bits) will be presented.
                     */
                    SHR_IF_ERR_EXIT(diag_dnx_field_utils_key_size_conversion
                                    (unit, fg_info->group_full_info.key_length_type, &key_size_in_bits));
                    SHR_IF_ERR_EXIT(diag_dnx_field_utils_action_size_conversion
                                    (unit, fg_info->group_full_info.action_length_type, &payload_size_in_bits));
                    PRT_CELL_SET("%d", key_size_in_bits);
                    PRT_CELL_SET("%d", payload_size_in_bits);
                    break;
                }
                case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
                case DNX_FIELD_GROUP_TYPE_CONST:
                case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
                {
                    /** Calculates the "used" payload_size, by the user. */
                    SHR_IF_ERR_EXIT(dnx_field_group_calc_actions_bit_size(unit, fg_info->group_basic_info.field_stage,
                                                                          fg_info->group_full_info.
                                                                          actions_payload_info.actions_on_payload_info,
                                                                          &payload_size_in_bits));
                    /** In case of DE or ST FG, the "used", key_size and payload_size, by the user will be presented. */
                    PRT_CELL_SET("%d", fg_info->group_full_info.key_template.key_size_in_bits);
                    PRT_CELL_SET("%d", payload_size_in_bits);
                    break;
                }
                case DNX_FIELD_GROUP_TYPE_KBP:
                {
                    external_key_size = 0;
                    /** In case of External FGs, the key_size and action_size should be presented byte aligned. */
                    for (qual_iter = 0; qual_iter < max_num_quals &&
                         fg_info->group_full_info.key_template.key_qual_map[qual_iter].qual_type !=
                         DNX_FIELD_QUAL_TYPE_INVALID; qual_iter++)
                    {
                        byte_align_key_size = fg_info->group_full_info.key_template.key_qual_map[qual_iter].size;
                        if ((byte_align_key_size % 8) != 0)
                        {
                            byte_align_key_size += 8 - (byte_align_key_size % 8);
                        }
                        external_key_size += byte_align_key_size;
                    }
                    /** Calculates the "used" payload_size, by the user. */
                    SHR_IF_ERR_EXIT(dnx_field_group_calc_actions_bit_size(unit, fg_info->group_basic_info.field_stage,
                                                                          fg_info->group_full_info.
                                                                          actions_payload_info.actions_on_payload_info,
                                                                          &payload_size_in_bits));
                    SHR_IF_ERR_EXIT(dnx_kbp_valid_result_width(unit, payload_size_in_bits, &external_payload_size));

                    PRT_CELL_SET("%d", external_key_size);
                    PRT_CELL_SET("%d", external_payload_size);
                    break;
                }
                default:
                {
                    PRT_CELL_SET("%s", "N/A");
                    PRT_CELL_SET("%s", "N/A");
                    break;
                }
            }

            do_display = TRUE;
        }
    }
    if (do_display)
    {
        PRT_COMMITX;
    }
    else
    {
        /*
         * If nothing was printed then just clear loaded PRT memory.
         * This could be left to 'exit' but it is clearer here.
         */
        PRT_FREE;
        LOG_CLI_EX("\r\n" "NO allocated field group was found on specified filters!! %s%s%s%s\r\n\n", EMPTY, EMPTY,
                   EMPTY, EMPTY);
    }

exit:
    SHR_FREE(fg_info);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays Field Group full info for given group ID, specified by the caller.
 * \param [in] unit - The unit number.
 * \param [in] args -
 *   Null terminated string. Contains 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_group_info_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 is_fg_allocated;
    dnx_field_group_t fg_id;
    dnx_field_group_full_info_t *fg_info = NULL;
    char converted_fg_type[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_GROUP, fg_id);

    SHR_ALLOC(fg_info, sizeof(dnx_field_group_full_info_t), "fg_info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /**
     * In case the field group is allocated, retrieve all needed information for it.
     * If not return an error.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_fg_allocated));
    if (!is_fg_allocated)
    {
        LOG_CLI_EX("\r\n" "The given FG Id %d is not allocated!!!%s%s%s\r\n\n", fg_id, EMPTY, EMPTY, EMPTY);
    }

    /** Get information about the current Field Group. */
    SHR_IF_ERR_EXIT(dnx_field_group_get(unit, fg_id, fg_info));

    /** Print information only for supported FG Types. */
    switch (fg_info->group_basic_info.fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_group_tcam_info_cb(unit, fg_id, fg_info, sand_control));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_group_dt_tcam_info_cb(unit, fg_id, fg_info, sand_control));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_group_de_info_cb(unit, fg_id, fg_info, sand_control));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_CONST:
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_group_const_info_cb(unit, fg_id, fg_info, sand_control));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_group_st_info_cb(unit, fg_id, fg_info, sand_control));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_KBP:
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_group_external_info_cb(unit, fg_id, fg_info, sand_control));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_group_exem_info_cb(unit, fg_id, fg_info, sand_control));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_group_mdb_dt_info_cb(unit, fg_id, fg_info, sand_control));
            break;
        }
        default:
        {
            /** None of the supported types. */
            SHR_IF_ERR_EXIT(diag_dnx_field_utils_group_type_conversion
                            (unit, fg_info->group_basic_info.fg_type, converted_fg_type));

            LOG_CLI_EX("\r\n" "Field Group Type %s is not supported by FG Info diagnostic!! %s%s%s\r\n\n",
                       converted_fg_type, EMPTY, EMPTY, EMPTY);
            break;
        }
    }

exit:
    SHR_FREE(fg_info);
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/*
 * Global and Static
 * {
 */
/*
 * }
 */
/* *INDENT-ON* */

/*
 * Globals required at end because they reference static procedures/tables above.
 * {
 */
/**
 * \brief
 *   List of shell options for 'group' shell commands (list, info)
 */
sh_sand_cmd_t Sh_dnx_field_group_cmds[] = {
    {"list", diag_dnx_field_group_list_cb, NULL, Field_group_list_options, &Field_group_list_man}
    ,
    {"info", diag_dnx_field_group_info_cb, NULL, Field_group_info_options, &Field_group_info_man}
    ,
    {NULL}
};

/*
 * }
 */
