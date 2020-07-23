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

#ifndef EXCLUDE_PHYMOD_QTCE16_SIM 


#include <phymod/phymod_system.h>
#include <phymod/phymod_sim.h>

/* Convenience macro */
#define DBG_VERB PHYMOD_DEBUG_VERBOSE

/* Bit field get/set macros */
#define QTCE16_BF_SET(_val, _mask, _shift) _val |= ((_mask) << (_shift))
#define QTCE16_BF_GET(_val, _mask, _shift) (((_val) >> (_shift)) & (_mask))

/*
 * Raw 32-bit address consists of AER value in upper 16 bits and
 * clause 45 address in lower 16 bits.
 */
#define QTCE16_DEVAD_SHIFT       27
#define QTCE16_DEVAD_MASK        0x1f
#define QTCE16_DEVAD_GET(_addr) \
    QTCE16_BF_GET(_addr, QTCE16_DEVAD_MASK, QTCE16_DEVAD_SHIFT)
#define QTCE16_LANE_SHIFT        16
#define QTCE16_LANE_MASK         0x7
#define QTCE16_LANE_GET(_addr) \
    QTCE16_BF_GET(_addr, QTCE16_LANE_MASK, QTCE16_LANE_SHIFT)
#define QTCE16_REG_SHIFT         0
#define QTCE16_REG_MASK          0xffff
#define QTCE16_REG_GET(_addr) \
    QTCE16_BF_GET(_addr, QTCE16_REG_MASK, QTCE16_REG_SHIFT)

#define QTCE16_ADDR(_devad, _lane, _reg) \
    (((_devad) << QTCE16_DEVAD_SHIFT) +  \
     ((_lane) << QTCE16_LANE_SHIFT) +    \
     ((_reg) << QTCE16_REG_SHIFT))

#define QTCE16_AER               QTCE16_ADDR(0, 0, 0xffde)
#define QTCE16_BLK               QTCE16_ADDR(0, 0, 0x001f)

/*
 * The CL45 indicator is used to determine whether the upper 16 bits
 * of the address is an AER value or a clause 45 DEVAD.
 */
#define QTCE16_CL45              (0x20 << 16)
#define QTCE16_CL45_MASK         (0xe0 << 16)

#define QTCE16_SIM_FW_LOAD_DONE_REG_ADDR (0x0800d205)
#define QTCE16_SIM_DSC_UC_CTRL_REG_ADDR (0x0800d00d)

/*RAM sim*/
#define QTCE16_RAM_AHB_CTRL         (0xd202)
#define QTCE16_RAM_WR_ADDR_REG_MS   (0xd205)
#define QTCE16_RAM_WR_ADDR_REG_LS   (0xd204)
#define QTCE16_RAM_WR_DATA_REG_MS   (0xd207)
#define QTCE16_RAM_WR_DATA_REG_LS   (0xd206)
#define QTCE16_RAM_RD_ADDR_REG_MS   (0xd209)
#define QTCE16_RAM_RD_ADDR_REG_LS   (0xd208)
#define QTCE16_RAM_RD_DATA_REG_MS   (0xd20b)
#define QTCE16_RAM_RD_DATA_REG_LS   (0xd20a)
#define QTCE16_IS_RAM_ADDR_REG(reg) (reg == QTCE16_RAM_WR_ADDR_REG_MS || reg == QTCE16_RAM_WR_ADDR_REG_LS || reg == QTCE16_RAM_RD_ADDR_REG_MS || reg == QTCE16_RAM_RD_ADDR_REG_LS)
#define QTCE16_IS_RAM_DATA_REG(reg) (reg == QTCE16_RAM_WR_DATA_REG_MS || reg == QTCE16_RAM_WR_DATA_REG_LS || reg == QTCE16_RAM_RD_DATA_REG_MS || reg == QTCE16_RAM_RD_DATA_REG_LS)

