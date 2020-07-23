/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: CMICX L2MOD Driver
 */

#include <shared/bsl.h>
#include <shared/alloc.h>
#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/l2x.h>
#include <soc/fifodma_internal.h>

#if defined(BCM_CMICX_SUPPORT) && defined(BCM_FIFODMA_SUPPORT)
#include <soc/cmicx.h>
#include <soc/intr_cmicx.h>
#endif

#if defined(BCM_ESW_SUPPORT) && defined(BCM_CMICX_SUPPORT)

STATIC int
_soc_td3_l2mod_sbus_fifo_enable(int unit, int enable)
{
    uint32 rval = 0;

    soc_reg_field_set(unit, L2_MOD_FIFO_ENABLEr, &rval, L2_DELETEf, enable);
    soc_reg_field_set(unit, L2_MOD_FIFO_ENABLEr, &rval, L2_LEARNf, enable);
    soc_reg_field_set(unit, L2_MOD_FIFO_ENABLEr, &rval, L2_INSERTf, enable);

    SOC_IF_ERROR_RETURN(WRITE_L2_MOD_FIFO_ENABLEr(unit, rval));
    return SOC_E_NONE;
}

#if defined(BCM_FIRELIGHT_SUPPORT)
STATIC int
_soc_fl_l2mod_sbus_fifo_enable(int unit, int enable)
{
    uint32 rval = 0;

    soc_reg_field_set(unit, AUX_ARB_CONTROLr, &rval, L2_MOD_FIFO_ENABLE_AGEf, enable);
    soc_reg_field_set(unit, AUX_ARB_CONTROLr, &rval, L2_MOD_FIFO_ENABLE_LEARNf, enable);
    soc_reg_field_set(unit, AUX_ARB_CONTROLr, &rval, L2_MOD_FIFO_ENABLE_L2_DELETEf, enable);
    soc_reg_field_set(unit, AUX_ARB_CONTROLr, &rval, L2_MOD_FIFO_ENABLE_L2_INSERTf, enable);

    SOC_IF_ERROR_RETURN(WRITE_AUX_ARB_CONTROLr(unit, rval));
    return SOC_E_NONE;
}
#endif /* BCM_FIRELIGHT_SUPPORT */

extern uint32 soc_mem_fifo_delay_value;

STATIC void
_soc_cmicx_l2mod_sbus_fifo_dma_thread(void *unit_vp)
{
    uint8 overflow, timeout;
    int i, count, handled_count, adv_threshold;
    int unit = PTR_TO_INT(unit_vp);
    int cmc = SOC_PCI_CMC(unit);
    soc_control_t *soc = SOC_CONTROL(unit);
    uint32 entries_per_buf, rval;
    int rv, interval, non_empty;
    uint8 ch;
    uint32 *buff_max;
    void *host_entry, *host_buff;
    int entry_words;
    uint32 hostmem_timeout, hostmem_overflow, done, error;

    if (SOC_IS_TRIDENT3X(unit)) {
        /*
         * Channel 0 is for X pipe
         * Channel 1 is for Y pipe but nothing will go into Y pipe L2_MOD_FIFO.
         */
        ch = SOC_MEM_FIFO_DMA_CHANNEL_0;
    } else {
        ch = SOC_MEM_FIFO_DMA_CHANNEL_1;
    }

    entries_per_buf = soc_property_get(unit, spn_L2XMSG_HOSTBUF_SIZE, 1024);
    adv_threshold = entries_per_buf / 2;

    entry_words = soc_mem_entry_words(unit, L2_MOD_FIFOm);
    host_buff = soc_cm_salloc(unit, entries_per_buf * entry_words * sizeof(uint32),
                             "L2_MOD DMA Buffer");
    if (host_buff == NULL) {
        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR,
                           SOC_SWITCH_EVENT_THREAD_L2MOD_DMA, __LINE__,
                           SOC_E_MEMORY);
        goto cleanup_exit;
    }

    soc_fifodma_stop(unit, ch);

    rv = soc_fifodma_masks_get(unit, &hostmem_timeout, &hostmem_overflow, &error, &done);

    rv = soc_fifodma_start(unit, ch, TRUE, L2_MOD_FIFOm, 0, MEM_BLOCK_ANY, 0,
                                      entries_per_buf, host_buff);
    if (SOC_FAILURE(rv)) {
        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR,
                           SOC_SWITCH_EVENT_THREAD_L2MOD_DMA, __LINE__, rv);
        goto cleanup_exit;
    }

    host_entry = host_buff;
    buff_max = (uint32 *)host_entry + (entries_per_buf * entry_words);

    if (!soc_feature(unit, soc_feature_fifo_dma)) {
        soc_fifodma_intr_enable(unit, ch);
    }

