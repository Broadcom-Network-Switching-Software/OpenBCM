/*! \file bcmbd_cmicd_tbl.c
 *
 * Table operations in CMICd
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_error.h>
#include <bcmdrd/bcmdrd_types.h>

#include <bcmbd/bcmbd.h>
#include <bcmbd/bcmbd_cmicd.h>
#include <bcmbd/bcmbd_cmicd_tbl.h>
#include <bcmbd/bcmbd_cmicd_dump.h>

#include "bcmbd_cmicd_schan.h"

/* Log source for this component */
#define BSL_LOG_MODULE  BSL_LS_BCMBD_SOCMEM

/* Memroy index of table operation result. */
#define DEFAULT_RESP_WORD_INDEX_GET(_w)  ((_w) & 0xfffff)

static int
cmicd_default_tbl_insert(int unit, uint32_t adext, uint32_t addr,
                         uint32_t *data, size_t wsize, uint32_t bank_ignore_mask,
                         bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data)
{
    int rv;
    int acctype, dstblk, datalen;
    unsigned int idx;
    schan_msg_t schan_msg;

    /* Configure S-Channel parameters */
    acctype = BCMBD_CMICD_ADEXT2ACCTYPE(adext);
    dstblk = BCMBD_CMICD_ADEXT2BLOCK(adext);
    datalen = 4 * wsize;

    /* Setup S-Channel command packet */
    SCHAN_MSG_CLEAR(&schan_msg);
    SCMH_OPCODE_SET(schan_msg.gencmd.header, TABLE_INSERT_CMD_MSG);
    SCMH_ACCTYPE_SET(schan_msg.gencmd.header, acctype);
    SCMH_DSTBLK_SET(schan_msg.gencmd.header, dstblk);
    SCMH_DATALEN_SET(schan_msg.gencmd.header, datalen);
    SCMH_BANK_SET(schan_msg.gencmd.header, bank_ignore_mask);

    schan_msg.gencmd.address = addr;
    for (idx = 0; idx < wsize; idx++) {
        schan_msg.gencmd.data[idx] = data[idx];
    }

    rv = bcmbd_cmicd_schan_op(unit, &schan_msg, 2 + wsize, 2 + wsize);
    if (SHR_FAILURE(rv) && SCMH_NACK_GET(schan_msg.genresp.header) == 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "SOC table insert error: "
                              "addr=0x%04"PRIx32"%08"PRIx32"\n"),
                   adext, addr));
        /* SHR_E_FAIL is reserved for NACK-bit inserted condition. */
        return (rv == SHR_E_FAIL) ? SHR_E_INTERNAL : rv;
    }

    /* Check result */
    if (SCMH_OPCODE_GET(schan_msg.genresp.header) != TABLE_INSERT_DONE_MSG) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "SOC table insert invalid ACK: "
                              "%"PRIu32" (expected %d) addr=0x%08"PRIx32"\n"),
                   SCMH_OPCODE_GET(schan_msg.readresp.header),
                   TABLE_INSERT_DONE_MSG, addr));
        return SHR_E_INTERNAL;
    }

    /* Get response information */
    if (resp_info) {
        resp_info->resp_word = schan_msg.genresp.response;
    }

    if (SCMH_NACK_GET(schan_msg.genresp.header)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "SOC table insert NACK: "
                                "resp_word=0x%08"PRIx32" addr=0x%08"PRIx32"\n"),
                     schan_msg.genresp.response, addr));
        return SHR_E_FAIL;
    }

    /* Get response data if NACK is not set. */
    if (resp_data) {
        for (idx = 0; idx < wsize; idx++) {
            resp_data[idx] = schan_msg.genresp.data[idx];
        }
    }

    /* Debug output */
    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        shr_pb_t *pb = shr_pb_create();
        int memidx = DEFAULT_RESP_WORD_INDEX_GET(schan_msg.genresp.response);
        bcmbd_cmicd_dump_mem(pb, unit, adext, addr + memidx, resp_data, wsize);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "SOC table insert: %s\n"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
    }

    return SHR_E_NONE;
}

