
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_ERR_RECOVERY_MANAGER_H

#define _DNX_ERR_RECOVERY_MANAGER_H

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>
#include <sal/core/thread.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnx/recovery/rollback_journal_utils.h>
#include <soc/dnx/recovery/rollback_journal_diag.h>
#include <soc/dnx/dnx_er_regression.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <bcm/switch.h>

#ifdef DNX_ERR_RECOVERY_VALIDATION_BASIC

#define DNX_ERR_RECOVERY_MAX_NUM_NESTED_APIS 3

#define DNX_ERR_RECOVERY_API_COUNTER_INC(_unit)\
    dnx_err_recovery_api_counter_inc(_unit)

#define DNX_ERR_RECOVERY_API_COUNTER_DEC(_unit)\
    dnx_err_recovery_api_counter_dec(_unit, r_rv != BCM_E_NONE)
#else
#define DNX_ERR_RECOVERY_API_COUNTER_INC(_unit)
#define DNX_ERR_RECOVERY_API_COUNTER_DEC(_unit)
#endif

#ifdef DNX_ERR_RECOVERY_VALIDATION

#define DNX_ERR_RECOVERY_NOT_NEEDED(_unit)\
    DNX_ERR_RECOVERY_REGRESSION_API_NO_TESTING(unit);\
    dnx_err_recovery_not_needed(_unit)

#define DNX_ERR_RECOVERY_NOT_NEEDED_SWITCH(_unit)\
    dnx_err_recovery_not_needed(_unit)

#define DNX_ERR_RECOVERY_HW_ACCESS_IS_VALID(_unit)\
    dnx_err_recovery_hw_access_is_valid(_unit)
#else
#define DNX_ERR_RECOVERY_NOT_NEEDED(_unit) DNX_ERR_RECOVERY_REGRESSION_API_NO_TESTING(_unit)
#define DNX_ERR_RECOVERY_NOT_NEEDED_SWITCH(_unit)
#define DNX_ERR_RECOVERY_HW_ACCESS_IS_VALID(_unit)
#endif

#ifdef DNX_ROLLBACK_JOURNAL_DIAGNOSTICS

#define DNX_ROLLBACK_JOURNAL_DIAGNOSTICS_TRANSACTION_START(_unit)\
    SHR_IF_ERR_EXIT(dnx_rollback_journal_diag_start(_unit, __FUNCTION__))

#define DNX_ROLLBACK_JOURNAL_DIAGNOSTICS_TRANSACTION_END(_unit)\
    dnx_rollback_journal_diag_end(_unit)

#else
#define DNX_ROLLBACK_JOURNAL_DIAGNOSTICS_TRANSACTION_START(_unit)
#define DNX_ROLLBACK_JOURNAL_DIAGNOSTICS_TRANSACTION_END(_unit)
#endif

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING

#define DNX_ERR_RECOVERY_END_INTERNAL_ERR(_unit, _module_id)                                     \
do {                                                                                             \
    if(dnx_err_recovery_transaction_end(_unit, _module_id, SHR_GET_CURRENT_ERR()) != _SHR_E_NONE)\
    {                                                                                            \
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);                                                    \
    }                                                                                            \
} while(0)
#else

#define DNX_ERR_RECOVERY_END_INTERNAL_ERR(_unit, _module_id)\
    dnx_err_recovery_transaction_end(_unit, _module_id, SHR_GET_CURRENT_ERR())
#endif

#define DNX_ERR_RECOVERY_MAX_NUM_NESTED_TRANSACTIONS 15

#define DNX_ERR_RECOVERY_START_INTERNAL(_unit, _module_id)\
    SHR_IF_ERR_EXIT(dnx_err_recovery_transaction_start(_unit, _module_id))

#define DNX_ERR_RECOVERY_END_INTERNAL(_unit, _module_id)\
    DNX_ERR_RECOVERY_END_INTERNAL_ERR(_unit, _module_id)

#define DNX_ERR_RECOVERY_SUPPRESS(_unit)\
    dnx_err_recovery_tmp_allow(_unit, FALSE)

