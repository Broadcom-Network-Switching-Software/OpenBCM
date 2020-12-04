/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: CMICX SBUS-DMA Register Driver
 */

#include <shared/bsl.h>
#include <shared/alloc.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/sbusdma_internal.h>

#if defined(BCM_CMICX_SUPPORT) && defined(BCM_SBUSDMA_SUPPORT)
#include <soc/cmicx.h>
#include <soc/intr_cmicx.h>

#ifdef BCM_SAND_SUPPORT
#include <soc/sand/sand_aux_access.h>
#endif

#define SBUSDMA_REG_WAIT(wparam) \
                  (&((cmic_sbusdma_wparam_t  *)wparam)->wait)

typedef struct cmicx_sbusdma_prog_s {
    uint8     rev_slam;
    uint8     mem_clear;
    int       opcode;
    int       cmc;
    int       ch;
    uint32    data_beats;
    uint32    start_addr;
    uint32    count;
    uint32    ser_flags;
}cmicx_sbusdma_prog_t;


/*******************************************
* @function _cmicx_sbusdma_reg_program
* purpose Programing the registers and start DMA
*
* @param unit [in] unit
* @param param [in] soc_sbusdma_reg_param_t pointer
* @param prog [in] cmicx_sbusdma_prog_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC void
_cmicx_sbusdma_reg_program(int unit, schan_msg_t *msg,
                           soc_sbusdma_reg_param_t  *param,
                           cmicx_sbusdma_prog_t  *prog)
{
    uint32        ctrl, rval;
    uint32        spacing;
    int big_pio, big_packet, big_other;
    soc_control_t *soc = SOC_CONTROL(unit);

     LOG_VERBOSE(BSL_LS_SOC_DMA,
               (BSL_META_U(unit,
                "CMIC_CMCx_SBUSDMA_CHy_REQUEST=0x%x, "
                "CMIC_CMCx_SBUSDMA_CHy_CONTROL= 0x%x\n"),
                CMIC_CMCx_SBUSDMA_CHy_REQUEST(prog->cmc, prog->ch),
                CMIC_CMCx_SBUSDMA_CHy_CONTROL(prog->cmc, prog->ch)));
    soc_endian_get(unit, &big_pio, &big_packet, &big_other);
    ctrl = soc_pci_read(unit,
                        CMIC_CMCx_SBUSDMA_CHy_CONTROL(prog->cmc, prog->ch));
    /* Set mode */
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &ctrl,
                      MODEf, 0);
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(prog->cmc,
                  prog->ch), ctrl);
    /* Set 1st schan ctrl word as opcode */
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_OPCODE(prog->cmc, prog->ch),
                  msg->dwords[0]);
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_HOSTADDR_LO(prog->cmc, prog->ch),
                  PTR_TO_INT(soc_cm_l2p(unit, param->buffer)));
    /* Add HI ADDRESS */
    if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) {
        soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_HOSTADDR_HI(prog->cmc, prog->ch),
                  (PTR_HI_TO_INT(soc_cm_l2p(unit, param->buffer)) |
                                CMIC_PCIE_SO_OFFSET));
    } else {
        soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_HOSTADDR_HI(prog->cmc, prog->ch),
                  PTR_HI_TO_INT(soc_cm_l2p(unit, param->buffer)));
    }
    /* Start Address */
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_ADDRESS(prog->cmc, prog->ch),
                  prog->start_addr);
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_COUNT(prog->cmc, prog->ch),
                  prog->count);

    /* Program beats, increment/decrement, single */

    rval = 0;
    if (big_other) {
       soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
             HOSTMEMWR_ENDIANESSf, 1);
       soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
             HOSTMEMRD_ENDIANESSf, 1);
    }
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
      INCR_SHIFTf, 0);

    if(prog->opcode == READ_MEMORY_CMD_MSG) {
        soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                          REP_WORDSf, prog->data_beats);
        soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                          REQ_WORDSf, 0);
    }

    if(prog->opcode == WRITE_MEMORY_CMD_MSG) {
        soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                          REQ_WORDSf, prog->data_beats);
        soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                          REP_WORDSf, 0);
        if (prog->mem_clear) {
            soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                              REQ_SINGLEf, 1);
        } else {
            soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                              REQ_SINGLEf, 0);
        }
        if (prog->rev_slam) {
            soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                              DECRf, 1);
        } else {
            soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                              DECRf, 0);
        }
