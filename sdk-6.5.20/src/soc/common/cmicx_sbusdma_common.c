/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: CMICX SBUS-DMA Common utility functions
 */

#include <shared/bsl.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <sal/core/sync.h>
#include <soc/util.h>
#include <soc/drv.h>
#include <soc/sbusdma_internal.h>

#if defined(BCM_CMICX_SUPPORT) && defined(BCM_SBUSDMA_SUPPORT)
#include <soc/cmicx.h>
#include <soc/intr_cmicx.h>


typedef struct intr_data_s {
    int cmc;
    int ch;
}intr_data_t;


typedef struct cmicx_sbusdma_ch_s {
    sal_spinlock_t lock[SOC_MAX_NUM_DEVICES];
    int timeout;
    sal_sem_t sem[SOC_MAX_NUM_DEVICES];
    uint8  ch[SOC_MAX_NUM_DEVICES][CMIC_CMC_NUM_MAX];
    uint8  wait[SOC_MAX_NUM_DEVICES][CMIC_CMC_NUM_MAX];
}cmicx_sbusdma_ch_t;


STATIC intr_data_t _intr_data[CMIC_CMC_NUM_MAX][CMIC_CMCx_SBUSDMA_CHAN_MAX];

STATIC cmicx_sbusdma_ch_t  _cmicx_sbusdma_ch;


/*******************************************
* @function cmicx_sbusdma_curr_op_details
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
cmicx_sbusdma_curr_op_details(int unit, int cmc, int ch)
{
    uint32 rval;

    /* Set up the trigger incase PCI trace need to be taken */
    soc_pci_analyzer_trigger(unit);

    rval = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc, ch));
    LOG_WARN(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "STATUS: 0x%08x. cmc = %d, ch = %d\n"), rval, cmc, ch));
    rval = soc_pci_read(unit,
                        CMIC_CMCx_SBUSDMA_CHy_CUR_SBUSDMA_CONFIG_OPCODE(cmc, ch));
    LOG_WARN(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "OPCODE: 0x%08x\n"), rval));
    rval = soc_pci_read(unit,
                        CMIC_CMCx_SBUSDMA_CHy_CUR_SBUSDMA_CONFIG_ADDR_LO(cmc, ch));
    LOG_WARN(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "HOSTMEM ADDR LO: 0x%08x\n"), rval));
    rval = soc_pci_read(unit,
                        CMIC_CMCx_SBUSDMA_CHy_CUR_SBUSDMA_CONFIG_ADDR_HI(cmc, ch));
    LOG_WARN(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "HOSTMEM ADDR HI: 0x%08x\n"), rval));
    rval = soc_pci_read(unit,
                   CMIC_CMCx_SBUSDMA_CHy_CUR_SBUSDMA_CONFIG_SBUS_START_ADDRESS(cmc, ch));
    LOG_WARN(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "START ADDR: 0x%08x\n"), rval));
    rval = soc_pci_read(unit,
                       CMIC_CMCx_SBUSDMA_CHy_CUR_SBUSDMA_CONFIG_COUNT(cmc, ch));
    LOG_WARN(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "COUNT: 0x%08x\n"), rval));
    rval = soc_pci_read(unit,
                       CMIC_CMCx_SBUSDMA_CHy_CUR_SBUSDMA_CONFIG_REQUEST(cmc, ch));
    LOG_WARN(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "REQUEST: 0x%08x\n"), rval));

    rval = soc_pci_read(unit,
                       CMIC_CMCx_SBUSDMA_CHy_CUR_SBUSDMA_ITER_COUNT(cmc, ch));
    LOG_WARN(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "ITER COUNT: 0x%08x\n"), rval));

    rval = soc_pci_read(unit,
              CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch));

    LOG_WARN(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "CONTROL: 0x%08x\n"), rval));

   rval = soc_pci_read(unit,
              CMIC_CMCx_SBUSDMA_CHy_CUR_HOSTMEM_ADDRESS_LO(cmc, ch));

    LOG_WARN(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "CUR_HOSTMEM_ADDRESS_LO: 0x%08x\n"), rval));

   rval = soc_pci_read(unit,
              CMIC_CMCx_SBUSDMA_CHy_CUR_HOSTMEM_ADDRESS_HI(cmc, ch));

    LOG_WARN(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "CUR_HOSTMEM_ADDRESS_HI: 0x%08x\n"), rval));

   rval = soc_pci_read(unit,
              CMIC_CMCx_SBUSDMA_CHy_CUR_DESC_ADDRESS_LO(cmc, ch));

    LOG_WARN(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "CUR_DESC_ADDRESS_LO: 0x%08x\n"), rval));

   rval = soc_pci_read(unit,
              CMIC_CMCx_SBUSDMA_CHy_CUR_DESC_ADDRESS_HI(cmc, ch));

    LOG_WARN(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "CUR_DESC_ADDRESS_HI: 0x%08x\n"), rval));

