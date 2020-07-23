/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: CMICM Register Based SBUS DMA Driver
 */

#include <shared/bsl.h>
#include <shared/alloc.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/sbusdma_internal.h>

#if defined(BCM_CMICM_SUPPORT) && defined(BCM_SBUSDMA_SUPPORT)
#include <soc/cmicm.h>
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
#include <soc/er_tcam.h>
#include <soc/triumph3.h>
#endif

#ifdef BCM_KATANA2_SUPPORT
#include <soc/katana2.h>
#include <soc/katana.h>
#endif

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
#if defined(BCM_CMICM_SUPPORT) && defined(BCM_SBUSDMA_SUPPORT)

#ifdef BCM_KATANA2_SUPPORT
#define RD_DMA_CFG_REG                   0
#define RD_DMA_HOTMEM_THRESHOLD_REG      1
#define RD_DMA_STAT                      2
#define RD_DMA_STAT_CLR                  3
#endif

#define SOC_SBUSDMA_FP_RETRIES 200

#undef PRINT_DMA_TIME

STATIC uint32 _soc_irq_sbusdma_ch[] = {
    IRQ_SBUSDMA_CH0_DONE,
    IRQ_SBUSDMA_CH1_DONE,
    IRQ_SBUSDMA_CH2_DONE
};

STATIC int _soc_multi_cmc = 0;

/*******************************************
* @function _cmicm_sbusdma_reg_array_read
* purpose DMA acceleration for soc_mem_read_range() on FB/ER
*
* @param unit [in] unit
* @param drv [in] soc_sbusdma_reg_param_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_cmicm_sbusdma_reg_array_read(int unit, soc_sbusdma_reg_param_t  *param)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    uint32        start_addr;
    uint32        count;
    uint32        data_beats;
    uint32        spacing;
    int           rv = SOC_E_NONE;
    uint32        ctrl, rval;
    uint8         at;
    int           ch;
    int           cmc = SOC_PCI_CMC(unit);
    schan_msg_t   msg;
    int           dst_blk, acc_type, dma;
#ifdef PRINT_DMA_TIME
    sal_usecs_t   start_time;
    int           diff_time;
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
    uint32        wa_rval, wa_val = 0;
#endif

    LOG_VERBOSE(BSL_LS_SOC_DMA,
             (BSL_META_U(unit,
                         "unit %d"
                         " mem %s.%s[%u] index %d-%d SER flags 0x%08x buffer %p\n"),
              unit, SOC_MEM_UFNAME(unit, param->mem), SOC_BLOCK_NAME(unit,
              param->copyno), param->array_id_start, param->index_begin,
              param->index_end, param->ser_flags, param->buffer));

   if (param->copyno == COPYNO_ALL) {
        return SOC_E_INTERNAL;
    }

    data_beats = soc_mem_entry_words(unit, param->mem);

    /* muticmc with grab all the channels and release one to grab */
    if (_soc_multi_cmc) {
        SOC_IF_ERROR_RETURN(cmicm_sbusdma_ch_get(unit, &cmc, &ch));
        assert((ch >= 0) && (ch < CMIC_CMCx_SBUSDMA_CHAN_MAX));
    } else {
        ch = soc->tdma_ch;
    }

    assert((ch >= 0) && (ch < soc->max_sbusdma_channels));
    count = param->index_end - param->index_begin + 1;
    if (count < 1) {
        if (_soc_multi_cmc) {
            cmicm_sbusdma_ch_put(unit, cmc, ch);
        }
        return SOC_E_NONE;
    }
    schan_msg_clear(&msg);
    acc_type = SOC_MEM_ACC_TYPE(unit, param->mem);
    dst_blk = SOC_BLOCK2SCH(unit, param->copyno);

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (0 != (param->ser_flags & _SOC_SER_FLAG_MULTI_PIPE)) {
        if ((param->ser_flags & _SOC_SER_FLAG_ACC_TYPE_MASK) ||
            (soc_feature(unit, soc_feature_unique_acc_type_access))) {

            /* Override ACC_TYPE in opcode */
            acc_type = param->ser_flags & _SOC_SER_FLAG_ACC_TYPE_MASK;
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    if (SOC_IS_ARAD(unit)) {
        /* DMA transaction indicator on SBUS */
        dma = 1;
    } else {
        dma = 0;
    }
    soc_schan_header_cmd_set(unit, &msg.header, READ_MEMORY_CMD_MSG,
                             dst_blk, 0, acc_type, 4, dma, 0);
    start_addr = soc_mem_addr_get(unit, param->mem, param->array_id_start,
                                  param->copyno, param->index_begin, &at);

    if (_soc_multi_cmc) {
        SBUS_DMA_LOCK(unit, cmc, ch);
    } else {
        TABLE_DMA_LOCK(unit);
    }

    LOG_VERBOSE(BSL_LS_SOC_DMA,
                (BSL_META_U(unit,
                 "unit=%d %s.acc_type = %0x, idx[%0d-%0d], cmc= %d, ch = %d\n"),
                 unit, SOC_MEM_NAME(unit, param->mem), acc_type,
                 param->index_begin, param->index_end, cmc, ch));

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_esm_correction) &&
        ((SOC_BLOCK_TYPE(unit, param->copyno) == SOC_BLK_ESM) ||
         (SOC_BLOCK_TYPE(unit, param->copyno) == SOC_BLK_ETU))) {
        SOC_ESM_LOCK(unit);
    }
