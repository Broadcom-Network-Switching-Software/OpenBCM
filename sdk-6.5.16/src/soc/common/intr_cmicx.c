/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * SOC CMICX Interrupt Handlers
 *
 * NOTE: These handlers are called from an interrupt context, so their
 *       actions are restricted accordingly.
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <sal/core/spl.h>
#include <sal/core/sync.h>
#include <sal/core/dpc.h>

#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/feature.h>
#include <soc/mem.h>
#include <soc/iproc.h>

#ifdef BCM_CMICX_SUPPORT
#include <soc/intr_cmicx.h>

#define INTC_REMAP_BITPOS_BASE               (INTC_REMAP_BITPOS_REG0r)
#define INTC_REMAP_BITPOS_NUM                (5)
#define INTC_REMAP_BITS_NUM                  (6)
#define INTC_REMAP_BITPOS_OFFSET(intr)   \
                       ((intr / INTC_REMAP_BITPOS_NUM) + INTC_REMAP_BITPOS_BASE)
#define INTC_REMAP_BITPOS_SHIFT(intr)  \
                       ((intr % INTC_REMAP_BITPOS_NUM) * INTC_REMAP_BITS_NUM)


#define IRQ_CMC_NUM                (CMC1_CH0_DESC_DONE - CMC0_CH0_DESC_DONE)
#define IS_UNIT_VALID(u)           (u >= 0 && u < SOC_MAX_NUM_DEVICES)
#define IS_IRQ_VALID(intr)            (intr < CMIC_INTERRUPT_NUM_MAX)
#define CMIC_INTR_HANDLE(u, n)     (&_cmicx_handler[u].intr_handler[n])
#define IRQ_MASK(u, i)             (_irq_mask[u].mask[i])
#define IRQ_MASK_SET(u, i, m)  { \
                               _irq_mask[u].mask[i] = m; \
                               WRITE_INTC_INTR(u, _irq_reg_map[i].enable, m); \
                               }
#define IHOST_ENABLE_IRQ(u, i, m, b)  { \
                               _irq_mask[u].mask[i] = m; \
                               WRITE_INTC_INTR(u, _ihost_irq_reg_map[i].enable, b); \
                             }
#define IHOST_DISABLE_IRQ(u, i, m, b)  { \
                               _irq_mask[u].mask[i] = m; \
                               WRITE_INTC_INTR(u, _ihost_irq_reg_map[i].disable, b); \
                             }
#define IRQ_BIT(intr)              (intr % (sizeof(uint32)*8))
#define IRQ_MASK_INDEX(intr)       (intr / (sizeof(uint32)*8))


#define READ_INTC_INTR(unit, reg, rvp) \
        soc_iproc_getreg(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0), rvp)
#define WRITE_INTC_INTR(unit, reg, rv) \
        soc_iproc_setreg(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0), rv)


typedef struct cmicx_intr_handler_s {
    soc_cmic_intr_handler_t  intr_handler[CMIC_INTERRUPT_NUM_MAX];
}cmicx_intr_handler_t;

typedef struct  cmicx_intr_mask_s {
    uint32   mask[CMICX_INTR_REG_NUM];
}cmicx_irq_mask_t;

typedef struct  cmicx_intr_reg_map_s {
    uint32   enable;
    uint32   stat;
    char     stat_nm[50];
} cmicx_intr_reg_map_t;

typedef struct  ihost_cmicx_intr_reg_map_s {
    uint32   enable;
    uint32   disable;
} ihost_cmicx_intr_reg_map_t;

/* local variable */

STATIC cmicx_intr_handler_t   _cmicx_handler[SOC_MAX_NUM_DEVICES];

STATIC  cmicx_irq_mask_t   _irq_mask[SOC_MAX_NUM_DEVICES];

