/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */
#define NO_MCM 1
#include <soc/types.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/wb_engine.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/portmod_chain.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_defs.h>
#include <soc/mcm/memregs.h>
#include <soc/ua.h>

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM8X100_GEN2_SUPPORT

#include <soc/portmod/pm8x100_gen2.h>
#include <soc/portmod/pm8x100_gen2_shared.h>
#include <soc/portmod/pm8x100_gen2_internal.h>
#include <soc/portmod/dcccport.h>

#define PM_8x100_GEN2_INFO(pm_info) ((pm_info)->pm_data.pm8x100_gen2_db)

/* Warmboot variable defines - start */

#define PM8x100_GEN2_WB_BUFFER_VERSION        (1)

#define PM8x100_GEN2_IS_CORE_INITIALIZED_SET(unit, pm_info, is_core_initialized) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[isCoreInitialized],         \
                          &is_core_initialized)
#define PM8x100_GEN2_IS_CORE_INITIALIZED_GET(unit, pm_info, is_core_initialized) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[isCoreInitialized],         \
                          is_core_initialized)

#define PM8x100_GEN2_IS_ACTIVE_SET(unit, pm_info, is_active)   \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[isActive],\
                         &is_active)
#define PM8x100_GEN2_IS_ACTIVE_GET(unit, pm_info, is_active)    \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[isActive],\
                          is_active)

#define PM8x100_GEN2_TVCO_PLL_ACTIVE_LANE_BITMAP_SET(unit, pm_info, tvco_pll_active_lane_bitmap)\
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                            \
                          pm_info->wb_vars_ids[tvcopllActiveLaneBitmap],             \
                          &tvco_pll_active_lane_bitmap)
#define PM8x100_GEN2_TVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, tvco_pll_active_lane_bitmap)\
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                            \
                          pm_info->wb_vars_ids[tvcopllActiveLaneBitmap],             \
                          tvco_pll_active_lane_bitmap)

#define PM8x100_GEN2_TVCO_PLL_ADV_LANE_BITMAP_SET(unit, pm_info, tvco_pll_adv_lane_bitmap)\
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[tvcopllAdvLaneBitmap],          \
                          &tvco_pll_adv_lane_bitmap)
#define PM8x100_GEN2_TVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, tvco_pll_adv_lane_bitmap)\
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[tvcopllAdvLaneBitmap],          \
                          tvco_pll_adv_lane_bitmap)

#define PM8x100_GEN2_LANE2PORT_SET(unit, pm_info, lane, port)                  \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                   \
                          pm_info->wb_vars_ids[lane2portMap], &port, lane)
#define PM8x100_GEN2_LANE2PORT_GET(unit, pm_info, lane, port)                 \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                  \
                          pm_info->wb_vars_ids[lane2portMap], port, lane)

#define PM8x100_GEN2_AN_MODE_SET(unit, pm_info, an_mode, port_index)              \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[anMode], &an_mode, port_index)
#define PM8x100_GEN2_AN_MODE_GET(unit, pm_info, an_mode, port_index)             \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[anMode], an_mode, port_index)

#define PM8x100_GEN2_LANE2FEC_SET(unit, pm_info, lane, fec)                   \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                  \
                          pm_info->wb_vars_ids[lane2fecMap], &fec, lane)
#define PM8x100_GEN2_LANE2FEC_GET(unit, pm_info, lane, fec)                   \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                  \
                          pm_info->wb_vars_ids[lane2fecMap], fec, lane)

#define PM8x100_GEN2_PORT_IS_PCS_BYPASSED_SET(unit, pm_info, is_bypassed, port_index)             \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                                      \
                          pm_info->wb_vars_ids[portIsPcsBypassed], &is_bypassed, port_index)
#define PM8x100_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, is_bypassed, port_index)             \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                                      \
                          pm_info->wb_vars_ids[portIsPcsBypassed], is_bypassed, port_index)

#define PM8x100_GEN2_TS_ENABLE_PORT_COUNT_SET(unit, pm_info, ts_enable_port_count)   \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[tsEnablePortCount],\
                         &ts_enable_port_count)
#define PM8x100_GEN2_TS_ENABLE_PORT_COUNT_GET(unit, pm_info, ts_enable_port_count)    \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[tsEnablePortCount],\
                          ts_enable_port_count)

#define PM8x100_GEN2_TIMESYNC_CONFIG_SET(unit, pm_info, timesync_config, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[timesyncEnable], &timesync_config, port_index)
#define PM8x100_GEN2_TIMESYNC_CONFIG_GET(unit, pm_info, timesync_config, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[timesyncEnable], timesync_config, port_index)

#define PM8x100_GEN2_SPEED_ID_TABLE_STATUS_SET(unit, pm_info, speed_id_table_status)   \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[speedIdTableStatus],\
                         &speed_id_table_status)
#define PM8x100_GEN2_SPEED_ID_TABLE_STATUS_GET(unit, pm_info, speed_id_table_status)    \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[speedIdTableStatus],\
                          speed_id_table_status)

typedef enum pm8x100_gen2_wb_vars{
    isCoreInitialized,
    isActive,
    tvcopllActiveLaneBitmap,
    tvcopllAdvLaneBitmap,
    lane2portMap,
    anMode,
    lane2fecMap,
    tsEnablePortCount,
    timesyncEnable,
    portIsPcsBypassed,
    speedIdTableStatus
}pm8x100_gen2_wb_vars_t;

/* Warmboot variable defines - end */

/* This macro set devad to reg_addr if it is PMD phy address. */
#define PM8X100_GEN2_PHY_REG_SET(reg_addr) \
    do { \
        uint32 phy_reg_addr; \
        phy_reg_addr = reg_addr & 0xffff; \
        if ((phy_reg_addr <= 0x01ff) || \
            (0x1000 <= phy_reg_addr && phy_reg_addr <= 0xdfff) || \
            (0xffd0 <= phy_reg_addr && phy_reg_addr <= 0xffdf)) { \
            reg_addr |= 1 << 16; \
        } \
    } while (0)


#define PM8X100_GEN2_MAX_NUM_PLLS (1)

#define PM8x100_GEN2_VCO_ALL (PM8x100_GEN2_VCO_ETH_51P5625G | PM8x100_GEN2_VCO_ETH_53P125G)

#define PM8x100_GEN2_VCO_BMP_EMPTY(bmp) \
        ( (bmp & PM8x100_GEN2_VCO_ALL) == 0 )


/* Highest NRZ per lane speed is 28.125G, supported in ILKN mode */
#define PM8X100_GEN2_NRZ_LANE_SPEED_MAX 28125
#define PM8X100_GEN2_50G_PAM4_LANE_SPEED_MAX 50000

#define PM8x100_GEN2_AN_ABILITY_TABLE_SIZE 30
#define PM8x100_GEN2_FS_ABILITY_TABLE_SIZE 32
#define PM8X100_GEN2_MAX_AN_ABILITY 20

STATIC portmod_ucode_buf_t pm8x100_gen2_ucode_buf[SOC_MAX_NUM_DEVICES] = {{NULL, 0}};
STATIC portmod_ucode_buf_t pm8x100_gen2_ucode_buf_2nd[SOC_MAX_NUM_DEVICES] = {{NULL, 0}};

/* register per logical or physical port number accessing */
#define PM8X100_GEN2_REG_READ(unit, reg_name, phy_acc, reg_val) \
            ((phy_acc & SOC_REG_ADDR_PHY_ACC_MASK) ?\
             UA_REG_READ_PHY_PORT(unit, reg_name, phy_acc & (~SOC_REG_ADDR_PHY_ACC_MASK), 0, reg_val) : \
             UA_REG_READ_PORT(unit, reg_name, phy_acc, 0, reg_val))

#define PM8X100_GEN2_REG_WRITE(unit, reg_name, phy_acc, reg_val) \
            ((phy_acc & SOC_REG_ADDR_PHY_ACC_MASK) ?\
             UA_REG_WRITE_PHY_PORT(unit, reg_name, phy_acc & (~SOC_REG_ADDR_PHY_ACC_MASK), 0, reg_val) : \
             UA_REG_WRITE_PORT(unit, reg_name, phy_acc, 0, reg_val))

/*
 * Entries of the force speed ability table; each entry specifies a
 * unique FS port speed ability and its associated VCO rate
 */
typedef struct pm8x100_gen2_fs_ability_table_entry_s {
    uint32 speed; /* port speed in Mbps */
    uint32 num_lanes; /* number of lanes */
    portmod_fec_t fec_type; /* FEC type */
    portmod_vco_type_t vco; /* associated VCO rate of the ability */
} pm8x100_gen2_fs_ability_table_entry_t;

/*
 * Entries of the autoneg ability table; each entry specifies a unique
 * AN speed ability and its associated VCO rate
 */
typedef struct pm8x100_gen2_an_ability_table_entry_s {
    uint32 speed; /* port speed in Mbps */
    uint32 num_lanes; /* number of lanes */
    portmod_fec_t fec_type; /* FEC type */
    portmod_port_phy_control_autoneg_mode_t an_mode; /* autoneg mode such as cl73, bam or msa */
    portmod_vco_type_t vco; /* associated VCO rate of the ability */
} pm8x100_gen2_an_ability_table_entry_t;

/* a comprehensive list of pm8x100 gen2 forced speed abilities and their VCO rates */
const pm8x100_gen2_fs_ability_table_entry_t pm8x100_gen2_fs_ability_table[PM8x100_GEN2_FS_ABILITY_TABLE_SIZE] =
{
    /* port_speed, num_lanes, fec_type, vco */
    { 10000, 1, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO51P5625G },
    { 25000, 1, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO51P5625G },
    { 25000, 1, PORTMOD_PORT_PHY_FEC_RS_FEC,     portmodVCO51P5625G },
    { 50000, 1, PORTMOD_PORT_PHY_FEC_RS_FEC,     portmodVCO51P5625G },
    { 50000, 2, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO51P5625G },
    { 50000, 2, PORTMOD_PORT_PHY_FEC_RS_FEC,     portmodVCO51P5625G },
    {100000, 2, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO51P5625G },
    {100000, 2, PORTMOD_PORT_PHY_FEC_RS_FEC,     portmodVCO51P5625G },
    {100000, 4, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO51P5625G },
    {100000, 4, PORTMOD_PORT_PHY_FEC_RS_FEC,     portmodVCO51P5625G },
    {200000, 4, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO51P5625G },
    { 50000, 1, PORTMOD_PORT_PHY_FEC_RS_544,     portmodVCO53P125G },
    { 50000, 1, PORTMOD_PORT_PHY_FEC_RS_272,     portmodVCO53P125G },
    { 50000, 2, PORTMOD_PORT_PHY_FEC_RS_544,     portmodVCO53P125G },
    {100000, 1, PORTMOD_PORT_PHY_FEC_RS_544,     portmodVCO53P125G },
    {100000, 1, PORTMOD_PORT_PHY_FEC_RS_544_2XN, portmodVCO53P125G },
    {100000, 1, PORTMOD_PORT_PHY_FEC_RS_272,     portmodVCO53P125G },
    {100000, 2, PORTMOD_PORT_PHY_FEC_RS_544,     portmodVCO53P125G },
    {100000, 2, PORTMOD_PORT_PHY_FEC_RS_272,     portmodVCO53P125G },
    {200000, 2, PORTMOD_PORT_PHY_FEC_RS_544,     portmodVCO53P125G },
    {200000, 2, PORTMOD_PORT_PHY_FEC_RS_544_2XN, portmodVCO53P125G },
    {200000, 2, PORTMOD_PORT_PHY_FEC_RS_272,     portmodVCO53P125G },
    {200000, 2, PORTMOD_PORT_PHY_FEC_RS_272_2XN, portmodVCO53P125G },
    {200000, 4, PORTMOD_PORT_PHY_FEC_RS_544,     portmodVCO53P125G },
    {200000, 4, PORTMOD_PORT_PHY_FEC_RS_544_2XN, portmodVCO53P125G },
    {200000, 4, PORTMOD_PORT_PHY_FEC_RS_272,     portmodVCO53P125G },
    {200000, 4, PORTMOD_PORT_PHY_FEC_RS_272_2XN, portmodVCO53P125G },
    {400000, 4, PORTMOD_PORT_PHY_FEC_RS_544_2XN, portmodVCO53P125G },
    {400000, 4, PORTMOD_PORT_PHY_FEC_RS_272_2XN, portmodVCO53P125G },
    {400000, 8, PORTMOD_PORT_PHY_FEC_RS_544_2XN, portmodVCO53P125G },
    {400000, 8, PORTMOD_PORT_PHY_FEC_RS_272_2XN, portmodVCO53P125G },
    {800000, 8, PORTMOD_PORT_PHY_FEC_RS_544_2XN, portmodVCO53P125G }
};


/* a comprehensive list of pm8x100 autoneg abilities and their VCO rates */
const pm8x100_gen2_an_ability_table_entry_t pm8x100_gen2_an_ability_table[PM8x100_GEN2_AN_ABILITY_TABLE_SIZE] =
{
    /* port_speed, num_lanes, fec_type, autoneg_mode, vco */
    {25000,  1, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73,     portmodVCO51P5625G},
    {25000,  1, PORTMOD_PORT_PHY_FEC_RS_FEC,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73,     portmodVCO51P5625G},
    {25000,  1, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_MSA, portmodVCO51P5625G},
    {25000,  1, PORTMOD_PORT_PHY_FEC_RS_FEC,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_MSA, portmodVCO51P5625G},
    {25000,  1, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO51P5625G},
    {25000,  1, PORTMOD_PORT_PHY_FEC_RS_FEC,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO51P5625G},
    {50000,  1, PORTMOD_PORT_PHY_FEC_RS_FEC,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO51P5625G},
    {50000,  2, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_MSA, portmodVCO51P5625G},
    {50000,  2, PORTMOD_PORT_PHY_FEC_RS_FEC,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_MSA, portmodVCO51P5625G},
    {50000,  2, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO51P5625G},
    {50000,  2, PORTMOD_PORT_PHY_FEC_RS_FEC,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO51P5625G},
    {100000, 2, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO51P5625G},
    {100000, 2, PORTMOD_PORT_PHY_FEC_RS_FEC,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO51P5625G},
    {100000, 4, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO51P5625G},
    {100000, 4, PORTMOD_PORT_PHY_FEC_RS_FEC,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73,     portmodVCO51P5625G},
    {200000, 4, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO51P5625G},
    {50000,  1, PORTMOD_PORT_PHY_FEC_RS_544,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73,     portmodVCO53P125G},
    {50000,  1, PORTMOD_PORT_PHY_FEC_RS_272,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_MSA, portmodVCO53P125G},
    {50000,  2, PORTMOD_PORT_PHY_FEC_RS_544,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO53P125G},
    {100000, 1, PORTMOD_PORT_PHY_FEC_RS_544_2XN, PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73,     portmodVCO53P125G},
    {100000, 1, PORTMOD_PORT_PHY_FEC_RS_544,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73,     portmodVCO53P125G},
    {100000, 2, PORTMOD_PORT_PHY_FEC_RS_544,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73,     portmodVCO53P125G},
    {100000, 2, PORTMOD_PORT_PHY_FEC_RS_272,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_MSA, portmodVCO53P125G},
    {200000, 2, PORTMOD_PORT_PHY_FEC_RS_544_2XN, PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73,     portmodVCO53P125G},
    {200000, 4, PORTMOD_PORT_PHY_FEC_RS_544_2XN, PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73,     portmodVCO53P125G},
    {200000, 4, PORTMOD_PORT_PHY_FEC_RS_544,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO53P125G},
    {200000, 4, PORTMOD_PORT_PHY_FEC_RS_272_2XN, PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_MSA, portmodVCO53P125G},
    {400000, 4, PORTMOD_PORT_PHY_FEC_RS_544_2XN, PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73,     portmodVCO53P125G},
    {400000, 8, PORTMOD_PORT_PHY_FEC_RS_544_2XN, PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_MSA, portmodVCO53P125G},
    {800000, 8, PORTMOD_PORT_PHY_FEC_RS_544_2XN, PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO53P125G}
};

static phymod_dispatch_type_t pm8x100_gen2_serdes_list[] =
{
#ifdef PHYMOD_TSCP_SUPPORT
    phymodDispatchTypeTscp,
#endif
    phymodDispatchTypeInvalid
};


/*
 * given the port speed, number of lanes, and FEC type, return the VCO specified in
 * pm8x100_gen2_fs_ability_table in the associated entry; VCO should be portmodVCOInvalid
 * if the ability is invalid
 */
STATIC
int _pm8x100_gen2_fs_ability_table_vco_get(uint32 speed, uint32 num_lanes, portmod_fec_t fec_type,
                                           portmod_vco_type_t* vco)
{
    int i;

    *vco = portmodVCOInvalid;
    for (i = 0; i < PM8x100_GEN2_FS_ABILITY_TABLE_SIZE; i++) {
        if (pm8x100_gen2_fs_ability_table[i].speed == speed &&
            pm8x100_gen2_fs_ability_table[i].num_lanes == num_lanes &&
            pm8x100_gen2_fs_ability_table[i].fec_type == fec_type)
        {
            *vco = pm8x100_gen2_fs_ability_table[i].vco;
            break;
        }
    }

    return SOC_E_NONE;
}

/*
 * given the port speed, number of lanes, FEC type, and autoneg mode, return the VCO specified in
 * pm8x100_gen2_an_ability_table; VCO should be portmodVCOInvalid if the ability is invalid.
 * this function is temporarily masked out because there is no caller. advert_ability_get() will
 * need to call this function to verify the VCO requirement
 */
STATIC
int _pm8x100_gen2_an_ability_table_vco_get(uint32 speed, uint32 num_lanes, portmod_fec_t fec_type,
                                           portmod_port_phy_control_autoneg_mode_t an_mode,
                                           portmod_vco_type_t* vco)
{
    int i;

    *vco = portmodVCOInvalid;
    for (i = 0; i < PM8x100_GEN2_AN_ABILITY_TABLE_SIZE; i++) {
        if (pm8x100_gen2_an_ability_table[i].speed == speed &&
            pm8x100_gen2_an_ability_table[i].num_lanes == num_lanes &&
            pm8x100_gen2_an_ability_table[i].fec_type == fec_type &&
            pm8x100_gen2_an_ability_table[i].an_mode == an_mode)
        {
            *vco = pm8x100_gen2_an_ability_table[i].vco;
            break;
        }
    }

    return SOC_E_NONE;
}

STATIC
int _pm8x100_gen2_port_index_get(int unit, int port, pm_info_t pm_info,
                                 int *first_index, uint32_t *bitmap)
{
   int i, rv = 0, tmp_port = 0;

   SOC_INIT_FUNC_DEFS;

   *first_index = -1;
   *bitmap = 0;

   for( i = 0 ; i < MAX_PORTS_PER_PM8X100_GEN2; i++){
       rv = PM8x100_GEN2_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
       _SOC_IF_ERR_EXIT(rv);

       if(tmp_port == port){
           *first_index = (*first_index == -1 ? i : *first_index);
           SHR_BITSET(bitmap, i);
       }
   }

   if(*first_index == -1) {
       _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
              (_SOC_MSG("port was not found in internal DB %d"), port));
   }

exit:
    SOC_FUNC_RETURN;
}

STATIC int
_pm8x100_gen2_phy_access_get(int unit,
                             int port,
                             pm_info_t pm_info,
                             int *phy_acc)
{
    uint32 is_bypassed = 0;
    int port_index;
    uint32 bitmap[1];

    SOC_INIT_FUNC_DEFS;

    *phy_acc = 0;
    _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_index_get(unit, port, pm_info, &port_index, bitmap));
    _SOC_IF_ERR_EXIT(PM8x100_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));
    if (is_bypassed) {
        *phy_acc = (PM_8x100_GEN2_INFO(pm_info)->first_phy + port_index) | SOC_REG_ADDR_PHY_ACC_MASK;
    } else {
        *phy_acc = port;
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm8x100_gen2_port_interrupt_all_enable_set(int unit, int port, int enable)
{
    int i = 0;
    portmod_intr_type_t intrs[] = {
                                    portmodIntrTypeTxPfcFifoOverflow,
                                    portmodIntrTypeRxPfcFifoOverflow,
                                    portmodIntrTypeFdrInterrupt,
                                    portmodIntrTypeTxPktUnderflow,
                                    portmodIntrTypeTxPktOverflow,
                                    portmodIntrTypeTxCdcSingleBitErr,
                                    portmodIntrTypeTxCdcDoubleBitErr,
                                    portmodIntrTypeMibMemSingleBitErr,
                                    portmodIntrTypeMibMemDoubleBitErr,
                                    portmodIntrTypeMibMemMultipleBitErr
                                  };
    SOC_INIT_FUNC_DEFS;

    for (i = 0; i < sizeof(intrs)/sizeof(intrs[0]); i++) {
        _SOC_IF_ERR_EXIT(
        portmod_port_interrupt_enable_set(unit, port, intrs[i], enable));
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
void _pm8x100_gen2_signalling_method_get(int is_bypassed,
                                         const portmod_speed_config_t *speed_config,
                                         phymod_phy_signalling_method_t *signalling_method)
{
    int lane_rate;

    if (!is_bypassed) {
        lane_rate = speed_config->speed / speed_config->num_lane;
    } else {
        lane_rate = speed_config->speed;
    }

    *signalling_method = lane_rate > PM8X100_GEN2_50G_PAM4_LANE_SPEED_MAX ? \
                        phymodSignallingMethodPAM4 : \
                        lane_rate > PM8X100_GEN2_NRZ_LANE_SPEED_MAX ? \
                        phymodSignallingMethod50gPAM4 : phymodSignallingMethodNRZ;
}

/*
 * Get PM active Ethernet port lanes bitmap.
 *
 *    This API doesn't consider the ports whose speeds
 *    are not configured.
 *
 * Inputs:
 *     unit:          unit number;
 *     pm_info:       pm_info data structure;
 *
 * Output:
 *     eth_active_lanes:   active ethernet lane bitmap
 */

STATIC
int _pm8x100_gen2_pm_active_eth_lanes_get(int unit,
                                          pm_info_t pm_info,
                                          uint8_t * eth_active_lanes)
{
    int tmp_port, i, is_pcs_bypassed = 0;
    uint8_t tvco_pll_active_lane_bitmap;
    uint8_t tvco_pll_adv_lane_bitmap;
    uint8_t all_active_lane_bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(
        PM8x100_GEN2_TVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_active_lane_bitmap));
    _SOC_IF_ERR_EXIT(
        PM8x100_GEN2_TVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_adv_lane_bitmap));

    all_active_lane_bitmap = (tvco_pll_active_lane_bitmap | tvco_pll_adv_lane_bitmap);

    *eth_active_lanes = 0;
    for(i = 0 ; i < MAX_PORTS_PER_PM8X100_GEN2; i++) {
        /* Exclude the ports whose speeds are not configured */
        if (all_active_lane_bitmap & (1 << i)) {
            _SOC_IF_ERR_EXIT(PM8x100_GEN2_LANE2PORT_GET(unit, pm_info, i, &tmp_port));
            if (tmp_port >= 0) {
                _SOC_IF_ERR_EXIT(PM8x100_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_pcs_bypassed, i));
                if (!is_pcs_bypassed) {
                    *eth_active_lanes |= (1 << i);
                }
            }
        }
    }
exit:
    SOC_FUNC_RETURN;
}

/*Get whether the inerface type is supported by PM */
int pm8x100_gen2_pm_interface_type_is_supported(int unit,
                                                soc_port_if_t interface,
                                                int* is_supported)
{

    switch(interface){
    case SOC_PORT_IF_ILKN:
        *is_supported = FALSE;
        break;
    default:
        *is_supported = TRUE;
    }

    return (SOC_E_NONE);
}

STATIC int
pm8x100_gen2_default_fw_loader(const phymod_core_access_t* core, uint32_t length, const uint8_t* data)
{
    int rv, bcast_en = 0;
    portmod_default_user_access_t *user_data;
    int unit = ((portmod_default_user_access_t*)core->access.user_acc)->unit;
    portmod_ucode_buf_order_t load_order;
    SOC_INIT_FUNC_DEFS;

    user_data = (portmod_default_user_access_t*)core->access.user_acc;

    if (PORTMOD_USER_ACCESS_FW_LOAD_REVERSE_GET(user_data)) {
        load_order = portmod_ucode_buf_order_reversed;
    } else {
        load_order = portmod_ucode_buf_order_straight;
    }

#ifdef BCM_ACCESS_SUPPORT
    rv = portmod_firmware_set_access(unit,
                              user_data->blk_id,
                              data,
                              length,
                              load_order,
                              bcast_en,
                              &(pm8x100_gen2_ucode_buf[unit]),
                              &(pm8x100_gen2_ucode_buf_2nd[unit]),
                              mDC3PORT_TSC_UCMEM_DATA,
                              rDC3PORT_TSC_MEM_CTRL_fACCESS_MODE);
#else
    rv = portmod_firmware_set(unit,
                              user_data->blk_id,
                              data,
                              length,
                              load_order,
                              bcast_en,
                              &(pm8x100_gen2_ucode_buf[unit]),
                              &(pm8x100_gen2_ucode_buf_2nd[unit]),
                              DC3PORT_TSC_UCMEM_DATAm,
                              DC3PORT_TSC_MEM_CTRLr);
#endif
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}


STATIC
int
pm8x100_gen2_default_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr,
                               uint32_t val)
{
    /* will force the core_addr to zero since it's  NOT being used */
#ifdef BCM_ACCESS_SUPPORT
    return portmod_common_phy_sbus_reg_write_access(mDC3PORT_TSC_UCMEM_DATA, user_acc,
                                                    0x0, reg_addr, val);
#else
    return portmod_common_phy_sbus_reg_write(DC3PORT_TSC_UCMEM_DATAm, user_acc,
                                             0x0, reg_addr, val);
#endif
}

STATIC
int
pm8x100_gen2_default_mem_write(void* user_acc,
                               phymod_mem_type_t mem_type,
                               uint32_t mem_index,
                               const uint32_t* val)
{
    int rv = SOC_E_NONE;
    soc_mem_t tsc_mem_data;
    const uint32_t *array_ptr;
    portmod_default_user_access_t *user_data = user_acc;

    if(user_data == NULL){
        return SOC_E_PARAM;
    }

    /* sanity check on the mem_type */
    if (mem_type >= phymodMemCount) {
        return SOC_E_PARAM;
    }

    /*next check if simulation */
    if (SAL_BOOT_QUICKTURN) {
        return SOC_E_NONE;
    }

    /* next need to map to the correct memory */
    switch (mem_type) {
#ifdef BCM_ACCESS_SUPPORT
    case phymodMemSpeedIdTable:
        tsc_mem_data = mSPEED_ID_TABLE;
        break;
    case phymodMemAMTable:
        tsc_mem_data = mAM_TABLE;
        break;
    case phymodMemUMTable:
        tsc_mem_data = mUM_TABLE;
        break;
    case phymodMemTxLkup1588Mpp0:
        tsc_mem_data = mTX_LKUP_1588_MEM_MPP0;
        break;
    case phymodMemTxLkup1588Mpp1:
        tsc_mem_data = mTX_LKUP_1588_MEM_MPP1;
        break;
    case phymodMemRxLkup1588Mpp0:
        tsc_mem_data = mRX_LKUP_1588_MEM_MPP0;
        break;
    case phymodMemRxLkup1588Mpp1:
        tsc_mem_data = mRX_LKUP_1588_MEM_MPP1;
        break;
    case phymodMemSpeedPriorityMapTable:
        tsc_mem_data = mSPEED_PRIORITY_MAP_TBL;
        break;
    case phymodMemRsFecSymbErrMib:
        tsc_mem_data = mRSFEC_SYMBOL_ERROR_MIB;
        break;
#else
    case phymodMemSpeedIdTable:
        tsc_mem_data = SPEED_ID_TABLEm;
        break;
    case phymodMemAMTable:
        tsc_mem_data = AM_TABLEm;
        break;
    case phymodMemUMTable:
        tsc_mem_data = UM_TABLEm;
        break;
    case phymodMemTxLkup1588Mpp0:
        tsc_mem_data = TX_LKUP_1588_MEM_MPP0m;
        break;
    case phymodMemTxLkup1588Mpp1:
        tsc_mem_data = TX_LKUP_1588_MEM_MPP1m;
        break;
    case phymodMemRxLkup1588Mpp0:
        tsc_mem_data = RX_LKUP_1588_MEM_MPP0m;
        break;
    case phymodMemRxLkup1588Mpp1:
        tsc_mem_data = RX_LKUP_1588_MEM_MPP1m;
        break;
    case phymodMemSpeedPriorityMapTable:
        tsc_mem_data = SPEED_PRIORITY_MAP_TBLm;
        break;
    case phymodMemRsFecSymbErrMib:
        tsc_mem_data = RSFEC_SYMBOL_ERROR_MIBm;
        break;
#endif
    default:
        return SOC_E_PARAM;
    }
    array_ptr = val;

#ifdef BCM_ACCESS_SUPPORT
    rv = access_regmem(user_data->unit, FLAG_ACCESS_IS_WRITE, tsc_mem_data, user_data->blk_id, 0, mem_index, (void *) &array_ptr[0]);
#else
    rv = soc_mem_write(user_data->unit, tsc_mem_data, user_data->blk_id, mem_index, (void *) &array_ptr[0]);
#endif

    return rv;
}


