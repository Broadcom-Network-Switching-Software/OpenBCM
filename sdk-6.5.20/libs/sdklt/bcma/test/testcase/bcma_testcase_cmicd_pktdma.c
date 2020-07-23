/*! \file bcma_testcase_cmicd_pktdma.c
 *
 * Broadcom test cases for CMICD Packet DMA access
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>

#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <sal/sal_sem.h>
#include <sal/sal_sleep.h>
#include <sal/sal_libc.h>

#include <bcmbd/bcmbd_cmicd_intr.h>
#include <bcmbd/bcmbd_cmicd_acc.h>
#include <bcmbd/bcmbd.h>

#include <bcmdrd/bcmdrd_feature.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/testcase/bcma_testcase_cmicd_pktdma.h>
#include <bcma/test/util/bcma_testutil_port.h>
#include <bcma/test/util/bcma_testutil_packet.h>

#include "bcma_testcase_pktdma_internal.h"

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

/* Tx/Rx DCB must be aligned in word */
#define TX_DCB_WORD_SIZE            SHR_BYTES2WORDS(TX_DCB_SIZE)
#define RX_DCB_WORD_SIZE            SHR_BYTES2WORDS(RX_DCB_SIZE)

static uint32_t cmicd_pktdma_chain_intr[] = {
    CMICD_IRQ_CH0_CHAIN_DONE,
    CMICD_IRQ_CH1_CHAIN_DONE,
    CMICD_IRQ_CH2_CHAIN_DONE,
    CMICD_IRQ_CH3_CHAIN_DONE
};

static sal_sem_t *cmicd_pktdma_intr[BCMDRD_CONFIG_MAX_UNITS] = {0};

#define CMICD_PKTDMA_CMC_MAX        1
#define CMICD_PKTDMA_CMC_CHAN       4
#define CMICD_PKTDMA_CHAN_MAX       CMICD_PKTDMA_CMC_MAX * CMICD_PKTDMA_CMC_CHAN
#define CMICD_PKTDMA_MHDR_SIZE      4
#define CMICD_PKTDMA_RX_DIR         0
#define CMICD_PKTDMA_TX_DIR         1

#define CMICD_PKTDMA_TIMEOUT        1000000
#define CMICD_PKTDMA_POLL           10000

static void
clear_chan(int unit, int chan)
{
    CMIC_CMC_DMA_CTRLr_t cdc;
    CMIC_CMC_DMA_STAT_CLRr_t cdsc;
    int cmc = chan / CMICD_PKTDMA_CMC_CHAN;
    int idx = chan % CMICD_PKTDMA_CMC_CHAN;

    /* Clear descriptor complete */
    CMIC_CMC_DMA_STAT_CLRr_CLR(cdsc);
    CMIC_CMC_DMA_STAT_CLRr_DESCRD_CMPLT_CLRf_SET(cdsc, (1 << idx));
    WRITE_CMIC_CMC_DMA_STAT_CLRr(unit, cmc, cdsc);

    SAL_CONFIG_MEMORY_BARRIER;

    /* Disable DMA */
    READ_CMIC_CMC_DMA_CTRLr(unit, cmc, idx, &cdc);
    CMIC_CMC_DMA_CTRLr_DMA_ENf_SET(cdc, 0);
    WRITE_CMIC_CMC_DMA_CTRLr(unit, cmc, idx, cdc);

    SAL_CONFIG_MEMORY_BARRIER;
}

static void
cmicd_pktdma_chan_init(int unit, int chan, int dir)
{
    CMIC_CMC_DMA_CTRLr_t cdc;
    CMIC_CMC_COS_CTRL_RX_0r_t cos_ctrl0;
    CMIC_CMC_COS_CTRL_RX_1r_t cos_ctrl1;
    int cmc = chan / CMICD_PKTDMA_CMC_CHAN;
    int idx = chan % CMICD_PKTDMA_CMC_CHAN;

    READ_CMIC_CMC_DMA_CTRLr(unit, cmc, idx, &cdc);
    CMIC_CMC_DMA_CTRLr_DIRECTIONf_SET(cdc, dir);
    WRITE_CMIC_CMC_DMA_CTRLr(unit, cmc, idx, cdc);

    /* Enable 48 CPU COS queues for rx chan */
    if (dir == CMICD_PKTDMA_RX_DIR) {
        CMIC_CMC_COS_CTRL_RX_0r_SET(cos_ctrl0, 0xffffffff);
        WRITE_CMIC_CMC_COS_CTRL_RX_0r(unit, cmc, idx, cos_ctrl0);
        CMIC_CMC_COS_CTRL_RX_1r_SET(cos_ctrl1, 0xffff);
        WRITE_CMIC_CMC_COS_CTRL_RX_1r(unit, cmc, idx, cos_ctrl1);
    }
}

