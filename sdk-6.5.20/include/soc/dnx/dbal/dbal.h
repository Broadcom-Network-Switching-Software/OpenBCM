/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */




#ifndef DBAL_H_INCLUDED
#define DBAL_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#    error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/dnx/dbal/dbal_external_defines.h>
#include <soc/dnx/dbal/dbal_string_mgmt.h>








#define DBAL_FUNC_INIT_VARS(_in_unit)                                                                           \
{                                                                                                               \
  int _dbal_handles[DBAL_FUNC_NOF_ENTRY_HANDLES]=                                                               \
      {DBAL_SW_NOF_ENTRY_HANDLES,DBAL_SW_NOF_ENTRY_HANDLES,DBAL_SW_NOF_ENTRY_HANDLES,DBAL_SW_NOF_ENTRY_HANDLES, \
       DBAL_SW_NOF_ENTRY_HANDLES,DBAL_SW_NOF_ENTRY_HANDLES,DBAL_SW_NOF_ENTRY_HANDLES,DBAL_SW_NOF_ENTRY_HANDLES, \
       DBAL_SW_NOF_ENTRY_HANDLES,DBAL_SW_NOF_ENTRY_HANDLES};                                                    \
  int _free_handle_index;                                                                                       \
  int _nof_local_handles = 0;                                                                                   \
  int _unit = _in_unit


#define DBAL_HANDLE_ALLOC(_unit, _table_id, _entry_handle_id)                         \
    dbal_entry_handle_take_macro(_unit, _table_id, _entry_handle_id);                 \
if((*_entry_handle_id) < DBAL_SW_NOF_ENTRY_HANDLES)                                        \
{                                                                                       \
    do                                                                                    \
    {                                                                                     \
        int _handle_index;                                                                \
        uint32 *_entry_handle_id_ptr = _entry_handle_id;                                  \
                                                                                          \
                       \
        if (_nof_local_handles == DBAL_FUNC_NOF_ENTRY_HANDLES)                            \
        {                                                                                 \
            dbal_entry_handle_release_internal(_unit, *_entry_handle_id_ptr);             \
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "function dbal handle usage exceed maximum %d\n"\
                        ,DBAL_FUNC_NOF_ENTRY_HANDLES);                                    \
        }                                                                                 \
                                        \
        for (_handle_index=0; _handle_index<DBAL_FUNC_NOF_ENTRY_HANDLES; _handle_index++) \
        {                                                                                 \
            if (_dbal_handles[_handle_index] == DBAL_SW_NOF_ENTRY_HANDLES)                \
            {                                                                             \
                                                \
                _dbal_handles[_handle_index] = *_entry_handle_id_ptr;                     \
                _nof_local_handles++;                                                     \
                break;                                                                    \
            }                                                                             \
        }                                                                                 \
    } while(0);                                                                          \
}

 
 #define DBAL_HANDLE_COPY(_unit, _src_handle_id, _dst_handle_id)                       \
     dbal_entry_handle_copy_macro(_unit, _src_handle_id, _dst_handle_id);              \
 do                                                                                    \
 {                                                                                     \
     int _handle_index;                                                                \
     uint32 *_entry_handle_id_ptr = _dst_handle_id;                                  \
                                                                                       \
                    \
     if (_nof_local_handles == DBAL_FUNC_NOF_ENTRY_HANDLES)                            \
     {                                                                                 \
         dbal_entry_handle_release_internal(_unit, *_entry_handle_id_ptr);             \
         SHR_ERR_EXIT(_SHR_E_INTERNAL, "function dbal handle usage exceed maximum %d\n"\
                     ,DBAL_FUNC_NOF_ENTRY_HANDLES);                                    \
     }                                                                                 \
                                     \
     for (_handle_index=0; _handle_index<DBAL_FUNC_NOF_ENTRY_HANDLES; _handle_index++) \
     {                                                                                 \
         if (_dbal_handles[_handle_index] == DBAL_SW_NOF_ENTRY_HANDLES)                \
         {                                                                             \
                                             \
             _dbal_handles[_handle_index] = *_entry_handle_id_ptr;                     \
             _nof_local_handles++;                                                     \
             break;                                                                    \
         }                                                                             \
     }                                                                                 \
 } while(0)


