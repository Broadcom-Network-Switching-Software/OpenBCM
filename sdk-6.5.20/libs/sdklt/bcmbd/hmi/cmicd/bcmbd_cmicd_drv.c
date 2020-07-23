/*! \file bcmbd_cmicd_drv.c
 *
 * Main entry points for CMICd base driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_assert.h>

#include <shr/shr_debug.h>

#include <bcmbd/bcmbd.h>
#include <bcmbd/bcmbd_cmicd.h>
#include <bcmbd/bcmbd_cmicd_reg.h>
#include <bcmbd/bcmbd_cmicd_mem.h>
#include <bcmbd/bcmbd_cmicd_tbl.h>
#include <bcmbd/bcmbd_cmicd_fifo.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_pt.h>

#include "bcmbd_cmicd_schan.h"

#define BSL_LOG_MODULE BSL_LS_BCMBD_DEV

/*******************************************************************************
 * Local definitions
 */

typedef struct cmicd_addr_s {
    int acctype;
    uint32_t offset;
    int index;
    int blknum;
    int lane;
} cmicd_addr_t;

/*
 * Cookie for bcmdrd_symbols_iter() in cmicd_addr_decode()
 */
typedef struct cmicd_addr_dec_cookie_s {
    int unit;
    const bcmdrd_chip_info_t *cinfo;
    bcmdrd_addr_decode_f addr_dec_func;
    int dup_acc;
    int num_pipes;
    uint32_t addr;
    int acctype;
    int blknum;
    int blktype;
    int blkinst;
    int is_decoded;
    int possible_match;
    int relaxed_match;
    bcmdrd_sid_t sid;
    bcmbd_pt_dyn_info_t dyn_info;
} cmicd_addr_dec_cookie_t;

/*******************************************************************************
 * Private functions
 */

static uint32_t
cmicd_block_port_addr(int block, int lane,
                            uint32_t offset, uint32_t idx)
{
    uint32_t addr;

    if (lane < 0) {
        /* Default block/port calculation for memories */
        addr = offset;
        addr += idx;
    } else {
        /* Default block/port calculation for registers */
        addr = (offset | lane);
        addr += (idx * 0x100);
    }
    return addr;
}

static int
cmicd_addr_decode(const bcmdrd_symbol_t *symbol, int block, uint32_t addr,
                  uint32_t *idx, uint32_t *lane)
{
    uint32_t diff = 0, port_num = 0;
    uint32_t addr_mask, offset_mask;
    uint32_t mask;

    /* Check for Stage ID and basetype. */
    mask = 0xff800000;
    if ((addr & mask) != (symbol->addr & mask)) {
        return -1;
    }

    /* Mask the Stage ID and basetype. */
    mask = ~mask;
    addr_mask = addr & mask;
    offset_mask = symbol->addr & mask;

    if (addr_mask < offset_mask) {
        return -1;
    }

    diff = addr_mask - offset_mask;
    if (symbol->flags & BCMDRD_SYMBOL_FLAG_REGISTER) {
        /*
         * For SW created MMU individual symbols, the symbol offset might
         * already contain the base index (the same to the port_num value).
         * Use the difference of the access address and symbol offset to
         * calculate the port_num instead of using the access address directly
         * to eliminate the base index to be returned as port_num if it's
         * already in the symbol offset.
         */
        port_num = diff & 0xff;
        diff = diff >> 8;
    }

    if (idx) {
        *idx = diff;
    }
    if (lane) {
        *lane = port_num;
    }
    return 0;
}

/*
 * Helper functions for PT interface
 */

static uint32_t
cmicd_raw_addr_get(int unit, cmicd_addr_t *ca)
{
    return bcmbd_block_port_addr(unit, ca->blknum, ca->lane,
                                 ca->offset, ca->index);
}

static uint32_t
cmicd_raw_adext_get(int unit, cmicd_addr_t *ca)
{
    uint32_t adext = 0;

    BCMBD_CMICD_ADEXT_BLOCK_SET(adext, ca->blknum);
    BCMBD_CMICD_ADEXT_ACCTYPE_SET(adext, ca->acctype);

    return adext;
}

static int
cmicd_sym_addr(int unit,
               const bcmdrd_chip_info_t *cinfo,
               bcmdrd_sym_info_t *sinfo,
               bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
               cmicd_addr_t *ca)
{
    bcmdrd_pblk_t pblk;
    int blktype, blkinst, port, linst;
    uint32_t blkacc;
    uint32_t pipe_info, pmask, ovrr_at;

    SHR_FUNC_ENTER(unit);

    /* First check if the instance and index is valid */
    if (!bcmdrd_pt_index_valid(unit, sinfo->sid,
                               dyn_info->tbl_inst, dyn_info->index)) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "sid = %s, "
                                        "tbl_inst = %d, index = %d\n"),
                             sinfo->name, dyn_info->tbl_inst, dyn_info->index));
    }

    sal_memset(ca, 0, sizeof(*ca));

    blkacc = sinfo->blktypes;

    ca->acctype = BCMBD_CMICD_BLKACC2ACCTYPE(blkacc);
    ca->offset = sinfo->offset;
    ca->index = dyn_info->index * sinfo->step_size;;

    blktype = bcmdrd_pt_blktype_get(unit, sinfo->sid, 0);

    /* Check if per-pipe block is to be accessed */
    pipe_info = bcmbd_cmicd_pipe_info(unit, ca->offset, ca->acctype,
                                      blktype, -1);
    pmask = BCMBD_CMICD_PIPE_PMASK(pipe_info);
    linst = BCMBD_CMICD_PIPE_LINST(pipe_info);

    if (pmask != 0) {
        /*
         * Override access type to per-pipe or duplicate access type
         * according to table instance.
         */
        if (dyn_info->tbl_inst == -1) {
            /*
             * Access all pipes.
             * Override access type by duplicate access type.
             */
            ovrr_at = BCMBD_CMICD_PIPE_AT_DUPLICATE(pipe_info);
        } else {
            ovrr_at = dyn_info->tbl_inst;
        }
        ca->acctype = ovrr_at;
    }

    if (sinfo->flags & BCMDRD_SYMBOL_FLAG_PORT) {
        /* Iterate over blocks until matching port is found */
        port = dyn_info->tbl_inst;
        do {
            if (bcmdrd_chip_port_block(cinfo, port, blktype, &pblk) == 0) {
                break;
            }
            blktype = bcmdrd_pt_blktype_get(unit, sinfo->sid, blktype + 1);
        } while (blktype >= 0);
        if (blktype < 0) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit,
                                            "sid = %s, port = %d\n"),
                                 sinfo->name, port));
        }
        ca->blknum = pblk.blknum;
        ca->lane = pblk.lane;
    } else {
        if (pmask != 0 || linst != 0) {
            /* Use instance 0 as block instance for per-pipe table. */
            blkinst = 0;
        } else {
            blkinst = dyn_info->tbl_inst;
        }
        if (blkinst < 0) {
            blkinst = 0;
        }
        ca->blknum = bcmdrd_chip_block_number(cinfo, blktype, blkinst);
        if (ca->blknum < 0) {
            SHR_IF_ERR_MSG_EXIT
                (SHR_E_PARAM,
                 (BSL_META_U(unit,
                             "sid = %s, blktype = %d, blkinst = %d\n"),
                  sinfo->name, blktype, blkinst));
        }

        /*
         * The SIDs with UNIQUE access type and BASE_TYPE instance > 1
         * should have been separate to per-pipe SIDs.
         */
        if (pmask != 0 && linst > 1) {
            /*
             * If we get here it means that the caller is using an overlay
             * SID, or that a per-pipe SID has not been split into multiple
             * SIDs by the register file processing script.
             */
            SHR_IF_ERR_MSG_EXIT
                (SHR_E_INTERNAL,
                 (BSL_META_U(unit,
                             "Ambiguous operation for %s "
                             "[index = %d, tbl_inst = %d].\n"),
                  sinfo->name, dyn_info->index, dyn_info->tbl_inst));
        }

        if (pmask == 0) {
            if (linst != 0) {
                ca->lane = (dyn_info->tbl_inst < 0) ? 0 : dyn_info->tbl_inst;
            } else {
                ca->lane = 0;
            }
        }
    }
    if (sinfo->flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        uint32_t sec_size = BCMBD_CMICD_PIPE_SECT_SIZE(pipe_info);

        if (linst != 0 && sec_size != 0 && dyn_info->tbl_inst > 0) {
            ca->offset += dyn_info->tbl_inst * sec_size;
        }
        ca->lane = -1;
    }

    /* Override access type to per-pipe access. */
    if (ovrr_info) {
        bcmbd_cmicd_ovrr_info_t *ovrr = (bcmbd_cmicd_ovrr_info_t *)ovrr_info;

        if (ovrr->inst >= 0 && ovrr->inst < BCMBD_CMICD_PHYS_INST_MAX) {
            ca->acctype = ovrr->inst;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
cmicd_sym_read(int unit, bcmdrd_sym_info_t *sinfo,
               bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
               uint32_t *data, size_t wsize)
{
    const bcmdrd_chip_info_t *cinfo;
    cmicd_addr_t ca;
    uint32_t addr, adext;

    SHR_FUNC_ENTER(unit);

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (wsize < sinfo->entry_wsize) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "entry_wsize = %d, wsize = %d\n"),
                             (int)sinfo->entry_wsize, (int)wsize));
    }
    wsize = sinfo->entry_wsize;

    SHR_IF_ERR_EXIT
        (cmicd_sym_addr(unit, cinfo, sinfo, dyn_info, ovrr_info, &ca));

    addr = cmicd_raw_addr_get(unit, &ca);
    adext = cmicd_raw_adext_get(unit, &ca);

    if (sinfo->flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        SHR_IF_ERR_EXIT
            (bcmbd_cmicd_mem_read(unit, adext, addr, data, wsize));
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (bcmbd_cmicd_reg_read(unit, adext, addr, data, wsize));

exit:
    SHR_FUNC_EXIT();
}

