/*! \file bcmbd_cmicx_lp_intr.c
 *
 * CMICx low-priority (LP) interrupt handler framework.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_error.h>

#include <bcmdrd/bcmdrd_types.h>

#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_cmicx_acc.h>

#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/bcmbd_cmicx_lp_intr.h>

#define BSL_LOG_MODULE  BSL_LS_BCMBD_DEV

/*******************************************************************************
 * Local definitions
 */

#define BCMBD_CMICX_MAX_INTR_REGS       8

#define MAX_UNITS       BCMDRD_CONFIG_MAX_UNITS
#define MAX_INTR_REGS   BCMBD_CMICX_MAX_INTR_REGS
#define MAX_INTRS       (32 * MAX_INTR_REGS)
#define MAX_CPUS        3

#define INTR_CNT_MAX    100000

#define TXRX_INTR_MASK  0xffffffff

#define IIOWIN_SIZE     0x1000

#define IIOWIN_OFFS(_a) \
    ((_a) & (IIOWIN_SIZE - 1))

#define INTR_VALID(_i) ((unsigned int)(_i) < MAX_INTRS)

#define INTR_MASK_LOCK(_u)
#define INTR_MASK_UNLOCK(_u)

typedef struct bcmbd_intr_reg_s {
    unsigned int intr_base;
    uint32_t status_reg;
    uint32_t mask_reg;
    uint32_t mask_val;
} bcmbd_intr_reg_t;

typedef struct bcmbd_intr_reg_info_s {
    unsigned int num_intr_regs;
    bcmbd_intr_reg_t intr_regs[MAX_INTR_REGS];
} bcmbd_intr_reg_info_t;

typedef struct bcmbd_intr_vect_s {
    bcmbd_intr_f func;
    uint32_t param;
} bcmbd_intr_vect_t;

typedef struct bcmbd_intr_vect_info_s {
    bcmbd_intr_vect_t intr_vects[MAX_INTRS];
} bcmbd_intr_vect_info_t;

typedef struct bcmbd_intr_mask_update_s {
    int unit;
    int irq_num;
    bcmbd_intr_reg_t *intr_reg;
    uint32_t mask_or;
    uint32_t mask_and;
} bcmbd_intr_mask_update_t;

/*******************************************************************************
 * Local data
 */

static bcmbd_intr_reg_info_t lp_intr_reg_info[MAX_UNITS];
static bcmbd_intr_vect_info_t lp_intr_vect_info[MAX_UNITS];

/* Default set of interrupt registers to handle */
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

static bcmbd_intr_reg_t *
ireg_get(int unit, unsigned int intr_num)
{
    unsigned int rdx;
    bcmbd_intr_reg_t *ireg;
    bcmbd_intr_reg_info_t *ri;

    ri = &lp_intr_reg_info[unit];
    for (rdx = 0; rdx < ri->num_intr_regs; rdx++) {
        ireg = &ri->intr_regs[rdx];
        if ((intr_num & ~0x1f) == (ireg->intr_base & ~0x1f)) {
            return ireg;
        }
    }
    return NULL;
}

static void
ireg_mask_write(int unit, bcmbd_intr_reg_t *ireg)
{
    BCMDRD_IPROC_WRITE(unit, ireg->mask_reg, ireg->mask_val);
}

/*
 * Synchronized update of interrupt mask register.
 *
 * This function should be called via bcmdrd_hal_intr_sync to ensure
 * that it does not execute concurrently with the interrupt handler.
 */
static int
ireg_mask_update(void *data)
{
    bcmbd_intr_mask_update_t *imu = (bcmbd_intr_mask_update_t *)data;

    /* We may be called by multiple application threads */
    INTR_MASK_LOCK(unit);

    /* Update interrupt mask value */
    imu->intr_reg->mask_val &= imu->mask_and;
    imu->intr_reg->mask_val |= imu->mask_or;

    /* Write to hardware */
    ireg_mask_write(imu->unit, imu->intr_reg);

    INTR_MASK_UNLOCK(unit);

    return 0;
}

/*
 * Update interrupt mask register.
 *
 * This function contains the bulk of the work for
 * bcmbd_cmicx_lp_intr_enable and bcmbd_cmicx_lp_intr_disable.
 */
static void
bcmbd_cmicx_lp_intr_update(int unit, int intr_num, bool enable)
{
    int irq_num = 0;
    int intr_bit;
    bcmbd_intr_reg_t *ireg;
    bcmbd_intr_mask_update_t intr_mask_update, *imu = &intr_mask_update;

    ireg = ireg_get(unit, intr_num);
    if (ireg) {
        /* Set up sync structure */
        sal_memset(imu, 0, sizeof(*imu));
        imu->unit = unit;
        imu->intr_reg = ireg;
        intr_bit = 1 << (intr_num & 0x1f);
        imu->mask_and = ~intr_bit;
        if (enable) {
            imu->mask_or = intr_bit;
        }

        /* Synchronize update with interrupt context */
        bcmdrd_hal_intr_sync(unit, irq_num, ireg_mask_update, imu);
    }
}

/*******************************************************************************
 * Public functions
 */

