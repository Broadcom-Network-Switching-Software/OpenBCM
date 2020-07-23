/*! \file bcmbd_cmicd_intr.c
 *
 * CMICd interrupt handler framework.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_error.h>

#include <bcmdrd/bcmdrd_types.h>

#include <bcmbd/bcmbd_cmicd.h>
#include <bcmbd/bcmbd_cmicd_acc.h>

#include <bcmbd/bcmbd_cmicd_intr.h>

#define BSL_LOG_MODULE  BSL_LS_BCMBD_INTR

/*******************************************************************************
 * Local definitions
 */

#define BCMBD_CMICD_MAX_CMCS            3
#define BCMBD_CMICD_MAX_INTR_REGS       6

#define MAX_UNITS       BCMDRD_CONFIG_MAX_UNITS
#define MAX_INTR_REGS   BCMBD_CMICD_MAX_INTR_REGS
#define MAX_INTRS       (32 * MAX_INTR_REGS)
#define MAX_CMCS        BCMBD_CMICD_MAX_CMCS
#define MAX_CPUS        3

#define CMC_OFFSET      0x1000

#define INTR_CNT_MAX    100000

#define TXRX_INTR_MASK \
    ((1 << CMICD_IRQ_CH0_DESC_CTRL_INTR) |      \
     (1 << CMICD_IRQ_CH1_DESC_CTRL_INTR) |      \
     (1 << CMICD_IRQ_CH2_DESC_CTRL_INTR) |      \
     (1 << CMICD_IRQ_CH3_DESC_CTRL_INTR))

#define CMC_VALID(_c) ((unsigned int)(_c) < MAX_CMCS)
#define INTR_VALID(_i) ((unsigned int)(_i) < MAX_INTRS)

#define INTR_MASK_LOCK(_u)
#define INTR_MASK_UNLOCK(_u)

typedef struct bcmbd_intr_reg_s {
    unsigned int intr_base;
    uint32_t status_reg;
    uint32_t mask_reg;
    uint32_t mask_val;
    uint32_t kmask;
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

static bool intr_threaded[MAX_UNITS];
static bool intr_connected[MAX_UNITS];
static bool use_knet_intr[MAX_UNITS];
static bcmbd_intr_reg_info_t intr_reg_info[MAX_UNITS][MAX_CMCS];
static bcmbd_intr_vect_info_t intr_vect_info[MAX_UNITS][MAX_CMCS];

/*******************************************************************************
 * Private functions
 */

static bcmbd_intr_reg_t *
ireg_get(int unit, int cmc, unsigned int intr_num)
{
    unsigned int rdx;
    bcmbd_intr_reg_t *ireg;
    bcmbd_intr_reg_info_t *ri;

    if (cmc < 0) {
        cmc = BCMBD_CMICD_CMC_GET(unit);
    }
    ri = &intr_reg_info[unit][cmc];
    for (rdx = 0; rdx < ri->num_intr_regs; rdx++) {
        ireg = &ri->intr_regs[rdx];
        if ((intr_num & ~0x1f) == (ireg->intr_base & ~0x1f)) {
            return ireg;
        }
    }
    return NULL;
}

static void
ireg_mask_write(int unit, int irq_num, bcmbd_intr_reg_t *ireg)
{
    if (ireg->kmask && use_knet_intr[unit]) {
        /* Use protected access if mask register is shared */
        bcmdrd_hal_intr_mask_write(unit, irq_num,
                                   ireg->status_reg, ireg->mask_val);
    } else {
        BCMDRD_DEV_WRITE32(unit, ireg->mask_reg, ireg->mask_val);
    }
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
    ireg_mask_write(imu->unit, imu->irq_num, imu->intr_reg);

    INTR_MASK_UNLOCK(unit);

    return 0;
}

/*
 * Update interrupt mask register.
 *
 * This function contains the bulk of the work for
 * bcmbd_cmicd_intr_enable and bcmbd_cmicd_intr_disable.
 */
static void
bcmbd_cmicd_intr_update(int unit, int cmc, int intr_num, bool enable)
{
    int irq_num = 0;
    int intr_bit;
    bcmbd_intr_reg_t *ireg;
    bcmbd_intr_mask_update_t intr_mask_update, *imu = &intr_mask_update;

    ireg = ireg_get(unit, cmc, intr_num);
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

static void
bcmbd_cmicd_intr(int unit)
{
    int cmc;
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

        for (cmc = 0; cmc < MAX_CMCS; cmc++) {
            ri = &intr_reg_info[unit][cmc];
            if (ri->num_intr_regs == 0) {
                /* Unused CMC */
                continue;
            }
            vi = &intr_vect_info[unit][cmc];
            for (rdx = 0; rdx < ri->num_intr_regs; rdx++) {
                ireg = &ri->intr_regs[rdx];
                if (ireg->mask_val == 0) {
                    /* No interrupts enabled in this mask */
                    continue;
                }
                BCMDRD_DEV_READ32(unit, ireg->status_reg, &istatus);
                /* Mask off interrupts handled by kernel driver */
                if (use_knet_intr[unit]) {
                    istatus &= ~ireg->kmask;
                }
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
                    LOG_DEBUG(BSL_LOG_MODULE,
                              (BSL_META_UPX(unit, -1, intr_num,
                                          "CMIC intr %u on CMC %d\n"),
                               intr_num, cmc));
                    if (ivect->func) {
                        ivect->func(unit, ivect->param);
                    } else {
                        /* No handler for enabled interrupt? */
                        bcmbd_cmicd_intr_disable(unit, cmc, intr_num);
                        LOG_ERROR(BSL_LOG_MODULE,
                                  (BSL_META_U(unit,
                                              "No interrupt handler for "
                                              "intr %u on CMC %d\n"),
                                   intr_num, cmc));
                    }
                    /* Prevent infinite loop */
                    if (++intr_cnt > INTR_CNT_MAX) {
                        bcmbd_cmicd_intr_disable(unit, cmc, intr_num);
                        intr_cnt = 0;
                        LOG_ERROR(BSL_LOG_MODULE,
                                  (BSL_META_U(unit,
                                              "Cannot clear interrupt for "
                                              "intr %u on CMC %d\n"),
                                   intr_num, cmc));
                    }
                }
            }
        }
    } while (active_intr);
}