#define DNX_ERR_RECOVERY_UNSUPPRESS(_unit)\
    dnx_err_recovery_tmp_allow(_unit, TRUE)

#define DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(_unit)\
    DNX_ERR_RECOVERY_NOT_NEEDED(_unit);         \
    dnx_err_recovery_transaction_no_support_counter_inc(_unit)

#define DNX_ERR_RECOVERY_NO_SUPPORT_END(_unit)\
    dnx_err_recovery_transaction_no_support_counter_dec(_unit)

#define DNX_ERR_RECOVERY_TRANSACTION_START(_unit)              \
    DNX_ERR_RECOVERY_API_COUNTER_INC(_unit);                   \
    DNX_ERR_RECOVERY_REGRESSION_TESTING_API_NAME_UPDATE(_unit);\
    DNX_ERR_RECOVERY_REGRESSION_TESTING_BEGIN(_unit)

#define DNX_ERR_RECOVERY_TRANSACTION_END(_unit)\
    DNX_ERR_RECOVERY_REGRESSION_TESTING_END(_unit);\
    DNX_ERR_RECOVERY_API_COUNTER_DEC(_unit)

#define DNX_ROLLBACK_JOURNAL_START(_unit)\
    SHR_IF_ERR_EXIT(dnx_rollback_journal_start(_unit))

#define DNX_ROLLBACK_JOURNAL_END_AND_CLEAR(_unit)\
    SHR_IF_ERR_EXIT(dnx_rollback_journal_end(_unit, TRUE))

#define DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(_expr)                                                      \
do                                                                                                             \
{                                                                                                              \
    int _rv;                                                                                                   \
                                                                                                               \
    DNX_ERR_RECOVERY_SUPPRESS(unit);                                                                           \
    DNX_ERR_RECOVERY_REGRESSION_ERROR_SUPPRESS(unit, DNX_ERR_RECOVERY_REGRESS_ERROR_SW_ACCESS_OUTSIDE_TRANS);  \
    dnx_state_comparison_suppress(unit, TRUE);                                                                 \
                                                                                                               \
    _rv = _expr;                                                                                               \
                                                                                                               \
    dnx_state_comparison_suppress(unit, FALSE);                                                                \
    DNX_ERR_RECOVERY_REGRESSION_ERROR_UNSUPPRESS(unit, DNX_ERR_RECOVERY_REGRESS_ERROR_SW_ACCESS_OUTSIDE_TRANS);\
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);                                                                         \
                                                                                                               \
    if (SHR_FAILURE(_rv))                                                                                      \
    {                                                                                                          \
        LOG_ERROR_EX(BSL_LOG_MODULE,                                                                           \
            " Error '%s' indicated ; %s%s%s\r\n" ,                                                             \
            shrextend_errmsg_get(_rv) ,EMPTY,EMPTY,EMPTY) ;                                                    \
            _func_rv = _rv ;                                                                                   \
        SHR_EXIT() ;                                                                                           \
    }                                                                                                          \
} while (0)

#define DNX_ERR_RECOVERY_START(_unit)                          \
    DNX_ROLLBACK_JOURNAL_DIAGNOSTICS_TRANSACTION_START(_unit);\
    DNX_ERR_RECOVERY_START_INTERNAL(_unit, bcmModuleCount)

#define DNX_ERR_RECOVERY_START_MODULE_ID(_unit, _module_id)    \
    DNX_ROLLBACK_JOURNAL_DIAGNOSTICS_TRANSACTION_START(_unit);\
    DNX_ERR_RECOVERY_START_INTERNAL(_unit, _module_id)

#define DNX_ERR_RECOVERY_END(_unit)                      \
    DNX_ERR_RECOVERY_END_INTERNAL(_unit, bcmModuleCount);\
    DNX_ROLLBACK_JOURNAL_DIAGNOSTICS_TRANSACTION_END(_unit)

#define DNX_ERR_RECOVERY_END_MODULE_ID(_unit, _module_id)\
    DNX_ERR_RECOVERY_END_INTERNAL(_unit, _module_id);    \
    DNX_ROLLBACK_JOURNAL_DIAGNOSTICS_TRANSACTION_END(_unit)

