
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
protocol_trap_handling_profile_l_2_cp_trap_handling_profile_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__in_pp_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IN_PP_PORT, key_value__in_pp_port);
    *offset = key_value__in_pp_port;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
protocol_l2cp_trap_action_profile_ingress_trap_id_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__protocol_trap_args;
    uint32 key_value__protocol_trap_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PROTOCOL_TRAP_ARGS, key_value__protocol_trap_args);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PROTOCOL_TRAP_PROFILE, key_value__protocol_trap_profile);
    *offset = 4*key_value__protocol_trap_args+key_value__protocol_trap_profile;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
protocol_icmp_trap_action_profile_ingress_trap_id_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__protocol_type;
    uint32 key_value__protocol_trap_args;
    uint32 key_value__protocol_trap_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PROTOCOL_TYPE, key_value__protocol_type);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PROTOCOL_TRAP_ARGS, key_value__protocol_trap_args);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PROTOCOL_TRAP_PROFILE, key_value__protocol_trap_profile);
    *offset = key_value__protocol_type+4*key_value__protocol_trap_args+key_value__protocol_trap_profile;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
protocol_8021x_trap_action_profile_ingress_trap_id_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__protocol_type;
    uint32 key_value__protocol_trap_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PROTOCOL_TYPE, key_value__protocol_type);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PROTOCOL_TRAP_PROFILE, key_value__protocol_trap_profile);
    *offset = key_value__protocol_type+key_value__protocol_trap_profile;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
protocol_trap_my_ndp_ip_1_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    *offset = full_key_value*32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
