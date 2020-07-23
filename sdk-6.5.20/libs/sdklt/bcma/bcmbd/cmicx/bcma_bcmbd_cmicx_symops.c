/*! \file bcma_bcmbd_cmicx_symops.c
 *
 * Symbolic register/memory operations for CMICx devices.
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

#include <bcmbd/bcmbd_cmicx.h>

#include <bcma/bcmbd/bcma_bcmbd_symlog.h>
#include <bcma/bcmbd/bcma_bcmbd_cmicx.h>

/*******************************************************************************
 * Private functions
 */

/*
 * Get or set a CMIC register.
 */
static int
cmic_symop(int unit, const bcmdrd_symbol_t *symbol, bcmdrd_sid_t enum_val,
           bcma_bcmbd_id_t *sid, uint32_t *and_masks, uint32_t *or_masks)
{
    uint32_t addr, data, step;
    int idx, rv;
    shr_pb_t *pb;
    bool is_iproc;

    /* No port/block identifiers on cmic registers */
    if (sid->port.valid || sid->block.valid) {
        return bcma_cli_parse_error("cmic address", sid->id);
    }

    pb = shr_pb_create();

    /* Did the user specify indices? */
    if (sid->addr.start == -1) {
        /* No indices specified, insert limits for the memory */
        sid->addr.start = BCMDRD_SYMBOL_INDEX_MIN_GET(symbol->index);
        sid->addr.end = BCMDRD_SYMBOL_INDEX_MAX_GET(symbol->index);
    }

    step = BCMDRD_SYMBOL_INDEX_STEP_GET(symbol->index);
    is_iproc = (symbol->flags & BCMDRD_SYMBOL_FLAG_IPROC) ? true : false;

    for (idx = sid->addr.start; idx <= sid->addr.end; idx++) {

        /* Index 32 bit addresses */
        addr = symbol->addr + (idx * 4 * step);

        /* Read the data */
        if (is_iproc) {
            rv = BCMDRD_IPROC_READ(unit, addr, &data);
        } else {
            rv = BCMDRD_DEV_READ32(unit, addr, &data);
        }

        /* Log the operation. */
        bcma_bcmbd_symlog_cmic_set(unit, addr, symbol->flags,
                                   BCMLT_PT_OPCODE_GET, rv);

        /* This is a read-modify-write if masks are specified */
        if (and_masks || or_masks ) {
            if(and_masks) data &= and_masks[0];
            if(or_masks) data |= or_masks[0];

            /* Write the data */
            if (is_iproc) {
                rv = BCMDRD_IPROC_WRITE(unit, addr, data);
            } else {
                rv = BCMDRD_DEV_WRITE32(unit, addr, data);
            }

            /* Log the operation. */
            bcma_bcmbd_symlog_cmic_set(unit, addr, symbol->flags,
                                       BCMLT_PT_OPCODE_SET, rv);
        } else {
            if (sid->flags & BCMA_BCMBD_CMD_IDF_NONZERO) {
                if (data == 0) {
                    continue;
                }
            }

            if (sid->flags & BCMA_BCMBD_CMD_IDF_DIFF) {
                if (bcma_bcmbd_resetval_cmp(unit, symbol, 1, &data) == 0) {
                    continue;
                }
            }

            /* If we're here, it was a read operation and we should output the data */
            shr_pb_printf(pb, "%s", symbol->name);
            if (BCMDRD_SYMBOL_INDEX_MAX_GET(symbol->index) > 0) {
                shr_pb_printf(pb, "[%d]", idx);
            }
            shr_pb_printf(pb, ".cmic [0x%08"PRIx32"] = 0x%"PRIx32"\n",
                          addr, data);

            /* Output field data if it is available */
#if BCMDRD_CONFIG_INCLUDE_FIELD_INFO == 1
            if (symbol->fields && (sid->flags & BCMA_BCMBD_CMD_IDF_RAW) == 0) {
                bcma_bcmbd_dump_fields(pb, unit, symbol, &data, sid->flags,
                                       sid->ccb, sid->ccb_cookie);
            }
#endif
            cli_out("%s", shr_pb_str(pb));
            shr_pb_mark(pb, 0);
            shr_pb_reset(pb);
        }
    }

    shr_pb_destroy(pb);

    return 0;
}

