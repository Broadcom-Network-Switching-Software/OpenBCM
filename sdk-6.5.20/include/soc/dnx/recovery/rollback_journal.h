/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * INFO: This module is a rollback journal,
 * saving the old values that were overridden
 * by access calls since the beginning of the last transaction.
 */

#ifndef _DNX_ROLLBACK_JOURNAL_JOURNAL_H

#define _DNX_ROLLBACK_JOURNAL_JOURNAL_H

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>
#include <sal/core/thread.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>
#include <soc/dnx/recovery/rollback_journal_utils.h>

#ifdef DNX_ERR_RECOVERY_VALIDATION

#define DNX_ROLLBACK_JOURNAL_VALIDATION_ASSERT

#ifdef DNX_ROLLBACK_JOURNAL_VALIDATION_ASSERT
#define DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT(_expr)\
    DNXC_SW_STATE_IF_ERR_ASSERT(_expr)

#define DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_expr,_formatting_string,_param2,_param3,_param4)\
    DNXC_SW_STATE_IF_ERR_ASSERT_WITH_LOG(_expr,_formatting_string,_param2,_param3,_param4)
#else
#define DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT(_expr)\
    SHR_IF_ERR_EXIT(_expr)

#define DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_expr,_formatting_string,_param2,_param3,_param4)\
    SHR_IF_ERR_EXIT_WITH_LOG(_expr,_formatting_string,_param2,_param3,_param4)
#endif

#else
#define DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT(_expr)
#define DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_expr,_formatting_string,_param2,_param3,_param4)
#endif

#define DNX_ROLLBACK_JOURNAL_JOURNAL_MAX_NUM 6

#if 1
#define DNX_ROLLBACK_JOURNAL_MAX_SIZE 0
#endif

#define DNX_ROLLBACK_JOURNAL_MAX_STAMP_SIZE 512
#define DNX_ROLLBACK_JOURNAL_CHUNK_SIZE (20*4096)

typedef shr_error_e(
    *dnx_rollback_journal_rollback_handler) (
    int unit,
    uint8 *metadata,
    uint8 *payload,
    char *stamp);

typedef shr_error_e(
    *dnx_rollback_journal_print_handler) (
    int unit,
    uint8 *metadata,
    uint8 *payload);

typedef shr_error_e(
    *dnx_rollback_journal_rollback_traverse_handler) (
    int unit,
    uint8 *metadata,
    uint8 *payload,
    uint8 *traversed_metadata,
    uint8 *traversed_payload);

typedef shr_error_e(
    *dnx_rollback_journal_is_distinct_cb) (
    int unit,
    uint8 *first_metadata,
    uint8 *first_payload,
    uint8 *second_metadata,
    uint8 *second_payload,
    uint8 *result);

typedef uint8 (
    *dnx_rollback_journal_is_on_cb) (
    int unit);

typedef uint32 (
    *dnx_rollback_journal_new_entry_seq_id_get_cb) (
    int unit);

typedef enum dnx_rollback_journal_entry_state_d
{

    DNX_ROLLBACK_JOURNAL_ENTRY_STATE_INVALID = 0,

    DNX_ROLLBACK_JOURNAL_ENTRY_STATE_VALID,

    DNX_ROLLBACK_JOURNAL_ENTRY_STATE_ALWAYS_VALID
} dnx_rollback_journal_entry_state;

typedef struct dnx_rollback_journal_mem_chunk_d
{
    struct dnx_rollback_journal_mem_chunk_d *prev;
    struct dnx_rollback_journal_mem_chunk_d *next;
    uint32 data_size;
    int32 num_allocated_bytes;
    int32 available_space_at_end;
} dnx_rollback_journal_mem_chunk_t;

typedef struct dnx_rollback_journal_mem_pool_d
{
    dnx_rollback_journal_mem_chunk_t *head;
    dnx_rollback_journal_mem_chunk_t *tail;
    int32 num_chunks;
    int32 total_bytes_allocated;
} dnx_rollback_journal_mem_pool_t;

typedef struct dnx_rollback_journal_entry_d
{
    dnx_rollback_journal_mem_chunk_t *mem_chunk;
    int32 split_offset;
    struct dnx_rollback_journal_entry_d *next;
    struct dnx_rollback_journal_entry_d *prev;

    dnx_rollback_journal_entry_state state;

    uint32 seq_id;
    uint32 metadata_size;
    uint32 payload_size;
    dnx_rollback_journal_rollback_handler rollback_handler;
    dnx_rollback_journal_rollback_traverse_handler rollback_traverse_handler;
    dnx_rollback_journal_print_handler print_handler;

    char *stamp;
    dnx_rollback_journal_mem_chunk_t *stamp_mem_chunk;
    uint32 stamp_size;
} dnx_rollback_journal_entry_t;

