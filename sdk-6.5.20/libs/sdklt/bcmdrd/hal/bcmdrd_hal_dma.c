/*! \file bcmdrd_hal_dma.c
 *
 * API for managing DMA memory.
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

void *
bcmdrd_hal_dma_alloc(int unit, size_t size, const char *s, uint64_t *dma_addr)
{
    bcmdrd_dev_t *dev = bcmdrd_dev_get(unit);
    bcmdrd_hal_dma_t *dma;

    assert(dev);

    dma = &dev->dma;

    if (dma->dma_alloc == NULL) {
        return NULL;
    }

    return dma->dma_alloc(dma->devh, size, dma_addr);
}

void
bcmdrd_hal_dma_free(int unit, size_t size, void *dma_mem, uint64_t dma_addr)
{
    bcmdrd_dev_t *dev = bcmdrd_dev_get(unit);
    bcmdrd_hal_dma_t *dma;

    assert(dev);

    dma = &dev->dma;

    if (dma->dma_free) {
        dma->dma_free(dma->devh, size, dma_mem, dma_addr);
    }
}

uint64_t
bcmdrd_hal_dma_map(int unit, void *dma_mem, size_t size, bool to_dev)
{
    bcmdrd_dev_t *dev = bcmdrd_dev_get(unit);
    bcmdrd_hal_dma_t *dma;

    assert(dev);

    dma = &dev->dma;

    if (dma->dma_map == NULL) {
        return 0;
    }

    return dma->dma_map(dma->devh, dma_mem, size, to_dev);
}

void
bcmdrd_hal_dma_unmap(int unit, uint64_t dma_addr, size_t size, bool to_dev)
{
    bcmdrd_dev_t *dev = bcmdrd_dev_get(unit);
    bcmdrd_hal_dma_t *dma;

    assert(dev);

    dma = &dev->dma;

    if (dma->dma_unmap) {
        dma->dma_unmap(dma->devh, dma_addr, size, to_dev);
    }
}
