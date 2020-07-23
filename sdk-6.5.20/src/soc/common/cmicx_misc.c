/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * CMICx miscllaneous functions
 */

#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <sal/core/boot.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <sal/core/sync.h>
#include <soc/util.h>
#include <soc/cmic.h>

#ifdef BCM_CMICX_SUPPORT
#include <soc/cmicx.h>
#include <soc/iproc.h>
#endif

/* PCIe capabilities */
#ifndef PCI_CAPABILITY_LIST
#define PCI_CAPABILITY_LIST     0x34
#endif
#ifndef PCI_CAP_ID_EXP
#define PCI_CAP_ID_EXP          0x10
#endif
#ifndef PCI_EXP_DEVCAP
#define PCI_EXP_DEVCAP          4
#endif
#ifndef PCI_EXP_DEVCTL
#define PCI_EXP_DEVCTL          8
#endif
#ifndef PCI_EXT_CAP_START
#define PCI_EXT_CAP_START       0x100
#endif
#ifndef PCI_EXT_CAP_ID
#define PCI_EXT_CAP_ID(_hdr)    (_hdr & 0x0000ffff)
#endif
#ifndef PCI_EXT_CAP_VER
#define PCI_EXT_CAP_VER(_hdr)   ((_hdr >> 16) & 0xf)
#endif
#ifndef PCI_EXT_CAP_NEXT
#define PCI_EXT_CAP_NEXT(_hdr)  ((_hdr >> 20) & 0xffc)
#endif
#ifndef PCI_EXT_CAP_ID_VNDR
#define PCI_EXT_CAP_ID_VNDR     0x0b
#endif

