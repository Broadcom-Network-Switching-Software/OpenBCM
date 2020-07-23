
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_ACTIONS_ACCESS_H__
#define __DNX_FIELD_ACTIONS_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_actions_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>



typedef int (*dnx_field_action_sw_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_field_action_sw_db_init_cb)(
    int unit);


typedef int (*dnx_field_action_sw_db_info_set_cb)(
    int unit, uint32 info_idx_0, CONST dnx_field_user_action_info_t *info);


typedef int (*dnx_field_action_sw_db_info_get_cb)(
    int unit, uint32 info_idx_0, dnx_field_user_action_info_t *info);


typedef int (*dnx_field_action_sw_db_info_valid_set_cb)(
    int unit, uint32 info_idx_0, uint32 valid);


typedef int (*dnx_field_action_sw_db_info_valid_get_cb)(
    int unit, uint32 info_idx_0, uint32 *valid);


typedef int (*dnx_field_action_sw_db_info_field_id_set_cb)(
    int unit, uint32 info_idx_0, dbal_fields_e field_id);


typedef int (*dnx_field_action_sw_db_info_field_id_get_cb)(
    int unit, uint32 info_idx_0, dbal_fields_e *field_id);


typedef int (*dnx_field_action_sw_db_info_size_set_cb)(
    int unit, uint32 info_idx_0, uint32 size);


typedef int (*dnx_field_action_sw_db_info_size_get_cb)(
    int unit, uint32 info_idx_0, uint32 *size);


typedef int (*dnx_field_action_sw_db_info_prefix_set_cb)(
    int unit, uint32 info_idx_0, uint32 prefix);


typedef int (*dnx_field_action_sw_db_info_prefix_get_cb)(
    int unit, uint32 info_idx_0, uint32 *prefix);


typedef int (*dnx_field_action_sw_db_info_prefix_size_set_cb)(
    int unit, uint32 info_idx_0, uint8 prefix_size);


typedef int (*dnx_field_action_sw_db_info_prefix_size_get_cb)(
    int unit, uint32 info_idx_0, uint8 *prefix_size);


typedef int (*dnx_field_action_sw_db_info_bcm_id_set_cb)(
    int unit, uint32 info_idx_0, int bcm_id);


typedef int (*dnx_field_action_sw_db_info_bcm_id_get_cb)(
    int unit, uint32 info_idx_0, int *bcm_id);


typedef int (*dnx_field_action_sw_db_info_ref_count_set_cb)(
    int unit, uint32 info_idx_0, uint32 ref_count);


typedef int (*dnx_field_action_sw_db_info_ref_count_get_cb)(
    int unit, uint32 info_idx_0, uint32 *ref_count);


typedef int (*dnx_field_action_sw_db_info_flags_set_cb)(
    int unit, uint32 info_idx_0, dnx_field_action_flags_e flags);


typedef int (*dnx_field_action_sw_db_info_flags_get_cb)(
    int unit, uint32 info_idx_0, dnx_field_action_flags_e *flags);


typedef int (*dnx_field_action_sw_db_info_base_dnx_action_set_cb)(
    int unit, uint32 info_idx_0, dnx_field_action_t base_dnx_action);


typedef int (*dnx_field_action_sw_db_info_base_dnx_action_get_cb)(
    int unit, uint32 info_idx_0, dnx_field_action_t *base_dnx_action);


typedef int (*dnx_field_action_sw_db_fem_info_alloc_cb)(
    int unit);


typedef int (*dnx_field_action_sw_db_fem_info_is_allocated_cb)(
    int unit, uint8 *is_allocated);


typedef int (*dnx_field_action_sw_db_fem_info_fg_id_info_alloc_cb)(
    int unit, uint32 fem_info_idx_0);


typedef int (*dnx_field_action_sw_db_fem_info_fg_id_info_is_allocated_cb)(
    int unit, uint32 fem_info_idx_0, uint8 *is_allocated);


typedef int (*dnx_field_action_sw_db_fem_info_fg_id_info_fg_id_set_cb)(
    int unit, uint32 fem_info_idx_0, uint32 fg_id_info_idx_0, dnx_field_group_t fg_id);


typedef int (*dnx_field_action_sw_db_fem_info_fg_id_info_fg_id_get_cb)(
    int unit, uint32 fem_info_idx_0, uint32 fg_id_info_idx_0, dnx_field_group_t *fg_id);


typedef int (*dnx_field_action_sw_db_fem_info_fg_id_info_input_offset_set_cb)(
    int unit, uint32 fem_info_idx_0, uint32 fg_id_info_idx_0, uint8 input_offset);


typedef int (*dnx_field_action_sw_db_fem_info_fg_id_info_input_offset_get_cb)(
    int unit, uint32 fem_info_idx_0, uint32 fg_id_info_idx_0, uint8 *input_offset);


