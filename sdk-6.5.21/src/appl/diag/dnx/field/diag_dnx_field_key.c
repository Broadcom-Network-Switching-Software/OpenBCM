/** \file diag_dnx_field_key.c
 *
 * 'key' operations procedures for DNX.
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
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "diag_dnx_field_key.h"
#include <bcm_int/dnx/field/field_key.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_group_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_context_access.h>
#include "diag_dnx_field_utils.h"
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_field_key_alloc_access.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/**
 * Array, which contains information for
 * relevant DBAL table per stage.
 * Used in field_key_last_key_info_cb() to allocate
 * a handle to the table.
 */
static dbal_tables_e Last_key_info_tables[DNX_FIELD_KEY_NUM_STAGES] = {
    DBAL_TABLE_FIELD_IPMF1_LAST_KEY_INFO,
    DBAL_TABLE_FIELD_IPMF2_LAST_KEY_INFO,
    DBAL_TABLE_FIELD_IPMF3_LAST_KEY_INFO,
    /**
     * Add relevant table for EPMF stage,
     * once the debug registers have been added to the adapter.
     */
    DBAL_NOF_TABLES
};
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
 *  'Help' description for 'INFO' (shell commands).
 */
static sh_sand_man_t Field_key_info_man = {
    .brief = "'Info' related utilities",
    .full = "'Info' display info related to keys.\r\n"
    "If no context is specified then 'minimal val'-'maximal val' is assumed.\r\n"
    "If no stage is specified then range of all-stages is assumed.\r\n"
    "If only one value is specified for context/stage then this 'single value' range is assumed.\r\n"
    "If 'valid' is not specified or is set to 'yes' then only stage/context combinations, with at"
    " least one key has any allocation, are displayed.\r\n",
    .synopsis = "[stage=<<IPMF1 | IPMF2 | ...>-<<IPMF1 | IPMF2 | ...>>]"
                " [context=<<Lowest context number>-<Highest context number>>]"
                " [valid=<YES | NO>]",
    .examples = "valid=no stage=epmf\n"
                "valid=YES context=2-15 stage=IPMF1-IPMF3\n"
                "",
};

static sh_sand_option_t Field_key_info_options[] = {
    {DIAG_DNX_FIELD_OPTION_CONTEXT,    SAL_FIELD_TYPE_UINT32, "Lowest-highest context ID to get key info for",  "0xFFFFFFFF"},
    {DIAG_DNX_FIELD_OPTION_STAGE,      SAL_FIELD_TYPE_UINT32, "Lowest-highest field stage to get key info for", "STAGE_LOWEST-STAGE_HIGHEST",  (void *)Field_key_diag_stage_enum_table, "STAGE_LOWEST-STAGE_HIGHEST"},
    {DIAG_DNX_FIELD_OPTION_VALID,      SAL_FIELD_TYPE_BOOL,   "If TRUE, display non-empty key blocks only",     "Yes"},
    {NULL}
};


/*
 * }
 */
/* *INDENT-ON* */
/**
 * \brief
 *   This function sets relevant information about usage of the initial keys
 *   by field groups.
 * \param [in] unit - The unit number.
 * \param [in] new_ctx_print_p - Flag to indicate whether to print
 *  the context ID in the relevant sell or to skip it.
 * \param [in] new_key_print_p - Flag to indicate whether to print
 *  the initial key ID in the relevant sell or to skip it.
 * \param [in] context_id - Context Id for which information for
 *  initial key usage have to be presented.
 * \param [in] context_name_p - Name of the given context ID to be
 *  presented in the relevant cell.
 * \param [in] initial_key_id - Initial key for which information
 *  will be presented.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 * \param [out] any_key_was_displayed_p - Flag to be set to TRUE in case any data
 *  about for the given initial key was set. Default value is FALSE.
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_key_initial_fg_key_usage_set(
    int unit,
    uint8 *new_ctx_print_p,
    uint8 *new_key_print_p,
    dnx_field_context_t context_id,
    char *context_name_p,
    dbal_enum_value_field_field_key_e initial_key_id,
    prt_control_t * prt_ctr,
    int *any_key_was_displayed_p)
{
    dnx_field_group_t group_index;
    dnx_field_group_context_full_info_t *attach_full_info_p;
    dnx_field_group_t fg_ids[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS];
    int nof_fgs;
    field_group_name_t group_name;
    uint32 qual_index, ffc_in_qual_index;
    bcm_field_input_types_t bcm_input_type;

    SHR_FUNC_INIT_VARS(unit);

    attach_full_info_p = NULL;

    SHR_ALLOC(attach_full_info_p, sizeof(*attach_full_info_p), "initial keys",
              "Issue with memory for attach_full_info %s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Get all field groups, attached to the given context_id. */
    SHR_IF_ERR_EXIT(dnx_field_group_context_fgs_get(unit, DNX_FIELD_STAGE_IPMF2, context_id, fg_ids, &nof_fgs));

    /** Iterate over nof_fgs and retrieve relevant information for them. */
    for (group_index = 0; group_index < nof_fgs; group_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fg_ids[group_index], context_id, attach_full_info_p));

        /** Iterate over all valid qualifiers for the current FG. */
        for (qual_index = 0; qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
             attach_full_info_p->attach_basic_info.dnx_quals[qual_index] != DNX_FIELD_QUAL_TYPE_INVALID; qual_index++)
        {
            uint8 first_ffc_presented = FALSE;
            /** Iterate over valid FFCs for the current qualifier.*/
            for (ffc_in_qual_index = 0; ffc_in_qual_index < DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL &&
                 attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_index].
                 ffc_info[ffc_in_qual_index].ffc_initial.ffc.ffc_id != DNX_FIELD_FFC_ID_INVALID; ffc_in_qual_index++)
            {
                /**
                 * Check if the key_id, used by the current FG, is not the same as the given one.
                 * In case they are different continue until next group key_id arrive.
                 */
                if (attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_index].
                    ffc_info[ffc_in_qual_index].ffc_initial.ffc.key_id != initial_key_id)
                {
                    continue;
                }

                /**
                 * Add new row with underscore separator in case we have to print
                 * new context ID and set the new_ctx_print_p flag to FALSE.
                 * Otherwise add rows without separators and skip the context ID cell.
                 */
                if (*new_ctx_print_p)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    /**
                     * Check if Context name is being provided, in case no set N/A as default value,
                     * in other case set the provided name.
                     */
                    if (sal_strncmp(context_name_p, "", DBAL_MAX_STRING_LENGTH))
                    {
                        PRT_CELL_SET("%d / \"%.*s\"", context_id, DBAL_MAX_STRING_LENGTH, context_name_p);
                    }
                    else
                    {
                        PRT_CELL_SET("%d / \"%s\"", context_id, "N/A");
                    }

                    *new_ctx_print_p = FALSE;
                }
                else
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SKIP(1);
                }

                /**
                 * Set the initial_key_id cell in case we have to print
                 * new initial key and set the new_key_print_p flag to FALSE.
                 * Otherwise skip the context ID cell.
                 */
                if (*new_key_print_p)
                {
                    PRT_CELL_SET("%c", ('A' + initial_key_id));
                    *new_key_print_p = FALSE;
                }
                else
                {
                    PRT_CELL_SKIP(1);
                }

                /**
                 * In order to avoid printing the qual name for every FFC ID,
                 * just check the indication for 'first_ffc_presented'.
                 */
                if (!first_ffc_presented)
                {
                    PRT_CELL_SET("%s",
                                 dnx_field_dnx_qual_text(unit,
                                                         attach_full_info_p->attach_basic_info.dnx_quals[qual_index]));
                    first_ffc_presented = TRUE;
                }
                else
                {
                    PRT_CELL_SKIP(1);
                }

                PRT_CELL_SET("FFC %d",
                             attach_full_info_p->group_ffc_info.key.
                             qualifier_ffc_info[qual_index].ffc_info[ffc_in_qual_index].ffc_initial.ffc.ffc_id);
                PRT_CELL_SET("%d",
                             (attach_full_info_p->group_ffc_info.key.
                              qualifier_ffc_info[qual_index].ffc_info[ffc_in_qual_index].ffc_initial.ffc.
                              ffc_instruction.size + 1));
                PRT_CELL_SET("%d",
                             attach_full_info_p->group_ffc_info.key.
                             qualifier_ffc_info[qual_index].ffc_info[ffc_in_qual_index].ffc_initial.key_dest_offset);

                /** Convert the input type, of the current qualifier, to its BCM type. */
                SHR_IF_ERR_EXIT(dnx_field_map_qual_input_type_dnx_to_bcm(unit,
                                                                         attach_full_info_p->attach_basic_info.
                                                                         qual_info[qual_index].input_type,
                                                                         &bcm_input_type));
                PRT_CELL_SET("%s", dnx_field_bcm_input_type_text(bcm_input_type));
                if (attach_full_info_p->attach_basic_info.qual_info[qual_index].input_type ==
                    DNX_FIELD_INPUT_TYPE_META_DATA
                    || attach_full_info_p->attach_basic_info.qual_info[qual_index].input_type ==
                    DNX_FIELD_INPUT_TYPE_META_DATA2)
                {
                    PRT_CELL_SET("%s", "-");
                    if (DNX_QUAL_CLASS(attach_full_info_p->attach_basic_info.dnx_quals[qual_index]) ==
                        DNX_FIELD_QUAL_CLASS_USER)
                    {
                        PRT_CELL_SET("%d", attach_full_info_p->attach_basic_info.qual_info[qual_index].offset);
                    }
                    else
                    {
                        PRT_CELL_SET("%s", "-");
                    }
                }
                else if (attach_full_info_p->attach_basic_info.qual_info[qual_index].input_type ==
                         DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD ||
                         attach_full_info_p->attach_basic_info.qual_info[qual_index].input_type ==
                         DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE)
                {
                    PRT_CELL_SET("%d", attach_full_info_p->attach_basic_info.qual_info[qual_index].input_arg);
                    if (DNX_QUAL_CLASS(attach_full_info_p->attach_basic_info.dnx_quals[qual_index]) ==
                        DNX_FIELD_QUAL_CLASS_USER)
                    {
                        PRT_CELL_SET("%d", attach_full_info_p->attach_basic_info.qual_info[qual_index].offset);
                    }
                    else
                    {
                        PRT_CELL_SET("%s", "-");
                    }
                }
                else if (attach_full_info_p->attach_basic_info.qual_info[qual_index].input_type ==
                         DNX_FIELD_INPUT_TYPE_KBP)
                {
                    PRT_CELL_SET("%s", "-");
                    PRT_CELL_SET("%d", attach_full_info_p->attach_basic_info.qual_info[qual_index].offset);
                }
                else if (attach_full_info_p->attach_basic_info.qual_info[qual_index].input_type ==
                         DNX_FIELD_INPUT_TYPE_CONST)
                {
                    PRT_CELL_SET("0x%X", attach_full_info_p->attach_basic_info.qual_info[qual_index].input_arg);
                    PRT_CELL_SET("%s", "-");
                }
                else
                {
                    PRT_CELL_SET("%d", attach_full_info_p->attach_basic_info.qual_info[qual_index].input_arg);
                    PRT_CELL_SET("%d", attach_full_info_p->attach_basic_info.qual_info[qual_index].offset);
                }

                /** Take the name for current FG from the SW state. */
                SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.name.get(unit, fg_ids[group_index], &group_name));
                /**
                 * Check if Field Group name is being provided, in case no set N/A as default value,
                 * in other case set the provided name.
                 */
                if (sal_strncmp(group_name.value, "", sizeof(group_name.value)))
                {
                    PRT_CELL_SET("%d/\"%.*s\"", fg_ids[group_index], (int) sizeof(group_name.value), group_name.value);
                }
                else
                {
                    PRT_CELL_SET("%d/\"%s\"", fg_ids[group_index], "N/A");
                }

                /**
                 * Update the any_key_was_displayed_p flag,
                 * because we set information at least for one key.
                 */
                *any_key_was_displayed_p = TRUE;
            }
        }
    }

