
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX


#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_actions_types.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_actions.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_field_access.h>








#define DNX_FIELD_ACTION_PREFIX_TO_MASK(prefix, action_size)    ((prefix) << (action_size))
extern shr_error_e dbal_fields_field_types_info_get(
    int unit,
    dbal_fields_e field_id,
    dbal_field_types_basic_info_t ** field_type_info);



char *
dnx_field_io_text(
    dbal_enum_value_field_field_io_e field_io)
{
    char *ret;
    switch (field_io)
    {
        case (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0):
        {
            ret = "INTERNAL_TCAM_0";
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1):
        {
            ret = "INTERNAL_TCAM_1";
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_2):
        {
            ret = "INTERNAL_TCAM_2";
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_3):
        {
            ret = "INTERNAL_TCAM_3";
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_4):
        {
            ret = "INTERNAL_TCAM_4";
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_5):
        {
            ret = "INTERNAL_TCAM_5";
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_6):
        {
            ret = "INTERNAL_TCAM_6";
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_7):
        {
            ret = "INTERNAL_TCAM_7";
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_DIRECT_0):
        {
            ret = "DIRECT_0";
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_DIRECT_1):
        {
            ret = "DIRECT_1";
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_LEXEM):
        {
            ret = "LEXEM";
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_SEXEM):
        {
            ret = "SEXEM";
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0_1):
        {
            ret = "INTERNAL_TCAM_0_1";
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1_2):
        {
            ret = "INTERNAL_TCAM_1_2";
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_2_3):
        {
            ret = "INTERNAL_TCAM_2_3";
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_4_5):
        {
            ret = "INTERNAL_TCAM_4_5";
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_5_6):
        {
            ret = "INTERNAL_TCAM_5_6";
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_6_7):
        {
            ret = "INTERNAL_TCAM_6_7";
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_ACE):
        {
            ret = "ACE";
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_INVALID_INTERNAL_IO):
        {
            ret = "INVALID_INTERNAL_IO";
            break;
        }
        case (DBAL_NOF_ENUM_FIELD_IO_VALUES):
        {
            ret = "NUM_IO_VALUES";
            break;
        }
        default:
        {
            ret = "UNKNOWN_IO";
            break;
        }
    }
    return (ret);
}


