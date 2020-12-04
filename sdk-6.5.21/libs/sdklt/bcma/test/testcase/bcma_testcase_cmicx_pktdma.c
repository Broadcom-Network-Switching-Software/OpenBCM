/*! \file bcma_testcase_cmicx_pktdma.c
 *
 * Broadcom test cases for CMICx packet DMA.
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

#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/bcmbd_cmicx_acc.h>
#include <bcmbd/bcmbd.h>

#include <bcmdrd/bcmdrd_feature.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/testcase/bcma_testcase_cmicx_pktdma.h>
#include <bcma/test/util/bcma_testutil_port.h>
#include <bcma/test/util/bcma_testutil_packet.h>

#include "bcma_testcase_pktdma_internal.h"

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

/* Tx/Rx DCB must be aligned in word */
#define TX_DCB_WORD_SIZE            SHR_BYTES2WORDS(TX_DCB_SIZE)
#define RX_DCB_WORD_SIZE            SHR_BYTES2WORDS(RX_DCB_SIZE)

/*! CMICX PCIE offset */
#define CMICX_PCIE_SO_OFFSET        0x10000000
/*! Higher DMA address to bus address */
#define DMA_TO_BUS_HI(dma)          ((dma) | CMICX_PCIE_SO_OFFSET)
/*! Higher bus address to DMA address */
#define BUS_TO_DMA_HI(bus)          ((bus) & ~CMICX_PCIE_SO_OFFSET)

#define CMICX_IP2EP_LB_HDR          16

static uint32_t cmicx_pktdma_chain_intr[] = {
    CMICX_IRQ_CMC0_CH0_CHAIN_DONE,
    CMICX_IRQ_CMC0_CH1_CHAIN_DONE,
    CMICX_IRQ_CMC0_CH2_CHAIN_DONE,
    CMICX_IRQ_CMC0_CH3_CHAIN_DONE,
    CMICX_IRQ_CMC0_CH4_CHAIN_DONE,
    CMICX_IRQ_CMC0_CH5_CHAIN_DONE,
    CMICX_IRQ_CMC0_CH6_CHAIN_DONE,
    CMICX_IRQ_CMC0_CH7_CHAIN_DONE,
    CMICX_IRQ_CMC1_CH0_CHAIN_DONE,
    CMICX_IRQ_CMC1_CH1_CHAIN_DONE,
    CMICX_IRQ_CMC1_CH2_CHAIN_DONE,
    CMICX_IRQ_CMC1_CH3_CHAIN_DONE,
    CMICX_IRQ_CMC1_CH4_CHAIN_DONE,
    CMICX_IRQ_CMC1_CH5_CHAIN_DONE,
    CMICX_IRQ_CMC1_CH6_CHAIN_DONE,
    CMICX_IRQ_CMC1_CH7_CHAIN_DONE
};

static sal_sem_t *cmicx_pktdma_intr[BCMDRD_CONFIG_MAX_UNITS];

#define CMICX_PKTDMA_CMC_MAX        1
#define CMICX_PKTDMA_CMC_CHAN       8
#define CMICX_PKTDMA_CHAN_MAX       (CMICX_PKTDMA_CMC_MAX * CMICX_PKTDMA_CMC_CHAN)
#define CMICX_PKTDMA_MHDR_SIZE      4
#define CMICX_PKTDMA_RX_DIR         0
#define CMICX_PKTDMA_TX_DIR         1

#define CMICX_PKTDMA_TIMEOUT        10000000
#define CMICX_PKTDMA_POLL           100000

#define DONE_PER_CH     (4)
#define CHAIN_OFFSET    (1)
#define CHAIN_DONE(ch)  (1 << (ch * DONE_PER_CH + CHAIN_OFFSET))

static void
clear_chan(int unit, int chan)
{
    CMIC_CMC_PKTDMA_CTRLr_t cdc;
    int cmc = chan / CMICX_PKTDMA_CMC_CHAN;
    int idx = chan % CMICX_PKTDMA_CMC_CHAN;

    SAL_CONFIG_MEMORY_BARRIER;

    /* Disable DMA */
    READ_CMIC_CMC_PKTDMA_CTRLr(unit, cmc, idx, &cdc);
    CMIC_CMC_PKTDMA_CTRLr_DMA_ENf_SET(cdc, 0);
    CMIC_CMC_PKTDMA_CTRLr_ABORT_DMAf_SET(cdc, 0);
    WRITE_CMIC_CMC_PKTDMA_CTRLr(unit, cmc, idx, cdc);

    SAL_CONFIG_MEMORY_BARRIER;

    return;
}

