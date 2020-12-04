/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: CMICM SBUS-DMA Common utility functions
 */

#include <shared/bsl.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <sal/core/sync.h>
#include <soc/util.h>
#include <soc/drv.h>
#include <soc/sbusdma_internal.h>

#if defined(BCM_CMICM_SUPPORT) && defined(BCM_SBUSDMA_SUPPORT)
#include <soc/cmicm.h>


typedef struct cmicm_sbusdma_ch_s {
    sal_spinlock_t   lock;
    int timeout;
    uint8  ch[SOC_MAX_NUM_DEVICES][CMIC_CMC_NUM_MAX];
    uint8  pci_err[SOC_MAX_NUM_DEVICES][CMIC_CMC_NUM_MAX];
}cmicm_sbusdma_ch_t;


STATIC cmicm_sbusdma_ch_t  _cmicm_sbusdma_ch = {0};


/*******************************************
* @function cmicm_sbusdma_curr_op_details
* purpose Print operation details
*
* @param unit [in] unit
* @param cmc [in] cmc number
* @param ch [in] channel number
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
void
cmicm_sbusdma_curr_op_details(int unit, int cmc, int ch)
{
    uint32 rval = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc, ch));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "STATUS: 0x%08x\n"), rval));
    rval = soc_pci_read(unit,
                        CMIC_CMCx_SBUSDMA_CHy_CUR_SBUSDMA_CONFIG_OPCODE(cmc, ch));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "OPCODE: 0x%08x\n"), rval));
    rval = soc_pci_read(unit,
                        CMIC_CMCx_SBUSDMA_CHy_CUR_SBUSDMA_CONFIG_ADDR(cmc, ch));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "START ADDR: 0x%08x\n"), rval));
    rval = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_CUR_SBUSADDR(cmc, ch));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "CUR ADDR: 0x%08x\n"), rval));
}

/*******************************************
* @function cmicm_sbusdma_error_details
* purpose Print error details
*
* @param unit [in] unit
* @param rval [in] Error value
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
void
cmicm_sbusdma_error_details(int unit, uint32 rval)
{
    if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                          rval, DESCRD_ERRORf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Error while reading descriptor from host memory.\n")));
    }
    if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                          rval, SBUSACK_TIMEOUTf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "sbus ack not received within configured time.\n")));
    }
    if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                          rval, SBUSACK_ERRORf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "H/W received sbus ack with error bit set.\n")));
    }
    if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                          rval, SBUSACK_NACKf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "H/W received sbus nack with error bit set.\n")));
    }
    if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                          rval, SBUSACK_WRONG_OPCODEf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Received sbus ack has wrong opcode.\n")));
    }
    if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                          rval, SBUSACK_WRONG_BEATCOUNTf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Received sbus ack data size is not the same as in rep_words "
                              "fields.\n")));
    }
    if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                          rval, SER_CHECK_FAILf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Received sbus ack with SER_CHECK_FAIL set.\n")));
    }
    if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                          rval, HOSTMEMRD_ERRORf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Error while copying SBUSDMA data from Host Memory.\n")));
    }
    if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                          rval, HOSTMEMWR_ERRORf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "DMA operation encountered a schan response error "
                              "or host side error.\n")));
    }
}

/*******************************************
* @function _cmicm_sbusdma_ch_get
* purpose get idle channel on cmc
*
* @param unit [in] unit #
* @param cmc [in] cmc number 0-2
* @param mask [in] mask for available channels
* @param ch [out] channel number 0-2
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
STATIC int
_cmicm_sbusdma_ch_get(int unit, int cmc,
                       uint8 mask, int *ch)
{
    int rv = SOC_E_BUSY;
    int i;

    sal_spinlock_lock(_cmicm_sbusdma_ch.lock);
    for (i = 0; i < CMIC_CMCx_SBUSDMA_CHAN_MAX; i++) {
        if ((mask & 0x01 << i) &&
             (_cmicm_sbusdma_ch.ch[unit][cmc] & 0x01 << i)) {
            rv = SOC_E_NONE;
            *ch = i;
            _cmicm_sbusdma_ch.ch[unit][cmc] &= ~(0x01 << i);
            break;
        }
    }
    sal_spinlock_unlock(_cmicm_sbusdma_ch.lock);
    return rv;
}

/*******************************************
* @function cmicm_sbusdma_ch_get
* purpose get idle channel on cmc
*
* @param unit [in] unit #
* @param cmc [out] cmc number 0-2
* @param ch [out] channel number 0-2
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
int cmicm_sbusdma_ch_get(int unit, int *cmc, int *ch)
{
    int rv;

    soc_timeout_t to;

    soc_timeout_init(&to, _cmicm_sbusdma_ch.timeout, 100);

    do {
        *cmc = CMC0;
        rv = _cmicm_sbusdma_ch_get(unit, *cmc,
                  CMIC_CMC0_SBUSDMA_CH_MASK, ch);
        if (rv == SOC_E_NONE)
            break;

        *cmc = CMC1;
        rv = _cmicm_sbusdma_ch_get(unit, *cmc,
                  CMIC_CMC1_SBUSDMA_CH_MASK, ch);
        if (rv == SOC_E_NONE)
            break;

        *cmc = CMC2;
        rv = _cmicm_sbusdma_ch_get(unit, *cmc,
                  CMIC_CMC2_SBUSDMA_CH_MASK, ch);
        if (rv == SOC_E_NONE)
            break;
    } while (!soc_timeout_check(&to));
    return rv;
}

/*******************************************
* @function cmicm_sbusdma_ch_try_get
* purpose get idle channel on cmc
*
* @param unit [in] unit #
* @param cmc [in] cmc number 0-2
* @param ch [in] channel number 0-2
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
int cmicm_sbusdma_ch_try_get(int unit, int cmc, int ch)
{
    int rv = SOC_E_BUSY;
    uint8 mask = 0;

    soc_timeout_t to;

    soc_timeout_init(&to, _cmicm_sbusdma_ch.timeout, 100);

    if ((cmc < 0) || (cmc > CMIC_CMC_NUM_MAX - 1) ||
        (ch < 0) || (ch > CMIC_CMCx_SBUSDMA_CHAN_MAX - 1)) {
        rv = SOC_E_PARAM;
        return rv;
    }

    if (cmc == 0) {
        mask = CMIC_CMC0_SBUSDMA_CH_MASK;
    } else if (cmc == 1) {
        mask = CMIC_CMC1_SBUSDMA_CH_MASK;
    } else if (cmc == 2) {
        mask = CMIC_CMC2_SBUSDMA_CH_MASK;
    }

    sal_spinlock_lock(_cmicm_sbusdma_ch.lock);
    do {
        if ((mask & (0x01 << ch)) &&
            (_cmicm_sbusdma_ch.ch[unit][cmc] & (0x01 << ch))) {
            rv = SOC_E_NONE;
            _cmicm_sbusdma_ch.ch[unit][cmc] &= ~(0x01 << ch);
            break;
        }
    } while (!soc_timeout_check(&to));
    sal_spinlock_unlock(_cmicm_sbusdma_ch.lock);

    return rv;
}

/*******************************************
* @function cmicm_sbusdma_ch_put
* purpose put back the freed channel on cmc
*
* @param cmc [in] cmc number 0-2
* @param ch [in] channel number 0-2
*
* @returns SOC_E_PARAM
* @returns SOC_E_NONE
*
* @end
 */