typedef int (*dnx_field_action_sw_db_fem_info_fg_id_info_input_size_set_cb)(
    int unit, uint32 fem_info_idx_0, uint32 fg_id_info_idx_0, uint8 input_size);


typedef int (*dnx_field_action_sw_db_fem_info_fg_id_info_input_size_get_cb)(
    int unit, uint32 fem_info_idx_0, uint32 fg_id_info_idx_0, uint8 *input_size);


typedef int (*dnx_field_action_sw_db_fem_info_fg_id_info_second_fg_id_set_cb)(
    int unit, uint32 fem_info_idx_0, uint32 fg_id_info_idx_0, dnx_field_group_t second_fg_id);


typedef int (*dnx_field_action_sw_db_fem_info_fg_id_info_second_fg_id_get_cb)(
    int unit, uint32 fem_info_idx_0, uint32 fg_id_info_idx_0, dnx_field_group_t *second_fg_id);


typedef int (*dnx_field_action_sw_db_fem_info_fg_id_info_ignore_actions_set_cb)(
    int unit, uint32 fem_info_idx_0, uint32 fg_id_info_idx_0, uint8 ignore_actions);


typedef int (*dnx_field_action_sw_db_fem_info_fg_id_info_ignore_actions_get_cb)(
    int unit, uint32 fem_info_idx_0, uint32 fg_id_info_idx_0, uint8 *ignore_actions);


typedef int (*dnx_field_action_sw_db_fem_info_fg_id_info_fem_encoded_actions_set_cb)(
    int unit, uint32 fem_info_idx_0, uint32 fg_id_info_idx_0, uint32 fem_encoded_actions_idx_0, dnx_field_action_t fem_encoded_actions);


typedef int (*dnx_field_action_sw_db_fem_info_fg_id_info_fem_encoded_actions_get_cb)(
    int unit, uint32 fem_info_idx_0, uint32 fg_id_info_idx_0, uint32 fem_encoded_actions_idx_0, dnx_field_action_t *fem_encoded_actions);


typedef int (*dnx_field_action_sw_db_fem_info_fg_id_info_fem_encoded_actions_alloc_cb)(
    int unit, uint32 fem_info_idx_0, uint32 fg_id_info_idx_0);


typedef int (*dnx_field_action_sw_db_fem_info_fg_id_info_fem_encoded_actions_is_allocated_cb)(
    int unit, uint32 fem_info_idx_0, uint32 fg_id_info_idx_0, uint8 *is_allocated);




typedef struct {
    dnx_field_action_sw_db_info_valid_set_cb set;
    dnx_field_action_sw_db_info_valid_get_cb get;
} dnx_field_action_sw_db_info_valid_cbs;


typedef struct {
    dnx_field_action_sw_db_info_field_id_set_cb set;
    dnx_field_action_sw_db_info_field_id_get_cb get;
} dnx_field_action_sw_db_info_field_id_cbs;


typedef struct {
    dnx_field_action_sw_db_info_size_set_cb set;
    dnx_field_action_sw_db_info_size_get_cb get;
} dnx_field_action_sw_db_info_size_cbs;


typedef struct {
    dnx_field_action_sw_db_info_prefix_set_cb set;
    dnx_field_action_sw_db_info_prefix_get_cb get;
} dnx_field_action_sw_db_info_prefix_cbs;


typedef struct {
    dnx_field_action_sw_db_info_prefix_size_set_cb set;
    dnx_field_action_sw_db_info_prefix_size_get_cb get;
} dnx_field_action_sw_db_info_prefix_size_cbs;


typedef struct {
    dnx_field_action_sw_db_info_bcm_id_set_cb set;
    dnx_field_action_sw_db_info_bcm_id_get_cb get;
} dnx_field_action_sw_db_info_bcm_id_cbs;


typedef struct {
    dnx_field_action_sw_db_info_ref_count_set_cb set;
    dnx_field_action_sw_db_info_ref_count_get_cb get;
} dnx_field_action_sw_db_info_ref_count_cbs;


typedef struct {
    dnx_field_action_sw_db_info_flags_set_cb set;
    dnx_field_action_sw_db_info_flags_get_cb get;
} dnx_field_action_sw_db_info_flags_cbs;


typedef struct {
    dnx_field_action_sw_db_info_base_dnx_action_set_cb set;
    dnx_field_action_sw_db_info_base_dnx_action_get_cb get;
} dnx_field_action_sw_db_info_base_dnx_action_cbs;


