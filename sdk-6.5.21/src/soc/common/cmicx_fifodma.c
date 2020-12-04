/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: CMICX FIFO DMA Driver
 */

#include <shared/bsl.h>
#include <shared/alloc.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/fifodma_internal.h>

#if defined(BCM_CMICX_SUPPORT)
#include <soc/cmicx.h>
#include <soc/intr_cmicx.h>

#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/cmn/dnxf_drv.h>
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#endif

#ifdef BCM_DNX_SUPPORT
#include <sal/core/dpc.h>
#include <soc/dnx/dnx_fifodma.h>
#include <bcm_int/dnx/l2/l2_learn.h>
#endif

typedef struct fifodma_intr_data_s {
    int cmc;
    int ch;
}fifodma_intr_data_t;

static fifodma_intr_data_t _intr_data[CMIC_CMC_NUM_MAX][CMICX_N_FIFODMA_CHAN];

/*
 * Function:
 *      cmicx_fifodma_ch_endian_set
 * Purpose:
 *      To set cmicx fifo dma channel endianness
 *
 * Parameters:
 *      unit    - SOC unit #
 *      channel - channel number
 *      be      - big_endian
 *
 * @returns SOC_E_NONE
 */
int
cmicx_fifodma_ch_endian_set(int unit, int channel, int be) {
    uint32 val = 0;

    val = soc_pci_read(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_OFFSET(channel));
    soc_reg_field_set(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_CFGr, &val,
                      ENDIANESSf, be);
    soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_OFFSET(channel), val);
    return SOC_E_NONE;
}

/*
 * Function:
 *      cmicx_ctr_evict_fifodma_done
 * Purpose:
 *      The interrupt handler for fifodma channel interrupt
 *
 * Parameters:
 *      unit  - SOC unit #
 *      *data - pointer provided during registration
 */

static void
cmicx_ctr_evict_fifodma_done(int unit, void *data)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    fifodma_intr_data_t *in_d = (fifodma_intr_data_t *)data;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "fifo dma done cmc = %d ch = %d\n"),
                                  in_d->cmc, in_d->ch));

    soc_cmic_intr_disable(unit, INTR_FIFO_DMA(in_d->ch));

    soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_STAT_CLR_OFFSET(in_d->ch), 0x3);

    if (soc->ctrEvictDmaIntrEnb) {
        SOC_CONTROL(unit)->stat.intr_fifo_dma[in_d->ch]++;
        sal_sem_give(soc->ctrEvictIntr);
    }
}

/*
 * Function:
 *      cmicx_l2_mod_fifodma_done
 * Purpose:
 *      The interrupt handler for fifodma channel interrupt
 *
 * Parameters:
 *      unit  - SOC unit #
 *      *data - pointer provided during registration
 */

static void
cmicx_l2_mod_fifodma_done(int unit, void *data)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    fifodma_intr_data_t *in_d = (fifodma_intr_data_t *)data;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "fifo dma done cmc = %d ch = %d\n"),
                                  in_d->cmc, in_d->ch));

    soc_cmic_intr_disable(unit, INTR_FIFO_DMA(in_d->ch));

    soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_STAT_CLR_OFFSET(in_d->ch), 0x3);

    if (soc->l2modDmaIntrEnb) {
        SOC_CONTROL(unit)->stat.intr_fifo_dma[in_d->ch]++;
        sal_sem_give(soc->arl_notify);
    }
}

/*
 * Function:
 *      cmicx_bsc_export_fifodma_done
 * Purpose:
 *      The interrupt handler for fifodma channel interrupt
 *
 * Parameters:
 *      unit  - SOC unit #
 *      *data - pointer provided during registration
 */

static void
cmicx_bsc_export_fifodma_done(int unit, void *data)
{
    fifodma_intr_data_t *in_d = (fifodma_intr_data_t *)data;

#ifdef BCM_FLOWTRACKER_EXPORT_FIFO_SUPPORT
    soc_control_t *soc = SOC_CONTROL(unit);
#endif /* BCM_FLOWTRACKER_EXPORT_FIFO_SUPPORT */

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "fifo dma done cmc = %d ch = %d\n"),
                                  in_d->cmc, in_d->ch));

    soc_cmic_intr_disable(unit, INTR_FIFO_DMA(in_d->ch));

    soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_STAT_CLR_OFFSET(in_d->ch), 0x3);

