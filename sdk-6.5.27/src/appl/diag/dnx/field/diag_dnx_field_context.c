/** \file diag_dnx_field_context.c
 *
 * Diagnostics procedures, for DNX, for 'context' operations.
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
#include "diag_dnx_field_context.h"
#include "diag_dnx_field_presel.h"
#include <bcm_int/dnx/field/field_context.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_group_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_context_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_qualifier_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_field_key_alloc_access.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include "diag_dnx_field_utils.h"
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

#if defined(INCLUDE_KBP)
#include <bcm_int/dnx/field/field_kbp.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#endif

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
/*
 * Global and Static
 * {
 */

static shr_error_e
diag_dnx_field_context_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(diag_dnx_field_generic_init(unit, FALSE));

    /*
     * When KBP is available (and adapter compilations with KBP pretend that it does)
     * KBP information will not be printed until device lock.
     */
    {
        int external_tcam_exists;
        SHR_IF_ERR_EXIT(dnx_field_map_is_external_tcam_available(unit, &external_tcam_exists));
        if (external_tcam_exists)
        {
            int device_locked;
            SHR_IF_ERR_EXIT(dnx_field_map_is_device_locked(unit, &device_locked));
            if (device_locked == FALSE)
            {
                /*
                 * Successful device lock cannot be rolled back so we suppress error recovery used for deinit.
                 */
                DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchExternalTcamSync, 0));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/**
 *  'Help' description for Context display (shell commands).
 */
static sh_sand_man_t Field_context_list_man = {
    .brief = "'Context'- displays the basic info about a specific context or range of contexts "
             "on a certain stage (filter by field context id and field stage)",
    .full = "'Context' display for a range of contexts or field stages. Range is specified via 'context/stage'.\r\n"
            "If no 'context' is specified then '0'-'DNX_DATA.field.common_max_val.nof_contexts-1' is assumed (0-63).\r\n"
            "If only one value is specified for context then this 'single value' range is assumed.\r\n"
            "If no stage is specified then 'IPMF1'-'EPMF' is assumed (0-3).\r\n",
    .synopsis = "[context=<lower value>-<upper value>]"
                "[stage=<<IPMF1 | IPMF2 | ...>-<<IPMF1 | IPMF2 | ...>>]",
    .examples = ""
                "\n" "context=0-20 stage=IPMF1-IPMF3",
    .init_cb = diag_dnx_field_context_init,
};
static sh_sand_option_t Field_context_list_options[] = {
    {DIAG_DNX_FIELD_OPTION_CONTEXT,  SAL_FIELD_TYPE_UINT32, "Lowest-highest field context to get its info",     "0xFFFFFFFF"},
    {DIAG_DNX_FIELD_OPTION_STAGE,  SAL_FIELD_TYPE_UINT32, "Lowest-highest field stage to context key info for",   "STAGE_LOWEST-STAGE_HIGHEST",    (void *)Field_context_diag_stage_enum_table, "STAGE_LOWEST-STAGE_HIGHEST"},
    {NULL}
};

/**
 *  'Help' description for Context display (shell commands).
 */
static sh_sand_man_t Field_context_info_man = {
    .brief = "'Context'- displays the full info about a specific context on a specific field stage (filter by context and stage)",
    .full = "'Context' display for a context on a specific stage. \r\n"
      "If no 'context' is specified then an error will occur.\r\n"
      "Input parameters 'context' and 'stage' are mandatory.\r\n"
      "Option 'opcode' is supported only for External stage.",
    .synopsis = "[context=<0-63>] [stage=<IPMF1 | IPMF2 | IPMF3 | EPMF>] [opcode=<0-63>]",
    .examples = "context=15 stage=ipmf1"
                "\n" "context=12 stage=epmf"
                "\n" "context=7 stage=external"
                "\n" "opcode=3 stage=external",
    .init_cb = diag_dnx_field_context_init,

};
static sh_sand_option_t Field_context_info_options[] = {
    {DIAG_DNX_FIELD_OPTION_CONTEXT,  SAL_FIELD_TYPE_UINT32, "Lowest-highest field context to get its info",     "0xFFFFFFFF"},
    {DIAG_DNX_FIELD_OPTION_OPCODE,  SAL_FIELD_TYPE_UINT32, "Opcode Id, only for External stage",                "0xFFFFFFFF"   , NULL},
    {DIAG_DNX_FIELD_OPTION_STAGE,  SAL_FIELD_TYPE_ENUM, "Lowest-highest field stage to get context info for",   NULL,    (void *)Field_context_diag_stage_enum_table},
    {NULL}
};

/*
 * }
 */
/* *INDENT-ON* */

/**
 * \brief
 *   This function prepares and sets information about the given context.
 *   Like ID, stage, name, presel ids and quals, and context modes.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage on which context has been allocated.
 * \param [in] context_id - Id of the current context for which info will be presented.
 * \param [in] print_presel_info - if TRUE print presel info.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 * \param [out] context_mode_p - Pointer, which contains
 *  all context mode information or IPMF1 and IPMF2 context modes.
 * \param [out] apptype_found_p - Indication if the apptype was found or not,
 *  depends on the given opcode_id (context_id). In case apptype was found
 *  output will be TRUE, otherwise FALSE. Relevant only for External stage.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_context_info_print(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    int print_presel_info,
    prt_control_t * prt_ctr,
    dnx_field_context_mode_t * context_mode_p,
    uint8 *apptype_found_p)
{
    bcm_field_presel_t presel_index;
    int presel_list_index;
    uint32 max_nof_cs_lines;
    char presel_quals_string[DIAG_DNX_FIELD_UTILS_INPUT_STR_SIZE] = { 0 };
    char presel_ids_string[DIAG_DNX_FIELD_UTILS_INPUT_STR_SIZE] = { 0 };
    int nof_presel_ids = 0;
    bcm_field_presel_t presel_ids[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CS_LINES];
    char context_name[DBAL_MAX_STRING_LENGTH];
    char mode_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    uint8 is_apptype_static = FALSE;
    dnx_field_context_apptype_info_t apptype_info;
    bcm_field_context_t bcm_context_id;
    bcm_field_stage_t bcm_stage;
    bcm_field_presel_entry_id_t entry_id;
    bcm_field_presel_entry_data_t entry_data;
    char context_id_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /** Get max number of context selection lines. */
    max_nof_cs_lines = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_cs_lines;

    /** Sanity check.*/
    if (max_nof_cs_lines <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Stage %d has no context selection lines (%d).\r\n",
                     field_stage, max_nof_cs_lines);
    }

    /** Take the name for current context from the SW state. */
    sal_memset(context_name, 0, sizeof(context_name));
    SHR_IF_ERR_EXIT(dnx_field_context_sw.context_info.name.
                    value.stringget(unit, context_id, field_stage, context_name));

    /** Convert DNX to BCM Field Stage. */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage));

    if (field_stage == DNX_FIELD_STAGE_EXTERNAL)
    {
        shr_error_e rv = _SHR_E_NONE;
        bcm_field_AppType_t apptype;
        uint32 opcode_id;
        opcode_id = context_id;
        rv = dnx_field_map_opcode_to_apptype_dnx_to_bcm(unit, opcode_id, &apptype);
        if (rv == _SHR_E_NOT_FOUND)
        {
            *apptype_found_p = FALSE;
            SHR_EXIT();
        }
        *apptype_found_p = TRUE;
        /** Initialize the apptype structure. */
        SHR_IF_ERR_EXIT(dnx_field_context_apptype_info_t_init(unit, &apptype_info));
        SHR_IF_ERR_EXIT(dnx_field_context_apptype_info_get(unit, apptype, &apptype_info));
        if (apptype_info.base_static_apptype == DNX_FIELD_APPTYPE_INVALID)
        {
            is_apptype_static = TRUE;
        }
        sal_strncpy_s(context_name, apptype_info.name, sizeof(context_name));
        sal_snprintf(context_id_string, sizeof(context_id_string), "%d (AT)", apptype);
        bcm_context_id = apptype;
    }
    else
    {
        sal_snprintf(context_id_string, sizeof(context_id_string), "%d", context_id);
        bcm_context_id = context_id;
    }

    /** Get the preselectors, which has being set to the current context. */
    bcm_field_presel_entry_id_info_init(&entry_id);
    bcm_field_presel_entry_data_info_init(&entry_data);
    for (presel_index = 0; presel_index < max_nof_cs_lines; presel_index++)
    {
        entry_id.presel_id = presel_index;
        entry_id.stage = bcm_stage;
        /** Retrieve information about the current presel_id. */
        SHR_IF_ERR_EXIT(bcm_field_presel_get(unit, 0, &entry_id, &entry_data));
        /**
         * Check if the presel entry is valid for the current context.
         * In case no, continue iterating over all preselectors.
         */
        if (!((entry_data.entry_valid) && (entry_data.context_id == bcm_context_id)))
        {
            continue;
        }

        presel_ids[nof_presel_ids] = presel_index;
        nof_presel_ids++;
    }

    sal_strncpy_s(presel_quals_string, "", sizeof(presel_quals_string));

    for (presel_list_index = 0; presel_list_index < nof_presel_ids; presel_list_index++)
    {
        char presel_id_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
        if (sal_strncmp(presel_ids_string, "", sizeof(presel_ids_string)))
        {
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(presel_ids_string, "%s", ", ");
            sal_strncat_s(presel_ids_string, ", ", sizeof(presel_ids_string));
        }
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%d", presel_ids[presel_list_index]);
        sal_snprintf(presel_id_buff, sizeof(presel_id_buff), "%d", presel_ids[presel_list_index]);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(presel_ids_string, "%s", presel_id_buff);
        sal_strncat_s(presel_ids_string, presel_id_buff, sizeof(presel_ids_string));
    }

    /** Set first presel.*/
    if (nof_presel_ids > 0)
    {
        char presel_id_buff_for_list[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
        char presel_quals_string_for_list[DIAG_DNX_FIELD_UTILS_INPUT_STR_SIZE] = { 0 };
        presel_list_index = 0;

        SHR_IF_ERR_EXIT(diag_dnx_field_presel_string
                        (unit, field_stage, presel_ids[presel_list_index], "\n ", presel_quals_string_for_list));

        sal_snprintf(presel_id_buff_for_list, sizeof(presel_id_buff_for_list), "%d: ", presel_ids[presel_list_index]);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(presel_quals_string, "%s%s", presel_id_buff_for_list,
                                              presel_quals_string_for_list);
        sal_strncat_s(presel_quals_string, presel_id_buff_for_list, sizeof(presel_quals_string));
        sal_strncat_s(presel_quals_string, presel_quals_string_for_list, sizeof(presel_quals_string));
    }
    else
    {
        sal_strncpy_s(presel_quals_string, "-", sizeof(presel_quals_string));
    }
    /** Sanity check*/
    if (sal_strncmp(presel_quals_string, "", sizeof(presel_quals_string)) == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Presel info not filled.\r\n");
    }

    /** Only add underscore if it is the last line.*/
    if ((nof_presel_ids > 1) && print_presel_info)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    }
    else
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    }

    PRT_CELL_SET("%s", context_id_string);
    PRT_CELL_SET("%s", dnx_field_bcm_stage_text(bcm_stage));

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

    if (sal_strncmp(presel_ids_string, "", sizeof(presel_ids_string)))
    {
        PRT_CELL_SET("%s", presel_ids_string);
    }
    else
    {
        if (field_stage == DNX_FIELD_STAGE_IPMF2)
        {
            PRT_CELL_SET("%s", "No hit - iPMF1 ctx chosen!");
        }
        else if (field_stage == DNX_FIELD_STAGE_EXTERNAL)
        {
            PRT_CELL_SET("%s", "No hit - FWD ctx chosen!");
        }
        else
        {
            PRT_CELL_SET("%s", "No Presel (unreachable context)!");
        }
    }

    if (print_presel_info)
    {
        PRT_CELL_SET("%s", presel_quals_string);
    }

    if (field_stage != DNX_FIELD_STAGE_IPMF3 && field_stage != DNX_FIELD_STAGE_EPMF &&
        field_stage != DNX_FIELD_STAGE_EXTERNAL)
    {
        SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, context_mode_p));
        /** If the current context is allocated, get the information about it. */
        SHR_IF_ERR_EXIT(dnx_field_context_mode_get(unit, field_stage, context_id, context_mode_p));
    }

    /** Prepare and set information about context modes, which have being enabled for the current context. */
    if (field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_context_modes_string_get(unit, *context_mode_p, mode_string));

        if (sal_strncmp(mode_string, "", sizeof(mode_string)))
        {
            PRT_CELL_SET("%s", mode_string);
        }
        else
        {
            PRT_CELL_SET("%s", "N/A");
        }
    }
    else if (field_stage == DNX_FIELD_STAGE_IPMF2
             && (context_mode_p->context_ipmf2_mode.cascaded_from != DNX_FIELD_CONTEXT_ID_INVALID))
    {
        PRT_CELL_SET("Cascading_Ctx %d", context_mode_p->context_ipmf2_mode.cascaded_from);
    }
    else if (field_stage == DNX_FIELD_STAGE_EXTERNAL)
    {
        if (is_apptype_static)
        {
            PRT_CELL_SET("Predefined Apptype");
        }
        else
        {
            PRT_CELL_SET("Cascading %s (%d)", dnx_field_bcm_apptype_text(unit, apptype_info.base_static_apptype),
                         apptype_info.base_static_apptype);
        }
    }
    else
    {
        PRT_CELL_SET("%s", "N/A");
    }

    /** Print other presels.*/
    if (print_presel_info)
    {
        for (presel_list_index = 1; presel_list_index < nof_presel_ids; presel_list_index++)
        {
            char presel_id_buff_for_list[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
            char presel_quals_string_for_list[DIAG_DNX_FIELD_UTILS_INPUT_STR_SIZE] = { 0 };

            if (presel_list_index == nof_presel_ids - 1)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            }
            else
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            }

            PRT_CELL_SKIP(4);

            SHR_IF_ERR_EXIT(diag_dnx_field_presel_string
                            (unit, field_stage, presel_ids[presel_list_index], "\n ", presel_quals_string_for_list));

            sal_strncpy_s(presel_quals_string, "", sizeof(presel_quals_string));
            sal_snprintf(presel_id_buff_for_list, sizeof(presel_id_buff_for_list), "%d: ",
                         presel_ids[presel_list_index]);
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(presel_quals_string, "%s%s", presel_id_buff_for_list,
                                                  presel_quals_string_for_list);
            sal_strncat_s(presel_quals_string, presel_id_buff_for_list, sizeof(presel_quals_string));
            sal_strncat_s(presel_quals_string, presel_quals_string_for_list, sizeof(presel_quals_string));
            PRT_CELL_SET("%s", presel_quals_string);
            PRT_CELL_SKIP(1);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays Contexts per context ID or range of IDs and stage (optional), specified by the caller.
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
diag_dnx_field_context_list_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 is_allocated;
    dnx_field_context_t ctx_id_index;
    int stage_lower, stage_upper;
    int stage_index;
    dnx_field_context_t ctx_id_lower_in, ctx_id_upper_in;
    dnx_field_context_t ctx_id_lower, ctx_id_upper;
    dnx_field_context_mode_t context_mode;
    int do_display;
    uint32 opcode_id;
    uint8 apptype_found;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_CONTEXT, ctx_id_lower_in, ctx_id_upper_in);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_STAGE, stage_lower, stage_upper);

    PRT_TITLE_SET("CONTEXT");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "CTX ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Stage");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Presel IDs");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Presel(Value, Mask, Arg)");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Mode");

    do_display = FALSE;
    ctx_id_lower = ctx_id_upper = 0;

    for (stage_index = stage_lower; stage_index <= stage_upper; stage_index++)
    {
        uint8 skip_print = FALSE;
        dnx_field_stage_e field_stage;
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                        (unit, Field_context_diag_stage_enum_table[stage_index].string, &field_stage));
        if (field_stage == DNX_FIELD_STAGE_EXTERNAL)
        {
            bcm_field_AppType_t apptype;
            dnx_field_context_t ctx_id_internal;
            uint8 is_standard_image;

            SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_standard_image));

            if (((ctx_id_lower_in != 0xFFFFFFFF) && (ctx_id_upper_in != 0xFFFFFFFF)) && (stage_lower == stage_upper) &&
                ((((int) ctx_id_lower_in < 0) || (ctx_id_lower_in >= bcmFieldAppTypeCount)) &&
                 ((ctx_id_lower_in < dnx_data_field.external_tcam.apptype_user_1st_get(unit)) ||
                  (ctx_id_lower_in >=
                   (dnx_data_field.external_tcam.apptype_user_1st_get(unit) +
                    dnx_data_field.external_tcam.apptype_user_nof_get(unit)))))
                && ((((int) ctx_id_upper_in < 0) || (ctx_id_upper_in >= bcmFieldAppTypeCount))
                    && ((ctx_id_upper_in < dnx_data_field.external_tcam.apptype_user_1st_get(unit))
                        || (ctx_id_upper_in >=
                            (dnx_data_field.external_tcam.apptype_user_1st_get(unit) +
                             dnx_data_field.external_tcam.apptype_user_nof_get(unit))))))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "\r\n" "NOTE! Option \"ConTeXt\", in case of External stage, has meaning of APPTYPE!\r\n"
                             "Apptype provided %d-%d out of range. Must be within range 0-%d for predefined apptypes, "
                             "or %d-%d for user defiend apptypes.\r\n\n",
                             ctx_id_lower_in, ctx_id_upper_in, (bcmFieldAppTypeCount - 1),
                             dnx_data_field.external_tcam.apptype_user_1st_get(unit),
                             (dnx_data_field.external_tcam.apptype_user_1st_get(unit) +
                              dnx_data_field.external_tcam.apptype_user_nof_get(unit) - 1));
            }

            /** We should print all of the predefined apptypes, before user defined one. */
            if ((ctx_id_lower_in == 0xFFFFFFFF) && (ctx_id_upper_in == 0xFFFFFFFF))
            {
                ctx_id_lower = 0;
                if (is_standard_image)
                {
                    ctx_id_upper = bcmFieldAppTypeCount - 1;
                }
                else
                {
                    ctx_id_upper = DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF - 1;
                }
            }
            else
            {
                if ((((int) ctx_id_lower_in < 0) || (ctx_id_lower_in >= bcmFieldAppTypeCount)) ||
                    (((int) ctx_id_upper_in < 0) || (ctx_id_upper_in >= bcmFieldAppTypeCount)))
                {
                    skip_print = TRUE;
                }
                else
                {
                    ctx_id_lower = ctx_id_lower_in;
                    ctx_id_upper = ctx_id_upper_in;
                }

                if ((((int) ctx_id_lower_in >= 0) && (ctx_id_upper_in >= bcmFieldAppTypeCount)))
                {
                    ctx_id_upper = bcmFieldAppTypeCount - 1;
                    skip_print = FALSE;
                }
            }

            if (!skip_print)
            {
                for (ctx_id_index = ctx_id_lower; ctx_id_index <= ctx_id_upper; ctx_id_index++)
                {
                    shr_error_e rv;
                    apptype = ctx_id_index;
                    if (((int) apptype < 0) || (apptype >= bcmFieldAppTypeCount))
                    {
                        continue;
                    }
                    rv = dnx_field_map_apptype_to_opcode_bcm_to_dnx_int(unit, apptype, &opcode_id);
                    if (rv == _SHR_E_NOT_FOUND)
                    {
                        continue;
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(rv);
                    }
                    ctx_id_internal = opcode_id;
                    /**
                     * Prepare and print a table, which contains all basic info about the current opcode id on
                     * the given field stage.
                     */
                    SHR_IF_ERR_EXIT(diag_dnx_field_context_info_print
                                    (unit, field_stage, ctx_id_internal, TRUE, prt_ctr, &context_mode, &apptype_found));
                    do_display = TRUE;
                }
            }

            /** Print the user defined apptypes. */
            if ((ctx_id_lower_in == 0xFFFFFFFF) && (ctx_id_upper_in == 0xFFFFFFFF))
            {
                ctx_id_lower = dnx_data_field.external_tcam.apptype_user_1st_get(unit);
                ctx_id_upper =
                    ((dnx_data_field.external_tcam.apptype_user_1st_get(unit) +
                      dnx_data_field.external_tcam.apptype_user_nof_get(unit)) - 1);
            }
            else
            {
                if ((((int) ctx_id_lower_in < dnx_data_field.external_tcam.apptype_user_1st_get(unit)) ||
                     (ctx_id_lower_in >=
                      (dnx_data_field.external_tcam.apptype_user_1st_get(unit) +
                       dnx_data_field.external_tcam.apptype_user_nof_get(unit))))
                    || (((int) ctx_id_upper_in < dnx_data_field.external_tcam.apptype_user_1st_get(unit))
                        || (ctx_id_upper_in >=
                            (dnx_data_field.external_tcam.apptype_user_1st_get(unit) +
                             dnx_data_field.external_tcam.apptype_user_nof_get(unit)))))
                {
                    skip_print = TRUE;
                }
                else
                {
                    ctx_id_lower = ctx_id_lower_in;
                    ctx_id_upper = ctx_id_upper_in;
                }

                if (((ctx_id_lower_in >= dnx_data_field.external_tcam.apptype_user_1st_get(unit))
                     && (ctx_id_upper_in >= bcmFieldAppTypeCount)))
                {
                    ctx_id_upper =
                        ((dnx_data_field.external_tcam.apptype_user_1st_get(unit) +
                          dnx_data_field.external_tcam.apptype_user_nof_get(unit)) - 1);
                    skip_print = FALSE;
                }
            }

            if (!skip_print)
            {
                for (ctx_id_index = ctx_id_lower; ctx_id_index <= ctx_id_upper; ctx_id_index++)
                {
                    apptype = ctx_id_index;
                    SHR_IF_ERR_EXIT(dnx_algo_field_context_apptype_is_allocated(unit, apptype, &is_allocated));
                    if (!is_allocated)
                    {
                        continue;
                    }
                    SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_opcode_bcm_to_dnx(unit, apptype, &opcode_id));
                    ctx_id_internal = opcode_id;
                    /**
                     * Prepare and print a table, which contains all basic info about the current opcode id on
                     * the given field stage.
                     */
                    SHR_IF_ERR_EXIT(diag_dnx_field_context_info_print
                                    (unit, field_stage, ctx_id_internal, TRUE, prt_ctr, &context_mode, &apptype_found));
                    do_display = TRUE;
                }
            }
        }
        else
        {
            if (((ctx_id_lower_in != 0xFFFFFFFF) && (ctx_id_upper_in != 0xFFFFFFFF)) && (stage_lower == stage_upper) &&
                ((((int) ctx_id_lower_in < 0)
                  || (ctx_id_lower_in >= dnx_data_field.common_max_val.nof_contexts_get(unit)))
                 && (((int) ctx_id_upper_in < 0)
                     || (ctx_id_upper_in >= dnx_data_field.common_max_val.nof_contexts_get(unit)))))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n" "Context provided (%d-%d) must be within range 0-%d.\r\n\n",
                             ctx_id_lower_in, ctx_id_upper_in,
                             dnx_data_field.common_max_val.nof_contexts_get(unit) - 1);
            }

            if ((ctx_id_lower_in == 0xFFFFFFFF) && (ctx_id_upper_in == 0xFFFFFFFF))
            {
                ctx_id_lower = 0;
                ctx_id_upper = dnx_data_field.common_max_val.nof_contexts_get(unit) - 1;
            }
            else
            {
                if (((int) ctx_id_lower_in < 0
                     || ctx_id_lower_in >= dnx_data_field.common_max_val.nof_contexts_get(unit)
                     || (int) ctx_id_upper_in < 0
                     || ctx_id_upper_in >= dnx_data_field.common_max_val.nof_contexts_get(unit)))
                {
                    skip_print = TRUE;
                }
                else
                {
                    ctx_id_lower = ctx_id_lower_in;
                    ctx_id_upper = ctx_id_upper_in;
                }

                if ((((int) ctx_id_lower_in >= 0)
                     && (ctx_id_upper_in >= dnx_data_field.common_max_val.nof_contexts_get(unit))))
                {
                    ctx_id_upper = dnx_data_field.common_max_val.nof_contexts_get(unit) - 1;
                    skip_print = FALSE;
                }
            }

            if (!skip_print)
            {
                for (ctx_id_index = ctx_id_lower; ctx_id_index <= ctx_id_upper; ctx_id_index++)
                {

                    /** Check if the specified Context ID is allocated. */
                    SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated(unit, field_stage, ctx_id_index, &is_allocated));
                    if (!is_allocated)
                    {
                        continue;
                    }

                    /**
                     * Prepare and print a table, which contains all basic info about the current context id on
                     * the given field stage.
                     */
                    SHR_IF_ERR_EXIT(diag_dnx_field_context_info_print
                                    (unit, field_stage, ctx_id_index, TRUE, prt_ctr, &context_mode, &apptype_found));

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
        /*
         * If nothing was printed then just clear loaded PRT memory.
         * This could be left to 'exit' but it is clearer here.
         */
        PRT_FREE;
        LOG_CLI_EX("\r\n" "NO allocated Context was found on specified filters!! %s%s%s%s\r\n\n", EMPTY, EMPTY,
                   EMPTY, EMPTY);
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints compare key information about a specific compare pair.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage on which context has been allocated.
 * \param [in] nof_cmp_keys - Number of keys to compare.
 * \param [in] pair_id - Id of the compare Pair.
 * \param [in] context_compare_info_p - Pointer, which contains
 *  all needed context compare related information.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_context_info_cmp_key_print(
    int unit,
    dnx_field_stage_e field_stage,
    int nof_cmp_keys,
    dnx_field_context_compare_pair_e pair_id,
    dnx_field_context_compare_info_t * context_compare_info_p,
    sh_sand_control_t * sand_control)
{
    int cmp_key_id, key_qual_index;
    dnx_field_context_compare_mode_info_t *cmp_pair;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    cmp_pair = NULL;

    if (pair_id == DNX_FIELD_CONTEXT_COMPARE_FIRST_PAIR)
    {
        cmp_pair = &(context_compare_info_p->pair_1);
    }
    else if (pair_id == DNX_FIELD_CONTEXT_COMPARE_SECOND_PAIR)
    {
        cmp_pair = &(context_compare_info_p->pair_2);
    }

    /** Prepare context compare key info. */
    for (cmp_key_id = 0; cmp_key_id < nof_cmp_keys; cmp_key_id++)
    {
        PRT_TITLE_SET("CMP%d Key %d", pair_id, cmp_key_id + 1);
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Size");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Lsb");

        for (key_qual_index = 0; key_qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
             cmp_pair->key_info[cmp_key_id].key_template.key_qual_map[key_qual_index].qual_type !=
             DNX_FIELD_QUAL_TYPE_INVALID; key_qual_index++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

            PRT_CELL_SET("%s", dnx_field_dnx_qual_text
                         (unit, cmp_pair->key_info[cmp_key_id].key_template.key_qual_map[key_qual_index].qual_type));
            PRT_CELL_SET("%d", cmp_pair->key_info[cmp_key_id].key_template.key_qual_map[key_qual_index].size);
            PRT_CELL_SET("%d", cmp_pair->key_info[cmp_key_id].key_template.key_qual_map[key_qual_index].lsb);
        }

        PRT_COMMITX;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints context hash key information.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage on which context has been allocated.
 * \param [in] hashing_info_p - Pointer, which contains
 *  all needed context hashing related information.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_context_info_hash_key_print(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_hashing_info_t * hashing_info_p,
    sh_sand_control_t * sand_control)
{
    int key_qual_index;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("HASH Key");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Lsb");

    /** Prepare context hashing key info. */
    for (key_qual_index = 0; key_qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
         hashing_info_p->key_info.key_template.key_qual_map[key_qual_index].qual_type !=
         DNX_FIELD_QUAL_TYPE_INVALID; key_qual_index++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        PRT_CELL_SET("%s", dnx_field_dnx_qual_text
                     (unit, hashing_info_p->key_info.key_template.key_qual_map[key_qual_index].qual_type));
        PRT_CELL_SET("%d", hashing_info_p->key_info.key_template.key_qual_map[key_qual_index].size);
        PRT_CELL_SET("%d", hashing_info_p->key_info.key_template.key_qual_map[key_qual_index].lsb);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints context hash key information.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage on which context has been allocated.
 * \param [in] context_id - Id of the current context.
 * \param [in] context_mode_p - Pointer, which contains
 *  all context mode information.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_context_info_cmp_hash_key_print(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_context_mode_t * context_mode_p,
    sh_sand_control_t * sand_control)
{
    int nof_cmp_keys;
    dnx_field_context_compare_info_t context_compare_info;
    dnx_field_context_hashing_info_t hashing_info;

    SHR_FUNC_INIT_VARS(unit);

    nof_cmp_keys = 0;

    SHR_IF_ERR_EXIT(dnx_field_context_cmp_info_get(unit, context_id, &context_compare_info));

    /** Prepare and print information about context compare and hashing keys. */
    if (field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        if (context_mode_p->context_ipmf1_mode.compare_mode_1 == DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE)
        {
            nof_cmp_keys = 1;
            if (context_compare_info.pair_1.is_set)
            {
                SHR_IF_ERR_EXIT(diag_dnx_field_context_info_cmp_key_print
                                (unit, field_stage, nof_cmp_keys, DNX_FIELD_CONTEXT_COMPARE_FIRST_PAIR,
                                 &context_compare_info, sand_control));
            }
        }
        else if (context_mode_p->context_ipmf1_mode.compare_mode_1 == DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE)
        {
            nof_cmp_keys = 2;
            if (context_compare_info.pair_1.is_set)
            {
                SHR_IF_ERR_EXIT(diag_dnx_field_context_info_cmp_key_print
                                (unit, field_stage, nof_cmp_keys, DNX_FIELD_CONTEXT_COMPARE_FIRST_PAIR,
                                 &context_compare_info, sand_control));
            }
        }

        if (context_mode_p->context_ipmf1_mode.compare_mode_2 == DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE)
        {
            nof_cmp_keys = 1;
            if (context_compare_info.pair_2.is_set)
            {
                SHR_IF_ERR_EXIT(diag_dnx_field_context_info_cmp_key_print
                                (unit, field_stage, nof_cmp_keys, DNX_FIELD_CONTEXT_COMPARE_SECOND_PAIR,
                                 &context_compare_info, sand_control));
            }
        }
        else if (context_mode_p->context_ipmf1_mode.compare_mode_2 == DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE)
        {
            nof_cmp_keys = 2;
            if (context_compare_info.pair_2.is_set)
            {
                SHR_IF_ERR_EXIT(diag_dnx_field_context_info_cmp_key_print
                                (unit, field_stage, nof_cmp_keys, DNX_FIELD_CONTEXT_COMPARE_SECOND_PAIR,
                                 &context_compare_info, sand_control));
            }
        }

        if (context_mode_p->context_ipmf1_mode.hash_mode == DNX_FIELD_CONTEXT_HASH_MODE_ENABLED)
        {
            SHR_IF_ERR_EXIT(dnx_field_context_hashing_info_get(unit, context_id, &hashing_info));
            SHR_IF_ERR_EXIT(diag_dnx_field_context_info_hash_key_print(unit, field_stage, &hashing_info, sand_control));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Find the list of field groups on a key and the bits they use.
 * \param [in] unit               - Device ID
 * \param [in] field_stage        - stage.
 * \param [in] context_id         - context.
 * \param [in] key_id             - key.
 * \param [out] nof_fgs_found_p   - Number of field group on key.
 * \param [out] fg_ids            - Array with DNX_DATA_MAX_FIELD_GROUP_NOF_FGS + 1 elements.
 *                                  First nof_fgs_found_p are the field group IDs on key.
 *                                  If the field group is DNX_FIELD_GROUP_INVALID, it represents
 *                                  reserves space for state table.
 * \param [out] bit_lsb           - Array with DNX_DATA_MAX_FIELD_GROUP_NOF_FGS + 1 elements.
 *                                  First nof_fgs_found_p are the lsb bit of the corresponding field group.
 * \param [out] bit_msb           - Array with DNX_DATA_MAX_FIELD_GROUP_NOF_FGS + 1 elements.
 *                                  First nof_fgs_found_p are the msb bit of the corresponding field group.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_diag_fgs_on_key(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    int key_id,
    int *nof_fgs_found_p,
    dnx_field_group_t fg_ids[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS + 1],
    int bit_lsb[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS + 1],
    int bit_msb[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS + 1])
{
    int nof_fgs_total = dnx_data_field.group.nof_fgs_get(unit);
    dnx_field_group_t fg_id_iter;
    dnx_field_group_t fg_ids_nosort[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS + 1];
    int bit_lsb_nosort[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS + 1];
    int bit_msb_nosort[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS + 1];
    int nof_fg_ids;
    int state_table_lsb_on_key;
    int state_table_key_size;
    int nof_lookup_fgs = 0;
    int key_has_state_table_reserved = FALSE;
    int state_table_fg_found = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    if (field_stage != DNX_FIELD_STAGE_IPMF1 && field_stage != DNX_FIELD_STAGE_IPMF2 &&
        field_stage != DNX_FIELD_STAGE_IPMF3 && field_stage != DNX_FIELD_STAGE_EPMF)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "field stage %d isn't PMF.\n", field_stage);
    }

    sal_memset(fg_ids, 0x0, sizeof(fg_ids[0]) * (DNX_DATA_MAX_FIELD_GROUP_NOF_FGS + 1));
    sal_memset(bit_lsb, 0x0, sizeof(bit_lsb[0]) * (DNX_DATA_MAX_FIELD_GROUP_NOF_FGS + 1));
    sal_memset(bit_msb, 0x0, sizeof(bit_msb[0]) * (DNX_DATA_MAX_FIELD_GROUP_NOF_FGS + 1));
    sal_memset(fg_ids_nosort, 0x0, sizeof(fg_ids_nosort[0]) * (DNX_DATA_MAX_FIELD_GROUP_NOF_FGS + 1));
    sal_memset(bit_lsb_nosort, 0x0, sizeof(bit_lsb_nosort[0]) * (DNX_DATA_MAX_FIELD_GROUP_NOF_FGS + 1));
    sal_memset(bit_msb_nosort, 0x0, sizeof(bit_msb_nosort[0]) * (DNX_DATA_MAX_FIELD_GROUP_NOF_FGS + 1));

    /*
     * Get state table info.
     */
    if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        dnx_field_context_mode_t context_mode;
        int context_reserves_state_table;
        SHR_IF_ERR_EXIT(dnx_field_context_mode_get(unit, field_stage, context_id, &context_mode));
        if (context_mode.state_table_mode == DNX_FIELD_CONTEXT_STATE_TABLE_MODE_ENABLED)
        {
            context_reserves_state_table = TRUE;
        }
        else if (context_mode.state_table_mode == DNX_FIELD_CONTEXT_STATE_TABLE_MODE_DISABLED)
        {
            context_reserves_state_table = FALSE;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "field_stage %d context_id %d, unknown state_table_mode %d.\r\n",
                         field_stage, context_id, context_mode.state_table_mode);
        }
        if (context_reserves_state_table)
        {
            int use_acr;
            dnx_field_stage_e state_table_field_stage;
            dbal_enum_value_field_field_key_e state_table_key_id;
            SHR_IF_ERR_EXIT(dnx_field_map_state_table_source_get
                            (unit, &use_acr, &state_table_field_stage, &state_table_key_id,
                             &state_table_lsb_on_key, &state_table_key_size));
            if ((use_acr == FALSE) && (state_table_field_stage == field_stage) && (state_table_key_id == key_id))
            {
                key_has_state_table_reserved = TRUE;
            }
        }
    }

    /*
     * Get all field groups on key.
     */
    nof_fg_ids = 0;
    for (fg_id_iter = 0; fg_id_iter < nof_fgs_total; fg_id_iter++)
    {
        uint8 is_allocated;
        dnx_field_context_t context_id_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS];
        int context_index;
        dnx_field_attach_context_info_t context_attach_info;
        int key_id_index;
        dnx_field_group_type_e fg_type;
        dnx_field_key_length_type_e key_length_type;
        dnx_field_stage_e fg_field_stage;
        int nof_keys_to_allocate;
        int lsb_first_key;
        int msb_first_key;
        int lsb_second_key;
        int msb_second_key;

        SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id_iter, &is_allocated));
        if (!is_allocated)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id_iter, &fg_field_stage));
        if (fg_field_stage != field_stage)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_group_context_id_arr_get(unit, fg_id_iter, context_id_arr));
        for (context_index = 0; context_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS &&
             context_id_arr[context_index] != DNX_FIELD_CONTEXT_ID_INVALID; context_index++)
        {
            if (context_id_arr[context_index] == context_id)
            {
                SHR_IF_ERR_EXIT(dnx_field_group_context_info_get(unit, fg_id_iter, context_id, &context_attach_info));
                for (key_id_index = 0; key_id_index < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX; key_id_index++)
                {
                    if (context_attach_info.key_id.id[key_id_index] == key_id)
                    {
                        /** Sanity check*/
                        if (nof_fg_ids == DNX_DATA_MAX_FIELD_GROUP_NOF_FGS)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found more than %d field groups.\n", nof_fg_ids);
                        }
                        fg_ids_nosort[nof_fg_ids] = fg_id_iter;
                        /** Get the LSB and MSB.*/
                        SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id_iter, &fg_type));
                        switch (fg_type)
                        {
                            case DNX_FIELD_GROUP_TYPE_TCAM:
                            case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
                            case DNX_FIELD_GROUP_TYPE_EXEM:
                            {
                                SHR_IF_ERR_EXIT(dnx_field_group_key_length_type_get
                                                (unit, fg_id_iter, &key_length_type));
                                SHR_IF_ERR_EXIT(dnx_algo_field_key_id_allocate_size_on_keys(unit, field_stage, fg_type,
                                                                                            key_length_type,
                                                                                            &nof_keys_to_allocate,
                                                                                            &lsb_first_key,
                                                                                            &msb_first_key,
                                                                                            &lsb_second_key,
                                                                                            &msb_second_key));
                                /** Sanity checks*/
                                if ((nof_keys_to_allocate < 1) || (nof_keys_to_allocate > 2))
                                {
                                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                                 "fg_id %d fg_type %d key_length type %d has %d keys.\n",
                                                 fg_id_iter, fg_type, key_length_type, nof_keys_to_allocate);
                                }
                                if ((nof_keys_to_allocate == 2) &&
                                    ((msb_first_key != msb_second_key) || (lsb_first_key != lsb_second_key)))
                                {
                                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                                 "fg_id %d fg_type %d key_length type %d has double key but the two "
                                                 "keys are note identical (%d-%d) and (%d-%d).\n",
                                                 fg_id_iter, fg_type, key_length_type,
                                                 lsb_first_key, msb_first_key, lsb_second_key, msb_second_key);
                                }
                                if (key_id_index >= nof_keys_to_allocate)
                                {
                                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                                 "fg_id %d fg_type %d key_length type %d has %d keys but matched "
                                                 "on key %d.\n",
                                                 fg_id_iter, fg_type, key_length_type,
                                                 nof_keys_to_allocate, key_id_index);
                                }

                                if (key_id_index == 0)
                                {
                                    bit_lsb_nosort[nof_fg_ids] = lsb_first_key;
                                    bit_msb_nosort[nof_fg_ids] = msb_first_key;
                                }
                                else if (key_id_index == 1)
                                {
                                    bit_lsb_nosort[nof_fg_ids] = lsb_second_key;
                                    bit_msb_nosort[nof_fg_ids] = msb_second_key;
                                }
                                else
                                {
                                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Reached illegal key within fg %d.\n", key_id_index);
                                }
                                nof_lookup_fgs++;
                                break;
                            }
                            case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
                            case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
                            case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
                            {
                                /** sanity check*/
                                if (key_id_index != 0)
                                {
                                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                                 "fg_id %d fg_type %d is not a lookup field group but has more "
                                                 "than one key (%d and %d). "
                                                 "key_id_index %d field_stage %d context_id %d.\n",
                                                 fg_id_iter, fg_type, context_attach_info.key_id.id[0],
                                                 context_attach_info.key_id.id[1], key_id_index, field_stage,
                                                 context_id);
                                }

                                if (key_id_index == 0)
                                {
                                    bit_lsb_nosort[nof_fg_ids] = context_attach_info.key_id.offset_on_first_key;
                                }
                                else
                                {
                                    bit_lsb_nosort[nof_fg_ids] = 0;
                                }
                                bit_msb_nosort[nof_fg_ids] =
                                    bit_lsb_nosort[nof_fg_ids] + context_attach_info.key_id.nof_bits[key_id_index] - 1;
                                break;

                            }
                            case DNX_FIELD_GROUP_TYPE_CONST:
                            {
                                SHR_ERR_EXIT(_SHR_E_INTERNAL, "fg_id %d is a const field group, but is "
                                             "attached to key %d in context %d stage %d.\n",
                                             fg_id_iter, key_id, context_id, field_stage);
                                break;
                            }
                            case DNX_FIELD_GROUP_TYPE_EXTERNAL_TCAM:
                            {
                                SHR_ERR_EXIT(_SHR_E_INTERNAL, "fg_id %d is an external TCAM field group, but is "
                                             "attached to key %d in context %d stage %d. Expected stage %d.\n",
                                             fg_id_iter, key_id, context_id, field_stage, DNX_FIELD_STAGE_EXTERNAL);
                                break;
                            }
                            default:
                            {
                                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown group type %d for fg_id %d.\n",
                                             fg_type, fg_id_iter);
                            }
                        }
                        if (fg_type == DNX_FIELD_GROUP_TYPE_STATE_TABLE)
                        {
                            if (state_table_fg_found)
                            {
                                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                             "fg_id %d is state table attached to "
                                             "field_stage %d context_id %d key_id %d, "
                                             "but no another stage table field group is attached to the key.\n",
                                             fg_id_iter, field_stage, context_id, key_id);
                            }
                            state_table_fg_found = TRUE;
                            if (key_has_state_table_reserved == FALSE)
                            {
                                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                             "fg_id %d is state table attached to "
                                             "field_stage %d context_id %d key_id %d, "
                                             "but no state table reserved for key.\n",
                                             fg_id_iter, field_stage, context_id, key_id);
                            }
                        }
                        nof_fg_ids++;
                    }
                }
            }
        }
    }
    /*
     * Add reserved space for state table.
     */
    if (key_has_state_table_reserved && (state_table_fg_found == FALSE))
    {
        fg_ids_nosort[nof_fg_ids] = DNX_FIELD_GROUP_INVALID;
        bit_lsb_nosort[nof_fg_ids] = state_table_lsb_on_key;
        bit_msb_nosort[nof_fg_ids] = state_table_lsb_on_key + state_table_key_size - 1;
        nof_fg_ids++;
    }
    /*
     * Sort by lsb on key.
     */
    {
        int fg_id_index;
        int fg_id_index_2;
        int highest_lsb_sorted = -1;
        int lowest_lsb_unsorted_in;

        for (fg_id_index = 0; fg_id_index < nof_fg_ids; fg_id_index++)
        {
            lowest_lsb_unsorted_in = DNX_DATA_MAX_FIELD_GROUP_NOF_FGS + 1;
            for (fg_id_index_2 = 0; fg_id_index_2 < nof_fg_ids; fg_id_index_2++)
            {
                if (bit_lsb_nosort[fg_id_index_2] > highest_lsb_sorted)
                {
                    if ((lowest_lsb_unsorted_in == (DNX_DATA_MAX_FIELD_GROUP_NOF_FGS + 1)) ||
                        (bit_lsb_nosort[fg_id_index_2] < bit_lsb_nosort[lowest_lsb_unsorted_in]))
                    {
                        lowest_lsb_unsorted_in = fg_id_index_2;
                    }
                }
            }
            if (lowest_lsb_unsorted_in == (DNX_DATA_MAX_FIELD_GROUP_NOF_FGS + 1))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to sort field groups.\n");
            }
            /** Sanity check, verify the same lsb isn't found for two field groups.*/
            for (fg_id_index_2 = 0; fg_id_index_2 < nof_fg_ids; fg_id_index_2++)
            {
                if ((fg_id_index_2 != lowest_lsb_unsorted_in) &&
                    (bit_lsb_nosort[fg_id_index_2] == bit_lsb_nosort[lowest_lsb_unsorted_in]))
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Two field groups %d and %d have the same lsb %d. "
                                 "field_stage %d context_id %d.\n",
                                 fg_ids_nosort[fg_id_index_2], fg_ids_nosort[lowest_lsb_unsorted_in],
                                 bit_lsb_nosort[fg_id_index_2], field_stage, context_id);
                }
            }
            fg_ids[fg_id_index] = fg_ids_nosort[lowest_lsb_unsorted_in];
            bit_lsb[fg_id_index] = bit_lsb_nosort[lowest_lsb_unsorted_in];
            bit_msb[fg_id_index] = bit_msb_nosort[lowest_lsb_unsorted_in];
            highest_lsb_sorted = bit_lsb_nosort[lowest_lsb_unsorted_in];
        }
        /** Sanity check, verify that the msb is not below the lsb.*/
        for (fg_id_index = 0; fg_id_index < nof_fg_ids; fg_id_index++)
        {
            if (bit_msb[fg_id_index] < bit_lsb[fg_id_index])
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "MSB (%d) lower than LSB (%d).\n",
                             bit_msb[fg_id_index], bit_lsb[fg_id_index]);
            }
        }
        /** Sanity check, verify that the field groups do not overlap.*/
        for (fg_id_index = 1; fg_id_index < nof_fg_ids; fg_id_index++)
        {
            if (bit_msb[fg_id_index - 1] >= bit_lsb[fg_id_index])
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "field groups %d and %d overlap. "
                             "field_stage %d context_id %d key_id %d.\n",
                             fg_ids[fg_id_index - 1], fg_ids[fg_id_index], field_stage, context_id, key_id);
            }
        }
    }

    (*nof_fgs_found_p) = nof_fg_ids;

    /** Sanity check verify lookups.*/
    {
        uint8 is_lookup_allocated;
        SHR_IF_ERR_EXIT(dnx_algo_field_key_key_lookup_occupation_bmp_is_allocated
                        (unit, field_stage, context_id, key_id, &is_lookup_allocated));
        if ((nof_lookup_fgs > 1) || (nof_lookup_fgs < 0))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "field_stage %d context_id %d key_id %d has %d lookup field groups attached.\n",
                         field_stage, context_id, key_id, nof_lookup_fgs);
        }
        if ((nof_lookup_fgs == 0) && is_lookup_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "field_stage %d context_id %d key_id %d has no lookup field groups attached, "
                         "but lookup allocated.\n", field_stage, context_id, key_id);
        }
        if ((nof_lookup_fgs == 1) && (is_lookup_allocated == FALSE))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "field_stage %d context_id %d key_id %d has a lookup field group attached, "
                         "but no lookup allocated.\n", field_stage, context_id, key_id);
        }
    }

    /** Sanity checks*/
    {
        int fg_id_index;
        uint32 bmp_from_groups[(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE + (SAL_UINT32_NOF_BITS - 1)) /
                               SAL_UINT32_NOF_BITS] = { 0 };
        uint32 bcm_from_algo[(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE + (SAL_UINT32_NOF_BITS - 1)) /
                             SAL_UINT32_NOF_BITS] = { 0 };
        int key_size = dnx_data_field.common_max_val.kbr_size_get(unit);
        int bit_index;
        uint8 bit_occupied;

        /** Find the allocated bitmap. */
        for (fg_id_index = 0; fg_id_index < nof_fg_ids; fg_id_index++)
        {
            if (utilex_bitstream_have_one_in_range(bmp_from_groups, bit_lsb[fg_id_index], bit_msb[fg_id_index]))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field groups overlapping. field_stage %d, context_id %d key_id %d.\n",
                             field_stage, context_id, key_id);
            }
            SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range
                            (bmp_from_groups, bit_lsb[fg_id_index], bit_msb[fg_id_index]));
        }
        /** Find the bitmap in algo_field_key.*/
        for (bit_index = 0; bit_index < key_size; bit_index++)
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_key_occupation_bmp_is_any_occupied
                            (unit, field_stage, context_id, key_id, bit_index, 1, &bit_occupied));
            if (bit_occupied)
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(bcm_from_algo, bit_index));
            }
        }
        /** Compare the two.*/
        for (bit_index = 0; bit_index < key_size; bit_index++)
        {
            if (utilex_bitstream_test_bit(bmp_from_groups, bit_index) !=
                utilex_bitstream_test_bit(bcm_from_algo, bit_index))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Field groups bit allocation mismatch. field_stage %d, context_id %d key_id %d bit %d.\n",
                             field_stage, context_id, key_id, bit_index);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This diagnostics function will add key allocation 'cells' to an existing PRT table,
 *  for specified field group type, stage and context.
 * \param [in] unit               - Device ID
 * \param [in] field_stage        -
 *   dnx_field_stage_e. Stage to use for key occupation display
 * \param [in] context_id            -
 *   dnx_field_context_t. Context id (program) to use for key occupation display
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller.
 * \param [in] prt_ctr -
 *   Control information related PRT tables. Required for procedures which, for example, add
 *   cells to an existing table.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * Output of this procedure is keys occupation as stored on SW state.
 * \see
 *   * dnx_field_diag_display_key_occupation()
 */
