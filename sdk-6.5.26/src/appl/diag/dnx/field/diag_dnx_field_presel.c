/** \file diag_dnx_field_presel.c
 *
 * Diagnostics procedures, for DNX, for 'presel' operations.
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

#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDDIAGSDNX
/*
 * Include files.
 * {
 */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "diag_dnx_field_presel.h"
#include <bcm_int/dnx/field/field_group.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_context_access.h>
#include <soc/dnx/field/tcam/tcam_handler.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/field/field_entry.h>
#include "diag_dnx_field_utils.h"
#include <src/bcm/dnx/field/map/field_map_local.h>

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

static shr_error_e
diag_dnx_field_presel_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(diag_dnx_field_generic_init(unit, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Global and Static
 * {
 */

/* *INDENT-OFF* */
/**
 *  'Help' description for presel list (shell commands).
 */
static sh_sand_man_t Field_presel_list_man = {
    .brief = "Display preselectors.",
    .full = "A list of all preselectors configured, with a table for each field stage.\r\n",
    .synopsis = "[presel=<<Lowest presel Id>-<Highest presel Id type>>]"
                "[stage=<<IPMF1 | IPMF2 | ...>-<<IPMF1 | IPMF2 | ...>>]",
    .examples = ""
                "\n" "stage=iPMF2 presel=127",
    .init_cb =  diag_dnx_field_presel_init,
};

static sh_sand_option_t Field_presel_list_options[] = {
    {DIAG_DNX_FIELD_OPTION_PRESEL_ID,  SAL_FIELD_TYPE_UINT32, "Lowest-highest presel ID to get its info",     "0xFFFFFFFF"},
    {DIAG_DNX_FIELD_OPTION_STAGE,      SAL_FIELD_TYPE_UINT32, "Lowest-highest field stage to get info for",   "STAGE_LOWEST-STAGE_HIGHEST",    (void *)Field_presel_diag_stage_enum_table, "STAGE_LOWEST-STAGE_HIGHEST"},
    {NULL}
};

/*
 * }
 */
/* *INDENT-ON* */

/**
 * \brief
 *   This function prints presel qualifer information
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage of the presel.
 * \param [in] qual_data - Presel qualifier data.
 * \param [out] presel_qual_arg_p - Output string of the presel qualifier argument.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_presel_info_print_qual_arg_string_get(
    int unit,
    dnx_field_stage_e field_stage,
    bcm_field_presel_qualify_data_t qual_data,
    char *presel_qual_arg_p)
{
    const dnx_field_cs_qual_map_t *cs_qual_map_p;
    uint32 nof_layer_records;

    SHR_FUNC_INIT_VARS(unit);

    cs_qual_map_p = &dnx_field_map_stage_info[field_stage].cs_qual_map[qual_data.qual_type];

    nof_layer_records = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_layer_records;

    if (cs_qual_map_p->qual_type_bcm_to_dnx_conversion_cb != NULL)
    {
        sal_snprintf(presel_qual_arg_p, DIAG_DNX_FIELD_UTILS_STR_SIZE, "%d", qual_data.qual_arg);
    }
    else if (cs_qual_map_p->nof == 1 || cs_qual_map_p->index_shift == 0)
    {
        sal_strncpy_s(presel_qual_arg_p, "N/A", DIAG_DNX_FIELD_UTILS_STR_SIZE);
    }
    else
    {
        if (qual_data.qual_type == bcmFieldQualifyForwardingType ||
            qual_data.qual_type == bcmFieldQualifyForwardingTypeRaw ||
            qual_data.qual_type == bcmFieldQualifyForwardingLayerQualifier)
        {
            if (qual_data.qual_arg == 0)
            {
                sal_strncpy_s(presel_qual_arg_p, "0", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            }
            else if (qual_data.qual_arg > 0)
            {
                sal_snprintf(presel_qual_arg_p, DIAG_DNX_FIELD_UTILS_STR_SIZE, "%d/%d",
                             qual_data.qual_arg, (qual_data.qual_arg - nof_layer_records));
            }
            else
            {
                sal_snprintf(presel_qual_arg_p, DIAG_DNX_FIELD_UTILS_STR_SIZE, "%d/%d",
                             qual_data.qual_arg, (nof_layer_records + qual_data.qual_arg));
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Given qualifier %s is not supported! Please update current function!\r\n",
                         dnx_field_bcm_qual_text(unit, qual_data.qual_type));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See h file.
 */
shr_error_e
diag_dnx_field_presel_string(
    int unit,
    dnx_field_stage_e field_stage,
    bcm_field_presel_t presel_id,
    const char *delineator_string,
    char presel_quals_string_out[DIAG_DNX_FIELD_UTILS_INPUT_STR_SIZE])
{
    char presel_quals_string[DIAG_DNX_FIELD_UTILS_INPUT_STR_SIZE] = { 0 };
    int presel_qual_index;
    bcm_field_stage_t bcm_stage;
    bcm_field_presel_entry_id_t entry_id;
    bcm_field_presel_entry_data_t entry_data;
    char presel_qual_buff[DIAG_DNX_FIELD_UTILS_INPUT_STR_SIZE] = { 0 };
    char presel_qual_arg[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    sal_strncpy_s(presel_quals_string, "", sizeof(presel_quals_string));

    SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage));

    bcm_field_presel_entry_id_info_init(&entry_id);
    bcm_field_presel_entry_data_info_init(&entry_data);
    entry_id.presel_id = presel_id;
    entry_id.stage = bcm_stage;
    SHR_IF_ERR_EXIT(bcm_field_presel_get(unit, 0, &entry_id, &entry_data));
    /** Sanity check */
    if (entry_data.entry_valid == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid presel %d in stage %d.\r\n", entry_id.presel_id, entry_id.stage);
    }

    /**
     * Construct a string, which contains information (name/value) about
     * all preselector qualifiers.
     */
    for (presel_qual_index = 0; presel_qual_index < entry_data.nof_qualifiers; presel_qual_index++)
    {
        SHR_IF_ERR_EXIT(diag_dnx_field_presel_info_print_qual_arg_string_get
                        (unit, field_stage, entry_data.qual_data[presel_qual_index], presel_qual_arg));
        /**
         * In case of bcmFieldQualifyForwardingType qualifier we will print the name of the FWD type value.
         * Otherwise print the value itself.
         */
        if (entry_data.qual_data[presel_qual_index].qual_type == bcmFieldQualifyForwardingType)
        {
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                                  "%s(%s, 0x%X, %s)",
                                                  dnx_field_bcm_qual_text(unit, entry_data.qual_data
                                                                          [presel_qual_index].qual_type),
                                                  dnx_field_bcm_layer_type_text(entry_data.qual_data
                                                                                [presel_qual_index].qual_value),
                                                  entry_data.qual_data[presel_qual_index].qual_mask, presel_qual_arg);
            sal_snprintf(presel_qual_buff, sizeof(presel_qual_buff), "%s(%s, 0x%X, %s)",
                         dnx_field_bcm_qual_text(unit, entry_data.qual_data[presel_qual_index].qual_type),
                         dnx_field_bcm_layer_type_text(entry_data.qual_data[presel_qual_index].qual_value),
                         entry_data.qual_data[presel_qual_index].qual_mask, presel_qual_arg);
        }
        else
        {
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                                  "%s(%d, 0x%X, %s)",
                                                  dnx_field_bcm_qual_text(unit, entry_data.qual_data
                                                                          [presel_qual_index].qual_type),
                                                  entry_data.qual_data[presel_qual_index].qual_value,
                                                  entry_data.qual_data[presel_qual_index].qual_mask, presel_qual_arg);
            sal_snprintf(presel_qual_buff, sizeof(presel_qual_buff), "%s(%d, 0x%X, %s)",
                         dnx_field_bcm_qual_text(unit, entry_data.qual_data[presel_qual_index].qual_type),
                         entry_data.qual_data[presel_qual_index].qual_value,
                         entry_data.qual_data[presel_qual_index].qual_mask, presel_qual_arg);
        }
        if (sal_strncmp(presel_quals_string, "", sizeof(presel_quals_string)))
        {
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(presel_quals_string, "%s", delineator_string);
            sal_strncat_s(presel_quals_string, delineator_string, sizeof(presel_quals_string));
        }
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(presel_quals_string, "%s", presel_qual_buff);
        sal_strncat_s(presel_quals_string, presel_qual_buff, sizeof(presel_quals_string));
    }

    if (entry_data.nof_qualifiers == 0)
    {
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(presel_quals_string, "%s", "Always Hit");
        sal_strncat_s(presel_quals_string, "Always Hit", sizeof(presel_quals_string));
    }

    sal_strncpy_s(presel_quals_string_out, presel_quals_string,
                  sizeof(presel_quals_string_out[0]) * DIAG_DNX_FIELD_UTILS_INPUT_STR_SIZE);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays preselectors.
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
diag_dnx_field_presel_list_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    dnx_field_stage_e stage_lower, stage_upper;
    int stage_index;
    bcm_field_stage_t bcm_stage;
    int presel_id_lower_in, presel_id_upper_in;
    dnx_field_presel_t presel_id_lower, presel_id_upper;
    bcm_field_presel_t presel_id_index;
    bcm_field_presel_entry_id_t entry_id;
    bcm_field_presel_entry_data_t entry_data;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_PRESEL_ID, presel_id_lower_in, presel_id_upper_in);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_STAGE, stage_lower, stage_upper);

    if ((presel_id_lower_in == 0xFFFFFFFF) && (presel_id_upper_in == 0xFFFFFFFF))
    {
        presel_id_lower = 0;
        presel_id_upper = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CS_LINES;
    }
    else
    {
        if ((presel_id_lower_in < 0) || (presel_id_lower_in >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CS_LINES))
        {
            LOG_CLI_EX("\r\n" "Value:\"%d\" for option:\"presel\" is out of range:%d-%d!%s\r\n\n",
                       presel_id_lower_in, 0, (DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CS_LINES - 1), EMPTY);
            SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
            SHR_EXIT();
        }
        if ((presel_id_upper_in < 0) || (presel_id_upper_in >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CS_LINES))
        {
            LOG_CLI_EX("\r\n" "Value:\"%d\" for option:\"presel\" is out of range:%d-%d!%s\r\n\n",
                       presel_id_upper_in, 0, (DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CS_LINES - 1), EMPTY);
            SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
            SHR_EXIT();
        }
        presel_id_lower = presel_id_lower_in;
        presel_id_upper = presel_id_upper_in;
    }

    for (stage_index = stage_lower; stage_index <= stage_upper; stage_index++)
    {
        uint8 do_display = FALSE;
        int max_nof_cs_lines;
        dnx_field_stage_e field_stage;
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                        (unit, Field_presel_diag_stage_enum_table[stage_index].string, &field_stage));
        SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage));

        max_nof_cs_lines = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_cs_lines;
        /** Sanity check.*/
        if (max_nof_cs_lines <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Stage  %d has no cs lines (%d).\r\n", field_stage, max_nof_cs_lines);
        }

        for (presel_id_index = presel_id_lower;
             (presel_id_index <= presel_id_upper) && (presel_id_index < max_nof_cs_lines); presel_id_index++)
        {
            char presel_quals_string[DIAG_DNX_FIELD_UTILS_INPUT_STR_SIZE] = { 0 };
            entry_id.presel_id = presel_id_index;
            entry_id.stage = bcm_stage;
            SHR_IF_ERR_EXIT(bcm_field_presel_get(unit, 0, &entry_id, &entry_data));
            if (entry_data.entry_valid == FALSE)
            {
                continue;
            }
            if (do_display == FALSE)
            {
                PRT_TITLE_SET("Stage %s", dnx_field_stage_text(unit, field_stage));
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Presel ID");
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Context ID");
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qualifier info");
                if (field_stage == DNX_FIELD_STAGE_IPMF2)
                {
                    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Cascaded from");
                }
                do_display = TRUE;
            }
            SHR_IF_ERR_EXIT(diag_dnx_field_presel_string
                            (unit, field_stage, presel_id_index, "\n", presel_quals_string));
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            PRT_CELL_SET("%d", presel_id_index);
            PRT_CELL_SET("%d", entry_data.context_id);
            PRT_CELL_SET("%s", presel_quals_string);
            if (field_stage == DNX_FIELD_STAGE_IPMF2)
            {
                dnx_field_context_mode_t context_mode;
                SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));
                SHR_IF_ERR_EXIT(dnx_field_context_mode_get(unit, field_stage, entry_data.context_id, &context_mode));
                PRT_CELL_SET("%d", context_mode.context_ipmf2_mode.cascaded_from);
            }
        }
        if (do_display)
        {
            PRT_COMMITX;
        }
        else
        {
            LOG_CLI_EX("\r\n" "No presels for stage %s.%s%s%s\r\n\n", dnx_field_stage_text(unit, field_stage),
                       EMPTY, EMPTY, EMPTY);
        }
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
 *   List of shell options for 'presel' shell commands (list)
 */
sh_sand_cmd_t Sh_dnx_field_presel_cmds[] = {
    {"list", diag_dnx_field_presel_list_cb, NULL, Field_presel_list_options, &Field_presel_list_man}
    ,
    {NULL}
};

/*
 * }
 */
