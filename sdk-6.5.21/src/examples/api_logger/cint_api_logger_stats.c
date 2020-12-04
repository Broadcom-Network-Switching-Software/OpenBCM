/*
 * File: cint_api_logger_stats.c
 *
 * Purpose:
 *
 * Show per-API run-time statistics collected during advanced API logging.
 *
 * Run-time statistics for every called API are stored in its api_params
 * structure by the API-logger. Here they are simply displayed. Since this
 * is only Display of previously collected statistics, this CINT does not
 * affect API performance in any way.
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/*
 * Returns a string with c dashes.
 * Used for building boundaries of the output table
 */
char*
dashes (int c)
{
    static char arr[100];

    if ((c <= 0) || (c >= sizeof(arr))) {
        c = 0;
    }

    sal_memset(arr, '-', c);
    arr[c] = '\0';
    return arr;
}

/*
 * Print header of the table.
 *
 * Function
 *   name of the API
 *
 * nArgs
 *   number of arguments to the API
 *
 * Called
 *   number of times the API was called
 *
 * Argptrs
 *   phase where pointers to API arguments are loaded up into an array
 *
 * Preload
 *   this happens only at the first invocation of the API where api_params
 *   are populated by searching the API by its name (string) in the CINT
 *   interpreter database
 *
 * Call1
 *   time spent logging BEFORE the dispatch, i.e. the chip-specific
 *   implementation was called
 *
 * Disp
 *   time spent in the dispatch / chip-specific implementation of the API.
 *   This may be seen as the time spent in the API functionality. This has
 *   nothing to do with the logger; it is captured only to display overheads
 *   contributed by the logging
 *
 * Call2
 *   time spent logging AFTER the dispatch / chip-specific implementation
 *   was called
 *
 * Total
 *   Total time spent in the API (~sum of all the above).
 *
 * Ohd
 *   overhead imposed by logging computed as (Total - Disp) by the CINT here.
 *
 * Total Runtime
 *   Total time spent in Logging + Dispatch across all calls to the API
 *
 * Overhead Runtime
 *   Total time spent except Dispatch across all calls to the API
 *
 * In each multi value box for a statistic (except Ohd) there are four values.
 *   1) value of the statistic at last API invocation
 *   2) Running average of the statistic across all invocations of the API so far
 *   3) Min value of the statistic across all invocations of the API so far
 *   4) Max value of the statistic across all invocations of the API so far
 *
 * In the Ohd box
 *   1) overhead during the Last call to the API
 *   2) average of overheads during all calls to the API
 *   3) overhead in percentage during the Last call to the API (ohdL/Total)
 *   4) average of overheads as a percentage of average of Totals during all
 *      calls to the API
 */
void
print_logger_stats_header ()
{
    printf("+%-40s+%-5s+%-7s+%+7s---%-7s+%+7s---%-7s+%+7s---%-7s+%+7s---%-7s+%+7s---%-7s+%+7s---%-7s+%+7s---%-7s+\n",
           dashes(40)    ,
           dashes(5)     , dashes(7)   ,
           dashes(7)     , dashes(7)   ,
           dashes(7)     , dashes(7)   ,
           dashes(7)     , dashes(7)   ,
           dashes(7)     , dashes(7)   ,
           dashes(7)     , dashes(7)   ,
           dashes(7)     , dashes(7)   ,
           dashes(7)     , dashes(7)
           );

    printf("|%-40s|%-5s|%-7s|%+7s / %-7s|%+7s / %-7s|%+7s / %-7s|%+7s / %-7s|%+7s / %-7s|%+7s / %-7s|%+7s / %-7s|\n",
           "Function" ,
           "nArgs"    , "Called",
           "ArgPtrs"  , "Avg",
           "Preload"  , "Avg",
           "Call1"    , "Avg",
           "Disp"     , "Avg",
           "Call2"    , "Avg",
           "Total"    , "Avg",
           "OhdL"     , "OhdAvg"
           );

    printf("|%-16s%s%16s|%-5s|%-7s|%+7s / %-7s|%+7s / %-7s|%+7s / %-7s|%+7s / %-7s|%+7s / %-7s|%+7s / %-7s|%+7s / %-7s|\n",
           " Total Runtime", "        ", "Total Overhead " ,
           ""              , ""        ,
           "Min"      , "Max",
           "Min"      , "Max",
           "Min"      , "Max",
           "Min"      , "Max",
           "Min"      , "Max",
           "Min"      , "Max",
           "%"        , "%"
           );

    printf("+%-40s+%-5s+%-7s+%+7s---%-7s+%+7s---%-7s+%+7s---%-7s+%+7s---%-7s+%+7s---%-7s+%+7s---%-7s+%+7s---%-7s+\n",
           dashes(40)    ,
           dashes(5)     , dashes(7)   ,
           dashes(7)     , dashes(7)   ,
           dashes(7)     , dashes(7)   ,
           dashes(7)     , dashes(7)   ,
           dashes(7)     , dashes(7)   ,
           dashes(7)     , dashes(7)   ,
           dashes(7)     , dashes(7)   ,
           dashes(7)     , dashes(7)
           );
}