/* debug print definitions */
#define LOG_CMC_CHAN(suffix) \
    val = soc_pci_read(unit, CMIC_CMCx_PKTDMA_CHy_##suffix##_OFFSET(cmc, chan)); \
    LOG_ERROR(BSL_LS_SOC_DMA, ("CMIC_CMC%d_PKTDMA_CH%d_" #suffix ":0x%x\n", cmc, chan, val))
#define LOG_CMC(suffix) \
    val = soc_pci_read(unit, CMIC_CMCx_##suffix##_OFFSET(cmc)); \
    LOG_ERROR(BSL_LS_SOC_DMA, ("CMIC_CMC%d_" #suffix ":0x%x\n", cmc, val))

#ifdef BCM_CMICX_SUPPORT
/**************************************************
* @function soc_is_cmicx(unit)
* purpose API to test if soc has cmicx
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int soc_is_cmicx(int unit)
{
    unsigned int cap_base, rval;
    if (SOC_IS_MONTEREY(unit))
        return FALSE;  
    /* Look for PCIe vendor-specific extended capability (VSEC) */
    cap_base = PCI_EXT_CAP_START;
    while (cap_base) {
        rval = soc_pci_conf_read(unit, cap_base);
        if (rval == 0xffffffff) {
           /* Assume PCI HW read error */
           return 0;
        }

        if (PCI_EXT_CAP_ID(rval) == PCI_EXT_CAP_ID_VNDR) {
            break;
        }
        cap_base = PCI_EXT_CAP_NEXT(rval);
    }
    if (cap_base) {
        /*
         * VSEC layout:
         *
         * 0x00: PCI Express Extended Capability Header
         * 0x04: Vendor-Specific Header
         * 0x08: Vendor-Specific Register 1
         * 0x0c: Vendor-Specific Register 2
         *     ...
         * 0x24: Vendor-Specific Register 8
         */
         /* Read PCIe Vendor Specific Register 1 */
         /* VENODR REG FORMAT
          * [7:0] iProc Rev = 8'h0E (for P14)
          * [11:8] CMIC BAR = 4'h1 (BAR64-1)
          * [15:12] CMIC Version = 4'h4
          * [19:16] CMIC Rev = 4'h1
          * [22:20] SBUS Version = 4'h4
          */

        rval = soc_pci_conf_read(unit, cap_base + 8);

        if (((rval >> 12) & 0xf) == 0x4) {
            return 1;
        }
    }

    return 0;

}
/**************************************************
* @function soc_cmicx_iproc_version_get
* purpose API to test if soc has cmicx
*
* @param unit [in] unit
* @param iproc_ver [out] iproc version
* @param cmic_ver  [out] cmic version
* @param cmic_rev  [out] cmic revision
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int soc_cmicx_iproc_version_get(int unit, unsigned int *iproc_ver,
                                      unsigned int *cmic_ver,
                                      unsigned int *cmic_rev)
{
    unsigned int cap_base, rval;
    if (SOC_IS_MONTEREY(unit))
        return FALSE;
    /* Look for PCIe vendor-specific extended capability (VSEC) */
    cap_base = PCI_EXT_CAP_START;
    while (cap_base) {
        rval = soc_pci_conf_read(unit, cap_base);
        if (rval == 0xffffffff) {
           /* Assume PCI HW read error */
           return 0;
        }

        if (PCI_EXT_CAP_ID(rval) == PCI_EXT_CAP_ID_VNDR) {
            break;
        }
        cap_base = PCI_EXT_CAP_NEXT(rval);
    }
    if (cap_base) {
        /*
         * VSEC layout:
         *
         * 0x00: PCI Express Extended Capability Header
         * 0x04: Vendor-Specific Header
         * 0x08: Vendor-Specific Register 1
         * 0x0c: Vendor-Specific Register 2
         *     ...
         * 0x24: Vendor-Specific Register 8
         */
         /* Read PCIe Vendor Specific Register 1 */
         /* VENODR REG FORMAT
          * [7:0] iProc Rev = 8'h0E (for P14)
          * [11:8] CMIC BAR = 4'h1 (BAR64-1)
          * [15:12] CMIC Version = 4'h4
          * [19:16] CMIC Rev = 4'h1
          * [22:20] SBUS Version = 4'h4
          */

        rval = soc_pci_conf_read(unit, cap_base + 8);

        *iproc_ver = rval & 0xff;
        *cmic_ver = (rval >> 12) & 0xf;
        *cmic_rev = (rval >> 16) & 0xf;
        return 1;
    }

    return 0;

}

/**************************************************
* @function soc_cmicx_pci_test
* purpose API to test PCI access to cmicx registers
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int soc_cmicx_pci_test(int unit)
{
    int i;
    uint32 tmp, reread;
    uint32 pat;

    SCHAN_LOCK(unit);

    /* Check for address uniqueness */

        for (i = 0; i < CMIC_SCHAN_WORDS(unit); i++) {
            pat = 0x55555555 ^ (i << 24 | i << 16 | i << 8 | i);
            soc_pci_write(unit, CMIC_COMMON_POOL_SCHAN_CHx_MESSAGEn(0, i), pat);
        }

        for (i = 0; i < CMIC_SCHAN_WORDS(unit); i++) {
            pat = 0x55555555 ^ (i << 24 | i << 16 | i << 8 | i);
            tmp = soc_pci_read(unit, CMIC_COMMON_POOL_SCHAN_CHx_MESSAGEn(0, i));
            if (tmp != pat) {
                goto error;
            }
        }

    if (!SAL_BOOT_QUICKTURN) {  /* Takes too long */
        /* Rotate walking zero/one pattern through each register */

        pat = 0xff7f0080;       /* Simultaneous walking 0 and 1 */

        for (i = 0; i < CMIC_SCHAN_WORDS(unit); i++) {
            int j;

            for (j = 0; j < 32; j++) {
                    soc_pci_write(unit, CMIC_COMMON_POOL_SCHAN_CHx_MESSAGEn(0, i), pat);
                    tmp = soc_pci_read(unit, CMIC_COMMON_POOL_SCHAN_CHx_MESSAGEn(0, i));
                if (tmp != pat) {
                    goto error;
                }
                pat = (pat << 1) | ((pat >> 31) & 1);	/* Rotate left */
            }
        }
    }

    /* Clear to zeroes when done */

    for (i = 0; i < CMIC_SCHAN_WORDS(unit); i++) {
            soc_pci_write(unit, CMIC_COMMON_POOL_SCHAN_CHx_MESSAGEn(0, i), 0);
    }
    SCHAN_UNLOCK(unit);
    return SOC_E_NONE;

 error:
    reread = soc_pci_read(unit, CMIC_COMMON_POOL_SCHAN_CHx_MESSAGEn(0, i));

    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "FATAL PCI error testing PCIM[0x%x]:\n"
                          "Wrote 0x%x, read 0x%x, re-read 0x%x\n"),
               i, pat, tmp, reread));

    SCHAN_UNLOCK(unit);
    return SOC_E_INTERNAL;
}