#define QTCE16_SIM_ENTRY_F_RAM_LS_DATA_ENTRY (0x1)
#define QTCE16_SIM_ENTRY_F_RAM_MS_DATA_ENTRY (0x2)
#define QTCE16_SIM_ENTRY_F_RAM_LS_ADDR_ENTRY (0x4)
#define QTCE16_SIM_ENTRY_F_RAM_MS_ADDR_ENTRY (0x8)

#define QTCE16_UC_TABLE_RAM_BASE  (0x100)
#define QTCE16_UC_TABLE_RAM_SIZE  (56)
static uint16_t qtce16_uc_mem_table[QTCE16_UC_TABLE_RAM_SIZE] =
{
    0x6e49, 0x3566, 0x010c, 0x00d1, 0x0300, 0x0100, 0x2404, 0x0100, 0x0100, 0x2000, 0x0400, 0x2000,
    0x0,    0x0,    0x0500, 0x2000, 0x0008, 0x0,    0x0080, 0x0040, 0x0424, 0x2000, 0x0e00, 0x2000,
    0x0a00, 0x2000, 0x0010, 0x0,    0x0900, 0x2000, 0x0444, 0x2000, 0x0,    0x0,    0x0,    0x0,
    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
    0x1,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0
};
static int qtce16_ram_addr;

/* Forward declarations */
STATIC int
qtce16_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data);
STATIC int
qtce16_sim_read(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t *data);

STATIC uint32_t
qtce16_sim_default_data_get(uint32_t addr)
{
    uint32_t devad, reg;

    /*force FW load success */
    if(addr == QTCE16_SIM_FW_LOAD_DONE_REG_ADDR) {
        return (1 << 15);
    }

    devad = QTCE16_DEVAD_GET(addr);
    reg = QTCE16_REG_GET(addr);

    if (devad == 0) {
        switch (reg) {
        case 0x0002:
            return 0x600d;
        case 0x0003:
            return 0x8770;
        case 0x9007: /* MAIN_SERDESID */
            return 0x0316;
        case 0xc041: /* sc_x4_final_config_status_sp(0)_A */
        case 0xc051: /* sc_x4_final_config_status_sp(1)_A */
        case 0xc061: /* sc_x4_final_config_status_sp(2)_A */
        case 0xc071: /* sc_x4_final_config_status_sp(3)_A */
            return 0x20;
            /*
        case 0xc020:  sc_x4_control_control 
            return 0x1ff;
            */
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
        case 0xd0fa:
            return 0x403c;
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
            return 0x0271;
        case 0xd111:
            return 0x0020;
        case 0xd189:
            return 0x0007;
        case 0xd203:
            /* this is not the default value. value is changed to pass through
             * core init.
             */
            return 0x1;
        default:
            break;
        }
    }

    return 0;
}

STATIC uint32_t
qtce16_sim_reg_copies_get(uint32_t addr)
{
    uint32_t devad, reg;

    devad = QTCE16_DEVAD_GET(addr);
    reg = QTCE16_REG_GET(addr);

    if (reg == QTCE16_AER || reg == QTCE16_BLK) {
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
        if ((reg & 0xffff) == 0xd0b8) {
            return 1;
        }
        return 4;
    }
    return 0;
}

STATIC int
_qtce16_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
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
            DBG_VERB(("_qtce16_sim_write 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                      addr, pse->data));
            return PHYMOD_E_NONE;
        }
    }

    /* Check if database is full */
    if (pms_data->entries_used >= pms_data->num_entries) {
        return PHYMOD_E_RESOURCE;
    }

    /* Check if new data matches default value */
    if (data == qtce16_sim_default_data_get(addr)) {
        return PHYMOD_E_NONE;
    }

    /* Add new register value */
    pse = &pms_data->entries[pms_data->entries_used++];
    pse->addr = addr;
    pse->data = data;

    DBG_VERB(("_qtce16_sim_write 0x%08"PRIx32" = 0x%04"PRIx32" (new)\n",
              addr, pse->data));

    return PHYMOD_E_NONE;
}