static int
cmicd_sym_write(int unit, bcmdrd_sym_info_t *sinfo,
                bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                uint32_t *data)
{
    const bcmdrd_chip_info_t *cinfo;
    cmicd_addr_t ca;
    uint32_t wsize;
    uint32_t addr, adext;

    SHR_FUNC_ENTER(unit);

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    wsize = sinfo->entry_wsize;

    SHR_IF_ERR_EXIT
        (cmicd_sym_addr(unit, cinfo, sinfo, dyn_info, ovrr_info, &ca));

    addr = cmicd_raw_addr_get(unit, &ca);
    adext = cmicd_raw_adext_get(unit, &ca);

    if (sinfo->flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        SHR_IF_ERR_EXIT
            (bcmbd_cmicd_mem_write(unit, adext, addr, data, wsize));
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (bcmbd_cmicd_reg_write(unit, adext, addr, data, wsize));

exit:
    SHR_FUNC_EXIT();
}

static int
cmicd_sym_tbl_addr(int unit, bcmdrd_sym_info_t *sinfo,
                   bcmbd_pt_dyn_hash_info_t *dyn_hash_info,
                   void *ovrr_hash_info,
                   uint32_t *adext, uint32_t *addr, uint32_t *bank_ignore_mask)
{
    const bcmdrd_chip_info_t *cinfo;
    bcmbd_pt_dyn_info_t dyn_info;
    cmicd_addr_t ca;

    SHR_FUNC_ENTER(unit);

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    dyn_info.tbl_inst = dyn_hash_info->tbl_inst;
    dyn_info.index = 0;

    SHR_IF_ERR_EXIT
        (cmicd_sym_addr(unit, cinfo, sinfo, &dyn_info, NULL, &ca));

    *addr = cmicd_raw_addr_get(unit, &ca);
    *bank_ignore_mask = 0;

    *adext = cmicd_raw_adext_get(unit, &ca);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Helper functions for macro access.
 */

/*
 * Calculate address of port-based register
 */
static uint32_t
cmicd_port_addr(int unit, uint32_t blkacc, int port,
                uint32_t offset, int idx, uint32_t *adext)
{
    int p, blk_port = 0;
    int blknum = -1;
    const bcmdrd_chip_info_t *cinfo;
    const bcmdrd_block_t *blkp;

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return -1;
    }

    /*
     * Determine which block this port belongs to.
     * Note that if block = CMIC block this may be a register
     * that exists in more than one port block type.
     */
    blkp = bcmdrd_chip_block_types_match(cinfo, blkacc, NULL);
    while (blkp) {
        if (BCMDRD_PBMP_MEMBER(blkp->pbmps, port)) {
            blknum = blkp->blknum;
            break;
        }
        blkp = bcmdrd_chip_block_types_match(cinfo, blkacc, blkp + 1);
    }

    /* Get the physical port number within this block */
    if (blkp) {
        BCMDRD_PBMP_ITER(blkp->pbmps, p) {
            if (p == port) {
                /* Construct address extension from access type and block */
                *adext = BCMBD_CMICD_BLKACC2ADEXT(blkacc);
                BCMBD_CMICD_ADEXT_BLOCK_SET(*adext, blknum);
                return bcmbd_block_port_addr(unit, blknum, blk_port,
                                             offset, idx);
            }
            blk_port++;
        }
    }

    /*
     * If we get here then something is not right, but we do not
     * want to assert because we could have been called from the
     * CLI with a raw address.
     */
    LOG_WARN(BSL_LS_BCMBD_DEV,
             (BSL_META_U(unit,
                        "cmicd_port_addr: invalid port %d "
                        "for offset 0x%08"PRIx32"\n"),
             port, offset));

    return offset;
}

static int
cmicd_mem_clear(int unit, uint32_t adext, uint32_t addr,
                uint32_t si, uint32_t ei, size_t wsize)
{
    int ioerr = 0;
    uint32_t idx;
    static uint32_t zeros[BCMDRD_MAX_PT_WSIZE];
    const bcmdrd_chip_info_t *cinfo;

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return SHR_E_INTERNAL;
    }

    for (idx = si; idx <= ei; idx++) {
        ioerr += bcmbd_cmicd_mem_write(unit, adext, addr + idx, zeros, wsize);
    }

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

/* Read block-based memories */
static int
cmicd_mem_block_read(int unit, uint32_t blkacc, int blknum, uint32_t offset,
                     int idx, uint32_t *data, size_t wsize)
{
    uint32_t addr, adext;

    addr = bcmbd_block_port_addr(unit, blknum, -1, offset, 0);

    adext = BCMBD_CMICD_BLKACC2ADEXT(blkacc);
    BCMBD_CMICD_ADEXT_BLOCK_SET(adext, blknum);

    return bcmbd_cmicd_mem_read(unit, adext, addr + idx, data, wsize);
}

/* Write block-based memories */
static int
cmicd_mem_block_write(int unit, uint32_t blkacc, int blknum, uint32_t offset,
                      int idx, uint32_t *data, size_t wsize)
{
    uint32_t addr, adext;

    addr = bcmbd_block_port_addr(unit, blknum, -1, offset, 0);

    adext = BCMBD_CMICD_BLKACC2ADEXT(blkacc);
    BCMBD_CMICD_ADEXT_BLOCK_SET(adext, blknum);

    return bcmbd_cmicd_mem_write(unit, adext, addr + idx, data, wsize);
}

static int
cmicd_mem_block_clear(int unit, uint32_t blkacc, int blknum,
                      uint32_t offset, uint32_t si, uint32_t ei, int wsize)
{
    uint32_t addr, adext;

    addr = bcmbd_block_port_addr(unit, blknum, -1, offset, 0);

    adext = BCMBD_CMICD_BLKACC2ADEXT(blkacc);
    BCMBD_CMICD_ADEXT_BLOCK_SET(adext, blknum);

    return cmicd_mem_clear(unit, adext, addr, si, ei, wsize);
}

/*
 * Call-back function of bcmdrd_symbols_iter() in cmicd_addr_decode()
 */
static int
cmicd_symbol_addr_decode(const bcmdrd_symbol_t *symbol, bcmdrd_sid_t sid,
                         void *vptr)
{
    cmicd_addr_dec_cookie_t *cad = (cmicd_addr_dec_cookie_t *)vptr;
    int unit = cad->unit;
    const bcmdrd_chip_info_t *cinfo = cad->cinfo;
    bcmbd_pt_dyn_info_t *dyn_info = &cad->dyn_info;
    int blktype = -1, blktype_match = 0;
    int port = -1, baseidx = -1;
    uint32_t minidx, maxidx, idx = 0, lane = 0;
    uint32_t pipe_info;
    int acctype, linst;
    bool is_dup_acc, sym_is_dup_acc;

    acctype = BCMBD_CMICD_BLKACC2ACCTYPE(symbol->info);
    blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
    pipe_info = bcmbd_cmicd_pipe_info(unit, symbol->addr, acctype, blktype, -1);

    /* Check if the address is accessed in global view */
    is_dup_acc = (cad->acctype == cad->dup_acc);
    /* Check if the access type of the compared symbol is in global view */
    sym_is_dup_acc = (acctype == cad->dup_acc);

    /* Check for access type */
    if (cad->acctype != -1 && cad->acctype != acctype) {
        /* Check for UNIQUE or DUPLICATE access */
        uint32_t pmask = BCMBD_CMICD_PIPE_PMASK(pipe_info);
        if (pmask != 0) {
            /*
             * Per-pipe or duplicate access are allowed for UNIQUE access type
             * symbols.
             */
            if ((!is_dup_acc && (pmask & (1 << cad->acctype)) == 0) ) {
                /* Continue for the next symbol */
                return 0;
            }
        } else {
            /* Duplicate access type symbols are allowed for per-pipe access */
            if (!sym_is_dup_acc || cad->acctype >= cad->num_pipes) {
                /* Continue for the next symbol */
                return 0;
            }
        }
    }

    /* Check for block type */
    blktype = -1;
    while ((blktype = bcmdrd_pt_blktype_get(unit, sid, blktype + 1)) >= 0) {
        if (cad->blktype == blktype) {
            blktype_match = 1;
            break;
        }
    }
    if (blktype_match == 0) {
        /* Continue for the next symbol */
        return 0;
    }

    if (cad->addr_dec_func(symbol, cad->blknum, cad->addr, &idx, &lane) < 0) {
        /* Continue for the next symbol */
        return 0;
    }

    linst = BCMBD_CMICD_PIPE_LINST(pipe_info);

    minidx = BCMDRD_SYMBOL_INDEX_MIN_GET(symbol->index);
    maxidx = BCMDRD_SYMBOL_INDEX_MAX_GET(symbol->index);

    if (symbol->flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        int sec_size = BCMBD_CMICD_PIPE_SECT_SIZE(pipe_info);

        /* Check the returned idx for memory */
        maxidx = bcmdrd_chip_mem_maxidx(cinfo, sid, maxidx);
        sec_size = BCMBD_CMICD_PIPE_SECT_SIZE(pipe_info);
        if (linst > 0 && sec_size != 0) {
            maxidx += (linst - 1) * sec_size;
        }
        if (idx < minidx || idx > maxidx) {
            /* Continue for the next symbol */
            return 0;
        }
        if (linst > 0 && sec_size != 0) {
            int sec_offset = BCMBD_CMICD_PIPE_SECT(pipe_info);

            baseidx = idx >> sec_offset;
            idx = idx & (sec_size - 1);
        }
    } else {
        /* Check the returned idx/lane for register */
        uint32_t addr;
        int step = BCMDRD_SYMBOL_INDEX_STEP_GET(symbol->index);

        idx = idx / step;
        if (idx < minidx || idx > maxidx) {
            /* Continue for the next symbol. */
            return 0;
        }

        if (symbol->flags & BCMDRD_SYMBOL_FLAG_PORT) {
            port = bcmdrd_chip_port_number(cinfo, cad->blknum, lane);
            if (port < 0) {
                cad->possible_match++;
                if (!cad->relaxed_match) {
                    /* Continue for the next symbol. */
                    return 0;
                }
            }
        } else if (linst > 0) {
            baseidx = lane;
            if (baseidx >= linst) {
                cad->possible_match++;
                if (!cad->relaxed_match) {
                    /* Continue for the next symbol. */
                    return 0;
                }
            }
        }

        /* Verify the address */
        addr = bcmbd_block_port_addr(unit, cad->blknum, lane,
                                     symbol->addr, idx * step);
        if (cad->addr != addr) {
            /* Continue for the next symbol. */
            return 0;
        }
    }

    /* The symbol is found if we get here. */
    cad->is_decoded = 1;
    cad->sid = sid;
    sal_memset(dyn_info, 0, sizeof(*dyn_info));
    dyn_info->index = idx;
    if (acctype == 31) {
        /* The SID is with Unique access type */
        dyn_info->tbl_inst = is_dup_acc ? -1 : cad->acctype;
    } else if (port != -1) {
        dyn_info->tbl_inst = port;
    } else if (baseidx != -1) {
        dyn_info->tbl_inst = baseidx;
    } else {
        dyn_info->tbl_inst = cad->blkinst;
    }

    /* Stop the symbol iteration. */
    return -1;
}

/*******************************************************************************
 * Public functions
 */

uint32_t
bcmbd_cmicd_pipe_info(int unit, uint32_t addr, int acctype, int blktype,
                      int baseidx)
{
    bcmdrd_pipe_info_t drd_pi, *pi = &drd_pi;
    uint32_t pipe_info = 0;

    sal_memset(pi, 0, sizeof(*pi));
    pi->offset = addr;
    pi->acctype = acctype;
    pi->blktype = blktype;
    pi->baseidx = baseidx;

    if (baseidx < 0) {
        uint32_t pmask, linst, sect_shft, dup_acc;

        pmask = bcmdrd_dev_pipe_info(unit, pi, BCMDRD_PIPE_INFO_TYPE_PMASK);
        linst = bcmdrd_dev_pipe_info(unit, pi, BCMDRD_PIPE_INFO_TYPE_LINST);
        sect_shft = bcmdrd_dev_pipe_info(unit, pi,
                                         BCMDRD_PIPE_INFO_TYPE_SECT_SHFT);
        dup_acc = bcmdrd_dev_pipe_info(unit, pi,
                                       BCMDRD_PIPE_INFO_TYPE_AT_UNIQUE_GLOBAL);

        pipe_info = BCMBD_CMICD_PIPE_INFO_SET(pmask, linst, sect_shft, dup_acc);
    } else {
        pipe_info = bcmdrd_dev_pipe_info(unit, pi,
                                         BCMDRD_PIPE_INFO_TYPE_BASETYPE_PMASK);
    }

    return pipe_info;
}

/*
 * Implemetation for accessing register/memory
 * from macro defined in <chip>_acc.h for CMICd
 */

int
bcmbd_cmicd_mem_blocks_read(int unit, uint32_t blkacc, int port,
                            uint32_t offset, int idx,
                            uint32_t *data, size_t wsize)
{
    int rv = SHR_E_PORT;
    bcmdrd_port_num_domain_t pnd;
    bcmdrd_pbmp_t pbmp;
    const bcmdrd_chip_info_t *cinfo;
    const bcmdrd_block_t *blkp;

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return SHR_E_INTERNAL;
    }

    /* Iterate over all physical blocks of this type */
    blkp = bcmdrd_chip_block_types_match(cinfo, blkacc, NULL);
    while (blkp) {
        BCMDRD_PBMP_ASSIGN(pbmp, blkp->pbmps);
        pnd = bcmdrd_chip_port_num_domain(cinfo, -1, blkp->type);
        if (pnd == BCMDRD_PND_PHYS) {
            BCMDRD_PBMP_AND(pbmp, cinfo->valid_pbmps[BCMDRD_PND_PHYS]);
        }
        /* Skip unused blocks */
        if ((port < 0  && BCMDRD_PBMP_NOT_NULL(pbmp)) ||
            BCMDRD_PBMP_MEMBER(blkp->pbmps, port)) {
            rv = cmicd_mem_block_read(unit, blkacc, blkp->blknum,
                                      offset, idx, data, wsize);
            break;
        }
        blkp = bcmdrd_chip_block_types_match(cinfo, blkacc, blkp + 1);
    }
    return rv;
}