STATIC
int
pm8x100_gen2_default_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr,
                              uint32_t *val)
{
    int rv = SOC_E_NONE;

    /* will force the core_addr to zero since it's NOT being used */
#ifdef BCM_ACCESS_SUPPORT
    rv = portmod_common_phy_sbus_reg_read_access(mDC3PORT_TSC_UCMEM_DATA, user_acc,
                                                 0x0, reg_addr, val);
#else
    rv = portmod_common_phy_sbus_reg_read(DC3PORT_TSC_UCMEM_DATAm, user_acc,
                                            0x0, reg_addr, val);
#endif
    
    /* needs to down shift read by 16 bit */
    *val = (*val >> 16) & 0xffff;
    return (rv);
}

STATIC
int
pm8x100_gen2_default_mem_read(void* user_acc,
                              phymod_mem_type_t mem_type,
                              uint32_t mem_index,
                              uint32_t* val)
{
    int rv = SOC_E_NONE;
    soc_mem_t tsc_mem_data;
    portmod_default_user_access_t *user_data = user_acc;

    if(user_data == NULL){
        return SOC_E_PARAM;
    }
    /* sanity check on the mem_type */
    if (mem_type >= phymodMemCount) {
        return SOC_E_PARAM;
    }

    /*next check if simulation */
    if (SAL_BOOT_QUICKTURN) {
        return SOC_E_NONE;
    }

    /* next need to map to the correct memory */
    switch (mem_type) {
#ifdef BCM_ACCESS_SUPPORT
    case phymodMemSpeedIdTable:
        tsc_mem_data = mSPEED_ID_TABLE;
        break;
    case phymodMemAMTable:
        tsc_mem_data = mAM_TABLE;
        break;
    case phymodMemUMTable:
        tsc_mem_data = mUM_TABLE;
        break;
    case phymodMemTxLkup1588Mpp0:
        tsc_mem_data = mTX_LKUP_1588_MEM_MPP0;
        break;
    case phymodMemTxLkup1588Mpp1:
        tsc_mem_data = mTX_LKUP_1588_MEM_MPP1;
        break;
    case phymodMemRxLkup1588Mpp0:
        tsc_mem_data = mRX_LKUP_1588_MEM_MPP0;
        break;
    case phymodMemRxLkup1588Mpp1:
        tsc_mem_data = mRX_LKUP_1588_MEM_MPP1;
        break;
    case phymodMemSpeedPriorityMapTable:
        tsc_mem_data = mSPEED_PRIORITY_MAP_TBL;
        break;
    case phymodMemTxTwostep1588Ts:
        tsc_mem_data = mTX_TWOSTEP_1588_TS;
        break;
    case phymodMemRsFecSymbErrMib:
        tsc_mem_data = mRSFEC_SYMBOL_ERROR_MIB;
        break;
#else
   case phymodMemSpeedIdTable:
        tsc_mem_data = SPEED_ID_TABLEm;
        break;
    case phymodMemAMTable:
        tsc_mem_data = AM_TABLEm;
        break;
    case phymodMemUMTable:
        tsc_mem_data = UM_TABLEm;
        break;
    case phymodMemTxLkup1588Mpp0:
        tsc_mem_data = TX_LKUP_1588_MEM_MPP0m;
        break;
    case phymodMemTxLkup1588Mpp1:
        tsc_mem_data = TX_LKUP_1588_MEM_MPP1m;
        break;
    case phymodMemRxLkup1588Mpp0:
        tsc_mem_data = RX_LKUP_1588_MEM_MPP0m;
        break;
    case phymodMemRxLkup1588Mpp1:
        tsc_mem_data = RX_LKUP_1588_MEM_MPP1m;
        break;
    case phymodMemSpeedPriorityMapTable:
        tsc_mem_data = SPEED_PRIORITY_MAP_TBLm;
        break;
    case phymodMemTxTwostep1588Ts:
        tsc_mem_data = TX_TWOSTEP_1588_TSm;
        break;
    case phymodMemRsFecSymbErrMib:
        tsc_mem_data = RSFEC_SYMBOL_ERROR_MIBm;
        break;
#endif
    default:
        return SOC_E_PARAM;
    }

#ifdef BCM_ACCESS_SUPPORT
    rv = access_regmem(user_data->unit, 0, tsc_mem_data, user_data->blk_id, 0, mem_index, val);
#else
    rv = soc_mem_read(user_data->unit, tsc_mem_data, user_data->blk_id, mem_index, val);
#endif

    return rv;
}


/*
 * Function:
 *      portmod_pm8x100_gen2_wb_upgrade_func
 * Purpose:
 *      This function will take care of the warmboot variable manipulation
 *      in case of upgrade case, when variable definition got change or
 *      unspoorted varaible from previous version.
 * Parameters:
 *      unit              -(IN) Device unit number .
 *      arg               -(IN) Generic pointer for a specific module to be used
 *      recovered_version -(IN) Warmboot version of the existing data.
 *      new_version       -(IN) Warmboot version of new data.
 * Returns:
 *      Status
 */

int portmod_pm8x100_gen2_wb_upgrade_func(int unit, void *arg, int recovered_version,
                                         int new_version)
{
    return SOC_E_NONE;

}

int pm8x100_gen2_pm_wb_debug_log(int unit, pm_info_t pm_info)
{
    int i, rv = 0;
    int tmp_port;
    int ts_enable_port_count, speed_id_table_status;
    uint8_t tvco_pll_active_lane_bm;
    uint8_t tvco_pll_adv_lane_bm;
    uint32 is_core_initialized, is_active, is_bypassed, timesync_enable;
    phymod_an_mode_type_t an_mode = 0;
    portmod_fec_t fec;

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x100_gen2\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x100_gen2:{wb_buffer_index}\n")));
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "{%d}\n"), pm_info->wb_buffer_id));
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x100_gen2:{wb_buffer_index}\n")));

    rv = PM8x100_GEN2_IS_CORE_INITIALIZED_GET(unit, pm_info, &is_core_initialized);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x100_gen2:{is_initialized}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), is_core_initialized));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x100_gen2:{is_initialized}\n")));
    }

    rv = PM8x100_GEN2_IS_ACTIVE_GET(unit, pm_info, &is_active);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x100_gen2:{is_active}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), is_active));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x100_gen2:{is_active}\n")));
    }

    rv = PM8x100_GEN2_TVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_active_lane_bm);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x100_gen2:{tvco_pll_active_lane_bitmap}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{0x%x}\n"), tvco_pll_active_lane_bm));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x100_gen2:{tvco_pll_active_lane_bitmap}\n")));
    }

    rv = PM8x100_GEN2_TVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_adv_lane_bm);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x100_gen2:{tvco_pll_adv_lane_bitmap}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{0x%x}\n"), tvco_pll_adv_lane_bm));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x100_gen2:{tvco_pll_adv_lane_bitmap}\n")));
    }

    rv = PM8x100_GEN2_TS_ENABLE_PORT_COUNT_GET(unit, pm_info, &ts_enable_port_count);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x100_gen2:{ts_enable_port_count}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), ts_enable_port_count));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x100_gen2:{ts_enable_port_count}\n")));
    }

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x100_gen2:{lane,lane2port_map}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X100_GEN2; i++) {

        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit, "\n")));
        rv = PM8x100_GEN2_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, tmp_port));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x100_gen2:{lane,lane2port_map}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x100_gen2:{port,an_mode}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X100_GEN2; i++) {
        rv = PM8x100_GEN2_AN_MODE_GET(unit, pm_info, &an_mode, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, an_mode));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x100_gen2:{port,an_mode}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x100_gen2:{lane,lane2fec_map}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X100_GEN2; i++) {
        rv = PM8x100_GEN2_LANE2FEC_GET(unit, pm_info, i, &fec);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, fec));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x100_gen2:{lane,lane2fec_map}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x100_gen2:{port,port_is_pcs_bypassed}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X100_GEN2; i++) {
        rv = PM8x100_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, is_bypassed));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x100_gen2:{port,port_is_pcs_bypassed}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x100_gen2:{port,timesync_enable}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X100_GEN2; i++) {
        rv = PM8x100_GEN2_TIMESYNC_CONFIG_GET(unit, pm_info, &timesync_enable, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, timesync_enable));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x100_gen2:{port,timesync_enable}\n")));

    rv = PM8x100_GEN2_SPEED_ID_TABLE_STATUS_GET(unit, pm_info, &speed_id_table_status);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x100_gen2:{speed_id_table_status}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), speed_id_table_status));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x100_gen2:{speed_id_table_status}\n")));
    }

    return SOC_E_NONE;
}

/*
 * Initialize the buffer to support warmboot
 * The state of warmboot is store in the variables like
 * isInitialized, isActive, portIsPcsBypassed, ports.. etc.,
 * All of these variables need to be added to warmboot
 * any variables added to save the state of warmboot should be
 * included here.
 */
STATIC
int pm8x100_gen2_wb_buffer_init(int unit, int wb_buffer_index, pm_info_t pm_info)
{
    /* Declare the common variables needed for warmboot */
    WB_ENGINE_INIT_TABLES_DEFS;
    int wb_var_id, rv = SOC_E_NONE;
    int buffer_id = wb_buffer_index; /*required by SOC_WB_ENGINE_ADD_ Macros*/
    SOC_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(buffer_is_dynamic);

    SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PORTMOD, wb_buffer_index, "pm8x100_gen2",
                           portmod_pm8x100_gen2_wb_upgrade_func, pm_info,
                           PM8x100_GEN2_WB_BUFFER_VERSION, 1,
                           SOC_WB_ENGINE_PRE_RELEASE);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id,
                         "is_core_initialized", wb_buffer_index, sizeof(int),
                          NULL, MAX_PORTS_PER_PM8X100_GEN2, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isCoreInitialized] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_active",
                          wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isActive] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "tvco_pll_active_lane_bitmap",
                          wb_buffer_index, sizeof(uint8), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[tvcopllActiveLaneBitmap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "tvco_pll_adv_lane_bitmap",
                          wb_buffer_index, sizeof(uint8), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[tvcopllAdvLaneBitmap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "lane2portMap",
                          wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_PM8X100_GEN2, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[lane2portMap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "an_mode",
                          wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_PM8X100_GEN2, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[anMode] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "lane2fecMap",
                          wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_PM8X100_GEN2, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[lane2fecMap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));

    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "ts_enable_port_count",
                          wb_buffer_index, sizeof(int), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[tsEnablePortCount] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "timesync_enable",
                           wb_buffer_index, sizeof(uint32), NULL, MAX_PORTS_PER_PM8X100_GEN2, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[timesyncEnable] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "port_is_pcs_bypassed",
                          wb_buffer_index, sizeof(uint32), NULL, MAX_PORTS_PER_PM8X100_GEN2, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[portIsPcsBypassed] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "speed_id_table_status",
                          wb_buffer_index, sizeof(int), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[speedIdTableStatus] = wb_var_id;

    _SOC_IF_ERR_EXIT(soc_wb_engine_init_buffer(unit, SOC_WB_ENGINE_PORTMOD,
                                               wb_buffer_index, FALSE));

exit:
    SOC_FUNC_RETURN;
}

phymod_bus_t pm8x100_gen2_default_bus = {
    "PM8x100_gen2 Bus",
    pm8x100_gen2_default_bus_read,
    pm8x100_gen2_default_bus_write,
    NULL,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    pm8x100_gen2_default_mem_read,
    pm8x100_gen2_default_mem_write,
    PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

STATIC
int _pm8x100_gen2_pll_to_vco_get(uint32_t pll, portmod_vco_type_t* vco)
{
    int rv = SOC_E_NONE;

    switch (pll) {
        case phymod_TSCBH_PLL_DIV165:
            *vco = portmodVCO51P5625G;
            break;
        case phymod_TSCBH_PLL_DIV170:
            *vco = portmodVCO53P125G;
            break;
        default:
            *vco = portmodVCOInvalid;
            break;
    }

    return rv;
}

int pm8x100_gen2_port_mac_link_get(int unit, int port, pm_info_t pm_info, int* link)
{
    uint32 bitmap;
    int port_index, phy_acc;
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(_pm8x100_gen2_phy_access_get(unit, port, pm_info, &phy_acc));
    _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_index_get(unit, port, pm_info, &port_index, &bitmap));

    _SOC_IF_ERR_EXIT(
        dc3port_link_status_get(unit, phy_acc, port_index, link));

exit:
    SOC_FUNC_RETURN;
}


/*Add new pm.*/
int pm8x100_gen2_pm_init(int unit,
                         const portmod_pm_create_info_internal_t* pm_add_info,
                         int wb_buffer_index,
                         pm_info_t pm_info)
{
    const portmod_pm8x100_gen2_create_info_t *info =
                &pm_add_info->pm_specific_info.pm8x100_gen2;
    pm8x100_gen2_t pm8x100_gen2_data = NULL;
    int i, rv;
    int pm_is_active, ts_enable_port_count, speed_id_table_status;
    int is_core_initialized, fec, invalid_port;
    uint8 tvco_pll_adv_lane_bitmap, tvco_pll_active_lane_bitmap;
    int probe= 0;
    uint32_t pll_div, is_pwrdn;
    phymod_phy_access_t phy_access;
    portmod_vco_type_t vco;
    int phy;

    SOC_INIT_FUNC_DEFS;

    pm_info->type = pm_add_info->type;
    pm_info->unit = unit;
    pm_info->wb_buffer_id = wb_buffer_index;

    /* PM8x100 gen2 specific info */
    pm8x100_gen2_data = sal_alloc(sizeof(struct pm8x100_gen2_s), "specific_db");
    SOC_NULL_CHECK(pm8x100_gen2_data);
    pm_info->pm_data.pm8x100_gen2_db = pm8x100_gen2_data;

    PORTMOD_PBMP_ASSIGN(pm8x100_gen2_data->phys, pm_add_info->phys);
    pm8x100_gen2_data->int_core_access.type = phymodDispatchTypeCount;
    PORTMOD_PBMP_ITER(pm_add_info->phys, phy) {
        PM_8x100_GEN2_INFO(pm_info)->first_phy = phy;
        break;
    }
    pm8x100_gen2_data->warmboot_skip_db_restore = TRUE;
    pm8x100_gen2_data->portmod_mac_soft_reset = info->portmod_mac_soft_reset;

    /* init intertnal SerDes core access */
    phymod_core_access_t_init(&pm8x100_gen2_data->int_core_access);

    sal_memcpy(&pm8x100_gen2_data->polarity, &info->polarity,
               sizeof(phymod_polarity_t));
    sal_memcpy(&(pm8x100_gen2_data->int_core_access.access), &info->access.access,
                sizeof(phymod_access_t));

    sal_memcpy(&pm8x100_gen2_data->lane_map, &info->lane_map,
                sizeof(pm8x100_gen2_data->lane_map));
    pm8x100_gen2_data->ref_clk = info->ref_clk;
    pm8x100_gen2_data->fw_load_method = info->fw_load_method;
    pm8x100_gen2_data->external_fw_loader = info->external_fw_loader;
    pm8x100_gen2_data->tvco = info->tvco;
    pm8x100_gen2_data->pm_offset = info->pm_offset;


    if (info->access.access.bus == NULL) {
        /* if null - use default */
        pm8x100_gen2_data->int_core_access.access.bus = &pm8x100_gen2_default_bus;
    } else {
        /* check null for mem_read/mem_write */
        if (pm8x100_gen2_data->int_core_access.access.bus->mem_read == NULL) {
            pm8x100_gen2_data->int_core_access.access.bus->mem_read =pm8x100_gen2_default_bus.mem_read;
        }
        if (pm8x100_gen2_data->int_core_access.access.bus->mem_write == NULL) {
            pm8x100_gen2_data->int_core_access.access.bus->mem_write =pm8x100_gen2_default_bus.mem_write;
        }
    }

    if (pm8x100_gen2_data->external_fw_loader == NULL) {
        /* if null - use default */
        pm8x100_gen2_data->external_fw_loader = pm8x100_gen2_default_fw_loader;
    }

    /*init wb buffer*/
    _SOC_IF_ERR_EXIT(pm8x100_gen2_wb_buffer_init(unit, wb_buffer_index,  pm_info));

    if (pm8x100_gen2_data->int_core_access.access.pmd_info_ptr == NULL) {
        pm8x100_gen2_data->int_core_access.access.pmd_info_ptr = sal_alloc(PMD_INFO_DATA_STRUCTURE_SIZE,
                             "PortMod PM PMD info");
        if (pm8x100_gen2_data->int_core_access.access.pmd_info_ptr == NULL) {
            return SOC_E_MEMORY;
        }
        sal_memset(pm8x100_gen2_data->int_core_access.access.pmd_info_ptr, 0, PMD_INFO_DATA_STRUCTURE_SIZE);
    }


    if (SOC_WARM_BOOT(unit)) {
        /* For warmboot, probe Serdes driver type for active PMs. */
        is_core_initialized = 0;

        rv = PM8x100_GEN2_IS_CORE_INITIALIZED_GET(unit, pm_info, &is_core_initialized);
        _SOC_IF_ERR_EXIT(rv);

        /* print scache information for debugging */
        pm8x100_gen2_pm_wb_debug_log(unit, pm_info);

        if (is_core_initialized) {
            _SOC_IF_ERR_EXIT(portmod_common_serdes_probe(pm8x100_gen2_serdes_list, &pm8x100_gen2_data->int_core_access, &probe));
            if (!probe) {
                LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit, "ERROR: serdes probe failed type=%d\n"), pm8x100_gen2_data->int_core_access.type));
            }

            /* Get VCO rates from HW */
            sal_memcpy(&phy_access, &(pm8x100_gen2_data->int_core_access),
                       sizeof(phymod_phy_access_t));

            /* 1. Get TVCO rate :
             *    a. If TVCO is not free, get TVCO rate from HW.
             *    b. If TVCO is free, TVCO is powerdown.
             */
            _SOC_IF_ERR_EXIT(phymod_phy_pll_powerdown_get(&phy_access, phy_access.access.tvco_pll_index, &is_pwrdn));
            if (is_pwrdn)
            {
                pm8x100_gen2_data->tvco = portmodVCOInvalid;
            }
            else
            {
                phy_access.access.pll_idx = phy_access.access.tvco_pll_index;
                _SOC_IF_ERR_EXIT(phymod_phy_pll_multiplier_get(&phy_access, &pll_div));
                _SOC_IF_ERR_EXIT(_pm8x100_gen2_pll_to_vco_get(pll_div, &vco));
                pm8x100_gen2_data->tvco = vco;
            }
        }
    } else {
        /* For coldboot, initialized warmboot variables for the PM. */
        is_core_initialized = 0;
        rv = PM8x100_GEN2_IS_CORE_INITIALIZED_SET(unit, pm_info,
                                            is_core_initialized);
        _SOC_IF_ERR_EXIT(rv);

        pm_is_active = 0;
        rv = PM8x100_GEN2_IS_ACTIVE_SET(unit, pm_info, pm_is_active);
        _SOC_IF_ERR_EXIT(rv);

        tvco_pll_adv_lane_bitmap = 0;
        rv = PM8x100_GEN2_TVCO_PLL_ADV_LANE_BITMAP_SET(unit, pm_info, tvco_pll_adv_lane_bitmap);
        _SOC_IF_ERR_EXIT(rv);

        tvco_pll_active_lane_bitmap = 0;
        rv = PM8x100_GEN2_TVCO_PLL_ACTIVE_LANE_BITMAP_SET(unit, pm_info, tvco_pll_active_lane_bitmap);
        _SOC_IF_ERR_EXIT(rv);

        fec = 0;
        for (i = 0; i < MAX_PORTS_PER_PM8X100_GEN2; i++) {
            _SOC_IF_ERR_EXIT(PM8x100_GEN2_LANE2FEC_SET(unit, pm_info, i, fec));
        }

        invalid_port = -1;
        for (i = 0; i < MAX_PORTS_PER_PM8X100_GEN2; i++) {
            _SOC_IF_ERR_EXIT(PM8x100_GEN2_LANE2PORT_SET(unit, pm_info, i, invalid_port));
        }

        ts_enable_port_count = 0;
        rv = PM8x100_GEN2_TS_ENABLE_PORT_COUNT_SET(unit, pm_info, ts_enable_port_count);
        _SOC_IF_ERR_EXIT(rv);

        speed_id_table_status = PM8x100_GEN2_SPEED_ID_TABLE_STATUS_NOT_LOADED;
        rv = PM8x100_GEN2_SPEED_ID_TABLE_STATUS_SET(unit, pm_info, speed_id_table_status);
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
    if (SOC_FUNC_ERROR) {
        pm8x100_gen2_pm_destroy(unit, pm_info);
    }
    SOC_FUNC_RETURN;

}

/*Release PM resources*/
int pm8x100_gen2_pm_destroy(int unit, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;

    if (pm_info->pm_data.pm8x100_gen2_db != NULL) {
        if (PM_8x100_GEN2_INFO(pm_info)->int_core_access.access.pmd_info_ptr != NULL) {
            sal_free(PM_8x100_GEN2_INFO(pm_info)->int_core_access.access.pmd_info_ptr);
            PM_8x100_GEN2_INFO(pm_info)->int_core_access.access.pmd_info_ptr = NULL;
        }
        sal_free(pm_info->pm_data.pm8x100_gen2_db);
        pm_info->pm_data.pm8x100_gen2_db = NULL;
    }

    SOC_FUNC_RETURN;
}

/*
 * Get the VCO rates from given PCS bypassed port speed
 */

int _pm8x100_gen2_pcs_bypassed_vco_get(int speed, portmod_vco_type_t* vco)
{
    SOC_INIT_FUNC_DEFS;

    switch (speed) {
        default:
            *vco = portmodVCOInvalid;
            break;
    }

    SOC_FUNC_RETURN;
}

/* This function will check the speed config is valid or not, and return required vco for valid input */
/* The supported speed config in this function is based on PM8x100_gen2 Portmod Spec */
STATIC
int _pm8x100_gen2_port_speed_config_to_vco_get(const portmod_speed_config_t* speed_config,
                                               int speed_for_pcs_bypass_port,
                                               portmod_vco_type_t* vco)
{
    SOC_INIT_FUNC_DEFS;

    if (!speed_for_pcs_bypass_port) {
        _SOC_IF_ERR_EXIT(
                _pm8x100_gen2_fs_ability_table_vco_get(speed_config->speed,
                                                      speed_config->num_lane,
                                                      speed_config->fec,
                                                      vco));
    } else {
        _SOC_IF_ERR_EXIT(
                _pm8x100_gen2_pcs_bypassed_vco_get(speed_config->speed, vco));
    }
    /*
     * when *vco == portmodVCOInvalid, it means the entered combination of
     * port speed, number of lanes, and FEC type is not supported.
     */
    if (*vco == portmodVCOInvalid) {
        return SOC_E_CONFIG;
    }

exit:
    SOC_FUNC_RETURN;
}


STATIC
int _pm8x100_gen2_lanebitmap_set(int starting_lane, int num_lane, uint8 *bitmap)
{
    int i;

    for (i = 0; i < num_lane; i++) {
        *bitmap |= 1 << (starting_lane + i);
    }

    return SOC_E_NONE;
}

/*
 * This function is to validate the FEC settings on each PM.
 * For each MPP, we can not support both RS544 and RS272.
 */
STATIC
int _pm8x100_gen2_fec_validate(int unit,
                               uint8_t rs544_bitmap,
                               uint8_t rs272_bitmap,
                               uint8_t *affected_lane_bitmap)
{
    uint8_t rs544_mpp0_01, rs544_mpp0_23, rs544_mpp1_01, rs544_mpp1_23;
    uint8_t rs272_mpp0_01, rs272_mpp0_23, rs272_mpp1_01, rs272_mpp1_23;

    SOC_INIT_FUNC_DEFS;

    /* get the proper fec bitmap properly */
    rs544_mpp0_01 = rs544_bitmap & 0x3;
    rs544_mpp0_23 = rs544_bitmap & 0xc;
    rs544_mpp1_01 = rs544_bitmap & 0x30;
    rs544_mpp1_23 = rs544_bitmap & 0xc0;

    rs272_mpp0_01 = rs272_bitmap & 0x3;
    rs272_mpp0_23 = rs272_bitmap & 0xc;
    rs272_mpp1_01 = rs272_bitmap & 0x30;
    rs272_mpp1_23 = rs272_bitmap & 0xc0;

    *affected_lane_bitmap = 0;
    if (rs544_mpp0_01 && rs272_mpp0_01) {
        *affected_lane_bitmap = rs544_mpp0_01 | rs272_mpp0_01;
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("Can not accommodate FEC settings on MPP0_01.")));
    } else if (rs544_mpp0_23 && rs272_mpp0_23) {
        *affected_lane_bitmap = rs544_mpp0_23 | rs272_mpp0_23;
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("Can not accommodate FEC settings on MPP0_23.")));
    } else if (rs544_mpp1_01 && rs272_mpp1_01) {
        *affected_lane_bitmap = rs544_mpp1_01 | rs272_mpp1_01;
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("Can not accommodate FEC settings on MPP1_01.")));
    } else if (rs544_mpp1_23 && rs272_mpp1_23) {
        *affected_lane_bitmap = rs544_mpp1_23 | rs272_mpp1_23;
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("Can not accommodate FEC settings on MPP1_23.")));
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * This function will return the lane_mask for a given
 * port index and number of lanes of the port.
 */
static void
_pm8x100_gen2_lane_mask_get(int port_index, int num_lane, uint32_t *lane_mask)
{
    int i;

    *lane_mask = 0;

    for (i = 0; i < num_lane; i++) {
        *lane_mask |= 1 << (port_index + i);
    }

}

static int
_pm8x100_gen2_pm_port_lane_map_validate(int unit, uint32_t lane_map)
{
    int valid = 0;
    int idx;
    uint32_t lmap_lane[] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
                            0x3, 0xc, 0x30, 0xc0,
                            0xf, 0xf0, 0xff};

    /* Check for lane map. */
    for (idx = 0; idx < COUNTOF(lmap_lane); idx++) {
        if (lane_map == lmap_lane[idx]) {
            valid = 1;
            break;
        }
    }

    if (!valid) {
        return (SOC_E_PARAM);
    }

    return SOC_E_NONE;
}

/*Get the suggested VCO values based on the speed config list*/
int pm8x100_gen2_pm_vcos_get(int unit,
                             portmod_dispatch_type_t pm_type,
                             uint32 flags,
                             portmod_pm_vco_setting_t* vco_select)
{
    portmod_vco_type_t current_vco = portmodVCOInvalid, req_vco;
    portmod_speed_config_t *current_speed_config = NULL;
    uint8_t rs544_bm = 0, rs272_bm = 0, affected_bm = 0;
    int i, rv = 0, is_pcs_bypass;
    uint32_t lane_mask = 0;

    SOC_INIT_FUNC_DEFS;

    if(vco_select == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("vco_select NULL paramaeter")));
    }

    for (i = 0; i < vco_select->num_speeds; i++) {
        current_speed_config = &vco_select->speed_config_list[i];

        if (vco_select->port_starting_lane_list != NULL) {
            _pm8x100_gen2_lane_mask_get(vco_select->port_starting_lane_list[i],
                                        current_speed_config->num_lane, &lane_mask);

            if (vco_select->port_starting_lane_list != NULL) {
                /* 1. Check lane_map. */
                rv = _pm8x100_gen2_pm_port_lane_map_validate(unit, lane_mask);
                if (rv != SOC_E_NONE) {
                    _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("lane_map validate failed\n")));
                }
            }
        }
        /* 2. Validate force speed ability. */
        is_pcs_bypass = vco_select->speed_for_pcs_bypass_port[i];
        req_vco = portmodVCOInvalid;
        rv = _pm8x100_gen2_port_speed_config_to_vco_get(current_speed_config,
                                                        is_pcs_bypass,
                                                        &req_vco);
        if (rv != SOC_E_NONE) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                      (_SOC_MSG("Speed config is not supported")));
        }

        if (current_vco == portmodVCOInvalid) {
            current_vco = req_vco;
        } else if (req_vco != current_vco) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                      (_SOC_MSG("request vco is unavailable\n")));
        }

        if ((current_speed_config->fec == PORTMOD_PORT_PHY_FEC_RS_272) ||
            (current_speed_config->fec == PORTMOD_PORT_PHY_FEC_RS_272_2XN)) {
            rs272_bm |= lane_mask;
        } else if ((current_speed_config->fec == PORTMOD_PORT_PHY_FEC_RS_544) ||
                   (current_speed_config->fec == PORTMOD_PORT_PHY_FEC_RS_544_2XN)) {
            rs544_bm |= lane_mask;
        }
    }

    /* Validate FEC settings for RS544 and RS272. */
    _SOC_IF_ERR_EXIT(
        _pm8x100_gen2_fec_validate(unit, rs544_bm, rs272_bm, &affected_bm));

    /* Set output. */
    vco_select->tvco = current_vco;
    vco_select->is_tvco_new = (current_vco == portmodVCOInvalid) ? 0 : 1;

exit:
    SOC_FUNC_RETURN;
}

/*Enable port macro.*/
/* This function contains 3 parts:
 * 1. Bring Serdes out of hard reset.
 * 2. Bring 2 DC3MACs out of hard reset.
 */
