/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *
 * This software simulator can emulate basic register access for the
 * TSC/Eagle SerDes PHY.
 *
 * The simulator suppor both IEEE clause 22/45 access and Broadcom
 * proprietary SBUS access.
 *
 * Clause 22 address format:
 *   Bits [4:0]   : Clause 22 register address
 *   Bits [31:5]  : Unused
 *
 * Clause 45 address format:
 *   Bits [15:0]  : Clause 45 register address
 *   Bits [20:16] : Clause 45 DEVAD
 *   Bits [23:21] : Clause 45 indicator (001b)
 *   Bits [31:24] : Unused
 *
 * SBUS address format:
 *   Bits [15:0]  : Clause 45 register address
 *   Bits [18:16] : Lane control
 *   Bits [26:19] : Lane multicast (old format)
 *   Bits [31:27] : Clause 45 DEVAD
 *
 * The upper 16 bits if the SBUS address format is identical to the
 * Broadcom Address Extension Register (AER) format.
 *
 * The clause 45 indicator serves two purposes which is to ensure that
 * the upper 16 bits are never zero for a clause 45 address, but it
 * also makes it possible for the PHY bus driver to distinguish
 * between a clause 45 DEVAD and the old AER multicast format.
 */

#include <phymod/phymod_system.h>
#include <phymod/phymod_sim.h>

/* Convenience macro */
#define DBG_VERB PHYMOD_DEBUG_VERBOSE

/* Bit field get/set macros */
#define TSCBH_BF_SET(_val, _mask, _shift) _val |= ((_mask) << (_shift))
#define TSCBH_BF_GET(_val, _mask, _shift) (((_val) >> (_shift)) & (_mask))

/*
 * Raw 32-bit address consists of AER value in upper 16 bits and
 * clause 45 address in lower 16 bits.
 */
#define TSCBH_DEVAD_SHIFT       27
#define TSCBH_DEVAD_MASK        0x1f
#define TSCBH_DEVAD_GET(_addr) \
    TSCBH_BF_GET(_addr, TSCBH_DEVAD_MASK, TSCBH_DEVAD_SHIFT)
#define TSCBH_LANE_SHIFT        16
#define TSCBH_LANE_MASK         0xff
#define TSCBH_LANE_GET(_addr) \
    TSCBH_BF_GET(_addr, TSCBH_LANE_MASK, TSCBH_LANE_SHIFT)
#define TSCBH_PLL_ID_SHIFT      24
#define TSCBH_PLL_ID_MASK       0x3
#define TSCBH_PLL_ID_GET(_addr) \
    TSCBH_BF_GET(_addr, TSCBH_PLL_ID_MASK, TSCBH_PLL_ID_SHIFT)
#define TSCBH_REG_SHIFT         0
#define TSCBH_REG_MASK          0xffff
#define TSCBH_REG_GET(_addr) \
    TSCBH_BF_GET(_addr, TSCBH_REG_MASK, TSCBH_REG_SHIFT)
#define TSCBH_MPP_ID_SHIFT      24
#define TSCBH_MPP_ID_MASK       0x3
#define TSCBH_MPP_ID_GET(_addr) \
    TSCBH_BF_GET(_addr, TSCBH_MPP_ID_MASK, TSCBH_MPP_ID_SHIFT)

#define TSCBH_ADDR(_devad, _pll_id, _lane, _reg) \
    (((_devad) << TSCBH_DEVAD_SHIFT) +  \
     ((_pll_id) << TSCBH_PLL_ID_SHIFT) +  \
     ((_lane) << TSCBH_LANE_SHIFT) +    \
     ((_reg) << TSCBH_REG_SHIFT))

#define TSCBH_ADDR_PCS(_devad, _mpp_id, _lane, _reg) \
   (((_devad) << TSCBH_DEVAD_SHIFT) + \
    ((_mpp_id) << TSCBH_MPP_ID_SHIFT) + \
    ((_lane) << TSCBH_LANE_SHIFT) + \
    ((_reg) << TSCBH_REG_SHIFT))

