/**
 * \file diag_dnx_field_action.c
 *
 * Diagnostics procedures, for DNX, for 'action' operations.
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
#include <sal/appl/sal.h>

#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_context_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_qualifier_access.h>

#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/tcam/tcam_handler.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/algo/field/algo_field.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include "diag_dnx_field_action.h"
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
 *  'Help' description for action display (shell commands).
 */
static sh_sand_man_t Field_action_user_list_man = {
    .brief = "'id'- displays the basic info about a specific user defined action or range of actions "
             "on a certain stage (filter by action id and field stage)",
    .full = "'id' display for a range of field actions or field stage. Range is specified via 'action'.\r\n"
            "If no 'id' is specified then '0'-'nof_actions-1' is assumed (0-255).\r\n"
            "If only one value is specified for 'id' then this single value range is assumed.\r\n"
            "If no stage is specified then 'IPMF1'-'EPMF' is assumed (0-3).\r\n",
    .synopsis = "[id=<lower value>-<upper value>]"
                "[stage=<<IPMF1 | IPMF2 | ...>-<<IPMF1 | IPMF2 | ...>>]",
    .examples = "id=0-20 stage=IPMF1-IPMF3",
};

static sh_sand_man_t Field_action_predefined_man = {
    .brief = "List the predefined actions that are supported by SDK ",
    .full = "'List the predefined actions that are supported by SDK.\r\n"
            "If no stage is specified then 'IPMF1'-'EPMF' is assumed (0-3).\r\n",
    .synopsis = "[id=<lower value>-<upper value>]"
                "[stage=<<IPMF1 | IPMF2 | ...>-<<IPMF1 | IPMF2 | ...>>]",
    .examples = "stage=IPMF1-IPMF3",
};

static sh_sand_option_t Field_action_user_list_options[] = {
    {DIAG_DNX_FIELD_OPTION_ID,  SAL_FIELD_TYPE_UINT32, "Lowest-highest user defined action ids to get its info",       "MIN-MAX",       (void *)Field_action_enum_table, "MIN-MAX"},
    {DIAG_DNX_FIELD_OPTION_STAGE,  SAL_FIELD_TYPE_UINT32, "Lowest-highest field stage to get action info for",   "STAGE_LOWEST-STAGE_HIGHEST",    (void *)Field_action_diag_stage_enum_table, "STAGE_LOWEST-STAGE_HIGHEST"},
    {NULL}
};

static sh_sand_option_t field_map_action_bcm_options[] = {
    {"name",  SAL_FIELD_TYPE_STR,  "Filter actions by comparing this substring to the name", ""},
    {DIAG_DNX_FIELD_OPTION_STAGE, SAL_FIELD_TYPE_UINT32, "Show only actions for the stage", "STAGE_LOWEST-STAGE_HIGHEST", (void *)Field_action_diag_stage_enum_table, "STAGE_LOWEST-STAGE_HIGHEST"},
    {DIAG_DNX_FIELD_OPTION_SHOW_DESC, SAL_FIELD_TYPE_BOOL, "Show qualifiers description", "true"},
    {DIAG_DNX_FIELD_OPTION_SHOW_SIG, SAL_FIELD_TYPE_BOOL, "Show qualifiers signals", "false"},
    {NULL}
};

static sh_sand_option_t field_map_action_dnx_options[] = {
    {"name",  SAL_FIELD_TYPE_STR,  "Filter actions by comparing this substring to the name", ""},
    {DIAG_DNX_FIELD_OPTION_STAGE, SAL_FIELD_TYPE_UINT32, "Show only actions for the stage", "STAGE_LOWEST-STAGE_HIGHEST", (void *)Field_action_diag_stage_enum_table, "STAGE_LOWEST-STAGE_HIGHEST"},
    {DIAG_DNX_FIELD_OPTION_SHOW_SIG, SAL_FIELD_TYPE_BOOL, "Show qualifiers signals", "false"},
    {NULL}
};

