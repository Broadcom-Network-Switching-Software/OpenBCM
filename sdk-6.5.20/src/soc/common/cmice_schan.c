/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *  CMICE S-Channel (internal command bus) support
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


#ifdef BCM_TRIUMPH2_SUPPORT
#include <soc/triumph2.h>
#endif
#ifdef BCM_TRIDENT_SUPPORT
#include <soc/trident.h>
#endif

#include <soc/cmicm.h>
/*
 * Utilities related to soc_pci_read/soc_pci_write (See documentation there)
 * Specifically added since implementation of these as macros involved
 * braced-groups within expressions which is forbidden by ISO C.
 */
/* { */
uint32
soc_pci_read_helper(int unit, uint32 addr)
{
    uint32 ret ;
    if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit))
    {
        assert(addr < 0x50000 || ((addr<7*1024*1024+128*1024) && (addr >= 7*1024*1024)));
    }
    ret = CMREAD(unit, addr);
    return (ret);
}
uint32
soc_pci_write_helper(int unit, uint32 addr, uint32 data)
{
    uint32 ret ;
    if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit))
    {
        assert(addr < 0x50000 || ((addr<7*1024*1024+128*1024) && (addr >= 7*1024*1024)));
    }
    ret = CMWRITE(unit, addr, data);
    return (ret);
}
/* } */

/*******************************************
* @function _soc_cmice_schan_reset
* @Purpose Resets the CMICE S-Channel interface.
*
* @param unit [in] unit
*
* @returns None
* @comments This is required when we sent
* a message and did not receive a response after the poll count was
* exceeded.
* @end
 */
STATIC void
_soc_cmice_schan_reset(int unit, int cmc, int ch)
{
    uint32 val;

    val = soc_pci_read(unit, CMIC_CONFIG);

    /* Toggle S-Channel abort bit in CMIC_CONFIG register */
    soc_pci_write(unit, CMIC_CONFIG, val | CC_SCHAN_ABORT);

    SDK_CONFIG_MEMORY_BARRIER;

    soc_pci_write(unit, CMIC_CONFIG, val);

    SDK_CONFIG_MEMORY_BARRIER;

    if (SAL_BOOT_QUICKTURN) {
        /* Give Quickturn at least 2 cycles */
        sal_usleep(10 * MILLISECOND_USEC);
    }
}

/*******************************************
* @function _soc_cmice_schan_dpp_err_check
* purpose dpp error check
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @comments   On DPP and DFE devices (without CMIC-M)
* but with the schan_err_check feature,
* exercise the feature and check for schan errors.

* @end
 */
STATIC int
_soc_cmice_schan_dpp_err_check(int unit)
{
    int rv = SOC_E_NONE;

#if defined(BCM_SAND_SUPPORT)
    if(soc_feature(unit, soc_feature_schan_err_check)) {
        uint32 schan_err, err_bit = 0;

        schan_err = soc_pci_read(unit, CMIC_SCHAN_ERR);
        err_bit = soc_reg_field_get(unit, CMIC_SCHAN_ERRr, schan_err, ERRBITf);

        if (err_bit) {
            soc_pci_write(unit, CMIC_SCHAN_ERR, 0);
            rv = SOC_E_FAIL;
            LOG_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit,
                      "  ERRBIT received in CMIC_SCHAN_ERR.\n")));
        }
    }
#endif /* BCM_SAND_SUPPORT */

    return rv;
}

/*******************************************
* @function _soc_cmice_schan_intr_wait
* purpose interrupt wait to complete SCHAN operation
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @comments  Called by schan_op_* functions.
* wait on a schan interrupt.  When schan interrupts are disabled,
* _soc_cmice_schan_poll_wait is called instead.

* @end
 */
