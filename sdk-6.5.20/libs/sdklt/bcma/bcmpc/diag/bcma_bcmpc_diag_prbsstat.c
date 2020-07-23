/*! \file bcma_bcmpccmd_diagstat.c
 *
 * CLI 'phy diag prbstat and fecstat' command handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <sal/sal_mutex.h>
#include <shr/shr_thread.h>
#include <shr/shr_debug.h>
#include <shr/shr_pb_format.h>
#include <bcmpc/bcmpc_diag.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcma/bcmpc/bcma_bcmpc_diag.h>
#include "bcma_bcmpc_diag_internal.h"

/*******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_APPL_PORTDIAG

#define STAT_LOCK(lock)  \
        if (lock) { \
            sal_mutex_take(lock, SAL_MUTEX_FOREVER); \
        }

#define STAT_UNLOCK(lock)  \
        if (lock) { \
            sal_mutex_give(lock); \
        }

/* PRBS error and loss of lock state. */
typedef struct prbs_stat_subcounter_s {
    uint64_t errors;
    uint64_t losslock;
} prbs_stat_subcounter_t;

/* PRBS error counter. */
typedef struct prbs_stat_counter_s {
    prbs_stat_subcounter_t acc;
    prbs_stat_subcounter_t cur;
} prbs_stat_counter_t;

/*
 * Per port information
 */
typedef struct prbs_stat_pinfo_s {
    /*! Port speed. */
    uint32_t speed;

    /*! Number of lane on the port. */
    int lanes;

    /*! FEC type the port. */
    bcmpc_cmd_fec_t fec_type;

    /*! Interval per lane. */
    int intervals[BCMA_BCMPC_PM_MAX_LANES];

    /*! PRBS lock status per lane. */
    int prbs_lock[BCMA_BCMPC_PM_MAX_LANES];

    /*! PRBS error counter per lane. */
    prbs_stat_counter_t counters[BCMA_BCMPC_PM_MAX_LANES];

    /*! PRBS ber per lane. */
    double ber[BCMA_BCMPC_PM_MAX_LANES];

} prbs_stat_pinfo_t;

/*!
 * Priodically collect PRBS error and compute BER.
 *
 * Hardware polling interval is stored in secs, the interval determines how fast
 * new data is available to be examined with displaying counters and ber.
 * If command <counters> or <ber> are called before an interval has transpired,
 * there will be no new errors or ber computation available.
 */


typedef struct prbs_stat_cb_s {
    /*! Hardware polling interval time. */
    int secs;

    /*! Port map. */
    bcmdrd_pbmp_t  pbmp;

    /*! stat port infomation. */
    prbs_stat_pinfo_t pinfo[BCMDRD_CONFIG_MAX_PORTS];

    /*! Thread id. */
    shr_thread_t thread_hndl;

    /*! Mutex handle. */
    sal_mutex_t lock;

} prbs_stat_cb_t;

static prbs_stat_cb_t *prbs_stat_cb[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

static void
prbs_stat_counter_init(int unit)
{
    int lane;
    bcmpc_lport_t lport = BCMPC_INVALID_LPORT;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    prbs_stat_cb_t *pscb;
    prbs_stat_pinfo_t *pspi;

    pscb = prbs_stat_cb[unit];

    BCMDRD_PBMP_ITER(pscb->pbmp, lport) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            continue;
        }
        pspi = &pscb->pinfo[lport];

        sal_memset(&pspi->counters, 0, sizeof(pspi->counters));

        sal_memset(&pspi->ber, 0, sizeof(pspi->ber));

        /* Clear hardware counters. */
        for (lane = 0; lane < pspi->lanes; lane++) {
            pspi->intervals[lane] = 0;
        }
    }
}

