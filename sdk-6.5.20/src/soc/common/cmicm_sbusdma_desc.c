/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: CMICM SBUS Descriptor based DMA Driver.
 */

#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/sbusdma_internal.h>

#if defined(BCM_CMICM_SUPPORT) && defined(BCM_SBUSDMA_SUPPORT)

#include <soc/cmicm.h>

#define SOC_SBUSDMA_FP_RETRIES 200

typedef struct { /* h/w desc structure */
    uint32 cntrl;    /* DMA control info */
    uint32 req;      /* DMA request info (refer h/w spec for details) */
    uint32 count;    /* DMA count */
    uint32 opcode;   /* Schan opcode (refer h/w spec for details) */
    uint32 addr;     /* Schan address */
    uint32 hostaddr; /* h/w mapped host address */
} soc_sbusdma_desc_t;


STATIC uint32 _cmicm_irq_sbusdma_ch[] = {
    IRQ_SBUSDMA_CH0_DONE,
    IRQ_SBUSDMA_CH1_DONE,
    IRQ_SBUSDMA_CH2_DONE
};

STATIC int _soc_desc_multi_cmc = 0;


/*******************************************
* @function _cmicm_sbusdma_desc_endian_config (private)
* purpose Configure endianess of sbus dma descriptor
*
* @param unit [in] unit
* @param desc [in] soc_sbusdma_desc_t pointer
*
* @returns None
* @end
 */
STATIC void
_cmicm_sbusdma_desc_endian_config(int unit, soc_sbusdma_desc_t *desc)
{
    int  big_pio, big_packet, big_other;

    soc_endian_get(unit, &big_pio, &big_packet, &big_other);

    if (big_other) {
       soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                               &(desc->req), HOSTMEMWR_ENDIANESSf, 1);
       soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                               &(desc->req), HOSTMEMRD_ENDIANESSf, 1);
    }
}

/*******************************************
* @function _cmicm_sbusdma_desc_prepare
* purpose API to prepare h/w descriptor
*
* @param unit [in] unit
* @param desc [in] soc_sbusdma_desc_t pointer
* @param cfg [in] soc_sbusdma_desc_cfg_t pointer
* @param count [in] descriptor count
* @param cfg_count [in] configuration count
* @param append [in] last of appended
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC
void _cmicm_sbusdma_desc_prepare(int unit, soc_sbusdma_desc_t *desc,
                                soc_sbusdma_desc_cfg_t *cfg, int count,
                                soc_sbusdma_desc_ctrl_t *ctrl, int append)
{
    sal_vaddr_t hptr = (sal_vaddr_t)(ctrl->buff);
    uint32 flags;
    schan_msg_t msg;
    int opcode, dst_blk, acc_type;
    int           data_byte_len = 0;

    sal_memset(desc, 0, sizeof(soc_sbusdma_desc_t));
    schan_msg_clear(&msg);
    flags = (ctrl->flags & SOC_SBUSDMA_CFG_USE_FLAGS) ? cfg->flags : ctrl->flags;


    if (flags & SOC_SBUSDMA_WRITE_CMD_MSG) {
        opcode = (flags & SOC_SBUSDMA_MEMORY_CMD_MSG) ?
            WRITE_MEMORY_CMD_MSG : WRITE_REGISTER_CMD_MSG;
        data_byte_len = cfg->width * sizeof(uint32);
    } else {
        opcode = (flags & SOC_SBUSDMA_MEMORY_CMD_MSG) ?
            READ_MEMORY_CMD_MSG : READ_REGISTER_CMD_MSG;
    }
    acc_type = cfg->acc_type;
    dst_blk = cfg->blk;

    soc_schan_header_cmd_set(unit, &msg.header, opcode, dst_blk, 0,
                             acc_type, data_byte_len, 0, 0);


    desc->count = cfg->count;
    desc->opcode = msg.dwords[0];
    desc->addr = cfg->addr;

    /* Prepare h/w desc */
    if ((ctrl->cfg_count == 1) || (count == ctrl->cfg_count - 1)) {
        desc->cntrl |= SOC_SBUSDMA_CTRL_LAST;
    }
    if (append && count) {
        desc->cntrl |= SOC_SBUSDMA_CTRL_APND;
    }

    if (ctrl->cfg_count == 1) {
        desc->hostaddr = soc_cm_l2p(unit, cfg->buff);
    } else {
       if (append) {
           desc->hostaddr = soc_cm_l2p(unit, ctrl->buff);
           ctrl->buff =  (void *)(hptr + (cfg->count * cfg->width));
       } else {
           desc->hostaddr = soc_cm_l2p(unit, cfg->buff);
       }
    }
    if ((opcode ==  WRITE_MEMORY_CMD_MSG) ||
        (opcode ==  WRITE_REGISTER_CMD_MSG)){
        soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                          &(desc->req), REQ_WORDSf, cfg->width);
        soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                          &(desc->req), REP_WORDSf, 0);
    } else {
        soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                          &(desc->req), REP_WORDSf, cfg->width);
        soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                          &(desc->req), REQ_WORDSf, 0);
        if (flags & SOC_SBUSDMA_CFG_MOR) {
            soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                              &(desc->req), IGNORE_SBUS_EARLYACKf, 1);
            soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                              &(desc->req), PEND_CLOCKSf, 30);
        }
    }

    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                      &(desc->req), INCR_SHIFTf, cfg->addr_shift);

    _cmicm_sbusdma_desc_endian_config(unit, desc);

    return;
}

