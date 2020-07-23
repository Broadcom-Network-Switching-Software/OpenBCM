/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

#include "dbal_internal.h"
#include <soc/dnx/dbal/dbal.h>
#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include <soc/sand/sand_signals.h>
#include <bcm_int/dnx/field/field_entry.h>

shr_error_e
dnx_dbal_fields_string_form_hw_value_get(
    int unit,
    char *field_name,
    uint32 hw_value,
    char *str)
{
    dbal_fields_e field_id;
    dbal_field_types_basic_info_t *field_info;
    int ii;
    uint32 local_hw_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    sal_strcpy(str, EMPTY);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_id, &field_info));

    
    if (field_info->nof_defines_values > 0)
    {
        for (ii = 0; ii < field_info->nof_defines_values; ii++)
        {
            if (field_info->defines_info[ii].value == hw_value)
            {
                
                sal_snprintf(str, RHSTRING_MAX_SIZE, "%s_%s", field_info->name, field_info->defines_info[ii].name);
                SHR_EXIT();
            }
        }
    }

    
    if (field_info->nof_enum_values > 0)
    {
        for (ii = 0; ii < field_info->nof_enum_values; ii++)
        {
            if ((field_info->enum_val_info[ii].value == hw_value) && (!field_info->enum_val_info[ii].is_invalid))
            {
                
                sal_snprintf(str, RHSTRING_MAX_SIZE, "%s_%s", field_info->name, field_info->enum_val_info[ii].name);
                SHR_EXIT();
            }
        }
        SHR_ERR_EXIT(_SHR_E_PARAM, "For enum field:%s HW value:%u does not match existing values\n",
                     field_name, hw_value);
    }

    
    if (field_info->nof_child_fields > 0)
    {
        dbal_fields_e sub_field_id;
        uint32 sub_field_val;
        sub_field_val = 0;
        local_hw_value[0] = hw_value;
        SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode(unit, field_id, local_hw_value, &sub_field_id, &sub_field_val));

        if (sub_field_id != DBAL_FIELD_EMPTY)
        {
            sal_sprintf(str, "%s %x", dbal_field_to_string(unit, sub_field_id), sub_field_val);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_fields_struct_field_encode(
    int unit,
    char *field_name,
    char *field_in_struct_name,
    uint32 *field_in_struct_val,
    uint32 *full_field_val)
{
    dbal_fields_e field_id;
    dbal_fields_e field_in_struct_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));
    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_in_struct_name, &field_in_struct_id));

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, field_id, field_in_struct_id, field_in_struct_val, full_field_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_fields_struct_field_decode(
    int unit,
    char *field_name,
    char *field_in_struct_name,
    uint32 *field_in_struct_val,
    uint32 *full_field_val)
{
    dbal_fields_e field_id;
    dbal_fields_e field_in_struct_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));
    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_in_struct_name, &field_in_struct_id));

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, field_id, field_in_struct_id, field_in_struct_val, full_field_val));

exit:
    SHR_FUNC_EXIT;
}


int
dnx_dbal_fields_enum_value_get(
    int unit,
    char *field_name,
    char *enum_val_name,
    int *hw_val)
{
    dbal_fields_e field_id;
    int ii;
    dbal_field_types_basic_info_t *field_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));
    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_id, &field_info));

    for (ii = 0; ii < field_info->nof_enum_values; ii++)
    {
        if ((sal_strcasecmp(enum_val_name, field_info->enum_val_info[ii].name) == 0)
            && (!field_info->enum_val_info[ii].is_invalid))
        {
            (*hw_val) = (int) field_info->enum_val_info[ii].value;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_PARAM, "DBAL - field %s val name %s wasn't found \n", field_name, enum_val_name);

exit:
    SHR_FUNC_EXIT;
}

int
dnx_dbal_fields_enum_dbal_value_get(
    int unit,
    char *field_name,
    char *enum_val_name,
    int *dbal_val)
{
    dbal_fields_e field_id;
    int ii;
    dbal_field_types_basic_info_t *field_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));
    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_id, &field_info));

    for (ii = 0; ii < field_info->nof_enum_values; ii++)
    {
        if ((sal_strcasecmp(enum_val_name, field_info->enum_val_info[ii].name) == 0)
            && (!field_info->enum_val_info[ii].is_invalid))
        {
            (*dbal_val) = ii;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_PARAM, "DBAL - field %s val name %s wasn't found \n", field_name, enum_val_name);

exit:
    SHR_FUNC_EXIT;
}

