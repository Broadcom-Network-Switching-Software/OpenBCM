/*! \file bcmbd_cmicx2_lp_intr.c
 *
 * The ICFG_xxx registers on CMICx v2 do not use the original CMICx
 * addresses, so the low-priority interrupt register tables must be
 * initialized using a device-specific function.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_debug.h>

#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/bcmbd_cmicx2_lp_intr.h>
#include <bcmbd/bcmbd_cmicx2_acc.h>

/* Log source for this component */
#define BSL_LOG_MODULE                  BSL_LS_BCMBD_DEV

/*******************************************************************************
 * Local definitions
 */

/* This device uses a non-standard low-priority interrupt number */
#define CMICX2_IRQ_CHIP_LP_INTR         127

/*******************************************************************************
 * Local data
 */

static struct cmicx_lp_iregs_s {
    unsigned int intr_base;
    uint32_t status_reg;
    uint32_t mask_reg;
} cmicx_lp_iregs[] = {
    { LP_INTR_BASE0, ICFG_CHIP_LP_INTR_RAW_STATUS_REG0r_OFFSET,
      ICFG_CHIP_LP_INTR_ENABLE_REG0r_OFFSET },
    { LP_INTR_BASE1, ICFG_CHIP_LP_INTR_RAW_STATUS_REG1r_OFFSET,
      ICFG_CHIP_LP_INTR_ENABLE_REG1r_OFFSET },
    { LP_INTR_BASE2, ICFG_CHIP_LP_INTR_RAW_STATUS_REG2r_OFFSET,
      ICFG_CHIP_LP_INTR_ENABLE_REG2r_OFFSET },
    { LP_INTR_BASE3, ICFG_CHIP_LP_INTR_RAW_STATUS_REG3r_OFFSET,
      ICFG_CHIP_LP_INTR_ENABLE_REG3r_OFFSET },
    { 0 }
};

/*******************************************************************************
 * Private functions
 */

static int
bcmbd_cmicx2_lp_intr_regs_init(int unit)
{
    int rv;
    unsigned int idx;
    unsigned int intr_base;
    uint32_t status_reg;
    uint32_t mask_reg;

    for (idx = 0; idx < COUNTOF(cmicx_lp_iregs); idx++) {
        status_reg = cmicx_lp_iregs[idx].status_reg;
        if (status_reg == 0) {
            /* All done */
            break;
        }
        mask_reg = cmicx_lp_iregs[idx].mask_reg;
        intr_base = cmicx_lp_iregs[idx].intr_base;

        /* Initialize interrupt register entry */
        rv = bcmbd_cmicx_lp_intr_reg_set(unit, intr_base,
                                         status_reg, mask_reg);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
    }

    return SHR_E_NONE;
}

/*******************************************************************************
 * Public functions
 */

int
bcmbd_cmicx2_lp_intr_init(int unit)
{
    int rv;

    rv = bcmbd_cmicx_lp_intr_regs_clr(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    rv = bcmbd_cmicx2_lp_intr_regs_init(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    return SHR_E_NONE;
}

int
bcmbd_cmicx2_lp_intr_stop(int unit)
{
    int rv;

    /* Disable low-priority interrupt */
    bcmbd_cmicx_intr_disable(unit, CMICX2_IRQ_CHIP_LP_INTR);

    rv = bcmbd_cmicx_lp_intr_regs_clr(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    return SHR_E_NONE;
}

int
bcmbd_cmicx2_lp_intr_start(int unit)
{
    int rv;

    /* Install low-priority interrupt handler */
    rv = bcmbd_cmicx_intr_func_set(unit, CMICX2_IRQ_CHIP_LP_INTR,
                                   bcmbd_cmicx_lp_intr, 0);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Enable low-priority interrupt */
    bcmbd_cmicx_intr_enable(unit, CMICX2_IRQ_CHIP_LP_INTR);

    return SHR_E_NONE;
}
