
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
mact_vsi_counters_counter_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 21;
    *offset = full_key_value%4*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
vsi_learning_profile_vsi_learning_profile_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    *offset = full_key_value/16;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
vsi_learning_profile_vsi_learning_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 5;
    *offset = full_key_value%16*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mact_app_db_configuration_mact_app_db_index_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    *offset = full_key_value*2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mact_app_db_index_to_app_db_lem_app_db_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    *offset = full_key_value*6;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mact_aging_configuration_age_out_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__age_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_AGE_PROFILE, key_value__age_profile);
    *offset = key_value__age_profile*3+1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mact_aging_configuration_age_refresh_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__age_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_AGE_PROFILE, key_value__age_profile);
    *offset = key_value__age_profile*3+2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mact_aging_configuration_age_delete_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__age_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_AGE_PROFILE, key_value__age_profile);
    *offset = key_value__age_profile*3;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mact_limit_exceed_configuration_drop_if_exceed_mact_limit_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__limit_check_pass;
    uint32 key_value__is_host_request;
    uint32 key_value__is_static;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LIMIT_CHECK_PASS, key_value__limit_check_pass);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IS_HOST_REQUEST, key_value__is_host_request);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IS_STATIC, key_value__is_static);
    *offset = key_value__limit_check_pass*4+key_value__is_host_request*2+key_value__is_static;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
olp_dsp_event_route_olp_distribution_mc_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__mact_app_db_index;
    uint32 key_value__olp_command;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_MACT_APP_DB_INDEX, key_value__mact_app_db_index);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_OLP_COMMAND, key_value__olp_command);
    *offset = 8*key_value__mact_app_db_index+key_value__olp_command;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
olp_dsp_event_route_olp_distribution_mc_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__event_forwarding_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EVENT_FORWARDING_PROFILE, key_value__event_forwarding_profile);
    *offset = key_value__event_forwarding_profile*4+1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
olp_dsp_event_route_olp_distribution_cpu_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__event_forwarding_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EVENT_FORWARDING_PROFILE, key_value__event_forwarding_profile);
    *offset = key_value__event_forwarding_profile*4+2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
olp_dsp_event_route_olp_distribution_cpu_dma_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__event_forwarding_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EVENT_FORWARDING_PROFILE, key_value__event_forwarding_profile);
    *offset = key_value__event_forwarding_profile*4+3;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
olp_header_configuration_olp_header_padding_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__olp_command_destination;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_OLP_COMMAND_DESTINATION, key_value__olp_command_destination);
    *offset = key_value__olp_command_destination;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
l2_learn_payload_build_learn_ffc_instruction_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    
    uint32 result_size__learn_ffc_instruction = 0;
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    
    result_size__learn_ffc_instruction += 19;
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = result_size__learn_ffc_instruction*instance_idx;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ing_vsi_learn_learn_key_context_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    *offset = full_key_value%512;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
l2_learn_key_construction_mact_app_db_idx_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__learn_key_context;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LEARN_KEY_CONTEXT, key_value__learn_key_context);
    
    full_result_size += 2 + 3;
    *offset = key_value__learn_key_context*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
l2_learn_key_construction_vlan_source_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__learn_key_context;
    
    uint32 full_result_size = 0;
    
    uint32 result_size__mact_app_db_idx = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LEARN_KEY_CONTEXT, key_value__learn_key_context);
    
    full_result_size += 2 + 3;
    
    result_size__mact_app_db_idx += 2;
    *offset = key_value__learn_key_context*full_result_size+result_size__mact_app_db_idx;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
l2_learn_format_index_to_vmv_mdb_vmv_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__learn_format_index;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LEARN_FORMAT_INDEX, key_value__learn_format_index);
    
    full_result_size += 6;
    *offset = key_value__learn_format_index*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
l2_learn_format_index_from_vmv_learn_format_index_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__mdb_vmv;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_MDB_VMV, key_value__mdb_vmv);
    
    full_result_size += 2;
    *offset = key_value__mdb_vmv*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
