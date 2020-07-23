/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * S-Channel (internal command bus) support
 */

#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <sal/core/boot.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/error.h>
#include <sal/core/sync.h>
#include <soc/util.h>
#include <soc/cmic.h>

#ifdef BCM_CMICX_SUPPORT
#include <soc/cmicx.h>
#include <soc/intr_cmicx.h>
#include <soc/schanmsg_internal.h>

#ifdef BCM_SAND_SUPPORT
#include <soc/sand/sand_aux_access.h>
#endif
#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#include <shared/cmicfw/cmicx_link.h>
#endif

#define SOC_SCHAN_POLL_WAIT_TIMEOUT 100


typedef struct intr_data_s {
    sal_sem_t   schanIntr;
    int         ch;
}intr_data_t;

typedef struct soc_cmicx_schan_s {
    sal_spinlock_t   lock;
    int              timeout;
    int              ch_map;
    intr_data_t      intr[CMIC_SCHAN_NUM_MAX];
}soc_cmicx_schan_t;


STATIC soc_cmicx_schan_t _soc_cmicx_schan[SOC_MAX_NUM_DEVICES];

#if defined BCM_DNX_SUPPORT && defined DNX_EMULATION_1_CORE
STATIC int _adjust_2nd_core_sbus_access(schan_header_t *header, uint32 blk)
{
    switch(header->v4.opcode) {
      case READ_REGISTER_CMD_MSG:
        /* No need for header->v4.opcode = READ_REGISTER_ACK_MSG; as a register is read */
        break;
      case READ_MEMORY_CMD_MSG:
        /* No need for header->v4.opcode = READ_MEMORY_ACK_MSG; as a memory is read*/
        break;
      /* writes that are not really performed do not require setting a returned opcode */
      default:
        header->v4.err = 0;
        return TRUE;
    }
    header->v4.dst_blk = blk >> 4;
    header->v4.acc_type = (blk & 0xf) << 1;
    return FALSE;
}

#define SET_V4_HEADER_DST_BLK_TYPE(blk_value_) \
    exit_now = _adjust_2nd_core_sbus_access(header, blk_value_); \
    break;

#endif /* defined BCM_DNX_SUPPORT && defined DNX_EMULATION_1_CORE */


/*******************************************
* @function _soc_cmicx_schan_reset
* purposeResets the CMICX S-Channel interface.
*
* @param unit [in] unit #
* @param cmc [in] cmc number 0
* @param ch [in] channel number 0-4
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */

STATIC void
_soc_cmicx_schan_reset(int unit, int cmc, int ch)
{
    uint32 val;

    COMPILER_REFERENCE(cmc);

    val = soc_pci_read(unit, CMIC_COMMON_POOL_SCHAN_CHx_CTRL(ch));
    /* Toggle S-Channel abort bit in CMIC_SCHAN_CTRL register */
    soc_pci_write(unit, CMIC_COMMON_POOL_SCHAN_CHx_CTRL(ch),
                                       val | SC_CHx_SCHAN_ABORT);
    SDK_CONFIG_MEMORY_BARRIER;
    soc_pci_write(unit, CMIC_COMMON_POOL_SCHAN_CHx_CTRL(ch), val);
    SDK_CONFIG_MEMORY_BARRIER;

    if (SAL_BOOT_QUICKTURN) {
        /* Give Quickturn at least 2 cycles */
        sal_usleep(10 * MILLISECOND_USEC);
    }
}

