/**
 * \file tsn_thread.c
 * 
 *
 * TSN manager thread
 *
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TSN

/*
 * Include files.
 * {
 */

#include <bcm/tsn.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_tsn_access.h>
#include <shared/utilex/utilex_ptp_time.h>
#include <shared/utilex/utilex_u64.h>

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tsn.h>

#include "tsn_thread.h"
#include "tsn_counter.h"
#include "tsn_taf.h"
#include "tsn_tas.h"

/*
 * }
 */

/*
 * Defines:
 * {
 */

#define DNX_TSN_THREAD_NAME "TSN profile manager thread"

#define DNX_TSN_THREAD_INTERVAL_MAX_USEC  (1800000000)
#define DNX_TSN_THREAD_MAX_NOF_PIDS (DNX_DATA_MAX_TSN_TAF_NOF_GATES * DNX_DATA_MAX_TSN_TAF_MAX_PROFILES_PER_GATE + DNX_DATA_MAX_TSN_TAS_NOF_TAS_PORTS * DNX_DATA_MAX_TSN_TAS_MAX_PROFILES_PER_PORT)

 /*
  * }
  */

/**
* Structures:
* {
*/

typedef struct
{
    SHR_BITDCL bitmap[_SHR_BITDCLSIZE(DNX_TSN_THREAD_MAX_NOF_PIDS)];
} dnx_tsn_thread_pid_bmp_t;

/**
 * }
 */

/**
* Macros:
* {
*/

#define DNX_TSN_THREAD_PID_BMP_INIT(pid_bmp) \
    SHR_BITCLR_RANGE((pid_bmp)->bitmap, 0, DNX_TSN_THREAD_MAX_NOF_PIDS)

#define DNX_TSN_THREAD_PID_BMP_IS_EMPTY(pid_bmp) \
    SHR_BITNULL_RANGE((pid_bmp)->bitmap, 0, DNX_TSN_THREAD_MAX_NOF_PIDS)

#define DNX_TSN_THREAD_PID_BMP_BITSET(pid_bmp, type, pid)  \
    SHR_BITSET((pid_bmp)->bitmap, ((type == DNX_TSN_GATE_TYPE_TAF) ? 0 : DNX_DATA_MAX_TSN_TAF_NOF_GATES * DNX_DATA_MAX_TSN_TAF_MAX_PROFILES_PER_GATE) + pid)

#define DNX_TSN_THREAD_PID_BMP_IS_BITSET(pid_bmp, type, pid)  \
    SHR_IS_BITSET((pid_bmp)->bitmap, ((type == DNX_TSN_GATE_TYPE_TAF) ? 0 : DNX_DATA_MAX_TSN_TAF_NOF_GATES * DNX_DATA_MAX_TSN_TAF_MAX_PROFILES_PER_GATE) + pid)

#define DNX_TSN_THREAD_PID_BMP_BIT_ITER(pid_bmp, iter) \
    SHR_BIT_ITER((pid_bmp)->bitmap, DNX_TSN_THREAD_MAX_NOF_PIDS, iter)

#define DNX_TSN_THREAD_PID_BMP_ITER_PID_GET(iter) \
    (iter < DNX_DATA_MAX_TSN_TAF_NOF_GATES * DNX_DATA_MAX_TSN_TAF_MAX_PROFILES_PER_GATE ? iter : iter - DNX_DATA_MAX_TSN_TAF_NOF_GATES * DNX_DATA_MAX_TSN_TAF_MAX_PROFILES_PER_GATE)

#define DNX_TSN_THREAD_PID_BMP_ITER_TYPE_GET(iter) \
    (iter < DNX_DATA_MAX_TSN_TAF_NOF_GATES * DNX_DATA_MAX_TSN_TAF_MAX_PROFILES_PER_GATE ? DNX_TSN_GATE_TYPE_TAF : DNX_TSN_GATE_TYPE_TAS)

/**
 * }
 */

/**
 * \brief - TSN thread main logic
 */
shr_error_e dnx_tsn_thread_main(
    int unit,
    void *user_data);

/*
 * See .h file
 */