typedef enum
{
    DNX_ERR_RECOVERY_API_NO_TEST_UNKNOWN = 0,

    DNX_ERR_RECOVERY_API_NO_TEST_IN_OUT_PARAM,

    DNX_ERR_RECOVERY_API_NO_TEST_DBAL_TBL_CREATE_DESTROY,

    DNX_ERR_RECOVERY_API_NO_TEST_SIBLING_TRANSACTIONS,

    DNX_ERR_RECOVERY_API_NO_TESTING_NOT_NEEDED
} dnx_err_recovery_api_no_test_reason_t;

#ifdef DNX_ERR_RECOVERY_VALIDATION

typedef enum
{
    DNX_ERR_RECOVERY_API_STATUS_UNKNOWN = 0,
    DNX_ERR_RECOVERY_API_OPTS_IN,
    DNX_ERR_RECOVERY_API_NOT_NEEDED
} dnx_err_recovery_api_status;
#endif

typedef struct err_recovery_manager_d
{

    uint32 is_suppressed_counter;

    uint32 transaction_counter;

    uint32 entry_counter;

    uint32 trans_bookmarks[DNX_ERR_RECOVERY_MAX_NUM_NESTED_TRANSACTIONS + 1];

#ifdef DNX_ERR_RECOVERY_VALIDATION_BASIC

    uint8 validation_enabled;

    uint32 api_counter;

    uint32 api_to_trans_map[DNX_ERR_RECOVERY_MAX_NUM_NESTED_APIS + 1];
#endif

#ifdef DNX_ERR_RECOVERY_VALIDATION

    dnx_err_recovery_api_status api_status;

    uint32 api_status_count;
#endif

} err_recovery_manager_t;

#ifdef DNX_ERR_RECOVERY_VALIDATION_BASIC

shr_error_e dnx_err_recovery_api_counter_inc(
    int unit);

shr_error_e dnx_err_recovery_api_counter_dec(
    int unit,
    uint8 bcm_api_error);
#endif

#ifdef DNX_ERR_RECOVERY_VALIDATION

shr_error_e dnx_err_recovery_not_needed(
    int unit);

uint8 dnx_err_recovery_hw_access_is_valid(
    int unit);
#endif

shr_error_e dnx_err_recovery_is_init_check_bypass(
    int unit,
    uint8 is_on);

shr_error_e dnx_err_recovery_initialize(
    int unit);

int dnx_err_recovery_module_init(
    int unit);

shr_error_e dnx_err_recovery_deinitialize(
    int unit);

int dnx_err_recovery_module_deinit(
    int unit);

uint8 dnx_err_recovery_is_on_test(
    int unit);

shr_error_e dnx_err_recovery_tmp_allow(
    int unit,
    uint8 is_on);

shr_error_e dnx_rollback_journal_start(
    int unit);

shr_error_e dnx_rollback_journal_end(
    int unit,
    uint8 rollback_journal);

shr_error_e dnx_err_recovery_hw_access(
    int unit,
    uint8 is_on);

shr_error_e dnx_err_recovery_transaction_no_support_counter_inc(
    int unit);

shr_error_e dnx_err_recovery_transaction_no_support_counter_dec(
    int unit);

shr_error_e dnx_err_recovery_transaction_start(
    int unit,
    bcm_switch_module_t module_id);

shr_error_e dnx_err_recovery_transaction_end(
    int unit,
    bcm_switch_module_t module_id,
    int api_result);

shr_error_e dnx_err_recovery_transaction_invalidate(
    int unit);

shr_error_e dnx_err_recovery_unsupported_dbal_unsup_tbls_bypass(
    int unit,
    uint8 is_on);

shr_error_e dnx_rollback_journal_print_journal(
    int unit,
    dnx_rollback_journal_subtype_e type);

shr_error_e dnx_rollback_journal_logger_state_change(
    int unit,
    dnx_rollback_journal_subtype_e type);

shr_error_e dnx_rollback_journal_appl_init_state_change(
    int unit,
    uint8 is_init);

#endif
