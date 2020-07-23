/** \file diag_dnx_field_utils.c
 *
 * Diagnostics utilities to be used for DNX diagnostics of Field Module.
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
#include <appl/diag/sand/diag_sand_packet.h>
#include <bcm_int/dnx/field/field_context.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_context_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_qualifier_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_group_access.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/field/tcam/tcam_handler.h>
#include <soc/sand/sand_signals.h>
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
    char block[RHSTRING_MAX_SIZE],
    char stage[RHSTRING_MAX_SIZE],
    char to[RHSTRING_MAX_SIZE],
    char to_tcam[RHSTRING_MAX_SIZE],
    char tcam[RHSTRING_MAX_SIZE],
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
    char *converted_fg_type_p)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        {
            sal_strncpy(converted_fg_type_p, "TCAM", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            sal_strncpy(converted_fg_type_p, "DIRECT_TABLE_TCAM", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            sal_strncpy(converted_fg_type_p, "DIRECT_TABLE_MDB", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            sal_strncpy(converted_fg_type_p, "EXEM", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
        {
            sal_strncpy(converted_fg_type_p, "STATE_TABLE", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
        {
            sal_strncpy(converted_fg_type_p, "DIRECT_EXTRACTION", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_KBP:
        {
            sal_strncpy(converted_fg_type_p, "EXTERNAL_TCAM", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_CONST:
        {
            sal_strncpy(converted_fg_type_p, "CONST", DIAG_DNX_FIELD_UTILS_STR_SIZE);
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
    char *converted_mode_p,
    int is_hash,
    int is_state)
{
    SHR_FUNC_INIT_VARS(unit);

    sal_strncpy(converted_mode_p, "", DIAG_DNX_FIELD_UTILS_STR_SIZE);

    if (mode == DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE && !is_hash && !is_state)
    {
        sal_strncpy(converted_mode_p, "S, ", DIAG_DNX_FIELD_UTILS_STR_SIZE);
    }
    else if (mode == DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE)
    {
        sal_strncpy(converted_mode_p, "D, ", DIAG_DNX_FIELD_UTILS_STR_SIZE);
    }
    else if ((mode == DNX_FIELD_CONTEXT_HASH_MODE_ENABLED) && is_hash)
    {
        sal_strncpy(converted_mode_p, "HASH.E, ", DIAG_DNX_FIELD_UTILS_STR_SIZE);
    }
    else if ((mode == DNX_FIELD_CONTEXT_STATE_TABLE_MODE_ENABLED) && is_state)
    {
        sal_strncpy(converted_mode_p, "ST.E, ", DIAG_DNX_FIELD_UTILS_STR_SIZE);
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
    sal_strncpy(mode_string, "", DIAG_DNX_FIELD_UTILS_STR_SIZE - 1);

    if (context_mode.context_ipmf1_mode.compare_mode_1 != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE)
    {
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_cmp_hash_state_mode_conversion(unit,
                                                                            context_mode.
                                                                            context_ipmf1_mode.compare_mode_1,
                                                                            converted_mode, is_hash, is_state));
        sal_strncpy(mode_prefix_string, "CMP1.", sizeof(mode_prefix_string) - 1);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(mode_string, "%s", mode_prefix_string);
        sal_strncat_s(mode_string, mode_prefix_string, sizeof(mode_string));
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(mode_string, "%s", converted_mode);
        sal_strncat_s(mode_string, converted_mode, sizeof(mode_string));
    }

    if (context_mode.context_ipmf1_mode.compare_mode_2 != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE)
    {
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_cmp_hash_state_mode_conversion(unit,
                                                                            context_mode.
                                                                            context_ipmf1_mode.compare_mode_2,
                                                                            converted_mode, is_hash, is_state));
        sal_strncpy(mode_prefix_string, "CMP2.", sizeof(mode_prefix_string) - 1);
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
                                                                            converted_mode, is_hash, is_state));
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(mode_string, "%s", converted_mode);
        sal_strncat_s(mode_string, converted_mode, sizeof(mode_string));
    }

    if (context_mode.state_table_mode != DNX_FIELD_CONTEXT_STATE_TABLE_MODE_DISABLED)
    {
        is_state = 1;
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_cmp_hash_state_mode_conversion(unit,
                                                                            context_mode.state_table_mode,
                                                                            converted_mode, is_hash, is_state));
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(mode_string, "%s", converted_mode);
        sal_strncat_s(mode_string, converted_mode, sizeof(mode_string));
    }

    sal_strncpy(mode_string_p, mode_string, DIAG_DNX_FIELD_UTILS_STR_SIZE - 1);

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
 * See header on diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_ffc_groups_names_get(
    int unit,
    dnx_field_stage_e field_stage,
    unsigned int ffc_group_num,
    char ffc_group_name[DIAG_DNX_FIELD_UTILS_STR_SIZE])
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ffc_group_name, _SHR_E_PARAM, "ffc_group_name");

    if (ffc_group_num >= dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_ffc_groups)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "There are only %d FFC groups in stage %s (%d), requested group %d.\r\n",
                     dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_ffc_groups,
                     dnx_field_stage_text(unit, field_stage), field_stage, ffc_group_num);
    }

    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        {
            switch (ffc_group_num)
            {
                case 0:
                {
                    sal_strncpy_s(ffc_group_name, "MetaData/LayerReord/Header", DIAG_DNX_FIELD_UTILS_STR_SIZE);
                    break;
                }
                case 1:
                {
                    sal_strncpy_s(ffc_group_name, "MetaData/LayerReord", DIAG_DNX_FIELD_UTILS_STR_SIZE);
                    break;
                }
                case 2:
                {
                    sal_strncpy_s(ffc_group_name, "MetaData/Header", DIAG_DNX_FIELD_UTILS_STR_SIZE);
                    break;
                }
                case 3:
                {
                    sal_strncpy_s(ffc_group_name, "MetaData/LayerReord/Header", DIAG_DNX_FIELD_UTILS_STR_SIZE);
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Stage iPMF1 has more than 4 FFC field groups.\r\n");
                    break;
                }
            }
            break;
        }
        case DNX_FIELD_STAGE_IPMF2:
        {
            /*
             * All stages other than iPMF1 have only one FFC group.
             */
            if (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_ffc_groups != 1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Stage %s (%d) is not iPMF1, but has more than one field group type (%d).\r\n",
                             dnx_field_stage_text(unit, field_stage), field_stage,
                             dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_ffc_groups);
            }
            sal_strncpy_s(ffc_group_name, "MetaData", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            /*
             * All stages other than iPMF1 have only one FFC group.
             */
            if (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_ffc_groups != 1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Stage %s (%d) is not iPMF1, but has more than one field group type (%d).\r\n",
                             dnx_field_stage_text(unit, field_stage), field_stage,
                             dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_ffc_groups);
            }
            sal_strncpy_s(ffc_group_name, "MetaData/LayerReord", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        case DNX_FIELD_STAGE_EXTERNAL:
        {
            /*
             * All stages other than iPMF1 have only one FFC group.
             */
            if (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_ffc_groups != 1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Stage %s (%d) is not iPMF1, but has more than one field group type (%d).\r\n",
                             dnx_field_stage_text(unit, field_stage), field_stage,
                             dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_ffc_groups);
            }
            sal_strncpy_s(ffc_group_name, "MetaData/LayerReord/Header", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        default:
        {
            bcm_field_stage_t bcm_stage;
            /** Convert DNX to BCM Field Stage */
            SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage));
            LOG_CLI_EX("\r\n" "Given field stage (%s) is not supported!!%s%s%s\r\n\n",
                       dnx_field_bcm_stage_text(bcm_stage), EMPTY, EMPTY, EMPTY);
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
                                                       key_qual_map[qual_index].lsb,
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
        LOG_CLI_EX("\r\n" "Only for DirectTable Field groups are supported!! %s%s%s%s\r\n\n",
                   EMPTY, EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }

    num_available_entries = 0;

    /** Set max number of DT entries, to iterate on, depends on the max DT key size. */
    max_nof_dt_entries = utilex_power_of_2(dnx_data_field.tcam.dt_max_key_size_get(unit));

    /** Iterate over max number of DT entries (2k). */
    for (entry_iter = 0; entry_iter < max_nof_dt_entries; entry_iter++)
    {
        entry_handle = DNX_FIELD_TCAM_ENTRY(entry_iter, core_id);

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

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_qual_signal_value_raw_data_get(
    int unit,
    bcm_core_t core_id,
    char *block_p,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t dnx_qual,
    uint32 out_qual_sig_value[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY])
{
    dnx_field_map_qual_signal_info_t qual_signals[DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_QUALIFIER];
    uint32 signal_iter;
    int out_buff_offset;
    signal_output_t *signal_output = NULL;

    SHR_FUNC_INIT_VARS(unit);

    out_buff_offset = 0;

    /** Get signals related to the current qualifier. */
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_signals(unit, field_stage, dnx_qual, qual_signals));

    /** Iterate over number of signals for current qualifier. */
    for (signal_iter = 0; signal_iter < DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_QUALIFIER; signal_iter++)
    {
        /** Break in case we reach an empty string, which is indication the end of signals array. */
        if (qual_signals[signal_iter].name == 0)
        {
            break;
        }

        SHR_IF_ERR_EXIT(sand_signal_output_find(unit, core_id, 0,
                                                block_p, qual_signals[signal_iter].from, qual_signals[signal_iter].to,
                                                qual_signals[signal_iter].name, &signal_output));

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(signal_output->value, out_buff_offset,
                                                       signal_output->size, out_qual_sig_value));

        out_buff_offset += signal_output->size;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_action_signal_value_raw_data_get(
    int unit,
    bcm_core_t core_id,
    char *block_p,
    char *from_p,
    char *to_p,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t dnx_action,
    uint32 *out_action_sig_value_p)
{
    char *action_signals[DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_ACTION];
    uint32 signal_iter;
    int out_buff_offset;
    signal_output_t *signal_output = NULL;

    SHR_FUNC_INIT_VARS(unit);

    out_buff_offset = 0;

    /** Get signals related to the current action. */
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_signals(unit, field_stage, dnx_action, action_signals));

    /** Iterate over number of signals for current qualifier. */
    for (signal_iter = 0; signal_iter < DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_ACTION; signal_iter++)
    {
        /** Break in case we reach an empty string, which is indication the end of signals array. */
        if (action_signals[signal_iter] == 0)
        {
            break;
        }

        SHR_IF_ERR_EXIT(sand_signal_output_find(unit, core_id, 0, block_p, from_p, to_p,
                                                action_signals[signal_iter], &signal_output));

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(signal_output->value, out_buff_offset,
                                                       signal_output->size, out_action_sig_value_p));

        out_buff_offset += signal_output->size;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_user_qual_metadata_value_get(
    int unit,
    bcm_core_t core_id,
    char *block_p,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t dnx_qual,
    int qual_attach_offset,
    char out_qual_value[DIAG_DNX_FIELD_UTILS_STR_SIZE])
{
    dnx_field_user_qual_info_t user_qual_info;
    int qual_offset;
    int next_qual_offset;
    int qual_index;
    const dnx_field_qual_map_t *meta_qual_map;
    uint32 user_qual_size;
    uint32 qual_size;
    uint8 qual_found;
    dnx_field_qual_t pbus_dnx_qual;
    int buff_offset, buff_size;
    int val_iter;
    int max_val_iter;
    int max_num_bits_in_ffc;
    uint32 out_qual_value_buff[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY] = { 0 };
    uint32 out_qual_value_buff_offset;
    shr_error_e rv;

    
    bsl_severity_t original_severity_fld_proc = BSL_INFO;

    SHR_FUNC_INIT_VARS(unit);

    sal_strncpy(out_qual_value, "0x", DIAG_DNX_FIELD_UTILS_STR_SIZE - 1);
    buff_offset = buff_size = 0;
    qual_offset = 0;
    next_qual_offset = qual_attach_offset;
    out_qual_value_buff_offset = 0;

    /** Get the max num of bits in FFC from DNX data. */
    max_num_bits_in_ffc = dnx_data_field.common_max_val.nof_bits_in_ffc_get(unit);

    /** Get info for the given user qualifier. */
    SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.info.get(unit, DNX_QUAL_ID(dnx_qual), &user_qual_info));
    user_qual_size = user_qual_info.size;

    /** Flag to indicate if we found a qualifier with matching offset with the given one qual_attach_offset. */
    qual_found = FALSE;

    /** Iterate over all BCM qualifiers, until reach max number bcmFieldQualifyCount, or size of the give user qualifier is equal to 0. */
    for (qual_index = 0; qual_index < bcmFieldQualifyCount && user_qual_size; qual_index++)
    {
        /** Get the qualifier map info for the given stage and current bcm qualifier. */
        meta_qual_map = &dnx_field_map_stage_info[field_stage].meta_qual_map[qual_index];

        /** Check if this bcm qual is mapped on this stage, if not continue. */
        if (meta_qual_map->dnx_qual == 0)
        {
            continue;
        }

        if (field_stage == DNX_FIELD_STAGE_IPMF2)
        {
            pbus_dnx_qual = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META2, field_stage, meta_qual_map->dnx_qual);
        }
        else
        {
            pbus_dnx_qual = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, field_stage, meta_qual_map->dnx_qual);
        }

        
        SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
        /** Get current qualifier size and offset. */
        
        rv = dnx_field_map_dnx_qual_size(unit, field_stage, pbus_dnx_qual, &qual_size);
        
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
        if (rv == _SHR_E_PARAM)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(rv);

        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset(unit, field_stage, pbus_dnx_qual, &qual_offset));

        /**
         * Check if current qualifier is in the correct offset of PBUS.
         * For example:
         *      We have as input parameter the qualifier offset from the attach info
         *      to be (next_qual_offset = qual_attach_offset = 1400). Current qualifier offset is
         *      (qual_offset = 1300) and (qual_size = 200). We should check in which offset
         *      user qualifier resides and its value should be taken. In other hand we check
         *      that sum of the current qualifier offset and size is bigger than next_qual_offset.
         *      (1300 <= 1400) && ((1300 + 200) > 1400) --> TRUE && (1500 > 1400) --> TRUE && TRUE = TRUE
         *      Let say that user_qual_size is 100b. It means that our user qualifier is taking its value:
         *
         *                      1400 (next_qaul_offset = qual_attach_offset)
         *                       |
         *      _________________v________________
         *     |    Qualifier 1                   |
         *     |__________________________________|
         *   1300 (qual_offset)                  1500 (qual_offset + qual_size)
         *
         *   User qualifier value will be the second 100b if the current qualifier.
         *   This is if we have user qualifier, which is getting its value from just one qualifier from PBUS.
         *   In case our user qualifier is 200b, we already have first 100b and the second 100 will be taken from
         *   the next qualifier in the PBUS. Here we should calculate the starting offset of the next qualifier
         *   next_qual_offset = qual_offset + qual_size --> 1300 + 200 = 1500:
         *   Now we have 2 qualifiers (Q1 and Q2) with following parameters:
         *   Q1 --> Q1_qual_offset = 1300 | Q1_qual_size = 200
         *   Q2 --> Q2_qual_offset = 1500 | Q2_qual_size = 300
         *
         *    (next_qual_offset = qual_attach_offset)      (qual_attach_offset + user qualifier size)
         *                     1400                               1600
         *                       |                                 |
         *      _________________v________________ ________________v__________________
         *     |    Qualifier 1                   |    Qualifier 2                   |
         *     |__________________________________|__________________________________|
         *   1300 (Q1_qual_offset)               1500                               1800 (Q2_qual_offset + Q2_qual_size)
         *                      (new calculated Q2_next_qaul_offset)
         *
         *  First 100b of the user qualifier value will be taken from last 100b of the Q1 and
         *  Second 100b of the user qualifier value will be taken from the first 100b of the Q1.
         *  Same logic continue in case we have more qualifier from which we should take the data for user qualiifer.
         */
        if ((qual_offset <= next_qual_offset) && ((qual_offset + qual_size) > next_qual_offset))
        {
            /** Calculate the offset inside the qual_signal_val buffer, from where we will start getting the user qual value. */
            buff_offset = next_qual_offset - qual_offset;
            /** Calculate the size inside the qual_signal_val buffer, how many bits we will take from the current qualifier. */
            if ((qual_size - buff_offset) <= user_qual_size)
            {
                buff_size = qual_size - buff_offset;
            }
            else
            {
                buff_size = user_qual_size;
            }
            /**
             * Extract buffer size from user qualifier size, to see if any bits left from the user_qual_size.
             * If yes we should take them from the next qualifier in the PBUS.
             */
            user_qual_size = user_qual_size - buff_size;
            /** Calculate the next qualifier offset in the PBUS. */
            next_qual_offset = qual_offset + qual_size;
            /**
             * Indicates that we found correct qualifier from which we will extract the data for user qualifier value.
             * The flag will be used to get signal values for the current qualifier.
             */
            qual_found = TRUE;
            /**
             * Set the qual_index to 0, because we take the offset of the current qualifier (qual_offset) from the DNX data.
             * It can be that the qualifiers there are not ordered by offset. Every time we start from the first qualifier and
             * we are searching for the calculated new_qual_offset.
             */
            qual_index = 0;
        }

        /** In case qual_found is TRUE, we can retrieve signal data for the current qualifier. */
        if (qual_found)
        {
            uint32 qual_signal_val[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY] = { 0 };
            uint32 curr_qual_val[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY] = { 0 };
            /** Get the current qualifier signal value as raw data. */
            SHR_IF_ERR_EXIT(diag_dnx_field_utils_qual_signal_value_raw_data_get
                            (unit, core_id, block_p, field_stage, pbus_dnx_qual, qual_signal_val));

            /**
             * Get the exact value from the returned qual_signal_val buffer.
             * For this purpose we are using already calculated buff_offset and buff_size.
             */
            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(qual_signal_val, buff_offset, buff_size, curr_qual_val));

            /** Stick together all the values, of the found qualifiers, in the out qual value buffer. Will be converted to string at the end. */
            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                            (curr_qual_val, out_qual_value_buff_offset, buff_size, out_qual_value_buff));

            /**
             * On every new found qualifier increase the out_qual_value_buff_offset, to know where we should place the next one.
             * At the end out_qual_value_buff_offset should be equal to user_qual_info.size.
             */
            out_qual_value_buff_offset += buff_size;

            /** Reset the qual_found flag. */
            qual_found = FALSE;
        }
    }

    if (out_qual_value_buff_offset != user_qual_info.size)
    {
        LOG_CLI_EX("\r\n"
                   "The calculated out_qual_value_buff_offset (%d) is not the same as given user qualifier size (%d)!! %s%s\r\n\n",
                   out_qual_value_buff_offset, user_qual_info.size, EMPTY, EMPTY);
        SHR_EXIT();
    }

    /** Set the right, max value to iterate on, depends on current qual_size and max number of bits in FFC. */
    max_val_iter = (user_qual_info.size / max_num_bits_in_ffc);
    if ((max_val_iter % max_num_bits_in_ffc) != 0)
    {
        max_val_iter += 1;
    }
    else
    {
        max_val_iter = DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY - 4;
    }

    for (val_iter = max_val_iter - 1; val_iter >= 0 && out_qual_value_buff_offset; val_iter--)
    {
        char val_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
        int print_size = 0;
        /**
         * Array of strings, which are presenting the format in which the string will be written to val_buff.
         * Depends on current buffer size. Indicated how many nibbles we will present in the output sting.
         * For example:
         *      If we have print_size == 9b, we will need 3 nibbles to present the value 0x123
         *          print_size == 6b ---> number of nibbles == 2
         *          print_size == 32b ---> number of nibbles == 8
         *          print_size == 16b ---> number of nibbles == 4
         *          print_size == 23b ---> number of nibbles == 6
         *
         *      Calculation of number of nibbles per given buffer size is done using
         *      ((print_size + 3) / 4)
         *
         *      The result from the above macro is also used as array index in string_format[((print_size + 3) / 4) - 1].
         */
        char string_format[SAL_UINT8_NOF_BITS][5] = { "%01X", "%02X", "%03X", "%04X", "%05X", "%06X", "%07X", "%08X" };
        if ((out_qual_value_buff_offset % SAL_UINT32_NOF_BITS) != 0)
        {
            print_size = (out_qual_value_buff_offset % SAL_UINT32_NOF_BITS);
        }
        else
        {
            print_size = SAL_UINT32_NOF_BITS;
        }
        sal_snprintf(val_buff, sizeof(val_buff), string_format[((print_size + 3) / 4) - 1],
                     out_qual_value_buff[val_iter]);
        sal_strncat_s(out_qual_value, val_buff, DIAG_DNX_FIELD_UTILS_STR_SIZE);

        out_qual_value_buff_offset -= print_size;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_fwd_key_size_get(
    int unit,
    uint8 context_profile,
    uint32 *key_size_used_by_fwd_p)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_kbp_kbr_idx_e key_id;
    /** field_key_size_value_in_bytes defined as an array to fix coverity issue */
    uint32 field_key_size_value_in_bytes[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *key_size_used_by_fwd_p = 0;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_ALIGNER_MAPPING, &entry_handle_id));

    for (key_id = DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_0; key_id < DBAL_NOF_ENUM_KBP_KBR_IDX_VALUES; key_id++)
    {
        /** Specifying for which context_profile, the DBAL table info should be taken. */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, context_profile);

        /** Setting the key ID, for which we want to retrieve an information. */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, key_id);

        /** Perform the DBAL read. */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

        /** Get the key size for the current key. */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_FWD_KEY_SIZE, INST_SINGLE, field_key_size_value_in_bytes));

        /** If retrieved key size is not zero, then it is used by FWD. */
        if (field_key_size_value_in_bytes[0])
        {
            *key_size_used_by_fwd_p += BYTES2BITS(field_key_size_value_in_bytes[0]);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

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

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_kbp_segment_share_string_update(
    int unit,
    dnx_field_group_t fg_id,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
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
    dnx_field_group_kbp_segment_info_t seg_info;
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
    SHR_IF_ERR_EXIT(dnx_field_group_sw.kbp_info.master_key_info.
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

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_header_qual_signal_value_get(
    int unit,
    bcm_core_t core_id,
    dnx_field_context_t context_id,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t dnx_qual,
    dnx_field_qual_attach_info_t * attach_info_p,
    char out_qual_value[DIAG_DNX_FIELD_UTILS_STR_SIZE])
{
    signal_output_t *packet_header_signal_output = NULL;
    signal_output_t *fwd_layer_index_signal_output = NULL;
    signal_output_t *layer_offsets_signal_output = NULL;
    char *block = NULL;
    char *pkt_hdr_from = NULL;
    char *pkt_hdr_to = NULL;
    char *pkt_hdr_name = NULL;
    char *field_stage_from = NULL;
    char *field_stage_to = NULL;
    char *fwd_layer_index_name = NULL;
    char layer_offsets_sig_name[DIAG_DNX_FIELD_UTILS_STR_SIZE];
    uint32 nof_layer_record_indexes;
    int max_val_iter, val_iter;
    uint32 curr_qual_val[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY] = { 0 };
    dnx_field_key_attached_qual_info_t dnx_qual_info;
    int qual_size;
    int qual_offset;
    int extracted_layer_index = 0;
    int pbus_header_length;
    int start_offset_in_pkt_header;
    char *backup_pkt_hdr_from = NULL;
    char *backup_pkt_hdr_to = NULL;
    char *backup_pkt_hdr_name = NULL;

    SHR_FUNC_INIT_VARS(unit);

    sal_strncpy(out_qual_value, "0x", DIAG_DNX_FIELD_UTILS_STR_SIZE - 1);

    SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get
                    (unit, field_stage, context_id, dnx_qual, attach_info_p, &dnx_qual_info));
    if ((DNX_QUAL_CLASS(dnx_qual) != DNX_FIELD_QUAL_CLASS_HEADER
         && DNX_QUAL_CLASS(dnx_qual) != DNX_FIELD_QUAL_CLASS_USER)
        || (attach_info_p->input_type != DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE
            && attach_info_p->input_type != DNX_FIELD_INPUT_TYPE_LAYER_FWD))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Given qualifier class %s is not supported! Only %s qualifiers are supported!\r\n",
                     dnx_field_qual_class_text(DNX_QUAL_CLASS(dnx_qual)),
                     dnx_field_qual_class_text(DNX_FIELD_QUAL_CLASS_HEADER));
    }
    qual_size = dnx_qual_info.size;
    qual_offset = (attach_info_p->offset >= 0) ? dnx_qual_info.offset : (attach_info_p->offset + dnx_qual_info.offset);

    pbus_header_length = dnx_data_field.stage.stage_info_get(unit, field_stage)->pbus_header_length;

    if (pbus_header_length <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Given field stage %s doesn't support header qualifiers!\r\n",
                     dnx_field_stage_text(unit, field_stage));
    }

    /** Set relevant signal information per field_stage. */
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
        {
            block = "IRPP";
            pkt_hdr_from = "NIF";
            pkt_hdr_to = "PRT";
            pkt_hdr_name = "Header";
            field_stage_from = "IPMF1";
            field_stage_to = "FER";
            fwd_layer_index_name = "Fwd_Layer_Index";
            backup_pkt_hdr_from = "PRT";
            backup_pkt_hdr_to = "IParser";
            backup_pkt_hdr_name = "Pkt_Header";
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            block = "IRPP";
            pkt_hdr_from = "NIF";
            pkt_hdr_to = "PRT";
            pkt_hdr_name = "Header";
            field_stage_from = "IPMF3";
            field_stage_to = "LBP";
            fwd_layer_index_name = "Fwd_Layer_Index";
            backup_pkt_hdr_from = "PRT";
            backup_pkt_hdr_to = "IParser";
            backup_pkt_hdr_name = "Pkt_Header";
            break;
        }
        default:
        {
            bcm_field_stage_t bcm_stage;
            /** Convert DNX to BCM Field Stage. */
            SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage));
            LOG_CLI_EX("\r\n" "Given field stage (%s) is not supported!!%s%s%s\r\n\n",
                       dnx_field_bcm_stage_text(bcm_stage), EMPTY, EMPTY, EMPTY);
            break;
        }
    }

    /** Get information about packet header signal. */
    if (sand_signal_output_find
        (unit, core_id, 0, block, pkt_hdr_from, pkt_hdr_to, pkt_hdr_name, &packet_header_signal_output) != _SHR_E_NONE)
    {
        /** For special cases where instead of from "NIF" and "InNetworkAdaptor", used another block name as "PRT". */
        SHR_IF_ERR_EXIT(sand_signal_output_find
                        (unit, core_id, 0, block, backup_pkt_hdr_from, backup_pkt_hdr_to, backup_pkt_hdr_name,
                         &packet_header_signal_output));
    }

    /** Get the number of layer record indexes. */
    nof_layer_record_indexes = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_layer_records;
    if (nof_layer_record_indexes <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Stage %s does not have any layer records.\n",
                     dnx_field_stage_text(unit, field_stage));
    }

    /**
     * In case of LayerAbsolute we should just take the layer offset of the given input_arg.
     * Otherwise in case of LayerForwarding we should take in a count the forwarding layer index.
     */
    if (attach_info_p->input_type == DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE)
    {
        extracted_layer_index = dnx_qual_info.index;
    }
    else if (attach_info_p->input_type == DNX_FIELD_INPUT_TYPE_LAYER_FWD)
    {
        /** Get information about forwarding layer index signal. */
        SHR_IF_ERR_EXIT(sand_signal_output_find
                        (unit, core_id, 0, block, field_stage_from,
                         field_stage_to, fwd_layer_index_name, &fwd_layer_index_signal_output));

        extracted_layer_index =
            ((fwd_layer_index_signal_output->value[0] + dnx_qual_info.index) % nof_layer_record_indexes);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Given qualifier input_type %s is not supported! Only %s and %s qualifier input types are supported!\r\n",
                     dnx_field_input_type_text(attach_info_p->input_type),
                     dnx_field_input_type_text(DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE),
                     dnx_field_input_type_text(DNX_FIELD_INPUT_TYPE_LAYER_FWD));
    }

    if ((extracted_layer_index < 0) ||
        (extracted_layer_index >= dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_layer_records))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Calculated Layer Index:%d is not in rage for stage: %s. "
                     "Should be between 0 and %d.\n",
                     extracted_layer_index, dnx_field_stage_text(unit, field_stage), (nof_layer_record_indexes - 1));
    }

    sal_snprintf(layer_offsets_sig_name, sizeof(layer_offsets_sig_name), "Layer_Offsets.%d", extracted_layer_index);
    /** Get information about layer offsets signal. */
    SHR_IF_ERR_EXIT(sand_signal_output_find
                    (unit, core_id, 0, block, field_stage_from,
                     field_stage_to, layer_offsets_sig_name, &layer_offsets_signal_output));

    start_offset_in_pkt_header =
        ((qual_offset - BYTES2BITS(layer_offsets_signal_output->value[0])) % pbus_header_length);

    if ((start_offset_in_pkt_header + qual_size) > pbus_header_length)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Sum of the stating offset, in the header pbus, and qualifier size (%d)"
                     "is not in the header pbus rage (%d)!\r\n",
                     (start_offset_in_pkt_header + qual_size), pbus_header_length);
    }

    /** Get the exact value, of the current qualifier, from the received packet header signal value. */
    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                    (packet_header_signal_output->value, start_offset_in_pkt_header, qual_size, curr_qual_val));

    /** Construct the output string for the qualifier value. */
    max_val_iter = BITS2WORDS(qual_size);
    for (val_iter = max_val_iter - 1; val_iter >= 0 && qual_size; val_iter--)
    {
        char val_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
        int print_size = 0;
        /**
         * Array of strings, which are presenting the format in which the string will be written to val_buff.
         * Depends on current buffer size. Indicated how many nibbles we will present in the output string.
         * For example:
         *      If we have print_size == 9b, we will need 3 nibbles to present the value 0x123
         *          print_size == 6b ---> number of nibbles == 2
         *          print_size == 32b ---> number of nibbles == 8
         *          print_size == 16b ---> number of nibbles == 4
         *          print_size == 23b ---> number of nibbles == 6
         *
         *      Calculation of number of nibbles per given buffer size is done using
         *      ((print_size + 3) / 4)
         *
         *      The result from the above macro is also used as array index in string_format[((print_size + 3) / 4) - 1].
         */
        char string_format[SAL_UINT8_NOF_BITS][5] = { "%01X", "%02X", "%03X", "%04X", "%05X", "%06X", "%07X", "%08X" };
        if ((qual_size % SAL_UINT32_NOF_BITS) != 0)
        {
            print_size = (qual_size % SAL_UINT32_NOF_BITS);
        }
        else
        {
            print_size = SAL_UINT32_NOF_BITS;
        }
        sal_snprintf(val_buff, sizeof(val_buff), string_format[((print_size + 3) / 4) - 1], curr_qual_val[val_iter]);
        sal_strncat_s(out_qual_value, val_buff, DIAG_DNX_FIELD_UTILS_STR_SIZE);

        qual_size -= print_size;
    }

