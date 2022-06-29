/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * This software simulator can emulate basic register access for the
 * TSC/D5 SerDes PHY.
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
 *
 */

#ifndef EXCLUDE_PHYMOD_TSCD_SIM

#include <phymod/phymod_system.h>
#include <phymod/phymod_sim.h>

/* Convenience macro */
#define DBG_VERB PHYMOD_DEBUG_VERBOSE

/* Bit field get/set macros */
#define TSCD_BF_SET(_val, _mask, _shift) _val |= ((_mask) << (_shift))
#define TSCD_BF_GET(_val, _mask, _shift) (((_val) >> (_shift)) & (_mask))

/*
 * Raw 32-bit address consists of AER value in upper 16 bits and
 * clause 45 address in lower 16 bits.
 */
#define TSCD_DEVAD_SHIFT       27
#define TSCD_DEVAD_MASK        0x1f
#define TSCD_DEVAD_GET(_addr) \
    TSCD_BF_GET(_addr, TSCD_DEVAD_MASK, TSCD_DEVAD_SHIFT)
#define TSCD_LANE_SHIFT        16
#define TSCD_LANE_MASK         0x7
#define TSCD_LANE_GET(_addr) \
    TSCD_BF_GET(_addr, TSCD_LANE_MASK, TSCD_LANE_SHIFT)
#define TSCD_REG_SHIFT         0
#define TSCD_REG_MASK          0xffff
#define TSCD_REG_GET(_addr) \
    TSCD_BF_GET(_addr, TSCD_REG_MASK, TSCD_REG_SHIFT)

#define TSCD_ADDR(_devad, _lane, _reg) \
    (((_devad) << TSCD_DEVAD_SHIFT) +  \
     ((_lane) << TSCD_LANE_SHIFT) +    \
     ((_reg) << TSCD_REG_SHIFT))

#define TSCD_AER               TSCD_ADDR(0, 0, 0xffde)
#define TSCD_BLK               TSCD_ADDR(0, 0, 0x001f)

/*
 * The CL45 indicator is used to determine whether the upper 16 bits
 * of the address is an AER value or a clause 45 DEVAD.
 */
#define TSCD_CL45              (0x20 << 16)
#define TSCD_CL45_MASK         (0xe0 << 16)

#define TSCD_ID0                 0x600d
#define TSCD_ID1                 0x8770
#define TSCD_SERDES_ID           0x03AE

#define TSCD_SIM_FW_LOAD_DONE_REG_ADDR (0x800d203)

/* Forward declarations */
STATIC int
tscd_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data);
STATIC int
tscd_sim_read(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t *data);

STATIC uint32_t
tscd_sim_default_data_get(uint32_t addr)
{
    uint32_t reg;

    reg = TSCD_REG_GET(addr);

    /*force FW load success */
    if(addr == TSCD_SIM_FW_LOAD_DONE_REG_ADDR) {
        return 1;
    }

    switch (reg) {
        case 0x0002:
            return TSCD_ID0;
        case 0x0003:
            return TSCD_ID1;
        case 0x900e:
            return TSCD_SERDES_ID;
        default:
            break;
    }

    return 0;
}
STATIC uint32_t
tscd_sim_reg_copies_get(uint32_t addr)
{
    uint32_t devad, reg;

    devad = TSCD_DEVAD_GET(addr);
    reg = TSCD_REG_GET(addr);

    if (reg == TSCD_AER || reg == TSCD_BLK) {
        return 1;
    }

    if (devad == 0) {
        if ((reg & 0xf000) == 0x9000) {
            return 1;
        }
        if ((reg & 0xf000) == 0xa000) {
            return 2;
        }
        return 4;
    } else if (devad == 1) {
        return 4;
    }
    return 0;
}

