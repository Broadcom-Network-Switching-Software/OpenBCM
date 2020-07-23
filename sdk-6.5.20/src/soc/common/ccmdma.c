/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: SOC CCM DMA driver.
 */

#include <shared/bsl.h>

#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>

#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/ccmdma_internal.h>

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
#ifdef BCM_CCMDMA_SUPPORT

#if (!defined(_LIMITS_H)) && !defined(_LIBC_LIMITS_H_)

#if (!defined(INT_MIN)) && !defined(INT_MAX)
#define INT_MIN (((int)1)<<(sizeof(int)*8-1))
#define INT_MAX (~INT_MIN)
#endif

#ifndef UINT_MAX
#define UINT_MAX ((unsigned)-1)
#endif

#endif

STATIC soc_ccmdma_drv_t _ccmdma_drv[SOC_MAX_NUM_DEVICES];

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
soc_ccmdma_ch_get(int unit, int *vchan)
{
    int rv = SOC_E_UNAVAIL;

    if (_ccmdma_drv[unit].soc_ccmdma_ch_get != NULL) {
        rv = _ccmdma_drv[unit].soc_ccmdma_ch_get(unit, vchan);
    }

    return rv;
}

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
soc_ccmdma_ch_put(int unit, int vchan)
{
    int rv = SOC_E_UNAVAIL;

    if (_ccmdma_drv[unit].soc_ccmdma_ch_put != NULL) {
        rv = _ccmdma_drv[unit].soc_ccmdma_ch_put(unit, vchan);
    }

    return rv;
}

/*******************************************
* @function soc_ccmdma_copy
* purpose to initiate DMA from source to
*            destination memory on vchan
* @param srcbuf [in] pointer to source memory
* @param dstbuf [in] pointer to dest memory
* @param count [in] number of bytes to xfer
* @param flags [in]
* @param vchan [in] channel number
*
* @returns SOC_E_XXXX
* @returns SOC_E_NONE
*
* @end
 */
int
soc_ccmdma_copy(int unit,  sal_paddr_t *srcbuf, sal_paddr_t *dstbuf,
                unsigned int src_is_internal, unsigned int dst_is_internal,
                int count, int flags, int vchan)
{
    int rv = SOC_E_UNAVAIL;

    if (_ccmdma_drv[unit].soc_ccmdma_copy != NULL) {
        rv = _ccmdma_drv[unit].soc_ccmdma_copy(unit, srcbuf, dstbuf,
                                               src_is_internal, dst_is_internal,
                                               count, flags, vchan);
    }

    return rv;
}

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
soc_ccmdma_copy_wait(int unit, int vchan)
{
    int rv = SOC_E_UNAVAIL;

    if (_ccmdma_drv[unit].soc_ccmdma_copy_wait!= NULL) {
        rv = _ccmdma_drv[unit].soc_ccmdma_copy_wait(unit, vchan);
    }

    return rv;
}

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
soc_ccmdma_abort(int unit, int vchan)
{
    int rv = SOC_E_UNAVAIL;

    if (_ccmdma_drv[unit].soc_ccmdma_abort != NULL) {
        rv = _ccmdma_drv[unit].soc_ccmdma_abort(unit, vchan);
    }

    return rv;
}

/*******************************************
* @function soc_ccmdma_clean
* purpose to clean ccmdma
* @param max_cmx [in] max cmc
*
* @returns SOC_E_XXXX
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_ccmdma_clean(int unit, int max_cmc)
{
    int rv = SOC_E_UNAVAIL;

    if (_ccmdma_drv[unit].soc_ccmdma_clean != NULL) {
        rv = _ccmdma_drv[unit].soc_ccmdma_clean(unit, max_cmc);
    }

    return rv;
}

/*******************************************
* @function soc_ccmdma_init
* purpose API to Initialize CCM DMA
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int soc_ccmdma_init(int unit)
{
#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {
        SOC_IF_ERROR_RETURN(cmicm_ccmdma_init(unit,
                            &_ccmdma_drv[unit]));
    }
#endif
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        SOC_IF_ERROR_RETURN(cmicx_ccmdma_init(unit,
                            &_ccmdma_drv[unit]));
    }
#endif
    return SOC_E_NONE;
}

/*******************************************
* @function soc_ccmdma_deinit
* purpose API to Deinitialize CCM DMA
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
*
* @end
 */
int soc_ccmdma_deinit(int unit)
{
#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {

        SOC_IF_ERROR_RETURN(cmicm_ccmdma_deinit(unit,
                            &_ccmdma_drv[unit]));
    }
#endif
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {

        SOC_IF_ERROR_RETURN(cmicx_ccmdma_deinit(unit,
                            &_ccmdma_drv[unit]));
    }
#endif
    return SOC_E_NONE;
}

#endif /* BCM_CCMDMA_SUPPORT */
#endif /* defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) */
