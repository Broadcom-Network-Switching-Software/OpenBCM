/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *
 * This software simulator can emulate basic register access for the
 * QSGMII/Eagle SerDes PHY.
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

#ifndef EXCLUDE_PHYMOD_QSGMIIE_SIM 

#include <phymod/phymod_system.h>
#include <phymod/phymod_sim.h>
#include <phymod/chip/bcmi_qsgmiie_serdes_resetval.h>

/* Convenience macro */
#define DBG_VERB PHYMOD_DEBUG_VERBOSE

/* Bit field get/set macros */
#define QSGMIIE_BF_SET(_val, _mask, _shift) _val |= ((_mask) << (_shift))
#define QSGMIIE_BF_GET(_val, _mask, _shift) (((_val) >> (_shift)) & (_mask))

/*
 * Raw 32-bit address consists of AER value in upper 16 bits and
 * clause 45 address in lower 16 bits.
 */
#define QSGMIIE_DEVAD_SHIFT     27
#define QSGMIIE_DEVAD_MASK      0x1f
#define QSGMIIE_DEVAD_GET(_addr) \
    QSGMIIE_BF_GET(_addr, QSGMIIE_DEVAD_MASK, QSGMIIE_DEVAD_SHIFT)
#define QSGMIIE_LANE_SHIFT      16
#define QSGMIIE_LANE_MASK       0x7
#define QSGMIIE_LANE_GET(_addr) \
    QSGMIIE_BF_GET(_addr, QSGMIIE_LANE_MASK, QSGMIIE_LANE_SHIFT)
#define QSGMIIE_REG_SHIFT       0
#define QSGMIIE_REG_MASK        0xffff
#define QSGMIIE_REG_GET(_addr) \
    QSGMIIE_BF_GET(_addr, QSGMIIE_REG_MASK, QSGMIIE_REG_SHIFT)

#define QSGMIIE_ADDR(_devad, _lane, _reg) \
    (((_devad) << QSGMIIE_DEVAD_SHIFT) +  \
     ((_lane) << QSGMIIE_LANE_SHIFT) +    \
     ((_reg) << QSGMIIE_REG_SHIFT))

#define QSGMIIE_AER             QSGMIIE_ADDR(0, 0, 0xffde)
#define QSGMIIE_BLK             QSGMIIE_ADDR(0, 0, 0x001f)

/*
 * The CL45 indicator is used to determine whether the upper 16 bits
 * of the address is an AER value or a clause 45 DEVAD.
 */
#define QSGMIIE_CL45            (0x20 << 16)
#define QSGMIIE_CL45_MASK       (0xe0 << 16)

STATIC uint32_t
qsgmiie_sim_default_data_get(uint32_t addr)
{
    uint32_t devad, reg;
    devad = QSGMIIE_DEVAD_GET(addr);
    reg = QSGMIIE_REG_GET(addr);
    COMPILER_REFERENCE(devad);
    COMPILER_REFERENCE(reg);
#ifdef PHYMOD_QSGMIIE_SUPPORT
    return bcmi_qsgmiie_serdes_resetval_get(devad, reg);
#else
    return 0;
#endif
}

STATIC uint32_t
qsgmiie_sim_reg_copies_get(uint32_t addr)
{
    uint32_t devad, reg;

    devad = QSGMIIE_DEVAD_GET(addr);
    reg = QSGMIIE_REG_GET(addr);

    if (reg == QSGMIIE_AER || reg == QSGMIIE_BLK) {
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
    }
    return 0;
}