#endif

    ctrl = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch));
    /* Set mode, clear abort, start (clears status and errors) */
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &ctrl,
                      MODEf, 0);
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &ctrl,
                      ABORTf, 0);
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &ctrl,
                      STARTf, 0);
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch), ctrl);
    /* Set 1st schan ctrl word as opcode */
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_OPCODE(cmc, ch), msg.dwords[0]);
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_HOSTADDR(cmc, ch),
                  soc_cm_l2p(unit, param->buffer));
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_ADDRESS(cmc, ch), start_addr);
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_COUNT(cmc, ch), count);

    /* Program beats, increment etc */
    rval = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_REQUEST(cmc, ch));
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                      REP_WORDSf, data_beats);
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                      REQ_WORDSf, 0);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_etu_support)) {
        if (SOC_BLOCK_TYPE(unit, param->copyno) == SOC_BLK_ETU) {
            int index0, index1, increment;
            soc_mem_t real_mem;

            soc_tcam_mem_index_to_raw_index(unit, param->mem, 0,
                                            &real_mem, &index0);
            soc_tcam_mem_index_to_raw_index(unit, param->mem, 1,
                                            &real_mem, &index1);
            increment = _shr_popcount(index1 - index0 - 1);
            soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                              INCR_SHIFTf, increment);
            SOC_IF_ERROR_RETURN(READ_ETU_TX_REQ_FIFO_CTLr(unit, &wa_rval));
            wa_val = soc_reg_field_get(unit, ETU_TX_REQ_FIFO_CTLr, wa_rval,
                                       CP_ACC_THRf);
            soc_reg_field_set(unit, ETU_TX_REQ_FIFO_CTLr, &wa_rval,
                              CP_ACC_THRf, 0);
            SOC_IF_ERROR_RETURN(WRITE_ETU_TX_REQ_FIFO_CTLr(unit, wa_rval));
        } else {
            soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                              INCR_SHIFTf, 0);
            if ((param->mem == EXT_L2_ENTRY_1m) ||
                 (param->mem == EXT_L2_ENTRY_2m)) {
                SOC_IF_ERROR_RETURN(READ_ETU_TX_REQ_FIFO_CTLr(unit, &wa_rval));
                wa_val = soc_reg_field_get(unit, ETU_TX_REQ_FIFO_CTLr, wa_rval,
                                           CP_ACC_THRf);
                soc_reg_field_set(unit, ETU_TX_REQ_FIFO_CTLr, &wa_rval,
                                  CP_ACC_THRf, 0);
                SOC_IF_ERROR_RETURN(WRITE_ETU_TX_REQ_FIFO_CTLr(unit, wa_rval));
            }
        }
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_SAND_SUPPORT)
    if (SOC_IS_SAND(unit)) {
        soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                          INCR_SHIFTf, 0);
    } else
#endif
    {
        soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                          INCR_SHIFTf, 0);
    }
    if (soc_feature(unit, soc_feature_multi_sbus_cmds)) {
        
        if (soc->sbusCmdSpacing < 0) {
            spacing = data_beats > 7 ? data_beats + 1 : 8;
        } else {
            spacing = soc->sbusCmdSpacing;
        }
        if (!((SOC_BLOCK_TYPE(unit, param->copyno) == SOC_BLK_IPIPE) ||
             (SOC_BLOCK_TYPE(unit, param->copyno) == SOC_BLK_EPIPE))) {
            spacing = 0;
        }
        soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                          PEND_CLOCKSf, spacing);
    }
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_REQUEST(cmc, ch), rval);
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &ctrl, STARTf, 1);
    /* Start DMA */
#ifdef PRINT_DMA_TIME
    start_time = sal_time_usecs();
