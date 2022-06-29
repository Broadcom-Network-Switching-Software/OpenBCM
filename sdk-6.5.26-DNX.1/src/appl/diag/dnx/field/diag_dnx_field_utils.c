/** \file diag_dnx_field_utils.c
 *
 * Diagnostics utilities to be used for DNX diagnostics of Field Module.
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
#include <appl/diag/sand/diag_sand_packet.h>
#include <bcm_int/dnx/field/field_context.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_context_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_qualifier_access.h>
#include <soc/dnx/field/tcam/tcam_handler.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <soc/sand/sand_signals.h>
#include "diag_dnx_field_utils.h"

#if defined(INCLUDE_KBP)
#include <soc/dnx/swstate/auto_generated/access/dnx_field_kbp_access.h>
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
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_sig_block_names_t_init(
    int unit,
    diag_dnx_field_utils_sig_block_names_t * names_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(names_p, _SHR_E_PARAM, "names");
    sal_memset((void *) names_p, 0, sizeof(*names_p));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_sig_block_names_t_set(
    int unit,
    const char *block,
    const char *stage,
    const char *to,
    const char *to_tcam,
    const char *tcam,
    diag_dnx_field_utils_sig_block_names_t * names_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(names_p, _SHR_E_PARAM, "names");

    sal_strncpy_s(names_p->block, block, RHSTRING_MAX_SIZE);
    sal_strncpy_s(names_p->stage, stage, RHSTRING_MAX_SIZE);
    sal_strncpy_s(names_p->to, to, RHSTRING_MAX_SIZE);
    sal_strncpy_s(names_p->to_tcam, to_tcam, RHSTRING_MAX_SIZE);
    sal_strncpy_s(names_p->tcam, tcam, RHSTRING_MAX_SIZE);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_group_type_conversion(
    int unit,
    dnx_field_group_type_e fg_type,
    char converted_fg_type[DIAG_DNX_FIELD_UTILS_STR_SIZE])
{
    SHR_FUNC_INIT_VARS(unit);
    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        {
            sal_strncpy_s(converted_fg_type, "TCAM", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            sal_strncpy_s(converted_fg_type, "DIRECT_TABLE_TCAM", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            sal_strncpy_s(converted_fg_type, "DIRECT_TABLE_MDB", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            sal_strncpy_s(converted_fg_type, "EXEM", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
        {
            sal_strncpy_s(converted_fg_type, "STATE_TABLE", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
        {
            sal_strncpy_s(converted_fg_type, "DIRECT_EXTRACTION", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXTERNAL_TCAM:
        {
            sal_strncpy_s(converted_fg_type, "EXTERNAL_TCAM", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_CONST:
        {
            sal_strncpy_s(converted_fg_type, "CONST", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Field Group type (%s).", dnx_field_group_type_e_get_name(fg_type));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_key_size_conversion(
    int unit,
    dnx_field_key_length_type_e key_size,
    uint32 *key_size_in_bits)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (key_size)
    {
        case DNX_FIELD_KEY_LENGTH_TYPE_DT:
        {
            *key_size_in_bits = dnx_data_field.tcam.dt_max_key_size_get(unit);
            break;
        }
        case DNX_FIELD_KEY_LENGTH_TYPE_HALF:
        {
            *key_size_in_bits = dnx_data_field.tcam.key_size_half_get(unit);
            break;
        }
        case DNX_FIELD_KEY_LENGTH_TYPE_SINGLE:
        {
            *key_size_in_bits = dnx_data_field.tcam.key_size_single_get(unit);
            break;
        }
        case DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE:
        {
            *key_size_in_bits = dnx_data_field.tcam.key_size_double_get(unit);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error converting key size enum (%d) to TCAM key size", key_size);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_action_size_conversion(
    int unit,
    dnx_field_action_length_type_e action_size,
    uint32 *action_size_in_bits)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (action_size)
    {
        case DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_HALF:
        {
            *action_size_in_bits = dnx_data_field.tcam.action_size_half_get(unit);
            break;
        }
        case DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_SINGLE:
            *action_size_in_bits = dnx_data_field.tcam.action_size_single_get(unit);
            break;
        case DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_DOUBLE:
        {
            *action_size_in_bits = dnx_data_field.tcam.action_size_double_get(unit);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error converting action size enum (%d) to TCAM action size", action_size);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_cmp_hash_state_mode_conversion(
    int unit,
    int mode,
    int is_hash,
    int is_state,
    char converted_mode[DIAG_DNX_FIELD_UTILS_STR_SIZE])
{
    SHR_FUNC_INIT_VARS(unit);

    sal_strncpy_s(converted_mode, "", DIAG_DNX_FIELD_UTILS_STR_SIZE);

    if (mode == DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE && !is_hash && !is_state)
    {
        sal_strncpy_s(converted_mode, "S", DIAG_DNX_FIELD_UTILS_STR_SIZE);
    }
    else if (mode == DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE)
    {
        sal_strncpy_s(converted_mode, "D", DIAG_DNX_FIELD_UTILS_STR_SIZE);
    }
    else if ((mode == DNX_FIELD_CONTEXT_HASH_MODE_ENABLED) && is_hash)
    {
        sal_strncpy_s(converted_mode, "HASH.E", DIAG_DNX_FIELD_UTILS_STR_SIZE);
    }
    else if ((mode == DNX_FIELD_CONTEXT_STATE_TABLE_MODE_ENABLED) && is_state)
    {
        sal_strncpy_s(converted_mode, "ST.E", DIAG_DNX_FIELD_UTILS_STR_SIZE);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Mode.");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_context_modes_string_get(
    int unit,
    dnx_field_context_mode_t context_mode,
    char *mode_string_p)
{
    char mode_prefix_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    char converted_mode[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    char mode_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    int is_hash, is_state;

    SHR_FUNC_INIT_VARS(unit);

    is_hash = is_state = 0;
    sal_strncpy_s(mode_string, "", sizeof(mode_string));

    if (context_mode.context_ipmf1_mode.compare_mode_1 != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE)
    {
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_cmp_hash_state_mode_conversion(unit,
                                                                            context_mode.
                                                                            context_ipmf1_mode.compare_mode_1, is_hash,
                                                                            is_state, converted_mode));
        if (sal_strncmp(mode_string, "", sizeof(mode_string)))
        {
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(mode_string, "%s", ", ");
            sal_strncat_s(mode_string, ", ", sizeof(mode_string));
        }
        sal_strncpy_s(mode_prefix_string, "CMP1.", sizeof(mode_prefix_string));
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(mode_string, "%s", mode_prefix_string);
        sal_strncat_s(mode_string, mode_prefix_string, sizeof(mode_string));
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(mode_string, "%s", converted_mode);
        sal_strncat_s(mode_string, converted_mode, sizeof(mode_string));
    }

    if (context_mode.context_ipmf1_mode.compare_mode_2 != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE)
    {
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_cmp_hash_state_mode_conversion(unit,
                                                                            context_mode.
                                                                            context_ipmf1_mode.compare_mode_2, is_hash,
                                                                            is_state, converted_mode));
        if (sal_strncmp(mode_string, "", sizeof(mode_string)))
        {
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(mode_string, "%s", ", ");
            sal_strncat_s(mode_string, ", ", sizeof(mode_string));
        }
        sal_strncpy_s(mode_prefix_string, "CMP2.", sizeof(mode_prefix_string));
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(mode_string, "%s", mode_prefix_string);
        sal_strncat_s(mode_string, mode_prefix_string, sizeof(mode_string));
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(mode_string, "%s", converted_mode);
        sal_strncat_s(mode_string, converted_mode, sizeof(mode_string));
    }

    if (context_mode.context_ipmf1_mode.hash_mode != DNX_FIELD_CONTEXT_HASH_MODE_DISABLED)
    {
        is_hash = 1;
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_cmp_hash_state_mode_conversion(unit,
                                                                            context_mode.context_ipmf1_mode.hash_mode,
                                                                            is_hash, is_state, converted_mode));
        if (sal_strncmp(mode_string, "", sizeof(mode_string)))
        {
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(mode_string, "%s", ", ");
            sal_strncat_s(mode_string, ", ", sizeof(mode_string));
        }
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(mode_string, "%s", converted_mode);
        sal_strncat_s(mode_string, converted_mode, sizeof(mode_string));
    }

    if (context_mode.state_table_mode != DNX_FIELD_CONTEXT_STATE_TABLE_MODE_DISABLED)
    {
        is_state = 1;
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_cmp_hash_state_mode_conversion(unit,
                                                                            context_mode.state_table_mode,
                                                                            is_hash, is_state, converted_mode));
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(mode_string, "%s", converted_mode);
        sal_strncat_s(mode_string, converted_mode, sizeof(mode_string));
    }

    sal_strncpy_s(mode_string_p, mode_string, DIAG_DNX_FIELD_UTILS_STR_SIZE);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_cmp_hash_key_usage_info_get(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_context_mode_t mode,
    dnx_field_context_compare_info_t * compare_info_p,
    dnx_field_context_hashing_info_t * hashing_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_context_compare_info_t_init(unit, compare_info_p));
    SHR_IF_ERR_EXIT(dnx_field_context_hashing_info_t_init(unit, hashing_info_p));

    if (mode.context_ipmf1_mode.compare_mode_1 != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE ||
        mode.context_ipmf1_mode.compare_mode_2 != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE)
    {
        SHR_IF_ERR_EXIT(dnx_field_context_cmp_info_get(unit, context_id, compare_info_p));
    }

    if (mode.context_ipmf1_mode.hash_mode != DNX_FIELD_CONTEXT_HASH_MODE_DISABLED)
    {
        SHR_IF_ERR_EXIT(dnx_field_context_hashing_info_get(unit, context_id, hashing_info_p));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_nof_alloc_fems_per_fg_get(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    dnx_field_fem_id_t * nof_fems_p)
{
    dnx_field_fem_id_t dnx_fem_id, max_nof_fems, nof_alloc_fems;
    dnx_field_fem_program_t fem_program_index, fem_program_max;
    SHR_FUNC_INIT_VARS(unit);

    max_nof_fems = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
    fem_program_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs;

    nof_alloc_fems = 0;

    for (dnx_fem_id = 0; dnx_fem_id < max_nof_fems; dnx_fem_id++)
    {
        if (context_id != DNX_FIELD_CONTEXT_ID_INVALID)
        {
            /*
             * Check if the FEM is being used by a specific context_ID, to account for invalidated FEMs.
             */
            unsigned int using_fem;
            SHR_IF_ERR_EXIT(dnx_field_fem_is_context_using(unit, dnx_fem_id, context_id, fg_id, &using_fem));
            if (using_fem == FALSE)
            {
                continue;
            }
        }
        /*
         * If a match is found then display all four 'actions' corresponding to this FEM id
         */
        for (fem_program_index = 0; fem_program_index < fem_program_max; fem_program_index++)
        {
            dnx_field_group_t returned_fg_id;

            SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.fg_id.get(unit, dnx_fem_id, fem_program_index, &returned_fg_id));
            if (returned_fg_id == fg_id)
            {
                /*
                 * This 'fem_program' is occupied for specified 'fem_id'.
                 */
                nof_alloc_fems++;
                break;
            }
        }
    }

    /** Return the calculated value. */
    *nof_fems_p = nof_alloc_fems;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_nof_alloc_added_efes_per_fg_ctx_get(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    dnx_field_fes_id_t * nof_allocated_added_efes_p)
{
    dnx_field_fes_id_t added_efes_action_iter, nof_alloc_efes;
    bcm_field_efes_action_info_t efes_action_info;
    int max_num_efes_actions;
    dnx_field_group_context_full_info_t *attach_full_info = NULL;
    dnx_field_action_position_t fes_position;
    dnx_field_action_array_t fes_array;
    bcm_field_action_priority_t encoded_position;
    dnx_field_stage_e fg_stage;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(attach_full_info, sizeof(dnx_field_group_context_full_info_t), "attach_full_info", "%s%s%s\r\n", EMPTY,
              EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &fg_stage));
    max_num_efes_actions = dnx_data_field.group.nof_action_per_fg_get(unit);
    nof_alloc_efes = 0;

    SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &(attach_full_info->attach_basic_info)));
    SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fg_id, context_id, attach_full_info));

    for (added_efes_action_iter = 0; added_efes_action_iter < max_num_efes_actions &&
         attach_full_info->actions_fes_info.added_fes_quartets[added_efes_action_iter].fes_id !=
         DNX_FIELD_EFES_ID_INVALID; added_efes_action_iter++)
    {
        DNX_FIELD_ACTION_FES_ID_ARRAY_POSITION_GET(fes_array, fes_position, unit, fg_stage,
                                                   attach_full_info->
                                                   actions_fes_info.added_fes_quartets[added_efes_action_iter].fes_id);
        encoded_position = BCM_FIELD_ACTION_POSITION(fes_array, fes_position);
        SHR_IF_ERR_EXIT(bcm_field_efes_action_info_get(unit, fg_id, context_id, encoded_position, &efes_action_info));

        nof_alloc_efes++;
    }

    /** Return the calculated value. */
    *nof_allocated_added_efes_p = nof_alloc_efes;

