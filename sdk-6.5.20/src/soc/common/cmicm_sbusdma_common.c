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
}cmicm_sbusdma_ch_t;


STATIC cmicm_sbusdma_ch_t  _cmicm_sbusdma_ch;


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
