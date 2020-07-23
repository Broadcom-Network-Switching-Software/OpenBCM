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
#ifndef EXCLUDE_PHYMOD_BLACKHAWK_SIM

#include <phymod/phymod_system.h>
#include <phymod/phymod_sim.h>

/* Convenience macro */
#define DBG_VERB PHYMOD_DEBUG_VERBOSE

/* Bit field get/set macros */
#define BLACKHAWK_BF_SET(_val, _mask, _shift) _val |= ((_mask) << (_shift))
#define BLACKHAWK_BF_GET(_val, _mask, _shift) (((_val) >> (_shift)) & (_mask))

/*
 * Raw 32-bit address consists of AER value in upper 16 bits and
 * clause 45 address in lower 16 bits.
 */
#define BLACKHAWK_DEVAD_SHIFT       27
#define BLACKHAWK_DEVAD_MASK        0x1f
#define BLACKHAWK_DEVAD_GET(_addr) \
    BLACKHAWK_BF_GET(_addr, BLACKHAWK_DEVAD_MASK, BLACKHAWK_DEVAD_SHIFT)
#define BLACKHAWK_LANE_SHIFT        16
#define BLACKHAWK_LANE_MASK         0xff
#define BLACKHAWK_LANE_GET(_addr) \
    BLACKHAWK_BF_GET(_addr, BLACKHAWK_LANE_MASK, BLACKHAWK_LANE_SHIFT)
#define BLACKHAWK_PLL_ID_SHIFT      24
#define BLACKHAWK_PLL_ID_MASK       0x3
#define BLACKHAWK_PLL_ID_GET(_addr) \
    BLACKHAWK_BF_GET(_addr, BLACKHAWK_PLL_ID_MASK, BLACKHAWK_PLL_ID_SHIFT)
#define BLACKHAWK_REG_SHIFT         0
#define BLACKHAWK_REG_MASK          0xffff
#define BLACKHAWK_REG_GET(_addr) \
    BLACKHAWK_BF_GET(_addr, BLACKHAWK_REG_MASK, BLACKHAWK_REG_SHIFT)

#define BLACKHAWK_ADDR(_devad, _pll_id, _lane, _reg) \
    (((_devad) << BLACKHAWK_DEVAD_SHIFT) +  \
     ((_pll_id) << BLACKHAWK_PLL_ID_SHIFT) +  \
     ((_lane) << BLACKHAWK_LANE_SHIFT) +    \
     ((_reg) << BLACKHAWK_REG_SHIFT))

#define BLACKHAWK_AER               BLACKHAWK_ADDR(0, 0, 0, 0xffde)
#define BLACKHAWK_BLK               BLACKHAWK_ADDR(0, 0, 0, 0x001f)

/*
 * The CL45 indicator is used to determine whether the upper 16 bits
 * of the address is an AER value or a clause 45 DEVAD.
 */
#define BLACKHAWK_CL45              (0x20 << 16)
#define BLACKHAWK_CL45_MASK         (0xe0 << 16)

#define BLACKHAWK_ID0                 0x600d
#define BLACKHAWK_ID1                 0x8770
#define BLACKHAWK_MODEL               0x26

#define BLACKHAWK_SIM_FW_LOAD_DONE_REG_ADDR (0x800d203)

/*RAM sim*/
#define BLACKHAWK_RAM_AHB_CTRL         (0xd202)
#define BLACKHAWK_RAM_WR_ADDR_REG_MS   (0xd205)          
#define BLACKHAWK_RAM_WR_ADDR_REG_LS   (0xd204)
#define BLACKHAWK_RAM_WR_DATA_REG_MS   (0xd207)
#define BLACKHAWK_RAM_WR_DATA_REG_LS   (0xd206)
#define BLACKHAWK_RAM_RD_ADDR_REG_MS   (0xd209)          
#define BLACKHAWK_RAM_RD_ADDR_REG_LS   (0xd208)
#define BLACKHAWK_RAM_RD_DATA_REG_MS   (0xd20b)
#define BLACKHAWK_RAM_RD_DATA_REG_LS   (0xd20a)
#define BLACKHAWK_IS_RAM_ADDR_REG(reg) (reg == BLACKHAWK_RAM_WR_ADDR_REG_MS || reg == BLACKHAWK_RAM_WR_ADDR_REG_LS || reg == BLACKHAWK_RAM_RD_ADDR_REG_MS || reg == BLACKHAWK_RAM_RD_ADDR_REG_LS)
#define BLACKHAWK_IS_RAM_DATA_REG(reg) (reg == BLACKHAWK_RAM_WR_DATA_REG_MS || reg == BLACKHAWK_RAM_WR_DATA_REG_LS || reg == BLACKHAWK_RAM_RD_DATA_REG_MS || reg == BLACKHAWK_RAM_RD_DATA_REG_LS)

