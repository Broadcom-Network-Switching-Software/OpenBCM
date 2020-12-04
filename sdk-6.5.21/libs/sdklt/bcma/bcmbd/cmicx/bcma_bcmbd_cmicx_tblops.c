/*! \file bcma_bcmbd_cmicx_tblops.c
 *
 * Symbolic hash table operations for CMICx devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_error.h>

#include <sal/sal_libc.h>
#include <sal/sal_assert.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmbd/bcmbd_cmicx_tbl.h>

#include <bcma/bcmbd/bcma_bcmbd_symlog.h>
#include <bcma/bcmbd/bcma_bcmbd_cmicx.h>

#define CMICX_RESP_WORD_INDEX_GET(_w)  ((_w) & 0xfffff)

/*! Supported CLI BCMBD CMICx table operations. */
typedef enum cmicx_tbl_op_e {
    CMICX_TBL_INSERT,
    CMICX_TBL_DELETE,
    CMICX_TBL_LOOKUP,
} cmicx_tbl_op_t;

/*******************************************************************************
 * Private functions
 */

/*
 * Perform table insert/delete/lookup operation and
 * display necessary information.
 */
static int
cmicx_cmd_tblop(shr_pb_t *pb, int unit, const bcmdrd_symbol_t *symbol,
                bcma_bcmbd_id_t *sid, int block, int pipe,
                uint32_t size, uint32_t *and_masks, uint32_t *or_masks,
                cmicx_tbl_op_t tbl_op)
{
    int rv;
    int tbl_idx;
    uint32_t adext, idx, addr;
    uint32_t  bank_ignore_mask = 0;
    uint32_t data[BCMDRD_MAX_PT_WSIZE] = {0}, resp_data[BCMDRD_MAX_PT_WSIZE];
    bcma_bcmbd_cmicx_tblop_f tblop_func;
    bcmbd_tbl_resp_info_t resp_info;
    char *err_str, *ok_str;
    bcmlt_pt_opcode_t pt_op;

    if (size > COUNTOF(data)) {
        shr_pb_printf(pb, "%sentity size (%"PRIx32" words) too big\n",
                      BCMA_CLI_CONFIG_ERROR_STR, size);
        return -1;
    }

    /* If the table operation failed, output an error */
    if (tbl_op == CMICX_TBL_INSERT) {
        err_str = "Table insert failure: ";
        ok_str = "Entry is inserted or updated at ";
        tblop_func = bcmbd_cmicx_tbl_insert;
        pt_op = BCMLT_PT_OPCODE_INSERT;
    } else if (tbl_op == CMICX_TBL_DELETE) {
        err_str = "Table delete failure: ";
        ok_str = "Entry is deleted at ";
        tblop_func = bcmbd_cmicx_tbl_delete;
        pt_op = BCMLT_PT_OPCODE_DELETE;
    } else if (tbl_op == CMICX_TBL_LOOKUP) {
        err_str = "Table lookup failure: ";
        ok_str = "Entry is found at ";
        tblop_func = bcmbd_cmicx_tbl_lookup;
        pt_op = BCMLT_PT_OPCODE_LOOKUP;
    } else {
        shr_pb_printf(pb, "%sunknown table operation\n",
                      BCMA_CLI_CONFIG_ERROR_STR);
        return -1;
    }

    /* Default address */
    addr = sid->addr.name32;
    adext = sid->addr.ext32;

    if (pipe >= 0) {
        BCMBD_CMICX_ADEXT_ACCTYPE_SET(adext, pipe);
    }

    /* Calculate the absolute address for this table operation */
    if (block > 0) {
        /* Update address extension with specified block */
        BCMBD_CMICX_ADEXT_BLOCK_SET(adext, block);
        addr = bcmbd_block_port_addr(unit, block, -1, addr, -1);
    }

    /* Set table entry value */
    if (and_masks || or_masks) {
        for (idx = 0; idx < size; idx++) {
            if(and_masks) data[idx] &= and_masks[idx];
            if(or_masks) data[idx] |= or_masks[idx];
        }

        /* Perform table operation */
        rv = tblop_func(unit, adext, addr, data, size, bank_ignore_mask,
                        &resp_info, resp_data);

        /* Log the operation. */
        bcma_bcmbd_symlog_set(unit, adext, addr, BCMDRD_SYMBOL_FLAG_HASHED,
                              pt_op, rv);

        /* If the table operation failed, output an error */
        if (rv != SHR_E_NONE) {
            shr_pb_printf(pb, "%s%s%s (%d)\n", BCMA_CLI_CONFIG_ERROR_STR,
                          err_str, shr_errmsg(rv), rv);
            return rv;
        }

        /* Get memory index if no failure */
        tbl_idx = CMICX_RESP_WORD_INDEX_GET(resp_info.resp_word);

        /* Display returned entry index. */
        shr_pb_printf(pb, "%s%s[%d]\n", ok_str, sid->addr.name, tbl_idx);

        if (tbl_op != CMICX_TBL_LOOKUP) {
            return rv;
        }
    } else {
        shr_pb_printf(pb, "%sHash table key is not specified.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return -1;
    }

    if (sid->flags & BCMA_BCMBD_CMD_IDF_NONZERO) {
        for (idx = 0; idx < size; idx++) {
            if (resp_data[idx]) break;
        }
        if (idx >= size) return 0;
    }

    /*
     * If we got here, this was a successful lookup operation.
     * Print out the memory data.
     */
    shr_pb_printf(pb, "Lookup returned data =");
    for (idx = 0; idx < size; idx++) {
        shr_pb_printf(pb, " 0x%08"PRIx32, resp_data[idx]);
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
        bcma_bcmbd_dump_fields(pb, unit, symbol, resp_data, sid->flags,
                               sid->ccb, sid->ccb_cookie);
    }
#endif

    return 0;
}

/*
 * This function iterates over all specified memory blocks in a
 * given sid, performs the request table operation, and displays necessary info.
 */
static int
cmicx_tblops(int unit, const bcmdrd_symbol_t *symbol, bcma_bcmbd_id_t *sid,
             uint32_t size, uint32_t *and_masks, uint32_t *or_masks,
             cmicx_tbl_op_t tbl_op)
{
    int b, bt, t;
    uint32_t pipe_info = 0, pmask = 0;
    int acc_type = -1;
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);
    shr_pb_t *pb;

    assert(cinfo);

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
            }
            pmask = BCMBD_CMICX_PIPE_PMASK(pipe_info);
        }

        for (t = sid->pipe.start; t <= sid->pipe.end; t++) {
            /* Loop for base type (if applicable) */
            if (t != -1) {
                /* If this is not a valid pipe value, skip it */
                if (!((1 << t) & pmask)) {
                    continue;
                }
            }

            cmicx_cmd_tblop(pb, unit, symbol, sid, b, t, size,
                            and_masks, or_masks, tbl_op);
            cli_out("%s", shr_pb_str(pb));
            shr_pb_mark(pb, 0);
            shr_pb_reset(pb);
        }
    }
    shr_pb_destroy(pb);

    return 0;
}