int
bcmbd_cmicd_mem_blocks_write(int unit, uint32_t blkacc, int port,
                             uint32_t offset, int idx,
                             uint32_t *data, size_t wsize)
{
    int rv = SHR_E_PORT;
    bcmdrd_port_num_domain_t pnd;
    bcmdrd_pbmp_t pbmp;
    const bcmdrd_chip_info_t *cinfo;
    const bcmdrd_block_t *blkp;

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return SHR_E_INTERNAL;
    }

    /* Iterate over all physical blocks of this type */
    blkp = bcmdrd_chip_block_types_match(cinfo, blkacc, NULL);
    while (blkp) {
        BCMDRD_PBMP_ASSIGN(pbmp, blkp->pbmps);
        pnd = bcmdrd_chip_port_num_domain(cinfo, -1, blkp->type);
        if (pnd == BCMDRD_PND_PHYS) {
            BCMDRD_PBMP_AND(pbmp, cinfo->valid_pbmps[BCMDRD_PND_PHYS]);
        }
        /* Skip unused blocks */
        if ((port < 0  && BCMDRD_PBMP_NOT_NULL(pbmp)) ||
            BCMDRD_PBMP_MEMBER(blkp->pbmps, port)) {
            rv = cmicd_mem_block_write(unit, blkacc, blkp->blknum,
                                       offset, idx, data, wsize);
            if (SHR_FAILURE(rv)) {
                break;
            }
        }
        blkp = bcmdrd_chip_block_types_match(cinfo, blkacc, blkp + 1);
    }
    return rv;
}