#define DBAL_HANDLE_CLEAR(_unit, _table_id, _entry_handle_id)                         \
    dbal_entry_handle_clear_macro(_unit, _table_id, _entry_handle_id);


#define DBAL_HANDLE_FREE(_unit, _entry_handle_id)                                     \
    SHR_SET_CURRENT_ERR(dbal_entry_handle_release_internal(_unit, _entry_handle_id)); \
do                                                                                    \
{                                                                                     \
    int _handle_index;                                                                \
    for (_handle_index=0; _handle_index<DBAL_FUNC_NOF_ENTRY_HANDLES; _handle_index++) \
    {                                                                                 \
        if (_dbal_handles[_handle_index] == _entry_handle_id)                         \
        {                                                                             \
            _dbal_handles[_handle_index] = DBAL_SW_NOF_ENTRY_HANDLES;                 \
            _nof_local_handles--;                                                     \
            break;                                                                    \
        }                                                                             \
    }                                                                                 \
    if(_handle_index == DBAL_FUNC_NOF_ENTRY_HANDLES)                                  \
    {                                                                                 \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "trying to free handle that already freed\n");  \
    }                                                                                 \
} while(0)


#define DBAL_FUNC_FREE_VARS                                                                               \
    do                                                                                                    \
    {                                                                                                     \
        for (_free_handle_index=0; _free_handle_index<DBAL_FUNC_NOF_ENTRY_HANDLES; _free_handle_index++)  \
        {                                                                                                 \
            if (_dbal_handles[_free_handle_index] != DBAL_SW_NOF_ENTRY_HANDLES)                           \
            {                                                                                             \
                dbal_entry_handle_release_internal(_unit, _dbal_handles[_free_handle_index]);             \
                _nof_local_handles--;                                                                     \
            }                                                                                             \
        }                                                                                                 \
    }while (0);                                                                                           \
}






void dbal_entry_key_field8_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 field_val);


void dbal_entry_key_field16_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint16 field_val);


void dbal_entry_key_field32_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 field_val);


void dbal_entry_key_field64_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint64 field_val);



void dbal_entry_key_field_arr8_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 *field_val);


void dbal_entry_key_field_arr32_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 *field_val);


void dbal_entry_key_field_predefine_value_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    dbal_field_predefine_value_type_e value_type);


void dbal_entry_key_field8_range_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 min_val,
    uint8 max_val);


void dbal_entry_key_field16_range_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint16 min_val,
    uint16 max_val);


void dbal_entry_key_field32_range_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 min_val,
    uint32 max_val);


void dbal_entry_key_field8_masked_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 field_val,
    uint8 field_mask);


void dbal_entry_key_field16_masked_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint16 field_val,
    uint16 field_mask);


void dbal_entry_key_field32_masked_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 field_val,
    uint32 field_mask);


void dbal_entry_key_field_arr8_masked_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 *field_val,
    uint8 *field_mask);


void dbal_entry_key_field_arr32_masked_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 *field_val,
    uint32 *field_mask);


void dbal_entry_key_field64_masked_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint64 field_val,
    uint64 field_mask);


void dbal_entry_value_field8_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint8 field_val);


void dbal_entry_value_field16_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint16 field_val);


void dbal_entry_value_field32_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint32 field_val);


void dbal_entry_value_field_arr8_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint8 *field_val);


void dbal_entry_value_field_arr32_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint32 *field_val);


void dbal_entry_value_field64_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint64 field_val);


void dbal_entry_value_field_predefine_value_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    dbal_field_predefine_value_type_e value_type);


void dbal_entry_value_field_unset(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id);



