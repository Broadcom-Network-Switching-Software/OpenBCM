/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:     CMICM interface drvier for SOC DMA
 */

#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <shared/bsl.h>

#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/dma.h>
#include <soc/drv.h>
#include <soc/dcb.h>
#include <soc/cmicm.h>
#include <soc/iproc.h>

#ifdef INCLUDE_KNET
#include <soc/knet.h>
#endif

#ifdef BCM_CMICM_SUPPORT

/* Static Driver Functions */

/*!
 * Dump registers for a channel
 */
static void
cmicm_pdma_chan_reg_dump(int unit, int vchan)
{
    uint32 val = 0, val2 = 0;
    int cmc, chan;

    cmc = vchan / N_DMA_CHAN;
    chan = vchan % N_DMA_CHAN;

    val = soc_pci_read(unit, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, chan));
    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                (BSL_META_U(unit,
                            "CMIC_CMC%d_CH%d_DMA_CTRL: 0x%08x\n"),
                 cmc, chan, val));

    val = soc_pci_read(unit, CMIC_CMCx_DMA_DESCy_OFFSET(cmc, chan));
    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                (BSL_META_U(unit,
                            "CMIC_CMC%d_DMA_DESC%d: 0x%08x\n"),
                 cmc, chan, val));

    val = soc_pci_read(unit, CMIC_CMCx_CHy_DMA_CURR_DESC_OFFSET(cmc, chan));
    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                (BSL_META_U(unit,
                            "CMIC_CMC%d_CH%d_DMA_CURR_DESC: 0x%08x\n"),
                 cmc, chan, val));

    val = soc_pci_read(unit,
                       CMIC_CMCx_DMA_CHy_DESC_HALT_ADDR_OFFSET(cmc, chan));
    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                (BSL_META_U(unit,
                            "CMIC_CMC%d_DMA_CH%d_DESC_HALT_ADDR: 0x%08x\n"),
                 cmc, chan, val));

    val = soc_pci_read(unit, CMIC_CMCx_CHy_COS_CTRL_RX_0_OFFSET(cmc, chan));
    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                (BSL_META_U(unit,
                            "CMIC_CMC%d_CH%d_COS_CTRL_RX_0: 0x%08x\n"),
                 cmc, chan, val));

    val = soc_pci_read(unit, CMIC_CMCx_CHy_COS_CTRL_RX_1_OFFSET(cmc, chan));
    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                (BSL_META_U(unit,
                            "CMIC_CMC%d_CH%d_COS_CTRL_RX_1: 0x%08x\n"),
                 cmc, chan, val));

    val = soc_pci_read(unit, CMIC_CMCx_PROGRAMMABLE_COS_MASK0_OFFSET(cmc));
    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                (BSL_META_U(unit,
                            "CMIC_CMC%d_PROGRAMMABLE_COS_MASK0: 0x%08x\n"),
                 cmc, val));

    val = soc_pci_read(unit, CMIC_CMCx_PROGRAMMABLE_COS_MASK1_OFFSET(cmc));
    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                (BSL_META_U(unit,
                            "CMIC_CMC%d_PROGRAMMABLE_COS_MASK1: 0x%08x\n"),
                 cmc, val));

    val = soc_pci_read(unit, CMIC_CMCx_DMA_CHy_INTR_COAL_OFFSET(cmc, chan));
    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                (BSL_META_U(unit,
                            "CMIC_CMC%d_DMA_CH%d_INTR_COAL: 0x%08x\n"),
                 cmc, chan, val));
    switch (chan) {
    case 0:
        val = soc_pci_read(unit,
                           CMIC_CMCx_CH0_RXBUF_THRESHOLD_CONFIG_OFFSET(cmc));
        break;
    case 1:
        val = soc_pci_read(unit,
                           CMIC_CMCx_CH1_RXBUF_THRESHOLD_CONFIG_OFFSET(cmc));
        break;
    case 2:
        val = soc_pci_read(unit,
                           CMIC_CMCx_CH2_RXBUF_THRESHOLD_CONFIG_OFFSET(cmc));
        break;
    case 3:
        val = soc_pci_read(unit,
                           CMIC_CMCx_CH3_RXBUF_THRESHOLD_CONFIG_OFFSET(cmc));
        break;
    default:
        break;
    }
    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                (BSL_META_U(unit,
                            "CMIC_CMC%d_CH%d_RXBUF_THRESHOLD_CONFIG:"
                            " 0x%08x\n"),
                 cmc, chan, val));

    val = soc_pci_read(unit, CMIC_CMCx_DMA_STAT_OFFSET(cmc));
    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                (BSL_META_U(unit,
                            "CMIC_CMC%d_DMA_STAT: 0x%08x\n"),
                 cmc, val));

    val = soc_pci_read(unit, CMIC_CMCx_DMA_STAT_HI_OFFSET(cmc));
    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                (BSL_META_U(unit,
                            "CMIC_CMC%d_DMA_STAT_HI: 0x%08x\n"),
                 cmc, val));

    val = soc_pci_read(unit, CMIC_CMCx_DMA_STAT_CLR_OFFSET(cmc));
    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                (BSL_META_U(unit,
                            "CMIC_CMC%d_DMA_STAT_CLR: 0x%08x\n"),
                 cmc, val));
    switch (chan) {
    case 0:
        val = soc_pci_read(unit, CMIC_CMCx_PKT_COUNT_CH0_RXPKT_OFFSET(cmc));
        val2 = soc_pci_read(unit, CMIC_CMCx_PKT_COUNT_CH0_TXPKT_OFFSET(cmc));
        break;
    case 1:
        val = soc_pci_read(unit, CMIC_CMCx_PKT_COUNT_CH1_RXPKT_OFFSET(cmc));
        val2 = soc_pci_read(unit, CMIC_CMCx_PKT_COUNT_CH1_TXPKT_OFFSET(cmc));
        break;
    case 2:
        val = soc_pci_read(unit, CMIC_CMCx_PKT_COUNT_CH2_RXPKT_OFFSET(cmc));
        val2 = soc_pci_read(unit, CMIC_CMCx_PKT_COUNT_CH2_TXPKT_OFFSET(cmc));
        break;
    case 3:
        val = soc_pci_read(unit, CMIC_CMCx_PKT_COUNT_CH3_RXPKT_OFFSET(cmc));
        val2 = soc_pci_read(unit, CMIC_CMCx_PKT_COUNT_CH3_TXPKT_OFFSET(cmc));
        break;
    default:
        break;
    }
    if (chan < N_DMA_CHAN) {
        LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                    (BSL_META_U(unit,
                                "CMIC_CMC%d_PKT_COUNT_CH%d_RXPKT: 0x%08x\n"),
                     cmc, chan, val));
        LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                    (BSL_META_U(unit,
                                "CMIC_CMC%d_PKT_COUNT_CH%d_TXPKT: 0x%08x\n"),
                     cmc, chan, val2));
    }

    val = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT0_OFFSET(cmc));
    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                (BSL_META_U(unit,
                            "CMIC_CMC%d_IRQ_STAT0: 0x%08x\n"),
                 cmc, val));

    val = soc_pci_read(unit, CMIC_CMCx_PCIE_IRQ_MASK0_OFFSET(cmc));
    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                (BSL_META_U(unit,
                            "CMIC_CMC%d_PCIE_IRQ_MASK0: 0x%08x\n"),
                 cmc, val));
}