int
bcmbd_cmicd_mem_blocks_clear(int unit, uint32_t blkacc, uint32_t offset,
                             uint32_t si, uint32_t ei, int wsize)
{
    int rv = SHR_E_PORT;
    bcmdrd_port_num_domain_t pnd;
    bcmdrd_pbmp_t pbmp;
    const bcmdrd_chip_info_t *cinfo;
    const bcmdrd_block_t *blkp;

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return SHR_E_INTERNAL;
    }

    /* Iterate over all physical blocks of this type */
    blkp = bcmdrd_chip_block_types_match(cinfo, blkacc, NULL);
    while (blkp) {
        BCMDRD_PBMP_ASSIGN(pbmp, blkp->pbmps);
        pnd = bcmdrd_chip_port_num_domain(cinfo, -1, blkp->type);
        if (pnd == BCMDRD_PND_PHYS) {
            BCMDRD_PBMP_AND(pbmp, cinfo->valid_pbmps[BCMDRD_PND_PHYS]);
        }
        /* Skip unused blocks and invalid ports */
        if (BCMDRD_PBMP_NOT_NULL(pbmp)) {
            rv = cmicd_mem_block_clear(unit, blkacc, blkp->blknum,
                                       offset, si, ei, wsize);
            if (SHR_FAILURE(rv)) {
                break;
            }
        }
        blkp = bcmdrd_chip_block_types_match(cinfo, blkacc, blkp + 1);
    }
    return rv;
}

int
bcmbd_cmicd_mem_unique_block_read(int unit, uint32_t blkacc, int blknum,
                                  int blkidx, int baseidx,
                                  uint32_t offset, int idx,
                                  uint32_t *data, int wsize)
{
    int blktype, acctype, pipe_offset, linst;
    uint32_t pipe_info, pmask;
    uint32_t addr, adext;
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    if (bcmdrd_chip_block_type(cinfo, blknum, &blktype, NULL) < 0) {
        return SHR_E_FAIL;
    }

    acctype = BCMBD_CMICD_BLKACC2ACCTYPE(blkacc);

    pipe_info = bcmbd_cmicd_pipe_info(unit, offset, acctype, blktype, -1);
    pmask = BCMBD_CMICD_PIPE_PMASK(pipe_info);
    linst = BCMBD_CMICD_PIPE_LINST(pipe_info);

    /* Check Parameters */
    if (blkidx >= 0 && pmask != 0) {
        if (((1 << blkidx) & pmask) == 0) {
            LOG_ERROR(BSL_LS_BCMBD_SOCMEM,
                      (BSL_META_U(unit,
                                  "bcmbd_cmicd_mem_unique_block_read:"
                                  "unique blk idx err "
                                  "block=%d blkidx=%d pmask=0x%02"PRIx32"\n"),
                       blknum, blkidx, pmask));
            return SHR_E_FAIL;
        }
    }
    if (baseidx >= linst && linst > 0) {
        int basetype = BCMBD_CMICD_ADDR2BASETYPE(offset);

        LOG_ERROR(BSL_LS_BCMBD_SOCMEM,
                  (BSL_META_U(unit,
                              "bcmbd_cmicd_mem_unique_block_read:"
                              "base idx err "
                              "block=%d basetype=%d baseidx=%d "
                              "linst=0x%02d\n"),
                   blknum, basetype, baseidx, linst));
        return SHR_E_FAIL;
    }

    addr = bcmbd_block_port_addr(unit, blknum, -1, offset, 0);

    adext = BCMBD_CMICD_BLKACC2ADEXT(blkacc);
    BCMBD_CMICD_ADEXT_BLOCK_SET(adext, blknum);

    if (pmask != 0) {
        if (blkidx < 0) {
            /* Read from first valid unique block if blkidx < 0 */
            if (linst > 0) {
                /* unique block with basetype defined */
                uint32_t inst_mask, pdx;

                inst_mask = bcmbd_cmicd_pipe_info(unit, offset, acctype,
                                                  blktype, baseidx);
                for (pdx = 0; pdx < BCMBD_CMICD_PHYS_INST_MAX; pdx++) {
                    if (inst_mask & (1 << pdx)) {
                        blkidx = pdx;
                        break;
                    }
                }
            } else {
                /* unique block without basetype defined */
                blkidx = 0;
            }
        }
        /* Update access type in address extension */
        BCMBD_CMICD_ADEXT_ACCTYPE_SET(adext, blkidx);
    }

    pipe_offset = baseidx * BCMBD_CMICD_PIPE_SECT_SIZE(pipe_info);

    return bcmbd_cmicd_mem_read(unit, adext, addr + pipe_offset + idx,
                                data, wsize);
}

