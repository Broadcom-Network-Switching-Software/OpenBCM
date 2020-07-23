
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
fabric_mesh_topology_links_status_link_status_value_groupoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fabric_link;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FABRIC_LINK, key_value__fabric_link);
    *offset = key_value__fabric_link/2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_mesh_topology_links_status_link_status_value_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fabric_link;
    
    uint32 result_size__link_status_value = 0;
    
    uint32 result_size__link_status_is_valid = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FABRIC_LINK, key_value__fabric_link);
    
    result_size__link_status_value += 15;
    
    result_size__link_status_is_valid += 1;
    *offset = key_value__fabric_link%2*result_size__link_status_value+key_value__fabric_link%2*result_size__link_status_is_valid;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_mesh_topology_links_status_link_status_is_valid_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fabric_link;
    
    uint32 result_size__link_status_value = 0;
    
    uint32 result_size__link_status_is_valid = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FABRIC_LINK, key_value__fabric_link);
    
    result_size__link_status_value += 15;
    
    result_size__link_status_is_valid += 1;
    *offset = key_value__fabric_link%2*result_size__link_status_value+key_value__fabric_link%2*result_size__link_status_is_valid+result_size__link_status_value;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