/*
 * Get or set a chip register via S-channel.
 */
static int
reg_symop(int unit, const bcmdrd_symbol_t *symbol, bcmdrd_sid_t enum_val,
          bcma_bcmbd_id_t *sid, uint32_t *and_masks, uint32_t *or_masks)
{
    int port;
    int maxidx;
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    assert(symbol);
    assert(symbol->flags & BCMDRD_SYMBOL_FLAG_REGISTER);
    assert(cinfo);

    /* Did the user specify indices? */
    if (sid->addr.start == -1) {
        /* No indices specified, insert limits if any */
        maxidx = BCMDRD_SYMBOL_INDEX_MAX_GET(symbol->index);
        if (maxidx) {
            sid->addr.start = 0;
            sid->addr.end = maxidx;
        }
    }

    /*
     * If the user specified a port number, but not block, then the ports as
     * specified are not block-based. Since such top-level ports can span
     * different blocks, we will iterate over all of the top-level ports
     * specified, and handle each specific port within the loop below.
     */
    for (port = sid->port.start; port <= sid->port.end; port++) {

        int b, match_block, wsize;
        bcma_bcmbd_id_t sid2;
        int blktype, acc_type;
        const bcmdrd_block_t *blkp;
        uint32_t pipe_info;

        /*
         * Iterate through all blocks of this symbol
         */
        for (blktype = 0; blktype < cinfo->nblktypes; blktype++) {

            if (!bcma_bcmbd_info_block_type_valid(unit, blktype, symbol->info)) {
                continue;
            }

            /* Need a copy of the SID for this block iteration */
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

            /*
             * Iterate through all blocks in the chip
             */
            for (b = 0; b < cinfo->nblocks; b++) {

                /* Get the block pointer for this block */
                blkp = cinfo->blocks + b;
                assert(blkp);

                if (blkp->type != blktype) {
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
                }

                /* Does the ID contain a block specifier? */
                if (!sid2.block.valid) {
                    /* User didn't specify the block, so we'll insert this one */
                    sal_strlcpy(sid2.block.name,
                                bcma_bcmbd_block_type2name(unit, blktype),
                                sizeof(sid2.block.name));

                    /*
                     * If the user DID specify a top-level port number (port != -1)
                     * we need to see if that port is actually a part of this block.
                     * If not, we will just punt.
                     */
                    if (port != -1) {
                        bcmdrd_pblk_t pb;
                        bcmdrd_port_num_domain_t pnd;

                        pnd = bcmdrd_chip_port_num_domain(cinfo, enum_val, blkp->type);
                        if (!bcmdrd_chip_port_valid(cinfo, pnd, port)) {
                            continue;
                        }
                        if (pnd == bcmdrd_chip_port_num_domain(cinfo, -1, blkp->type)) {
                            /* Look for this port within this blocktype */
                            if (bcmdrd_chip_port_block(cinfo, port, blkp->type, &pb) == 0 &&
                                pb.blknum == blkp->blknum) {
                                /* This top-level port is a member of this block */
                                sid2.block.start = sid2.block.end = pb.blknum;
                                sid2.port.start = sid2.port.end = pb.lane;
                                if (BCMBD_CMICX_PIPE_LINST(pipe_info)) {
                                    sid2.port.start = sid2.port.end = port;
                                }
                            }
                        } else if (pnd != BCMDRD_PND_PHYS) {
                            /* Skip default port checks if not physical ports. */
                            sid2.block.start = sid2.block.end = blkp->blknum;
                            sid2.port.start = sid2.port.end = port;
                        } else {
                            /* This top-level port is not a member of this block */
                            continue;
                        }
                    }
                    else {
                        /* No block and no ports, insert all ports in this block */
                        /* Add all blocks of this type */
                        if (sid2.block.start == -1 || sid2.block.end < blkp->blknum) {
                            sid2.block.end = blkp->blknum;
                        }
                        if (sid2.block.start == -1 || sid2.block.start > blkp->blknum) {
                            sid2.block.start = blkp->blknum;
                        }
                    }
                } else {
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

                /* Blocks are all setup. Now we need to check ports */
                if (BCMBD_CMICX_PIPE_LINST(pipe_info)) {
                    if (!sid2.port.valid) {
                       int linst = BCMBD_CMICX_PIPE_LINST(pipe_info);

                       sid2.port.start = 0;
                       sid2.port.end = linst ? linst - 1 : 0;
                    }
                } else if (symbol->flags & BCMDRD_SYMBOL_FLAG_PORT) {
                    /* This is a port-based register */
                    /* Were specific ports specified? */
                    if (!sid2.port.valid) {
                        sid2.port.start = 0;
                        sid2.port.end = BCMDRD_CONFIG_MAX_PORTS - 1;
                    }
                } else {
                    /* Ignore port specification if not a port-based register */
                    sid2.port.start = -1;
                    sid2.port.end = -1;
                }
            }

            if (sid2.block.valid && !match_block) {
                continue;
            }

            /* Skip if we don't have a valid block range by now */
            if (sid2.block.start == -1) {
                continue;
            }

            /* Get dual-pipe access type from symbol flags */
            sid2.addr.ext32 = BCMBD_CMICX_BLKACC2ADEXT(symbol->info);

            /* Lets get it on */
            wsize = BCMDRD_BYTES2WORDS(BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index));
            bcma_bcmbd_cmicx_regops(unit, symbol, enum_val, &sid2, wsize,
                                    and_masks, or_masks);
        }

        /*
         * If a block was specified, the ports we happen to be iterating over
         * are block-based physical ports, which we already processed.
         * Lets bail out of the outermost for loop.
         */
        if (sid->block.valid) {
            break;
        }
    }

    return 0;
}

