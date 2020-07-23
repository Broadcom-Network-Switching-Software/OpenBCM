/*! \file bcmbd_cmicx_fifodma.c
 *
 * CMICx FIFODMA routines
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
#include <sal/sal_sleep.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_cmicx2_acc.h>
#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/bcmbd_fifodma.h>
#include <bcmbd/bcmbd_internal.h>
#include "bcmbd_cmicx_fifodma.h"

#define BSL_LOG_MODULE  BSL_LS_BCMBD_FIFODMA

#define BCMBD_CMICX_FIFODMA_INTR        CMICX_IRQ_FIFO_CH0_DMA_INTR
#define CMICX_FIFODMA_INTR_NUM(chan)    (BCMBD_CMICX_FIFODMA_INTR + chan)

static cmicx_fifodma_dev_t fifodma_dev[FIFODMA_DEV_NUM_MAX];

/*!
 * Check FIFODMA status
 */
static int
cmicx_fifodma_stat_check(cmicx_fifodma_dev_t *dev, CMIC_FIFO_RD_DMA_STATr_t *sta)
{
    int unit = dev->unit;
    int rv = SHR_E_NONE;

    if (CMIC_FIFO_RD_DMA_STATr_ERRORf_GET(*sta)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Error happened on FIFODMA.\n")));
        rv = SHR_E_IO;
    }
    if (CMIC_FIFO_RD_DMA_STATr_HOSTMEMWR_ERRORf_GET(*sta)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Error while copying data to host memory.\n")));
        rv = SHR_E_IO;
    }
    if (CMIC_FIFO_RD_DMA_STATr_SBUSACK_WRONG_BEATCOUNTf_GET(*sta)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Received sbus ack data size is not the same as "
                              "in rep_words fields.\n")));
        rv = SHR_E_IO;
    }
    if (CMIC_FIFO_RD_DMA_STATr_SBUSACK_WRONG_OPCODEf_GET(*sta)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Received sbus ack has wrong opcode.\n")));
        rv = SHR_E_IO;
    }
    if (CMIC_FIFO_RD_DMA_STATr_SBUSACK_NACKf_GET(*sta)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "H/W received sbus nack with error bit set.\n")));
        rv = SHR_E_IO;
    }
    if (CMIC_FIFO_RD_DMA_STATr_SBUSACK_ERRORf_GET(*sta)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "H/W received sbus ack with error bit set.\n")));
        rv = SHR_E_IO;
    }
    if (CMIC_FIFO_RD_DMA_STATr_SBUSACK_TIMEOUTf_GET(*sta)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "sbus ack not received within configured time.\n")));
        rv = SHR_E_IO;
    }
    if (CMIC_FIFO_RD_DMA_STATr_ECC_2BIT_CHECK_FAILf_GET(*sta)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Received sbus ack with ECC_2BIT_CHECK_FAIL set.\n")));
        rv = SHR_E_IO;
    }

    return rv;
}

/*!
 * Start FIFODMA device
 */
