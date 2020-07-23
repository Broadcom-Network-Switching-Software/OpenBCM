
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
ingress_pp_port_learn_lif_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pp_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PP_PORT, key_value__pp_port);
    *offset = key_value__pp_port%512;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ingress_pp_port_visibility_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pp_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PP_PORT, key_value__pp_port);
    *offset = key_value__pp_port%256;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egress_pp_port_vlan_membership_enable_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pp_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PP_PORT, key_value__pp_port);
    *offset = key_value__pp_port/256;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ingress_ptc_port_ptc_cs_profile_vtt1_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ptc;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PTC, key_value__ptc);
    *offset = key_value__ptc/4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ingress_ptc_port_ptc_cs_profile_vtt1_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ptc;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PTC, key_value__ptc);
    *offset = key_value__ptc%4*4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ingress_ptc_port_ptc_routing_enable_profile_vtt1_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ptc;
    
    uint32 result_size__ptc_routing_enable_profile_vtt1 = 0;
    
    uint32 result_size__ptc_cs_profile_vtt2 = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PTC, key_value__ptc);
    
    result_size__ptc_routing_enable_profile_vtt1 += dnx_data_aod_sizes.AOD.sizes_get(unit, 184)->value;
    
    result_size__ptc_cs_profile_vtt2 += dnx_data_aod_sizes.AOD.sizes_get(unit, 178)->value;
    *offset = key_value__ptc%128*(result_size__ptc_routing_enable_profile_vtt1+result_size__ptc_cs_profile_vtt2)+result_size__ptc_cs_profile_vtt2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ingress_ptc_port_ptc_routing_enable_profile_vtt1_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ptc;
    
    uint32 result_size__ptc_routing_enable_profile_vtt1 = 0;
    
    uint32 result_size__ptc_cs_profile_vtt2 = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PTC, key_value__ptc);
    
    result_size__ptc_routing_enable_profile_vtt1 += dnx_data_aod_sizes.AOD.sizes_get(unit, 184)->value;
    
    result_size__ptc_cs_profile_vtt2 += dnx_data_aod_sizes.AOD.sizes_get(unit, 178)->value;
    *offset = key_value__ptc*(result_size__ptc_routing_enable_profile_vtt1+result_size__ptc_cs_profile_vtt2)+result_size__ptc_cs_profile_vtt2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ingress_ptc_port_ptc_routing_enable_profile_vtt1_dataoffset_2_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ptc;
    
    uint32 result_size__ptc_routing_enable_profile_vtt1 = 0;
    
    uint32 result_size__ptc_cs_profile_vtt2 = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PTC, key_value__ptc);
    
    result_size__ptc_routing_enable_profile_vtt1 += dnx_data_aod_sizes.AOD.sizes_get(unit, 184)->value;
    
    result_size__ptc_cs_profile_vtt2 += dnx_data_aod_sizes.AOD.sizes_get(unit, 178)->value;
    *offset = key_value__ptc%4*(result_size__ptc_routing_enable_profile_vtt1+result_size__ptc_cs_profile_vtt2)+result_size__ptc_cs_profile_vtt2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ingress_ptc_port_ptc_cs_profile_vtt2_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ptc;
    
    uint32 result_size__ptc_routing_enable_profile_vtt1 = 0;
    
    uint32 result_size__ptc_cs_profile_vtt2 = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PTC, key_value__ptc);
    
    result_size__ptc_routing_enable_profile_vtt1 += dnx_data_aod_sizes.AOD.sizes_get(unit, 184)->value;
    
    result_size__ptc_cs_profile_vtt2 += dnx_data_aod_sizes.AOD.sizes_get(unit, 178)->value;
    *offset = key_value__ptc%128*(result_size__ptc_routing_enable_profile_vtt1+result_size__ptc_cs_profile_vtt2);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ingress_ptc_port_ptc_cs_profile_vtt2_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ptc;
    
    uint32 result_size__ptc_routing_enable_profile_vtt1 = 0;
    
    uint32 result_size__ptc_cs_profile_vtt2 = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PTC, key_value__ptc);
    
    result_size__ptc_routing_enable_profile_vtt1 += dnx_data_aod_sizes.AOD.sizes_get(unit, 184)->value;
    
    result_size__ptc_cs_profile_vtt2 += dnx_data_aod_sizes.AOD.sizes_get(unit, 178)->value;
    *offset = key_value__ptc*(result_size__ptc_routing_enable_profile_vtt1+result_size__ptc_cs_profile_vtt2);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ingress_ptc_port_ptc_cs_profile_vtt2_dataoffset_2_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ptc;
    
    uint32 result_size__ptc_routing_enable_profile_vtt1 = 0;
    
    uint32 result_size__ptc_cs_profile_vtt2 = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PTC, key_value__ptc);
    
    result_size__ptc_routing_enable_profile_vtt1 += dnx_data_aod_sizes.AOD.sizes_get(unit, 184)->value;
    
    result_size__ptc_cs_profile_vtt2 += dnx_data_aod_sizes.AOD.sizes_get(unit, 178)->value;
    *offset = key_value__ptc%4*(result_size__ptc_routing_enable_profile_vtt1+result_size__ptc_cs_profile_vtt2);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ingress_vlan_domain_info_ingress_llvp_profile_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__vlan_domain;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_VLAN_DOMAIN, key_value__vlan_domain);
    *offset = key_value__vlan_domain/16;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ingress_vlan_domain_info_ingress_llvp_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__vlan_domain;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_VLAN_DOMAIN, key_value__vlan_domain);
    *offset = key_value__vlan_domain%16*4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ingress_vlan_membership_info_vlan_membership_enable_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__vlan_membership_if;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_VLAN_MEMBERSHIP_IF, key_value__vlan_membership_if);
    *offset = key_value__vlan_membership_if/16;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ingress_vlan_membership_info_vlan_membership_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__vlan_membership_if;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_VLAN_MEMBERSHIP_IF, key_value__vlan_membership_if);
    *offset = key_value__vlan_membership_if%16*4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
pp_attributes_per_tm_port_table_erpp_visibility_enable_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tm_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TM_PORT, key_value__tm_port);
    *offset = key_value__tm_port/256;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
pp_attributes_per_tm_port_table_erpp_visibility_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tm_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TM_PORT, key_value__tm_port);
    *offset = key_value__tm_port%256;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
