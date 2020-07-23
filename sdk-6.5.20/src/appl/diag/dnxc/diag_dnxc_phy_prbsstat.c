/** \file diag_dnxc_phy_prbsstat.c
 *
 * PRBSStat is a command to periodically collect PRBS error counters and
  * compute BER based on the port configuration and the observed errorr
  * counters. The command processes and displays counters and BER calculation
  * for all lanes on a given port.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_COMMON

/* shared */
#include <shared/bsl.h>
/* appl */
#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/diag.h>
#include <appl/diag/dport.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <soc/portmod/portmod.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_er_threading.h>
#endif

#define DNXC_PRBS_STAT_F_INIT        (1 << 0)
#define DNXC_PRBS_STAT_F_RUNNING     (1 << 1)

#define DNXC_PRBS_STAT_IS_INIT(f)       (f & DNXC_PRBS_STAT_F_INIT)
#define DNXC_PRBS_STAT_IS_RUNNING(f)    (f & DNXC_PRBS_STAT_F_RUNNING)

#define DNXC_PRBS_STAT_MAX_LANES        8

#define DNXC_PRBS_STAT_LOCK(lock)      \
    if (lock) { \
        sal_mutex_take(lock, sal_mutex_FOREVER);   \
    }

#define DNXC_PRBS_STAT_UNLOCK(lock) \
    if (lock) { \
        sal_mutex_give(lock); \
    }

typedef enum
{
    DIAG_DNXC_PHY_PRBS_STAT_CURR_LOCK = 0,
    DIAG_DNXC_PHY_PRBS_STAT_CURR_LOCK_LOST = -1,
    DIAG_DNXC_PHY_PRBS_STAT_PREV_LOCK_LOST = -2,
    DIAG_DNXC_PHY_PRBS_STAT_COUNTER_OVF = 0x7FFFFFFF
} diag_dnxc_phy_prbs_stat_status_e;

/*
 * The main control block
 *
 * The hardware polling interval is stored in secs. Due to the latched
 * clear-on-read hardware register behavior, the interval determines
 * how fast new data is available to be examined with show counters / show
 * ber. If show commands are called before an interval is expired,
 * there will be no new errors or ber computation available.
 *
 * Loss of lock events are recorded. For counters, there are lossoflock
 * counters that get displayed. For BER computation, once there is a
 * loss of lock, the BER computation will not be updated until the
 * next show ber is called. This ensures loss of lock events are
 * noticed in the show ber output.
 */
typedef struct dnxc_prbs_stat_subcounter_s
{
    uint64 errors;              /* PRBS errors */
    uint64 losslock;            /* Loss of lock */
} dnxc_prbs_stat_subcounter_t;

typedef struct dnxc_prbs_stat_counter_s
{
    /*
     * acc: accumulated counters since beginning or last clear. cur: currently counted counters since last time shown.
     */
    dnxc_prbs_stat_subcounter_t acc;
    dnxc_prbs_stat_subcounter_t cur;
} dnxc_prbs_stat_counter_t;

typedef struct dnxc_prbs_stat_ber_s
{
    int delta;
    sal_time_t secs;
} dnxc_prbs_stat_ber_t;

typedef struct dnxc_prbs_stat_pinfo_s
{
    int speed;
    int lanes;
    bcm_port_phy_fec_t fec_type;
    int state[DNXC_PRBS_STAT_MAX_LANES];        /* PRBS status */
    int intervals[DNXC_PRBS_STAT_MAX_LANES];    /* Intervals expired since last shown */
    dnxc_prbs_stat_counter_t counters[DNXC_PRBS_STAT_MAX_LANES];        /* Counters per port/lane */
    dnxc_prbs_stat_ber_t ber[DNXC_PRBS_STAT_MAX_LANES]; /* Raw stats for calculating BER when PRBS locked. */
} dnxc_prbs_stat_pinfo_t;

/* PRBS Stat control block.
  * It manages,
  * 1. ports to be monitored.
  * 2. statistics of each port.
  * 3. data collection thread infos.
  */
