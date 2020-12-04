/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	shr_bprof.c
 */
#include <sal/types.h>
#include <sal/core/time.h>
#include <shared/shr_bprof.h>
#include <shared/error.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>

static sal_usecs_t _bprof_stime_main_start;
static sal_usecs_t _bprof_stime_main_end;
static shr_bprof_stats_entry_t _bprof_stats[SHR_BPROF_STATS_MAX];

/* Max name should be of length < SHR_BPROF_STATS_MAX_NAME 
  This should match with shr_bprof_type_e in shr_bprof.h */
static char _bprof_stats_names[SHR_BPROF_STATS_MAX][SHR_BPROF_STATS_MAX_NAME] = {
    "soc probe",
    "soc attach",
    "init soc",
    "init misc",
    "init mmu",
    "init bcm",
    "commom",
    "port",
    "l2",
    "stg",
    "vlan",
    "trunk",
    "cosq",
    "mCast",
    "linkscan",
    "stat",
    "stk",
    "rate",
    "knet",
    "field",
    "mirror",
    "tx",
    "rx",
    "l3",
    "ipmc",
    "mpls",
    "mim",
    "wlan",
    "proxy",
    "subport",
    "qos",
    "trill",
    "niv",
    "l2gre",
    "vxlan",
    "extender",
    "multicast",
    "auth",
    "regex",
    "time",
    "oam",
    "failover",
    "ces",
    "ptp",
    "bfd",
    "global_meter",
    "fcoe",
    "udf",
    "sat",
    "range",
    "switch",
    "flow",
    "tsn",
#ifdef SW_AUTONEG_SUPPORT
    "SW_AN",
#endif    
    "xflow_macsec",
    "flowtracker",
    "gdpll",
    "ifa",
    "collector",
    "telemetry",
    "latency_monitor",
    "int",
    "sum",
    "hash",
    "pktio",
    "virtual",
    "flexctr",
    "flexstate",
    "flexdigest",
    "policer",
    "knetsync",
    "cpri"
};

void shr_bprof_stats_time_init(void)
{
    _bprof_stime_main_start = sal_time_usecs();

    return;
}

void shr_bprof_stats_time_end(void)
{
    _bprof_stime_main_end = sal_time_usecs();

    return;
}

sal_usecs_t shr_bprof_stats_time_taken(void)
{
    return (_bprof_stime_main_end - _bprof_stime_main_start);
}

char * shr_bprof_stats_name(int dispname)
{
    return _bprof_stats_names[dispname];
}

void shr_bprof_stats_timer_start(int op)
{
    sal_usecs_t now;                                                  

    now = sal_time_usecs();
    _bprof_stats[op].start_stime = now;

    return;
}

void shr_bprof_stats_timer_stop(int op)
{
    sal_usecs_t now;
    sal_usecs_t delta;

    now = sal_time_usecs();
    delta = now - _bprof_stats[op].start_stime;

    if (!_bprof_stats[op].runs) {
            _bprof_stats[op].total_stime += delta;
            _bprof_stats[op].runs++;
            _bprof_stats[op].min_stime = delta;
            _bprof_stats[op].max_stime = delta;
    } else {
        _bprof_stats[op].total_stime += delta;
        _bprof_stats[op].runs++;

        if (delta < _bprof_stats[op].min_stime) {
            _bprof_stats[op].min_stime = delta;
        } else if (delta > _bprof_stats[op].max_stime) {
            _bprof_stats[op].max_stime = delta;
        } 
    }

    return;
}

int shr_bprof_stats_get(int idx, shr_bprof_stats_entry_t *bprof_stats)
{
    if ((idx < 0) || (idx >= SHR_BPROF_STATS_MAX)) {
        return _SHR_E_UNAVAIL;
    }

    bprof_stats->start_stime = _bprof_stats[idx].start_stime;
    bprof_stats->total_stime = _bprof_stats[idx].total_stime;
    bprof_stats->max_stime = _bprof_stats[idx].max_stime;
    bprof_stats->min_stime = _bprof_stats[idx].min_stime;
    bprof_stats->runs = _bprof_stats[idx].runs;

    return _SHR_E_NONE;
}
