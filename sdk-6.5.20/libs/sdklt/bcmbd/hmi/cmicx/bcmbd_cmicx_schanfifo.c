/*! \file bcmbd_cmicx_schanfifo.c
 *
 * CMICx SCHAN FIFO routines
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal_config.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_sem.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_pb.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_hal.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_cmicx_acc.h>
#include <bcmbd/bcmbd_cmicx_dump.h>
#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/bcmbd_schanfifo.h>
#include <bcmbd/bcmbd_internal.h>
#include "bcmbd_cmicx_ccmdma.h"
#include "bcmbd_cmicx_schanfifo.h"

/* BSL source for SCHAN FIFO */
#define BSL_LOG_MODULE  BSL_LS_BCMBD_SCHANFIFO

/* Interrupt number */
#define BCMBD_CMICX_SCHANFIFO_INTR      CMICX_IRQ_SCHAN_FIFO_CH0_DONE
#define CMICX_SCHANFIFO_INTR_NUM(chan)  (BCMBD_CMICX_SCHANFIFO_INTR + chan)

static cmicx_schanfifo_dev_t schanfifo_dev[SCHANFIFO_DEV_NUM_MAX];

/*!
 * Dump operations data.
 */
static void
cmicx_schanfifo_data_dump(cmicx_schanfifo_dev_t *dev, int ch, uint32_t ops,
                          uint32_t *data, size_t wsize)
{
    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        shr_pb_t *pb = shr_pb_create();
        bcmbd_cmicx_dump_data(pb, data, wsize);
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(dev->unit,
                              "S-chan FIFO chan-%d ops-%d data:%s\n"),
                   ch, ops, shr_pb_str(pb)));
        shr_pb_destroy(pb);
    }
}

/*!
 * Reset SCHAN FIFO channel.
 */
static void
cmicx_schanfifo_chan_reset(cmicx_schanfifo_dev_t *dev, int ch)
{
    CMIC_SCHANFIFO_CTRLr_t ctl;
    CMIC_SCHANFIFO_STATUSr_t stat;
    int retry = dev->polls;

    READ_CMIC_SCHANFIFO_CTRLr(dev->unit, ch, &ctl);
    CMIC_SCHANFIFO_CTRLr_STARTf_SET(ctl, 1);
    CMIC_SCHANFIFO_CTRLr_ABORTf_SET(ctl, 1);
    WRITE_CMIC_SCHANFIFO_CTRLr(dev->unit, ch, ctl);

    /* Wait for completion */
    while (retry--) {
        READ_CMIC_SCHANFIFO_STATUSr(dev->unit, ch, &stat);
        if (CMIC_SCHANFIFO_STATUSr_DONEf_GET(stat)) {
            break;
        }
    }
    if (retry < 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(dev->unit, "S-chan FIFO abort failed on chan%d\n"),
                   ch));
    }

    CMIC_SCHANFIFO_CTRLr_CLR(ctl);
    WRITE_CMIC_SCHANFIFO_CTRLr(dev->unit, ch, ctl);
}

/*!
 * Configure SCHAN FIFO registers.
 */