static sh_sand_option_t Field_action_virtualwire_options[] = {
    {DIAG_DNX_FIELD_OPTION_ID,  SAL_FIELD_TYPE_UINT32, "Lowest-highest virtual wire action ids to get its info",       "MIN-MAX",       (void *)Field_action_vw_enum_table, "MIN-MAX"},
    {DIAG_DNX_FIELD_OPTION_STAGE,  SAL_FIELD_TYPE_UINT32, "Lowest-highest field stage to get vw action info for",   "STAGE_LOWEST-STAGE_HIGHEST",    (void *)Field_action_diag_stage_enum_table, "STAGE_LOWEST-STAGE_HIGHEST"},
    {DIAG_DNX_FIELD_OPTION_VW_NAME,  SAL_FIELD_TYPE_STR,  "Filter virtualwire action by comparing this substring to the virtualwire name", ""},
    {DIAG_DNX_FIELD_OPTION_PREDEFINED_NAME,  SAL_FIELD_TYPE_STR,  "Filter virtualwire action by comparing this substring to the predefined action name", ""},
    {NULL}
};


/*
 * }
 */
/* *INDENT-ON* */

static sh_sand_man_t field_map_action_dnx_man = {
    .brief = "Present valid dnx actions and their properties per stage",
    .full = "Present valid dnx actions and their properties per stage",
    .synopsis = "[name=<dnx action substring>] [stage=<IPMF1 | IPMF2 | ...>]\n",
    .examples = "stage=IPMF3\n" "name=snoop\n" "show_sig=true",
};

static shr_error_e
diag_dnx_field_map_action_dnx_print(
    int unit,
    char *match_n,
    dnx_field_stage_e stage,
    uint8 show_sig,
    prt_control_t * prt_ctr)
{
    dnx_field_action_t *dnx_action_list = NULL;
    int dnx_action_nof, i_act;
    CONST char *dnx_action_name;
    unsigned int action_size;
    bcm_field_action_t bcm_action;
    dnx_field_action_t invalid_action_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_map_action_list(unit, stage, &dnx_action_list, &dnx_action_nof));
    if (dnx_action_nof > 0)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_get_invalid_dnx_action(unit, stage, &invalid_action_type));
    }

    for (i_act = 0; i_act < dnx_action_nof; i_act++)
    {
        /** Skip the Invalid action. */
        if (dnx_action_list[i_act] == invalid_action_type)
        {
            continue;
        }
        dnx_action_name = dnx_field_dnx_action_text(unit, dnx_action_list[i_act]);

        if (!ISEMPTY(match_n) && !sal_strcasestr(dnx_action_name, match_n))
            continue;

        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, stage, dnx_action_list[i_act], &action_size));
        SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm(unit, stage, dnx_action_list[i_act], &bcm_action));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", dnx_action_name);
        PRT_CELL_SET("%d", action_size);
        PRT_CELL_SET("%s", dnx_field_dnx_action_text(unit, dnx_action_list[i_act]));
        if (bcm_action != DNX_BCM_ID_INVALID)
        {
            PRT_CELL_SET("%s", dnx_field_bcm_action_text(unit, bcm_action));
        }
        else
        {
            PRT_CELL_SET("N/A");
        }

        if (show_sig == TRUE)
        {
            char *action_signals[DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_ACTION];
            char act_sig_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
            uint32 signal_iter;
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_signals(unit, stage, dnx_action_list[i_act], action_signals));
            for (signal_iter = 0; signal_iter < DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_QUALIFIER; signal_iter++)
            {
                char act_sig_string_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
                /** Break in case we reach an empty string, which is indication the end of signals array. */
                if (action_signals[signal_iter] == 0)
                {
                    break;
                }

                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%s\n", action_signals[signal_iter]);
                sal_snprintf(act_sig_string_buff, sizeof(act_sig_string_buff), "%s\n", action_signals[signal_iter]);
                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(act_sig_string, "%s", act_sig_string_buff);
                sal_strncat_s(act_sig_string, act_sig_string_buff, sizeof(act_sig_string));
            }

            PRT_CELL_SET("%s", act_sig_string);
        }

        PRT_CELL_SET("%s", dnx_field_stage_text(unit, stage));
    }