/**************************************************
* @function soc_cmicx_paxb_tx_arbiter_set
* purpose: API to set tx arbiter
*
* @param unit [in] unit
* @param unit [in] enable
* @param unit [in] priority
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int soc_cmicx_paxb_tx_arbiter_set(int unit, uint8 enable, uint8 priority)
{
    uint32 cmic_reg_val = 0;

    if (SOC_IS_TRIDENT3X(unit)) {
        soc_reg_field_set(unit, PAXB_0_PAXB_TX_ARBITER_PRIORITYr, &cmic_reg_val,
                          TX_REQ_SEQ_ENf, enable);
        soc_reg_field_set(unit, PAXB_0_PAXB_TX_ARBITER_PRIORITYr, &cmic_reg_val,
                          TX_ARB_PRIORITYf, priority);
        SOC_IF_ERROR_RETURN(WRITE_PAXB_0_PAXB_TX_ARBITER_PRIORITYr(unit, cmic_reg_val));
    }

    return SOC_E_NONE;
}

/**************************************************
* @function soc_cmicx_reg_offset_get
* purpose: API to get min and max offsets for cmicx reg
*
* @param unit [out] min
* @param unit [out] max
*
* @returns none
*
* @end
 */

void soc_cmicx_reg_offset_get(uint32 *min, uint32 *max)
{
    *min = CMICX_OFFSET_MIN;
    *max = CMICX_OFFSET_MAX;

    return;
}

/* store the cmc and IF of a DMA channel */
typedef struct {
    uint8 channel_cmc;
    uint8 channel_id;
} channel_ids_t;

/*
 * structure/object storing channel_ids_ts that can be added, and then iterated,
 * while the current channel_ids_t can be removed during iteration.
 * All functions have O(1) runtime.
 * Does not preserve the order of the channels.
 */
#define CHANS_GROUP_MAX_SIZE (CMIC_CMC_NUM_MAX * CMICX_N_DMA_CHAN)
typedef struct {
    unsigned nof_chans;
    channel_ids_t channels[CHANS_GROUP_MAX_SIZE];
} chans_group_t;
typedef channel_ids_t *chans_group_iter_t;
/* init object to be empty */
static inline void chans_group_init(chans_group_t *group) {
    group->nof_chans = 0;
}
/* init the itertor to the start of the group */
static inline void chans_group_iter_start(chans_group_t *group, chans_group_iter_t *iter) {
    *iter = group->channels;
}
/* is iterator pointing to beyond the end of the group */
static inline int chans_group_is_end(chans_group_t *group, chans_group_iter_t iter) {
    return iter >= group->channels + group->nof_chans;
}
/* return the size (number of channels) of the group */
static inline unsigned chans_group_get_nof_chans(chans_group_t *group) {
    return group->nof_chans;
}
/* Insert a channel to the group */
static inline void chans_group_insert(chans_group_t *group, int cmc, int chan) {
    chans_group_iter_t i = group->channels + group->nof_chans;
    assert(group->nof_chans < CHANS_GROUP_MAX_SIZE);
    i->channel_cmc = cmc;
    i->channel_id = chan;
    ++group->nof_chans;
}
/*
 * Remove the channel pointed to by the iterator from the group,
 * the iterator will point to a channel not yet traversed,
 * or point past the end of the group if all channels were traversed
 */
static inline void chans_group_delete(chans_group_t *group, chans_group_iter_t iter) {
    assert(group->nof_chans > 0);
    *iter = group->channels[--group->nof_chans];
}
/* get the cahnnel pointed to by the iterator */
static inline void chans_group_iter_t_get(chans_group_iter_t channel_iter, int *cmc, int *chan) {
    *cmc = channel_iter->channel_cmc;
    *chan = channel_iter->channel_id;
}

typedef enum { /* possible states of the timeout used by cmicx_dma_abort() */
    abort_timeout_not_initialized,
    abort_timeout_not_passed,
    abort_timeout_passed
} abort_timeout_state_t;

/* The timeout needs to be at least 50ms which is the PCIe completion timeout value  */
#define MIN_CMICX_DMA_ABORT_TIMEOUT 50000
#define DEFAULT_CMICX_DMA_ABORT_TIMEOUT (MIN_CMICX_DMA_ABORT_TIMEOUT * 6 / 5)
#define CMICX_INITIAL_SLEEP 10000
/* Do DMA abort in cold boot by default */
#define DEFAULT_CMIC_DMA_ABORT_IN_COLD_BOOT 1