STATIC uint32_t
qtce16_sim_write_adjust(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
{
    uint32_t devad, reg;
    uint32_t addr_tmp, data_tmp, data_r;
    uint32_t i, mask;

    devad = QTCE16_DEVAD_GET(addr);
    reg = QTCE16_REG_GET(addr);

    if (devad == 0) {
        switch (reg) {
        case 0xc020:
            
            /* Set SW_SPEED_CHANGE_DONE and SW_SPEED_CONFIG_VLD in status reg */
            qtce16_sim_write(pms_data, addr + 2, 0x3);
            /* Sync speed to  0xc040 sc_x4_final_config_status_sp0_resolved_speed */
            qtce16_sim_write(pms_data, addr + 0x20, data & 0xff);
            /* Set 0xc179 RX_X4_Status0_pcs_live_status and 0xc178 RX_X4_Status0_pcs_latched_status. force all ports link up */
            qtce16_sim_write(pms_data, addr + 0x158, 0x0);
            qtce16_sim_write(pms_data, addr + 0x159, 0xff);
            break;
        case 0xc021:
            mask = (data >> 16);
            if (mask == 0) {
                mask = 0xffff;
            }

            qtce16_sim_read(pms_data, addr, &data_r);
            data_r &= ~mask;
            data |= data_r;

            for (i = 0; i < 4; i++) {
                /* for QSGMII resolved speed. register 0xc041 */
                addr_tmp = addr + 0x20 + i*0x10;
                data_tmp = (data >> i*2) & 0x3; /* 0: 1G, 1: 100M, 2: 10M  for 0xc021*/
                data_tmp =  data_tmp == 0 ? 2 : data_tmp == 1 ? 0 : 1;  /* 0: 100M, 1: 10M, 2: 1G for 0xc041  */
                qtce16_sim_read(pms_data, addr_tmp, &data_r);
                qtce16_sim_write(pms_data, addr_tmp, ((data_r & 0xffcf) |  (data_tmp << 4)));

                /* for USXGMII resolved speed, register 0xc040 */
                addr_tmp = addr + 0x1f + i*0x10;
                data_tmp = (data >> i*2) & 0x3; /* 0: 1G, 1: 100M, 2: 10M, 3: USXGMII 2.5G  for 0xc021*/
                data_tmp =  data_tmp == 0 ? 3 : data_tmp == 1 ? 2 : data_tmp == 2 ? 1 : 0x48;  /* 1: 10M, 2: 100M, 3: 1G, 0x48: 2.5G for 0xc040  */
                qtce16_sim_read(pms_data, addr_tmp, &data_r);
                qtce16_sim_write(pms_data, addr_tmp, ((data_r & 0xff00) |  (data_tmp)));
             }
            break;
        default:
            break;
        }
    } else if (devad == 1) {
        switch (reg) {
        case 0xd127:
            /* Sync configured all lanes' PLL_CAL_CTL7 register. Only one PLL/VCO per core */
            /* Write lanes 0, 1, 2 and 3 */
            addr = QTCE16_ADDR(devad, 0, reg);
            (void)_qtce16_sim_write(pms_data, addr, data);

            addr = QTCE16_ADDR(devad, 1, reg);
            (void)_qtce16_sim_write(pms_data, addr, data);

            addr = QTCE16_ADDR(devad, 2, reg);
            (void)_qtce16_sim_write(pms_data, addr, data);

            addr = QTCE16_ADDR(devad, 3, reg);
            (void)_qtce16_sim_write(pms_data, addr, data);
            break;
         case 0xd201:
            /* Active uC (0xd0f4) when reset uC (0xd201) */
            if (data & 0x2) {
                qtce16_sim_read(pms_data, (addr - 0x10d), &data_tmp);
                _qtce16_sim_write(pms_data, (addr - 0x10d), (data_tmp | 0x8000));

                /* reset ram memory address */
                qtce16_ram_addr = 0;
            }
            break;
        case 0xd203:
            /* write data register to UC_RAM_WRDATA(0xd203)
             * read data register from UC_RAM_RDDATA(0xd204)
             */
            _qtce16_sim_write(pms_data, addr + 1, data);
            break;
        case 0xd0c1:
            /* Sync configured SIGDET_CTL1 register. When a port is set disable,
             * the port's RX_X4_PCS_LIVE_STS register deletes link status.
             * RX_X4_PCS_LIVE_STS register is PCS register, so need set devad to 0.
             * 0xF48 = 0xd0c1 - 0xc179*/
            qtce16_sim_read(pms_data, (addr - 0xF48) & 0x7ffff, &data_r);
            data_r = (data_r & 0xff00) | (((data & 0x80) == 0x80) ? 0 : 0xff);
            qtce16_sim_write(pms_data, (addr - 0xF48) & 0x7ffff, data_r);
            break;
        default:
            break;
        }
    }

    return data;
}

STATIC int
qtce16_sim_init(phymod_sim_data_t *pms_data,
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
qtce16_sim_reset(phymod_sim_data_t *pms_data)
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
qtce16_sim_read(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t *data)
{
    int idx;
    uint32_t aer, blk, devad, reg, copies;
    uint32_t lane = 0;
    phymod_sim_entry_t *pse;
    uint32_t addr_ms = 0, addr_ls = 0, abh_ctrl = 0;
    uint32_t pse_flag = 0;
    int increase_addr_auto = 0;

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    devad = 0;

    if (addr < QTCE16_BLK) {
        /* Assume clause 22 access */
        (void)qtce16_sim_read(pms_data, QTCE16_BLK, &blk);
        /* IEEE bit */
        if (addr & 0x10) {
            blk |= 0x8000;
        } else {
            blk &= ~0x8000;
        }
        addr = (addr & 0xf) | (blk & 0xfff0);
        if (addr != QTCE16_AER && addr != QTCE16_BLK) {
            (void)qtce16_sim_read(pms_data, QTCE16_AER, &aer);
            addr |= (aer << 16);
        }
    } else {
        /* Extract devad if clause 45 address format */
        if ((addr & QTCE16_CL45_MASK) == QTCE16_CL45) {
            devad = (addr >> 16) & 0x1f;
            addr &= 0xffff;
        }
    }

    if (addr != QTCE16_AER && addr != QTCE16_BLK) {
        /* Assume AER is in upper 16 bits */
        aer = (addr >> 16);
        if (aer == 0) {
            /* Try reading real AER instead */
            (void)qtce16_sim_read(pms_data, QTCE16_AER, &aer);
        }
        /* Add clause 45 devad (if used) */
        if (devad) {
            aer |= (devad << 11);
            addr = (addr & 0xffff) | (aer << 16);
        }
        lane = (aer & 0x7);
        if (lane > 3) {
            /* Force lane 0 if lane is invalid */
            addr = QTCE16_ADDR(QTCE16_DEVAD_GET(addr), 0, QTCE16_REG_GET(addr));
        }
    }

    /* Adjust lane according to number of copies */
    devad = QTCE16_DEVAD_GET(addr);
    reg = QTCE16_REG_GET(addr);
    copies = qtce16_sim_reg_copies_get(addr);
    if (copies == 1) {
        lane = 0;
    } else if (copies == 2) {
        lane &= ~0x1;
    }

    /*handle ram read/write*/
    if (QTCE16_IS_RAM_DATA_REG(reg) || QTCE16_IS_RAM_ADDR_REG(reg)) {

        if (QTCE16_IS_RAM_DATA_REG(reg)) {
            (void)qtce16_sim_read(pms_data, QTCE16_ADDR(devad, lane, QTCE16_RAM_AHB_CTRL), &abh_ctrl);
            increase_addr_auto = ((abh_ctrl >> 13) & 0x1);

            (void)qtce16_sim_read(pms_data, QTCE16_ADDR(devad, lane, QTCE16_RAM_RD_ADDR_REG_LS), &addr_ls);
            (void)qtce16_sim_read(pms_data, QTCE16_ADDR(devad, lane, QTCE16_RAM_RD_ADDR_REG_MS), &addr_ms);
            addr = addr_ls | (addr_ms << 16);

            if (qtce16_ram_addr == 0 ) {
                qtce16_ram_addr = addr;
            } else {
                if (increase_addr_auto) {
                    qtce16_ram_addr += 1;
                } else {
                    qtce16_ram_addr = 0;
                }
            }

            if (reg == QTCE16_RAM_RD_DATA_REG_LS || reg == QTCE16_RAM_WR_DATA_REG_LS) {
                pse_flag |= QTCE16_SIM_ENTRY_F_RAM_LS_DATA_ENTRY;
            } else {
                pse_flag |= QTCE16_SIM_ENTRY_F_RAM_MS_DATA_ENTRY;
            }
        } else { /*QTCE16_IS_RAM_ADDR_REG*/
            addr = QTCE16_ADDR(devad, lane, 0 /*dummy*/);
            if (reg == QTCE16_RAM_RD_ADDR_REG_LS || reg == QTCE16_RAM_WR_ADDR_REG_LS) {
                pse_flag |= QTCE16_SIM_ENTRY_F_RAM_LS_ADDR_ENTRY;
            } else {
                pse_flag |= QTCE16_SIM_ENTRY_F_RAM_MS_ADDR_ENTRY;
            }
        }

        /* Check if this register has been written already */
        for (idx = 0; idx < pms_data->entries_used; idx++) {
            pse = &pms_data->entries[idx];
            if ((pse->addr == addr) && (pse->flags == pse_flag)) {
                *data = pse->data;
                DBG_VERB(("qtce16_sim_read 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                          addr, *data));
                return PHYMOD_E_NONE;
            }
        }

        if((qtce16_ram_addr >= QTCE16_UC_TABLE_RAM_BASE) && (qtce16_ram_addr < (QTCE16_UC_TABLE_RAM_BASE + QTCE16_UC_TABLE_RAM_SIZE))) {
            *data = qtce16_uc_mem_table[qtce16_ram_addr - QTCE16_UC_TABLE_RAM_BASE];
        }else {
            *data = 0;
        }

        return PHYMOD_E_NONE;
    }

    addr = QTCE16_ADDR(devad, lane, reg);

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
            DBG_VERB(("qtce16_sim_read 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                      addr, *data));
            return PHYMOD_E_NONE;
        }
    }

    /* Return default value if register was never written */
    *data = qtce16_sim_default_data_get(addr);

    DBG_VERB(("qtce16_sim_read 0x%08"PRIx32" = [0x%04"PRIx32"]\n",
              addr, *data));

    return PHYMOD_E_NONE;
}

STATIC int
qtce16_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
{
    int idx;
    uint32_t aer, blk, devad, reg, copies;
    uint32_t lane = 0;
    phymod_sim_entry_t *pse;
    uint32_t pse_flag = 0;
    uint32_t addr_ms = 0, addr_ls = 0;

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    devad = 0;

    if (addr < QTCE16_BLK) {
        /* Assume clause 22 access */
        (void)qtce16_sim_read(pms_data, QTCE16_BLK, &blk);
        /* IEEE bit */
        if (addr & 0x10) {
            blk |= 0x8000;
        } else {
            blk &= ~0x8000;
        }
        addr = (addr & 0xf) | (blk & 0xfff0);
        if (addr != QTCE16_AER && addr != QTCE16_BLK) {
            (void)qtce16_sim_read(pms_data, QTCE16_AER, &aer);
            addr |= (aer << 16);
        }
    } else {
        /* Extract devad if clause 45 address format */
        if ((addr & QTCE16_CL45_MASK) == QTCE16_CL45) {
            devad = (addr >> 16) & 0x1f;
            addr &= 0xffff;
        }
    }

    if (addr != QTCE16_AER && addr != QTCE16_BLK) {
        /* Assume AER is in upper 16 bits */
        aer = (addr >> 16);
        if (aer == 0) {
            /* Try reading real AER instead */
            (void)qtce16_sim_read(pms_data, QTCE16_AER, &aer);
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
            reg = QTCE16_REG_GET(addr);
            devad = QTCE16_DEVAD_GET(addr);
            if (lane == 4 || lane == 6) {
                /* Write lanes 0 and 1 */
                addr = QTCE16_ADDR(devad, 8, reg);
                (void)qtce16_sim_write(pms_data, addr, data);
                addr = QTCE16_ADDR(devad, 1, reg);
                (void)qtce16_sim_write(pms_data, addr, data);
            }
            if (lane == 5 || lane == 6) {
                /* Write lanes 2 and 3 */
                addr = QTCE16_ADDR(devad, 2, reg);
                (void)qtce16_sim_write(pms_data, addr, data);
                addr = QTCE16_ADDR(devad, 3, reg);
                (void)qtce16_sim_write(pms_data, addr, data);
            }
            return PHYMOD_E_NONE;
        }
    }

    /* Adjust data and/or related registers */
    data = qtce16_sim_write_adjust(pms_data, addr, data);

    /* Adjust lane according to number of copies */
    devad = QTCE16_DEVAD_GET(addr);
    reg = QTCE16_REG_GET(addr);
    copies = qtce16_sim_reg_copies_get(addr);
    if (copies == 1) {
        lane = 0;
    } else if (copies == 2) {
        lane &= ~0x1;
    }

    if (QTCE16_IS_RAM_DATA_REG(reg) || QTCE16_IS_RAM_ADDR_REG(reg)) {

        if (QTCE16_IS_RAM_DATA_REG(reg)) {
            (void)qtce16_sim_read(pms_data, QTCE16_ADDR(devad, lane, QTCE16_RAM_RD_ADDR_REG_LS), &addr_ls);
            (void)qtce16_sim_read(pms_data, QTCE16_ADDR(devad, lane, QTCE16_RAM_RD_ADDR_REG_MS), &addr_ms);
            addr = addr_ls | (addr_ms << 16);
            if (reg == QTCE16_RAM_RD_DATA_REG_LS || reg == QTCE16_RAM_WR_DATA_REG_LS) {
                pse_flag |= QTCE16_SIM_ENTRY_F_RAM_LS_DATA_ENTRY;
            } else {
                pse_flag |= QTCE16_SIM_ENTRY_F_RAM_MS_DATA_ENTRY;
            }
        } else { /*QTCE16_IS_RAM_ADDR_REG*/
            addr = QTCE16_ADDR(devad, lane, 0/*dummy*/);
            if (reg == QTCE16_RAM_RD_ADDR_REG_LS || reg == QTCE16_RAM_WR_ADDR_REG_LS) {
                pse_flag |= QTCE16_SIM_ENTRY_F_RAM_LS_ADDR_ENTRY;
            } else {
                pse_flag |= QTCE16_SIM_ENTRY_F_RAM_MS_ADDR_ENTRY;
            }
        }

        /* Check if this register has been written already */
        for (idx = 0; idx < pms_data->entries_used; idx++) {
            pse = &pms_data->entries[idx];
            if ((pse->addr == addr) && (pse->flags == pse_flag)) {
                pse->data = data;
                DBG_VERB(("qtce16_sim_write 0x%08"PRIx32" = 0x%04"PRIx32" - flag = %u \n",
                          addr, pse->data, pse_flag));
                return PHYMOD_E_NONE;
            }
        }

        pse = &pms_data->entries[pms_data->entries_used++];
        pse->addr = addr;
        pse->data = data;
        pse->flags = pse_flag;

        DBG_VERB(("qtce16_sim_write 0x%08"PRIx32" = 0x%04"PRIx32"  - flag = %u (new)\n",
              addr, pse->data, pse->flags));

        return PHYMOD_E_NONE;
    }

    addr = QTCE16_ADDR(devad, lane, reg);

    return _qtce16_sim_write(pms_data, addr, data);;
}

STATIC int
qtce16_sim_event(phymod_sim_data_t *pms_data, phymod_sim_event_t event)
{
    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    return PHYMOD_E_NONE;
}

phymod_sim_drv_t qtce16_sim_drv = {
    qtce16_sim_init,
    qtce16_sim_reset,
    qtce16_sim_read,
    qtce16_sim_write,
    qtce16_sim_event
};

#endif /* EXCLUDE_PHYMOD_QTCE16_SIM */ 
