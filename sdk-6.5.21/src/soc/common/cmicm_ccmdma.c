/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: CMICM CCM DMA Driver
 */

#include <shared/bsl.h>
#include <shared/alloc.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/ccmdma_internal.h>

#if defined(BCM_CMICM_SUPPORT) && defined(BCM_CCMDMA_SUPPORT)
#include <soc/cmicm.h>
#endif

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
#if defined(BCM_CMICM_SUPPORT) && defined(BCM_CCMDMA_SUPPORT)

typedef struct cmicm_ccmdma_ch_s {
    sal_spinlock_t  lock[SOC_MAX_NUM_DEVICES][CMIC_CMC_NUM_MAX];
    int timeout;
    uint32  ch[SOC_MAX_NUM_DEVICES][CMIC_CMC_NUM_MAX];
}cmicm_ccmdma_ch_t;

STATIC cmicm_ccmdma_ch_t  _cmicm_ccmdma_ch;

/*******************************************
* @function _cmicm_ccmdma_ch_get
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
static int
_cmicm_ccmdma_ch_get(int unit, int *vchan)
{
    int rv = SOC_E_BUSY;
    int j;
    soc_timeout_t to;

    soc_timeout_init(&to, _cmicm_ccmdma_ch.timeout, 10);
    do {
        for(j = 0; j < CMIC_CMC_NUM_MAX; j++) {
            sal_spinlock_lock(_cmicm_ccmdma_ch.lock[unit][j]);
            if(_cmicm_ccmdma_ch.ch[unit][j] == 1) {
                *vchan = j;
                _cmicm_ccmdma_ch.ch[unit][j] = 0;
                sal_spinlock_unlock(_cmicm_ccmdma_ch.lock[unit][j]);
                rv = SOC_E_NONE;
                break;
            }
            sal_spinlock_unlock(_cmicm_ccmdma_ch.lock[unit][j]);
        }
        if (rv == SOC_E_NONE) {
            break;
        }
    } while (!soc_timeout_check(&to));
    return rv;
}

/*******************************************
* @function _cmicm_ccmdma_ch_put
* purpose put back the channel on free list
*
* @param ch [in] channel number
*
* @returns SOC_E_PARAM
* @returns SOC_E_NONE
*
* @end
 */
static int
_cmicm_ccmdma_ch_put(int unit, int vchan)
{
    sal_spinlock_lock(_cmicm_ccmdma_ch.lock[unit][vchan]);
    _cmicm_ccmdma_ch.ch[unit][vchan] = 1;
    sal_spinlock_unlock(_cmicm_ccmdma_ch.lock[unit][vchan]);
    return SOC_E_NONE;
}

