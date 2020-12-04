/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file utilex_framework.c
 *
 * All common utilities related to framework.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SHAREDSWDNX_UTILSDNX

/*************
* INCLUDES  *
 */
/*
 * {
 */

#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_os_interface.h>
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <sal/types.h>

/*
 * }
 */
#ifdef UTILEX_LL_TIMER
/** { */
/*************
* INCLUDES  *
 */
/*
 * {
 */
#include <sal/core/time.h>
#include <sal/core/libc.h>
/*
 * }
 */
/*************
* GLOBALS    *
 */
/*
 * {
 */
/**
 * The timers' groups
 * This is not such an elegant initialization but it goes into effect as
 * the executable is loaded.
 */
UTILEX_LL_TIMER_GROUP Utilex_ll_timer_group[UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS] = {
    {UTILEX_LL_TIMER_EMPTY_GROUP_NAME, FALSE, FALSE, 0}
    ,
    {UTILEX_LL_TIMER_EMPTY_GROUP_NAME, FALSE, FALSE, 0}
    ,
    {UTILEX_LL_TIMER_EMPTY_GROUP_NAME, FALSE, FALSE, 0}
    ,
    {UTILEX_LL_TIMER_EMPTY_GROUP_NAME, FALSE, FALSE, 0}
};
/**
 *  The function timers
 */
UTILEX_LL_TIMER_FUNCTION Utilex_ll_timer_cnt[UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS][UTILEX_LL_TIMER_MAX_NOF_TIMERS];
/*
 * }
 */
/**********************
* PACKAGE PROCEDURES  *
 */
/*
 * {
 */

uint32
utilex_ll_get_time_in_ms(
    void)
{
    uint32 seconds, nano_seconds;

    utilex_os_get_time(&seconds, &nano_seconds);
    return seconds * 1000 + nano_seconds / 1000000;
}
/*
 * Allocate a free timers' group (if there is any)
 */
shr_error_e
utilex_ll_timer_group_allocate(
    char group_name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME],
    uint32 *timers_group_p)
{
    shr_error_e ret;
    uint32 ii;
    UTILEX_LL_TIMER_GROUP *utilex_ll_timer_group;

    ret = _SHR_E_NONE;
    if (!timers_group_p)
    {
        ret = _SHR_E_PARAM;
        goto exit;
    }
    /*
     * This value will be returned if no unlocked group is found.
     */
    *timers_group_p = UTILEX_LL_TIMER_NO_GROUP;
    utilex_ll_timer_group = &Utilex_ll_timer_group[0];
    for (ii = 0; ii < UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS; ii++, utilex_ll_timer_group++)
    {
        if (utilex_ll_timer_group->locked)
        {
            continue;
        }
        /*
         * A free group has been found. Quit.
         */
        sal_strncpy(utilex_ll_timer_group->name, group_name, UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME);
        utilex_ll_timer_group->name[sizeof(utilex_ll_timer_group->name) - 1] = 0;
        utilex_ll_timer_group->locked = TRUE;
        *timers_group_p = ii;
        break;
    }
exit:
    return (ret);
}

/*
 * Free specified timers' group (if it is locked)
 */
shr_error_e
utilex_ll_timer_group_free(
    uint32 timers_group)
{
    shr_error_e ret;
    UTILEX_LL_TIMER_GROUP *utilex_ll_timer_group;

    ret = _SHR_E_NONE;
    if (timers_group >= UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS)
    {
        /*
         * Illegal timers' group identifier: Larger than max
         */
        ret = _SHR_E_PARAM;
        goto exit;
    }
    utilex_ll_timer_group = &Utilex_ll_timer_group[timers_group];
    if (!utilex_ll_timer_group->locked)
    {
        ret = _SHR_E_UNAVAIL;
        goto exit;
    }
    if (strlen(UTILEX_LL_TIMER_EMPTY_GROUP_NAME) > UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME)
    {
        sal_strncpy(utilex_ll_timer_group->name, UTILEX_LL_TIMER_EMPTY_GROUP_NAME,
                    UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME);
        utilex_ll_timer_group->name[sizeof(utilex_ll_timer_group->name) - 1] = 0;
    }
    else
    {
        sal_strcpy(utilex_ll_timer_group->name, UTILEX_LL_TIMER_EMPTY_GROUP_NAME);
    }
    utilex_ll_timer_group->locked = FALSE;
exit:
    return (ret);
}

/*
 *  Init the counters
 */
