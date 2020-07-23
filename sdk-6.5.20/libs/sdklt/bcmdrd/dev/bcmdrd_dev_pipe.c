/*! \file bcmdrd_dev_pipe.c
 *
 * Pipeline index for different port number domains.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_dev.h>

static int
pnd_port_pipe(const bcmdrd_chip_info_t *cinfo,
                        bcmdrd_port_num_domain_t pnd, int port)
{
    bcmdrd_pipe_info_t pipe_info, *pi = &pipe_info;

    sal_memset(pi, 0, sizeof(*pi));
    pi->pnd = pnd;
    pi->port = port;
    return bcmdrd_chip_pipe_info(cinfo, pi,
                                 BCMDRD_PIPE_INFO_TYPE_PIPE_INDEX_FROM_PORT);
}

int
bcmdrd_dev_phys_port_pipe(int unit, int port)
{
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);
    bcmdrd_port_num_domain_t pnd = BCMDRD_PND_PHYS;

    if (cinfo == NULL) {
        return -1;
    }

    if (!BCMDRD_PBMP_MEMBER(cinfo->valid_pbmps[pnd], port)) {
        return -1;
    }

    return pnd_port_pipe(cinfo, pnd, port);
}

int
bcmdrd_dev_logic_port_pipe(int unit, int port)
{
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);
    bcmdrd_port_num_domain_t pnd = BCMDRD_PND_LOGIC;

    if (cinfo == NULL) {
        return -1;
    }

    if (!BCMDRD_PBMP_MEMBER(cinfo->valid_pbmps[pnd], port)) {
        return -1;
    }

    return pnd_port_pipe(cinfo, pnd, port);
}

int
bcmdrd_dev_mmu_port_pipe(int unit, int port)
{
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);
    bcmdrd_port_num_domain_t pnd = BCMDRD_PND_MMU;

    if (cinfo == NULL) {
        return -1;
    }

    if (!BCMDRD_PBMP_MEMBER(cinfo->valid_pbmps[pnd], port)) {
        return -1;
    }

    return pnd_port_pipe(cinfo, pnd, port);
}

int
bcmdrd_dev_blk_port_pipe(int unit, int blktype, int port)
{
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);
    bcmdrd_port_num_domain_t pnd;
    int dev_pipe_no, blk_pipe_no = 0;
    uint32_t dev_pipe_map, blk_pipe_map = 0;

    if (cinfo == NULL) {
        return -1;
    }

    pnd = bcmdrd_chip_port_num_domain(cinfo, -1, blktype);
    if (!BCMDRD_PBMP_MEMBER(cinfo->valid_pbmps[pnd], port)) {
        return -1;
    }

    dev_pipe_no = pnd_port_pipe(cinfo, pnd, port);
    dev_pipe_map = 1 << dev_pipe_no;
    if (SHR_FAILURE(bcmdrd_dev_valid_blk_pipes_get(unit, dev_pipe_map, blktype,
                                                   &blk_pipe_map))) {
        return -1;
    }

    if (blk_pipe_map == dev_pipe_map) {
        return dev_pipe_no;
    }

    while (blk_pipe_map) {
        if (blk_pipe_map & 1) {
            break;
        }
        blk_pipe_map >>= 1;
        blk_pipe_no++;
    }

    return blk_pipe_no;
}
