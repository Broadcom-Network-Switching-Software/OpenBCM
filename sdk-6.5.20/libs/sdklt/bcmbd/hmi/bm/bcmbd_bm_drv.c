/*! \file bcmbd_bm_drv.c
 *
 * Top-level APIs for BM access.
 *
 * The primary PT API maps directly to the BM API, so the port/block
 * access functions are only to allow us to work with the built-in
 * simulator.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_assert.h>
#include <sal/sal_libc.h>

#include <shr/shr_debug.h>

#include <bcmbd/bcmbd.h>
#include <bcmbd/bcmbd_simhook.h>
#include <bcmbd/bcmbd_bm.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_pt.h>

/*******************************************************************************
 * BM PT interface for symbol read/write.
 */

static uint32_t
bcmbd_bm_block_port_addr(int block, int lane,
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
bcmbd_bm_block_addr(int unit, bcmdrd_sym_info_t *sinfo,
                    bcmbd_pt_dyn_info_t *dyn_info,
                    uint32_t *addr)
{
    const bcmdrd_chip_info_t *cinfo;
    bcmdrd_pblk_t pblk;
    int blktype, blkinst, blknum, port;

    assert(addr);

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return -1;
    }

    blktype = bcmdrd_pt_blktype_get(unit, sinfo->sid, 0);

    /*
     * For BM, device addresses are always base addresses which are
     * converted to BM SIDs, with the table index passed separately,
     * so pass an index of zero when calculating addr.
     */
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
            return -1;
        }
        blknum = pblk.blknum;
        *addr = bcmbd_block_port_addr(unit, blknum, pblk.lane,
                                      sinfo->offset, 0);
    } else {
        blkinst = dyn_info->tbl_inst;
        if (blkinst < 0) {
            blkinst = 0;
        }
        blknum = bcmdrd_chip_block_number(cinfo, blktype, blkinst);
        if (blknum < 0) {
            return -1;
        }
        *addr = bcmbd_block_port_addr(unit, blknum, -1,
                                      sinfo->offset, 0);
    }
    return 0;
}

static int
bcmbd_bm_model_addr(int unit, bcmdrd_sid_t sid,
                    bcmbd_pt_dyn_info_t *dyn_info,
                    uint32_t *addr)
{
    int rv;
    int err;
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    uint32_t bm_addr;
    rv = bcmdrd_pt_info_get(unit, sid, sinfo);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    err = bcmbd_bm_block_addr(unit, sinfo, dyn_info, &bm_addr);
    if (err < 0) {
        return SHR_E_FAIL;
    }

    *addr = bm_addr >> 16; /* Is there an official conversion? */

    return rv;
}

int
bcmbd_bm_read(int unit, bcmdrd_sid_t sid,
                    bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                    uint32_t *data, size_t wsize)
{
    int rv;
    uint32_t addr;

    /* Simulator hooks */
    if (bcmbd_simhook_read == NULL) {
        return SHR_E_UNAVAIL;
    }

    rv = bcmbd_bm_model_addr(unit, sid, dyn_info, &addr);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* dyn_info->tbl_inst ignored */
    return bcmbd_simhook_read(unit, dyn_info->index, addr,
                              data, BCMDRD_WORDS2BYTES(wsize));
}

int
bcmbd_bm_write(int unit, bcmdrd_sid_t sid,
                     bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                     uint32_t *data)
{
    int rv;
    uint32_t addr;

    /* Simulator hooks */
    if (bcmbd_simhook_write == NULL) {
        return SHR_E_UNAVAIL;
    }

    rv = bcmbd_bm_model_addr(unit, sid, dyn_info, &addr);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* dyn_info->tbl_inst ignored */
    return bcmbd_simhook_write(unit, dyn_info->index, addr,
                               data, BCMDRD_WORDS2BYTES(BCMDRD_MAX_PT_WSIZE));
}

/*******************************************************************************
 * BM driver init and cleanup
 */

int
bcmbd_bm_drv_init(int unit)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_FAIL;
    }

    /* Look for device-specific address calculation, otherwise use default */
    bd->block_port_addr = bcmdrd_dev_block_port_addr_func(unit);
    if (bd->block_port_addr == NULL) {
        bd->block_port_addr = bcmbd_bm_block_port_addr;
    }

    bd->pt_read = bcmbd_bm_read;
    bd->pt_write = bcmbd_bm_write;

    return SHR_E_NONE;
}

int
bcmbd_bm_drv_cleanup(int unit)
{
    return SHR_E_NONE;
}