int pm8x100_gen2_pm_enable(int unit,
                           int pm_id,
                           pm_info_t pm_info,
                           int flags,
                           int enable)
{
    int phy_acc;
    uint32 is_reset;
    int skip_mac, mac_only;

    SOC_INIT_FUNC_DEFS;

    /* Get the first physical port of the pm core */
    phy_acc = PM_8x100_GEN2_INFO(pm_info)->first_phy | SOC_REG_ADDR_PHY_ACC_MASK;

    _SOC_IF_ERR_EXIT(dc3port_dc3mac_control_get(unit, phy_acc, &is_reset));
    skip_mac = PORTMOD_PM_ENABLE_SKIP_MAC_GET(flags);
    mac_only = PORTMOD_PM_ENABLE_MAC_ONLY_GET(flags);
    if(enable && is_reset) {
        if (!mac_only) {
            /* Bring Serdes OOR */
            _SOC_IF_ERR_EXIT(dc3port_tsc_ctrl_set(unit, phy_acc, 1/*tsc_pwr_on*/));
        }
        if (!skip_mac) {            /* Bring MAC OOR */
            _SOC_IF_ERR_EXIT(dc3port_dc3mac_control_set(unit, phy_acc, 0));
        }
    } else if ((!enable) && (!is_reset)){ /* disable */
        if (!skip_mac) {
            /* put MAC in reset */
            _SOC_IF_ERR_EXIT(dc3port_dc3mac_control_set(unit, phy_acc, 1));
        }
        if (!mac_only) {
            /* If current PM is not master PM, put Serdes into reset*/
            _SOC_IF_ERR_EXIT(dc3port_tsc_ctrl_set(unit, phy_acc, 0/*tsc_pwr_off*/));

            /* clear PMD info table */
            sal_memset(PM_8x100_GEN2_INFO(pm_info)->int_core_access.access.pmd_info_ptr, 0, PMD_INFO_DATA_STRUCTURE_SIZE);
        }
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm8x100_gen2_pm_core_probe(int unit, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int probe =0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(
        portmod_common_serdes_probe(pm8x100_gen2_serdes_list,
                                    &PM_8x100_GEN2_INFO(pm_info)->int_core_access, &probe));

exit:
    SOC_FUNC_RETURN;
}

/* This function will return required pll based on the ref_clk and vco */
int _pm8x100_gen2_vco_to_pll_get(portmod_vco_type_t vco, uint32_t* pll)
{
    int rv = SOC_E_NONE;
    SOC_INIT_FUNC_DEFS;

    switch (vco) {
        case portmodVCO51P5625G:
            *pll = phymod_TSCBH_PLL_DIV165;
            break;
        case portmodVCO53P125G:
            *pll = phymod_TSCBH_PLL_DIV170;
            break;
       case portmodVCOInvalid:
            *pll = phymod_TSCBH_PLL_DIVNONE;
            break;
       default:
            rv = SOC_E_PARAM;
            break;
    }
    _SOC_IF_ERR_EXIT(rv);

 exit:
     SOC_FUNC_RETURN;
}

STATIC
int _pm8x100_gen2_pm_serdes_core_init(int unit, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    portmod_speed_config_t pm_spd_cfg;
    portmod_vco_type_t init_vco = portmodVCOInvalid;
    int init_flags = 0;
    int rv = 0, is_pcs_bypassed = 0;
    phymod_core_init_config_t core_config;
    phymod_core_status_t core_status;
    uint32_t core_is_initialized;

    SOC_INIT_FUNC_DEFS;

    rv = PM8x100_GEN2_IS_CORE_INITIALIZED_GET(unit, pm_info, &core_is_initialized)
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(phymod_core_init_config_t_init(&core_config));
    _SOC_IF_ERR_EXIT(phymod_core_status_t_init(&core_status));
    core_status.pmd_active = 0;

    if (PORTMOD_PORT_ADD_F_SKIP_SPEED_INIT_GET(add_info)) {
        /* If PORTMOD_PORT_ADD_F_SKIP_SPEED_INIT flag is set, do not bring up PLLs */
        if (!PORTMOD_CORE_INIT_FLAG_INITIALZIED_GET(core_is_initialized)) {
            /* Only if core is not initilized - put tvco as invalid */
            init_vco = portmodVCOInvalid;
            PM_8x100_GEN2_INFO(pm_info)->tvco = init_vco;
        }
    } else if (add_info->tvco != portmodVCOInvalid) {
        init_vco = add_info->tvco;
        PM_8x100_GEN2_INFO(pm_info)->tvco = init_vco;
    } else {
        /* Get the init PLL value based on port config. */
        pm_spd_cfg.speed = add_info->speed_config.speed;
        pm_spd_cfg.num_lane = add_info->speed_config.num_lane;
        pm_spd_cfg.fec = add_info->speed_config.fec;

        is_pcs_bypassed = PORTMOD_PORT_ADD_F_PCS_BYPASSED_GET(add_info) ? 1 : 0;
        /* Get requeseted VCO. */
        _SOC_IF_ERR_EXIT
            (_pm8x100_gen2_port_speed_config_to_vco_get(&pm_spd_cfg,
            is_pcs_bypassed, &init_vco));
    }

    _SOC_IF_ERR_EXIT
        (_pm8x100_gen2_vco_to_pll_get(init_vco, &(core_config.pll0_div_init_value)));

    /* Add lane map config */
    core_config.firmware_load_method = PM_8x100_GEN2_INFO(pm_info)->fw_load_method;
    core_config.firmware_loader = PM_8x100_GEN2_INFO(pm_info)->external_fw_loader;
    core_config.lane_map = PM_8x100_GEN2_INFO(pm_info)->lane_map;
    core_config.polarity_map = PM_8x100_GEN2_INFO(pm_info)->polarity;
    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&core_config.interface));
    core_config.interface.ref_clock = PM_8x100_GEN2_INFO(pm_info)->ref_clk;

    if (PORTMOD_PORT_ADD_F_BYPASS_FW_CRC_CHECK_GET(add_info)) {
        PHYMOD_CORE_INIT_F_BYPASS_CRC_CHECK_SET(&core_config);
    }
    init_flags = PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) | PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info);

    if (PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_GET(add_info)) {
        PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_SET(&core_config);
    }

    if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        if (PORTMOD_CORE_INIT_FLAG_FIRMWARE_LOADED_GET(core_is_initialized)) {
            return SOC_E_NONE;
        } else {
            PHYMOD_CORE_INIT_F_EXECUTE_PASS1_SET(&core_config);
            PHYMOD_CORE_INIT_F_EXECUTE_PASS2_CLR(&core_config);
        }
    }

    if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
        PHYMOD_CORE_INIT_F_EXECUTE_PASS1_CLR(&core_config);
        PHYMOD_CORE_INIT_F_EXECUTE_PASS2_SET(&core_config);
    }

    if (PORTMOD_PORT_ADD_F_BYPASS_FW_CRC_CHECK_GET(add_info)) {
        PHYMOD_CORE_INIT_F_BYPASS_CRC_CHECK_SET(&core_config);
    }

#ifdef FW_BCAST_DOWNLOAD
    if (PORTMOD_PORT_ADD_F_FIRMWARE_DOWNLOAD_BCAST_GET(add_info)) {
        PHYMOD_CORE_INIT_F_SERDES_FW_BCAST_SET(&core_config);
    }
#endif

    if (!PORTMOD_CORE_INIT_FLAG_INITIALZIED_GET(core_is_initialized)) {
         /* firmware load will happen after pass 1 */
        if (!PORTMOD_CORE_INIT_FLAG_FIRMWARE_LOADED_GET(core_is_initialized) ||
            PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
            _SOC_IF_ERR_EXIT
                (phymod_core_init(&PM_8x100_GEN2_INFO(pm_info)->int_core_access,
                                  &core_config,
                                  &core_status));
        }
        /* Update firmware download flag if apply */
        if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) || init_flags == 0) {
            PORTMOD_CORE_INIT_FLAG_FIRMWARE_LOADED_SET(core_is_initialized);
            rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,
                                       pm_info->wb_vars_ids[isCoreInitialized],
                                       &core_is_initialized);
            _SOC_IF_ERR_EXIT(rv);
        }
        /* only if core init pass2 */
        if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info) || init_flags == 0) {
            /* Program PM offset at the end of core initialization. */
            _SOC_IF_ERR_EXIT
                (phymod_timesync_offset_set(&PM_8x100_GEN2_INFO(pm_info)->int_core_access,
                                            PM_8x100_GEN2_INFO(pm_info)->pm_offset));

            PORTMOD_CORE_INIT_FLAG_INITIALZIED_SET(core_is_initialized);
            rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,
                                       pm_info->wb_vars_ids[isCoreInitialized],
                                       &core_is_initialized);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

exit:
    SOC_FUNC_RETURN;
}

/*PM Core init routine*/
/* PM core init has 3 phase:
 * 1. Core probe: Probe serdes dispatch type.
 * 2. Pass1: Download FW, program ref_clk, polarity, lane map, etc.
 * 3. Pass2: PLL configuration, program UC core config, load PCS tables etc.
 */
int pm8x100_gen2_pm_serdes_core_init(int unit,
                                     int pm_id,
                                     pm_info_t pm_info,
                                     const portmod_port_add_info_t* add_info)
{
    int init_all = 0;
    int rv = SOC_E_NONE;

    SOC_INIT_FUNC_DEFS;

    /* Update per core based database */
    if (add_info->init_config.polarity_overwrite == 1) {
        sal_memcpy(&PM_8x100_GEN2_INFO(pm_info)->polarity,
            &add_info->init_config.polarity[0], sizeof(phymod_polarity_t));
    }
    if (add_info->init_config.lane_map_overwrite == 1) {
        sal_memcpy(&PM_8x100_GEN2_INFO(pm_info)->lane_map,
            &add_info->init_config.lane_map[0], sizeof(phymod_lane_map_t));
    }
    if (add_info->init_config.fw_load_method_overwrite == 1) {
        PM_8x100_GEN2_INFO(pm_info)->fw_load_method =
            add_info->init_config.fw_load_method[0];
    }
    if (add_info->init_config.ref_clk_overwrite == 1) {
        PM_8x100_GEN2_INFO(pm_info)->ref_clk = add_info->init_config.ref_clk;
    }

    init_all = (!PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) &&
                !PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) &&
                !PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) ? 1 : 0;

    /* probe serdes core */
    if (PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) || init_all) {
        rv = _pm8x100_gen2_pm_core_probe(unit, pm_info, add_info);
        _SOC_IF_ERR_EXIT(rv);
    }

    /* Return here if caller only request Core Probe */
    if (!(PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) &&
        (PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info))) {
        return (rv);
    }

    /* core config for internal serdes. */
    rv = _pm8x100_gen2_pm_serdes_core_init(unit, pm_info, add_info);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm8x100_gen2_pm_port_init(int unit,
                               int port,
                               pm_info_t pm_info,
                               int internal_port,
                               const portmod_port_add_info_t* add_info,
                               int enable)
{
    int phy_acc;
    uint32 rsv_mask;
    uint32 is_bypassed = 0;
    int port_index;
    uint32 bitmap[1];

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x100_gen2_phy_access_get(unit, port, pm_info, &phy_acc));
    if (internal_port == -1)  {
        _SOC_EXIT_WITH_ERR(SOC_E_PORT, (_SOC_MSG("Invalid internal Port %d"),
                                internal_port));
    }

    if (enable) {
        _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_index_get(unit, port, pm_info, &port_index, bitmap));
        _SOC_IF_ERR_EXIT(PM8x100_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));
        if (!is_bypassed)
        {
            /* RSV Mask */
            rsv_mask = 0;
            SHR_BITSET(&rsv_mask, 3); /* Receive terminate/code error */
            SHR_BITSET(&rsv_mask, 4); /* CRC error */
            SHR_BITSET(&rsv_mask, 6); /* Truncated/Frame out of Range */
            SHR_BITSET(&rsv_mask, 17); /* RUNT detected*/
           _SOC_IF_ERR_EXIT(dc3mac_rsv_mask_set(unit, phy_acc, rsv_mask));

            /* Init MAC */
            _SOC_IF_ERR_EXIT(dc3mac_port_init(unit, phy_acc, 1));
        }

        /* LSS */
        _SOC_IF_ERR_EXIT(dc3port_port_fault_link_status_set(unit, phy_acc, 1));

        if (!is_bypassed)
        {
            /* Counter MAX size */
            _SOC_IF_ERR_EXIT(pm8x100_gen2_port_cntmaxsize_set(unit, phy_acc, pm_info, 1518));
        }
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * Perfom MAC soft reset, including drain cells procedure.
 *
 * Supported reset modes: IN, OUT, IN-OUT
 *
 *    IN - set MAC in soft reset. 
 *         When reset_mode=IN the parameters saved_rx_enable, saved_mac_ctrl are outputs, 
 *         and should be sent back to this function when called with OUT.
 *    OUT - get MAC out of soft reset.
 *         When reset_mode=OUT the parameters saved_rx_enable, saved_mac_ctrl are inputs,
 *         the values that should be passed in are the values returned from IN.
 *         Example sequnece: 
 *          int saved_rx_enablel uint64 saved_mac_ctrl;
 *          pm8x100_gen2_port_soft_reset(unit, port, pinfo, portmodMacSoftResetModeIn, &saved_rx_enablel, &saved_mac_ctrl);
 *          ... here the MAC is in reset ...
 *          pm8x100_gen2_port_soft_reset(unit, port, pinfo, portmodMacSoftResetModeOut, &saved_rx_enablel, &saved_mac_ctrl);
 *     IN-OUT - get MAC IN reset and then out of reset. 
 *          When  reset_mode=INOUT, saved_rx_enable, saved_mac_ctrl can be set to NULL.
 */
int pm8x100_gen2_port_soft_reset(int unit,
                                 int port,
                                 pm_info_t pm_info,
                                 portmod_mac_soft_reset_mode_t reset_mode,
                                 int *saved_rx_enable,
                                 uint64 *saved_mac_ctrl)
{

    int rv, rx_enable = 0;
    portmod_drain_cells_t drain_cells;
    uint32 cell_count;
    uint64 mac_ctrl;
    soc_timeout_t to;
    uint32_t soft_reset_enable;
    uint32_t fld_val = 0;

    SOC_INIT_FUNC_DEFS;

    /* protect this function from concurrent access */
    if (PM_8x100_GEN2_INFO(pm_info)->portmod_mac_soft_reset) {
        rv = PM_8x100_GEN2_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypeLock);
        _SOC_IF_ERR_EXIT(rv);
    }

    /* Callback soft reset function */
    if (PM_8x100_GEN2_INFO(pm_info)->portmod_mac_soft_reset) {
        rv = PM_8x100_GEN2_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypePre);
        _SOC_IF_ERR_EXIT(rv);
    }

    if (portmodMacSoftResetModeOut != reset_mode) {
        /* reset_mode is portmodMacSoftResetModeIn or portmodMacSoftResetModeInOut */

        rv = dc3mac_egress_queue_drain_get(unit, port, &mac_ctrl, &rx_enable);
        _SOC_IF_ERR_EXIT(rv);

        /* return MAC state if required */
        if ((NULL != saved_rx_enable) && (NULL != saved_mac_ctrl)) {
            *saved_rx_enable = rx_enable;
            COMPILER_64_COPY(*saved_mac_ctrl, mac_ctrl);
        }

        _SOC_IF_ERR_EXIT(dc3mac_reset_get(unit, port, &soft_reset_enable));
        if (!soft_reset_enable)
        {
            /* Drain cells */
            rv = dc3mac_drain_cell_get(unit, port, &drain_cells);
            _SOC_IF_ERR_EXIT(rv);

            /* Start TX FIFO draining */
            rv = dc3mac_drain_cell_start(unit, port);
            _SOC_IF_ERR_EXIT(rv);

            /* De-assert SOFT_RESET to let the drain start */
            rv = dc3mac_reset_set(unit, port, 0);
            _SOC_IF_ERR_EXIT(rv);

            if (!SAL_BOOT_SIMULATION){
                /* Wait until TX fifo cell count is 0 */
                soc_timeout_init(&to, 250000, 0);
                for (;;) {
                    rv = dc3mac_txfifo_cell_cnt_get(unit, port, &cell_count);
                    _SOC_IF_ERR_EXIT(rv);
                    if (cell_count == 0) {
                        break;
                    }
                    if (soc_timeout_check(&to)) {
                        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("ERROR: u=%d p=%d timeout draining TX FIFO (%d cells remain)\n"),
                                                            unit, port, cell_count));
                    }
                }
            } else {
                rv = dc3mac_txfifo_cell_cnt_get(unit, port, &cell_count);
                _SOC_IF_ERR_EXIT(rv);
            }

            /* Stop TX FIFO draining */
            rv = dc3mac_drain_cell_stop(unit, port, &drain_cells);
            _SOC_IF_ERR_EXIT(rv);

            /* Put port into SOFT_RESET */
            rv = dc3mac_reset_set(unit, port, 1);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

    if(portmodMacSoftResetModeIn != reset_mode) {
        /* reset_mode is portmodMacSoftResetModeOut or portmodMacSoftResetModeInOut */
        if (PM_8x100_GEN2_INFO(pm_info)->portmod_mac_soft_reset) {

            _SOC_IF_ERR_EXIT(dc3mac_reset_get(unit, port, &soft_reset_enable));
            if (soft_reset_enable)
            {
                /* 
                 * reset credits only if 
                 * soft reset would toggle
                 */
                rv = PM_8x100_GEN2_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypeDuring);
                _SOC_IF_ERR_EXIT(rv);
            }
        }

        /* For out of reset only operation, use MAC state from input */
        if (portmodMacSoftResetModeOut == reset_mode) {
            SOC_NULL_CHECK(saved_rx_enable);
            SOC_NULL_CHECK(saved_mac_ctrl);
            rx_enable = *saved_rx_enable;
            COMPILER_64_COPY(mac_ctrl, *saved_mac_ctrl);
        }

        fld_val = 0;
        /* coverity[incompatible_cast:FALSE] */
        UA_REG_FIELD_SET(unit, DC3MAC_CTRL, &mac_ctrl, SOFT_RESET, &fld_val); /*make sure restored data wont put mac back in reset*/

        /* Restore dc3mac_CTRL to original value */
        rv = dc3mac_mac_ctrl_set(unit, port, mac_ctrl);
        _SOC_IF_ERR_EXIT(rv);

        /* Callback soft reset function */
        if (PM_8x100_GEN2_INFO(pm_info)->portmod_mac_soft_reset) {
            rv = PM_8x100_GEN2_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypePost);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

exit:

    if (PM_8x100_GEN2_INFO(pm_info)->portmod_mac_soft_reset) {
        int local_rv = SOC_E_NONE;
        /* unlock function mutex */
        local_rv = PM_8x100_GEN2_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypeUnlock);
        if (local_rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_PORT,(BSL_META_UP(unit, port,
                                                   "ERROR: u=%d p=%d unable to unlock\n"), unit, port));
            return local_rv;
        }
    }

    SOC_FUNC_RETURN;
}

STATIC
int _pm8x100_gen2_port_mac_drain_soft_reset(int unit, int port)
{
    int rv;
    portmod_drain_cells_t drain_cells;
    uint32 cell_count;
    soc_timeout_t to;
    SOC_INIT_FUNC_DEFS;

    /* Drain cells */
    rv = dc3mac_drain_cell_get(unit, port, &drain_cells);
    _SOC_IF_ERR_EXIT(rv);

    /* Start TX FIFO draining */
    rv = dc3mac_drain_cell_start(unit, port);
    _SOC_IF_ERR_EXIT(rv);

    /* De-assert SOFT_RESET to let the drain start */
    rv = dc3mac_reset_set(unit, port, 0);
    _SOC_IF_ERR_EXIT(rv);
    
    if (!SAL_BOOT_SIMULATION){
        /* Wait until TX fifo cell count is 0 */
        soc_timeout_init(&to, 250000, 0);
        for (;;) {
            rv = dc3mac_txfifo_cell_cnt_get(unit, port, &cell_count);
            _SOC_IF_ERR_EXIT(rv);
            if (cell_count == 0) {
                break;
            }
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_PORT,(BSL_META_UP(unit, port,
                                           "ERROR: u=%d p=%d timeout draining TX FIFO (%d cells remain)\n"), unit, port, cell_count));
                return SOC_E_INTERNAL;
            }
        }
    } else {
        rv = dc3mac_txfifo_cell_cnt_get(unit, port, &cell_count);
        _SOC_IF_ERR_EXIT(rv);
    }
    /* Stop TX FIFO draining */
    rv = dc3mac_drain_cell_stop(unit, port, &drain_cells);
    _SOC_IF_ERR_EXIT(rv);

    /* Put port into SOFT_RESET */
    rv = dc3mac_reset_set(unit, port, 1);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm8x100_gen2_port_rx_restore_mac_out_of_reset(int unit, int port, int rx_enable)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    /* Enable RX, de-assert SOFT_RESET */
    rv = dc3mac_egress_queue_drain_rx_en(unit, port, rx_enable);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

/*Add new port*/
int pm8x100_gen2_port_attach(int unit, int port, pm_info_t pm_info,
                             const portmod_port_add_info_t* add_info)
{
    int port_index = -1, pm_is_active = 0, is_pcs_bypassed = 0;
    int rv = 0, port_i, my_i, found_first_active_lane;
    int phy, ii, nof_phys;
    portmod_pbmp_t port_phys_in_pm;
    phymod_phy_init_config_t init_config;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    phymod_phy_signalling_method_t signalling_mode;
    phymod_an_mode_type_t init_an_mode = phymod_AN_MODE_NONE;
    const portmod_speed_config_t *speed_config = &add_info->speed_config;
    int usr_cfg_idx;
    uint32 timesync_config_default = 0;

    SOC_INIT_FUNC_DEFS;

    /* Get the first phy related to this port */
    PORTMOD_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
    PORTMOD_PBMP_AND(port_phys_in_pm, PM_8x100_GEN2_INFO(pm_info)->phys);

    ii = 0;
    PORTMOD_PBMP_ITER(PM_8x100_GEN2_INFO(pm_info)->phys, phy) {
        if(PORTMOD_PBMP_MEMBER(port_phys_in_pm, phy)) {
            rv = PM8x100_GEN2_LANE2PORT_SET(unit, pm_info, ii, port);
            _SOC_IF_ERR_EXIT(rv);
            port_index = (port_index == -1 ? ii : port_index);
        }
        ii++;
    }

    is_pcs_bypassed = PORTMOD_PORT_ADD_F_PCS_BYPASSED_GET(add_info) ? 1 : 0;
    rv = PM8x100_GEN2_PORT_IS_PCS_BYPASSED_SET(unit, pm_info, is_pcs_bypassed, port_index);
    _SOC_IF_ERR_EXIT(rv);


    /* initalize port */
    rv = _pm8x100_gen2_pm_port_init(unit, port, pm_info, port_index, add_info, 1);
    _SOC_IF_ERR_EXIT(rv);

    /* initialze phy */
    _SOC_IF_ERR_EXIT(phymod_phy_init_config_t_init(&init_config));
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    init_config.an_en = add_info->autoneg_en;
    init_config.cl72_en = add_info->link_training_en;
    init_config.op_mode = add_info->interface_config.port_op_mode;
    my_i = 0, usr_cfg_idx = 0;
    found_first_active_lane = 0;
    for(ii=0; ii<PM8X100_GEN2_LANES_PER_CORE; ii++) {
        rv = PM8x100_GEN2_LANE2PORT_GET(unit, pm_info, ii, &port_i);
        _SOC_IF_ERR_EXIT(rv);

        if(port_i != port) {
            /* There is a difference between XGS and DNX devices. For DNX, a port can consist inconsecutive physical lanes.
             * For example, a port can consist lane 1 and 3 of the same core. For XGS, on the other hand, a port always consist
             * consecutive physical lanes. Therefore my_i below needs not be incremented for XGS devices.
             */
            if(PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy_access.device_op_mode) && found_first_active_lane){
                my_i++;
            }
            continue;
        } else{
            found_first_active_lane = 1;
        }

        if (PORTMOD_PORT_ADD_F_SKIP_SPEED_INIT_GET(add_info)) {
            /* If PORTMOD_PORT_ADD_F_SKIP_SPEED_INIT is set, set tx_taps to NRZ default */
            signalling_mode = phymodSignallingMethodNRZ;
        } else {
            _pm8x100_gen2_signalling_method_get(is_pcs_bypassed, speed_config, &signalling_mode);
        }

        if (PORTMOD_USER_SET_TX_PREEMPHASIS_BY_CONFIG_GET(add_info->init_config.tx_params_user_flag[usr_cfg_idx])) {
            init_config.tx[my_i].pre = add_info->init_config.tx_params[usr_cfg_idx].pre;
            init_config.tx[my_i].main = add_info->init_config.tx_params[usr_cfg_idx].main;
            init_config.tx[my_i].post = add_info->init_config.tx_params[usr_cfg_idx].post;
            init_config.tx[my_i].pre2 = add_info->init_config.tx_params[usr_cfg_idx].pre2;
            init_config.tx[my_i].post2 = add_info->init_config.tx_params[usr_cfg_idx].post2;
            init_config.tx[my_i].tap_mode = add_info->init_config.tx_params[usr_cfg_idx].tap_mode;
            init_config.tx[my_i].sig_method = add_info->init_config.tx_params[usr_cfg_idx].sig_method;
        } else {
            /* Need to get the default values for tx taps */
            _SOC_IF_ERR_EXIT(phymod_phy_tx_taps_default_get(&phy_access, signalling_mode, &init_config.tx[my_i]));
        }

        usr_cfg_idx++;
        my_i++;
    }

    _SOC_IF_ERR_EXIT
        (phymod_phy_init(&phy_access, &init_config));

    if (!PORTMOD_PORT_ADD_F_SKIP_SPEED_INIT_GET(add_info)) {
        _SOC_IF_ERR_EXIT(pm8x100_gen2_port_speed_config_set(unit, port, pm_info, speed_config));
    }
    else if (!is_pcs_bypassed)
    {
       _SOC_IF_ERR_EXIT(pm8x100_gen2_port_soft_reset(unit, port, pm_info, portmodMacSoftResetModeIn_Out, NULL, NULL));
    }


    /* Update WB DB */
    rv = PM8x100_GEN2_IS_ACTIVE_GET(unit, pm_info, &pm_is_active);
    _SOC_IF_ERR_EXIT(rv);
    if (!pm_is_active) {
        pm_is_active = 1;
        rv = PM8x100_GEN2_IS_ACTIVE_SET(unit, pm_info, pm_is_active);
        _SOC_IF_ERR_EXIT(rv);
    }

    rv = PM8x100_GEN2_AN_MODE_SET(unit, pm_info, init_an_mode, port_index);
    _SOC_IF_ERR_EXIT(rv);


    rv = PM8x100_GEN2_TIMESYNC_CONFIG_SET(unit, pm_info, timesync_config_default, port_index);
    _SOC_IF_ERR_EXIT(rv);

    if (add_info->init_config.parity_enable) {
        _pm8x100_gen2_port_interrupt_all_enable_set(unit, port, 1);

    }

exit:
    SOC_FUNC_RETURN;
}

/*get port cores' phymod access*/
int pm8x100_gen2_pm_core_info_get(int unit, pm_info_t pm_info, int phyn,
                                  portmod_pm_core_info_t* core_info)
{
    SOC_INIT_FUNC_DEFS;

    if(phyn < 0) {
        phyn = PM8X100_GEN2_MAX_NUM_PHYS;
    }

    core_info->ref_clk = PM_8x100_GEN2_INFO(pm_info)->ref_clk;
    sal_memcpy(&core_info->lane_map, &(PM_8x100_GEN2_INFO(pm_info)->lane_map),
               sizeof(phymod_lane_map_t));

    SOC_FUNC_RETURN;
}

/*Get PM phys.*/
int pm8x100_gen2_pm_phys_get(int unit, pm_info_t pm_info, portmod_pbmp_t* phys)
{
    SOC_INIT_FUNC_DEFS;
    PORTMOD_PBMP_ASSIGN(*phys, PM_8x100_GEN2_INFO(pm_info)->phys);
    SOC_FUNC_RETURN;
}


