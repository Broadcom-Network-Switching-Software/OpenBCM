
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __MULTITHREAD_ANALYZER_ACCESS_H__
#define __MULTITHREAD_ANALYZER_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/auto_generated/types/multithread_analyzer_types.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>



typedef int (*multithread_analyzer_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*multithread_analyzer_init_cb)(
    int unit);


typedef int (*multithread_analyzer_deinit_cb)(
    int unit);


typedef int (*multithread_analyzer_htb_create_cb)(
    int unit, sw_state_htbl_init_info_t *init_info);


typedef int (*multithread_analyzer_htb_destroy_cb)(
    int unit);


typedef int (*multithread_analyzer_htb_find_cb)(
    int unit, const thread_access_log_entry_t *key, uint32 *data_index, uint8 *found);


typedef int (*multithread_analyzer_htb_insert_cb)(
    int unit, const thread_access_log_entry_t *key, const uint32 *data_index, uint8 *success);


typedef int (*multithread_analyzer_htb_get_next_cb)(
    int unit, SW_STATE_HASH_TABLE_ITER *iter, const thread_access_log_entry_t *key, const uint32 *data_index);


typedef int (*multithread_analyzer_htb_clear_cb)(
    int unit);


typedef int (*multithread_analyzer_htb_delete_cb)(
    int unit, const thread_access_log_entry_t *key);


typedef int (*multithread_analyzer_htb_insert_at_index_cb)(
    int unit, const thread_access_log_entry_t *key, uint32 data_idx, uint8 *success);


typedef int (*multithread_analyzer_htb_delete_by_index_cb)(
    int unit, uint32 data_index);


typedef int (*multithread_analyzer_htb_get_by_index_cb)(
    int unit, uint32 data_index, thread_access_log_entry_t *key, uint8 *found);




typedef struct {
    multithread_analyzer_htb_create_cb create;
    multithread_analyzer_htb_destroy_cb destroy;
    multithread_analyzer_htb_find_cb find;
    multithread_analyzer_htb_insert_cb insert;
    multithread_analyzer_htb_get_next_cb get_next;
    multithread_analyzer_htb_clear_cb clear;
    multithread_analyzer_htb_delete_cb delete;
    multithread_analyzer_htb_insert_at_index_cb insert_at_index;
    multithread_analyzer_htb_delete_by_index_cb delete_by_index;
    multithread_analyzer_htb_get_by_index_cb get_by_index;
} multithread_analyzer_htb_cbs;


typedef struct {
    multithread_analyzer_is_init_cb is_init;
    multithread_analyzer_init_cb init;
    multithread_analyzer_deinit_cb deinit;
    
    multithread_analyzer_htb_cbs htb;
} multithread_analyzer_cbs;





extern multithread_analyzer_cbs multithread_analyzer;

#endif 
