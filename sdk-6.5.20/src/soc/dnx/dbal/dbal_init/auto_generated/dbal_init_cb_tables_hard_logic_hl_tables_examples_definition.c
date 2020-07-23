
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
example_arr_prefix_and_cb_destination_encoding_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tc;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TC, key_value__tc);
    *offset = key_value__tc;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
example_table_register_group_threshold_groupoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__rmc;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_RMC, key_value__rmc);
    *offset = key_value__rmc;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
example_table_register_group_threshold_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__core_id;
    uint32 key_value__ethu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CORE_ID, key_value__core_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ETHU, key_value__ethu);
    *offset = key_value__core_id*5+key_value__ethu-1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
example_table_register_group_threshold_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__priority;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PRIORITY, key_value__priority);
    *offset = key_value__priority*15;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
example_table_register_group_threshold_blockindex_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__core_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CORE_ID, key_value__core_id);
    *offset = key_value__core_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
example_table_register_group_threshold_blockindex_2_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ethu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ETHU, key_value__ethu);
    *offset = key_value__ethu;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
example_table_register_group_threshold_blockindex_3_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ethu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ETHU, key_value__ethu);
    *offset = key_value__ethu-dnx_data_nif.eth.cdu_nof_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    *offset = full_key_value%4*64;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
example_for_hl_with_memory_aliasing_mpls_out_cmd_aliasmemoryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    *offset = full_key_value%4*0X40;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
example_for_dnx_data_ref_res_field_a_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    uint32 key_value__key_field_a;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_KEY_FIELD_A, key_value__key_field_a);
    *offset = key_value__key_field_a*1-full_key_value+dnx_data_module_testing.dbal.all_get(unit)->key_max_val;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
