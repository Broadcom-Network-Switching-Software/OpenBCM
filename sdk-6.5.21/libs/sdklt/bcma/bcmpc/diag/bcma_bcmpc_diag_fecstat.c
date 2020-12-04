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

typedef struct fec_stat_subcounter_s {
    uint64_t corrected;
    uint64_t uncorrected;
    /* Per-lane based symbol error count. RS FEC only. */
    uint64_t symbol_error[BCMA_BCMPC_PM_MAX_LANES];
} fec_stat_subcounter_t;

/*
 * Acc: accummulated from beginning/last clear
 * Cur: count since last time shown
 */
typedef struct fec_stat_counter_s {
    fec_stat_subcounter_t acc;
    fec_stat_subcounter_t cur;
} fec_stat_counter_t;

/*
 * Per port info
 */
typedef struct fec_stat_pinfo_s {
    uint32_t speed;
    int lanes;
    bcmpc_cmd_fec_t fec_type;
    int intervals;
    /* Counters per port for this FEC. */
    fec_stat_counter_t counters;
    /* Updated every interval. */
    double ber[BCMA_BCMPC_PM_MAX_LANES];
} fec_stat_pinfo_t;

typedef struct fec_stat_cb_s {
    int secs;
    int cfg_secs;
    bcmdrd_pbmp_t pbmp;
    fec_stat_pinfo_t pinfo[BCMDRD_CONFIG_MAX_PORTS];
    shr_thread_t thread_hndl;
    sal_mutex_t lock;
} fec_stat_cb_t;