shr_error_e
dnx_field_actions_io_to_replace_select(
    int unit,
    dbal_enum_value_field_field_io_e field_io,
    dbal_enum_value_field_field_fem_replace_lsb_select_e * replace_lsb_select_p)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (field_io)
    {
        case (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0):
        {
            *replace_lsb_select_p = DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_0;
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1):
        {
            *replace_lsb_select_p = DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_1;
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_2):
        {
            *replace_lsb_select_p = DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_2;
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_3):
        {
            *replace_lsb_select_p = DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_3;
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_4):
        {
            *replace_lsb_select_p = DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_4;
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_5):
        {
            *replace_lsb_select_p = DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_5;
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_6):
        {
            *replace_lsb_select_p = DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_6;
            break;
        }
        case (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_7):
        {
            *replace_lsb_select_p = DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_7;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Specified IO (%s) for 'replace' must be of TCAM type only (for FEMs). Range is %d to %d (incl. edges)\r\n",
                         dnx_field_io_text(field_io), DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0,
                         DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_7);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_actions_ace_fes_mask_hw_set(
    int unit,
    uint32 fes_id,
    uint32 fes_mask_id,
    uint32 action_mask)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "fes_mask_id %d fes_id %d action_mask 0x%08X %s\r\n", fes_mask_id, fes_id, action_mask, EMPTY);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_ACE_EFES_ACTION_MASK, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_ID, fes_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_MASK_ID, fes_mask_id);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_ACTION_MASK, INST_SINGLE, action_mask);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_action_attach_info_single_t_init(
    int unit,
    dnx_field_action_attach_info_t * action_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(action_info_p, _SHR_E_PARAM, "action_info_p");

    action_info_p->priority = BCM_FIELD_ACTION_DONT_CARE;
    action_info_p->valid_bit_polarity = DNX_FIELD_ACTION_FES_VALID_BIT_POLARITY_ONE;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_action_attach_info_t_init(
    int unit,
    dnx_field_action_attach_info_t action_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP])
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(action_info, _SHR_E_PARAM, "action_info");

    for (ii = 0; ii < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP; ii++)
    {
        SHR_IF_ERR_EXIT(dnx_field_action_attach_info_single_t_init(unit, &action_info[ii]));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_actions_fes_single_add_conf_t_init(
    int unit,
    dnx_field_actions_fes_single_add_conf_t efes_action_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES])
{
    int fes_prog_index;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(efes_action_info, _SHR_E_PARAM, "efes_action_info");

    sal_memset(efes_action_info, 0, sizeof(efes_action_info[0]) * DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES);

    for (fes_prog_index = 0; fes_prog_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES; fes_prog_index++)
    {
        efes_action_info[fes_prog_index].dnx_action = DNX_FIELD_ACTION_INVALID;
        efes_action_info[fes_prog_index].valid_bit_polarity = DNX_FIELD_ACTION_FES_VALID_BIT_POLARITY_ONE;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_actions_ace_fes_mask_hw_get(
    int unit,
    uint32 fes_id,
    uint32 fes_mask_id,
    uint32 *action_mask_p)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(action_mask_p, _SHR_E_PARAM, "action_mask_p");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_ACE_EFES_ACTION_MASK, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_ID, fes_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_MASK_ID, fes_mask_id);
    
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_ACTION_MASK, INST_SINGLE, action_mask_p);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "fes_mask_id %d fes_id %d action_mask 0x%08X %s\r\n", fes_mask_id, fes_id, *action_mask_p, EMPTY);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_actions_ace_fes_program_table_hw_set(
    int unit,
    unsigned int fes_id,
    dnx_field_ace_id_t ace_id,
    dnx_field_actions_fes_common_info_fes2msb_t * fes_inst_common_info_fes2msb_p,
    dnx_field_fes_chosen_mask_t chosen_mask)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fes_inst_common_info_fes2msb_p, _SHR_E_PARAM, "fes_inst_common_info_fes2msb_p");
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Write to ACE ACR: %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "fes_id %d, ace_id %d, %s%s", fes_id, ace_id, EMPTY, EMPTY);
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "action_type %d, valid_bits %d shift %d type %d, ",
                 fes_inst_common_info_fes2msb_p->action_type, fes_inst_common_info_fes2msb_p->valid_bits,
                 fes_inst_common_info_fes2msb_p->shift, fes_inst_common_info_fes2msb_p->type);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "polarity %d, chosen_mask %d. %s%s\r\n",
                 fes_inst_common_info_fes2msb_p->polarity, chosen_mask, EMPTY, EMPTY);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_ACE_FES_PROGRAM_TABLE, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_ID, fes_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_ACE_RESULT_TYPE, ace_id);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACE_ACTION, INST_SINGLE,
                                 (uint32) (fes_inst_common_info_fes2msb_p->action_type));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_INVALID_BITS, INST_SINGLE,
                                 (uint32) (fes_inst_common_info_fes2msb_p->valid_bits));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_SHIFT, INST_SINGLE,
                                 (uint32) (fes_inst_common_info_fes2msb_p->shift));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_TYPE, INST_SINGLE,
                                 (uint32) (fes_inst_common_info_fes2msb_p->type));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_POLARITY, INST_SINGLE,
                                 (uint32) (fes_inst_common_info_fes2msb_p->polarity));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_CHOSEN_MASK, INST_SINGLE,
                                 (uint32) (chosen_mask));

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_actions_ace_fes_program_table_hw_get(
    int unit,
    unsigned int fes_id,
    dnx_field_ace_id_t ace_id,
    dnx_field_actions_fes_common_info_fes2msb_t * fes_inst_common_info_fes2msb_p,
    dnx_field_fes_chosen_mask_t * chosen_mask_p)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fes_inst_common_info_fes2msb_p, _SHR_E_PARAM, "fes_inst_common_info_fes2msb_p");
    SHR_NULL_CHECK(chosen_mask_p, _SHR_E_PARAM, "chosen_mask_p");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_ACE_FES_PROGRAM_TABLE, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_ID, fes_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_ACE_RESULT_TYPE, ace_id);
    
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_ACE_ACTION, INST_SINGLE,
                                                       &(fes_inst_common_info_fes2msb_p->action_type)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_FIELD_FES_INVALID_BITS, INST_SINGLE,
                     &(fes_inst_common_info_fes2msb_p->valid_bits)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_FIELD_FES_SHIFT, INST_SINGLE,
                     &(fes_inst_common_info_fes2msb_p->shift)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_FIELD_FES_TYPE, INST_SINGLE,
                     &(fes_inst_common_info_fes2msb_p->type)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_FIELD_FES_POLARITY, INST_SINGLE,
                     &(fes_inst_common_info_fes2msb_p->polarity)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_FIELD_FES_CHOSEN_MASK, INST_SINGLE, chosen_mask_p));

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Read from ACE ACR: %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "fes_id %d, ace_id %d, %s%s", fes_id, ace_id, EMPTY, EMPTY);
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "action_type %d, valid_bits %d shift %d type %d, ",
                 fes_inst_common_info_fes2msb_p->action_type, fes_inst_common_info_fes2msb_p->valid_bits,
                 fes_inst_common_info_fes2msb_p->shift, fes_inst_common_info_fes2msb_p->type);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "polarity %d, chosen_mask %d. %s%s\r\n",
                 fes_inst_common_info_fes2msb_p->polarity, *chosen_mask_p, EMPTY, EMPTY);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_actions_fes_mask_hw_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_fes_id_t fes_id,
    dnx_field_fes_mask_id_t fes_mask_id,
    dnx_field_fes_mask_t action_mask)
{
    uint32 entry_handle_id;
    dbal_tables_e efes_mask_dbal_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "writing fes_mask_id %d fes_id %d action_mask 0x%08X.%s\r\n", fes_mask_id, fes_id, action_mask, EMPTY);

    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
        {
            efes_mask_dbal_table_id = DBAL_TABLE_FIELD_PMF_A_EFES_ACTION_MASK;
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            efes_mask_dbal_table_id = DBAL_TABLE_FIELD_PMF_B_EFES_ACTION_MASK;
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            efes_mask_dbal_table_id = DBAL_TABLE_FIELD_E_PMF_EFES_ACTION_MASK;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage for function \"%s\" (%d).\r\n",
                         dnx_field_stage_text(unit, field_stage), field_stage);
        }
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, efes_mask_dbal_table_id, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_ID, fes_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_MASK_ID, fes_mask_id);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_ACTION_MASK, INST_SINGLE, action_mask);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_actions_fes_mask_hw_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_fes_id_t fes_id,
    dnx_field_fes_mask_id_t fes_mask_id,
    dnx_field_fes_mask_t * action_mask_p)
{
    uint32 entry_handle_id;
    dbal_tables_e efes_mask_dbal_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
        {
            efes_mask_dbal_table_id = DBAL_TABLE_FIELD_PMF_A_EFES_ACTION_MASK;
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            efes_mask_dbal_table_id = DBAL_TABLE_FIELD_PMF_B_EFES_ACTION_MASK;
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            efes_mask_dbal_table_id = DBAL_TABLE_FIELD_E_PMF_EFES_ACTION_MASK;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage for function \"%s\" (%d).\r\n",
                         dnx_field_stage_text(unit, field_stage), field_stage);
        }
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, efes_mask_dbal_table_id, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_ID, fes_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_MASK_ID, fes_mask_id);
    
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_ACTION_MASK, INST_SINGLE, action_mask_p);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "fes_mask_id %d fes_id %d action_mask 0x%08X %s\r\n", fes_mask_id, fes_id, *action_mask_p, EMPTY);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_actions_fes_2nd_instruction_hw_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_fes_id_t fes_id,
    dnx_field_fes_pgm_id_t fes_pgm_id,
    unsigned int fes2msb_ndx,
    dnx_field_actions_fes_common_info_fes2msb_t * fes_inst_common_info_fes2msb_p,
    dnx_field_fes_chosen_mask_t chosen_mask)
{
    uint32 entry_handle_id;
    dbal_fields_e action_dbal_field_id;
    dbal_tables_e fes_2nd_str_dbal_table_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fes_inst_common_info_fes2msb_p, _SHR_E_PARAM, "fes_inst_common_info_p");

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Writing to stage %d: fes_id %d, fes_pgm_id %d, fes2msb_ndx %d.\r\n",
                 field_stage, fes_id, fes_pgm_id, fes2msb_ndx);
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "==> action_type %d valid_bits %d shift %d type %d ",
                 fes_inst_common_info_fes2msb_p->action_type, fes_inst_common_info_fes2msb_p->valid_bits,
                 fes_inst_common_info_fes2msb_p->shift, fes_inst_common_info_fes2msb_p->type);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "polarity %d chosen_mask %d.%s%s\r\n",
                 fes_inst_common_info_fes2msb_p->polarity, chosen_mask, EMPTY, EMPTY);

    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
        {
            action_dbal_field_id = DBAL_FIELD_IPMF1_ACTION;
            fes_2nd_str_dbal_table_id = DBAL_TABLE_FIELD_PMF_A_FES_2ND_INSTRUCTION;
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            action_dbal_field_id = DBAL_FIELD_IPMF3_ACTION;
            fes_2nd_str_dbal_table_id = DBAL_TABLE_FIELD_PMF_B_FES_2ND_INSTRUCTION;
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {

            action_dbal_field_id = DBAL_FIELD_EPMF_ACTION;
            fes_2nd_str_dbal_table_id = DBAL_TABLE_FIELD_E_PMF_FES_2ND_INSTRUCTION;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage for function \"%s\"(%d).\r\n",
                         dnx_field_stage_text(unit, field_stage), field_stage);
        }
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, fes_2nd_str_dbal_table_id, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_ID, fes_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_PGM_ID, fes_pgm_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_FES_DATA_2MSB, fes2msb_ndx);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, action_dbal_field_id, INST_SINGLE,
                                 (uint32) (fes_inst_common_info_fes2msb_p->action_type));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_INVALID_BITS, INST_SINGLE,
                                 (uint32) (fes_inst_common_info_fes2msb_p->valid_bits));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_SHIFT, INST_SINGLE,
                                 (uint32) (fes_inst_common_info_fes2msb_p->shift));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_TYPE, INST_SINGLE,
                                 (uint32) (fes_inst_common_info_fes2msb_p->type));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_POLARITY, INST_SINGLE,
                                 (uint32) (fes_inst_common_info_fes2msb_p->polarity));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_CHOSEN_MASK, INST_SINGLE,
                                 (uint32) (chosen_mask));

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_actions_fes_2nd_instruction_hw_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_fes_id_t fes_id,
    dnx_field_fes_pgm_id_t fes_pgm_id,
    unsigned int fes2msb_ndx,
    dnx_field_actions_fes_common_info_fes2msb_t * fes_inst_common_info_fes2msb_p,
    dnx_field_fes_chosen_mask_t * chosen_mask_p)
{
    uint32 entry_handle_id;
    dbal_fields_e action_dbal_field_id;
    dbal_tables_e fes_2nd_str_dbal_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fes_inst_common_info_fes2msb_p, _SHR_E_PARAM, "fes_inst_common_info_p");
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Reading from stage %d: fes_id %d, fes_pgm_id %d, fes2msb_ndx %d.\r\n",
                 field_stage, fes_id, fes_pgm_id, fes2msb_ndx);

    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
        {
            action_dbal_field_id = DBAL_FIELD_IPMF1_ACTION;
            fes_2nd_str_dbal_table_id = DBAL_TABLE_FIELD_PMF_A_FES_2ND_INSTRUCTION;
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            action_dbal_field_id = DBAL_FIELD_IPMF3_ACTION;
            fes_2nd_str_dbal_table_id = DBAL_TABLE_FIELD_PMF_B_FES_2ND_INSTRUCTION;
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {

            action_dbal_field_id = DBAL_FIELD_EPMF_ACTION;
            fes_2nd_str_dbal_table_id = DBAL_TABLE_FIELD_E_PMF_FES_2ND_INSTRUCTION;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage for function \"%s\"(%d).\r\n",
                         dnx_field_stage_text(unit, field_stage), field_stage);
        }
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, fes_2nd_str_dbal_table_id, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_ID, fes_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_PGM_ID, fes_pgm_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_FES_DATA_2MSB, fes2msb_ndx);
    
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit, entry_handle_id, action_dbal_field_id, INST_SINGLE,
                                                       &(fes_inst_common_info_fes2msb_p->action_type)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_FIELD_FES_INVALID_BITS, INST_SINGLE,
                     &(fes_inst_common_info_fes2msb_p->valid_bits)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_FIELD_FES_SHIFT, INST_SINGLE,
                     &(fes_inst_common_info_fes2msb_p->shift)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_FIELD_FES_TYPE, INST_SINGLE,
                     &(fes_inst_common_info_fes2msb_p->type)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_FIELD_FES_POLARITY, INST_SINGLE,
                     &(fes_inst_common_info_fes2msb_p->polarity)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_FIELD_FES_CHOSEN_MASK, INST_SINGLE, chosen_mask_p));

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Read from stage %d fes_id %d, fes_pgm_id %d, fes2msb_ndx %d:\r\n",
                 field_stage, fes_id, fes_pgm_id, fes2msb_ndx);
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "action type %d, valid_bits %d shift %d type %d, ",
                 fes_inst_common_info_fes2msb_p->action_type, fes_inst_common_info_fes2msb_p->valid_bits,
                 fes_inst_common_info_fes2msb_p->shift, fes_inst_common_info_fes2msb_p->type);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "polarity %d, chosen_mask %d. %s%s\r\n",
                 fes_inst_common_info_fes2msb_p->polarity, *chosen_mask_p, EMPTY, EMPTY);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_fes_program_fes_instruction_hw_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_fes_id_t fes_id,
    dnx_field_fes_pgm_id_t fes_pgm_id,
    dnx_field_fes_key_select_t key_select)
{
    uint32 entry_handle_id;
    dbal_fields_e key_select_dbal_field_id;
    dbal_tables_e select_key_n_fes_dbal_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    LOG_DEBUG_EX(BSL_LOG_MODULE, "field_stage %d: %s%s%s\r\n", context_id, EMPTY, EMPTY, EMPTY);
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Context_id %d. fes_id %d fes_pgm_id %d key_select %d\r\n",
                 context_id, (int) fes_id, (int) fes_pgm_id, key_select);

    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
        {
            key_select_dbal_field_id = DBAL_FIELD_FIELD_PMF_A_FES_KEY_SELECT;
            select_key_n_fes_dbal_table_id = DBAL_TABLE_FIELD_PMF_A_SELECT_KEY_N_FES;
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            key_select_dbal_field_id = DBAL_FIELD_FIELD_PMF_B_FES_KEY_SELECT;
            select_key_n_fes_dbal_table_id = DBAL_TABLE_FIELD_PMF_B_SELECT_KEY_N_FES;
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {

            key_select_dbal_field_id = DBAL_FIELD_FIELD_E_PMF_FES_KEY_SELECT;
            select_key_n_fes_dbal_table_id = DBAL_TABLE_FIELD_E_PMF_SELECT_KEY_N_FES;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage for function \"%s\"(%d).\r\n",
                         dnx_field_stage_text(unit, field_stage), field_stage);
        }
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, select_key_n_fes_dbal_table_id, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, (uint32) context_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_ID, (uint32) fes_id);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, key_select_dbal_field_id, INST_SINGLE, (uint32) key_select);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_PGM_ID, INST_SINGLE, (uint32) fes_pgm_id);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_fes_program_fes_instruction_hw_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_fes_id_t fes_id,
    dnx_field_fes_pgm_id_t * fes_pgm_id_p,
    dnx_field_fes_key_select_t * key_select_p)
{
    uint32 entry_handle_id;
    dbal_fields_e key_select_dbal_field_id;
    dbal_tables_e select_key_n_fes_dbal_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
        {
            key_select_dbal_field_id = DBAL_FIELD_FIELD_PMF_A_FES_KEY_SELECT;
            select_key_n_fes_dbal_table_id = DBAL_TABLE_FIELD_PMF_A_SELECT_KEY_N_FES;
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            key_select_dbal_field_id = DBAL_FIELD_FIELD_PMF_B_FES_KEY_SELECT;
            select_key_n_fes_dbal_table_id = DBAL_TABLE_FIELD_PMF_B_SELECT_KEY_N_FES;
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {

            key_select_dbal_field_id = DBAL_FIELD_FIELD_E_PMF_FES_KEY_SELECT;
            select_key_n_fes_dbal_table_id = DBAL_TABLE_FIELD_E_PMF_SELECT_KEY_N_FES;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage for function \"%s\"(%d).\r\n",
                         dnx_field_stage_text(unit, field_stage), field_stage);
        }
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, select_key_n_fes_dbal_table_id, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, (uint32) context_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_ID, (uint32) fes_id);
    
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit, entry_handle_id, key_select_dbal_field_id,
                                                       INST_SINGLE, key_select_p));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_PGM_ID,
                                                       INST_SINGLE, fes_pgm_id_p));

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Reading for stage %d Context_ID %d, FES_ID %d:%s\r\n", field_stage, context_id, fes_id, EMPTY);
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "==> program ID %d key select %d.%s%s\r\n", *fes_pgm_id_p, *key_select_p, EMPTY, EMPTY);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_action_in_group_info_t_init(
    int unit,
    dnx_field_action_in_group_info_t * struct_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(struct_p, _SHR_E_PARAM, "struct_p");

    sal_memset(struct_p, 0x0, sizeof(*struct_p));

    struct_p->dnx_action = DNX_FIELD_ACTION_INVALID;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_action_in_group_bit_size(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_action_in_group_info_t * action_p,
    unsigned int *action_size_w_valid_p,
    unsigned int *action_size_wo_valid_p)
{
    unsigned int mapped_size;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(action_p, _SHR_E_PARAM, "action_p");
    SHR_NULL_CHECK(action_size_w_valid_p, _SHR_E_PARAM, "action_size_w_valid_p");
    SHR_NULL_CHECK(action_size_wo_valid_p, _SHR_E_PARAM, "action_size_wo_valid_p");

    
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, field_stage, action_p->dnx_action, &mapped_size));
    
    if ((int) mapped_size < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "The size of DNX action 0x%X is negative: %d.\r\n", action_p->dnx_action, mapped_size);
    }
    (*action_size_wo_valid_p) = mapped_size;
    
    if (action_p->dont_use_valid_bit)
    {
        (*action_size_w_valid_p) = (*action_size_wo_valid_p);
    }
    else
    {
        (*action_size_w_valid_p) = (*action_size_wo_valid_p) + 1;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_actions_fes_key_select_block_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dbal_enum_value_field_field_io_e actions_block)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((actions_block < 0) || (actions_block >= DBAL_NOF_ENUM_FIELD_IO_VALUES))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal value of field_io %d.\n", actions_block);
    }

    
    
    if (actions_block == DBAL_ENUM_FVAL_FIELD_IO_LEXEM)
    {
        if ((field_stage != DNX_FIELD_STAGE_IPMF1) && (field_stage != DNX_FIELD_STAGE_EPMF))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field Stage=%d does not support LEXEM \n", field_stage);
        }
    }
    else if (actions_block == DBAL_ENUM_FVAL_FIELD_IO_SEXEM)
    {
        if ((field_stage != DNX_FIELD_STAGE_IPMF2) && (field_stage != DNX_FIELD_STAGE_IPMF3))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field Stage=%d does not support SEXEM \n", field_stage);
        }
    }
    else if ((actions_block == DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_2) ||
             (actions_block == DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_3) ||
             (actions_block == DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_2_3) ||
             (actions_block == DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1_2))
    {
        if (field_stage != DNX_FIELD_STAGE_IPMF1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Field Stage=%d does not support action_block INTERNAL_TCAM_2/INTERNAL_TCAM_3 and combinations\n",
                         field_stage);
        }
    }
    else if ((actions_block == DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_4) ||
             (actions_block == DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_5) ||
             (actions_block == DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_6) ||
             (actions_block == DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_7) ||
             (actions_block == DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_4_5) ||
             (actions_block == DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_5_6) ||
             (actions_block == DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_6_7))
    {
        if (field_stage != DNX_FIELD_STAGE_IPMF2)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Field Stage=%d does not support action_block INTERNAL TCAM_4/TCAM_5/TCAM_6/TCAM_7 and combinations\n",
                         field_stage);
        }
    }
    else if ((actions_block == DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0) ||
             (actions_block == DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1) ||
             (actions_block == DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0_1))
    {
        if ((field_stage != DNX_FIELD_STAGE_IPMF1) &&
            (field_stage != DNX_FIELD_STAGE_IPMF2) &&
            (field_stage != DNX_FIELD_STAGE_IPMF3) && (field_stage != DNX_FIELD_STAGE_EPMF))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Field Stage=%d does not support action_block INTERNAL_TCAM_0/INTERNAL_TCAM_1 and combinations\n",
                         field_stage);
        }
    }
    else if (actions_block == DBAL_ENUM_FVAL_FIELD_IO_ACE)
    {
        if (field_stage != DNX_FIELD_STAGE_ACE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field Stage=%d does not support action_block ACE\n", field_stage);
        }
    }
    else if (actions_block == DBAL_ENUM_FVAL_FIELD_IO_DIRECT_0)
    {
        if (field_stage != DNX_FIELD_STAGE_IPMF1 && field_stage != DNX_FIELD_STAGE_IPMF3)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field Stage=%d does not support action_block DIRECT TABLE\n", field_stage);
        }
    }
    else if (actions_block == DBAL_ENUM_FVAL_FIELD_IO_DIRECT_1)
    {
        if (field_stage != DNX_FIELD_STAGE_IPMF2)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field Stage=%d does not support action_block DIRECT_TABLE_1\n", field_stage);
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_actions_fes_key_select_get(
    int unit,
    dbal_enum_value_field_field_io_e actions_block,
    dnx_field_stage_e field_stage,
    unsigned int action_size,
    unsigned int action_lsb,
    int force_action_input_select,
    unsigned int *action_input_select_p,
    unsigned int *required_shift_p,
    int *found_p)
{
    unsigned int key_select_ndx;
    int found;
    int required_shift;
    dnx_field_stage_e acr_stage;
    const dnx_data_field_efes_key_select_properties_t *field_io_key_select_info_p;
    int max_nof_key_selects_per_field_io = dnx_data_field.efes.max_nof_key_selects_per_field_io_get(unit);
    

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(action_input_select_p, _SHR_E_PARAM, "action_input_select_p");
    SHR_NULL_CHECK(required_shift_p, _SHR_E_PARAM, "required_shift_p");
    SHR_NULL_CHECK(found_p, _SHR_E_PARAM, "found_p");

    SHR_INVOKE_VERIFY_DNX(dnx_field_actions_fes_key_select_block_verify(unit, field_stage, actions_block));

    if (field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        acr_stage = DNX_FIELD_STAGE_IPMF2;
    }
    else
    {
        acr_stage = field_stage;
    }
    *found_p = FALSE;
    found = FALSE;

    
    if (action_size == 0)
    {
        *action_input_select_p = dnx_data_field.stage.stage_info_get(unit, field_stage)->fes_key_select_for_zero_bit;
        *required_shift_p = dnx_data_field.stage.stage_info_get(unit, field_stage)->fes_shift_for_zero_bit;
        if (((int) (*required_shift_p)) < 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage \"%s\"(%d) for zero sized action.\r\n",
                         dnx_field_stage_text(unit, field_stage), field_stage);
        }
        found = TRUE;
    }
    else
    {
        field_io_key_select_info_p = dnx_data_field.efes.key_select_properties_get(unit, acr_stage, actions_block);
        for (key_select_ndx = 0; key_select_ndx < max_nof_key_selects_per_field_io; key_select_ndx++)
        {
            if (force_action_input_select &&
                (field_io_key_select_info_p->key_select[key_select_ndx] != (*action_input_select_p)))
            {
                continue;
            }
            if (field_io_key_select_info_p->num_bits[key_select_ndx] != 0)
            {
                required_shift = action_lsb - field_io_key_select_info_p->lsb[key_select_ndx];
                if (required_shift >= 0)
                {
                    
                    if ((action_size + required_shift) <= field_io_key_select_info_p->num_bits[key_select_ndx])
                    {
                        
                        *action_input_select_p = field_io_key_select_info_p->key_select[key_select_ndx];
                        *required_shift_p =
                            required_shift + field_io_key_select_info_p->num_bits_not_on_key[key_select_ndx];
                        found = TRUE;
                        break;
                    }
                }
            }
        }
    }
    *found_p = found;
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_actions_fes_key_select_2msb_get(
    int unit,
    dbal_enum_value_field_field_io_e actions_block,
    dnx_field_stage_e field_stage,
    int shift,
    int group_lsb,
    int group_msb,
    int nof_msb_bits,
    int requested_msb_of_2msb_in_group,
    int *min_position_2msb_p,
    int *max_position_2msb_p,
    unsigned int *action_input_select_2msb_p,
    int *found_requested_p,
    int *found_any_in_range_p)
{
    unsigned int key_select_ndx;
    dnx_field_stage_e acr_stage;
    const dnx_data_field_efes_key_select_properties_t *field_io_key_select_info_p;
    int found_any = FALSE;
    int found_specific = FALSE;
    int position_of_msb_on_key_select =
        dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_bits_in_fes_key_select - 1;
    int position_of_msb_in_group;
    int max_nof_key_selects_per_field_io = dnx_data_field.efes.max_nof_key_selects_per_field_io_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(min_position_2msb_p, _SHR_E_PARAM, "min_position_2msb_p");
    SHR_NULL_CHECK(max_position_2msb_p, _SHR_E_PARAM, "max_position_2msb_p");
    SHR_NULL_CHECK(action_input_select_2msb_p, _SHR_E_PARAM, "action_input_select_2msb_p");
    SHR_NULL_CHECK(found_requested_p, _SHR_E_PARAM, "found_requested_p");
    SHR_NULL_CHECK(found_any_in_range_p, _SHR_E_PARAM, "found_any_in_range_p");

    SHR_INVOKE_VERIFY_DNX(dnx_field_actions_fes_key_select_block_verify(unit, field_stage, actions_block));

    
    *min_position_2msb_p = -1;
    *max_position_2msb_p = -2;

    if (field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        acr_stage = DNX_FIELD_STAGE_IPMF2;
    }
    else
    {
        acr_stage = field_stage;
    }

    if (nof_msb_bits > position_of_msb_on_key_select + 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof_msb_bits (%d) larger than key select size (%d).\r\n",
                     nof_msb_bits, position_of_msb_on_key_select + 1);
    }
    if (shift < 0 || group_lsb < 0 || group_msb < 0 || nof_msb_bits < 0 || requested_msb_of_2msb_in_group < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported negative value. "
                     "shift %d group_lsb %d group_msb %d nof_msb_bits %d requested_msb_of_2msb_in_group %d.\r\n",
                     shift, group_lsb, group_msb, nof_msb_bits, requested_msb_of_2msb_in_group);
    }

    field_io_key_select_info_p = dnx_data_field.efes.key_select_properties_get(unit, acr_stage, actions_block);
    for (key_select_ndx = 0; key_select_ndx < max_nof_key_selects_per_field_io; key_select_ndx++)
    {
        if (field_io_key_select_info_p->num_bits[key_select_ndx] != 0)
        {
            position_of_msb_in_group =
                field_io_key_select_info_p->lsb[key_select_ndx] - shift +
                position_of_msb_on_key_select - field_io_key_select_info_p->num_bits_not_on_key[key_select_ndx];
            if ((position_of_msb_in_group - nof_msb_bits + 1 >= group_lsb) && (position_of_msb_in_group <= group_msb))
            {
                if (found_any == FALSE)
                {
                    *min_position_2msb_p = position_of_msb_in_group;
                    *max_position_2msb_p = position_of_msb_in_group;
                    found_any = TRUE;
                }
                else
                {
                    if ((*min_position_2msb_p) > position_of_msb_in_group)
                    {
                        *min_position_2msb_p = position_of_msb_in_group;
                    }
                    if ((*max_position_2msb_p) < position_of_msb_in_group)
                    {
                        *max_position_2msb_p = position_of_msb_in_group;
                    }
                }
                if (position_of_msb_in_group == requested_msb_of_2msb_in_group)
                {
                    if (found_specific)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Found key select (%d) twice. "
                                     "shift %d group_lsb %d group_msb %d actions_block %d "
                                     "requested_msb_of_2msb_in_group %d.\r\n",
                                     field_io_key_select_info_p->key_select[key_select_ndx],
                                     shift, group_lsb, group_msb, actions_block, requested_msb_of_2msb_in_group);
                    }
                    *action_input_select_2msb_p = field_io_key_select_info_p->key_select[key_select_ndx];
                    found_specific = TRUE;

                }
            }
        }
    }
    
    if (found_specific && (found_any == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found the requested 2msb, but did not find any 2msb.\r\n");
    }
    if (found_any && ((*min_position_2msb_p < 0) || (*max_position_2msb_p < 0)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Min/Max not written to, despite a key select being found.\r\n");
    }

    *found_requested_p = found_specific;
    *found_any_in_range_p = found_any;

exit:
    SHR_FUNC_EXIT;
}




static shr_error_e
dnx_field_actions_place_actions_verify(
    int unit,
    dnx_field_stage_e field_stage,
    unsigned int max_nof_bits_on_payload,
    dnx_field_action_in_group_info_t actions_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP])
{
    unsigned int first_index, second_index;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(actions_info, _SHR_E_PARAM, "actions_info");

    
    for (first_index = 0;
         first_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && actions_info[first_index].dnx_action != DNX_FIELD_ACTION_INVALID; first_index++)
    {
        for (second_index = 0; second_index < first_index; second_index++)
        {
            if (actions_info[first_index].dnx_action == actions_info[second_index].dnx_action)
            {
                
                SHR_ERR_EXIT(_SHR_E_PARAM, "Action type (0x%X, %s) was requested twice, indices %d and %d.\r\n",
                             actions_info[first_index].dnx_action,
                             dnx_field_dnx_action_text(unit, actions_info[first_index].dnx_action),
                             second_index, first_index);
            }
        }
    }
    if (first_index == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No actions were given.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_actions_place_actions(
    int unit,
    dnx_field_stage_e field_stage,
    unsigned int max_nof_bits_on_payload,
    dnx_field_action_in_group_info_t actions_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP])
{
    unsigned int action_index;
    unsigned int total_num_bits;
    unsigned int num_on_actions_info;
    unsigned int action_size_with_valid_bit[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    unsigned int action_size_without_valid_bit;
    unsigned int lsb_within_actions_chunk;
    unsigned int offset_within_block;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_actions_place_actions_verify
                          (unit, field_stage, max_nof_bits_on_payload, actions_info));

    
    total_num_bits = 0;
    for (action_index = 0;
         action_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && actions_info[action_index].dnx_action != DNX_FIELD_ACTION_INVALID; action_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_action_in_group_bit_size
                        (unit, field_stage, &(actions_info[action_index]), &(action_size_with_valid_bit[action_index]),
                         &action_size_without_valid_bit));
        total_num_bits += action_size_with_valid_bit[action_index];
    }
    
    num_on_actions_info = action_index;
    
    if (total_num_bits > max_nof_bits_on_payload)
    {
        
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Total number of bits on all actions (%d) is larger than the number\r\n"
                     "of bits available on selected payload type (%d).\r\n", total_num_bits, max_nof_bits_on_payload);
    }

    
    lsb_within_actions_chunk = 0;
    offset_within_block = (max_nof_bits_on_payload - total_num_bits);
    for (action_index = 0; action_index < num_on_actions_info; action_index++)
    {
        
        actions_info[action_index].lsb = lsb_within_actions_chunk + offset_within_block;
        
        lsb_within_actions_chunk += action_size_with_valid_bit[action_index];
    }
    

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_actions_calc_feses(
    int unit,
    dnx_field_stage_e field_stage,
    dbal_enum_value_field_field_io_e field_io,
    dnx_field_action_in_group_info_t actions_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP],
    int template_shift,
    dnx_field_action_attach_info_t action_attach_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP],
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP])
{
    unsigned int action_index;
    unsigned int inst_ndx;
    dnx_field_action_type_t action_type;
    unsigned int fes2msb_index;
    unsigned int required_shift;
    int found;
    int action_is_void;
    unsigned int action_size_with_valid;
    unsigned int action_size_without_valid;
    uint32 prefix;
    unsigned int prefix_size;
    
    unsigned int action_input_select;
    int action_lsb;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_info_t_init(unit, field_stage, fes_inst_info));

    inst_ndx = 0;
    for (action_index = 0;
         action_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && actions_info[action_index].dnx_action != DNX_FIELD_ACTION_INVALID; action_index++)
    {
        
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_is_void
                        (unit, field_stage, actions_info[action_index].dnx_action, &action_is_void));
        if (action_is_void)
        {
            continue;
        }
        
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_action_type
                        (unit, field_stage, actions_info[action_index].dnx_action, &action_type));
        fes_inst_info[inst_ndx].common_info.fes2msb_info[0].action_type = action_type;
        
        SHR_IF_ERR_EXIT(dnx_field_action_in_group_bit_size
                        (unit, field_stage, &(actions_info[action_index]), &action_size_with_valid,
                         &action_size_without_valid));

        
        fes_inst_info[inst_ndx].common_info.fes2msb_info[0].valid_bits =
            (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_bits_in_fes_action -
             action_size_without_valid);
        
        if (fes_inst_info[inst_ndx].common_info.fes2msb_info[0].valid_bits ==
            dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_bits_in_fes_action)
        {
            fes_inst_info[inst_ndx].common_info.fes2msb_info[0].valid_bits--;
        }
        fes_inst_info[inst_ndx].common_info.fes2msb_info[0].type = actions_info[action_index].dont_use_valid_bit;
        fes_inst_info[inst_ndx].common_info.fes2msb_info[0].polarity =
            action_attach_info[action_index].valid_bit_polarity;
        
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_prefix
                        (unit, field_stage, actions_info[action_index].dnx_action, &prefix, &prefix_size));

        fes_inst_info[inst_ndx].alloc_info.fes2msb_info[0].required_mask =
            DNX_FIELD_ACTION_PREFIX_TO_MASK(prefix, action_size_without_valid);

        
        action_lsb = template_shift + actions_info[action_index].lsb;
        SHR_IF_ERR_EXIT(dnx_field_actions_fes_key_select_get
                        (unit,
                         field_io, field_stage,
                         action_size_with_valid, action_lsb, FALSE, &action_input_select, &required_shift, &found));
        if (!found)
        {
            
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                     "\r\n"
                                     "No 'key select' was found for action 0x%x "
                                     "(Located LSB %d. size %d)\r\n",
                                     actions_info[action_index].dnx_action, action_lsb, action_size_with_valid);
        }
        fes_inst_info[inst_ndx].common_info.fes2msb_info[0].shift = required_shift;
        
        fes_inst_info[inst_ndx].write_info.key_select = action_input_select;
        LOG_DEBUG_EX(BSL_LOG_MODULE, "fes_inst_info[action %d]:%s%s%s\r\n", action_index, EMPTY, EMPTY, EMPTY);
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "==> action 0x%x, action type %d, shift %d, valid_bits %d\r\n",
                     actions_info[action_index].dnx_action,
                     fes_inst_info[inst_ndx].common_info.fes2msb_info[0].action_type,
                     fes_inst_info[inst_ndx].common_info.fes2msb_info[0].shift,
                     (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_bits_in_fes_action -
                      fes_inst_info[inst_ndx].common_info.fes2msb_info[0].valid_bits));
        LOG_DEBUG_EX(BSL_LOG_MODULE, "==> action mask 0x%08X, key select %d, type %d, polarity %d\r\n",
                     fes_inst_info[inst_ndx].alloc_info.fes2msb_info[0].required_mask,
                     fes_inst_info[inst_ndx].write_info.key_select,
                     fes_inst_info[inst_ndx].common_info.fes2msb_info[0].type,
                     fes_inst_info[inst_ndx].common_info.fes2msb_info[0].polarity);
        
        for (fes2msb_index = 1;
             fes2msb_index < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes; fes2msb_index++)
        {
            fes_inst_info[inst_ndx].common_info.fes2msb_info[fes2msb_index] =
                fes_inst_info[inst_ndx].common_info.fes2msb_info[0];
            fes_inst_info[inst_ndx].alloc_info.fes2msb_info[fes2msb_index] =
                fes_inst_info[inst_ndx].alloc_info.fes2msb_info[0];
        }
        
        fes_inst_info[inst_ndx].alloc_info.priority = action_attach_info[action_index].priority;
        
        inst_ndx++;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_actions_calc_feses_single_efes_add(
    int unit,
    dnx_field_stage_e field_stage,
    dbal_enum_value_field_field_io_e field_io,
    int action_input_select_is_forced,
    unsigned int action_input_select,
    dnx_field_actions_fes_single_add_conf_t efes_encoded_extraction[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES],
    dnx_field_action_priority_t priority,
    int template_shift,
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP])
{
    unsigned int nof_progs_per_fes = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
    dnx_field_action_type_t action_type;
    unsigned int fes2msb_index;
    unsigned int required_shift;
    int found;
    unsigned int action_size_without_valid;
    unsigned int action_size_with_valid;
    
    unsigned int action_input_select_int = action_input_select;
    int action_lsb;
    int action_is_void;
    int force_input_select;

    SHR_FUNC_INIT_VARS(unit);

    if (action_input_select_is_forced && (action_input_select == DNX_FIELD_FEM_KEY_SELECT_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Trying to force invalid key select (%d).\r\n", action_input_select);
    }
    force_input_select = action_input_select_is_forced;
    action_input_select_int = action_input_select;

    
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_info_t_init(unit, field_stage, fes_inst_info));

    for (fes2msb_index = 0; fes2msb_index < nof_progs_per_fes; fes2msb_index++)
    {
        if (efes_encoded_extraction[fes2msb_index].dnx_action == DNX_FIELD_ACTION_INVALID)
        {
            continue;
        }
        
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_is_void
                        (unit, field_stage, efes_encoded_extraction[fes2msb_index].dnx_action, &action_is_void));
        if (action_is_void)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cannot write a void action to EFES (0x%x), fes2msbcomb %d.\r\n",
                         efes_encoded_extraction[fes2msb_index].dnx_action, fes2msb_index);
        }
        
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_action_type
                        (unit, field_stage, efes_encoded_extraction[fes2msb_index].dnx_action, &action_type));
        fes_inst_info[0].common_info.fes2msb_info[fes2msb_index].action_type = action_type;

        action_size_without_valid = efes_encoded_extraction[fes2msb_index].size;
        
        fes_inst_info[0].common_info.fes2msb_info[fes2msb_index].valid_bits =
            (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_bits_in_fes_action -
             action_size_without_valid);
        
        if (fes_inst_info[0].common_info.fes2msb_info[fes2msb_index].valid_bits ==
            dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_bits_in_fes_action)
        {
            fes_inst_info[0].common_info.fes2msb_info[fes2msb_index].valid_bits--;
        }
        fes_inst_info[0].common_info.fes2msb_info[fes2msb_index].type =
            efes_encoded_extraction[fes2msb_index].dont_use_valid_bit;
        fes_inst_info[0].common_info.fes2msb_info[fes2msb_index].polarity =
            efes_encoded_extraction[fes2msb_index].valid_bit_polarity;

        fes_inst_info[0].alloc_info.fes2msb_info[fes2msb_index].required_mask =
            efes_encoded_extraction[fes2msb_index].mask;

        if (efes_encoded_extraction[fes2msb_index].dont_use_valid_bit)
        {
            action_size_with_valid = action_size_without_valid;
        }
        else
        {
            action_size_with_valid = action_size_without_valid + 1;
        }
        
        if (efes_encoded_extraction[fes2msb_index].dont_use_valid_bit)
        {
            action_lsb = template_shift + efes_encoded_extraction[fes2msb_index].lsb;
        }
        else
        {
            action_lsb = template_shift + efes_encoded_extraction[fes2msb_index].lsb - 1;
        }
        
        if (action_lsb < 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "action_lsb (%d) is negative.\r\n", action_lsb);
        }
        SHR_IF_ERR_EXIT(dnx_field_actions_fes_key_select_get
                        (unit, field_io, field_stage, action_size_with_valid, action_lsb,
                         force_input_select, &action_input_select_int, &required_shift, &found));
        if (!found)
        {
            
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "No 'key select' was found for action 0x%x "
                         "(Located LSB %d size %d 2msb combination %d)\r\n",
                         efes_encoded_extraction[fes2msb_index].dnx_action, action_lsb, action_size_with_valid,
                         fes2msb_index);
        }
        fes_inst_info[0].common_info.fes2msb_info[fes2msb_index].shift = required_shift;
        
        force_input_select = TRUE;
        fes_inst_info[0].write_info.key_select = action_input_select_int;
        LOG_DEBUG_EX(BSL_LOG_MODULE, "fes_inst_info[0] 2msb comb %d:%s%s%s\r\n", fes2msb_index, EMPTY, EMPTY, EMPTY);
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "==> action type %d, shift %d, valid_bits %d%s\r\n",
                     fes_inst_info[0].common_info.fes2msb_info[fes2msb_index].action_type,
                     fes_inst_info[0].common_info.fes2msb_info[fes2msb_index].shift,
                     (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_bits_in_fes_action -
                      fes_inst_info[0].common_info.fes2msb_info[fes2msb_index].valid_bits), EMPTY);
        LOG_DEBUG_EX(BSL_LOG_MODULE, "==> action mask 0x%08X, key select %d, type %d, polarity %d\r\n",
                     fes_inst_info[0].alloc_info.fes2msb_info[fes2msb_index].required_mask,
                     fes_inst_info[0].write_info.key_select,
                     fes_inst_info[0].common_info.fes2msb_info[fes2msb_index].type,
                     fes_inst_info[0].common_info.fes2msb_info[fes2msb_index].polarity);
        
        fes_inst_info[0].alloc_info.priority = priority;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_actions_fes_config_init(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_actions_fes_common_info_fes2msb_t * fes2msb_conf_p,
    dnx_field_fes_chosen_mask_t * chosen_mask_p,
    dnx_field_fes_pgm_id_t * fes_pgm_id_p,
    dnx_field_fes_key_select_t * key_select_p,
    dnx_field_fes_mask_t * action_mask_p)
{
    dnx_field_action_type_t action_type_invalid;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fes2msb_conf_p, _SHR_E_PARAM, "zero_fes2msb_conf_p");
    SHR_NULL_CHECK(chosen_mask_p, _SHR_E_PARAM, "chosen_mask_p");
    SHR_NULL_CHECK(fes_pgm_id_p, _SHR_E_PARAM, "fes_pgm_id_p");
    SHR_NULL_CHECK(key_select_p, _SHR_E_PARAM, "key_select_p");
    SHR_NULL_CHECK(action_mask_p, _SHR_E_PARAM, "action_mask_p");

    
    sal_memset(fes2msb_conf_p, 0x0, sizeof(*fes2msb_conf_p));
    SHR_IF_ERR_EXIT(dnx_field_map_get_invalid_action_type(unit, field_stage, &action_type_invalid));
    fes2msb_conf_p->action_type = action_type_invalid;
    fes2msb_conf_p->valid_bits = 0;
    fes2msb_conf_p->shift = 0;
    fes2msb_conf_p->type = 0;
    fes2msb_conf_p->polarity = 0;
    (*chosen_mask_p) = 0;
    (*fes_pgm_id_p) = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
    (*key_select_p) = 0;
    (*action_mask_p) = DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK;

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_actions_fes_array_write_move_single(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_algo_field_action_fes_alloc_out_t * alloc_result_p,
    unsigned int num_fes_change)
{
    unsigned int fes2msb_ndx;
    dnx_field_fes_key_select_t key_select;
    dnx_field_fes_mask_t action_mask;
    dnx_field_actions_fes_common_info_fes2msb_t fes_inst_common_info_fes2msb;
    dnx_field_fes_chosen_mask_t chosen_mask;
    dnx_field_fes_pgm_id_t fes_pgm_id;
    dnx_field_actions_fes_common_info_fes2msb_t zero_fes2msb_conf;
    dnx_field_fes_chosen_mask_t zero_chosen_mask;
    dnx_field_fes_pgm_id_t zero_fes_pgm_id;
    dnx_field_fes_key_select_t zero_key_select;
    dnx_field_fes_mask_t zero_action_mask;
    int mask_id_deleted[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];
    unsigned int mask_id_ndx;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(alloc_result_p, _SHR_E_PARAM, "alloc_result_p");

    
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_config_init
                    (unit, field_stage, &zero_fes2msb_conf, &zero_chosen_mask, &zero_fes_pgm_id, &zero_key_select,
                     &zero_action_mask));

    
    for (mask_id_ndx = 0; mask_id_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES; mask_id_ndx++)
    {
        mask_id_deleted[mask_id_ndx] = FALSE;
    }

    
    if (alloc_result_p->fes_quartet_change[num_fes_change].source_fes_id ==
        alloc_result_p->fes_quartet_change[num_fes_change].dest_fes_id)
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FES change %d moves to and from the same FES ID %d.\r\n", num_fes_change,
                     alloc_result_p->fes_quartet_change[num_fes_change].source_fes_id);

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Handling FES change %d (move), for context ID %d, field group %d. "
                 "Writing to EFES.%s\r\n", num_fes_change, context_id, fg_id, EMPTY);

    
    if (alloc_result_p->fes_quartet_change[num_fes_change].dest_is_shared == FALSE)
    {
        
        for (fes2msb_ndx = 0; fes2msb_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
             fes2msb_ndx++)
        {
            
            SHR_IF_ERR_EXIT(dnx_field_actions_fes_2nd_instruction_hw_get
                            (unit, field_stage,
                             alloc_result_p->fes_quartet_change[num_fes_change].source_fes_id,
                             alloc_result_p->fes_quartet_change[num_fes_change].source_fes_pgm_id,
                             fes2msb_ndx, &fes_inst_common_info_fes2msb, &chosen_mask));

            
            if (alloc_result_p->fes_quartet_change[num_fes_change].dest_fes_mask_id[fes2msb_ndx] !=
                DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
            {
                
                SHR_IF_ERR_EXIT(dnx_field_actions_fes_mask_hw_get
                                (unit, field_stage,
                                 alloc_result_p->fes_quartet_change[num_fes_change].source_fes_id,
                                 chosen_mask, &action_mask));
                
                SHR_IF_ERR_EXIT(dnx_field_actions_fes_mask_hw_set
                                (unit, field_stage,
                                 alloc_result_p->fes_quartet_change[num_fes_change].dest_fes_id,
                                 alloc_result_p->fes_quartet_change[num_fes_change].dest_fes_mask_id[fes2msb_ndx],
                                 action_mask));
            }

            
            SHR_IF_ERR_EXIT(dnx_field_actions_fes_2nd_instruction_hw_set
                            (unit, field_stage,
                             alloc_result_p->fes_quartet_change[num_fes_change].dest_fes_id,
                             alloc_result_p->fes_quartet_change[num_fes_change].dest_fes_pgm_id,
                             fes2msb_ndx, &fes_inst_common_info_fes2msb,
                             alloc_result_p->fes_quartet_change[num_fes_change].dest_fes_mask_id[fes2msb_ndx]));
        }
    }

    
    
    SHR_IF_ERR_EXIT(dnx_field_fes_program_fes_instruction_hw_get
                    (unit, field_stage, context_id,
                     alloc_result_p->fes_quartet_change[num_fes_change].source_fes_id, &fes_pgm_id, &key_select));
    
    SHR_IF_ERR_EXIT(dnx_field_fes_program_fes_instruction_hw_set
                    (unit, field_stage, context_id,
                     alloc_result_p->fes_quartet_change[num_fes_change].dest_fes_id,
                     alloc_result_p->fes_quartet_change[num_fes_change].dest_fes_pgm_id, key_select));
    
    SHR_IF_ERR_EXIT(dnx_field_fes_program_fes_instruction_hw_set
                    (unit, field_stage, context_id,
                     alloc_result_p->fes_quartet_change[num_fes_change].source_fes_id,
                     zero_fes_pgm_id, zero_key_select));

    
    if (alloc_result_p->fes_quartet_change[num_fes_change].source_is_shared == FALSE)
    {
        for (fes2msb_ndx = 0; fes2msb_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
             fes2msb_ndx++)
        {
            SHR_IF_ERR_EXIT(dnx_field_actions_fes_2nd_instruction_hw_get
                            (unit, field_stage,
                             alloc_result_p->fes_quartet_change[num_fes_change].source_fes_id,
                             alloc_result_p->fes_quartet_change[num_fes_change].source_fes_pgm_id,
                             fes2msb_ndx, &fes_inst_common_info_fes2msb, &chosen_mask));

            SHR_IF_ERR_EXIT(dnx_field_actions_fes_2nd_instruction_hw_set
                            (unit, field_stage,
                             alloc_result_p->fes_quartet_change[num_fes_change].source_fes_id,
                             alloc_result_p->fes_quartet_change[num_fes_change].source_fes_pgm_id,
                             fes2msb_ndx, &zero_fes2msb_conf, zero_chosen_mask));

            
            if (chosen_mask != DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID && mask_id_deleted[chosen_mask] == FALSE)
            {
                SHR_IF_ERR_EXIT(dnx_field_actions_fes_mask_hw_set
                                (unit, field_stage,
                                 alloc_result_p->fes_quartet_change[num_fes_change].source_fes_id,
                                 chosen_mask, zero_action_mask));
            }

        }

        
        for (fes2msb_ndx = 0; fes2msb_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
             fes2msb_ndx++)
        {

        }
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "End of handling FES change %d (move), for context ID %d, field group %d. "
                 "Writing to EFES.%s\r\n", num_fes_change, context_id, fg_id, EMPTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_actions_fes_array_write_new_single(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_actions_fes_info_t * fes_inst_info_p,
    dnx_algo_field_action_fes_alloc_out_t * alloc_result_p,
    unsigned int num_fes_change)
{
    unsigned int fes2msb_ndx;
    dnx_field_action_type_t action_type_invalid;
    unsigned int place_in_alloc;
    int mask_id_written[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];
    dnx_field_fes_mask_id_t mask_id_ndx;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(alloc_result_p, _SHR_E_PARAM, "alloc_result_p");

    
    for (mask_id_ndx = 0; mask_id_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES; mask_id_ndx++)
    {
        mask_id_written[mask_id_ndx] = FALSE;
    }

    
    SHR_IF_ERR_EXIT(dnx_field_map_get_invalid_action_type(unit, field_stage, &action_type_invalid));

    place_in_alloc = alloc_result_p->fes_quartet_change[num_fes_change].place_in_alloc;

    
    if (place_in_alloc >= dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "place_in_alloc (%d) cannot exceed the number of FES IDs (%d) for the stage %s.\r\n",
                     place_in_alloc, dnx_data_field.stage.stage_info_get(unit,
                                                                         field_stage)->nof_fes_instruction_per_context,
                     dnx_field_stage_text(unit, field_stage));
    }
    if (place_in_alloc >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "place_in_alloc (%d) cannot exceed the number of FES IDs (%d) for field group.\r\n",
                     place_in_alloc, DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP);
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Handling FES change %d (new fes_quatert %d) for context ID %d field group Id %d. "
                 "Writing to EFES.\r\n", num_fes_change, place_in_alloc, context_id, fg_id);
    
    if (alloc_result_p->fes_quartet_change[num_fes_change].dest_is_shared == FALSE)
    {
        
        for (fes2msb_ndx = 0; fes2msb_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
             fes2msb_ndx++)
        {
            if (fes_inst_info_p->common_info.fes2msb_info[fes2msb_ndx].action_type != action_type_invalid)
            {
                
                if (alloc_result_p->fes_quartet_change[num_fes_change].dest_fes_mask_id[fes2msb_ndx] !=
                    DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
                {
                    
                    if (fes_inst_info_p->alloc_info.fes2msb_info[fes2msb_ndx].required_mask ==
                        DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Allocation rule that all zero masks are allocated the "
                                     "zero mask ID has been broken.\r\n");
                    }
                    if (mask_id_written
                        [alloc_result_p->fes_quartet_change[num_fes_change].dest_fes_mask_id[fes2msb_ndx]] == FALSE)
                    {
                        SHR_IF_ERR_EXIT(dnx_field_actions_fes_mask_hw_set
                                        (unit, field_stage,
                                         alloc_result_p->fes_quartet_change[num_fes_change].dest_fes_id,
                                         alloc_result_p->
                                         fes_quartet_change[num_fes_change].dest_fes_mask_id[fes2msb_ndx],
                                         fes_inst_info_p->alloc_info.fes2msb_info[fes2msb_ndx].required_mask));
                        mask_id_written[alloc_result_p->
                                        fes_quartet_change[num_fes_change].dest_fes_mask_id[fes2msb_ndx]] = TRUE;
                    }
                }
                else
                {
                    
                    if (fes_inst_info_p->alloc_info.fes2msb_info[fes2msb_ndx].required_mask !=
                        DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "A mask that is not all zeros (%u) wasn't allocated a mask ID.\r\n",
                                     fes_inst_info_p->alloc_info.fes2msb_info[fes2msb_ndx].required_mask);
                    }
                }
                
                SHR_IF_ERR_EXIT(dnx_field_actions_fes_2nd_instruction_hw_set
                                (unit, field_stage,
                                 alloc_result_p->fes_quartet_change[num_fes_change].dest_fes_id,
                                 alloc_result_p->fes_quartet_change[num_fes_change].dest_fes_pgm_id,
                                 fes2msb_ndx,
                                 &(fes_inst_info_p->common_info.fes2msb_info[fes2msb_ndx]),
                                 alloc_result_p->fes_quartet_change[num_fes_change].dest_fes_mask_id[fes2msb_ndx]));
            }
        }
    }
    
    SHR_IF_ERR_EXIT(dnx_field_fes_program_fes_instruction_hw_set
                    (unit, field_stage, context_id,
                     alloc_result_p->fes_quartet_change[num_fes_change].dest_fes_id,
                     alloc_result_p->fes_quartet_change[num_fes_change].dest_fes_pgm_id,
                     fes_inst_info_p->write_info.key_select));
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "End of handling FES change %d (new fes_quatert %d) for context ID %d field group Id %d. "
                 "Writing to EFES.\r\n", num_fes_change, place_in_alloc, context_id, fg_id);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_actions_fes_array_write(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP],
    dnx_algo_field_action_fes_alloc_out_t * alloc_result_p)
{
    unsigned int fes_change_ndx;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(alloc_result_p, _SHR_E_PARAM, "alloc_result_p");

    
    if (alloc_result_p->nof_fes_quartet_changes >
        (sizeof(alloc_result_p->fes_quartet_change) / sizeof(alloc_result_p->fes_quartet_change[0])))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of FES changes (movements+new FES quartets %d) exceeds array limit "
                     "(%u) in for field group %d context ID %d.\r\n",
                     alloc_result_p->nof_fes_quartet_changes,
                     (unsigned
                      int) ((sizeof(alloc_result_p->fes_quartet_change) /
                             sizeof(alloc_result_p->fes_quartet_change[0]))), fg_id, context_id);
    }

    
    for (fes_change_ndx = 0; fes_change_ndx < alloc_result_p->nof_fes_quartet_changes; fes_change_ndx++)
    {
        
        if (alloc_result_p->fes_quartet_change[fes_change_ndx].place_in_alloc == DNX_ALGO_FIELD_ACTION_MOVEMENT)
        {
            SHR_IF_ERR_EXIT(dnx_field_actions_fes_array_write_move_single
                            (unit, field_stage, fg_id, context_id, alloc_result_p, fes_change_ndx));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_field_actions_fes_array_write_new_single
                            (unit, field_stage, fg_id, context_id,
                             &fes_inst_info[alloc_result_p->fes_quartet_change[fes_change_ndx].place_in_alloc],
                             alloc_result_p, fes_change_ndx));
        }

    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_actions_fes_inst_count(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP],
    unsigned int *nof_fes_quartets_p)
{
    unsigned int fes_quartet_ndx;
    unsigned int fes2msb_ndx;
    int found_an_all_invalid_element;
    dnx_field_action_type_t action_type_invalid;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fes_inst_info, _SHR_E_PARAM, "fes_inst_info");
    SHR_NULL_CHECK(nof_fes_quartets_p, _SHR_E_PARAM, "nof_fes_quartets_p");

    
    SHR_IF_ERR_EXIT(dnx_field_map_get_invalid_action_type(unit, field_stage, &action_type_invalid));

    
    found_an_all_invalid_element = FALSE;
    for (fes_quartet_ndx = 0;
         fes_quartet_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && found_an_all_invalid_element == FALSE; fes_quartet_ndx++)
    {
        found_an_all_invalid_element = TRUE;
        for (fes2msb_ndx = 0; fes2msb_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
             fes2msb_ndx++)
        {
            if (fes_inst_info[fes_quartet_ndx].common_info.fes2msb_info[fes2msb_ndx].action_type != action_type_invalid)
            {
                found_an_all_invalid_element = FALSE;
                break;
            }
        }
    }

    if (found_an_all_invalid_element)
    {
        (*nof_fes_quartets_p) = fes_quartet_ndx - 1;
    }
    else
    {
        (*nof_fes_quartets_p) = fes_quartet_ndx;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_actions_fes_place_masks(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP],
    dnx_algo_field_action_fes_alloc_in_t * fes_fg_in_p)
{
    unsigned int fes_quartet_ndx;
    unsigned int fes2msb_1_ndx;
    unsigned int fes2msb_2_ndx;
    int mask_distict;
    dnx_field_fes_mask_id_t next_mask_id;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fes_inst_info, _SHR_E_PARAM, "fes_inst_info");
    SHR_NULL_CHECK(fes_fg_in_p, _SHR_E_PARAM, "fes_fg_in_p");

    
    for (fes_quartet_ndx = 0; fes_quartet_ndx < fes_fg_in_p->nof_fes_quartets; fes_quartet_ndx++)
    {
        next_mask_id = 0;
        if (next_mask_id == DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            next_mask_id++;
        }
        
        for (fes2msb_1_ndx = 0;
             fes2msb_1_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes; fes2msb_1_ndx++)
        {
            
            if (fes_inst_info[fes_quartet_ndx].alloc_info.fes2msb_info[fes2msb_1_ndx].required_mask !=
                DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK)
            {
                
                mask_distict = TRUE;
                for (fes2msb_2_ndx = 0; fes2msb_2_ndx < fes2msb_1_ndx; fes2msb_2_ndx++)
                {
                    if (fes_inst_info[fes_quartet_ndx].alloc_info.fes2msb_info[fes2msb_1_ndx].required_mask ==
                        fes_inst_info[fes_quartet_ndx].alloc_info.fes2msb_info[fes2msb_2_ndx].required_mask)
                    {
                        mask_distict = FALSE;
                        break;
                    }
                }
                if (mask_distict)
                {
                    
                    if (next_mask_id >= dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "In new FES quartet %d in field group ID %d context ID %d we attempt to allocate "
                                     "more than %d distinct non zero masks, which is the maximum.\r\n",
                                     fes_quartet_ndx, fg_id, context_id, dnx_data_field.stage.stage_info_get(unit,
                                                                                                             field_stage)->nof_masks_per_fes
                                     - 1);
                    }
                    fes_fg_in_p->fes_quartet[fes_quartet_ndx].fes_mask_id[fes2msb_1_ndx] = next_mask_id;
                    
                    next_mask_id++;
                    if (next_mask_id == DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
                    {
                        next_mask_id++;
                    }
                }
                else
                {
                    
                    fes_fg_in_p->fes_quartet[fes_quartet_ndx].fes_mask_id[fes2msb_1_ndx] =
                        fes_fg_in_p->fes_quartet[fes_quartet_ndx].fes_mask_id[fes2msb_2_ndx];
                }

            }
            else
            {
                
                fes_fg_in_p->fes_quartet[fes_quartet_ndx].fes_mask_id[fes2msb_1_ndx] =
                    DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_actions_convert_fes_instructions_to_alloc(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP],
    uint8 *polarity_zero_exists_p,
    dnx_algo_field_action_fes_alloc_in_t * fes_fg_in_p)
{
    unsigned int fes_quartet_ndx;
    dnx_field_fes_id_t fes_id_ndx;
    unsigned int fes_2msb_ndx;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fes_inst_info, _SHR_E_PARAM, "fes_inst_info");
    SHR_NULL_CHECK(fes_fg_in_p, _SHR_E_PARAM, "fes_fg_in_p");

    *polarity_zero_exists_p = FALSE;

    
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_inst_count
                    (unit, field_stage, fes_inst_info, &(fes_fg_in_p->nof_fes_quartets)));
    
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_place_masks
                    (unit, field_stage, fg_id, context_id, fes_inst_info, fes_fg_in_p));

    
    for (fes_quartet_ndx = 0; fes_quartet_ndx < fes_fg_in_p->nof_fes_quartets; fes_quartet_ndx++)
    {
        fes_fg_in_p->fes_quartet[fes_quartet_ndx].priority = fes_inst_info[fes_quartet_ndx].alloc_info.priority;
    }

    
    for (fes_id_ndx = 0; fes_id_ndx < fes_fg_in_p->nof_fes_quartets; fes_id_ndx++)
    {
        for (fes_2msb_ndx = 0; fes_2msb_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
             fes_2msb_ndx++)
        {
            fes_fg_in_p->fes_quartet[fes_id_ndx].fes_action_type[fes_2msb_ndx] =
                fes_inst_info[fes_id_ndx].common_info.fes2msb_info[fes_2msb_ndx].action_type;
            if (fes_inst_info[fes_id_ndx].common_info.fes2msb_info[fes_2msb_ndx].polarity ==
                DNX_FIELD_ACTION_FES_VALID_BIT_POLARITY_ZERO)
            {
                *polarity_zero_exists_p = TRUE;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_actions_fes_info_t_init(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP])
{
    
    unsigned int fes_quartet_ndx;
    
    unsigned int fes2msb_ndx;
    dnx_field_action_type_t action_type_invalid;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fes_inst_info, _SHR_E_PARAM, "fes_inst_info");

    
    SHR_IF_ERR_EXIT(dnx_field_map_get_invalid_action_type(unit, field_stage, &action_type_invalid));

    
    
    for (fes_quartet_ndx = 0; fes_quartet_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP;
         fes_quartet_ndx++)
    {
        
        sal_memset(&(fes_inst_info[fes_quartet_ndx]), 0x0, sizeof(fes_inst_info[0]));
        for (fes2msb_ndx = 0; fes2msb_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES; fes2msb_ndx++)
        {
            
            fes_inst_info[fes_quartet_ndx].common_info.fes2msb_info[fes2msb_ndx].action_type = action_type_invalid;
            
            fes_inst_info[fes_quartet_ndx].alloc_info.fes2msb_info[fes2msb_ndx].required_mask =
                DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK;
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_actions_fes_context_hw_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_fes_pgm_id_t fes_pgm_id_allocated[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT])
{
    unsigned int fes_id_ndx;
    uint32 entry_handle_id;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fes_pgm_id_allocated, _SHR_E_PARAM, "fes_pgm_id_allocated");

    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
        {
            table_id = DBAL_TABLE_FIELD_PMF_A_SELECT_KEY_N_FES;
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            table_id = DBAL_TABLE_FIELD_PMF_B_SELECT_KEY_N_FES;
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            table_id = DBAL_TABLE_FIELD_E_PMF_SELECT_KEY_N_FES;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage for function \"%s\" (%d).\r\n",
                         dnx_field_stage_text(unit, field_stage), field_stage);
        }
    }

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    
    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_ID, fes_id_ndx);
        
        dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_PGM_ID, INST_SINGLE,
                                  &(fes_pgm_id_allocated[fes_id_ndx]));
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    }
    
    for (; fes_id_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        fes_pgm_id_allocated[fes_id_ndx] = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_actions_fes_alloc_current_state_read(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_algo_field_action_fes_alloc_state_in_t * current_state_p)
{
    dnx_field_fes_id_t fes_id_ndx;
    dnx_field_context_t context_id_ndx;
    unsigned int fes2msb_ndx;
    dnx_field_fes_pgm_id_t fes_pgm_id_1;
    dnx_field_fes_pgm_id_t fes_pgm_id_2;
    dnx_field_fes_key_select_t key_select;
    dnx_field_actions_fes_common_info_fes2msb_t fes_inst_common_info_fes2msb;
    dnx_field_action_type_t fes_action_type_invalid;
    int fes_mask_used[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];
    dnx_field_fes_mask_id_t fes_mask_id_ndx;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_p, _SHR_E_PARAM, "current_state_p");

    

    SHR_IF_ERR_EXIT(dnx_field_map_get_invalid_action_type(unit, field_stage, &fes_action_type_invalid));

    sal_memset(current_state_p, 0x0, sizeof(*current_state_p));

    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_fes_program_fes_instruction_hw_get
                        (unit, field_stage, context_id, fes_id_ndx, &fes_pgm_id_1, &key_select));
        current_state_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id = fes_pgm_id_1;
        if (fes_pgm_id_1 != DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
        {
            for (fes2msb_ndx = 0;
                 fes2msb_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes; fes2msb_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_field_actions_fes_2nd_instruction_hw_get
                                (unit, field_stage, fes_id_ndx, fes_pgm_id_1, fes2msb_ndx,
                                 &fes_inst_common_info_fes2msb,
                                 &(current_state_p->context_state.fes_id_info[fes_id_ndx].fes_mask_id[fes2msb_ndx])));
                current_state_p->context_state.fes_id_info[fes_id_ndx].fes_action_type[fes2msb_ndx] =
                    fes_inst_common_info_fes2msb.action_type;
            }
            
            for (; fes2msb_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES; fes2msb_ndx++)
            {
                current_state_p->context_state.fes_id_info[fes_id_ndx].fes_mask_id[fes2msb_ndx] =
                    DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
                current_state_p->context_state.fes_id_info[fes_id_ndx].fes_action_type[fes2msb_ndx] =
                    fes_action_type_invalid;
            }
            current_state_p->context_state.fes_id_info[fes_id_ndx].is_shared = FALSE;
            for (context_id_ndx = 0;
                 context_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_contexts;
                 context_id_ndx++)
            {
                if (context_id_ndx != context_id)
                {
                    SHR_IF_ERR_EXIT(dnx_field_fes_program_fes_instruction_hw_get
                                    (unit, field_stage, context_id_ndx, fes_id_ndx, &fes_pgm_id_2, &key_select));
                    if (fes_pgm_id_1 == fes_pgm_id_2)
                    {
                        current_state_p->context_state.fes_id_info[fes_id_ndx].is_shared = TRUE;
                        break;
                    }
                }
            }
        }
    }

    
    for (; fes_id_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        current_state_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
    }

    
    
    for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        current_state_p->fes_state.fes_id_info[fes_id_ndx].nof_fes_quartets = 0;
        current_state_p->fes_state.fes_id_info[fes_id_ndx].nof_masks = 0;
    }
    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        for (fes_mask_id_ndx = 0;
             fes_mask_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes;
             fes_mask_id_ndx++)
        {
            fes_mask_used[fes_mask_id_ndx] = FALSE;
        }
        for (context_id_ndx = 0; context_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_contexts;
             context_id_ndx++)
        {
            SHR_IF_ERR_EXIT(dnx_field_fes_program_fes_instruction_hw_get
                            (unit, field_stage, context_id_ndx, fes_id_ndx, &fes_pgm_id_1, &key_select));
            if (fes_pgm_id_1 != DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
            {
                
                (current_state_p->fes_state.fes_id_info[fes_id_ndx].nof_fes_quartets)++;
                for (fes2msb_ndx = 0;
                     fes2msb_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
                     fes2msb_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_field_actions_fes_2nd_instruction_hw_get
                                    (unit, field_stage, fes_id_ndx, fes_pgm_id_1, fes2msb_ndx,
                                     &fes_inst_common_info_fes2msb, &fes_mask_id_ndx));
                    fes_mask_used[fes_mask_id_ndx] = TRUE;
                }
            }
        }
        for (fes_mask_id_ndx = 0;
             fes_mask_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes;
             fes_mask_id_ndx++)
        {
            if (fes_mask_id_ndx != DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID && fes_mask_used[fes_mask_id_ndx])
            {
                
                (current_state_p->fes_state.fes_id_info[fes_id_ndx].nof_masks)++;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_actions_fes_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    int is_nonshareable,
    int is_post_attach,
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP])
{
    dnx_algo_field_action_fes_alloc_state_in_t *current_state_p = NULL;
    dnx_algo_field_action_fes_alloc_in_t fes_fg_in;
    dnx_algo_field_action_fes_alloc_out_t *alloc_result_p = NULL;
    int allow_fes_pgm_sharing;
    uint8 polarity_zero_exists;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fes_inst_info, _SHR_E_PARAM, "fes_inst_info");

    
    SHR_IF_ERR_EXIT(dnx_field_actions_convert_fes_instructions_to_alloc
                    (unit, field_stage, fg_id, context_id, fes_inst_info, &polarity_zero_exists, &fes_fg_in));

    
    if (fes_fg_in.nof_fes_quartets > 0)
    {
        
        SHR_ALLOC_SET_ZERO(current_state_p, sizeof(*current_state_p), "current_state_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
        
        SHR_ALLOC_SET_ZERO(alloc_result_p, sizeof(*alloc_result_p), "alloc_result_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

        
        SHR_IF_ERR_EXIT(dnx_field_actions_fes_alloc_current_state_read(unit, field_stage, context_id, current_state_p));

        
        if (is_nonshareable || polarity_zero_exists)
        {
            allow_fes_pgm_sharing = FALSE;
        }
        else
        {
            allow_fes_pgm_sharing = TRUE;
        }

        
        SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                        (unit, field_stage, fg_id, context_id, current_state_p, &fes_fg_in, allow_fes_pgm_sharing,
                         is_post_attach, alloc_result_p));
        
        SHR_IF_ERR_EXIT(dnx_field_actions_fes_array_write
                        (unit, field_stage, fg_id, context_id, fes_inst_info, alloc_result_p));
    }

exit:
    SHR_FREE(current_state_p);
    SHR_FREE(alloc_result_p);
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_actions_fes_ace_set_verify(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP],
    unsigned int nof_fes_quartets)
{
    unsigned int fes_quartet_ndx;
    unsigned int fes2msb_ndx;
    unsigned int nof_fes2msb;
    unsigned int action_type_ndx;
    int found_an_all_invalid_element;
    dnx_field_action_type_t action_type_invalid;
    dnx_field_action_type_t invalidate_next_action_type;
    uint8 *action_found_p = NULL;
    unsigned int nof_fes_quartets_found;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fes_inst_info, _SHR_E_PARAM, "fes_inst_info");
    SHR_ALLOC(action_found_p, sizeof(*action_found_p) * DNX_FIELD_ACTION_ID_NOF(DNX_FIELD_STAGE_ACE), "action_found_p",
              "%s%s%s", EMPTY, EMPTY, EMPTY);

    
    for (action_type_ndx = 0; action_type_ndx < DNX_FIELD_ACTION_ID_NOF(DNX_FIELD_STAGE_ACE); action_type_ndx++)
    {
        action_found_p[action_type_ndx] = FALSE;
    }

    
    SHR_IF_ERR_EXIT(dnx_field_map_get_invalid_action_type(unit, DNX_FIELD_STAGE_ACE, &action_type_invalid));
    
    SHR_IF_ERR_EXIT(dnx_field_map_get_invalidate_next_action_type
                    (unit, DNX_FIELD_STAGE_ACE, &invalidate_next_action_type));

    
    found_an_all_invalid_element = FALSE;
    nof_fes2msb = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_ACE)->nof_prog_per_fes;
    for (fes_quartet_ndx = 0;
         fes_quartet_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && found_an_all_invalid_element == FALSE; fes_quartet_ndx++)
    {
        found_an_all_invalid_element = TRUE;
        for (fes2msb_ndx = 0; fes2msb_ndx < nof_fes2msb; fes2msb_ndx++)
        {
            if (fes_inst_info[fes_quartet_ndx].common_info.fes2msb_info[fes2msb_ndx].action_type ==
                invalidate_next_action_type)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "ACE format does not support invalidate next action. "
                             "ACE ID %d, quartet %d fes2msb combination %d.\r\n", ace_id, fes_quartet_ndx, fes2msb_ndx);
            }
            if (fes_inst_info[fes_quartet_ndx].common_info.fes2msb_info[fes2msb_ndx].action_type >=
                DNX_FIELD_ACTION_ID_NOF(DNX_FIELD_STAGE_ACE))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Action type (%d) exceeds range for ACE actions (%d). "
                             "ACE ID %d, quartet %d fes2msb combination %d.\r\n",
                             fes_inst_info[fes_quartet_ndx].common_info.fes2msb_info[fes2msb_ndx].action_type,
                             DNX_FIELD_ACTION_ID_NOF(DNX_FIELD_STAGE_ACE) - 1, ace_id, fes_quartet_ndx, fes2msb_ndx);
            }
            else if (action_found_p[fes_inst_info[fes_quartet_ndx].common_info.fes2msb_info[fes2msb_ndx].action_type])
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Action type (%d) appears twice. "
                             "ACE ID %d, quartet %d fes2msb combination %d.\r\n",
                             fes_inst_info[fes_quartet_ndx].common_info.fes2msb_info[fes2msb_ndx].action_type,
                             ace_id, fes_quartet_ndx, fes2msb_ndx);
            }
            else
            {
                action_found_p[fes_inst_info[fes_quartet_ndx].common_info.fes2msb_info[fes2msb_ndx].action_type] = TRUE;
            }
            if (fes_inst_info[fes_quartet_ndx].common_info.fes2msb_info[fes2msb_ndx].action_type != action_type_invalid)
            {
                found_an_all_invalid_element = FALSE;
            }
        }
    }

    
    if (found_an_all_invalid_element)
    {
        nof_fes_quartets_found = fes_quartet_ndx - 1;
    }
    else
    {
        nof_fes_quartets_found = fes_quartet_ndx;
    }
    if (nof_fes_quartets_found != nof_fes_quartets)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of actions found by verify function is %d, while the number of actions "
                     "found by counting function is %d.\r\n", nof_fes_quartets_found, nof_fes_quartets);
    }

    if (nof_fes_quartets >
        dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_ACE)->nof_fes_instruction_per_context)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of actions in ACE format (%d) excceeds maximum (%d).\r\n",
                     nof_fes_quartets,
                     dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_ACE)->nof_fes_instruction_per_context);
    }

