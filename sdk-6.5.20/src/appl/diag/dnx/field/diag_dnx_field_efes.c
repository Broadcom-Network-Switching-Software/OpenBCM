/** \file diag_dnx_field_efes.c
 *
 * Diagnostics procedures, for DNX, for 'efes' operations.
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
#include "diag_dnx_field_efes.h"
#include <bcm_int/dnx/field/field_context.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_context_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_group_access.h>
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
 *  'Help' description for EFES List (shell commands).
 */
static sh_sand_man_t Field_efes_list_man = {
    .brief = "'EFES List'- displays EFES programs, masks and usage-status configurations for a certain stage (filter by field stage).",
    .full = "'EFES List' displays EFES programs, masks and usage-status configurations for a certain stage.\r\n"
            "If no stage 'IPMF1'-'EPMF' is specified then an error will occur.\r\n",
    .synopsis = "[stage=<IPMF1 | IPMF2 | IPMF3 | EPMF>]",
    .examples = "stage=IPMF1",
};
static sh_sand_option_t Field_efes_list_options[] = {
    {DIAG_DNX_FIELD_OPTION_STAGE,  SAL_FIELD_TYPE_UINT32, "Field stage to present EFES info for.",   NULL,    (void *)Field_efes_diag_stage_enum_table, "STAGE_LOWEST-STAGE_HIGHEST"},
    {NULL}
};

/**
 *  'Help' description for EFES Info (shell commands).
 */
static sh_sand_man_t Field_efes_info_man = {
    .brief = "'EFES Info'- displays EFES detailed information for a certain stage (filter by field stage, program, array and position).",
    .full = "'EFES Info' displays EFES detailed information for a certain stage.\r\n"
            "If no stage 'IPMF1'-'EPMF' is specified then an error will occur.\r\n"
            "If no program 'IPMF1/2/3/EPMF (0-31)' is specified then an error will occur.\r\n"
            "If no array 'IPMF1/2 (0-1), IPMF3/EPMF (0)' is specified then an error will occur.\r\n"
            "If no position 'IPMF1/2/3 (0-15), EPMF (0-11)' is specified then an error will occur.\r\n",
    .synopsis = "[stage=<IPMF1 | IPMF2 | IPMF3 | EPMF>]"
                "[program=<Can be 'IPMF1/2/3/EPMF (0-31)'>]"
                "[array=<Can be 'IPMF1/2 (0-1), IPMF3/EPMF (0)'>]"
                "[position=<Can be 'IPMF1/2/3 (0-15), EPMF (0-11)'>]",
    .examples = "stage=IPMF2 program=1 array=0 position=7",
};
static sh_sand_option_t Field_efes_info_options[] = {
    {DIAG_DNX_FIELD_OPTION_STAGE,  SAL_FIELD_TYPE_UINT32, "Field stage to present EFES info for.",   NULL,           (void *)Field_efes_diag_stage_enum_table, "STAGE_LOWEST-STAGE_HIGHEST"},
    {DIAG_DNX_FIELD_OPTION_PROGRAM,  SAL_FIELD_TYPE_INT32, "EFES program to present EFES info for.",   NULL,           NULL},
    {DIAG_DNX_FIELD_OPTION_ARRAY,  SAL_FIELD_TYPE_INT32, "EFES array to present EFES info for.",   NULL,           NULL},
    {DIAG_DNX_FIELD_OPTION_POSITION,  SAL_FIELD_TYPE_INT32, "EFES position to present EFES info for.",   NULL,           NULL},
    {NULL}
};

/**
 *  'Help' description for EFES Context (shell commands).
 */
static sh_sand_man_t Field_efes_ctx_man = {
    .brief = "'EFES context'- displays list of context and relevant EFES CTX information for a certain stage (filter by field stage).",
    .full = "'EFES context' displays list of context and relevant EFES CTX information for a certain stage.\r\n"
            "If no stage 'IPMF1'-'EPMF' is specified then an error will occur.\r\n",
    .synopsis = "[stage=<IPMF1 | IPMF2 | IPMF3 | EPMF>]",
    .examples = "stage=IPMF1",
};
static sh_sand_option_t Field_efes_ctx_options[] = {
    {DIAG_DNX_FIELD_OPTION_STAGE,  SAL_FIELD_TYPE_UINT32, "Field stage to present EFES CTX info for.",   NULL,           (void *)Field_efes_diag_stage_enum_table, "STAGE_LOWEST-STAGE_HIGHEST"},
    {NULL}
};

/**
 *  'Help' description for EFES List Programs (shell commands).
 */
static sh_sand_man_t Field_efes_list_programs_man = {
    .brief = "'EFES List Programs'- displays EFES programs info for a certain stage (filter by field stage).",
    .full = "'EFES List Programs' displays all contexts and field groups that use a specific EFES ID, and their EFES program ID for a certain stage.\r\n"
            "If no stage 'IPMF1'-'EPMF' is specified then an error will occur.\r\n",
    .synopsis = "[stage=<IPMF1 | IPMF2 | IPMF3 | EPMF>]",
    .examples = "stage=IPMF1",
};

/**
 *  'Help' description for EFES List Masks (shell commands).
 */
static sh_sand_man_t Field_efes_list_masks_man = {
    .brief = "'EFES List Mask'- displays EFES masks programs, context and values usage for a certain stage (filter by field stage).",
    .full = "'EFES List Mask' displays EFES masks programs, context and values usage for a certain stage.\r\n"
            "If no stage 'IPMF1'-'EPMF' is specified then an error will occur.\r\n",
    .synopsis = "[stage=<IPMF1 | IPMF2 | IPMF3 | EPMF>]",
    .examples = "stage=IPMF1",
};

/**
 *  'Help' description for EFES List Status (shell commands).
 */
static sh_sand_man_t Field_efes_list_status_man = {
    .brief = "'EFES List Status'- displays the number of EFES programs and masks used by each EFES ID for a certain stage (filter by field stage).",
    .full = "'EFES List Status' displays the number of EFES programs and masks used by each EFES ID for a certain stage.\r\n"
            "If no stage 'IPMF1'-'EPMF' is specified then an error will occur.\r\n",
    .synopsis = "[stage=<IPMF1 | IPMF2 | IPMF3 | EPMF>]",
    .examples = "stage=IPMF1",
};

/**
 *  'Help' description for EFES List Masks Program (shell commands).
 */
static sh_sand_man_t Field_efes_list_masks_program_man = {
    .brief = "'EFES List Mask Program'- displays EFES program usage by EFES MASK IDs for a certain stage (filter by field stage).",
    .full = "'EFES List Mask Program' displays EFES program usage by EFES MASK IDs for a certain stage.\r\n"
            "If no stage 'IPMF1'-'EPMF' is specified then an error will occur.\r\n",
    .synopsis = "[stage=<IPMF1 | IPMF2 | IPMF3 | EPMF>]",
    .examples = "stage=IPMF1",
};

/**
 *  'Help' description for EFES List Masks Context (shell commands).
 */
static sh_sand_man_t Field_efes_list_masks_context_man = {
    .brief = "'EFES List Mask Context'- displays context usage by FES MASK IDs for a certain stage (filter by field stage).",
    .full = "'EFES List Mask Context' displays context usage by FES MASK IDs for a certain stage.\r\n"
            "If no stage 'IPMF1'-'EPMF' is specified then an error will occur.\r\n",
    .synopsis = "[stage=<IPMF1 | IPMF2 | IPMF3 | EPMF>]",
    .examples = "stage=IPMF1",
};

/**
 *  'Help' description for EFES List Masks Value (shell commands).
 */