exit:
    if (dnx_action_list != NULL)
        sal_free(dnx_action_list);
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnx_field_map_action_dnx_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e stage_lower, stage_upper;
    dnx_field_stage_e stage_index;
    char *match_n;
    uint8 show_sig;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("name", match_n);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_STAGE, stage_lower, stage_upper);
    SH_SAND_GET_BOOL(DIAG_DNX_FIELD_OPTION_SHOW_SIG, show_sig);

    PRT_TITLE_SET("Action Map");
    PRT_COLUMN_ADD("Action");
    PRT_COLUMN_ADD("Size");
    PRT_COLUMN_ADD("Action Type");
    PRT_COLUMN_ADD("BCM");
    if (show_sig == TRUE)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Signals");
    }
    PRT_COLUMN_ADD("Stage");

    for (stage_index = stage_lower; stage_index <= stage_upper; stage_index++)
    {
        dnx_field_stage_e field_stage;
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                        (unit, Field_action_diag_stage_enum_table[stage_index].string, &field_stage));

        SHR_IF_ERR_EXIT(diag_dnx_field_map_action_dnx_print(unit, match_n, field_stage, show_sig, prt_ctr));
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t field_map_action_bcm_man = {
    .brief = "Present valid bcm actions and the set of dnx action IDs per each bcm action",
    .full = "Present valid bcm actions and the set of dnx action IDs per each bcm action",
    .synopsis = "[name=<bcm action substring>] [stage=<IPMF1 | IPMF2 | ...>]\n",
    .examples =
        "\n" "name=snoop\n" "stage=STAGE_LOWEST-STAGE_HIGHEST\n" "stage=ace\n" "show_desc=false\n" "show_sig=true",
};