/*
 * Function:
 *      cmicx_dma_abort
 * Purpose:
 *      Deconfigure previously defined DMA operations and abort pending operation.
 *      Useful in case SDK was not de-initialized / not brought down properly,
 *      and when not resetting on startup; like in an unscheduled warm boot.
 *      Most important for DMA operations initiated by the device operating on
 *      external CPU memory, like FIFO and packet DMA.
 *      Also handles operations that did not complete.
 * Parameters:
 *      unit - SOC unit #
 *      flags - flags specifying not to handle some DMA types
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */
int
soc_cmicx_dma_abort(int unit, uint32 flags)
{
    int cmc, chan, nof_channels, nof_failures = 0;
    int cmc_num_max = CMIC_CMC_NUM_MAX;
    uint32 val, cmic_address, done;
    
    sal_usecs_t start_time = 0, iter_time, timeout, timeout_sleep = CMICX_INITIAL_SLEEP;
    abort_timeout_state_t timeout_state = abort_timeout_not_initialized;
    chans_group_iter_t channel_iter;
    chans_group_t packet_channels, sbus_channels, fifo_channels, ccm_channels, schan_fifo_channels;
    uint32 sbusdma_chan_max  = CMIC_CMCx_SBUSDMA_CHAN_MAX;

#ifdef BCM_TRIDENT3_SUPPORT
    uint32 ifa_cc_num_sbusdma = soc_property_get(unit,
                                                spn_IFA_CC_NUM_SBUS_DMA, 2);
    if ((SOC_IS_TRIDENT3(unit)) && (ifa_cc_num_sbusdma > 2)) {
        /* CMC_0 2 SBUS DMA Channels are used by IFA CC Eapp on M0 */
        sbusdma_chan_max = (CMIC_CMCx_SBUSDMA_CHAN_MAX - 2);
    }
#endif

    LOG_VERBOSE(BSL_LS_SOC_DMA, (BSL_META_U(unit, "aborting and disabling DMA\n")));

    /* Abort all DMA types */
    /* Use flag SOC_DMA_ABORT_CMC0 in case other CMC's are used by the internal Arm CPUs */

    if (flags & SOC_DMA_ABORT_CMC0) {
        cmc_num_max = 1;
    }

    timeout = soc_property_get(unit, spn_DMA_ABORT_TIMEOUT_USEC, DEFAULT_CMICX_DMA_ABORT_TIMEOUT);
    if (timeout < MIN_CMICX_DMA_ABORT_TIMEOUT) {
        timeout = MIN_CMICX_DMA_ABORT_TIMEOUT;
    }

    /* Before aborting, prevent receiving packet DMA from the device */
    /* If performed during receiving packets, may result in partial packets */
    if ((flags & SOC_DMA_ABORT_DISCONNECT_PACKET)) {
        for (cmc = 0; cmc < cmc_num_max; ++cmc) {
            for (chan = 0; chan < CMIC_CMCx_PKTDMA_NOF_CHANS; ++chan) {
                /* Drop waiting packets */
                cmic_address = CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan);
                val = soc_pci_read(unit, cmic_address);
                soc_pci_write(unit, cmic_address, val | PKTDMA_DROP_RX_PKT_ON_CHAIN_END);
                /* Prevent more packets from arriving */
                soc_pci_write(unit, CMIC_CMCx_PKTDMA_CHy_COS_CTRL_RX_0_OFFSET(cmc, chan), 0);
                soc_pci_write(unit, CMIC_CMCx_PKTDMA_CHy_COS_CTRL_RX_1_OFFSET(cmc, chan), 0);
            }
        }
    }

    /* abort packet DMA in all channels */
    chans_group_init(&packet_channels);
    if ((flags & SOC_DMA_ABORT_SKIP_PACKET) == 0) {
        for (cmc = 0; cmc < cmc_num_max; ++cmc) {
            for (chan = 0; chan < CMIC_CMCx_PKTDMA_NOF_CHANS; ++chan) {
                cmic_address = CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan);
                /* if CMIC_CMCx_PKTDMA_CHy_CTRL.DMA_EN == 1 */
                val = soc_pci_read(unit, cmic_address);
                if (val & PKTDMA_ENABLE) {
                    /* set CMIC_CMCx_PKTDMA_CHy_CTRL.ABORT_DMA=1 to abort, and later wait for CMIC_CMCx_SHARED_IRQ_STAT0.CHy_CHAIN_DONE==1 */
                    soc_pci_write(unit, cmic_address, val | PKTDMA_ABORT);
                    chans_group_insert(&packet_channels, cmc, chan); /* mark the channel to be waited on */
                    LOG_DEBUG(BSL_LS_SOC_DMA, (BSL_META_U(unit, "Aborting packet DMA CMC %d channel %d\n"), cmc, chan));
                }
            }
        }
    }

    /* abort s-bus DMA in all channels */
    chans_group_init(&sbus_channels);
    if ((flags & SOC_DMA_ABORT_SKIP_SBUS) == 0) {
        /* For  CMCs x=0-1  and channels y=0-3 and [channel used by this CPU] */
        for (cmc = 0; cmc < cmc_num_max; ++cmc) {
            for (chan = 0; chan < sbusdma_chan_max; ++chan) {
                cmic_address = CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, chan);
                /* if CMIC_CMCx_SBUSDMA_CHy_CONTROL.START == 1 */
                val = soc_pci_read(unit, cmic_address);
                if (val & CMIC_CMCx_SBUSDMA_CHy_CONTROL_START) {
                    /* set CMIC_CMCx_SBUSDMA_CHy_CONTROL.ABORT=1 to abort */
                    soc_pci_write(unit, cmic_address, val | CMIC_CMCx_SBUSDMA_CHy_CONTROL_ABORT);
                    chans_group_insert(&sbus_channels, cmc, chan); /* mark the channel to be waited on */
                    LOG_DEBUG(BSL_LS_SOC_DMA, (BSL_META_U(unit, "Aborting s-bus DMA CMC %d channel %d\n"), cmc, chan));
                }
            }
        }
    }

    /* abort FIFO DMA in all channels, FIFO DMA is not per CMC */
    chans_group_init(&fifo_channels);
    if ((flags & SOC_DMA_ABORT_SKIP_FIFO) == 0) {
        for (chan = 0; chan < CMICX_N_FIFODMA_CHAN; ++chan) {
            cmic_address = CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_OFFSET(chan);
            /* if CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG.ENABLE == 1 */
            val = soc_pci_read(unit, cmic_address);
            if (val & CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_ENABLE) {
                /* set CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG.ABORT=1 to abort */
                soc_pci_write(unit, cmic_address, val | CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_ABORT);
                chans_group_insert(&fifo_channels, 0, chan); /* mark the channel to be waited on */
                LOG_DEBUG(BSL_LS_SOC_DMA, (BSL_META_U(unit, "Aborting FIFO DMA channel %d\n"), chan));
            }
        }
    }

    /* abort CCM DMA in all channels */
    chans_group_init(&ccm_channels);
    if ((flags & SOC_DMA_ABORT_SKIP_CCM) == 0) {
        /* For  CMCs x=0-1  and channels y=0-1 and [channel used by this CPU] */
        for (cmc = 0; cmc < cmc_num_max; ++cmc) {
            for (chan = 0; chan < CMICX_N_CCMDMA_CHAN; ++chan) {
                cmic_address = CMIC_CMCx_CCMDMA_CHy_CFG_OFFSET(cmc, chan);
                /* if CMIC_CMCx_CCMDMA_CHy_CFG.EN == 1 */
                val = soc_pci_read(unit, cmic_address);
                if (val & CMIC_CMCx_CCMDMA_CHy_CFG_EN) {
                    /* set CMIC_CMCx_CCMDMA_CHy_CFG.ABORT=1 to abort */
                    soc_pci_write(unit, cmic_address, val | CMIC_CMCx_CCMDMA_CHy_CFG_ABORT);
                    chans_group_insert(&ccm_channels, cmc, chan); /* mark the channel to be waited on */
                    LOG_DEBUG(BSL_LS_SOC_DMA, (BSL_META_U(unit, "Aborting CCM DMA CMC %d channel %d\n"), cmc, chan));
                }
            }
        }
    }

    /* abort s-chan FIFO in all channels, s-chan FIFO is not per CMC */
    chans_group_init(&schan_fifo_channels);
    if ((flags & SOC_DMA_ABORT_SKIP_SCHAN_FIFO) == 0) {
        /* For channels y=0-1 and [channels used by this CPU] */
        for (chan = 0; chan < CMIC_SCHAN_FIFO_NUM_MAX; ++chan) {
            cmic_address = CMIC_SCHAN_FIFO_CHx_CTRL(chan);
            /* if CMIC_COMMON_POOL_SCHAN_FIFO_0_CHy_CTRL.START == 1 */
            val = soc_pci_read(unit, cmic_address);
            if (val & SCHAN_FIFO_CTRL_START) {
                /* set CMIC_COMMON_POOL_SCHAN_FIFO_0_CHy_CTRL.ABORT=1 to abort */
                soc_pci_write(unit, cmic_address, val | SCHAN_FIFO_CTRL_ABORT);
                chans_group_insert(&schan_fifo_channels, 0, chan); /* mark the channel to be waited on */
                LOG_DEBUG(BSL_LS_SOC_DMA, (BSL_META_U(unit, "Aborting s-chan FIFO channel %d\n"), chan));
            }
        }
    }

    nof_channels = chans_group_get_nof_chans(&packet_channels) + chans_group_get_nof_chans(&sbus_channels) +
      chans_group_get_nof_chans(&fifo_channels) + chans_group_get_nof_chans(&ccm_channels) + chans_group_get_nof_chans(&schan_fifo_channels);

    /* loop and check that each abort finished. When it finished or after time out, clear the operation and disable the DMA */
    do {
        /* measure time at the start of the iteration */
        iter_time = sal_time_usecs();
        if (timeout_state == abort_timeout_not_initialized) {
            timeout_state = abort_timeout_not_passed;
            start_time = iter_time;
        }
        if (iter_time - start_time >= timeout ) { /* was the timeout value reached? */
            timeout_state = abort_timeout_passed;
        }


        /* for all packet DMA channels still not done */
        for (chans_group_iter_start(&packet_channels, &channel_iter); !chans_group_is_end(&packet_channels, channel_iter); ++channel_iter) {
            chans_group_iter_t_get(channel_iter, &cmc, &chan); /* get the channel to work on */
            /* If the abort is done (CMIC_CMCx_SHARED_IRQ_STAT0.CHy_CHAIN_DONE==1), disable packet DMA */
            done = soc_pci_read(unit, CMIC_CMCx_SHARED_IRQ_STAT0_OFFSET(cmc)) & DS_CMCx_CHy_DMA_CHAIN_DONE(chan);
            if (done || timeout_state == abort_timeout_passed) {
                /* clear CMIC_CMCx_PKTDMA_CHy_CTRL.ABORT_DMA|DMA_EN in the same write to disable; it also clears statuses */
                if (done) { /* remove the channel from the channels waited on */
                    chans_group_delete(&packet_channels, channel_iter--);
                } else {
                    unsigned i, debug_values[] = {0x10000, 0x10010, 0x10020, 0x10030, 0x10080, 0x10090, 0x100a0, 0x100b0} ;
                    LOG_ERROR(BSL_LS_SOC_DMA, (BSL_META_U(unit, "Failed to abort packet DMA in CMC %d channel %d, check with the design team\n"), cmc, chan));
                    ++nof_failures;
                    LOG_CMC_CHAN(CTRL);
                    LOG_CMC_CHAN(DESC_ADDR_LO);
                    LOG_CMC_CHAN(DESC_ADDR_HI);
                    LOG_CMC_CHAN(STAT);
                    LOG_CMC_CHAN(DEBUG_STATUS);
                    LOG_CMC_CHAN(CURR_DESC_LO);
                    LOG_CMC_CHAN(CURR_DESC_HI);
                    LOG_CMC_CHAN(PKT_COUNT_RXPKT);
                    LOG_CMC_CHAN(PKT_COUNT_TXPKT);
                    LOG_CMC_CHAN(PKT_COUNT_RXPKT_DROP);
                    LOG_CMC_CHAN(DESC_COUNT_REQ);
                    LOG_CMC_CHAN(DESC_COUNT_RX);
                    LOG_CMC_CHAN(DESC_COUNT_STATUS_WR);

                    cmic_address = CMIC_CMCx_PKTDMA_CHy_DEBUG_CONTROL_OFFSET(cmc, chan);
                    for (i = 0; i < 4; ++i) {
                        soc_pci_write(unit, cmic_address, (i << 8) | 0x3f);
                        LOG_ERROR(BSL_LS_SOC_DMA, ("0x%x: ", (i << 8) | 0x3f));
                        LOG_CMC_CHAN(DEBUG_CONTROL);
                        LOG_CMC_CHAN(DEBUG_SM_STATUS);
                        LOG_CMC_CHAN(CURR_DESC_LO);
                        LOG_CMC_CHAN(CURR_DESC_HI);
                    }
                    LOG_CMC(SHARED_DMA_MASTER_TOTAL_OUTSTD_TRANS_LIMIT);
                    LOG_CMC(SHARED_DMA_MASTER_READ_PER_ID_OUTSTD_TRANS_LIMIT);
                    LOG_CMC(SHARED_AXI_STAT);
                    LOG_CMC(SHARED_AXI_PER_ID_STAT);

                    for (i = 0; i < sizeof(debug_values) / sizeof(debug_values[0]); ++i) {
                    WRITE_DMU_CRU_IPROC_DEBUG_SELr(unit, debug_values[i]);
                    READ_DMU_CRU_IPROC_DEBUG_STATUSr(unit, &val);
                    LOG_ERROR(BSL_LS_SOC_DMA, ("wrote DMU_CRU_IPROC_DEBUG_SEL:0x%x read MU_CRU_IPROC_DEBUG_STATUS:%x\n",
                      debug_values[i], val));
                    }
                }
                cmic_address = CMIC_CMCx_PKTDMA_CHy_CTRL_OFFSET(cmc, chan);
                val = soc_pci_read(unit, cmic_address);
                soc_pci_write(unit, cmic_address, val & ~(PKTDMA_ENABLE | PKTDMA_ABORT));
            }
        }

        /* for all s-bus DMA channels still not done */
        for (chans_group_iter_start(&sbus_channels, &channel_iter); !chans_group_is_end(&sbus_channels, channel_iter); ++channel_iter) {
            chans_group_iter_t_get(channel_iter, &cmc, &chan); /* get the channel to work on */
            /* If the abort is done (CMIC_CMCx_SBUSDMA_CHy_STAT.DONE==1), then clear the operation */
            done = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc, chan)) & CMIC_CMCx_SBUSDMA_CHy_STATUS_DONE;
            if (done || timeout_state == abort_timeout_passed) {
                /* clear CMIC_CMCx_SBUSDMA_CHy_CONTROL.ABORT|START in the same write disables the original operation and abort, and clears statuses */
                cmic_address = CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, chan);
                val = soc_pci_read(unit, cmic_address);
                soc_pci_write(unit, cmic_address, val & ~(CMIC_CMCx_SBUSDMA_CHy_CONTROL_ABORT |CMIC_CMCx_SBUSDMA_CHy_CONTROL_START ));
                if (done) { /* remove the channel from the channels waited on */
                    chans_group_delete(&sbus_channels, channel_iter--);
                } else {
                    LOG_ERROR(BSL_LS_SOC_DMA, (BSL_META_U(unit, "Failed to abort s-bus DMA in CMC %d channel %d, check with the design team\n"), cmc, chan));
                    ++nof_failures;
                }
            }
        }

        /* for all FIFO DMA channels still not done */
        for (chans_group_iter_start(&fifo_channels, &channel_iter); !chans_group_is_end(&fifo_channels, channel_iter); ++channel_iter) {
            chans_group_iter_t_get(channel_iter, &cmc, &chan); /* get the channel to work on */
            /* If the abort is done (CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_STAT.ABORTED==1), then clear the operation and disable */
            done = soc_pci_read(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_STAT_OFFSET(chan)) & CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_STAT_ABORTED;
            if (done || timeout_state == abort_timeout_passed) {
                /* clear CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG.ABORT|ENABLE in the same write: disables and clears statuses */
                cmic_address = CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_OFFSET(chan);
                val = soc_pci_read(unit, cmic_address);
                soc_pci_write(unit, cmic_address, val & ~(CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_ABORT | CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_ENABLE));
                if (done) { /* remove the channel from the channels waited on */
                    chans_group_delete(&fifo_channels, channel_iter--);
                } else {
                    LOG_ERROR(BSL_LS_SOC_DMA, (BSL_META_U(unit, "Failed to abort FIFO DMA in channel %d, check with the design team\n"), chan));
                    ++nof_failures;
                }
            }
        }

        /* for all CCM DMA channels still not done */
        for (chans_group_iter_start(&ccm_channels, &channel_iter); !chans_group_is_end(&ccm_channels, channel_iter); ++channel_iter) {
            chans_group_iter_t_get(channel_iter, &cmc, &chan); /* get the channel to work on */
            /* If the abort is done (CMIC_CMCx_CCMDMA_CHy_STAT.DONE==1), then clear the operation */
            done = soc_pci_read(unit, CMIC_CMCx_CCMDMA_CHy_STAT_OFFSET(cmc, chan)) & CMIC_CMCx_CCMDMA_CHy_STAT_DONE;
            if (done || timeout_state == abort_timeout_passed) {
                /* clear CMIC_CMCx_CCMDMA_CHy_CFG.ABORT|EN in the same write: disables and clears statuses */
                cmic_address = CMIC_CMCx_CCMDMA_CHy_CFG_OFFSET(cmc, chan);
                val = soc_pci_read(unit, cmic_address);
                soc_pci_write(unit, cmic_address, val & ~(CMIC_CMCx_CCMDMA_CHy_CFG_ABORT | CMIC_CMCx_CCMDMA_CHy_CFG_EN));
                if (done) { /* remove the channel from the channels waited on */
                    chans_group_delete(&ccm_channels, channel_iter--);
                } else {
                    LOG_ERROR(BSL_LS_SOC_DMA, (BSL_META_U(unit, "Failed to abort CCM DMA in CMC %d channel %d, check with the design team\n"), cmc, chan));
                    ++nof_failures;
                }
            }
        }

        /* for all s-chan FIFO channels still not done */
        for (chans_group_iter_start(&schan_fifo_channels, &channel_iter); !chans_group_is_end(&schan_fifo_channels, channel_iter); ++channel_iter) {
            chans_group_iter_t_get(channel_iter, &cmc, &chan); /* get the channel to work on */
            /* If the abort is done (CMIC_COMMON_POOL_SCHAN_FIFO_0_CHy_STATUS.DONE==1), then clear the operation */
            done = soc_pci_read(unit, CMIC_SCHAN_FIFO_CHx_STATUS(chan)) & SCHAN_FIFO_STATUS_DONE;
            if (done || timeout_state == abort_timeout_passed) {
                /* clear CMIC_COMMON_POOL_SCHAN_FIFO_0_CHy_CTRL.ABORT|START in the same write: disables and clears statuses*/
                cmic_address = CMIC_SCHAN_FIFO_CHx_CTRL(chan);
                val = soc_pci_read(unit, cmic_address);
                soc_pci_write(unit, cmic_address, val & ~(SCHAN_FIFO_CTRL_ABORT | SCHAN_FIFO_CTRL_START));
                if (done) { /* remove the channel from the channels waited on */
                    chans_group_delete(&schan_fifo_channels, channel_iter--);
                } else {
                    LOG_ERROR(BSL_LS_SOC_DMA, (BSL_META_U(unit, "Failed to abort s-chan FIFO in channel %d, check with the design team\n"), chan));
                    ++nof_failures;
                }
            }
        }


        if (timeout_state != abort_timeout_not_passed || (!chans_group_get_nof_chans(&packet_channels) &&
            !chans_group_get_nof_chans(&sbus_channels) && !chans_group_get_nof_chans(&fifo_channels) &&
            !chans_group_get_nof_chans(&ccm_channels) && !chans_group_get_nof_chans(&schan_fifo_channels))) {
            break; /* exist if the timeout passed at the start of the iteration, or if there is no channel left to wait for */
        }
        sal_usleep(timeout_sleep); /* wait before the next iteration */
        timeout_sleep += CMICX_INITIAL_SLEEP; /* next time sleep more, will use less than sqrt(2 * timeout / CMICX_INITIAL_SLEEP) iterations */
    } while (1);


    LOG_INFO(BSL_LS_SOC_DMA, (BSL_META_U(unit,
      "Aborted and disabled %d DMA channels with %d abort failures\n"), nof_channels, nof_failures));

    return nof_failures ? SOC_E_TIMEOUT : SOC_E_NONE;
}

