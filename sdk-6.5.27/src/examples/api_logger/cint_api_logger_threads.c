/*
 * File: cint_api_logger_threads.c
 *
 * Purpose:
 *
 *  Provides a filter/formatter function to process in-memory logging Entries.
 *  Concurrent API calls and Nested API calls can be detected and reported. API
 *  run-times can also be calculated and reported. Each of these outputs is
 *  enabled/disabled using its own boolean.
 *
 * Example shell command to invoke after sourcing this CINT:
 *
 *     LOGGER SHow FirstN=100000 ForMaTter=threader_fn
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/*
 * Boolean to enable/disable reporting concurrent calls
 */
int report_concurrent_calls = 1;

/*
 * Boolean to enable/disable reporting nested calls
 */
int report_nested_apis = 1;

/*
 * Boolean to enable/disable reporting of API run-times
 */
int report_api_runtime = 1;

/*
 * Aggregate statistics for an API
 */
struct api_info_t {
    /*
     * Name of the API, copied from this.api
     */
    char api[256];
    /*
     * Total number of calls found for this API. This count is incremented when
     * both BEFORE and AFTER entries for a call have been encountered, which
     * provide the start and finish times respectively.
     */
    int ncalls;
    /*
     * Total run-time reported for this API. It is updated when ncalls is
     * incremented, i.e. when both start and finish timestamps have been found
     * for a call. For each such call, the difference of finish and start time
     * is added to it.
     */
    cint_time_t total_time;
};

/*
 * Structure to hold metadata about an API for which a call1 entry was found.
 * This is just a subset of the call_ctxt structure from such an Entry.
 */
struct running_api_info_t {
    /*
     * Name of the API, copied from this.api
     */
    char api[256];
    /*
     * API aggregate statistics
     */
    api_info_t *api_info;
    /*
     * Unit if the Entry had a unit argument, else -1
     */
    int unit;
    /*
     * Call ID, copied from this.call_id
     */
    int call_id;
    /*
     * Timestamp from the call_ctxt structure, i.e. this.timestamp
     */
    cint_time_t start_ts;
};

/*
 * Structure to hold metadata about a unique thread-id that was found in the
 * call_ctxt structure of an Entry. It holds the thread ID, thread Name if
 * available, and a list of APIs executing on the thread.
 *
 * A new entry is pushed into the APIs list when a call1 Entry is found; said
 * entry is popped when the corresponding call2 Entry is found. This way nested
 * API calls may be detected if the number of entries in this list ever exceeds
 * 1.
 */
struct thread_info_t {
    /*
     * Thread ID
     */
    void *tid;
    /*
     * Thread name, Dup'd from this.caller_tname if available
     */
    char tname[100];
    /*
     * List of APIs invoked by this thread, i.e. for which the call1 Entry has
     * been found and has not yet been popped.
     */
    running_api_info_t apis[10];
    /*
     * Number of entries in the list of APIs above
     */
    int napis;
};

/*
 * Array of per-Thread structures. Its length 512 is arbitrarily chosen. We
 * expect that in any application the number of threads will be generally
 * small, and even if threads are destroyed and created, the total number of
 * unique thread IDs found in the in-memory buffer Entries will be within this
 * number.
 */
thread_info_t threads[512];

/*
 * Max number of per-Thread structures available to us
 */
int maxthreads = sizeof(threads) / sizeof(threads[0]);

/*
 * Number of unique thread-IDs found so far. Represents the number of entries
 * used up in the aray of per-Thread structures "threads"
 */
int nthreads = 0;

/*
 * Number of threads running any API. This count is incremented whenever a
 * call1 Entry is found, and the number of entries in the APIs list of the
 * corresponding thread is not more than 1, i.e. the thread was already counted
 * as active
 */
int rthreads = 0;

/*
 * Arrray of structures holding aggregate statistics about an API
 */
api_info_t api_infos[512];

/*
 * Max number of APIs for which we can track aggregate statistics
 */
int max_api_infos = sizeof(api_infos) / sizeof(api_infos[0]);

/*
 * Next free slot in the api_infos array
 */
int next_api_info = 0;

/*
 * Utility function to print provided cint_time_t as HH:MM:SS.USECS
 *
 * Parameters:
 *   [out] buf
 *     buffer where ts input is formatted as string and written
 *   [in] bufsz
 *     size of buf; this similar to the size input of snprintf
 *   [in] ts
 *     cint_time_t to be formatted as a string into buf
 */
