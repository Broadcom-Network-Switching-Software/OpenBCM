/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * XGS internal PHY read function with AER support.
 *
 * This function accepts a 32-bit PHY register address and will
 * properly configure clause 45 DEVAD and XAUI lane access.
 * Please see phymod_reg.h for additional information.
 */

#include <phymod/acc/phymod_tsc_iblk.h>
#include <phymod/phymod_debug.h>

/*
 * Some bus drivers can handle both clause 22 and clause 45 access, so
 * we use this bit to request clause 45 access.
 */
#define FORCE_CL45      0x20
#define LANE_IS_MEMBER(_lnmap, _ln) (((_lnmap) & (0x1 << (_ln))) ? 1 : 0)
#define LANE_MAX 16 /* QSGMII has multi-core, support more than 4 lanes */

/* TSCBH has 8 lanes, lower 4 lanes belong to MPP0 and upper 4 lanes belong to MPP1 */
#define TSCBH_LANE_IS_MPP1(_ln) (_ln > 3 ? 1: 0)

/* TSCO each PML  has 4 lanes, lower 2 lanes belong to MPP0 and upper 2 lanes belong to MPP1 */
#define TSCO_LANE_IS_MPP1(_ln) (_ln > 1 ? 1: 0)


int
phymod_tsc_iblk_read(const phymod_access_t *pa, uint32_t addr, uint32_t *data)
{
    int ioerr = 0;
    uint32_t devad = (addr >> 16) & 0xf;
    uint32_t blkaddr, regaddr;
    uint32_t lane_map, lane;
    uint32_t aer, add;
    phymod_bus_t* bus;
    uint8_t pll_index = 0;

    add = addr ;

    /* Work around for ARM compiler error [-Werror=unused-but-set-variable] */
    if ( add == 0 ) {
        add = addr;
    }

    if (pa == NULL) {
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd add=%x pa=null\n", add));
        return -1;
    }

    bus = PHYMOD_ACC_BUS(pa);
    pll_index = PHYMOD_ACC_PLLIDX(pa) & 0x3;

    /* Do not attempt to read write-only registers */
    if (addr & PHYMOD_REG_ACC_TSC_IBLK_WR_ONLY) {
        *data = 0;
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd add=%x WO=1\n", add));
        return 0;
    }

    /* Determine which lane to read from */
    lane = 0;
    if (addr & PHYMOD_REG_ACC_AER_IBLK_FORCE_LANE) {
        /* Forcing lane overrides default behavior */
        lane = (addr >> PHYMOD_REG_ACCESS_FLAGS_SHIFT) & 0x7;
    } else {
        /* Use first lane in lane map by default */
        lane_map = PHYMOD_ACC_LANE_MASK(pa);
        if (lane_map & 0x1) {
            lane = 0;
        } else if (lane_map & 0x2) {
            lane = 1;
        } else if (lane_map & 0x4) {
            lane = 2;
        } else if (lane_map & 0x8) {
            lane = 3;
        } else if (lane_map & 0xfff0) {
            lane = -1;
            while (lane_map) {
                lane++;
                lane_map >>= 1;
            }
        }
    }

    /* Use default DEVAD if none specified */
    if(((pa->devad & PHYMOD_ACC_DEVAD_0_OVERRIDE_MASK) != 0) && (devad == 0)) {
        devad = (pa->devad & PHYMOD_ACC_DEVAD_MASK);
    /* Force DEVAD if want special address */
    } else if(pa->devad & PHYMOD_ACC_DEVAD_FORCE_MASK) {
        devad = (pa->devad & PHYMOD_ACC_DEVAD_MASK);
    }

    /* Encode address extension */
    aer = lane | (devad << 11) | (pll_index << 8);

    /* Mask raw register value */
    addr &= 0xffff;

    /* If bus driver supports lane control, then we are done */
    if (PHYMOD_BUS_CAP_LANE_CTRL_GET(bus)) {
        ioerr += PHYMOD_BUS_READ(pa, addr | (aer << 16), data);
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd sbus add=%x aer=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                                add, aer, addr, pa->lane_mask, ioerr, *data));
        return ioerr;
    }

    /* Use clause 45 access if supported */
    if (PHYMOD_ACC_F_CLAUSE45_GET(pa)) {
        devad |= FORCE_CL45;
        ioerr += PHYMOD_BUS_WRITE(pa, 0xffde | (devad << 16), aer);
        ioerr += PHYMOD_BUS_READ(pa, addr | (devad << 16), data);
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd cl45 add=%x dev=%x aer=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                                add, devad, aer, addr, pa->lane_mask, ioerr, *data));
        return ioerr;
    }

    /* Write address extension register */
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1f, 0xffd0);
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1e, aer);

    /* Select block */
    blkaddr = addr & 0xfff0;
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1f, blkaddr);

    /* Read register value */
    regaddr = addr & 0xf;
    if (addr & 0x8000) {
        regaddr |= 0x10;
    }
    ioerr += PHYMOD_BUS_READ(pa, regaddr, data);
    PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd cl22 add=%x aer=%x blk=%x adr=%x reg=%x lm=%0d rtn=%0d d=%x\n",
                            add, aer, blkaddr, addr, regaddr, pa->lane_mask, ioerr, *data));
    return ioerr;
}



