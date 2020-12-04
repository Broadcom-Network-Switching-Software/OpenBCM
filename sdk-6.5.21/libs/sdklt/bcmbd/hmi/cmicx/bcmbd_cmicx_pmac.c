/*! \file bcmbd_cmicx_pmac.c
 *
 * PMAC register access driver for CMICX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <shr/shr_pmac.h>
#include <bcmbd/bcmbd.h>
#include <bcmbd/bcmbd_cmicx_reg.h>
#include <bcmbd/bcmbd_cmicx_mem.h>
#include <bcmbd/bcmbd_cmicx_pmac.h>


/* Convert bytes to words */
#define CMICX_BYTES2WORDS(_bytes_)  ((_bytes_ + 3) / 4)

/* Set RT bit of CMICX register address */
#define CMICX_ADDR_RT_SET(_addr_)    _addr_ |= 0x2000000

typedef struct cmicx_addr_s {
    uint32_t addr;
    uint32_t adext;
} cmicx_addr_t;

/* Convert PMAC address to CMICX address */
static void
cmicx_addr_get(int unit, int blk_id, uint32_t lane,
               uint32_t pmac_addr, uint32_t idx, cmicx_addr_t *cmicx_addr)
{
    int acc_type;
    uint32_t reg_type, reg_addr;

    acc_type = SHR_PMAC_ACC_TYPE_GET(pmac_addr);
    reg_type = SHR_PMAC_REG_TYPE_GET(pmac_addr);
    reg_addr = SHR_PMAC_REG_ADDR_GET(pmac_addr);

    /* CMICX address extension */
    cmicx_addr->adext = 0;
    BCMBD_CMICX_ADEXT_ACCTYPE_SET(cmicx_addr->adext, acc_type);
    BCMBD_CMICX_ADEXT_BLOCK_SET(cmicx_addr->adext, blk_id);

    /* CMICX address */
    if (reg_type == SHR_PMAC_REGTYPE_MEM) {
        cmicx_addr->addr = bcmbd_block_port_addr(unit, blk_id, -1,
                                                 reg_addr, idx);
    } else if (reg_type == SHR_PMAC_REGTYPE_BLK) {
        cmicx_addr->addr = bcmbd_block_port_addr(unit, blk_id, 0,
                                                 reg_addr, idx);

        /* Set RT bit for block register */
        CMICX_ADDR_RT_SET(cmicx_addr->addr);
    } else {
        cmicx_addr->addr = bcmbd_block_port_addr(unit, blk_id, lane,
                                                 reg_addr, idx);
    }
}

int
bcmbd_cmicx_pmac_read(int unit, int blk_id, int port, uint32_t addr,
                      uint32_t idx, size_t size, uint32_t *data)
{
    cmicx_addr_t cmicx_addr;

    cmicx_addr_get(unit, blk_id, port, addr, idx, &cmicx_addr);

    if (SHR_PMAC_REG_TYPE_GET(addr) == SHR_PMAC_REGTYPE_MEM) {
        return bcmbd_cmicx_mem_read(unit, cmicx_addr.adext,
                                    cmicx_addr.addr, data,
                                    CMICX_BYTES2WORDS(size));
    }

    return bcmbd_cmicx_reg_read(unit, cmicx_addr.adext,
                                cmicx_addr.addr, data,
                                CMICX_BYTES2WORDS(size));
}

int
bcmbd_cmicx_pmac_write(int unit, int blk_id, int port, uint32_t addr,
                       uint32_t idx, size_t size, uint32_t *data)
{
    cmicx_addr_t cmicx_addr;

    cmicx_addr_get(unit, blk_id, port, addr, idx, &cmicx_addr);

    if (SHR_PMAC_REG_TYPE_GET(addr) == SHR_PMAC_REGTYPE_MEM) {
        return bcmbd_cmicx_mem_write(unit, cmicx_addr.adext,
                                     cmicx_addr.addr, data,
                                     CMICX_BYTES2WORDS(size));
    }

    return bcmbd_cmicx_reg_write(unit, cmicx_addr.adext,
                                 cmicx_addr.addr, data,
                                 CMICX_BYTES2WORDS(size));
}