/*
 * Function:
 *      cmicm_dma_chan_config
 * Purpose:
 *      Initialize the CMICM DMA channel for the specified SOC unit.
 * Parameters:
 *      unit - SOC unit #
 *      chan - channel #
 *      type - tx or rx
 *      f_intr - interrupt flags
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */
static int
cmicm_dma_chan_config(int unit, int vchan, dvt_t type, uint32 flags)
{
    int             rv = SOC_E_NONE;
    soc_control_t   *soc = SOC_CONTROL(unit);
    sdc_t           *sc = &soc->soc_channels[vchan];
    uint32          bits;
    int             cmc = vchan / N_DMA_CHAN;
    int             chan = vchan % N_DMA_CHAN;
    uint32          cr;

    /* Define locals and turn flags into easy to use things */
    /*int f_mbm = (flags & SOC_DMA_F_MBM) != 0; */
    int f_interrupt = (flags & SOC_DMA_F_POLL) == 0;
    /*int f_drop = (flags & SOC_DMA_F_TX_DROP) != 0; */
    int f_default = (flags & SOC_DMA_F_DEFAULT) != 0;
    int f_desc_intr = (flags & SOC_DMA_F_INTR_ON_DESC) != 0;
    int init_hw = TRUE;

    /* Initial state of channel */
    sc->sc_flags = 0;                   /* Clear flags to start */

#ifdef INCLUDE_KNET
    if (SOC_KNET_MODE(unit) && SOC_WARM_BOOT(unit) &&
        soc_property_get(unit, spn_WARMBOOT_KNET_SHUTDOWN_MODE, 0)) {
        init_hw = FALSE;
    }
#endif

    if (init_hw) {
        (void)soc_cmicm_cmcx_intr0_disable(unit, cmc, IRQ_CMCx_DESC_DONE(chan) |
                                            IRQ_CMCx_CHAIN_DONE(chan));

        cr = soc_pci_read(unit,CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc,chan));
        cr &= ~PKTDMA_ENABLE; /* clearing enable will also clear CHAIN_DONE */
        soc_pci_write(unit, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc,chan), cr);

        cr = soc_pci_read(unit, CMIC_CMCx_DMA_STAT_CLR_OFFSET(cmc));
        soc_pci_write(unit, CMIC_CMCx_DMA_STAT_CLR_OFFSET(cmc),
                      (cr | DS_DESCRD_CMPLT_CLR(chan)));
        soc_pci_write(unit, CMIC_CMCx_DMA_STAT_CLR_OFFSET(cmc), cr);
    }
    /* Setup new mode */
    /* MBM Deprecated in CMICm */
    /* DROP_TX Deprecated in CMICm */
    bits = f_desc_intr ? PKTDMA_SEL_INTR_ON_DESC_OR_PKT : 0;

    if (type == DV_TX) {
        bits |= PKTDMA_DIRECTION;
        if (f_default) {
            soc->soc_dma_default_tx = sc;
        }
    } else if (type == DV_RX) {
        bits &= ~PKTDMA_DIRECTION;
        if (f_default) {
            soc->soc_dma_default_rx = sc;
        }
    } else if (type == DV_NONE) {
        f_interrupt = FALSE;            /* Force off */
    } else {
        assert(0);
    }

#ifdef INCLUDE_KNET
    if (SOC_KNET_MODE(unit)) {
        /* Interrupt are handled by kernel */
        f_interrupt = FALSE;
    }
#endif

    if (f_interrupt) {
        (void)soc_cmicm_cmcx_intr0_enable(unit, cmc, IRQ_CMCx_DESC_DONE(chan)
                                     | IRQ_CMCx_CHAIN_DONE(chan));
    }

    sc->sc_type = type;
    if (init_hw) {
        cr = soc_pci_read(unit,CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc,chan));
        /* clear everything except Endianess */
        cr &= (PKTDMA_BIG_ENDIAN | PKTDMA_DESC_BIG_ENDIAN);
        cr |= bits;
        soc_pci_write(unit, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, chan), cr);
    }

    return rv;
}