int cmicm_sbusdma_ch_put(int unit, int cmc, int ch)
{
   int rv = SOC_E_NONE;

   if ((cmc < 0) || (cmc > CMIC_CMC_NUM_MAX - 1) ||
       (ch < 0) || (ch > CMIC_CMCx_SBUSDMA_CHAN_MAX - 1)) {
       rv = SOC_E_PARAM;
       return rv;
   }
   sal_spinlock_lock(_cmicm_sbusdma_ch.lock);
   _cmicm_sbusdma_ch.ch[unit][cmc] |= (0x01 << ch);
   sal_spinlock_unlock(_cmicm_sbusdma_ch.lock);

   return rv;
}

/*******************************************
* @function cmicm_sbusdma_ch_pci_err_get
* purpose  Get PCI error bit,
*
* @param unit [in] unit #
* @param cmc [in] cmc number.
* @param ch [in] channel number.
* @param bit [out] bit.
*
* @returns SOC_E_NONE
* @returns SOC_E_PARAM
*
* @end
 */
extern int
cmicm_sbusdma_ch_pci_err_get(int unit, int cmc, int ch, int *bit)
{
   if ((cmc < 0) || (cmc > CMIC_CMC_NUM_MAX - 1) ||
       (ch < 0) || (ch > CMIC_CMCx_SBUSDMA_CHAN_MAX - 1)) {
       return SOC_E_PARAM;
   }

    sal_spinlock_lock(_cmicm_sbusdma_ch.lock);
    *bit = _cmicm_sbusdma_ch.pci_err[unit][cmc] & (1 << ch);
    *bit = *bit >> ch;
    sal_spinlock_unlock(_cmicm_sbusdma_ch.lock);

    return SOC_E_NONE;
}

