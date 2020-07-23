
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
fabric_cgm_rci_enable_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pipe_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PIPE_ID, key_value__pipe_id);
    *offset = key_value__pipe_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_rci_thresholds_link_rci_threshold_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pipe;
    uint32 key_value__level;
    
    uint32 result_size__link_rci_threshold = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PIPE, key_value__pipe);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LEVEL, key_value__level);
    
    result_size__link_rci_threshold += dnx_data_fabric.dbal.link_rci_threshold_bits_nof_get(unit);
    *offset = key_value__pipe*result_size__link_rci_threshold*dnx_data_fabric.cgm.nof_rci_threshold_levels_get(unit)+key_value__level*result_size__link_rci_threshold;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_rci_thresholds_egress_rci_threshold_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pipe;
    uint32 key_value__level;
    
    uint32 result_size__egress_rci_threshold = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PIPE, key_value__pipe);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LEVEL, key_value__level);
    if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core))
    {
        
        result_size__egress_rci_threshold += 12;
    }
    *offset = key_value__pipe*result_size__egress_rci_threshold*dnx_data_fabric.cgm.nof_rci_threshold_levels_get(unit)+key_value__level*result_size__egress_rci_threshold;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_rci_thresholds_total_egress_rci_threshold_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pipe;
    uint32 key_value__level;
    
    uint32 result_size__total_egress_rci_threshold = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PIPE, key_value__pipe);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LEVEL, key_value__level);
    if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core))
    {
        
        result_size__total_egress_rci_threshold += 14;
    }
    *offset = key_value__pipe*result_size__total_egress_rci_threshold*dnx_data_fabric.cgm.nof_rci_threshold_levels_get(unit)+key_value__level*result_size__total_egress_rci_threshold;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_rci_local_thresholds_local_rci_threshold_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__level;
    
    uint32 result_size__local_rci_threshold = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LEVEL, key_value__level);
    
    result_size__local_rci_threshold += 8;
    *offset = key_value__level*result_size__local_rci_threshold;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_rci_local_thresholds_total_local_rci_threshold_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__level;
    
    uint32 result_size__total_local_rci_threshold = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LEVEL, key_value__level);
    if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core))
    {
        
        result_size__total_local_rci_threshold += 9;
    }
    *offset = key_value__level*result_size__total_local_rci_threshold;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_rci_score_mapping_link_rci_factor_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__level;
    
    uint32 result_size__link_rci_factor = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LEVEL, key_value__level);
    
    result_size__link_rci_factor += 3;
    *offset = key_value__level*result_size__link_rci_factor;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_rci_score_mapping_egress_rci_score_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__level;
    
    uint32 result_size__egress_rci_score = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LEVEL, key_value__level);
    if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core))
    {
        
        result_size__egress_rci_score += 14;
    }
    *offset = 30+key_value__level*result_size__egress_rci_score;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_rci_score_to_level_thresholds_core_threshold_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__level;
    
    uint32 result_size__core_threshold = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LEVEL, key_value__level);
    if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core))
    {
        
        result_size__core_threshold += 14;
    }
    *offset = key_value__level*result_size__core_threshold;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_rci_score_to_level_thresholds_device_threshold_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__level;
    
    uint32 result_size__device_threshold = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LEVEL, key_value__level);
    
    result_size__device_threshold += 14;
    *offset = key_value__level*result_size__device_threshold;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_gci_leaky_bucket_config_max_value_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__leaky_bucket_id;
    
    uint32 result_size__max_value = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LEAKY_BUCKET_ID, key_value__leaky_bucket_id);
    
    result_size__max_value += 8;
    *offset = key_value__leaky_bucket_id*result_size__max_value;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_gci_leaky_bucket_config_threshold_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__leaky_bucket_id;
    
    uint32 result_size__threshold = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LEAKY_BUCKET_ID, key_value__leaky_bucket_id);
    
    result_size__threshold += 8;
    *offset = key_value__leaky_bucket_id*result_size__threshold;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_gci_backoff_congestion_severity_thresholds_threshold_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__level_separate;
    
    uint32 result_size__threshold = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LEVEL_SEPARATE, key_value__level_separate);
    
    result_size__threshold += 10;
    *offset = key_value__level_separate*result_size__threshold;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_gci_backoff_timer_max_max_value_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__level;
    
    uint32 result_size__max_value = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LEVEL, key_value__level);
    
    result_size__max_value += 12;
    *offset = key_value__level*result_size__max_value;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_slow_start_enable_enable_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fmc_shaper_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FMC_SHAPER_ID, key_value__fmc_shaper_id);
    *offset = key_value__fmc_shaper_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_slow_start_config_timer_period_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__phase;
    
    uint32 result_size__timer_period = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PHASE, key_value__phase);
    
    result_size__timer_period += 5;
    *offset = 1+key_value__phase*result_size__timer_period;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_slow_start_config_delay_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__phase;
    
    uint32 result_size__delay = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PHASE, key_value__phase);
    
    result_size__delay += 16;
    *offset = 11+key_value__phase*result_size__delay;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_slow_start_config_calendar_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__phase;
    
    uint32 result_size__calendar = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PHASE, key_value__phase);
    
    result_size__calendar += 16;
    *offset = 43+key_value__phase*result_size__calendar;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_ing_transmit_per_core_shapers_config_null_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ing_transmit_shaper;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ING_TRANSMIT_SHAPER, key_value__ing_transmit_shaper);
    *offset = key_value__ing_transmit_shaper;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_llfc_ing_transmit_context_map_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fabric_subcontext_id;
    uint32 key_value__fabric_pipe_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FABRIC_SUBCONTEXT_ID, key_value__fabric_subcontext_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FABRIC_PIPE_ID, key_value__fabric_pipe_id);
    *offset = dnx_data_fabric.pipes.max_nof_pipes_get(unit)*key_value__fabric_subcontext_id+key_value__fabric_pipe_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_llfc_enable_enable_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__link;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LINK, key_value__link);
    *offset = key_value__link/dnx_data_fabric.blocks.nof_links_in_fmac_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_llfc_enable_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__link;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LINK, key_value__link);
    *offset = key_value__link%dnx_data_fabric.blocks.nof_links_in_fmac_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_llfc_thresholds_threshold_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pipe_id;
    
    uint32 result_size__threshold = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PIPE_ID, key_value__pipe_id);
    
    result_size__threshold += dnx_data_fabric.dbal.cgm_llfc_threshold_bits_nof_get(unit);
    *offset = result_size__threshold*key_value__pipe_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_egress_fifo_drop_thresholds_threshold_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__cast;
    uint32 key_value__fabric_priority;
    
    uint32 result_size__threshold = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CAST, key_value__cast);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FABRIC_PRIORITY, key_value__fabric_priority);
    
    result_size__threshold += dnx_data_fabric.dbal.egress_fifo_drop_threshold_bits_nof_get(unit);
    *offset = key_value__cast*result_size__threshold*dnx_data_fabric.cell.nof_priorities_get(unit)+key_value__fabric_priority*result_size__threshold;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_mesh_mc_fifo_drop_thresholds_threshold_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fabric_priority;
    
    uint32 result_size__threshold = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FABRIC_PRIORITY, key_value__fabric_priority);
    
    result_size__threshold += 8;
    *offset = result_size__threshold*key_value__fabric_priority;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_link_fifo_base_address_base_address_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pipe;
    
    uint32 result_size__base_address = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PIPE, key_value__pipe);
    
    result_size__base_address += dnx_data_fabric.dbal.cgm_link_fifo_base_address_bits_nof_get(unit);
    *offset = result_size__base_address*key_value__pipe-result_size__base_address;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_egress_fifo_base_address_base_address_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pipe;
    
    uint32 result_size__base_address = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PIPE, key_value__pipe);
    
    result_size__base_address += dnx_data_fabric.dbal.cgm_egress_fifo_base_address_bits_nof_get(unit);
    *offset = result_size__base_address*key_value__pipe-result_size__base_address;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_egress_fifo_fc_thresholds_threshold_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pipe;
    
    uint32 result_size__threshold = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PIPE, key_value__pipe);
    
    result_size__threshold += dnx_data_fabric.dbal.cgm_egress_fifo_fc_threshold_bits_nof_get(unit);
    *offset = result_size__threshold*key_value__pipe;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_egress_drop_filter_enable_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pipe_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PIPE_ID, key_value__pipe_id);
    *offset = key_value__pipe_id+1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_egress_drop_filter_enable_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = instance_idx/(dnx_data_fabric.links.nof_links_get(unit)/dnx_data_device.general.nof_cores_get(unit));
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_cgm_egress_drop_filter_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = instance_idx%(dnx_data_fabric.links.nof_links_get(unit)/dnx_data_device.general.nof_cores_get(unit));
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
