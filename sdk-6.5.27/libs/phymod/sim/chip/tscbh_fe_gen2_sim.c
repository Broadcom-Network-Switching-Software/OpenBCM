/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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
 *
 */

#include <phymod/phymod_system.h>
#include <phymod/phymod_sim.h>

/* Convenience macro */
#define DBG_VERB PHYMOD_DEBUG_VERBOSE

/* Bit field get/set macros */
#define TSCBH_FE_GEN2_BF_SET(_val, _mask, _shift) _val |= ((_mask) << (_shift))
#define TSCBH_FE_GEN2_BF_GET(_val, _mask, _shift) (((_val) >> (_shift)) & (_mask))

/*
 * Raw 32-bit address consists of AER value in upper 16 bits and
 * clause 45 address in lower 16 bits.
 */
#define TSCBH_FE_GEN2_DEVAD_SHIFT       27
#define TSCBH_FE_GEN2_DEVAD_MASK        0x1f
#define TSCBH_FE_GEN2_DEVAD_GET(_addr) \
    TSCBH_FE_GEN2_BF_GET(_addr, TSCBH_FE_GEN2_DEVAD_MASK, TSCBH_FE_GEN2_DEVAD_SHIFT)
#define TSCBH_FE_GEN2_LANE_SHIFT        16
#define TSCBH_FE_GEN2_LANE_MASK         0xff
#define TSCBH_FE_GEN2_LANE_GET(_addr) \
    TSCBH_FE_GEN2_BF_GET(_addr, TSCBH_FE_GEN2_LANE_MASK, TSCBH_FE_GEN2_LANE_SHIFT)
#define TSCBH_FE_GEN2_PLL_ID_SHIFT      24
#define TSCBH_FE_GEN2_PLL_ID_MASK       0x3
#define TSCBH_FE_GEN2_PLL_ID_GET(_addr) \
    TSCBH_FE_GEN2_BF_GET(_addr, TSCBH_FE_GEN2_PLL_ID_MASK, TSCBH_FE_GEN2_PLL_ID_SHIFT)
#define TSCBH_FE_GEN2_REG_SHIFT         0
#define TSCBH_FE_GEN2_REG_MASK          0xffff
#define TSCBH_FE_GEN2_REG_GET(_addr) \
    TSCBH_FE_GEN2_BF_GET(_addr, TSCBH_FE_GEN2_REG_MASK, TSCBH_FE_GEN2_REG_SHIFT)
#define TSCBH_FE_GEN2_MPP_ID_SHIFT      24
#define TSCBH_FE_GEN2_MPP_ID_MASK       0x3
#define TSCBH_FE_GEN2_MPP_ID_GET(_addr) \
    TSCBH_FE_GEN2_BF_GET(_addr, TSCBH_FE_GEN2_MPP_ID_MASK, TSCBH_FE_GEN2_MPP_ID_SHIFT)

#define TSCBH_FE_GEN2_ADDR(_devad, _pll_id, _lane, _reg) \
    (((_devad) << TSCBH_FE_GEN2_DEVAD_SHIFT) +  \
     ((_pll_id) << TSCBH_FE_GEN2_PLL_ID_SHIFT) +  \
     ((_lane) << TSCBH_FE_GEN2_LANE_SHIFT) +    \
     ((_reg) << TSCBH_FE_GEN2_REG_SHIFT))

#define TSCBH_FE_GEN2_ADDR_PCS(_devad, _mpp_id, _lane, _reg) \
   (((_devad) << TSCBH_FE_GEN2_DEVAD_SHIFT) + \
    ((_mpp_id) << TSCBH_FE_GEN2_MPP_ID_SHIFT) + \
    ((_lane) << TSCBH_FE_GEN2_LANE_SHIFT) + \
    ((_reg) << TSCBH_FE_GEN2_REG_SHIFT))

