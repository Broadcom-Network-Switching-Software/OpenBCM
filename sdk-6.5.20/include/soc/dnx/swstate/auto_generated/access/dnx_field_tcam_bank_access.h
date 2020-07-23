
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_BANK_ACCESS_H__
#define __DNX_FIELD_TCAM_BANK_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_bank_types.h>
#include <soc/dnxc/swstate/callbacks/sw_state_ll_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_linked_list.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_access_types.h>



typedef int (*dnx_field_tcam_bank_sw_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_field_tcam_bank_sw_init_cb)(
    int unit);


typedef int (*dnx_field_tcam_bank_sw_tcam_banks_alloc_cb)(
    int unit);


typedef int (*dnx_field_tcam_bank_sw_tcam_banks_owner_stage_set_cb)(
    int unit, uint32 tcam_banks_idx_0, dnx_field_tcam_stage_e owner_stage);


typedef int (*dnx_field_tcam_bank_sw_tcam_banks_owner_stage_get_cb)(
    int unit, uint32 tcam_banks_idx_0, dnx_field_tcam_stage_e *owner_stage);


typedef int (*dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_create_empty_cb)(
    int unit, uint32 tcam_banks_idx_0, sw_state_ll_init_info_t *init_info);


typedef int (*dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_nof_elements_cb)(
    int unit, uint32 tcam_banks_idx_0, uint32 *nof_elements);


typedef int (*dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_node_value_cb)(
    int unit, uint32 tcam_banks_idx_0, sw_state_ll_node_t node, uint32 *value);


typedef int (*dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_node_update_cb)(
    int unit, uint32 tcam_banks_idx_0, sw_state_ll_node_t node, const uint32 *value);


typedef int (*dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_next_node_cb)(
    int unit, uint32 tcam_banks_idx_0, sw_state_ll_node_t node, sw_state_ll_node_t *next_node);


typedef int (*dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_previous_node_cb)(
    int unit, uint32 tcam_banks_idx_0, sw_state_ll_node_t node, sw_state_ll_node_t *prev_node);


typedef int (*dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_add_first_cb)(
    int unit, uint32 tcam_banks_idx_0, const uint32 *value);


typedef int (*dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_add_last_cb)(
    int unit, uint32 tcam_banks_idx_0, const uint32 *value);


typedef int (*dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_remove_node_cb)(
    int unit, uint32 tcam_banks_idx_0, sw_state_ll_node_t node);


typedef int (*dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_get_first_cb)(
    int unit, uint32 tcam_banks_idx_0, sw_state_ll_node_t *node);


typedef int (*dnx_field_tcam_bank_sw_tcam_banks_nof_free_entries_set_cb)(
    int unit, uint32 tcam_banks_idx_0, uint32 nof_free_entries_idx_0, uint32 nof_free_entries_idx_1, uint32 nof_free_entries);


typedef int (*dnx_field_tcam_bank_sw_tcam_banks_nof_free_entries_get_cb)(
    int unit, uint32 tcam_banks_idx_0, uint32 nof_free_entries_idx_0, uint32 nof_free_entries_idx_1, uint32 *nof_free_entries);


typedef int (*dnx_field_tcam_bank_sw_tcam_banks_nof_free_entries_alloc_cb)(
    int unit, uint32 tcam_banks_idx_0, uint32 nof_instances_to_alloc_0, uint32 nof_instances_to_alloc_1);


typedef int (*dnx_field_tcam_bank_sw_tcam_banks_bank_mode_set_cb)(
    int unit, uint32 tcam_banks_idx_0, dnx_field_tcam_bank_mode_e bank_mode);


typedef int (*dnx_field_tcam_bank_sw_tcam_banks_bank_mode_get_cb)(
    int unit, uint32 tcam_banks_idx_0, dnx_field_tcam_bank_mode_e *bank_mode);




typedef struct {
    dnx_field_tcam_bank_sw_tcam_banks_owner_stage_set_cb set;
    dnx_field_tcam_bank_sw_tcam_banks_owner_stage_get_cb get;
} dnx_field_tcam_bank_sw_tcam_banks_owner_stage_cbs;


typedef struct {
    dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_create_empty_cb create_empty;
    dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_nof_elements_cb nof_elements;
    dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_node_value_cb node_value;
    dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_node_update_cb node_update;
    dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_next_node_cb next_node;
    dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_previous_node_cb previous_node;
    dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_add_first_cb add_first;
    dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_add_last_cb add_last;
    dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_remove_node_cb remove_node;
    dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_get_first_cb get_first;
} dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_cbs;


typedef struct {
    dnx_field_tcam_bank_sw_tcam_banks_nof_free_entries_set_cb set;
    dnx_field_tcam_bank_sw_tcam_banks_nof_free_entries_get_cb get;
    dnx_field_tcam_bank_sw_tcam_banks_nof_free_entries_alloc_cb alloc;
} dnx_field_tcam_bank_sw_tcam_banks_nof_free_entries_cbs;


typedef struct {
    dnx_field_tcam_bank_sw_tcam_banks_bank_mode_set_cb set;
    dnx_field_tcam_bank_sw_tcam_banks_bank_mode_get_cb get;
} dnx_field_tcam_bank_sw_tcam_banks_bank_mode_cbs;


typedef struct {
    dnx_field_tcam_bank_sw_tcam_banks_alloc_cb alloc;
    
    dnx_field_tcam_bank_sw_tcam_banks_owner_stage_cbs owner_stage;
    
    dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_cbs active_handlers_id;
    
    dnx_field_tcam_bank_sw_tcam_banks_nof_free_entries_cbs nof_free_entries;
    
    dnx_field_tcam_bank_sw_tcam_banks_bank_mode_cbs bank_mode;
} dnx_field_tcam_bank_sw_tcam_banks_cbs;


typedef struct {
    dnx_field_tcam_bank_sw_is_init_cb is_init;
    dnx_field_tcam_bank_sw_init_cb init;
    
    dnx_field_tcam_bank_sw_tcam_banks_cbs tcam_banks;
} dnx_field_tcam_bank_sw_cbs;






const char *
dnx_field_tcam_bank_mode_e_get_name(dnx_field_tcam_bank_mode_e value);




extern dnx_field_tcam_bank_sw_cbs dnx_field_tcam_bank_sw;

#endif 
