/*! \file bcm56980_a0_pdma_intr.c
 *
 * Initialize PDMA driver resources.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef INCLUDE_PKTIO

#include <pktio_dep.h>
#include <soc/intr.h>

/*! Interrupt handler */
typedef void (*bcmcnet_intr_f)(int unit, uint32_t data);

typedef struct pdma_intr_info_s {
    int inum;
    bcmcnet_intr_f intr_fn;
    uint32 idata;
} pdma_intr_info_t;

#define PDMA_INTR_INFO_MAX 16

typedef struct pdma_intr_handle_s{
    pdma_intr_info_t intr_info[PDMA_INTR_INFO_MAX];
} pdma_intr_handle_t;

static pdma_intr_handle_t pdma_handler[SOC_MAX_NUM_DEVICES];

static int intr_info_initialized[SOC_MAX_NUM_DEVICES] = {0};

static void
bcm56980_a0_cnet_intr_dispatch(int unit, void *data)
{
    pdma_intr_info_t *iinfo = data;

    if (iinfo->intr_fn) {
        iinfo->intr_fn(unit, iinfo->idata);
    }
    return;
}

int
bcm56980_a0_cnet_pdma_intr_enable(int unit, int cmc, int inum)
{
    return soc_cmic_intr_enable(unit, inum);
}

int
bcm56980_a0_cnet_pdma_intr_disable(int unit, int cmc, int inum)
{
    return soc_cmic_intr_disable(unit, inum);
}

int
bcm56980_a0_cnet_pdma_intr_connect(int unit, int cmc, int inum,
                                   bcmcnet_intr_f ifunc, uint32_t idata)
{
    int i;
    soc_cmic_intr_handler_t handle;
    pdma_intr_info_t *iinfo;

    if (!intr_info_initialized[unit]) {
        sal_memset(&pdma_handler[unit], 0, sizeof(pdma_intr_handle_t));
        intr_info_initialized[unit] = 1;
    }

    for (i = 0; i < PDMA_INTR_INFO_MAX; i ++) {
        iinfo = &pdma_handler[unit].intr_info[i];
        if (iinfo->inum == inum) {
            break;
        }
        if (iinfo->inum == 0) {
            iinfo->inum = inum;
            iinfo->idata = idata;
            iinfo->intr_fn = ifunc;
            break;
        }
    }
    if (i == PDMA_INTR_INFO_MAX) {
        return SOC_E_INTERNAL;
    }

    handle.num = inum;
    handle.intr_fn = bcm56980_a0_cnet_intr_dispatch;
    handle.intr_data = iinfo;

    return soc_cmic_intr_register(unit, &handle, 1);
}

#endif /* INCLUDE_PKTIO */