exit:
    SHR_FREE(attach_full_info);
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_last_key_table_info_get(
    int unit,
    dbal_tables_e table_id,
    bcm_core_t core_id,
    dbal_enum_value_field_field_key_e key_id,
    uint32 *field_key_value_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

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
                    (unit, entry_handle_id, DBAL_FIELD_LAST_KEY, INST_SINGLE, field_key_value_p));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_exem_entry_key_info_get(
    int unit,
    dnx_field_group_full_info_t * fg_info_p,
    uint32 *key_values_p,
    int template_offset_on_key,
    dnx_field_entry_key_t * entry_key_p)
{
    unsigned int qual_index;

    SHR_FUNC_INIT_VARS(unit);

    for (qual_index = 0; qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
         fg_info_p->group_full_info.key_template.key_qual_map[qual_index].qual_type != DNX_FIELD_QUAL_TYPE_INVALID;
         qual_index++)
    {
        entry_key_p->qual_info[qual_index].dnx_qual =
            fg_info_p->group_full_info.key_template.key_qual_map[qual_index].qual_type;

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(key_values_p,
                                                       fg_info_p->group_full_info.key_template.
                                                       key_qual_map[qual_index].lsb + template_offset_on_key,
                                                       fg_info_p->group_full_info.key_template.
                                                       key_qual_map[qual_index].size,
                                                       entry_key_p->qual_info[qual_index].qual_value));
    }

    if (qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
    {
        entry_key_p->qual_info[qual_index].dnx_qual = DNX_FIELD_QUAL_TYPE_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_dt_available_entries_get(
    int unit,
    dnx_field_group_t fg_id,
    bcm_core_t core_id,
    uint32 *num_available_entries_p)
{
    uint32 entry_iter, num_available_entries;
    uint32 max_nof_dt_entries;
    uint32 entry_handle;
    dnx_field_group_type_e fg_type;
    uint8 entry_found;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));

    if (fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal fg_type (%d) for this function.\n", fg_type);
    }

    num_available_entries = 0;

    /** Set max number of DT entries, to iterate on, depends on the max DT key size. */
    max_nof_dt_entries = utilex_power_of_2(dnx_data_field.tcam.dt_max_key_size_get(unit));

    /** Iterate over max number of DT entries (2k). */
    for (entry_iter = 0; entry_iter < max_nof_dt_entries; entry_iter++)
    {
        entry_handle = DNX_FIELD_ENTRY_TCAM_DT(entry_iter, fg_id);

        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_id_exists(unit, core_id, entry_handle, &entry_found));
        /** In case the current entry is not found, then increase the output counter. */
        if (!entry_found)
        {
            num_available_entries++;
        }
    }

    *num_available_entries_p = num_available_entries;