#endif
    /* On BCMSIM, this interrupt just be reported one time. 
    If we enable this interrupt after starting table DMA, the interrupt may be lost.
    But on chip, the interrupt will be reported continually until we clear STARTf of
    CMIC_CMC0_SBUSDMA_CH0_CONTROLr */
    if (soc->tableDmaIntrEnb) {
        soc_cmicm_cmcx_intr0_enable(unit, cmc, _soc_irq_sbusdma_ch[ch]);
    }

    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch), ctrl);

    LOG_VERBOSE(BSL_LS_SOC_DMA,
             (BSL_META_U(unit,
                         "%d entries %d beats "
                         "addr 0x%x (index %d-%d) Interrupt-Mode(%d)\n"),
              count, data_beats, start_addr, param->index_begin,
              param->index_end, soc->tableDmaIntrEnb));

    rv = SOC_E_TIMEOUT;
    if (soc->tableDmaIntrEnb) {
        if (SBUSDMA_DMA_INTR_WAIT(unit, cmc, ch, soc->tableDmaTimeout) < 0) {
            rv = SOC_E_TIMEOUT;
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                       (BSL_META_U(unit,
                                  "%s.%s interrupt timeout\n"),
                       SOC_MEM_UFNAME(unit, param->mem),
                       SOC_BLOCK_NAME(unit, param->copyno)));
        }
#ifdef PRINT_DMA_TIME
        diff_time = SAL_USECS_SUB(sal_time_usecs(), start_time);
        LOG_VERBOSE(BSL_LS_SOC_DMA,
                    (BSL_META_U(unit,
                     "(%s)HW dma read time[count=%u, beats=%u]: %d usecs, [%d nsecs per op]\n"),
                     SOC_MEM_UFNAME(unit, param->mem),count,data_beats,
                     diff_time, diff_time*1000/count));
#endif
        soc_cmicm_cmcx_intr0_disable(unit, cmc, _soc_irq_sbusdma_ch[ch]);

        rval = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc, ch));
        if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                              rval, DONEf)) {
            rv = SOC_E_NONE;
            if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                                  rval, ERRORf)) {
                rv = SOC_E_FAIL;
                cmicm_sbusdma_curr_op_details(unit, cmc, ch);
            }
        }
    } else {
        soc_timeout_t to;
        soc_timeout_init(&to, soc->tableDmaTimeout, 0);
        do {
            rval = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc, ch));
            if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                                  rval, DONEf)) {
                rv = SOC_E_NONE;
#ifdef PRINT_DMA_TIME
                diff_time = SAL_USECS_SUB(sal_time_usecs(), start_time);
                LOG_VERBOSE(BSL_LS_SOC_DMA,
                            (BSL_META_U(unit,
                             "(%s)HW dma read poll time[count=%u, beats=%u]: %d usecs, [%d nsecs per op]\n"),
                             SOC_MEM_UFNAME(unit, param->mem),count,
                             data_beats, diff_time, diff_time*1000/count));
#endif
                if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                                      rval, ERRORf)) {
                    rv = SOC_E_FAIL;
                    cmicm_sbusdma_curr_op_details(unit, cmc, ch);
                }
                break;
            }
        } while(!(soc_timeout_check(&to)));
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_etu_support)) {
        if ((SOC_BLOCK_TYPE(unit, param->copyno) == SOC_BLK_ETU) ||
            ((param->mem == EXT_L2_ENTRY_1m) ||
             (param->mem == EXT_L2_ENTRY_2m))) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ETU_TX_REQ_FIFO_CTLr,
                                        REG_PORT_ANY,
                                        CP_ACC_THRf, wa_val));
        }
    }
#endif

    if (rv != SOC_E_NONE) {
        if (rv != SOC_E_TIMEOUT) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s.%s failed(ERR)\n"),
                       SOC_MEM_UFNAME(unit, param->mem),
                       SOC_BLOCK_NAME(unit, param->copyno)));
            cmicm_sbusdma_error_details(unit,
                soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc, ch)));
        } else { /* Timeout cleanup */
            soc_timeout_t to;

            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s.%s %s timeout\n"),
                       SOC_MEM_UFNAME(unit, param->mem),
                       SOC_BLOCK_NAME(unit, param->copyno),
                       soc->tableDmaIntrEnb ? "interrupt" : "polling"));
#ifdef BCM_PETRA_SUPPORT
            if (SOC_IS_ARAD(unit)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                           "This DMA failure may be due to wrong PCI configuration. "
                           "Timeout configured to %dus.\n"),
                           soc->tableDmaTimeout));
            }
