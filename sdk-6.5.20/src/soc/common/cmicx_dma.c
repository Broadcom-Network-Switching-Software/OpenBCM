/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:     CMICX interface driver for SOC DMA
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
#include <soc/cmic.h>

#ifdef INCLUDE_KNET
#include <soc/knet.h>
#endif

#ifdef BCM_CMICX_SUPPORT
#include <soc/cmicx.h>
#include <soc/intr_cmicx.h>
#include <bcm_int/common/rx.h>

typedef struct pktdma_intr_data_s {
    int cmc;
    int ch;
} pktdma_intr_data_t;

static pktdma_intr_data_t _intr_data[CMIC_CMC_NUM_MAX][CMICX_N_DMA_CHAN];

#define DCB_LAST_DWORD 3
#define DCB_DONE_BIT   0x80000000

#define CH_DESC_DONE            0
#define CH_CHAIN_DONE           1
#define CH_INTR_COALESCING_INTR 2
#define CH_DESC_CONTROLLED_INTR 3

void
soc_cmicx_pktdma_desc_endian_set(int unit, int cmc, int channel, int big_endian)
{
    uint32 val = 0;

    val = soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, channel));
    val &= ~PKTDMA_DESC_BIG_ENDIAN;
    if (big_endian) {
        val |= PKTDMA_DESC_BIG_ENDIAN;
    }
    soc_pci_write(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, channel), val);
}

void
soc_cmicx_pktdma_hdr_endian_set(int unit, int cmc, int channel, int big_endian)
{
    uint32 val = 0;

    val = soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, channel));
    val &= ~PKTDMA_HEADER_ENDIAN;
    if (big_endian) {
        val |= PKTDMA_HEADER_ENDIAN;
    }
    soc_pci_write(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, channel), val);
}

void
soc_cmicx_pktdma_data_endian_set(int unit, int cmc, int channel, int big_endian)
{
    uint32 val = 0;

    val = soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, channel));
    val &= ~PKTDMA_BIG_ENDIAN;
    if (big_endian) {
        val |= PKTDMA_BIG_ENDIAN;
    }
    soc_pci_write(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, channel), val);
}

uint32
soc_cmicx_pktdma_cos_ctrl_rx0_get(int unit, int cmc, int channel)
{
    return soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_COS_CTRL_RX_0_OFFSET(cmc, channel));
}

uint32
soc_cmicx_pktdma_cos_ctrl_rx1_get(int unit, int cmc, int channel)
{
    return soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_COS_CTRL_RX_1_OFFSET(cmc, channel));
}

uint32
soc_cmicx_pktdma_cos_ctrl_rx0_reg_addr_get(int unit, int cmc, int channel)
{
    return CMIC_CMCx_PKTDMA_CHy_COS_CTRL_RX_0_OFFSET(cmc, channel);
}

uint32
soc_cmicx_pktdma_cos_ctrl_rx1_reg_addr_get(int unit, int cmc, int channel)
{
    return CMIC_CMCx_PKTDMA_CHy_COS_CTRL_RX_1_OFFSET(cmc, channel);
}

uint32
soc_cmicx_pktdma_cos_ctrl_queue_id_get(int unit, int cmc, int channel, int queue)
{
    uint32 reg_addr;
    reg_addr = (queue < 32) ?
                CMIC_CMCx_PKTDMA_CHy_COS_CTRL_RX_0_OFFSET(cmc, channel) :
                CMIC_CMCx_PKTDMA_CHy_COS_CTRL_RX_1_OFFSET(cmc, channel);

    return soc_pci_read(unit, reg_addr);
}

sal_vaddr_t
soc_cmicx_pktdma_desc_addr_get(int unit, int cmc, int channel)
{
    uint32 val1;
    sal_paddr_t paddr;

    val1 = soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_DESC_ADDR_LO_OFFSET(cmc, channel));

#if defined(COMPILER_OVERRIDE_NO_LONGLONG) || defined(__PEDANTIC__)
    paddr = (sal_paddr_t)val1;
#else
    if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) {
        paddr = (((uint64)((soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_DESC_ADDR_HI_OFFSET(cmc, channel)))
                        & ~(CMIC_PCIE_SO_OFFSET)) << 32) | val1);
    } else {
        paddr = (((uint64)((soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_DESC_ADDR_HI_OFFSET(cmc, channel)))) << 32) | val1);
    }
#endif

    return (sal_vaddr_t)soc_cm_p2l(unit, paddr);
}

uint32
soc_cmicx_pktdma_header_size_get(int unit)
{
    uint32 hdr_size = 0;

    /* The header size is an encoded value from only the first 4 bits of
     * the CMIC_TOP_EP_TO_CPU_HEADER_SIZE register. The actual header
     * size is the encoded value multiplied by 8.
     */
    hdr_size = ((soc_pci_read(unit, CMIC_TOP_EP_TO_CPU_HEADER_SIZE_OFFSET) & 0xf) * 8);

    return hdr_size;
}

/* Static Driver Functions */

/*
 * Function:
 *      cmicx_dma_get_intr_base
 * Purpose:
 *      The interrupt numbers are dependent on the cmc.
 *      This calculates the interrupt base number.
 * Parameters:
 *      cmc - cmc#
 * Returns:
 *      The appropriate interrupt base for associated cmc
 * Notes:
 */

static uint32
cmicx_dma_get_intr_base(int cmc)
{
    uint32 intr_base = 0;

    intr_base = (cmc == 0)? CMC0_CH0_DESC_DONE: CMC1_CH0_DESC_DONE;

    return intr_base;
}

/*
 * Function:
 *      cmicx_pkdma_ch_desc_done
 * Purpose:
 *      The interrupt handler for channel descriptor done
 *
 * Parameters:
 *      unit  - SOC unit #
 *      *data - pointer provided during registration
 */

static void
cmicx_pktdma_ch_desc_done(int unit, void *data)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    sdc_t *sc;
    pktdma_intr_data_t *in_d = (pktdma_intr_data_t *)data;
    int vchan = 0;
    uint32 val = 0;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel desc done cmc = %d ch = %d\n"),
                                  in_d->cmc, in_d->ch));

    val = soc_pci_read(unit, CMIC_CMCx_SHARED_IRQ_STAT0_OFFSET(in_d->cmc));

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "shared irq stat0 val in desc done cb = %x\n"), val));

    val &= (DS_CMCx_CHy_DMA_DESC_DONE(in_d->ch) | DS_CMCx_CHy_DMA_CHAIN_DONE(in_d->ch) |
                   DS_CMCx_CHy_DESC_CONTROLLED_INTR(in_d->ch));

    vchan = (CMICX_N_DMA_CHAN * in_d->cmc) + in_d->ch;
    sc = &soc->soc_channels[vchan];

    if (sc->sc_q_cnt > 0) {
        soc_dma_cmicx_done_desc(unit, vchan);
    } else {
        soc_pci_write(unit,
                      CMIC_CMCx_SHARED_IRQ_STAT_CLR0_OFFSET(in_d->cmc),
                      val | (DS_CMCx_CHy_DMA_DESC_DONE(in_d->ch)));
    }
}

/*
 * Function:
 *      cmicx_dma_pci_timeout_handle
 * Purpose:
 *      Handle PCI completion Timeout
 *
 * Parameters:
 *      unit  - SOC unit #
 *      cmc
 *      ch      DMAchannel
         *sc      DMA Control
 */