int
phymod_tscbh_iblk_read(const phymod_access_t *pa, uint32_t addr, uint32_t *data)
{
    int ioerr = 0;
    uint32_t devad = (addr >> 16) & 0xf;
    uint32_t blkaddr, regaddr;
    uint32_t lane_map, lane;
    uint32_t aer, add;
    phymod_bus_t* bus;
    uint8_t pll_index = 0, mpp_sel = 0;

    add = addr ;

    /* Work around for ARM compiler error [-Werror=unused-but-set-variable] */
    if ( add == 0 ) {
        add = addr;
    }

    if (pa == NULL) {
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd add=%x pa=null\n", add));
        return -1;
    }

    bus = PHYMOD_ACC_BUS(pa);
    pll_index = PHYMOD_ACC_PLLIDX(pa) & 0x3;

    /* Do not attempt to read write-only registers */
    if (addr & PHYMOD_REG_ACC_TSC_IBLK_WR_ONLY) {
        *data = 0;
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd add=%x WO=1\n", add));
        return 0;
    }

    /* Determine which lane to read from */
    lane = 0;
    if (addr & PHYMOD_REG_ACC_AER_IBLK_FORCE_LANE) {
        /* Forcing lane overrides default behavior */
        lane = (addr >> PHYMOD_REG_ACCESS_FLAGS_SHIFT) & 0x7;
    } else {
        /* Use first lane in lane map by default */
        lane_map = PHYMOD_ACC_LANE_MASK(pa);
        if (lane_map & 0x1) {
            lane = 0;
        } else if (lane_map & 0x2) {
            lane = 1;
        } else if (lane_map & 0x4) {
            lane = 2;
        } else if (lane_map & 0x8) {
            lane = 3;
        } else if (lane_map & 0x10) {
            lane = 4;
        } else if (lane_map & 0x20) {
            lane = 5;
        } else if (lane_map & 0x40) {
            lane = 6;
        } else if (lane_map & 0x80) {
            lane = 7;
        }
    }

    /* Use default DEVAD if none specified */
    if(((pa->devad & PHYMOD_ACC_DEVAD_0_OVERRIDE_MASK) != 0) && (devad == 0)) {
        devad = (pa->devad & PHYMOD_ACC_DEVAD_MASK);
    /* Force DEVAD if want special address */
    } else if(pa->devad & PHYMOD_ACC_DEVAD_FORCE_MASK) {
        devad = (pa->devad & PHYMOD_ACC_DEVAD_MASK);
    }

    /* Encode address extension based on if pcs or PMD */
    if (devad) {
    /* PMD device read */
        aer = lane | (devad << 11) | (pll_index << 8);
    } else {
    /* PCS device */
        if (TSCBH_LANE_IS_MPP1(lane)) {
            mpp_sel = PHYMOD_TSCBH_IBLK_MPP_1;
        } else {
            mpp_sel = PHYMOD_TSCBH_IBLK_MPP_0;
        }
        aer = (lane % 4) | (mpp_sel << PHYMOD_TSCBH_IBLK_MPP_SHIFT);
    }

    /* Mask raw register value */
    addr &= 0xffff;

    /* If bus driver supports lane control, then we are done */
    if (PHYMOD_BUS_CAP_LANE_CTRL_GET(bus)) {
        ioerr += PHYMOD_BUS_READ(pa, addr | (aer << 16), data);
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd sbus add=%x aer=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                                add, aer, addr, pa->lane_mask, ioerr, *data));
        return ioerr;
    }

    /* Use clause 45 access if supported */
    if (PHYMOD_ACC_F_CLAUSE45_GET(pa)) {
        devad |= FORCE_CL45;
        ioerr += PHYMOD_BUS_WRITE(pa, 0xffde | (devad << 16), aer);
        ioerr += PHYMOD_BUS_READ(pa, addr | (devad << 16), data);
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd cl45 add=%x dev=%x aer=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                                add, devad, aer, addr, pa->lane_mask, ioerr, *data));
        return ioerr;
    }

    /* Write address extension register */
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1f, 0xffd0);
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1e, aer);

    /* Select block */
    blkaddr = addr & 0xfff0;
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1f, blkaddr);

    /* Read register value */
    regaddr = addr & 0xf;
    if (addr & 0x8000) {
        regaddr |= 0x10;
    }
    ioerr += PHYMOD_BUS_READ(pa, regaddr, data);
    PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd cl22 add=%x aer=%x blk=%x adr=%x reg=%x lm=%0d rtn=%0d d=%x\n",
                            add, aer, blkaddr, addr, regaddr, pa->lane_mask, ioerr, *data));
    return ioerr;
}

/* this function should be only be invoked to access 1 copy per MPP PCS register
 * any PMD register access should be treated as error */