exit:
    SHR_FREE(attach_full_info_p);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets relevant information about usage of the initial keys
 *   by state table field groups.
 * \param [in] unit - The unit number.
 * \param [in] new_ctx_print_p - Flag to indicate whether to print
 *  the context ID in the relevant sell or to skip it.
 * \param [in] new_key_print_p - Flag to indicate whether to print
 *  the initial key ID in the relevant sell or to skip it.
 * \param [in] context_id - Context Id for which information for
 *  initial key usage have to be presented.
 * \param [in] context_name_p - Name of the given context ID to be
 *  presented in the relevant cell.
 * \param [in] initial_key_id - Initial key for which information
 *  will be presented.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 * \param [out] any_key_was_displayed_p - Flag to be set to TRUE in case any data
 *  about for the given initial key was set. Default value is FALSE.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_key_initial_ctx_mode_key_usage_state_table_info_set(
    int unit,
    uint8 *new_ctx_print_p,
    uint8 *new_key_print_p,
    dnx_field_context_t context_id,
    char *context_name_p,
    dbal_enum_value_field_field_key_e initial_key_id,
    prt_control_t * prt_ctr,
    int *any_key_was_displayed_p)
{
    dnx_field_group_t group_index;
    dnx_field_group_t fg_ids[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS];
    int nof_fgs;
    field_group_name_t group_name;
    dnx_field_group_type_e fg_type;
    uint32 qual_index, ffc_in_qual_index;
    bcm_field_input_types_t bcm_input_type;
    dnx_field_group_context_full_info_t *attach_full_info_p = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(attach_full_info_p, sizeof(*attach_full_info_p), "initial keys",
              "Issue with memory for attach_full_info %s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Get all field groups, attached to the given context_id. */
    SHR_IF_ERR_EXIT(dnx_field_group_context_fgs_get(unit, DNX_FIELD_STAGE_IPMF1, context_id, fg_ids, &nof_fgs));

    /** Iterate over nof_fgs and retrieve relevant information for them. */
    for (group_index = 0; group_index < nof_fgs; group_index++)
    {
        /** Get the type of the current field group. */
        SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_ids[group_index], &fg_type));
        /**
         * Check if the current FG is not of type STATE_TABLE,
         * then we can skip it and continue iterating.
         */
        if (fg_type != DNX_FIELD_GROUP_TYPE_STATE_TABLE)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fg_ids[group_index], context_id, attach_full_info_p));

        /** Iterate over all valid qualifiers for the current FG. */
        for (qual_index = 0; qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
             attach_full_info_p->attach_basic_info.dnx_quals[qual_index] != DNX_FIELD_QUAL_TYPE_INVALID; qual_index++)
        {
            uint8 first_ffc_presented = FALSE;
            /** Iterate over valid FFCs for the current qualifier.*/
            for (ffc_in_qual_index = 0; ffc_in_qual_index < DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL &&
                 attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_index].ffc_info[ffc_in_qual_index].
                 ffc.ffc_id != DNX_FIELD_FFC_ID_INVALID; ffc_in_qual_index++)
            {
                /**
                 * Add new row with underscore separator in case we have to print
                 * new context ID and set the new_ctx_print_p flag to FALSE.
                 * Otherwise add rows without separators and skip the context ID cell.
                 */
                if (*new_ctx_print_p)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    /**
                     * Check if Context name is being provided, in case no set N/A as default value,
                     * in other case set the provided name.
                     */
                    if (sal_strncmp(context_name_p, "", DBAL_MAX_STRING_LENGTH))
                    {
                        PRT_CELL_SET("%d / \"%.*s\"", context_id, DBAL_MAX_STRING_LENGTH, context_name_p);
                    }
                    else
                    {
                        PRT_CELL_SET("%d / \"%s\"", context_id, "N/A");
                    }

                    *new_ctx_print_p = FALSE;
                }
                else
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SKIP(1);
                }

                /**
                 * Set the initial_key_id cell in case we have to print
                 * new initial key and set the new_key_print_p flag to FALSE.
                 * Otherwise skip the context ID cell.
                 */
                if (*new_key_print_p)
                {
                    PRT_CELL_SET("%c", ('A' + initial_key_id));
                    *new_key_print_p = FALSE;
                }
                else
                {
                    PRT_CELL_SKIP(1);
                }

                /**
                 * In order to avoid printing the qual name for every FFC ID,
                 * just check the indication for 'first_ffc_presented'.
                 */
                if (!first_ffc_presented)
                {
                    PRT_CELL_SET("%s",
                                 dnx_field_dnx_qual_text(unit,
                                                         attach_full_info_p->attach_basic_info.dnx_quals[qual_index]));
                }
                else
                {
                    PRT_CELL_SKIP(1);
                }
                PRT_CELL_SET("FFC %d",
                             attach_full_info_p->group_ffc_info.key.
                             qualifier_ffc_info[qual_index].ffc_info[ffc_in_qual_index].ffc.ffc_id);
                PRT_CELL_SET("%d",
                             attach_full_info_p->group_ffc_info.key.
                             qualifier_ffc_info[qual_index].ffc_info[ffc_in_qual_index].ffc.ffc_instruction.size);
                PRT_CELL_SET("%d", 0);

                /** Convert the input type, of the current qualifier, to its BCM type. */
                SHR_IF_ERR_EXIT(dnx_field_map_qual_input_type_dnx_to_bcm(unit,
                                                                         attach_full_info_p->attach_basic_info.
                                                                         qual_info[qual_index].input_type,
                                                                         &bcm_input_type));
                PRT_CELL_SET("%s", dnx_field_bcm_input_type_text(bcm_input_type));
                if (attach_full_info_p->attach_basic_info.qual_info[qual_index].input_type ==
                    DNX_FIELD_INPUT_TYPE_META_DATA
                    || attach_full_info_p->attach_basic_info.qual_info[qual_index].input_type ==
                    DNX_FIELD_INPUT_TYPE_META_DATA2)
                {
                    PRT_CELL_SET("%s", "-");
                    if (DNX_QUAL_CLASS(attach_full_info_p->attach_basic_info.dnx_quals[qual_index]) ==
                        DNX_FIELD_QUAL_CLASS_USER)
                    {
                        PRT_CELL_SET("%d", attach_full_info_p->attach_basic_info.qual_info[qual_index].offset);
                    }
                    else
                    {
                        PRT_CELL_SET("%s", "-");
                    }
                }
                else if (attach_full_info_p->attach_basic_info.qual_info[qual_index].input_type ==
                         DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD ||
                         attach_full_info_p->attach_basic_info.qual_info[qual_index].input_type ==
                         DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE)
                {
                    PRT_CELL_SET("%d", attach_full_info_p->attach_basic_info.qual_info[qual_index].input_arg);
                    if (DNX_QUAL_CLASS(attach_full_info_p->attach_basic_info.dnx_quals[qual_index]) ==
                        DNX_FIELD_QUAL_CLASS_USER)
                    {
                        PRT_CELL_SET("%d", attach_full_info_p->attach_basic_info.qual_info[qual_index].offset);
                    }
                    else
                    {
                        PRT_CELL_SET("%s", "-");
                    }
                }
                else if (attach_full_info_p->attach_basic_info.qual_info[qual_index].input_type ==
                         DNX_FIELD_INPUT_TYPE_KBP)
                {
                    PRT_CELL_SET("%s", "-");
                    PRT_CELL_SET("%d", attach_full_info_p->attach_basic_info.qual_info[qual_index].offset);
                }
                else if (attach_full_info_p->attach_basic_info.qual_info[qual_index].input_type ==
                         DNX_FIELD_INPUT_TYPE_CONST)
                {
                    PRT_CELL_SET("0x%X", attach_full_info_p->attach_basic_info.qual_info[qual_index].input_arg);
                    PRT_CELL_SET("%s", "-");
                }
                else
                {
                    PRT_CELL_SET("%d", attach_full_info_p->attach_basic_info.qual_info[qual_index].input_arg);
                    PRT_CELL_SET("%d", attach_full_info_p->attach_basic_info.qual_info[qual_index].offset);
                }

                /** Take the name for current FG from the SW state. */
                SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.name.get(unit, fg_ids[group_index], &group_name));
                /**
                 * Check if Field Group name is being provided, in case no set N/A as default value,
                 * in other case set the provided name.
                 */
                if (sal_strncmp(group_name.value, "", sizeof(group_name.value)))
                {
                    PRT_CELL_SET("%d/\"%.*s\"", fg_ids[group_index], (int) sizeof(group_name.value), group_name.value);
                }
                else
                {
                    PRT_CELL_SET("%d/\"%s\"", fg_ids[group_index], "N/A");
                }

                /**
                 * Update the any_key_was_displayed_p flag,
                 * because we set information at least for one key.
                 */
                *any_key_was_displayed_p = TRUE;
            }
        }
    }