static shr_error_e
dnx_field_diag_display_key_occupation_partial(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    sh_sand_control_t * sand_control,
    prt_control_t * prt_ctr)
{
    uint8 key_iter;
    /**
     * Number of keys available for this stage.
     */
    uint8 nof_available_keys_on_stage;
    /**
     * Indicates the first key that is available for this stage.
     * 'uint8' is used here, to save swstate space, but the implied
     * type is dbal_enum_value_field_field_key_e
     */
    uint8 first_available_key_on_stage;
    uint8 last_available_key_on_stage;
    char *dnx_field_prefix_group_type;
    int dnx_field_prefix_group_type_size;
    dnx_field_group_t fg_ids[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS + 1];
    int bit_lsb[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS + 1];
    int bit_msb[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS + 1];
    int nof_fgs_found = -1;
    int fg_index;
    dnx_field_group_type_e fg_type;
    field_group_name_t group_name;

    SHR_FUNC_INIT_VARS(unit);
    dnx_field_prefix_group_type = "DNX_FIELD_GROUP_TYPE_";
    dnx_field_prefix_group_type_size =
        sal_strnlen(dnx_field_prefix_group_type, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    DNX_FIELD_STAGE_VERIFY(field_stage);
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                    nof_available_keys_on_stage.get(unit, field_stage, &nof_available_keys_on_stage));
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                    first_available_key_on_stage.get(unit, field_stage, &first_available_key_on_stage));
    if (nof_available_keys_on_stage <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "field_stage (%d) has no keys to allocate from.\r\n", field_stage);
    }

    last_available_key_on_stage = first_available_key_on_stage + nof_available_keys_on_stage - 1;

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    for (key_iter = first_available_key_on_stage; key_iter <= last_available_key_on_stage; key_iter++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%c", ('A' + key_iter));
        SHR_IF_ERR_EXIT(dnx_field_diag_fgs_on_key
                        (unit, field_stage, context_id, key_iter, &nof_fgs_found, fg_ids, bit_lsb, bit_msb));
        /** Sanity check for compiler*/
        if (nof_fgs_found < 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof_fgs_found (%d) illegal.\r\n", nof_fgs_found);
        }
        if (nof_fgs_found == 0)
        {
            PRT_CELL_SET("%s", "EMPTY");
            PRT_CELL_SET("%s", "-");
        }
        for (fg_index = 0; fg_index < nof_fgs_found; fg_index++)
        {
            if (fg_index > 0)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                /** key.*/
                PRT_CELL_SKIP(1);
            }
            if (fg_ids[fg_index] == DNX_FIELD_GROUP_INVALID)
            {
                /*
                 * Reserves space for state table.
                 */
                PRT_CELL_SET("N/A / RESERVED SPACE FOR STATE TABLE");
            }
            else
            {
                /*
                 * Field group.
                 */
                SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_ids[fg_index], &fg_type));
                /** Take the name for current FG from the SW state. */
                SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.name.get(unit, fg_ids[fg_index], &group_name));
                /**
                 * Check if Field Group name is being provided, in case no set N/A as default value,
                 * in other case set the provided name.
                 */
                if (!(sal_strncmp(group_name.value, "", sizeof(group_name.value))))
                {
                    sal_strncpy_s(group_name.value, "N/A", sizeof(group_name.value));
                }
                if (fg_type == DNX_FIELD_GROUP_TYPE_TCAM)
                {
                    PRT_CELL_SET("%d / \"%.*s\" (%s)",
                                 fg_ids[fg_index], (int) sizeof(group_name.value), group_name.value, "TCAM");
                }
                else
                {
                    PRT_CELL_SET("%d / \"%.*s\" (%s)",
                                 fg_ids[fg_index], (int) sizeof(group_name.value), group_name.value,
                                 (char *)
                                 &(dnx_field_group_type_e_get_name(fg_type))[dnx_field_prefix_group_type_size]);
                }
            }
            if (bit_msb[fg_index] == bit_lsb[fg_index])
            {
                PRT_CELL_SET("%d", bit_lsb[fg_index]);
            }
            else
            {
                PRT_CELL_SET("%d-%d", bit_lsb[fg_index], bit_msb[fg_index]);
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints information about which keys are
 *   being used by FGs attached to the current context.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage on which the context was created.
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
diag_dnx_field_context_info_fg_key_print(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    sh_sand_control_t * sand_control)
{
    int fg_id_index;
    dnx_field_group_context_full_info_t *attach_full_info = NULL;
    dnx_field_group_t fgs_per_context[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS];
    int nof_fgs_per_ctx;
    int qual_iter, ffc_iter, ffc_out_index = 0;
    int ffc_ids[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC];
    field_group_name_t group_name;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(attach_full_info, sizeof(dnx_field_group_context_full_info_t), "attach_full_info", "%s%s%s\r\n", EMPTY,
              EMPTY, EMPTY);

    for (ffc_iter = 0; ffc_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC; ffc_iter++)
    {
        /** Initialize the FFC IDs array to default values. */
        ffc_ids[ffc_iter] = DNX_FIELD_FFC_ID_INVALID;
    }

    SHR_IF_ERR_EXIT(dnx_field_group_context_fgs_get(unit, field_stage, context_id, fgs_per_context, &nof_fgs_per_ctx));

    if (field_stage == DNX_FIELD_STAGE_EXTERNAL)
    {
        PRT_TITLE_SET("Attached FGs and Lookup Ids");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FG ID");

        /** Print all attached FGs. */
        for (fg_id_index = 0; fg_id_index < nof_fgs_per_ctx; fg_id_index++)
        {
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "fg %d", fgs_per_context[fg_id_index]);
        }
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        PRT_CELL_SET("%s", "LOOKUP_ID");

        for (fg_id_index = 0; fg_id_index < nof_fgs_per_ctx; fg_id_index++)
        {
            /** Get attach info for the current context in fgs_per_context[]. */
            SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &(attach_full_info->attach_basic_info)));
            SHR_IF_ERR_EXIT(dnx_field_group_context_get
                            (unit, fgs_per_context[fg_id_index], context_id, attach_full_info));
            PRT_CELL_SET("%d", attach_full_info->attach_basic_info.payload_id);
        }
    }
    else
    {
        PRT_TITLE_SET("Attached FGs and Key usage");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Group ID/Name (type)");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Bits Allocated");
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        SHR_IF_ERR_EXIT(dnx_field_diag_display_key_occupation_partial
                        (unit, field_stage, context_id, sand_control, prt_ctr));
    }
    /** Print all used keys by the attached FGs. */
    for (fg_id_index = 0; fg_id_index < nof_fgs_per_ctx; fg_id_index++)
    {
        dnx_field_group_type_e fg_type;
        SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fgs_per_context[fg_id_index], &fg_type));

        /** Get attach info for the current context in fgs_per_context[]. */
        SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &(attach_full_info->attach_basic_info)));
        SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fgs_per_context[fg_id_index], context_id, attach_full_info));

        if (field_stage != DNX_FIELD_STAGE_EXTERNAL && fg_type == DNX_FIELD_GROUP_TYPE_CONST)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            /** key.*/
            PRT_CELL_SKIP(1);

            /** Take the name for current FG from the SW state. */
            SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.name.get(unit, fg_id_index, &group_name));
            PRT_CELL_SET("%d / \"%.*s\" (%s)", fg_id_index, (int) sizeof(group_name.value), group_name.value, "CONST");
            PRT_CELL_SKIP(1);
        }

        /** Iterate over max number of qualifiers for FG and check if current qual is valid for it. */
        for (qual_iter = 0; qual_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
             attach_full_info->group_ffc_info.key.qualifier_ffc_info[qual_iter].qual_type !=
             DNX_FIELD_QUAL_TYPE_INVALID; qual_iter++)
        {
            /**
             * Iterate over max number of FFCs for qualifier until we reach invalid one.
             * Fill the ffc_ids[] array with the valid FFC IDs indexes.
             */
            for (ffc_iter = 0; ffc_iter < DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL &&
                 attach_full_info->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[ffc_iter].ffc.ffc_id !=
                 DNX_FIELD_FFC_ID_INVALID; ffc_iter++)
            {
                int is_ffc_in_array = FALSE;
                /** Check if the current qualifier is already presented. */
                for (ffc_out_index = 0; ffc_out_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC &&
                     ffc_ids[ffc_out_index] != DNX_FIELD_FFC_ID_INVALID; ffc_out_index++)
                {
                    if (ffc_ids[ffc_out_index] ==
                        attach_full_info->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[ffc_iter].
                        ffc.ffc_id)
                    {
                        is_ffc_in_array = TRUE;
                        break;
                    }
                }

                if (ffc_out_index >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "ffc_out_index %d is out of ffc_ids[] array range (%d). Qualifier: %d FFC_ID: %d\r\n",
                                 ffc_out_index, DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC,
                                 attach_full_info->group_ffc_info.key.qualifier_ffc_info[qual_iter].qual_type,
                                 attach_full_info->group_ffc_info.key.qualifier_ffc_info[qual_iter].
                                 ffc_info[ffc_iter].ffc.ffc_id);
                }

                /** Continue to next FFC, if the current one is already in the array. */
                if (is_ffc_in_array)
                {
                    continue;
                }
                /**
                 * Set only those indexes from the ffc_ids[] array, which are valid. All others
                 * will say with the initial value DNX_FIELD_CONTEXT_ID_INVALID.
                 */
                ffc_ids[ffc_out_index] =
                    attach_full_info->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[ffc_iter].ffc.ffc_id;
            }
        }
    }

    if (nof_fgs_per_ctx)
    {
        PRT_COMMITX;
    }
    else
    {
        PRT_FREE;
        LOG_CLI_EX("\r\n" "No attached Field Groups were found for context %d !!!%s%s%s\r\n\n", context_id, EMPTY,
                   EMPTY, EMPTY);
    }