int
dnx_dbal_fields_sub_field_info_get(
    int unit,
    char *parent_field_name,
    uint32 hw_val,
    char *sub_field_name,
    int *sub_field_val)
{
    dbal_fields_e field_id;
    dbal_field_types_basic_info_t *field_info;
    uint32 hw_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    (*sub_field_val) = 0;

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, parent_field_name, &field_id));
    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_id, &field_info));

    
    if (field_info->nof_child_fields > 0)
    {
        dbal_fields_e sub_field_id;
        uint32 sub_field_val_as_uint[1];
        hw_value[0] = hw_val;
        SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode
                        (unit, field_id, hw_value, &sub_field_id, sub_field_val_as_uint));

        (*sub_field_val) = sub_field_val_as_uint[0];

        if (sub_field_id != DBAL_FIELD_EMPTY)
        {
            sal_sprintf(sub_field_name, "%s%c", dbal_field_to_string(unit, sub_field_id), '\0');
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_PARAM, "ilegal input no subfields found or/ non of them match to he value\n");

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_handle_take(
    int unit,
    char *table_name,
    uint32 *entry_handle_id)
{
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, table_name, &table_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_take_macro(unit, table_id, entry_handle_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_handle_clear(
    int unit,
    char *table_name,
    uint32 entry_handle_id)
{
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, table_name, &table_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_clear_macro(unit, table_id, entry_handle_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_handle_free(
    int unit,
    uint32 entry_handle_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_release_internal(unit, entry_handle_id));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_key_field_arr32_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint32 *field_val)
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    dbal_entry_key_field_arr32_set(unit, entry_handle_id, field_id, field_val);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_key_field_arr32_masked_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint32 *field_val,
    uint32 *field_mask)
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    dbal_entry_key_field_arr32_masked_set(unit, entry_handle_id, field_id, field_val, field_mask);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_key_field_symbol_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    char *symbol_name)
{
    dbal_fields_e field_id;
    uint32 symbol_value[1];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    SHR_IF_ERR_EXIT(dbal_enum_string_to_id(unit, field_id, symbol_name, &symbol_value[0]));

    dbal_entry_key_field_arr32_set(unit, entry_handle_id, field_id, &symbol_value[0]);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_key_field_arr8_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint8 *field_val)
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    dbal_entry_key_field_arr8_set(unit, entry_handle_id, field_id, field_val);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_key_field_arr8_masked_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint8 *field_val,
    uint8 *field_mask)
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, field_id, field_val, field_mask);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_value_field_arr32_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    int inst_id,
    uint32 *field_val)
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, field_id, inst_id, field_val);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_value_field_symbol_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    int inst_id,
    char *symbol_name)
{
    dbal_fields_e field_id;
    uint32 symbol_value[1];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    if (field_id == DBAL_FIELD_RESULT_TYPE)
    {
        dbal_entry_handle_t *entry_handle;

        SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
        SHR_IF_ERR_EXIT(dbal_result_type_string_to_id(unit, entry_handle->table_id, symbol_name, &symbol_value[0]));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_enum_string_to_id(unit, field_id, symbol_name, &symbol_value[0]));
    }

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, field_id, inst_id, &symbol_value[0]);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_value_field_arr8_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    int inst_id,
    uint8 *field_val)
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    dbal_entry_value_field_arr8_set(unit, entry_handle_id, field_id, inst_id, field_val);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_commit(
    int unit,
    uint32 entry_handle_id,
    char *flag_name)
{
    dbal_entry_action_flags_e action_flag;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_flag_string_to_id(unit, flag_name, &action_flag));

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, action_flag));
exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_dbal_entry_get(
    int unit,
    uint32 entry_handle_id)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_dbal_entry_nof_result_field_get(
    int unit,
    uint32 entry_handle_id,
    int *nof_result_field_names)
{
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    *nof_result_field_names = entry_handle->table->multi_res_info[entry_handle->cur_res_type].nof_result_fields;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_result_field_name_get(
    int unit,
    uint32 entry_handle_id,
    int result_field_index,
    char *result_field_name)
{
    dbal_entry_handle_t *entry_handle;
    dbal_fields_e field_id;
    const char *field_name;
    int nof_result_fields;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    nof_result_fields = entry_handle->table->multi_res_info[entry_handle->cur_res_type].nof_result_fields;

    if (result_field_index >= nof_result_fields)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "result field index must be in range 0 - %d \n", nof_result_fields);
    }

    field_id =
        entry_handle->table->multi_res_info[entry_handle->cur_res_type].results_info[result_field_index].field_id;
    field_name = dbal_field_to_string(unit, field_id);
    sal_strncpy_s(result_field_name, field_name, sal_strlen(field_name));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_handle_access_id_set(
    int unit,
    uint32 entry_handle_id,
    uint32 entry_access_id)
{
    dbal_entry_handle_t *entry_handle;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_handle_access_id_allocate_and_set(
    int unit,
    uint32 entry_handle_id,
    int core,
    uint32 priority,
    uint32 *entry_access_id)
{
    dbal_entry_handle_t *entry_handle;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    SHR_IF_ERR_EXIT(dnx_field_entry_access_id_create(unit, core, entry_handle->table_id, priority, entry_access_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, *entry_access_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_handle_key_field_arr32_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS])
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, field_id, field_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_handle_key_field_arr32_masked_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    uint32 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS])
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                    (unit, entry_handle_id, field_id, field_val, field_mask));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_handle_key_field_arr8_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint8 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS])
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get(unit, entry_handle_id, field_id, field_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_handle_key_field_arr8_masked_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint8 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    uint8 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS])
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_masked_get
                    (unit, entry_handle_id, field_id, field_val, field_mask));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_handle_value_field_arr32_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    int inst_id,
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS])
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, inst_id, field_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_handle_value_field_symbol_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    int inst_id,
    char *symbol_name)
{
    dbal_fields_e field_id;
    uint32 field_val[1];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, inst_id, &field_val[0]));

    if (field_id == DBAL_FIELD_RESULT_TYPE)
    {
        dbal_entry_handle_t *entry_handle;

        SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
        sal_sprintf(symbol_name, "%s", dbal_result_type_to_string(unit, entry_handle->table_id, field_val[0]));
    }
    else
    {
        sal_sprintf(symbol_name, "%s", dbal_enum_id_to_string(unit, field_id, field_val[0]));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_handle_access_id_get(
    int unit,
    uint32 entry_handle_id,
    uint32 *entry_access_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_get(unit, entry_handle_id, entry_access_id));

exit:
    SHR_FUNC_EXIT;
}
shr_error_e
dnx_dbal_entry_handle_value_field_arr8_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    int inst_id,
    uint8 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES])
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id, field_id, inst_id, field_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_clear(
    int unit,
    uint32 entry_handle_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_dbal_table_clear(
    int unit,
    char *table_name)
{
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, table_name, &table_id));

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_dbal_decode_field_signal_copy(
    int unit,
    signal_output_t * sig,
    rhlist_t * list)
{
    rhhandle_t rhhandle;
    signal_output_t *new_sig;
    char name[RHNAME_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    sal_snprintf(name, RHNAME_MAX_SIZE, "%s", RHNAME(sig));
    
    SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(list, name, RHID_TO_BE_GENERATED, &rhhandle));
    new_sig = rhhandle;
    
    new_sig->size = sig->size;
    SHR_BITCOPY_RANGE(new_sig->value, 0, sig->value, 0, sig->size);
    sal_strncpy(new_sig->print_value, sig->print_value, DSIG_MAX_SIZE_STR);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_parent_field_string_convert_to_signal_string(
    int unit,
    uint32 decoded_val,
    int index,
    dbal_value_field_encode_types_e encode_type,
    char **decoded_values_list,
    dbal_fields_e sub_field_id,
    dbal_field_types_basic_info_t * field_info,
    uint32 data_size_bits,
    signal_output_t * sig_format)
{
    int ii;
    dbal_field_types_defs_e field_type;
    uint32 size = 0;
    uint32 field_size = 0;
    dbal_fields_e struct_member_field_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, sub_field_id, &field_size));
    
    if (index == 0)
    {
        sal_strncpy(sig_format->entry.name, "type", RHNAME_MAX_SIZE);
        
        for (ii = 0; ii < field_info->nof_child_fields; ii++)
        {
            if (field_info->sub_field_info[ii].sub_field_id == sub_field_id)
            {
                
                sig_format->value[0] = field_info->sub_field_info[ii].encode_info.input_param;
                
                sig_format->size = data_size_bits - field_size;
                break;
            }
        }

    }
    if (index == 1)
    {
        sal_strncpy(sig_format->entry.name, "value", RHNAME_MAX_SIZE);
        
        sig_format->value[0] = decoded_val;
        
        sig_format->size = field_size;
    }

    
    if ((index > 1) && (encode_type == DBAL_VALUE_FIELD_ENCODE_STRUCTURE))
    {
        if ((index & 1) == 0)
        {
            sal_strncpy(sig_format->entry.name, decoded_values_list[index], RHNAME_MAX_SIZE - 1);
        }
        
        SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, sub_field_id, &field_type));
        
        SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, decoded_values_list[index], &struct_member_field_id));
        
        SHR_IF_ERR_EXIT(dbal_field_struct_field_size_get(unit, field_type, struct_member_field_id, &size));
        
        if (!sal_strcmp(decoded_values_list[index + 1], "True"))
        {
            sig_format->value[0] = 1;
        }
        else if (!sal_strcmp(decoded_values_list[index + 1], "False"))
        {
            sig_format->value[0] = 0;
        }
        else
        {
            
            utilex_str_stoul(decoded_values_list[index + 1], &decoded_val);
            sig_format->value[0] = decoded_val;
        }
        
        sig_format->size = size;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_string_convert_to_signal_string(
    int unit,
    dbal_fields_e field_id,
    uint32 *data,
    char *decoded_data_string,
    dbal_field_types_basic_info_t * field_info,
    dbal_value_field_encode_types_e encode_type,
    uint32 data_size_bits,
    rhlist_t * decoded_values)
{
    dbal_fields_e sub_field_id;
    uint32 sub_field_val = 0;
    uint32 nof_values = 0;
    uint32 decoded_val = 0;
    int index = 0;
    rhhandle_t sig_handle;
    int ii = 0;
    dbal_field_types_basic_info_t *struct_member_field_info = NULL;
    dbal_fields_e struct_member_field_id;
    signal_output_t *sig_format = NULL;
    char **decoded_values_list = NULL;
    dbal_field_types_basic_info_t *subfield_info = NULL;
    SHR_FUNC_INIT_VARS(unit);

    if (field_info->nof_child_fields > 0)
    {
        
        SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode(unit, field_id, data, &sub_field_id, &sub_field_val));
        SHR_IF_ERR_EXIT(dbal_fields_encode_type_get(unit, sub_field_id, &encode_type));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_fields_encode_type_get(unit, field_id, &encode_type));
    }

    
    if (encode_type == DBAL_VALUE_FIELD_ENCODE_STRUCTURE)
    {
        
        utilex_str_replace(decoded_data_string, ';', ' ');
        
        utilex_str_replace(decoded_data_string, '(', ' ');
        
        utilex_str_replace(decoded_data_string, ')', ' ');
        
        utilex_str_shrink(decoded_data_string);
        
        if ((decoded_values_list = utilex_str_split(decoded_data_string, " ", 20, &nof_values)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to parse decoded_data_string: %s\n", decoded_data_string);
        }
        if (field_info->nof_child_fields > 0)
        {
            
            SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, decoded_values_list[0], &sub_field_id));
        }
    }
    else
    {
        
        utilex_str_replace(decoded_data_string, '(', ' ');
        
        utilex_str_replace(decoded_data_string, ')', ' ');
        
        utilex_str_shrink(decoded_data_string);
        
        if ((decoded_values_list = utilex_str_split(decoded_data_string, " ", 3, &nof_values)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to parse decoded_data_string: %s\n", decoded_data_string);
        }
    }

    if ((field_info->nof_enum_values > 0) || (field_info->nof_child_fields > 0))
    {
        
        utilex_str_stoul(decoded_values_list[1], &decoded_val);
    }

    if (NULL == sig_format)
    {
        if (field_info->nof_defines_values > 0)
        {
            
            nof_values = nof_values + 1;
        }
        
        for (index = 0; index < nof_values - 1; index++)
        {
            SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(decoded_values, "DBAL", RHID_TO_BE_GENERATED, &sig_handle));
            sig_format = sig_handle;
            
            sig_format->size = data_size_bits;

            if (field_info->nof_defines_values > 0)
            {
                
                sal_strncpy(sig_format->entry.name, "Define_Name", RHNAME_MAX_SIZE);

                for (ii = 0; ii < field_info->nof_defines_values; ii++)
                {
                    char name[DBAL_MAX_LONG_STRING_LENGTH];
                    
                    sal_snprintf(name, RHSTRING_MAX_SIZE, "%s_%s", field_info->name, field_info->defines_info[ii].name);

                    if (name == decoded_values_list[0])
                    {
                        
                        sig_format->value[0] = field_info->defines_info[ii].value;
                        break;
                    }
                }
            }
            else if (field_info->nof_enum_values > 0)
            {
                
                sal_strncpy(sig_format->entry.name, "EnumVal", RHNAME_MAX_SIZE);
                for (ii = 0; ii < field_info->nof_enum_values; ii++)
                {
                    char name[2 * DBAL_MAX_STRING_LENGTH];

                    
                    sal_snprintf(name, 2 * DBAL_MAX_STRING_LENGTH, "%s_%s", field_info->name,
                                 field_info->enum_val_info[ii].name);
                    if (name == decoded_values_list[0])
                    {
                        
                        sig_format->value[0] = field_info->enum_val_info[ii].value;
                        index = index + 1;
                        break;
                    }
                }
            }
            else if (field_info->nof_child_fields > 0)
            {
                SHR_IF_ERR_EXIT(dbal_parent_field_string_convert_to_signal_string
                                (unit, decoded_val, index, encode_type, decoded_values_list,
                                 sub_field_id, field_info, data_size_bits, sig_format));
                if ((index > 1) && (encode_type == DBAL_VALUE_FIELD_ENCODE_STRUCTURE))
                {
                    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, decoded_values_list[index], &struct_member_field_id));
                    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, struct_member_field_id, &subfield_info));
                    index++;
                    if (subfield_info->nof_enum_values > 0)
                    {
                        sal_snprintf(sig_format->print_value, DSIG_MAX_SIZE_STR, "%s", decoded_values_list[index]);
                        
                        index++;
                    }
                    else if (!sal_strcmp(decoded_values_list[index], "True")
                             || !sal_strcmp(decoded_values_list[index], "False"))
                    {
                        sal_snprintf(sig_format->print_value, DSIG_MAX_SIZE_STR, "%s", decoded_values_list[index]);
                    }
                    else
                    {
                        
                        utilex_str_stoul(decoded_values_list[index], &decoded_val);
                        
                        sal_snprintf(sig_format->print_value, DSIG_MAX_SIZE_STR, "%x", decoded_val);
                    }
                }
            }
            else
            {
                
                index++;
                sal_strncpy(sig_format->entry.name, decoded_values_list[index], RHNAME_MAX_SIZE);
                
                SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, decoded_values_list[index], &struct_member_field_id));
                
                SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get
                                (unit, struct_member_field_id, &struct_member_field_info));

                if (struct_member_field_info->nof_enum_values > 0)
                {
                    
                    index++;
                }
                
                if (!sal_strcmp(decoded_values_list[index + 1], "True"))
                {
                    sig_format->value[0] = 1;
                }
                else if (!sal_strcmp(decoded_values_list[index + 1], "False"))
                {
                    sig_format->value[0] = 0;
                }
                else
                {
                    
                    utilex_str_stoul(decoded_values_list[index + 1], &decoded_val);
                    sig_format->value[0] = decoded_val;
                }
                
                sig_format->size = struct_member_field_info->max_size;
                if ((field_info->nof_child_fields == 0) && encode_type == DBAL_VALUE_FIELD_ENCODE_STRUCTURE)
                {
                    if (struct_member_field_info->nof_enum_values > 0)
                    {
                        
                        sal_snprintf(sig_format->print_value, DSIG_MAX_SIZE_STR, "%s", decoded_values_list[index]);

                    }
                    else
                    {
                        
                        sal_snprintf(sig_format->print_value, DSIG_MAX_SIZE_STR, "%s", decoded_values_list[index + 1]);
                    }
                }
            }

            if ((field_info->nof_child_fields == 0) && (encode_type == DBAL_VALUE_FIELD_ENCODE_STRUCTURE))
            {
                continue;
            }
            else if ((field_info->nof_child_fields != 0) && (index == 1))
            {
                
                sal_snprintf(sig_format->print_value, DSIG_MAX_SIZE_STR, "%x", decoded_val);
            }
            else if ((field_info->nof_child_fields != 0) && (index > 1))
            {
                continue;
            }
            else
            {
                
                sal_snprintf(sig_format->print_value, DSIG_MAX_SIZE_STR, "%s", decoded_values_list[index]);
            }
        }
    }