static void
cmicx_schanfifo_reg_config(cmicx_schanfifo_dev_t *dev, int ch)
{
    CMIC_SCHANFIFO_RESP_HOSTMEM_START_ADDR_UPPERr_t rau;
    CMIC_SCHANFIFO_RESP_HOSTMEM_START_ADDR_LOWERr_t ral;
    CMIC_SCHANFIFO_SUMMARY_HOSTMEM_START_ADDR_UPPERr_t sau;
    CMIC_SCHANFIFO_SUMMARY_HOSTMEM_START_ADDR_LOWERr_t sal;

    /* Set host memory address for response messages */
    CMIC_SCHANFIFO_RESP_HOSTMEM_START_ADDR_UPPERr_SET(rau, BCMBD_CMICX_PCIE_SLAVE_OFFSET |
                                                           dev->resp_paddr[ch] >> 32);
    WRITE_CMIC_SCHANFIFO_RESP_HOSTMEM_START_ADDR_UPPERr(dev->unit, ch, rau);
    CMIC_SCHANFIFO_RESP_HOSTMEM_START_ADDR_LOWERr_SET(ral, dev->resp_paddr[ch]);
    WRITE_CMIC_SCHANFIFO_RESP_HOSTMEM_START_ADDR_LOWERr(dev->unit, ch, ral);

    /* Set host memory address for summary messages */
    CMIC_SCHANFIFO_SUMMARY_HOSTMEM_START_ADDR_UPPERr_SET(sau, BCMBD_CMICX_PCIE_SLAVE_OFFSET |
                                                              dev->sum_paddr[ch] >> 32);
    WRITE_CMIC_SCHANFIFO_SUMMARY_HOSTMEM_START_ADDR_UPPERr(dev->unit, ch, sau);
    CMIC_SCHANFIFO_SUMMARY_HOSTMEM_START_ADDR_LOWERr_SET(sal, dev->sum_paddr[ch]);
    WRITE_CMIC_SCHANFIFO_SUMMARY_HOSTMEM_START_ADDR_LOWERr(dev->unit, ch, sal);

    /* Pre-configure control register */
    CMIC_SCHANFIFO_CTRLr_CLR(dev->ctl_reg[ch]);
    if (dev->endians & SCHANFIFO_EF_BIGENDIAN) {
        CMIC_SCHANFIFO_CTRLr_RESPONSE_ENDIANESSf_SET(dev->ctl_reg[ch], 1);
        CMIC_SCHANFIFO_CTRLr_SUMMARY_ENDIANESSf_SET(dev->ctl_reg[ch], 1);
    }
    CMIC_SCHANFIFO_CTRLr_AXI_IDf_SET(dev->ctl_reg[ch], CMICX_SCHANFIFO_AXI_ID);
    CMIC_SCHANFIFO_CTRLr_SUMMARY_UPDATE_INTERVALf_SET(dev->ctl_reg[ch], CMICX_SCHANFIFO_SUM_UI);
    if (!(dev->flags & SCHANFIFO_IF_IGNORE_SER_ABORT)) {
        CMIC_SCHANFIFO_CTRLr_ENABLE_RESP_MEM_ECCERR_BASED_ABORTf_SET(dev->ctl_reg[ch], 1);
    }
}

/*!
 * Write commands.
 */
static void
cmicx_schanfifo_cmds_write(cmicx_schanfifo_dev_t *dev, int ch, uint32_t *cmds_buff,
                           size_t cmds_wsize, uint64_t buff_paddr)
{
    uint32_t cmdmem_addr;
    uint32_t mi;
    bcmbd_ccmdma_work_t cw;
    int rv;

    /* DMA write */
    if (dev->flags & SCHANFIFO_IF_CCMDMA_WR && buff_paddr) {
        cw.src_paddr = buff_paddr;
        cw.dst_paddr = dev->cmds_base + 0x580 * ch;
        cw.xfer_count = cmds_wsize;
        cw.flags = 0;
        rv = bcmbd_cmicx_ccmdma_xfer(dev->unit, &cw);
        if (SHR_SUCCESS(rv)) {
            return;
        }
    }

    /* PIO write */
    cmdmem_addr = CMIC_SCHANFIFO_COMMAND_MEMORYr_OFFSET + (ch * 0x580);
    if (BCMBD_CMICX_USE_64BIT()) {
        for (mi = 0; mi < cmds_wsize; mi += 2) {
            uint64_t data64;
            BCMBD_CMICX_DATA64_ENCODE(data64, &cmds_buff[mi]);
            BCMBD_CMICX_WRITE64(dev->unit, cmdmem_addr + mi*4, data64);
        }
    } else {
        for (mi = 0; mi < cmds_wsize; mi++) {
            BCMBD_CMICX_WRITE(dev->unit, cmdmem_addr + mi*4, cmds_buff[mi]);
        }
    }
}

/*!
 * Set SCHAN FIFO start.
 */
