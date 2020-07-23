/*! \file bcmbd_cmicx_reg.c
 *
 * <description>
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_cmicx_reg.h>
#include <bcmbd/bcmbd_cmicx_dump.h>
#include <bcmbd/bcmbd_simhook.h>

#include "bcmbd_cmicx_schan.h"

/* Log source for this component */
#define BSL_LOG_MODULE  BSL_LS_BCMBD_SOCREG

int
bcmbd_cmicx_reg_read(int unit, uint32_t adext, uint32_t addr,
                     uint32_t *data, size_t wsize)
{
    int rv;
    int acctype, dstblk, datalen;
    unsigned int idx;
    schan_msg_t schan_msg;

    SHR_FUNC_ENTER(unit);

    /* Check for pipeline bypass */
    if (bcmbd_addr_bypass(unit, adext, addr)) {
        SHR_EXIT();
    }

    /* Simulator hooks */
    if (bcmbd_simhook_read) {
        SHR_IF_ERR_EXIT
            (bcmbd_simhook_read(unit, adext | BCMBD_SIM_SOC_REG, addr,
                                data, BCMDRD_WORDS2BYTES(wsize)));
        /* Debug output */
        if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
            shr_pb_t *pb = shr_pb_create();
            bcmbd_cmicx_dump_reg(pb, unit, adext, addr, data, wsize);
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Sim reg read: %s\n"), shr_pb_str(pb)));
            shr_pb_destroy(pb);
        }
        SHR_EXIT();
    }

    /* Configure S-Channel parameters */
    acctype = BCMBD_CMICX_ADEXT2ACCTYPE(adext);
    dstblk = BCMBD_CMICX_ADEXT2BLOCK(adext);
    datalen = 4 * wsize;

    /* Write message to S-Channel */
    SCHAN_MSG_CLEAR(&schan_msg);
    SCMH_OPCODE_SET(schan_msg.readcmd.header, READ_REGISTER_CMD_MSG);
    SCMH_ACCTYPE_SET(schan_msg.readcmd.header, acctype);
    SCMH_DSTBLK_SET(schan_msg.readcmd.header, dstblk);
    SCMH_DATALEN_SET(schan_msg.readcmd.header, datalen);
    schan_msg.readcmd.address = addr;

    /* Write header word + address DWORD, read header word + data DWORD */
    rv = bcmbd_cmicx_schan_op(unit, &schan_msg, 2, 1 + wsize);
    if (SHR_FAILURE(rv)) {
        shr_pb_t *pb = shr_pb_create();
        bcmbd_cmicx_dump_reg(pb, unit, adext, addr, data, wsize);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "SOC reg read error: %s\n"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
        SHR_IF_ERR_EXIT(rv);
    }

    /* Check result */
    if (SCMH_OPCODE_GET(schan_msg.readresp.header) != READ_REGISTER_ACK_MSG) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "SOC reg read invalid ACK: "
                              "%"PRIu32" (expected %d) addr=0x%08"PRIx32"\n"),
                   SCMH_OPCODE_GET(schan_msg.readresp.header),
                   READ_REGISTER_ACK_MSG, addr));
        SHR_IF_ERR_EXIT(SHR_E_FAIL);
    }

    for (idx = 0; idx < wsize; idx++) {
        /* Return requested data */
        data[idx] = schan_msg.readresp.data[idx];
    }

    /* Debug output */
    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        shr_pb_t *pb = shr_pb_create();
        bcmbd_cmicx_dump_reg(pb, unit, adext, addr, data, wsize);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "SOC reg read: %s\n"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
    }

  exit:
    SHR_FUNC_EXIT();
}

int
bcmbd_cmicx_reg_write(int unit, uint32_t adext, uint32_t addr,
                      uint32_t *data, size_t wsize)
{
    int rv;
    int acctype, dstblk, datalen;
    unsigned int idx;
    schan_msg_t schan_msg;

    SHR_FUNC_ENTER(unit);

    /* Check for pipeline bypass */
    if (bcmbd_addr_bypass(unit, adext, addr)) {
        SHR_EXIT();
    }

    /* Simulator hooks */
    if (bcmbd_simhook_write) {
        SHR_IF_ERR_EXIT
            (bcmbd_simhook_write(unit, adext | BCMBD_SIM_SOC_REG, addr,
                                 data, BCMDRD_WORDS2BYTES(wsize)));
        /* Debug output */
        if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
            shr_pb_t *pb = shr_pb_create();
            bcmbd_cmicx_dump_reg(pb, unit, adext, addr, data, wsize);
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Sim reg write: %s\n"), shr_pb_str(pb)));
            shr_pb_destroy(pb);
        }
        SHR_EXIT();
    }

    /* Configure S-Channel parameters */
    acctype = BCMBD_CMICX_ADEXT2ACCTYPE(adext);
    dstblk = BCMBD_CMICX_ADEXT2BLOCK(adext);
    datalen = 4 * wsize;

    /*
     * Setup S-Channel command packet
     *
     * NOTE: the datalen field matters only for the Write Memory and
     * Write Register commands, where it is used only by the CMIC to
     * determine how much data to send, and is in units of bytes.
     */

    SCHAN_MSG_CLEAR(&schan_msg);
    SCMH_OPCODE_SET(schan_msg.writecmd.header, WRITE_REGISTER_CMD_MSG);
    SCMH_ACCTYPE_SET(schan_msg.writecmd.header, acctype);
    SCMH_DSTBLK_SET(schan_msg.writecmd.header, dstblk);
    SCMH_DATALEN_SET(schan_msg.writecmd.header, datalen);

    schan_msg.writecmd.address = addr;
    for (idx = 0; idx < wsize; idx++) {
        schan_msg.writecmd.data[idx] = data[idx];
    }

    rv = bcmbd_cmicx_schan_op(unit, &schan_msg, 2 + wsize, 0);
    if (SHR_FAILURE(rv)) {
        shr_pb_t *pb = shr_pb_create();
        bcmbd_cmicx_dump_reg(pb, unit, adext, addr, data, wsize);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "SOC reg write error: %s\n"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
        SHR_IF_ERR_EXIT(rv);
    }

    /* Debug output */
    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        shr_pb_t *pb = shr_pb_create();
        bcmbd_cmicx_dump_reg(pb, unit, adext, addr, data, wsize);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "SOC reg write: %s\n"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
    }

  exit:
    SHR_FUNC_EXIT();
}