#ifdef BCM_ESW_SUPPORT
    if (SOC_IS_TRIDENT3X(unit) || SOC_IS_TOMAHAWKX(unit)){
        uint32 val;
        for (val = 0x10800; val <= 0x10840; val += 0x10) {
            soc_iproc_setreg(
                unit, soc_reg_addr(unit, DMU_CRU_IPROC_DEBUG_SELr,
                REG_PORT_ANY, 0), val);
            sal_udelay(1);
            soc_iproc_getreg(
                unit, soc_reg_addr(unit, DMU_CRU_IPROC_DEBUG_STATUSr,
                REG_PORT_ANY, 0), &rval);
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                      "DMU_SEL = 0x%08x, DMU_STAT = 0x%08x\n"), val, rval));
            rval = soc_pci_read(unit,
                       CMIC_CMCx_SHARED_AXI_STAT_OFFSET(cmc));
            LOG_WARN(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "CMIC_CMCx_SHARED_AXI_STAT: 0x%08x\n"), rval));
        }
    }
#endif

}

/*******************************************
* @function cmicx_sbusdma_error_details
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
cmicx_sbusdma_error_details(int unit, uint32 rval)
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
* @function _cmicx_sbusdma_ch_get
* purpose get idle channel and cmc
*
* @param unit [in] unit #
* @param cmc [out] cmc number 0-1
* @param ch [out] channel number 0-3
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
STATIC int
_cmicx_sbusdma_ch_get(int unit, int *cmc, int *ch)
{
    int rv = SOC_E_BUSY;
    int i;
    uint8 mask = 0;
    uint32 sbusdma_chan_max  = CMIC_CMCx_SBUSDMA_CHAN_MAX;

#ifdef BCM_TRIDENT3_SUPPORT
    uint32 ifa_cc_num_sbusdma = soc_property_get(unit,
                                                spn_IFA_CC_NUM_SBUS_DMA, 2);
    if ((SOC_IS_TRIDENT3(unit)) && (ifa_cc_num_sbusdma > 2)) {
        /* CMC_0 2 SBUS DMA Channels are used by IFA CC Eapp on M0 */
        sbusdma_chan_max = (CMIC_CMCx_SBUSDMA_CHAN_MAX - 2);
    }
