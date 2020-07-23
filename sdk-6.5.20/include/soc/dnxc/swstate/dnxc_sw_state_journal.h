/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
#include <assert.h>
#include <sal/core/thread.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/recovery/rollback_journal.h>
#endif
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>

#ifdef BCM_DNX_SUPPORT
#define DNXC_SW_STATE_JOURNAL_EXCLUDED_STAMPS_MAX_NOF 10

typedef void (
    *dnxc_sw_state_journal_access_cb) (
    int unit);

typedef struct dnxc_sw_state_journal_transaction_d
{
    uint32 excluded_stamps_count;
    char *excluded_stamps[DNXC_SW_STATE_JOURNAL_EXCLUDED_STAMPS_MAX_NOF];
} dnxc_sw_state_journal_transaction_t;

typedef struct dnxc_sw_state_journal_d
{
    dnx_rollback_journal_handle_t handle;
    dnxc_sw_state_journal_transaction_t transaction;
    dnxc_sw_state_journal_access_cb access_cb;
} dnxc_sw_state_journal_t;

typedef struct dnxc_sw_state_journal_manager_d
{
    int dnxc_sw_state_journal_dummy;
    dnxc_sw_state_journal_t journals[2];
} dnxc_sw_state_journal_manager_t;

typedef enum dnxc_sw_state_journal_entry_e
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
    SW_JOURNAL_ENTRY_TYPE_COUNT
} dnxc_sw_state_journal_entry_type_t;

typedef struct dnxc_sw_state_journal_memcpy_entry_d
{
    sal_vaddr_t offset;
} dnxc_sw_state_journal_memcpy_entry_t;

typedef struct dnxc_sw_state_journal_alloc_entry_d
{
    sal_vaddr_t location_offset;
    sal_vaddr_t ptr_location_offset;
} dnxc_sw_state_journal_alloc_entry_t;

typedef struct dnxc_sw_state_journal_free_entry_d
{
    sal_vaddr_t location_offset;
    sal_vaddr_t ptr_location_offset;
} dnxc_sw_state_journal_free_entry_t;

typedef struct dnxc_sw_state_journal_mutex_create_entry_d
{
    sal_vaddr_t mtx_offset;
} dnxc_sw_state_journal_mutex_create_entry_t;

typedef struct dnxc_sw_state_journal_mutex_destroy_entry_d
{
    sal_vaddr_t mtx_offset;
} dnxc_sw_state_journal_mutex_destroy_entry_t;

typedef struct dnxc_sw_state_journal_mutex_take_entry_d
{
    sal_vaddr_t mtx_offset;
    uint32 usec;
} dnxc_sw_state_journal_mutex_take_entry_t;

typedef struct dnxc_sw_state_journal_mutex_give_entry_d
{
    sal_vaddr_t mtx_offset;
} dnxc_sw_state_journal_mutex_give_entry_t;

typedef struct dnxc_sw_state_journal_sem_create_entry_d
{
    sal_vaddr_t sem_offset;
} dnxc_sw_state_journal_sem_create_entry_t;

typedef struct dnxc_sw_state_journal_sem_destroy_entry_d
{
    sal_vaddr_t sem_offset;
    int is_binary;
    int initial_count;
} dnxc_sw_state_journal_sem_destroy_entry_t;

typedef struct dnxc_sw_state_journal_sem_take_entry_d
{
    sal_vaddr_t sem_offset;
    uint32 usec;
} dnxc_sw_state_journal_sem_take_entry_t;

typedef struct dnxc_sw_state_journal_sem_give_entry_d
{
    sal_vaddr_t sem_offset;
} dnxc_sw_state_journal_sem_give_entry_t;

typedef struct dnxc_sw_state_journal_htbl_entry_d
{

    sal_vaddr_t hash_table_offset;
} dnxc_sw_state_journal_htbl_entry_t;

typedef union dnxc_sw_state_journal_entry_data_d
{
    dnxc_sw_state_journal_memcpy_entry_t memcpy_data;
    dnxc_sw_state_journal_alloc_entry_t alloc_data;
    dnxc_sw_state_journal_free_entry_t free_data;
    dnxc_sw_state_journal_mutex_create_entry_t mutex_create;
    dnxc_sw_state_journal_mutex_destroy_entry_t mutex_destroy;
    dnxc_sw_state_journal_mutex_take_entry_t mutex_take;
    dnxc_sw_state_journal_mutex_give_entry_t mutex_give;
    dnxc_sw_state_journal_sem_create_entry_t sem_create;
    dnxc_sw_state_journal_sem_destroy_entry_t sem_destroy;
    dnxc_sw_state_journal_sem_take_entry_t sem_take;
    dnxc_sw_state_journal_sem_give_entry_t sem_give;
    dnxc_sw_state_journal_htbl_entry_t htbl_data;
} dnxc_sw_state_journal_entry_data_t;

