
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
ecgm_multicast_queue_map_to_sp_mc_pd_sp_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ecgm_port_profile;
    uint32 key_value__ecgm_queue_offset;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ECGM_PORT_PROFILE, key_value__ecgm_port_profile);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ECGM_QUEUE_OFFSET, key_value__ecgm_queue_offset);
    *offset = key_value__ecgm_port_profile*8+key_value__ecgm_queue_offset;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecgm_map_mc_tc_to_db_sp_db_sp_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__mc_tc;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_MC_TC, key_value__mc_tc);
    *offset = key_value__mc_tc;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecgm_map_sys_tc_dp_to_mc_cos_params_mc_tc_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__sys_tc;
    uint32 key_value__sys_dp;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SYS_TC, key_value__sys_tc);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SYS_DP, key_value__sys_dp);
    *offset = key_value__sys_tc*4+key_value__sys_dp;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecgm_map_mc_se_shared_resource_eligibility_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__sp;
    uint32 key_value__sys_dp;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SP, key_value__sp);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SYS_DP, key_value__sys_dp);
    *offset = key_value__sp*4+key_value__sys_dp;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecgm_queue_pd_count_pd_count_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__is_mc;
    uint32 key_value__qpair;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IS_MC, key_value__is_mc);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_QPAIR, key_value__qpair);
    *offset = dnx_data_egr_queuing.params.nof_q_pairs_get(unit)*key_value__is_mc+key_value__qpair;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecgm_interface_resources_statistics_current_pd_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__is_mc;
    uint32 key_value__ecgm_priority;
    uint32 key_value__egq_if;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IS_MC, key_value__is_mc);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ECGM_PRIORITY, key_value__ecgm_priority);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EGQ_IF, key_value__egq_if);
    *offset = key_value__is_mc*2*dnx_data_egr_queuing.params.nof_egr_interfaces_get(unit)+key_value__ecgm_priority*dnx_data_egr_queuing.params.nof_egr_interfaces_get(unit)+key_value__egq_if;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecgm_port_resources_statistics_current_pd_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__is_mc;
    uint32 key_value__otm_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IS_MC, key_value__is_mc);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_OTM_PORT, key_value__otm_port);
    *offset = key_value__is_mc*dnx_data_egr_queuing.params.nof_q_pairs_get(unit)+key_value__otm_port;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecgm_queue_resources_statistics_current_pd_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__is_mc;
    uint32 key_value__qpair;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IS_MC, key_value__is_mc);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_QPAIR, key_value__qpair);
    *offset = key_value__is_mc*dnx_data_egr_queuing.params.nof_q_pairs_get(unit)+key_value__qpair;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ecgm_service_pool_tc_resources_statistics_current_pd_arrayoffset_0_cb(
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
    *offset = key_value__service_pool_id*8+key_value__tc;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