#define TSCBH_FE_GEN2_AER               TSCBH_FE_GEN2_ADDR(0, 0, 0, 0xffde)
#define TSCBH_FE_GEN2_BLK               TSCBH_FE_GEN2_ADDR(0, 0, 0, 0x001f)
#define TSCBH_NOF_LANES_IN_CORE         8

/*
 * The CL45 indicator is used to determine whether the upper 16 bits
 * of the address is an AER value or a clause 45 DEVAD.
 */
#define TSCBH_FE_GEN2_CL45              (0x20 << 16)
#define TSCBH_FE_GEN2_CL45_MASK         (0xe0 << 16)

#define TSCBH_FE_GEN2_ID0                 0x600d
#define TSCBH_FE_GEN2_ID1                 0x8770
#define TSCBH_FE_GEN2_MODEL               0x28
#define TSCBH_FE_GEN2_SERDES_ID           0x036f /* 0x9008 Main0_serdesID - Serdes ID Register */

#define TSCBH_FE_GEN2_MPP_0               0x1
#define TSCBH_FE_GEN2_MPP_1               0x2
#define TSCBH_FE_GEN2_MPP_0_1             0x3

#define TSCBH_FE_GEN2_SIM_FW_LOAD_DONE_REG_ADDR (0x800d203)

/*RAM sim*/
#define TSCBH_FE_GEN2_RAM_WR_ADDR_REG_MS   (0xd205)
#define TSCBH_FE_GEN2_RAM_WR_ADDR_REG_LS   (0xd204)
#define TSCBH_FE_GEN2_RAM_WR_DATA_REG_MS   (0xd207)
#define TSCBH_FE_GEN2_RAM_WR_DATA_REG_LS   (0xd206)
#define TSCBH_FE_GEN2_RAM_RD_ADDR_REG_MS   (0xd209)
#define TSCBH_FE_GEN2_RAM_RD_ADDR_REG_LS   (0xd208)
#define TSCBH_FE_GEN2_RAM_RD_DATA_REG_MS   (0xd20b)
#define TSCBH_FE_GEN2_RAM_RD_DATA_REG_LS   (0xd20a)
#define TSCBH_FE_GEN2_IS_RAM_ADDR_REG(reg) (reg == TSCBH_FE_GEN2_RAM_WR_ADDR_REG_MS || reg == TSCBH_FE_GEN2_RAM_WR_ADDR_REG_LS || reg == TSCBH_FE_GEN2_RAM_RD_ADDR_REG_MS || reg == TSCBH_FE_GEN2_RAM_RD_ADDR_REG_LS)
#define TSCBH_FE_GEN2_IS_RAM_DATA_REG(reg) (reg == TSCBH_FE_GEN2_RAM_WR_DATA_REG_MS || reg == TSCBH_FE_GEN2_RAM_WR_DATA_REG_LS || reg == TSCBH_FE_GEN2_RAM_RD_DATA_REG_MS || reg == TSCBH_FE_GEN2_RAM_RD_DATA_REG_LS)

#define TSCBH_FE_GEN2_SIM_ENTRY_F_RAM_LS_DATA_ENTRY (0x1)
#define TSCBH_FE_GEN2_SIM_ENTRY_F_RAM_MS_DATA_ENTRY (0x2)
#define TSCBH_FE_GEN2_SIM_ENTRY_F_RAM_LS_ADDR_ENTRY (0x4)
#define TSCBH_FE_GEN2_SIM_ENTRY_F_RAM_MS_ADDR_ENTRY (0x8)

#define TSCBH_FE_GEN2_RX_LANE_SWAP         (0x9225)
#define TSCBH_FE_GEN2_TX_LANE_SWAP         (0x9200)

#define TSCBH_FE_GEN2_PMD_IDX0             (4)
#define TSCBH_FE_GEN2_PMD_IDX1             (5)
#define TSCBH_FE_GEN2_PMD_IDX2             (6)
#define TSCBH_FE_GEN2_PMD_IDX3             (7)

