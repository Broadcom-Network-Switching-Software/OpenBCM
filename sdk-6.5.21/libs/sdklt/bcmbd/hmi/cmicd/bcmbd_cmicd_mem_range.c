/*! \file bcmbd_cmicd_mem_range.c
 *
 * Low-level SOC memory access via SBUS DMA.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmbd/bcmbd_cmicd.h>
#include <bcmbd/bcmbd_cmicd_mem.h>

#include "bcmbd_cmicd_schan.h"
#include "bcmbd_cmicd_sbusdma.h"

/* Log source for this component */
#define BSL_LOG_MODULE          BSL_LS_BCMBD_SOCMEM

static int
bcmbd_cmicd_mem_range_op(int unit, uint32_t adext, uint32_t addr,
                         size_t wsize, uint32_t shift, uint32_t count,
                         uint64_t buf_paddr, uint32_t flags)
{
    bool rd_op = (flags & BCMBD_ROF_READ);
    int opcode, acctype, dstblk, datalen;
    schan_msg_t schan_msg;
    bcmbd_sbusdma_data_t data;
    bcmbd_sbusdma_work_t work;
    int rv;

    /* Initialize sbusdma work */
    sal_memset(&data, 0, sizeof(data));
    sal_memset(&work, 0, sizeof(work));

    /* Structure operation code */
    opcode = rd_op ? READ_MEMORY_CMD_MSG : WRITE_MEMORY_CMD_MSG;
    acctype = BCMBD_CMICD_ADEXT2ACCTYPE(adext);
    dstblk = BCMBD_CMICD_ADEXT2BLOCK(adext);
    datalen = wsize * sizeof(uint32_t);
    SCHAN_MSG_CLEAR(&schan_msg);
    SCMH_OPCODE_SET(schan_msg.header, opcode);
    SCMH_ACCTYPE_SET(schan_msg.header, acctype);
    SCMH_DSTBLK_SET(schan_msg.header, dstblk);
    SCMH_DATALEN_SET(schan_msg.header, datalen);

    /* Prepare sbusdma work data */
    data.start_addr = addr;
    data.data_width = wsize;
    data.addr_gap = shift;
    data.op_code = schan_msg.dwords[0];
    data.op_count = count;
    data.buf_paddr = buf_paddr;
    work.data = &data;
    work.flags = rd_op ? SBUSDMA_WF_READ_CMD : SBUSDMA_WF_WRITE_CMD;
    if (flags) {
        if (flags & BCMBD_ROF_INTR) {
            work.flags |= SBUSDMA_WF_INT_MODE;
        }
        if (flags & BCMBD_ROF_SGL_DATA) {
            work.flags |= SBUSDMA_WF_SGL_DATA;
        }
        if (flags & BCMBD_ROF_SGL_ADDR) {
            work.flags |= SBUSDMA_WF_SGL_ADDR;
        }
        if (flags & BCMBD_ROF_INV_ADDR) {
            work.flags |= SBUSDMA_WF_DEC_ADDR;
        }
    }

    /* Do sbusdma operation in register mode */
    rv = bcmbd_cmicd_sbusdma_reg_op(unit, &work);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Memory range %s error: "
                              "addr=0x%04"PRIx32"%08"PRIx32"\n"),
                   rd_op ? "read" : "write", adext, addr));
    }

    return rv;
}

int
bcmbd_cmicd_mem_range_read(int unit, uint32_t adext, uint32_t addr,
                           size_t wsize, uint32_t shift, uint32_t count,
                           uint64_t buf_paddr, uint32_t flags)
{
    return bcmbd_cmicd_mem_range_op(unit, adext, addr, wsize, shift, count,
                                    buf_paddr, flags | BCMBD_ROF_READ);
}

int
bcmbd_cmicd_mem_range_write(int unit, uint32_t adext, uint32_t addr,
                            size_t wsize, uint32_t shift, uint32_t count,
                            uint64_t buf_paddr, uint32_t flags)
{
    return bcmbd_cmicd_mem_range_op(unit, adext, addr, wsize, shift, count,
                                    buf_paddr, flags &= ~BCMBD_ROF_READ);
}
