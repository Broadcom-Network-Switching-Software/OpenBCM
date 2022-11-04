/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * This software simulator can emulate basic register access for the
 * TSC/Osprey SerDes PHY.
 *
 * The simulator suppor Broadcom proprietary SBUS access.
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
 */

#include <phymod/phymod_system.h>
#include <phymod/phymod_sim.h>

/* Convenience macro */
#define DBG_VERB PHYMOD_DEBUG_VERBOSE

/* Bit field get/set macros */
#define TSCO_BF_SET(_val, _mask, _shift) _val |= ((_mask) << (_shift))
#define TSCO_BF_GET(_val, _mask, _shift) (((_val) >> (_shift)) & (_mask))

/*
 * Raw 32-bit address consists of AER value in upper 16 bits and
 * clause 45 address in lower 16 bits.
 */
#define TSCO_DEVAD_SHIFT       27
#define TSCO_DEVAD_MASK        0x1f
#define TSCO_DEVAD_GET(_addr) \
    TSCO_BF_GET(_addr, TSCO_DEVAD_MASK, TSCO_DEVAD_SHIFT)
#define TSCO_LANE_SHIFT        16
#define TSCO_LANE_MASK         0xff
#define TSCO_LANE_GET(_addr) \
    TSCO_BF_GET(_addr, TSCO_LANE_MASK, TSCO_LANE_SHIFT)
#define TSCO_REG_SHIFT         0
#define TSCO_REG_MASK          0xffff
#define TSCO_REG_GET(_addr) \
    TSCO_BF_GET(_addr, TSCO_REG_MASK, TSCO_REG_SHIFT)
#define TSCO_MPP_ID_SHIFT      24
#define TSCO_MPP_ID_MASK       0x3
#define TSCO_MPP_ID_GET(_addr) \
    TSCO_BF_GET(_addr, TSCO_MPP_ID_MASK, TSCO_MPP_ID_SHIFT)

#define TSCO_ADDR(_devad, _lane, _reg) \
    (((_devad) << TSCO_DEVAD_SHIFT) +  \
     ((_lane) << TSCO_LANE_SHIFT) +    \
     ((_reg) << TSCO_REG_SHIFT))

#define TSCO_ADDR_PCS(_devad, _mpp_id, _lane, _reg) \
   (((_devad) << TSCO_DEVAD_SHIFT) + \
    ((_mpp_id) << TSCO_MPP_ID_SHIFT) + \
    ((_lane) << TSCO_LANE_SHIFT) + \
    ((_reg) << TSCO_REG_SHIFT))

#define TSCO_AER               TSCO_ADDR(0, 0, 0xffde)
#define TSCO_BLK               TSCO_ADDR(0, 0, 0x001f)

#define TSCO_ID0                 0x600d
#define TSCO_ID1                 0x8770
#define TSCO_MODEL               0x29
#define TSCO_SERDES_ID           0x0369 /* 0x9008 Main0_serdesID - Serdes ID Register */

#define TSCO_MPP_0               0x1
#define TSCO_MPP_1               0x2
#define TSCO_MPP_0_1             0x3

#define TSCO_SIM_FW_LOAD_DONE_REG_ADDR (0x800d203)

/*RAM sim*/
#define TSCO_RAM_WR_ADDR_REG_MS   (0xd205)
#define TSCO_RAM_WR_ADDR_REG_LS   (0xd204)
#define TSCO_RAM_WR_DATA_REG_MS   (0xd207)
#define TSCO_RAM_WR_DATA_REG_LS   (0xd206)
#define TSCO_RAM_RD_ADDR_REG_MS   (0xd209)
#define TSCO_RAM_RD_ADDR_REG_LS   (0xd208)
#define TSCO_RAM_RD_DATA_REG_MS   (0xd20b)
#define TSCO_RAM_RD_DATA_REG_LS   (0xd20a)
#define TSCO_IS_RAM_ADDR_REG(reg) (reg == TSCO_RAM_WR_ADDR_REG_MS || reg == TSCO_RAM_WR_ADDR_REG_LS || reg == TSCO_RAM_RD_ADDR_REG_MS || reg == TSCO_RAM_RD_ADDR_REG_LS)
#define TSCO_IS_RAM_DATA_REG(reg) (reg == TSCO_RAM_WR_DATA_REG_MS || reg == TSCO_RAM_WR_DATA_REG_LS || reg == TSCO_RAM_RD_DATA_REG_MS || reg == TSCO_RAM_RD_DATA_REG_LS)

