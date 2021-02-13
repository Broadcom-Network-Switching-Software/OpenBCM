/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 *
 * This software simulator can emulate basic register access for the
 * TSC/Merlin7 SerDes PHY.
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

#ifndef EXCLUDE_PHYMOD_TSCE7_SIM

#include <phymod/phymod_system.h>
#include <phymod/phymod_sim.h>
#include <phymod/chip/bcmi_tsce7_xgxs_defs.h>

/* Convenience macro */
#define DBG_VERB PHYMOD_DEBUG_VERBOSE

/* Bit field get/set macros */
#define TSCE7_BF_SET(_val, _mask, _shift) _val |= ((_mask) << (_shift))
#define TSCE7_BF_GET(_val, _mask, _shift) (((_val) >> (_shift)) & (_mask))

/*
 * Raw 32-bit address consists of AER value in upper 16 bits and
 * clause 45 address in lower 16 bits.
 */
#define TSCE7_DEVAD_SHIFT       27
#define TSCE7_DEVAD_MASK        0x1f
#define TSCE7_DEVAD_GET(_addr) \
    TSCE7_BF_GET(_addr, TSCE7_DEVAD_MASK, TSCE7_DEVAD_SHIFT)
#define TSCE7_LANE_SHIFT        16
#define TSCE7_LANE_MASK         0x7
#define TSCE7_LANE_GET(_addr) \
    TSCE7_BF_GET(_addr, TSCE7_LANE_MASK, TSCE7_LANE_SHIFT)
#define TSCE7_REG_SHIFT         0
#define TSCE7_REG_MASK          0xffff
#define TSCE7_REG_GET(_addr) \
    TSCE7_BF_GET(_addr, TSCE7_REG_MASK, TSCE7_REG_SHIFT)

#define TSCE7_ADDR(_devad, _lane, _reg) \
    (((_devad) << TSCE7_DEVAD_SHIFT) +  \
     ((_lane) << TSCE7_LANE_SHIFT) +    \
     ((_reg) << TSCE7_REG_SHIFT))

#define TSCE7_AER               TSCE7_ADDR(0, 0, 0xffde)
#define TSCE7_BLK               TSCE7_ADDR(0, 0, 0x001f)

/*
 * The CL45 indicator is used to determine whether the upper 16 bits
 * of the address is an AER value or a clause 45 DEVAD.
 */
#define TSCE7_CL45              (0x20 << 16)
#define TSCE7_CL45_MASK         (0xe0 << 16)

#define TSCE7_ID0               0x600d
#define TSCE7_ID1               0x8770
#define TSCE7_SERDES_ID         0x0352

#define TSCE7_SIM_FW_LOAD_DONE_REG_ADDR (0x0800d205)
#define TSCE7_SIM_DSC_UC_CTRL_REG_ADDR (0x0800d00d)

/* Forward declarations */
STATIC int
tsce7_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data);
STATIC int
tsce7_sim_read(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t *data);

STATIC uint32_t
tsce7_sim_default_data_get(uint32_t addr)
{
    uint32_t reg;

    reg = TSCE7_REG_GET(addr);

    /*force FW load success */
    if(addr == TSCE7_SIM_FW_LOAD_DONE_REG_ADDR) {
        return (1 << 15);
    }

    switch (reg) {
        case 0x0002:
            return TSCE7_ID0;
        case 0x0003:
            return TSCE7_ID1;
        case 0x900e:
            return TSCE7_SERDES_ID;
        case 0x9003:
            return 0xe4e4;
        case 0x925a:
            return 0x0002;
        case 0x925b:
            return 0x006b;
        case 0x925d:
            return 0x3b5f;
        case 0x925e:
            return 0x006b;
        case 0xa000:
            return 0xfffc;
        case 0xa001:
            return 0x8030;
        case 0xc070:
            return 0xff00;
        case 0xc111:
            return 0x0800;
        case 0xc113:
            return 0x01c4;
        case 0xc134:
            return 0x2870;
        case 0xc185:
            return 0x02a0;
        case 0xc330:
            return 0x0002;
        case 0xd089:
            return 0x0007;
        case 0xd0a2:
            return 0x5250;
        case 0xd0b1:
            return 0x0007;
        case 0xd0b4:
            return 0x0500;
        case 0xd0b8:
            return 0x4042;
        case 0xd0c1:
            return 0x0008;
        case 0xd0d2:
            return 0x000e;
        case 0xd0e2:
            return 0x0002;
        case 0xd0f4:
            /* this is not the default value. value is changed to pass through
             * core int.
             */
            return 0x8271;
        case 0xd0f8:
            return 0x0007;
        case 0xd111:
            return 0x0020;
        case 0xd189:
            return 0x0007;
        case 0xd203:
            /* this is not the default value. value is changed to pass through
             * core init.
             */
            return 0x1;
        case 0xd0fa:
            /* this is not the default value. value is changed to pass through
             * lane_map_set.
             */
            return 0x4000;
        default:
            return 0;
    }
    return 0;
}

