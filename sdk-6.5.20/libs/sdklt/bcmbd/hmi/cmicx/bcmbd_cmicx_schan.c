/*! \file bcmbd_cmicx_schan.c
 *
 * S-Channel (internal command bus) support
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_assert.h>
#include <sal/sal_spinlock.h>

#include <shr/shr_debug.h>
#include <shr/shr_pb.h>

#include <bcmbd/bcmbd_cmicx_acc.h>
#include <bcmbd/bcmbd_cmicx_dump.h>
#include <bcmbd/bcmbd_cmicx_schan_error.h>
#include <bcmbd/bcmbd_simhook.h>
#include <bcmbd/bcmbd_sim.h>

#include "bcmbd_cmicx_schan.h"

/* Log source for this component */
#define BSL_LOG_MODULE  BSL_LS_BCMBD_SCHAN

/*******************************************************************************
 * Local definitions
 */

/*! Maximum polls for S-channel operation to complete */
#ifndef BCMBD_CMICX_SCHAN_MAX_POLLS
#define BCMBD_CMICX_SCHAN_MAX_POLLS     100
#endif

/*! S-channel control */
typedef struct schan_ctrl_s {
    sal_spinlock_t lock;
    int ch_map;
} schan_ctrl_t;

/*! Per-unit S-channel control object */
static schan_ctrl_t schan_ctrl[BCMDRD_CONFIG_MAX_UNITS];

/*! Per-unit S-channel error handler */
static bcmbd_cmicx_schan_error_f schan_efunc[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Local functions
 */

/*
 * Resets the CMIC S-Channel interface. This is required when we sent
 * a message and did not receive a response after the poll count was
 * exceeded.
 */
static void
schan_reset(int unit, int ch)
{
    CMIC_SCHAN_CTRLr_t ctrl;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "S-channel reset\n")));

    /* Toggle S-Channel abort bit in CMIC_SCHAN_CTRL register */
    CMIC_SCHAN_CTRLr_CLR(ctrl);
    CMIC_SCHAN_CTRLr_ABORTf_SET(ctrl, 1);
    WRITE_CMIC_SCHAN_CTRLr(unit, ch, ctrl);

    SAL_CONFIG_MEMORY_BARRIER;

    CMIC_SCHAN_CTRLr_ABORTf_SET(ctrl, 0);
    WRITE_CMIC_SCHAN_CTRLr(unit, ch, ctrl);

    SAL_CONFIG_MEMORY_BARRIER;
}

/*
 * Gets an idle S-channel thru which the PIO operation will be
 * performed.
 */
static void
schan_get(int unit, int *ch)
{
    int i;

    while (1) {
        sal_spinlock_lock(schan_ctrl[unit].lock);
        for (i = 0; i < CMICX_SCHAN_NUM_MAX; i++) {
           if (schan_ctrl[unit].ch_map & 0x01 << i) {
               *ch = i;
               schan_ctrl[unit].ch_map &= ~(0x01 << i);
               sal_spinlock_unlock(schan_ctrl[unit].lock);
               return;
           }
       }
       sal_spinlock_unlock(schan_ctrl[unit].lock);
    }
}

/*
 * Puts an S-channel into the idle channel pool.
 */
static void
schan_put(int unit, int ch)
{
    sal_spinlock_lock(schan_ctrl[unit].lock);
    schan_ctrl[unit].ch_map |= (0x01 << ch);
    sal_spinlock_unlock(schan_ctrl[unit].lock);
}

/*
 * Get adjusted error status since.
 */
