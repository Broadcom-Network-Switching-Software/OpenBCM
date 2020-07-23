/*! \file bcma_bcmbd_cmicx_memregs.c
 *
 * This file provides all of the basic register and memory reads and writes
 * for CMICx devices.
 * These functions are shared by both the symbolic and non-symbolic commands.
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
#include <bcmdrd/bcmdrd_pt.h>

#include <bcmbd/bcmbd_cmicx_reg.h>
#include <bcmbd/bcmbd_cmicx_mem.h>

#include <bcma/bcmbd/bcma_bcmbd_symlog.h>
#include <bcma/bcmbd/bcma_bcmbd_cmicx.h>

/*******************************************************************************
 * Private functions
 */

/*
 * Get the port number domain for a symbol ID
 */
static int
sym_pnd(int unit, bcmdrd_sid_t enum_val, bcmdrd_port_num_domain_t *pnd)
{
    int bt;
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    assert(cinfo);

    /* Get block type */
    bt = bcmdrd_chip_blktype_get(cinfo, enum_val, 0);
    if (bt < 0) {
        return -1;
    }

    if (pnd) {
        *pnd = bcmdrd_chip_port_num_domain(cinfo, enum_val, bt);
    }

    return 0;
}

/*
 * Display register/memory operations information
 */
static void
print_memreg_op(shr_pb_t *pb, int unit, const char *prefix,
                bcmdrd_sid_t enum_val, bcma_bcmbd_id_t *sid,
                int a, int b, int p, int t, int sect_size,
                uint32_t adext, uint32_t addr,
                const char *errstr, int rc)
{
    char blockname[32];
    char port_str[32];
    bcmdrd_port_num_domain_t pnd;
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
        } else {
            int port = bcmdrd_pt_port_number_get(unit, enum_val, b, p);
            sal_snprintf(port_str, sizeof(port_str), "%2d", port);

            /* Check if this symbol uses logical/MMU port numbers */
            if (b >= 0 && sym_pnd(unit, enum_val, &pnd) == 0) {
                if (pnd == BCMDRD_PND_LOGIC) {
                    sal_sprintf(port_str, "logic-%d", p);
                } else if (pnd == BCMDRD_PND_MMU) {
                    sal_sprintf(port_str, "mmu-%d", p);
                }
            }
            if (bcma_cli_parse_is_int(sid->port.name)) {
                shr_pb_printf(pb, ".%d (BLOCK %d, PORT %s)", p, b, port_str);
            } else {
                shr_pb_printf(pb, ".%s%d (BLOCK %d, PORT %s)",
                              sid->port.name, p, b, port_str);
            }
        }
    }
    shr_pb_printf(pb, " [0x%04"PRIx32"%08"PRIx32"]", adext, addr);

    if (rc < 0) {
        shr_pb_printf(pb, " (%d)", rc);
    }
}

/*
 * Perform register read/write operation and display necessary information.
 */