#define TSCO_SIM_ENTRY_F_RAM_LS_DATA_ENTRY (0x1)
#define TSCO_SIM_ENTRY_F_RAM_MS_DATA_ENTRY (0x2)
#define TSCO_SIM_ENTRY_F_RAM_LS_ADDR_ENTRY (0x4)
#define TSCO_SIM_ENTRY_F_RAM_MS_ADDR_ENTRY (0x8)


/* Forward declarations */
STATIC int
tsco_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data);
STATIC int
tsco_sim_read(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t *data);


STATIC uint32_t
tsco_sim_default_data_get(uint32_t addr)
{
    uint32_t reg;

    reg = TSCO_REG_GET(addr);

    /*force FW load success */
    if(addr == TSCO_SIM_FW_LOAD_DONE_REG_ADDR) {
        return 1;
    }
    switch (reg) {
        case 0x0002:
            return TSCO_ID0;
        case 0x0003:
            return TSCO_ID1;
        case 0x9000:
            return 0x81;
        case 0x9008: /* Main0_serdesID - Serdes ID Register */
            return TSCO_SERDES_ID;
        case 0xc160: /* return link status as up */
            return 0x400;
        case 0xd57d:
            /*
             * This is DSC_A_DSC_UC_CTRL for Osprey. Make sure
             * The 'ready' bit is set.
             */
            return 0x0080;
        case 0xd0b9: /* RXTXCOM_CKRST_CTRL_RXTXCOM_LANE_DP_RESET_STATE_STATUS */
            return 0x7;
        case 0xd100:
            return TSCO_MODEL;
        case 0xd10a: /* DIG_COM_REVID1 - REVID1 */
            return 0x803c;
        case 0xd119: /* PLL_CONTROL_9 - AMS PLL CONTROL_9 */
            return 0x1860;
        case 0xd11a: /* PLL_CONTROL_10 - AMS PLL CONTROL_10 */
            return 0x3b;
        case 0xd11c: /* PLL_CONTROL_12 - AMS PLL CONTROL_12 */
            return 0x900;
        case 0xd148: /* PLL_CALSTS_0 - PLL_CALSTS_0 */
            return 0x308;
        case 0xd188:
            /*
             * Reset check
             */
            return 0x7;
        case 0xd170: /* TLB_TX_patt_gen_config - Pattern Generator Control */
            return 0xb000;
        case 0xd1c9: /* LANE_DP_RESET_STATUS */
            return 0x7;
        case 0xd327: /* PLL_INTCTRL - INTERNAL PLL CONTROL */
            return 0x4;
        default:
            break;
    }

    return 0;
}

