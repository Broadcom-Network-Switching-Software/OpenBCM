/*! \file bcmbd_cmicx_fifo.c
 *
 * FIFO operation for CMICx-based device.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_assert.h>

#include <shr/shr_error.h>
#include <bcmdrd/bcmdrd_types.h>

#include <bcmbd/bcmbd.h>
#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_cmicx_fifo.h>
#include <bcmbd/bcmbd_cmicx_dump.h>

#include "bcmbd_cmicx_schan.h"

/* Log source for this component */
#define BSL_LOG_MODULE  BSL_LS_BCMBD_SOCMEM

int
bcmbd_cmicx_fifo_pop(int unit, uint32_t adext, uint32_t addr,
                     uint32_t *data, size_t wsize)
{
    int rv;
    int acctype, dstblk, datalen;
    unsigned int idx;
    schan_msg_t schan_msg;

    /* Configure S-Channel parameters */
    acctype = BCMBD_CMICX_ADEXT2ACCTYPE(adext);
    dstblk = BCMBD_CMICX_ADEXT2BLOCK(adext);
    datalen = 4 * wsize;

    /* Write message to S-Channel */
    SCHAN_MSG_CLEAR(&schan_msg);
    SCMH_OPCODE_SET(schan_msg.popcmd.header, FIFO_POP_CMD_MSG);
    SCMH_ACCTYPE_SET(schan_msg.popcmd.header, acctype);
    SCMH_DSTBLK_SET(schan_msg.popcmd.header, dstblk);
    SCMH_DATALEN_SET(schan_msg.popcmd.header, datalen);
    schan_msg.popcmd.address = addr;

    /* Write header word + address DWORD, read header word + data DWORD */
    rv = bcmbd_cmicx_schan_op(unit, &schan_msg, 2, 1 + wsize);
    if (SHR_FAILURE(rv) && SCMH_NACK_GET(schan_msg.genresp.header) == 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "SOC FIFO pop error: "
                              "addr=0x%04"PRIx32"%08"PRIx32"\n"),
                   adext, addr));
        return rv;
    }

    /* Check result */
    if (SCMH_OPCODE_GET(schan_msg.popresp.header) != FIFO_POP_DONE_MSG) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "SOC FIFO pop invalid ACK: "
                              "%"PRIu32" (expected %d) addr=0x%08"PRIx32"\n"),
                   SCMH_OPCODE_GET(schan_msg.popresp.header),
                   FIFO_POP_DONE_MSG, addr));
        return SHR_E_FAIL;
    }

    if (SCMH_NACK_GET(schan_msg.genresp.header)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "SOC FIFO pop NACK: "
                                "resp_word=0x%08"PRIx32" addr=0x%08"PRIx32"\n"),
                     schan_msg.genresp.response, addr));
        return SHR_E_EMPTY;
    }

    for (idx = 0; idx < wsize; idx++) {
        /* Return requested data */
        data[idx] = schan_msg.popresp.data[idx];
    }

    /* Debug output */
    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        shr_pb_t *pb = shr_pb_create();
        bcmbd_cmicx_dump_mem(pb, unit, adext, addr, data, wsize);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "SOC FIFO pop: %s\n"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
    }

    return SHR_E_NONE;
}

int
bcmbd_cmicx_fifo_push(int unit, uint32_t adext, uint32_t addr,
                      uint32_t *data, size_t wsize)
{
    int rv;
    int acctype, dstblk, datalen;
    unsigned int idx;
    schan_msg_t schan_msg;

    /* Configure S-Channel parameters */
    acctype = BCMBD_CMICX_ADEXT2ACCTYPE(adext);
    dstblk = BCMBD_CMICX_ADEXT2BLOCK(adext);
    datalen = 4 * wsize;

    /*
     * Setup S-Channel command packet
     *
     * NOTE: the datalen field matters only for the Push command,
     * where it is used only by the CMIC to determine
     * how much data to send, and is in units of bytes.
     */

    SCHAN_MSG_CLEAR(&schan_msg);
    SCMH_OPCODE_SET(schan_msg.pushcmd.header, FIFO_PUSH_CMD_MSG);
    SCMH_ACCTYPE_SET(schan_msg.pushcmd.header, acctype);
    SCMH_DSTBLK_SET(schan_msg.pushcmd.header, dstblk);
    SCMH_DATALEN_SET(schan_msg.pushcmd.header, datalen);

    schan_msg.pushcmd.address = addr;
    for (idx = 0; idx < wsize; idx++) {
        schan_msg.pushcmd.data[idx] = data[idx];
    }

    rv = bcmbd_cmicx_schan_op(unit, &schan_msg, 2 + wsize, 0);
    if (SHR_FAILURE(rv) && SCMH_NACK_GET(schan_msg.genresp.header) == 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "SOC FIFO push error: "
                              "addr=0x%04"PRIx32"%08"PRIx32"\n"),
                   adext, addr));
        return rv;
    }

    if (SCMH_NACK_GET(schan_msg.genresp.header)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "SOC FIFO push NACK: "
                                "resp_word=0x%08"PRIx32" addr=0x%08"PRIx32"\n"),
                     schan_msg.genresp.response, addr));
        return SHR_E_FULL;
    }

    /* Debug output */
    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        shr_pb_t *pb = shr_pb_create();
        bcmbd_cmicx_dump_mem(pb, unit, adext, addr, data, wsize);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "SOC FIFO push: %s\n"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
    }

    return SHR_E_NONE;
}
