
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_ER_REGRESSION_H

#define _DNX_ER_REGRESSION_H

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>
#include <sal/core/thread.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnx/recovery/rollback_journal_utils.h>
#include <soc/dnx/recovery/rollback_journal_diag.h>
#include <soc/dnx/swstate/dnx_sw_state_journal.h>

#define DNX_ERR_RECOVERY_REGRESSION_TESTING_API_NAME_UPDATE(_unit)\
    dnx_er_regress_testing_api_name_update(_unit, __func__)

#define DNX_ERR_RECOVERY_REGRESSION_TESTING_BEGIN(_unit)                     \
    r_rv = BCM_E_NONE;                                                       \
    do {

#define DNX_ERR_RECOVERY_REGRESSION_TESTING_END(_unit)          \
    } while(BCM_CONTROL_UNIT_LEGAL(_unit) && dnx_er_regress_test_should_loop(_unit) && (BCM_E_NONE == r_rv))

#define DNX_ERR_RECOVERY_REGRESSION_API_NO_TESTING(_unit)\
    dnx_er_regress_testing_api_exclude(_unit)

#define DNX_ERR_RECOVERY_REGRESSION_API_SKIP_TESTING(_unit)\
    dnx_er_regress_sim_skip(_unit)

#define DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(_unit, _size, _var_ptr)     \
do {                                                                                   \
    if(NULL != (_var_ptr))                                                             \
    {                                                                                  \
        SHR_IF_ERR_EXIT(dnx_er_regress_journal_var(_unit, _size, (uint8 *)(_var_ptr)));\
    }                                                                                  \
} while(0)

#define DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR(_unit, _var)\
    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(_unit, sizeof(_var), &(_var))

#define DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_ARRAY(_unit, _array, _count) \
do {                                                                            \
    for(int er_idx = 0; er_idx < (_count); er_idx++) {                          \
        DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR(_unit, (_array)[er_idx]);\
    }                                                                           \
} while(0)

#define DNX_ERR_RECOVERY_REGRESSION_ERROR_SUPPRESS(_unit, _type) dnx_err_recovery_regress_violate_tmp_allow(_unit, _type, FALSE)

#define DNX_ERR_RECOVERY_REGRESSION_ERROR_UNSUPPRESS(_unit, _type) dnx_err_recovery_regress_violate_tmp_allow(_unit, _type, TRUE)

typedef enum
{
    DNX_ERR_RECOVERY_REGRESS_ERROR_SW_ACCESS_OUTSIDE_TRANS = 0,
    DNX_ERR_RECOVERY_REGRESS_ERROR_DBAL_ACCESS_OUTSIDE_TRANS,
    DNX_ERR_RECOVERY_REGRESS_ERROR_NOF
} dnx_err_recovery_regress_error_t;

typedef uint8 (
    *dnx_er_regression_cb) (
    int unit);

typedef struct dnx_er_regress_transaction_info_d
{

    uint8 is_api_excluded;

    uint8 is_api_ran;

    uint8 has_access_outside_trans;

    uint32 top_level_trans_in_api_count;

    uint32 err_type_suppress_counters[DNX_ERR_RECOVERY_REGRESS_ERROR_NOF];

    char dnx_er_regress_testing_api_name[DNX_ROLLBACK_JOURNAL_API_NAME_MAX_LENGTH];

} dnx_er_regress_transaction_info_t;

typedef struct dnx_er_regress_d
{

    uint8 is_appl_init_done;

    uint8 is_testing_enabled;

} dnx_er_regress_t;

void dnx_er_regress_init_deinit_counters_reset(
    int unit,
    dnx_er_db_t * dnx_er_db);

shr_error_e dnx_er_regress_init(
    int unit);

shr_error_e dnx_er_regress_deinit(
    int unit);

void dnx_er_regress_top_level_api_flags_reset(
    int unit,
    dnx_er_db_t * dnx_er_db);

shr_error_e dnx_er_regress_comparison_tmp_allow(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 is_on);

shr_error_e dnx_er_regress_transaction_start(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 is_top_level_transaction,
    const char *function_name);

uint8 dnx_er_regress_transaction_end_should_abort(
    int unit,
    dnx_er_db_t * dnx_er_db);

shr_error_e dnx_er_regress_transaction_end(
    int unit,
    dnx_er_db_t * dnx_er_db);

void dnx_er_regress_journal_start(
    int unit);

void dnx_er_regress_appl_init_state_change(
    int unit,
    uint8 is_init);

void dnx_er_regress_testing_api_name_update(
    int unit,
    const char *func_name);

uint8 dnx_er_regress_test_should_loop(
    int unit);

uint8 dnx_er_regress_testing_api_test_mode_get(
    int unit);

void dnx_er_regress_testing_api_test_mode_change(
    int unit,
    uint8 is_on);

void dnx_er_regress_testing_api_exclude(
    int unit);

void dnx_er_regress_sim_skip(
    int unit);

shr_error_e dnx_er_regress_journal_var(
    int unit,
    uint32 size,
    uint8 *ptr);

shr_error_e dnx_err_recovery_regress_violate_tmp_allow(
    int unit,
    dnx_err_recovery_regress_error_t type,
    uint8 is_on);

void dnx_er_regress_swstate_access_cb(
    int unit,
    dnx_er_db_t * dnx_er_db);

void dnx_er_regress_dbal_access_cb(
    int unit,
    dnx_er_db_t * dnx_er_db,
    dbal_logical_table_t * table_ptr);

shr_error_e dnx_er_regress_affirm_er_macro_usage_legality(
    int unit,
    dnx_er_db_t * dnx_er_db,
    const char *function_name,
    uint8 check_er_regress_conds);

#endif