#define BLACKHAWK_SIM_ENTRY_F_RAM_LS_DATA_ENTRY (0x1)
#define BLACKHAWK_SIM_ENTRY_F_RAM_MS_DATA_ENTRY (0x2)
#define BLACKHAWK_SIM_ENTRY_F_RAM_LS_ADDR_ENTRY (0x4)
#define BLACKHAWK_SIM_ENTRY_F_RAM_MS_ADDR_ENTRY (0x8)

#define BLACKHAWK_UC_TABLE_RAM_BASE  (0x100)
#define BLACKHAWK_UC_TABLE_RAM_SIZE  (56)
static uint16_t blackhawk_uc_mem_table[BLACKHAWK_UC_TABLE_RAM_SIZE] =
{
    0x6e49, 0x3666, 0x0208, 0x00d0, 0x0200, 0x3400, 0x4002, 0x0100, 0x0100, 0x2000, 0x0080, 0x2000,
    0x0,    0x0,    0x0800, 0x2000, 0x0008, 0x0,    0x0104, 0x0100, 0x0310, 0x2000, 0x1c80, 0x2000,
    0x1480, 0x2000, 0x0,    0x0,    0x0,    0x0,    0x0330, 0x2000, 0x0080, 0x0,    0x1380, 0x2000,
    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
    0x0104, 0x0000, 0x0300, 0x2000, 0x1fd0, 0x0,    0x0,    0x0

};
static int blackhawk_ram_addr;


/* Forward declarations */
STATIC int
blackhawk_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data);
STATIC int
blackhawk_sim_read(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t *data);


STATIC uint32_t
blackhawk_sim_default_data_get(uint32_t addr)
{
    uint32_t reg;

    reg = BLACKHAWK_REG_GET(addr);

    /*force FW load success */
    if(addr == BLACKHAWK_SIM_FW_LOAD_DONE_REG_ADDR) {
        return 1;
    }
    switch (reg) {
        case 0x0002:
            return 0;
        case 0x0003:
            return 0;
        case 0xd03d:
            /*
             * This is DSC_A_DSC_UC_CTRL for Falcon. Make sure
             * The 'ready' bit is set.
             */
            return 0x0080;
        case 0xd0b9:
            return 0x7;
        case 0xd0d9:
            return 0x47c0;
        case 0xd100:
            return BLACKHAWK_MODEL;
        case 0xd10a:
            return 0x803c;
        case 0xd188:
            /*
             * Reset check
             */
            return 0x7;
        case 0xd170:
            return 0xb000;
        case 0xd1c9:
            return 0x0007;
        default:
            break;
    }

    return 0;
}

STATIC int
_blackhawk_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
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
            DBG_VERB(("_blackhawk_sim_write 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                      addr, pse->data));
            return PHYMOD_E_NONE;
        }
    }

    /* Check if database is full */
    if (pms_data->entries_used >= pms_data->num_entries) {
        return PHYMOD_E_RESOURCE;
    }

    /* Check if new data matches default value */
    if (data == blackhawk_sim_default_data_get(addr)) {
        return PHYMOD_E_NONE;
    }

    /* Add new register value */
    pse = &pms_data->entries[pms_data->entries_used++];
    pse->addr = addr;
    pse->data = data;

    DBG_VERB(("_blackhawk_sim_write 0x%08"PRIx32" = 0x%04"PRIx32" (new)\n",
              addr, pse->data));

    return PHYMOD_E_NONE;
}