exit:
    SHR_FUNC_EXIT;
}

#if defined(INCLUDE_KBP)
/* { */
/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_payload_ids_per_fg_get(
    int unit,
    dnx_field_context_t context_id,
    uint32 payload_ids[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS])
{
    int fg_id_index;
    int nof_fgs_per_ctx;
    dnx_field_group_t fgs_per_context[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS];
    dnx_field_attach_context_info_t context_info;

    SHR_FUNC_INIT_VARS(unit);

    for (fg_id_index = 0; fg_id_index < DNX_DATA_MAX_FIELD_GROUP_NOF_FGS; fg_id_index++)
    {
        /** Initialize the Payload IDs to default values. */
        payload_ids[fg_id_index] = BCM_FIELD_INVALID;
    }

    SHR_IF_ERR_EXIT(dnx_field_group_context_fgs_get
                    (unit, DNX_FIELD_STAGE_EXTERNAL, context_id, fgs_per_context, &nof_fgs_per_ctx));

    for (fg_id_index = 0; fg_id_index < nof_fgs_per_ctx; fg_id_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_context_info_get
                        (unit, fgs_per_context[fg_id_index], context_id, &context_info));
        payload_ids[fgs_per_context[fg_id_index]] = context_info.payload_id;
    }

exit:
    SHR_FUNC_EXIT;
}
/* } */
#endif
#if defined(INCLUDE_KBP)
/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_kbp_segment_share_string_update(
    int unit,
    dnx_field_group_t fg_id,
    uint32 opcode_id,
    int seg_index,
    kbp_mngr_key_segment_t ms_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY],
    uint8 nof_fwd_segments,
    uint32 fg_payload_ids[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS],
    dnx_field_group_attach_info_t * attach_basic_info_p,
    int qual_iter,
    char segment_sharing_string[DIAG_DNX_FIELD_UTILS_STR_SIZE])
{
    int result_index;
    int is_fwd_set_in_kbp = FALSE;
#if defined(INCLUDE_KBP)
    int is_shared_with_fwd = FALSE;
#endif
    int acl_result_reached = FALSE;
    uint32 result_ids[DNX_KBP_MAX_NOF_RESULTS];
    dnx_field_kbp_segment_info_t seg_info;
    char segment_sharing_string_local[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (seg_index == DNX_KBP_INVALID_SEGMENT_ID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Segment invalid.\r\n");
    }
    if (seg_index >= DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Segment %d, maximum is %d.\r\n",
                     seg_index, DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY - 1);
    }

    SHR_IF_ERR_EXIT(kbp_mngr_segment_used_result_indexes_get
                    (unit, opcode_id, ms_key_segments[seg_index].name, result_ids));
    SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.master_key_info.
                    segment_info.get(unit, opcode_id, seg_index, &seg_info));

    /*
     * First print if the segment is a FWD segment
     */
    if (seg_index < nof_fwd_segments)
    {
        if (ms_key_segments[seg_index].is_overlay_field)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Segment %d, opcode id %d FWD and overlay.\r\n", seg_index, opcode_id);
        }
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(segment_sharing_string_local, "%s", "FWD");
        sal_strncat_s(segment_sharing_string_local, "FWD", sizeof(segment_sharing_string_local));
