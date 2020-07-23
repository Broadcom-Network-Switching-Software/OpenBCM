/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DBAL_DYNAMIC_H_INCLUDED
#define DBAL_DYNAMIC_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/dnx/dbal/dbal.h>

shr_error_e dbal_fields_field_create(
    int unit,
    dbal_field_types_defs_e field_type,
    char field_name[DBAL_MAX_STRING_LENGTH],
    dbal_fields_e * field_id);

shr_error_e dbal_tables_table_create(
    int unit,
    dbal_access_method_e access_method,
    dbal_table_type_e table_type,
    dbal_core_mode_e core_mode,
    int nof_key_fields,
    dbal_table_field_input_info_t * keys_info,
    int nof_result_fields,
    dbal_table_field_input_info_t * results_info,
    char *table_name,
    dbal_tables_e * table_id);

shr_error_e dbal_tables_result_type_add(
    int unit,
    dbal_tables_e table_id,
    int nof_result_fields,
    dbal_table_field_input_info_t * results_info,
    char *res_type_name,
    int result_type_size,
    int result_type_hw_value,
    int *result_type_index);

shr_error_e dbal_tables_table_destroy(
    int unit,
    dbal_tables_e table_id);

shr_error_e dbal_tables_table_restore(
    int unit,
    dbal_tables_e table_id);

shr_error_e dbal_tables_restore_tables(
    int unit);

shr_error_e dbal_tables_table_access_info_set(
    int unit,
    dbal_tables_e table_id,
    dbal_access_method_e access_method,
    void *access_info);

shr_error_e dbal_fields_field_destroy(
    int unit,
    dbal_fields_e field_id);

shr_error_e dbal_tables_field_predefine_value_update(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    int is_key,
    int result_type,
    int instance_id,
    dbal_field_predefine_value_type_e value_type,
    uint32 predef_value);

shr_error_e dbal_tables_app_id_set(
    int unit,
    dbal_tables_e table_id,
    uint32 app_db_id);

shr_error_e dbal_tables_dynamic_result_type_delete(
    int unit,
    dbal_tables_e table_id,
    int result_type_index);

shr_error_e dbal_tables_dynamic_result_type_destroy_all(
    int unit,
    dbal_tables_e table_id);

shr_error_e dbal_tables_result_type_restore_rollback(
    int unit,
    dbal_tables_e table_id);

#endif
