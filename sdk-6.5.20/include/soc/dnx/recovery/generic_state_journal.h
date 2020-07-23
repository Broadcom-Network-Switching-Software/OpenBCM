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

#ifndef _DNX_GENERIC_STATE_ROLLBACK_JOURNAL_JOURNAL_H

#define _DNX_GENERIC_STATE_ROLLBACK_JOURNAL_JOURNAL_H

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>
#include <sal/core/thread.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/recovery/rollback_journal.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dbal/dbal_structures.h>

typedef struct dnx_generic_state_journal_dbal_table_entry_d
{
    int res_type_idx;
    dbal_tables_e table_id;
} dnx_generic_state_journal_dbal_table_entry_t;
#endif

typedef struct dnx_generic_state_journal_memcpy_entry_d
{
    uint8 *ptr;
    uint32 size;
} dnx_generic_state_journal_entry_t;

typedef struct dnx_generic_state_journal_free_entry_d
{
    uint32 nof_elements;
    uint32 element_size;
    uint8 *location;
    uint8 **ptr_location;
} dnx_generic_state_journal_free_entry_t;

typedef struct dnx_generic_state_journal_d
{
    dnx_rollback_journal_handle_t handle;
} dnx_generic_state_journal_t;

typedef struct dnx_generic_state_journal_manager_d
{
    int dnx_generic_state_journal_dummy;
    dnx_generic_state_journal_t journals[2];
} dnx_generic_state_journal_manager_t;

#define DNX_GENERIC_STATE_JOURNAL_EXIT_IF_OFF(unit)        \
    do {                                                    \
        if (!dnx_is_generic_state_journal_is_on(unit))     \
        {                                                   \
            SHR_EXIT();                                     \
        }                                                   \
    } while(0)

typedef shr_error_e(
    *dnx_generic_state_journal_rollback_handler) (
    int unit,
    uint8 *metadata,
    uint8 *payload,
    char *stamp);

typedef shr_error_e(
    *dnx_generic_state_journal_print_handler) (
    int unit,
    uint8 *metadata,
    uint8 *payload);

shr_error_e dnx_generic_state_journal_initialize(
    int unit,
    dnx_rollback_journal_cbs_t rollback_journal_cbs);

shr_error_e dnx_generic_state_journal_destroy(
    int unit,
    uint8 is_comparison);

shr_error_e dnx_generic_state_journal_log_memcpy(
    int unit,
    uint32 size,
    uint8 *ptr);

shr_error_e dnx_generic_state_journal_log_free(
    int unit,
    uint32 nof_elements,
    uint32 element_size,
    uint8 **ptr_location);

#ifdef BCM_DNX_SUPPORT

shr_error_e dnx_generic_state_journal_log_dbal_table_create(
    int unit,
    dbal_tables_e table_id);

shr_error_e dnx_generic_state_journal_log_dbal_table_info_change(
    int unit,
    dbal_tables_e table_id);

shr_error_e dnx_generic_state_journal_log_result_type_add(
    int unit,
    dbal_tables_e table_id,
    int res_type_idx);

shr_error_e dnx_generic_state_journal_log_result_type_delete(
    int unit,
    dbal_tables_e table_id);

#endif

int dnx_is_generic_state_journal_is_on(
    int unit);

shr_error_e dnx_generic_state_journal_entry_new(
    int unit,
    uint8 *metadata,
    uint32 metadata_size,
    uint8 *payload,
    uint32 payload_size,
    dnx_generic_state_journal_rollback_handler rollback_handler);

shr_error_e dnx_generic_state_journal_clear(
    int unit,
    uint8 only_head_clear);

shr_error_e dnx_generic_state_journal_get_head_seq_id(
    int unit,
    uint32 *seq_id);

shr_error_e dnx_generic_state_journal_roll_back(
    int unit,
    uint8 only_head_rollback);

shr_error_e dnx_generic_state_journal_all_tmp_suppress_unsafe(
    int unit,
    uint8 only_head_rollback);

#endif