/*******************************************
* @function _soc_cmicx_schan_get
* purpose get idle channel
*
* @param unit [in] unit #
* @param ch [out] channel number 0-4
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
STATIC int _soc_cmicx_schan_get(int unit, int *ch)
{
    int i;
    int rv = SOC_E_BUSY;

    soc_timeout_t to;

    soc_timeout_init(&to,  _soc_cmicx_schan[unit].timeout, 100);

    do {
       sal_spinlock_lock(_soc_cmicx_schan[unit].lock);
       for ( i = 0; i < CMIC_SCHAN_NUM_MAX; i++) {
           if (_soc_cmicx_schan[unit].ch_map & 0x01 << i) {
               rv = SOC_E_NONE;
               *ch = i;
               _soc_cmicx_schan[unit].ch_map &= ~(0x01 << i);
               break;
           }
       }
       sal_spinlock_unlock(_soc_cmicx_schan[unit].lock);
       if (rv == SOC_E_NONE)
          break;

    } while (!soc_timeout_check(&to));
    return rv;
}


/*******************************************
* @function _soc_cmicx_schan_put
* purpose get idle channel
*
* @param unit [in] unit #
* @param ch [in] channel number 0-4
*
* @returns SOC_E_PARAM
* @returns SOC_E_NONE

*
* @end
 */
STATIC int _soc_cmicx_schan_put(int unit, int ch)
{
   int rv = SOC_E_NONE;

   if ((ch < 0) || (ch > CMIC_SCHAN_NUM_MAX -1)) {
       rv = SOC_E_PARAM;
       return rv;
   }
   sal_spinlock_lock(_soc_cmicx_schan[unit].lock);
   _soc_cmicx_schan[unit].ch_map |= (0x01 << ch);
   sal_spinlock_unlock(_soc_cmicx_schan[unit].lock);

   return rv;
}

/*******************************************
* @function _soc_cmicx_schan_cmic_err_handle
* purpose Handle schan cimicx error
*
* @param unit [in] unit
* @param schanCtrl [in] schan control word
* @param ch [in] channel
*
* @returns SOC_E_XXX
* @returns SOC_E_NONE
*
* @end
 */

STATIC int
_soc_cmicx_schan_err_handle(int unit, int schanCtrl, int ch, uint32 flags)
{
    int rv = SOC_E_NONE;
    uint32 schan_err = 0;

    if (schanCtrl & SC_CHx_MSG_NAK) {
        LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                   (BSL_META_U(unit,
                   "NAK received from SCHAN.\n")));
        rv = SOC_E_FAIL;
    }

    if (schanCtrl & SC_CHx_MSG_SER_CHECK_FAIL) {
        LOG_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit,
                  "SER Parity Check Error.\n")));
        rv = SOC_E_FAIL;
    }

    if (soc_feature(unit, soc_feature_schan_hw_timeout)) {
        if (schanCtrl & SC_CHx_MSG_TIMEOUT_TST) {
            LOG_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit,
                      "Hardware Timeout Error.\n")));
            rv = SOC_E_TIMEOUT;
        }
    }

    if (soc_feature(unit, soc_feature_schan_err_check)) {

        if (schanCtrl & SC_CHx_MSG_SCHAN_ERR) {
            uint32 err_code, data_len, src_port, dst_port, op_code;

            schan_err = soc_pci_read(unit, CMIC_COMMON_POOL_SCHAN_CHx_ERR(ch));

            rv = SOC_E_FAIL;

            if (schan_err & SC_CHx_SCHAN_ERR_ERR_BIT) {
                rv = SOC_E_TIMEOUT;
                if ((flags & SCHAN_OP_FLAG_DO_NOT_LOG_TIMEOUTS) == 0)
                {
                    LOG_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit,
                        "  block timeout: ERRBIT received in CMIC_SCHAN_ERR.\n")));
                }
            }

            if (schan_err & SC_CHx_SCHAN_ERR_NACK_BIT) {
                LOG_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit,
                    "  NACK received in CMIC_SCHAN_ERR.\n")));
            }

            /* dump error data */
            err_code = soc_reg_field_get(unit, CMIC_COMMON_POOL_SCHAN_CH0_ERRr, schan_err, ERR_CODEf);
            data_len = soc_reg_field_get(unit, CMIC_COMMON_POOL_SCHAN_CH0_ERRr, schan_err, DATA_LENf);
            src_port = soc_reg_field_get(unit, CMIC_COMMON_POOL_SCHAN_CH0_ERRr, schan_err, SRC_PORTf);
            dst_port = soc_reg_field_get(unit, CMIC_COMMON_POOL_SCHAN_CH0_ERRr, schan_err, DST_PORTf);
            op_code = soc_reg_field_get(unit, CMIC_COMMON_POOL_SCHAN_CH0_ERRr, schan_err, OP_CODEf);
            if ((flags & SCHAN_OP_FLAG_DO_NOT_LOG_TIMEOUTS) == 0)
            {
                LOG_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit,"CMIC_SCHAN_ERR data dump: "
                        "err_code=%u, data_len=%u, src_port=%u, dst_port=%u, op_code=%u\n"),
                        err_code, data_len, src_port, dst_port, op_code));
            }
        }

        soc_pci_write(unit, CMIC_COMMON_POOL_SCHAN_CHx_CTRL(ch), SC_CHx_MSG_CLR);
    }

    return rv;
}

