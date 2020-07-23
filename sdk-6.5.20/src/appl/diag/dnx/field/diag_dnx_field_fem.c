/** \file diag_dnx_field_fem.c
 *
 * Diagnostics procedures, for DNX, for 'fem' operations (allocation/add/remove, context attach/detach)
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
#include "diag_dnx_field_fem.h"
#include <bcm_int/dnx/field/field_actions.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>

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
 *  'Help' description for FEM display (shell commands).
 */
static sh_sand_man_t Field_fem_info_by_fg_man = {
    .brief = "'FEM'-related display utilities (filter by field group). On prompt, type: 'field fem by_fg ...'",
    .full = "'FEM' display for a range of fems. Range is specified via 'group', 'sec_group' and 'fem id'\r\n"
      "If no 'group' is specified then '0'-'nof_fgs-1' is assumed.\r\n"
      "If no 'sec_group' is specified then '0'-'nof_fgs-1' is assumed.\r\n"
      "If no 'active_actions' is specified then it is ignored.\r\n"
      "If no 'id' is specified then '0'-'nof_fem_id-1' is assumed.\r\n"
      "If only one value is specified for group/sec_group then this 'single value' range is assumed.\r\n"
      "If 'valid' is not specified or is set to 'yes' then only group/sec_group combinations, with at"
      " least one allocated fem, are displayed.\r\n",
    .synopsis = "[group=<lower value>-<upper value>]"
                " [sec_group=<lower value>-<upper value>]"
                " [active_actions=<4-bits bitmap of required actions>]"
                " [id=<lower value>-<upper value>]"
                " [valid=<YES | NO>]",
    .examples = "valid=YES group=0-20 sec_group=min-max active_actions=ignore id=min-max",
};
static sh_sand_option_t Field_fem_info_by_fg_options[] = {
    {DIAG_DNX_FIELD_OPTION_GROUP,          SAL_FIELD_TYPE_UINT32, "Lowest-highest field group to get fem info for",           "MIN-MAX"   ,(void *)Field_group_enum_table, "MIN-MAX"},
    {DIAG_DNX_FIELD_OPTION_SEC_GROUP,      SAL_FIELD_TYPE_UINT32, "Lowest-highest secondary field group to get fem info for", "MIN-MAX"   ,(void *)Field_group_enum_table, "MIN-MAX"},
    {DIAG_DNX_FIELD_OPTION_VALID,          SAL_FIELD_TYPE_BOOL,   "If TRUE, display activated FEMs only",                     "Yes"},
    {DIAG_DNX_FIELD_OPTION_ACTIVE_ACTIONS, SAL_FIELD_TYPE_UINT32, "4-bits bitmap to indicate the active actions as filter",   "IGNORE"    ,(void *)Field_fem_active_actions_enum_table},
    {DIAG_DNX_FIELD_OPTION_ID,            SAL_FIELD_TYPE_UINT32, "Lowest-highest fem id to get info for",                    "MIN-MAX"   ,(void *)Field_fem_enum_table, "MIN-MAX"},
    {NULL}
};

static sh_sand_man_t Field_fem_info_by_context_man = {
    .brief = "'FEM'-related display utilities (filter by context). On prompt, type: 'field fem by_context ...'",
    .full = "'FEM' display for a range of fems. Range is specified via 'fem id' and 'context'\r\n"
      "If no context is specified then 'minimal val'-'maximal val' is assumed.\r\n"
      "If no 'id' is specified then '0'-'nof_fem_id-1' is assumed.\r\n"
      "If only one value is specified for context/fem then this 'single value' range is assumed.\r\n"
      "If 'valid' is not specified or is set to 'yes' then only contexts, with at"
      " least one allocated fem, are displayed.\r\n",
    .synopsis = "[context=<<Lowest context number>-<Highest context number>>]"
                " [id=<lower value>-<upper value>]"
                " [valid=<YES | NO>]",
    .examples = "valid=YES context=0-20 id=min-max",
};
static sh_sand_option_t Field_fem_info_by_context_options[] = {
    {DIAG_DNX_FIELD_OPTION_CONTEXT,    SAL_FIELD_TYPE_UINT32, "Lowest-highest context ID to get fem info for",  "0xFFFFFFFF"},
    {DIAG_DNX_FIELD_OPTION_VALID,      SAL_FIELD_TYPE_BOOL,   "If TRUE, display non-empty fem blocks only",     "Yes"},
    {DIAG_DNX_FIELD_OPTION_ID,        SAL_FIELD_TYPE_UINT32, "Lowest-highest fem id to get info for",          "MIN-MAX"   ,(void *)Field_fem_enum_table, "MIN-MAX"},
    {NULL}
};