void dbal_value_field8_request(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint8 *field_val);


void dbal_value_field16_request(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint16 *field_val);


void dbal_value_field32_request(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint32 *field_val);


void dbal_value_field_arr8_request(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint8 *field_val);


void dbal_value_field_arr32_request(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint32 *field_val);


void dbal_value_field64_request(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint64 *field_val);





shr_error_e
dbal_entry_handle_related_table_get(
    int unit,
    uint32 entry_handle_id,
	dbal_tables_e *table_id);



shr_error_e
dbal_entry_handle_key_field_range_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 *min_val,
    uint32 *max_val);


shr_error_e dbal_entry_handle_key_field_arr32_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 *field_val);


shr_error_e dbal_entry_handle_key_field_arr8_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 *field_val);



shr_error_e dbal_entry_handle_key_field_arr8_masked_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 *field_val,
    uint8 *field_mask);


shr_error_e dbal_entry_handle_key_field_arr32_masked_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 *field_val,
    uint32 *field_mask);


shr_error_e dbal_entry_handle_value_field8_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint8 *field_val);


shr_error_e dbal_entry_handle_value_field16_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint16 *field_val);


shr_error_e dbal_entry_handle_value_field32_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint32 *field_val);


shr_error_e dbal_entry_handle_value_field_arr8_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint8 *field_val);


shr_error_e dbal_entry_handle_value_field_arr32_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint32 *field_val);


shr_error_e dbal_entry_handle_value_field64_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint64 *field_val);


shr_error_e dbal_entry_handle_access_id_set(
    int unit,
    uint32 entry_handle_id,
    uint32 entry_access_id);


shr_error_e dbal_entry_handle_access_id_get(
    int unit,
    uint32 entry_handle_id,
    uint32 *entry_access_id);


shr_error_e dbal_entry_attribute_set(
    int unit,
    uint32 entry_handle_id,
    uint32 attr_type,
    uint32 attr_val);


shr_error_e dbal_entry_attribute_get(
    int unit,
    uint32 entry_handle_id,
    uint32 attr_type,
    uint32 *attr_val);


shr_error_e dbal_entry_attribute_request(
    int unit,
    uint32 entry_handle_id,
    uint32 attr_type,
    uint32 *attr_val);


shr_error_e dbal_entry_get(
    int unit,
    uint32 entry_handle_id,
    dbal_entry_action_flags_e flags);



shr_error_e dbal_entry_default_get(
    int unit,
    uint32 entry_handle_id,
	int result_type);


shr_error_e dbal_entry_commit(
    int unit,
    uint32 entry_handle_id,
    dbal_entry_action_flags_e flags);

shr_error_e dbal_entry_clear(
    int unit,
    uint32 entry_handle_id,
    dbal_entry_action_flags_e flags);


shr_error_e dbal_entry_handle_key_payload_update(
    int unit,
    uint32 entry_handle_id,
    uint32 *key_buffer,
    uint32 *key_mask_buffer,
    int core_id,
    int payload_size,
    uint32 *payload_buffer);


shr_error_e dbal_entry_access_id_by_key_get(
    int unit,
    uint32 entry_handle_id,
    uint32 *entry_access_id,
    dbal_entry_action_flags_e flags);



shr_error_e dbal_iterator_init(
    int unit,
    uint32 entry_handle_id,
    dbal_iterator_mode_e mode);


shr_error_e dbal_iterator_key_field_arr8_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    dbal_condition_types_e condition,
    uint8 *field_val,
    uint8 *field_mask);


shr_error_e dbal_iterator_key_field_arr32_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    dbal_condition_types_e condition,
    uint32 *field_val,
    uint32 *field_mask);


shr_error_e
dbal_iterator_key_field32_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    dbal_condition_types_e condition,
    uint32 field_val,
    uint32 field_mask);

shr_error_e dbal_iterator_value_field_arr8_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_idx,
    dbal_condition_types_e condition,
    uint8 *field_val,
    uint8 *field_mask);