/*
 * Function:
 *      cmicm_dma_chan_start
 * Purpose:
 *      Start the CMICM DMA channel for the specified SOC unit.
 * Parameters:
 *      unit - SOC unit #
 *      sc   - SOC channel data structure pointer
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */
static int
cmicm_dma_chan_start(int unit, sdc_t *sc)
{
    int             rv = SOC_E_NONE;
    dv_t            *dv;
    int             cmc = sc->sc_channel / N_DMA_CHAN;
    int             chan = sc->sc_channel % N_DMA_CHAN;
    uint32          val;

    if ((dv = sc->sc_dv_active = sc->sc_q) == NULL) {
        sc->sc_q_tail = NULL;
        return rv;
    }

#ifdef INCLUDE_KNET
    if (SOC_KNET_MODE(unit)) {
        return rv;
    }
#endif

    /* Set up DMA descriptor address */
    soc_pci_write(unit, CMIC_CMCx_DMA_DESCy_OFFSET(cmc, chan),
                  soc_cm_l2p(unit, sc->sc_q->dv_dcb));

    if (sc->sc_flags & SOC_DMA_F_CLR_CHN_DONE) {
        /* Clearing CMIC_CMC(0..2)_CH(0..3)_DMA_CTRL.ENABLE will
         * clear CMIC_CMC(0..2)_DMA_STAT.CHAIN_DONE bit.
         */
        val = soc_pci_read(unit,
                           CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, chan));
        val &= ~PKTDMA_ENABLE;
        soc_pci_write(unit,
                      CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, chan),
                      val);
    } else {
        sc->sc_flags |= SOC_DMA_F_CLR_CHN_DONE;
    }

    /* Start DMA */
    if (bsl_check(bslLayerSoc, bslSourcePacketdma, bslSeverityNormal, unit)) {
        cmicm_pdma_chan_reg_dump(unit ,sc->sc_channel);
    }
    val = soc_pci_read(unit,CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, chan));
    val |= PKTDMA_ENABLE;

    soc_pci_write(unit, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, chan), val);

    SDK_CONFIG_MEMORY_BARRIER;

    if (!(sc->sc_flags & SOC_DMA_F_POLL)) {
        (void)soc_cmicm_cmcx_intr0_enable(unit, cmc, IRQ_CMCx_CHAIN_DONE(chan));
    }

    return rv;
}

/*
 * Function:
 *      cmicm_dma_chan_dv_start
 * Purpose:
 *      Adds DV to the ready queue for CMICM DMA for the specified SOC unit.
 * Parameters:
 *      unit - SOC unit #
 *      chan - channel #
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 *      Assumes SOC_DMA_LOCK is held for CMIC_DMA_CTRL manipulation.
 */
static int
cmicm_dma_chan_dv_start(int unit, sdc_t *sc, dv_t *dv_chain)
{
    int rv = SOC_E_NONE;
#define DV_CHAINS_MAX   9
    static dv_t *dcs[N_DMA_CHAN][DV_CHAINS_MAX];
    static int di[N_DMA_CHAN] = {0};
    int ch;

    dv_chain->dv_next = NULL;
    if (sc->sc_q_cnt != 0) {
        sc->sc_q_tail->dv_next = dv_chain;
    } else {
        sc->sc_q = dv_chain;
    }
    sc->sc_q_tail = dv_chain;
    sc->sc_q_cnt++;

    /* WAR: fix chain done interrupt lost issue caused by BCMSIM */
    if (SAL_BOOT_BCMSIM) {
        ch = sc->sc_channel % N_DMA_CHAN;
        dcs[ch][di[ch]] = dv_chain;
        di[ch] = (di[ch] + 1) % DV_CHAINS_MAX;
        if (sc->sc_dv_active == NULL && sc->sc_q_cnt > 1) {
            sc->sc_q = dcs[ch][(di[ch] - sc->sc_q_cnt + 1 + DV_CHAINS_MAX) % DV_CHAINS_MAX];
            sc->sc_q_cnt--;
            return cmicm_dma_chan_start(unit, sc);
        }
    }

    if (sc->sc_q_cnt == 1) {    /* Start DMA if channel not active */
        rv = cmicm_dma_chan_start(unit, sc);
    }

    return rv;
}

/*
 * Function:
 *      cmicm_dma_chan_poll
 * Purpose:
 *      Poll the CMICM DMA channel for the specified SOC unit.
 * Parameters:
 *      unit - SOC unit #
 *      chan - channel #
 *      type - poll type (chain done or desc done)
 *      detected - poll result pointer to be updated
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 *      Assumes that SOC_DMA_LOCK is held when called.
 */
int
cmicm_dma_chan_poll(int unit,
                    int vchan,
                    soc_dma_poll_type_t type,
                    int * detected)
{
    int            rv = SOC_E_NONE;
    int            cmc = vchan / N_DMA_CHAN;
    int            chan = vchan % N_DMA_CHAN;

    switch (type) {
    case SOC_DMA_POLL_DESC_DONE:
        *detected = ( soc_pci_read(unit, CMIC_CMCx_DMA_STAT_OFFSET(cmc))
                     & DS_CMCx_DMA_DESC_DONE(chan) );
        break;
    case SOC_DMA_POLL_CHAIN_DONE:
        *detected = ( soc_pci_read(unit, CMIC_CMCx_DMA_STAT_OFFSET(cmc))
                     & DS_CMCx_DMA_CHAIN_DONE(chan) );
        break;
    default:
        break;
    }

    return rv;
}

/*
 * Function:
 *      cmicm_dma_chan_abort
 * Purpose:
 *      Initialize the CMICM DMA channel for the specified SOC unit.
 * Parameters:
 *      unit - SOC unit #
 *      chan - channel #
 *      type - tx or rx
 *      f_intr - interrupt flags
 * Returns:
 *      SOC_E_NONE   - Success
 *      SOC_E_TIMEOUT - failed (Timeout)
 * Notes:
 *      Assumes SOC_DMA_LOCK is held for CMIC_DMA_CTRL manipulation.
 */
