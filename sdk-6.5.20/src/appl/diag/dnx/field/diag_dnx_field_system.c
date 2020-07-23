/** \file diag_dnx_field_system.c
 *
 * Diagnostics procedures, for DNX, for 'system' operations.
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
#include "diag_dnx_field_system.h"
#include <bcm_int/dnx/field/field_context.h>
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
 *  'Help' description for System Info (shell commands).
 */
static sh_sand_man_t Field_system_info_man = {
    .brief = "'System Info'- displays system view of the entire field lookup process for a certain stage (filter by field stage).",
    .full = "'System Info' displays system view of the entire field lookup process for a certain stage.\r\n"
            "If no stage 'IPMF1'-'EPMF' is specified then an error will occur.\r\n",
    .synopsis = "[stage=<IPMF1 | IPMF2 | IPMF3 | EPMF>]",
    .examples = "stage=IPMF1",
};
static sh_sand_option_t Field_system_info_options[] = {
    {DIAG_DNX_FIELD_OPTION_STAGE,  SAL_FIELD_TYPE_UINT32, "Field stage to show system info for",   NULL,    (void *)Field_system_diag_stage_enum_table, "STAGE_LOWEST-STAGE_HIGHEST"},
    {NULL}
};

/**
 *  'Help' description for System Header (shell commands).
 */
static sh_sand_man_t Field_system_header_man = {
    .brief = " displays the basic info about all system header profiles configured in the system ",
    .full = "displays the basic info about all system header profiles configured in the system ",
    .synopsis = "",
    .examples = "",
};

/*
 * }
 */
/* *INDENT-ON* */

/**
 * \brief
 *  This function collects and sets information about context to which, the presel is being set and
 *  field groups, which are being attach to the same context.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage on which is given by the user.
 * \param [in] entry_data_p - Pointer, which contains the information about the presel entry data,
 *  used to be filled in the relevant cells and for some verifies.
 * \param [out] fg_string_p - Following string is used to store information about all attached field groups, to a specific context,
 *  and their names. The size of the string is calculated by multiplying the max number of field groups
 *  and max number of symbols, which we can have per field group. Example (120 / "EXAMPLE_FIELD_GROUP").
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_system_info_ctx_fg_print(
    int unit,
    dnx_field_stage_e field_stage,
    bcm_field_presel_entry_data_t * entry_data_p,
    char *fg_string_p,
    prt_control_t * prt_ctr)
{
    uint8 is_fg_allocated;
    int ctx_id_index, fg_id_index;
    int max_nof_fgs;
    dnx_field_group_full_info_t *fg_info = NULL;

    /**
     * Following string is used to store information about all attached field groups, to a specific context,
     * and their names. The size of the string is calculated by multiplying the max number of field groups
     * and max number of symbols, which we can have per field group. Example (120 / "EXAMPLE_FIELD_GROUP").
     */
    char fg_string[40 * DBAL_MAX_SHORT_STRING_LENGTH] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /** Get max number of field groups. */
    max_nof_fgs = dnx_data_field.group.nof_fgs_get(unit);

    SHR_ALLOC(fg_info, sizeof(dnx_field_group_full_info_t), "fg_info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /**
     * Iterate over maximum number of FGs and take information for those, which are being
     * attached to the current context.
     */
    for (fg_id_index = 0; fg_id_index < max_nof_fgs; fg_id_index++)
    {
        /** Array, which will store all contexts that the FG is attached to */
        dnx_field_context_t context_id_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS];

        /** Check if the current FG is allocated. */
        SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id_index, &is_fg_allocated));
        if (!is_fg_allocated)
        {
            /** If FG_ID is not allocated, move to next FG ID */
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_field_group_get(unit, fg_id_index, fg_info));
        if (fg_info->group_basic_info.field_stage != field_stage)
        {
            /**
             * If the current allocated FG ID is not on the same stage as the given context,
             * continue iterating over all FGs.
             */
            continue;
        }

        /** Initialize the context_id array to default values. */
        sal_memset(context_id_arr, DNX_FIELD_CONTEXT_ID_INVALID, sizeof(context_id_arr));

        /** Get the list of all contexts that the FG is attached to. */
        SHR_IF_ERR_EXIT(dnx_field_group_context_id_arr_get(unit, fg_id_index, context_id_arr));

        /**
         * Check if the any of the context IDs of the context array for the current FG contains
         * the context id of the preselector entry.
         * In case yes, store the ID and name of the current FG in the fg_string.
         */
        for (ctx_id_index = 0; ctx_id_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS &&
             context_id_arr[ctx_id_index] != DNX_FIELD_CONTEXT_ID_INVALID; ctx_id_index++)
        {
            if (context_id_arr[ctx_id_index] == entry_data_p->context_id)
            {
                char fg_id_name_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
                /**
                 * Check if Field Group name is being provided, in case no set N/A as default value,
                 * in other case set the provided name.
                 */
                if (sal_strncmp(fg_info->group_basic_info.name, "", sizeof(fg_info->group_basic_info.name)))
                {
                    DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                                          "%d / \"%s\"\n", fg_id_index, fg_info->group_basic_info.name);

                    sal_snprintf(fg_id_name_buff, sizeof(fg_id_name_buff), "%d / \"%s\"\n", fg_id_index,
                                 fg_info->group_basic_info.name);
                }
                else
                {
                    DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%d / \"N/A\"\n", fg_id_index);

                    sal_snprintf(fg_id_name_buff, sizeof(fg_id_name_buff), "%d / \"N/A\"\n", fg_id_index);
                }
                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(fg_string, "%s", fg_id_name_buff);
                sal_strncat_s(fg_string, fg_id_name_buff, sizeof(fg_string));
            }
        }
    }

    sal_strncpy(fg_string_p, fg_string, (40 * DBAL_MAX_SHORT_STRING_LENGTH));