static fec_stat_cb_t *fec_stat_cb[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

static int
fec_stat_pinfo_update(int unit, shr_port_t lport)
{
    fec_stat_pinfo_t *fspi;
    fec_stat_counter_t *fsco;
    bcmpc_cmd_port_cfg_t port_cfg;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;

    fspi = &fec_stat_cb[unit]->pinfo[lport];
    pport = bcmpc_lport_to_pport(unit, lport);

    bcmpc_phy_port_port_cfg_get(unit, pport, &port_cfg);
    if (fspi->speed == port_cfg.speed &&
        fspi->lanes == port_cfg.lanes &&
        fspi->fec_type == port_cfg.fec_type) {
        return SHR_E_NONE;
    }
    fspi->speed = port_cfg.speed;
    fspi->lanes = port_cfg.lanes;
    fspi->fec_type = port_cfg.fec_type;

    fsco = &fspi->counters;
    sal_memset(&fsco->acc, 0, sizeof(fsco->acc));
    sal_memset(&fsco->cur, 0, sizeof(fsco->cur));

    return SHR_E_NONE;
}

static int
fec_stat_counter_init(int unit, bcmdrd_pbmp_t *pbmp)
{
    uint32_t value, error_count[BCMA_BCMPC_PM_MAX_LANES];
    bcmpc_phy_rsfec_symb_errcnt_t symbol_error_count;
    fec_stat_pinfo_t *fspi;
    fec_stat_cb_t *fscb;
    bcmpc_lport_t lport = BCMPC_INVALID_LPORT;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    bcmpc_cmd_fec_t fec_type;
    bcmpc_cmd_port_cfg_t port_cfg;

    SHR_FUNC_ENTER(unit);
    fscb = fec_stat_cb[unit];
    BCMDRD_PBMP_ITER(fscb->pbmp, lport) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            continue;
        }
        fspi = &fscb->pinfo[lport];
        sal_memset(&fspi->counters, 0, sizeof(fspi->counters));
        SHR_IF_ERR_EXIT
            (fec_stat_pinfo_update(unit, lport));

        /* Prime counters to avoid first read to get all values since reboot. */
        SHR_IF_ERR_EXIT
            (bcmpc_phy_port_port_cfg_get(unit, pport, &port_cfg));
        fec_type = port_cfg.fec_type;
        if (fec_type == BCMPC_CMD_PORT_FEC_RS_544 ||
            fec_type == BCMPC_CMD_PORT_FEC_RS_272 ||
            fec_type == BCMPC_CMD_PORT_FEC_RS_544_2XN ||
            fec_type == BCMPC_CMD_PORT_FEC_RS_272_2XN) {
            symbol_error_count.max_count = BCMA_BCMPC_PM_MAX_LANES;
            symbol_error_count.symbol_errcnt = error_count;
            SHR_IF_ERR_EXIT
                (bcmpc_phy_rsfec_sym_counter_get(unit, pport,
                                                 &symbol_error_count));
        }
        SHR_IF_ERR_EXIT
            (bcmpc_phy_fec_counter_get(unit, pport, fec_type,
                                       BCMPC_PHY_FEC_CORRECTED_COUNTER,
                                       &value));
        SHR_IF_ERR_EXIT
            (bcmpc_phy_fec_counter_get(unit, pport, fec_type,
                                       BCMPC_PHY_FEC_UNCORRECTED_COUNTER,
                                       &value));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
fec_stat_ber_compute(int unit, shr_port_t lport, uint32_t delta,
                     uint32_t *sym_delta, sal_time_t secs, double *ber)
{
    int idx;
    double rate = 0, nbits;
    uint32_t sum_sym_delta = 0;
    fec_stat_pinfo_t *fspi;
    bcmpc_cmd_fec_t fec_type;
    bcmpc_cmd_port_cfg_t port_cfg;
    shr_port_t pport;

    SHR_FUNC_ENTER(unit);
    if (secs < 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    fspi = &fec_stat_cb[unit]->pinfo[lport];
    bcma_bcmpc_diag_speed_rate_get(fspi->speed, fspi->lanes, fspi->fec_type,
                                   &rate);
    if (rate == 0) {
        cli_out("Error: Unsupported speed mode: %d\n", lport);
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    pport = bcmpc_lport_to_pport(unit, lport);
    SHR_IF_ERR_EXIT
        (bcmpc_phy_port_port_cfg_get(unit, pport, &port_cfg));
    fec_type = port_cfg.fec_type;

    /* Rate needs to be adjusted from Gbps to bps */
    rate = rate * 1024 * 1024 * 1024;
    /* Calculate per port BER for 2xN RS FEC based on sum of symbol error delta.
     * Calculate per lane BER for other RS FEC based on symbol error delta.
     * Calculate per port BER for other FEC types based on block error delta.
     */
    if (fspi->fec_type == BCMPC_CMD_PORT_FEC_RS_544_2XN ||
        fspi->fec_type == BCMPC_CMD_PORT_FEC_RS_272_2XN) {
        nbits = rate * fspi->lanes;
        for (idx = 0; idx < fspi->lanes; idx++) {
            sum_sym_delta += sym_delta[idx];
        }
        /* Per port BER stored in ber[0]. */
        ber[0] = sum_sym_delta / (nbits * secs);
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Updated BER for port %d: %8.2e\n"),
                  lport, ber[0]));
    } else if (fec_type == BCMPC_CMD_PORT_FEC_RS_544 ||
               fec_type == BCMPC_CMD_PORT_FEC_RS_272) {
        nbits = rate;
        for (idx = 0; idx < fspi->lanes; idx++) {
            ber[idx] = sym_delta[idx] / (nbits * secs);
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Updated BER for port %d lane%d: %8.2e\n"),
                      lport, idx, ber[idx]));
        }
    } else {
        /* Per port BER stored in ber[0]. */
        nbits = rate * fspi->lanes;
        ber[0] = delta / (nbits * secs);
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Updated BER for port %d: %8.2e\n"),
                  lport, ber[0]));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
fec_stat_ber_update(int unit, shr_port_t lport, uint32_t delta,
                    uint32_t *sym_delta)
{
    fec_stat_cb_t *fscb;
    fec_stat_pinfo_t *fspi;

    fscb = fec_stat_cb[unit];
    fspi = &fscb->pinfo[lport];

    return fec_stat_ber_compute(unit, lport, delta, sym_delta, fscb->secs,
                                fspi->ber);
}