STATIC cmicx_intr_reg_map_t   _irq_reg_map[] = {
{INTC_INTR_ENABLE_REG0r, INTC_INTR_RAW_STATUS_REG0r, "INTR_RAW_STATUS_REG0r"},
{INTC_INTR_ENABLE_REG1r, INTC_INTR_RAW_STATUS_REG1r, "INTR_RAW_STATUS_REG1r"},
{INTC_INTR_ENABLE_REG2r, INTC_INTR_RAW_STATUS_REG2r, "INTR_RAW_STATUS_REG2r"},
{INTC_INTR_ENABLE_REG3r, INTC_INTR_RAW_STATUS_REG3r, "INTR_RAW_STATUS_REG3r"},
{INTC_INTR_ENABLE_REG4r, INTC_INTR_RAW_STATUS_REG4r, "INTR_RAW_STATUS_REG4r"},
{INTC_INTR_ENABLE_REG5r, INTC_INTR_RAW_STATUS_REG5r, "INTR_RAW_STATUS_REG5r"},
{INTC_INTR_ENABLE_REG6r, INTC_INTR_RAW_STATUS_REG6r, "INTR_RAW_STATUS_REG6r"},
{INTC_INTR_ENABLE_REG7r, INTC_INTR_RAW_STATUS_REG7r, "INTR_RAW_STATUS_REG7r"},
};

STATIC ihost_cmicx_intr_reg_map_t   _ihost_irq_reg_map[] = {
{IHOST_GIC_GIC400_GICD_ISENABLERN_1r, IHOST_GIC_GIC400_GICD_ICENABLERN_1r},
{IHOST_GIC_GIC400_GICD_ISENABLERN_2r, IHOST_GIC_GIC400_GICD_ICENABLERN_2r},
{IHOST_GIC_GIC400_GICD_ISENABLERN_3r, IHOST_GIC_GIC400_GICD_ICENABLERN_3r},
{IHOST_GIC_GIC400_GICD_ISENABLERN_4r, IHOST_GIC_GIC400_GICD_ICENABLERN_4r},
{IHOST_GIC_GIC400_GICD_ISENABLERN_5r, IHOST_GIC_GIC400_GICD_ICENABLERN_5r},
{IHOST_GIC_GIC400_GICD_ISENABLERN_6r, IHOST_GIC_GIC400_GICD_ICENABLERN_6r},
{IHOST_GIC_GIC400_GICD_ISENABLERN_7r, IHOST_GIC_GIC400_GICD_ICENABLERN_7r},
{IHOST_GIC_GIC400_GICD_ISENABLERN_8r, IHOST_GIC_GIC400_GICD_ICENABLERN_8r},
};

/*******************************************
* @function _irq_least_bit_set
* purpose function to identify rightmost bit set
*
* @param unit [in] uint32 number
*
* @returns right most bit set
* @returns -1 if no bit is set
*
* @end
 */

STATIC int
_irq_least_bit_set(uint32 n)
{
    uint32 bit = 0;

    if (n == 0) {
         return -1;
    }

    if ((n & 0x0000FFFF) == 0) {
        bit = bit + 16;
        n = n >> 16;
    }
    if ((n & 0x000000FF) == 0) {
        bit = bit + 8;
        n = n >> 8;
    }
    if ((n & 0x0000000F) == 0) {
        bit = bit + 4;
        n = n >> 4;
    }
    if ((n & 0x00000003) == 0) {
        bit = bit + 2;
        n = n >> 2;
    }
    if ((n & 0x00000001) == 0) {
        bit = bit + 1;
    }

    return bit;
}


/*******************************************
* @function _soc_cmicx_intr_enable
* purpose Enable a particular interrupt
*
* @param unit [in] unit
* @param param [in] intr_num_t, IRQ Number
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_soc_cmicx_intr_enable(int unit, intr_num_t intr)
{
    uint32 oldMask;
    uint32 newMask;
    int s, ind;

    if (!IS_UNIT_VALID(unit)) {
       return SOC_E_PARAM;
    }

    if (!IS_IRQ_VALID(intr)) {
       return SOC_E_PARAM;
    }

    s = sal_splhi();
    ind = IRQ_MASK_INDEX(intr);

    oldMask = IRQ_MASK(unit, ind);
    newMask = 0x01 << IRQ_BIT(intr);
    newMask |= oldMask;

    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = 0;
    }
    LOG_VERBOSE(BSL_LS_SOC_INTR,
             (BSL_META_U(unit,
                         "%s:unit %d, intr %u\n"),
                         __func__, unit, intr));

    if (soc_cm_get_bus_type(unit) & SOC_AXI_DEV_TYPE) {
        IHOST_ENABLE_IRQ(unit, ind, newMask, 0x01 << IRQ_BIT(intr));
    } else {
        IRQ_MASK_SET(unit, ind, newMask);
    }

    /* In polling mode, keep the mask value */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = (uint32)0x01 << IRQ_BIT(intr);
        newMask |= oldMask;
        IRQ_MASK(unit, ind) = newMask;
    }

    sal_spl(s);

    return SOC_E_NONE;
}

