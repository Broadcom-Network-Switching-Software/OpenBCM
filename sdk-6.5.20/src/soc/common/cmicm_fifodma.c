/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: CMICM FIFO DMA Driver
 */

#include <shared/bsl.h>
#include <shared/alloc.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/fifodma_internal.h>

#if defined(BCM_CMICM_SUPPORT)
#include <soc/cmicm.h>
#endif

#if defined(BCM_ESW_SUPPORT) || \
    defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT) || defined(BCM_SBUSDMA_SUPPORT)
#if defined(BCM_CMICM_SUPPORT)


/**
 * Function used to (partially) configure the DMA to
 * write to a redirect writes from a register/memory
 * to a host memory.
 *
 *
 * @param unit
 * @param ch - Channel used. should be between 0 and 3.
 * @param is_mem
 * @param mem -name of memory, if memory is used (0 otherwise).
 * @param reg - name of register, if register used (0 otherwise).
 * @param copyno
 * @param force_entry_size - in a case that entry size does not match
 *        the register\memory size  - ignore when equals to 0.
 * @param host_entries  - number of host entries used by the DMA.
 *  Must be a power of 2 between 64 and 16384.
 *  Note that only after host_entries writes does the DMA wrap around the host_buff.
 * @param host_buff - local memory on which the DMA writes. should be big enough to
 *                     allow host_entries writes,
 * in other words whould be the size of host_entries * ( size of memory or
 *                         register used, in bytes).
 *
 * Other register the caller may need to configure seperatly:
 *  CMIC_CMCx_FIFO_CHy_RD_DMA_HOSTMEM_THRESHOLD - The amount of writes by the DMA until a
 *  threshold based interrupt occurs.
 *
 *  TIMEOUT_COUNT field in CMIC_CMCx_FIFO_CHy_RD_DMA - Time between a DMA write and a
 *                          timeout based interrupt.
 *  (May be set to 0 to disable timeout based interrupts).
 *
 *  Endianess field in CMIC_CMCx_FIFO_CHy_RD_DMA.
 *
 * @return int - success or failure.
 */
static int
soc_fifo_dma_start_memreg(int unit, int ch,
                                    int is_mem, soc_mem_t mem, soc_reg_t reg,
                                    int copyno, int force_entry_size,
                                    int host_entries, void *host_buff)
{
    uint8 at;
    uint32 rval, data_beats, sel;
    int cmc;
    int blk;
    schan_msg_t msg;
    int acc_type;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicm fifodma start for chan %d, host entries %d\n"),
                                  ch, host_entries));

    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
        if (ch < 0 || ch > 12 || host_buff == NULL) {
            return SOC_E_PARAM;
        }
        cmc = ch / 4;
        ch = ch % 4;
    } else {
        cmc = SOC_PCI_CMC(unit);
        if (ch < 0 || ch > 3 || host_buff == NULL) {
            return SOC_E_PARAM;
        }
    }

    switch (host_entries) {
    case 64:    sel = 0; break;
    case 128:   sel = 1; break;
    case 256:   sel = 2; break;
    case 512:   sel = 3; break;
    case 1024:  sel = 4; break;
    case 2048:  sel = 5; break;
    case 4096:  sel = 6; break;
    case 8192:  sel = 7; break;
    case 16384: sel = 8; break;
    default:
        return SOC_E_PARAM;
    }

#ifdef BCM_IPFIX_SUPPORT
#ifdef BCM_PETRA_SUPPORT
    if (!SOC_IS_DPP(unit) && !SOC_IS_DFE(unit))
#endif
    {
        if (mem != ING_IPFIX_EXPORT_FIFOm && mem != EGR_IPFIX_EXPORT_FIFOm &&
            mem !=  L2_MOD_FIFOm && mem != FT_EXPORT_FIFOm &&
            mem != FT_EXPORT_DATA_ONLYm &&
            mem != EGR_SER_FIFOm && mem != ING_SER_FIFOm &&
            mem != CENTRAL_CTR_EVICTION_FIFOm) {
            return SOC_E_BADID;
        }
    }
