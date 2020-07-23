/*! \file bcmdrd_dev_ha_dma.c
 *
 * High-level API for high-availability DMA memory block.
 *
 * This API simply stores the infomation about the DMA memory
 * block. The HA DMA memory client is responsible for alloc/free
 * management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_dev.h>

/*******************************************************************************
* Local definitions
 */

#define HA_DMA_MEM_SIZE_DEFAULT (4 * 1024 * 1024)

/*! DMA memory block information. */
typedef struct ha_dma_info_s {

    /*! Physical address of DMA memory block. */
    uint64_t dma_addr;

    /*! Logical address of DMA memory block. */
    void *dma_mem;

    /*! Size (in bytes) of DMA memory block. */
    size_t dma_size;

} ha_dma_info_t;

static ha_dma_info_t ha_dma_info[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Public Functions
 */

int
bcmdrd_dev_ha_dma_mem_set(int unit, size_t size,
                          void *ha_dma_mem, uint64_t dma_addr)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    ha_dma_info[unit].dma_size = size;
    ha_dma_info[unit].dma_mem = ha_dma_mem;
    ha_dma_info[unit].dma_addr = dma_addr;
    return SHR_E_NONE;
}

void *
bcmdrd_dev_ha_dma_mem_get(int unit, size_t *size, uint64_t *dma_addr)
{
    if (size) {
        *size = 0;
    }
    if (!BCMDRD_UNIT_VALID(unit)) {
        return NULL;
    }
    if (dma_addr == NULL) {
        if (size) {
            *size = HA_DMA_MEM_SIZE_DEFAULT;
        }
        return NULL;
    }
    if (size) {
        *size = ha_dma_info[unit].dma_size;
    }
    *dma_addr = ha_dma_info[unit].dma_addr;
    return ha_dma_info[unit].dma_mem;
}