int
bcmbd_cmicd_mem_unique_block_write(int unit, uint32_t blkacc, int blknum,
                                   int blkidx, int baseidx,
                                   uint32_t offset, int idx,
                                   uint32_t *data, int wsize)
{
    int rv = 0;
    int blktype, acctype, pipe_offset, linst;
    uint32_t pipe_info, pmask;
    uint32_t addr, adext;
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    if (bcmdrd_chip_block_type(cinfo, blknum, &blktype, NULL) < 0) {
        return SHR_E_FAIL;
    }

    acctype = BCMBD_CMICD_BLKACC2ACCTYPE(blkacc);

    pipe_info = bcmbd_cmicd_pipe_info(unit, offset, acctype, blktype, -1);
    pmask = BCMBD_CMICD_PIPE_PMASK(pipe_info);
    linst = BCMBD_CMICD_PIPE_LINST(pipe_info);

    /* Check Parameters */
    if (blkidx >= 0 && pmask != 0) {
        if (((1 << blkidx) & pmask) == 0) {
            LOG_ERROR(BSL_LS_BCMBD_SOCMEM,
                      (BSL_META_U(unit,
                                  "bcmbd_cmicd_mem_unique_block_write:"
                                  "unique blk idx err "
                                  "block=%d blkidx=%d pmask=0x%02"PRIx32"\n"),
                       blknum, blkidx, pmask));
            return SHR_E_FAIL;
        }
    }
    if (baseidx >= linst && linst > 0) {
        int basetype = BCMBD_CMICD_ADDR2BASETYPE(offset);

        LOG_ERROR(BSL_LS_BCMBD_SOCMEM,
                  (BSL_META_U(unit,
                              "bcmbd_cmicd_mem_unique_block_write:"
                              "base idx err "
                              "block=%d basetype=%d baseidx=%d "
                              "linst=0x%02d\n"),
                   blknum, basetype, baseidx, linst));
        return SHR_E_FAIL;
    }

    addr = bcmbd_block_port_addr(unit, blknum, -1, offset, 0);

    adext = BCMBD_CMICD_BLKACC2ADEXT(blkacc);
    BCMBD_CMICD_ADEXT_BLOCK_SET(adext, blknum);

    if (pmask != 0) {
        if (blkidx < 0) {
            /* Write all unique blocks if blkidx < 0 */
            if (linst > 0) {
                /* unique block with basetype defined */
                uint32_t inst_mask;
                int pdx;

                inst_mask = bcmbd_cmicd_pipe_info(unit, offset, acctype,
                                                  blktype, baseidx);
                pipe_offset = baseidx * BCMBD_CMICD_PIPE_SECT_SIZE(pipe_info);
                for (pdx = 0; pdx < BCMBD_CMICD_PHYS_INST_MAX; pdx++) {
                    if (inst_mask & (1 << pdx)) {
                        /* Update access type in address extension */
                        BCMBD_CMICD_ADEXT_ACCTYPE_SET(adext, pdx);
                        rv += bcmbd_cmicd_mem_write(unit, adext,
                                                    addr + pipe_offset + idx,
                                                    data, wsize);
                    }
                }
            } else {
                /* unique block without basetype defined */
                int unique_idx = 0;

                while (((1 << unique_idx) & pmask) != 0) {
                    /* Update access type in address extension */
                    BCMBD_CMICD_ADEXT_ACCTYPE_SET(adext, unique_idx);
                    pipe_offset = baseidx * BCMBD_CMICD_PIPE_SECT_SIZE(pipe_info);
                    rv += bcmbd_cmicd_mem_write(unit, adext,
                                                addr + pipe_offset + idx,
                                                data, wsize);
                    unique_idx++;
                }
            }
        } else {
            /* Update access type in address extension */
            BCMBD_CMICD_ADEXT_ACCTYPE_SET(adext, blkidx);
            pipe_offset = baseidx * BCMBD_CMICD_PIPE_SECT_SIZE(pipe_info);
            rv = bcmbd_cmicd_mem_write(unit, adext, addr + pipe_offset + idx,
                                       data, wsize);
        }
    } else {
        pipe_offset = baseidx * BCMBD_CMICD_PIPE_SECT_SIZE(pipe_info);
        rv = bcmbd_cmicd_mem_write(unit, adext, addr + pipe_offset + idx,
                                   data, wsize);
    }

    return (rv == 0) ? SHR_E_NONE : SHR_E_FAIL;
}

int
bcmbd_cmicd_reg_unique_block_read(int unit, uint32_t blkacc, int blknum,
                                  int blkidx, int baseidx,
                                  uint32_t offset, int idx,
                                  uint32_t *data, size_t wsize)
{
    int blktype, acctype, linst;
    uint32_t pipe_info, pmask;
    uint32_t addr, adext;
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    if (baseidx < 0) {
        return SHR_E_FAIL;
    }

    if (bcmdrd_chip_block_type(cinfo, blknum, &blktype, NULL) < 0) {
        return SHR_E_FAIL;
    }

    acctype = BCMBD_CMICD_BLKACC2ACCTYPE(blkacc);

    pipe_info = bcmbd_cmicd_pipe_info(unit, offset, acctype, blktype, -1);
    pmask = BCMBD_CMICD_PIPE_PMASK(pipe_info);
    linst = BCMBD_CMICD_PIPE_LINST(pipe_info);

    /* Check Parameters */
    if (blkidx >= 0 && pmask != 0) {
        if (((1 << blkidx) & pmask) == 0) {
            LOG_ERROR(BSL_LS_BCMBD_SOCREG,
                      (BSL_META_U(unit,
                                  "cmicd_reg_unique_blockport_read:"
                                  "unique blk idx err "
                                  "block=%d blkidx=%d pmask=0x%02"PRIx32"\n"),
                       blknum, blkidx, pmask));
            return SHR_E_FAIL;
        }
    }
    if (baseidx >= linst && linst > 0) {
        int basetype = BCMBD_CMICD_ADDR2BASETYPE(offset);

        LOG_ERROR(BSL_LS_BCMBD_SOCREG,
                  (BSL_META_U(unit,
                              "cmicd_reg_unique_blockport_read:"
                              "base idx err "
                              "block=%d basetype=%d baseidx=%d "
                              "linst=0x%02d\n"),
                   blknum, basetype, baseidx, linst));
        return SHR_E_FAIL;
    }

    addr = bcmbd_block_port_addr(unit, blknum, baseidx, offset, idx);

    adext = BCMBD_CMICD_BLKACC2ADEXT(blkacc);
    BCMBD_CMICD_ADEXT_BLOCK_SET(adext, blknum);

    if (pmask != 0) {
        if (blkidx < 0) {
            /* Read from first valid unique block if blkidx < 0 */
            if (linst > 0) {
                /* unique block with basetype defined */
                uint32_t inst_mask, pdx;

                inst_mask = bcmbd_cmicd_pipe_info(unit, offset, acctype,
                                                  blktype, baseidx);
                for (pdx = 0; pdx < BCMBD_CMICD_PHYS_INST_MAX; pdx++) {
                    if (inst_mask & (1 << pdx)) {
                        blkidx = pdx;
                        break;
                    }
                }
            } else { /* unique block without basetype defined */
                blkidx = 0;
            }
        }
        /* Update access type in address extension */
        BCMBD_CMICD_ADEXT_ACCTYPE_SET(adext, blkidx);
    }

    return bcmbd_cmicd_reg_read(unit, adext, addr, data, wsize);
}

