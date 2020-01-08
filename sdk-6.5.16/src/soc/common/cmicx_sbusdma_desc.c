/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * Purpose: CMICX SBUS Descriptor based DMA Driver.
 */

#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/sbusdma_internal.h>

#if defined(BCM_CMICX_SUPPORT) && defined(BCM_SBUSDMA_SUPPORT)

#include <soc/cmicx.h>
#include <soc/intr_cmicx.h>

#define SOC_SBUSDMA_FP_RETRIES 200

typedef struct { /* h/w desc structure */
    uint32 cntrl;    /* DMA control info */
    uint32 req;      /* DMA request info (refer h/w spec for details) */
    uint32 count;    /* DMA count */
    uint32 opcode;   /* Schan opcode (refer h/w spec for details) */
    uint32 addr;     /* Schan address */
    uint32 hostaddrLo; /* h/w mapped host address Lo*/
    uint32 hostaddrHi; /* h/w mapped host address Hi*/
    uint32 rsrv;
} soc_sbusdma_desc_t;



/*******************************************
* @function _cmicx_sbusdma_desc_endian_config (private)
* purpose Configure endianess of sbus dma descriptor
*
* @param unit [in] unit
* @param desc [in] soc_sbusdma_desc_t pointer
*
* @returns None
* @end
 */
STATIC void
_cmicx_sbusdma_desc_endian_config(int unit, soc_sbusdma_desc_t *desc)
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
* @function _cmicx_sbusdma_desc_prepare
* purpose API to prepare h/w descriptor
*
* @param unit [in] unit
* @param desc [in] soc_sbusdma_desc_t pointer
* @param cfg [in] soc_sbusdma_desc_cfg_t pointer
* @param count [in] descriptor count
* @param ctrl [in] soc_sbusdma_desc_ctrl_t pointer
* @param append [in] last of appended
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC
void _cmicx_sbusdma_desc_prepare(int unit, soc_sbusdma_desc_t *desc,
                                soc_sbusdma_desc_cfg_t *cfg, int count,
                                soc_sbusdma_desc_ctrl_t *ctrl, int append)
{
    sal_vaddr_t hptr = (sal_vaddr_t)(ctrl->buff);
    schan_msg_t msg;
    int opcode, dst_blk, acc_type, dlen;
    uint32 flags;
    uint32 address = 0;
    uint32 pend_clock_val = 0;
    soc_mem_t mem;
    sal_memset(desc, 0, sizeof(soc_sbusdma_desc_t));
    schan_msg_clear(&msg);
    flags = (ctrl->flags & SOC_SBUSDMA_CFG_USE_FLAGS) ? \
                 cfg->flags : ctrl->flags;

    if (flags & SOC_SBUSDMA_WRITE_CMD_MSG) {
        opcode = (flags & SOC_SBUSDMA_MEMORY_CMD_MSG) ?
            WRITE_MEMORY_CMD_MSG : WRITE_REGISTER_CMD_MSG;
        dlen = (flags & SOC_SBUSDMA_MEMORY_CMD_MSG) ?
            (sizeof(uint32) * cfg->width) : sizeof(uint32);
    } else {
        opcode = (flags & SOC_SBUSDMA_MEMORY_CMD_MSG) ?
            READ_MEMORY_CMD_MSG : READ_REGISTER_CMD_MSG;
        dlen = 0;
    }
    acc_type = cfg->acc_type;
    dst_blk = cfg->blk;
    soc_schan_header_cmd_set(unit, &msg.header, opcode, dst_blk, 0,
                             acc_type, dlen, 0, 0);


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
    /* Set the descriptor remaining in prefetch is enabled */
    if (flags & SOC_SBUSDMA_CFG_PREFETCH_ENB) {
        desc->cntrl |= ((ctrl->cfg_count - 1 - count) <<
                            SOC_SBUSDMA_CTRL_DSEC_REMAIN_SHIFT);
    }

    if (ctrl->cfg_count == 1) {
        desc->hostaddrLo = PTR_TO_INT(soc_cm_l2p(unit, cfg->buff));
        desc->hostaddrHi = PTR_HI_TO_INT(soc_cm_l2p(unit, cfg->buff));
        if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) {
            desc->hostaddrHi |= CMIC_PCIE_SO_OFFSET;
        }
    } else {
        if (append) {
            desc->hostaddrLo = PTR_TO_INT(soc_cm_l2p(unit, ctrl->buff));
            desc->hostaddrHi = PTR_HI_TO_INT(soc_cm_l2p(unit, ctrl->buff));
            if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) {
                desc->hostaddrHi |= CMIC_PCIE_SO_OFFSET;
            }
            ctrl->buff =  (void *)(hptr + (cfg->count * cfg->width));
        } else {
            desc->hostaddrLo = PTR_TO_INT(soc_cm_l2p(unit, cfg->buff));
            desc->hostaddrHi = PTR_HI_TO_INT(soc_cm_l2p(unit, cfg->buff));
            if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) {
                desc->hostaddrHi |= CMIC_PCIE_SO_OFFSET;
            }
        }
    }
    address = desc->addr;

    mem = soc_addr_to_mem_extended(unit, -1, -1, address);

    if ((opcode ==  WRITE_MEMORY_CMD_MSG) ||
        (opcode ==  WRITE_REGISTER_CMD_MSG)){
        soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                          &(desc->req), REQ_WORDSf, cfg->width);
        soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                          &(desc->req), REP_WORDSf, 0);
        if (cfg->mem_clear) {
            soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &(desc->req),
                              REQ_SINGLEf, 1);
        } else {
            soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &(desc->req),
                              REQ_SINGLEf, 0);
        }
    } else {
        soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                          &(desc->req), REQ_WORDSf, 0);
        soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                          &(desc->req), REP_WORDSf, cfg->width);

        if (flags & SOC_SBUSDMA_CFG_MOR) {
            if (mem > 0) {
                if (SOC_MEM_INFO(unit, mem).cmic_mor_clks) {
                    pend_clock_val = SOC_MEM_INFO(unit, mem).cmic_mor_clks;
                }
            }
            soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                              &(desc->req), IGNORE_SBUS_EARLYACKf, 1);
            if (pend_clock_val != 0) {
                soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                                  &(desc->req), PEND_CLOCKSf, pend_clock_val);
            } else {
                soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                                  &(desc->req), PEND_CLOCKSf, 30);
            }
        }
    }
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                      &(desc->req), INCR_SHIFTf, cfg->addr_shift);

    _cmicx_sbusdma_desc_endian_config(unit, desc);

    return;
}