int
phymod_tsco1_iblk_read(const phymod_access_t *pa, uint32_t addr, uint32_t *data)
{
    int ioerr = 0;
    uint32_t devad = (addr >> 16) & 0xf;
    uint32_t blkaddr, regaddr;
    uint32_t lane_map, lane;
    uint32_t aer, add;
    phymod_bus_t* bus;
    uint8_t mpp_sel = 0;

    add = addr ;

    /* Work around for ARM compiler error [-Werror=unused-but-set-variable] */
    if ( add == 0 ) {
        add = addr;
    }

    if (pa == NULL) {
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd add=%x pa=null\n", add));
        return -1;
    }

    bus = PHYMOD_ACC_BUS(pa);

    /* Do not attempt to read write-only registers */
    if (addr & PHYMOD_REG_ACC_TSC_IBLK_WR_ONLY) {
        *data = 0;
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd add=%x WO=1\n", add));
        return 0;
    }

    /* Determine which lane to read from */
    lane = 0;
    if (addr & PHYMOD_REG_ACC_AER_IBLK_FORCE_LANE) {
        /* Forcing lane overrides default behavior */
        lane = (addr >> PHYMOD_REG_ACCESS_FLAGS_SHIFT) & 0x7;
    } else {
        /* Use first lane in lane map by default */
        lane_map = PHYMOD_ACC_LANE_MASK(pa);
        if (lane_map & 0x1) {
            lane = 0;
        } else if (lane_map & 0x2) {
            lane = 1;
        } else if (lane_map & 0x4) {
            lane = 2;
        } else if (lane_map & 0x8) {
            lane = 3;
        } else if (lane_map & 0x10) {
            lane = 0;
        } else if (lane_map & 0x20) {
            lane = 1;
        } else if (lane_map & 0x40) {
            lane = 2;
        } else if (lane_map & 0x80) {
            lane = 3;
        }
    }

    /* Use default DEVAD if none specified */
    if(((pa->devad & PHYMOD_ACC_DEVAD_0_OVERRIDE_MASK) != 0) && (devad == 0)) {
        devad = (pa->devad & PHYMOD_ACC_DEVAD_MASK);
    /* Force DEVAD if want special address */
    } else if(pa->devad & PHYMOD_ACC_DEVAD_FORCE_MASK) {
        devad = (pa->devad & PHYMOD_ACC_DEVAD_MASK);
    }

    /* Encode address extension based on if pcs or PMD */
    if (devad) {
        /*  invalid PMD device read */
        PHYMOD_VDBG(DBG_ACC,pa,("invalid PMD register access add=%x \n", addr));
        return -1;
    } else {
    /* PCS device */
        if (TSCO_LANE_IS_MPP1(lane)) {
            mpp_sel = PHYMOD_TSCBH_IBLK_MPP_1;
        } else {
            mpp_sel = PHYMOD_TSCBH_IBLK_MPP_0;
        }
        /* since this is one copy register, so set the lane to 0 always */
        lane = 0;
        aer = (lane % 4) | (mpp_sel << PHYMOD_TSCBH_IBLK_MPP_SHIFT);
    }

    /* Mask raw register value */
    addr &= 0xffff;

    /* If bus driver supports lane control, then we are done */
    if (PHYMOD_BUS_CAP_LANE_CTRL_GET(bus)) {
        ioerr += PHYMOD_BUS_READ(pa, addr | (aer << 16), data);
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd sbus add=%x aer=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                                add, aer, addr, pa->lane_mask, ioerr, *data));
        return ioerr;
    }

    /* Use clause 45 access if supported */
    if (PHYMOD_ACC_F_CLAUSE45_GET(pa)) {
        devad |= FORCE_CL45;
        ioerr += PHYMOD_BUS_WRITE(pa, 0xffde | (devad << 16), aer);
        ioerr += PHYMOD_BUS_READ(pa, addr | (devad << 16), data);
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd cl45 add=%x dev=%x aer=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                                add, devad, aer, addr, pa->lane_mask, ioerr, *data));
        return ioerr;
    }

    /* Write address extension register */
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1f, 0xffd0);
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1e, aer);

    /* Select block */
    blkaddr = addr & 0xfff0;
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1f, blkaddr);

    /* Read register value */
    regaddr = addr & 0xf;
    if (addr & 0x8000) {
        regaddr |= 0x10;
    }
    ioerr += PHYMOD_BUS_READ(pa, regaddr, data);
    PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd cl22 add=%x aer=%x blk=%x adr=%x reg=%x lm=%0d rtn=%0d d=%x\n",
                            add, aer, blkaddr, addr, regaddr, pa->lane_mask, ioerr, *data));
    return ioerr;
}

/* this function should be only be invoked to access 2 copy per MPP PCS register
 * any PMD register access should be treated as error */
int
phymod_tsco2_iblk_read(const phymod_access_t *pa, uint32_t addr, uint32_t *data)
{
    int ioerr = 0;
    uint32_t devad = (addr >> 16) & 0xf;
    uint32_t blkaddr, regaddr;
    uint32_t lane_map, lane;
    uint32_t aer, add;
    phymod_bus_t* bus;
    uint8_t mpp_sel = 0;

    add = addr ;

    /* Work around for ARM compiler error [-Werror=unused-but-set-variable] */
    if ( add == 0 ) {
        add = addr;
    }

    if (pa == NULL) {
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd add=%x pa=null\n", add));
        return -1;
    }

    bus = PHYMOD_ACC_BUS(pa);

    /* Do not attempt to read write-only registers */
    if (addr & PHYMOD_REG_ACC_TSC_IBLK_WR_ONLY) {
        *data = 0;
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd add=%x WO=1\n", add));
        return 0;
    }

    /* Determine which lane to read from */
    lane = 0;
    if (addr & PHYMOD_REG_ACC_AER_IBLK_FORCE_LANE) {
        /* Forcing lane overrides default behavior */
        lane = (addr >> PHYMOD_REG_ACCESS_FLAGS_SHIFT) & 0x7;
    } else {
        /* Use first lane in lane map by default */
        lane_map = PHYMOD_ACC_LANE_MASK(pa);
        if (lane_map & 0x1) {
            lane = 0;
        } else if (lane_map & 0x2) {
            lane = 1;
        } else if (lane_map & 0x4) {
            lane = 2;
        } else if (lane_map & 0x8) {
            lane = 3;
        } else if (lane_map & 0x10) {
            lane = 0;
        } else if (lane_map & 0x20) {
            lane = 1;
        } else if (lane_map & 0x40) {
            lane = 2;
        } else if (lane_map & 0x80) {
            lane = 3;
        }
    }

    /* Use default DEVAD if none specified */
    if(((pa->devad & PHYMOD_ACC_DEVAD_0_OVERRIDE_MASK) != 0) && (devad == 0)) {
        devad = (pa->devad & PHYMOD_ACC_DEVAD_MASK);
    /* Force DEVAD if want special address */
    } else if(pa->devad & PHYMOD_ACC_DEVAD_FORCE_MASK) {
        devad = (pa->devad & PHYMOD_ACC_DEVAD_MASK);
    }

    /* Encode address extension based on if pcs or PMD */
    if (devad) {
        /*  invalid PMD device read */
        PHYMOD_VDBG(DBG_ACC,pa,("invalid PMD register access add=%x \n", addr));
        return -1;
    } else {
    /* PCS device */
        if (TSCO_LANE_IS_MPP1(lane)) {
            mpp_sel = PHYMOD_TSCBH_IBLK_MPP_1;
        } else {
            mpp_sel = PHYMOD_TSCBH_IBLK_MPP_0;
        }
        /* since this is 2  copy register, so need to mod 2 */
        aer = (lane % 2) | (mpp_sel << PHYMOD_TSCBH_IBLK_MPP_SHIFT);
    }

    /* Mask raw register value */
    addr &= 0xffff;

    /* If bus driver supports lane control, then we are done */
    if (PHYMOD_BUS_CAP_LANE_CTRL_GET(bus)) {
        ioerr += PHYMOD_BUS_READ(pa, addr | (aer << 16), data);
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd sbus add=%x aer=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                                add, aer, addr, pa->lane_mask, ioerr, *data));
        return ioerr;
    }

    /* Use clause 45 access if supported */
    if (PHYMOD_ACC_F_CLAUSE45_GET(pa)) {
        devad |= FORCE_CL45;
        ioerr += PHYMOD_BUS_WRITE(pa, 0xffde | (devad << 16), aer);
        ioerr += PHYMOD_BUS_READ(pa, addr | (devad << 16), data);
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd cl45 add=%x dev=%x aer=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                                add, devad, aer, addr, pa->lane_mask, ioerr, *data));
        return ioerr;
    }

    /* Write address extension register */
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1f, 0xffd0);
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1e, aer);

    /* Select block */
    blkaddr = addr & 0xfff0;
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1f, blkaddr);

    /* Read register value */
    regaddr = addr & 0xf;
    if (addr & 0x8000) {
        regaddr |= 0x10;
    }
    ioerr += PHYMOD_BUS_READ(pa, regaddr, data);
    PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd cl22 add=%x aer=%x blk=%x adr=%x reg=%x lm=%0d rtn=%0d d=%x\n",
                            add, aer, blkaddr, addr, regaddr, pa->lane_mask, ioerr, *data));
    return ioerr;
}