int
bcmbd_cmicd_reg_unique_block_write(int unit, uint32_t blkacc, int blknum,
                                   int blkidx, int baseidx,
                                   uint32_t offset, int idx,
                                   uint32_t *data, size_t wsize)
{
    int rv = 0;
    int blktype, acctype, linst;
    uint32_t pipe_info, pmask;
    uint32_t addr, adext;
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    if (baseidx < 0) {
        return SHR_E_FAIL;
    }

    if (bcmdrd_chip_block_type(cinfo, blknum, &blktype, NULL) < 0) {
        return SHR_E_FAIL;
    }

    acctype = BCMBD_CMICD_BLKACC2ACCTYPE(blkacc);

    pipe_info = bcmbd_cmicd_pipe_info(unit, offset, acctype, blktype, -1);
    pmask = BCMBD_CMICD_PIPE_PMASK(pipe_info);
    linst = BCMBD_CMICD_PIPE_LINST(pipe_info);

    /* Check Parameters */
    if (blkidx >= 0 && pmask != 0) {
        if (((1 << blkidx) & pmask) == 0) {
            LOG_ERROR(BSL_LS_BCMBD_SOCREG,
                      (BSL_META_U(unit,
                                  "cmicd_reg_unique_blockport_write:"
                                  "unique blk idx err "
                                  "block=%d blkidx=%d pmask=0x%02"PRIx32"\n"),
                       blknum, blkidx, pmask));
            return SHR_E_FAIL;
        }
    }
    if (baseidx >= linst && linst > 0) {
        int basetype = BCMBD_CMICD_ADDR2BASETYPE(offset);

        LOG_ERROR(BSL_LS_BCMBD_SOCREG,
                  (BSL_META_U(unit,
                              "cmicd_reg_unique_blockport_write:"
                              "base idx err "
                              "block=%d basetype=%d baseidx=%d "
                              "linst=0x%02d\n"),
                   blknum, basetype, baseidx, linst));
        return SHR_E_FAIL;
    }

    addr = bcmbd_block_port_addr(unit, blknum, baseidx, offset, idx);

    adext = BCMBD_CMICD_BLKACC2ADEXT(blkacc);
    BCMBD_CMICD_ADEXT_BLOCK_SET(adext, blknum);

    if (pmask != 0) {
        if (blkidx < 0) {
            /* Write all unique blocks if blkidx < 0 */
            int acc_dup = BCMBD_CMICD_PIPE_AT_DUPLICATE(pipe_info);

            BCMBD_CMICD_ADEXT_ACCTYPE_SET(adext, acc_dup);
            rv += bcmbd_cmicd_reg_write(unit, adext, addr, data,wsize);
        } else {
            /* Update access type in address extension */
            BCMBD_CMICD_ADEXT_ACCTYPE_SET(adext, blkidx);
            rv = bcmbd_cmicd_reg_write(unit, adext, addr, data, wsize);
        }
    } else {
        rv = bcmbd_cmicd_reg_write(unit, adext, addr, data, wsize);
    }

    return (rv == 0) ? SHR_E_NONE : SHR_E_FAIL;
}

int
bcmbd_cmicd_reg_port_read(int unit, uint32_t blkacc, int port,
                          uint32_t offset, int idx, uint32_t *data, int wsize)
{
    uint32_t addr, adext;

    addr = cmicd_port_addr(unit, blkacc, port, offset, idx, &adext);

    return bcmbd_cmicd_reg_read(unit, adext, addr, data, wsize);
}

int
bcmbd_cmicd_reg_port_write(int unit, uint32_t blkacc, int port,
                           uint32_t offset, int idx, uint32_t *data, int wsize)
{
    uint32_t addr, adext;

    addr = cmicd_port_addr(unit, blkacc, port, offset, idx, &adext);

    return bcmbd_cmicd_reg_write(unit, adext, addr, data, wsize);
}

int
bcmbd_cmicd_reg_blocks_read(int unit, uint32_t blkacc, int port,
                            uint32_t offset, int idx,
                            uint32_t *data, size_t wsize)
{
    int rv = SHR_E_PORT;
    bcmdrd_port_num_domain_t pnd;
    bcmdrd_pbmp_t pbmp;
    const bcmdrd_chip_info_t *cinfo;
    const bcmdrd_block_t *blkp;

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return SHR_E_INTERNAL;
    }

    /* Iterate over all physical blocks of this type */
    blkp = bcmdrd_chip_block_types_match(cinfo, blkacc, NULL);
    while (blkp) {
        BCMDRD_PBMP_ASSIGN(pbmp, blkp->pbmps);
        pnd = bcmdrd_chip_port_num_domain(cinfo, -1, blkp->type);
        if (pnd == BCMDRD_PND_PHYS) {
            BCMDRD_PBMP_AND(pbmp, cinfo->valid_pbmps[BCMDRD_PND_PHYS]);
        }
        /* Skip unused blocks */
        if ((port < 0  && BCMDRD_PBMP_NOT_NULL(pbmp)) ||
            BCMDRD_PBMP_MEMBER(blkp->pbmps, port)) {
            rv = bcmbd_cmicd_reg_block_read(unit, blkacc, blkp->blknum,
                                            offset, idx, data, wsize);
            break;
        }
        blkp = bcmdrd_chip_block_types_match(cinfo, blkacc, blkp + 1);
    }
    return rv;
}

int
bcmbd_cmicd_reg_blocks_write(int unit, uint32_t blkacc, int port,
                             uint32_t offset, int idx,
                             uint32_t *data, size_t wsize)
{
    int rv = SHR_E_PORT;
    bcmdrd_port_num_domain_t pnd;
    bcmdrd_pbmp_t pbmp;
    const bcmdrd_chip_info_t *cinfo;
    const bcmdrd_block_t *blkp;

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return SHR_E_INTERNAL;
    }

    /* Iterate over all physical blocks of this type */
    blkp = bcmdrd_chip_block_types_match(cinfo, blkacc, NULL);
    while (blkp) {
        BCMDRD_PBMP_ASSIGN(pbmp, blkp->pbmps);
        pnd = bcmdrd_chip_port_num_domain(cinfo, -1, blkp->type);
        if (pnd == BCMDRD_PND_PHYS) {
            BCMDRD_PBMP_AND(pbmp, cinfo->valid_pbmps[BCMDRD_PND_PHYS]);
        }
        /* Skip unused blocks */
        if ((port < 0  && BCMDRD_PBMP_NOT_NULL(pbmp)) ||
            BCMDRD_PBMP_MEMBER(blkp->pbmps, port)) {
            rv = bcmbd_cmicd_reg_block_write(unit, blkacc, blkp->blknum,
                                             offset, idx, data, wsize);
            if (SHR_FAILURE(rv)) {
                break;
            }
        }
        blkp = bcmdrd_chip_block_types_match(cinfo, blkacc, blkp + 1);
    }
    return rv;
}

int
bcmbd_cmicd_reg_block_read(int unit, uint32_t blkacc, int blknum,
                           uint32_t offset, int idx,
                           uint32_t *data, size_t wsize)
{
    uint32_t addr, adext;

    addr = bcmbd_block_port_addr(unit, blknum, 0, offset, idx);

    adext = BCMBD_CMICD_BLKACC2ADEXT(blkacc);
    BCMBD_CMICD_ADEXT_BLOCK_SET(adext, blknum);

    return bcmbd_cmicd_reg_read(unit, adext, addr, data, wsize);
}

int
bcmbd_cmicd_reg_block_write(int unit, uint32_t blkacc, int blknum,
                            uint32_t offset, int idx,
                            uint32_t *data, size_t wsize)
{
    uint32_t addr, adext;

    addr = bcmbd_block_port_addr(unit, blknum, 0, offset, idx);

    adext = BCMBD_CMICD_BLKACC2ADEXT(blkacc);
    BCMBD_CMICD_ADEXT_BLOCK_SET(adext, blknum);

    return bcmbd_cmicd_reg_write(unit, adext, addr, data, wsize);
}