/*******************************************
* @function _soc_cmicx_intr_disable
* purpose Enable a particular interrupt
*
* @param unit [in] unit
* @param param [in] intr_num_t, IRQ Number
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_soc_cmicx_intr_disable(int unit, intr_num_t intr)
{
    uint32 oldMask;
    uint32 newMask;
    int s, ind;

    if (!IS_UNIT_VALID(unit)) {
       return SOC_E_PARAM;
    }

    if (!IS_IRQ_VALID(intr)) {
       return SOC_E_PARAM;
    }
    s = sal_splhi();

    ind = IRQ_MASK_INDEX(intr);

    oldMask = IRQ_MASK(unit, ind);
    newMask = 0x01 << IRQ_BIT(intr);
    newMask = oldMask & ~newMask;

    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = 0;
    }
    LOG_VERBOSE(BSL_LS_SOC_INTR,
             (BSL_META_U(unit,
                         "%s: unit %d, intr %u\n"),
                         __func__, unit, intr));

    if (soc_cm_get_bus_type(unit) & SOC_AXI_DEV_TYPE) {
        IHOST_DISABLE_IRQ(unit, ind, newMask, 0x01 << IRQ_BIT(intr));
    } else {
        IRQ_MASK_SET(unit, ind, newMask);
    }

    /* In polling mode, mask value be kept */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = (uint32)0x01 << IRQ_BIT(intr);
        newMask = oldMask & ~newMask;
        IRQ_MASK(unit, ind) = newMask;
    }

    sal_spl(s);

    return SOC_E_NONE;
}