#define TSCBH_AER               TSCBH_ADDR(0, 0, 0, 0xffde)
#define TSCBH_BLK               TSCBH_ADDR(0, 0, 0, 0x001f)

/*
 * The CL45 indicator is used to determine whether the upper 16 bits
 * of the address is an AER value or a clause 45 DEVAD.
 */
#define TSCBH_CL45              (0x20 << 16)
#define TSCBH_CL45_MASK         (0xe0 << 16)

#define TSCBH_ID0                 0x600d
#define TSCBH_ID1                 0x8770
#define TSCBH_MODEL               0x26
#define TSCBH_SERDES_ID           0x0325 /* 0x9008 Main0_serdesID - Serdes ID Register */

#define TSCBH_MPP_0               0x1
#define TSCBH_MPP_1               0x2
#define TSCBH_MPP_0_1             0x3

#define TSCBH_SIM_FW_LOAD_DONE_REG_ADDR (0x800d203)

/*RAM sim*/
#define TSCBH_RAM_WR_ADDR_REG_MS   (0xd205)
#define TSCBH_RAM_WR_ADDR_REG_LS   (0xd204)
#define TSCBH_RAM_WR_DATA_REG_MS   (0xd207)
#define TSCBH_RAM_WR_DATA_REG_LS   (0xd206)
#define TSCBH_RAM_RD_ADDR_REG_MS   (0xd209)
#define TSCBH_RAM_RD_ADDR_REG_LS   (0xd208)
#define TSCBH_RAM_RD_DATA_REG_MS   (0xd20b)
#define TSCBH_RAM_RD_DATA_REG_LS   (0xd20a)
#define TSCBH_IS_RAM_ADDR_REG(reg) (reg == TSCBH_RAM_WR_ADDR_REG_MS || reg == TSCBH_RAM_WR_ADDR_REG_LS || reg == TSCBH_RAM_RD_ADDR_REG_MS || reg == TSCBH_RAM_RD_ADDR_REG_LS)
#define TSCBH_IS_RAM_DATA_REG(reg) (reg == TSCBH_RAM_WR_DATA_REG_MS || reg == TSCBH_RAM_WR_DATA_REG_LS || reg == TSCBH_RAM_RD_DATA_REG_MS || reg == TSCBH_RAM_RD_DATA_REG_LS)

#define TSCBH_SIM_ENTRY_F_RAM_LS_DATA_ENTRY (0x1)
#define TSCBH_SIM_ENTRY_F_RAM_MS_DATA_ENTRY (0x2)
#define TSCBH_SIM_ENTRY_F_RAM_LS_ADDR_ENTRY (0x4)
#define TSCBH_SIM_ENTRY_F_RAM_MS_ADDR_ENTRY (0x8)


/* Forward declarations */
STATIC int
tscbh_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data);
STATIC int
tscbh_sim_read(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t *data);


STATIC uint32_t
tscbh_sim_default_data_get(uint32_t addr)
{
    uint32_t reg;

    reg = TSCBH_REG_GET(addr);

    /*force FW load success */
    if(addr == TSCBH_SIM_FW_LOAD_DONE_REG_ADDR) {
        return 1;
    }
    switch (reg) {
        case 0x0002:
            return TSCBH_ID0;
        case 0x0003:
            return TSCBH_ID1;
        case 0x9000:
            return 0x80;
        case 0x9008: /* Main0_serdesID - Serdes ID Register */
            return TSCBH_SERDES_ID;
        case 0xd03d:
            /*
             * This is DSC_A_DSC_UC_CTRL for Falcon. Make sure
             * The 'ready' bit is set.
             */
            return 0x0080;
        case 0xd0b9: /* RXTXCOM_CKRST_CTRL_sinai_LANE_DP_RESET_STATE_STATUS */
            return 0x7;
        case 0xd100:
            return TSCBH_MODEL;
        case 0xd10a: /* DIG_COM_REVID1 - REVID1 */
            return 0x803c;
        case 0xd147:
            return 0x7;
        case 0xd148: /* PLL_CALSTS_0 - PLL_CALSTS_0 */
            return 0x100;
        case 0xd188:
            /*
             * Reset check
             */
            return 0x7;
        case 0xd170: /* TLB_TX_patt_gen_config - Pattern Generator Control */
            return 0xb000;
        case 0xd1c9: /* LANE_DP_RESET_STATUS */
            return 0x7;
        default:
            break;
    }

    return 0;
}