#if defined(INCLUDE_KBP)
        is_shared_with_fwd = TRUE;
#endif
        if (seg_info.fg_id != DNX_FIELD_GROUP_INVALID)
        {
            /** It is a zero padding segment.*/
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(segment_sharing_string_local, "%s", "-ZP");
            sal_strncat_s(segment_sharing_string_local, "-ZP", sizeof(segment_sharing_string_local));
        }
    }

    /** Iterate over all result indexes, used by FWD and PMF. */
    for (result_index = 0; result_index < DNX_KBP_MAX_NOF_RESULTS &&
         result_ids[result_index] != DNX_KBP_INVALID_RESULT_ID; result_index++)
    {
        int payload_fg_index;
        int is_result_id_found = FALSE;
        /** Iterate over payload IDs used by PMF field groups. */
        acl_result_reached = FALSE;
        for (payload_fg_index = 0; payload_fg_index < DNX_DATA_MAX_FIELD_GROUP_NOF_FGS; payload_fg_index++)
        {
            /**
             * Check if the current result ID is match one of used by PMF FGs.
             * In case yes, then it means that we will present the FG ID, which is using it.
             * Otherwise it means that current result ID is used by FWD stage.
             */
            if (result_ids[result_index] == fg_payload_ids[payload_fg_index])
            {
                is_result_id_found = TRUE;
                break;
            }
        }

        if (is_result_id_found)
        {
            char shared_fg_id_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };

            acl_result_reached = TRUE;
            /** If this is not the first thing written, add ", ".*/
            if (segment_sharing_string_local[0] != '\0')
            {
                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(segment_sharing_string_local, "%s", ", ");
                sal_strncat_s(segment_sharing_string_local, ", ", sizeof(segment_sharing_string_local));
            }
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%d", payload_fg_index);
            sal_snprintf(shared_fg_id_buff, sizeof(shared_fg_id_buff), "%d", payload_fg_index);
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(segment_sharing_string_local, "%s", shared_fg_id_buff);
            sal_strncat_s(segment_sharing_string_local, shared_fg_id_buff, sizeof(segment_sharing_string_local));
        }
        else
        {
            /*
             * In case we have a sharing with FWD stage, will still present 'FWD' only once.
             * We add "-KBP" if it is being used by KBP.
             */
            if (!is_fwd_set_in_kbp)
            {
                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(segment_sharing_string_local, "%s", "-KBP");
                sal_strncat_s(segment_sharing_string_local, "-KBP", sizeof(segment_sharing_string_local));
                is_fwd_set_in_kbp = TRUE;
            }
        }
    }

    if (acl_result_reached == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Qualifier %d of fg_id %d opcode %d never found in KBP.\r\n",
                     qual_iter, fg_id, opcode_id);
    }

    /*
     * Add indication for overlay
     */
    if (ms_key_segments[seg_index].is_overlay_field)
    {
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(segment_sharing_string_local, "%s", " (overlay)");
        sal_strncat_s(segment_sharing_string_local, " (overlay)", sizeof(segment_sharing_string_local));
    }
    /*
     * Sanity check done only when compiling KBP to silence coverity.
     */
