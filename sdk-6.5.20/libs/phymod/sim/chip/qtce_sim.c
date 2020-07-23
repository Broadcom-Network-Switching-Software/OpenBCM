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

#ifndef EXCLUDE_PHYMOD_QTCE_SIM 

#include <phymod/phymod_system.h>
#include <phymod/phymod_sim.h>

/* Convenience macro */
#define DBG_VERB PHYMOD_DEBUG_VERBOSE

/* Bit field get/set macros */
#define QTCE_BF_SET(_val, _mask, _shift) _val |= ((_mask) << (_shift))
#define QTCE_BF_GET(_val, _mask, _shift) (((_val) >> (_shift)) & (_mask))

/*
 * Raw 32-bit address consists of AER value in upper 16 bits and
 * clause 45 address in lower 16 bits.
 */
#define QTCE_DEVAD_SHIFT       27
#define QTCE_DEVAD_MASK        0x1f
#define QTCE_DEVAD_GET(_addr) \
    QTCE_BF_GET(_addr, QTCE_DEVAD_MASK, QTCE_DEVAD_SHIFT)
#define QTCE_LANE_SHIFT        16
#define QTCE_LANE_MASK         0x7
#define QTCE_LANE_GET(_addr) \
    QTCE_BF_GET(_addr, QTCE_LANE_MASK, QTCE_LANE_SHIFT)
#define QTCE_REG_SHIFT         0
#define QTCE_REG_MASK          0xffff
#define QTCE_REG_GET(_addr) \
    QTCE_BF_GET(_addr, QTCE_REG_MASK, QTCE_REG_SHIFT)

#define QTCE_ADDR(_devad, _lane, _reg) \
    (((_devad) << QTCE_DEVAD_SHIFT) +  \
     ((_lane) << QTCE_LANE_SHIFT) +    \
     ((_reg) << QTCE_REG_SHIFT))

#define QTCE_AER               QTCE_ADDR(0, 0, 0xffde)
#define QTCE_BLK               QTCE_ADDR(0, 0, 0x001f)

/*
 * The CL45 indicator is used to determine whether the upper 16 bits
 * of the address is an AER value or a clause 45 DEVAD.
 */
#define QTCE_CL45              (0x20 << 16)
#define QTCE_CL45_MASK         (0xe0 << 16)

/* Forward declarations */
STATIC int
qtce_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data);
STATIC int
qtce_sim_read(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t *data);

STATIC uint32_t
qtce_sim_default_data_get(uint32_t addr)
{
    uint32_t devad, reg;

    devad = QTCE_DEVAD_GET(addr);
    reg = QTCE_REG_GET(addr);

    if (devad == 0) {
        switch (reg) {
        case 0x0002:
            return 0x600d;
        case 0x0003:
            return 0x8770;
        case 0x9007: /* MAIN_SERDESID */
            return 0x02d5;
        case 0xc041: /* sc_x4_final_config_status_sp(0)_A */
        case 0xc051: /* sc_x4_final_config_status_sp(1)_A */
        case 0xc061: /* sc_x4_final_config_status_sp(2)_A */
        case 0xc071: /* sc_x4_final_config_status_sp(3)_A */
            return 0x20;
        case 0xc020: /* sc_x4_control_control */
            return 0x1ff;
        case 0xc179: 
            return 0xff;
        default:
            break;
        }
    } else if (devad == 1) {
        switch (reg) {
        case 0xd205: /* UC_DOWNLOAD_STS */
            return 0x0080;
        case 0xd089:
            return 0x0007;
        case 0xd0f8:
            return 0x0007;
        default:
            break;
        }
    }

    return 0;
}

