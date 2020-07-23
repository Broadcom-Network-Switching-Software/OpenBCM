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
#include <sal/core/dpc.h>

#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/drv.h>
#include <soc/hwstate/hw_log.h>
#include <soc/error.h>
#include <soc/cmic.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif
#include <soc/schanmsg_internal.h>

#if defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
#include <soc/dnxc/dnxc_ha.h>
#endif

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_err_recovery_manager.h>
#endif

#if defined(PLISIM) && defined(PLISIM_DIRECT_SCHAN)
/* Back door into the simulation to perform direct SCHANNEL operations */
extern uint32 plibde_schan_op(int unit, schan_msg_t* msg, int, int);
#endif

#ifdef _SER_TIME_STAMP
sal_usecs_t ser_time_1;
#endif

#define IS_WRITE_ALLOWED_DURING_WB(unit) \
    (SOC_CONTROL(unit)->schan_wb_thread_id == sal_thread_self())

soc_cmic_schan_control_t  soc_schan_control[SOC_MAX_NUM_DEVICES];

STATIC soc_cmic_schan_drv_t _soc_schan_drv[SOC_MAX_NUM_DEVICES];


/*
 * S-Chanel operation names
 */

STATIC char *_soc_schan_op_names[] = {
    "UNKNOWN_OPCODE",
    "BP_WARN_STATUS",           /* 0x01 */
    "BP_DISCARD_STATUS",        /* 0x02 */
    "COS_QSTAT_NOTIFY",         /* 0x03 */
    "HOL_STAT_NOTIFY",          /* 0x04 */
    "",                         /* 0x05 */
    "",                         /* 0x06 */
    "READ_MEM_CMD",             /* 0x07 */
    "READ_MEM_ACK",             /* 0x08 */
    "WRITE_MEM_CMD",            /* 0x09 */
    "WRITE_MEM_ACK",            /* 0x0a */
    "READ_REG_CMD",             /* 0x0b */
    "READ_REG_ACK",             /* 0x0c */
    "WRITE_REG_CMD",            /* 0x0d */
    "WRITE_REG_ACK",            /* 0x0e */
    "ARL_INSERT_CMD",           /* 0x0f */
    "ARL_INSERT_DONE",          /* 0x10 */
    "ARL_DELETE_CMD",           /* 0x11 */
    "ARL_DELETE_DONE",          /* 0x12 */
    "LINKSTAT_NOTIFY",          /* 0x13 */
    "MEM_FAIL_NOTIFY",          /* 0x14 */
    "INIT_CFAP",                /* 0x15 */
    "",                         /* 0x16 */
    "ENTER_DEBUG_MODE",         /* 0x17 */
    "EXIT_DEBUG_MODE",          /* 0x18 */
    "ARL_LOOKUP_CMD",           /* 0x19 */
    "L3_INSERT_CMD",            /* 0x1a */
    "L3_INSERT_DONE",           /* 0x1b */
    "L3_DELETE_CMD",            /* 0x1c */
    "L3_DELETE_DONE",           /* 0x1d */
    "L3_LOOKUP_CMD",            /* 0x1e */
    "UNKNOWN_OPCODE",           /* 0x1f */
    "L2_LOOKUP_CMD_MSG",        /* 0x20 */
    "L2_LOOKUP_ACK_MSG",        /* 0x21 */
    "L3X2_LOOKUP_CMD_MSG",      /* 0x22 */
    "L3X2_LOOKUP_ACK_MSG",      /* 0x23 */
    "TABLE_INSERT_CMD_MSG",     /* 0x24 */
    "TABLE_INSERT_DONE_MSG",    /* 0x25 */
    "TABLE_DELETE_CMD_MSG",     /* 0x26 */
    "TABLE_DELETE_DONE_MSG",    /* 0x27 */
    "TABLE_LOOKUP_CMD_MSG",     /* 0x28 */
    "TABLE_LOOKUP_DONE_MSG",    /* 0x29 */
    "FIFO_POP_CMD_MSG",         /* 0x2a */
    "FIFO_POP_DONE_MSG",        /* 0x2b */
    "FIFO_PUSH_CMD_MSG",        /* 0x2c */
    "FIFO_PUSH_DONE_MSG",       /* 0x2d */
};

char *
soc_schan_op_name(int op)
{
    if (op < 0 || op >= COUNTOF(_soc_schan_op_names)) {
        op = 0;
    }

    return _soc_schan_op_names[op];
}

#if defined(BCM_TRIUMPH_SUPPORT)
STATIC char *_soc_schan_gen_resp_type_names[] = {
    "FOUND",          /*  0 */
    "NOT_FOUND",      /*  1 */
    "FULL",           /*  2 */
    "INSERTED",       /*  3 */
    "REPLACED",       /*  4 */
    "DELETED",        /*  5 */
    "ENTRY_IS_OLD",   /*  6 */
    "CLEARED_VALID",  /*  7 */
    "L2_FIFO_FULL",   /*  8 */
    "MAC_LIMIT_THRE", /*  9 */
    "MAC_LIMIT_DEL",  /* 10 */
    "L2_STATIC",      /* 11 */
    "UNKNOWN",        /* 12 */
    "UNKNOWN",        /* 13 */
    "UNKNOWN",        /* 14 */
    "ERROR"           /* 15 */
};