static int
schan_error(int unit, int ch, uint32_t *schan_msg,
            uint32_t ctl_val, uint32_t err_val)
{
    int rv = SHR_E_FAIL;
    CMIC_SCHAN_CTRLr_t ctrl;
    CMIC_SCHAN_ERRr_t err;
    shr_pb_t *pb;

    if (schan_efunc[unit]) {
        rv = schan_efunc[unit](unit, ch, schan_msg, ctl_val);
        if (rv == SHR_E_NONE) {
            return rv;
        }
    }

    CMIC_SCHAN_CTRLr_SET(ctrl, ctl_val);
    pb = NULL;
    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        pb = shr_pb_create();
    }
    if (CMIC_SCHAN_CTRLr_NACKf_GET(ctrl)) {
        shr_pb_printf(pb, " NACK");
    }
    if (CMIC_SCHAN_CTRLr_SER_CHECK_FAILf_GET(ctrl)) {
        shr_pb_printf(pb, " SER");
    }
    if (CMIC_SCHAN_CTRLr_TIMEOUTf_GET(ctrl)) {
        shr_pb_printf(pb, " TO");
    }
    CMIC_SCHAN_ERRr_SET(err, err_val);
    shr_pb_printf(pb, " "
                  "NACK=%d ERR_CODE=%d ERRBIT=%d DATA_LEN=%d",
                  CMIC_SCHAN_ERRr_NACKf_GET(err),
                  CMIC_SCHAN_ERRr_ERR_CODEf_GET(err),
                  CMIC_SCHAN_ERRr_ERRBITf_GET(err),
                  CMIC_SCHAN_ERRr_DATA_LENf_GET(err));
    if (ctl_val != 0) {
        /*
         * The fields are only valid if the error value is obtained
         * from the error register and not the S-cahnnel message
         * header.
         */
        shr_pb_printf(pb, " "
                      "SRC_PORT=%d DST_PORT=%d OP_CODE=%d",
                      CMIC_SCHAN_ERRr_SRC_PORTf_GET(err),
                      CMIC_SCHAN_ERRr_DST_PORTf_GET(err),
                      CMIC_SCHAN_ERRr_OP_CODEf_GET(err));
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "S-channel (%d) error:%s\n"),
                 ch, shr_pb_str(pb)));
    shr_pb_destroy(pb);

    return rv;
}

/*
 * Dump S-channel buffer.
 */
static void
cmicx_schan_debug_dump(int unit, int ch, const char *op_str,
                       uint32_t *dwords, size_t num_words)
{
    shr_pb_t *pb = shr_pb_create();
    bcmbd_cmicx_dump_data(pb, dwords, num_words);
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "S-channel (%d) %s:%s\n"),
               ch, op_str, shr_pb_str(pb)));
    shr_pb_destroy(pb);
}

/*******************************************************************************
 * Public functions
 */

int
bcmbd_cmicx_schan_init(int unit)
{
    if (schan_ctrl[unit].lock) {
        return SHR_E_INTERNAL;
    }
    schan_ctrl[unit].lock = sal_spinlock_create("schan lock");
    if (schan_ctrl[unit].lock == NULL) {
        return SHR_E_RESOURCE;
    }
    schan_ctrl[unit].ch_map = (0x01 << CMICX_SCHAN_NUM_MAX) -1;
    return SHR_E_NONE;
}

int
bcmbd_cmicx_schan_cleanup(int unit)
{
    if (schan_ctrl[unit].lock) {
        sal_spinlock_destroy(schan_ctrl[unit].lock);
        schan_ctrl[unit].lock = NULL;
        schan_ctrl[unit].ch_map = 0;
    }
    schan_efunc[unit] = NULL;
    return SHR_E_NONE;
}

int
bcmbd_cmicx_schan_raw_op(int unit, uint32_t *buf,
                         int dwc_write, int dwc_read, int *dwc_max)
{
    if (buf == NULL) {
        if (dwc_max == NULL) {
            return SHR_E_PARAM;
        }
        *dwc_max = CMIC_SCHAN_WORDS_ALLOC;
        return SHR_E_NONE;
    }

    return bcmbd_cmicx_schan_op(unit, (schan_msg_t *)buf, dwc_write, dwc_read);
}