static sh_sand_man_t Field_efes_list_masks_value_man = {
    .brief = "'EFES List Mask Value'- displays EFES MASKs values for a certain stage (filter by field stage).",
    .full = "'EFES List Mask Value' displays EFES MASKs values for a certain stage.\r\n"
            "If no stage 'IPMF1'-'EPMF' is specified then an error will occur.\r\n",
    .synopsis = "[stage=<IPMF1 | IPMF2 | IPMF3 | EPMF>]",
    .examples = "stage=IPMF1",
};

/**
 *  'Help' description for EFES added (shell commands).
 */
static sh_sand_man_t Field_efes_added_man = {
    .brief = "'EFES added'- displays list of added EFESs and relevant information for a certain group/context (filter by field group/context).",
    .full = "'EFES added' displays list of added EFESs and relevant information for a certain group/context.\r\n"
            "If no 'group' ('0'-'nof_fgs-1' --> (0-127)) is specified then info for all groups will be shown.\r\n"
            "If no 'context' ('0'-'DNX_DATA.field.common_max_val.nof_contexts-1' is assumed (0-63)) is specified then info for all contexts will be shown.\r\n",
    .synopsis = "[group=0-127]"
                "[context=0-63]",
    .examples = "group=0 context=0",
};
static sh_sand_option_t Field_efes_added_options[] = {
    {DIAG_DNX_FIELD_OPTION_GROUP,  SAL_FIELD_TYPE_UINT32, "Field group to present added EFES info for.",   "0xFFFFFFFF",           (void *)Field_group_enum_table, "MIN-MAX"},
    {DIAG_DNX_FIELD_OPTION_CONTEXT,  SAL_FIELD_TYPE_UINT32, "Field context to present added EFES info for.",   "0xFFFFFFFF"},
    {NULL}
};

/*
 * }
 */
/* *INDENT-ON* */

