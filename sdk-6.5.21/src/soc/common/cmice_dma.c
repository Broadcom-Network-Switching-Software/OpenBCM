/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:     CMICE interface drvier for SOC DMA
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

#ifdef INCLUDE_KNET
#include <soc/knet.h>
#endif

#ifdef BCM_CMIC_SUPPORT

/* Static Driver Functions */

/*
 * Function:
 *      cmice_dma_ctrl_init
 * Purpose:
 *      Reset the CMICE DMA Control to a known good state for
 *      the specified SOC unit.
 * Parameters:
 *      unit - SOC unit #
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */
static int
cmice_dma_ctrl_init(int unit)
{
    int rv = SOC_E_NONE;

    soc_pci_write(unit, CMIC_DMA_CTRL, 0); /* Known good state */

    return rv;
}

/*
 * Function:
 *      cmice_dma_init
 * Purpose:
 *      Initialize the CMICE DMA for the specified SOC unit.
 * Parameters:
 *      unit - SOC unit #
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */
static int
cmice_dma_init(int unit)
{
    int rv = SOC_E_NONE;

    cmice_dma_ctrl_init(unit);

    return rv;
}

/*
 * Function:
 *      cmice_dma_chan_config
 * Purpose:
 *      Configure the CMICE DMA channel for the specified SOC unit.
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
cmice_dma_chan_config(int unit, int chan, dvt_t type, uint32 flags)
{
    int           rv = SOC_E_NONE;
    soc_control_t *soc = SOC_CONTROL(unit);
    sdc_t         *sc = &soc->soc_channels[chan];
    uint32        bits, cr;

    /* Define locals and turn flags into easy to use things */
    int f_mbm = (flags & SOC_DMA_F_MBM) != 0;
    int f_interrupt = (flags & SOC_DMA_F_POLL) == 0;
    int f_drop = (flags & SOC_DMA_F_TX_DROP) != 0;
    int f_default = (flags & SOC_DMA_F_DEFAULT) != 0;
    int f_desc_intr = (flags & SOC_DMA_F_INTR_ON_DESC) != 0;
    int init_hw = TRUE;

#ifdef INCLUDE_KNET
    if (SOC_KNET_MODE(unit) && SOC_WARM_BOOT(unit) &&
        soc_property_get(unit, spn_WARMBOOT_KNET_SHUTDOWN_MODE, 0)) {
        init_hw = FALSE;
    }
#endif

    /* Initial state of channel */
    sc->sc_flags = 0;                   /* Clear flags to start */
    if (init_hw) {
        (void)soc_intr_disable(unit, IRQ_DESC_DONE(chan) | IRQ_CHAIN_DONE(chan));
        soc_pci_write(unit, CMIC_DMA_STAT, DS_DMA_EN_CLR(chan));
        soc_pci_write(unit, CMIC_DMA_STAT, DS_DESC_DONE_CLR(chan));
        soc_pci_write(unit, CMIC_DMA_STAT, DS_CHAIN_DONE_CLR(chan));
    }

    /* Setup new mode */
    bits  = f_mbm ? DC_MOD_BITMAP(chan) : DC_NO_MOD_BITMAP(chan);
    bits |= f_drop ? DC_DROP_TX(chan) : DC_NO_DROP_TX(chan);
    bits |= f_desc_intr ? DC_INTR_ON_DESC(chan) : DC_INTR_ON_PKT(chan);

    if (type == DV_TX) {
        bits |= DC_MEM_TO_SOC(chan);
        if (f_default) {
            soc->soc_dma_default_tx = sc;
        }
    } else if (type == DV_RX) {
        bits |= DC_SOC_TO_MEM(chan);
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
        (void)soc_intr_enable(unit,
                              IRQ_DESC_DONE(chan) | IRQ_CHAIN_DONE(chan));
    }
    sc->sc_type = type;

    if (init_hw) {
        cr = soc_pci_read(unit, CMIC_DMA_CTRL);
        cr &= ~DC_CHAN_MASK(chan);             /* Clear this channels bits */
        cr |= bits;                         /* Set new values */
        soc_pci_write(unit, CMIC_DMA_CTRL, cr);
    }

    return rv;
}

/*
 * Function:
 *      cmice_dma_chan_start
 * Purpose:
 *      Start the CMICE DMA channel for the specified SOC unit.
 * Parameters:
 *      unit - SOC unit #
 *      sc   - SOC channel data structure pointer
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */
static int
cmice_dma_chan_start(int unit, sdc_t *sc)
{
    int rv = SOC_E_NONE;
    dv_t *dv;

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
    soc_pci_write(unit, CMIC_DMA_DESC(sc->sc_channel),
                  soc_cm_l2p(unit, sc->sc_q->dv_dcb));
    /* Start DMA (Actually starts when the done bit is cleared below) */
    soc_pci_write(unit, CMIC_DMA_STAT,
                  DS_DMA_EN_SET(sc->sc_channel));

    SDK_CONFIG_MEMORY_BARRIER;

    /*
     * Clear CHAIN_DONE if required, and re-enable the
     * interrupt. This is required to support multiple DMA
     */
    if (sc->sc_flags & SOC_DMA_F_CLR_CHN_DONE) {
        soc_pci_write(unit, CMIC_DMA_STAT,
                      DS_CHAIN_DONE_CLR(sc->sc_channel));
    } else {
        sc->sc_flags |= SOC_DMA_F_CLR_CHN_DONE;
    }
    if (!(sc->sc_flags & SOC_DMA_F_POLL)) {
        (void)soc_intr_enable(unit, IRQ_CHAIN_DONE(sc->sc_channel));
    }

    return rv;
}

