
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
instru_ipt_profile_metadata_bitmap_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__profile;
    
    uint32 result_size__metadata_bitmap = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PROFILE, key_value__profile);
    
    result_size__metadata_bitmap += dnx_data_instru.ipt.metadata_bitmap_size_get(unit);
    *offset = key_value__profile*result_size__metadata_bitmap;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
instru_ipt_profile_metadata_size_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__profile;
    
    uint32 result_size__metadata_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PROFILE, key_value__profile);
    
    result_size__metadata_size += dnx_data_instru.ipt.metadata_edit_size_get(unit);
    *offset = key_value__profile*result_size__metadata_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