/*******************************************
* @function _cmicm_sbusdma_desc_create
* purpose API to prepare h/w descriptor
*
* @param unit [in] unit
* @param desc [in] soc_sbusdma_desc_ctrl_t pointer
* @param cfg [in] soc_sbusdma_desc_cfg_t pointer
* @param alloc_memory [in] 1- allocate descriptor memory, 0 - use received descriptor pointer, not used in this function exist in function pointer prototype.
* @param desc [out] descriptor pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC
int _cmicm_sbusdma_desc_create(int unit, soc_sbusdma_desc_ctrl_t *ctrl,
                        soc_sbusdma_desc_cfg_t *cfg, uint8 alloc_memory, void **desc)
{
    soc_sbusdma_desc_t *dma_desc;
    uint8 append = 0;
    sal_vaddr_t hptr = (sal_vaddr_t)(ctrl->buff);
    uint32 count;

    if (ctrl->cfg_count == 1) { /* single mode */
         dma_desc = soc_cm_salloc(unit, sizeof(soc_sbusdma_desc_t),
                                      "soc_sbusdma_desc_t");
        if (dma_desc == NULL) {
            return SOC_E_MEMORY;
        }
        _cmicm_sbusdma_desc_prepare(unit, dma_desc, cfg, 0, ctrl, 0);
    } else {
        if (ctrl->buff) {
            append = 1;
        }
        dma_desc = soc_cm_salloc(unit, sizeof(soc_sbusdma_desc_t) *
                                      ctrl->cfg_count, "soc_sbusdma_desc_t");
        if (dma_desc == NULL) {
            return SOC_E_MEMORY;
        }
        for (count = 0; count < ctrl->cfg_count; count++) {
            _cmicm_sbusdma_desc_prepare(unit, &dma_desc[count], &cfg[count],
                                                         count, ctrl, append);
        }
        ctrl->buff = (void *)hptr;
    }
    *desc = dma_desc;

    return SOC_E_NONE;
}


/*******************************************
* @function _cmicm_sbusdma_intr_wait
* purpose API to wait Descriptor DMA complete
*
* @param unit [in] unit
* @param cmc [in] cmc
* @param ch [in] channel
* @param interval [in] timeout
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_cmicm_sbusdma_intr_wait(int unit, int cmc, int ch, int timeout)
{
    int rv = SOC_E_TIMEOUT;
    int rval;

    soc_cmicm_cmcx_intr0_enable(unit, cmc, _cmicm_irq_sbusdma_ch[ch]);
    if (sal_sem_take(SOC_CONTROL(unit)->sbusDmaIntrs[cmc][ch], timeout) < 0) {
        rv = SOC_E_TIMEOUT;
    }
    soc_cmicm_cmcx_intr0_disable(unit, cmc, _cmicm_irq_sbusdma_ch[ch]);

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

    return rv;
}

/*******************************************
* @function _cmicm_sbusdma_poll_wait
* purpose API to wait Descriptor using Polling mode
*
* @param unit [in] unit
* @param cmc [in] cmc
* @param ch [in] channel
* @param interval [in] timeout
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_cmicm_sbusdma_poll_wait(int unit, int cmc, int ch, int timeout)
{
   soc_timeout_t to;
   int rv = SOC_E_TIMEOUT;
   int rval;

   soc_timeout_init(&to, timeout, 0);

   do {
       rval = soc_pci_read(unit,
                           CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc, ch));
       
       if (SAL_BOOT_PLISIM && !SAL_BOOT_BCMSIM) {
           rv = SOC_E_NONE;
           break;
       }
       if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                             rval, DONEf)) {
           rv = SOC_E_NONE;
           if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                                 rval, ERRORf)) {
               rv = SOC_E_FAIL;
               cmicm_sbusdma_curr_op_details(unit, cmc, ch);
           }
           break;
       }
   } while(!(soc_timeout_check(&to)));

   return rv;

}

/*******************************************
* @function _cmicm_sbusdma_wait
* purpose API to wait Descriptor DMA complete
*
* @param unit [in] unit
* @param cmc [in] cmc
* @param ch [in] channel
* @param interval [in] time interval
* @param intrEnb [in] interrupt enable
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_cmicm_sbusdma_wait(int unit, int cmc, int ch, int interval, uint8 intrEnb)
{
    int rv = SOC_E_TIMEOUT;
    int rval;
    uint32 ctrl;

    if (intrEnb) {
        rv = _cmicm_sbusdma_intr_wait(unit, cmc, ch, interval);
    } else {
        rv = _cmicm_sbusdma_poll_wait(unit, cmc, ch, interval);
    }

    if (rv != SOC_E_NONE) {
        if (rv != SOC_E_TIMEOUT) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "failed(ERR)\n")));
            cmicm_sbusdma_error_details(unit,
                soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc, ch)));
        } else { /* Timeout cleanup */
            soc_timeout_t to;

            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s timeout\n"),
                       intrEnb ?
                       "interrupt" : "polling"));

            /* Abort DMA */
            ctrl = soc_pci_read(unit,
                                CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch));
            soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr,
                                &ctrl, ABORTf, 1);
            soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch),
                                ctrl);

            /* Check the done bit to confirm */
            soc_timeout_init(&to, interval, 0);
            while (1) {
                rval = soc_pci_read(unit,
                                    CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc, ch));
                if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr,
                    rval, DONEf)) {
                    break;
                }
                if (soc_timeout_check(&to)) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "_cmicm_sbusdma_desc: Abort Failed\n")));
                    break;
                }
            }
        }
    } /* rv != SOC_E_NONE */

    if (_soc_desc_multi_cmc) {
        cmicm_sbusdma_ch_put(unit, cmc, ch);
    }

    return rv;
}

