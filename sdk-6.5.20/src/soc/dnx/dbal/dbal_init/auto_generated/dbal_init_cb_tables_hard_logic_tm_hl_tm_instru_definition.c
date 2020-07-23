
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__counters_record_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_COUNTERS_RECORD_INDEX, key_value__counters_record_index);
    *offset = key_value__counters_record_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__counters_record_index;
    uint32 key_value__phys_port_in_ethu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_COUNTERS_RECORD_INDEX, key_value__counters_record_index);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PHYS_PORT_IN_ETHU, key_value__phys_port_in_ethu);
    *offset = ((key_value__counters_record_index*(dnx_data_nif.eth.nof_lanes_in_cdu_get(unit)))+key_value__phys_port_in_ethu);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__counters_record_index;
    uint32 key_value__phys_port_in_ethu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_COUNTERS_RECORD_INDEX, key_value__counters_record_index);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PHYS_PORT_IN_ETHU, key_value__phys_port_in_ethu);
    *offset = ((key_value__counters_record_index*16)+key_value__phys_port_in_ethu);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
