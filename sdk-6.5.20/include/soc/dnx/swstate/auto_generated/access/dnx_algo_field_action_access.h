
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_ALGO_FIELD_ACTION_ACCESS_H__
#define __DNX_ALGO_FIELD_ACTION_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_algo_field_action_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_key_types.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_algo_field_types.h>



typedef int (*dnx_algo_field_action_sw_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_algo_field_action_sw_init_cb)(
    int unit);


typedef int (*dnx_algo_field_action_sw_ipmf2_set_cb)(
    int unit, CONST dnx_algo_field_action_ipmf2_t *ipmf2);


typedef int (*dnx_algo_field_action_sw_ipmf2_get_cb)(
    int unit, dnx_algo_field_action_ipmf2_t *ipmf2);


typedef int (*dnx_algo_field_action_sw_ipmf2_fes_state_set_cb)(
    int unit, uint32 fes_state_idx_0, CONST dnx_algo_field_action_fes_state_t *fes_state);


typedef int (*dnx_algo_field_action_sw_ipmf2_fes_state_get_cb)(
    int unit, uint32 fes_state_idx_0, dnx_algo_field_action_fes_state_t *fes_state);


typedef int (*dnx_algo_field_action_sw_ipmf2_fes_state_field_group_set_cb)(
    int unit, uint32 fes_state_idx_0, uint32 field_group_idx_0, dnx_field_group_t field_group);


typedef int (*dnx_algo_field_action_sw_ipmf2_fes_state_field_group_get_cb)(
    int unit, uint32 fes_state_idx_0, uint32 field_group_idx_0, dnx_field_group_t *field_group);


typedef int (*dnx_algo_field_action_sw_ipmf2_fes_state_place_in_fg_set_cb)(
    int unit, uint32 fes_state_idx_0, uint32 place_in_fg_idx_0, uint8 place_in_fg);


typedef int (*dnx_algo_field_action_sw_ipmf2_fes_state_place_in_fg_get_cb)(
    int unit, uint32 fes_state_idx_0, uint32 place_in_fg_idx_0, uint8 *place_in_fg);


typedef int (*dnx_algo_field_action_sw_ipmf2_fes_state_mask_is_alloc_set_cb)(
    int unit, uint32 fes_state_idx_0, uint32 mask_is_alloc_idx_0, uint8 mask_is_alloc);


typedef int (*dnx_algo_field_action_sw_ipmf2_fes_state_mask_is_alloc_get_cb)(
    int unit, uint32 fes_state_idx_0, uint32 mask_is_alloc_idx_0, uint8 *mask_is_alloc);


typedef int (*dnx_algo_field_action_sw_ipmf2_context_state_priority_set_cb)(
    int unit, uint32 context_state_idx_0, uint32 priority_idx_0, dnx_field_action_priority_t priority);


typedef int (*dnx_algo_field_action_sw_ipmf2_context_state_priority_get_cb)(
    int unit, uint32 context_state_idx_0, uint32 priority_idx_0, dnx_field_action_priority_t *priority);


typedef int (*dnx_algo_field_action_sw_ipmf3_set_cb)(
    int unit, CONST dnx_algo_field_action_ipmf3_t *ipmf3);


typedef int (*dnx_algo_field_action_sw_ipmf3_get_cb)(
    int unit, dnx_algo_field_action_ipmf3_t *ipmf3);


typedef int (*dnx_algo_field_action_sw_ipmf3_fes_state_set_cb)(
    int unit, uint32 fes_state_idx_0, CONST dnx_algo_field_action_fes_state_t *fes_state);


typedef int (*dnx_algo_field_action_sw_ipmf3_fes_state_get_cb)(
    int unit, uint32 fes_state_idx_0, dnx_algo_field_action_fes_state_t *fes_state);


typedef int (*dnx_algo_field_action_sw_ipmf3_fes_state_field_group_set_cb)(
    int unit, uint32 fes_state_idx_0, uint32 field_group_idx_0, dnx_field_group_t field_group);