static int
fec_stat_collect(int unit, shr_port_t lport)
{
    int idx;
    uint32_t count, corrected_count, error_count[BCMA_BCMPC_PM_MAX_LANES];
    bcmpc_phy_rsfec_symb_errcnt_t symbol_error_count;
    bcmpc_cmd_fec_t fec_type;
    bcmpc_cmd_port_cfg_t port_cfg;
    fec_stat_cb_t *fscb;
    fec_stat_pinfo_t *fspi;
    fec_stat_counter_t *fsco;
    shr_port_t pport;

    SHR_FUNC_ENTER(unit);
    fscb = fec_stat_cb[unit];
    fspi = &fscb->pinfo[lport];
    fsco = &fspi->counters;

    STAT_LOCK(fscb->lock);

    SHR_IF_ERR_EXIT
        (fec_stat_pinfo_update(unit, lport));

    pport = bcmpc_lport_to_pport(unit, lport);
    SHR_IF_ERR_EXIT
        (bcmpc_phy_port_port_cfg_get(unit, pport, &port_cfg));
    fec_type = port_cfg.fec_type;

    SHR_IF_ERR_EXIT
        (bcmpc_phy_fec_counter_get(unit, pport, fec_type,
                                   BCMPC_PHY_FEC_CORRECTED_COUNTER,
                                   &corrected_count));
    SHR_IF_ERR_EXIT
        (bcmpc_phy_fec_counter_get(unit, pport, fec_type,
                                   BCMPC_PHY_FEC_UNCORRECTED_COUNTER,
                                   &count));

    fsco->acc.corrected += corrected_count;
    fsco->acc.uncorrected += count;

    if (fec_type == BCMPC_CMD_PORT_FEC_RS_544 ||
        fec_type == BCMPC_CMD_PORT_FEC_RS_272 ||
        fec_type == BCMPC_CMD_PORT_FEC_RS_544_2XN ||
        fec_type == BCMPC_CMD_PORT_FEC_RS_272_2XN) {
        symbol_error_count.max_count = BCMA_BCMPC_PM_MAX_LANES;
        symbol_error_count.symbol_errcnt = error_count;

        SHR_IF_ERR_EXIT
            (bcmpc_phy_rsfec_sym_counter_get(unit, pport, &symbol_error_count));

        if (symbol_error_count.actual_count != fspi->lanes) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }

        for (idx = 0; idx < fspi->lanes; idx++) {
            fsco->acc.symbol_error[idx] += error_count[idx];
        }
    }

    fec_stat_ber_update(unit, lport, corrected_count, error_count);

    fspi->intervals++;

exit:
    STAT_UNLOCK(fscb->lock);
    SHR_FUNC_EXIT();
}

static bool
fec_stat_running(int unit)
{
    fec_stat_cb_t *fscb = fec_stat_cb[unit];

    if (fscb && fscb->thread_hndl) {
        return true;
    }
    return false;
}

static void
fec_stat_thread(shr_thread_t th, void *arg)
{
    int unit = (int)(uintptr_t)arg;
    shr_port_t lport;
    fec_stat_cb_t *fscb;

    fscb = fec_stat_cb[unit];

    while (1) {
        BCMDRD_PBMP_ITER(fscb->pbmp, lport) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Collecting FEC stats for port %d.\n"),
                      lport));
            if (SHR_FAILURE(fec_stat_collect(unit, lport))) {
                LOG_INFO(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "Failed collecting PRBS stats for "
                                     "port %d.\n "), lport));
            }
        }
        shr_thread_sleep(th, fscb->secs * SECOND_USEC);
        if (shr_thread_stopping(th)) {
            break;
        }
    }

   cli_out("FECSTAT thread exiting...\n" );
   STAT_LOCK(fscb->lock);
   sal_memset(fscb->pinfo, 0, sizeof(fscb->pinfo));
   STAT_UNLOCK(fscb->lock);
}