/*
 * Function:
 *      cmice_dma_chan_dv_start
 * Purpose:
 *      Adds DV to the ready queue for CMICE DMA for the specified SOC unit.
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
cmice_dma_chan_dv_start(int unit, sdc_t *sc, dv_t *dv_chain)
{
    int rv = SOC_E_NONE;

    dv_chain->dv_next = NULL;
    if (sc->sc_q_cnt != 0) {
        sc->sc_q_tail->dv_next = dv_chain;
    } else {
        sc->sc_q = dv_chain;
    }
    sc->sc_q_tail = dv_chain;
    sc->sc_q_cnt++;
    if (sc->sc_dv_active == NULL) {     /* Start DMA if channel not active */
        rv = cmice_dma_chan_start(unit, sc);
    }

    return rv;
}

/*
 * Function:
 *      cmice_dma_chan_poll
 * Purpose:
 *      Poll the CMICE DMA channel for the specified SOC unit.
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
static int
cmice_dma_chan_poll(int unit,
                    int chan,
                    soc_dma_poll_type_t type,
                    int *detected)
{
    int rv = SOC_E_NONE;

    switch (type) {
    case SOC_DMA_POLL_DESC_DONE:
        *detected = ( soc_pci_read(unit, CMIC_DMA_STAT)
                      & DS_DESC_DONE_TST(chan) );
        break;
    case SOC_DMA_POLL_CHAIN_DONE:
        *detected = ( soc_pci_read(unit, CMIC_DMA_STAT)
                      & DS_CHAIN_DONE_TST(chan) );
        break;
    default:
        break;
    }

    return rv;
}

/*
 * Function:
 *      cmice_dma_chan_abort
 * Purpose:
 *      Aborts active DMA on the CMICE DMA channel for the specified SOC unit.
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
cmice_dma_chan_abort(int unit, int chan)
{
    int		        to;
    uint32          ctrl;
    int             rv = SOC_E_NONE;

    soc_pci_write(unit, CMIC_DMA_STAT, DS_DMA_EN_CLR(chan));

    SDK_CONFIG_MEMORY_BARRIER;

    ctrl = soc_pci_read(unit, CMIC_DMA_CTRL);
    assert((ctrl & DC_ABORT_DMA(chan)) == 0);
    soc_pci_write(unit, CMIC_DMA_CTRL, ctrl | DC_ABORT_DMA(chan));

    SDK_CONFIG_MEMORY_BARRIER;

    to = soc_property_get(unit, spn_PDMA_TIMEOUT_USEC, 500000);

    while ((to >= 0) &&
           ((soc_pci_read(unit, CMIC_DMA_STAT) & DS_DMA_ACTIVE(chan)) != 0)) {
        sal_udelay(1000);
        to -= 1000;
    }

    if ((soc_pci_read(unit, CMIC_DMA_STAT) & DS_DMA_ACTIVE(chan)) != 0) {
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

    soc_pci_write(unit, CMIC_DMA_CTRL, ctrl);
    soc_pci_write(unit, CMIC_DMA_STAT, DS_DESC_DONE_CLR(chan));
    soc_pci_write(unit, CMIC_DMA_STAT, DS_CHAIN_DONE_CLR(chan));

    SDK_CONFIG_MEMORY_BARRIER;

    return rv;
}

/*
 * Function:
 *      cmice_dma_chan_tx_purge
 * Purpose:
 *      Aborts active DMA on the CMICE DMA channel for the specified SOC unit.
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
cmice_dma_chan_tx_purge(int unit, int chan, dv_t *dv)
{
    sal_usecs_t start_time;
    int         diff_time;
    uint32      dma_stat;
    uint32      dma_state;
    uint32      ctrl;
    int         rv = SOC_E_NONE;

    ctrl = soc_pci_read(unit, CMIC_DMA_CTRL);
    soc_pci_write(unit, CMIC_DMA_CTRL, ctrl | DC_MEM_TO_SOC(chan));
    soc_pci_write(unit, CMIC_DMA_DESC(chan),
                  soc_cm_l2p(unit, dv->dv_dcb));
    /* Start DMA */
    soc_pci_write(unit, CMIC_DMA_STAT, DS_DMA_EN_SET(chan));

    start_time = sal_time_usecs();
    diff_time = 0;
    dma_state = (DS_DESC_DONE_TST(chan) | DS_CHAIN_DONE_TST(chan));
    do {
        sal_udelay(SAL_BOOT_SIMULATION ? 1000 : 10);
        dma_stat = soc_pci_read(unit, CMIC_DMA_STAT) &
            (DS_DESC_DONE_TST(chan) | DS_CHAIN_DONE_TST(chan));
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
    soc_pci_write(unit, CMIC_DMA_STAT, DS_DMA_EN_CLR(chan));
    soc_pci_write(unit, CMIC_DMA_STAT, DS_DESC_DONE_CLR(chan));
    soc_pci_write(unit, CMIC_DMA_STAT, DS_CHAIN_DONE_CLR(chan));
    soc_pci_write(unit, CMIC_DMA_CTRL, ctrl);

    return rv;
}