exit:
    SHR_FREE(fg_info);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function prints information about system view of the entire field lookup process.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage for which the system info should be printed.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 * \param [out] do_display_p - Pointer, which indicates if any info was found on the given stage.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_system_info_print(
    int unit,
    dnx_field_stage_e field_stage,
    prt_control_t * prt_ctr,
    int *do_display_p)
{
    bcm_field_presel_t presel_index, presel_qual_index;
    uint32 max_nof_cs_lines;
    uint8 ipmf2_cotnext_print_flag;

    bcm_field_stage_t bcm_stage;
    bcm_field_presel_entry_id_t entry_id;
    bcm_field_presel_entry_data_t entry_data;
    bcm_field_presel_entry_id_t entry_id_ipmf1;
    bcm_field_presel_entry_data_t entry_data_ipmf1;
    dnx_field_context_mode_t context_mode;

    /**
     * Following string is used to store information about all attached field groups, to a specific context,
     * and their names. The size of the string is calculated by multiplying the max number of field groups
     * and max number of symbols, which we can have per field group. Example (120 / "EXAMPLE_FIELD_GROUP").
     */
    char fg_string[40 * DBAL_MAX_SHORT_STRING_LENGTH] = { 0 };
    char context_name[DBAL_MAX_STRING_LENGTH];

    SHR_FUNC_INIT_VARS(unit);

    /** Get max number of context selection lines. */
    max_nof_cs_lines = dnx_data_field.common_max_val.nof_cs_lines_get(unit);

    /** Get information about preselectors on the given field stage. */
    for (presel_index = 0; presel_index < max_nof_cs_lines; presel_index++)
    {
        /**
         * Following string is used to store information about all preselector qualifiers.
         * The size of the string is calculated by multiplying the max number of context selection qualifiers (per presel)
         * and max number of symbols, which we can have per qualifier.
         * Example:
         *  Qual: ExamplePreselQualifier
         *   - Val: 0xFFFF
         *   - Arg: 1
         *   - Mask: 0xFFFF
         */
        char presel_quals_string[BCM_FIELD_MAX_NOF_CS_QUALIFIERS * 80] = { 0 };

        /** Initialize the flag for every new preselector. */
        ipmf2_cotnext_print_flag = FALSE;

        bcm_field_presel_entry_id_info_init(&entry_id_ipmf1);
        bcm_field_presel_entry_data_info_init(&entry_data_ipmf1);
        bcm_field_presel_entry_id_info_init(&entry_id);
        bcm_field_presel_entry_data_info_init(&entry_data);
        entry_id.presel_id = presel_index;
        /** Convert DNX to BCM Field Stage. */
        SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage));
        entry_id.stage = bcm_stage;
        SHR_IF_ERR_EXIT(bcm_field_presel_get(unit, 0, &entry_id, &entry_data));

        SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));
        /**
         * In case of IPMF2 stage and the current context is not the DEFAULT one, we should retrieve context_mode
         * information and later on to check the cascaded_from value.
         */
        if (field_stage == DNX_FIELD_STAGE_IPMF2 && entry_data.context_id != DNX_FIELD_CONTEXT_ID_DEFAULT(unit)
            && entry_data.context_id != BCM_FIELD_CONTEXT_ID_INVALID)
        {
            /** Get information about the presel entry context. */
            SHR_IF_ERR_EXIT(dnx_field_context_mode_get(unit, field_stage, entry_data.context_id, &context_mode));
        }
        /**
         * Check if the presel entry is valid and if we have any qualifiers in the presel set.
         * In case no, continue iterating over all preselectors. In other hand check if the
         * cascaed_from context is not invalid, we should pass the if statement and to retrieve
         * and present the relevant information about it. In some cases we can have a dummy presel
         * to match on and to change the IPMF1 context with a new one in IPMF2 stage.
         */
        if (!((entry_data.entry_valid) && (entry_data.nof_qualifiers)) &&
            ((context_mode.context_ipmf2_mode.cascaded_from == DNX_FIELD_CONTEXT_ID_INVALID)))
        {
            if (field_stage == DNX_FIELD_STAGE_IPMF2)
            {
                entry_id_ipmf1.presel_id = presel_index;
                entry_id_ipmf1.stage = bcmFieldStageIngressPMF1;
                SHR_IF_ERR_EXIT(bcm_field_presel_get(unit, 0, &entry_id_ipmf1, &entry_data_ipmf1));
                if ((entry_data_ipmf1.entry_valid) && (entry_data_ipmf1.nof_qualifiers))
                {
                    ipmf2_cotnext_print_flag = TRUE;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                continue;
            }
        }

        /**
         * Construct a string, which contains information (Qual name/value/arg/mask) about
         * all preselector qualifiers.
         */
        for (presel_qual_index = 0; presel_qual_index < entry_data.nof_qualifiers &&
             entry_data.qual_data[presel_qual_index].qual_type != bcmFieldQualifyCount; presel_qual_index++)
        {
            char presel_qual_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };

            /** In case of bcmFieldQualifyForwardingType we print the value and relevant enum string. */
            if (entry_data.qual_data[presel_qual_index].qual_type == bcmFieldQualifyForwardingType)
            {
                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                                      "Qual: %s\n  - Val: 0x%X (%s)\n  - Arg: %d\n  - Mask: 0x%X\n",
                                                      dnx_field_bcm_qual_text(unit, entry_data.qual_data
                                                                              [presel_qual_index].qual_type),
                                                      entry_data.qual_data[presel_qual_index].qual_value,
                                                      dnx_field_bcm_layer_type_text(entry_data.qual_data
                                                                                    [presel_qual_index].qual_value),
                                                      entry_data.qual_data[presel_qual_index].qual_arg,
                                                      entry_data.qual_data[presel_qual_index].qual_mask);

                sal_snprintf(presel_qual_buff, sizeof(presel_qual_buff),
                             "Qual: %s\n  - Val: 0x%X (%s)\n  - Arg: %d\n  - Mask: 0x%X\n",
                             dnx_field_bcm_qual_text(unit, entry_data.qual_data[presel_qual_index].qual_type),
                             entry_data.qual_data[presel_qual_index].qual_value,
                             dnx_field_bcm_layer_type_text(entry_data.qual_data[presel_qual_index].qual_value),
                             entry_data.qual_data[presel_qual_index].qual_arg,
                             entry_data.qual_data[presel_qual_index].qual_mask);
            }
            else
            {
                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                                      "Qual: %s\n  - Val: 0x%X\n  - Arg: %d\n  - Mask: 0x%X\n",
                                                      dnx_field_bcm_qual_text(unit, entry_data.qual_data
                                                                              [presel_qual_index].qual_type),
                                                      entry_data.qual_data[presel_qual_index].qual_value,
                                                      entry_data.qual_data[presel_qual_index].qual_arg,
                                                      entry_data.qual_data[presel_qual_index].qual_mask);

                sal_snprintf(presel_qual_buff, sizeof(presel_qual_buff),
                             "Qual: %s\n  - Val: 0x%X\n  - Arg: %d\n  - Mask: 0x%X\n",
                             dnx_field_bcm_qual_text(unit, entry_data.qual_data[presel_qual_index].qual_type),
                             entry_data.qual_data[presel_qual_index].qual_value,
                             entry_data.qual_data[presel_qual_index].qual_arg,
                             entry_data.qual_data[presel_qual_index].qual_mask);
            }
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(presel_quals_string, "%s", presel_qual_buff);
            sal_strncat_s(presel_quals_string, presel_qual_buff, sizeof(presel_quals_string));
        }

        /** Only in IPMF2 stage we should present the cascaded_from value. */
        if (field_stage == DNX_FIELD_STAGE_IPMF2
            && context_mode.context_ipmf2_mode.cascaded_from == DNX_FIELD_CONTEXT_ID_INVALID)
        {
            /**
             * Collect and set information about context to which, the presel is being set and
             * field groups, which are being attach to the same context.
             */
            SHR_IF_ERR_EXIT(diag_dnx_field_system_info_ctx_fg_print
                            (unit, field_stage, &entry_data_ipmf1, fg_string, prt_ctr));
            if (!(sal_strncmp(fg_string, "", sizeof(fg_string))))
            {
                continue;
            }
        }
        else
        {
            /**
             * Collect and set information about context to which, the presel is being set and
             * field groups, which are being attach to the same context.
             */
            SHR_IF_ERR_EXIT(diag_dnx_field_system_info_ctx_fg_print
                            (unit, field_stage, &entry_data, fg_string, prt_ctr));
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        if (!ipmf2_cotnext_print_flag)
        {
            PRT_CELL_SET("%d", presel_index);
            PRT_CELL_SET("%s", presel_quals_string);
        }
        else
        {
            PRT_CELL_SET("%s", "IPMF1 presel");
            PRT_CELL_SET("%s", "IPMF1 presel quals");
        }

        if (entry_data.context_id == BCM_FIELD_CONTEXT_ID_INVALID && field_stage == DNX_FIELD_STAGE_IPMF2)
        {
            /** Take the name for current context from the SW state. */
            sal_memset(context_name, 0, sizeof(context_name));
            SHR_IF_ERR_EXIT(dnx_field_context_sw.context_info.name.
                            value.stringget(unit, entry_data_ipmf1.context_id, DNX_FIELD_STAGE_IPMF1, context_name));
            /**
             * Check if Context name is being provided, in case no set N/A as default value,
             * in other case set the provided name.
             */
            if (sal_strncmp(context_name, "", sizeof(context_name)))
            {
                PRT_CELL_SET("%d / \"%s\"", entry_data_ipmf1.context_id, context_name);
            }
            else
            {
                PRT_CELL_SET("%d / \"N/A\"", entry_data_ipmf1.context_id);
            }
        }
        else
        {
            /** Take the name for current context from the SW state. */
            sal_memset(context_name, 0, sizeof(context_name));
            SHR_IF_ERR_EXIT(dnx_field_context_sw.context_info.name.
                            value.stringget(unit, entry_data.context_id, field_stage, context_name));
            /**
             * Check if Context name is being provided, in case no set N/A as default value,
             * in other case set the provided name.
             */
            if (sal_strncmp(context_name, "", sizeof(context_name)))
            {
                PRT_CELL_SET("%d / \"%s\"", entry_data.context_id, context_name);
            }
            else
            {
                PRT_CELL_SET("%d / \"N/A\"", entry_data.context_id);
            }
        }

        if (sal_strncmp(fg_string, "", sizeof(fg_string)))
        {
            PRT_CELL_SET("%s", fg_string);
        }
        else
        {
            PRT_CELL_SET("%s", "N/A");
        }

        /** Only in IPMF2 stage we should present the cascaded_from value. */
        if (field_stage == DNX_FIELD_STAGE_IPMF2)
        {

            if (context_mode.context_ipmf2_mode.cascaded_from != DNX_FIELD_CONTEXT_ID_INVALID)
            {
                PRT_CELL_SET("%d", context_mode.context_ipmf2_mode.cascaded_from);
            }
            else
            {
                PRT_CELL_SET("%s", "-");
            }
        }

        /** If any info was found for the given stage, then display the information about it. */
        *do_display_p = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays information about system view of the entire field lookup process for
 *   a certain stage, specified by the caller.
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
diag_dnx_field_system_info_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e field_stage;
    int do_display;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_STAGE, field_stage);

    SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                    (unit, Field_system_diag_stage_enum_table[field_stage].string, &field_stage));

    do_display = FALSE;

    PRT_TITLE_SET("Field System Lookup Flow Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Presel ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Presel Qual\n(value/arg/mask)");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "CTX ID/Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FG ID/Name");
    /** Only in IPMF2 we will present Cascaded from column with the relevant value. */
    if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Cascaded From");
    }

    /**
     * Prepare and print a table, which contains info about system view of the entire field lookup process for
     * a given FP stage.
     */
    SHR_IF_ERR_EXIT(diag_dnx_field_system_info_print(unit, field_stage, prt_ctr, &do_display));

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
        LOG_CLI_EX("\r\n" "No info was found for the given FP stage (%s)!! %s%s%s\r\n\n",
                   dnx_field_stage_text(unit, field_stage), EMPTY, EMPTY, EMPTY);
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays system header profiles configured in the device.
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
diag_dnx_field_system_header_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int profile_index;
    uint32 entry_handle_id;
    uint8 build_ftmh, build_pph, build_tsh, build_udh;
    char *profile_name;
    int do_display;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("System Header profiles");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "profile ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Build PPH");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Build FTMH");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Build TSH");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Build UDH");

    do_display = FALSE;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_INGRESS_HEADER_PROFILE, &entry_handle_id));
    for (profile_index = 0; profile_index < DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_NOF; profile_index++)
    {

        /*
         * Read data from HW and DNX-data
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HEADER_PROFILE, profile_index);
        dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_BUILD_FTMH, INST_SINGLE, &build_ftmh);
        dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_BUILD_OAM_TS_HEADER, INST_SINGLE, &build_tsh);
        dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_BUILD_PPH, INST_SINGLE, &build_pph);
        dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_BUILD_UDH, INST_SINGLE, &build_udh);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        profile_name = dnx_data_field.system_headers.system_header_profiles_get(unit, profile_index)->name;
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%d", profile_index);

        PRT_CELL_SET("%s", profile_name);

        PRT_CELL_SET("%d", build_pph);
        PRT_CELL_SET("%d", build_ftmh);
        PRT_CELL_SET("%d", build_tsh);
        PRT_CELL_SET("%d", build_udh);

        do_display = TRUE;

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
        LOG_CLI_EX("\r\n" "NO system header profiles were found on device!! %s%s%s%s\r\n\n", EMPTY,
                   EMPTY, EMPTY, EMPTY);
    }

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Globals required at end because they reference static procedures/tables above.
 * {
 */
/**
 * \brief
 *   List of shell options for 'system' shell commands (info)
 */
sh_sand_cmd_t Sh_dnx_field_system_cmds[] = {
    {"info", diag_dnx_field_system_info_cb, NULL, Field_system_info_options, &Field_system_info_man}
    ,
    {"header", diag_dnx_field_system_header_cb, NULL, NULL, &Field_system_header_man}
    ,
    {NULL}
};

/*
 * }
 */
