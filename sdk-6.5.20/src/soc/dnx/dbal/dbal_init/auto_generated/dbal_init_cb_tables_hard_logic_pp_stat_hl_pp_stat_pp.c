
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
stat_pp_irpp_profile_info_stat_object_metadata_size_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__stat_object_metadata_shift = 0;
    
    uint32 result_size__stat_object_metadata_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__stat_object_metadata_shift += 4;
    
    result_size__stat_object_metadata_size += 2;
    *offset = result_size__stat_object_metadata_shift*full_key_value+result_size__stat_object_metadata_size*full_key_value+result_size__stat_object_metadata_shift;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
stat_pp_irpp_profile_info_stat_object_metadata_shift_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__stat_object_metadata_shift = 0;
    
    uint32 result_size__stat_object_metadata_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__stat_object_metadata_shift += 4;
    
    result_size__stat_object_metadata_size += 2;
    *offset = result_size__stat_object_metadata_shift*full_key_value+result_size__stat_object_metadata_size*full_key_value;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
stat_pp_irpp_profile_info_fwd_metadata_size_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__fwd_metadata_shift = 0;
    
    uint32 result_size__fwd_metadata_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__fwd_metadata_shift += 4;
    
    result_size__fwd_metadata_size += 2;
    *offset = result_size__fwd_metadata_shift*full_key_value+result_size__fwd_metadata_size*full_key_value;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
stat_pp_irpp_profile_info_fwd_metadata_shift_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__fwd_metadata_shift = 0;
    
    uint32 result_size__fwd_metadata_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__fwd_metadata_shift += 4;
    
    result_size__fwd_metadata_size += 2;
    *offset = result_size__fwd_metadata_shift*full_key_value+result_size__fwd_metadata_size*full_key_value+result_size__fwd_metadata_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
stat_pp_irpp_profile_info_fwd_plus_one_metadata_size_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__fwd_plus_one_metadata_shift = 0;
    
    uint32 result_size__fwd_plus_one_metadata_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__fwd_plus_one_metadata_shift += 4;
    
    result_size__fwd_plus_one_metadata_size += 2;
    *offset = result_size__fwd_plus_one_metadata_shift*full_key_value+result_size__fwd_plus_one_metadata_size*full_key_value;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
stat_pp_irpp_profile_info_fwd_plus_one_metadata_shift_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__fwd_plus_one_metadata_shift = 0;
    
    uint32 result_size__fwd_plus_one_metadata_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__fwd_plus_one_metadata_shift += 4;
    
    result_size__fwd_plus_one_metadata_size += 2;
    *offset = result_size__fwd_plus_one_metadata_shift*full_key_value+result_size__fwd_plus_one_metadata_size*full_key_value+result_size__fwd_plus_one_metadata_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
stat_pp_etpp_profile_info_stat_if_object_id_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__stat_if_object_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__stat_if_object_id += 4;
    *offset = result_size__stat_if_object_id*full_key_value;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
stat_pp_etpp_profile_info_stat_if_type_id_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__stat_if_type_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__stat_if_type_id += 2;
    *offset = result_size__stat_if_type_id*full_key_value;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
stat_pp_etpp_profile_info_is_meter_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__is_meter = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__is_meter += 1;
    *offset = result_size__is_meter*full_key_value;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
stat_pp_etpp_profile_info_meter_interface_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__meter_interface = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__meter_interface += 1;
    *offset = result_size__meter_interface*full_key_value;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
stat_pp_etpp_profile_info_meter_qos_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__meter_qos_profile = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__meter_qos_profile += 3;
    *offset = result_size__meter_qos_profile*full_key_value;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
stat_pp_irpp_vtt_statistics_metadata_value_map_stat_metadata_val_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__stat_metadata_val = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__stat_metadata_val += 3;
    *offset = full_key_value*result_size__stat_metadata_val;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
stat_pp_irpp_fwd_statistics_metadata_value_map_stat_metadata_val_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__stat_metadata_val = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__stat_metadata_val += 3;
    *offset = full_key_value*result_size__stat_metadata_val;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
stat_pp_irpp_fwd_plus_one_statistics_metadata_value_map_stat_metadata_val_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__stat_metadata_val = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__stat_metadata_val += 3;
    *offset = full_key_value*result_size__stat_metadata_val;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
stat_pp_irpp_fec_ecmp_statistics_profile_map_fec_statistic_profile_map_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__fec_statistic_profile_map = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__fec_statistic_profile_map += 2;
    *offset = full_key_value*result_size__fec_statistic_profile_map;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
stat_pp_irpp_fec_ecmp_statistics_profile_map_ecmp_statistic_profile_map_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__ecmp_statistic_profile_map = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__ecmp_statistic_profile_map += 2;
    *offset = full_key_value*result_size__ecmp_statistic_profile_map;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
stat_pp_irpp_fec_ecmp_statistics_profile_map_fec_take_primary_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__fec_take_primary = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__fec_take_primary += 1;
    *offset = full_key_value*result_size__fec_take_primary;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
stat_pp_etpp_fwd_statistics_metadata_value_map_stat_metadata_val_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__stat_metadata_val = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__stat_metadata_val += 3;
    *offset = full_key_value*result_size__stat_metadata_val;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
stat_pp_etpp_enc_statistics_metadata_value_map_stat_metadata_val_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__stat_metadata_val = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__stat_metadata_val += 3;
    *offset = full_key_value*result_size__stat_metadata_val;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
