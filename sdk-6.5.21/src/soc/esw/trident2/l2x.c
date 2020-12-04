/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * XGS TD2 Bulk Aging Management.
 */

#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/l2x.h>
#include <soc/ptable.h>
#include <soc/debug.h>
#include <soc/util.h>
#include <soc/mem.h>

#ifdef BCM_XGS_SWITCH_SUPPORT
#ifdef BCM_TRIDENT2_SUPPORT

#define _SOC_IF_ERROR_UNLOCK_EXIT(rv) \
        if (!SOC_SUCCESS(rv)) { \
            soc_mem_unlock(unit, L2Xm); \
            sal_sem_give(soc->l2x_age_sync); \
            LOG_ERROR(BSL_LS_SOC_L2, \
                      (BSL_META_U(unit, \
                                  "rv: %d\n"), rv)); \
            goto thread_exit; \
        }

static int _soc_td2_l2_bulk_age_iter[SOC_MAX_NUM_DEVICES] = {0};

/*
 * Function:
 *  _soc_td2_l2_bulk_age
 * Purpose:
 *  l2 bulk age thread
 * Parameters:
 *  unit - unit number.
 * Returns:
 *  none
 */
STATIC void
_soc_td2_l2_bulk_age(void *unit_vp)
{
    int unit = PTR_TO_INT(unit_vp);
    soc_control_t *soc = SOC_CONTROL(unit);
    sal_usecs_t interval, cfg_interval, stime, etime;
    l2x_entry_t match_data;
    l2x_entry_t match_mask;
    l2x_entry_t new_data;
    l2x_entry_t new_mask;
    uint32 rval;
    int i, rv, iter = 0;

    cfg_interval = soc_property_get(unit, spn_L2_SW_AGING_INTERVAL, 300);

    while ((interval = soc->l2x_age_interval) != 0) {
        if (!iter) {
            goto age_delay;
        }

        LOG_VERBOSE(BSL_LS_SOC_ARL,
                    (BSL_META_U(unit,
                                "l2_bulk_age_thread: "
                                "Process iters(total:%d, this run:%d\n"),
                     ++_soc_td2_l2_bulk_age_iter[unit], iter));

        stime = sal_time_usecs();

        if (!soc->l2x_age_enable) {
            goto age_delay;
        }

        sal_sem_take(soc->l2x_age_sync, sal_sem_FOREVER);
        soc_mem_lock(unit, L2Xm);

        /* Match VALIDf=1, STATICf=0, HITSAf=0, HITDAf=0 */
        /* Set match data */
        sal_memset(&match_data, 0, sizeof(l2x_entry_t));
        soc_mem_field32_set(unit, L2Xm, &match_data, VALIDf, 1);

        /* Set match mask */
        sal_memset(&match_mask, 0, sizeof(l2x_entry_t));
        soc_mem_field32_set(unit, L2Xm, &match_mask, VALIDf, 1);
        soc_mem_field32_set(unit, L2Xm, &match_mask, STATIC_BITf, 1);
        soc_mem_field32_set(unit, L2Xm, &match_mask, HITSAf, 1);
        soc_mem_field32_set(unit, L2Xm, &match_mask, HITDAf, 1);
        if (soc_mem_field_valid(unit, L2Xm, EVPN_AGE_DISABLEf)) {
            /* Overlaid on MESHf. */
            soc_mem_field32_set(unit, L2Xm, &match_mask, EVPN_AGE_DISABLEf, 1);
        }

        _SOC_IF_ERROR_UNLOCK_EXIT
            (WRITE_L2_BULK_MATCH_DATAm(unit, MEM_BLOCK_ALL, 0, &match_data));
        _SOC_IF_ERROR_UNLOCK_EXIT
            (WRITE_L2_BULK_MATCH_MASKm(unit, MEM_BLOCK_ALL, 0, &match_mask));

        sal_memset(&new_data, 0, sizeof(l2x_entry_t));
        if (SOC_CONTROL(unit)->l2x_mode == L2MODE_FIFO) { /* Clear VALIDf */
            sal_memset(&new_mask, 0, sizeof(l2x_entry_t));
            soc_mem_field32_set(unit, L2Xm, &new_mask, VALIDf, 1);
        } else { /* Clear All Fields */
            sal_memset(&new_mask, 0xff, sizeof(l2x_entry_t));
        }

        _SOC_IF_ERROR_UNLOCK_EXIT
            (WRITE_L2_BULK_REPLACE_DATAm(unit, MEM_BLOCK_ALL, 0, &new_data));
        _SOC_IF_ERROR_UNLOCK_EXIT
            (WRITE_L2_BULK_REPLACE_MASKm(unit, MEM_BLOCK_ALL, 0, &new_mask));

        /* Set bulk control action */
        _SOC_IF_ERROR_UNLOCK_EXIT(READ_L2_BULK_CONTROLr(unit, &rval));
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 2);
        if (SOC_CONTROL(unit)->l2x_mode == L2MODE_FIFO) {
            soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, 
                              L2_MOD_FIFO_RECORDf, 1);
        }
        _SOC_IF_ERROR_UNLOCK_EXIT(WRITE_L2_BULK_CONTROLr(unit, rval));

        rv = soc_l2x_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
        if (rv == SOC_E_TIMEOUT) {
            goto age_exit;
        }
        _SOC_IF_ERROR_UNLOCK_EXIT(rv);

        /* Match VALIDf=1, HITSAf=1, HITDAf=1 */
        /* Set match data */
        sal_memset(&match_data, 0, sizeof(l2x_entry_t));
        soc_mem_field32_set(unit, L2Xm, &match_data, VALIDf, 1);

        /* Set match mask */
        sal_memset(&match_mask, 0, sizeof(l2x_entry_t));
        soc_mem_field32_set(unit, L2Xm, &match_mask, VALIDf, 1);
        /* soc_mem_field32_set(unit, L2Xm, &match_mask, STATIC_BITf, 1); */

        _SOC_IF_ERROR_UNLOCK_EXIT
            (WRITE_L2_BULK_MATCH_DATAm(unit, MEM_BLOCK_ALL, 0, &match_data));
        _SOC_IF_ERROR_UNLOCK_EXIT
            (WRITE_L2_BULK_MATCH_MASKm(unit, MEM_BLOCK_ALL, 0, &match_mask));

        sal_memset(&new_data, 0, sizeof(l2x_entry_t));
        /* Clear HIT bits */
        sal_memset(&new_mask, 0, sizeof(l2x_entry_t));
        soc_mem_field32_set(unit, L2Xm, &new_mask, HITSAf, 1);
        soc_mem_field32_set(unit, L2Xm, &new_mask, HITDAf, 1);
        soc_mem_field32_set(unit, L2Xm, &new_mask, LOCAL_SAf, 1);

        _SOC_IF_ERROR_UNLOCK_EXIT
            (WRITE_L2_BULK_REPLACE_DATAm(unit, MEM_BLOCK_ALL, 0, &new_data));
        _SOC_IF_ERROR_UNLOCK_EXIT
            (WRITE_L2_BULK_REPLACE_MASKm(unit, MEM_BLOCK_ALL, 0, &new_mask));

        /* Set bulk control action */
        _SOC_IF_ERROR_UNLOCK_EXIT(READ_L2_BULK_CONTROLr(unit, &rval));
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 2);
        if (SOC_CONTROL(unit)->l2x_mode == L2MODE_FIFO) {
            soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, 
                              L2_MOD_FIFO_RECORDf, 0);
        }
        _SOC_IF_ERROR_UNLOCK_EXIT(WRITE_L2_BULK_CONTROLr(unit, rval));

        rv = soc_l2x_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
        if (rv == SOC_E_TIMEOUT) {
            goto age_exit;
        }
        _SOC_IF_ERROR_UNLOCK_EXIT(rv);