#endif

    /*
     By Default all resources in CMC1 are reserved for embedded apps, i.e R5 apps,
     only CMC0 is avaiable for use.
     Return error in case channel allocation failure
     */
    if (sal_sem_take(_cmicx_sbusdma_ch.sem[unit],
        _cmicx_sbusdma_ch.timeout) < 0) {
        return rv;
    }

    sal_spinlock_lock(_cmicx_sbusdma_ch.lock[unit]);
    /* Allocate CMC*/
    if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) {
        *cmc = CMIC_CMCx_PCI;
        mask = CMIC_CMC0_SBUSDMA_CH_MASK;
        if (_cmicx_sbusdma_ch.ch[unit][*cmc] == 0) {
            /* Check if CMIC_CMCx_AXI is configured */
            if (SOC_PCI_CMCS_NUM(unit) > 1) {
                *cmc = CMIC_CMCx_AXI;
                mask = CMIC_CMC1_SBUSDMA_CH_MASK;
            }
        }
    } else {
        /* Check if CMIC_CMCx_AXI is configured */
        if (SOC_PCI_CMCS_NUM(unit) > 1) {
            *cmc = CMIC_CMCx_AXI;
            mask = CMIC_CMC1_SBUSDMA_CH_MASK;
            if (_cmicx_sbusdma_ch.ch[unit][*cmc] == 0) {
                *cmc = CMIC_CMCx_PCI;
                mask = CMIC_CMC0_SBUSDMA_CH_MASK;
            }
        } else {
            *cmc = CMIC_CMCx_PCI;
            mask = CMIC_CMC0_SBUSDMA_CH_MASK;
        }
    }
    /* Allocate Channel */
    for (i = 0; i < sbusdma_chan_max; i++) {
        if ((mask & 0x01 << i) &&
             (_cmicx_sbusdma_ch.ch[unit][*cmc] & 0x01 << i)) {
            rv = SOC_E_NONE;
            *ch = i;
            _cmicx_sbusdma_ch.ch[unit][*cmc] &= ~(0x01 << i);
            break;
        }
    }
    sal_spinlock_unlock(_cmicx_sbusdma_ch.lock[unit]);
    return rv;
}

/*******************************************
* @function cmicx_sbusdma_ch_try_get
* purpose try to get channel on cmc
*
* @param unit [in] unit #
* @param cmc [out] cmc number 0-1
* @param ch [out] channel number 0-3
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
int cmicx_sbusdma_ch_try_get(int unit, int cmc, int ch)
{
    int rv = SOC_E_BUSY;
    soc_timeout_t to;
    soc_timeout_init(&to, _cmicx_sbusdma_ch.timeout, 100);

    /* Try to allocate specified Channel from specified CMC */
    do {
       sal_spinlock_lock(_cmicx_sbusdma_ch.lock[unit]);
       if (_cmicx_sbusdma_ch.ch[unit][cmc] & (0x01 << ch)) {
           rv = SOC_E_NONE;
           _cmicx_sbusdma_ch.ch[unit][cmc] &= ~(0x01 << ch);
           sal_spinlock_unlock(_cmicx_sbusdma_ch.lock[unit]);
           break;
       }
       sal_spinlock_unlock(_cmicx_sbusdma_ch.lock[unit]);
    } while (!soc_timeout_check(&to));
    /* Channel consumed */
    if (rv == SOC_E_NONE) {
        (void)sal_sem_take(_cmicx_sbusdma_ch.sem[unit]
                           ,_cmicx_sbusdma_ch.timeout);
    }

    return rv;
}

/*******************************************
* @function cmicx_sbusdma_ch_get
* purpose get idle channel on cmc
*
* @param unit [in] unit #
* @param cmc [out] cmc number 0-1
* @param ch [out] channel number 0-3
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
int cmicx_sbusdma_ch_get(int unit, int *cmc, int *ch)
{
    int rv;
    uint32 sbusdma_chan_max  = CMIC_CMCx_SBUSDMA_CHAN_MAX;

#ifdef BCM_TRIDENT3_SUPPORT
    uint32 ifa_cc_num_sbusdma = soc_property_get(unit,
                                                spn_IFA_CC_NUM_SBUS_DMA, 2);
    if ((SOC_IS_TRIDENT3(unit)) && (ifa_cc_num_sbusdma > 2)) {
        /* CMC_0 2 SBUS DMA Channels are used by IFA CC Eapp on M0 */
        sbusdma_chan_max = (CMIC_CMCx_SBUSDMA_CHAN_MAX - 2);
    }