STATIC char *
soc_schan_gen_resp_type_name(int type)
{
    if (type < 0 || type >= COUNTOF(_soc_schan_gen_resp_type_names)) {
        type = 8;
    }

    return _soc_schan_gen_resp_type_names[type];
}

STATIC char *_soc_schan_gen_resp_err_names[] = {
    "NONE",            /* 0 */
    "SRAM_P_ERR",      /* 1 */
    "TCAM_SRCH_ERR",   /* 2 */
    "MULTIPLE",        /* 3 */
    "TCAM_RD_ERR",     /* 4 */
    "MULTIPLE",        /* 5 */
    "MULTIPLE",        /* 6 */
    "MULTIPLE",        /* 7 */
    "TCAM_SEQ_ERR",    /* 8 */
    "MULTIPLE",        /* 9 */
    "MULTIPLE",        /* 10 */
    "MULTIPLE",        /* 11 */
    "MULTIPLE",        /* 12 */
    "MULTIPLE",        /* 13 */
    "MULTIPLE",        /* 14 */
    "MULTIPLE",        /* 15 */
};


STATIC char *
soc_schan_gen_resp_err_name(int err)
{
    if (err < 0 || err >= COUNTOF(_soc_schan_gen_resp_err_names)) {
        err = 2;
    }

    return _soc_schan_gen_resp_err_names[err];
}

#endif /* BCM_TRIUMPH_SUPPORT */


#define IS_COMPOSITE(unit) 0
/** the 4 MSBs define the sub unit in case of composite device */
#define SUB_UNIT_INDEX_GET(unit, destination_block) (destination_block >> 28)
/*******************************************
* @function soc_schan_header_cmd_set
* purpose API to set the command header
*
* @param handle [in] unit
* @param header [out] Populated Command header pointer
* @param opcode [in] Opcode
* @param dst_blk [in] Block ID
* @param acc_type [in] Acc Type
* @param data_byte_len [in] Dlen
* @param dma, bank_ignore_mask [in] Misc Ctrl/ Status
* @returns None
*
* @end
 */
void
soc_schan_header_cmd_set(int unit, schan_header_t *header, int opcode,
                         int dst_blk, int src_blk, int acc_type,
                         int data_byte_len, int dma, uint32 bank_ignore_mask)
{
    if (soc_feature(unit, soc_feature_sbus_format_v4)) {
        header->v4.opcode = opcode;
        header->v4.dst_blk = soc_feature(unit, soc_feature_blkid_extended_format) ? (dst_blk >> 4) & 0x7f : dst_blk;
        /* input argument src_blk is ignored */
        header->v4.acc_type = soc_feature(unit, soc_feature_blkid_extended_format) ? (dst_blk & 0xf)<<1 : acc_type;
        header->v4.data_byte_len = data_byte_len;
        header->v4.dma = dma;
        header->v4.bank_ignore_mask = bank_ignore_mask;
        /* ecode field is currently not used, use this field to keep the sub unit index for composite devices */
        if(IS_COMPOSITE(unit))
        {
            header->v4.ecode = SUB_UNIT_INDEX_GET(unit, dst_blk);
        }
    } else if (soc_feature(unit, soc_feature_new_sbus_format)) {
        header->v3.opcode = opcode;
        header->v3.dst_blk = dst_blk;
        /* input argument src_blk is ignored */
        header->v3.acc_type = acc_type;
        header->v3.data_byte_len = data_byte_len;
        header->v3.dma = dma;
        header->v3.bank_ignore_mask = bank_ignore_mask;
    } else {
        header->v2.opcode = opcode;
        header->v2.dst_blk = dst_blk;
        header->v2.src_blk = src_blk;
        /* input argument acc_type is ignored */
        header->v2.data_byte_len = data_byte_len;
        /* input argument dma is ignored */
        header->v2.bank_ignore_mask = bank_ignore_mask;
    }
}

/*******************************************
* @function soc_schan_header_cmd_get
* purpose API to get the command header
*
* @param handle [in] unit
* @param header [in] Populated Command header pointer
* @param opcode [out] Opcode
* @param dst_blk [out] Block ID
* @param acc_type [out] Acc Type
* @param data_byte_len [out] Dlen
* @param dma, bank_ignore_mask [out] Misc Ctrl/ Status
* @returns None
*
* @end
 */