age_exit:
        soc_mem_unlock(unit, L2Xm);
        sal_sem_give(soc->l2x_age_sync);

        etime = sal_time_usecs();
        LOG_VERBOSE(BSL_LS_SOC_ARL,
                    (BSL_META_U(unit,
                                "l2_bulk_age_thread: unit=%d: done in %d usec with rv=%d\n"),
                     unit, SAL_USECS_SUB(etime, stime), rv));

age_delay:
        rv = -1;
        for (i = 0; i < interval / cfg_interval; i++) {
            rv = sal_sem_take(soc->l2x_age_notify, cfg_interval * 1000000);
            if (interval != soc->l2x_age_interval || rv == 0) {
                break;
            }
        }
        /* age interval is not changed */
        if (rv != 0 && interval == soc->l2x_age_interval && interval % cfg_interval) {
            rv = sal_sem_take(soc->l2x_age_notify, interval % cfg_interval * 1000000);
        }
        /* age interval is changed */
        if (soc->l2x_age_interval && (interval != soc->l2x_age_interval || rv == 0)) {
            interval = soc->l2x_age_interval;
            goto age_delay;
        }

        iter++;
    }

thread_exit:
    LOG_VERBOSE(BSL_LS_SOC_L2,
                (BSL_META_U(unit,
                            "l2_bulk_age_thread: exiting\n")));

    soc->l2x_age_pid = SAL_THREAD_ERROR;
    soc->l2x_age_interval = 0;
    soc->l2x_age_enable = 0;

    sal_thread_exit(0);
}

