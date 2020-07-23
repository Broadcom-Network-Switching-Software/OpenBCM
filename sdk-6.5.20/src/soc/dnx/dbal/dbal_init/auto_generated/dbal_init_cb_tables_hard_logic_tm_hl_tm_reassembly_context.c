
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
ingr_reassembly_interleaved_reassembly_context_bitmap_active_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__interleaved_interface_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_INTERLEAVED_INTERFACE_ID, key_value__interleaved_interface_id);
    *offset = key_value__interleaved_interface_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ingr_reassembly_special_interface_base_address_reassembly_context_map_base_address_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__reassembly_special_interface_type;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_REASSEMBLY_SPECIAL_INTERFACE_TYPE, key_value__reassembly_special_interface_type);
    *offset = key_value__reassembly_special_interface_type;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