static int
cmicx_fifodma_start(cmicx_fifodma_dev_t *dev, int chan)
{
    int unit = dev->unit;
    bcmbd_fifodma_work_t *work = &dev->ctrl->work[chan];
    CMIC_FIFO_RD_DMA_CFGr_t cfg;
    CMIC_FIFO_RD_DMA_CFG_1r_t cfg1;
    CMIC_FIFO_RD_DMA_OPCODEr_t opc;
    CMIC_FIFO_RD_DMA_SBUS_START_ADDRESSr_t ssa;
    CMIC_FIFO_RD_DMA_HOSTMEM_START_ADDRESS_HIr_t sah;
    CMIC_FIFO_RD_DMA_HOSTMEM_START_ADDRESS_LOr_t sal;
    CMIC_FIFO_RD_DMA_HOSTMEM_THRESHOLDr_t hth;
    uint32_t sel;

    /* Get the encoding for host memory entries */
    for (sel = 0; sel <= CMICX_FIFODMA_SEL_MAX; sel++) {
        if (work->data->num_entries == (uint32_t)(64 << sel)) {
            break;
        }
    }
    if (sel > CMICX_FIFODMA_SEL_MAX) {
        return SHR_E_PARAM;
    }

    /* Configure FIFODMA registers */
    CMIC_FIFO_RD_DMA_CFGr_CLR(cfg);
    CMIC_FIFO_RD_DMA_CFGr_NACK_FATALf_SET(cfg, 1);
    CMIC_FIFO_RD_DMA_CFGr_BEAT_COUNTf_SET(cfg, work->data->data_width);
    CMIC_FIFO_RD_DMA_CFGr_HOST_NUM_ENTRIES_SELf_SET(cfg, sel);
    if (dev->flags & FIFODMA_CF_BIGENDIAN) {
        CMIC_FIFO_RD_DMA_CFGr_ENDIANESSf_SET(cfg, 1);
    }
    if (work->data->timeout_cnt) {
        CMIC_FIFO_RD_DMA_CFGr_TIMEOUT_COUNTf_SET(cfg, work->data->timeout_cnt);
    } else {
        CMIC_FIFO_RD_DMA_CFGr_TIMEOUT_COUNTf_SET(cfg, 0x3fff);
    }
    CMIC_FIFO_RD_DMA_CFGr_MULTIPLE_SBUS_CMD_SPACINGf_SET(cfg, work->data->cmd_spacing);
    WRITE_CMIC_FIFO_RD_DMA_CFGr(unit, chan, cfg);
    READ_CMIC_FIFO_RD_DMA_CFG_1r(unit, chan, &cfg1);
    if (work->flags & FIFODMA_WF_NEW_FIFOIF) {
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_CMICX2)) {
            CMIC_FIFO_RD_DMA_CFG_1r_COUNT_SELf_SET(cfg1, work->data->fifo_if);
        } else {
            return SHR_E_UNAVAIL;
        }
    }
    if (work->flags & FIFODMA_WF_IGN_EARLYACK) {
        if (!work->data->cmd_spacing) {
            return SHR_E_PARAM;
        }
        CMIC_FIFO_RD_DMA_CFG_1r_IGNORE_SBUS_EARLYACKf_SET(cfg1, 1);
    } else {
        CMIC_FIFO_RD_DMA_CFG_1r_IGNORE_SBUS_EARLYACKf_SET(cfg1, 0);
    }
    CMIC_FIFO_RD_DMA_CFG_1r_MULTIPLE_SBUS_CMD_SPACING_MSB_BITSf_SET(cfg1, work->data->cmd_spacing >> 5);
    CMIC_FIFO_RD_DMA_CFG_1r_TIMEOUT_COUNT_MSB_BITSf_SET(cfg1, work->data->timeout_cnt >> 14);
    WRITE_CMIC_FIFO_RD_DMA_CFG_1r(unit, chan, cfg1);
    CMIC_FIFO_RD_DMA_OPCODEr_SET(opc, work->data->op_code);
    WRITE_CMIC_FIFO_RD_DMA_OPCODEr(unit, chan, opc);
    CMIC_FIFO_RD_DMA_SBUS_START_ADDRESSr_SET(ssa, work->data->start_addr);
    WRITE_CMIC_FIFO_RD_DMA_SBUS_START_ADDRESSr(unit, chan, ssa);
    CMIC_FIFO_RD_DMA_HOSTMEM_START_ADDRESS_HIr_SET(sah, BCMBD_CMICX_PCIE_SLAVE_OFFSET |
                                                        dev->ctrl->fifo[chan].buf_paddr >> 32);
    WRITE_CMIC_FIFO_RD_DMA_HOSTMEM_START_ADDRESS_HIr(unit, chan, sah);
    CMIC_FIFO_RD_DMA_HOSTMEM_START_ADDRESS_LOr_SET(sal, dev->ctrl->fifo[chan].buf_paddr);
    WRITE_CMIC_FIFO_RD_DMA_HOSTMEM_START_ADDRESS_LOr(unit, chan, sal);
    CMIC_FIFO_RD_DMA_HOSTMEM_THRESHOLDr_SET(hth, work->data->thresh_entries);
    WRITE_CMIC_FIFO_RD_DMA_HOSTMEM_THRESHOLDr(unit, chan, hth);

    /* Enable interrupt if in interrupt mode */
    if (work->flags & FIFODMA_WF_INT_MODE) {
        dev->ctrl->fifo[chan].intr_enabled = 1;
        bcmbd_cmicx_intr_enable(unit, CMICX_FIFODMA_INTR_NUM(chan));
    }

    /* Start FIFODMA */
    CMIC_FIFO_RD_DMA_CFGr_ENABLEf_SET(cfg, 1);
    WRITE_CMIC_FIFO_RD_DMA_CFGr(unit, chan, cfg);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Fifodma started on chan%d\n"), chan));

    return SHR_E_NONE;
}

