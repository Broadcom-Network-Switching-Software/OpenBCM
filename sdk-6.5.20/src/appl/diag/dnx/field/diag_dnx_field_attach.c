/** \file diag_dnx_field_attach.c
 *
 * Diagnostics procedures, for DNX, for 'attach' operations.
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
#include "diag_dnx_field_attach.h"
#include <bcm_int/dnx/field/field_group.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_context_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_qualifier_access.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include "diag_dnx_field_utils.h"

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
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

/**
 *  'Help' description for Attach info (shell commands).
 */
static sh_sand_man_t Field_attach_info_man = {
    .brief = "'Attach'- displays the Field attach info about a specific field group and context (filter by field group id and context)",
    .full = "'Attach' displays Field attach info of a given FG and context.\r\n"
      "If no 'group' or 'context' are specified then an error will occur.\r\n"
      "Input parameters 'group' and 'context' are mandatory!\r\n",
    .synopsis = "[group=<0-127>] [context=<0-63>]",
    .examples = "group=1 context=63\n"
                "group=10 context=5\n"
                "group=13 context=0",
};
static sh_sand_option_t Field_attach_info_options[] = {
    {DIAG_DNX_FIELD_OPTION_GROUP,     SAL_FIELD_TYPE_UINT32, "Field group id to get its attach info",     NULL   ,(void *)Field_group_enum_table, "MIN-MAX"},
    {DIAG_DNX_FIELD_OPTION_CONTEXT,  SAL_FIELD_TYPE_UINT32, "Field context id to which the FG is attached"},
    {NULL}
};
/*
 * }
 */
/* *INDENT-ON* */