static void
bcmbd_cmicd_isr(void *data)
{
    int unit = (unsigned long)data;
    int irq_num = 0;
    int cmc;
    unsigned int rdx;
    bcmbd_intr_reg_t *ireg;
    bcmbd_intr_reg_info_t *ri;

    bcmbd_cmicd_intr(unit);

    if (intr_threaded[unit]) {
        /* Threaded interrupt handlers need to re-enable interrupts */
        for (cmc = 0; cmc < MAX_CMCS; cmc++) {
            ri = &intr_reg_info[unit][cmc];
            if (ri->num_intr_regs == 0) {
                /* Unused CMC */
                continue;
            }
            for (rdx = 0; rdx < ri->num_intr_regs; rdx++) {
                ireg = &ri->intr_regs[rdx];
                ireg_mask_write(unit, irq_num, ireg);
            }
        }
    }
}

static int
bcmbd_cmicd_intr_force_disconnect(int unit)
{
    int irq_num = 0;
    int rv;

    rv = bcmdrd_hal_intr_disconnect(unit, irq_num);

    if (SHR_SUCCESS(rv)) {
        intr_connected[unit] = false;
    }

    return rv;
}

/*******************************************************************************
 * Public functions
 */

int
bcmbd_cmicd_intr_func_set(int unit, int cmc, unsigned int intr_num,
                          bcmbd_intr_f intr_func, uint32_t intr_param)
{
    bcmbd_intr_vect_t *vi;

    if (cmc < 0) {
        cmc = BCMBD_CMICD_CMC_GET(unit);
    }

    if (!CMC_VALID(cmc)) {
        return SHR_E_PARAM;
    }
    if (!INTR_VALID(intr_num)) {
        return SHR_E_PARAM;
    }

    vi = &intr_vect_info[unit][cmc].intr_vects[intr_num];
    vi->func = intr_func;
    vi->param= intr_param;

    return SHR_E_NONE;
}

void
bcmbd_cmicd_intr_enable(int unit, int cmc, int intr_num)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_UPX(unit, -1, intr_num,
                          "Enable CMIC intr %u on CMC %d\n"),
                 intr_num, cmc));
    bcmbd_cmicd_intr_update(unit, cmc, intr_num, true);
}

void
bcmbd_cmicd_intr_disable(int unit, int cmc, int intr_num)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_UPX(unit, -1, intr_num,
                          "Disable CMIC intr %u on CMC %d\n"),
                 intr_num, cmc));
    bcmbd_cmicd_intr_update(unit, cmc, intr_num, false);
}

void
bcmbd_cmicd_intr_disable_deferred(int unit, int cmc, int intr_num)
{
    int irq_num = 0;
    int intr_bit;
    bcmbd_intr_reg_t *ireg;

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_UPX(unit, -1, intr_num,
                            "Deferred disable CMIC intr %u on CMC %d\n"),
               intr_num, cmc));
    ireg = ireg_get(unit, cmc, intr_num);
    if (ireg) {
        intr_bit = 1 << (intr_num & 0x1f);
        ireg->mask_val &= ~intr_bit;
        /* Only update software structure if threaded interrupt handler */
        if (!intr_threaded[unit]) {
            ireg_mask_write(unit, irq_num, ireg);
        }
    }
}