shr_error_e
dnx_tsn_thread_db_init(
    int unit)
{
    sw_state_ll_init_info_t init_info;
    SHR_FUNC_INIT_VARS(unit);

    /** create mutex */
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.mutex.create(unit));

    /** create pending list */
    sal_memset(&init_info, 0x0, sizeof(init_info));

    init_info.max_nof_elements = dnx_data_tsn.taf.nof_gates_get(unit) + dnx_data_tsn.tas.nof_tas_ports_get(unit);
    init_info.key_cmp_cb_name = "sw_state_sorted_list_cmp_uint64";

    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.create_empty(unit, &init_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - create TSN thread
 */
static shr_error_e
dnx_tsn_thread_create(
    int unit)
{
    shr_thread_manager_config_t tsn_thread_config_info;
    shr_thread_manager_handler_t tsn_thread_handler;
    SHR_FUNC_INIT_VARS(unit);

    /** Configure Thread and start */
    shr_thread_manager_config_t_init(&tsn_thread_config_info);
    tsn_thread_config_info.name = DNX_TSN_THREAD_NAME;
    tsn_thread_config_info.bsl_module = BSL_LOG_MODULE;
    tsn_thread_config_info.type = SHR_THREAD_MANAGER_TYPE_EVENT;
    tsn_thread_config_info.interval = sal_sem_FOREVER;
    tsn_thread_config_info.callback = dnx_tsn_thread_main;
    SHR_IF_ERR_EXIT(shr_thread_manager_create(unit, &tsn_thread_config_info, &tsn_thread_handler));

    /** Save Thread handler to SW-State */
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.tsn_thread_handler.set(unit, tsn_thread_handler));

    /** Wake up once to check pending list. Required for warm-boot but called also called at cold-boot for simplification */
    SHR_IF_ERR_EXIT(shr_thread_manager_trigger(tsn_thread_handler));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - destroy TSN thread
 */
static shr_error_e
dnx_tsn_thread_destory(
    int unit)
{

    shr_thread_manager_handler_t tsn_thread_handler;
    SHR_FUNC_INIT_VARS(unit);

    /** Get thread handle from SW-State */
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.tsn_thread_handler.get(unit, &tsn_thread_handler));

    /** Destroy thread */
    if (tsn_thread_handler != NULL)
    {
        SHR_IF_ERR_EXIT(shr_thread_manager_destroy(&tsn_thread_handler));
    }

    /** Set thread handle to SW-State to invalid */
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.tsn_thread_handler.set(unit, NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_thread_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_thread_create(unit));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_thread_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_thread_destory(unit));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Trigger TSN thread. Should be called after DB update.
 */
static shr_error_e
dnx_tsn_thread_trigger(
    int unit)
{
    shr_thread_manager_handler_t tsn_thread_handler;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.tsn_thread_handler.get(unit, &tsn_thread_handler));
    SHR_IF_ERR_EXIT(shr_thread_manager_trigger(tsn_thread_handler));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set pid's config change time (actual change time)
 */
shr_error_e
dnx_tsn_thread_config_change_time_set(
    int unit,
    dnx_tsn_gate_type_e type,
    int pid,
    bcm_ptp_timestamp_t * ptp_base_time)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case DNX_TSN_GATE_TYPE_TAF:
            SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.config_change_time.set(unit, pid, *ptp_base_time));
            break;
        case DNX_TSN_GATE_TYPE_TAS:
            /** NOTE: TSN thread supports only single core */
            SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.config_change_time.set(unit, 0, pid, *ptp_base_time));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "gate type not supported %d\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set pid's config (actual) change time
 */
shr_error_e
dnx_tsn_thread_config_change_time_get(
    int unit,
    dnx_tsn_gate_type_e type,
    int pid,
    bcm_ptp_timestamp_t * ptp_base_time)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case DNX_TSN_GATE_TYPE_TAF:
            SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.config_change_time.get(unit, pid, ptp_base_time));
            break;
        case DNX_TSN_GATE_TYPE_TAS:
            /** NOTE: TSN thread supports only single core */
            SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.config_change_time.get(unit, 0, pid, ptp_base_time));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "gate type not supported %d\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 */
