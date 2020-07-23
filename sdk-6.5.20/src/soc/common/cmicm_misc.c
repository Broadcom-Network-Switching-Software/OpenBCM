/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * CMICm miscllaneous functions
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

#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#include <soc/iproc.h>

/* store the cmc and IF of a DMA channel */
typedef struct {
    uint8 cmc;
    uint8 chan;
} channel_ids_t;

/*
 * structure/object storing channel_ids_ts that can be added, and then iterated,
 * while the current channel_ids_t can be removed during iteration.
 * All functions have O(1) runtime.
 * Does not preserve the order of the channels.
 */
#define CHANS_GROUP_MAX_SIZE ((CMIC_CMC_NUM_MAX + 1) * N_DMA_CHAN)
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
    i->cmc = cmc;
    i->chan = chan;
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
    *cmc = channel_iter->cmc;
    *chan = channel_iter->chan;
}

typedef enum { /* possible states of the timeout used by cmicm_dma_abort() */
    abort_timeout_not_initialized,
    abort_timeout_not_passed,
    abort_timeout_passed
} abort_timeout_state_t;

/* The timeout needs to be at least 50ms which is the PCIe completion timeout value  */
#define MIN_CMICM_DMA_ABORT_TIMEOUT         50000
#define DEFAULT_CMICM_DMA_ABORT_TIMEOUT     (MIN_CMICM_DMA_ABORT_TIMEOUT * 6 / 5)
#define CMICM_INITIAL_SLEEP                 10000

