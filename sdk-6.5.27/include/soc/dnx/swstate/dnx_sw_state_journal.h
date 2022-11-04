/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * INFO: This module is one of the components needed for Error Recovery,
 * it is a rollback journal, saving the old values that were overridden
 * by sw_state access calls since the beginning of the last transaction.
 */

#ifndef _DNX_SW_STATE_JOURNAL_H

#define _DNX_SW_STATE_JOURNAL_H

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <soc/dnx/recovery/rollback_journal.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>
#include <soc/dnxc/swstate/types/sw_state_index_hash_table.h>
#include <soc/dnxc/swstate/types/sw_state_htb.h>
#include <soc/dnx/dnx_er_db.h>

#define DNX_SW_STATE_JOURNAL_EXCLUDED_STAMPS_MAX_NOF 10

#define DNX_SW_STATE_JOURNAL_MAX_STAMP_LENGTH 512

typedef void (
    *dnx_sw_state_journal_access_cb) (
    int unit,
    dnx_er_db_t * dnx_er_db);

typedef struct dnx_sw_state_journal_transaction_d
{
    uint32 excluded_stamps_count;
    char excluded_stamps[DNX_SW_STATE_JOURNAL_EXCLUDED_STAMPS_MAX_NOF][DNX_SW_STATE_JOURNAL_MAX_STAMP_LENGTH];
} dnx_sw_state_journal_transaction_t;

typedef struct dnx_sw_state_journal_d
{
    dnx_sw_state_journal_transaction_t transaction;
    dnx_sw_state_journal_access_cb access_cb;
} dnx_sw_state_journal_t;

typedef struct dnx_sw_state_journal_manager_d
{
    int dnx_sw_state_journal_dummy;
    dnx_sw_state_journal_t journals[2];
} dnx_sw_state_journal_manager_t;

typedef enum dnx_sw_state_journal_entry_e
{
    SW_JOURNAL_ENTRY_MEMCPY = 0,
    SW_JOURNAL_ENTRY_ALLOC,
    SW_JOURNAL_ENTRY_FREE,
    SW_JOURNAL_ENTRY_MUTEX_CREATE,
    SW_JOURNAL_ENTRY_MUTEX_DESTROY,
    SW_JOURNAL_ENTRY_MUTEX_TAKE,
    SW_JOURNAL_ENTRY_MUTEX_GIVE,
    SW_JOURNAL_ENTRY_SEMAPHORE_CREATE,
    SW_JOURNAL_ENTRY_SEMAPHORE_DESTROY,
    SW_JOURNAL_ENTRY_SEMAPHORE_TAKE,
    SW_JOURNAL_ENTRY_SEMAPHORE_GIVE,
    SW_JOURNAL_ENTRY_HTBL_ENTRY,
    SW_JOURNAL_ENTRY_INDEX_HTBL_ENTRY,
    SW_JOURNAL_ENTRY_RH_HTBL_ENTRY,
    SW_JOURNAL_ENTRY_TYPE_COUNT
} dnx_sw_state_journal_entry_type_t;

typedef struct dnx_sw_state_journal_memcpy_entry_d
{
    sal_vaddr_t offset;
} dnx_sw_state_journal_memcpy_entry_t;

typedef struct dnx_sw_state_journal_alloc_entry_d
{
    sal_vaddr_t location_offset;
    sal_vaddr_t ptr_location_offset;
} dnx_sw_state_journal_alloc_entry_t;

typedef struct dnx_sw_state_journal_free_entry_d
{
    sal_vaddr_t location_offset;
    sal_vaddr_t ptr_location_offset;
} dnx_sw_state_journal_free_entry_t;

typedef struct dnx_sw_state_journal_mutex_create_entry_d
{
    sal_vaddr_t mtx_offset;
} dnx_sw_state_journal_mutex_create_entry_t;

typedef struct dnx_sw_state_journal_mutex_destroy_entry_d
{
    sal_vaddr_t mtx_offset;
} dnx_sw_state_journal_mutex_destroy_entry_t;

typedef struct dnx_sw_state_journal_mutex_take_entry_d
{
    sal_vaddr_t mtx_offset;
    uint32 usec;
} dnx_sw_state_journal_mutex_take_entry_t;

typedef struct dnx_sw_state_journal_mutex_give_entry_d
{
    sal_vaddr_t mtx_offset;
} dnx_sw_state_journal_mutex_give_entry_t;

typedef struct dnx_sw_state_journal_sem_create_entry_d
{
    sal_vaddr_t sem_offset;
} dnx_sw_state_journal_sem_create_entry_t;

typedef struct dnx_sw_state_journal_sem_destroy_entry_d
{
    sal_vaddr_t sem_offset;
    int is_binary;
    int initial_count;
} dnx_sw_state_journal_sem_destroy_entry_t;

typedef struct dnx_sw_state_journal_sem_take_entry_d
{
    sal_vaddr_t sem_offset;
    uint32 usec;
} dnx_sw_state_journal_sem_take_entry_t;

typedef struct dnx_sw_state_journal_sem_give_entry_d
{
    sal_vaddr_t sem_offset;
} dnx_sw_state_journal_sem_give_entry_t;

typedef struct dnx_sw_state_journal_htbl_entry_d
{

    sal_vaddr_t hash_table_offset;
} dnx_sw_state_journal_htbl_entry_t;