shr_error_e dbal_iterator_value_field_arr32_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_idx,
    dbal_condition_types_e condition,
    uint32 *field_val,
    uint32 *field_mask);


shr_error_e
dbal_iterator_value_field32_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_idx,
    dbal_condition_types_e condition,
    uint32 field_val,
    uint32 field_mask);


shr_error_e dbal_iterator_attribute_rule_add(
    int unit,
    uint32 entry_handle_id,
    uint32 attr_type,
    uint32 attr_val);


shr_error_e dbal_iterator_field_arr8_action_add(
    int unit,
    uint32 entry_handle_id,
    dbal_iterator_action_types_e action_type,
    dbal_fields_e field_id,
    int inst_idx,
    uint8 *field_val,
    uint8 *field_mask);


shr_error_e dbal_iterator_field_arr32_action_add(
    int unit,
    uint32 entry_handle_id,
    dbal_iterator_action_types_e action_type,
    dbal_fields_e field_id,
    int inst_idx,
    uint32 *field_val,
    uint32 *field_mask);


shr_error_e dbal_iterator_entry_action_add(
    int unit,
    uint32 entry_handle_id,
    dbal_iterator_action_types_e action_type);


shr_error_e dbal_iterator_attribute_action_add(
    int unit,
    uint32 entry_handle_id,
    uint32 attr_type);



shr_error_e dbal_iterator_action_commit(
    int unit,
    uint32 entry_handle_id);


shr_error_e dbal_iterator_get_next(
    int unit,
    uint32 entry_handle_id,
    int *is_end);


shr_error_e dbal_iterator_destroy_macro(
    int unit,
    uint32 entry_handle_id);


shr_error_e dbal_iterator_entries_count_get(
    int unit,
    uint32 entry_handle_id,
    int *entries_counter);




shr_error_e dbal_fields_parent_field_decode(
    int unit,
    dbal_fields_e field_id,
    uint32 *orig_val,
    dbal_fields_e * sub_field_id,
    uint32 *sub_field_val);


shr_error_e dbal_fields_parent_field_decode_no_err(
    int unit,
    dbal_fields_e field_id,
    uint32 *orig_val,
    dbal_fields_e * sub_field_id,
    uint32 *sub_field_val);


shr_error_e dbal_fields_uint32_sub_field_info_get(
    int unit,
    dbal_fields_e field_id,
    uint32 orig_val,
    dbal_fields_e * sub_field_id,
    uint32 *sub_field_val);


shr_error_e dbal_fields_max_size_get(
    int unit,
    dbal_fields_e field_id,
    uint32 *field_size);


shr_error_e dbal_fields_struct_field_encode(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e field_in_struct_id,
    uint32 *field_in_struct_val,
    uint32 *full_field_val);


shr_error_e dbal_fields_struct_field_encode_masked(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e field_in_struct_id,
    uint32 *field_in_struct_val,
    uint32 *field_in_struct_mask,
    uint32 *full_field_val,
    uint32 *full_field_mask);

shr_error_e dbal_fields_struct_field_decode(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e field_in_struct_id,
    uint32 *field_in_struct_val,
    uint32 *full_field_val);


shr_error_e dbal_fields_struct_field_decode_masked(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e field_in_struct_id,
    uint32 *field_in_struct_val,
    uint32 *field_in_struct_mask,
    uint32 *full_field_val,
    uint32 *full_field_mask,
    int is_uint32);


shr_error_e dbal_fields_parent_field32_value_set(
    int unit,
    dbal_fields_e parent_field_id,
    dbal_fields_e sub_field_id,
    uint32 *sub_field_val,
    uint32 *parent_field_val);


shr_error_e dbal_fields_enum_hw_value_get(
    int unit,
    dbal_fields_e field_id,
    uint32 enum_value,
    uint32 *hw_value);


shr_error_e dbal_fields_enum_value_get(
    int unit,
    dbal_fields_e field_id,
    uint32 hw_value,
    uint32 *enum_value);