static void
cmicx_schanfifo_start_set(cmicx_schanfifo_dev_t *dev, int ch, bool start)
{
    CMIC_SCHANFIFO_CTRLr_t ctl;

    if (start) {
        CMIC_SCHANFIFO_CTRLr_SET(ctl, dev->ctl_reg[ch].v[0]);
        CMIC_SCHANFIFO_CTRLr_STARTf_SET(ctl, 1);
    } else {
        CMIC_SCHANFIFO_CTRLr_CLR(ctl);
    }
    WRITE_CMIC_SCHANFIFO_CTRLr(dev->unit, ch, ctl);
}

/*!
 * Report SCHAN FIFO errors.
 */
static void
cmicx_schanfifo_error_report(cmicx_schanfifo_dev_t *dev, int ch,
                             CMIC_SCHANFIFO_STATUSr_t *stat)
{
    int unit = dev->unit;

    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_NACKf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "H/W received sbus nack.\n")));
    }
    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_MESSAGE_ERRf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "H/W received error message.\n")));
    }
    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_MESSAGE_ERR_CODEf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "H/W received error message code.\n")));
    }
    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_ACK_OPCODE_MISMATCHf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Received sbus ack has wrong opcode.\n")));
    }
    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_CMD_MEM_ECC_ERRf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "ECC error from command memory.\n")));
    }
    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_RESP_MEM_ECC_ERRf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "ECC error from response memory.\n")));
    }
    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_CMD_OPCODE_IS_INVALIDf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Invalid sbus command opcode.\n")));
    }
    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_CMD_DLEN_IS_INVALIDf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Invalid sbus command length.\n")));
    }
    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_SBUS_TIMEOUTf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Sbus ack not received within configured time.\n")));
    }
    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_ACK_DATA_BEAT_GT20f_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Data beats was greater than 20.\n")));
    }
    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_AXI_RESP_ERRf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "AXI response error.\n")));
    }
    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_AXI_SLAVE_ABORTf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "AXI slave abort.\n")));
    }
    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_CMD_MEM_RD_ACCESS_INVALIDf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Invalid access for command memory read.\n")));
    }
}

/*!
 * Check SCHAN FIFO status.
 */
static int
cmicx_schanfifo_status_check(cmicx_schanfifo_dev_t *dev, int ch)
{
    CMIC_SCHANFIFO_STATUSr_t stat;

    READ_CMIC_SCHANFIFO_STATUSr(dev->unit, ch, &stat);
    if (CMIC_SCHANFIFO_STATUSr_DONEf_GET(stat)) {
        if (CMIC_SCHANFIFO_STATUSr_ERRORf_GET(stat)) {
            cmicx_schanfifo_error_report(dev, ch, &stat);
            return SHR_E_FAIL;
        }
        return SHR_E_NONE;
    } else {
        return SHR_E_UNAVAIL;
    }
}

/*!
 * Check SCHAN FIFO summary.
 */
static int
cmicx_schanfifo_summary_check(cmicx_schanfifo_dev_t *dev, int ch, uint32_t cmd_id)
{
    uint16_t *sum = (uint16_t *)dev->sum_addr[ch];

    /* Not done yet */
    if (!(sum[cmd_id] & 0x8000)) {
        return SHR_E_UNAVAIL;
    }

    /* Done or with SER error */
    if (!(sum[cmd_id] & 0x6000) ||
        ((sum[cmd_id] & 0x6000) == 0x2000 &&
         (dev->flags & SCHANFIFO_IF_IGNORE_SER_ABORT))) {
        return SHR_E_NONE;
    }

    /* Abort with other errors */
    cmicx_schanfifo_chan_reset(dev, ch);

    return SHR_E_FAIL;
}

/*!
 * Scan SCHAN FIFO summary.
 */
static void
cmicx_schanfifo_summary_scan(cmicx_schanfifo_dev_t *dev, int ch,
                             uint32_t num_cmds, uint32_t *done_cmds)
{
    uint32_t cn;
    int rv;

    for (cn = 0; cn < num_cmds; cn++) {
        rv = cmicx_schanfifo_summary_check(dev, ch, cn);
        if (SHR_FAILURE(rv)) {
            break;
        }
    }

    *done_cmds = cn;
}

/*!
 * Interrupt service routine.
 */