#endif

    /* Differentiate between reg and mem to get address, size and block */
    if ((!is_mem) && SOC_REG_IS_VALID(unit, reg)) {
        data_beats = BYTES2WORDS(soc_reg_bytes(unit, reg));
        rval = soc_reg_addr_get(unit, reg, REG_PORT_ANY, 0,
                                SOC_REG_ADDR_OPTION_NONE, &blk, &at);
    } else {
        data_beats = soc_mem_entry_words(unit, mem);
        if (copyno == MEM_BLOCK_ANY) {
            copyno = SOC_MEM_BLOCK_ANY(unit, mem);
        }
        rval = soc_mem_addr_get(unit, mem, 0, copyno, 0, &at);
        blk = SOC_BLOCK2SCH(unit, copyno);
    }
    /*Force entry size*/
    if (force_entry_size > 0)
    {
        data_beats = BYTES2WORDS(force_entry_size);
    }

    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_SBUS_START_ADDRESS_OFFSET(cmc, ch), rval);

    schan_msg_clear(&msg);
    if (is_mem) {
        acc_type = SOC_MEM_ACC_TYPE(unit, mem);
    } else {
        acc_type = 0;
    }
    soc_schan_header_cmd_set(unit, &msg.header, FIFO_POP_CMD_MSG, blk, 0,
                             acc_type, 4, 0, 0);
    /* Set 1st schan ctrl word as opcode */
    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_OPCODE_OFFSET(cmc, ch), msg.dwords[0]);

    rval = soc_cm_l2p(unit, host_buff);
    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_HOSTMEM_START_ADDRESS_OFFSET(cmc, ch),
                  rval);

    rval = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch));
    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, &rval, BEAT_COUNTf,
                      data_beats);
    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, &rval,
                      HOST_NUM_ENTRIES_SELf, sel);
    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, &rval, ABORTf, 0);

        /* Note: Following might need to be tuned */
        soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, &rval,
                      TIMEOUT_COUNTf, 1000);
        /* Note: Following could be removed ?? */
        soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, &rval,
                      NACK_FATALf, 1);

    if (soc_feature(unit, soc_feature_multi_sbus_cmds)) {
        
    }
    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch), rval);

    if (SOC_IS_ARAD(unit) || SOC_IS_DFE(unit)) {
        /* Always 1 in Arad when interrupt mechanism is not enabled */
        soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_HOSTMEM_THRESHOLD_OFFSET(cmc, ch),
                     1);
    } else {
        /* Note: Following might need to be tuned */
        soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_HOSTMEM_THRESHOLD_OFFSET(cmc, ch),
                      host_entries/10);
    }
    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, &rval, ENABLEf, 1);
    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch), rval);
    return SOC_E_NONE;
}

/*******************************************
* @function _cmicm_fifodma_start
* purpose to start fifo dma channel
*
* @param unit [in] unit #
* @param ch [in] channel number
*
* @returns SOC_E_PARAM
* @returns SOC_E_NONE
*
* @end
 */
static int
_cmicm_fifodma_start(int unit, int ch, int is_mem,
                  soc_mem_t mem, soc_reg_t reg,
                  int copyno, int force_entry_size,
                  int host_entries, void *host_buff)
{
    return soc_fifo_dma_start_memreg(unit, ch,
                                     is_mem, mem, reg,
                                     copyno, force_entry_size, host_entries, host_buff);
}

