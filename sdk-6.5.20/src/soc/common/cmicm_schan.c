/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * CMICM S-Channel (internal command bus) support
 */

#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <sal/core/boot.h>
#include <sal/core/dpc.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/error.h>
#include <soc/cmic.h>
#include <soc/schanmsg_internal.h>

#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>

#define SOC_CHECK_MULTI_CMC(unit) \
         (soc_feature(unit, soc_feature_cmicm_multi_schan_cmc) && \
                (SOC_PCI_CMCS_NUM(unit) > 1))

typedef struct cmicm_schan_ch_s {
    sal_spinlock_t lock;
    int timeout;
    uint8 ch[CMIC_CMC_NUM_MAX+1];
} cmicm_schan_ch_t;


STATIC cmicm_schan_ch_t  _cmicm_schan_ch[SOC_MAX_NUM_DEVICES];
STATIC int _soc_schan_multi_cmc[SOC_MAX_NUM_DEVICES];


/*******************************************
* @function fschan_wait_idle
* @Purpose FIFO SCHAN wait function.
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
* @end
 */
int
fschan_wait_idle(int unit)
{
    int cmc = SOC_PCI_CMC(unit);
    soc_timeout_t to;

    soc_timeout_init(&to, SOC_CONTROL(unit)->schanTimeout, 100);
    do {
        if (0==(soc_pci_read(unit, CMIC_CMCx_FSCHAN_STATUS_OFFSET(cmc)) & FSCHAN_BUSY)) {
            return SOC_E_NONE;
        }
    } while (!soc_timeout_check(&to));
    return SOC_E_TIMEOUT;
}

/*******************************************
* @function _soc_cmicm_schan_reset
* @Purpose Resets the CMIC S-Channel interface.
*
* @param unit [in] unit
* @param cmc [in] cmc number 0-2 or 3 for common
* @param ch [in] channel number 0
*
* @returns None
* @comments This is required when we sent
* a message and did not receive a response after the poll count was
* exceeded.
* @end
 */
STATIC void
_soc_cmicm_schan_reset(int unit, int cmc, int ch)
{
    uint32 val, addr;

    COMPILER_REFERENCE(ch);

    addr = CMIC_COMMON_CMCx_SCHAN_CTRL_OFFSET(cmc);
    val = soc_pci_read(unit, addr);
    /* Toggle S-Channel abort bit in CMIC_SCHAN_CTRL register */
    soc_pci_write(unit, addr, val | SC_CMCx_SCHAN_ABORT);
    SDK_CONFIG_MEMORY_BARRIER;
    soc_pci_write(unit, addr, val);
    SDK_CONFIG_MEMORY_BARRIER;

    if (SAL_BOOT_QUICKTURN) {
        /* Give Quickturn at least 2 cycles */
        sal_usleep(10 * MILLISECOND_USEC);
    }
}


/*******************************************
* @function _soc_cmicm_schan_intr_wait
* purpose interrupt wait to complete SCHAN operation
*
* @param unit [in] unit
* @param cmc [in] cmc number 0-2 or 3 for common
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @comments  Called by schan_op_* functions.
* wait on a schan interrupt.  When schan interrupts are disabled,
* _soc_cmicm_schan_poll_wait is called instead.

* @end
 */