/*Port remove in PM level*/
int pm8x100_gen2_port_detach(int unit, int port, pm_info_t pm_info)
{
    portmod_phy_timesync_config_t ts_config;
    int port_index = -1;
    uint32_t port_lane_mask;
    int invalid_port = -1, tmp_port, i = 0, pm_id;
    int is_last_one = 1, fec_null = 0;
    const uint32 is_active = 0;
    uint8_t tvco_pll_active_lane_bm;
    uint8_t tvco_pll_adv_lane_bm;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    phymod_an_mode_type_t an_mode;
    uint32 is_bypassed, is_other_bypassed = 0;
    int other_port_index;
    uint32 other_bitmap;
    int is_last_non_bypassed, speed_id_table_status = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_periodic_callback_unregister(unit, port));

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT
        (_pm8x100_gen2_port_index_get(unit, port, pm_info, &port_index, &port_lane_mask));

    _SOC_IF_ERR_EXIT
        (pm8x100_gen2_port_enable_set(unit, port, pm_info,
         PORTMOD_PORT_ENABLE_PHY | PORTMOD_PORT_ENABLE_MAC, 0));

    /* Disable 1588 on the port. */
    ts_config.flags = 0;
    _SOC_IF_ERR_EXIT
        (pm8x100_gen2_port_timesync_config_set(unit, port,
                                               pm_info, &ts_config));
    /* Clean up WB info for the port. */
    _SOC_IF_ERR_EXIT(PM8x100_GEN2_TVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_active_lane_bm));
    _SOC_IF_ERR_EXIT(PM8x100_GEN2_TVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_adv_lane_bm));

    _SOC_IF_ERR_EXIT(PM8x100_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));

    is_last_non_bypassed = !is_bypassed;

    for (i = 0 ; i < MAX_PORTS_PER_PM8X100_GEN2; i++) {
        _SOC_IF_ERR_EXIT(PM8x100_GEN2_LANE2PORT_GET(unit, pm_info, i, &tmp_port));
        if (tmp_port == port) {
            port_index = (port_index == -1 ? i : port_index);
            _SOC_IF_ERR_EXIT(PM8x100_GEN2_LANE2PORT_SET(unit, pm_info, i, invalid_port));
            _SOC_IF_ERR_EXIT(PM8x100_GEN2_LANE2FEC_SET(unit, pm_info, i, fec_null));
            tvco_pll_active_lane_bm &= ~(1 << i);
            tvco_pll_adv_lane_bm &= ~(1 << i);
        } else if (tmp_port != invalid_port) {
            is_last_one = 0;
            if (!is_bypassed) {
                /* for DNX only - check if this is last non bypassed (not ILKN) port in order to reset MAC before powering down its clock */
                _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_index_get(unit, tmp_port, pm_info, &other_port_index, &other_bitmap));
                _SOC_IF_ERR_EXIT(PM8x100_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_other_bypassed, other_port_index));
                if (!is_other_bypassed) {
                    is_last_non_bypassed = 0;
                }
            }
        }
    }

    if (port_index == -1)
        _SOC_EXIT_WITH_ERR(SOC_E_PORT, (_SOC_MSG("Port %d wasn't found"), port));

    /* Power off PM if no port is configured on it. */
    if ((!tvco_pll_active_lane_bm) && (!tvco_pll_adv_lane_bm)) {
        /* Here we just update the PM software state */
        PM_8x100_GEN2_INFO(pm_info)->tvco = portmodVCOInvalid;

        speed_id_table_status = PM8x100_GEN2_SPEED_ID_TABLE_STATUS_NOT_LOADED;
        _SOC_IF_ERR_EXIT
            (PM8x100_GEN2_SPEED_ID_TABLE_STATUS_SET(unit, pm_info, speed_id_table_status));
    }

    _SOC_IF_ERR_EXIT(PM8x100_GEN2_TVCO_PLL_ACTIVE_LANE_BITMAP_SET(unit, pm_info, tvco_pll_active_lane_bm));
    _SOC_IF_ERR_EXIT(PM8x100_GEN2_TVCO_PLL_ADV_LANE_BITMAP_SET(unit, pm_info, tvco_pll_adv_lane_bm));

    an_mode = phymod_AN_MODE_NONE;
    _SOC_IF_ERR_EXIT(PM8x100_GEN2_AN_MODE_SET(unit, pm_info, an_mode, port_index));

    if (is_last_one) {
        _SOC_IF_ERR_EXIT(portmod_port_pm_id_get(unit, port, &pm_id));
        _SOC_IF_ERR_EXIT(PM8x100_GEN2_IS_ACTIVE_SET(unit, pm_info, is_active));
        _SOC_IF_ERR_EXIT(PM8x100_GEN2_IS_CORE_INITIALIZED_SET(unit, pm_info, is_active));

        PM_8x100_GEN2_INFO(pm_info)->int_core_access.type = phymodDispatchTypeInvalid;
    }

    if (!is_last_one && is_last_non_bypassed) {
        /* DNX only - reset MAC if last NON bypassed port is removed */  
        int pm_enable_flags = 0;
        PORTMOD_PM_ENABLE_MAC_ONLY_SET(pm_enable_flags);
        _SOC_IF_ERR_EXIT(portmod_port_pm_id_get(unit, port, &pm_id));
        _SOC_IF_ERR_EXIT(pm8x100_gen2_pm_enable(unit, pm_id, pm_info, pm_enable_flags, 0));

    }
exit:
    SOC_FUNC_RETURN;
}

/*Port replace in PM level*/
int pm8x100_gen2_port_replace(int unit, int port, pm_info_t pm_info, int new_port)
{
    int i, tmp_port;
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_periodic_callback_unregister(unit, port));
    /* replace old port with new port */
    for(i = 0; i < MAX_PORTS_PER_PM8X100_GEN2; i++) {
        rv = PM8x100_GEN2_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
        _SOC_IF_ERR_EXIT(rv);
        if (tmp_port == port) {
            rv = PM8x100_GEN2_LANE2PORT_SET(unit, pm_info, i, new_port);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

exit:
    SOC_FUNC_RETURN;
}


/*Port enable*/
int pm8x100_gen2_port_enable_set(int unit, int port, pm_info_t pm_info,
                                 int flags, int enable)
{
    int actual_flags = flags;
    int port_index;
    uint32 is_bypassed = 0, bitmap[1];
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys = 0;
    int rx_enable;
    uint64 mac_ctrl;
    uint32 soft_reset_enable;

    SOC_INIT_FUNC_DEFS;

    /* If no RX\TX flags - set both*/
    if((!PORTMOD_PORT_ENABLE_TX_GET(flags)) &&
       (!PORTMOD_PORT_ENABLE_RX_GET(flags))) {
        PORTMOD_PORT_ENABLE_RX_SET(actual_flags);
        PORTMOD_PORT_ENABLE_TX_SET(actual_flags);
    }

    /* if no MAC\Phy flags - set both*/
    if((!PORTMOD_PORT_ENABLE_PHY_GET(flags)) &&
       (!PORTMOD_PORT_ENABLE_MAC_GET(flags))) {
        PORTMOD_PORT_ENABLE_PHY_SET(actual_flags);
        PORTMOD_PORT_ENABLE_MAC_SET(actual_flags);
    }

    /*
     * if MAC is set and having only either RX or TX set
     * is invalid combination
     */
    if((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) &&
       (!PORTMOD_PORT_ENABLE_PHY_GET(actual_flags))) {

        if((!PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) ||
           (!PORTMOD_PORT_ENABLE_RX_GET(actual_flags))) {
           _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                      (_SOC_MSG("MAC RX and TX can't be enabled separately")));
        }
    }

    if(PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) {
        /* Only internal phy supported. */
        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
        _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port,
                                     pm_info, &params, 1, &phy_access,
                                     &nof_phys, NULL));
    }
    _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_index_get(unit, port, pm_info, &port_index, bitmap));
    _SOC_IF_ERR_EXIT(PM8x100_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));

    if(enable){
        if((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) && (!is_bypassed)) {

            if(!PM_8x100_GEN2_INFO(pm_info)->portmod_mac_soft_reset)
            {
                _SOC_IF_ERR_EXIT(dc3mac_port_enable_set(unit, port, 1));
            }
            else
            {
                /* Enable DC3MAC RX,TX, skip SOFT_RESET */
                _SOC_IF_ERR_EXIT(dc3mac_tx_enable_set(unit, port, 1));
                _SOC_IF_ERR_EXIT(dc3mac_rx_enable_set(unit, port, 1));
                /* Get MAC CTRL */
                _SOC_IF_ERR_EXIT(dc3mac_egress_queue_drain_get(unit, port, &mac_ctrl, &rx_enable));
                
                _SOC_IF_ERR_EXIT(pm8x100_gen2_port_soft_reset(unit, port, pm_info, portmodMacSoftResetModeOut,
                                                              &rx_enable, &mac_ctrl));
            }
        }

    if (PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) {
        if (!SAL_BOOT_SIMULATION) {
            _SOC_IF_ERR_EXIT
                (phymod_port_enable_set(&phy_access, enable));
        } else {
                if (PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) {
                    _SOC_IF_ERR_EXIT(
                        phymod_phy_tx_lane_control_set(&phy_access,
                                                       phymodTxSquelchOff));
                }
                if (PORTMOD_PORT_ENABLE_RX_GET(actual_flags)) {
                    _SOC_IF_ERR_EXIT(
                        phymod_phy_rx_lane_control_set(&phy_access,
                                                       phymodRxSquelchOff));
                }
            }
        } /* PORTMOD_PORT_ENABLE_PHY_GET */
    } else {/* disable */
        /* disable PMD RX/TX */
        if (PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) {
            if (PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) {
                _SOC_IF_ERR_EXIT(
                    phymod_phy_tx_lane_control_set(&phy_access,
                                                   phymodTxSquelchOn));
            }

            if (PORTMOD_PORT_ENABLE_RX_GET(actual_flags)) {
                _SOC_IF_ERR_EXIT(
                    phymod_phy_rx_lane_control_set(&phy_access,
                                                   phymodRxSquelchOn));
            }
        }

        /* Drain cells and reset MAC */
        if((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags))  && (!is_bypassed)) {
            if(!PM_8x100_GEN2_INFO(pm_info)->portmod_mac_soft_reset)
            {
                _SOC_IF_ERR_EXIT(dc3mac_port_enable_set(unit, port, 0));
            }
            else
            {
                /* Disable DC3MAC RX,TX, skip SOFT_RESET */
               _SOC_IF_ERR_EXIT(dc3mac_reset_get(unit, port, &soft_reset_enable));
                if (!soft_reset_enable)
                {
                    /* not in Soft reset yet */
                 _SOC_IF_ERR_EXIT(dc3mac_tx_enable_set (unit, port, 1));
                 _SOC_IF_ERR_EXIT(dc3mac_rx_enable_set (unit, port, 0));
                 _SOC_IF_ERR_EXIT(pm8x100_gen2_port_soft_reset(unit, port, pm_info, portmodMacSoftResetModeIn,
                                                               &rx_enable, &mac_ctrl));
                }
            }
        }
        /* disable phy port */
        if (PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) {
            _SOC_IF_ERR_EXIT(phymod_port_enable_set(&phy_access, enable));
        } 
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_enable_get(int unit, int port, pm_info_t pm_info,
                                 int flags, int* enable)
{
    int nof_phys = 0, port_index;
    uint32_t phy_enable = 1, mac_enable = 1;
    uint32 is_bypassed = 0, bitmap[1];
    int actual_flags = flags;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    phymod_phy_tx_lane_control_t tx_ctrl = phymodTxSquelchOn;
    phymod_phy_rx_lane_control_t rx_ctrl = phymodRxSquelchOn;
    uint32_t mac_reset = 0;

    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    /* If no RX\TX flags - set both*/
    if((!PORTMOD_PORT_ENABLE_TX_GET(flags)) &&
       (!PORTMOD_PORT_ENABLE_RX_GET(flags))) {
        PORTMOD_PORT_ENABLE_RX_SET(actual_flags);
        PORTMOD_PORT_ENABLE_TX_SET(actual_flags);
    }

    /* if no MAC\Phy flags - set both*/
    if((!PORTMOD_PORT_ENABLE_PHY_GET(flags)) &&
       (!PORTMOD_PORT_ENABLE_MAC_GET(flags))) {
        PORTMOD_PORT_ENABLE_PHY_SET(actual_flags);
        PORTMOD_PORT_ENABLE_MAC_SET(actual_flags);
    }

    if(PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) {
        /* Only internal phy supported. */
        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
        _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port,
                                     pm_info, &params, 1, &phy_access,
                                     &nof_phys, NULL));
    }

    _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_index_get(unit, port, pm_info, &port_index, bitmap));
    _SOC_IF_ERR_EXIT(PM8x100_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));




    if (PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) {
        if (!SAL_BOOT_SIMULATION) {
            _SOC_IF_ERR_EXIT
                (phymod_port_enable_get(&phy_access, &phy_enable));
        } else {
            _SOC_IF_ERR_EXIT
                (phymod_phy_tx_lane_control_get(&phy_access, &tx_ctrl));
            _SOC_IF_ERR_EXIT
                (phymod_phy_rx_lane_control_get(&phy_access, &rx_ctrl));

            if (tx_ctrl == phymodTxSquelchOn && rx_ctrl == phymodRxSquelchOn) {
                phy_enable = 0;
            } else {
                phy_enable = 1;
            }
        }
    }
    if ((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) && (!is_bypassed)) {
        _SOC_IF_ERR_EXIT
            (dc3mac_reset_get(unit, port, &mac_reset));
        if (mac_reset) {
            mac_enable = 0;
        } else {
            _SOC_IF_ERR_EXIT
                (dc3mac_rx_enable_get(unit, port, &mac_enable));
        }
    }
    *enable = (mac_enable && phy_enable);

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm8x100_gen2_fec_lanebitmap_get(int unit,
                                     pm_info_t pm_info,
                                     uint8 *rs544_bitmap,
                                     uint8 *rs272_bitmap)
{
    int i, rv;
    portmod_fec_t tmp_fec;

    SOC_INIT_FUNC_DEFS;

    *rs544_bitmap = 0;
    *rs272_bitmap = 0;
    /* Get RS528, RS544, RS272 usage bitmap from WB */
    for (i = 0 ; i < MAX_PORTS_PER_PM8X100_GEN2; i++){
        rv = PM8x100_GEN2_LANE2FEC_GET(unit, pm_info, i, &tmp_fec);
        _SOC_IF_ERR_EXIT(rv);
        if ((tmp_fec == PORTMOD_PORT_PHY_FEC_RS_544) ||
                   (tmp_fec == PORTMOD_PORT_PHY_FEC_RS_544_2XN)) {
            *rs544_bitmap |= 1 << i;
        } else if ((tmp_fec == PORTMOD_PORT_PHY_FEC_RS_272) ||
                   (tmp_fec == PORTMOD_PORT_PHY_FEC_RS_272_2XN)) {
            *rs272_bitmap |= 1 << i;
        }
    }
exit:
    SOC_FUNC_RETURN;
}

/*
 * Function:
 *_pm8x100_gen2_vco_setting_validate
 *
 * Purpose:
 *     VCOs setting validation.
 *
 * Inputs:
 *     unit:               unit number;
 *     pm_info:            pm_info data structure;
 *     ports:              Logical port bitmaps which reqiring the input vcos;
 *     required_vco:       Required vco;
 *     flag:               PLL switch flag;
 * Output:
 *     vco_setting:    If validation pass, vco_setting will indicating
 *                     what's the new vco rate and whether it is new or not;
 *                     vco_setting.num_speeds and vco_setting.speed_config_list
 *                     are no care in this function.
 */
int _pm8x100_gen2_vco_setting_validate(int unit,
                                       pm_info_t pm_info,
                                       const portmod_pbmp_t* ports,
                                       portmod_vco_type_t required_vco,
                                       int flag,
                                       portmod_pm_vco_setting_t* vco_setting)
{
    uint8_t tvco_pll_active_lane_bitmap, tvco_pll_adv_lane_bitmap;
    int is_tvco_in_use;
    portmod_vco_type_t current_tvco;
    uint32_t port_lane_mask = 0, lane_bitmap[1];
    int port, port_index;
    uint8_t active_eth_lanes = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(
            PM8x100_GEN2_TVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_active_lane_bitmap));
    _SOC_IF_ERR_EXIT(
            PM8x100_GEN2_TVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_adv_lane_bitmap));
    _SOC_IF_ERR_EXIT(
            _pm8x100_gen2_pm_active_eth_lanes_get(unit, pm_info, &active_eth_lanes));

    /*
     * Get lane mask for the input ports
     */
    PORTMOD_PBMP_ITER(*ports, port) {
        _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_index_get(unit, port, pm_info, &port_index, lane_bitmap));
        port_lane_mask |= lane_bitmap[0];
    }

    /* Remove input lanes from tvco and check if it is in use */
    tvco_pll_active_lane_bitmap &= ~port_lane_mask;
    tvco_pll_adv_lane_bitmap &= ~port_lane_mask;
    active_eth_lanes &= ~port_lane_mask;
    is_tvco_in_use = ((tvco_pll_active_lane_bitmap | tvco_pll_adv_lane_bitmap | active_eth_lanes) == 0) ? 0 : 1;
    if (is_tvco_in_use) {
        /* tvco in use (not refering to the ports that are currently requested) */
        current_tvco = PM_8x100_GEN2_INFO(pm_info)->tvco;
        if (required_vco != current_tvco) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                (_SOC_MSG("Can not accommodate VCO settings.\n request [VCO %d, state [TVCO %d]\n"), required_vco, current_tvco));
        }
    }
    else {
        current_tvco = required_vco;
    }

    /* Set output */
    vco_setting->tvco = current_tvco;
    vco_setting->is_tvco_new = (PM_8x100_GEN2_INFO(pm_info)->tvco == current_tvco) ? 0 : 1;

exit:
    SOC_FUNC_RETURN;
}

/*
 * Validate if a set of speed can be configured for given PORTMACRO without
 * affecting the other active ports.
 *
 * Inputs:
 *     unit:           unit number;
 *     pm_id           portmacro ID;
 *     pm_info:        pm_info data structure;
 *     ports:          Logical port bitmaps which reqiring the input vcos;
 *     flag:           PLL switch flag.
 *
 * Output:
 *     vco_setting:    If validation pass, vco_setting will indicating
 *                     what's the new vco rates and whether they are new or not;
 *
 */
int pm8x100_gen2_pm_speed_config_validate(int unit,
                                          int pm_id,
                                          pm_info_t pm_info,
                                          const portmod_pbmp_t* ports,
                                          int flag,
                                          portmod_pm_vco_setting_t* vco_setting)
{
    int i, rv;
    portmod_vco_type_t vco, required_vco = portmodVCOInvalid;
    int is_pcs_bypass;
    uint8_t rs528_bm = 0, rs544_bm = 0, rs272_bm = 0, affected_bm = 0;
    int nof_phys, port;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;

    SOC_INIT_FUNC_DEFS;

    /* Get RS FEC usage bitmaps from WB */
    _SOC_IF_ERR_EXIT(_pm8x100_gen2_fec_lanebitmap_get(unit, pm_info,&rs544_bm, &rs272_bm));

    /* Remove the input ports from FEC usage bitmaps*/
    PORTMOD_PBMP_ITER(*ports, port) {
        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
        _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                    &params, 1, &phy_access, &nof_phys, NULL));
        rs528_bm &= ~phy_access.access.lane_mask;
        rs544_bm &= ~phy_access.access.lane_mask;
        rs272_bm &= ~phy_access.access.lane_mask;
    }

    /* Get the VCOs required by the input speed config list */
    for (i = 0; i < vco_setting->num_speeds; i++) {
        vco = portmodVCOInvalid;
        rv = _pm8x100_gen2_port_speed_config_to_vco_get(&(vco_setting->speed_config_list[i]),
                                                  vco_setting->speed_for_pcs_bypass_port[i],
                                                  &vco);
        if (rv == SOC_E_CONFIG) {
            _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                      (_SOC_MSG("Can not support speed: %d num_lane: %d fec: %d"),
                      vco_setting->speed_config_list[i].speed,
                      vco_setting->speed_config_list[i].num_lane,
                      vco_setting->speed_config_list[i].fec));
        }
        /* Can not accommodate more than one VCO */
        if (required_vco == portmodVCOInvalid) {
            required_vco = vco;
        } else if (required_vco != vco) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                (_SOC_MSG("Can not accommodate more than one VCO.\n")));
        }
        is_pcs_bypass = vco_setting->speed_for_pcs_bypass_port[i];

        if (!is_pcs_bypass) {
            if ((vco_setting->speed_config_list[i].fec == PORTMOD_PORT_PHY_FEC_RS_272) ||
                (vco_setting->speed_config_list[i].fec == PORTMOD_PORT_PHY_FEC_RS_272_2XN)) {
                _SOC_IF_ERR_EXIT(_pm8x100_gen2_lanebitmap_set(vco_setting->port_starting_lane_list[i],
                                                        vco_setting->speed_config_list[i].num_lane,
                                                        &rs272_bm));
            } else if ((vco_setting->speed_config_list[i].fec == PORTMOD_PORT_PHY_FEC_RS_544) ||
                       (vco_setting->speed_config_list[i].fec == PORTMOD_PORT_PHY_FEC_RS_544_2XN)) {
                _SOC_IF_ERR_EXIT(_pm8x100_gen2_lanebitmap_set(vco_setting->port_starting_lane_list[i],
                                                        vco_setting->speed_config_list[i].num_lane,
                                                        &rs544_bm));
            } else if (vco_setting->speed_config_list[i].fec == PORTMOD_PORT_PHY_FEC_RS_FEC) {
                _SOC_IF_ERR_EXIT(_pm8x100_gen2_lanebitmap_set(vco_setting->port_starting_lane_list[i],
                                                        vco_setting->speed_config_list[i].num_lane,
                                                        &rs528_bm));
            }
        }

    }

    /* Validate FEC settings for RS528, RS544 and RS272 */
    _SOC_IF_ERR_EXIT(_pm8x100_gen2_fec_validate(unit, rs544_bm, rs272_bm, &affected_bm));

    _SOC_IF_ERR_EXIT(_pm8x100_gen2_vco_setting_validate(unit, pm_info, ports, required_vco, flag, vco_setting));

exit:
    SOC_FUNC_RETURN;
}

/*Port speed validation.*/
int pm8x100_gen2_port_speed_config_validate(int unit,
                                            int port,
                                            pm_info_t pm_info,
                                            const portmod_speed_config_t* speed_config,
                                            portmod_pbmp_t* affected_pbmp)
{
    portmod_vco_type_t cur_vco = portmodVCOInvalid, req_vco = portmodVCOInvalid;
    uint8_t rs544_req = 0, rs272_req = 0, vco_change_allowed = 0;
    uint8_t rs544_bm = 0, rs272_bm = 0;
    uint32_t pll_div = 0, is_pll_pwrdn = 1;
    int nof_phys, pm_id = 0;
    int rv, i, tmp_port, port_index, port_is_pcs_bypassed;
    uint8_t tvco_pll_active_lane_bitmap;
    uint32_t port_lane_mask[1];
    uint8_t affected_lane_bitmap = 0;
    phymod_phy_access_t phy_access, phy_copy;
    portmod_access_get_params_t params;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT
        (pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                         &params, 1, &phy_access, &nof_phys, NULL));
    /* clear the affected_pbmp first */
    PORTMOD_PBMP_CLEAR(*affected_pbmp);

    /* Get lane_mask and number of lanes for the port from DB */
    _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_index_get(unit, port, pm_info, &port_index, port_lane_mask));
    _SOC_IF_ERR_EXIT(PM8x100_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &port_is_pcs_bypassed, port_index));

    if (!port_is_pcs_bypassed) {
        /* 1. Check lane_map. */
        rv = _pm8x100_gen2_pm_port_lane_map_validate(unit, port_lane_mask[0]);
        if (rv != SOC_E_NONE) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Invalid lane map request on exising logical port.")));
        }

        /*
         * 2. Validate force speed ability.
         * Check if port speed configuration is one of the entries of
         * force_speed_ability table.
         */
        rv = _pm8x100_gen2_port_speed_config_to_vco_get(speed_config, port_is_pcs_bypassed, &req_vco);
        if (rv == SOC_E_CONFIG) {
            _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                      (_SOC_MSG("Speed config is not supported")));
        }

        /* 3. check 10G speed with link training enabled */
        if ((speed_config->link_training) && (speed_config->speed == 10000)) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Link training is not supported for this speed %"PRIu32".\n"), speed_config->speed));
        }

        /* 4. Validate FEC settings on each MPP. */
        if ((speed_config->fec == PORTMOD_PORT_PHY_FEC_RS_544) ||
            (speed_config->fec == PORTMOD_PORT_PHY_FEC_RS_544_2XN)) {
            rs544_req = 1;
        } else if ((speed_config->fec == PORTMOD_PORT_PHY_FEC_RS_272) ||
            (speed_config->fec == PORTMOD_PORT_PHY_FEC_RS_272_2XN)) {
            rs272_req = 1;
        }

        if (rs544_req || rs272_req) {
            /* Get RS544, RS272 usage bitmap from WB */
            _SOC_IF_ERR_EXIT(_pm8x100_gen2_fec_lanebitmap_get(unit, pm_info, &rs544_bm, &rs272_bm));

            rs544_bm &= ~port_lane_mask[0];
            rs272_bm &= ~port_lane_mask[0];
            if (rs544_req) {
                rs544_bm |= port_lane_mask[0];
            }
            if (rs272_req) {
                rs272_bm |= port_lane_mask[0];
            }
            rv = _pm8x100_gen2_fec_validate(unit, rs544_bm, rs272_bm, &affected_lane_bitmap);
            if (rv != SOC_E_NONE) {
                /* If FEC validate fails, return affected port bit map */
                affected_lane_bitmap &= ~port_lane_mask[0];
                for(i = 0 ; i < MAX_PORTS_PER_PM8X100_GEN2; i++){
                    if ((affected_lane_bitmap >> i) & 1) {
                        _SOC_IF_ERR_EXIT(PM8x100_GEN2_LANE2PORT_GET(unit, pm_info, i, &tmp_port));
                        PORTMOD_PBMP_PORT_ADD(*affected_pbmp, tmp_port);
                    }
                }
                return rv;
            }
        }
    }

    /* 5. Check requested VCO is valid on the PM. */
    _SOC_IF_ERR_EXIT(
        PM8x100_GEN2_TVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_active_lane_bitmap));
    if ((tvco_pll_active_lane_bitmap == port_lane_mask[0]) ||
        (tvco_pll_active_lane_bitmap == 0x0)) {
        vco_change_allowed = 1;
    }

    /* Use the first lane. */
    sal_memcpy(&phy_copy, &phy_access, sizeof(phy_access));
    phy_copy.access.lane_mask = 0x1;

    _SOC_IF_ERR_EXIT(phymod_phy_pll_powerdown_get(&phy_copy, 0, &is_pll_pwrdn));
    if (is_pll_pwrdn) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("The VCO is powered off on port %"PRIu32".\n"), port));
    } else {
        _SOC_IF_ERR_EXIT(phymod_phy_pll_multiplier_get(&phy_copy, &pll_div));
        _SOC_IF_ERR_EXIT(_pm8x100_gen2_pll_to_vco_get(pll_div, &cur_vco));
    }

    if (req_vco != cur_vco) {
        if (vco_change_allowed) {
            /*
             * PM reconfigure the VCO based on the new speed once the
             * validation passes.
             */
            _SOC_IF_ERR_EXIT(portmod_port_pm_id_get(unit, port, &pm_id));
            _SOC_IF_ERR_EXIT
                (pm8x100_gen2_pm_vco_reconfig(unit, pm_id, pm_info, &req_vco));
        } else {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Requested VCO %"PRIu32" for speed "
                                 "%"PRIu32" cannot be configured on current "
                                 "settings VCO %"PRIu32"\n"),
                                 req_vco, speed_config->speed, cur_vco));

        }
   }

exit:
    SOC_FUNC_RETURN;
}

/*
 * This API reconfig the VCO rates of the PM.
 *
 * 'pm8x100_gen2_pm_speed_config_validate' need to be called before this function.
 * Only when pm8x100_gen2_pm_speed_config_validate reuturns E_NONE and indicates
 * VCO change requirement, user can call this API to change VCO.
 *
 * vco[0]: TVCO.
 */
int pm8x100_gen2_pm_vco_reconfig(int unit,
                                 int pm_id,
                                 pm_info_t pm_info,
                                 const portmod_vco_type_t* vco)
{
    uint8_t zero = 0, active_eth_lanes = 0;
    portmod_vco_type_t cur_tvco = PM_8x100_GEN2_INFO(pm_info)->tvco;
    uint32_t pll_div;
    phymod_phy_access_t phy_access;
    int fec_null = 0, i;
    uint8_t tvco_pll_active_lane_bitmap, tvco_pll_adv_lane_bitmap;
    int speed_id_table_status = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(PM8x100_GEN2_TVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_active_lane_bitmap));
    _SOC_IF_ERR_EXIT(PM8x100_GEN2_TVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_adv_lane_bitmap));
    _SOC_IF_ERR_EXIT(_pm8x100_gen2_pm_active_eth_lanes_get(unit, pm_info, &active_eth_lanes));

    if (cur_tvco != vco[0]) {
        /* TVCO need to be changed */
        PM_8x100_GEN2_INFO(pm_info)->tvco = vco[0];
        /* next get the TVCO's PLL divider */
        _SOC_IF_ERR_EXIT(_pm8x100_gen2_vco_to_pll_get(vco[0], &pll_div));

        sal_memcpy(&phy_access, &(PM_8x100_GEN2_INFO(pm_info)->int_core_access),
                   sizeof(phymod_phy_access_t));
        if (pll_div != phymod_TSCBH_PLL_DIVNONE) {
            if (active_eth_lanes == (tvco_pll_adv_lane_bitmap | tvco_pll_active_lane_bitmap)) {
                /*
                 * Set lane_mask to 0xFF for the following two cases:
                 *
                 * 1. If active_eth_lanes != 0, meaning there are only ethernet ports
                 *    in the PM, it is safe to set lane_mask to 0xFF.
                 * 2. If active_eth_lanes == 0, meaning there is no active ports in
                 *    the PM, it is safe to set lane_mask to 0xFF.
                 */
                phy_access.access.lane_mask = 0xFF;
            } else {
                /*
                 * There are ILKN ports in the PM, lane_mask should include:
                 *
                 * 1. All ethernet lanes
                 * 2. ILKN lanes using TVCO.
                 */
                phy_access.access.lane_mask = active_eth_lanes | tvco_pll_active_lane_bitmap;
            }
            _SOC_IF_ERR_EXIT(
                phymod_phy_pll_reconfig(&phy_access, 0, pll_div, PM_8x100_GEN2_INFO(pm_info)->ref_clk));
        } else {
             /* will pwoer down this PLL */
             _SOC_IF_ERR_EXIT(phymod_phy_pll_pwrdn(&phy_access, 0, 1));
        }
        /* For Ethernet mode, TVCO change will reset the whole serdes core.
         * So we will clear:
         *     1. the lane bitmaps for both PLLs.
         *     2. FEC type for all lanes.
         */
        _SOC_IF_ERR_EXIT(PM8x100_GEN2_TVCO_PLL_ACTIVE_LANE_BITMAP_SET(unit, pm_info, zero));
        _SOC_IF_ERR_EXIT(PM8x100_GEN2_TVCO_PLL_ADV_LANE_BITMAP_SET(unit, pm_info, zero));
        for (i = 0 ; i < MAX_PORTS_PER_PM8X100_GEN2; i++) {
            _SOC_IF_ERR_EXIT(PM8x100_GEN2_LANE2FEC_SET(unit, pm_info, i, fec_null));
        }
        speed_id_table_status = PM8x100_GEN2_SPEED_ID_TABLE_STATUS_NOT_LOADED;
        _SOC_IF_ERR_EXIT(
            PM8x100_GEN2_SPEED_ID_TABLE_STATUS_SET(unit, pm_info, speed_id_table_status));
    }


