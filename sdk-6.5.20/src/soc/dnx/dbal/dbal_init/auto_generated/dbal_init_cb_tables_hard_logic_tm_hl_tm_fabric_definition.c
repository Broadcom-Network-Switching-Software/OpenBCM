
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
fabric_pipes_mapping_pipe_id_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__cast;
    uint32 key_value__fabric_priority;
    
    uint32 result_size__pipe_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CAST, key_value__cast);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FABRIC_PRIORITY, key_value__fabric_priority);
    
    result_size__pipe_id += 2;
    *offset = 19+result_size__pipe_id*dnx_data_fabric.cell.nof_priorities_get(unit)*key_value__cast+result_size__pipe_id*key_value__fabric_priority;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_pipes_mapping_pipe_id_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__cast;
    uint32 key_value__fabric_priority;
    
    uint32 result_size__pipe_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CAST, key_value__cast);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FABRIC_PRIORITY, key_value__fabric_priority);
    
    result_size__pipe_id += 2;
    *offset = result_size__pipe_id*dnx_data_fabric.cell.nof_priorities_get(unit)*key_value__cast+result_size__pipe_id*key_value__fabric_priority;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_pcp_config_fabric_pcp_mode_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fap_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FAP_ID, key_value__fap_id);
    *offset = key_value__fap_id/16;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_pcp_config_fabric_pcp_mode_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fap_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FAP_ID, key_value__fap_id);
    *offset = key_value__fap_id%16*2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_wfq_context_enable_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__context_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CONTEXT_ID, key_value__context_id);
    *offset = key_value__context_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_wfq_contexts_weights_weight_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__context_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CONTEXT_ID, key_value__context_id);
    *offset = 3+key_value__context_id*7;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_wfq_pipes_weights_weight_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pipe_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PIPE_ID, key_value__pipe_id);
    *offset = key_value__pipe_id*7;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_fault_links_fault_local_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fabric_link;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FABRIC_LINK, key_value__fabric_link);
    *offset = key_value__fabric_link;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_sr_cell_receive_data_sr_cell_data_lsb_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fabric_sr_cell_instance;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FABRIC_SR_CELL_INSTANCE, key_value__fabric_sr_cell_instance);
    *offset = key_value__fabric_sr_cell_instance%dnx_data_fabric.cell.sr_cell_nof_instances_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_sr_cell_receive_data_sr_cell_data_lsb_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fabric_sr_cell_instance;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FABRIC_SR_CELL_INSTANCE, key_value__fabric_sr_cell_instance);
    *offset = key_value__fabric_sr_cell_instance/dnx_data_fabric.cell.sr_cell_nof_instances_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_sr_cell_receive_control_sr_cell_data_ready_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fabric_sr_cell_instance;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FABRIC_SR_CELL_INSTANCE, key_value__fabric_sr_cell_instance);
    *offset = 1+4*(key_value__fabric_sr_cell_instance%dnx_data_fabric.cell.sr_cell_nof_instances_get(unit));
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_sr_cell_receive_control_sr_cell_data_ready_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fabric_sr_cell_instance;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FABRIC_SR_CELL_INSTANCE, key_value__fabric_sr_cell_instance);
    *offset = 11+key_value__fabric_sr_cell_instance%dnx_data_fabric.cell.sr_cell_nof_instances_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_mesh_dev_id_local_mesh_mode_dev_id_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    
    uint32 result_size__mesh_mode_dev_id = 0;
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    
    result_size__mesh_mode_dev_id += 11;
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = result_size__mesh_mode_dev_id*instance_idx;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_mesh_dev_id_dest_mesh_mode_dev_id_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    
    uint32 result_size__mesh_mode_dev_id = 0;
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    
    result_size__mesh_mode_dev_id += 11;
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = 44+result_size__mesh_mode_dev_id*instance_idx;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_mesh_dev_id_dest_mesh_mode_dev_id_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    
    uint32 result_size__mesh_mode_dev_id = 0;
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    
    result_size__mesh_mode_dev_id += 11;
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = 88+result_size__mesh_mode_dev_id*instance_idx;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_mesh_dev_id_dest_mesh_mode_dev_id_dataoffset_2_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    
    uint32 result_size__mesh_mode_dev_id = 0;
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    
    result_size__mesh_mode_dev_id += 11;
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = 132+result_size__mesh_mode_dev_id*instance_idx;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_mesh_dev_id_dest_mesh_mode_dev_id_dataoffset_3_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    
    uint32 result_size__mesh_mode_dev_id = 0;
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    
    result_size__mesh_mode_dev_id += 11;
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = 22+result_size__mesh_mode_dev_id*instance_idx;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_mesh_dev_id_dest_mesh_mode_dev_id_dataoffset_4_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    
    uint32 result_size__mesh_mode_dev_id = 0;
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    
    result_size__mesh_mode_dev_id += 11;
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = 66+result_size__mesh_mode_dev_id*instance_idx;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_mesh_mc_db_mesh_mc_rep_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__mesh_mc_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_MESH_MC_ID, key_value__mesh_mc_id);
    *offset = key_value__mesh_mc_id/32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_mesh_mc_db_mesh_mc_rep_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__mesh_mc_id;
    
    uint32 result_size__mesh_mc_rep = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_MESH_MC_ID, key_value__mesh_mc_id);
    
    result_size__mesh_mc_rep += dnx_data_fabric.mesh_mc.nof_replication_get(unit);
    *offset = key_value__mesh_mc_id%32*result_size__mesh_mc_rep;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_mesh_replication_link_map_mesh_link_enable_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__mesh_dest_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_MESH_DEST_ID, key_value__mesh_dest_id);
    *offset = key_value__mesh_dest_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_mesh_fap_id_to_context_map_group_context_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    
    uint32 result_size__group_context = 0;
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    
    result_size__group_context += 4;
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = result_size__group_context*instance_idx;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_min_links_to_dest_min_nof_links_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fap_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FAP_ID, key_value__fap_id);
    *offset = key_value__fap_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_interleaving_enable_subcontext_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fabric_subcontext_id;
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FABRIC_SUBCONTEXT_ID, key_value__fabric_subcontext_id);
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = instance_idx*dnx_data_fabric.pipes.max_nof_subcontexts_get(unit)+key_value__fabric_subcontext_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_interleaving_enable_subcontext_tdm_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fabric_subcontext_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FABRIC_SUBCONTEXT_ID, key_value__fabric_subcontext_id);
    *offset = key_value__fabric_subcontext_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