exit:
    SHR_FREE(action_found_p);
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_actions_fes_ace_set(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP])
{
    unsigned int fes_inst_ndx;
    unsigned int fes_id_ndx;
    unsigned int fes_mask_ndx;
    dnx_field_fes_mask_t action_mask;
    dnx_algo_field_action_ace_fes_alloc_state_in_t current_state_in;
    dnx_algo_field_action_ace_fes_alloc_in_t fes_fg_in;
    dnx_algo_field_action_ace_fes_alloc_out_t alloc_result;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_inst_count
                    (unit, DNX_FIELD_STAGE_ACE, fes_inst_info, &(fes_fg_in.nof_actions)));

    
    SHR_INVOKE_VERIFY_DNX(dnx_field_actions_fes_ace_set_verify(unit, ace_id, fes_inst_info, fes_fg_in.nof_actions));

    
    for (fes_inst_ndx = 0; fes_inst_ndx < fes_fg_in.nof_actions; fes_inst_ndx++)
    {
        fes_fg_in.masks[fes_inst_ndx] = fes_inst_info[fes_inst_ndx].alloc_info.fes2msb_info[0].required_mask;
    }
    
    for (fes_mask_ndx = 0; fes_mask_ndx < DNX_DATA_MAX_FIELD_ACE_NOF_MASKS_PER_FES; fes_mask_ndx++)
    {
        for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
        {
            
            SHR_IF_ERR_EXIT(dnx_field_actions_ace_fes_mask_hw_get(unit, fes_id_ndx, fes_mask_ndx, &action_mask));
            current_state_in.masks[fes_id_ndx][fes_mask_ndx] = action_mask;
        }
    }

    
    SHR_IF_ERR_EXIT(dnx_algo_field_action_ace_fes_allocate(unit, ace_id, &current_state_in, &fes_fg_in, &alloc_result));

    
    for (fes_inst_ndx = 0; fes_inst_ndx < alloc_result.nof_actions; fes_inst_ndx++)
    {
        if (alloc_result.mask_id[fes_inst_ndx] != DNX_ALGO_FIELD_ZERO_FES_ACE_ACTION_MASK_ID)
        {
            SHR_IF_ERR_EXIT(dnx_field_actions_ace_fes_mask_hw_set
                            (unit, alloc_result.fes_id[fes_inst_ndx], alloc_result.mask_id[fes_inst_ndx],
                             fes_fg_in.masks[fes_inst_ndx]));
        }
    }

    
    for (fes_inst_ndx = 0; fes_inst_ndx < alloc_result.nof_actions; fes_inst_ndx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_actions_ace_fes_program_table_hw_set
                        (unit, alloc_result.fes_id[fes_inst_ndx], ace_id,
                         &(fes_inst_info[fes_inst_ndx].common_info.fes2msb_info[0]),
                         alloc_result.mask_id[fes_inst_ndx]));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_actions_fes_mask_context_hw_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_fes_pgm_id_t fes_pgm_id_allocated[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    uint8
    mask_id_used[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT]
    [DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES])
{
    unsigned int fes_id_ndx;
    unsigned int fes2msb_id_ndx;
    unsigned int mask_ndx;
    dnx_field_fes_chosen_mask_t chosen_mask;
    uint32 entry_handle_id;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fes_pgm_id_allocated, _SHR_E_PARAM, "fes_pgm_id_allocated");

    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
        {
            table_id = DBAL_TABLE_FIELD_PMF_A_FES_2ND_INSTRUCTION;
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            table_id = DBAL_TABLE_FIELD_PMF_B_FES_2ND_INSTRUCTION;
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            table_id = DBAL_TABLE_FIELD_E_PMF_FES_2ND_INSTRUCTION;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage for function \"%s\" (%d).\r\n",
                         dnx_field_stage_text(unit, field_stage), field_stage);
        }
    }

    
    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        for (mask_ndx = 0; mask_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes;
             mask_ndx++)
        {
            mask_id_used[fes_id_ndx][mask_ndx] = FALSE;
        }
    }
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    
    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        for (fes2msb_id_ndx = 0;
             fes2msb_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
             fes2msb_id_ndx++)
        {
            
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_ID, fes_id_ndx);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_PGM_ID,
                                       fes_pgm_id_allocated[fes_id_ndx]);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_FES_DATA_2MSB, fes2msb_id_ndx);
            
            dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_FIELD_FES_CHOSEN_MASK, INST_SINGLE,
                                      &chosen_mask);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
            
            mask_id_used[fes_id_ndx][chosen_mask] = TRUE;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_actions_fes_pgm_mask_ref_count_hw_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_fes_pgm_id_t fes_pgm_id_allocated[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    uint8
    mask_nof_refs[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT]
    [DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES],
    uint8 fes_quartet_nof_refs[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT])
{
    unsigned int context_ndx;
    unsigned int fes_id_ndx;
    unsigned int fes2msb_id_ndx;
    unsigned int mask_ndx;
    dnx_field_fes_pgm_id_t fes_pgm_id;
    int mask_used[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];
    uint32 entry_handle_id_fes_pgm_id;
    uint32 entry_handle_id_2nd_inst;
    dnx_field_fes_chosen_mask_t chosen_mask;
    dbal_tables_e dbal_table_select_key_n_fes;
    dbal_tables_e dbal_table_2nd_instruction;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fes_pgm_id_allocated, _SHR_E_PARAM, "fes_pgm_id_allocated");

    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
        {
            dbal_table_select_key_n_fes = DBAL_TABLE_FIELD_PMF_A_SELECT_KEY_N_FES;
            dbal_table_2nd_instruction = DBAL_TABLE_FIELD_PMF_A_FES_2ND_INSTRUCTION;
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            dbal_table_select_key_n_fes = DBAL_TABLE_FIELD_PMF_B_SELECT_KEY_N_FES;
            dbal_table_2nd_instruction = DBAL_TABLE_FIELD_PMF_B_FES_2ND_INSTRUCTION;
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            dbal_table_select_key_n_fes = DBAL_TABLE_FIELD_E_PMF_SELECT_KEY_N_FES;
            dbal_table_2nd_instruction = DBAL_TABLE_FIELD_E_PMF_FES_2ND_INSTRUCTION;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage for function \"%s\" (%d).\r\n",
                         dnx_field_stage_text(unit, field_stage), field_stage);
        }
    }

    
    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        if (fes_pgm_id_allocated[fes_id_ndx] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
        {
            
            fes_quartet_nof_refs[fes_id_ndx] = 1;
        }

        else
        {
            
            fes_quartet_nof_refs[fes_id_ndx] = 0;
        }
        for (mask_ndx = 0; mask_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes;
             mask_ndx++)
        {
            if (mask_nof_refs[fes_id_ndx][mask_ndx])
            {
                
                mask_nof_refs[fes_id_ndx][mask_ndx] = 1;
            }
            else
            {
                
                mask_nof_refs[fes_id_ndx][mask_ndx] = 0;
            }
        }
    }

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_select_key_n_fes, &entry_handle_id_fes_pgm_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_2nd_instruction, &entry_handle_id_2nd_inst));
    
    for (context_ndx = 0; context_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_contexts;
         context_ndx++)
    {
        
        if (context_ndx == context_id)
        {
            continue;
        }
        
        for (fes_id_ndx = 0;
             fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
             fes_id_ndx++)
        {
            
            if (fes_pgm_id_allocated[fes_id_ndx] == DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
            {
                continue;
            }
            
            dbal_entry_key_field32_set(unit, entry_handle_id_fes_pgm_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_ndx);
            dbal_entry_key_field32_set(unit, entry_handle_id_fes_pgm_id, DBAL_FIELD_FIELD_FES_ID, fes_id_ndx);
            
            dbal_value_field8_request(unit, entry_handle_id_fes_pgm_id, DBAL_FIELD_FIELD_FES_PGM_ID, INST_SINGLE,
                                      &fes_pgm_id);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_fes_pgm_id, DBAL_COMMIT));
            
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_select_key_n_fes, entry_handle_id_fes_pgm_id));
            
            if (fes_pgm_id == fes_pgm_id_allocated[fes_id_ndx])
            {
                
                (fes_quartet_nof_refs[fes_id_ndx])++;
                
                for (mask_ndx = 0; mask_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes;
                     mask_ndx++)
                {
                    mask_used[mask_ndx] = FALSE;
                }
                
                for (fes2msb_id_ndx = 0;
                     fes2msb_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
                     fes2msb_id_ndx++)
                {
                    
                    dbal_entry_key_field32_set(unit, entry_handle_id_2nd_inst, DBAL_FIELD_FIELD_FES_ID, fes_id_ndx);
                    dbal_entry_key_field32_set(unit, entry_handle_id_2nd_inst, DBAL_FIELD_FIELD_FES_PGM_ID,
                                               fes_pgm_id_allocated[fes_id_ndx]);
                    dbal_entry_key_field32_set(unit, entry_handle_id_2nd_inst, DBAL_FIELD_FIELD_PMF_FES_DATA_2MSB,
                                               fes2msb_id_ndx);
                    
                    dbal_value_field8_request(unit, entry_handle_id_2nd_inst, DBAL_FIELD_FIELD_FES_CHOSEN_MASK,
                                              INST_SINGLE, &chosen_mask);
                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_2nd_inst, DBAL_COMMIT));
                    
                    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_2nd_instruction, entry_handle_id_2nd_inst));
                    
                    if ((unsigned int) chosen_mask > DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES - 1)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Initial value select must be between %d and %d.\r\n",
                                     0, DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES - 1);
                    }
                    
                    (mask_used[chosen_mask]) = TRUE;
                }
                
                for (mask_ndx = 0; mask_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes;
                     mask_ndx++)
                {
                    if (mask_used[mask_ndx] && mask_nof_refs[fes_id_ndx][mask_ndx] > 0)
                    {
                        (mask_nof_refs[fes_id_ndx][mask_ndx])++;
                    }
                }
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_action_fes_array_delete(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_algo_field_action_fes_dealloc_out_t * dealloc_result_p)
{
    unsigned int fes_id_ndx;
    unsigned int mask_ndx;
    unsigned int fes2msb_ndx;
    dnx_field_actions_fes_common_info_fes2msb_t zero_fes2msb_conf;
    dnx_field_fes_chosen_mask_t zero_chosen_mask;
    dnx_field_fes_pgm_id_t zero_fes_pgm_id;
    dnx_field_fes_key_select_t zero_key_select;
    dnx_field_fes_mask_t zero_action_mask;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dealloc_result_p, _SHR_E_PARAM, "dealloc_result_p");

    
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_config_init
                    (unit, field_stage, &zero_fes2msb_conf, &zero_chosen_mask, &zero_fes_pgm_id, &zero_key_select,
                     &zero_action_mask));

    
    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        
        if (dealloc_result_p->belongs_to_fg[fes_id_ndx])
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Removing FES ID %d usage from context ID %d. %s%s\r\n", fes_id_ndx, context_id, EMPTY, EMPTY);
            
            SHR_IF_ERR_EXIT(dnx_field_fes_program_fes_instruction_hw_set
                            (unit, field_stage, context_id, fes_id_ndx, zero_fes_pgm_id, zero_key_select));
            
            if (dealloc_result_p->fes_pgm_id_to_delete[fes_id_ndx] == DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "FES ID %d for context ID %d. No FES instructions to delete. %s%s\r\n",
                             fes_id_ndx, context_id, EMPTY, EMPTY);
                continue;
            }
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Handling FES instructions deletion in FES ID %d for context ID %d. Writing to EFES. %s%s\r\n",
                         fes_id_ndx, context_id, EMPTY, EMPTY);
            
            for (mask_ndx = 0; mask_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes;
                 mask_ndx++)
            {
                if (dealloc_result_p->delete_mask[fes_id_ndx][mask_ndx])
                {
                    SHR_IF_ERR_EXIT(dnx_field_actions_fes_mask_hw_set
                                    (unit, field_stage, fes_id_ndx, mask_ndx, zero_action_mask));
                }
            }
            
            for (fes2msb_ndx = 0;
                 fes2msb_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes; fes2msb_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_field_actions_fes_2nd_instruction_hw_set
                                (unit, field_stage,
                                 fes_id_ndx, dealloc_result_p->fes_pgm_id_to_delete[fes_id_ndx], fes2msb_ndx,
                                 &zero_fes2msb_conf, zero_chosen_mask));
            }
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "End of handling FES instructions deletion in FES ID %d deletion for context ID %d. %s%s\r\n",
                         fes_id_ndx, context_id, EMPTY, EMPTY);
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_actions_fes_dealloc_current_state_read(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_algo_field_action_fes_dealloc_state_in_t * current_state_dealloc_p)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_dealloc_p, _SHR_E_PARAM, "current_state_dealloc_p");

    
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_context_hw_get
                    (unit, field_stage, context_id, current_state_dealloc_p->context_state.fes_pgm_id));
    
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_mask_context_hw_get
                    (unit, field_stage, context_id, current_state_dealloc_p->context_state.fes_pgm_id,
                     current_state_dealloc_p->context_state.mask_nof_refs));
    
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_pgm_mask_ref_count_hw_get
                    (unit, field_stage, context_id, current_state_dealloc_p->context_state.fes_pgm_id,
                     current_state_dealloc_p->context_state.mask_nof_refs,
                     current_state_dealloc_p->context_state.fes_quartet_nof_refs));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_actions_fes_detach(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_fes_id_t fes_id)
{
    dnx_algo_field_action_fes_dealloc_state_in_t current_state_dealloc;
    dnx_algo_field_action_fes_dealloc_out_t dealloc_result;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_dealloc_current_state_read
                    (unit, field_stage, context_id, &current_state_dealloc));
    
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_dealloc
                    (unit, field_stage, fg_id, context_id, fes_id, &current_state_dealloc, &dealloc_result));
    
    SHR_IF_ERR_EXIT(dnx_field_action_fes_array_delete(unit, field_stage, context_id, &dealloc_result));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_actions_fes_ace_detach(
    int unit,
    dnx_field_ace_id_t ace_id)
{
    unsigned int fes_id_ndx;
    unsigned int ace_id_ndx;
    dnx_field_actions_fes_common_info_fes2msb_t zero_fes2msb_conf;
    dnx_field_actions_fes_common_info_fes2msb_t read_fes2msb_conf;
    dnx_field_action_type_t action_type_invalid;
    dnx_field_fes_chosen_mask_t read_chosen_mask;
    dnx_field_fes_chosen_mask_t mask_to_delete;
    int mask_to_be_deleted;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_field_map_get_invalid_action_type(unit, DNX_FIELD_STAGE_ACE, &action_type_invalid));
    zero_fes2msb_conf.action_type = action_type_invalid;
    zero_fes2msb_conf.valid_bits = 0;
    zero_fes2msb_conf.shift = 0;
    zero_fes2msb_conf.type = 0;
    zero_fes2msb_conf.polarity = 0;

    
    for (fes_id_ndx = 0; fes_id_ndx < dnx_data_field.ace.nof_fes_instruction_per_context_get(unit); fes_id_ndx++)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "Removing FES ID %d usage from ACE ID %d. %s%s\r\n", fes_id_ndx, ace_id, EMPTY, EMPTY);
        
        SHR_IF_ERR_EXIT(dnx_field_actions_ace_fes_program_table_hw_get
                        (unit, fes_id_ndx, ace_id, &read_fes2msb_conf, &mask_to_delete));
        
        mask_to_be_deleted = FALSE;
        if (mask_to_delete != DNX_ALGO_FIELD_ZERO_FES_ACE_ACTION_MASK_ID)
        {
            mask_to_be_deleted = TRUE;
            for (ace_id_ndx = 0; ace_id_ndx < dnx_data_field.ace.nof_ace_id_get(unit); ace_id_ndx++)
            {
                if (ace_id_ndx != ace_id)
                {
                    SHR_IF_ERR_EXIT(dnx_field_actions_ace_fes_program_table_hw_get
                                    (unit, fes_id_ndx, ace_id_ndx, &read_fes2msb_conf, &read_chosen_mask));
                    if (read_chosen_mask == mask_to_delete)
                    {
                        mask_to_be_deleted = FALSE;
                        break;
                    }
                }
            }
        }

        
        SHR_IF_ERR_EXIT(dnx_field_actions_ace_fes_program_table_hw_set
                        (unit, fes_id_ndx, ace_id, &zero_fes2msb_conf, DNX_ALGO_FIELD_ZERO_FES_ACE_ACTION_MASK_ID));

        
        if (mask_to_be_deleted)
        {
            SHR_IF_ERR_EXIT(dnx_field_actions_ace_fes_mask_hw_set
                            (unit, fes_id_ndx, mask_to_be_deleted, DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK));
            LOG_DEBUG_EX(BSL_LOG_MODULE, "Deleting mask ID %d for FES ID %d. %s%s\r\n", mask_to_be_deleted, fes_id_ndx,
                         EMPTY, EMPTY);
        }

        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "End of handling FES instructions deletion in FES ID %d deletion for ACE ID %d. %s%s\r\n",
                     fes_id_ndx, ace_id, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_actions_context_fes_info_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_actions_fes_context_get_info_t * context_fes_get_info_p)
{
    unsigned int fes_id_ndx;
    unsigned int fes_quartet_ndx;
    unsigned int fes2msb_id_ndx;
    dnx_field_fes_pgm_id_t fes_pgm_id;
    dnx_field_fes_key_select_t key_select;
    dnx_algo_field_action_fes_quartet_sw_state_get_info_t fes_quartet_sw_state_info;
    dnx_field_actions_fes_common_info_fes2msb_t fes_inst_common_info_fes2msb;
    dnx_field_fes_chosen_mask_t chosen_mask;
    dnx_field_fes_mask_t action_mask;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(context_fes_get_info_p, _SHR_E_PARAM, "context_fes_get_info_p");

    sal_memset(context_fes_get_info_p, 0x0, sizeof(*context_fes_get_info_p));
    
    fes_quartet_ndx = 0;
    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        
        SHR_IF_ERR_EXIT(dnx_field_fes_program_fes_instruction_hw_get
                        (unit, field_stage, context_id, fes_id_ndx, &fes_pgm_id, &key_select));
        
        if (fes_pgm_id != DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
        {
            
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_quartet_sw_state_info_get
                            (unit, field_stage, context_id, fes_id_ndx, fes_pgm_id, &fes_quartet_sw_state_info));
            
            context_fes_get_info_p->fes_quartets[fes_quartet_ndx].fes_id = fes_id_ndx;
            context_fes_get_info_p->fes_quartets[fes_quartet_ndx].fes_pgm_id = fes_pgm_id;
            context_fes_get_info_p->fes_quartets[fes_quartet_ndx].fg_id = fes_quartet_sw_state_info.fg_id;
            context_fes_get_info_p->fes_quartets[fes_quartet_ndx].place_in_fg = fes_quartet_sw_state_info.place_in_fg;
            context_fes_get_info_p->fes_quartets[fes_quartet_ndx].priority = fes_quartet_sw_state_info.priotrity;
            context_fes_get_info_p->fes_quartets[fes_quartet_ndx].key_select = key_select;
            
            for (fes2msb_id_ndx = 0;
                 fes2msb_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
                 fes2msb_id_ndx++)
            {
                
                SHR_IF_ERR_EXIT(dnx_field_actions_fes_2nd_instruction_hw_get
                                (unit, field_stage, fes_id_ndx, fes_pgm_id, fes2msb_id_ndx,
                                 &fes_inst_common_info_fes2msb, &chosen_mask));
                
                SHR_IF_ERR_EXIT(dnx_field_actions_fes_mask_hw_get
                                (unit, field_stage, fes_id_ndx, chosen_mask, &action_mask));
                
                if (chosen_mask == DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID
                    && action_mask != DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Action mask %d for FES ID %d has illegal value for zero mask: 0x%08X. \r\n",
                                 chosen_mask, fes_id_ndx, action_mask);
                }
                
                context_fes_get_info_p->fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_id_ndx].action_type =
                    fes_inst_common_info_fes2msb.action_type;
                context_fes_get_info_p->fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_id_ndx].valid_bits =
                    fes_inst_common_info_fes2msb.valid_bits;
                context_fes_get_info_p->fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_id_ndx].shift =
                    fes_inst_common_info_fes2msb.shift;
                context_fes_get_info_p->fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_id_ndx].type =
                    fes_inst_common_info_fes2msb.type;
                context_fes_get_info_p->fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_id_ndx].polarity =
                    fes_inst_common_info_fes2msb.polarity;
                context_fes_get_info_p->fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_id_ndx].chosen_mask =
                    chosen_mask;
                context_fes_get_info_p->fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_id_ndx].mask = action_mask;

            }
            
            fes_quartet_ndx++;
        }
    }
    
    context_fes_get_info_p->nof_fes_quartets = fes_quartet_ndx;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_actions_fes_id_info_get(
    int unit,
    dnx_field_stage_e field_stage,
    uint32 fes_id,
    dnx_field_actions_fes_id_get_info_t * fes_id_get_info_p)
{
    uint8 is_cs_allocated;
    int ctx_id_index;
    uint8 mask_id_iter;
    unsigned int fes2msb_id_ndx;
    dnx_field_fes_pgm_id_t fes_pgm_id;
    dnx_field_fes_key_select_t key_select;
    dnx_algo_field_action_fes_quartet_sw_state_get_info_t fes_quartet_sw_state_info;
    dnx_field_actions_fes_common_info_fes2msb_t fes_inst_common_info_fes2msb;
    dnx_field_fes_chosen_mask_t chosen_mask;
    dnx_field_fes_mask_t action_mask;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fes_id_get_info_p, _SHR_E_PARAM, "fes_id_get_info_p");

    sal_memset(fes_id_get_info_p, 0, sizeof(*fes_id_get_info_p));
    for (fes_pgm_id = 0; fes_pgm_id < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS; fes_pgm_id++)
    {
        
        fes_id_get_info_p->fes_pgm_info[fes_pgm_id].field_group = DNX_FIELD_GROUP_INVALID;

        
        for (mask_id_iter = 0; mask_id_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES; mask_id_iter++)
        {
            fes_id_get_info_p->fes_pgm_info[fes_pgm_id].masks_id_used[mask_id_iter] =
                DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
        }
    }

    is_cs_allocated = 0;

    for (ctx_id_index = 0; ctx_id_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS; ctx_id_index++)
    {
        
        SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated(unit, field_stage, ctx_id_index, &is_cs_allocated));
        if (!is_cs_allocated)
        {
            continue;
        }

        
        SHR_IF_ERR_EXIT(dnx_field_fes_program_fes_instruction_hw_get
                        (unit, field_stage, ctx_id_index, fes_id, &fes_pgm_id, &key_select));
        
        if (fes_pgm_id != DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
        {
            
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_quartet_sw_state_info_get
                            (unit, field_stage, ctx_id_index, fes_id, fes_pgm_id, &fes_quartet_sw_state_info));

            SHR_IF_ERR_EXIT(utilex_bitstream_set_bit
                            (fes_id_get_info_p->fes_pgm_info[fes_pgm_id].context_bmp, ctx_id_index));

            fes_id_get_info_p->fes_pgm_info[fes_pgm_id].field_group = fes_quartet_sw_state_info.fg_id;

            fes_id_get_info_p->place_in_fg = fes_quartet_sw_state_info.place_in_fg;
            fes_id_get_info_p->key_select = key_select;

            
            for (fes2msb_id_ndx = 0;
                 fes2msb_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
                 fes2msb_id_ndx++)
            {
                
                SHR_IF_ERR_EXIT(dnx_field_actions_fes_2nd_instruction_hw_get
                                (unit, field_stage, fes_id, fes_pgm_id, fes2msb_id_ndx,
                                 &fes_inst_common_info_fes2msb, &chosen_mask));
                fes_id_get_info_p->fes_pgm_info[fes_pgm_id].masks_id_used[chosen_mask] = TRUE;

                
                fes_id_get_info_p->fes_pgm_info[fes_pgm_id].
                    pgm_ctx_2msb_info[ctx_id_index].fes2msb_info[fes2msb_id_ndx].action_type =
                    fes_inst_common_info_fes2msb.action_type;
                fes_id_get_info_p->fes_pgm_info[fes_pgm_id].
                    pgm_ctx_2msb_info[ctx_id_index].fes2msb_info[fes2msb_id_ndx].valid_bits =
                    fes_inst_common_info_fes2msb.valid_bits;
                fes_id_get_info_p->fes_pgm_info[fes_pgm_id].
                    pgm_ctx_2msb_info[ctx_id_index].fes2msb_info[fes2msb_id_ndx].shift =
                    fes_inst_common_info_fes2msb.shift;
                fes_id_get_info_p->fes_pgm_info[fes_pgm_id].
                    pgm_ctx_2msb_info[ctx_id_index].fes2msb_info[fes2msb_id_ndx].type =
                    fes_inst_common_info_fes2msb.type;
                fes_id_get_info_p->fes_pgm_info[fes_pgm_id].
                    pgm_ctx_2msb_info[ctx_id_index].fes2msb_info[fes2msb_id_ndx].polarity =
                    fes_inst_common_info_fes2msb.polarity;
                fes_id_get_info_p->fes_pgm_info[fes_pgm_id].
                    pgm_ctx_2msb_info[ctx_id_index].fes2msb_info[fes2msb_id_ndx].chosen_mask = chosen_mask;
            }
        }
    }

    for (mask_id_iter = 0; mask_id_iter < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes;
         mask_id_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_field_actions_fes_mask_hw_get(unit, field_stage, fes_id, mask_id_iter, &action_mask));
        
        if (mask_id_iter == DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID
            && action_mask != DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Action mask %d for FES ID %d has illegal value for zero mask: 0x%08X. \r\n",
                         mask_id_iter, fes_id, action_mask);
        }
        
        fes_id_get_info_p->action_masks[mask_id_iter] = action_mask;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_actions_context_fes_info_to_group_fes_info(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_actions_fes_context_get_info_t * fes_context_info_p,
    dnx_field_actions_fes_context_group_get_info_t * fes_group_info_p)
{
    unsigned int fes_quartet_group_ndx;
    unsigned int fes_quartet_context_ndx;
    unsigned int nof_fes_quartets_initial;
    unsigned int nof_fes_quartets_added;
    int place_in_fg;
    dnx_field_actions_fes_quartet_context_group_get_info_t *fes_info_to_fill_p;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fes_context_info_p, _SHR_E_PARAM, "fes_context_info_p");
    SHR_NULL_CHECK(fes_group_info_p, _SHR_E_PARAM, "fes_group_info_p");

    
    if (fes_context_info_p->nof_fes_quartets > DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of FES quartets exceeds the maximum number of FESes.\r\n");
    }

    
    sal_memset(fes_group_info_p->initial_fes_quartets, 0x0, sizeof(fes_group_info_p->initial_fes_quartets));
    sal_memset(fes_group_info_p->added_fes_quartets, 0x0, sizeof(fes_group_info_p->added_fes_quartets));
    for (fes_quartet_group_ndx = 0; fes_quartet_group_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP;
         fes_quartet_group_ndx++)
    {
        fes_group_info_p->initial_fes_quartets[fes_quartet_group_ndx].priority = DNX_FIELD_ACTION_PRIORITY_INVALID;
        fes_group_info_p->initial_fes_quartets[fes_quartet_group_ndx].fes_id = DNX_FIELD_EFES_ID_INVALID;
        fes_group_info_p->added_fes_quartets[fes_quartet_group_ndx].priority = DNX_FIELD_ACTION_PRIORITY_INVALID;
        fes_group_info_p->added_fes_quartets[fes_quartet_group_ndx].fes_id = DNX_FIELD_EFES_ID_INVALID;
    }

    
    nof_fes_quartets_initial = 0;
    nof_fes_quartets_added = 0;
    for (fes_quartet_context_ndx = 0; fes_quartet_context_ndx < fes_context_info_p->nof_fes_quartets;
         fes_quartet_context_ndx++)
    {
        if (fes_context_info_p->fes_quartets[fes_quartet_context_ndx].fes_id == DNX_FIELD_EFES_ID_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "FES ID in invalid.\r\n");
        }
        if (fes_context_info_p->fes_quartets[fes_quartet_context_ndx].fg_id == fg_id)
        {
            place_in_fg = fes_context_info_p->fes_quartets[fes_quartet_context_ndx].place_in_fg;
            
            if ((place_in_fg >= 0) && (place_in_fg < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP))
            {
                fes_info_to_fill_p = &fes_group_info_p->initial_fes_quartets[place_in_fg];
                nof_fes_quartets_initial++;
            }
            else if ((place_in_fg >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP) &&
                     (place_in_fg < (DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP * 2)))
            {
                fes_info_to_fill_p =
                    &fes_group_info_p->added_fes_quartets[place_in_fg -
                                                          DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
                nof_fes_quartets_added++;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Place in field group (%d) of FES quartet %d, for field group %d, "
                             "exceeds the maximum number of actions per group (%d).\r\n",
                             place_in_fg, fes_quartet_context_ndx, fg_id,
                             DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP * 2);
            }
            
            if (fes_info_to_fill_p->fes_id != DNX_FIELD_EFES_ID_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Place in field group (%d), for field group %d for the context ID, "
                             "appears twice.\r\n", place_in_fg, fg_id);
            }
            
            fes_info_to_fill_p->fes_id = fes_context_info_p->fes_quartets[fes_quartet_context_ndx].fes_id;
            fes_info_to_fill_p->fes_pgm_id = fes_context_info_p->fes_quartets[fes_quartet_context_ndx].fes_pgm_id;
            fes_info_to_fill_p->priority = fes_context_info_p->fes_quartets[fes_quartet_context_ndx].priority;
            fes_info_to_fill_p->key_select = fes_context_info_p->fes_quartets[fes_quartet_context_ndx].key_select;
            sal_memcpy(fes_info_to_fill_p->fes2msb_info,
                       fes_context_info_p->fes_quartets[fes_quartet_context_ndx].fes2msb_info,
                       sizeof(fes_info_to_fill_p->fes2msb_info));
        }
    }
    
    
    if (nof_fes_quartets_initial > DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of FES quartets found belonging to the field group %d added "
                     "at group add (%d) exceeds the maximum number of actions per field group (%d).\r\n",
                     fg_id, nof_fes_quartets_initial, DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP);
    }
    if (nof_fes_quartets_added > DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of FES quartets found belonging to the field group %d added "
                     "with efes add (%d) exceeds the maximum number of actions per field group (%d).\r\n",
                     fg_id, nof_fes_quartets_added, DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_actions_ace_id_nof_mask_state_get(
    int unit,
    uint8 nof_masks[DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT])
{
    dnx_field_fes_id_t fes_id_ndx;
    dnx_field_fes_mask_id_t mask_id;
    uint32 action_mask;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(nof_masks, _SHR_E_PARAM, "nof_masks");

    
    for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        nof_masks[fes_id_ndx] = 0;
    }

    
    for (fes_id_ndx = 0; fes_id_ndx < dnx_data_field.ace.nof_fes_instruction_per_context_get(unit); fes_id_ndx++)
    {
        for (mask_id = 0; mask_id < dnx_data_field.ace.nof_masks_per_fes_get(unit); mask_id++)
        {
            SHR_IF_ERR_EXIT(dnx_field_actions_ace_fes_mask_hw_get(unit, fes_id_ndx, mask_id, &action_mask));
            if (action_mask != DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK)
            {
                
                if (mask_id == DNX_ALGO_FIELD_ZERO_FES_ACE_ACTION_MASK_ID)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "ACE EFES action mask ID %d for FES ID %d has illegal value for zero mask: 0x%08X.\r\n",
                                 mask_id, fes_id_ndx, action_mask);
                }
                nof_masks[fes_id_ndx]++;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_actions_ace_id_fes_info_get(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_field_actions_fes_ace_get_info_t * ace_id_fes_get_info_p)
{
    dnx_field_fes_id_t fes_id_ndx;
    unsigned int fes_instr_ndx;
    dnx_field_actions_fes_common_info_fes2msb_t fes_inst_common_info_fes2msb;
    dnx_field_fes_chosen_mask_t chosen_mask;
    uint32 action_mask;
    dnx_field_action_type_t action_type_invalid;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ace_id_fes_get_info_p, _SHR_E_PARAM, "ace_id_fes_get_info_p");

    
    SHR_IF_ERR_EXIT(dnx_field_map_get_invalid_action_type(unit, DNX_FIELD_STAGE_ACE, &action_type_invalid));

    
    fes_instr_ndx = 0;
    for (fes_id_ndx = 0; fes_id_ndx < dnx_data_field.ace.nof_fes_instruction_per_context_get(unit); fes_id_ndx++)
    {
        
        SHR_IF_ERR_EXIT(dnx_field_actions_ace_fes_program_table_hw_get
                        (unit, fes_id_ndx, ace_id, &fes_inst_common_info_fes2msb, &chosen_mask));
        
        if (fes_inst_common_info_fes2msb.action_type != action_type_invalid)
        {
            
            SHR_IF_ERR_EXIT(dnx_field_actions_ace_fes_mask_hw_get(unit, fes_id_ndx, chosen_mask, &action_mask));
            
            ace_id_fes_get_info_p->fes_instr[fes_instr_ndx].fes_id = fes_id_ndx;
            ace_id_fes_get_info_p->fes_instr[fes_instr_ndx].action_type = fes_inst_common_info_fes2msb.action_type;
            ace_id_fes_get_info_p->fes_instr[fes_instr_ndx].valid_bits = fes_inst_common_info_fes2msb.valid_bits;
            ace_id_fes_get_info_p->fes_instr[fes_instr_ndx].shift = fes_inst_common_info_fes2msb.shift;
            ace_id_fes_get_info_p->fes_instr[fes_instr_ndx].type = fes_inst_common_info_fes2msb.type;
            ace_id_fes_get_info_p->fes_instr[fes_instr_ndx].polarity = fes_inst_common_info_fes2msb.polarity;
            ace_id_fes_get_info_p->fes_instr[fes_instr_ndx].chosen_mask = chosen_mask;
            ace_id_fes_get_info_p->fes_instr[fes_instr_ndx].mask = action_mask;

            
            if (chosen_mask == DNX_ALGO_FIELD_ZERO_FES_ACE_ACTION_MASK_ID
                && action_mask != DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "ACE EFES action mask %d for FES ID %d has illegal value for zero mask: 0x%08X.\r\n",
                             chosen_mask, fes_id_ndx, action_mask);
            }
            
            fes_instr_ndx++;
        }
    }
    
    ace_id_fes_get_info_p->nof_fes_instr = fes_instr_ndx;

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_efes_action_add_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    bcm_field_action_priority_t action_priority,
    int use_condition,
    int efes_condition_msb,
    dnx_field_actions_fes_single_add_conf_t efes_encoded_extraction[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES])
{
    int efes_2msb_index;
    int extraction_offset_within_fg;
    int nof_condition_bits = DNX_FIELD_NOF_BITS_2MSB_SELECT;
    int nof_bits_efes_input = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_bits_in_fes_key_select;
    unsigned int is_attached;
    unsigned int action_type_size;
    int valid_action_found;
    unsigned int fg_payload_size;
    int fg_payload_min_offset;
    int fg_payload_max_offset;
    dnx_field_group_type_e fg_type;
    dnx_field_fes_id_t fes_id;
    int nof_progs_per_efes = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
    int nof_bits_in_fes_action = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_bits_in_fes_action;
    int action_is_void;
    dnx_field_group_context_attach_flags_e attach_flags;
    dnx_field_actions_fes_context_get_info_t context_fes_get_info;
    int fes_quartet_ndx;
    int nof_added_actions;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(efes_encoded_extraction, _SHR_E_PARAM, "efes_encoded_extraction");

    DNX_FIELD_ACTION_PRIORITY_VERIFY(action_priority);
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);

    if (DNX_FIELD_ACTION_PRIORITY_IS_POSITION(action_priority) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "When adding an EFES, the priority must by position, that is created by "
                     "BCM_FIELD_ACTION_POSITION(). fg_id %d, context_id %d, priority 0x%x.\r\n",
                     fg_id, context_id, action_priority);
    }
    
    DNX_FIELD_ACTION_PRIORITY_POSITION_FES_ID_GET(fes_id, unit, field_stage, action_priority);

    SHR_IF_ERR_EXIT(dnx_field_group_is_context_id_on_arr(unit, fg_id, context_id, &is_attached));
    if (is_attached == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Field Group %d is not attached to context %d. \n", fg_id, context_id);
    }

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    if (fg_type == DNX_FIELD_GROUP_TYPE_CONST)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot perform EFES add for const field groups (field group ID %d).\n", fg_id);
    }
    if (fg_type == DNX_FIELD_GROUP_TYPE_KBP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot perform EFES add for external field groups (field group ID %d).\n", fg_id);
    }

    if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION && use_condition)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_context_attach_flags_get(unit, fg_id, context_id, &attach_flags));
        if ((attach_flags & DNX_FIELD_GROUP_CONTEXT_ATTACH_FLAG_32_RESULT_MSB_ALIGN) == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot add an EFES with condition bits to a direct extraction field group "
                         "that was not attached with the 32_RESULT_MSB_ALIGN flag. "
                         "fg_id %d, context_id %d, priority 0x%x.\n", fg_id, context_id, action_priority);
        }
    }

    if ((dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_array <= 0) ||
        (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_id_per_array <= 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Field group %d is in stage \"%s\", stage does not have EFES.\n",
                     fg_id, dnx_field_stage_text(unit, field_stage));
    }

    if (use_condition && (efes_condition_msb < 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "efes_condition_msb (%d) is negative. fg_id %d, context_id %d, priority 0x%x.\r\n",
                     efes_condition_msb, fg_id, context_id, action_priority);
    }

    
    for (efes_2msb_index = nof_progs_per_efes; efes_2msb_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES;
         efes_2msb_index++)
    {
        if (efes_encoded_extraction[efes_2msb_index].dnx_action != DNX_FIELD_ACTION_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Configuration option %d is valid, but there are only %d options for FES in stage %s. "
                         "fg_id %d, context_id %d, priority 0x%x.\r\n",
                         efes_2msb_index, nof_progs_per_efes, dnx_field_stage_text(unit, field_stage),
                         fg_id, context_id, action_priority);
        }
    }

    
    if (use_condition == FALSE)
    {
        for (efes_2msb_index = 1; efes_2msb_index < nof_progs_per_efes; efes_2msb_index++)
        {
            if (efes_encoded_extraction[efes_2msb_index].dnx_action != DNX_FIELD_ACTION_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "When no condition is provided, only the first condition configuration "
                             "should be valid. Combination %d uses action %s (0x%x). "
                             "fg_id %d, context_id, %d priority 0x%x.\r\n",
                             efes_2msb_index,
                             dnx_field_dnx_action_text(unit, efes_encoded_extraction[efes_2msb_index].dnx_action),
                             efes_encoded_extraction[efes_2msb_index].dnx_action, fg_id, context_id, action_priority);
            }
        }
    }

    
    valid_action_found = FALSE;
    for (efes_2msb_index = 0; efes_2msb_index < nof_progs_per_efes; efes_2msb_index++)
    {
        if (efes_encoded_extraction[efes_2msb_index].dnx_action != DNX_FIELD_ACTION_INVALID)
        {
            valid_action_found = TRUE;
            break;
        }
    }
    if (valid_action_found == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "At least one combination must have a valid action. EFES configuration does nothing. "
                     "fg_id %d, context_id, %d priority 0x%x.\r\n", fg_id, context_id, action_priority);
    }

    
    SHR_IF_ERR_EXIT(dnx_field_group_actions_payload_size_get(unit, fg_id, &fg_payload_size));
    if (fg_payload_size <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "fg_id %d had no paylaod. payload_size %d, context_id, %d priority 0x%x.\r\n",
                     fg_id, fg_payload_size, context_id, action_priority);
    }
    SHR_IF_ERR_EXIT(dnx_field_group_payload_template_min_max_get(unit, fg_id,
                                                                 &fg_payload_min_offset, &fg_payload_max_offset));
    if ((fg_payload_min_offset < 0) || (fg_payload_max_offset < 0))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Cannot aquire payload range on template. fg_payload_min_offset %d, fg_payload_max_offset %d. "
                     "fg_id %d, context_id, %d priority 0x%x.\r\n",
                     fg_payload_min_offset, fg_payload_max_offset, fg_id, context_id, action_priority);
    }

    for (efes_2msb_index = 0; efes_2msb_index < nof_progs_per_efes; efes_2msb_index++)
    {
        if (efes_encoded_extraction[efes_2msb_index].dnx_action == DNX_FIELD_ACTION_INVALID)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_is_void
                        (unit, field_stage, efes_encoded_extraction[efes_2msb_index].dnx_action, &action_is_void));
        if (action_is_void)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot write a void action to EFES. "
                         "fg_id %d, context_id, %d priority 0x%x, condition %d.\r\n",
                         fg_id, context_id, action_priority, efes_2msb_index);
        }
        if (DNX_ACTION_CLASS(efes_encoded_extraction[efes_2msb_index].dnx_action) == DNX_FIELD_ACTION_CLASS_USER)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "There is no benefit to using user defined actions when adding an EFES. "
                         "fg_id %d, context_id, %d priority 0x%x, condition %d.\r\n",
                         fg_id, context_id, action_priority, efes_2msb_index);
        }
        
        if (efes_encoded_extraction[efes_2msb_index].size <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Number of bits taken from the input (%d) must be at least 1. "
                         "fg_id %d, context_id, %d priority 0x%x, condition %d.\r\n",
                         efes_encoded_extraction[efes_2msb_index].size,
                         fg_id, context_id, action_priority, efes_2msb_index);
        }
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, field_stage,
                                                      efes_encoded_extraction[efes_2msb_index].dnx_action,
                                                      &action_type_size));

        if (efes_encoded_extraction[efes_2msb_index].size > nof_bits_in_fes_action)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Number of bits taken from the input (%d) exceeds the maximum number of bits for EFES (%d). "
                         "fg_id %d, context_id, %d priority 0x%x, condition %d.\r\n",
                         efes_encoded_extraction[efes_2msb_index].size, nof_bits_in_fes_action,
                         fg_id, context_id, action_priority, efes_2msb_index);
        }

        
        if ((action_type_size > 0) && (efes_encoded_extraction[efes_2msb_index].size > action_type_size))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Number of bits taken from the input (%d) exceeds the number of bits on the action (%d). "
                         "fg_id %d, context_id, %d priority 0x%x, condition %d.\r\n",
                         efes_encoded_extraction[efes_2msb_index].size, action_type_size,
                         fg_id, context_id, action_priority, efes_2msb_index);
        }
        else
        {
            if ((action_type_size <= 0) && (efes_encoded_extraction[efes_2msb_index].size > 1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Number of bits taken from the input (%d) exceeds the number of bits on the action (%d, "
                             "so we allow up to one bit). "
                             "fg_id %d, context_id, %d priority 0x%x, condition %d.\r\n",
                             efes_encoded_extraction[efes_2msb_index].size, action_type_size,
                             fg_id, context_id, action_priority, efes_2msb_index);
            }
        }
        
        if (((uint32) (efes_encoded_extraction[efes_2msb_index].mask)) != efes_encoded_extraction[efes_2msb_index].mask)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "mask 0x%x has more bits than expected.\r\n",
                         efes_encoded_extraction[efes_2msb_index].mask);
        }
        if ((action_type_size < SAL_UINT32_NOF_BITS) &&
            ((efes_encoded_extraction[efes_2msb_index].mask & SAL_UPTO_BIT(action_type_size)) !=
             efes_encoded_extraction[efes_2msb_index].mask))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "OR mask (0x%x) exceeds the number of bits on the action (%d). "
                         "fg_id %d, context_id, %d priority 0x%x, condition %d.\r\n",
                         efes_encoded_extraction[efes_2msb_index].mask, action_type_size,
                         fg_id, context_id, action_priority, efes_2msb_index);
        }

        if ((efes_encoded_extraction[efes_2msb_index].lsb + efes_encoded_extraction[efes_2msb_index].size) >
            fg_payload_max_offset + 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "input offset (%d) plus bits to take (%d) is larger than the reads beyond the last bit on "
                         "the payload of the field group (%d). "
                         "fg_id %d, context_id, %d priority 0x%x, condition %d.\r\n",
                         efes_encoded_extraction[efes_2msb_index].lsb, efes_encoded_extraction[efes_2msb_index].size,
                         fg_payload_max_offset, fg_id, context_id, action_priority, efes_2msb_index);
        }
        if (efes_encoded_extraction[efes_2msb_index].lsb < fg_payload_min_offset)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "input offset (%d) is smaller than the minimal bit of the field group %d. "
                         "fg_id %d, context_id, %d priority 0x%x, condition %d.\r\n",
                         efes_encoded_extraction[efes_2msb_index].lsb, fg_payload_min_offset,
                         fg_id, context_id, action_priority, efes_2msb_index);
        }
        if ((efes_encoded_extraction[efes_2msb_index].dont_use_valid_bit == FALSE) &&
            ((efes_encoded_extraction[efes_2msb_index].lsb - 1) < fg_payload_min_offset))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "input offset (%d) minus one for valid bit (%d) is smaller "
                         "than the minimal bit of the field group %d. "
                         "fg_id %d, context_id, %d priority 0x%x, condition %d.\r\n",
                         efes_encoded_extraction[efes_2msb_index].lsb, efes_encoded_extraction[efes_2msb_index].lsb - 1,
                         fg_payload_min_offset, fg_id, context_id, action_priority, efes_2msb_index);
        }
    }

    
    if (use_condition)
    {
        if (efes_condition_msb < 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Condition bits MSB is negative (%d). fg_id %d, context_id, %d priority 0x%x.\r\n",
                         efes_condition_msb, fg_id, context_id, action_priority);
        }
        if (efes_condition_msb - nof_condition_bits + 1 < 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Condition bits MSB is %d, and the number of condition bits is %d, so the LSB of the "
                         "condition bit is %d, negative. fg_id %d, context_id, %d priority 0x%x.\r\n",
                         efes_condition_msb, nof_condition_bits, efes_condition_msb - nof_condition_bits + 1,
                         fg_id, context_id, action_priority);
        }
        if (efes_condition_msb - nof_condition_bits + 1 < 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Condition bits MSB is %d, and the number of condition bits is %d, so the LSB of the "
                         "condition bit is %d, negative. fg_id %d, context_id, %d priority 0x%x.\r\n",
                         efes_condition_msb, nof_condition_bits, efes_condition_msb - nof_condition_bits + 1,
                         fg_id, context_id, action_priority);
        }

        if (efes_condition_msb > fg_payload_max_offset)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Condition bits MSB is %d, but the maximal bit in the field group's payload is %d. "
                         "fg_id %d, context_id, %d priority 0x%x.\r\n",
                         efes_condition_msb, fg_payload_max_offset, fg_id, context_id, action_priority);
        }
        if (efes_condition_msb - nof_condition_bits + 1 < fg_payload_min_offset)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Condition bits MSB is %d, and the number of condition bits is %d, so the LSB of the "
                         "condition bit is %d, and the minimum for the field group is %d. "
                         "fg_id %d, context_id, %d priority 0x%x.\r\n",
                         efes_condition_msb, nof_condition_bits, efes_condition_msb - nof_condition_bits + 1,
                         fg_payload_min_offset, fg_id, context_id, action_priority);
        }
        for (efes_2msb_index = 0; efes_2msb_index < nof_progs_per_efes; efes_2msb_index++)
        {
            if (efes_encoded_extraction[efes_2msb_index].dnx_action == DNX_FIELD_ACTION_INVALID)
            {
                continue;
            }
            
            extraction_offset_within_fg = efes_encoded_extraction[efes_2msb_index].lsb;
            if (efes_condition_msb < extraction_offset_within_fg)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Condition MSB (%d) cannot be smaller than the extraction LSB (%d). "
                             "fg_id %d, context_id, %d priority 0x%x.\r\n",
                             efes_condition_msb, extraction_offset_within_fg, fg_id, context_id, action_priority);
            }
            if (efes_encoded_extraction[efes_2msb_index].dont_use_valid_bit)
            {
                
                if ((efes_condition_msb - extraction_offset_within_fg) > (nof_bits_efes_input - 1))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Extraction cannot begin more than %d bits before the condition MSB. "
                                 "lsb is %d, condition MSB is at %d. "
                                 "fg_id %d, context_id, %d, priority 0x%x, condition %d.\r\n",
                                 nof_bits_efes_input - 1, extraction_offset_within_fg, efes_condition_msb,
                                 fg_id, context_id, action_priority, efes_2msb_index);
                }
            }
            else
            {
                
                if ((efes_condition_msb - extraction_offset_within_fg + 1) > (nof_bits_efes_input - 1))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Extraction cannot begin more than %d bits before the condition MSB. "
                                 "lsb is %d (minus one for valid bit is %d), condition MSB is at %d. "
                                 "fg_id %d, context_id, %d, priority 0x%x, condition %d.\r\n",
                                 nof_bits_efes_input - 1, extraction_offset_within_fg, extraction_offset_within_fg - 1,
                                 efes_condition_msb, fg_id, context_id, action_priority, efes_2msb_index);
                }
            }
            if ((extraction_offset_within_fg + efes_encoded_extraction[efes_2msb_index].size - 1) > efes_condition_msb)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Extraction cannot end after the condition MSB. "
                             "lsb (%d) plus size (%d) equals %d. Condition MSB is %d. "
                             "fg_id %d, context_id, %d, priority 0x%x, condition %d.\r\n",
                             extraction_offset_within_fg, efes_encoded_extraction[efes_2msb_index].size,
                             extraction_offset_within_fg + efes_encoded_extraction[efes_2msb_index].size,
                             efes_condition_msb, fg_id, context_id, action_priority, efes_2msb_index);
            }

        }
    }

    

    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id, &context_fes_get_info));
    if (context_fes_get_info.nof_fes_quartets > DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT)
    {
        nof_added_actions = 0;
        for (fes_quartet_ndx = 0; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets; fes_quartet_ndx++)
        {
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg >=
                DNX_ALGO_FIELD_ACTION_POST_ATTACH_FIRST_PLACE_IN_FG)
            {
                nof_added_actions++;
            }
        }
        if (nof_added_actions >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE,
                         "Already added %d EFESes to the field group and context, maximum is %d. "
                         "fg_id %d, context_id, %d, priority 0x%x, condition %d.\r\n",
                         nof_added_actions, DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT,
                         fg_id, context_id, action_priority, efes_2msb_index);
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_efes_action_add(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    bcm_field_action_priority_t action_priority,
    int use_condition,
    int efes_condition_msb,
    dnx_field_actions_fes_single_add_conf_t efes_encoded_extraction[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES])
{
    dnx_field_stage_e field_stage;
    dbal_enum_value_field_field_io_e field_io;
    dnx_field_actions_fes_single_add_conf_t
        efes_encoded_extraction_int[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];
    unsigned int fes_2msb_comb;
    int template_shift;
    unsigned int action_input_select = DNX_FIELD_FEM_KEY_SELECT_INVALID;
    int group_position;
    int de_fg_lsb;
    int fg_payload_min_offset;
    int fg_payload_max_offset;
    int potential_2msb_min = -1;
    int potential_2msb_max = -2;
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    dnx_field_group_type_e fg_type;
    int key_select_found;
    int any_key_select_on_group;
    int potential_2msb;
    char legal_2msb_places[(DNX_FIELD_ACTIONS_NUM_LOCATIONS_OF_2MSB_ON_FG * 6) + 1] = { 0 };
    int nof_progs_per_efes;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    SHR_INVOKE_VERIFY_DNX(dnx_field_efes_action_add_verify(unit, field_stage, fg_id, context_id, action_priority,
                                                           use_condition, efes_condition_msb, efes_encoded_extraction));

    nof_progs_per_efes = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;

    sal_memcpy(efes_encoded_extraction_int, efes_encoded_extraction, sizeof(efes_encoded_extraction_int));

    
    if (use_condition == FALSE)
    {
        for (fes_2msb_comb = 1; fes_2msb_comb < nof_progs_per_efes; fes_2msb_comb++)
        {
            sal_memcpy(&efes_encoded_extraction_int[fes_2msb_comb], &efes_encoded_extraction_int[0],
                       sizeof(efes_encoded_extraction_int[0]));
        }
    }

    SHR_IF_ERR_EXIT(dnx_field_group_field_io_get(unit, fg_id, context_id, &field_io));
    SHR_IF_ERR_EXIT(dnx_field_group_action_buffer_shift_get(unit, fg_id, &template_shift));
    
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_context_de_position_get(unit, fg_id, context_id, &de_fg_lsb));
    }
    else
    {
        de_fg_lsb = 0;
    }

    if (use_condition)
    {
        group_position = template_shift + de_fg_lsb;
        
        SHR_IF_ERR_EXIT(dnx_field_group_payload_template_min_max_get(unit, fg_id,
                                                                     &fg_payload_min_offset, &fg_payload_max_offset));
        
        SHR_IF_ERR_EXIT(dnx_field_actions_fes_key_select_2msb_get(unit, field_io, field_stage,
                                                                  group_position,
                                                                  fg_payload_min_offset, fg_payload_max_offset,
                                                                  DNX_FIELD_NOF_BITS_2MSB_SELECT,
                                                                  efes_condition_msb, &potential_2msb_min,
                                                                  &potential_2msb_max, &action_input_select,
                                                                  &key_select_found, &any_key_select_on_group));
        if (key_select_found == FALSE)
        {
            if (any_key_select_on_group)
            {
                
                if (potential_2msb_min < 0 || potential_2msb_max < 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "potential_2msb_min (%d) and potential_2msb_max (%d) "
                                 "cannot be negative as they were found.\r\n", potential_2msb_min, potential_2msb_max);
                }
                for (potential_2msb = potential_2msb_min; potential_2msb <= potential_2msb_max;
                     potential_2msb += DNX_FIELD_KEY_SELECT_BIT_GRANULARITY)
                {
                    char place_string[6 + 1];
                    if (potential_2msb > potential_2msb_min)
                    {
                        sal_strncat_s(legal_2msb_places, ", ", sizeof(legal_2msb_places));
                    }
                    sal_snprintf(place_string, sizeof(place_string), "%d", potential_2msb);
                    sal_strncat_s(legal_2msb_places, place_string, sizeof(legal_2msb_places));
                }
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Condition at msb %d is not a legal position. legal positions are (%s). "
                             "fg_id %d, context_id, %d, priority 0x%x.\r\n",
                             efes_condition_msb, legal_2msb_places, fg_id, context_id, action_priority);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "No available potential locations for condition bits on fg_id %d context_id %d. "
                             "Group is between %d and %d.\r\n",
                             fg_id, context_id,
                             group_position + fg_payload_min_offset, group_position + fg_payload_max_offset);
            }
        }
    }

    SHR_IF_ERR_EXIT(dnx_field_actions_calc_feses_single_efes_add(unit, field_stage, field_io, use_condition,
                                                                 action_input_select, efes_encoded_extraction_int,
                                                                 action_priority, template_shift + de_fg_lsb,
                                                                 fes_inst_info));

    
    if (field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        
        dnx_field_context_t impf2_context_id;
        dnx_field_context_t parent_context_id;
        uint8 is_allocated;

        for (impf2_context_id = 0;
             impf2_context_id < dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_contexts;
             impf2_context_id++)
        {
            
            SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated
                            (unit, DNX_FIELD_STAGE_IPMF2, impf2_context_id, &is_allocated));
            if (is_allocated)
            {
                SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                                (unit, DNX_FIELD_STAGE_IPMF2, impf2_context_id, &parent_context_id));
                if (parent_context_id == context_id)
                {
                    SHR_IF_ERR_EXIT(dnx_field_actions_fes_set
                                    (unit, DNX_FIELD_STAGE_IPMF2, fg_id, impf2_context_id, TRUE, TRUE, fes_inst_info));
                }
            }
        }
    }
    else
    {
        
        SHR_IF_ERR_EXIT(dnx_field_actions_fes_set(unit, field_stage, fg_id, context_id, TRUE, TRUE, fes_inst_info));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_efes_action_info_get_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    bcm_field_action_priority_t action_priority,
    int *use_condition_p,
    int *efes_condition_msb_p,
    dnx_field_actions_fes_single_add_conf_t efes_encoded_extraction[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES])
{
    unsigned int is_attached;
    dnx_field_fes_id_t fes_id;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(use_condition_p, _SHR_E_PARAM, "use_condition_p");
    SHR_NULL_CHECK(efes_condition_msb_p, _SHR_E_PARAM, "efes_condition_msb_p");
    SHR_NULL_CHECK(efes_encoded_extraction, _SHR_E_PARAM, "efes_encoded_extraction");

    DNX_FIELD_ACTION_PRIORITY_VERIFY(action_priority);
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);

    if (DNX_FIELD_ACTION_PRIORITY_IS_POSITION(action_priority) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "When adding an EFES, the priority must by position, that is created by "
                     "BCM_FIELD_ACTION_POSITION(). fg_id %d, context_id %d, priority 0x%x.\r\n",
                     fg_id, context_id, action_priority);
    }
    
    DNX_FIELD_ACTION_PRIORITY_POSITION_FES_ID_GET(fes_id, unit, field_stage, action_priority);

    SHR_IF_ERR_EXIT(dnx_field_group_is_context_id_on_arr(unit, fg_id, context_id, &is_attached));
    if (is_attached == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Field Group %d is not attached to context %d. \n", fg_id, context_id);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_efes_action_info_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    bcm_field_action_priority_t action_priority,
    int *use_condition_p,
    int *efes_condition_msb_p,
    dnx_field_actions_fes_single_add_conf_t efes_encoded_extraction[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES])
{
    dnx_field_stage_e field_stage;
    dnx_field_stage_e acr_stage;
    dnx_field_actions_fes_context_get_info_t context_fes_get_info;
    int fes_quartet_index;
    int found_added_fes;
    dnx_field_fes_id_t fes_id;
    int fest2msbcomb;
    int nof_progs_per_efes;
    int nof_bits_in_fes_action;
    int position_of_msb_on_key_select;
    int all_combinations_identical;
    int group_position;
    int group_position_in_key_select;
    int de_fg_lsb;
    dnx_field_group_type_e fg_type;
    int template_shift;
    dbal_enum_value_field_field_io_e field_io;
    int key_select_index;
    const dnx_data_field_efes_key_select_properties_t *field_io_key_select_info_p;
    dnx_field_action_type_t invalid_action_type;
    int max_nof_key_selects_per_field_io = dnx_data_field.efes.max_nof_key_selects_per_field_io_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    SHR_INVOKE_VERIFY_DNX(dnx_field_efes_action_info_get_verify(unit, field_stage, fg_id, context_id, action_priority,
                                                                use_condition_p, efes_condition_msb_p,
                                                                efes_encoded_extraction));

    dnx_field_actions_fes_single_add_conf_t_init(unit, efes_encoded_extraction);

    nof_progs_per_efes = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
    nof_bits_in_fes_action = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_bits_in_fes_action;
    position_of_msb_on_key_select =
        dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_bits_in_fes_key_select - 1;

    
    DNX_FIELD_ACTION_PRIORITY_POSITION_FES_ID_GET(fes_id, unit, field_stage, action_priority);

    
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id, &context_fes_get_info));

    
    if (context_fes_get_info.nof_fes_quartets >
        (sizeof(context_fes_get_info.fes_quartets) / sizeof(context_fes_get_info.fes_quartets[0])))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "context_fes_get_info.nof_fes_quartets (%d) is larger than maximum value (%d). "
                     "fg_id %d, context_id %d.\n",
                     context_fes_get_info.nof_fes_quartets,
                     (int) (sizeof(context_fes_get_info.fes_quartets) / sizeof(context_fes_get_info.fes_quartets[0])),
                     fg_id, context_id);
    }

    found_added_fes = FALSE;
    for (fes_quartet_index = 0; fes_quartet_index < context_fes_get_info.nof_fes_quartets; fes_quartet_index++)
    {
        if ((context_fes_get_info.fes_quartets[fes_quartet_index].fg_id == fg_id) &&
            (context_fes_get_info.fes_quartets[fes_quartet_index].place_in_fg >=
             DNX_ALGO_FIELD_ACTION_POST_ATTACH_FIRST_PLACE_IN_FG) &&
            (context_fes_get_info.fes_quartets[fes_quartet_index].fes_id == fes_id))
        {
            found_added_fes = TRUE;
            break;
        }
    }

    if (found_added_fes == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cound not find an added EFES for (fg_id %d, context_id %d priority 0x%x) .\n",
                     fg_id, context_id, action_priority);
    }
    else
    {
        
        SHR_IF_ERR_EXIT(dnx_field_map_get_invalid_action_type(unit, field_stage, &invalid_action_type));
        
        SHR_IF_ERR_EXIT(dnx_field_group_action_buffer_shift_get(unit, fg_id, &template_shift));
        SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
        if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
        {
            SHR_IF_ERR_EXIT(dnx_field_group_context_de_position_get(unit, fg_id, context_id, &de_fg_lsb));
        }
        else
        {
            de_fg_lsb = 0;
        }
        group_position = template_shift + de_fg_lsb;
        SHR_IF_ERR_EXIT(dnx_field_group_field_io_get(unit, fg_id, context_id, &field_io));
        if (field_stage == DNX_FIELD_STAGE_IPMF1)
        {
            acr_stage = DNX_FIELD_STAGE_IPMF2;
        }
        else
        {
            acr_stage = field_stage;
        }
        field_io_key_select_info_p = dnx_data_field.efes.key_select_properties_get(unit, acr_stage, field_io);
        for (key_select_index = 0; key_select_index < max_nof_key_selects_per_field_io; key_select_index++)
        {
            if (field_io_key_select_info_p->key_select[key_select_index] ==
                context_fes_get_info.fes_quartets[fes_quartet_index].key_select)
            {
                break;
            }
        }
        if (key_select_index == max_nof_key_selects_per_field_io)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Key select %d not found for field IO %d. "
                         "fg_id %d, context_id %d priority 0x%x.\n",
                         context_fes_get_info.fes_quartets[fes_quartet_index].key_select, field_io,
                         fg_id, context_id, action_priority);
        }
        group_position_in_key_select = group_position - field_io_key_select_info_p->lsb[key_select_index] +
            field_io_key_select_info_p->num_bits_not_on_key[key_select_index];
        
        all_combinations_identical = TRUE;
        for (fest2msbcomb = 1; fest2msbcomb < nof_progs_per_efes; fest2msbcomb++)
        {
            if (sal_memcmp(&context_fes_get_info.fes_quartets[fes_quartet_index].fes2msb_info[fest2msbcomb],
                           &context_fes_get_info.fes_quartets[fes_quartet_index].fes2msb_info[0],
                           sizeof(context_fes_get_info.fes_quartets[fes_quartet_index].fes2msb_info[0])))
            {
                all_combinations_identical = FALSE;
                break;
            }
        }
        if (all_combinations_identical)
        {
            (*use_condition_p) = FALSE;
        }
        else
        {
            (*use_condition_p) = TRUE;
            (*efes_condition_msb_p) = position_of_msb_on_key_select - group_position_in_key_select;
        }

        
        for (fest2msbcomb = 0; fest2msbcomb < nof_progs_per_efes; fest2msbcomb++)
        {
            if (context_fes_get_info.fes_quartets[fes_quartet_index].fes2msb_info[fest2msbcomb].action_type ==
                invalid_action_type)
            {
                efes_encoded_extraction[fest2msbcomb].dnx_action = DNX_FIELD_ACTION_INVALID;
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_field_map_action_type_to_predef_dnx_action
                                (unit, field_stage,
                                 context_fes_get_info.fes_quartets[fes_quartet_index].
                                 fes2msb_info[fest2msbcomb].action_type,
                                 &(efes_encoded_extraction[fest2msbcomb].dnx_action)));
                efes_encoded_extraction[fest2msbcomb].dont_use_valid_bit =
                    context_fes_get_info.fes_quartets[fes_quartet_index].fes2msb_info[fest2msbcomb].type;
                efes_encoded_extraction[fest2msbcomb].valid_bit_polarity =
                    context_fes_get_info.fes_quartets[fes_quartet_index].fes2msb_info[fest2msbcomb].polarity;
                efes_encoded_extraction[fest2msbcomb].mask =
                    context_fes_get_info.fes_quartets[fes_quartet_index].fes2msb_info[fest2msbcomb].mask;
                efes_encoded_extraction[fest2msbcomb].size =
                    nof_bits_in_fes_action -
                    context_fes_get_info.fes_quartets[fes_quartet_index].fes2msb_info[fest2msbcomb].valid_bits;
                if (efes_encoded_extraction[fest2msbcomb].dont_use_valid_bit)
                {
                    efes_encoded_extraction[fest2msbcomb].lsb =
                        context_fes_get_info.fes_quartets[fes_quartet_index].fes2msb_info[fest2msbcomb].shift -
                        group_position_in_key_select;
                }
                else
                {
                    efes_encoded_extraction[fest2msbcomb].lsb =
                        context_fes_get_info.fes_quartets[fes_quartet_index].fes2msb_info[fest2msbcomb].shift -
                        group_position_in_key_select + 1;
                }
            }
            if ((*use_condition_p) == FALSE)
            {
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_efes_action_remove_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    bcm_field_action_priority_t action_priority)
{
    unsigned int is_attached;
    dnx_field_fes_id_t fes_id;
    dnx_field_actions_fes_context_get_info_t context_fes_get_info;
    int fes_quartet_index;
    int found_added_fes;

    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_ACTION_PRIORITY_VERIFY(action_priority);
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);

    if (DNX_FIELD_ACTION_PRIORITY_IS_POSITION(action_priority) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "When adding an EFES, the priority must by position, that is created by "
                     "BCM_FIELD_ACTION_POSITION(). fg_id %d, context_id %d, priority 0x%x.\r\n",
                     fg_id, context_id, action_priority);
    }
    
    DNX_FIELD_ACTION_PRIORITY_POSITION_FES_ID_GET(fes_id, unit, field_stage, action_priority);

    SHR_IF_ERR_EXIT(dnx_field_group_is_context_id_on_arr(unit, fg_id, context_id, &is_attached));
    if (is_attached == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Field Group %d is not attached to context %d. \n", fg_id, context_id);
    }

    

    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id, &context_fes_get_info));

    
    if (context_fes_get_info.nof_fes_quartets >
        (sizeof(context_fes_get_info.fes_quartets) / sizeof(context_fes_get_info.fes_quartets[0])))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "context_fes_get_info.nof_fes_quartets (%d) is larger than maximum value (%d). "
                     "fg_id %d, context_id %d.\n",
                     context_fes_get_info.nof_fes_quartets,
                     (int) (sizeof(context_fes_get_info.fes_quartets) / sizeof(context_fes_get_info.fes_quartets[0])),
                     fg_id, context_id);
    }

    found_added_fes = FALSE;
    for (fes_quartet_index = 0; fes_quartet_index < context_fes_get_info.nof_fes_quartets; fes_quartet_index++)
    {
        if ((context_fes_get_info.fes_quartets[fes_quartet_index].fg_id == fg_id) &&
            (context_fes_get_info.fes_quartets[fes_quartet_index].place_in_fg >=
             DNX_ALGO_FIELD_ACTION_POST_ATTACH_FIRST_PLACE_IN_FG) &&
            (context_fes_get_info.fes_quartets[fes_quartet_index].fes_id) == fes_id)
        {
            found_added_fes = TRUE;
            break;
        }
    }

    if (found_added_fes == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cound not find an added EFES for (fg_id %d, context_id %d priority 0x%x) .\n",
                     fg_id, context_id, action_priority);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_efes_action_remove(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    bcm_field_action_priority_t action_priority)
{
    dnx_field_stage_e field_stage;
    dnx_field_fes_id_t fes_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));

    SHR_INVOKE_VERIFY_DNX(dnx_field_efes_action_remove_verify(unit, field_stage, fg_id, context_id, action_priority));

    
    DNX_FIELD_ACTION_PRIORITY_POSITION_FES_ID_GET(fes_id, unit, field_stage, action_priority);

    
    if (field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        
        dnx_field_context_t impf2_context_id;
        dnx_field_context_t parent_context_id;
        uint8 is_allocated;

        for (impf2_context_id = 0;
             impf2_context_id < dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_contexts;
             impf2_context_id++)
        {
            
            SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated
                            (unit, DNX_FIELD_STAGE_IPMF2, impf2_context_id, &is_allocated));
            if (is_allocated)
            {
                SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                                (unit, DNX_FIELD_STAGE_IPMF2, impf2_context_id, &parent_context_id));
                if (parent_context_id == context_id)
                {
                    SHR_IF_ERR_EXIT(dnx_field_actions_fes_detach
                                    (unit, DNX_FIELD_STAGE_IPMF2, fg_id, impf2_context_id, fes_id));
                }
            }
        }
    }
    else
    {
        
        SHR_IF_ERR_EXIT(dnx_field_actions_fes_detach(unit, field_stage, fg_id, context_id, fes_id));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_efes_action_context_create_fes_update(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_t cascaded_from)
{
    dnx_field_actions_fes_context_get_info_t context_fes_get_info;
    int fes_quartet_index;
    dnx_field_group_t fg_id;
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    int fes_2msb_ind;

    SHR_FUNC_INIT_VARS(unit);

    
    if (stage == DNX_FIELD_STAGE_IPMF2)
    {
        
        if (context_id == cascaded_from)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Attempting to allocate an iPMF2 context for default cascading (context id %d).\r\n",
                         context_id);
        }
        
        SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get
                        (unit, DNX_FIELD_STAGE_IPMF1, cascaded_from, &context_fes_get_info));

        
        if (context_fes_get_info.nof_fes_quartets >
            (sizeof(context_fes_get_info.fes_quartets) / sizeof(context_fes_get_info.fes_quartets[0])))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "context_fes_get_info.nof_fes_quartets (%d) is larger than maximum value (%d). "
                         "context_id %d.\n", context_fes_get_info.nof_fes_quartets,
                         (int) (sizeof(context_fes_get_info.fes_quartets) /
                                sizeof(context_fes_get_info.fes_quartets[0])), context_id);
        }

        for (fes_quartet_index = 0; fes_quartet_index < context_fes_get_info.nof_fes_quartets; fes_quartet_index++)
        {
            
            if (context_fes_get_info.fes_quartets[fes_quartet_index].place_in_fg >=
                DNX_ALGO_FIELD_ACTION_POST_ATTACH_FIRST_PLACE_IN_FG)
            {
                fg_id = context_fes_get_info.fes_quartets[fes_quartet_index].fg_id;
                SHR_IF_ERR_EXIT(dnx_field_actions_fes_info_t_init(unit, DNX_FIELD_STAGE_IPMF2, fes_inst_info));
                for (fes_2msb_ind = 0; fes_2msb_ind < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES;
                     fes_2msb_ind++)
                {
                    fes_inst_info[0].common_info.fes2msb_info[fes_2msb_ind].action_type =
                        context_fes_get_info.fes_quartets[fes_quartet_index].fes2msb_info[fes_2msb_ind].action_type;
                    fes_inst_info[0].common_info.fes2msb_info[fes_2msb_ind].valid_bits =
                        context_fes_get_info.fes_quartets[fes_quartet_index].fes2msb_info[fes_2msb_ind].valid_bits;
                    fes_inst_info[0].common_info.fes2msb_info[fes_2msb_ind].type =
                        context_fes_get_info.fes_quartets[fes_quartet_index].fes2msb_info[fes_2msb_ind].type;
                    fes_inst_info[0].common_info.fes2msb_info[fes_2msb_ind].polarity =
                        context_fes_get_info.fes_quartets[fes_quartet_index].fes2msb_info[fes_2msb_ind].polarity;
                    fes_inst_info[0].alloc_info.fes2msb_info[fes_2msb_ind].required_mask =
                        context_fes_get_info.fes_quartets[fes_quartet_index].fes2msb_info[fes_2msb_ind].mask;
                    fes_inst_info[0].common_info.fes2msb_info[fes_2msb_ind].shift =
                        context_fes_get_info.fes_quartets[fes_quartet_index].fes2msb_info[fes_2msb_ind].shift;
                }
                fes_inst_info[0].write_info.key_select =
                    context_fes_get_info.fes_quartets[fes_quartet_index].key_select;
                fes_inst_info[0].alloc_info.priority = context_fes_get_info.fes_quartets[fes_quartet_index].priority;
                SHR_IF_ERR_EXIT(dnx_field_actions_fes_set
                                (unit, DNX_FIELD_STAGE_IPMF2, fg_id, context_id, TRUE, TRUE, fes_inst_info));
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "No stage other than iPMF2 has cascaded contexts and inter stage shared FESes. "
                     "Attempted to update added FESes for stage %s context Id %d.\r\n",
                     dnx_field_stage_text(unit, stage), context_id);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_efes_action_context_destroy_fes_update(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_t cascaded_from)
{
    dnx_field_actions_fes_context_get_info_t context_fes_get_info;
    int fes_quartet_index;
    dnx_field_group_t fg_id;
    dnx_field_fes_id_t fes_id;

    SHR_FUNC_INIT_VARS(unit);

    
    if (stage == DNX_FIELD_STAGE_IPMF2)
    {
        
        if (context_id == cascaded_from)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Attempting to allocate an iPMF2 context for default cascading (context id %d).\r\n",
                         context_id);
        }
        
        SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get
                        (unit, DNX_FIELD_STAGE_IPMF1, cascaded_from, &context_fes_get_info));

        
        if (context_fes_get_info.nof_fes_quartets >
            (sizeof(context_fes_get_info.fes_quartets) / sizeof(context_fes_get_info.fes_quartets[0])))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "context_fes_get_info.nof_fes_quartets (%d) is larger than maximum value (%d). "
                         "context_id %d.\n", context_fes_get_info.nof_fes_quartets,
                         (int) (sizeof(context_fes_get_info.fes_quartets) /
                                sizeof(context_fes_get_info.fes_quartets[0])), context_id);
        }

        for (fes_quartet_index = 0; fes_quartet_index < context_fes_get_info.nof_fes_quartets; fes_quartet_index++)
        {
            
            if (context_fes_get_info.fes_quartets[fes_quartet_index].place_in_fg >=
                DNX_ALGO_FIELD_ACTION_POST_ATTACH_FIRST_PLACE_IN_FG)
            {
                fg_id = context_fes_get_info.fes_quartets[fes_quartet_index].fg_id;
                fes_id = context_fes_get_info.fes_quartets[fes_quartet_index].fes_id;

                SHR_IF_ERR_EXIT(dnx_field_actions_fes_detach(unit, DNX_FIELD_STAGE_IPMF1, fg_id, context_id, fes_id));
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "No stage other than iPMF2 has cascaded contexts and inter stage shared FESes. "
                     "Attempted to update added FESes for stage %s context Id %d.\r\n",
                     dnx_field_stage_text(unit, stage), context_id);
    }