static int
prbs_stat_ber_compute(int unit, shr_port_t lport, int lanes, uint32_t delta,
                      sal_time_t secs, double *ber)
{
    double rate;
    double nbits;
    prbs_stat_pinfo_t *pspi;

    if (secs < 1) {
        return SHR_E_FAIL;
    }
    /* Make sure BER is computed at max if no errors. */
    if (delta == 0) {
        delta = 1;
    }
    pspi = &prbs_stat_cb[unit]->pinfo[lport];
    bcma_bcmpc_diag_speed_rate_get(pspi->speed, pspi->lanes, pspi->fec_type,
                                   &rate);

    /* Convert rate from Gbps to bps. */
    rate = rate * 1024 * 1024 * 1024;
    nbits = rate * lanes;
    *ber = delta / (nbits * secs);

    return SHR_E_NONE;
}

static void
prbs_stat_ber_update(int unit, shr_port_t lport, int lane, uint32_t delta)
{
    double ber;
    prbs_stat_cb_t *pscb;
    prbs_stat_pinfo_t *pspi;

    pscb = prbs_stat_cb[unit];
    pspi = &pscb->pinfo[lport];

    if (prbs_stat_ber_compute(unit, lport, 1, delta, pscb->secs, &ber)) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "%d[%d]: could not compute BER\n"), lport, lane));
        return;
    }
    pspi->ber[lane] = ber;
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Updated BER for port %d: %8.2e (delta = %"PRIu32"\n"),
              lport, ber, delta));
}

static int
prbs_stat_pinfo_update(int unit, shr_port_t lport)
{
    int rv = SHR_E_NONE;
    prbs_stat_cb_t *pscb;
    prbs_stat_pinfo_t *pspi;
    bcmpc_cmd_port_cfg_t port_cfg;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;

    pscb = prbs_stat_cb[unit];
    pspi = &pscb->pinfo[lport];
    pport = bcmpc_lport_to_pport(unit, lport);

    rv = bcmpc_phy_port_port_cfg_get(unit, pport, &port_cfg);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    if (pspi->speed == port_cfg.speed &&
        pspi->lanes == port_cfg.lanes &&
        pspi->fec_type == port_cfg.fec_type) {
        return rv;
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Updating port %d configuration\n"), lport));

    STAT_LOCK(pscb->lock);
    pspi->speed = port_cfg.speed;
    pspi->lanes = port_cfg.lanes;
    pspi->fec_type = port_cfg.fec_type;

    sal_memset(&pspi->counters, 0, sizeof(pspi->counters));
    sal_memset(&pspi->ber, 0, sizeof(pspi->ber));
    STAT_UNLOCK(pscb->lock);

    return rv;
}

static int
prbs_stat_collect(int unit, shr_port_t lport)
{
    int rv = SHR_E_NONE, lane;
    bcmpc_phy_prbs_status_t status;
    prbs_stat_cb_t *pscb;
    prbs_stat_pinfo_t *pspi;
    prbs_stat_counter_t *psco;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;

    rv = prbs_stat_pinfo_update(unit, lport);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    pscb = prbs_stat_cb[unit];
    pspi = &pscb->pinfo[lport];
    psco = pspi->counters;
    pport = bcmpc_lport_to_pport(unit, lport);

    STAT_LOCK(pscb->lock);
    for (lane = 0; lane < pspi->lanes; lane++) {
        rv = bcmpc_phy_prbs_status_get(unit, pport, lane, 0, &status);
        if (SHR_FAILURE(rv)) {
            STAT_UNLOCK(pscb->lock);
            return rv;
        }
        if (!status.prbs_lock) {
            pspi->prbs_lock[lane] = 0;
            pspi->ber[lane] = 0;
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "%d : Collecting status.prbs_lock "
                                 "%"PRIu32"\n"), lport, status.prbs_lock));
        } else if (status.prbs_lock_loss) {
            pspi->prbs_lock[lane] = 1;
            psco[lane].acc.losslock = 1;
            pspi->ber[lane] = -2;
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "%d : Collecting status.prbs_lock_loss "
                                 "%"PRIu32"\n"), lport, status.prbs_lock_loss));
        } else {
            pspi->prbs_lock[lane] = 1;
            pspi->intervals[lane]++;
            psco[lane].acc.errors += status.error_count;
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "%d : Collecting status.prbs_error_count "
                                 "%"PRIu32"\n"), lport, status.error_count));
            if (pspi->ber[lane] != -2 ){
                prbs_stat_ber_update(unit, lport, lane, status.error_count);
            }
        }
    }
    STAT_UNLOCK(pscb->lock);

    return rv;
}

