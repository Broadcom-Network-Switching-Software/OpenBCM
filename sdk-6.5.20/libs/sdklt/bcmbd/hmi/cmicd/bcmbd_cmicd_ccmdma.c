/*! \file bcmbd_cmicd_ccmdma.c
 *
 * CMICd CCMDMA routines
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
#include <sal/sal_sleep.h>
#include <sal/sal_spinlock.h>
#include <sal/sal_time.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/bcmbd_cmicd_acc.h>
#include <bcmbd/bcmbd_cmicd_intr.h>
#include <bcmbd/bcmbd_ccmdma.h>
#include <bcmbd/bcmbd_internal.h>
#include "bcmbd_cmicd_ccmdma.h"

#define BSL_LOG_MODULE  BSL_LS_BCMBD_CCMDMA

static cmicd_ccmdma_dev_t ccmdma_dev[CCMDMA_DEV_NUM_MAX];

static uint32_t cmicd_ccmdma_intr[] = {
    CMICD_IRQ_CCMDMA_DONE
};

/*!
 * Allocate a CCMDMA channel
 */
inline static int
cmicd_ccmdma_chan_get(cmicd_ccmdma_dev_t *dev, int *cmc, int *chan)
{
    int ch;

    while (1) {
        sal_spinlock_lock(dev->lock);
        if (!dev->bm_cmc) {
            sal_spinlock_unlock(dev->lock);
            return SHR_E_UNAVAIL;
        }
        for (ch = 0; ch < CMICD_CCMDMA_CHAN_MAX; ch++) {
            if (1 << ch & dev->bm_chan) {
                *cmc = ch / CMICD_CCMDMA_CMC_CHAN;
                *chan = ch % CMICD_CCMDMA_CMC_CHAN;
                dev->bm_chan &= ~(1 << ch);
                sal_spinlock_unlock(dev->lock);
                return SHR_E_NONE;
            }
        }
        dev->status |= CCMDMA_DS_CHAN_BUSY;
        sal_spinlock_unlock(dev->lock);
        sal_sem_take(dev->sem, SAL_SEM_FOREVER);
    }
}

/*!
 * Release a CCMDMA channel
 */
inline static int
cmicd_ccmdma_chan_put(cmicd_ccmdma_dev_t *dev, int cmc, int chan)
{
    int ch;

    if ((cmc < 0 || cmc >= CMICD_CCMDMA_CMC_MAX) ||
        (chan < 0 || chan >= CMICD_CCMDMA_CHAN_MAX)) {
        return SHR_E_PARAM;
    }

    sal_spinlock_lock(dev->lock);
    ch = chan + cmc * CMICD_CCMDMA_CMC_CHAN;
    dev->bm_chan |= 1 << ch;
    if (dev->status & CCMDMA_DS_CHAN_BUSY) {
        dev->status &= ~CCMDMA_DS_CHAN_BUSY;
        sal_spinlock_unlock(dev->lock);
        sal_sem_give(dev->sem);
        return SHR_E_NONE;
    }
    sal_spinlock_unlock(dev->lock);

    return SHR_E_NONE;
}

/*!
 * Report CCMDMA errors
 */
static void
cmicd_ccmdma_error_report(cmicd_ccmdma_dev_t *dev, int cmc,
                          CMIC_CMC_CCM_DMA_STATr_t *stat)
{
    int unit = dev->unit;
    CMIC_CMC_CCM_DMA_CUR_HOST0_ADDRr_t cha0;
    CMIC_CMC_CCM_DMA_CUR_HOST1_ADDRr_t cha1;

    if (CMIC_CMC_CCM_DMA_STATr_ERRORf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Error happened on CCMDMA.\n")));
    }
    if (CMIC_CMC_CCM_DMA_STATr_ECC_2BIT_CHECK_FAILf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Received sbus ack with ECC_2BIT_CHECK_FAIL set.\n")));
    }
    READ_CMIC_CMC_CCM_DMA_CUR_HOST0_ADDRr(unit, cmc, &cha0);
    READ_CMIC_CMC_CCM_DMA_CUR_HOST1_ADDRr(unit, cmc, &cha1);
    LOG_ERROR(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "Current src addr: 0x%x, dst addr: 0x%x.\n"),
               CMIC_CMC_CCM_DMA_CUR_HOST0_ADDRr_ADDRf_GET(cha0),
               CMIC_CMC_CCM_DMA_CUR_HOST1_ADDRr_ADDRf_GET(cha1)));
}