static int
cmicm_dma_chan_abort(int unit, int vchan)
{
    int             rv = SOC_E_NONE;

    int             cmc = vchan / N_DMA_CHAN;
    int             chan = vchan % N_DMA_CHAN;
    int		        to;
    uint32          ctrl;
    uint32          val;

    if ((soc_pci_read(unit, CMIC_CMCx_DMA_STAT_OFFSET(cmc))
         & DS_CMCx_DMA_ACTIVE(chan)) != 0) {
        ctrl = soc_pci_read(unit, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, chan));
        ctrl |= PKTDMA_ENABLE;
        soc_pci_write(unit, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, chan), ctrl);
        soc_pci_write(unit,
                      CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, chan),
                      ctrl | PKTDMA_ABORT);
        SDK_CONFIG_MEMORY_BARRIER;

        to = soc_property_get(unit, spn_PDMA_TIMEOUT_USEC, 500000);
        while ((to >= 0) &&
               ((soc_pci_read(unit, CMIC_CMCx_DMA_STAT_OFFSET(cmc))
                 & DS_CMCx_DMA_ACTIVE(chan)) != 0)) {
            sal_udelay(1000);
            to -= 1000;
        }
        if ((soc_pci_read(unit, CMIC_CMCx_DMA_STAT_OFFSET(cmc))
             & DS_CMCx_DMA_ACTIVE(chan)) != 0) {
            LOG_ERROR(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "soc_dma_abort_channel unit %d: "
                                  "channel %d abort timeout\n"),
                       unit, chan));
            rv = SOC_E_TIMEOUT;
            if (SOC_WARM_BOOT(unit)) {
                return rv;
            }
        }
    }
    ctrl = soc_pci_read(unit, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, chan));
    /* Clearing enable will also clear CHAIN_DONE */
    ctrl &= ~(PKTDMA_ENABLE|PKTDMA_ABORT);
    soc_pci_write(unit, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, chan), ctrl);

    val = soc_pci_read(unit, CMIC_CMCx_DMA_STAT_CLR_OFFSET(cmc));
    soc_pci_write(unit,
                  CMIC_CMCx_DMA_STAT_CLR_OFFSET(cmc),
                  (val | DS_DESCRD_CMPLT_CLR(chan)));
    soc_pci_write(unit, CMIC_CMCx_DMA_STAT_CLR_OFFSET(cmc), val);
    SDK_CONFIG_MEMORY_BARRIER;

    return rv;
}

/*
 * Function:
 *      cmicm_dma_chan_desc_done
 * Purpose:
 *      Clears desc done on the CMICM DMA channel for the specified SOC unit.
 * Parameters:
 *      unit - SOC unit #
 *      chan - channel #
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 *      INTERRUPT LEVEL ROUTINE.
 */
static int
cmicm_dma_chan_desc_done(int unit, int vchan)
{
    int             rv = SOC_E_NONE;
    int             cmc = vchan / N_DMA_CHAN;
    int             chan = vchan % N_DMA_CHAN;
    uint32          val;

    /* Do we still need to generate an edge for CMICm and newer devices? */
    val = soc_pci_read(unit, CMIC_CMCx_DMA_STAT_CLR_OFFSET(cmc));
    soc_pci_write(unit, CMIC_CMCx_DMA_STAT_CLR_OFFSET(cmc),
                  (val | DS_DESCRD_CMPLT_CLR(chan)));
    soc_pci_write(unit, CMIC_CMCx_DMA_STAT_CLR_OFFSET(cmc), val);

    /* Flush posted writes from PCI bridge */
    (void)soc_pci_read(unit, CMIC_CMCx_DMA_STAT_OFFSET(cmc));

    return rv;
}

/*
 * Function:
 *      cmicm_dma_ctrl_init
 * Purpose:
 *      Initialize the CMICM DMA Control to a known good state for
 *      the specified SOC unit.
 * Parameters:
 *      unit - SOC unit #
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */

static int
cmicm_dma_ctrl_init(int unit)
{
    int             rv = SOC_E_NONE;
    int             cmc;
    uint32          val, chan, vchan;

    for(vchan=0; vchan < SOC_DCHAN_NUM(unit); vchan++) {
	cmc = vchan / N_DMA_CHAN;
	chan = vchan % N_DMA_CHAN;
        val = soc_pci_read(unit, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, chan));
        val &= (PKTDMA_BIG_ENDIAN | PKTDMA_DESC_BIG_ENDIAN);
        soc_pci_write(unit, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, chan), val);
    }

    /* Set to Legacy Mode */
    SOC_DMA_MODE(unit) = SOC_DMA_MODE_CHAINED;

    /* 
     * This has to be done only once after both CMIC and EP blocks
     * are out of reset.
     */
    soc_pci_write(unit, CMIC_RXBUF_EPINTF_RELEASE_ALL_CREDITS_OFFSET, 0);
    soc_pci_write(unit, CMIC_RXBUF_EPINTF_RELEASE_ALL_CREDITS_OFFSET, 1);

    return rv;
}

/*
 * Function:
 *      cmicm_dma_init
 * Purpose:
 *      Initialize the CMICM DMA for the specified SOC unit.
 * Parameters:
 *      unit - SOC unit #
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */

static int
cmicm_dma_init(int unit)
{
    int rv = SOC_E_NONE;
#ifdef BCM_TOMAHAWK_SUPPORT
    int cmc = 0;
    uint32 mask;

    /* Enable PCI completion time out recovery for Tomahawk/Tomahawk2 */
    if (SOC_IS_TOMAHAWK(unit) || SOC_IS_TOMAHAWK2(unit)) {
        /*Clear all PAXB interrupts */
        (void)READ_PAXB_0_PAXB_INTR_CLEARr(unit, &mask);
        mask = ~(uint32)0x0;
        (void)WRITE_PAXB_0_PAXB_INTR_CLEARr(unit, mask);

        /* Only enable PCIE_CMPL_TIMEOUT_INTR */
        mask = PCIE_CMPL_TIMEOUT_INTR_MASK;
        (void)WRITE_PAXB_0_PAXB_INTR_ENr(unit, mask);

        /* Enabling PCIe completion timeout interrupt */
        for(cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit); cmc++) {
            soc_cmicm_cmcx_intr2_enable(unit, cmc, IRQ_CMCx_PCI_CMPL_MASK);
        }
    }
#endif
    return rv;
}