#ifdef BCM_FLOWTRACKER_EXPORT_FIFO_SUPPORT
    if (soc->ftExportIntrEnb) {
        SOC_CONTROL(unit)->stat.intr_fifo_dma[in_d->ch]++;
        sal_sem_give(soc->ftExportIntr);
    }
#endif /* BCM_FLOWTRACKER_EXPORT_FIFO_SUPPORT */
}


#ifdef BCM_DNXF_SUPPORT
/*
 * Function:
 *      cmicx_dnxf_fifodma_done
 * Purpose:
 *      The interrupt handler for fifodma channel interrupt
 *
 * Parameters:
 *      unit  - SOC unit #
 *      *data - pointer provided during registration
 */
static void
cmicx_dnxf_fifodma_done(int unit, void *data)
{
    fifodma_intr_data_t *in_d = (fifodma_intr_data_t *)data;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "fifo dma done cmc = %d ch = %d\n"),
                                  in_d->cmc, in_d->ch));

    soc_cmic_intr_disable(unit, INTR_FIFO_DMA(in_d->ch));

    soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_STAT_CLR_OFFSET(in_d->ch), 0x3);

    if (dnxf_data_fabric.cell.fifo_dma_enable_get(unit) && SOC_DNXF_CONTROL(unit)->rx_thread_fifo_dma_semaphore != NULL) {
        SOC_CONTROL(unit)->stat.intr_fifo_dma[in_d->ch]++;
        sal_sem_give(SOC_DNXF_CONTROL(unit)->rx_thread_fifo_dma_semaphore);
    }
}
#endif


#ifdef BCM_DNX_SUPPORT
/*
 * Function:
 *      cmicx_dnx_fifodma_done
 * Purpose:
 *      The interrupt handler for dnx fifodma channel interrupt
 *
 * Parameters:
 *      unit  - SOC unit #
 *      *data - pointer provided during registration
 */

static void
cmicx_dnx_fifodma_done(int unit, void *data)
{
    int rv = 0;
    fifodma_intr_data_t *in_d = (fifodma_intr_data_t *)data;
    int olp_channel = -1;
    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "fifo dma done cmc = %d ch = %d\n"),
                                  in_d->cmc, in_d->ch));

    soc_cmic_intr_disable(unit, INTR_FIFO_DMA(in_d->ch));
    /** Clear HOSTMEM OVERFLOW and TIMEOUT interrupt */
    soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_STAT_CLR_OFFSET(in_d->ch), 0x3);
    /** Handle under sal_dpc in case this might be a long operation in the interrupt context */
    soc_dnx_fifodma_channel_get(unit, soc_dnx_fifodma_src_olp, &olp_channel);
    if (in_d->ch == olp_channel)
    {
        if (dnx_l2_cpu_learn_thread_info[unit].sem)
        {
            sal_sem_give(dnx_l2_cpu_learn_thread_info[unit].sem);
        }
    }
    else
    {
        rv = sal_dpc(dnx_fifodma_intr_handler, INT_TO_PTR(unit), INT_TO_PTR(in_d->ch), NULL, NULL, NULL);
    }
    if (rv)
    {
        LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                          (BSL_META_U(unit,
                                      "DPC failed to execute fifo dma handler for cmc = %d ch = %d\n"),
                                      in_d->cmc, in_d->ch));

        /*
         * Interrupt is enabled in dnx_fifodma_intr_handler.
         * In the case of DPC failure, interrupt is disabled forerver, so enable it here.
         */
        soc_cmic_intr_enable(unit, INTR_FIFO_DMA(in_d->ch));
    }
}
#endif

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
    int blk;
    schan_msg_t msg;
    int acc_type;
    sal_paddr_t     addr;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicx fifodma start for chan %d, host entries %d\n"),
                                  ch, host_entries));

    if (ch < 0 || ch > 12 || host_buff == NULL) {
        return SOC_E_PARAM;
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

    /* Force entry size */
    if (force_entry_size > 0)
    {
        data_beats = BYTES2WORDS(force_entry_size);
    }

    soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_SBUS_START_ADDRESS_OFFSET(ch), rval);

    schan_msg_clear(&msg);
    if (is_mem) {
        acc_type = SOC_MEM_ACC_TYPE(unit, mem);
    } else {
        acc_type = 0;
    }

    soc_schan_header_cmd_set(unit, &msg.header, FIFO_POP_CMD_MSG, blk, 0,
                             acc_type, 4, 0, 0);
    /* Set 1st schan ctrl word as opcode */
    soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_OPCODE_OFFSET(ch), msg.dwords[0]);

    addr = soc_cm_l2p(unit, host_buff);

    soc_pci_write(unit,
                  CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_HOSTMEM_START_ADDRESS_LO_OFFSET(ch),
                  PTR_TO_INT(addr));
    if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) {
        soc_pci_write(unit,
                  CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_HOSTMEM_START_ADDRESS_HI_OFFSET(ch),
                  (CMIC_PCIE_SO_OFFSET | PTR_HI_TO_INT(addr)));
    } else {
        soc_pci_write(unit,
                  CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_HOSTMEM_START_ADDRESS_HI_OFFSET(ch),
                  (PTR_HI_TO_INT(addr)));
    }

    rval = soc_pci_read(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_OFFSET(ch));

    soc_reg_field_set(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_CFGr, &rval,
                      BEAT_COUNTf, data_beats);
    soc_reg_field_set(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_CFGr, &rval,
                      HOST_NUM_ENTRIES_SELf, sel);
    soc_reg_field_set(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_CFGr, &rval,
                      ABORTf, 0);
    soc_reg_field_set(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_CFGr, &rval,
                      TIMEOUT_COUNTf, 1000);
    soc_reg_field_set(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_CFGr, &rval,
                      NACK_FATALf, 1);

    if (soc_feature(unit, soc_feature_multi_sbus_cmds)) {
        
    }

    soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_OFFSET(ch), rval);

    soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_HOSTMEM_THRESHOLD_OFFSET(ch),
                 host_entries/10);

    soc_reg_field_set(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_CFGr, &rval, ENABLEf, 1);
    soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_OFFSET(ch), rval);

    return SOC_E_NONE;
}