exit:
    SHR_FUNC_EXIT;
}

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
diag_dnx_field_utils_layer_record_qual_signal_value_get(
    int unit,
    bcm_core_t core_id,
    dnx_field_context_t context_id,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t dnx_qual,
    dnx_field_qual_attach_info_t * attach_info_p,
    char out_qual_value[DIAG_DNX_FIELD_UTILS_STR_SIZE])
{
    signal_output_t *layer_offsets_signal_output = NULL;
    signal_output_t *layer_protocols_signal_output = NULL;
    signal_output_t *layer_qualifiers_signal_output = NULL;
    signal_output_t *fwd_layer_index_signal_output = NULL;
    char layer_offsets_sig_name[DIAG_DNX_FIELD_UTILS_STR_SIZE];
    char layer_protocols_sig_name[DIAG_DNX_FIELD_UTILS_STR_SIZE];
    char layer_qualifiers_sig_name[DIAG_DNX_FIELD_UTILS_STR_SIZE];
    char *block = NULL;
    char *field_stage_from = NULL;
    char *field_stage_to = NULL;
    uint32 nof_layer_record_indexes;
    int max_val_iter, val_iter;
    uint32 curr_qual_val[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY] = { 0 };
    dnx_field_key_attached_qual_info_t dnx_qual_info;
    int qual_size;
    int qual_offset;
    int extracted_layer_index = 0;
    int pbus_header_length;
    int start_offset_in_layer_record_info;
    int layer_index;
    uint32 layer_records_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_LAYER_RECORDS *
                              DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_LAYER_RECORD_SIZE] = { 0 };
    int lr_offset_mapped_offset;
    int lr_protocol_mapped_offset;
    int lr_qualifier_mapped_offset;
    uint32 lr_offset_mapped_size;
    uint32 lr_protocol_mapped_size;
    uint32 lr_qualifier_mapped_size;
    int layer_record_size;
    int total_layer_record_pbus_size;
    int layer_record_info_array_layer_offset = 0;

    SHR_FUNC_INIT_VARS(unit);

    sal_strncpy(out_qual_value, "0x", DIAG_DNX_FIELD_UTILS_STR_SIZE - 1);

    /** Get the number of layer record indexes. */
    nof_layer_record_indexes = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_layer_records;
    pbus_header_length = dnx_data_field.stage.stage_info_get(unit, field_stage)->pbus_header_length;
    layer_record_size = dnx_data_field.stage.stage_info_get(unit, field_stage)->layer_record_size;
    total_layer_record_pbus_size = (layer_record_size * nof_layer_record_indexes);

    if ((DNX_QUAL_CLASS(dnx_qual) != DNX_FIELD_QUAL_CLASS_LAYER_RECORD
         && DNX_QUAL_CLASS(dnx_qual) != DNX_FIELD_QUAL_CLASS_USER)
        || (attach_info_p->input_type != DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD
            && attach_info_p->input_type != DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Given qualifier class %s is not supported! Only %s qualifiers are supported!\r\n",
                     dnx_field_qual_class_text(DNX_QUAL_CLASS(dnx_qual)),
                     dnx_field_qual_class_text(DNX_FIELD_QUAL_CLASS_LAYER_RECORD));
    }

    if (nof_layer_record_indexes <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Stage %s does not have any layer records.\n",
                     dnx_field_stage_text(unit, field_stage));
    }

    SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get
                    (unit, field_stage, context_id, dnx_qual, attach_info_p, &dnx_qual_info));
    qual_size = dnx_qual_info.size;

    /** Set relevant signal information per field_stage. */
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
        {
            block = "IRPP";
            field_stage_from = "IPMF1";
            field_stage_to = "FER";
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            block = "IRPP";
            field_stage_from = "IPMF3";
            field_stage_to = "LBP";
            break;
        }
        default:
        {
            bcm_field_stage_t bcm_stage;
            /** Convert DNX to BCM Field Stage. */
            SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage));
            LOG_CLI_EX("\r\n" "Given field stage (%s) is not supported!!%s%s%s\r\n\n",
                       dnx_field_bcm_stage_text(bcm_stage), EMPTY, EMPTY, EMPTY);
            break;
        }
    }

    /**
     * In case of LayerRecordsAbsolute we should just take the layer offset of the given input_arg.
     * Otherwise in case of LayerRecordsForwarding we should take in a count the forwarding layer index.
     */
    if (attach_info_p->input_type == DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE)
    {
        extracted_layer_index = 0;

        qual_offset = (dnx_qual_info.offset - pbus_header_length);
    }
    else if (attach_info_p->input_type == DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD)
    {
        /** Get information about forwarding layer index signal. */
        SHR_IF_ERR_EXIT(sand_signal_output_find
                        (unit, core_id, 0, block, field_stage_from,
                         field_stage_to, "Fwd_Layer_Index", &fwd_layer_index_signal_output));

        extracted_layer_index =
            ((fwd_layer_index_signal_output->value[0] + dnx_qual_info.index) % nof_layer_record_indexes);

        qual_offset = dnx_qual_info.offset;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Given qualifier input_type %s is not supported! Only %s and %s qualifier input types are supported!\r\n",
                     dnx_field_input_type_text(attach_info_p->input_type),
                     dnx_field_input_type_text(DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE),
                     dnx_field_input_type_text(DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD));
    }

    if ((extracted_layer_index < 0) ||
        (extracted_layer_index >= dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_layer_records))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Calculated Layer Index:%d is not in rage for stage: %s. "
                     "Should be between 0 and %d.\n",
                     extracted_layer_index, dnx_field_stage_text(unit, field_stage), (nof_layer_record_indexes - 1));
    }

    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset
                    (unit, field_stage,
                     DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, field_stage, DNX_FIELD_LR_QUAL_OFFSET),
                     &lr_offset_mapped_offset));
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                    (unit, field_stage,
                     DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, field_stage, DNX_FIELD_LR_QUAL_OFFSET),
                     &lr_offset_mapped_size));
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset
                    (unit, field_stage,
                     DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, field_stage, DNX_FIELD_LR_QUAL_PROTOCOL),
                     &lr_protocol_mapped_offset));
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                    (unit, field_stage,
                     DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, field_stage, DNX_FIELD_LR_QUAL_PROTOCOL),
                     &lr_protocol_mapped_size));
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset
                    (unit, field_stage,
                     DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, field_stage, DNX_FIELD_LR_QUAL_QUALIFIER),
                     &lr_qualifier_mapped_offset));
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                    (unit, field_stage,
                     DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, field_stage, DNX_FIELD_LR_QUAL_QUALIFIER),
                     &lr_qualifier_mapped_size));

    /** Construct the layer record info as follow: layer_info = layer_offset | layer_qualifier | layer_protocol. */
    for (layer_index = 0; layer_index < nof_layer_record_indexes; layer_index++)
    {
        sal_snprintf(layer_offsets_sig_name, sizeof(layer_offsets_sig_name), "Layer_Offsets.%d", layer_index);
        /** Get information about layer offsets signal. */
        SHR_IF_ERR_EXIT(sand_signal_output_find
                        (unit, core_id, 0, block, field_stage_from,
                         field_stage_to, layer_offsets_sig_name, &layer_offsets_signal_output));

        sal_snprintf(layer_protocols_sig_name, sizeof(layer_protocols_sig_name), "Layer_Protocols.%d", layer_index);
         /** Get information about layer protocols signal. */
        SHR_IF_ERR_EXIT(sand_signal_output_find
                        (unit, core_id, 0, block, field_stage_from,
                         field_stage_to, layer_protocols_sig_name, &layer_protocols_signal_output));

        sal_snprintf(layer_qualifiers_sig_name, sizeof(layer_qualifiers_sig_name), "Layer_Qualifiers.%d", layer_index);
         /** Get information about layer qualifiers signal. */
        SHR_IF_ERR_EXIT(sand_signal_output_find
                        (unit, core_id, 0, block, field_stage_from,
                         field_stage_to, layer_qualifiers_sig_name, &layer_qualifiers_signal_output));

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (layer_offsets_signal_output->value,
                         (lr_offset_mapped_offset + layer_record_info_array_layer_offset), lr_offset_mapped_size,
                         layer_records_info));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (layer_qualifiers_signal_output->value,
                         (lr_qualifier_mapped_offset + layer_record_info_array_layer_offset), lr_qualifier_mapped_size,
                         layer_records_info));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (layer_protocols_signal_output->value,
                         (lr_protocol_mapped_offset + layer_record_info_array_layer_offset), lr_protocol_mapped_size,
                         layer_records_info));

        /** Set the starting offset for every new layer on every iteration. */
        layer_record_info_array_layer_offset += layer_record_size;
    }

    start_offset_in_layer_record_info = ((qual_offset) % total_layer_record_pbus_size);

    if (start_offset_in_layer_record_info > total_layer_record_pbus_size)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "The starting offset, in the layer record pbus, (%d)"
                     "is not in the layer records range (%d)!\r\n",
                     start_offset_in_layer_record_info, total_layer_record_pbus_size);
    }

    if ((start_offset_in_layer_record_info + qual_size) > total_layer_record_pbus_size)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Sum of the starting offset, in the layer record pbus, and qualifier size (%d)"
                     "is not in the layer records range (%d)!\r\n",
                     (start_offset_in_layer_record_info + qual_size), total_layer_record_pbus_size);
    }

    /** Get the exact value, of the current qualifier, from the constructed LR info. */
    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                    (layer_records_info, start_offset_in_layer_record_info, qual_size, curr_qual_val));

    /** Construct the output string for the qualifier value. */
    max_val_iter = BITS2WORDS(qual_size);
    for (val_iter = max_val_iter - 1; val_iter >= 0 && qual_size; val_iter--)
    {
        char val_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
        int print_size = 0;
        /**
         * Array of strings, which are presenting the format in which the string will be written to val_buff.
         * Depends on current buffer size. Indicated how many nibbles we will present in the output string.
         * For example:
         *      If we have print_size == 9b, we will need 3 nibbles to present the value 0x123
         *          print_size == 6b ---> number of nibbles == 2
         *          print_size == 32b ---> number of nibbles == 8
         *          print_size == 16b ---> number of nibbles == 4
         *          print_size == 23b ---> number of nibbles == 6
         *
         *      Calculation of number of nibbles per given buffer size is done using
         *      ((print_size + 3) / 4)
         *
         *      The result from the above macro is also used as array index in string_format[((print_size + 3) / 4) - 1].
         */
        char string_format[SAL_UINT8_NOF_BITS][5] = { "%01X", "%02X", "%03X", "%04X", "%05X", "%06X", "%07X", "%08X" };
        if ((qual_size % SAL_UINT32_NOF_BITS) != 0)
        {
            print_size = (qual_size % SAL_UINT32_NOF_BITS);
        }
        else
        {
            print_size = SAL_UINT32_NOF_BITS;
        }
        sal_snprintf(val_buff, sizeof(val_buff), string_format[((print_size + 3) / 4) - 1], curr_qual_val[val_iter]);
        sal_strncat_s(out_qual_value, val_buff, DIAG_DNX_FIELD_UTILS_STR_SIZE);

        qual_size -= print_size;
    }

exit:
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
