/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ccmdma.h
 * Purpose:     Maps out structures used for CCMDMA operations and
 *              exports routines.
 */

#ifndef _SOC_CCMDMA_H
#define _SOC_CCMDMA_H

#include <soc/defs.h>
#include <soc/types.h>
#include <sal/types.h>
#include <sal/core/time.h>
#include <sal/core/sync.h>
#include <sal/core/thread.h>

/*******************************************
* @function soc_ccmdma_ch_get
* purpose get idle DMA channel
*
* @param unit [in] unit #
* @param vchan [out] channel number
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_ccmdma_ch_get(int unit, int *vchan);

/*******************************************
* @function soc_ccmdma_ch_put
* purpose put back the channel on free list
*
* @param ch [in] channel number
*
* @returns SOC_E_PARAM
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_ccmdma_ch_put(int unit, int vchan);

/*******************************************
* @function soc_ccmdma_copy_wait
* purpose to initiate DMA from source to
*            destination memory on vchan
* @param vchan [in] channel number
*
* @returns SOC_E_XXXX
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_ccmdma_copy_wait(int unit, int vchan);

/*******************************************
* @function soc_ccmdma_copy
* purpose to initiate DMA from source to
*            destination memory on vchan
* @param srcbuf [in] pointer to source memory
* @param dstbuf [in] pointer to dest memory
* @param src_is_internal [in] is src an internal memory
* @param dst_is_internal [in] is dst an internal memory
* @param count [in] number of bytes to xfer
* @param flags [in]
* @param vchan [in] channel number
*
* @returns SOC_E_XXXX
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_ccmdma_copy(int unit, sal_paddr_t *srcbuf, sal_paddr_t *dstbuf,
                unsigned int src_is_internal, unsigned int dst_is_internal,
                int count, int flags, int vchan);
/*******************************************
* @function soc_ccmdma_abort
* purpose to abort active DMA operation on
*            given channel
* @param vchan [in] channel number
*
* @returns SOC_E_XXXX
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_ccmdma_abort(int unit, int vchan);

/*******************************************
* @function soc_ccmdma_clean
* purpose to clean
* @param max_cmc [in] number of cmc
*
* @returns SOC_E_XXXX
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_ccmdma_clean(int unit, int max_cmc);

extern int
soc_ccmdma_init(int unit);

extern int
soc_ccmdma_deinit(int unit);

#endif /* !_SOC_CCMDMA_H */