STATIC uint32_t
tsco_sim_write_adjust(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
{
    uint32_t devad, reg, val;

    devad = TSCO_DEVAD_GET(addr);
    reg = TSCO_REG_GET(addr);

    if (devad == 0) {
        switch (reg) {
        case 0xc050:
            /* Sync configured speed into final speed status register
             * 0x20 = 0xc070 - 0xc050 */
            tsco_sim_read(pms_data, addr + 0x20, &val);
            val = (val & 0xffff03ff) | ((data & 0x3f) << 10);
            tsco_sim_write(pms_data, addr + 0x20, val);
            /* Set speed_change_done bit (0xc051 bit 0) if sw_speed_change bit
             * (0xc050 bit 8) is set.
             */
            tsco_sim_read(pms_data, addr + 0x1, &val);
            val = (val & 0xfffffffe) | ((data & 0x100) >> 8);
            tsco_sim_write(pms_data, addr + 0x1, val);
            break;
        default:
            break;
        }
    }

    return data;
}

STATIC int
_tsco_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
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
            DBG_VERB(("_tsco_sim_write 0x%08"PRIx32" = 0x%04"PRIx32"\n",
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
    if (data == tsco_sim_default_data_get(addr)) {
        return PHYMOD_E_NONE;
    }

    /* Add new register value */
    pse = &pms_data->entries[pms_data->entries_used++];
    pse->addr = addr;
    pse->data = data;

    DBG_VERB(("_tsco_sim_write 0x%08"PRIx32" = 0x%04"PRIx32" (new)\n",
              addr, pse->data));

    return PHYMOD_E_NONE;
}



STATIC uint32_t
tsco_sim_reg_copies_get(uint32_t addr)
{
    uint32_t devad, reg;

    devad = TSCO_DEVAD_GET(addr);
    reg = TSCO_REG_GET(addr);

    if (reg == TSCO_AER || reg == TSCO_BLK) {
        return 1;
    }

    if (devad == 0) {
        if ((reg & 0xf000) == 0x9000) {
            return 2;
        }
        if (((reg & 0xfff0) == 0xc010) ||
            ((reg & 0xfff0) == 0xc150) ||
            ((reg & 0xfff0) == 0xc170) ||
            ((reg & 0xfff0) == 0xc1a0) ||
            ((reg & 0xfff0) == 0xc210)) {
            return 8;
        }
        if ((reg & 0xf000) == 0xc000) {
            return 4;
        }
    } else if (devad == 1) {
        if (((reg & 0xfff0) == 0xd100) ||
            ((reg & 0xfff0) == 0xd190) ||
            ((reg & 0xfff0) == 0xd180) ||
            ((reg & 0xfff0) == 0xd140) ||
            ((reg & 0xfff0) == 0xd110) ||
            ((reg & 0xfff0) == 0xd320) ||
            ((reg & 0xfff0) == 0xd120) ||
            ((reg & 0xfff0) == 0xd200) ||
            ((reg & 0xfff0) == 0xd210) ||
            ((reg & 0xfff0) == 0xd220) ||
            ((reg & 0xfff0) == 0xd230) ||
            ((reg & 0xfff0) == 0xd310) ||
            ((reg & 0xfff0) == 0xffd0)) {
            return 1;
        }
        if ((reg & 0xfff0) == 0xd240) {
            /* separate for each micro. */
            return 4;
        }
        return 8;
    }
    return 0;
}

STATIC int
tsco_sim_init(phymod_sim_data_t *pms_data,
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
tsco_sim_reset(phymod_sim_data_t *pms_data)
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
tsco_sim_read(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t *data)
{
    int idx;
    uint32_t aer, devad, reg, copies;
    uint32_t lane = 0;
    uint32_t mpp_id=0;
    phymod_sim_entry_t *pse;
    uint32_t pse_flag = 0;
    uint32_t addr_ms, addr_ls;

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    devad = 0;

    if (addr != TSCO_AER && addr != TSCO_BLK) {
        /* Assume AER is in upper 16 bits */
        aer = (addr >> 16);
        if (aer == 0) {
            /* Try reading real AER instead */
            (void)tsco_sim_read(pms_data, TSCO_AER, &aer);
        }
        /* Add clause 45 devad (if used) */
        aer |= (devad << 11);
        addr = (addr & 0xffff) | (aer << 16);
        devad = TSCO_DEVAD_GET(addr);

        lane = (aer & 0x7);

        if (devad != 1) { /* PCS registers */
            mpp_id = TSCO_MPP_ID_GET(addr);
            if (mpp_id == TSCO_MPP_0_1) {
                /* Force mpp0 if mpp is invalid */
                addr = TSCO_ADDR_PCS(devad, TSCO_MPP_0, lane, TSCO_REG_GET(addr));
            }
            if (lane > 3) {
                /* Force lane 0 if lane is invalid */
                lane = 0;
                addr = TSCO_ADDR_PCS(devad, TSCO_MPP_ID_GET(addr), lane, TSCO_REG_GET(addr));
            }
        }
    }

    /* Adjust lane according to number of copies */
    devad = TSCO_DEVAD_GET(addr);
    reg = TSCO_REG_GET(addr);
    copies = tsco_sim_reg_copies_get(addr);
    if (devad == 1) { /* PMD register */
        if (copies == 1) {
            lane = 0;
        }
    } else { /* PCS register */
        mpp_id = TSCO_MPP_ID_GET(addr);
        if (copies == 2) {
           lane = 0;
        }
    }

    /*handle ram read/write*/
    if (TSCO_IS_RAM_DATA_REG(reg) || TSCO_IS_RAM_ADDR_REG(reg)) {

        if (TSCO_IS_RAM_DATA_REG(reg)) {
            (void)tsco_sim_read(pms_data, TSCO_ADDR(devad, lane, TSCO_RAM_RD_ADDR_REG_LS), &addr_ls);
            (void)tsco_sim_read(pms_data, TSCO_ADDR(devad, lane, TSCO_RAM_RD_ADDR_REG_MS), &addr_ms);
            addr = addr_ls | (addr_ms << 16);
            if (reg == TSCO_RAM_RD_DATA_REG_LS || reg == TSCO_RAM_WR_DATA_REG_LS) {
                pse_flag |= TSCO_SIM_ENTRY_F_RAM_LS_DATA_ENTRY;
            } else {
                pse_flag |= TSCO_SIM_ENTRY_F_RAM_MS_DATA_ENTRY;
            }
        } else { /*TSCO_IS_RAM_ADDR_REG*/
            addr = TSCO_ADDR(devad, lane, 0 /*dummy*/);
            if (reg == TSCO_RAM_RD_ADDR_REG_LS || reg == TSCO_RAM_WR_ADDR_REG_LS) {
                pse_flag |= TSCO_SIM_ENTRY_F_RAM_LS_DATA_ENTRY;
            } else {
                pse_flag |= TSCO_SIM_ENTRY_F_RAM_MS_DATA_ENTRY;
            }
        }

        /* Check if this register has been written already */
        for (idx = 0; idx < pms_data->entries_used; idx++) {
            pse = &pms_data->entries[idx];
            if ((pse->addr == addr) && (pse->flags == pse_flag)) {
                *data = pse->data;
                DBG_VERB(("tsco_sim_read 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                          addr, *data));
                return PHYMOD_E_NONE;
            }
        }

        *data = 0;
        return PHYMOD_E_NONE;
    }


    if (devad == 1) {
        addr = TSCO_ADDR(devad, lane, reg);
    } else {
        addr = TSCO_ADDR_PCS(devad, mpp_id, lane, reg);
    }

    /* Check if this register has been written already */
    for (idx = 0; idx < pms_data->entries_used; idx++) {
        pse = &pms_data->entries[idx];
        if ((pse->addr == addr) && (pse->flags == pse_flag)) {
            *data = pse->data;
            DBG_VERB(("tsco_sim_read 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                      addr, *data));
            return PHYMOD_E_NONE;
        }
    }

    /* Return default value if register was never written */
    *data = tsco_sim_default_data_get(addr);
    DBG_VERB(("tsco_sim_read 0x%08"PRIx32" = [0x%04"PRIx32"]\n",
              addr, *data));

    return PHYMOD_E_NONE;
}

STATIC int
tsco_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
{
    uint32_t aer, devad, reg, copies;
    uint32_t lane = 0;
    uint32_t mpp_id;

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    devad = 0;

    if (addr != TSCO_AER && addr != TSCO_BLK) {
        /* Assume AER is in upper 16 bits */
        aer = (addr >> 16);
        if (aer == 0) {
            /* Try reading real AER instead */
            (void)tsco_sim_read(pms_data, TSCO_AER, &aer);
        }

        /* Add clause 45 devad (if used) */
        aer |= (devad << 11);
        addr = (addr & 0xffff) | (aer << 16);
        devad = TSCO_DEVAD_GET(addr);
        reg = TSCO_REG_GET(addr);

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

                if (lane == 0x20) {
                    /* Write lanes 0 and 1 */
                    addr = TSCO_ADDR(devad, 0, reg);
                    (void)tsco_sim_write(pms_data, addr, data);
                    addr = TSCO_ADDR(devad, 1, reg);
                    (void)tsco_sim_write(pms_data, addr, data);
                }
                if (lane == 0x21) {
                    /* Write lanes 2 and 3 */
                    addr = TSCO_ADDR(devad, 2, reg);
                    (void)tsco_sim_write(pms_data, addr, data);
                    addr = TSCO_ADDR(devad, 3, reg);
                    (void)tsco_sim_write(pms_data, addr, data);
                }
                if (lane == 0x22) {
                    /* Write lanes 4 and 5 */
                    addr = TSCO_ADDR(devad, 4, reg);
                    (void)tsco_sim_write(pms_data, addr, data);
                    addr = TSCO_ADDR(devad, 5, reg);
                    (void)tsco_sim_write(pms_data, addr, data);
                }
                if (lane == 0x23) {
                    /* Write lanes 6 and 7 */
                    addr = TSCO_ADDR(devad, 6, reg);
                    (void)tsco_sim_write(pms_data, addr, data);
                    addr = TSCO_ADDR(devad, 7, reg);
                    (void)tsco_sim_write(pms_data, addr, data);
                }
                if (lane == 0x40) {
                    /* Write lanes 0,1,2,and 3 */
                    addr = TSCO_ADDR(devad, 0, reg);
                    (void)tsco_sim_write(pms_data, addr, data);
                    addr = TSCO_ADDR(devad, 1, reg);
                    (void)tsco_sim_write(pms_data, addr, data);
                    addr = TSCO_ADDR(devad, 2, reg);
                    (void)tsco_sim_write(pms_data, addr, data);
                    addr = TSCO_ADDR(devad, 3, reg);
                    (void)tsco_sim_write(pms_data, addr, data);
                }
                if (lane == 0x41) {
                    /* Write lanes 4,5,6 and 7 */
                    addr = TSCO_ADDR(devad, 4, reg);
                    (void)tsco_sim_write(pms_data, addr, data);
                    addr = TSCO_ADDR(devad, 5, reg);
                    (void)tsco_sim_write(pms_data, addr, data);
                    addr = TSCO_ADDR(devad, 6, reg);
                    (void)tsco_sim_write(pms_data, addr, data);
                    addr = TSCO_ADDR(devad, 7, reg);
                    (void)tsco_sim_write(pms_data, addr, data);
                }
                if (lane == 0xff) {
                    /* Write all lanes */
                    addr = TSCO_ADDR(devad, 0, reg);
                    (void)tsco_sim_write(pms_data, addr, data);
                    addr = TSCO_ADDR(devad, 1, reg);
                    (void)tsco_sim_write(pms_data, addr, data);
                    addr = TSCO_ADDR(devad, 2, reg);
                    (void)tsco_sim_write(pms_data, addr, data);
                    addr = TSCO_ADDR(devad, 3, reg);
                    (void)tsco_sim_write(pms_data, addr, data);
                    addr = TSCO_ADDR(devad, 4, reg);
                    (void)tsco_sim_write(pms_data, addr, data);
                    addr = TSCO_ADDR(devad, 5, reg);
                    (void)tsco_sim_write(pms_data, addr, data);
                    addr = TSCO_ADDR(devad, 6, reg);
                    (void)tsco_sim_write(pms_data, addr, data);
                    addr = TSCO_ADDR(devad, 7, reg);
                    (void)tsco_sim_write(pms_data, addr, data);
                }
                return PHYMOD_E_NONE;
            }
        } else { /* Access PCS registers */
            mpp_id = TSCO_MPP_ID_GET(addr);
            lane = aer & 0x7;
            if (lane > 0x6) {
                return PHYMOD_E_PARAM;
            }
            if (mpp_id == TSCO_MPP_0_1) {
                addr = TSCO_ADDR_PCS(devad, TSCO_MPP_0, lane, reg);
                (void)tsco_sim_write(pms_data, addr, data);
                addr = TSCO_ADDR_PCS(devad, TSCO_MPP_1, lane, reg);
                (void)tsco_sim_write(pms_data, addr, data);
                return PHYMOD_E_NONE;
            } else {
                if (lane > 3) {
                    if (lane == 0x4) {
                        /* Write lanes 0 and 1 */
                        addr = TSCO_ADDR_PCS(devad, mpp_id, 0, reg);
                        (void)tsco_sim_write(pms_data, addr, data);
                        addr = TSCO_ADDR_PCS(devad, mpp_id, 1, reg);
                        (void)tsco_sim_write(pms_data, addr, data);
                    }
                    if (lane == 0x5) {
                        /* Write lanes 2 and 3 */
                        addr = TSCO_ADDR_PCS(devad, mpp_id, 2, reg);
                        (void)tsco_sim_write(pms_data, addr, data);
                        addr = TSCO_ADDR_PCS(devad, mpp_id, 3, reg);
                        (void)tsco_sim_write(pms_data, addr, data);
                    }
                    if (lane == 0x6) {
                        /* Write lanes 0, 1, 2 and 3 */
                        addr = TSCO_ADDR_PCS(devad, mpp_id, 0, reg);
                        (void)tsco_sim_write(pms_data, addr, data);
                        addr = TSCO_ADDR_PCS(devad, mpp_id, 1, reg);
                        (void)tsco_sim_write(pms_data, addr, data);
                        addr = TSCO_ADDR_PCS(devad, mpp_id, 2, reg);
                        (void)tsco_sim_write(pms_data, addr, data);
                        addr = TSCO_ADDR_PCS(devad, mpp_id, 3, reg);
                        (void)tsco_sim_write(pms_data, addr, data);
                    }
                    return PHYMOD_E_NONE;
                }
            }
        }
    }

    /* Adjust data and/or related registers */
    data = tsco_sim_write_adjust(pms_data, addr, data);

    /* Adjust lane according to number of copies */
    devad = TSCO_DEVAD_GET(addr);
    reg = TSCO_REG_GET(addr);
    copies = tsco_sim_reg_copies_get(addr);

    if (devad == 1) { /* PMD registers */
        if (copies == 1) {
            lane = 0;
        }
        addr = TSCO_ADDR(devad, lane, reg);
    } else { /* PCS registers */
        mpp_id = TSCO_MPP_ID_GET(addr);
        if (copies == 2) {
            lane = 0;
        }
        addr = TSCO_ADDR_PCS(devad, mpp_id, lane, reg);
    }

    return _tsco_sim_write(pms_data, addr, data);
}

STATIC int
tsco_sim_event(phymod_sim_data_t *pms_data, phymod_sim_event_t event)
{
    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    return PHYMOD_E_NONE;
}

phymod_sim_drv_t tsco_sim_drv = {
    tsco_sim_init,
    tsco_sim_reset,
    tsco_sim_read,
    tsco_sim_write,
    NULL,
    NULL,
    tsco_sim_event
};