#if defined(BCM_HELIX5_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
        if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit) ||
            SOC_IS_FIRELIGHT(unit)) {
            if(param->mem == PMQPORT_WC_UCMEM_DATAm) {
                /* H/W will increment the PRAM address automatically per write transaction in PM4X10Q Q mode.
                   Disable CMIC SBUS increase address.
                 */
                soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &rval,
                                  INCR_NOADDf, 1);
            }
        }
#endif
    }

    if (soc_feature(unit, soc_feature_multi_sbus_cmds)) {
        /*
         * rules for spacing:
         * 1. word_count + 1
         * 2. special requirements (i.e. higher value)
         *    before collect all special requirement, set to 8 or above
         */
        if (soc->sbusCmdSpacing < 0) {
            spacing = prog->data_beats > 7 ? prog->data_beats + 1 : 8;
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
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_REQUEST(prog->cmc,
                prog->ch), rval);

    return;
}

/*******************************************
* @function _cmicx_sbusdma_reg_complete
* purpose Wait and complete the DMA
*
* @param unit [in] unit
* @param param [in] soc_sbusdma_reg_param_t pointer
* @param wparam [in] void pointer
* @param do_not_free_channel [in] prevent freeing the channel
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_cmicx_sbusdma_reg_complete(int unit, void *data,
                                      void *wparam, uint32 do_not_free_channel)
{
    int rv;
    uint32 do_not_perform_start_operation = 0;
    uint32 ctrl, rval;
    cmic_sbusdma_wait_t  *wait;
    soc_sbusdma_reg_param_t  *param;
#ifdef PRINT_DMA_TIME
    int diff_time;
    uint32 count;
    uint32 data_beats;
#endif

    if ((data == NULL) || (wparam == NULL)) {
        return SOC_E_PARAM;
    }

    param = (soc_sbusdma_reg_param_t  *)data;
    wait = SBUSDMA_REG_WAIT(wparam);
#ifdef PRINT_DMA_TIME
    count = param->index_end - param->index_begin + 1;
    data_beats = soc_mem_entry_words(unit, param->mem);
    wait->start_time = sal_time_usecs();
#endif
    if((param->flags & SOC_MEM_WRITE_STATUS_ONLY) != 0)
    {
        do_not_perform_start_operation = 1;
    }
    if (wait->intr_enb) {
        rv = cmicx_sbusdma_intr_wait(unit, wait->cmc, wait->ch,
                                     wait->timeout, do_not_perform_start_operation);
        if (rv != SOC_E_TIMEOUT) {
            SOC_CONTROL(unit)->stat.intr_tslam++;
        }
    } else {
        rv = cmicx_sbusdma_poll_wait(unit, wait->cmc, wait->ch,
                                     wait->timeout, do_not_perform_start_operation);
    }

#ifdef PRINT_DMA_TIME
    diff_time = SAL_USECS_SUB(sal_time_usecs(), wait->start_time);
    LOG_VERBOSE(BSL_LS_SOC_DMA,
                (BSL_META_U(unit,
                 "(%s)HW dma time[count=%u beats=%u]: %d usecs, [%d nsecs per op]\n"),
                 SOC_MEM_UFNAME(unit, param->mem),count,data_beats,
                 diff_time, diff_time*1000/count));
#endif

    if (rv != SOC_E_NONE) {
        if (rv != SOC_E_TIMEOUT) {
            LOG_ERROR(BSL_LS_SOC_DMA,
                      (BSL_META_U(unit,
                                  "%s.%s failed(ERR %d)\n"),
                       SOC_MEM_UFNAME(unit, param->mem),
                       SOC_BLOCK_NAME(unit, param->copyno), rv));
            cmicx_sbusdma_error_details(unit,
                soc_pci_read(unit,
                       CMIC_CMCx_SBUSDMA_CHy_STATUS(wait->cmc, wait->ch)));
        } else { /* Timeout cleanup */
            soc_timeout_t to;

            LOG_ERROR(BSL_LS_SOC_DMA,
                      (BSL_META_U(unit,
                                  "%s.%s %s timeout\n"),
                       SOC_MEM_UFNAME(unit, param->mem),
                       SOC_BLOCK_NAME(unit, param->copyno),
                       wait->intr_enb ? "interrupt" : "polling"));

            /* Abort DMA */
            ctrl = soc_pci_read(unit,
                                CMIC_CMCx_SBUSDMA_CHy_CONTROL(wait->cmc, wait->ch));
            soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &ctrl,
                              ABORTf, 1);
            LOG_ERROR(BSL_LS_SOC_DMA,
                      (BSL_META_U(unit,
                                  "CMIC_CMC0_SBUSDMA_CH0_CONTROLr = 0x%x\n"), ctrl));
            soc_pci_write(unit,
                          CMIC_CMCx_SBUSDMA_CHy_CONTROL(wait->cmc, wait->ch), ctrl);

            /* Check the done bit to confirm */
            soc_timeout_init(&to, wait->timeout, 0);
            while (1) {
                rval = soc_pci_read(unit,
                                    CMIC_CMCx_SBUSDMA_CHy_STATUS(wait->cmc, wait->ch));
                if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                    rval, DONEf)) {
                    break;
                }
                if (soc_timeout_check(&to)) {
                    LOG_ERROR(BSL_LS_SOC_DMA,
                              (BSL_META_U(unit,
                                          "_cmicx_sbusdma_reg_read: Abort Failed\n")));
                    break;
                }
            }
        }
    } else {
        if (wait->opcode == READ_MEMORY_CMD_MSG) {
            soc_cm_sinval(unit, (void *)param->buffer,
                  WORDS2BYTES(param->data_beats) * wait->count);
        }
    }
    /* Stop and free the channel if not reserved */
    cmicx_sbusdma_stop(unit, wait->cmc, wait->ch);

    if ((param->vchan == -1) && (do_not_free_channel == 0)) {
        cmicx_sbusdma_ch_put(unit, wait->cmc, wait->ch);
    }

    return rv;
}

