/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	profile_sal.c
 * Purpose: 	SAL resource usage profiler
 */

#ifdef BROADCOM_DEBUG
#ifdef INCLUDE_BCM_SAL_PROFILE

#include <sal/types.h>
#include <shared/alloc.h>
#include <shared/bsl.h>
#include <sal/core/sync.h>
#include <sal/core/thread.h>

#include <sal/appl/sal.h>

#include <appl/diag/shell.h>
#include <appl/diag/cmdlist.h>
#include <soc/cm.h>

char cmd_sal_profile_usage[] =
    "Parameters: None\n\t"
    "Display current SAL resource usage\n";

static void
_cmd_sal_profile_display(void)
{
    unsigned int res0_curr, res0_max;
    unsigned int res1_curr, res1_max;

    /*
     * appl/sal resource usage
     */
    sal_dma_alloc_resource_usage_get(&res0_curr, &res0_max);
    if (res0_max != 0) {
        cli_out("DMA Memory Allocation Current / Maximum   %d / %d Bytes\n",
                res0_curr,res0_max);
    }

    /*
     * core/sal resource usage
     */
    sal_alloc_resource_usage_get(&res0_curr, &res0_max);
    cli_out("CPU Memory Allocation Current / Maximum   %d / %d Bytes\n",
            res0_curr,res0_max);

    sal_sem_resource_usage_get(&res0_curr, &res0_max);
    if (res0_max != 0) {
        cli_out("Semaphore Count Current / Maximum         %d / %d\n",
                res0_curr,res0_max);
    }

    sal_mutex_resource_usage_get(&res0_curr, &res0_max);
    cli_out("Mutex Count Current / Maximum             %d / %d\n",
            res0_curr,res0_max);

    sal_thread_resource_usage_get(&res0_curr, &res1_curr, &res0_max, &res1_max);
    cli_out("Thread Count Current / Maximum            %d / %d\n",
            res0_curr,res0_max);

    if (res1_max != 0) {
        cli_out("Thread Stack Allocation Current / Maximum %d / %d Bytes\n",
                res1_curr,res1_max);
    }
}

cmd_result_t
cmd_sal_profile(int u, args_t *a)
/*
 * Function: 	cmd_sal_profile
 * Purpose:	Displays current SAL resource usage
 * Parameters:	None
 * Returns:	CMD_OK/CMD_USAGE
 */
{
    int  arg_count;

    COMPILER_REFERENCE(u);

    arg_count = ARG_CNT(a);

    if (arg_count != 0) {
        return(CMD_USAGE);
    } else {
        _cmd_sal_profile_display();
        soc_cm_dump_info(u);
        return(CMD_OK);
    }
}
#endif
#endif
typedef int _profile_sal_c_not_empty; /* Make ISO compilers happy. */