static void
cmicx_schanfifo_isr(cmicx_schanfifo_dev_t *dev, int ch)
{
    /* Disable interrupt */
    bcmbd_cmicx_intr_disable_deferred(dev->unit, CMICX_SCHANFIFO_INTR_NUM(ch));

    sal_sem_give(dev->intr[ch]);
}

/*!
 * Get SCHAN FIFO information.
 */
static int
bcmbd_cmicx_schanfifo_info_get(schanfifo_ctrl_t *ctrl,
                               int *num_chans, size_t *cmd_mem_wsize)
{
    *num_chans = ctrl->channels;
    *cmd_mem_wsize = ctrl->cmds_wsize;

    return SHR_E_NONE;
}

/*!
 * Initialize SCHAN FIFO.
 */
static int
bcmbd_cmicx_schanfifo_dev_init(schanfifo_ctrl_t *ctrl,
                               uint32_t max_polls, uint32_t flags)
{
    cmicx_schanfifo_dev_t *dev = &schanfifo_dev[ctrl->unit];
    int ch;

    dev->polls = max_polls ? max_polls : CMICX_SCHANFIFO_POLLS_DFLT;
    dev->flags |= flags;

    for (ch = 0; ch < CMICX_SCHANFIFO_CHAN_MAX; ch++) {
        /* Pre-configure registers */
        cmicx_schanfifo_reg_config(dev, ch);
    }

    return SHR_E_NONE;
}

/*!
 * Send operations to SCHAN FIFO channel.
 */
static int
bcmbd_cmicx_schanfifo_ops_send(schanfifo_ctrl_t *ctrl, int chan, uint32_t num_ops,
                               uint32_t *req_buff, size_t req_wsize,
                               uint64_t buff_paddr, uint32_t flags)
{
    cmicx_schanfifo_dev_t *dev = &schanfifo_dev[ctrl->unit];

    /* Debug output */
    cmicx_schanfifo_data_dump(dev, chan, num_ops, req_buff, req_wsize);

    /* Write commands */
    cmicx_schanfifo_cmds_write(dev, chan, req_buff, req_wsize, buff_paddr);

    /* Pre-configure control register */
    CMIC_SCHANFIFO_CTRLr_NUMB_OF_COMMANDSf_SET(dev->ctl_reg[chan], num_ops);
    if (flags & SCHANFIFO_OF_DIS_RESP) {
        CMIC_SCHANFIFO_CTRLr_RESPONSE_WRITE_DISABLEf_SET(dev->ctl_reg[chan], 1);
    } else {
        CMIC_SCHANFIFO_CTRLr_RESPONSE_WRITE_DISABLEf_SET(dev->ctl_reg[chan], 0);
    }

    /* Enable interrupt if in interrupt mode */
    if (!(dev->flags & SCHANFIFO_IF_POLL_WAIT)) {
        bcmbd_cmicx_intr_enable(dev->unit, CMICX_SCHANFIFO_INTR_NUM(chan));
    }

    if (flags & SCHANFIFO_OF_SET_START) {
        /* Start operation */
        cmicx_schanfifo_start_set(dev, chan, TRUE);
    }

    return SHR_E_NONE;
}

/*!
 * Set start for SCHAN FIFO channel.
 */
static int
bcmbd_cmicx_schanfifo_start_set(schanfifo_ctrl_t *ctrl,
                                int chan, bool start)
{
    cmicx_schanfifo_dev_t *dev = &schanfifo_dev[ctrl->unit];

    cmicx_schanfifo_start_set(dev, chan, start);

    return SHR_E_NONE;
}

/*!
 * Get SCHAN FIFO channel status.
 */
