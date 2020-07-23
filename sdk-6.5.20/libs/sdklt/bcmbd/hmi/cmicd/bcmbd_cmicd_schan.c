/*! \file bcmbd_cmicd_schan.c
 *
 * S-Channel (internal command bus) support
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_assert.h>
#include <sal/sal_mutex.h>

#include <shr/shr_debug.h>
#include <shr/shr_pb.h>

#include <bcmbd/bcmbd_cmicd_acc.h>
#include <bcmbd/bcmbd_cmicd_dump.h>
#include <bcmbd/bcmbd_simhook.h>
#include <bcmbd/bcmbd_sim.h>

#include "bcmbd_cmicd_schan.h"

/* Log source for this component */
#define BSL_LOG_MODULE  BSL_LS_BCMBD_SCHAN

/*******************************************************************************
 * Local definitions
 */

/*! Maximum polls for S-channel operation to complete */
#ifndef BCMBD_CMICD_SCHAN_MAX_POLLS
#define BCMBD_CMICD_SCHAN_MAX_POLLS     100
#endif

/*! S-channel operation lock */
static sal_mutex_t schan_lock[BCMDRD_CONFIG_MAX_UNITS];

#define SCHAN_LOCK(_u) sal_mutex_take(schan_lock[_u], SAL_MUTEX_FOREVER)
#define SCHAN_UNLOCK(_u) sal_mutex_give(schan_lock[_u])

/*******************************************************************************
 * Local functions
 */

/*
 * Resets the CMIC S-Channel interface. This is required when we sent
 * a message and did not receive a response after the poll count was
 * exceeded.
 */
static void
schan_reset(int unit, int cmc)
{
    CMIC_CMC_SCHAN_CTRLr_t schan_ctrl;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "S-channel reset\n")));

    /* Toggle S-Channel abort bit in CMIC_CMC_SCHAN_CTRL register */
    CMIC_CMC_SCHAN_CTRLr_CLR(schan_ctrl);
    CMIC_CMC_SCHAN_CTRLr_ABORTf_SET(schan_ctrl, 1);
    WRITE_CMIC_CMC_SCHAN_CTRLr(unit, cmc, schan_ctrl);

    SAL_CONFIG_MEMORY_BARRIER;

    CMIC_CMC_SCHAN_CTRLr_ABORTf_SET(schan_ctrl, 0);
    WRITE_CMIC_CMC_SCHAN_CTRLr(unit, cmc, schan_ctrl);

    SAL_CONFIG_MEMORY_BARRIER;
}

/*
 * Get adjusted error status since.
 */
static int
schan_error(int unit, int cmc, uint32_t *schan_msg,
            uint32_t ctl_val, uint32_t err_val)
{
    CMIC_CMC_SCHAN_CTRLr_t schan_ctrl;
    CMIC_CMC_SCHAN_ERRr_t err;
    shr_pb_t *pb;

    CMIC_CMC_SCHAN_CTRLr_SET(schan_ctrl, ctl_val);
    pb = NULL;
    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        pb = shr_pb_create();
    }
    if (CMIC_CMC_SCHAN_CTRLr_NACKf_GET(schan_ctrl)) {
        shr_pb_printf(pb, " NACK");
    }
    if (CMIC_CMC_SCHAN_CTRLr_SER_CHECK_FAILf_GET(schan_ctrl)) {
        shr_pb_printf(pb, " SER");
    }
    if (CMIC_CMC_SCHAN_CTRLr_TIMEOUTf_GET(schan_ctrl)) {
        shr_pb_printf(pb, " TO");
    }
    CMIC_CMC_SCHAN_ERRr_SET(err, err_val);
    shr_pb_printf(pb, " "
                  "NACK=%d ERR_CODE=%d ERRBIT=%d DATA_LEN=%d",
                  CMIC_CMC_SCHAN_ERRr_NACKf_GET(err),
                  CMIC_CMC_SCHAN_ERRr_ERR_CODEf_GET(err),
                  CMIC_CMC_SCHAN_ERRr_ERRBITf_GET(err),
                  CMIC_CMC_SCHAN_ERRr_DATA_LENf_GET(err));
    if (ctl_val != 0) {
        /*
         * The fields are only valid if the error value is obtained
         * from the error register and not the S-cahnnel message
         * header.
         */
        shr_pb_printf(pb, " "
                      "SRC_PORT=%d DST_PORT=%d OP_CODE=%d",
                      CMIC_CMC_SCHAN_ERRr_SRC_PORTf_GET(err),
                      CMIC_CMC_SCHAN_ERRr_DST_PORTf_GET(err),
                      CMIC_CMC_SCHAN_ERRr_OP_CODEf_GET(err));
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "S-channel (%d) error:%s\n"),
                 cmc, shr_pb_str(pb)));
    shr_pb_destroy(pb);
    return SHR_E_FAIL;
}

/*
 * Dump S-channel buffer.
 */
static void
cmicd_schan_debug_dump(int unit, int cmc, const char *op_str,
                       uint32_t *dwords, size_t num_words)
{
    shr_pb_t *pb = shr_pb_create();
    bcmbd_cmicd_dump_data(pb, dwords, num_words);
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "S-channel (%d) %s:%s\n"),
               cmc, op_str, shr_pb_str(pb)));
    shr_pb_destroy(pb);
}