exit:
    SHR_FUNC_EXIT;
}





shr_error_e
dnx_field_actions_fem_key_select_get(
    int unit,
    dbal_enum_value_field_field_io_e actions_block,
    unsigned int action_size,
    unsigned int action_lsb,
    dbal_enum_value_field_field_pmf_a_fem_key_select_e * action_input_select_p,
    unsigned int *required_shift_p,
    int *found_p)
{
    unsigned int key_select_ndx;
    int found;
    int required_shift;
    const dnx_data_field_fem_key_select_properties_t *field_io_key_select_info_p;
    int max_nof_key_selects_per_field_io = dnx_data_field.fem.max_nof_key_selects_per_field_io_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(action_input_select_p, _SHR_E_PARAM, "action_input_select_p");
    SHR_NULL_CHECK(required_shift_p, _SHR_E_PARAM, "required_shift_p");
    SHR_NULL_CHECK(found_p, _SHR_E_PARAM, "found_p");

    *found_p = FALSE;
    found = FALSE;
    field_io_key_select_info_p = dnx_data_field.fem.key_select_properties_get(unit, actions_block);
    for (key_select_ndx = 0; key_select_ndx < max_nof_key_selects_per_field_io; key_select_ndx++)
    {
        if (field_io_key_select_info_p->num_bits[key_select_ndx] != 0)
        {
            required_shift = action_lsb - field_io_key_select_info_p->lsb[key_select_ndx];
            if (required_shift >= 0)
            {
                
                if ((action_size + required_shift) <= field_io_key_select_info_p->num_bits[key_select_ndx])
                {
                    
                    *action_input_select_p = field_io_key_select_info_p->key_select[key_select_ndx];
                    *required_shift_p =
                        required_shift + field_io_key_select_info_p->num_bits_not_on_key[key_select_ndx];
                    found = TRUE;
                    break;
                }
            }
        }
    }
    *found_p = found;
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_actions_fem_phys_to_log_bit_source(
    int unit,
    dnx_field_fem_bit_val_t fem_bit_val,
    dnx_field_fem_bit_info_t * fem_bit_info_p)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fem_bit_info_p, _SHR_E_PARAM, "fem_bit_info_p");

    if (fem_bit_val &
        SAL_FROM_BIT(dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_bits_in_fem_field_select))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Physical value of FIELD_FEM_BIT_VAL (0x%08X) is illegal. Must be smaller than %d\r\n",
                     (uint32) fem_bit_val,
                     SAL_BIT(dnx_data_field.stage.
                             stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_bits_in_fem_field_select));
    }
    if ((fem_bit_val & DNX_FIELD_FEM_MASK_FOR_MAP_TYPE) == DNX_FIELD_FEM_MAP_TYPE_KEY_SELECT)
    {
        
        fem_bit_info_p->fem_bit_format = DNX_FIELD_FEM_BIT_FORMAT_FROM_KEY_SELECT;
        fem_bit_info_p->fem_bit_value.bit_on_key_select = fem_bit_val & SAL_UPTO_BIT(DNX_FIELD_FEM_BIT_FOR_MAP_TYPE);
    }
    else if ((fem_bit_val & DNX_FIELD_FEM_MASK_FOR_MAP_DATA) == DNX_FIELD_FEM_MASK_OF_MAP_DATA)
    {
        
        fem_bit_info_p->fem_bit_format = DNX_FIELD_FEM_BIT_FORMAT_FROM_MAP_DATA;
        fem_bit_info_p->fem_bit_value.bit_on_map_data =
            fem_bit_val & SAL_UPTO_BIT(dnx_data_field.stage.
                                       stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->log_nof_bits_in_fem_map_data_field);
    }
    else
    {
        
        fem_bit_info_p->fem_bit_format = DNX_FIELD_FEM_BIT_FORMAT_FROM_THIS_FIELD;
        fem_bit_info_p->fem_bit_value.bit_on_map_data =
            fem_bit_val & SAL_UPTO_BIT(DNX_FIELD_FEM_NUM_VALUES_ON_BIT_VALUE);
    }
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_actions_fem_log_to_phys_bit_source(
    int unit,
    dnx_field_fem_bit_info_t * fem_bit_info_p,
    dnx_field_fem_bit_val_t * fem_bit_val_p)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fem_bit_info_p, _SHR_E_PARAM, "fem_bit_info_p");
    SHR_NULL_CHECK(fem_bit_val_p, _SHR_E_PARAM, "fem_bit_val_p");

    switch (fem_bit_info_p->fem_bit_format)
    {
        case DNX_FIELD_FEM_BIT_FORMAT_INVALID:
        {
            
            *fem_bit_val_p = 0 | DNX_FIELD_FEM_MASK_OF_BIT_VALUE;
            break;
        }
        case DNX_FIELD_FEM_BIT_FORMAT_FROM_KEY_SELECT:
        {
            
            if (fem_bit_info_p->fem_bit_value.bit_on_key_select >=
                dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_key_select)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Fem bit_on_key_select (%d) is illegal. Must be smaller than %d\r\n",
                             fem_bit_info_p->fem_bit_value.bit_on_key_select,
                             dnx_data_field.stage.stage_info_get(unit,
                                                                 DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_key_select);
            }
            *fem_bit_val_p = fem_bit_info_p->fem_bit_value.bit_on_key_select;
            break;
        }
        case DNX_FIELD_FEM_BIT_FORMAT_FROM_MAP_DATA:
        {
            
            if (fem_bit_info_p->fem_bit_value.bit_on_map_data >=
                dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_map_data_field)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Fem bit_on_map_data (%d) is illegal. Must be smaller than %d\r\n",
                             fem_bit_info_p->fem_bit_value.bit_on_map_data,
                             dnx_data_field.stage.stage_info_get(unit,
                                                                 DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_map_data_field);
            }
            *fem_bit_val_p = fem_bit_info_p->fem_bit_value.bit_on_map_data | DNX_FIELD_FEM_MASK_OF_MAP_DATA;
            break;
        }
        case DNX_FIELD_FEM_BIT_FORMAT_FROM_THIS_FIELD:
        {
            
            if (fem_bit_info_p->fem_bit_value.bit_on_map_data >= DNX_FIELD_FEM_NUM_VALUES_ON_BIT_VALUE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Fem bit_on_map_data (%d) is illegal. Must be smaller than %d\r\n",
                             fem_bit_info_p->fem_bit_value.bit_on_map_data, DNX_FIELD_FEM_NUM_VALUES_ON_BIT_VALUE);
            }
            *fem_bit_val_p = fem_bit_info_p->fem_bit_value.bit_value | DNX_FIELD_FEM_MASK_OF_BIT_VALUE;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Fem bit format (%d) is illegal. Must be between %d and %d\r\n",
                         fem_bit_info_p->fem_bit_format, DNX_FIELD_FEM_BIT_FORMAT_FIRST, DNX_FIELD_FEM_BIT_FORMAT_NUM);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_single_fem_prg_sw_state_init(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program)
{
    dnx_field_fg_id_info_t fg_id_info;
    dnx_field_fem_map_index_t fem_map_index, fem_map_index_max;
    dnx_field_action_t fem_encoded_action;
    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);
    DNX_FIELD_FEM_PROGRAM_VERIFY(unit, fem_program);

    fg_id_info.fg_id = DNX_FIELD_GROUP_INVALID;
    fg_id_info.input_offset = 0;
    fg_id_info.second_fg_id = DNX_FIELD_GROUP_INVALID;
    fg_id_info.ignore_actions = DNX_FIELD_IGNORE_ALL_ACTIONS;

    SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.fg_id.set(unit, fem_id, fem_program, fg_id_info.fg_id));
    SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.input_offset.set(unit, fem_id, fem_program, fg_id_info.input_offset));
    SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.second_fg_id.set(unit, fem_id, fem_program, fg_id_info.second_fg_id));
    SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.ignore_actions.set(unit, fem_id, fem_program, fg_id_info.ignore_actions));

    fem_encoded_action = DNX_FIELD_ACTION_INVALID;
    fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
    for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
    {
        SHR_IF_ERR_EXIT(FEM_INFO_FEM_ENCODED_ACTIONS.set(unit, fem_id, fem_program, fem_map_index, fem_encoded_action));
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_fem_is_any_fem_occupied_on_fg(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    dnx_field_group_t second_fg_id,
    uint8 active_actions,
    int *none_occupied_p)
{
    dnx_field_fem_id_t fem_id_index, fem_id_max;
    dnx_field_fem_program_t fem_program_index, fem_program_max;
    uint8 is_allocated;
    uint8 active_actions_mask;

    SHR_FUNC_INIT_VARS(unit);
    if (fg_id != DNX_FIELD_GROUP_INVALID)
    {
        DNX_FIELD_FG_ID_VERIFY(unit, fg_id);
    }
    if (second_fg_id != DNX_FIELD_GROUP_INVALID)
    {
        DNX_FIELD_FG_ID_VERIFY(unit, second_fg_id);
    }
    *none_occupied_p = TRUE;
    {
        
        SHR_IF_ERR_EXIT(FEM_INFO.is_allocated(unit, &is_allocated));
        if (!is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "FEM_INFO is not yet allocated on SW state. Can not search occupation of FEMs.\r\n");
        }
    }
    fem_id_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
    fem_program_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs;
    active_actions_mask = SAL_UPTO_BIT(fem_program_max);
    for (fem_id_index = 0; fem_id_index < fem_id_max; fem_id_index++)
    {
        int mark_as_occupied;
        int main_fg_found;
        
        SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.is_allocated(unit, fem_id_index, &is_allocated));
        if (!is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "FEM_INFO_FG_ID_INFO is not yet allocated (on SW state) for fem_id %d.\r\n"
                         "Can not search occupation of FEMs.\r\n", fem_id_index);
        }
        if (context_id != DNX_FIELD_CONTEXT_ID_INVALID)
        {
            
            unsigned int using_fem;
            SHR_IF_ERR_EXIT(dnx_field_fem_is_context_using(unit, fem_id_index, context_id, fg_id, &using_fem));
            if (using_fem == FALSE)
            {
                continue;
            }
        }
        main_fg_found = FALSE;
        for (fem_program_index = 0; fem_program_index < fem_program_max; fem_program_index++)
        {
            dnx_field_group_t sw_fg_id;

            mark_as_occupied = FALSE;
            SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.fg_id.get(unit, fem_id_index, fem_program_index, &sw_fg_id));
            if (sw_fg_id == DNX_FIELD_GROUP_INVALID)
            {
                
                continue;
            }
            
            if (fg_id != DNX_FIELD_GROUP_INVALID)
            {
                
                if (fg_id != sw_fg_id)
                {
                    continue;
                }
                main_fg_found = TRUE;
            }
            else
            {
                mark_as_occupied = TRUE;
            }
            if (second_fg_id != DNX_FIELD_GROUP_INVALID)
            {
                
                DNX_FIELD_FG_ID_VERIFY(unit, second_fg_id);
                SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.second_fg_id.get(unit, fem_id_index, fem_program_index, &sw_fg_id));
                if (second_fg_id != sw_fg_id)
                {
                    continue;
                }
            }
            else
            {
                mark_as_occupied = TRUE;
            }
            if (active_actions != DNX_FIELD_IGNORE_ALL_ACTIONS)
            {
                
                uint8 sw_ignore_actions, sw_active_actions, input_active_actions;

                SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.
                                ignore_actions.get(unit, fem_id_index, fem_program_index, &sw_ignore_actions));
                sw_active_actions = (~sw_ignore_actions & active_actions_mask);
                input_active_actions = (active_actions & active_actions_mask);
                
                if ((input_active_actions & sw_active_actions) != input_active_actions)
                {
                    continue;
                }
            }
            else
            {
                mark_as_occupied = TRUE;
            }
            if (mark_as_occupied != FALSE)
            {
                *none_occupied_p = FALSE;
                SHR_EXIT();
            }
        }
        if ((context_id != DNX_FIELD_CONTEXT_ID_INVALID) && (main_fg_found == FALSE))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "FEM ID %d, context_id %d field group %d found on context but not in sw state.",
                         fem_id_index, context_id, fg_id);
        }
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_all_fems_sw_state_init(
    int unit)
{
    dnx_field_fem_id_t fem_id_index, fem_id_max;
    dnx_field_fem_program_t fem_program_index, fem_program_max;
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);

    {
        
        SHR_IF_ERR_EXIT(FEM_INFO.is_allocated(unit, &is_allocated));
        if (!is_allocated)
        {
            SHR_IF_ERR_EXIT(FEM_INFO.alloc(unit));
        }
    }
    fem_id_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
    fem_program_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs;
    for (fem_id_index = 0; fem_id_index < fem_id_max; fem_id_index++)
    {
        
        SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.is_allocated(unit, fem_id_index, &is_allocated));
        if (!is_allocated)
        {
            SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.alloc(unit, fem_id_index));
        }
        for (fem_program_index = 0; fem_program_index < fem_program_max; fem_program_index++)
        {
            SHR_IF_ERR_EXIT(FEM_INFO_FEM_ENCODED_ACTIONS.is_allocated
                            (unit, fem_id_index, fem_program_index, &is_allocated));
            if (!is_allocated)
            {
                SHR_IF_ERR_EXIT(FEM_INFO_FEM_ENCODED_ACTIONS.alloc(unit, fem_id_index, fem_program_index));
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}



shr_error_e
dnx_field_pmf_fem_context_entry_t_init(
    int unit,
    dnx_field_pmf_fem_context_entry_t * struct_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(struct_p, _SHR_E_PARAM, "struct_p");
    
    sal_memset(struct_p, 0, sizeof(*struct_p));
    struct_p->pmf_fem_program.replace_lsb_flag = DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_FLAG_REPLACE_LSB_IS_NOT_ACTIVE;
    
    struct_p->pmf_fem_program.fem_key_select = DNX_FIELD_INVALID_FEM_KEY_SELECT;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_fem_action_entry_t_init(
    int unit,
    dnx_field_fem_action_entry_t * struct_p)
{
    unsigned int fem_action_bit_index, fem_action_bit_max;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(struct_p, _SHR_E_PARAM, "struct_p");

    
    sal_memset(struct_p, 0x0, sizeof(*struct_p));
    struct_p->fem_action = DBAL_ENUM_FVAL_IPMF1_ACTION_INVALID;
    fem_action_bit_max = sizeof(struct_p->fem_bit_info) / sizeof(struct_p->fem_bit_info[0]);
    for (fem_action_bit_index = 0; fem_action_bit_index < fem_action_bit_max; fem_action_bit_index++)
    {
        struct_p->fem_bit_info[fem_action_bit_index].fem_bit_format = DNX_FIELD_FEM_BIT_FORMAT_INVALID;
        
        struct_p->fem_bit_info[fem_action_bit_index].fem_bit_value.bit_value = 0;
    }
    struct_p->fem_adder = 0;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_fem_condition_entry_t_init(
    int unit,
    dnx_field_fem_condition_entry_t * struct_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(struct_p, _SHR_E_PARAM, "struct_p");
    
    struct_p->fem_map_index = 0;
    struct_p->fem_map_data = 0;
    
    struct_p->fem_action_valid =
        (dnx_field_fem_action_valid_t) DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_NOT_ACTIVE;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_fem_condition_entry_t_array_init(
    int unit,
    dnx_field_fem_condition_entry_t fem_condition_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION])
{
    dnx_field_fem_condition_t fem_condition_index;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fem_condition_entry, _SHR_E_PARAM, "fem_condition_entry");

    
    for (fem_condition_index = 0; fem_condition_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION;
         fem_condition_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_fem_condition_entry_t_init(unit, &fem_condition_entry[fem_condition_index]));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_fem_is_fg_owner(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_group_t fg_id,
    dnx_field_fem_program_t * fem_program_p)
{
    SHR_FUNC_INIT_VARS(unit);
    
    *fem_program_p = DNX_FIELD_FEM_PROGRAM_INVALID;
    {
        uint8 is_allocated;
        dnx_field_stage_e field_stage;
        
        DNX_FIELD_FG_ID_VERIFY(unit, fg_id);
        SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
        if (!is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Field Group %d does not exist.\r\n", fg_id);
        }
        
        SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
        if ((field_stage != DNX_FIELD_STAGE_IPMF1) && (field_stage != DNX_FIELD_STAGE_IPMF2))
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Specified 'stage' (%d,%s), for Field Group %d, is not relevant for FEMs. (It should be IPMF1 or IPMF2). %s\r\n",
                         field_stage, dnx_field_stage_e_get_name(field_stage), fg_id, EMPTY);
            SHR_EXIT();
        }
        
        DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);
    }
    {
        dnx_field_fem_program_t fem_program_index, fem_program_max;
        dnx_field_group_t sw_fg_id;

        
        fem_program_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs;
        for (fem_program_index = FIRST_AVAILABLE_FEM_PROGRAM; fem_program_index < fem_program_max; fem_program_index++)
        {
            SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.fg_id.get(unit, fem_id, fem_program_index, &sw_fg_id));
            if (sw_fg_id == fg_id)
            {
                *fem_program_p = fem_program_index;
                
                break;
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_fem_get_next_fg_owner(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_fem_id_t * fem_id_p,
    dnx_field_fem_program_t * fem_program_p)
{
    dnx_field_fem_id_t fem_id_index, fem_id_max;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fem_id_p, _SHR_E_PARAM, "fem_id_p");
    SHR_NULL_CHECK(fem_program_p, _SHR_E_PARAM, "fem_program_p");
    
    if (*fem_id_p == DNX_FIELD_FEM_ID_INVALID)
    {
        fem_id_index = 0;
    }
    else
    {
        fem_id_index = *fem_id_p + 1;
    }
    
    *fem_id_p = DNX_FIELD_FEM_ID_INVALID;
    
    *fem_program_p = DNX_FIELD_FEM_PROGRAM_INVALID;
    fem_id_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
    {
        uint8 is_allocated;
        dnx_field_stage_e field_stage;
        
        DNX_FIELD_FG_ID_VERIFY(unit, fg_id);
        SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
        if (!is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Field Group %d does not exist.\r\n", fg_id);
        }
        
        SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
        if ((field_stage != DNX_FIELD_STAGE_IPMF1) && (field_stage != DNX_FIELD_STAGE_IPMF2))
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Specified 'stage' (%d,%s), for Field Group %d, is not relevant for FEMs. (It should be IPMF1 or IPMF2). %s\r\n",
                         field_stage, dnx_field_stage_e_get_name(field_stage), fg_id, EMPTY);
            SHR_EXIT();
        }
        if (fem_id_index >= fem_id_max)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Specified starter 'fem_id' (%d), is larger than maximal 'fem_id' (%d). Return with 'no match'. %s%s\r\n",
                         fem_id_index, fem_id_max - 1, EMPTY, EMPTY);
            SHR_EXIT();
        }
    }
    {
        
        for (; fem_id_index < fem_id_max; fem_id_index++)
        {
            SHR_IF_ERR_EXIT(dnx_field_fem_is_fg_owner(unit, fem_id_index, fg_id, fem_program_p));
            if (*fem_program_p != DNX_FIELD_FEM_PROGRAM_INVALID)
            {
                
                *fem_id_p = fem_id_index;
                SHR_EXIT();
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_fem_is_fg_secondary(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_group_t second_fg_id,
    dnx_field_fem_program_t * fem_program_p)
{
    SHR_FUNC_INIT_VARS(unit);
    
    *fem_program_p = DNX_FIELD_FEM_PROGRAM_INVALID;
    {
        uint8 is_allocated;
        dnx_field_stage_e field_stage;
        
        DNX_FIELD_FG_ID_VERIFY(unit, second_fg_id);
        SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, second_fg_id, &is_allocated));
        if (!is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Field Group %d does not exist.\r\n", second_fg_id);
        }
        
        SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, second_fg_id, &field_stage));
        if ((field_stage != DNX_FIELD_STAGE_IPMF1) && (field_stage != DNX_FIELD_STAGE_IPMF2))
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Specified 'stage' (%d,%s), for Field Group %d, is not relevant for FEMs. (It should be IPMF1 or IPMF2). %s\r\n",
                         field_stage, dnx_field_stage_e_get_name(field_stage), second_fg_id, EMPTY);
            SHR_EXIT();
        }
        
        DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);
    }
    {
        dnx_field_fem_program_t fem_program_index, fem_program_max;
        dnx_field_group_t sw_fg_id;

        
        fem_program_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs;
        for (fem_program_index = FIRST_AVAILABLE_FEM_PROGRAM; fem_program_index < fem_program_max; fem_program_index++)
        {
            SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.second_fg_id.get(unit, fem_id, fem_program_index, &sw_fg_id));
            if (sw_fg_id == second_fg_id)
            {
                *fem_program_p = fem_program_index;
                
                break;
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_fem_is_fg_secondary_on_any(
    int unit,
    dnx_field_group_t second_fg_id,
    dnx_field_fem_id_t * fem_id_p)
{
    dnx_field_fem_id_t fem_id_index, fem_id_max;
    dnx_field_fem_program_t fem_program;

    SHR_FUNC_INIT_VARS(unit);
    *fem_id_p = DNX_FIELD_FEM_ID_INVALID;
    {
        
        dnx_field_stage_e field_stage;

        SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, second_fg_id, &field_stage));
        if ((field_stage != DNX_FIELD_STAGE_IPMF1) && (field_stage != DNX_FIELD_STAGE_IPMF2))
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "\r\n"
                         "Specified 'stage' (%d,%s), for Field Group %d, is not relevant for FEMs.\r\n"
                         "==> This field group is not 'secondary'. %s\r\n",
                         field_stage, dnx_field_stage_e_get_name(field_stage), second_fg_id, EMPTY);
            SHR_EXIT();
        }
    }
    fem_id_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
    
    for (fem_id_index = 0; fem_id_index < fem_id_max; fem_id_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_fem_is_fg_secondary(unit, fem_id_index, second_fg_id, &fem_program));
        if (fem_program != DNX_FIELD_FEM_PROGRAM_INVALID)
        {
            
            *fem_id_p = fem_id_index;
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_fem_num_action_bits_verify(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_action_t dnx_encoded_fem_action,
    dnx_field_fem_action_entry_t * fem_action_entry_p)
{
    unsigned int bit_in_fem_action_index, bit_in_fem_action_min, bit_in_fem_action_max;
    unsigned int bit_in_fem_action_count;
    dnx_field_fem_bit_format_e fem_bit_format;
    unsigned int action_mapped_size;
    dnx_field_stage_e field_stage;

    SHR_FUNC_INIT_VARS(unit);
    
    DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);

    
    field_stage = DNX_ACTION_STAGE(dnx_encoded_fem_action);
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, field_stage, dnx_encoded_fem_action, &action_mapped_size));

    bit_in_fem_action_count = 0;

    
    bit_in_fem_action_min =
        dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_action_fems_0_1;
    bit_in_fem_action_max =
        dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_action_fems_2_15;
    if (fem_id < dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_fems_with_short_action)
    {
        
        for (bit_in_fem_action_index = 0; bit_in_fem_action_index < bit_in_fem_action_min; bit_in_fem_action_index++)
        {
            fem_bit_format = fem_action_entry_p->fem_bit_info[bit_in_fem_action_index].fem_bit_format;
            if ((((int) fem_bit_format >= (int) (DNX_FIELD_FEM_BIT_FORMAT_NUM)) ||
                 ((int) fem_bit_format < (int) (DNX_FIELD_FEM_BIT_FORMAT_FIRST))) &&
                ((unsigned int) fem_bit_format != (unsigned int) (DNX_FIELD_FEM_BIT_FORMAT_INVALID)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "\r\n"
                             "This is 'FEM id' %d which has %d meaningful bits. However, bit %d is NOT marked properly on 'fem_bit_format'. Quit.\r\n",
                             fem_id, bit_in_fem_action_min, bit_in_fem_action_index);
            }
        }
        for (bit_in_fem_action_index = 0; bit_in_fem_action_index < bit_in_fem_action_min; bit_in_fem_action_index++)
        {
            fem_bit_format = fem_action_entry_p->fem_bit_info[bit_in_fem_action_index].fem_bit_format;
            if (((unsigned int) fem_bit_format == (unsigned int) (DNX_FIELD_FEM_BIT_FORMAT_INVALID)))
            {
                continue;
            }
            
            bit_in_fem_action_count++;
        }
        if (bit_in_fem_action_count > bit_in_fem_action_min)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "This is 'FEM id' %d which has only %d bits. However, the provided valid bits %d are more than allowed for this 'FEM id' (%d). Quit.\r\n",
                         fem_id, bit_in_fem_action_min, bit_in_fem_action_count, bit_in_fem_action_min);
        }
        if (bit_in_fem_action_count > action_mapped_size)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "This is 'FEM id' %d which has only %d bits. However, the provided valid bits %d are more than size of the provided action (%d). Quit.\r\n",
                         fem_id, bit_in_fem_action_min, bit_in_fem_action_count, action_mapped_size);
        }
        for (bit_in_fem_action_index = bit_in_fem_action_count;
             bit_in_fem_action_index < bit_in_fem_action_max; bit_in_fem_action_index++)
        {
            if (fem_action_entry_p->fem_bit_info[bit_in_fem_action_index].fem_bit_format !=
                DNX_FIELD_FEM_BIT_FORMAT_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "\r\n"
                             "This is 'FEM id' %d which has only %d bits. However, bit %d is NOT marked as 'invalid' on 'fem_bit_format' (%d). Quit.\r\n",
                             fem_id, bit_in_fem_action_min, bit_in_fem_action_index,
                             fem_action_entry_p->fem_bit_info[bit_in_fem_action_index].fem_bit_format);
            }
            if (fem_action_entry_p->fem_bit_info[bit_in_fem_action_index].fem_bit_value.bit_descriptor_value != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "\r\n"
                             "This is 'FEM id' %d which has only %d bits. Bit %d is NOT marked as 'invalid',\r\n"
                             "as it should, but its 'descroptor_value' is not '0' (%d). Quit.\r\n",
                             fem_id, bit_in_fem_action_min, bit_in_fem_action_index,
                             fem_action_entry_p->fem_bit_info[bit_in_fem_action_index].
                             fem_bit_value.bit_descriptor_value);
            }
        }
        if (fem_action_entry_p->fem_adder != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "This is 'FEM id' %d which has only %d bits and no 'adder'. However, 'adder' (%d) is NOT zero. Quit.\r\n",
                         fem_id, bit_in_fem_action_min, fem_action_entry_p->fem_adder);
        }
    }
    else
    {
        for (bit_in_fem_action_index = 0; bit_in_fem_action_index < bit_in_fem_action_max; bit_in_fem_action_index++)
        {
            fem_bit_format = fem_action_entry_p->fem_bit_info[bit_in_fem_action_index].fem_bit_format;
            if ((((unsigned int) fem_bit_format >= (unsigned int) (DNX_FIELD_FEM_BIT_FORMAT_NUM)) ||
                 ((int) fem_bit_format < (int) (DNX_FIELD_FEM_BIT_FORMAT_FIRST))) &&
                ((unsigned int) fem_bit_format != (unsigned int) (DNX_FIELD_FEM_BIT_FORMAT_INVALID)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "\r\n"
                             "This is 'FEM id' %d which has %d meaningful bits. However, bit %d is NOT marked properly on 'fem_bit_format'. Quit.\r\n",
                             fem_id, bit_in_fem_action_max, bit_in_fem_action_index);
            }
        }
        for (bit_in_fem_action_index = 0; bit_in_fem_action_index < bit_in_fem_action_max; bit_in_fem_action_index++)
        {
            fem_bit_format = fem_action_entry_p->fem_bit_info[bit_in_fem_action_index].fem_bit_format;
            if (((unsigned int) fem_bit_format == (unsigned int) (DNX_FIELD_FEM_BIT_FORMAT_INVALID)))
            {
                continue;
            }
            
            bit_in_fem_action_count++;
        }
        if (bit_in_fem_action_count > bit_in_fem_action_max)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "This is 'FEM id' %d which has only %d bits. However, the provided valid bits %d are more than allowed for this 'FEM id' (%d). Quit.\r\n",
                         fem_id, bit_in_fem_action_max, bit_in_fem_action_count, bit_in_fem_action_max);
        }
        if (bit_in_fem_action_count > action_mapped_size)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "This is 'FEM id' %d which has only %d bits. However, the provided valid bits %d are more than size of the provided action (%d). Quit.\r\n",
                         fem_id, bit_in_fem_action_min, bit_in_fem_action_count, action_mapped_size);
        }
        for (bit_in_fem_action_index = bit_in_fem_action_count;
             bit_in_fem_action_index < bit_in_fem_action_max; bit_in_fem_action_index++)
        {
            if (fem_action_entry_p->fem_bit_info[bit_in_fem_action_index].fem_bit_format !=
                DNX_FIELD_FEM_BIT_FORMAT_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "\r\n"
                             "This is 'FEM id' %d which has only %d bits. However, bit %d is NOT marked as 'invalid' on 'fem_bit_format' (%d). Quit.\r\n",
                             fem_id, bit_in_fem_action_min, bit_in_fem_action_index,
                             fem_action_entry_p->fem_bit_info[bit_in_fem_action_index].fem_bit_format);
            }
        }
        if ((fem_action_entry_p->fem_adder & SAL_FROM_BIT(bit_in_fem_action_max)) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "This is 'FEM id' %d which has %d bits and 'adder' of the same no. of bits. \r\n"
                         "However, 'adder' (0x%08lX) has non zero bits above BIT(%d). Quit.\r\n",
                         fem_id, bit_in_fem_action_max,
                         (unsigned long) fem_action_entry_p->fem_adder, (bit_in_fem_action_max - 1));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_fem_is_any_input_action_the_same(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_action_entry_t fem_action_entry[],
    uint8 ignore_actions,
    dnx_field_fem_map_index_t * fem_map_index_p)
{
    dnx_field_fem_map_index_t fem_map_index_max, fem_map_index_a, fem_map_index_b;
    unsigned int actions_are_the_same;
    unsigned int fem_action_bit_max;
    unsigned int ignore_count;

    SHR_FUNC_INIT_VARS(unit);
    
    *fem_map_index_p = DNX_FIELD_FEM_MAP_INDEX_INVALID;
    
    fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
    
    ignore_count = 0;
    for (fem_map_index_a = 0; fem_map_index_a < fem_map_index_max; fem_map_index_a++)
    {
        if (ignore_actions & SAL_BIT(fem_map_index_a))
        {
            ignore_count++;
        }
    }
    if (ignore_count <= 1)
    {
        SHR_EXIT();
    }
    
    if (fem_id < dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_fems_with_short_action)
    {
        fem_action_bit_max =
            dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_action_fems_0_1;
    }
    else
    {
        fem_action_bit_max =
            dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_action_fems_2_15;
    }
    actions_are_the_same = FALSE;
    for (fem_map_index_a = 0; fem_map_index_a < (fem_map_index_max - 1); fem_map_index_a++)
    {
        if ((ignore_actions & SAL_BIT(fem_map_index_a)) != 0)
        {
            continue;
        }
        
        for (fem_map_index_b = fem_map_index_a + 1; fem_map_index_b < fem_map_index_max; fem_map_index_b++)
        {
            if ((ignore_actions & SAL_BIT(fem_map_index_b)) != 0)
            {
                continue;
            }
            
            if (fem_action_entry[fem_map_index_a].fem_action != fem_action_entry[fem_map_index_b].fem_action)
            {
                continue;
            }
            if (sal_memcmp(fem_action_entry[fem_map_index_a].fem_bit_info,
                           fem_action_entry[fem_map_index_b].fem_bit_info,
                           sizeof(fem_action_entry[fem_map_index_a].fem_bit_info[0]) * fem_action_bit_max))
            {
                continue;
            }
            
            if (fem_id >= dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_fems_with_short_action)
            {
                if (fem_action_entry[fem_map_index_a].fem_adder != fem_action_entry[fem_map_index_b].fem_adder)
                {
                    
                    continue;
                }
            }
            
            actions_are_the_same = TRUE;
            break;
        }
        if (actions_are_the_same)
        {
            
            *fem_map_index_p = fem_map_index_a;
            break;
        }
        SHR_EXIT();
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_fem_is_context_valid(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_context_t context_id,
    unsigned int *is_valid_p)
{
    dnx_field_pmf_fem_context_entry_t dnx_field_pmf_fem_program_entry;

    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);
    
    *is_valid_p = FALSE;
    dnx_field_pmf_fem_program_entry.fem_id = fem_id;
    dnx_field_pmf_fem_program_entry.context_id = context_id;
    SHR_IF_ERR_EXIT(dnx_field_actions_fem_context_hw_get(unit, &dnx_field_pmf_fem_program_entry));
    if (dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_key_select != DNX_FIELD_INVALID_FEM_KEY_SELECT)
    {
        
        *is_valid_p = TRUE;
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_fem_is_context_using(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    unsigned int *using_fem_p)
{
    dnx_field_pmf_fem_context_entry_t dnx_field_pmf_fem_program_entry;
    dnx_field_fem_program_t fem_program;

    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);
    if (fg_id != DNX_FIELD_GROUP_TYPE_INVALID)
    {
        DNX_FIELD_GROUP_ID_VERIFY(fg_id);
    }

    
    dnx_field_pmf_fem_program_entry.fem_id = fem_id;
    dnx_field_pmf_fem_program_entry.context_id = context_id;
    SHR_IF_ERR_EXIT(dnx_field_actions_fem_context_hw_get(unit, &dnx_field_pmf_fem_program_entry));
    *using_fem_p = TRUE;
    if (dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_key_select == DNX_FIELD_INVALID_FEM_KEY_SELECT)
    {
        *using_fem_p = FALSE;
    }
    else if (fg_id != DNX_FIELD_GROUP_TYPE_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_field_fem_is_fg_owner(unit, fem_id, fg_id, &fem_program));
        if (fem_program != dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_program)
        {
            *using_fem_p = FALSE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_fem_is_context_consistent(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    dnx_field_context_t context_id,
    unsigned int *consistent_p)
{
    dnx_field_pmf_fem_context_entry_t dnx_field_pmf_fem_program_entry;

    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);
    DNX_FIELD_FEM_PROGRAM_VERIFY(unit, fem_program);
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);
    
    *consistent_p = FALSE;
    dnx_field_pmf_fem_program_entry.fem_id = fem_id;
    dnx_field_pmf_fem_program_entry.context_id = context_id;
    SHR_IF_ERR_EXIT(dnx_field_actions_fem_context_hw_get(unit, &dnx_field_pmf_fem_program_entry));
    if (dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_key_select != DNX_FIELD_INVALID_FEM_KEY_SELECT)
    {
        
        if (dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_program == fem_program)
        {
            *consistent_p = TRUE;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_fem_is_context_valid_replace(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    dnx_field_context_t context_id,
    unsigned int *valid_replace_p)
{
    dnx_field_pmf_fem_context_entry_t dnx_field_pmf_fem_program_entry;

    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);
    DNX_FIELD_FEM_PROGRAM_VERIFY(unit, fem_program);
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);
    
    *valid_replace_p = FALSE;
    dnx_field_pmf_fem_program_entry.fem_id = fem_id;
    dnx_field_pmf_fem_program_entry.context_id = context_id;
    SHR_IF_ERR_EXIT(dnx_field_actions_fem_context_hw_get(unit, &dnx_field_pmf_fem_program_entry));
    if (dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_key_select != DNX_FIELD_INVALID_FEM_KEY_SELECT)
    {
        
        if (dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_program == fem_program)
        {
            if (dnx_field_pmf_fem_program_entry.pmf_fem_program.replace_lsb_flag ==
                DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_FLAG_REPLACE_LSB_IS_ACTIVE)
            {
                *valid_replace_p = TRUE;
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_fem_is_any_context_with_replace(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    dnx_field_context_t * context_id_p)
{
    dnx_field_context_t context_id_index, context_id_max;
    dnx_field_pmf_fem_context_entry_t dnx_field_pmf_fem_program_entry;

    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);
    DNX_FIELD_FEM_PROGRAM_VERIFY(unit, fem_program);
    
    *context_id_p = DNX_FIELD_CONTEXT_ID_INVALID;
    SHR_IF_ERR_EXIT(dnx_field_pmf_fem_context_entry_t_init(unit, &dnx_field_pmf_fem_program_entry));
    dnx_field_pmf_fem_program_entry.fem_id = fem_id;
    context_id_max = dnx_data_field.common_max_val.nof_contexts_get(unit);
    for (context_id_index = 0; context_id_index < context_id_max; context_id_index++)
    {
        dnx_field_pmf_fem_program_entry.context_id = context_id_index;
        SHR_IF_ERR_EXIT(dnx_field_actions_fem_context_hw_get(unit, &dnx_field_pmf_fem_program_entry));
        if (dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_key_select == DNX_FIELD_INVALID_FEM_KEY_SELECT)
        {
            continue;
        }
        if (dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_program == fem_program)
        {
            if (dnx_field_pmf_fem_program_entry.pmf_fem_program.replace_lsb_flag ==
                DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_FLAG_REPLACE_LSB_IS_ACTIVE)
            {
                *context_id_p = context_id_index;
                break;
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_fem_is_any_context_with_program(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    dnx_field_context_t * context_id_p)
{
    dnx_field_context_t context_id_index, context_id_max;
    dnx_field_pmf_fem_context_entry_t dnx_field_pmf_fem_program_entry;

    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);
    DNX_FIELD_FEM_PROGRAM_VERIFY(unit, fem_program);
    
    *context_id_p = DNX_FIELD_CONTEXT_ID_INVALID;
    SHR_IF_ERR_EXIT(dnx_field_pmf_fem_context_entry_t_init(unit, &dnx_field_pmf_fem_program_entry));
    dnx_field_pmf_fem_program_entry.fem_id = fem_id;
    context_id_max = dnx_data_field.common_max_val.nof_contexts_get(unit);
    for (context_id_index = 0; context_id_index < context_id_max; context_id_index++)
    {
        dnx_field_pmf_fem_program_entry.context_id = context_id_index;
        SHR_IF_ERR_EXIT(dnx_field_actions_fem_context_hw_get(unit, &dnx_field_pmf_fem_program_entry));
        if (dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_key_select == DNX_FIELD_INVALID_FEM_KEY_SELECT)
        {
            continue;
        }
        if (dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_program == fem_program)
        {
            *context_id_p = context_id_index;
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_fem_action_remove(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_group_t fg_id)
{
    dnx_field_fem_program_t fem_program;
    dnx_field_fg_id_info_t fg_id_info;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_field_fem_is_fg_owner(unit, fem_id, fg_id, &fem_program));
    if (fem_program == DNX_FIELD_FEM_PROGRAM_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "\r\n" "Can't remove this FG. Field Group %d is not marked for FEM id %d.\r\n", fg_id, fem_id);
    }
    
    {
        
        dnx_field_context_t context_id_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS];
        SHR_IF_ERR_EXIT(dnx_field_group_context_id_arr_get(unit, fg_id, context_id_arr));
        if (context_id_arr[0] != DNX_FIELD_CONTEXT_ID_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "A FEM (%d) can not be removed from a Field Group while there is any context still attached.\r\n"
                         "This Field Group (%d) has some attached contexts (e.g., %d)\n",
                         fem_id, fg_id, context_id_arr[0]);
        }
    }
    {
        
        dnx_field_context_t context_id;
        SHR_IF_ERR_EXIT(dnx_field_fem_is_any_context_with_program(unit, fem_id, fem_program, &context_id));
        if (context_id != DNX_FIELD_CONTEXT_ID_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "A FEM (%d) can not be removed from a Field Group (%d) while there is any \r\n"
                         "fem context (e.g., %d) is still using the corresponding 'fem_program' (%d).\r\n",
                         fem_id, fg_id, context_id, fem_program);
        }
    }
    {
        {
            
            uint8 last_reference;
            dnx_field_fem_map_index_t fem_map_index_max, fem_map_index;
            uint8 ignore_actions;

            SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.ignore_actions.get(unit, fem_id, fem_program, &ignore_actions));
            fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
            for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
            {
                if ((ignore_actions & SAL_BIT(fem_map_index)) == 0)
                {
                    LOG_DEBUG_EX(BSL_LOG_MODULE,
                                 "FEM id %d. fem_program %d. Going to release profile %d on template manager. ignore_actions 0x%08lX\r\n",
                                 fem_id, fem_program, fem_map_index, (unsigned long) ignore_actions);
                    SHR_IF_ERR_EXIT(algo_field_info_sw.action_template_per_fem.free_single(unit, fem_id,
                                                                                           (int) fem_map_index,
                                                                                           &last_reference));
                }
            }
        }
        
        SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.
                        ignore_actions.get(unit, fem_id, fem_program, &(fg_id_info.ignore_actions)));
        
        SHR_IF_ERR_EXIT(dnx_field_actions_fem_defaults_set(unit, fem_id, fem_program, DNX_FIELD_IGNORE_ALL_ACTIONS));
        
        SHR_IF_ERR_EXIT(dnx_field_single_fem_prg_sw_state_init(unit, fem_id, fem_program));
        
        SHR_IF_ERR_EXIT(dnx_algo_field_action_pmf_a_fem_pgm_id_deallocate(unit, fem_id, fem_program));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_fem_cal_ignore_actions(
    int unit,
    dnx_field_fem_condition_entry_t fem_condition_entry[],
    uint8 *ignore_actions_p)
{
    dnx_field_fem_condition_t fem_condition_index, fem_condition_max;
    SHR_FUNC_INIT_VARS(unit);
    
    *ignore_actions_p = DNX_FIELD_IGNORE_ALL_ACTIONS;
    fem_condition_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_condition;
    for (fem_condition_index = 0; fem_condition_index < fem_condition_max; fem_condition_index++)
    {
        DNX_FIELD_FEM_CONDITION_ENTRY_VERIFY(unit, fem_condition_entry[fem_condition_index]);
        if (fem_condition_entry[fem_condition_index].fem_action_valid ==
            DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_ACTIVE)
        {
            *ignore_actions_p &= SAL_RBIT(fem_condition_entry[fem_condition_index].fem_map_index);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_fem_action_add_verify(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_group_t fg_id,
    uint8 input_offset,
    uint8 input_size,
    dnx_field_group_t second_fg_id,
    dnx_field_fem_condition_ms_bit_t fem_condition_ms_bit,
    dnx_field_fem_condition_entry_t fem_condition_entry[],
    dnx_field_action_t fem_encoded_actions[],
    dnx_field_fem_action_entry_t fem_action_entry[])
{
    uint8 ignore_actions;
    uint8 is_allocated;
    dnx_field_stage_e field_stage;
    SHR_FUNC_INIT_VARS(unit);

    
    DNX_FIELD_INPUT_SIZE_OFFSET_VERIFY(unit, input_offset, input_size, fg_id);
    
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);
    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Field Group %d does not exist.\r\n", fg_id);
    }
    
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    if ((field_stage != DNX_FIELD_STAGE_IPMF1) && (field_stage != DNX_FIELD_STAGE_IPMF2))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Specified 'stage' (%d,%s), for Field Group %d,is illegal. For FEMs, it must be IPMF2 or IMPF1.\r\n",
                     field_stage, dnx_field_stage_e_get_name(field_stage), fg_id);
    }
    DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);
    if (second_fg_id != DNX_FIELD_GROUP_INVALID)
    {
        dnx_field_group_type_e fg_type;
        int template_shift;
        
        DNX_FIELD_FG_ID_VERIFY(unit, second_fg_id);
        SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, second_fg_id, &is_allocated));
        if (!is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "Second Field Group %d does not exist (although 'second_fg_id_active' is true).\r\n",
                         second_fg_id);
        }
        
        SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, second_fg_id, &field_stage));
        if ((field_stage != DNX_FIELD_STAGE_IPMF1) && (field_stage != DNX_FIELD_STAGE_IPMF2))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Specified 'stage' (%d,%s), for 'second' Field Group %d,is illegal. For FEMs, it must be either IPMF1 or IPMF2.\r\n",
                         field_stage, dnx_field_stage_e_get_name(field_stage), second_fg_id);
        }
        SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, second_fg_id, &fg_type));
        
        if (fg_type != DNX_FIELD_GROUP_TYPE_TCAM)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "Second Field Group %d is of type %d (%s) while it must be DNX_FIELD_GROUP_TYPE_TCAM. Quit.\r\n",
                         second_fg_id, fg_type, dnx_field_group_type_e_get_name(fg_type));
        }
        
        SHR_IF_ERR_EXIT(dnx_field_group_action_buffer_shift_get(unit, second_fg_id, &template_shift));
        if (template_shift != 0)
        {
            if (dnx_data_field.features.tcam_result_flip_eco_get(unit))
            {
                dnx_field_action_length_type_e action_length_type;
                SHR_IF_ERR_EXIT(dnx_field_group_action_length_type_get(unit, second_fg_id, &action_length_type));
                if (action_length_type == DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_HALF)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Second Field Group %d with half TCAM payload size is not supported in this device, please use Field Groups with payload size bigger than half TCAM payload size. Quit.\r\n",
                                 second_fg_id);
                }
            }
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Second Field Group %d has a buffer that doesn't start from 0. Quit.\r\n", second_fg_id);
        }
    }
    {
        
        dnx_field_context_t context_id_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS];
        SHR_IF_ERR_EXIT(dnx_field_group_context_id_arr_get(unit, fg_id, context_id_arr));
        if (context_id_arr[0] != DNX_FIELD_CONTEXT_ID_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "A FEM can not be added to a Field Group after any context has been attached.\r\n"
                         "This Field Group=%d has some attached contexts (e.g., %d)\n", fg_id, context_id_arr[0]);
        }
    }
    {
        
        SHR_IF_ERR_EXIT(dnx_field_fem_cal_ignore_actions(unit, fem_condition_entry, &ignore_actions));
    }
    {
        
        dnx_field_fem_map_index_t fem_map_index_max, fem_map_index;
        dnx_field_fem_action_entry_t *fem_action_entry_p;
        fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
        if (fem_id < dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_fems_with_short_action)
        {
            for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
            {
                if ((ignore_actions & SAL_BIT(fem_map_index)) != 0)
                {
                    
                    continue;
                }
                fem_action_entry_p = &fem_action_entry[fem_map_index];
                SHR_IF_ERR_EXIT(dnx_field_fem_num_action_bits_verify
                                (unit, fem_id, fem_encoded_actions[fem_map_index], fem_action_entry_p));
            }
        }
    }
    {
        
        dnx_field_fem_map_index_t fem_map_index;
        SHR_IF_ERR_EXIT(dnx_field_fem_is_any_input_action_the_same
                        (unit, fem_id, fem_action_entry, ignore_actions, &fem_map_index));
        if (fem_map_index != DNX_FIELD_FEM_MAP_INDEX_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n" "Two actions were found to be the same. One is at index %d. Quit.\r\n", fem_map_index);
        }
    }
    {
        
        dnx_field_fem_program_t fem_program_index, fem_program_max;
        dnx_field_group_t sw_fg_id;
        dnx_field_group_t sw_second_fg_id;

        fem_program_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs;
        for (fem_program_index = FIRST_AVAILABLE_FEM_PROGRAM; fem_program_index < fem_program_max; fem_program_index++)
        {
            
            SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.fg_id.get(unit, fem_id, fem_program_index, &sw_fg_id));
            SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.second_fg_id.get(unit, fem_id, fem_program_index, &sw_second_fg_id));
            if ((sw_fg_id == fg_id) || (sw_second_fg_id == fg_id))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "\r\n"
                             "This FEM (%d) is already configured (on 'fem program' %d, fg_id %d, second_fg_id %d) with this Field Group (%d) .\r\n"
                             " We do not allow the same Field Group (main or second) to be added to the same FEM more than once.\r\n",
                             fem_id, fem_program_index, sw_fg_id, sw_second_fg_id, fg_id);
            }
            if (second_fg_id != DNX_FIELD_GROUP_INVALID)
            {
                if ((sw_fg_id == second_fg_id) || (sw_second_fg_id == second_fg_id))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "\r\n"
                                 "This FEM (%d) is already configured (on 'fem program' %d, fg_id %d, second_fg_id %d) with this second Field Group (%d) .\r\n"
                                 " We do not allow the same Field Group (main or second) to be added to the same FEM more than once.\r\n",
                                 fem_id, fem_program_index, sw_fg_id, sw_second_fg_id, second_fg_id);
                }
            }
        }
    }
    {
        
        dnx_field_fem_map_index_t fem_map_index_max, fem_map_index;
        uint32 prefix;
        unsigned int prefix_size;

        fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
        for (fem_map_index = 0; fem_map_index < fem_map_index_max &&
             fem_encoded_actions[fem_map_index] != DNX_FIELD_ACTION_INVALID; fem_map_index++)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_prefix
                            (unit, DNX_ACTION_STAGE(fem_encoded_actions[fem_map_index]),
                             fem_encoded_actions[fem_map_index], &prefix, &prefix_size));
            if (prefix != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Non Zero prefix is not supported for FEM "
                             "(Attempted to add action number %d (%s) with prefix %u prefix_size %u to FEM ID %d)\r\n",
                             fem_map_index, dnx_field_dnx_action_text(unit, fem_encoded_actions[fem_map_index]),
                             prefix, prefix_size, fem_id);
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_fem_action_add(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_group_t fg_id,
    uint8 input_offset,
    uint8 input_size,
    dnx_field_group_t second_fg_id,
    dnx_field_fem_condition_ms_bit_t fem_condition_ms_bit,
    dnx_field_fem_condition_entry_t fem_condition_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION],
    dnx_field_action_t fem_encoded_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX],
    dnx_field_fem_action_entry_t fem_action_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX])
{
    uint8 ignore_actions, already_in_hw;
    unsigned int revert_hardware, revert_swstate;
    dnx_field_fem_program_t fem_program;
    dnx_field_stage_e field_stage, action_field_stage;
    
    dnx_field_fem_condition_entry_t
        fem_condition_entry_destination[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION];
    
    dnx_field_fem_action_entry_t fem_action_entry_destination[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX];
    
    dnx_field_action_t fem_encoded_actions_destination[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX];

    SHR_FUNC_INIT_VARS(unit);
    
    fem_program = DNX_FIELD_FEM_PROGRAM_INVALID;
    
    revert_hardware = revert_swstate = FALSE;
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    {
        
        dnx_field_fem_map_index_t fem_map_index_max, fem_map_index;

        fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
        
        for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
        {
            fem_encoded_actions_destination[fem_map_index] = DNX_FIELD_ACTION_INVALID;
        }
        for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
        {
            
            if (fem_encoded_actions[fem_map_index] == DNX_FIELD_ACTION_INVALID)
            {
                break;
            }
            
            action_field_stage = DNX_ACTION_STAGE(fem_encoded_actions[fem_map_index]);
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_action_type
                            (unit, action_field_stage,
                             fem_encoded_actions[fem_map_index], &(fem_action_entry[fem_map_index].fem_action)));
        }
    }
    {
        
        SHR_INVOKE_VERIFY_DNX(dnx_field_fem_action_add_verify
                              (unit, fem_id, fg_id, input_offset, input_size, second_fg_id, fem_condition_ms_bit,
                               fem_condition_entry, fem_encoded_actions, fem_action_entry));
    }
    {
        dnx_field_fem_program_t available_fem_program;

        {
            
            int template_shift;
            SHR_IF_ERR_EXIT(dnx_field_group_action_buffer_shift_get(unit, fg_id, &template_shift));
            input_offset += template_shift;
        }

        {
            
            SHR_IF_ERR_EXIT(dnx_field_fem_cal_ignore_actions(unit, fem_condition_entry, &ignore_actions));
        }
        {
            
            uint8 sum_allocated_actions;

            SHR_IF_ERR_EXIT(dnx_algo_field_action_find_available_fem_program
                            (unit, fem_id, &available_fem_program, &sum_allocated_actions));
            
            {
                
                SHR_IF_ERR_EXIT(dnx_algo_field_action_update_conditions
                                (unit, fem_id, fem_condition_entry, fem_encoded_actions, fem_action_entry,
                                 sum_allocated_actions, fem_condition_entry_destination, &ignore_actions,
                                 &already_in_hw, fem_encoded_actions_destination, fem_action_entry_destination));
                
            }
            
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Found free 'FEM program' (%d) for 'FEM id' %d %s%s\r\n",
                         available_fem_program, fem_id, EMPTY, EMPTY);
        }
        
        fem_program = available_fem_program;
        {
            
            dnx_field_context_t context_id;
            SHR_IF_ERR_EXIT(dnx_field_fem_is_any_context_with_program(unit, fem_id, fem_program, &context_id));
            if (context_id != DNX_FIELD_CONTEXT_ID_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "\r\n"
                             "A FEM (%d) can not be added to a Field Group (%d) while there is any \r\n"
                             "fem context (e.g., %d) is still using the corresponding 'fem_program' (%d).\r\n",
                             fem_id, fg_id, context_id, fem_program);
            }
        }
    }
    
    
    
    {
        
        revert_hardware = TRUE;
        
        SHR_IF_ERR_EXIT(dnx_field_actions_fem_set
                        (unit, fem_id, fem_program, fem_condition_ms_bit, fem_condition_entry_destination,
                         (ignore_actions | already_in_hw), fem_action_entry_destination));
        revert_hardware = FALSE;
        
        revert_swstate = TRUE;
        SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.second_fg_id.set(unit, fem_id, fem_program, second_fg_id));
        SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.input_offset.set(unit, fem_id, fem_program, input_offset));
        SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.input_size.set(unit, fem_id, fem_program, input_size));
        SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.fg_id.set(unit, fem_id, fem_program, fg_id));
        SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.ignore_actions.set(unit, fem_id, fem_program, ignore_actions));
        {
            
            dnx_field_fem_map_index_t fem_map_index, fem_map_index_max;
            fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
            for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
            {
                SHR_IF_ERR_EXIT(FEM_INFO_FEM_ENCODED_ACTIONS.set(unit, fem_id, fem_program, fem_map_index,
                                                                 fem_encoded_actions_destination[fem_map_index]));
            }
        }
        revert_swstate = FALSE;
    }