static sh_sand_man_t Field_fem_display_conditions_man = {
    .brief = "'FEM'-related display utilities (for 'conditions'). On prompt, type: 'field fem condition ...'",
    .full = "'FEM' display for a range of fems. Range is specified via 'fem id' and 'fem program'\r\n"
      "If no 'program' is specified then 'minimal val'-'maximal val' is assumed.\r\n"
      "If no 'id' is specified then '0'-'nof_fem_id-1' is assumed.\r\n"
      "If only one value is specified for 'fem program'/fem then this 'single value' range is assumed.\r\n"
      "If 'valid' is not specified or is set to 'yes' then only 'fem programs', with at"
      " least one 'valid' condition, are displayed.\r\n",
    .synopsis = "[program=<<Lowest fem_program number>-<Highest fem_program number>>]"
                " [id=<lower value>-<upper value>]"
                " [valid=<YES | NO>]",
    .examples = "valid=YES program=1 id=0-4"
};
static sh_sand_option_t Field_fem_info_conditions_options[] = {
    {DIAG_DNX_FIELD_OPTION_PROGRAM,SAL_FIELD_TYPE_UINT32, "Lowest-highest fem_program to get condition info for",  "FEM_PROGRAM_LOWEST-FEM_PROGRAM_HIGHEST", (void *)Field_fem_program_enum_table_for_display, "FEM_PROGRAM_LOWEST-FEM_PROGRAM_HIGHEST"},
    {DIAG_DNX_FIELD_OPTION_VALID,      SAL_FIELD_TYPE_BOOL,   "If TRUE, display non-empty fem blocks only",            "Yes"},
    {DIAG_DNX_FIELD_OPTION_ID,        SAL_FIELD_TYPE_UINT32, "Lowest-highest fem id to get info for",                 "MIN-MAX"   ,(void *)Field_fem_enum_table, "MIN-MAX"},
    {NULL}
};

static sh_sand_man_t Field_fem_info_actions_man = {
    .brief = "'FEM'-related display utilities (for 'actions'). On prompt, type: 'field fem action ...'",
    .full = "'FEM' display for a range of fems. Range is specified via 'fem id'\r\n"
      "If no 'id' is specified then '0'-'nof_fem_id-1' is assumed.\r\n"
      "If only one value is specified for 'fem id' then this 'single value' range is assumed.\r\n"
      "If 'valid' is not specified or is set to 'yes' then only 'fem ids', with at"
      " least one 'valid' action, are displayed. An 'action' is 'valid' if it is pointed by\r\n"
      " at least one 'valid' condition. 'Condition is 'valid' if it specifies its pointed\r\n"
      " action to be 'valid'.\r\n",
    .synopsis = " [id=<lower value>-<upper value>]"
                " [valid=<YES | NO>]",
    .examples = "valid=YES id=0-4"
};
static sh_sand_option_t Field_fem_info_actions_options[] = {
    {DIAG_DNX_FIELD_OPTION_VALID,      SAL_FIELD_TYPE_BOOL,   "If TRUE, display non-empty fem blocks only",            "Yes"},
    {DIAG_DNX_FIELD_OPTION_ID,        SAL_FIELD_TYPE_UINT32, "Lowest-highest fem id to get info for",                 "MIN-MAX"   ,(void *)Field_fem_enum_table, "MIN-MAX"},
    {NULL}
};
/*
 * }
 */