void
soc_schan_header_cmd_get(int unit, schan_header_t *header, int *opcode,
                         int *dst_blk, int *src_blk, int *acc_type,
                         int *data_byte_len, int *dma,
                         uint32 *bank_ignore_mask)
{
    if (soc_feature(unit, soc_feature_sbus_format_v4)) {
        if (opcode != NULL) {
            *opcode = header->v4.opcode;
        }
        if (dst_blk != NULL) {
            *dst_blk = soc_feature(unit, soc_feature_blkid_extended_format) ? (header->v4.dst_blk << 4) |(header->v4.acc_type>>1)  : header->v4.dst_blk;
        }
        if (src_blk != NULL) {
            *src_blk = 0;
        }
        if (acc_type != NULL) {
            *acc_type = (soc_feature(unit, soc_feature_blkid_extended_format)) ? 0  : header->v4.acc_type;
        }
        if (data_byte_len != NULL) {
            *data_byte_len = header->v4.data_byte_len;
        }
        if (dma != NULL) {
            *dma = header->v4.dma;
        }
        if (bank_ignore_mask != NULL) {
            *bank_ignore_mask = header->v4.bank_ignore_mask;
        }
    } else if (soc_feature(unit, soc_feature_new_sbus_format)) {
        if (opcode != NULL) {
            *opcode = header->v3.opcode;
        }
        if (dst_blk != NULL) {
            *dst_blk = header->v3.dst_blk;
        }
        if (src_blk != NULL) {
            *src_blk = 0;
        }
        if (acc_type != NULL) {
            *acc_type = header->v3.acc_type;
        }
        if (data_byte_len != NULL) {
            *data_byte_len = header->v3.data_byte_len;
        }
        if (dma != NULL) {
            *dma = header->v3.dma;
        }
        if (bank_ignore_mask != NULL) {
            *bank_ignore_mask = header->v3.bank_ignore_mask;
        }
    } else {
        if (opcode != NULL) {
            *opcode = header->v2.opcode;
        }
        if (dst_blk != NULL) {
            *dst_blk = header->v2.dst_blk;
        }
        if (src_blk != NULL) {
            *src_blk = header->v2.src_blk;
        }
        if (acc_type != NULL) {
            *acc_type = 0;
        }
        if (data_byte_len != NULL) {
            *data_byte_len = header->v2.data_byte_len;
        }
        if (dma != NULL) {
            *dma = 0;
        }
        if (bank_ignore_mask != NULL) {
            *bank_ignore_mask = header->v2.bank_ignore_mask;
        }
    }
}

/*******************************************
* @function soc_schan_header_status_get
* purpose API to get the header status
*
* @param handle [in] unit
* @param header [in] Populated Command header pointer
* @param opcode [out] Opcode
* @param dst_blk [out] Block ID
* @param acc_type [out] Acc Type
* @param data_byte_len [out] Dlen
* @param err, ecode, nack [out] Misc Ctrl/ Status
* @returns None
*
* @end
 */
void
soc_schan_header_status_get(int unit, schan_header_t *header, int *opcode,
                            int *src_blk, int *data_byte_len,
                            int *err, int *ecode, int *nack)
{
    if (soc_feature(unit, soc_feature_sbus_format_v4)) {
        *opcode = header->v4.opcode;
        if (src_blk != NULL) {
            *src_blk = 0;
        }
        if (data_byte_len != NULL) {
            *data_byte_len = header->v4.data_byte_len;
        }
        if (err != NULL) {
            *err = header->v4.err;
        }
        if (ecode != NULL) {
            *ecode = header->v4.ecode;
        }
        if (nack != NULL) {
            *nack = header->v4.nack;
        }
    } else if (soc_feature(unit, soc_feature_new_sbus_format)) {
        *opcode = header->v3.opcode;
        if (src_blk != NULL) {
            *src_blk = 0;
        }
        if (data_byte_len != NULL) {
            *data_byte_len = header->v3.data_byte_len;
        }
        if (err != NULL) {
            *err = header->v3.err;
        }
        if (ecode != NULL) {
            *ecode = header->v3.ecode;
        }
        if (nack != NULL) {
            *nack = header->v3.nack;
        }
    } else {
        *opcode = header->v2.opcode;
        if (src_blk != NULL) {
            *src_blk = header->v2.src_blk;
        }
        if (data_byte_len != NULL) {
            *data_byte_len = header->v2.data_byte_len;
        }
        if (err != NULL) {
            *err = header->v2.err;
        }
        if (ecode != NULL) {
            *ecode = header->v2.ecode;
        }
        if (nack != NULL) {
            *nack = header->v2.nack;
        }
    }
}