/* Forward declarations */
STATIC int
tscbh_fe_gen2_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data);
STATIC int
tscbh_fe_gen2_sim_read(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t *data);


STATIC uint32_t
tscbh_fe_gen2_sim_default_data_get(uint32_t addr)
{
    uint32_t reg;

    reg = TSCBH_FE_GEN2_REG_GET(addr);

    /*force FW load success */
    if(addr == TSCBH_FE_GEN2_SIM_FW_LOAD_DONE_REG_ADDR) {
        return 1;
    }
    switch (reg) {
        case 0x0002:
            return TSCBH_FE_GEN2_ID0;
        case 0x0003:
            return TSCBH_FE_GEN2_ID1;
        case 0x9000:
            return 0x85;
        case 0x9008: /* Main0_serdesID - Serdes ID Register */
            return TSCBH_FE_GEN2_SERDES_ID;
        case 0xd03d:
            /*
             * This is DSC_A_DSC_UC_CTRL for Falcon. Make sure
             * The 'ready' bit is set.
             */
            return 0x0080;
        case 0xd0b9: /* RXTXCOM_CKRST_CTRL_sinai_LANE_DP_RESET_STATE_STATUS */
            return 0x7;
        case 0xd100:
            return TSCBH_FE_GEN2_MODEL;
        case 0xd10a: /* DIG_COM_REVID1 - REVID1 */
            return 0x203c;
        case 0xd147:
            return 0x7;
        case 0xd119: /* PLL_CONTROL_9 - AMS PLL CONTROL_9 */ 
            return 0x1800;
        case 0xd11a: /* PLL_CONTROL_10 - AMS PLL CONTROL_10 */ 
            return 0xb4;
        case 0xd11c: /* PLL_CONTROL_12 - AMS PLL CONTROL_12 */
            return 0x108; 
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
        case 0xd324: /* PLL_INTCTRL - INTERNAL PLL CONTROL */ 
            return 0x4;
        case 0xc051: /* SC_X4_STS */
            return 0x1;
        case 0xd19a:  /* Set AFE IP version */
            return 0x11;
        case 0x9200:  /* TX_X1_Control0_tx_lane_swap  */
            return 0x0688;
        case 0x9225:  /* RX_X1_Control0_rx_lane_swap   */
            return 0x0688;
        default:
            break;
    }

    return 0;
}

