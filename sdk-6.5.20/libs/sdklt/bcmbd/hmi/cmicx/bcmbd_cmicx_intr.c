/*! \file bcmbd_cmicx_intr.c
 *
 * CMICx interrupt handler framework.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_assert.h>

#include <bsl/bsl.h>

#include <shr/shr_error.h>

#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_hal.h>

#include <bcmbd/bcmbd_ipoll.h>
#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_cmicx_acc.h>
#include <bcmbd/bcmbd_cmicx_intr.h>

#define BSL_LOG_MODULE  BSL_LS_BCMBD_INTR

/*******************************************************************************
 * Local definitions
 */

#define BCMBD_CMICX_MAX_INTR_REGS       8

#define MAX_UNITS       BCMDRD_CONFIG_MAX_UNITS
#define MAX_INTR_REGS   BCMBD_CMICX_MAX_INTR_REGS
#define MAX_INTRS       (32 * MAX_INTR_REGS)
#define MAX_CPUS        1

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
static bool polled_irq[MAX_UNITS];
static bcmbd_intr_reg_info_t intr_reg_info[MAX_UNITS];
static bcmbd_intr_vect_info_t intr_vect_info[MAX_UNITS];

/* Interrupt register base address for optimized access */
static volatile uint32_t *intc_base[MAX_UNITS];

/* Default set of interrupt registers to handle */
static struct cmicx_iregs_s {
    unsigned int intr_base;
    uint32_t status_reg;
    uint32_t mask_reg[MAX_CPUS];
} cmicx_iregs[] = {
    { IPROC_IRQ_BASE1, INTC_INTR_RAW_STATUS_REG1r_OFFSET,
      { INTC_INTR_ENABLE_REG1r_OFFSET } },
    { IPROC_IRQ_BASE2, INTC_INTR_RAW_STATUS_REG2r_OFFSET,
      { INTC_INTR_ENABLE_REG2r_OFFSET } },
    { IPROC_IRQ_BASE3, INTC_INTR_RAW_STATUS_REG3r_OFFSET,
      { INTC_INTR_ENABLE_REG3r_OFFSET } },
    { IPROC_IRQ_BASE4, INTC_INTR_RAW_STATUS_REG4r_OFFSET,
      { INTC_INTR_ENABLE_REG4r_OFFSET, } },
    { IPROC_IRQ_BASE5, INTC_INTR_RAW_STATUS_REG5r_OFFSET,
      { INTC_INTR_ENABLE_REG5r_OFFSET, } },
    { IPROC_IRQ_BASE6, INTC_INTR_RAW_STATUS_REG6r_OFFSET,
      { INTC_INTR_ENABLE_REG6r_OFFSET, } },
    { IPROC_IRQ_BASE7, INTC_INTR_RAW_STATUS_REG7r_OFFSET,
      { INTC_INTR_ENABLE_REG7r_OFFSET, } },
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

    ri = &intr_reg_info[unit];
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
    if (polled_irq[unit]) {
        /* Keep hardware interrupts disabled */
        return;
    }
    if (ireg->kmask) {
        if (use_knet_intr[unit]) {
            /* Kernel driver owns all bits or none */
            return;
        }
    }
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
    ireg_mask_write(imu->unit, imu->irq_num, imu->intr_reg);

    INTR_MASK_UNLOCK(unit);

    return 0;
}

/*
 * Update interrupt mask register.
 *
 * This function contains the bulk of the work for
 * bcmbd_cmicx_intr_enable and bcmbd_cmicx_intr_disable.
 */
static void
bcmbd_cmicx_intr_update(int unit, int intr_num, bool enable)
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

static void
bcmbd_cmicx_intr(int unit)
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

    /* Clear MSI interrupt status if needed */
    bcmdrd_hal_iproc_msi_clear(unit);

    do {
        /* Count number of interrupts processed in this iteration */
        active_intr = 0;

        ri = &intr_reg_info[unit];
        vi = &intr_vect_info[unit];
        for (rdx = 0; rdx < ri->num_intr_regs; rdx++) {
            ireg = &ri->intr_regs[rdx];
            if (ireg->mask_val == 0) {
                /* No interrupts enabled in this mask */
                continue;
            }
            BCMDRD_IPROC_READ(unit, ireg->status_reg, &istatus);
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
                if (ivect->func) {
                    ivect->func(unit, ivect->param);
                } else {
                    /* No handler for enabled interrupt? */
                    bcmbd_cmicx_intr_disable_deferred(unit, intr_num);
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "No interrupt handler for "
                                          "intr %u\n"),
                               intr_num));
                }
                /* Prevent infinite loop */
                if (++intr_cnt > INTR_CNT_MAX) {
                    bcmbd_cmicx_intr_disable_deferred(unit, intr_num);
                    intr_cnt = 0;
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "Cannot clear interrupt for "
                                          "intr %u\n"),
                               intr_num));
                }
            }
        }
    } while (active_intr);
}