/*******************************************
* @function _cmicx_fifodma_start
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
_cmicx_fifodma_start(int unit, int ch, int is_mem,
                  soc_mem_t mem, soc_reg_t reg,
                  int copyno, int force_entry_size,
                  int host_entries, void *host_buff)
{
    return soc_fifo_dma_start_memreg(unit, ch,
                                     is_mem, mem, reg,
                                     copyno, force_entry_size, host_entries, host_buff);
}

/*******************************************
* @function _cmicx_fifodma_stop
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
_cmicx_fifodma_stop(int unit, int ch)
{
    int iter = 0;
    uint32 rval;
    int to = SAL_BOOT_QUICKTURN ? 30000000 : 10000000;
    uint32 aborted = 0;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicx fifodma stop for chan %d\n"), ch));

    if (ch < 0 || ch > 12) {
        return SOC_E_PARAM;
    }

    rval = soc_pci_read(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_OFFSET(ch));
    if (!soc_reg_field_get(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_CFGr, rval, ENABLEf)) {
        return SOC_E_NONE;
    }

    rval = soc_pci_read(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_OFFSET(ch));
    soc_reg_field_set(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_CFGr, &rval, ABORTf, 1);
    soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_OFFSET(ch), rval);

    sal_udelay(1000);
    rval = soc_pci_read(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_STAT_OFFSET(ch));
    aborted = soc_reg_field_get(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_STATr,
                                rval, ABORTEDf);
    if (SAL_BOOT_BCMSIM) {
        aborted = 1;
    }
    while (aborted == 0 && iter++ < to) {
        sal_udelay(1000);
        rval = soc_pci_read(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_STAT_OFFSET(ch));
        aborted = soc_reg_field_get(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_STATr,
                                    rval, ABORTEDf);
    }

    rval = soc_pci_read(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_OFFSET(ch));
    soc_reg_field_set(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_CFGr, &rval, ENABLEf, 0);
    soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_OFFSET(ch), rval);

    if (iter >= to) {
        LOG_ERROR(BSL_LS_SOC_FIFODMA,
                  (BSL_META_U(unit,
                              "FIFO DMA abort failed !!\n")));
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;

}


/*******************************************
* @function _cmicx_fifodma_intr_enable
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
_cmicx_fifodma_intr_enable(int unit, int ch)
{
    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicx fifodma interrupt enable for chan %d\n"), ch));

    soc_cmic_intr_enable(unit, INTR_FIFO_DMA(ch));

    return 0;
}

/*******************************************
* @function _cmicx_fifodma_intr_disable
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
_cmicx_fifodma_intr_disable(int unit, int ch)
{
    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicx fifodma interrupt disable for chan %d\n"), ch));

    soc_cmic_intr_disable(unit, INTR_FIFO_DMA(ch));

    return 0;
}

/*******************************************
* @function _cmicx_fifodma_get_read_ptr
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
_cmicx_fifodma_get_read_ptr(int unit, int chan, void **host_ptr, int *count)
{
    return 0;
}

/*******************************************
* @function _cmicx_fifodma_advance_read_ptr
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
_cmicx_fifodma_advance_read_ptr(int unit, int chan, int count)
{
    return 0;
}

/*******************************************
* @function _cmicx_fifodma_set_entries_read
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
_cmicx_fifodma_set_entries_read(int unit, int ch, uint32 num)
{
    uint32 rval = 0;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicx fifodma set %d entries for chan %d\n"), num, ch));

    if (ch < 0 || ch > 12) {
        return SOC_E_PARAM;
    }

    soc_reg_field_set(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_NUM_OF_ENTRIES_READ_FRM_HOSTMEMr, &rval,
                      ENTRYCOUNTf, num);
    soc_reg_field_set(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_NUM_OF_ENTRIES_READ_FRM_HOSTMEMr, &rval,
                      WR_NUMBER_OF_ENTRIESf, 1);

    soc_pci_write(unit,
        CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_NUM_OF_ENTRIES_READ_FRM_HOSTMEM_OFFSET(ch), rval);

    return SOC_E_NONE;
}

/*******************************************
* @function _cmicx_fifodma_num_entries_get
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
_cmicx_fifodma_num_entries_get(int unit, int ch, int *count)
{
    uint32 val = 0;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicx fifodma entries get for chan %d\n"), ch));

    if (ch < 0 || ch > 12) {
        return SOC_E_PARAM;
    }

    val = soc_pci_read(unit,
                  CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_NUM_OF_ENTRIES_VALID_IN_HOSTMEM_OFFSET(ch));

    *count = val;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicx fifodma %d entries get for chan %d\n"), val, ch));
    if (val) {
        return SOC_E_NONE;
    }

    return SOC_E_EMPTY;
}

/*******************************************
* @function _cmicx_fifodma_status_clear
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
_cmicx_fifodma_status_clear(int unit, int chan)
{
    soc_reg_t stat_clr_reg;
    uint32 stat_clr_addr;
    uint32 stat_clr_mask;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicx fifodma status clear for chan %d\n"), chan));

    if (chan < 0 || chan > 12) {
        return SOC_E_PARAM;
    }

    stat_clr_reg = CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_STATr;
    stat_clr_mask = 0;
    soc_reg_field_set(unit, stat_clr_reg, &stat_clr_mask, HOSTMEM_TIMEOUTf, 1);
    soc_reg_field_set(unit, stat_clr_reg, &stat_clr_mask, HOSTMEM_OVERFLOWf, 1);

    stat_clr_addr = CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_STAT_CLR_OFFSET(chan);

    soc_pci_write(unit, stat_clr_addr, stat_clr_mask);

    return SOC_E_NONE;
}

/*******************************************
* @function _cmicx_fifodma_status_get
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
_cmicx_fifodma_status_get(int unit, int chan, uint32 *status)
{
    uint32 stat_addr;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicx fifodma status get for chan %d\n"), chan));

    if (chan < 0 || chan > 12) {
        return SOC_E_PARAM;
    }

    stat_addr = CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_STAT_OFFSET(chan);

    *status = soc_pci_read(unit, stat_addr);

    return SOC_E_NONE;
}

/*******************************************
* @function _cmicx_fifodma_masks_get
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
_cmicx_fifodma_masks_get(int unit, uint32 *hostmem_timeout, uint32 *hostmem_overflow, uint32 *error, uint32 *done)
{
    int rv   = SOC_E_NONE;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicx fifodma masks get\n")));

    *hostmem_timeout  = 1 << 3;
    *hostmem_overflow = 1 << 2;
    *error            = 1 << 0;
    *done             = 1 << 4;

    return rv;
}

/*******************************************
* @function _cmicx_l2mod_sbus_fifo_field_set
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
_cmicx_l2mod_sbus_fifo_field_set(int unit, soc_field_t field, int enable)
{
    uint32 rval = 0;
    uint32 fval = enable?1:0;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicx l2mod sbus fifo set\n")));

    if (SOC_IS_TRIDENT3X(unit)) {
        if(soc_reg_field_valid(unit, L2_MOD_FIFO_ENABLEr, field)) {
            SOC_IF_ERROR_RETURN(READ_L2_MOD_FIFO_ENABLEr(unit, &rval));

            soc_reg_field_set(unit, L2_MOD_FIFO_ENABLEr, &rval, field, fval);

            SOC_IF_ERROR_RETURN(WRITE_L2_MOD_FIFO_ENABLEr(unit, rval));
        }
        return SOC_E_NONE;
    }

    return SOC_E_UNAVAIL;
}

/*******************************************
* @function _cmicx_l2mod_sbus_fifo_field_get
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
_cmicx_l2mod_sbus_fifo_field_get(int unit, soc_field_t field, int *enable)
{
    uint32 reg = 0;
    uint32 fval = 0;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicx l2mod sbus fifo get\n")));


    if (SOC_IS_TRIDENT3X(unit)) {
        if(soc_reg_field_valid(unit, L2_MOD_FIFO_ENABLEr, field)) {
            SOC_IF_ERROR_RETURN(READ_L2_MOD_FIFO_ENABLEr(unit, &reg));

            fval = soc_reg_field_get(unit, L2_MOD_FIFO_ENABLEr, reg, field);
        }
        *enable = fval?1:0;
        return SOC_E_NONE;
    }

    return SOC_E_UNAVAIL;
}

/*******************************************
* @function cmicx_fifodma_init
* purpose API to Initialize cmicx FIFO DMA
*
* @param unit [in] unit
* @param drv [out] soc_fifodma_drv_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int cmicx_fifodma_init(int unit, soc_fifodma_drv_t *drv)
{
    int rv = SOC_E_NONE;
    int cmc = 0;
    soc_cmic_intr_handler_t *handle;
    soc_cmic_intr_handler_t *hitr;
    uint32 val, y;
    int nof_channels = CMICX_N_FIFODMA_CHAN;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "cmicx fifo dma init, registering interrupts\n")));

    /* Register the descriptor done interrupts */
    handle = sal_alloc(sizeof(soc_cmic_intr_handler_t)*CMICX_N_FIFODMA_CHAN,
                       "fifodma_interrupt");
    if (handle == NULL) {
        return SOC_E_MEMORY;
    }

    hitr = handle;