typedef struct {
    dnx_field_action_sw_db_info_set_cb set;
    dnx_field_action_sw_db_info_get_cb get;
    
    dnx_field_action_sw_db_info_valid_cbs valid;
    
    dnx_field_action_sw_db_info_field_id_cbs field_id;
    
    dnx_field_action_sw_db_info_size_cbs size;
    
    dnx_field_action_sw_db_info_prefix_cbs prefix;
    
    dnx_field_action_sw_db_info_prefix_size_cbs prefix_size;
    
    dnx_field_action_sw_db_info_bcm_id_cbs bcm_id;
    
    dnx_field_action_sw_db_info_ref_count_cbs ref_count;
    
    dnx_field_action_sw_db_info_flags_cbs flags;
    
    dnx_field_action_sw_db_info_base_dnx_action_cbs base_dnx_action;
} dnx_field_action_sw_db_info_cbs;


typedef struct {
    dnx_field_action_sw_db_fem_info_fg_id_info_fg_id_set_cb set;
    dnx_field_action_sw_db_fem_info_fg_id_info_fg_id_get_cb get;
} dnx_field_action_sw_db_fem_info_fg_id_info_fg_id_cbs;


typedef struct {
    dnx_field_action_sw_db_fem_info_fg_id_info_input_offset_set_cb set;
    dnx_field_action_sw_db_fem_info_fg_id_info_input_offset_get_cb get;
} dnx_field_action_sw_db_fem_info_fg_id_info_input_offset_cbs;


typedef struct {
    dnx_field_action_sw_db_fem_info_fg_id_info_input_size_set_cb set;
    dnx_field_action_sw_db_fem_info_fg_id_info_input_size_get_cb get;
} dnx_field_action_sw_db_fem_info_fg_id_info_input_size_cbs;


typedef struct {
    dnx_field_action_sw_db_fem_info_fg_id_info_second_fg_id_set_cb set;
    dnx_field_action_sw_db_fem_info_fg_id_info_second_fg_id_get_cb get;
} dnx_field_action_sw_db_fem_info_fg_id_info_second_fg_id_cbs;


typedef struct {
    dnx_field_action_sw_db_fem_info_fg_id_info_ignore_actions_set_cb set;
    dnx_field_action_sw_db_fem_info_fg_id_info_ignore_actions_get_cb get;
} dnx_field_action_sw_db_fem_info_fg_id_info_ignore_actions_cbs;


typedef struct {
    dnx_field_action_sw_db_fem_info_fg_id_info_fem_encoded_actions_set_cb set;
    dnx_field_action_sw_db_fem_info_fg_id_info_fem_encoded_actions_get_cb get;
    dnx_field_action_sw_db_fem_info_fg_id_info_fem_encoded_actions_alloc_cb alloc;
    dnx_field_action_sw_db_fem_info_fg_id_info_fem_encoded_actions_is_allocated_cb is_allocated;
} dnx_field_action_sw_db_fem_info_fg_id_info_fem_encoded_actions_cbs;


typedef struct {
    dnx_field_action_sw_db_fem_info_fg_id_info_alloc_cb alloc;
    dnx_field_action_sw_db_fem_info_fg_id_info_is_allocated_cb is_allocated;
    
    dnx_field_action_sw_db_fem_info_fg_id_info_fg_id_cbs fg_id;
    
    dnx_field_action_sw_db_fem_info_fg_id_info_input_offset_cbs input_offset;
    
    dnx_field_action_sw_db_fem_info_fg_id_info_input_size_cbs input_size;
    
    dnx_field_action_sw_db_fem_info_fg_id_info_second_fg_id_cbs second_fg_id;
    
    dnx_field_action_sw_db_fem_info_fg_id_info_ignore_actions_cbs ignore_actions;
    
    dnx_field_action_sw_db_fem_info_fg_id_info_fem_encoded_actions_cbs fem_encoded_actions;
} dnx_field_action_sw_db_fem_info_fg_id_info_cbs;


typedef struct {
    dnx_field_action_sw_db_fem_info_alloc_cb alloc;
    dnx_field_action_sw_db_fem_info_is_allocated_cb is_allocated;
    
    dnx_field_action_sw_db_fem_info_fg_id_info_cbs fg_id_info;
} dnx_field_action_sw_db_fem_info_cbs;


typedef struct {
    dnx_field_action_sw_db_is_init_cb is_init;
    dnx_field_action_sw_db_init_cb init;
    
    dnx_field_action_sw_db_info_cbs info;
    
    dnx_field_action_sw_db_fem_info_cbs fem_info;
} dnx_field_action_sw_db_cbs;






const char *
dnx_field_action_flags_e_get_name(dnx_field_action_flags_e value);



const char *
dnx_field_fem_bit_format_e_get_name(dnx_field_fem_bit_format_e value);




extern dnx_field_action_sw_db_cbs dnx_field_action_sw_db;

#endif 
