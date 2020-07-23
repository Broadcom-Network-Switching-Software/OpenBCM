/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *
 * This software simulator can emulate basic register access for the
 * Viper SerDes PHY.
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

#ifndef EXCLUDE_PHYMOD_VIPER_SIM

#include <phymod/phymod_system.h>
#include <phymod/phymod_sim.h>
#include <phymod/chip/bcmi_viper_xgxs_resetval.h>

/* Convenience macro */
#define DBG_VERB PHYMOD_DEBUG_VERBOSE

/* Bit field get/set macros */
#define VIPER_BF_SET(_val, _mask, _shift) _val |= ((_mask) << (_shift))
#define VIPER_BF_GET(_val, _mask, _shift) (((_val) >> (_shift)) & (_mask))

/*
 * Raw 32-bit address consists of AER value in upper 16 bits and
 * clause 45 address in lower 16 bits.
 */
#define VIPER_DEVAD_SHIFT       27
#define VIPER_DEVAD_MASK        0x1f
#define VIPER_DEVAD_GET(_addr) \
    VIPER_BF_GET(_addr, VIPER_DEVAD_MASK, VIPER_DEVAD_SHIFT)
#define VIPER_LANE_SHIFT        16
#define VIPER_LANE_MASK         0x7
#define VIPER_LANE_GET(_addr) \
    VIPER_BF_GET(_addr, VIPER_LANE_MASK, VIPER_LANE_SHIFT)
#define VIPER_REG_SHIFT         0
#define VIPER_REG_MASK          0xffff
#define VIPER_REG_GET(_addr) \
    VIPER_BF_GET(_addr, VIPER_REG_MASK, VIPER_REG_SHIFT)

#define VIPER_ADDR(_devad, _lane, _reg) \
    (((_devad) << VIPER_DEVAD_SHIFT) +  \
     ((_lane) << VIPER_LANE_SHIFT) +    \
     ((_reg) << VIPER_REG_SHIFT))

#define VIPER_AER               VIPER_ADDR(0, 0, 0xffde)
#define VIPER_BLK               VIPER_ADDR(0, 0, 0x001f)


/*
 * The CL45 indicator is used to determine whether the upper 16 bits
 * of the address is an AER value or a clause 45 DEVAD.
 */
#define VIPER_CL45              (0x20 << 16)
#define VIPER_CL45_MASK         (0xe0 << 16)

#define VMOD_ID0       0x0143
#define VMOD_ID1       0xbff0

#define VXMOD_MODEL    0x02c8
#define VGMOD_MODEL    0x02cf

/* Forward declarations */
STATIC int
_viper_sim_read(phymod_sim_data_t *pms_data, uint32_t core_type, uint32_t addr, uint32_t *data);
STATIC int
_viper_sim_write(phymod_sim_data_t *pms_data, uint32_t core_type, uint32_t addr, uint32_t data);


STATIC uint32_t
viper_sim_default_data_get(uint32_t core_type, uint32_t addr)
{
    uint32_t devad, reg;

    devad = VIPER_DEVAD_GET(addr);
    reg = VIPER_REG_GET(addr);

    if (devad == 0) {
        switch (reg) {
            case 0x0002:
            case 0xffe2:
                return VMOD_ID0;
            case 0x0003:
            case 0xffe3:
                return VMOD_ID1;
            case 0x8310:
                return (core_type == VXMOD_MODEL ? VXMOD_MODEL : VGMOD_MODEL);
            case 0x00000001:
                return 0x109;
            default:
                break;
        }
    }

    
    return 0;
}