/*******************************************
* @function cmicm_sbusdma_ch_pci_err_set
* purpose  Set PCI error bit,
*
* @param unit [in] unit #
* @param cmc [in] cmc number.
* @param ch [in] channel number.
* @param bit [in] bit.
*
* @returns SOC_E_NONE
* @returns SOC_E_PARAM
*
* @end
 */
extern int
cmicm_sbusdma_ch_pci_err_set(int unit, int cmc, int ch, int bit)
{
   if ((cmc < 0) || (cmc > CMIC_CMC_NUM_MAX - 1) ||
       (ch < 0) || (ch > CMIC_CMCx_SBUSDMA_CHAN_MAX - 1)) {
       return SOC_E_PARAM;
   }

    sal_spinlock_lock(_cmicm_sbusdma_ch.lock);
    _cmicm_sbusdma_ch.pci_err[unit][cmc] |= bit << ch;
    sal_spinlock_unlock(_cmicm_sbusdma_ch.lock);

    return SOC_E_NONE;
}

/*******************************************
* @function cmicm_sbusdma_ch_abort
* purpose  try to abort SBUSDMA channel
*
* @param cmc [in] cmc number 0-2
* @param ch [in] channel number 0-2
* @param timeout [in] channel tiemout in usec.
*
* @returns SOC_E_PARAM
* @returns SOC_E_NONE
* @returns SOC_E_TIMEOUT
*
* @end
 */
int cmicm_sbusdma_ch_abort(int unit, int cmc, int ch, int timeout)
{
    int rv = SOC_E_NONE;
    uint32 val;
    soc_timeout_t to;

   if ((cmc < 0) || (cmc > CMIC_CMC_NUM_MAX - 1) ||
       (ch < 0) || (ch > CMIC_CMCx_SBUSDMA_CHAN_MAX - 1)) {
       rv = SOC_E_PARAM;
       return rv;
   }
    /* Abort DMA */
    val = soc_pci_read(unit,
                        CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch));
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr,
                        &val, ABORTf, 1);
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch),
                        val);

    /* Check the done bit to confirm */
    soc_timeout_init(&to, timeout, 0);
    while (1) {
        val = soc_pci_read(unit,
                            CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc, ch));
       if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                            val, DONEf)) {
           break;
       }
       if (soc_timeout_check(&to)) {
           LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                      "_cmicm_sbusdma_ch_abort: Abort Failed\n")));
           rv = SOC_E_TIMEOUT;
           break;
       }
    }

    return rv;
}