static void
cmicd_pktdma_chan_start(int unit, int chan, dma_addr_t dcb)
{
    CMIC_CMC_DMA_CTRLr_t cdc;
    CMIC_CMC_DMA_DESCr_t cdd;
    int cmc = chan / CMICD_PKTDMA_CMC_CHAN;
    int idx = chan % CMICD_PKTDMA_CMC_CHAN;

    /* Write the DCB address to the DESC address for this channel */
    CMIC_CMC_DMA_DESCr_CLR(cdd);
    CMIC_CMC_DMA_DESCr_ADDRf_SET(cdd, dcb);
    WRITE_CMIC_CMC_DMA_DESCr(unit, cmc, idx, cdd);

    SAL_CONFIG_MEMORY_BARRIER;

    /* Kick it off */
    READ_CMIC_CMC_DMA_CTRLr(unit, cmc, idx, &cdc);
    CMIC_CMC_DMA_CTRLr_DMA_ENf_SET(cdc, 1);
    WRITE_CMIC_CMC_DMA_CTRLr(unit, cmc, idx, cdc);

    SAL_CONFIG_MEMORY_BARRIER;
}

static int
cmicd_pktdma_chan_poll(int unit, int chan, int polls)
{
    CMIC_CMC_DMA_STATr_t dma_stat;
    sal_sem_t *intr = cmicd_pktdma_intr[unit];
    uint32_t done;
    int px;
    int cmc = chan / CMICD_PKTDMA_CMC_CHAN;
    int idx = chan % CMICD_PKTDMA_CMC_CHAN;

    SHR_FUNC_ENTER(unit);

    CMIC_CMC_DMA_STATr_CLR(dma_stat);

    /* Interrupt mode */
    if (polls == 0) {
        /* Wait chan done interrupt */
        if (sal_sem_take(intr[chan], CMICD_PKTDMA_TIMEOUT)) {
            SHR_ERR_EXIT(SHR_E_TIMEOUT);
        } else {
            READ_CMIC_CMC_DMA_STATr(unit, cmc, &dma_stat);
            done = CMIC_CMC_DMA_STATr_CHAIN_DONEf_GET(dma_stat);
            if (done & (1 << idx)) {
                /* DMA complete. Clear the channel */
                clear_chan(unit, chan);

                SHR_EXIT();
            }
        }
    } else {
        for (px = 0; px < polls; px++) {
            READ_CMIC_CMC_DMA_STATr(unit, cmc, &dma_stat);
            done = CMIC_CMC_DMA_STATr_CHAIN_DONEf_GET(dma_stat);
            if (done & (1 << idx)) {
                /* DMA complete. Clear the channel */
                clear_chan(unit, chan);

                SHR_EXIT();
            }
            sal_usleep(200);
        }
    }
    cli_out("chan %"PRId32" poll timeout dma_stat 0x%"PRIx32"\n",
            chan, dma_stat._cmic_cmc_dma_stat);

    SHR_ERR_EXIT(SHR_E_TIMEOUT);

exit:
    SHR_FUNC_EXIT();
}