/*******************************************
* @function soc_schan_dump
* @Purpose Dump an S-Channel message for debugging
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
void
soc_schan_dump(int unit, schan_msg_t *msg, int dwc)
{
    char                buf[128];
    int                 i, j;

    if (soc_feature(unit, soc_feature_sbus_format_v4)) {
        uint32 dst_blk = msg->header.v4.dst_blk;
        uint32 acc_type = msg->header.v4.acc_type;
        uint32 block_id = soc_feature(unit, soc_feature_blkid_extended_format) ? (dst_blk << 4) | (acc_type >> 1) : dst_blk;
        LOG_VERBOSE(BSL_LS_SOC_SCHAN,
            (BSL_META_U(unit,
                            "  HDR[NACK=%d BANK=%d DMA=%d ECODE=%d ERR=%d "
                 "DLEN=%d ACC=%d DPORT=%d BLOCKID=%d OPC=%d=%s]\n"),
                 msg->header.v4.nack, msg->header.v4.bank_ignore_mask,
                 msg->header.v4.dma, msg->header.v4.ecode,
                 msg->header.v4.err, msg->header.v4.data_byte_len,
                 msg->header.v4.acc_type, msg->header.v4.dst_blk, block_id,
                 msg->header.v4.opcode,
                 soc_schan_op_name(msg->header.v4.opcode)));
    } else if (soc_feature(unit, soc_feature_new_sbus_format)) {
        LOG_VERBOSE(BSL_LS_SOC_SCHAN,
            (BSL_META_U(unit,
                            "  HDR[NACK=%d BANK=%d DMA=%d ECODE=%d ERR=%d "
                 "DLEN=%d ACC=%d DPORT=%d OPC=%d=%s]\n"),
                 msg->header.v3.nack, msg->header.v3.bank_ignore_mask,
                 msg->header.v3.dma, msg->header.v3.ecode,
                 msg->header.v3.err, msg->header.v3.data_byte_len,
                 msg->header.v3.acc_type, msg->header.v3.dst_blk,
                 msg->header.v3.opcode,
                 soc_schan_op_name(msg->header.v3.opcode)));
    } else {
        LOG_VERBOSE(BSL_LS_SOC_SCHAN,
            (BSL_META_U(unit,
                            "  HDR[NACK=%d BANK=%d DMA=%d ECODE=%d ERR=%d "
                 "DLEN=%d SPORT=%d DPORT=%d OPC=%d=%s]\n"),
                 msg->header.v2.nack, msg->header.v2.bank_ignore_mask,
                 msg->header.v2.dma, msg->header.v2.ecode,
                 msg->header.v2.err, msg->header.v2.data_byte_len,
                 msg->header.v2.src_blk, msg->header.v2.dst_blk,
                 msg->header.v2.opcode,
                 soc_schan_op_name(msg->header.v2.opcode)));
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_feature(unit, soc_feature_generic_table_ops) &&
        ((msg->header.v2.opcode == TABLE_INSERT_DONE_MSG) ||
         (msg->header.v2.opcode == TABLE_DELETE_DONE_MSG) ||
         (msg->header.v2.opcode == TABLE_LOOKUP_DONE_MSG))) {
        if (soc_feature(unit, soc_feature_new_sbus_format) &&
            !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
            LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                (BSL_META_U(unit,
                                "  RSP[TYPE=%d=%s ERR_INFO=%d=%s "
                     "INDEX=0x%05x]\n"),
                     msg->genresp_v2.response.type,
                     soc_schan_gen_resp_type_name(msg->genresp_v2.response.type),
                     msg->genresp_v2.response.err_info,
                     soc_schan_gen_resp_err_name(msg->genresp_v2.response.err_info),
                     msg->genresp_v2.response.index));
        } else {
            LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                (BSL_META_U(unit,
                                "  RSP[SRC=%d TYPE=%d=%s ERR_INFO=%d=%s "
                     "INDEX=0x%05x]\n"),
                     msg->genresp.response.src,
                     msg->genresp.response.type,
                     soc_schan_gen_resp_type_name(msg->genresp.response.type),
                     msg->genresp.response.err_info,
                     soc_schan_gen_resp_err_name(msg->genresp.response.err_info),
                     msg->genresp.response.index));
        }
    }
#endif /* BCM_TRIUMPH_SUPPORT */

    assert(dwc <= CMIC_SCHAN_WORDS(unit));

    for (i = 0; i < dwc; i += 4) {
        buf[0] = 0;

        for (j = i; j < i + 4 && j < dwc; j++) {
            sal_sprintf(buf + sal_strlen(buf),
                        " DW[%2d]=0x%08x", j, msg->dwords[j]);
        }

        LOG_VERBOSE(BSL_LS_SOC_SCHAN,
            (BSL_META_U(unit, " %s\n"), buf));
    }
}
/*******************************************
* @function soc_schan_override_enable
* @Purpose Enable schan override
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */

#define SOC_SCHAN_WB_TIME_OUT            (5000000) /*5 sec*/
int
soc_schan_override_enable(int unit)
{
    if (unit < 0 || unit >= SOC_MAX_NUM_DEVICES) {
        return SOC_E_UNIT;
    }
    if (!SOC_HW_ACCESS_DISABLE(unit)) {
        return SOC_E_NONE;
    }
    if (SOC_WARM_BOOT(unit)) {
        if (sal_mutex_take(SOC_CONTROL(unit)->schan_wb_mutex,
                           SOC_SCHAN_WB_TIME_OUT)) {
            LOG_ERROR(BSL_LS_SOC_SCHAN,
                      (BSL_META_U(unit,
                                  "Failed to take schan_wb_mutex.\n")));
            return SOC_E_INTERNAL;
        }
        SOC_CONTROL(unit)->schan_wb_thread_id = sal_thread_self();
    }

    return SOC_E_NONE;
}

/*******************************************
* @function soc_schan_override_disable
* @Purpose Disable schan override
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int
soc_schan_override_disable(int unit)
{
    if (unit < 0 || unit >= SOC_MAX_NUM_DEVICES) {
        return SOC_E_UNIT;
    }
    if (SOC_CONTROL(unit)->schan_wb_thread_id == sal_thread_self()) {
        SOC_CONTROL(unit)->schan_wb_thread_id = SAL_THREAD_ERROR;
        if (sal_mutex_give(SOC_CONTROL(unit)->schan_wb_mutex)) {
            LOG_ERROR(BSL_LS_SOC_SCHAN,
                      (BSL_META_U(unit,
                                  "Failed to release schan_wb_mutex.\n")));
            return SOC_E_INTERNAL;
        }
    }

    return SOC_E_NONE;
}

/*******************************************
* @function soc_schan_op
* purpose cmic SCHAN operation
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
* @comments  This calls appropriate function based on CMIC type
* Writes a message of dwc_write DWORDs from msg to the S-Channel, waits
* for the operation to complete, then reads dwc_read DWORDs from the
* channel into msg.  If there is no return data, use dwc_read = 0.
*
* On platforms where it is appropriate, the S-Channel is locked during
* the operation to prevent multiple tasks trying to use the S-Channel
* simultaneously.
*
* Return value is negative on error, 0 on success.
*
* If intr is true, this routine goes to sleep until an S-Channel
* completion interrupt wakes it up.  Otherwise, it polls for the done
* bit.  NOTE: if schanIntrEnb is false, intr is always overridden to 0.
*
* Polling is more CPU efficient for most operations since they complete
* much faster than the interrupt processing time would take.  However,
* due to the chip design, some operations such as ARL insert and delete
* may have unbounded response time.  In this case, the interrupt should
* be used.  The worst case we have seen is 1 millisec when the switch
* is passing max traffic with random addresses at min packet size.
*
* @end
 */
int
soc_schan_op(int unit,
             schan_msg_t *msg,
             int dwc_write, int dwc_read,
             uint32 flags)
{
    int rv;

    if(IS_COMPOSITE(unit) && (soc_feature(unit, soc_feature_sbus_format_v4)))
    {
        /* for composite devices, the ecode field was used to store the sub device index, get that info and clear the ecode field */
        int sub_unit_index = msg->header.v4.ecode;
        unit = sub_unit_index;
        msg->header.v4.ecode = 0;
    }

    if (_soc_schan_drv[unit].soc_schan_op == NULL) {
        LOG_FATAL(BSL_LS_SOC_SCHAN, (BSL_META_U(unit,
                  "SOC_SCHAN_OP() function is undefined\n")));
        rv = SOC_E_FAIL;
    } else {
        rv =  _soc_schan_drv[unit].soc_schan_op(unit, msg, dwc_write, dwc_read, flags);
    }

    return rv;
}

/*******************************************
* @function _soc_schan_check_hw_access_disabled
* @Purpose return TRUE if hardware access is disabled
*
* @param unit [in] unit
* @param msg [in] pointer to Type <schan_msg_t>
* @param dwc_read [out] pointer to status
*
* @returns TRUE or FALSE
*
* @comments If hardware access is disabled, this
* should return TRUE and set *rv to an error code that
* will be passed up to the caller.  Otherwise, return FALSE
* and do not alter *rv.

* @end
 */