STATIC int
_soc_cmicm_schan_intr_wait(int unit, int cmc)
{
    uint32 errAddr;
    int rv = SOC_E_NONE;

    if (cmc != CMIC_CMC_NUM_MAX) {
        soc_cmicm_cmcx_intr0_enable(unit, cmc, IRQ_CMCx_SCH_OP_DONE);
    } else {
        /* this is for common SCHAN, this is available to all cmcs */
        soc_cmicm_cmcx_intr1_enable(unit, (cmc - 1),
                                        IRQ_CMCx_COMMON_SCHAN_DONE);
    }
    if (sal_sem_take(SOC_CONTROL(unit)->schanIntr[cmc],
                     SOC_CONTROL(unit)->schanTimeout) != 0) {
        rv = SOC_E_TIMEOUT;
    } else {
        LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "  Interrupt received\n")));

        if (SOC_CONTROL(unit)->schan_result[cmc] & SC_CMCx_MSG_NAK) {
            rv = SOC_E_FAIL;
        }
        if (soc_feature(unit, soc_feature_schan_hw_timeout)) {
            if (SOC_CONTROL(unit)->schan_result[cmc] &
                SC_CMCx_MSG_TIMEOUT_TST) {
                rv = SOC_E_TIMEOUT;
            }
        }

        if (soc_feature(unit, soc_feature_schan_err_check)) {
            uint32 schan_err, err_bit = 0;

            errAddr = CMIC_COMMON_CMCx_SCHAN_ERR_OFFSET(cmc);
            schan_err = soc_pci_read(unit, errAddr);
            err_bit = soc_reg_field_get(unit, CMIC_CMC0_SCHAN_ERRr, schan_err, ERRBITf);

            if (err_bit) {
                rv = SOC_E_FAIL;
                LOG_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit,
                    "  ERRBIT received in CMIC_SCHAN_ERR.\n")));
            }
        }
    }

    if (cmc != CMIC_CMC_NUM_MAX) {
        soc_cmicm_cmcx_intr0_disable(unit, cmc, IRQ_CMCx_SCH_OP_DONE);
    } else {
        /* this is for common SCHAN, this is available to all cmcs */
        soc_cmicm_cmcx_intr1_disable(unit, (cmc - 1),
                                      IRQ_CMCx_COMMON_SCHAN_DONE);
    }

    return rv;
}

/*******************************************
* @function _soc_cmicm_schan_poll_wait
* purpose POLL wait to complete cmicm SCHAN operation
*
* @param unit [in] unit
* @param cmc [in] cmc number 0-2 or 3 for common
* @param msg [in] schan_msg_t
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
* @comments  Called by schan_cmicm_schan_op function.
* In case interrupts are disabled, wait on a schan response via polling.
 */
STATIC int
_soc_cmicm_schan_poll_wait(int unit, int cmc, schan_msg_t *msg)
{
    uint32 schanCtrl, ctrlAddr, msgAddr, errAddr;
    int rv = SOC_E_NONE;
    soc_timeout_t to;

    soc_timeout_init(&to, SOC_CONTROL(unit)->schanTimeout, 100);
    ctrlAddr = CMIC_COMMON_CMCx_SCHAN_CTRL_OFFSET(cmc);

    while (((schanCtrl = soc_pci_read(unit, ctrlAddr)) & SC_CMCx_MSG_DONE) == 0) {
        if (soc_timeout_check(&to)) {
            return SOC_E_TIMEOUT;
        }
    }

    if (rv == SOC_E_NONE) {
        LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "  Done in %d polls\n"), to.polls));
    }

    if (schanCtrl & SC_CMCx_MSG_NAK) {
        schan_msg_t resp_msg;

        /* This is still the input opcode */
        msgAddr = CMIC_COMMON_CMCx_SCHAN_MESSAGEn(cmc, 1);
        resp_msg.dwords[1] = soc_pci_read(unit, msgAddr);

        rv = SOC_E_FAIL;
        LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "  NAK received from SCHAN.\n")));

        switch (msg->header.v2.opcode) {
            case TABLE_INSERT_CMD_MSG:
            case TABLE_DELETE_CMD_MSG:
            case TABLE_LOOKUP_CMD_MSG:
                if (resp_msg.genresp.response.type != SCHAN_GEN_RESP_TYPE_ERROR) {
                    /* Not a parity error */
                    break;
                }
                /* Else fallthru */
            case READ_MEMORY_CMD_MSG:
            case READ_REGISTER_CMD_MSG:
#if defined(BCM_XGS_SUPPORT)
            {
                int dst_blk, opcode;
                soc_schan_header_cmd_get(unit, &msg->header, &opcode, &dst_blk,
                    NULL, NULL, NULL, NULL, NULL);

                (void)soc_ser_mem_nack(INT_TO_PTR(unit),
                    INT_TO_PTR(msg->gencmd.address),
                    INT_TO_PTR(dst_blk),
                    opcode == READ_REGISTER_CMD_MSG ? INT_TO_PTR(1) : 0, 0);
            }
#endif
            default:
                break;
        }
    }
    if ((schanCtrl & SC_CMCx_MSG_SER_CHECK_FAIL) &&
        soc_feature(unit, soc_feature_ser_parity)){
        rv = SOC_E_FAIL;
        LOG_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit,
                  "  SER Parity Check Error.\n")));