/*
 * Function:
 *      cmicx_dma_abort_and_handle_hot_swap
 * Purpose:
 *      Handle DMA aborting and the iproc hot swap mechanism at startup and
 *      de-configure previously defined DMA operations and abort pending operation.
 *      The hot swap handling will be done if the iproc hot-swap state machine
 *      shows it is needed.
 * Parameters:
 *      unit - SOC unit #
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */
int
soc_cmicx_handle_hotswap_and_dma_abort(int unit)
{
    int rv = SOC_E_NONE, rv2;
    if (!SOC_WARM_BOOT(unit)) {
        if (soc_property_get(unit, spn_CMIC_DMA_ABORT_IN_COLD_BOOT, DEFAULT_CMIC_DMA_ABORT_IN_COLD_BOOT))
        {
            if ((rv = soc_cmicx_dma_abort(unit, 0)) != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_DMA, (BSL_META_U(unit, "Failed aborting DMA operations\n")));
            }
        }
        rv2 = soc_pcie_hot_swap_handling(unit, SOC_PCIE_HOTSWAP_HANDLING_FLAG_DONOT_ABORT |
          SOC_PCIE_HOTSWAP_HANDLING_FLAG_DONOT_DISABLE_INTERRUPTS);
        if (rv2 != SOC_E_NONE && rv == SOC_E_NONE) {
            rv = rv2;
        }
    } else {
        rv = soc_pcie_hot_swap_handling(unit, SOC_PCIE_HOTSWAP_HANDLING_FLAG_DONOT_DISABLE_INTERRUPTS);
    }
    return rv;
}

#endif /* BCM_CMICX_SUPPORT  */
