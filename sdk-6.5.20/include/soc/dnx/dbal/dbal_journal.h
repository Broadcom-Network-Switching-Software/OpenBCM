/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * INFO: This module is one of the components needed for Error Recovery,
 * it is a rollback journal, saving the old values that were overridden
 * by dbal access calls since the beginning of the last transaction.
 */
#ifndef _DNX_DBAL_JOURNAL_H

#define _DNX_DBAL_JOURNAL_H

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>
#include <sal/core/thread.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnx/recovery/rollback_journal.h>

#define DNX_DBAL_JOURNAL_EXCLUDED_TABLES_MAX_NOF 10

#define DNX_DBAL_JOURNAL_ROLLBACK_FLAG                     SAL_BIT(0)
#define DNX_DBAL_JOURNAL_COMPARE_FLAG                      SAL_BIT(1)

typedef enum
{
    DNX_DBAL_JOURNAL_ROLLBACK = 0,
    DNX_DBAL_JOURNAL_COMPARISON,
    DNX_DBAL_JOURNAL_ALL
} dnx_err_recovery_journal_e;

typedef void (
    *dnx_dbal_journal_access_cb) (
    int unit,
    dbal_logical_table_t * table_ptr);

typedef struct dnx_dbal_journal_entry_d
{
    dbal_actions_e action;
} dnx_dbal_journal_entry_t;

typedef struct dnx_dbal_journal_transaction_d
{
    uint32 excluded_tables_count;
    dbal_tables_e excluded_tables[DNX_DBAL_JOURNAL_EXCLUDED_TABLES_MAX_NOF];
} dnx_dbal_journal_transaction_t;

typedef struct dnx_dbal_journal_d
{
    dnx_rollback_journal_handle_t handle;
    dnx_dbal_journal_transaction_t transaction;
    dnx_dbal_journal_access_cb access_cb;
} dnx_dbal_journal_t;

typedef struct dnx_dbal_journal_manager_d
{
    uint32 unsupported_table_bypass_counter;
    dnx_dbal_journal_t journals[2];
} dnx_dbal_journal_manager_t;

uint8 dnx_dbal_journal_is_done_init(
    int unit);

shr_error_e dnx_dbal_journal_initialize(
    int unit,
    dnx_rollback_journal_cbs_t rollback_journal_cbs,
    dnx_dbal_journal_access_cb access_cb,
    uint8 is_comparison);

uint8 dnx_dbal_journal_is_on(
    int unit,
    dnx_err_recovery_journal_e journals);

shr_error_e dnx_dbal_journal_destroy(
    int unit,
    uint8 is_comparison);

shr_error_e dnx_dbal_journal_roll_back(
    int unit,
    uint8 only_head_rollback,
    uint8 is_comparison);

shr_error_e dnx_dbal_journal_clear(
    int unit,
    uint8 only_head_clear,
    uint8 is_comparison);

shr_error_e dnx_dbal_journal_log_add(
    int unit,
    dbal_entry_handle_t * get_handle,
    dbal_entry_handle_t * entry_handle,
    uint8 is_dbal_error);

shr_error_e dnx_dbal_journal_log_clear(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 is_dbal_error);

shr_error_e dnx_dbal_journal_get_head_seq_id(
    int unit,
    uint8 is_comparison,
    uint32 *seq_id);

shr_error_e dnx_dbal_journal_all_tmp_suppress_unsafe(
    int unit,
    uint8 is_disable);

shr_error_e dnx_dbal_journal_comparison_tmp_suppress_unsafe(
    int unit,
    uint8 is_disable);

uint8 dnx_dbal_journal_is_tmp_suppressed_unsafe(
    int unit,
    uint8 is_comparison);

uint8 dnx_dbal_journal_should_create_test(
    int unit,
    uint32 table_id);

shr_error_e dnx_dbal_journal_unsupported_tables_bypass(
    int unit,
    uint8 is_bypassed);

uint8 dnx_dbal_journal_are_unsupported_tables_bypassed(
    int unit);

shr_error_e dnx_dbal_journal_exclude_table_by_stamp(
    int unit,
    char *stamp,
    uint8 is_comparison);

shr_error_e dnx_dbal_journal_exclude_table_clear(
    int unit,
    uint8 is_comparison);

shr_error_e dnx_dbal_journal_print(
    int unit,
    uint8 is_comparison);

shr_error_e dnx_dbal_journal_logger_state_change(
    int unit,
    uint8 is_comparison);

shr_error_e dnx_dbal_journal_logger_state_get(
    int unit,
    uint8 *is_logging);

#define DNX_DBAL_JRNL(x) x
#define DNX_DBAL_JOURNAL_IS_ON(_unit, _flags) dnx_dbal_journal_is_on(_unit, _flags)

#endif