/*
 * Get or Set a chip memory via S-channel.
 */
static int
mem_symop(int unit, const bcmdrd_symbol_t *symbol, bcmdrd_sid_t enum_val,
          bcma_bcmbd_id_t *sid, uint32_t *and_masks, uint32_t *or_masks)
{
    int b, match_block, wsize;
    int maxidx, linst;
    bcma_bcmbd_id_t sid2;
    const bcmdrd_block_t *blkp;
    int blktype, acc_type;
    uint32_t pipe_info;
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    assert(symbol);
    assert(symbol->flags & BCMDRD_SYMBOL_FLAG_MEMORY);
    assert(cinfo);

    /* Did the user specify indices? */
    if (sid->addr.start == -1) {
        /* No indices specified, insert limits for the memory */
        sid->addr.start = BCMDRD_SYMBOL_INDEX_MIN_GET(symbol->index);
        maxidx = BCMDRD_SYMBOL_INDEX_MAX_GET(symbol->index);
        sid->addr.end = bcmdrd_chip_mem_maxidx(cinfo, enum_val, maxidx);
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
        bcma_bcmbd_cmicx_memops(unit, symbol, enum_val, &sid2, wsize,
                                and_masks, or_masks);
    }

    return 0;
}

/*******************************************************************************
 * Exported functions
 */

int
bcma_bcmbd_cmicx_symop(int unit,
                       const bcmdrd_symbol_t *symbol, bcmdrd_sid_t enum_val,
                       bcma_bcmbd_id_t *sid,
                       uint32_t *and_masks, uint32_t *or_masks)
{
    int cmic_blktype = bcma_bcmbd_block_name2type(unit, "cmic");

    /*
     * Dispatch according to symbol type.
     * Symbols which are not registers or memories will be skipped silently.
     */
    if (bcma_bcmbd_info_block_type_valid(unit, cmic_blktype, symbol->info)) {
        cmic_symop(unit, symbol, enum_val, sid, and_masks, or_masks);
    }
    else if (symbol->flags & BCMDRD_SYMBOL_FLAG_REGISTER) {
        reg_symop(unit, symbol, enum_val, sid, and_masks, or_masks);
    }
    else if (symbol->flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        mem_symop(unit, symbol, enum_val, sid, and_masks, or_masks);
    }
    return 0;
}