typedef int (*dnx_algo_field_action_sw_ipmf3_fes_state_field_group_get_cb)(
    int unit, uint32 fes_state_idx_0, uint32 field_group_idx_0, dnx_field_group_t *field_group);


typedef int (*dnx_algo_field_action_sw_ipmf3_fes_state_place_in_fg_set_cb)(
    int unit, uint32 fes_state_idx_0, uint32 place_in_fg_idx_0, uint8 place_in_fg);


typedef int (*dnx_algo_field_action_sw_ipmf3_fes_state_place_in_fg_get_cb)(
    int unit, uint32 fes_state_idx_0, uint32 place_in_fg_idx_0, uint8 *place_in_fg);


typedef int (*dnx_algo_field_action_sw_ipmf3_fes_state_mask_is_alloc_set_cb)(
    int unit, uint32 fes_state_idx_0, uint32 mask_is_alloc_idx_0, uint8 mask_is_alloc);


typedef int (*dnx_algo_field_action_sw_ipmf3_fes_state_mask_is_alloc_get_cb)(
    int unit, uint32 fes_state_idx_0, uint32 mask_is_alloc_idx_0, uint8 *mask_is_alloc);


typedef int (*dnx_algo_field_action_sw_ipmf3_context_state_priority_set_cb)(
    int unit, uint32 context_state_idx_0, uint32 priority_idx_0, dnx_field_action_priority_t priority);


typedef int (*dnx_algo_field_action_sw_ipmf3_context_state_priority_get_cb)(
    int unit, uint32 context_state_idx_0, uint32 priority_idx_0, dnx_field_action_priority_t *priority);


typedef int (*dnx_algo_field_action_sw_epmf_set_cb)(
    int unit, CONST dnx_algo_field_action_epmf_t *epmf);


typedef int (*dnx_algo_field_action_sw_epmf_get_cb)(
    int unit, dnx_algo_field_action_epmf_t *epmf);


typedef int (*dnx_algo_field_action_sw_epmf_fes_state_set_cb)(
    int unit, uint32 fes_state_idx_0, CONST dnx_algo_field_action_fes_state_t *fes_state);


typedef int (*dnx_algo_field_action_sw_epmf_fes_state_get_cb)(
    int unit, uint32 fes_state_idx_0, dnx_algo_field_action_fes_state_t *fes_state);


typedef int (*dnx_algo_field_action_sw_epmf_fes_state_field_group_set_cb)(
    int unit, uint32 fes_state_idx_0, uint32 field_group_idx_0, dnx_field_group_t field_group);


typedef int (*dnx_algo_field_action_sw_epmf_fes_state_field_group_get_cb)(
    int unit, uint32 fes_state_idx_0, uint32 field_group_idx_0, dnx_field_group_t *field_group);


typedef int (*dnx_algo_field_action_sw_epmf_fes_state_place_in_fg_set_cb)(
    int unit, uint32 fes_state_idx_0, uint32 place_in_fg_idx_0, uint8 place_in_fg);


typedef int (*dnx_algo_field_action_sw_epmf_fes_state_place_in_fg_get_cb)(
    int unit, uint32 fes_state_idx_0, uint32 place_in_fg_idx_0, uint8 *place_in_fg);


typedef int (*dnx_algo_field_action_sw_epmf_fes_state_mask_is_alloc_set_cb)(
    int unit, uint32 fes_state_idx_0, uint32 mask_is_alloc_idx_0, uint8 mask_is_alloc);


typedef int (*dnx_algo_field_action_sw_epmf_fes_state_mask_is_alloc_get_cb)(
    int unit, uint32 fes_state_idx_0, uint32 mask_is_alloc_idx_0, uint8 *mask_is_alloc);


typedef int (*dnx_algo_field_action_sw_epmf_context_state_priority_set_cb)(
    int unit, uint32 context_state_idx_0, uint32 priority_idx_0, dnx_field_action_priority_t priority);