STATIC int
_soc_schan_check_hw_access_disabled(int unit, schan_msg_t *msg, int *rv)
{
    if (SOC_HW_ACCESS_DISABLE(unit)) {
        switch (msg->header.v2.opcode) {
        case WRITE_MEMORY_CMD_MSG:
            *rv = SOC_E_NONE;
            return TRUE;
        case WRITE_REGISTER_CMD_MSG:
            msg->header.v2.opcode = WRITE_REGISTER_ACK_MSG;
            *rv = SOC_E_NONE;
            return TRUE;
        case ARL_INSERT_CMD_MSG:
            msg->header.v2.opcode = ARL_INSERT_DONE_MSG;
            *rv = SOC_E_NONE;
            return TRUE;
        case ARL_DELETE_CMD_MSG:
            msg->header.v2.opcode = ARL_DELETE_DONE_MSG;
            *rv = SOC_E_NONE;
            return TRUE;
        case L3_INSERT_CMD_MSG:
            msg->header.v2.opcode = L3_INSERT_DONE_MSG;
            *rv = SOC_E_NONE;
            return TRUE;
        case L3_DELETE_CMD_MSG:
            msg->header.v2.opcode = L3_DELETE_DONE_MSG;
            *rv = SOC_E_NONE;
            return TRUE;
        case INIT_CFAP_MSG:
            *rv = SOC_E_NONE;
            return TRUE;
        case READ_REGISTER_CMD_MSG:
            if (SOC_IS_DETACHING(unit)) {
                sal_memset(msg->bytes, 0,
                           sizeof(uint32) * CMIC_SCHAN_WORDS_ALLOC);
                msg->header.v2.opcode = READ_REGISTER_ACK_MSG;
                *rv = SOC_E_NONE;
                return TRUE;
            }
            break;
        case READ_MEMORY_CMD_MSG:
            if (SOC_IS_DETACHING(unit)) {
                sal_memset(msg->bytes, 0,
                           sizeof(uint32) * CMIC_SCHAN_WORDS_ALLOC);
                msg->header.v2.opcode = READ_MEMORY_ACK_MSG;
                *rv = SOC_E_NONE;
                return TRUE;
            }
            break;
        case L2_LOOKUP_CMD_MSG:
            if (SOC_IS_DETACHING(unit)) {
                msg->header.v2.opcode = L2_LOOKUP_ACK_MSG;
                *rv = SOC_E_NOT_FOUND;
                return TRUE;
            }
            break;
        case L3X2_LOOKUP_CMD_MSG:
            if (SOC_IS_DETACHING(unit)) {
                msg->header.v2.opcode = L3X2_LOOKUP_ACK_MSG;
                *rv = SOC_E_NOT_FOUND;
                return TRUE;
            }
            break;
        case ARL_LOOKUP_CMD_MSG:
        case L3_LOOKUP_CMD_MSG:
            if (SOC_IS_DETACHING(unit)) {
                *rv = SOC_E_NOT_FOUND;
                return TRUE;
            }
            break;
        case TABLE_INSERT_CMD_MSG:
            msg->header.v2.opcode = TABLE_INSERT_DONE_MSG;
            *rv = SOC_E_NONE;
            return TRUE;
        case TABLE_DELETE_CMD_MSG:
            msg->header.v2.opcode = TABLE_DELETE_DONE_MSG;
            *rv = SOC_E_NONE;
            return TRUE;
        case FIFO_POP_CMD_MSG:
            msg->header.v2.opcode = FIFO_POP_DONE_MSG;
            *rv = SOC_E_NONE;
            return TRUE;
        case FIFO_PUSH_CMD_MSG:
            msg->header.v2.opcode = FIFO_PUSH_DONE_MSG;
            *rv = SOC_E_NONE;
            return TRUE;
        default:
            break;
        }
    }

    return FALSE;
}

/*******************************************
* @function _soc_schan_op_arad_sanity_check
* purpose API to perform sanity check for ARAD
*
* @param unit [in] unit
* @param msg [in] pointer to Type <schan_msg_t>
* @param dwc_write [in] write
* @param dwc_read [in] read
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
* @end
 */

/*******************************************
* @function soc_schan_op_sanity_check
* purpose API to perform sanity checks
*
* @param unit [in] unit
* @param msg [in] pointer to Type <schan_msg_t>
* @param dwc_write [in] write
* @param dwc_read [in] read
* @param dwc_read [out] pointer to status
*
* @returns TRUE if the caller should abort the call and return *rv up the stack.
* @returns FALSE if the caller should continue as normal.
*
* @comments First, check that writing is allowed:
*    If (device is ARAD) and (opcode is not a READ command) and (override flag is off)
*    Then: writing is not allowed.
*
* Then, assert sanity checks.
* Finally, log the message that schan access is beginning
*
* @end
 */
int
soc_schan_op_sanity_check(int unit, schan_msg_t *msg, int dwc_write,
                           int dwc_read, int *rv)
{
    *rv = SOC_E_NONE;

    assert(! sal_int_context());
    assert(dwc_write <= CMIC_SCHAN_WORDS(unit));
    assert(dwc_read <= CMIC_SCHAN_WORDS(unit));

    /* don't perform check for DNX devices as they undergo different check */
    if (!SOC_IS_DNX(unit) && !SOC_IS_DNXF(unit)) {
        if (_soc_schan_check_hw_access_disabled(unit, msg, rv) == TRUE) {
            return TRUE;
        }
    }

    if (LOG_CHECK(BSL_LS_SOC_SCHAN | BSL_VERBOSE)) {
        LOG_VERBOSE(BSL_LS_SOC_SCHAN,
            (BSL_META_U(unit,
                            "S-CHANNEL %s: (unit %d)\n"),
                 soc_schan_op_name(msg->header.v2.opcode), unit));
        soc_schan_dump(unit, msg, dwc_write);
    }

    return FALSE;
}