exit:
    SHR_FREE(attach_full_info);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints information about preselectors for a context.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage on which the context was created.
 * \param [in] context_id - Id of the current context.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 * \param [in] context_mode_p - Pointer, which contains
 *  all context mode information or IPMF1 and IPMF2 context modes.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_context_info_presels_print(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    sh_sand_control_t * sand_control,
    dnx_field_context_mode_t * context_mode_p)
{
    bcm_field_presel_t presel_index;
    uint32 max_nof_cs_lines;
    char presel_quals_string[DIAG_DNX_FIELD_UTILS_INPUT_STR_SIZE] = { 0 };
    int nof_presel_ids = 0;
    bcm_field_stage_t bcm_stage;
    bcm_field_presel_entry_id_t entry_id;
    bcm_field_presel_entry_data_t entry_data;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Preselectors");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Presel ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Presel Qualifier(Value, Mask, Arg)");

    /** Get max number of context selection lines. */
    max_nof_cs_lines = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_cs_lines;

    /** Sanity check.*/
    if (max_nof_cs_lines <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Stage %d has no context selection lines (%d).\r\n",
                     field_stage, max_nof_cs_lines);
    }

    SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage));

    for (presel_index = 0; presel_index < max_nof_cs_lines; presel_index++)
    {
        entry_id.presel_id = presel_index;
        entry_id.stage = bcm_stage;

        SHR_IF_ERR_EXIT(bcm_field_presel_get(unit, 0, &entry_id, &entry_data));
        if (!((entry_data.entry_valid) && (entry_data.context_id == context_id)))
        {
            continue;
        }

        SHR_IF_ERR_EXIT(diag_dnx_field_presel_string(unit, field_stage, presel_index, "\n ", presel_quals_string));
        nof_presel_ids++;

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("%d", presel_index);
        PRT_CELL_SET("%s", presel_quals_string);
    }

    if (nof_presel_ids > 0)
    {
        PRT_COMMITX;
    }
    else if ((field_stage == DNX_FIELD_STAGE_IPMF2) && (context_mode_p->context_ipmf2_mode.cascaded_from == context_id))
    {
        PRT_FREE;
        LOG_CLI_EX("\r\n" "No presel pointing to context %d, "
                   "but it is the default cascading context from iPMF1.%s%s%s\r\n\n", context_id, EMPTY, EMPTY, EMPTY);
    }
    else if (field_stage == DNX_FIELD_STAGE_EXTERNAL)
    {
        bcm_field_AppType_t apptype;
        dnx_field_context_apptype_info_t apptype_info;

        SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_apptype_dnx_to_bcm(unit, context_id, &apptype));
        SHR_IF_ERR_EXIT(dnx_field_context_apptype_info_t_init(unit, &apptype_info));
        SHR_IF_ERR_EXIT(dnx_field_context_apptype_info_get(unit, apptype, &apptype_info));
        if (apptype_info.base_static_apptype == DNX_FIELD_APPTYPE_INVALID)
        {
            PRT_FREE;
            LOG_CLI_EX("\r\n" "No presel pointing to opcode %d apptype %d, "
                       "but it is a predefined apptype.%s%s\r\n\n", context_id, apptype, EMPTY, EMPTY);
        }
        else
        {
            PRT_FREE;
            LOG_CLI_EX("\r\n" "No presel pointing to opcode %d apptype %d.%s%s\r\n\n",
                       context_id, apptype, EMPTY, EMPTY);
        }
    }
    else
    {
        PRT_FREE;
        LOG_CLI_EX("\r\n" "No presel pointing to context %d.%s%s%s\r\n\n", context_id, EMPTY, EMPTY, EMPTY);
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

#if defined(INCLUDE_KBP)
/**
 * \brief
 *   This function prepares/sets/prints information about master key,
 *   being used by External FGs attached to the current context.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage on which the context was created.
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
diag_dnx_field_context_info_external_master_key_print(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    sh_sand_control_t * sand_control)
{
    int fg_id_index;
    int nof_fgs_per_ctx;
    dnx_field_group_context_full_info_t *attach_full_info = NULL;
    dnx_field_group_t fgs_per_context[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS];
    int qual_iter;
    kbp_mngr_key_segment_t ms_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    uint32 nof_segments;
    uint32 payload_ids[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS];
    uint8 segments_presented[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY] = { 0 };
    uint32 opcode_id;
    uint8 nof_fwd_segments;
    uint8 seg_index_on_master_by_qual[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    uint8 qual_index_on_segment[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    uint8 qual_offset_on_segment[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    int qual_packed_and_not_first;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    opcode_id = context_id;

    SHR_ALLOC(attach_full_info, sizeof(dnx_field_group_context_full_info_t), "attach_full_info", "%s%s%s\r\n", EMPTY,
              EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_field_group_context_fgs_get(unit, field_stage, context_id, fgs_per_context, &nof_fgs_per_ctx));
    SHR_IF_ERR_EXIT(diag_dnx_field_utils_payload_ids_per_fg_get(unit, opcode_id, payload_ids));

    PRT_TITLE_SET("Master Key Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qualifier");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Lsb");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Size on Key");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Segment Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Used by FGs");

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_segments, ms_key_segments));
    SHR_IF_ERR_EXIT(dnx_field_kbp_group_opcode_nof_fwd_segments(unit, opcode_id, &nof_fwd_segments));

    /** Print all used keys by the attached FGs. */
    for (fg_id_index = 0; fg_id_index < nof_fgs_per_ctx; fg_id_index++)
    {
        /** Get attach info for the current context in fgs_per_context[]. */
        SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fgs_per_context[fg_id_index], opcode_id, attach_full_info));
        /** Get the segment indices on master key for the qualifiers. */
        SHR_IF_ERR_EXIT(dnx_field_kbp_group_segment_indices(unit, fgs_per_context[fg_id_index], opcode_id,
                                                            seg_index_on_master_by_qual, qual_index_on_segment,
                                                            qual_offset_on_segment));

        /** Iterate over max number of qualifiers for FG and check if current qual is valid for it. */
        for (qual_iter = 0; qual_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
             attach_full_info->group_ffc_info.key.qualifier_ffc_info[qual_iter].qual_type !=
             DNX_FIELD_QUAL_TYPE_INVALID; qual_iter++)
        {
            char segment_sharing_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
            dbal_enum_value_field_kbp_kbr_idx_e kbp_kbr_id;
            uint32 offset_in_kbp_kbr;
            uint32 qual_size;
            uint32 seg_index;
            int qual_size_on_key;

            seg_index = seg_index_on_master_by_qual[qual_iter];

            if (seg_index >= nof_segments)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "qualifier number %d in field group %d uses segment %d, only %d segments in master key.\r\n",
                             qual_iter, fgs_per_context[fg_id_index], seg_index, nof_segments);
            }

            /** Continue to next qualifier, if the segment is already presented. */
            if (segments_presented[seg_index])
            {
                continue;
            }
            else
            {
                /** Mark the segment as presented. */
                segments_presented[seg_index] = TRUE;
            }

            SHR_IF_ERR_EXIT(diag_dnx_field_utils_kbp_segment_share_string_update
                            (unit, fgs_per_context[fg_id_index], opcode_id, seg_index, ms_key_segments,
                             nof_fwd_segments, payload_ids, &(attach_full_info->attach_basic_info), qual_iter,
                             segment_sharing_string));

            /** Get the qual size. */
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                            (unit, field_stage, attach_full_info->attach_basic_info.dnx_quals[qual_iter],
                             NULL, NULL, &qual_size));

            qual_size_on_key = BYTES2BITS(ms_key_segments[seg_index].nof_bytes);

            if (qual_index_on_segment[qual_iter] > 0)
            {
                qual_packed_and_not_first = TRUE;
            }
            else
            {
                qual_packed_and_not_first = FALSE;
            }

            /*
             * Get the segment placement on key.
             */
            if (ms_key_segments[seg_index].is_overlay_field)
            {
                uint32 keys_bmp;
                int offset_on_last_key;
                if (attach_full_info->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[0].ffc.ffc_id !=
                    DNX_FIELD_FFC_ID_INVALID)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "For opcode_id %d, fg_id %d, qualifier number %d (0x%x), FFC found, "
                                 "but the qualifeir is an overlay segemnt.\n",
                                 opcode_id, fgs_per_context[fg_id_index], qual_iter,
                                 attach_full_info->group_ffc_info.key.qualifier_ffc_info[qual_iter].qual_type);
                }
                SHR_IF_ERR_EXIT(dnx_field_kbp_key_master_offset_to_key(unit, opcode_id,
                                                                       BYTES2BITS(ms_key_segments
                                                                                  [seg_index].overlay_offset_bytes),
                                                                       qual_size_on_key, &kbp_kbr_id, &keys_bmp,
                                                                       &offset_on_last_key));
                offset_in_kbp_kbr = offset_on_last_key;
            }
            else
            {
                if (attach_full_info->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[0].ffc.ffc_id ==
                    DNX_FIELD_FFC_ID_INVALID)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "For opcode_id %d, fg_id %d, qualifier number %d (0x%x), no FFC found.\n", opcode_id,
                                 fgs_per_context[fg_id_index], qual_iter,
                                 attach_full_info->group_ffc_info.key.qualifier_ffc_info[qual_iter].qual_type);
                }

                SHR_IF_ERR_EXIT(dnx_field_kbp_key_fwd2_to_acl_key_id_convert
                                (unit,
                                 attach_full_info->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[0].
                                 ffc.key_id, &kbp_kbr_id));
                offset_in_kbp_kbr =
                    attach_full_info->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[0].ffc.
                    ffc_instruction.key_offset;
            }

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", dnx_field_dnx_qual_text(unit, attach_full_info->attach_basic_info.dnx_quals[qual_iter]));
            PRT_CELL_SET("%s", dbal_enum_id_type_to_string(unit, DBAL_FIELD_TYPE_DEF_FIELD_KEY, kbp_kbr_id));
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
        }
    }

    if (nof_fgs_per_ctx)
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
        LOG_CLI_EX("\r\n" "No attached Field Groups were found for context %d(AT) !!!%s%s%s\r\n\n", context_id, EMPTY,
                   EMPTY, EMPTY);
    }