exit:
    SOC_FUNC_RETURN;
}

/* set/get the speed config for the specified port.*/
int pm8x100_gen2_port_speed_config_set(int unit, int port, pm_info_t pm_info, const portmod_speed_config_t* speed_config)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    uint64 mac_ctrl;
    phymod_phy_signalling_method_t signalling_mode;
    int nof_phys, i, rx_enable = 0, port_is_pcs_bypassed;
    portmod_pbmp_t affected_pbmp;
    portmod_pmd_lane_config_t portmod_lane_config;
    phymod_phy_speed_config_t phy_speed_config;
    phymod_phy_pll_state_t old_pll_state, new_pll_state;
    uint32 lane_config, port_lane_mask;
    uint8 tvco_pll_lanes_bitmap;
    portmod_vco_type_t required_vco;
    int port_index, is_bypassed = 0, flags = 0;
    phymod_an_mode_type_t an_mode = phymod_AN_MODE_NONE;
    portmod_encap_t encap;

    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(mac_ctrl);
    _SOC_IF_ERR_EXIT
        (phymod_phy_speed_config_t_init(&phy_speed_config));
    _SOC_IF_ERR_EXIT
        (phymod_phy_pll_state_t_init(&old_pll_state));
    _SOC_IF_ERR_EXIT
        (phymod_phy_pll_state_t_init(&new_pll_state));

    _SOC_IF_ERR_EXIT(portmod_pmd_lane_config_t_init(unit, &portmod_lane_config));
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_index_get(unit, port, pm_info, &port_index, &port_lane_mask));
    _SOC_IF_ERR_EXIT(PM8x100_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));
    if(!PM_8x100_GEN2_INFO(pm_info)->portmod_mac_soft_reset)
    {
        /* Get original MAC CTRL */
        _SOC_IF_ERR_EXIT(dc3mac_egress_queue_drain_get(unit, port, &mac_ctrl, &rx_enable));

        /* Disable MAC RX, drain MAC FIFO, assert MAC SOFT_RESET */
        _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_mac_drain_soft_reset(unit, port));
    }
    else if (!is_bypassed)
    {
       _SOC_IF_ERR_EXIT(pm8x100_gen2_port_soft_reset(unit, port, pm_info, portmodMacSoftResetModeIn, &rx_enable, &mac_ctrl));
    }

    /* Validate speed_config */
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_speed_config_validate(unit, port, pm_info, speed_config, &affected_pbmp));

    _SOC_IF_ERR_EXIT(
        PM8x100_GEN2_TVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_lanes_bitmap));
    old_pll_state.pll0_lanes_bitmap = tvco_pll_lanes_bitmap;
    port_is_pcs_bypassed = PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy_access.device_op_mode);
    _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_speed_config_to_vco_get(speed_config, port_is_pcs_bypassed, &required_vco));

    port_lane_mask = phy_access.access.lane_mask;

    if (required_vco == PM_8x100_GEN2_INFO(pm_info)->tvco) {
        tvco_pll_lanes_bitmap |= port_lane_mask;
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("VCO need to be reconfigured before changing speed.")));
    }
    /* Decode the lane_config in speed_config */
    lane_config = speed_config->lane_config;
    /* Retrieve the default lane config */
    _pm8x100_gen2_signalling_method_get(is_bypassed, speed_config, &signalling_mode);
    if (lane_config == -1) {
        _SOC_IF_ERR_EXIT(
            phymod_phy_lane_config_default_get(&phy_access, signalling_mode, &portmod_lane_config.pmd_firmware_lane_config));
        portmod_lane_config.pam4_channel_loss = 0;
    } else {
        if (signalling_mode != phymodSignallingMethodNRZ) {
            PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_PAM4_SET(lane_config);
            PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_NRZ_CLEAR(lane_config);
        } else {
            PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_NRZ_SET(lane_config);
            PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_PAM4_CLEAR(lane_config);
            PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_ES_CLEAR(lane_config);
            PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_NS_CLEAR(lane_config);
        }
        _SOC_IF_ERR_EXIT(
            portmod_common_pmd_lane_config_decode(lane_config, &portmod_lane_config));
    }

    if (!is_bypassed) {
        if (speed_config->fec == PORTMOD_PORT_PHY_FEC_NONE) {
            phy_speed_config.fec_type = phymod_fec_None;
        } else if (speed_config->fec ==  PORTMOD_PORT_PHY_FEC_RS_FEC) {
            phy_speed_config.fec_type = phymod_fec_CL91;
        } else if (speed_config->fec ==  PORTMOD_PORT_PHY_FEC_RS_544) {
            phy_speed_config.fec_type = phymod_fec_RS544;
        } else if (speed_config->fec == PORTMOD_PORT_PHY_FEC_RS_272) {
            phy_speed_config.fec_type = phymod_fec_RS272;
        } else if (speed_config->fec == PORTMOD_PORT_PHY_FEC_RS_544_2XN) {
            phy_speed_config.fec_type = phymod_fec_RS544_2XN;
        } else if (speed_config->fec == PORTMOD_PORT_PHY_FEC_RS_272_2XN) {
            phy_speed_config.fec_type = phymod_fec_RS272_2XN;
        }
    }

    phy_speed_config.data_rate = speed_config->speed;
    phy_speed_config.linkTraining = speed_config->link_training;
    phy_speed_config.PAM4_channel_loss = portmod_lane_config.pam4_channel_loss;
    phy_speed_config.pmd_lane_config = portmod_lane_config.pmd_firmware_lane_config;
    _SOC_IF_ERR_EXIT
        (dc3port_port_mode_set(unit, port, flags, phy_access.access.lane_mask));

    _SOC_IF_ERR_EXIT
        (phymod_phy_speed_config_set(&phy_access, &phy_speed_config, &old_pll_state,
                                     &new_pll_state));

    _SOC_IF_ERR_EXIT
        (dc3mac_encap_get(unit, port, &encap));
    /*
     * For Higig3 port, need to program codec mode to
     * ReducedIPG mode after speed change.
     */
    /*
     * Both SOC_ENCAP_HIGIG3 and BCMPMAC_ENCAP_IEEE_REDUCED_IPG
     * set the same mode in DC3MAC_MODRr.HDR_MODE.
     * Checking ENCAP_HG3 type here, which means REDUCED_PREAMBLE mode is set.
     */
    if (encap == SOC_ENCAP_HIGIG3) {
        _SOC_IF_ERR_EXIT
            (phymod_phy_codec_mode_set(&phy_access, phymodCodecReducedIPG));
    }


    if(!PM_8x100_GEN2_INFO(pm_info)->portmod_mac_soft_reset)
    {
        /* Enable MAC RX, De-assert MAC SOFT_RESET */
        _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_rx_restore_mac_out_of_reset(unit, port, rx_enable));
        /* Restore original MAC CONTROL */
        _SOC_IF_ERR_EXIT(dc3mac_mac_ctrl_set(unit, port, mac_ctrl));
    }
    else if (!is_bypassed)
    {
        _SOC_IF_ERR_EXIT(pm8x100_gen2_port_soft_reset(unit, port, pm_info, portmodMacSoftResetModeOut, &rx_enable, &mac_ctrl));
    }

    /* Update PLL active lane bitmaps */
    _SOC_IF_ERR_EXIT(
        PM8x100_GEN2_TVCO_PLL_ACTIVE_LANE_BITMAP_SET(unit, pm_info, tvco_pll_lanes_bitmap));

    /* Clear the adv lane bitmap for the port because it's configured as a force speed port */
    _SOC_IF_ERR_EXIT(
        PM8x100_GEN2_TVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_lanes_bitmap));
    port_lane_mask = phy_access.access.lane_mask;
    tvco_pll_lanes_bitmap &= ~port_lane_mask;
    _SOC_IF_ERR_EXIT(
        PM8x100_GEN2_TVCO_PLL_ADV_LANE_BITMAP_SET(unit, pm_info, tvco_pll_lanes_bitmap));

    /* Clear an_mode for the port to force user set new abilities before enable AN later on */
    _SOC_IF_ERR_EXIT(PM8x100_GEN2_AN_MODE_SET(unit, pm_info, an_mode, port_index));

    if (!is_bypassed) {
        /* Update FEC usage in WB */
        for (i = 0 ; i < MAX_PORTS_PER_PM8X100_GEN2; i++) {
            if ((port_lane_mask >> i) & 0x1) {
                _SOC_IF_ERR_EXIT(PM8x100_GEN2_LANE2FEC_SET(unit, pm_info, i, speed_config->fec));
            }
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_speed_config_get(int unit, int port, pm_info_t pm_info, portmod_speed_config_t* speed_config)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, port_num_lanes;
    uint32_t lane_config;
    portmod_pmd_lane_config_t portmod_lane_config;
    phymod_phy_speed_config_t phy_speed_config;
    phymod_autoneg_status_t an_status;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    PORTMOD_BIT_COUNT(phy_access.access.lane_mask, port_num_lanes);

    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_autoneg_status_get(unit, port, pm_info, &an_status));

    if (an_status.enabled && an_status.locked) {
        speed_config->num_lane = an_status.resolved_num_lane;
    } else {
        speed_config->num_lane = port_num_lanes;
    }

    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_t_init(&phy_speed_config));
    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_get(&phy_access, &phy_speed_config));

    portmod_lane_config.pam4_channel_loss = phy_speed_config.PAM4_channel_loss;
    portmod_lane_config.pmd_firmware_lane_config = phy_speed_config.pmd_lane_config;
    /* Encode the lane_config in speed_config */
    _SOC_IF_ERR_EXIT(
        portmod_common_pmd_lane_config_encode(&portmod_lane_config, &lane_config));

    if (phy_speed_config.fec_type == phymod_fec_None) {
        speed_config->fec = PORTMOD_PORT_PHY_FEC_NONE;
    } else if (phy_speed_config.fec_type == phymod_fec_CL91) {
        speed_config->fec = PORTMOD_PORT_PHY_FEC_RS_FEC;
    } else if (phy_speed_config.fec_type == phymod_fec_RS544) {
        speed_config->fec = PORTMOD_PORT_PHY_FEC_RS_544;
    } else if (phy_speed_config.fec_type == phymod_fec_RS272) {
        speed_config->fec = PORTMOD_PORT_PHY_FEC_RS_272;
    } else if (phy_speed_config.fec_type == phymod_fec_RS544_2XN) {
        speed_config->fec = PORTMOD_PORT_PHY_FEC_RS_544_2XN;
    } else if (phy_speed_config.fec_type == phymod_fec_RS272_2XN) {
        speed_config->fec = PORTMOD_PORT_PHY_FEC_RS_272_2XN;
    }

    speed_config->speed = phy_speed_config.data_rate;
    speed_config->link_training = phy_speed_config.linkTraining;
    speed_config->lane_config = lane_config;

exit:
    SOC_FUNC_RETURN;
}

/*Port cl72 set\get*/
int pm8x100_gen2_port_cl72_set(int unit, int port, pm_info_t pm_info, uint32 enable)
{
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    if (!SAL_BOOT_SIMULATION) {
        _SOC_IF_ERR_EXIT
            (phymod_phy_cl72_set(&phy_access, enable));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_cl72_get(int unit, int port, pm_info_t pm_info, uint32* enable)
{
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    if (!SAL_BOOT_SIMULATION) {
        _SOC_IF_ERR_EXIT
            (phymod_phy_cl72_get(&phy_access, enable));
    }

exit:
    SOC_FUNC_RETURN;
}

/*Get port cl72 status*/
int pm8x100_gen2_port_cl72_status_get(int unit, int port, pm_info_t pm_info,
                                      phymod_cl72_status_t* status)
{
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    if (!SAL_BOOT_SIMULATION) {
        _SOC_IF_ERR_EXIT
            (phymod_phy_cl72_status_get(&phy_access, status));
    }

exit:
    SOC_FUNC_RETURN;
}

/*Port loopback set\get*/
int pm8x100_gen2_port_loopback_set(int unit, int port, pm_info_t pm_info,
                                   portmod_loopback_mode_t loopback_type, int enable)
{
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;

    SOC_INIT_FUNC_DEFS;

    switch (loopback_type) {
        case portmodLoopbackPhyGloopPMD:
            _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
            _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                             &params, 1, &phy_access, &nof_phys, NULL));
            _SOC_IF_ERR_EXIT(phymod_phy_loopback_set(&phy_access, phymodLoopbackGlobal, enable));
           break;
        case portmodLoopbackPhyRloopPMD:
            _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
            _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                             &params, 1, &phy_access, &nof_phys, NULL));
            _SOC_IF_ERR_EXIT(phymod_phy_loopback_set(&phy_access, phymodLoopbackRemotePMD, enable));
           break;
        case portmodLoopbackPhyGloopPCS:
        case portmodLoopbackPhyRloopPCS:
        case portmodLoopbackMacOuter:
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (
                      _SOC_MSG("unsupported loopback type %d"), loopback_type));
           break;
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_loopback_get(int unit, int port, pm_info_t pm_info,
                                   portmod_loopback_mode_t loopback_type,
                                   int* enable)
{
    portmod_access_get_params_t params;
    int nof_phys;
    phymod_phy_access_t phy_access;
    uint32_t lpbk_en = 0;

    SOC_INIT_FUNC_DEFS;

    switch (loopback_type) {
        case portmodLoopbackPhyGloopPMD:
            _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
            _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                             &params, 1, &phy_access, &nof_phys, NULL));
            _SOC_IF_ERR_EXIT
                (phymod_phy_loopback_get(&phy_access, phymodLoopbackGlobal,
                                         &lpbk_en));
            break;
        case portmodLoopbackPhyRloopPMD:
            _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
            _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                             &params, 1, &phy_access, &nof_phys, NULL));
            _SOC_IF_ERR_EXIT
                (phymod_phy_loopback_get(&phy_access, phymodLoopbackRemotePMD,
                                         &lpbk_en));
            break;
        case portmodLoopbackMacOuter:
        case portmodLoopbackPhyGloopPCS:
        case portmodLoopbackPhyRloopPCS:
        default:
            break;
    }

    *enable = lpbk_en;

exit:
    SOC_FUNC_RETURN;
}

/*Port RX MAC ENABLE set\get*/
int pm8x100_gen2_port_rx_mac_enable_set(int unit, int port,
                                        pm_info_t pm_info, int enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(dc3mac_rx_enable_set(unit, port, enable));
exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_rx_mac_enable_get(int unit, int port,
                                        pm_info_t pm_info, int* enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(dc3mac_rx_enable_get(unit, port, (uint32_t *)enable));
exit:
    SOC_FUNC_RETURN;
}

/*Port TX MAC ENABLE set\get*/
int pm8x100_gen2_port_tx_mac_enable_set(int unit, int port,
                                        pm_info_t pm_info,
                                        int enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(dc3mac_tx_enable_set(unit, port, enable));

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_tx_mac_enable_get(int unit, int port,
                                        pm_info_t pm_info,
                                        int* enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(dc3mac_tx_enable_get(unit, port, (uint32_t *)enable));

exit:
    SOC_FUNC_RETURN;
}

/*
 * This API looks at the current port's number of lanes and max speed,
 * returns the complete list of force-speed abilities with the same number
 * of lanes and speed that is <= max speed, and the complete list of autoneg
 * abilities with number of lanes <= current port's number of lanes and
 * speed <= max speed.
 */
int pm8x100_gen2_port_speed_ability_local_get(int unit, int port, pm_info_t pm_info,
                                              int max_num_abilities,
                                              portmod_port_speed_ability_t* abilities,
                                              int* num_abilities)
{
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys, port_num_lanes, port_lane_mask, idx = 0;
    const pm8x100_gen2_fs_ability_table_entry_t *fs_abil = NULL;
    const pm8x100_gen2_an_ability_table_entry_t *an_abil = NULL;

   SOC_INIT_FUNC_DEFS;

    /* find number of lanes of the port */
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    port_lane_mask = phy_access.access.lane_mask;
    PORTMOD_BIT_COUNT(port_lane_mask, port_num_lanes);

    for (idx = 0; idx < COUNTOF(pm8x100_gen2_fs_ability_table); idx++) {
        fs_abil = &pm8x100_gen2_fs_ability_table[idx];
        if (fs_abil->num_lanes == (uint32_t)port_num_lanes) {
            if (*num_abilities < max_num_abilities) {
                abilities[*num_abilities].speed = fs_abil->speed;
                abilities[*num_abilities].num_lanes = fs_abil->num_lanes;
                abilities[*num_abilities].fec_type = fs_abil->fec_type;
                abilities[*num_abilities].medium = PORTMOD_PORT_PHY_MEDIUM_ALL;
                abilities[*num_abilities].pause = PORTMOD_PORT_PHY_PAUSE_ALL;
                abilities[*num_abilities].channel = PORTMOD_PORT_PHY_CHANNEL_ALL;
                abilities[*num_abilities].an_mode = PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_NONE;
                (*num_abilities) += 1;
            } else {
                break;
            }
        }
    }

    for (idx = 0; idx < COUNTOF(pm8x100_gen2_an_ability_table); idx++) {
        an_abil = &pm8x100_gen2_an_ability_table[idx];
        if (an_abil->num_lanes <= (uint32_t)port_num_lanes) {
            if (*num_abilities < max_num_abilities) {
                abilities[*num_abilities].speed = an_abil->speed;
                abilities[*num_abilities].num_lanes = an_abil->num_lanes;
                abilities[*num_abilities].fec_type = an_abil->fec_type;
                abilities[*num_abilities].medium = PORTMOD_PORT_PHY_MEDIUM_ALL;
                abilities[*num_abilities].pause = PORTMOD_PORT_PHY_PAUSE_ALL;
                abilities[*num_abilities].channel = PORTMOD_PORT_PHY_CHANNEL_ALL;
                abilities[*num_abilities].an_mode = an_abil->an_mode;
                (*num_abilities) += 1;
            } else {
                break;
            }
        }
    }


exit:
    SOC_FUNC_RETURN;
}

/* Translate portmod_port_speed_ability_t to phymod_autoneg_advert_abilities_t */
int _pm8x100_gen2_port_port_to_phy_ability(int num_abilities,
                                           const portmod_port_speed_ability_t* abilities,
                                           phymod_autoneg_advert_abilities_t* an_advert_abilities)
{
    int i;

    an_advert_abilities->num_abilities = num_abilities;
    for (i = 0; i < num_abilities; i++) {
        an_advert_abilities->autoneg_abilities[i].speed = abilities[i].speed;
        an_advert_abilities->autoneg_abilities[i].resolved_num_lanes = abilities[i].num_lanes;
        switch (abilities[i].fec_type) {
            case PORTMOD_PORT_PHY_FEC_NONE:
                an_advert_abilities->autoneg_abilities[i].fec = phymod_fec_None;
                break;
            case PORTMOD_PORT_PHY_FEC_RS_FEC:
                an_advert_abilities->autoneg_abilities[i].fec = phymod_fec_CL91;
                break;
            case PORTMOD_PORT_PHY_FEC_RS_544:
                an_advert_abilities->autoneg_abilities[i].fec = phymod_fec_RS544;
                break;
            case PORTMOD_PORT_PHY_FEC_RS_544_2XN:
                an_advert_abilities->autoneg_abilities[i].fec = phymod_fec_RS544_2XN;
                break;
            case PORTMOD_PORT_PHY_FEC_RS_272:
                an_advert_abilities->autoneg_abilities[i].fec = phymod_fec_RS272;
                break;
            case PORTMOD_PORT_PHY_FEC_RS_272_2XN:
                an_advert_abilities->autoneg_abilities[i].fec = phymod_fec_RS272_2XN;
                break;
            default:
                 return SOC_E_CONFIG;
        }
        switch (abilities[i].pause) {
            case PORTMOD_PORT_PHY_PAUSE_NONE:
                an_advert_abilities->autoneg_abilities[i].pause = phymod_pause_none;
                break;
            case PORTMOD_PORT_PHY_PAUSE_TX:
                an_advert_abilities->autoneg_abilities[i].pause = phymod_pause_asym;
                break;
            case PORTMOD_PORT_PHY_PAUSE_RX:
                an_advert_abilities->autoneg_abilities[i].pause = phymod_pause_asym_symm;
                break;
            case PORTMOD_PORT_PHY_PAUSE_SYMM:
                an_advert_abilities->autoneg_abilities[i].pause = phymod_pause_symm;
                break;
            default:
                return SOC_E_CONFIG;
        }
        switch (abilities[i].an_mode) {
            case PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73:
                an_advert_abilities->autoneg_abilities[i].an_mode = phymod_AN_MODE_CL73;
                break;
            case PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM:
                an_advert_abilities->autoneg_abilities[i].an_mode = phymod_AN_MODE_CL73BAM;
                break;
            case PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_MSA:
                an_advert_abilities->autoneg_abilities[i].an_mode = phymod_AN_MODE_MSA;
                break;
            default:
                return SOC_E_CONFIG;
        }
        if (abilities[i].medium == PORTMOD_PORT_PHY_MEDIUM_COPPER) {
            an_advert_abilities->autoneg_abilities[i].medium = phymodFirmwareMediaTypeCopperCable;
        } else {
            an_advert_abilities->autoneg_abilities[i].medium = phymodFirmwareMediaTypePcbTraceBackPlane;
        }
        if (abilities[i].channel == PORTMOD_PORT_PHY_CHANNEL_SHORT) {
            an_advert_abilities->autoneg_abilities[i].channel = phymod_channel_short;
        } else {
            an_advert_abilities->autoneg_abilities[i].channel = phymod_channel_long;
        }
    }

    return SOC_E_NONE;
}

/* Translate phymod_autoneg_advert_abilities_t to portmod_port_speed_ability_t */
int _pm8x100_gen2_port_phy_to_port_ability(phymod_autoneg_advert_abilities_t* an_advert_abilities,
                                           int* num_abilities,
                                           portmod_port_speed_ability_t* abilities)
{
    int i;

    *num_abilities = an_advert_abilities->num_abilities;
    for (i = 0; i < *num_abilities; i++) {
        abilities[i].speed = an_advert_abilities->autoneg_abilities[i].speed;
        abilities[i].num_lanes = an_advert_abilities->autoneg_abilities[i].resolved_num_lanes;
        switch (an_advert_abilities->autoneg_abilities[i].fec) {
            case phymod_fec_None:
                abilities[i].fec_type = PORTMOD_PORT_PHY_FEC_NONE;
                break;
            case phymod_fec_CL91:
                abilities[i].fec_type = PORTMOD_PORT_PHY_FEC_RS_FEC;
                break;
            case phymod_fec_RS544:
                abilities[i].fec_type = PORTMOD_PORT_PHY_FEC_RS_544;
                break;
            case phymod_fec_RS544_2XN:
                abilities[i].fec_type = PORTMOD_PORT_PHY_FEC_RS_544_2XN;
                break;
            case phymod_fec_RS272:
                abilities[i].fec_type = PORTMOD_PORT_PHY_FEC_RS_272;
                break;
            case phymod_fec_RS272_2XN:
                abilities[i].fec_type = PORTMOD_PORT_PHY_FEC_RS_272_2XN;
                break;
            default:
                 return SOC_E_CONFIG;
        }
        switch (an_advert_abilities->autoneg_abilities[i].pause) {
            case phymod_pause_none:
                abilities[i].pause = PORTMOD_PORT_PHY_PAUSE_NONE;
                break;
            case phymod_pause_asym:
                abilities[i].pause = PORTMOD_PORT_PHY_PAUSE_TX;
                break;
            case phymod_pause_asym_symm:
                abilities[i].pause = PORTMOD_PORT_PHY_PAUSE_RX;
                break;
            case phymod_pause_symm:
                abilities[i].pause = PORTMOD_PORT_PHY_PAUSE_SYMM;
                break;
            default:
                return SOC_E_CONFIG;
        }
        switch (an_advert_abilities->autoneg_abilities[i].an_mode) {
            case phymod_AN_MODE_CL73:
                abilities[i].an_mode = PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73;
                break;
            case phymod_AN_MODE_CL73BAM:
                abilities[i].an_mode = PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM;
                break;
            case phymod_AN_MODE_MSA:
                abilities[i].an_mode = PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_MSA;
                break;
            default:
                return SOC_E_CONFIG;
        }
        if (an_advert_abilities->autoneg_abilities[i].medium == phymodFirmwareMediaTypeCopperCable) {
            abilities[i].medium = PORTMOD_PORT_PHY_MEDIUM_COPPER;
        } else {
            abilities[i].medium = PORTMOD_PORT_PHY_MEDIUM_BACKPLANE;
        }
        if (an_advert_abilities->autoneg_abilities[i].channel == phymod_channel_short) {
            abilities[i].channel = PORTMOD_PORT_PHY_CHANNEL_SHORT;
        } else {
            abilities[i].channel = PORTMOD_PORT_PHY_CHANNEL_LONG;
        }
    }

    return SOC_E_NONE;
}

static int
_pm8x100_gen2_an_encap_mode_get(int unit, int port, pm_info_t pm_info,
                                int *encap)
{
    portmod_encap_t tmp_encap;
    int phy_acc;

    SOC_INIT_FUNC_DEFS;

    /*
     * PM8x100 auto-negotiation(AN) only support two encap mode in AN:
     * PM_ENCAP_IEEE or PM_ENCAP_HG3.
     * Within each PM, HiGig3 and IEEE mode can not co-exist in AN.
     * If there's any AN port using HiGig3 encap mode, PM is in HiGig3 AN mode.
     * If there's any AN port using IEEE encap mode, PM is in IEEE AN mode.
     * If no AN port within the PML, user can choose AN encap mode freely.
     */
    /* Get the first physical port of the pm core */
    phy_acc = PM_8x100_GEN2_INFO(pm_info)->first_phy | SOC_REG_ADDR_PHY_ACC_MASK;
    _SOC_IF_ERR_EXIT(dc3mac_encap_get(unit, phy_acc, &tmp_encap));
    if (tmp_encap == SOC_ENCAP_HIGIG3) {
        *encap = SOC_ENCAP_HIGIG3;
        return SOC_E_NONE;
    }
    if (tmp_encap == SOC_ENCAP_IEEE) {
        *encap = SOC_ENCAP_IEEE;
        return SOC_E_NONE;
    }
    *encap = -1;

exit:
    SOC_FUNC_RETURN;
}

/*
 * Function:
 *   pm8x100_gen2_port_advert_abilities_validate
 * Purpose:
 *   Validate AN advertisement based on:
 *      1. PM support list.
 *      2. Protocol limitation.
 *      3. VCO limitation.
 *      4. FEC arch restriction.
 *      5. Encap support.
 *   If validation passes, return requested
 *   an_mode, FEC arch and VCO.
 */
int
pm8x100_gen2_port_advert_abilities_validate(int unit, int port, pm_info_t pm_info,
                                           int num_abilities,
                                           const portmod_port_speed_ability_t *abilities,
                                           phymod_an_mode_type_t *an_mode,
                                           portmod_fec_t *rs_fec_req)  /* portmod fec type or phymod fec type */
{
    int idx, is_dual_50g, port_num_lanes;
    int rs544_req = 0, rs272_req = 0;
    int is_bam = 0, is_msa = 0;
    uint8_t rs544_bm = 0, rs272_bm = 0, affected_bm = 0;
    int ieee_100G_single_lane_request = 0;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, i, port_index, mpp_index;
    uint32 bitmap;
    uint8_t tvco_pll_active_lane_bm, tvco_pll_adv_lane_bm;
    portmod_vco_type_t vco, current_vco;
    portmod_fec_t  fec_cl73bam_msa;
    int current_media_type = PORTMOD_PORT_PHY_MEDIUM_BACKPLANE;
    phymod_firmware_lane_config_t fw_lane_config;
    portmod_encap_t port_encap;
    int pm_encap_mode;
    phymod_an_mode_type_t  cur_an_mode;

    SOC_INIT_FUNC_DEFS;

    if (num_abilities == 0) {
        return SOC_E_PARAM;
    }

    fec_cl73bam_msa = PORTMOD_PORT_PHY_FEC_INVALID;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    PORTMOD_BIT_COUNT(phy_access.access.lane_mask, port_num_lanes);

    _SOC_IF_ERR_EXIT(
        phymod_phy_firmware_lane_config_get(&phy_access, &fw_lane_config));

    if (fw_lane_config.MediaType == phymodFirmwareMediaTypePcbTraceBackPlane) {
        current_media_type = PORTMOD_PORT_PHY_MEDIUM_BACKPLANE;
    } else if (fw_lane_config.MediaType == phymodFirmwareMediaTypeCopperCable) {
        current_media_type = PORTMOD_PORT_PHY_MEDIUM_COPPER;
    }

    /*
     * If the port is the only active port on the PM, and the port is not
     * running AN, there's no VCO restriction.
     * Otherwise, current VCO can not be changed.
     */
    _SOC_IF_ERR_EXIT(PM8x100_GEN2_TVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_active_lane_bm));
    _SOC_IF_ERR_EXIT(PM8x100_GEN2_TVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_adv_lane_bm));
    _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_index_get(unit, port, pm_info, &port_index, &bitmap));

    if ((tvco_pll_active_lane_bm == phy_access.access.lane_mask)
         && !(tvco_pll_adv_lane_bm & phy_access.access.lane_mask)) {
        current_vco = portmodVCOInvalid;
    } else {
        current_vco = PM_8x100_GEN2_INFO(pm_info)->tvco;
    }

    for (idx = 0; idx < num_abilities; idx++) {
        is_dual_50g = 0;

        /* Check parameters */
        if (abilities[idx].fec_type >= PORTMOD_PORT_PHY_FEC_COUNT) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                               (_SOC_MSG("port %d: Invalid FEC type %d."), port, abilities[idx].fec_type));
        } else if (abilities[idx].pause >= PORTMOD_PORT_PHY_PAUSE_COUNT) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                               (_SOC_MSG("port %d: Invalid pause %d."), port, abilities[idx].pause));
        } else if (abilities[idx].medium >= PORTMOD_PORT_PHY_MEDIUM_COUNT) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                               (_SOC_MSG("port %d: Invalid medium %d."), port, abilities[idx].medium));
        } else if (abilities[idx].channel >= PORTMOD_PORT_PHY_CHANNEL_COUNT) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                               (_SOC_MSG("port %d: Invalid channel %d."), port, abilities[idx].channel));
        } else if (abilities[idx].an_mode >= PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_COUNT) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                               (_SOC_MSG("port %d: Invalid AN mode %d."), port, abilities[idx].an_mode));
        }

        /* 1. AN ability is supported by HW. */
        /* 1.1 AN ability is in the port's support list. */
        _SOC_IF_ERR_EXIT
            (_pm8x100_gen2_an_ability_table_vco_get(abilities[idx].speed, abilities[idx].num_lanes,
                                                   abilities[idx].fec_type, abilities[idx].an_mode, &vco));

        /* 1.2 AN ability is supported by current VCO. */
        if (current_vco == portmodVCOInvalid) {
            current_vco = vco;
        } else if (current_vco != vco) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                (_SOC_MSG("Can not accommodate VCO settings.\n request VCO %d, current VCO %d\n"),
                 vco, current_vco));
        }

        /*
         * 1.3 Number of lane request in the abilies does not exceed the
         * number of physical lanes of the logical port.
         */
        if (abilities[idx].num_lanes > (uint32_t)port_num_lanes) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                (_SOC_MSG("port %d: Number of lane request in the abilies does not exceed the number of physical lanes of the port."), port));
        }

        if ((abilities[idx].fec_type == PORTMOD_PORT_PHY_FEC_RS_544) ||
            (abilities[idx].fec_type == PORTMOD_PORT_PHY_FEC_RS_544_2XN)) {
            rs544_req = 1;
        } else if ((abilities[idx].fec_type == PORTMOD_PORT_PHY_FEC_RS_272) ||
                   (abilities[idx].fec_type == PORTMOD_PORT_PHY_FEC_RS_272_2XN)) {
            rs272_req = 1;
        }

        /* 2. AN abilities do not conflict with each other. */
        /* 2.1 Pause, medium do not conflict among abilities. */
        if (abilities[idx].pause != abilities[0].pause) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                               (_SOC_MSG("port %d: Pause conflicts among abilities."), port));
        }
        if (abilities[idx].medium != current_media_type) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                               (_SOC_MSG("port %d: Medium conflicts with current media type."), port));
        }

        if ((abilities[idx].speed == 50000) &&
            (abilities[idx].num_lanes == 2) &&
            (abilities[idx].fec_type != PORTMOD_PORT_PHY_FEC_RS_544)) {
            /* Set flag indicating current ability is dual lane 50G. */
            is_dual_50g = 1;
        }

        /* both 100G 1 lane 544 and 544 2XN can not co-exists */
        if ((abilities[idx].speed == 100000) &&
            (abilities[idx].num_lanes == 1)) {
            if (ieee_100G_single_lane_request == 0) {
                ieee_100G_single_lane_request = 1;
            } else {
                return (SOC_E_FAIL);
            }
        }

        /* 2.2 Check FEC conflicts for each AN mode. */
        switch (abilities[idx].an_mode) {
            case PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM:
                is_bam = 1;

                /*
                 * Speeds 50G-2lane share the same RS528 FEC
                 * bit in BAM. Need to verify if there's any conflict among
                 * these abilities.
                 */
                if (is_dual_50g) {
                    if ((fec_cl73bam_msa == PORTMOD_PORT_PHY_FEC_NONE) &&
                        (abilities[idx].fec_type != PORTMOD_PORT_PHY_FEC_NONE)) {
                        /*
                         * Previous abilities request FEC_NONE, while current
                         * ability request other FEC type.
                         */
                        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                                           (_SOC_MSG("port %d: FEC conflicts among abilities."), port));
                    } else if ((abilities[idx].fec_type == PORTMOD_PORT_PHY_FEC_NONE) &&
                               (fec_cl73bam_msa == PORTMOD_PORT_PHY_FEC_RS_FEC)) {
                        /*
                         * Current ability request FEC_NONE, while previous
                         * abilities request other FEC type.
                         */
                        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                                           (_SOC_MSG("port %d: FEC conflicts among abilities."), port));
                    } else {
                        /* No FEC conflicts. Record current FEC request. */
                        fec_cl73bam_msa = abilities[idx].fec_type;
                    }
                }
                break;
            case PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_MSA:
                is_msa = 1;
                if (is_dual_50g) {
                    if ((fec_cl73bam_msa == PORTMOD_PORT_PHY_FEC_NONE) &&
                        (abilities[idx].fec_type != PORTMOD_PORT_PHY_FEC_NONE)) {
                        /*
                         * Previous abilities request FEC_NONE, while current
                         * ability request other FEC type.
                         */
                        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                                           (_SOC_MSG("port %d: FEC conflicts among abilities."), port));
                    } else if ((abilities[idx].fec_type == PORTMOD_PORT_PHY_FEC_NONE) &&
                               (fec_cl73bam_msa == PORTMOD_PORT_PHY_FEC_RS_FEC)) {
                        /*
                         * Current ability request FEC_NONE, while previous
                         * abilities request other FEC type.
                         */
                        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                                           (_SOC_MSG("port %d: FEC conflicts among abilities."), port));
                    } else {
                        /* No FEC conflicts. Record current FEC request. */
                        fec_cl73bam_msa = abilities[idx].fec_type;
                    }
                }
                break;
            case PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73:
                break;
            default:
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                                   (_SOC_MSG("port %d: Invalid AN mode %d."), port, abilities[idx].an_mode));
        }
    }

    /*
     * 2.3 Verify an_mode do not conflicts with each other.
     * CL73BAM and MSA can not be advertised at the same time.
     */
    if (is_bam && is_msa) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("port %d: an_mode conflicts among abilities."
                                     "Cannot advertise CL73MSA and CL73BAM at the same time."), port));
    } else if (is_bam) {
        *an_mode = phymod_AN_MODE_CL73BAM;
    } else if (is_msa) {
        *an_mode = phymod_AN_MODE_CL73_MSA;
    } else {
        *an_mode = phymod_AN_MODE_CL73;
    }

    /*
     * 2.4 Verify an_mode that CL73BAM and MSA does not co-exist
     * on the same MPP.
     */
    mpp_index = (int)(port_index / 4);
    for (i = (0 + (mpp_index * 4)); i < (4 + (mpp_index*4)); i++) {
        if (i == port_index) {
            continue;
        }
        _SOC_IF_ERR_EXIT(PM8x100_GEN2_AN_MODE_GET(unit, pm_info, &cur_an_mode, i));
        if ((*an_mode == phymod_AN_MODE_CL73_MSA) &&
            (cur_an_mode == phymod_AN_MODE_CL73BAM)) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                (_SOC_MSG("port %d: CL73_MSA cannot be advertised when current MPP is in CL73_BAM."), port));
        }
        if ((*an_mode == phymod_AN_MODE_CL73BAM) &&
            (cur_an_mode == phymod_AN_MODE_CL73_MSA)) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                (_SOC_MSG("port %d: CL73_BAM cannot be advertised when current MPP is in CL73_MSA."), port));
        }
    }

    /* 3. FEC arch can be supported within each MPP. */
    if (rs544_req || rs272_req) {
        _SOC_IF_ERR_EXIT(_pm8x100_gen2_fec_lanebitmap_get(unit, pm_info, &rs544_bm, &rs272_bm));
        rs544_bm &= ~phy_access.access.lane_mask;
        rs272_bm &= ~phy_access.access.lane_mask;
        if (rs544_req) {
            rs544_bm |= phy_access.access.lane_mask;
        }
        if (rs272_req) {
            rs272_bm |= phy_access.access.lane_mask;
        }
        _SOC_IF_ERR_EXIT(_pm8x100_gen2_fec_validate(unit, rs544_bm, rs272_bm, &affected_bm));
        if (rs544_req) {
            *rs_fec_req = PORTMOD_PORT_PHY_FEC_RS_544;
        } else if (rs272_req) {
            *rs_fec_req = PORTMOD_PORT_PHY_FEC_RS_272;
        }
    } else {
        *rs_fec_req = PORTMOD_PORT_PHY_FEC_NONE;
    }

    /*
     *  4. PM encap mode can support current port's request.
     *  If the given port has AN enabled with a different encap mode,
     *  This function will reject the encap mode change. User need to
     *  disable AN on the port before they switch the encap mode.
     */
    _SOC_IF_ERR_EXIT
        (_pm8x100_gen2_an_encap_mode_get(unit, port, pm_info,
                            &pm_encap_mode));
    _SOC_IF_ERR_EXIT
        (dc3mac_encap_get(unit, port, &port_encap));
    if ((pm_encap_mode != -1) && (pm_encap_mode != (int)port_encap)) {
        _SOC_IF_ERR_EXIT(SOC_E_FAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

/*Set/get port auto negotiation ability*/
int pm8x100_gen2_port_autoneg_ability_advert_set(int unit, int port, pm_info_t pm_info,
                                                 int num_abilities,
                                                 const portmod_port_speed_ability_t* abilities)
{
    phymod_an_mode_type_t an_mode = phymod_AN_MODE_NONE;
    phymod_autoneg_advert_ability_t
                                  autoneg_abilities[PM8X100_GEN2_MAX_AN_ABILITY];
    phymod_autoneg_advert_abilities_t an_advert_abilities;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    phymod_phy_pll_state_t old_pll_state, new_pll_state;
    int port_index, idx;
    portmod_fec_t rs_fec_req = PORTMOD_PORT_PHY_FEC_NONE;
    uint32 bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    sal_memset(autoneg_abilities, 0,
          PM8X100_GEN2_MAX_AN_ABILITY * sizeof(phymod_autoneg_advert_ability_t));

    /* 1. Validate abilities. */
    _SOC_IF_ERR_EXIT
        (pm8x100_gen2_port_advert_abilities_validate(unit, port,
                                                pm_info, num_abilities,
                                                abilities, &an_mode,
                                                &rs_fec_req));
    an_advert_abilities.autoneg_abilities = autoneg_abilities;
    _SOC_IF_ERR_EXIT
        (_pm8x100_gen2_port_port_to_phy_ability(num_abilities, abilities, &an_advert_abilities));

    /* 2. Set AN abilities in HW. */
    _SOC_IF_ERR_EXIT
        (phymod_phy_autoneg_advert_ability_set(&phy_access,
                                               &an_advert_abilities,
                                               &old_pll_state,
                                               &new_pll_state));

    /* 3. Update DB. */
    /* 3.1 Update an_mode. */
    _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(PM8x100_GEN2_AN_MODE_SET(unit, pm_info, an_mode, port_index));

    /* 3.2 Update FEC type. */
    for (idx = 0 ; idx < PM8X100_GEN2_LANES_PER_CORE; idx++) {
        if ((phy_access.access.lane_mask >> idx) & 0x1) {
            _SOC_IF_ERR_EXIT(PM8x100_GEN2_LANE2FEC_SET(unit, pm_info, idx, rs_fec_req));
        }
    }

exit:
    SOC_FUNC_RETURN;

}

int pm8x100_gen2_port_autoneg_ability_advert_get(int unit, int port, pm_info_t pm_info,
                                                 int max_num_abilities,
                                                 portmod_port_speed_ability_t* abilities,
                                                 int* actual_num_abilities)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, i;
    phymod_autoneg_advert_ability_t autoneg_abilities[PM8X100_GEN2_MAX_AN_ABILITY];
    phymod_autoneg_advert_abilities_t an_advert_abilities;

    SOC_INIT_FUNC_DEFS;

    for (i = 0; i < PM8X100_GEN2_MAX_AN_ABILITY; i++) {
        _SOC_IF_ERR_EXIT(phymod_autoneg_advert_ability_t_init(&autoneg_abilities[i]));
    }
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    an_advert_abilities.autoneg_abilities = autoneg_abilities;
    _SOC_IF_ERR_EXIT(phymod_phy_autoneg_advert_ability_get(&phy_access, &an_advert_abilities));

    if (an_advert_abilities.num_abilities > max_num_abilities) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("port %d: There are %d AN abilities. Larger array is needed."),
                           port, an_advert_abilities.num_abilities));
    } else {
        _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_phy_to_port_ability(&an_advert_abilities, actual_num_abilities, abilities));
    }

