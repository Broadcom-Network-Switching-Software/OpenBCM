/*! \file bcmbd_cmicd_pmac.c
 *
 * PMAC register access driver for CMICD.
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
#include <bcmbd/bcmbd_cmicd_pmac.h>
#include <bcmbd/bcmbd_cmicd_reg.h>
#include <bcmbd/bcmbd_cmicd_mem.h>
#include <bcmbd/bcmbd_cmicd_pmac.h>


/* Convert bytes to words */
#define CMICD_BYTES2WORDS(_bytes_)  ((_bytes_ + 3) / 4)

/* Set RT bit of CMICD register address */
#define CMICD_ADDR_RT_SET(_addr_)    _addr_ |= 0x2000000

typedef struct cmicd_addr_s {
    uint32_t addr;
    uint32_t adext;
} cmicd_addr_t;

/* Convert PMAC address to CMICD address */
static void
cmicd_addr_get(int unit, int blk_id, uint32_t lane,
               uint32_t pmac_addr, uint32_t idx, cmicd_addr_t *cmicd_addr)
{
    int acc_type;
    uint32_t reg_type, reg_addr;

    acc_type = SHR_PMAC_ACC_TYPE_GET(pmac_addr);
    reg_type = SHR_PMAC_REG_TYPE_GET(pmac_addr);
    reg_addr = SHR_PMAC_REG_ADDR_GET(pmac_addr);

    /* CMICD address extension */
    cmicd_addr->adext = 0;
    BCMBD_CMICD_ADEXT_ACCTYPE_SET(cmicd_addr->adext, acc_type);
    BCMBD_CMICD_ADEXT_BLOCK_SET(cmicd_addr->adext, blk_id);

    /* CMICD address */
    if (reg_type == SHR_PMAC_REGTYPE_MEM) {
        cmicd_addr->addr = bcmbd_block_port_addr(unit,
                                                 blk_id, -1,
                                                 reg_addr, idx);
    } else if (reg_type == SHR_PMAC_REGTYPE_BLK) {
        cmicd_addr->addr = bcmbd_block_port_addr(unit,
                                                 blk_id, 0,
                                                 reg_addr, idx);

        /* Set RT bit for block register */
        CMICD_ADDR_RT_SET(cmicd_addr->addr);
    } else {
        cmicd_addr->addr = bcmbd_block_port_addr(unit,
                                                 blk_id, lane,
                                                 reg_addr, idx);
    }
}

int
bcmbd_cmicd_pmac_read(int unit, int blk_id, int port, uint32_t addr,
                      uint32_t idx, size_t size, uint32_t *data)
{
    cmicd_addr_t cmicd_addr;

    cmicd_addr_get(unit, blk_id, port, addr, idx, &cmicd_addr);

    if (SHR_PMAC_REG_TYPE_GET(addr) == SHR_PMAC_REGTYPE_MEM) {
        return bcmbd_cmicd_mem_read(unit, cmicd_addr.adext,
                                    cmicd_addr.addr, data,
                                    CMICD_BYTES2WORDS(size));
    }

    return bcmbd_cmicd_reg_read(unit, cmicd_addr.adext,
                                cmicd_addr.addr, data,
                                CMICD_BYTES2WORDS(size));
}

int
bcmbd_cmicd_pmac_write(int unit, int blk_id, int port, uint32_t addr,
                       uint32_t idx, size_t size, uint32_t *data)
{
    cmicd_addr_t cmicd_addr;

    cmicd_addr_get(unit, blk_id, port, addr, idx, &cmicd_addr);

    if (SHR_PMAC_REG_TYPE_GET(addr) == SHR_PMAC_REGTYPE_MEM) {
        return bcmbd_cmicd_mem_write(unit, cmicd_addr.adext,
                                     cmicd_addr.addr, data,
                                     CMICD_BYTES2WORDS(size));
    }

    return bcmbd_cmicd_reg_write(unit, cmicd_addr.adext,
                                 cmicd_addr.addr, data,
                                 CMICD_BYTES2WORDS(size));
}