exit:
    if (decoded_values_list != NULL)
    {
        utilex_str_split_free(decoded_values_list, nof_values);
    }
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_table_parse_to_signal_string(
    int unit,
    int core,
    char *dbal_table_name,
    dbal_tables_e table_id,
    rhlist_t * decoded_values)
{

    signal_output_t *sig_format = NULL;
    rhhandle_t rhhandle;
    dbal_printable_entry_t *entry_info = NULL;
    int payload_size = 0;
    int result_field_size = 0;
    uint32 result_field = 0;
    uint32 entry_handle_id;
    dbal_entry_handle_t *entry_handle;
    dbal_entry_handle_t *get_entry_handle = NULL;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    
    if ((entry_handle->table->nof_key_fields != 1)
        && (entry_handle->table->keys_info[0].field_id != DBAL_FIELD_CORE_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The dbal table: %s has not only one key field! It can be used only dbal tables with single key - CORE_ID!\n",
                     dbal_table_name);
    }

    
    SHR_ALLOC_SET_ZERO(get_entry_handle, sizeof(dbal_entry_handle_t), "Handle Alloc", "%s%s%s\r\n", EMPTY, EMPTY,
                       EMPTY);
    entry_handle->core_id = core;
    
    SHR_IF_ERR_EXIT(dbal_actions_access_entry_get(unit, entry_handle, get_entry_handle, 0));

    entry_info = sal_alloc(sizeof(dbal_printable_entry_t), "dbal_printable_entry");
    SHR_NULL_CHECK(entry_info, _SHR_E_MEMORY, "entry_info");
    sal_memset(entry_info, 0, sizeof(dbal_printable_entry_t));
    
    SHR_IF_ERR_EXIT(dbal_tables_payload_size_get(unit, table_id, 0, &payload_size));
    
    SHR_IF_ERR_EXIT_NO_MSG(dbal_table_printable_entry_get
                           (unit, table_id, NULL, core, payload_size, get_entry_handle->phy_entry.payload, entry_info));

    
    for (result_field = 0; result_field < entry_info->nof_res_fields; ++result_field)
    {
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail
                        (decoded_values, entry_info->res_fields_info[result_field].field_name, RHID_TO_BE_GENERATED,
                         &rhhandle));
        sig_format = rhhandle;
        
        sal_snprintf(sig_format->print_value, DSIG_MAX_SIZE_STR - 1, "%s",
                     entry_info->res_fields_info[result_field].field_print_value);
        
        sig_format->value[0] = *entry_info->res_fields_info[result_field].field_value;
        
        SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                        (unit, table_id, entry_info->res_fields_info[result_field].field_id, 0, 0, 0,
                         &result_field_size));
        
        sig_format->size = result_field_size;
    }

