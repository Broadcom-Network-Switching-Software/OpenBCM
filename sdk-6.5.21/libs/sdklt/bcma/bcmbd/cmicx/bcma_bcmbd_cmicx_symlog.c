/*! \file bcma_bcmbd_cmicx_symlog.c
 *
 * This file provides the symlog utilities for CMICx devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_types.h>

#include <bcmbd/bcmbd_cmicx.h>

#include <bcma/bcmbd/bcma_bcmbd_cmicx.h>

/*******************************************************************************
 * Local defines
 */

#define CMICX_SCHAN_CMD_HDR_WSIZE 2

#define SCMH_ACCTYPE_GET(d)     F32_GET(d,14,5)
#define SCMH_DSTBLK_GET(d)      F32_GET(d,19,7)
#define SCMH_OPCODE_GET(d)      F32_GET(d,26,6)

#define READ_MEMORY_CMD_MSG     0x07
#define WRITE_MEMORY_CMD_MSG    0x09
#define READ_REGISTER_CMD_MSG   0x0b
#define WRITE_REGISTER_CMD_MSG  0x0d
#define TABLE_INSERT_CMD_MSG    0x24
#define TABLE_DELETE_CMD_MSG    0x26
#define TABLE_LOOKUP_CMD_MSG    0x28
#define FIFO_POP_CMD_MSG        0x2a
#define FIFO_PUSH_CMD_MSG       0x2c


/*******************************************************************************
 * Internal public functions
 */

int
bcma_bcmbd_cmicx_schan_cmd_decode(uint32_t *schan_cmd, uint32_t wsize,
                                  bcma_bcmbd_symlog_info_t *info)
{
    uint32_t opcode, dstblk;
    int acctype;

    if (wsize < CMICX_SCHAN_CMD_HDR_WSIZE) {
        return -1;
    }

    opcode = SCMH_OPCODE_GET(schan_cmd[0]);
    acctype = SCMH_ACCTYPE_GET(schan_cmd[0]);
    dstblk = SCMH_DSTBLK_GET(schan_cmd[0]);

    sal_memset(info, 0, sizeof(*info));
    BCMBD_CMICX_ADEXT_BLOCK_SET(info->adext, dstblk);
    BCMBD_CMICX_ADEXT_ACCTYPE_SET(info->adext, acctype);
    info->addr = schan_cmd[1];

    switch (opcode) {
        case READ_MEMORY_CMD_MSG:
            info->sym_flags = BCMDRD_SYMBOL_FLAG_MEMORY;
            info->op = BCMLT_PT_OPCODE_GET;
            break;
        case WRITE_MEMORY_CMD_MSG:
            info->sym_flags = BCMDRD_SYMBOL_FLAG_MEMORY;
            info->op = BCMLT_PT_OPCODE_SET;
            break;
        case READ_REGISTER_CMD_MSG:
            info->sym_flags = BCMDRD_SYMBOL_FLAG_REGISTER;
            info->op = BCMLT_PT_OPCODE_GET;
            break;
        case WRITE_REGISTER_CMD_MSG:
            info->sym_flags = BCMDRD_SYMBOL_FLAG_REGISTER;
            info->op = BCMLT_PT_OPCODE_SET;
            break;
        case TABLE_INSERT_CMD_MSG:
            info->sym_flags = BCMDRD_SYMBOL_FLAG_HASHED;
            info->op = BCMLT_PT_OPCODE_INSERT;
            break;
        case TABLE_DELETE_CMD_MSG:
            info->sym_flags = BCMDRD_SYMBOL_FLAG_HASHED;
            info->op = BCMLT_PT_OPCODE_DELETE;
            break;
        case TABLE_LOOKUP_CMD_MSG:
            info->sym_flags = BCMDRD_SYMBOL_FLAG_HASHED;
            info->op = BCMLT_PT_OPCODE_LOOKUP;
            break;
        case FIFO_POP_CMD_MSG:
            info->sym_flags = BCMDRD_SYMBOL_FLAG_FIFO;
            info->op = BCMLT_PT_OPCODE_FIFO_POP;
            break;
        case FIFO_PUSH_CMD_MSG:
            info->sym_flags = BCMDRD_SYMBOL_FLAG_FIFO;
            info->op = BCMLT_PT_OPCODE_FIFO_PUSH;
            break;
        default:
            return -1;
    }

    return 0;
}