/*
 * CMICd PT interface for symbol read/write.
 */

int
bcmbd_cmicd_read(int unit, bcmdrd_sid_t sid,
                 bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                 uint32_t *data, size_t wsize)
{
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;

    SHR_FUNC_ENTER(unit);

    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, sinfo))) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    if (sinfo->flags & BCMDRD_SYMBOL_FLAG_MEMMAPPED) {
        uint32_t addr = sinfo->offset +
                        (dyn_info->index * sizeof(uint32_t) * sinfo->step_size);
        if (sinfo->flags & BCMDRD_SYMBOL_FLAG_IPROC) {
            SHR_IF_ERR_EXIT
                (BCMDRD_IPROC_READ(unit, addr, data));
        } else {
            SHR_IF_ERR_EXIT
                (BCMDRD_DEV_READ32(unit, addr, data));
        }
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (cmicd_sym_read(unit, sinfo, dyn_info, ovrr_info, data, wsize));

exit:
    SHR_FUNC_EXIT();
}

int
bcmbd_cmicd_write(int unit, bcmdrd_sid_t sid,
                  bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                  uint32_t *data)
{
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;

    SHR_FUNC_ENTER(unit);

    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, sinfo))) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    if (sinfo->flags & BCMDRD_SYMBOL_FLAG_MEMMAPPED) {
        uint32_t addr = sinfo->offset +
                        (dyn_info->index * sizeof(uint32_t) * sinfo->step_size);
        if (sinfo->flags & BCMDRD_SYMBOL_FLAG_IPROC) {
            SHR_IF_ERR_EXIT
                (BCMDRD_IPROC_WRITE(unit, addr, *data));
        } else {
            SHR_IF_ERR_EXIT
                (BCMDRD_DEV_WRITE32(unit, addr, *data));
        }
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (cmicd_sym_write(unit, sinfo, dyn_info, ovrr_info, data));

exit:
    SHR_FUNC_EXIT();
}

/*
 * CMICd PT interface for table operatioins.
 */

int
bcmbd_cmicd_insert(int unit, bcmdrd_sid_t sid,
                   bcmbd_pt_dyn_hash_info_t *dyn_hash_info,
                   void *ovrr_hash_info,
                   uint32_t *data, size_t wsize,
                   bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data)
{
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    uint32_t adext, addr, bank_ignore_mask;

    SHR_FUNC_ENTER(unit);

    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, sinfo))) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (wsize < sinfo->entry_wsize) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "entry_wsize = %d, wsize = %d\n"),
                             (int)sinfo->entry_wsize, (int)wsize));
    }
    wsize = sinfo->entry_wsize;

    SHR_IF_ERR_EXIT
        (cmicd_sym_tbl_addr(unit, sinfo, dyn_hash_info, ovrr_hash_info,
                            &adext, &addr, &bank_ignore_mask));

    SHR_IF_ERR_EXIT
        (bcmbd_cmicd_tbl_insert(unit, adext, addr, data, wsize,
                                bank_ignore_mask, resp_info, resp_data));

exit:
    SHR_FUNC_EXIT();
}

int
bcmbd_cmicd_delete(int unit, bcmdrd_sid_t sid,
                   bcmbd_pt_dyn_hash_info_t *dyn_hash_info,
                   void *ovrr_hash_info,
                   uint32_t *data, size_t wsize,
                   bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data)
{
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    uint32_t adext, addr, bank_ignore_mask;

    SHR_FUNC_ENTER(unit);

    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, sinfo))) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (wsize < sinfo->entry_wsize) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "entry_wsize = %d, wsize = %d\n"),
                             (int)sinfo->entry_wsize, (int)wsize));
    }
    wsize = sinfo->entry_wsize;

    SHR_IF_ERR_EXIT
        (cmicd_sym_tbl_addr(unit, sinfo, dyn_hash_info, ovrr_hash_info,
                            &adext, &addr, &bank_ignore_mask));

    SHR_IF_ERR_EXIT
        (bcmbd_cmicd_tbl_delete(unit, adext, addr, data, wsize,
                                bank_ignore_mask, resp_info, resp_data));

exit:
    SHR_FUNC_EXIT();
}

int
bcmbd_cmicd_lookup(int unit, bcmdrd_sid_t sid,
                   bcmbd_pt_dyn_hash_info_t *dyn_hash_info,
                   void *ovrr_hash_info,
                   uint32_t *data, size_t wsize,
                   bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data)
{
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    uint32_t adext, addr, bank_ignore_mask;

    SHR_FUNC_ENTER(unit);

    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, sinfo))) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (wsize < sinfo->entry_wsize) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "entry_wsize = %d, wsize = %d\n"),
                             (int)sinfo->entry_wsize, (int)wsize));
    }
    wsize = sinfo->entry_wsize;

    SHR_IF_ERR_EXIT
        (cmicd_sym_tbl_addr(unit, sinfo, dyn_hash_info, ovrr_hash_info,
                            &adext, &addr, &bank_ignore_mask));

    SHR_IF_ERR_EXIT
        (bcmbd_cmicd_tbl_lookup(unit, adext, addr, data, wsize,
                                bank_ignore_mask, resp_info, resp_data));

exit:
    SHR_FUNC_EXIT();
}

/*
 * CMICd PT interface for FIFO operatioins.
 */

int
bcmbd_cmicd_pop(int unit, bcmdrd_sid_t sid,
                bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                uint32_t *data, size_t wsize)
{
    const bcmdrd_chip_info_t *cinfo;
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    cmicd_addr_t ca;
    uint32_t addr, adext;

    SHR_FUNC_ENTER(unit);

    /* Check if the sid is valid for FIFO operations */
    if (!bcmdrd_pt_attr_is_fifo(unit, sid)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (!dyn_info || dyn_info->tbl_inst < 0) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, sinfo))) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (wsize < sinfo->entry_wsize) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "entry_wsize = %d, wsize = %d\n"),
                             (int)sinfo->entry_wsize, (int)wsize));
    }
    wsize = sinfo->entry_wsize;

    SHR_IF_ERR_EXIT
        (cmicd_sym_addr(unit, cinfo, sinfo, dyn_info, ovrr_info, &ca));

    addr = cmicd_raw_addr_get(unit, &ca);
    adext = cmicd_raw_adext_get(unit, &ca);

    SHR_IF_ERR_EXIT
        (bcmbd_cmicd_fifo_pop(unit, adext, addr, data, wsize));

exit:
    SHR_FUNC_EXIT();
}

int
bcmbd_cmicd_push(int unit, bcmdrd_sid_t sid,
                 bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                 uint32_t *data)
{
    const bcmdrd_chip_info_t *cinfo;
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    cmicd_addr_t ca;
    uint32_t wsize;
    uint32_t addr, adext;

    SHR_FUNC_ENTER(unit);

    /* Check if the sid is valid for FIFO operations */
    if (!bcmdrd_pt_attr_is_fifo(unit, sid)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (!dyn_info || dyn_info->tbl_inst < 0) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, sinfo))) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    wsize = sinfo->entry_wsize;

    SHR_IF_ERR_EXIT
        (cmicd_sym_addr(unit, cinfo, sinfo, dyn_info, ovrr_info, &ca));

    addr = cmicd_raw_addr_get(unit, &ca);
    adext = cmicd_raw_adext_get(unit, &ca);

    SHR_IF_ERR_EXIT
        (bcmbd_cmicd_fifo_push(unit, adext, addr, data, wsize));