static int
bcmbd_cmicx_schanfifo_status_get(schanfifo_ctrl_t *ctrl,
                                 int chan, uint32_t num_ops, uint32_t flags,
                                 uint32_t *done_ops, uint32_t **resp_buff)
{
    cmicx_schanfifo_dev_t *dev = &schanfifo_dev[ctrl->unit];
    int retry = dev->polls;
    int rv;

    if (resp_buff) {
        *resp_buff = dev->resp_addr[chan];
    }

    /* One time check */
    if (!(flags & SCHANFIFO_OF_WAIT_COMPLETE)) {
        rv = cmicx_schanfifo_summary_check(dev, chan, num_ops - 1);
        if (SHR_SUCCESS(rv)) {
            /* Debug output */
            if (resp_buff) {
                cmicx_schanfifo_data_dump(dev, chan, num_ops, dev->resp_addr[chan],
                                          num_ops * CMICX_SCHANFIFO_RESP_SIZE / 4);
            }
            *done_ops = num_ops;
        }
        if (flags & SCHANFIFO_OF_CLEAR_START) {
            cmicx_schanfifo_start_set(dev, chan, FALSE);
        }
        return rv;
    }

    if (dev->flags & SCHANFIFO_IF_POLL_WAIT) {
        /* Wait for completion */
        while (retry--) {
            rv = cmicx_schanfifo_status_check(dev, chan);
            if (rv != SHR_E_UNAVAIL) {
                break;
            }
        }
        if (retry < 0) {
            rv = SHR_E_TIMEOUT;
        }
    } else {
        /* Wait for interrupt */
        if (sal_sem_take(dev->intr[chan], CMICX_SCHANFIFO_WAIT_TIME)) {
            rv = SHR_E_TIMEOUT;
        } else {
            rv = cmicx_schanfifo_status_check(dev, chan);
            if (rv == SHR_E_UNAVAIL) {
                rv = SHR_E_TIMEOUT;
            }
            cmicx_schanfifo_start_set(dev, chan, FALSE);
        }
    }

    if (SHR_SUCCESS(rv)) {
        /* Debug output */
        if (resp_buff) {
            cmicx_schanfifo_data_dump(dev, chan, num_ops, dev->resp_addr[chan],
                                      num_ops * CMICX_SCHANFIFO_RESP_SIZE / 4);
        }
        *done_ops = num_ops;
        if (flags & SCHANFIFO_OF_CLEAR_START) {
            cmicx_schanfifo_start_set(dev, chan, FALSE);
        }
    } else {
        /* Scan summaries to get done ones */
        cmicx_schanfifo_summary_scan(dev, chan, num_ops, done_ops);
        if (rv == SHR_E_TIMEOUT) {
            cmicx_schanfifo_chan_reset(dev, chan);
        }
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(dev->unit, "S-chan FIFO operation failed @chan%d.\n"),
                   chan));
    }

    return rv;
}

static const schanfifo_ops_t cmicx_schanfifo_ops = {
    .info_get           = bcmbd_cmicx_schanfifo_info_get,
    .dev_init           = bcmbd_cmicx_schanfifo_dev_init,
    .ops_send           = bcmbd_cmicx_schanfifo_ops_send,
    .start_set          = bcmbd_cmicx_schanfifo_start_set,
    .status_get         = bcmbd_cmicx_schanfifo_status_get
};

/*!
 * Initialize SCHAN FIFO device.
 */