#if defined(BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_FIRELIGHT(unit)) {
        _soc_fl_l2mod_sbus_fifo_enable(unit, 1);
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    if (SOC_IS_TRIDENT3X(unit)) {
        _soc_td3_l2mod_sbus_fifo_enable(unit, 1);
    }

    while ((interval = soc->l2x_interval)) {
        overflow = 0; timeout = 0;
        if (soc->l2modDmaIntrEnb) {
            soc_fifodma_intr_enable(unit, ch);
            if (sal_sem_take(soc->arl_notify, interval) < 0) {
                LOG_VERBOSE(BSL_LS_SOC_INTR,
                            (BSL_META_U(unit,
                                        "%s polling timeout soc_mem_fifo_delay_value=%d\n"),
                             soc->l2x_name, soc_mem_fifo_delay_value));
            } else {
                LOG_VERBOSE(BSL_LS_SOC_INTR,
                            (BSL_META_U(unit,
                                        "%s woken up soc_mem_fifo_delay_value=%d\n"),
                             soc->l2x_name, soc_mem_fifo_delay_value));
                /* check for timeout or overflow and either process or continue */
                soc_fifodma_status_get(unit, ch, &rval);
                timeout = rval & hostmem_timeout;
                if (!timeout) {
                    /* overflow = soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STATr,
                                                 rval, HOSTMEM_OVERFLOWf); */
                    overflow = rval & hostmem_overflow;
                    timeout |= overflow;
                }
            }

        } else {
            sal_usleep(interval);
        }

        handled_count = 0;

        do {
            non_empty = FALSE;

            /* get entry count, process if nonzero else continue */
            rv = soc_fifodma_num_entries_get(unit, ch, &count);
            if (SOC_SUCCESS(rv)) {
                non_empty = TRUE;
                if (count > adv_threshold) {
                    count = adv_threshold;
                }
                /* Invalidate DMA memory to read */
                if (((uint32 *)host_entry + count * entry_words) > buff_max) {
                    /* roll-over cases */
                    soc_cm_sinval(unit, host_entry,
                        (buff_max - (uint32 *)host_entry) * sizeof(uint32));
                    soc_cm_sinval(unit, host_buff,
                        ((count * entry_words) - (buff_max - (uint32 *)host_entry)) * sizeof(uint32));
                }
                else {
                    soc_cm_sinval(unit, host_entry, (count * entry_words) * sizeof(uint32));
                }
                for (i = 0; i < count; i++) {
                    if(!soc->l2x_interval) {
                        goto cleanup_exit;
                    }
#if defined(BCM_FIRELIGHT_SUPPORT)
                    if (SOC_IS_FIRELIGHT(unit)) {
                        _soc_hu2_l2mod_fifo_process(unit, soc->l2x_flags, host_entry);
                    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
                    {
#if defined(BCM_TRIDENT2_SUPPORT)
                        _soc_td2_l2mod_fifo_process(unit, soc->l2x_flags, host_entry);
#endif
                    }
                    host_entry = (uint32 *)host_entry + entry_words;
                    /* handle roll over */
                    if ((uint32 *)host_entry >= buff_max) {
                        host_entry = host_buff;
                    }

                    handled_count++;
                    /*
                     * PPA may wait for available space in mod_fifo, lower the
                     * threshold when ppa is running, therefore read point can
                     * be updated sooner.
                     */
                    if (SOC_CONTROL(unit)->l2x_ppa_in_progress && i >= 63) {
                        i++;
                        break;
                    }
                }
                soc_fifodma_set_entries_read(unit, ch, i);
            }

            /* check and clear error */
            soc_fifodma_status_get(unit, ch, &rval);
            if (rval & done) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "FIFO DMA engine terminated for cmc[%d]:ch[%d]\n"),
                           cmc, ch));
                if (rval & error) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "FIFO DMA engine encountered error: [0x%x]\n"),
                               rval));
                }
                goto cleanup_exit;
            }

            if (!SOC_CONTROL(unit)->l2x_ppa_in_progress) {
                /* For some operating systems, sal_thread_yield is
                 * not sufficient to yield CPU resource to other lower
                 * priority thread
                 */
                if ((handled_count * 2) >= adv_threshold) {
                    sal_usleep(1000);
                    handled_count = 0;
                }
            }
        } while (non_empty);
        /* Clearing of the FIFO_CH_DMA_INT interrupt by resetting
           overflow & timeout status in FIFO_CHy_RD_DMA_STAT_CLR reg */
        if (timeout) {
            soc_fifodma_status_clear(unit, ch);
        }
    }