#endif /* BCM_PETRA_SUPPORT */

            /* Abort Table DMA */
            ctrl = soc_pci_read(unit,
                                CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch));
            soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &ctrl,
                              ABORTf, 1);
            soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch), ctrl);

            /* Check the done bit to confirm */
            soc_timeout_init(&to, soc->tableDmaTimeout, 0);
            while (1) {
                rval = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc, ch));
                if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                    rval, DONEf)) {
                    break;
                }
                if (soc_timeout_check(&to)) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                               "_cmicm_sbusdma_reg_read: Abort Failed\n")));
                    break;
                }
            }
        }
    }

    soc_cm_sinval(unit, (void *)param->buffer, WORDS2BYTES(data_beats) * count);
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_esm_correction) &&
                    ((SOC_BLOCK_TYPE(unit, param->copyno) == SOC_BLK_ESM) ||
                    (SOC_BLOCK_TYPE(unit, param->copyno) == SOC_BLK_ETU))) {
                    SOC_ESM_UNLOCK(unit);
    }
#endif
    if (_soc_multi_cmc) {
        SBUS_DMA_UNLOCK(unit, cmc, ch);
        cmicm_sbusdma_ch_put(unit, cmc, ch);
    } else {
        TABLE_DMA_UNLOCK(unit);
    }

    return rv;
}

/*******************************************
* @function _cmicm_sbusdma_reg_array_write
* purpose DMA acceleration for soc_mem_write_range() on FB/ER
*
* @param unit [in] unit
* @param drv [in] soc_sbusdma_reg_param_t pointer
* @param mem_clear [in] flag to clear the memory
* @param clear_buf_ent [in] clear_buf_ent is not used as
*        SLAM DMA only supports a one entry buffer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */

STATIC int
_cmicm_sbusdma_reg_array_write(int unit, soc_sbusdma_reg_param_t  *param)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    uint32        start_addr;
    uint32        count = 0;
    uint32        data_beats;
    uint32        spacing;
    int           rv = SOC_E_NONE;
    uint32        ctrl, rval;
    uint8         at, rev_slam = 0;
    int           ch;
    int           cmc = SOC_PCI_CMC(unit);
    schan_msg_t   msg;
    int           dst_blk, acc_type, data_byte_len, dma;
    unsigned      array_index;
    int           dma_retries=0;
    int           max_retries=0;

#ifdef PRINT_DMA_TIME
    sal_usecs_t   start_time;
    int           diff_time;
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
    uint32        wa_rval, wa_val = 0;
#endif
#if defined(BCM_PETRA_SUPPORT)
    uint32        *cache;
    uint8         *vmap;
    int           index = 0;
    int           mem_array_vmap_offset;
    int           mem_array_cache_offset;
    int           blk, copyno_override, cache_copyno = param->copyno;
#endif

    LOG_VERBOSE(BSL_LS_SOC_DMA,
             (BSL_META_U(unit,
                         "unit %d"
                         " mem %s[%u-%u].%s index %d-%d buffer %p\n"),
              unit, SOC_MEM_UFNAME(unit, param->mem), param->array_id_start,
              param->array_id_end, SOC_BLOCK_NAME(unit, param->copyno),
              param->index_begin, param->index_end, param->buffer));

    data_beats = soc_mem_entry_words(unit, param->mem);

    /* muticmc with grab all the channels and release one to grab */
    if (_soc_multi_cmc) {
        SOC_IF_ERROR_RETURN(cmicm_sbusdma_ch_get(unit, &cmc, &ch));
        assert((ch >= 0) && (ch < CMIC_CMCx_SBUSDMA_CHAN_MAX));
    } else {
        ch = soc->tslam_ch;
    }

    assert((ch >= 0) && (ch < soc->max_sbusdma_channels));

    if (param->index_begin > param->index_end) {
        count = param->index_begin - param->index_end + 1;
        rev_slam = 1;
    } else {
        count = param->index_end - param->index_begin + 1;
    }
    if (count < 1) {
        if (_soc_multi_cmc) {
            cmicm_sbusdma_ch_put(unit, cmc, ch);
        }
        return SOC_E_NONE;
    }

    schan_msg_clear(&msg);
    acc_type = SOC_MEM_ACC_TYPE(unit, param->mem);
    dst_blk = SOC_BLOCK2SCH(unit, param->copyno);
    data_byte_len = data_beats * sizeof(uint32);

#ifdef FW_BCAST_DOWNLOAD
    if (SOC_IS_TOMAHAWK2(unit)) {
        if (param->flags & SOC_MEM_WRITE_BCAST_OP) {
            int bcast_id = 0;

            rv = SOC_E_FAIL;
            if ((SOC_PORTCTRL_FUNCTIONS(unit) != NULL)
                && (SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_sbus_bcast_id_get != NULL)) {
                rv = SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_sbus_bcast_id_get(unit, dst_blk, &bcast_id);
                if (SOC_SUCCESS(rv)) {
                    dst_blk = bcast_id;
                }
            }

            param->flags &= ~SOC_MEM_WRITE_BCAST_OP;
        }
    }