/*******************************************
* @function _cmicx_sbusdma_reg_prog_init
* purpose Helper function to initialize data for DMA program
*
* @param unit [in] unit
* @param param [in] soc_sbusdma_reg_param_t pointer
* @param msg[out] schan_msg_t pointer
* @param prog[out] cmicx_sbusdma_prog_t pointer
* @param opcode [in] opcode.
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_cmicx_sbusdma_reg_prog_init(int unit, soc_sbusdma_reg_param_t  *param,
                           schan_msg_t   *msg,
                           cmicx_sbusdma_prog_t  *prog, int opcode)
{
    int           dst_blk, acc_type, dma;
    int           data_byte_len = 0;
    uint32 sbusdma_chan_max  = CMIC_CMCx_SBUSDMA_CHAN_MAX;

#ifdef BCM_TRIDENT3_SUPPORT
    uint32 ifa_cc_num_sbusdma = soc_property_get(unit,
                                                spn_IFA_CC_NUM_SBUS_DMA, 2);
    if ((SOC_IS_TRIDENT3(unit)) && (ifa_cc_num_sbusdma > 2)) {
        /* CMC_0 2 SBUS DMA Channels are used by IFA CC Eapp on M0 */
        sbusdma_chan_max = (CMIC_CMCx_SBUSDMA_CHAN_MAX - 2);
    }