cleanup_exit:
    soc_fifodma_stop(unit, ch);

#if defined(BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_FIRELIGHT(unit)) {
        _soc_fl_l2mod_sbus_fifo_enable(unit, 0);
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    if (SOC_IS_TRIDENT3X(unit)) {
        _soc_td3_l2mod_sbus_fifo_enable(unit, 0);
    }

    if (host_buff != NULL) {
        soc_cm_sfree(unit, host_buff);
    }

    soc->l2x_pid = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}

/*
 * Function:
 *     soc_l2mod_running
 * Purpose:
 *     Determine the L2MOD sync thread running parameters
 * Parameters:
 *     unit - unit number.
 *     flags (OUT) - if non-NULL, receives the current flag settings
 *     interval (OUT) - if non-NULL, receives the current pass interval
 * Returns:
 *     Boolean; TRUE if L2MOD sync thread is running
 */

int
_soc_cmicx_l2mod_running(int unit, uint32 *flags, sal_usecs_t *interval)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    soc_control_t *soc = SOC_CONTROL(unit);

    if (SOC_IS_XGS3_SWITCH(unit)) {
        if (soc->l2x_pid != SAL_THREAD_ERROR) {
            if (flags != NULL) {
                *flags = soc->l2x_flags;
            }
            if (interval != NULL) {
                *interval = soc->l2x_interval;
            }
        }

        return (soc->l2x_pid != SAL_THREAD_ERROR);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return SOC_E_UNAVAIL;
}

/*
 * Function:
 *     _soc_cmicx_l2mod_stop
 * Purpose:
 *     Stop cmicx L2MOD-related thread
 * Parameters:
 *     unit - unit number.
 * Returns:
 *     SOC_E_XXX
 */