/**
 * \brief
 *   This function set all needed information about used qualifiers by a specific FG.
 * \param [in] unit - The unit number.
 * \param [in] qual_iter - Id of the current qualifier.
 * \param [in] context_id - Id of the current Context.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] attach_full_info_p - Pointer, which contains all Attach related information.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_attach_info_qual_print(
    int unit,
    int qual_iter,
    dnx_field_context_t context_id,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    prt_control_t * prt_ctr)
{
    bcm_field_input_types_t bcm_input_type;

    SHR_FUNC_INIT_VARS(unit);

    /** Print all needed info about current qualifier. */
    PRT_CELL_SET("%s", dnx_field_dnx_qual_text(unit, attach_full_info_p->attach_basic_info.dnx_quals[qual_iter]));
    PRT_CELL_SET("%d", fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size);
    if (fg_info_p->group_basic_info.field_stage != DNX_FIELD_STAGE_EXTERNAL)
    {
        PRT_CELL_SET("%d", fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].lsb);
    }

    SHR_IF_ERR_EXIT(dnx_field_map_qual_input_type_dnx_to_bcm(unit,
                                                             attach_full_info_p->attach_basic_info.
                                                             qual_info[qual_iter].input_type, &bcm_input_type));
    PRT_CELL_SET("%s", dnx_field_bcm_input_type_text(bcm_input_type));
    if (attach_full_info_p->attach_basic_info.qual_info[qual_iter].input_type == DNX_FIELD_INPUT_TYPE_META_DATA ||
        attach_full_info_p->attach_basic_info.qual_info[qual_iter].input_type == DNX_FIELD_INPUT_TYPE_META_DATA2)
    {
        PRT_CELL_SET("%s", "-");
        if (DNX_QUAL_CLASS(attach_full_info_p->attach_basic_info.dnx_quals[qual_iter]) == DNX_FIELD_QUAL_CLASS_USER)
        {
            PRT_CELL_SET("%d", fg_info_p->group_full_info.context_info[context_id].qual_attach_info[qual_iter].offset);
        }
        else
        {
            PRT_CELL_SET("%s", "-");
        }
    }
    else if (attach_full_info_p->attach_basic_info.qual_info[qual_iter].input_type ==
             DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD ||
             attach_full_info_p->attach_basic_info.qual_info[qual_iter].input_type ==
             DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE)
    {
        PRT_CELL_SET("%d", fg_info_p->group_full_info.context_info[context_id].qual_attach_info[qual_iter].input_arg);
        if (DNX_QUAL_CLASS(attach_full_info_p->attach_basic_info.dnx_quals[qual_iter]) == DNX_FIELD_QUAL_CLASS_USER)
        {
            PRT_CELL_SET("%d", fg_info_p->group_full_info.context_info[context_id].qual_attach_info[qual_iter].offset);
        }
        else
        {
            PRT_CELL_SET("%s", "-");
        }
    }
    else if (attach_full_info_p->attach_basic_info.qual_info[qual_iter].input_type == DNX_FIELD_INPUT_TYPE_KBP)
    {
        PRT_CELL_SET("%s", "-");
        PRT_CELL_SET("%d", fg_info_p->group_full_info.context_info[context_id].qual_attach_info[qual_iter].offset);
    }
    else if (attach_full_info_p->attach_basic_info.qual_info[qual_iter].input_type == DNX_FIELD_INPUT_TYPE_CONST)
    {
        PRT_CELL_SET("0x%X", fg_info_p->group_full_info.context_info[context_id].qual_attach_info[qual_iter].input_arg);
        PRT_CELL_SET("%s", "-");
    }
    else
    {
        PRT_CELL_SET("%d", fg_info_p->group_full_info.context_info[context_id].qual_attach_info[qual_iter].input_arg);
        PRT_CELL_SET("%d", fg_info_p->group_full_info.context_info[context_id].qual_attach_info[qual_iter].offset);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function set all needed information about used actions by a specific FG.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Id of the current FG.
 * \param [in] action_iter - Id of the current action.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] attach_full_info_p - Pointer, which contains all Attach related information.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_attach_info_action_print(
    int unit,
    dnx_field_group_t fg_id,
    int action_iter,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    prt_control_t * prt_ctr)
{
    int action_offset;
    unsigned int action_size;
    int action_is_void;

    SHR_FUNC_INIT_VARS(unit);

    action_offset = 0;

    /** Get action size for the current action. */
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, fg_info_p->group_basic_info.field_stage,
                                                  attach_full_info_p->attach_basic_info.dnx_actions[action_iter],
                                                  &action_size));

    /** Take the offset for the current action. */
    SHR_IF_ERR_EXIT(dnx_field_group_action_offset_get(unit, fg_id,
                                                      attach_full_info_p->attach_basic_info.dnx_actions[action_iter],
                                                      &action_offset));

    /** Print all needed info about current action. */
    PRT_CELL_SET("%s", dnx_field_dnx_action_text(unit, attach_full_info_p->attach_basic_info.dnx_actions[action_iter]));
    if (fg_info_p->group_basic_info.fg_type != DNX_FIELD_GROUP_TYPE_CONST)
    {
        PRT_CELL_SET("%d", action_size);
        PRT_CELL_SET("%d", action_offset);
    }
    /** In case of External stage the action priority is irrelevant, then we remove it from the table. */
    if (fg_info_p->group_basic_info.field_stage != DNX_FIELD_STAGE_EXTERNAL)
    {
        if (attach_full_info_p->attach_basic_info.action_info[action_iter].priority == BCM_FIELD_ACTION_DONT_CARE)
        {
            PRT_CELL_SET("%s", "Don't care");
        }
        else if (attach_full_info_p->attach_basic_info.action_info[action_iter].priority == BCM_FIELD_ACTION_INVALIDATE)
        {
            PRT_CELL_SET("%s", "Disabled");
        }
        else
        {
            PRT_CELL_SET("0x%X", attach_full_info_p->attach_basic_info.action_info[action_iter].priority);
        }
    }

    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_is_void
                    (unit, fg_info_p->group_basic_info.field_stage,
                     attach_full_info_p->attach_basic_info.dnx_actions[action_iter], &action_is_void));
    if (fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[action_iter].dont_use_valid_bit
        || action_is_void)
    {
        PRT_CELL_SET("%s", "N/A");
    }
    else
    {
        PRT_CELL_SET("%d", attach_full_info_p->attach_basic_info.action_info[action_iter].valid_bit_polarity);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints key and payload information about a specific FG.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Id of the current FG.
 * \param [in] context_id - Id of the current Context.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] attach_full_info_p - Pointer, which contains all Attach related information.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_attach_info_key_and_payload_print(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    sh_sand_control_t * sand_control)
{
    int qual_action_iter;
    int max_num_qual_actions;
    int qual_left, action_left;
    int max_num_actions;
    int max_num_quals;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Used as flags to indicate if we have any quals/actions to print. */
    qual_left = action_left = TRUE;
    max_num_quals = dnx_data_field.group.nof_quals_per_fg_get(unit);
    max_num_actions = dnx_data_field.group.nof_action_per_fg_get(unit);
    /** The max number of qualifiers and actions per field group (32). */
    max_num_qual_actions = MAX(max_num_quals, max_num_actions);

    PRT_TITLE_SET("Key & Payload Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Size");
    /** In case of External stage the qualify LSB is irrelevant, then we remove it from the table. */
    if (fg_info_p->group_basic_info.field_stage != DNX_FIELD_STAGE_EXTERNAL)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Lsb");
    }
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Input Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Input Arg");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Input Offset");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, " ");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Lsb");
    /** In case of External stage the action priority is irrelevant, then we remove it from the table. */
    if (fg_info_p->group_basic_info.field_stage != DNX_FIELD_STAGE_EXTERNAL)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Priority");
    }
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Polarity");

    /** Iterate over maximum number of qualifiers and actions per FG, to retrieve information about these, which are valid. */
    for (qual_action_iter = 0; qual_action_iter < max_num_qual_actions && (qual_left || action_left);
         qual_action_iter++)
    {
        /**
         * Check if the qual_action_iter is still in the qualifiers range (DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) and if the
         * current qualifier is invalid (DNX_FIELD_QUAL_TYPE_INVALID), we should set the qual_left to FALSE,
         * which will indicate that we should stop printing any info related to qualifiers.
         */
        if (qual_action_iter < max_num_quals
            && attach_full_info_p->attach_basic_info.dnx_quals[qual_action_iter] == DNX_FIELD_QUAL_TYPE_INVALID)
        {
            qual_left = FALSE;
        }
        /**
         * Check if the qual_action_iter is still in the actions range (DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG) and if the
         * current action is invalid (DNX_FIELD_ACTION_INVALID), we should set the action_left to FALSE,
         * which will indicate that we should stop printing any info related to actions.
         */
        if (qual_action_iter < max_num_actions
            && attach_full_info_p->attach_basic_info.dnx_actions[qual_action_iter] == DNX_FIELD_ACTION_INVALID)
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
            SHR_IF_ERR_EXIT(diag_dnx_field_attach_info_qual_print
                            (unit, qual_action_iter, context_id, fg_info_p, attach_full_info_p, prt_ctr));
        }
        else
        {
            qual_left = FALSE;
            /** In case of External stage the action priority is irrelevant, then we remove it from the table. */
            if (fg_info_p->group_basic_info.field_stage == DNX_FIELD_STAGE_EXTERNAL)
            {
                /**
                 * Skip 5 cells (Qual, Size, Input Type, Input Arg, Input Offset) about qualifier info
                 * if no more valid qualifiers were found, but we still have valid actions to be printed.
                 */
                PRT_CELL_SKIP(5);
            }
            else
            {
                /**
                 * Skip 6 cells (Qual, Size, Lsb, Input Type, Input Arg, Input Offset) about qualifier info
                 * if no more valid qualifiers were found, but we still have valid actions to be printed.
                 */
                PRT_CELL_SKIP(6);
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
            SHR_IF_ERR_EXIT(diag_dnx_field_attach_info_action_print
                            (unit, fg_id, qual_action_iter, fg_info_p, attach_full_info_p, prt_ctr));
        }
        else
        {
            action_left = FALSE;
            /** In case of External stage the action priority is irrelevant, then we remove it from the table. */
            if (fg_info_p->group_basic_info.field_stage == DNX_FIELD_STAGE_EXTERNAL)
            {
                /**
                 * Skip 3 cells (Action, Size, Lsb) about action info if no more valid actions were found,
                 * but we still have valid qualifiers to be printed.
                 */
                PRT_CELL_SKIP(4);
            }
            else
            {
                /**
                 * Skip 4 cells (Action, Size, Lsb, Priority) about action info if no more valid actions were found,
                 * but we still have valid qualifiers to be printed.
                 */
                PRT_CELL_SKIP(5);
            }
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnx_field_attach_info_const_payload_print(
    int unit,
    dnx_field_group_t fg_iter,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
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
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Priority");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Polarity");

    /** Iterate over maximum number of actions per FG, to retrieve information about these, which are valid. */
    for (action_iter = 0; action_iter < max_num_actions; action_iter++)
    {
        /**
         * Check if the action_iter is still in the actions range (DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG) and if the
         * current action is invalid (DNX_FIELD_ACTION_INVALID), we should set the action_left to FALSE,
         * which will indicate that we should stop printing any info related to actions.
         */
        if (action_iter < max_num_actions &&
            fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[action_iter].dnx_action ==
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
            SHR_IF_ERR_EXIT(diag_dnx_field_attach_info_action_print
                            (unit, fg_iter, action_iter, fg_info_p, attach_full_info_p, prt_ctr));
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
 *   This function prepares/sets/prints information about which keys are
 *   being used by the given FG attached to the given context.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage on which the context was created.
 * \param [in] fg_id - Id of the group which is attached to the given context.
 * \param [in] context_id - Id of the current context.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_attach_info_fg_key_print(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    sh_sand_control_t * sand_control)
{
    dnx_field_group_context_full_info_t *attach_full_info = NULL;
    int key_id_iter;
    char keys_per_fg_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(attach_full_info, sizeof(dnx_field_group_context_full_info_t), "attach_full_info", "%s%s%s\r\n", EMPTY,
              EMPTY, EMPTY);

    PRT_TITLE_SET("Attached FG %d Key usage", fg_id);
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "KEY ID");
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    /** Get attach info for the current context in fgs_per_context[]. */
    SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fg_id, context_id, attach_full_info));

    /** Iterate over max number of keys in double key and check if we a valid key id. */
    for (key_id_iter = 0; key_id_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX &&
         attach_full_info->key_id.id[key_id_iter] != DNX_FIELD_KEY_ID_INVALID; key_id_iter++)
    {
        /**
          * Set a temporary string buffer to store the KEY name (A, B, C, D...)
          * then concatenate it to keys_per_fg_string and print them later one.
          */
        char key_id_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                              "%s, ",
                                              Field_key_enum_table[attach_full_info->key_id.id[key_id_iter]].string);
        sal_snprintf(key_id_buff, sizeof(key_id_buff), "%s, ",
                     Field_key_enum_table[attach_full_info->key_id.id[key_id_iter]].string);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(keys_per_fg_string, "%s", key_id_buff);
        sal_strncat(keys_per_fg_string, key_id_buff, sizeof(keys_per_fg_string) - 1);
    }

    /**
      * Extract last 2 symbols of the constructed string
      * to not present comma and space ", " at the end of it.
      */
    keys_per_fg_string[sal_strlen(keys_per_fg_string) - 2] = '\0';
    PRT_CELL_SET("%s", keys_per_fg_string);

    PRT_COMMITX;
exit:
    SHR_FREE(attach_full_info);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints FFCs and qualifiers, which are using them about a specific FG.
 * \param [in] unit - The unit number.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] attach_full_info_p - Pointer, which contains all Attach related information.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_attach_info_ffc_print(
    int unit,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    sh_sand_control_t * sand_control)
{
    int qual_iter, ffc_iter;
    int ffc_iter_internal;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FFC Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FFC ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Type");

    /** Prepare and print FFCs and qualifiers which are using them. */
    for (qual_iter = 0; qual_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
         attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_iter].qual_type !=
         DNX_FIELD_QUAL_TYPE_INVALID; qual_iter++)
    {
        for (ffc_iter = 0; ffc_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_QUAL &&
             attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[ffc_iter].ffc.ffc_id !=
             DNX_FIELD_FFC_ID_INVALID; ffc_iter++)
        {
            for (ffc_iter_internal = 0; ffc_iter_internal < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC;
                 ffc_iter_internal++)
            {
                if (ffc_iter_internal ==
                    attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[ffc_iter].ffc.ffc_id)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

                    PRT_CELL_SET("FFC %d",
                                 attach_full_info_p->group_ffc_info.key.
                                 qualifier_ffc_info[qual_iter].ffc_info[ffc_iter].ffc.ffc_id);

                    /** Print qualifier name. */
                    PRT_CELL_SET("%s", dnx_field_dnx_qual_text(unit,
                                                               attach_full_info_p->group_ffc_info.
                                                               key.qualifier_ffc_info[qual_iter].qual_type));
                }
            }
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints FFCs and qualifiers, which are using them about a specific External FG.
 * \param [in] unit - The unit number.
 * \param [in] opcode_id - opcode ID (DNX vlaue of context ID for KBP).
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] attach_full_info_p - Pointer, which contains all Attach related information.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_attach_info_external_ffc_print(
    int unit,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    sh_sand_control_t * sand_control)
{
    int qual_iter, ffc_iter;
    uint8 fwd_nof_contexts;
    kbp_mngr_fwd_acl_context_mapping_t fwd_acl_ctx_mapping[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
    unsigned int fwd_context_idx;
    unsigned int acl_context_idx;
    uint8 print_new_acl_context;
    uint8 print_new_fwd_context;
    uint8 do_display;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    print_new_acl_context = FALSE;
    print_new_fwd_context = FALSE;
    do_display = FALSE;

    PRT_TITLE_SET("FFC Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FWD Context");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ACL Context");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FFC ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Type");

    /** Prepare and print FFCs and qualifiers which are using them. */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_to_contexts_get(unit, opcode_id, &fwd_nof_contexts, fwd_acl_ctx_mapping));
    for (fwd_context_idx = 0; fwd_context_idx < fwd_nof_contexts; fwd_context_idx++)
    {
        print_new_fwd_context = TRUE;
        for (acl_context_idx = 0; acl_context_idx < fwd_acl_ctx_mapping[fwd_context_idx].nof_acl_contexts;
             acl_context_idx++)
        {
            print_new_acl_context = TRUE;

            /** Prepare and print FFCs and qualifiers which are using them. */
            for (qual_iter = 0; qual_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
                 attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_iter].qual_type !=
                 DNX_FIELD_QUAL_TYPE_INVALID; qual_iter++)
            {
                for (ffc_iter = 0; ffc_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_QUAL &&
                     attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[ffc_iter].
                     ffc.ffc_id != DNX_FIELD_FFC_ID_INVALID; ffc_iter++)
                {
                    /**
                     * In case the flag print_new_fwd_context is set to TRUE,
                     * we will print FWD Context ID on the first line of the table.
                     * Otherwise we skip the FWD COntext ID cell.
                     */
                    if (print_new_fwd_context)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                        PRT_CELL_SET("%d", fwd_acl_ctx_mapping[fwd_context_idx].fwd_context);
                        print_new_fwd_context = FALSE;
                    }
                    else
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SKIP(1);
                    }

                    /**
                     * In case the flag print_new_acl_context is set to TRUE,
                     * we will print ACL Context ID on the first line of the table.
                     * Otherwise we skip the ACL COntext ID cell.
                     */
                    if (print_new_acl_context)
                    {
                        PRT_CELL_SET("%d", fwd_acl_ctx_mapping[fwd_context_idx].acl_contexts[acl_context_idx]);
                        print_new_acl_context = FALSE;
                    }
                    else
                    {
                        PRT_CELL_SKIP(1);
                    }

                    PRT_CELL_SET("FFC %d",
                                 attach_full_info_p->group_ffc_info.key.
                                 qualifier_ffc_info[qual_iter].ffc_info[ffc_iter].ffc.ffc_id);
                    PRT_CELL_SET("%s",
                                 dnx_field_dnx_qual_text(unit,
                                                         attach_full_info_p->group_ffc_info.
                                                         key.qualifier_ffc_info[qual_iter].qual_type));

                    do_display = TRUE;
                }
            }
        }
    }

    if (do_display)
    {
        PRT_COMMITX;
    }
    else
    {
        PRT_FREE;
        LOG_CLI_EX("\r\n" "No FFC info was found for External stage opcode_id %d!!!%s%s%s\r\n\n", opcode_id, EMPTY,
                   EMPTY, EMPTY);
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints information about master key,
 *   being used by External FGs attached to the current context.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage on which the context was created.
 * \param [in] fg_id - Id of the given field group.
 * \param [in] opcode_id - opcode ID (DNX vlaue of context ID for KBP).
 * \param [in] attach_full_info_p - Pointer, which contains all Attach related information.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_attach_info_external_master_key_print(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    sh_sand_control_t * sand_control)
{
    int qual_iter;
    kbp_mngr_key_segment_t ms_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    uint32 nof_segments;
    uint8 do_display;
    uint32 payload_ids[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS];
    uint8 nof_fwd_segments;
    uint8 seg_index_on_master_by_qual[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    uint8 qual_index_on_segment[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    uint8 qual_offset_on_segment[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    do_display = FALSE;

    PRT_TITLE_SET("Master Key Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qualifier");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Lsb");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Size on Key");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Segment Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Used by FGs");

    SHR_IF_ERR_EXIT(diag_dnx_field_utils_payload_ids_per_fg_get(unit, opcode_id, payload_ids));
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_segments, ms_key_segments));
    SHR_IF_ERR_EXIT(dnx_field_group_kbp_opcode_nof_fwd_segments(unit, opcode_id, &nof_fwd_segments));
    SHR_IF_ERR_EXIT(dnx_field_group_kbp_segment_indices(unit, fg_id, opcode_id,
                                                        seg_index_on_master_by_qual, qual_index_on_segment,
                                                        qual_offset_on_segment));

    /** Iterate over max number of qualifiers for FG and check if current qual is valid for it. */
    for (qual_iter = 0; qual_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
         attach_full_info_p->attach_basic_info.dnx_quals[qual_iter] != DNX_FIELD_QUAL_TYPE_INVALID; qual_iter++)
    {
        dbal_enum_value_field_kbp_kbr_idx_e kbp_kbr_id;
        uint32 qual_size;
        char segment_sharing_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
        uint32 offset_in_kbp_kbr;
        uint32 seg_index;
        uint32 offset_on_segment;
        int qual_size_on_key;
        int qual_packed_and_not_first;

        seg_index = seg_index_on_master_by_qual[qual_iter];
        if (seg_index >= nof_segments)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "qualifier number %d in field group %d uses segment %d, only %d segments in master key.\r\n",
                         qual_iter, fg_id, seg_index, nof_segments);
        }
        offset_on_segment = qual_offset_on_segment[qual_iter];

        SHR_IF_ERR_EXIT(diag_dnx_field_utils_kbp_segment_share_string_update
                        (unit, fg_id, opcode_id, seg_index, ms_key_segments, nof_fwd_segments, payload_ids,
                         &(attach_full_info_p->attach_basic_info), qual_iter, segment_sharing_string));

        /*
         * Get the qual size.
         */
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                        (unit, field_stage, attach_full_info_p->attach_basic_info.dnx_quals[qual_iter], &qual_size));

        qual_size_on_key = BYTES2BITS(ms_key_segments[seg_index].nof_bytes);

        if (qual_index_on_segment[qual_iter] > 0)
        {
            qual_packed_and_not_first = TRUE;
        }
        else
        {
            qual_packed_and_not_first = FALSE;
        }

        if (offset_on_segment + qual_size > BYTES2BITS(ms_key_segments[seg_index].nof_bytes))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "For opcode_id %d, fg_id %d, qualifier number %d (0x%x), does not fit "
                         "withing segment. Offset within segment %d, qual size %d, segment size %d.\n",
                         opcode_id, fg_id, qual_iter,
                         attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_iter].qual_type,
                         offset_on_segment, qual_size, BYTES2BITS(ms_key_segments[seg_index].nof_bytes));
        }

        /*
         * Get the segment placement on key.
         */
        if (ms_key_segments[seg_index].is_overlay_field)
        {
            uint32 keys_bmp;
            int offset_on_last_key;
            if (attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[0].ffc.ffc_id !=
                DNX_FIELD_FFC_ID_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "For opcode_id %d, fg_id %d, qualifier number %d (0x%x), FFC found, "
                             "but the qualifeir is an overlay segemnt.\n",
                             opcode_id, fg_id, qual_iter,
                             attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_iter].qual_type);
            }
            SHR_IF_ERR_EXIT(dnx_field_key_kbp_master_offset_to_key(unit, opcode_id,
                                                                   BYTES2BITS(ms_key_segments
                                                                              [seg_index].overlay_offset_bytes),
                                                                   qual_size_on_key, &kbp_kbr_id, &keys_bmp,
                                                                   &offset_on_last_key));
            offset_in_kbp_kbr = offset_on_last_key;
        }
        else
        {
            if (attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[0].ffc.ffc_id ==
                DNX_FIELD_FFC_ID_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "For opcode_id %d, fg_id %d, qualifier number %d (0x%x), no FFC found.\n",
                             opcode_id, fg_id, qual_iter,
                             attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_iter].qual_type);
            }

            SHR_IF_ERR_EXIT(dnx_field_key_fwd2_to_acl_key_id_convert
                            (unit,
                             attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[0].
                             ffc.key_id, &kbp_kbr_id));
            offset_in_kbp_kbr =
                attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[0].ffc.
                ffc_instruction.key_offset;
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", dnx_field_dnx_qual_text(unit, attach_full_info_p->attach_basic_info.dnx_quals[qual_iter]));
        PRT_CELL_SET("%s", Field_key_enum_table[kbp_kbr_id].string);
        PRT_CELL_SET("%d", offset_in_kbp_kbr);
        PRT_CELL_SET("%d", qual_size);
        if (qual_packed_and_not_first)
        {
            PRT_CELL_SET("-");
        }
        else
        {
            PRT_CELL_SET("%d", qual_size_on_key);
        }
        PRT_CELL_SET("%s", ms_key_segments[seg_index].name);
        PRT_CELL_SET("%s", segment_sharing_string);

        do_display = TRUE;

    }

    if (do_display)
    {
        PRT_COMMITX;
        LOG_CLI_EX("\r\n" "In the \"Used by FGs\" column, FWD indicates that the segment is in the FWD part of the "
                   "master key, FWD-KBP indicates that the segment is being used by a FWD KBP lookup, and FWD-ZP "
                   "indicates that it is a filled zero padding segment.\n"
                   "The marking \"overlay\" indicates that the segment is an overlay segment, meaning it does not "
                   "take space on the master key by itself, but reads from non overlay segments.%s%s%s%s\r\n\n",
                   EMPTY, EMPTY, EMPTY, EMPTY);
    }
    else
    {
        PRT_FREE;
        LOG_CLI_EX("\r\n" "No Master key info was found for External stage opcode_id %d!!!%s%s%s\r\n\n", opcode_id,
                   EMPTY, EMPTY, EMPTY);
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints FESs and actions, which are using them about a specific FG.
 * \param [in] unit - The unit number.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] attach_full_info_p - Pointer, which contains all Attach related information.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_attach_info_fes_print(
    int unit,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    sh_sand_control_t * sand_control)
{
    int action_iter;
    int fes2msb_index;
    uint8 is_all_fes2msb_actions_same;
    bcm_field_action_t bcm_action;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FES Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FES ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Type");

    /** Iterate over used FESs and print their IDs and actions, which are using them. */
    for (action_iter = 0;
         (action_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP) &&
         (attach_full_info_p->attach_basic_info.dnx_actions[action_iter] != DNX_FIELD_ACTION_INVALID); action_iter++)
    {
        if (attach_full_info_p->actions_fes_info.initial_fes_quartets[action_iter].fes_id == DNX_FIELD_EFES_ID_INVALID)
        {
            continue;
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

        PRT_CELL_SET("FES %d", attach_full_info_p->actions_fes_info.initial_fes_quartets[action_iter].fes_id);

        SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm
                        (unit, fg_info_p->group_basic_info.field_stage,
                         attach_full_info_p->attach_basic_info.dnx_actions[action_iter], &bcm_action));
        PRT_CELL_SET("%s", dnx_field_bcm_action_text(unit, bcm_action));
    }

    for (action_iter = 0;
         (action_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP) &&
         (attach_full_info_p->actions_fes_info.added_fes_quartets[action_iter].fes_id != DNX_FIELD_EFES_ID_INVALID);
         action_iter++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("FES %d (added)", attach_full_info_p->actions_fes_info.added_fes_quartets[action_iter].fes_id);

        is_all_fes2msb_actions_same = TRUE;
        for (fes2msb_index = 0; fes2msb_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES; fes2msb_index++)
        {
            if (attach_full_info_p->actions_fes_info.added_fes_quartets[action_iter].fes2msb_info[0].action_type !=
                attach_full_info_p->actions_fes_info.added_fes_quartets[action_iter].
                fes2msb_info[fes2msb_index].action_type)
            {
                is_all_fes2msb_actions_same = FALSE;
                break;
            }
        }

        if (is_all_fes2msb_actions_same)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm
                            (unit, fg_info_p->group_basic_info.field_stage,
                             DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, fg_info_p->group_basic_info.field_stage,
                                        attach_full_info_p->actions_fes_info.
                                        added_fes_quartets[action_iter].fes2msb_info[0].action_type), &bcm_action));
            PRT_CELL_SET("%s", dnx_field_bcm_action_text(unit, bcm_action));
        }
        else
        {
            dnx_field_action_type_t invalid_action_type;
            SHR_IF_ERR_EXIT(dnx_field_map_get_invalid_action_type
                            (unit, fg_info_p->group_basic_info.field_stage, &invalid_action_type));
            for (fes2msb_index = 0; fes2msb_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES; fes2msb_index++)
            {
                if (fes2msb_index != 0)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SKIP(1);
                }
                if (attach_full_info_p->actions_fes_info.added_fes_quartets[action_iter].
                    fes2msb_info[fes2msb_index].action_type == invalid_action_type)
                {
                    PRT_CELL_SET("%s", "Invalid");
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm
                                    (unit, fg_info_p->group_basic_info.field_stage,
                                     DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, fg_info_p->group_basic_info.field_stage,
                                                attach_full_info_p->actions_fes_info.
                                                added_fes_quartets[action_iter].fes2msb_info[fes2msb_index].
                                                action_type), &bcm_action));
                    PRT_CELL_SET("%s", dnx_field_bcm_action_text(unit, bcm_action));
                }
            }
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints External Lookup info, ID and offset.
 * \param [in] unit - The unit number.
 * \param [in] attach_full_info_p - Pointer, which contains all Attach related information.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_attach_info_ifwd2_lookup_info_print(
    int unit,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    sh_sand_control_t * sand_control)
{
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Payload Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Payload ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Payload offset (lsb of payload from lsb of buffer)");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    PRT_CELL_SET("%d", attach_full_info_p->attach_basic_info.payload_id);
    PRT_CELL_SET("%d", attach_full_info_p->attach_basic_info.payload_offset);

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays Field Attach info per given group ID and context ID, specified by the caller.
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
diag_dnx_field_attach_info_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 is_allocated;
    uint32 is_attached;
    dnx_field_group_t fg_id;
    dnx_field_context_t context_id;
    bcm_field_AppType_t app_type = bcmFieldAppTypeCount;
    dnx_field_group_context_full_info_t *attach_full_info = NULL;
    dnx_field_group_full_info_t *fg_info = NULL;
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_GROUP, fg_id);
    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_CONTEXT, context_id);

    SHR_ALLOC(attach_full_info, sizeof(dnx_field_group_context_full_info_t), "attach_full_info", "%s%s%s\r\n", EMPTY,
              EMPTY, EMPTY);
    SHR_ALLOC(fg_info, sizeof(dnx_field_group_full_info_t), "fg_info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Check if the specified FG ID or rage of FG IDs are allocated. */
    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
    if (!is_allocated)
    {
        LOG_CLI_EX("\r\n" "The given FG Id %d is not allocated%s%s%s\r\n\n", fg_id, EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }

    /** Get information about the current Field Group. */
    SHR_IF_ERR_EXIT(dnx_field_group_get(unit, fg_id, fg_info));

    /** Check if the context ID is within range. */
    if (fg_info->group_basic_info.field_stage == DNX_FIELD_STAGE_EXTERNAL)
    {
        if ((context_id < dnx_data_field.kbp.apptype_user_1st_get(unit)) ||
            (context_id >= dnx_data_field.kbp.apptype_user_1st_get(unit) +
             dnx_data_field.kbp.apptype_user_nof_get(unit)))
        {
            if ((((int) context_id) < 0) || context_id >= bcmFieldAppTypeCount)
            {
                LOG_CLI_EX("\r\n"
                           "The given context %d is not within the valid range 0-%d, %d-%d, for external stage.\r\n\n",
                           context_id, bcmFieldAppTypeCount - 1,
                           dnx_data_field.kbp.apptype_user_1st_get(unit),
                           dnx_data_field.kbp.apptype_user_1st_get(unit) +
                           dnx_data_field.kbp.apptype_user_nof_get(unit));
                SHR_EXIT();
            }
        }
    }
    else
    {
        if ((((int) context_id) < 0) ||
            (context_id >=
             dnx_data_field.stage.stage_info_get(unit, fg_info->group_basic_info.field_stage)->nof_contexts))
        {
            LOG_CLI_EX("\r\n" "The given context %d is not within the valid range 0-%d, for stage \"%s\".%s\r\n\n",
                       context_id,
                       dnx_data_field.stage.stage_info_get(unit, fg_info->group_basic_info.field_stage)->nof_contexts,
                       dnx_field_stage_text(unit, fg_info->group_basic_info.field_stage), EMPTY);
            SHR_EXIT();
        }
    }

    if (fg_info->group_basic_info.field_stage == DNX_FIELD_STAGE_EXTERNAL)
    {
        dbal_enum_value_field_kbp_fwd_opcode_e opcode_id;
        SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
        if (DNX_KBP_STATUS_IS_LOCKED(kbp_mngr_status) == FALSE)
        {
            LOG_CLI_EX("\r\n"
                       "Attach Info, for External stage, will be presented only after device lock!!!%s%s%s%s\r\n\n",
                       EMPTY, EMPTY, EMPTY, EMPTY);
            SHR_EXIT();
        }

        /** Convert apptype to opcode and pass it on as context. */
        rv = dnx_field_map_apptype_to_opcode_bcm_to_dnx_int(unit, context_id, &opcode_id);
        if (rv == _SHR_E_NOT_FOUND)
        {
            LOG_CLI_EX("\r\n" "The given Apptype %d does not exist!!!%s%s%s\r\n\n", app_type, EMPTY, EMPTY, EMPTY);
            SHR_EXIT();
        }

        app_type = context_id;
        context_id = opcode_id;
    }
    else
    {
        /** Check if the specified Context ID is allocated. */
        SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated
                        (unit, fg_info->group_basic_info.field_stage, context_id, &is_allocated));

        if (!is_allocated)
        {
            LOG_CLI_EX("\r\n" "The given context Id %d is not allocated!!!%s%s%s\r\n\n",
                       context_id, EMPTY, EMPTY, EMPTY);
            SHR_EXIT();
        }
    }

    /** Check if the specified Context ID is attached to the field group. */
    SHR_IF_ERR_EXIT(dnx_field_group_is_context_id_on_arr(unit, fg_id, context_id, &is_attached));
    if (!is_attached)
    {
        if (fg_info->group_basic_info.field_stage == DNX_FIELD_STAGE_EXTERNAL)
        {
            LOG_CLI_EX("\r\n" "The given FG Id %d is not attached to Apptype %d (opcode ID %d).%s\r\n\n",
                       fg_id, app_type, context_id, EMPTY);
        }
        else
        {
            LOG_CLI_EX("\r\n" "The given FG Id %d is not attached to context %d.%s%s\r\n\n",
                       fg_id, context_id, EMPTY, EMPTY);
        }
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &(attach_full_info->attach_basic_info)));
    SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fg_id, context_id, attach_full_info));

    if (fg_info->group_basic_info.field_stage == DNX_FIELD_STAGE_EXTERNAL)
    {
        /**
         * Prepare and print a table, which contains attach information about key and payload
         * for a specific field group and context.
         */
        SHR_IF_ERR_EXIT(diag_dnx_field_attach_info_key_and_payload_print
                        (unit, fg_id, context_id, fg_info, attach_full_info, sand_control));

        /** In case of External stage we should print Lookup info. */
        SHR_IF_ERR_EXIT(diag_dnx_field_attach_info_ifwd2_lookup_info_print(unit, attach_full_info, sand_control));

        /**
         * Prepare and print a table, which contains FFCs and qualifiers, which are using them
         * for a specific External field group and context.
         */
        SHR_IF_ERR_EXIT(diag_dnx_field_attach_info_external_ffc_print
                        (unit, context_id, fg_info, attach_full_info, sand_control));

        /**
         * Prepare and print a table, which contains Master key info
         * for a specific External field group and context.
         */
        SHR_IF_ERR_EXIT(diag_dnx_field_attach_info_external_master_key_print
                        (unit, fg_info->group_basic_info.field_stage, fg_id, context_id, attach_full_info,
                         sand_control));
    }
    else
    {
        if (fg_info->group_basic_info.fg_type != DNX_FIELD_GROUP_TYPE_CONST)
        {
            /**
             * Prepare and print a table, which contains attach information about key and payload
             * for a specific field group and context.
             */

            SHR_IF_ERR_EXIT(diag_dnx_field_attach_info_key_and_payload_print
                            (unit, fg_id, context_id, fg_info, attach_full_info, sand_control));

            SHR_IF_ERR_EXIT(diag_dnx_field_attach_info_fg_key_print
                            (unit, fg_info->group_basic_info.field_stage, fg_id, context_id, sand_control));

            /**
             * Prepare and print a table, which contains FFCs and qualifiers, which are using them
             * for a specific field group and context.
             */
            SHR_IF_ERR_EXIT(diag_dnx_field_attach_info_ffc_print(unit, fg_info, attach_full_info, sand_control));
        }
        else
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_attach_info_const_payload_print
                            (unit, fg_id, fg_info, attach_full_info, sand_control));
        }

        /**
         * Prepare and print a table, which contains FESs and actions, which are using them
         * for a specific field group and context.
         */
        SHR_IF_ERR_EXIT(diag_dnx_field_attach_info_fes_print(unit, fg_info, attach_full_info, sand_control));
    }

exit:
    SHR_FREE(attach_full_info);
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
 *   List of shell options for 'attach' shell commands (info)
 */
sh_sand_cmd_t Sh_dnx_field_attach_cmds[] = {
    {"info", diag_dnx_field_attach_info_cb, NULL, Field_attach_info_options, &Field_attach_info_man}
    ,
    {NULL}
};

/*
 * }
 */