/*******************************************
* @function _soc_cmicx_schan_intr_wait
* purpose On CMICX chips with schan interrupts
*
* @param unit [in] unit
* @param ch [in] channel number 0-4
*
* @returns SOC_E_XXX
* @returns SOC_E_XXX
*
* Commnet: Called when CMICX chips with schan interrupts
* enabled, wait on a schan interrupt.
* When schan interrupts are disabled,
* _soc_cmicx_schan_poll_wait is called instead.

* @end
 */
STATIC int
_soc_cmicx_schan_intr_wait(int unit, int ch, uint32 flags)
{
    int rv = SOC_E_NONE;
    uint32 schanCtrl;
    intr_data_t    *int_data;

    soc_cmic_intr_enable(unit, INTR_SCHAN(ch));
    int_data = &_soc_cmicx_schan[unit].intr[ch];
    if (sal_sem_take(int_data->schanIntr,
                     SOC_CONTROL(unit)->schanTimeout) != 0) {
        soc_cmic_intr_disable(unit, INTR_SCHAN(ch));
        rv = SOC_E_TIMEOUT;
    }

    if (rv != SOC_E_TIMEOUT) {
        LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "  Interrupt Done\n")));
        schanCtrl = soc_pci_read(unit, CMIC_COMMON_POOL_SCHAN_CHx_CTRL(ch));
        rv = _soc_cmicx_schan_err_handle(unit, schanCtrl, ch, flags);
    }
    return rv;
}

/*******************************************
* @function _soc_cmicx_schan_poll_wait
* purpose On CMICX chips with no schan interrupts
*
* @param unit [in] unit
* @param msg [in] message <schan_msg_t>
* @param ch [in] channel number 0-4
*
* @returns SOC_E_XXX
* @returns SOC_E_XXX
*
* Commnet: Called when CMICX chips with schan interrupts
* disabled, wait on a schan interrupt.
* When schan interrupts are enabled,
* _soc_cmicx_schan_intr_wait is called instead.

* @end
 */

STATIC int
_soc_cmicx_schan_poll_wait(int unit, schan_msg_t *msg, int ch, uint32 flags)
{
    int rv = SOC_E_NONE;
    soc_timeout_t to;
    uint32 schanCtrl;

    soc_timeout_init(&to, SOC_CONTROL(unit)->schanTimeout,
                     SOC_SCHAN_POLL_WAIT_TIMEOUT);

    LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "  Timeout= %d usec\n"),
                             SOC_CONTROL(unit)->schanTimeout));

    while (((schanCtrl = soc_pci_read(unit, CMIC_COMMON_POOL_SCHAN_CHx_CTRL(ch))) &
            SC_CHx_MSG_DONE) == 0) {
        if (soc_timeout_check(&to)) {
            rv = SOC_E_TIMEOUT;
            if ((flags & SCHAN_OP_FLAG_DO_NOT_LOG_TIMEOUTS) == 0)
            {
                LOG_VERBOSE(BSL_LS_SOC_SCHAN, (BSL_META_U(unit,
                " schan control value 0x%x after timeout in %d polls\n"), schanCtrl, to.polls));
            }
            break;
        }
    }
    /* Polling finished no timeout , check other errors */
    if (rv == SOC_E_NONE) {
        LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "  Done in %d polls\n"), to.polls));
        rv = _soc_cmicx_schan_err_handle(unit, schanCtrl, ch, flags);
    }

    return rv;
}

