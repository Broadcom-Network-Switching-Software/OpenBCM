/** \file diag_dnxc_counter.c
 *
 * Diagnostic pack for counter
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_COUNTER

/* shared */
#include <shared/bsl.h>
/* appl */
#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_utils.h>
/*soc*/
#include <soc/counter.h>
#ifdef BCM_DNX_SUPPORT
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#endif
static int prev_counter_interval[SOC_MAX_NUM_DEVICES];

/* *INDENT-OFF* */
sh_sand_option_t dnxc_counter_options[] = {
    /*name         type                  desc                                                    default  */
    {"on",         SAL_FIELD_TYPE_BOOL,  "enable counter collection",                            "false"},
    {"off",        SAL_FIELD_TYPE_BOOL,  "disable counter collection",                           "false"},
    {"sync",       SAL_FIELD_TYPE_BOOL,  "sync reads in all the counters to synchronize",        "false"},
    {"interval",   SAL_FIELD_TYPE_INT32, "configure counter interval",                           "-1",  },
    {"PortBitMap", SAL_FIELD_TYPE_PORT,  "configure counter scan ports specified by PortBitMap", "all"  },
    {NULL}
};

static void
diag_dnxc_counter_print_status(
    int unit,
    int interval,
    bcm_pbmp_t *pbmp)
{
    char pbmp_str[FORMAT_PBMP_MAX];
    char pfmt[SOC_PBMP_FMT_LEN];

    sal_memset(pbmp_str, 0, (sizeof(char) * FORMAT_PBMP_MAX));
    sal_memset(pfmt, 0, (sizeof(char) * SOC_PBMP_FMT_LEN));

    format_pbmp(unit, pbmp_str, sizeof(pbmp_str), *pbmp);
    cli_out("    Interval=%d\n", interval);
    cli_out("    PortBitMap=%s %s\n", SOC_PBMP_FMT(*pbmp, pfmt), pbmp_str);

    return;
}

static shr_error_e update_counter_pbmp(
    int unit,
    bcm_pbmp_t *counter_pbmp,
    int is_counter_on)
{
    bcm_port_t p;
    soc_control_t  *soc = SOC_CONTROL(unit);
    bcm_pbmp_t pbmp;
    SHR_FUNC_INIT_VARS(unit);

    if (soc_feature(unit, soc_feature_cpuport_stat_dma))
    {
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_OR(pbmp, PBMP_PORT_ALL(unit));
        BCM_PBMP_OR(pbmp, PBMP_CMIC(unit));
        BCM_PBMP_AND(*counter_pbmp, pbmp);
    }
    else
    {
        BCM_PBMP_AND(*counter_pbmp, PBMP_PORT_ALL(unit));
    }

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        bcm_pbmp_t stat_pbmp;
        dnx_algo_port_info_s port_info;
        int is_master_port;

        BCM_PBMP_CLEAR(stat_pbmp);
        BCM_PBMP_ITER(*counter_pbmp, p)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, p, &is_master_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, p, &port_info));
            if ((DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, TRUE, FALSE) && is_master_port)
                || DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, TRUE, FALSE)
                || DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
            {
                BCM_PBMP_PORT_ADD(stat_pbmp, p);
            }
        }
        BCM_PBMP_AND(*counter_pbmp, stat_pbmp);
    }
#endif

    if(!is_counter_on)
    {
        /*update counters bitmap in case counter thread is not start*/
        SOC_PBMP_ASSIGN(soc->counter_pbmp, *counter_pbmp);
        BCM_PBMP_ITER(soc->counter_pbmp, p)
        {
            if ((BCM_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, all), p)))
            {
                BCM_PBMP_PORT_REMOVE(soc->counter_pbmp, p);
            }
            if (IS_LB_PORT(unit, p))
            {
                BCM_PBMP_PORT_REMOVE(soc->counter_pbmp, p);
            }
        }
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/*
 * counter command functions
 */