typedef int (*dnx_algo_field_action_sw_epmf_context_state_priority_get_cb)(
    int unit, uint32 context_state_idx_0, uint32 priority_idx_0, dnx_field_action_priority_t *priority);




typedef struct {
    dnx_algo_field_action_sw_ipmf2_fes_state_field_group_set_cb set;
    dnx_algo_field_action_sw_ipmf2_fes_state_field_group_get_cb get;
} dnx_algo_field_action_sw_ipmf2_fes_state_field_group_cbs;


typedef struct {
    dnx_algo_field_action_sw_ipmf2_fes_state_place_in_fg_set_cb set;
    dnx_algo_field_action_sw_ipmf2_fes_state_place_in_fg_get_cb get;
} dnx_algo_field_action_sw_ipmf2_fes_state_place_in_fg_cbs;


typedef struct {
    dnx_algo_field_action_sw_ipmf2_fes_state_mask_is_alloc_set_cb set;
    dnx_algo_field_action_sw_ipmf2_fes_state_mask_is_alloc_get_cb get;
} dnx_algo_field_action_sw_ipmf2_fes_state_mask_is_alloc_cbs;


typedef struct {
    dnx_algo_field_action_sw_ipmf2_fes_state_set_cb set;
    dnx_algo_field_action_sw_ipmf2_fes_state_get_cb get;
    
    dnx_algo_field_action_sw_ipmf2_fes_state_field_group_cbs field_group;
    
    dnx_algo_field_action_sw_ipmf2_fes_state_place_in_fg_cbs place_in_fg;
    
    dnx_algo_field_action_sw_ipmf2_fes_state_mask_is_alloc_cbs mask_is_alloc;
} dnx_algo_field_action_sw_ipmf2_fes_state_cbs;


typedef struct {
    dnx_algo_field_action_sw_ipmf2_context_state_priority_set_cb set;
    dnx_algo_field_action_sw_ipmf2_context_state_priority_get_cb get;
} dnx_algo_field_action_sw_ipmf2_context_state_priority_cbs;


typedef struct {
    
    dnx_algo_field_action_sw_ipmf2_context_state_priority_cbs priority;
} dnx_algo_field_action_sw_ipmf2_context_state_cbs;


typedef struct {
    dnx_algo_field_action_sw_ipmf2_set_cb set;
    dnx_algo_field_action_sw_ipmf2_get_cb get;
    
    dnx_algo_field_action_sw_ipmf2_fes_state_cbs fes_state;
    
    dnx_algo_field_action_sw_ipmf2_context_state_cbs context_state;
} dnx_algo_field_action_sw_ipmf2_cbs;


typedef struct {
    dnx_algo_field_action_sw_ipmf3_fes_state_field_group_set_cb set;
    dnx_algo_field_action_sw_ipmf3_fes_state_field_group_get_cb get;
} dnx_algo_field_action_sw_ipmf3_fes_state_field_group_cbs;


typedef struct {
    dnx_algo_field_action_sw_ipmf3_fes_state_place_in_fg_set_cb set;
    dnx_algo_field_action_sw_ipmf3_fes_state_place_in_fg_get_cb get;
} dnx_algo_field_action_sw_ipmf3_fes_state_place_in_fg_cbs;


typedef struct {
    dnx_algo_field_action_sw_ipmf3_fes_state_mask_is_alloc_set_cb set;
    dnx_algo_field_action_sw_ipmf3_fes_state_mask_is_alloc_get_cb get;
} dnx_algo_field_action_sw_ipmf3_fes_state_mask_is_alloc_cbs;


typedef struct {
    dnx_algo_field_action_sw_ipmf3_fes_state_set_cb set;
    dnx_algo_field_action_sw_ipmf3_fes_state_get_cb get;
    
    dnx_algo_field_action_sw_ipmf3_fes_state_field_group_cbs field_group;
    
    dnx_algo_field_action_sw_ipmf3_fes_state_place_in_fg_cbs place_in_fg;
    
    dnx_algo_field_action_sw_ipmf3_fes_state_mask_is_alloc_cbs mask_is_alloc;
} dnx_algo_field_action_sw_ipmf3_fes_state_cbs;