#endif

    LOG_VERBOSE(BSL_LS_SOC_DMA,
                (BSL_META_U(unit,
                            "unit %d"
                            " mem %s[%u-%u].%s index %d-%d buffer %p\n"),
                            unit, SOC_MEM_UFNAME(unit, param->mem),
                            param->array_id_start, param->array_id_end,
                            SOC_BLOCK_NAME(unit, param->copyno),
                            param->index_begin, param->index_end, param->buffer));

    prog->data_beats = param->data_beats;

    prog->opcode = opcode;

    if (param->vchan == -1) {
        SOC_IF_ERROR_RETURN(cmicx_sbusdma_ch_get(unit, &prog->cmc, &prog->ch));
    } else {
       SOC_IF_ERROR_RETURN(cmicx_sbusdma_vchan_to_cmc_ch(unit,
                            param->vchan, &prog->cmc, &prog->ch));
    }

    LOG_VERBOSE(BSL_LS_SOC_DMA,
             (BSL_META_U(unit,
                         "unit %d"
                         "cmc = %d, channel =%d\n"),
                         unit, prog->cmc, prog->ch));

    assert((prog->ch >= 0) && (prog->ch < sbusdma_chan_max));

    if (opcode == WRITE_MEMORY_CMD_MSG) {
        if (param->index_begin > param->index_end) {
            prog->count = param->index_begin - param->index_end + 1;
            prog->rev_slam = 1;
        } else {
            prog->count = param->index_end - param->index_begin + 1;
        }
    } else {
        prog->count = param->index_end - param->index_begin + 1;
    }

    if (prog->count < 1) {
        return SOC_E_NONE;
    }

    schan_msg_clear(msg);
    acc_type = SOC_MEM_ACC_TYPE(unit, param->mem);
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (0 != (param->ser_flags & _SOC_SER_FLAG_MULTI_PIPE)) {
        if ((param->ser_flags & _SOC_SER_FLAG_ACC_TYPE_MASK) ||
            (soc_feature(unit, soc_feature_unique_acc_type_access))) {

            /* Override ACC_TYPE in opcode */
            acc_type = param->ser_flags & _SOC_SER_FLAG_ACC_TYPE_MASK;
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    dst_blk = SOC_BLOCK2SCH(unit, param->copyno);
    if (opcode == WRITE_MEMORY_CMD_MSG) {
        data_byte_len = prog->data_beats * sizeof(uint32);
    } else {
        data_byte_len = 4;
    }

    LOG_VERBOSE(BSL_LS_SOC_DMA,
                (BSL_META_U(unit,
                 "unit=%d %s.acc_type = %0x, idx[%0d-%0d]\n"),
                 unit, SOC_MEM_NAME(unit, param->mem), acc_type,
                 param->index_begin, param->index_end));
    /* DMA transaction indicator on SBUS */
    dma = 1;

    soc_schan_header_cmd_set(unit, &msg->header, opcode,
                             dst_blk, 0, acc_type, data_byte_len, dma, 0);

    return SOC_E_NONE;
}

/*******************************************
* @function _cmicx_sbusdma_reg_array_read_prog
* purpose : Program DMA for read
*
* @param unit [in] unit
* @param param [in] soc_sbusdma_reg_param_t pointer
* @param prog [in] pointer to void
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_cmicx_sbusdma_reg_array_read_prog(int unit,
             void *data, void *wparam)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    schan_msg_t   msg;
    cmicx_sbusdma_prog_t  prog;
    cmic_sbusdma_wait_t  *wait;
    uint8         at;
    soc_sbusdma_reg_param_t  *param;

    if ((data == NULL) || (wparam == NULL)) {
        return SOC_E_PARAM;
    }
    param = (soc_sbusdma_reg_param_t  *)data;

    wait = SBUSDMA_REG_WAIT(wparam);

    if (wait == NULL) {
        return SOC_E_PARAM;
    }

    if (param->copyno < 0) {
        return SOC_E_PARAM;
    }

    LOG_VERBOSE(BSL_LS_SOC_DMA,
                (BSL_META_U(unit,
                         "_cmicx_sbusdma_reg_read: unit %d"
                         " mem %s.%s[%u] index %d-%d"
                         "SER flags 0x%08x buffer %p, copyno = %d\n"),
                         unit, SOC_MEM_UFNAME(unit, param->mem),
                         SOC_BLOCK_NAME(unit, param->copyno),
                         param->array_id_start, param->index_begin,
                         param->index_end, param->ser_flags,
                         param->buffer, param->copyno));

    sal_memset(&prog, 0, sizeof(cmicx_sbusdma_prog_t));

    param->data_beats = soc_mem_entry_words(unit, param->mem);

    SOC_IF_ERROR_RETURN(_cmicx_sbusdma_reg_prog_init(unit, param, &msg,
                                      &prog, READ_MEMORY_CMD_MSG));

    prog.ser_flags = param->ser_flags;
    prog.start_addr = soc_mem_addr_get(unit, param->mem, param->array_id_start,
                                  param->copyno, param->index_begin, &at);

    _cmicx_sbusdma_reg_program(unit, &msg, param, &prog);

    LOG_VERBOSE(BSL_LS_SOC_DMA,
                (BSL_META_U(unit,
                         "_cmicx_sbusdma_reg_read: %d entries %d beats "
                         "addr 0x%x (index %d-%d) Interrupt-Mode(%d)"
                         "Rev Slam %d\n"),
                         prog.count, prog.data_beats, prog.start_addr,
                         param->index_begin, param->index_end,
                         soc->tableDmaIntrEnb, prog.rev_slam));

    wait->count =  prog.count;
    wait->ch = prog.ch;
    wait->cmc = prog.cmc;
    wait->intr = INTR_SBUSDMA(prog.cmc, prog.ch);
    wait->opcode = READ_MEMORY_CMD_MSG;

    return SOC_E_NONE;
}


/*******************************************
* @function _cmicx_sbusdma_reg_array_read
* purpose DMA acceleration for soc_mem_read_range() on FB/ER
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
_cmicx_sbusdma_reg_array_read(int unit, soc_sbusdma_reg_param_t  *param)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int           rv = SOC_E_NONE;
    cmic_sbusdma_wparam_t wparam;

    wparam.wait.intr_enb = soc->tableDmaIntrEnb;
    wparam.wait.timeout = soc->tableDmaTimeout;

    rv = _cmicx_sbusdma_reg_array_read_prog(unit, param, &wparam);
    SOC_IF_ERROR_RETURN(rv);

    rv = _cmicx_sbusdma_reg_complete(unit, param, &wparam, 0);

    return rv;
}

/*******************************************
* @function _cmicx_sbusdma_reg_array_write_prog
* purpose DMA acceleration for soc_mem_write_range() on FB/ER
*
* @param unit [in] unit
* @param param [in] soc_sbusdma_reg_param_t pointer
* @param prog [in] pointer to void
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_cmicx_sbusdma_reg_array_write_prog(int unit,
                 void  *data, void *wparam)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int           rv = SOC_E_NONE;
    schan_msg_t   msg;
    unsigned      array_index;
    cmicx_sbusdma_prog_t  prog;
    cmic_sbusdma_wait_t  *wait;
    uint8         at;
    soc_sbusdma_reg_param_t  *param;

    if ((data == NULL) || (wparam == NULL)) {
        return SOC_E_PARAM;
    }
    param = (soc_sbusdma_reg_param_t  *)data;

    if ((param->flags & (SOC_MEM_WRITE_COMMIT_ONLY | SOC_MEM_WRITE_STATUS_ONLY | SOC_MEM_WRITE_SET_ONLY)) != 0 && param->array_id_start < param->array_id_end) {
        LOG_ERROR(BSL_LS_SOC_DMA, (BSL_META_U(unit,
            "cannot perform partial write operations for multiple memory array indices\n")));
        return SOC_E_PARAM;
    }

    if ((param->flags & SOC_MEM_WRITE_COMMIT_ONLY) != 0 && param->vchan != -1) {
        SOC_IF_ERROR_RETURN(cmicx_sbusdma_vchan_to_cmc_ch(unit,
                                param->vchan, &prog.cmc, &prog.ch));
        _cmicx_sbusdma_start(unit, prog.cmc, prog.ch);
        return 0;
    }
    wait = SBUSDMA_REG_WAIT(wparam);

    if (wait == NULL) {
        return SOC_E_PARAM;
    }

    if (param->copyno < 0) {
        return SOC_E_PARAM;
    }

    LOG_VERBOSE(BSL_LS_SOC_DMA,
             (BSL_META_U(unit,
                         "_cmicx_sbusdma_reg_write: unit %d"
                         " mem %s[%u-%u].%s index %d-%d buffer %p\n"),
                         unit, SOC_MEM_UFNAME(unit, param->mem),
                         param->array_id_start, param->array_id_end,
                         SOC_BLOCK_NAME(unit, param->copyno),
                         param->index_begin, param->index_end, param->buffer));

    sal_memset(&prog, 0, sizeof(cmicx_sbusdma_prog_t));

    param->data_beats = soc_mem_entry_words(unit, param->mem);

    if ((param->flags & (SOC_MEM_WRITE_COMMIT_ONLY | SOC_MEM_WRITE_STATUS_ONLY)) == 0) { /* used by SOC_MEM_WRITE_SET_ONLY */
        SOC_IF_ERROR_RETURN(_cmicx_sbusdma_reg_prog_init(unit, param, &msg,
                            &prog, WRITE_MEMORY_CMD_MSG));
        prog.mem_clear = param->mem_clear;
    } else { /* calculate channel instead */
        if (param->vchan == -1) {
            LOG_ERROR(BSL_LS_SOC_DMA, (BSL_META_U(unit,
                "SOC_MEM_WRITE_STATUS_ONLY requires a channel to be specified\n")));
            return SOC_E_PARAM;
        } else {
            SOC_IF_ERROR_RETURN(cmicx_sbusdma_vchan_to_cmc_ch(unit,
                                param->vchan, &prog.cmc, &prog.ch));
        }
    }
    /* fill out the interrupt data */
    wait->ch = prog.ch;
    wait->cmc = prog.cmc;
    wait->intr = INTR_SBUSDMA(prog.cmc, prog.ch);
    wait->opcode = WRITE_MEMORY_CMD_MSG;

    for (array_index = param->array_id_start;
                           array_index <= param->array_id_end; ++array_index) {
        if ((param->flags & (SOC_MEM_WRITE_COMMIT_ONLY | SOC_MEM_WRITE_STATUS_ONLY)) == 0) { /* used by SOC_MEM_WRITE_SET_ONLY */
            prog.start_addr = soc_mem_addr_get(unit, param->mem, array_index,
                                          param->copyno, param->index_begin, &at);

            _cmicx_sbusdma_reg_program(unit, &msg, param, &prog);

            LOG_VERBOSE(BSL_LS_SOC_DMA,
                     (BSL_META_U(unit,
                                 "_cmicx_sbusdma_reg_write: %d entries %d beats "
                                 "addr 0x%x (index %d-%d) Interrupt-Mode(%d)\n"),
                                 prog.count, prog.data_beats, prog.start_addr,
                                 param->index_begin, param->index_end,
                                 soc->tslamDmaIntrEnb));

            if (param->mem_clear) {
                soc_cm_sflush(unit, param->buffer, WORDS2BYTES(prog.data_beats));
            } else {
                soc_cm_sflush(unit, param->buffer, WORDS2BYTES(prog.data_beats)
                              * prog.count);
            }
        }
        if ((param->flags & SOC_MEM_WRITE_SET_ONLY ) == 0) { /* used by SOC_MEM_WRITE_STATUS_ONLY */
            /* Wait for each index to finish, except the last one */
            if (array_index != param->array_id_end) {
                rv = _cmicx_sbusdma_reg_complete(unit, param, wparam, 1);
                if (SOC_FAILURE(rv) && param->vchan == -1) { /* Free the DMA channel in case of failure */
                    cmicx_sbusdma_ch_put(unit, wait->cmc, wait->ch);
                }
            }
        }
    } /* end of loop over array indices */

    return rv;


}