static shr_error_e
diag_dnx_field_map_action_bcm_print(
    int unit,
    char *match_n,
    dnx_field_stage_e stage,
    uint8 show_sig,
    uint8 show_desc,
    prt_control_t * prt_ctr)
{
    bcm_field_action_t bcm_action;
    bcm_field_action_t bcm_action_raw;
    uint32 size;
    dnx_field_action_t dnx_action;
    dnx_field_action_t dnx_action_raw;
    const dnx_field_action_map_t *action_map;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    for (bcm_action = 0; bcm_action < bcmFieldActionCount; bcm_action++)
    {
        const char *bcm_action_n = NULL;

        if (!ISEMPTY(match_n) && !sal_strcasestr(dnx_field_bcm_action_text(unit, bcm_action), match_n))
        {
            continue;
        }
        rv = dnx_field_map_action_bcm_to_dnx_int(unit, stage, FALSE, bcm_action, &dnx_action, &action_map);
        if (rv != _SHR_E_PARAM && rv != _SHR_E_NOT_FOUND)
        {
            SHR_IF_ERR_EXIT(rv);
            bcm_action_n = dnx_field_bcm_action_text(unit, bcm_action);
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, stage, dnx_action, &size));
        }

        if (ISEMPTY(bcm_action_n))
        {
            continue;
        }

        /** We arrived here so for this bcmAction we have at least one dnx action on one of stages. */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", bcm_action_n);
        if (show_desc == TRUE)
        {
            char *action_desc;
            /** Check if there is any description for specific bcmAction. */
            SHR_IF_ERR_EXIT(dnx_field_action_description_get(unit, bcm_action, &action_desc));
            PRT_CELL_SET("%s", action_desc);
        }
        PRT_CELL_SET("%d", size);

        /**
         * In case current action is marked as basic_object, we should present it.
         * Otherwise we should find the basic action for current BCM action.
         */
        if (action_map->flags & BCM_TO_DNX_BASIC_OBJECT)
        {
            PRT_CELL_SET("%s", bcm_action_n);
        }
        else
        {
            uint8 raw_action_found = FALSE;
            for (bcm_action_raw = 0; bcm_action_raw < bcmFieldActionCount; bcm_action_raw++)
            {
                rv = dnx_field_map_action_bcm_to_dnx_int(unit, stage, FALSE, bcm_action_raw, &dnx_action_raw,
                                                         &action_map);
                if (rv == _SHR_E_PARAM || rv == _SHR_E_NOT_FOUND)
                {
                    continue;
                }
                SHR_IF_ERR_EXIT(rv);
                if ((action_map->flags & BCM_TO_DNX_BASIC_OBJECT) && (dnx_action == dnx_action_raw))
                {
                    PRT_CELL_SET("%s", dnx_field_bcm_action_text(unit, bcm_action_raw));
                    raw_action_found = TRUE;
                    break;
                }
            }

            /**
             * If no raw action was found, it means that the current action don't have
             * the flag BCM_TO_DNX_BASIC_OBJECT and in such case we present current BCM action.
             */
            if (!raw_action_found)
            {
                PRT_CELL_SET("%s", bcm_action_n);
            }
        }
        PRT_CELL_SET("%s", dnx_field_dnx_action_text(unit, dnx_action));

        if (show_sig == TRUE)
        {
            char *action_signals[DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_ACTION];
            char act_sig_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
            uint32 signal_iter;
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_signals(unit, stage, dnx_action, action_signals));
            for (signal_iter = 0; signal_iter < DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_QUALIFIER; signal_iter++)
            {
                char act_sig_string_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
                /** Break in case we reach an empty string, which is indication the end of signals array. */
                if (action_signals[signal_iter] == 0)
                {
                    break;
                }

                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%s\n", action_signals[signal_iter]);
                sal_snprintf(act_sig_string_buff, sizeof(act_sig_string_buff), "%s\n", action_signals[signal_iter]);
                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(act_sig_string, "%s", act_sig_string_buff);
                sal_strncat_s(act_sig_string, act_sig_string_buff, sizeof(act_sig_string));
            }

            PRT_CELL_SET("%s", act_sig_string);
        }

        PRT_CELL_SET("%s", dnx_field_stage_text(unit, stage));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnx_field_map_action_bcm_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *match_n;
    dnx_field_stage_e stage_lower, stage_upper;
    dnx_field_stage_e stage_index;
    uint8 show_sig;
    uint8 show_desc;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("name", match_n);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_STAGE, stage_lower, stage_upper);
    SH_SAND_GET_BOOL(DIAG_DNX_FIELD_OPTION_SHOW_SIG, show_sig);
    SH_SAND_GET_BOOL(DIAG_DNX_FIELD_OPTION_SHOW_DESC, show_desc);

    PRT_TITLE_SET("Action Map");

    PRT_COLUMN_ADD("bcmAction");
    if (show_desc == TRUE)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Description");
    }
    PRT_COLUMN_ADD("Size");
    PRT_COLUMN_ADD("Raw");
    PRT_COLUMN_ADD("DNX Action");
    if (show_sig == TRUE)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Signals");
    }
    PRT_COLUMN_ADD("Stage");

    for (stage_index = stage_lower; stage_index <= stage_upper; stage_index++)
    {
        dnx_field_stage_e field_stage;
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                        (unit, Field_action_diag_stage_enum_table[stage_index].string, &field_stage));

        SHR_IF_ERR_EXIT(diag_dnx_field_map_action_bcm_print(unit, match_n, field_stage, show_sig, show_desc, prt_ctr));
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t Field_action_virtualwire_man = {
    .brief = "Present valid virtual wire actions and their properties per stage",
    .full = "Present valid virtual wire actions and their properties per stage",
    .synopsis =
        "[id=<virtualwire action id>] [stage=<IPMF1 | IPMF2 | ...>] [vw_name=<virtualwire name>] [predefined=<predefined action name>]\n",
    .examples = "predefined= vw_name= id=8292-8390 stage=IPMF1\n",
};

