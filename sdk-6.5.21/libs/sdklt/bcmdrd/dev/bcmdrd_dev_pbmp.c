/*! \file bcmdrd_dev_pbmp.c
 *
 * Device port bit maps in DRD.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_dev.h>

int
bcmdrd_dev_phys_pbmp(int unit, bcmdrd_pbmp_t *pbmp)
{
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    return bcmdrd_chip_valid_pbmp(cinfo, BCMDRD_PND_PHYS, pbmp);
}

int
bcmdrd_dev_logic_pbmp(int unit, bcmdrd_pbmp_t *pbmp)
{
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    return bcmdrd_chip_valid_pbmp(cinfo, BCMDRD_PND_LOGIC, pbmp);
}

int
bcmdrd_dev_mmu_pbmp(int unit, bcmdrd_pbmp_t *pbmp)
{
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    return bcmdrd_chip_valid_pbmp(cinfo, BCMDRD_PND_MMU, pbmp);
}

int
bcmdrd_dev_blktype_pbmp(int unit, int blktype, bcmdrd_pbmp_t *pbmp)
{
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    return bcmdrd_chip_blktype_pbmp(cinfo, blktype, pbmp);
}

int
bcmdrd_dev_pipe_phys_pbmp(int unit, int pipe_no, bcmdrd_pbmp_t *pbmp)
{
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    return bcmdrd_chip_pipe_pbmp(cinfo, pipe_no, BCMDRD_PND_PHYS, pbmp);
}

int
bcmdrd_dev_pipe_logic_pbmp(int unit, int pipe_no, bcmdrd_pbmp_t *pbmp)
{
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    return bcmdrd_chip_pipe_pbmp(cinfo, pipe_no, BCMDRD_PND_LOGIC, pbmp);
}

int
bcmdrd_dev_pipe_mmu_pbmp(int unit, int pipe_no, bcmdrd_pbmp_t *pbmp)
{
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    return bcmdrd_chip_pipe_pbmp(cinfo, pipe_no, BCMDRD_PND_MMU, pbmp);
}

int
bcmdrd_dev_blk_pipe_pbmp(int unit, int blktype, int blk_pipe_no,
                         bcmdrd_pbmp_t *pbmp)
{
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);
    bcmdrd_port_num_domain_t pnd;
    bcmdrd_pbmp_t pipe_pbmp;
    uint32_t dev_pipe_map = 0, blk_pipe_map = 0;
    int dev_pipe_no = 0;
    int rv;

    if (cinfo == NULL || pbmp == NULL) {
        return -1;
    }
    pnd = bcmdrd_chip_port_num_domain(cinfo, -1, blktype);
    BCMDRD_PBMP_CLEAR(*pbmp);

    if (SHR_FAILURE(bcmdrd_dev_valid_pipes_get(unit, &dev_pipe_map))) {
        return -1;
    }

    /* Get the mapped device pipe(s) of the block pipe number. */
    while (dev_pipe_map) {
        if (dev_pipe_map & 1) {
            rv = bcmdrd_dev_valid_blk_pipes_get(unit, 1 << dev_pipe_no, blktype,
                                                &blk_pipe_map);
            if (SHR_FAILURE(rv)) {
                return -1;
            }
            if (blk_pipe_map & (1 << blk_pipe_no)) {
                rv = bcmdrd_chip_pipe_pbmp(cinfo, dev_pipe_no, pnd, &pipe_pbmp);
                if (rv < 0) {
                    return -1;
                }
                BCMDRD_PBMP_OR(*pbmp, pipe_pbmp);
            }
        }
        dev_pipe_map >>= 1;
        dev_pipe_no++;
    }

    return 0;
}

int
bcmdrd_dev_lb_pbmp(int unit, bcmdrd_pbmp_t *pbmp)
{
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    return bcmdrd_chip_port_type_pbmp(cinfo, BCMDRD_PORT_TYPE_LB, pbmp);
}