/*!
 * Stop FIFODMA device
 */
static int
cmicx_fifodma_stop(cmicx_fifodma_dev_t *dev, int chan)
{
    int unit = dev->unit;
    CMIC_FIFO_RD_DMA_CFGr_t cfg;
    CMIC_FIFO_RD_DMA_STATr_t sta;
    int retry = CMICX_FIFODMA_RETRY;

    /* Check FIFODMA enabled */
    READ_CMIC_FIFO_RD_DMA_CFGr(unit, chan, &cfg);
    if (!CMIC_FIFO_RD_DMA_CFGr_ENABLEf_GET(cfg)) {
        return SHR_E_NONE;
    }

    /* Abort FIFODMA channel */
    CMIC_FIFO_RD_DMA_CFGr_ABORTf_SET(cfg, 1);
    WRITE_CMIC_FIFO_RD_DMA_CFGr(unit, chan, cfg);

    /* Wait abort complete */
    do {
        READ_CMIC_FIFO_RD_DMA_CFGr(unit, chan, &cfg);
        if (!CMIC_FIFO_RD_DMA_CFGr_ENABLEf_GET(cfg)) {
            break;
        }
        READ_CMIC_FIFO_RD_DMA_STATr(unit, chan, &sta);
        if (CMIC_FIFO_RD_DMA_STATr_ABORTEDf_GET(sta)) {
            break;
        }
        sal_usleep(CMICX_FIFODMA_WAIT);
    } while (retry--);
    if (retry <= 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Fifodma abort failed on chan%d\n"), chan));
    }

    /* Disable interrupt if in interrupt mode */
    if (dev->ctrl->fifo[chan].intr_enabled) {
        bcmbd_cmicx_intr_disable_deferred(unit, CMICX_FIFODMA_INTR_NUM(chan));
        dev->ctrl->fifo[chan].intr_enabled = 0;
    }

    CMIC_FIFO_RD_DMA_CFGr_CLR(cfg);
    WRITE_CMIC_FIFO_RD_DMA_CFGr(unit, chan, cfg);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Fifodma stopped on chan%d\n"), chan));

    return SHR_E_NONE;
}

/*!
 * Pop FIFODMA device
 */
static int
cmicx_fifodma_pop(cmicx_fifodma_dev_t *dev, int chan, uint32_t *ptr, uint32_t *num)
{
    int unit = dev->unit;
    fifodma_chan_t *fifo = &dev->ctrl->fifo[chan];
    CMIC_FIFO_RD_DMA_STATr_t sta;
    CMIC_FIFO_RD_DMA_NUM_OF_ENTRIES_VALID_IN_HOSTMEMr_t nov;
    CMIC_FIFO_RD_DMA_NUM_OF_ENTRIES_READ_FRM_HOSTMEMr_t nor;
    uint32_t head, tail, size, entries;
    char *bc, *ba;
    int rv;

    /* Check status */
    READ_CMIC_FIFO_RD_DMA_STATr(unit, chan, &sta);
    rv = cmicx_fifodma_stat_check(dev, &sta);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Get ready entries */
    READ_CMIC_FIFO_RD_DMA_NUM_OF_ENTRIES_VALID_IN_HOSTMEMr(unit, chan, &nov);
    entries = CMIC_FIFO_RD_DMA_NUM_OF_ENTRIES_VALID_IN_HOSTMEMr_GET(nov);
    if (!entries) {
        return SHR_E_EMPTY;
    }

    /* Copy to buffer cache */
    tail = fifo->ptr;
    fifo->ptr = (fifo->ptr + entries) % fifo->num_entries;
    head = fifo->ptr;
    size = tail * fifo->data_width * sizeof(uint32_t);
    bc = (char *)fifo->buf_cache + size;
    ba = (char *)fifo->buf_addr + size;
    if (head > tail) {
        size = entries * fifo->data_width * sizeof(uint32_t);
        sal_memcpy(bc, ba, size);
    } else {
        size = (fifo->num_entries - tail) * fifo->data_width * sizeof(uint32_t);
        sal_memcpy(bc, ba, size);
        if (head > 0) {
            bc = (char *)fifo->buf_cache;
            ba = (char *)fifo->buf_addr;
            size = head * fifo->data_width * sizeof(uint32_t);
            sal_memcpy(bc, ba, size);
        }
    }

    /* Acknowledge ready entries */
    CMIC_FIFO_RD_DMA_NUM_OF_ENTRIES_READ_FRM_HOSTMEMr_CLR(nor);
    CMIC_FIFO_RD_DMA_NUM_OF_ENTRIES_READ_FRM_HOSTMEMr_ENTRYCOUNTf_SET(nor, entries);
    CMIC_FIFO_RD_DMA_NUM_OF_ENTRIES_READ_FRM_HOSTMEMr_WR_NUMBER_OF_ENTRIESf_SET(nor, 1);
    WRITE_CMIC_FIFO_RD_DMA_NUM_OF_ENTRIES_READ_FRM_HOSTMEMr(unit, chan, nor);

    /* Enable interrupt if in interrupt mode */
    if (fifo->intr_enabled) {
        bcmbd_cmicx_intr_enable(unit, CMICX_FIFODMA_INTR_NUM(chan));
    }

    *ptr = tail;
    *num = entries;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Fifodma popped %d entries on chan%d\n"),
                 entries, chan));

    return SHR_E_NONE;
}