static int
cmicx_dma_pci_timeout_handle(int unit, int cmc, int ch, sdc_t *sc)
{
    uint32 stat = 0;
    uint32 val = 0;
    dcb_t *dcb;
    sal_paddr_t addr;
    int rv = SOC_E_NONE;

    /* Only Handle the address decode errors */
    stat = soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_STAT_OFFSET(cmc, ch));
    if (PKTDMA_ADDR_DECODE_ERR & stat) {
        rv = SOC_E_TIMEOUT;
        LOG_ERROR(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "Address Decode Error cmc = %d, ch = %d\n"),
                                   cmc, ch));
        /* Disable DMA */
        val = soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, ch));
        val &= ~PKTDMA_ENABLE;
        soc_pci_write(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, ch), val);
        soc_pci_write(unit, CMIC_CMCx_SHARED_IRQ_STAT_CLR0_OFFSET(cmc),
                      DS_CMCx_CHy_DMA_CHAIN_DONE(ch));
        /* Check for RX channel */
        if (sc->sc_dv_active->dv_op == DV_RX) {
            dcb = SOC_DCB_IDX2PTR(unit, sc->sc_dv_active->dv_dcb,
                      sc->sc_dv_active->dv_dcnt);
            /* Set up DMA descriptor address */
            addr = soc_cm_l2p(unit, dcb);
            soc_pci_write(unit,
                          CMIC_CMCx_PKTDMA_CHy_DESC_ADDR_LO_OFFSET(cmc, ch),
                  PTR_TO_INT(addr));
            if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) {
                soc_pci_write(unit,
                              CMIC_CMCx_PKTDMA_CHy_DESC_ADDR_HI_OFFSET(cmc, ch),
                              (CMIC_PCIE_SO_OFFSET | PTR_HI_TO_INT(addr)));
            } else {
                soc_pci_write(unit,
                              CMIC_CMCx_PKTDMA_CHy_DESC_ADDR_HI_OFFSET(cmc, ch),
                              PTR_HI_TO_INT(addr));
            }
        }
        /* Start DMA - Enable Continuous Mode for this channel */
        val = soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, ch));
        val |= (PKTDMA_ENABLE);
        if (SOC_DMA_MODE(unit) == SOC_DMA_MODE_CONTINUOUS) {
            val |= (PKTDMA_CONTINUOUS_ENABLE);
        }
        soc_pci_write(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, ch), val);
    }

    return rv;
}

/*
 * Function:
 *      cmicx_pktdma_ch_chain_done
 * Purpose:
 *      The interrupt handler for channel chain done
 *
 * Parameters:
 *      unit  - SOC unit #
 *      *data - pointer provided during registration
 */

static void
cmicx_pktdma_ch_chain_done(int unit, void *data)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    sdc_t *sc;
    pktdma_intr_data_t *in_d = (pktdma_intr_data_t *)data;
    int cmc;
    int ch;
    int vchan = 0;
    uint32 val = 0;
    uint32 stat_paxb;
    int err = SOC_E_NONE;

    if (data == NULL) {
        LOG_ERROR(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit, "Error: NULL Parameter\n")));
        return;
    }

    cmc = in_d->cmc;
    ch = in_d->ch;
    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel chain done cmc = %d ch = %d\n"),
                                  cmc, ch));

    val = soc_pci_read(unit, CMIC_CMCx_SHARED_IRQ_STAT0_OFFSET(cmc));

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "shared irq stat0 val in chain done cb = %x\n"), val));

    val &= (DS_CMCx_CHy_DMA_DESC_DONE(ch) | DS_CMCx_CHy_DMA_CHAIN_DONE(ch) |
                   DS_CMCx_CHy_DESC_CONTROLLED_INTR(ch));

    vchan = (CMICX_N_DMA_CHAN * cmc) + ch;
    sc = &soc->soc_channels[vchan];

    if (SOC_IS_TRIDENT3(unit) || SOC_IS_TOMAHAWK3(unit)) {
        /* Check if PCI completion Timeout happended */
        (void)READ_PAXB_0_PAXB_INTR_STATUSr(unit, &stat_paxb);
        err = soc_reg_field_get(unit, PAXB_0_PAXB_INTR_STATUSr,
                              stat_paxb, PCIE_CMPL_TIMEOUT_INTR_STATUSf);
        if (err) {
            soc_reg_field_set(unit, PAXB_0_PAXB_INTR_STATUSr, &stat_paxb,
                              PCIE_CMPL_TIMEOUT_INTR_STATUSf, 0);
            (void)WRITE_PAXB_0_PAXB_INTR_STATUSr(unit, stat_paxb);
            /* Handle PCI completion Error */
            err = cmicx_dma_pci_timeout_handle(unit, cmc, ch, sc);
            soc->stat.pci_cmpl_timeout++;
        }
    }
    if (err == SOC_E_NONE) {
        if (sc->sc_q_cnt > 0) {
            soc_dma_done_chain(unit, vchan);
        } else {
             soc_pci_write(unit,
                          CMIC_CMCx_SHARED_IRQ_STAT_CLR0_OFFSET(cmc),
                          val | (DS_CMCx_CHy_DMA_CHAIN_DONE(ch)));
        }
    }
}

int
soc_cmicx_dma_ctrl_release_cpu_credits(int unit)
{
    uint32 val;

    if (SOC_IS_TOMAHAWK3(unit)) {
        READ_CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr(unit, &val);

        soc_reg_field_set(unit, CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr,
                          &val, WR_EP_INTF_CREDITSf, 0x1);
        soc_reg_field_set(unit, CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr,
                          &val, MAX_CREDITSf, 0x3F);

        WRITE_CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr(unit, val);

        soc_pci_write(unit, CMIC_TOP_EPINTF_RELEASE_ALL_CREDITS_OFFSET, 1);
        soc_pci_write(unit, CMIC_TOP_EPINTF_RELEASE_ALL_CREDITS_OFFSET, 0);

        READ_CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr(unit, &val);

        soc_reg_field_set(unit, CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr,
                          &val, WR_EP_INTF_CREDITSf, 0x1);
        soc_reg_field_set(unit, CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr,
                          &val, MAX_CREDITSf, 0x1);

        WRITE_CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr(unit, val);

        soc_pci_write(unit, CMIC_TOP_EPINTF_RELEASE_ALL_CREDITS_OFFSET, 1);
        soc_pci_write(unit, CMIC_TOP_EPINTF_RELEASE_ALL_CREDITS_OFFSET, 0);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      cmicx_dma_ctrl_init
 * Purpose:
 *      Initialize the CMICX DMA Control to a known good state for
 *      the specified SOC unit.
 * Parameters:
 *      unit - SOC unit #
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */

int
cmicx_dma_ctrl_init(int unit)
{
    int rv = SOC_E_NONE;
    int cmc;
    uint32 val, chan, vchan;

    for(vchan=0; vchan < SOC_DCHAN_NUM(unit); vchan++) {
        cmc = vchan / CMICX_N_DMA_CHAN;
        chan = vchan % CMICX_N_DMA_CHAN;

        val = soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan));
        val &= (PKTDMA_BIG_ENDIAN | PKTDMA_DESC_BIG_ENDIAN | PKTDMA_HEADER_ENDIAN);
        soc_pci_write(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan), val);
    }

    /*
     * This has to be done only once after both CMIC and EP blocks
     * are out of reset.
     */

#ifdef BCM_TOMAHAWK3_SUPPORT
    /* This is the SW WAR for TH3-3751. Since only 6 bits of
       iproc_cmic_to_ep_credits[5:0] are being used, so we cannot set the
       max credits to be released to EP to 64. Hence the SW WAR is to set the
       max credits to 63 first and then release to EP and set to 1 and release
       it again.
    */
    if (SOC_IS_TOMAHAWK3(unit)) {
        READ_CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr(unit, &val);

        soc_reg_field_set(unit, CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr,
                          &val, WR_EP_INTF_CREDITSf, 0x1);
        soc_reg_field_set(unit, CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr,
                          &val, MAX_CREDITSf, 0x3F);

        WRITE_CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr(unit, val);

        soc_pci_write(unit, CMIC_TOP_EPINTF_RELEASE_ALL_CREDITS_OFFSET, 1);
        soc_pci_write(unit, CMIC_TOP_EPINTF_RELEASE_ALL_CREDITS_OFFSET, 0);

        READ_CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr(unit, &val);

        soc_reg_field_set(unit, CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr,
                          &val, WR_EP_INTF_CREDITSf, 0x1);
        soc_reg_field_set(unit, CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr,
                          &val, MAX_CREDITSf, 0x1);

        WRITE_CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr(unit, val);

        soc_pci_write(unit, CMIC_TOP_EPINTF_RELEASE_ALL_CREDITS_OFFSET, 1);
        soc_pci_write(unit, CMIC_TOP_EPINTF_RELEASE_ALL_CREDITS_OFFSET, 0);
    } else
