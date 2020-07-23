
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
ecgm_core_multicast_sp_thresholds_pd_drop_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__service_pool_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SERVICE_POOL_ID, key_value__service_pool_id);
    *offset = dnx_data_ecgm.core_resources.total_pds_nof_bits_get(unit)*key_value__service_pool_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecgm_core_multicast_sp_thresholds_db_drop_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__service_pool_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SERVICE_POOL_ID, key_value__service_pool_id);
    *offset = dnx_data_ecgm.core_resources.total_dbs_nof_bits_get(unit)*key_value__service_pool_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecgm_core_tc_multicast_thresholds_pd_flow_control_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tc;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TC, key_value__tc);
    *offset = dnx_data_ecgm.core_resources.total_pds_nof_bits_get(unit)*key_value__tc;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecgm_core_tc_multicast_thresholds_db_flow_control_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tc;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TC, key_value__tc);
    *offset = dnx_data_ecgm.core_resources.total_dbs_nof_bits_get(unit)*key_value__tc;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecgm_core_tc_multicast_sp_thresholds_pd_drop_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__service_pool_id;
    uint32 key_value__tc;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SERVICE_POOL_ID, key_value__service_pool_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TC, key_value__tc);
    *offset = dnx_data_ecgm.core_resources.total_pds_nof_bits_get(unit)*8*key_value__service_pool_id+dnx_data_ecgm.core_resources.total_pds_nof_bits_get(unit)*key_value__tc;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecgm_core_tc_multicast_sp_thresholds_db_drop_max_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__service_pool_id;
    uint32 key_value__tc;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SERVICE_POOL_ID, key_value__service_pool_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TC, key_value__tc);
    *offset = dnx_data_ecgm.core_resources.total_dbs_nof_bits_get(unit)*8*key_value__service_pool_id+dnx_data_ecgm.core_resources.total_dbs_nof_bits_get(unit)*key_value__tc;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecgm_core_tc_multicast_sp_thresholds_db_drop_alpha_0_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__service_pool_id;
    uint32 key_value__tc;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SERVICE_POOL_ID, key_value__service_pool_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TC, key_value__tc);
    *offset = key_value__service_pool_id*32+key_value__tc*4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecgm_interface_unicast_thresholds_pd_flow_control_max_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__interface_profile;
    uint32 key_value__ecgm_priority;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_INTERFACE_PROFILE, key_value__interface_profile);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ECGM_PRIORITY, key_value__ecgm_priority);
    *offset = key_value__interface_profile*2+key_value__ecgm_priority;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecgm_egq_if_to_interface_profile_map_interface_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__egq_if;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EGQ_IF, key_value__egq_if);
    *offset = key_value__egq_if*3;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecgm_queue_disable_is_queue_disabled_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__qpair;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_QPAIR, key_value__qpair);
    *offset = key_value__qpair;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecgm_queue_disable_is_queue_disabled_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__qpair;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_QPAIR, key_value__qpair);
    *offset = key_value__qpair-dnx_data_egr_queuing.params.nof_q_pairs_get(unit)/2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecgm_unicast_port_thresholds_pd_drop_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ecgm_port_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ECGM_PORT_PROFILE, key_value__ecgm_port_profile);
    *offset = key_value__ecgm_port_profile;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecgm_base_q_pair_to_port_profile_map_ecgm_port_profile_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__base_q_pair;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_BASE_Q_PAIR, key_value__base_q_pair);
    *offset = key_value__base_q_pair;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecgm_multicast_queue_tc_dp_thresholds_pd_drop_alpha_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__dp;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DP, key_value__dp);
    *offset = key_value__dp*4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecgm_multicast_queue_tc_dp_thresholds_db_drop_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__dp;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DP, key_value__dp);
    *offset = key_value__dp*22;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