/*
 * Function:
 *      cmicd_dma_pci_timeout_handle
 * Purpose:
 *      Handle the PCI timeout error.
 * Parameters:
 *      unit - SOC unit #
 *      sc   - SOC channel data structure pointer
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */
static int
cmicm_dma_pci_timeout_handle(int unit)
{
    int rv = SOC_E_NONE;
#ifdef BCM_TOMAHAWK_SUPPORT
    int cmc = 0;
    int chan;
    uint32 stat, stat_paxb, clr_pxab;
    uint32 val;
    soc_control_t *soc = SOC_CONTROL(unit);
    sdc_t *sc;
    dcb_t *dcb;
    int intr_clr = 0;

    if (SOC_IS_TOMAHAWK(unit) || SOC_IS_TOMAHAWK2(unit)) {
        (void)READ_PAXB_0_PAXB_INTR_STATUSr(unit, &stat_paxb);
        /* Check CMIC_CMC0_IRQ_STAT2.bit[19] is set */
        for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit); cmc++) {
            stat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT2_OFFSET(cmc));
            if (stat & IRQ_CMCx_PCI_CMPL_MASK) {
                /* Check if PAXB PCI complete timeout bit is set */
                if (stat_paxb & PCIE_CMPL_TIMEOUT_INTR_MASK) {
                    /* Check if address decode err is set. */
                    stat = soc_pci_read(unit, CMIC_CMCx_DMA_STAT_OFFSET(cmc));
                    for (chan = 0; chan < N_DMA_CHAN; chan++) {
                        /* If set, issue s/w abort to that channel */
                        if (DS_CMCx_DMA_ADDR_DECODE_ERR(chan) & stat) {
                            int vchan = cmc * N_DMA_CHAN + chan;
                            rv = cmicm_dma_chan_abort(unit, vchan);
                            /*Clear PCIe completion timeout interrupt, before starting DMA */
                            (void)READ_PAXB_0_PAXB_INTR_CLEARr(unit, &clr_pxab);
                            clr_pxab |= PCIE_CMPL_TIMEOUT_INTR_MASK;
                            (void)WRITE_PAXB_0_PAXB_INTR_CLEARr(unit, clr_pxab);
                            soc->stat.pci_cmpl_timeout++;
                            intr_clr = 1;
                            sc = &soc->soc_channels[vchan];
                            if (sc->sc_dv_active->dv_op == DV_RX) {
                                dcb = SOC_DCB_IDX2PTR(unit, sc->sc_dv_active->dv_dcb,
                                          sc->sc_dv_active->dv_dcnt);
                                soc_pci_write(unit,
                                    CMIC_CMCx_DMA_DESCy_OFFSET(cmc, chan),
                                    soc_cm_l2p(unit, dcb));
                            }
                            val = soc_pci_read(unit,
                                      CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, chan));
                            val |= PKTDMA_ENABLE;
                            soc_pci_write(unit,
                                CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, chan), val);
                        }
                    }
                }
            }
        }
        /* In case interrupt is not handled print status registers , clear PCI timeout interrupt*/
        if (!intr_clr) {
            LOG_ERROR(BSL_LS_SOC_PACKETDMA,
                          (BSL_META_U(unit, "Unhandled interrupt\n")));
            (void)READ_PAXB_0_PAXB_INTR_STATUSr(unit, &stat);
            LOG_ERROR(BSL_LS_SOC_PACKETDMA,
                          (BSL_META_U(unit, "PAXB_0_PAXB_INTR_STATUS = 0x%x\n"), stat));
            for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit); cmc++) {
                stat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT2_OFFSET(cmc));
                LOG_ERROR(BSL_LS_SOC_PACKETDMA,
                          (BSL_META_U(unit, "CMIC_CMCx_IRQ_STAT2_OFFSET[%d] = 0x%x\n"),
                               cmc, stat));
                stat = soc_pci_read(unit, CMIC_CMCx_DMA_STAT_OFFSET(cmc));
                LOG_ERROR(BSL_LS_SOC_PACKETDMA,
                          (BSL_META_U(unit, "CMIC_CMCx_DMA_STAT_OFFSET[%d] = 0x%x\n"),
                               cmc, stat));
            }
            /* Clear PCIe completion timeout interrupt */
            (void)READ_PAXB_0_PAXB_INTR_CLEARr(unit, &clr_pxab);
            clr_pxab |= PCIE_CMPL_TIMEOUT_INTR_MASK;
            (void)WRITE_PAXB_0_PAXB_INTR_CLEARr(unit, clr_pxab);
            soc->stat.pci_cmpl_timeout++;
        }
    }
#endif
    return rv;
}

/*
 * Function:
 *      cmicm_dma_chan_chain_done
 * Purpose:
 *      on the CMICM DMA channel for the specified SOC unit.
 * Parameters:
 *      unit - SOC unit #
 *      chan - channel #
 *      mitigation - >0 = interrupt mitigation on
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 *      INTERRUPT LEVEL ROUTINE.
 */
static int
cmicm_dma_chan_chain_done(int unit, int vchan, int mitigation)
{
    int             rv = SOC_E_NONE;
    int             cmc = vchan / N_DMA_CHAN;
    int             chan = vchan % N_DMA_CHAN;
    uint32          val;

    if (mitigation) {
        (void)soc_cmicm_cmcx_intr0_disable(unit, cmc,
                                      IRQ_CMCx_DESC_DONE(chan)
                                      | IRQ_CMCx_CHAIN_DONE(chan));
    } else {
        (void)soc_cmicm_cmcx_intr0_disable(unit,
                                           cmc, IRQ_CMCx_CHAIN_DONE(chan));
    }
    /* Enable DMA */
    val = soc_pci_read(unit,CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc,chan));
    val &= ~PKTDMA_ENABLE;
    soc_pci_write(unit, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc,chan), val);

    /* Clear the descriptor read complete stat bit */
    val = soc_pci_read(unit, CMIC_CMCx_DMA_STAT_CLR_OFFSET(cmc));
    soc_pci_write(unit,
                  CMIC_CMCx_DMA_STAT_CLR_OFFSET(cmc),
                  (val | DS_DESCRD_CMPLT_CLR(chan)));
    soc_pci_write(unit, CMIC_CMCx_DMA_STAT_CLR_OFFSET(cmc), val);

    /* Flush posted writes from PCI bridge */
    (void)soc_pci_read(unit, CMIC_CMCx_DMA_STAT_OFFSET(cmc));

    return rv;
}

/*
 * Function:
 *      cmicm_dma_chan_counter_get
 * Purpose:
 *      Obtain tx and rx counter values
 * Parameters:
 *      unit    - SOC unit #
 *      vchan   - SOC virtual channel number
 * Returns:
 *      SOC_E_NONE - success
 * Notes:
 */
static int
cmicm_dma_chan_counter_get(int unit, int vchan, uint32 *tx_pkt, uint32 *rx_pkt)
{
    int rv   = SOC_E_NONE;
    int cmc  = vchan / N_DMA_CHAN;
    int chan = vchan % N_DMA_CHAN;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel counter get\n")));

    *tx_pkt = soc_pci_read(unit, CMIC_CMCx_PKT_COUNT_CHy_TXPKT_OFFSET(cmc, chan));
    *rx_pkt = soc_pci_read(unit, CMIC_CMCx_PKT_COUNT_CHy_RXPKT_OFFSET(cmc, chan));

    return rv;
}