int
bcmbd_cmicd_intr_regs_clr(int unit, int cmc)
{
    int idx;
    bcmbd_intr_reg_info_t *ri;

    for (idx = 0; idx < MAX_CMCS; idx++) {
        if (cmc >= 0 && cmc != idx) {
            continue;
        }
        ri = &intr_reg_info[unit][cmc];
        sal_memset(ri, 0, sizeof(*ri));
    }
    return SHR_E_NONE;
}

int
bcmbd_cmicd_intr_reg_set(int unit, int cmc, unsigned int intr_base,
                         uint32_t status_offs, uint32_t mask_offs,
                         uint32_t kmask)
{
    bcmbd_intr_reg_info_t *ri;
    bcmbd_intr_reg_t *ireg;
    unsigned int rdx;

    if (!CMC_VALID(cmc)) {
        return SHR_E_PARAM;
    }
    if (!INTR_VALID(intr_base)) {
        return SHR_E_PARAM;
    }

    /* Look for existing entry (unlikely) */
    ri = &intr_reg_info[unit][cmc];
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
    ireg->kmask = kmask;

    return SHR_E_NONE;
}

static struct cmicd_iregs_s {
    unsigned int intr_base;
    uint32_t status_reg;
    uint32_t mask_reg[MAX_CPUS];
} cmicd_iregs[] = {
    { CMICD_IRQ_BASE0, CMIC_CMC_IRQ_STAT0r_OFFSET,
      { CMIC_CMC_PCIE_IRQ_MASK0r_OFFSET,
        0 /*CMIC_CMC_UC0_IRQ_MASK0r_OFFSET */,
        0 /* CMIC_CMC_UC1_IRQ_MASK0r_OFFSET */ } },
    { CMICD_IRQ_BASE1, CMIC_CMC_IRQ_STAT1r_OFFSET,
      { CMIC_CMC_PCIE_IRQ_MASK1r_OFFSET,
        0 /*CMIC_CMC_UC1_IRQ_MASK1r_OFFSET */,
        0 /* CMIC_CMC_UC1_IRQ_MASK1r_OFFSET */ } },
    { CMICD_IRQ_BASE2, CMIC_CMC_IRQ_STAT2r_OFFSET,
      { CMIC_CMC_PCIE_IRQ_MASK2r_OFFSET,
        0 /*CMIC_CMC_UC2_IRQ_MASK2r_OFFSET */,
        0 /* CMIC_CMC_UC2_IRQ_MASK2r_OFFSET */ } },
    { CMICD_IRQ_BASE3, CMIC_CMC_IRQ_STAT3r_OFFSET,
      { CMIC_CMC_PCIE_IRQ_MASK3r_OFFSET,
        0 /*CMIC_CMC_UC3_IRQ_MASK3r_OFFSET */,
        0 /* CMIC_CMC_UC3_IRQ_MASK3r_OFFSET */ } },
    { CMICD_IRQ_BASE4, CMIC_CMC_IRQ_STAT4r_OFFSET,
      { CMIC_CMC_PCIE_IRQ_MASK4r_OFFSET,
        0 /*CMIC_CMC_UC4_IRQ_MASK4r_OFFSET */,
        0 /* CMIC_CMC_UC4_IRQ_MASK4r_OFFSET */ } },
    { CMICD_IRQ_BASE5, CMIC_CMC_IRQ_STAT5r_OFFSET,
      { CMIC_CMC_PCIE_IRQ_MASK5r_OFFSET,
        0 /*CMIC_CMC_UC5_IRQ_MASK5r_OFFSET */,
        0 /* CMIC_CMC_UC5_IRQ_MASK5r_OFFSET */ } },
    { CMICD_IRQ_BASE6, CMIC_CMC_IRQ_STAT6r_OFFSET,
      { CMIC_CMC_PCIE_IRQ_MASK6r_OFFSET,
        0 /*CMIC_CMC_UC6_IRQ_MASK6r_OFFSET */,
        0 /* CMIC_CMC_UC6_IRQ_MASK6r_OFFSET */ } },
    { 0 }
};

