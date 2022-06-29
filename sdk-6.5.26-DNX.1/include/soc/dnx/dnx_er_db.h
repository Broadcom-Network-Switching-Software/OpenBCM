
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_ERR_RECOVERY_DB_H
#define _DNX_ERR_RECOVERY_DB_H

#include <sal/core/alloc.h>
#include <sal/core/thread.h>
#include <soc/sand/shrextend/shrextend_error.h>
#include <soc/sand/shrextend/shrextend_debug.h>

typedef enum
{
    DNX_ER_DB_JOURNAL_HANDLE_SW_STATE_ROLLBACK,
    DNX_ER_DB_JOURNAL_HANDLE_DBAL_ROLLBACK,
    DNX_ER_DB_JOURNAL_HANDLE_GENERIC_STATE_ROLLBACK,
    DNX_ER_DB_JOURNAL_HANDLE_SW_STATE_COMPARISON,
    DNX_ER_DB_JOURNAL_HANDLE_DBAL_COMPARISON,
    DNX_ER_DB_JOURNAL_HANDLE_GENERIC_STATE_COMPARISON,
    DNX_ER_DB_JOURNAL_HANDLE_MAX_NUM
} dnx_er_db_journal_handle_e;

typedef struct dnx_er_db_d
{

    int unit;

    struct err_recovery_manager_d *dnx_err_recovery_manager;

    struct dnx_rollback_journal_pool_d *dnx_rollback_journal_pool;

    struct dnx_dbal_journal_manager_d *dnx_dbal_journal_manager;

    struct dnx_sw_state_journal_manager_d *dnx_sw_state_manager;

    struct dnx_state_snapshot_manager_d *dnx_state_snapshot_manager;
} dnx_er_db_t;

void dnx_er_db_free(
    void *dnx_er_db);

void dnx_er_db_key_init(
    int unit);

shr_error_e dnx_er_db_key_deinit(
    int unit);

shr_error_e dnx_er_db_get(
    int unit,
    dnx_er_db_t ** dnx_er_db);

shr_error_e dnx_er_db_get_if_initialized(
    int unit,
    dnx_er_db_t ** dnx_er_db);

#endif
