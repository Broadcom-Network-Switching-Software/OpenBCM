/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Tomahawk switch doesn't completely support the hardware based l2 aging 
 * mechanism. Although it provides the l2 aging function, it doesn't have
 * a periodic timer to trigger the function. 
 * This file implements a timer thread to support the software triggered 
 * L2 aging. The thread periodically performs the aging function through 
 * the L2_BULK_CONTROL register at the user configured time interval.
 * The aging action checkes the hit bit of each entry in the l2 table.
 * If the hit bit is not set, the entry is deleted. If it is set, the
 * hit bit is cleared and the entry remains. Only static entries are 
 * excluded from l2 aging.
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/l2x.h>
#include <soc/ptable.h>
#include <soc/debug.h>
#include <soc/util.h>
#include <soc/mem.h>
#include <soc/tomahawk.h>
#include <bcm_int/esw/l2.h>

#ifdef BCM_XGS_SWITCH_SUPPORT

#define _SOC_TH_L2_BULK_BURST_MIN  0

static int _soc_th_l2_bulk_age_iter[SOC_MAX_NUM_DEVICES] = {0};
STATIC int
_soc_th_l2_bulk_age_entries_delete(int unit, int *retry)
{
    int rv;
    l2_bulk_entry_t match_mask;
    l2_bulk_entry_t match_data;
    l2_bulk_entry_t new_mask;
    l2_bulk_entry_t new_data;
    uint32 rval = 0;
    uint32 start = 0;
    uint32 complete = 0;
    soc_field_t valid_f = VALIDf;
    if (soc_feature(unit, soc_feature_base_valid)) {
        valid_f = BASE_VALIDf;
    }

    *retry = 0;
    sal_memset(&match_mask, 0, sizeof(match_mask));
    sal_memset(&match_data, 0, sizeof(match_data));

    soc_mem_field32_set(unit, L2_BULKm, &match_mask, valid_f, 1);
    soc_mem_field32_set(unit, L2_BULKm, &match_data, valid_f, 1);

    soc_mem_field32_set(unit, L2_BULKm, &match_mask, STATIC_BITf, 1);
    soc_mem_field32_set(unit, L2_BULKm, &match_mask, HITSAf, 1);
    soc_mem_field32_set(unit, L2_BULKm, &match_mask, HITDAf, 1);

    /* The mesh-bit-set entries are exclueded. */
    if (soc_mem_field_valid(unit, L2_BULKm, MESHf)) {
        soc_mem_field32_set(unit, L2_BULKm, &match_mask, MESHf, 1);
    } else if (soc_mem_field_valid(unit, L2_BULKm, EVPN_AGE_DISABLEf)) {
        /* EVPN_AGE_DISABLEf overlaid on MESHf. */
        soc_mem_field32_set(unit, L2_BULKm, &match_mask, EVPN_AGE_DISABLEf, 1);
    }

    sal_memset(&new_data, 0, sizeof(l2_bulk_entry_t));
    sal_memset(&new_mask, 0, sizeof(l2_bulk_entry_t));

    soc_mem_lock(unit, L2Xm);
    rv = READ_L2_BULK_CONTROLr(unit, &rval);
    start = soc_reg_field_get(unit, L2_BULK_CONTROLr, rval, STARTf);
    complete = soc_reg_field_get(unit, L2_BULK_CONTROLr, rval, COMPLETEf);

    if ((start == 1) && (complete == 0)) {
        *retry = 1;
        soc_mem_unlock(unit, L2Xm);
        return SOC_E_BUSY;
    }

    if (SOC_CONTROL(unit)->l2x_mode == L2MODE_FIFO) {
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, L2_MOD_FIFO_RECORDf, 1);
        soc_mem_field32_set(unit, L2_BULKm, &new_mask, valid_f, 1);
    } else {
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, L2_MOD_FIFO_RECORDf, 0);
        sal_memset(&new_mask, 0xff, sizeof(l2_bulk_entry_t));
    }
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 2);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, BURST_ENTRIESf, 
                      _SOC_TH_L2_BULK_BURST_MIN);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 
                      soc_mem_index_count(unit, L2Xm));

    /* write to bulk mask entry, valid bit and static bit */
    rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL,
                        _BCM_L2_BULK_MATCH_MASK_INX, &match_mask);
    /* write to bulk data entry, valid=1 and static=0 */
    rv |= WRITE_L2_BULKm(unit, MEM_BLOCK_ALL,
                         _BCM_L2_BULK_MATCH_DATA_INX, &match_data);
    rv |= WRITE_L2_BULKm(unit, MEM_BLOCK_ALL,
                         _BCM_L2_BULK_REPLACE_DATA_INX, &new_data);
    rv |= WRITE_L2_BULKm(unit, MEM_BLOCK_ALL,
                         _BCM_L2_BULK_REPLACE_MASK_INX, &new_mask);
    if (SOC_SUCCESS(rv)) {
        rv = WRITE_L2_BULK_CONTROLr(unit, rval);
    }
    if (SOC_SUCCESS(rv)) {
        rv = soc_l2x_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
        if (rv == SOC_E_TIMEOUT) {
            *retry = 1;
        }
    }
    soc_mem_unlock(unit, L2Xm);
    return rv;
}