static int
cmicx_cmd_regop(shr_pb_t *pb, int unit,
                const bcmdrd_symbol_t *symbol, bcmdrd_sid_t enum_val,
                bcma_bcmbd_id_t *sid,
                int aindex, int block, int port, int pipe, uint32_t size,
                uint32_t *and_masks, uint32_t *or_masks)
{
    int rc = 0;
    uint32_t idx, adext, addr, sym_flags;
    uint32_t data[2];
    bool write_op;

    if (size > sizeof(data) / sizeof(uint32_t)) {
        shr_pb_printf(pb, "%sentity size (%"PRIx32" words) too big\n",
                      BCMA_CLI_CONFIG_ERROR_STR, size);
        return -1;
    }

    /* Default address */
    addr = sid->addr.name32;
    adext = sid->addr.ext32;

    if (pipe >= 0) {
        BCMBD_CMICX_ADEXT_ACCTYPE_SET(adext, pipe);
    }

    /* Calculate the absolute address for this access */
    if (symbol && block > 0) {
        int lane = port;
        uint32_t step = BCMDRD_SYMBOL_INDEX_STEP_GET(symbol->index);
        if (lane < 0) {
            lane = 0;
        }
        /* Update address extension with specified block */
        BCMBD_CMICX_ADEXT_BLOCK_SET(adext, block);
        addr = bcmbd_block_port_addr(unit, block, lane, addr, aindex * step);
    }

    if (symbol) {
        sym_flags = symbol->flags;
    } else {
        sym_flags = BCMDRD_SYMBOL_FLAG_REGISTER;
    }

    /*
     * If masks are specific, this is a read-modify-write or a write-only
     * operation
     */
    write_op = (and_masks || or_masks);

    if (write_op && (sid->flags & BCMA_BCMBD_CMD_IDF_RAW)) {
        /* Write-only from the set command. Initialize the data to all zero's */
        sal_memset(data, 0, size * 4);
    } else {
        /* Read the data */
        if (size == 1) {
            rc = bcmbd_cmicx_reg_read(unit, adext, addr, data, 1);
        } else {
            rc = bcmbd_cmicx_reg_read(unit, adext, addr, data, 2);
        }

        /* Log the operation. */
        bcma_bcmbd_symlog_set(unit, adext, addr, sym_flags,
                              BCMLT_PT_OPCODE_GET, rc);
    }

    /* Print an error message if the read failed */
    if (rc < 0) {
        print_memreg_op(pb, unit, "register",
                        enum_val, sid,
                        aindex, block, port, pipe, -1,
                        adext, addr, "reading", rc);
        shr_pb_printf(pb, "\n");
        return rc;
    }

    if (write_op) {
        for (idx = 0; idx < size; idx++) {
            if (and_masks) {
                data[idx] &= and_masks[idx];
            }
            if (or_masks) {
                data[idx] |= or_masks[idx];
            }
        }

        /* Write the data */
        if (size == 1) {
            rc = bcmbd_cmicx_reg_write(unit, adext, addr, data, 1);
        } else {
            rc = bcmbd_cmicx_reg_write(unit, adext, addr, data, 2);
        }

        /* Log the operation. */
        bcma_bcmbd_symlog_set(unit, adext, addr, sym_flags,
                              BCMLT_PT_OPCODE_SET, rc);

        /* Print en error message if the write failed */
        if (rc < 0) {
            print_memreg_op(pb, unit, "register",
                            enum_val, sid,
                            aindex, block, port, pipe, -1,
                            adext, addr, "writing", rc);
            shr_pb_printf(pb, "\n");
            return rc;
        }

        /* No more to be done if this was a write rather than a read */
        return 0;
    }

    if (sid->flags & BCMA_BCMBD_CMD_IDF_NONZERO) {
        for (idx = 0; idx < size; idx++) {
            if (data[idx]) {
                break;
            }
        }
        if (idx >= size) {
            return 0;
        }
    }

    if (symbol && sid->flags & BCMA_BCMBD_CMD_IDF_DIFF) {
        if (bcma_bcmbd_resetval_cmp(unit, symbol, size, data) == 0) {
            return 0;
        }
    }

    /* If we are here, this was a read. Output the register data */
    print_memreg_op(pb, unit, "register",
                    enum_val, sid,
                    aindex, block, port, pipe, -1,
                    adext, addr, "reading", rc);
    if (size == 1) {
        shr_pb_printf(pb, " = 0x%08"PRIx32, data[0]);
    }
    if (size == 2) {
        shr_pb_printf(pb, " = 0x%08"PRIx32":0x%08"PRIx32, data[0], data[1]);
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
 * Perform memory read/write operation and display necessary information.
 */
static int
cmicx_cmd_memop(shr_pb_t *pb, int unit,
                const bcmdrd_symbol_t *symbol, bcmdrd_sid_t enum_val,
                bcma_bcmbd_id_t *sid,
                int block, int mindex, int pipe, int basetype, int sect_size,
                uint32_t size, uint32_t *and_masks, uint32_t *or_masks)
{
    int rc = 0;
    int idx;
    uint32_t adext, i, addr, sym_flags;
    uint32_t data[BCMDRD_MAX_PT_WSIZE];
    bool write_op;

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

    if (symbol) {
        sym_flags = symbol->flags;
    } else {
        sym_flags = BCMDRD_SYMBOL_FLAG_MEMORY;
    }

    /*
     * If masks are specific, this is a read-modify-write or a write-only
     * operation
     */
    write_op = (and_masks || or_masks);

    if (write_op && (sid->flags & BCMA_BCMBD_CMD_IDF_RAW)) {
        /* Write-only from the set command. Initialize the data to all zero's */
        sal_memset(data, 0, size * 4);
    } else {
        /* Read the data */
        /* coverity[overrun-call] */
        rc = bcmbd_cmicx_mem_read(unit, adext, addr + idx, data, size);

        /* Log the operation. */
        bcma_bcmbd_symlog_set(unit, adext, addr + idx, sym_flags,
                              BCMLT_PT_OPCODE_GET, rc);
    }

    /* If the read failed, output an error */
    if (rc < 0) {
        print_memreg_op(pb, unit, "memory",
                        enum_val, sid,
                        mindex, block, basetype, pipe, sect_size,
                        adext, addr + idx, "reading", rc);
        shr_pb_printf(pb, "\n");
        return rc;
    }

    if (write_op) {
        for (i = 0; i < size; i++) {
            if(and_masks) data[i] &= and_masks[i];
            if(or_masks) data[i] |= or_masks[i];
        }

        /* Write the data */
        rc = bcmbd_cmicx_mem_write(unit, adext, addr + idx, data, size);

        /* Log the operation. */
        bcma_bcmbd_symlog_set(unit, adext, addr + idx, sym_flags,
                              BCMLT_PT_OPCODE_SET, rc);

        /* If the write failed, output an error */
        if (rc < 0) {
            print_memreg_op(pb, unit, "memory",
                            enum_val, sid,
                            mindex, block, basetype, pipe, sect_size,
                            adext, addr + idx, "writing", rc);
            shr_pb_printf(pb, "\n");
            return rc;
        }

        /* Nothing more to be done for writes */
        return rc;
    }

    if (sid->flags & BCMA_BCMBD_CMD_IDF_NONZERO) {
        for (i = 0; i < size; i++) {
            if (data[i]) break;
        }
        if (i >= size) return 0;
    }

    if (symbol && sid->flags & BCMA_BCMBD_CMD_IDF_DIFF) {
        if (bcma_bcmbd_resetval_cmp(unit, symbol, size, data) == 0) {
            return 0;
        }
    }

    /* If we got here, this was a read. Print out the memory data */
    print_memreg_op(pb, unit, "memory",
                    enum_val, sid,
                    mindex, block, basetype, pipe, sect_size,
                    adext, addr + idx, "reading", rc);
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

/*******************************************************************************
 * Exported functions
 */

int
bcma_bcmbd_cmicx_regops(int unit,
                        const bcmdrd_symbol_t *symbol, bcmdrd_sid_t enum_val,
                        bcma_bcmbd_id_t *sid, uint32_t size,
                        uint32_t *and_masks, uint32_t *or_masks)
{
    int a, b, bt = -1, p, t;
    uint32_t pipe_info = 0, pmask = 0;
    int linst = 0, acc_type = -1;
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);
    uint32_t blk_pipe_bmp = 0;
    shr_pb_t *pb;
    bool write_op = (and_masks || or_masks);

    assert(cinfo);

    pb = shr_pb_create();

    /* Foreach address index */
    for (a = sid->addr.start; a <= sid->addr.end; a++) {
        /* Foreach block number */
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

                /*
                 * Skip this block if its port bitmap is empty.
                 * This is because not all blocks of the same type that exist
                 * on a chip may have ports in them, specially true of GPORTs.
                 */
                if (BCMDRD_ID_VALID(enum_val)) {
                    bcmdrd_pbmp_t pbmp;

                    if (bcma_bcmbd_symbol_block_valid_ports_get(unit, enum_val,
                                                                b, &pbmp) < 0) {
                        continue;
                    }
                    if (BCMDRD_PBMP_IS_NULL(pbmp)) {
                        continue;
                    }
                }

                if (symbol) {
                    acc_type = BCMBD_CMICX_BLKACC2ACCTYPE(symbol->info);
                    pipe_info = bcmbd_cmicx_pipe_info(unit, sid->addr.name32,
                                                      acc_type, bt, -1);
                    pmask = BCMBD_CMICX_PIPE_PMASK(pipe_info);
                    linst = BCMBD_CMICX_PIPE_LINST(pipe_info);
                    (void)bcmdrd_dev_valid_blk_pipes_get(unit, 0, bt,
                                                         &blk_pipe_bmp);
                }
            }

            for (t = sid->pipe.start; t <= sid->pipe.end; t++) {
                /* Foreach port (or base index) */
                for (p = sid->port.start; p <= sid->port.end; p++) {
                    if (t != -1 && pmask == 0) {
                        /*
                         * For non-UNIQUE access type, we allow "read" on
                         * each individual physical instance for debug
                         * purpose.
                         */
                        if (write_op || !((1 << t) & blk_pipe_bmp)) {
                            continue;
                        }
                    } else if (BCMDRD_ID_VALID(enum_val)) {
                        int port = p;

                        /*
                         * Convert the block port number to top-level port
                         * number for port-based register check in
                         * bcmdrd_pt_index_valid()
                         */
                        if (p != -1 && bcmdrd_pt_is_port_reg(unit, enum_val)) {
                            port = bcmdrd_pt_port_number_get(unit, enum_val,
                                                             b, p);
                            if (port == -1) {
                                continue;
                            }
                        }
                        if (!bcmdrd_pt_index_valid(unit, enum_val,
                                                   t >= 0 ? t : port, a)) {
                            continue;
                        }
                    }

                    if (symbol && linst && p != -1 && bt != -1) {
                        uint32_t bpm;

                        /* Check if baseindex is valid */
                        if (p >= linst) {
                            continue;
                        }

                        /*
                         * Symbols with unique access type and basetype
                         * will carray overlay flag and the base index validity
                         * can not be checked from bcmdrd_pt_index_valid()
                         */
                        bpm = bcmbd_cmicx_pipe_info(unit, sid->addr.name32,
                                                    acc_type, bt, p);
                        if (t >= 0 && bpm && (bpm & (1 << t)) == 0) {
                            continue;
                        }
                    }

                    /* Perform the operation on this specific register */
                    cmicx_cmd_regop(pb, unit, symbol, enum_val, sid,
                                    a, b, p, t, size,
                                    and_masks, or_masks);
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

int
bcma_bcmbd_cmicx_memops(int unit,
                        const bcmdrd_symbol_t *symbol, bcmdrd_sid_t enum_val,
                        bcma_bcmbd_id_t *sid, uint32_t size,
                        uint32_t *and_masks, uint32_t *or_masks)
{
    int b, bt = -1, idx, t, p;
    uint32_t pipe_info = 0, pmask = 0, sect_size = 0;
    int linst = 0, acc_type = -1;
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);
    uint32_t blk_pipe_bmp = 0;
    shr_pb_t *pb;
    bool write_op = (and_masks || or_masks);

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

            /*
             * Check if the block port bitmap is NULL
             * if this is a port block symbol.
             */
            if (symbol && (symbol->flags & BCMDRD_SYMBOL_FLAG_SOFT_PORT)) {
                bcmdrd_pbmp_t pbmp;

                if (bcma_bcmbd_symbol_block_valid_ports_get(unit, enum_val, b, &pbmp) < 0) {
                    continue;
                }
                if (BCMDRD_PBMP_IS_NULL(pbmp)) {
                    continue;
                }
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

                if (t != -1 && pmask == 0) {
                    /*
                     * For non-UNIQUE access type, we allow "read" on
                     * each individual physical instance for debug
                     * purpose.
                     */
                    if (write_op || !((1 << t) & blk_pipe_bmp)) {
                        continue;
                    }
                } else if (BCMDRD_ID_VALID(enum_val) &&
                           !bcmdrd_pt_index_valid(unit, enum_val,
                                                  t >= 0 ? t : p, -1)) {
                    continue;
                }

                if (symbol && linst && p != -1 && bt != -1) {
                    uint32_t bpm;

                    /* Check if index for BASE_TYPE is valid */
                    if (p >= linst) {
                        continue;
                    }

                    /*
                     * Symbols with unique access type and basetype
                     * will carray overlay flag and the base index validity
                     * can not be checked from bcmdrd_pt_index_valid()
                     */
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
                    if (BCMDRD_ID_VALID(enum_val) &&
                        !bcmdrd_pt_index_valid(unit, enum_val, -1, idx)) {
                        continue;
                    }
                    cmicx_cmd_memop(pb, unit, symbol, enum_val, sid,
                                    b, idx + pipe_offset, t, p, sect_size, size,
                                    and_masks, or_masks);
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