STATIC int
_soc_cmice_schan_intr_wait(int unit)
{
    int rv = SOC_E_NONE;

    soc_intr_enable(unit, IRQ_SCH_MSG_DONE);

    if (sal_sem_take(SOC_CONTROL(unit)->schanIntr[SOC_PCI_CMC(unit)],
                      SOC_CONTROL(unit)->schanTimeout) != 0) {
        rv = SOC_E_TIMEOUT;
    } else {
        LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "  Interrupt received\n")));

        if (SOC_CONTROL(unit)->schan_result[SOC_PCI_CMC(unit)] & SC_MSG_NAK_TST) {
            rv = SOC_E_FAIL;
        }
        if (soc_feature(unit, soc_feature_schan_hw_timeout)) {
            if (SOC_CONTROL(unit)->schan_result[SOC_PCI_CMC(unit)] & SC_MSG_TIMEOUT_TST) {
                rv = SOC_E_TIMEOUT;
            }
        }

        SOC_IF_ERROR_RETURN(_soc_cmice_schan_dpp_err_check(unit));
    }

    soc_intr_disable(unit, IRQ_SCH_MSG_DONE);

    return rv;
}

/*******************************************
* @function _soc_cmice_schan_tr2_check_ser_nack
* purpose check for a ser and nack.
*
* @param unit [in] unit
* @param msg [in] schan_msg_t
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @comments  On Triumph2 devices, enable special processing in
* response to schan NACK responses.  For table insert/lookup/delete,
* if a parity error appears, send a NACK.  For memory/register reads,
* send a NACK regardless.

* @end
 */

STATIC int
_soc_cmice_schan_tr2_check_ser_nack(int unit, schan_msg_t *msg)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    schan_msg_t resp_msg;

    /* This is still the input opcode */
    resp_msg.dwords[1] =
        soc_pci_read(unit, CMIC_SCHAN_MESSAGE(unit, 1));

    switch (msg->header.v2.opcode) {
        case TABLE_INSERT_CMD_MSG:
        case TABLE_DELETE_CMD_MSG:
        case TABLE_LOOKUP_CMD_MSG:
            if (SCHAN_GEN_RESP_TYPE_ERROR !=
                resp_msg.genresp.response.type) {
                /* Not a parity error */
                break;
            }
            /* Else fallthru */
        case READ_MEMORY_CMD_MSG:
        case READ_REGISTER_CMD_MSG:
            {
                int dst_blk, opcode;
                soc_schan_header_cmd_get(unit, &msg->header, &opcode, &dst_blk,
                    NULL, NULL, NULL, NULL, NULL);

                (void)soc_ser_mem_nack(INT_TO_PTR(unit),
                    INT_TO_PTR(msg->gencmd.address),
                    INT_TO_PTR(dst_blk),
                    opcode == READ_REGISTER_CMD_MSG ? INT_TO_PTR(1) : 0, 0);
            }
            break;
        default:
            break;
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

    return SOC_E_NONE;
}

/*******************************************
* @function _soc_cmice_schan_check_ser_parity
* purpose check for a ser parity error.
*
* @param unit [in] unit
* @param schanCtrl [in] schan control
* @param msg [in] schan_msg_t
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_soc_cmice_schan_check_ser_parity(int unit, uint32 *schanCtrl, schan_msg_t *msg)
{
    int rv = SOC_E_NONE;

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if ((*schanCtrl & SC_MSG_SER_CHECK_FAIL_TST) &&
        soc_feature(unit, soc_feature_ser_parity)) {
            rv = SOC_E_FAIL;
            LOG_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit,
                      "  SER Parity Check Error.\n")));
            sal_dpc(soc_ser_fail, INT_TO_PTR(unit),
                    INT_TO_PTR(msg->gencmd.address), 0, 0, 0);
    }
#endif /* BCM_TRIUMPH2_SUPPORT, BCM_TRIDENT_SUPPORT */

    return rv;
}


/*******************************************
* @function _soc_cmice_schan_poll_wait
* purpose POLL wait to complete cmice SCHAN operation
*
* @param unit [in] unit
* @param msg [in] schan_msg_t
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
* @comments  Called by schan_cmice_schan_op function.
* In case interrupts are disabled, wait on a schan response via polling.
 */