shr_error_e
utilex_ll_timer_clear_all(
    uint32 timers_group)
{
    uint32 cnt_ndx;
    shr_error_e ret;
    UTILEX_LL_TIMER_FUNCTION *utilex_ll_timer_function;

    ret = _SHR_E_NONE;
    if (timers_group >= UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS)
    {
        ret = _SHR_E_PARAM;
        goto exit;
    }
    utilex_ll_timer_function = &Utilex_ll_timer_cnt[timers_group][0];
    for (cnt_ndx = 0; cnt_ndx < UTILEX_LL_TIMER_MAX_NOF_TIMERS; ++cnt_ndx, utilex_ll_timer_function++)
    {
        sal_strncpy(utilex_ll_timer_function->name, "no_function", UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_NAME);
        utilex_ll_timer_function->nof_hits = 0;
        utilex_ll_timer_function->total_time = 0;
        utilex_ll_timer_function->start_timer = 0;
        utilex_ll_timer_function->active = 0;
    }
    Utilex_ll_timer_group[timers_group].is_any_active = FALSE;
    Utilex_ll_timer_group[timers_group].timer_total = 0;
exit:
    return (ret);
}

shr_error_e
utilex_ll_timer_stop_all(
    uint32 timers_group)
{
    uint32 cnt_ndx;
    shr_error_e ret;
    if (timers_group >= UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS)
    {
        ret = _SHR_E_PARAM;
        goto exit;
    }
    for (cnt_ndx = 0; cnt_ndx < UTILEX_LL_TIMER_MAX_NOF_TIMERS; ++cnt_ndx)
    {
        if (Utilex_ll_timer_cnt[timers_group][cnt_ndx].active)
        {
            utilex_ll_timer_stop(timers_group, cnt_ndx);
        }
    }
    ret = _SHR_E_NONE;
exit:
    return (ret);
}

shr_error_e
utilex_ll_timer_set(
    char name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_NAME],
    uint32 timers_group,
    uint32 timer_ndx)
{
    shr_error_e ret;
    UTILEX_LL_TIMER_FUNCTION *utilex_ll_timer_function;

    if (timers_group >= UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS)
    {
        ret = _SHR_E_PARAM;
        goto exit;
    }
    if (timer_ndx >= UTILEX_LL_TIMER_MAX_NOF_TIMERS)
    {
        /*
         * Invalid input. Exit with error.
         */
        ret = _SHR_E_PARAM;
        goto exit;
    }
    utilex_ll_timer_function = &(Utilex_ll_timer_cnt[timers_group][timer_ndx]);
    if (utilex_ll_timer_function->active)
    {
        /*
         * Invalid invocation: Timer has already been started/restarted.
         * Exit with error.
         */
        ret = _SHR_E_UNAVAIL;
        goto exit;
    }
    if (utilex_ll_timer_function->nof_hits == 0)
    {
        /*
         * First invocation: Set the ASCII name of the counter.
         */
        sal_strncpy(utilex_ll_timer_function->name, name, UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_NAME);
        utilex_ll_timer_function->name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_NAME - 1] = 0;
    }
    /*
     * Mark the start time to be used for calculating the active run time.
     */
    utilex_ll_timer_function->start_timer = sal_time_usecs();
    /*
     * Make sure that even if one timer has been activated, 'any activated' flag will be set.
     */
    Utilex_ll_timer_group[timers_group].is_any_active = TRUE;
    utilex_ll_timer_function->active = TRUE;
    ret = _SHR_E_NONE;
exit:
    return (ret);
}
/*
 * Get info on specified timer, regardless of whether it (or its group) is active.
 */
shr_error_e
utilex_ll_timer_get_info(
    uint32 timers_group,
    uint32 timer_ndx,
    int *is_active,
    char **name,
    uint32 *nof_hits,
    uint32 *total_time)
{
    shr_error_e ret;
    UTILEX_LL_TIMER_FUNCTION *utilex_ll_timer_function;

    if ((is_active == NULL) || (nof_hits == NULL) || (total_time == NULL) || (name == NULL))
    {
        /*
         * Illegal input. Null pointer. Exit with error.
         */
        ret = _SHR_E_PARAM;
        goto exit;
    }
    if (timers_group >= UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS)
    {
        /*
         * Invalid input. Exit with error.
         */
        ret = _SHR_E_PARAM;
        goto exit;
    }
    if (timer_ndx >= UTILEX_LL_TIMER_MAX_NOF_TIMERS)
    {
        /*
         * Invalid input. Exit with error.
         */
        ret = _SHR_E_PARAM;
        goto exit;
    }
    utilex_ll_timer_function = &(Utilex_ll_timer_cnt[timers_group][timer_ndx]);
    *is_active = utilex_ll_timer_function->active;
    *name = utilex_ll_timer_function->name;
    *nof_hits = utilex_ll_timer_function->nof_hits;
    *total_time = utilex_ll_timer_function->total_time;
    ret = _SHR_E_NONE;
exit:
    return (ret);
}
/*
 * Get info on specified timers' group, regardless of whether it is locked.
 */