#if defined(INCLUDE_KBP)
    if (is_fwd_set_in_kbp && (is_shared_with_fwd == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Qualifier %d of fg_id %d opcode %d used by FWD lookup "
                     "but is not a FWD segment.\r\n", qual_iter, fg_id, opcode_id);
    }
#endif

    sal_strncpy_s(segment_sharing_string, segment_sharing_string_local,
                  sizeof(segment_sharing_string[0]) * DIAG_DNX_FIELD_UTILS_STR_SIZE);

exit:
    SHR_FUNC_EXIT;
}
#endif

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_send_itmh_packet(
    int unit)
{
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags = 0;
    rhhandle_t packet_tx = NULL;
    uint32 dst = 0xc0000 | 201;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_port_get(unit, 200, &flags, &interface_info, &mapping_info));
    /** Allocate and init packet_tx. */
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_tx));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_tx, "PTCH_2"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_tx, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_tx, "PTCH_2.Parser_Program_Control", "0"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_tx, "PTCH_2.Opaque_PT_Attributes", "0"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_tx, "PTCH_2.Reserved", "0"));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_tx, "ITMH_Base"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_tx, "ITMH_Base.Dst", &dst, 32));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_tx, "ITMH_Base.pph_type", "0"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_tx, "ITMH_Base.Injected_AS_Extension_Present", "1"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_tx, "ITMH_Base.Extension_Exists", "1"));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_tx, "ITMH_Out_LIF_Ext"));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_tx, "ETH1"));

    /** Send packet. */
    SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, 200, packet_tx, SAND_PACKET_RESUME));

