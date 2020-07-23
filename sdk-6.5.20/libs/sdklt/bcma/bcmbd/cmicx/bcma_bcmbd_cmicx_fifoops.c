/*! \file bcma_bcmbd_cmicx_fifoops.c
 *
 * This file provides the FIFO operation for CMICx devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_assert.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmbd/bcmbd_cmicx_fifo.h>

#include <bcma/bcmbd/bcma_bcmbd_symlog.h>
#include <bcma/bcmbd/bcma_bcmbd_cmicx.h>

/*******************************************************************************
 * Local definitions
 */

/* FIFO operations: pop or push. */
typedef enum cmicx_fifo_op_s {
    CMICX_FIFO_OP_POP,
    CMICX_FIFO_OP_PUSH,
    CMICX_FIFO_OP_COUNT
} cmicx_fifo_op_t;

/*******************************************************************************
 * Private functions
 */

/*
 * Display memory information
 */
static void
print_mem_op(shr_pb_t *pb, int unit, const char *prefix, bcma_bcmbd_id_t *sid,
             int a, int b, int p, int t, int sect_size,
             uint32_t adext, uint32_t addr,
             const char *errstr, int rc)
{
    char blockname[32];
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    assert(cinfo);

    if (rc < 0) {
        shr_pb_printf(pb, "%s%s ", BCMA_CLI_CONFIG_ERROR_STR, errstr);
    }

    shr_pb_printf(pb, "%s %s", prefix, sid->addr.name);
    if (t >= 0) {
        shr_pb_printf(pb, "{%d}", t);
    }
    if (a >= 0) {
        /* Check if memory index contains base_type information */
        if (p >= 0 && sect_size > 0) {
            int midx = a - p * sect_size;

            if (midx >= 0) {
                shr_pb_printf(pb, "[%d]", midx);
            } else {
                shr_pb_printf(pb, "[%d]", a);
            }
        } else {
            shr_pb_printf(pb, "[%d]", a);
        }
    }
    if (b >= 0) {
        shr_pb_printf(pb, ".%s", bcma_bcmbd_block_name(unit, b, blockname,
                                                       sizeof(blockname)));
    }
    if (p >= 0) {
        if (sect_size > 0) {
            /* Memory */

            /* BASE_TYPE index */
            shr_pb_printf(pb, ".%d", p);
        }
    }
    shr_pb_printf(pb, " [0x%04"PRIx32"%08"PRIx32"]", adext, addr);

    if (rc < 0) {
        shr_pb_printf(pb, " %s (%d)", shr_errmsg(rc), rc);
    }
}

/*
 * Check whether the baseidx is valid in current pipe bitmap.
 */
static bool
skip_single_acc_inst(int unit, uint32_t blk_pipe_bmp,
                     uint32_t offset, int acctype, int blktype, int baseidx)
{
    bcmdrd_pipe_info_t pipe_info, *pi = &pipe_info;
    uint32_t inst_mask;
    bcmdrd_pipe_info_type_t type = BCMDRD_PIPE_INFO_TYPE_BASETYPE_SINGLE_PMASK;

    sal_memset(pi, 0, sizeof(*pi));
    pi->offset = offset;
    pi->acctype = acctype;
    pi->blktype = blktype;
    pi->baseidx = baseidx;
    inst_mask = bcmdrd_dev_pipe_info(unit, pi, type);
    if (inst_mask != 0 && (inst_mask & blk_pipe_bmp) == 0) {
        return true;
    }
    return false;
}

/*
 * Perform FIFO pop operation and display necessary information.
 */
