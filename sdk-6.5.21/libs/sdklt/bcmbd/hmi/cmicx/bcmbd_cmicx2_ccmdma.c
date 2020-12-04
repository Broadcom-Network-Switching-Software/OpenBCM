/*! \file bcmbd_cmicx2_ccmdma.c
 *
 * CMICx V2 CCMDMA routines
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal_config.h>
#include <sal/sal_types.h>
#include <sal/sal_sem.h>
#include <sal/sal_sleep.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_cmicx2_acc.h>
#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/bcmbd_ccmdma.h>
#include <bcmbd/bcmbd_internal.h>
#include "bcmbd_cmicx_ccmdma.h"

#define BSL_LOG_MODULE  BSL_LS_BCMBD_CCMDMA

/*!
 * Report CCMDMA errors
 */
static void
cmicx_ccmdma_error_report(cmicx_ccmdma_dev_t *dev, CMIC_CMC_CCMDMA_STATr_t *stat,
                          int cmc, int ch)
{
    int unit = dev->unit;
    CMIC_CMC_CCMDMA_CUR_HOST0_ADDR_LOr_t cha0;
    CMIC_CMC_CCMDMA_CUR_HOST1_ADDR_LOr_t cha1;

    if (CMIC_CMC_CCMDMA_STATr_ERRORf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Error happened on CCMDMA.\n")));
    }
    if (CMIC_CMC_CCMDMA_STATr_ECC_2BIT_CHECK_FAILf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Received sbus ack with ECC_2BIT_CHECK_FAIL set.\n")));
    }
    READ_CMIC_CMC_CCMDMA_CUR_HOST0_ADDR_LOr(unit, cmc, ch, &cha0);
    READ_CMIC_CMC_CCMDMA_CUR_HOST1_ADDR_LOr(unit, cmc, ch, &cha1);
    LOG_ERROR(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "Current src addr: 0x%x, dst addr: 0x%x.\n"),
               CMIC_CMC_CCMDMA_CUR_HOST0_ADDR_LOr_ADDRf_GET(cha0),
               CMIC_CMC_CCMDMA_CUR_HOST1_ADDR_LOr_ADDRf_GET(cha1)));
}

/*!
 * Reset CCMDMA channel
 */
static void
cmicx_ccmdma_chan_reset(cmicx_ccmdma_dev_t *dev, int cmc, int ch)
{
    int unit = dev->unit;
    CMIC_CMC_CCMDMA_CFGr_t cfg;
    CMIC_CMC_CCMDMA_STATr_t stat;
    int retry = CMICX_CCMDMA_RETRY;

    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM)) {
        return;
    }

    /* Configure registers */
    CMIC_CMC_CCMDMA_CFGr_CLR(cfg);
    CMIC_CMC_CCMDMA_CFGr_ENf_SET(cfg, 1);
    CMIC_CMC_CCMDMA_CFGr_ABORTf_SET(cfg, 1);
    WRITE_CMIC_CMC_CCMDMA_CFGr(unit, cmc, ch, cfg);

    /* Wait the reset complete */
    do {
        READ_CMIC_CMC_CCMDMA_STATr(unit, cmc, ch, &stat);
        if (CMIC_CMC_CCMDMA_STATr_DONEf_GET(stat)) {
            break;
        }
        sal_usleep(CMICX_CCMDMA_WAIT);
    } while (retry--);
    if (retry <= 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Ccmdma abort failed on %d/%d\n"), cmc, ch));
    }

    CMIC_CMC_CCMDMA_CFGr_CLR(cfg);
    WRITE_CMIC_CMC_CCMDMA_CFGr(unit, cmc, ch, cfg);
}

/*!
 * Wait for CCMDMA operation complete
 */
