
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
oamp_pe_programs_properties_const_value_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__oamp_pe_program_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_OAMP_PE_PROGRAM_INDEX, key_value__oamp_pe_program_index);
    *offset = key_value__oamp_pe_program_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
oamp_pe_fems_fem_op_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__oam_pe_fem_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_OAM_PE_FEM_INDEX, key_value__oam_pe_fem_index);
    *offset = key_value__oam_pe_fem_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
oamp_pe_fems_fem_op_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    
    uint32 result_size__fem_op = 0;
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    
    result_size__fem_op += 6;
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = result_size__fem_op*instance_idx;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
oamp_mep_pe_profiles_byte_0_offset_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__mep_pe_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_MEP_PE_PROFILE, key_value__mep_pe_profile);
    *offset = key_value__mep_pe_profile;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