/*
 * print microseconds (int) as HH:MM:SS:MSEC:USEC in the string subject to a
 * max length
 */
void
snprintf_duration (char *str, int len, int dur_us)
{
    int us, ms, ss, mm, hh;

    us = dur_us;

    ms = us / 1000;
    us = us % 1000;

    ss = ms / 1000;
    ms = ms % 1000;

    mm = ss / 60;
    ss = ss % 60;

    hh = mm / 60;
    mm = mm % 60;

    /* ... to be printed as a string ... */
    snprintf(str, len, "%02d:%02d:%02d:%03d:%03d", hh, mm, ss, ms, us);
}


/*
 * print microseconds (uint64) as HH:MM:SS:MSEC:USEC in the string subject to a
 * max length
 */
void
snprintf_duration_64 (char *str, int len, uint64 dur_us)
{
    int us, ms, ss, mm, hh;
    uint64 totalus, totalms, tmp;

    totalus = dur_us;

    tmp = totalus;
    COMPILER_64_UDIV_32(tmp, 1000);
    COMPILER_64_UMUL_32(tmp, 1000);
    COMPILER_64_SUB_64(totalus, tmp);
    us = COMPILER_64_LO(totalus);

    COMPILER_64_UDIV_32(tmp, 1000);
    totalms = tmp;

    COMPILER_64_UDIV_32(tmp, 1000);
    COMPILER_64_UMUL_32(tmp, 1000);
    COMPILER_64_SUB_64(totalms, tmp);
    ms = COMPILER_64_LO(totalms);

    COMPILER_64_UDIV_32(tmp, 1000);
    ss = COMPILER_64_LO(tmp);

    mm = ss / 60;
    ss = ss % 60;

    hh = mm / 60;
    mm = mm % 60;

    /* ... to be printed as a string */
    snprintf(str, len, "%02d:%02d:%02d:%03d:%03d", hh, mm, ss, ms, us);
}

/*
 * print 'a' as a percentage of 'b' in the string subject to a max length
 */
void
snprintf_percent_64 (char *str, int len, uint64 a, uint64 b)
{
    uint64 tmp;
    int whol, frac;

    COMPILER_64_UMUL_32(&a, 100);

    tmp = a;
    COMPILER_64_UDIV_64(&tmp, b);
    whol = COMPILER_64_LO(&tmp);

    tmp = b;
    COMPILER_64_UMUL_32(&tmp, whol);
    COMPILER_64_SUB_64(&a, tmp);

    COMPILER_64_UMUL_32(&a, 100);
    COMPILER_64_UDIV_64(&a, b);
    frac = COMPILER_64_LO(&a);

    snprintf(str, len, "%d.%02d", whol, frac);
}

/*
 * Show the per-API logging statistics. Also show the logger global
 * configuration and state structure.
 */