STATIC int
_soc_cmice_schan_poll_wait(int unit, schan_msg_t *msg)
{
    int rv = SOC_E_NONE;
    soc_timeout_t to;
    uint32 schanCtrl;

    soc_timeout_init(&to,  SOC_CONTROL(unit)->schanTimeout, 100);

    while (((schanCtrl = soc_pci_read(unit, CMIC_SCHAN_CTRL)) &
            SC_MSG_DONE_TST) == 0) {
        if (soc_timeout_check(&to)) {
            rv = SOC_E_TIMEOUT;
            break;
        }
    }

    if (rv == SOC_E_NONE) {
        LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "  Done in %d polls\n"), to.polls));
    }

    if (schanCtrl & SC_MSG_NAK_TST) {
        rv = SOC_E_FAIL;
#ifdef _SER_TIME_STAMP
        ser_time_1 = sal_time_usecs();
#endif
        LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "  NAK received from SCHAN.\n")));

        SOC_IF_ERROR_RETURN(_soc_cmice_schan_tr2_check_ser_nack(unit, msg));
    }

    SOC_IF_ERROR_RETURN(_soc_cmice_schan_check_ser_parity(unit, &schanCtrl, msg));

    if (soc_feature(unit, soc_feature_schan_hw_timeout)) {
        if (schanCtrl & SC_MSG_TIMEOUT_TST) {
            rv = SOC_E_TIMEOUT;
        }
    }

    SOC_IF_ERROR_RETURN(_soc_cmice_schan_dpp_err_check(unit));
    soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_MSG_DONE_CLR);

    return rv;
}

/*******************************************
* @function _soc_cmice_schan_op
* purpose cmice SCHAN operation
*
* @param unit [in] unit
* @param msg [in] schan_msg_t
* @param dwc_write [in] Write messages
* @param dwc_write [in] read messages
* @param intr [in] use interrupt
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
* @comments  this is the basic logic for schan
 */
STATIC int
_soc_cmice_schan_op(int unit, schan_msg_t *msg, int dwc_write,
                      int dwc_read, uint32 flags)
{
    int i, rv;

    if (soc_schan_op_sanity_check(unit, msg, dwc_write, dwc_read, &rv) == TRUE) {
        return rv;
    }

    SCHAN_LOCK(unit);

    do {
        rv = SOC_E_NONE;

        /* Write raw S-Channel Data: dwc_write words */
        for (i = 0; i < dwc_write; i++) {
            soc_pci_write(unit, CMIC_SCHAN_MESSAGE(unit, i), msg->dwords[i]);
        }

        soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_MSG_START_SET);

        /* Wait for completion using either the interrupt or polling method */
        if (((flags & SCHAN_OP_FLAG_ALLOW_INTERRUPTS) != 0) &&  SOC_CONTROL(unit)->schanIntrEnb) {
            rv = _soc_cmice_schan_intr_wait(unit);
        } else {
            rv = _soc_cmice_schan_poll_wait(unit, msg);
        }

        if (soc_schan_timeout_check(unit, &rv, msg, 0, 0, flags) == TRUE) {
            break;
        }

        /* Read in data from S-Channel buffer space, if any */
        for (i = 0; i < dwc_read; i++) {
            msg->dwords[i] = soc_pci_read(unit, CMIC_SCHAN_MESSAGE(unit, i));
        }

        if (LOG_CHECK(BSL_LS_SOC_SCHAN | BSL_VERBOSE)) {
            soc_schan_dump(unit, msg, dwc_read);
        }

    } while (0);

    SCHAN_UNLOCK(unit);

    if (rv == SOC_E_TIMEOUT) {
        if (LOG_CHECK(BSL_LS_SOC_SCHAN | BSL_ERROR)) {
            LOG_ERROR(BSL_LS_SOC_SCHAN,
                      (BSL_META_U(unit,
                                  "SchanTimeOut:soc_schan_op operation timed out\n")));
            soc_schan_dump(unit, msg, dwc_write);
        }
    }

    return rv;
}

/*******************************************
* @function _soc_cmice_schan_deinit
* purpose API to de-nitialize cmice SCHAN
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_soc_cmice_schan_deinit(int unit)
{
    _soc_cmice_schan_reset(unit, 0, 0);
    return SOC_E_NONE;
}

/*******************************************
* @function soc_schan_cmice_init
* purpose API to Initialize cmice SCHAN
*
* @param unit [in] unit
* @param drv [out] soc_cmic_schan_drv_t
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
* @comments used for cmice
 */
int soc_cmice_schan_init(int unit, soc_cmic_schan_drv_t *drv)
{
    drv->soc_schan_deinit = _soc_cmice_schan_deinit;
    drv->soc_schan_op = _soc_cmice_schan_op;
    drv->soc_schan_reset = _soc_cmice_schan_reset;

    return SOC_E_NONE;

}