typedef struct dnxc_prbs_stat_cb_s
{
    uint32 flags;
    int secs;
    bcm_pbmp_t pbmp;
    dnxc_prbs_stat_pinfo_t pinfo[BCM_PBMP_PORT_MAX];
    sal_sem_t sem;
    sal_thread_t thread_id;
    sal_mutex_t lock;
} dnxc_prbs_stat_cb_t;

static dnxc_prbs_stat_cb_t dnxc_prbs_stat_cb[BCM_MAX_NUM_UNITS] = { {0} };

static sh_sand_option_t dnxc_phy_prbs_stat_start_options[] = {
    {"pbmp", SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", "0", NULL, NULL, SH_SAND_ARG_FREE},
    {"interval", SAL_FIELD_TYPE_INT32, "interval [seconds]", "1", NULL, "1-180", SH_SAND_ARG_FREE},
    {NULL}
};

static sh_sand_man_t dnxc_phy_prbs_stat_start_man = {
    .brief = "Start or display PRBSStat.",
    .full = "Start collecting PRBS errors on given ports.\n"
        "If no option is specified, dump current PRBSStat configuration.",
    .synopsis = "<pbmp> <interval>",
    .examples = "1 interval=30 \n" "1 \n",
};

static sh_sand_man_t dnxc_phy_prbs_stat_stop_man = {
    .brief = "Stop PRBSStat.",
    .full = "Stop collecting PRBS errors.\n",
    .synopsis = NULL,
    .examples = NULL,
};

static sh_sand_option_t dnxc_phy_prbs_stat_counters_options[] = {
    {"pbmp", SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", "0", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

static sh_sand_man_t dnxc_phy_prbs_stat_counters_man = {
    .brief = "Display PRBS errors.",
    .full = "Display collected PRBS errors within a full interval.",
    .synopsis = NULL,
    .examples = NULL,
};

static sh_sand_option_t dnxc_phy_prbs_stat_ber_options[] = {
    {"pbmp", SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", "0", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

static sh_sand_man_t dnxc_phy_prbs_stat_ber_man = {
    .brief = "Display BER.",
    .full = "Display computed BERs.",
    .synopsis = NULL,
    .examples = NULL,
};

static sh_sand_option_t dnxc_phy_prbs_stat_clear_options[] = {
    {"pbmp", SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", "0", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

static sh_sand_man_t dnxc_phy_prbs_stat_clear_man = {
    .brief = "Clear PRBS errors.",
    .full = "Clear collected PRBS errors.",
    .synopsis = NULL,
    .examples = NULL,
};

extern shr_error_e dnxc_phy_diag_serdes_rate_get(
    int unit,
    bcm_port_t port,
    uint32 *serdes_rate);

extern shr_error_e dnxc_diag_format_big_integer_div(
    int unit,
    uint64 divisor,
    int dividend,
    uint32 *int32,
    uint32 *remainder32,
    int *power);

static shr_error_e
diag_dnxc_phy_prbs_stat_ber_compute(
    int unit,
    bcm_port_t port,
    int delta,
    sal_time_t secs,
    int *ber_pow,
    uint32 *ber_int,
    uint32 *ber_remainder)
{
    uint32 rate;
    uint64 total_bits;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Make sure BER is computed at max if no errors
     */
    if (delta == 0)
    {
        delta = 1;
    }

    if (secs == 0)
    {
        SHR_EXIT();
    }

    dnxc_phy_diag_serdes_rate_get(unit, port, &rate);

    /*
     * Get the total bits
     */
    COMPILER_64_SET(total_bits, 0, secs * 1000000);
    COMPILER_64_UMUL_32(total_bits, rate);
    SHR_IF_ERR_EXIT(dnxc_diag_format_big_integer_div(unit, total_bits, delta, ber_int, ber_remainder, ber_pow));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnxc_phy_prbs_stat_validate_pbmp(
    int unit,
    bcm_pbmp_t * pbmp)
{
    bcm_pbmp_t check_pbmp;
    bcm_port_config_t *port_config = NULL;
    char pbmp_str[FORMAT_PBMP_MAX];

    SHR_FUNC_INIT_VARS(unit);

    port_config = sal_alloc(sizeof(bcm_port_config_t), "port_config");
    if (NULL == port_config)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Not enough memory to allocate 'port_config'.\n");
    }

    sal_memset(pbmp_str, 0, (sizeof(char) * FORMAT_PBMP_MAX));
    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, port_config));
    BCM_PBMP_CLEAR(check_pbmp);
    BCM_PBMP_ASSIGN(check_pbmp, *pbmp);
    BCM_PBMP_REMOVE(check_pbmp, port_config->port);
    if (BCM_PBMP_NOT_NULL(check_pbmp))
    {
        sal_memset(pbmp_str, 0, (sizeof(char) * FORMAT_PBMP_MAX));
        SHR_CLI_EXIT(_SHR_E_PARAM, "Port %s either doesn't exists or is not enabled.\n",
                     shr_pbmp_range_format(check_pbmp, pbmp_str, sizeof(pbmp_str)));
    }

exit:
    if (NULL != port_config)
    {
        sal_free(port_config);
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnxc_phy_prbs_stat_init(
    int unit)
{
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    SHR_FUNC_INIT_VARS(unit);

    if (DNXC_PRBS_STAT_IS_INIT(pscb->flags))
    {
        SHR_EXIT();
    }

    sal_memset(pscb, 0, sizeof(dnxc_prbs_stat_cb_t));
    if ((pscb->lock = sal_mutex_create("PRBSStat lock")) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_RESOURCE, "Failed to create mutex.\n");
    }

    if ((pscb->sem = sal_sem_create("PRBSStat sleep", sal_sem_BINARY, 0)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_RESOURCE, "Failed to create semaphore.\n");
    }

    pscb->flags |= DNXC_PRBS_STAT_F_INIT;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnxc_phy_prbs_stat_counter_init(
    int unit)
{
    int lane;
    int status;
    bcm_port_t port;
    bcm_gport_t gport;
    dnxc_prbs_stat_pinfo_t *pspi;
    dnxc_prbs_stat_counter_t *psco;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    SHR_FUNC_INIT_VARS(unit);

    BCM_PBMP_ITER(pscb->pbmp, port)
    {
        pspi = &(pscb->pinfo[port]);
        psco = pspi->counters;

        sal_memset(psco, 0, sizeof(dnxc_prbs_stat_counter_t) * DNXC_PRBS_STAT_MAX_LANES);
        sal_memset(&pspi->ber, 0, sizeof(dnxc_prbs_stat_ber_t) * DNXC_PRBS_STAT_MAX_LANES);

        /*
         * Prime PRBS once to clear hardware counters 
         */
        for (lane = 0; lane < pspi->lanes; lane++)
        {
            if (IS_SFI_PORT(unit, port))
            {
                SHR_IF_ERR_EXIT(bcm_port_control_get(unit, port, bcmPortControlPrbsRxStatus, &status));
            }
            else
            {
                BCM_PHY_GPORT_LANE_PORT_SET(gport, lane, port);
                SHR_IF_ERR_EXIT(bcm_port_control_get(unit, gport, bcmPortControlPrbsRxStatus, &status));
            }
            pspi->intervals[lane] = 0;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnxc_phy_prbs_stat_dump_cfg(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_port_t port;
    dnxc_prbs_stat_pinfo_t *pspi;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    cli_out("PRBSStat interval=%ds\n", pscb->secs);

    PRT_TITLE_SET("PRBSStat Ports");
    PRT_COLUMN_ADD("Port") PRT_COLUMN_ADD("Lanes");

    BCM_PBMP_ITER(pscb->pbmp, port)
    {
        pspi = &(pscb->pinfo[port]);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", BCM_PORT_NAME(unit, port));
        PRT_CELL_SET("%d", pspi->lanes);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnxc_phy_prbs_stat_show_counters(
    int unit,
    bcm_pbmp_t * pbmp,
    sh_sand_control_t * sand_control)
{
    int lane;
    int unlocked = 0;
    bcm_port_t port;
    uint64 delta_error, acc_errors;
    uint64 delta_losslock, acc_losslocks;
    int intervals = 0;
    dnxc_prbs_stat_pinfo_t *pspi;
    dnxc_prbs_stat_counter_t *psco;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("PRBSStat Counters");
    PRT_COLUMN_ADD("Port");
    PRT_COLUMN_ADD("Stats");
    PRT_COLUMN_ADD("Accumulated count");
    PRT_COLUMN_ADD("Last count");
    PRT_COLUMN_ADD("Count Per Second");

    BCM_PBMP_ITER(*pbmp, port)
    {
        pspi = &(pscb->pinfo[port]);
        psco = pspi->counters;

        for (lane = 0; lane < pspi->lanes; lane++)
        {
            if (pspi->state[lane] == DIAG_DNXC_PHY_PRBS_STAT_CURR_LOCK_LOST)
            {
                unlocked++;
            }
        }

        if (pspi->lanes == unlocked)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", BCM_PORT_NAME(unit, port));
            PRT_CELL_SET("No PRBS lock");
            PRT_CELL_SET("N/A");
            PRT_CELL_SET("N/A");
            PRT_CELL_SET("N/A");
            unlocked = 0;
            continue;
        }

        for (lane = 0; lane < pspi->lanes; lane++)
        {
            char buf[64];
            uint64 tmp_64;

            DNXC_PRBS_STAT_LOCK(pscb->lock);
            COMPILER_64_COPY(delta_error, psco[lane].acc.errors);
            COMPILER_64_SUB_64(delta_error, psco[lane].cur.errors);
            COMPILER_64_COPY(psco[lane].cur.errors, psco[lane].acc.errors);
            COMPILER_64_COPY(acc_errors, psco[lane].acc.errors);
            intervals = pspi->intervals[lane];
            pspi->intervals[lane] = 0;

            COMPILER_64_COPY(delta_losslock, psco[lane].acc.losslock);
            COMPILER_64_SUB_64(delta_losslock, psco[lane].cur.losslock);
            COMPILER_64_COPY(psco[lane].cur.losslock, psco[lane].acc.losslock);
            COMPILER_64_COPY(acc_losslocks, psco[lane].acc.losslock);
            DNXC_PRBS_STAT_UNLOCK(pscb->lock);

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s[%d]", BCM_PORT_NAME(unit, port), lane);
            COMPILER_64_ZERO(tmp_64);
            if (COMPILER_64_NE(delta_losslock, tmp_64))
            {
                PRT_CELL_SET("LossLock");

                /*
                 * Show couters since beginning 
                 */
                format_uint64_decimal(buf, acc_losslocks, ',');
                PRT_CELL_SET("%s", buf);

                /*
                 * Show counters since last show 
                 */
                format_uint64_decimal(buf, delta_losslock, ',');
                PRT_CELL_SET("%s", buf);

                /*
                 * No rate 
                 */
                PRT_CELL_SET("");
            }
            else
            {
                COMPILER_64_ZERO(tmp_64);
                if (COMPILER_64_NE(delta_error, tmp_64))
                {
                    PRT_CELL_SET("Errors");
                    /*
                     * Show couters since beginning 
                     */
                    format_uint64_decimal(buf, acc_errors, ',');
                    PRT_CELL_SET("%s", buf);
                    /*
                     * Show counters since last show 
                     */
                    format_uint64_decimal(buf, delta_error, ',');
                    PRT_CELL_SET("%s", buf);

                    /*
                     * Show counters per second 
                     */
                    COMPILER_64_UDIV_32(delta_error, (pscb->secs * intervals));
                    format_uint64_decimal(buf, delta_error, ',');
                    PRT_CELL_SET("%s/s", buf);
                }
                else
                {
                    PRT_CELL_SET("No Errors");
                    /*
                     * Show couters since beginning 
                     */
                    format_uint64_decimal(buf, acc_errors, ',');
                    PRT_CELL_SET("%s", buf);
                    PRT_CELL_SET("0");
                    PRT_CELL_SET("0/s");
                }
            }
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnxc_phy_prbs_stat_show_ber(
    int unit,
    bcm_pbmp_t * pbmp,
    sh_sand_control_t * sand_control)
{
    int lane;
    bcm_port_t port;
    dnxc_prbs_stat_pinfo_t *pspi;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("PRBSStat Ber");
    PRT_COLUMN_ADD("Port");
    PRT_COLUMN_ADD("Stats");
    PRT_COLUMN_ADD("Ber");

    BCM_PBMP_ITER(*pbmp, port)
    {
        pspi = &(pscb->pinfo[port]);

        for (lane = 0; lane < pspi->lanes; lane++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s[%d]", BCM_PORT_NAME(unit, port), lane);

            if (pspi->state[lane] == DIAG_DNXC_PHY_PRBS_STAT_PREV_LOCK_LOST)
            {
                PRT_CELL_SET("LossOfLock");
                PRT_CELL_SET("");
            }
            else if (pspi->state[lane] == DIAG_DNXC_PHY_PRBS_STAT_CURR_LOCK_LOST)
            {
                PRT_CELL_SET("No lock");
                PRT_CELL_SET("");
            }
            else
            {
                uint32 ber_int = 0;
                uint32 ber_remainder = 0;
                int ber_pow = 0;
                DNXC_PRBS_STAT_LOCK(pscb->lock);
                SHR_IF_ERR_EXIT(diag_dnxc_phy_prbs_stat_ber_compute(unit, port,
                                                                    pspi->ber[lane].delta, pspi->ber[lane].secs,
                                                                    &ber_pow, &ber_int, &ber_remainder));
                pspi->ber[lane].delta = 0;
                pspi->ber[lane].secs = 0;
                DNXC_PRBS_STAT_UNLOCK(pscb->lock);

                PRT_CELL_SET("PRBS locked");
                PRT_CELL_SET("%d.%02de-%02d", ber_int, ber_remainder, ber_pow);
            }
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * If there is a port configuration change, update the new configuration
 * and clear counters, BER
 */
static shr_error_e
diag_dnxc_phy_prbs_stat_pinfo_update(
    int unit,
    bcm_port_t port)
{
    bcm_port_resource_t resource;
    portmod_speed_config_t port_speed_config;
    dnxc_prbs_stat_pinfo_t *pspi;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    SHR_FUNC_INIT_VARS(unit);

    pspi = &(pscb->pinfo[port]);

    SHR_IF_ERR_EXIT(portmod_speed_config_t_init(unit, &port_speed_config));
    SHR_IF_ERR_EXIT(portmod_port_speed_config_get(unit, port, &port_speed_config));
    bcm_port_resource_t_init(&resource);
    SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, port, &resource));
    if ((pspi->speed == resource.speed) &&
        (pspi->lanes == port_speed_config.num_lane) && (pspi->fec_type == resource.fec_type))
    {
        SHR_EXIT();
    }

    /*
     * Update port configs 
     */
    DNXC_PRBS_STAT_LOCK(pscb->lock);
    pspi->speed = resource.speed;
    pspi->lanes = port_speed_config.num_lane;
    pspi->fec_type = resource.fec_type;

    DNXC_PRBS_STAT_UNLOCK(pscb->lock);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnxc_phy_prbs_stat_collect(
    int unit,
    bcm_port_t port)
{
    int rv;
    int lane;
    int status;
    uint64 lcount;
    bcm_gport_t gport;
    dnxc_prbs_stat_pinfo_t *pspi;
    dnxc_prbs_stat_counter_t *psco;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    SHR_FUNC_INIT_VARS(unit);

    diag_dnxc_phy_prbs_stat_pinfo_update(unit, port);
    pspi = &(pscb->pinfo[port]);
    psco = pspi->counters;

    DNXC_PRBS_STAT_LOCK(pscb->lock);
    for (lane = 0; lane < pspi->lanes; lane++)
    {
        COMPILER_64_ZERO(lcount);

        if (IS_SFI_PORT(unit, port))
        {
            rv = bcm_port_control_get(unit, port, bcmPortControlPrbsRxStatus, &status);
        }
        else
        {
            BCM_PHY_GPORT_LANE_PORT_SET(gport, lane, port);
            rv = bcm_port_control_get(unit, gport, bcmPortControlPrbsRxStatus, &status);
        }
        if (BCM_FAILURE(rv))
        {
            DNXC_PRBS_STAT_UNLOCK(pscb->lock);
            SHR_CLI_EXIT(_SHR_E_FAIL, "Failed to get PRBS RX status for port %d.\n", port);
        }

        switch (status)
        {
            case DIAG_DNXC_PHY_PRBS_STAT_CURR_LOCK:
                pspi->state[lane] = DIAG_DNXC_PHY_PRBS_STAT_CURR_LOCK;
                break;

            case DIAG_DNXC_PHY_PRBS_STAT_CURR_LOCK_LOST:
                pspi->state[lane] = DIAG_DNXC_PHY_PRBS_STAT_CURR_LOCK_LOST;
                break;

            case DIAG_DNXC_PHY_PRBS_STAT_PREV_LOCK_LOST:
                COMPILER_64_ADD_32(psco[lane].acc.losslock, 1);
                pspi->state[lane] = DIAG_DNXC_PHY_PRBS_STAT_PREV_LOCK_LOST;
                break;

            default:
                COMPILER_64_SET(lcount, 0, status);
                COMPILER_64_ADD_64(psco[lane].acc.errors, lcount);
                pspi->intervals[lane]++;
                pspi->state[lane] = DIAG_DNXC_PHY_PRBS_STAT_CURR_LOCK;
                if (status != DIAG_DNXC_PHY_PRBS_STAT_COUNTER_OVF)
                {
                    /*
                     * Do not update BER to preserve the loss of lock event.
                     * The next show BER will 'unlatch' the loss of lock event
                     * and restart BER calculation. Without this, the loss
                     * of lock event will be lost if multiple intervals
                     * transpire between show ber calls.
                     */
                    pspi->ber[lane].delta = status;
                    pspi->ber[lane].secs = pscb->secs;
                }
        }
    }
    DNXC_PRBS_STAT_UNLOCK(pscb->lock);

exit:
    SHR_FUNC_EXIT;
}

static void
diag_dnxc_phy_prbs_stat_thread(
    int unit)
{
    int u_interval;
    bcm_port_t port;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREAD_ADD(unit, DNX_ERR_RECOVERY_INTERNAL_THREAD_PHY_PRBSSTAT);
    }
#endif

    while (pscb->secs)
    {
        u_interval = pscb->secs * 1000000;      /* Adjust to usec */

        BCM_PBMP_ITER(pscb->pbmp, port)
        {
            if (_SHR_E_NONE != diag_dnxc_phy_prbs_stat_collect(unit, port))
            {
                cli_out("Failed collecting PRBS stats for port %d.\n", port);
            }
        }

        sal_sem_take(pscb->sem, u_interval);
    }

    DNXC_PRBS_STAT_LOCK(pscb->lock);
    BCM_PBMP_ITER(pscb->pbmp, port)
    {
        sal_memset(pscb->pinfo[port].counters, 0, sizeof(dnxc_prbs_stat_counter_t) * DNXC_PRBS_STAT_MAX_LANES);
    }

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREAD_REMOVE(unit, DNX_ERR_RECOVERY_INTERNAL_THREAD_PHY_PRBSSTAT);
    }
#endif

    pscb->flags &= ~DNXC_PRBS_STAT_F_RUNNING;
    DNXC_PRBS_STAT_UNLOCK(pscb->lock);

    pscb->thread_id = NULL;
    cli_out("PRBSStat thread exiting...\n");
    sal_thread_exit(0);
}

static shr_error_e
cmd_dnxc_phy_prbs_stat_start(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int is_port_present = FALSE;
    int is_interval_present = FALSE;
    bcm_pbmp_t pbmp;
    int interval;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(diag_dnxc_phy_prbs_stat_init(unit));

    SH_SAND_IS_PRESENT("pbmp", is_port_present);
    SH_SAND_GET_PORT("pbmp", pbmp);
    SH_SAND_IS_PRESENT("interval", is_interval_present);
    SH_SAND_GET_INT32("interval", interval);

    if (FALSE == is_port_present && FALSE == is_interval_present)
    {
        if (!DNXC_PRBS_STAT_IS_RUNNING(pscb->flags))
        {
            cli_out("PRBSStat: not running\n");
        }
        else
        {
            diag_dnxc_phy_prbs_stat_dump_cfg(unit, args, sand_control);
        }

        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(diag_dnxc_phy_prbs_stat_validate_pbmp(unit, &pbmp));

    /*
     * Allow start to be called while thread has already been running to update
     * interval and pbmp
     */
    if (DNXC_PRBS_STAT_IS_RUNNING(pscb->flags))
    {
        DNXC_PRBS_STAT_LOCK(pscb->lock);
    }

    if (TRUE == is_port_present)
    {
        BCM_PBMP_ASSIGN(pscb->pbmp, pbmp);
    }

    if (interval != pscb->secs)
    {
        pscb->secs = interval;
    }

    diag_dnxc_phy_prbs_stat_counter_init(unit);

    if (DNXC_PRBS_STAT_IS_RUNNING(pscb->flags))
    {
        DNXC_PRBS_STAT_UNLOCK(pscb->lock);
        SHR_EXIT();
    }

    pscb->thread_id = sal_thread_create("PRBSStat",
                                        SAL_THREAD_STKSZ,
                                        100, (void (*)(void *)) diag_dnxc_phy_prbs_stat_thread, INT_TO_PTR(unit));
    if (pscb->thread_id == SAL_THREAD_ERROR)
    {
         /* coverity[unreachable:FALSE]  */
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "PRBSStat: Could not create PRBSStat thread\n");
        pscb->flags &= ~DNXC_PRBS_STAT_F_RUNNING;
        SHR_EXIT();
    }

    cli_out("PRBSStat thread started...\n");
    pscb->flags |= DNXC_PRBS_STAT_F_RUNNING;
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dnxc_phy_prbs_stat_stop(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    soc_timeout_t to;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    SHR_FUNC_INIT_VARS(unit);

    if (DNXC_PRBS_STAT_IS_RUNNING(pscb->flags))
    {
        /*
         * Signal thread to stop running
         */
        pscb->secs = 0;
        cli_out("PRBSStat stopping thread.\n");
        sal_sem_give(pscb->sem);

        soc_timeout_init(&to, 10000000 /* 10 secs */ , 0);
        while (pscb->thread_id != NULL)
        {
            if (soc_timeout_check(&to))
            {
                SHR_CLI_EXIT(_SHR_E_INTERNAL, "PRBSStat: Thread did not exit\n");
            }
        }
    }
    else
    {
        cli_out("PRBSStat thread is not running.\n");
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dnxc_phy_prbs_stat_counters(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int is_port_present = FALSE;
    bcm_pbmp_t pbmp;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    SHR_FUNC_INIT_VARS(unit);

    if (!DNXC_PRBS_STAT_IS_RUNNING(pscb->flags))
    {
        cli_out("PRBSStat: not running\n");
        SHR_EXIT();
    }

    SH_SAND_IS_PRESENT("pbmp", is_port_present);
    if (FALSE == is_port_present)
    {
        BCM_PBMP_ASSIGN(pbmp, pscb->pbmp);
    }
    else
    {
        SH_SAND_GET_PORT("pbmp", pbmp);
    }

    SHR_IF_ERR_EXIT(diag_dnxc_phy_prbs_stat_validate_pbmp(unit, &pbmp));

    diag_dnxc_phy_prbs_stat_show_counters(unit, &pbmp, sand_control);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dnxc_phy_prbs_stat_ber(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int is_port_present = FALSE;
    bcm_pbmp_t pbmp;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    SHR_FUNC_INIT_VARS(unit);

    if (!DNXC_PRBS_STAT_IS_RUNNING(pscb->flags))
    {
        cli_out("PRBSStat: not running\n");
        SHR_EXIT();
    }

    SH_SAND_IS_PRESENT("pbmp", is_port_present);
    if (FALSE == is_port_present)
    {
        BCM_PBMP_ASSIGN(pbmp, pscb->pbmp);
    }
    else
    {
        SH_SAND_GET_PORT("pbmp", pbmp);
    }

    SHR_IF_ERR_EXIT(diag_dnxc_phy_prbs_stat_validate_pbmp(unit, &pbmp));

    diag_dnxc_phy_prbs_stat_show_ber(unit, &pbmp, sand_control);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dnxc_phy_prbs_stat_clear(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int is_port_present = FALSE;
    bcm_pbmp_t pbmp;
    bcm_port_t port;
    dnxc_prbs_stat_pinfo_t *pspi;
    dnxc_prbs_stat_counter_t *psco;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_IS_PRESENT("pbmp", is_port_present);
    if (FALSE == is_port_present)
    {
        BCM_PBMP_ASSIGN(pbmp, pscb->pbmp);
    }
    else
    {
        SH_SAND_GET_PORT("pbmp", pbmp);
    }

    SHR_IF_ERR_EXIT(diag_dnxc_phy_prbs_stat_validate_pbmp(unit, &pbmp));

    DNXC_PRBS_STAT_LOCK(pscb->lock);
    BCM_PBMP_ITER(pbmp, port)
    {
        pspi = &(pscb->pinfo[port]);
        psco = pspi->counters;

        sal_memset(psco, 0, sizeof(dnxc_prbs_stat_counter_t) * DNXC_PRBS_STAT_MAX_LANES);
    }
    DNXC_PRBS_STAT_UNLOCK(pscb->lock);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

sh_sand_cmd_t sh_dnxc_phy_prbsst_cmds[] = {
    /*
     * keyword, action, command, options, man
     */
    {"start", cmd_dnxc_phy_prbs_stat_start, NULL, dnxc_phy_prbs_stat_start_options, &dnxc_phy_prbs_stat_start_man, NULL,
     NULL, SH_CMD_NO_XML_VERIFY}
    ,
    {"stop", cmd_dnxc_phy_prbs_stat_stop, NULL, NULL, &dnxc_phy_prbs_stat_stop_man, NULL, NULL, SH_CMD_NO_XML_VERIFY}
    ,
    {"counters", cmd_dnxc_phy_prbs_stat_counters, NULL, dnxc_phy_prbs_stat_counters_options,
     &dnxc_phy_prbs_stat_counters_man, NULL, NULL, SH_CMD_NO_XML_VERIFY}
    ,
    {"ber", cmd_dnxc_phy_prbs_stat_ber, NULL, dnxc_phy_prbs_stat_ber_options, &dnxc_phy_prbs_stat_ber_man, NULL, NULL,
     SH_CMD_NO_XML_VERIFY}
    ,
    {"clear", cmd_dnxc_phy_prbs_stat_clear, NULL, dnxc_phy_prbs_stat_clear_options, &dnxc_phy_prbs_stat_clear_man, NULL,
     NULL, SH_CMD_NO_XML_VERIFY}
    ,
    {NULL}
};

sh_sand_man_t dnxc_phy_prbsstat_man = {
    .brief = "Diagnostic for PRBSSTat",
    .full = "Diagnostic commands for collecting/displaying PRBS status.",
};
