/*! \file bcma_bcmbd_blocks.c
 *
 * BCMBD command block utility functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_assert.h>

#include <bcmdrd/bcmdrd_chip.h>
#include <bcmdrd/bcmdrd_dev.h>

#include <bcma/bcmbd/bcma_bcmbd.h>

static char *badblk = "*BADBLK*";

const char *
bcma_bcmbd_block_type2name(int unit, int blktype)
{
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    assert(cinfo);

    if (blktype >= 0 && blktype < cinfo->nblktypes) {
        return cinfo->blktype_names[blktype];
    }
    return NULL;
}

int
bcma_bcmbd_block_name2type(int unit, const char *name)
{
    const bcmdrd_chip_info_t *cinfo;

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return -1;
    }

    return bcmdrd_chip_blktype_get_by_name(cinfo, name);
}

char*
bcma_bcmbd_block_name(int unit, int blknum, char *dst, int dstsz)
{
    int blktype, blkinst;
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    assert(cinfo);

    if (bcmdrd_chip_block_type(cinfo, blknum, &blktype, &blkinst) >= 0) {
        const char *blkname = bcma_bcmbd_block_type2name(unit, blktype);

        if (blkname) {
            sal_snprintf(dst, dstsz, "%s%d", blkname, blkinst);
            return dst;
        }
    }
    return badblk;
}