#if defined(BCM_XGS_SUPPORT)
        sal_dpc(soc_ser_fail, INT_TO_PTR(unit),
                INT_TO_PTR(msg->gencmd.address), 0, 0, 0);
#endif
    }

    if (soc_feature(unit, soc_feature_schan_hw_timeout)) {
        if (schanCtrl & SC_CMCx_MSG_TIMEOUT_TST) {
            rv = SOC_E_TIMEOUT;
        }
    }

    if (soc_feature(unit, soc_feature_schan_err_check)) {
        uint32 schan_err, err_bit = 0;

        errAddr = CMIC_COMMON_CMCx_SCHAN_ERR_OFFSET(cmc);
        schan_err = soc_pci_read(unit, errAddr);
        err_bit = soc_reg_field_get(unit, CMIC_CMC0_SCHAN_ERRr, schan_err, ERRBITf);

        if (err_bit) {
            rv = SOC_E_FAIL;
            LOG_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit,
                      "  ERRBIT received in CMIC_SCHAN_ERR.\n")));
        }
    }
    soc_pci_write(unit, ctrlAddr, SC_CMCx_MSG_CLR);

    return rv;
}

/*******************************************
* @function _soc_cmicm_schan_op
* purpose cmicm SCHAN operation
*
* @param unit [in] unit
* @param msg [in] schan_msg_t
* @param dwc_write [in] Write messages
* @param dwc_write [in] read messages
* @param flags [in] use flags for interrupt
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
* @comments  This is the basic logic for schan
 */

STATIC int
_soc_cmicm_schan_op(int unit, schan_msg_t *msg, int dwc_write,
                   int dwc_read, uint32 flags)
{
    uint32 ctrlAddr, msgAddr;
    int i, rv;
    int cmc;

    if (soc_schan_op_sanity_check(unit, msg, dwc_write, dwc_read, &rv) == TRUE) {
        return rv;
    }

#if defined(BCM_SABER2_SUPPORT)
   if (!(SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM)) {
       if (SOC_IS_SABER2(unit)) {
        if (msg->header.v4.dst_blk == SOC_BLOCK_INFO(unit, OAMP_BLOCK(unit)).
                                      schan) {
           if (msg->header.v4.opcode == WRITE_REGISTER_CMD_MSG) {
               LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                           (BSL_META_U(unit,
                           "OAMP Write Block access OLD :%x \n"),
                           msg->writecmd.address));
               msg->writecmd.address =
               (msg->writecmd.address >> 8) & 0x000003ff;
               LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                           (BSL_META_U(unit,
                           "NEW :%x \n"), msg->writecmd.address));
           }
           if (msg->header.v4.opcode == READ_REGISTER_CMD_MSG) {
               LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                           (BSL_META_U(unit,
                           "OAMP READ Block access OLD:%x "),
                           msg->readcmd.address));
               msg->readcmd.address =
               (msg->readcmd.address >> 8) & 0x000003ff;
               LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                           (BSL_META_U(unit,
                           "New: %x \n"), msg->readcmd.address));
           }
        }
       }
   }