#endif

    if ((soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) ||
        (soc_cm_get_bus_type(unit) & SOC_AXI_DEV_TYPE)) {
        rv = _cmicx_sbusdma_ch_get(unit, cmc, ch);
    } else {
        rv = SOC_E_FAIL;
    }

    if (SOC_SUCCESS(rv)) {
        /* Check if interrupt resources are initialized */
        if (SOC_CMC_SBUSDMA_INTR(unit, *cmc, *ch) == NULL) {
            cmicx_sbusdma_ch_put(unit, *cmc, *ch);
            SOC_STAT(unit)->err_sbusdma_intr_res++;
            rv = SOC_E_FAIL;
        }
    } else {
        int i;
        /* Get the details of each channel in case of channel allocation fails */
        for (i = 0; i < sbusdma_chan_max; i++) {
            cmicx_sbusdma_curr_op_details(unit, *cmc, i);
        }
    }
    return rv;
}


/*******************************************
* @function cmicx_sbusdma_ch_put
* purpose put back the freed channel on cmc
*
* @param cmc [in] cmc number 0
* @param ch [in] channel number 0-2
*
* @returns SOC_E_PARAM
* @returns SOC_E_NONE
*
* @end
 */
int cmicx_sbusdma_ch_put(int unit, int cmc, int ch)
{
    int rv = SOC_E_NONE;
    uint32 sbusdma_chan_max  = CMIC_CMCx_SBUSDMA_CHAN_MAX;

#ifdef BCM_TRIDENT3_SUPPORT
    uint32 ifa_cc_num_sbusdma = soc_property_get(unit,
                                                spn_IFA_CC_NUM_SBUS_DMA, 2);
    if ((SOC_IS_TRIDENT3(unit)) && (ifa_cc_num_sbusdma > 2)) {
        /* CMC_0 2 SBUS DMA Channels are used by IFA CC Eapp on M0 */
        sbusdma_chan_max = (CMIC_CMCx_SBUSDMA_CHAN_MAX - 2);
    }
#endif

    if ((ch < 0) || (ch > sbusdma_chan_max -1)) {
       rv = SOC_E_PARAM;
       return rv;
    }
    sal_spinlock_lock(_cmicx_sbusdma_ch.lock[unit]);
    _cmicx_sbusdma_ch.ch[unit][cmc] |= (0x01 << ch);
    sal_spinlock_unlock(_cmicx_sbusdma_ch.lock[unit]);
    sal_sem_give(_cmicx_sbusdma_ch.sem[unit]);
    return rv;
}

/*******************************************
* @function cmicx_sbusdma_ch_deinit
* purpose API to de-Initialize sbusdma channel
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int cmicx_sbusdma_ch_deinit(int unit)

{
    int i;

    for (i = 0; i < SOC_MAX_NUM_DEVICES; i++) {
        if (_cmicx_sbusdma_ch.lock[i] != NULL) {
            sal_spinlock_destroy(_cmicx_sbusdma_ch.lock[i]);
            _cmicx_sbusdma_ch.lock[i] = NULL;
        }
    }
    /* Cleanup channel semaphore */
    for (i = 0; i < SOC_MAX_NUM_DEVICES; i++) {
      if (_cmicx_sbusdma_ch.sem[i] != NULL) {
          sal_sem_destroy(_cmicx_sbusdma_ch.sem[i]);
          _cmicx_sbusdma_ch.sem[i] = NULL;
      }
    }

    return SOC_E_NONE;

}