static shr_error_e
dnx_tsn_thread_config_change_time_update(
    int unit,
    dnx_tsn_gate_type_e type,
    int pid)
{
    const bcm_cosq_tas_profile_t *profile_tas_ptr;
    const bcm_tsn_taf_profile_t *profile_taf_ptr;
    uint64 cycle_time_nsec;
    uint32 time_margin;
    bcm_ptp_timestamp_t config_change_time_prev, config_change_time;
    dnx_tsn_thread_pending_pid_t pending_pid;
    uint64 config_change_time_nsec;

    SHR_FUNC_INIT_VARS(unit);

    /** set pid info struct */
    pending_pid.type = type;
    pending_pid.pid = pid;

    switch (type)
    {
        case DNX_TSN_GATE_TYPE_TAS:
            SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.profile_data.get(unit, 0, pid, &profile_tas_ptr));
            COMPILER_64_SET(cycle_time_nsec, profile_tas_ptr->ptp_cycle_time_upper, profile_tas_ptr->ptp_cycle_time);
            break;
        case DNX_TSN_GATE_TYPE_TAF:
            SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_data.get(unit, pid, &profile_taf_ptr));
            COMPILER_64_SET(cycle_time_nsec, profile_taf_ptr->ptp_cycle_time_upper, profile_taf_ptr->ptp_cycle_time);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "gate type not supported %d\n", type);
    }

    /** get actual change time */
    SHR_IF_ERR_EXIT(dnx_tsn_thread_config_change_time_get(unit, type, pid, &config_change_time_prev));

    /** calculate new base time */
    time_margin =
        dnx_data_tsn.tsn_thread.wakeup_interval_nsec_get(unit) +
        dnx_data_tsn.tsn_thread.group_window_length_nsec_get(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_counter_config_change_time_get
                    (unit, &config_change_time_prev, cycle_time_nsec, time_margin, &config_change_time));

    /** set actual change time */
    SHR_IF_ERR_EXIT(dnx_tsn_thread_config_change_time_set(unit, type, pid, &config_change_time));

    /** translate base PTP time into nanosecs */
    SHR_IF_ERR_EXIT(utilex_ptp_time_to_nanoseconds_convert(unit, &config_change_time, &config_change_time_nsec));

    /** add pid to pending list */
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.add(unit, &config_change_time_nsec, &pending_pid));

    /* *INDENT-OFF* */
    LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, " Update Base Time for : Type %u, Pid %u [Base time =  " COMPILER_64_PRINTF_FORMAT "]\n"),
                               type, pid, COMPILER_64_PRINTF_VALUE(config_change_time_nsec)));
    /* *INDENT-ON* */

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reschedule base times in the past to the future
 *          1. Remove all pids in past time
 *          2. Calculate new base time for TAS pids and add back to pending list
 *          3. Calculate new base time for TAF pids and add back to pending list
 */
static shr_error_e
dnx_tsn_thread_base_time_reschedule(
    int unit,
    uint8 *is_update_done)
{
    int is_mutex_locked = FALSE;
    sw_state_ll_node_t node, next_node;
    uint64 curr_time_nsec, iter_time_nsec;
    dnx_tsn_thread_pending_pid_t pending_pid;
    dnx_tsn_thread_pid_bmp_t pid_bmp;
    dnx_tsn_gate_type_e type;
    int iter, pid;

    SHR_FUNC_INIT_VARS(unit);

    /** init pid_bmp */
    DNX_TSN_THREAD_PID_BMP_INIT(&pid_bmp);

    /** take mutex */
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.mutex.take(unit, sal_mutex_FOREVER));
    is_mutex_locked = TRUE;

    /** get the current time */
    SHR_IF_ERR_EXIT(dnx_tsn_counter_curr_time_get(unit, &curr_time_nsec));
    COMPILER_64_ADD_32(curr_time_nsec, dnx_data_tsn.tsn_thread.max_process_time_nsec_get(unit));

    /** get first element */
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.get_first(unit, &node));

    /** remove all pids in past time */
    while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
    {
        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.next_node(unit, node, &next_node));

        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.node_key(unit, node, (uint64 *) &iter_time_nsec));

        if (COMPILER_64_GE(iter_time_nsec, curr_time_nsec))
        {
            break;
        }

        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.
                        pending_list.node_value(unit, node, (dnx_tsn_thread_pending_pid_t *) & pending_pid));

        DNX_TSN_THREAD_PID_BMP_BITSET(&pid_bmp, pending_pid.type, pending_pid.pid);

        /*
         * remove from list 
         */
        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.remove_node(unit, node));

        node = next_node;
    }

    /** Calculate new base time for TAF pids and add back to pending list */
    DNX_TSN_THREAD_PID_BMP_BIT_ITER(&pid_bmp, iter)
    {
        type = DNX_TSN_THREAD_PID_BMP_ITER_TYPE_GET(iter);
        pid = DNX_TSN_THREAD_PID_BMP_ITER_PID_GET(iter);

        /** update the base time */
        SHR_IF_ERR_EXIT(dnx_tsn_thread_config_change_time_update(unit, type, pid));
    }

    *is_update_done = !(DNX_TSN_THREAD_PID_BMP_IS_EMPTY(&pid_bmp));

