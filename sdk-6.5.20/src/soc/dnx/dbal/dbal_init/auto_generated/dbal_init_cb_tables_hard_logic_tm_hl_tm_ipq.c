
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
ipq_region_interdigitated_mode_is_interdigitated_mode_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__region_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_REGION_INDEX, key_value__region_index);
    *offset = key_value__region_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ipq_quartet_to_system_port_map_sys_port_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__queue_quartet;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_QUEUE_QUARTET, key_value__queue_quartet);
    *offset = key_value__queue_quartet;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ipq_system_port_to_queue_base_map_voq_base_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__sys_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SYS_PORT, key_value__sys_port);
    *offset = key_value__sys_port;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ipq_traffic_class_to_voq_offset_map_voq_offset_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__dp;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DP, key_value__dp);
    *offset = key_value__dp*dnx_data_ipq.queues.voq_offset_bits_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ipq_flow_quartet_to_flow_profile_map_flow_profile_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__voq_flow_quartet;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_VOQ_FLOW_QUARTET, key_value__voq_flow_quartet);
    *offset = key_value__voq_flow_quartet/4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ipq_flow_quartet_to_flow_profile_map_flow_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__voq_flow_quartet;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_VOQ_FLOW_QUARTET, key_value__voq_flow_quartet);
    *offset = key_value__voq_flow_quartet%4*dnx_data_ipq.tc_map.voq_flow_profiles_bits_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