STATIC uint32_t
tsce7_sim_reg_copies_get(uint32_t addr)
{
    uint32_t devad, reg;

    devad = TSCE7_DEVAD_GET(addr);
    reg = TSCE7_REG_GET(addr);

    if (reg == TSCE7_AER || reg == TSCE7_BLK) {
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
_tsce7_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
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
            DBG_VERB(("_tsce7_sim_write 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                      addr, pse->data));
            return PHYMOD_E_NONE;
        }
    }

    /* Check if database is full */
    if (pms_data->entries_used >= pms_data->num_entries) {
        return PHYMOD_E_RESOURCE;
    }

    /* Check if new data matches default value */
    if (data == tsce7_sim_default_data_get(addr)) {
        return PHYMOD_E_NONE;
    }

    /* Add new register value */
    pse = &pms_data->entries[pms_data->entries_used++];
    pse->addr = addr;
    pse->data = data;

    DBG_VERB(("_tsce7_sim_write 0x%08"PRIx32" = 0x%04"PRIx32" (new)\n",
              addr, pse->data));

    return PHYMOD_E_NONE;
}

STATIC uint32_t
tsce7_sim_write_adjust(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
{
    uint32_t devad, reg, val;

    devad = TSCE7_DEVAD_GET(addr);
    reg = TSCE7_REG_GET(addr);

    if (devad == 0) {
        switch (reg) {
        case 0xc050:
            /* Set SW_SPEED_CHANGE_DONE and SW_SPEED_CONFIG_VLD in status reg */
            tsce7_sim_write(pms_data, addr + 1, 0x3);
            /* Force link up in RX_X4_PCS_LIVE_STS register
               offset 0x104 = 0xc154 - 0xc050 */
            tsce7_sim_write(pms_data, addr + 0x104, 0x1b);
            /* Sync configured speed into final speed status register 0x20 = 0xc070 - 0xc050 */
            tsce7_sim_read(pms_data, addr + 0x20, &val);
            val = (val & 0xffff00ff) | ((data & 0xff)<<8);
            tsce7_sim_write(pms_data, addr + 0x20, val);
            break;
        case 0xc055:
            /* Sync configured Lane Number into Lane Number status register 0x1B = 0xc070 - 0xc055 */
            tsce7_sim_read(pms_data, addr + 0x1B, &val);
            val = (val & 0xfffffff8) | (data & 0x7);
            tsce7_sim_write(pms_data, addr + 0x1B, val);
            break;
        default:
            break;
        }
    } else if (devad == 1) {
        switch (reg) {
        case 0xd0c1:
            /* Sync configured SIGDET_CTL1 register. When a port is set disable,
             * the port's RX_X4_PCS_LIVE_STS register deletes link status.
             * RX_X4_PCS_LIVE_STS register is PCS register, so need set devad to 0.
             * 0xF6D = 0xd0c1 - 0xc154*/
            tsce7_sim_read(pms_data, (addr - 0xF6D) & 0x7ffff, &val);
            val = (val & 0xfffd) | (((data&0x80)==0x80)?0:0x2);
            tsce7_sim_write(pms_data, (addr - 0xF6D) & 0x7ffff, val);
            break;
        default:
            break;
        }
    }

    return data;
}

STATIC int
tsce7_sim_init(phymod_sim_data_t *pms_data,
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
tsce7_sim_reset(phymod_sim_data_t *pms_data)
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
tsce7_sim_read(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t *data)
{
    int idx;
    uint32_t aer, blk, devad, reg, copies;
    uint32_t lane = 0;
    phymod_sim_entry_t *pse;

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    devad = 0;

    if (addr < TSCE7_BLK) {
        /* Assume clause 22 access */
        (void)tsce7_sim_read(pms_data, TSCE7_BLK, &blk);
        /* IEEE bit */
        if (addr & 0x10) {
            blk |= 0x8000;
        } else {
            blk &= ~0x8000;
        }
        addr = (addr & 0xf) | (blk & 0xfff0);
        if (addr != TSCE7_AER && addr != TSCE7_BLK) {
            (void)tsce7_sim_read(pms_data, TSCE7_AER, &aer);
            addr |= (aer << 16);
        }
    } else {
        /* Extract devad if clause 45 address format */
        if ((addr & TSCE7_CL45_MASK) == TSCE7_CL45) {
            devad = (addr >> 16) & 0x1f;
            addr &= 0xffff;
        }
    }

    if (addr != TSCE7_AER && addr != TSCE7_BLK) {
        /* Assume AER is in upper 16 bits */
        aer = (addr >> 16);
        if (aer == 0) {
            /* Try reading real AER instead */
            (void)tsce7_sim_read(pms_data, TSCE7_AER, &aer);
        }
        /* Add clause 45 devad (if used) */
        if (devad) {
            aer |= (devad << 11);
            addr = (addr & 0xffff) | (aer << 16);
        }
        lane = (aer & 0x7);
        if (lane > 3) {
            /* Force lane 0 if lane is invalid */
            addr = TSCE7_ADDR(TSCE7_DEVAD_GET(addr), 0, TSCE7_REG_GET(addr));
        }
    }

    /* Adjust lane according to number of copies */
    devad = TSCE7_DEVAD_GET(addr);
    reg = TSCE7_REG_GET(addr);
    copies = tsce7_sim_reg_copies_get(addr);
    if (copies == 1) {
        lane = 0;
    } else if (copies == 2) {
        lane &= ~0x1;
    }
    addr = TSCE7_ADDR(devad, lane, reg);

    /* for DSC_UC_CTRL always succeed */
    if(addr == TSCE7_SIM_DSC_UC_CTRL_REG_ADDR) {
        *data = 0x80;
        DBG_VERB(("tsce7_sim_read 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                      addr, *data));
        return PHYMOD_E_NONE;
    }

    /* Check if this register has been written already */
    for (idx = 0; idx < pms_data->entries_used; idx++) {
        pse = &pms_data->entries[idx];
        if (pse->addr == addr) {
            *data = pse->data;
            DBG_VERB(("tsce7_sim_read 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                      addr, *data));
            return PHYMOD_E_NONE;
        }
    }

    /* Return default value if register was never written */
    *data = tsce7_sim_default_data_get(addr);

    DBG_VERB(("tsce7_sim_read 0x%08"PRIx32" = [0x%04"PRIx32"]\n",
              addr, *data));

    return PHYMOD_E_NONE;
}

STATIC int
tsce7_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
{
    uint32_t aer, blk, devad, reg, copies;
    uint32_t lane = 0;

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    devad = 0;

    if (addr < TSCE7_BLK) {
        /* Assume clause 22 access */
        (void)tsce7_sim_read(pms_data, TSCE7_BLK, &blk);
        /* IEEE bit */
        if (addr & 0x10) {
            blk |= 0x8000;
        } else {
            blk &= ~0x8000;
        }
        addr = (addr & 0xf) | (blk & 0xfff0);
        if (addr != TSCE7_AER && addr != TSCE7_BLK) {
            (void)tsce7_sim_read(pms_data, TSCE7_AER, &aer);
            addr |= (aer << 16);
        }
    } else {
        /* Extract devad if clause 45 address format */
        if ((addr & TSCE7_CL45_MASK) == TSCE7_CL45) {
            devad = (addr >> 16) & 0x1f;
            addr &= 0xffff;
        }
    }

    if (addr != TSCE7_AER && addr != TSCE7_BLK) {
        /* Assume AER is in upper 16 bits */
        aer = (addr >> 16);
        if (aer == 0) {
            /* Try reading real AER instead */
            (void)tsce7_sim_read(pms_data, TSCE7_AER, &aer);
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
            reg = TSCE7_REG_GET(addr);
            devad = TSCE7_DEVAD_GET(addr);
            if (lane == 4 || lane == 6) {
                /* Write lanes 0 and 1 */
                addr = TSCE7_ADDR(devad, 8, reg);
                (void)tsce7_sim_write(pms_data, addr, data);
                addr = TSCE7_ADDR(devad, 1, reg);
                (void)tsce7_sim_write(pms_data, addr, data);
            }
            if (lane == 5 || lane == 6) {
                /* Write lanes 2 and 3 */
                addr = TSCE7_ADDR(devad, 2, reg);
                (void)tsce7_sim_write(pms_data, addr, data);
                addr = TSCE7_ADDR(devad, 3, reg);
                (void)tsce7_sim_write(pms_data, addr, data);
            }
            return PHYMOD_E_NONE;
        }
    }

    /* Adjust data and/or related registers */
    data = tsce7_sim_write_adjust(pms_data, addr, data);

    /* Adjust lane according to number of copies */
    devad = TSCE7_DEVAD_GET(addr);
    reg = TSCE7_REG_GET(addr);
    copies = tsce7_sim_reg_copies_get(addr);
    if (copies == 1) {
        lane = 0;
    } else if (copies == 2) {
        lane &= ~0x1;
    }
    addr = TSCE7_ADDR(devad, lane, reg);

    return _tsce7_sim_write(pms_data, addr, data);
}

STATIC int
tsce7_sim_event(phymod_sim_data_t *pms_data, phymod_sim_event_t event)
{
    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    return PHYMOD_E_NONE;
}

phymod_sim_drv_t tsce7_sim_drv = {
    tsce7_sim_init,
    tsce7_sim_reset,
    tsce7_sim_read,
    tsce7_sim_write,
    tsce7_sim_event
};

#endif /* EXCLUDE_PHYMOD_TSCE7_SIM */