/*******************************************
* @function soc_cmic_intr_dump
* purpose dump registers particular interrupt
*
* @param unit [in] unit
* @param param [in] intr_num_t, Interrupt Number
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_soc_cmicx_intr_dump(int unit, intr_num_t intr)
{
    uint32 mask, irqStat;
    uint32 irqMask;
    int i, s;

    if (!IS_UNIT_VALID(unit)) {
       return SOC_E_PARAM;
    }

    if (!IS_IRQ_VALID(intr)) {
       return SOC_E_PARAM;
    }
    s = sal_splhi();

    i = IRQ_MASK_INDEX(intr);
    mask = 0x01 << IRQ_BIT(intr);

    READ_INTC_INTR(unit, _irq_reg_map[i].stat, &irqStat);
    READ_INTC_INTR(unit, _irq_reg_map[i].enable, &irqMask);

    LOG_WARN(BSL_LS_SOC_INTR,
             (BSL_META_U(unit,
                         "%s:unit=%d, intr=%u, enable=%d, status=%d\n"),
                         __func__, unit, intr,
                         (irqMask & mask) ? 1 : 0, (irqStat & mask)? 1 : 0));

    sal_spl(s);

    return SOC_E_NONE;
}


/*******************************************
* @function _soc_cmicx_intr_all_enable
* purpose Enable a particular interrupt
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_soc_cmicx_intr_all_enable(int unit)
{
    int i, s;

    s = sal_splhi();

    for (i = 0; i < CMICX_INTR_REG_NUM; i++) {
        if (soc_cm_get_bus_type(unit) & SOC_AXI_DEV_TYPE) {
            if (i < IRQ_MASK_INDEX(CHIP_INTR_LOW_PRIORITY)) {
                _irq_mask[unit].mask[i] = 0;
                continue;
            }
            if (i == IRQ_MASK_INDEX(CHIP_INTR_LOW_PRIORITY)) {
                IHOST_ENABLE_IRQ(unit, i, 0x01 << IRQ_BIT(CHIP_INTR_LOW_PRIORITY),
                    0x01 << IRQ_BIT(CHIP_INTR_LOW_PRIORITY));
            } else {
                IHOST_ENABLE_IRQ(unit, i, ~0, ~0);
            }
        } else {
            IRQ_MASK_SET(unit, i, ~0);
        }
    }
    sal_spl(s);

    return SOC_E_NONE;
}

/*******************************************
* @function _soc_cmicx_intr_all_disable
* purpose Enable a particular interrupt
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_soc_cmicx_intr_all_disable(int unit)
{
    int i, s;

    s = sal_splhi();
    for (i = 0; i < CMICX_INTR_REG_NUM; i++) {
        if (soc_cm_get_bus_type(unit) & SOC_AXI_DEV_TYPE) {
            if (i < IRQ_MASK_INDEX(CHIP_INTR_LOW_PRIORITY)) {
                _irq_mask[unit].mask[i] = 0;
                continue;
            }
            if (i == IRQ_MASK_INDEX(CHIP_INTR_LOW_PRIORITY)) {
                IHOST_DISABLE_IRQ(unit, i, 0, 0x01 << IRQ_BIT(CHIP_INTR_LOW_PRIORITY));
            } else {
                IHOST_DISABLE_IRQ(unit, i, 0, ~0);
            }
        } else {
            IRQ_MASK_SET(unit, i, 0);
        }
    }
    sal_spl(s);

    return SOC_E_NONE;
}

/*******************************************
* @function _soc_cmicx_intr_is_mask
* purpose Get specific interrupt mask status
*
* @param unit [in] unit
* @param param [in] intr_num_t, Interrupt Number
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_soc_cmicx_intr_is_mask(int unit, intr_num_t intr, int *mask)
{
    uint32 oldMask;
    uint32 newMask;
    int s, ind;

    if (mask == NULL) {
        return SOC_E_PARAM;
    }

    if (!IS_UNIT_VALID(unit)) {
        return SOC_E_PARAM;
    }

    if (!IS_IRQ_VALID(intr)) {
        return SOC_E_PARAM;
    }

    s = sal_splhi();
    ind = IRQ_MASK_INDEX(intr);

    oldMask = IRQ_MASK(unit, ind);
    newMask = (uint32)0x01 << IRQ_BIT(intr);
    *mask = (oldMask & newMask) ? 0 : 1;

    sal_spl(s);
    return SOC_E_NONE;
}

/*******************************************
* @function _soc_cmicx_intr_register
* purpose Register the interrupt handler
*
* @param unit [in] unit
* @param param [in] soc_cmic_intr_handler_t pointer
* @param param [in] int, size of the array elements
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_soc_cmicx_intr_register(int unit, soc_cmic_intr_handler_t *handle, int size)
{
    int i, s;

    if (!IS_UNIT_VALID(unit)) {
        return SOC_E_UNIT;
    }

    if (!handle) {
       return SOC_E_PARAM;
    }

    for (i = 0 ; i < size ; i++) {
        if (!IS_IRQ_VALID(handle[i].num)) {
           return SOC_E_PARAM;
        }

        if (!handle[i].intr_fn) {
            return SOC_E_PARAM;
        }
        s = sal_splhi();
            sal_memcpy(CMIC_INTR_HANDLE(unit, handle[i].num), &handle[i],
                       sizeof(soc_cmic_intr_handler_t));
        sal_spl(s);
    }

    return SOC_E_NONE;
}

/*******************************************
* @function _soc_cmicx_intr_map
* purpose Map CMICX interrupts
*
* @param unit [in] unit
* @param cmic_start [in] int, cmic interrupt start
* @param cmic_end [in] int, cmic interrupt end
* @param msi_vec [in] int, MSI interrupt vector
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_soc_cmicx_intr_map(int unit, int cmic_start, int cmic_end, int msi_vec)
{
    int i;
    soc_reg_t reg;
    uint32 val;
    uint32 mask;

    if (!IS_UNIT_VALID(unit)) {
       return SOC_E_PARAM;
    }

    if (!IS_IRQ_VALID(cmic_start)) {
       return SOC_E_PARAM;
    }

    if (!IS_IRQ_VALID(cmic_end)) {
       return SOC_E_PARAM;
    }

    for (i = cmic_start; i <= cmic_end; i++) {
        /* 6 bit mask, each interrupt is represented by */
        mask = ((0x01 << INTC_REMAP_BITS_NUM) - 1);
        reg = INTC_REMAP_BITPOS_OFFSET(i);
        READ_INTC_INTR(unit, reg, &val);
        mask <<= INTC_REMAP_BITPOS_SHIFT(i);
        val &= ~mask;
        val |=  msi_vec << INTC_REMAP_BITPOS_SHIFT(i);
        WRITE_INTC_INTR(unit, reg, val);
    }

    return SOC_E_NONE;
}

