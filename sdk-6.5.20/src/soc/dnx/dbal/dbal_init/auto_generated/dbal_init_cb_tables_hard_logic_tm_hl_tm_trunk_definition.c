
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
trunk_smooth_division_member_index_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__profile_index;
    uint32 key_value__entry_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PROFILE_INDEX, key_value__profile_index);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_INDEX, key_value__entry_index);
    *offset = key_value__profile_index*64+key_value__entry_index/4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
trunk_smooth_division_member_index_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_INDEX, key_value__entry_index);
    *offset = key_value__entry_index%4*4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
trunk_egress_mc_resolution_out_port_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_index;
    uint32 key_value__egress_trunk_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_INDEX, key_value__entry_index);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EGRESS_TRUNK_INDEX, key_value__egress_trunk_index);
    *offset = key_value__entry_index*2/dnx_data_trunk.egress_trunk.nof_lb_keys_per_profile_get(unit)*dnx_data_trunk.egress_trunk.dbal_entry_index_msb_multiplier_get(unit)+key_value__egress_trunk_index*dnx_data_trunk.egress_trunk.dbal_egress_trunk_index_multiplier_get(unit)+key_value__entry_index*2%dnx_data_trunk.egress_trunk.nof_lb_keys_per_profile_get(unit)/2/dnx_data_trunk.egress_trunk.dbal_entry_index_divider_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
trunk_egress_mc_resolution_out_port_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_INDEX, key_value__entry_index);
    *offset = key_value__entry_index%4*dnx_data_port.general.nof_port_bits_in_pp_bus_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