STATIC uint32_t
blackhawk_sim_reg_copies_get(uint32_t addr)
{
    uint32_t devad, reg;

    devad = BLACKHAWK_DEVAD_GET(addr);
    reg = BLACKHAWK_REG_GET(addr);

    if (reg == BLACKHAWK_AER || reg == BLACKHAWK_BLK) {
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
blackhawk_sim_init(phymod_sim_data_t *pms_data,
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
blackhawk_sim_reset(phymod_sim_data_t *pms_data)
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
blackhawk_sim_read(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t *data)
{
    int idx;
    uint32_t aer, blk, devad, reg, copies, pll_id=0;
    uint32_t lane = 0;
    phymod_sim_entry_t *pse;
    uint32_t pse_flag = 0;
    uint32_t addr_ms = 0, addr_ls = 0, abh_ctrl = 0;
    int increase_addr_auto = 0;

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    devad = 0;


    if (addr < BLACKHAWK_BLK) {
        /* Assume clause 22 access */
        (void)blackhawk_sim_read(pms_data, BLACKHAWK_BLK, &blk);
        if (addr & 0x10) {
            /* IEEE bit */
            blk |= 0x8000;
        }
        addr = (addr & 0xf) | (blk & 0xfff0);
        if (addr != BLACKHAWK_AER && addr != BLACKHAWK_BLK) {
            (void)blackhawk_sim_read(pms_data, BLACKHAWK_AER, &aer);
            addr |= (aer << 16);
        }
    } else {
        /* Extract devad if clause 45 address format */
        if ((addr & BLACKHAWK_CL45_MASK) == BLACKHAWK_CL45) {
            devad = (addr >> 16) & 0x1f;
            addr &= 0xffff;
        }
    }

    if (addr != BLACKHAWK_AER && addr != BLACKHAWK_BLK) {
        /* Assume AER is in upper 16 bits */
        aer = (addr >> 16);
        if (aer == 0) {
            /* Try reading real AER instead */
            (void)blackhawk_sim_read(pms_data, BLACKHAWK_AER, &aer);
        }
        /* Add clause 45 devad (if used) */
        aer |= (devad << 11);
        addr = (addr & 0xffff) | (aer << 16);

        lane = (aer & 0x7);
    }

    /* Adjust lane according to number of copies */
    devad = BLACKHAWK_DEVAD_GET(addr);
    reg = BLACKHAWK_REG_GET(addr);
    copies = blackhawk_sim_reg_copies_get(addr);
    if (copies == 1) {
        lane = 0;
    } else if (copies == 2) {
        pll_id = BLACKHAWK_PLL_ID_GET(addr);
        lane = (pll_id == 0) ? 0 : 1;
    }
    
    /*handle ram read/write*/
    if (BLACKHAWK_IS_RAM_DATA_REG(reg) || BLACKHAWK_IS_RAM_ADDR_REG(reg)) {

        if (BLACKHAWK_IS_RAM_DATA_REG(reg)) {
            (void)blackhawk_sim_read(pms_data, BLACKHAWK_ADDR(devad, pll_id, lane, BLACKHAWK_RAM_AHB_CTRL), &abh_ctrl);
            increase_addr_auto = ((abh_ctrl >> 13) & 0x1);

            (void)blackhawk_sim_read(pms_data, BLACKHAWK_ADDR(devad, pll_id, lane, BLACKHAWK_RAM_RD_ADDR_REG_LS), &addr_ls);
            (void)blackhawk_sim_read(pms_data, BLACKHAWK_ADDR(devad, pll_id, lane, BLACKHAWK_RAM_RD_ADDR_REG_MS), &addr_ms);
            addr = addr_ls | (addr_ms << 16);
            if (blackhawk_ram_addr == 0 ) {
                blackhawk_ram_addr = addr;
            } else {
                if (increase_addr_auto) {
                    blackhawk_ram_addr += 1;
                } else {
                    blackhawk_ram_addr = 0;
                }
            }

            if (reg == BLACKHAWK_RAM_RD_DATA_REG_LS || reg == BLACKHAWK_RAM_WR_DATA_REG_LS) {
                pse_flag |= BLACKHAWK_SIM_ENTRY_F_RAM_LS_DATA_ENTRY;
            } else {
                pse_flag |= BLACKHAWK_SIM_ENTRY_F_RAM_MS_DATA_ENTRY;
            }
        } else { /*BLACKHAWK_IS_RAM_ADDR_REG*/
            addr = BLACKHAWK_ADDR(devad, 0, lane, 0 /*dummy*/);
            if (reg == BLACKHAWK_RAM_RD_ADDR_REG_LS || reg == BLACKHAWK_RAM_WR_ADDR_REG_LS) {
                pse_flag |= BLACKHAWK_SIM_ENTRY_F_RAM_LS_ADDR_ENTRY;
            } else {
                pse_flag |= BLACKHAWK_SIM_ENTRY_F_RAM_MS_ADDR_ENTRY;
            }
        }
    
        /* Check if this register has been written already */
        for (idx = 0; idx < pms_data->entries_used; idx++) {
            pse = &pms_data->entries[idx];
            if ((pse->addr == addr) && (pse->flags == pse_flag)) {
                *data = pse->data;
                DBG_VERB(("blackhawk_sim_read 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                          addr, *data));
                return PHYMOD_E_NONE;
            }
        }

        if((blackhawk_ram_addr >= BLACKHAWK_UC_TABLE_RAM_BASE) && (blackhawk_ram_addr < (BLACKHAWK_UC_TABLE_RAM_BASE + BLACKHAWK_UC_TABLE_RAM_SIZE))) {
            *data = blackhawk_uc_mem_table[blackhawk_ram_addr - BLACKHAWK_UC_TABLE_RAM_BASE];
        }else {
            *data = 0;
        }

        return PHYMOD_E_NONE;
    }



    addr = BLACKHAWK_ADDR(devad, pll_id, lane, reg);

    /* Check if this register has been written already */
    for (idx = 0; idx < pms_data->entries_used; idx++) {
        pse = &pms_data->entries[idx];
        if ((pse->addr == addr) && (pse->flags == pse_flag)) {
            *data = pse->data;
            DBG_VERB(("blackhawk_sim_read 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                      addr, *data));
            return PHYMOD_E_NONE;
        }
    }

    /* Return default value if register was never written */
    *data = blackhawk_sim_default_data_get(addr);

    DBG_VERB(("blackhawk_sim_read 0x%08"PRIx32" = [0x%04"PRIx32"]\n",
              addr, *data));

    return PHYMOD_E_NONE;
}

STATIC int
blackhawk_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
{
    int idx;
    uint32_t aer, blk, devad, reg, copies, pll_id=0;
    uint32_t lane = 0;
    phymod_sim_entry_t *pse;
    uint32_t pse_flag = 0;
    uint32_t addr_ms = 0, addr_ls = 0;

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    devad = 0;

    if (addr < BLACKHAWK_BLK) {
        /* Assume clause 22 access */
        (void)blackhawk_sim_read(pms_data, BLACKHAWK_BLK, &blk);
        /* IEEE bit */
        if (addr & 0x10) {
            blk |= 0x8000;
        } else {
            blk &= ~0x8000;
        }
        addr = (addr & 0xf) | (blk & 0xfff0);
        if (addr != BLACKHAWK_AER && addr != BLACKHAWK_BLK) {
            (void)blackhawk_sim_read(pms_data, BLACKHAWK_AER, &aer);
            addr |= (aer << 16);
        }
    } else {
        /* Extract devad if clause 45 address format */
        if ((addr & BLACKHAWK_CL45_MASK) == BLACKHAWK_CL45) {
            devad = (addr >> 16) & 0x1f;
            addr &= 0xffff;
        }
    }

    if (addr != BLACKHAWK_AER && addr != BLACKHAWK_BLK) {
        /* Assume AER is in upper 16 bits */
        aer = (addr >> 16);
        if (aer == 0) {
            /* Try reading real AER instead */
            (void)blackhawk_sim_read(pms_data, BLACKHAWK_AER, &aer);
        }

        /* Add clause 45 devad (if used) */
        aer |= (devad << 11);
        addr = (addr & 0xffff) | (aer << 16);

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
            reg = BLACKHAWK_REG_GET(addr);
            devad = BLACKHAWK_DEVAD_GET(addr);
            pll_id = BLACKHAWK_PLL_ID_GET(addr);
            if (lane == 0x20) {
                /* Write lanes 0 and 1 */
                addr = BLACKHAWK_ADDR(devad, pll_id, 0, reg);
                (void)blackhawk_sim_write(pms_data, addr, data);
                addr = BLACKHAWK_ADDR(devad, pll_id, 1, reg);
                (void)blackhawk_sim_write(pms_data, addr, data);
            }
            if (lane == 0x21) {
                /* Write lanes 2 and 3 */
                addr = BLACKHAWK_ADDR(devad, pll_id, 2, reg);
                (void)blackhawk_sim_write(pms_data, addr, data);
                addr = BLACKHAWK_ADDR(devad, pll_id, 3, reg);
                (void)blackhawk_sim_write(pms_data, addr, data);
            }
            if (lane == 0x22) {
                /* Write lanes 4 and 5 */
                addr = BLACKHAWK_ADDR(devad, pll_id, 4, reg);
                (void)blackhawk_sim_write(pms_data, addr, data);
                addr = BLACKHAWK_ADDR(devad, pll_id, 5, reg);
                (void)blackhawk_sim_write(pms_data, addr, data);
            }
            if (lane == 0x23) {
                /* Write lanes 6 and 7 */
                addr = BLACKHAWK_ADDR(devad, pll_id, 6, reg);
                (void)blackhawk_sim_write(pms_data, addr, data);
                addr = BLACKHAWK_ADDR(devad, pll_id, 7, reg);
                (void)blackhawk_sim_write(pms_data, addr, data);
            }
            if (lane == 0x40) {
                /* Write lanes 0,1,2,and 3 */
                addr = BLACKHAWK_ADDR(devad, pll_id, 0, reg);
                (void)blackhawk_sim_write(pms_data, addr, data);
                addr = BLACKHAWK_ADDR(devad, pll_id, 1, reg);
                (void)blackhawk_sim_write(pms_data, addr, data);
                addr = BLACKHAWK_ADDR(devad, pll_id, 2, reg);
                (void)blackhawk_sim_write(pms_data, addr, data);
                addr = BLACKHAWK_ADDR(devad, pll_id, 3, reg);
                (void)blackhawk_sim_write(pms_data, addr, data);
            }
            if (lane == 0x41) {
                /* Write lanes 4,5,6 and 7 */
                addr = BLACKHAWK_ADDR(devad, pll_id, 4, reg);
                (void)blackhawk_sim_write(pms_data, addr, data);
                addr = BLACKHAWK_ADDR(devad, pll_id, 5, reg);
                (void)blackhawk_sim_write(pms_data, addr, data);
                addr = BLACKHAWK_ADDR(devad, pll_id, 6, reg);
                (void)blackhawk_sim_write(pms_data, addr, data);
                addr = BLACKHAWK_ADDR(devad, pll_id, 7, reg);
                (void)blackhawk_sim_write(pms_data, addr, data);
            }
            if (lane == 0xff) {
                /* Write lanes 2 and 3 */
                addr = BLACKHAWK_ADDR(devad, pll_id, 0, reg);
                (void)blackhawk_sim_write(pms_data, addr, data);
                addr = BLACKHAWK_ADDR(devad, pll_id, 1, reg);
                (void)blackhawk_sim_write(pms_data, addr, data);
                addr = BLACKHAWK_ADDR(devad, pll_id, 2, reg);
                (void)blackhawk_sim_write(pms_data, addr, data);
                addr = BLACKHAWK_ADDR(devad, pll_id, 3, reg);
                (void)blackhawk_sim_write(pms_data, addr, data);
                addr = BLACKHAWK_ADDR(devad, pll_id, 4, reg);
                (void)blackhawk_sim_write(pms_data, addr, data);
                addr = BLACKHAWK_ADDR(devad, pll_id, 5, reg);
                (void)blackhawk_sim_write(pms_data, addr, data);
                addr = BLACKHAWK_ADDR(devad, pll_id, 6, reg);
                (void)blackhawk_sim_write(pms_data, addr, data);
                addr = BLACKHAWK_ADDR(devad, pll_id, 7, reg);
                (void)blackhawk_sim_write(pms_data, addr, data);
            }
            return PHYMOD_E_NONE;
        }
    }

    /* Adjust lane according to number of copies */
    devad = BLACKHAWK_DEVAD_GET(addr);
    reg = BLACKHAWK_REG_GET(addr);
    copies = blackhawk_sim_reg_copies_get(addr);
    if (copies == 1) {
        lane = 0;
    } else if (copies == 2) {
        pll_id = BLACKHAWK_PLL_ID_GET(addr);
        lane = (pll_id == 0) ? 0 : 1;
    }

    /*handle ram read/write*/
    if (BLACKHAWK_IS_RAM_DATA_REG(reg) || BLACKHAWK_IS_RAM_ADDR_REG(reg)) {

        if (BLACKHAWK_IS_RAM_DATA_REG(reg)) {

            (void)blackhawk_sim_read(pms_data, BLACKHAWK_ADDR(devad, pll_id, lane, BLACKHAWK_RAM_RD_ADDR_REG_LS), &addr_ls);
            (void)blackhawk_sim_read(pms_data, BLACKHAWK_ADDR(devad, pll_id, lane, BLACKHAWK_RAM_RD_ADDR_REG_MS), &addr_ms);
            addr = addr_ls | (addr_ms << 16);
            if (reg == BLACKHAWK_RAM_RD_DATA_REG_LS || reg == BLACKHAWK_RAM_WR_DATA_REG_LS) {
                pse_flag |= BLACKHAWK_SIM_ENTRY_F_RAM_LS_DATA_ENTRY;
            } else {
                pse_flag |= BLACKHAWK_SIM_ENTRY_F_RAM_MS_DATA_ENTRY;
            }
        } else { /*BLACKHAWK_IS_RAM_ADDR_REG*/
            addr = BLACKHAWK_ADDR(devad, 0, lane, 0 /*dummy*/);
            if (reg == BLACKHAWK_RAM_RD_ADDR_REG_LS || reg == BLACKHAWK_RAM_WR_ADDR_REG_LS) {
                pse_flag |= BLACKHAWK_SIM_ENTRY_F_RAM_LS_ADDR_ENTRY;
            } else {
                pse_flag |= BLACKHAWK_SIM_ENTRY_F_RAM_MS_ADDR_ENTRY;
            }
        }

        /* Check if this register has been written already */
        for (idx = 0; idx < pms_data->entries_used; idx++) {
            pse = &pms_data->entries[idx];
            if ((pse->addr == addr) && (pse->flags == pse_flag)) {
                pse->data = data;
                DBG_VERB(("blackhawk_sim_write 0x%08"PRIx32" = 0x%04"PRIx32" - flag = %u \n",
                          addr, pse->data, pse_flag));
                return PHYMOD_E_NONE;
            }
        }
        pse = &pms_data->entries[pms_data->entries_used++];
        pse->addr = addr;
        pse->data = data;
        pse->flags = pse_flag;

        DBG_VERB(("blackhawk_sim_write 0x%08"PRIx32" = 0x%04"PRIx32"  - flag = %u (new)\n",
              addr, pse->data, pse->flags));

        return PHYMOD_E_NONE;
    }

    addr = BLACKHAWK_ADDR(devad, pll_id, lane, reg);

    return _blackhawk_sim_write(pms_data, addr, data);
}

STATIC int                                                        
blackhawk_sim_event(phymod_sim_data_t *pms_data, phymod_sim_event_t event)
{
    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    return PHYMOD_E_NONE;
}

phymod_sim_drv_t blackhawk_sim_drv = {
    blackhawk_sim_init,
    blackhawk_sim_reset,
    blackhawk_sim_read,
    blackhawk_sim_write,
    blackhawk_sim_event
};

#endif /* EXCLUDE_PHYMOD_BLACKHAWK_SIM */