/*******************************************
* @function _soc_cmicx_schan_wait
* purpose This waits for either interrupt or poll
*
* @param handle [in] unit
* @param msg [in] SCHAN message pointer
* @param dwc_write [in] Number of messages to write
* @param dwc_read [in] Number of messages to read
* @param flags [in] Interrupt
* @param ch [in] channel number 0-4
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */

STATIC int _soc_cmicx_schan_wait(int unit,
                                 schan_msg_t *msg,
                                 int dwc_write,
                                 int dwc_read,
                                 uint32 flags,
                                 int ch)
{
    int i, rv;

    LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "  Enter\n")));

    do {

        /* Wait for completion using either the interrupt or polling method */
        if (((flags & SCHAN_OP_FLAG_ALLOW_INTERRUPTS) != 0) && *(SOC_SCHAN_CONTROL(unit).schanIntrEnb)) {
            rv = _soc_cmicx_schan_intr_wait(unit, ch, flags);
        } else {
            rv = _soc_cmicx_schan_poll_wait(unit, msg, ch, flags);
        }

        if (soc_schan_timeout_check(unit, &rv, msg, -1, ch, flags) == TRUE) {
            break;
        }

        /* Read in data from S-Channel buffer space, if any */
        for (i = 0; i < dwc_read; i++) {
            msg->dwords[i] =
                 soc_pci_read(unit, CMIC_COMMON_POOL_SCHAN_CHx_MESSAGEn(ch, i));
        }

        if (LOG_CHECK(BSL_LS_SOC_SCHAN | BSL_VERBOSE)) {
            soc_schan_dump(unit, msg, dwc_read);
        }

    } while(0);

    if ((rv == SOC_E_TIMEOUT) && ((flags & SCHAN_OP_FLAG_DO_NOT_LOG_TIMEOUTS) == 0)) {
        if (LOG_CHECK(BSL_LS_SOC_SCHAN | BSL_ERROR)) {
            LOG_ERROR(BSL_LS_SOC_SCHAN,
                      (BSL_META_U(unit,
                       "soc_cmicx_schan_op operation timed out\n")));
            soc_schan_dump(unit, msg, dwc_read);
        }
    }

    return rv;
}

/*******************************************
* @function soc_cmicx_schan_op
* purpose SDK-6 API used to initiate the schan operation
*
* @param handle [in] unit
* @param msg [in] SCHAN message pointer
* @param dwc_write [in] Number of messages to write
* @param dwc_read [in] Number of messages to read
* @param flags [in] Interrupt
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_soc_cmicx_schan_op(
         int unit,
         schan_msg_t *msg,
         int dwc_write,
         int dwc_read,
         uint32 flags)

{
    int i, rv;
    int ch = 0;

    LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "  Enter\n")));

#if defined BCM_DNX_SUPPORT && defined DNX_EMULATION_1_CORE
    if (SOC_IS_JERICHO2_A0(unit) && (soc_sand_is_emulation_system(unit)!= 0)) {
        uint32 sbus_blk_id;
        int exit_now = 0;
        schan_header_t *header = &(msg->header);
        sbus_blk_id = (header->v4.dst_blk << 4) |(header->v4.acc_type>>1);
        switch(sbus_blk_id) {
            /* core 1 doesn't exist, redirect to core 0 when reading, return OK when writing */
            case 97: SET_V4_HEADER_DST_BLK_TYPE(96 )/* KAPS1 */
            case 155: SET_V4_HEADER_DST_BLK_TYPE(36) /* IPPA1 */
            case 156: SET_V4_HEADER_DST_BLK_TYPE(37) /* IPPB1 */
            case 157: SET_V4_HEADER_DST_BLK_TYPE(38)  /* IPPC1 */
            case 158: SET_V4_HEADER_DST_BLK_TYPE(39)  /* IPPD1 */
            case 159: SET_V4_HEADER_DST_BLK_TYPE(40) /* IPPE1 */
            case 160: SET_V4_HEADER_DST_BLK_TYPE(41)  /* IPPF1 */
            case 120: SET_V4_HEADER_DST_BLK_TYPE(51)  /* CDUM1 */
            /* Should not be really needed */
            case 121: SET_V4_HEADER_DST_BLK_TYPE(53)  /* CDU5 */
            case 122: SET_V4_HEADER_DST_BLK_TYPE(53)  /* CDU6 */
            case 123: SET_V4_HEADER_DST_BLK_TYPE(53)  /* CDU7 */
            case 124: SET_V4_HEADER_DST_BLK_TYPE(53)  /* CDU8 */
            case 125: SET_V4_HEADER_DST_BLK_TYPE(53)  /* CDU9 */
            case 151: SET_V4_HEADER_DST_BLK_TYPE(32)  /* PEM4 */
            case 152: SET_V4_HEADER_DST_BLK_TYPE(33)  /* PEM5 */
            case 153: SET_V4_HEADER_DST_BLK_TYPE(34)  /* PEM6 */
            case 154: SET_V4_HEADER_DST_BLK_TYPE(35)  /* PEM7 */
