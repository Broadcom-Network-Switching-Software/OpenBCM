/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:	l2xmsg.c
 * Purpose:	Provide needed routines for L2 overflow.
 */

#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <sal/core/spl.h>
#include <sal/core/time.h>
#include <shared/bsl.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/l2x.h>
#include <soc/ism_hash.h>
#include <soc/intr.h>
#include <soc/iproc.h>
#include <soc/mcm/intr_iproc.h>
#include <soc/trident2.h>
#include <soc/tomahawk.h>
#include <soc/apache.h>

#ifdef BCM_TRIDENT2_SUPPORT

int
soc_td2_l2_overflow_disable(int unit)
{
    int rv = SOC_E_NONE;
    soc_reg_t reg = (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) ? L2_MGMT_INTR_MASKr : IL2LU_INTR_ENABLEr;

    SOC_CONTROL_LOCK(unit);
    rv = soc_reg_field32_modify(unit, reg, REG_PORT_ANY,
                                L2_LEARN_INSERT_FAILUREf, 0);
    if (SOC_FAILURE(rv)) {
        SOC_CONTROL_UNLOCK(unit);
        return rv;
    }
    /* Mark as inactive */
    SOC_CONTROL(unit)->l2_overflow_active = FALSE;
    SOC_CONTROL_UNLOCK(unit);
    /* But don't disable interrupt as its shared by various IL2LU events */
    return SOC_E_NONE;
}

int
soc_td2_l2_overflow_enable(int unit)
{
    int rv = SOC_E_NONE;
    soc_reg_t reg = (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) ? L2_MGMT_INTR_MASKr : IL2LU_INTR_ENABLEr;

    SOC_CONTROL_LOCK(unit);
    rv = soc_reg_field32_modify(unit, reg, REG_PORT_ANY,
                                L2_LEARN_INSERT_FAILUREf, 1);
    if (SOC_FAILURE(rv)) {
        SOC_CONTROL_UNLOCK(unit);
        return rv;
    }
    /* Mark as active */
    SOC_CONTROL(unit)->l2_overflow_active = TRUE;
    SOC_CONTROL_UNLOCK(unit);
    /* Enable interrupt */
#ifdef BCM_CMICX_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            if (soc_feature(unit, soc_feature_cmicx)) {
                uint32 cmic_rval0;
                soc_iproc_getreg(unit,soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG0r, REG_PORT_ANY, 0),
                                &cmic_rval0);
                cmic_rval0 |= 1 << 1; /* Bit 1 is used for l2_mgmt to cmic interupt */
                soc_iproc_setreg(unit,
                                 soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG0r, REG_PORT_ANY, 0),
                                 cmic_rval0);
                soc_cmic_intr_enable(unit, CHIP_INTR_LOW_PRIORITY);
            }
            return SOC_E_NONE;
        }
#endif   
#ifdef BCM_CMICM_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        (void)soc_cmicm_intr3_enable(unit, _SOC_TH_L2_MGMT_INTR_MASK);
    } else if (SOC_IS_APACHE(unit)) {
        (void)soc_cmicm_intr3_enable(unit, _SOC_AP_L2LU_INTR_MASK);
    } else {
        (void)soc_cmicm_intr1_enable(unit, _SOC_TD2_L2LU_INTR_MASK);
    }
#endif
    return SOC_E_NONE;
}

int
soc_td2_l2_overflow_fill(int unit, uint8 zeros_or_ones)
{
    l2_learn_insert_failure_entry_t entry;
    
    if (zeros_or_ones) {
        sal_memset(&entry, 0xffffffff, sizeof(entry));
        SOC_IF_ERROR_RETURN
            (WRITE_L2_LEARN_INSERT_FAILUREm(unit, COPYNO_ALL, 0, &entry));
    } else {
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, L2_LEARN_INSERT_FAILUREm, COPYNO_ALL, FALSE));
    }    
    return SOC_E_NONE;
}

void
soc_td2_l2_overflow_interrupt_handler(int unit)
{
    l2x_entry_t l2x_entry;

    if (!SOC_CONTROL(unit)->l2_overflow_active) {
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                              "Received L2 overflow event with no app handler or"   \
                              " processing inactive !!\n")));
    }
    if (soc_td2_l2_overflow_disable(unit)) {
        return;
    }

    if (READ_L2_LEARN_INSERT_FAILUREm(unit, COPYNO_ALL, 0, &l2x_entry)) {
        return;
    }

    /* Callback */
	soc_l2x_callback(unit, SOC_L2X_ENTRY_OVERFLOW, NULL, &l2x_entry);    
}

int
soc_td2_l2_overflow_stop(int unit)
{
    if (!SOC_CONTROL(unit)->l2_overflow_enable) {
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN(soc_td2_l2_overflow_disable(unit));
    SOC_IF_ERROR_RETURN(soc_td2_l2_overflow_fill(unit, 1));
    return SOC_E_NONE;
}

int
soc_td2_l2_overflow_start(int unit)
{
    if (!SOC_CONTROL(unit)->l2_overflow_enable) {
        return SOC_E_NONE;
    }
    if (SOC_CONTROL(unit)->l2_overflow_active) {
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN(soc_td2_l2_overflow_fill(unit, 0));
    SOC_IF_ERROR_RETURN(soc_td2_l2_overflow_enable(unit));
    return SOC_E_NONE;
}

#endif /* BCM_TRIUMPH3_SUPPORT */
