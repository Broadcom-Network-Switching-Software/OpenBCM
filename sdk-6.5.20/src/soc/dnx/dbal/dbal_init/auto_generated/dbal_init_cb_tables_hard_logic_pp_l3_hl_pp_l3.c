
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
ecmp_table_fec_pointer_base_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ecmp_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ECMP_ID, key_value__ecmp_id);
    *offset = key_value__ecmp_id/dnx_data_l3.ecmp.nof_ecmp_per_bank_of_extended_ecmps_get(unit)-32768/dnx_data_l3.ecmp.nof_ecmp_per_bank_of_extended_ecmps_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecmp_table_fec_pointer_base_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ecmp_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ECMP_ID, key_value__ecmp_id);
    *offset = key_value__ecmp_id % dnx_data_l3.ecmp.nof_ecmp_per_bank_of_extended_ecmps_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecmp_table_fec_pointer_base_arrayoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ecmp_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ECMP_ID, key_value__ecmp_id);
    *offset = 16+key_value__ecmp_id/dnx_data_l3.ecmp.nof_ecmp_per_bank_of_extended_ecmps_get(unit)-32768/dnx_data_l3.ecmp.nof_ecmp_per_bank_of_extended_ecmps_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecmp_table_fec_pointer_base_entryoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ecmp_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ECMP_ID, key_value__ecmp_id);
    *offset = key_value__ecmp_id%dnx_data_l3.ecmp.nof_ecmp_per_bank_of_extended_ecmps_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecmp_table_fec_pointer_base_arrayoffset_2_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ecmp_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ECMP_ID, key_value__ecmp_id);
    *offset = key_value__ecmp_id/dnx_data_l3.ecmp.nof_ecmp_per_bank_of_low_ecmps_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecmp_table_fec_pointer_base_entryoffset_2_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ecmp_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ECMP_ID, key_value__ecmp_id);
    *offset = key_value__ecmp_id%dnx_data_l3.ecmp.nof_ecmp_per_bank_of_low_ecmps_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecmp_group_profile_table_ecmp_mode_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__hierarchy_level;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_HIERARCHY_LEVEL, key_value__hierarchy_level);
    *offset = key_value__hierarchy_level;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecmp_group_profile_table_ecmp_mode_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ecmp_group_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ECMP_GROUP_PROFILE, key_value__ecmp_group_profile);
    *offset = key_value__ecmp_group_profile;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecmp_tunnel_priority_map_tp_index_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    
    uint32 key_size__tp_map_profile = 0;
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tp_value;
    uint32 key_value__tp_map_profile;
    
    uint32 result_size__tp_index = 0;
    SHR_FUNC_INIT_VARS(unit);
    
    key_size__tp_map_profile += 2;
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TP_VALUE, key_value__tp_value);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TP_MAP_PROFILE, key_value__tp_map_profile);
    
    result_size__tp_index += dnx_data_l3.ecmp.tunnel_priority_index_field_width_get(unit);
    *offset = result_size__tp_index*((key_value__tp_value<<key_size__tp_map_profile)+key_value__tp_map_profile);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fec_ecmp_members_ecmp_member_val_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ecmp_member_row_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ECMP_MEMBER_ROW_ID, key_value__ecmp_member_row_id);
    *offset = key_value__ecmp_member_row_id/256;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fec_ecmp_members_ecmp_member_val_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ecmp_member_row_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ECMP_MEMBER_ROW_ID, key_value__ecmp_member_row_id);
    *offset = key_value__ecmp_member_row_id%256;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fec_ecmp_members_ecmp_member_val_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    
    uint32 result_size__ecmp_member_val = 0;
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    
    result_size__ecmp_member_val += 4;
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = instance_idx*result_size__ecmp_member_val;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fec_ecmp_members_ecmp_member_val_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    
    uint32 result_size__ecmp_member_val = 0;
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    
    result_size__ecmp_member_val += 8;
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = instance_idx*result_size__ecmp_member_val;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecmp_bank_to_stage_map_hierarchy_level_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    
    uint32 result_size__hierarchy_level = 0;
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    
    result_size__hierarchy_level += 2;
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = instance_idx*result_size__hierarchy_level;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kaps_strength_mapping_strength_non_default_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__kaps_interface;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_KAPS_INTERFACE, key_value__kaps_interface);
    *offset = key_value__kaps_interface;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kaps_strength_mapping_strength_non_default_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__prefix_length;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PREFIX_LENGTH, key_value__prefix_length);
    *offset = key_value__prefix_length;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kaps_strength_mapping_strength_non_default_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lpm_profile;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LPM_PROFILE, key_value__lpm_profile);
    
    full_result_size += 3 + 3;
    *offset = key_value__lpm_profile*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kaps_strength_mapping_strength_default_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lpm_profile;
    
    uint32 full_result_size = 0;
    
    uint32 result_size__strength_non_default = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LPM_PROFILE, key_value__lpm_profile);
    
    full_result_size += 3 + 3;
    
    result_size__strength_non_default += 3;
    *offset = key_value__lpm_profile*full_result_size+result_size__strength_non_default;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kaps_lpm_profile_null_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__appdb_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_APPDB_ID, key_value__appdb_id);
    *offset = key_value__appdb_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fec_bank_to_stage_map_hierarchy_level_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    
    uint32 result_size__hierarchy_level = 0;
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    
    result_size__hierarchy_level += 2;
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = instance_idx*result_size__hierarchy_level;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