/*
 * Function:
 *  soc_td2_l2_bulk_age_start
 * Purpose:
 *  Start l2 bulk age thread
 * Parameters:
 *  unit - unit number.
 * Returns:
 *  SOC_E_XXX
 */
int
soc_td2_l2_bulk_age_start(int unit, int interval)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int cfg_interval;
    int thread_prio;

    if (soc->l2x_age_pid && soc->l2x_age_pid != SAL_THREAD_ERROR) {
        return SOC_E_NONE;
    }

    cfg_interval = soc_property_get(unit, spn_L2_SW_AGING_INTERVAL, 300);

    SOC_CONTROL_LOCK(unit);
    soc->l2x_age_interval = interval ? interval : cfg_interval;
    soc->l2x_age_enable = interval ? 1 : 0;
    sal_snprintf(soc->l2x_age_name, sizeof(soc->l2x_age_name), "bcmL2age.%d", unit);
    thread_prio = 50;
    if (soc_feature(unit, soc_feature_l2_hw_aging_bug)) {
        /* Real-time threads not permitted in simulation environments */
        if (!SAL_BOOT_SIMULATION) {
            thread_prio = SAL_THREAD_PRIO_NO_PREEMPT;
        }
    }
    soc->l2x_age_pid = sal_thread_create(soc->l2x_age_name, SAL_THREAD_STKSZ,
                                         soc_property_get(unit, spn_L2AGE_THREAD_PRI, thread_prio),
                                         _soc_td2_l2_bulk_age, INT_TO_PTR(unit));
    if (soc->l2x_age_pid == SAL_THREAD_ERROR) {
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                              "Could not start L2 bulk age thread\n")));
        SOC_CONTROL_UNLOCK(unit);
        return SOC_E_MEMORY;
    }
    SOC_CONTROL_UNLOCK(unit);

    return SOC_E_NONE;
}

/*
 * Function:
 *  soc_td2_l2_bulk_age_stop
 * Purpose:
 *  Stop l2 bulk age thread
 * Parameters:
 *  unit - unit number.
 * Returns:
 *  SOC_E_XXX
 */
int
soc_td2_l2_bulk_age_stop(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_timeout_t to;
    int rv = SOC_E_NONE;

    SOC_CONTROL_LOCK(unit);
    soc->l2x_age_interval = 0;
    soc->l2x_age_enable = 0;
    SOC_CONTROL_UNLOCK(unit);

    if (soc->l2x_age_pid && soc->l2x_age_pid != SAL_THREAD_ERROR) {
        sal_sem_give(soc->l2x_age_notify);

        if (SAL_BOOT_SIMULATION) {
            soc_timeout_init(&to, 300 * 1000000, 0);
        } else {
            soc_timeout_init(&to, 60 * 1000000, 0);
        }

        while (soc->l2x_age_pid != SAL_THREAD_ERROR) {
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_L2,
                          (BSL_META_U(unit,
                                      "thread will not exit\n")));
                rv = SOC_E_INTERNAL;
                break;
            }
        }
    }

    return rv;
}

#endif /* BCM_TRIDENT2_SUPPORT */
#endif /* BCM_XGS_SWITCH_SUPPORT */
