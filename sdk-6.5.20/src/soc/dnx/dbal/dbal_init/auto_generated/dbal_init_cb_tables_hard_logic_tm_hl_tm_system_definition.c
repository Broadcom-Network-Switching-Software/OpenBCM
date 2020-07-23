
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
system_cfg_fap_index_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = 2*instance_idx;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
system_sysport_modport_map_null_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__dst_sys_port_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DST_SYS_PORT_ID, key_value__dst_sys_port_id);
    *offset = key_value__dst_sys_port_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
system_pp_dsp_mapping_pp_port_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pp_dsp;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PP_DSP, key_value__pp_dsp);
    *offset = key_value__pp_dsp;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
system_clos_dqcq_map_fap_id_local_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__core;
    uint32 key_value__local_indx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CORE, key_value__core);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LOCAL_INDX, key_value__local_indx);
    *offset = 16*key_value__core*dnx_data_device.general.fap_ids_per_core_bits_get(unit)+16*key_value__local_indx;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
system_clos_dqcq_map_fap_id_local_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__core;
    uint32 key_value__local_indx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CORE, key_value__core);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LOCAL_INDX, key_value__local_indx);
    *offset = 0+32*key_value__core*dnx_data_device.general.fap_ids_per_core_bits_get(unit)+32*key_value__local_indx;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
system_clos_dqcq_map_fap_id_local_dataoffset_2_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__core;
    uint32 key_value__local_indx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CORE, key_value__core);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LOCAL_INDX, key_value__local_indx);
    *offset = 16+32*key_value__core*dnx_data_device.general.fap_ids_per_core_bits_get(unit)+32*key_value__local_indx;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
system_clos_dqcq_map_mask_local_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__core;
    uint32 key_value__local_indx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CORE, key_value__core);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LOCAL_INDX, key_value__local_indx);
    *offset = 11*key_value__core*dnx_data_device.general.fap_ids_per_core_bits_get(unit)+11*key_value__local_indx;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
system_mesh_dqcq_map_system_mesh_context_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fap_id_lsb;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FAP_ID_LSB, key_value__fap_id_lsb);
    *offset = key_value__fap_id_lsb*4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