/*!
 * Interrupt service routine.
 */
static void
cmicx_fifodma_work_isr(int unit, int chan)
{
    cmicx_fifodma_dev_t *dev = &fifodma_dev[unit];
    fifodma_ctrl_t *ctrl = dev->ctrl;

    /* Disable interrupt */
    bcmbd_cmicx_intr_disable_deferred(unit, CMICX_FIFODMA_INTR_NUM(chan));

    ctrl->intr_notify(unit, chan);
}

/*!
 * Handle CMICx FIFODMA device.
 */
static void
bcmbd_cmicx_fifodma_handle(fifodma_ctrl_t *ctrl, int chan)
{
    int unit = ctrl->unit;
    CMIC_FIFO_RD_DMA_CFGr_t cfg;
    CMIC_FIFO_RD_DMA_STATr_t sta;
    CMIC_FIFO_RD_DMA_STAT_CLRr_t stc;

    /* Clear normal interrupt status */
    CMIC_FIFO_RD_DMA_STAT_CLRr_CLR(stc);
    READ_CMIC_FIFO_RD_DMA_STATr(unit, chan, &sta);
    if (CMIC_FIFO_RD_DMA_STATr_HOSTMEM_OVERFLOWf_GET(sta)) {
        CMIC_FIFO_RD_DMA_STAT_CLRr_HOSTMEM_OVERFLOWf_SET(stc, 1);
    }
    if (CMIC_FIFO_RD_DMA_STATr_HOSTMEM_TIMEOUTf_GET(sta)) {
        CMIC_FIFO_RD_DMA_STAT_CLRr_HOSTMEM_TIMEOUTf_SET(stc, 1);
    }
    if (CMIC_FIFO_RD_DMA_STATr_ECC_2BIT_CHECK_FAILf_GET(sta)) {
        CMIC_FIFO_RD_DMA_STAT_CLRr_ECC_2BIT_CHECK_FAILf_SET(stc, 1);
    }
    if (CMIC_FIFO_RD_DMA_STAT_CLRr_GET(stc)) {
        WRITE_CMIC_FIFO_RD_DMA_STAT_CLRr(unit, chan, stc);
    }

    /* Clear ABORT status and disable interrupt */
    if (CMIC_FIFO_RD_DMA_STATr_ABORTEDf_GET(sta)) {
        READ_CMIC_FIFO_RD_DMA_CFGr(unit, chan, &cfg);
        CMIC_FIFO_RD_DMA_CFGr_ENABLEf_SET(cfg, 0);
        WRITE_CMIC_FIFO_RD_DMA_CFGr(unit, chan, cfg);
        bcmbd_cmicx_intr_disable_deferred(unit, CMICX_FIFODMA_INTR_NUM(chan));
        ctrl->fifo[chan].intr_enabled = 0;
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Fifodma interrupt handle on chan%d\n"), chan));
}

/*!
 * Start CMICx FIFODMA device.
 */
static int
bcmbd_cmicx_fifodma_start(fifodma_ctrl_t *ctrl, int chan)
{
    cmicx_fifodma_dev_t *dev = &fifodma_dev[ctrl->unit];

    if (!dev->inited) {
        return SHR_E_UNAVAIL;
    }

    return cmicx_fifodma_start(dev, chan);
}

/*!
 * Stop CMICx FIFODMA device.
 */
static int
bcmbd_cmicx_fifodma_stop(fifodma_ctrl_t *ctrl, int chan)
{
    cmicx_fifodma_dev_t *dev = &fifodma_dev[ctrl->unit];

    if (!dev->inited) {
        return SHR_E_UNAVAIL;
    }

    return cmicx_fifodma_stop(dev, chan);
}

/*!
 * Pop CMICx FIFODMA device.
 */
static int
bcmbd_cmicx_fifodma_pop(fifodma_ctrl_t *ctrl, int chan, uint32_t *ptr, uint32_t *num)
{
    cmicx_fifodma_dev_t *dev = &fifodma_dev[ctrl->unit];

    if (!dev->inited) {
        return SHR_E_UNAVAIL;
    }

    return cmicx_fifodma_pop(dev, chan, ptr, num);
}

static const fifodma_ops_t cmicx_fifodma_ops = {
    .fifodma_handle     = bcmbd_cmicx_fifodma_handle,
    .fifodma_start      = bcmbd_cmicx_fifodma_start,
    .fifodma_stop       = bcmbd_cmicx_fifodma_stop,
    .fifodma_pop        = bcmbd_cmicx_fifodma_pop
};

/*!
 * Initialize FIFODMA device
 */
int
bcmbd_cmicx_fifodma_init(int unit)
{
    fifodma_ctrl_t *ctrl = bcmbd_fifodma_ctrl_get(unit);
    cmicx_fifodma_dev_t *dev = &fifodma_dev[unit];
    int cmc, ch;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_fifodma_attach(unit));

    if (!ctrl || !ctrl->active) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    sal_memset(dev, 0, sizeof(*dev));
    dev->unit = unit;
    dev->ctrl = ctrl;
    dev->bm_cmc = 0x1 & CMICX_FIFODMA_CMC_MASK;

    if (bcmdrd_dev_byte_swap_non_packet_dma_get(unit)) {
        dev->flags |= FIFODMA_CF_BIGENDIAN;
    }

    /* Resolve all the available channels */
    for (cmc = 0; cmc < CMICX_FIFODMA_CMC_MAX; cmc++) {
        if (1 << cmc & dev->bm_cmc) {
            dev->bm_chan |= CMICX_FIFODMA_CHAN_MASK << (cmc * CMICX_FIFODMA_CMC_CHAN);
            for (ch = 0; ch < CMICX_FIFODMA_CMC_CHAN; ch++) {
                /* Connect respective interrupt */
                bcmbd_cmicx_intr_func_set(unit, CMICX_FIFODMA_INTR_NUM(ch),
                                          (bcmbd_intr_f)cmicx_fifodma_work_isr,
                                          ch + cmc * CMICX_FIFODMA_CMC_CHAN);
            }
        }
    }

    ctrl->ops = (fifodma_ops_t *)&cmicx_fifodma_ops;

    dev->inited = 1;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Clean up FIFODMA device
 */
int
bcmbd_cmicx_fifodma_cleanup(int unit)
{
    fifodma_ctrl_t *ctrl = bcmbd_fifodma_ctrl_get(unit);
    cmicx_fifodma_dev_t *dev = &fifodma_dev[unit];
    int cmc, ch;

    SHR_FUNC_ENTER(unit);

    if (!ctrl) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (!dev->inited) {
        SHR_EXIT();
    }

    dev->inited = 0;

    ctrl->ops = NULL;

    /* Disable all interrupts and channels */
    for (cmc = 0; cmc < CMICX_FIFODMA_CMC_MAX; cmc++) {
        if (1 << cmc & dev->bm_cmc) {
            for (ch = 0; ch < CMICX_FIFODMA_CMC_CHAN; ch++) {
                if (ctrl->fifo[ch].intr_enabled) {
                    bcmbd_cmicx_intr_disable_deferred(unit, CMICX_FIFODMA_INTR_NUM(ch));
                    ctrl->fifo[ch].intr_enabled = 0;
                }
                if (ctrl->fifo[ch].started) {
                    cmicx_fifodma_stop(dev, ch + cmc * CMICX_FIFODMA_CMC_CHAN);
                }
            }
        }
    }

    SHR_IF_ERR_EXIT
        (bcmbd_fifodma_detach(unit));

exit:
    SHR_FUNC_EXIT();
}