exit:
    SOC_FUNC_RETURN;
}

/*Port ability remote Adv get*/
int pm8x100_gen2_port_autoneg_ability_remote_get(int unit, int port, pm_info_t pm_info,
                                                 int max_num_abilities,
                                                 portmod_port_speed_ability_t* abilities,
                                                 int* actual_num_abilities)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, i;
    phymod_autoneg_advert_ability_t autoneg_abilities[PM8X100_GEN2_MAX_AN_ABILITY];
    phymod_autoneg_advert_abilities_t an_advert_abilities;

    SOC_INIT_FUNC_DEFS;

    for (i = 0; i < PM8X100_GEN2_MAX_AN_ABILITY; i++) {
        _SOC_IF_ERR_EXIT(phymod_autoneg_advert_ability_t_init(&autoneg_abilities[i]));
    }

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    an_advert_abilities.autoneg_abilities = autoneg_abilities;

    _SOC_IF_ERR_EXIT(phymod_phy_autoneg_remote_advert_ability_get(&phy_access, &an_advert_abilities));

    if (an_advert_abilities.num_abilities > max_num_abilities) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("port %d: There are %d AN abilities. Larger array is needed."),
                           port, an_advert_abilities.num_abilities));
    } else {
        _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_phy_to_port_ability(&an_advert_abilities, actual_num_abilities, abilities));
    }

exit:
    SOC_FUNC_RETURN;
}

/*Set\Get autoneg*/
int pm8x100_gen2_port_autoneg_set(int unit, int port, pm_info_t pm_info,
                                  uint32 phy_flags,
                                  const phymod_autoneg_control_t* an)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, port_num_lanes, speed_id_table_status = 0;

    phymod_autoneg_control_t an_control;
    portmod_port_speed_ability_t abilities[PM8X100_GEN2_MAX_AN_ABILITY];
    int port_index, flags = 0, actual_num_abilities, pm_id = 0;
    uint32_t idx, lane_mask_backup, bitmap;
    uint8_t rs544_req = 0, rs272_req = 0;
    portmod_vco_type_t req_vco = portmodVCOInvalid, current_vco = portmodVCOInvalid;
    portmod_encap_t encap;
    uint8_t pll_lanes_bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_pm_id_get(unit, port, &pm_id));
    _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    PORTMOD_BIT_COUNT(phy_access.access.lane_mask, port_num_lanes);
    lane_mask_backup = phy_access.access.lane_mask;
    _SOC_IF_ERR_EXIT(
         PM8x100_GEN2_TVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll_lanes_bitmap));
    an_control = *an;

    an_control.num_lane_adv = port_num_lanes;

    /* Get an_mode from WB */
    _SOC_IF_ERR_EXIT(PM8x100_GEN2_AN_MODE_GET(unit, pm_info, &an_control.an_mode, port_index));
    if (an_control.an_mode == phymod_AN_MODE_NONE) {
        /* Return error only when user want to enable AN without any AN abilities being advertised. */
        if (!an_control.enable) {
            an_control.an_mode = phymod_AN_MODE_CL73;
        } else {
            _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                               (_SOC_MSG("port %d: There's no valid AN advertisement for the port."), port));
        }
    }


    if (an_control.enable) {
        /* 1. Check if VCO change is needed. */
        /* 1.1 Get current VCO rate. */
        current_vco = PM_8x100_GEN2_INFO(pm_info)->tvco;
        _SOC_IF_ERR_EXIT
            (pm8x100_gen2_port_autoneg_ability_advert_get(unit, port, pm_info,
                                                 PM8X100_GEN2_MAX_AN_ABILITY,
                                                 abilities,
                                                 &actual_num_abilities));

        /* 1.2 Get the VCO requirement for AN advertisement. */
        if (actual_num_abilities == 0) {
            _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                               (_SOC_MSG("port %d: There's no valid ability for the port."), port));
        } else {
                /*
                 * All the AN advertisement will use the same VCO rate.
                 * So break once we get a valid request VCO value.
                 */
            _SOC_IF_ERR_EXIT
                (_pm8x100_gen2_an_ability_table_vco_get(abilities[0].speed,
                                                        abilities[0].num_lanes,
                                                        abilities[0].fec_type,
                                                        abilities[0].an_mode, &req_vco));
        }

        /* 1.3 Change VCO if needed. */
        if (current_vco != req_vco) {
            if (pll_lanes_bitmap == lane_mask_backup) {
                /* The current port is the only active port on the PM. */
                _SOC_IF_ERR_EXIT
                    (pm8x100_gen2_pm_vco_reconfig(unit, pm_id,
                                                 pm_info, &req_vco));
            } else {
                _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                               (_SOC_MSG("vco rate %d is not available.\n"), req_vco));
            }
        }

        /* 2. Reload SPEED ID table if needed. */
        /*
         * Encap validation is done in pm8x100_gen2_port_ability_advert_set.
         * Here just need to make sure the SPEED ID table loaded can serve
         * the given port.
         */
        _SOC_IF_ERR_EXIT
            (pm8x100_gen2_port_encap_get(unit, port, pm_info, &flags, &encap));
        if (encap == SOC_ENCAP_HIGIG3) {

            _SOC_IF_ERR_EXIT
                (PM8x100_GEN2_SPEED_ID_TABLE_STATUS_GET(unit, pm_info, &speed_id_table_status));
            /* The port's encap mode is HG3. */
            if (speed_id_table_status !=
                PM8x100_GEN2_SPEED_ID_TABLE_STATUS_HG3_MODE_LOADED) {
                /*
                 * Reload SPEED ID table for HG3 mode.
                 * Update table loading status in HA info.
                 */
                _SOC_IF_ERR_EXIT
                    (phymod_phy_autoneg_speed_id_table_reload(&phy_access, 1));
                speed_id_table_status = PM8x100_GEN2_SPEED_ID_TABLE_STATUS_HG3_MODE_LOADED;
                _SOC_IF_ERR_EXIT
                    (PM8x100_GEN2_SPEED_ID_TABLE_STATUS_SET(unit, pm_info, speed_id_table_status));
            }
        } else {
            /* The port's encap mode is IEEE. */
            if (speed_id_table_status !=
                PM8x100_GEN2_SPEED_ID_TABLE_STATUS_NOT_LOADED) {
                /*
                 * Reload SPEED ID table for IEEE mode.
                 * Update table loading status in HA info.
                 */
                _SOC_IF_ERR_EXIT
                    (phymod_phy_autoneg_speed_id_table_reload(&phy_access, 0));
                speed_id_table_status = PM8x100_GEN2_SPEED_ID_TABLE_STATUS_IEEE_MODE_LOADED;
                _SOC_IF_ERR_EXIT
                    (PM8x100_GEN2_SPEED_ID_TABLE_STATUS_SET(unit, pm_info, speed_id_table_status));
            }
        }

        /* 3. Set FEC request flag for Phymod. */
        for (idx = 0; idx < actual_num_abilities; idx++) {
            /* Check FEC requests. */
            if ((abilities[idx].fec_type == PORTMOD_PORT_PHY_FEC_RS_544) ||
                (abilities[idx].fec_type == PORTMOD_PORT_PHY_FEC_RS_544_2XN)) {
                rs544_req = 1;
            } else if ((abilities[idx].fec_type == PORTMOD_PORT_PHY_FEC_RS_272) ||
                       (abilities[idx].fec_type == PORTMOD_PORT_PHY_FEC_RS_272_2XN)) {
                rs272_req = 1;
            }
        }

        if (rs544_req) {
            PHYMOD_AN_F_FEC_RS272_CLR_SET(&an_control);
        } else if (rs272_req) {
            PHYMOD_AN_F_FEC_RS272_REQ_SET(&an_control);
        }

        /* 4. Update port mode in MAC. */
        phy_access.access.lane_mask = 1 << port_index;
        _SOC_IF_ERR_EXIT
            (dc3port_port_mode_set(unit, port, flags, phy_access.access.lane_mask));
    }

    phy_access.access.lane_mask = lane_mask_backup;
    /* Enable AN in Serdes. */
    _SOC_IF_ERR_EXIT
        (phymod_phy_autoneg_set(&phy_access, &an_control));

    _SOC_IF_ERR_EXIT(
        PM8x100_GEN2_TVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll_lanes_bitmap));
    if (an_control.enable) {
        pll_lanes_bitmap |= phy_access.access.lane_mask;
    } else {
        pll_lanes_bitmap &= ~phy_access.access.lane_mask;
    }
    _SOC_IF_ERR_EXIT(
        PM8x100_GEN2_TVCO_PLL_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll_lanes_bitmap));

exit:
    SOC_FUNC_RETURN;

}

int pm8x100_gen2_port_autoneg_get(int unit, int port, pm_info_t pm_info,
                                  uint32 phy_flags,
                                  phymod_autoneg_control_t* an)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    uint32_t an_done;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_autoneg_get(&phy_access, an, &an_done));

exit:
    SOC_FUNC_RETURN;
}

/*Get autoneg status*/
int pm8x100_gen2_port_autoneg_status_get(int unit, int port, pm_info_t pm_info,
                                         phymod_autoneg_status_t* an_status)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(phymod_phy_autoneg_status_get(&phy_access, an_status));
    if (!(an_status->enabled && an_status->locked)) {
        /* upper layer should not rely on the
         * data rate if the AN is not locked
         */
        an_status->data_rate = 0;
        an_status->interface = phymodInterfaceBypass;
    }


exit:
    SOC_FUNC_RETURN;
}

/* link up event */
/* 1. AN ports: Update port mode based on reslved speed mode.
 *              Program resolved Pause settings.
 * 2. 1588 ports: timesync adjust.
 * 3. register callback function if BER WAR is applicable
 */
int pm8x100_gen2_port_phy_link_up_event(int unit, int port, pm_info_t pm_info)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access, phy_copy;
    int nof_phys, port_index = -1;
    uint32 timesync_config, bitmap;
    int rx_pause, tx_pause, flags = 0;
    uint32_t idx;
    phymod_autoneg_status_t an_status;
    int num_advert, num_remote;
    portmod_port_speed_ability_t advert_ability[PM8X100_GEN2_MAX_AN_ABILITY];
    portmod_port_speed_ability_t remote_ability[PM8X100_GEN2_MAX_AN_ABILITY];
    portmod_port_phy_pause_t pause_local, pause_remote;
    portmod_pause_control_t pause_ctrl;
    portmod_encap_t port_encap;
    phymod_timesync_adjust_config_info_t timesync_adjust_config;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT
        (pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT
        (_pm8x100_gen2_port_index_get(unit, port, pm_info, &port_index, &bitmap));

    /* 1. 1588 ports: Timesync adjust. */
    _SOC_IF_ERR_EXIT
        (PM8x100_GEN2_TIMESYNC_CONFIG_GET(unit, pm_info, &timesync_config, port_index));
    if (PORTMOD_PORT_TIMESYNC_CONFIG_ENABLE_GET(timesync_config)) {
        /* Set flags to disable TS_UPDATE_EN. */
        flags = 0;
        PHYMOD_TIMESYNC_ENABLE_F_RX_SET(flags);
        _SOC_IF_ERR_EXIT
            (phymod_timesync_enable_set(&phy_access, flags, 0));

        _SOC_IF_ERR_EXIT
                (phymod_timesync_adjust_config_info_t_init(&timesync_adjust_config));

        /*
         * If 1588 is enabled on the port, need to adjust timesync during
         * link up event.
         * rx_ts_update will be enabled at the end of adjust function.
         */
        if (PORTMOD_PORT_TIMESYNC_CONFIG_COMP_MODE_LATEST_LANE_GET(timesync_config)){
            timesync_adjust_config.am_norm_mode = phymodTimesyncCompensationModeLatestlane;
        } else {
            timesync_adjust_config.am_norm_mode = phymodTimesyncCompensationModeEarliestLane;
        }

        

        flags = 0;
        PHYMOD_TIMESYNC_ENABLE_F_RX_SET(flags);
        /* Set flags for timestamp adjust. */
        if (PORTMOD_PORT_TIMESYNC_CONFIG_SFD_GET(timesync_config)) {
            PHYMOD_TIMESYNC_F_SOP_SET(flags);
        }
        if (PORTMOD_PORT_TIMESYNC_CONFIG_MAC_DA_GET(timesync_config)) {
            PHYMOD_TIMESYNC_F_MAC_DA_SET(flags);
        }

        _SOC_IF_ERR_EXIT
            (pm8x100_gen2_port_encap_get(unit, port, pm_info, &flags, &port_encap));

        /*
         * Both SOC_ENCAP_HIGIG3 and BCMPMAC_ENCAP_IEEE_REDUCED_IPG
         * set the same mode in DC3MAC_MODRr.HDR_MODE.
         * Checking ENCAP_HG3 type here, which means REDUCED_PREAMBLE mode is set.
         */
        if (port_encap == SOC_ENCAP_HIGIG3) {
            PHYMOD_TIMESYNC_F_REDUCED_PREAMBLE_MODE_SET(flags);
        }

        _SOC_IF_ERR_EXIT
            (phymod_timesync_adjust_set(&phy_access, flags, &timesync_adjust_config));

        /* Set flags to enable TS_UPDATE_EN. */
        flags = 0;
        PHYMOD_TIMESYNC_ENABLE_F_RX_SET(flags);
        _SOC_IF_ERR_EXIT
            (phymod_timesync_enable_set(&phy_access, flags, 1));
        /* Wait for one AM spacing time. */
        sal_usleep(500);
    }

    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_autoneg_status_get(unit, port, pm_info, &an_status));
    if ((an_status.enabled && an_status.locked)) {
        /* 2.1 AN ports: Port mode update. */
        sal_memcpy(&phy_copy, &phy_access, sizeof(phy_copy));
        phy_copy.access.lane_mask = 0;
        /* Update lane mask based on resovled number of lanes. */
        for (idx = 0; idx < an_status.resolved_num_lane; idx++) {
            phy_copy.access.lane_mask |= 0x1 << (port_index + idx);
        }
        _SOC_IF_ERR_EXIT
            (dc3port_port_mode_set(unit, port, flags, phy_copy.access.lane_mask));

        /* 2.2 AN ports: Pause update. */
        _SOC_IF_ERR_EXIT
            (pm8x100_gen2_port_autoneg_ability_advert_get(unit, port, pm_info,
                                                 PM8X100_GEN2_MAX_AN_ABILITY,
                                                 advert_ability, &num_advert));
        _SOC_IF_ERR_EXIT
            (pm8x100_gen2_port_autoneg_ability_remote_get(unit, port, pm_info,
                                                 PM8X100_GEN2_MAX_AN_ABILITY,
                                                 remote_ability, &num_remote));
        if (num_advert && num_remote) {
            pause_local = advert_ability[0].pause;
            pause_remote = remote_ability[0].pause;
            if (
                ((pause_local == PORTMOD_PORT_PHY_PAUSE_RX) &&
                 (pause_remote == PORTMOD_PORT_PHY_PAUSE_TX)) ||
                ((pause_local == PORTMOD_PORT_PHY_PAUSE_RX ||
                  pause_local == PORTMOD_PORT_PHY_PAUSE_SYMM) &&
                 (pause_remote == PORTMOD_PORT_PHY_PAUSE_RX ||
                  pause_remote == PORTMOD_PORT_PHY_PAUSE_SYMM))
               ) {
                rx_pause = 1;
            } else {
                rx_pause = 0;
            }
            if (
                ((pause_local == PORTMOD_PORT_PHY_PAUSE_RX ||
                  pause_local == PORTMOD_PORT_PHY_PAUSE_SYMM) &&
                 (pause_remote == PORTMOD_PORT_PHY_PAUSE_RX ||
                  pause_remote == PORTMOD_PORT_PHY_PAUSE_SYMM)) ||
                ((pause_local == PORTMOD_PORT_PHY_PAUSE_TX) &&
                 (pause_remote == PORTMOD_PORT_PHY_PAUSE_RX))
               ) {
                tx_pause = 1;
            } else {
                tx_pause = 0;
            }
        } else {
            tx_pause = 0;
            rx_pause = 0;
        }

        _SOC_IF_ERR_EXIT
            (pm8x100_gen2_port_pause_control_get(unit, port, pm_info,
                                                 &pause_ctrl));
        if ((pause_ctrl.rx_enable != rx_pause) ||
            (pause_ctrl.tx_enable != tx_pause)) {
            pause_ctrl.rx_enable = rx_pause;
            pause_ctrl.tx_enable = tx_pause;
            _SOC_IF_ERR_EXIT
                (pm8x100_gen2_port_pause_control_set(unit, port, pm_info,
                                                     &pause_ctrl));
        }
    }

    /* Need to do the MAC link down recovery sequence. */
    _SOC_IF_ERR_EXIT
        (dc3mac_rx_enable_set(unit, port, 0));

    _SOC_IF_ERR_EXIT
        (dc3mac_discard_set(unit, port, 1));

    sal_usleep(10000);

    _SOC_IF_ERR_EXIT
        (dc3mac_rx_enable_set(unit, port, 1));

    _SOC_IF_ERR_EXIT
        (dc3mac_discard_set(unit, port, 0));