int
show_api_logger_stats ()
{
    int ii;
    int ohdl, ohda;
    float ohdlp, ohdap;
    cint_logger_api_params_t *sp = cint_logger_cfg.ctxt_head;
    uint64 api_ovhdus, api_totalus, ovhdus, totalus;
    char Ohhmmssmsus[100];
    char Thhmmssmsus[100];

    COMPILER_64_ZERO(&ovhdus);
    COMPILER_64_ZERO(&totalus);

    print_logger_stats_header();

    for (ii = 0; ii < cint_logger_cfg.next_free_ctxt; ii++) {
        sp = &cint_logger_cfg.ctxt_head[ii];


        /* absolute value of overhead in last call and avg */
        ohdl = sp->total.last - sp->dispatch.last;
        ohda = sp->total.avg - sp->dispatch.avg;

        /* percent of overhead in last call and avg */
        ohdlp = (100.0 * ohdl) / sp->total.last;
        ohdap = (100.0 * ohda) / sp->total.avg;

        /* overheads accumulated by this API ... */
        COMPILER_64_ZERO(&api_ovhdus);
        COMPILER_64_SET(&api_ovhdus, 0, ohda);
        COMPILER_64_UMUL_32(&api_ovhdus, sp->called_count);
        /* ... to be printed as a string ... */
        snprintf_duration_64(Ohhmmssmsus, sizeof(Ohhmmssmsus), api_ovhdus);
        /* ... add to the total overheads */
        COMPILER_64_ADD_64(&ovhdus, api_ovhdus);

        /* total time spent in this API ... */
        COMPILER_64_ZERO(&api_totalus);
        COMPILER_64_SET(&api_totalus, 0, sp->total.avg);
        COMPILER_64_UMUL_32(&api_totalus, sp->called_count);
        /* ... to be printed as a string ... */
        snprintf_duration_64(Thhmmssmsus, sizeof(Thhmmssmsus), api_totalus);
        /* ... add to the total runtime */
        COMPILER_64_ADD_64(&totalus, api_totalus);

        printf("|%-40s|%-5d|%-7d|%7d / %-7d|%7d / %-7d|%7d / %-7d|%7d / %-7d|%7d / %-7d|%7d / %-7d|%7d / %-7d|\n",
               sp->fn,
               sp->nargs, sp->called_count,
               sp->arg_ptrs.last        , sp->arg_ptrs.avg       ,
               sp->preload.last         , sp->preload.avg        ,
               sp->call1.last           , sp->call1.avg          ,
               sp->dispatch.last        , sp->dispatch.avg       ,
               sp->call2.last           , sp->call2.avg          ,
               sp->total.last           , sp->total.avg          ,
               ohdl                     , ohda);
        printf("|%-16s%s%16s|%-5s|%-7s|%7d / %-7d|%7d / %-7d|%7d / %-7d|%7d / %-7d|%7d / %-7d|%7d / %-7d|%7.2f / %-7.2f|\n",
               Thhmmssmsus, "        ", Ohhmmssmsus,
               ""         , ""        ,
               sp->arg_ptrs.min         , sp->arg_ptrs.max       ,
               sp->preload.min          , sp->preload.max        ,
               sp->call1.min            , sp->call1.max          ,
               sp->dispatch.min         , sp->dispatch.max       ,
               sp->call2.min            , sp->call2.max          ,
               sp->total.min            , sp->total.max          ,
               ohdlp                    , ohdap);
        printf("+%-40s+%-5s+%-7s+%7s---%-7s+%7s---%-7s+%7s---%-7s+%7s---%-7s+%7s---%-7s+%7s---%-7s+%7s---%-7s+\n",
               dashes(40)    ,
               dashes(5)     , dashes(7)   ,
               dashes(7)     , dashes(7)   ,
               dashes(7)     , dashes(7)   ,
               dashes(7)     , dashes(7)   ,
               dashes(7)     , dashes(7)   ,
               dashes(7)     , dashes(7)   ,
               dashes(7)     , dashes(7)   ,
               dashes(7)     , dashes(7));
    }

    print_logger_stats_header();

    /* convert total overhead into hh,mm,ss,ms,us ... */
    snprintf_duration_64(Ohhmmssmsus, sizeof(Ohhmmssmsus), ovhdus);

    printf("\nTotal Estimated Overhead: %s\n", Ohhmmssmsus);

    /* convert total runtime into hh,mm,ss,ms,us ... */
    snprintf_duration_64(Thhmmssmsus, sizeof(Thhmmssmsus), totalus);

    printf("\nTotal Estimated  Runtime: %s\n", Thhmmssmsus);

    /* find overhead as a percentage of the total time */
    snprintf_percent_64(Ohhmmssmsus, sizeof(Ohhmmssmsus), ovhdus, totalus);

    printf("\nOverhead / Total Runtime: %s%%\n\n", Ohhmmssmsus);

    print cint_logger_cfg;

    return 0;
}