/*******************************************
* @function _cmicm_ccmdma_copy
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
static int
_cmicm_ccmdma_copy(int unit, sal_paddr_t *srcbuf, sal_paddr_t *dstbuf,
                   unsigned int src_is_internal, unsigned int dst_is_internal,
                   int count, int flags, int vchan)
{
    int i, rv;
    soc_control_t *soc = SOC_CONTROL(unit);
    uint32 *srcptr;
    uint32 *dstptr;
    uint32 reg, reg2;
    int cmc = vchan; /* as one channel/CMC in CMICM/D */

    if (src_is_internal) {
#ifdef PTRS_ARE_64BITS
         srcbuf = (sal_paddr_t *)soc_cm_p2l(unit, ((uint64)srcbuf | 0xf0000000));
#else
         srcbuf = (sal_paddr_t *)soc_cm_p2l(unit, ((uint32)srcbuf | 0xf0000000));
#endif
    }

    if (dst_is_internal) {
#ifdef PTRS_ARE_64BITS
         dstbuf = (sal_paddr_t *)soc_cm_p2l(unit, ((uint64)dstbuf | 0xf0000000));
#else
         dstbuf = (sal_paddr_t *)soc_cm_p2l(unit, ((uint32)dstbuf | 0xf0000000));
#endif
    }

    srcptr = (uint32 *)srcbuf;
    dstptr = (uint32 *)dstbuf;

    assert (srcptr && dstptr);

    if (SOC_CONTROL(unit)->ccmDmaMutex[cmc] == 0) {
        /* If DMA not enabled, or short length use PIO to copy */
        for (i = 0; i < count; i ++ ) {
            if (flags & 2) {            /* Read from PCI */
                reg = soc_pci_mcs_read(unit, PTR_TO_INT(srcptr));
            } else {
                reg = *srcptr;
            }

            if (flags & 1) {
                reg = ((reg & 0xff000000) >> 24) |
                    ((reg & 0x00ff0000) >> 8) |
                    ((reg & 0x0000ff00) << 8) |
                    ((reg & 0x000000ff) << 24);
            }

            if (flags & 2) {
                *dstptr = reg;
            } else {
                soc_pci_mcs_write(unit, PTR_TO_INT(dstptr), reg);
                reg2 = soc_pci_mcs_read(unit, PTR_TO_INT(dstptr));
                if (reg2 != reg) {
                    LOG_ERROR(BSL_LS_SOC_SOCMEM,
                              (BSL_META_U(unit,
                                          "ccm_dma: compare error %x (%x %x)\n"),
                               PTR_TO_INT(dstptr), reg, reg2));
                }
            }

            dstptr++;
            srcptr++;
        }
        return SOC_E_NONE;
    }

    CCM_DMA_LOCK(unit, cmc);

    soc_pci_write(unit, CMIC_CMCx_CCM_DMA_HOST0_MEM_START_ADDR_OFFSET(cmc), soc_cm_l2p(unit, srcbuf));
    soc_pci_write(unit, CMIC_CMCx_CCM_DMA_HOST1_MEM_START_ADDR_OFFSET(cmc), soc_cm_l2p(unit, dstbuf));
    soc_pci_write(unit, CMIC_CMCx_CCM_DMA_ENTRY_COUNT_OFFSET(cmc), count);
    /* Keep endianess default... */
    reg = soc_pci_read(unit, CMIC_CMCx_CCM_DMA_CFG_OFFSET(cmc));
    soc_reg_field_set(unit, CMIC_CMC0_CCM_DMA_CFGr, &reg, ABORTf, 0);
    soc_reg_field_set(unit, CMIC_CMC0_CCM_DMA_CFGr, &reg, ENf, 0);
    soc_pci_write(unit, CMIC_CMCx_CCM_DMA_CFG_OFFSET(cmc), reg);  /* Clearing EN clears stats */
    /* Start DMA */
    soc_reg_field_set(unit, CMIC_CMC0_CCM_DMA_CFGr, &reg, ENf, 1);
    soc_pci_write(unit, CMIC_CMCx_CCM_DMA_CFG_OFFSET(cmc), reg);

    rv = SOC_E_TIMEOUT;
    if (soc->ccmDmaIntrEnb) {
            (void)soc_cmicm_cmcx_intr0_enable(unit, cmc, IRQ_CMCx_CCMDMA_DONE);
            if (sal_sem_take(soc->ccmDmaIntr[cmc], soc->ccmDmaTimeout) < 0) {
                rv = SOC_E_TIMEOUT;
            }
           (void)soc_cmicm_cmcx_intr0_disable(unit, cmc, IRQ_CMCx_CCMDMA_DONE);

            reg = soc_pci_read(unit, CMIC_CMCx_CCM_DMA_STAT_OFFSET(cmc));
            if (soc_reg_field_get(unit, CMIC_CMC0_CCM_DMA_STATr,
                                                        reg, DONEf)) {
                rv = SOC_E_NONE;
                if (soc_reg_field_get(unit, CMIC_CMC0_CCM_DMA_STATr,
                                                        reg, ERRORf)) {
                    rv = SOC_E_FAIL;
                }
            }
    } else {
        soc_timeout_t to;
        LOG_WARN(BSL_LS_SOC_SOCMEM,
                 (BSL_META_U(unit,
                             "using Polling mode for CCM DMA\n")));
        soc_timeout_init(&to, soc->ccmDmaTimeout, 10000);
        do {
            reg = soc_pci_read(unit, CMIC_CMCx_CCM_DMA_STAT_OFFSET(cmc));
            if (soc_reg_field_get(unit, CMIC_CMC0_CCM_DMA_STATr,
                                                    reg, DONEf)) {
                rv = SOC_E_NONE;
                if (soc_reg_field_get(unit, CMIC_CMC0_CCM_DMA_STATr,
                                                    reg, ERRORf)) {
                    rv = SOC_E_FAIL;
                }
                break;
            }
        } while(!(soc_timeout_check(&to)));
    }

    if (rv == SOC_E_TIMEOUT) {
        LOG_ERROR(BSL_LS_SOC_SOCMEM,
                  (BSL_META_U(unit,
                              "CcmDmaTimeout: unit %d, ccm_dma timeout\n"), unit));

        /* Abort CCM DMA */

        /* Dummy read to allow abort to finish */

        /* Disable CCM DMA */

        /* Clear ccm DMA abort bit */
    }

    CCM_DMA_UNLOCK(unit, cmc);
    return rv;

}