static int
cmicx_cmd_pop(shr_pb_t *pb, int unit, const bcmdrd_symbol_t *symbol,
                bcma_bcmbd_id_t *sid,
                int block, int mindex, int pipe, int basetype, int sect_size,
                uint32_t size)
{
    int rc;
    int idx;
    uint32_t adext, i, addr;
    uint32_t data[BCMDRD_MAX_PT_WSIZE];

    if (size > COUNTOF(data)) {
        shr_pb_printf(pb, "%sentity size (%"PRIx32" words) too big\n",
                      BCMA_CLI_CONFIG_ERROR_STR, size);
        return -1;
    }

    /* For raw access, memory index may be unspecified */
    idx = mindex;
    if (idx < 0) {
        idx = 0;
    }

    /* Default address */
    addr = sid->addr.name32;
    adext = sid->addr.ext32;

    if (pipe >= 0) {
        BCMBD_CMICX_ADEXT_ACCTYPE_SET(adext, pipe);
    }

    /* Calculate the absolute address for this memory read */
    if (block > 0) {
        /* Update address extension with specified block */
        BCMBD_CMICX_ADEXT_BLOCK_SET(adext, block);
        addr = bcmbd_block_port_addr(unit, block, -1, addr, -1);
    }

    /* Read the data */
    /* coverity[overrun-call] */
    rc = bcmbd_cmicx_fifo_pop(unit, adext, addr + idx, data, size);

    /* Log the operation. */
    bcma_bcmbd_symlog_set(unit, adext, addr, BCMDRD_SYMBOL_FLAG_FIFO,
                          BCMLT_PT_OPCODE_FIFO_POP, rc);

    /* If the read failed, output an error */
    if (rc < 0) {
        print_mem_op(pb, unit, "memory", sid, mindex, block, basetype,
                     pipe, sect_size, adext, addr + idx, "pop", rc);
        shr_pb_printf(pb, "\n");
        return rc;
    }

    if (sid->flags & BCMA_BCMBD_CMD_IDF_NONZERO) {
        for (i = 0; i < size; i++) {
            if (data[i]) break;
        }
        if (i >= size) return 0;
    }

    /* Print out the memory data */
    print_mem_op(pb, unit, "memory", sid, mindex, block, basetype,
                 pipe, sect_size, adext, addr + idx, "pop", rc);
    shr_pb_printf(pb, " =");
    for (i = 0; i < size; i++) {
        shr_pb_printf(pb, " 0x%08"PRIx32, data[i]);
    }
    if ((sid->flags & (BCMA_BCMBD_CMD_IDF_COMPACT | BCMA_BCMBD_CMD_IDF_RAW)) ==
        BCMA_BCMBD_CMD_IDF_COMPACT) {
        shr_pb_printf(pb, "; ");
    } else {
        shr_pb_printf(pb, "\n");
    }

    /* Decode the individual fields if they are available */
#if BCMDRD_CONFIG_INCLUDE_FIELD_INFO == 1
    if (sid->flags & BCMA_BCMBD_CMD_IDF_RAW) {
        return 0;
    }
    if (symbol && symbol->fields) {
        /* Decode the result */
        bcma_bcmbd_dump_fields(pb, unit, symbol, data, sid->flags,
                               sid->ccb, sid->ccb_cookie);
    }
#endif

    return 0;
}

/*
 * Perform FIFO push operation and display necessary information.
 */
static int
cmicx_cmd_push(shr_pb_t *pb, int unit, const bcmdrd_symbol_t *symbol,
               bcma_bcmbd_id_t *sid,
               int block, int mindex, int pipe, int basetype, int sect_size,
               uint32_t *data, uint32_t size)
{
    int rc;
    int idx;
    uint32_t adext, addr;

    /* For raw access, memory index may be unspecified */
    idx = mindex;
    if (idx < 0) {
        idx = 0;
    }

    /* Default address */
    addr = sid->addr.name32;
    adext = sid->addr.ext32;

    if (pipe >= 0) {
        BCMBD_CMICX_ADEXT_ACCTYPE_SET(adext, pipe);
    }

    /* Calculate the absolute address for this memory read */
    if (block > 0) {
        /* Update address extension with specified block */
        BCMBD_CMICX_ADEXT_BLOCK_SET(adext, block);
        addr = bcmbd_block_port_addr(unit, block, -1, addr, -1);
    }

    /* Push the data */
    rc = bcmbd_cmicx_fifo_push(unit, adext, addr + idx, data, size);

    /* Log the operation. */
    bcma_bcmbd_symlog_set(unit, adext, addr, BCMDRD_SYMBOL_FLAG_FIFO,
                          BCMLT_PT_OPCODE_FIFO_PUSH, rc);

    /* If the read failed, output an error */
    if (rc < 0) {
        print_mem_op(pb, unit, "memory", sid, mindex, block, basetype,
                     pipe, sect_size, adext, addr + idx, "push", rc);
        shr_pb_printf(pb, "\n");
        return rc;
    }

    return 0;
}