shr_error_e
utilex_ll_timers_group_get_info(
    uint32 timers_group,
    int *is_locked,
    char **name,
    int *is_any_active,
    uint32 *total_time_all_timers)
{
    shr_error_e ret;
    UTILEX_LL_TIMER_GROUP *utilex_ll_timer_group;

    if ((is_locked == NULL) || (name == NULL) || (is_any_active == NULL) || (total_time_all_timers == NULL))
    {
        /*
         * Illegal input. Null pointer. Exit with error.
         */
        ret = _SHR_E_PARAM;
        goto exit;
    }
    if (timers_group >= UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS)
    {
        /*
         * Invalid input. Exit with error.
         */
        ret = _SHR_E_PARAM;
        goto exit;
    }
    utilex_ll_timer_group = &(Utilex_ll_timer_group[timers_group]);
    *is_locked = utilex_ll_timer_group->locked;
    *name = utilex_ll_timer_group->name;
    *is_any_active = utilex_ll_timer_group->is_any_active;
    *total_time_all_timers = utilex_ll_timer_group->timer_total;
    ret = _SHR_E_NONE;
exit:
    return (ret);
}

shr_error_e
utilex_ll_timers_group_name_get(
    uint32 timers_group,
    char **name)
{
    shr_error_e ret;
    UTILEX_LL_TIMER_GROUP *utilex_ll_timer_group;

    if (name == NULL)
    {
        /*
         * Illegal input. Null pointer. Exit with error.
         */
        ret = _SHR_E_PARAM;
        goto exit;
    }
    if (timers_group >= UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS)
    {
        /*
         * Invalid input. Exit with error.
         */
        ret = _SHR_E_PARAM;
        goto exit;
    }
    utilex_ll_timer_group = &(Utilex_ll_timer_group[timers_group]);
    *name = utilex_ll_timer_group->name;
    ret = _SHR_E_NONE;
exit:
    return (ret);
}

/*
 *  Stop the timer associated with the function
 */
shr_error_e
utilex_ll_timer_stop(
    uint32 timers_group,
    uint32 timer_ndx)
{
    uint32 new_delta;
    shr_error_e ret;
    UTILEX_LL_TIMER_FUNCTION *utilex_ll_timer_function;

    if (timers_group >= UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS)
    {
        /*
         * Invalid input. Exit with error.
         */
        ret = _SHR_E_PARAM;
        goto exit;
    }
    if (timer_ndx >= UTILEX_LL_TIMER_MAX_NOF_TIMERS)
    {
        /*
         * Invalid input. Exit with error.
         */
        ret = _SHR_E_PARAM;
        goto exit;
    }
    if (!Utilex_ll_timer_cnt[timers_group][timer_ndx].active)
    {
        /*
         * Invalid invocation: Timer has not been started/restarted.
         * Exit with error.
         */
        ret = _SHR_E_UNAVAIL;
        goto exit;
    }
    utilex_ll_timer_function = &Utilex_ll_timer_cnt[timers_group][timer_ndx];
    new_delta = sal_time_usecs() - utilex_ll_timer_function->start_timer;
    utilex_ll_timer_function->nof_hits += 1;
    utilex_ll_timer_function->total_time += new_delta;
    utilex_ll_timer_function->start_timer = 0;

    Utilex_ll_timer_group[timers_group].timer_total += new_delta;
    utilex_ll_timer_function->active = FALSE;
    ret = _SHR_E_NONE;
exit:
    return (ret);
}

/*
 *  Print all the results for one timer's group
 */