static void
cmicx_pktdma_chan_init(int unit, int chan, int dir)
{
    CMIC_CMC_PKTDMA_CTRLr_t cdc;
    CMIC_CMC_PKTDMA_COS_CTRL_RX_0r_t cos_ctrl0;
    CMIC_CMC_PKTDMA_COS_CTRL_RX_1r_t cos_ctrl1;
    int cmc = chan / CMICX_PKTDMA_CMC_CHAN;
    int idx = chan % CMICX_PKTDMA_CMC_CHAN;

    READ_CMIC_CMC_PKTDMA_CTRLr(unit, cmc, idx, &cdc);
    CMIC_CMC_PKTDMA_CTRLr_DIRECTIONf_SET(cdc, dir);
    WRITE_CMIC_CMC_PKTDMA_CTRLr(unit, cmc, idx, cdc);

    /* Enable 48 CPU COS queues for Rx channel */
    if (dir == CMICX_PKTDMA_RX_DIR) {
        CMIC_CMC_PKTDMA_COS_CTRL_RX_0r_SET(cos_ctrl0, 0xffffffff);
        WRITE_CMIC_CMC_PKTDMA_COS_CTRL_RX_0r(unit, cmc, idx, cos_ctrl0);
        CMIC_CMC_PKTDMA_COS_CTRL_RX_1r_SET(cos_ctrl1, 0xffff);
        WRITE_CMIC_CMC_PKTDMA_COS_CTRL_RX_1r(unit, cmc, idx, cos_ctrl1);
    }

    return;
}

static void
cmicx_pktdma_chan_start(int unit, int chan, uint64_t dcb)
{
    CMIC_CMC_PKTDMA_CTRLr_t cdc;
    CMIC_CMC_PKTDMA_DESC_ADDR_LOr_t cddl;
    CMIC_CMC_PKTDMA_DESC_ADDR_HIr_t cddh;
    int cmc = chan / CMICX_PKTDMA_CMC_CHAN;
    int idx = chan % CMICX_PKTDMA_CMC_CHAN;

    /* Write the DCB address to the DESC address for this channel */
    CMIC_CMC_PKTDMA_DESC_ADDR_LOr_CLR(cddl);
    CMIC_CMC_PKTDMA_DESC_ADDR_LOr_ADDRf_SET(cddl, dcb);
    WRITE_CMIC_CMC_PKTDMA_DESC_ADDR_LOr(unit, cmc, idx, cddl);

    CMIC_CMC_PKTDMA_DESC_ADDR_HIr_CLR(cddh);
    CMIC_CMC_PKTDMA_DESC_ADDR_HIr_ADDRf_SET(cddh, DMA_TO_BUS_HI(dcb >> 32));
    WRITE_CMIC_CMC_PKTDMA_DESC_ADDR_HIr(unit, cmc, idx, cddh);

    SAL_CONFIG_MEMORY_BARRIER;

    /* Kick it off */
    READ_CMIC_CMC_PKTDMA_CTRLr(unit, cmc, idx, &cdc);
    CMIC_CMC_PKTDMA_CTRLr_DMA_ENf_SET(cdc, 1);
    WRITE_CMIC_CMC_PKTDMA_CTRLr(unit, cmc, idx, cdc);

    SAL_CONFIG_MEMORY_BARRIER;

    /* Clear the DCB address to the DESC address for this channel */
    CMIC_CMC_PKTDMA_DESC_ADDR_LOr_CLR(cddl);
    WRITE_CMIC_CMC_PKTDMA_DESC_ADDR_LOr(unit, cmc, idx, cddl);

    CMIC_CMC_PKTDMA_DESC_ADDR_HIr_CLR(cddh);
    WRITE_CMIC_CMC_PKTDMA_DESC_ADDR_HIr(unit, cmc, idx, cddh);

    return;
}