typedef struct {
    dnx_algo_field_action_sw_ipmf3_context_state_priority_set_cb set;
    dnx_algo_field_action_sw_ipmf3_context_state_priority_get_cb get;
} dnx_algo_field_action_sw_ipmf3_context_state_priority_cbs;


typedef struct {
    
    dnx_algo_field_action_sw_ipmf3_context_state_priority_cbs priority;
} dnx_algo_field_action_sw_ipmf3_context_state_cbs;


typedef struct {
    dnx_algo_field_action_sw_ipmf3_set_cb set;
    dnx_algo_field_action_sw_ipmf3_get_cb get;
    
    dnx_algo_field_action_sw_ipmf3_fes_state_cbs fes_state;
    
    dnx_algo_field_action_sw_ipmf3_context_state_cbs context_state;
} dnx_algo_field_action_sw_ipmf3_cbs;


typedef struct {
    dnx_algo_field_action_sw_epmf_fes_state_field_group_set_cb set;
    dnx_algo_field_action_sw_epmf_fes_state_field_group_get_cb get;
} dnx_algo_field_action_sw_epmf_fes_state_field_group_cbs;


typedef struct {
    dnx_algo_field_action_sw_epmf_fes_state_place_in_fg_set_cb set;
    dnx_algo_field_action_sw_epmf_fes_state_place_in_fg_get_cb get;
} dnx_algo_field_action_sw_epmf_fes_state_place_in_fg_cbs;


typedef struct {
    dnx_algo_field_action_sw_epmf_fes_state_mask_is_alloc_set_cb set;
    dnx_algo_field_action_sw_epmf_fes_state_mask_is_alloc_get_cb get;
} dnx_algo_field_action_sw_epmf_fes_state_mask_is_alloc_cbs;


typedef struct {
    dnx_algo_field_action_sw_epmf_fes_state_set_cb set;
    dnx_algo_field_action_sw_epmf_fes_state_get_cb get;
    
    dnx_algo_field_action_sw_epmf_fes_state_field_group_cbs field_group;
    
    dnx_algo_field_action_sw_epmf_fes_state_place_in_fg_cbs place_in_fg;
    
    dnx_algo_field_action_sw_epmf_fes_state_mask_is_alloc_cbs mask_is_alloc;
} dnx_algo_field_action_sw_epmf_fes_state_cbs;


typedef struct {
    dnx_algo_field_action_sw_epmf_context_state_priority_set_cb set;
    dnx_algo_field_action_sw_epmf_context_state_priority_get_cb get;
} dnx_algo_field_action_sw_epmf_context_state_priority_cbs;


typedef struct {
    
    dnx_algo_field_action_sw_epmf_context_state_priority_cbs priority;
} dnx_algo_field_action_sw_epmf_context_state_cbs;


typedef struct {
    dnx_algo_field_action_sw_epmf_set_cb set;
    dnx_algo_field_action_sw_epmf_get_cb get;
    
    dnx_algo_field_action_sw_epmf_fes_state_cbs fes_state;
    
    dnx_algo_field_action_sw_epmf_context_state_cbs context_state;
} dnx_algo_field_action_sw_epmf_cbs;


typedef struct {
    dnx_algo_field_action_sw_is_init_cb is_init;
    dnx_algo_field_action_sw_init_cb init;
    
    dnx_algo_field_action_sw_ipmf2_cbs ipmf2;
    
    dnx_algo_field_action_sw_ipmf3_cbs ipmf3;
    
    dnx_algo_field_action_sw_epmf_cbs epmf;
} dnx_algo_field_action_sw_cbs;





extern dnx_algo_field_action_sw_cbs dnx_algo_field_action_sw;

#endif 