exit:
    SHR_FREE(attach_full_info_p);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * \brief
 *   This function sets relevant information about usage of a single initial key
 *   by context modes (compare and hashing).
 * \param [in] unit - The unit number.
 * \param [in] new_ctx_print_p - Flag to indicate whether to print
 *  the context ID in the relevant sell or to skip it.
 * \param [in] new_key_print_p - Flag to indicate whether to print
 *  the initial key ID in the relevant sell or to skip it.
 * \param [in] context_id - Context Id for which information for
 *  initial key usage have to be presented.
 * \param [in] context_name_p - Name of the given context ID to be
 *  presented in the relevant cell.
 * \param [in] initial_key_id - Initial key for which information
 *  will be presented.
 * \param [in] key_info_p - Key information about the current context mode, contains
 *  key_id, key_template and attach information.
 * \param [in] ctx_mode_name_p - Name of the current context mode, can be one
 *  of following (CMP1, CMP2 and HASH).
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 * \param [out] last_qual_index_p - Last qualifier index, on which iteration stoped.
 * \param [out] number_of_used_ffc_per_qual_p - How many FFC left to be presented in case of
 *  qualifier, which is using two keys.
 * \param [out] any_key_was_displayed_p - Flag to be set to TRUE in case any data
 *  about for the given initial key was set. Default value is FALSE.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_key_initial_ctx_mode_single_key_usage_cmp_hash_info_set(
    int unit,
    uint8 *new_ctx_print_p,
    uint8 *new_key_print_p,
    dnx_field_context_t context_id,
    char *context_name_p,
    dbal_enum_value_field_field_key_e initial_key_id,
    dnx_field_context_key_info_t * key_info_p,
    char *ctx_mode_name_p,
    prt_control_t * prt_ctr,
    uint32 *last_qual_index_p,
    uint8 *number_of_used_ffc_per_qual_p,
    int *any_key_was_displayed_p)
{
    bcm_field_input_types_t bcm_input_type;
    uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC];
    dnx_field_key_initial_ffc_info_t ipmf1_ffc_initial;
    uint8 key_index;
    uint32 ffc_id_index = 0;
    int temp_qual_size;
    int max_num_bits_in_ffc;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the max num of bits in FFC from DNX data. */
    max_num_bits_in_ffc = dnx_data_field.common_max_val.nof_bits_in_ffc_get(unit);

    /**
     * In compare and HASH modes we can have up to two keys,
     * iterate over them and check if any of them matches on
     * the current initial key, if not exit from the function,
     * otherwise break the loop and continue with presenting the data.
     */
    for (key_index = 0; key_index < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX; key_index++)
    {
        if (key_info_p->key_id.id[key_index] == initial_key_id)
        {
            break;
        }
    }
    if (key_index == DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX)
    {
        SHR_EXIT();
    }

    /** Get an array of ffc_id, which are used for construction of the current inital_key. */
    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_array_hw_get
                    (unit, DNX_FIELD_STAGE_IPMF1, context_id, initial_key_id, ffc_id));
    /**
     * Check if we have at least one valid FFCs returned.
     * The last element of the returned array have to be indicated with DNX_FIELD_FFC_ID_INVALID.
     * In case there are no valid FFCs exit the function.
     */
    if (ffc_id[0] == DNX_FIELD_FFC_ID_INVALID)
    {
        SHR_EXIT();
    }

    /** Iterate over all valid qualifiers for the current context mode. */
    for (; *last_qual_index_p < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
         key_info_p->key_template.key_qual_map[*last_qual_index_p].qual_type != DNX_FIELD_QUAL_TYPE_INVALID;
         (*last_qual_index_p)++)
    {
        uint8 first_ffc_presented = FALSE;

        if (*number_of_used_ffc_per_qual_p == 0)
        {
            /**
             * Set the the maximum numbers of used FFCs by current qualifiers to iterate on,
             * depends on current qual_size and max number of bits in FFC.
             */
            for (temp_qual_size = key_info_p->key_template.key_qual_map[*last_qual_index_p].size;
                 temp_qual_size > 0; temp_qual_size -= max_num_bits_in_ffc, (*number_of_used_ffc_per_qual_p)++);
        }

        /** Iterate over maximum number of FFCs per qualifier.*/
        while (ffc_id[ffc_id_index] != DNX_FIELD_FFC_ID_INVALID)
        {
            if (*number_of_used_ffc_per_qual_p == 0)
            {
                break;
            }
            /** Get initial FFC info about the current FFC. */
            SHR_IF_ERR_EXIT(dnx_field_key_single_initial_ffc_get
                            (unit, context_id, initial_key_id, ffc_id[ffc_id_index], &ipmf1_ffc_initial));

            /**
             * Add new row with underscore separator in case we have to print
             * new context ID and set the new_ctx_print_p flag to FALSE.
             * Otherwise add rows without separators and skip the context ID cell.
             */
            if (*new_ctx_print_p)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                /**
                 * Check if Context name is being provided, in case no set N/A as default value,
                 * in other case set the provided name.
                 */
                if (sal_strncmp(context_name_p, "", DBAL_MAX_STRING_LENGTH))
                {
                    PRT_CELL_SET("%d / \"%.*s\"", context_id, DBAL_MAX_STRING_LENGTH, context_name_p);
                }
                else
                {
                    PRT_CELL_SET("%d / \"%s\"", context_id, "N/A");
                }

                *new_ctx_print_p = FALSE;
            }
            else
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(1);
            }

            /**
             * Set the initial_key_id cell in case we have to print
             * new initial key and set the new_key_print_p flag to FALSE.
             * Otherwise skip the context ID cell.
             */
            if (*new_key_print_p)
            {
                PRT_CELL_SET("%c", ('A' + initial_key_id));
                *new_key_print_p = FALSE;
            }
            else
            {
                PRT_CELL_SKIP(1);
            }

            /**
             * In order to avoid printing the qual name for every FFC ID,
             * just check the indication for 'first_ffc_presented'.
             */
            if (!first_ffc_presented)
            {
                PRT_CELL_SET("%s",
                             dnx_field_dnx_qual_text(unit,
                                                     key_info_p->key_template.
                                                     key_qual_map[*last_qual_index_p].qual_type));
                first_ffc_presented = TRUE;
            }
            else
            {
                PRT_CELL_SKIP(1);
            }

            PRT_CELL_SET("FFC %d", ffc_id[ffc_id_index]);
            PRT_CELL_SET("%d", (ipmf1_ffc_initial.ffc.ffc_instruction.size + 1));
            PRT_CELL_SET("%d", ipmf1_ffc_initial.ffc.ffc_instruction.key_offset);

            /** Convert the input type, of the current qualifier, to its BCM type. */
            SHR_IF_ERR_EXIT(dnx_field_map_qual_input_type_dnx_to_bcm(unit,
                                                                     key_info_p->
                                                                     attach_info[*last_qual_index_p].input_type,
                                                                     &bcm_input_type));
            PRT_CELL_SET("%s", dnx_field_bcm_input_type_text(bcm_input_type));
            if (key_info_p->attach_info[*last_qual_index_p].input_type == DNX_FIELD_INPUT_TYPE_META_DATA ||
                key_info_p->attach_info[*last_qual_index_p].input_type == DNX_FIELD_INPUT_TYPE_META_DATA2)
            {
                PRT_CELL_SET("%s", "-");
                if (DNX_QUAL_CLASS(key_info_p->key_template.key_qual_map[*last_qual_index_p].qual_type) ==
                    DNX_FIELD_QUAL_CLASS_USER)
                {
                    PRT_CELL_SET("%d", key_info_p->attach_info[*last_qual_index_p].offset);
                }
                else
                {
                    PRT_CELL_SET("%s", "-");
                }
            }
            else if (key_info_p->attach_info[*last_qual_index_p].input_type == DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD ||
                     key_info_p->attach_info[*last_qual_index_p].input_type ==
                     DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE)
            {
                PRT_CELL_SET("%d", key_info_p->attach_info[*last_qual_index_p].input_arg);
                if (DNX_QUAL_CLASS(key_info_p->key_template.key_qual_map[*last_qual_index_p].qual_type) ==
                    DNX_FIELD_QUAL_CLASS_USER)
                {
                    PRT_CELL_SET("%d", key_info_p->attach_info[*last_qual_index_p].offset);
                }
                else
                {
                    PRT_CELL_SET("%s", "-");
                }
            }
            else if (key_info_p->attach_info[*last_qual_index_p].input_type == DNX_FIELD_INPUT_TYPE_KBP)
            {
                PRT_CELL_SET("%s", "-");
                PRT_CELL_SET("%d", key_info_p->attach_info[*last_qual_index_p].offset);
            }
            else if (key_info_p->attach_info[*last_qual_index_p].input_type == DNX_FIELD_INPUT_TYPE_CONST)
            {
                PRT_CELL_SET("0x%X", key_info_p->attach_info[*last_qual_index_p].input_arg);
                PRT_CELL_SET("%s", "-");
            }
            else
            {
                PRT_CELL_SET("%d", key_info_p->attach_info[*last_qual_index_p].input_arg);
                PRT_CELL_SET("%d", key_info_p->attach_info[*last_qual_index_p].offset);
            }

            PRT_CELL_SET("%s", ctx_mode_name_p);

            /**
             * Update the any_key_was_displayed_p flag,
             * because we set information at least for one key.
             */
            *any_key_was_displayed_p = TRUE;
            ffc_id_index++;
            (*number_of_used_ffc_per_qual_p)--;
        }

        if (ffc_id[ffc_id_index] == DNX_FIELD_FFC_ID_INVALID)
        {
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets relevant information about usage of the initial keys
 *   by context modes (compare, hashing and state_table).
 * \param [in] unit - The unit number.
 * \param [in] new_ctx_print_p - Flag to indicate whether to print
 *  the context ID in the relevant sell or to skip it.
 * \param [in] new_key_print_p - Flag to indicate whether to print
 *  the initial key ID in the relevant sell or to skip it.
 * \param [in] context_id - Context Id for which information for
 *  initial key usage have to be presented.
 * \param [in] context_name_p - Name of the given context ID to be
 *  presented in the relevant cell.
 * \param [in] initial_key_id - Initial key for which information
 *  will be presented.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 * \param [out] last_qual_index_p - Last qualifier index, on which iteration stoped.
 * \param [out] number_of_used_ffc_per_qual_p - How many FFC left to be presented in case of
 *  qualifier, which is using two keys.
 * \param [out] any_key_was_displayed_p - Flag to be set to TRUE in case any data
 *  about for the given initial key was set. Default value is FALSE.
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_key_initial_ctx_mode_key_usage_set(
    int unit,
    uint8 *new_ctx_print_p,
    uint8 *new_key_print_p,
    dnx_field_context_t context_id,
    char *context_name_p,
    dbal_enum_value_field_field_key_e initial_key_id,
    prt_control_t * prt_ctr,
    uint32 *last_qual_index_p,
    uint8 *number_of_used_ffc_per_qual_p,
    int *any_key_was_displayed_p)
{
    dnx_field_context_mode_t context_mode;
    dnx_field_context_compare_info_t compare_info;
    dnx_field_context_hashing_info_t hashing_info;
    char ctx_mode_name[DIAG_DNX_FIELD_UTILS_STR_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));
    /** If the current context is allocated, get the information about it. */
    SHR_IF_ERR_EXIT(dnx_field_context_mode_get(unit, DNX_FIELD_STAGE_IPMF1, context_id, &context_mode));

    /** Get information for compare, hash, state table modes for the current context. */
    SHR_IF_ERR_EXIT(diag_dnx_field_utils_cmp_hash_key_usage_info_get
                    (unit, context_id, context_mode, &compare_info, &hashing_info));

    /** In case one of the compare pairs has valid mode, present its information, otherwise skip it. */
    if (context_mode.context_ipmf1_mode.compare_mode_1 != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE ||
        context_mode.context_ipmf1_mode.compare_mode_2 != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE)
    {
        uint8 cmp_key_index;
        uint8 max_num_cmp_keys;
        uint8 cmp_pair_index;
        uint8 max_num_cmp_pairs;
        max_num_cmp_pairs = dnx_data_field.common_max_val.nof_compare_pairs_get(unit);
        max_num_cmp_keys = dnx_data_field.common_max_val.nof_compare_keys_in_compare_mode_get(unit);

        for (cmp_pair_index = 0; cmp_pair_index < max_num_cmp_pairs; cmp_pair_index++)
        {
            for (cmp_key_index = 0; cmp_key_index < max_num_cmp_keys; cmp_key_index++)
            {
                uint32 cmp_last_qual_index = 0;
                uint8 cmp_number_of_used_ffc_per_qual = 0;
                if (cmp_pair_index == DNX_FIELD_CONTEXT_COMPARE_MODE_PAIR_1)
                {
                    sal_strncpy(ctx_mode_name, "CMP1", DIAG_DNX_FIELD_UTILS_STR_SIZE - 1);
                    SHR_IF_ERR_EXIT(diag_dnx_field_key_initial_ctx_mode_single_key_usage_cmp_hash_info_set
                                    (unit, new_ctx_print_p, new_key_print_p, context_id, context_name_p, initial_key_id,
                                     &compare_info.pair_1.key_info[cmp_key_index],
                                     ctx_mode_name, prt_ctr, &cmp_last_qual_index, &cmp_number_of_used_ffc_per_qual,
                                     any_key_was_displayed_p));
                }
                else
                {
                    sal_strncpy(ctx_mode_name, "CMP2", DIAG_DNX_FIELD_UTILS_STR_SIZE - 1);
                    SHR_IF_ERR_EXIT(diag_dnx_field_key_initial_ctx_mode_single_key_usage_cmp_hash_info_set
                                    (unit, new_ctx_print_p, new_key_print_p, context_id, context_name_p, initial_key_id,
                                     &compare_info.pair_2.key_info[cmp_key_index],
                                     ctx_mode_name, prt_ctr, &cmp_last_qual_index, &cmp_number_of_used_ffc_per_qual,
                                     any_key_was_displayed_p));
                }
            }
        }
    }

    /** In case hashing has valid mode, present its information, otherwise skip it. */
    if (context_mode.context_ipmf1_mode.hash_mode != DNX_FIELD_CONTEXT_HASH_MODE_DISABLED)
    {
        sal_strncpy(ctx_mode_name, "HASH", DIAG_DNX_FIELD_UTILS_STR_SIZE - 1);
        SHR_IF_ERR_EXIT(diag_dnx_field_key_initial_ctx_mode_single_key_usage_cmp_hash_info_set
                        (unit, new_ctx_print_p, new_key_print_p, context_id, context_name_p, initial_key_id,
                         &hashing_info.key_info, ctx_mode_name, prt_ctr, last_qual_index_p,
                         number_of_used_ffc_per_qual_p, any_key_was_displayed_p));
    }

    /** In case state table has valid mode, present its information, otherwise skip it. */
    if (context_mode.state_table_mode != DNX_FIELD_CONTEXT_STATE_TABLE_MODE_DISABLED)
    {
        int state_table_use_acr;
        int state_table_lsb_on_key;
        int state_table_key_size;
        dnx_field_stage_e state_table_field_stage;
        dbal_enum_value_field_field_key_e state_table_key_id;
        SHR_IF_ERR_EXIT(dnx_field_map_state_table_source_get
                        (unit, &state_table_use_acr, &state_table_field_stage, &state_table_key_id,
                         &state_table_lsb_on_key, &state_table_key_size));
        /*
         * Sanity check
         */
        if (state_table_use_acr)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "State table actions not supported. \n");
        }
        /*
         * Only if the state table is in iPMF1, it is relevant for initial keys.
         * Check if the initial key is the key used by the state table.
         */
        if (state_table_field_stage == DNX_FIELD_STAGE_IPMF1 && state_table_key_id == initial_key_id)
            SHR_IF_ERR_EXIT(diag_dnx_field_key_initial_ctx_mode_key_usage_state_table_info_set
                            (unit, new_ctx_print_p, new_key_print_p, context_id, context_name_p,
                             initial_key_id, prt_ctr, any_key_was_displayed_p));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays allocated initial keys.
 *   Data can be sorted by context_id and initial key, specified by the caller.
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
diag_dnx_field_key_initial_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_context_t context_lower, context_upper;
    dnx_field_context_t context_index;
    dbal_enum_value_field_field_key_e initial_key_lower, initial_key_upper;
    dbal_enum_value_field_field_key_e initial_key_index;
    int any_key_was_displayed;
    uint8 ipmf1_ctx_is_alloc, ipmf2_ctx_is_alloc;
    char context_name[DBAL_MAX_STRING_LENGTH];
    uint8 new_ctx_print, new_key_print;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_KEY, initial_key_lower, initial_key_upper);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_CONTEXT, context_lower, context_upper);

    /*
     * 0xFFFFFFFF will mark all contexts for context
     */
    if ((context_lower == 0xFFFFFFFF) && (context_upper == 0xFFFFFFFF))
    {
        context_lower = 0;
        context_upper = dnx_data_field.common_max_val.nof_contexts_get(unit) - 1;
    }
    if ((int) context_lower < 0 || context_lower >= dnx_data_field.common_max_val.nof_contexts_get(unit) ||
        (int) context_upper < 0 || context_upper >= dnx_data_field.common_max_val.nof_contexts_get(unit))
    {
        LOG_CLI_EX("\r\n" "Context provided (%d-%d) must be within range 0-%d.%s\r\n\n", context_lower, context_upper,
                   dnx_data_field.common_max_val.nof_contexts_get(unit) - 1, EMPTY);
        SHR_EXIT();
    }

    any_key_was_displayed = FALSE;
    new_ctx_print = FALSE;
    new_key_print = FALSE;

    PRT_TITLE_SET("Initial Keys Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "CTX ID/Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Types");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FFC ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Offset in Key");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Input Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Input Arg");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Offset");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Group (ID/Name) / Mode");

    /** Iterate over all contexts or the range specified by the caller. */
    for (context_index = context_lower; context_index <= context_upper; context_index++)
    {
        uint8 number_of_used_ffc_per_qual = 0;
        uint32 last_qual_index = 0;
        /** Check if the current Context ID is allocated for IPMF1 stage. */
        SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated
                        (unit, DNX_FIELD_STAGE_IPMF1, context_index, &ipmf1_ctx_is_alloc));
        /**
         * Check if the current Context ID is allocated for IPMF2 stage.
         * Done in case we have context switch.
         */
        SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated
                        (unit, DNX_FIELD_STAGE_IPMF2, context_index, &ipmf2_ctx_is_alloc));

        if (!ipmf1_ctx_is_alloc && !ipmf2_ctx_is_alloc)
        {
            continue;
        }

        sal_memset(context_name, 0, sizeof(context_name));
        if (ipmf1_ctx_is_alloc)
        {
            /** Take the name for current context from the SW state. */
            SHR_IF_ERR_EXIT(dnx_field_context_sw.context_info.name.
                            value.stringget(unit, context_index, DNX_FIELD_STAGE_IPMF1, context_name));
        }
        else
        {
            /** Take the name for current context from the SW state. */
            SHR_IF_ERR_EXIT(dnx_field_context_sw.context_info.name.
                            value.stringget(unit, context_index, DNX_FIELD_STAGE_IPMF2, context_name));
        }

        /** Indicates that new context have to be printed. */
        new_ctx_print = TRUE;

        /** Iterate over all initial keys or the range specified by the caller. */
        for (initial_key_index = initial_key_lower; initial_key_index <= initial_key_upper; initial_key_index++)
        {
            /** Indicates that new initial key have to be printed. */
            new_key_print = TRUE;

            /** Present field groups, which are using current initial key, and relevant information about it. */
            SHR_IF_ERR_EXIT(diag_dnx_field_key_initial_fg_key_usage_set(unit, &new_ctx_print, &new_key_print,
                                                                        context_index, context_name, initial_key_index,
                                                                        prt_ctr, &any_key_was_displayed));

            /** Check the context modes key usage only for IPMF1 context. */
            if (ipmf1_ctx_is_alloc)
            {
                /**
                 * Present context modes (CMP, HASH. STATE_TABLE),
                 * which are using current initial key and relevant information to it.
                 */
                SHR_IF_ERR_EXIT(diag_dnx_field_key_initial_ctx_mode_key_usage_set(unit, &new_ctx_print, &new_key_print,
                                                                                  context_index, context_name,
                                                                                  initial_key_index, prt_ctr,
                                                                                  &last_qual_index,
                                                                                  &number_of_used_ffc_per_qual,
                                                                                  &any_key_was_displayed));
            }
        }
    }

    if (!any_key_was_displayed)
    {
        LOG_CLI_EX("\r\n" "NO occupied initial key was found within specified ranges!! %s%s%s%s\r\n\n", EMPTY, EMPTY,
                   EMPTY, EMPTY);
        SHR_EXIT();
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/** Context ID to be used for init and deinit procedures. */
static bcm_field_context_t context_id;

/**
 * \brief
 *   This function allocates some resources to be used
 *   for testing during the 'field key initial exec' command.
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_key_initial_init_before_run(
    int unit)
{
    bcm_field_context_info_t context_info;
    bcm_field_context_hash_info_t hash_info;
    bcm_field_context_compare_info_t compare_info;
    void *dest_char;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "key_initial_diag", sizeof(context_info.name));
    context_info.hashing_enabled = TRUE;
    context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeDouble;
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id));

    bcm_field_context_compare_info_t_init(&compare_info);
    compare_info.first_key_info.nof_quals = 3;
    compare_info.first_key_info.qual_types[0] = bcmFieldQualifyL4DstPort;
    compare_info.first_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.first_key_info.qual_info[0].input_arg = 2;
    compare_info.first_key_info.qual_info[0].offset = 0;
    compare_info.first_key_info.qual_types[1] = bcmFieldQualifyColor;
    compare_info.first_key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    compare_info.first_key_info.qual_types[2] = bcmFieldQualifyIpProtocol;
    compare_info.first_key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.first_key_info.qual_info[2].input_arg = 1;
    compare_info.first_key_info.qual_info[2].offset = 0;

    compare_info.second_key_info.nof_quals = 1;
    compare_info.second_key_info.qual_types[0] = bcmFieldQualifyL4SrcPort;
    compare_info.second_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.second_key_info.qual_info[0].input_arg = 2;
    compare_info.second_key_info.qual_info[0].offset = 0;
    SHR_IF_ERR_EXIT(bcm_field_context_compare_create(unit, 0, bcmFieldStageIngressPMF1, context_id, 0, &compare_info));

    bcm_field_context_hash_info_t_init(&hash_info);
    hash_info.hash_function = bcmFieldContextHashFunctionCrc16Bisync;
    hash_info.order = TRUE;
    hash_info.hash_config.function_select = bcmFieldContextHashActionValueReplaceCrc;
    hash_info.hash_config.action_key = bcmFieldContextHashActionKeyAdditionalLbKey;
    hash_info.key_info.nof_quals = 5;
    hash_info.key_info.qual_types[0] = bcmFieldQualifySrcIp;
    hash_info.key_info.qual_types[1] = bcmFieldQualifyDstIp;
    hash_info.key_info.qual_types[2] = bcmFieldQualifyIpProtocol;
    hash_info.key_info.qual_types[3] = bcmFieldQualifyL4SrcPort;
    hash_info.key_info.qual_types[4] = bcmFieldQualifyL4DstPort;
    hash_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[0].input_arg = 1;
    hash_info.key_info.qual_info[0].offset = 0;
    hash_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[1].input_arg = 1;
    hash_info.key_info.qual_info[1].offset = 0;
    hash_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[2].input_arg = 1;
    hash_info.key_info.qual_info[2].offset = 0;
    hash_info.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[3].input_arg = 2;
    hash_info.key_info.qual_info[3].offset = 0;
    hash_info.key_info.qual_info[4].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[4].input_arg = 2;
    hash_info.key_info.qual_info[4].offset = 0;
    SHR_IF_ERR_EXIT(bcm_field_context_hash_create(unit, 0, bcmFieldStageIngressPMF1, context_id, &hash_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function deallocates all resources
 *   allocated by diag_dnx_field_key_initial_init_before_run.
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_key_initial_deinit_after_run(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Compare destroy the first pair_id. */
    SHR_IF_ERR_EXIT(bcm_field_context_compare_destroy(unit, bcmFieldStageIngressPMF1, context_id, 0));

    /** Hashing destroy. */
    SHR_IF_ERR_EXIT(bcm_field_context_hash_destroy(unit, bcmFieldStageIngressPMF1, context_id));

    /** Destroy the context. */
    SHR_IF_ERR_EXIT(bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, context_id));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   This function allocates some resources to be used