#ifdef JR2_PREVENT_ACCESS_TO_ADDITIONAL_BLOCKS
            case 384: SET_V4_HEADER_DST_BLK_TYPE(240)  /* FSRD3 */
            case 432: SET_V4_HEADER_DST_BLK_TYPE(240)  /* FSRD4 */
            case 480: SET_V4_HEADER_DST_BLK_TYPE(240)  /* FSRD5 */
            case 528: SET_V4_HEADER_DST_BLK_TYPE(240)  /* FSRD6 */
            case 576: SET_V4_HEADER_DST_BLK_TYPE(240)  /* FSRD7 */
            case 624: SET_V4_HEADER_DST_BLK_TYPE(240)  /* FSRD8 */
            case 672: SET_V4_HEADER_DST_BLK_TYPE(240)  /* FSRD9 */
            case 720: SET_V4_HEADER_DST_BLK_TYPE(240)  /* FSRD10 */
            case 768: SET_V4_HEADER_DST_BLK_TYPE(240)  /* FSRD11 */
            case 816: SET_V4_HEADER_DST_BLK_TYPE(240)  /* FSRD12 */
            case 864: SET_V4_HEADER_DST_BLK_TYPE(240)  /* FSRD13 */
            case 352: SET_V4_HEADER_DST_BLK_TYPE(208)  /* FMAC6 */
            case 368: SET_V4_HEADER_DST_BLK_TYPE(208)  /* FMAC7 */
            case 400: SET_V4_HEADER_DST_BLK_TYPE(208)  /* FMAC8 */
            case 416: SET_V4_HEADER_DST_BLK_TYPE(208)  /* FMAC9 */
            case 448: SET_V4_HEADER_DST_BLK_TYPE(208)  /* FMAC10 */
            case 464: SET_V4_HEADER_DST_BLK_TYPE(208)  /* FMAC11 */
            case 496: SET_V4_HEADER_DST_BLK_TYPE(208)  /* FMAC12 */
            case 512: SET_V4_HEADER_DST_BLK_TYPE(208)  /* FMAC13 */
            case 544: SET_V4_HEADER_DST_BLK_TYPE(208)  /* FMAC14 */
            case 560: SET_V4_HEADER_DST_BLK_TYPE(208)  /* FMAC15 */
            case 592: SET_V4_HEADER_DST_BLK_TYPE(208)  /* FMAC16 */
            case 608: SET_V4_HEADER_DST_BLK_TYPE(208)  /* FMAC17 */
            case 640: SET_V4_HEADER_DST_BLK_TYPE(208)  /* FMAC18 */
            case 656: SET_V4_HEADER_DST_BLK_TYPE(208)  /* FMAC19 */
            case 688: SET_V4_HEADER_DST_BLK_TYPE(208)  /* FMAC20 */
            case 704: SET_V4_HEADER_DST_BLK_TYPE(208)  /* FMAC21 */
            case 736: SET_V4_HEADER_DST_BLK_TYPE(208)  /* FMAC22 */
            case 752: SET_V4_HEADER_DST_BLK_TYPE(208)  /* FMAC23 */
            case 784: SET_V4_HEADER_DST_BLK_TYPE(208)  /* FMAC24 */
            case 800: SET_V4_HEADER_DST_BLK_TYPE(208)  /* FMAC25 */
            case 832: SET_V4_HEADER_DST_BLK_TYPE(208)  /* FMAC26 */
            case 848: SET_V4_HEADER_DST_BLK_TYPE(208)  /* FMAC27 */
            case 1792: SET_V4_HEADER_DST_BLK_TYPE(1072)  /* CDMAC12, CDMAC13, CDPORT6 */
            case 1808: SET_V4_HEADER_DST_BLK_TYPE(1072)  /* CDMAC14, CDMAC15, CDPORT7 */
            case 1824: SET_V4_HEADER_DST_BLK_TYPE(1072)  /* CDMAC16, CDMAC17, CDPORT8 */
            case 1840: SET_V4_HEADER_DST_BLK_TYPE(1072)  /* CDMAC18, CDMAC19, CDPORT9 */
            case 1856: SET_V4_HEADER_DST_BLK_TYPE(1072)  /* CDMAC20, CDMAC20, CDPORT10 */
            case 1872: SET_V4_HEADER_DST_BLK_TYPE(1072)  /* CDMAC22, CDMAC23, CDPORT11 */