/*******************************************
* @function _cmicx_sbusdma_reg_array_write
* purpose Program DMA for write
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
_cmicx_sbusdma_reg_array_write(int unit, soc_sbusdma_reg_param_t  *param)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int rv = SOC_E_NONE;
    cmic_sbusdma_wparam_t wparam;

    wparam.wait.intr_enb = soc->tslamDmaIntrEnb;
    wparam.wait.timeout = soc->tslamDmaTimeout;

    rv = _cmicx_sbusdma_reg_array_write_prog(unit, param, &wparam);
    SOC_IF_ERROR_RETURN(rv);

    if ((param->flags & (SOC_MEM_WRITE_COMMIT_ONLY | SOC_MEM_WRITE_SET_ONLY)) == 0) { /* used by SOC_MEM_WRITE_STATUS_ONLY */
        rv = _cmicx_sbusdma_reg_complete(unit, param, &wparam, 0);
    }

    return rv;
}

/*******************************************
* @function _cmicx_sbusdma_reg_clear_specific
* purpose: Program DMA to clear specific memory/table
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
_cmicx_sbusdma_reg_clear_specific_prog(int unit,
             void  *data, void *wparam)
{
    int    rv = SOC_E_NONE;
    int    chunk_size, chunk_entries, mem_size, entry_words;
    int    index, blk, tmp, copyno;
    uint32 *buf;
    soc_sbusdma_reg_param_t  *param;

    param = (soc_sbusdma_reg_param_t  *)data;

    if (param == NULL) {
        return SOC_E_PARAM;
    }
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
            if (param->array_id_end >= maip->numels + maip->first_array_index) {
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
        rv = _cmicx_sbusdma_reg_array_write(unit, param);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_SOC_DMA,
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
* @function _cmicx_sbusdma_reg_clear_specific
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
_cmicx_sbusdma_reg_clear_specific(int unit, soc_sbusdma_reg_param_t  *param)
{
    int rv = SOC_E_NONE;

    rv = _cmicx_sbusdma_reg_clear_specific_prog(unit, param, NULL);

    return rv;
}

/*******************************************
* @function _cmicx_sbusdma_cmc_max_num_ch_get
* @Returns number of SBUSDMA channels in cmc
*
* @end
 */
