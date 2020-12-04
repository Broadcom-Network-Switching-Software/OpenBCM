/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: CMICX CCM DMA Driver
 */

#include <shared/bsl.h>
#include <shared/alloc.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/ccmdma_internal.h>

#if defined(BCM_CMICX_SUPPORT) && defined(BCM_CCMDMA_SUPPORT)
#include <soc/cmicx.h>
#endif

#if defined(BCM_ESW_SUPPORT) || \
    defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
#if defined(BCM_CMICX_SUPPORT) && defined(BCM_CCMDMA_SUPPORT)

typedef struct cmicx_ccmdma_ch_s {
    sal_spinlock_t  lock[SOC_MAX_NUM_DEVICES][CMIC_CMC_NUM_MAX];
    int timeout;
    uint32  ch[SOC_MAX_NUM_DEVICES][CMIC_CMC_NUM_MAX];
}cmicx_ccmdma_ch_t;

STATIC cmicx_ccmdma_ch_t  _cmicx_ccmdma_ch;

/*******************************************
* @function _cmicx_ccmdma_ch_get
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
_cmicx_ccmdma_ch_get(int unit, int *vchan)
{
    int rv = SOC_E_BUSY;
    int j, ch;
    soc_timeout_t to;

    soc_timeout_init(&to, _cmicx_ccmdma_ch.timeout, 10);
    do {
        for(j = 0; j < CMIC_CMC_NUM_MAX; j++) {
            sal_spinlock_lock(_cmicx_ccmdma_ch.lock[unit][j]);
            for(ch = 0; ch < CMICX_N_CCMDMA_CHAN; ch++) {
                if(_cmicx_ccmdma_ch.ch[unit][j] & (1 << ch)) {
                    *vchan = (j * CMICX_N_CCMDMA_CHAN) + ch;
                    _cmicx_ccmdma_ch.ch[unit][j] &= ~(1 << ch);
                    rv = SOC_E_NONE;
                    break;
                }
            }
            sal_spinlock_unlock(_cmicx_ccmdma_ch.lock[unit][j]);
        }
        if (rv == SOC_E_NONE) {
            break;
        }
    } while (!soc_timeout_check(&to));
    return rv;
}

/*******************************************
* @function _cmicx_ccmdma_ch_put
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
_cmicx_ccmdma_ch_put(int unit, int vchan)
{
    int cmc, ch;
    cmc = vchan / CMICX_N_CCMDMA_CHAN;
    ch = vchan % CMICX_N_CCMDMA_CHAN;
    sal_spinlock_lock(_cmicx_ccmdma_ch.lock[unit][cmc]);
    _cmicx_ccmdma_ch.ch[unit][cmc] |= 1 << ch;
    sal_spinlock_unlock(_cmicx_ccmdma_ch.lock[unit][cmc]);
    return SOC_E_NONE;
}

/*******************************************
* @function _cmicx_ccmdma_copy_wait
* purpose to initiate DMA from source to
*            destination memory on vchan
* @param vchan [in] channel number
*
* @returns SOC_E_XXXX
* @returns SOC_E_NONE
*
* @end
 */
