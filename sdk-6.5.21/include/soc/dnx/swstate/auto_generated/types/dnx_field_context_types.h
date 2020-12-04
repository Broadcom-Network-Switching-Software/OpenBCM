
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_CONTEXT_TYPES_H__
#define __DNX_FIELD_CONTEXT_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <include/soc/dnx/dbal/dbal_structures.h>
#include <soc/dnxc/swstate/types/sw_state_string.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_key_types.h>



typedef enum {
    
    DNX_FIELD_CONTEXT_COMPARE_MODE_FIRST,
    
    DNX_FIELD_CONTEXT_COMPARE_MODE_NONE = DNX_FIELD_CONTEXT_COMPARE_MODE_FIRST,
    
    DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE,
    
    DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE,
    
    DNX_FIELD_CONTEXT_COMPARE_MODE_NOF
} dnx_field_context_compare_mode_e;


typedef enum {
    
    DNX_FIELD_CONTEXT_HASH_MODE_FIRST = 0,
    
    DNX_FIELD_CONTEXT_HASH_MODE_DISABLED = DNX_FIELD_CONTEXT_HASH_MODE_FIRST,
    
    DNX_FIELD_CONTEXT_HASH_MODE_ENABLED,
    
    DNX_FIELD_CONTEXT_HASH_MODE_NOF
} dnx_field_context_hash_mode_e;


typedef enum {
    
    DNX_FIELD_CONTEXT_STATE_TABLE_MODE_FIRST,
    
    DNX_FIELD_CONTEXT_STATE_TABLE_MODE_DISABLED = DNX_FIELD_CONTEXT_STATE_TABLE_MODE_FIRST,
    
    DNX_FIELD_CONTEXT_STATE_TABLE_MODE_ENABLED,
    
    DNX_FIELD_CONTEXT_STATE_TABLE_MODE_NOF
} dnx_field_context_state_table_mode_e;




typedef struct {
    
    dnx_field_key_template_t key_template;
    
    dnx_field_key_id_t key_id;
    
    dnx_field_qual_attach_info_t attach_info[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
} dnx_field_context_key_info_t;


typedef struct {
    
    dnx_field_context_compare_mode_e mode;
    
    uint8 is_set;
    
    dnx_field_context_key_info_t key_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE];
} dnx_field_context_compare_mode_info_t;


typedef struct {
    
    dnx_field_context_compare_mode_info_t pair_1;
    
    dnx_field_context_compare_mode_info_t pair_2;
} dnx_field_context_compare_info_t;


typedef struct {
    
    dnx_field_context_hash_mode_e mode;
    
    uint8 is_set;
    
    dnx_field_context_key_info_t key_info;
} dnx_field_context_hashing_info_t;


typedef struct {
    
    dnx_field_context_state_table_mode_e mode;
    
    uint8 is_set;
} dnx_field_context_state_table_info_t;


typedef struct {
    
    dnx_field_context_compare_info_t compare_info;
    
    dnx_field_context_hashing_info_t hashing_info;
} dnx_field_context_ipmf1_sw_info_t;


typedef struct {
    
    dnx_field_context_t cascaded_from;
} dnx_field_context_ipmf2_sw_info_t;


typedef struct {
    
    sw_state_string_t value[DBAL_MAX_STRING_LENGTH];
} dnx_field_context_name_t;


typedef struct {
    
    dnx_field_context_ipmf1_sw_info_t context_ipmf1_info;
    
    dnx_field_context_ipmf2_sw_info_t context_ipmf2_info;
    
    dnx_field_context_state_table_info_t state_table_info;
    
    dnx_field_context_name_t name[DNX_FIELD_STAGE_NOF];
} dnx_field_context_sw_info_t;


typedef struct {
    
    dnx_field_context_sw_info_t* context_info;
} dnx_field_context_sw_t;


#endif 
