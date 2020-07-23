
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
system_header_global_cfg_udh_udh_map_type_to_length_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__udh_type;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_UDH_TYPE, key_value__udh_type);
    *offset = key_value__udh_type*3;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
jr_vsi_src_mapping_table_jr_vsi_src_dataoffset_0_cb(
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
    if (1-dnx_data_field.init.jr1_ipmc_inlif_get(unit))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += dnx_data_headers.pph.pph_vsi_selection_size_get(unit);
    *offset = full_result_size*full_key_value;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
jr_vsi_src_mapping_table_jr_vsi_src_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__jr_vsi_src = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__jr_vsi_src += dnx_data_headers.pph.pph_vsi_selection_size_get(unit);
    *offset = result_size__jr_vsi_src*full_key_value;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
jr_vsi_src_mapping_table_jr_in_lif_src_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    
    uint32 result_size__jr_vsi_src = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    if (1-dnx_data_field.init.jr1_ipmc_inlif_get(unit))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += dnx_data_headers.pph.pph_vsi_selection_size_get(unit);
    
    result_size__jr_vsi_src += dnx_data_headers.pph.pph_vsi_selection_size_get(unit);
    *offset = full_result_size*full_key_value+result_size__jr_vsi_src;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
jr_fhei_size_mapping_fhei_size_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__fhei_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__fhei_size += dnx_data_aod_sizes.AOD.sizes_get(unit, 58)->value;
    *offset = result_size__fhei_size*full_key_value;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