/*******************************************
* @function _cmicm_sbusdma_program
* purpose API to program Descriptor DMA
*
* @param unit [in] unit
* @param cmc [out] cmc
* @param swd [out] _soc_sbusdma_state_t pointer
* @param ch [out] channel
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_cmicm_sbusdma_program(int unit, int *cmc, _soc_sbusdma_state_t *swd, int *ch)
{
    uint32 ctrl;
    int i;
    soc_sbusdma_desc_t *desc = swd->desc;

    /*   multicmc */
    if (_soc_desc_multi_cmc) {
             SOC_IF_ERROR_RETURN(cmicm_sbusdma_ch_get(unit, cmc, ch));
             assert((*ch >= 0) && (*ch < CMIC_CMCx_SBUSDMA_CHAN_MAX));
    }

    ctrl = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(*cmc, *ch));

    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &ctrl,
                      MODEf, 1);
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &ctrl,
                      ABORTf, 0);
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &ctrl,
                      STARTf, 0);
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(*cmc, *ch), ctrl);

    /* write desc address */
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_DESCADDR(*cmc, *ch),
                  soc_cm_l2p(unit, desc));

    /* Debug stuff */
    LOG_VERBOSE(BSL_LS_SOC_DMA,
                (BSL_META_U(unit,
                            "Count: %d, cmc = %d, ch = %d\n"),
                            swd->ctrl.cfg_count, *cmc, *ch));
    for (i = 0; i < swd->ctrl.cfg_count; i++) {
        LOG_VERBOSE(BSL_LS_SOC_DMA,
                    (BSL_META_U(unit,
                                "cntrl: %08x, req: %08x, count: %08x, "
                                "opcode: %08x, saddr: %08x, haddr: %08x\n"),
                                desc[i].cntrl, desc[i].req,
                                desc[i].count, desc[i].opcode,
                                desc[i].addr, desc[i].hostaddr));
    }

    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &ctrl,
                      STARTf, 1);
    /* Start DMA */
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(*cmc, *ch), ctrl);

    return SOC_E_NONE;
}


/*******************************************
* @function cmicm_sbusdma_desc_init
* purpose API to Initialize cmicm Descriptor DMA
*
* @param unit [in] unit
* @param drv [out] soc_sbusdma_desc_drv_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int cmicm_sbusdma_desc_init(int unit,
                            soc_sbusdma_desc_drv_t *drv)
{
    /* Cache multi CMC flag */
    if (SOC_CHECK_MULTI_CMC(unit)) {
        _soc_desc_multi_cmc = 1;
    }
    drv->soc_sbusdma_desc_detach = NULL;
    drv->soc_sbusdma_program = _cmicm_sbusdma_program;
    drv->soc_sbusdma_desc_create = _cmicm_sbusdma_desc_create;
    drv->soc_sbusdma_wait = _cmicm_sbusdma_wait;

    return SOC_E_NONE;
}


#endif