STATIC int
qsgmiie_sim_init(phymod_sim_data_t *pms_data,
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
qsgmiie_sim_reset(phymod_sim_data_t *pms_data)
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
qsgmiie_sim_read(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t *data)
{
    int idx;
    uint32_t aer, blk, devad, reg, copies;
    uint32_t lane = 0;
    phymod_sim_entry_t *pse;

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    devad = 0;

    if (addr < QSGMIIE_BLK) {
        /* Assume clause 22 access */
        (void)qsgmiie_sim_read(pms_data, QSGMIIE_BLK, &blk);
        /* IEEE bit */
        if (addr & 0x10) {
            blk |= 0x8000;
        } else {
            blk &= ~0x8000;
        }
        addr = (addr & 0xf) | (blk & 0xfff0);
        if (addr != QSGMIIE_AER && addr != QSGMIIE_BLK) {
            (void)qsgmiie_sim_read(pms_data, QSGMIIE_AER, &aer);
            addr |= (aer << 16);
        }
    } else {
        /* Extract devad if clause 45 address format */
        if ((addr & QSGMIIE_CL45_MASK) == QSGMIIE_CL45) {
            devad = (addr >> 16) & 0x1f;
            addr &= 0xffff;
        }
    }

    if (addr != QSGMIIE_AER && addr != QSGMIIE_BLK) {
        /* Assume AER is in upper 16 bits */
        aer = (addr >> 16);
        if (aer == 0) {
            /* Try reading real AER instead */
            (void)qsgmiie_sim_read(pms_data, QSGMIIE_AER, &aer);
        }
        /* Add clause 45 devad (if used) */
        if (devad) {
            aer |= (devad << 11);
            addr = (addr & 0xffff) | (aer << 16);
        }
        lane = (aer & 0x7);
        if (lane > 3) {
            /* Force lane 0 if lane is invalid */
            addr = QSGMIIE_ADDR(QSGMIIE_DEVAD_GET(addr), 0, QSGMIIE_REG_GET(addr));
        }
    }

    /* Adjust lane according to number of copies */
    devad = QSGMIIE_DEVAD_GET(addr);
    reg = QSGMIIE_REG_GET(addr);
    copies = qsgmiie_sim_reg_copies_get(addr);
    if (copies == 1) {
        lane = 0;
    } else if (copies == 2) {
        lane &= ~0x1;
    }
    addr = QSGMIIE_ADDR(devad, lane, reg);

    /* Check if this register has been written already */
    for (idx = 0; idx < pms_data->entries_used; idx++) {
        pse = &pms_data->entries[idx];
        if (pse->addr == addr) {
            *data = pse->data;
            DBG_VERB(("qsgmiie_sim_read 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                      addr, *data));
            return PHYMOD_E_NONE;
        }
    }

    /* Return default value if register was never written */
    *data = qsgmiie_sim_default_data_get(addr);

    DBG_VERB(("qsgmiie_sim_read 0x%08"PRIx32" = [0x%04"PRIx32"]\n",
              addr, *data));

    return PHYMOD_E_NONE;
}

STATIC int
qsgmiie_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
{
    int idx;
    uint32_t aer, blk, devad, reg, copies, mask;
    uint32_t lane = 0;
    phymod_sim_entry_t *pse;

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    devad = 0;

    if (addr < QSGMIIE_BLK) {
        /* Assume clause 22 access */
        (void)qsgmiie_sim_read(pms_data, QSGMIIE_BLK, &blk);
        /* IEEE bit */
        if (addr & 0x10) {
            blk |= 0x8000;
        } else {
            blk &= ~0x8000;
        }
        addr = (addr & 0xf) | (blk & 0xfff0);
        if (addr != QSGMIIE_AER && addr != QSGMIIE_BLK) {
            (void)qsgmiie_sim_read(pms_data, QSGMIIE_AER, &aer);
            addr |= (aer << 16);
        }
    } else {
        /* Extract devad if clause 45 address format */
        if ((addr & QSGMIIE_CL45_MASK) == QSGMIIE_CL45) {
            devad = (addr >> 16) & 0x1f;
            addr &= 0xffff;
        }
    }

    if (addr != QSGMIIE_AER && addr != QSGMIIE_BLK) {
        /* Assume AER is in upper 16 bits */
        aer = (addr >> 16);
        if (aer == 0) {
            /* Try reading real AER instead */
            (void)qsgmiie_sim_read(pms_data, QSGMIIE_AER, &aer);
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
            reg = QSGMIIE_REG_GET(addr);
            devad = QSGMIIE_DEVAD_GET(addr);
            if (lane == 4 || lane == 6) {
                /* Write lanes 0 and 1 */
                addr = QSGMIIE_ADDR(devad, 8, reg);
                (void)qsgmiie_sim_write(pms_data, addr, data);
                addr = QSGMIIE_ADDR(devad, 1, reg);
                (void)qsgmiie_sim_write(pms_data, addr, data);
            }
            if (lane == 5 || lane == 6) {
                /* Write lanes 2 and 3 */
                addr = QSGMIIE_ADDR(devad, 2, reg);
                (void)qsgmiie_sim_write(pms_data, addr, data);
                addr = QSGMIIE_ADDR(devad, 3, reg);
                (void)qsgmiie_sim_write(pms_data, addr, data);
            }
            return PHYMOD_E_NONE;
        }
    }

    /* Adjust lane according to number of copies */
    devad = QSGMIIE_DEVAD_GET(addr);
    reg = QSGMIIE_REG_GET(addr);
    copies = qsgmiie_sim_reg_copies_get(addr);
    if (copies == 1) {
        lane = 0;
    } else if (copies == 2) {
        lane &= ~0x1;
    }
    addr = QSGMIIE_ADDR(devad, lane, reg);

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
            DBG_VERB(("qsgmiie_sim_write 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                      addr, pse->data));
            return PHYMOD_E_NONE;
        }
    }

    /* Check if database is full */
    if (pms_data->entries_used >= pms_data->num_entries) {
        return PHYMOD_E_RESOURCE;
    }

    /* Check if new data matches default value */
    if (data == qsgmiie_sim_default_data_get(addr)) {
        return PHYMOD_E_NONE;
    }

    /* Add new register value */
    pse = &pms_data->entries[pms_data->entries_used++];
    pse->addr = addr;
    pse->data = data;

    DBG_VERB(("qsgmiie_sim_write 0x%08"PRIx32" = 0x%04"PRIx32" (new)\n",
              addr, pse->data));

    return PHYMOD_E_NONE;
}

STATIC int
qsgmiie_sim_event(phymod_sim_data_t *pms_data, phymod_sim_event_t event)
{
    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    return PHYMOD_E_NONE;
}

phymod_sim_drv_t qsgmiie_sim_drv = {
    qsgmiie_sim_init,
    qsgmiie_sim_reset,
    qsgmiie_sim_read,
    qsgmiie_sim_write,
    qsgmiie_sim_event
};

#endif /* EXCLUDE_PHYMOD_QSGMIIE_SIM */ 