#endif /* JR2_PREVENT_ACCESS_TO_ADDITIONAL_BLOCKS */
            case 99: SET_V4_HEADER_DST_BLK_TYPE(98)  /* TCAM1 */
            case 131: SET_V4_HEADER_DST_BLK_TYPE(68)  /* IQM1 */
            case 161: SET_V4_HEADER_DST_BLK_TYPE(42)  /* ITPPD1 */
            case 162: SET_V4_HEADER_DST_BLK_TYPE(43)  /* ITPP1 */
            case 164: SET_V4_HEADER_DST_BLK_TYPE(45)  /* ETPPA1 */
            case 165: SET_V4_HEADER_DST_BLK_TYPE(46)  /* ETPPB1 */
            case 166: SET_V4_HEADER_DST_BLK_TYPE(47)  /* ETPPC1 */
            case 163: SET_V4_HEADER_DST_BLK_TYPE(44)  /* ERPP1 */
            case 150: SET_V4_HEADER_DST_BLK_TYPE(31)  /* EDB1 */
            case 132: SET_V4_HEADER_DST_BLK_TYPE(69)  /* DQM1 */
            case 136: SET_V4_HEADER_DST_BLK_TYPE(17)  /* CRPS1 */
            case 127: SET_V4_HEADER_DST_BLK_TYPE(64)  /* MRPS1 */
            case 143: SET_V4_HEADER_DST_BLK_TYPE(24)  /* EPNI1 */
            case 144: SET_V4_HEADER_DST_BLK_TYPE(25)  /* EPRE1 */
            case 145: SET_V4_HEADER_DST_BLK_TYPE(26)  /* ECGM1 */
            case 139: SET_V4_HEADER_DST_BLK_TYPE(20)  /* CGM1 */
            case 141: SET_V4_HEADER_DST_BLK_TYPE(22)  /* FDR1 */
            case 129: SET_V4_HEADER_DST_BLK_TYPE(66)  /* MCP1 */
            case 881: SET_V4_HEADER_DST_BLK_TYPE(880)  /* OCB1 */
            case 118: SET_V4_HEADER_DST_BLK_TYPE(48)  /* ILE1 */
            case 167: SET_V4_HEADER_DST_BLK_TYPE(63)  /* SCH1 */
            case 138: SET_V4_HEADER_DST_BLK_TYPE(19)  /* IPT1 */
            case 137: SET_V4_HEADER_DST_BLK_TYPE(18)  /* IPS1 */
            case 134: SET_V4_HEADER_DST_BLK_TYPE(71)  /* IRE1 */
            case 119: SET_V4_HEADER_DST_BLK_TYPE(49)  /* NMG1 */
            case 149: SET_V4_HEADER_DST_BLK_TYPE(30)  /* EPS1 */
            case 148: SET_V4_HEADER_DST_BLK_TYPE(29)  /* FQP1 */
            case 146: SET_V4_HEADER_DST_BLK_TYPE(27)  /* RQP1 */
            case 147: SET_V4_HEADER_DST_BLK_TYPE(28)  /* PQP1 */
            case 133: SET_V4_HEADER_DST_BLK_TYPE(70)  /* SQM1 */
            case 135: SET_V4_HEADER_DST_BLK_TYPE(16)  /* SPB1 */
            case 128: SET_V4_HEADER_DST_BLK_TYPE(65)  /* DDP1 */
            case 130: SET_V4_HEADER_DST_BLK_TYPE(67)  /* PDM1 */
            case 140: SET_V4_HEADER_DST_BLK_TYPE(21)  /* BDM1 */
            case 126: SET_V4_HEADER_DST_BLK_TYPE(62)  /* SIF1 */
            default:
                break;
        }
        if (exit_now) {
            return SOC_E_NONE;
        }
    }