/*******************************************
* @function soc_cmicx_intr_init
* purpose initialize CMICX interrupt framework
*
* @param unit [in] unit
* @param unit [out] soc_cmic_intr_op_t pointer
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int
soc_cmicx_intr_init(int unit, soc_cmic_intr_op_t *intr_op)
{
    if (!IS_UNIT_VALID(unit)) {
        return SOC_E_UNIT;
    }

    sal_memset(&_cmicx_handler[unit], 0, sizeof(cmicx_intr_handler_t));
    sal_memset(&_irq_mask[unit], 0, sizeof(cmicx_irq_mask_t));

    /*
    * Since linux BDE currently supports only one bit MSI interrupt vector
    * All interrupts will be currntly assigned to bit 0. We need to
    * Identify the use cases how interrupts can be grouped and
    * partitioned based on performance requirement along with BDE changes
    * to support multiple MSI/MSIX interrupt requests.
    */

    /*Map Non CMIC interrupt */
    _soc_cmicx_intr_map(unit, WDOG_INTR,
                        CHIP_INTR_LOW_PRIORITY, 0);

    /* Map CMIC interrupts */
    _soc_cmicx_intr_map(unit, CMC0_CH0_DESC_DONE,
                        SCHAN_FIFO_CH1_DONE, 0);

    /* Set MSI mode to SW clear vs auto clear */
    WRITE_PAXB_0_PAXB_IC_INTRCLR_MODE_0r(unit, 0);
    WRITE_PAXB_0_PAXB_IC_INTRCLR_MODE_1r(unit, 0);

    intr_op->soc_cmic_intr_enable =   _soc_cmicx_intr_enable;
    intr_op->soc_cmic_intr_disable =   _soc_cmicx_intr_disable;
    intr_op->soc_cmic_intr_dump =   _soc_cmicx_intr_dump;
    intr_op->soc_cmic_intr_all_enable =   _soc_cmicx_intr_all_enable;
    intr_op->soc_cmic_intr_all_disable =   _soc_cmicx_intr_all_disable;
    intr_op->soc_cmic_intr_is_mask = _soc_cmicx_intr_is_mask;
    intr_op->soc_cmic_intr_register = _soc_cmicx_intr_register;

    return SOC_E_NONE;
}

/*******************************************
* @function soc_cmicx_intr
* purpose SOC CMICX Interrupt Service Routine
*
* @param unit [in] unit
*
*
* @end
 */
