/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Profile Stats commands
 */
#include <sal/types.h>
#include <sal/core/libc.h>
#include <appl/diag/shell.h>
#include <soc/cmdebug.h>
#include <shared/shr_bprof.h>
#include <shared/bsl.h>
#include <bcm/error.h>

#ifdef BCM_BPROF_STATS
void
sh_bprof_stats(int verbose)
/*
 * Function:     sh_prof_stats
 * Purpose:    Prints profile statistics.
 * Parameters:    verbose - includes more info if TRUE
 * Returns:    Nothing
 */
{
    int i = 0;
    shr_bprof_stats_entry_t bprof_stats;

    cli_out("\tOP name   \tTime(usec):Total \t Avg \t   Max \t    Min \t    Runs \n");
    for (i = 0; i < SHR_BPROF_STATS_MAX; i++) {
        if (BCM_E_NONE == shr_bprof_stats_get(i, &bprof_stats)) {
            if (bprof_stats.runs == 1) {
                cli_out("%30s = %10d %10d %10d %10d %10d \n", 
                        shr_bprof_stats_name(i), 
                        bprof_stats.total_stime,
                        0,
                        0,
                        0,
                        bprof_stats.runs);
            } else if (bprof_stats.runs > 1) {
                cli_out("%30s = %10d %10d %10d %10d %10d \n", 
                        shr_bprof_stats_name(i), 
                        bprof_stats.total_stime,
                        (bprof_stats.total_stime/bprof_stats.runs),
                        bprof_stats.max_stime,
                        bprof_stats.min_stime,
                        bprof_stats.runs);
            }
        }
    }
    cli_out("Total boot time = %10d\n", shr_bprof_stats_time_taken());

    return;
}

char sh_prof_usage[] =
    "Parameters: none\n\t"
    "Prints current profile stats\n";

/*
 * Function:     sh_prof
 * Purpose:    Print prof stats
 * Parameters:    u - Unit number (ignored)
 *        a - pointer to arguments.
 * Returns:    CMD_USAGE/CMD_OK
 */
cmd_result_t
sh_prof(int u, args_t *a)
{
    COMPILER_REFERENCE(u);

    if (ARG_CNT(a)) {
    return(CMD_USAGE);
    }
    sh_bprof_stats(TRUE);
    return(CMD_OK);
}
#endif /* BCM_BPROF_STATS */
