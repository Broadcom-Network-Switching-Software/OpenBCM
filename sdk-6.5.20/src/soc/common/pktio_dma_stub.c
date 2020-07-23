/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        pktio_dma_stub.c
 */

#include <soc/drv.h>
#include <soc/cmicx.h>

#ifdef INCLUDE_PKTIO

static int
pktio_dma_init(int unit)
{
    return SOC_E_NONE;
}

static int
pktio_dma_ctrl_init(int unit)
{
    return SOC_E_NONE;
}

static int
pktio_dma_chan_abort(int unit, int chan)
{
    return SOC_E_NONE;
}

static int
pktio_dma_chan_tx_purge(int unit, int chan, dv_t *dv)
{
    return SOC_E_NONE;
}

static int
pktio_dma_chan_config(int unit, int chan, dvt_t type, uint32 flags)
{
    return SOC_E_NONE;
}

static int
pktio_dma_chan_dv_start(int unit, sdc_t *sc, dv_t *dv)
{
    return SOC_E_NONE;
}

static int
pktio_dma_chan_start(int unit, sdc_t *sc)
{
    return SOC_E_NONE;
}

static int
pktio_dma_chan_poll(int unit, int chan, soc_dma_poll_type_t type, int *detected)
{
    return SOC_E_NONE;
}

static int
pktio_dma_chan_desc_done_intr_enable(int unit, int chan)
{
    return SOC_E_NONE;
}

static int
pktio_dma_chan_desc_done(int unit, int chan)
{
    return SOC_E_NONE;
}

static int
pktio_dma_chan_chain_done(int unit, int chan, int mitigation)
{
    return SOC_E_NONE;
}

static int
pktio_dma_chan_reload_done(int unit, int chan, dcb_t *dcb, int *rld_done)
{
    return SOC_E_NONE;
}

static int
pktio_dma_chan_counter_get(int unit, int chan, uint32 *tx_pkt, uint32 *rx_pkt)
{
    return SOC_E_NONE;
}

static int
pktio_dma_chan_counter_clear(int unit, int chan, uint32 mask)
{
    return SOC_E_NONE;
}

static int
pktio_dma_chan_cos_ctrl_get(int unit, int chan, uint32 mask, uint32 *cos_ctrl)
{
    return SOC_E_NONE;
}

static int
pktio_dma_chan_cos_ctrl_set(int unit, int chan, uint32 mask, uint32 cos_ctrl)
{
    return SOC_E_NONE;
}

static int
pktio_dma_chan_cos_ctrl_queue_get(int unit, int cmc, int chan, int queue, uint32 *cos_ctrl)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      pktio_dma_chan_cos_ctrl_reg_addr_get
 * Purpose:
 *      get cos ctrl for a channel based on queue
 * Parameters:
 *      unit     - SOC unit #
 *      cmc      - CMC #
 *      chan     - chan #
 *      queue    - cos queue
 *      reg_addr - reg addr val
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */
static int
pktio_dma_chan_cos_ctrl_reg_addr_get(int unit, int cmc, int chan, int queue, uint32 *reg_addr)
{
    *reg_addr = (queue < 32) ?
                CMIC_CMCx_PKTDMA_CHy_COS_CTRL_RX_0_OFFSET(cmc, chan) :
                CMIC_CMCx_PKTDMA_CHy_COS_CTRL_RX_1_OFFSET(cmc, chan);
    return SOC_E_NONE;
}

static int
pktio_dma_chan_status_get(int unit, int chan, uint32 mask, int *status)
{
    return SOC_E_NONE;
}

static int
pktio_dma_chan_status_clear(int unit, int chan)
{
    return SOC_E_NONE;
}

static int
pktio_dma_chan_halt_get(int unit, int chan, uint32 mask, int *active)
{
    return SOC_E_NONE;
}

static int
pktio_dma_chan_ctrl_reg_get(int unit, int chan, uint32 *val)
{
    return SOC_E_NONE;
}

static int
pktio_dma_chan_ctrl_reg_set(int unit, int chan, uint32 val)
{
    return SOC_E_NONE;
}

static int
pktio_dma_chan_rxbuf_threshold_cfg(int unit, int chan, uint32 val)
{
    return SOC_E_NONE;
}

static int
pktio_dma_cmc_rxbuf_threshold_cfg(int unit, int cmc, uint32 val)
{
    return SOC_E_NONE;
}