/* *INDENT-ON* */

/**
 * \brief
 *   This function displays fems per filter (mainly by field group) as specified by the caller.
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
diag_dnx_field_fem_info_by_fg_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_group_t fg_id_lower, fg_id_upper;
    dnx_field_group_t second_fg_id_lower, second_fg_id_upper;
    int non_empty_only, first_fem_program_to_print;
    uint8 active_actions, active_actions_mask;
    dnx_field_fem_id_t fem_id_lower, fem_id_upper;
    int none_occupied, do_display;
    dnx_field_fem_id_t fem_id_index;
    dnx_field_fem_program_t fem_program_index, fem_program_max;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_GROUP, fg_id_lower, fg_id_upper);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_SEC_GROUP, second_fg_id_lower, second_fg_id_upper);
    SH_SAND_GET_BOOL(DIAG_DNX_FIELD_OPTION_VALID, non_empty_only);
    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_ACTIVE_ACTIONS, active_actions);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_ID, fem_id_lower, fem_id_upper);
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "fg_id: from %d to %d, second_fg_id: from %d to %d\r\n",
                 fg_id_lower, fg_id_upper, second_fg_id_lower, second_fg_id_upper);
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Display non empty entries only %d active_actions 0x%04X, from %d to %d fem_id:\r\n",
                 non_empty_only, (unsigned short) active_actions, fem_id_lower, fem_id_upper);
    SHR_IF_ERR_EXIT(dnx_field_fem_is_any_fem_occupied_on_fg(unit, DNX_FIELD_CONTEXT_ID_INVALID, DNX_FIELD_GROUP_INVALID,
                                                            DNX_FIELD_GROUP_INVALID, DNX_FIELD_IGNORE_ALL_ACTIONS,
                                                            &none_occupied));
    if (none_occupied)
    {
        LOG_CLI_EX("\r\n" "NO occupied fem was found anywhere!! %s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }
    fem_program_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs;
    active_actions_mask = SAL_UPTO_BIT(fem_program_max);
    if (active_actions != DNX_FIELD_IGNORE_ALL_ACTIONS)
    {
        if (active_actions & ~active_actions_mask)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Illegal 'active_actions' (0x%04X). May only have bits set within this mask: 0x%04X. Quit.\r\n",
                         (unsigned short) active_actions, (unsigned short) active_actions_mask);
        }
    }
    {
        if (non_empty_only)
        {
            PRT_TITLE_SET("%s - SWSTATE for FEM id: from %d to %d", "Active-only actions", fem_id_lower, fem_id_upper);
        }
        else
        {
            PRT_TITLE_SET("%s - SWSTATE for FEM id: from %d to %d", "All actions", fem_id_lower, fem_id_upper);
        }
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "fem_id");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "fg_id");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "input offset");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "second fg_id");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ignore actions");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "map index");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "encoded actions");
    }
    do_display = FALSE;
    for (fem_id_index = fem_id_lower; fem_id_index <= fem_id_upper; fem_id_index++)
    {
        /*
         * If a match is found then display all four 'actions' corresponding to this FEM id
         */
        first_fem_program_to_print = TRUE;
        for (fem_program_index = 0; fem_program_index < fem_program_max; fem_program_index++)
        {
            dnx_field_group_t fg_id;
            dnx_field_group_t second_fg_id;

            SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.fg_id.get(unit, fem_id_index, fem_program_index, &fg_id));
            if (non_empty_only)
            {
                if (fg_id == DNX_FIELD_GROUP_INVALID)
                {
                    /*
                     * This 'fem_program' is not occupied for specified 'fem_id'. Go to the next.
                     */
                    continue;
                }
            }
            /*
             * 'fem_id' is occupied. Check on the various filters.
             */
            if ((fg_id == DNX_FIELD_GROUP_INVALID) || ((fg_id >= fg_id_lower) && (fg_id <= fg_id_upper)))
            {
                SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.
                                second_fg_id.get(unit, fem_id_index, fem_program_index, &second_fg_id));
                if ((second_fg_id == DNX_FIELD_GROUP_INVALID)
                    || ((second_fg_id >= second_fg_id_lower) && (second_fg_id <= second_fg_id_upper)))
                {
                    if (active_actions != DNX_FIELD_IGNORE_ALL_ACTIONS)
                    {
                        uint8 sw_ignore_actions, sw_active_actions;

                        SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.
                                        ignore_actions.get(unit, fem_id_index, fem_program_index, &sw_ignore_actions));
                        /*
                         * From each of the 'action' bitmasks, take only the LS 4 bits.
                         */
                        sw_active_actions = (~sw_ignore_actions & active_actions_mask);
                        /*
                         * If not all bits, specified on input, are set on SWSTATE then this is not a match.
                         */
                        if ((active_actions & sw_active_actions) != active_actions)
                        {
                            continue;
                        }
                    }
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    if (first_fem_program_to_print == TRUE)
                    {
                        /*
                         * Print 'FEM id' on first printed program only.
                         */
                        PRT_CELL_SET("%d", fem_id_index);
                        first_fem_program_to_print = FALSE;
                    }
                    else
                    {
                        PRT_CELL_SKIP(1);
                    }
                    /*
                     * Skip 5 columns to get to the "map index" column:
                     * "fem_id", "fg_id", "input offset", "second fg_id", "ignore actions'
                     */
                    SHR_IF_ERR_EXIT(appl_dnx_action_swstate_fem_prg_display
                                    (unit, fem_id_index, fem_program_index, 5, sand_control, prt_ctr));
                    do_display = TRUE;
                }
            }
        }
    }
    if (do_display != FALSE)
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
        LOG_CLI_EX("\r\n" "NO occupied fem was found on specified filters!! %s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY,
                   EMPTY);
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   This function displays fems per filter (mainly by context) as specified by the caller.
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
diag_dnx_field_fem_info_by_context_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_fem_id_t fem_id_lower, fem_id_upper;
    dnx_field_context_t context_id_lower, context_id_upper;
    int non_empty_only, context_was_loaded, do_print;
    dnx_field_fem_id_t fem_id_index;
    dnx_field_context_t context_id_index;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_ID, fem_id_lower, fem_id_upper);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_CONTEXT, context_id_lower, context_id_upper);
    SH_SAND_GET_BOOL(DIAG_DNX_FIELD_OPTION_VALID, non_empty_only);

    /*
     * 0xFFFFFFFF will mark all contexts for context
     */
    if ((context_id_lower == 0xFFFFFFFF) && (context_id_upper == 0xFFFFFFFF))
    {
        context_id_lower = 0;
        context_id_upper = dnx_data_field.common_max_val.nof_contexts_get(unit) - 1;
    }
    if ((int) context_id_lower < 0 || context_id_lower >= dnx_data_field.common_max_val.nof_contexts_get(unit) ||
        (int) context_id_upper < 0 || context_id_upper >= dnx_data_field.common_max_val.nof_contexts_get(unit))
    {
        LOG_CLI_EX("\r\n" "Context provided (%d-%d) must be within range 0-%d.%s\r\n\n", context_id_lower,
                   context_id_upper, dnx_data_field.common_max_val.nof_contexts_get(unit) - 1, EMPTY);
        SHR_EXIT();
    }

    {
        /*
         * Print legend for 'fem contexts' table below.
         */
        PRT_TITLE_SET("Legend - For table of FEM contexts, below.");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "column name");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "description");
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "FM");
        PRT_CELL_SET("%s", "FEM");
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "ID");
        PRT_CELL_SET("%s", "FEM identifier.");
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "KSL");
        PRT_CELL_SET("%s",
                     "FEM key select (dbal_enum_value_field_field_pmf_a_fem_key_select_e). If set to '--' then FEM is not active.");
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "PRG");
        PRT_CELL_SET("%s", "FEM program (dnx_field_fem_program_t). One of four.");
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "RFLG");
        PRT_CELL_SET("%s", "Booelan flag of 'replace' ('override') feature. If non-zero then 'TCAM' is meaningful.");
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "TCAM");
        PRT_CELL_SET("%s", "Identifier of the TCAM result to take the LS 16 bits from. Range: 0 - 7.");
        PRT_COMMITX;
    }
    {
        if (non_empty_only)
        {
            PRT_TITLE_SET("%s - HW for context id: from %d to %d",
                          "Active-only contexts", context_id_lower, context_id_upper);
        }
        else
        {
            PRT_TITLE_SET("%s - HW for context id: from %d to %d", "All contexts", context_id_lower, context_id_upper);
        }
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Ctxt");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Field");
        for (fem_id_index = fem_id_lower; fem_id_index <= fem_id_upper; fem_id_index++)
        {
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FM %d", fem_id_index);
        }
    }
    do_print = FALSE;
    for (context_id_index = context_id_lower; context_id_index <= context_id_upper; context_id_index++)
    {
        SHR_IF_ERR_EXIT(appl_dnx_action_full_fem_context_display
                        (unit, context_id_index, non_empty_only, fem_id_lower, fem_id_upper, sand_control, prt_ctr,
                         &context_was_loaded));
        if (context_was_loaded)
        {
            /*
             * If a context was found such that at least one of the 'fem id's was 'valid'
             * then print the table. Otherwise, just print a message indicating all
             * contexts were 'empty'
             */
            do_print = TRUE;
        }
    }
    if (do_print != FALSE)
    {
        PRT_COMMITX;
    }
    else
    {
        /*
         * If nothing was printed then just clear loaded PRT memory.
         */
        LOG_CLI_EX("\r\n" "NO contexts with valid fem info were found on specified filters!! %s%s%s%s\r\n\n", EMPTY,
                   EMPTY, EMPTY, EMPTY);
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   This function displays conditions per filter (by fem and fem_program) as specified by the caller.
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
diag_dnx_field_fem_info_conditions_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_fem_id_t fem_id_lower, fem_id_upper;
    dnx_field_context_t fem_program_lower, fem_program_upper;
    int non_empty_only, valid_condition_found, do_print;
    dnx_field_fem_id_t fem_id_index;
    dnx_field_fem_program_t fem_program_index;
    unsigned int num_skips_per_row;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_ID, fem_id_lower, fem_id_upper);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_PROGRAM, fem_program_lower, fem_program_upper);
    SH_SAND_GET_BOOL(DIAG_DNX_FIELD_OPTION_VALID, non_empty_only);
    {
        /*
         * Print introductive header for 'fem conditions' table below.
         */
        PRT_TITLE_SET("");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Remarks - ");
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s",
                     "For table of FEM conditions, below,\nall elements which are marked 'invalid', are meaningless");
        PRT_COMMITX;
    }
    do_print = FALSE;
    /*
     * Since we have a column for 'fem id' and a column for 'fem_program', we skip them for every
     * row assigned for 'condition'
     */
    num_skips_per_row = 2;
    {
        if (non_empty_only)
        {
            PRT_TITLE_SET("%s - HW for fem program: from %d to %d",
                          "Active-only fem_programs", fem_program_lower, fem_program_upper);
        }
        else
        {
            PRT_TITLE_SET("%s - HW for fem program: from %d to %d", "All fem_programs", fem_program_lower,
                          fem_program_upper);
        }
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FEM id");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FEM\nprogram");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Condition");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Valid");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Map\ndata");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action\nindex");
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        for (fem_id_index = fem_id_lower; fem_id_index <= fem_id_upper; fem_id_index++)
        {
            for (fem_program_index = fem_program_lower; fem_program_index <= fem_program_upper; fem_program_index++)
            {
                SHR_IF_ERR_EXIT(appl_dnx_action_hw_fem_is_any_condition_valid
                                (unit, fem_id_index, fem_program_index, &valid_condition_found));
                if (valid_condition_found || !non_empty_only)
                {
                    /*
                     * Do print if:
                     * User required to see all (not just the 'valid' ones)
                     * OR
                     * User required to see only the 'valid' ones and that 'fem program' did
                     * have at least one 'valid' condition.
                     */
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%d", (unsigned int) fem_id_index);
                    PRT_CELL_SET("%d", (unsigned int) fem_program_index);
                    SHR_IF_ERR_EXIT(appl_dnx_action_hw_fem_condition_display
                                    (unit, fem_id_index, fem_program_index, num_skips_per_row, sand_control, prt_ctr));
                    do_print = TRUE;
                }
            }
        }
    }
    if (do_print != FALSE)
    {
        PRT_COMMITX;
    }
    else
    {
        /*
         * If nothing was printed then just clear loaded PRT memory.
         */
        LOG_CLI_EX("\r\n" "NO fem_program with any valid condition were found on specified filters!! %s%s%s%s\r\n\n",
                   EMPTY, EMPTY, EMPTY, EMPTY);
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   This function displays actions per filter (by fem) as specified by the caller.
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
diag_dnx_field_fem_info_actions_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_fem_id_t fem_id_lower, fem_id_upper;
    int non_empty_only, valid_action_found, do_print;
    dnx_field_fem_id_t fem_id_index;
    unsigned int num_skips_per_row;
    uint8 is_any_action_user_defined;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_ID, fem_id_lower, fem_id_upper);
    SH_SAND_GET_BOOL(DIAG_DNX_FIELD_OPTION_VALID, non_empty_only);
    {
        /*
         * Print legend for 'fem actions' table below.
         */
        PRT_TITLE_SET("Legend - For table of FEM actions, below.");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "column name");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "description");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Action index");
        PRT_CELL_SET("%s", "Index of 'action' within all 4 'actions' available per 'fem_id'.");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Fem program *");
        PRT_CELL_SET("%s",
                     "Indication on whether specified fem program has, on at least one of its 'conditions', a pointer to 'action index'.");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Action type value");
        PRT_CELL_SET("%s", "Numerical value of action, as stored on HW (and as specified in the spec.");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Action type string");
        PRT_CELL_SET("%s", "String descriptor of action, as stored on corresponding DBAL enum.");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Bit index");
        PRT_CELL_SET("%s",
                     "Index of the bit, within hardware, for which one of a few descriptors is selected by the user.\n"
                     "Note that there are 4 bits for 'fem_id' 0/1 and 24 bits for the others.");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Bit desc.");
        PRT_CELL_SET("%s",
                     "Indication on how HW is instructed to handle this bit:\n"
                     "'Ix' stands for 'For bit-index, take value from bit x on input 32 bits (x may be 0 - 31)',\n"
                     "'Cx' stands for 'For bit-index, take constant value x as stored by caller (x may be 0 or 1)',\n"
                     "'Dx' stands for 'For bit-index, take value from bit x fon 4-bits map-data (x may be 0 - 3)',\n"
                     "'--' stands for 'For bit-index, descriptor is 'invalid' (E.g., bit5 on fem_id 0).\n");
        PRT_COMMITX;

    }
    do_print = FALSE;
    is_any_action_user_defined = FALSE;
    /*
     * Since we have a column for 'fem id' and a column for 'fem_program', we skip them for every
     * row assigned for 'condition'
     */
    num_skips_per_row = 1;
    {
        dnx_field_fem_program_t fem_program_index, fem_program_max;
        if (non_empty_only)
        {
            PRT_TITLE_SET("%s - HW actions for fem: from %d to %d",
                          "Active-only fem actions", fem_id_lower, fem_id_upper);
        }
        else
        {
            PRT_TITLE_SET("%s - HW actions for fem: from %d to %d", "All fem actions", fem_id_lower, fem_id_upper);
        }
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FEM id");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action index");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action type value");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action type string");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Adder");
        fem_program_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs;
        for (fem_program_index = 0; fem_program_index < fem_program_max; fem_program_index++)
        {
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Fem program %d", fem_program_index);
        }
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Bit index");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Bit desc.");
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        for (fem_id_index = fem_id_lower; fem_id_index <= fem_id_upper; fem_id_index++)
        {
            SHR_IF_ERR_EXIT(appl_dnx_action_hw_fem_is_any_action_on_condition_valid
                            (unit, fem_id_index, &valid_action_found));
            if (valid_action_found || !non_empty_only)
            {
                /*
                 * Do print if:
                 * User required to see all (not just the 'valid' ones)
                 * OR
                 * User required to see only the 'valid' ones and that 'action' did
                 * have at least one 'valid' condition on one 'fem_program'.
                 */
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%d", (unsigned int) fem_id_index);
                SHR_IF_ERR_EXIT(appl_dnx_action_hw_fem_all_action_display
                                (unit, fem_id_index, num_skips_per_row, non_empty_only, sand_control, prt_ctr,
                                 &is_any_action_user_defined));
                do_print = TRUE;
            }
        }
    }
    if (do_print != FALSE)
    {
        PRT_COMMITX;
        if (is_any_action_user_defined)
        {
            LOG_CLI_EX("\r\n"
                       "User defined actions are being used by FEM, even though there is no added value "
                       "to using user defined actions over predefined actions! %s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY,
                       EMPTY);
        }
    }
    else
    {
        /*
         * If nothing was printed then just clear loaded PRT memory.
         */
        LOG_CLI_EX("\r\n"
                   "NO action with any valid condition (on all fem programs) were found on specified filters!! %s%s%s%s\r\n\n",
                   EMPTY, EMPTY, EMPTY, EMPTY);
    }