/*
 * Function:
 *      cmicm_dma_chan_counter_clear
 * Purpose:
 *      clear tx and rx counter values
 * Parameters:
 *      unit    - SOC unit #
 *      vchan   - SOC virtual channel number
 *      mask    - to clear the appopriate counter
 * Returns:
 *      SOC_E_NONE - success
 * Notes:
 */
static int
cmicm_dma_chan_counter_clear(int unit, int vchan, uint32 mask)
{
    int rv   = SOC_E_NONE;
    int cmc  = vchan / N_DMA_CHAN;
    int chan = vchan % N_DMA_CHAN;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel counter clear\n")));

    if (mask & 0x1) {
        soc_pci_write(unit, CMIC_CMCx_PKT_COUNT_CHy_TXPKT_OFFSET(cmc, chan), 0);
    }

    if (mask & 0x2) {
        soc_pci_write(unit, CMIC_CMCx_PKT_COUNT_CHy_RXPKT_OFFSET(cmc, chan), 0);
    }

    return rv;
}

/*
 * Function:
 *      cmicm_dma_cmc_counter_get
 * Purpose:
 *      Obtain tx and rx counter values
 * Parameters:
 *      unit    - SOC unit #
 *      cmc     - cmc number
 * Returns:
 *      SOC_E_NONE - success
 * Notes:
 */
static int
cmicm_dma_cmc_counter_get(int unit, int cmc, uint32 *tx_pkt, uint32 *rx_pkt)
{
    int rv   = SOC_E_NONE;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "cmc counter get\n")));

    *tx_pkt = soc_pci_read(unit, CMIC_CMCx_PKT_COUNT_TXPKT_OFFSET(cmc));
    *rx_pkt = soc_pci_read(unit, CMIC_CMCx_PKT_COUNT_RXPKT_OFFSET(cmc));

    return rv;
}

/*
 * Function:
 *      cmicm_dma_cmc_counter_clear
 * Purpose:
 *      clear tx and rx counter values
 * Parameters:
 *      unit    - SOC unit #
 *      cmc     - cmc number
 *      mask    - to clear the appopriate counter
 * Returns:
 *      SOC_E_NONE - success
 * Notes:
 */
static int
cmicm_dma_cmc_counter_clear(int unit, int cmc, uint32 mask)
{
    int rv   = SOC_E_NONE;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "cmc counter clear\n")));

    if (mask & 0x1) {
        soc_pci_write(unit, CMIC_CMCx_PKT_COUNT_TXPKT_OFFSET(cmc), 0);
    }

    if (mask & 0x2) {
        soc_pci_write(unit, CMIC_CMCx_PKT_COUNT_RXPKT_OFFSET(cmc), 0);
    }

    return rv;
}

/*
 * Function:
 *      cmicm_dma_chan_cos_ctrl_get
 * Purpose:
 *      get cos ctrl for a channel
 * Parameters:
 *      unit - SOC unit #
 *      cfg  - to choose appropriate register
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */

int
cmicm_dma_chan_cos_ctrl_get(int unit, int vchan, uint32 cfg, uint32 *cos_ctrl)
{
    int    rv  = SOC_E_NONE;
    int cmc  = vchan / N_DMA_CHAN;
    int chan = vchan % N_DMA_CHAN;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel cos ctrl get\n")));

    if (cfg & 0x1) {
        *cos_ctrl = soc_pci_read(unit, CMIC_CMCx_CHy_COS_CTRL_RX_0_OFFSET(cmc, chan));
    } else if (cfg & 0x2) {
        *cos_ctrl = soc_pci_read(unit, CMIC_CMCx_CHy_COS_CTRL_RX_1_OFFSET(cmc, chan));
    }

    return rv;
}

/*
 * Function:
 *      cmicm_dma_chan_cos_ctrl_set
 * Purpose:
 *      get cos ctrl for a channel
 * Parameters:
 *      unit - SOC unit #
 *      cfg  - to choose appropriate register
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */

int
cmicm_dma_chan_cos_ctrl_set(int unit, int vchan, uint32 cfg, uint32 cos_ctrl)
{
    int    rv  = SOC_E_NONE;
    int cmc  = vchan / N_DMA_CHAN;
    int chan = vchan % N_DMA_CHAN;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel cos ctrl set\n")));

    if (cfg & 0x1) {
        soc_pci_write(unit,
                      CMIC_CMCx_CHy_COS_CTRL_RX_0_OFFSET(cmc, chan),
                      cos_ctrl);
    } else if (cfg & 0x2) {
        soc_pci_write(unit,
                      CMIC_CMCx_CHy_COS_CTRL_RX_1_OFFSET(cmc, chan),
                      cos_ctrl);
    }

    return rv;
}

/*
 * Function:
 *      cmicm_dma_chan_cos_ctrl_queue_get
 * Purpose:
 *      get cos ctrl for a channel based on queue
 * Parameters:
 *      unit     - SOC unit #
 *      cmc      - CMC #
 *      chan     - chan #
 *      queue    - cos queue
 *      cos_ctrl - cos ctrl val
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */

int
cmicm_dma_chan_cos_ctrl_queue_get(int unit, int cmc, int chan,
                                  int queue, uint32 *cos_ctrl)
{
    int rv   = SOC_E_NONE;
    uint32 reg_addr;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel cos ctrl queue get\n")));

    reg_addr = (queue < 32) ?
                    CMIC_CMCx_CHy_COS_CTRL_RX_0_OFFSET(cmc, chan) :
                    CMIC_CMCx_CHy_COS_CTRL_RX_1_OFFSET(cmc, chan);

    *cos_ctrl = soc_pci_read(unit, reg_addr);

    return rv;
}

/*
 * Function:
 *      cmicm_dma_chan_cos_ctrl_reg_addr_get
 * Purpose:
 *      get cos ctrl reg addr for a channel based on queue
 * Parameters:
 *      unit     - SOC unit #
 *      cmc      - CMC #
 *      chan     - chan #
 *      queue    - cos queue
 *      reg addr - reg addr val
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */

