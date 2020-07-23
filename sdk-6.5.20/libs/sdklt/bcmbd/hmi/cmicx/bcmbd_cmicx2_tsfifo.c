/*! \file bcmbd_cmicx2_ts_fifo.c
 *
 * CMICx timesync fifo operation framework.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_sem.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <shr/shr_thread.h>

#include <bcmdrd/bcmdrd_types.h>

#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_cmicx2_acc.h>
#include <bcmbd/bcmbd_cmicx2_ts_intr.h>
#include <bcmbd/bcmbd_ts_intr.h>
#include <bcmbd/bcmbd_tsfifo.h>
#include <bcmbd/bcmbd_tsfifo_internal.h>
#include <bcmbd/bcmbd_cmicx2_tsfifo.h>

#define BSL_LOG_MODULE  BSL_LS_BCMBD_TIMESYNC
/*******************************************************************************
 * Local definitions
 */
#define MAX_UNITS       BCMDRD_CONFIG_MAX_UNITS

static bcmbd_tsfifo_capture_cb_t fifo_cap_cb[MAX_UNITS];

/*******************************************************************************
 * Private functions
 */
static void
write_entry(bcmbd_tsfifo_capture_info_t *entry,
            bcmbd_tsfifo_capture_cb_t *app_cb)

{
    void *fd;

    if (!entry || !app_cb) {
        return;
    }
    fd = app_cb->fd;
    if (!fd || !app_cb->write) {
        return;
    }
    if (app_cb->write(fd, entry, sizeof(*entry)) != sizeof(*entry)) {
        return;
    }
}

static void
tsfifo_pop(int unit)
{
    NS_TS_CAPTURE_STATUSr_t fifo_sts;
    uint32_t depth, valid;
    NS_TIMESYNC_INPUT_TIME_FIFO1_TS_0r_t fifo_ts0;
    NS_TIMESYNC_INPUT_TIME_FIFO1_TS_1r_t fifo_ts1;
    NS_TIMESYNC_INPUT_TIME_FIFO1_TS_2r_t fifo_ts2;
    NS_TIMESYNC_INPUT_TIME_FIFO1_TS_3r_t fifo_ts3;

    bcmbd_tsfifo_capture_info_t fifo_info;
    bcmbd_tsfifo_capture_cb_t *app_cb = &fifo_cap_cb[unit];

    READ_NS_TS_CAPTURE_STATUSr(unit, &fifo_sts);
    depth = NS_TS_CAPTURE_STATUSr_FIFO1_DEPTHf_GET(fifo_sts);
    LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, "depth %d\n"), depth));
    while (depth) {
        uint64_t ts0_l, ts0_u, ts1_l, ts1_m, ts1_u;

        READ_NS_TIMESYNC_INPUT_TIME_FIFO1_TS_3r(unit, &fifo_ts3);
        READ_NS_TIMESYNC_INPUT_TIME_FIFO1_TS_2r(unit, &fifo_ts2);
        READ_NS_TIMESYNC_INPUT_TIME_FIFO1_TS_1r(unit, &fifo_ts1);
        READ_NS_TIMESYNC_INPUT_TIME_FIFO1_TS_0r(unit, &fifo_ts0);
        valid = NS_TIMESYNC_INPUT_TIME_FIFO1_TS_3r_TS_VALIDf_GET(fifo_ts3);
        if (!valid) {
            depth--;
            continue;
        }

        ts0_l = NS_TIMESYNC_INPUT_TIME_FIFO1_TS_0r_TS0_Lf_GET(fifo_ts0);
        ts0_u = NS_TIMESYNC_INPUT_TIME_FIFO1_TS_1r_TS0_Uf_GET(fifo_ts1);
        ts1_l = NS_TIMESYNC_INPUT_TIME_FIFO1_TS_1r_TS1_Lf_GET(fifo_ts1);
        ts1_m = NS_TIMESYNC_INPUT_TIME_FIFO1_TS_2r_TS1_Mf_GET(fifo_ts2);
        ts1_u = NS_TIMESYNC_INPUT_TIME_FIFO1_TS_3r_TS1_Uf_GET(fifo_ts3);

        sal_memset(&fifo_info, 0, sizeof(bcmbd_tsfifo_capture_info_t));
        fifo_info.raw0 = (ts0_u << 32) | ts0_l;
        fifo_info.raw1 = (ts1_u << 44) | (ts1_m << 12) | ts1_l;
        fifo_info.evt_id = NS_TIMESYNC_INPUT_TIME_FIFO1_TS_3r_TS_EVENT_IDf_GET(fifo_ts3);
        fifo_info.offset = NS_TIMESYNC_INPUT_TIME_FIFO1_TS_3r_TS_ERRf_GET(fifo_ts3);
        if (app_cb->write) {
            write_entry(&fifo_info, app_cb);
        }

        depth--;
    }
}