static shr_error_e
diag_dnx_field_action_virtualwire_print(
    int unit,
    uint32 action_id_lower,
    uint32 action_id_upper,
    char *vw_match_n,
    char *predef_match_n,
    dnx_field_stage_e stage,
    prt_control_t * prt_ctr)
{
    int bcm_vw_act_id;
    int vw_first = dnx_data_field.action.vw_1st_get(unit);
    int vw_action_offset = 0;

    SHR_FUNC_INIT_VARS(unit);

    for (bcm_vw_act_id = action_id_lower; bcm_vw_act_id <= action_id_upper; bcm_vw_act_id++)
    {
        int vw_act_id = (bcm_vw_act_id - vw_first);
        dnx_field_action_t dnx_vw_action;
        unsigned int action_size;
        const dnx_field_action_map_t *action_map_entry;
        dnx_field_action_t base_dnx_action;
        CONST char *vw_name;
        CONST char *predef_dnx_name;
        shr_error_e rv =
            dnx_field_map_action_bcm_to_dnx_int(unit, stage, FALSE, bcm_vw_act_id, &dnx_vw_action, &action_map_entry);
        if (rv == _SHR_E_PARAM || rv == _SHR_E_NOT_FOUND)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(rv);
        /** Get base DNX action of the current VW action. */
        base_dnx_action =
            dnx_per_vw_action_info[dnx_vw_action_info[vw_act_id].vw_index].
            base_dnx_action_per_stage[dnx_vw_action_info[vw_act_id].action_index_within_vw][stage];

        /** Get the action size. */
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, stage, dnx_vw_action, &action_size));

        /** Get the VW name and compare it to the given one if any. */
        vw_name = dnx_field_dnx_action_text(unit, dnx_vw_action);
        if (!ISEMPTY(vw_match_n) && !sal_strcasestr(vw_name, vw_match_n))
        {
            vw_action_offset += action_size;
            continue;
        }

        predef_dnx_name = dnx_field_dnx_action_text(unit, base_dnx_action);
        if (!ISEMPTY(predef_match_n) && !sal_strcasestr(predef_dnx_name, predef_match_n))
        {
            vw_action_offset += action_size;
            continue;
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", bcm_vw_act_id);
        PRT_CELL_SET("%s", vw_name);
        PRT_CELL_SET("%d", dnx_vw_action_info[vw_act_id].action_index_within_vw);
        PRT_CELL_SET("%d", vw_action_offset);
        vw_action_offset += action_size;
        PRT_CELL_SET("%d", action_size);
        PRT_CELL_SET("%s", predef_dnx_name);
        PRT_CELL_SET("%s", dnx_field_stage_text(unit, stage));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnx_field_action_virtualwire_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e stage_lower, stage_upper;
    dnx_field_stage_e stage_index;
    char *vw_match_n;
    char *predef_match_n;
    uint32 action_id_lower, action_id_upper;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_STAGE, stage_lower, stage_upper);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_ID, action_id_lower, action_id_upper);
    SH_SAND_GET_STR(DIAG_DNX_FIELD_OPTION_VW_NAME, vw_match_n);
    SH_SAND_GET_STR(DIAG_DNX_FIELD_OPTION_PREDEFINED_NAME, predef_match_n);

    PRT_TITLE_SET("Virtual Wire Action Map");
    PRT_COLUMN_ADD("Action ID");
    PRT_COLUMN_ADD("VW Name");
    PRT_COLUMN_ADD("Action index on VW");
    PRT_COLUMN_ADD("Offset on VW");
    PRT_COLUMN_ADD("Size");
    PRT_COLUMN_ADD("Predefined DNX Action");
    PRT_COLUMN_ADD("Stage");

    for (stage_index = stage_lower; stage_index <= stage_upper; stage_index++)
    {
        dnx_field_stage_e field_stage;
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                        (unit, Field_action_diag_stage_enum_table[stage_index].string, &field_stage));

        SHR_IF_ERR_EXIT(diag_dnx_field_action_virtualwire_print(unit, action_id_lower, action_id_upper,
                                                                vw_match_n, predef_match_n, field_stage, prt_ctr));
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays FG IDs in which the specified by the caller Field actions
 *   per action ID or range of IDs (optional) is used.
 * \param [in] unit - The unit number.
 * \param [in] dnx_action_id - DNX ID of the current action.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_action_list_fg_usage_cb(
    int unit,
    dnx_field_action_t dnx_action_id,
    prt_control_t * prt_ctr)
{
    dnx_field_group_full_info_t *fg_info_p = NULL;
    uint8 is_fg_allocated;
    int max_nof_fg, fg_id_index, fg_act_iter;
    uint8 do_row_sep_none_add;
    dnx_field_ace_format_full_info_t *ace_format_id_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(fg_info_p, sizeof(dnx_field_group_full_info_t), "The FG full info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    max_nof_fg = dnx_data_field.group.nof_fgs_get(unit);

    do_row_sep_none_add = FALSE;

    /**
     * Iterate over maximum number of FGs.
     */
    for (fg_id_index = 0; fg_id_index < max_nof_fg; fg_id_index++)
    {
        /** Check if the FG is being allocated. */
        SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id_index, &is_fg_allocated));
        if (!is_fg_allocated)
        {
            /** If FG_ ID is not allocated, move to next FG ID. */
            continue;
        }

        /** Get information about the current Field Group. */
        SHR_IF_ERR_EXIT(dnx_field_group_get(unit, fg_id_index, fg_info_p));

        /** Iterate over all actions in the FG and stop if the action is being found. */
        for (fg_act_iter = 0; fg_act_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP &&
             fg_info_p->group_basic_info.dnx_actions[fg_act_iter] != DNX_FIELD_ACTION_INVALID; fg_act_iter++)
        {
            /** Get information about the current Field action. */
            if ((fg_info_p->group_basic_info.dnx_actions[fg_act_iter] != dnx_action_id))
            {
                continue;
            }

            if (do_row_sep_none_add)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                /**
                 * In case one action is used by more than one FG, we have to skip cells:
                 * Action ID, Name, Stage, Size, Prefix size, Prefix Value, Base action.
                 */
                PRT_CELL_SKIP(7);
            }

            if (sal_strncmp(fg_info_p->group_basic_info.name, "", sizeof(fg_info_p->group_basic_info.name)))
            {
                PRT_CELL_SET("FG: %d / \"%s\"", fg_id_index, fg_info_p->group_basic_info.name);
            }
            else
            {
                PRT_CELL_SET("FG: %d / \"N/A\"", fg_id_index);
            }

            do_row_sep_none_add = TRUE;
            break;
        }
    }

    /** In case there are no FGs, which are using the given action, we will check the ACE formats. */
    if (!do_row_sep_none_add)
    {
        uint8 is_ace_allocated;
        dnx_field_ace_id_t format_id_index;
        int max_num_format_ids;

        SHR_ALLOC(ace_format_id_info, sizeof(dnx_field_ace_format_full_info_t), "ace_format_id_info", "%s%s%s\r\n",
                  EMPTY, EMPTY, EMPTY);
        max_num_format_ids = dnx_data_field.ace.nof_ace_id_get(unit);

        for (format_id_index = 1; format_id_index < max_num_format_ids; format_id_index++)
        {
            /** Check if the specified ACE Format ID or range of ACE Format IDs are allocated. */
            SHR_IF_ERR_EXIT(dnx_algo_field_ace_id_is_allocated(unit, format_id_index, &is_ace_allocated));
            if (!is_ace_allocated)
            {
                continue;
            }
            /** Get info for the allocated ACE Formats. */
            SHR_IF_ERR_EXIT(dnx_field_ace_format_get(unit, format_id_index, ace_format_id_info));

            /** Iterate over all actions in the ACE format and stop if the action is being found. */
            for (fg_act_iter = 0; fg_act_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP &&
                 ace_format_id_info->format_basic_info.dnx_actions[fg_act_iter] != DNX_FIELD_ACTION_INVALID;
                 fg_act_iter++)
            {
                /** Get information about the current Field action. */
                if ((ace_format_id_info->format_basic_info.dnx_actions[fg_act_iter] != dnx_action_id))
                {
                    continue;
                }

                if (do_row_sep_none_add)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    /**
                     * In case one action is used by more than one FG, we have to skip cells:
                     * Action ID, Name, Stage, Size, Prefix size, Prefix Value, Base action.
                     */
                    PRT_CELL_SKIP(7);
                }

                if (sal_strncmp
                    (ace_format_id_info->format_basic_info.name, "",
                     sizeof(ace_format_id_info->format_basic_info.name)))
                {
                    PRT_CELL_SET("ACE: %d / \"%s\"", format_id_index, ace_format_id_info->format_basic_info.name);
                }
                else
                {
                    PRT_CELL_SET("ACE: %d / \"N/A\"", format_id_index);
                }

                do_row_sep_none_add = TRUE;
                break;
            }
        }

    }