STATIC uint32_t
tscbh_fe_gen2_sim_write_adjust(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
{
    uint32_t devad, reg, val;

    devad = TSCBH_FE_GEN2_DEVAD_GET(addr);
    reg = TSCBH_FE_GEN2_REG_GET(addr);

    if (devad == 0) {
        switch (reg) {
        case 0xc050:
            /* Sync configured speed into final speed status register
             * 0x20 = 0xc070 - 0xc050 */
            tscbh_fe_gen2_sim_read(pms_data, addr + 0x20, &val);
            val = (val & 0xffff03ff) | ((data & 0x3f) << 10);
            tscbh_fe_gen2_sim_write(pms_data, addr + 0x20, val);
            break;
        default:
            break;
        }
    }

    return data;
}

STATIC int
_tscbh_fe_gen2_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
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
            DBG_VERB(("_tscbh_fe_gen2_sim_write 0x%08"PRIx32" = 0x%04"PRIx32"\n",
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
    if (data == tscbh_fe_gen2_sim_default_data_get(addr)) {
        return PHYMOD_E_NONE;
    }

    /* Add new register value */
    pse = &pms_data->entries[pms_data->entries_used++];
    pse->addr = addr;
    pse->data = data;

    DBG_VERB(("_tscbh_fe_gen2_sim_write 0x%08"PRIx32" = 0x%04"PRIx32" (new)\n",
              addr, pse->data));

    return PHYMOD_E_NONE;
}



STATIC uint32_t
tscbh_fe_gen2_sim_reg_copies_get(uint32_t addr)
{
    uint32_t devad, reg;

    devad = TSCBH_FE_GEN2_DEVAD_GET(addr);
    reg = TSCBH_FE_GEN2_REG_GET(addr);

    if (reg == TSCBH_FE_GEN2_AER || reg == TSCBH_FE_GEN2_BLK) {
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
    else {
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
        if ((reg & 0xfff0) == 0xd320) {
            return 2;
        }
        return 8; /* to simplify the per lane sim access */
    }
    return 0;
}

STATIC int
tscbh_fe_gen2_sim_init(phymod_sim_data_t *pms_data,
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
tscbh_fe_gen2_sim_reset(phymod_sim_data_t *pms_data)
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

/*
 * for PMD reg sim access only,
 * get the mapped physical lane for the input logical lane.
 */
STATIC int
_tscbh_gen2_sim_ll_to_pl_get(phymod_sim_data_t *pms_data,
                             uint32_t logical_lane,
                             uint32_t *physical_lane,
                             uint32_t *devad)
{
    /* pcs lane mapping */
    uint32_t pcs_map = 0, tx_swap = 0, lane = 0, lane_map_addr = 0, idx = 0;
    uint8_t lane_map_tx[TSCBH_NOF_LANES_IN_CORE];
    phymod_sim_entry_t *pse;

    /* read MPP0 TX lane_map */
    lane_map_addr = TSCBH_FE_GEN2_ADDR_PCS(0, TSCBH_FE_GEN2_MPP_0, 0, TSCBH_FE_GEN2_TX_LANE_SWAP);
    for (idx = 0; idx < pms_data->entries_used; idx++) {
        pse = &pms_data->entries[idx];
        if (pse->addr == lane_map_addr) {
            pcs_map = pse->data;
            break;
        }
    }
    /* Return default value if register was never written */
    if (pcs_map == 0x0) {
        pcs_map = tscbh_fe_gen2_sim_default_data_get(lane_map_addr);
    }
    tx_swap = (((pcs_map >> 0) & 0x7) << 0) |
        (((pcs_map >> 3) & 0x7) << 4) |
        (((pcs_map >> 6) & 0x7) << 8) |
        (((pcs_map >> 9) & 0x7) << 12) ;

    /* read MPP1 TX lane_map */
    pcs_map = 0;
    lane_map_addr = TSCBH_FE_GEN2_ADDR_PCS(0, TSCBH_FE_GEN2_MPP_1, 0, TSCBH_FE_GEN2_TX_LANE_SWAP);
    for (idx = 0; idx < pms_data->entries_used; idx++) {
        pse = &pms_data->entries[idx];
        if (pse->addr == lane_map_addr) {
            pcs_map = pse->data;
            break;
        }
    }
    /* Return default value if register was never written */
    if (pcs_map == 0x0) {
        pcs_map = tscbh_fe_gen2_sim_default_data_get(lane_map_addr);
    }
    lane = pcs_map >> 0 & 0x7;
    tx_swap |= ((lane >= 4) ? (lane - 4) : (lane + 4)) << 16;
    lane = pcs_map >> 3 & 0x7;
    tx_swap |= ((lane >= 4) ? (lane - 4) : (lane + 4)) << 20;
    lane = pcs_map >> 6 & 0x7;
    tx_swap |= ((lane >= 4) ? (lane - 4) : (lane + 4)) << 24;
    lane = pcs_map >> 9 & 0x7;
    tx_swap |= ((lane >= 4) ? (lane - 4) : (lane + 4)) << 28;

    /*decode each lane as four bits*/
    for (lane = 0; lane < TSCBH_NOF_LANES_IN_CORE; lane++){
        lane_map_tx[(tx_swap >> (lane*4)) & 0xf] = lane;
    }
    /* find corresponding physical lane */
    *physical_lane = lane_map_tx[logical_lane];
    /* find pmd instance */
    if ( (lane_map_tx[logical_lane] / 2) == 0 ) {
        *devad = TSCBH_FE_GEN2_PMD_IDX0;
    } else if ( (lane_map_tx[logical_lane] / 2) == 1 ) {
        *devad = TSCBH_FE_GEN2_PMD_IDX1;
    } else if ( (lane_map_tx[logical_lane] / 2) == 2 ) {
        *devad = TSCBH_FE_GEN2_PMD_IDX2;
    } else if ( (lane_map_tx[logical_lane] / 2) == 3 ) {
        *devad = TSCBH_FE_GEN2_PMD_IDX3;
    }

    return PHYMOD_E_NONE;
}

STATIC int
tscbh_fe_gen2_sim_read(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t *data)
{
    int idx;
    uint32_t aer, blk, devad, reg, copies, pll_id=0;
    uint32_t lane = 0, physical_lane = 0;
    uint32_t mpp_id=0;
    phymod_sim_entry_t *pse;
    uint32_t pse_flag = 0;
    uint32_t addr_ms, addr_ls;

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    devad = 0;

    if (addr < TSCBH_FE_GEN2_BLK) {
        /* Assume clause 22 access */
        (void)tscbh_fe_gen2_sim_read(pms_data, TSCBH_FE_GEN2_BLK, &blk);
        if (addr & 0x10) {
            /* IEEE bit */
            blk |= 0x8000;
        }
        addr = (addr & 0xf) | (blk & 0xfff0);
        if (addr != TSCBH_FE_GEN2_AER && addr != TSCBH_FE_GEN2_BLK) {
            (void)tscbh_fe_gen2_sim_read(pms_data, TSCBH_FE_GEN2_AER, &aer);
            addr |= (aer << 16);
        }
    } else {
        /* Extract devad if clause 45 address format */
        if ((addr & TSCBH_FE_GEN2_CL45_MASK) == TSCBH_FE_GEN2_CL45) {
            devad = (addr >> 16) & 0x1f;
            addr &= 0xffff;
        }
    }

    if (addr != TSCBH_FE_GEN2_AER && addr != TSCBH_FE_GEN2_BLK) {
        /* Assume AER is in upper 16 bits */
        aer = (addr >> 16);
        if (aer == 0) {
            /* Try reading real AER instead */
            (void)tscbh_fe_gen2_sim_read(pms_data, TSCBH_FE_GEN2_AER, &aer);
        }
        /* Add clause 45 devad (if used) */
        aer |= (devad << 11);
        addr = (addr & 0xffff) | (aer << 16);

        lane = (aer & 0x7);

        if (devad != 1) { /* PCS registers */
            mpp_id = TSCBH_FE_GEN2_MPP_ID_GET(addr);
            if (mpp_id == TSCBH_FE_GEN2_MPP_0_1) {
                /* Force mpp0 if mpp is invalid */
                addr = TSCBH_FE_GEN2_ADDR_PCS(TSCBH_FE_GEN2_DEVAD_GET(addr), TSCBH_FE_GEN2_MPP_0, lane, TSCBH_FE_GEN2_REG_GET(addr));
            }
            if (lane > 3) {
                /* Force lane 0 if lane is invalid */
                lane = 0;
                addr = TSCBH_FE_GEN2_ADDR_PCS(TSCBH_FE_GEN2_DEVAD_GET(addr), TSCBH_FE_GEN2_MPP_ID_GET(addr), lane, TSCBH_FE_GEN2_REG_GET(addr));
            }
        }
    }

    /* Adjust lane according to number of copies */
    devad = TSCBH_FE_GEN2_DEVAD_GET(addr);
    reg = TSCBH_FE_GEN2_REG_GET(addr);
    copies = tscbh_fe_gen2_sim_reg_copies_get(addr);
    if (devad == 1) { /* PMD register */
        /* get the pmd instance & physical lane that the logical lane is mapped to */
        (void)_tscbh_gen2_sim_ll_to_pl_get(pms_data, lane, &physical_lane, &devad);
        devad = physical_lane / 2 + 4;
        if (copies == 1) {
            lane = 0;
        } else if (copies == 2) { /* per PLL registers */
            pll_id = TSCBH_FE_GEN2_PLL_ID_GET(addr);
            lane = 0;
        } else if (copies == 8) {
            pll_id = 0; /* don't care for per lane register sim access */
            lane = physical_lane % 2;
        }
    } else { /* PCS register */
        mpp_id = TSCBH_FE_GEN2_MPP_ID_GET(addr);
        if (copies == 2) {
           lane = 0;
        }
    }

    /*handle ram read/write*/
    if (TSCBH_FE_GEN2_IS_RAM_DATA_REG(reg) || TSCBH_FE_GEN2_IS_RAM_ADDR_REG(reg)) {

        if (TSCBH_FE_GEN2_IS_RAM_DATA_REG(reg)) {
            (void)tscbh_fe_gen2_sim_read(pms_data, TSCBH_FE_GEN2_ADDR(devad, pll_id, lane, TSCBH_FE_GEN2_RAM_RD_ADDR_REG_LS), &addr_ls);
            (void)tscbh_fe_gen2_sim_read(pms_data, TSCBH_FE_GEN2_ADDR(devad, pll_id, lane, TSCBH_FE_GEN2_RAM_RD_ADDR_REG_MS), &addr_ms);
            addr = addr_ls | (addr_ms << 16);
            if (reg == TSCBH_FE_GEN2_RAM_RD_DATA_REG_LS || reg == TSCBH_FE_GEN2_RAM_WR_DATA_REG_LS) {
                pse_flag |= TSCBH_FE_GEN2_SIM_ENTRY_F_RAM_LS_DATA_ENTRY;
            } else {
                pse_flag |= TSCBH_FE_GEN2_SIM_ENTRY_F_RAM_MS_DATA_ENTRY;
            }
        } else { /*TSCBH_FE_GEN2_IS_RAM_ADDR_REG*/
            addr = TSCBH_FE_GEN2_ADDR(devad, 0, lane, 0 /*dummy*/);
            if (reg == TSCBH_FE_GEN2_RAM_RD_ADDR_REG_LS || reg == TSCBH_FE_GEN2_RAM_WR_ADDR_REG_LS) {
                pse_flag |= TSCBH_FE_GEN2_SIM_ENTRY_F_RAM_LS_DATA_ENTRY;
            } else {
                pse_flag |= TSCBH_FE_GEN2_SIM_ENTRY_F_RAM_MS_DATA_ENTRY;
            }
        }

        /* Check if this register has been written already */
        for (idx = 0; idx < pms_data->entries_used; idx++) {
            pse = &pms_data->entries[idx];
            if ((pse->addr == addr) && (pse->flags == pse_flag)) {
                *data = pse->data;
                DBG_VERB(("tscbh_fe_gen2_sim_read 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                          addr, *data));
                return PHYMOD_E_NONE;
            }
        }

        *data = 0;
        return PHYMOD_E_NONE;
    }


    if (devad > 0) {
        addr = TSCBH_FE_GEN2_ADDR(devad, pll_id, lane, reg);
    } else {
        addr = TSCBH_FE_GEN2_ADDR_PCS(devad, mpp_id, lane, reg);
    }

    /* Check if this register has been written already */
    for (idx = 0; idx < pms_data->entries_used; idx++) {
        pse = &pms_data->entries[idx];
        if ((pse->addr == addr) && (pse->flags == pse_flag)) {
            *data = pse->data;
            DBG_VERB(("tscbh_fe_gen2_sim_read 0x%08"PRIx32" = 0x%04"PRIx32"\n",
                      addr, *data));
            return PHYMOD_E_NONE;
        }
    }

    /* Return default value if register was never written */
    *data = tscbh_fe_gen2_sim_default_data_get(addr);
    DBG_VERB(("tscbh_fe_gen2_sim_read 0x%08"PRIx32" = [0x%04"PRIx32"]\n",
              addr, *data));

    return PHYMOD_E_NONE;
}

STATIC int
tscbh_fe_gen2_sim_write(phymod_sim_data_t *pms_data, uint32_t addr, uint32_t data)
{
    uint32_t aer, blk, devad, reg, copies, pll_id=0;
    uint32_t lane = 0, physical_lane = 0;
    uint32_t mpp_id;

    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    devad = 0;

    if (addr < TSCBH_FE_GEN2_BLK) {
        /* Assume clause 22 access */
        (void)tscbh_fe_gen2_sim_read(pms_data, TSCBH_FE_GEN2_BLK, &blk);
        /* IEEE bit */
        if (addr & 0x10) {
            blk |= 0x8000;
        } else {
            blk &= ~0x8000;
        }
        addr = (addr & 0xf) | (blk & 0xfff0);
        if (addr != TSCBH_FE_GEN2_AER && addr != TSCBH_FE_GEN2_BLK) {
            (void)tscbh_fe_gen2_sim_read(pms_data, TSCBH_FE_GEN2_AER, &aer);
            addr |= (aer << 16);
        }
    } else {
        /* Extract devad if clause 45 address format */
        if ((addr & TSCBH_FE_GEN2_CL45_MASK) == TSCBH_FE_GEN2_CL45) {
            devad = (addr >> 16) & 0x1f;
            addr &= 0xffff;
        }
    }

    if (addr != TSCBH_FE_GEN2_AER && addr != TSCBH_FE_GEN2_BLK) {
        /* Assume AER is in upper 16 bits */
        aer = (addr >> 16);
        if (aer == 0) {
            /* Try reading real AER instead */
            (void)tscbh_fe_gen2_sim_read(pms_data, TSCBH_FE_GEN2_AER, &aer);
        }

        /* Add clause 45 devad (if used) */
        aer |= (devad << 11);
        addr = (addr & 0xffff) | (aer << 16);
        devad = TSCBH_FE_GEN2_DEVAD_GET(addr);
        reg = TSCBH_FE_GEN2_REG_GET(addr);

        if (devad == 1) { /* Access PMD registers */
            lane = (aer & 0xff);
            if (lane > 7) {
                /*
                 * Handle lane broadcast: is not supported
                 */
                return PHYMOD_E_FAIL;
            }
        } else { /* Access PCS registers */
            mpp_id = TSCBH_FE_GEN2_MPP_ID_GET(addr);
            lane = aer & 0x7;

            if (mpp_id == TSCBH_FE_GEN2_MPP_0_1) {
                addr = TSCBH_FE_GEN2_ADDR_PCS(devad, TSCBH_FE_GEN2_MPP_0, lane, reg);
                (void)tscbh_fe_gen2_sim_write(pms_data, addr, data);
                addr = TSCBH_FE_GEN2_ADDR_PCS(devad, TSCBH_FE_GEN2_MPP_1, lane, reg);
                (void)tscbh_fe_gen2_sim_write(pms_data, addr, data);
                return PHYMOD_E_NONE;
            } else {
                if (lane > 3) {
                    if (lane == 0x4) {
                        /* Write lanes 0 and 1 */
                        addr = TSCBH_FE_GEN2_ADDR_PCS(devad, mpp_id, 0, reg);
                        (void)tscbh_fe_gen2_sim_write(pms_data, addr, data);
                        addr = TSCBH_FE_GEN2_ADDR_PCS(devad, mpp_id, 1, reg);
                        (void)tscbh_fe_gen2_sim_write(pms_data, addr, data);
                    }
                    if (lane == 0x5) {
                        /* Write lanes 2 and 3 */
                        addr = TSCBH_FE_GEN2_ADDR_PCS(devad, mpp_id, 2, reg);
                        (void)tscbh_fe_gen2_sim_write(pms_data, addr, data);
                        addr = TSCBH_FE_GEN2_ADDR_PCS(devad, mpp_id, 3, reg);
                        (void)tscbh_fe_gen2_sim_write(pms_data, addr, data);
                    }
                    if (lane == 0x6) {
                        /* Write lanes 0, 1, 2 and 3 */
                        addr = TSCBH_FE_GEN2_ADDR_PCS(devad, mpp_id, 0, reg);
                        (void)tscbh_fe_gen2_sim_write(pms_data, addr, data);
                        addr = TSCBH_FE_GEN2_ADDR_PCS(devad, mpp_id, 1, reg);
                        (void)tscbh_fe_gen2_sim_write(pms_data, addr, data);
                        addr = TSCBH_FE_GEN2_ADDR_PCS(devad, mpp_id, 2, reg);
                        (void)tscbh_fe_gen2_sim_write(pms_data, addr, data);
                        addr = TSCBH_FE_GEN2_ADDR_PCS(devad, mpp_id, 3, reg);
                        (void)tscbh_fe_gen2_sim_write(pms_data, addr, data);
                    }
                    if (lane == 0x7) {
                        /* Write lanes 0, 2 */
                        addr = TSCBH_FE_GEN2_ADDR_PCS(devad, mpp_id, 0, reg);
                        (void)tscbh_fe_gen2_sim_write(pms_data, addr, data);
                        addr = TSCBH_FE_GEN2_ADDR_PCS(devad, mpp_id, 2, reg);
                        (void)tscbh_fe_gen2_sim_write(pms_data, addr, data);
                    }
                    return PHYMOD_E_NONE;
                }
            }
        }
    }

    /* Adjust data and/or related registers */
    data = tscbh_fe_gen2_sim_write_adjust(pms_data, addr, data);

    /* Adjust lane according to number of copies */
    devad = TSCBH_FE_GEN2_DEVAD_GET(addr);
    reg = TSCBH_FE_GEN2_REG_GET(addr);
    copies = tscbh_fe_gen2_sim_reg_copies_get(addr);

    if (devad == 1) { /* PMD registers */
        (void)_tscbh_gen2_sim_ll_to_pl_get(pms_data, lane, &physical_lane, &devad);
        devad = physical_lane / 2 + 4;
        if (copies == 1) {
            lane = 0;
        } else if (copies == 2) { /* per PLL registers */
            pll_id = TSCBH_FE_GEN2_PLL_ID_GET(addr);
            lane = 0;
        } else if (copies == 8) {
            pll_id = 0; /* don't care for per lane register sim access */
            lane = physical_lane % 2;
        }
        addr = TSCBH_FE_GEN2_ADDR(devad, pll_id, lane, reg);
    } else { /* PCS registers */
        mpp_id = TSCBH_FE_GEN2_MPP_ID_GET(addr);
        if (copies == 2) {
            lane = 0;
        }
        addr = TSCBH_FE_GEN2_ADDR_PCS(devad, mpp_id, lane, reg);
    }

    return _tscbh_fe_gen2_sim_write(pms_data, addr, data);
}

STATIC int
tscbh_fe_gen2_sim_event(phymod_sim_data_t *pms_data, phymod_sim_event_t event)
{
    if (pms_data == NULL || pms_data->entries == NULL) {
        return PHYMOD_E_INIT;
    }

    return PHYMOD_E_NONE;
}

phymod_sim_drv_t tscbh_fe_gen2_sim_drv = {
    tscbh_fe_gen2_sim_init,
    tscbh_fe_gen2_sim_reset,
    tscbh_fe_gen2_sim_read,
    tscbh_fe_gen2_sim_write,
    NULL,
    NULL,
    tscbh_fe_gen2_sim_event
};