static void
tsfifo_intr(int unit)
{
    uint32_t sts, overflow;
    NS_TS_INT_STATUSr_t intr_sts;

    READ_NS_TS_INT_STATUSr(unit, &intr_sts);
    sts = NS_TS_INT_STATUSr_TS_FIFO1_NOT_EMPTYf_GET(intr_sts);
    overflow = NS_TS_INT_STATUSr_TS_FIFO1_OVERFLOWf_GET(intr_sts);

    if (sts || overflow) {
        tsfifo_pop(unit);
    }

    if (overflow) {
        bcmbd_ts_intr_clear(unit, CMICX_TS_FIFO1_OVERFLOW);
    }

    if (sts) {
        bcmbd_ts_intr_clear(unit, CMICX_TS_FIFO1_NOT_EMPTY);
    }
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit, "empty %x overflow %x\n"), sts, overflow));
}

static void
tsfifo_isr(int unit, uint32_t intr_param)
{
    if (intr_param == CMICX_TS_FIFO1_NOT_EMPTY ||
        intr_param == CMICX_TS_FIFO1_OVERFLOW) {
        tsfifo_intr(unit);
    }
}

static void
tsfifo_value_compensate(bcmbd_tsfifo_capture_info_t *cap_info)
{
    uint64_t ts0, ts1;

    /* TS0 in picosecond = (TS0[51:0] + (TS_ERR[1:0] << 3)) * 1000 / 16 */
    ts0 = cap_info->raw0;
    ts0 += (cap_info->offset << 3);
    ts0 *= 1000;
    ts0 = ts0 / 16;

    ts1 = cap_info->raw1;
    ts1 += (cap_info->offset << 3);
    ts1 *= 1000;
    ts1 = ts1 / 16;

    cap_info->ts0 = ts0;
    cap_info->ts1 = ts1;
}

static int
cmicx2_tsfifo_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    bcmbd_ts_intr_func_set(unit, CMICX_TS_FIFO1_NOT_EMPTY,
                           tsfifo_isr, CMICX_TS_FIFO1_NOT_EMPTY);
    bcmbd_ts_intr_func_set(unit, CMICX_TS_FIFO1_OVERFLOW,
                           tsfifo_isr, CMICX_TS_FIFO1_OVERFLOW);

    SHR_FUNC_EXIT();
}

static int
cmicx2_tsfifo_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    bcmbd_ts_intr_disable(unit, CMICX_TS_FIFO1_NOT_EMPTY);
    bcmbd_ts_intr_disable(unit, CMICX_TS_FIFO1_OVERFLOW);

    bcmbd_ts_intr_func_set(unit, CMICX_TS_FIFO1_NOT_EMPTY, NULL, 0);
    bcmbd_ts_intr_func_set(unit, CMICX_TS_FIFO1_OVERFLOW, NULL, 0);

    SHR_FUNC_EXIT();
}

static int
cmicx2_tsfifo_capture_start(int unit, bcmbd_tsfifo_capture_cb_t *cb)
{
    SHR_FUNC_ENTER(unit);

    if (!cb || !cb->write || !cb->fd) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (fifo_cap_cb[unit].write) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    fifo_cap_cb[unit] = *cb;

    /* Enable FIFO1 interrupt */
    bcmbd_ts_intr_enable(unit, CMICX_TS_FIFO1_NOT_EMPTY);
    bcmbd_ts_intr_enable(unit, CMICX_TS_FIFO1_OVERFLOW);

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx2_tsfifo_capture_stop(int unit)
{
    /* Disable FIFO1 interrupt */
    bcmbd_ts_intr_disable(unit, CMICX_TS_FIFO1_NOT_EMPTY);
    bcmbd_ts_intr_disable(unit, CMICX_TS_FIFO1_OVERFLOW);

    fifo_cap_cb[unit].write = 0;

    return SHR_E_NONE;
}

static int
cmicx2_tsfifo_capture_dump(
        bcmbd_tsfifo_dump_action_f *cb,
        bcmbd_tsfifo_dump_cb_t *app_cb)
{
    void *fd;
    bcmbd_tsfifo_capture_info_t entry;

    if (!app_cb || !app_cb->read || !app_cb->fd) {
        return SHR_E_PARAM;
    }
    fd = app_cb->fd;

    while (app_cb->read(fd, &entry, sizeof(entry)) == sizeof(entry)) {
        if (cb) {
            tsfifo_value_compensate(&entry);
            cb(entry.unit, &entry);
        }
    }
    return SHR_E_NONE;
}

static bcmbd_tsfifo_drv_t cmicx2_tsfifo_drv = {
        .capture_start = cmicx2_tsfifo_capture_start,
        .capture_stop = cmicx2_tsfifo_capture_stop,
        .capture_dump = cmicx2_tsfifo_capture_dump,
        .cleanup = cmicx2_tsfifo_cleanup,
        .init = cmicx2_tsfifo_init
};

/*******************************************************************************
 * Public functions
 */

int
bcmbd_cmicx2_tsfifo_drv_init(int unit)
{
    return bcmbd_tsfifo_drv_init(unit, &cmicx2_tsfifo_drv);
}

int
bcmbd_cmicx2_tsfifo_drv_cleanup(int unit)
{
    return bcmbd_tsfifo_drv_init(unit, NULL);
}