static void
bcmbd_cmicx_isr(void *data)
{
    int unit = (unsigned long)data;
    int irq_num = 0;
    unsigned int rdx;
    bcmbd_intr_reg_t *ireg;
    bcmbd_intr_reg_info_t *ri;

    bcmbd_cmicx_intr(unit);

    if (intr_threaded[unit]) {
        /* Threaded interrupt handlers need to re-enable interrupts */
        ri = &intr_reg_info[unit];
        for (rdx = 0; rdx < ri->num_intr_regs; rdx++) {
            ireg = &ri->intr_regs[rdx];
            ireg_mask_write(unit, irq_num, ireg);
        }
    }
}

static int
bcmbd_cmicx_intr_force_disconnect(int unit)
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
bcmbd_cmicx_intr_func_set(int unit, unsigned int intr_num,
                          bcmbd_intr_f intr_func, uint32_t intr_param)
{
    bcmbd_intr_vect_t *vi;

    if (!INTR_VALID(intr_num)) {
        return SHR_E_PARAM;
    }

    vi = &intr_vect_info[unit].intr_vects[intr_num];
    vi->func = intr_func;
    vi->param= intr_param;

    return SHR_E_NONE;
}

void
bcmbd_cmicx_intr_enable(int unit, int intr_num)
{
    bcmbd_cmicx_intr_update(unit, intr_num, true);
}

void
bcmbd_cmicx_intr_disable(int unit, int intr_num)
{
    bcmbd_cmicx_intr_update(unit, intr_num, false);
}