static int
pktio_dma_cmc_counter_get(int unit, int cmc, uint32 *tx_pkt, uint32 *rx_pkt)
{
    return SOC_E_NONE;
}

static int
pktio_dma_cmc_counter_clear(int unit, int cmc, uint32 mask)
{
    return SOC_E_NONE;
}

static int
pktio_dma_masks_get(int unit, uint32 *pkt_endian, uint32 *desc_endian, uint32 *header_endian)
{
    return SOC_E_NONE;
}

static int
pktio_dma_loopback_config(int unit, uint32 cfg)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      pktio_dma_max_rx_channels_get
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
pktio_dma_max_rx_channels_get(int unit, uint32 *max_rx_channels)
{
    /*
     * Only CMC0 is used for BCM RX hence we can have
     * CMICX_N_DMA_CHAN number of maximum rx channels
     */
    *max_rx_channels = CMICX_N_DMA_CHAN;
    return SOC_E_NONE;
}

static int
pktio_dma_pci_timeout_handle(int unit)
{
    return SOC_E_NONE;
}

static int
pktio_dma_header_size_get(int unit, uint32 *hdr_size)
{
    return SOC_E_NONE;
}


/* Public Assignment Function */

/*
 * Function:
 *      soc_pktio_dma_drv_init
 * Purpose:
 *      Initialize the PKTIO DMA driver for the specified SOC unit.
 *      Assign function pointers for SOC DMA driver interface.
 * Parameters:
 *      unit - SOC unit #
 *      drv  - pointer to the function vector list
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 *      Most of the DMA process would be done via SDKLT.
 */
int
soc_pktio_dma_drv_init(int unit, soc_cmic_dma_drv_t *drv)
{
    int rv = SOC_E_NONE;

    drv->init                         = pktio_dma_init;
    drv->ctrl_init                    = pktio_dma_ctrl_init;
    drv->chan_config                  = pktio_dma_chan_config;
    drv->chan_dv_start                = pktio_dma_chan_dv_start;
    drv->chan_start                   = pktio_dma_chan_start;
    drv->chan_poll                    = pktio_dma_chan_poll;
    drv->chan_abort                   = pktio_dma_chan_abort;
    drv->chan_tx_purge                = pktio_dma_chan_tx_purge;
    drv->chan_desc_done_intr_enable   = pktio_dma_chan_desc_done_intr_enable;
    drv->chan_desc_done               = pktio_dma_chan_desc_done;
    drv->chan_chain_done              = pktio_dma_chan_chain_done;
    drv->chan_reload_done             = pktio_dma_chan_reload_done;
    drv->chan_counter_get             = pktio_dma_chan_counter_get;
    drv->chan_counter_clear           = pktio_dma_chan_counter_clear;
    drv->chan_cos_ctrl_get            = pktio_dma_chan_cos_ctrl_get;
    drv->chan_cos_ctrl_set            = pktio_dma_chan_cos_ctrl_set;
    drv->chan_cos_ctrl_queue_get      = pktio_dma_chan_cos_ctrl_queue_get;
    drv->chan_cos_ctrl_reg_addr_get   = pktio_dma_chan_cos_ctrl_reg_addr_get;
    drv->chan_halt_get                = pktio_dma_chan_halt_get;
    drv->chan_status_get              = pktio_dma_chan_status_get;
    drv->chan_status_clear            = pktio_dma_chan_status_clear;
    drv->chan_ctrl_reg_get            = pktio_dma_chan_ctrl_reg_get;
    drv->chan_ctrl_reg_set            = pktio_dma_chan_ctrl_reg_set;
    drv->chan_rxbuf_threshold_cfg     = pktio_dma_chan_rxbuf_threshold_cfg;
    drv->cmc_rxbuf_threshold_cfg      = pktio_dma_cmc_rxbuf_threshold_cfg;
    drv->cmc_counter_get              = pktio_dma_cmc_counter_get;
    drv->cmc_counter_clear            = pktio_dma_cmc_counter_clear;
    drv->loopback_config              = pktio_dma_loopback_config;
    drv->masks_get                    = pktio_dma_masks_get;
    drv->header_size_get              = pktio_dma_header_size_get;
    drv->max_rx_channels_get          = pktio_dma_max_rx_channels_get;
    drv->pci_timeout_handle           = pktio_dma_pci_timeout_handle;
    return rv;
}

#endif /* INCLUDE_PKTIO */