#endif
    {
#ifdef BCM_HELIX5_SUPPORT
    if(SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {

        READ_CMIC_TOP_IPINTF_INTERFACE_CREDITSr(unit, &val);
        soc_reg_field_set(unit, CMIC_TOP_IPINTF_INTERFACE_CREDITSr,
                          &val, FLUSH_IP_INTF_BUFFERf, 1);
        WRITE_CMIC_TOP_IPINTF_INTERFACE_CREDITSr(unit, val);

        READ_CMIC_TOP_IPINTF_INTERFACE_CREDITSr(unit, &val);
        soc_reg_field_set(unit, CMIC_TOP_IPINTF_INTERFACE_CREDITSr,
                          &val, FLUSH_IP_INTF_BUFFERf, 0);
        soc_reg_field_set(unit, CMIC_TOP_IPINTF_INTERFACE_CREDITSr,
                          &val, IP_INTF_CREDITSf, 0x20);
        WRITE_CMIC_TOP_IPINTF_INTERFACE_CREDITSr(unit, val);

        READ_CMIC_TOP_IPINTF_INTERFACE_CREDITSr(unit, &val);
        soc_reg_field_set(unit, CMIC_TOP_IPINTF_INTERFACE_CREDITSr,
                          &val, FLUSH_IP_INTF_BUFFERf, 0);
        soc_reg_field_set(unit, CMIC_TOP_IPINTF_INTERFACE_CREDITSr,
                          &val, IP_INTF_CREDITSf, 0x20);
        soc_reg_field_set(unit, CMIC_TOP_IPINTF_INTERFACE_CREDITSr,
                          &val, WR_IP_INTF_CREDITSf, 1);
        WRITE_CMIC_TOP_IPINTF_INTERFACE_CREDITSr(unit, val);
    }
#endif
        READ_CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr(unit, &val);

        soc_reg_field_set(unit, CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr,
                          &val, MAX_CREDITSf, 0x20);

        WRITE_CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr(unit, val);
        soc_reg_field_set(unit, CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr,
                          &val, WR_EP_INTF_CREDITSf, 1);

        WRITE_CMIC_TOP_EPINTF_MAX_INTERFACE_CREDITSr(unit, val);

        soc_pci_write(unit, CMIC_TOP_EPINTF_RELEASE_ALL_CREDITS_OFFSET, 0);
        soc_pci_write(unit, CMIC_TOP_EPINTF_RELEASE_ALL_CREDITS_OFFSET, 1);
    }
    /* Set IP_INTERFACE_HEADER_ENDIANESS=1 */
    val = soc_pci_read(unit, CMIC_TOP_CONFIG_OFFSET);
    val |= 0x80;
    soc_pci_write(unit, CMIC_TOP_CONFIG_OFFSET, val);
    return rv;
}

/*
 * Function:
 *      cmicx_dma_chan_config
 * Purpose:
 *      Initialize the CMICX DMA channel for the specified SOC unit.
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
cmicx_dma_chan_config(int unit, int vchan, dvt_t type, uint32 flags)
{
    int rv = SOC_E_NONE;
    soc_control_t *soc = SOC_CONTROL(unit);
    sdc_t *sc = &soc->soc_channels[vchan];
    uint32 bits = 0;
    int cmc = vchan / CMICX_N_DMA_CHAN;
    int chan = vchan % CMICX_N_DMA_CHAN;
    uint32 cr;
    uint32 intr_base = 0;

    int f_interrupt = (flags & SOC_DMA_F_POLL) == 0;
    int f_default = (flags & SOC_DMA_F_DEFAULT) != 0;
    int init_hw = TRUE;

    /* Set channel to Continuous Mode */
    if (soc_property_get(unit, spn_PDMA_CONTINUOUS_MODE_ENABLE, 0)) {
        SOC_DMA_MODE(unit) = SOC_DMA_MODE_CONTINUOUS;
    } else {
        SOC_DMA_MODE(unit) = SOC_DMA_MODE_CHAINED;
    }

    /* Set packet DMA descriptor prefetch mode */
    if (soc_property_get(unit, spn_PDMA_DESCRIPTOR_PREFETCH_ENABLE, 0)) {
        SOC_DMA_DESC_PREFETCH(unit) = TRUE;
    } else {
        SOC_DMA_DESC_PREFETCH(unit) = FALSE;
    }

    /* Initial state of channel */
    sc->sc_flags = 0;                   /* Clear flags to start */

#ifdef INCLUDE_KNET
    if (SOC_KNET_MODE(unit) && SOC_WARM_BOOT(unit) &&
        soc_property_get(unit, spn_WARMBOOT_KNET_SHUTDOWN_MODE, 0)) {
        init_hw = FALSE;
    }
#endif

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel config cmc = %d channel= %d type = %d\n"),
                                   cmc, chan, type));

    intr_base = cmicx_dma_get_intr_base(cmc);

    if (init_hw) {
        soc_cmic_intr_disable(unit, intr_base + (4 * chan) + CH_DESC_DONE);
        soc_cmic_intr_disable(unit, intr_base + (4 * chan) + CH_CHAIN_DONE);
        soc_cmic_intr_disable(unit, intr_base + (4 * chan) + CH_DESC_CONTROLLED_INTR);

        cr = soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc,chan));
        cr &= ~PKTDMA_ENABLE;
        soc_pci_write(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan), cr);

        soc_pci_write(unit, CMIC_CMCx_SHARED_IRQ_STAT_CLR0_OFFSET(cmc),
                      DS_CMCx_CHy_DMA_DESC_DONE(chan) | DS_CMCx_CHy_DESC_CONTROLLED_INTR(chan));
    }

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
        if (SOC_DMA_MODE(unit) == SOC_DMA_MODE_CONTINUOUS) {
            soc_cmic_intr_enable(unit, intr_base + (4 * chan) + CH_DESC_CONTROLLED_INTR);
        } else {
            soc_cmic_intr_enable(unit, intr_base + (4 * chan) + CH_DESC_DONE);
            soc_cmic_intr_enable(unit, intr_base + (4 * chan) + CH_CHAIN_DONE);
        }
    }
    sc->sc_type = type;

    if (init_hw) {
        cr = soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan));

        /* Clear everything except Endianess */
        cr &= (PKTDMA_BIG_ENDIAN | PKTDMA_DESC_BIG_ENDIAN | PKTDMA_HEADER_ENDIAN);
        cr |= bits;

        if (SOC_DMA_MODE(unit) == SOC_DMA_MODE_CONTINUOUS) {
            cr |= PKTDMA_DESC_DONE_INTR_MODE;
        }

        if (SOC_DMA_DESC_PREFETCH(unit)) {
            cr |=  PKTDMA_CONTIGUOUS_DESCRIPTORS;
        }

        soc_pci_write(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan), cr);
    }

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "completed channel config\n")));

    return rv;
}