exit:
    PRT_FREE;
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
 *   List of shell options for 'fem' shell commands (display, ...)
 * \remark
 *   * For now, just passive display of allocated keys and general keys space
 */
sh_sand_cmd_t Sh_dnx_field_fem_info_gen_cmds[] = {
    {"by_fg", diag_dnx_field_fem_info_by_fg_cb, NULL, Field_fem_info_by_fg_options, &Field_fem_info_by_fg_man}
    ,
    {"by_context", diag_dnx_field_fem_info_by_context_cb, NULL, Field_fem_info_by_context_options,
     &Field_fem_info_by_context_man}
    ,
    {"conditions", diag_dnx_field_fem_info_conditions_cb, NULL, Field_fem_info_conditions_options,
     &Field_fem_display_conditions_man}
    ,
    {"actions", diag_dnx_field_fem_info_actions_cb, NULL, Field_fem_info_actions_options,
     &Field_fem_info_actions_man}
    ,

    {NULL}
};

static sh_sand_man_t Sh_dnx_field_fem_info_gen_man = {
    .brief = "Operations related to display of FEMs using various filters",
    .full = NULL
};
/**
 * \brief
 *   List of shell options for 'fem'->''display' shell commands (by_fg, by_context,, ...)
 * \remark
 *   * For now, just passive display of allocated keys and general keys space
 */
sh_sand_cmd_t Sh_dnx_field_fem_base_cmds[] = {
    {"info", NULL, Sh_dnx_field_fem_info_gen_cmds, NULL, &Sh_dnx_field_fem_info_gen_man}
    ,
    {NULL}
};
/*
 * }
 */