#endif

    if (SOC_IS_ARAD(unit)) {
        dma = 1;
    } else {
        dma = 0;
    }

    soc_schan_header_cmd_set(unit, &msg.header, WRITE_MEMORY_CMD_MSG,
                             dst_blk, 0, acc_type, data_byte_len, dma, 0);

    if (_soc_multi_cmc) {
        SBUS_DMA_LOCK(unit, cmc, ch);
    } else {
        TSLAM_DMA_LOCK(unit);
    }

    LOG_VERBOSE(BSL_LS_SOC_DMA,
                (BSL_META_U(unit,
                 "unit=%d %s.acc_type = %0x, idx[%0d-%0d], cmc= %d, ch = %d\n"),
                 unit, SOC_MEM_NAME(unit, param->mem), acc_type,
                 param->index_begin, param->index_end, cmc, ch));

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_esm_correction) &&
                    ((SOC_BLOCK_TYPE(unit, param->copyno) == SOC_BLK_ESM) ||
                    (SOC_BLOCK_TYPE(unit, param->copyno) == SOC_BLK_ETU))) {
                    SOC_ESM_LOCK(unit);
    }
#endif

    for (array_index = param->array_id_start;
                           array_index <= param->array_id_end; ++array_index) {
        if (!param->flags || (param->flags & SOC_MEM_WRITE_SET_ONLY)) {
            start_addr = soc_mem_addr_get(unit, param->mem, array_index,
                                          param->copyno, param->index_begin, &at);

            ctrl = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch));
            /* Set mode, clear abort, start (clears status and errors) */
            soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &ctrl,
                              MODEf, 0);
            soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &ctrl,
                              ABORTf, 0);
            soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &ctrl,
                              STARTf, 0);
            soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch), ctrl);
            /* Set 1st schan ctrl word as opcode */
            soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_OPCODE(cmc, ch),
                          msg.dwords[0]);
            soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_HOSTADDR(cmc, ch),
                          soc_cm_l2p(unit, param->buffer));
            soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_ADDRESS(cmc, ch),
                          start_addr);
            soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_COUNT(cmc, ch), count);

            /* Program beats, increment/decrement, single etc */
            rval = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_REQUEST(cmc, ch));
            soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                              REQ_WORDSf, data_beats);
            soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                              REP_WORDSf, 0);

#if defined(BCM_TRIUMPH3_SUPPORT)
            if (soc_feature(unit, soc_feature_etu_support)) {
                if (SOC_BLOCK_TYPE(unit, param->copyno) == SOC_BLK_ETU) {
                    int index0, index1, increment;
                    soc_mem_t real_mem;

                    soc_tcam_mem_index_to_raw_index(unit, param->mem, 0,
                                                    &real_mem, &index0);
                    soc_tcam_mem_index_to_raw_index(unit, param->mem, 1,
                                                    &real_mem, &index1);
                    increment = _shr_popcount(index1 - index0 - 1);
                    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                                    INCR_SHIFTf, increment);
                    SOC_IF_ERROR_RETURN(READ_ETU_TX_REQ_FIFO_CTLr(unit, &wa_rval));
                    wa_val = soc_reg_field_get(unit, ETU_TX_REQ_FIFO_CTLr, wa_rval,
                                               CP_ACC_THRf);
                    soc_reg_field_set(unit, ETU_TX_REQ_FIFO_CTLr, &wa_rval,
                                      CP_ACC_THRf, 0);
                    SOC_IF_ERROR_RETURN(WRITE_ETU_TX_REQ_FIFO_CTLr(unit, wa_rval));
                } else {
                    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                                      INCR_SHIFTf, 0);
                    if ((param->mem == EXT_L2_ENTRY_1m) ||
                                       (param->mem == EXT_L2_ENTRY_2m)) {
                        SOC_IF_ERROR_RETURN(READ_ETU_TX_REQ_FIFO_CTLr(unit, &wa_rval));
                        wa_val = soc_reg_field_get(unit, ETU_TX_REQ_FIFO_CTLr, wa_rval,
                                                   CP_ACC_THRf);
                        soc_reg_field_set(unit, ETU_TX_REQ_FIFO_CTLr, &wa_rval,
                                          CP_ACC_THRf, 0);
                        SOC_IF_ERROR_RETURN(WRITE_ETU_TX_REQ_FIFO_CTLr(unit, wa_rval));
                    }
                }
            } else
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_SAND_SUPPORT)
            if (SOC_IS_SAND(unit)) {
                soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                              INCR_SHIFTf, 0);
            } else
