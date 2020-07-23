
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __OAM_ACCESS_H__
#define __OAM_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/oam_types.h>
#include <include/bcm/oam.h>
#include <include/bcm/types.h>
#include <soc/dnxc/swstate/callbacks/sw_state_ll_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_linked_list.h>



typedef int (*oam_sw_db_info_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*oam_sw_db_info_init_cb)(
    int unit);


typedef int (*oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_create_empty_cb)(
    int unit, sw_state_ll_init_info_t *init_info);


typedef int (*oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_nof_elements_cb)(
    int unit, uint32 ll_head_index, uint32 *nof_elements);


typedef int (*oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_node_value_cb)(
    int unit, sw_state_ll_node_t node, bcm_oam_group_t *value);


typedef int (*oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_node_update_cb)(
    int unit, sw_state_ll_node_t node, const bcm_oam_group_t *value);


typedef int (*oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_next_node_cb)(
    int unit, uint32 ll_head_index, sw_state_ll_node_t node, sw_state_ll_node_t *next_node);


typedef int (*oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_previous_node_cb)(
    int unit, uint32 ll_head_index, sw_state_ll_node_t node, sw_state_ll_node_t *prev_node);


typedef int (*oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_add_first_cb)(
    int unit, uint32 ll_head_index, const bcm_oam_group_t *value);


typedef int (*oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_add_last_cb)(
    int unit, uint32 ll_head_index, const bcm_oam_group_t *value);


typedef int (*oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_remove_node_cb)(
    int unit, uint32 ll_head_index, sw_state_ll_node_t node);


typedef int (*oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_get_first_cb)(
    int unit, uint32 ll_head_index, sw_state_ll_node_t *node);


typedef int (*oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_create_empty_cb)(
    int unit, sw_state_ll_init_info_t *init_info);


typedef int (*oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_nof_elements_cb)(
    int unit, uint32 ll_head_index, uint32 *nof_elements);


typedef int (*oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_node_value_cb)(
    int unit, sw_state_ll_node_t node, bcm_oam_endpoint_t *value);


typedef int (*oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_node_update_cb)(
    int unit, sw_state_ll_node_t node, const bcm_oam_endpoint_t *value);


typedef int (*oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_next_node_cb)(
    int unit, uint32 ll_head_index, sw_state_ll_node_t node, sw_state_ll_node_t *next_node);


typedef int (*oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_previous_node_cb)(
    int unit, uint32 ll_head_index, sw_state_ll_node_t node, sw_state_ll_node_t *prev_node);


typedef int (*oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_add_first_cb)(
    int unit, uint32 ll_head_index, const bcm_oam_endpoint_t *value);


typedef int (*oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_add_last_cb)(
    int unit, uint32 ll_head_index, const bcm_oam_endpoint_t *value);


typedef int (*oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_remove_node_cb)(
    int unit, uint32 ll_head_index, sw_state_ll_node_t node);


typedef int (*oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_get_first_cb)(
    int unit, uint32 ll_head_index, sw_state_ll_node_t *node);


typedef int (*oam_sw_db_info_reflector_encap_id_set_cb)(
    int unit, int encap_id);


typedef int (*oam_sw_db_info_reflector_encap_id_get_cb)(
    int unit, int *encap_id);


typedef int (*oam_sw_db_info_reflector_is_allocated_set_cb)(
    int unit, int is_allocated);


typedef int (*oam_sw_db_info_reflector_is_allocated_get_cb)(
    int unit, int *is_allocated);


typedef int (*oam_sw_db_info_tst_trap_used_cnt_set_cb)(
    int unit, uint32 tst_trap_used_cnt);


typedef int (*oam_sw_db_info_tst_trap_used_cnt_get_cb)(
    int unit, uint32 *tst_trap_used_cnt);




typedef struct {
    oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_create_empty_cb create_empty;
    oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_nof_elements_cb nof_elements;
    oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_node_value_cb node_value;
    oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_node_update_cb node_update;
    oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_next_node_cb next_node;
    oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_previous_node_cb previous_node;
    oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_add_first_cb add_first;
    oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_add_last_cb add_last;
    oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_remove_node_cb remove_node;
    oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_get_first_cb get_first;
} oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_cbs;


typedef struct {
    
    oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_cbs oam_group_array_of_linked_lists;
} oam_sw_db_info_oam_group_sw_db_info_cbs;


typedef struct {
    oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_create_empty_cb create_empty;
    oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_nof_elements_cb nof_elements;
    oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_node_value_cb node_value;
    oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_node_update_cb node_update;
    oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_next_node_cb next_node;
    oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_previous_node_cb previous_node;
    oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_add_first_cb add_first;
    oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_add_last_cb add_last;
    oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_remove_node_cb remove_node;
    oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_get_first_cb get_first;
} oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_cbs;


typedef struct {
    
    oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_cbs oam_endpoint_array_of_rmep_linked_lists;
} oam_sw_db_info_oam_endpoint_sw_db_info_cbs;


typedef struct {
    oam_sw_db_info_reflector_encap_id_set_cb set;
    oam_sw_db_info_reflector_encap_id_get_cb get;
} oam_sw_db_info_reflector_encap_id_cbs;


typedef struct {
    oam_sw_db_info_reflector_is_allocated_set_cb set;
    oam_sw_db_info_reflector_is_allocated_get_cb get;
} oam_sw_db_info_reflector_is_allocated_cbs;


typedef struct {
    
    oam_sw_db_info_reflector_encap_id_cbs encap_id;
    
    oam_sw_db_info_reflector_is_allocated_cbs is_allocated;
} oam_sw_db_info_reflector_cbs;


typedef struct {
    oam_sw_db_info_tst_trap_used_cnt_set_cb set;
    oam_sw_db_info_tst_trap_used_cnt_get_cb get;
} oam_sw_db_info_tst_trap_used_cnt_cbs;


typedef struct {
    oam_sw_db_info_is_init_cb is_init;
    oam_sw_db_info_init_cb init;
    
    oam_sw_db_info_oam_group_sw_db_info_cbs oam_group_sw_db_info;
    
    oam_sw_db_info_oam_endpoint_sw_db_info_cbs oam_endpoint_sw_db_info;
    
    oam_sw_db_info_reflector_cbs reflector;
    
    oam_sw_db_info_tst_trap_used_cnt_cbs tst_trap_used_cnt;
} oam_sw_db_info_cbs;





extern oam_sw_db_info_cbs oam_sw_db_info;

#endif 