/*******************************************
* @function _cmicm_fifodma_stop
* purpose to stop fifo dma channel
*
* @param unit [in] unit #
* @param chan [in] channel number
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
static int
_cmicm_fifodma_stop(int unit, int ch)
{
    int cmc, iter = 0;
    uint32 rval;
    int to = SAL_BOOT_QUICKTURN ? 30000000 : 10000000;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicm fifodma stop for chan %d\n"), ch));

    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
        if (ch < 0 || ch > 12) {
            return SOC_E_PARAM;
        }
        cmc = ch / 4;
        ch = ch % 4;
    } else {
        cmc = SOC_PCI_CMC(unit);
        if (ch < 0 || ch > 3) {
            return SOC_E_PARAM;
        }
    }

    rval = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch));
    if (!soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, rval, ENABLEf)) {
        return SOC_E_NONE;
    }
    rval = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch));
    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, &rval, ABORTf, 1);
    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch), rval);

    sal_udelay(1000);
    rval = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_OFFSET(cmc, ch));

    while (soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STATr, rval, DONEf) == 0 &&
           iter++ <to) {
        sal_udelay(1000);
        rval = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_OFFSET(cmc, ch));
    }
    rval = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch));
    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, &rval, ENABLEf, 0);
    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch), rval);

    if (iter >= to) {
        LOG_ERROR(BSL_LS_SOC_FIFODMA,
                  (BSL_META_U(unit,
                              "FIFO DMA abort failed !!\n")));
        return SOC_E_INTERNAL;
    }
    return SOC_E_NONE;
}


/*******************************************
* @function _cmicm_fifodma_intr_enable
* purpose to enable interrupt
*
* @param unit [in] unit #
* @param ch [in] channel
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
static int
_cmicm_fifodma_intr_enable(int unit, int ch)
{
    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicm fifodma interrupt enable for chan %d\n"), ch));

    soc_cmicm_intr0_enable(unit, IRQ_CMCx_FIFO_CH_DMA(ch));

    return 0;
}

/*******************************************
* @function _cmicm_fifodma_intr_disable
* purpose to disable interrupt
*
* @param unit [in] unit #
* @param ch [in] channel
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
static int
_cmicm_fifodma_intr_disable(int unit, int ch)
{
    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicm fifodma interrupt disable for chan %d\n"), ch));

    soc_cmicm_intr0_disable(unit, IRQ_CMCx_FIFO_CH_DMA(ch));

    return 0;
}

/*******************************************
* @function _cmicm_fifodma_get_read_ptr
* purpose to get read ptr
*
* @param unit [in] unit #
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
static int
_cmicm_fifodma_get_read_ptr(int unit, int chan, void **host_ptr, int *count)
{
    return 0;
}

/*******************************************
* @function _cmicm_fifodma_advance_read_ptr
* purpose to advance read ptr
*
* @param unit [in] unit #
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
static int
_cmicm_fifodma_advance_read_ptr(int unit, int chan, int count)
{
    return 0;
}

/*******************************************
* @function _cmicm_fifodma_set_entries_read
* purpose to read set entries
*
* @param unit [in] unit #
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
static int
_cmicm_fifodma_set_entries_read(int unit, int ch, uint32 num)
{
    int cmc;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicm fifodma set %d entries for chan %d\n"), num, ch));

    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
        if (ch < 0 || ch > 12) {
            return SOC_E_PARAM;
        }
        cmc = ch / 4;
        ch = ch % 4;
    } else {
        cmc = SOC_PCI_CMC(unit);
        if (ch < 0 || ch > 3) {
            return SOC_E_PARAM;
        }
    }

    

    soc_pci_write(unit,
        CMIC_CMCx_FIFO_CHy_RD_DMA_NUM_OF_ENTRIES_READ_FRM_HOSTMEM_OFFSET(cmc, ch), num);

    return SOC_E_NONE;
}

/*******************************************
* @function _cmicm_fifodma_num_entries_get
* purpose to get number of entries
*
* @param unit [in] unit #
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
static int
_cmicm_fifodma_num_entries_get(int unit, int ch, int *count)
{
    uint32 val = 0;
    int cmc;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicm fifodma entries get for chan %d\n"), ch));

    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
        if (ch < 0 || ch > 12) {
            return SOC_E_PARAM;
        }
        cmc = ch / 4;
        ch = ch % 4;
    } else {
        cmc = SOC_PCI_CMC(unit);
        if (ch < 0 || ch > 3) {
            return SOC_E_PARAM;
        }
    }

    val = soc_pci_read(unit,
                  CMIC_CMCx_FIFO_CHy_RD_DMA_NUM_OF_ENTRIES_VALID_IN_HOSTMEM_OFFSET(cmc, ch));

    *count = val;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicm fifodma %d entries get for chan %d\n"), val, ch));

    if (val) {
        return SOC_E_NONE;
    }

    return SOC_E_EMPTY;
}

/*******************************************
* @function _cmicm_fifodma_status_clear
* purpose to clear fifo dma channel status
*
* @param unit [in] unit #
* @param chan [in] channel number
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
static int
_cmicm_fifodma_status_clear(int unit, int chan)
{
    soc_reg_t stat_clr_reg;
    uint32 stat_clr_addr;
    uint32 stat_clr_mask;
    int cmc;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicm fifodma status clear for chan %d\n"), chan));

    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
        if (chan < 0 || chan > 12) {
            return SOC_E_PARAM;
        }
        cmc = chan / 4;
        chan = chan % 4;
    } else {
        cmc = SOC_PCI_CMC(unit);
        if (chan < 0 || chan > 3) {
            return SOC_E_PARAM;
        }
    }

    stat_clr_reg = CMIC_CMC0_FIFO_CH1_RD_DMA_STAT_CLRr;
    stat_clr_mask = 0;
    soc_reg_field_set(unit, stat_clr_reg, &stat_clr_mask, HOSTMEM_TIMEOUTf, 1);
    soc_reg_field_set(unit, stat_clr_reg, &stat_clr_mask, HOSTMEM_OVERFLOWf, 1);

    stat_clr_addr = CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_CLR_OFFSET(cmc, chan);

    soc_pci_write(unit, stat_clr_addr, stat_clr_mask);

    return SOC_E_NONE;
}

/*******************************************
* @function _cmicm_fifodma_status_get
* purpose to obtain fifo dma channel status
*
* @param unit [in] unit #
* @param chan [in] channel number
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
static int
_cmicm_fifodma_status_get(int unit, int chan, uint32 *status)
{
    uint32 stat_addr;
    int cmc;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicm fifodma status get for chan %d\n"), chan));

    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
        if (chan < 0 || chan > 12) {
            return SOC_E_PARAM;
        }
        cmc = chan / 4;
        chan = chan % 4;
    } else {
        cmc = SOC_PCI_CMC(unit);
        if (chan < 0 || chan > 3) {
            return SOC_E_PARAM;
        }
    }

    stat_addr = CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_OFFSET(cmc, chan);

    *status = soc_pci_read(unit, stat_addr);

    return SOC_E_NONE;
}

/*******************************************
* @function _cmicm_fifodma_masks_get
* purpose to obtain fifodma masks
*
* @param unit [in] unit #
* @param chan [in] channel number
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
static int
_cmicm_fifodma_masks_get(int unit, uint32 *hostmem_timeout, uint32 *hostmem_overflow, uint32 *error, uint32 *done)
{
    int rv   = SOC_E_NONE;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicm fifodma masks get\n")));

    *hostmem_timeout  = 1 << 3;
    *hostmem_overflow = 1 << 2;
    *error            = 1 << 0;
    *done             = 1 << 4;

    return rv;
}

/*******************************************
* @function _cmicm_l2mod_sbus_fifo_field_set
* obtain l2 mod sbus field set
*
* @param unit   [in] unit #
* @param field  [in] field
* @param enable [in] enable
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
static int
_cmicm_l2mod_sbus_fifo_field_set(int unit, soc_field_t field, int enable)
{
    uint32 rval = 0;
    uint32 fval = enable?1:0;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicm l2mod sbus fifo set\n")));

    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT(unit) ||
        SOC_IS_TITAN2(unit) || SOC_IS_TITAN(unit)) {
        if(soc_reg_field_valid(unit, AUX_ARB_CONTROLr, field)) {
            SOC_IF_ERROR_RETURN(READ_AUX_ARB_CONTROLr(unit, &rval));

            soc_reg_field_set(unit, AUX_ARB_CONTROLr, &rval, field, fval);

            SOC_IF_ERROR_RETURN(WRITE_AUX_ARB_CONTROLr(unit, rval));
        }
        return SOC_E_NONE;
    }
    return SOC_E_UNAVAIL;
}

/*******************************************
* @function _cmicm_l2mod_sbus_fifo_field_get
* obtain l2 mod sbus field get
*
* @param unit   [in] unit #
* @param field  [in] field
* @param enable [in] enable
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
static int
_cmicm_l2mod_sbus_fifo_field_get(int unit, soc_field_t field, int *enable)
{
    uint32 reg = 0;
    uint32 fval = 0;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicm l2mod sbus fifo get\n")));

    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT(unit) ||
        SOC_IS_TITAN2(unit) || SOC_IS_TITAN(unit)) {
        if(soc_reg_field_valid(unit, AUX_ARB_CONTROLr, field)) {
            SOC_IF_ERROR_RETURN(READ_AUX_ARB_CONTROLr(unit, &reg));

            fval = soc_reg_field_get(unit, AUX_ARB_CONTROLr, reg, field);
        }
        *enable = fval?1:0;
        return SOC_E_NONE;
    }

    return SOC_E_UNAVAIL;
}

/*******************************************
* @function cmicm_fifodma_init
* purpose API to Initialize cmicm FIFO DMA
*
* @param unit [in] unit
* @param drv [out] soc_fifodma_drv_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int cmicm_fifodma_init(int unit, soc_fifodma_drv_t *drv)
{
    int rv = SOC_E_NONE;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicm fifo dma init\n")));

    drv->fifodma_start             = _cmicm_fifodma_start;
    drv->fifodma_stop              = _cmicm_fifodma_stop;
    drv->fifodma_intr_enable       = _cmicm_fifodma_intr_enable;
    drv->fifodma_intr_disable      = _cmicm_fifodma_intr_disable;
    drv->fifodma_get_read_ptr      = _cmicm_fifodma_get_read_ptr;
    drv->fifodma_advance_read_ptr  = _cmicm_fifodma_advance_read_ptr;
    drv->fifodma_set_entries_read  = _cmicm_fifodma_set_entries_read;
    drv->fifodma_num_entries_get   = _cmicm_fifodma_num_entries_get;
    drv->fifodma_status_clear      = _cmicm_fifodma_status_clear;
    drv->fifodma_status_get        = _cmicm_fifodma_status_get;
    drv->fifodma_masks_get         = _cmicm_fifodma_masks_get;
    drv->l2mod_sbus_fifo_field_set = _cmicm_l2mod_sbus_fifo_field_set;
    drv->l2mod_sbus_fifo_field_get = _cmicm_l2mod_sbus_fifo_field_get;

    return(rv);
}

/*******************************************
* @function cmicm_fifodma_deinit
* purpose API to Deinitialize cmicm FIFO DMA
*
* @param unit [in] unit
* @param drv [in] soc_fifodma_drv_t pointer
*
* @returns SOC_E_NONE

*
* @end
 */
int cmicm_fifodma_deinit(int unit, soc_fifodma_drv_t *drv)
{
    drv->fifodma_start             = NULL;
    drv->fifodma_stop              = NULL;
    drv->fifodma_intr_enable       = NULL;
    drv->fifodma_intr_disable      = NULL;
    drv->fifodma_get_read_ptr      = NULL;
    drv->fifodma_advance_read_ptr  = NULL;
    drv->fifodma_set_entries_read  = NULL;
    drv->fifodma_num_entries_get   = NULL;
    drv->fifodma_status_clear      = NULL;
    drv->fifodma_status_get        = NULL;
    drv->fifodma_masks_get         = NULL;
    drv->l2mod_sbus_fifo_field_set = NULL;
    drv->l2mod_sbus_fifo_field_get = NULL;

    return SOC_E_NONE;
}

#endif /* BCM_FIFODMA_SUPPORT */
#endif /* BCM_ESW_SUPPORT */