static bool
prbs_stat_running(int unit)
{
    prbs_stat_cb_t *pscb = prbs_stat_cb[unit];

    if (pscb && pscb->thread_hndl) {
        return true;
    }
    return false;
}

static void
prbs_stat_thread(shr_thread_t th, void *arg)
{
    int unit = (int)(uintptr_t)arg;
    shr_port_t lport;
    prbs_stat_cb_t *pscb;
    prbs_stat_pinfo_t *pspi;

    pscb = prbs_stat_cb[unit];

    while (1) {
        BCMDRD_PBMP_ITER(pscb->pbmp, lport) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Collecting PRBS stats for port %d.\n"),
                      lport));
            if (SHR_FAILURE(prbs_stat_collect(unit, lport))) {
                LOG_INFO(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "Failed collecting PRBS stats for "
                                     "port %d.\n "), lport));
            }
        }
        shr_thread_sleep(th, pscb->secs * SECOND_USEC);
        if (shr_thread_stopping(th)) {
            break;
        }
    }

    STAT_LOCK(pscb->lock);
    BCMDRD_PBMP_ITER(pscb->pbmp, lport) {
        pspi = &pscb->pinfo[lport];
        sal_memset(&pspi->counters, 0, sizeof(pspi->counters));
    }
    STAT_UNLOCK(pscb->lock);
}

static void
prbs_stat_counter_show(char *name, int lport, int lane, uint64_t acc_counter,
                       uint64_t cur_counter, sal_time_t secs, int show_rate)
{
    char chdr[32];
    uint64_t delta = 0;

    sal_sprintf(chdr, "%s.%d[%d]", name, lport, lane);
    delta = acc_counter;
    delta -= cur_counter;

    if (delta) {
        /* Show counters since beginning.*/
        cli_out("%-s : %20"PRIu64"", chdr, acc_counter);

        /* Show counters since last show.*/
        cli_out("%20"PRIu64"", delta);

        if (show_rate) {
            /* Show counters per second.*/
            cli_out("%12"PRIu64"/s", delta/secs);
        }
        cli_out("\n");
    }
}

static int
prbs_stat_port_counter(int unit, shr_port_t lport)
{
    int rv = SHR_E_NONE;
    int lane, unlocked = 0;
    prbs_stat_cb_t *pscb;
    prbs_stat_pinfo_t *pspi;
    prbs_stat_counter_t *psco;

    pscb = prbs_stat_cb[unit];
    pspi = &pscb->pinfo[lport];
    psco = pspi->counters;

    for (lane = 0; lane < pspi->lanes; lane++) {
        if (!pspi->prbs_lock[lane]) {
            unlocked++;
        }
    }

    if (unlocked == pspi->lanes) {
        cli_out("%d: no PRBS lock\n", lport);
    }
    for (lane = 0; lane < pspi->lanes; lane++) {
        prbs_stat_counter_show("ERRORS", lport, lane, psco[lane].acc.errors,
                               psco[lane].cur.errors,
                               pscb->secs * pspi->intervals[lane], 1);
        pspi->intervals[lane] = 0;
        psco[lane].cur.errors = psco[lane].acc.errors;
    }
    for (lane = 0; lane < pspi->lanes; lane++) {
        prbs_stat_counter_show("LOSSLOCK", lport, lane, psco[lane].acc.losslock,
                               psco[lane].cur.losslock, 0, 0);
        psco[lane].cur.losslock = psco[lane].acc.losslock;
    }

    return rv;
}