exit:
    SHR_FUNC_EXIT();
}

int
bcmbd_cmicd_addr_decode(int unit,
                        uint32_t adext, uint32_t addr, uint32_t sym_flags,
                        bcmdrd_sid_t *sid, bcmbd_pt_dyn_info_t *dyn_info)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);
    bcmdrd_addr_decode_f addr_dec_func;
    int blknum = BCMBD_CMICD_ADEXT2BLOCK(adext);
    int acctype = BCMBD_CMICD_ADEXT2ACCTYPE(adext);
    int blktype, blkinst;
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_chip_info_t *cinfo;
    bcmdrd_symbols_iter_t symbols_iter, *iter = &symbols_iter;
    cmicd_addr_dec_cookie_t addr_dec_cookie, *ad = &addr_dec_cookie;

    if (bd == NULL) {
        return SHR_E_FAIL;
    }
    if (sid == NULL || dyn_info == NULL) {
        return SHR_E_FAIL;
    }
    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return SHR_E_FAIL;
    }

    /* Look for device-specific address decoding, otherwise use default */
    addr_dec_func = bcmdrd_dev_addr_decode_func(unit);
    if (addr_dec_func == NULL) {
        addr_dec_func = cmicd_addr_decode;
    }

    if (bcmdrd_chip_block_type(cinfo, blknum, &blktype, &blkinst) < 0) {
        /* Invalid blocknum. */
        return SHR_E_FAIL;
    }

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return SHR_E_FAIL;
    }

    sal_memset(ad, 0, sizeof(*ad));
    ad->unit = unit;
    ad->cinfo = cinfo;
    ad->addr_dec_func = addr_dec_func;
    ad->dup_acc = bcmdrd_dev_pipe_info(unit, NULL,
                                       BCMDRD_PIPE_INFO_TYPE_AT_UNIQUE_GLOBAL);
    ad->num_pipes = bcmdrd_dev_pipe_info(unit, NULL,
                                         BCMDRD_PIPE_INFO_TYPE_NUM_PIPE_INST);
    ad->addr = addr;
    ad->acctype = acctype;
    ad->blknum = blknum;
    ad->blktype = blktype;
    ad->blkinst = blkinst;

    sal_memset(iter, 0, sizeof(*iter));
    iter->name = "*";
    iter->pflags = sym_flags;
    /* Ignore the overlay symbols */
    iter->aflags |= BCMDRD_SYMBOL_FLAG_OVERLAY;
    /* Ignore the memory-mapped registers unless explicit requested */
    if ((sym_flags & BCMDRD_SYMBOL_FLAG_MEMMAPPED) == 0) {
        iter->aflags |= BCMDRD_SYMBOL_FLAG_MEMMAPPED;
    }
    iter->symbols = symbols;
    iter->function = cmicd_symbol_addr_decode;
    iter->vptr = ad;

    if (bcmdrd_symbols_iter(iter) < 0 && ad->is_decoded) {
        /* The address is decoded successfully */
        *sid = ad->sid;
        sal_memcpy(dyn_info, &ad->dyn_info, sizeof(*dyn_info));
        return SHR_E_NONE;
    }
    /*
     * Retry with relaxed checking if a single possible match was
     * found. Typically this means a bad port or pipe.
     */
    if (ad->possible_match == 1) {
        ad->relaxed_match = 1;
        if (bcmdrd_symbols_iter(iter) < 0 && ad->is_decoded) {
            /* The address is decoded successfully */
            *sid = ad->sid;
            sal_memcpy(dyn_info, &ad->dyn_info, sizeof(*dyn_info));
            return SHR_E_NONE;
        }
    }
    return SHR_E_FAIL;
}

int
bcmbd_cmicd_cmd_hdr_get(int unit, bcmdrd_sid_t sid,
                        bcmbd_pt_dyn_info_t *dyn_info,
                        void *ovrr_info, bcmbd_pt_cmd_t cmd,
                        uint32_t wsize, uint32_t *data,
                        uint32_t *hdr_wsize)
{
    const bcmdrd_chip_info_t *cinfo;
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    cmicd_addr_t ca;
    uint32_t addr, adext;
    int opcode, acctype, dstblk, datalen;
    bool is_mem;

    SHR_FUNC_ENTER(unit);

    assert(data);

    /* S-Channel message header is two 32-bit words */
    if (wsize < 2) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    if (hdr_wsize) {
        *hdr_wsize = 2;
    }

    /* Get symbol info to calculate address */
    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, sinfo))) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    if (sinfo->flags & BCMDRD_SYMBOL_FLAG_MEMMAPPED) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (cmicd_sym_addr(unit, cinfo, sinfo, dyn_info, ovrr_info, &ca));

    addr = cmicd_raw_addr_get(unit, &ca);
    adext = cmicd_raw_adext_get(unit, &ca);

    is_mem = (sinfo->flags & BCMDRD_SYMBOL_FLAG_MEMORY) ? true : false;

    switch (cmd) {
    case BCMBD_PT_CMD_WRITE:
        opcode = is_mem ? WRITE_MEMORY_CMD_MSG : WRITE_REGISTER_CMD_MSG;
        break;
    case BCMBD_PT_CMD_READ:
        opcode = is_mem ? READ_MEMORY_CMD_MSG : READ_REGISTER_CMD_MSG;
        break;
    case BCMBD_PT_CMD_POP:
        if (is_mem) {
            opcode = FIFO_POP_CMD_MSG;
        } else {
            return SHR_E_PARAM;
        }
        break;
    default:
        return SHR_E_UNAVAIL;
    }

    /* Configure S-Channel parameters */
    acctype = BCMBD_CMICD_ADEXT2ACCTYPE(adext);
    dstblk = BCMBD_CMICD_ADEXT2BLOCK(adext);
    datalen = 4 * sinfo->entry_wsize;

    /*
     * Create S-Channel message header
     *
     * NOTE: the datalen field matters only for the Write Memory and
     * Write Register commands, where it is used only by the CMIC to
     * determine how much data to send, and is in units of bytes.
     */
    data[0] = 0;
    SCMH_OPCODE_SET(data[0], opcode);
    SCMH_ACCTYPE_SET(data[0], acctype);
    SCMH_DSTBLK_SET(data[0], dstblk);
    SCMH_DATALEN_SET(data[0], datalen);
    data[1] = addr;

  exit:
    SHR_FUNC_EXIT();
}

/*
 * CMICd driver init and cleanup
 */

int
bcmbd_cmicd_drv_init(int unit)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    SHR_FUNC_ENTER(unit);

    if (bd == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_FAIL);
    }

    /* Look for device-specific address calculation, otherwise use default */
    bd->block_port_addr = bcmdrd_dev_block_port_addr_func(unit);
    if (bd->block_port_addr == NULL) {
        bd->block_port_addr = cmicd_block_port_addr;
    }

    bd->pt_read = bcmbd_cmicd_read;
    bd->pt_write = bcmbd_cmicd_write;
    bd->pt_insert = bcmbd_cmicd_insert;
    bd->pt_delete = bcmbd_cmicd_delete;
    bd->pt_lookup = bcmbd_cmicd_lookup;
    bd->pt_pop = bcmbd_cmicd_pop;
    bd->pt_push = bcmbd_cmicd_push;
    bd->pt_addr_decode = bcmbd_cmicd_addr_decode;
    bd->pt_cmd_hdr_get = bcmbd_cmicd_cmd_hdr_get;

exit:
    SHR_FUNC_EXIT();
}

int
bcmbd_cmicd_drv_cleanup(int unit)
{
    return SHR_E_NONE;
}
