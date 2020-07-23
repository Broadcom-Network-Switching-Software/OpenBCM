
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
load_balancing_crc_seeds_lb_crc_seed_dataoffset_0_cb(
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
    
    full_result_size += 16;
    *offset = full_key_value*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
load_balancing_use_layer_record_null_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    
    uint32 key_size__port_lb_profile = 0;
    
    uint32 key_size__in_lif_lb_profile = 0;
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__layer_protocol;
    uint32 key_value__port_lb_profile;
    uint32 key_value__in_lif_lb_profile;
    SHR_FUNC_INIT_VARS(unit);
    
    key_size__port_lb_profile += 2;
    
    key_size__in_lif_lb_profile += 2;
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LAYER_PROTOCOL, key_value__layer_protocol);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PORT_LB_PROFILE, key_value__port_lb_profile);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IN_LIF_LB_PROFILE, key_value__in_lif_lb_profile);
    *offset = (key_value__layer_protocol<<(key_size__port_lb_profile+key_size__in_lif_lb_profile))+(key_value__port_lb_profile<<key_size__in_lif_lb_profile)+key_value__in_lif_lb_profile;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