/*******************************************************************************
 * Public functions
 */

int
bcma_bcmpc_diag_prbsstat_init(int unit)
{
    prbs_stat_cb_t *pscb;

    pscb = prbs_stat_cb[unit];
    if (pscb) {
        return SHR_E_NONE;
    }

    pscb = sal_alloc(sizeof(prbs_stat_cb_t), "bcmaBcmpcDiagPRBSStat");
    if (pscb == NULL) {
        return SHR_E_MEMORY;
    }
    sal_memset(pscb, 0, sizeof(*pscb));
    if ((pscb->lock = sal_mutex_create("PRBSStat lock")) == NULL) {
        sal_free(pscb);
        return SHR_E_FAIL;
    }
    prbs_stat_cb[unit] = pscb;

    return SHR_E_NONE;
}

int
bcma_bcmpc_diag_prbsstat_cleanup(int unit)
{
    int u;
    int rv;
    prbs_stat_cb_t *pscb;

    for (u = 0; u < BCMDRD_CONFIG_MAX_UNITS; u++) {
        if (unit >= 0 && unit != u) {
            continue;
        }
        pscb = prbs_stat_cb[u];
        if (pscb) {
            if (pscb->thread_hndl) {
                rv = shr_thread_stop(pscb->thread_hndl, 500 * MSEC_USEC);
                if (SHR_FAILURE(rv)) {
                    cli_out("Failed to stop PRBSStat thread (%s)\n",
                            shr_errmsg(rv));
                }
            }
            sal_mutex_destroy(pscb->lock);
            sal_free(pscb);
            prbs_stat_cb[u] = NULL;
        }
    }

    return SHR_E_NONE;
}

int
bcma_bcmpc_diag_prbsstat_status(int unit)
{
    if (prbs_stat_running(unit)) {
        prbs_stat_cb_t *pscb = prbs_stat_cb[unit];
        shr_pb_t *pb = shr_pb_create();

        shr_pb_printf(pb, "PRBSStat: Polling interval: %d sec\n", pscb->secs);
        shr_pb_format_uint32(pb, "PRBSStat: Port bitmap: ",
                             pscb->pbmp.w, COUNTOF(pscb->pbmp.w), 0);
        cli_out("%s\n", shr_pb_str(pb));
        shr_pb_destroy(pb);
    } else {
        cli_out(("PRBSStat not started\n"));
    }

    return SHR_E_NONE;
}

int
bcma_bcmpc_diag_prbsstat_start(int unit, bcmdrd_pbmp_t *pbmp, uint32_t time)
{
    prbs_stat_cb_t  *pscb;
    bool prbsstat_running = prbs_stat_running(unit);

    if (time == 0 || time > 180) {
       cli_out("Internval time must be between 1 and 180 seconds\n");
       return SHR_E_PARAM;
    }

    pscb = prbs_stat_cb[unit];
    if (pscb == NULL) {
        return SHR_E_INIT;
    }

    if (prbsstat_running) {
        STAT_LOCK(pscb->lock);
    }
    pscb->secs = time;
    BCMDRD_PBMP_ASSIGN(pscb->pbmp, *pbmp);
    prbs_stat_counter_init(unit);
    if (prbsstat_running) {
        /* Must return without starting new thread. */
        STAT_UNLOCK(pscb->lock);
        return SHR_E_NONE;
    }

    pscb->thread_hndl = shr_thread_start("bcmaBcmpcPrbsStat", -1,
                                         prbs_stat_thread,
                                         (void *)(uintptr_t)unit);
   if (pscb->thread_hndl == NULL) {
       return SHR_E_FAIL;
   }
   cli_out("PRBSStat thread started ...\n");

   return SHR_E_NONE;
}