/*!
 * Reset CCMDMA channel
 */
static void
cmicd_ccmdma_chan_reset(cmicd_ccmdma_dev_t *dev, int cmc, int ch)
{
    int unit = dev->unit;
    CMIC_CMC_CCM_DMA_CFGr_t cfg;
    CMIC_CMC_CCM_DMA_STATr_t stat;
    int retry = CMICD_CCMDMA_RETRY;

    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM)) {
        return;
    }

    /* Configure registers */
    CMIC_CMC_CCM_DMA_CFGr_CLR(cfg);
    CMIC_CMC_CCM_DMA_CFGr_ENf_SET(cfg, 1);
    CMIC_CMC_CCM_DMA_CFGr_ABORTf_SET(cfg, 1);
    WRITE_CMIC_CMC_CCM_DMA_CFGr(unit, cmc, cfg);

    /* Wait the reset complete */
    do {
        READ_CMIC_CMC_CCM_DMA_STATr(unit, cmc, &stat);
        if (CMIC_CMC_CCM_DMA_STATr_DONEf_GET(stat)) {
            break;
        }
        sal_usleep(CMICD_CCMDMA_WAIT);
    } while (retry--);
    if (retry <= 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Ccmdma abort failed on %d/%d\n"), cmc, ch));
    }

    CMIC_CMC_CCM_DMA_CFGr_CLR(cfg);
    WRITE_CMIC_CMC_CCM_DMA_CFGr(unit, cmc, cfg);
}

/*!
 * Wait for CCMDMA operation complete
 */
static int
cmicd_ccmdma_op_wait(cmicd_ccmdma_dev_t *dev, bcmbd_ccmdma_work_t *work,
                     int cmc, int ch)
{
    int unit = dev->unit;
    CMIC_CMC_CCM_DMA_CFGr_t cfg;
    CMIC_CMC_CCM_DMA_STATr_t stat;
    int retry = SECOND_USEC;
    int rv = SHR_E_NONE;

    if (work->flags & CCMDMA_WF_INT_MODE) {
        /* Wait interrupt raise */
        if (sal_sem_take(dev->ctrl->intr[ch + cmc * CMICD_CCMDMA_CMC_CHAN], SECOND_USEC)) {
            /* Operation timeout */
            rv = SHR_E_TIMEOUT;
        } else {
            READ_CMIC_CMC_CCM_DMA_STATr(unit, cmc, &stat);
            if (CMIC_CMC_CCM_DMA_STATr_DONEf_GET(stat)) {
                if (CMIC_CMC_CCM_DMA_STATr_ERRORf_GET(stat)) {
                    /* Report details if operation failed */
                    cmicd_ccmdma_error_report(dev, cmc, &stat);
                    rv = SHR_E_FAIL;
                }
            } else {
                /* Operation timeout */
                rv = SHR_E_TIMEOUT;
            }
            CMIC_CMC_CCM_DMA_CFGr_CLR(cfg);
            WRITE_CMIC_CMC_CCM_DMA_CFGr(unit, cmc, cfg);
        }
    } else {
        /* Poll transaction done indicator */
        while (retry--) {
            READ_CMIC_CMC_CCM_DMA_STATr(unit, cmc, &stat);
            if (CMIC_CMC_CCM_DMA_STATr_DONEf_GET(stat)) {
                if (CMIC_CMC_CCM_DMA_STATr_ERRORf_GET(stat)) {
                    /* Report details if operation failed */
                    cmicd_ccmdma_error_report(dev, cmc, &stat);
                    rv = SHR_E_FAIL;
                }
                break;
            }
            sal_usleep(1);
        }
        if (retry <= 0) {
            /* Operation timeout */
            rv = SHR_E_TIMEOUT;
        }
    }

    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_TIMEOUT) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Ccmdma timeout on %d/%d\n"), cmc, ch));

            /* Reset the channel if timeout happened */
            cmicd_ccmdma_chan_reset(dev, cmc, ch);
        }
    }

    return rv;
}

/*!
 * Operate CCMDMA
 */