exit:
    SOC_FUNC_RETURN;
}

/*PRBS configuration set/get*/
int pm8x100_gen2_port_prbs_config_set(int unit, int port, pm_info_t pm_info,
                                      portmod_prbs_mode_t mode, int flags,
                                      const phymod_prbs_t* config)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    /* MAC */
    if(mode == 1) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("MAC PRBS is not supported")));
    }

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_prbs_config_set(&phy_access, flags, config));

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_prbs_config_get(int unit, int port, pm_info_t pm_info,
                                      portmod_prbs_mode_t mode, int flags,
                                      phymod_prbs_t* config)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    /* MAC */
    if(mode == 1) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("MAC PRBS is not supported")));
    }

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_prbs_config_get(&phy_access, flags, config));

exit:
    SOC_FUNC_RETURN;
}

/*PRBS enable set/get*/
int pm8x100_gen2_port_prbs_enable_set(int unit, int port, pm_info_t pm_info,
                                      portmod_prbs_mode_t mode, int flags,
                                      int enable)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    /* MAC */
    if(mode == 1) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("MAC PRBS is not supported")));
    }

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_prbs_enable_set(&phy_access, flags, enable));

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_prbs_enable_get(int unit, int port, pm_info_t pm_info,
                                      portmod_prbs_mode_t mode, int flags,
                                      int* enable)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    /* MAC */
    if(mode == 1) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("MAC PRBS is not supported")));
    }

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_prbs_enable_get(&phy_access, flags,
                                                (uint32_t *) enable));

exit:
    SOC_FUNC_RETURN;
}

/*PRBS status get*/
int pm8x100_gen2_port_prbs_status_get(int unit, int port, pm_info_t pm_info,
                                      portmod_prbs_mode_t mode, int flags,
                                      phymod_prbs_status_t* status)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    /* MAC */
    if(mode == 1) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("MAC PRBS is not supported")));
    }

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_prbs_status_get(&phy_access, flags, status));

exit:
    SOC_FUNC_RETURN;
}

/*Port tx taps set\get*/
int pm8x100_gen2_port_tx_set(int unit, int port, pm_info_t pm_info, const phymod_tx_t* tx)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_tx_set(&phy_access, tx));

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_tx_get(int unit, int port, pm_info_t pm_info, phymod_tx_t* tx)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_tx_get(&phy_access, tx));

exit:
    SOC_FUNC_RETURN;
}

/*Number of lanes get*/
int pm8x100_gen2_port_nof_lanes_get(int unit, int port, pm_info_t pm_info,
                                    int* nof_lanes)
{
    int port_index;
    uint32_t bitmap, bcnt = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_index_get(unit, port, pm_info, &port_index, &bitmap));

    while (bitmap) {
        if (bitmap & 0x1) bcnt++;
        bitmap >>= 1;
    }
    *nof_lanes = bcnt;

exit:
    SOC_FUNC_RETURN;
}

/*Filter packets smaller than the specified threshold*/
int pm8x100_gen2_port_runt_threshold_set(int unit, int port,
                                         pm_info_t pm_info,
                                         int value)
{
    return (SOC_E_UNAVAIL);
}

int pm8x100_gen2_port_runt_threshold_get(int unit, int port, pm_info_t pm_info,
                                         int* value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(dc3mac_runt_threshold_get(unit, port, (uint32_t *)value));
exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_tx_threshold_set(int unit, int port,
                                       pm_info_t pm_info,
                                       int value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(dc3mac_tx_threshold_set(unit, port, value));
exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_tx_threshold_get(int unit, int port, pm_info_t pm_info,
                                       int* value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(dc3mac_tx_threshold_get(unit, port, (uint32_t *)value));
exit:
    SOC_FUNC_RETURN;
}

/*Filter packets bigger than the specified value*/
int pm8x100_gen2_port_max_packet_size_set(int unit, int port, pm_info_t pm_info,
                                          int value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(dc3mac_frame_max_set(unit, port , value));
exit:
    SOC_FUNC_RETURN;
}
int pm8x100_gen2_port_max_packet_size_get(int unit, int port, pm_info_t pm_info,
                                          int* value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(dc3mac_frame_max_get(unit, port , (uint32_t *)value));
exit:
    SOC_FUNC_RETURN;
}

/*set/get the MAC source address that will be sent in case of Pause/LLFC*/
int pm8x100_gen2_port_tx_mac_sa_set(int unit, int port, pm_info_t pm_info,
                                    sal_mac_addr_t mac_sa)
{
   SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(dc3mac_pause_addr_set(unit, port, mac_sa));

exit:
    SOC_FUNC_RETURN;
}
int pm8x100_gen2_port_tx_mac_sa_get(int unit, int port, pm_info_t pm_info,
                                    sal_mac_addr_t mac_sa)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(dc3mac_pause_addr_get(unit, port, mac_sa));

exit:
    SOC_FUNC_RETURN;
}

/* set/get SA recognized for MAC control packets
 * in addition to the standard 0x0180C2000001
 */
int pm8x100_gen2_port_rx_mac_sa_set(int unit, int port, pm_info_t pm_info,
                                    sal_mac_addr_t mac_sa)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(dc3mac_pause_addr_set(unit, port, mac_sa));

exit:
    SOC_FUNC_RETURN;
}
int pm8x100_gen2_port_rx_mac_sa_get(int unit, int port, pm_info_t pm_info,
                                    sal_mac_addr_t mac_sa)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(dc3mac_pause_addr_get(unit, port, mac_sa));

exit:
    SOC_FUNC_RETURN;
}

/*set/get Average inter-packet gap*/
int pm8x100_gen2_port_tx_average_ipg_set(int unit, int port, pm_info_t pm_info,
                                         int value)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(dc3mac_avg_ipg_set(unit, port, value));

exit:
    SOC_FUNC_RETURN;


}
int pm8x100_gen2_port_tx_average_ipg_get(int unit, int port, pm_info_t pm_info,
                                         int* value)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(dc3mac_avg_ipg_get(unit, port, (uint8_t *)value));

exit:
    SOC_FUNC_RETURN;

}

/*local fault set/get*/
int pm8x100_gen2_port_local_fault_control_set(int unit, int port, pm_info_t pm_info,
                                              const portmod_local_fault_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(dc3mac_local_fault_disable_set(unit, port, control));

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_local_fault_control_get(int unit, int port, pm_info_t pm_info,
                                              portmod_local_fault_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(dc3mac_local_fault_disable_get(unit, port, control));

exit:
    SOC_FUNC_RETURN;

}

int pm8x100_gen2_port_local_fault_force_set(int unit, int port, pm_info_t pm_info, int enable)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(dc3mac_link_fault_os_set(unit, port, 0, enable));

exit:
    SOC_FUNC_RETURN;
}
int pm8x100_gen2_port_local_fault_force_get(int unit, int port, pm_info_t pm_info, int * enable)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(dc3mac_link_fault_os_get(unit, port, 0, (uint32_t *)enable));

exit:
    SOC_FUNC_RETURN;
}

/*remote fault set/get*/
int pm8x100_gen2_port_remote_fault_control_set(int unit, int port, pm_info_t pm_info,
                                               const portmod_remote_fault_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(dc3mac_remote_fault_disable_set(unit, port, control));

exit:
    SOC_FUNC_RETURN;

}
int pm8x100_gen2_port_remote_fault_control_get(int unit, int port, pm_info_t pm_info,
                                               portmod_remote_fault_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(dc3mac_remote_fault_disable_get(unit, port, control));

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_remote_fault_force_set(int unit, int port, pm_info_t pm_info, int enable)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(dc3mac_link_fault_os_set(unit, port, 1, enable));

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_remote_fault_force_get(int unit, int port, pm_info_t pm_info, int * enable)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(dc3mac_link_fault_os_get(unit, port, 1, (uint32_t *)enable));

exit:
    SOC_FUNC_RETURN;
}

/*local fault steatus get*/
int pm8x100_gen2_port_local_fault_status_get(int unit, int port, pm_info_t pm_info,
                                             int* value)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(dc3mac_local_fault_status_get(unit, port, value));

exit:
    SOC_FUNC_RETURN;
}

/*remote fault status get*/
int pm8x100_gen2_port_remote_fault_status_get(int unit, int port, pm_info_t pm_info,
                                              int* value)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(dc3mac_remote_fault_status_get(unit, port, value));

exit:
    SOC_FUNC_RETURN;
}

/*local fault steatus clear, there are no clear register for DC3MAC, just clear by read*/
int pm8x100_gen2_port_local_fault_status_clear(int unit, int port, pm_info_t pm_info)
{
    int value;
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(dc3mac_local_fault_status_get(unit, port, &value));

exit:
    SOC_FUNC_RETURN;
}

/*remote fault status clear, there are no clear register for DC3MAC, just clear by read*/
int pm8x100_gen2_port_remote_fault_status_clear(int unit, int port,
                                                pm_info_t pm_info)
{
    int value;
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(dc3mac_remote_fault_status_get(unit, port, &value));

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_pause_control_set(int unit, int port, pm_info_t pm_info,
                                        const portmod_pause_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(dc3mac_pause_set(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_pause_control_get(int unit, int port, pm_info_t pm_info,
                                        portmod_pause_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(dc3mac_pause_get(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_pfc_control_set(int unit, int port, pm_info_t pm_info,
                                      const portmod_pfc_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(dc3mac_pfc_control_set(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_pfc_control_get(int unit, int port, pm_info_t pm_info,
                                portmod_pfc_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(dc3mac_pfc_control_get(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

/*get port cores' phymod access*/
int pm8x100_gen2_port_core_access_get(int unit, int port, pm_info_t pm_info,
                                      int phyn, int max_cores,
                                      phymod_core_access_t* core_access_arr,
                                      int* nof_cores, int* is_most_ext)
{
    SOC_INIT_FUNC_DEFS;

    /* There are only internal phys(1) on PM8x100_GEN2, setting phyn to 0 */
    if(phyn < 0)
    {
        phyn = 0;
    }

    _SOC_IF_ERR_EXIT(phymod_core_access_t_init(&core_access_arr[0]));

    if( phyn == 0 ){
        sal_memcpy(&core_access_arr[0],
                   &(PM_8x100_GEN2_INFO(pm_info)->int_core_access),
                   sizeof(phymod_core_access_t));
        *nof_cores = 1;
    }

    if (is_most_ext) {
            *is_most_ext = 1;
    }

exit:
    SOC_FUNC_RETURN;
}

/*Get lane phymod access structure. can be used for per lane operations*/
int pm8x100_gen2_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info,
                                          const portmod_access_get_params_t* params,
                                          int max_phys,
                                          phymod_phy_access_t* phy_access,
                                          int* nof_phys, int* is_most_ext)
{
    int rv;
    uint32  iphy_lane_mask = 0;
    int port_index=0;
    int i;
    uint32 is_bypassed = 0;
    int lane_count;

    SOC_INIT_FUNC_DEFS;

    if(max_phys > MAX_NUM_CORES)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("max_phys parameter exceeded the "
                          "MAX value. max_phys=%d, max allowed %d."),
                          max_phys, MAX_NUM_CORES));
    }

    /* get port index and lane mask */
    _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_index_get(unit, port, pm_info, &port_index,
                                                  &iphy_lane_mask));

    rv = PM8x100_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index);
    _SOC_IF_ERR_EXIT(rv);

    for( i = 0 ; i < max_phys; i++) {
        _SOC_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access[i]));
    }

    *nof_phys = 1;
    /* internal core */
    sal_memcpy (&phy_access[0], &(PM_8x100_GEN2_INFO(pm_info)->int_core_access),
                sizeof(phymod_phy_access_t));
    phy_access[0].access.lane_mask = iphy_lane_mask;
    if (params->lane != -1) {
        lane_count = 0;
        phy_access[0].access.lane_mask = 0;
        for (i = 0; i < MAX_PORTS_PER_PM8X100_GEN2; ++i)
        {
            if (iphy_lane_mask & (1 << i))
            {
                if (lane_count == params->lane)
                {
                    phy_access[0].access.lane_mask = (1 << i);
                    break;
                }

                ++lane_count;
            }
        }
        if (phy_access[0].access.lane_mask == 0) {
            *nof_phys = 0;
        }
    }
    /* only line is availabe for internal. */
    phy_access[0].port_loc = phymodPortLocLine;

    if (is_bypassed) {
        PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_SET(phy_access[0].device_op_mode);
    }

    if (is_most_ext) {
        *is_most_ext = 1;
    }

exit:
    SOC_FUNC_RETURN;
}
/*
 * Get lane phymod access structure via pm id.
 * This function can be used for getting phy access
 * when port is not attached.
 *
 * If a single lane access is required, please specify the params.lane.
 * Otherwise, set params.lane = -1, meaning get lane access for all the
 * lanes in the given PM.
 */
int pm8x100_gen2_pm_phy_lane_access_get(int unit, int pm_id, pm_info_t pm_info,
                                        const portmod_access_get_params_t* params,
                                        int max_phys, phymod_phy_access_t* phy_access,
                                        int* nof_phys, int* is_most_ext)
{
    uint32  iphy_lane_mask = 0;
    int port_index=0;
    int i;
    int port = -1;
    uint32 is_bypassed = 0;

    SOC_INIT_FUNC_DEFS;

    for( i = 0 ; i < max_phys; i++) {
        _SOC_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access[i]));
    }

    *nof_phys = 1;

    /* internal core */
    sal_memcpy (&phy_access[0], &(PM_8x100_GEN2_INFO(pm_info)->int_core_access),
                sizeof(phymod_phy_access_t));

    if (params->lane != -1) {
        /*
         * If lane index is not -1, need to check if there is any port attched on
         * the given lane. If there is active port on the lane, need to check if
         * the lane is working at bypass mode.
         */
        _SOC_IF_ERR_EXIT(PM8x100_GEN2_LANE2PORT_GET(unit, pm_info, params->lane, &port));
        if (port > 0) {
            _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_index_get(unit, port, pm_info, &port_index,
                                                    &iphy_lane_mask));
            _SOC_IF_ERR_EXIT(PM8x100_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));
            if (is_bypassed) {
                PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_SET(phy_access[0].device_op_mode);
            }
        }
        phy_access[0].access.lane_mask = (0x1 << (params->lane));
    } else {
        /*
         * If lane index is -1, assume all the lanes are accessed
         */
        phy_access[0].access.lane_mask = (0x1 << MAX_PORTS_PER_PM8X100_GEN2) - 1;
    }
    /* only line is availabe for internal. */
    phy_access[0].port_loc = phymodPortLocLine;

    if (is_most_ext) {
        *is_most_ext = 1;
    }
exit:
    SOC_FUNC_RETURN;
}

/*Port PHY Control register read*/
int pm8x100_gen2_port_phy_reg_read(int unit, int port, pm_info_t pm_info, int lane,
                                   int flags, int reg_addr, uint32* value)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    uint32 lane_mask;
    int start_lane;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    /* get start lane and lane mask */
    _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_index_get(unit, port, pm_info, &start_lane, &lane_mask));

    PM8X100_GEN2_PHY_REG_SET(reg_addr);

    if (lane >= 0) {
        reg_addr |= PHYMOD_REG_ACC_AER_IBLK_FORCE_LANE;
        reg_addr &= ~(0x7 << PHYMOD_REG_ACCESS_FLAGS_SHIFT);
        reg_addr |= (((start_lane + lane) & 0x7) << PHYMOD_REG_ACCESS_FLAGS_SHIFT);
    }

    _SOC_IF_ERR_EXIT(phymod_phy_reg_read(&phy_access, reg_addr, value));

exit:
    SOC_FUNC_RETURN;
}

/*Port PHY Control register write*/
int pm8x100_gen2_port_phy_reg_write(int unit, int port, pm_info_t pm_info,
                                    int lane, int flags, int reg_addr, uint32 value)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    uint32 lane_mask;
    int start_lane;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    /* get start lane and lane mask */
    _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_index_get(unit, port, pm_info, &start_lane, &lane_mask));

    PM8X100_GEN2_PHY_REG_SET(reg_addr);

    if (lane >= 0) {
        reg_addr |= PHYMOD_REG_ACC_AER_IBLK_FORCE_LANE;
        reg_addr &= ~(0x7 << PHYMOD_REG_ACCESS_FLAGS_SHIFT);
        reg_addr |= (((start_lane + lane) & 0x7) << PHYMOD_REG_ACCESS_FLAGS_SHIFT);
    }

    _SOC_IF_ERR_EXIT(phymod_phy_reg_write(&phy_access, reg_addr, value));

exit:
    SOC_FUNC_RETURN;
}

/*Port Reset set\get*/
int pm8x100_gen2_port_reset_set(int unit, int port,
                                pm_info_t pm_info, int mode,
                                int opcode, int value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_phy_port_reset_set(unit, port, pm_info,
                                                mode, opcode, value));
exit:
    SOC_FUNC_RETURN
}
int pm8x100_gen2_port_reset_get(int unit, int port,
                                pm_info_t pm_info, int mode,
                                int opcode, int* value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_phy_port_reset_get(unit, port, pm_info,
                                                mode, opcode, value));
exit:
    SOC_FUNC_RETURN
}

/*Drv Name Get*/
int pm8x100_gen2_port_drv_name_get(int unit, int port,
                             pm_info_t pm_info,
                             char* name, int len)
{
    strncpy(name, "PM8X100_GEN2 Driver", len);
    return (SOC_E_NONE);
}

/*set/get port fec enable according to local/remote FEC ability*/
int pm8x100_gen2_port_fec_enable_set(int unit, int port, pm_info_t pm_info,
                                     uint32 phy_flags, uint32_t enable)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    if (phy_flags != PORTMOD_INIT_F_INTERNAL_SERDES_ONLY) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("Only internal phys")));
    }
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_fec_enable_set(&phy_access, enable));

exit:
    SOC_FUNC_RETURN;
}
int pm8x100_gen2_port_fec_enable_get(int unit, int port, pm_info_t pm_info,
                                     uint32 phy_flags, uint32_t* enable)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    if (phy_flags != PORTMOD_INIT_F_INTERNAL_SERDES_ONLY) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("Only internal phys")));
    }
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_fec_enable_get(&phy_access, enable));

exit:
    SOC_FUNC_RETURN;
}


/*set/get pass control frames.*/
int pm8x100_gen2_port_rx_control_set(int unit, int port, pm_info_t pm_info,
                                     const portmod_rx_control_t* rx_ctrl)
{
    SOC_INIT_FUNC_DEFS;

    if (rx_ctrl->flags & PORTMOD_MAC_PASS_CONTROL_FRAME) {
        _SOC_IF_ERR_EXIT(dc3mac_rsv_selective_mask_set(unit, port,
                                                    DC3MAC_RSV_MASK_CONTROL_FRAME,
                                                    rx_ctrl->pass_control_frames));
        _SOC_IF_ERR_EXIT(dc3mac_pass_control_frame_set(unit, port, rx_ctrl->pass_control_frames));
    }

    if (rx_ctrl->flags & PORTMOD_MAC_PASS_PFC_FRAME) {
        _SOC_IF_ERR_EXIT(dc3mac_rsv_selective_mask_set(unit, port,
                                                    DC3MAC_RSV_MASK_PFC_FRAME,
                                                    rx_ctrl->pass_pfc_frames));
        _SOC_IF_ERR_EXIT(dc3mac_pass_pfc_frame_set(unit, port, rx_ctrl->pass_pfc_frames));
    }

    if (rx_ctrl->flags & PORTMOD_MAC_PASS_PAUSE_FRAME) {
        _SOC_IF_ERR_EXIT(dc3mac_rsv_selective_mask_set(unit, port,
                                                    DC3MAC_RSV_MASK_PAUSE_FRAME,
                                                    rx_ctrl->pass_pause_frames));
        _SOC_IF_ERR_EXIT(dc3mac_pass_pause_frame_set(unit, port, rx_ctrl->pass_pause_frames));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_rx_control_get(int unit, int port, pm_info_t pm_info,
                                     portmod_rx_control_t* rx_ctrl)
{
    uint32 rx_pass_control_frames = 0, rx_pass_pfc_frames = 0, rx_pass_pause_frames = 0;
    uint32 rsv_pass_control_frames = 0, rsv_pass_pfc_frames = 0, rsv_pass_pause_frames = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(dc3mac_pass_control_frame_get(unit, port, &rx_pass_control_frames));
    _SOC_IF_ERR_EXIT(dc3mac_pass_pfc_frame_get(unit, port, &rx_pass_pfc_frames));
    _SOC_IF_ERR_EXIT(dc3mac_pass_pause_frame_get(unit, port, &rx_pass_pause_frames));
    _SOC_IF_ERR_EXIT(dc3mac_rsv_selective_mask_get(unit, port, DC3MAC_RSV_MASK_CONTROL_FRAME,
                                                   &rsv_pass_control_frames));
    _SOC_IF_ERR_EXIT(dc3mac_rsv_selective_mask_get(unit, port, DC3MAC_RSV_MASK_PFC_FRAME,
                                                   &rsv_pass_pfc_frames));
    _SOC_IF_ERR_EXIT(dc3mac_rsv_selective_mask_get(unit, port, DC3MAC_RSV_MASK_PAUSE_FRAME,
                                                   &rsv_pass_pause_frames));

    rx_ctrl->pass_control_frames = rx_pass_control_frames & rsv_pass_control_frames;
    rx_ctrl->pass_pfc_frames = rx_pass_pfc_frames & rsv_pass_pfc_frames;
    rx_ctrl->pass_pause_frames = rx_pass_pause_frames & rsv_pass_pause_frames;
exit:
    SOC_FUNC_RETURN;
}

/*set PFC config registers.*/
int pm8x100_gen2_port_pfc_config_set(int unit, int port, pm_info_t pm_info,
                                     const portmod_pfc_config_t* pfc_cfg)
{
    SOC_INIT_FUNC_DEFS;

    if (pfc_cfg->classes != 8) {
        return SOC_E_PARAM;
    }
    _SOC_IF_ERR_EXIT(dc3mac_pfc_config_set(unit, port, pfc_cfg));
    /* set the pfc frame control in dc3mac rsv */
    _SOC_IF_ERR_EXIT(dc3mac_rsv_selective_mask_set(unit, port,
                                                   DC3MAC_RSV_MASK_PFC_FRAME,
                                                   pfc_cfg->rxpass));
exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_pfc_config_get(int unit, int port, pm_info_t pm_info,
                                     portmod_pfc_config_t* pfc_cfg)
{
    SOC_INIT_FUNC_DEFS;

    pfc_cfg->classes = 8;
    _SOC_IF_ERR_EXIT(dc3mac_pfc_config_get(unit, port, pfc_cfg));
    _SOC_IF_ERR_EXIT(dc3mac_rsv_selective_mask_get(unit, port,
                                                   DC3MAC_RSV_MASK_PFC_FRAME,
                                                   &pfc_cfg->rxpass));
exit:
    SOC_FUNC_RETURN;
}

/*set Vlan Inner/Outer tag.*/
int pm8x100_gen2_port_vlan_tag_set(int unit, int port, pm_info_t pm_info,
                                   const portmod_vlan_tag_t* vlan_tag)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(dc3mac_vlan_tag_set(unit, port,
                                         vlan_tag->outer_vlan_tag,
                                         vlan_tag->inner_vlan_tag));
exit:
    SOC_FUNC_RETURN;
}
int pm8x100_gen2_port_vlan_tag_get(int unit, int port, pm_info_t pm_info,
                                   portmod_vlan_tag_t* vlan_tag)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(dc3mac_vlan_tag_get(unit, port,
                                         (int*)&vlan_tag->outer_vlan_tag,
                                         (int*)&vlan_tag->inner_vlan_tag));
exit:
    SOC_FUNC_RETURN;
}

/*set modid field.*/
int pm8x100_gen2_port_mode_set(int unit, int port, pm_info_t pm_info,
                               const portmod_port_mode_info_t* mode)
{

    return (SOC_E_NONE);
}


STATIC
int _pm8x100_gen2_dc3port_mode_get(int unit, int port, pm_info_t pm_info,
                                  int first_phy_index, portmod_port_mode_info_t *p_mode)
{
    uint32 port_mode = 0;
    int phy_acc;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x100_gen2_phy_access_get(unit, port, pm_info, &phy_acc));

    _SOC_IF_ERR_EXIT(PM8X100_GEN2_REG_READ(unit, DC3PORT_MODE_REG, phy_acc, &port_mode));

    switch (port_mode) {
        case 0:
            p_mode->cur_mode = portmodPortModeQuad;
            p_mode->lanes = 1;
            p_mode->port_index = first_phy_index;
            break;
        case 1:
            p_mode->cur_mode = portmodPortModeTri012;
            if ((first_phy_index == 0)||(first_phy_index == 1)) {
                p_mode->lanes = 1;
            } else {
                p_mode->lanes = 2;
            }
            p_mode->port_index = first_phy_index;
            break;
        case 2:
            p_mode->cur_mode = portmodPortModeTri023;
            if ((first_phy_index == 2)||(first_phy_index == 3)) {
                p_mode->lanes = 1;
            } else {
                p_mode->lanes = 2;
            }
            p_mode->port_index = first_phy_index;
            break;
        case 3:
            p_mode->cur_mode = portmodPortModeDual;
            p_mode->lanes = 2;
            p_mode->port_index = first_phy_index;
            break;
        case 4:
            p_mode->cur_mode = portmodPortModeSingle;
            p_mode->lanes = 4;
            p_mode->port_index = first_phy_index;
            break;
        case 8:
            p_mode->cur_mode = portmodPortModeSingle_8;
            p_mode->lanes = 4;
            p_mode->port_index = 0;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid port mode")));
            break;
    }

exit:
    SOC_FUNC_RETURN;

}

int pm8x100_gen2_port_mode_get(int unit, int port, pm_info_t pm_info,
                               portmod_port_mode_info_t* mode)
{
    int port_index;
    uint32_t bitmap;

    SOC_INIT_FUNC_DEFS;

     _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_index_get (unit, port, pm_info, &port_index, &bitmap));
     _SOC_IF_ERR_EXIT(_pm8x100_gen2_dc3port_mode_get(unit, port, pm_info, port_index, mode));

exit:
    SOC_FUNC_RETURN;
}

/*set port encap.*/
int pm8x100_gen2_port_encap_set(int unit, int port, pm_info_t pm_info,
                                int flags, portmod_encap_t encap)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(dc3mac_encap_set(unit, port, encap));
exit:
    SOC_FUNC_RETURN;
}
int pm8x100_gen2_port_encap_get(int unit, int port, pm_info_t pm_info,
                                int* flags, portmod_encap_t* encap)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(dc3mac_encap_get(unit, port, encap));
exit:
    SOC_FUNC_RETURN;
}

/*set/get hwfailover for trident.*/
int pm8x100_gen2_port_diag_ctrl(int unit, int port, pm_info_t pm_info,
                                uint32 inst, int op_type, int op_cmd,
                                const void* arg)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    portmod_access_get_params_t params;
    int nof_phys = 0;

    SOC_INIT_FUNC_DEFS;
    /* Only internal phy supported. */
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                      &params, 1, phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(portmod_phy_port_diag_ctrl(unit, port, phy_access, nof_phys, inst, op_type, op_cmd, arg));

exit:
    SOC_FUNC_RETURN;

}

int pm8x100_gen2_port_cntmaxsize_set(int unit, int port, pm_info_t pm_info, int val)
{
    return (dc3mac_mib_oversize_set(unit, port, val));
}

int pm8x100_gen2_port_cntmaxsize_get(int unit, int port, pm_info_t pm_info, int* val)
{
    return (dc3mac_mib_oversize_get(unit, port, (uint32_t *)val));
}

/*Get Info needed to restore after drain cells.*/
int pm8x100_gen2_port_drain_cell_get(int unit, int port, pm_info_t pm_info,
                                     portmod_drain_cells_t* drain_cells)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(dc3mac_drain_cell_get(unit, port, drain_cells));

exit:
    SOC_FUNC_RETURN;
}

/*Restore informaation after drain cells.*/
int pm8x100_gen2_port_drain_cell_stop(int unit, int port, pm_info_t pm_info,
                                      const portmod_drain_cells_t* drain_cells)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(dc3mac_drain_cell_stop(unit, port, drain_cells));

exit:
    SOC_FUNC_RETURN;
}

/*Restore informaation after drain cells.*/
int pm8x100_gen2_port_drain_cell_start(int unit, int port, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(dc3mac_drain_cell_start(unit, port));

exit:
    SOC_FUNC_RETURN;
}

/**/
int pm8x100_gen2_port_drain_cells_rx_enable(int unit, int port,
                                            pm_info_t pm_info,
                                            int rx_en)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(dc3mac_drain_cells_rx_enable(unit, port, rx_en));

exit:
    SOC_FUNC_RETURN;
}

/**/
int pm8x100_gen2_port_egress_queue_drain_rx_en(int unit, int port,
                                               pm_info_t pm_info,
                                               int rx_en)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(dc3mac_egress_queue_drain_rx_en(unit, port, rx_en));

exit:
    SOC_FUNC_RETURN;
}

/**/
int pm8x100_gen2_port_mac_ctrl_set(int unit, int port,
                                   pm_info_t pm_info,
                                   uint64 ctrl)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(dc3mac_mac_ctrl_set(unit, port, ctrl));