/*******************************************
* @function _cmicm_ccmdma_abort
* purpose to abort active DMA operation on
*            given channel
* @param vchan [in] channel number
*
* @returns SOC_E_XXXX
* @returns SOC_E_NONE
*
* @end
 */
static int
_cmicm_ccmdma_abort(int unit, int vchan)
{
    int rv = SOC_E_UNAVAIL;


    return rv;
}

/*******************************************
* @function cmicm_ccmdma_clean
* purpose to clean
* @param max_cmc [in] number of cmc
*
* @returns SOC_E_XXXX
* @returns SOC_E_NONE
*
* @end
 */
static int
_cmicm_ccmdma_clean(int unit, int max_cmc)
{
    int rv = 0;
    uint32 cfg_dw;
    int cmc;

    for (cmc = 0; cmc < max_cmc; cmc++) {
        cfg_dw = soc_pci_read(unit, CMIC_CMCx_CCM_DMA_CFG_OFFSET(cmc));
        soc_reg_field_set(unit, CMIC_CMC0_CCM_DMA_CFGr, &cfg_dw, ABORTf, 1);
        soc_pci_write(unit, CMIC_CMCx_CCM_DMA_CFG_OFFSET(cmc), cfg_dw);
    }
    sal_usleep(1000);
    for (cmc = 0; cmc < max_cmc; cmc++) {
        soc_pci_write(unit, CMIC_CMCx_CCM_DMA_CFG_OFFSET(cmc), 0x00000000);
        soc_pci_write(unit, CMIC_CMCx_CCM_DMA_STAT_OFFSET(cmc), 0x00000000);
    }

    return rv;
}

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
int cmicm_ccmdma_init(int unit, soc_ccmdma_drv_t *drv)
{
    int rv = SOC_E_NONE;
    int j;

    for(j = 0; j < CMIC_CMC_NUM_MAX; j++) {
        _cmicm_ccmdma_ch.lock[unit][j] = sal_spinlock_create("ccmdma Lock");
        if (_cmicm_ccmdma_ch.lock[unit][j] == NULL) {
            rv = SOC_E_MEMORY;
            goto cleanup_exit;
        }
        if(j  < SOC_PCI_CMCS_NUM(unit)) {
            _cmicm_ccmdma_ch.ch[unit][j] = 1; /* only one CCMDMA channel/CMC */
        }
    }

    _cmicm_ccmdma_ch.timeout = SAL_BOOT_QUICKTURN ? CCMDMA_TIMEOUT_QT : CCMDMA_TIMEOUT;
    drv->soc_ccmdma_ch_get  = _cmicm_ccmdma_ch_get;
    drv->soc_ccmdma_ch_put = _cmicm_ccmdma_ch_put;
    drv->soc_ccmdma_copy = _cmicm_ccmdma_copy;
    drv->soc_ccmdma_abort = _cmicm_ccmdma_abort;
    drv->soc_ccmdma_clean = _cmicm_ccmdma_clean;
    return SOC_E_NONE;

cleanup_exit:
    for(j = 0; j < CMIC_CMC_NUM_MAX; j++) {
        if(_cmicm_ccmdma_ch.lock[unit][j] != NULL)  {
            sal_spinlock_destroy(_cmicm_ccmdma_ch.lock[unit][j]);
        }
    }
    return(rv);
}

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
int cmicm_ccmdma_deinit(int unit, soc_ccmdma_drv_t *drv)
{
    int j;

    drv->soc_ccmdma_ch_get  = NULL;
    drv->soc_ccmdma_ch_put = NULL;
    drv->soc_ccmdma_copy = NULL;
    drv->soc_ccmdma_abort = NULL;

    for(j = 0; j < CMIC_CMC_NUM_MAX; j++) {
        if(_cmicm_ccmdma_ch.lock[unit][j] != NULL)  {
            (void)sal_spinlock_destroy(_cmicm_ccmdma_ch.lock[unit][j]);
        }
    }
    return SOC_E_NONE;
}

#endif /* BCM_CCMDMA_SUPPORT */
#endif /* defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) */