/*******************************************
* @function _cmicx_sbusdma_desc_create
* purpose API to prepare h/w descriptor
*
* @param unit [in] unit
* @param desc [in] soc_sbusdma_desc_ctrl_t pointer
* @param cfg [in] soc_sbusdma_desc_cfg_t pointer
* @param desc [out] descriptor pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC
int _cmicx_sbusdma_desc_create(int unit, soc_sbusdma_desc_ctrl_t *ctrl,
                        soc_sbusdma_desc_cfg_t *cfg, void **desc)
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
        _cmicx_sbusdma_desc_prepare(unit, dma_desc, cfg, 0, ctrl, 0);
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
            _cmicx_sbusdma_desc_prepare(unit, &dma_desc[count], &cfg[count],
                                                         count, ctrl, append);
        }
        ctrl->buff = (void *)hptr;
    }
    *desc = dma_desc;

    return SOC_E_NONE;
}

/*******************************************
* @function _cmicx_sbusdma_wait
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
_cmicx_sbusdma_wait(int unit, int cmc, int ch, int interval, uint8 intrEnb)
{
    int rv = SOC_E_TIMEOUT;
    int rval;
    uint32 ctrl;

    if (intrEnb) {
        rv = cmicx_sbusdma_intr_wait(unit, cmc, ch, interval, 0);
        if (rv != SOC_E_TIMEOUT) {
            SOC_CONTROL(unit)->stat.intr_desc++;
        }
    } else {
        rv = cmicx_sbusdma_poll_wait(unit, cmc, ch, interval, 0);
    }

    if (rv != SOC_E_NONE) {
        if (rv != SOC_E_TIMEOUT) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "failed(ERR)\n")));
            cmicx_sbusdma_error_details(unit,
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
                                          "_cmicx_sbusdma_desc: Abort Failed\n")));
                    break;
                }
            }
        }
    } /* rv != SOC_E_NONE */

    /* Stop and free the channel */
    cmicx_sbusdma_stop(unit, cmc, ch);

    cmicx_sbusdma_ch_put(unit, cmc, ch);

    return rv;
}

