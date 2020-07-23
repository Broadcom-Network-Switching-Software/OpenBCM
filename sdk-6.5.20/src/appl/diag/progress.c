/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Progress Report module
 *
 *   Usage:
 *
 *	Call progress_init and pass it a measure of the total count.
 *		To avoid messing up the display, try not to print
 *		anything from now until you've called progress_done.
 *
 *	In your loop, call progress_report on every iteration and pass it
 *		an increment to the current count (usually 1).
 *		Regardless of how often it is called, an adaptive
 *		algorithm calls time() on average once per second,
 *		and updates the display if the percentage changes.
 *
 *	Call progress_status at any time to add or change a status
 *		message displayed along with the percent done.
 *
 *	Call progress_done when finished.
 *
 *   If the entire procedure completes before start_seconds
 *		elapses, nothing is printed at all.
 */

#include <appl/diag/progress.h>
#include <sal/appl/sal.h>
#include <shared/bsl.h>
#include <sal/appl/io.h>
#include <sal/core/time.h>

struct {
    int start_seconds;
    int disable;
    uint32 total_count;
    uint32 partial_count;
    sal_time_t start_time;
    sal_time_t check_time;
    int printed_something;
    int last_frac_printed;
    int iter_per_check_time;
    int iter_count;
    char *status_str;
} progress;

#define VT100_CLEAR_TO_EOL	"\033\133\113"

void
progress_init(uint32 total_count, int start_seconds, int disable)
{
    progress.start_seconds = start_seconds;
    progress.disable = disable;
    progress.total_count = total_count;
    progress.partial_count = 0;
    progress.start_time = sal_time();
    progress.check_time = progress.start_time;
    progress.printed_something = 0;
    progress.last_frac_printed = -1;
    progress.iter_per_check_time = 1;
    progress.iter_count = 0;
    progress.status_str = 0;
}

void
progress_status(char *status_str)
{
    progress.status_str = status_str;
    progress.last_frac_printed = -1;
}

void
progress_report(uint32 count_incr)
{
    sal_time_t tm;
    if (progress.disable)
	return;
    if ((progress.partial_count += count_incr) > progress.total_count)
	progress.partial_count = progress.total_count;
    if (++progress.iter_count < progress.iter_per_check_time)
	return;
    tm = sal_time();
    if (progress.check_time == tm) {
	int old_iter = progress.iter_per_check_time;
	/* Calling time() more than once per second; slow down */
	progress.iter_per_check_time =
	    progress.iter_per_check_time * 5 / 4;
	if (progress.iter_per_check_time == old_iter)
	    progress.iter_per_check_time++;
    } else {
	if (tm > progress.check_time + 1) {
	    /* Calling time() less than once per second; speed up */
	    progress.iter_per_check_time =
		progress.iter_per_check_time * 4 / 5;
	}
	progress.check_time = tm;
	if (tm > progress.start_time + progress.start_seconds) {
	    int frac;
	    if (progress.total_count >= 0x1000000)	/* Don't overflow */
		frac = progress.partial_count / (progress.total_count / 100);
	    else
		frac = (100 * progress.partial_count) / progress.total_count;
	    if (frac != progress.last_frac_printed) {
		cli_out("\r[%d%% done] %s " VT100_CLEAR_TO_EOL,
                        frac,
                        progress.status_str ? progress.status_str : "");
		progress.printed_something = 1;
		progress.last_frac_printed = frac;
	    }
	}
    }
    progress.iter_count = 0;
}

void
progress_done(void)
{
    if (progress.printed_something)
	cli_out("\r" VT100_CLEAR_TO_EOL);
}