/*
 * Function:
 *      cmicx_dma_chan_start
 * Purpose:
 *      Start the CMICd DMA channel for the specified SOC unit.
 * Parameters:
 *      unit - SOC unit #
 *      sc   - SOC channel data structure pointer
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */
static int
cmicx_dma_chan_start(int unit, sdc_t *sc)
{
    int rv = SOC_E_NONE;
    dv_t *dv;
    int cmc = sc->sc_channel / CMICX_N_DMA_CHAN;
    int chan = sc->sc_channel % CMICX_N_DMA_CHAN;
    uint32 val;
    sal_paddr_t addr;
    uint32 intr_base = 0;

    if ((dv = sc->sc_dv_active = sc->sc_q) == NULL) {
        sc->sc_q_tail = NULL;
        return rv;
    }

#ifdef INCLUDE_KNET
    if (SOC_KNET_MODE(unit)) {
        return rv;
    }
#endif

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "Starting channel %d\n"),
                       sc->sc_channel));

    /* Set up DMA descriptor address */
    addr = soc_cm_l2p(unit, sc->sc_q->dv_dcb);

    soc_pci_write(unit, CMIC_CMCx_PKTDMA_CHy_DESC_ADDR_LO_OFFSET(cmc, chan),
                  PTR_TO_INT(addr));

    if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) {
        soc_pci_write(unit, CMIC_CMCx_PKTDMA_CHy_DESC_ADDR_HI_OFFSET(cmc, chan),
                  (CMIC_PCIE_SO_OFFSET | PTR_HI_TO_INT(addr)));
    } else {
        soc_pci_write(unit, CMIC_CMCx_PKTDMA_CHy_DESC_ADDR_HI_OFFSET(cmc, chan), PTR_HI_TO_INT(addr));
    }

    if (SOC_DMA_MODE(unit) != SOC_DMA_MODE_CONTINUOUS) {
        if (sc->sc_flags & SOC_DMA_F_CLR_CHN_DONE) {
            val = soc_pci_read(unit,
                               CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan));
            val &= ~PKTDMA_ENABLE;
            soc_pci_write(unit,
                          CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan),
                          val);
        } else {
            sc->sc_flags |= SOC_DMA_F_CLR_CHN_DONE;
        }
    }

    /* Start DMA - Enable Continuous Mode for this channel */
    val = soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan));
    val |= (PKTDMA_ENABLE);

    if (SOC_DMA_MODE(unit) == SOC_DMA_MODE_CONTINUOUS) {
        val |= (PKTDMA_CONTINUOUS_ENABLE);
    }

    soc_pci_write(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan), val);

    SDK_CONFIG_MEMORY_BARRIER;

    if (!(sc->sc_flags & SOC_DMA_F_POLL)) {
        intr_base = cmicx_dma_get_intr_base(cmc);
        if (SOC_DMA_MODE(unit) == SOC_DMA_MODE_CONTINUOUS) {
            soc_cmic_intr_enable(unit, intr_base + (4 * chan) + CH_DESC_CONTROLLED_INTR);
        } else {
            soc_cmic_intr_enable(unit, intr_base + (4 * chan) + CH_CHAIN_DONE);
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "completed channel start\n")));

    return rv;
}

/*
 * Function:
 *      cmicx_dma_chan_poll
 * Purpose:
 *      Poll the CMICX DMA channel for the specified SOC unit.
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
cmicx_dma_chan_poll(int unit, int vchan, soc_dma_poll_type_t type, int * detected)
{
    int rv = SOC_E_NONE;
    int cmc = vchan / CMICX_N_DMA_CHAN;
    int chan = vchan % CMICX_N_DMA_CHAN;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel poll cmc = %d channel = %d\n"),
                                   cmc, chan));

    switch (type) {
    case SOC_DMA_POLL_DESC_DONE:
        *detected = (soc_pci_read(unit, CMIC_CMCx_SHARED_IRQ_STAT0_OFFSET(cmc))
                     & DS_CMCx_CHy_DMA_DESC_DONE(chan));
        break;
    case SOC_DMA_POLL_CHAIN_DONE:
        *detected = (soc_pci_read(unit, CMIC_CMCx_SHARED_IRQ_STAT0_OFFSET(cmc))
                     & DS_CMCx_CHy_DMA_CHAIN_DONE(chan));
        break;
    default:
        break;
    }

    return rv;
}

/*
 * Function:
 *      cmicx_dma_chan_abort
 * Purpose:
 *      Initialize the CMICX DMA channel for the specified SOC unit.
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
cmicx_dma_chan_abort(int unit, int vchan)
{
    int rv = SOC_E_NONE;
    int cmc = vchan / CMICX_N_DMA_CHAN;
    int chan = vchan % CMICX_N_DMA_CHAN;
    int to;
    uint32 ctrl, val;
    soc_control_t *soc = SOC_CONTROL(unit);
    sdc_t *sc = &soc->soc_channels[chan];

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel abort cmc = %d channel = %d\n"),
                                   cmc, chan));

    if ((soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_STAT_OFFSET(cmc, chan))
         & PKTDMA_CH_IS_ACTIVE) != 0) {
        ctrl = soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan));
        ctrl |= PKTDMA_ENABLE;
        soc_pci_write(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan), ctrl);
        soc_pci_write(unit,
                      CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan),
                      ctrl | PKTDMA_ABORT);
        SDK_CONFIG_MEMORY_BARRIER;

        to = soc_property_get(unit, spn_PDMA_TIMEOUT_USEC, 1000000);
        while ((to >= 0) &&
               ((soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_STAT_OFFSET(cmc, chan))
                 & PKTDMA_CH_IS_ACTIVE) != 0)) {
            sal_udelay(10000);
            to -= 1000;
        }
        if ((soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_STAT_OFFSET(cmc, chan))
             & PKTDMA_CH_IS_ACTIVE) != 0) {
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
    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                (BSL_META_U(unit,
                            "soc_dma_abort_channel unit %d: "
                            "channel %d\n"),
                 unit, chan));

    ctrl = soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan));
    /* Clearing enable will also clear CHAIN_DONE */
    ctrl &= ~(PKTDMA_ENABLE | PKTDMA_ABORT);
    soc_pci_write(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan), ctrl);

    val = soc_pci_read(unit, CMIC_CMCx_SHARED_IRQ_STAT0_OFFSET(cmc));

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "shared irq stat0 val = %x\n"), val));

    val &= (DS_CMCx_CHy_DMA_DESC_DONE(chan) | DS_CMCx_CHy_DMA_CHAIN_DONE(chan) |
                   DS_CMCx_CHy_DESC_CONTROLLED_INTR(chan));

    soc_pci_write(unit,
                  CMIC_CMCx_SHARED_IRQ_STAT_CLR0_OFFSET(cmc),
                  val | (DS_CMCx_CHy_DMA_DESC_DONE(chan) | DS_CMCx_CHy_DMA_CHAIN_DONE(chan) |
                   DS_CMCx_CHy_DESC_CONTROLLED_INTR(chan)));

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "In channel abort val of ctrl reg = %x\n"),
                                   ctrl));

    SDK_CONFIG_MEMORY_BARRIER;

    /* Mark channel as not started */
    sc->sc_dma_started = 0;

    return rv;
}

/*
 * Function:
 *      cmicx_dma_chan_dv_start
 * Purpose:
 *      Adds DV to the ready queue for CMICX DMA for the specified SOC unit.
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
cmicx_dma_chan_dv_start(int unit, sdc_t *sc, dv_t *dv_chain)
{
    int rv = SOC_E_NONE;
    int cmc = sc->sc_channel / CMICX_N_DMA_CHAN;
    int chan = sc->sc_channel % CMICX_N_DMA_CHAN;
    dcb_t *dcb;
    sal_paddr_t reg_val = 0;

    if (dv_chain == sc->sc_q) {
        DV_STATE(dv_chain) = DV_S_FILLED;
        return rv;
    }

    dv_chain->dv_next = NULL;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel dv start cmc = %d channel = %d\n"),
                                   cmc, chan));

#ifdef INCLUDE_KNET
    if (SOC_KNET_MODE(unit)) {
        /* KNET handling of DV start */
        if (sc->sc_q_cnt != 0) {
            sc->sc_q_tail->dv_next = dv_chain;
        } else {
            sc->sc_q = dv_chain;
        }
        sc->sc_q_tail = dv_chain;
        sc->sc_q_cnt++;
        if (sc->sc_dv_active == NULL) {
            if (sc->sc_dma_started) {
                sc->sc_dv_active = sc->sc_q;
            } else {
                /* Start DMA if channel not active */
                rv = cmicx_dma_chan_start(unit, sc);
                sc->sc_dma_started = 1;
            }
        }
        return rv;
    }