exit:
    SHR_FREE(attach_full_info);
    PRT_FREE;
    SHR_FUNC_EXIT;
}
#endif

/**
 * \brief
 *   This function returns Key Gen value for the given stage and context ID.
 *
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage on which context has been allocated.
 * \param [in] context_id - Id of the current context for which info will be presented.
 * \param [out] key_gen_value_p - Key Gen Value, which was set by context_param_set().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_context_info_key_gen_var_info_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    uint32 *key_gen_value_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_PMF_CONTEXT_GENERAL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_STAGE, field_stage);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PROGRAM_KEY_GEN_VAR, INST_SINGLE, key_gen_value_p);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function returns System Header Profile index for the given context ID.
 *
 * \param [in] unit - The unit number.
 * \param [in] context_id - Id of the current context for which info will be presented.
 * \param [out] sys_hdr_prof_index_p - Sytem header profile index, which was set by context_param_set().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_context_info_sys_hdr_prof_index_get(
    int unit,
    dnx_field_context_t context_id,
    uint32 *sys_hdr_prof_index_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_IPMF1_CONTEXT_GENERAL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SYSTEM_HEADER_PROFILE, INST_SINGLE,
                               sys_hdr_prof_index_p);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function returns Bytes to remove header info for the given context ID.
 *
 * \param [in] unit - The unit number.
 * \param [in] context_id - Id of the current context for which info will be presented.
 * \param [out] bytes_to_remove_p - Bytes to remove, which was set by context_param_set().
 * \param [out] layer_to_remove_p - Layer to remove, which was set by context_param_set().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_context_info_bytes_to_remove_header_info_get(
    int unit,
    dnx_field_context_t context_id,
    uint32 *bytes_to_remove_p,
    uint32 *layer_to_remove_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_IPMF2_CONTEXT_GENERAL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_NUM_BYTES_TO_REMOVE, INST_SINGLE, bytes_to_remove_p);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_NUM_LAYERS_TO_REMOVE, INST_SINGLE, layer_to_remove_p);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints external stage apptype related info,
 *   like opcode_id, FWD contexts and ACL contexts.
 * \param [in] unit - The unit number.
 * \param [in] context_id - The context id (opcode).
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_context_info_external_info_print(
    int unit,
    dnx_field_context_t context_id,
    sh_sand_control_t * sand_control)
{
    int opcode_id;
    int fwd_nof_contexts;
    int acl_nof_contexts;
    unsigned int fwd_context_idx;
    unsigned int acl_context_idx;
    uint8 do_display;
    char fwd_ctx_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    char acl_ctx_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    dnx_field_context_t fwd_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
    dnx_field_context_t acl_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    opcode_id = context_id;

    PRT_TITLE_SET("KBP Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Opcode Id");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FWD Context");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ACL Context");

    /** Prepare and print FFCs and qualifiers which are using them. */
    SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_lookup_fwd_contexts(unit, opcode_id, &fwd_nof_contexts, fwd_contexts));
    SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_acl_contexts(unit, opcode_id, TRUE, &acl_nof_contexts, acl_contexts));
    for (fwd_context_idx = 0; fwd_context_idx < fwd_nof_contexts; fwd_context_idx++)
    {
        char fwd_ctx_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
        if (sal_strncmp(fwd_ctx_string, "", sizeof(fwd_ctx_string)))
        {
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(fwd_ctx_string, "%s", ", ");
            sal_strncat_s(fwd_ctx_string, ", ", sizeof(fwd_ctx_string));
        }
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%d", fwd_contexts[fwd_context_idx]);
        sal_snprintf(fwd_ctx_buff, sizeof(fwd_ctx_buff), "%d", fwd_contexts[fwd_context_idx]);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(fwd_ctx_string, "%s", fwd_ctx_buff);
        sal_strncat_s(fwd_ctx_string, fwd_ctx_buff, sizeof(fwd_ctx_string));
        do_display = TRUE;
    }
    for (acl_context_idx = 0; acl_context_idx < acl_nof_contexts; acl_context_idx++)
    {
        char acl_ctx_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
        if (sal_strncmp(acl_ctx_string, "", sizeof(acl_ctx_string)))
        {
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(acl_ctx_string, "%s", ", ");
            sal_strncat_s(acl_ctx_string, ", ", sizeof(acl_ctx_string));
        }
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%d", acl_contexts[acl_context_idx]);
        sal_snprintf(acl_ctx_buff, sizeof(acl_ctx_buff), "%d", acl_contexts[acl_context_idx]);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(acl_ctx_string, "%s", acl_ctx_buff);
        sal_strncat_s(acl_ctx_string, acl_ctx_buff, sizeof(acl_ctx_string));
        do_display = TRUE;
    }
    

    if (do_display)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%d", opcode_id);
        PRT_CELL_SET("%s", fwd_ctx_string);
        PRT_CELL_SET("%s", acl_ctx_string);

        PRT_COMMITX;
    }
    else
    {
        PRT_FREE;
        LOG_CLI_EX("\r\n" "No FFC info was found for External stage opcode_id %d!!!%s%s%s\r\n\n", context_id, EMPTY,
                   EMPTY, EMPTY);
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function is verifying diagnostic input parameters.
 * \param [in] unit - The unit number.
 * \param [in] context_id - Input context id.
 * \param [in] field_stage - Input field stage.
 * \param [in] opcode_id - Input opcode id.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_context_info_input_verify(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_stage_e field_stage,
    uint32 opcode_id)
{
    int nof_contexts;

    SHR_FUNC_INIT_VARS(unit);

    nof_contexts = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_contexts;

    if (field_stage != DNX_FIELD_STAGE_EXTERNAL && context_id == DNX_FIELD_CONTEXT_ID_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n" "Presence of option:\"ConTeXt\" is requested!\r\n\n");
    }

    if (field_stage != DNX_FIELD_STAGE_EXTERNAL &&
        (((unsigned int) (context_id)) >= ((unsigned int) (nof_contexts)) || (context_id >= nof_contexts)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n" "Value:\"%d\" for option:\"ConTeXt\" is out of range:%d-%d!\r\n\n",
                     context_id, 0, (nof_contexts - 1));
    }

    if (field_stage != DNX_FIELD_STAGE_EXTERNAL && opcode_id != DNX_FIELD_CONTEXT_ID_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n" "Option:\"OPcode\" is supported only for External stage!\r\n\n");
    }

    if (field_stage == DNX_FIELD_STAGE_EXTERNAL && context_id == DNX_FIELD_CONTEXT_ID_INVALID &&
        opcode_id == DNX_FIELD_CONTEXT_ID_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n"
                     "Presence of either option \"ConTeXt\" or option \"OPcode\" is requested!\r\n\n");
    }

    if (field_stage == DNX_FIELD_STAGE_EXTERNAL && context_id != DNX_FIELD_CONTEXT_ID_INVALID &&
        (((int) context_id < 0) || (context_id >= bcmFieldAppTypeCount)) &&
        ((context_id < dnx_data_field.external_tcam.apptype_user_1st_get(unit)) ||
         (context_id >= (dnx_data_field.external_tcam.apptype_user_1st_get(unit) +
                         dnx_data_field.external_tcam.apptype_user_nof_get(unit)))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n"
                     "NOTE! Option \"ConTeXt\", in case of External stage, has meaning of APPTYPE!\r\n"
                     "Apptype %d out of range. Must be 0 and %d for predefined apptypes, "
                     "or between %d and %d for user defiend apptypes.\r\n\n",
                     context_id, bcmFieldAppTypeCount, dnx_data_field.external_tcam.apptype_user_1st_get(unit),
                     (dnx_data_field.external_tcam.apptype_user_1st_get(unit) +
                      dnx_data_field.external_tcam.apptype_user_nof_get(unit) - 1));
    }

    if (context_id != DNX_FIELD_CONTEXT_ID_INVALID && opcode_id != DNX_FIELD_CONTEXT_ID_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n" "Options \"OPcode\" and \"ConTeXt\" cannot be set together. "
                     "Only one option is to be set!\r\n\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays Context full info per context ID
 *   on a specific field stage specified by the caller.
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
diag_dnx_field_context_info_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 is_allocated;
    dnx_field_context_t context_id;
    int field_stage_enum_value;
    dnx_field_stage_e field_stage;
    dnx_field_context_mode_t context_mode;
    bcm_field_stage_t bcm_stage;
    uint32 opcode_id;
    shr_error_e rv;
    uint8 apptype_found;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_CONTEXT, context_id);
    SH_SAND_GET_ENUM(DIAG_DNX_FIELD_OPTION_STAGE, field_stage_enum_value);
    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_OPCODE, opcode_id);

    SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                    (unit, Field_context_diag_stage_enum_table[field_stage_enum_value].string, &field_stage));

    SHR_IF_ERR_EXIT(diag_dnx_field_context_info_input_verify(unit, context_id, field_stage, opcode_id));

    if (field_stage == DNX_FIELD_STAGE_EXTERNAL)
    {
        if (opcode_id == DNX_FIELD_CONTEXT_ID_INVALID)
        {
            bcm_field_AppType_t app_type = context_id;
            /** Convert apptype to opcode and pass it on as context. */
            rv = dnx_field_map_apptype_to_opcode_bcm_to_dnx_int(unit, app_type, &opcode_id);
            if (rv == _SHR_E_NOT_FOUND)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n"
                             "Apptype %d, given in option \"ConTeXt\" does not exist.\r\n\n", app_type);
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
        }
        context_id = opcode_id;
    }

    /** Verify if the given context_id is in range. */
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);

    /** Check if the specified Context ID is allocated. */
    SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated(unit, field_stage, context_id, &is_allocated));
    if (!is_allocated)
    {
        /** Convert DNX to BCM Field Stage */
        SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage));
        SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n" "Context ID %d is not allocated for %s stage!!\r\n\n",
                     context_id, dnx_field_bcm_stage_text(bcm_stage));
    }

    PRT_TITLE_SET("CONTEXT");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "CTX ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Stage");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Presel IDs");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Mode");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key Gen Var");
    if (field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "System Header Profile");
    }
    else if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Bytes to Remove");
    }

    /**
     * Prepare and print a table, which contains all basic info about the current context id on
     * the given field stage. Returns context_mode info to be used in for compare and hash key
     * preparation and printing.
     */
    SHR_IF_ERR_EXIT(diag_dnx_field_context_info_print
                    (unit, field_stage, context_id, FALSE, prt_ctr, &context_mode, &apptype_found));
    if (field_stage == DNX_FIELD_STAGE_EXTERNAL && !apptype_found)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n" "Opcode ID %d doesn't exist!!\r\n\n", context_id);
    }

    /** Key Gen value is not available on egress and external stages, in other cases we will present it. */
    if (field_stage != DNX_FIELD_STAGE_EPMF && field_stage != DNX_FIELD_STAGE_EXTERNAL)
    {
        uint32 key_gen_value;

        SHR_IF_ERR_EXIT(diag_dnx_field_context_info_key_gen_var_info_get
                        (unit, field_stage, context_id, &key_gen_value));

        PRT_CELL_SET("0x%08x", key_gen_value);
    }
    else
    {
        PRT_CELL_SET("%s", "N/A");
    }

    /** Present the system header profiles in case of IPMF1 and layers and bytes to remove for IPMF3. */
    if (field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        uint32 sys_hdr_prof_index;

        SHR_IF_ERR_EXIT(diag_dnx_field_context_info_sys_hdr_prof_index_get(unit, context_id, &sys_hdr_prof_index));

        PRT_CELL_SET("%s", dnx_data_field.system_headers.system_header_profiles_get(unit, sys_hdr_prof_index)->name);
    }
    else if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        uint32 bytes_to_remove;
        uint32 layer_to_remove;

        SHR_IF_ERR_EXIT(diag_dnx_field_context_info_bytes_to_remove_header_info_get
                        (unit, context_id, &bytes_to_remove, &layer_to_remove));

        PRT_CELL_SET("%s + %d bytes", dnx_field_bcm_packet_remove_layer_text(layer_to_remove), bytes_to_remove);
    }

    PRT_COMMITX;

    if (field_stage == DNX_FIELD_STAGE_EXTERNAL)
    {
#if defined(INCLUDE_KBP)
        /*
         * Wait for device lock if there is a KBP device.
         */
        int waiting_for_lock = FALSE;
        int external_tcam_exists;
        SHR_IF_ERR_EXIT(dnx_field_map_is_external_tcam_available(unit, &external_tcam_exists));
        if (external_tcam_exists)
        {
            int device_locked;
            SHR_IF_ERR_EXIT(dnx_field_map_is_device_locked(unit, &device_locked));
            if (device_locked == FALSE)
            {
                waiting_for_lock = TRUE;
                LOG_CLI_EX("\r\n"
                           "External stage apptype info will be presented only after device lock!!%s%s%s%s\r\n\n",
                           EMPTY, EMPTY, EMPTY, EMPTY);
            }
        }
        if (waiting_for_lock == FALSE)
#endif
        {
            /**
             * Prepare and print a table, which contains KBP related information, like
             * opcode_id, FWD contexts and ACL contexts.
             */
            SHR_IF_ERR_EXIT(diag_dnx_field_context_info_external_info_print(unit, context_id, sand_control));
            /**
             * Prepare and print a table, which contains information about which keys are
             * being used by FGs attached to the current context.
             */
            SHR_IF_ERR_EXIT(diag_dnx_field_context_info_fg_key_print(unit, field_stage, context_id, sand_control));
#if defined(INCLUDE_KBP)
            if (external_tcam_exists)
            {
                /**
                 * Prepare and print a table, which contains information about master key.
                 * Like used segments (key in which it is located, lsb, size).
                 */
                SHR_IF_ERR_EXIT(diag_dnx_field_context_info_external_master_key_print
                                (unit, field_stage, context_id, sand_control));
            }
#endif
        }
    }
    else
    {
        /**
         * Prepare and print a table, which contains information about which keys are
         * being used by FGs attached to the current context.
         */
        SHR_IF_ERR_EXIT(diag_dnx_field_context_info_fg_key_print(unit, field_stage, context_id, sand_control));

        if (field_stage == DNX_FIELD_STAGE_IPMF1)
        {
            /** Prepare and print tables, which contain information about compare and hash key construction. */
            SHR_IF_ERR_EXIT(diag_dnx_field_context_info_cmp_hash_key_print
                            (unit, field_stage, context_id, &context_mode, sand_control));
        }
    }

    /**
     * Print presels
     */
    SHR_IF_ERR_EXIT(diag_dnx_field_context_info_presels_print
                    (unit, field_stage, context_id, sand_control, &context_mode));

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * Globals required at end because they reference static procedures/tables above.
 * {
 */
/**
 * \brief
 *   List of shell options for 'context' shell commands (list, info)
 */
sh_sand_cmd_t Sh_dnx_field_context_cmds[] = {
    {"list", diag_dnx_field_context_list_cb, NULL, Field_context_list_options, &Field_context_list_man}
    ,
    {"info", diag_dnx_field_context_info_cb, NULL, Field_context_info_options, &Field_context_info_man}
    ,
    {NULL}
};

/*
 * }
 */
