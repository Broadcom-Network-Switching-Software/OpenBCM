/*! \file ngbde_iio.c
 *
 * API for managing and accessing memory-mapped I/O for interrupt
 * controller registers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <ngbde.h>

void *
ngbde_iio_map(void *devh, phys_addr_t addr, phys_addr_t size)
{
    struct ngbde_dev_s *sd = (struct ngbde_dev_s *)devh;

    if (sd->iio_mem) {
        if (addr == sd->iio_win.addr && size == sd->iio_win.size) {
            /* Already mapped */
            return sd->iio_mem;
        }
        ngbde_iio_unmap(devh);
    }

    sd->iio_mem = ioremap_nocache(addr, size);

    if (sd->iio_mem) {
        /* Save mapped resources */
        sd->iio_win.addr = addr;
        sd->iio_win.size = size;
    }

    return sd->iio_mem;
}

void
ngbde_iio_unmap(void *devh)
{
    struct ngbde_dev_s *sd = (struct ngbde_dev_s *)devh;

    if (sd->iio_mem) {
        iounmap(sd->iio_mem);
        sd->iio_mem = NULL;
    }
}

void
ngbde_iio_cleanup(void)
{
    struct ngbde_dev_s *swdev, *sd;
    unsigned int num_swdev, idx;

    ngbde_swdev_get_all(&swdev, &num_swdev);

    for (idx = 0; idx < num_swdev; idx++) {
        sd = ngbde_swdev_get(idx);
        ngbde_iio_unmap(sd);
    }
}

void
ngbde_iio_write32(void *devh, uint32_t offs, uint32_t val)
{
    struct ngbde_dev_s *sd = (struct ngbde_dev_s *)devh;

    if (sd->iio_mem) {
        NGBDE_IOWRITE32(val, sd->iio_mem + offs);
    }
}

uint32_t
ngbde_iio_read32(void *devh, uint32_t offs)
{
    struct ngbde_dev_s *sd = (struct ngbde_dev_s *)devh;

    if (sd->iio_mem) {
        return NGBDE_IOREAD32(sd->iio_mem + offs);
    }
    return 0;
}

void *
ngbde_paxb_map(void *devh, phys_addr_t addr, phys_addr_t size)
{
    struct ngbde_dev_s *sd = (struct ngbde_dev_s *)devh;

    if (sd->paxb_mem) {
        iounmap(sd->paxb_mem);
    }

    sd->paxb_mem = ioremap_nocache(addr, size);

    return sd->paxb_mem;
}

void
ngbde_paxb_unmap(void *devh)
{
    struct ngbde_dev_s *sd = (struct ngbde_dev_s *)devh;

    if (sd->paxb_mem) {
        iounmap(sd->paxb_mem);
        sd->paxb_mem = NULL;
    }
}

void
ngbde_paxb_cleanup(void)
{
    struct ngbde_dev_s *swdev, *sd;
    unsigned int num_swdev, idx;

    ngbde_swdev_get_all(&swdev, &num_swdev);

    for (idx = 0; idx < num_swdev; idx++) {
        sd = ngbde_swdev_get(idx);
        ngbde_paxb_unmap(sd);
    }
}