/*******************************************
* @function cmicx_sbusdma_ch_init
* purpose API to Initialize sbusdma channel seletion mechanism
*
* @param unit [in] unit
* @param timeout [in] int
* @param cmic_ch [out] soc_sbusdma_cmic_ch_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int cmicx_sbusdma_ch_init(int unit, int timeout,
                            soc_sbusdma_cmic_ch_t *cmic_ch)

{
    int rv = SOC_E_NONE;
    int i;
    uint32 sbusdma_chan_max  = CMIC_CMCx_SBUSDMA_CHAN_MAX;

#ifdef BCM_TRIDENT3_SUPPORT
    uint32 ifa_cc_num_sbusdma = soc_property_get(unit,
                                                spn_IFA_CC_NUM_SBUS_DMA, 2);
    if ((SOC_IS_TRIDENT3(unit)) && (ifa_cc_num_sbusdma > 2)) {
        /* CMC_0 2 SBUS DMA Channels are used by IFA CC Eapp on M0 */
        sbusdma_chan_max = (CMIC_CMCx_SBUSDMA_CHAN_MAX - 2);
    }
#endif

    for (i = 0; i < SOC_MAX_NUM_DEVICES; i++) {
        _cmicx_sbusdma_ch.lock[i] = sal_spinlock_create("sbusdma Lock");
        if (_cmicx_sbusdma_ch.lock[i] == NULL) {
            return SOC_E_MEMORY;
        }
    }
    /* Initialize channel semaphore */
    for (i = 0; i < SOC_MAX_NUM_DEVICES; i++) {
       _cmicx_sbusdma_ch.sem[i] =
           sal_sem_create("SBUSDMA Channel",
           sal_sem_COUNTING,
           sbusdma_chan_max * SOC_PCI_CMCS_NUM(unit));
       if (_cmicx_sbusdma_ch.sem[i] == NULL) {
           return SOC_E_MEMORY;
       }
    }
    /* Availability of channels */
    _cmicx_sbusdma_ch.ch[unit][0] = CMIC_CMC0_SBUSDMA_CH_MASK;
    _cmicx_sbusdma_ch.ch[unit][1] = CMIC_CMC1_SBUSDMA_CH_MASK;
    _cmicx_sbusdma_ch.timeout = timeout;
    cmic_ch->soc_sbusdma_ch_try_get = cmicx_sbusdma_ch_try_get;
    cmic_ch->soc_sbusdma_ch_put = cmicx_sbusdma_ch_put;
    return rv;

}

/*******************************************
* @function cmicx_sbusdma_ch_endian_set
* set the endianess value
*
* @param cmc [in] cmc number
* @param ch [in] channel number
* @param ch [in] big_endian
*
* @returns SOC_E_PARAM
* @returns SOC_E_NONE
*
* @end
 */
extern int
cmicx_sbusdma_ch_endian_set(int unit, int cmc, int ch, int be)
{
    uint32 val;

    if ( ((be != 0) && (be != 1)) ||
         (ch >= SOC_SBUSDMA_CH_PER_CMC) ) {
        return SOC_E_PARAM;
    }

    val = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch));
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr,
                                 &val, DESCRIPTOR_ENDIANESSf, be);
    soc_pci_write(unit,  CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch), val);

    return SOC_E_NONE;
}

/*******************************************
* @function _cmicx_sbusdma_start
* start the SBUSDMA
*
* @param unit [in] unit
* @param cmc [in] cmc
* @param ch [in] channel
*
* @returns SOC_E_NONE
*
* @end
 */
int
_cmicx_sbusdma_start(int unit, int cmc, int ch)
{
    int rv = SOC_E_NONE;
    uint32        ctrl;
    ctrl = soc_pci_read(unit,
              CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch));
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr,
                      &ctrl, STARTf, 1);
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch),
                  ctrl);

    return rv;
}
/*******************************************
* @function cmicx_sbusdma_stop
* start the SBUSDMA
*
* @param unit [in] unit
* @param cmc [in] cmc
* @param ch [in] channel
*
* @returns SOC_E_NONE
*
* @end
 */
int
cmicx_sbusdma_stop(int unit, int cmc, int ch)
{
    int rv = SOC_E_NONE;
    uint32        ctrl;

    ctrl = soc_pci_read(unit,
                   CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch));
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &ctrl,
                                  STARTf, 0);
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &ctrl,
                                  ABORTf, 0);
    soc_pci_write(unit,
            CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch), ctrl);

    return rv;
}