exit:
    SHR_FREE(ace_format_id_info);
    SHR_FREE(fg_info_p);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays Field actions per action ID or range of IDs  (optional), specified by the caller.
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
diag_dnx_field_action_user_list_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_field_action_t action_id_lower, action_id_upper;
    dnx_field_stage_e stage_lower, stage_upper;
    dnx_field_stage_e stage_index;
    int do_display;
    dnx_field_action_id_t dnx_action_id;
    dnx_field_user_action_info_t user_action_info;
    dnx_field_stage_e dnx_stage;
    dnx_field_action_t dnx_action;
    bcm_field_action_t action_id;
    bcm_field_action_info_t action_info;
    bcm_field_action_t bcm_action;
    CONST char *dnx_action_name;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_ID, action_id_lower, action_id_upper);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_STAGE, stage_lower, stage_upper);

    PRT_TITLE_SET("FIELD_User_Defined_actions");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Stage");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Prefix size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Prefix Value");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Base action");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FG Id/Name");

    do_display = FALSE;
    for (stage_index = stage_lower; stage_index <= stage_upper; stage_index++)
    {
        dnx_field_stage_e field_stage;
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                        (unit, Field_action_diag_stage_enum_table[stage_index].string, &field_stage));

        for (action_id = (BCM_FIELD_FIRST_USER_ACTION_ID + action_id_lower);
             action_id <= (BCM_FIELD_FIRST_USER_ACTION_ID + action_id_upper); action_id++)
        {
           /**
             * Check if the specified action ID or range of action IDs are valid.
             */
            dnx_action_id = action_id - dnx_data_field.action.user_1st_get(unit);
            SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.get(unit, dnx_action_id, &user_action_info));
            if (user_action_info.valid == FALSE)
            {
                continue;
            }
            /** Get information about the current Field action. */
            SHR_IF_ERR_EXIT(bcm_field_action_info_get(unit, action_id, bcmFieldStageCount, &action_info));
            SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, action_info.stage, &dnx_stage));
            SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm
                            (unit, dnx_stage, user_action_info.base_dnx_action, &bcm_action));
            dnx_action_name = dnx_field_bcm_action_text(unit, bcm_action);
            if (field_stage != dnx_stage)
            {
                continue;
            }

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            PRT_CELL_SET("%d", dnx_action_id);
           /**
             * Check if Field action name is being provided, in case no set N/A as default value,
             * in other case set the provided name.
             */
            if (sal_strncmp((char *) (action_info.name), "", sizeof(action_info.name)))
            {
                PRT_CELL_SET("%s", action_info.name);
            }
            else
            {
                PRT_CELL_SET("N/A");
            }

            PRT_CELL_SET("%s", dnx_field_bcm_stage_text(action_info.stage));
            PRT_CELL_SET("%d", action_info.size);
            PRT_CELL_SET("%d", action_info.prefix_size);
            PRT_CELL_SET("0x%x", action_info.prefix_value);
            PRT_CELL_SET("%s", dnx_action_name);

            SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx(unit, dnx_stage, action_id, &dnx_action));
            SHR_IF_ERR_EXIT(diag_dnx_field_action_list_fg_usage_cb(unit, dnx_action, prt_ctr));

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
        LOG_CLI_EX("\r\n" "NO allocated field actions were found on specified filters!! %s%s%s%s\r\n\n", EMPTY,
                   EMPTY, EMPTY, EMPTY);
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
 *   List of shell options for 'action' shell commands (list, info)
 */

static sh_sand_cmd_t sh_dnx_field_action_predefined_cmds[] = {
    {"BCM", diag_dnx_field_map_action_bcm_cb, NULL, field_map_action_bcm_options, &field_map_action_bcm_man},
    {"DNX", diag_dnx_field_map_action_dnx_cb, NULL, field_map_action_dnx_options, &field_map_action_dnx_man},
    {NULL}
};

sh_sand_cmd_t Sh_dnx_field_action_cmds[] = {
    {"PReDefined", NULL, sh_dnx_field_action_predefined_cmds, NULL, &Field_action_predefined_man},
    {"USeR", diag_dnx_field_action_user_list_cb, NULL, Field_action_user_list_options, &Field_action_user_list_man},
    {"VirtualWire", diag_dnx_field_action_virtualwire_cb, NULL, Field_action_virtualwire_options,
     &Field_action_virtualwire_man},
    {NULL}
};

/*
 * }
 */