/*******************************************
* @function  cmicm_sbusdma_pci_timeout_handle
* Process CMICM SBUSDMA PCI timeout error.
*
* @param unit [in] unit
* @param cmc [out] cmc
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* CMCIM handler will check if the SBUS DMA transaction
* is active in any of the channels  for a given CMC.
* If active, check the following error bits in the status register.
*
* @end
 */
int
cmicm_sbusdma_pci_timeout_handle(int unit, int cmc)
{
    int rv = SOC_E_NONE;
    int ch;
    int pci_timeout =  PCI_COMPLETION_WAIT_TIMEOUT_USEC;
    soc_timeout_t to;
    uint32 val;
    int addr_err = 0;
    uint32 ctrl;

    for (ch = 0; ch < CMIC_CMCx_SBUSDMA_CHAN_MAX; ch++) {
        soc_timeout_init(&to, pci_timeout, 0);
        do {
            val = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc, ch));
            if (!soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                              val, ACTIVEf)) {
                /* SBUSDMA done check next channel */
                continue;
            }
        } while(!(soc_timeout_check(&to)));


        /*
         * check  CMIC_CMC(0..2)_SBUSDMA_CH(0..2)_STATUS.ERROR
         * CMIC_CMC(0..2)_SBUSDMA_CH(0..2)_STATUS.HOSTMEMRD_ERROR.
         * CMIC_CMC(0..2)_SBUSDMA_CH(0..2)_STATUS.DESCRD_ERROR
         */
        ctrl = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch));
        addr_err = soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                          val, ERRORf);
        if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr,
                          ctrl, MODEf)) {
            /* Descriptor DMA */
            addr_err &= (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                          val, HOSTMEMRD_ERRORf) ||
                         soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                          val, DESCRD_ERRORf));
        } else {
            /* Register DMA */
            addr_err &= soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                          val, HOSTMEMRD_ERRORf);
        }

        /*
         * In case DMA address error s set ABORT the ongoing DMA on
         * that particular channel. Disable DMA so that error bits
         * are clear. Retry  the DMA.
         */
        if (addr_err) {
             cmicm_sbusdma_ch_pci_err_set(unit, cmc, ch, 1);
             rv = cmicm_sbusdma_ch_abort(unit, cmc, ch, pci_timeout);
             if (SOC_FAILURE(rv)) {
                 rv = SOC_E_TIMEOUT;
                 break;
             }
        } else {
             rv = SOC_E_TIMEOUT;
             break;
        }
    }

    /* Log register values in case error is not resolved */
    if (SOC_FAILURE(rv)) {
        cmicm_sbusdma_curr_op_details(unit, cmc, ch);
    }

    return rv;
}

/*******************************************
* @function cmicm_sbusdma_ch_init
* purpose API to Initialize sbusdma channel seletion mechanism
*
* @param unit [in] unit
* @param timeout [in] int
* @param cmic_ch [out] pointer type soc_sbusdma_cmic_ch_t
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int cmicm_sbusdma_ch_init(int unit, int timeout,
                           soc_sbusdma_cmic_ch_t *cmic_ch)

{
   int rv = SOC_E_NONE;

   _cmicm_sbusdma_ch.lock = sal_spinlock_create("sbusdma Lock");

   if (_cmicm_sbusdma_ch.lock == NULL) {
        return SOC_E_MEMORY;
   }
   /* Availability of channels */
   _cmicm_sbusdma_ch.ch[unit][0] = CMIC_CMC0_SBUSDMA_CH_MASK;
   _cmicm_sbusdma_ch.ch[unit][1] = CMIC_CMC1_SBUSDMA_CH_MASK;
   _cmicm_sbusdma_ch.ch[unit][2] = CMIC_CMC2_SBUSDMA_CH_MASK;
   _cmicm_sbusdma_ch.timeout = timeout;
   cmic_ch->soc_sbusdma_ch_try_get = cmicm_sbusdma_ch_try_get;
   cmic_ch->soc_sbusdma_ch_put = cmicm_sbusdma_ch_put;

   return rv;
}


#endif
