
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_HIT_INDICATION_ACCESS_H__
#define __DNX_FIELD_TCAM_HIT_INDICATION_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_hit_indication_types.h>



typedef int (*dnx_field_tcam_hit_indication_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_field_tcam_hit_indication_init_cb)(
    int unit);


typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_alloc_cb)(
    int unit);


typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_bank_line_id_set_cb)(
    int unit, uint32 tcam_action_hit_indication_idx_0, uint32 bank_line_id_idx_0, uint32 bank_line_id_idx_1, uint8 bank_line_id);


typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_bank_line_id_get_cb)(
    int unit, uint32 tcam_action_hit_indication_idx_0, uint32 bank_line_id_idx_0, uint32 bank_line_id_idx_1, uint8 *bank_line_id);


typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_bank_line_id_alloc_cb)(
    int unit, uint32 tcam_action_hit_indication_idx_0);


typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_alloc_cb)(
    int unit);


typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_bank_line_id_set_cb)(
    int unit, uint32 tcam_action_hit_indication_small_idx_0, uint32 bank_line_id_idx_0, uint32 bank_line_id_idx_1, uint8 bank_line_id);


typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_bank_line_id_get_cb)(
    int unit, uint32 tcam_action_hit_indication_small_idx_0, uint32 bank_line_id_idx_0, uint32 bank_line_id_idx_1, uint8 *bank_line_id);


typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_bank_line_id_alloc_cb)(
    int unit, uint32 tcam_action_hit_indication_small_idx_0);




typedef struct {
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_bank_line_id_set_cb set;
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_bank_line_id_get_cb get;
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_bank_line_id_alloc_cb alloc;
} dnx_field_tcam_hit_indication_tcam_action_hit_indication_bank_line_id_cbs;


typedef struct {
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_alloc_cb alloc;
    
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_bank_line_id_cbs bank_line_id;
} dnx_field_tcam_hit_indication_tcam_action_hit_indication_cbs;


typedef struct {
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_bank_line_id_set_cb set;
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_bank_line_id_get_cb get;
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_bank_line_id_alloc_cb alloc;
} dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_bank_line_id_cbs;


typedef struct {
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_alloc_cb alloc;
    
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_bank_line_id_cbs bank_line_id;
} dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_cbs;


typedef struct {
    dnx_field_tcam_hit_indication_is_init_cb is_init;
    dnx_field_tcam_hit_indication_init_cb init;
    
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_cbs tcam_action_hit_indication;
    
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_cbs tcam_action_hit_indication_small;
} dnx_field_tcam_hit_indication_cbs;





extern dnx_field_tcam_hit_indication_cbs dnx_field_tcam_hit_indication;

#endif 