/*******************************************************************************
 * Public functions
 */

int
bcmbd_cmicd_schan_init(int unit)
{
    if (schan_lock[unit]) {
        return SHR_E_INTERNAL;
    }
    schan_lock[unit] = sal_mutex_create("schan_op");
    if (schan_lock[unit] == NULL) {
        return SHR_E_RESOURCE;
    }
    return SHR_E_NONE;
}

int
bcmbd_cmicd_schan_cleanup(int unit)
{
    if (schan_lock[unit]) {
        sal_mutex_destroy(schan_lock[unit]);
        schan_lock[unit] = NULL;
    }
    return SHR_E_NONE;
}

int
bcmbd_cmicd_schan_raw_op(int unit, uint32_t *buf,
                         int dwc_write, int dwc_read, int *dwc_max)
{
    if (buf == NULL) {
        if (dwc_max == NULL) {
            return SHR_E_PARAM;
        }
        *dwc_max = CMIC_SCHAN_WORDS_ALLOC;
        return SHR_E_NONE;
    }

    return bcmbd_cmicd_schan_op(unit, (schan_msg_t *)buf, dwc_write, dwc_read);
}

int
bcmbd_cmicd_schan_op(int unit,
                     schan_msg_t *msg,
                     int dwc_write, int dwc_read)
{
    int i, rv = SHR_E_NONE;
    int cmc = BCMBD_CMICD_CMC_GET(unit);
    uint32_t polls = 0;
    CMIC_CMC_SCHAN_CTRLr_t schan_ctrl;
    CMIC_CMC_SCHAN_ERRr_t schan_err;
    uint32_t msg_addr;

    assert(dwc_write <= CMIC_SCHAN_WORDS_ALLOC);
    assert(dwc_read <= CMIC_SCHAN_WORDS_ALLOC);

    /* Debug output */
    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        cmicd_schan_debug_dump(unit, cmc, "write", msg->dwords, dwc_write);
    }

    if (bcmbd_simhook_schan_op) {
        rv = bcmbd_simhook_schan_op(unit, msg->dwords, dwc_write, dwc_read);
        if (SHR_SUCCESS(rv)) {
            uint32_t schan_hdr = msg->genresp.header;
            if (SCMH_NACK_GET(schan_hdr) || SCMH_EBIT_GET(schan_hdr)) {
                rv = schan_error(unit, cmc, msg->dwords, 0, schan_hdr);
            }
            /* Debug output */
            if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
                cmicd_schan_debug_dump(unit, cmc, "read", msg->dwords, dwc_read);
            }
        }
        return rv;
    }

    SCHAN_LOCK(unit);

    /* S-Channel message buffer address */
    msg_addr = CMIC_CMC_SCHAN_MESSAGEr_OFFSET;

    /* Write raw S-Channel Data: dwc_write words */
    for (i = 0; i < dwc_write; i++) {
        BCMBD_CMICD_CMC_WRITE(unit, msg_addr + i*4, msg->dwords[i]);
    }

    /* Tell CMIC to start */
    CMIC_CMC_SCHAN_CTRLr_CLR(schan_ctrl);
    CMIC_CMC_SCHAN_CTRLr_MSG_STARTf_SET(schan_ctrl, 1);
    WRITE_CMIC_CMC_SCHAN_CTRLr(unit, cmc, schan_ctrl);

    SAL_CONFIG_MEMORY_BARRIER;

    /* Poll for completion */
    for (polls = 0; polls < BCMBD_CMICD_SCHAN_MAX_POLLS; polls++) {
        READ_CMIC_CMC_SCHAN_CTRLr(unit, cmc, &schan_ctrl);
        if (CMIC_CMC_SCHAN_CTRLr_MSG_DONEf_GET(schan_ctrl)) {
            break;
        }
    }

    /* Check for completion timeout */
    if (polls == BCMBD_CMICD_SCHAN_MAX_POLLS) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "S-channel timeout\n")));
        rv = SHR_E_TIMEOUT;
    }

    /* Check for error conditions */
    if (CMIC_CMC_SCHAN_CTRLr_SCHAN_ERRORf_GET(schan_ctrl)) {
        READ_CMIC_CMC_SCHAN_ERRr(unit, cmc, &schan_err);
        rv = schan_error(unit, cmc, msg->dwords,
                         CMIC_CMC_SCHAN_CTRLr_GET(schan_ctrl),
                         CMIC_CMC_SCHAN_ERRr_GET(schan_err));
    }

    SAL_CONFIG_MEMORY_BARRIER;

    if (rv == SHR_E_TIMEOUT) {
        schan_reset(unit, cmc);
    } else {
        /* Read in data from S-Channel buffer space, if any */
        for (i = 0; i < dwc_read; i++) {
            BCMBD_CMICD_CMC_READ(unit, msg_addr + 4*i, &msg->dwords[i]);
        }

        /* Debug output */
        if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
            cmicd_schan_debug_dump(unit, cmc, "read", msg->dwords, dwc_read);
        }
    }

    SCHAN_UNLOCK(unit);

    return rv;
}