#endif
            {
                soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                                  INCR_SHIFTf, 0);
            }
            if (param->mem_clear) {
                soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                                  REQ_SINGLEf, 1);
            } else {
                soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                                  REQ_SINGLEf, 0);
            }
            if (rev_slam) {
                soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                                  DECRf, 1);
            } else {
                soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                                  DECRf, 0);
            }
            if (soc_feature(unit, soc_feature_multi_sbus_cmds)) {
                
                if (soc->sbusCmdSpacing < 0) {
                    spacing = data_beats > 7 ? data_beats + 1 : 8;
                } else {
                    spacing = soc->sbusCmdSpacing;
                }
                if (!((SOC_BLOCK_TYPE(unit, param->copyno) == SOC_BLK_IPIPE) ||
                     (SOC_BLOCK_TYPE(unit, param->copyno) == SOC_BLK_EPIPE))) {
                    spacing = 0;
                }
                soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                                  PEND_CLOCKSf, spacing);
            }
            soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_REQUEST(cmc, ch), rval);

            if (param->mem_clear) {
                soc_cm_sflush(unit, param->buffer, WORDS2BYTES(data_beats));
            } else {
                soc_cm_sflush(unit, param->buffer, WORDS2BYTES(data_beats) * count);
            }

            LOG_VERBOSE(BSL_LS_SOC_DMA,
                     (BSL_META_U(unit,
                                 "_soc_mem_sbusdma_write: %d entries %d beats "
                                 "addr 0x%x (index %d-%d) Interrupt-Mode(%d)\n"),
                      count, data_beats, start_addr,
                      param->index_begin, param->index_end,
                      soc->tslamDmaIntrEnb));
        }

        if (!param->flags || (param->flags & SOC_MEM_WRITE_COMMIT_ONLY)) {
            ctrl = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch));
            soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &ctrl, STARTf, 1);
            /* On BCMSIM, this interrupt just be reported one time. 
            If we enable this interrupt after starting table DMA, the interrupt may be lost.
            But on chip, the interrupt will be reported continually until we clear STARTf of
            CMIC_CMC0_SBUSDMA_CH0_CONTROLr */
            if (soc->tslamDmaIntrEnb) {
                soc_cmicm_cmcx_intr0_enable(unit, cmc, _soc_irq_sbusdma_ch[ch]);
            }

            /* Start DMA */
            soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch), ctrl);
        }

        if (!param->flags || (param->flags & SOC_MEM_WRITE_STATUS_ONLY)) {
#ifdef PRINT_DMA_TIME
            start_time = sal_time_usecs();
#endif
            rv = SOC_E_TIMEOUT;
            if (soc->tslamDmaIntrEnb) {
                if (SBUSDMA_DMA_INTR_WAIT(unit, cmc, ch, soc->tslamDmaTimeout) < 0) {
                        rv = SOC_E_TIMEOUT;
                        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                   (BSL_META_U(unit,
                                              "%s.%s interrupt timeout\n"),
                                   SOC_MEM_UFNAME(unit, param->mem),
                                   SOC_BLOCK_NAME(unit, param->copyno)));
                }
#ifdef PRINT_DMA_TIME
                diff_time = SAL_USECS_SUB(sal_time_usecs(), start_time);
                LOG_VERBOSE(BSL_LS_SOC_DMA,
                            (BSL_META_U(unit,
                             "(%s)HW dma write time[count=%u, beats=%u]: %d usecs, [%d nsecs per op]\n"),
                             SOC_MEM_UFNAME(unit, param->mem),count,data_beats,
                             diff_time, diff_time*1000/count));
#endif
                soc_cmicm_cmcx_intr0_disable(unit, cmc, _soc_irq_sbusdma_ch[ch]);

                rval = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc, ch));
                if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                                      rval, DONEf)) {
                    rv = SOC_E_NONE;
                    if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                                          rval, ERRORf)) {
                        rv = SOC_E_FAIL;
                        cmicm_sbusdma_curr_op_details(unit, cmc, ch);
                    }
                }
            } else {
                soc_timeout_t to;
                soc_timeout_init(&to, soc->tslamDmaTimeout, 0);
                /* Accessing the memory location prior to read loop improves performance */
                rval=soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc, ch));
                do {
                    rval = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc, ch));
                    if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                                          rval, DONEf)) {
#ifdef PRINT_DMA_TIME
                        diff_time = SAL_USECS_SUB(sal_time_usecs(), start_time);
                        LOG_VERBOSE(BSL_LS_SOC_DMA,
                                    (BSL_META_U(unit,
                                    "(%s)HW dma write poll time[count=%u, beats=%u]: %d usecs, [%d nsecs per op]\n"),
                                    SOC_MEM_UFNAME(unit, param->mem),count, data_beats,
                                    diff_time, diff_time*1000/count));
#endif
                        rv = SOC_E_NONE;
                        if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                                              rval, ERRORf)) {
                            rv = SOC_E_FAIL;
                            cmicm_sbusdma_curr_op_details(unit, cmc, ch);
                        }
                        break;
                    }
                } while(dma_retries++ < max_retries || !(soc_timeout_check(&to)));
            }