static void
fec_stat_counter_show(char *name, int lport, int lane, uint64_t acc_counter,
                      uint64_t cur_counter, sal_time_t secs)
{
    char chdr[32];
    uint64_t delta = 0;

    if (lane != -1) {
        sal_snprintf(chdr, sizeof(chdr), "%s.%d[%d]", name, lport, lane);
    } else {
        sal_snprintf(chdr, sizeof(chdr), "%s.%d", name, lport);
    }

    delta = acc_counter;
    delta -= cur_counter;
    if (delta) {
        /* Show counters since beginning.*/
        cli_out("%-s :  %15"PRIu64"", chdr, acc_counter);

        /* Show counters since last show.*/
        cli_out("%15"PRIu64"", delta);

        /* Show counters per second.*/
        cli_out("%12"PRIu64"/s", delta/secs);
        cli_out("\n");
    }
}

static int
fec_stat_port_counter(int unit, shr_port_t lport)
{
    int secs, lane, symb_err_valid = 0;
    fec_stat_cb_t *fscb;
    fec_stat_pinfo_t *fspi;
    fec_stat_counter_t *fsco;
    char *corrected_name;
    char *uncorrected_name;
    char *symbol_error_name = "RS_SYMBOL_ERR";
    bcmpc_cmd_fec_t fec_type;
    bcmpc_cmd_port_cfg_t port_cfg;
    shr_port_t pport;

    SHR_FUNC_ENTER(unit);
    fscb = fec_stat_cb[unit];
    fspi = &fscb->pinfo[lport];
    fsco = &fspi->counters;

    pport = bcmpc_lport_to_pport(unit, lport);
    SHR_IF_ERR_EXIT
        (bcmpc_phy_port_port_cfg_get(unit, pport, &port_cfg));
    fec_type = port_cfg.fec_type;

    if (fec_type == BCMPC_CMD_PORT_FEC_RS_544 ||
        fec_type == BCMPC_CMD_PORT_FEC_RS_272 ||
        fec_type == BCMPC_CMD_PORT_FEC_RS_544_2XN ||
        fec_type == BCMPC_CMD_PORT_FEC_RS_272_2XN) {
        corrected_name = "CORREC_RS_CW";
        uncorrected_name = "UNCORREC_RS_CW";
        if (fspi->fec_type != BCMPC_CMD_PORT_FEC_RS_544_2XN &&
            fspi->fec_type != BCMPC_CMD_PORT_FEC_RS_272_2XN) {
            symb_err_valid = 1;
        }
    } else {
        corrected_name = "CORREC_BASER";
        uncorrected_name = "UNCORREC_BASER";
    }

    secs = fscb->secs * fspi->intervals;
    fec_stat_counter_show(corrected_name, lport, -1,
                          fsco->acc.corrected, fsco->cur.corrected,
                          secs);
    fec_stat_counter_show(uncorrected_name, lport, -1,
                          fsco->acc.uncorrected, fsco->cur.uncorrected,
                          secs);
    if (symb_err_valid) {
        for (lane = 0; lane < fspi->lanes; lane++) {
            fec_stat_counter_show(symbol_error_name, lport, lane,
                                  fsco->acc.symbol_error[lane],
                                  fsco->cur.symbol_error[lane], secs);
            fsco->cur.symbol_error[lane] = fsco->acc.symbol_error[lane];
        }
    }
    fspi->intervals = 0;
    fsco->cur.corrected = fsco->acc.corrected;
    fsco->cur.uncorrected = fsco->acc.uncorrected;

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcma_bcmpc_diag_fecstat_init(int unit)
{
    fec_stat_cb_t *fscb;

    fscb = fec_stat_cb[unit];
    if (fscb != NULL) {
        return SHR_E_NONE;
    }

    fscb = sal_alloc(sizeof(fec_stat_cb_t), "bcmaBcmpcDiagFecStat");
    if (fscb == NULL) {
        return SHR_E_MEMORY;
    }
    sal_memset(fscb, 0, sizeof(*fscb));

    if ((fscb->lock = sal_mutex_create("FECSTAT lock")) == NULL) {
        sal_free(fscb);
        return SHR_E_FAIL;
    }
    fec_stat_cb[unit] = fscb;

    return SHR_E_NONE;
}

int
bcma_bcmpc_diag_fecstat_cleanup(int unit)
{
    int u;
    int rv;
    fec_stat_cb_t *fscb;

    for (u = 0; u < BCMDRD_CONFIG_MAX_UNITS; u++) {
        if (unit >= 0 && unit != u) {
            continue;
        }
        fscb = fec_stat_cb[u];
        if (fscb) {
            if (fscb->thread_hndl) {
                rv = shr_thread_stop(fscb->thread_hndl, 500 * MSEC_USEC);
                if (SHR_FAILURE(rv)) {
                    cli_out("Failed to stop FECSTAT thread (%s)\n",
                            shr_errmsg(rv));
                }
            }
            sal_mutex_destroy(fscb->lock);
            sal_free(fscb);
            fec_stat_cb[u] = NULL;
        }
    }

    return SHR_E_NONE;
}

int
bcma_bcmpc_diag_fecstat_status(int unit)
{
    if (fec_stat_running(unit)) {
        fec_stat_cb_t *fscb = fec_stat_cb[unit];
        shr_pb_t *pb = shr_pb_create();

        shr_pb_printf(pb, "FECStat: Polling interval: %d sec\n", fscb->secs);
        shr_pb_format_uint32(pb, "FECStat: Port bitmap: ",
                             fscb->pbmp.w, COUNTOF(fscb->pbmp.w), 0);
        cli_out("%s\n", shr_pb_str(pb));
        shr_pb_destroy(pb);
    } else {
        cli_out(("FECStat not started\n"));
    }

    return SHR_E_NONE;
}

int
bcma_bcmpc_diag_fecstat_start(int unit, bcmdrd_pbmp_t *pbmp, uint32_t time)
{
    int rv;
    fec_stat_cb_t *fscb;
    bool fscbstat_running = fec_stat_running(unit);

    if (time == 0 || time > 180) {
       cli_out("Internval time must be between 1 and 180 seconds\n");
       return SHR_E_FAIL;
    }

    fscb = fec_stat_cb[unit];
    if (fscb == NULL) {
        return SHR_E_INIT;
    }

    if (fscbstat_running) {
        STAT_LOCK(fscb->lock);
    }

    fscb->secs = time;
    BCMDRD_PBMP_ASSIGN(fscb->pbmp, *pbmp);
    rv = fec_stat_counter_init(unit, pbmp);

    if (fscbstat_running) {
        /*
         * If fec_stat is already running,
         * need to return without starting new thread.
         */
        STAT_UNLOCK(fscb->lock);
    }
    if (SHR_FAILURE(rv) || fscbstat_running) {
        return rv;
    }

    fscb->thread_hndl = shr_thread_start("bcmaBcmpcFecStat", -1,
                                         fec_stat_thread,
                                         (void *)(uintptr_t)unit);

   if (fscb->thread_hndl == NULL) {
       return SHR_E_FAIL;
   }
   cli_out("FECSTAT thread started ...\n");

   return SHR_E_NONE;
}

int
bcma_bcmpc_diag_fecstat_stop(int unit)
{
    int rv;
    fec_stat_cb_t *fscb;

    fscb = fec_stat_cb[unit];
    if (fec_stat_running(unit)) {
        /* Signal thread to stop running. */
        fscb->secs = 0;
        rv = shr_thread_stop(fscb->thread_hndl, 500 * MSEC_USEC);
        if (SHR_FAILURE(rv)) {
            cli_out("Failed to stop FECSTAT thread (%s)\n", shr_errmsg(rv));
            return SHR_E_FAIL;
        }
        fscb->thread_hndl = NULL;
        cli_out("Stopping FECSTAT thread\n");
    } else {
        cli_out("FECSTAT already stopped\n");
    }

    return SHR_E_NONE;
}

int
bcma_bcmpc_diag_fecstat_clear(int unit, bcmdrd_pbmp_t *pbmp)
{
    bcmpc_lport_t lport = BCMPC_INVALID_LPORT;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    fec_stat_cb_t *fscb;
    fec_stat_pinfo_t *fspi;

    fscb = fec_stat_cb[unit];
    if (fscb == NULL) {
        return SHR_E_INIT;
    }
    STAT_LOCK(fscb->lock);
    BCMDRD_PBMP_ITER(fscb->pbmp, lport) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            continue;
        }
        fspi = &fscb->pinfo[lport];
        sal_memset(&fspi->counters, 0, sizeof(fspi->counters));
    }
    STAT_UNLOCK(fscb->lock);

    return SHR_E_NONE;
}