#endif /* defined BCM_DNX_SUPPORT && defined DNX_EMULATION_1_CORE */

    if ((soc_schan_op_sanity_check(unit, msg,
                   dwc_write, dwc_read, &rv) == TRUE) &&
        (!(flags & SCHAN_OP_FLAG_ALLOW_INDIRECT_ACCESS_READ))){
        return rv;
    }

    /* Get the free channel */
    rv = _soc_cmicx_schan_get(unit, &ch);

    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "  Unable to assign channel\n")));
        return rv;

    }

    LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             " Assign channel = %d\n"), ch));

    do {
        /* Write raw S-Channel Data: dwc_write words */
        for (i = 0; i < dwc_write; i++) {
            soc_pci_write(unit, CMIC_COMMON_POOL_SCHAN_CHx_MESSAGEn(ch, i),
                                            msg->dwords[i]);
        }

        /* Tell CMIC to start */
        soc_pci_write(unit, CMIC_COMMON_POOL_SCHAN_CHx_CTRL(ch),
                                            SC_CHx_MSG_START);
    } while(0);

    rv = _soc_cmicx_schan_wait(unit, msg, dwc_write, dwc_read, flags, ch);

    _soc_cmicx_schan_put(unit, ch);

   return rv;
}

/*******************************************
* @function _cmicx_schan_ch_done
* Interrupt handler for schan channel done
*
* @param unit [in] unit
* @param data [in] pointer provided during registration
*
*
* @end
 */
STATIC void
_cmicx_schan_ch_done(int unit, void *data)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    intr_data_t *in_d = (intr_data_t *)data;

    if (soc->schanIntrEnb) {
        soc_cmic_intr_disable(unit, INTR_SCHAN(in_d->ch));
        sal_sem_give(in_d->schanIntr);
    }
}

