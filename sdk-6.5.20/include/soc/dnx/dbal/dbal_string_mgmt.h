
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DBAL_STRING_MGMT_H_INCLUDED
#define DBAL_STRING_MGMT_H_INCLUDED


#include <shared/utilex/utilex_rhlist.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/swstate/auto_generated/access/dbal_access.h>
#include <soc/dnx/dbal/dbal_external_defines.h>



shr_error_e dbal_field_string_to_id(
    int unit,
    char *str,
    dbal_fields_e * field_id);


shr_error_e dbal_flag_string_to_id(
    int unit,
    char *flag_name,
    dbal_entry_action_flags_e *action_flag);


shr_error_e dbal_hl_tcam_access_type_string_to_id(
    int unit,
    char *str,
    dbal_hl_tcam_access_type_e * access_type);


shr_error_e dbal_field_string_to_id_no_error(
    int unit,
    char *str,
    dbal_fields_e * field_id);


shr_error_e dbal_field_types_def_string_to_id(
    int unit,
    char *str,
    dbal_field_types_defs_e * field_types_def_id);


shr_error_e dbal_logical_table_string_to_id_no_error(
    int unit,
    char *str,
    dbal_tables_e * log_table_id);


shr_error_e dbal_logical_table_string_to_id(
    int unit,
    char *str,
    dbal_tables_e * log_table_id);


shr_error_e
dbal_result_type_string_to_id(int unit, dbal_tables_e table_id, char* res_type_str, uint32* res_type);

shr_error_e
dbal_result_type_string_to_id_no_err(int unit, dbal_tables_e table_id, char* res_type_str, uint32* res_type);

shr_error_e
dbal_enum_type_string_to_id(int unit, dbal_field_types_defs_e field_type, char *enum_value_string, uint32 *enum_value);

shr_error_e
dbal_enum_type_string_to_id_no_err(int unit, dbal_field_types_defs_e field_type, char *enum_value_string, uint32 *enum_value);

CONST char * dbal_enum_id_type_to_string(int unit,dbal_field_types_defs_e field_type,uint32 enum_value);


shr_error_e
dbal_enum_string_to_id(int unit, dbal_fields_e field_id, char *enum_value_string, uint32 *enum_value);


CONST char *
dbal_enum_id_to_string(int unit, dbal_fields_e field_id, uint32 enum_value);


shr_error_e dbal_hw_entity_string_to_id(
    int unit,
    char *str,
    dbal_hl_entity_type_e entity_type,
    int *hw_entity_id);

shr_error_e dbal_hw_field_form_reg_string_to_id(
    int unit,
    char *str,
    soc_reg_t reg,
    int *hw_entity_id);

shr_error_e dbal_hw_field_form_mem_string_to_id(
    int unit,
    char *str,
    soc_mem_t mem,
    int *hw_entity_id);


shr_error_e dbal_physical_table_string_to_id(
    int unit,
    char *str,
    dbal_physical_tables_e * phy_table_id);


shr_error_e dbal_label_string_to_id(
    int unit,
    char *str,
    dbal_labels_e * label_id);


shr_error_e dbal_core_mode_string_to_id(
    int unit,
    char *str,
    dbal_core_mode_e * core_mode_id);


shr_error_e dbal_condition_string_to_id(
    int unit,
    char *str,
    dbal_condition_types_e * condition_id);


shr_error_e dbal_field_print_type_string_to_id(
    int unit,
    char *str,
    dbal_field_print_type_e * field_print_type_id);


shr_error_e dbal_field_encode_type_string_to_id(
    int unit,
    char *str,
    dbal_value_field_encode_types_e * encode_type_id);


shr_error_e dbal_logical_table_type_string_to_id(
    int unit,
    char *str,
    dbal_table_type_e * table_type_id);


shr_error_e dbal_access_method_string_to_id(
    int unit,
    char *str,
    dbal_access_method_e * access_method_id);


shr_error_e dbal_maturity_level_string_to_id(
    int unit,
    char *str,
    dbal_maturity_level_e * maturity_level_id);


shr_error_e dbal_stage_string_to_id(
    int unit,
    char *str,
    dbal_stage_e * dbal_stage);


shr_error_e dbal_table_status_string_to_id(
    int unit,
    char *str,
    dbal_table_status_e * table_status_id);

CONST char *dbal_field_to_string(
    int unit,
    dbal_fields_e field_id);


char *dbal_hl_tcam_access_type_to_string(
    int unit,
    dbal_hl_tcam_access_type_e hl_tcam_access_type);


char *dbal_field_types_def_to_string(
    int unit,
    dbal_field_types_defs_e field_types_def_id);



char *dbal_logical_table_to_string(
    int unit,
    dbal_tables_e table_id);



CONST char *dbal_result_type_to_string(
    int unit,
    dbal_tables_e table_id,
    int result_type);


char *dbal_logger_type_to_string(
    int unit,
    dnx_dbal_logger_type_e dbal_logger_type);


char *dbal_physical_table_to_string(
    int unit,
    dbal_physical_tables_e phy_table_id);


char *dbal_label_to_string(
    int unit,
    dbal_labels_e label_id);


char *
dbal_mdb_hitbit_to_string(
    int unit,
    dbal_physical_tables_e physical_db_id,
    uint8 hitbit);


char *
dbal_entry_attr_to_string(
    int unit,
    uint32 attr_type);


char *dbal_hw_entity_to_string(
    int unit,
    int hw_entity_id);


char *dbal_core_mode_to_string(
    int unit,
    dbal_core_mode_e core_mode_id);


char *dbal_condition_to_string(
    int unit,
    dbal_condition_types_e condition_id);


char *dbal_field_print_type_to_string(
    int unit,
    dbal_field_print_type_e field_print_type_id);


char *dbal_field_encode_type_to_string(
    int unit,
    dbal_value_field_encode_types_e encode_type_id);


char *dbal_action_flags_to_string(
    int unit,
    dbal_entry_action_flags_e action_flag_id);


char *dbal_access_method_to_string(
    int unit,
    dbal_access_method_e access_method_id);


char *dbal_table_type_to_string(
    int unit,
    dbal_table_type_e table_type_id);


char *dbal_table_status_to_string(
    int unit,
    dbal_table_status_e table_status_id);


char *dbal_maturity_level_to_string(
    int unit,
    dbal_maturity_level_e maturity_level_id);


char *dbal_stage_to_string(
    int unit,
    dbal_stage_e dbal_stage);


char *
dbal_device_state_to_string(
        int unit,
        dbal_device_state_e dbal_device_state);

 
char * 
dbal_field_permission_to_string(
    int unit, 
    dbal_field_permission_e permission_id);


shr_error_e dbal_full_table_to_string(
    int unit,
    dbal_tables_e table_id,
    dbal_table_string_t * table_string);


shr_error_e dbal_decode(
    int unit,
    int core,
    char *desc,
    uint32 *data,
    uint32 data_size_bits,
    rhlist_t ** parsed_info,
    char from_n[RHNAME_MAX_SIZE],
    char to_n[RHNAME_MAX_SIZE]);


shr_error_e
dbal_hw_entity_group_string_to_id(
    int unit,
    char *str,
    dbal_hw_entity_group_e * group_id);


char *
dbal_hw_entity_group_to_string(
    int unit,
    dbal_hw_entity_group_e hw_entity_group_id);

#endif
