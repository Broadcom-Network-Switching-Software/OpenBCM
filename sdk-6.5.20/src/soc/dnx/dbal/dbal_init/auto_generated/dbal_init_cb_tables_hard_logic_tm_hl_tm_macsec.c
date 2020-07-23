
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
macsec_ingress_sub_port_tcam_result_type_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__subport_priority;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SUBPORT_PRIORITY, key_value__subport_priority);
    *offset = key_value__subport_priority;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
macsec_ingress_sub_port_tcam_result_type_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__macsec_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_MACSEC_ID, key_value__macsec_id);
    *offset = key_value__macsec_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
macsec_ingress_sub_port_policy_untag_ctrl_port_en_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__policy_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_POLICY_ID, key_value__policy_id);
    *offset = key_value__policy_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
macsec_egress_secure_channel_sc_sci_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__sc_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SC_ID, key_value__sc_id);
    *offset = key_value__sc_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
macsec_egress_secure_association_sak_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__sa_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SA_ID, key_value__sa_id);
    *offset = key_value__sa_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
macsec_per_port_control_bypass_en_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__macsec_port_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_MACSEC_PORT_ID, key_value__macsec_port_id);
    *offset = key_value__macsec_port_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
macsec_ingress_svtag_cpu_flex_map_tcam_src_pbmp_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__hw_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_HW_ID, key_value__hw_id);
    *offset = key_value__hw_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