STATIC int
_soc_th_l2_bulk_age_entries_hit_clear(int unit, int *retry)
{
    int rv;
    l2_bulk_entry_t match_mask;
    l2_bulk_entry_t match_data;
    l2_bulk_entry_t new_mask;
    l2_bulk_entry_t new_data;
    uint32 rval = 0;
    uint32 start = 0;
    uint32 complete = 0;
    soc_field_t valid_f = VALIDf;
    if (soc_feature(unit, soc_feature_base_valid)) {
        valid_f = BASE_VALIDf;
    }

    *retry = 0;
    sal_memset(&match_mask, 0, sizeof(match_mask));
    sal_memset(&match_data, 0, sizeof(match_data));

    /* Match VALIDf=1, HITSAf=1, HITDAf=1 */
    /* Set match data */
    sal_memset(&match_data, 0, sizeof(l2_bulk_entry_t));
    soc_mem_field32_set(unit, L2_BULKm, &match_data, valid_f, 1);
    
    /* Set match mask */
    sal_memset(&match_mask, 0, sizeof(l2_bulk_entry_t));
    soc_mem_field32_set(unit, L2_BULKm, &match_mask, valid_f, 1);

    sal_memset(&new_data, 0, sizeof(l2_bulk_entry_t));
    /* Clear HIT bits */
    sal_memset(&new_mask, 0, sizeof(l2_bulk_entry_t));
    soc_mem_field32_set(unit, L2_BULKm, &new_mask, HITSAf, 1);
    soc_mem_field32_set(unit, L2_BULKm, &new_mask, HITDAf, 1);
    soc_mem_field32_set(unit, L2_BULKm, &new_mask, LOCAL_SAf, 1);

    soc_mem_lock(unit, L2Xm);
    rv = READ_L2_BULK_CONTROLr(unit, &rval);
    start = soc_reg_field_get(unit, L2_BULK_CONTROLr, rval, STARTf);
    complete = soc_reg_field_get(unit, L2_BULK_CONTROLr, rval, COMPLETEf);

    if ((start == 1) && (complete == 0)) {
        *retry = 1;
        soc_mem_unlock(unit, L2Xm);
        return SOC_E_BUSY;
    }

    if (SOC_CONTROL(unit)->l2x_mode == L2MODE_FIFO) {
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, L2_MOD_FIFO_RECORDf, 0);
    }
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 2);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, BURST_ENTRIESf, 
                      _SOC_TH_L2_BULK_BURST_MIN);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 
                      soc_mem_index_count(unit, L2Xm));

    rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL,
                        _BCM_L2_BULK_MATCH_MASK_INX, &match_mask);
    rv |= WRITE_L2_BULKm(unit, MEM_BLOCK_ALL,
                         _BCM_L2_BULK_MATCH_DATA_INX, &match_data);
    rv |= WRITE_L2_BULKm(unit, MEM_BLOCK_ALL,
                         _BCM_L2_BULK_REPLACE_DATA_INX, &new_data);
    rv |= WRITE_L2_BULKm(unit, MEM_BLOCK_ALL,
                         _BCM_L2_BULK_REPLACE_MASK_INX, &new_mask);
    if (SOC_SUCCESS(rv)) {
        rv = WRITE_L2_BULK_CONTROLr(unit, rval);
    }
    if (SOC_SUCCESS(rv)) {
        rv = soc_l2x_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
        if (rv == SOC_E_TIMEOUT) {
            *retry = 1;
        }
    }
    soc_mem_unlock(unit, L2Xm);
    return rv;
}

/*
 * Function:
 * 	_soc_tr3_l2_bulk_age
 * Purpose:
 *   	l2 bulk age thread
 * Parameters:
 *	unit - unit number.
 * Returns:
 *	none
 */