int
cmicm_dma_chan_cos_ctrl_reg_addr_get(int unit, int cmc, int chan,
                                  int queue, uint32 *reg_addr)
{
    int rv   = SOC_E_NONE;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel cos ctrl reg addr get\n")));

    *reg_addr = (queue < 32) ?
                    CMIC_CMCx_CHy_COS_CTRL_RX_0_OFFSET(cmc, chan) :
                    CMIC_CMCx_CHy_COS_CTRL_RX_1_OFFSET(cmc, chan);

    return rv;
}

/*
 * Function:
 *      cmicm_dma_chan_status_get
 * Purpose:
 *      get channel status
 * Parameters:
 *      unit - SOC unit #
 *      vchan - channel #
 *      status - channel status
 *
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */

int
cmicm_dma_chan_status_get(int unit, int vchan, uint32 mask, int *status)
{
    int    rv  = SOC_E_NONE;
    int cmc  = vchan / N_DMA_CHAN;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel status get\n")));

    *status = soc_pci_read(unit, CMIC_CMCx_DMA_STAT_OFFSET(cmc));

    return rv;
}

/*
 * Function:
 *      cmicm_dma_chan_status_clear
 * Purpose:
 *      clear channel status
 * Parameters:
 *      unit - SOC unit #
 *      vchan - channel #
 *
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */

int
cmicm_dma_chan_status_clear(int unit, int vchan)
{
    int    rv  = SOC_E_NONE;
    int cmc  = vchan / N_DMA_CHAN;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel status clear\n")));

    soc_pci_write(unit, CMIC_CMCx_DMA_STAT_OFFSET(cmc), 0x0);

    return rv;
}

/*
 * Function:
 *      cmicm_dma_chan_ctrl_reg_get
 * Purpose:
 *      get channel ctrl reg value
 * Parameters:
 *      unit - SOC unit #
 *      vchan - channel #
 *      value - channel ctrl reg value
 *
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */

int
cmicm_dma_chan_ctrl_reg_get(int unit, int vchan, uint32 *val)
{
    int    rv  = SOC_E_NONE;
    int cmc  = vchan / N_DMA_CHAN;
    int chan = vchan % N_DMA_CHAN;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel ctrl reg get\n")));

    *val = soc_pci_read(unit, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, chan));

    return rv;
}

/*
 * Function:
 *      cmicm_dma_chan_ctrl_reg_set
 * Purpose:
 *      set channel ctrl reg
 * Parameters:
 *      unit - SOC unit #
 *      vchan - channel #
 *      value - channel ctrl reg value to set
 *
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */

int
cmicm_dma_chan_ctrl_reg_set(int unit, int vchan, uint32 val)
{
    int    rv  = SOC_E_NONE;
    int cmc  = vchan / N_DMA_CHAN;
    int chan = vchan % N_DMA_CHAN;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel ctrl reg get\n")));

    soc_pci_write(unit, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, chan), val);

    return rv;
}

/*
 * Function:
 *      cmicm_dma_chan_rxbuf_threshold_config
 * Purpose:
 *      appropriately set mmu back pressure
 * Parameters:
 *      unit - SOC unit #
 *      vchan - channel #
 *      value - mmu backpressure settings
 *
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */

int
cmicm_dma_chan_rxbuf_threshold_config(int unit, int vchan, uint32 val)
{
    int    rv  = SOC_E_NONE;
    int cmc  = vchan / N_DMA_CHAN;
    int chan = vchan % N_DMA_CHAN;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel rxbuf threshold config\n")));

    soc_pci_write(unit, CMIC_CMCx_CHy_RXBUF_THRESHOLD_CONFIG(cmc, chan), val);

    return rv;
}

/*
 * Function:
 *      cmicm_dma_chan_tx_purge
 * Purpose:
 *      Purge partial pkt left in the pipeline from TX DMA abort.
 * Parameters:
 *      unit - SOC unit #
 *      chan - channel #
 * Returns:
 *      SOC_E_NONE   - Success
 *      SOC_E_TIMEOUT - failed (Timeout)
 * Notes:
 *      Assumes SOC_DMA_LOCK is held for CMIC_DMA_CTRL manipulation.
 */
static int
cmicm_dma_chan_tx_purge(int unit, int vchan, dv_t *dv)
{
    int             rv = SOC_E_NONE;
    int             cmc = vchan / N_DMA_CHAN;
    int             chan = vchan % N_DMA_CHAN;
    sal_usecs_t     start_time;
    int             diff_time;
    uint32          dma_stat;
    uint32          dma_state;

    soc_pci_write(unit,
                  CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, chan),
                  soc_pci_read(unit, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc,chan))
                  | PKTDMA_DIRECTION);
    soc_pci_write(unit,
                  CMIC_CMCx_DMA_DESCy_OFFSET(cmc, chan),
                  soc_cm_l2p(unit, dv->dv_dcb));
    /* Start DMA */
    soc_pci_write(unit,
                  CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, chan),
                  soc_pci_read(unit, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc ,chan))
                  | PKTDMA_ENABLE);

    start_time = sal_time_usecs();
    diff_time = 0;
    dma_state = (DS_CMCx_DMA_DESC_DONE(chan) | DS_CMCx_DMA_CHAIN_DONE(chan));
    do {
        sal_udelay(SAL_BOOT_SIMULATION ? 1000 : 10);
        dma_stat = soc_pci_read(unit, CMIC_CMCx_DMA_STAT_OFFSET(cmc)) &
            (DS_CMCx_DMA_DESC_DONE(chan) | DS_CMCx_DMA_CHAIN_DONE(chan));
        diff_time = SAL_USECS_SUB(sal_time_usecs(), start_time);
        if (diff_time > DMA_ABORT_TIMEOUT) { /* 10 Sec(QT)/10 msec */
            rv = SOC_E_TIMEOUT;
            break;
        } else if (diff_time < 0) {
            /* Restart in case system time changed */
            start_time = sal_time_usecs();
        }
    } while (dma_stat != dma_state);
    /* Clear CHAIN_DONE and DESC_DONE */
    soc_pci_write(unit,
                  CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, chan),
                  soc_pci_read(unit,CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, chan))
                  & ~PKTDMA_ENABLE);

    dma_stat = soc_pci_read(unit, CMIC_CMCx_DMA_STAT_CLR_OFFSET(cmc));
    soc_pci_write(unit,
                  CMIC_CMCx_DMA_STAT_CLR_OFFSET(cmc),
                  (dma_stat | DS_DESCRD_CMPLT_CLR(chan)));
    soc_pci_write(unit, CMIC_CMCx_DMA_STAT_CLR_OFFSET(cmc), dma_stat);
    return rv;
}