/*
 * Function:
 *      cmicm_dma_abort
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
soc_cmicm_dma_abort(int unit, uint32 flags)
{
    int cmc, chan, nof_channels = 0, nof_failures = 0;
    int cmc_num_max = CMIC_CMC_NUM_MAX;
    uint32 val, cmic_address, done, active;
    
    sal_usecs_t start_time = 0, iter_time, timeout, timeout_sleep = CMICM_INITIAL_SLEEP;
    abort_timeout_state_t timeout_state = abort_timeout_not_initialized;
    chans_group_iter_t channel_iter;
    chans_group_t packet_channels, sbus_channels, fifo_channels, ccm_channels, schan_modules;

    LOG_VERBOSE(BSL_LS_SOC_DMA, (BSL_META_U(unit, "aborting and disabling DMA\n")));

    /* Abort all DMA types */
    /* Use flag SOC_DMA_ABORT_CMC0 in case other CMC's are used by the internal Arm CPUs */

    if (flags & SOC_DMA_ABORT_CMC0) {
        cmc_num_max = 1;
    }

    timeout = soc_property_get(unit, spn_DMA_ABORT_TIMEOUT_USEC, DEFAULT_CMICM_DMA_ABORT_TIMEOUT);
    if (timeout < MIN_CMICM_DMA_ABORT_TIMEOUT) {
        timeout = MIN_CMICM_DMA_ABORT_TIMEOUT;
    }

    /* Before aborting, prevent receiving packet DMA from the device */
    /* If performed during receiving packets, may result in partial packets */
    if ((flags & SOC_DMA_ABORT_DISCONNECT_PACKET)) {
        for (cmc = 0; cmc < cmc_num_max; ++cmc) {
            for (chan = 0; chan < N_DMA_CHAN; ++chan) {
                /* Drop waiting packets */
                cmic_address = CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, chan);
                val = soc_pci_read(unit, cmic_address);
                soc_pci_write(unit, cmic_address, val | PKTDMA_DROP_RX_PKT_ON_CHAIN_END);
                /* Prevent more packets from arriving */
                soc_pci_write(unit, CMIC_CMCx_CHy_COS_CTRL_RX_0_OFFSET(cmc, chan), 0);
                soc_pci_write(unit, CMIC_CMCx_CHy_COS_CTRL_RX_1_OFFSET(cmc, chan), 0);
            }
        }
    }

    /* abort packet DMA in all channels */
    chans_group_init(&packet_channels);
    if ((flags & SOC_DMA_ABORT_SKIP_PACKET) == 0) {
        for (cmc = 0; cmc < cmc_num_max; ++cmc) {
            for (chan = 0; chan < N_DMA_CHAN; ++chan) {
                cmic_address = CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, chan);
                /* if PKTDMA_ENABLE == 1 */
                val = soc_pci_read(unit, cmic_address);
                if (val & PKTDMA_ENABLE) {
                    /* set CMIC_CMCx_CHy_DMA_CTRL.ABORT_DMA=1 to abort, and later wait for CMIC_CMCx_DMA_STAT.CHy_DMA_ACTIVE==0 */
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
            for (chan = 0; chan < CMIC_CMCx_SBUSDMA_CHAN_MAX; ++chan) {
                cmic_address = CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, chan);
                /* if SBUSDMA_START == 1 */
                val = soc_pci_read(unit, cmic_address);
                if (val & SBUSDMA_START) {
                    /* set CMIC_CMCx_SBUSDMA_CHy_CONTROL.ABORT=1 to abort */
                    soc_pci_write(unit, cmic_address, val | SBUSDMA_ABORT);
                    chans_group_insert(&sbus_channels, cmc, chan); /* mark the channel to be waited on */
                    LOG_DEBUG(BSL_LS_SOC_DMA, (BSL_META_U(unit, "Aborting s-bus DMA CMC %d channel %d\n"), cmc, chan));
                }
            }
        }
    }

    /* abort FIFO DMA in all channels, FIFO DMA is not per CMC */
    chans_group_init(&fifo_channels);
    if ((flags & SOC_DMA_ABORT_SKIP_FIFO) == 0) {
        for (cmc = 0; cmc < cmc_num_max; ++cmc) {
            for (chan = 0; chan < N_DMA_CHAN; ++chan) {
                cmic_address = CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, chan);
                /* if FIFODMA_ENABLE == 1 */
                val = soc_pci_read(unit, cmic_address);
                if (val & FIFODMA_ENABLE) {
                    /* set CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG.ABORT=1 to abort */
                    soc_pci_write(unit, cmic_address, val | FIFODMA_ABORT);
                    chans_group_insert(&fifo_channels, cmc, chan); /* mark the channel to be waited on */
                    LOG_DEBUG(BSL_LS_SOC_DMA, (BSL_META_U(unit, "Aborting FIFO DMA CMC %d channel %d\n"), cmc, chan));
                }
            }
        }
    }


    /* abort CCM DMA in all channels */
    chans_group_init(&ccm_channels);
    if ((flags & SOC_DMA_ABORT_SKIP_CCM) == 0) {
        /* For  CMCs x=0-1  and channels y=0-1 and [channel used by this CPU] */
        for (cmc = 0; cmc < cmc_num_max; ++cmc) {
            cmic_address = CMIC_CMCx_CCM_DMA_CFG_OFFSET(cmc);
            /* if CCMDMA_ENABLE == 1 */
            val = soc_pci_read(unit, cmic_address);
            if (val & CCMDMA_ENABLE) {
                /* set CMIC_CMCx_CCMDMA_CHy_CFG.ABORT=1 to abort */
                soc_pci_write(unit, cmic_address, val | CCMDMA_ABORT);
                chans_group_insert(&ccm_channels, cmc, 0); /* mark the channel to be waited on */
                LOG_DEBUG(BSL_LS_SOC_DMA, (BSL_META_U(unit, "Aborting CCM DMA CMC %d\n"), cmc));
            }
        }
    }

    /* abort SCHAN for all modules */
    chans_group_init(&schan_modules);
    if ((flags & SOC_DMA_ABORT_SKIP_SCHAN_FIFO) == 0) {
        for (cmc = 0; cmc <= CMIC_CMC_NUM_MAX; ++cmc) {
            cmic_address = CMIC_COMMON_CMCx_SCHAN_CTRL_OFFSET(cmc);
            /* if CMIC_COMMON_POOL_SCHAN_FIFO_0_CHy_CTRL.MSG_START == 1 */
            val = soc_pci_read(unit, cmic_address);
            if (val & SC_CMCx_MSG_START) {
                /* set CMIC_COMMON_POOL_SCHAN_FIFO_0_CHy_CTRL.ABORT=1 to abort */
                soc_pci_write(unit, cmic_address, val | SC_CMCx_SCHAN_ABORT);
                chans_group_insert(&schan_modules, cmc, 0); /* mark the channel to be waited on */
                LOG_DEBUG(BSL_LS_SOC_DMA, (BSL_META_U(unit, "Aborting s-chan modules %d\n"), cmc));
            }
        }
    }

    nof_channels = chans_group_get_nof_chans(&packet_channels) + chans_group_get_nof_chans(&sbus_channels) +
                   chans_group_get_nof_chans(&fifo_channels) + chans_group_get_nof_chans(&ccm_channels) + 
                   chans_group_get_nof_chans(&schan_modules);

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
            /* If the abort is done (CMIC_CMCx_DMA_STAT_OFFSET.DS_CMCx_DMA_ACTIVE==0), disable packet DMA */
            active = soc_pci_read(unit, CMIC_CMCx_DMA_STAT_OFFSET(cmc)) & DS_CMCx_DMA_ACTIVE(chan);
            if (!active || timeout_state == abort_timeout_passed) {
                /* clear CMIC_CMCx_PKTDMA_CHy_CTRL.ABORT_DMA|DMA_EN in the same write to disable; it also clears statuses */
                if (!active) { /* remove the channel from the channels waited on */
                    chans_group_delete(&packet_channels, channel_iter--);
                } else {
                    LOG_ERROR(BSL_LS_SOC_DMA, (BSL_META_U(unit, "Failed to abort packet DMA in CMC %d channel %d, check with the design team\n"), cmc, chan));
                    ++nof_failures;
                }
                cmic_address = CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, chan);
                val = soc_pci_read(unit, cmic_address);
                soc_pci_write(unit, cmic_address, val & ~(PKTDMA_ENABLE | PKTDMA_ABORT));

                cmic_address = CMIC_CMCx_DMA_STAT_CLR_OFFSET(cmc);
                val = soc_pci_read(unit, cmic_address);
                soc_pci_write(unit, cmic_address, (val | DS_DESCRD_CMPLT_CLR(chan)));
                soc_pci_write(unit, cmic_address, val);
            }
        }

        /* for all s-bus DMA channels still not done */
        for (chans_group_iter_start(&sbus_channels, &channel_iter); !chans_group_is_end(&sbus_channels, channel_iter); ++channel_iter) {
            chans_group_iter_t_get(channel_iter, &cmc, &chan); /* get the channel to work on */
            /* If the abort is done (CMIC_CMCx_SBUSDMA_CHy_STATUS.DONE==1), then clear the operation */
            done = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc, chan)) & ST_CMCx_SBUSDMA_DONE;
            if (done || timeout_state == abort_timeout_passed) {
                /* clear CMIC_CMCx_SBUSDMA_CHy_CONTROL.ABORT in the same write disables the original operation and abort, and clears statuses */
                cmic_address = CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, chan);
                val = soc_pci_read(unit, cmic_address);
                soc_pci_write(unit, cmic_address, val & ~(SBUSDMA_ABORT));
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
            /* If the abort is done (CMIC_CMCx_FIFO_CHy_RD_DMA_STAT.DONE==1), then clear the operation and disable */
            done = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_OFFSET(cmc, chan)) & ST_CMCx_FIFODMA_DONE;
            if (done || timeout_state == abort_timeout_passed) {
                /* clear CMIC_CMCx_FIFO_CHy_RD_DMA_CFG.ABORT|ENABLE in the same write: disables and clears statuses */
                cmic_address = CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, chan);
                val = soc_pci_read(unit, cmic_address);
                soc_pci_write(unit, cmic_address, val & ~(FIFODMA_ABORT | FIFODMA_ENABLE));
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
            /* If the abort is done (CMIC_CMCx_CCM_DMA_STAT.DONE==1), then clear the operation */
            done = soc_pci_read(unit, CMIC_CMCx_CCM_DMA_STAT_OFFSET(cmc)) & ST_CMCx_CCMDMA_DONE;
            if (done || timeout_state == abort_timeout_passed) {
                /* clear CMIC_CMCx_CCM_DMA_CFG_OFFSET.ABORT|EN in the same write: disables and clears statuses */
                cmic_address = CMIC_CMCx_CCM_DMA_CFG_OFFSET(cmc);
                val = soc_pci_read(unit, cmic_address);
                soc_pci_write(unit, cmic_address, val & ~(CCMDMA_ABORT | CCMDMA_ENABLE));
                if (done) { /* remove the channel from the channels waited on */
                    chans_group_delete(&ccm_channels, channel_iter--);
                } else {
                    LOG_ERROR(BSL_LS_SOC_DMA, (BSL_META_U(unit, "Failed to abort CCM DMA in CMC %d channel %d, check with the design team\n"), cmc, chan));
                    ++nof_failures;
                }
            }
        }

        /* for all SCHAN modules still not done */
        for (chans_group_iter_start(&schan_modules, &channel_iter); !chans_group_is_end(&schan_modules, channel_iter); ++channel_iter) {
            chans_group_iter_t_get(channel_iter, &cmc, &chan); /* get the channel to work on */
            /* If the abort is done (CMIC_COMMON_CMCx_SCHAN_CTRL.MSG_DONE==1), then clear the operation */
            cmic_address = CMIC_COMMON_CMCx_SCHAN_CTRL_OFFSET(cmc);
            done = soc_pci_read(unit, cmic_address) & SC_CMCx_MSG_DONE;
            if (done || timeout_state == abort_timeout_passed) {
                /* clear CMIC_COMMON_CMCx_SCHAN_CTRL.ABORT|START in the same write: disables and clears statuses*/
                val = soc_pci_read(unit, cmic_address);
                soc_pci_write(unit, cmic_address, val & ~(SC_CMCx_SCHAN_ABORT));
                if (done) { /* remove the channel from the channels waited on */
                    chans_group_delete(&schan_modules, channel_iter--);
                } else {
                    LOG_ERROR(BSL_LS_SOC_DMA, (BSL_META_U(unit, "Failed to abort s-chan FIFO in channel %d, check with the design team\n"), chan));
                    ++nof_failures;
                }
            }
        }


        if (timeout_state != abort_timeout_not_passed || (!chans_group_get_nof_chans(&packet_channels) &&
            !chans_group_get_nof_chans(&sbus_channels) && !chans_group_get_nof_chans(&fifo_channels) &&
            !chans_group_get_nof_chans(&ccm_channels) && !chans_group_get_nof_chans(&schan_modules))) {
            break; /* exist if the timeout passed at the start of the iteration, or if there is no channel left to wait for */
        }
        sal_usleep(timeout_sleep); /* wait before the next iteration */
        timeout_sleep += CMICM_INITIAL_SLEEP; /* next time sleep more, will use less than sqrt(2 * timeout / CMICM_INITIAL_SLEEP) iterations */
    } while (1);

    LOG_INFO(BSL_LS_SOC_DMA, (BSL_META_U(unit,
      "Aborted and disabled %d DMA channels with %d abort failures\n"), nof_channels, nof_failures));

    return nof_failures ? SOC_E_TIMEOUT : SOC_E_NONE;
}

#endif /* BCM_CMICM_SUPPORT  */