int
bcmbd_cmicd_intr_regs_init(int unit, int cmc, unsigned int num_regs)
{
    int rv;
    unsigned int idx;
    unsigned int cpu_no = 0;
    unsigned int intr_base;
    uint32_t status_reg;
    uint32_t mask_reg;
    uint32_t kmask;

    if (cmc < 0) {
        cmc = BCMBD_CMICD_CMC_GET(unit);
    }

    if (!CMC_VALID(cmc)) {
        return SHR_E_PARAM;
    }

    for (idx = 0; idx < num_regs; idx++) {
        status_reg = cmicd_iregs[idx].status_reg;
        if (status_reg == 0) {
            break;
        }
        mask_reg = cmicd_iregs[idx].mask_reg[cpu_no];
        intr_base = cmicd_iregs[idx].intr_base;
        status_reg += (cmc * CMC_OFFSET);
        mask_reg += (cmc * CMC_OFFSET);
        
        kmask = 0;
        if (status_reg == CMIC_CMC_IRQ_STAT0r_OFFSET) {
            /* Set KNET controlled interrupt bits */
            kmask = TXRX_INTR_MASK;
        }
        rv = bcmbd_cmicd_intr_reg_set(unit, cmc, intr_base,
                                      status_reg, mask_reg, kmask);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
    }

    return SHR_E_NONE;
}

int
bcmbd_cmicd_intr_disconnect(int unit)
{
    if (!intr_connected[unit]) {
        /* Not connected */
        return SHR_E_NONE;
    }

    if (use_knet_intr[unit]) {
        /* Kernel-level packet I/O driver remains active. */
        return SHR_E_NONE;
    }

    return bcmbd_cmicd_intr_force_disconnect(unit);
}

int
bcmbd_cmicd_intr_connect(int unit)
{
    int irq_num = 0;
    int rv, cmc;
    unsigned int num_iregs, rdx;
    bcmbd_intr_reg_t *ireg;
    bcmbd_intr_reg_info_t *ri;
    bcmdrd_hal_intr_info_t intr_info;
    bcmdrd_hal_io_t io;
    void (*isr_func)(void *data);
    void *isr_data;

    if (intr_connected[unit]) {
        /* Already connected */
        return SHR_E_NONE;
    }

    sal_memset(&intr_info, 0, sizeof(intr_info));

    rv = bcmdrd_dev_hal_io_get(unit, &io);
    if (SHR_FAILURE(rv)) {
        return SHR_E_INTERNAL;
    }
    intr_info.piowin_addr = io.phys_addr[0];
    intr_info.piowin_size = BCMBD_CMICD_IOMEM_SIZE;

    num_iregs = 0;
    for (cmc = 0; cmc < MAX_CMCS; cmc++) {
        ri = &intr_reg_info[unit][cmc];
        if (ri->num_intr_regs == 0) {
            /* Unused CMC */
            continue;
        }
        for (rdx = 0; rdx < ri->num_intr_regs; rdx++) {
            if (num_iregs >= COUNTOF(intr_info.irq_regs)) {
                return SHR_E_RESOURCE;
            }
            ireg = &ri->intr_regs[rdx];
            /* coverity[overrun-local : FALSE] */
            intr_info.irq_regs[num_iregs].status_reg = ireg->status_reg;
            intr_info.irq_regs[num_iregs].mask_reg = ireg->mask_reg;
            if (use_knet_intr[unit]) {
                intr_info.irq_regs[num_iregs].kmask = ireg->kmask;
                LOG_INFO(BSL_LOG_MODULE,
                         (BSL_META_U(unit, "Enable KNET interrupts\n")));
            }
            num_iregs++;
        }
    }
    intr_info.num_irq_regs = num_iregs;

    rv = bcmdrd_hal_intr_configure(unit, irq_num, &intr_info);

    intr_threaded[unit] = SHR_SUCCESS(rv) ? true : false;
    isr_func = bcmbd_cmicd_isr;
    isr_data = (void *)(unsigned long)unit;

    rv = bcmdrd_hal_intr_connect(unit, irq_num, isr_func, isr_data);

    if (SHR_SUCCESS(rv)) {
        intr_connected[unit] = true;
    }

    return rv;
}

int
bcmbd_cmicd_intr_knet_enable_set(int unit, bool enable)
{
    if (!bcmdrd_dev_exists(unit)) {
        return SHR_E_INTERNAL;
    }

    if (enable == use_knet_intr[unit]) {
        /* No change */
        return SHR_E_NONE;
    }

    use_knet_intr[unit] = enable;

    /* Disconnect and reconnect to update settings in kernel */
    if (intr_connected[unit]) {
        bcmbd_cmicd_intr_force_disconnect(unit);
        bcmbd_cmicd_intr_connect(unit);
    }

    return SHR_E_NONE;
}

int
bcmbd_cmicd_intr_knet_enable_get(int unit, bool *enabled)
{
    if (!bcmdrd_dev_exists(unit)) {
        return SHR_E_INTERNAL;
    }

    if (enabled) {
        *enabled = use_knet_intr[unit];
    }

    return SHR_E_NONE;
}