/*
 * Function:
 *      cmicm_dma_chan_desc_done_intr_enable
 * Purpose:
 *      Clears desc done on the CMICM DMA channel for the specified SOC unit.
 * Parameters:
 *      unit - SOC unit #
 *      chan - channel #
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 *      INTERRUPT LEVEL ROUTINE.
 */
static int
cmicm_dma_chan_desc_done_intr_enable(int unit, int vchan)
{
    int rv = SOC_E_NONE;
    int cmc = vchan / N_DMA_CHAN;
    int chan = vchan % N_DMA_CHAN;

    (void)soc_cmicm_cmcx_intr0_enable(unit, cmc, IRQ_CMCx_DESC_DONE(chan));
    return rv;
}

/*
 * Function:
 *      cmicm_dma_masks_get
 * Purpose:
 *      obtain the individual big endian masks
 * Parameters:
 *      unit          - SOC unit #
 *      pkt_endian    - pkt endian mask
 *      desc_endian   - desc endian mask
 *      header_endian - header endian mask
 *
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */

int
cmicm_dma_masks_get(int unit, uint32 *pkt_endian, uint32 *desc_endian, uint32 *header_endian)
{
    int rv = SOC_E_NONE;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "cmicm dma get masks\n")));

    *pkt_endian    = PKTDMA_BIG_ENDIAN;
    *desc_endian   = PKTDMA_DESC_BIG_ENDIAN;

    return rv;	
}

/*
 * Function:
 *      cmicm_dma_header_size_get
 * Purpose:
 *      obtain the ep to cpu header size
 * Parameters:
 *      unit     - SOC unit #
 *      hdr_size - return value of the ep to cpu header size
 *
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */

int
cmicm_dma_header_size_get(int unit, uint32 *hdr_size)
{
    int rv = SOC_E_NONE;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "cmicm dma get header size\n")));

    *hdr_size = 0;

    return rv;
}

/*
 * Function:
 *      cmicm_dma_max_rx_channels_get
 * Purpose:
 *      obtain maximum number of dma channels that can be used for BCM RX
 * Parameters:
 *      unit - SOC unit #
 *      max_rx_channels - maximum rx channels that can be used
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error
 */

static int
cmicm_dma_max_rx_channels_get(int unit, uint32 *max_rx_channels)
{
    int rv = SOC_E_NONE;

    /*
     * Only CMC0 is used for BCM RX hence we can have
     * N_DMA_CHAN number of maximum rx channels
     */

    *max_rx_channels = N_DMA_CHAN;

    return rv;
}

/* Public Functions */

/*
 * Function:
 *      soc_cmicm_dma_drv_init
 * Purpose:
 *      Initialize the CMICM DMA driver for the specified SOC unit.
 *      Assign function pointers for SOC DMA driver interface.
 * Parameters:
 *      unit - SOC unit #
 *      drv  - pointer to the function vector list
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */
int
soc_cmicm_dma_drv_init(int unit, soc_cmic_dma_drv_t *drv)
{
    int rv = SOC_E_NONE;

    drv->init                       = cmicm_dma_init;
    drv->ctrl_init                  = cmicm_dma_ctrl_init;
    drv->chan_config                = cmicm_dma_chan_config;
    drv->chan_dv_start              = cmicm_dma_chan_dv_start;
    drv->chan_start                 = cmicm_dma_chan_start;
    drv->chan_poll                  = cmicm_dma_chan_poll;
    drv->chan_abort                 = cmicm_dma_chan_abort;
    drv->chan_tx_purge              = cmicm_dma_chan_tx_purge;
    drv->chan_desc_done_intr_enable = cmicm_dma_chan_desc_done_intr_enable;
    drv->chan_desc_done             = cmicm_dma_chan_desc_done;
    drv->chan_chain_done            = cmicm_dma_chan_chain_done;
    drv->chan_reload_done           = NULL;
    drv->chan_counter_get           = cmicm_dma_chan_counter_get;
    drv->chan_counter_clear         = cmicm_dma_chan_counter_clear;
    drv->chan_cos_ctrl_get          = cmicm_dma_chan_cos_ctrl_get;
    drv->chan_cos_ctrl_set          = cmicm_dma_chan_cos_ctrl_set;
    drv->chan_cos_ctrl_queue_get    = cmicm_dma_chan_cos_ctrl_queue_get;
    drv->chan_cos_ctrl_reg_addr_get = cmicm_dma_chan_cos_ctrl_reg_addr_get;
    drv->chan_status_get            = cmicm_dma_chan_status_get;
    drv->chan_status_clear          = cmicm_dma_chan_status_clear;
    drv->chan_halt_get              = NULL;
    drv->chan_ctrl_reg_get          = cmicm_dma_chan_ctrl_reg_get;
    drv->chan_ctrl_reg_set          = cmicm_dma_chan_ctrl_reg_set;
    drv->chan_rxbuf_threshold_cfg   = cmicm_dma_chan_rxbuf_threshold_config;
    drv->cmc_rxbuf_threshold_cfg    = NULL;
    drv->cmc_counter_get            = cmicm_dma_cmc_counter_get;
    drv->cmc_counter_clear          = cmicm_dma_cmc_counter_clear;
    drv->loopback_config            = NULL;
    drv->masks_get                  = cmicm_dma_masks_get;
    drv->header_size_get            = cmicm_dma_header_size_get;
    drv->max_rx_channels_get        = cmicm_dma_max_rx_channels_get;
    drv->pci_timeout_handle         = cmicm_dma_pci_timeout_handle;

    return rv;
}

#endif /* BCM_CMICM_SUPPORT */