exit:
    SOC_FUNC_RETURN;
}

/**/
int pm8x100_gen2_port_txfifo_cell_cnt_get(int unit, int port,
                                          pm_info_t pm_info,
                                          uint32* cnt)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(dc3mac_txfifo_cell_cnt_get(unit, port, cnt));

exit:
    SOC_FUNC_RETURN;
}

/**/
int pm8x100_gen2_port_egress_queue_drain_get(int unit, int port,
                                             pm_info_t pm_info,
                                             uint64* ctrl, int* rxen)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(dc3mac_egress_queue_drain_get(unit, port, ctrl, rxen));

exit:
    SOC_FUNC_RETURN;
}

/**/
int pm8x100_gen2_port_mac_reset_set(int unit, int port, pm_info_t pm_info, int val)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(dc3mac_reset_set(unit, port, val));

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_mac_reset_get(int unit, int port, pm_info_t pm_info, int* val)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(dc3mac_reset_get(unit, port, (uint32_t *)val));

    exit:
        SOC_FUNC_RETURN;
}

/*Check if MAC needs to be reset.*/
int pm8x100_gen2_port_mac_reset_check(int unit, int port,
                                      pm_info_t pm_info,
                                      int enable, int* reset)
{
    return (dc3mac_reset_check(unit, port, enable, reset));
}


/*get the speed for the specified port*/
int pm8x100_gen2_port_speed_get(int unit, int port, pm_info_t pm_info, int* speed)
{
    phymod_phy_speed_config_t speed_config;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    *speed = 0;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                   &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_t_init(&speed_config));
    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_get(&phy_access, &speed_config));

    *speed = speed_config.data_rate;
exit:
    SOC_FUNC_RETURN;
}

/*Port discard set*/
int pm8x100_gen2_port_discard_set(int unit, int port, pm_info_t pm_info, int discard)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(dc3mac_discard_set(unit, port, discard));

exit:
    SOC_FUNC_RETURN;
}

/*Port tx_en=0 and softreset mac*/
int pm8x100_gen2_port_tx_down(int unit, int port, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(dc3mac_tx_enable_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(dc3mac_discard_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(dc3mac_reset_set(unit, port, 1));

exit:
    SOC_FUNC_RETURN;
}

/*port control phy timesync config set/get*/
int pm8x100_gen2_port_control_phy_timesync_set(int unit,
                                               int port, pm_info_t pm_info,
                                               portmod_port_control_phy_timesync_t config,
                                               uint64 value)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    uint32_t flags = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    if (config == SOC_PORT_CONTROL_PHY_TIMESYNC_TIMESTAMP_ADJUST) {
        PHYMOD_TIMESYNC_ENABLE_F_RX_SET(flags);
        _SOC_IF_ERR_EXIT(phymod_timesync_enable_set(&phy_access, flags, 0));

    }
    _SOC_IF_ERR_EXIT(portmod_common_control_phy_timesync_set(&phy_access, config, value));

    if (config == SOC_PORT_CONTROL_PHY_TIMESYNC_TIMESTAMP_ADJUST) {
        _SOC_IF_ERR_EXIT(phymod_timesync_enable_set(&phy_access, flags, 1));
        /* Wait for one AM spacing time */
        sal_usleep(500);
    }
exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_control_phy_timesync_get(int unit,
                                         int port, pm_info_t pm_info,
                                         portmod_port_control_phy_timesync_t config,
                                         uint64* value)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(portmod_common_control_phy_timesync_get(&phy_access, config, value));

exit:
    SOC_FUNC_RETURN;
}

/*"port timesync config set/get"*/
int pm8x100_gen2_port_timesync_config_set(int unit, int port, pm_info_t pm_info,
                                          const portmod_phy_timesync_config_t* config)
{
    uint32_t mac_da_timestamp_en = 0;
    int port_index, /*port_ts_status, */ts_is_enable, ts_enable_port_count;
    uint32 bitmap, one_step_req, is_one_step = 0, flags = 0;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, enable;
    uint32_t timesync_config = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    enable = (config->flags & SOC_PORT_PHY_TIMESYNC_ENABLE) ? 1 : 0;

    _SOC_IF_ERR_EXIT
        (PM8x100_GEN2_TIMESYNC_CONFIG_GET(unit, pm_info, &timesync_config, port_index));
    ts_is_enable = PORTMOD_PORT_TIMESYNC_CONFIG_ENABLE_GET(timesync_config);
    _SOC_IF_ERR_EXIT(PM8x100_GEN2_TS_ENABLE_PORT_COUNT_GET(unit, pm_info, &ts_enable_port_count));

    /* Clear port timestamp status. */
    timesync_config = 0;

    if (enable) {
        /* Enable 1588. */
        if (ts_is_enable) {
            /* Clear current 1588 configs if the port has 1588 enabled. */
            PHYMOD_TIMESYNC_ENABLE_F_RX_SET(flags);
            PHYMOD_TIMESYNC_ENABLE_F_ONE_STEP_PIPELINE_SET(flags);
            _SOC_IF_ERR_EXIT
                (phymod_timesync_enable_set(&phy_access, flags, 0));
            flags = 0;
        } else {
            /* 1588 is not enabled on the port. */
            if (ts_enable_port_count == 0) {
                /* This is the first 1588 port on the PM. */
                PHYMOD_TIMESYNC_ENABLE_F_CORE_SET(flags);
            }
            ts_enable_port_count++;
        }
        one_step_req = (config->flags & SOC_PORT_PHY_TIMESYNC_ONE_STEP_ENABLE) ? 1 : 0;
        if (one_step_req) {
            PHYMOD_TIMESYNC_ENABLE_F_ONE_STEP_PIPELINE_SET(flags);
        }
        /* Rx enable will be done on link up as per programming requirement. */
        _SOC_IF_ERR_EXIT
                (phymod_timesync_enable_set(&phy_access, flags, 1));

        /* Update port timestamp status. */
        PORTMOD_PORT_TIMESYNC_CONFIG_ENABLE_SET(timesync_config);

        mac_da_timestamp_en = 0;

        if (config->flags & SOC_PORT_PHY_TIMESYNC_SELECT_SFD) {
            PORTMOD_PORT_TIMESYNC_CONFIG_SFD_SET(timesync_config);
        }
        if (config->flags & SOC_PORT_PHY_TIMESYNC_SELECT_MAC_DA) {
            PORTMOD_PORT_TIMESYNC_CONFIG_MAC_DA_SET(timesync_config);
            mac_da_timestamp_en = 1;
        }
        if (config->flags & SOC_PORT_PHY_TIMESYNC_COMP_MODE_ON) {
            PORTMOD_PORT_TIMESYNC_CONFIG_COMP_MODE_ON_SET(timesync_config);
            if (config->flags & SOC_PORT_PHY_TIMESYNC_COMP_MODE_LATEST_LANE){
                PORTMOD_PORT_TIMESYNC_CONFIG_COMP_MODE_LATEST_LANE_SET(timesync_config);
            } else {
                PORTMOD_PORT_TIMESYNC_CONFIG_COMP_MODE_LATEST_LANE_CLR(timesync_config);
            }
        } else {
            PORTMOD_PORT_TIMESYNC_CONFIG_COMP_MODE_ON_CLR(timesync_config);
            PORTMOD_PORT_TIMESYNC_CONFIG_COMP_MODE_LATEST_LANE_CLR(timesync_config);
        }

        /* program dc3mac da_timestamp_en. */
        _SOC_IF_ERR_EXIT
            (dc3mac_rx_da_timestmap_enable_set(unit, port, mac_da_timestamp_en));
    } else {
        /* Disable 1588. */
        if (ts_is_enable) {
            /* If 1588 is currently enabled on the port. */
            /* 1. Disable One Stpe pipeline if it is enabled. */
            PHYMOD_TIMESYNC_ENABLE_F_ONE_STEP_PIPELINE_SET(flags);
            _SOC_IF_ERR_EXIT
                (phymod_timesync_enable_get(&phy_access, flags, &is_one_step));
            if (!is_one_step) {
                flags = 0;
            }
            /* 2. Disable RX timestamp. */
            PHYMOD_TIMESYNC_ENABLE_F_RX_SET(flags);

            ts_enable_port_count--;
            /* 3. Disable FCLK if no port will using 1588 on the PM. */
            if (ts_enable_port_count == 0) {
                PHYMOD_TIMESYNC_ENABLE_F_CORE_SET(flags);
            }

            _SOC_IF_ERR_EXIT
                (phymod_timesync_enable_set(&phy_access, flags, 0));
        }
    }

    /* Update Timesync Enable Status in WB */
    _SOC_IF_ERR_EXIT(PM8x100_GEN2_TS_ENABLE_PORT_COUNT_SET(unit, pm_info, ts_enable_port_count));
    if (enable) {
        PORTMOD_PORT_TIMESYNC_CONFIG_ENABLE_SET(timesync_config);
    } else {
        PORTMOD_PORT_TIMESYNC_CONFIG_ENABLE_CLR(timesync_config);
    }
    _SOC_IF_ERR_EXIT(PM8x100_GEN2_TIMESYNC_CONFIG_SET(unit, pm_info, timesync_config, port_index));

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_timesync_config_get(int unit, int port, pm_info_t pm_info,
                                    portmod_phy_timesync_config_t* config)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, timesync_config, timesync_enable = 0;
    int port_index;
    uint32 bitmap, is_one_step, flags = 0;
    SOC_INIT_FUNC_DEFS;

    sal_memset(config, 0,sizeof(portmod_phy_timesync_config_t));

    _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(PM8x100_GEN2_TIMESYNC_CONFIG_GET(unit, pm_info, &timesync_config, port_index));
    PHYMOD_TIMESYNC_ENABLE_F_ONE_STEP_PIPELINE_SET(flags);
    _SOC_IF_ERR_EXIT(phymod_timesync_enable_get(&phy_access, flags, &is_one_step));
    timesync_enable = PORTMOD_PORT_TIMESYNC_CONFIG_ENABLE_GET(timesync_config);
    config->flags |= timesync_enable ? (is_one_step? SOC_PORT_PHY_TIMESYNC_ONE_STEP_ENABLE : SOC_PORT_PHY_TIMESYNC_TWO_STEP_ENABLE) : 0;
    config->flags |= PORTMOD_PORT_TIMESYNC_CONFIG_ENABLE_GET(timesync_config)?
                         SOC_PORT_PHY_TIMESYNC_ENABLE : 0;
    config->flags |= PORTMOD_PORT_TIMESYNC_CONFIG_SFD_GET(timesync_config)?
                         SOC_PORT_PHY_TIMESYNC_SELECT_SFD: 0;
    config->flags |= PORTMOD_PORT_TIMESYNC_CONFIG_MAC_DA_GET(timesync_config)?
                         SOC_PORT_PHY_TIMESYNC_SELECT_MAC_DA: 0;
    config->flags |= PORTMOD_PORT_TIMESYNC_CONFIG_COMP_MODE_ON_GET(timesync_config)?
                         SOC_PORT_PHY_TIMESYNC_COMP_MODE_ON: 0;
    config->flags |= PORTMOD_PORT_TIMESYNC_CONFIG_COMP_MODE_LATEST_LANE_GET(timesync_config) ?
                         SOC_PORT_PHY_TIMESYNC_COMP_MODE_LATEST_LANE: 0;

exit:
    SOC_FUNC_RETURN;
}

/*set/get interrupt enable value. */
int pm8x100_gen2_port_interrupt_enable_set(int unit, int port,
                                           pm_info_t pm_info,
                                           int intr_type, uint32 val)
{
    uint32 reg_val;
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x100_gen2_phy_access_get(unit, port, pm_info, &phy_acc));
    _SOC_IF_ERR_EXIT(PM8X100_GEN2_REG_READ(unit, DC3PORT_PORT_INTR_ENABLE, phy_acc, &reg_val));


    switch(intr_type) {
        case portmodIntrTypeLinkdown:
            UA_REG_FIELD_SET(unit, DC3PORT_PORT_INTR_ENABLE, &reg_val, LINK_DOWN, &val);
            break;
        case portmodIntrTypeLinkup:
            UA_REG_FIELD_SET(unit, DC3PORT_PORT_INTR_ENABLE, &reg_val, LINK_UP, &val);
            break;
        case portmodIntrTypeTxPktUnderflow:
        case portmodIntrTypeTxPktOverflow:
        case portmodIntrTypeTxCdcSingleBitErr:
        case portmodIntrTypeTxCdcDoubleBitErr:
        case portmodIntrTypeLocalFaultStatus:
        case portmodIntrTypeRemoteFaultStatus:
        case portmodIntrTypeMibMemSingleBitErr:
        case portmodIntrTypeMibMemDoubleBitErr:
        case portmodIntrTypeMibMemMultipleBitErr:
        case portmodIntrTypeRxPfcFifoOverflow:
        case portmodIntrTypeTxPfcFifoOverflow:
        case portmodIntrTypeFdrInterrupt:
            _SOC_IF_ERR_EXIT(dc3mac_interrupt_enable_set(unit, port, intr_type, val));
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
            break;
    }

    _SOC_IF_ERR_EXIT(PM8X100_GEN2_REG_WRITE(unit, DC3PORT_PORT_INTR_ENABLE, phy_acc, &reg_val));


exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm8x100_gen2_phy_timesync_tx_info_get(int unit, int port, pm_info_t pm_info, portmod_fifo_status_t* tx_info)
{
    phymod_phy_access_t phy_access;
    int nof_phys;
    int start_lane;
    uint32_t lane_mask;
    uint32_t ts_valid =0;
    portmod_access_get_params_t params;
    phymod_ts_fifo_status_t phy_ts_tx_info;
    SOC_INIT_FUNC_DEFS;

    /* get start lane and lane mask */
    _SOC_IF_ERR_EXIT
        (_pm8x100_gen2_port_index_get(unit, port, pm_info, &start_lane, &lane_mask));

    _SOC_IF_ERR_EXIT(phymod_ts_fifo_status_t_init(&phy_ts_tx_info));
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT
        (dc3port_timestamp_fifo_status_get(unit, port, start_lane, &ts_valid));
    if(ts_valid){
        _SOC_IF_ERR_EXIT
            (phymod_timesync_tx_info_get(&phy_access, &phy_ts_tx_info));
        tx_info->timestamps_in_fifo     = phy_ts_tx_info.ts_in_fifo_lo;
        tx_info->timestamps_in_fifo_hi  = phy_ts_tx_info.ts_in_fifo_hi;
        tx_info->sequence_id            = phy_ts_tx_info.ts_seq_id;
        tx_info->timestamp_sub_nanosec  = phy_ts_tx_info.ts_sub_nanosec;
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_EMPTY, (_SOC_MSG("FIFO Empty")));
    }
exit:
    SOC_FUNC_RETURN;
}

/*!
 * pm8x100_gen2_port_timesync_tx_info_get
 *
 * @brief get port timestamps in fifo
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [inout]  tx_info         - timestamp and seq id form fifo
 */

int pm8x100_gen2_port_timesync_tx_info_get(int unit, int port, pm_info_t pm_info, portmod_fifo_status_t* tx_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x100_gen2_phy_timesync_tx_info_get(unit, port, pm_info, tx_info));

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_interrupt_enable_get(int unit, int port,
                                           pm_info_t pm_info,
                                           int intr_type, uint32* val)
{
    uint32 reg_val;
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x100_gen2_phy_access_get(unit, port, pm_info, &phy_acc));
    _SOC_IF_ERR_EXIT(PM8X100_GEN2_REG_READ(unit, DC3PORT_PORT_INTR_ENABLE, phy_acc, &reg_val));

    switch(intr_type) {
        case portmodIntrTypeLinkdown:
            UA_REG_FIELD_GET(unit, DC3PORT_PORT_INTR_ENABLE, &reg_val, LINK_DOWN, val);
            break;
        case portmodIntrTypeLinkup:
            UA_REG_FIELD_GET(unit, DC3PORT_PORT_INTR_ENABLE, &reg_val, LINK_UP, val);
            break;
        case portmodIntrTypeTxPktUnderflow:
        case portmodIntrTypeTxPktOverflow:
        case portmodIntrTypeTxCdcSingleBitErr:
        case portmodIntrTypeTxCdcDoubleBitErr:
        case portmodIntrTypeLocalFaultStatus:
        case portmodIntrTypeRemoteFaultStatus:
        case portmodIntrTypeMibMemSingleBitErr:
        case portmodIntrTypeMibMemDoubleBitErr:
        case portmodIntrTypeMibMemMultipleBitErr:
        case portmodIntrTypeRxPfcFifoOverflow:
        case portmodIntrTypeTxPfcFifoOverflow:
        case portmodIntrTypeFdrInterrupt:
            _SOC_IF_ERR_EXIT(dc3mac_interrupt_enable_get(unit, port, intr_type, val));
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

/*get interrupt status value. */
int pm8x100_gen2_port_interrupt_get(int unit, int port, pm_info_t pm_info, int intr_type, uint32* val)
{
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x100_gen2_phy_access_get(unit, port, pm_info, &phy_acc));

    switch(intr_type) {
        case portmodIntrTypeTxPktUnderflow:
        case portmodIntrTypeTxPktOverflow:
        case portmodIntrTypeTxCdcSingleBitErr:
        case portmodIntrTypeTxCdcDoubleBitErr:
        case portmodIntrTypeLocalFaultStatus:
        case portmodIntrTypeRemoteFaultStatus:
        case portmodIntrTypeMibMemSingleBitErr:
        case portmodIntrTypeMibMemDoubleBitErr:
        case portmodIntrTypeMibMemMultipleBitErr:
        case portmodIntrTypeRxPfcFifoOverflow:
        case portmodIntrTypeTxPfcFifoOverflow:
        case portmodIntrTypeFdrInterrupt:
            _SOC_IF_ERR_EXIT(dc3mac_interrupt_status_get(unit, port, intr_type, val));
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

/*get interrupt value array. */
int pm8x100_gen2_port_interrupts_get(int unit, int port, pm_info_t pm_info,
                                     int arr_max_size, uint32* intr_arr,
                                     uint32* size)
{
    uint32 cnt=0;
    int phy_acc;
    uint32 mac_intr_cnt = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x100_gen2_phy_access_get(unit, port, pm_info, &phy_acc));

    _SOC_IF_ERR_EXIT(dc3mac_interrupts_status_get(unit, port, (arr_max_size - cnt), (intr_arr + cnt), &mac_intr_cnt));
    cnt += mac_intr_cnt;
    *size = cnt;

exit:
    SOC_FUNC_RETURN;
}

/* portmod port rsv mask set*/
int pm8x100_gen2_port_mac_rsv_mask_set(int unit, int port,
                                       pm_info_t pm_info,
                                       uint32 rsv_mask)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(dc3mac_rsv_mask_set(unit, port, rsv_mask));

exit:
    SOC_FUNC_RETURN;
}

/*Returns if the PortMacro associated with the port is initialized or not*/
int pm8x100_gen2_pm_is_initialized(int unit, int pm_id, pm_info_t pm_info, int* is_initialized)
{
    int is_core_initialized = 0, rv;
     rv = PM8x100_GEN2_IS_CORE_INITIALIZED_GET(unit, pm_info, &is_core_initialized);
     *is_initialized = is_core_initialized;

    return rv;
}

/* get the logical port bitmap of the current PM */
int pm8x100_gen2_pm_logical_pbmp_get(int unit, int pm_id, pm_info_t pm_info,
                                     portmod_pbmp_t* logical_pbmp)
{
    int i, port;
    SOC_INIT_FUNC_DEFS;

    PORTMOD_PBMP_CLEAR(*logical_pbmp);
    for (i = 0 ; i < MAX_PORTS_PER_PM8X100_GEN2; i++) {
        _SOC_IF_ERR_EXIT(PM8x100_GEN2_LANE2PORT_GET(unit, pm_info, i, &port));
        /*
         * A physical lane can be associated with a logical port number
         * -1 if the port was previously detached.
         */
        if (port >= 0) {
            PORTMOD_PBMP_PORT_ADD(*logical_pbmp, port);
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_core_add(int unit, int pm_id, pm_info_t pm_info, int flags, const portmod_port_add_info_t* add_info)
{
    int is_initialized = 0;
    int pm_enable_flags = 0;
    int force_mac_init = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pm8x100_gen2_pm_is_initialized(unit, pm_id, pm_info, &is_initialized));

    if (PORTMOD_CORE_ADD_SKIP_MAC_GET(flags)) {
        PORTMOD_PM_ENABLE_SKIP_MAC_SET(pm_enable_flags);
    }
    force_mac_init = PORTMOD_CORE_ADD_FORCE_MAC_GET(flags);
    if (is_initialized && force_mac_init) {
        PORTMOD_PM_ENABLE_MAC_ONLY_SET(pm_enable_flags);
    }
    if (!is_initialized || force_mac_init) {
        _SOC_IF_ERR_EXIT(pm8x100_gen2_pm_enable(unit, pm_id, pm_info, pm_enable_flags, 1));
    }

    _SOC_IF_ERR_EXIT(pm8x100_gen2_pm_serdes_core_init(unit, pm_id, pm_info, add_info));

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_lane_map_set(int unit, int port, pm_info_t pm_info, uint32 flags, const phymod_lane_map_t* lane_map)
{
    LOG_ERROR(BSL_LS_SOC_COMMON,
        (BSL_META_U(unit, "%s() is not supported\n"), FUNCTION_NAME()));
    return SOC_E_UNAVAIL;
}

int pm8x100_gen2_port_lane_map_get(int unit, int port, pm_info_t pm_info, uint32 flags, phymod_lane_map_t* lane_map)
{
    SOC_INIT_FUNC_DEFS;

    *lane_map = PM_8x100_GEN2_INFO(pm_info)->lane_map;

    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_synce_clk_ctrl_set(int unit, int port, pm_info_t pm_info,
                                         const portmod_port_synce_clk_ctrl_t* cfg)
{
    phymod_synce_clk_ctrl_t phy_synce_cfg;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    int config_valid = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    phymod_synce_clk_ctrl_t_init(&phy_synce_cfg);
    phy_synce_cfg.stg0_mode = cfg->stg0_mode;
    phy_synce_cfg.stg1_mode = cfg->stg1_mode;
    phy_synce_cfg.sdm_val = cfg->sdm_val;

    /* next validate the stage0/1 config */
    /* first check legacy mode */
    if (cfg->stg0_mode == 0x0) {
        config_valid = 1;
    } else if ((cfg->stg0_mode == 0x1) && (cfg->stg1_mode == 0x2)) {
        config_valid = 1;
    } else if ((cfg->stg0_mode == 0x2) && (cfg->stg1_mode == 0x0)) {
        config_valid = 1;
    }

    if (config_valid) {
        _SOC_IF_ERR_EXIT(phymod_phy_synce_clk_ctrl_set(&phy_access,
                                                       phy_synce_cfg));
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                  (_SOC_MSG("SyncE config is not valid \n")));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_synce_clk_ctrl_get(int unit, int port, pm_info_t pm_info,
                                         portmod_port_synce_clk_ctrl_t* cfg)
{
    phymod_synce_clk_ctrl_t phy_synce_cfg;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    phymod_synce_clk_ctrl_t_init(&phy_synce_cfg);

    _SOC_IF_ERR_EXIT(phymod_phy_synce_clk_ctrl_get(&phy_access,
                                                    &phy_synce_cfg));

    cfg->stg0_mode = phy_synce_cfg.stg0_mode;
    cfg->stg1_mode = phy_synce_cfg.stg1_mode;
    cfg->sdm_val = phy_synce_cfg.sdm_val;

exit:
    SOC_FUNC_RETURN;
}


int pm8x100_gen2_pm_interrupt_process(int unit, int pm_id, pm_info_t pm_info,
                                      portmod_ecc_intr_info_t *ecc_info)
{

    return SOC_E_NONE;
}


int pm8x100_gen2_port_mac_enable_set(int unit,int port, pm_info_t pm_info,
                                     int enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(dc3mac_port_enable_set(unit, port, enable));
exit:
    SOC_FUNC_RETURN;
} 

/* This function collects the reasons for local_faults that might have 
 * happened before it's invocation. Returns a bit-map with all those
 * reasons
 */
int pm8x100_gen2_port_local_fault_reasons_get(int unit, int port, pm_info_t pm_info,
                                              uint32_t* local_fault_reasons)
{
    int no_local_fault = FALSE;
    uint32_t an_done;
    int nof_phys;
    int port_index;
    uint32_t bitmap[1], is_bypassed = 0;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    portmod_speed_config_t speed_config;
    phymod_autoneg_control_t an_control;
    phymod_phy_local_fault_info_t local_fault_info;

    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    *local_fault_reasons = 0;
    sal_memset(&speed_config, 0, sizeof(portmod_speed_config_t));
    sal_memset(&local_fault_info, 0, sizeof(phymod_phy_local_fault_info_t));

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                &params, 1, &phy_access, &nof_phys, NULL));


    _SOC_IF_ERR_EXIT(_pm8x100_gen2_port_index_get(unit, port, pm_info, &port_index, bitmap));
    _SOC_IF_ERR_EXIT(PM8x100_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));

    if (is_bypassed) {
        PORTMOD_PORT_LOCAL_FAULT_REASON_UNKNOWN_SET(*local_fault_reasons);
        return SOC_E_NONE;
    }

    _SOC_IF_ERR_EXIT(phymod_phy_autoneg_get(&phy_access, &an_control, &an_done));

    if (an_control.enable) {
        PORTMOD_PORT_LOCAL_FAULT_REASON_UNKNOWN_SET(*local_fault_reasons);
        return SOC_E_NONE;
    }

    _SOC_IF_ERR_EXIT
        (phymod_phy_local_fault_info_get(&phy_access, &local_fault_info));

    if (!local_fault_info.pcs_latched_local_fault && 
        local_fault_info.pcs_link_status_live) {
        no_local_fault = TRUE;
    }

    if (local_fault_info.pcs_latched_deskew_low) {
        PORTMOD_PORT_LOCAL_FAULT_REASON_NO_DESKEW_SET(*local_fault_reasons);
    }

    if (local_fault_info.pmd_rx_locked && !local_fault_info.pmd_rx_lock_change) {
        /* PMD is locked and there was no change. 
         * Don't need to report PMD no lock reason
         */
    } else {
        /* For all other cases, report PMD no lock reason */
        PORTMOD_PORT_LOCAL_FAULT_REASON_PMD_NO_LOCK_SET(*local_fault_reasons);
    }

    if (local_fault_info.am_lock_latched_low) {
        PORTMOD_PORT_LOCAL_FAULT_REASON_AM_NO_LOCK_SET(*local_fault_reasons);
    }

    _SOC_IF_ERR_EXIT
        (pm8x100_gen2_port_speed_config_get(unit, port, pm_info, &speed_config));

    if (speed_config.fec != PORTMOD_PORT_PHY_FEC_NONE) {
        if (local_fault_info.fec_align_latched_low) {
            PORTMOD_PORT_LOCAL_FAULT_REASON_FEC_NO_ALIGN_SET(*local_fault_reasons);
        }
    }

    if (!(*local_fault_reasons)) {
        if (no_local_fault) {
            PORTMOD_PORT_LOCAL_FAULT_REASON_NONE_SET(*local_fault_reasons);
        } else {
            PORTMOD_PORT_LOCAL_FAULT_REASON_UNKNOWN_SET(*local_fault_reasons);
        }
    } 

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_fec_error_inject_set(int unit, int port, pm_info_t pm_info,
                                           uint16 error_control_map,
                                           portmod_fec_error_mask_t bit_error_mask)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT
        (phymod_phy_fec_error_inject_set(&phy_access, error_control_map, bit_error_mask));

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_gen2_port_fec_error_inject_get(int unit, int port, pm_info_t pm_info,
                                           uint16* error_control_map,
                                           portmod_fec_error_mask_t* bit_error_mask)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT
        (phymod_phy_fec_error_inject_get(&phy_access, error_control_map, bit_error_mask));

exit:
    SOC_FUNC_RETURN;
}

/*local and remote fault status get*/
int pm8x100_gen2_port_faults_status_get(int unit, int port, pm_info_t pm_info,
                                        portmod_port_fault_status_t* faults)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(dc3mac_remote_fault_status_get(unit, port, &(faults->remote)));
    _SOC_IF_ERR_EXIT(dc3mac_local_fault_status_get(unit, port, &(faults->local)));

exit:
    SOC_FUNC_RETURN;
}
int pm8x100_gen2_port_pmd_rx_lock_status_get(int unit, int port, pm_info_t pm_info, uint32* pmd_lock, uint32* pmd_lock_change)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    uint32_t rx_pmd_locked, rx_pmd_lock_change;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT
        (phymod_phy_rx_pmd_lock_status_get(&phy_access, &rx_pmd_locked, &rx_pmd_lock_change));

    *pmd_lock = rx_pmd_locked;
    *pmd_lock_change = rx_pmd_lock_change;

exit:
    SOC_FUNC_RETURN;
}

/*get link status*/
int pm8x100_gen2_port_link_get(int unit, int port, pm_info_t pm_info,
                               int flags, int* link)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    if (SAL_BOOT_SIMULATION){
        _SOC_IF_ERR_EXIT(pm8x100_gen2_port_mac_link_get(unit, port, pm_info, link));
    } else {
        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
        _SOC_IF_ERR_EXIT(pm8x100_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                        &params, 1, &phy_access, &nof_phys, NULL));
        _SOC_IF_ERR_EXIT(phymod_phy_link_status_get(&phy_access, (uint32_t *) link));
    }
exit:
    SOC_FUNC_RETURN;
}

#endif