/*******************************************
* @function _cmicx_schan_intr_deinit
* purpose API to de-Initialize SCHAN interrupts
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_cmicx_schan_intr_deinit(int unit)
{
    int ch;
    intr_data_t    *int_data;
    for (ch = 0; ch < CMIC_SCHAN_NUM_MAX; ch++) {
        int_data = &_soc_cmicx_schan[unit].intr[ch];
        if (int_data->schanIntr) {
             sal_sem_destroy(int_data->schanIntr);
        }
    }

    return SOC_E_NONE;
}

/*******************************************
* @function _cmicx_schan_intr_init
* purpose API to Initialize SCHAN interrupts
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_cmicx_schan_intr_init(int unit)
{
    int ch;
    soc_cmic_intr_handler_t *handle;
    soc_cmic_intr_handler_t *hitr;
    intr_data_t    *int_data;
    int rv = SOC_E_NONE;


    /* Register interrupts */
    handle = sal_alloc(CMIC_SCHAN_NUM_MAX * sizeof(soc_cmic_intr_handler_t),
                        "schan_interrupt");
    if (handle == NULL) {
        return SOC_E_MEMORY;
    }

    hitr = handle;
    for (ch = 0; ch < CMIC_SCHAN_NUM_MAX; ch++) {
         int_data = &_soc_cmicx_schan[unit].intr[ch];
         int_data->schanIntr =
                         sal_sem_create("SCHAN interrupt",
                                        sal_sem_BINARY, 0);
         if (int_data->schanIntr == NULL) {
             _cmicx_schan_intr_deinit(unit);
             rv = SOC_E_MEMORY;
             goto error;
         }
         int_data->ch = ch;
         hitr->num = INTR_SCHAN(ch);
         hitr->intr_fn = _cmicx_schan_ch_done;
         hitr->intr_data = int_data;
         hitr++;
    }
    rv = soc_cmic_intr_register(unit, handle, CMIC_SCHAN_NUM_MAX);
error:
    sal_free(handle);

    return rv;
}


/*******************************************
* @function _soc_cmicx_schan_deinit
* purpose API to de-Initialize SCHAN
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_soc_cmicx_schan_deinit(int unit)
{
   int ch;

   _cmicx_schan_intr_deinit(unit);

   /* reset all th schan channels */
   for (ch = 0 ; ch < CMIC_SCHAN_NUM_MAX; ch++) {
       _soc_cmicx_schan_reset(unit, -1, ch);
   }
   sal_spinlock_destroy(_soc_cmicx_schan[unit].lock);

   return SOC_E_NONE;
}

/*******************************************
* @function soc_cmicx_schan_init
* purpose API to Initialize cmicx SCHAN
*
* @param unit [in] unit
* @param drv [out] soc_cmic_schan_drv_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int soc_cmicx_schan_init(int unit, soc_cmic_schan_drv_t *drv)

{
   int rv = SOC_E_NONE;
#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
   uint32 schan_num;
#endif

#if defined(PLISIM) && (defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT))
        if (SAL_BOOT_PLISIM && (SOC_IS_DNX(unit)|| SOC_IS_DNXF(unit))) {
            /* Use CMICM simulation instead of CMICX simulation */
            return soc_cmicm_schan_init(unit, drv);
        }
#endif

   sal_memset(&_soc_cmicx_schan[unit], 0, sizeof(soc_cmicx_schan_t));
   _soc_cmicx_schan[unit].lock = sal_spinlock_create("schan Lock");

   if (_soc_cmicx_schan[unit].lock == NULL) {
        return SOC_E_MEMORY;
   }

   _soc_cmicx_schan[unit].ch_map = (0x01 << CMIC_SCHAN_NUM_MAX) -1;
   _soc_cmicx_schan[unit].timeout = SOC_CONTROL(unit)->schanTimeout;
   /* program SBUS_RING_MAP registers */

   /* Program SCHAN ARB Weighted priority
    * CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_BASE  12'h04c */

   /* Configure CMIC_SBUS_TIMEOUT */
   rv = _cmicx_schan_intr_init(unit);

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
    if ((SOC_IS_DNX(unit)) || (SOC_IS_DNXF(unit)))
    {
        schan_num = soc_property_get(unit, spn_CMICX_LINK_SBUS_CHANNEL_ID, CMICX_LS_M0FW_SCHAN_DEF);
        if (schan_num < CMIC_SCHAN_NUM_MAX)
        {
            _soc_cmicx_schan[unit].ch_map &= ~(0x01 << schan_num);
        }
    }
#endif

   if (SOC_FAILURE(rv)) {
       _soc_cmicx_schan_deinit(unit);
       return rv;
   }

   drv->soc_schan_deinit = _soc_cmicx_schan_deinit;
   drv->soc_schan_op = _soc_cmicx_schan_op;
   drv->soc_schan_reset = _soc_cmicx_schan_reset;

   return rv;
}


#endif /* BCM_CMICX_SUPPORT  */