void snprintf_time(char *buf, int bufsz, cint_time_t *ts)
{
    int len;
    cint_tm_t result;

    if (!buf || !bufsz || !ts) {
        return;
    }

    cint_strftime(buf, bufsz, "%T", cint_localtime_r(&ts->tv_sec, &result));
    len = sal_strlen(buf);
    snprintf(&buf[len], bufsz-len, ".%06d", ts->tv_usec);
}

/*
 * Find an existing or new Slot for provided thread-ID in the array of
 * per-Thread structures
 */
void* tid_to_tinfo (void *tid)
{
    int ii;
    thread_info_t *tinfo = NULL;

    /*
     * Search for this thread ID
     */
    for (ii = 0; ii < nthreads; ii++) {
        tinfo = &threads[ii];
        if (tinfo->tid == tid) {
            return tinfo;
        }
    }

    /*
     * Search failed so we need a new slot. Ensure we still have free slots. If
     * this condition is ever met, the user should simply increase maxthreads
     * above.
     */
    if (nthreads >= maxthreads) {
        printf("no more empty slots to hold thread info\n");
        return NULL;
    }

    tinfo = &threads[nthreads];
    nthreads++;

    return tinfo;
}

/*
 * Find a previously allocated or a new Slot in the api_infos array for an API,
 * given its name. NULL is returned if no empty slot is available on the array,
 * in such case the user should simply increase the size of the api_infos
 * array.
 */
void* api_to_api_info (char *api)
{
    int ii;
    api_info_t *api_info;

    for (ii = 0; ii < max_api_infos; ii++) {

        api_info = &api_infos[ii];

        if (!sal_strcmp(api_info->api, api)) {
            return api_info;
        }
    }

    /*
     * Search failed, so we need a new slot. Ensure we still have free slots.
     * If this condition is ever met, the user should simply increase maxapis
     * above.
     */
    if (next_api_info >= max_api_infos) {
        printf("no more empty slots to hold api_info\n");
        return NULL;
    }

    api_info = &api_infos[next_api_info];
    sal_strncpy(api_info->api, api, sizeof(api_info->api));
    next_api_info++;

    return api_info;
}

/*
 * Utility function to print the list of APIs which started executing in a
 * thread and not yet completed.
 */
void print_running_apis (thread_info_t *tinfo)
{
   int ii, len;
   running_api_info_t *running_info;
   char str[256];

   for (ii = 0; ii < tinfo->napis; ii++) {
       running_info = &tinfo->apis[ii];
       snprintf_time(str, sizeof(str), &running_info->start_ts);
       printf("    %2d: %-40s %10d %s\n", running_info->unit, running_info->api, running_info->call_id, str);
   }
}

/*
 * Utility function to print the list of active threads. A thread is marked
 * active when it has at least one API in its list of APIs
 */
void print_active_threads ()
{
   int ii;
   thread_info_t *tinfo;

   for (ii = 0; ii < nthreads; ii++) {
       tinfo = &threads[ii];
       if (tinfo->napis > 0) {
           printf("  Thread %s,%p is executing\n", tinfo->tname, tinfo->tid);
           print_running_apis(tinfo);
       }
   }
}


/*
 * The variable "this" should exist auto-magically. Test that by referring to
 * this.flags without prototype.
 */
int CINT_LOGGER_IS_BEFORE ()
{
    uint32 flags = this.flags;

    return (((flags & 0x100) == 0x100) &&
            ((flags & 0x100) == flags));
}

/*
 * The variable "this" should exist auto-magically. Test that by referring to
 * this.flags without prototype.
 */
int CINT_LOGGER_IS_AFTER ()
{
    uint32 flags = this.flags;

    return (((flags & 0x200) == 0x200) &&
            ((flags & 0x200) == flags));
}

/*
 * This is a filter/formatter which is expected to be used from the LOGGER
 * SHow cmd. It will maintain a record of API calls against threads and can
 * report the following 3 cases, each turned on or off by its own boolean at the
 * top of this file.
 *
 *   1. APIs may have executed concurrently, report_concurrent_calls
 *   2. Nested API calls (one API calling another), report_nested_apis
 *   3. Runtimes for each API invocation, report_api_runtime
 *
 */