int
bcma_bcmpc_diag_prbsstat_stop(int unit)
{
    int rv;
    prbs_stat_cb_t *pscb;

    pscb = prbs_stat_cb[unit];
    if (prbs_stat_running(unit)) {
        /* Signal thread to stop running. */
        pscb->secs = 0;
        rv = shr_thread_stop(pscb->thread_hndl, 500 * MSEC_USEC);
        if (SHR_FAILURE(rv)) {
            cli_out("Failed to stop PRBSStat thread (%s)\n", shr_errmsg(rv));
            return SHR_E_FAIL;
        }
        pscb->thread_hndl = NULL;
        cli_out("Stopping PRBSStat thread\n");
    } else {
        cli_out("PRBSStat already stopped\n");
    }

    return SHR_E_NONE;
}

int
bcma_bcmpc_diag_prbsstat_counter(int unit, bcmdrd_pbmp_t *pbmp)
{
    int rv = SHR_E_NONE;
    prbs_stat_cb_t  *pscb;
    bcmpc_lport_t lport = BCMPC_INVALID_LPORT;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;

    pscb = prbs_stat_cb[unit];

    if (!prbs_stat_running(unit)) {
        cli_out("PRBSStat: not running\n");
        return SHR_E_FAIL;
    }

    STAT_LOCK(pscb->lock);
    cli_out("%12s %20s %20s %12s", "port", "accumulated count", "last count",
            "count per sec\n");
    BCMDRD_PBMP_ITER(pscb->pbmp, lport) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            continue;
        }
        rv = prbs_stat_port_counter(unit, lport);
        if (SHR_FAILURE(rv)) {
            break;
        }
    }
    STAT_UNLOCK(pscb->lock);

    return rv;

}

int
bcma_bcmpc_diag_prbsstat_ber(int unit, bcmdrd_pbmp_t *pbmp)
{
    int lane;
    prbs_stat_cb_t  *pscb;
    prbs_stat_pinfo_t *pspi;
    bcmpc_lport_t lport = BCMPC_INVALID_LPORT;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;

    pscb = prbs_stat_cb[unit];
    if (!prbs_stat_running(unit)) {
        cli_out("PRBSStat: not running\n");
        return SHR_E_FAIL;
    }

    cli_out("%-6s   %s\n", "port", "BER");
    cli_out("====\n");

    STAT_LOCK(pscb->lock);
    BCMDRD_PBMP_ITER(*pbmp, lport) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            continue;
        }
        pspi = &pscb->pinfo[lport];
        for (lane = 0; lane < pspi->lanes; lane++) {
            if (pspi->ber[lane] == -2) {
                cli_out("%d[%d] : LossOfLock\n", lport, lane);
                /* Clear it that has been displayed. */
                pspi->ber[lane] = 0;
            } else if (pspi->ber[lane]) {
                cli_out("%d[%d] : %4.2e\n", lport, lane, pspi->ber[lane]);
            } else {
                cli_out("%d[%d] : Nolock\n", lport, lane);
            }
        }
        cli_out("====\n");
    }
    STAT_UNLOCK(pscb->lock);

    return SHR_E_NONE;
}

int
bcma_bcmpc_diag_prbsstat_clear(int unit, bcmdrd_pbmp_t *pbmp)
{
    prbs_stat_cb_t  *pscb;
    prbs_stat_pinfo_t *pspi;
    bcmpc_lport_t lport = BCMPC_INVALID_LPORT;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;

    pscb = prbs_stat_cb[unit];
    if (pscb == NULL) {
        return SHR_E_INIT;
    }

    STAT_LOCK(pscb->lock);
    BCMDRD_PBMP_ITER(pscb->pbmp, lport) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            continue;
        }
        pspi = &pscb->pinfo[lport];
        sal_memset(&pspi->counters, 0, sizeof(pspi->counters));
    }
    STAT_UNLOCK(pscb->lock);

    return SHR_E_NONE;
}