exit:
    if (is_mutex_locked)
    {
       /** give mutex */
        SHR_IF_ERR_CONT(dnx_tsn_db.tsn_thread.mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get group of pids to be handled
 *          Skip the pid in the past (will be rescheduled at next iteration).
 *          If the first future time is close enough, return all pids in group window.
 */
static shr_error_e
dnx_tsn_thread_pid_group_get(
    int unit,
    dnx_tsn_thread_pid_bmp_t * group_bmp)
{
    int is_mutex_locked = FALSE;
    sw_state_ll_node_t node;
    uint64 curr_time_nsec, iter_time_nsec, max_process_time_nsec, wakeup_time_nsec, group_window_nsec;
    dnx_tsn_thread_pending_pid_t pending_pid;

    SHR_FUNC_INIT_VARS(unit);

    /** init group_bmp  */
    DNX_TSN_THREAD_PID_BMP_INIT(group_bmp);

    /** init group window */
    COMPILER_64_ZERO(group_window_nsec);

    /** take mutex */
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.mutex.take(unit, sal_mutex_FOREVER));
    is_mutex_locked = TRUE;

    /** get the current time */
    SHR_IF_ERR_EXIT(dnx_tsn_counter_curr_time_get(unit, &curr_time_nsec));

    /** calc maximal process time */
    COMPILER_64_COPY(max_process_time_nsec, curr_time_nsec);
    COMPILER_64_ADD_32(max_process_time_nsec, dnx_data_tsn.tsn_thread.max_process_time_nsec_get(unit));

    /** calc wakeup time */
    COMPILER_64_COPY(wakeup_time_nsec, curr_time_nsec);
    COMPILER_64_ADD_32(wakeup_time_nsec, dnx_data_tsn.tsn_thread.wakeup_interval_nsec_get(unit));

    /** get first element */
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.get_first(unit, &node));

    while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
    {
        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.node_key(unit, node, (uint64 *) &iter_time_nsec));

        if (COMPILER_64_LT(iter_time_nsec, max_process_time_nsec))
        {
            /** skip pids in the past */
            SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.next_node(unit, node, &node));
            continue;
        }

        /** check the first future element */
        if (DNX_TSN_THREAD_PID_BMP_IS_EMPTY(group_bmp))
        {
            if (COMPILER_64_LE(iter_time_nsec, wakeup_time_nsec))
            {
                /** calc group window - First time + group window length */
                COMPILER_64_COPY(group_window_nsec, iter_time_nsec);
                COMPILER_64_ADD_32(group_window_nsec, dnx_data_tsn.tsn_thread.group_window_length_nsec_get(unit));

                /* *INDENT-OFF* */
                LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, " New Group [Current Time = " COMPILER_64_PRINTF_FORMAT ", First time = " COMPILER_64_PRINTF_FORMAT "]\n"),
                                           COMPILER_64_PRINTF_VALUE(curr_time_nsec), COMPILER_64_PRINTF_VALUE(iter_time_nsec)));
                /* *INDENT-ON* */
            }
            else
            {
                /** first time is too late */
                break;
            }
        }

        /** add the pids inside the group window */
        if (COMPILER_64_LE(iter_time_nsec, group_window_nsec))
        {
            SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.
                            pending_list.node_value(unit, node, (dnx_tsn_thread_pending_pid_t *) & pending_pid));

            DNX_TSN_THREAD_PID_BMP_BITSET(group_bmp, pending_pid.type, pending_pid.pid);

            /* *INDENT-OFF* */
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit, " Add to Group - Type %u, Pid %u\n"), pending_pid.type, pending_pid.pid));
            /* *INDENT-ON* */
        }
        else
        {
            /** Exit, as all pids in group window were collected. */
            break;
        }

        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.next_node(unit, node, &node));
    }

exit:
    if (is_mutex_locked)
    {
        /** give mutex */
        SHR_IF_ERR_CONT(dnx_tsn_db.tsn_thread.mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remove the pids of the group from pending list
 */
static shr_error_e
dnx_tsn_thread_pid_group_remove(
    int unit,
    dnx_tsn_thread_pid_bmp_t * group_bmp)
{
    int is_mutex_locked = FALSE;
    sw_state_ll_node_t node, next_node;
    dnx_tsn_thread_pending_pid_t pending_pid;

    SHR_FUNC_INIT_VARS(unit);

    /** take mutex */
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.mutex.take(unit, sal_mutex_FOREVER));
    is_mutex_locked = TRUE;

    /** get first element */
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.get_first(unit, &node));

    while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
    {
        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.next_node(unit, node, &next_node));

        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.
                        pending_list.node_value(unit, node, (dnx_tsn_thread_pending_pid_t *) & pending_pid));

        if (DNX_TSN_THREAD_PID_BMP_IS_BITSET(group_bmp, pending_pid.type, pending_pid.pid))
        {
            /** remove pid from pending list */
            SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.remove_node(unit, node));
        }

        node = next_node;
    }