typedef struct dnx_rollback_journal_cbs_d
{
    dnx_rollback_journal_is_on_cb is_on;
    dnx_rollback_journal_new_entry_seq_id_get_cb seq_id_get;
} dnx_rollback_journal_cbs_t;

typedef struct dnx_rollback_journal_d
{
    dnx_rollback_journal_mem_pool_t mem_pool;
    dnx_rollback_journal_entry_t *head;
    dnx_rollback_journal_entry_t *tail;
    uint8 is_logging;
    uint8 is_inverse_journaling;
    uint8 is_distinct;
    uint32 entry_counter;
    uint32 size;
    uint32 max_size;
    uint32 is_journaled_disabled_counter;
    dnx_rollback_journal_type_e type;
    dnx_rollback_journal_subtype_e subtype;
    dnx_rollback_journal_cbs_t cbs;
} dnx_rollback_journal_t;

typedef struct dnx_rollback_journal_pool_d
{
    uint8 is_any_journal_rolling_back;
    uint32 is_init_restrict_bypassed_counter;
    uint32 count;
    dnx_rollback_journal_t *entries[DNX_ROLLBACK_JOURNAL_JOURNAL_MAX_NUM];
} dnx_rollback_journal_pool_t;

typedef uint32 dnx_rollback_journal_handle_t;

#define DNX_ROLLBACK_JOURNAL_INVALID_HANDLE -1

uint8 dnx_rollback_journal_is_done_init(
    int unit);

shr_error_e dnx_rollback_journal_is_init_check_bypass(
    int unit,
    uint8 is_on);

uint8 dnx_rollback_journal_is_error_recovery_bypassed(
    int unit);

uint8 dnx_rollback_journal_is_on(
    int unit,
    dnx_rollback_journal_handle_t handle);

shr_error_e dnx_rollback_journal_new(
    int unit,
    uint8 is_inverse_journaling,
    dnx_rollback_journal_type_e type,
    dnx_rollback_journal_subtype_e subtype,
    dnx_rollback_journal_cbs_t cbs,
    dnx_rollback_journal_handle_t * handle);

shr_error_e dnx_rollback_journal_destroy(
    int unit,
    dnx_rollback_journal_handle_t * handle);

shr_error_e dnx_rollback_journal_rollback(
    int unit,
    uint8 only_head_rollback,
    dnx_rollback_journal_handle_t handle);

shr_error_e dnx_rollback_journal_clear(
    int unit,
    uint8 only_head_clear,
    dnx_rollback_journal_handle_t handle);

shr_error_e dnx_rollback_journal_entry_new(
    int unit,
    dnx_rollback_journal_handle_t handle,
    uint8 *metadata,
    uint32 metadata_size,
    uint8 *payload,
    uint32 payload_size,
    dnx_rollback_journal_entry_state default_state,
    dnx_rollback_journal_rollback_handler rollback_handler,
    dnx_rollback_journal_rollback_traverse_handler rollback_traverse_handler,
    dnx_rollback_journal_print_handler print_handler);

shr_error_e dnx_rollback_journal_get_head_seq_id(
    int unit,
    uint32 *seq_id,
    dnx_rollback_journal_handle_t handle);

shr_error_e dnx_rollback_journal_invalidate_last_inserted_entry(
    int unit,
    dnx_rollback_journal_handle_t handle);

shr_error_e dnx_rollback_journal_distinct(
    int unit,
    dnx_rollback_journal_handle_t handle,
    dnx_rollback_journal_is_distinct_cb is_distinct_cb);

shr_error_e dnx_rollback_journal_disabled_counter_change(
    int unit,
    uint8 is_disable,
    dnx_rollback_journal_handle_t handle,
    uint8 is_unsafe);

shr_error_e dnx_rollback_journal_disabled_counter_get(
    int unit,
    dnx_rollback_journal_handle_t handle,
    uint32 *counter);

shr_error_e dnx_rollback_journal_entries_stamp(
    int unit,
    dnx_rollback_journal_handle_t handle,
    char *format);

uint8 dnx_rollback_journal_is_any_journal_rolling_back(
    int unit);

shr_error_e dnx_rollback_journal_print(
    int unit,
    dnx_rollback_journal_handle_t handle);

shr_error_e dnx_rollback_journal_logger_state_change_by_type(
    int unit,
    dnx_rollback_journal_handle_t handle);

shr_error_e dnx_rollback_journal_is_logging(
    int unit,
    dnx_rollback_journal_handle_t handle,
    uint8 *is_logging);

#endif