static int
cmicd_default_tbl_delete(int unit, uint32_t adext, uint32_t addr,
                         uint32_t *data, size_t wsize, uint32_t bank_ignore_mask,
                         bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data)
{
    int rv;
    int acctype, dstblk, datalen;
    unsigned int idx;
    schan_msg_t schan_msg;

    /* Configure S-Channel parameters */
    acctype = BCMBD_CMICD_ADEXT2ACCTYPE(adext);
    dstblk = BCMBD_CMICD_ADEXT2BLOCK(adext);
    datalen = 4 * wsize;

    /* Setup S-Channel command packet */
    SCHAN_MSG_CLEAR(&schan_msg);
    SCMH_OPCODE_SET(schan_msg.gencmd.header, TABLE_DELETE_CMD_MSG);
    SCMH_ACCTYPE_SET(schan_msg.gencmd.header, acctype);
    SCMH_DSTBLK_SET(schan_msg.gencmd.header, dstblk);
    SCMH_DATALEN_SET(schan_msg.gencmd.header, datalen);
    SCMH_BANK_SET(schan_msg.gencmd.header, bank_ignore_mask);

    schan_msg.gencmd.address = addr;
    for (idx = 0; idx < wsize; idx++) {
        schan_msg.gencmd.data[idx] = data[idx];
    }

    rv = bcmbd_cmicd_schan_op(unit, &schan_msg, 2 + wsize, 2 + wsize);
    if (SHR_FAILURE(rv) && SCMH_NACK_GET(schan_msg.genresp.header) == 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "SOC table delete error: "
                              "addr=0x%04"PRIx32"%08"PRIx32"\n"),
                   adext, addr));
        /* SHR_E_FAIL is reserved for NACK-bit inserted condition. */
        return (rv == SHR_E_FAIL) ? SHR_E_INTERNAL : rv;
    }

    /* Check result */
    if (SCMH_OPCODE_GET(schan_msg.genresp.header) != TABLE_DELETE_DONE_MSG) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "SOC table delete invalid ACK: "
                              "%"PRIu32" (expected %d) addr=0x%08"PRIx32"\n"),
                   SCMH_OPCODE_GET(schan_msg.readresp.header),
                   TABLE_DELETE_DONE_MSG, addr));
        return SHR_E_INTERNAL;
    }

    /* Get response information */
    if (resp_info) {
        resp_info->resp_word = schan_msg.genresp.response;
    }

    if (SCMH_NACK_GET(schan_msg.genresp.header)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "SOC table delete NACK: "
                                "resp_word=0x%08"PRIx32" addr=0x%08"PRIx32"\n"),
                     schan_msg.genresp.response, addr));
        return SHR_E_FAIL;
    }

    /* Get response data if NACK is not set. */
    if (resp_data) {
        for (idx = 0; idx < wsize; idx++) {
            resp_data[idx] = schan_msg.genresp.data[idx];
        }
    }

    /* Debug output */
    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        shr_pb_t *pb = shr_pb_create();
        int memidx = DEFAULT_RESP_WORD_INDEX_GET(schan_msg.genresp.response);
        bcmbd_cmicd_dump_mem(pb, unit, adext, addr + memidx, resp_data, wsize);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "SOC table delete: %s\n"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
    }

    return SHR_E_NONE;
}