static int
cmicx_ccmdma_op_wait(cmicx_ccmdma_dev_t *dev, bcmbd_ccmdma_work_t *work,
                     int cmc, int ch)
{
    int unit = dev->unit;
    CMIC_CMC_CCMDMA_CFGr_t cfg;
    CMIC_CMC_CCMDMA_STATr_t stat;
    int retry = SECOND_USEC;
    int rv = SHR_E_NONE;

    if (work->flags & CCMDMA_WF_INT_MODE) {
        /* Wait interrupt raise */
        if (sal_sem_take(dev->ctrl->intr[ch + cmc * CMICX_CCMDMA_CMC_CHAN], SECOND_USEC)) {
            /* Operation timeout */
            rv = SHR_E_TIMEOUT;
        } else {
            READ_CMIC_CMC_CCMDMA_STATr(unit, cmc, ch, &stat);
            if (CMIC_CMC_CCMDMA_STATr_DONEf_GET(stat)) {
                if (CMIC_CMC_CCMDMA_STATr_ERRORf_GET(stat)) {
                    /* Report details if operation failed */
                    cmicx_ccmdma_error_report(dev, &stat, cmc, ch);
                    rv = SHR_E_FAIL;
                }
            } else {
                /* Operation timeout */
                rv = SHR_E_TIMEOUT;
            }
            CMIC_CMC_CCMDMA_CFGr_CLR(cfg);
            WRITE_CMIC_CMC_CCMDMA_CFGr(unit, cmc, ch, cfg);
        }
    } else {
        /* Poll transaction done indicator */
        while (retry--) {
            READ_CMIC_CMC_CCMDMA_STATr(unit, cmc, ch, &stat);
            if (CMIC_CMC_CCMDMA_STATr_DONEf_GET(stat)) {
                if (CMIC_CMC_CCMDMA_STATr_ERRORf_GET(stat)) {
                    /* Report details if operation failed */
                    cmicx_ccmdma_error_report(dev, &stat, cmc, ch);
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
            cmicx_ccmdma_chan_reset(dev, cmc, ch);
        }
    }

    return rv;
}

/*!
 * Operate CCMDMA
 */
static int
cmicx_ccmdma_op(cmicx_ccmdma_dev_t *dev, bcmbd_ccmdma_work_t *work)
{
    int unit = dev->unit;
    CMIC_CMC_CCMDMA_CFGr_t cfg;
    CMIC_CMC_CCMDMA_HOST0_MEM_START_ADDR_HIr_t hsah0;
    CMIC_CMC_CCMDMA_HOST0_MEM_START_ADDR_LOr_t hsal0;
    CMIC_CMC_CCMDMA_HOST1_MEM_START_ADDR_HIr_t hsah1;
    CMIC_CMC_CCMDMA_HOST1_MEM_START_ADDR_LOr_t hsal1;
    CMIC_CMC_CCMDMA_ENTRY_COUNTr_t cnt;
    uint32_t hsah;
    int cmc, ch;
    int rv;

    /* Allocate a channel */
    rv = cmicx_ccmdma_chan_get(dev, &cmc, &ch);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Configure registers */
    CMIC_CMC_CCMDMA_CFGr_CLR(cfg);
    if (dev->flags & CCMDMA_CF_BIGENDIAN && !(work->flags & CCMDMA_WF_SRC_INNER)) {
        CMIC_CMC_CCMDMA_CFGr_PROCESSOR0_ENDIANESSf_SET(cfg, 1);
    }
    if (dev->flags & CCMDMA_CF_BIGENDIAN && work->flags & CCMDMA_WF_DST_OUTER) {
        CMIC_CMC_CCMDMA_CFGr_PROCESSOR1_ENDIANESSf_SET(cfg, 1);
    }
    WRITE_CMIC_CMC_CCMDMA_CFGr(unit, cmc, ch, cfg);

    hsah = work->src_paddr >> 32;
    if (!(work->flags & CCMDMA_WF_SRC_INNER)) {
        hsah |= BCMBD_CMICX_PCIE_SLAVE_OFFSET;
    }
    CMIC_CMC_CCMDMA_HOST0_MEM_START_ADDR_HIr_SET(hsah0, hsah);
    WRITE_CMIC_CMC_CCMDMA_HOST0_MEM_START_ADDR_HIr(unit, cmc, ch, hsah0);
    CMIC_CMC_CCMDMA_HOST0_MEM_START_ADDR_LOr_SET(hsal0, work->src_paddr);
    WRITE_CMIC_CMC_CCMDMA_HOST0_MEM_START_ADDR_LOr(unit, cmc, ch, hsal0);
    hsah = work->dst_paddr >> 32;
    if (work->flags & CCMDMA_WF_DST_OUTER) {
        hsah |= BCMBD_CMICX_PCIE_SLAVE_OFFSET;
    }
    CMIC_CMC_CCMDMA_HOST1_MEM_START_ADDR_HIr_SET(hsah1, hsah);
    WRITE_CMIC_CMC_CCMDMA_HOST1_MEM_START_ADDR_HIr(unit, cmc, ch, hsah1);
    CMIC_CMC_CCMDMA_HOST1_MEM_START_ADDR_LOr_SET(hsal1, work->dst_paddr);
    WRITE_CMIC_CMC_CCMDMA_HOST1_MEM_START_ADDR_LOr(unit, cmc, ch, hsal1);
    CMIC_CMC_CCMDMA_ENTRY_COUNTr_SET(cnt, work->xfer_count);
    WRITE_CMIC_CMC_CCMDMA_ENTRY_COUNTr(unit, cmc, ch, cnt);

    /* Enable interrupt if in interrupt mode */
    if (work->flags & CCMDMA_WF_INT_MODE) {
        bcmbd_cmicx_intr_enable(unit, dev->int_src[cmc][ch]);
    }

    /* Start operation */
    CMIC_CMC_CCMDMA_CFGr_ENf_SET(cfg, 1);
    WRITE_CMIC_CMC_CCMDMA_CFGr(unit, cmc, ch, cfg);

    /* Wait operation complete */
    rv = cmicx_ccmdma_op_wait(dev, work, cmc, ch);

    /* Release the channel */
    cmicx_ccmdma_chan_put(dev, cmc, ch);

    return rv;
}

/*!
 * Initialize CMICx V2 CCMDMA device
 */
int
bcmbd_cmicx2_ccmdma_init(int unit)
{
    ccmdma_ctrl_t *ctrl = bcmbd_ccmdma_ctrl_get(unit);
    cmicx_ccmdma_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    if (!ctrl) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_ccmdma_init(unit));

    dev = (cmicx_ccmdma_dev_t *)ctrl->dev;
    dev->ccmdma_op = cmicx_ccmdma_op;

exit:
    SHR_FUNC_EXIT();
}