exit:
    {
        shr_error_e shr_error;

        if (revert_hardware)
        {
            
            shr_error = dnx_field_actions_fem_defaults_set(unit, fem_id, fem_program, DNX_FIELD_IGNORE_ALL_ACTIONS);
            if (shr_error != _SHR_E_NONE)
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Failure on dnx_field_actions_fem_defaults_set() (%d, %s)  %s%s\r\n",
                             shr_error, shrextend_errmsg_get(shr_error), EMPTY, EMPTY);
            }
        }
        if (revert_swstate)
        {
            
            shr_error = dnx_field_single_fem_prg_sw_state_init(unit, fem_id, fem_program);
            if (shr_error != _SHR_E_NONE)
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Failure on dnx_field_single_fem_prg_sw_state_init() (%d, %s)  %s%s\r\n",
                             shr_error, shrextend_errmsg_get(shr_error), EMPTY, EMPTY);
            }
        }
    }
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_actions_fem_defaults_set(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    uint8 ignore_actions)
{
    dnx_field_fem_condition_entry_t fem_condition_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION];
    dnx_field_fem_action_entry_t fem_action_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX];

    SHR_FUNC_INIT_VARS(unit);
    {
        SHR_IF_ERR_EXIT(dnx_field_fem_condition_entry_t_array_init(unit, fem_condition_entry));
    }
    {
        dnx_field_fem_map_index_t fem_map_index_max, fem_map_index;

        fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
        for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
        {
            
            SHR_IF_ERR_EXIT(dnx_field_fem_action_entry_t_init(unit, &(fem_action_entry[fem_map_index])));
        }
    }
    SHR_IF_ERR_EXIT(dnx_field_actions_fem_set
                    (unit, fem_id, fem_program,
                     dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->fem_condition_ms_bit_min_value,
                     fem_condition_entry, ignore_actions, fem_action_entry));
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_actions_fem_set(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    dnx_field_fem_condition_ms_bit_t fem_condition_ms_bit,
    dnx_field_fem_condition_entry_t fem_condition_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION],
    uint8 ignore_actions,
    dnx_field_fem_action_entry_t fem_action_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX])
{
    SHR_FUNC_INIT_VARS(unit);
    
    {
        dnx_field_fem_condition_ms_bit_entry_t dnx_field_fem_condition_ms_bit_entry;

        dnx_field_fem_condition_ms_bit_entry.fem_id = fem_id;
        dnx_field_fem_condition_ms_bit_entry.fem_program = fem_program;
        dnx_field_fem_condition_ms_bit_entry.fem_condition_ms_bit = fem_condition_ms_bit;
        SHR_IF_ERR_EXIT(dnx_field_actions_fem_condition_ms_bit_hw_set(unit, &dnx_field_fem_condition_ms_bit_entry));
    }
    {
        dnx_field_fem_map_index_entry_t dnx_field_fem_map_index_entry;
        dnx_field_fem_condition_t fem_condition_index, fem_condition_max;
        
        fem_condition_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_condition;

        dnx_field_fem_map_index_entry.fem_id = fem_id;
        dnx_field_fem_map_index_entry.fem_program = fem_program;
        for (fem_condition_index = 0; fem_condition_index < fem_condition_max; fem_condition_index++)
        {
            dnx_field_fem_map_index_entry.fem_condition = fem_condition_index;
            dnx_field_fem_map_index_entry.fem_condition_entry = fem_condition_entry[fem_condition_index];
            SHR_IF_ERR_EXIT(dnx_field_actions_fem_map_index_hw_set(unit, &dnx_field_fem_map_index_entry));
        }
    }
    {
        dnx_field_fem_map_index_t fem_map_index_max, fem_map_index;
        dnx_field_fem_action_info_t dnx_field_fem_action_info;

        
        fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
        dnx_field_fem_action_info.fem_id = fem_id;
        for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
        {
            if ((ignore_actions & SAL_BIT(fem_map_index)) != 0)
            {
                continue;
            }
            dnx_field_fem_action_info.fem_map_index = fem_map_index;
            dnx_field_fem_action_info.fem_action_entry = fem_action_entry[fem_map_index];
            dnx_field_fem_action_info.fem_action_entry.fem_adder = fem_action_entry[fem_map_index].fem_adder;
            SHR_IF_ERR_EXIT(dnx_field_actions_fem_action_info_hw_set(unit, &dnx_field_fem_action_info));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_actions_fem_get(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    dnx_field_fem_condition_ms_bit_t * fem_condition_ms_bit_p,
    dnx_field_fem_condition_entry_t fem_condition_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION],
    dnx_field_fem_action_entry_t fem_action_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX])
{
    SHR_FUNC_INIT_VARS(unit);
    
    {
        dnx_field_fem_condition_ms_bit_entry_t dnx_field_fem_condition_ms_bit_entry;

        dnx_field_fem_condition_ms_bit_entry.fem_id = fem_id;
        dnx_field_fem_condition_ms_bit_entry.fem_program = fem_program;
        SHR_IF_ERR_EXIT(dnx_field_actions_fem_condition_ms_bit_hw_get(unit, &dnx_field_fem_condition_ms_bit_entry));
        *fem_condition_ms_bit_p = dnx_field_fem_condition_ms_bit_entry.fem_condition_ms_bit;
    }
    {
        dnx_field_fem_map_index_entry_t dnx_field_fem_map_index_entry;
        dnx_field_fem_condition_t fem_condition_index, fem_condition_max;
        
        fem_condition_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_condition;

        dnx_field_fem_map_index_entry.fem_id = fem_id;
        dnx_field_fem_map_index_entry.fem_program = fem_program;
        for (fem_condition_index = 0; fem_condition_index < fem_condition_max; fem_condition_index++)
        {
            dnx_field_fem_map_index_entry.fem_condition = fem_condition_index;
            SHR_IF_ERR_EXIT(dnx_field_actions_fem_map_index_hw_get(unit, &dnx_field_fem_map_index_entry));
            fem_condition_entry[fem_condition_index] = dnx_field_fem_map_index_entry.fem_condition_entry;
        }
    }
    {
        dnx_field_fem_map_index_t fem_map_index_max, fem_map_index;
        dnx_field_fem_action_info_t dnx_field_fem_action_info;

        
        fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
        dnx_field_fem_action_info.fem_id = fem_id;
        for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
        {
            dnx_field_fem_action_info.fem_map_index = fem_map_index;
            SHR_IF_ERR_EXIT(dnx_field_actions_fem_action_info_hw_get(unit, &dnx_field_fem_action_info));
            fem_action_entry[fem_map_index] = dnx_field_fem_action_info.fem_action_entry;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_actions_single_fem_single_context_defaults_set(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_fem_id_t fem_id)
{
    dnx_field_pmf_fem_context_entry_t dnx_field_pmf_fem_program_entry;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);
    dnx_field_pmf_fem_program_entry.fem_id = fem_id;
    dnx_field_pmf_fem_program_entry.context_id = context_id;
    dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_program = DEFAULT_FEM_PROGRAM;
    dnx_field_pmf_fem_program_entry.pmf_fem_program.replace_lsb_flag =
        DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_FLAG_REPLACE_LSB_IS_NOT_ACTIVE;
    dnx_field_pmf_fem_program_entry.pmf_fem_program.replace_lsb_select =
        DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_0;
    
    dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_key_select = DNX_FIELD_INVALID_FEM_KEY_SELECT;
    SHR_IF_ERR_EXIT(dnx_field_actions_fem_context_hw_set(unit, &dnx_field_pmf_fem_program_entry));
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_actions_all_fem_single_context_defaults_set(
    int unit,
    dnx_field_context_t context_id)
{
    dnx_field_fem_id_t fem_id_index, fem_id_max;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);
    fem_id_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
    for (fem_id_index = 0; fem_id_index < fem_id_max; fem_id_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_actions_single_fem_single_context_defaults_set(unit, context_id, fem_id_index));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_actions_all_fem_all_context_defaults_set(
    int unit)
{
    dnx_field_context_t context_id_index, context_id_max;
    SHR_FUNC_INIT_VARS(unit);
    context_id_max = dnx_data_field.common_max_val.nof_contexts_get(unit);
    for (context_id_index = 0; context_id_index < context_id_max; context_id_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_actions_all_fem_single_context_defaults_set(unit, context_id_index));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_actions_fem_context_hw_set(
    int unit,
    dnx_field_pmf_fem_context_entry_t * dnx_field_pmf_fem_program_entry_p)
{
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id;
    dbal_field_types_basic_info_t *field_info;
    char *fem_key_select_name;
    char *fem_lsb_select_name;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_field_pmf_fem_program_entry_p, _SHR_E_PARAM, "dnx_field_fem_condition_ms_bit_entry_p");
    
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Modify FEM context for: fem_id %d, context_id %d. New fem_program %d New fem_key_select %d ",
                 dnx_field_pmf_fem_program_entry_p->fem_id,
                 dnx_field_pmf_fem_program_entry_p->context_id,
                 dnx_field_pmf_fem_program_entry_p->pmf_fem_program.fem_program,
                 dnx_field_pmf_fem_program_entry_p->pmf_fem_program.fem_key_select);

    
    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, DBAL_FIELD_FIELD_PMF_A_FEM_KEY_SELECT, &field_info));
    fem_key_select_name =
        field_info->enum_val_info[dnx_field_pmf_fem_program_entry_p->pmf_fem_program.fem_key_select].name;
    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, DBAL_FIELD_FIELD_FEM_REPLACE_LSB_SELECT, &field_info));
    fem_lsb_select_name =
        field_info->enum_val_info[dnx_field_pmf_fem_program_entry_p->pmf_fem_program.replace_lsb_select].name;
    LOG_DEBUG_CONT_EX(BSL_LOG_MODULE, "(%s)\r\n==> replace_lsb_flag %d replace_lsb_select %d (%s)\r\n",
                      fem_key_select_name, dnx_field_pmf_fem_program_entry_p->pmf_fem_program.replace_lsb_flag,
                      dnx_field_pmf_fem_program_entry_p->pmf_fem_program.replace_lsb_select, fem_lsb_select_name);
    {
        
        dbal_table_id = DBAL_TABLE_FIELD_PMF_A_KEY_SELECT_N_FEM_PROG;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID,
                                   dnx_field_pmf_fem_program_entry_p->context_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_ID,
                                   dnx_field_pmf_fem_program_entry_p->fem_id);
        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_A_FEM_KEY_SELECT, INST_SINGLE,
                                     (uint32) (dnx_field_pmf_fem_program_entry_p->pmf_fem_program.fem_key_select));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_PGM_ID, INST_SINGLE,
                                     (uint32) (dnx_field_pmf_fem_program_entry_p->pmf_fem_program.fem_program));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_REPLACE_LSB_FLAG, INST_SINGLE,
                                     (uint32) (dnx_field_pmf_fem_program_entry_p->pmf_fem_program.replace_lsb_flag));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_REPLACE_LSB_SELECT, INST_SINGLE,
                                     (uint32) (dnx_field_pmf_fem_program_entry_p->pmf_fem_program.replace_lsb_select));

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_actions_fem_context_hw_get(
    int unit,
    dnx_field_pmf_fem_context_entry_t * dnx_field_pmf_fem_program_entry_p)
{
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id;
    dbal_field_types_basic_info_t *field_info;
    char *fem_key_select_name;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_field_pmf_fem_program_entry_p, _SHR_E_PARAM, "dnx_field_fem_condition_ms_bit_entry_p");
    
    {
        
        dbal_table_id = DBAL_TABLE_FIELD_PMF_A_KEY_SELECT_N_FEM_PROG;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID,
                                   dnx_field_pmf_fem_program_entry_p->context_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_ID,
                                   dnx_field_pmf_fem_program_entry_p->fem_id);
        
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_A_FEM_KEY_SELECT, INST_SINGLE,
                                   &(dnx_field_pmf_fem_program_entry_p->pmf_fem_program.fem_key_select));
        dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_PGM_ID, INST_SINGLE,
                                  &(dnx_field_pmf_fem_program_entry_p->pmf_fem_program.fem_program));
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_REPLACE_LSB_FLAG, INST_SINGLE,
                                   &(dnx_field_pmf_fem_program_entry_p->pmf_fem_program.replace_lsb_flag));
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_REPLACE_LSB_SELECT, INST_SINGLE,
                                   &(dnx_field_pmf_fem_program_entry_p->pmf_fem_program.replace_lsb_select));
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
    }
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Retrived FEM context info for: fem_id %d, context_id %d. Retrived: fem_program %d New fem_key_select %d ",
                 dnx_field_pmf_fem_program_entry_p->fem_id,
                 dnx_field_pmf_fem_program_entry_p->context_id,
                 dnx_field_pmf_fem_program_entry_p->pmf_fem_program.fem_program,
                 dnx_field_pmf_fem_program_entry_p->pmf_fem_program.fem_key_select);
    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, DBAL_FIELD_FIELD_PMF_A_FEM_KEY_SELECT, &field_info));
    fem_key_select_name =
        field_info->enum_val_info[dnx_field_pmf_fem_program_entry_p->pmf_fem_program.fem_key_select].name;
    LOG_DEBUG_CONT_EX(BSL_LOG_MODULE, "(%s)\r\n==> replace_lsb_flag %d replace_lsb_select %d %s\r\n",
                      fem_key_select_name, dnx_field_pmf_fem_program_entry_p->pmf_fem_program.replace_lsb_flag,
                      dnx_field_pmf_fem_program_entry_p->pmf_fem_program.replace_lsb_select, EMPTY);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_actions_fem_condition_ms_bit_hw_set(
    int unit,
    dnx_field_fem_condition_ms_bit_entry_t * dnx_field_fem_condition_ms_bit_entry_p)
{
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_field_fem_condition_ms_bit_entry_p, _SHR_E_PARAM, "dnx_field_fem_condition_ms_bit_entry_p");
    
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Modify bit select for: fem_id %d, fem_program %d. New fem_condition_ms_bit %d %s\r\n",
                 dnx_field_fem_condition_ms_bit_entry_p->fem_id,
                 dnx_field_fem_condition_ms_bit_entry_p->fem_program,
                 dnx_field_fem_condition_ms_bit_entry_p->fem_condition_ms_bit, EMPTY);
    {
        
        dbal_table_id = DBAL_TABLE_FIELD_PMF_A_FEM_CONDITION_MS_BIT;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_PGM_ID,
                                   dnx_field_fem_condition_ms_bit_entry_p->fem_program);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_ID,
                                   dnx_field_fem_condition_ms_bit_entry_p->fem_id);
        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_CONDITION_MS_BIT, INST_SINGLE,
                                     (uint32) (dnx_field_fem_condition_ms_bit_entry_p->fem_condition_ms_bit));

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_actions_fem_condition_ms_bit_hw_get(
    int unit,
    dnx_field_fem_condition_ms_bit_entry_t * dnx_field_fem_condition_ms_bit_entry_p)
{
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_field_fem_condition_ms_bit_entry_p, _SHR_E_PARAM, "dnx_field_fem_condition_ms_bit_entry_p");
    
    {
        
        dbal_table_id = DBAL_TABLE_FIELD_PMF_A_FEM_CONDITION_MS_BIT;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_PGM_ID,
                                   dnx_field_fem_condition_ms_bit_entry_p->fem_program);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_ID,
                                   dnx_field_fem_condition_ms_bit_entry_p->fem_id);
        
        dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_CONDITION_MS_BIT,
                                  INST_SINGLE, &(dnx_field_fem_condition_ms_bit_entry_p->fem_condition_ms_bit));
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
    }
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Just read bit select for: fem_id %d, fem_program %d fem_condition_ms_bit %d %s\r\n",
                 dnx_field_fem_condition_ms_bit_entry_p->fem_id,
                 dnx_field_fem_condition_ms_bit_entry_p->fem_program,
                 dnx_field_fem_condition_ms_bit_entry_p->fem_condition_ms_bit, EMPTY);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_actions_fem_map_index_hw_get(
    int unit,
    dnx_field_fem_map_index_entry_t * dnx_field_fem_map_index_entry_p)
{
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_field_fem_map_index_entry_p, _SHR_E_PARAM, "dnx_field_fem_map_index_entry_p");
    
    {
        
        dbal_table_id = DBAL_TABLE_FIELD_PMF_A_FEM_MAP_INDEX;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_PGM_ID,
                                   dnx_field_fem_map_index_entry_p->fem_program);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_ID,
                                   dnx_field_fem_map_index_entry_p->fem_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_CONDITION,
                                   dnx_field_fem_map_index_entry_p->fem_condition);
        
        dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_MAP_INDEX,
                                  INST_SINGLE, &(dnx_field_fem_map_index_entry_p->fem_condition_entry.fem_map_index));
        dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_MAP_DATA,
                                  INST_SINGLE, &(dnx_field_fem_map_index_entry_p->fem_condition_entry.fem_map_data));
        dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_ACTION_VALID,
                                  INST_SINGLE,
                                  &(dnx_field_fem_map_index_entry_p->fem_condition_entry.fem_action_valid));
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
    }
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Just read map index info for: fem_id %d, fem_program %d fem_condition %d %s\r\n",
                 dnx_field_fem_map_index_entry_p->fem_id,
                 dnx_field_fem_map_index_entry_p->fem_program, dnx_field_fem_map_index_entry_p->fem_condition, EMPTY);
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "==> map index data: map_index %d map_data 0x%08X action_valid %d %s \r\n",
                 dnx_field_fem_map_index_entry_p->fem_condition_entry.fem_map_index,
                 (uint16) (dnx_field_fem_map_index_entry_p->fem_condition_entry.fem_map_data),
                 dnx_field_fem_map_index_entry_p->fem_condition_entry.fem_action_valid, EMPTY);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_actions_fem_map_index_hw_set(
    int unit,
    dnx_field_fem_map_index_entry_t * dnx_field_fem_map_index_entry_p)
{
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_field_fem_map_index_entry_p, _SHR_E_PARAM, "dnx_field_fem_map_index_entry_p");
    
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Writing map index info for: fem_id %d, fem_program %d fem_condition %d %s\r\n",
                 dnx_field_fem_map_index_entry_p->fem_id,
                 dnx_field_fem_map_index_entry_p->fem_program, dnx_field_fem_map_index_entry_p->fem_condition, EMPTY);
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "==> map index data: map_index %d map_data 0x%08X action_valid %d %s \r\n",
                 dnx_field_fem_map_index_entry_p->fem_condition_entry.fem_map_index,
                 (uint16) (dnx_field_fem_map_index_entry_p->fem_condition_entry.fem_map_data),
                 dnx_field_fem_map_index_entry_p->fem_condition_entry.fem_action_valid, EMPTY);

    {
        
        dbal_table_id = DBAL_TABLE_FIELD_PMF_A_FEM_MAP_INDEX;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_PGM_ID,
                                   dnx_field_fem_map_index_entry_p->fem_program);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_ID,
                                   dnx_field_fem_map_index_entry_p->fem_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_CONDITION,
                                   dnx_field_fem_map_index_entry_p->fem_condition);
        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_MAP_INDEX, INST_SINGLE,
                                     (uint32) (dnx_field_fem_map_index_entry_p->fem_condition_entry.fem_map_index));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_MAP_DATA, INST_SINGLE,
                                     (uint32) (dnx_field_fem_map_index_entry_p->fem_condition_entry.fem_map_data));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_ACTION_VALID, INST_SINGLE,
                                     (uint32) (dnx_field_fem_map_index_entry_p->fem_condition_entry.fem_action_valid));

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_field_actions_fem_action_info_hw_set(
    int unit,
    dnx_field_fem_action_info_t * dnx_field_fem_action_info_p)
{
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id;
    dnx_field_action_t dnx_action;
    dnx_field_fem_bit_val_t fem_bit_val;
    unsigned int bit_in_fem_action, nof_bits_in_fem_action;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_field_fem_action_info_p, _SHR_E_PARAM, "dnx_field_fem_action_info_p");
    
    
    dnx_action =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF1,
                   dnx_field_fem_action_info_p->fem_action_entry.fem_action);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Writing action info for: fem_id %d, fem_map_index %d fem_action %d (%s)\r\n",
                 dnx_field_fem_action_info_p->fem_id, dnx_field_fem_action_info_p->fem_map_index,
                 dnx_field_fem_action_info_p->fem_action_entry.fem_action, dnx_field_dnx_action_text(unit, dnx_action));
    LOG_DEBUG_EX(BSL_LOG_MODULE, "==> action_bit format: %d %d %d %d ",
                 dnx_field_fem_action_info_p->fem_action_entry.fem_bit_info[0].fem_bit_format,
                 dnx_field_fem_action_info_p->fem_action_entry.fem_bit_info[1].fem_bit_format,
                 dnx_field_fem_action_info_p->fem_action_entry.fem_bit_info[2].fem_bit_format,
                 dnx_field_fem_action_info_p->fem_action_entry.fem_bit_info[3].fem_bit_format);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "adder %d. %s%s%s\r\n", dnx_field_fem_action_info_p->fem_action_entry.fem_adder, EMPTY,
                 EMPTY, EMPTY);

    if (dnx_field_fem_action_info_p->fem_id <
        dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_fems_with_short_action)
    {
        
        
        dbal_table_id = DBAL_TABLE_FIELD_PMF_A_FEM_ACTION_TYPE_MAP;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_MAP_INDEX,
                                   dnx_field_fem_action_info_p->fem_map_index);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_ID, dnx_field_fem_action_info_p->fem_id);
        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPMF1_ACTION, INST_SINGLE,
                                     (uint32) (dnx_field_fem_action_info_p->fem_action_entry.fem_action));

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        
        
        dbal_table_id = DBAL_TABLE_FIELD_PMF_A_FEM_FIELD_SELECT_FEMS_0_1_MAP;
        nof_bits_in_fem_action =
            dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_action_fems_0_1;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
        
        for (bit_in_fem_action = 0; bit_in_fem_action < nof_bits_in_fem_action; bit_in_fem_action++)
        {
            
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_MAP_INDEX,
                                       dnx_field_fem_action_info_p->fem_map_index);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_ID,
                                       dnx_field_fem_action_info_p->fem_id);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_SELECTED_BIT, bit_in_fem_action);
            
            SHR_IF_ERR_EXIT(dnx_field_actions_fem_log_to_phys_bit_source
                            (unit, &(dnx_field_fem_action_info_p->fem_action_entry.fem_bit_info[bit_in_fem_action]),
                             &fem_bit_val));
            
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_BIT_VAL, INST_SINGLE,
                                         (uint32) fem_bit_val);
            

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        }
        
    }
    else
    {
        
        
        dbal_table_id = DBAL_TABLE_FIELD_PMF_A_FEM_ACTION_TYPE_MAP;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_MAP_INDEX,
                                   dnx_field_fem_action_info_p->fem_map_index);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_ID, dnx_field_fem_action_info_p->fem_id);
        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPMF1_ACTION, INST_SINGLE,
                                     (uint32) (dnx_field_fem_action_info_p->fem_action_entry.fem_action));

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        
        
        dbal_table_id = DBAL_TABLE_FIELD_PMF_A_FEM_FIELD_SELECT_FEMS_2_15_MAP;
        nof_bits_in_fem_action =
            dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_action_fems_2_15;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
        
        for (bit_in_fem_action = 0; bit_in_fem_action < nof_bits_in_fem_action; bit_in_fem_action++)
        {
            
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_MAP_INDEX,
                                       dnx_field_fem_action_info_p->fem_map_index);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_ID,
                                       dnx_field_fem_action_info_p->fem_id);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_SELECTED_BIT, bit_in_fem_action);
            
            SHR_IF_ERR_EXIT(dnx_field_actions_fem_log_to_phys_bit_source
                            (unit, &(dnx_field_fem_action_info_p->fem_action_entry.fem_bit_info[bit_in_fem_action]),
                             &fem_bit_val));
            
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_BIT_VAL, INST_SINGLE,
                                         (uint32) fem_bit_val);
            

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        }
        
        
        dbal_table_id = DBAL_TABLE_FIELD_PMF_A_FEM_ADDER_FEMS_2_15_MAP;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_MAP_INDEX,
                                   dnx_field_fem_action_info_p->fem_map_index);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_ID, dnx_field_fem_action_info_p->fem_id);
        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_ADDER, INST_SINGLE,
                                     (uint32) (dnx_field_fem_action_info_p->fem_action_entry.fem_adder));

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_actions_fem_action_info_hw_get(
    int unit,
    dnx_field_fem_action_info_t * dnx_field_fem_action_info_p)
{
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id;
    dnx_field_action_t dnx_action;
    dnx_field_fem_bit_val_t fem_bit_val;
    unsigned int bit_in_fem_action, nof_bits_in_fem_action;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_field_fem_action_info_p, _SHR_E_PARAM, "dnx_field_fem_action_info_p");
    
    
    SHR_IF_ERR_EXIT(dnx_field_fem_action_entry_t_init(unit, &(dnx_field_fem_action_info_p->fem_action_entry)));
    if (dnx_field_fem_action_info_p->fem_id <
        dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_fems_with_short_action)
    {
        
        
        dbal_table_id = DBAL_TABLE_FIELD_PMF_A_FEM_ACTION_TYPE_MAP;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_MAP_INDEX,
                                   dnx_field_fem_action_info_p->fem_map_index);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_ID, dnx_field_fem_action_info_p->fem_id);
        
        dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IPMF1_ACTION,
                                  INST_SINGLE, &(dnx_field_fem_action_info_p->fem_action_entry.fem_action));
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        
        
        dbal_table_id = DBAL_TABLE_FIELD_PMF_A_FEM_FIELD_SELECT_FEMS_0_1_MAP;
        nof_bits_in_fem_action =
            dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_action_fems_0_1;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
        
        for (bit_in_fem_action = 0; bit_in_fem_action < nof_bits_in_fem_action; bit_in_fem_action++)
        {
            
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_MAP_INDEX,
                                       dnx_field_fem_action_info_p->fem_map_index);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_ID,
                                       dnx_field_fem_action_info_p->fem_id);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_SELECTED_BIT, bit_in_fem_action);
            
            dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_BIT_VAL, INST_SINGLE, &(fem_bit_val));
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            
            SHR_IF_ERR_EXIT(dnx_field_actions_fem_phys_to_log_bit_source
                            (unit, fem_bit_val,
                             &(dnx_field_fem_action_info_p->fem_action_entry.fem_bit_info[bit_in_fem_action])));
        }
        
    }
    else
    {
        
        
        dbal_table_id = DBAL_TABLE_FIELD_PMF_A_FEM_ACTION_TYPE_MAP;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_MAP_INDEX,
                                   dnx_field_fem_action_info_p->fem_map_index);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_ID, dnx_field_fem_action_info_p->fem_id);
        
        dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IPMF1_ACTION,
                                  INST_SINGLE, &(dnx_field_fem_action_info_p->fem_action_entry.fem_action));
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        
        
        dbal_table_id = DBAL_TABLE_FIELD_PMF_A_FEM_FIELD_SELECT_FEMS_2_15_MAP;
        nof_bits_in_fem_action =
            dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_action_fems_2_15;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
        
        for (bit_in_fem_action = 0; bit_in_fem_action < nof_bits_in_fem_action; bit_in_fem_action++)
        {
            
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_MAP_INDEX,
                                       dnx_field_fem_action_info_p->fem_map_index);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_ID,
                                       dnx_field_fem_action_info_p->fem_id);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_SELECTED_BIT, bit_in_fem_action);
            
            dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_BIT_VAL, INST_SINGLE, &(fem_bit_val));
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            
            SHR_IF_ERR_EXIT(dnx_field_actions_fem_phys_to_log_bit_source
                            (unit, fem_bit_val,
                             &(dnx_field_fem_action_info_p->fem_action_entry.fem_bit_info[bit_in_fem_action])));
        }
        
        
        dbal_table_id = DBAL_TABLE_FIELD_PMF_A_FEM_ADDER_FEMS_2_15_MAP;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_MAP_INDEX,
                                   dnx_field_fem_action_info_p->fem_map_index);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_ID, dnx_field_fem_action_info_p->fem_id);
        
        dbal_value_field32_request(unit, entry_handle_id,
                                   DBAL_FIELD_FIELD_FEM_ADDER, INST_SINGLE,
                                   &(dnx_field_fem_action_info_p->fem_action_entry.fem_adder));
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }
    
    dnx_action =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF1,
                   dnx_field_fem_action_info_p->fem_action_entry.fem_action);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Retrieved action info for: fem_id %d, fem_map_index %d fem_action %d (%s)\r\n",
                 dnx_field_fem_action_info_p->fem_id, dnx_field_fem_action_info_p->fem_map_index,
                 dnx_field_fem_action_info_p->fem_action_entry.fem_action, dnx_field_dnx_action_text(unit, dnx_action));
    LOG_DEBUG_EX(BSL_LOG_MODULE, "==> action_bit format: %d %d %d %d ",
                 dnx_field_fem_action_info_p->fem_action_entry.fem_bit_info[0].fem_bit_format,
                 dnx_field_fem_action_info_p->fem_action_entry.fem_bit_info[1].fem_bit_format,
                 dnx_field_fem_action_info_p->fem_action_entry.fem_bit_info[2].fem_bit_format,
                 dnx_field_fem_action_info_p->fem_action_entry.fem_bit_info[3].fem_bit_format);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "adder %d. %s%s%s\r\n", dnx_field_fem_action_info_p->fem_action_entry.fem_adder, EMPTY,
                 EMPTY, EMPTY);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