int
bcmbd_cmicx_schan_op(int unit,
                     schan_msg_t *msg,
                     int dwc_write, int dwc_read)
{
    int i, rv = SHR_E_NONE;
    int ch = 0;
    uint32_t polls = 0;
    CMIC_SCHAN_CTRLr_t ctrl;
    CMIC_SCHAN_ERRr_t err;
    uint32_t msg_addr;

    assert(dwc_write <= CMIC_SCHAN_WORDS_ALLOC);
    assert(dwc_read <= CMIC_SCHAN_WORDS_ALLOC);

    /* Debug output */
    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        cmicx_schan_debug_dump(unit, ch, "write", msg->dwords, dwc_write);
    }

    if (bcmbd_simhook_schan_op) {
        rv = bcmbd_simhook_schan_op(unit, msg->dwords, dwc_write, dwc_read);
        if (SHR_SUCCESS(rv)) {
            uint32_t schan_hdr = msg->genresp.header;
            if (SCMH_NACK_GET(schan_hdr) || SCMH_EBIT_GET(schan_hdr)) {
                rv = schan_error(unit, ch, msg->dwords, 0, schan_hdr);
            }
            /* Debug output */
            if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
                cmicx_schan_debug_dump(unit, ch, "read", msg->dwords, dwc_read);
            }
        }
        return rv;
    }

    schan_get(unit, &ch);

    /* S-Channel message buffer address */
    msg_addr = CMIC_SCHAN_MESSAGEr_OFFSET + (ch * 0x100);

    /* Write raw S-Channel Data: dwc_write words */
    if (BCMBD_CMICX_USE_64BIT()) {
        /* S-channel message buffer is not 64-bit aligned */
        BCMBD_CMICX_WRITE(unit, msg_addr, msg->dwords[0]);
        for (i = 1; i < dwc_write; i += 2) {
            uint64_t data64;
            BCMBD_CMICX_DATA64_ENCODE(data64, &msg->dwords[i]);
            BCMBD_CMICX_WRITE64(unit, msg_addr + i*4, data64);
        }
    } else {
        for (i = 0; i < dwc_write; i++) {
            BCMBD_CMICX_WRITE(unit, msg_addr + i*4, msg->dwords[i]);
        }
    }

    /* Tell CMIC to start */
    CMIC_SCHAN_CTRLr_CLR(ctrl);
    CMIC_SCHAN_CTRLr_MSG_STARTf_SET(ctrl, 1);
    WRITE_CMIC_SCHAN_CTRLr(unit, ch, ctrl);

    SAL_CONFIG_MEMORY_BARRIER;

    /* Poll for completion */
    for (polls = 0; polls < BCMBD_CMICX_SCHAN_MAX_POLLS; polls++) {
        READ_CMIC_SCHAN_CTRLr(unit, ch, &ctrl);
        if (CMIC_SCHAN_CTRLr_MSG_DONEf_GET(ctrl)) {
            break;
        }
    }

    /* Check for completion timeout */
    if (polls == BCMBD_CMICX_SCHAN_MAX_POLLS) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "S-channel timeout\n")));
        rv = SHR_E_TIMEOUT;
    }

    /* Check for error conditions */
    if (CMIC_SCHAN_CTRLr_SCHAN_ERRORf_GET(ctrl)) {
        READ_CMIC_SCHAN_ERRr(unit, ch, &err);
        rv = schan_error(unit, ch, msg->dwords,
                         CMIC_SCHAN_CTRLr_GET(ctrl),
                         CMIC_SCHAN_ERRr_GET(err));
    }

    SAL_CONFIG_MEMORY_BARRIER;

    if (rv == SHR_E_TIMEOUT) {
        schan_reset(unit, ch);
    } else {
        /* Read in data from S-Channel buffer space, if any */
        for (i = 0; i < dwc_read; i++) {
            BCMBD_CMICX_READ(unit, msg_addr + 4*i, &msg->dwords[i]);
        }

        /* Debug output */
        if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
            cmicx_schan_debug_dump(unit, ch, "read", msg->dwords, dwc_read);
        }
    }

    schan_put(unit, ch);

    return rv;
}

int
bcmbd_cmicx_schan_error_func_set(int unit, bcmbd_cmicx_schan_error_f efunc)
{
    schan_efunc[unit] = efunc;

    return SHR_E_NONE;
}