exit:
    diag_sand_packet_free(unit, packet_tx);
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_efes_key_select_text_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_fes_key_select_t key_select,
    char *key_select_name)
{
    dbal_fields_e key_select_dbal_field_id;

    SHR_FUNC_INIT_VARS(unit);

    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
        {
            key_select_dbal_field_id = DBAL_FIELD_FIELD_PMF_A_FES_KEY_SELECT;
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            key_select_dbal_field_id = DBAL_FIELD_FIELD_PMF_B_FES_KEY_SELECT;
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            key_select_dbal_field_id = DBAL_FIELD_FIELD_E_PMF_FES_KEY_SELECT;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Given field stage (%s) is not supported!\r\n", dnx_field_stage_text(unit, field_stage));
            break;
        }
    }

    sal_strncpy_s(key_select_name, dbal_enum_id_to_string(unit, key_select_dbal_field_id, key_select),
                  DIAG_DNX_FIELD_UTILS_STR_SIZE);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_stage_str_to_enum_val_conversion(
    int unit,
    char *field_stage_str_p,
    dnx_field_stage_e * field_stage_enum_val_p)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!sal_strncmp(field_stage_str_p, "IPMF1", (SH_SAND_MAX_TOKEN_SIZE - 1)))
    {
        *field_stage_enum_val_p = DNX_FIELD_STAGE_IPMF1;
    }
    else if (!sal_strncmp(field_stage_str_p, "IPMF2", (SH_SAND_MAX_TOKEN_SIZE - 1)))
    {
        *field_stage_enum_val_p = DNX_FIELD_STAGE_IPMF2;
    }
    else if (!sal_strncmp(field_stage_str_p, "IPMF3", (SH_SAND_MAX_TOKEN_SIZE - 1)))
    {
        *field_stage_enum_val_p = DNX_FIELD_STAGE_IPMF3;
    }
    else if (!sal_strncmp(field_stage_str_p, "EPMF", (SH_SAND_MAX_TOKEN_SIZE - 1)))
    {
        *field_stage_enum_val_p = DNX_FIELD_STAGE_EPMF;
    }
    else if (!sal_strncmp(field_stage_str_p, "ACE", (SH_SAND_MAX_TOKEN_SIZE - 1)))
    {
        *field_stage_enum_val_p = DNX_FIELD_STAGE_ACE;
    }
    else if (!sal_strncmp(field_stage_str_p, "EXTERNAL", (SH_SAND_MAX_TOKEN_SIZE - 1)))
    {
        *field_stage_enum_val_p = DNX_FIELD_STAGE_EXTERNAL;
    }
    else if (!sal_strncmp(field_stage_str_p, "L4_OPS", (SH_SAND_MAX_TOKEN_SIZE - 1)))
    {
        *field_stage_enum_val_p = DNX_FIELD_STAGE_L4_OPS;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Field Stage string (%s).", field_stage_str_p);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_range_type_str_to_enum_val_conversion(
    int unit,
    char *field_range_type_str_p,
    dnx_field_range_type_e * field_range_type_enum_val_p)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!sal_strncmp(field_range_type_str_p, "L4_SRC", (SH_SAND_MAX_TOKEN_SIZE - 1)))
    {
        *field_range_type_enum_val_p = DNX_FIELD_RANGE_TYPE_L4_SRC_PORT;
    }
    else if (!sal_strncmp(field_range_type_str_p, "L4_DST", (SH_SAND_MAX_TOKEN_SIZE - 1)))
    {
        *field_range_type_enum_val_p = DNX_FIELD_RANGE_TYPE_L4_DST_PORT;
    }
    else if (!sal_strncmp(field_range_type_str_p, "OUT_LIF", (SH_SAND_MAX_TOKEN_SIZE - 1)))
    {
        *field_range_type_enum_val_p = DNX_FIELD_RANGE_TYPE_OUT_LIF;
    }
    else if (!sal_strncmp(field_range_type_str_p, "PACKET_HEADER_SIZE", (SH_SAND_MAX_TOKEN_SIZE - 1)))
    {
        *field_range_type_enum_val_p = DNX_FIELD_RANGE_TYPE_PKT_HDR_SIZE;
    }
    else if (!sal_strncmp(field_range_type_str_p, "L4OPS_PKT_HDR_SIZE", (SH_SAND_MAX_TOKEN_SIZE - 1)))
    {
        *field_range_type_enum_val_p = DNX_FIELD_RANGE_TYPE_L4OPS_PKT_HDR_SIZE;
    }
    else if (!sal_strncmp(field_range_type_str_p, "IN_TTL", (SH_SAND_MAX_TOKEN_SIZE - 1)))
    {
        *field_range_type_enum_val_p = DNX_FIELD_RANGE_TYPE_IN_TTL;
    }
    else if (!sal_strncmp(field_range_type_str_p, "FFC1_LOW", (SH_SAND_MAX_TOKEN_SIZE - 1)))
    {
        *field_range_type_enum_val_p = DNX_FIELD_RANGE_TYPE_FFC1_LOW;
    }
    else if (!sal_strncmp(field_range_type_str_p, "FFC1_HIGH", (SH_SAND_MAX_TOKEN_SIZE - 1)))
    {
        *field_range_type_enum_val_p = DNX_FIELD_RANGE_TYPE_FFC1_HIGH;
    }
    else if (!sal_strncmp(field_range_type_str_p, "FFC2_LOW", (SH_SAND_MAX_TOKEN_SIZE - 1)))
    {
        *field_range_type_enum_val_p = DNX_FIELD_RANGE_TYPE_FFC2_LOW;
    }
    else if (!sal_strncmp(field_range_type_str_p, "FFC2_HIGH", (SH_SAND_MAX_TOKEN_SIZE - 1)))
    {
        *field_range_type_enum_val_p = DNX_FIELD_RANGE_TYPE_FFC2_HIGH;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Field Range Type string (%s).", field_range_type_str_p);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