static int
cmicx_pktdma_chan_poll(int unit, int chan, int polls)
{
    CMIC_CMC_SHARED_IRQ_STAT0r_t irq_stat0;
    sal_sem_t *intr = cmicx_pktdma_intr[unit];
    uint32_t done;
    int px;
    int cmc = chan / CMICX_PKTDMA_CMC_CHAN;
    int idx = chan % CMICX_PKTDMA_CMC_CHAN;

    SHR_FUNC_ENTER(unit);

    CMIC_CMC_SHARED_IRQ_STAT0r_CLR(irq_stat0);

    /* Interrupt mode */
    if (polls == 0) {
        /* Wait for channel done interrupt */
        if (sal_sem_take(intr[chan], CMICX_PKTDMA_TIMEOUT)) {
            SHR_ERR_EXIT(SHR_E_TIMEOUT);
        } else {
            READ_CMIC_CMC_SHARED_IRQ_STAT0r(unit, cmc, &irq_stat0);
            done = CMIC_CMC_SHARED_IRQ_STAT0r_GET(irq_stat0) & CHAIN_DONE(idx);
            if (done) {
                /* DMA complete. Clear the channel */
                clear_chan(unit, chan);

                SHR_EXIT();
            }
        }
    } else {
        for (px = 0; px < polls; px++) {
            READ_CMIC_CMC_SHARED_IRQ_STAT0r(unit, cmc, &irq_stat0);
            done = CMIC_CMC_SHARED_IRQ_STAT0r_GET(irq_stat0) & CHAIN_DONE(idx);
            if (done) {
                /* DMA complete. Clear the channel */
                clear_chan(unit, chan);

                SHR_EXIT();
            }
            sal_usleep(200);
        }
    }
    cli_out("chan %"PRId32" poll timeout dma_stat 0x%"PRIx32"\n",
            chan, irq_stat0._cmic_cmc_shared_irq_stat0);

    SHR_ERR_EXIT(SHR_E_TIMEOUT);

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_pktdma_chan_abort(int unit, int chan, int dir)
{
    CMIC_CMC_PKTDMA_CTRLr_t cdc;
    CMIC_CMC_PKTDMA_STATr_t dma_stat;
    CMIC_CMC_PKTDMA_COS_CTRL_RX_0r_t cos_ctrl0;
    CMIC_CMC_PKTDMA_COS_CTRL_RX_1r_t cos_ctrl1;
    uint32_t active;
    int px;
    int cmc = chan / CMICX_PKTDMA_CMC_CHAN;
    int idx = chan % CMICX_PKTDMA_CMC_CHAN;
    int polls = CMICX_PKTDMA_POLL;

    SHR_FUNC_ENTER(unit);

    /* Abort the channel */
    READ_CMIC_CMC_PKTDMA_CTRLr(unit, cmc, idx, &cdc);
    CMIC_CMC_PKTDMA_CTRLr_ABORT_DMAf_SET(cdc, 1);
    WRITE_CMIC_CMC_PKTDMA_CTRLr(unit, cmc, idx, cdc);

    SAL_CONFIG_MEMORY_BARRIER;

    /* Poll for abort completion */
    for (px = 0; px < polls; px++) {
        READ_CMIC_CMC_PKTDMA_STATr(unit, cmc, idx, &dma_stat);
        active = CMIC_CMC_PKTDMA_STATr_DMA_ACTIVEf_GET(dma_stat);
        if (!active) {
            /* DMA complete. Clear the channel */
            clear_chan(unit, chan);

            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_TIMEOUT);

exit:
    /* Disable all CPU COS queues for Rx channel */
    if (dir == CMICX_PKTDMA_RX_DIR) {
        CMIC_CMC_PKTDMA_COS_CTRL_RX_0r_CLR(cos_ctrl0);
        WRITE_CMIC_CMC_PKTDMA_COS_CTRL_RX_0r(unit, cmc, idx, cos_ctrl0);
        CMIC_CMC_PKTDMA_COS_CTRL_RX_1r_CLR(cos_ctrl1);
        WRITE_CMIC_CMC_PKTDMA_COS_CTRL_RX_1r(unit, cmc, idx, cos_ctrl1);
    }

    SHR_FUNC_EXIT();
}

static void
cmicx_pktdma_credits_release(int unit)
{
    CMIC_TOP_EPINTF_RELEASE_ALL_CREDITSr_t rel_credit;
    CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr_t max_credit;

    /*
     * Since only 6 bits of iproc_cmic_to_ep_credits[5:0] are being used,
     * so we have to set the max credits value twice in order to release
     * 64 credits to EP.
     * Only do this once when HW is initialized.
     */
    READ_CMIC_TOP_EPINTF_RELEASE_ALL_CREDITSr(unit, &rel_credit);
    if (CMIC_TOP_EPINTF_RELEASE_ALL_CREDITSr_GET(rel_credit)) {
        return;
    }
    CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr_CLR(max_credit);
    CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr_MAX_CREDITSf_SET(max_credit, 63);
    CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr_WR_EP_INTF_CREDITSf_SET(max_credit, 1);
    WRITE_CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr(unit, max_credit);
    CMIC_TOP_EPINTF_RELEASE_ALL_CREDITSr_RELEASE_ALL_CREDITSf_SET(rel_credit, 1);
    WRITE_CMIC_TOP_EPINTF_RELEASE_ALL_CREDITSr(unit, rel_credit);

    CMIC_TOP_EPINTF_RELEASE_ALL_CREDITSr_CLR(rel_credit);
    WRITE_CMIC_TOP_EPINTF_RELEASE_ALL_CREDITSr(unit, rel_credit);
    CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr_MAX_CREDITSf_SET(max_credit, 1);
    WRITE_CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr(unit, max_credit);
    CMIC_TOP_EPINTF_RELEASE_ALL_CREDITSr_RELEASE_ALL_CREDITSf_SET(rel_credit, 1);
    WRITE_CMIC_TOP_EPINTF_RELEASE_ALL_CREDITSr(unit, rel_credit);
}

static int
cmicx_pktdma_init(int unit)
{
    CMIC_CMC_PKTDMA_CTRLr_t dma_ctrl;
    int chan, cmc, idx;

    SHR_FUNC_ENTER(unit);

    /* Set Packet DMA endian */
    for (chan = 0; chan < CMICX_PKTDMA_CMC_MAX * CMICX_PKTDMA_CMC_CHAN; chan++) {
        cmc = chan / CMICX_PKTDMA_CMC_CHAN;
        idx = chan % CMICX_PKTDMA_CMC_CHAN;
        READ_CMIC_CMC_PKTDMA_CTRLr(unit, cmc, idx, &dma_ctrl);
        if (bcmdrd_dev_byte_swap_packet_dma_get(unit)) {
            CMIC_CMC_PKTDMA_CTRLr_PKTDMA_ENDIANESSf_SET(dma_ctrl, 1);
        }
        if (bcmdrd_dev_byte_swap_non_packet_dma_get(unit)) {
            CMIC_CMC_PKTDMA_CTRLr_DESC_ENDIANESSf_SET(dma_ctrl, 1);
            CMIC_CMC_PKTDMA_CTRLr_HEADER_ENDIANESSf_SET(dma_ctrl, 1);
        }
        WRITE_CMIC_CMC_PKTDMA_CTRLr(unit, cmc, idx, dma_ctrl);
    }
    cmicx_pktdma_credits_release(unit);
    SHR_FUNC_EXIT();
}

static void
cmicx_pktdma_chain_isr(int unit, uint32_t chan)
{
    sal_sem_t *intr = cmicx_pktdma_intr[unit];

    /* Disable the interrupt */
    bcmbd_cmicx_intr_disable_deferred(unit, cmicx_pktdma_chain_intr[chan]);

    sal_sem_give(intr[chan]);
}

static int
cmicx_pktdma_intr_attach(int unit)
{
    sal_sem_t *intr = NULL;
    int ch;

    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(intr, sizeof(sal_sem_t) * CMICX_PKTDMA_CHAN_MAX,
              "bcmaTestCaseCmicxPKTDMAIntr");
    SHR_NULL_CHECK(intr, SHR_E_MEMORY);
    sal_memset(intr, 0, sizeof(sal_sem_t) * CMICX_PKTDMA_CHAN_MAX);
    cmicx_pktdma_intr[unit] = intr;

    for (ch = 0; ch < CMICX_PKTDMA_CHAN_MAX; ch++) {
        intr[ch] = sal_sem_create("bcmaTestCaseCmicxPKTDMAIntr",
                                  SAL_SEM_BINARY, 0);
        SHR_NULL_CHECK(intr[ch], SHR_E_MEMORY);

        bcmbd_cmicx_intr_func_set(unit, cmicx_pktdma_chain_intr[ch],
                                  (bcmbd_intr_f)cmicx_pktdma_chain_isr,
                                  ch);
        bcmbd_cmicx_intr_enable(unit, cmicx_pktdma_chain_intr[ch]);
    }

exit:
    SHR_FUNC_EXIT();
}

static void
cmicx_pktdma_intr_detach(int unit)
{
    sal_sem_t *intr = cmicx_pktdma_intr[unit];
    int ch;

    for (ch = 0; ch < CMICX_PKTDMA_CHAN_MAX; ch++) {
        bcmbd_cmicx_intr_disable_deferred(unit, cmicx_pktdma_chain_intr[ch]);
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
cmicx_pktdma_dump_tx_dcbs(int unit, uint32_t *dcbs, int dcb_cnt,
                          int dcb_size, int mh_size)
{
    /* Dump DMA descriptor */
    dump_words("Tx DMA laddr =", dcbs, dcb_cnt, 0, dcb_size);
    dump_words("Tx DMA haddr =", dcbs, dcb_cnt, 1, dcb_size);
    dump_words("Tx DMA ctrl  =", dcbs, dcb_cnt, 2, dcb_size);
    dump_words("Tx DMA stat  =", dcbs, dcb_cnt, dcb_size - 1, dcb_size);

    return;
}

static void
cmicx_pktdma_dump_rx_dcbs(int unit, uint32_t *dcbs, int dcb_cnt,
                          int dcb_size, int mh_size)
{
    /* Dump DMA descriptor */
    dump_words("Rx DMA laddr =", dcbs, dcb_cnt, 0, dcb_size);
    dump_words("Rx DMA haddr =", dcbs, dcb_cnt, 1, dcb_size);
    dump_words("Rx DMA ctrl  =", dcbs, dcb_cnt, 2, dcb_size);
    dump_words("Rx DMA stat  =", dcbs, dcb_cnt, dcb_size - 1, dcb_size);

    return;
}

static void
dump_packet(char *prefix, uint32_t num, uint8_t *pkt_data, uint32_t pkt_len)
{
    uint32_t idx;

    cli_out("%s %uB index %u:", prefix, pkt_len, num);

    for (idx = 0; idx < pkt_len; idx++) {
        if (idx % 16 == 0) {
            cli_out("\n\t");
        }
        cli_out("%02"PRIx32" ", *pkt_data++);
    }
    cli_out("\n");
}

static int
cmicx_pktdma_txrx(int unit, pktdma_test_param_t *p, int tx_chan, int rx_chan,
                  uint32_t ep_to_cpu_hdr_sz)
{
    int poll = p->intr_mode ? 0 : CMICX_PKTDMA_POLL;
    uint32_t num;
    bool sim = bcmdrd_feature_is_sim(unit);

    SHR_FUNC_ENTER(unit);

    if (!sim) {
        cmicx_pktdma_chan_init(unit, rx_chan, CMICX_PKTDMA_RX_DIR);
        cmicx_pktdma_chan_start(unit, rx_chan, p->rxdcb_dma_addr);
    }

    cmicx_pktdma_chan_init(unit, tx_chan, CMICX_PKTDMA_TX_DIR);
    cmicx_pktdma_chan_start(unit, tx_chan, p->txdcb_dma_addr);

    /* Poll for Tx DMA completion. */
    SHR_IF_ERR_EXIT
        (cmicx_pktdma_chan_poll(unit, tx_chan, poll));

    /* Need chan start to receive packet in PLISIM. */
    if (sim) {
        cmicx_pktdma_chan_init(unit, rx_chan, CMICX_PKTDMA_RX_DIR);
        cmicx_pktdma_chan_start(unit, rx_chan, p->rxdcb_dma_addr);
    }

    /* Poll for Rx DMA completion. */
    SHR_IF_ERR_EXIT
        (cmicx_pktdma_chan_poll(unit, rx_chan, poll));

exit:
    if (p->verbose) {
        cmicx_pktdma_dump_tx_dcbs(unit, (uint32_t *)p->txdcb, p->num_dcbs,
                                  TX_DCB_WORD_SIZE, CMICX_PKTDMA_MHDR_SIZE);

        cmicx_pktdma_dump_rx_dcbs(unit, (uint32_t *)p->rxdcb, p->num_dcbs,
                                  RX_DCB_WORD_SIZE, CMICX_PKTDMA_MHDR_SIZE);

        for (num = 0; num < p->num_pkts; num++) {
            dump_packet("\nTx Pkt First", num, p->txpkt[num],
                        PKTDMA_MIN_PKT_SIZE);

            if (!p->cmic_lb) {
                dump_packet("Rx Pkt EP_TO_CPU", num, p->rxpkt[num],
                            ep_to_cpu_hdr_sz);
                dump_packet("Rx Pkt First", num,
                            p->rxpkt[num] + ep_to_cpu_hdr_sz,
                            PKTDMA_MIN_PKT_SIZE);
            } else {
                dump_packet("Rx Pkt First", num, p->rxpkt[num],
                            PKTDMA_MIN_PKT_SIZE);
            }
        }
    }

    cmicx_pktdma_chan_abort(unit, rx_chan, CMICX_PKTDMA_RX_DIR);

    cmicx_pktdma_chan_abort(unit, tx_chan, CMICX_PKTDMA_TX_DIR);

    SHR_FUNC_EXIT();
}

static int
cmicx_pktdma_select(int unit)
{
    return bcmdrd_feature_enabled(unit, BCMDRD_FT_CMICX);
}

static int
cmicx_pktdma_header_size(int unit, int *size)
{
    CMIC_TOP_EP_TO_CPU_HEADER_SIZEr_t chs;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(size, SHR_E_PARAM);

    READ_CMIC_TOP_EP_TO_CPU_HEADER_SIZEr(unit, &chs);
    *size = CMIC_TOP_EP_TO_CPU_HEADER_SIZEr_GET(chs) * 8;
exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_pktdma_max_buff_size(int unit, int *size)
{
    int hdr_size;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(size, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (cmicx_pktdma_header_size(unit, &hdr_size));

    *size = hdr_size + PKTDMA_MAX_PKT_SIZE;

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_pktdma_dcb_setup(int unit, pktdma_test_param_t *p, uint32_t pkt_size)
{
    TX_DCB_t *tx_dcb;
    RX_DCB_t *rx_dcb;
    uint64_t addr;
    int hdr_size, max_buff_size;
    uint32_t idx, num;

    SHR_FUNC_ENTER(unit);

#define TXDCB_PTR(_i) &((TX_DCB_t *)p->txdcb)[(_i)]
#define RXDCB_PTR(_i) &((RX_DCB_t *)p->rxdcb)[(_i)]

    SHR_IF_ERR_EXIT
        (cmicx_pktdma_header_size(unit, &hdr_size));

    SHR_IF_ERR_EXIT
        (cmicx_pktdma_max_buff_size(unit, &max_buff_size));

    /* TX DCBs setup */
    for (idx = 0, num = 0; idx < p->num_dcbs; idx++) {
        tx_dcb = TXDCB_PTR(idx);
        /* Reload */
        if ((p->flags & PKTDMA_F_RELOAD) && (num == p->num_pkts / 2)) {
            /* Set up Reload DMA descriptor */
            TX_DCB_CLR(*tx_dcb);
            addr = bcmdrd_hal_dma_map(unit, TXDCB_PTR(idx + 2), TX_DCB_SIZE,
                                      TRUE);
            if (!addr) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            TX_DCB_ADDR_LOf_SET(*tx_dcb, addr);
            TX_DCB_ADDR_HIf_SET(*tx_dcb, DMA_TO_BUS_HI(addr >> 32));
            TX_DCB_RELOADf_SET(*tx_dcb, 1);
            TX_DCB_CHAINf_SET(*tx_dcb, 1);
            /* Skip next DMA descriptor */
            tx_dcb = TXDCB_PTR(++idx);
            TX_DCB_CLR(*tx_dcb);

            /* Set up next-next DMA descriptor */
            tx_dcb = TXDCB_PTR(++idx);
        }

        /* Scatter/Gather */
        if (p->flags & PKTDMA_F_SG) {
            /* Set up first DMA descriptor */
            TX_DCB_CLR(*tx_dcb);
            addr = bcmdrd_hal_dma_map(unit, p->txpkt[num],
                                      PKTDMA_SG_MIN_DCB_SIZE, TRUE);
            if (!addr) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            TX_DCB_ADDR_LOf_SET(*tx_dcb, addr);
            TX_DCB_ADDR_HIf_SET(*tx_dcb, DMA_TO_BUS_HI(addr >> 32));
            TX_DCB_BYTE_COUNTf_SET(*tx_dcb, PKTDMA_SG_MIN_DCB_SIZE);
            TX_DCB_SGf_SET(*tx_dcb, 1);
            TX_DCB_CHAINf_SET(*tx_dcb, 1);

            /* Set up second DMA descriptor */
            tx_dcb = TXDCB_PTR(++idx);
            TX_DCB_CLR(*tx_dcb);
            addr = bcmdrd_hal_dma_map(unit,
                                      p->txpkt[num] + PKTDMA_SG_MIN_DCB_SIZE,
                                      pkt_size - PKTDMA_SG_MIN_DCB_SIZE,
                                      TRUE);
            if (!addr) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            TX_DCB_ADDR_LOf_SET(*tx_dcb, addr);
            TX_DCB_ADDR_HIf_SET(*tx_dcb, DMA_TO_BUS_HI(addr >> 32));
            TX_DCB_BYTE_COUNTf_SET(*tx_dcb,
                                   pkt_size - PKTDMA_SG_MIN_DCB_SIZE);
        } else {
            /* Set up DMA descriptor */
            TX_DCB_CLR(*tx_dcb);
            addr = bcmdrd_hal_dma_map(unit, p->txpkt[num], pkt_size, TRUE);
            if (!addr) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            TX_DCB_ADDR_LOf_SET(*tx_dcb, addr);
            TX_DCB_ADDR_HIf_SET(*tx_dcb, DMA_TO_BUS_HI(addr >> 32));
            TX_DCB_BYTE_COUNTf_SET(*tx_dcb, pkt_size);
        }

        /* Chain except if last DCB */
        if ((p->flags & PKTDMA_F_CHAIN) && (idx != p->num_dcbs - 1)) {
            TX_DCB_CHAINf_SET(*tx_dcb, 1);
        }

        /* Handle next packet */
        num++;
    }

    /* RX DCBs setup */
    for (idx = 0, num = 0; idx < p->num_dcbs; idx++) {
        rx_dcb = RXDCB_PTR(idx);
        /* Reload */
        if ((p->flags & PKTDMA_F_RELOAD) && (num == p->num_pkts / 2)) {
            /* Set up Reload DMA descriptor */
            RX_DCB_CLR(*rx_dcb);
            addr = bcmdrd_hal_dma_map(unit, RXDCB_PTR(idx + 2), RX_DCB_SIZE,
                                      TRUE);
            if (!addr) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            RX_DCB_ADDR_LOf_SET(*rx_dcb, addr);
            RX_DCB_ADDR_HIf_SET(*rx_dcb, DMA_TO_BUS_HI(addr >> 32));
            RX_DCB_RELOADf_SET(*rx_dcb, 1);
            RX_DCB_CHAINf_SET(*rx_dcb, 1);

            /* Skip next DMA descriptor */
            rx_dcb = RXDCB_PTR(++idx);
            RX_DCB_CLR(*rx_dcb);

            /* Set up next-next DMA descriptor */
            rx_dcb = RXDCB_PTR(++idx);
        }

        /* Scatter/Gather */
        if (p->flags & PKTDMA_F_SG) {
            /* Set up first DMA descriptor (not less than EP_TO_CPU_HDR) */
            RX_DCB_CLR(*rx_dcb);
            addr = bcmdrd_hal_dma_map(unit, p->rxpkt[num], hdr_size, TRUE);
            if (!addr) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            RX_DCB_ADDR_LOf_SET(*rx_dcb, addr);
            RX_DCB_ADDR_HIf_SET(*rx_dcb, DMA_TO_BUS_HI(addr >> 32));
            RX_DCB_BYTE_COUNTf_SET(*rx_dcb, hdr_size);
            RX_DCB_SGf_SET(*rx_dcb, 1);
            RX_DCB_CHAINf_SET(*rx_dcb, 1);

            rx_dcb = RXDCB_PTR(++idx);
            /* Set up second DMA descriptor */
            RX_DCB_CLR(*rx_dcb);
            addr = bcmdrd_hal_dma_map(unit, p->rxpkt[num] + hdr_size,
                                      max_buff_size - hdr_size, TRUE);
            if (!addr) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            RX_DCB_ADDR_LOf_SET(*rx_dcb, addr);
            RX_DCB_ADDR_HIf_SET(*rx_dcb, DMA_TO_BUS_HI(addr >> 32));
            RX_DCB_BYTE_COUNTf_SET(*rx_dcb, max_buff_size - hdr_size);
        } else {
            /* Set up DMA descriptor */
            RX_DCB_CLR(*rx_dcb);
            addr = bcmdrd_hal_dma_map(unit, p->rxpkt[num], max_buff_size, TRUE);
            if (!addr) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            RX_DCB_ADDR_LOf_SET(*rx_dcb, addr);
            RX_DCB_ADDR_HIf_SET(*rx_dcb, DMA_TO_BUS_HI(addr >> 32));
            RX_DCB_BYTE_COUNTf_SET(*rx_dcb, max_buff_size);
        }

        /* Chain except if last DCB */
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
cmicx_pktdma_test_cb(int unit, void *bp, uint32_t option)
{
    pktdma_test_param_t *p = (pktdma_test_param_t *)bp;
    bool enable = false;
    uint32_t pkt_size, ep_to_cpu_hdr_sz, lb_hdr_sz = 0;
    int max_buff_size = 0;
    int inc;
    int tx_chan = 0, rx_chan = 0;
    uint32_t idx, num;
    uint8_t *txpkt, *rxpkt;
    CMIC_TOP_CONFIGr_t ctc;
    CMIC_TOP_EP_TO_CPU_HEADER_SIZEr_t chs;

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
        (cmicx_pktdma_max_buff_size(unit, &max_buff_size));

    SHR_IF_ERR_EXIT
        (bcma_testcase_pktdma_buf_alloc(unit, p, max_buff_size));

    /* Allocate Tx DCB from DMA memory pool */
    p->txdcb = bcmdrd_hal_dma_alloc(unit, TX_DCB_SIZE * p->num_dcbs,
                                    "bcmaTestCaseCmicxPKTDMATxDCB",
                                    &p->txdcb_dma_addr);
    SHR_NULL_CHECK(p->txdcb, SHR_E_MEMORY);

    /* Allocate Rx DCB from DMA memory pool */
    p->rxdcb = bcmdrd_hal_dma_alloc(unit, RX_DCB_SIZE * p->num_dcbs,
                                    "bcmaTestCaseCmicxPKTDMARxDCB",
                                    &p->rxdcb_dma_addr);
    SHR_NULL_CHECK(p->rxdcb, SHR_E_MEMORY);

    /* Initialize packet DMA chain interrupt */
    if (p->intr_mode) {
        SHR_IF_ERR_EXIT(cmicx_pktdma_intr_attach(unit));
    }

    /* Initialize packet DMA driver */
    SHR_IF_ERR_EXIT(cmicx_pktdma_init(unit));

    READ_CMIC_TOP_EP_TO_CPU_HEADER_SIZEr(unit, &chs);
    ep_to_cpu_hdr_sz = CMIC_TOP_EP_TO_CPU_HEADER_SIZEr_GET(chs) * 8;

    if (p->cmic_lb) {
        /* Additional 16 bytes loopback header */
        lb_hdr_sz = CMICX_IP2EP_LB_HDR;
        READ_CMIC_TOP_CONFIGr(unit, &ctc);
        CMIC_TOP_CONFIGr_IP_2_EP_LOOPBACK_ENABLEf_SET(ctc, 1);
        WRITE_CMIC_TOP_CONFIGr(unit, ctc);
    }

    for (idx = 0; idx < p->num_pairs; idx++, tx_chan++, rx_chan++) {
        /* Fetch Tx/Rx channel pair */
        while (((p->tx_ch_bmp >> tx_chan) % 2) == 0) {
            tx_chan++;
        }

        while (((p->rx_ch_bmp >> rx_chan) % 2) == 0) {
            rx_chan++;
        }

        if ((tx_chan > CMICX_PKTDMA_CHAN_MAX) ||
            (rx_chan > CMICX_PKTDMA_CHAN_MAX) ||
            (tx_chan == rx_chan)) {
            cli_out("Invalid chan tx %"PRId32" rx %"PRId32" max %"PRId32"\n",
                    tx_chan, rx_chan, CMICX_PKTDMA_CHAN_MAX);
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

        /* Fill in Tx packet payload */
        for (num = 0; num < p->num_pkts; num++) {
            /* Initialize IP2EP lb header and set up the COS = 0 in lb_hdr[7] */
            if (p->cmic_lb) {
                sal_memset(p->txpkt[num], 0, lb_hdr_sz);
            }
            inc = idx * 16 + num;
            SHR_IF_ERR_EXIT
                (bcma_testcase_pktdma_packet_fill(p->txpkt[num] + lb_hdr_sz,
                                                  pkt_size, inc));
        }

        SHR_IF_ERR_EXIT
            (cmicx_pktdma_dcb_setup(unit, p, pkt_size + lb_hdr_sz));

        if (p->verbose) {
            cli_out("idx %"PRId32" pkt_size %"PRId32" "
                    "tx_ch %"PRId32" rx_ch %"PRId32"\n\n",
                    idx, pkt_size, tx_chan, rx_chan);
        }

        SHR_IF_ERR_EXIT
            (cmicx_pktdma_txrx(unit, p, tx_chan, rx_chan, ep_to_cpu_hdr_sz));

        /* Check packet content */
        if (p->check_pkt) {
            uint32_t chk_sz = 0;
            for (num = 0; num < p->num_pkts; num++) {
                /* Get original packet for dump. */
                if (p->cmic_lb) {
                    txpkt = p->txpkt[num] + lb_hdr_sz;
                    rxpkt = p->rxpkt[num] + lb_hdr_sz;
                    chk_sz = pkt_size;
                } else {
                    txpkt = p->txpkt[num];
                    rxpkt = p->rxpkt[num] + ep_to_cpu_hdr_sz;
                    chk_sz = pkt_size;
                }

                if (p->verbose) {
                    dump_packet("\nTx Pkt", num, txpkt, pkt_size);
                    dump_packet("Rx Pkt", num, rxpkt, pkt_size);
                }

                /* Skip first ep_to_cpu_hdr_sz due to endianness issue.*/
                if (p->cmic_lb) {
                    txpkt = p->txpkt[num] + ep_to_cpu_hdr_sz;
                    rxpkt = p->rxpkt[num] + ep_to_cpu_hdr_sz;
                    chk_sz -= ep_to_cpu_hdr_sz;
                }

                if (sal_memcmp(txpkt, rxpkt, chk_sz)) {
                    cli_out("Packet integrity check failure at %"PRId32"\n", num);
                    SHR_IF_ERR_EXIT(SHR_E_FAIL);
                }
            }
        }
    }

exit:
    if (p->cmic_lb) {
        READ_CMIC_TOP_CONFIGr(unit, &ctc);
        CMIC_TOP_CONFIGr_IP_2_EP_LOOPBACK_ENABLEf_SET(ctc, 0);
        WRITE_CMIC_TOP_CONFIGr(unit, ctc);
    }

    if (p->intr_mode) {
        cmicx_pktdma_intr_detach(unit);
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
static bcma_test_proc_t cmicx_pktdma_proc[] = {
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
        .cb = cmicx_pktdma_test_cb,
    },
    {
        .desc = "cleanup port and VLAN configuration\n",
        .cb = bcma_testcase_pktdma_cleanup_cb,
    },
};

#undef BCMA_TEST_PROC_DESCRIPTION

static bcma_test_op_t cmicx_pktdma_op = {
    .select = cmicx_pktdma_select,
    .parser = bcma_testcase_pktdma_parser,
    .help = bcma_testcase_pktdma_help,
    .recycle = bcma_testcase_pktdma_recycle,
    .procs = cmicx_pktdma_proc,
    .proc_count = COUNTOF(cmicx_pktdma_proc),
};

bcma_test_op_t *
bcma_testcase_cmicx_pktdma_op_get(void)
{
    return &cmicx_pktdma_op;
}