void
soc_cmicx_intr(void *_unit)
{
    soc_control_t *soc;
    uint32 irqStat, irqMask, mask;
#ifdef BROADCOM_DEBUG
    uint32 irqEnable;
#endif
    int unit = PTR_TO_INT(_unit);
    int i = 0;
    int intr, s;

    s = sal_splhi();

    /* Clear MSI interrupts immediately to prevent spurious interrupts */
    WRITE_PAXB_0_PAXB_IC_INTRCLR_0r(unit, 0xFFFFFFFF);
    WRITE_PAXB_0_PAXB_IC_INTRCLR_1r(unit, 0xFFFFFFFF);
    soc = SOC_CONTROL(unit);

    if (soc == NULL || (soc->soc_flags & SOC_F_BUSY) ||
        !(soc->soc_flags & SOC_F_ATTACHED)) {
        sal_spl(s);
        return;
    }

    soc->stat.intr++; /* Update count */

    for (i = 0; i < CMICX_INTR_REG_NUM; i++) {
#ifdef SEPARATE_PKTDMA_INTR_HANDLER
        if (i == IRQ_MASK_INDEX(CMC0_CH0_DESC_DONE))
        {
            /** Bypass Packet DMA Interrupts */
            continue;
        }
#endif
#ifdef INCLUDE_KNET
        if (SOC_KNET_MODE(unit) && i == IRQ_MASK_INDEX(CMC0_CH0_DESC_DONE)) {
            continue;
        }
#endif

        if ((soc_cm_get_bus_type(unit) & SOC_AXI_DEV_TYPE) &&
            (i < IRQ_MASK_INDEX(CHIP_INTR_LOW_PRIORITY))) {
            continue;
        }

        READ_INTC_INTR(unit, _irq_reg_map[i].stat, &irqStat);

        if ((soc_cm_get_bus_type(unit) & SOC_AXI_DEV_TYPE) &&
            (i == IRQ_MASK_INDEX(CHIP_INTR_LOW_PRIORITY))) {
            irqStat &= 0x01 << IRQ_BIT(CHIP_INTR_LOW_PRIORITY);
        }

        irqMask = IRQ_MASK(unit, i);

        /* Re-Enable Enabled-but-Unserviced interrupts */
        if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
            IRQ_MASK(unit, i) = (irqMask & ~irqStat);
        } else {
            if (soc_cm_get_bus_type(unit) & SOC_AXI_DEV_TYPE) {
                IHOST_ENABLE_IRQ(unit, i, (irqMask & ~irqStat), (irqMask & ~irqStat));
            } else {
                IRQ_MASK_SET(unit, i, (irqMask & ~irqStat));
            }
        }
        LOG_VERBOSE(BSL_LS_SOC_INTR,
                    (BSL_META_U(unit,
                     "%s:unit %d, stat reg %s, val = 0x%x mask = 0x%x prog-enable = 0x%x\n"),
                     __func__, unit, _irq_reg_map[i].stat_nm, irqStat, irqMask, (irqMask & ~irqStat)));

#ifdef BROADCOM_DEBUG
        if (soc_cm_get_bus_type(unit) & SOC_AXI_DEV_TYPE) {
            READ_INTC_INTR(unit, _ihost_irq_reg_map[i].enable, &irqEnable);
        } else {
            READ_INTC_INTR(unit, _irq_reg_map[i].enable, &irqEnable);
        }
        LOG_VERBOSE(BSL_LS_SOC_INTR, (BSL_META_U(unit,
                                                    "%s:unit %d, enable reg %s 0x%x\n"),
                                                    __func__, unit, _irq_reg_map[i].stat_nm, irqEnable));
#endif

        while (irqStat) {
            intr = _irq_least_bit_set(irqStat);
            mask = 0x01 << intr;
            intr += i * (sizeof(uint32) << 3);
            if ((irqMask & mask) && IS_IRQ_VALID(intr)) {
                if ((CMIC_INTR_HANDLE(unit, intr)->num == intr) &&
                    (CMIC_INTR_HANDLE(unit, intr)->intr_fn != NULL)) {

                      LOG_VERBOSE(BSL_LS_SOC_INTR, (BSL_META_U(unit,
                                                    "%s:unit %d, intr %u\n"),
                                                    __func__, unit, intr));

                        CMIC_INTR_HANDLE(unit, intr)->intr_fn(unit,
                            CMIC_INTR_HANDLE(unit, intr)->intr_data);
                }
            }
            irqStat = irqStat & ~mask;
        }
    }

    sal_spl(s);

}

#ifdef SEPARATE_PKTDMA_INTR_HANDLER
/*******************************************
* @function soc_cmicx_pktdma_intr
* purpose SOC CMICX Interrupt Service Routine
*
* @param unit [in] unit
*
*
* @end
 */