exit:
    SHR_FREE(get_entry_handle);
    if (entry_info)
    {
        sal_free(entry_info);
    }
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_decode(
    int unit,
    int core,
    char *desc,
    uint32 *data,
    uint32 data_size_bits,
    rhlist_t ** parsed_info,
    char from_n[RHNAME_MAX_SIZE],
    char to_n[RHNAME_MAX_SIZE])
{
    dbal_fields_e field_id;
    signal_output_t *sig = NULL;
    char decoded_data_string[DBAL_MAX_PRINTABLE_BUFFER_SIZE] = { 0 };
    rhlist_t *decoded_values = NULL;
    dbal_field_types_basic_info_t *field_info;
    dbal_value_field_encode_types_e encode_type = DBAL_VALUE_FIELD_ENCODE_NONE;
    uint8 is_dbal_field = 0;
    dbal_tables_e table_id = DBAL_TABLE_EMPTY;
    uint32 *data_ptr;
    uint32 data_u32;
    SHR_FUNC_INIT_VARS(unit);
    
    SHR_NULL_CHECK(desc, _SHR_E_PARAM, "desc");
    SHR_NULL_CHECK(parsed_info, _SHR_E_PARAM, "parsed_info");

    
    if (dbal_field_string_to_id_no_error(unit, desc, &field_id) == _SHR_E_NONE)
    {
        is_dbal_field = 1;
        
        SHR_NULL_CHECK(data, _SHR_E_PARAM, "data");
    }
    else
    {
        if (dbal_logical_table_string_to_id(unit, desc, &table_id) == _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "desc: %s is neither dbal table, nor dbal field! It should be used a valid table/field name! \n",
                         desc);
        }
    }

    if (is_dbal_field)
    {
        SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, desc, &field_id));
        
        SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_id, &field_info));
        SHR_IF_ERR_EXIT(dbal_fields_encode_type_get(unit, field_id, &encode_type));

        if ((field_info->nof_defines_values == 0) && (field_info->nof_enum_values == 0)
            && (field_info->nof_child_fields == 0) && (encode_type != DBAL_VALUE_FIELD_ENCODE_STRUCTURE))
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "Only enums, defines, structures and parent fields with child fields can be decoded! %s is not supported!",
                         desc);
        }
        data_ptr = data;
        if (encode_type == DBAL_VALUE_FIELD_ENCODE_ENUM)
        {
            int enum_idx;
            for (enum_idx = 0; enum_idx < field_info->nof_enum_values; enum_idx++)
            {
                if (!field_info->enum_val_info[enum_idx].is_invalid &&
                    field_info->enum_val_info[enum_idx].value == data[0])
                {
                    data_u32 = enum_idx;
                    data_ptr = &data_u32;
                    break;
                }
            }
        }
        
        SHR_IF_ERR_EXIT(dbal_field_printable_string_get
                        (unit, field_id, data_ptr, NULL, 0, DBAL_FIELD_EMPTY, data_size_bits, decoded_data_string));

        if (sal_strncmp(decoded_data_string, "No child field found!", sal_strlen("No child field found!")) == 0)
        {
            
            SHR_EXIT();
        }

    }

    
    if (NULL == *parsed_info)
    {
        if ((*parsed_info = utilex_rhlist_create("dbal information", sizeof(signal_output_t), 0)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Could not allocate parsed information list to parse the required buffer.\n");
        }
    }

    
    if ((decoded_values = utilex_rhlist_create("Decoded values", sizeof(signal_output_t), 0)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Could not allocate parsed information list to parse the required buffer.\n");
    }

    if (is_dbal_field)
    {
        SHR_IF_ERR_EXIT(dbal_string_convert_to_signal_string
                        (unit, field_id, data, decoded_data_string, field_info, encode_type, data_size_bits,
                         decoded_values));
    }
    else
    {
        
        SHR_IF_ERR_EXIT(dbal_table_parse_to_signal_string(unit, core, desc, table_id, decoded_values));
    }

    RHITERATOR(sig, decoded_values)
    {
        
        SHR_IF_ERR_EXIT(dnx_dbal_decode_field_signal_copy(unit, sig, *parsed_info));
    }

exit:
    if (decoded_values)
    {
        sand_signal_list_free(decoded_values);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_table_is_tcam(
    int unit,
    char *table_name,
    uint8 *is_tcam)
{
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, table_name, &table_id));

    *is_tcam = dbal_tables_is_tcam(unit, table_id);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_table_is_shared_between_cores(
    int unit,
    char *table_name,
    uint8 *is_shared_between_cores)
{
    dbal_tables_e table_id;
    dbal_core_mode_e core_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, table_name, &table_id));

    SHR_IF_ERR_EXIT(dbal_tables_core_mode_get(unit, table_id, &core_mode));

    *is_shared_between_cores = (core_mode == DBAL_CORE_MODE_SBC);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_table_access_method_get(
    int unit,
    char *table_name,
    char **access_method)
{
    dbal_tables_e table_id;
    dbal_access_method_e access_method_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, table_name, &table_id));

    SHR_IF_ERR_EXIT(dbal_tables_access_method_get(unit, table_id, &access_method_id));

    *access_method = dbal_access_method_to_string(unit, access_method_id);

exit:
    SHR_FUNC_EXIT;
}
