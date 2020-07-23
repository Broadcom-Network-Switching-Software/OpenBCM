/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        rx_utils.c
 * Purpose:
 * Requires:
 */


#include <shared/bsl.h>

#include <sal/core/boot.h>
#include <sal/core/dpc.h>
#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/dma.h>

#include <soc/rx.h>

#if defined(BCM_CMICM_SUPPORT)
#include <soc/cmicm.h>
#endif

int
soc_rx_queue_channel_set(int unit, int queue_id,
                         int chan_id)
{
#ifdef BCM_KATANA_SUPPORT
    uint32 i = 0;
#endif
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)
    uint32 ix;
    uint32 chan_id_max = SOC_RX_CHANNELS;
    uint32 soc_rx_channels = SOC_RX_CHANNELS;
    uint32 reg_addr = 0, reg_val;
    int cmc;
    int pci_cmc = SOC_PCI_CMC(unit);
    uint32 chan_off = 0;
    int startq = 0;
    int numq, endq, countq;
    int start_chan_id;

    soc_dma_max_rx_channels_get(unit, &chan_id_max);
    soc_rx_channels = chan_id_max;

    if (0 != SOC_CMCS_NUM(unit)) {
        chan_id_max *= SOC_CMCS_NUM(unit);
    }

    if (SOC_WARM_BOOT(unit)) {
        return SOC_E_NONE;
    }
    if ((chan_id < 0) || (chan_id >= chan_id_max)) {
        /* Verify the chan id */
        return SOC_E_PARAM;
    } else if (queue_id >= NUM_CPU_COSQ(unit)) {
        return SOC_E_PARAM;
    }

    if(soc_feature(unit, soc_feature_cmicm) ||
        soc_feature(unit, soc_feature_cmicx)) {
        /* We institute the normalizing assumption that the
         * channels are numbered first in the PCI CMC, then in order of the
         * ARM CMCs.  This is why we have the shuffling steps below.
         */
        if (chan_id < soc_rx_channels) {
            cmc = pci_cmc;
        } else {
            cmc = SOC_ARM_CMC(unit, ((chan_id / soc_rx_channels) - 1));
            /* compute start queue number for any CMC */
            startq = NUM_CPU_ARM_COSQ(unit, pci_cmc);
            for (ix = 0; ix < cmc; ix++) {
                startq += (ix != pci_cmc) ? NUM_CPU_ARM_COSQ(unit, ix) : 0;
            }
        }

        numq = NUM_CPU_ARM_COSQ(unit, cmc);
        start_chan_id = (cmc != pci_cmc) ? cmc * soc_rx_channels : 0;

        if (queue_id < 0) { /* All COS Queues */
            /* validate the queue range of CMC is in the valid range
             * for this CMC
             */
            SHR_BITCOUNT_RANGE(CPU_ARM_QUEUE_BITMAP(unit, cmc),
                    countq, startq, numq);
            if (numq != countq) {
                return SOC_E_PARAM;
            }

            /* We know chan_id != -1 from the parameter check at the
             * start of the function */
            endq = startq + NUM_CPU_ARM_COSQ(unit, cmc);
            for (ix = start_chan_id; ix < (start_chan_id + soc_rx_channels); ix++) {
                /* set CMIC_CMCx_CHy_COS_CTRL_RX_0/1 based on CMC's start
                 * and end queue number
                 */
                chan_off = ix % soc_rx_channels;
                reg_val = 0;
                if (ix == (uint32)chan_id) {
                    reg_val |= (endq < 32) ?
                        ((uint32)1 << endq) - 1 : 0xffffffff;
                    reg_val &= (startq < 32) ?
                        ~(((uint32)1 << startq) - 1) : 0;
                }

                /*
                 * Hard Code queue_id to 0 to ensure we get back COS_CTRL_RX_0
                 * register address.
                 */
                soc_dma_cos_ctrl_reg_addr_get(unit, cmc, chan_off,
                                              0, &reg_addr);

                /* Incoporate the reserved queues (if any on this device)
                 * into the CMIC programming,  */
                reg_val |= CPU_ARM_RSVD_QUEUE_BITMAP(unit,cmc)[0];
                soc_pci_write(unit, reg_addr, reg_val);

                /*
                 * Hard Code queue_id to 32 to ensure we get back COS_CTRL_RX_1
                 * register address.
                 */
                soc_dma_cos_ctrl_reg_addr_get(unit, cmc, chan_off,
                                              32, &reg_addr);

#if defined(BCM_KATANA_SUPPORT)
                if (SOC_IS_KATANAX(unit)) {
                    /*
                     * Katana queues have been disabled to prevent packets
                     * that cannot egress from reaching the CMIC. At this
                     * point those queues can be enabled.
                     */
                    for (i = startq; i < endq; i++) {
                        reg_val = 0;
                        soc_reg_field_set(unit,
                                THDO_QUEUE_DISABLE_CFG2r, &reg_val, QUEUE_NUMf, i);
                        SOC_IF_ERROR_RETURN(
                                WRITE_THDO_QUEUE_DISABLE_CFG2r(unit, reg_val));
                        reg_val = 0;
                        soc_reg_field_set(unit,
                                THDO_QUEUE_DISABLE_CFG1r, &reg_val, QUEUE_WRf, 1);
                        SOC_IF_ERROR_RETURN(
                                WRITE_THDO_QUEUE_DISABLE_CFG1r(unit, reg_val));
                    }
                }
#endif /*BCM_KATANA_SUPPORT */
                reg_val = 0;
                /* Incoporate the reserved queues (if any on this device)
                 * into the CMIC programming,  */
                reg_val |= CPU_ARM_RSVD_QUEUE_BITMAP(unit,cmc)[1];

                if (ix == (uint32)chan_id) {
                    reg_val |= ((endq >= 32) && (endq < 64)) ?
                        (((uint32)1 << (endq % 32)) - 1) :
                        ((endq < 32) ? 0 : 0xffffffff);
                    reg_val &= (startq >= 32) ?
                        ~(((uint32)1 << (startq % 32)) - 1) : 0xffffffff;
                    if (SOC_IS_TD2P_TT2P(unit)) {
                        soc_pci_write(unit, reg_addr, reg_val);
                    }

                }
                if (!SOC_IS_TD2P_TT2P(unit)) {
                    soc_pci_write(unit, reg_addr, reg_val);
                }
            }
        } else {
            return SOC_E_PARAM;
        }
    } else
#endif
    {
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}