static int
cmicd_pktdma_chan_abort(int unit, int chan, int dir, int polls)
{
    CMIC_CMC_DMA_CTRLr_t cdc;
    CMIC_CMC_DMA_STATr_t dma_stat;
    CMIC_CMC_COS_CTRL_RX_0r_t cos_ctrl0;
    CMIC_CMC_COS_CTRL_RX_1r_t cos_ctrl1;
    uint32_t active;
    int px;
    int cmc = chan / CMICD_PKTDMA_CMC_CHAN;
    int idx = chan % CMICD_PKTDMA_CMC_CHAN;

    SHR_FUNC_ENTER(unit);

    /* Abort the channel */
    READ_CMIC_CMC_DMA_CTRLr(unit, cmc, idx, &cdc);
    CMIC_CMC_DMA_CTRLr_ABORT_DMAf_SET(cdc, 1);
    WRITE_CMIC_CMC_DMA_CTRLr(unit, cmc, idx, cdc);

    SAL_CONFIG_MEMORY_BARRIER;

    /* Poll for abort completion */
    for (px = 0; px < polls; px++) {
        READ_CMIC_CMC_DMA_STATr(unit, cmc, &dma_stat);
        active = CMIC_CMC_DMA_STATr_DMA_ACTIVEf_GET(dma_stat);
        if (!(active & (1 << idx))) {
            /* Clear abort */
            READ_CMIC_CMC_DMA_CTRLr(unit, cmc, idx, &cdc);
            CMIC_CMC_DMA_CTRLr_ABORT_DMAf_SET(cdc, 0);
            WRITE_CMIC_CMC_DMA_CTRLr(unit, cmc, idx, cdc);

            /* DMA complete. Clear the channel */
            clear_chan(unit, chan);

            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_TIMEOUT);

exit:
    /* Disable 48 CPU COS queues for rx chan */
    if (dir == CMICD_PKTDMA_RX_DIR) {
        CMIC_CMC_COS_CTRL_RX_0r_CLR(cos_ctrl0);
        WRITE_CMIC_CMC_COS_CTRL_RX_0r(unit, cmc, idx, cos_ctrl0);
        CMIC_CMC_COS_CTRL_RX_1r_CLR(cos_ctrl1);
        WRITE_CMIC_CMC_COS_CTRL_RX_1r(unit, cmc, idx, cos_ctrl1);
    }


    SHR_FUNC_EXIT();
}

static int
cmicd_pktdma_init(int unit)
{
    CMIC_CMC_DMA_CTRLr_t dma_ctrl;
    CMIC_RXBUF_EP_RLS_CREDr_t rel_credit;
    int ch, cmc, idx;

    SHR_FUNC_ENTER(unit);

    /* Set Packet DMA endian */
    for (ch = 0; ch < CMICD_PKTDMA_CMC_MAX * CMICD_PKTDMA_CMC_CHAN; ch++) {
        cmc = ch / CMICD_PKTDMA_CMC_CHAN;
        idx = ch % CMICD_PKTDMA_CMC_CHAN;
        READ_CMIC_CMC_DMA_CTRLr(unit, cmc, idx, &dma_ctrl);
        if (bcmdrd_dev_byte_swap_packet_dma_get(unit)) {
            CMIC_CMC_DMA_CTRLr_PKTDMA_ENDIANESSf_SET(dma_ctrl, 1);
        }
        if (bcmdrd_dev_byte_swap_non_packet_dma_get(unit)) {
            CMIC_CMC_DMA_CTRLr_DESC_ENDIANESSf_SET(dma_ctrl, 1);
        }
        WRITE_CMIC_CMC_DMA_CTRLr(unit, cmc, idx, dma_ctrl);
    }

    /* Release all credits to EPIPE */
    CMIC_RXBUF_EP_RLS_CREDr_CLR(rel_credit);
    CMIC_RXBUF_EP_RLS_CREDr_RELEASE_ALL_CREDITSf_SET(rel_credit, 1);
    WRITE_CMIC_RXBUF_EP_RLS_CREDr(unit, rel_credit);

    SHR_FUNC_EXIT();
}

static void
cmicd_pktdma_chain_isr(int unit, uint32_t chan)
{
    sal_sem_t *intr = cmicd_pktdma_intr[unit];
    int cmc, idx;

    /* Disable the interrupt */
    cmc = chan / CMICD_PKTDMA_CMC_CHAN;
    idx = chan % CMICD_PKTDMA_CMC_CHAN;
    bcmbd_cmicd_intr_disable_deferred(unit, cmc, cmicd_pktdma_chain_intr[idx]);

    sal_sem_give(intr[chan]);
}

static int
cmicd_pktdma_intr_attach(int unit)
{
    sal_sem_t *intr = NULL;
    int ch, cmc, idx;

    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(intr, sizeof(sal_sem_t) * CMICD_PKTDMA_CHAN_MAX,
              "bcmaTestCaseCmicdPKTDMAIntr");
    SHR_NULL_CHECK(intr, SHR_E_MEMORY);
    sal_memset(intr, 0, sizeof(sal_sem_t) * CMICD_PKTDMA_CHAN_MAX);
    cmicd_pktdma_intr[unit] = intr;

    for (ch = 0; ch < CMICD_PKTDMA_CHAN_MAX; ch++) {
        cmc = ch / CMICD_PKTDMA_CMC_CHAN;
        idx = ch % CMICD_PKTDMA_CMC_CHAN;

        intr[ch] = sal_sem_create("bcmaTestCaseCmicdPKTDMAIntr", SAL_SEM_BINARY, 0);
        SHR_NULL_CHECK(intr[ch], SHR_E_MEMORY);

        bcmbd_cmicd_intr_func_set(unit, cmc, cmicd_pktdma_chain_intr[idx],
                                  (bcmbd_intr_f)cmicd_pktdma_chain_isr,
                                  ch);
        bcmbd_cmicd_intr_enable(unit, cmc, cmicd_pktdma_chain_intr[idx]);
    }

exit:
    SHR_FUNC_EXIT();
}

static void
cmicd_pktdma_intr_detach(int unit)
{
    sal_sem_t *intr = cmicd_pktdma_intr[unit];
    int ch, cmc, idx;

    for (ch = 0; ch < CMICD_PKTDMA_CHAN_MAX; ch++) {
        cmc = ch / CMICD_PKTDMA_CMC_CHAN;
        idx = ch % CMICD_PKTDMA_CMC_CHAN;

        bcmbd_cmicd_intr_disable_deferred(unit, cmc, cmicd_pktdma_chain_intr[idx]);
        if (intr[ch]) {
            sal_sem_destroy(intr[ch]);
        }
    }

    SHR_FREE(intr);
}

static void
dump_words(char *prefix, uint32_t *wdata, int cnt, int offs, int incr)
{
    int idx;

    if (cnt == 0) {
        return;
    }

    cli_out("%s", prefix);
    for (idx = 0; idx < cnt; idx++, offs += incr) {
        cli_out(" %08"PRIx32, wdata[offs]);
    }
    cli_out("\n");
}

static void
cmicd_pktdma_dump_tx_dcbs(int unit, uint32_t *dcbs, int dcb_cnt,
                          int dcb_size, int mh_size)
{
    /* Dump DMA descriptor */
    dump_words("Tx DMA addr =", dcbs, dcb_cnt, 0, dcb_size);
    dump_words("Tx DMA ctrl =", dcbs, dcb_cnt, 1, dcb_size);
    dump_words("Tx DMA stat =", dcbs, dcb_cnt, dcb_size - 1, dcb_size);
}

static void
cmicd_pktdma_dump_rx_dcbs(int unit, uint32_t *dcbs, int dcb_cnt,
                          int dcb_size, int mh_size)
{
    int idx;

    /* Dump DMA descriptor */
    dump_words("Rx DMA addr =", dcbs, dcb_cnt, 0, dcb_size);
    dump_words("Rx DMA ctrl =", dcbs, dcb_cnt, 1, dcb_size);
    dump_words("Rx DMA stat =", dcbs, dcb_cnt, dcb_size - 1, dcb_size);

    /* Dump module header if supplied */
    for (idx = 0; idx < dcb_cnt; idx++) {
        dump_words("Rx DMA mhdr =", dcbs + dcb_size * idx, mh_size, 6, 1);
    }
}

static void
dump_packet(char *prefix, uint8_t *pkt_data, uint32_t pkt_len)
{
    uint32_t idx;

    cli_out("%s", prefix);

    for (idx = 0; idx < pkt_len; idx++) {
        if (idx % 16 == 0) {
            cli_out("\n");
        }
        cli_out("%02"PRIx32" ", *pkt_data++);
    }
    cli_out("\n");
}

static int
cmicd_pktdma_tx_test(int unit, void *bp, int tx_chan)
{
    pktdma_test_param_t *p = (pktdma_test_param_t *)bp;
    dma_addr_t bdcb = (dma_addr_t)p->txdcb_dma_addr;
    int poll = p->intr_mode ? 0 : CMICD_PKTDMA_POLL;

    SHR_FUNC_ENTER(unit);

    /* Init tx chan DMA */
    cmicd_pktdma_chan_init(unit, tx_chan, CMICD_PKTDMA_TX_DIR);

    /* Start tx chan DMA */
    cmicd_pktdma_chan_start(unit, tx_chan, bdcb);

    /* Poll for DMA completion */
    SHR_IF_ERR_EXIT(cmicd_pktdma_chan_poll(unit, tx_chan, poll));

exit:
    /* Abort tx chan DMA */
    cmicd_pktdma_chan_abort(unit, tx_chan, CMICD_PKTDMA_TX_DIR,
                            CMICD_PKTDMA_POLL);

    SHR_FUNC_EXIT();
}

static int
cmicd_pktdma_rx_test(int unit, void *bp, int rx_chan)
{
    pktdma_test_param_t *p = (pktdma_test_param_t *)bp;
    dma_addr_t bdcb = (dma_addr_t)p->rxdcb_dma_addr;
    int poll = p->intr_mode ? 0 : CMICD_PKTDMA_POLL;

    SHR_FUNC_ENTER(unit);

    /* Init rx chan DMA */
    cmicd_pktdma_chan_init(unit, rx_chan, CMICD_PKTDMA_RX_DIR);

    /* Start rx chan DMA */
    cmicd_pktdma_chan_start(unit, rx_chan, bdcb);

    /* Poll for DMA completion */
    SHR_IF_ERR_EXIT(cmicd_pktdma_chan_poll(unit, rx_chan, poll));

exit:
    /* Abort rx chan DMA */
    cmicd_pktdma_chan_abort(unit, rx_chan, CMICD_PKTDMA_RX_DIR,
                            CMICD_PKTDMA_POLL);
    SHR_FUNC_EXIT();
}

static int
cmicd_pktdma_select(int unit)
{
    return bcmdrd_feature_enabled(unit, BCMDRD_FT_CMICD);
}

static int
cmicd_pktdma_max_buff_size(int unit, int *size)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(size, SHR_E_PARAM);

    *size = PKTDMA_MAX_PKT_SIZE;
exit:
    SHR_FUNC_EXIT();
}