int threader_fn ()
{
    int ii;
    thread_info_t *tinfo;
    running_api_info_t *running_info;
    api_info_t *api_info;
    char str[256];
    int found;
    cint_time_t api_rt = { 0, 0 };

    /*
     * Find the per-thread structure for this thread ID. If we cannot find it,
     * not much else can be done here
     */
    tinfo = tid_to_tinfo(this.caller_tid);
    if (!tinfo) {
        printf("per-Thread structure for thread ID %p was not found...exitting\n", this.caller_tid);
        return 1;
    }

    /*
     * If tinfo points to a new slot, i.e. this thread ID was seen the first
     * time, initialize from the call_ctxt structure.
     */
    if (!tinfo->tid) {
        tinfo->tid = this.caller_tid;
        if (this.caller_tname) {
            sal_strncpy(tinfo->tname, this.caller_tname, sizeof(tinfo->tname));
        } else {
            sal_strncpy(tinfo->tname, "_noname", sizeof(tinfo->tname));
        }
    }

    /*
     * If this is the call1 Entry...
     */
    if (CINT_LOGGER_IS_BEFORE()) {

        /*
         * push this API to the list of running APIs
         */
        running_info = &tinfo->apis[tinfo->napis];
        sal_strncpy(running_info->api, this.api, sizeof(running_info->api));
        running_info->unit = (cint_defined("unit")) ? unit : -1;
        running_info->call_id = this.call_id;
        running_info->start_ts = this.timestamp;
        running_info->api_info = api_to_api_info(this.api);
        tinfo->napis++;

        /*
         * If the number of running APIs is more than one, we just detected
         * nested API calls on a thread that would have already been counted as
         * running. Report the nested API calls in this case and do not
         * increment rthreads
         *
         * Else, this API started running on this thread - increment the number
         * of running threads rthreads
         */
        if (tinfo->napis > 1) {
            if (report_nested_apis) {
                printf("Nested APIs (Thread %s,%p)\n", tinfo->tname, tinfo->tid);
                print_running_apis(tinfo);
                printf("\n");
            }
        } else {
            rthreads++;
        }

        /*
         * If the number of running threads is greater than 1, we just detected
         * concurrent BCM API calls. Report the concurrent calls from the list
         * of APIs of all active threads
         */
        if (report_concurrent_calls && (rthreads > 1)) {
            snprintf_time(str, sizeof(str), &this.timestamp);
            printf("Concurrent calls detected (at %s entering %d)\n", str, this.call_id);
            print_active_threads();
            printf("\n");
        }

    }
    /*
     * If this is the call2 Entry...
     */
    else if (CINT_LOGGER_IS_AFTER()) {

        /*
         * An API just finished executing. Pop it from the list.
         */
        for (ii = tinfo->napis -1; ii >= 0; ii--) {

            running_info = &tinfo->apis[ii];

            /*
             * Sanity check. These conditions should never happen
             */
            if (!this.api || !running_info->api) {
                continue;
            }

            /*
             * We expect the API that is finishing to be the same as the one
             * that was pushed last. Therefore, the following conditions should
             * never occur (SNO)
             *
             * The API names should be the same...
             */
            if (sal_strcmp(this.api, running_info->api)) {
                printf("SNO: API names do not match (%s vs. %s)\n", this.api, running_info->api);
                continue;
            }

            /*
             * ...the call_id should be the same...
             */
            if (this.call_id != running_info->call_id) {
                printf("SNO: call_id does not match (%d vs. %d)\n", this.call_id, running_info->call_id);
                continue;
            }

            /*
             * ...the completion time must be later than the start time...
             */
            if (cint_timercmp(&this.timestamp, &running_info->start_ts) < 0) {
                printf("SNO: call2 timestamp < call1 timestamp\n");
                continue;
            }

            /*
             * ...and the said entry should be the last that was pushed
             */
            if (ii != (tinfo->napis-1)) {
                printf("SNO: found entry is not the last entry that was pushed (%d vs. %d)\n", ii, tinfo->napis-1);
            }

            /*
             * The correct entry in the executing API list was found. If at this
             * point the number of threads running is more than 1, we just
             * detected concurrent BCM API calls, possibly again. Report the
             * concurrent calls from the list of APIs of all active threads,
             * along with the completion time of the current API
             */
            if (report_concurrent_calls && (rthreads > 1)) {
                snprintf_time(str, sizeof(str), &this.timestamp);
                printf("Concurrent calls detected (at %s exitting %d)\n", str, this.call_id);
                print_active_threads();
                printf("\n");
            }

            /*
             * An API completion was found. Report the run-time of this API. Its
             * start time was saved in the running_info, its completion time is
             * available in the call_ctxt structure, and the return value in
             * variable r
             */
            if (report_api_runtime) {
                cint_timersub(&this.timestamp, &running_info->start_ts, &api_rt);
                snprintf_time(str, sizeof(str), &running_info->start_ts);
                printf("%p: %d: %-40s: %6d started %s finished in %d.%06ds (%d,%s)\n",
                       tinfo->tid,
                       running_info->unit, running_info->api, running_info->call_id,
                       str, api_rt.tv_sec, api_rt.tv_usec, r, bcm_errmsg(r));
                /*
                 * Also update API aggregate statistics
                 */
                api_info = running_info->api_info;
                cint_timeradd(&api_info->total_time, &api_rt, &api_info->total_time);
                api_info->ncalls++;
            }

            /*
             * Pop the entry from API list now.
             */
            sal_memset(running_info, 0, sizeof(*running_info));
            running_info->unit = -1;
            tinfo->napis--;

            /*
             * If there are no more APIs executing on this thread, this thread
             * is no longer active. Decrement the number of active threads.
             */
            if (tinfo->napis == 0) {
                rthreads--;
            }

            break;
        }
    }

    /*
     * filter the Entry out (i.e. do not print it in the C-like format)
     */
    return 1;
}