shr_error_e
cmd_dnxc_counter(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int on_flag, off_flag, sync_flag;
    int us_itvl, us, counter_interval_default;
    bcm_pbmp_t counter_pbmp;
    int is_pbmp_present = FALSE;
    uint32 flags;
    int on_state, off_state;
    int rv;
    soc_control_t  *soc = SOC_CONTROL(unit);
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("on", on_flag);
    SH_SAND_GET_BOOL("off", off_flag);
    SH_SAND_GET_BOOL("sync", sync_flag);
    SH_SAND_GET_INT32("interval", us_itvl);

    SH_SAND_IS_PRESENT("PortBitMap", is_pbmp_present);

    counter_interval_default = (SAL_BOOT_PLISIM) ? (SOC_COUNTER_INTERVAL_SIM_DEFAULT) * 8 :
                                                    SOC_COUNTER_INTERVAL_DEFAULT;
    on_state = (us_itvl > 0) || on_flag;
    off_state = (us_itvl == 0) || (us_itvl < -1) || off_flag;
    /*
     * if sync presents, sync driver info from HW  
     */
    if(sync_flag)
    {
        rv = soc_counter_sync(unit);
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "%s: Error: Could not sync counters: %s%s\n",
                                 ARG_CMD(args), bcm_errmsg(rv), EMPTY);
    }
    SOC_PBMP_ASSIGN(counter_pbmp, soc->counter_pbmp);
    if (is_pbmp_present)
    {
        SH_SAND_GET_PORT("PortBitMap", counter_pbmp);
    }

    flags = soc->counter_flags;
    /* counters are non-dmable */
    flags &= ~SOC_COUNTER_F_DMA;

    us = soc->counter_interval;
    if (on_state && off_state )
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "%s: Error: Options turning counter on and turning counter off cannot be used at the same time\n",
                     ARG_CMD(args));
    }
    else if (off_state)/*stop counter */
    {
        us = us > 0 ? us : ((prev_counter_interval[unit] > 0) ? 
                            prev_counter_interval[unit] : counter_interval_default);

        prev_counter_interval[unit] = us;
        rv = soc_counter_stop(unit);
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "%s: Error: Could not stop counter: %s%s\n",
                                ARG_CMD(args), bcm_errmsg(rv), EMPTY);
        if (is_pbmp_present)
        {
            rv = update_counter_pbmp(unit, &counter_pbmp, 0);
            SHR_IF_ERR_EXIT_WITH_LOG(rv, "%s: Error: Could not update counter pbmp: %s%s\n",
                                ARG_CMD(args), bcm_errmsg(rv), EMPTY);
        }

    }
    else if (on_state)/*start counter */
    {
        us = (us_itvl != -1) ? us_itvl : (us > 0 ? us
                                   : (prev_counter_interval[unit] > 0) ? 
                                     prev_counter_interval[unit] : counter_interval_default);

        if (is_pbmp_present)
        {
            rv = update_counter_pbmp(unit, &counter_pbmp, 1);
            SHR_IF_ERR_EXIT_WITH_LOG(rv, "%s: Error: Could not update counter pbmp: %s%s\n",
                            ARG_CMD(args), bcm_errmsg(rv), EMPTY);
        }
        rv = soc_counter_start(unit, flags, us, counter_pbmp);
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "%s: Error: Could not start counter: %s%s\n",
                                ARG_CMD(args), bcm_errmsg(rv), EMPTY);
    }
    else /* config or print counter */
    {
        /*
         * if no option presents, just print counter status 
         */
        if ( !sync_flag && (us_itvl == -1) && !is_pbmp_present)
        {
            diag_dnxc_counter_print_status(unit, us, &counter_pbmp);
            SHR_EXIT();
        }

        if (is_pbmp_present)
        {
            rv = update_counter_pbmp(unit, &counter_pbmp, us > 0);
            SHR_IF_ERR_EXIT_WITH_LOG(rv, "%s: Error: Could not update counter pbmp: %s%s\n",
                            ARG_CMD(args), bcm_errmsg(rv), EMPTY);
            if (us > 0)
            {
                rv = soc_counter_start(unit, flags, us, counter_pbmp);
                SHR_IF_ERR_EXIT_WITH_LOG(rv, "%s: Error: Could not start counter: %s%s\n",
                                ARG_CMD(args), bcm_errmsg(rv), EMPTY);
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
#ifdef BCM_DNX_SUPPORT
sh_sand_man_t sh_dnx_counter_man = {
    .brief =    "Enable/disable counter collection.",
    .full =     "Parameters: [on] [off] [sync] [Interval=<usec>]\n"
                "       [PortBitMap=<pbmp>]\n"
                "       Starts the counter collection task running every <usec>\n"
                "       microseconds.  The task tallies software counters based on\n"
                "       hardware values and must run often enough to avoid counter\n"
                "       overflow. \n"
                "       If <interval> is 0, stops the task.\n"
                "       If <interval> is omitted, prints the current INTERVAL.\n"
                "       sync reads in all the counters to synchronize\n"
                "       'show counter' for the first time, and must be used alone.\n",
    .synopsis = "[on] [off] [sync] [Interval=<usec>] [PortBitMap=<pbmp>]",
    .examples = "off \n"
                "on \n"
                "sync \n"
                "interval=2000000 \n"
                "PortBitMap=xe13-xe15 \n"
                "interval=2000000 PortBitMap=all \n",
};
#endif

#ifdef BCM_DNXF_SUPPORT
sh_sand_man_t sh_dnxf_counter_man = {
    .brief =    "Enable/disable counter collection.",
    .full =     "Parameters: [on] [off] [sync] [Interval=<usec>]\n"
                "       [PortBitMap=<pbmp>]\n"
                "       Starts the counter collection task running every <usec>\n"
                "       microseconds.  The task tallies software counters based on\n"
                "       hardware values and must run often enough to avoid counter\n"
                "       overflow. \n"
                "       If <interval> is 0, stops the task.\n"
                "       If <interval> is omitted, prints the current INTERVAL.\n"
                "       sync reads in all the counters to synchronize\n"
                "       'show counter' for the first time, and must be used alone.\n",
    .synopsis = "[on] [off] [sync] [Interval=<usec>] [PortBitMap=<pbmp>]",
    .examples = "off \n"
                "on \n"
                "sync \n"
                "interval=2000000 \n"
                "PortBitMap=sfi13-sfi16 \n"
                "interval=2000000 PortBitMap=all \n"
};
#endif
/* *INDENT-ON* */