/*******************************************
* @function cmicx_sbusdma_intr_wait
* purpose API to wait for DMA complete using interrupt mode
*
* @param unit [in] unit
* @param cmc [in] cmc
* @param ch [in] channel
* @param interval [in] timeout
* @param do_not_perform_start_operation [in] if the argument is non-zero, the operation is assumed to be already started.
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int
cmicx_sbusdma_intr_wait(int unit, int cmc, int ch, int timeout, uint32 do_not_perform_start_operation)
{
    int rv = SOC_E_NONE;
    int rval;
    soc_control_t *soc = SOC_CONTROL(unit);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "cmc = %d, ch = %d, timeout = %d\n"),
                                   cmc, ch, timeout));
    soc_cmic_intr_enable(unit, INTR_SBUSDMA(cmc, ch));

    if (do_not_perform_start_operation == 0 ) {
        _cmicx_sbusdma_start(unit, cmc, ch);
    }
    _cmicx_sbusdma_ch.wait[unit][cmc] |= (0x01 << ch);

    if (sal_sem_take(soc->sbusDmaIntrs[cmc][ch], timeout) < 0) {
        rv = SOC_E_TIMEOUT;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, "Interrupt Timeout\n")));
    }
    _cmicx_sbusdma_ch.wait[unit][cmc] &= ~(0x01 << ch);
    soc_cmic_intr_disable(unit, INTR_SBUSDMA(cmc, ch));

    rval = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc, ch));
    /* Check if DMA is DONE */
    if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                          rval, DONEf)) {
        rv = SOC_E_NONE;
        if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                              rval, ERRORf)) {
            rv = SOC_E_FAIL;
        }
    }
    if (rv != SOC_E_NONE) {
        cmicx_sbusdma_curr_op_details(unit, cmc, ch);
        soc_event_generate(unit, SOC_SWITCH_EVENT_IO_ERROR, cmc, ch, 0);
    }

    return rv;
}


/*******************************************
* @function cmicx_sbusdma_poll_wait
* purpose API to wait Descriptor using Polling mode
*
* @param unit [in] unit
* @param cmc [in] cmc
* @param ch [in] channel
* @param interval [in] timeout
* @param do_not_perform_start_operation [in] if the argument is non-zero, the operation is assumed to be already started.
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int
cmicx_sbusdma_poll_wait(int unit, int cmc, int ch, int timeout, uint32 do_not_perform_start_operation)
{
    soc_timeout_t to;
    int rv = SOC_E_TIMEOUT;
    int rval;

    if (do_not_perform_start_operation == 0 ) {
        _cmicx_sbusdma_start(unit, cmc, ch);
    }
    _cmicx_sbusdma_ch.wait[unit][cmc] |= (0x01 << ch);
    soc_timeout_init(&to, timeout, 0);

    do {
        rval = soc_pci_read(unit,
                            CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc, ch));
         LOG_VERBOSE(BSL_LS_SOC_COMMON,
                       (BSL_META_U(unit,
                                   "cmc = %d, ch = %d, rval = %x\n"),
                                    cmc, ch, rval));
        if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                              rval, DONEf)) {
            rv = SOC_E_NONE;
            if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                                  rval, ERRORf)) {
                rv = SOC_E_FAIL;
            }
            break;
        }
    } while(!(soc_timeout_check(&to)));

    _cmicx_sbusdma_ch.wait[unit][cmc] &= ~(0x01 << ch);
    if (rv != SOC_E_NONE) {
        cmicx_sbusdma_curr_op_details(unit, cmc, ch);
        soc_event_generate(unit, SOC_SWITCH_EVENT_IO_ERROR, cmc, ch, 0);
    }

    return rv;

}


/*******************************************
* @function cmicx_sbusdma_desc_done
* Interrupt handler for descriptor done
*
* @param unit [in] unit
* @param data [in] pointer provided during registration
*
*
* @end
 */