/*
 * Display the statistics table headers.
 *
 * Uses the function dashes() which is defined in cint_api_logger_stats.c,
 * which therefore must be sourced prior to executing this function.
 */
int threader_fn_print_statistics_header ()
{
    printf("+-%-3s-+-%-40s-+-%7s-+-%16s-+-%16s-+\n",
           dashes(3), dashes(40), dashes(7), dashes(16), dashes(16));

    printf("| %3s | %-40s | %7s | %16s | %16s |\n",
           "#", "Function", "Called", "Total Runtime", "Avg Runtime (us)");

    printf("+-%-3s-+-%-40s-+-%7s-+-%16s-+-%16s-+\n",
           dashes(3), dashes(40), dashes(7), dashes(16), dashes(16));

    return 0;
}

/*
 * Display the aggregated run-time statistics of APIs...
 *
 * Uses the function snprintf_duration_64() which is defined in
 * cint_api_logger_stats.c, which therefore must be sourced prior to executing
 * this function.
 */
int threader_fn_show_api_statistics ()
{
    int ii;
    api_info_t *api_info;
    uint64 totalus;
    char Thhmmssmsus[100];
    int need_trailing_headers = 0;

    threader_fn_print_statistics_header();

    for (ii = 0; ii < max_api_infos; ii++) {

        api_info = &api_infos[ii];

        if (!sal_strcmp(api_info->api, "") || !api_info->ncalls) {
            continue;
        }

        /*
         * compute total microseconds spent executing this API
         */
        COMPILER_64_ZERO(&totalus);
        COMPILER_64_SET(&totalus, 0, api_info->total_time.tv_sec);
        COMPILER_64_UMUL_32(&totalus, 1000000);
        COMPILER_64_ADD_32(&totalus, api_info->total_time.tv_usec);

        /*
         * ... to be printed as string ...
         */
        snprintf_duration_64(Thhmmssmsus, sizeof(Thhmmssmsus), totalus);

        /*
         * ... divide by ncalls to get average microseconds per call
         */
        COMPILER_64_UDIV_32(&totalus, api_info->ncalls);

        printf("| %3d | %-40s | %7d | %16s | %16d |\n", ii+1,
               api_info->api, api_info->ncalls,
               Thhmmssmsus, COMPILER_64_LO(&totalus));

        need_trailing_headers = 1;
    }

    if (need_trailing_headers) {
        threader_fn_print_statistics_header();
    }

    return 0;
}

/*
 * This is a utility to reset all global datastructures used here so that any
 * stale state left over from a previous invocation does not interfere with
 * subsequent invocations of the script.
 */
int threader_fn_reset ()
{
    nthreads = 0;
    rthreads = 0;
    sal_memset(threads, 0, sizeof(threads));

    next_api_info = 0;
    sal_memset(api_infos, 0, sizeof(api_infos));

    printf("threader_fn: state was reset\n");
    return 0;
}

