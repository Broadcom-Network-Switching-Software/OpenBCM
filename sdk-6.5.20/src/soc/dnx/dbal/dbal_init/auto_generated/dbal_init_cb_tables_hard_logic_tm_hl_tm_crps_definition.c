
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
crps_engine_cfg_crps_engine_counter_format_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__crps_engine_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CRPS_ENGINE_ID, key_value__crps_engine_id);
    *offset = key_value__crps_engine_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
crps_itm_int_stat_ptr_map_stat_ptr_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__base_octet_voq;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_BASE_OCTET_VOQ, key_value__base_octet_voq);
    *offset = key_value__base_octet_voq/4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
crps_itm_int_stat_ptr_map_stat_ptr_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__base_octet_voq;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_BASE_OCTET_VOQ, key_value__base_octet_voq);
    *offset = key_value__base_octet_voq%4*dnx_data_ipq.queues.queue_id_bits_get(unit)-key_value__base_octet_voq%4*3;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
crps_engine_type_config_crps_engine_type_admission_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__stat_if_type_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_STAT_IF_TYPE_ID, key_value__stat_if_type_id);
    *offset = key_value__stat_if_type_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
crps_expansion_data_mapping_crps_expansion_data_admission_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__crps_expansion_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CRPS_EXPANSION_INDEX, key_value__crps_expansion_index);
    *offset = key_value__crps_expansion_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
crps_expansion_data_mapping_crps_expansion_data_admission_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__crps_engine_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CRPS_ENGINE_ID, key_value__crps_engine_id);
    *offset = key_value__crps_engine_id-dnx_data_crps.engine.nof_engines_get(unit)+dnx_data_crps.engine.nof_mid_engines_get(unit)+dnx_data_crps.engine.nof_big_engines_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
crps_expansion_data_mapping_crps_expansion_data_admission_arrayoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__crps_engine_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CRPS_ENGINE_ID, key_value__crps_engine_id);
    *offset = key_value__crps_engine_id-dnx_data_crps.engine.nof_engines_get(unit)+dnx_data_crps.engine.nof_big_engines_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
crps_irpp_compensation_select_packet_size_select_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__crps_command_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CRPS_COMMAND_ID, key_value__crps_command_id);
    *offset = key_value__crps_command_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
crps_irpp_filter_mask_filter_mask_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__filter_group_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FILTER_GROUP_ID, key_value__filter_group_id);
    *offset = key_value__filter_group_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