STATIC void
_soc_th_l2_bulk_age(void *unit_vp)
{
    int unit = PTR_TO_INT(unit_vp);
    int c, m, r, rv, iter = 0, retry = 0;
    soc_control_t *soc = SOC_CONTROL(unit);
    sal_usecs_t interval;
    sal_usecs_t stime, etime;

    while((interval = soc->l2x_age_interval) != 0) {
        if (!iter) {
            goto age_delay;
        }
        LOG_VERBOSE(BSL_LS_SOC_ARL,
                    (BSL_META_U(unit,
                                "l2_bulk_age_thread: "
                                "Process iters(total:%d, this run:%d\n"),
                     ++_soc_th_l2_bulk_age_iter[unit], iter));
        stime = sal_time_usecs();

        if (!soc->l2x_age_enable) {
            goto age_delay;
        }

        if (soc_mem_index_count(unit, L2Xm) == 0) {
            goto cleanup_exit;
        }

        rv = _soc_th_l2_bulk_age_entries_delete(unit, &retry);

        if (!SOC_SUCCESS(rv)) {
            if (retry) {
                goto age_delay;
            }
            goto cleanup_exit;
        }
        rv = _soc_th_l2_bulk_age_entries_hit_clear(unit, &retry);

        if (!SOC_SUCCESS(rv)) {
            if (retry) {
                goto age_delay;
            }
            goto cleanup_exit;
        }
        etime = sal_time_usecs();
        LOG_VERBOSE(BSL_LS_SOC_ARL,
                    (BSL_META_U(unit,
                                "l2_bulk_age_thread: unit=%d: done in %d usec\n"),
                     unit, SAL_USECS_SUB(etime, stime)));
age_delay:
        rv = -1; /* timeout */
        if (interval > 2147) {
            m = (interval / 2147) * 1000;
            r = (interval % 2147) * 1000000;
            for (c = 0; c < m; c++) {
                rv = sal_sem_take(soc->l2x_age_notify, 2147000);
                /* age interval is changed */
                if (rv == 0 || interval != soc->l2x_age_interval) {
                    break;
                }
            }
            if (soc->l2x_age_interval) {
                if (rv == 0 || interval != soc->l2x_age_interval) {
                    /* age interval is changed */
                    interval = soc->l2x_age_interval;
                    goto age_delay;
                } else if (r) {
                     /* age interval is not changed */
                    (void)sal_sem_take(soc->l2x_age_notify, r);
                }
            }
        } else {
            rv = sal_sem_take(soc->l2x_age_notify, interval * 1000000);
            /* age interval is changed */
            if (soc->l2x_age_interval && (rv == 0 || interval != soc->l2x_age_interval)) {
                interval = soc->l2x_age_interval;
                goto age_delay;
            }
        }
        iter++;
    }

cleanup_exit:
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "l2_bulk_age_thread: exiting\n")));
    soc->l2x_age_pid = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}

/*
 * Function:
 * 	soc_th_l2_bulk_age_start
 * Purpose:
 *   	Start l2 bulk age thread
 * Parameters:
 *	unit - unit number.
 * Returns:
 *	SOC_E_XXX
 */
int
soc_th_l2_bulk_age_start(int unit, int interval)
{
    int cfg_interval;
    soc_control_t *soc = SOC_CONTROL(unit);

    cfg_interval = soc_property_get(unit, spn_L2_SW_AGING_INTERVAL, 
                                    SAL_BOOT_QUICKTURN ? 30 : 10);
    SOC_CONTROL_LOCK(unit);
    soc->l2x_age_interval = interval ? interval : cfg_interval;
    sal_snprintf(soc->l2x_age_name, sizeof (soc->l2x_age_name),
                 "bcmL2age.%d", unit);
    soc->l2x_age_pid = sal_thread_create(soc->l2x_age_name, SAL_THREAD_STKSZ,
                                         soc_property_get(unit, spn_L2AGE_THREAD_PRI, 50),
                                         _soc_th_l2_bulk_age, INT_TO_PTR(unit));
    if (soc->l2x_age_pid == SAL_THREAD_ERROR) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "bcm_esw_l2_init: Could not start L2 bulk age thread\n")));
        SOC_CONTROL_UNLOCK(unit);
        return SOC_E_MEMORY;
    }
    SOC_CONTROL_UNLOCK(unit);
    return SOC_E_NONE;
}

/*
 * Function:
 * 	soc_th_l2_bulk_age_stop
 * Purpose:
 *   	Stop l2 bulk age thread
 * Parameters:
 *	unit - unit number.
 * Returns:
 *	SOC_E_XXX
 */
int
soc_th_l2_bulk_age_stop(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int           rv = SOC_E_NONE;
    soc_timeout_t to;

    SOC_CONTROL_LOCK(unit);
    soc->l2x_age_interval = 0;  /* Request exit */
    SOC_CONTROL_UNLOCK(unit);

    if (soc->l2x_age_pid && (soc->l2x_age_pid != SAL_THREAD_ERROR)) {
        /* Wake up thread so it will check the exit flag */
        sal_sem_give(soc->l2x_age_notify);

        /* Give thread a few seconds to wake up and exit */
        if (SAL_BOOT_SIMULATION) {
            soc_timeout_init(&to, 300 * 1000000, 0);
        } else {
            soc_timeout_init(&to, 60 * 1000000, 0);
        }

        while (soc->l2x_age_pid != SAL_THREAD_ERROR) {
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "thread will not exit\n")));
                rv = SOC_E_INTERNAL;
                break;
            }
        }
    }
    return rv;
}

#endif /* BCM_XGS_SWITCH_SUPPORT */