STATIC uint32_t
viper_sim_reg_copies_get(uint32_t addr)
{
    uint32_t devad, reg;

    devad = VIPER_DEVAD_GET(addr);
    reg = VIPER_REG_GET(addr);

    if (reg == VIPER_AER || reg == VIPER_BLK) {
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

STATIC uint32_t
viper_sim_write_adjust(phymod_sim_data_t *pms_data, uint32_t core_type, uint32_t addr, uint32_t data)
{
    uint32_t devad, reg, val;
    uint32_t sgmii_mode = 0, speed_id = 0, duplex_status = 0;

    devad = VIPER_DEVAD_GET(addr);
    reg = VIPER_REG_GET(addr);

    if (devad == 0) {
        switch (reg) {
        case 0xc050:
            /* Set SW_SPEED_CHANGE_DONE and SW_SPEED_CONFIG_VLD in status reg */
            _viper_sim_write(pms_data, core_type, addr + 1, 0x3);
            break;
        case 0x0000:
            /* Set SPEED_STATUS and DUPLEX_STATUS  in status reg 0x8304  */
            _viper_sim_read(pms_data, core_type, addr + 0x8304, &val);
            speed_id = (((data >> 6) & 0x1) << 1) | ((data >> 13) & 0x1);
            duplex_status = ((data >> 8) & 0x1);
            val = (val & 0xfffffffd) | ((duplex_status & 0x1) << 2) | (0x1 << 1);
            val = (val & 0xffffffc7) | ((speed_id & 0x3) << 3);
            _viper_sim_write(pms_data, core_type, addr + 0x8304, val);
            break;
        case 0x8300:
            /* Set SGMII_MODE in status reg 0x8304 */
            _viper_sim_read(pms_data, core_type, addr + 0x4, &val);
            sgmii_mode = ((data & 0x1)==1)?0:1;
            val = (val & 0xfffffffe) | (sgmii_mode & 0x1) | (0x1 << 1);
            _viper_sim_write(pms_data, core_type, addr + 0x4, val);
            break;
        case 0x8308:
            if ((data & 0x1f)==0x10) {   /* 2.5G, set SPEED_STATUS in status reg 0x8304 */
                speed_id = 0x3;
                _viper_sim_read(pms_data, core_type, addr - 0x4, &val);
                val = (val & 0xffffffc7) | ((speed_id & 0x3) << 3) | (0x1 << 1);
                _viper_sim_write(pms_data, core_type, addr - 0x4, val);
            } else if ((data & 0x1f)==0x14) { /* 10G, set SPEED_STATUS in status reg 0x8122 */
                speed_id = 0x7;
                _viper_sim_read(pms_data, core_type, addr - 0x1E6, &val);
                val = (val & 0xffffffF0) | (speed_id & 0xF) | (0x1 << 9);
                _viper_sim_write(pms_data, core_type,  addr - 0x1E6, val);
            }
            break;
        default:
            break;
        }
    } else if (devad == 1) {
        switch (reg) {
        default:
            break;
        }
    }

    return data;
}

STATIC int
viper_sim_init(phymod_sim_data_t *pms_data,
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
viper_sim_reset(phymod_sim_data_t *pms_data)
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
_viper_sim_read(phymod_sim_data_t *pms_data, uint32_t core_type, uint32_t addr, uint32_t *data)
{
    int idx;
    uint32_t aer, blk, devad, reg, copies;
    uint32_t lane = 0;
    phymod_sim_entry_t *pse;

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    devad = 0;

    if (addr < VIPER_BLK) {
        /* Assume clause 22 access */
        (void)_viper_sim_read(pms_data, core_type, VIPER_BLK, &blk);
        /* IEEE bit */
        if (addr & 0x10) {
            blk |= 0x8000;
        } else {
            blk &= ~0x8000;
        }
        addr = (addr & 0xf) | (blk & 0xfff0);
        if (addr != VIPER_AER && addr != VIPER_BLK) {
            (void)_viper_sim_read(pms_data, core_type, VIPER_AER, &aer);
            addr |= (aer << 16);
        }
    } else {
        /* Extract devad if clause 45 address format */
        if ((addr & VIPER_CL45_MASK) == VIPER_CL45) {
            devad = (addr >> 16) & 0x1f;
            addr &= 0xffff;
        }
    }

    if (addr != VIPER_AER && addr != VIPER_BLK) {
        /* Assume AER is in upper 16 bits */
        aer = (addr >> 16);
        if (aer == 0) {
            /* Try reading real AER instead */
            (void)_viper_sim_read(pms_data, core_type, VIPER_AER, &aer);
        }
        /* Add clause 45 devad (if used) */
        if (devad) {
            aer |= (devad << 11);
            addr = (addr & 0xffff) | (aer << 16);
        }
        lane = (aer & 0x7);
        if (lane > 3) {
            /* Force lane 0 if lane is invalid */
            addr = VIPER_ADDR(VIPER_DEVAD_GET(addr), 0, VIPER_REG_GET(addr));
        }
    }

    /* Adjust lane according to number of copies */
    devad = VIPER_DEVAD_GET(addr);
    reg = VIPER_REG_GET(addr);
    copies = viper_sim_reg_copies_get(addr);
    if (copies == 1) {
        lane = 0;
    } else if (copies == 2) {
        lane &= ~0x1;
    }
    addr = VIPER_ADDR(devad, lane, reg);

    /* Check if this register has been written already */
    for (idx = 0; idx < pms_data->entries_used; idx++) {
        pse = &pms_data->entries[idx];
        if (pse->addr == addr) {
            *data = pse->data;
            DBG_VERB(("_viper_sim_read 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                      addr, *data));
            return PHYMOD_E_NONE;
        }
    }

    /* Return default value if register was never written */
    *data = viper_sim_default_data_get(core_type, addr);

    DBG_VERB(("_viper_sim_read 0x%08"PRIx32" = [0x%04"PRIx32"]\n",
              addr, *data));

    return PHYMOD_E_NONE;
}

STATIC int
_viper_sim_write(phymod_sim_data_t *pms_data, uint32_t core_type, uint32_t addr, uint32_t data)
{
    int idx;
    uint32_t aer, blk, devad, reg, copies, mask;
    uint32_t lane = 0;
    phymod_sim_entry_t *pse;

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    devad = 0;

    if (addr < VIPER_BLK) {
        /* Assume clause 22 access */
        (void)_viper_sim_read(pms_data, core_type, VIPER_BLK, &blk);
        /* IEEE bit */
        if (addr & 0x10) {
            blk |= 0x8000;
        } else {
            blk &= ~0x8000;
        }
        addr = (addr & 0xf) | (blk & 0xfff0);
        if (addr != VIPER_AER && addr != VIPER_BLK) {
            (void)_viper_sim_read(pms_data, core_type, VIPER_AER, &aer);
            addr |= (aer << 16);
        }
    } else {
        /* Extract devad if clause 45 address format */
        if ((addr & VIPER_CL45_MASK) == VIPER_CL45) {
            devad = (addr >> 16) & 0x1f;
            addr &= 0xffff;
        }
    }

    if (addr != VIPER_AER && addr != VIPER_BLK) {
        /* Assume AER is in upper 16 bits */
        aer = (addr >> 16);
        if (aer == 0) {
            /* Try reading real AER instead */
            (void)_viper_sim_read(pms_data, core_type, VIPER_AER, &aer);
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
            reg = VIPER_REG_GET(addr);
            devad = VIPER_DEVAD_GET(addr);
            if (lane == 4 || lane == 6) {
                /* Write lanes 0 and 1 */
                addr = VIPER_ADDR(devad, 8, reg);
                (void)_viper_sim_write(pms_data, core_type, addr, data);
                addr = VIPER_ADDR(devad, 1, reg);
                (void)_viper_sim_write(pms_data, core_type, addr, data);
            }
            if (lane == 5 || lane == 6) {
                /* Write lanes 2 and 3 */
                addr = VIPER_ADDR(devad, 2, reg);
                (void)_viper_sim_write(pms_data, core_type, addr, data);
                addr = VIPER_ADDR(devad, 3, reg);
                (void)_viper_sim_write(pms_data, core_type, addr, data);
            }
            return PHYMOD_E_NONE;
        }
    }

    /* Adjust data and/or related registers */
    data = viper_sim_write_adjust(pms_data, core_type, addr, data);

    /* Adjust lane according to number of copies */
    devad = VIPER_DEVAD_GET(addr);
    reg = VIPER_REG_GET(addr);
    copies = viper_sim_reg_copies_get(addr);
    if (copies == 1) {
        lane = 0;
    } else if (copies == 2) {
        lane &= ~0x1;
    }
    addr = VIPER_ADDR(devad, lane, reg);

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
            DBG_VERB(("_viper_sim_write 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                      addr, pse->data));
            return PHYMOD_E_NONE;
        }
    }

    /* Check if database is full */
    if (pms_data->entries_used >= pms_data->num_entries) {
        return PHYMOD_E_RESOURCE;
    }

    /* Check if new data matches default value */
    if (data == viper_sim_default_data_get(core_type, addr)) {
        return PHYMOD_E_NONE;
    }

    /* Add new register value */
    pse = &pms_data->entries[pms_data->entries_used++];
    pse->addr = addr;
    pse->data = data;

    DBG_VERB(("_viper_sim_write 0x%08"PRIx32" = 0x%04"PRIx32" (new)\n",
              addr, pse->data));

    return PHYMOD_E_NONE;
}
STATIC int
viper_sim_read(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t *data)
{
    return _viper_sim_read(pms_data, VXMOD_MODEL, addr, data);
}

STATIC int
viper_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
{
    return _viper_sim_write(pms_data, VXMOD_MODEL, addr, data);
}

STATIC int
viper_sp2_sim_read(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t *data)
{
    return _viper_sim_read(pms_data, VGMOD_MODEL, addr, data);
}

STATIC int
viper_sp2_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
{
    return _viper_sim_write(pms_data, VGMOD_MODEL, addr, data);
}

STATIC int
viper_sim_event(phymod_sim_data_t *pms_data, phymod_sim_event_t event)
{
    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    return PHYMOD_E_NONE;
}

phymod_sim_drv_t viper_sim_drv = {
    viper_sim_init,
    viper_sim_reset,
    viper_sim_read,
    viper_sim_write,
    viper_sim_event
};


/* for SGMIIPLUS2 core sim driver */
phymod_sim_drv_t viper_sp2_sim_drv = {
    viper_sim_init,
    viper_sim_reset,
    viper_sp2_sim_read,
    viper_sp2_sim_write,
    viper_sim_event
};
#endif /* EXCLUDE_PHYMOD_VIPER_SIM */