int
bcmbd_cmicx_schanfifo_init(int unit)
{
    schanfifo_ctrl_t *ctrl = bcmbd_schanfifo_ctrl_get(unit);
    cmicx_schanfifo_dev_t *dev = &schanfifo_dev[unit];
    int ch;

    SHR_FUNC_ENTER(unit);

    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmbd_schanfifo_attach(unit));

    if (!ctrl || !ctrl->active) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    sal_memset(dev, 0, sizeof(*dev));
    dev->unit = unit;
    dev->ctrl = ctrl;
    dev->cmds_base = bcmbd_cmicx_base_addr(unit) + CMIC_SCHANFIFO_COMMAND_MEMORYr_OFFSET;
    dev->polls = CMICX_SCHANFIFO_POLLS_DFLT;

    if (bcmdrd_dev_byte_swap_non_packet_dma_get(unit)) {
        dev->endians |= SCHANFIFO_EF_BIGENDIAN;
    }

    for (ch = 0; ch < CMICX_SCHANFIFO_CHAN_MAX; ch++) {
        /* Allocate response buffer */
        dev->resp_size[ch] = CMICX_SCHANFIFO_CMDS_MAX * CMICX_SCHANFIFO_RESP_SIZE;
        dev->resp_addr[ch] = bcmdrd_hal_dma_alloc(unit, dev->resp_size[ch],
                                                  "bcmbdSchanFifoResponse",
                                                  &dev->resp_paddr[ch]);
        if (!dev->resp_addr[ch]) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }

        /* Allocate summary buffer */
        dev->sum_size[ch] = CMICX_SCHANFIFO_CMDS_MAX * CMICX_SCHANFIFO_SUM_SIZE;
        dev->sum_addr[ch] = bcmdrd_hal_dma_alloc(unit, dev->sum_size[ch],
                                                 "bcmbdSchanFifoSummary",
                                                 &dev->sum_paddr[ch]);
        if (!dev->sum_addr[ch]) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }

        /* Create semaphore for interrupt mode */
        dev->intr[ch] = sal_sem_create("bcmbdSchanFifoIntrSem", SAL_SEM_BINARY, 0);
        if (!dev->intr[ch]) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }

        /* Connect respective interrupt */
        bcmbd_cmicx_intr_func_set(unit, CMICX_SCHANFIFO_INTR_NUM(ch),
                                  (bcmbd_intr_f)cmicx_schanfifo_isr, ch);
    }

    ctrl->channels = CMICX_SCHANFIFO_CHAN_MAX;
    ctrl->cmds_wsize = CMICX_SCHANFIFO_CMDS_WSIZE;
    ctrl->ops = (schanfifo_ops_t *)&cmicx_schanfifo_ops;

    dev->inited = 1;

exit:
    if (SHR_FUNC_ERR()) {
        for (ch = 0; ch < CMICX_SCHANFIFO_CHAN_MAX; ch++) {
            if (dev->resp_addr[ch]) {
                bcmdrd_hal_dma_free(unit, dev->resp_size[ch], dev->resp_addr[ch],
                                    dev->resp_paddr[ch]);
                dev->resp_addr[ch] = NULL;
            }
            if (dev->sum_addr[ch]) {
                bcmdrd_hal_dma_free(unit, dev->sum_size[ch], dev->sum_addr[ch],
                                    dev->sum_paddr[ch]);
                dev->sum_addr[ch] = NULL;
            }
            if (dev->intr[ch]) {
                sal_sem_destroy(dev->intr[ch]);
                dev->intr[ch] = NULL;
            }
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * Clean up SCHAN FIFO device.
 */
int
bcmbd_cmicx_schanfifo_cleanup(int unit)
{
    schanfifo_ctrl_t *ctrl = bcmbd_schanfifo_ctrl_get(unit);
    cmicx_schanfifo_dev_t *dev = &schanfifo_dev[unit];
    int ch;

    SHR_FUNC_ENTER(unit);

    if (!ctrl) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (!dev->inited) {
        SHR_EXIT();
    }

    dev->inited = 0;

    ctrl->ops = NULL;

    /* Free all resources */
    for (ch = 0; ch < CMICX_SCHANFIFO_CHAN_MAX; ch++) {
        if (!(dev->flags & SCHANFIFO_IF_POLL_WAIT)) {
            bcmbd_cmicx_intr_disable_deferred(unit, CMICX_SCHANFIFO_INTR_NUM(ch));
        }
        if (dev->resp_addr[ch]) {
            bcmdrd_hal_dma_free(unit, dev->resp_size[ch], dev->resp_addr[ch],
                                dev->resp_paddr[ch]);
            dev->resp_addr[ch] = NULL;
        }
        if (dev->sum_addr[ch]) {
            bcmdrd_hal_dma_free(unit, dev->sum_size[ch], dev->sum_addr[ch],
                                dev->sum_paddr[ch]);
            dev->sum_addr[ch] = NULL;
        }
        if (dev->intr[ch]) {
            sal_sem_destroy(dev->intr[ch]);
            dev->intr[ch] = NULL;
        }
    }

    SHR_IF_ERR_EXIT
        (bcmbd_schanfifo_detach(unit));

exit:
    SHR_FUNC_EXIT();
}