#endif

    if (sc->sc_q_cnt != 0) {
        sc->sc_q_tail->dv_next = dv_chain;
        if (SOC_DMA_MODE(unit) == SOC_DMA_MODE_CONTINUOUS) {
                /* Set the reload addr of tail
                 * to the address of the appended chain
                 */
                dcb = SOC_DCB_IDX2PTR(unit, sc->sc_q_tail->dv_dcb,
                                      sc->sc_q_tail->dv_vcnt - 1);
                SOC_DCB_ADDR_SET(dv_chain->dv_unit, dcb,
                                 (sal_vaddr_t)dv_chain->dv_dcb);
        }
    } else {
        sc->sc_q = dv_chain;
    }
    sc->sc_q_tail = dv_chain;
    sc->sc_q_cnt++;
    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel dv start sc_q_cnt = %d\n"),
                                   sc->sc_q_cnt));

    if (SOC_DMA_MODE(unit) == SOC_DMA_MODE_CONTINUOUS) {
        /* Set the HW register for the new DESC_HALT_ADDR for CMICX
         * Need logical to physical mapping for addr
         * If DMA is currently halted, writing the new rld addr starts it again
         */
        reg_val = soc_cm_l2p(unit,
                            (void*)SOC_DCB_IDX2PTR(unit, dv_chain->dv_dcb,
                            (dv_chain->dv_vcnt - 1)));

        soc_pci_write(unit,
                      CMIC_CMCx_PKTDMA_CHy_DESC_HALT_ADDR_LO_OFFSET(cmc, chan),
                      PTR_TO_INT(reg_val));

        if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) {
            soc_pci_write(unit,
                      CMIC_CMCx_PKTDMA_CHy_DESC_HALT_ADDR_HI_OFFSET(cmc, chan),
                      CMIC_PCIE_SO_OFFSET | PTR_HI_TO_INT(reg_val));
        } else {
            soc_pci_write(unit,
                      CMIC_CMCx_PKTDMA_CHy_DESC_HALT_ADDR_HI_OFFSET(cmc, chan), PTR_HI_TO_INT(reg_val));
        }
    }

    SDK_CONFIG_MEMORY_BARRIER;

    /* Start DMA if channel not active */
    if (SOC_DMA_MODE(unit) == SOC_DMA_MODE_CONTINUOUS) {
        if ((sc->sc_dv_active == NULL) && (!sc->sc_dma_started)) {
            rv = cmicx_dma_chan_start(unit, sc);
            sc->sc_dma_started = 1;
        }
    }

    if (SOC_DMA_MODE(unit) != SOC_DMA_MODE_CONTINUOUS) {
        if (sc->sc_q_cnt == 1) {
            rv = cmicx_dma_chan_start(unit, sc);
            sc->sc_dma_started = 1;
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "end of channel dv start\n")));

    return rv;
}

/*
 * Function:
 *      cmicx_dma_chan_chain_done
 * Purpose:
 *      on the CMICX DMA channel for the specified SOC unit.
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
cmicx_dma_chan_chain_done(int unit, int vchan, int mitigation)
{
    int rv = SOC_E_NONE;
    int cmc = vchan / CMICX_N_DMA_CHAN;
    int chan = vchan % CMICX_N_DMA_CHAN;
    uint32 val;
    uint32 intr_base = 0;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel chain done cmc = %d channel = %d "
                                  "mitigation = %d\n"),
                                   cmc, chan, mitigation));

    intr_base = cmicx_dma_get_intr_base(cmc);

    if (mitigation) {
        soc_cmic_intr_disable(unit, intr_base + (4 * chan) + CH_DESC_DONE);
        soc_cmic_intr_disable(unit, intr_base + (4 * chan) + CH_CHAIN_DONE);
    } else {
        soc_cmic_intr_disable(unit, intr_base + (4 * chan) + CH_CHAIN_DONE);
    }

    /* Disable DMA */
    val = soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan));
    val &= ~PKTDMA_ENABLE;
    soc_pci_write(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan), val);

    soc_pci_write(unit,
                  CMIC_CMCx_SHARED_IRQ_STAT_CLR0_OFFSET(cmc),
                  DS_CMCx_CHy_DMA_CHAIN_DONE(chan));

    /* Flush posted writes from PCI bridge */
    (void)soc_pci_read(unit, CMIC_CMCx_SHARED_IRQ_STAT_CLR0_OFFSET(cmc));

    val = soc_pci_read(unit, CMIC_CMCx_SHARED_IRQ_STAT0_OFFSET(cmc));

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "shared irq stat0 val at chain done = %x\n"), val));

    return rv;
}

/*
 * Function:
 *      cmicx_dma_chan_desc_done
 * Purpose:
 *      Clears desc done on the CMICX DMA channel for the specified SOC unit.
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
cmicx_dma_chan_desc_done(int unit, int vchan)
{
    int rv = SOC_E_NONE;
    int cmc = vchan / CMICX_N_DMA_CHAN;
    int chan = vchan % CMICX_N_DMA_CHAN;
    uint32 val;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel desc done cmc = %d channel = %d\n"),
                                   cmc, chan));

    /*
     * Older CMIC devices required a read/write/modify to clear done
     * interrupts. That does not work here for continuous DMA.  Need only
     * to set the bit for the corresponding interrupt to clear.
     */
    val = soc_pci_read(unit, CMIC_CMCx_SHARED_IRQ_STAT0_OFFSET(cmc));

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "shared irq stat0 val = %x\n"), val));

    val &= (DS_CMCx_CHy_DMA_DESC_DONE(chan) | DS_CMCx_CHy_DMA_CHAIN_DONE(chan) |
                   DS_CMCx_CHy_DESC_CONTROLLED_INTR(chan));

    if (SOC_DMA_MODE(unit) == SOC_DMA_MODE_CONTINUOUS) {
        soc_pci_write(unit,
                      CMIC_CMCx_SHARED_IRQ_STAT_CLR0_OFFSET(cmc),
                      (DS_CMCx_CHy_DESC_CONTROLLED_INTR(chan)));
    } else {
        soc_pci_write(unit,
                      CMIC_CMCx_SHARED_IRQ_STAT_CLR0_OFFSET(cmc),
                      (DS_CMCx_CHy_DMA_DESC_DONE(chan)));
    }

    /* Flush posted writes from PCI bridge */
    (void)soc_pci_read(unit, CMIC_CMCx_SHARED_IRQ_STAT_CLR0_OFFSET(cmc));

    return rv;
}

/*
 * Function:
 *      cmicx_dma_chan_desc_done_intr_enable
 * Purpose:
 *      Clears desc done on the CMICX DMA channel for the specified SOC unit.
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
cmicx_dma_chan_desc_done_intr_enable(int unit, int vchan)
{
    int rv = SOC_E_NONE;
    int cmc = vchan / CMICX_N_DMA_CHAN;
    int chan = vchan % CMICX_N_DMA_CHAN;
    uint32 intr_base = 0;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel desc done intr enable cmc = %d channel = %d\n"),
                                   cmc, chan));

    intr_base = cmicx_dma_get_intr_base(cmc);

    if (SOC_DMA_MODE(unit) == SOC_DMA_MODE_CONTINUOUS) {
        soc_cmic_intr_enable(unit, intr_base + (4 * chan) + CH_DESC_CONTROLLED_INTR);
    } else {
        soc_cmic_intr_enable(unit, intr_base + (4 * chan) + CH_DESC_DONE);
    }

    return rv;
}

/*
 * Function:
 *      cmicx_dma_chan_reload_done
 * Purpose:
 *      Detect for reload descriptor completed
 * Parameters:
 *      unit - SOC unit #
 *      sc   - SOC channel data structure pointer
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */
static int
cmicx_dma_chan_reload_done(int unit, int vchan, dcb_t * dcb, int *rld_done)
{
    int rv = SOC_E_NONE;
    uint32 *rld_dcb;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel reload done\n")));

    *rld_done = 0; /* Default to not reload */

    /* There is no pkt data to process for a reload descriptor */
    if(SOC_DCB_RELOAD_GET(unit, dcb)) {
        /* HW sets bit 31 in the dcb's last dword to indicate done */
        rld_dcb = (uint32 *)dcb;
        if (rld_dcb[DCB_LAST_DWORD] & DCB_DONE_BIT) {
            *rld_done = 1;
        }
    }

    return rv;
}