void
soc_cmicx_pktdma_intr(void *_unit)
{
    soc_control_t *soc;
    uint32 irqStat, irqMask, mask;
#ifdef BROADCOM_DEBUG
    uint32 irqEnable;
#endif
    int unit = PTR_TO_INT(_unit);
    int i = 0;
    int intr, s;

    s = sal_splhi();

    /* Clear MSI interrupts immediately to prevent spurious interrupts */
    WRITE_PAXB_0_PAXB_IC_INTRCLR_0r(unit, 0xFFFFFFFF);
    WRITE_PAXB_0_PAXB_IC_INTRCLR_1r(unit, 0xFFFFFFFF);
    soc = SOC_CONTROL(unit);

    if (soc == NULL || (soc->soc_flags & SOC_F_BUSY) ||
        !(soc->soc_flags & SOC_F_ATTACHED)) {
        sal_spl(s);
        return;
    }

    soc->stat.intr++; /* Update count */

    i = IRQ_MASK_INDEX(CMC0_CH0_DESC_DONE);

#ifdef INCLUDE_KNET
    if (SOC_KNET_MODE(unit) && i == IRQ_MASK_INDEX(CMC0_CH0_DESC_DONE)) {
        return;
    }
#endif

    if ((soc_cm_get_bus_type(unit) & SOC_AXI_DEV_TYPE) &&
        (i < IRQ_MASK_INDEX(CHIP_INTR_LOW_PRIORITY))) {
        return;
    }

    READ_INTC_INTR(unit, _irq_reg_map[i].stat, &irqStat);

    if ((soc_cm_get_bus_type(unit) & SOC_AXI_DEV_TYPE) &&
        (i == IRQ_MASK_INDEX(CHIP_INTR_LOW_PRIORITY))) {
        irqStat &= 0x01 << IRQ_BIT(CHIP_INTR_LOW_PRIORITY);
    }

    irqMask = IRQ_MASK(unit, i);

    /* Re-Enable Enabled-but-Unserviced interrupts */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        IRQ_MASK(unit, i) = (irqMask & ~irqStat);
    } else {
        if (soc_cm_get_bus_type(unit) & SOC_AXI_DEV_TYPE) {
            IHOST_ENABLE_IRQ(unit, i, (irqMask & ~irqStat), (irqMask & ~irqStat));
        } else {
            IRQ_MASK_SET(unit, i, (irqMask & ~irqStat));
        }
    }
    LOG_VERBOSE(BSL_LS_SOC_INTR,
                (BSL_META_U(unit,
                 "%s:unit %d, stat reg %s, val = 0x%x mask = 0x%x prog-enable = 0x%x\n"),
                 __func__, unit, _irq_reg_map[i].stat_nm, irqStat, irqMask, (irqMask & ~irqStat)));

#ifdef BROADCOM_DEBUG
    if (soc_cm_get_bus_type(unit) & SOC_AXI_DEV_TYPE) {
        READ_INTC_INTR(unit, _ihost_irq_reg_map[i].enable, &irqEnable);
    } else {
        READ_INTC_INTR(unit, _irq_reg_map[i].enable, &irqEnable);
    }
    LOG_VERBOSE(BSL_LS_SOC_INTR, (BSL_META_U(unit,
                                                "%s:unit %d, enable reg %s 0x%x\n"),
                                                __func__, unit, _irq_reg_map[i].stat_nm, irqEnable));
#endif

    while (irqStat) {
         intr = _irq_least_bit_set(irqStat);
         mask = 0x01 << intr;
         intr += i * (sizeof(uint32) << 3);
         if ((irqMask & mask) && IS_IRQ_VALID(intr)) {
             if ((CMIC_INTR_HANDLE(unit, intr)->num == intr) &&
                 (CMIC_INTR_HANDLE(unit, intr)->intr_fn != NULL)) {

                   LOG_VERBOSE(BSL_LS_SOC_INTR, (BSL_META_U(unit,
                                                 "%s:unit %d, intr %u\n"),
                                                 __func__, unit, intr));

                     CMIC_INTR_HANDLE(unit, intr)->intr_fn(unit,
                         CMIC_INTR_HANDLE(unit, intr)->intr_data);
             }
         }
         irqStat = irqStat & ~mask;
     }

    sal_spl(s);
}

#endif

#endif /* CMICX Support */
