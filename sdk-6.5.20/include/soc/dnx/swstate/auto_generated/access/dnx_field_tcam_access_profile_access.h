
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_ACCESS_PROFILE_ACCESS_H__
#define __DNX_FIELD_TCAM_ACCESS_PROFILE_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_access_profile_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>



typedef int (*dnx_field_tcam_access_profile_sw_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_field_tcam_access_profile_sw_init_cb)(
    int unit);


typedef int (*dnx_field_tcam_access_profile_sw_access_profiles_set_cb)(
    int unit, uint32 access_profiles_idx_0, CONST dnx_field_tcam_access_profile_t *access_profiles);


typedef int (*dnx_field_tcam_access_profile_sw_access_profiles_get_cb)(
    int unit, uint32 access_profiles_idx_0, dnx_field_tcam_access_profile_t *access_profiles);


typedef int (*dnx_field_tcam_access_profile_sw_access_profiles_alloc_cb)(
    int unit);


typedef int (*dnx_field_tcam_access_profile_sw_access_profiles_occupied_set_cb)(
    int unit, uint32 access_profiles_idx_0, uint8 occupied);


typedef int (*dnx_field_tcam_access_profile_sw_access_profiles_occupied_get_cb)(
    int unit, uint32 access_profiles_idx_0, uint8 *occupied);


typedef int (*dnx_field_tcam_access_profile_sw_access_profiles_key_size_set_cb)(
    int unit, uint32 access_profiles_idx_0, dnx_field_key_length_type_e key_size);


typedef int (*dnx_field_tcam_access_profile_sw_access_profiles_key_size_get_cb)(
    int unit, uint32 access_profiles_idx_0, dnx_field_key_length_type_e *key_size);


typedef int (*dnx_field_tcam_access_profile_sw_access_profiles_action_size_set_cb)(
    int unit, uint32 access_profiles_idx_0, dnx_field_action_length_type_e action_size);


typedef int (*dnx_field_tcam_access_profile_sw_access_profiles_action_size_get_cb)(
    int unit, uint32 access_profiles_idx_0, dnx_field_action_length_type_e *action_size);


typedef int (*dnx_field_tcam_access_profile_sw_access_profiles_bank_ids_bmp_set_cb)(
    int unit, uint32 access_profiles_idx_0, uint16 bank_ids_bmp);


typedef int (*dnx_field_tcam_access_profile_sw_access_profiles_bank_ids_bmp_get_cb)(
    int unit, uint32 access_profiles_idx_0, uint16 *bank_ids_bmp);


typedef int (*dnx_field_tcam_access_profile_sw_access_profiles_is_direct_set_cb)(
    int unit, uint32 access_profiles_idx_0, uint8 is_direct);


typedef int (*dnx_field_tcam_access_profile_sw_access_profiles_is_direct_get_cb)(
    int unit, uint32 access_profiles_idx_0, uint8 *is_direct);


typedef int (*dnx_field_tcam_access_profile_sw_access_profiles_prefix_set_cb)(
    int unit, uint32 access_profiles_idx_0, uint8 prefix);


typedef int (*dnx_field_tcam_access_profile_sw_access_profiles_prefix_get_cb)(
    int unit, uint32 access_profiles_idx_0, uint8 *prefix);


typedef int (*dnx_field_tcam_access_profile_sw_access_profiles_handler_id_set_cb)(
    int unit, uint32 access_profiles_idx_0, uint32 handler_id);


typedef int (*dnx_field_tcam_access_profile_sw_access_profiles_handler_id_get_cb)(
    int unit, uint32 access_profiles_idx_0, uint32 *handler_id);




typedef struct {
    dnx_field_tcam_access_profile_sw_access_profiles_occupied_set_cb set;
    dnx_field_tcam_access_profile_sw_access_profiles_occupied_get_cb get;
} dnx_field_tcam_access_profile_sw_access_profiles_occupied_cbs;


typedef struct {
    dnx_field_tcam_access_profile_sw_access_profiles_key_size_set_cb set;
    dnx_field_tcam_access_profile_sw_access_profiles_key_size_get_cb get;
} dnx_field_tcam_access_profile_sw_access_profiles_key_size_cbs;


typedef struct {
    dnx_field_tcam_access_profile_sw_access_profiles_action_size_set_cb set;
    dnx_field_tcam_access_profile_sw_access_profiles_action_size_get_cb get;
} dnx_field_tcam_access_profile_sw_access_profiles_action_size_cbs;


typedef struct {
    dnx_field_tcam_access_profile_sw_access_profiles_bank_ids_bmp_set_cb set;
    dnx_field_tcam_access_profile_sw_access_profiles_bank_ids_bmp_get_cb get;
} dnx_field_tcam_access_profile_sw_access_profiles_bank_ids_bmp_cbs;


typedef struct {
    dnx_field_tcam_access_profile_sw_access_profiles_is_direct_set_cb set;
    dnx_field_tcam_access_profile_sw_access_profiles_is_direct_get_cb get;
} dnx_field_tcam_access_profile_sw_access_profiles_is_direct_cbs;


typedef struct {
    dnx_field_tcam_access_profile_sw_access_profiles_prefix_set_cb set;
    dnx_field_tcam_access_profile_sw_access_profiles_prefix_get_cb get;
} dnx_field_tcam_access_profile_sw_access_profiles_prefix_cbs;


typedef struct {
    dnx_field_tcam_access_profile_sw_access_profiles_handler_id_set_cb set;
    dnx_field_tcam_access_profile_sw_access_profiles_handler_id_get_cb get;
} dnx_field_tcam_access_profile_sw_access_profiles_handler_id_cbs;


typedef struct {
    dnx_field_tcam_access_profile_sw_access_profiles_set_cb set;
    dnx_field_tcam_access_profile_sw_access_profiles_get_cb get;
    dnx_field_tcam_access_profile_sw_access_profiles_alloc_cb alloc;
    
    dnx_field_tcam_access_profile_sw_access_profiles_occupied_cbs occupied;
    
    dnx_field_tcam_access_profile_sw_access_profiles_key_size_cbs key_size;
    
    dnx_field_tcam_access_profile_sw_access_profiles_action_size_cbs action_size;
    
    dnx_field_tcam_access_profile_sw_access_profiles_bank_ids_bmp_cbs bank_ids_bmp;
    
    dnx_field_tcam_access_profile_sw_access_profiles_is_direct_cbs is_direct;
    
    dnx_field_tcam_access_profile_sw_access_profiles_prefix_cbs prefix;
    
    dnx_field_tcam_access_profile_sw_access_profiles_handler_id_cbs handler_id;
} dnx_field_tcam_access_profile_sw_access_profiles_cbs;


typedef struct {
    dnx_field_tcam_access_profile_sw_is_init_cb is_init;
    dnx_field_tcam_access_profile_sw_init_cb init;
    
    dnx_field_tcam_access_profile_sw_access_profiles_cbs access_profiles;
} dnx_field_tcam_access_profile_sw_cbs;





extern dnx_field_tcam_access_profile_sw_cbs dnx_field_tcam_access_profile_sw;

#endif 
