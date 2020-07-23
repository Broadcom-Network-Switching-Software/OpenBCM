/*! \file bcmdrd_dev_num_ports.c
 *
 * Functions to get number of ports in DRD.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_dev.h>

static int
pbmp_num_ports(bcmdrd_pbmp_t *pbmp)
{
    int port, num_ports = 0;

    BCMDRD_PBMP_ITER(*pbmp, port) {
        num_ports++;
    }
    return num_ports;
}

int
bcmdrd_dev_pipe_num_phys_ports(int unit, int pipe_no)
{
    bcmdrd_pbmp_t pbmp;

    if (bcmdrd_dev_pipe_phys_pbmp(unit, pipe_no, &pbmp) < 0) {
        return -1;
    }
    return pbmp_num_ports(&pbmp);
}

int
bcmdrd_dev_pipe_num_logic_ports(int unit, int pipe_no)
{
    bcmdrd_pbmp_t pbmp;

    if (bcmdrd_dev_pipe_logic_pbmp(unit, pipe_no, &pbmp) < 0) {
        return -1;
    }
    return pbmp_num_ports(&pbmp);
}

int
bcmdrd_dev_pipe_num_mmu_ports(int unit, int pipe_no)
{
    bcmdrd_pbmp_t pbmp;

    if (bcmdrd_dev_pipe_mmu_pbmp(unit, pipe_no, &pbmp) < 0) {
        return -1;
    }
    return pbmp_num_ports(&pbmp);
}

int
bcmdrd_dev_blk_pipe_num_ports(int unit, int blktype, int blk_pipe_no)
{
    bcmdrd_pbmp_t pbmp;

    if (bcmdrd_dev_blk_pipe_pbmp(unit, blktype, blk_pipe_no, &pbmp) < 0) {
        return -1;
    }
    return pbmp_num_ports(&pbmp);
}