STATIC uint32_t
qtce_sim_reg_copies_get(uint32_t addr)
{
    uint32_t devad, reg;

    devad = QTCE_DEVAD_GET(addr);
    reg = QTCE_REG_GET(addr);

    if (reg == QTCE_AER || reg == QTCE_BLK) {
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
qtce_sim_write_adjust(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
{
    uint32_t devad, reg;
    uint32_t addr_tmp, data_tmp, data_r;
    uint32_t i, mask;

    devad = QTCE_DEVAD_GET(addr);
    reg = QTCE_REG_GET(addr);

    if (devad == 0) {
        switch (reg) {
        case 0xc020:
            /* Set SW_SPEED_CHANGE_DONE and SW_SPEED_CONFIG_VLD in status reg */
            qtce_sim_write(pms_data, addr + 2, 0x3);
            /* Sync speed to  0xc040 sc_x4_final_config_status_sp0_resolved_speed */
            qtce_sim_write(pms_data, addr + 0x20, data & 0Xff);
            /* Set 0xc179 RX_X4_Status0_pcs_live_status and 0xc178 RX_X4_Status0_pcs_latched_status. force all ports link up */
            qtce_sim_write(pms_data, addr + 0x158, 0x0);
            qtce_sim_write(pms_data, addr + 0x159, 0xff);
            break;
        case 0xc021:
            mask = (data >> 16);
            if (mask == 0) {
                mask = 0xffff;
            }

            qtce_sim_read(pms_data, addr, &data_r);
            data_r &= ~mask;
            data |= data_r;

            for (i = 0; i < 4; i++) {
                addr_tmp = addr + 0x20 + i*0x10;  /* register 0xc041, 0xc051, 0xc061, 0xc071 */
                data_tmp = (data >> i*2) & 0x3; /* 0: 1G, 1: 100M, 2: 10M  for 0xc021*/
                data_tmp =  data_tmp == 0 ? 2 : data_tmp == 1 ? 0 : 1;  /* 0: 100M, 1: 10M, 2: 1G for 0xc041  */
                qtce_sim_read(pms_data, addr_tmp, &data_r);
                qtce_sim_write(pms_data, addr_tmp, ((data_r & 0xffcf) |  (data_tmp << 4)));
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
qtce_sim_init(phymod_sim_data_t *pms_data,
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
qtce_sim_reset(phymod_sim_data_t *pms_data)
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
qtce_sim_read(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t *data)
{
    int idx;
    uint32_t aer, blk, devad, reg, copies;
    uint32_t lane = 0;
    phymod_sim_entry_t *pse;

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    devad = 0;

    if (addr < QTCE_BLK) {
        /* Assume clause 22 access */
        (void)qtce_sim_read(pms_data, QTCE_BLK, &blk);
        /* IEEE bit */
        if (addr & 0x10) {
            blk |= 0x8000;
        } else {
            blk &= ~0x8000;
        }
        addr = (addr & 0xf) | (blk & 0xfff0);
        if (addr != QTCE_AER && addr != QTCE_BLK) {
            (void)qtce_sim_read(pms_data, QTCE_AER, &aer);
            addr |= (aer << 16);
        }
    } else {
        /* Extract devad if clause 45 address format */
        if ((addr & QTCE_CL45_MASK) == QTCE_CL45) {
            devad = (addr >> 16) & 0x1f;
            addr &= 0xffff;
        }
    }

    if (addr != QTCE_AER && addr != QTCE_BLK) {
        /* Assume AER is in upper 16 bits */
        aer = (addr >> 16);
        if (aer == 0) {
            /* Try reading real AER instead */
            (void)qtce_sim_read(pms_data, QTCE_AER, &aer);
        }
        /* Add clause 45 devad (if used) */
        if (devad) {
            aer |= (devad << 11);
            addr = (addr & 0xffff) | (aer << 16);
        }
        lane = (aer & 0x7);
        if (lane > 3) {
            /* Force lane 0 if lane is invalid */
            addr = QTCE_ADDR(QTCE_DEVAD_GET(addr), 0, QTCE_REG_GET(addr));
        }
    }

    /* Adjust lane according to number of copies */
    devad = QTCE_DEVAD_GET(addr);
    reg = QTCE_REG_GET(addr);
    copies = qtce_sim_reg_copies_get(addr);
    if (copies == 1) {
        lane = 0;
    } else if (copies == 2) {
        lane &= ~0x1;
    }
    addr = QTCE_ADDR(devad, lane, reg);

    if (addr == 0x800d00d) { /* DSC_UC_CTL:UC_DSC_READY_FOR_CMD */
        *data = 0x80;
        return PHYMOD_E_NONE;
    } else if (addr == 0x800d00e) { /* DSC_SCRATCH */
        *data = 0xf6c0; /* expected CRC */
        return PHYMOD_E_NONE;
    }

    /* Check if this register has been written already */
    for (idx = 0; idx < pms_data->entries_used; idx++) {
        pse = &pms_data->entries[idx];
        if (pse->addr == addr) {
            *data = pse->data;
            DBG_VERB(("qtce_sim_read 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                      addr, *data));
            return PHYMOD_E_NONE;
        }
    }

    /* Return default value if register was never written */
    *data = qtce_sim_default_data_get(addr);

    DBG_VERB(("qtce_sim_read 0x%08"PRIx32" = [0x%04"PRIx32"]\n",
              addr, *data));

    return PHYMOD_E_NONE;
}

STATIC int
qtce_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
{
    int idx;
    uint32_t aer, blk, devad, reg, copies, mask;
    uint32_t lane = 0;
    phymod_sim_entry_t *pse;

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    devad = 0;

    if (addr < QTCE_BLK) {
        /* Assume clause 22 access */
        (void)qtce_sim_read(pms_data, QTCE_BLK, &blk);
        /* IEEE bit */
        if (addr & 0x10) {
            blk |= 0x8000;
        } else {
            blk &= ~0x8000;
        }
        addr = (addr & 0xf) | (blk & 0xfff0);
        if (addr != QTCE_AER && addr != QTCE_BLK) {
            (void)qtce_sim_read(pms_data, QTCE_AER, &aer);
            addr |= (aer << 16);
        }
    } else {
        /* Extract devad if clause 45 address format */
        if ((addr & QTCE_CL45_MASK) == QTCE_CL45) {
            devad = (addr >> 16) & 0x1f;
            addr &= 0xffff;
        }
    }

    if (addr != QTCE_AER && addr != QTCE_BLK) {
        /* Assume AER is in upper 16 bits */
        aer = (addr >> 16);
        if (aer == 0) {
            /* Try reading real AER instead */
            (void)qtce_sim_read(pms_data, QTCE_AER, &aer);
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
            reg = QTCE_REG_GET(addr);
            devad = QTCE_DEVAD_GET(addr);
            if (lane == 4 || lane == 6) {
                /* Write lanes 0 and 1 */
                addr = QTCE_ADDR(devad, 8, reg);
                (void)qtce_sim_write(pms_data, addr, data);
                addr = QTCE_ADDR(devad, 1, reg);
                (void)qtce_sim_write(pms_data, addr, data);
            }
            if (lane == 5 || lane == 6) {
                /* Write lanes 2 and 3 */
                addr = QTCE_ADDR(devad, 2, reg);
                (void)qtce_sim_write(pms_data, addr, data);
                addr = QTCE_ADDR(devad, 3, reg);
                (void)qtce_sim_write(pms_data, addr, data);
            }
            return PHYMOD_E_NONE;
        }
    }

    /* Adjust data and/or related registers */
    data = qtce_sim_write_adjust(pms_data, addr, data);

    /* Adjust lane according to number of copies */
    devad = QTCE_DEVAD_GET(addr);
    reg = QTCE_REG_GET(addr);
    copies = qtce_sim_reg_copies_get(addr);
    if (copies == 1) {
        lane = 0;
    } else if (copies == 2) {
        lane &= ~0x1;
    }
    addr = QTCE_ADDR(devad, lane, reg);

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
            DBG_VERB(("qtce_sim_write 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                      addr, pse->data));
            return PHYMOD_E_NONE;
        }
    }

    /* Check if database is full */
    if (pms_data->entries_used >= pms_data->num_entries) {
        return PHYMOD_E_RESOURCE;
    }

    /* Check if new data matches default value */
    if (data == qtce_sim_default_data_get(addr)) {
        return PHYMOD_E_NONE;
    }

    /* Add new register value */
    pse = &pms_data->entries[pms_data->entries_used++];
    pse->addr = addr;
    pse->data = data;

    DBG_VERB(("qtce_sim_write 0x%08"PRIx32" = 0x%04"PRIx32" (new)\n",
              addr, pse->data));

    return PHYMOD_E_NONE;
}

STATIC int
qtce_sim_event(phymod_sim_data_t *pms_data, phymod_sim_event_t event)
{
    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    return PHYMOD_E_NONE;
}

phymod_sim_drv_t qtce_sim_drv = {
    qtce_sim_init,
    qtce_sim_reset,
    qtce_sim_read,
    qtce_sim_write,
    qtce_sim_event
};

#endif /* EXCLUDE_PHYMOD_QTCE_SIM */