shr_error_e dbal_fields_is_illegal_value(
    int unit,
    dbal_fields_e field_id,
    uint32 value,
    uint8 *is_illegal);



shr_error_e dbal_fields_is_allocator_get(
    int unit,
    dbal_fields_e field_id,
    uint8 *is_allocator);


shr_error_e dbal_field_struct_field_size_get(
    int unit,
    dbal_field_types_defs_e field_type,
    dbal_fields_e field_id,
    uint32 *field_size_p);


shr_error_e dbal_fields_field_type_get(
    int unit,
    dbal_fields_e field_id,
    dbal_field_types_defs_e * field_type);


shr_error_e dbal_fields_predefine_value_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int is_key,
    dbal_field_predefine_value_type_e value_type,
    uint32 *predef_value);



shr_error_e dbal_table_clear(
    int unit,
    dbal_tables_e table_id);


shr_error_e dbal_tables_table_nof_res_type_get(
    int unit,
    dbal_tables_e table_id,
    int *nof_res_type);


shr_error_e dbal_tables_capacity_get(
    int unit,
    dbal_tables_e table_id,
    int *max_capacity);


shr_error_e dbal_tables_field_info_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    dbal_table_field_info_t * field_info);


shr_error_e dbal_tables_field_size_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    int *field_size);



shr_error_e dbal_tables_field_nof_instance_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int res_type,
    int *nof_inst);


shr_error_e dbal_tables_field_max_value_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    int *max_value);


shr_error_e dbal_tables_hw_value_result_type_get(
    int unit,
    dbal_tables_e table_id,
    uint32 hw_value,
    int *result_type_idx);


shr_error_e dbal_tables_result_type_by_name_get(
    int unit,
    dbal_tables_e table_id,
    char *result_type_name,
    int *result_type_idx);


shr_error_e dbal_tables_field_offset_in_hw_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    int *field_offset);


shr_error_e dbal_tables_field_is_key_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 *is_key);


shr_error_e
dbal_tables_next_table_get(
    int unit,
	dbal_tables_e table_id,
    dbal_labels_e label,
    dbal_access_method_e access_method,
    dbal_physical_tables_e mdb_physical_db,
	dbal_table_type_e table_type,
	dbal_tables_e 	*next_table_id);


shr_error_e
dbal_tables_next_field_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    int is_key,
    int result_type,
    dbal_fields_e * next_field_id);


shr_error_e dbal_tables_field_info_get_no_err(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    dbal_table_field_info_t * field_info);


shr_error_e dbal_tables_app_db_id_get(
    int unit,
    dbal_tables_e table_id,
    uint32 *app_db_id,
    int *app_db_size);


shr_error_e dbal_tables_core_mode_get(
    int unit,
    dbal_tables_e table_id,
    dbal_core_mode_e * core_mode);


shr_error_e dbal_tables_result_type_hw_value_get(
    int unit,
    dbal_tables_e table_id,
    int result_type_idx,
    uint32 *hw_value);


shr_error_e dbal_tables_tcam_handler_id_get(
    int unit,
    dbal_tables_e table_id,
    uint32 *tcam_handler_id);


shr_error_e
dbal_tables_indication_get(
    int unit,
    dbal_tables_e table_id,
    dbal_table_indications_e ind_type,
    uint32 *ind_val);


shr_error_e
dbal_tables_indication_set(
    int unit,
    dbal_tables_e table_id,
    dbal_table_indications_e ind_type,
    uint32 ind_val);


shr_error_e dbal_tables_table_type_get(
    int unit,
    dbal_tables_e table_id,
    dbal_table_type_e * table_type);


shr_error_e dbal_tables_payload_size_get(
    int unit,
    dbal_tables_e table_id,
    int res_type_idx,
    int *p_size);