/*******************************************
* @function _cmicx_sbusdma_program
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
_cmicx_sbusdma_program(int unit, int *cmc, _soc_sbusdma_state_t *swd, int *ch)
{
    uint32 ctrl;
    int i;
    soc_sbusdma_desc_t *desc = (soc_sbusdma_desc_t *)swd->desc;

    SOC_IF_ERROR_RETURN(cmicx_sbusdma_ch_get(unit, cmc, ch));

    LOG_VERBOSE(BSL_LS_SOC_DMA,
                (BSL_META_U(unit,
                            "cmc = %d, channel = %d\n"),
                            *cmc, *ch));

    assert((*ch >= 0) && (*ch < CMIC_CMCx_SBUSDMA_CHAN_MAX));
    ctrl = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(*cmc, *ch));

    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &ctrl,
                      MODEf, 1);

    if (swd->ctrl.flags & SOC_SBUSDMA_CFG_PREFETCH_ENB) {
        soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &ctrl,
                      DESC_PREFETCH_ENABLEf, 1);
    }

    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(*cmc, *ch), ctrl);

    /* write desc Lo address */
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_DESCADDR_LO(*cmc, *ch),
                   PTR_TO_INT(soc_cm_l2p(unit, swd->desc)));
    /* write desc Hi address */
    if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) {
        soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_DESCADDR_HI(*cmc, *ch),
                  (PTR_HI_TO_INT(soc_cm_l2p(unit, swd->desc)) |
                   CMIC_PCIE_SO_OFFSET));
    } else {
        soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_DESCADDR_HI(*cmc, *ch),
                  PTR_HI_TO_INT(soc_cm_l2p(unit, swd->desc)));
    }
    /* Debug stuff */
    LOG_VERBOSE(BSL_LS_SOC_DMA,
                (BSL_META_U(unit,
                            "desc = %x, Count: %d\n"),
                            PTR_TO_INT(soc_cm_l2p(unit, swd->desc)),
                            swd->ctrl.cfg_count));
    for (i = 0; i < swd->ctrl.cfg_count; i++) {
        LOG_VERBOSE(BSL_LS_SOC_DMA,
                    (BSL_META_U(unit,
                                "cntrl: %08x, req: %08x, count: %08x, "
                                "opcode: %08x, saddr: %08x, haddr: %08x\n"),
                                desc[i].cntrl, desc[i].req,
                                desc[i].count, desc[i].opcode,
                                desc[i].addr, desc[i].hostaddrLo));
    }

    return SOC_E_NONE;
}

/*******************************************
* @function cmicx_sbusdma_desc_init
* purpose API to Initialize cmicx Descriptor DMA
*
* @param unit [in] unit
* @param drv [out] soc_sbusdma_desc_drv_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int cmicx_sbusdma_desc_init(int unit,
                            soc_sbusdma_desc_drv_t *drv)
{
    int cmc, ch;
    uint32 val;
    int big_pio, big_packet, big_other;

    /* configure descriptor endianess */
    soc_endian_get(unit, &big_pio, &big_packet, &big_other);

    if (big_other) {
        for (cmc = 0; cmc < CMIC_CMC_NUM_MAX; cmc++) {
            for (ch = 0 ; ch < CMIC_CMCx_SBUSDMA_CHAN_MAX; ch++) {
                val = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch));
                soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr,
                                     &val, DESCRIPTOR_ENDIANESSf, 1);
                soc_pci_write(unit,  CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch), val);
            }
        }
    }
    drv->soc_sbusdma_desc_detach = NULL;
    drv->soc_sbusdma_program = _cmicx_sbusdma_program;
    drv->soc_sbusdma_desc_create = _cmicx_sbusdma_desc_create;
    drv->soc_sbusdma_wait = _cmicx_sbusdma_wait;

    return SOC_E_NONE;
}


#endif