static int
cmicd_ccmdma_op(cmicd_ccmdma_dev_t *dev, bcmbd_ccmdma_work_t *work)
{
    int unit = dev->unit;
    CMIC_CMC_CCM_DMA_CFGr_t cfg;
    CMIC_CMC_CCM_DMA_HOST0_MEM_START_ADDRr_t hsa0;
    CMIC_CMC_CCM_DMA_HOST1_MEM_START_ADDRr_t hsa1;
    CMIC_CMC_CCM_DMA_ENTRY_COUNTr_t cnt;
    int cmc, ch;
    int rv;

    /* Allocate a channel */
    rv = cmicd_ccmdma_chan_get(dev, &cmc, &ch);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Configure registers */
    CMIC_CMC_CCM_DMA_CFGr_CLR(cfg);
    if (dev->flags & CCMDMA_CF_BIGENDIAN && !(work->flags & CCMDMA_WF_SRC_INNER)) {
        CMIC_CMC_CCM_DMA_CFGr_PROCESSOR0_ENDIANESSf_SET(cfg, 1);
    }
    if (dev->flags & CCMDMA_CF_BIGENDIAN && work->flags & CCMDMA_WF_DST_OUTER) {
        CMIC_CMC_CCM_DMA_CFGr_PROCESSOR1_ENDIANESSf_SET(cfg, 1);
    }
    WRITE_CMIC_CMC_CCM_DMA_CFGr(unit, cmc, cfg);

    CMIC_CMC_CCM_DMA_HOST0_MEM_START_ADDRr_SET(hsa0, work->src_paddr);
    WRITE_CMIC_CMC_CCM_DMA_HOST0_MEM_START_ADDRr(unit, cmc, hsa0);
    CMIC_CMC_CCM_DMA_HOST1_MEM_START_ADDRr_SET(hsa1, work->dst_paddr);
    WRITE_CMIC_CMC_CCM_DMA_HOST1_MEM_START_ADDRr(unit, cmc, hsa1);
    CMIC_CMC_CCM_DMA_ENTRY_COUNTr_SET(cnt, work->xfer_count);
    WRITE_CMIC_CMC_CCM_DMA_ENTRY_COUNTr(unit, cmc, cnt);

    /* Enable interrupt if in interrupt mode */
    if (work->flags & CCMDMA_WF_INT_MODE) {
        bcmbd_cmicd_intr_enable(unit, cmc, cmicd_ccmdma_intr[ch]);
    }

    /* Start operation */
    CMIC_CMC_CCM_DMA_CFGr_ENf_SET(cfg, 1);
    WRITE_CMIC_CMC_CCM_DMA_CFGr(unit, cmc, cfg);

    /* Wait operation complete */
    rv = cmicd_ccmdma_op_wait(dev, work, cmc, ch);

    /* Release the channel */
    cmicd_ccmdma_chan_put(dev, cmc, ch);

    return rv;
}

/*!
 * Interrupt service routine
 */
static void
cmicd_ccmdma_work_isr(int unit, uint32_t chan)
{
    cmicd_ccmdma_dev_t *dev = &ccmdma_dev[unit];
    ccmdma_ctrl_t *ctrl = dev->ctrl;
    int cmc, ch;

    /* Disable the interrupt */
    cmc = chan / CMICD_CCMDMA_CMC_CHAN;
    ch = chan % CMICD_CCMDMA_CMC_CHAN;
    bcmbd_cmicd_intr_disable_deferred(unit, cmc, cmicd_ccmdma_intr[ch]);

    sal_sem_give(ctrl->intr[chan]);
}

/*!
 * Execute work
 */
static int
bcmbd_cmicd_ccmdma_work_execute(ccmdma_ctrl_t *ctrl, bcmbd_ccmdma_work_t *work)
{
    cmicd_ccmdma_dev_t *dev = &ccmdma_dev[ctrl->unit];

    if (!dev->inited) {
        return SHR_E_UNAVAIL;
    }

    return cmicd_ccmdma_op(dev, work);
}

static const ccmdma_ops_t cmicd_ccmdma_ops = {
    .work_execute       = bcmbd_cmicd_ccmdma_work_execute
};

/*!
 * Transfer work
 */
int
bcmbd_cmicd_ccmdma_xfer(int unit, bcmbd_ccmdma_work_t *work)
{
    cmicd_ccmdma_dev_t *dev = &ccmdma_dev[unit];

    if (!dev->inited) {
        return SHR_E_UNAVAIL;
    }

    return cmicd_ccmdma_op(dev, work);
}

