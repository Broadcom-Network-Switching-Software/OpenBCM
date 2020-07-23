/**
 * \file dbal_db_init.h
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Main functions for init the dbal fields and logical tables DB
 */

#ifndef DBAL_DB_INIT_H_INCLUDED
#define DBAL_DB_INIT_H_INCLUDED


#include <soc/dnx/dbal/dbal_structures.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <soc/dnx/pp_stage.h>




#define DBAL_TABLE_MAX_NUM_OF_ACCESS_FIELDS                       400


#define DBAL_TABLE_FIELD_POSITION_UNKNOWN                         -1
#define DBAL_TABLE_FIELD_KEY_OR_RESULT                            -1


#define DB_INIT_DNX_DATA_INDICATION                       "DNX_DATA"


#define DB_INIT_PHYDB_INDICATION                          "PHYDB"


#define DBAL_LOG2_FUNCTION                                  "#log2_round_up"

#define DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(_table_entry, _num_of_results_types)                                    \
    (_table_entry->nof_result_types) = (_num_of_results_types);                                                             \
    if (_table_entry->nof_result_types > DBAL_MAX_NUMBER_OF_RESULT_TYPES - 1)                                           \
    {                                                                                                                   \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s has more result types than max allowed.\n", _table_entry->table_name);  \
    }

#define DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(_table_entry, _multi_res_info, _num_of_results_fields)         \
    (_multi_res_info.nof_result_fields) = (_num_of_results_fields);                                                 \
    if (_multi_res_info.nof_result_fields > DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE)                   \
    {                                                                                                           \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s, result type %s has more result fields than max allowed.\n",    \
                     _table_entry->table_name, _multi_res_info.result_type_name);                               \
    }

#define DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(_table_entry, _multi_res_info)                         \
            SHR_ALLOC_SET_ZERO(_multi_res_info.results_info,                                                    \
                           _multi_res_info.nof_result_fields * sizeof(dbal_table_field_info_t),                 \
                           "results fields info allocation",                                                    \
                           "%s%s%s\r\n", _table_entry->table_name, EMPTY, EMPTY)

#define DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(_multi_res_info, _hw_value) \
            _multi_res_info.result_type_hw_value[_multi_res_info.result_type_nof_hw_values++] = _hw_value;

#define DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(_table_entry, _is_standard_1_table, _is_compatible_with_all_images)  \
    if (_is_standard_1_table || !_is_compatible_with_all_images)                                                                    \
    {                                                                                                                               \
                                                                                         \
        SHR_BITSET(_table_entry->indications_bm, DBAL_TABLE_IND_IS_IMAGE_SPECIFIC_TABLE);                                           \
    }



typedef struct
{
    
    int int_val;

    
    char formula_as_string[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];

    
    dbal_offset_formula_t *formula;

        
    int is_invalid;

} dbal_db_int_or_dnx_data_info_struct_t;


typedef struct
{
    
    char module[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];

    
    char submodule[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];

    
    char data[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];

    
    char member[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];

    
    int key1;

    
    int key2;
} dbal_parsed_db_dnx_data_struct_t;


typedef struct
{
    
    char name[DBAL_MAX_STRING_LENGTH];

    
    char encode_type[DBAL_MAX_SHORT_STRING_LENGTH];

    
    int encode_param1;

    
    char reference_field_value[DBAL_MAX_STRING_LENGTH];

} dbal_db_child_field_info_struct_t;


typedef struct
{
    
    char name[DBAL_MAX_STRING_LENGTH];

    
    int start_offest;

    
    int length;

    
    int arr_prefix_table_entry_value;

    
    int arr_prefix_table_entry_width;

} dbal_db_struct_field_info_struct_t;


typedef struct
{
    
    char name_from_interface[2 * DBAL_MAX_STRING_LENGTH];

    
    int value_from_mapping;
    uint8 is_invalid_value_from_mapping;

} dbal_db_enum_info_struct_t;


typedef struct
{
    
    char define_name[DBAL_MAX_STRING_LENGTH];

    
    int define_value;

} dbal_db_defines_info_struct_t;


typedef struct
{

    
    int nof_childs;

    
    dbal_db_child_field_info_struct_t *childs;

    
    int nof_enum_vals;

    
    dbal_db_enum_info_struct_t *enums;

    
    int nof_defined_vals;

    
    dbal_db_defines_info_struct_t *define;

    
    int max_value;

    
    int min_value;

    
    int const_value;

    
    int nof_illegal_value;

    
    int *illegal_values;

    
    int default_val;

    
    uint8 default_val_valid;

} field_type_db_struct_t;


typedef struct
{
    
    int valid;

    
    int size;

    
    int nof_instances;

    
    int max_nof_instances;

    
    uint8 is_validate_nof_instances;

    
    int max_value;

    
    int min_value;

    
    int const_value;

    
    dbal_field_permission_e permission;

    
    uint8 is_reverse_order;

    
    int offset;

    
    int arr_prefix_value;

    
    int arr_prefix_width;

} table_db_field_params_struct_t;


typedef struct
{
    
    int formula_int;
    
    dnx_dbal_formula_offset_cb formula_cb;
} table_db_offset_in_hw_struct_t;


typedef struct
{
    dbal_condition_types_e condition_type;

    dbal_fields_e field_id;

    
    int value;

    
    
    uint32 enum_val;

} table_db_access_condition_struct_t;


typedef struct
{
    
    int app_db_id;

    
    int app_db_id_size;

    
    char phy_db[DBAL_MAX_SHORT_STRING_LENGTH];

} table_db_mdb_access_struct_t;


typedef struct
{
    
    uint32 pemla_key_mapping[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS];

    
    uint32 pemla_result_mapping[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE];
} table_db_pemla_access_struct_t;


typedef struct
{
    
    dbal_hard_logic_access_types_e access_type;

    
    dbal_fields_e access_field_id;

    
    dbal_hl_tcam_access_type_e tcam_access_type;

    
    dbal_hw_entity_group_e hw_entity_group_id;

    

    
    int reg_mem_hw_entity_id;

    
    int access_size;

    
    int access_offset;

    
    
    dbal_value_field_encode_types_e encode_type;

    
    int encode_param1;

    int nof_conditions;

    
    table_db_access_condition_struct_t access_condition[DBAL_FIELD_MAX_NUM_OF_CONDITIONS];

    
    table_db_offset_in_hw_struct_t array_offset;

    
    table_db_offset_in_hw_struct_t entry_offset;

    
    table_db_offset_in_hw_struct_t block_index;

    
    table_db_offset_in_hw_struct_t data_offset;

    
    table_db_offset_in_hw_struct_t group_offset;

    
    table_db_offset_in_hw_struct_t alias_data_offset;

    
    int hw_field;

    
    int alias_reg_mem_hw_entity_id;

} table_db_access_params_struct_t;


#define DBAL_INIT_HARD_LOGIC_ACCESS_STRUCT_PACKED_FIELDS  (0x1)


typedef struct
{
    
    int reg_mem_hw_entity_id;

} table_db_cs_access_struct_t;


typedef struct
{
    
    uint8 is_packed_fields;

    
    int nof_access;

    table_db_access_params_struct_t access[DBAL_TABLE_MAX_NUM_OF_ACCESS_FIELDS];

    
    char mapping_result_name[DBAL_MAX_STRING_LENGTH];

} table_db_hl_access_struct_t;


typedef struct
{
    
    dbal_stage_e pipe_stage;

    
    int indirect_table_size;

    
    dbal_physical_tables_e physical_db_id;
    char table_size_str[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];

    
    int is_hook_active; 

    
    table_db_mdb_access_struct_t *mdb_access;

    
    table_db_hl_access_struct_t *hl_access;

    
    table_db_cs_access_struct_t *cs_access;

    
    table_db_pemla_access_struct_t *pemla_access;
} table_db_struct_t;




shr_error_e dbal_logical_table_t_init(
    int unit,
    dbal_logical_table_t * table_info);


shr_error_e dbal_logical_dynamic_table_t_init(
    int unit,
    dbal_logical_table_t * table_info);


shr_error_e dbal_db_init_table_alloc(
    int unit,
    dbal_logical_table_t * table_info);


shr_error_e dbal_db_init_dynamic_dbal_tables_sw_state(
    int unit);


shr_error_e dbal_db_init_dynamic_dbal_tables_memory(
    int unit);


shr_error_e dbal_db_init_logical_tables(
    int unit,
    dbal_logical_table_t * table_info);


shr_error_e dbal_db_init_tables_logical_validation(
    int unit,
    dbal_logical_table_t * tables_info);


void dbal_init_table_db_struct_clear(
    table_db_struct_t * cur_table_param);


shr_error_e dbal_db_init_general_info_set(
    int unit,
    dbal_logical_table_t * table_entry,
    dbal_tables_e table_id,
    int is_valid,
    char *table_name,
    dbal_access_method_e access_method);


void dbal_db_init_table_db_struct_hl_access_params_clear(
    table_db_access_params_struct_t * access);


void dbal_db_init_table_db_struct_access_clear(
    table_db_struct_t * cur_table_param,
    dbal_logical_table_t * table_entry);


void dbal_db_init_table_field_params_init(
    table_db_field_params_struct_t * field_db);


shr_error_e dbal_db_init_table_field_set_mandatory_values(
    int unit,
    dbal_fields_e field_id,
    dbal_field_types_defs_e field_type,
    dbal_table_field_info_t * field_info);


shr_error_e dbal_db_init_table_field_set_optional_or_default_values(
    int unit,
    dbal_logical_table_t * table_entry,
    dbal_table_field_info_t * field_info,
    table_db_field_params_struct_t * field_read_data,
    uint8 is_key_fields);


void dbal_db_init_table_set_field_arr_prefix(
    int unit,
    table_db_field_params_struct_t * field_db,
    uint32 arr_prefix_value,
    uint32 arr_prefix_width);


shr_error_e dbal_db_init_table_set_access_register_memory(
    int unit,
    table_db_access_params_struct_t * access_params,
    dbal_hw_entity_group_e hw_entity_group_id,
    int reg_mem_hw_entity_id,
    int hw_field,
    int alias_reg_mem_hw_entity_id);


shr_error_e dbal_db_init_table_set_access_condition(
    int unit,
    table_db_access_condition_struct_t * access_condition,
    dbal_condition_types_e type,
    dbal_fields_e field,
    int value,
    uint32 enum_value);


shr_error_e dbal_db_init_table_set_access_field(
    int unit,
    table_db_access_params_struct_t * access_params,
    int field_size,
    int field_offset,
    dbal_value_field_encode_types_e encode_type,
    int encode_value);


shr_error_e dbal_db_init_hard_logic_direct_set_access_type(
    int unit,
    table_db_access_params_struct_t * access_params,
    uint8 is_register,
    uint8 is_memory);


shr_error_e dbal_db_init_sw_state_set_indirect_table_size(
    int unit,
    table_db_struct_t * cur_table_param,
    char *indirect_table_size,
    int indirect_table_size_int);


void dbal_db_init_table_set_pemla_mapping_by_index(
    table_db_struct_t * table_param,
    dbal_logical_table_t * table_entry);

shr_error_e dbal_db_init_table_tcam_cs_mapping_add(
    int unit,
    table_db_struct_t * cur_table_param,
    soc_mem_t mem);


shr_error_e dbal_db_init_table_set_mdb_info(
    int unit,
    table_db_struct_t * cur_table_param,
    char *phy_db,
    int app_db_id_size,
    int app_db_id,
    dbal_stage_e stage);


shr_error_e dbal_db_init_table_add(
    int unit,
    table_db_struct_t * table_params,
    dbal_tables_e table_id,
    dbal_logical_table_t * table_info);


shr_error_e dbal_db_parse_dnx_data_indication(
    int unit,
    dbal_db_int_or_dnx_data_info_struct_t * int_or_dnx_data_as_formula,
    char *value_to_parse,
    uint8 is_mandatory,
    int default_val,
    char *str_child_name,
    char *str_to_read);

char *dbal_db_init_image_name_get(
    int unit);


shr_error_e dbal_db_init_field_type_general_info_set(
    int unit,
    field_type_db_struct_t * cur_field_type_param,
    dbal_field_types_basic_info_t * field_types_info,
    char *name,
    int size,
    int is_valid,
    uint8 has_resource_name,
    dbal_field_print_type_e print_type,
    uint8 is_common,
    uint8 is_default,
    uint8 is_standard_1);

void dbal_init_field_type_db_struct_clear(
    int unit,
    field_type_db_struct_t * cur_field_type_param);


shr_error_e dbal_db_init_field_type_add(
    int unit,
    field_type_db_struct_t * fld_type_prm,
    dbal_field_types_basic_info_t * field_types_info);

shr_error_e dbal_db_init_hw_element_group_info_init(
    int unit,
    hl_groups_info_t * groups_info);

shr_error_e dbal_db_init_field_type_struct_alloc(
    int unit,
    dbal_field_types_basic_info_t * fld_type_ent);


shr_error_e dbal_db_init_hw_entity_group_general_info_set(
    int unit,
    hl_group_info_t * group_info,
    uint32 element_counter,
    uint8 is_reg);


void dbal_db_init_hw_entity_group_reg_mem_name_set(
    int unit,
    hl_group_info_t * group_info,
    int element_index,
    int reg_or_mem_id);


void dbal_db_init_hw_entity_porting_reg_mapping_set(
    int unit,
    hl_porting_info_t * hl_porting_info,
    int reg_id,
    int map_to_reg_id);


void dbal_db_init_hw_entity_porting_mem_mapping_set(
    int unit,
    hl_porting_info_t * hl_porting_info,
    int mem_id,
    int map_to_mem_id);


void dbal_db_init_table_calculate_key_size(
    dbal_logical_table_t * table_entry);


shr_error_e dbal_db_init_table_and_db_fields_set_allocator_fields(
    int unit,
    dbal_logical_table_t * table_entry,
    int num_of_fields);


shr_error_e dbal_db_init_table_db_interface_results_alloc(
    int unit,
    dbal_logical_table_t * table_entry);


shr_error_e dbal_db_init_table_add_result_type_physical_values_by_reference_field(
    int unit,
    multi_res_info_t * multi_res_info,
    dbal_field_types_defs_e reference_field);


void dbal_db_init_table_add_result_type_physical_values_by_result_type_index(
    dbal_logical_table_t * table_entry);


shr_error_e dbal_db_init_table_set_table_incompatible_image_sw_state_indication(
    int unit,
    dbal_tables_e table_id,
    uint8 is_standard_1_table,
    uint8 is_compatible_with_all_images);

#endif 