/* Iterate the symbol identifier descriptor to perform FIFO operations */
static int
cmicx_fifo_ops(int unit, cmicx_fifo_op_t op, const bcmdrd_symbol_t *symbol,
           bcmdrd_sid_t enum_val, bcma_bcmbd_id_t *sid,
           uint32_t *data, uint32_t size)
{
    int b, bt = -1, idx, t, p;
    uint32_t pipe_info = 0, pmask = 0, sect_size = 0;
    int linst = 0, acc_type = -1;
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);
    uint32_t blk_pipe_bmp = 0;
    shr_pb_t *pb;

    pb = shr_pb_create();

    for (b = sid->block.start; b <= sid->block.end; b++) {
        if (b != -1) {
            /* If this is not a valid block number for the chip, skip it */
            if (bcmdrd_chip_block_type(cinfo, b, &bt, NULL) < 0) {
                continue;
            }

            /* Check that block type is valid for this symbol */
            if (symbol &&
                !bcma_bcmbd_info_block_type_valid(unit, bt, symbol->info)) {
                continue;
            }

            /* Check that block type is valid for this operation */
            if (sid->block.ext32 && (sid->block.ext32 & (1 << bt)) == 0) {
                continue;
            }

            if (symbol) {
                acc_type = BCMBD_CMICX_BLKACC2ACCTYPE(symbol->info);
                pipe_info = bcmbd_cmicx_pipe_info(unit, sid->addr.name32,
                                                  acc_type, bt, -1);
                (void)bcmdrd_dev_valid_blk_pipes_get(unit, 0, bt,
                                                     &blk_pipe_bmp);
            }
            pmask = BCMBD_CMICX_PIPE_PMASK(pipe_info);
            linst = BCMBD_CMICX_PIPE_LINST(pipe_info);
            sect_size = BCMBD_CMICX_PIPE_SECT_SIZE(pipe_info);
        }

        for (t = sid->pipe.start; t <= sid->pipe.end; t++) {
            /* Loop for base type (if applicable) */
            for (p = sid->port.start; p <= sid->port.end; p++){
                int pipe_offset = 0;

                if (t != -1) {
                    /* If this is not a valid pipe value, skip it */
                    if (!((1 << t) & pmask)) {
                        continue;
                    }
                }

                if (symbol && linst && p != -1 && bt != -1) {
                    uint32_t bpm;

                    /* Check if index for BASE_TYPE is valid */
                    if (p >= linst) {
                        continue;
                    }
                    if (skip_single_acc_inst(unit, blk_pipe_bmp,
                                             sid->addr.name32,
                                             acc_type, bt, p)) {
                        continue;
                    }

                    /* combination of unique block and basetype */
                    bpm = bcmbd_cmicx_pipe_info(unit, sid->addr.name32,
                                                acc_type, bt, p);
                    if (t >= 0 && bpm && (bpm & (1 << t)) == 0) {
                        continue;
                    }
                }

                /* Check for BASE_TYPE for memory section size */
                if (p >= 0) {
                    pipe_offset = p * sect_size;
                }

                for (idx = sid->addr.start; idx <= sid->addr.end; idx++) {
                    if (op == CMICX_FIFO_OP_POP) {
                        cmicx_cmd_pop(pb, unit, symbol, sid, b, idx + pipe_offset,
                                      t, p, sect_size, size);
                    } else if (op == CMICX_FIFO_OP_PUSH) {
                        cmicx_cmd_push(pb, unit, symbol, sid, b, idx + pipe_offset,
                                      t, p, sect_size, data, size);
                    }
                    cli_out("%s", shr_pb_str(pb));
                    shr_pb_mark(pb, 0);
                    shr_pb_reset(pb);
                }
            }
        }
    }
    shr_pb_destroy(pb);

    return 0;
}

/*
 * Perform symbol FIFO operation via S-channel.
 */
