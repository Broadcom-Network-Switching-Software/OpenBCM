
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
snif_command_table_destination_ow_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__snif_type;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SNIF_TYPE, key_value__snif_type);
    *offset = key_value__snif_type;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
snif_command_table_destination_ow_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__snif_command_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SNIF_COMMAND_ID, key_value__snif_command_id);
    *offset = key_value__snif_command_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
snif_ingress_mirror_rule_table_mirror_profile_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__vid_profile;
    uint32 key_value__pp_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_VID_PROFILE, key_value__vid_profile);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PP_PORT, key_value__pp_port);
    *offset = key_value__vid_profile*dnx_data_port.general.nof_pp_ports_get(unit)+key_value__pp_port;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
snif_ingress_mirror_rule_table_mirror_profile_entryoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__vid_profile;
    uint32 key_value__pp_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_VID_PROFILE, key_value__vid_profile);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PP_PORT, key_value__pp_port);
    *offset = key_value__vid_profile*1024+key_value__pp_port;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
snif_egress_command_table_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__snif_command_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SNIF_COMMAND_ID, key_value__snif_command_id);
    *offset = key_value__snif_command_id*40+0;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
snif_egress_command_table_snif_egress_probability_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__snif_command_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SNIF_COMMAND_ID, key_value__snif_command_id);
    *offset = key_value__snif_command_id*40+1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
snif_egress_command_table_snif_rcy_priority_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__snif_command_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SNIF_COMMAND_ID, key_value__snif_command_id);
    *offset = key_value__snif_command_id*40+33;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
snif_egress_command_table_snif_append_orig_headers_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__snif_command_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SNIF_COMMAND_ID, key_value__snif_command_id);
    *offset = key_value__snif_command_id*40+35;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
snif_egress_command_table_snif_crop_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__snif_command_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SNIF_COMMAND_ID, key_value__snif_command_id);
    *offset = key_value__snif_command_id*40+34;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
snif_egress_command_table_snif_rcy_cmd_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__snif_command_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SNIF_COMMAND_ID, key_value__snif_command_id);
    *offset = key_value__snif_command_id*40+36;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
snif_egress_command_table_enable_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__snif_command_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SNIF_COMMAND_ID, key_value__snif_command_id);
    *offset = key_value__snif_command_id*41+0;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
snif_egress_command_table_snif_egress_probability_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__snif_command_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SNIF_COMMAND_ID, key_value__snif_command_id);
    *offset = key_value__snif_command_id*41+1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
snif_egress_command_table_snif_rcy_priority_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__snif_command_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SNIF_COMMAND_ID, key_value__snif_command_id);
    *offset = key_value__snif_command_id*41+33;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
snif_egress_command_table_snif_append_orig_headers_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__snif_command_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SNIF_COMMAND_ID, key_value__snif_command_id);
    *offset = key_value__snif_command_id*41+36;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
snif_egress_command_table_snif_crop_enable_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__snif_command_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SNIF_COMMAND_ID, key_value__snif_command_id);
    *offset = key_value__snif_command_id*41+35;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
snif_egress_command_table_snif_rcy_cmd_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__snif_command_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SNIF_COMMAND_ID, key_value__snif_command_id);
    *offset = key_value__snif_command_id*41+37;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
snif_egress_mirror_vid_profile_table_vlan_id_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__vid_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_VID_PROFILE, key_value__vid_profile);
    *offset = key_value__vid_profile*12;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
snif_egress_mirror_rule_table_mirror_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__vid_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_VID_PROFILE, key_value__vid_profile);
    *offset = key_value__vid_profile*3;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
snif_system_header_table_dsp_ext_destination_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    *offset = full_key_value+32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
snif_mirr_profile_to_code_map_mirror_code_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__mirror_profile_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_MIRROR_PROFILE_ID, key_value__mirror_profile_id);
    *offset = key_value__mirror_profile_id*9;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
snif_code_to_profile_map_snif_command_id_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__snif_code;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SNIF_CODE, key_value__snif_code);
    *offset = key_value__snif_code;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
snif_rcy_cmd_map_mirror_code_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__recycle_command_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_RECYCLE_COMMAND_ID, key_value__recycle_command_id);
    *offset = key_value__recycle_command_id*9;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
snif_rcy_cmd_map_rcy_snoop_code_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__recycle_command_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_RECYCLE_COMMAND_ID, key_value__recycle_command_id);
    *offset = key_value__recycle_command_id*12;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
snif_rcy_cmd_map_rcy_snoop_strength_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__recycle_command_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_RECYCLE_COMMAND_ID, key_value__recycle_command_id);
    *offset = key_value__recycle_command_id*12+9;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