STATIC int
_tscd_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
{
    int idx;
    uint32_t mask;
    uint32_t devad;
    phymod_sim_entry_t *pse;

    devad = TSCD_DEVAD_GET(addr);

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    /*
     * On PM4x25D,
     * PCS register address is 16-bit
     *     register data width is 16-bit
     * PMD register address is 16-bit
     *     register data width is 32-bit
     *
     * PCS supports Register Modify operations.
     * PMD does not support register modify
     * operation.
     *
     * For PCS register writes, the function argument
     * data is encoded with the value to be written
     * along with the mask, both are 16-bits wide.
     *
     * For PMD register writes, data is not encoded
     * and mask and value extraction not needed. 
     */

    /* Determine PCS or PMD write. */

    if (devad) {
        /* Check if this register has been written already */
        for (idx = 0; idx < pms_data->entries_used; idx++) {
            pse = &pms_data->entries[idx];
            if (pse->addr == addr) {
                pse->data = data;
                DBG_VERB(("PMD TSCD_sim_write 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                          addr, pse->data));
                return PHYMOD_E_NONE;
            }
        }
    } else {
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
                DBG_VERB(("PCS TSCD_sim_write 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                          addr, pse->data));
                return PHYMOD_E_NONE;
            }
        }
    }

    /* Check if database is full */
    if (pms_data->entries_used >= pms_data->num_entries) {
        return PHYMOD_E_RESOURCE;
    }

    /* Check if new data matches default value */
    if (data == tscd_sim_default_data_get(addr)) {
        return PHYMOD_E_NONE;
    }

    /* Add new register value */
    pse = &pms_data->entries[pms_data->entries_used++];
    pse->addr = addr;
    pse->data = data;

    DBG_VERB(("_TSCD_sim_write 0x%08"PRIx32" = 0x%04"PRIx32" (new)\n",
              addr, pse->data));

    return PHYMOD_E_NONE;
}

STATIC uint32_t
tscd_sim_write_adjust(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
{
    uint32_t devad, reg, val;
    uint32_t mask;

    devad = TSCD_DEVAD_GET(addr);
    reg = TSCD_REG_GET(addr);

    /* Support optional write mask in upper 16 bits */
    mask = (data >> 16);
    if (mask == 0) {
        mask = 0xffff;
    }

    if (devad == 0) {
        switch (reg) {
        case 0xc050:
            /* Set SW_SPEED_CHANGE_DONE and SW_SPEED_CONFIG_VLD in status reg */
            tscd_sim_write(pms_data, addr + 1, 0x3);
            /* Force link up in RX_X4_PCS_LIVE_STS register
               offset 0x111 = 0xc161 - 0xc050 */
            tscd_sim_write(pms_data, addr + 0x111, 0x2);
            /* Sync configured speed into final speed status register
             * 0x20 = 0xc070 - 0xc050
             * Only when speed_id (data mask includes 0xff) is updated in 0xc050*/
            if ((mask & 0xff) == 0xff) {
                tscd_sim_read(pms_data, addr + 0x20, &val);
                val = (val & 0xffff00ff) | ((data & 0xff)<<8);
                tscd_sim_write(pms_data, addr + 0x20, val);
            }
            break;
        case 0xc055:
            /* Sync configured Lane Number into Lane Number status register
             * 0x1B = 0xc070 - 0xc055 */
            tscd_sim_read(pms_data, addr + 0x1B, &val);
            val = (val & 0xfffffff8) | (data & 0x7);
            tscd_sim_write(pms_data, addr + 0x1B, val);
            break;
        default:
            break;
        }
    } else if (devad == 1) {
        switch (reg) {
        case 0xd204:
        case 0xd205:
        case 0xd206:
        case 0xd207:
            /* Sync configured aMICRO_A_COM registers. Write and read access different register
             * Write registers: 0xd204~0xd207; Read registers: 0xd208~0xd20b
             * 0x4 = 0xd208(~0xd20b) - 0xd204(~0xd207)*/
            tscd_sim_write(pms_data, addr + 0x4, data);
            break;
        case 0xd0e1:
            /* Sync configured SIGDET_CTL1 register. When a port is set disable,
             * the port's RX_X4_PCS_LIVE_STS1 register deletes link status.
             * RX_X4_PCS_LIVE_STS1 register is PCS register, so need set devad to 0.
             * 0xF80 = 0xd0e1 - 0xc161*/
            tscd_sim_read(pms_data, (addr - 0xF80) & 0x7ffff, &val);
            val = (val & 0xfffd) | (((data&0x80)==0x80)?0:0x2);
            tscd_sim_write(pms_data, (addr - 0xF80) & 0x7ffff, val);
            break;
        case 0xd147:
            /* Sync configured all lanes' PLL_CAL_CTL7 register. Only one PLL/VCO per core */
            /* Write lanes 0, 1, 2 and 3 */
            addr = TSCD_ADDR(devad, 0, reg);
            (void)_tscd_sim_write(pms_data, addr, data);

            addr = TSCD_ADDR(devad, 1, reg);
            (void)_tscd_sim_write(pms_data, addr, data);

            addr = TSCD_ADDR(devad, 2, reg);
            (void)_tscd_sim_write(pms_data, addr, data);

            addr = TSCD_ADDR(devad, 3, reg);
            (void)_tscd_sim_write(pms_data, addr, data);
            break;
        case 0xd094:
        case 0xd095:
            /* Sync configured CL93N72 Control 2/3 registers. Write and read access different register
             * Write registers: 0xd094/d095; Read registers: 0xd133/0xd134
             * 0x9f = 0xd133(0xd134) - 0xd094(0xd095)*/
            tscd_sim_write(pms_data, addr + 0x9f, data);
            break;
        default:
            break;
        }
    }

    return data;
}

STATIC int
tscd_sim_init(phymod_sim_data_t *pms_data,
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
tscd_sim_reset(phymod_sim_data_t *pms_data)
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
tscd_sim_read(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t *data)
{
    int idx;
    uint32_t aer, blk, devad, reg, copies;
    uint32_t lane = 0;
    phymod_sim_entry_t *pse;

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    devad = 0;

    if (addr < TSCD_BLK) {
        /* Assume clause 22 access */
        (void)tscd_sim_read(pms_data, TSCD_BLK, &blk);
        /* IEEE bit */
        if (addr & 0x10) {
            blk |= 0x8000;
        } else {
            blk &= ~0x8000;
        }
        addr = (addr & 0xf) | (blk & 0xfff0);
        if (addr != TSCD_AER && addr != TSCD_BLK) {
            (void)tscd_sim_read(pms_data, TSCD_AER, &aer);
            addr |= (aer << 16);
        }
    } else {
        /* Extract devad if clause 45 address format */
        if ((addr & TSCD_CL45_MASK) == TSCD_CL45) {
            devad = (addr >> 16) & 0x1f;
            addr &= 0xffff;
        }
    }

    if (addr != TSCD_AER && addr != TSCD_BLK) {
        /* Assume AER is in upper 16 bits */
        aer = (addr >> 16);
        if (aer == 0) {
            /* Try reading real AER instead */
            (void)tscd_sim_read(pms_data, TSCD_AER, &aer);
        }
        /* Add clause 45 devad (if used) */
        if (devad) {
            aer |= (devad << 11);
            addr = (addr & 0xffff) | (aer << 16);
        }
        lane = (aer & 0x7);
        if (lane > 3) {
            /* Force lane 0 if lane is invalid */
            addr = TSCD_ADDR(TSCD_DEVAD_GET(addr), 0, TSCD_REG_GET(addr));
        }
    }

    /* Adjust lane according to number of copies */
    devad = TSCD_DEVAD_GET(addr);
    reg = TSCD_REG_GET(addr);
    copies = tscd_sim_reg_copies_get(addr);
    if (copies == 1) {
        lane = 0;
    } else if (copies == 2) {
        lane &= ~0x1;
    }
    addr = TSCD_ADDR(devad, lane, reg);

    /* Check if this register has been written already */
    for (idx = 0; idx < pms_data->entries_used; idx++) {
        pse = &pms_data->entries[idx];
        if (pse->addr == addr) {
            *data = pse->data;
            DBG_VERB(("tscd_sim_read 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                      addr, *data));
            return PHYMOD_E_NONE;
        }
    }

    /* Return default value if register was never written */
    *data = tscd_sim_default_data_get(addr);

    DBG_VERB(("tscd_sim_read 0x%08"PRIx32" = [0x%04"PRIx32"]\n",
              addr, *data));

    return PHYMOD_E_NONE;
}

STATIC int
tscd_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
{
    uint32_t aer, blk, devad, reg, copies;
    uint32_t lane = 0;

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    devad = 0;

    if (addr < TSCD_BLK) {
        /* Assume clause 22 access */
        (void)tscd_sim_read(pms_data, TSCD_BLK, &blk);
        /* IEEE bit */
        if (addr & 0x10) {
            blk |= 0x8000;
        } else {
            blk &= ~0x8000;
        }
        addr = (addr & 0xf) | (blk & 0xfff0);
        if (addr != TSCD_AER && addr != TSCD_BLK) {
            (void)tscd_sim_read(pms_data, TSCD_AER, &aer);
            addr |= (aer << 16);
        }
    } else {
        /* Extract devad if clause 45 address format */
        if ((addr & TSCD_CL45_MASK) == TSCD_CL45) {
            devad = (addr >> 16) & 0x1f;
            addr &= 0xffff;
        }
    }

    if (addr != TSCD_AER && addr != TSCD_BLK) {
        /* Assume AER is in upper 16 bits */
        aer = (addr >> 16);
        if (aer == 0) {
            /* Try reading real AER instead */
            (void)tscd_sim_read(pms_data, TSCD_AER, &aer);
        }
        /* Add clause 45 devad (if used) */
        if (devad) {
            aer |= (devad << 11);
            addr = (addr & 0xffff) | (aer << 16);
        }
        lane = (aer & 0x7);
        if (lane > 6) {
            return PHYMOD_E_PARAM;
        }
        if (lane > 3) {
            /*
             * Handle lane broadcast
             *
             * Note that we use lane 8 instead of lane 0 to prevent a
             * broadcast loop. The value 8 will become 0 when masked
             * with 0x7, but it prevents the AER in the upper 16 bits
             * from being zero, which will cause the code above to
             * obtain the AER value from register 0xffde.
             */
            reg = TSCD_REG_GET(addr);
            devad = TSCD_DEVAD_GET(addr);
            if (lane == 4 || lane == 6) {
                /* Write lanes 0 and 1 */
                addr = TSCD_ADDR(devad, 8, reg);
                (void)tscd_sim_write(pms_data, addr, data);
                addr = TSCD_ADDR(devad, 1, reg);
                (void)tscd_sim_write(pms_data, addr, data);
            }
            if (lane == 5 || lane == 6) {
                /* Write lanes 2 and 3 */
                addr = TSCD_ADDR(devad, 2, reg);
                (void)tscd_sim_write(pms_data, addr, data);
                addr = TSCD_ADDR(devad, 3, reg);
                (void)tscd_sim_write(pms_data, addr, data);
            }
            return PHYMOD_E_NONE;
        }
    }

    /* Adjust data and/or related registers */
    data = tscd_sim_write_adjust(pms_data, addr, data);

    /* Adjust lane according to number of copies */
    devad = TSCD_DEVAD_GET(addr);
    reg = TSCD_REG_GET(addr);
    copies = tscd_sim_reg_copies_get(addr);
    if (copies == 1) {
        lane = 0;
    } else if (copies == 2) {
        lane &= ~0x1;
    }
    addr = TSCD_ADDR(devad, lane, reg);

    return _tscd_sim_write(pms_data, addr, data);
}

STATIC int
tscd_sim_read32(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t *data)
{
    int idx;
    uint32_t aer, blk, devad, reg, copies;
    uint32_t lane = 0;
    phymod_sim_entry_t *pse;

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    devad = 0;

    if (addr < TSCD_BLK) {
        /* Assume clause 22 access */
        (void)tscd_sim_read(pms_data, TSCD_BLK, &blk);
        /* IEEE bit */
        if (addr & 0x10) {
            blk |= 0x8000;
        } else {
            blk &= ~0x8000;
        }
        addr = (addr & 0xf) | (blk & 0xfff0);
        if (addr != TSCD_AER && addr != TSCD_BLK) {
            (void)tscd_sim_read32(pms_data, TSCD_AER, &aer);
            addr |= (aer << 16);
        }
    } else {
        /* Extract devad if clause 45 address format */
        if ((addr & TSCD_CL45_MASK) == TSCD_CL45) {
            devad = (addr >> 16) & 0x1f;
            addr &= 0xffff;
        }
    }

    if (addr != TSCD_AER && addr != TSCD_BLK) {
        /* Assume AER is in upper 16 bits */
        aer = (addr >> 16);
        if (aer == 0) {
            /* Try reading real AER instead */
            (void)tscd_sim_read32(pms_data, TSCD_AER, &aer);
        }
        /* Add clause 45 devad (if used) */
        if (devad) {
            aer |= (devad << 11);
            addr = (addr & 0xffff) | (aer << 16);
        }
        lane = (aer & 0x7);
        if (lane > 3) {
            /* Force lane 0 if lane is invalid */
            addr = TSCD_ADDR(TSCD_DEVAD_GET(addr), 0, TSCD_REG_GET(addr));
        }
    }

    /* Adjust lane according to number of copies */
    devad = TSCD_DEVAD_GET(addr);
    reg = TSCD_REG_GET(addr);
    copies = tscd_sim_reg_copies_get(addr);
    if (copies == 1) {
        lane = 0;
    } else if (copies == 2) {
        lane &= ~0x1;
    }
    addr = TSCD_ADDR(devad, lane, reg);

    /* Check if this register has been written already */
    for (idx = 0; idx < pms_data->entries_used; idx++) {
        pse = &pms_data->entries[idx];
        if (pse->addr == addr) {
            *data = pse->data;
            DBG_VERB(("tscd_sim_read32 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                      addr, *data));
            return PHYMOD_E_NONE;
        }
    }

    /* Return default value if register was never written */
    *data = tscd_sim_default_data_get(addr);

    DBG_VERB(("tscd_sim_read32 0x%08"PRIx32" = [0x%04"PRIx32"]\n",
              addr, *data));

    return PHYMOD_E_NONE;
}

STATIC int
tscd_sim_write32(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
{
    uint32_t aer, blk, devad, reg, copies;
    uint32_t lane = 0;

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    devad = 0;

    if (addr < TSCD_BLK) {
        /* Assume clause 22 access */
        (void)tscd_sim_read32(pms_data, TSCD_BLK, &blk);
        /* IEEE bit */
        if (addr & 0x10) {
            blk |= 0x8000;
        } else {
            blk &= ~0x8000;
        }
        addr = (addr & 0xf) | (blk & 0xfff0);
        if (addr != TSCD_AER && addr != TSCD_BLK) {
            (void)tscd_sim_read32(pms_data, TSCD_AER, &aer);
            addr |= (aer << 16);
        }
    } else {
        /* Extract devad if clause 45 address format */
        if ((addr & TSCD_CL45_MASK) == TSCD_CL45) {
            devad = (addr >> 16) & 0x1f;
            addr &= 0xffff;
        }
    }

    if (addr != TSCD_AER && addr != TSCD_BLK) {
        /* Assume AER is in upper 16 bits */
        aer = (addr >> 16);
        if (aer == 0) {
            /* Try reading real AER instead */
            (void)tscd_sim_read32(pms_data, TSCD_AER, &aer);
        }
        /* Add clause 45 devad (if used) */
        if (devad) {
            aer |= (devad << 11);
            addr = (addr & 0xffff) | (aer << 16);
        }
        lane = (aer & 0x7);
        if (lane > 6) {
            return PHYMOD_E_PARAM;
        }
        if (lane > 3) {
            /*
             * Handle lane broadcast
             *
             * Note that we use lane 8 instead of lane 0 to prevent a
             * broadcast loop. The value 8 will become 0 when masked
             * with 0x7, but it prevents the AER in the upper 16 bits
             * from being zero, which will cause the code above to
             * obtain the AER value from register 0xffde.
             */
            reg = TSCD_REG_GET(addr);
            devad = TSCD_DEVAD_GET(addr);
            if (lane == 4 || lane == 6) {
                /* Write lanes 0 and 1 */
                addr = TSCD_ADDR(devad, 8, reg);
                (void)tscd_sim_write32(pms_data, addr, data);
                addr = TSCD_ADDR(devad, 1, reg);
                (void)tscd_sim_write32(pms_data, addr, data);
            }
            if (lane == 5 || lane == 6) {
                /* Write lanes 2 and 3 */
                addr = TSCD_ADDR(devad, 2, reg);
                (void)tscd_sim_write32(pms_data, addr, data);
                addr = TSCD_ADDR(devad, 3, reg);
                (void)tscd_sim_write(pms_data, addr, data);
            }
            return PHYMOD_E_NONE;
        }
    }

    /* Adjust data and/or related registers */
    data = tscd_sim_write_adjust(pms_data, addr, data);

    /* Adjust lane according to number of copies */
    devad = TSCD_DEVAD_GET(addr);
    reg = TSCD_REG_GET(addr);
    copies = tscd_sim_reg_copies_get(addr);
    if (copies == 1) {
        lane = 0;
    } else if (copies == 2) {
        lane &= ~0x1;
    }
    addr = TSCD_ADDR(devad, lane, reg);

    return _tscd_sim_write(pms_data, addr, data);
}

STATIC int
tscd_sim_event(phymod_sim_data_t *pms_data, phymod_sim_event_t event)
{
    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    return PHYMOD_E_NONE;
}

phymod_sim_drv_t tscd_sim_drv = {
    tscd_sim_init,
    tscd_sim_reset,
    tscd_sim_read,
    tscd_sim_write,
    tscd_sim_read32,
    tscd_sim_write32,
    tscd_sim_event
};

#endif /* EXCLUDE_PHYMOD_TSCD_SIM */
