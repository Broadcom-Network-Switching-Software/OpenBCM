
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
field_pmf_a_key_select_n_fem_prog_field_fem_pgm_id_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fem_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FEM_ID, key_value__field_fem_id);
    *offset = 13*key_value__field_fem_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_pmf_a_key_select_n_fem_prog_field_pmf_a_fem_key_select_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fem_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FEM_ID, key_value__field_fem_id);
    *offset = 13*key_value__field_fem_id+2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_pmf_a_key_select_n_fem_prog_field_fem_replace_lsb_flag_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fem_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FEM_ID, key_value__field_fem_id);
    *offset = 13*key_value__field_fem_id+9;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_pmf_a_key_select_n_fem_prog_field_fem_replace_lsb_select_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fem_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FEM_ID, key_value__field_fem_id);
    *offset = 13*key_value__field_fem_id+10;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_pmf_a_fem_condition_ms_bit_field_fem_condition_ms_bit_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fem_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FEM_ID, key_value__field_fem_id);
    *offset = key_value__field_fem_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_pmf_a_fem_condition_ms_bit_field_fem_condition_ms_bit_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fem_pgm_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FEM_PGM_ID, key_value__field_fem_pgm_id);
    *offset = key_value__field_fem_pgm_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_pmf_a_fem_map_index_field_fem_action_valid_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fem_condition;
    uint32 key_value__field_fem_pgm_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FEM_CONDITION, key_value__field_fem_condition);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FEM_PGM_ID, key_value__field_fem_pgm_id);
    *offset = key_value__field_fem_condition+16*key_value__field_fem_pgm_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_pmf_a_fem_action_type_map_ipmf1_action_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fem_map_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FEM_MAP_INDEX, key_value__field_fem_map_index);
    *offset = key_value__field_fem_map_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_pmf_a_fem_field_select_fems_0_1_map_field_fem_bit_val_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fem_selected_bit;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FEM_SELECTED_BIT, key_value__field_fem_selected_bit);
    *offset = key_value__field_fem_selected_bit*6;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
