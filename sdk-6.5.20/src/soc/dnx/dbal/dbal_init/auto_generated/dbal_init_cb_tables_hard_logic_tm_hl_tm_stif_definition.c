
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
stif_qsize_init_stif_qsize_init_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__stif_qsize_source;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_STIF_QSIZE_SOURCE, key_value__stif_qsize_source);
    *offset = key_value__stif_qsize_source;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
stif_billing_config_stif_billing_init_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__stif_billing_source;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_STIF_BILLING_SOURCE, key_value__stif_billing_source);
    *offset = key_value__stif_billing_source;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
stif_cgm_scrubber_thresholds_sram_buffers_th_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__scrubber_limit;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SCRUBBER_LIMIT, key_value__scrubber_limit);
    *offset = key_value__scrubber_limit;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
stif_compensation_masks_enable_compensation_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__stif_mask_header_delta;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_STIF_MASK_HEADER_DELTA, key_value__stif_mask_header_delta);
    *offset = key_value__stif_mask_header_delta;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