STATIC uint32_t
tscbh_sim_write_adjust(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
{
    uint32_t devad, reg, val;

    devad = TSCBH_DEVAD_GET(addr);
    reg = TSCBH_REG_GET(addr);

    if (devad == 0) {
        switch (reg) {
        case 0xc050:
            /* Sync configured speed into final speed status register
             * 0x20 = 0xc070 - 0xc050 */
            tscbh_sim_read(pms_data, addr + 0x20, &val);
            val = (val & 0xffff03ff) | ((data & 0x3f) << 10);
            tscbh_sim_write(pms_data, addr + 0x20, val);
            break;
        default:
            break;
        }
    }

    return data;
}

STATIC int
_tscbh_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
{
    int idx;
    uint32_t mask;

    phymod_sim_entry_t *pse;

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    /* Support optional write mask in upper 16 bits */
    mask = (data >> 16);
    if (mask == 0) {
        mask = 0xffff;
    }
    data &= mask;

    /* Check if this register has been written already */
    for (idx = 0; idx < pms_data->entries_used; idx++) {
        pse = &pms_data->entries[idx];
        if (pse->addr == addr) {
            pse->data &= ~mask;
            pse->data |= data;
            DBG_VERB(("_tscbh_sim_write 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                      addr, pse->data));
            return PHYMOD_E_NONE;
        }
    }

    /* Check if database is full */
    if (pms_data->entries_used >= pms_data->num_entries) {
        PHYMOD_DEBUG_ERROR(("phy sim ERROR: Nof modified registers is larger than phy sim database. Please increase PHY_NUM_SIM_ENTRIES.\n"));
        return PHYMOD_E_RESOURCE;
    }

    /* Check if new data matches default value */
    if (data == tscbh_sim_default_data_get(addr)) {
        return PHYMOD_E_NONE;
    }

    /* Add new register value */
    pse = &pms_data->entries[pms_data->entries_used++];
    pse->addr = addr;
    pse->data = data;

    DBG_VERB(("_tscbh_sim_write 0x%08"PRIx32" = 0x%04"PRIx32" (new)\n",
              addr, pse->data));

    return PHYMOD_E_NONE;
}



STATIC uint32_t
tscbh_sim_reg_copies_get(uint32_t addr)
{
    uint32_t devad, reg;

    devad = TSCBH_DEVAD_GET(addr);
    reg = TSCBH_REG_GET(addr);

    if (reg == TSCBH_AER || reg == TSCBH_BLK) {
        return 1;
    }

    if (devad == 0) {
        if ((reg & 0xf000) == 0x9000) {
            return 2;
        }
        if ((reg & 0xf000) == 0xc000) {
            return 4;
        }
    }
    else if (devad == 1) {
        if ((reg & 0xfff0) == 0xd100) {
            return 1;
        }
        if ((reg & 0xfff0) == 0xd190) {
            return 1;
        }
        if ((reg & 0xfff0) == 0xd180) {
            return 2;
        }
        if ((reg & 0xfff0) == 0xd140) {
            return 2;
        }
        if ((reg & 0xfff0) == 0xd110) {
            return 2;
        }
        return 8;
    }
    return 0;
}

STATIC int
tscbh_sim_init(phymod_sim_data_t *pms_data,
               int num_entries, phymod_sim_entry_t *entries)
{
    if (pms_data != NULL) {
        PHYMOD_MEMSET(pms_data, 0, sizeof(*pms_data));
        pms_data->num_entries = num_entries;
        pms_data->entries = entries;
    }
    return PHYMOD_E_NONE;
}

STATIC int
tscbh_sim_reset(phymod_sim_data_t *pms_data)
{
    uint32_t sim_size;

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    pms_data->entries_used = 0;
    sim_size = pms_data->num_entries * sizeof(phymod_sim_entry_t);
    PHYMOD_MEMSET(pms_data->entries, 0, sim_size);

    return PHYMOD_E_NONE;
}

STATIC int
tscbh_sim_read(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t *data)
{
    int idx;
    uint32_t aer, blk, devad, reg, copies, pll_id=0;
    uint32_t lane = 0;
    uint32_t mpp_id=0;
    phymod_sim_entry_t *pse;
    uint32_t pse_flag = 0;
    uint32_t addr_ms, addr_ls;

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    devad = 0;


    if (addr < TSCBH_BLK) {
        /* Assume clause 22 access */
        (void)tscbh_sim_read(pms_data, TSCBH_BLK, &blk);
        if (addr & 0x10) {
            /* IEEE bit */
            blk |= 0x8000;
        }
        addr = (addr & 0xf) | (blk & 0xfff0);
        if (addr != TSCBH_AER && addr != TSCBH_BLK) {
            (void)tscbh_sim_read(pms_data, TSCBH_AER, &aer);
            addr |= (aer << 16);
        }
    } else {
        /* Extract devad if clause 45 address format */
        if ((addr & TSCBH_CL45_MASK) == TSCBH_CL45) {
            devad = (addr >> 16) & 0x1f;
            addr &= 0xffff;
        }
    }

    if (addr != TSCBH_AER && addr != TSCBH_BLK) {
        /* Assume AER is in upper 16 bits */
        aer = (addr >> 16);
        if (aer == 0) {
            /* Try reading real AER instead */
            (void)tscbh_sim_read(pms_data, TSCBH_AER, &aer);
        }
        /* Add clause 45 devad (if used) */
        aer |= (devad << 11);
        addr = (addr & 0xffff) | (aer << 16);

        lane = (aer & 0x7);

        if (devad != 1) { /* PCS registers */
            mpp_id = TSCBH_MPP_ID_GET(addr);
            if (mpp_id == TSCBH_MPP_0_1) {
                /* Force mpp0 if mpp is invalid */
                addr = TSCBH_ADDR_PCS(TSCBH_DEVAD_GET(addr), TSCBH_MPP_0, lane, TSCBH_REG_GET(addr));
            }
            if (lane > 3) {
                /* Force lane 0 if lane is invalid */
                lane = 0;
                addr = TSCBH_ADDR_PCS(TSCBH_DEVAD_GET(addr), TSCBH_MPP_ID_GET(addr), lane, TSCBH_REG_GET(addr));
            }
        }
    }

    /* Adjust lane according to number of copies */
    devad = TSCBH_DEVAD_GET(addr);
    reg = TSCBH_REG_GET(addr);
    copies = tscbh_sim_reg_copies_get(addr);
    if (devad == 1) { /* PMD register */
        if (copies == 1) {
            lane = 0;
        } else if (copies == 2) {
            pll_id = TSCBH_PLL_ID_GET(addr);
            lane = (pll_id == 0) ? 0 : 1;
        }
    } else { /* PCS register */
        mpp_id = TSCBH_MPP_ID_GET(addr);
        if (copies == 2) {
           lane = 0;
        }
    }

    /*handle ram read/write*/
    if (TSCBH_IS_RAM_DATA_REG(reg) || TSCBH_IS_RAM_ADDR_REG(reg)) {

        if (TSCBH_IS_RAM_DATA_REG(reg)) {
            (void)tscbh_sim_read(pms_data, TSCBH_ADDR(devad, pll_id, lane, TSCBH_RAM_RD_ADDR_REG_LS), &addr_ls);
            (void)tscbh_sim_read(pms_data, TSCBH_ADDR(devad, pll_id, lane, TSCBH_RAM_RD_ADDR_REG_MS), &addr_ms);
            addr = addr_ls | (addr_ms << 16);
            if (reg == TSCBH_RAM_RD_DATA_REG_LS || reg == TSCBH_RAM_WR_DATA_REG_LS) {
                pse_flag |= TSCBH_SIM_ENTRY_F_RAM_LS_DATA_ENTRY;
            } else {
                pse_flag |= TSCBH_SIM_ENTRY_F_RAM_MS_DATA_ENTRY;
            }
        } else { /*TSCBH_IS_RAM_ADDR_REG*/
            addr = TSCBH_ADDR(devad, 0, lane, 0 /*dummy*/);
            if (reg == TSCBH_RAM_RD_ADDR_REG_LS || reg == TSCBH_RAM_WR_ADDR_REG_LS) {
                pse_flag |= TSCBH_SIM_ENTRY_F_RAM_LS_DATA_ENTRY;
            } else {
                pse_flag |= TSCBH_SIM_ENTRY_F_RAM_MS_DATA_ENTRY;
            }
        }

        /* Check if this register has been written already */
        for (idx = 0; idx < pms_data->entries_used; idx++) {
            pse = &pms_data->entries[idx];
            if ((pse->addr == addr) && (pse->flags == pse_flag)) {
                *data = pse->data;
                DBG_VERB(("tscbh_sim_read 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                          addr, *data));
                return PHYMOD_E_NONE;
            }
        }

        *data = 0;
        return PHYMOD_E_NONE;
    }


    if (devad == 1) {
        addr = TSCBH_ADDR(devad, pll_id, lane, reg);
    } else {
        addr = TSCBH_ADDR_PCS(devad, mpp_id, lane, reg);
    }

    /* Check if this register has been written already */
    for (idx = 0; idx < pms_data->entries_used; idx++) {
        pse = &pms_data->entries[idx];
        if ((pse->addr == addr) && (pse->flags == pse_flag)) {
            *data = pse->data;
            DBG_VERB(("tscbh_sim_read 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                      addr, *data));
            return PHYMOD_E_NONE;
        }
    }

    /* Return default value if register was never written */
    *data = tscbh_sim_default_data_get(addr);

    DBG_VERB(("tscbh_sim_read 0x%08"PRIx32" = [0x%04"PRIx32"]\n",
              addr, *data));

    return PHYMOD_E_NONE;
}

STATIC int
tscbh_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
{
    uint32_t aer, blk, devad, reg, copies, pll_id=0;
    uint32_t lane = 0;
    uint32_t mpp_id;

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    devad = 0;

    if (addr < TSCBH_BLK) {
        /* Assume clause 22 access */
        (void)tscbh_sim_read(pms_data, TSCBH_BLK, &blk);
        /* IEEE bit */
        if (addr & 0x10) {
            blk |= 0x8000;
        } else {
            blk &= ~0x8000;
        }
        addr = (addr & 0xf) | (blk & 0xfff0);
        if (addr != TSCBH_AER && addr != TSCBH_BLK) {
            (void)tscbh_sim_read(pms_data, TSCBH_AER, &aer);
            addr |= (aer << 16);
        }
    } else {
        /* Extract devad if clause 45 address format */
        if ((addr & TSCBH_CL45_MASK) == TSCBH_CL45) {
            devad = (addr >> 16) & 0x1f;
            addr &= 0xffff;
        }
    }

    if (addr != TSCBH_AER && addr != TSCBH_BLK) {
        /* Assume AER is in upper 16 bits */
        aer = (addr >> 16);
        if (aer == 0) {
            /* Try reading real AER instead */
            (void)tscbh_sim_read(pms_data, TSCBH_AER, &aer);
        }

        /* Add clause 45 devad (if used) */
        aer |= (devad << 11);
        addr = (addr & 0xffff) | (aer << 16);
        devad = TSCBH_DEVAD_GET(addr);
        reg = TSCBH_REG_GET(addr);

        if (devad == 1) { /* Access PMD registers */
            lane = (aer & 0xff);
            if (lane > 7) {
                /*
                 * Handle lane broadcast
                 *
                 * Note that we use lane 8 instead of lane 0 to prevent a
                 * broadcast loop. The value 8 will become 0 when masked
                 * with 0x7, but it prevents the AER in the upper 16 bits
                 * from being zero, which will cause the code above to
                 * obtain the AER value from register 0xffde.
                 */

                pll_id = TSCBH_PLL_ID_GET(addr);
                if (lane == 0x20) {
                    /* Write lanes 0 and 1 */
                    addr = TSCBH_ADDR(devad, pll_id, 0, reg);
                    (void)tscbh_sim_write(pms_data, addr, data);
                    addr = TSCBH_ADDR(devad, pll_id, 1, reg);
                    (void)tscbh_sim_write(pms_data, addr, data);
                }
                if (lane == 0x21) {
                    /* Write lanes 2 and 3 */
                    addr = TSCBH_ADDR(devad, pll_id, 2, reg);
                    (void)tscbh_sim_write(pms_data, addr, data);
                    addr = TSCBH_ADDR(devad, pll_id, 3, reg);
                    (void)tscbh_sim_write(pms_data, addr, data);
                }
                if (lane == 0x22) {
                    /* Write lanes 4 and 5 */
                    addr = TSCBH_ADDR(devad, pll_id, 4, reg);
                    (void)tscbh_sim_write(pms_data, addr, data);
                    addr = TSCBH_ADDR(devad, pll_id, 5, reg);
                    (void)tscbh_sim_write(pms_data, addr, data);
                }
                if (lane == 0x23) {
                    /* Write lanes 6 and 7 */
                    addr = TSCBH_ADDR(devad, pll_id, 6, reg);
                    (void)tscbh_sim_write(pms_data, addr, data);
                    addr = TSCBH_ADDR(devad, pll_id, 7, reg);
                    (void)tscbh_sim_write(pms_data, addr, data);
                }
                if (lane == 0x40) {
                    /* Write lanes 0,1,2,and 3 */
                    addr = TSCBH_ADDR(devad, pll_id, 0, reg);
                    (void)tscbh_sim_write(pms_data, addr, data);
                    addr = TSCBH_ADDR(devad, pll_id, 1, reg);
                    (void)tscbh_sim_write(pms_data, addr, data);
                    addr = TSCBH_ADDR(devad, pll_id, 2, reg);
                    (void)tscbh_sim_write(pms_data, addr, data);
                    addr = TSCBH_ADDR(devad, pll_id, 3, reg);
                    (void)tscbh_sim_write(pms_data, addr, data);
                }
                if (lane == 0x41) {
                    /* Write lanes 4,5,6 and 7 */
                    addr = TSCBH_ADDR(devad, pll_id, 4, reg);
                    (void)tscbh_sim_write(pms_data, addr, data);
                    addr = TSCBH_ADDR(devad, pll_id, 5, reg);
                    (void)tscbh_sim_write(pms_data, addr, data);
                    addr = TSCBH_ADDR(devad, pll_id, 6, reg);
                    (void)tscbh_sim_write(pms_data, addr, data);
                    addr = TSCBH_ADDR(devad, pll_id, 7, reg);
                    (void)tscbh_sim_write(pms_data, addr, data);
                }
                if (lane == 0xff) {
                    /* Write lanes 2 and 3 */
                    addr = TSCBH_ADDR(devad, pll_id, 0, reg);
                    (void)tscbh_sim_write(pms_data, addr, data);
                    addr = TSCBH_ADDR(devad, pll_id, 1, reg);
                    (void)tscbh_sim_write(pms_data, addr, data);
                    addr = TSCBH_ADDR(devad, pll_id, 2, reg);
                    (void)tscbh_sim_write(pms_data, addr, data);
                    addr = TSCBH_ADDR(devad, pll_id, 3, reg);
                    (void)tscbh_sim_write(pms_data, addr, data);
                    addr = TSCBH_ADDR(devad, pll_id, 4, reg);
                    (void)tscbh_sim_write(pms_data, addr, data);
                    addr = TSCBH_ADDR(devad, pll_id, 5, reg);
                    (void)tscbh_sim_write(pms_data, addr, data);
                    addr = TSCBH_ADDR(devad, pll_id, 6, reg);
                    (void)tscbh_sim_write(pms_data, addr, data);
                    addr = TSCBH_ADDR(devad, pll_id, 7, reg);
                    (void)tscbh_sim_write(pms_data, addr, data);
                }
                return PHYMOD_E_NONE;
            }
        } else { /* Access PCS registers */
            mpp_id = TSCBH_MPP_ID_GET(addr);
            lane = aer & 0x7;
            if (lane > 0x6) {
                return PHYMOD_E_PARAM;
            }
            if (mpp_id == TSCBH_MPP_0_1) {
                addr = TSCBH_ADDR_PCS(devad, TSCBH_MPP_0, lane, reg);
                (void)tscbh_sim_write(pms_data, addr, data);
                addr = TSCBH_ADDR_PCS(devad, TSCBH_MPP_1, lane, reg);
                (void)tscbh_sim_write(pms_data, addr, data);
                return PHYMOD_E_NONE;
            } else {
                if (lane > 3) {
                    if (lane == 0x4) {
                        /* Write lanes 0 and 1 */
                        addr = TSCBH_ADDR_PCS(devad, mpp_id, 0, reg);
                        (void)tscbh_sim_write(pms_data, addr, data);
                        addr = TSCBH_ADDR_PCS(devad, mpp_id, 1, reg);
                        (void)tscbh_sim_write(pms_data, addr, data);
                    }
                    if (lane == 0x5) {
                        /* Write lanes 2 and 3 */
                        addr = TSCBH_ADDR_PCS(devad, mpp_id, 2, reg);
                        (void)tscbh_sim_write(pms_data, addr, data);
                        addr = TSCBH_ADDR_PCS(devad, mpp_id, 3, reg);
                        (void)tscbh_sim_write(pms_data, addr, data);
                    }
                    if (lane == 0x6) {
                        /* Write lanes 0, 1, 2 and 3 */
                        addr = TSCBH_ADDR_PCS(devad, mpp_id, 0, reg);
                        (void)tscbh_sim_write(pms_data, addr, data);
                        addr = TSCBH_ADDR_PCS(devad, mpp_id, 1, reg);
                        (void)tscbh_sim_write(pms_data, addr, data);
                        addr = TSCBH_ADDR_PCS(devad, mpp_id, 2, reg);
                        (void)tscbh_sim_write(pms_data, addr, data);
                        addr = TSCBH_ADDR_PCS(devad, mpp_id, 3, reg);
                        (void)tscbh_sim_write(pms_data, addr, data);
                    }
                    return PHYMOD_E_NONE;
                }
            }
        }
    }

    /* Adjust data and/or related registers */
    data = tscbh_sim_write_adjust(pms_data, addr, data);

    /* Adjust lane according to number of copies */
    devad = TSCBH_DEVAD_GET(addr);
    reg = TSCBH_REG_GET(addr);
    copies = tscbh_sim_reg_copies_get(addr);

    if (devad == 1) { /* PMD registers */
        if (copies == 1) {
            lane = 0;
        } else if (copies == 2) {
            pll_id = TSCBH_PLL_ID_GET(addr);
            lane = (pll_id == 0) ? 0 : 1;
        }
        addr = TSCBH_ADDR(devad, pll_id, lane, reg);
    } else { /* PCS registers */
        mpp_id = TSCBH_MPP_ID_GET(addr);
        if (copies == 2) {
            lane = 0;
        }
        addr = TSCBH_ADDR_PCS(devad, mpp_id, lane, reg);
    }

    return _tscbh_sim_write(pms_data, addr, data);
}

STATIC int
tscbh_sim_event(phymod_sim_data_t *pms_data, phymod_sim_event_t event)
{
    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    return PHYMOD_E_NONE;
}

phymod_sim_drv_t tscbh_sim_drv = {
    tscbh_sim_init,
    tscbh_sim_reset,
    tscbh_sim_read,
    tscbh_sim_write,
    tscbh_sim_event
};