STATIC uint32
_cmicx_sbusdma_cmc_max_num_ch_get(int unit)
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
    return sbusdma_chan_max;
}

/*******************************************
* @function cmicx_sbusdma_reg_init
* purpose API to Initialize cmicx Register DMA
*
* @param unit [in] unit
* @param drv [out] soc_sbusdma_desc_drv_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int cmicx_sbusdma_reg_init(int unit, soc_sbusdma_reg_drv_t *drv)
{
    /* Set ARB control register */
    soc_pci_write(unit, CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_OFFSET,
                  CMIC_TOP_SBUS_RING_ARB_CTRL_SET);

    drv->soc_mem_sbusdma_read  = _cmicx_sbusdma_reg_array_read;
    drv->soc_mem_sbusdma_write = _cmicx_sbusdma_reg_array_write;
    drv->soc_mem_sbusdma_clear = _cmicx_sbusdma_reg_clear_specific;
    drv->soc_sbusdma_read_prog = _cmicx_sbusdma_reg_array_read_prog;
    drv->soc_sbusdma_write_prog = _cmicx_sbusdma_reg_array_write_prog;
    drv->soc_sbusdma_clear_prog = _cmicx_sbusdma_reg_clear_specific_prog;
    drv->soc_sbusdma_reg_complete =  _cmicx_sbusdma_reg_complete;
    drv->soc_sbusdma_cmc_max_num_ch_get = _cmicx_sbusdma_cmc_max_num_ch_get;

    return SOC_E_NONE;
}

#endif /* BCM_SBUSDMA_SUPPORT */