shr_error_e dbal_table_field_printable_string_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint32 buffer_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    uint32 buffer_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    int result_type_idx,
    uint8 is_key,
    char buffer_to_print[DBAL_MAX_PRINTABLE_BUFFER_SIZE]);


shr_error_e dbal_table_field_is_ranged_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 * is_ranged);


shr_error_e dbal_table_field_is_packed_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 * is_packed);


shr_error_e dbal_tables_table_access_info_get(
    int unit,
    dbal_tables_e table_id,
    dbal_access_method_e access_method,
    void *access_info);



shr_error_e dbal_table_printable_entry_get(
    int unit,
    dbal_tables_e table_id,
    uint32 *key_buffer,
    int core_id,
    int payload_size,
    uint32 *payload_buffer,
    dbal_printable_entry_t * entry_print_info);


shr_error_e dbal_tables_tcam_handler_id_set(
    int unit,
    dbal_tables_e table_id,
    uint32 tcam_handler_id);


shr_error_e dbal_tables_field_predefine_value_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    dbal_field_predefine_value_type_e value_type,
    uint32 *predef_value);


shr_error_e
dbal_tables_is_table_active(
    int unit,
    dbal_tables_e table_id,
    int *is_table_active);


shr_error_e dbal_tables_access_method_get(
    int unit,
    dbal_tables_e table_id,
    dbal_access_method_e * access_method);


shr_error_e dbal_entry_result_type_update(
    int unit,
    uint32 entry_handle_id,
    int new_result_type,
    dbal_result_type_update_mode_e mode);




shr_error_e dbal_bulk_mode_set(
    int unit,
    dbal_access_method_e access_method,
    dbal_bulk_module_e bulk_module,
    int collect_mode);


shr_error_e dbal_bulk_mode_get(
    int unit,
    dbal_access_method_e access_method,
    dbal_bulk_module_e bulk_module,
    int *collect_mode);


shr_error_e dbal_bulk_commit(
    int unit,
    dbal_access_method_e access_method,
    dbal_bulk_module_e bulk_module);


shr_error_e dbal_bulk_clear(
    int unit,
    dbal_access_method_e access_method,
    dbal_bulk_module_e bulk_module);

int
dbal_is_collection_mode_enabled(
    int unit);



shr_error_e
dbal_collection_trigger_set(
   int unit, 
   dbal_collection_trigger_type_e trigger_type);


shr_error_e dbal_init(
    int unit);


shr_error_e dbal_deinit(
    int unit);


shr_error_e dbal_device_init_done(
    int unit);


shr_error_e dbal_field_types_init(
    int unit);


shr_error_e dbal_field_types_deinit(
    int unit);


shr_error_e dbal_tables_init(
    int unit);


shr_error_e dbal_tables_deinit(
    int unit);


shr_error_e dbal_access_method_init(
   int unit, 
   dbal_access_method_e access_method);


shr_error_e dbal_access_method_deinit(
   int unit, 
   dbal_access_method_e access_method);


shr_error_e dnx_dbal_post_init_operations(
    int unit);


shr_error_e dnx_dbal_pre_deinit_operations(
    int unit);



shr_error_e dbal_logger_table_lock(
    int unit,
    dbal_tables_e table_id);


shr_error_e
dbal_logger_table_mode_set(
    int unit,
    dbal_logger_mode_e mode);


extern shr_error_e dbal_entry_handle_take_macro(
    int unit,
    dbal_tables_e table_id,
    uint32 *entry_handle_id);


extern shr_error_e dbal_entry_handle_clear_macro(
    int unit,
    dbal_tables_e table_id,
    uint32 entry_handle_id);


extern shr_error_e dbal_entry_handle_copy_macro(
    int unit,
    uint32 src_handle_id,
    uint32 *dst_handle_id);


extern shr_error_e dbal_entry_handle_release_internal(
    int unit,
    uint32 entry_handle_id);


shr_error_e
dbal_tables_table_is_direct(
    int unit,
    dbal_tables_e table_id,
    int *is_table_direct);


#endif