#if defined(PLISIM) && defined(PLISIM_DIRECT_SCHAN)

/*******************************************
* @function _soc_schan_op_wrapper_plibde
* purpose  this is called with plisim enabled
*
* @param unit [in] unit
* @param msg [in] schan_msg_t
* @param dwc_write [in] Write messages
* @param dwc_write [in] read messages
* @param flags [in] use interrupt
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_soc_schan_op_wrapper_plibde(int unit, schan_msg_t *msg, int dwc_write,
                            int dwc_read, uint32 flags)
{
    int rv;

    if (soc_schan_op_sanity_check(unit, msg, dwc_write, dwc_read, &rv) == TRUE) {
        return rv;
    }
    /* Back door into the simulation to perform direct SCHANNEL operations */
    if (LOG_CHECK(BSL_LS_SOC_SCHAN | BSL_VERBOSE)) {
        LOG_VERBOSE(BSL_LS_SOC_SCHAN,
            (BSL_META_U(unit,
                            "S-CHANNEL %s: (unit %d)\n"),
                 soc_schan_op_name(msg->header.v2.opcode), unit));
        soc_schan_dump(unit, msg, dwc_write);
    }

    rv = plibde_schan_op(unit, msg, dwc_write, dwc_read);

    if (LOG_CHECK(BSL_LS_SOC_SCHAN | BSL_VERBOSE)) {
        soc_schan_dump(unit, msg, dwc_read);
    }

    return rv;
}
#endif

#ifdef INCLUDE_RCPU

/*******************************************
* @function _soc_schan_op_wrapper_rcpu
* purpose  Called if rcpu schan callback is registered
*
* @param unit [in] unit
* @param msg [in] schan_msg_t
* @param dwc_write [in] Write messages
* @param dwc_write [in] read messages
* @param flags [in] use interrupt
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_soc_schan_op_wrapper_rcpu(int unit, schan_msg_t *msg, int dwc_write,
                          int dwc_read, uint32 flags)
{
    if (SOC_CONTROL(unit)->soc_rcpu_schan_op == NULL) {
        LOG_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit,
                  "No soc_rcpu_schan_op callback defined\n")));
        return SOC_E_FAIL;
    }

    return SOC_CONTROL(unit)->soc_rcpu_schan_op(unit, msg, dwc_write, dwc_read);
}
#endif /* INCLUDE_RCPU */


/*******************************************
* @function soc_schan_timeout_check
* @Purpose enable special processing for timeout
*
* @param unit [in] unit
* @param msg [in] pointer to Type <schan_msg_t>
* @param dwc_read [out] pointer to status
*
* @returns TRUE or FALSE
*
* @end
 */
int
soc_schan_timeout_check(int unit, int *rv, schan_msg_t *msg, int cmc, int ch, uint32 flag)
{
    if (*rv != SOC_E_TIMEOUT) {
        SOC_CONTROL(unit)->stat.schan_op++;
        return FALSE;
    }

    if (SOC_IS_TRIUMPH3(unit) && msg->header.v3.opcode == WRITE_REGISTER_CMD_MSG) {
        if ((msg->writecmd.address == 0x02029700) &&
            (msg->writecmd.header.v3.dst_blk >= 0x8  && msg->writecmd.header.v3.dst_blk <= 0xf)) {
            *rv = SOC_E_NONE;
             SOC_CONTROL(unit)->stat.schan_op++;
        }
    } else if ((flag & SCHAN_OP_FLAG_DO_NOT_LOG_TIMEOUTS) == 0) {
        LOG_WARN(BSL_LS_SOC_SCHAN,
            (BSL_META_U(unit,
                "soc_schan_op: operation attempt timed out\n")));
        SOC_CONTROL(unit)->stat.err_sc_tmo++;
        if (_soc_schan_drv[unit].soc_schan_reset != NULL) {
            _soc_schan_drv[unit].soc_schan_reset(unit, cmc, ch);
        }
        return TRUE;
    }

    return FALSE;
}

