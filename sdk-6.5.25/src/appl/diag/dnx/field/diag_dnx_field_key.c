/** \file diag_dnx_field_key.c
 *
 * 'key' operations procedures for DNX.
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
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
                         DNX_FIELD_INPUT_TYPE_EXTERNAL)
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
                         DNX_FIELD_INPUT_TYPE_EXTERNAL)
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
            else if (key_info_p->attach_info[*last_qual_index_p].input_type == DNX_FIELD_INPUT_TYPE_EXTERNAL)
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
        max_num_cmp_pairs = dnx_data_field.compare.nof_compare_pairs_get(unit);
        max_num_cmp_keys = dnx_data_field.compare.nof_keys_get(unit);

        for (cmp_pair_index = 0; cmp_pair_index < max_num_cmp_pairs; cmp_pair_index++)
        {
            for (cmp_key_index = 0; cmp_key_index < max_num_cmp_keys; cmp_key_index++)
            {
                uint32 cmp_last_qual_index = 0;
                uint8 cmp_number_of_used_ffc_per_qual = 0;
                if (cmp_pair_index == DNX_FIELD_CONTEXT_COMPARE_MODE_PAIR_1)
                {
                    sal_strncpy_s(ctx_mode_name, "CMP1", DIAG_DNX_FIELD_UTILS_STR_SIZE);
                    SHR_IF_ERR_EXIT(diag_dnx_field_key_initial_ctx_mode_single_key_usage_cmp_hash_info_set
                                    (unit, new_ctx_print_p, new_key_print_p, context_id, context_name_p, initial_key_id,
                                     &compare_info.pair_1.key_info[cmp_key_index],
                                     ctx_mode_name, prt_ctr, &cmp_last_qual_index, &cmp_number_of_used_ffc_per_qual,
                                     any_key_was_displayed_p));
                }
                else
                {
                    sal_strncpy_s(ctx_mode_name, "CMP2", DIAG_DNX_FIELD_UTILS_STR_SIZE);
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
        sal_strncpy_s(ctx_mode_name, "HASH", DIAG_DNX_FIELD_UTILS_STR_SIZE);
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
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(diag_dnx_field_generic_init(unit, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
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
    .examples = "" "\n" "key=5-7 context=0-20",
    .init_cb = diag_dnx_field_key_initial_init_before_run,
};

static sh_sand_option_t Field_key_initial_options[] = {
    {DIAG_DNX_FIELD_OPTION_CONTEXT, SAL_FIELD_TYPE_UINT32, "Lowest-highest context ID to get initial keys of",
     "0xFFFFFFFF"},
    {DIAG_DNX_FIELD_OPTION_KEY, SAL_FIELD_TYPE_UINT32, "Initial key to be displayed (F-J)", "MIN-MAX",
     (void *) Field_init_key_enum_table, "MIN-MAX"},
    {NULL}
};

sh_sand_cmd_t Sh_dnx_field_key_cmds[] = {
    {"initial", diag_dnx_field_key_initial_cb, NULL, Field_key_initial_options,
     &Field_key_initial_man},
    {NULL}
};
/*
 * }
 */