/**
 * \brief
 *   This function displays all contexts and field groups that use a specific EFES ID, and their
 *   EFES program ID for a certain stage, specified by the caller.
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
diag_dnx_field_efes_list_programs_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e field_stage;
    uint8 do_display;
    uint8 add_new_fes_id;

    uint32 fes_id_iter;
    uint32 fes_pgm_iter;
    uint32 fes_mask_iter;
    int array, position;
    dnx_field_actions_fes_id_get_info_t *fes_id_info = NULL;
    bcm_field_stage_t bcm_stage;

    int ctx_iter;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_STAGE, field_stage);
    SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                    (unit, Field_efes_diag_stage_enum_table[field_stage].string, &field_stage));

    SHR_ALLOC(fes_id_info, sizeof(dnx_field_actions_fes_id_get_info_t), "fes_id_info", "%s%s%s\r\n", EMPTY, EMPTY,
              EMPTY);

    PRT_TITLE_SET("EFES List - programs");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES array");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES offset");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES program");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FG ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES mask IDs Used");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "PMF Context");

    do_display = FALSE;
    add_new_fes_id = FALSE;

    /** Iterates over max number of FESs for the given stage and retrieve/present relevant info. */
    for (fes_id_iter = 0;
         fes_id_iter < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_iter++)
    {
        /** Get information about the current FES ID, like FES PGM, contexts, FGs. */
        SHR_IF_ERR_EXIT(dnx_field_actions_fes_id_info_get(unit, field_stage, fes_id_iter, fes_id_info));

        /**
         * On every new iteration of FES IDs we should add new line with underscore separator.
         * For this case we are using following flag add_new_fes_id, which is being set to TRUE for this purpose.
         */
        add_new_fes_id = TRUE;
        /** Iterate over all FES programs and present returned information from fes_id_info. */
        for (fes_pgm_iter = 0; fes_pgm_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS; fes_pgm_iter++)
        {
            /**
             * Define following strings to store information about used
             * EFES masks and Contexts by the current FES program.
             */
            char ctx_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
            char efes_masks_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };

            /**
             * Check if any bit of the context bitmap for the current FES program is being set.
             * If not we are not going to present information about this FES PGM.
             */
            if (!
                (utilex_bitstream_have_one
                 (fes_id_info->fes_pgm_info[fes_pgm_iter].context_bmp,
                  BITS2WORDS(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS))))
            {
                continue;
            }

            /** In case the flag is set, we should set value for EFES ID, EFES array and EFES position on this roll. */
            if (add_new_fes_id)
            {
                array = 0;
                position = 0;

                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                /** Set 'EFES ID' cell. */
                PRT_CELL_SET("%d", fes_id_iter);

                /** Get the array and position of the given FES ID. */
                DNX_FIELD_ACTION_FES_ID_ARRAY_POSITION_GET(array, position, unit, field_stage, fes_id_iter);

                /** Set 'EFES array' cell. */
                PRT_CELL_SET("%d", array);
                /** Set 'EFES offset' cell. */
                PRT_CELL_SET("%d", position);

                /** Set the flag to FALSE, if we have more then one EFES PGM for the current EFES, we should skip those 3 cells. */
                add_new_fes_id = FALSE;
            }
            else
            {
                /** In case the flag is not set we should skip cells related to EFES ID, EFES array and EFES position. */
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                /** Skip 'EFES ID', 'EFES array' and 'EFES offset' cells. */
                PRT_CELL_SKIP(3);
            }

            /** Set 'EFES program' cell. */
            PRT_CELL_SET("%d", fes_pgm_iter);
            /** Set 'FG ID' cell. */
            PRT_CELL_SET("%d", fes_id_info->fes_pgm_info[fes_pgm_iter].field_group);

            /** Iterate over all EFES masks for the current EFES PGM. */
            for (fes_mask_iter = 0; fes_mask_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES;
                 fes_mask_iter++)
            {
                /**
                 * Check if current mask is not zero one and the current element of  the returned mask_id_used[] array is TRUE,
                 * then add the current mask to the final string to be presented.
                 */
                if (fes_mask_iter != DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID
                    && fes_id_info->fes_pgm_info[fes_pgm_iter].masks_id_used[fes_mask_iter])
                {
                    char fes_mask_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
                    DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%d, ", fes_mask_iter);
                    sal_snprintf(fes_mask_buff, sizeof(fes_mask_buff), "%d, ", fes_mask_iter);
                    DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(efes_masks_string, "%s", fes_mask_buff);
                    sal_strncat_s(efes_masks_string, fes_mask_buff, sizeof(efes_masks_string));
                }
            }
            /** If the EFES mask string is not empty we should set its value to the relevant cell. */
            if (sal_strncmp(efes_masks_string, "", sizeof(efes_masks_string)))
            {
                /**
                 * Extract last 2 symbols of the constructed string
                 * to not present comma and space ", " at the end of it.
                 */
                efes_masks_string[sal_strlen(efes_masks_string) - 2] = '\0';
                /** Set 'EFES mask IDs Used' cell. */
                PRT_CELL_SET("%s", efes_masks_string);
            }
            else
            {
                /** Set 'EFES mask IDs Used' cell. */
                PRT_CELL_SET("None");
            }

            /** Iterate over all Contexts and check which are relevant for the current EFES PGM. */
            for (ctx_iter = 0; ctx_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS; ctx_iter++)
            {
                /**
                 * Check if the current bit of the context bitmap is set then,
                 * add the current context id to the final string to be presented.
                 */
                if (utilex_bitstream_test_bit(fes_id_info->fes_pgm_info[fes_pgm_iter].context_bmp, ctx_iter))
                {
                    char ctx_id_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
                    DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%d, ", ctx_iter);
                    sal_snprintf(ctx_id_buff, sizeof(ctx_id_buff), "%d, ", ctx_iter);
                    DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(ctx_string, "%s", ctx_id_buff);
                    sal_strncat_s(ctx_string, ctx_id_buff, sizeof(ctx_string));
                }
            }
            /**
             * Extract last 2 symbols of the constructed string
             * to not present comma and space ", " at the end of it.
             */
            ctx_string[sal_strlen(ctx_string) - 2] = '\0';
            /** Set 'PMF Context' cell. */
            PRT_CELL_SET("%s", ctx_string);

            /** In case we reach this line, it means that there is some information to be presented. */
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
        /** Convert DNX to BCM Field Stage */
        SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage));
        LOG_CLI_EX("\r\n" "No EFES info was found for the given FP stage (%s)!! %s%s%s\r\n\n",
                   dnx_field_bcm_stage_text(bcm_stage), EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FREE(fes_id_info);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays EFES Program usage by EFES MASK IDs for a certain stage, specified by the caller.
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
diag_dnx_field_efes_list_masks_program_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e field_stage;
    uint8 do_display;
    uint8 add_new_fes_id;

    uint32 fes_id_iter;
    uint32 fes_pgm_iter;
    uint32 fes_mask_iter;
    int array, position;
    dnx_field_actions_fes_id_get_info_t *fes_id_info = NULL;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_STAGE, field_stage);
    SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                    (unit, Field_efes_diag_stage_enum_table[field_stage].string, &field_stage));

    SHR_ALLOC(fes_id_info, sizeof(dnx_field_actions_fes_id_get_info_t), "fes_id_info", "%s%s%s\r\n", EMPTY, EMPTY,
              EMPTY);

    PRT_TITLE_SET("EFES status - masks program usage");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES array");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES offset");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES Mask ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Used by EFES programs");

    do_display = FALSE;

    /** Iterates over max number of FESs for the given stage and retrieve/present relevant info. */
    for (fes_id_iter = 0;
         fes_id_iter < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_iter++)
    {
        /**
         * On every new iteration of FES IDs we should add new line with underscore separator.
         * For this case we are using following flag add_new_fes_id, which is being set to TRUE for this purpose.
         */
        add_new_fes_id = TRUE;

        /** Get information about the current FES ID, like FES PGM, contexts, FGs. */
        SHR_IF_ERR_EXIT(dnx_field_actions_fes_id_info_get(unit, field_stage, fes_id_iter, fes_id_info));

        /** Iterate over all EFES masks. */
        for (fes_mask_iter = 0; fes_mask_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES; fes_mask_iter++)
        {
            /**
             * Define following string to store information about used EFES PGMs by the current EFES mask.
             */
            char efes_pgm_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };

            /** Iterate over all FES programs. */
            for (fes_pgm_iter = 0; fes_pgm_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS; fes_pgm_iter++)
            {
                /**
                 * Check if any bit of the context bitmap for the current FES program is being set.
                 * If not we are not going to present information about this FES PGM.
                 */
                if (!
                    (utilex_bitstream_have_one
                     (fes_id_info->fes_pgm_info[fes_pgm_iter].context_bmp,
                      BITS2WORDS(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS))))
                {
                    continue;
                }

                /** Check if the current EFES mask id is TRUE for the current EFES program. */
                if (fes_id_info->fes_pgm_info[fes_pgm_iter].masks_id_used[fes_mask_iter])
                {
                    char fes_pgm_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
                    DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%d, ", fes_pgm_iter);
                    sal_snprintf(fes_pgm_buff, sizeof(fes_pgm_buff), "%d, ", fes_pgm_iter);
                    DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(efes_pgm_string, "%s", fes_pgm_buff);
                    sal_strncat_s(efes_pgm_string, fes_pgm_buff, sizeof(efes_pgm_string));
                }
            }

            /** If the EFES PGM string is not empty we should set its value to the relevant cell. */
            if (sal_strncmp(efes_pgm_string, "", sizeof(efes_pgm_string)))
            {
                /** In case the flag is set, we should set value for EFES ID, EFES array and EFES position on this roll. */
                if (add_new_fes_id)
                {
                    array = 0;
                    position = 0;

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    /** Set 'EFES ID' cell. */
                    PRT_CELL_SET("%d", fes_id_iter);

                    /** Get the array and position of the given FES ID. */
                    DNX_FIELD_ACTION_FES_ID_ARRAY_POSITION_GET(array, position, unit, field_stage, fes_id_iter);

                    /** Set 'EFES array' cell. */
                    PRT_CELL_SET("%d", array);
                    /** Set 'EFES offset' cell. */
                    PRT_CELL_SET("%d", position);

                    /** Set the flag to FALSE, if we have more then one EFES PGM for the current EFES, we should skip those 3 cells. */
                    add_new_fes_id = FALSE;
                }
                else
                {
                    /** In case the flag is not set we should skip cells related to EFES ID, EFES array and EFES position. */
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    /** Skip 'EFES ID', 'EFES array' and 'EFES offset' cells. */
                    PRT_CELL_SKIP(3);
                }

                /** Set 'EFES Mask ID' cell. */
                PRT_CELL_SET("%d", fes_mask_iter);
                /**
                 * Extract last 2 symbols of the constructed string
                 * to not present comma and space ", " at the end of it.
                 */
                efes_pgm_string[sal_strlen(efes_pgm_string) - 2] = '\0';
                /** Set 'Used by EFES programs' cell. */
                PRT_CELL_SET("%s", efes_pgm_string);

                /** In case we reach this line, it means that there is some information to be presented. */
                do_display = TRUE;
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
        LOG_CLI_EX("\r\n" "No EFES info was found on specified filters!! %s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FREE(fes_id_info);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays context usahe by EFES MASK IDs for a certain stage, specified by the caller.
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
diag_dnx_field_efes_list_masks_context_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e field_stage;
    uint8 do_display;
    uint8 add_new_fes_id;

    uint32 fes_id_iter;
    uint32 fes_pgm_iter;
    uint32 fes_mask_iter;
    int array, position;
    dnx_field_actions_fes_id_get_info_t *fes_id_info = NULL;

    int ctx_iter;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_STAGE, field_stage);
    SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                    (unit, Field_efes_diag_stage_enum_table[field_stage].string, &field_stage));

    SHR_ALLOC(fes_id_info, sizeof(dnx_field_actions_fes_id_get_info_t), "fes_id_info", "%s%s%s\r\n", EMPTY, EMPTY,
              EMPTY);

    PRT_TITLE_SET("EFES status - masks context usage");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES array");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES offset");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES Mask ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Used by Contexts");

    do_display = FALSE;

    /** Iterates over max number of FESs for the given stage and retrieve/present relevant info. */
    for (fes_id_iter = 0;
         fes_id_iter < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_iter++)
    {
        /**
         * On every new iteration of FES IDs we should add new line with underscore separator.
         * For this case we are using following flag add_new_fes_id, which is being set to TRUE for this purpose.
         */
        add_new_fes_id = TRUE;

        /** Get information about the current FES ID, like FES PGM, contexts, FGs. */
        SHR_IF_ERR_EXIT(dnx_field_actions_fes_id_info_get(unit, field_stage, fes_id_iter, fes_id_info));

        /** Iterate over all EFES masks. */
        for (fes_mask_iter = 0; fes_mask_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES; fes_mask_iter++)
        {
            /**
             * Define following string to store information about used Context by the current EFES mask.
             */
            char efes_ctx_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };

            /** Iterate over all EFES PGMs. */
            for (fes_pgm_iter = 0; fes_pgm_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS; fes_pgm_iter++)
            {
                /**
                 * Check if any bit of the context bitmap for the current FES program is being set.
                 * If not we are not going to present information about this FES PGM.
                 */
                if (!
                    (utilex_bitstream_have_one
                     (fes_id_info->fes_pgm_info[fes_pgm_iter].context_bmp,
                      BITS2WORDS(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS))))
                {
                    continue;
                }

                /** Check if the current EFES mask id is TRUE for the current EFES program. */
                if (fes_id_info->fes_pgm_info[fes_pgm_iter].masks_id_used[fes_mask_iter])
                {
                    /** Iterate over all Contexts and check which are relevant for the current EFES PGM. */
                    for (ctx_iter = 0; ctx_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS; ctx_iter++)
                    {
                        /**
                         * Check if the current bit of the context bitmap is set then,
                         * add the current context id to the final string to be presented.
                         */
                        if (utilex_bitstream_test_bit(fes_id_info->fes_pgm_info[fes_pgm_iter].context_bmp, ctx_iter))
                        {
                            char fes_cs_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
                            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%d, ", ctx_iter);
                            sal_snprintf(fes_cs_buff, sizeof(fes_cs_buff), "%d, ", ctx_iter);
                            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(efes_ctx_string, "%s", fes_cs_buff);
                            sal_strncat_s(efes_ctx_string, fes_cs_buff, sizeof(efes_ctx_string));
                        }
                    }
                }
            }

            /** If the EFES CTX string is not empty we should set its value to the relevant cells. */
            if (sal_strncmp(efes_ctx_string, "", sizeof(efes_ctx_string)))
            {
                 /** In case the flag is set, we should set value for EFES ID, EFES array and EFES position on this roll. */
                if (add_new_fes_id)
                {
                    array = 0;
                    position = 0;

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    /** Set 'EFES ID' cell. */
                    PRT_CELL_SET("%d", fes_id_iter);

                    /** Get the array and position of the given FES ID. */
                    DNX_FIELD_ACTION_FES_ID_ARRAY_POSITION_GET(array, position, unit, field_stage, fes_id_iter);

                    /** Set 'EFES array' cell. */
                    PRT_CELL_SET("%d", array);
                    /** Set 'EFES offset' cell. */
                    PRT_CELL_SET("%d", position);

                    /** Set the flag to FALSE, if we have more then one EFES PGM for the current EFES, we should skip those 3 cells. */
                    add_new_fes_id = FALSE;
                }
                else
                {
                    /** In case the flag is not set we should skip cells related to EFES ID, EFES array and EFES position. */
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    /** Skip 'EFES ID', 'EFES array' and 'EFES offset' cells. */
                    PRT_CELL_SKIP(3);
                }

                /** Set 'EFES Mask ID' cell. */
                PRT_CELL_SET("%d", fes_mask_iter);
                /**
                 * Extract last 2 symbols of the constructed string
                 * to not present comma and space ", " at the end of it.
                 */
                efes_ctx_string[sal_strlen(efes_ctx_string) - 2] = '\0';
                /** Set 'Used by Contexts' cell. */
                PRT_CELL_SET("%s", efes_ctx_string);

                /** In case we reach this line, it means that there is some information to be presented. */
                do_display = TRUE;
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
        LOG_CLI_EX("\r\n" "No EFES info was found on specified filters!! %s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FREE(fes_id_info);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays EFES MASKs values for a certain stage, specified by the caller.
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
diag_dnx_field_efes_list_masks_value_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e field_stage;
    uint8 do_display;

    uint32 fes_id_iter;
    uint8 fes_mask_iter;
    int array, position;
    dnx_field_actions_fes_id_get_info_t *fes_id_info = NULL;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_STAGE, field_stage);
    SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                    (unit, Field_efes_diag_stage_enum_table[field_stage].string, &field_stage));

    SHR_ALLOC(fes_id_info, sizeof(dnx_field_actions_fes_id_get_info_t), "fes_id_info", "%s%s%s\r\n", EMPTY, EMPTY,
              EMPTY);

    PRT_TITLE_SET("EFES status - masks value");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES array");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES offset");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES Mask 0\n(Always zero)");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES Mask 1");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES Mask 2");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES Mask 3");

    do_display = FALSE;

    /** Iterates over max number of FESs for the given stage and retrieve/present relevant info. */
    for (fes_id_iter = 0;
         fes_id_iter < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_iter++)
    {
        /** Get information about the current FES ID, like FES PGM, contexts, FGs. */
        SHR_IF_ERR_EXIT(dnx_field_actions_fes_id_info_get(unit, field_stage, fes_id_iter, fes_id_info));

        /**
         * Check if any bit of the action_masks[] array for the current FES program is being set.
         * If not we are not going to present information about this FES PGM.
         */
        if (!
            (utilex_bitstream_have_one(fes_id_info->action_masks, DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES)))
        {
            continue;
        }

        array = 0;
        position = 0;

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        /** Set 'EFES ID' cell. */
        PRT_CELL_SET("%d", fes_id_iter);

        /** Get the array and position of the given FES ID. */
        DNX_FIELD_ACTION_FES_ID_ARRAY_POSITION_GET(array, position, unit, field_stage, fes_id_iter);

        /** Set 'EFES array' cell. */
        PRT_CELL_SET("%d", array);
        /** Set 'EFES offset' cell. */
        PRT_CELL_SET("%d", position);

        /** Iterate over all EFES masks for the current EFES PGM. */
        for (fes_mask_iter = 0;
             fes_mask_iter < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes; fes_mask_iter++)
        {
            /**
             * Check if current mask is not zero one and the current element of the returned action_masks[] array is zero mask,
             * then we count this mask as "Unused" one.
             * In other cases present masks values.
             */
            if (fes_mask_iter != DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID
                && fes_id_info->action_masks[fes_mask_iter] == DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK)
            {
                /** Set 'EFES Mask 0\n(Always zero)', 'EFES Mask 1', 'EFES Mask 2' and 'EFES Mask 3' cells. */
                PRT_CELL_SET("Unused");
            }
            else
            {
                /** Set 'EFES Mask 0\n(Always zero)', 'EFES Mask 1', 'EFES Mask 2' and 'EFES Mask 3' cells. */
                PRT_CELL_SET("0x%08X", fes_id_info->action_masks[fes_mask_iter]);
            }
        }
        /** In case we reach this line, it means that there is some information to be presented. */
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
        LOG_CLI_EX("\r\n" "No EFES info was found on specified filters!! %s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FREE(fes_id_info);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays the number of EFES programs and masks used by each EFES ID for
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
diag_dnx_field_efes_list_status_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e field_stage;
    uint8 do_display;

    uint32 fes_id_iter;
    uint32 fes_pgm_iter;
    uint8 fes_mask_iter;
    int array, position;
    dnx_field_actions_fes_id_get_info_t *fes_id_info = NULL;

    uint32 nof_used_fes_pgm;
    uint8 nof_used_fes_masks;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_STAGE, field_stage);
    SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                    (unit, Field_efes_diag_stage_enum_table[field_stage].string, &field_stage));

    SHR_ALLOC(fes_id_info, sizeof(dnx_field_actions_fes_id_get_info_t), "fes_id_info", "%s%s%s\r\n", EMPTY, EMPTY,
              EMPTY);

    PRT_TITLE_SET("EFES status - program and masks usage");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES array");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES offset");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES Programs used");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES Programs free");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES Masks used");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES Masks free");

    do_display = FALSE;

    /** Iterates over max number of FESs for the given stage and retrieve/present relevant info. */
    for (fes_id_iter = 0;
         fes_id_iter < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_iter++)
    {
        /**
         * Using following variable to indicate which masks are being used by the current EFES.
         * Set the default value to be FALSE (0), it will be updated later.
         */
        uint8 used_masks[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES] = { 0 };
        /** Using following variables to store the max value of used EFES PGMs and Masks. */
        nof_used_fes_pgm = 0;
        nof_used_fes_masks = 0;

        /** Get information about the current FES ID, like FES PGM, contexts, FGs. */
        SHR_IF_ERR_EXIT(dnx_field_actions_fes_id_info_get(unit, field_stage, fes_id_iter, fes_id_info));

        /** Iterate over all FES programs. */
        for (fes_pgm_iter = 0; fes_pgm_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS; fes_pgm_iter++)
        {
            /**
             * Check if any bit of the context bitmap for the current FES program is being set.
             * If not we are not going to present information about this FES PGM.
             */
            if (!
                (utilex_bitstream_have_one
                 (fes_id_info->fes_pgm_info[fes_pgm_iter].context_bmp,
                  BITS2WORDS(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS))))
            {
                continue;
            }

            /** In case we pass the above check, we should increase the number of used EFES PGMs. */
            nof_used_fes_pgm++;

            /** Iterate over all EFES masks for the current EFES PGM. */
            for (fes_mask_iter = 0;
                 fes_mask_iter < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes;
                 fes_mask_iter++)
            {
                /**
                 * Check if the current mask and the current element of the returned action_masks[] array are not zero one.
                 * Check the current element of the internal array of used_masks[], which indicated that current mask id is being
                 * used by one of the EFES programs.
                 * Increase the number of used EFES masks.
                 */
                if (fes_mask_iter != DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID
                    && fes_id_info->action_masks[fes_mask_iter] != DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK
                    && used_masks[fes_mask_iter] == FALSE)
                {
                    /**
                     * Set the current element of the used_masks[] array to TRUE, to indicate that this mask was already
                     * counted for the current EFES PGM.
                     */
                    used_masks[fes_mask_iter] = TRUE;
                    nof_used_fes_masks++;
                }
            }
        }

        /** Check if the nof_used_fes_pgm is different than 0, then present information about current EFES ID. */
        if (nof_used_fes_pgm)
        {
            array = 0;
            position = 0;

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            /** Set 'EFES ID' cell. */
            PRT_CELL_SET("%d", fes_id_iter);

            /** Get the array and position of the given FES ID. */
            DNX_FIELD_ACTION_FES_ID_ARRAY_POSITION_GET(array, position, unit, field_stage, fes_id_iter);

            /** Set 'EFES array' cell. */
            PRT_CELL_SET("%d", array);
            /** Set 'EFES offset' cell. */
            PRT_CELL_SET("%d", position);

            /** Set 'EFES Programs used' cell. */
            PRT_CELL_SET("%d", nof_used_fes_pgm);
            /** Set 'EFES Programs free' cell. */
            PRT_CELL_SET("%d", (DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS - nof_used_fes_pgm));
            /** Set 'EFES Masks used' cell. */
            PRT_CELL_SET("%d", nof_used_fes_masks);
            /** Set 'EFES Masks free' cell. */
            PRT_CELL_SET("%d",
                         ((dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes - 1) -
                          nof_used_fes_masks));

            /** In case we reach this line, it means that there is some information to be presented. */
            do_display = TRUE;
        }
        else
        {
            /** If no programs were detected for the current EFES ID, continue to the next one. */
            continue;
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
        LOG_CLI_EX("\r\n" "No EFES info was found on specified filters!! %s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FREE(fes_id_info);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays EFES detailed information for a certain stage, specified by the caller.
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
diag_dnx_field_efes_info_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e field_stage;

    dnx_field_actions_fes_id_get_info_t *fes_id_info = NULL;
    uint32 fes_id, fes_pgm_id;
    uint8 fes2msb_id_ndx;
    int fes_array, fes_position;

    int nof_fes_id_in_array;
    int nof_fes_array;
    int nof_fes_pgm;
    int nof_fem_array;

    unsigned int action_size;

    int ctx_iter;

    dnx_field_actions_fes_quartet_get_info_fes2msb_t fes2msb_info;
    dnx_field_action_t dnx_action;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_STAGE, field_stage);
    SH_SAND_GET_INT32(DIAG_DNX_FIELD_OPTION_PROGRAM, fes_pgm_id);
    SH_SAND_GET_INT32(DIAG_DNX_FIELD_OPTION_ARRAY, fes_array);
    SH_SAND_GET_INT32(DIAG_DNX_FIELD_OPTION_POSITION, fes_position);
    SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                    (unit, Field_efes_diag_stage_enum_table[field_stage].string, &field_stage));

    SHR_ALLOC(fes_id_info, sizeof(dnx_field_actions_fes_id_get_info_t), "fes_id_info", "%s%s%s\r\n", EMPTY, EMPTY,
              EMPTY);

    /** Get the DNX data for the given field stage. */
    nof_fes_id_in_array = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_id_per_array;
    nof_fes_array = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_array;
    nof_fes_pgm = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_programs;
    nof_fem_array = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fem_array;
    if ((nof_fes_array <= 0) || (nof_fes_id_in_array <= 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n" "Stage \"%s\" does not have any EFES.\r\n\n", dnx_field_stage_text(unit, field_stage));
    }

    /** Check the input parameters EFES program/array/position, if they are valid or no. */
    if (fes_pgm_id >= nof_fes_pgm)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Given EFES PGM (%d) is invalid for FP stage (%s)! EFES PGM can be between (%d) and (%d). \r\n\n",
                     fes_pgm_id, dnx_field_stage_text(unit, field_stage), 0, (nof_fes_pgm - 1));
    }
    if (fes_array < 0 || fes_array >= (nof_fes_array + nof_fem_array))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Given array_id (%d) is invalid for FP stage (%s)! Array id can be between (%d) and (%d). \r\n\n",
                     fes_array, dnx_field_stage_text(unit, field_stage), 0, (nof_fes_array + nof_fem_array - 1));
    }
    if (dnx_data_field.common_max_val.array_id_type_get(unit, fes_array)->is_fes == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n" "Given array_id (%d) is not an EFES array. \r\n\n", fes_array);
    }
    if (fes_position < 0 || fes_position >= nof_fes_id_in_array)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Given position (%d) is invalid for FP stage (%s)! Position can be between (%d) and (%d). \r\n\n",
                     fes_position, dnx_field_stage_text(unit, field_stage), 0, (nof_fes_id_in_array - 1));
    }

    fes_id =
        (dnx_data_field.common_max_val.array_id_type_get(unit, fes_array)->fes_array_index * nof_fes_id_in_array) +
        fes_position;

    /** Get information about the current EFES ID, like EFES PGM, contexts, FGs. */
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_id_info_get(unit, field_stage, fes_id, fes_id_info));

    /**
     * Check if any bit of the context bitmap for the given EFES program is being set.
     * If not we are counting this EFES PGM as invalid.
     */
    if (!(utilex_bitstream_have_one(fes_id_info->fes_pgm_info[fes_pgm_id].context_bmp,
                                    BITS2WORDS(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "No info was found for the given EFES PGM (%d) on the given EFES array/position (%d/%d) for FP stage (%s)! \r\n\n",
                     fes_pgm_id, fes_array, fes_position, dnx_field_stage_text(unit, field_stage));
    }

    PRT_TITLE_SET("EFES status - detailed info");
    PRT_INFO_ADD("EFES ID %d (array = %d, position = %d)  |  EFES Program ID %d", fes_id, fes_array, fes_position,
                 fes_pgm_id);
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES 2msb inst");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "CTX ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Starting bit");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Number of bits\nwithout/with valid bit");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Use valid bit");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Polarity");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Mask ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Mask value");

    /** Iterate over all Contexts and check which are relevant for the given EFES PGM. */
    for (ctx_iter = 0; ctx_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS; ctx_iter++)
    {
        /**
         * Check which bit of the context bitmap is set.
         */
        if (utilex_bitstream_test_bit(fes_id_info->fes_pgm_info[fes_pgm_id].context_bmp, ctx_iter))
        {
            /** Iterate over all FES2msb instructions for the current EFES ID. */
            for (fes2msb_id_ndx = 0;
                 fes2msb_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
                 fes2msb_id_ndx++)
            {
                fes2msb_info =
                    fes_id_info->fes_pgm_info[fes_pgm_id].pgm_ctx_2msb_info[ctx_iter].fes2msb_info[fes2msb_id_ndx];

                action_size =
                    dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_bits_in_fes_action -
                    fes2msb_info.valid_bits;

                SHR_IF_ERR_EXIT(dnx_field_map_action_type_to_predef_dnx_action
                                (unit, field_stage, fes2msb_info.action_type, &dnx_action));

                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

                /** Set 'EFES 2msb inst' cell. */
                PRT_CELL_SET("%d", fes2msb_id_ndx);
                /** Set 'CTX ID' cell. */
                PRT_CELL_SET("%d", ctx_iter);
                /** Set 'Action type' cell. */
                PRT_CELL_SET("%s", dnx_field_dnx_action_text(unit, dnx_action));
                /** Set 'Starting bit' cell. */
                PRT_CELL_SET("%d", fes2msb_info.shift);

                if (fes2msb_info.type == 0)
                {
                    /** Set 'Number of bits without/with valid bit' cell. */
                    PRT_CELL_SET("%d/%d", action_size, (action_size + 1));
                    /** Set 'Use valid bit' cell. */
                    PRT_CELL_SET("Yes");
                }
                else
                {
                    /** Set 'Number of bits without/with valid bit' cell. */
                    PRT_CELL_SET("%d/%d", action_size, action_size);
                    /** Set 'Use valid bit' cell. */
                    PRT_CELL_SET("No");
                }

                /** Set 'Polarity' cell. */
                PRT_CELL_SET("%d", fes2msb_info.polarity);
                /** Set 'Mask ID' cell. */
                PRT_CELL_SET("%d", fes2msb_info.chosen_mask);
                /** Set 'Mask value' cell. */
                PRT_CELL_SET("0x%08X", fes2msb_info.mask);
            }
        }
    }

    PRT_COMMITX;