exit:
    if (is_mutex_locked)
    {
        /** give mutex */
        SHR_IF_ERR_CONT(dnx_tsn_db.tsn_thread.mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - Update thread interval
 */
static shr_error_e
dnx_tsn_thread_interval_update(
    int unit)
{
    sw_state_ll_node_t node;
    int is_mutex_locked = FALSE;
    uint64 diff, first_time_nsec, curr_time_nsec;
    shr_thread_manager_handler_t tsn_thread_handler;
    int interval = sal_sem_FOREVER;

    SHR_FUNC_INIT_VARS(unit);

    /** take mutex */
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.mutex.take(unit, sal_mutex_FOREVER));
    is_mutex_locked = TRUE;

    /** get first node */
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.get_first(unit, &node));

    if (DNX_SW_STATE_LL_IS_NODE_VALID(node))
    {
        /** get first gate base time */
        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.node_key(unit, node, (uint64 *) &first_time_nsec));

        /** get the current time */
        SHR_IF_ERR_EXIT(dnx_tsn_counter_curr_time_get(unit, &curr_time_nsec));

        /*
         * if first time is smaller current time, wake up instantly.
         * else calculate the thread sleep interval according to the time difference
         */
        COMPILER_64_ADD_32(curr_time_nsec,
                           dnx_data_tsn.tsn_thread.wakeup_interval_nsec_get(unit) -
                           (dnx_data_tsn.tsn_thread.wakeup_interval_nsec_get(unit) / 100));

        if (COMPILER_64_LE(first_time_nsec, curr_time_nsec))
        {
            /** wake up instantly */
            SHR_IF_ERR_EXIT(dnx_tsn_thread_trigger(unit));
        }
        else
        {
           /** calc the difference between   */
            COMPILER_64_COPY(diff, first_time_nsec);
            COMPILER_64_SUB_64(diff, curr_time_nsec);

            /** Convert to usecs */
            COMPILER_64_UDIV_32(diff, 1000);

            if ((COMPILER_64_HI(diff) != 0) || (COMPILER_64_LO(diff) >= DNX_TSN_THREAD_INTERVAL_MAX_USEC))
            {
                /** set to the maximal interval */
                interval = DNX_TSN_THREAD_INTERVAL_MAX_USEC;
            }
            else
            {
                interval = COMPILER_64_LO(diff);
            }
        }
    }

    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.tsn_thread_handler.get(unit, &tsn_thread_handler));
    SHR_IF_ERR_EXIT(shr_thread_manager_interval_set(tsn_thread_handler, 1, interval));

    if (interval != sal_sem_FOREVER)
    {
        /* *INDENT-OFF* */
        LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, " Interval = %d (usec) [Current Time = " COMPILER_64_PRINTF_FORMAT ", First time = " COMPILER_64_PRINTF_FORMAT "] \n"),
                                   interval, COMPILER_64_PRINTF_VALUE(curr_time_nsec), COMPILER_64_PRINTF_VALUE(first_time_nsec)));
        /* *INDENT-ON* */
    }
    else
    {
        LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, " Interval = FOREVER (Empty List)\n")));
    }