typedef struct dnxc_sw_state_journal_entry_d
{
    dnxc_sw_state_journal_entry_type_t entry_type;
    uint32 nof_elements;
    uint32 element_size;
    uint32 module_id;
    dnxc_sw_state_journal_entry_data_t data;
} dnxc_sw_state_journal_entry_t;

uint8 dnxc_sw_state_journal_is_done_init(
    int unit);

shr_error_e dnxc_sw_state_journal_initialize(
    int unit,
    dnx_rollback_journal_cbs_t rollback_journal_cbs,
    dnxc_sw_state_journal_access_cb access_cb,
    uint8 is_comparison);

shr_error_e dnxc_sw_state_journal_destroy(
    int unit,
    uint8 is_comparison);

shr_error_e dnxc_sw_state_journal_roll_back(
    int unit,
    uint8 only_head_rollback,
    uint8 is_comparison);

shr_error_e dnxc_sw_state_journal_clear(
    int unit,
    uint8 only_head_clear,
    uint8 is_comparison);

shr_error_e dnxc_sw_state_journal_log_free(
    int unit,
    uint32 module_id,
    uint32 nof_elements,
    uint32 element_size,
    uint8 **ptr);

shr_error_e dnxc_sw_state_journal_log_alloc(
    int unit,
    uint32 module_id,
    uint8 **ptr);

shr_error_e dnxc_sw_state_journal_log_memcpy(
    int unit,
    uint32 module_id,
    uint32 size,
    uint8 *ptr);

#ifdef BCM_DNX_SUPPORT

shr_error_e dnxc_sw_state_journal_log_htbl(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table,
    SW_STATE_HASH_TABLE_KEY * const key);
#endif

shr_error_e dnxc_sw_state_journal_mutex_create(
    int unit,
    uint32 module_id,
    sw_state_mutex_t * ptr_mtx);

shr_error_e dnxc_sw_state_journal_mutex_destroy(
    int unit,
    uint32 module_id,
    sw_state_mutex_t * ptr_mtx);

shr_error_e dnxc_sw_state_journal_mutex_take(
    int unit,
    uint32 module_id,
    sw_state_mutex_t * ptr_mtx,
    uint32 usec);

shr_error_e dnxc_sw_state_journal_mutex_give(
    int unit,
    uint32 module_id,
    sw_state_mutex_t * ptr_mtx);

shr_error_e dnxc_sw_state_journal_sem_create(
    int unit,
    uint32 module_id,
    sw_state_sem_t * ptr_sem);

shr_error_e dnxc_sw_state_journal_sem_destroy(
    int unit,
    uint32 module_id,
    sw_state_sem_t * ptr_sem,
    int is_binary,
    int initial_count);

shr_error_e dnxc_sw_state_journal_sem_take(
    int unit,
    uint32 module_id,
    sw_state_sem_t * ptr_sem,
    uint32 usec);

shr_error_e dnxc_sw_state_journal_sem_give(
    int unit,
    uint32 module_id,
    sw_state_sem_t * ptr_sem);

shr_error_e dnxc_sw_state_journal_get_head_seq_id(
    int unit,
    uint8 is_comparison,
    uint32 *seq_id);

shr_error_e dnxc_sw_state_journal_entries_stamp(
    int unit,
    char *format,
    va_list args);

uint8 dnxc_sw_state_journal_should_stamp(
    int unit);

shr_error_e dnxc_sw_state_journal_all_tmp_suppress_unsafe(
    int unit,
    uint8 is_disable);

shr_error_e dnxc_sw_state_comparison_journal_tmp_suppress(
    int unit,
    uint8 is_disabled);

shr_error_e dnxc_sw_state_journal_exclude_by_stamp(
    int unit,
    char *stamp,
    uint8 is_comparison);

shr_error_e dnxc_sw_state_journal_exclude_clear(
    int unit,
    uint8 is_comparison);

shr_error_e dnxc_sw_state_journal_print(
    int unit,
    uint8 is_comparison);

shr_error_e dnxc_sw_state_journal_logger_state_change(
    int unit,
    uint8 is_comparison);

#define DNXC_SW_STATE_COMPARISON_SUPPRESS(_unit)\
    dnxc_sw_state_comparison_journal_tmp_suppress(_unit, TRUE)

#define DNXC_SW_STATE_COMPARISON_UNSUPPRESS(_unit)\
    dnxc_sw_state_comparison_journal_tmp_suppress(_unit, FALSE)

#else
#define DNXC_SW_STATE_COMPARISON_SUPPRESS(_unit)
#define DNXC_SW_STATE_COMPARISON_UNSUPPRESS(_unit)
#endif

#endif
