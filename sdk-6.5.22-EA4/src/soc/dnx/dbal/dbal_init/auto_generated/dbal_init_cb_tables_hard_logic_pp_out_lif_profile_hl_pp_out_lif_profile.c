/** \file dbal_init_cb_tables_hard_logic_pp_out_lif_profile_hl_pp_out_lif_profile.c
 * Logical DB enums \n 
 * DO NOT EDIT THIS FILE!\n 
 * This file is auto-generated.\n 
 * Edits to this file will be lost when it is regenerated.\n 
 * \n 
 */
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

shr_error_e
etpp_out_lif_profile_table_out_lif_orientation_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__out_lif_profile;
    /** size is calculated by the Autocoder  */
    uint32 result_size__out_lif_orientation = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_OUT_LIF_PROFILE, key_value__out_lif_profile);
    /** size of OUT_LIF_ORIENTATION field  */
    result_size__out_lif_orientation += 3;
    *offset = key_value__out_lif_profile%64*result_size__out_lif_orientation;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
etpp_out_lif_profile_table_oam_priority_map_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    /** size is calculated by the Autocoder  */
    uint32 result_size__oam_priority_map_profile = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    /** size of OAM_PRIORITY_MAP_PROFILE field  */
    result_size__oam_priority_map_profile += 2;
    *offset = full_key_value*result_size__oam_priority_map_profile;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
erpp_ttl_scope_table_ttl_scope_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__outlif_ttl_scope_index;
    /** size is calculated by the Autocoder  */
    uint32 result_size__ttl_scope = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_OUTLIF_TTL_SCOPE_INDEX, key_value__outlif_ttl_scope_index);
    /** size of TTL_SCOPE field  */
    result_size__ttl_scope += 8;
    *offset = result_size__ttl_scope*key_value__outlif_ttl_scope_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egress_glob_out_lif_bta_sop_table_bta_sop_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__glob_out_lif_msb;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_GLOB_OUT_LIF_MSB, key_value__glob_out_lif_msb);
    *offset = key_value__glob_out_lif_msb;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