void
bcmbd_cmicx_lp_intr(int unit, uint32_t param)
{
    unsigned int idx, rdx;
    unsigned int intr_num;
    unsigned int active_intr;
    unsigned int intr_cnt;
    uint32_t mask;
    uint32_t istatus;
    bcmbd_intr_reg_t *ireg;
    bcmbd_intr_reg_info_t *ri;
    bcmbd_intr_vect_t *ivect;
    bcmbd_intr_vect_info_t *vi;

    /* Count total number of interrupts processed */
    intr_cnt = 0;

    do {
        /* Count number of interrupts processed in this iteration */
        active_intr = 0;

        ri = &lp_intr_reg_info[unit];
        vi = &lp_intr_vect_info[unit];
        for (rdx = 0; rdx < ri->num_intr_regs; rdx++) {
            ireg = &ri->intr_regs[rdx];
            if (ireg->mask_val == 0) {
                /* No interrupts enabled in this mask */
                continue;
            }
            BCMDRD_IPROC_READ(unit, ireg->status_reg, &istatus);
            /* Mask off inactive interrupts */
            istatus &= ireg->mask_val;
            if (istatus == 0) {
                /* No more pending interrupts in this register */
                continue;
            }
            for (idx = 0, mask = 0x1; idx < 32; idx++, mask <<= 1) {
                if ((istatus & mask) == 0) {
                    continue;
                }
                active_intr++;
                intr_num = ireg->intr_base + idx;
                ivect = &vi->intr_vects[intr_num];
                if (ivect->func) {
                    ivect->func(unit, ivect->param);
                } else {
                    /* No handler for enabled interrupt? */
                    bcmbd_cmicx_lp_intr_disable_nosync(unit, intr_num);
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "No interrupt handler for "
                                          "low-prio intr %u\n"),
                               intr_num));
                }
                /* Prevent infinite loop */
                if (++intr_cnt > INTR_CNT_MAX) {
                    bcmbd_cmicx_lp_intr_disable_nosync(unit, intr_num);
                    intr_cnt = 0;
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "Cannot clear interrupt for "
                                          "low-prio intr %u\n"),
                               intr_num));
                }
            }
        }
    } while (active_intr);
}

int
bcmbd_cmicx_lp_intr_func_set(int unit, unsigned int intr_num,
                             bcmbd_intr_f intr_func, uint32_t intr_param)
{
    bcmbd_intr_vect_t *vi;

    if (!INTR_VALID(intr_num)) {
        return SHR_E_PARAM;
    }

    vi = &lp_intr_vect_info[unit].intr_vects[intr_num];
    vi->func = intr_func;
    vi->param= intr_param;

    return SHR_E_NONE;
}

void
bcmbd_cmicx_lp_intr_enable(int unit, int lp_intr_num)
{
    bcmbd_cmicx_lp_intr_update(unit, lp_intr_num, true);
}

void
bcmbd_cmicx_lp_intr_disable(int unit, int lp_intr_num)
{
    bcmbd_cmicx_lp_intr_update(unit, lp_intr_num, false);
}

void
bcmbd_cmicx_lp_intr_disable_nosync(int unit, int lp_intr_num)
{
    int intr_bit;
    bcmbd_intr_reg_t *ireg;

    ireg = ireg_get(unit, lp_intr_num);
    if (ireg) {
        intr_bit = 1 << (lp_intr_num & 0x1f);
        ireg->mask_val &= ~intr_bit;
        ireg_mask_write(unit, ireg);
    }
}

int
bcmbd_cmicx_lp_intr_regs_clr(int unit)
{
    bcmbd_intr_reg_info_t *ri;

    ri = &lp_intr_reg_info[unit];
    sal_memset(ri, 0, sizeof(*ri));

    return SHR_E_NONE;
}

int
bcmbd_cmicx_lp_intr_reg_set(int unit, unsigned int intr_base,
                            uint32_t status_offs, uint32_t mask_offs)
{
    bcmbd_intr_reg_info_t *ri;
    bcmbd_intr_reg_t *ireg;
    unsigned int rdx;

    if (!INTR_VALID(intr_base)) {
        return SHR_E_PARAM;
    }

    /* Look for existing entry (unlikely) */
    ri = &lp_intr_reg_info[unit];
    ireg = NULL;
    for (rdx = 0; rdx < ri->num_intr_regs; rdx++) {
        if (ri->intr_regs[rdx].intr_base == intr_base) {
            ireg = &ri->intr_regs[rdx];
            break;
        }
    }

    /* Use next available entry */
    if (ireg == NULL) {
        if (ri->num_intr_regs >= COUNTOF(ri->intr_regs)) {
            return SHR_E_RESOURCE;
        }
        ireg = &ri->intr_regs[ri->num_intr_regs++];
    }

    /* Initialize interrupt register entry */
    sal_memset(ireg, 0, sizeof(*ireg));
    ireg->intr_base = intr_base;
    ireg->status_reg = status_offs;
    ireg->mask_reg = mask_offs;

    return SHR_E_NONE;
}

int
bcmbd_cmicx_lp_intr_regs_init(int unit)
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

int
bcmbd_cmicx_lp_intr_init(int unit)
{
    int rv;

    rv = bcmbd_cmicx_lp_intr_regs_clr(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    rv = bcmbd_cmicx_lp_intr_regs_init(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    return SHR_E_NONE;
}

int
bcmbd_cmicx_lp_intr_stop(int unit)
{
    int rv;

    /* Disable low-priority interrupt */
    bcmbd_cmicx_intr_disable(unit, IPROC_IRQ_CHIP_LP_INTR);

    rv = bcmbd_cmicx_lp_intr_regs_clr(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    return SHR_E_NONE;
}

int
bcmbd_cmicx_lp_intr_start(int unit)
{
    int rv;

    /* Install low-priority interrupt handler */
    rv = bcmbd_cmicx_intr_func_set(unit, IPROC_IRQ_CHIP_LP_INTR,
                                   bcmbd_cmicx_lp_intr, 0);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Enable low-priority interrupt */
    bcmbd_cmicx_intr_enable(unit, IPROC_IRQ_CHIP_LP_INTR);

    return SHR_E_NONE;
}