int
bcma_bcmpc_diag_fecstat_counter(int unit, bcmdrd_pbmp_t *pbmp)
{
    int rv = SHR_E_NONE;
    bcmpc_lport_t lport = BCMPC_INVALID_LPORT;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    fec_stat_cb_t *fscb;

    fscb = fec_stat_cb[unit];
    if (fscb == NULL) {
        return SHR_E_INIT;
    }
    STAT_LOCK(fscb->lock);
    cli_out("%12s %20s %15s %15s",
            "port", "accumulated count", "last count", "count per sec\n");
    BCMDRD_PBMP_ITER(fscb->pbmp, lport) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            continue;
        }
        rv = fec_stat_port_counter(unit, lport);
        if (SHR_FAILURE(rv)) {
            break;
        }
    }
    STAT_UNLOCK(fscb->lock);

    return rv;
}

int
bcma_bcmpc_diag_fecstat_ber(int unit, bcmdrd_pbmp_t *pbmp)
{
    int lane;
    fec_stat_cb_t *fscb;
    fec_stat_pinfo_t *fspi;
    bcmpc_lport_t lport = BCMPC_INVALID_LPORT;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    bcmpc_cmd_fec_t fec_type;
    bcmpc_cmd_port_cfg_t port_cfg;

    fscb = fec_stat_cb[unit];
    if (fscb == NULL) {
        return SHR_E_INIT;
    }
    STAT_LOCK(fscb->lock);
    cli_out("%-6s   %s\n", "port", "BER");
    cli_out("====\n");
    BCMDRD_PBMP_ITER(*pbmp, lport) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            continue;
        }
        fspi = &fscb->pinfo[lport];
        bcmpc_phy_port_port_cfg_get(unit, pport, &port_cfg);
        fec_type = port_cfg.fec_type;
        if (fspi->fec_type == BCMPC_CMD_PORT_FEC_RS_544_2XN ||
            fspi->fec_type == BCMPC_CMD_PORT_FEC_RS_272_2XN ) {
            if (fspi->ber[0]) {
                cli_out("%d : %4.2e\n", lport, fspi->ber[0]);
            } else {
                cli_out("%d : N/A\n", lport);
            }
        } else if (fec_type == BCMPC_CMD_PORT_FEC_RS_544 ||
                   fec_type == BCMPC_CMD_PORT_FEC_RS_272 ||
                   fec_type == BCMPC_CMD_PORT_FEC_RS_544_2XN ||
                   fec_type == BCMPC_CMD_PORT_FEC_RS_272_2XN ) {
            for (lane = 0; lane < fspi->lanes; lane++) {
                if (fspi->ber[lane]) {
                    cli_out("%d[%d] : %4.2e\n", lport, lane, fspi->ber[lane]);
                } else {
                    cli_out("%d : N/A\n", lport);
                }
            }
        } else {
            if (fspi->ber[0]) {
                cli_out("%d : %4.2e\n", lport, fspi->ber[0]);
            } else {
                cli_out("%d : N/A\n", lport);
            }
        }
    }
    STAT_UNLOCK(fscb->lock);

    return SHR_E_NONE;
}
