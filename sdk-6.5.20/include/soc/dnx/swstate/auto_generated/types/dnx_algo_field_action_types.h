
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_ALGO_FIELD_ACTION_TYPES_H__
#define __DNX_ALGO_FIELD_ACTION_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_key_types.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_algo_field_types.h>



typedef struct {
    
    dnx_field_group_t field_group[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS];
    
    uint8 place_in_fg[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS];
    
    uint8 mask_is_alloc[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];
} dnx_algo_field_action_fes_state_t;


typedef struct {
    
    dnx_field_action_priority_t priority[DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT];
} dnx_algo_field_action_context_state_ipmf2_t;


typedef struct {
    
    dnx_field_action_priority_t priority[DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT];
} dnx_algo_field_action_context_state_ipmf3_t;


typedef struct {
    
    dnx_field_action_priority_t priority[DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT];
} dnx_algo_field_action_context_state_epmf_t;


typedef struct {
    
    dnx_algo_field_action_fes_state_t fes_state[DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT];
    
    dnx_algo_field_action_context_state_ipmf2_t context_state[DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS];
} dnx_algo_field_action_ipmf2_t;


typedef struct {
    
    dnx_algo_field_action_fes_state_t fes_state[DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT];
    
    dnx_algo_field_action_context_state_ipmf3_t context_state[DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_CONTEXTS];
} dnx_algo_field_action_ipmf3_t;


typedef struct {
    
    dnx_algo_field_action_fes_state_t fes_state[DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT];
    
    dnx_algo_field_action_context_state_epmf_t context_state[DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_CONTEXTS];
} dnx_algo_field_action_epmf_t;


typedef struct {
    
    dnx_algo_field_action_ipmf2_t ipmf2;
    
    dnx_algo_field_action_ipmf3_t ipmf3;
    
    dnx_algo_field_action_epmf_t epmf;
} dnx_algo_field_action_sw_t;


#endif 
