/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        shr_bprof.h
 * Purpose:     Defines profiling tool data structures 
 * Usage:   define enum for the profiling section of code and add entry for 
 * corresponding text in shr_bprof.c and use SHR_BPROF_STATS_TIME()
 */
#ifndef _SHR_BPROF_H
#define _SHR_BPROF_H

#include <sal/core/time.h>

#define SHR_BPROF_STATS_MAX_NAME     80

typedef struct shr_bprof_stats_entry_s {
    sal_usecs_t start_stime;
    sal_usecs_t total_stime;
    sal_usecs_t max_stime;
    sal_usecs_t min_stime;
    uint32 runs;
} shr_bprof_stats_entry_t;

/* This should match with _bprof_stats_names in shr_bprof.c */
typedef enum shr_bprof_type_e {
    SHR_BPROF_SOC_PROBE = 0,
    SHR_BPROF_SOC_ATTACH,
    SHR_BPROF_SOC_INIT, 
    SHR_BPROF_SOC_MISC,
    SHR_BPROF_SOC_MMU,
    SHR_BPROF_BCM_INIT,
    SHR_BPROF_BCM_COMMON,
    SHR_BPROF_BCM_PORT,
    SHR_BPROF_BCM_L2,
    SHR_BPROF_BCM_STG,
    SHR_BPROF_BCM_VLAN,
    SHR_BPROF_BCM_TRUNK,
    SHR_BPROF_BCM_COSQ,
    SHR_BPROF_BCM_MCAST,
    SHR_BPROF_BCM_LINKSCAN,
    SHR_BPROF_BCM_STAT,
    SHR_BPROF_BCM_STK,
    SHR_BPROF_BCM_RATE,
    SHR_BPROF_BCM_KNET,
    SHR_BPROF_BCM_FIELD,
    SHR_BPROF_BCM_MIRROR,
    SHR_BPROF_BCM_TX,
    SHR_BPROF_BCM_RX,
    SHR_BPROF_BCM_L3,
    SHR_BPROF_BCM_IPMC,
    SHR_BPROF_BCM_MPLS,
    SHR_BPROF_BCM_MIM,
    SHR_BPROF_BCM_WLAN,
    SHR_BPROF_BCM_PROXY,
    SHR_BPROF_BCM_SUBPORT,
    SHR_BPROF_BCM_QOS,
    SHR_BPROF_BCM_TRILL,
    SHR_BPROF_BCM_NIV,
    SHR_BPROF_BCM_L2GRE,
    SHR_BPROF_BCM_VXLAN,
    SHR_BPROF_BCM_EXTENDER,
    SHR_BPROF_BCM_MULTICAST,
    SHR_BPROF_BCM_AUTH,
    SHR_BPROF_BCM_REGEX,
    SHR_BPROF_BCM_TIME,
    SHR_BPROF_BCM_OAM,
    SHR_BPROF_BCM_FAILOVER,
    SHR_BPROF_BCM_CES,
    SHR_BPROF_BCM_PTP,
    SHR_BPROF_BCM_BFD,
    SHR_BPROF_BCM_GLB_METER,
    SHR_BPROF_BCM_FCOE,
    SHR_BPROF_BCM_UDF,
    SHR_BPROF_BCM_SAT,
    SHR_BPROF_BCM_RANGE,
    SHR_BPROF_BCM_SWITCH,
    SHR_BPROF_BCM_FLOW,
    SHR_BPROF_BCM_TSN,
#ifdef SW_AUTONEG_SUPPORT    
    SHR_BPROF_BCM_SW_AN,
#endif    
    SHR_BPROF_BCM_XFLOW_MACSEC,
    SHR_BPROF_BCM_FT,
    SHR_BPROF_BCM_GDPLL,
    SHR_BPROF_BCM_IFA,
    SHR_BPROF_BCM_COLLECTOR,
    SHR_BPROF_BCM_TELEMETRY,
    SHR_BPROF_BCM_LATENCY_MONITOR,
    SHR_BPROF_BCM_INT,
    SHR_BPROF_BCM_SUM,
    SHR_BPROF_BCM_HASH,
    SHR_BPROF_BCM_PKTIO,
    SHR_BPROF_BCM_VIRTUAL,
    SHR_BPROF_BCM_FLEXCTR,
    SHR_BPROF_BCM_FLEXSTATE,
    SHR_BPROF_BCM_FLEXDIGEST,
    SHR_BPROF_BCM_POLICER,
    SHR_BPROF_BCM_KNETSYNC,
    SHR_BPROF_BCM_CPRI,
    SHR_BPROF_STATS_MAX
} shr_bprof_type_t;

char * shr_bprof_stats_name(int dispname);
void shr_bprof_stats_time_init(void);
void shr_bprof_stats_time_end(void);
sal_usecs_t shr_bprof_stats_time_taken(void);
void shr_bprof_stats_timer_start(int op);
void shr_bprof_stats_timer_stop(int op);
int shr_bprof_stats_get(int idx, shr_bprof_stats_entry_t *bprof_stats);

#ifdef BCM_BPROF_STATS
#define SHR_BPROF_STATS_DECL  volatile int _once

#define SHR_BPROF_STATS_TIME(stat)            \
    shr_bprof_stats_timer_start(stat);        \
    for (_once = 0; _once == 0; shr_bprof_stats_timer_stop(stat), _once = 1)
#else
#define SHR_BPROF_STATS_DECL
#define SHR_BPROF_STATS_TIME(stat)
#endif /* BCM_BPROF_STATS */
#endif /*_SHR_BPROF_H */