static int
cmicd_default_tbl_lookup(int unit, uint32_t adext, uint32_t addr,
                         uint32_t *data, size_t wsize, uint32_t bank_ignore_mask,
                         bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data)
{
    int rv;
    int acctype, dstblk, datalen;
    unsigned int idx;
    schan_msg_t schan_msg;

    /* Configure S-Channel parameters */
    acctype = BCMBD_CMICD_ADEXT2ACCTYPE(adext);
    dstblk = BCMBD_CMICD_ADEXT2BLOCK(adext);
    datalen = 4 * wsize;

    /* Setup S-Channel command packet */
    SCHAN_MSG_CLEAR(&schan_msg);
    SCMH_OPCODE_SET(schan_msg.gencmd.header, TABLE_LOOKUP_CMD_MSG);
    SCMH_ACCTYPE_SET(schan_msg.gencmd.header, acctype);
    SCMH_DSTBLK_SET(schan_msg.gencmd.header, dstblk);
    SCMH_DATALEN_SET(schan_msg.gencmd.header, datalen);
    SCMH_BANK_SET(schan_msg.gencmd.header, bank_ignore_mask);

    schan_msg.gencmd.address = addr;
    for (idx = 0; idx < wsize; idx++) {
        schan_msg.gencmd.data[idx] = data[idx];
    }

    rv = bcmbd_cmicd_schan_op(unit, &schan_msg, 2 + wsize, 2 + wsize);
    if (SHR_FAILURE(rv) && SCMH_NACK_GET(schan_msg.genresp.header) == 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "SOC table lookup error: "
                              "addr=0x%04"PRIx32"%08"PRIx32"\n"),
                   adext, addr));
        /* SHR_E_FAIL is reserved for NACK-bit inserted condition. */
        return (rv == SHR_E_FAIL) ? SHR_E_INTERNAL : rv;
    }

    /* Check result */
    if (SCMH_OPCODE_GET(schan_msg.genresp.header) != TABLE_LOOKUP_DONE_MSG) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "SOC table lookup invalid ACK: "
                              "%"PRIu32" (expected %d) addr=0x%08"PRIx32"\n"),
                   SCMH_OPCODE_GET(schan_msg.readresp.header),
                   TABLE_LOOKUP_DONE_MSG, addr));
        return SHR_E_INTERNAL;
    }

    /* Get response information */
    if (resp_info) {
        resp_info->resp_word = schan_msg.genresp.response;
    }

    if (SCMH_NACK_GET(schan_msg.genresp.header)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "SOC table lookup NACK: "
                                "resp_word=0x%08"PRIx32" addr=0x%08"PRIx32"\n"),
                     schan_msg.genresp.response, addr));
        return SHR_E_FAIL;
    }

    /* Get response data if NACK is not set. */
    if (resp_data) {
        for (idx = 0; idx < wsize; idx++) {
            resp_data[idx] = schan_msg.genresp.data[idx];
        }
    }

    /* Debug output */
    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        shr_pb_t *pb = shr_pb_create();
        int memidx = DEFAULT_RESP_WORD_INDEX_GET(schan_msg.genresp.response);
        bcmbd_cmicd_dump_mem(pb, unit, adext, addr + memidx, resp_data, wsize);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "SOC table lookup: %s\n"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
    }

    return SHR_E_NONE;
}

int
bcmbd_cmicd_tbl_insert(int unit, uint32_t adext, uint32_t addr,
                       uint32_t *data, size_t wsize, uint32_t bank_ignore_mask,
                       bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data)
{
    return cmicd_default_tbl_insert(unit, adext, addr, data, wsize,
                                    bank_ignore_mask, resp_info, resp_data);
}

int
bcmbd_cmicd_tbl_delete(int unit, uint32_t adext, uint32_t addr,
                       uint32_t *data, size_t wsize, uint32_t bank_ignore_mask,
                       bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data)
{
    return cmicd_default_tbl_delete(unit, adext, addr, data, wsize,
                                    bank_ignore_mask, resp_info, resp_data);
}

int
bcmbd_cmicd_tbl_lookup(int unit, uint32_t adext, uint32_t addr,
                       uint32_t *data, size_t wsize, uint32_t bank_ignore_mask,
                       bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data)
{
    return cmicd_default_tbl_lookup(unit, adext, addr, data, wsize,
                                    bank_ignore_mask, resp_info, resp_data);
}
