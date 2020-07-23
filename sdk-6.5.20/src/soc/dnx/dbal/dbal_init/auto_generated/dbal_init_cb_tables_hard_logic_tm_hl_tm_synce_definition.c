
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
synce_counter_null_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__synce_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SYNCE_INDEX, key_value__synce_index);
    *offset = key_value__synce_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
synce_ref_clk_nif_chain_enable_enable_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__nif_synce_clk_blk;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_NIF_SYNCE_CLK_BLK, key_value__nif_synce_clk_blk);
    *offset = key_value__nif_synce_clk_blk;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
synce_ref_clk_nif_chain_enable_enable_blockindex_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__nif_synce_clk_blk;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_NIF_SYNCE_CLK_BLK, key_value__nif_synce_clk_blk);
    *offset = key_value__nif_synce_clk_blk-dnx_data_nif.eth.cdu_nof_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
synce_ref_clk_nif_chain_enable_enable_blockindex_2_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__nif_synce_clk_blk;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_NIF_SYNCE_CLK_BLK, key_value__nif_synce_clk_blk);
    *offset = key_value__nif_synce_clk_blk-1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
synce_ref_clk_nif_chain_enable_enable_blockindex_3_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__nif_synce_clk_blk;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_NIF_SYNCE_CLK_BLK, key_value__nif_synce_clk_blk);
    *offset = key_value__nif_synce_clk_blk-2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
synce_ref_clk_nif_chain_enable_enable_blockindex_4_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__nif_synce_clk_blk;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_NIF_SYNCE_CLK_BLK, key_value__nif_synce_clk_blk);
    *offset = key_value__nif_synce_clk_blk/6;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_synce_ref_clk_fsrd_ctrl_master_clk_sel_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fabric_synce_clk_blk;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FABRIC_SYNCE_CLK_BLK, key_value__fabric_synce_clk_blk);
    *offset = key_value__fabric_synce_clk_blk;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
