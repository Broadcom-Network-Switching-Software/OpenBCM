
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_ACTIONS_TYPES_H__
#define __DNX_FIELD_ACTIONS_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>



#define DNX_FIELD_IGNORE_ALL_ACTIONS (uint8)(-1)


#define DNX_FIELD_IGNORE_NO_ACTIONS (uint8)(0)


#define DNX_FIELD_FEM_MAP_INDEX_INVALID ((dnx_field_fem_map_index_t)(-1))




typedef uint8 dnx_field_fem_condition_ms_bit_t;


typedef uint8 dnx_field_fem_map_index_t;


typedef uint8 dnx_field_fem_map_data_t;


typedef uint8 dnx_field_fem_action_valid_t;


typedef uint8 dnx_field_fem_program_t;


typedef uint8 dnx_field_fem_bit_val_t;


typedef uint32 dnx_field_fem_adder_t;




typedef enum {
    
    DNX_FIELD_ACTION_FLAG_WITH_ID = 0x1,
    DNX_FIELD_ACTION_FLAG_NOF
} dnx_field_action_flags_e;


typedef enum {
    
    DNX_FIELD_FEM_BIT_FORMAT_INVALID = -1,
    
    DNX_FIELD_FEM_BIT_FORMAT_FIRST = 0,
    
    DNX_FIELD_FEM_BIT_FORMAT_FROM_KEY_SELECT = DNX_FIELD_FEM_BIT_FORMAT_FIRST,
    
    DNX_FIELD_FEM_BIT_FORMAT_FROM_MAP_DATA,
    
    DNX_FIELD_FEM_BIT_FORMAT_FROM_THIS_FIELD,
    DNX_FIELD_FEM_BIT_FORMAT_NUM
} dnx_field_fem_bit_format_e;




typedef struct {
    
    dnx_field_action_t dnx_action;
    
    uint8 dont_use_valid_bit;
    
    uint8 lsb;
} __ATTRIBUTE_PACKED__ dnx_field_action_in_group_info_t;


typedef struct {
    
    dnx_field_action_in_group_info_t actions_on_payload_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
} __ATTRIBUTE_PACKED__ dnx_field_actions_fg_payload_sw_info_t;


typedef struct {
    
    dnx_field_action_in_group_info_t actions_on_payload_info[DNX_DATA_MAX_FIELD_ACE_NOF_ACTION_PER_ACE_FORMAT];
} __ATTRIBUTE_PACKED__ dnx_field_actions_ace_payload_sw_info_t;


typedef struct {
    
    uint32 valid;
    
    dbal_fields_e field_id;
    
    uint32 size;
    
    uint32 prefix;
    
    uint8 prefix_size;
    
    int bcm_id;
    
    uint32 ref_count;
    
    dnx_field_action_flags_e flags;
    
    dnx_field_action_t base_dnx_action;
} __ATTRIBUTE_PACKED__ dnx_field_user_action_info_t;

typedef union {
    
    dnx_field_fem_bit_val_t bit_on_key_select;
    
    dnx_field_fem_bit_val_t bit_on_map_data;
    
    dnx_field_fem_bit_val_t bit_value;
    
    uint8 bit_descriptor_value;
} dnx_field_fem_bit_value_t;


typedef struct {
    
    dnx_field_fem_bit_format_e fem_bit_format;
    
    dnx_field_fem_bit_value_t fem_bit_value;
} __ATTRIBUTE_PACKED__ dnx_field_fem_bit_info_t;


typedef struct {
    
    dnx_field_action_type_t fem_action;
    
    dnx_field_fem_bit_info_t fem_bit_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FEM_ACTION];
    
    dnx_field_fem_adder_t fem_adder;
} __ATTRIBUTE_PACKED__ dnx_field_fem_action_entry_t;


typedef struct {
    
    dnx_field_fem_map_index_t fem_map_index;
    
    dnx_field_fem_map_data_t fem_map_data;
    
    dnx_field_fem_action_valid_t fem_action_valid;
} __ATTRIBUTE_PACKED__ dnx_field_fem_condition_entry_t;


typedef struct {
    
    dnx_field_group_t fg_id;
    
    uint8 input_offset;
    
    uint8 input_size;
    
    dnx_field_group_t second_fg_id;
    
    uint8 ignore_actions;
    
    dnx_field_action_t* fem_encoded_actions;
} __ATTRIBUTE_PACKED__ dnx_field_fg_id_info_t;


typedef struct {
    
    dnx_field_fg_id_info_t* fg_id_info;
} __ATTRIBUTE_PACKED__ dnx_field_fem_state_t;


typedef struct {
    
    dnx_field_user_action_info_t info[DNX_DATA_MAX_FIELD_ACTION_USER_NOF];
    
    dnx_field_fem_state_t* fem_info;
} dnx_field_action_sw_db_t;


#endif 