STATIC void
cmicx_sbusdma_ch_done(int unit, void *data)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    intr_data_t *in_d = (intr_data_t *)data;

    if ((soc->sbusDmaIntrEnb) || (soc->tslamDmaIntrEnb) ||
               (soc->tableDmaIntrEnb)) {
        if (_cmicx_sbusdma_ch.wait[unit][in_d->cmc] & 0x01 << in_d->ch) {
            sal_sem_give(soc->sbusDmaIntrs[in_d->cmc][in_d->ch]);
        }
    }
}

/*******************************************
* @function cmicx_sbusdma_vchan_to_cmc_ch
* API takes vchan as input and return cmc and channel
*
* @param unit [in] int
* @param vchan [in] virtual channel
* @param cmc [out] cmc associated
* @param channel [out] channel associated
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int
cmicx_sbusdma_vchan_to_cmc_ch(int unit, int vchan,
                               int *cmc, int *ch)
{
    uint32 sbusdma_chan_max  = CMIC_CMCx_SBUSDMA_CHAN_MAX;

#ifdef BCM_TRIDENT3_SUPPORT
    uint32 ifa_cc_num_sbusdma = soc_property_get(unit,
                                                spn_IFA_CC_NUM_SBUS_DMA, 2);
    if ((SOC_IS_TRIDENT3(unit)) && (ifa_cc_num_sbusdma > 2)) {
        /* CMC_0 2 SBUS DMA Channels are used by IFA CC Eapp on M0 */
        sbusdma_chan_max = (CMIC_CMCx_SBUSDMA_CHAN_MAX - 2);
    }
#endif

    if (vchan < 0 ||
         vchan >= SOC_PCI_CMCS_NUM(unit) * sbusdma_chan_max) {
        return SOC_E_PARAM;
    }
    *cmc = vchan / sbusdma_chan_max;
    *ch =  vchan % sbusdma_chan_max;

    return SOC_E_NONE;
}

/*******************************************
* @function cmicx_sbusdma_intr_init
* purpose API to Initialize SBUSDMA interrupts
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
cmicx_sbusdma_intr_init(int unit)
{
    int cmc, ch;
    soc_cmic_intr_handler_t *handle;
    soc_cmic_intr_handler_t *hitr;
    int rv;
    uint32 sbusdma_chan_max  = CMIC_CMCx_SBUSDMA_CHAN_MAX;

#ifdef BCM_TRIDENT3_SUPPORT
    uint32 ifa_cc_num_sbusdma = soc_property_get(unit,
                                                spn_IFA_CC_NUM_SBUS_DMA, 2);
    if ((SOC_IS_TRIDENT3(unit)) && (ifa_cc_num_sbusdma > 2)) {
        /* CMC_0 2 SBUS DMA Channels are used by IFA CC Eapp on M0 */
        sbusdma_chan_max = (CMIC_CMCx_SBUSDMA_CHAN_MAX - 2);
    }
#endif

    /* Register the descriptor interrupts */
    handle = sal_alloc(sizeof(soc_cmic_intr_handler_t)*CMIC_CMC_NUM_MAX*
                       sbusdma_chan_max, "sbusdma_interrupt");
    if (handle == NULL) {
        return SOC_E_MEMORY;
    }
    hitr = handle;
    for (cmc = 0; cmc < CMIC_CMC_NUM_MAX; cmc++) {
         for (ch = 0; ch < sbusdma_chan_max; ch++) {
              _intr_data[cmc][ch].cmc = cmc;
              _intr_data[cmc][ch].ch = ch;
              hitr->num = INTR_SBUSDMA(cmc, ch);
              hitr->intr_fn = cmicx_sbusdma_ch_done;
              hitr->intr_data = &_intr_data[cmc][ch];
              hitr++;
         }
    }
    rv = soc_cmic_intr_register(unit, handle,
                           CMIC_CMC_NUM_MAX*sbusdma_chan_max);
    sal_free(handle);

    return rv;
}


#endif