static int
fifo_symop(int unit, cmicx_fifo_op_t op, const bcmdrd_symbol_t *symbol,
           bcmdrd_sid_t enum_val, bcma_bcmbd_id_t *sid,
           uint32_t *data, uint32_t size)
{
    int b, match_block;
    int linst;
    bcma_bcmbd_id_t sid2;
    const bcmdrd_block_t *blkp;
    int blktype, acc_type;
    uint32_t wsize, pipe_info;
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    assert(symbol);
    assert(cinfo);

    /* Did the user specify indices? */
    if (sid->addr.start == -1) {
        /* No indices specified, operate on one entry for the memory */
        sid->addr.start = 0;
        sid->addr.end = 0;
    }

    /*
     * Iterate through all blocks of which this memory is a member
     */
    for (blktype = 0; blktype < cinfo->nblktypes; blktype++) {

        if (!bcma_bcmbd_info_block_type_valid(unit, blktype, symbol->info)) {
            continue;
        }

        /* Need a copy of this SID for this block iteration */
        sal_memcpy(&sid2, sid, sizeof(sid2));

        /*
         * Set block type filter in case identical block types are
         * not contiguous.
         */
        sid2.block.ext32 = (1 << blktype);

        acc_type = BCMBD_CMICX_BLKACC2ACCTYPE(symbol->info);
        pipe_info = bcmbd_cmicx_pipe_info(unit, sid->addr.name32,
                                          acc_type, blktype, -1);

        match_block = 0;

        for (b = 0; b < cinfo->nblocks; b++) {

            /* Get the block pointer for this block */
            blkp = cinfo->blocks + b;
            assert(blkp);

            if (blkp->type != blktype){
                continue;
            }

            /* Setup pipe info */
            if (!sid2.pipe.valid) {
                if (pipe_info > 0) {
                    int num_pipe = 0;
                    int pmask = BCMBD_CMICX_PIPE_PMASK(pipe_info);

                    if (pmask > 0) {
                        while (pmask) {
                            num_pipe++;
                            pmask >>= 1;
                        };
                        sid2.pipe.start = 0;
                        sid2.pipe.end = num_pipe - 1;
                    }
                }
            } else {
                if (pipe_info == 0) {
                    sid2.pipe.start = -1;
                    sid2.pipe.end = -1;
                }
            }

            /* Does the SID contain a block specifier? */
            if (!sid2.block.valid) {
                /* If no specific blocks were specified, add all blocks of this type */
                if (sid2.block.start == -1 || sid2.block.end < blkp->blknum) {
                    sid2.block.end = blkp->blknum;
                }
                if (sid2.block.start == -1 || sid2.block.start > blkp->blknum) {
                    sid2.block.start = blkp->blknum;
                }
            }
            else {
                /* User specified a block identifier */
                /* does the block match this one? */
                if (sal_strcmp(sid2.block.name,
                               bcma_bcmbd_block_type2name(unit, blktype)) == 0) {
                    /* Block specifier matches */
                    match_block = 1;
                    /* If start and stop were omitted, then we need to put them in */
                    if (sid->block.start == -1) {
                        /* Add all blocks of this type */
                        if (sid2.block.start == -1 || sid2.block.end < blkp->blknum) {
                            sid2.block.end = blkp->blknum;
                        }
                        if (sid2.block.start == -1 || sid2.block.start > blkp->blknum) {
                            sid2.block.start = blkp->blknum;
                        }
                    }
                    else {
                        /* specific blocks were indicated. */
                        /* Need to convert these to physical blocks */
                        sid2.block.start = bcmdrd_chip_block_number(cinfo, blktype,
                                                                    sid->block.start);
                        sid2.block.end = bcmdrd_chip_block_number(cinfo, blktype,
                                                                  sid->block.end);
                    }
                }
                else {
                    /* Block specified does not match this one. */
                    /* I guess we're done */
                    continue;
                }
            }
        }

        /* Does the specified block match? */
        if (sid2.block.valid && !match_block) {
            continue;
        }

        /* We don't handle port numbers on memories */
        assert((symbol->flags & BCMDRD_SYMBOL_FLAG_PORT) == 0);

        /* Skip if we don't have a valid block range by now */
        if (sid2.block.start == -1) {
            continue;
        }

        /* Store Base type information in port */
        linst = BCMBD_CMICX_PIPE_LINST(pipe_info);
        if (linst > 0) {
            if (sid2.port.start == -1) {
                sid2.port.start = 0;
            }
            if (sid2.port.end == -1 || sid2.port.end >= linst) {
                sid2.port.end = linst - 1;
            }
            if (sid2.port.start > sid2.port.end) {
                sid2.port.start = sid2.port.end;
            }
        } else {
            sid2.port.start = -1;
            sid2.port.end = -1;
        }

        /* Get dual-pipe access type from symbol flags */
        sid2.addr.ext32 = BCMBD_CMICX_BLKACC2ADEXT(symbol->info);

        wsize = BCMDRD_BYTES2WORDS(BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index));
        if (op == CMICX_FIFO_OP_PUSH && size < wsize) {
            wsize = size;
        }
        cmicx_fifo_ops(unit, op, symbol, enum_val, &sid2, data, wsize);
    }

    return 0;
}

/*******************************************************************************
 * Exported functions
 */

int
bcma_bcmbd_cmicx_fifo_pop(int unit,
                          const bcmdrd_symbol_t *symbol, bcmdrd_sid_t enum_val,
                          bcma_bcmbd_id_t *sid)
{
    if (!(symbol->flags & BCMDRD_SYMBOL_FLAG_FIFO)) {
        cli_out("%ssymbol '%s' is not valid for FIFO operation\n",
                BCMA_CLI_CONFIG_ERROR_STR, symbol->name);
        return -1;
    }

    return fifo_symop(unit, CMICX_FIFO_OP_POP, symbol, enum_val, sid, NULL, 0);
}

int
bcma_bcmbd_cmicx_fifo_push(int unit,
                           const bcmdrd_symbol_t *symbol, bcmdrd_sid_t enum_val,
                           bcma_bcmbd_id_t *sid, uint32_t *data, uint32_t size)
{
    if (!(symbol->flags & BCMDRD_SYMBOL_FLAG_FIFO)) {
        cli_out("%ssymbol '%s' is not valid for FIFO operation\n",
                BCMA_CLI_CONFIG_ERROR_STR, symbol->name);
        return -1;
    }

    return fifo_symop(unit, CMICX_FIFO_OP_PUSH,
                      symbol, enum_val, sid, data, size);
}
