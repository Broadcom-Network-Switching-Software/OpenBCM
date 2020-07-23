
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
field_pmf_a_select_key_n_fes_field_pmf_a_fes_key_select_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fes_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FES_ID, key_value__field_fes_id);
    *offset = dnx_data_field.base_ipmf1.fes_instruction_size_get(unit)*key_value__field_fes_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_pmf_a_select_key_n_fes_field_fes_pgm_id_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fes_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FES_ID, key_value__field_fes_id);
    *offset = dnx_data_field.base_ipmf1.fes_instruction_size_get(unit)*key_value__field_fes_id+dnx_data_field.base_ipmf1.fes_pgm_id_offset_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_pmf_a_fes_2nd_instruction_ipmf1_action_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fes_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FES_ID, key_value__field_fes_id);
    *offset = dnx_data_field.base_ipmf1.nof_fes_instruction_per_context_get(unit)-1-key_value__field_fes_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_pmf_a_fes_2nd_instruction_ipmf1_action_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_pmf_fes_data_2msb;
    uint32 key_value__field_fes_pgm_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_PMF_FES_DATA_2MSB, key_value__field_pmf_fes_data_2msb);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FES_PGM_ID, key_value__field_fes_pgm_id);
    *offset = key_value__field_pmf_fes_data_2msb*dnx_data_field.base_ipmf1.nof_fes_programs_get(unit)+key_value__field_fes_pgm_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_pmf_a_efes_action_mask_null_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fes_mask_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FES_MASK_ID, key_value__field_fes_mask_id);
    *offset = key_value__field_fes_mask_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_pmf_b_select_key_n_fes_field_pmf_b_fes_key_select_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fes_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FES_ID, key_value__field_fes_id);
    *offset = dnx_data_field.base_ipmf3.fes_instruction_size_get(unit)*key_value__field_fes_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_pmf_b_select_key_n_fes_field_fes_pgm_id_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fes_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FES_ID, key_value__field_fes_id);
    *offset = dnx_data_field.base_ipmf3.fes_instruction_size_get(unit)*key_value__field_fes_id+dnx_data_field.base_ipmf3.fes_pgm_id_offset_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_pmf_b_fes_2nd_instruction_ipmf3_action_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fes_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FES_ID, key_value__field_fes_id);
    *offset = dnx_data_field.base_ipmf3.nof_fes_instruction_per_context_get(unit)-1-key_value__field_fes_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_pmf_b_fes_2nd_instruction_ipmf3_action_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_pmf_fes_data_2msb;
    uint32 key_value__field_fes_pgm_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_PMF_FES_DATA_2MSB, key_value__field_pmf_fes_data_2msb);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FES_PGM_ID, key_value__field_fes_pgm_id);
    *offset = key_value__field_pmf_fes_data_2msb*dnx_data_field.base_ipmf3.nof_fes_programs_get(unit)+key_value__field_fes_pgm_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_e_pmf_select_key_n_fes_field_e_pmf_fes_key_select_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fes_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FES_ID, key_value__field_fes_id);
    *offset = dnx_data_field.base_epmf.fes_instruction_size_get(unit)*key_value__field_fes_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_e_pmf_select_key_n_fes_field_fes_pgm_id_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fes_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FES_ID, key_value__field_fes_id);
    *offset = dnx_data_field.base_epmf.fes_instruction_size_get(unit)*key_value__field_fes_id+dnx_data_field.base_epmf.fes_pgm_id_offset_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_e_pmf_fes_2nd_instruction_null_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fes_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FES_ID, key_value__field_fes_id);
    *offset = dnx_data_field.base_epmf.nof_fes_instruction_per_context_get(unit)-1-key_value__field_fes_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_e_pmf_fes_2nd_instruction_null_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_pmf_fes_data_2msb;
    uint32 key_value__field_fes_pgm_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_PMF_FES_DATA_2MSB, key_value__field_pmf_fes_data_2msb);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FES_PGM_ID, key_value__field_fes_pgm_id);
    *offset = key_value__field_pmf_fes_data_2msb*dnx_data_field.base_epmf.nof_fes_programs_get(unit)+key_value__field_fes_pgm_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_ace_fes_program_table_field_fes_polarity_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_ace_result_type;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_ACE_RESULT_TYPE, key_value__field_ace_result_type);
    *offset = key_value__field_ace_result_type;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_ace_fes_program_table_field_fes_polarity_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fes_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FES_ID, key_value__field_fes_id);
    *offset = dnx_data_field.ace.fes_instruction_size_get(unit)*key_value__field_fes_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_ace_fes_program_table_field_fes_shift_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fes_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FES_ID, key_value__field_fes_id);
    *offset = dnx_data_field.ace.fes_instruction_size_get(unit)*key_value__field_fes_id+dnx_data_field.ace.fes_shift_offset_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_ace_fes_program_table_field_fes_invalid_bits_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fes_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FES_ID, key_value__field_fes_id);
    *offset = dnx_data_field.ace.fes_instruction_size_get(unit)*key_value__field_fes_id+dnx_data_field.ace.fes_invalid_bits_offset_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_ace_fes_program_table_field_fes_type_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fes_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FES_ID, key_value__field_fes_id);
    *offset = dnx_data_field.ace.fes_instruction_size_get(unit)*key_value__field_fes_id+dnx_data_field.ace.fes_type_offset_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_ace_fes_program_table_ace_action_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fes_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FES_ID, key_value__field_fes_id);
    *offset = dnx_data_field.ace.fes_instruction_size_get(unit)*key_value__field_fes_id+dnx_data_field.ace.fes_ace_action_offset_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_ace_fes_program_table_field_fes_chosen_mask_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fes_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FES_ID, key_value__field_fes_id);
    *offset = dnx_data_field.ace.fes_instruction_size_get(unit)*key_value__field_fes_id+dnx_data_field.ace.fes_chosen_mask_offset_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_ace_efes_action_mask_field_fes_action_mask_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fes_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FES_ID, key_value__field_fes_id);
    *offset = key_value__field_fes_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_ace_efes_action_mask_field_fes_action_mask_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_fes_mask_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_FES_MASK_ID, key_value__field_fes_mask_id);
    *offset = key_value__field_fes_mask_id*dnx_data_field.ace.nof_bits_in_fes_action_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