#endif /* BCM_SABER2_SUPPORT */

    if (_soc_schan_multi_cmc[unit]){
        SOC_IF_ERROR_RETURN(cmicm_schan_ch_get(unit, &cmc));
    } else {
        SCHAN_LOCK(unit);
        cmc = SOC_PCI_CMC(unit);
    }

    do {
        rv = SOC_E_NONE;

        /* Write raw S-Channel Data: dwc_write words */
        for (i = 0; i < dwc_write; i++) {
            msgAddr = CMIC_COMMON_CMCx_SCHAN_MESSAGEn(cmc, i);
            soc_pci_write(unit, msgAddr, msg->dwords[i]);
        }

        /* Tell CMIC to start */
        ctrlAddr = CMIC_COMMON_CMCx_SCHAN_CTRL_OFFSET(cmc);
        soc_pci_write(unit, ctrlAddr, SC_CMCx_MSG_START);

        /* Wait for completion using either the interrupt or polling method */
        if (((flags & SCHAN_OP_FLAG_ALLOW_INTERRUPTS) != 0) && SOC_CONTROL(unit)->schanIntrEnb) {
            rv = _soc_cmicm_schan_intr_wait(unit, cmc);
        } else {
            rv = _soc_cmicm_schan_poll_wait(unit, cmc, msg);
        }

        if (soc_schan_timeout_check(unit, &rv, msg, cmc, 0, flags) == TRUE) {
            break;
        }

        /* Read in data from S-Channel buffer space, if any */
        for (i = 0; i < dwc_read; i++) {
            msgAddr = CMIC_COMMON_CMCx_SCHAN_MESSAGEn(cmc, i);
            msg->dwords[i] = soc_pci_read(unit, msgAddr);
        }

        if (LOG_CHECK(BSL_LS_SOC_SCHAN | BSL_VERBOSE)) {
            soc_schan_dump(unit, msg, dwc_read);
        }

    } while (0);

    if (_soc_schan_multi_cmc[unit]) {
        cmicm_schan_ch_put(unit, cmc);
    } else {
        SCHAN_UNLOCK(unit);
    }

    if (rv == SOC_E_TIMEOUT) {
        if (LOG_CHECK(BSL_LS_SOC_SCHAN | BSL_ERROR)) {
            LOG_ERROR(BSL_LS_SOC_SCHAN,
                      (BSL_META_U(unit,
                        "SchanTimeOut:soc_schan_op operation timed out cmc=%d\n"), cmc));
            soc_schan_dump(unit, msg, dwc_write);
        }
    }

    return rv;
}

/*******************************************
* @function _cmicm_schan_ch_get
* purpose get idle schan on cmc
*
* @param unit [in] unit #
* @param *cmc [out] cmc number 0-2 or 3 for common
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
STATIC int
_cmicm_schan_ch_get(int unit, int *cmc)
{
    int rv = SOC_E_BUSY;
    int i;

    sal_spinlock_lock(_cmicm_schan_ch[unit].lock);
    for (i = 0; i <= CMIC_CMC_NUM_MAX; i++) {
        if (_cmicm_schan_ch[unit].ch[i] & 0x01) {
            rv = SOC_E_NONE;
            *cmc = i;
            _cmicm_schan_ch[unit].ch[i] &= ~(0x01);
            break;
        }
    }
    sal_spinlock_unlock(_cmicm_schan_ch[unit].lock);
    return rv;
}

/*******************************************
* @function cmicm_schan_ch_get
* purpose get idle schan on cmc
*
* @param unit [in] unit #
* @param cmc [out] cmc number 0-2 or 3 for common
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
int cmicm_schan_ch_get(int unit, int *cmc)
{
    int rv;

    soc_timeout_t to;

    soc_timeout_init(&to, _cmicm_schan_ch[unit].timeout, 100);

    do {
        rv = _cmicm_schan_ch_get(unit, cmc);
        if (rv == SOC_E_NONE)
            break;
    } while (!soc_timeout_check(&to));
    return rv;
}

/*******************************************
* @function cmicm_schan_ch_try_get
* purpose get idle channel on cmc
*
* @param unit [in] unit #
* @param cmc [in] cmc number 0-2 or 3 for common
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
int cmicm_schan_ch_try_get(int unit, int cmc)
{
    int rv = SOC_E_BUSY;

    soc_timeout_t to;

    soc_timeout_init(&to, _cmicm_schan_ch[unit].timeout, 100);

    sal_spinlock_lock(_cmicm_schan_ch[unit].lock);
    do {
        if (_cmicm_schan_ch[unit].ch[cmc] & 0x01) {
            rv = SOC_E_NONE;
            _cmicm_schan_ch[unit].ch[cmc] &= ~(0x01);
            break;
        }
    } while (!soc_timeout_check(&to));
    sal_spinlock_unlock(_cmicm_schan_ch[unit].lock);

    return rv;
}

/*******************************************
* @function cmicm_schan_ch_put
* purpose put back the freed channel on cmc
*
* @param unit [in] unit #
* @param cmc [in] cmc number 0-2 or 3 for common
*
* @returns SOC_E_PARAM
* @returns SOC_E_NONE
*
* @end
 */