/* this function should be only be invoked to access 4 copy per MPP PCS register
 * any PMD register access should be treated as error */
int
phymod_tsco4_iblk_read(const phymod_access_t *pa, uint32_t addr, uint32_t *data)
{
    int ioerr = 0;
    uint32_t devad = (addr >> 16) & 0xf;
    uint32_t blkaddr, regaddr;
    uint32_t lane_map, lane;
    uint32_t aer, add;
    phymod_bus_t* bus;
    uint8_t mpp_sel = 0, is_8_lane_port = 0;

    add = addr ;

    /* Work around for ARM compiler error [-Werror=unused-but-set-variable] */
    if ( add == 0 ) {
        add = addr;
    }

    if (pa == NULL) {
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd add=%x pa=null\n", add));
        return -1;
    }

    bus = PHYMOD_ACC_BUS(pa);

    /* Do not attempt to read write-only registers */
    if (addr & PHYMOD_REG_ACC_TSC_IBLK_WR_ONLY) {
        *data = 0;
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd add=%x WO=1\n", add));
        return 0;
    }

    /* Determine which lane to read from */
    lane = 0;
    if (addr & PHYMOD_REG_ACC_AER_IBLK_FORCE_LANE) {
        /* Forcing lane overrides default behavior */
        lane = (addr >> PHYMOD_REG_ACCESS_FLAGS_SHIFT) & 0x7;
    } else {
        /* Use first lane in lane map by default */
        /* for 4 copy register access, need to check if 8 lane port */
        lane_map = PHYMOD_ACC_LANE_MASK(pa);
        is_8_lane_port = (lane_map >> PHYMOD_TSCO_8_LANE_PORT_SHIFT) & 0x1;
        if (lane_map & 0x1) {
            lane = 0;
        } else if (lane_map & 0x2) {
            lane = 1;
        } else if (lane_map & 0x4) {
            lane = 2;
        } else if (lane_map & 0x8) {
            lane = 3;
        } else if (lane_map & 0x10) {
            /* check if it's 8 lane port */
            if (is_8_lane_port) {
                lane = 4;
            } else {
                lane = 0;
            }
        } else if (lane_map & 0x20) {
            /* check if it's 8 lane port */
            if (is_8_lane_port) {
                lane = 5;
            } else {
                lane = 1;
            }
        } else if (lane_map & 0x40) {
            if (is_8_lane_port) {
                lane = 6;
            } else {
                lane = 2;
            }
        } else if (lane_map & 0x80) {
            if (is_8_lane_port) {
                lane = 7;
            } else {
                lane = 3;
            }
        }
    }

    /* Use default DEVAD if none specified */
    if(((pa->devad & PHYMOD_ACC_DEVAD_0_OVERRIDE_MASK) != 0) && (devad == 0)) {
        devad = (pa->devad & PHYMOD_ACC_DEVAD_MASK);
    /* Force DEVAD if want special address */
    } else if(pa->devad & PHYMOD_ACC_DEVAD_FORCE_MASK) {
        devad = (pa->devad & PHYMOD_ACC_DEVAD_MASK);
    }

    /* Encode address extension based on if pcs or PMD */
    if (devad) {
        /*  invalid PMD device read */
        PHYMOD_VDBG(DBG_ACC,pa,("invalid PMD register access add=%x \n", addr));
        return -1;
    } else {
    /* PCS device */
        /* below need to use tscbh MPP definition */
        if (TSCBH_LANE_IS_MPP1(lane)) {
            mpp_sel = PHYMOD_TSCBH_IBLK_MPP_1;
        } else {
            mpp_sel = PHYMOD_TSCBH_IBLK_MPP_0;
        }
        /* since this is 4 copy register, so need to do mod 4 */
        aer = (lane % 4) | (mpp_sel << PHYMOD_TSCBH_IBLK_MPP_SHIFT);
    }

    /* Mask raw register value */
    addr &= 0xffff;

    /* If bus driver supports lane control, then we are done */
    if (PHYMOD_BUS_CAP_LANE_CTRL_GET(bus)) {
        ioerr += PHYMOD_BUS_READ(pa, addr | (aer << 16), data);
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd sbus add=%x aer=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                                add, aer, addr, pa->lane_mask, ioerr, *data));
        return ioerr;
    }

    /* Use clause 45 access if supported */
    if (PHYMOD_ACC_F_CLAUSE45_GET(pa)) {
        devad |= FORCE_CL45;
        ioerr += PHYMOD_BUS_WRITE(pa, 0xffde | (devad << 16), aer);
        ioerr += PHYMOD_BUS_READ(pa, addr | (devad << 16), data);
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd cl45 add=%x dev=%x aer=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                                add, devad, aer, addr, pa->lane_mask, ioerr, *data));
        return ioerr;
    }

    /* Write address extension register */
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1f, 0xffd0);
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1e, aer);

    /* Select block */
    blkaddr = addr & 0xfff0;
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1f, blkaddr);

    /* Read register value */
    regaddr = addr & 0xf;
    if (addr & 0x8000) {
        regaddr |= 0x10;
    }
    ioerr += PHYMOD_BUS_READ(pa, regaddr, data);
    PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd cl22 add=%x aer=%x blk=%x adr=%x reg=%x lm=%0d rtn=%0d d=%x\n",
                            add, aer, blkaddr, addr, regaddr, pa->lane_mask, ioerr, *data));
    return ioerr;
}


