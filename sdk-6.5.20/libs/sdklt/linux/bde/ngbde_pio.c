/*! \file ngbde_pio.c
 *
 * API for managing and accessing memory-mapped I/O for switch
 * registers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <ngbde.h>

void *
ngbde_pio_map(void *devh, phys_addr_t addr, phys_addr_t size)
{
    struct ngbde_dev_s *sd = (struct ngbde_dev_s *)devh;

    if (sd->pio_mem) {
        if (addr == sd->pio_win.addr && size == sd->pio_win.size) {
            /* Already mapped */
            return sd->pio_mem;
        }
        ngbde_pio_unmap(devh);
    }

    sd->pio_mem = ioremap_nocache(addr, size);

    if (sd->pio_mem) {
        /* Save mapped resources */
        sd->pio_win.addr = addr;
        sd->pio_win.size = size;
    } else {
        printk(KERN_WARNING "%s: Unable to map address 0x%08lu\n",
               MOD_NAME, (unsigned long)addr);
    }

    return sd->pio_mem;
}

void
ngbde_pio_unmap(void *devh)
{
    struct ngbde_dev_s *sd = (struct ngbde_dev_s *)devh;

    if (sd->pio_mem) {
        iounmap(sd->pio_mem);
        sd->pio_mem = NULL;
    }
}

void
ngbde_pio_cleanup(void)
{
    struct ngbde_dev_s *swdev, *sd;
    unsigned int num_swdev, idx;

    ngbde_swdev_get_all(&swdev, &num_swdev);

    for (idx = 0; idx < num_swdev; idx++) {
        sd = ngbde_swdev_get(idx);
        ngbde_pio_unmap(sd);
    }
}

void
ngbde_pio_write32(void *devh, uint32_t offs, uint32_t val)
{
    struct ngbde_dev_s *sd = (struct ngbde_dev_s *)devh;

    if (sd->pio_mem) {
        NGBDE_IOWRITE32(val, sd->pio_mem + offs);
    }
}

uint32_t
ngbde_pio_read32(void *devh, uint32_t offs)
{
    struct ngbde_dev_s *sd = (struct ngbde_dev_s *)devh;

    if (sd->pio_mem) {
        return NGBDE_IOREAD32(sd->pio_mem + offs);
    }
    return 0;
}
