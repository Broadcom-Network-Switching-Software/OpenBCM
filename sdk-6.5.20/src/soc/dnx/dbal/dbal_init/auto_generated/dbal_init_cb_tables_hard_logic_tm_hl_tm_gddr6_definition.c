
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
gddr6_dram_pll_reset_dram_pll_resetb_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__dram_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DRAM_INDEX, key_value__dram_index);
    *offset = key_value__dram_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
gddr6_enable_dynamic_memory_access_enable_dynamic_memory_access_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__dram_index;
    uint32 key_value__channel;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DRAM_INDEX, key_value__dram_index);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CHANNEL, key_value__channel);
    *offset = dnx_data_dram.general_info.nof_channels_get(unit)*key_value__dram_index+key_value__channel;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
gddr6_dram_phy_register_access_dram_phy_register_access_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__phy_reg_address;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PHY_REG_ADDRESS, key_value__phy_reg_address);
    *offset = key_value__phy_reg_address;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
gddr6_dram_bist_config_pattern_2_bist_dq_pattern_n_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = instance_idx+4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
gddr6_phy_write_max_vdl_data_ui_shift_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__channel;
    uint32 key_value__dbyte;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CHANNEL, key_value__channel);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DBYTE, key_value__dbyte);
    *offset = 0xa + 0x200*key_value__channel + 0x100*key_value__dbyte;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