static int
cmicd_pktdma_dcb_setup(int unit, pktdma_test_param_t *p, uint32_t pkt_size)
{
    TX_DCB_t *tx_dcb;
    RX_DCB_t *rx_dcb;
    dma_addr_t addr;
    int hdr_size = 16;
    int max_buff_size;
    uint32_t idx, num;
    SHR_FUNC_ENTER(unit);

#define TXDCB_PTR(_i) &((TX_DCB_t *)p->txdcb)[(_i)]
#define RXDCB_PTR(_i) &((RX_DCB_t *)p->rxdcb)[(_i)]

    SHR_IF_ERR_EXIT
        (cmicd_pktdma_max_buff_size(unit, &max_buff_size));

    /* TX dcbs setup */
    for (idx = 0, num = 0; idx < p->num_dcbs; idx++) {
        tx_dcb = TXDCB_PTR(idx);
        /* Reload */
        if ((p->flags & PKTDMA_F_RELOAD) && (num == p->num_pkts / 2)) {
            /* Setup Reload DMA descriptor to next next descriptor */
            TX_DCB_CLR(*tx_dcb);
            addr = (dma_addr_t)bcmdrd_hal_dma_map(unit, TXDCB_PTR(idx + 2),
                                                  TX_DCB_SIZE, TRUE);
            if (!addr) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            TX_DCB_ADDRf_SET(*tx_dcb, addr);
            TX_DCB_RELOADf_SET(*tx_dcb, 1);
            TX_DCB_CHAINf_SET(*tx_dcb, 1);
            /* Skip next DMA descriptor */
            tx_dcb = TXDCB_PTR(++idx);
            TX_DCB_CLR(*tx_dcb);

            /* Setup next next DMA descriptor */
            tx_dcb = TXDCB_PTR(++idx);
        }

        /* SG */
        if (p->flags & PKTDMA_F_SG) {
            /* Setup first DMA descriptor */
            TX_DCB_CLR(*tx_dcb);
            addr = (dma_addr_t)bcmdrd_hal_dma_map(unit, p->txpkt[num],
                                                  hdr_size, TRUE);
            if (!addr) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            TX_DCB_ADDRf_SET(*tx_dcb, addr);
            TX_DCB_BYTE_COUNTf_SET(*tx_dcb, hdr_size);
            TX_DCB_SGf_SET(*tx_dcb, 1);
            TX_DCB_CHAINf_SET(*tx_dcb, 1);

            /* Setup second DMA descriptor */
            tx_dcb = TXDCB_PTR(++idx);
            TX_DCB_CLR(*tx_dcb);
            addr = (dma_addr_t)bcmdrd_hal_dma_map(unit, p->txpkt[num] + hdr_size,
                                                  pkt_size - hdr_size, TRUE);
            if (!addr) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            TX_DCB_ADDRf_SET(*tx_dcb, addr);
            TX_DCB_BYTE_COUNTf_SET(*tx_dcb, pkt_size - hdr_size);
        } else {
            /* Setup DMA descriptor */
            TX_DCB_CLR(*tx_dcb);
            addr = (dma_addr_t)bcmdrd_hal_dma_map(unit, p->txpkt[num],
                                                  pkt_size, TRUE);
            if (!addr) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            TX_DCB_ADDRf_SET(*tx_dcb, addr);
            TX_DCB_BYTE_COUNTf_SET(*tx_dcb, pkt_size);
        }

        /* Chain */
        if ((p->flags & PKTDMA_F_CHAIN) && (idx != p->num_dcbs - 1)) {
            TX_DCB_CHAINf_SET(*tx_dcb, 1);
        }

        /* Handle next packet */
        num++;
    }

    /* RX dcbs setup */
    for (idx = 0, num = 0; idx < p->num_dcbs; idx++) {
        rx_dcb = RXDCB_PTR(idx);
        /* Reload */
        if ((p->flags & PKTDMA_F_RELOAD) && (num == p->num_pkts / 2)) {
            /* Setup Reload DMA descriptor to next next descriptor */
            RX_DCB_CLR(*rx_dcb);
            addr = (dma_addr_t)bcmdrd_hal_dma_map(unit, RXDCB_PTR(idx + 2),
                                                  RX_DCB_SIZE, TRUE);
            if (!addr) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            RX_DCB_ADDRf_SET(*rx_dcb, addr);
            RX_DCB_RELOADf_SET(*rx_dcb, 1);
            RX_DCB_CHAINf_SET(*rx_dcb, 1);

            /* Skip next DMA descriptor */
            rx_dcb = RXDCB_PTR(++idx);
            RX_DCB_CLR(*rx_dcb);

            /* Setup next next DMA descriptor */
            rx_dcb = RXDCB_PTR(++idx);
        }

        /* SG */
        if (p->flags & PKTDMA_F_SG) {
            /* Setup first DMA descriptor */
            RX_DCB_CLR(*rx_dcb);
            addr = (dma_addr_t)bcmdrd_hal_dma_map(unit, p->rxpkt[num],
                                                  hdr_size, TRUE);
            if (!addr) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            RX_DCB_ADDRf_SET(*rx_dcb, addr);
            RX_DCB_BYTE_COUNTf_SET(*rx_dcb, hdr_size);
            RX_DCB_SGf_SET(*rx_dcb, 1);
            RX_DCB_CHAINf_SET(*rx_dcb, 1);

            rx_dcb = RXDCB_PTR(++idx);
            /* Setup second DMA descriptor */
            RX_DCB_CLR(*rx_dcb);
            addr = (dma_addr_t)bcmdrd_hal_dma_map(unit, p->rxpkt[num] + hdr_size,
                                                  max_buff_size - hdr_size, TRUE);
            if (!addr) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            RX_DCB_ADDRf_SET(*rx_dcb, addr);
            RX_DCB_BYTE_COUNTf_SET(*rx_dcb, max_buff_size - hdr_size);
        } else {
            /* Setup DMA descriptor */
            RX_DCB_CLR(*rx_dcb);
            addr = (dma_addr_t)bcmdrd_hal_dma_map(unit, p->rxpkt[num],
                                                  max_buff_size, TRUE);
            if (!addr) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            RX_DCB_ADDRf_SET(*rx_dcb, addr);
            RX_DCB_BYTE_COUNTf_SET(*rx_dcb, max_buff_size);
        }

        /* Chain */
        if ((p->flags & PKTDMA_F_CHAIN) && (idx != p->num_dcbs - 1)) {
            RX_DCB_CHAINf_SET(*rx_dcb, 1);
        }

        /* Handle next packet */
        num++;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
cmicd_pktdma_test_cb(int unit, void *bp, uint32_t option)
{
    pktdma_test_param_t *p = (pktdma_test_param_t *)bp;
    bool enable = false;
    uint32_t pkt_size;
    int max_buff_size = 0;
    int inc;
    int tx_chan = 0, rx_chan = 0;
    uint32_t idx, num;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(p, SHR_E_PARAM);

    /* Cleanup packet device for testing */
    (void)bcma_testutil_packet_dev_cleanup(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_knet_enable_get(unit, &enable));

    if (enable) {
        /* Handle all packet interrupts in user mode */
        SHR_IF_ERR_EXIT
            (bcmbd_knet_enable_set(unit, false));
    }

    /* Allocate Packet buffer */
    SHR_IF_ERR_EXIT
        (cmicd_pktdma_max_buff_size(unit, &max_buff_size));

    SHR_IF_ERR_EXIT
        (bcma_testcase_pktdma_buf_alloc(unit, p, max_buff_size));

    /* Allocate tx dcb from DMA memory pool */
    p->txdcb = bcmdrd_hal_dma_alloc(unit, TX_DCB_SIZE * p->num_dcbs,
                                    "bcmaTestCaseCmicdPKTDMATxDCB",
                                    &p->txdcb_dma_addr);
    SHR_NULL_CHECK(p->txdcb, SHR_E_MEMORY);

    /* Allocate rx dcb from DMA memory pool */
    p->rxdcb = bcmdrd_hal_dma_alloc(unit, RX_DCB_SIZE * p->num_dcbs,
                                    "bcmaTestCaseCmicdPKTDMARxDCB",
                                    &p->rxdcb_dma_addr);
    SHR_NULL_CHECK(p->rxdcb, SHR_E_MEMORY);

    /* Init packet dma chain interrupt */
    if (p->intr_mode) {
        SHR_IF_ERR_EXIT(cmicd_pktdma_intr_attach(unit));
    }

    /* Init packet dma driver */
    SHR_IF_ERR_EXIT(cmicd_pktdma_init(unit));

    for (idx = 0; idx < p->num_pairs; idx++, tx_chan++, rx_chan++) {
        /* Fetch tx, rx chan pair */
        while (((p->tx_ch_bmp>> tx_chan) % 2) == 0) {
            tx_chan++;
        }
        while (((p->rx_ch_bmp>> rx_chan) % 2) == 0) {
            rx_chan++;
        }

        if ((tx_chan > CMICD_PKTDMA_CHAN_MAX) ||
            (rx_chan > CMICD_PKTDMA_CHAN_MAX) ||
            (tx_chan == rx_chan)) {
            cli_out("Invalid chan tx %"PRId32" rx %"PRId32" max %"PRId32"\n",
                    tx_chan, rx_chan, CMICD_PKTDMA_CHAN_MAX);
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }

        if (p->pkt_size == 0) {
            /* coverity[dont_call] */
            pkt_size = PKTDMA_MIN_PKT_SIZE +
                       (uint32_t)sal_rand() % (PKTDMA_MAX_PKT_SIZE -
                                               PKTDMA_MIN_PKT_SIZE);
        } else {
            pkt_size = p->pkt_size;
        }

        /* Setup pktdma descriptors for tx rx pair */
        SHR_IF_ERR_EXIT(cmicd_pktdma_dcb_setup(unit, p, pkt_size));

        /* Fill in tx packet payload */
        for (num = 0; num < p->num_pkts; num++) {
            inc = idx * 16 + num;
            SHR_IF_ERR_EXIT(bcma_testcase_pktdma_packet_fill(
                                p->txpkt[num], pkt_size, inc));
        }

        SHR_IF_ERR_EXIT(cmicd_pktdma_tx_test(unit, bp, tx_chan));

        SHR_IF_ERR_EXIT(cmicd_pktdma_rx_test(unit, bp, rx_chan));

        if (p->verbose) {
            cli_out("idx %"PRId32" pkt_size %"PRId32" "
                    "tx_ch %"PRId32" rx_ch %"PRId32"\n",
                    idx, pkt_size, tx_chan, rx_chan);
            cmicd_pktdma_dump_tx_dcbs(unit, (uint32_t *)p->txdcb, p->num_dcbs,
                                      TX_DCB_WORD_SIZE, CMICD_PKTDMA_MHDR_SIZE);
            cmicd_pktdma_dump_rx_dcbs(unit, (uint32_t *)p->rxdcb, p->num_dcbs,
                                      RX_DCB_WORD_SIZE, CMICD_PKTDMA_MHDR_SIZE);
            for (num = 0; num < p->num_pkts; num++) {
                dump_packet("Tx Pkt First 64B:", p->txpkt[num], PKTDMA_MIN_PKT_SIZE);
                dump_packet("Rx Pkt First 64B:", p->rxpkt[num], PKTDMA_MIN_PKT_SIZE);
            }
        }

        /* Check packet content */
        if (p->check_pkt) {
            for (num = 0; num < p->num_pkts; num++) {
                if (sal_memcmp(p->txpkt[num], p->rxpkt[num], pkt_size)) {
                    cli_out("Packet integrity check failure at %"PRId32"\n", num);
                    dump_packet("Tx Pkt", p->txpkt[num], pkt_size);
                    dump_packet("Rx Pkt", p->rxpkt[num], pkt_size);
                    SHR_IF_ERR_EXIT(SHR_E_FAIL);
                }
            }
        }
    }

exit:
    if (p->intr_mode) {
        cmicd_pktdma_intr_detach(unit);
    }

    if (p->txdcb) {
        bcmdrd_hal_dma_free(unit, TX_DCB_SIZE * p->num_dcbs,
                            p->txdcb, p->txdcb_dma_addr);
        p->txdcb = NULL;
    }

    if (p->rxdcb) {
        bcmdrd_hal_dma_free(unit, RX_DCB_SIZE * p->num_dcbs,
                            p->rxdcb, p->rxdcb_dma_addr);
        p->rxdcb = NULL;
    }

    if (enable) {
        (void)bcmbd_knet_enable_set(unit, true);
    }

    bcma_testcase_pktdma_buf_free(unit, p, max_buff_size);

    SHR_FUNC_EXIT();
}

#define BCMA_TEST_PROC_DESCRIPTION \
    "Execute the Packet DMA test.\n" \
    "   0). Allocate Tx/Rx DCBs from DMA memory pool.\n" \
    "   1). Initialize packet DMA chain interrupts if needed.\n" \
    "   2). Initialize packet DMA driver.\n" \
    "   3). Fetch a Tx/Rx channel pair for test.\n" \
    "   4). Setup Tx/Rx channel descriptor and fill packet data.\n" \
    "   5). Start Tx channel to send packets.\n" \
    "   6). Start Rx channel to receive packets.\n" \
    "   7). Compare Tx/Rx packet data.\n" \
    "   8). Go to 3 for testing next Tx/Rx channel pair.\n" \
    "   9). Cleanup resources.\n" \
    ""

static bcma_test_proc_t cmicd_pktdma_proc[] = {
    {
        .desc = "setup cpu port\n",
        .cb = bcma_testutil_port_cpu_setup_cb,
    },
    {
        .desc = "create VLAN and add ports into VLAN\n",
        .cb = bcma_testcase_pktdma_vlan_set_cb,
    },
    {
        .desc = "configure a l2 user entry for copying to CPU\n",
        .cb = bcma_testcase_pktdma_l2_forward_add_cb,
    },
    {
        .desc = BCMA_TEST_PROC_DESCRIPTION,
        .cb = cmicd_pktdma_test_cb,
    },
    {
        .desc = "cleanup port and VLAN configuration\n",
        .cb = bcma_testcase_pktdma_cleanup_cb,
    },
};

#undef BCMA_TEST_PROC_DESCRIPTION

static bcma_test_op_t cmicd_pktdma_op = {
    .select = cmicd_pktdma_select,
    .parser = bcma_testcase_pktdma_parser,
    .help = bcma_testcase_pktdma_help,
    .recycle = bcma_testcase_pktdma_recycle,
    .procs = cmicd_pktdma_proc,
    .proc_count = COUNTOF(cmicd_pktdma_proc),
};

bcma_test_op_t *
bcma_testcase_cmicd_pktdma_op_get(void)
{
    return &cmicd_pktdma_op;
}
