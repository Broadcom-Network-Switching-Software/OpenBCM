/*! \file bcmbd_cmicx_mem_range.c
 *
 * Low-level SOC memory access via SBUS DMA.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_cmicx_mem.h>

#include "bcmbd_cmicx_schan.h"
#include "bcmbd_cmicx_sbusdma.h"

/* Log source for this component */
#define BSL_LOG_MODULE          BSL_LS_BCMBD_SOCMEM

/* Size of SBUSDMA command FIFO (64 bits wide) */
#define CMDFIFO_SIZE            32

/* Align size in 32-bit words to size in 64-bit words */
#define DWORD_ALIGN64(_sz)      (((_sz) + 1) / 2)
int
bcmbd_cmicx_mem_range_op(int unit, uint32_t adext, uint32_t addr, size_t wsize,
                         uint32_t shift, uint32_t count, uint64_t buf_paddr,
                         uint32_t flags, uint32_t mor_clks)
{
    int opcode, acctype, dstblk, datalen, cmdfifo_loc;
    schan_msg_t schan_msg;
    bcmbd_sbusdma_data_t data;
    bcmbd_sbusdma_work_t work;
    char *op_str;
    int rv;

    /* Check for pipeline bypass */
    if (bcmbd_addr_bypass(unit, adext, addr)) {
        return SHR_E_NONE;
    }

    /* Initialize SBUSDMA work */
    sal_memset(&data, 0, sizeof(data));
    sal_memset(&work, 0, sizeof(work));

    /* Initialize data that differ between read and write */
    opcode = WRITE_MEMORY_CMD_MSG;
    work.flags = SBUSDMA_WF_WRITE_CMD;
    op_str = "write";
    if (flags & BCMBD_ROF_READ) {
        opcode = READ_MEMORY_CMD_MSG;
        work.flags = SBUSDMA_WF_READ_CMD;
        op_str = "read";
    }

    /* Structure operation code */
    acctype = BCMBD_CMICX_ADEXT2ACCTYPE(adext);
    dstblk = BCMBD_CMICX_ADEXT2BLOCK(adext);
    datalen = wsize * sizeof(uint32_t);
    SCHAN_MSG_CLEAR(&schan_msg);
    SCMH_OPCODE_SET(schan_msg.header, opcode);
    SCMH_ACCTYPE_SET(schan_msg.header, acctype);
    SCMH_DSTBLK_SET(schan_msg.header, dstblk);
    SCMH_DATALEN_SET(schan_msg.header, datalen);

    /* Prepare SBUSDMA work data */
    data.start_addr = addr;
    data.data_width = wsize;
    data.addr_gap = shift;
    data.pend_clocks = mor_clks;
    data.op_code = schan_msg.dwords[0];
    data.op_count = count;
    data.buf_paddr = buf_paddr;
    work.data = &data;

    /*
     * Calculate the number of command FIFO locations required to hold
     * a single command, given that the command FIFO is 64 bits
     * wide. Command length is the entry size plus one opcode beat and
     * one address beat.
     */
    cmdfifo_loc = DWORD_ALIGN64(wsize + 1 + 1);

    /*
     * Set burst size to the maximum number of table entries that will
     * fit into the command FIFO.
     */
    data.burst_cmds = CMDFIFO_SIZE / cmdfifo_loc;

    /* Adjust operation mode based on flags */
    if (flags & ~BCMBD_ROF_READ) {
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

    /* Do SBUSDMA operation in register mode */
    rv = bcmbd_cmicx_sbusdma_reg_op(unit, &work);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Memory range %s error: "
                              "addr=0x%04"PRIx32"%08"PRIx32"\n"),
                   op_str, adext, addr));
    }

    return rv;
}

int
bcmbd_cmicx_mem_range_read(int unit, uint32_t adext, uint32_t addr,
                           size_t wsize, uint32_t shift, uint32_t count,
                           uint64_t buf_paddr, uint32_t flags)
{
    return bcmbd_cmicx_mem_range_op(unit, adext, addr, wsize, shift, count,
                                    buf_paddr, flags | BCMBD_ROF_READ, 0);
}

int
bcmbd_cmicx_mem_range_write(int unit, uint32_t adext, uint32_t addr,
                            size_t wsize, uint32_t shift, uint32_t count,
                            uint64_t buf_paddr, uint32_t flags)
{
    return bcmbd_cmicx_mem_range_op(unit, adext, addr, wsize, shift, count,
                                    buf_paddr, flags &= ~BCMBD_ROF_READ, 0);
}
