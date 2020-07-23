
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
fabric_control_cells_llfc_enable_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__mac;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_MAC, key_value__mac);
    *offset = key_value__mac;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_control_cells_reachability_status_is_reachable_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fap_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FAP_ID, key_value__fap_id);
    *offset = key_value__fap_id/2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_control_cells_reachability_status_is_reachable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fap_id;
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FAP_ID, key_value__fap_id);
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = key_value__fap_id%2*dnx_data_fabric.links.nof_links_get(unit)+instance_idx;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_control_cells_connectivity_status_null_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fabric_link_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FABRIC_LINK_ID, key_value__fabric_link_id);
    *offset = key_value__fabric_link_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
