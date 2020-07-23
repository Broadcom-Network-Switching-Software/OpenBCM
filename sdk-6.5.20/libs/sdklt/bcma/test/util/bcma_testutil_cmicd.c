/*! \file bcma_testutil_cmicd.c
 *
 * CMICD utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmbd/bcmbd_sbusdma.h>

#include <bcmbd/bcmbd_cmicd.h>
#include <bcmbd/bcmbd_cmicd_mem.h>

#include <bcma/test/util/bcma_testutil_cmicd.h>

static void
mem_address_encode(int unit, bcmdrd_sid_t sid, int index, int tbl_inst,
                   uint32_t *addr, uint32_t *adext)
{
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);
    bcmdrd_sym_info_t sinfo;
    int blktype, blknum, acctype;
    uint32_t offset, pipe_info, pmask, linst, sect_size;

    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, &sinfo))) {
        *addr = 0;
        *adext = 0;
        return;
    }

    offset = sinfo.offset;
    index *= sinfo.step_size;

    blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
    acctype = bcmdrd_pt_acctype_get(unit, sid);
    blknum = bcmdrd_chip_block_number(cinfo, blktype, 0);

    pipe_info = bcmbd_cmicd_pipe_info(unit, offset, acctype,
                                      blktype, -1);
    /* pmask > 0 if (access type == UNIQUE && (block type == IP/EP/MMU_SC/MMU_XPE)) */
    pmask = BCMBD_CMICD_PIPE_PMASK(pipe_info);
    /* linst > 0 if (block type == MMU_SC/MMU_XPE/MMU_GLB), indicate the number of base index */
    linst = BCMBD_CMICD_PIPE_LINST(pipe_info);
    sect_size = BCMBD_CMICD_PIPE_SECT_SIZE(pipe_info);

    if (pmask != 0) {
        /* Select a specific block instance for unique access type */
        acctype = tbl_inst;
    }

    if (linst != 0 && sect_size != 0) {
        offset += tbl_inst * sect_size;
    }

    *adext = 0;
    BCMBD_CMICD_ADEXT_BLOCK_SET(*adext, blknum);
    BCMBD_CMICD_ADEXT_ACCTYPE_SET(*adext, acctype);

    *addr = bcmbd_block_port_addr(unit, blknum, -1, offset, index);
}

int
bcma_testutil_cmicd_mem_range_write(int unit, bcmdrd_sid_t sid, int index_start,
                                    int index_end, int tbl_inst, uint64_t buf)
{
    uint32_t entry_wsize, addr, adext;
    int flags = 0, count;

    entry_wsize = bcmdrd_pt_entry_wsize(unit, sid);

    mem_address_encode(unit, sid, index_start, tbl_inst, &addr, &adext);

    if (index_start > index_end) {
        flags |= BCMBD_ROF_INV_ADDR;
        count = index_start - index_end + 1;
    } else {
        count = index_end - index_start + 1;
    }

    return bcmbd_cmicd_mem_range_write(unit, adext, addr, entry_wsize, 0,
                                       count, buf, flags);
}

int
bcma_testutil_cmicd_mem_range_read(int unit, bcmdrd_sid_t sid, int index_start,
                                   int index_end, int tbl_inst, uint64_t buf)
{
    uint32_t entry_wsize, addr, adext;
    int flags = 0, count;

    entry_wsize = bcmdrd_pt_entry_wsize(unit, sid);

    mem_address_encode(unit, sid, index_start, tbl_inst, &addr, &adext);

    if (index_start > index_end) {
        flags |= BCMBD_ROF_INV_ADDR;
        count = index_start - index_end + 1;
    } else {
        count = index_end - index_start + 1;
    }

    return bcmbd_cmicd_mem_range_read(unit, adext, addr, entry_wsize, 0,
                                      count, buf, flags);
}