/*
 * Function:
 *      cmicx_dma_chan_counter_get
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
cmicx_dma_chan_counter_get(int unit, int vchan, uint32 *tx_pkt, uint32 *rx_pkt)
{
    int rv = SOC_E_NONE;
    int cmc = vchan / CMICX_N_DMA_CHAN;
    int chan = vchan % CMICX_N_DMA_CHAN;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel counter get\n")));

    *tx_pkt = soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_PKT_COUNT_TXPKT_OFFSET(cmc, chan));
    *rx_pkt = soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_PKT_COUNT_RXPKT_OFFSET(cmc, chan));
#if 0
    /* keep this code ready and handy, just in case somebody needs it */
    *rx_pkt_drop = soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_PKT_COUNT_RXPKT_DROP_OFFSET(cmc, chan));
#endif
    return rv;
}

/*
 * Function:
 *      cmicx_dma_chan_counter_clear
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
cmicx_dma_chan_counter_clear(int unit, int vchan, uint32 mask)
{
    int rv = SOC_E_NONE;
    int cmc = vchan / CMICX_N_DMA_CHAN;
    int chan = vchan % CMICX_N_DMA_CHAN;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel counter clear\n")));

    if (mask & 0x1) {
        soc_pci_write(unit, CMIC_CMCx_PKTDMA_CHy_PKT_COUNT_TXPKT_OFFSET(cmc, chan), 0);
    }

    if (mask & 0x2) {
        soc_pci_write(unit, CMIC_CMCx_PKTDMA_CHy_PKT_COUNT_RXPKT_OFFSET(cmc, chan), 0);
    }

    /* Keep this code ready just in case apps start to use this feature of rx pkt drops */
    if (mask & 0x4) {
        soc_pci_write(unit, CMIC_CMCx_PKTDMA_CHy_PKT_COUNT_RXPKT_DROP_OFFSET(cmc, chan), 0);
    }

    return rv;
}

/*
 * Function:
 *      cmicx_dma_cmc_counter_get
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
cmicx_dma_cmc_counter_get(int unit, int cmc, uint32 *tx_pkt, uint32 *rx_pkt)
{
    int rv = SOC_E_NONE;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "cmc counter get\n")));

    *tx_pkt = soc_pci_read(unit, CMIC_CMCx_SHARED_PKT_COUNT_TXPKT_OFFSET(cmc));
    *rx_pkt = soc_pci_read(unit, CMIC_CMCx_SHARED_PKT_COUNT_RXPKT_OFFSET(cmc));

    return rv;
}

/*
 * Function:
 *      cmicx_dma_cmc_counter_clear
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
cmicx_dma_cmc_counter_clear(int unit, int cmc, uint32 mask)
{
    int rv = SOC_E_NONE;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "cmc counter clear\n")));

    if (mask & 0x1) {
        soc_pci_write(unit, CMIC_CMCx_SHARED_PKT_COUNT_TXPKT_OFFSET(cmc), 0);
    }

    if (mask & 0x2) {
        soc_pci_write(unit, CMIC_CMCx_SHARED_PKT_COUNT_RXPKT_OFFSET(cmc), 0);
    }

    return rv;
}

/*
 * Function:
 *      cmicx_loopback_config
 * Purpose:
 *      Set/reset cmicx in loopback mode
 * Parameters:
 *      unit - SOC unit #
 *      cfg  - to set/reset
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */

int
cmicx_loopback_config(int unit, uint32 cfg)
{
    int rv = SOC_E_NONE;
    uint32 val = 0;

    val = soc_pci_read(unit, CMIC_TOP_CONFIG_OFFSET);
    val &= 0x3f;

    if (cfg & 0x1) {
        val |= (1 << 1);
        soc_pci_write(unit, CMIC_TOP_CONFIG_OFFSET, val);
    } else {
        val &= ~(1 << 1);
        soc_pci_write(unit, CMIC_TOP_CONFIG_OFFSET, val);
    }

    return rv;
}

/*
 * Function:
 *      cmicx_dma_header_size_get
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

static int
cmicx_dma_header_size_get(int unit, uint32 *hdr_size)
{
    int rv = SOC_E_NONE;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "cmicx dma get header size\n")));

    *hdr_size = ((soc_pci_read(unit, CMIC_TOP_EP_TO_CPU_HEADER_SIZE_OFFSET) & 0xf) * 8);

    return rv;
}

/*
 * Function:
 *      cmicx_dma_max_rx_channels_get
 * Purpose:
 *      obtain maximum number of dma channels that can be used for BCM RX
 * Parameters:
 *      unit - SOC unit #
 *      max_rx_channels - maximum rx channels that can be used
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error
 */

int
cmicx_dma_max_rx_channels_get(int unit, uint32 *max_rx_channels)
{
    int rv = SOC_E_NONE;

    /*
     * Only CMC0 is used for BCM RX hence we can have
     * CMICX_N_DMA_CHAN number of maximum rx channels
     */

    *max_rx_channels = CMICX_N_DMA_CHAN;

    return rv;
}

/*
 * Function:
 *      cmicx_dma_chan_cos_ctrl_get
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
cmicx_dma_chan_cos_ctrl_get(int unit, int vchan, uint32 cfg, uint32 *cos_ctrl)
{
    int rv = SOC_E_NONE;
    int cmc = vchan / CMICX_N_DMA_CHAN;
    int chan = vchan % CMICX_N_DMA_CHAN;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel cos ctrl get\n")));

    if (cfg & 0x1) {
        *cos_ctrl = soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_COS_CTRL_RX_0_OFFSET(cmc, chan));
    } else if (cfg & 0x2) {
        *cos_ctrl = soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_COS_CTRL_RX_1_OFFSET(cmc, chan));
    }

    return rv;
}

/*
 * Function:
 *      cmicx_dma_chan_cos_ctrl_set
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
cmicx_dma_chan_cos_ctrl_set(int unit, int vchan, uint32 cfg, uint32 cos_ctrl)
{
    int rv = SOC_E_NONE;
    int cmc = vchan / CMICX_N_DMA_CHAN;
    int chan = vchan % CMICX_N_DMA_CHAN;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel cos ctrl set\n")));

    if (cfg & 0x1) {
        soc_pci_write(unit,
                      CMIC_CMCx_PKTDMA_CHy_COS_CTRL_RX_0_OFFSET(cmc, chan),
                      cos_ctrl);
    } else if (cfg & 0x2) {
        soc_pci_write(unit,
                      CMIC_CMCx_PKTDMA_CHy_COS_CTRL_RX_1_OFFSET(cmc, chan),
                      cos_ctrl);
    }

    return rv;
}

/*
 * Function:
 *      cmicx_dma_chan_cos_ctrl_queue_get
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
cmicx_dma_chan_cos_ctrl_queue_get(int unit, int cmc, int chan,
                                  int queue, uint32 *cos_ctrl)
{
    int rv = SOC_E_NONE;
    uint32 reg_addr;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel cos ctrl queue get\n")));

    reg_addr = (queue < 32) ?
                CMIC_CMCx_PKTDMA_CHy_COS_CTRL_RX_0_OFFSET(cmc, chan) :
                CMIC_CMCx_PKTDMA_CHy_COS_CTRL_RX_1_OFFSET(cmc, chan);

    *cos_ctrl = soc_pci_read(unit, reg_addr);

    return rv;
}

/*
 * Function:
 *      cmicx_dma_chan_cos_ctrl_reg_addr_get
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

int
cmicx_dma_chan_cos_ctrl_reg_addr_get(int unit, int cmc, int chan,
                                     int queue, uint32 *reg_addr)
{
    int rv = SOC_E_NONE;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel cos ctrl reg addr get\n")));

    *reg_addr = (queue < 32) ?
                CMIC_CMCx_PKTDMA_CHy_COS_CTRL_RX_0_OFFSET(cmc, chan) :
                CMIC_CMCx_PKTDMA_CHy_COS_CTRL_RX_1_OFFSET(cmc, chan);

    return rv;
}

/*
 * Function:
 *      cmicx_dma_chan_status_get
 * Purpose:
 *      get channel status
 * Parameters:
 *      unit   - SOC unit #
 *      vchan  - channel #
 *      status - channel status
 *
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */

int
cmicx_dma_chan_status_get(int unit, int vchan, uint32 mask, int *status)
{
    int rv = SOC_E_NONE;
    int cmc = vchan / CMICX_N_DMA_CHAN;
    int chan = vchan % CMICX_N_DMA_CHAN;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel status get\n")));

    *status = soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_STAT_OFFSET(cmc, chan));

    return rv;
}

/*
 * Function:
 *      cmicx_dma_chan_halt_get
 * Purpose:
 *      get channel halt status
 * Parameters:
 *      unit   - SOC unit #
 *      vchan  - channel #
 *      halt   - channel halt status
 *
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */

int
cmicx_dma_chan_halt_get(int unit, int vchan, uint32 mask, int *halt)
{
    int rv  = SOC_E_NONE;
    int cmc = vchan / CMICX_N_DMA_CHAN;
    int chan = vchan % CMICX_N_DMA_CHAN;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel halt get\n")));

    *halt = (soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_STAT_OFFSET(cmc, chan))
                            & PKTDMA_CH_IN_HALT);

    return rv;
}

/*
 * Function:
 *      cmicx_dma_chan_ctrl_reg_get
 * Purpose:
 *      get channel ctrl reg value
 * Parameters:
 *      unit  - SOC unit #
 *      vchan - channel #
 *      value - channel ctrl reg value
 *
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */

int
cmicx_dma_chan_ctrl_reg_get(int unit, int vchan, uint32 *val)
{
    int rv = SOC_E_NONE;
    int cmc = vchan / CMICX_N_DMA_CHAN;
    int chan = vchan % CMICX_N_DMA_CHAN;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel ctrl reg get\n")));

    *val = soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan));

    return rv;
}

/*
 * Function:
 *      cmicx_dma_chan_ctrl_reg_set
 * Purpose:
 *      set channel ctrl reg
 * Parameters:
 *      unit  - SOC unit #
 *      vchan - channel #
 *      value - channel ctrl reg value to set
 *
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */

int
cmicx_dma_chan_ctrl_reg_set(int unit, int vchan, uint32 val)
{
    int rv = SOC_E_NONE;
    int cmc = vchan / CMICX_N_DMA_CHAN;
    int chan = vchan % CMICX_N_DMA_CHAN;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel ctrl reg get\n")));

    soc_pci_write(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan), val);

    return rv;
}

/*
 * Function:
 *      cmicx_dma_chan_rxbuf_threshold_config
 * Purpose:
 *      appropriately set mmu back pressure per channel
 * Parameters:
 *      unit  - SOC unit #
 *      vchan - channel #
 *      value - mmu backpressure settings
 *
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */

int
cmicx_dma_chan_rxbuf_threshold_config(int unit, int vchan, uint32 val)
{
    int rv = SOC_E_NONE;
    int cmc = vchan / CMICX_N_DMA_CHAN;
    int chan = vchan % CMICX_N_DMA_CHAN;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel rxbuf threshold config\n")));

    soc_pci_write(unit, CMIC_CMCx_PKTDMA_CHy_RXBUF_THRESHOLD_CONFIG_OFFSET(cmc, chan), val);

    return rv;
}

/*
 * Function:
 *      cmicx_dma_cmc_rxbuf_threshold_config
 * Purpose:
 *      appropriately set mmu back pressure per cmc
 * Parameters:
 *      unit  - SOC unit #
 *      cmc   - cmc #
 *      value - mmu backpressure settings
 *
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */

int
cmicx_dma_cmc_rxbuf_threshold_config(int unit, int cmc, uint32 val)
{
    int rv = SOC_E_NONE;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "cmc rxbuf threshold config\n")));

    soc_pci_write(unit, CMIC_CMCx_SHARED_RXBUF_THRESHOLD_CONFIG_OFFSET(cmc), val);

    return rv;
}

/*
 * Function:
 *      cmicx_dma_masks_get
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
cmicx_dma_masks_get(int unit, uint32 *pkt_endian, uint32 *desc_endian, uint32 *header_endian)
{
    int rv = SOC_E_NONE;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "cmicx dma get masks\n")));

    *pkt_endian    = PKTDMA_BIG_ENDIAN;
    *desc_endian   = PKTDMA_DESC_BIG_ENDIAN;
    *header_endian = PKTDMA_HEADER_ENDIAN;

    return rv;
}

/*
 * Function:
 *      cmicx_dma_chan_tx_purge
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
cmicx_dma_chan_tx_purge(int unit, int vchan, dv_t *dv)
{
    int rv = SOC_E_NONE;
    int cmc = vchan / CMICX_N_DMA_CHAN;
    int chan = vchan % CMICX_N_DMA_CHAN;
    sal_usecs_t start_time;
    int diff_time;
    uint32 dma_stat;
    uint32 dma_state;
    sal_paddr_t addr;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "channel tx purge cmc = %d channel = %d\n"),
                                   cmc, chan));

    soc_pci_write(unit,
                  CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan),
                  soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan))
                  | PKTDMA_DIRECTION);

    addr = soc_cm_l2p(unit, dv->dv_dcb);

    soc_pci_write(unit,
                  CMIC_CMCx_PKTDMA_CHy_DESC_ADDR_LO_OFFSET(cmc, chan),
                  PTR_TO_INT(addr));
    if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) {
        soc_pci_write(unit,
                  CMIC_CMCx_PKTDMA_CHy_DESC_ADDR_HI_OFFSET(cmc, chan),
                  (CMIC_PCIE_SO_OFFSET | PTR_HI_TO_INT(addr)));
    } else {
        soc_pci_write(unit,
                  CMIC_CMCx_PKTDMA_CHy_DESC_ADDR_HI_OFFSET(cmc, chan),
                  PTR_HI_TO_INT(addr));
    }

    /* Start DMA */
    soc_pci_write(unit,
                  CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan),
                  soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc ,chan))
                  | PKTDMA_ENABLE);

    start_time = sal_time_usecs();
    diff_time = 0;
    dma_state = (DS_CMCx_CHy_DMA_DESC_DONE(chan) | DS_CMCx_CHy_DMA_CHAIN_DONE(chan));

    do {
        sal_udelay(SAL_BOOT_SIMULATION ? 1000 : 10);
        dma_stat = soc_pci_read(unit, CMIC_CMCx_SHARED_IRQ_STAT0_OFFSET(cmc)) &
            (DS_CMCx_CHy_DMA_DESC_DONE(chan) | DS_CMCx_CHy_DMA_CHAIN_DONE(chan));

        diff_time = SAL_USECS_SUB(sal_time_usecs(), start_time);

        if (diff_time > DMA_ABORT_TIMEOUT) { /* 10 Sec(QT)/10 msec */
            rv = SOC_E_TIMEOUT;
            break;
        } else if (diff_time < 0) {
            /* Restart in case system time changed */
            start_time = sal_time_usecs();
        }
    } while (dma_stat != dma_state);

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "disabling dma\n")));

    /* Clear CHAIN_DONE and DESC_DONE */
    soc_pci_write(unit,
                  CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan),
                  soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan))
                  & ~PKTDMA_ENABLE);

    dma_stat = soc_pci_read(unit, CMIC_CMCx_SHARED_IRQ_STAT0_OFFSET(cmc));
    soc_pci_write(unit,
                  CMIC_CMCx_SHARED_IRQ_STAT_CLR0_OFFSET(cmc),
                  (DS_CMCx_CHy_DMA_DESC_DONE(chan)));

    return rv;
}


