/*! \file bcmdrd_hal_reg.c
 *
 * Device access functions for HMI registers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_assert.h>

#include <shr/shr_error.h>

#include <bcmdrd/bcmdrd_internal.h>
#include <bcmdrd/bcmdrd_dev.h>

/*! Currently we only support a single reg32 window per device. */
volatile uint32_t *bcmdrd_reg32_iomem[BCMDRD_CONFIG_MAX_UNITS];

/*! Save size for unmapping. */
static size_t iomem_size[BCMDRD_CONFIG_MAX_UNITS];

int
bcmdrd_hal_reg32_init(int unit, unsigned int iomem_idx, size_t size)
{
    uint32_t **iomem;
    bcmdrd_dev_t *dev = bcmdrd_dev_get(unit);
    bcmdrd_hal_io_t *io;

    assert(dev);

    iomem = (uint32_t **)&bcmdrd_reg32_iomem[unit];
    if (*iomem != NULL) {
        return SHR_E_EXISTS;
    }

    io = &dev->io;

    if (io->ioremap == NULL) {
        /*
         * If no memory mapping function is specified, then we expect
         * to use function calls instead of memory mapped I/O.
         */
        if (io->read == NULL || io->write == NULL) {
            return SHR_E_INIT;
        }
#if BCMDRD_CONFIG_MEMMAP_DIRECT == 1
        {
            /* Check for invalid build configuration */
            int memmap_direct_not_supported = 0;
            assert(memmap_direct_not_supported);
        }
#endif
        return SHR_E_NONE;
    }

    *iomem = io->ioremap(io->phys_addr[iomem_idx], size);
    iomem_size[unit] = size;

    return (*iomem != NULL) ? SHR_E_NONE : SHR_E_FAIL;
}

int
bcmdrd_hal_reg32_cleanup(int unit)
{
    uint32_t *iomem32;
    bcmdrd_dev_t *dev = bcmdrd_dev_get(unit);
    bcmdrd_hal_io_t *io;

    assert(dev);

    io = &dev->io;

    iomem32 = (uint32_t *)bcmdrd_reg32_iomem[unit];
    if (io->iounmap && iomem32) {
        io->iounmap(iomem32, iomem_size[unit]);
        bcmdrd_reg32_iomem[unit] = NULL;
        iomem_size[unit] = 0;
    }

    return SHR_E_NONE;
}

int
bcmdrd_hal_reg32_read(int unit, uint32_t addr, uint32_t *val)
{
    int rv;
    volatile uint32_t *iomem32;
    bcmdrd_dev_t *dev = bcmdrd_dev_get(unit);
    bcmdrd_hal_io_t *io;

    assert(dev);

    iomem32 = bcmdrd_reg32_iomem[unit];
    if (iomem32) {
        *val = iomem32[addr/4];
        return SHR_E_NONE;
    }

    io = &dev->io;
    assert(io->read);

    rv = io->read(io->devh, BCMDRD_IOS_DEFAULT, addr, val, 4);

    return (rv >= 0) ? SHR_E_NONE : SHR_E_FAIL;
}

int
bcmdrd_hal_reg32_write(int unit, uint32_t addr, uint32_t val)
{
    int rv;
    volatile uint32_t *iomem32;
    bcmdrd_dev_t *dev = bcmdrd_dev_get(unit);
    bcmdrd_hal_io_t *io;

    assert(dev);

    iomem32 = bcmdrd_reg32_iomem[unit];
    if (iomem32) {
        iomem32[addr/4] = val;
        return SHR_E_NONE;
    }

    io = &dev->io;
    assert(io->write);

    rv = io->write(io->devh, BCMDRD_IOS_DEFAULT, addr, &val, 4);

    return (rv >= 0) ? SHR_E_NONE : SHR_E_FAIL;
}
