/*! \file bcmbd_cmicd_init.c
 *
 * CMICd initialization.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmbd/bcmbd_cmicd_acc.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>

/*
 * Basic CMIC Initialization
 */
#define CMIC_BIG_ENDIAN_PIO             0x01000001

int
bcmbd_cmicd_init(int unit)
{
    bool byte_swap_pio;
    CMIC_CMC_PCIE_IRQ_MASK0r_t irq_mask0;
    CMIC_CMC_PCIE_IRQ_MASK1r_t irq_mask1;
    CMIC_COMMON_UC0_PIO_ENDIANESSr_t uc0_pio_en;
    CMIC_COMMON_PCIE_PIO_ENDIANESSr_t pcie_pio_en;
    uint32_t io_flags;
    int cmc = BCMBD_CMICD_CMC_GET(unit);
    int ioerr = 0;

    /*
     * Certain PCIe cores may occasionally return invalid data in the
     * first PCI read following a soft-reset (CPS reset). The following
     * read operation is a dummy read to ensure that any invalid data
     * is flushed from the PCI read pipeline.
     */
    ioerr += READ_CMIC_CMC_PCIE_IRQ_MASK0r(unit, cmc, &irq_mask0);
    ioerr += READ_CMIC_CMC_PCIE_IRQ_MASK1r(unit, cmc, &irq_mask1);

    /* Configure endian */
    byte_swap_pio = bcmdrd_dev_byte_swap_pio_get(unit);
    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_IPROC)) {
        CMIC_COMMON_UC0_PIO_ENDIANESSr_CLR(uc0_pio_en);
        if (byte_swap_pio) {
            CMIC_COMMON_UC0_PIO_ENDIANESSr_SET(uc0_pio_en, CMIC_BIG_ENDIAN_PIO);
        }
        ioerr += WRITE_CMIC_COMMON_UC0_PIO_ENDIANESSr(unit, uc0_pio_en);
    } else {
        CMIC_COMMON_PCIE_PIO_ENDIANESSr_CLR(pcie_pio_en);
        if (byte_swap_pio) {
            CMIC_COMMON_PCIE_PIO_ENDIANESSr_SET(pcie_pio_en, CMIC_BIG_ENDIAN_PIO);
        }
        ioerr += WRITE_CMIC_COMMON_PCIE_PIO_ENDIANESSr(unit, pcie_pio_en);
    }

    /* Disable Interrupts */
    CMIC_CMC_PCIE_IRQ_MASK0r_CLR(irq_mask0);
    ioerr += WRITE_CMIC_CMC_PCIE_IRQ_MASK0r(unit, cmc, irq_mask0);
    CMIC_CMC_PCIE_IRQ_MASK1r_CLR(irq_mask1);
    ioerr += WRITE_CMIC_CMC_PCIE_IRQ_MASK1r(unit, cmc, irq_mask1);

    

    /* Additional configuration required when on PCI bus */
    if (bcmdrd_dev_bus_type_get(unit) == BCMDRD_BT_PCI) {
        int idx;
        CMIC_CMC_HOSTMEM_ADDR_REMAPr_t hostmem_remap;
        uint32_t remap_val[] = { 0x144D2450, 0x19617595, 0x1E75C6DA, 0x1f };

        /* Send DMA data to external host memory when on PCI bus */
        for (idx = 0; idx < COUNTOF(remap_val); idx++) {
            CMIC_CMC_HOSTMEM_ADDR_REMAPr_SET(hostmem_remap, remap_val[idx]);
            ioerr += WRITE_CMIC_CMC_HOSTMEM_ADDR_REMAPr(unit, cmc, idx,
                                                        hostmem_remap);
        }

        /* Enable MSI in the CMIC as needed */
        io_flags = bcmdrd_dev_io_flags_get(unit);
        if (io_flags & BCMDRD_HAL_IO_F_MSI) {
            CMIC_CMC_PCIE_MISCELr_t pcie_miscel;

            ioerr += READ_CMIC_CMC_PCIE_MISCELr(unit, cmc, &pcie_miscel);
            CMIC_CMC_PCIE_MISCELr_ENABLE_MSIf_SET(pcie_miscel, 1);
            ioerr += WRITE_CMIC_CMC_PCIE_MISCELr(unit, cmc, pcie_miscel);
        }
    }

    return ioerr;
}
