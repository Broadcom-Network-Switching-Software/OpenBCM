/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DBAL_STRING_APIS_H_INCLUDED
#define DBAL_STRING_APIS_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/dnx/dbal/dbal.h>

shr_error_e dnx_dbal_fields_string_form_hw_value_get(
    int unit,
    char *field_name,
    uint32 hw_value,
    char *str);

shr_error_e dnx_dbal_fields_struct_field_encode(
    int unit,
    char *field_name,
    char *field_in_struct_name,
    uint32 *field_in_struct_val,
    uint32 *full_field_val);

shr_error_e dnx_dbal_fields_struct_field_decode(
    int unit,
    char *field_name,
    char *field_in_struct_name,
    uint32 *field_in_struct_val,
    uint32 *full_field_val);

int dnx_dbal_fields_enum_value_get(
    int unit,
    char *field_name,
    char *field_val_name,
    int *hw_val);

int dnx_dbal_fields_enum_dbal_value_get(
    int unit,
    char *field_name,
    char *enum_val_name,
    int *dbal_val);

int dnx_dbal_fields_sub_field_info_get(
    int unit,
    char *parent_field_name,
    uint32 hw_val,
    char *sub_field_name,
    int *sub_field_val);

shr_error_e dnx_dbal_entry_handle_take(
    int unit,
    char *table_name,
    uint32 *entry_handle_id);

shr_error_e dnx_dbal_entry_handle_clear(
    int unit,
    char *table_name,
    uint32 entry_handle_id);

shr_error_e dnx_dbal_entry_handle_free(
    int unit,
    uint32 entry_handle_id);

shr_error_e dnx_dbal_entry_key_field_arr32_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint32 *field_val);

shr_error_e dnx_dbal_entry_key_field_symbol_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    char *symbol_name);

shr_error_e dnx_dbal_entry_key_field_arr32_masked_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint32 *field_val,
    uint32 *field_mask);

shr_error_e dnx_dbal_entry_key_field_arr8_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint8 *field_val);

shr_error_e dnx_dbal_entry_key_field_arr8_masked_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint8 *field_val,
    uint8 *field_mask);

shr_error_e dnx_dbal_entry_value_field_arr32_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    int inst_id,
    uint32 *field_val);

shr_error_e dnx_dbal_entry_value_field_symbol_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    int inst_id,
    char *symbol_name);

shr_error_e dnx_dbal_entry_handle_access_id_set(
    int unit,
    uint32 entry_handle_id,
    uint32 entry_access_id);

shr_error_e dnx_dbal_entry_handle_access_id_allocate_and_set(
    int unit,
    uint32 entry_handle_id,
    int core,
    uint32 priority,
    uint32 *entry_access_id);

shr_error_e dnx_dbal_entry_value_field_arr8_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    int inst_id,
    uint8 *field_val);

shr_error_e dnx_dbal_entry_commit(
    int unit,
    uint32 entry_handle_id,
    char *flag_name);

shr_error_e dnx_dbal_entry_get(
    int unit,
    uint32 entry_handle_id);

shr_error_e dnx_dbal_entry_nof_result_field_get(
    int unit,
    uint32 entry_handle_id,
    int *nof_result_field_names);

shr_error_e dnx_dbal_entry_result_field_name_get(
    int unit,
    uint32 entry_handle_id,
    int result_field_index,
    char *result_field_name);

shr_error_e dnx_dbal_entry_handle_key_field_arr32_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS]);

shr_error_e dnx_dbal_entry_handle_key_field_arr32_masked_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    uint32 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS]);

shr_error_e dnx_dbal_entry_handle_key_field_arr8_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint8 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS]);

shr_error_e dnx_dbal_entry_handle_key_field_arr8_masked_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint8 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    uint8 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS]);

shr_error_e dnx_dbal_entry_handle_value_field_arr32_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    int inst_id,
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS]);

shr_error_e dnx_dbal_entry_handle_value_field_symbol_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    int inst_id,
    char *symbol_name);

shr_error_e dnx_dbal_entry_handle_access_id_get(
    int unit,
    uint32 entry_handle_id,
    uint32 *entry_access_id);

shr_error_e dnx_dbal_entry_handle_value_field_arr8_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    int inst_id,
    uint8 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES]);

shr_error_e dnx_dbal_entry_clear(
    int unit,
    uint32 entry_handle_id);

shr_error_e dnx_dbal_table_clear(
    int unit,
    char *table_name);

shr_error_e dnx_dbal_table_is_tcam(
    int unit,
    char *table_name,
    uint8 *is_tcam);

shr_error_e dnx_dbal_table_is_shared_between_cores(
    int unit,
    char *table_name,
    uint8 *is_shared_between_cores);

shr_error_e dnx_dbal_table_access_method_get(
    int unit,
    char *table_name,
    char **access_method);

#endif