int cmicm_schan_ch_put(int unit, int cmc)
{
   int rv = SOC_E_NONE;

   if ((cmc < 0) || (cmc > CMIC_CMC_NUM_MAX)) {
       rv = SOC_E_PARAM;
       return rv;
   }
   sal_spinlock_lock(_cmicm_schan_ch[unit].lock);
   _cmicm_schan_ch[unit].ch[cmc] |= 0x01;
   sal_spinlock_unlock(_cmicm_schan_ch[unit].lock);

   return rv;
}

/*******************************************
* @function cmicm_schan_ch_init
* purpose API to Initialize schan channel seletion mechanism
*
* @param unit [in] unit
* @param timeout [in] timeout
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int cmicm_schan_ch_init(int unit, int timeout)
{
   int rv = SOC_E_NONE;
   int i;

   _cmicm_schan_ch[unit].lock = sal_spinlock_create("schan Lock");

   if (_cmicm_schan_ch[unit].lock == NULL) {
        return SOC_E_MEMORY;
   }
   /* Availability of channels */
    for (i = 0; i <= CMIC_CMC_NUM_MAX; i++) {
       _cmicm_schan_ch[unit].ch[i] = 0x01;
    }
   return rv;

}

/*******************************************
* @function _soc_cmicm_schan_deinit
* purpose API to de-intialize cmicm SCHAN
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_soc_cmicm_schan_deinit(int unit)
{
    int cmc;

    for (cmc = 0; cmc <= CMIC_CMC_NUM_MAX; cmc++) {
       _soc_cmicm_schan_reset(unit, cmc, 0);
    }
    sal_spinlock_destroy(_cmicm_schan_ch[unit].lock);

    return SOC_E_NONE;
}

/*******************************************
* @function soc_schan_cmicm_init
* purpose API to Initialize cmicm SCHAN
*
* @param unit [in] unit
* @param drv [out] soc_cmic_schan_drv_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int soc_cmicm_schan_init(int unit, soc_cmic_schan_drv_t *drv)
{
    /* Cache multi CMC flag */
    sal_memset(_soc_schan_multi_cmc, 0, sizeof(_soc_schan_multi_cmc));
    if (SOC_CHECK_MULTI_CMC(unit)) {
        _soc_schan_multi_cmc[unit] = 1;
    }
    drv->soc_schan_deinit = _soc_cmicm_schan_deinit;
    drv->soc_schan_op = _soc_cmicm_schan_op;
    drv->soc_schan_reset = _soc_cmicm_schan_reset;

    SOC_IF_ERROR_RETURN(cmicm_schan_ch_init(unit,
                        CMIC_CMCx_SCHAN_CHSELECT_TIMEOUT));

    return SOC_E_NONE;
}

#endif /* BCM_CMICM_SUPPORT */