#if defined(BCM_TRIUMPH3_SUPPORT)
            if (soc_feature(unit, soc_feature_etu_support)) {
                if ((SOC_BLOCK_TYPE(unit, param->copyno) == SOC_BLK_ETU) ||
                    ((param->mem == EXT_L2_ENTRY_1m) ||
                               (param->mem == EXT_L2_ENTRY_2m))) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_field32_modify(unit, ETU_TX_REQ_FIFO_CTLr,
                          REG_PORT_ANY, CP_ACC_THRf, wa_val));
                }
            }
#endif

            if (rv < 0) {
                if (rv != SOC_E_TIMEOUT) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "%s[%u].%s failed(ERR)\n"),
                               SOC_MEM_UFNAME(unit, param->mem), array_index,
                               SOC_BLOCK_NAME(unit, param->copyno)));
                    cmicm_sbusdma_error_details(unit,
                        soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc, ch)));
                } else { /* Timeout cleanup */
                    soc_timeout_t to;

                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "%s[%u].%s %s timeout\n"),
                               SOC_MEM_UFNAME(unit, param->mem), array_index,
                               SOC_BLOCK_NAME(unit, param->copyno),
                               soc->tslamDmaIntrEnb ? "interrupt" : "polling"));

                    /* Abort Table DMA */
                    ctrl = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch));
                    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &ctrl, ABORTf, 1);
                    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch), ctrl);

                    /* Check the done bit to confirm */
                    soc_timeout_init(&to, soc->tslamDmaTimeout, 0);
                    while (1) {
                        rval = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc, ch));
                        if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                            rval, DONEf)) {
                            break;
                        }
                        if (soc_timeout_check(&to)) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                       "_soc_mem_sbusdma_write: Abort Failed\n")));
                            break;
                        }
                    }
                }
            }
        }
#if defined(BCM_PETRA_SUPPORT)
        /* update cache, for broadcast block loop each block */
        copyno_override = soc_mem_broadcast_block_get(unit,param->mem);
        if (cache_copyno == copyno_override) {
            cache_copyno = COPYNO_ALL;
        }
        SOC_MEM_BLOCK_ITER(unit, param->mem, blk) {
            if ((cache_copyno != COPYNO_ALL) && (cache_copyno != blk)) {
                continue;
            }
            /* It will not go into this API if param->copyno = -1. */
            /* coverity[negative_returns] */
            cache = SOC_MEM_STATE(unit, param->mem).cache[blk];
            vmap = SOC_MEM_STATE(unit, param->mem).vmap[blk];
            mem_array_vmap_offset = array_index * soc_mem_index_count(unit, param->mem);
            mem_array_cache_offset = mem_array_vmap_offset * data_beats;
            if (cache != NULL && !SOC_MEM_TEST_SKIP_CACHE(unit)) {
                for (index = param->index_begin; index <= param->index_end; index ++) {
                    if ((param->mem_clear) &&
                        !(CACHE_VMAP_TST(vmap, index + mem_array_vmap_offset))) {
                        continue;
                    }
                    sal_memcpy(cache + mem_array_cache_offset + index * data_beats,
                               param->buffer, data_beats * 4);
                    /* update vmap */
                    CACHE_VMAP_SET(vmap, mem_array_vmap_offset + index);
                }
            }
        }
#endif
    } /* end of loop over array indices */

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_esm_correction) &&
                    ((SOC_BLOCK_TYPE(unit, param->copyno) == SOC_BLK_ESM) ||
                    (SOC_BLOCK_TYPE(unit, param->copyno) == SOC_BLK_ETU))) {
                    SOC_ESM_UNLOCK(unit);
    }
#endif
    if (_soc_multi_cmc) {
        SBUS_DMA_UNLOCK(unit, cmc, ch);
        cmicm_sbusdma_ch_put(unit, cmc, ch);
    } else {
        TSLAM_DMA_UNLOCK(unit);
    }

    return rv;
}