shr_error_e
utilex_ll_timer_print_all(
    uint32 timers_group)
{
    uint32 cnt_ndx;
    COMPILER_UINT64 total_time_1000, total_time_100, timers_total;
    shr_error_e ret;
    char *group_name;
    int is_locked, is_any_active;
    uint32 total_time_all_timers;

    if (timers_group >= UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS)
    {
        /*
         * Invalid input. Exit with error.
         */
        LOG_CLI((BSL_META("Error on input. timers_group (%d) is larger than max (%d)\n\r"),
                 (int) timers_group, UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS - 1));
        ret = _SHR_E_PARAM;
        goto exit;
    }
    ret =
        utilex_ll_timers_group_get_info(timers_group, &is_locked, &group_name, &is_any_active, &total_time_all_timers);
    if (ret != _SHR_E_NONE)
    {
        /*
         * utilex_ll_timers_group_get_info() failed. Exit with error.
         */
        LOG_CLI((BSL_META("Error on utilex_ll_timers_group_get_info. Error code %d\n\r"), (int) ret));
        goto exit;
    }
    LOG_CLI((BSL_META("\r\n")));
    if (total_time_all_timers == 0)
    {
        LOG_CLI((BSL_META("No timers were hit, total measured execution time: 0\n\r")));
    }
    else
    {
        LOG_CLI((BSL_META(" Execution Time Measurements.\n\r")));
        LOG_CLI((BSL_META(" Note: Percents are calculated relative to to the total measured time,\n\r")));
        LOG_CLI((BSL_META(" not accounting for overlapping timers\n\r")));
        LOG_CLI((BSL_META("\r\n")));
        LOG_CLI((BSL_META(" Name of GROUP: %s\r\n"), group_name));
        LOG_CLI((BSL_META
                 (" +-----------------------------------------------------------------------------------------+\n\r")));
        LOG_CLI((BSL_META
                 (" | Timer Name                             |Hit Count |Total Time[us] |Per Hit[us] |Percent |\n\r")));
        LOG_CLI((BSL_META
                 (" +-----------------------------------------------------------------------------------------+\n\r")));
        COMPILER_64_SET(timers_total, 0, total_time_all_timers);
        for (cnt_ndx = 0; cnt_ndx < UTILEX_LL_TIMER_MAX_NOF_TIMERS; ++cnt_ndx)
        {
            int is_active;
            uint32 nof_hits, total_time;
            char *name;
            ret = utilex_ll_timer_get_info(timers_group, cnt_ndx, &is_active, &name, &nof_hits, &total_time);
            if (ret != _SHR_E_NONE)
            {
                /*
                 * utilex_ll_timer_get_info() failed. Exit with error.
                 */
                LOG_CLI((BSL_META("Error on utilex_ll_timer_get_info. Error code %d\n\r"), (int) ret));
                goto exit;
            }
            if (nof_hits != 0)
            {
                COMPILER_64_SET(total_time_1000, 0, total_time);
                COMPILER_64_SET(total_time_100, 0, total_time);
                COMPILER_64_UMUL_32(total_time_1000, 1000);
                COMPILER_64_UMUL_32(total_time_100, 100);
                COMPILER_64_UDIV_64(total_time_1000, timers_total);
                COMPILER_64_UDIV_64(total_time_100, timers_total);
                LOG_CLI((BSL_META(" |%-40s| %-9d|%-15d|%-12d|%3d.%1d%%  |\n\r"),
                         name,
                         nof_hits,
                         total_time,
                         total_time / nof_hits,
                         COMPILER_64_LO(total_time_100),
                         COMPILER_64_LO(total_time_1000) - COMPILER_64_LO(total_time_100) * 10));
                LOG_CLI((BSL_META
                         (" +-----------------------------------------------------------------------------------------+\n\r")));
            }
        }
        LOG_CLI((BSL_META(" Total time: %u[us]\n\r"), total_time_all_timers));
    }   /* Timer hits != 0 */
    ret = _SHR_E_NONE;
exit:
    return (ret);
}
shr_error_e
utilex_ll_timer_test_01(
    void)
{
    uint32 timer_ndx[2];
    uint32 ii, work_reg;
    uint32 timers_group;
    shr_error_e err, ret;
    char group_name[UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS][UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME];
    ret = _SHR_E_NONE;
    {
        /*
         * Allocate maximal number of timers' goups.
         * We assume here that all groups are free, at this point.
         */
        for (ii = 0; ii < UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS; ii++)
        {
            sal_sprintf(group_name[ii], "%s%d", "Group", (int) ii);
            err = utilex_ll_timer_group_allocate(group_name[ii], &timers_group);
            if (err != _SHR_E_NONE)
            {
                LOG_CLI((BSL_META("Allocation of group has failed after %d attempts with error code %d.\n\r"),
                         ii + 1, (int) err));
                ret = _SHR_E_INTERNAL;
                goto exit;
            }
            else
            {
                if (timers_group == UTILEX_LL_TIMER_NO_GROUP)
                {
                    LOG_CLI((BSL_META("Allocation of group has failed after %d attempts. Expected to be able"
                                      " to allocate %d groups.\n\r"), ii + 1, UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS));
                    ret = _SHR_E_INTERNAL;
                    goto exit;
                }
            }
        }
        /*
         * Try to allocate one more. Expect fail.
         */
        err = utilex_ll_timer_group_allocate(group_name[0], &timers_group);
        if (err != _SHR_E_NONE)
        {
            LOG_CLI((BSL_META("Allocation of group has failed with error code %d.\n\r"), (int) err));
            ret = _SHR_E_INTERNAL;
            goto exit;
        }
        else
        {
            if (timers_group != UTILEX_LL_TIMER_NO_GROUP)
            {
                LOG_CLI((BSL_META("Allocation of group has succeeded after %d allocations. Expected to be fail\n\r"),
                         UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS));
                ret = _SHR_E_INTERNAL;
                goto exit;
            }
        }
        for (timers_group = 0; timers_group < UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS; timers_group++)
        {
            err = utilex_ll_timer_group_free(timers_group);
            if (err != _SHR_E_NONE)
            {
                LOG_CLI((BSL_META("Free of allocated group (%d) has failed. Expected to be able"
                                  " to free %d groups.\n\r"), ii, UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS));
                ret = _SHR_E_INTERNAL;
                goto exit;
            }
        }
        /*
         * Try to free one more. Expect fail.
         */
        timers_group = 0;
        err = utilex_ll_timer_group_free(timers_group);
        if (err == _SHR_E_NONE)
        {
            LOG_CLI((BSL_META("Free of unlocked group (%d) has succeeded. Expected to fail.\n\r"), (int) timers_group));
            ret = _SHR_E_INTERNAL;
            goto exit;
        }

    }
    {
        timer_ndx[0] = 0;
        timer_ndx[1] = 1;
        timers_group = 0;
        sal_sprintf(group_name[0], "%s%d", "TEST", (int) timers_group);
        err = utilex_ll_timer_group_allocate(group_name[0], &timers_group);
        if (err != _SHR_E_NONE)
        {
            LOG_CLI((BSL_META("Allocation of group %s has failed with error code %d.\n\r"), group_name[0], (int) err));
            ret = _SHR_E_INTERNAL;
            goto exit;
        }
        else
        {
            if (timers_group == UTILEX_LL_TIMER_NO_GROUP)
            {
                LOG_CLI((BSL_META("Allocation of group %s has failed. Expected to succeed since all are free.\n\r"),
                         group_name[0]));
                ret = _SHR_E_INTERNAL;
                goto exit;
            }
        }
        utilex_ll_timer_clear_all(timers_group);
        /*
         * Activate timer '0' and stop at end of test.
         */
        utilex_ll_timer_set("First Timer", timers_group, timer_ndx[0]);
        work_reg = 0;
        for (ii = 0; ii < 10000; ii++)
        {
            work_reg += ii;
        }
        /*
         * Activate timer '1' twicw (and stop after each).
         */
        utilex_ll_timer_set("Second Timer", timers_group, timer_ndx[1]);
        work_reg = 0;
        for (ii = 0; ii < 10000; ii++)
        {
            work_reg += ii;
        }
        utilex_ll_timer_stop(timers_group, timer_ndx[1]);

        utilex_ll_timer_set("Second Timer", timers_group, timer_ndx[1]);
        work_reg = 0;
        for (ii = 0; ii < 10000; ii++)
        {
            work_reg += ii;
        }
        utilex_ll_timer_stop(timers_group, timer_ndx[1]);
        work_reg = 0;
        for (ii = 0; ii < 10000; ii++)
        {
            work_reg += ii;
        }
        utilex_ll_timer_stop(timers_group, timer_ndx[0]);
        utilex_ll_timer_stop_all(timers_group);
        utilex_ll_timer_print_all(timers_group);
        {
            /*
             * Expect timer '0' to be activated once
             * Expect timer '1' to be activated twice
             *
             * Expect timer '0' to have total time which is about twice
             * that of timer '1'.
             * NOTE that we assume here that interrupts or other tasks are not
             * active enough to change the results. This assumption may not be true
             * under some conditions.
             */
            uint32 range_time[2];
            int is_active[2];
            uint32 nof_hits[2], total_time[2];
            char *name[2];

            ret =
                utilex_ll_timer_get_info(timers_group, timer_ndx[0], &is_active[0], &name[0], &nof_hits[0],
                                         &total_time[0]);
            if (ret != _SHR_E_NONE)
            {
                /*
                 * utilex_ll_timer_get_info() failed. Exit with error.
                 */
                LOG_CLI((BSL_META("Error on utilex_ll_timer_get_info. Error code %d\n\r"), (int) ret));
                goto exit;
            }
            ret =
                utilex_ll_timer_get_info(timers_group, timer_ndx[1], &is_active[1], &name[1], &nof_hits[1],
                                         &total_time[1]);
            if (ret != _SHR_E_NONE)
            {
                /*
                 * utilex_ll_timer_get_info() failed. Exit with error.
                 */
                LOG_CLI((BSL_META("Error on utilex_ll_timer_get_info. Error code %d\n\r"), (int) ret));
                goto exit;
            }

            if (nof_hits[0] != 1)
            {
                LOG_CLI((BSL_META
                         ("Expected nof hits for timer %s is '1' but recorded value is %d. Error in timers package!\n\r"),
                         name[0], nof_hits[0]));
            }
            if (nof_hits[1] != 2)
            {
                LOG_CLI((BSL_META
                         ("Expected nof hits for timer %s is '2' but recorded value is %d. Error in timers package!\n\r"),
                         name[1], nof_hits[1]));
            }
            range_time[0] = (total_time[0] * 9) / 10;
            range_time[1] = (total_time[0] * 11) / 10;
            if ((total_time[1] * 2) < range_time[0])
            {
                LOG_CLI((BSL_META("Expected twice total time of timer %s to be larger than %u"
                                  " but recorded value is %u. Error in timers package!\n\r"),
                         name[1], range_time[0], total_time[1] * 2));
            }
            if ((total_time[1] * 2) > range_time[1])
            {
                LOG_CLI((BSL_META("Expected twice total time of timer %s to be smaller than %u"
                                  " but recorded value is %u. Error in timers package!\n\r"),
                         name[1], range_time[1], total_time[1] * 2));
            }
        }
        utilex_ll_timer_clear_all(timers_group);
        err = utilex_ll_timer_group_free(timers_group);
        if (err != _SHR_E_NONE)
        {
            LOG_CLI((BSL_META("Free of locked group (%d,%s) has failed. Expected to succeed.\n\r"),
                     (int) timers_group, group_name[0]));
            ret = _SHR_E_INTERNAL;
            goto exit;
        }
    }
exit:
    return (ret);
}
/*
 * }
 */