/*******************************************
* @function _soc_schan_control_init
* purpose API to Initialize and SCHAN control
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int _soc_schan_control_init(int unit)
{
    int rv = SOC_E_NONE;
    int cmc;

    SOC_SCHAN_CONTROL(unit).schanTimeout = &SOC_CONTROL(unit)->schanTimeout;
    SOC_SCHAN_CONTROL(unit).schanIntrEnb = &SOC_CONTROL(unit)->schanIntrEnb;
    for (cmc = 0; cmc <= SOC_CMCS_NUM_MAX; cmc++) {
        SOC_SCHAN_CONTROL(unit).schanIntr[cmc] = &SOC_CONTROL(unit)->schanIntr[cmc];
        SOC_SCHAN_CONTROL(unit).schan_result[cmc] = &SOC_CONTROL(unit)->schan_result[cmc];
    }

    return rv;
}
/*******************************************
* @function soc_schan_init
* purpose API to Initialize and config SCHAN
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int soc_schan_init(int unit)
{
    int rv = SOC_E_NONE;

    SOC_IF_ERROR_RETURN(_soc_schan_control_init(unit));

#if defined(PLISIM) && defined(PLISIM_DIRECT_SCHAN)
    if ((SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM)) {
        _soc_schan_drv[unit].soc_schan_op = _soc_schan_op_wrapper_plibde;
        return rv;
    }
#endif

#ifdef INCLUDE_RCPU
    if (SOC_IS_RCPU_ONLY(unit) || SOC_IS_RCPU_UNIT(unit)) {
       _soc_schan_drv[unit].soc_schan_op = _soc_schan_op_wrapper_rcpu;
       return rv;
    }
#endif

#ifdef BCM_CMICX_SUPPORT
   if (soc_feature(unit, soc_feature_cmicx)) {
       rv = soc_cmicx_schan_init(unit, &_soc_schan_drv[unit]);
       return rv;
   }
#endif

#if defined(BCM_CMICM_SUPPORT)
    if (soc_feature(unit, soc_feature_cmicm)) {
        rv = soc_cmicm_schan_init(unit, &_soc_schan_drv[unit]);
        return rv;
    }
#endif


   rv = soc_cmice_schan_init(unit, &_soc_schan_drv[unit]);


    return rv;
}

/*******************************************
* @function soc_schan_deinit
* purpose SDK-7 API to de-Initialize SCHAN
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int soc_schan_deinit(int unit)
{
    int rv = SOC_E_NONE;

    if (_soc_schan_drv[unit].soc_schan_deinit) {
        rv = _soc_schan_drv[unit].soc_schan_deinit(unit);
    }

    _soc_schan_drv[unit].soc_schan_deinit = NULL;
    _soc_schan_drv[unit].soc_schan_op = NULL;
    _soc_schan_drv[unit].soc_schan_reset = NULL;

    return rv;
}
/*
 * Procedures related to PEM block access.
 * {
 */
/*
 * phy_mem_read
 * phy_mem_write
 *
 * Send/ (rcv) msg to/(from) the S-Channel to carry out PEM operation
 * as per '*mem' and '*entry_data'.
 * Wait for interrupt to indicate operation to complete. For 'read',
 * response data is on '*entry_data'.
 *
 * Access memory or register depending on the LS 16 bis of mem->mem_address
 * (non-zero for 'memory').
 *
 * S-Channel is locked during the operation to prevent multiple tasks
 * trying to use the S-Channel simultaneously.
 *
 * Return value is negative on error, 0 (SOC_E_NONE) on success.
 *
 * NOTE: If SOC_CONTROL(unit)->schanIntrEnb is false, interrupt operation
 * is overridden in favour of polling.
 *
 * NOTE: Within 'header', this procedure uses header.v4
 *
 * Ensure that a SOC_CONTROL(unit)->schan_op function pointer is defined for this
 * unit. If not, an error is flagged.
 *
 * See also:
 *   soc_schan_op(), _soc_schan_op_cmicm(), soc_direct_memreg_get(),
 *   soc_direct_memreg_set(), schan_header_t
 */
int phy_mem_read(int unit, phy_mem_t *mem, void *entry_data)
{
  int ret ;
  int is_mem ;
  uint32 addr, data_longs_to_read ;
  uint32 mask = 0xFFFF0000 ;
  /*
   * Access memory or register depending on the LS 16 bis.
   */
  is_mem = ( (mem->mem_address & mask) != 0 ) ? 1 : 0;
  /*
   * Always request an integer number of longs (expressed in bytes).
   */
  data_longs_to_read = BITS2WORDS(mem->mem_width_in_bits) ;
  /*
    * row_index was removed from phy_mem_t (see SDK-111156).
    * Need updated ucode.
    */
  addr = mem->mem_address;

  ret = soc_direct_memreg_get(unit, mem->block_identifier, addr, data_longs_to_read,is_mem, (uint32 *)entry_data) ;
  return (ret) ;
}
int phy_mem_write(int unit, phy_mem_t *mem, void *entry_data)
{
  int ret ;
  int is_mem ;
  uint32 addr, data_longs_to_write ;
  uint32 mask = 0xFFFF0000 ;
  /*
   * Access memory or register depending on the LS 16 bis.
   */
  is_mem = ( (mem->mem_address & mask) != 0 ) ? 1 : 0;
  /*
   * Always request an integer number of longs (expressed in bytes).
   */
  data_longs_to_write = BITS2WORDS(mem->mem_width_in_bits) ;
  /*
    * row_index was removed from phy_mem_t (see SDK-111156).
    * Need updated ucode.
    */
  addr = mem->mem_address;
  ret = soc_direct_memreg_set(unit, mem->block_identifier, addr, data_longs_to_write, is_mem, (uint32 *)entry_data) ;
  return (ret) ;
}

/*
 * }
 */