exit:
    if (is_mutex_locked)
    {
        /** give mutex */
        SHR_IF_ERR_CONT(dnx_tsn_db.tsn_thread.mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - TSN thread main logic
 *
 * Sequence:
 * 1. Reschedule base times at the past into the future
 * 2. Get group of pids to be handled
 * 3. Update gate control list / gate params HW
 * 4. Remove the pids of the group from pending list
 * 5. Update thread interval
 */
shr_error_e
dnx_tsn_thread_main(
    int unit,
    void *user_data)
{
    dnx_tsn_thread_pid_bmp_t group_bmp;
    uint8 is_update_done = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     *  1. Reschedule base times in the past to a future time
     *     Exit, if updated was done.
     */
    SHR_IF_ERR_EXIT(dnx_tsn_thread_base_time_reschedule(unit, &is_update_done));
    if (is_update_done)
    {
        SHR_IF_ERR_EXIT(dnx_tsn_thread_trigger(unit));
        SHR_EXIT();
    }

    /*
     *  2. Get group of pids to be handled
     */
    SHR_IF_ERR_EXIT(dnx_tsn_thread_pid_group_get(unit, &group_bmp));

    if (!DNX_TSN_THREAD_PID_BMP_IS_EMPTY(&group_bmp))
    {
        /*
         *  3. Update gate control list / gate params HW
         */

        /*
         *  4. Remove the pids of the group from pending list
         */
        SHR_IF_ERR_EXIT(dnx_tsn_thread_pid_group_remove(unit, &group_bmp));
    }

    /*
     * 5. Update thread interval
     */
    SHR_IF_ERR_EXIT(dnx_tsn_thread_interval_update(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - add pid to pending list
 */
static shr_error_e
dnx_tsn_thread_pending_list_add(
    int unit,
    bcm_ptp_timestamp_t * ptp_base_time,
    dnx_tsn_gate_type_e type,
    int pid)
{
    int is_mutex_locked = FALSE;
    dnx_tsn_thread_pending_pid_t pending_pid;
    uint64 base_time_nsec;

    SHR_FUNC_INIT_VARS(unit);

    /** translate base PTP time into nanosecs */
    SHR_IF_ERR_EXIT(utilex_ptp_time_to_nanoseconds_convert(unit, ptp_base_time, &base_time_nsec));

    /** set pid info struct */
    pending_pid.type = type;
    pending_pid.pid = pid;

    /** take mutex */
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.mutex.take(unit, sal_mutex_FOREVER));
    is_mutex_locked = TRUE;

    /** add pid to queue */
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.add(unit, &base_time_nsec, &pending_pid));

exit:
    if (is_mutex_locked)
    {
        /** give mutex */
        SHR_IF_ERR_CONT(dnx_tsn_db.tsn_thread.mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}

/*
 * \brief - remove all gate's pending pids
 */
static shr_error_e
dnx_tsn_thread_pending_list_remove_all(
    int unit,
    dnx_tsn_gate_type_e type,
    int gate_id)
{
    int is_mutex_locked = FALSE;
    dnx_tsn_thread_pending_pid_t pending_pid;
    int pid_gate_id;
    sw_state_ll_node_t node, next_node;

    SHR_FUNC_INIT_VARS(unit);

    /** take mutex */
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.mutex.take(unit, sal_mutex_FOREVER));
    is_mutex_locked = TRUE;

    /** remove gate's pending profile (one pending profile per gate is allowed at at time) */
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.get_first(unit, &node));

    while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
    {
        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.next_node(unit, node, &next_node));

        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.
                        pending_list.node_value(unit, node, (dnx_tsn_thread_pending_pid_t *) & pending_pid));

        pid_gate_id = pending_pid.pid / dnx_data_tsn.taf.max_profiles_per_gate_get(unit);

        if (pending_pid.type == type && pid_gate_id == gate_id)
        {
            SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.remove_node(unit, node));
            break;
        }

        node = next_node;
    }

exit:
    if (is_mutex_locked)
    {
        /** give mutex */
        SHR_IF_ERR_CONT(dnx_tsn_db.tsn_thread.mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_thread_profile_commit(
    int unit,
    bcm_ptp_timestamp_t * ptp_base_time,
    dnx_tsn_gate_type_e type,
    int pid)
{
    SHR_FUNC_INIT_VARS(unit);

    /** set the config_change_time (may be updated later by thread) */
    SHR_IF_ERR_EXIT(dnx_tsn_thread_config_change_time_set(unit, type, pid, ptp_base_time));

    /** add pid to pending list */
    SHR_IF_ERR_EXIT(dnx_tsn_thread_pending_list_add(unit, ptp_base_time, type, pid));

    /** trigger thread */
    SHR_IF_ERR_EXIT(dnx_tsn_thread_trigger(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_thread_profile_reset_all(
    int unit,
    dnx_tsn_gate_type_e type,
    int gate_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /** remove all gate's pids from pending list */
    SHR_IF_ERR_EXIT(dnx_tsn_thread_pending_list_remove_all(unit, type, gate_id));

    /** trigger thread */
    SHR_IF_ERR_EXIT(dnx_tsn_thread_trigger(unit));

exit:
    SHR_FUNC_EXIT;
}