exit:
    SHR_FREE(fes_id_info);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays list of relevant EFES Context information for
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
diag_dnx_field_efes_ctx_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e field_stage;
    uint8 do_display;

    uint8 is_cs_allocated;
    dnx_field_actions_fes_context_get_info_t context_fes_get_info;
    uint32 ctx_iter;
    char context_name[DBAL_MAX_STRING_LENGTH];
    uint8 add_new_cs_id;
    char key_select_name[DIAG_DNX_FIELD_UTILS_STR_SIZE];

    uint32 fes_id_iter;
    int array, position;
    int fes2msb_index;
    uint8 is_all_fes2msb_actions_same;

    bcm_field_action_t bcm_action;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_STAGE, field_stage);
    SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                    (unit, Field_efes_diag_stage_enum_table[field_stage].string, &field_stage));

    do_display = FALSE;

    PRT_TITLE_SET("EFES context");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "CTX ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "CTX Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key Select");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES ID (array, offset)");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES PGM");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Actions");

    /** Iterate over all contexts. */
    for (ctx_iter = 0; ctx_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS; ctx_iter++)
    {
        /** Check if the specified Context ID is allocated. */
        SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated(unit, field_stage, ctx_iter, &is_cs_allocated));
        if (!is_cs_allocated)
        {
            continue;
        }

        /**
         * On every new iteration of CS IDs we should add new line with underscore separator.
         * For this case we are using following flag add_new_cs_id, which is being set to TRUE for this purpose.
         */
        add_new_cs_id = TRUE;

        /** Gets the information about the FES quartets allocated to the context ID. */
        SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, ctx_iter, &context_fes_get_info));

        /** Iterates over max number of FESs for the given stage and retrieve/present relevant info. */
        for (fes_id_iter = 0; fes_id_iter < context_fes_get_info.nof_fes_quartets; fes_id_iter++)
        {
            /** In case the flag is set, we should set value for CS ID and name on this roll. */
            if (add_new_cs_id)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

                /** Set 'CTX ID' cell. */
                PRT_CELL_SET("%d", ctx_iter);

                /** Take the name for current context from the SW state. */
                sal_memset(context_name, 0, sizeof(context_name));
                SHR_IF_ERR_EXIT(dnx_field_context_sw.context_info.name.
                                value.stringget(unit, ctx_iter, field_stage, context_name));

                /**
                 * Check if Context name is being provided, in case no set N/A as default value,
                 * in other case set the provided name.
                 */
                if (sal_strncmp(context_name, "", sizeof(context_name)))
                {
                    /** Set 'CTX Name' cell. */
                    PRT_CELL_SET("%s", context_name);
                }
                else
                {
                    /** Set 'CTX Name' cell. */
                    PRT_CELL_SET("N/A");
                }

                /** Set the flag to FALSE, if we have more then one CS to be displayed, we should skip those 3 cells. */
                add_new_cs_id = FALSE;
            }
            else
            {
                /** In case the flag is not set we should skip cells related to CS ID and name. */
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                /** Skip 'CTX ID' and 'CTX Name' cells. */
                PRT_CELL_SKIP(2);
            }

            SHR_IF_ERR_EXIT(diag_dnx_field_utils_efes_key_select_text_get
                            (unit, field_stage, context_fes_get_info.fes_quartets[fes_id_iter].key_select,
                             key_select_name));
            /** Set 'Key Select' cell. */
            PRT_CELL_SET("%d (%s)", context_fes_get_info.fes_quartets[fes_id_iter].key_select, key_select_name);

            /** Get the array and position of the current EFES ID. */
            DNX_FIELD_ACTION_FES_ID_ARRAY_POSITION_GET(array, position, unit, field_stage,
                                                       context_fes_get_info.fes_quartets[fes_id_iter].fes_id);

            /** Set 'EFES ID (array, offset)' cell. */
            PRT_CELL_SET("%d (%d, %d)", context_fes_get_info.fes_quartets[fes_id_iter].fes_id, array, position);
            /** Set 'EFES PGM' cell. */
            PRT_CELL_SET("%d", context_fes_get_info.fes_quartets[fes_id_iter].fes_pgm_id);

            is_all_fes2msb_actions_same = TRUE;
            for (fes2msb_index = 0; fes2msb_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES; fes2msb_index++)
            {
                if (context_fes_get_info.fes_quartets[fes_id_iter].fes2msb_info[0].action_type !=
                    context_fes_get_info.fes_quartets[fes_id_iter].fes2msb_info[fes2msb_index].action_type)
                {
                    is_all_fes2msb_actions_same = FALSE;
                    break;
                }
            }

            if (is_all_fes2msb_actions_same)
            {
                SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm
                                (unit, field_stage,
                                 DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage,
                                            context_fes_get_info.fes_quartets[fes_id_iter].fes2msb_info[0].action_type),
                                 &bcm_action));
                /** Set 'Actions' cell. */
                PRT_CELL_SET("%s", dnx_field_bcm_action_text(unit, bcm_action));
            }
            else
            {
                dnx_field_action_type_t invalid_action_type;
                SHR_IF_ERR_EXIT(dnx_field_map_get_invalid_action_type(unit, field_stage, &invalid_action_type));
                for (fes2msb_index = 0; fes2msb_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES;
                     fes2msb_index++)
                {
                    if (fes2msb_index != 0)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SKIP(5);
                    }
                    if (context_fes_get_info.fes_quartets[fes_id_iter].fes2msb_info[fes2msb_index].action_type ==
                        invalid_action_type)
                    {
                        /** Set 'Actions' cell. */
                        PRT_CELL_SET("%s", "Invalid");
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm
                                        (unit, field_stage,
                                         DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage,
                                                    context_fes_get_info.
                                                    fes_quartets[fes_id_iter].fes2msb_info[fes2msb_index].action_type),
                                         &bcm_action));
                        /** Set 'Actions' cell. */
                        PRT_CELL_SET("%s", dnx_field_bcm_action_text(unit, bcm_action));
                    }
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
        LOG_CLI_EX("\r\n" "No EFES Context info was found on specified filters!! %s%s%s%s\r\n\n",
                   EMPTY, EMPTY, EMPTY, EMPTY);
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays list of added EFESs and relevant information by
 *   certain group and context, specified by the caller.
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
diag_dnx_field_efes_added_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 do_display;
    dnx_field_group_t field_group;
    dnx_field_context_t field_context;
    int max_nof_fgs;
    int max_nof_contexts;
    dnx_field_group_t fg_iter;
    dnx_field_context_t ctx_iter, ctx_begin;
    uint8 is_fg_allocated;
    uint8 is_ctx_allocated;
    dnx_field_stage_e fg_stage;
    dnx_field_group_context_full_info_t *attach_full_info = NULL;
    int max_num_efes_actions;
    dnx_field_fes_id_t efes_action_iter;
    dnx_field_action_position_t fes_position;
    dnx_field_action_array_t fes_array;
    bcm_field_action_priority_t encoded_position;
    bcm_field_efes_action_info_t efes_action_info;
    int fes2msb_index;
    uint8 is_all_fes2msb_actions_same;
    uint8 new_fg_ctx_add;
    uint8 new_efes_add;
    unsigned int is_attached;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_GROUP, field_group);
    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_CONTEXT, field_context);

    if (field_group == DNX_FIELD_GROUP_INVALID)
    {
        fg_iter = 0;
        max_nof_fgs = (dnx_data_field.group.nof_fgs_get(unit) - 1);
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, field_group, &is_fg_allocated));
        if (!is_fg_allocated)
        {
            LOG_CLI_EX("\r\n" "The given FG Id %d is not allocated!!!%s%s%s\r\n\n", field_group, EMPTY, EMPTY, EMPTY);
            SHR_EXIT();
        }
        max_nof_fgs = field_group;
        fg_iter = field_group;
    }

    if (field_context == DNX_FIELD_CONTEXT_ID_INVALID)
    {
        ctx_begin = 0;
        max_nof_contexts = (dnx_data_field.common_max_val.nof_contexts_get(unit) - 1);
    }
    else
    {
        if (field_group != DNX_FIELD_GROUP_INVALID)
        {
            SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, field_group, &fg_stage));
            SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated(unit, fg_stage, field_context, &is_ctx_allocated));
            if (!is_ctx_allocated)
            {
                LOG_CLI_EX("\r\n" "The given Context Id %d is not allocated for group stage %s!!!%s%s\r\n\n",
                           field_context, dnx_field_stage_text(unit, fg_stage), EMPTY, EMPTY);
                SHR_EXIT();
            }
        }
        max_nof_contexts = field_context;
        ctx_begin = field_context;
    }

    SHR_ALLOC(attach_full_info, sizeof(dnx_field_group_context_full_info_t), "attach_full_info", "%s%s%s\r\n", EMPTY,
              EMPTY, EMPTY);

    max_num_efes_actions = dnx_data_field.group.nof_action_per_fg_get(unit);
    do_display = FALSE;

    PRT_TITLE_SET("Added EFESs");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FG ID/Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "CTX ID/Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES ID (array, offset)");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Condition MSB");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Starting bit");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Use valid bit");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Number of bits\nwithout/with valid bit");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Polarity");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Mask value");

    for (; fg_iter <= max_nof_fgs; fg_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_iter, &is_fg_allocated));
        if (!is_fg_allocated)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_iter, &fg_stage));

        for (ctx_iter = ctx_begin; ctx_iter <= max_nof_contexts; ctx_iter++)
        {
            SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated(unit, fg_stage, ctx_iter, &is_ctx_allocated));
            if (!is_ctx_allocated)
            {
                continue;
            }

            SHR_IF_ERR_EXIT(dnx_field_group_is_context_id_on_arr(unit, fg_iter, ctx_iter, &is_attached));
            if (is_attached == FALSE)
            {
                continue;
            }

            new_fg_ctx_add = TRUE;

            SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &(attach_full_info->attach_basic_info)));
            SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fg_iter, ctx_iter, attach_full_info));

            for (efes_action_iter = 0; efes_action_iter < max_num_efes_actions &&
                 attach_full_info->actions_fes_info.added_fes_quartets[efes_action_iter].fes_id !=
                 DNX_FIELD_EFES_ID_INVALID; efes_action_iter++)
            {
                new_efes_add = TRUE;

                DNX_FIELD_ACTION_FES_ID_ARRAY_POSITION_GET(fes_array, fes_position, unit, fg_stage,
                                                           attach_full_info->
                                                           actions_fes_info.added_fes_quartets[efes_action_iter].
                                                           fes_id);
                encoded_position = BCM_FIELD_ACTION_POSITION(fes_array, fes_position);
                bcm_field_efes_action_info_t_init(&efes_action_info);
                SHR_IF_ERR_EXIT(bcm_field_efes_action_info_get
                                (unit, fg_iter, ctx_iter, encoded_position, &efes_action_info));

                if (efes_action_info.is_condition_valid == TRUE)
                {
                    if (new_fg_ctx_add)
                    {
                        char context_name[DBAL_MAX_STRING_LENGTH];
                        field_group_name_t group_name;

                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

                        /** Take the name for current FG from the SW state. */
                        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.name.get(unit, fg_iter, &group_name));

                        /**
                         * Check if Field Group name is being provided, in case no set N/A as default value,
                         * in other case set the provided name.
                         */
                        if (sal_strncmp(group_name.value, "", sizeof(group_name.value)))
                        {
                            /** Set 'FG ID/Name' cell. */
                            PRT_CELL_SET("%d / \"%s\"", fg_iter, group_name.value);
                        }
                        else
                        {
                            /** Set 'FG ID/Name' cell. */
                            PRT_CELL_SET("%d / \"N/A\"", fg_iter);
                        }

                        /** Take the name for current context from the SW state. */
                        sal_memset(context_name, 0, sizeof(context_name));
                        SHR_IF_ERR_EXIT(dnx_field_context_sw.context_info.name.
                                        value.stringget(unit, ctx_iter, fg_stage, context_name));
                        /**
                         * Check if Context name is being provided, in case no set N/A as default value,
                         * in other case set the provided name.
                         */
                        if (sal_strncmp(context_name, "", sizeof(context_name)))
                        {
                            /** Set 'CTX ID/Name' cell. */
                            PRT_CELL_SET("%d / \"%s\"", ctx_iter, context_name);
                        }
                        else
                        {
                            /** Set 'CTX ID/Name' cell. */
                            PRT_CELL_SET("%d / \"N/A\"", ctx_iter);
                        }
                        new_fg_ctx_add = FALSE;
                    }
                    else
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        /** Skip 'FG ID/Name' and 'CTX ID/Name' cells. */
                        PRT_CELL_SKIP(2);
                    }

                    if (new_efes_add)
                    {
                        /** Set 'EFES ID (array, offset)' cell. */
                        PRT_CELL_SET("%d (%d, %d)",
                                     attach_full_info->actions_fes_info.added_fes_quartets[efes_action_iter].fes_id,
                                     fes_array, fes_position);
                        /** Set 'Condition MSB' cell. */
                        PRT_CELL_SET("%d", efes_action_info.condition_msb);
                        new_efes_add = FALSE;
                    }
                    else
                    {
                        /** Skip 'EFES ID (array, offset)' and 'Condition MSB' cells. */
                        PRT_CELL_SKIP(2);
                    }

                    is_all_fes2msb_actions_same = TRUE;
                    for (fes2msb_index = 0; fes2msb_index < BCM_FIELD_NUMBER_OF_CONDITIONS_PER_EFES; fes2msb_index++)
                    {
                        if (sal_memcmp
                            (&efes_action_info.efes_condition_conf[0],
                             &efes_action_info.efes_condition_conf[fes2msb_index],
                             sizeof(bcm_field_efes_condition_conf_t)))
                        {
                            is_all_fes2msb_actions_same = FALSE;
                            break;
                        }
                    }

                    if (is_all_fes2msb_actions_same)
                    {
                        /** Set 'Action Type' cell. */
                        PRT_CELL_SET("%s",
                                     dnx_field_bcm_action_text(unit,
                                                               efes_action_info.efes_condition_conf[0].action_type));
                        /** Set 'Starting bit' cell. */
                        PRT_CELL_SET("%d", efes_action_info.efes_condition_conf[0].action_lsb);
                        if (efes_action_info.efes_condition_conf[0].action_with_valid_bit)
                        {
                            /** Set 'Use valid bit' cell. */
                            PRT_CELL_SET("%s", "Yes");
                            /** Set 'Number of bits without/with valid bit' cell. */
                            PRT_CELL_SET("%d/%d", efes_action_info.efes_condition_conf[0].action_nof_bits,
                                         (efes_action_info.efes_condition_conf[0].action_nof_bits + 1));
                            /** Set 'Polarity' cell. */
                            PRT_CELL_SET("%d", efes_action_info.efes_condition_conf[0].valid_bit_polarity);
                        }
                        else
                        {
                            /** Set 'Use valid bit' cell. */
                            PRT_CELL_SET("%s", "No");
                            /** Set 'Number of bits without/with valid bit' cell. */
                            PRT_CELL_SET("%d/%d", efes_action_info.efes_condition_conf[0].action_nof_bits,
                                         efes_action_info.efes_condition_conf[0].action_nof_bits);
                            /** Set 'Polarity' cell. */
                            PRT_CELL_SET("%s", "N/A");
                        }
                        /** Set 'Mask value' cell. */
                        PRT_CELL_SET("0x%X", efes_action_info.efes_condition_conf[0].action_or_mask);
                    }
                    else
                    {
                        for (fes2msb_index = 0; fes2msb_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES;
                             fes2msb_index++)
                        {
                            if (fes2msb_index != 0)
                            {
                                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                                /** Skip 'Action Type', 'Starting bit', 'Use valid bit' and 'Number of bits without/with valid bit' cells. */
                                PRT_CELL_SKIP(4);
                            }
                            if (efes_action_info.efes_condition_conf[fes2msb_index].action_type == BCM_FIELD_INVALID)
                            {
                                /** Set 'Action Type' cell. */
                                PRT_CELL_SET("%s", "Invalid");
                                /** Set 'Starting bit' cell. */
                                PRT_CELL_SET("%s", "N/A");
                                /** Set 'Use valid bit' cell. */
                                PRT_CELL_SET("%s", "N/A");
                                /** Set 'Number of bits without/with valid bit' cell. */
                                PRT_CELL_SET("%s", "N/A");
                                /** Set 'Polarity' cell. */
                                PRT_CELL_SET("%s", "N/A");
                                /** Set 'Mask value' cell. */
                                PRT_CELL_SET("%s", "N/A");
                            }
                            else
                            {
                                /** Set 'Action Type' cell. */
                                PRT_CELL_SET("%s",
                                             dnx_field_bcm_action_text(unit,
                                                                       efes_action_info.efes_condition_conf
                                                                       [fes2msb_index].action_type));
                                /** Set 'Starting bit' cell. */
                                PRT_CELL_SET("%d", efes_action_info.efes_condition_conf[fes2msb_index].action_lsb);
                                if (efes_action_info.efes_condition_conf[fes2msb_index].action_with_valid_bit)
                                {
                                    /** Set 'Use valid bit' cell. */
                                    PRT_CELL_SET("%s", "Yes");
                                    /** Set 'Number of bits without/with valid bit' cell. */
                                    PRT_CELL_SET("%d/%d",
                                                 efes_action_info.efes_condition_conf[fes2msb_index].action_nof_bits,
                                                 (efes_action_info.efes_condition_conf[fes2msb_index].action_nof_bits +
                                                  1));
                                    /** Set 'Polarity' cell. */
                                    PRT_CELL_SET("%d",
                                                 efes_action_info.
                                                 efes_condition_conf[fes2msb_index].valid_bit_polarity);
                                }
                                else
                                {
                                    /** Set 'Use valid bit' cell. */
                                    PRT_CELL_SET("%s", "No");
                                    /** Set 'Number of bits without/with valid bit' cell. */
                                    PRT_CELL_SET("%d/%d",
                                                 efes_action_info.efes_condition_conf[fes2msb_index].action_nof_bits,
                                                 efes_action_info.efes_condition_conf[fes2msb_index].action_nof_bits);
                                    /** Set 'Polarity' cell. */
                                    PRT_CELL_SET("%s", "N/A");
                                }
                                /** Set 'Mask value' cell. */
                                PRT_CELL_SET("0x%X",
                                             efes_action_info.efes_condition_conf[fes2msb_index].action_or_mask);
                            }
                        }
                    }

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
        LOG_CLI_EX("\r\n" "No added EFES info was found on specified filters!! %s%s%s%s\r\n\n",
                   EMPTY, EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FREE(attach_full_info);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * Globals required at end because they reference static procedures/tables above.
 * {
 */
/**
 * \brief
 *   List of shell options for 'efes masks' shell commands
 */
sh_sand_cmd_t Sh_dnx_field_efes_list_masks_cmds[] = {
    {"program", diag_dnx_field_efes_list_masks_program_cb, NULL, Field_efes_list_options,
     &Field_efes_list_masks_program_man}
    ,
    {"context", diag_dnx_field_efes_list_masks_context_cb, NULL, Field_efes_list_options,
     &Field_efes_list_masks_context_man}
    ,
    {"value", diag_dnx_field_efes_list_masks_value_cb, NULL, Field_efes_list_options, &Field_efes_list_masks_value_man}
    ,
    {NULL}
};
/**
 * \brief
 *   List of shell options for 'efes list' shell commands
 */
sh_sand_cmd_t Sh_dnx_field_efes_list_cmds[] = {
    {"programs", diag_dnx_field_efes_list_programs_cb, NULL, Field_efes_list_options, &Field_efes_list_programs_man}
    ,
    {"masks", NULL, Sh_dnx_field_efes_list_masks_cmds, NULL, &Field_efes_list_masks_man}
    ,
    {"status", diag_dnx_field_efes_list_status_cb, NULL, Field_efes_list_options, &Field_efes_list_status_man}
    ,
    {NULL}
};

/**
 * \brief
 *   List of shell options for 'efes' shell commands (list, info)
 */
sh_sand_cmd_t Sh_dnx_field_efes_cmds[] = {
    {"list", NULL, Sh_dnx_field_efes_list_cmds, NULL, &Field_efes_list_man}
    ,
    {"info", diag_dnx_field_efes_info_cb, NULL, Field_efes_info_options, &Field_efes_info_man}
    ,
    {"context", diag_dnx_field_efes_ctx_cb, NULL, Field_efes_ctx_options, &Field_efes_ctx_man}
    ,
    {"added", diag_dnx_field_efes_added_cb, NULL, Field_efes_added_options, &Field_efes_added_man}
    ,
    {NULL}
};

/*
 * }
 */