/*******************************************
* @function _cmicm_sbusdma_reg_clear_specific
* purpose clear a specific memory/table region using
*          DMA write (slam) acceleration.
*
* @param unit [in] unit
* @param param [in] soc_sbusdma_reg_param_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_cmicm_sbusdma_reg_clear_specific(int unit, soc_sbusdma_reg_param_t  *param)
{
    int    rv = 0, chunk_size, chunk_entries, mem_size, entry_words;
    int    index, blk, tmp, copyno;
    uint32 *buf;

    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)) {
        return SOC_E_NONE;
    }
    chunk_size = SOC_MEM_CLEAR_CHUNK_SIZE_GET(unit);
    /* get legal values for indices, if too
     * small/big use the memory's boundaries
     */
    tmp = soc_mem_index_min(unit, param->mem);
    if (param->index_begin < soc_mem_index_min(unit, param->mem)) {
      param->index_begin = tmp;
    }
    if (param->index_end < param->index_begin) {
        param->index_end = param->index_begin;
    } else {
         tmp = soc_mem_index_max(unit, param->mem);
         if (param->index_end > tmp) {
             param->index_end = tmp;
         }
    }

    entry_words = soc_mem_entry_words(unit, param->mem);
    mem_size = (param->index_end - param->index_begin + 1) * entry_words * 4;
    if (mem_size < chunk_size) {
        chunk_size = mem_size;
    }
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(unit) && (entry_words * 4 > chunk_size)) {
        /* The default is to use a one table entry buffer,
         * if not given a bigger chunk size
         * make sure the buffer size holds at least one table entry
         */
        chunk_size = entry_words * 4;
    }
#endif

    buf = soc_cm_salloc(unit, chunk_size, "mem_clear_buf");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    chunk_entries = chunk_size / (entry_words * 4);
    for (index = 0; index < chunk_entries; index++) {
        sal_memcpy(buf + (index * entry_words),
                   param->buffer, entry_words * 4);
    }

    /* get legal values for memory array indices */
    if (SOC_MEM_IS_ARRAY(unit, param->mem)) {
        soc_mem_array_info_t *maip = SOC_MEM_ARRAY_INFOP(unit, param->mem);
        if (maip) {
            if (param->array_id_end >= (maip->numels + maip->first_array_index)) {
                param->array_id_end = (maip->numels - 1) + maip->first_array_index;
            } else if (param->array_id_start < maip->first_array_index) {
                param->array_id_start = maip->first_array_index;
            }
        }
    } else {
        param->array_id_start = param->array_id_end = 0;
    }
    copyno = param->copyno;
    MEM_LOCK(unit, param->mem);
    SOC_MEM_BLOCK_ITER(unit, param->mem, blk) {
        if (copyno != COPYNO_ALL && copyno != blk) {
            continue;
        }
        param->buffer = buf;
        param->ser_flags = 0;
        param->flags = 0;
        param->mem_clear = TRUE;
        param->copyno = blk;
        rv = _cmicm_sbusdma_reg_array_write(unit, param);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_mem_sbusdma_clear: %s.%s[%d-%d] failed: %s\n"),
                                  SOC_MEM_UFNAME(unit, param->mem),
                                  SOC_BLOCK_NAME(unit, blk),
                                  param->index_begin, param->index_end,
                                  soc_errmsg(rv)));
        }
    }
    MEM_UNLOCK(unit, param->mem);
    /* restoring the copy number since it is modified */
    param->copyno = copyno;
    soc_cm_sfree(unit, buf);
    return rv;
}

/*******************************************
* @function _cmicm_sbusdma_cmc_max_num_ch_get
* @Returns number of SBUSDMA channels in cmc
*
* @end
 */
STATIC uint32
_cmicm_sbusdma_cmc_max_num_ch_get(int unit)
{
    return CMIC_CMCx_SBUSDMA_CHAN_MAX;
}


/*******************************************
* @function cmicm_sbusdma_reg_init
* purpose API to Initialize cmicm Register DMA
*
* @param unit [in] unit
* @param drv [out] soc_sbusdma_desc_drv_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int cmicm_sbusdma_reg_init(int unit, soc_sbusdma_reg_drv_t *drv)
{
    /* Cache multi CMC flag */
    if (SOC_CHECK_MULTI_CMC(unit)) {
        _soc_multi_cmc = 1;
    }
    drv->soc_mem_sbusdma_read  = _cmicm_sbusdma_reg_array_read;
    drv->soc_mem_sbusdma_write = _cmicm_sbusdma_reg_array_write;
    drv->soc_mem_sbusdma_clear = _cmicm_sbusdma_reg_clear_specific;
    drv->soc_sbusdma_cmc_max_num_ch_get = _cmicm_sbusdma_cmc_max_num_ch_get;

    return SOC_E_NONE;
}

#endif /* BCM_SBUSDMA_SUPPORT */
#endif /* defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) */