#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        int ch = 0;

        /* Number of supported channels */
        nof_channels = dnxf_data_fabric.cell.fifo_dma_nof_channels_get(unit);

        for (ch = 0; ch < nof_channels; ++ch)
        {
            _intr_data[cmc][0].cmc = cmc;
            _intr_data[cmc][0].ch = ch;
            hitr->num = INTR_FIFO_DMA(ch);
            hitr->intr_fn = cmicx_dnxf_fifodma_done;
            hitr->intr_data = &_intr_data[cmc][ch];
            hitr++;
        }
    }
    else
#endif
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        int ch = 0;
        /* Number of supported channels */
        for(ch = 0; ch < CMICX_N_FIFODMA_CHAN; ch++) {
            _intr_data[cmc][ch].cmc = cmc;
            _intr_data[cmc][ch].ch = ch;
            hitr->num = INTR_FIFO_DMA(ch);
            hitr->intr_fn = cmicx_dnx_fifodma_done;
            hitr->intr_data = &_intr_data[cmc][ch];
            hitr++;
        }
    }
    else
#endif

    {
        /* For Channel 0 */
        _intr_data[cmc][0].cmc = cmc;
        _intr_data[cmc][0].ch = 0;
        hitr->num = INTR_FIFO_DMA(0);
        hitr->intr_fn = cmicx_l2_mod_fifodma_done;
        hitr->intr_data = &_intr_data[cmc][0];
        hitr++;
        /* For Channel 1 */
        _intr_data[cmc][1].cmc = cmc;
        _intr_data[cmc][1].ch = 1;
        hitr->num = INTR_FIFO_DMA(1);
        hitr->intr_fn = cmicx_ctr_evict_fifodma_done;
        hitr->intr_data = &_intr_data[cmc][1];
        hitr++;

        /* For Channel 2 */
        _intr_data[cmc][2].cmc = cmc;
        _intr_data[cmc][2].ch = 2;
        hitr->num = INTR_FIFO_DMA(2);
        hitr->intr_fn = cmicx_bsc_export_fifodma_done;
        hitr->intr_data = &_intr_data[cmc][2];
        hitr++;
    }

    rv = soc_cmic_intr_register(unit, handle,
                                nof_channels);
    sal_free(handle);

    /*
     * Set CMIC_COMMON_POOL_SHARED_CONFIG Register,
     * FIFO DMA are sent through AXI master port of CMC0
     */
    val = soc_pci_read(unit, CMIC_COMMON_POOL_SHARED_CONFIG_OFFSET);

    soc_reg_field_set(unit, CMIC_COMMON_POOL_SHARED_CONFIGr,
                      &val, MAP_FIFODMA_SLICE2_MEMWR_REQf,
                      MAP_FIFO_DMA_MEMWR_REQ_CMC0);

    soc_reg_field_set(unit, CMIC_COMMON_POOL_SHARED_CONFIGr,
                      &val, MAP_FIFODMA_SLICE1_MEMWR_REQf,
                      MAP_FIFO_DMA_MEMWR_REQ_CMC0);

    soc_reg_field_set(unit, CMIC_COMMON_POOL_SHARED_CONFIGr,
                      &val, MAP_FIFODMA_SLICE0_MEMWR_REQf,
                      MAP_FIFO_DMA_MEMWR_REQ_CMC0);

    soc_pci_write(unit, CMIC_COMMON_POOL_SHARED_CONFIG_OFFSET, val);

    /*
     * Programming WRR (Arbitration) within CMC. Configure WRR weight
     * for FIFO DMA channels. The defaults should work but still
     * writing the defaults.
     */
    val = soc_pci_read(unit, CMIC_COMMON_POOL_SHARED_FIFO_DMA_WRITE_ARB_CTRL_OFFSET);

    soc_reg_field_set(unit, CMIC_COMMON_POOL_SHARED_FIFO_DMA_WRITE_ARB_CTRLr,
                      &val, SLICE2_MEMWR_WRR_WEIGHTf,
                      CMICX_FIFO_DMA_MEMWR_WRR_WEIGHT);

    soc_reg_field_set(unit, CMIC_COMMON_POOL_SHARED_FIFO_DMA_WRITE_ARB_CTRLr,
                      &val, SLICE1_MEMWR_WRR_WEIGHTf,
                      CMICX_FIFO_DMA_MEMWR_WRR_WEIGHT);

    soc_reg_field_set(unit, CMIC_COMMON_POOL_SHARED_FIFO_DMA_WRITE_ARB_CTRLr,
                      &val, SLICE0_MEMWR_WRR_WEIGHTf,
                      CMICX_FIFO_DMA_MEMWR_WRR_WEIGHT);

    soc_pci_write(unit,
                  CMIC_COMMON_POOL_SHARED_FIFO_DMA_WRITE_ARB_CTRL_OFFSET,
                  val);

    val = soc_pci_read(unit, CMIC_COMMON_POOL_SHARED_FIFO_DMA_WRITE_AXI_MAP_CTRL_OFFSET);

    soc_reg_field_set(unit, CMIC_COMMON_POOL_SHARED_FIFO_DMA_WRITE_AXI_MAP_CTRLr,
                      &val, CHANNEL0_MEMWR_AXI_IDf,
                      CMICX_FIFO_DMA_AXI_ID);

    soc_reg_field_set(unit, CMIC_COMMON_POOL_SHARED_FIFO_DMA_WRITE_AXI_MAP_CTRLr,
                      &val, CHANNEL1_MEMWR_AXI_IDf,
                      CMICX_FIFO_DMA_AXI_ID);

    soc_reg_field_set(unit, CMIC_COMMON_POOL_SHARED_FIFO_DMA_WRITE_AXI_MAP_CTRLr,
                      &val, CHANNEL2_MEMWR_AXI_IDf,
                      CMICX_FIFO_DMA_AXI_ID);

    soc_reg_field_set(unit, CMIC_COMMON_POOL_SHARED_FIFO_DMA_WRITE_AXI_MAP_CTRLr,
                      &val, CHANNEL3_MEMWR_AXI_IDf,
                      CMICX_FIFO_DMA_AXI_ID);

    soc_reg_field_set(unit, CMIC_COMMON_POOL_SHARED_FIFO_DMA_WRITE_AXI_MAP_CTRLr,
                      &val, CHANNEL4_MEMWR_AXI_IDf,
                      CMICX_FIFO_DMA_AXI_ID);

    soc_reg_field_set(unit, CMIC_COMMON_POOL_SHARED_FIFO_DMA_WRITE_AXI_MAP_CTRLr,
                      &val, CHANNEL5_MEMWR_AXI_IDf,
                      CMICX_FIFO_DMA_AXI_ID);

    soc_reg_field_set(unit, CMIC_COMMON_POOL_SHARED_FIFO_DMA_WRITE_AXI_MAP_CTRLr,
                      &val, CHANNEL6_MEMWR_AXI_IDf,
                      CMICX_FIFO_DMA_AXI_ID);

    soc_reg_field_set(unit, CMIC_COMMON_POOL_SHARED_FIFO_DMA_WRITE_AXI_MAP_CTRLr,
                      &val, CHANNEL7_MEMWR_AXI_IDf,
                      CMICX_FIFO_DMA_AXI_ID);

    soc_reg_field_set(unit, CMIC_COMMON_POOL_SHARED_FIFO_DMA_WRITE_AXI_MAP_CTRLr,
                      &val, CHANNEL8_MEMWR_AXI_IDf,
                      CMICX_FIFO_DMA_AXI_ID);

    soc_reg_field_set(unit, CMIC_COMMON_POOL_SHARED_FIFO_DMA_WRITE_AXI_MAP_CTRLr,
                      &val, CHANNEL9_MEMWR_AXI_IDf,
                      CMICX_FIFO_DMA_AXI_ID);

    soc_pci_write(unit, CMIC_COMMON_POOL_SHARED_FIFO_DMA_WRITE_AXI_MAP_CTRL_OFFSET, val);


    val = soc_pci_read(unit, CMIC_COMMON_POOL_SHARED_FIFO_DMA_WRITE_AXI_MAP_CTRL_1_OFFSET);

    soc_reg_field_set(unit, CMIC_COMMON_POOL_SHARED_FIFO_DMA_WRITE_AXI_MAP_CTRL_1r,
                      &val, CHANNEL10_MEMWR_AXI_IDf,
                      CMICX_FIFO_DMA_AXI_ID);

    soc_reg_field_set(unit, CMIC_COMMON_POOL_SHARED_FIFO_DMA_WRITE_AXI_MAP_CTRL_1r,
                      &val, CHANNEL11_MEMWR_AXI_IDf,
                      CMICX_FIFO_DMA_AXI_ID);

    soc_pci_write(unit, CMIC_COMMON_POOL_SHARED_FIFO_DMA_WRITE_AXI_MAP_CTRL_1_OFFSET, val);

    for(y = 0; y < CMICX_N_FIFODMA_CHAN; y++) {
        cmicx_fifodma_ch_endian_set(unit, y, 0);
    }

    drv->fifodma_start             = _cmicx_fifodma_start;
    drv->fifodma_stop              = _cmicx_fifodma_stop;
    drv->fifodma_intr_enable       = _cmicx_fifodma_intr_enable;
    drv->fifodma_intr_disable      = _cmicx_fifodma_intr_disable;
    drv->fifodma_get_read_ptr      = _cmicx_fifodma_get_read_ptr;
    drv->fifodma_advance_read_ptr  = _cmicx_fifodma_advance_read_ptr;
    drv->fifodma_set_entries_read  = _cmicx_fifodma_set_entries_read;
    drv->fifodma_num_entries_get   = _cmicx_fifodma_num_entries_get;
    drv->fifodma_status_clear      = _cmicx_fifodma_status_clear;
    drv->fifodma_status_get        = _cmicx_fifodma_status_get;
    drv->fifodma_masks_get         = _cmicx_fifodma_masks_get;
    drv->l2mod_sbus_fifo_field_set = _cmicx_l2mod_sbus_fifo_field_set;
    drv->l2mod_sbus_fifo_field_get = _cmicx_l2mod_sbus_fifo_field_get;

    return(rv);
}

/*******************************************
* @function cmicx_fifodma_deinit
* purpose API to Deinitialize cmicx FIFO DMA
*
* @param unit [in] unit
* @param drv [in] soc_fifodma_drv_t pointer
*
* @returns SOC_E_NONE

*
* @end
 */
int cmicx_fifodma_deinit(int unit, soc_fifodma_drv_t *drv)
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

#endif /* BCM_CMICX_SUPPORT */

