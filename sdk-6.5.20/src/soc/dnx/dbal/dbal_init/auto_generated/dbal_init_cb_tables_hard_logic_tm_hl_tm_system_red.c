
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
system_red_q_size_to_voq_size_range_map_voq_size_range_max_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__voq_rate_class;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_VOQ_RATE_CLASS, key_value__voq_rate_class);
    *offset = key_value__voq_rate_class;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
system_red_q_size_to_voq_size_range_map_voq_size_range_max_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__red_q_size_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_RED_Q_SIZE_INDEX, key_value__red_q_size_index);
    *offset = key_value__red_q_size_index*24;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
system_red_free_res_thr_pdbs_thr_value_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__thr_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_THR_INDEX, key_value__thr_index);
    *offset = dnx_data_system_red.info.dbal_get(unit, 0)->offset+key_value__thr_index*dnx_data_system_red.info.resource_get(unit, 0)->hw_resolution_nof_bits+key_value__thr_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
system_red_free_res_thr_sram_buffers_thr_value_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__thr_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_THR_INDEX, key_value__thr_index);
    *offset = dnx_data_system_red.info.dbal_get(unit, 1)->offset+key_value__thr_index*dnx_data_system_red.info.resource_get(unit, 1)->hw_resolution_nof_bits+key_value__thr_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
system_red_free_res_thr_dram_bdbs_thr_value_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__thr_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_THR_INDEX, key_value__thr_index);
    *offset = dnx_data_system_red.info.dbal_get(unit, 2)->offset+key_value__thr_index*dnx_data_system_red.info.resource_get(unit, 2)->hw_resolution_nof_bits+key_value__thr_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
system_red_free_res_q_size_index_red_q_size_index_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__res_type;
    uint32 key_value__range_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_RES_TYPE, key_value__res_type);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_RANGE_INDEX, key_value__range_index);
    *offset = key_value__res_type*16+key_value__range_index*4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
system_red_max_red_q_size_per_sysport_max_red_index_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__sys_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SYS_PORT, key_value__sys_port);
    *offset = key_value__sys_port/16;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
system_red_max_red_q_size_per_sysport_max_red_index_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__sys_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SYS_PORT, key_value__sys_port);
    *offset = key_value__sys_port%16*6;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