/*!
 * Initialize CCMDMA device
 */
int
bcmbd_cmicd_ccmdma_init(int unit)
{
    ccmdma_ctrl_t *ctrl = bcmbd_ccmdma_ctrl_get(unit);
    cmicd_ccmdma_dev_t *dev = &ccmdma_dev[unit];
    int cmc, ch;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_ccmdma_attach(unit));

    if (!ctrl || !ctrl->active) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    sal_memset(dev, 0, sizeof(*dev));
    dev->unit = unit;
    dev->ctrl = ctrl;
    dev->bm_cmc = 0x1 & CMICD_CCMDMA_CMC_MASK; /* Only use CMC0 for now */

    if (bcmdrd_dev_byte_swap_non_packet_dma_get(unit)) {
        dev->flags |= CCMDMA_CF_BIGENDIAN;
    }

    /* Resolve all the available channels */
    for (cmc = 0; cmc < CMICD_CCMDMA_CMC_MAX; cmc++) {
        if (1 << cmc & dev->bm_cmc) {
            dev->bm_chan |= CMICD_CCMDMA_CHAN_MASK << (cmc * CMICD_CCMDMA_CMC_CHAN);
            for (ch = 0; ch < CMICD_CCMDMA_CMC_CHAN; ch++) {
                /* Connect respective interrupt */
                bcmbd_cmicd_intr_func_set(unit, cmc, cmicd_ccmdma_intr[ch],
                                          (bcmbd_intr_f)cmicd_ccmdma_work_isr,
                                          ch + cmc * CMICD_CCMDMA_CMC_CHAN);
            }
        }
    }

    dev->sem = sal_sem_create("bcmbdCcmDmaDevSem", SAL_SEM_BINARY, 0);
    if (!dev->sem) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    dev->lock = sal_spinlock_create("bcmbdCcmDmaDevLock");
    if (!dev->lock) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    dev->inited = 1;

    ctrl->bm_grp = dev->bm_cmc;
    ctrl->bm_chan = dev->bm_chan;
    ctrl->ops = (ccmdma_ops_t *)&cmicd_ccmdma_ops;

exit:
    if (SHR_FUNC_ERR()) {
        if (dev->lock) {
            sal_spinlock_destroy(dev->lock);
            dev->lock = NULL;
        }

        if (dev->sem) {
            sal_sem_destroy(dev->sem);
            dev->sem = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * Clean up CCMDMA device
 */
int
bcmbd_cmicd_ccmdma_cleanup(int unit)
{
    ccmdma_ctrl_t *ctrl = bcmbd_ccmdma_ctrl_get(unit);
    cmicd_ccmdma_dev_t *dev = &ccmdma_dev[unit];
    int cmc, ch;
    int retry = CMICD_CCMDMA_RETRY * 10;

    SHR_FUNC_ENTER(unit);

    if (!ctrl) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (!dev->inited) {
        SHR_EXIT();
    }

    dev->inited = 0;

    ctrl->ops = NULL;

    /* Drain all channels */
    sal_spinlock_lock(dev->lock);
    dev->bm_cmc = 0;
    while (dev->bm_chan != ctrl->bm_chan && retry--) {
        sal_spinlock_unlock(dev->lock);
        sal_usleep(CMICD_CCMDMA_WAIT);
        sal_spinlock_lock(dev->lock);
    }
    sal_spinlock_unlock(dev->lock);
    if (retry <= 0) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Cannot clean up all channels.\n")));
    }

    /* Disable all interrupts */
    for (cmc = 0; cmc < CMICD_CCMDMA_CMC_MAX; cmc++) {
        if (1 << cmc & ctrl->bm_grp) {
            for (ch = 0; ch < CMICD_CCMDMA_CMC_CHAN; ch++) {
                bcmbd_cmicd_intr_disable_deferred(unit, cmc, cmicd_ccmdma_intr[ch]);
            }
        }
    }

    if (dev->lock) {
        sal_spinlock_destroy(dev->lock);
        dev->lock = NULL;
    }

    if (dev->sem) {
        sal_sem_destroy(dev->sem);
        dev->sem = NULL;
    }

    SHR_IF_ERR_EXIT
        (bcmbd_ccmdma_detach(unit));

exit:
    SHR_FUNC_EXIT();
}