/*
 * Function:
 *      cmice_dma_chan_desc_done_intr_enable
 * Purpose:
 *      Clears desc done on the CMICE DMA channel for the specified SOC unit.
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
cmice_dma_chan_desc_done_intr_enable(int unit, int chan)
{
    int rv = SOC_E_NONE;

    (void)soc_intr_enable(unit, IRQ_DESC_DONE(chan));

    return rv;
}

/*
 * Function:
 *      cmice_dma_chan_desc_done
 * Purpose:
 *      Clears desc done on the CMICE DMA channel for the specified SOC unit.
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
cmice_dma_chan_desc_done(int unit, int chan)
{
    int rv = SOC_E_NONE;

    soc_pci_write(unit, CMIC_DMA_STAT, DS_DESC_DONE_CLR(chan));

    /* Flush posted writes from PCI bridge */
    (void)soc_pci_read(unit, CMIC_DMA_STAT);

    return rv;
}

/*
 * Function:
 *      cmice_dma_chan_chain_done
 * Purpose:
 *      on the CMICE DMA channel for the specified SOC unit.
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
cmice_dma_chan_chain_done(int unit, int chan, int mitigation)
{
    int rv = SOC_E_NONE;

    if (mitigation) {
        (void)soc_intr_disable(unit,
                               IRQ_DESC_DONE(chan)
                               | IRQ_CHAIN_DONE(chan));
    } else {
        (void)soc_intr_disable(unit, IRQ_CHAIN_DONE(chan));
    }
    /* Enable DMA */
    soc_pci_write(unit, CMIC_DMA_STAT, DS_DMA_EN_CLR(chan));

    /* Clear the descriptor done stat bit */
    soc_pci_write(unit, CMIC_DMA_STAT, DS_DESC_DONE_CLR(chan));

    /* Flush posted writes from PCI bridge */
    (void)soc_pci_read(unit, CMIC_DMA_STAT);

    return rv;
}

/*
 * Function:
 *      cmice_dma_max_rx_channels_get
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
cmice_dma_max_rx_channels_get(int unit, uint32 *max_rx_channels)
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
 *      soc_cmice_dma_drv_init
 * Purpose:
 *      Initialize the CMICE DMA driver for the specified SOC unit.
 *      Assign function pointers for SOC DMA driver interface.
 * Parameters:
 *      unit - SOC unit #
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */
int
soc_cmice_dma_drv_init(int unit, soc_cmic_dma_drv_t *drv)
{
    int rv = SOC_E_NONE;

    drv->init                       = cmice_dma_init;
    drv->ctrl_init                  = cmice_dma_ctrl_init;
    drv->chan_config                = cmice_dma_chan_config;
    drv->chan_dv_start              = cmice_dma_chan_dv_start;
    drv->chan_start                 = cmice_dma_chan_start;
    drv->chan_poll                  = cmice_dma_chan_poll;
    drv->chan_abort                 = cmice_dma_chan_abort;
    drv->chan_tx_purge              = cmice_dma_chan_tx_purge;
    drv->chan_desc_done_intr_enable = cmice_dma_chan_desc_done_intr_enable;
    drv->chan_desc_done             = cmice_dma_chan_desc_done;
    drv->chan_chain_done            = cmice_dma_chan_chain_done;
    drv->chan_reload_done           = NULL;
    drv->chan_counter_get           = NULL;
    drv->chan_counter_clear         = NULL;
    drv->chan_cos_ctrl_get          = NULL;
    drv->chan_cos_ctrl_set          = NULL;
    drv->chan_cos_ctrl_queue_get    = NULL;
    drv->chan_cos_ctrl_reg_addr_get = NULL;
    drv->chan_status_get            = NULL;
    drv->chan_status_clear          = NULL;
    drv->chan_halt_get              = NULL;
    drv->chan_ctrl_reg_get          = NULL;
    drv->chan_ctrl_reg_set          = NULL;
    drv->chan_rxbuf_threshold_cfg   = NULL;
    drv->cmc_rxbuf_threshold_cfg    = NULL;
    drv->cmc_counter_get            = NULL;
    drv->cmc_counter_clear          = NULL;
    drv->loopback_config            = NULL;
    drv->masks_get                  = NULL;
    drv->header_size_get            = NULL;
    drv->max_rx_channels_get        = cmice_dma_max_rx_channels_get;
    drv->pci_timeout_handle         = NULL;

    return rv;
}

#endif /* BCM_CMIC_SUPPORT */
