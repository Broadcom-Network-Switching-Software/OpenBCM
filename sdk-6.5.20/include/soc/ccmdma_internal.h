/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ccmdma_internal.h
 * Purpose:     Maps out structures used for CCM DMA operations and
 *              internal routines.
 */

#ifndef _SOC_CCMDMA_INTERNAL_H
#define _SOC_CCMDMA_INTERNAL_H

#include <soc/defs.h>
#include <soc/types.h>
#include <sal/types.h>
#include <sal/core/time.h>
#include <sal/core/sync.h>
#include <sal/core/thread.h>
#include <soc/ccmdma.h>

#define CMIC_CMCx_CCMDMA_CHSELECT_TIMEOUT    (1000)


/* CCM DMA function pointers */
typedef struct soc_ccmdma_drv_s {
    int (*soc_ccmdma_ch_get)(int unit, int *vchan);
    int (*soc_ccmdma_ch_put)(int unit, int vchan);
    int (*soc_ccmdma_copy)(int unit, sal_paddr_t *srcbuf, sal_paddr_t *dstbuf,
                           unsigned int src_is_internal, unsigned int dst_is_internal,
                           int count, int flags, int vchan);
    int (*soc_ccmdma_copy_wait)(int unit, int vchan);
    int (*soc_ccmdma_abort)(int unit, int vchan);
    int (*soc_ccmdma_clean)(int unit, int max_cmc);
}soc_ccmdma_drv_t;

/*******************************************
* @function cmicm_ccmdma_init
* purpose API to Initialize cmicm CCM DMA
*
* @param unit [in] unit
* @param drv [out] soc_ccmdma_drv_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int cmicm_ccmdma_init(int unit, soc_ccmdma_drv_t *drv);

/*******************************************
* @function cmicm_ccmdma_deinit
* purpose API to Deinitialize cmicm CCM DMA
*
* @param unit [in] unit
* @param drv [in] soc_ccmdma_drv_t pointer
*
* @returns SOC_E_NONE

*
* @end
 */
extern int cmicm_ccmdma_deinit(int unit, soc_ccmdma_drv_t *drv);

/*******************************************
* @function cmicx_ccmdma_init
* purpose API to Initialize cmicx CCM DMA
*
* @param unit [in] unit
* @param drv [out] soc_ccmdma_drv_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int cmicx_ccmdma_init(int unit, soc_ccmdma_drv_t *drv);

/*******************************************
* @function cmicx_ccmdma_deinit
* purpose API to Deinitialize cmicx CCM DMA
*
* @param unit [in] unit
* @param drv [in] soc_ccmdma_drv_t pointer
*
* @returns SOC_E_NONE

*
* @end
 */
extern int cmicx_ccmdma_deinit(int unit, soc_ccmdma_drv_t *drv);

#endif /* !_SOC_CCMDMA_INTERNAL_H */