typedef union dnx_sw_state_journal_entry_data_d
{
    dnx_sw_state_journal_memcpy_entry_t memcpy_data;
    dnx_sw_state_journal_alloc_entry_t alloc_data;
    dnx_sw_state_journal_free_entry_t free_data;
    dnx_sw_state_journal_mutex_create_entry_t mutex_create;
    dnx_sw_state_journal_mutex_destroy_entry_t mutex_destroy;
    dnx_sw_state_journal_mutex_take_entry_t mutex_take;
    dnx_sw_state_journal_mutex_give_entry_t mutex_give;
    dnx_sw_state_journal_sem_create_entry_t sem_create;
    dnx_sw_state_journal_sem_destroy_entry_t sem_destroy;
    dnx_sw_state_journal_sem_take_entry_t sem_take;
    dnx_sw_state_journal_sem_give_entry_t sem_give;
    dnx_sw_state_journal_htbl_entry_t htbl_data;
} dnx_sw_state_journal_entry_data_t;

typedef struct dnx_sw_state_journal_entry_d
{
    dnx_sw_state_journal_entry_type_t entry_type;
    uint32 nof_elements;
    uint32 element_size;
    uint32 node_id;
    uint8 multi_head_idx;
    dnx_sw_state_journal_entry_data_t data;
} dnx_sw_state_journal_entry_t;

uint8 dnx_sw_state_journal_is_done_init(
    int unit);

shr_error_e dnx_sw_state_journal_initialize(
    int unit,
    dnx_er_db_t * dnx_er_db,
    dnx_rollback_journal_cbs_t rollback_journal_cbs,
    dnx_sw_state_journal_access_cb access_cb,
    uint8 is_comparison);

shr_error_e dnx_sw_state_journal_destroy(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 is_comparison);

shr_error_e dnx_sw_state_journal_roll_back(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 only_head_rollback,
    uint8 is_comparison);

shr_error_e dnx_sw_state_journal_clear(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 is_comparison);

shr_error_e dnx_sw_state_journal_log_free(
    int unit,
    uint32 node_id,
    uint32 nof_elements,
    uint32 element_size,
    uint8 **ptr);

shr_error_e dnx_sw_state_journal_log_alloc(
    int unit,
    uint32 node_id,
    uint8 **ptr);

shr_error_e dnx_sw_state_journal_log_memcpy(
    int unit,
    uint32 node_id,
    uint32 size,
    uint8 *ptr);

shr_error_e dnx_sw_state_journal_log_htbl(
    int unit,
    uint32 node_id,
    sw_state_htbl_t hash_table,
    SW_STATE_HASH_TABLE_KEY * const key);

shr_error_e dnx_sw_state_journal_log_idx_htbl(
    int unit,
    uint32 node_id,
    sw_state_idx_htbl_t hash_table,
    SW_STATE_INDEX_HASH_TABLE_KEY * const key);

shr_error_e dnx_sw_state_journal_log_rh_htbl(
    int unit,
    uint32 node_id,
    sw_state_htb_t hash_table,
    uint8 table_idx,
    void *const key);

shr_error_e dnx_sw_state_journal_mutex_create(
    int unit,
    uint32 node_id,
    sw_state_mutex_t * ptr_mtx);

shr_error_e dnx_sw_state_journal_mutex_destroy(
    int unit,
    uint32 node_id,
    sw_state_mutex_t * ptr_mtx);

shr_error_e dnx_sw_state_journal_mutex_take(
    int unit,
    uint32 node_id,
    sw_state_mutex_t * ptr_mtx,
    uint32 usec);

shr_error_e dnx_sw_state_journal_mutex_give(
    int unit,
    uint32 node_id,
    sw_state_mutex_t * ptr_mtx);

shr_error_e dnx_sw_state_journal_sem_create(
    int unit,
    uint32 node_id,
    sw_state_sem_t * ptr_sem);

shr_error_e dnx_sw_state_journal_sem_destroy(
    int unit,
    uint32 node_id,
    sw_state_sem_t * ptr_sem,
    int is_binary,
    int initial_count);

shr_error_e dnx_sw_state_journal_sem_take(
    int unit,
    uint32 node_id,
    sw_state_sem_t * ptr_sem,
    uint32 usec);

shr_error_e dnx_sw_state_journal_sem_give(
    int unit,
    uint32 node_id,
    sw_state_sem_t * ptr_sem);

shr_error_e dnx_sw_state_journal_get_head_seq_id(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 is_comparison,
    uint32 *seq_id);

shr_error_e dnx_sw_state_journal_entries_stamp(
    int unit,
    char *format,
    va_list args);

uint8 dnx_sw_state_journal_should_stamp(
    int unit);

shr_error_e dnx_sw_state_journal_all_tmp_suppress_unsafe(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 is_disable);

shr_error_e dnx_sw_state_comparison_journal_tmp_suppress(
    int unit,
    uint8 is_disabled);

shr_error_e dnx_sw_state_journal_exclude_by_stamp(
    int unit,
    dnx_er_db_t * dnx_er_db,
    char *stamp,
    uint8 is_comparison);

shr_error_e dnx_sw_state_journal_exclude_clear(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 is_comparison);

shr_error_e dnx_sw_state_journal_print(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 is_comparison);

shr_error_e dnx_sw_state_journal_logger_state_change(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 is_comparison);

#define DNX_SW_STATE_COMPARISON_SUPPRESS(_unit)\
    dnx_sw_state_comparison_journal_tmp_suppress(_unit, TRUE)

#define DNX_SW_STATE_COMPARISON_UNSUPPRESS(_unit)\
    dnx_sw_state_comparison_journal_tmp_suppress(_unit, FALSE)

#endif