/*
 * Function:
 *      cmicx_dma_init
 * Purpose:
 *      Initialize the CMICX DMA for the specified SOC unit.
 * Parameters:
 *      unit - SOC unit #
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */
static int
cmicx_dma_init(int unit)
{
    int cmc, ch;
    soc_cmic_intr_handler_t *handle;
    soc_cmic_intr_handler_t *hitr;
    int rv = SOC_E_NONE;
    uint32 intr_base = 0;

    LOG_VERBOSE(BSL_LS_SOC_PACKETDMA,
                      (BSL_META_U(unit,
                                  "cmicx dma init, registering interrupts\n")));

    /* Register the descriptor done interrupts */
    handle = sal_alloc(sizeof(soc_cmic_intr_handler_t)*CMIC_CMC_NUM_MAX*
                       CMICX_N_DMA_CHAN, "pktdma_desc_controlled_interrupt");
    if (handle == NULL) {
        return SOC_E_MEMORY;
    }

    hitr = handle;

    for (cmc = 0; cmc < CMIC_CMC_NUM_MAX; cmc++) {
         intr_base = cmicx_dma_get_intr_base(cmc);
         for (ch = 0; ch < CMICX_N_DMA_CHAN; ch++) {
              _intr_data[cmc][ch].cmc = cmc;
              _intr_data[cmc][ch].ch = ch;
              hitr->num = intr_base + (4 * ch) + CH_DESC_CONTROLLED_INTR;
              hitr->intr_fn = cmicx_pktdma_ch_desc_done;
              hitr->intr_data = &_intr_data[cmc][ch];
              hitr++;
         }
    }
    rv = soc_cmic_intr_register(unit, handle,
                           CMIC_CMC_NUM_MAX*CMICX_N_DMA_CHAN);
    sal_free(handle);

    /* Register the chain done interrupts */
    handle = sal_alloc(sizeof(soc_cmic_intr_handler_t)*CMIC_CMC_NUM_MAX*
                       CMICX_N_DMA_CHAN, "pktdma_chain_done_interrupt");
    if (handle == NULL) {
        return SOC_E_MEMORY;
    }

    hitr = handle;

    for (cmc = 0; cmc < CMIC_CMC_NUM_MAX; cmc++) {
         intr_base = cmicx_dma_get_intr_base(cmc);
         for (ch = 0; ch < CMICX_N_DMA_CHAN; ch++) {
              _intr_data[cmc][ch].cmc = cmc;
              _intr_data[cmc][ch].ch = ch;
              hitr->num = intr_base + (4 * ch) + CH_CHAIN_DONE;
              hitr->intr_fn = cmicx_pktdma_ch_chain_done;
              hitr->intr_data = &_intr_data[cmc][ch];
              hitr++;
         }
    }
    rv = soc_cmic_intr_register(unit, handle,
                           CMIC_CMC_NUM_MAX*CMICX_N_DMA_CHAN);
    sal_free(handle);

    /* Register the desc done interrupts */
    handle = sal_alloc(sizeof(soc_cmic_intr_handler_t)*CMIC_CMC_NUM_MAX*
                       CMICX_N_DMA_CHAN, "pktdma_descriptor_done_interrupt");
    if (handle == NULL) {
        return SOC_E_MEMORY;
    }

    hitr = handle;

    for (cmc = 0; cmc < CMIC_CMC_NUM_MAX; cmc++) {
         intr_base = cmicx_dma_get_intr_base(cmc);
         for (ch = 0; ch < CMICX_N_DMA_CHAN; ch++) {
              _intr_data[cmc][ch].cmc = cmc;
              _intr_data[cmc][ch].ch = ch;
              hitr->num = intr_base + (4 * ch) + CH_DESC_DONE;
              hitr->intr_fn = cmicx_pktdma_ch_desc_done;
              hitr->intr_data = &_intr_data[cmc][ch];
              hitr++;
         }
    }
    rv = soc_cmic_intr_register(unit, handle,
                           CMIC_CMC_NUM_MAX*CMICX_N_DMA_CHAN);
    sal_free(handle);

    return rv;
}

/* Public Assignment Function */

/*
 * Function:
 *      soc_cmicx_dma_drv_init
 * Purpose:
 *      Initialize the CMICX DMA driver for the specified SOC unit.
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
soc_cmicx_dma_drv_init(int unit, soc_cmic_dma_drv_t *drv)
{
    int rv = SOC_E_NONE;

    drv->init                         = cmicx_dma_init;
    drv->ctrl_init                    = cmicx_dma_ctrl_init;
    drv->chan_config                  = cmicx_dma_chan_config;
    drv->chan_dv_start                = cmicx_dma_chan_dv_start;
    drv->chan_start                   = cmicx_dma_chan_start;
    drv->chan_poll                    = cmicx_dma_chan_poll;
    drv->chan_abort                   = cmicx_dma_chan_abort;
    drv->chan_tx_purge                = cmicx_dma_chan_tx_purge;
    drv->chan_desc_done_intr_enable   = cmicx_dma_chan_desc_done_intr_enable;
    drv->chan_desc_done               = cmicx_dma_chan_desc_done;
    drv->chan_chain_done              = cmicx_dma_chan_chain_done;
    drv->chan_reload_done             = cmicx_dma_chan_reload_done;
    drv->chan_counter_get             = cmicx_dma_chan_counter_get;
    drv->chan_counter_clear           = cmicx_dma_chan_counter_clear;
    drv->chan_cos_ctrl_get            = cmicx_dma_chan_cos_ctrl_get;
    drv->chan_cos_ctrl_set            = cmicx_dma_chan_cos_ctrl_set;
    drv->chan_cos_ctrl_queue_get      = cmicx_dma_chan_cos_ctrl_queue_get;
    drv->chan_cos_ctrl_reg_addr_get   = cmicx_dma_chan_cos_ctrl_reg_addr_get;
    drv->chan_halt_get                = cmicx_dma_chan_halt_get;
    drv->chan_status_get              = cmicx_dma_chan_status_get;
    drv->chan_status_clear            = NULL;
    drv->chan_ctrl_reg_get            = cmicx_dma_chan_ctrl_reg_get;
    drv->chan_ctrl_reg_set            = cmicx_dma_chan_ctrl_reg_set;
    drv->chan_rxbuf_threshold_cfg     = cmicx_dma_chan_rxbuf_threshold_config;
    drv->cmc_rxbuf_threshold_cfg      = cmicx_dma_cmc_rxbuf_threshold_config;
    drv->cmc_counter_get              = cmicx_dma_cmc_counter_get;
    drv->cmc_counter_clear            = cmicx_dma_cmc_counter_clear;
    drv->loopback_config              = cmicx_loopback_config;
    drv->masks_get                    = cmicx_dma_masks_get;
    drv->header_size_get              = cmicx_dma_header_size_get;
    drv->max_rx_channels_get          = cmicx_dma_max_rx_channels_get;
    drv->pci_timeout_handle           = NULL;

    return rv;
}
#endif /* BCM_CMICX_SUPPORT */