*   for testing during the 'field key last exec' command.
* \param [in] unit - The unit number.
*
* \return
*   Error code (as per 'shr_error_e').
* \see
*   shr_error_e
*/
static shr_error_e
diag_dnx_field_key_last_init_before_run(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(diag_dnx_field_utils_send_itmh_packet(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/**
 * \brief
 *  This function will indicate key allocation state: Is there any occupied key
 *  on specified pair - stage and context
 * \param [in] unit               - Device ID
 * \param [in] field_stage        -
 *   dnx_field_stage_e. Stage to use for searching key occupation state
 * \param [in] context_id            -
 *   dnx_field_context_t. Context id (program) to use for searching key occupation state
 * \param [in] bit_range_only            -
 *   int. If this input is 'zero' then ignore it. If non-zero then search only
 *   'field group types' which support the 'bit-range' feature. If feature is
 *   not soppurted, this procedure will flag 'not occupied'
 * \param [out] none_occupied_p            -
 *   int pointer. This procedure loads pointed memory by a non-zero value
 *   if no key is occupied for this stage/context pair.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * Output of this procedure is based on keys occupation as stored on SW state.
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_field_key_is_any_key_occupied(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    int bit_range_only,
    int *none_occupied_p)
{
    uint8 key_nof;
    uint8 key_iter;
    uint8 key_lookup_allocated;
    uint8 key_bit_allocated;
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
    /*
     * Last key allowed on stage.
     * See dbal_enum_value_field_field_key_e.
     */
    uint8 last_available_key_on_stage;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(none_occupied_p, _SHR_E_PARAM, "none_occupied_p");

    DNX_FIELD_STAGE_VERIFY(field_stage);
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);

    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                    nof_available_keys_on_stage.get(unit, field_stage, &nof_available_keys_on_stage));
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                    first_available_key_on_stage.get(unit, field_stage, &first_available_key_on_stage));
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.nof_available_keys_on_stage.get(unit, field_stage, &key_nof));
    if (!(key_nof))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "field_stage (%s) has no keys to allocate from.\r\n",
                     dnx_field_stage_text(unit, field_stage));
    }
    *none_occupied_p = TRUE;
    last_available_key_on_stage = first_available_key_on_stage + key_nof - 1;
    for (key_iter = first_available_key_on_stage; key_iter <= last_available_key_on_stage; key_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_key_key_lookup_occupation_bmp_is_allocated
                        (unit, field_stage, context_id, key_iter, &key_lookup_allocated));
        if (key_lookup_allocated)
        {
            *none_occupied_p = FALSE;
            break;
        }
        SHR_IF_ERR_EXIT(dnx_algo_field_key_key_occupation_bmp_is_any_occupied
                        (unit, field_stage, context_id, key_iter, 0, DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE,
                         &key_bit_allocated));
        if (key_bit_allocated)
        {
            *none_occupied_p = FALSE;
            break;
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

                                bit_lsb_nosort[nof_fg_ids] = context_attach_info.key_id.lsb[key_id_index];
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
                            case DNX_FIELD_GROUP_TYPE_KBP:
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
 * \param [in] add_new_stage_p            -
 *   Flag to indicate if 'stage' cell should be filled (TRUE) or no (FALSE).
 * \param [in] add_new_context_p            -
 *   Flag to indicate if 'context' cell should be filled (TRUE) or no (FALSE).
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
    uint8 *add_new_stage_p,
    uint8 *add_new_context_p,
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
    char *dnx_field_prefix_field_stage;
    int dnx_field_prefix_field_stage_size;
    dnx_field_group_t fg_ids[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS + 1];
    int bit_lsb[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS + 1];
    int bit_msb[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS + 1];
    int nof_fgs_found = -1;
    int fg_index;
    dnx_field_group_type_e fg_type;
    field_group_name_t group_name;

    SHR_FUNC_INIT_VARS(unit);
    dnx_field_prefix_field_stage = "DNX_FIELD_STAGE_";
    dnx_field_prefix_field_stage_size = strlen(dnx_field_prefix_field_stage);
    dnx_field_prefix_group_type = "DNX_FIELD_GROUP_TYPE_";
    dnx_field_prefix_group_type_size = strlen(dnx_field_prefix_group_type);
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
    if (*add_new_stage_p)
    {
        if (field_stage == DNX_FIELD_STAGE_IPMF1)
        {
            /*
             * Since dnx_field_stage_e_get_name() would return "DNX_FIELD_STAGE_FIRST" in this case,
             * we need to create a special handling.
             */
            PRT_CELL_SET("%s", "IPMF1");
        }
        else
        {
            PRT_CELL_SET("%s", &(dnx_field_stage_e_get_name(field_stage)[dnx_field_prefix_field_stage_size]));
        }
        *add_new_stage_p = FALSE;
    }
    else
    {
        /** Skip the stage cell.*/
    PRT_CELL_SKIP(1)}
    if (*add_new_context_p)
    {
        dnx_field_context_mode_t context_mode;
        /*
         * Find the context name.
         */
        SHR_IF_ERR_EXIT(dnx_field_context_mode_get(unit, field_stage, context_id, &context_mode));
        if (sal_strncmp(context_mode.name, "", sizeof(context_mode.name)))
        {
            PRT_CELL_SET("%d / \"%.*s\"", context_id, (int) sizeof(context_mode.name), context_mode.name);
        }
        else
        {
            PRT_CELL_SET("%d / \"%s\"", context_id, "N/A");
        }
        *add_new_context_p = FALSE;
    }
    else
    {
        /** Skip the context cell.*/
    PRT_CELL_SKIP(1)}
    /*
     * Skip all other cells
     */
    PRT_CELL_SKIP(3);
    for (key_iter = first_available_key_on_stage; key_iter <= last_available_key_on_stage; key_iter++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        /** skip stage and context stages.*/
        PRT_CELL_SKIP(2);
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
                /** skip stage, context and key.*/
                PRT_CELL_SKIP(3);
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
 *   This function displays keys per range as specified by the caller.
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
diag_dnx_field_key_info_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e stage_lower, stage_upper;
    dnx_field_context_t context_lower, context_upper;
    int non_empty_only, any_key_was_displayed;
    dnx_field_stage_e stage_index;
    dnx_field_context_t context_index;
    uint8 add_new_stage;
    uint8 add_new_context;
    uint8 context_is_allocated;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_STAGE, stage_lower, stage_upper);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_CONTEXT, context_lower, context_upper);
    SH_SAND_GET_BOOL(DIAG_DNX_FIELD_OPTION_VALID, non_empty_only);
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "%s(), line %d:\r\n"
                 "Display non empty entries only: %d%s\r\n", __func__, __LINE__, non_empty_only, EMPTY);
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "==> stages: from %d to %d, Contexts: from %d to %d\r\n",
                 stage_lower, stage_upper, context_lower, context_upper);

    /** 0xFFFFFFFF will mark all contexts for context. */
    if ((context_lower == 0xFFFFFFFF) && (context_upper == 0xFFFFFFFF))
    {
        context_lower = 0;
        context_upper = dnx_data_field.common_max_val.nof_contexts_get(unit) - 1;
    }
    if ((int) context_lower < 0 || context_lower >= dnx_data_field.common_max_val.nof_contexts_get(unit) ||
        (int) context_upper < 0 || context_upper >= dnx_data_field.common_max_val.nof_contexts_get(unit))
    {
        LOG_CLI_EX("\r\n" "Context provided (%d-%d) must be within range 0-%d.%s\r\n\n", context_lower, context_upper,
                   dnx_data_field.common_max_val.nof_contexts_get(unit) - 1, EMPTY);
        SHR_EXIT();
    }

    any_key_was_displayed = FALSE;
    PRT_TITLE_SET("");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Stage");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Context ID/Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Group ID/Name (type)");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Bits Allocated");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    for (stage_index = stage_lower; stage_index <= stage_upper; stage_index++)
    {
        int none_occupied, do_display;
        dnx_field_stage_e field_stage;
        add_new_stage = TRUE;

        /** Initialization to silence the compiler.*/
        none_occupied = TRUE;

        SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                        (unit, Field_key_diag_stage_enum_table[stage_index].string, &field_stage));

        for (context_index = context_lower; context_index <= context_upper; context_index++)
        {
            SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated(unit, field_stage, context_index, &context_is_allocated));
            if (context_is_allocated == FALSE)
            {
                continue;
            }
            do_display = FALSE;
            add_new_context = TRUE;
            if (non_empty_only)
            {
                /*
                 * If caller requests to only see allocated/occupied keys, set 'do_display' accordingly.
                 */
                SHR_IF_ERR_EXIT(diag_dnx_field_key_is_any_key_occupied
                                (unit, field_stage, context_index, FALSE, &none_occupied));
                if (!none_occupied)
                {
                    do_display = TRUE;
                }
            }
            else
            {
                /*
                 * If caller does not request to only see allocated/occupied keys, then display all.
                 */
                do_display = TRUE;
            }
            if (do_display)
            {
                any_key_was_displayed = TRUE;
                SHR_IF_ERR_EXIT(dnx_field_diag_display_key_occupation_partial
                                (unit, field_stage, context_index, &add_new_stage, &add_new_context, sand_control,
                                 prt_ctr));
            }
        }
    }
    if (!any_key_was_displayed)
    {
        LOG_CLI_EX("\r\n" "NO occupied key was found within specified ranges!! %s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY,
                   EMPTY);
        SHR_EXIT();
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/*
 * Global and Static
 * {
 */
/**
 *  'Help' description for KEY display (shell commands).
 */
static sh_sand_man_t field_key_last_key_info_man = {
    .brief = "Dump a given key and display information about it in a table.",
    .full = "Field stage, for which we want to retrieve key information, should be specified. Can be: 'ipmf1' or 'ipmf2' or 'ipmf3' or 'epmf', \r\n",
            "Key should be specified if not the default is to display info for all keys of the given stage.\r\n",
    .synopsis = "field key last stage=<IPMF1 | IPMF2 | IPMF3 | EPMF> key=<A - J | ALL>",
    .examples = "stage=ipmf1 key=all\n"
                "stage=IPMF1 key=A\n"
                "stage=ipmf2 key=F\n"
                "stage=ipmf2 key=I",
    .init_cb = diag_dnx_field_key_last_init_before_run,
};

static sh_sand_option_t field_key_last_key_info_options[] = {
    /* Name */                                                  /* Type */                           /* Description */                               /* Default */
    {DIAG_DNX_FIELD_OPTION_STAGE,  SAL_FIELD_TYPE_ENUM,    "Stage to get key info for (ipmf1, ipmf2, ipmf3, epmf)",    "ipmf1",  (void *)Field_key_diag_stage_enum_table},
    {DIAG_DNX_FIELD_OPTION_KEY,    SAL_FIELD_TYPE_ENUM,    "Key to be displayed (A-J or ALL)",                         "all",  (void *)Field_key_enum_table},
    {NULL}
    /** End of options list - must be last. */
};
/*
 * }
 */
/* *INDENT-ON* */

/**
 * \brief
 *  This function fills a table with information,
 *  about a given key, taken from DBAL.
 * \param [in] unit -
 *  The unit number.
 * \param [in] core_id -
 *  From which core, the DBAL table info should be taken.
 * \param [in] key_id -
 *  Id of the key for which information should be presented.
 * \param [in] key_name_p -
 *  String, which contains the key name,
 *  parsed to the table to be displayed.
 * \param [in] entry_handle_id -
 *  Handle to the DBAL table.
 * \param [in] prt_ctr -
 *  Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_key_last_key_info_table_fill(
    int unit,
    int core_id,
    int key_id,
    char *key_name_p,
    uint32 entry_handle_id,
    prt_control_t * prt_ctr)
{
    /**
     * An array of 5 elements to store the returned
     * 160 bits key information, from the DBAL table.
     */
    uint32 field_key_value[5];

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Specifying from which core, the DBAL table info should be taken.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    /*
     * Setting the key ID, for which we want to retrieve an information.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_KEY, key_id);

    /*
     * Perform the DBAL read
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /*
     * Get a result fields
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_LAST_KEY, INST_SINGLE, field_key_value));

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    PRT_CELL_SET("KEY_%s", key_name_p);

    PRT_CELL_SET("0x%08x%08x%08x%08x%08x", field_key_value[4],
                 field_key_value[3], field_key_value[2], field_key_value[1], field_key_value[0]);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function dumps and displays information about a given key.
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
diag_dnx_field_key_last_key_info_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e field_stage;
    int field_key;
    dbal_tables_e table_id;
    uint32 entry_handle_id;
    int key_id;
    int core_id, nof_cores;
    int max_nof_keys_ipmf2;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get the inputs from Shell
     */
    SH_SAND_GET_ENUM(DIAG_DNX_FIELD_OPTION_STAGE, field_stage);
    SH_SAND_GET_ENUM(DIAG_DNX_FIELD_OPTION_KEY, field_key);

    SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                    (unit, Field_key_diag_stage_enum_table[field_stage].string, &field_stage));
    /**
     * Remove following check and add support for other stages,
     * once the debug registers for them have been added to the adapter.
     */
    if (field_stage == DNX_FIELD_STAGE_EPMF)
    {
        LOG_CLI_EX("\r\n" "Field stage %s is not supported!%s%s%s\r\n\n",
                   dnx_field_stage_e_get_name(field_stage), EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }

#ifdef ADAPTER_SERVER_MODE
    if (field_stage == DNX_FIELD_STAGE_IPMF3)
    {
        LOG_CLI_EX("\r\n" "Field stage %s is not supported on adapter!%s%s%s\r\n\n",
                   dnx_field_stage_e_get_name(field_stage), EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }
#endif

    if (field_key != DNX_FIELD_KEY_ALL)
    {
        if (field_stage == DNX_FIELD_STAGE_IPMF1 &&
            (field_key < DBAL_ENUM_FVAL_FIELD_KEY_A || field_key > DBAL_ENUM_FVAL_FIELD_KEY_E))
        {
            LOG_CLI_EX("\r\n" "Illegal key_id = %d, valid values are [%d (KEY_A) - %d (KEY_E)], stage IPMF1!%s\r\n\n",
                       field_key, DBAL_ENUM_FVAL_FIELD_KEY_A, DBAL_ENUM_FVAL_FIELD_KEY_E, EMPTY);
            SHR_EXIT();
        }

        if (field_stage == DNX_FIELD_STAGE_IPMF2 &&
            (field_key < DBAL_ENUM_FVAL_FIELD_KEY_F || field_key > DBAL_ENUM_FVAL_FIELD_KEY_J))
        {
            LOG_CLI_EX("\r\n"
                       "Illegal initial key_id = %d, valid values are [%d (KEY_F) - %d (KEY_J)], stage IPMF2!%s\r\n\n",
                       field_key, DBAL_ENUM_FVAL_FIELD_KEY_F, DBAL_ENUM_FVAL_FIELD_KEY_J, EMPTY);
            SHR_EXIT();
        }

        if (field_stage == DNX_FIELD_STAGE_IPMF3 &&
            (field_key < DBAL_ENUM_FVAL_FIELD_KEY_A || field_key > DBAL_ENUM_FVAL_FIELD_KEY_C))
        {
            LOG_CLI_EX("\r\n" "Illegal key_id = %d, valid values are [%d (KEY_A) - %d (KEY_C)], stage IPMF3!%s\r\n\n",
                       field_key, DBAL_ENUM_FVAL_FIELD_KEY_A, DBAL_ENUM_FVAL_FIELD_KEY_C, EMPTY);
            SHR_EXIT();
        }
    }

    /** Get max number of cores. */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    max_nof_keys_ipmf2 = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF2)->nof_keys_alloc * 2;

    table_id = Last_key_info_tables[field_stage];
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    for (core_id = 0; core_id < nof_cores; core_id++)
    {
        PRT_TITLE_SET("Table: %s   Core: %d", dbal_logical_table_to_string(unit, table_id), core_id);

        PRT_COLUMN_ADD("Key");
        PRT_COLUMN_ADD("Value");

        /*
         * If caller wants to see information about all keys (option "key=all") for a given stage.
         */
        if (field_key == DNX_FIELD_KEY_ALL)
        {
            if (field_stage == DNX_FIELD_STAGE_IPMF2)
            {
                /**
                 * "key_id" started from 5 because in IPMF2 we have only 5 keys,
                 * which are from "F" to "J".
                 */
                for (key_id = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_keys_alloc;
                     key_id < max_nof_keys_ipmf2; key_id++)
                {
                    SHR_IF_ERR_EXIT(diag_dnx_field_key_last_key_info_table_fill
                                    (unit, core_id, key_id, Field_key_enum_table[key_id].string, entry_handle_id,
                                     prt_ctr));
                }
            }
            else
            {
                for (key_id = 0; key_id < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_keys_alloc;
                     key_id++)
                {
                    SHR_IF_ERR_EXIT(diag_dnx_field_key_last_key_info_table_fill
                                    (unit, core_id, key_id, Field_key_enum_table[key_id].string, entry_handle_id,
                                     prt_ctr));
                }
            }
        }
        else
        {
            /*
             * If caller wants to see information about a specific key (option "key=A-J") for a given stage.
             */
            SHR_IF_ERR_EXIT(diag_dnx_field_key_last_key_info_table_fill
                            (unit, core_id, field_key, Field_key_enum_table[field_key].string, entry_handle_id,
                             prt_ctr));
        }

        PRT_COMMITX;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * Globals required at end because they reference static procedures/tables above.
 * {
 */

/**
 *  'Help' description for 'KEY INITIAL' (shell commands).
 */
static sh_sand_man_t Field_key_initial_man = {
    .brief = "'Initial key' related display utilities",
    .full =
        "'Key' display of 'initial' keys. For all inputs {'group' (field group), 'context', 'group type' and 'initial key'\r\n"
        "a range may be specified.\r\n" "If no context is specified then 'minimal val'-'maximal val' is assumed.\r\n"
        "If only one value is specified for context/stage/group_type then this 'single value' range is assumed.\r\n"
        "If 'init_key' is not specified then all used initial keys are displayed. Otherwise, only specified 'initial key's"
        " are displayed.\r\n",
    .synopsis =
        " [context=<<Lowest context number>-<Highest context number>>]" " [initial_key=<<Lowest key>-<<Highest key>>]",
    .examples = "key=MIN-MAX context=0-63",
    .init_cb = diag_dnx_field_key_initial_init_before_run,
    .deinit_cb = diag_dnx_field_key_initial_deinit_after_run
};

static sh_sand_option_t Field_key_initial_options[] = {
    {DIAG_DNX_FIELD_OPTION_CONTEXT, SAL_FIELD_TYPE_UINT32, "Lowest-highest context ID to get initial keys of",
     "0xFFFFFFFF"},
    {DIAG_DNX_FIELD_OPTION_KEY, SAL_FIELD_TYPE_UINT32, "Initial key to be displayed (F-J)", "MIN-MAX",
     (void *) Field_init_key_enum_table, "MIN-MAX"},
    {NULL}
};

sh_sand_cmd_t Sh_dnx_field_key_cmds[] = {
    {"info", diag_dnx_field_key_info_cb, NULL, Field_key_info_options,
     &Field_key_info_man},
    {"initial", diag_dnx_field_key_initial_cb, NULL, Field_key_initial_options,
     &Field_key_initial_man},
    {"last", diag_dnx_field_key_last_key_info_cb, NULL, field_key_last_key_info_options,
     &field_key_last_key_info_man},
    {NULL}
};
/*
 * }
 */