/** } UTILEX_LL_TIMER */
#endif

shr_error_e
utilex_pp_mac_address_struct_to_long(
    uint8 mac_address[UTILEX_PP_MAC_ADDRESS_NOF_U8],
    uint32 mac_add_long[UTILEX_PP_MAC_ADDRESS_NOF_UINT32S])
{
    uint32 tmp;
    uint32 char_indx, long_indx, write_to;
    SHR_FUNC_INIT_VARS(NO_UNIT);

    for (long_indx = 0; long_indx < UTILEX_PP_MAC_ADDRESS_NOF_UINT32S; long_indx++)
    {
        mac_add_long[long_indx] = 0;
    }
    write_to = 0;
    long_indx = 0;

    for (char_indx = 0; char_indx < UTILEX_PP_MAC_ADDRESS_NOF_U8; ++char_indx)
    {
        tmp = mac_address[UTILEX_PP_MAC_ADDRESS_NOF_U8 - char_indx - 1];
        mac_add_long[long_indx] |=
            UTILEX_SET_BITS_RANGE(tmp,
                                  (UTILEX_NOF_BITS_IN_CHAR * (write_to + 1) - 1), UTILEX_NOF_BITS_IN_CHAR * write_to);
        ++write_to;
        if (write_to >= sizeof(uint32))
        {
            long_indx++;
            write_to = 0;
        }
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
utilex_pp_mac_address_long_to_struct(
    uint32 mac_add_long[UTILEX_PP_MAC_ADDRESS_NOF_UINT32S],
    uint8 mac_address[UTILEX_PP_MAC_ADDRESS_NOF_U8])
{
    uint32 char_indx, long_indx, read_from;
    SHR_FUNC_INIT_VARS(NO_UNIT);

    read_from = 0;
    long_indx = 0;
    for (char_indx = 0; char_indx < UTILEX_PP_MAC_ADDRESS_NOF_U8; ++char_indx)
    {
        mac_address[UTILEX_PP_MAC_ADDRESS_NOF_U8 - char_indx - 1] =
            (uint8) UTILEX_GET_BITS_RANGE(mac_add_long[long_indx],
                                          UTILEX_NOF_BITS_IN_CHAR * (read_from + 1) - 1,
                                          UTILEX_NOF_BITS_IN_CHAR * read_from);
        ++read_from;
        if (read_from >= sizeof(uint32))
        {
            long_indx++;
            read_from = 0;
        }
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
utilex_pp_ipv6_address_struct_to_long(
    uint8 ipv6_address[UTILEX_PP_IPV6_ADDRESS_NOF_U8],
    uint32 ipv6_add_long[UTILEX_PP_IPV6_ADDRESS_NOF_UINT32S])
{
    uint32 tmp;
    uint32 char_indx, long_indx, write_to;
    SHR_FUNC_INIT_VARS(NO_UNIT);

    for (long_indx = 0; long_indx < UTILEX_PP_IPV6_ADDRESS_NOF_UINT32S; long_indx++)
    {
        ipv6_add_long[long_indx] = 0;
    }

    write_to = 0;
    long_indx = 0;

    for (char_indx = 0; char_indx < UTILEX_PP_IPV6_ADDRESS_NOF_U8; ++char_indx)
    {
        tmp = ipv6_address[UTILEX_PP_IPV6_ADDRESS_NOF_U8 - char_indx - 1];
        ipv6_add_long[long_indx] |=
            UTILEX_SET_BITS_RANGE(tmp,
                                  (UTILEX_NOF_BITS_IN_CHAR * (write_to + 1) - 1), UTILEX_NOF_BITS_IN_CHAR * write_to);
        ++write_to;
        if (write_to >= sizeof(uint32))
        {
            long_indx++;
            write_to = 0;
        }
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
utilex_pp_ipv6_address_long_to_struct(
    uint32 ipv6_add_long[UTILEX_PP_IPV6_ADDRESS_NOF_UINT32S],
    uint8 ipv6_address[UTILEX_PP_IPV6_ADDRESS_NOF_U8])
{
    uint32 char_indx, long_indx, read_from;
    SHR_FUNC_INIT_VARS(NO_UNIT);

    read_from = 0;
    long_indx = 0;
    for (char_indx = 0; char_indx < UTILEX_PP_IPV6_ADDRESS_NOF_U8; ++char_indx)
    {
        ipv6_address[UTILEX_PP_IPV6_ADDRESS_NOF_U8 - char_indx - 1] =
            (uint8) UTILEX_GET_BITS_RANGE(ipv6_add_long[long_indx],
                                          UTILEX_NOF_BITS_IN_CHAR * (read_from + 1) - 1,
                                          UTILEX_NOF_BITS_IN_CHAR * read_from);
        ++read_from;
        if (read_from >= sizeof(uint32))
        {
            long_indx++;
            read_from = 0;
        }
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
utilex_pp_ipv6_group_struct_to_long(
    uint8 ipv6_address[UTILEX_PP_IPV6_ADDRESS_NOF_U8],
    uint32 ipv6_add_long[UTILEX_PP_IPV6_ADDRESS_NOF_UINT32S])
{
    uint32 tmp;
    uint32 char_indx, long_indx, write_to;
    SHR_FUNC_INIT_VARS(NO_UNIT);

    tmp = 0;
    write_to = 1;
    sal_memset(ipv6_add_long, 0, UTILEX_PP_IPV6_ADDRESS_NOF_UINT32S * sizeof(uint32));

    for (char_indx = UTILEX_PP_IPV6_ADDRESS_NOF_U8 - 1, long_indx = 0;
         char_indx >= UTILEX_PP_IPV6_ADDRESS_NOF_U8 - UTILEX_PP_IPV6_GROUP_NOF_U8; char_indx--, write_to++)
    {
        tmp = ipv6_address[char_indx];
        ipv6_add_long[long_indx] |= (tmp << (UTILEX_NOF_BITS_IN_CHAR * (write_to - 1)));
        if (write_to % UTILEX_PP_IPV6_ADDRESS_NOF_UINT32S == 0)
        {
            long_indx++;
            write_to = 0;
        }
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * See utilex_framework.h
 */
shr_error_e
utilex_set_field(
    uint32 *reg_val,
    uint32 ms_bit,
    uint32 ls_bit,
    uint32 field_val)
{
    uint32 tmp_reg;
    shr_error_e ret;

    ret = _SHR_E_NONE;
    tmp_reg = *reg_val;

    /*
     * 32 bits at most
     */
    if (ms_bit - ls_bit + 1 > 32)
    {
        ret = _SHR_E_PARAM;
        goto exit;
    }

    tmp_reg &= UTILEX_ZERO_BITS_MASK(ms_bit, ls_bit);

    tmp_reg |= UTILEX_SET_BITS_RANGE(field_val, ms_bit, ls_bit);

    *reg_val = tmp_reg;

exit:
    return ret;

}

/**
 * \brief
 * Convert an uint8 array to an uint32 array
 *
 * \par DIRECT INPUT
 *   \param [in] *u8_val  - \n
 *       pointer to an array of uint8,
 *   \param [in] nof_bytes -\n
 *       size of the array
 *  \par DIRECT OUTPUT:
 *    Non-zero in case of an error
 *  \par INDIRECT OUTPUT
 *   \param *u32_val -\n
 *   the array of uint32
 */
shr_error_e
utilex_U8_to_U32(
    uint8 *u8_val,
    uint32 nof_bytes,
    uint32 *u32_val)
{
    uint32 u8_indx, cur_u8_indx, u32_indx;
    uint8 *cur_u8;
    shr_error_e ret = _SHR_E_NONE;

    if (!u8_val || !u32_val)
    {
        ret = _SHR_E_PARAM;
        goto exit;
    }

    cur_u8_indx = 0;
    u32_indx = 0;

    for (cur_u8 = u8_val, u8_indx = 0; u8_indx < nof_bytes; ++u8_indx, ++cur_u8)
    {
        utilex_set_field(&(u32_val[u32_indx]),
                         (cur_u8_indx + 1) * SAL_UINT8_NOF_BITS - 1, cur_u8_indx * SAL_UINT8_NOF_BITS, *cur_u8);

        cur_u8_indx++;
        if (cur_u8_indx >= sizeof(uint32))
        {
            cur_u8_indx = 0;
            ++u32_indx;
        }
    }
exit:
    return ret;
}

/**
 * \brief
 * Convert an uint32 array to an uint8 array
 *
 * \par DIRECT INPUT
 *   \param [in] *u32_val  - \n
 *       pointer to an array of uint32
 *   \param [in] nof_bytes -\n
 *       nof bytes
 *  \par DIRECT OUTPUT:
 *    Non-zero in case of an error
 *  \par INDIRECT OUTPUT
 *   \param *u8_val the array of uint8
 */
shr_error_e
utilex_U32_to_U8(
    uint32 *u32_val,
    uint32 nof_bytes,
    uint8 *u8_val)
{
    uint32 u8_indx, cur_u8_indx;
    uint32 *cur_u32;

    shr_error_e ret = _SHR_E_NONE;

    if (!u8_val || !u32_val)
    {
        ret = _SHR_E_PARAM;
        goto exit;
    }

    cur_u8_indx = 0;
    for (cur_u32 = u32_val, u8_indx = 0; u8_indx < nof_bytes; ++u8_indx)
    {
        u8_val[u8_indx] = (uint8)
            UTILEX_GET_BITS_RANGE(*cur_u32, (cur_u8_indx + 1) * SAL_UINT8_NOF_BITS - 1,
                                  cur_u8_indx * SAL_UINT8_NOF_BITS);

        ++cur_u8_indx;
        if (cur_u8_indx >= sizeof(uint32))
        {
            cur_u8_indx = 0;
            ++cur_u32;
        }
    }
exit:
    return ret;
}

/*
 * See .h file
 */
shr_error_e
utilex_pp_ip_addr_numeric_to_string(
    uint32 ip_addr,
    uint8 short_format,
    char decimal_ip[UTILEX_PP_IP_STR_SIZE])
{
    const char *format;

    if (short_format)
    {
        format = "%d.%d.%d.%d";
    }
    else
    {
        format = "%03d.%03d.%03d.%03d";
    }

/* 
 * EXTRACTING BYTES FROM uint32
 */
    sal_sprintf(decimal_ip, format,
                (unsigned char) (((ip_addr) >> 24) & 0xFF),
                (unsigned char) (((ip_addr) >> 16) & 0xFF),
                (unsigned char) (((ip_addr) >> 8) & 0xFF), (unsigned char) ((ip_addr) & 0xFF));

    return _SHR_E_NONE;
}