static int
_tsc_iblk_write_lane(
    const phymod_access_t *pa,
    uint32_t lane,
    uint32_t addr,
    uint32_t data)
{
    int ioerr = 0;
    uint32_t devad = (addr >> 16) & 0xf;
    uint32_t blkaddr, regaddr;
    uint32_t aer, add;
    uint32_t wr_mask, rdata;
    phymod_bus_t* bus;
    uint32_t is_write_disabled;
    uint8_t pll_index = 0;

    add = addr ;

    /* Work around for ARM compiler error [-Werror=unused-but-set-variable] */
    if ( add == 0 ) {
        add = addr;
    }

    if (pa == NULL) {
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_wr add=%x pa=null\n", addr));
        return -1;
    }

    bus = PHYMOD_ACC_BUS(pa);
    pll_index = PHYMOD_ACC_PLLIDX(pa) & 0x3;

    if (PHYMOD_IS_WRITE_DISABLED(pa, &is_write_disabled) ==  0) {
        if (is_write_disabled) {
            return ioerr;
        }
    }

    /* Use default DEVAD if none specified */
    if(((pa->devad & PHYMOD_ACC_DEVAD_0_OVERRIDE_MASK) != 0) && (devad == 0)) {
        devad = (pa->devad & PHYMOD_ACC_DEVAD_MASK);
    /* Force DEVAD if want special address */
    } else if(pa->devad & PHYMOD_ACC_DEVAD_FORCE_MASK) {
        devad = (pa->devad & PHYMOD_ACC_DEVAD_MASK);
    }

    /* Check if write mask is specified */
    wr_mask = (data >> 16);
    if (wr_mask) {
        /* Read register if bus driver does not support write mask */
        if (PHYMOD_BUS_CAP_WR_MODIFY_GET(bus) == 0) {
            ioerr += phymod_tsc_iblk_read(pa, addr, &rdata);
            data = (rdata & ~wr_mask) | (data & wr_mask);
            data &= 0xffff;
        }
    }

    /* Encode address extension */
    aer = lane | (devad << 11) | (pll_index << 8);

    /* Mask raw register value */
    addr &= 0xffff;

    /* If bus driver supports lane control, then we are done */
    if (PHYMOD_BUS_CAP_LANE_CTRL_GET(bus)) {
        ioerr += PHYMOD_BUS_WRITE(pa, addr | (aer << 16), data);
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_wr sbus add=%x aer=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                                add, aer, addr, pa->lane_mask, ioerr, data));
        return ioerr;
    }

    /* Use clause 45 access if supported */
    if (PHYMOD_ACC_F_CLAUSE45_GET(pa)) {
        addr &= 0xffff;
        devad |= FORCE_CL45;
        ioerr += PHYMOD_BUS_WRITE(pa, 0xffde | (devad << 16), aer);
        ioerr += PHYMOD_BUS_WRITE(pa, addr | (devad << 16), data);
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_wr cl45 add=%x dev=%x aer=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                                add, devad, aer, addr, pa->lane_mask, ioerr, data));
        return ioerr;
    }

    /* Write address extension register */
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1f, 0xffd0);
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1e, aer);

    /* Select block */
    blkaddr = addr & 0xfff0;
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1f, blkaddr);

    /* Write register value */
    regaddr = addr & 0xf;
    if (addr & 0x8000) {
        regaddr |= 0x10;
    }
    ioerr += PHYMOD_BUS_WRITE(pa, regaddr, data);
    PHYMOD_VDBG(DBG_ACC,pa,("iblk_wr cl22 add=%x aer=%x blk=%x reg=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                            addr, aer, blkaddr, regaddr, addr, pa->lane_mask, ioerr, data));
    return ioerr;
}

static int
_tscbh_iblk_write_lane(
    const phymod_access_t *pa,
    uint32_t lane,
    uint32_t addr,
    uint32_t data)
{
    int ioerr = 0;
    uint32_t devad = (addr >> 16) & 0xf;
    uint32_t blkaddr, regaddr;
    uint32_t aer, add;
    uint32_t wr_mask, rdata;
    phymod_bus_t* bus;
    uint32_t is_write_disabled;
    uint8_t pll_index = 0;

    add = addr ;

    /* Work around for ARM compiler error [-Werror=unused-but-set-variable] */
    if ( add == 0 ) {
        add = addr;
    }

    if (pa == NULL) {
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_wr add=%x pa=null\n", addr));
        return -1;
    }

    bus = PHYMOD_ACC_BUS(pa);
    pll_index = PHYMOD_ACC_PLLIDX(pa) & 0x3;

    if (PHYMOD_IS_WRITE_DISABLED(pa, &is_write_disabled) ==  0) {
        if (is_write_disabled) {
            return ioerr;
        }
    }

    /* Use default DEVAD if none specified */
    if(((pa->devad & PHYMOD_ACC_DEVAD_0_OVERRIDE_MASK) != 0) && (devad == 0)) {
        devad = (pa->devad & PHYMOD_ACC_DEVAD_MASK);
    /* Force DEVAD if want special address */
    } else if(pa->devad & PHYMOD_ACC_DEVAD_FORCE_MASK) {
        devad = (pa->devad & PHYMOD_ACC_DEVAD_MASK);
    }

    /* Check if write mask is specified */
    wr_mask = (data >> 16);
    if (wr_mask) {
        /* Read register if bus driver does not support write mask */
        if (PHYMOD_BUS_CAP_WR_MODIFY_GET(bus) == 0) {
            ioerr += phymod_tscbh_iblk_read(pa, addr, &rdata);
            data = (rdata & ~wr_mask) | (data & wr_mask);
            data &= 0xffff;
        }
    }

    /* Encode address extension based on if pcs or PMD */
    if (devad) { /* PMD */
        aer = lane | (devad << 11) | (pll_index << 8);
    } else {
        aer = lane | (devad << 11);
    }

    /* Mask raw register value */
    addr &= 0xffff;

    /* If bus driver supports lane control, then we are done */
    if (PHYMOD_BUS_CAP_LANE_CTRL_GET(bus)) {
        ioerr += PHYMOD_BUS_WRITE(pa, addr | (aer << 16), data);
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_wr sbus add=%x aer=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                                add, aer, addr, pa->lane_mask, ioerr, data));
        return ioerr;
    }

    /* Use clause 45 access if supported */
    if (PHYMOD_ACC_F_CLAUSE45_GET(pa)) {
        addr &= 0xffff;
        devad |= FORCE_CL45;
        ioerr += PHYMOD_BUS_WRITE(pa, 0xffde | (devad << 16), aer);
        ioerr += PHYMOD_BUS_WRITE(pa, addr | (devad << 16), data);
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_wr cl45 add=%x dev=%x aer=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                                add, devad, aer, addr, pa->lane_mask, ioerr, data));
        return ioerr;
    }

    /* Write address extension register */
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1f, 0xffd0);
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1e, aer);

    /* Select block */
    blkaddr = addr & 0xfff0;
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1f, blkaddr);

    /* Write register value */
    regaddr = addr & 0xf;
    if (addr & 0x8000) {
        regaddr |= 0x10;
    }
    ioerr += PHYMOD_BUS_WRITE(pa, regaddr, data);
    PHYMOD_VDBG(DBG_ACC,pa,("iblk_wr cl22 add=%x aer=%x blk=%x reg=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                            addr, aer, blkaddr, regaddr, addr, pa->lane_mask, ioerr, data));
    return ioerr;
}


int
phymod_tsc_iblk_write(const phymod_access_t *pa, uint32_t addr, uint32_t data)
{
    int ioerr = 0;
    uint32_t lane_map, hwlane, is_hwsupport = 1;

    hwlane = 0;
    if (addr & PHYMOD_REG_ACC_AER_IBLK_FORCE_LANE) {
        /* Forcing lane overrides default behavior */
        hwlane = (addr >> PHYMOD_REG_ACCESS_FLAGS_SHIFT) & 0x7;
        return _tsc_iblk_write_lane(pa, hwlane, addr, data);
    }
    lane_map = PHYMOD_ACC_LANE_MASK(pa) & 0xffff;
    switch (lane_map) {
        case 0x0:
            hwlane = 0x0;
            break;
        case 0x1:
            hwlane = 0x0;
            break;
        case 0x2:
            hwlane = 0x1;
            break;
        case 0x4:
            hwlane = 0x2;
            break;
        case 0x8:
            hwlane = 0x3;
            break;
        case 0xf:
            hwlane = PHYMOD_TSC_IBLK_BCAST;
            break;
        case 0x3:
            hwlane = PHYMOD_TSC_IBLK_MCAST01;
            break;
        case 0xc:
            hwlane = PHYMOD_TSC_IBLK_MCAST23;
            break;
        default:
            is_hwsupport = 0;
            hwlane = 0x0;
    }
     /*
     * If the lane mask is supported by HW, i.e. a single lane or a
     * supported multicast combination (0x3, 0xC or 0xF), then program
     * directly by HW.
     */
    if (is_hwsupport) {
        return _tsc_iblk_write_lane(pa, hwlane, addr, data);
    }
     /*
     * If the lane mask is not supported directly by HW, e.g. 0x9, then
     * program one lane at a time.
     */
    for (hwlane = 0; hwlane < LANE_MAX; hwlane++) {
        if (LANE_IS_MEMBER(lane_map, hwlane)) {
            ioerr = _tsc_iblk_write_lane(pa, hwlane, addr, data);
            if (ioerr != 0) {
                return ioerr;
            }
        }
    }
    return ioerr;
}

int
phymod_tscbh_iblk_write(const phymod_access_t *pa, uint32_t addr, uint32_t data)
{
    int ioerr = 0;
    uint32_t devad = (addr >> 16) & 0xf;
    uint32_t lane_map, hwlane = 0, is_hwsupport = 1;
    uint32_t mpp_sel = 0, lane = 0;

    if (addr & PHYMOD_REG_ACC_AER_IBLK_FORCE_LANE) {
        /* Forcing lane overrides default behavior */
        hwlane = (addr >> PHYMOD_REG_ACCESS_FLAGS_SHIFT) & 0x7;
        return _tscbh_iblk_write_lane(pa, hwlane, addr, data);
    }
    lane_map = PHYMOD_ACC_LANE_MASK(pa) & 0xffff;

    /* need to use different lane for pcs or PMD device */
    if (devad) {
    /* for PMD device */
        switch (lane_map) {
            case 0x0:
                hwlane = 0x0;
                break;
            case 0x1:
                hwlane = 0x0;
                break;
            case 0x2:
                hwlane = 0x1;
                break;
            case 0x4:
                hwlane = 0x2;
                break;
            case 0x8:
                hwlane = 0x3;
                break;
            case 0x10:
                hwlane = 0x4;
                break;
            case 0x20:
                hwlane = 0x5;
                break;
            case 0x40:
                hwlane = 0x6;
                break;
            case 0x80:
                hwlane = 0x7;
                break;
            case 0xf:
                hwlane = PHYMOD_BH_IBLK_MCAST0123;
                break;
            case 0xf0:
                hwlane = PHYMOD_BH_IBLK_MCAST4567;
                break;
            case 0x3:
                hwlane = PHYMOD_BH_IBLK_MCAST01;
                break;
            case 0xc:
                hwlane = PHYMOD_BH_IBLK_MCAST23;
                break;
            case 0x30:
                hwlane = PHYMOD_BH_IBLK_MCAST45;
                break;
            case 0xc0:
                hwlane = PHYMOD_BH_IBLK_MCAST67;
                break;
            case 0xff:
                hwlane = PHYMOD_BH_IBLK_BCAST;
                break;
            default:
                is_hwsupport = 0;
                hwlane = 0x0;
        }
    } else {
        /* pcs device */
        switch (lane_map) {
            case 0x0:
                hwlane = 0x100;
                break;
            case 0x1:
                hwlane = 0x100;
                break;
            case 0x2:
                hwlane = 0x101;
                break;
            case 0x4:
                hwlane = 0x102;
                break;
            case 0x8:
                hwlane = 0x103;
                break;
            case 0x10:
                hwlane = 0x200;
                break;
            case 0x20:
                hwlane = 0x201;
                break;
            case 0x40:
                hwlane = 0x202;
                break;
            case 0x80:
                hwlane = 0x203;
                break;
            case 0xf:
                hwlane = PHYMOD_TSCBH_IBLK_MCAST0123;
                break;
            case 0xff:
                hwlane = PHYMOD_TSCBH_IBLK_BCAST;
                break;
            case 0x3:
                hwlane = PHYMOD_TSCBH_IBLK_MCAST01;
                break;
            case 0xc:
                hwlane = PHYMOD_TSCBH_IBLK_MCAST23;
                break;
            case 0x30:
                hwlane = PHYMOD_TSCBH_IBLK_MCAST45;
                break;
            case 0xc0:
                hwlane = PHYMOD_TSCBH_IBLK_MCAST67;
                break;
            case 0x5:
                hwlane = PHYMOD_TSCBH_IBLK_MCAST02;
                break;
            case 0x50:
                hwlane = PHYMOD_TSCBH_IBLK_MCAST46;
                break;
            default:
                is_hwsupport = 0;
                hwlane = 0x0;
        }
     }
     /*
     * If the lane mask is supported by HW, i.e. a single lane or a
     * supported multicast combination (0x3, 0xC or 0xF), then program
     * directly by HW.
     */
    if (is_hwsupport) {
        return _tscbh_iblk_write_lane(pa, hwlane, addr, data);
    }
     /*
     * If the lane mask is not supported directly by HW, e.g. 0x9, then
     * program one lane at a time.
     */
    for (lane = 0; lane < LANE_MAX; lane++) {
        if (LANE_IS_MEMBER(lane_map, lane)) {
            /* if pcs device, then nbeed to form aer lane */
            if (!devad) {
                if (TSCBH_LANE_IS_MPP1(lane)) {
                    mpp_sel = PHYMOD_TSCBH_IBLK_MPP_1;
                } else {
                    mpp_sel = PHYMOD_TSCBH_IBLK_MPP_0;
                }
                hwlane = (lane % 4) | (mpp_sel << PHYMOD_TSCBH_IBLK_MPP_SHIFT);
            }
            ioerr = _tscbh_iblk_write_lane(pa, hwlane, addr, data);
            if (ioerr != 0) {
                return ioerr;
            }
        }
    }
    return ioerr;
}

/* this function should be only be invoked to access 1 copy per MPP PCS register
 * any PMD register access should be treated as error */
int
phymod_tsco1_iblk_write(const phymod_access_t *pa, uint32_t addr, uint32_t data)
{
    uint32_t devad = (addr >> 16) & 0xf;
    uint32_t lane_map, hwlane = 0, is_hwsupport = 1;

    if (addr & PHYMOD_REG_ACC_AER_IBLK_FORCE_LANE) {
        /* Forcing lane overrides default behavior */
        hwlane = (addr >> PHYMOD_REG_ACCESS_FLAGS_SHIFT) & 0x7;
        return _tscbh_iblk_write_lane(pa, hwlane, addr, data);
    }
    lane_map = PHYMOD_ACC_LANE_MASK(pa) & 0xffff;

    /* need to make sure this function is only for pcs device */
    if (devad) {
        /*  invalid PMD device read */
        PHYMOD_VDBG(DBG_ACC,pa,("invalid PMD register access add=%x \n", addr));
        return -1;
    } else {
        /* pcs device */
        switch (lane_map) {
            case 0x0:
                hwlane = 0x100;
                break;
            case 0x1:
                hwlane = 0x100;
                break;
            case 0x2:
                hwlane = 0x100;
                break;
            case 0x4:
                hwlane = 0x200;
                break;
            case 0x8:
                hwlane = 0x200;
                break;
            case 0x10:
                hwlane = 0x100;
                break;
            case 0x20:
                hwlane = 0x100;
                break;
            case 0x40:
                hwlane = 0x200;
                break;
            case 0x80:
                hwlane = 0x200;
                break;
            case 0xf:
                hwlane = 0x100;
                break;
            case 0xf0:
                hwlane = 0x100;
                break;
            case 0xff:
                hwlane =  0x100;
                break;
            case 0x3:
                hwlane =  0x100;
                break;
            case 0xc:
                hwlane = 0x200;
                break;
            case 0x30:
                hwlane = 0x100;
                break;
            case 0xc0:
                hwlane = 0x200;
                break;
            default:
                is_hwsupport = 0;
                hwlane = 0x0;
        }
     }
     /*
     * If the lane mask is supported by HW, i.e. a single lane or a
     * supported multicast combination (0x3, 0xC or 0xF), then program
     * directly by HW.
     */
    if (is_hwsupport) {
        return _tscbh_iblk_write_lane(pa, hwlane, addr, data);
    } else {
        PHYMOD_VDBG(DBG_ACC,pa,("invalid lane map=%x \n", lane_map));
        return -1;
    }
}

/* this function should be only be invoked to access 2 copy per MPP PCS register
 * any PMD register access should be treated as error */
int
phymod_tsco2_iblk_write(const phymod_access_t *pa, uint32_t addr, uint32_t data)
{
    uint32_t devad = (addr >> 16) & 0xf;
    uint32_t lane_map, hwlane = 0, is_hwsupport = 1;

    if (addr & PHYMOD_REG_ACC_AER_IBLK_FORCE_LANE) {
        /* Forcing lane overrides default behavior */
        hwlane = (addr >> PHYMOD_REG_ACCESS_FLAGS_SHIFT) & 0x7;
        return _tscbh_iblk_write_lane(pa, hwlane, addr, data);
    }
    lane_map = PHYMOD_ACC_LANE_MASK(pa) & 0xffff;

    /* need to make sure this function is only for pcs device */
    if (devad) {
        /*  invalid PMD device read */
        PHYMOD_VDBG(DBG_ACC,pa,("invalid PMD register access add=%x \n", addr));
        return -1;
    } else {
        /* pcs device */
        switch (lane_map) {
            case 0x0:
                hwlane = 0x100;
                break;
            case 0x1:
                hwlane = 0x100;
                break;
            case 0x2:
                hwlane = 0x101;
                break;
            case 0x4:
                hwlane = 0x200;
                break;
            case 0x8:
                hwlane = 0x201;
                break;
            case 0x10:
                hwlane = 0x100;
                break;
            case 0x20:
                hwlane = 0x101;
                break;
            case 0x40:
                hwlane = 0x200;
                break;
            case 0x80:
                hwlane = 0x201;
                break;
            case 0xf:
                hwlane =  0x100;
                break;
            case 0xf0:
                hwlane = 0x100;
                break;
            case 0xff:
                hwlane =  0x100;
                break;
            case 0x3:
                hwlane =  0x100;
                break;
            case 0xc:
                hwlane = 0x200;
                break;
            case 0x30:
                hwlane = 0x100;
                break;
            case 0xc0:
                hwlane = 0x200;
                break;
            default:
                is_hwsupport = 0;
                hwlane = 0x0;
        }
     }
     /*
     * If the lane mask is supported by HW, i.e. a single lane or a
     * supported multicast combination (0x3, 0xC or 0xF), then program
     * directly by HW.
     */
    if (is_hwsupport) {
        return _tscbh_iblk_write_lane(pa, hwlane, addr, data);
    } else {
        PHYMOD_VDBG(DBG_ACC,pa,("invalid lane map=%x \n", lane_map));
        return -1;
    }
}

/* this function should be only be invoked to access 4 copy per MPP PCS register
 * any PMD register access should be treated as error */
int
phymod_tsco4_iblk_write(const phymod_access_t *pa, uint32_t addr, uint32_t data)
{
    uint32_t devad = (addr >> 16) & 0xf;
    uint32_t lane_map, hwlane = 0, is_hwsupport = 1;
    uint8_t is_8_lane_port = 0;

    if (addr & PHYMOD_REG_ACC_AER_IBLK_FORCE_LANE) {
        /* Forcing lane overrides default behavior */
        hwlane = (addr >> PHYMOD_REG_ACCESS_FLAGS_SHIFT) & 0x7;
        return _tscbh_iblk_write_lane(pa, hwlane, addr, data);
    }
    lane_map = PHYMOD_ACC_LANE_MASK(pa) & 0xffff;
    is_8_lane_port = (lane_map >> PHYMOD_TSCO_8_LANE_PORT_SHIFT) & 0x1;
    /* Extract absolute lane_map. */
    lane_map &= 0xff;

    /* need to make sure this function is only for pcs device */
    if (devad) {
        /*  invalid PMD device read */
        PHYMOD_VDBG(DBG_ACC,pa,("invalid PMD register access add=%x \n", addr));
        return -1;
    } else {
        /* pcs device */
        switch (lane_map) {
            case 0x0:
                hwlane = 0x100;
                break;
            case 0x1:
                hwlane = 0x100;
                break;
            case 0x2:
                hwlane = 0x101;
                break;
            case 0x4:
                hwlane = 0x102;
                break;
            case 0x8:
                hwlane = 0x103;
                break;
            case 0x10:
                /* need check if 8 lane port */
                if (is_8_lane_port) {
                    hwlane = 0x200;
                } else {
                    hwlane = 0x100;
                }
                break;
            case 0x20:
                if (is_8_lane_port) {
                    hwlane = 0x201;
                } else {
                    hwlane = 0x101;
                }
                break;
            case 0x40:
                if (is_8_lane_port) {
                    hwlane = 0x202;
                } else {
                    hwlane = 0x102;
                }
                break;
            case 0x80:
                if (is_8_lane_port) {
                    hwlane = 0x203;
                } else {
                    hwlane = 0x103;
                }
                break;
            case 0xf:
                hwlane = PHYMOD_TSCBH_IBLK_MCAST0123;
                break;
            case 0xf0:
                hwlane = PHYMOD_TSCBH_IBLK_MCAST0123;
                break;
            case 0xff:
                hwlane = PHYMOD_TSCBH_IBLK_BCAST;
                break;
            case 0x3:
                hwlane =  PHYMOD_TSCBH_IBLK_MCAST01;
                break;
            case 0xc:
                hwlane = PHYMOD_TSCBH_IBLK_MCAST23;
                break;
            case 0x30:
                hwlane = PHYMOD_TSCBH_IBLK_MCAST01;
                break;
            case 0xc0:
                hwlane = PHYMOD_TSCBH_IBLK_MCAST23;
                break;
            default:
                is_hwsupport = 0;
                hwlane = 0x0;
        }
     }
     /*
     * If the lane mask is supported by HW, i.e. a single lane or a
     * supported multicast combination (0x3, 0xC or 0xF), then program
     * directly by HW.
     */
    if (is_hwsupport) {
        return _tscbh_iblk_write_lane(pa, hwlane, addr, data);
    } else {
        PHYMOD_VDBG(DBG_ACC,pa,("invalid lane map=%x \n", lane_map));
        return -1;
    }
}