static int
_cmicx_ccmdma_copy_wait(int unit, int vchan)
{
    int cmc, ch;
    int rv = SOC_E_TIMEOUT;
    soc_control_t *soc = SOC_CONTROL(unit);
    unsigned int reg;

    cmc = vchan / CMICX_N_CCMDMA_CHAN;
    ch = vchan % CMICX_N_CCMDMA_CHAN;

    CCM_DMA_LOCK(unit, cmc);

    if (soc->ccmDmaIntrEnb) {
        if (sal_sem_take(soc->ccmDmaIntr[cmc], soc->ccmDmaTimeout) < 0) {
            rv = SOC_E_TIMEOUT;
        }
        reg = soc_pci_read(unit, CMIC_CMCx_CCMDMA_CHy_STAT_OFFSET(cmc, ch));
        if (soc_reg_field_get(unit, CMIC_CMC0_CCMDMA_CH0_STATr,
                                                    reg, DONEf)) {
            rv = SOC_E_NONE;
            if (soc_reg_field_get(unit, CMIC_CMC0_CCMDMA_CH0_STATr,
                                                    reg, ERRORf)) {
                rv = SOC_E_FAIL;
            }
        }
    } else {
        soc_timeout_t to;
        LOG_DEBUG(BSL_LS_SOC_SOCMEM,
                 (BSL_META_U(unit,
                             "using Polling mode for CCM DMA\n")));
        soc_timeout_init(&to, soc->ccmDmaTimeout, 10000);
        do {
            reg = soc_pci_read(unit, CMIC_CMCx_CCMDMA_CHy_STAT_OFFSET(cmc, ch));
            if (soc_reg_field_get(unit, CMIC_CMC0_CCMDMA_CH0_STATr,
                                                    reg, DONEf)) {
                rv = SOC_E_NONE;
                if (soc_reg_field_get(unit, CMIC_CMC0_CCMDMA_CH0_STATr,
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
* @function _cmicx_ccmdma_endian_config
* purpose Configure endianess ccmdma engine
*
* @param unit [in] unit
* @param val [in] uint32  pointer
*
* @returns None
* @end
 */
STATIC void
_cmicx_ccmdma_endian_config(int unit,
                            unsigned int src_is_internal,
                            unsigned int dst_is_internal,
                            uint32 *val)
{
    int  big_pio, big_packet, big_other;

    soc_endian_get(unit, &big_pio, &big_packet, &big_other);

    if (big_other & !src_is_internal) {
        soc_reg_field_set(unit, CMIC_CMC0_CCMDMA_CH0_CFGr, val, PROCESSOR0_ENDIANESSf, 1);
    } else {
        soc_reg_field_set(unit, CMIC_CMC0_CCMDMA_CH0_CFGr, val, PROCESSOR0_ENDIANESSf, 0);
    }
    if (big_other & !dst_is_internal) {
        soc_reg_field_set(unit, CMIC_CMC0_CCMDMA_CH0_CFGr, val, PROCESSOR1_ENDIANESSf, 1);
    } else {
        soc_reg_field_set(unit, CMIC_CMC0_CCMDMA_CH0_CFGr, val, PROCESSOR1_ENDIANESSf, 0);
    }
}


/*******************************************
* @function _cmicx_ccmdma_copy
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
_cmicx_ccmdma_copy(int unit, sal_paddr_t *srcbuf, sal_paddr_t *dstbuf,
                   unsigned int src_is_internal, unsigned int dst_is_internal,
                   int count, int flags, int vchan)
{
    int i, rv = SOC_E_NONE;
    soc_control_t *soc = SOC_CONTROL(unit);
    unsigned int *srcptr = (unsigned int *)srcbuf;
    unsigned int *dstptr = (unsigned int *)dstbuf;
    unsigned int reg, reg2;
    int cmc, ch;
    unsigned int offset = 0;
    sal_paddr_t addr;

    /* If src/dst is internal, then the corresponding srcbuf/dstbuf 
     * should be a Physical Address. If external, then Logical Address.
     */

    srcptr = (unsigned int *)srcbuf;
    dstptr = (unsigned int *)dstbuf;

    assert (srcptr && dstptr);

    cmc = vchan / CMICX_N_CCMDMA_CHAN;
    ch = vchan % CMICX_N_CCMDMA_CHAN;

    if (soc->ccmDmaMutex[cmc] == 0) {
        /* If DMA not enabled, or short length use PIO to copy */
        for (i = 0; i < count; i ++ ) {
            if (flags & 2) {            /* Read from PCI */
                reg = soc_cm_iproc_read(unit, PTR_TO_INT(srcptr));
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
                soc_cm_iproc_write(unit, PTR_TO_INT(dstptr), reg);
                reg2 = soc_cm_iproc_read(unit, PTR_TO_INT(dstptr));
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

    if (src_is_internal) {
        offset = 0;
        addr = PTR_TO_UINTPTR(srcbuf);
    } else {
        if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) {
            offset = 0x10000000;
        }
        addr = soc_cm_l2p(unit, srcbuf);
    }

    soc_pci_write(unit, CMIC_CMCx_CCMDMA_CHy_HOST0_MEM_START_ADDR_LO_OFFSET(cmc, ch), PTR_TO_INT(addr));
    soc_pci_write(unit, CMIC_CMCx_CCMDMA_CHy_HOST0_MEM_START_ADDR_HI_OFFSET(cmc, ch), offset | PTR_HI_TO_INT(addr));

    if (dst_is_internal) {
        offset = 0;
        addr = PTR_TO_UINTPTR(dstbuf);
    } else {
        if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) {
            offset = 0x10000000;
        }
        addr = soc_cm_l2p(unit, dstbuf);
    }

    soc_pci_write(unit, CMIC_CMCx_CCMDMA_CHy_HOST1_MEM_START_ADDR_LO_OFFSET(cmc, ch), PTR_TO_INT(addr));
    soc_pci_write(unit, CMIC_CMCx_CCMDMA_CHy_HOST1_MEM_START_ADDR_HI_OFFSET(cmc, ch), offset | PTR_HI_TO_INT(addr));

    soc_pci_write(unit, CMIC_CMCx_CCMDMA_CHy_ENTRY_COUNT_OFFSET(cmc, ch), count);

    reg = soc_pci_read(unit, CMIC_CMCx_CCMDMA_CHy_CFG_OFFSET(cmc, ch));
    soc_reg_field_set(unit, CMIC_CMC0_CCMDMA_CH0_CFGr, &reg, ABORTf, 0);
    _cmicx_ccmdma_endian_config(unit, src_is_internal, dst_is_internal, &reg);
    soc_reg_field_set(unit, CMIC_CMC0_CCMDMA_CH0_CFGr, &reg, ENf, 0);
    soc_pci_write(unit, CMIC_CMCx_CCMDMA_CHy_CFG_OFFSET(cmc, ch), reg);  /* Clearing EN clears stats */

    /* Start DMA */
    soc_reg_field_set(unit, CMIC_CMC0_CCMDMA_CH0_CFGr, &reg, ENf, 1);
    soc_pci_write(unit, CMIC_CMCx_CCMDMA_CHy_CFG_OFFSET(cmc, ch), reg);

    CCM_DMA_UNLOCK(unit, cmc);
    /* Don't wait for DMA Complete if flag bit 0x100 is set */
    if (!(flags & 4)) {
        rv = _cmicx_ccmdma_copy_wait(unit, vchan);
    }
    return rv;

}

/*******************************************
* @function _cmicx_ccmdma_abort
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
_cmicx_ccmdma_abort(int unit, int vchan)
{
    int rv = SOC_E_UNAVAIL;

    return rv;
}

/*******************************************
* @function cmicx_ccmdma_clean
* purpose to clean
* @param max_cmc [in] number of cmc
*
* @returns SOC_E_XXXX
* @returns SOC_E_NONE
*
* @end
 */
static int
_cmicx_ccmdma_clean(int unit, int max_cmc)
{
    int rv = 0;
    uint32 cfg_dw;
    int cmc, chan;

    for (cmc = 0; cmc < max_cmc; cmc++) {
        for (chan = 0; chan < CMICX_N_CCMDMA_CHAN; chan++) {
            cfg_dw = soc_pci_read(unit, CMIC_CMCx_CCMDMA_CHy_CFG_OFFSET(cmc, chan));
            soc_reg_field_set(unit, CMIC_CMC0_CCMDMA_CH0_CFGr, &cfg_dw, ABORTf, 0);
            soc_pci_write(unit, CMIC_CMCx_CCMDMA_CHy_CFG_OFFSET(cmc, chan), cfg_dw);
        }
    }

    sal_usleep(1000);

    for (cmc = 0; cmc < max_cmc; cmc++) {
        for (chan = 0; chan < CMICX_N_CCMDMA_CHAN; chan++) {
            soc_pci_write(unit, CMIC_CMCx_CCMDMA_CHy_CFG_OFFSET(cmc, chan), 0x00000000);
        }
    }

    return rv;
}

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
int cmicx_ccmdma_init(int unit, soc_ccmdma_drv_t *drv)
{
    int rv = SOC_E_NONE;
    int j;

    for(j = 0; j < CMIC_CMC_NUM_MAX; j++) {
        _cmicx_ccmdma_ch.lock[unit][j] = sal_spinlock_create("ccmdma Lock");
        if (_cmicx_ccmdma_ch.lock[unit][j] == NULL) {
            rv = SOC_E_MEMORY;
            goto cleanup_exit;
        }
        if(j  < SOC_PCI_CMCS_NUM(unit)) {
            _cmicx_ccmdma_ch.ch[unit][j] = 0x3; /* Two CCMDMA channels/CMC */
        }
    }

    _cmicx_ccmdma_ch.timeout = SAL_BOOT_QUICKTURN ? CCMDMA_TIMEOUT_QT : CCMDMA_TIMEOUT;
    drv->soc_ccmdma_ch_get  = _cmicx_ccmdma_ch_get;
    drv->soc_ccmdma_ch_put = _cmicx_ccmdma_ch_put;
    drv->soc_ccmdma_copy = _cmicx_ccmdma_copy;
    drv->soc_ccmdma_copy_wait = _cmicx_ccmdma_copy_wait;
    drv->soc_ccmdma_abort = _cmicx_ccmdma_abort;
    drv->soc_ccmdma_clean = _cmicx_ccmdma_clean;
    return SOC_E_NONE;

cleanup_exit:
    for(j = 0; j < CMIC_CMC_NUM_MAX; j++) {
        if(_cmicx_ccmdma_ch.lock[unit][j] != NULL)  {
            sal_spinlock_destroy(_cmicx_ccmdma_ch.lock[unit][j]);
        }
    }
    return(rv);
}

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
int cmicx_ccmdma_deinit(int unit, soc_ccmdma_drv_t *drv)
{
    int j;

    drv->soc_ccmdma_ch_get  = NULL;
    drv->soc_ccmdma_ch_put = NULL;
    drv->soc_ccmdma_copy = NULL;
    drv->soc_ccmdma_abort = NULL;

    for(j = 0; j < CMIC_CMC_NUM_MAX; j++) {
        if(_cmicx_ccmdma_ch.lock[unit][j] != NULL)  {
            (void)sal_spinlock_destroy(_cmicx_ccmdma_ch.lock[unit][j]);
        }
    }
    return SOC_E_NONE;
}

#endif /* BCM_CCMDMA_SUPPORT */
#endif /* BCM_ESW_SUPPORT */

