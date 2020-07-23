
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_ERR_RECOVERY_MANAGER_UTILS_H

#define _DNX_ERR_RECOVERY_MANAGER_UTILS_H

#include <soc/types.h>
#include <soc/error.h>
#include <assert.h>
#include <sal/core/thread.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/recovery/rollback_journal_utils.h>

#define DNX_ERR_RECOVERY_TMP_EXCLUDED_EXTERNAL_THREADS_NOF 20

#define DNX_ERR_RECOVERY_INTERNAL_PERIODIC_EVENT_THREAD_NOF 20

typedef enum
{
    DNX_ERR_RECOVERY_INTERNAL_THREAD_DPC,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_IPROC_EVENT,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_COUNTER,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_CRPS_EVIC,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_LINKSCAN,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_PACKET_DMA,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_SBUS_DMA,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_IPOLL,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_RX,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_COMMON_RX,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_TX,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_PHY_PRBSSTAT,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_L2_FLUSH_TRAVERSE,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_NOF
} dnx_err_recovery_thread_internal_ids_e;

typedef struct dnx_err_recovery_thread_state_d
{

    sal_thread_t tid;

    uint32 disabled_counter;
} dnx_err_recovery_thread_state_t;

typedef struct dnx_er_threading_d
{

    uint8 is_initialized;

    sal_mutex_t mtx;

    dnx_err_recovery_thread_state_t journaling_thread;

    dnx_err_recovery_thread_state_t internal_threads[DNX_ERR_RECOVERY_INTERNAL_THREAD_NOF];

    uint32 internal_periodic_thread_count;

        dnx_err_recovery_thread_state_t
        internal_periodic_event_threads[DNX_ERR_RECOVERY_INTERNAL_PERIODIC_EVENT_THREAD_NOF];

    uint32 excluded_external_thread_count;

    dnx_err_recovery_thread_state_t excluded_external_threads[DNX_ERR_RECOVERY_TMP_EXCLUDED_EXTERNAL_THREADS_NOF];

    dnx_err_recovery_thread_state_t main_thread;

#ifdef DNX_ERR_RECOVERY_VALIDATION

    uint8 skip_validation;

    dnx_err_recovery_thread_state_t config_thread;
#endif

} dnx_er_threading_t;

void dnx_er_threading_perm_excluded_periodic_event_thread_add(
    int unit);

void dnx_er_threading_perm_excluded_thread_add(
    int unit,
    dnx_err_recovery_thread_internal_ids_e id);

void dnx_er_threading_perm_excluded_threads_add_all_units(
    dnx_err_recovery_thread_internal_ids_e id);

void dnx_er_threading_perm_excluded_periodic_event_thread_remove(
    int unit);

void dnx_er_threading_perm_excluded_thread_remove(
    int unit,
    dnx_err_recovery_thread_internal_ids_e id);

void dnx_er_threading_perm_excluded_threads_remove_all_units(
    dnx_err_recovery_thread_internal_ids_e id);

void dnx_er_threading_tmp_excluded_add(
    int unit);

void dnx_er_threading_tmp_excluded_remove(
    int unit);

uint8 dnx_er_threading_is_excluded_thread(
    int unit);

uint8 dnx_er_threading_is_main_thread(
    int unit);

#ifdef DNX_ERR_RECOVERY_VALIDATION

uint32 dnx_er_threading_test_skip_validation(
    int unit,
    uint8 is_on);
#endif

uint8 dnx_er_threading_is_journaling_thread(
    int unit);

shr_error_e dnx_er_threading_journaling_thread_set(
    int unit);

shr_error_e dnx_er_threading_journaling_thread_clear(
    int unit);

shr_error_e dnx_er_threading_initialize(
    int unit);

shr_error_e dnx_er_threading_deinitialize(
    int unit);

shr_error_e dnx_er_threading_no_parallel_transactions_verify(
    int unit);

#define DNX_ERR_RECOVERY_UTILS_EXCLUDED_PERIODIC_EVENT_THREAD_ADD(_unit)\
do {                                                                    \
    dnx_er_threading_perm_excluded_periodic_event_thread_add(_unit);    \
} while(0)

#define DNX_ERR_RECOVERY_UTILS_EXCLUDED_PERIODIC_EVENT_THREAD_REMOVE(_unit)\
do {                                                                       \
    dnx_er_threading_perm_excluded_periodic_event_thread_remove(_unit);    \
} while(0)

#define DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREAD_ADD(_unit, _thread_id)\
do {                                                                 \
    dnx_er_threading_perm_excluded_thread_add(_unit, _thread_id);    \
} while(0)

#define DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREAD_REMOVE(_unit, _thread_id)\
do {                                                                    \
    dnx_er_threading_perm_excluded_thread_remove(_unit, _thread_id);    \
} while(0)

#define DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREADS_ADD_ALL_UNITS(_thread_id)\
    dnx_er_threading_perm_excluded_threads_add_all_units(_thread_id)

#define DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREADS_REMOVE_ALL_UNITS(_thread_id)\
    dnx_er_threading_perm_excluded_threads_remove_all_units(_thread_id)

#define DNX_ER_THREADING_EXIT_IF_NOT_MAIN_THREAD(_unit)\
do{                                                    \
    if (!dnx_er_threading_is_main_thread(_unit)) {     \
        SHR_EXIT();                                    \
    }                                                  \
}while(0)

#define DNX_ER_THREADING_EXIT_IF_NOT_JOURNALING_THREAD(_unit)\
do{                                                          \
    if (!dnx_er_threading_is_journaling_thread(_unit)) {     \
        SHR_EXIT();                                          \
    }                                                        \
}while(0)

#define DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(_unit)                                  \
do{                                                                                                 \
    if (!dnx_er_threading_is_journaling_thread(_unit)) {                                            \
        LOG_CLI((BSL_META_U(_unit,                                                                  \
         "configuration error: two APIs are being called simultaneously for the same unit.\n")));   \
    }                                                                                               \
}while(0)

#define DNX_ER_THREADING_EXIT_IF_IS_EXCLUDED_THREAD(_unit)\
do{                                                       \
    if (dnx_er_threading_is_excluded_thread(_unit)) {     \
        SHR_EXIT();                                       \
    }                                                     \
}while(0)

#endif