void
bcmbd_cmicx_intr_disable_deferred(int unit, int intr_num)
{
    int irq_num = 0;
    int intr_bit;
    bcmbd_intr_reg_t *ireg;

    ireg = ireg_get(unit, intr_num);
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
bcmbd_cmicx_intr_regs_clr(int unit)
{
    bcmbd_intr_reg_info_t *ri;

    ri = &intr_reg_info[unit];
    sal_memset(ri, 0, sizeof(*ri));

    return SHR_E_NONE;
}

int
bcmbd_cmicx_intr_reg_set(int unit, unsigned int intr_base,
                         uint32_t status_offs, uint32_t mask_offs,
                         uint32_t kmask)
{
    bcmbd_intr_reg_info_t *ri;
    bcmbd_intr_reg_t *ireg;
    unsigned int rdx;

    if (!INTR_VALID(intr_base)) {
        return SHR_E_PARAM;
    }

    /* Look for existing entry (unlikely) */
    ri = &intr_reg_info[unit];
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

int
bcmbd_cmicx_intr_regs_init(int unit)
{
    int rv;
    unsigned int idx;
    unsigned int cpu_no = 0;
    unsigned int intr_base;
    uint32_t status_reg;
    uint32_t mask_reg;
    uint32_t kmask;

    for (idx = 0; idx < COUNTOF(cmicx_iregs); idx++) {
        status_reg = cmicx_iregs[idx].status_reg;
        if (status_reg == 0) {
            /* All done */
            break;
        }
        mask_reg = cmicx_iregs[idx].mask_reg[cpu_no];
        intr_base = cmicx_iregs[idx].intr_base;
        kmask = 0;
        if (status_reg == INTC_INTR_RAW_STATUS_REG4r_OFFSET ||
            status_reg == INTC_INTR_RAW_STATUS_REG6r_OFFSET) {
            /* Set KNET controlled interrupt bits */
            kmask = TXRX_INTR_MASK;
        }
        rv = bcmbd_cmicx_intr_reg_set(unit, intr_base,
                                      status_reg, mask_reg, kmask);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
    }

    return SHR_E_NONE;
}

int
bcmbd_cmicx_intr_init(int unit)
{
    int rv;
    uint32_t intc_reg;

    /*
     * Get the logical pointer to the interrupt register base address
     * in order to enable direct interrupt register access and lock
     * (reserve) the associated iProc PCI sub-window. Note that any
     * register offset within the interrupt register block can be used
     * to get the base address.
     */
    intc_reg = INTC_INTR_RAW_STATUS_REG0r_OFFSET;
    intc_base[unit] = bcmdrd_hal_iproc_mmap_get(unit, intc_reg);
#if BCMDRD_CONFIG_MEMMAP_DIRECT == 1
    if (intc_base[unit] == NULL) {
        /* Check for invalid build configuration */
        int memmap_direct_not_supported = 0;
        assert(memmap_direct_not_supported);
    }
#endif

    rv = bcmbd_cmicx_intr_regs_clr(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    rv = bcmbd_cmicx_intr_regs_init(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    rv = bcmbd_ipoll_init(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    polled_irq[unit] = bcmbd_ipoll_enabled(unit);

    return rv;
}

int
bcmbd_cmicx_intr_disconnect(int unit)
{
    if (!intr_connected[unit]) {
        /* Not connected */
        return SHR_E_NONE;
    }

    if (use_knet_intr[unit]) {
        /* Kernel-level packet I/O driver remains active. */
        return SHR_E_NONE;
    }

    return bcmbd_cmicx_intr_force_disconnect(unit);
}

int
bcmbd_cmicx_intr_connect(int unit)
{
    int irq_num = 0;
    int rv;
    uint64_t phys_addr;
    uint32_t axi_addr;
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

    /* Set MSI interrupt clearing to SW-clear */
    bcmdrd_hal_iproc_msi_auto_clear_set(unit, 0);

    sal_memset(&intr_info, 0, sizeof(intr_info));

    rv = bcmdrd_dev_hal_io_get(unit, &io);
    if (SHR_FAILURE(rv)) {
        return SHR_E_INTERNAL;
    }
    /* Base address of device register window (CMIC) */
    intr_info.piowin_addr = io.phys_addr[0];
    intr_info.piowin_size = BCMBD_CMICX_IOMEM_SIZE;

    /* Base address of interrupt controller window */
    phys_addr = io.phys_addr[1];
    axi_addr = INTC_INTR_RAW_STATUS_REG0r_OFFSET;
    phys_addr = bcmdrd_hal_iproc_phys_base_get(unit, phys_addr, axi_addr);
    intr_info.iiowin_addr = phys_addr;
    intr_info.iiowin_size = IIOWIN_SIZE;

    num_iregs = 0;
    ri = &intr_reg_info[unit];
    for (rdx = 0; rdx < ri->num_intr_regs; rdx++) {
        if (num_iregs >= COUNTOF(intr_info.irq_regs)) {
            return SHR_E_RESOURCE;
        }
        ireg = &ri->intr_regs[rdx];
        /* coverity[overrun-local : FALSE] */
        intr_info.irq_regs[num_iregs].status_reg = IIOWIN_OFFS(ireg->status_reg);
        intr_info.irq_regs[num_iregs].mask_reg = IIOWIN_OFFS(ireg->mask_reg);
        if (use_knet_intr[unit]) {
            intr_info.irq_regs[num_iregs].kmask = ireg->kmask;
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Enable KNET interrupts\n")));
        }
        num_iregs++;
    }
    intr_info.num_irq_regs = num_iregs;

    /* Base address of PCI bridge registers window */
    phys_addr = io.phys_addr[1];
    if (irq_num < 32) {
        axi_addr = PAXB_0_PAXB_IC_INTRCLR_0r_OFFSET;
    } else {
        axi_addr = PAXB_0_PAXB_IC_INTRCLR_1r_OFFSET;
    }
    phys_addr = bcmdrd_hal_iproc_phys_base_get(unit, phys_addr, axi_addr);
    intr_info.paxbwin_addr = phys_addr;
    intr_info.paxbwin_size = IIOWIN_SIZE;

    intr_info.intr_ack.ack_reg = IIOWIN_OFFS(axi_addr);
    intr_info.intr_ack.ack_val = (1 << (irq_num % 32));
    intr_info.intr_ack.flags = BCMDRD_HAL_INTR_ACK_F_PAXB;

    rv = bcmdrd_hal_intr_configure(unit, irq_num, &intr_info);

    intr_threaded[unit] = SHR_SUCCESS(rv) ? true : false;
    isr_func = bcmbd_cmicx_isr;
    isr_data = (void *)(unsigned long)unit;

    rv = bcmdrd_hal_intr_connect(unit, irq_num, isr_func, isr_data);

    if (SHR_SUCCESS(rv)) {
        intr_connected[unit] = true;
    }

    return rv;
}

int
bcmbd_cmicx_intr_knet_enable_set(int unit, bool enable)
{
    int rv = SHR_E_NONE;

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
        rv = bcmbd_cmicx_intr_force_disconnect(unit);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        rv = bcmbd_cmicx_intr_connect(unit);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
    }

    return SHR_E_NONE;
}

int
bcmbd_cmicx_intr_knet_enable_get(int unit, bool *enabled)
{
    if (!bcmdrd_dev_exists(unit)) {
        return SHR_E_INTERNAL;
    }

    if (enabled) {
        *enabled = use_knet_intr[unit];
    }

    return SHR_E_NONE;
}