/*
 * Hash table operation insert/delete/lookup via S-channel.
 */
static int
tbl_symop(int unit, const bcmdrd_symbol_t *symbol, bcma_bcmbd_id_t *sid,
          uint32_t *and_masks, uint32_t *or_masks, cmicx_tbl_op_t tbl_op)
{
    int b, wsize;
    bcma_bcmbd_id_t sid2;
    const bcmdrd_block_t *blkp;
    int blktype, acc_type;
    uint32_t pipe_info;
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    assert(symbol);
    assert(cinfo);

    if (!(symbol->flags & BCMDRD_SYMBOL_FLAG_HASHED)) {
        cli_out("%ssymbol '%s' is not a hashed table\n",
                BCMA_CLI_CONFIG_ERROR_STR, symbol->name);
        return -1;
    }

    /* The indices are always 0 for table operations. */
    sid->addr.start = 0;
    sid->addr.end = 0;

    sid->block.start = 0;
    sid->block.end = 0;

    sid->port.start = 0;
    sid->port.end = 0;

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

            /* specific blocks were indicated. */
            /* Need to convert these to physical blocks */
            sid2.block.start = bcmdrd_chip_block_number(cinfo, blktype,
                                                        sid->block.start);
            sid2.block.end = bcmdrd_chip_block_number(cinfo, blktype,
                                                      sid->block.end);
        }

        /* Skip if we don't have a valid block range by now */
        if (sid2.block.start == -1) {
            continue;
        }

        /* Get dual-pipe access type from symbol flags */
        sid2.addr.ext32 = BCMBD_CMICX_BLKACC2ADEXT(symbol->info);

        wsize = BCMDRD_BYTES2WORDS(BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index));
        cmicx_tblops(unit, symbol, &sid2, wsize, and_masks, or_masks, tbl_op);
    }

    return 0;
}

/*******************************************************************************
 * Exported functions
 */

int
bcma_bcmbd_cmicx_tbl_insert(int unit, const bcmdrd_symbol_t *symbol,
                            bcma_bcmbd_id_t *sid,
                            uint32_t *and_masks, uint32_t *or_masks)
{
    return tbl_symop(unit, symbol, sid, and_masks, or_masks, CMICX_TBL_INSERT);
}

int
bcma_bcmbd_cmicx_tbl_delete(int unit, const bcmdrd_symbol_t *symbol,
                            bcma_bcmbd_id_t *sid,
                            uint32_t *and_masks, uint32_t *or_masks)
{
    return tbl_symop(unit, symbol, sid, and_masks, or_masks, CMICX_TBL_DELETE);
}

int
bcma_bcmbd_cmicx_tbl_lookup(int unit, const bcmdrd_symbol_t *symbol,
                            bcma_bcmbd_id_t *sid,
                            uint32_t *and_masks, uint32_t *or_masks)
{
    return tbl_symop(unit, symbol, sid, and_masks, or_masks, CMICX_TBL_LOOKUP);
}