int
_soc_cmicx_l2mod_stop(int unit)
{
    soc_control_t * soc = SOC_CONTROL(unit);
    int rv = SOC_E_NONE;
    uint8 ch = SOC_MEM_FIFO_DMA_CHANNEL_0;

    if (SOC_IS_XGS3_SWITCH(unit)) {
        LOG_INFO(BSL_LS_SOC_ARL,
                 (BSL_META_U(unit,
                             "soc_cmicx_l2mod_stop: unit=%d\n"), unit));

#if defined(BCM_FIRELIGHT_SUPPORT)
        if (SOC_IS_FIRELIGHT(unit)) {
            _soc_fl_l2mod_sbus_fifo_enable(unit, 0);
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        if (SOC_IS_TRIDENT3X(unit)) {
            _soc_td3_l2mod_sbus_fifo_enable(unit, 0);
        }

        if (!soc_feature(unit, soc_feature_fifo_dma) ||
            SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
            soc_fifodma_intr_disable(unit, ch);
            soc->l2x_interval = 0;  /* Request exit */
        }

        if (soc->l2modDmaIntrEnb) {
            /* Wake up thread so it will check the exit flag */
            if(soc->arl_notify) {
                sal_sem_give(soc->arl_notify);
            }
        }

        return rv;
    }
    return SOC_E_UNAVAIL;
}

int
_soc_cmicx_l2mod_start(int unit, uint32 flags, sal_usecs_t interval)
{
    soc_control_t * soc = SOC_CONTROL(unit);
    int             pri;

    if (!soc_feature(unit, soc_feature_arl_hashed)) {
        return SOC_E_UNAVAIL;
    }

    if (soc->l2x_interval != 0) {
        SOC_IF_ERROR_RETURN(_soc_cmicx_l2mod_stop(unit));
    }

    sal_snprintf(soc->l2x_name, sizeof(soc->l2x_name), "bcmL2MOD.%d", unit);

    soc->l2x_flags = flags;
    soc->l2x_interval = interval;

    if (interval == 0) {
        return SOC_E_NONE;
    }

    if (soc->l2x_pid == SAL_THREAD_ERROR) {
        pri = soc_property_get(unit, spn_L2XMSG_THREAD_PRI, 50);

        soc->l2x_pid =
                sal_thread_create(soc->l2x_name, SAL_THREAD_STKSZ, pri,
                                  _soc_cmicx_l2mod_sbus_fifo_dma_thread, INT_TO_PTR(unit));
        if (soc->l2x_pid == SAL_THREAD_ERROR) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_cmicx_l2mod_start: Could not start L2MOD thread\n")));
            return SOC_E_MEMORY;
        }
    }

    return SOC_E_NONE;
}

int
_soc_cmicx_l2mod_sbus_fifo_field_set(int unit, soc_field_t field, int enable)
{
    uint32 rval = 0;
    uint32 fval = enable?1:0;

#if defined(BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_FIRELIGHT(unit)) {
        if(soc_reg_field_valid(unit, AUX_ARB_CONTROLr, field)) {
            SOC_IF_ERROR_RETURN(READ_AUX_ARB_CONTROLr(unit, &rval));

            soc_reg_field_set(unit, AUX_ARB_CONTROLr, &rval, field, fval);

            SOC_IF_ERROR_RETURN(WRITE_AUX_ARB_CONTROLr(unit, rval));
        }
        return SOC_E_NONE;
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    if (SOC_IS_TRIDENT3X(unit)) {
        if(soc_reg_field_valid(unit, L2_MOD_FIFO_ENABLEr, field)) {
            SOC_IF_ERROR_RETURN(READ_L2_MOD_FIFO_ENABLEr(unit, &rval));

            soc_reg_field_set(unit, L2_MOD_FIFO_ENABLEr, &rval, field, fval);

            SOC_IF_ERROR_RETURN(WRITE_L2_MOD_FIFO_ENABLEr(unit, rval));
        }
        return SOC_E_NONE;
    }

    return SOC_E_UNAVAIL;
}

int
_soc_cmicx_l2mod_sbus_fifo_field_get(int unit, soc_field_t field, int *enable)
{
    uint32 reg = 0;
    uint32 fval = 0;

#if defined(BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_FIRELIGHT(unit)) {
        if(soc_reg_field_valid(unit, AUX_ARB_CONTROLr, field)) {
            SOC_IF_ERROR_RETURN(READ_AUX_ARB_CONTROLr(unit, &reg));

            fval = soc_reg_field_get(unit, AUX_ARB_CONTROLr, reg, field);
        }
        *enable = fval?1:0;
        return SOC_E_NONE;
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    if (SOC_IS_TRIDENT3X(unit)) {
        if(soc_reg_field_valid(unit, L2_MOD_FIFO_ENABLEr, field)) {
            SOC_IF_ERROR_RETURN(READ_L2_MOD_FIFO_ENABLEr(unit, &reg));

            fval = soc_reg_field_get(unit, L2_MOD_FIFO_ENABLEr, reg, field);
        }
        *enable = fval?1:0;
        return SOC_E_NONE;
    }

    return SOC_E_UNAVAIL;
}

#endif /* BCM_ESW_SUPPORT && BCM_CMICX_SUPPORT */

