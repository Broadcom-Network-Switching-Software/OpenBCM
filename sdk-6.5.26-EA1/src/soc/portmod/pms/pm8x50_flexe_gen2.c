/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

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
#include <soc/portmod/portmod_legacy_phy.h>

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM8X50_FLEXE_GEN2_SUPPORT

#include <soc/portmod/cdmac.h>
#include <soc/portmod/pm8x50_flexe_gen2.h>
#include <soc/portmod/pm8x50_internal.h>

#define PM_8x50_FLEXE_GEN2_INFO(pm_info) ((pm_info)->pm_data.pm8x50_flexe_gen2_db)

/* Warmboot variable defines - start from 1.
 * Need to update the version once new WB variable added.
 */
#define PM8x50_FLEXE_GEN2_WB_BUFFER_VERSION        (1)
#define PORTMOD_THREAD_INTERVAL_1s 1000000
#define TIMEOUT_COUNT_400G_AN     10
#define COMPLETE_COUNT_400G_AN    2
#define AN_LINK_FAIL_INHIBIT_TIMER_LT_PAM4_DEFAULT (3200)

#define TXPI_DSM_VALUE_1PPM_HI (uint32) 0x29   /* 1ppm value is 1.80144E+11 (0x29F16B11C7) in register */
#define TXPI_DSM_VALUE_1PPM_LO (uint32) 0xF16B11C7

#define PM8x50_FLEXE_GEN2_IS_CORE_INITIALIZED_SET(unit, pm_info, is_core_initialized) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[isCoreInitialized],         \
                          &is_core_initialized)
#define PM8x50_FLEXE_GEN2_IS_CORE_INITIALIZED_GET(unit, pm_info, is_core_initialized) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[isCoreInitialized],         \
                          is_core_initialized)

#define PM8x50_FLEXE_GEN2_IS_ACTIVE_SET(unit, pm_info, is_active)   \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[isActive],\
                         &is_active)
#define PM8x50_FLEXE_GEN2_IS_ACTIVE_GET(unit, pm_info, is_active)    \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[isActive],\
                          is_active)

#define PM8x50_FLEXE_GEN2_PLL0_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll0_active_lane_bitmap)\
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                            \
                          pm_info->wb_vars_ids[pll0ActiveLaneBitmap],             \
                          &pll0_active_lane_bitmap)
#define PM8x50_FLEXE_GEN2_PLL0_ACTIVE_LANE_BITMAP_GET(unit, pm_info, pll0_active_lane_bitmap)\
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                            \
                          pm_info->wb_vars_ids[pll0ActiveLaneBitmap],             \
                          pll0_active_lane_bitmap)

#define PM8x50_FLEXE_GEN2_PLL1_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll1_active_lane_bitmap)\
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                            \
                          pm_info->wb_vars_ids[pll1ActiveLaneBitmap],             \
                          &pll1_active_lane_bitmap)
#define PM8x50_FLEXE_GEN2_PLL1_ACTIVE_LANE_BITMAP_GET(unit, pm_info, pll1_active_lane_bitmap)\
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                            \
                          pm_info->wb_vars_ids[pll1ActiveLaneBitmap],             \
                          pll1_active_lane_bitmap)

#define PM8x50_FLEXE_GEN2_PLL0_ADV_LANE_BITMAP_SET(unit, pm_info, pll0_adv_lane_bitmap)\
        SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                            \
                              pm_info->wb_vars_ids[pll0AdvLaneBitmap],             \
                              &pll0_adv_lane_bitmap)
#define PM8x50_FLEXE_GEN2_PLL0_ADV_LANE_BITMAP_GET(unit, pm_info, pll0_adv_lane_bitmap)\
        SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                            \
                              pm_info->wb_vars_ids[pll0AdvLaneBitmap],             \
                              pll0_adv_lane_bitmap)

#define PM8x50_FLEXE_GEN2_PLL1_ADV_LANE_BITMAP_SET(unit, pm_info, pll1_adv_lane_bitmap)\
        SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                            \
                              pm_info->wb_vars_ids[pll1AdvLaneBitmap],             \
                              &pll1_adv_lane_bitmap)
#define PM8x50_FLEXE_GEN2_PLL1_ADV_LANE_BITMAP_GET(unit, pm_info, pll1_adv_lane_bitmap)\
        SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                            \
                              pm_info->wb_vars_ids[pll1AdvLaneBitmap],             \
                              pll1_adv_lane_bitmap)

#define PM8x50_FLEXE_GEN2_LANE2PORT_SET(unit, pm_info, lane, port)                  \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                   \
                          pm_info->wb_vars_ids[lane2portMap], &port, lane)
#define PM8x50_FLEXE_GEN2_LANE2PORT_GET(unit, pm_info, lane, port)                 \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                  \
                          pm_info->wb_vars_ids[lane2portMap], port, lane)

#define PM8x50_FLEXE_GEN2_MAX_SPEED_SET(unit, pm_info, max_speed, port_index)              \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                          \
                          pm_info->wb_vars_ids[maxSpeed], &max_speed, port_index)
#define PM8x50_FLEXE_GEN2_MAX_SPEED_GET(unit, pm_info, max_speed, port_index)             \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                         \
                          pm_info->wb_vars_ids[maxSpeed], max_speed, port_index)

#define PM8x50_FLEXE_GEN2_AN_MODE_SET(unit, pm_info, an_mode, port_index)              \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[anMode], &an_mode, port_index)
#define PM8x50_FLEXE_GEN2_AN_MODE_GET(unit, pm_info, an_mode, port_index)             \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[anMode], an_mode, port_index)

#define PM8x50_FLEXE_GEN2_LANE2FEC_SET(unit, pm_info, lane, fec)                   \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                  \
                          pm_info->wb_vars_ids[lane2fecMap], &fec, lane)
#define PM8x50_FLEXE_GEN2_LANE2FEC_GET(unit, pm_info, lane, fec)                   \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                  \
                          pm_info->wb_vars_ids[lane2fecMap], fec, lane)

#define PM8x50_FLEXE_GEN2_PORT_IS_PCS_BYPASSED_SET(unit, pm_info, is_bypassed, port_index)             \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                                      \
                          pm_info->wb_vars_ids[portIsPcsBypassed], &is_bypassed, port_index)
#define PM8x50_FLEXE_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, is_bypassed, port_index)             \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                                      \
                          pm_info->wb_vars_ids[portIsPcsBypassed], is_bypassed, port_index)

#define PM8x50_FLEXE_GEN2_TS_ENABLE_PORT_COUNT_SET(unit, pm_info, ts_enable_port_count)   \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[tsEnablePortCount],\
                         &ts_enable_port_count)
#define PM8x50_FLEXE_GEN2_TS_ENABLE_PORT_COUNT_GET(unit, pm_info, ts_enable_port_count)    \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[tsEnablePortCount],\
                          ts_enable_port_count)

#define PM8x50_FLEXE_GEN2_TIMESYNC_CONFIG_SET(unit, pm_info, timesync_config, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[timesyncEnable], &timesync_config, port_index)
#define PM8x50_FLEXE_GEN2_TIMESYNC_CONFIG_GET(unit, pm_info, timesync_config, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[timesyncEnable], timesync_config, port_index)

#define PM8x50_FLEXE_GEN2_400G_AN_ENABLE_SET(unit, pm_info, is_400g_an, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[is400gAn], &is_400g_an, port_index)
#define PM8x50_FLEXE_GEN2_400G_AN_ENABLE_GET(unit, pm_info, is_400g_an, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[is400gAn], is_400g_an, port_index)

#define PM8x50_FLEXE_GEN2_400G_AN_STATE_SET(unit, pm_info, an_state_400g)   \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[anState400g],\
                         &an_state_400g)
#define PM8x50_FLEXE_GEN2_400G_AN_STATE_GET(unit, pm_info, an_state_400g)    \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[anState400g],\
                          an_state_400g)

#define PM8x50_FLEXE_GEN2_400G_AN_WAIT_LINK_COUNT_SET(unit, pm_info, an_wait_link_count)   \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[anWaitlinkCount],\
                         &an_wait_link_count)
#define PM8x50_FLEXE_GEN2_400G_AN_WAIT_LINK_COUNT_GET(unit, pm_info, an_wait_link_count)    \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[anWaitlinkCount],\
                          an_wait_link_count)

#define PM8x50_FLEXE_GEN2_400G_AN_REMOTE_PAUSE_SET(unit, pm_info, an_400g_remote_pause, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[an400gRemotePause], &an_400g_remote_pause, port_index)
#define PM8x50_FLEXE_GEN2_400G_AN_REMOTE_PAUSE_GET(unit, pm_info, an_400g_remote_pause, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[an400gRemotePause], an_400g_remote_pause, port_index)

#define PM8x50_FLEXE_GEN2_PORT_FLEXE_MODE_SET(unit, pm_info, port_flexe_mode, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[portFlexeMode], &port_flexe_mode, port_index)
#define PM8x50_FLEXE_GEN2_PORT_FLEXE_MODE_GET(unit, pm_info, port_flexe_mode, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[portFlexeMode], port_flexe_mode, port_index)

#define PM8x50_FLEXE_GEN2_AN_LINK_FAIL_INHIBIT_TIMER_LT_PAM4_SET(unit, pm_info, link_fail_wait_time) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[anLinkfailInhibitTimerLtPam4],         \
                          &link_fail_wait_time)
#define PM8x50_FLEXE_GEN2_AN_LINK_FAIL_INHIBIT_TIMER_LT_PAM4_GET(unit, pm_info, link_fail_wait_time) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[anLinkfailInhibitTimerLtPam4],         \
                          link_fail_wait_time)

#define PM8x50_FLEXE_GEN2_IS_FLEXIBLE_PLL_SET(unit, pm_info, is_flexible_pll)   \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[isFlexiblePLL],\
                         &is_flexible_pll)
#define PM8x50_FLEXE_GEN2_IS_FLEXIBLE_PLL_GET(unit, pm_info, is_flexible_pll)    \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[isFlexiblePLL],\
                          is_flexible_pll)

#define PM8x50_FLEXE_GEN2_PORT_OTN_MODE_SET(unit, pm_info, port_otn_mode, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[portOtnMode], &port_otn_mode, port_index)
#define PM8x50_FLEXE_GEN2_PORT_OTN_MODE_GET(unit, pm_info, port_otn_mode, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[portOtnMode], port_otn_mode, port_index)


typedef enum pm8x50_flexe_gen2_wb_vars{
    isCoreInitialized,
    isActive,
    pll0ActiveLaneBitmap,
    pll1ActiveLaneBitmap,
    pll0AdvLaneBitmap,
    pll1AdvLaneBitmap,
    lane2portMap,
    maxSpeed,
    anMode,
    lane2fecMap,
    tsEnablePortCount,
    timesyncEnable,
    is400gAn,           /* 400g An enabled */
    anState400g,        /* 400G an state */
    anWaitlinkCount,    /* 400G wait for link up count */
    portIsPcsBypassed,
    an400gRemotePause,  /* 400G AN Remote PAUSE value */
    portFlexeMode,                /* if port is in flexe mode */
    anLinkfailInhibitTimerLtPam4, /* an link fail inhibit timer for PMA4 with link training */
    isFlexiblePLL,                /* flexible PLL required */
    portOtnMode                   /* if port is OTN mode */
}pm8x50_flexe_gen2_wb_vars_t;

/* Warmboot variable defines - end */

/* This macro set devad to reg_addr if it is PMD phy address. */
#define PM8X50_FLEXE_GEN2_PHY_REG_SET(reg_addr) \
    do { \
        uint32 phy_reg_addr; \
        phy_reg_addr = reg_addr & 0xffff; \
        if ((0x90 <= phy_reg_addr && phy_reg_addr <= 0x9f) || \
            (0xd000 <= phy_reg_addr && phy_reg_addr <= 0xd4ff) || \
            (0xffd0 <= phy_reg_addr && phy_reg_addr <= 0xffdf)) { \
            reg_addr |= 1 << 16; \
        } \
    } while (0)

/* Highest NRZ per lane speed is 28.125G, supported in ILKN mode */
#define NRZ_MAX_PER_LANE_SPEED 28125

#define PM8x50_FLEXE_GEN2_FS_ABILITY_TABLE_SIZE 32
#define PM8x50_FLEXE_GEN2_AN_ABILITY_TABLE_SIZE 37
#define PM8x50_FLEXE_GEN2_MAX_AN_ABILITY 20

#define FLEXPORT_SW_WAR_ENABLE  0x1000

/* following defines are used for 400G AN SW WAR */
#define AN_X4_SW_CTRL_STS_REG 0xc1e6
#define AN_X4_SW_CTRL_STS_AN_COMPLETE_MASK 0x8000
#define AN_X4_CL73_CFG_REG 0xc1c0
#define AN_X4_CL73_CFG_AN_DISABLE_MASK 0xfaff
#define MAIN0_SETUP_REG 0x9000
#define MAIN0_SETUP_PORT_MODE_CLEAR 0xff8f
#define MAIN0_SETUP_PORT_MODE_OCTAL 0x50
#define SPEED_CTRL_REG  0xc050
#define SPEED_CTRL_SPEED_ID_CLEAR 0xffc0
#define SPEED_ID_400G    0xa

#define AN_X4_LP_BASE1r_REG 0xc1d3
#define AN_LP_BASE1r_PAGE_PAUSE_MASK   0x3
#define AN_LP_BASE1r_PAGE_PAUSE_OFFSET 0xA

STATIC portmod_ucode_buf_t pm8x50_flexe_gen2_ucode_buf[SOC_MAX_NUM_DEVICES] = {{NULL, 0}};
STATIC portmod_ucode_buf_t pm8x50_flexe_gen2_ucode_buf_2nd[SOC_MAX_NUM_DEVICES] = {{NULL, 0}};

typedef enum pm8x50_flexe_gen2_port_400g_an_state_e {
    PM8X50_FLEXE_GEN2_PORT_AN_CHECK_PAGE_COMPLETE,
    PM8X50_FLEXE_GEN2_PORT_AN_CHECK_PCS_LINKUP,
    PM8X50_FLEXE_GEN2_PORT_AN_CREDIT_RESET,
    PM8X50_FLEXE_GEN2_PORT_AN_DISABLE,
    PM8X50_FLEXE_GEN2_PORT_AN_RESTART,
    PM8X50_FLEXE_GEN2_PORT_AN_COMPLETE,
    PM8X50_FLEXE_GEN2_PORT_AN_STATE_COUNT
} pm8x50_flexe_gen2_port_400g_an_state_t;

/*
 * Entries of the force speed ability table; each entry specifies a
 * unique FS port speed ability and its associated VCO rate
 */
typedef struct pm8x50_flexe_gen2_fs_ability_table_entry_s {
    uint32 speed; /* port speed in Mbps */
    uint32 num_lanes; /* number of lanes */
    portmod_fec_t fec_type; /* FEC type */
    portmod_vco_type_t vco; /* associated VCO rate of the ability */
} pm8x50_flexe_gen2_fs_ability_table_entry_t;

/*
 * Entries of the autoneg ability table; each entry specifies a unique
 * AN speed ability and its associated VCO rate
 */
typedef struct pm8x50_flexe_gen2_an_ability_table_entry_s {
    uint32 speed; /* port speed in Mbps */
    uint32 num_lanes; /* number of lanes */
    portmod_fec_t fec_type; /* FEC type */
    portmod_port_phy_control_autoneg_mode_t an_mode; /* autoneg mode such as cl73, bam or msa */
    portmod_vco_type_t vco; /* associated VCO rate of the ability */
} pm8x50_flexe_gen2_an_ability_table_entry_t;

typedef enum cdmac_port_mode_e{
    CDMAC_4_LANES_SEPARATE  = 0,
    CDMAC_3_TRI_0_1_2_2     = 1,
    CDMAC_3_TRI_0_0_2_3     = 2,
    CDMAC_2_LANES_DUAL      = 3,
    CDMAC_4_LANES_TOGETHER  = 4
} cdmac_port_mode_t;

#define CDMAC_NUM_LANES 4

/*!
 * @brief Flags of available VCOs
 */
#define PM8x50_FLEXE_GEN2_VCO_20P625G  0x1
#define PM8x50_FLEXE_GEN2_VCO_25P781G  0x2
#define PM8x50_FLEXE_GEN2_VCO_26P562G  0x4


#define PM8x50_FLEXE_GEN2_VCO_20P625G_SET(flags) ((flags) |= PM8x50_FLEXE_GEN2_VCO_20P625G)
#define PM8x50_FLEXE_GEN2_VCO_25P781G_SET(flags) ((flags) |= PM8x50_FLEXE_GEN2_VCO_25P781G)
#define PM8x50_FLEXE_GEN2_VCO_26P562G_SET(flags) ((flags) |= PM8x50_FLEXE_GEN2_VCO_26P562G)


#define PM8x50_FLEXE_GEN2_VCO_20P625G_CLR(flags) ((flags) &= ~PM8x50_FLEXE_GEN2_VCO_20P625G)
#define PM8x50_FLEXE_GEN2_VCO_25P781G_CLR(flags) ((flags) &= ~PM8x50_FLEXE_GEN2_VCO_25P781G)
#define PM8x50_FLEXE_GEN2_VCO_26P562G_CLR(flags) ((flags) &= ~PM8x50_FLEXE_GEN2_VCO_26P562G)

#define PM8x50_FLEXE_GEN2_VCO_20P625G_GET(flags) ((flags) & PM8x50_FLEXE_GEN2_VCO_20P625G ? 1 : 0)
#define PM8x50_FLEXE_GEN2_VCO_25P781G_GET(flags) ((flags) & PM8x50_FLEXE_GEN2_VCO_25P781G ? 1 : 0)
#define PM8x50_FLEXE_GEN2_VCO_26P562G_GET(flags) ((flags) & PM8x50_FLEXE_GEN2_VCO_26P562G ? 1 : 0)


/* a comprehensive list of pm8x50 gen2 forced speed abilities and their VCO rates */
const pm8x50_flexe_gen2_fs_ability_table_entry_t pm8x50_flexe_gen2_fs_ability_table[PM8x50_FLEXE_GEN2_FS_ABILITY_TABLE_SIZE] =
{
    /* port_speed, num_lanes, fec_type, vco */
    {10000,  1, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO25P781G},
    {10000,  1, PORTMOD_PORT_PHY_FEC_BASE_R,     portmodVCO25P781G},
    {12000,  1, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO25G    },
    {20000,  1, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO20P625G},
    {20000,  1, PORTMOD_PORT_PHY_FEC_BASE_R,     portmodVCO20P625G},
    {40000,  4, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO25P781G},
    {40000,  4, PORTMOD_PORT_PHY_FEC_BASE_R,     portmodVCO25P781G},
    {40000,  2, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO20P625G},
    {25000,  1, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO25P781G},
    {25000,  1, PORTMOD_PORT_PHY_FEC_BASE_R,     portmodVCO25P781G},
    {25000,  1, PORTMOD_PORT_PHY_FEC_RS_FEC,     portmodVCO25P781G},
    {50000,  1, PORTMOD_PORT_PHY_FEC_RS_FEC,     portmodVCO25P781G},
    {50000,  2, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO25P781G},
    {50000,  2, PORTMOD_PORT_PHY_FEC_RS_FEC,     portmodVCO25P781G},
    {100000, 2, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO25P781G},
    {100000, 2, PORTMOD_PORT_PHY_FEC_RS_FEC,     portmodVCO25P781G},
    {100000, 4, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO25P781G},
    {100000, 4, PORTMOD_PORT_PHY_FEC_RS_FEC,     portmodVCO25P781G},
    {200000, 4, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO25P781G},
    {50000,  1, PORTMOD_PORT_PHY_FEC_RS_544,     portmodVCO26P562G},
    {50000,  1, PORTMOD_PORT_PHY_FEC_RS_272,     portmodVCO26P562G},
    {50000,  2, PORTMOD_PORT_PHY_FEC_RS_544,     portmodVCO26P562G},
    {100000, 2, PORTMOD_PORT_PHY_FEC_RS_544,     portmodVCO26P562G},
    {100000, 2, PORTMOD_PORT_PHY_FEC_RS_272,     portmodVCO26P562G},
    {100000, 4, PORTMOD_PORT_PHY_FEC_RS_544,     portmodVCO26P562G},
    {200000, 4, PORTMOD_PORT_PHY_FEC_RS_272,     portmodVCO26P562G},
    {200000, 4, PORTMOD_PORT_PHY_FEC_RS_272_2XN, portmodVCO26P562G},
    {200000, 4, PORTMOD_PORT_PHY_FEC_RS_544,     portmodVCO26P562G},
    {200000, 4, PORTMOD_PORT_PHY_FEC_RS_544_2XN, portmodVCO26P562G},
    {400000, 8, PORTMOD_PORT_PHY_FEC_RS_272_2XN, portmodVCO26P562G},
    {400000, 8, PORTMOD_PORT_PHY_FEC_RS_544_2XN, portmodVCO26P562G}
};

/* a comprehensive list of pm8x50 gen2 autoneg abilities and their VCO rates */
const pm8x50_flexe_gen2_an_ability_table_entry_t pm8x50_flexe_gen2_an_ability_table[PM8x50_FLEXE_GEN2_AN_ABILITY_TABLE_SIZE] =
{
    /* port_speed, num_lanes, fec_type, autoneg_mode, vco */
    {10000,  1, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73    , portmodVCO25P781G},
    {10000,  1, PORTMOD_PORT_PHY_FEC_BASE_R,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73    , portmodVCO25P781G},
    {20000,  1, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO20P625G},
    {20000,  1, PORTMOD_PORT_PHY_FEC_BASE_R,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO20P625G},
    {40000,  2, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO20P625G},
    {40000,  4, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73    , portmodVCO25P781G},
    {40000,  4, PORTMOD_PORT_PHY_FEC_BASE_R,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73    , portmodVCO25P781G},
    {25000,  1, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73    , portmodVCO25P781G},
    {25000,  1, PORTMOD_PORT_PHY_FEC_BASE_R,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73    , portmodVCO25P781G},
    {25000,  1, PORTMOD_PORT_PHY_FEC_RS_FEC,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73    , portmodVCO25P781G},
    {25000,  1, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_MSA, portmodVCO25P781G},
    {25000,  1, PORTMOD_PORT_PHY_FEC_BASE_R,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_MSA, portmodVCO25P781G},
    {25000,  1, PORTMOD_PORT_PHY_FEC_RS_FEC,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_MSA, portmodVCO25P781G},
    {25000,  1, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO25P781G},
    {25000,  1, PORTMOD_PORT_PHY_FEC_BASE_R,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO25P781G},
    {25000,  1, PORTMOD_PORT_PHY_FEC_RS_FEC,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO25P781G},
    {50000,  1, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO25P781G},
    {50000,  1, PORTMOD_PORT_PHY_FEC_RS_FEC,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO25P781G},
    {50000,  2, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_MSA, portmodVCO25P781G},
    {50000,  2, PORTMOD_PORT_PHY_FEC_RS_FEC,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_MSA, portmodVCO25P781G},
    {50000,  2, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO25P781G},
    {50000,  2, PORTMOD_PORT_PHY_FEC_RS_FEC,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO25P781G},
    {100000, 2, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO25P781G},
    {100000, 2, PORTMOD_PORT_PHY_FEC_RS_FEC,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO25P781G},
    {100000, 4, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO25P781G},
    {100000, 4, PORTMOD_PORT_PHY_FEC_RS_FEC,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73    , portmodVCO25P781G},
    {200000, 4, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO25P781G},
    {50000,  1, PORTMOD_PORT_PHY_FEC_RS_544,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73    , portmodVCO26P562G},
    {50000,  1, PORTMOD_PORT_PHY_FEC_RS_272,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_MSA, portmodVCO26P562G},
    {50000,  2, PORTMOD_PORT_PHY_FEC_RS_544,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO26P562G},
    {100000, 2, PORTMOD_PORT_PHY_FEC_RS_544,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73    , portmodVCO26P562G},
    {100000, 2, PORTMOD_PORT_PHY_FEC_RS_272,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_MSA, portmodVCO26P562G},
    {100000, 4, PORTMOD_PORT_PHY_FEC_RS_544,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO26P562G},
    {200000, 4, PORTMOD_PORT_PHY_FEC_RS_544_2XN, PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73    , portmodVCO26P562G},
    {200000, 4, PORTMOD_PORT_PHY_FEC_RS_272_2XN, PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_MSA, portmodVCO26P562G},
    {200000, 4, PORTMOD_PORT_PHY_FEC_RS_544,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO26P562G},
    {400000, 8, PORTMOD_PORT_PHY_FEC_RS_544_2XN, PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_MSA, portmodVCO26P562G}
};

static phymod_dispatch_type_t pm8x50_flexe_gen2_serdes_list[] =
{
#ifdef PHYMOD_TSCBH_FE_GEN2_SUPPORT
    phymodDispatchTypeTscbh_fe_gen2,
#endif
    phymodDispatchTypeInvalid
};

/* VCOs bitmap type */
typedef uint16_t pm8x50_flexe_gen2_vcos_bmp_t;

#define PM8X50_FLEXE_GEN2_LANES_PER_CORE              (8)
#define PM8X50_FLEXE_GEN2_PORTS_PER_CORE              (8)
#define PM8X50_FLEXE_GEN2_MAX_NUM_PHYS                (1)
#define PMD_INFO_DATA_STRUCTURE_SIZE                  (128)     /* in bytes */
#define MAX_NUM_PMD_INS                               (4)
#define MAX_NUM_LANES_PER_PMD_INS                     (2)

/* Translate p2l or l2p lane map array*/
#define PM8x50_FLEXE_GEN2_LANE_MAP_TRANSLATE(lane_map_src, lane_map_des) \
    do {                                                                   \
        int temp_lane_index_i;                                               \
        for (temp_lane_index_i = 0; temp_lane_index_i < PM8X50_FLEXE_GEN2_LANES_PER_CORE; temp_lane_index_i++) { \
            lane_map_des[lane_map_src[temp_lane_index_i]] = temp_lane_index_i;                             \
        }                                                                  \
    } while(0)

/* Translate logical lane mask to physical lane mask*/
#define PM8x50_FLEXE_GEN2_PORT_PHYISCAL_LANE_MASK_GET(pm_info, port_lane_mask, port_physical_lane_mask) \
        do {                                                                                            \
            int temp_lane_index_ii;                                                                     \
            uint32 *temp_lane_map_tx = NULL;                                                            \
            temp_lane_map_tx = PM_8x50_FLEXE_GEN2_INFO(pm_info)->lane_map.lane_map_tx;                  \
            for (temp_lane_index_ii = 0; temp_lane_index_ii < PM8X50_FLEXE_GEN2_LANES_PER_CORE; temp_lane_index_ii++) {  \
                if (port_lane_mask & (0x1 << temp_lane_index_ii)) {                                                      \
                    port_physical_lane_mask |= 0x1 << temp_lane_map_tx[temp_lane_index_ii];             \
                }                                                                                       \
            }                                                                                           \
        } while(0)

struct pm8x50_flexe_gen2_s{
    portmod_pbmp_t phys;
    int first_phy;
    phymod_ref_clk_t ref_clk;
    phymod_polarity_t polarity;
    phymod_lane_map_t lane_map;
    phymod_firmware_load_method_t fw_load_method;
    phymod_firmware_loader_f external_fw_loader;
    phymod_core_access_t int_core_access;
    uint8 core_num;
    portmod_mac_soft_reset_f portmod_mac_soft_reset;
    phymod_afe_pll_t afe_pll;
    int warmboot_skip_db_restore;
    int is_master_pm;
    portmod_egress_buffer_reset_f portmod_egress_buffer_reset;
    void* pmd_info;
    /* The delay from CMIC to PCS in nanoseconds. */
    uint32 pm_offset;
};

/* portmod tsc interrupts type */
typedef enum pm8x50_flexe_gen2_tsc_intr_type_e {
    pm8x50_flexe_gen2TscIntrEccNone,
    pm8x50_flexe_gen2TscIntrEcc1bErr,
    pm8x50_flexe_gen2TscIntrEcc1bErrRx1588400g = pm8x50_flexe_gen2TscIntrEcc1bErr,
    pm8x50_flexe_gen2TscIntrEcc1bErrRx1588Mpp1,
    pm8x50_flexe_gen2TscIntrEcc1bErrRx1588Mpp0,
    pm8x50_flexe_gen2TscIntrEcc1bErrTx1588400g,
    pm8x50_flexe_gen2TscIntrEcc1bErrTx1588Mpp1,
    pm8x50_flexe_gen2TscIntrEcc1bErrTx1588Mpp0,
    pm8x50_flexe_gen2TscIntrEcc1bErrUmTable,
    pm8x50_flexe_gen2TscIntrEcc1bErrAmTable,
    pm8x50_flexe_gen2TscIntrEcc1bErrSpeedTable,
    pm8x50_flexe_gen2TscIntrEcc1bErrDeskew,
    pm8x50_flexe_gen2TscIntrEcc1bErrRsfecRs400gMpp1,
    pm8x50_flexe_gen2TscIntrEcc1bErrRsfecRs400gMpp0,
    pm8x50_flexe_gen2TscIntrEcc1bErrRsfecRbufMpp1,
    pm8x50_flexe_gen2TscIntrEcc1bErrRsfecRbufMpp0,
    pm8x50_flexe_gen2TscIntrEcc1bErrBaseRFec,
    pm8x50_flexe_gen2TscIntrEcc1bErrCount = pm8x50_flexe_gen2TscIntrEcc1bErrBaseRFec,
    pm8x50_flexe_gen2TscIntrEcc2bErr,
    pm8x50_flexe_gen2TscIntrEcc2bErrRx1588400g = pm8x50_flexe_gen2TscIntrEcc2bErr,
    pm8x50_flexe_gen2TscIntrEcc2bErrRx1588Mpp1,
    pm8x50_flexe_gen2TscIntrEcc2bErrRx1588Mpp0,
    pm8x50_flexe_gen2TscIntrEcc2bErrTx1588400g,
    pm8x50_flexe_gen2TscIntrEcc2bErrTx1588Mpp1,
    pm8x50_flexe_gen2TscIntrEcc2bErrTx1588Mpp0,
    pm8x50_flexe_gen2TscIntrEcc2bErrUmTable,
    pm8x50_flexe_gen2TscIntrEcc2bErrAmTable,
    pm8x50_flexe_gen2TscIntrEcc2bErrSpdTable,
    pm8x50_flexe_gen2TscIntrEcc2bErrDeskew,
    pm8x50_flexe_gen2TscIntrEcc2bErrRsfecRs400gMpp1,
    pm8x50_flexe_gen2TscIntrEcc2bErrRsfecRs400gMpp0,
    pm8x50_flexe_gen2TscIntrEcc2bErrRsfecRbufMpp1,
    pm8x50_flexe_gen2TscIntrEcc2bErrRsfecRbufMpp0,
    pm8x50_flexe_gen2TscIntrEcc2bErrBaseRFec,
    pm8x50_flexe_gen2TscIntrEcc2bErrCount = pm8x50_flexe_gen2TscIntrEcc2bErrBaseRFec,
    pm8x50_flexe_gen2TscIntrEccCount
}pm8x50_flexe_gen2_tsc_intr_type_t;


/*
 * Macros to indicate the interrupt types(1-bit or 2-bit)
 * based on the register fields of the Portmacro.
 * CDPORT_TSC_INTR_STATUS,
 * TSC_ECC_1b_ERR - 15-bit field.
 * TSC_ECC_2b_ERR - 15-bit field.
 */
#define PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR     0x1
#define PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR     0x2


/*
 * Each bit position in the register fields
 * TSC_ECC_2b_ERR, TSC_ECC_2b_ERR indicate
 * a TSC memory as under.
 * TSC_ECC_1b_ERR - 15-bit field.
 * TSC_ECC_2b_ERR - 15-bit field.
 *
 * bit 14: err_intr_en_rx_1588_400g
 * bit 13: err_intr_en_rx_1588_mpp1
 * bit 12: err_intr_en_rx_1588_mpp0
 * bit 11: err_intr_en_tx_1588_400g
 * bit 10: err_intr_en_tx_1588_mpp1
 * bit  9: err_intr_en_tx_1588_mpp0
 * bit  8: err_intr_en_um_tbl
 * bit  7: err_intr_en_am_tbl
 * bit  6: err_intr_en_spd_tbl
 * bit  5: err_intr_en_deskew
 * bit  4: err_intr_en_rsfec_rs400g_mpp1
 * bit  3: err_intr_en_rsfec_rs400g_mpp0
 * bit  2: err_intr_en_rsfec_rbuf_mpp1
 * bit  1: err_intr_en_rsfec_rbuf_mpp0
 * bit  0: err_intr_en_base_r_fec
 */
/* Macros for 1-bit errors. */
#define PM8x50_FLEXE_GEN2_TSC_ECC_INTR_NONE                   0x0
#define PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_RX_1588_400G        (1 << 14)
#define PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_RX_1588_MPP1        (1 << 13)
#define PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_RX_1588_MPP0        (1 << 12)
#define PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_TX_1588_400G        (1 << 11)
#define PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_TX_1588_MPP1        (1 << 10)
#define PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_TX_1588_MPP0        (1 << 9)
#define PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_UM_TABLE            (1 << 8)
#define PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_AM_TABLE            (1 << 7)
#define PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_SPEED_TABLE         (1 << 6)
#define PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_DESKEW              (1 << 5)
#define PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_RSFEC_400G_MPP1     (1 << 4)
#define PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_RSFEC_400G_MPP0     (1 << 3)
#define PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_RSFEC_RBUF_MPP1     (1 << 2)
#define PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_RSFEC_RBUF_MPP0     (1 << 1)
#define PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_BASE_R_FEC          (1 << 0)

/* Macros for 2-bit errors. */
#define PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_RX_1588_400G        (1 << 14)
#define PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_RX_1588_MPP1        (1 << 13)
#define PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_RX_1588_MPP0        (1 << 12)
#define PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_TX_1588_400G        (1 << 11)
#define PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_TX_1588_MPP1        (1 << 10)
#define PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_TX_1588_MPP0        (1 << 9)
#define PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_UM_TABLE            (1 << 8)
#define PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_AM_TABLE            (1 << 7)
#define PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_SPEED_TABLE         (1 << 6)
#define PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_DESKEW              (1 << 5)
#define PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_RSFEC_400G_MPP1     (1 << 4)
#define PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_RSFEC_400G_MPP0     (1 << 3)
#define PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_RSFEC_RBUF_MPP1     (1 << 2)
#define PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_RSFEC_RBUF_MPP0     (1 << 1)
#define PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_BASE_R_FEC          (1 << 0)


typedef struct pm8x50_flexe_gen2_tsc_err_intr_phymod_map_e{
    uint32  flags;
    phymod_interrupt_type_t phymod_intr_map;
    char    *str;
}pm8x50_flexe_gen2_tsc_err_intr_phymod_map_t;

pm8x50_flexe_gen2_tsc_err_intr_phymod_map_t
         pm8x50_flexe_gen2_tsc_ecc_intr_info [pm8x50_flexe_gen2TscIntrEccCount] = {
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_INTR_NONE,
        phymodIntrNone,
        "Interrupt none"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_RX_1588_400G,
        phymodIntrEccRx1588400g,
        "1-bit RX 1588 400G ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_RX_1588_MPP1,
        phymodIntrEccRx1588Mpp1,
        "1-bit RX 1588 Mpp1 ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_RX_1588_MPP0,
        phymodIntrEccRx1588Mpp0,
        "1-bit RX 1588 Mpp0 ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_TX_1588_400G,
        phymodIntrEccTx1588400g,
        "1-bit TX 1588 400G ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_TX_1588_MPP1,
        phymodIntrEccTx1588Mpp1,
        "1-bit TX 1588 Mpp1 ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_TX_1588_MPP0,
        phymodIntrEccTx1588Mpp0,
        "1-bit TX 1588 Mpp0 ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_UM_TABLE,
        phymodIntrEccUMTable,
        "1-bit Unique Marker table ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_AM_TABLE,
        phymodIntrEccAMTable,
        "1-bit Alignment Marker table ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_SPEED_TABLE,
        phymodIntrEccSpeedTable,
        "1-bit Speed table ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_DESKEW,
        phymodIntrEccDeskew,
        "1-bit Deskew ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_RSFEC_400G_MPP1,
        phymodIntrEccRsFECRs400gMpp1,
        "1-bit RsFEC_Rs400g Mpp1 ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_RSFEC_400G_MPP0,
        phymodIntrEccRsFECRs400gMpp0,
        "1-bit RsFEC_Rs400g Mpp1 ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_RSFEC_RBUF_MPP1,
        phymodIntrEccRsFECRbufMpp1,
        "1-bit RsFEC_RBUF MPP1 ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_RSFEC_RBUF_MPP0,
        phymodIntrEccRsFECRbufMpp0,
        "1-bit RsFEC_RBUF MPP0 ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR_BASE_R_FEC,
        phymodIntrEccBaseRFEC,
        "1-bit BaseR FEC ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_RX_1588_400G,
        phymodIntrEccRx1588400g,
        "2-bit RX 1588 400G ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_RX_1588_MPP1,
        phymodIntrEccRx1588Mpp1,
        "2-bit RX 1588 Mpp1 ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_RX_1588_MPP0,
        phymodIntrEccRx1588Mpp0,
        "2-bit RX 1588 Mpp0 ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_TX_1588_400G,
        phymodIntrEccTx1588400g,
        "2-bit TX 1588 400G ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_TX_1588_MPP1,
        phymodIntrEccTx1588Mpp1,
        "2-bit TX 1588 Mpp1 ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_TX_1588_MPP0,
        phymodIntrEccTx1588Mpp0,
        "2-bit TX 1588 Mpp0 ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_UM_TABLE,
        phymodIntrEccUMTable,
        "2-bit Unique Marker table ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_AM_TABLE,
        phymodIntrEccAMTable,
        "2-bit Alignment Marker table ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_SPEED_TABLE,
        phymodIntrEccSpeedTable,
        "2-bit Speed table ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_DESKEW,
        phymodIntrEccDeskew,
        "2-bit Deskew ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_RSFEC_400G_MPP1,
        phymodIntrEccRsFECRs400gMpp1,
        "2-bit RsFEC_Rs400g Mpp1 ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_RSFEC_400G_MPP0,
        phymodIntrEccRsFECRs400gMpp0,
        "2-bit RsFEC_Rs400g Mpp1 ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_RSFEC_RBUF_MPP1,
        phymodIntrEccRsFECRbufMpp1,
        "2-bit RsFEC_RBUF MPP1 ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_RSFEC_RBUF_MPP0,
        phymodIntrEccRsFECRbufMpp0,
        "2-bit RsFEC_RBUF MPP0 ecc error"
    },
    {
        PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR_BASE_R_FEC,
        phymodIntrEccBaseRFEC,
        "2-bit BaseR FEC ecc error"
     }
};

portmod_intr_reg_info_t pm8x50_flexe_gen2_mac_intr_info[] = {
    {
        PORTMOD_INTR_STATUS_COR | PORTMOD_INTR_BLOCK_CLEAR_REG |
        PORTMOD_INTR_ECC_1B_ERR,
        portmodIntrTypeMibMemSingleBitErr,
        CDMAC_INTR_ENABLEr, MIB_COUNTER_SINGLE_BIT_ERRf,
        CDMAC_INTR_STATUSr, MIB_COUNTER_SINGLE_BIT_ERRf,
        CDMAC_ECC_STATUSr, MIB_COUNTER_SINGLE_BIT_ERRf,
        "CDMAC MIB single bit error"
    },
    {
        PORTMOD_INTR_STATUS_COR | PORTMOD_INTR_BLOCK_CLEAR_REG |
        PORTMOD_INTR_ECC_2B_ERR,
        portmodIntrTypeMibMemDoubleBitErr,
        CDMAC_INTR_ENABLEr, MIB_COUNTER_DOUBLE_BIT_ERRf,
        CDMAC_INTR_STATUSr, MIB_COUNTER_DOUBLE_BIT_ERRf,
        CDMAC_ECC_STATUSr, MIB_COUNTER_DOUBLE_BIT_ERRf,
        "CDMAC MIB double bit error"
    },
    {
        PORTMOD_INTR_STATUS_COR | PORTMOD_INTR_BLOCK_CLEAR_REG |
        PORTMOD_INTR_ECC_MULTIB_ERR,
        portmodIntrTypeMibMemMultipleBitErr,
        CDMAC_INTR_ENABLEr, MIB_COUNTER_MULTIPLE_ERRf,
        CDMAC_INTR_STATUSr, MIB_COUNTER_MULTIPLE_ERRf,
        CDMAC_ECC_STATUSr, MIB_COUNTER_MULTIPLE_ERRf,
        "CDMAC MIB Multiple bit error"
    },
    {
        PORTMOD_INTR_STATUS_COR | PORTMOD_INTR_BLOCK_CLEAR_REG |
        PORTMOD_INTR_ECC_1B_ERR,
        portmodIntrTypeTxCdcSingleBitErr,
        CDMAC_INTR_ENABLEr, TX_CDC_SINGLE_BIT_ERRf,
        CDMAC_INTR_STATUSr, TX_CDC_SINGLE_BIT_ERRf,
        CDMAC_ECC_STATUSr, TX_CDC_SINGLE_BIT_ERRf,
        "CDMAC TX CDC single bit error"
    },
    {
        PORTMOD_INTR_STATUS_COR | PORTMOD_INTR_BLOCK_CLEAR_REG |
        PORTMOD_INTR_ECC_1B_ERR,
        portmodIntrTypeTxCdcDoubleBitErr,
        CDMAC_INTR_ENABLEr, TX_CDC_DOUBLE_BIT_ERRf,
        CDMAC_INTR_STATUSr, TX_CDC_DOUBLE_BIT_ERRf,
        CDMAC_ECC_STATUSr, TX_CDC_DOUBLE_BIT_ERRf,
        "CDMAC TX CDC double bit error"
    },
    {
        PORTMOD_INTR_STATUS_COR | PORTMOD_INTR_BLOCK_CLEAR_REG,
        portmodIntrTypeTxPktUnderflow,
        CDMAC_INTR_ENABLEr, TX_PKT_UNDERFLOWf,
        CDMAC_INTR_STATUSr, TX_PKT_UNDERFLOWf,
        CDMAC_INTR_STATUSr, TX_PKT_UNDERFLOWf,
        "CDMAC TX packet underflow interrupt"
    },
    {
        PORTMOD_INTR_STATUS_COR | PORTMOD_INTR_BLOCK_CLEAR_REG,
        portmodIntrTypeTxPktOverflow,
        CDMAC_INTR_ENABLEr, TX_PKT_OVERFLOWf,
        CDMAC_INTR_STATUSr, TX_PKT_OVERFLOWf,
        CDMAC_INTR_STATUSr, TX_PKT_OVERFLOWf,
        "CDMAC TX packet overflow interrupt"
    },
    {
        0x0, /* No flags */
        portmodIntrTypeCount,
        INVALIDr, INVALIDf,
        INVALIDr, INVALIDf,
        INVALIDr, INVALIDf,
        " "
    }
};

portmod_intr_reg_info_t pm8x50_flexe_gen2_tsc_core_intr_info[] = {
    {
        PORTMOD_INTR_STATUS_W1TC | PORTMOD_INTR_STATUS_BIT_FLDS |
        PORTMOD_INTR_ECC_2B_ERR,
        portmodIntrTypeTscCore0Err,
        INVALIDr, INVALIDf,
        CDPORT_TSC_INTR_STATUSr, TSC_ECC_2B_ERRf,
        INVALIDr, INVALIDf,
        " "
    },
    {
        PORTMOD_INTR_STATUS_W1TC | PORTMOD_INTR_STATUS_BIT_FLDS |
        PORTMOD_INTR_ECC_1B_ERR,
        portmodIntrTypeTscCore0Err,
        INVALIDr, INVALIDf,
        CDPORT_TSC_INTR_STATUSr, TSC_ECC_1B_ERRf,
        INVALIDr, INVALIDf,
        " "
    }
};

portmod_pm_intr_info_t pm8x50_flexe_gen2_pm_intr_info[] = {
    {
        portmodIntrTypeMacErr, CDPORT_INTR_ENABLEr, MAC_ERRf,
        1, pm8x50_flexe_gen2_mac_intr_info,
        CDPORT_INTR_STATUSr, MAC_ERRf,
        1, pm8x50_flexe_gen2_mac_intr_info,
        INVALIDr, INVALIDf,
        0, NULL,
        INVALIDm, "PM MAC error interrupt"
    },
    {
        portmodIntrTypeTscCore0Err, CDPORT_INTR_ENABLEr, TSC_CORE0_ERRf,
        1, pm8x50_flexe_gen2_tsc_core_intr_info,
        CDPORT_INTR_STATUSr, TSC_CORE0_ERRf,
        1, pm8x50_flexe_gen2_tsc_core_intr_info,
        INVALIDr, INVALIDf,
        0, NULL,
        INVALIDm, "PM TSC Core0 error interrupt"
    },
    {
        portmodIntrTypeTscCore1Err, CDPORT_INTR_ENABLEr, TSC_CORE1_ERRf,
        1, pm8x50_flexe_gen2_tsc_core_intr_info,
        CDPORT_INTR_STATUSr, TSC_CORE1_ERRf,
        1, pm8x50_flexe_gen2_tsc_core_intr_info,
        INVALIDr, INVALIDf,
        0, NULL,
        INVALIDm, "PM TSC Core1 error interrupt"
    },
    {
        portmodIntrTypeFcReqFull, CDPORT_INTR_ENABLEr, FLOWCONTROL_REQ_FULLf,
        0, NULL,
        CDPORT_INTR_STATUSr, FLOWCONTROL_REQ_FULLf,
        0, NULL,
        INVALIDr, INVALIDf,
        0, NULL,
        INVALIDm, "PM Flow cotrol request interrupt"
    },
    {
        portmodIntrTypePmdErr, CDPORT_INTR_ENABLEr, PMD_ERRf,
        0, NULL,
        CDPORT_INTR_STATUSr, PMD_ERRf,
        0, NULL,
        INVALIDr, INVALIDf,
        0, NULL,
        INVALIDm, "PM PMD error interrupt"
    }
};

/*
 * Get the VCO rates from given PCS bypassed port speed
 */

STATIC
int _pm8x50_flexe_gen2_shared_pcs_bypassed_vco_get(int speed, portmod_vco_type_t* vco)
{
    SOC_INIT_FUNC_DEFS;

    switch (speed) {
        case 56250:
        case 28125:
            *vco = portmodVCO28P125G;
            break;
        case 26562:
        case 53125:
            *vco = portmodVCO26P562G;
            break;
        case 27343:
            *vco = portmodVCO27P343G;
            break;
        case 25781:
        case 10312:
            *vco = portmodVCO25P781G;
            break;
        case 25000:
            *vco = portmodVCO25G;
            break;
        case 12500:
            *vco = portmodVCO25G;
            break;
        case 20625:
            *vco = portmodVCO20P625G;
            break;
        case 10709:
            *vco = portmodVCO26P773G;
            break;
        case 11049:
            *vco = portmodVCO27P622G;
            break;
        case 11095:
            *vco = portmodVCO27P739G;
            break;
        case 27952:
        case 55904:
            *vco = portmodVCO27P952G;
            break;
        case 28076:
        case 56152:
            *vco = portmodVCO28P076G;
            break;
        default:
            *vco = portmodVCOInvalid;
            break;
    }

    SOC_FUNC_RETURN;
}


/*
 * given the port speed, number of lanes, and FEC type, return the VCO specified in
 * pm8x50_flexe_gen2_fs_ability_table in the associated entry; VCO should be portmodVCOInvalid
 * if the ability is invalid
 */
STATIC
int _pm8x50_flexe_gen2_fs_ability_table_vco_get(uint32 speed, uint32 num_lanes, portmod_fec_t fec_type,
                                                  portmod_vco_type_t* vco)
{
    int i;

    *vco = portmodVCOInvalid;
    for (i = 0; i < PM8x50_FLEXE_GEN2_FS_ABILITY_TABLE_SIZE; i++) {
        if (pm8x50_flexe_gen2_fs_ability_table[i].speed == speed &&
            pm8x50_flexe_gen2_fs_ability_table[i].num_lanes == num_lanes &&
            pm8x50_flexe_gen2_fs_ability_table[i].fec_type == fec_type)
        {
            *vco = pm8x50_flexe_gen2_fs_ability_table[i].vco;
            break;
        }
    }

    return SOC_E_NONE;
}

/*
 * given the port speed, number of lanes, FEC type, and autoneg mode, return the VCO specified in
 * pm8x50_flexe_gen2_an_ability_table; VCO should be portmodVCOInvalid if the ability is invalid.
 * this function is temporarily masked out because there is no caller. advert_ability_get() will
 * need to call this function to verify the VCO requirement
 */
STATIC
int _pm8x50_flexe_gen2_an_ability_table_vco_get(uint32 speed, uint32 num_lanes, portmod_fec_t fec_type,
                                          portmod_port_phy_control_autoneg_mode_t an_mode,
                                          portmod_vco_type_t* vco)
{
    int i;

    *vco = portmodVCOInvalid;
    for (i = 0; i < PM8x50_FLEXE_GEN2_AN_ABILITY_TABLE_SIZE; i++) {
        if (pm8x50_flexe_gen2_an_ability_table[i].speed == speed &&
            pm8x50_flexe_gen2_an_ability_table[i].num_lanes == num_lanes &&
            pm8x50_flexe_gen2_an_ability_table[i].fec_type == fec_type &&
            pm8x50_flexe_gen2_an_ability_table[i].an_mode == an_mode)
        {
            *vco = pm8x50_flexe_gen2_an_ability_table[i].vco;
            break;
        }
    }

    return SOC_E_NONE;
}

/* read an entry of pm8x50_flexe_gen2_fs_ability_table[] */
STATIC
int _pm8x50_flexe_gen2_fs_ability_table_read_entry(int index, pm8x50_flexe_gen2_fs_ability_table_entry_t *table_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (index < 0 || index >= PM8x50_FLEXE_GEN2_FS_ABILITY_TABLE_SIZE) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("index to read in pm8x50_flexe_gen2_fs_ability_table is out of boundary"));
    }

    *table_entry = pm8x50_flexe_gen2_fs_ability_table[index];

exit:
    SOC_FUNC_RETURN;
}

/* read an entry of pm8x50_flexe_gen2_an_ability_table[] */
STATIC
int _pm8x50_flexe_gen2_an_ability_table_read_entry(int index, pm8x50_flexe_gen2_an_ability_table_entry_t *table_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (index < 0 || index >= PM8x50_FLEXE_GEN2_AN_ABILITY_TABLE_SIZE) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("index to read in pm8x50_flexe_gen2_an_ability_table is out of boundary"));
    }

    *table_entry = pm8x50_flexe_gen2_an_ability_table[index];

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm8x50_flexe_gen2_port_index_get(int unit, int port, pm_info_t pm_info,
                                   int *first_index, uint32 *bitmap)
{
   int i, rv = 0, tmp_port = 0;

   SOC_INIT_FUNC_DEFS;

   *first_index = -1;
   *bitmap = 0;

   for( i = 0 ; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++){
       rv = PM8x50_FLEXE_GEN2_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
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
_pm8x50_flexe_gen2_phy_access_get(
            int unit,
            int port,
            pm_info_t pm_info,
            int *phy_acc)
{
    uint32 is_bypassed = 0;
    int port_index;
    uint32 bitmap[1];

    SOC_INIT_FUNC_DEFS;

    *phy_acc = 0;
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));
    if (is_bypassed) {
        *phy_acc = (PM_8x50_FLEXE_GEN2_INFO(pm_info)->first_phy + port_index) | SOC_REG_ADDR_PHY_ACC_MASK;
    } else {
        *phy_acc = port;
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm8x50_flexe_gen2_port_interrupt_all_enable_set(int unit, int port, int enable)
{
    int i = 0;
    portmod_intr_type_t intrs[] = {
                                    portmodIntrTypeMacErr,
                                    portmodIntrTypeTscCore0Err,
                                    portmodIntrTypeTscCore1Err,
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
int _pm8x50_flexe_gen2_pm_port_mode_update(int unit,
                                        int port,
                                        pm_info_t pm_info,
                                        int first_phy_index,
                                        int num_lanes)
{
    uint32 reg_val, mac_port_mode, mac_new_port_mode = 0, is_400g_port = 0;
    int phy_acc, first_lane_local = 0, mac_stage_id = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_phy_access_get(unit, port, pm_info, &phy_acc));

    mac_stage_id = first_phy_index / CDMAC_NUM_LANES;
    first_lane_local = first_phy_index % CDMAC_NUM_LANES;

    _SOC_IF_ERR_EXIT(READ_CDPORT_MODE_REGr(unit, phy_acc, &reg_val));
    is_400g_port = soc_reg_field_get(unit, CDPORT_MODE_REGr, reg_val, SINGLE_PORT_MODE_SPEED_400Gf);

    if (mac_stage_id) {
        /* MAC1 need to update port mode */
        mac_port_mode = soc_reg_field_get(unit, CDPORT_MODE_REGr, reg_val, MAC1_PORT_MODEf);
    } else {
        /* MAC0 need to update port mode */
        mac_port_mode = soc_reg_field_get(unit, CDPORT_MODE_REGr, reg_val, MAC0_PORT_MODEf);
    }

    if (num_lanes == 8) {
        /* 400G single port mode */
        soc_reg_field_set(unit, CDPORT_MODE_REGr, &reg_val, SINGLE_PORT_MODE_SPEED_400Gf, 1);
        soc_reg_field_set(unit, CDPORT_MODE_REGr, &reg_val, MAC1_PORT_MODEf, CDMAC_4_LANES_TOGETHER);
        soc_reg_field_set(unit, CDPORT_MODE_REGr, &reg_val, MAC0_PORT_MODEf, CDMAC_4_LANES_TOGETHER);
    } else if (num_lanes == 4) {
        soc_reg_field_set(unit, CDPORT_MODE_REGr, &reg_val, SINGLE_PORT_MODE_SPEED_400Gf, 0);
        if (mac_stage_id) {
            /* MAC1 is single-port mode */
            soc_reg_field_set(unit, CDPORT_MODE_REGr, &reg_val, MAC1_PORT_MODEf, CDMAC_4_LANES_TOGETHER);
        } else {
            /* MAC0 is single-port mode */
            soc_reg_field_set(unit, CDPORT_MODE_REGr, &reg_val, MAC0_PORT_MODEf, CDMAC_4_LANES_TOGETHER);
        }
    } else if (num_lanes == 2) {
        soc_reg_field_set(unit, CDPORT_MODE_REGr, &reg_val, SINGLE_PORT_MODE_SPEED_400Gf, 0);
        switch (mac_port_mode) {
            case CDMAC_4_LANES_SEPARATE:
                if (first_lane_local == 0) {
                    mac_new_port_mode = CDMAC_3_TRI_0_0_2_3;
                } else{
                    mac_new_port_mode = CDMAC_3_TRI_0_1_2_2;
                }
                break;
            case CDMAC_3_TRI_0_1_2_2:
                if (first_lane_local == 0) {
                    mac_new_port_mode = CDMAC_2_LANES_DUAL;
                } else {
                    mac_new_port_mode = CDMAC_3_TRI_0_1_2_2;
                }
                break;
            case CDMAC_3_TRI_0_0_2_3:
                if (first_lane_local == 0) {
                    mac_new_port_mode = CDMAC_3_TRI_0_0_2_3;
                } else {
                    mac_new_port_mode = CDMAC_2_LANES_DUAL;
                }
                break;
            case CDMAC_2_LANES_DUAL:
                mac_new_port_mode = CDMAC_2_LANES_DUAL;
                break;
            case CDMAC_4_LANES_TOGETHER:
                mac_new_port_mode = CDMAC_2_LANES_DUAL;
                break;
            default:
                break;
        }
        if (mac_stage_id) {
            soc_reg_field_set(unit, CDPORT_MODE_REGr, &reg_val, MAC1_PORT_MODEf, mac_new_port_mode);
        } else {
            soc_reg_field_set(unit, CDPORT_MODE_REGr, &reg_val, MAC0_PORT_MODEf, mac_new_port_mode);
        }
    } else if (num_lanes == 1) {
        soc_reg_field_set(unit, CDPORT_MODE_REGr, &reg_val, SINGLE_PORT_MODE_SPEED_400Gf, 0);
        switch (mac_port_mode) {
            case CDMAC_4_LANES_SEPARATE:
                mac_new_port_mode = CDMAC_4_LANES_SEPARATE;
                break;
            case CDMAC_3_TRI_0_1_2_2:
                if ((first_lane_local == 0) || (first_lane_local == 1)) {
                    mac_new_port_mode = CDMAC_3_TRI_0_1_2_2;
                } else if ((first_lane_local == 2) || (first_lane_local == 3)) {
                    mac_new_port_mode = CDMAC_4_LANES_SEPARATE;
                }
                break;
            case CDMAC_3_TRI_0_0_2_3:
                if ((first_lane_local == 0) || (first_lane_local == 1)) {
                    mac_new_port_mode = CDMAC_4_LANES_SEPARATE;
                } else if ((first_lane_local == 2) || (first_lane_local == 3)) {
                    mac_new_port_mode = CDMAC_3_TRI_0_0_2_3;
                }
                break;
            case CDMAC_2_LANES_DUAL:
                if ((first_lane_local == 0) || (first_lane_local == 1)) {
                    mac_new_port_mode = CDMAC_3_TRI_0_1_2_2;
                } else if ((first_lane_local == 2) || (first_lane_local == 3)){
                    mac_new_port_mode = CDMAC_3_TRI_0_0_2_3;
                }
                break;
            case CDMAC_4_LANES_TOGETHER:
                mac_new_port_mode = CDMAC_4_LANES_SEPARATE;
                break;
            default:
                break;
        }
        if (mac_stage_id) {
            soc_reg_field_set(unit, CDPORT_MODE_REGr, &reg_val, MAC1_PORT_MODEf, mac_new_port_mode);
        } else {
            soc_reg_field_set(unit, CDPORT_MODE_REGr, &reg_val, MAC0_PORT_MODEf, mac_new_port_mode);
            /* if current port is 400G mode, need to write cdmac1 as well */
            if (is_400g_port) {
                soc_reg_field_set(unit, CDPORT_MODE_REGr, &reg_val, MAC1_PORT_MODEf, mac_new_port_mode);
            }
        }
    }
    _SOC_IF_ERR_EXIT(WRITE_CDPORT_MODE_REGr(unit, phy_acc, reg_val));

exit:
    SOC_FUNC_RETURN;

}


STATIC
int _pm8x50_flexe_gen2_400g_an_workaround(int unit, int port, pm_info_t pm_info, int* is_done)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    uint32_t serdes_reg_val, wait_link_count, base_reg_val, remote_pause;
    int nof_phys;
    pm8x50_flexe_gen2_port_400g_an_state_t an_state;
    phymod_autoneg_control_t an_control;
    char* state_string = "\n";
    int port_index;
    uint32 bitmap;

    SOC_INIT_FUNC_DEFS;

    *is_done = 0;

    /* first get the an state and wait link count */
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_STATE_GET(unit, pm_info, &an_state));
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_WAIT_LINK_COUNT_GET(unit, pm_info, &wait_link_count));

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    /* based on the AN state */
    switch (an_state) {
        case PM8X50_FLEXE_GEN2_PORT_AN_CHECK_PAGE_COMPLETE:
        {
            state_string = "AN_CHECK_PAGE_COMPLETE\n";
            /* first read SW autoneg status register 0xc1e6 */
            /* clear the status first*/
            _SOC_IF_ERR_EXIT(phymod_phy_reg_read(&phy_access, AN_X4_SW_CTRL_STS_REG, &serdes_reg_val));
            sal_usleep(100);
            _SOC_IF_ERR_EXIT(phymod_phy_reg_read(&phy_access, AN_X4_SW_CTRL_STS_REG, &serdes_reg_val));

            /* need to check if re-start AN is needed */
            wait_link_count++;
            /* next check if AN needs to restarted */
            if (wait_link_count > TIMEOUT_COUNT_400G_AN) {
                wait_link_count = 0;
                /* needs to disable */
                an_control.enable = 0;
                an_control.an_mode = phymod_AN_MODE_CL73_MSA;
                an_control.num_lane_adv = 8;
                _SOC_IF_ERR_EXIT(phymod_phy_autoneg_set(&phy_access, &an_control));
                an_state = PM8X50_FLEXE_GEN2_PORT_AN_RESTART;
            }

            if (serdes_reg_val & AN_X4_SW_CTRL_STS_AN_COMPLETE_MASK) {
                int phy_acc;
                uint32 reg_val;

                /* clear the counter */
                wait_link_count = 0;
                /* get port index */
                _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, &bitmap));
                _SOC_IF_ERR_EXIT(phymod_phy_reg_read(&phy_access, AN_X4_LP_BASE1r_REG, &base_reg_val));
                /* store remote pause value */
                remote_pause = (base_reg_val >> AN_LP_BASE1r_PAGE_PAUSE_OFFSET) & AN_LP_BASE1r_PAGE_PAUSE_MASK;
                _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_REMOTE_PAUSE_SET(unit, pm_info, remote_pause, port_index));

                /* need to move one to the next stage */
                /* disable AN_Cl73 and AN_CL73BAM */
                _SOC_IF_ERR_EXIT(phymod_phy_reg_read(&phy_access, AN_X4_CL73_CFG_REG, &serdes_reg_val));
                serdes_reg_val &= AN_X4_CL73_CFG_AN_DISABLE_MASK;
                _SOC_IF_ERR_EXIT(phymod_phy_reg_write(&phy_access, AN_X4_CL73_CFG_REG, serdes_reg_val));


                /* next update the cdport mode register */
                _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_phy_access_get(unit, port, pm_info, &phy_acc));
                _SOC_IF_ERR_EXIT(READ_CDPORT_MODE_REGr(unit, phy_acc, &reg_val));
                soc_reg_field_set(unit, CDPORT_MODE_REGr, &reg_val, SINGLE_PORT_MODE_SPEED_400Gf, 1);
                soc_reg_field_set(unit, CDPORT_MODE_REGr, &reg_val, MAC1_PORT_MODEf, CDMAC_4_LANES_TOGETHER);
                soc_reg_field_set(unit, CDPORT_MODE_REGr, &reg_val, MAC0_PORT_MODEf, CDMAC_4_LANES_TOGETHER);
                _SOC_IF_ERR_EXIT(WRITE_CDPORT_MODE_REGr(unit, phy_acc, reg_val));

                /* next update serdes side port mode to be octal */
                _SOC_IF_ERR_EXIT(phymod_phy_reg_read(&phy_access, MAIN0_SETUP_REG, &serdes_reg_val));
                serdes_reg_val = (serdes_reg_val & MAIN0_SETUP_PORT_MODE_CLEAR) | MAIN0_SETUP_PORT_MODE_OCTAL;
                _SOC_IF_ERR_EXIT(phymod_phy_reg_write(&phy_access, MAIN0_SETUP_REG, serdes_reg_val));

                /* next assert pcs reset */
                _SOC_IF_ERR_EXIT(phymod_phy_pcs_enable_set(&phy_access, 0));
                /* change the speed id  to use 0xa*/
                _SOC_IF_ERR_EXIT(phymod_phy_reg_read(&phy_access, SPEED_CTRL_REG, &serdes_reg_val));
                serdes_reg_val = (serdes_reg_val & SPEED_CTRL_SPEED_ID_CLEAR) | SPEED_ID_400G;
                _SOC_IF_ERR_EXIT(phymod_phy_reg_write(&phy_access, SPEED_CTRL_REG, serdes_reg_val));
                /* release pcs reset */
                _SOC_IF_ERR_EXIT(phymod_phy_pcs_enable_set(&phy_access, 1));
                an_state = PM8X50_FLEXE_GEN2_PORT_AN_CHECK_PCS_LINKUP;
            }
            break;
       }
       case PM8X50_FLEXE_GEN2_PORT_AN_CHECK_PCS_LINKUP:
       {
            int link, remote_fault;

            state_string = "AN_CHECK_PCS_LINKUP\n";
            /* first check link status */
            _SOC_IF_ERR_EXIT(phymod_phy_link_status_get(&phy_access, (uint32_t *)&link));
            _SOC_IF_ERR_EXIT(phymod_phy_link_status_get(&phy_access, (uint32_t *)&link));

            /* next check remote fualt */
            _SOC_IF_ERR_EXIT(cdmac_remote_fault_status_get(unit, port, &remote_fault));
            _SOC_IF_ERR_EXIT(cdmac_remote_fault_status_get(unit, port, &remote_fault));

            if (link && (!remote_fault)) {
                wait_link_count = 0;
                an_state = PM8X50_FLEXE_GEN2_PORT_AN_COMPLETE;
            } else {
                wait_link_count++;
                /* next check if AN needs to restarted */
                if (wait_link_count > TIMEOUT_COUNT_400G_AN) {
                    wait_link_count = 0;
                    /* needs to disable */
                    an_control.enable = 0;
                    an_control.an_mode = phymod_AN_MODE_CL73_MSA;
                    an_control.num_lane_adv = 8;
                    _SOC_IF_ERR_EXIT(phymod_phy_autoneg_set(&phy_access, &an_control));
                    an_state = PM8X50_FLEXE_GEN2_PORT_AN_RESTART;
                }
            }

            break;
       }
       case PM8X50_FLEXE_GEN2_PORT_AN_DISABLE:
       {
            state_string = "AN_DISABLE \n";
            /* first needs to disable */
            an_control.enable = 0;
            an_control.an_mode = phymod_AN_MODE_CL73_MSA;
            an_control.num_lane_adv = 8;
            _SOC_IF_ERR_EXIT(phymod_phy_autoneg_set(&phy_access, &an_control));
            an_state = PM8X50_FLEXE_GEN2_PORT_AN_RESTART;
            break;
       }
       case PM8X50_FLEXE_GEN2_PORT_AN_RESTART:
       {
            state_string = "AN_RESTART \n";
            _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_pm_port_mode_update(unit, port, pm_info, 0, 1));

            /* needs to enable autoneg */
            an_control.enable = 1;
            an_control.an_mode = phymod_AN_MODE_CL73_MSA;
            an_control.num_lane_adv = 8;
            _SOC_IF_ERR_EXIT(phymod_phy_autoneg_set(&phy_access, &an_control));
            an_state = PM8X50_FLEXE_GEN2_PORT_AN_CHECK_PAGE_COMPLETE;
            break;
       }

        case PM8X50_FLEXE_GEN2_PORT_AN_COMPLETE:
       {
            int link, remote_fault;
            state_string = "AN_COMPLTETE \n";

            /* first check if link is up, if not, need to re-start AN */
            _SOC_IF_ERR_EXIT(phymod_phy_link_status_get(&phy_access, (uint32_t *)&link));
            _SOC_IF_ERR_EXIT(phymod_phy_link_status_get(&phy_access, (uint32_t *)&link));

            /* next check remote fualt */
            _SOC_IF_ERR_EXIT(cdmac_remote_fault_status_get(unit, port, &remote_fault));
            _SOC_IF_ERR_EXIT(cdmac_remote_fault_status_get(unit, port, &remote_fault));

            if (!link  || remote_fault) {
                wait_link_count = 0;
                    /* first assert MAC soft reset */
                an_control.enable = 0;
                an_control.an_mode = phymod_AN_MODE_CL73_MSA;
                an_control.num_lane_adv = 8;
                _SOC_IF_ERR_EXIT(phymod_phy_autoneg_set(&phy_access, &an_control));
                an_state = PM8X50_FLEXE_GEN2_PORT_AN_RESTART;
                } else {
                    *is_done = 1;
                /*if no link or remote fault, re-start AN */
            }
            break;
       }
        default:
            *is_done = 1;
            break;
    }

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "port %d 400G AN state is %-23s \n"),
                 port, state_string));

    /* next store the an state */
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_STATE_SET(unit, pm_info, an_state));
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_WAIT_LINK_COUNT_SET(unit, pm_info, wait_link_count));

exit:
    SOC_FUNC_RETURN;
}

STATIC
void _pm8x50_flexe_gen2_signalling_method_get(int is_bypassed,
                                           const portmod_speed_config_t *speed_config,
                                           phymod_phy_signalling_method_t *signalling_method)
{
    int lane_rate;

    if (!is_bypassed) {
        lane_rate = speed_config->speed / speed_config->num_lane;
    } else {
        lane_rate = speed_config->speed;
    }

    *signalling_method = (lane_rate > NRZ_MAX_PER_LANE_SPEED ? phymodSignallingMethodPAM4 : phymodSignallingMethodNRZ);
}

/*Get whether the inerface type is supported by PM */
int pm8x50_flexe_gen2_pm_interface_type_is_supported(int unit,
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
pm8x50_flexe_gen2_default_fw_loader(const phymod_core_access_t* core, uint32_t length, const uint8_t* data)
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

    rv = portmod_firmware_set(unit,
                              user_data->blk_id,
                              data,
                              length,
                              load_order,
                              bcast_en,
                              &(pm8x50_flexe_gen2_ucode_buf[unit]),
                              &(pm8x50_flexe_gen2_ucode_buf_2nd[unit]),
                              CDPORT_TSC_UCMEM_DATAm,
                              CDPORT_TSC_MEM_CTRLr);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}


STATIC
int
pm8x50_flexe_gen2_default_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr,
                                 uint32_t val)
{
    /* will force the core_addr to zero since it's  NOT being used */
    return portmod_common_phy_sbus_reg_write(CDPORT_TSC_UCMEM_DATAm, user_acc,
                                             0x0, reg_addr, val);
}

STATIC
int
pm8x50_flexe_gen2_default_mem_write(void* user_acc,
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
    default:
        return SOC_E_PARAM;
    }
    array_ptr = val;

    rv = soc_mem_write(user_data->unit, tsc_mem_data, user_data->blk_id, mem_index, (void *) &array_ptr[0]);

    return rv;
}


STATIC
int
pm8x50_flexe_gen2_default_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr,
                                uint32_t *val)
{
    int rv = SOC_E_NONE;

    /* will force the core_addr to zero since it's NOT being used */
    rv = portmod_common_phy_sbus_reg_read(CDPORT_TSC_UCMEM_DATAm, user_acc,
                                            0x0, reg_addr, val);
    /* needs to down shift read by 16 bit due to pm8x50 HW change */
    *val = (*val >> 16) & 0xffff;
    return (rv);
}

STATIC
int
pm8x50_flexe_gen2_default_mem_read(void* user_acc,
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
    default:
        return SOC_E_PARAM;
    }

    rv = soc_mem_read(user_data->unit, tsc_mem_data, user_data->blk_id, mem_index, val);

    return rv;
}


/*
 * Function:
 *      portmod_pm8x50_flexe_gen2_wb_upgrade_func
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

int portmod_pm8x50_flexe_gen2_wb_upgrade_func(int unit, void *arg, int recovered_version,
                                           int new_version)
{
     return SOC_E_NONE;
}

int pm8x50_flexe_gen2_pm_wb_debug_log(int unit, pm_info_t pm_info)
{
    int i, rv = 0;
    int an_state_400g;
    int tmp_port, max_speed;
    int ts_enable_port_count;
    int is_400g_an;
    int an_400g_remote_pause;
    uint8_t pll0_active_lane_bm, pll1_active_lane_bm;
    uint32 is_core_initialized, is_active, is_bypassed;
    uint32 an_wait_link_count, timesync_enable;
    phymod_an_mode_type_t an_mode = 0;
    portmod_fec_t fec;
    int port_flexe_mode = 0, port_otn_mode = 0;
    int link_fail_wait_time;
    uint32 is_flexible_pll = 0;

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50_flexe_gen2\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50_flexe_gen2:{wb_buffer_index}\n")));
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "{%d}\n"), pm_info->wb_buffer_id));
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50_flexe_gen2:{wb_buffer_index}\n")));

    rv = PM8x50_FLEXE_GEN2_IS_CORE_INITIALIZED_GET(unit, pm_info, &is_core_initialized);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x50_flexe_gen2:{is_initialized}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), is_core_initialized));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x50_flexe_gen2:{is_initialized}\n")));
    }

    rv = PM8x50_FLEXE_GEN2_IS_ACTIVE_GET(unit, pm_info, &is_active);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x50_flexe_gen2:{is_active}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), is_active));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x50_flexe_gen2:{is_active}\n")));
    }

    rv = PM8x50_FLEXE_GEN2_PLL0_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll0_active_lane_bm);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x50_flexe_gen2:{pll0_active_lane_bitmap}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{0x%x}\n"), pll0_active_lane_bm));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x50_flexe_gen2:{pll0_active_lane_bitmap}\n")));
    }

    rv = PM8x50_FLEXE_GEN2_PLL1_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll1_active_lane_bm);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x50_flexe_gen2:{pll1_active_lane_bitmap}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{0x%x}\n"), pll1_active_lane_bm));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x50_flexe_gen2:{pll1_active_lane_bitmap}\n")));
    }

    rv = PM8x50_FLEXE_GEN2_TS_ENABLE_PORT_COUNT_GET(unit, pm_info, &ts_enable_port_count);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x50_flexe_gen2:{ts_enable_port_count}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), ts_enable_port_count));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x50_flexe_gen2:{ts_enable_port_count}\n")));
    }

    rv = PM8x50_FLEXE_GEN2_400G_AN_STATE_GET(unit, pm_info, &an_state_400g);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x50_flexe_gen2:{an_state_400g}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), an_state_400g));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x50_flexe_gen2:{an_state_400g}\n")));
    }

    rv = PM8x50_FLEXE_GEN2_400G_AN_WAIT_LINK_COUNT_GET(unit, pm_info, &an_wait_link_count);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x50_flexe_gen2:{an_wait_link_count}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), an_wait_link_count));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x50_flexe_gen2:{an_wait_link_count}\n")));
    }

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50_flexe_gen2:{lane,lane2port_map}\n")));
    for (i = 0 ; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {

        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit, "\n")));
        rv = PM8x50_FLEXE_GEN2_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, tmp_port));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50_flexe_gen2:{lane,lane2port_map}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50_flexe_gen2:{port,max_speed}\n")));
    for (i = 0 ; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
        rv = PM8x50_FLEXE_GEN2_MAX_SPEED_GET(unit, pm_info, &max_speed, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, max_speed));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50_flexe_gen2:{port,max_speed}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50_flexe_gen2:{port,an_mode}\n")));
    for (i = 0 ; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
        rv = PM8x50_FLEXE_GEN2_AN_MODE_GET(unit, pm_info, &an_mode, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, an_mode));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50_flexe_gen2:{port,an_mode}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50_flexe_gen2:{lane,lane2fec_map}\n")));
    for (i = 0 ; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
        rv = PM8x50_FLEXE_GEN2_LANE2FEC_GET(unit, pm_info, i, &fec);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, fec));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50_flexe_gen2:{lane,lane2fec_map}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50_flexe_gen2:{port,port_is_pcs_bypassed}\n")));
    for (i = 0 ; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
        rv = PM8x50_FLEXE_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, is_bypassed));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50_flexe_gen2:{port,port_is_pcs_bypassed}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50_flexe_gen2:{port,timesync_enable}\n")));
    for (i = 0 ; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
        rv = PM8x50_FLEXE_GEN2_TIMESYNC_CONFIG_GET(unit, pm_info, &timesync_enable, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, timesync_enable));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50_flexe_gen2:{port,timesync_enable}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50_flexe_gen2:{port,400g_an_enable}\n")));
    for (i = 0 ; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
        rv = PM8x50_FLEXE_GEN2_400G_AN_ENABLE_GET(unit, pm_info, &is_400g_an, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, is_400g_an));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50_flexe_gen2:{port,400g_an_enable}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50_flexe_gen2:{port,an_400g_remote_pause}\n")));
    for (i = 0 ; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
        rv = PM8x50_FLEXE_GEN2_400G_AN_REMOTE_PAUSE_GET(unit, pm_info, &an_400g_remote_pause, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, an_400g_remote_pause));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50_flexe_gen2:{port,an_400g_remote_pause}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50_flexe_gen2:{port,port_flexe_mode}\n")));
    for (i = 0 ; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
        rv = PM8x50_FLEXE_GEN2_PORT_FLEXE_MODE_GET(unit, pm_info, &port_flexe_mode, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, port_flexe_mode));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50_flexe_gen2:{port,port_flexe_mode}\n")));

    rv = PM8x50_FLEXE_GEN2_AN_LINK_FAIL_INHIBIT_TIMER_LT_PAM4_GET(unit, pm_info, &link_fail_wait_time);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x50_flexe_gen2:{an link fail inhibit timer for pam4 with lt}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), link_fail_wait_time));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x50_flexe_gen2:{an link fail inhibit timer for pam4 with lt}\n")));
    }

    rv = PM8x50_FLEXE_GEN2_IS_FLEXIBLE_PLL_GET(unit, pm_info, &is_flexible_pll);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x50_flexe_gen2:{is_flexible_pll}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), is_flexible_pll));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x50_flexe_gen2:{is_active}\n")));
    }

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50_flexe_gen2:{port,port_otn_mode}\n")));
    for (i = 0 ; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
        rv = PM8x50_FLEXE_GEN2_PORT_FLEXE_MODE_GET(unit, pm_info, &port_otn_mode, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, port_otn_mode));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50_flexe_gen2:{port,port_flexe_mode}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50_flexe_gen2\n")));

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
int pm8x50_flexe_gen2_wb_buffer_init(int unit, int wb_buffer_index, pm_info_t pm_info)
{
    /* Declare the common variables needed for warmboot */
    WB_ENGINE_INIT_TABLES_DEFS;
    int wb_var_id, rv;
    int buffer_id = wb_buffer_index; /*required by SOC_WB_ENGINE_ADD_ Macros*/
    SOC_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(buffer_is_dynamic);

    SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PORTMOD, wb_buffer_index, "pm8x50_flexe_gen2",
                           portmod_pm8x50_flexe_gen2_wb_upgrade_func, pm_info,
                           PM8x50_FLEXE_GEN2_WB_BUFFER_VERSION, 1,
                           SOC_WB_ENGINE_PRE_RELEASE);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id,
                         "is_core_initialized", wb_buffer_index, sizeof(int),
                          NULL, PM8X50_FLEXE_GEN2_PORTS_PER_CORE, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isCoreInitialized] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_active",
                          wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isActive] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "pll0_active_lane_bitmap",
                          wb_buffer_index, sizeof(uint8), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[pll0ActiveLaneBitmap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "pll1_active_lane_bitmap",
                          wb_buffer_index, sizeof(uint8), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[pll1ActiveLaneBitmap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "pll0_adv_lane_bitmap",
                          wb_buffer_index, sizeof(uint8), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[pll0AdvLaneBitmap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "pll1_adv_lane_bitmap",
                          wb_buffer_index, sizeof(uint8), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[pll1AdvLaneBitmap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "lane2portMap",
                          wb_buffer_index, sizeof(int), NULL, PM8X50_FLEXE_GEN2_PORTS_PER_CORE, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[lane2portMap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "max_speed",
                          wb_buffer_index, sizeof(uint32), NULL, PM8X50_FLEXE_GEN2_PORTS_PER_CORE, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[maxSpeed] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "an_mode",
                          wb_buffer_index, sizeof(int), NULL, PM8X50_FLEXE_GEN2_PORTS_PER_CORE, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[anMode] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "lane2fecMap",
                          wb_buffer_index, sizeof(int), NULL, PM8X50_FLEXE_GEN2_PORTS_PER_CORE, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[lane2fecMap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));

    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "ts_enable_port_count",
                          wb_buffer_index, sizeof(int), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[tsEnablePortCount] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "timesync_enable",
                           wb_buffer_index, sizeof(uint32), NULL, PM8X50_FLEXE_GEN2_PORTS_PER_CORE, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[timesyncEnable] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "400g_an_enable",
                           wb_buffer_index, sizeof(uint32), NULL, PM8X50_FLEXE_GEN2_PORTS_PER_CORE, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[is400gAn] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "an_state_400g",
                          wb_buffer_index, sizeof(int), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[anState400g] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "an_wait_link_count",
                          wb_buffer_index, sizeof(int), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[anWaitlinkCount] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "port_is_pcs_bypassed",
                          wb_buffer_index, sizeof(uint32), NULL, PM8X50_FLEXE_GEN2_PORTS_PER_CORE, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[portIsPcsBypassed] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "an_400g_remote_pause",
                          wb_buffer_index, sizeof(int), NULL, PM8X50_FLEXE_GEN2_PORTS_PER_CORE, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[an400gRemotePause] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "port_flexe_mode",
                          wb_buffer_index, sizeof(int), NULL, PM8X50_FLEXE_GEN2_PORTS_PER_CORE, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[portFlexeMode] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id,
                          "anLinkfailInhibitTimerLtPam4", wb_buffer_index, sizeof(int),
                          NULL, PM8X50_FLEXE_GEN2_PORTS_PER_CORE, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[anLinkfailInhibitTimerLtPam4] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_flexible_pll",
                          wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isFlexiblePLL] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "port_otn_mode",
                          wb_buffer_index, sizeof(int), NULL, PM8X50_FLEXE_GEN2_PORTS_PER_CORE, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[portOtnMode] = wb_var_id;

    _SOC_IF_ERR_EXIT(soc_wb_engine_init_buffer(unit, SOC_WB_ENGINE_PORTMOD,
                                               wb_buffer_index, FALSE));

exit:
    SOC_FUNC_RETURN;
}

phymod_bus_t pm8x50_flexe_gen2_default_bus = {
    "PM8x50_FLEXE_GEN2 Bus",
    pm8x50_flexe_gen2_default_bus_read,
    pm8x50_flexe_gen2_default_bus_write,
    NULL,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    pm8x50_flexe_gen2_default_mem_read,
    pm8x50_flexe_gen2_default_mem_write,
    PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};



int pm8x50_flexe_gen2_port_mac_link_get(int unit, int port, pm_info_t pm_info, int* link)
{
    uint32 reg_val, bitmap;
    int port_index, phy_acc;
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_phy_access_get(unit, port, pm_info, &phy_acc));
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, &bitmap));

    switch(port_index) {
        case 0:
            _SOC_IF_ERR_EXIT(READ_CDPORT_XGXS0_LN0_STATUS_REGr(unit, phy_acc, &reg_val));
            (*link) = soc_reg_field_get(unit, CDPORT_XGXS0_LN0_STATUS_REGr, reg_val, LINK_STATUSf);
            break;

        case 1:
            _SOC_IF_ERR_EXIT(READ_CDPORT_XGXS0_LN1_STATUS_REGr(unit, phy_acc, &reg_val));
            (*link) = soc_reg_field_get(unit, CDPORT_XGXS0_LN1_STATUS_REGr, reg_val, LINK_STATUSf);
            break;

        case 2:
            _SOC_IF_ERR_EXIT(READ_CDPORT_XGXS0_LN2_STATUS_REGr(unit, phy_acc, &reg_val));
            (*link) = soc_reg_field_get(unit, CDPORT_XGXS0_LN2_STATUS_REGr, reg_val, LINK_STATUSf);
            break;

        case 3:
            _SOC_IF_ERR_EXIT(READ_CDPORT_XGXS0_LN3_STATUS_REGr(unit, phy_acc, &reg_val));
            (*link) = soc_reg_field_get(unit, CDPORT_XGXS0_LN3_STATUS_REGr, reg_val, LINK_STATUSf);
            break;

        case 4:
            _SOC_IF_ERR_EXIT(READ_CDPORT_XGXS0_LN4_STATUS_REGr(unit, phy_acc, &reg_val));
            (*link) = soc_reg_field_get(unit, CDPORT_XGXS0_LN4_STATUS_REGr, reg_val, LINK_STATUSf);
            break;

        case 5:
            _SOC_IF_ERR_EXIT(READ_CDPORT_XGXS0_LN5_STATUS_REGr(unit, phy_acc, &reg_val));
            (*link) = soc_reg_field_get(unit, CDPORT_XGXS0_LN5_STATUS_REGr, reg_val, LINK_STATUSf);
            break;

        case 6:
            _SOC_IF_ERR_EXIT(READ_CDPORT_XGXS0_LN6_STATUS_REGr(unit, phy_acc, &reg_val));
            (*link) = soc_reg_field_get(unit, CDPORT_XGXS0_LN6_STATUS_REGr, reg_val, LINK_STATUSf);
            break;

        case 7:
            _SOC_IF_ERR_EXIT(READ_CDPORT_XGXS0_LN7_STATUS_REGr(unit, phy_acc, &reg_val));
            (*link) = soc_reg_field_get(unit, CDPORT_XGXS0_LN7_STATUS_REGr, reg_val, LINK_STATUSf);
            break;

        default:
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                     (_SOC_MSG("Port %d, failed to get port index"), port));
    }

exit:
    SOC_FUNC_RETURN;
}

static int pm8x50_flexe_gen2_periodic_callback(int unit, int port)
{
    pm_info_t pm_info;
    int port_index, is_400g_an;
    uint32 bitmap;
    int an_done = 1;
    int valid = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_is_valid(unit, port, &valid));
    if (!valid) {
        return (SOC_E_NONE);
    }
    _SOC_IF_ERR_EXIT(portmod_pm_info_get(unit, port, &pm_info));

    /* get port index */
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, &bitmap));

    /* read the 400G an enabled */
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_ENABLE_GET(unit, pm_info, &is_400g_an, port_index));

    /* check if 400G an state machine needs to be updated */
    if (is_400g_an) {
        _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_400g_an_workaround(unit, port, pm_info, &an_done));
    }

     /* if both signal unregister - go for it */
    if (!is_400g_an) {
        _SOC_IF_ERR_EXIT(portmod_port_periodic_callback_unregister(unit, port));
    }

exit:
    SOC_FUNC_RETURN;
}

/*Add new pm.*/
int pm8x50_flexe_gen2_pm_init(int unit,
                           const portmod_pm_create_info_internal_t* pm_add_info,
                           int wb_buffer_index,
                           pm_info_t pm_info)
{
    const portmod_pm8x50_flexe_gen2_create_info_t *info =
                &pm_add_info->pm_specific_info.pm8x50_flexe_gen2;
    pm8x50_flexe_gen2_t pm8x50_flexe_gen2_data = NULL;
    int i, rv;
    int pm_is_active, ts_enable_port_count;
    int is_core_initialized, fec, invalid_port;
    int an_state, is_400g_an, an_wait_link_count;
    int remote_pause;
    uint8 pll0_adv_lane_bitmap, pll1_adv_lane_bitmap;
    uint8 pll0_active_lane_bitmap, pll1_active_lane_bitmap;
    int phy;
    int portmon_disable, port_flexe_mode, port_otn_mode;
    int link_fail_wait_time;
    uint32 flexible_pll_required = 0;
    SOC_INIT_FUNC_DEFS;

    pm_info->type = pm_add_info->type;
    pm_info->unit = unit;
    pm_info->wb_buffer_id = wb_buffer_index;

    /* PM8x50 specific info */
    pm8x50_flexe_gen2_data = sal_alloc(sizeof(struct pm8x50_flexe_gen2_s), "specific_db");
    SOC_NULL_CHECK(pm8x50_flexe_gen2_data);
    pm_info->pm_data.pm8x50_flexe_gen2_db = pm8x50_flexe_gen2_data;

    PORTMOD_PBMP_ASSIGN(pm8x50_flexe_gen2_data->phys, pm_add_info->phys);
    pm8x50_flexe_gen2_data->int_core_access.type = phymodDispatchTypeCount;
    PORTMOD_PBMP_ITER(pm_add_info->phys, phy) {
        PM_8x50_FLEXE_GEN2_INFO(pm_info)->first_phy = phy;
        break;
    }
    pm8x50_flexe_gen2_data->warmboot_skip_db_restore = TRUE;
    pm8x50_flexe_gen2_data->portmod_mac_soft_reset = info->portmod_mac_soft_reset;

    /* init intertnal SerDes core access */
    phymod_core_access_t_init(&pm8x50_flexe_gen2_data->int_core_access);

    sal_memcpy(&pm8x50_flexe_gen2_data->polarity, &info->polarity,
               sizeof(phymod_polarity_t));
    sal_memcpy(&(pm8x50_flexe_gen2_data->int_core_access.access), &info->access.access,
                sizeof(phymod_access_t));

    sal_memcpy(&pm8x50_flexe_gen2_data->lane_map, &info->lane_map,
                sizeof(pm8x50_flexe_gen2_data->lane_map));
    pm8x50_flexe_gen2_data->ref_clk = info->ref_clk;
    pm8x50_flexe_gen2_data->fw_load_method = info->fw_load_method;
    pm8x50_flexe_gen2_data->external_fw_loader = info->external_fw_loader;
    pm8x50_flexe_gen2_data->core_num  = info->core_num;
    pm8x50_flexe_gen2_data->afe_pll = info->afe_pll;
    pm8x50_flexe_gen2_data->is_master_pm = info->is_master_pm;
    pm8x50_flexe_gen2_data->pm_offset = info->pm_offset;

    if (info->access.access.bus == NULL) {
        /* if null - use default */
        pm8x50_flexe_gen2_data->int_core_access.access.bus = &pm8x50_flexe_gen2_default_bus;
    } else {
        /* check null for mem_read/mem_write */
        if (pm8x50_flexe_gen2_data->int_core_access.access.bus->mem_read == NULL) {
            pm8x50_flexe_gen2_data->int_core_access.access.bus->mem_read =pm8x50_flexe_gen2_default_bus.mem_read;
        }
        if (pm8x50_flexe_gen2_data->int_core_access.access.bus->mem_write == NULL) {
            pm8x50_flexe_gen2_data->int_core_access.access.bus->mem_write =pm8x50_flexe_gen2_default_bus.mem_write;
        }
    }

    if (pm8x50_flexe_gen2_data->external_fw_loader == NULL) {
        /* if null - use default */
        pm8x50_flexe_gen2_data->external_fw_loader = pm8x50_flexe_gen2_default_fw_loader;
    }

    /*init wb buffer*/
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_wb_buffer_init(unit, wb_buffer_index,  pm_info));

    /* Need to check if 4 copy of pmd_info_ptr needed*/
    if (pm8x50_flexe_gen2_data->int_core_access.access.pmd_info_ptr == NULL) {
        pm8x50_flexe_gen2_data->int_core_access.access.pmd_info_ptr = sal_alloc(PMD_INFO_DATA_STRUCTURE_SIZE,
                             "PortMod PM PMD info");
        if (pm8x50_flexe_gen2_data->int_core_access.access.pmd_info_ptr == NULL) {
            return SOC_E_MEMORY;
        }
        sal_memset(pm8x50_flexe_gen2_data->int_core_access.access.pmd_info_ptr, 0, PMD_INFO_DATA_STRUCTURE_SIZE);
    }


    if (SOC_WARM_BOOT(unit)) {
        int i, port_local;
        /* For warmboot, probe Serdes driver type for active PMs. */
        is_core_initialized = 0;
        rv = PM8x50_FLEXE_GEN2_IS_CORE_INITIALIZED_GET(unit, pm_info, &is_core_initialized);
        _SOC_IF_ERR_EXIT(rv);
        _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_STATE_GET(unit, pm_info, &an_state));


        /* print scache information for debugging */
        pm8x50_flexe_gen2_pm_wb_debug_log(unit, pm_info);

        /* next check if portmod thread needs to be re-registered */
        for (i = 0; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
            /* first get the logical port id */
            _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_LANE2PORT_GET(unit, pm_info,i, &port_local));
            _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_ENABLE_GET(unit, pm_info, &is_400g_an, i));
            _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_REMOTE_PAUSE_GET(unit, pm_info, &remote_pause, i));
            /* need to re-register the call back */
            if ( (port_local >= 0) && ((is_400g_an) && (an_state < PM8X50_FLEXE_GEN2_PORT_AN_COMPLETE))) {
                    _SOC_IF_ERR_EXIT
                        (portmod_portmon_disable_get(unit, &portmon_disable));
                    if (!portmon_disable) {
                        _SOC_IF_ERR_EXIT(portmod_port_periodic_callback_register(unit, port_local, pm8x50_flexe_gen2_periodic_callback, PORTMOD_THREAD_INTERVAL_1s));
                    }
            }
        }
    } else {
        /* For coldboot, initialized warmboot variables for the PM. */
        is_core_initialized = 0;
        rv = PM8x50_FLEXE_GEN2_IS_CORE_INITIALIZED_SET(unit, pm_info,
                                                    is_core_initialized);
        _SOC_IF_ERR_EXIT(rv);

        pm_is_active = 0;
        rv = PM8x50_FLEXE_GEN2_IS_ACTIVE_SET(unit, pm_info, pm_is_active);
        _SOC_IF_ERR_EXIT(rv);

        pll0_active_lane_bitmap = 0;
        rv = PM8x50_FLEXE_GEN2_PLL0_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll0_active_lane_bitmap);
        _SOC_IF_ERR_EXIT(rv);

        pll1_active_lane_bitmap = 0;
        rv = PM8x50_FLEXE_GEN2_PLL1_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll1_active_lane_bitmap);
        _SOC_IF_ERR_EXIT(rv);

        pll0_adv_lane_bitmap = 0;
        rv = PM8x50_FLEXE_GEN2_PLL0_ADV_LANE_BITMAP_SET(unit, pm_info, pll0_adv_lane_bitmap);
        _SOC_IF_ERR_EXIT(rv);

        pll1_adv_lane_bitmap = 0;
        rv = PM8x50_FLEXE_GEN2_PLL1_ADV_LANE_BITMAP_SET(unit, pm_info, pll1_adv_lane_bitmap);
        _SOC_IF_ERR_EXIT(rv);

        fec = 0;
        for (i = 0; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
            _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_LANE2FEC_SET(unit, pm_info, i, fec));
        }

        invalid_port = -1;
        for (i = 0; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
            _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_LANE2PORT_SET(unit, pm_info, i, invalid_port));
        }

        ts_enable_port_count = 0;
        rv = PM8x50_FLEXE_GEN2_TS_ENABLE_PORT_COUNT_SET(unit, pm_info, ts_enable_port_count);
        _SOC_IF_ERR_EXIT(rv);

        is_400g_an = 0;
        for (i = 0; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
            _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_ENABLE_SET(unit, pm_info, is_400g_an, i));
        }

        an_state = PM8X50_FLEXE_GEN2_PORT_AN_STATE_COUNT;
        rv = PM8x50_FLEXE_GEN2_400G_AN_STATE_SET(unit, pm_info, an_state);
        _SOC_IF_ERR_EXIT(rv);

        an_wait_link_count = 0;
        rv = PM8x50_FLEXE_GEN2_400G_AN_WAIT_LINK_COUNT_SET(unit, pm_info, an_wait_link_count);
        _SOC_IF_ERR_EXIT(rv);

        remote_pause = 0;
        for (i = 0; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
            _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_REMOTE_PAUSE_SET(unit, pm_info, remote_pause, i));
        }

        port_flexe_mode = 0;
        for (i = 0; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
            rv = PM8x50_FLEXE_GEN2_PORT_FLEXE_MODE_SET(unit, pm_info, port_flexe_mode, i);
            _SOC_IF_ERR_EXIT(rv);
        }

        link_fail_wait_time = AN_LINK_FAIL_INHIBIT_TIMER_LT_PAM4_DEFAULT;
        rv = PM8x50_FLEXE_GEN2_AN_LINK_FAIL_INHIBIT_TIMER_LT_PAM4_SET(unit, pm_info, link_fail_wait_time);
        _SOC_IF_ERR_EXIT(rv);

        flexible_pll_required = info->flexible_pll_required;
        rv = PM8x50_FLEXE_GEN2_IS_FLEXIBLE_PLL_SET(unit, pm_info, flexible_pll_required);
        _SOC_IF_ERR_EXIT(rv);

        port_otn_mode = 0;
        for (i = 0; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
            rv = PM8x50_FLEXE_GEN2_PORT_OTN_MODE_SET(unit, pm_info, port_otn_mode, i);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

exit:
    if (SOC_FUNC_ERROR) {
        pm8x50_flexe_gen2_pm_destroy(unit, pm_info);
    }
    SOC_FUNC_RETURN;
}

/*Release PM resources*/
int pm8x50_flexe_gen2_pm_destroy(int unit, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;

    if (pm_info->pm_data.pm8x50_flexe_gen2_db != NULL) {
        if (PM_8x50_FLEXE_GEN2_INFO(pm_info)->int_core_access.access.pmd_info_ptr != NULL) {
            sal_free(PM_8x50_FLEXE_GEN2_INFO(pm_info)->int_core_access.access.pmd_info_ptr);
            PM_8x50_FLEXE_GEN2_INFO(pm_info)->int_core_access.access.pmd_info_ptr = NULL;
        }
        sal_free(pm_info->pm_data.pm8x50_flexe_gen2_db);
        pm_info->pm_data.pm8x50_flexe_gen2_db = NULL;
    }

    SOC_FUNC_RETURN;
}

/* This function will check the speed config is valid or not, and return required vco for valid input */
/* The supported speed config in this function is based on PM8x50 Portmod Spec */
STATIC
int _pm8x50_flexe_gen2_port_speed_config_to_vco_get(const portmod_speed_config_t* speed_config,
                                                 int speed_for_pcs_bypass_port,
                                                 portmod_vco_type_t* vco)
{
    SOC_INIT_FUNC_DEFS;

    if (!speed_for_pcs_bypass_port) {
        _SOC_IF_ERR_EXIT(
                _pm8x50_flexe_gen2_fs_ability_table_vco_get(speed_config->speed,
                                                      speed_config->num_lane,
                                                      speed_config->fec,
                                                      vco));
    } else {
        _SOC_IF_ERR_EXIT(
                _pm8x50_flexe_gen2_shared_pcs_bypassed_vco_get(speed_config->speed, vco));
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
int _pm8x50_flexe_gen2_lanebitmap_set(int starting_lane, int num_lane, uint8 *bitmap)
{
    int i;

    for (i = 0; i < num_lane; i++) {
        *bitmap |= 1 << (starting_lane + i);
    }

    return SOC_E_NONE;
}

/* This function is to validate the FEC settings on each PM.
 * For each MPP, we can not support both RS544 and RS272.
 * On 16nm, we can also not support both RS528 and RS272 on the same MPP.
 */
STATIC
int _pm8x50_flexe_gen2_fec_validate(int unit,
                                 uint8 rs528_bitmap,
                                 uint8 rs544_bitmap,
                                 uint8 rs272_bitmap,
                                 uint8 *affected_lane_bitmap)
{
    uint8 rs_fec_mpp0, rs_fec_mpp1, rs272_mpp0, rs272_mpp1;
    SOC_INIT_FUNC_DEFS;

    /* rs_fec_mpp0, rs_fec_mpp1 contains lane bitmap for
     * the RS FECs which can not co-exist with RS272.
     * For 7nm, rsfec contains bitmap for RS544 only(no RS528).
     */
    rs_fec_mpp0 = rs544_bitmap & 0xf;
    rs_fec_mpp1 = rs544_bitmap & 0xf0;

    rs272_mpp0 = rs272_bitmap & 0xf;
    rs272_mpp1 = rs272_bitmap & 0xf0;
    *affected_lane_bitmap = 0;

    if (rs_fec_mpp0 && rs272_mpp0) {
        *affected_lane_bitmap = rs_fec_mpp0 | rs272_mpp0;
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("Can not accommodate FEC settings on MPP0.")));

    } else if (rs_fec_mpp1 && rs272_mpp1) {
        *affected_lane_bitmap = rs_fec_mpp1 | rs272_mpp1;
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("Can not accommodate FEC settings on MPP1.")));
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm8x50_flexe_gen2_port_tsc_reset_set(int unit, pm_info_t pm_info, int phy_acc, int in_reset)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDPORT_XGXS0_CTRL_REGr(unit, phy_acc, &reg_val));

    /*
     * in_reset == 1:
     *       1. Power down TSC.
     *       2. Put TSC into RST for slave cores.
     * in_reset == 0:
     *       1. Power up TSC.
     *       2. Toggle TSC RST bit.
     */
    if (in_reset) {
        int is_core_initialized = 0;
        soc_reg_field_set(unit, CDPORT_XGXS0_CTRL_REGr, &reg_val, TSC_PWRDWNf, 1);
        if (!(PM_8x50_FLEXE_GEN2_INFO(pm_info)->is_master_pm)) {
            /* Put TSC into reset for slave PMs */
            soc_reg_field_set(unit, CDPORT_XGXS0_CTRL_REGr, &reg_val, TSC_RSTBf, 0);
        }
        _SOC_IF_ERR_EXIT(WRITE_CDPORT_XGXS0_CTRL_REGr(unit, phy_acc, reg_val));

        /* clear the warm boot variable */
        _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_IS_CORE_INITIALIZED_SET(unit, pm_info, is_core_initialized));
    } else {
        soc_reg_field_set(unit, CDPORT_XGXS0_CTRL_REGr, &reg_val, TSC_PWRDWNf, 1);
        soc_reg_field_set(unit, CDPORT_XGXS0_CTRL_REGr, &reg_val, TSC_RSTBf, 0);
        _SOC_IF_ERR_EXIT(WRITE_CDPORT_XGXS0_CTRL_REGr(unit, phy_acc, reg_val));

        soc_reg_field_set(unit, CDPORT_XGXS0_CTRL_REGr, &reg_val, TSC_PWRDWNf, 0);
        soc_reg_field_set(unit, CDPORT_XGXS0_CTRL_REGr, &reg_val, TSC_RSTBf, 1);
        _SOC_IF_ERR_EXIT(WRITE_CDPORT_XGXS0_CTRL_REGr(unit, phy_acc, reg_val));
    }


    /* Based on the feedback from SJ pmd support team, ~10-15usecs would be sufficient
     * for the PLL/AFE to settle down out of IDDQ reset.
     */
    sal_usleep(10);

exit:
    SOC_FUNC_RETURN;
}

/*Enable port macro.*/
/* This function contains 3 parts:
 * 1. Bring Serdes out of hard reset.
 * 2. Bring 2 CDMACs out of hard reset.
 */
int pm8x50_flexe_gen2_pm_enable(int unit,
                             int pm_id,
                             pm_info_t pm_info,
                             int flags,
                             int enable)
{
    uint32 reg_val;
    int phy_acc;
    int is_reset;
    int skip_mac, mac_only;

    SOC_INIT_FUNC_DEFS;

    /* Get the first physical port of the pm core */
    phy_acc = PM_8x50_FLEXE_GEN2_INFO(pm_info)->first_phy | SOC_REG_ADDR_PHY_ACC_MASK;

    _SOC_IF_ERR_EXIT(READ_CDPORT_MAC_CONTROLr(unit, phy_acc, &reg_val));
    is_reset = soc_reg_field_get(unit, CDPORT_MAC_CONTROLr, reg_val, CDMAC0_RESETf);

    skip_mac = PORTMOD_PM_ENABLE_SKIP_MAC_GET(flags);
    mac_only = PORTMOD_PM_ENABLE_MAC_ONLY_GET(flags);
    if(enable && is_reset) {
        if (!mac_only) {
            /* Bring Serdes OOR */
            _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_tsc_reset_set(unit, pm_info, phy_acc, 0));
        }
        if (!skip_mac) {            /* Bring MAC OOR */
            _SOC_IF_ERR_EXIT(READ_CDPORT_MAC_CONTROLr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CDPORT_MAC_CONTROLr, &reg_val, CDMAC0_RESETf, 0);
            _SOC_IF_ERR_EXIT(WRITE_CDPORT_MAC_CONTROLr(unit, phy_acc, reg_val));
        }
    } else if ((!enable) && (!is_reset)){ /* disable */
        if (!skip_mac) {
            /* put MAC in reset */
            _SOC_IF_ERR_EXIT(READ_CDPORT_MAC_CONTROLr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CDPORT_MAC_CONTROLr, &reg_val, CDMAC0_RESETf, 1);
            _SOC_IF_ERR_EXIT(WRITE_CDPORT_MAC_CONTROLr(unit, phy_acc, reg_val));
        }
        if (!mac_only) {
            /* If current PM is not master PM, put Serdes into reset*/
            _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_tsc_reset_set(unit, pm_info, phy_acc, 1));
        }
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm8x50_flexe_gen2_pm_core_probe(int unit, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int probe =0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_common_serdes_probe(pm8x50_flexe_gen2_serdes_list, &PM_8x50_FLEXE_GEN2_INFO(pm_info)->int_core_access, &probe));

exit:
    SOC_FUNC_RETURN;
}

/* This function will return required pll based on the ref_clk and vco */
STATIC
int _pm8x50_flexe_gen2_vco_to_pll_get(phymod_ref_clk_t ref_clock, portmod_vco_type_t vco, uint32_t* pll, uint8_t is_otn_mode)
{
    int rv = SOC_E_NONE;
    SOC_INIT_FUNC_DEFS;

    switch (vco) {
        case portmodVCO20P625G:
            if (ref_clock == phymodRefClk156Mhz) {
                *pll = phymod_TSCBH_PLL_DIV132;
            } else if (ref_clock == phymodRefClk312Mhz) {
                *pll = phymod_TSCBH_PLL_DIV66;
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case portmodVCO25P781G:
            if (ref_clock == phymodRefClk156Mhz) {
                if (is_otn_mode) {
                    *pll = phymod_TSCBH_PLL_DIV165P0025695;
                } else {
                    *pll = phymod_TSCBH_PLL_DIV165;
                }
            } else if (ref_clock == phymodRefClk312Mhz) {
                *pll = phymod_TSCBH_PLL_DIV82P5;
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case portmodVCO26P562G:
            if (ref_clock == phymodRefClk156Mhz) {
                if (is_otn_mode) {
                    *pll = phymod_TSCBH_PLL_DIV170P0026474;
                } else {
                    *pll = phymod_TSCBH_PLL_DIV170;
                }
            } else if (ref_clock == phymodRefClk312Mhz) {
                *pll = phymod_TSCBH_PLL_DIV85;
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case portmodVCO25G:
            if (ref_clock == phymodRefClk156Mhz) {
                *pll = phymod_TSCBH_PLL_DIV160;
            } else if (ref_clock == phymodRefClk312Mhz) {
                *pll = phymod_TSCBH_PLL_DIV80;
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case portmodVCO27P343G:
            if (ref_clock == phymodRefClk156Mhz) {
                *pll = phymod_TSCBH_PLL_DIV175;
            } else if (ref_clock == phymodRefClk312Mhz) {
                *pll = phymod_TSCBH_PLL_DIV87P5;
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case portmodVCO28P125G:
            if (ref_clock == phymodRefClk156Mhz) {
                *pll = phymod_TSCBH_PLL_DIV180;
            } else if (ref_clock == phymodRefClk312Mhz) {
                *pll = phymod_TSCBH_PLL_DIV90;
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case portmodVCO26P773G:
            if (ref_clock == phymodRefClk156Mhz) {
                *pll = phymod_TSCBH_PLL_DIV171P347605;
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case portmodVCO27P622G:
            if (ref_clock == phymodRefClk156Mhz) {
                *pll = phymod_TSCBH_PLL_DIV176P7857143;
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case portmodVCO27P739G:
            if (ref_clock == phymodRefClk156Mhz) {
                *pll = phymod_TSCBH_PLL_DIV177P5316456;
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case portmodVCO27P952G:
            if (ref_clock == phymodRefClk156Mhz) {
                
                *pll = phymod_TSCBH_PLL_DIV178P8959577;
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case portmodVCO28P076G:
            if (ref_clock == phymodRefClk156Mhz) {
                *pll = phymod_TSCBH_PLL_DIV179P6875327;
            } else {
                rv = SOC_E_PARAM;
            }
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
int _pm8x50_flexe_gen2_pm_serdes_core_init(int unit, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int rv;
    phymod_core_init_config_t core_conf;
    phymod_core_status_t core_status;
    uint32_t core_is_initialized;
    uint32_t init_flags = 0;

    SOC_INIT_FUNC_DEFS;

    rv = PM8x50_FLEXE_GEN2_IS_CORE_INITIALIZED_GET(unit, pm_info, &core_is_initialized)
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(phymod_core_init_config_t_init(&core_conf));

    core_conf.firmware_load_method = PM_8x50_FLEXE_GEN2_INFO(pm_info)->fw_load_method;
    core_conf.firmware_loader = PM_8x50_FLEXE_GEN2_INFO(pm_info)->external_fw_loader;
    core_conf.lane_map = PM_8x50_FLEXE_GEN2_INFO(pm_info)->lane_map;
    core_conf.polarity_map = PM_8x50_FLEXE_GEN2_INFO(pm_info)->polarity;

    core_conf.afe_pll.afe_pll_change_default = PM_8x50_FLEXE_GEN2_INFO(pm_info)->afe_pll.afe_pll_change_default;
    core_conf.afe_pll.ams_pll_iqp = PM_8x50_FLEXE_GEN2_INFO(pm_info)->afe_pll.ams_pll_iqp;
    core_conf.afe_pll.ams_pll_en_hrz = PM_8x50_FLEXE_GEN2_INFO(pm_info)->afe_pll.ams_pll_en_hrz;

    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&core_conf.interface));
    core_conf.interface.ref_clock = PM_8x50_FLEXE_GEN2_INFO(pm_info)->ref_clk;

    /* Just set pll_DIVNONE as default */
    core_conf.pll0_div_init_value = phymod_TSCBH_PLL_DIVNONE;
    core_conf.pll1_div_init_value = phymod_TSCBH_PLL_DIVNONE;

    _SOC_IF_ERR_EXIT(phymod_core_status_t_init(&core_status));
    core_status.pmd_active = 0;
    init_flags = PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) | PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info);

    if (PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_GET(add_info)) {
        PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_SET(&core_conf);
    }
    if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        PHYMOD_CORE_INIT_F_EXECUTE_PASS1_SET(&core_conf);
    }
    if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
        PHYMOD_CORE_INIT_F_EXECUTE_PASS2_SET(&core_conf);
    }
    if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
        PHYMOD_CORE_INIT_F_RESUME_AFTER_FW_LOAD_SET(&core_conf);
    }

    if (PORTMOD_PORT_ADD_F_BYPASS_FW_CRC_CHECK_GET(add_info)) {
        PHYMOD_CORE_INIT_F_BYPASS_CRC_CHECK_SET(&core_conf);
    }

#ifdef FW_BCAST_DOWNLOAD
    if (PORTMOD_PORT_ADD_F_FIRMWARE_DOWNLOAD_BCAST_GET(add_info)) {
        PHYMOD_CORE_INIT_F_SERDES_FW_BCAST_SET(&core_conf);
    }
#endif

    if (!PORTMOD_CORE_INIT_FLAG_INITIALZIED_GET(core_is_initialized)) {
         /* firmware load will happen after pass 1 */
         if (!PORTMOD_CORE_INIT_FLAG_FIRMWARE_LOADED_GET(core_is_initialized) ||
             PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
             _SOC_IF_ERR_EXIT(phymod_core_init(&PM_8x50_FLEXE_GEN2_INFO(pm_info)->int_core_access,
                                               &core_conf,
                                               &core_status));
            if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) || init_flags == 0) {
                PORTMOD_CORE_INIT_FLAG_FIRMWARE_LOADED_SET(core_is_initialized);
                rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                                            pm_info->wb_vars_ids[isCoreInitialized],
                                            &core_is_initialized);
                _SOC_IF_ERR_EXIT(rv);
            }
            if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info) || init_flags == 0) {
                /* Program PM offset at the end of core initialization. */
                _SOC_IF_ERR_EXIT
                    (phymod_timesync_offset_set(&PM_8x50_FLEXE_GEN2_INFO(pm_info)->int_core_access,
                                                PM_8x50_FLEXE_GEN2_INFO(pm_info)->pm_offset));

                PORTMOD_CORE_INIT_FLAG_INITIALZIED_SET(core_is_initialized);
                rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                                            pm_info->wb_vars_ids[isCoreInitialized],
                                            &core_is_initialized);
                _SOC_IF_ERR_EXIT(rv);
            }
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
int pm8x50_flexe_gen2_pm_serdes_core_init(int unit,
                                       int pm_id,
                                       pm_info_t pm_info,
                                       const portmod_port_add_info_t* add_info)
{
    int init_all = 0;
    uint32 flexible_pll_required = 0;
    int rv = SOC_E_NONE;

    SOC_INIT_FUNC_DEFS;

    /* Update per core based database */
    if (add_info->init_config.polarity_overwrite == 1) {
        sal_memcpy(&PM_8x50_FLEXE_GEN2_INFO(pm_info)->polarity,
            &add_info->init_config.polarity[0], sizeof(phymod_polarity_t));
    }
    if (add_info->init_config.lane_map_overwrite == 1) {
        sal_memcpy(&PM_8x50_FLEXE_GEN2_INFO(pm_info)->lane_map,
            &add_info->init_config.lane_map[0], sizeof(phymod_lane_map_t));
    }
    if (add_info->init_config.fw_load_method_overwrite == 1) {
        PM_8x50_FLEXE_GEN2_INFO(pm_info)->fw_load_method =
            add_info->init_config.fw_load_method[0];
    }
    if (add_info->init_config.ref_clk_overwrite == 1) {
        PM_8x50_FLEXE_GEN2_INFO(pm_info)->ref_clk = add_info->init_config.ref_clk;
    }

    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_IS_FLEXIBLE_PLL_GET(unit, pm_info, &flexible_pll_required));
    if (flexible_pll_required) {
        PHYMOD_DEVICE_OP_MODE_FLEXIBLE_PLL_SELECT_SET(PM_8x50_FLEXE_GEN2_INFO(pm_info)->int_core_access.device_op_mode);
    }

    init_all = (!PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) &&
                !PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) &&
                !PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) ? 1 : 0;

    /* probe serdes core */
    if (PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) || init_all) {
        rv = _pm8x50_flexe_gen2_pm_core_probe(unit, pm_info, add_info);
        _SOC_IF_ERR_EXIT(rv);
    }

    /* Return here if caller only request Core Probe */
    if (!(PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) &&
        (PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info))) {
        return (rv);
    }

    /* core config for internal serdes. */
    rv = _pm8x50_flexe_gen2_pm_serdes_core_init(unit, pm_info, add_info);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm8x50_flexe_gen2_pm_port_init(int unit,
                                 int port,
                                 pm_info_t pm_info,
                                 int internal_port,
                                 const portmod_port_add_info_t* add_info,
                                 int enable)
{
    int rv, phy_acc;
    uint32 reg_val, flags;
    uint32 rsv_mask;
    uint32 is_bypassed = 0;
    int port_index;
    uint32 bitmap[1];

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_phy_access_get(unit, port, pm_info, &phy_acc));
    if (internal_port == -1)  {
        _SOC_EXIT_WITH_ERR(SOC_E_PORT, (_SOC_MSG("Invalid internal Port %d"),
                                internal_port));
    }

    if (enable) {
        _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, bitmap));
        _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));
        if (!is_bypassed)
        {
            /* RSV Mask */
            rsv_mask = 0;
            SHR_BITSET(&rsv_mask, 3); /* Receive terminate/code error */
            SHR_BITSET(&rsv_mask, 4); /* CRC error */
            SHR_BITSET(&rsv_mask, 6); /* Truncated/Frame out of Range */
            SHR_BITSET(&rsv_mask, 17); /* RUNT detected*/
            _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_mac_rsv_mask_set(unit, phy_acc, pm_info, rsv_mask));

            /* Init MAC */
            flags = 0;
            if(PORTMOD_PORT_ADD_F_RX_SRIP_CRC_GET(add_info)) {
                flags |= CDMAC_INIT_F_RX_STRIP_CRC;

            }

            if(PORTMOD_PORT_ADD_F_TX_APPEND_CRC_GET(add_info)) {
                flags |= CDMAC_INIT_F_TX_APPEND_CRC;
            }

            if(PORTMOD_PORT_ADD_F_TX_REPLACE_CRC_GET(add_info)) {
                flags |= CDMAC_INIT_F_TX_REPLACE_CRC;
            }

            if(PORTMOD_PORT_ADD_F_TX_PASS_THROUGH_CRC_GET(add_info)) {
            flags |= CDMAC_INIT_F_TX_PASS_THROUGH_CRC_MODE;
            }

            /* MIB counters are cleared in cdmac_init */
            rv = cdmac_init(unit, phy_acc, flags);
            _SOC_IF_ERR_EXIT(rv);
        }

        /* LSS */
        _SOC_IF_ERR_EXIT(READ_CDPORT_FAULT_LINK_STATUSr(unit, phy_acc, &reg_val));
        soc_reg_field_set(unit, CDPORT_FAULT_LINK_STATUSr, &reg_val, REMOTE_FAULTf, 1);
        soc_reg_field_set(unit, CDPORT_FAULT_LINK_STATUSr, &reg_val, LOCAL_FAULTf, 1);
        _SOC_IF_ERR_EXIT(WRITE_CDPORT_FAULT_LINK_STATUSr(unit, phy_acc, reg_val));

        if (!is_bypassed)
        {
            /* Counter MAX size */
            _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_cntmaxsize_set(unit, phy_acc, pm_info, 1518));
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
 *          pm8x50_flexe_gen2_port_soft_reset(unit, port, pinfo, portmodMacSoftResetModeIn, &saved_rx_enablel, &saved_mac_ctrl);
 *          ... here the MAC is in reset ...
 *          pm8x50_flexe_gen2_port_soft_reset(unit, port, pinfo, portmodMacSoftResetModeOut, &saved_rx_enablel, &saved_mac_ctrl);
 *     IN-OUT - get MAC IN reset and then out of reset.
 *          When  reset_mode=INOUT, saved_rx_enable, saved_mac_ctrl can be set to NULL.
 */
int pm8x50_flexe_gen2_port_soft_reset(int unit,
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
    int soft_reset_enable;

    SOC_INIT_FUNC_DEFS;

    /* protect this function from concurrent access */
    if (PM_8x50_FLEXE_GEN2_INFO(pm_info)->portmod_mac_soft_reset) {
        rv = PM_8x50_FLEXE_GEN2_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypeLock);
        _SOC_IF_ERR_EXIT(rv);
    }

    /* Callback soft reset function */
    if (PM_8x50_FLEXE_GEN2_INFO(pm_info)->portmod_mac_soft_reset) {
        rv = PM_8x50_FLEXE_GEN2_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypePre);
        _SOC_IF_ERR_EXIT(rv);
    }

    if (portmodMacSoftResetModeOut != reset_mode) {
        /* reset_mode is portmodMacSoftResetModeIn or portmodMacSoftResetModeInOut */

        rv = cdmac_egress_queue_drain_get(unit, port, &mac_ctrl, &rx_enable);
        _SOC_IF_ERR_EXIT(rv);

        /* return MAC state if required */
        if ((NULL != saved_rx_enable) && (NULL != saved_mac_ctrl)) {
            *saved_rx_enable = rx_enable;
            COMPILER_64_COPY(*saved_mac_ctrl, mac_ctrl);
        }

        _SOC_IF_ERR_EXIT(cdmac_soft_reset_get(unit, port, &soft_reset_enable));
        if (!soft_reset_enable)
        {
            /* Drain cells */
            rv = cdmac_drain_cell_get(unit, port, &drain_cells);
            _SOC_IF_ERR_EXIT(rv);

            /* Start TX FIFO draining */
            rv = cdmac_drain_cell_start(unit, port);
            _SOC_IF_ERR_EXIT(rv);

            /* De-assert SOFT_RESET to let the drain start */
            rv = cdmac_soft_reset_set(unit, port, 0);
            _SOC_IF_ERR_EXIT(rv);

            if (!SAL_BOOT_SIMULATION){
                /* Wait until TX fifo cell count is 0 */
                soc_timeout_init(&to, 250000, 0);
                for (;;) {
                    rv = cdmac_txfifo_cell_cnt_get(unit, port, &cell_count);
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
                rv = cdmac_txfifo_cell_cnt_get(unit, port, &cell_count);
                _SOC_IF_ERR_EXIT(rv);
            }

            /* Stop TX FIFO draining */
            rv = cdmac_drain_cell_stop(unit, port, &drain_cells);
            _SOC_IF_ERR_EXIT(rv);

            /* Put port into SOFT_RESET */
            rv = cdmac_soft_reset_set(unit, port, 1);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

    if(portmodMacSoftResetModeIn != reset_mode) {
        /* reset_mode is portmodMacSoftResetModeOut or portmodMacSoftResetModeInOut */
        if (PM_8x50_FLEXE_GEN2_INFO(pm_info)->portmod_mac_soft_reset) {

            _SOC_IF_ERR_EXIT(cdmac_soft_reset_get(unit, port, &soft_reset_enable));
            if (soft_reset_enable)
            {
                /*
                 * reset credits only if
                 * soft reset would toggle
                 */
                rv = PM_8x50_FLEXE_GEN2_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypeDuring);
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

        soc_reg64_field32_set(unit, CDMAC_CTRLr, &mac_ctrl, SOFT_RESETf, 0); /*make sure restored data wont put mac back in reset*/

        /* Restore cdmac_CTRL to original value */
        rv = cdmac_mac_ctrl_set(unit, port, mac_ctrl);
        _SOC_IF_ERR_EXIT(rv);

        /* Callback soft reset function */
        if (PM_8x50_FLEXE_GEN2_INFO(pm_info)->portmod_mac_soft_reset) {
            rv = PM_8x50_FLEXE_GEN2_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypePost);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

exit:

    if (PM_8x50_FLEXE_GEN2_INFO(pm_info)->portmod_mac_soft_reset) {
        int local_rv = SOC_E_NONE;
        /* unlock function mutex */
        local_rv = PM_8x50_FLEXE_GEN2_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypeUnlock);
        if (local_rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_PORT,(BSL_META_UP(unit, port,
                                                   "ERROR: u=%d p=%d unable to unlock\n"), unit, port));
            return local_rv;
        }
    }

    SOC_FUNC_RETURN;
}

STATIC
int _pm8x50_flexe_gen2_port_mac_drain_soft_reset(int unit, int port)
{
    int rv;
    portmod_drain_cells_t drain_cells;
    uint32 cell_count;
    soc_timeout_t to;
    SOC_INIT_FUNC_DEFS;

    /* Drain cells */
    rv = cdmac_drain_cell_get(unit, port, &drain_cells);
    _SOC_IF_ERR_EXIT(rv);

    /* Start TX FIFO draining */
    rv = cdmac_drain_cell_start(unit, port);
    _SOC_IF_ERR_EXIT(rv);

    /* De-assert SOFT_RESET to let the drain start */
    rv = cdmac_soft_reset_set(unit, port, 0);
    _SOC_IF_ERR_EXIT(rv);
    
    if (!SAL_BOOT_SIMULATION){
        /* Wait until TX fifo cell count is 0 */
        soc_timeout_init(&to, 250000, 0);
        for (;;) {
            rv = cdmac_txfifo_cell_cnt_get(unit, port, &cell_count);
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
        rv = cdmac_txfifo_cell_cnt_get(unit, port, &cell_count);
        _SOC_IF_ERR_EXIT(rv);
    }
    /* Stop TX FIFO draining */
    rv = cdmac_drain_cell_stop(unit, port, &drain_cells);
    _SOC_IF_ERR_EXIT(rv);

    /* Put port into SOFT_RESET */
    rv = cdmac_soft_reset_set(unit, port, 1);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm8x50_flexe_gen2_port_rx_restore_mac_out_of_reset(int unit, int port, int rx_enable)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    /* Enable RX, de-assert SOFT_RESET */
    rv = cdmac_egress_queue_drain_rx_en(unit, port, rx_enable);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

/*
 *Function:
 *  _pm8x50_flexe_gen2_port_phy_txpi_mode_set
 *
 *Purpose:
 *  Set PMD TXPI mode: normal or external PD
 *  This is a port level API
*/
int _pm8x50_flexe_gen2_port_phy_txpi_mode_set(int unit, int port, pm_info_t pm_info, uint32 mode)
{
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    phymod_txpi_config_t config;
    SOC_INIT_FUNC_DEFS;

    config.enable = mode ? 1 : 0;
    config.mode   = mode;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(phymod_phy_txpi_config_set(&phy_access, &config));


exit:
    SOC_FUNC_RETURN;
}

/*
 *Function:
 *  _pm8x50_flexe_gen2_txpi_lane_select_set
 *
 *Purpose:
 *  Set Portmacro TXPI lane select
 *  Defaults to using TXPI for it's own lane
 *  This is a core level API
*/
int _pm8x50_flexe_gen2_txpi_lane_select_set(int unit, int port, pm_info_t pm_info, uint32_t *lane_select, int physical_lane)
{
    int phy_acc;
    uint32 reg_val = 0;
    uint32 bitmap;
    int first_index;
    soc_field_t tx_pi_lane_sel[PM8X50_FLEXE_GEN2_LANES_PER_CORE] = {TXPI_LANE_SELECTION_0f, TXPI_LANE_SELECTION_1f,
                                                         TXPI_LANE_SELECTION_2f, TXPI_LANE_SELECTION_3f,
                                                         TXPI_LANE_SELECTION_4f, TXPI_LANE_SELECTION_5f,
                                                         TXPI_LANE_SELECTION_6f, TXPI_LANE_SELECTION_7f};

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_phy_access_get(unit, port, pm_info, &phy_acc));
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &first_index, &bitmap));


    _SOC_IF_ERR_EXIT(READ_CDPORT_TXPI_LANE_SELECTIONr(unit, phy_acc, &reg_val));
    soc_reg_field_set(unit, CDPORT_TXPI_LANE_SELECTIONr, &reg_val, tx_pi_lane_sel[physical_lane], lane_select[physical_lane]);

    _SOC_IF_ERR_EXIT(WRITE_CDPORT_TXPI_LANE_SELECTIONr(unit, phy_acc, reg_val));

exit:
    SOC_FUNC_RETURN;
}

/*Add new port*/
int pm8x50_flexe_gen2_port_attach(int unit, int port, pm_info_t pm_info,
                              const portmod_port_add_info_t* add_info)
{
    int port_index = -1, pm_is_active = 0, is_pcs_bypassed = 0;
    int port_flexe_mode = 0, port_otn_mode = 0/*, is_flexe_tvco_26g_required = 0*/;
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
    uint32 *pcs_lane_map;
    uint32 txpi_lane_select[PM8X50_FLEXE_GEN2_PORTS_PER_CORE]={0};
    int pcs_lane;

    SOC_INIT_FUNC_DEFS;

    /* Get the first phy related to this port */
    PORTMOD_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
    PORTMOD_PBMP_AND(port_phys_in_pm, PM_8x50_FLEXE_GEN2_INFO(pm_info)->phys);

    ii = 0;
    PORTMOD_PBMP_ITER(PM_8x50_FLEXE_GEN2_INFO(pm_info)->phys, phy) {
        if(PORTMOD_PBMP_MEMBER(port_phys_in_pm, phy)) {
            rv = PM8x50_FLEXE_GEN2_LANE2PORT_SET(unit, pm_info, ii, port);
            _SOC_IF_ERR_EXIT(rv);
            port_index = (port_index == -1 ? ii : port_index);
        }
        ii++;
    }

    is_pcs_bypassed = PORTMOD_PORT_ADD_F_PCS_BYPASSED_GET(add_info) ? 1 : 0;
    rv = PM8x50_FLEXE_GEN2_PORT_IS_PCS_BYPASSED_SET(unit, pm_info, is_pcs_bypassed, port_index);
    _SOC_IF_ERR_EXIT(rv);

    port_flexe_mode = PORTMOD_PORT_ADD_F_FLEXE_CONFIG_GET(add_info) ? 1 : 0;
    rv = PM8x50_FLEXE_GEN2_PORT_FLEXE_MODE_SET(unit, pm_info, port_flexe_mode, port_index);
    _SOC_IF_ERR_EXIT(rv);

    port_otn_mode = PORTMOD_PORT_ADD_F_OTN_CONFIG_GET(add_info) ? 1 : 0;
    rv = PM8x50_FLEXE_GEN2_PORT_OTN_MODE_SET(unit, pm_info, port_otn_mode, port_index);
    _SOC_IF_ERR_EXIT(rv);

    /* initalize port */
    rv = _pm8x50_flexe_gen2_pm_port_init(unit, port, pm_info, port_index, add_info, 1);
    _SOC_IF_ERR_EXIT(rv);

    /* initialze phy */
    _SOC_IF_ERR_EXIT(phymod_phy_init_config_t_init(&init_config));
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    my_i = 0, usr_cfg_idx = 0;
    found_first_active_lane = 0;
    for(ii=0; ii<PM8X50_FLEXE_GEN2_LANES_PER_CORE; ii++) {
        rv = PM8x50_FLEXE_GEN2_LANE2PORT_GET(unit, pm_info, ii, &port_i);
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

        _SOC_IF_ERR_EXIT(phymod_phy_media_type_tx_get(&phy_access, phymodMediaTypeChipToChip, &init_config.tx[my_i]));

        if (PORTMOD_PORT_ADD_F_SKIP_SPEED_INIT_GET(add_info)) {
            /* If PORTMOD_PORT_ADD_F_SKIP_SPEED_INIT is set, set tx_taps to NRZ default */
            signalling_mode = phymodSignallingMethodNRZ;
        } else {
            _pm8x50_flexe_gen2_signalling_method_get(is_pcs_bypassed, speed_config, &signalling_mode);
        }

        if (PORTMOD_USER_SET_TX_PREEMPHASIS_BY_CONFIG_GET(add_info->init_config.tx_params_user_flag[usr_cfg_idx])) {
            init_config.tx[my_i].pre = add_info->init_config.tx_params[usr_cfg_idx].pre;
            init_config.tx[my_i].main = add_info->init_config.tx_params[usr_cfg_idx].main;
            init_config.tx[my_i].post = add_info->init_config.tx_params[usr_cfg_idx].post;
            init_config.tx[my_i].pre2 = add_info->init_config.tx_params[usr_cfg_idx].pre2;
            init_config.tx[my_i].post2 = add_info->init_config.tx_params[usr_cfg_idx].post2;
            init_config.tx[my_i].post3 = add_info->init_config.tx_params[usr_cfg_idx].post3;
            init_config.tx[my_i].tap_mode = add_info->init_config.tx_params[usr_cfg_idx].tap_mode;
            init_config.tx[my_i].sig_method = add_info->init_config.tx_params[usr_cfg_idx].sig_method;
        } else {
            /* Need to get the default values for tx taps */
            _SOC_IF_ERR_EXIT(phymod_phy_tx_taps_default_get(&phy_access, signalling_mode, &init_config.tx[my_i]));
        }

        usr_cfg_idx++;
        my_i++;
    }

    /* Set PMD TXPI mode */
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_phy_txpi_mode_set(unit, port, pm_info, add_info->init_config.txpi_mode));

    /* Set txpi lane select when port is added */
    pcs_lane_map = (uint32 *)add_info->init_config.lane_map[0].lane_map_tx;
    for (pcs_lane = port_index; pcs_lane < (port_index + add_info->interface_config.port_num_lanes); pcs_lane++) {
         txpi_lane_select[pcs_lane_map[pcs_lane]] = port_index;
         _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_txpi_lane_select_set(unit, port, pm_info, txpi_lane_select, pcs_lane_map[pcs_lane]));
    }

    _SOC_IF_ERR_EXIT(phymod_phy_init(&phy_access, &init_config));

    /* MAC SOFT_RESET will be released within pm8x50_flexe_gen2_port_speed_config_set */
    if (!PORTMOD_PORT_ADD_F_SKIP_SPEED_INIT_GET(add_info)) {
        rv = pm8x50_flexe_gen2_port_speed_config_set(unit, port, pm_info, speed_config);
        _SOC_IF_ERR_EXIT(rv);
    } else if (!is_pcs_bypassed) {
       _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_soft_reset(unit, port, pm_info, portmodMacSoftResetModeIn_Out, NULL, NULL));
    }

    /* Update WB DB */
    rv = PM8x50_FLEXE_GEN2_IS_ACTIVE_GET(unit, pm_info, &pm_is_active);
    _SOC_IF_ERR_EXIT(rv);
    if (!pm_is_active) {
        pm_is_active = 1;
        rv = PM8x50_FLEXE_GEN2_IS_ACTIVE_SET(unit, pm_info, pm_is_active);
        _SOC_IF_ERR_EXIT(rv);
    }

    rv = PM8x50_FLEXE_GEN2_AN_MODE_SET(unit, pm_info, init_an_mode, port_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = PM8x50_FLEXE_GEN2_MAX_SPEED_SET(unit, pm_info, add_info->interface_config.max_speed, port_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = PM8x50_FLEXE_GEN2_TIMESYNC_CONFIG_SET(unit, pm_info, timesync_config_default, port_index);
    _SOC_IF_ERR_EXIT(rv);

    if (add_info->init_config.parity_enable) {
        _pm8x50_flexe_gen2_port_interrupt_all_enable_set(unit, port, 1);

    }

exit:
    SOC_FUNC_RETURN;
}

/*get port cores' phymod access*/
int pm8x50_flexe_gen2_pm_core_info_get(int unit, pm_info_t pm_info, int phyn,
                                    portmod_pm_core_info_t* core_info)
{
    SOC_INIT_FUNC_DEFS;

    if(phyn < 0) {
        phyn = PM8X50_FLEXE_GEN2_MAX_NUM_PHYS;
    }

    core_info->ref_clk = PM_8x50_FLEXE_GEN2_INFO(pm_info)->ref_clk;
    sal_memcpy(&core_info->lane_map, &(PM_8x50_FLEXE_GEN2_INFO(pm_info)->lane_map),
               sizeof(phymod_lane_map_t));

    SOC_FUNC_RETURN;
}

/*Get PM phys.*/
int pm8x50_flexe_gen2_pm_phys_get(int unit, pm_info_t pm_info, portmod_pbmp_t* phys)
{
    SOC_INIT_FUNC_DEFS;
    PORTMOD_PBMP_ASSIGN(*phys, PM_8x50_FLEXE_GEN2_INFO(pm_info)->phys);
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_callback_unregister(int unit, int port, pm_info_t pm_info)
{
    int port_index;
    uint32 bitmap;
    int is_400g_an = 0;
    phymod_an_mode_type_t an_mode;

    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, &bitmap));

    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_ENABLE_GET(unit, pm_info, &is_400g_an, port_index));
    if (is_400g_an) {
        is_400g_an = 0;
        _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_ENABLE_SET(unit, pm_info, is_400g_an, port_index));
        an_mode = phymod_AN_MODE_NONE;
        _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_AN_MODE_SET(unit, pm_info, an_mode, port_index));
    }

    _SOC_IF_ERR_EXIT(portmod_port_periodic_callback_unregister(unit, port));

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm8x50_flexe_gen2_port_pll_power_down(int unit, int port, pm_info_t pm_info)
{
    uint32 port_lane_mask = 0, port_physical_lane_mask = 0;
    uint8 pll0_active_lane_bm, pll1_active_lane_bm, pll0_adv_lane_bm, pll1_adv_lane_bm;
    uint8 pll0_active_lane_bitmap = 0, pll1_active_lane_bitmap = 0, pll0_adv_lane_bitmap = 0, pll1_adv_lane_bitmap = 0;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    uint32 *lane_map_tx = NULL;
    int nof_phys, i;
    uint32 p2l_lane_map[PM8X50_FLEXE_GEN2_LANES_PER_CORE] = {0};
    uint8 lane_mask_ins = 0;
    uint32 flexible_pll_required = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_IS_FLEXIBLE_PLL_GET(unit, pm_info, &flexible_pll_required));

    if (flexible_pll_required) {
        port_lane_mask = phy_access.access.lane_mask;
        _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PLL0_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll0_active_lane_bm));
        _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PLL1_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll1_active_lane_bm));
        _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PLL0_ADV_LANE_BITMAP_GET(unit, pm_info, &pll0_adv_lane_bm));
        _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PLL1_ADV_LANE_BITMAP_GET(unit, pm_info, &pll1_adv_lane_bm));

        /* Get the port physical lane mask */
        PM8x50_FLEXE_GEN2_PORT_PHYISCAL_LANE_MASK_GET(pm_info, port_lane_mask, port_physical_lane_mask);

        /* Get physical pll bitmap */
        lane_map_tx = PM_8x50_FLEXE_GEN2_INFO(pm_info)->lane_map.lane_map_tx;
        for (i = 0; i < PM8X50_FLEXE_GEN2_LANES_PER_CORE; i++) {
             pll0_active_lane_bitmap |= (pll0_active_lane_bm >> i & 0x1) << lane_map_tx[i];
             pll1_active_lane_bitmap |= (pll1_active_lane_bm >> i & 0x1) << lane_map_tx[i];
             pll0_adv_lane_bitmap |= (pll0_adv_lane_bm >> i & 0x1) << lane_map_tx[i];
             pll1_adv_lane_bitmap |= (pll1_adv_lane_bm >> i & 0x1) << lane_map_tx[i];
        }

        /* Power down unused PLLs, per PMD instance */
        PM8x50_FLEXE_GEN2_LANE_MAP_TRANSLATE(lane_map_tx, p2l_lane_map);
        for (i = 0; i < MAX_NUM_PMD_INS; i++) {
             /* Get the lane mask of the instance */
             lane_mask_ins = 0x3 << (i * MAX_NUM_LANES_PER_PMD_INS);
             phy_access.access.lane_mask = 0x1 << p2l_lane_map[i * MAX_NUM_LANES_PER_PMD_INS];
             /* Check if the instance is used by the port */
             if (port_physical_lane_mask & lane_mask_ins) {
                 /* PLL0 is free in this instance*/
                 if (!(pll0_active_lane_bitmap & lane_mask_ins) &&
                     !(pll0_adv_lane_bitmap & lane_mask_ins)) {
                     phy_access.access.pll_idx = 0;
                     _SOC_IF_ERR_EXIT(phymod_phy_pll_pwrdn(&phy_access, phy_access.access.pll_idx, 1));
                 }

                 /* PLL1 is free in this instance */
                 if (!(pll1_active_lane_bitmap & lane_mask_ins) &&
                     !(pll1_adv_lane_bitmap & lane_mask_ins)) {
                     phy_access.access.pll_idx = 1;
                     _SOC_IF_ERR_EXIT(phymod_phy_pll_pwrdn(&phy_access, phy_access.access.pll_idx, 1));
                 }
             }
        }
    } else {
        /* Fixed PLL mode: phymod will ignore the pll_index and power down pll for each lane */
        _SOC_IF_ERR_EXIT(phymod_phy_pll_pwrdn(&phy_access, 0, 1));
    }

exit:
    SOC_FUNC_RETURN;
}

/*Port remove in PM level*/
int pm8x50_flexe_gen2_port_detach(int unit, int port, pm_info_t pm_info)
{
    int enable, invalid_port = -1, tmp_port, i = 0, flags = 0, pm_id;
    int is_last_one = TRUE, port_index = -1, fec_null = 0;
    const uint32 is_active = 0;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, is_400g_an = 0;
    portmod_phy_timesync_config_t config;
    phymod_an_mode_type_t an_mode;
    uint32 is_bypassed, is_other_bypassed = 0;
    int other_port_index;
    uint32 other_bitmap[1];
    int is_last_non_bypassed;
    uint8_t pll0_active_lane_bm, pll1_active_lane_bm;
    uint8_t pll0_adv_lane_bm, pll1_adv_lane_bm;
    uint32 port_lane_mask = 0;
    uint32 bitmap = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(portmod_port_periodic_callback_unregister(unit, port));

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    port_lane_mask = phy_access.access.lane_mask;
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_enable_get(unit, port, pm_info, flags, &enable));
    if (enable)
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("can't detach active port %d"), port));

    /* Disable 1588 */
    config.flags = 0;
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_timesync_config_set(unit, port, pm_info, &config));

    /* Disable PCS for the port */
    _SOC_IF_ERR_EXIT(phymod_phy_pcs_enable_set(&phy_access, 0));

     /* Clear the active and adv lane bitmap for this port */
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PLL0_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll0_active_lane_bm));
     _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PLL1_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll1_active_lane_bm));
     _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PLL0_ADV_LANE_BITMAP_GET(unit, pm_info, &pll0_adv_lane_bm));
     _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PLL1_ADV_LANE_BITMAP_GET(unit, pm_info, &pll1_adv_lane_bm));
     pll0_active_lane_bm &= ~port_lane_mask;
     pll1_active_lane_bm &= ~port_lane_mask;
     pll0_adv_lane_bm &= ~port_lane_mask;
     pll1_adv_lane_bm &= ~port_lane_mask;
     _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PLL0_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll0_active_lane_bm));
     _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PLL1_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll1_active_lane_bm));
     _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PLL0_ADV_LANE_BITMAP_SET(unit, pm_info, pll0_adv_lane_bm));
     _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PLL1_ADV_LANE_BITMAP_SET(unit, pm_info, pll1_adv_lane_bm));

     /* Power down unused PLLs. */
    _SOC_IF_ERR_EXIT(
        _pm8x50_flexe_gen2_port_pll_power_down(unit, port, pm_info));

    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));

    is_last_non_bypassed = !is_bypassed;

    for (i = 0 ; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
        _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_LANE2PORT_GET(unit, pm_info, i, &tmp_port));
        if (tmp_port == port) {
            port_index = (port_index == -1 ? i : port_index);
            _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_LANE2PORT_SET(unit, pm_info, i, invalid_port));
            _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_LANE2FEC_SET(unit, pm_info, i, fec_null));
        } else if (tmp_port != invalid_port) {
            is_last_one = FALSE;
            if (!is_bypassed) {
                /* for DNX only - check if this is last non bypassed (not ILKN) port in order to reset MAC before powering down its clock */
                _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, tmp_port, pm_info, &other_port_index, other_bitmap));
                _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_other_bypassed, other_port_index));
                if (!is_other_bypassed) {
                    is_last_non_bypassed = FALSE;
                }
            }
        }
    }

    if (port_index == -1)
        _SOC_EXIT_WITH_ERR(SOC_E_PORT, (_SOC_MSG("Port %d wasn't found"), port));

    /*clear 400G AN enable Warmboot variable */
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_ENABLE_SET(unit, pm_info, is_400g_an, port_index));
    an_mode = phymod_AN_MODE_NONE;
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_AN_MODE_SET(unit, pm_info, an_mode, port_index));

    /*deinit PM in case of last one*/
    if (is_last_one) {
        _SOC_IF_ERR_EXIT(portmod_port_pm_id_get(unit, port, &pm_id));
        _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_pm_enable(unit, pm_id, pm_info, 0, 0));
        _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_IS_ACTIVE_SET(unit, pm_info, is_active));
        _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_IS_CORE_INITIALIZED_SET(unit, pm_info, is_active));

        PM_8x50_FLEXE_GEN2_INFO(pm_info)->int_core_access.type = phymodDispatchTypeInvalid;
    }

    if (!is_last_one && is_last_non_bypassed) {
        /* DNX only - reset MAC if last NON bypassed port is removed */
        int pm_enable_flags = 0;
        PORTMOD_PM_ENABLE_MAC_ONLY_SET(pm_enable_flags);
        _SOC_IF_ERR_EXIT(portmod_port_pm_id_get(unit, port, &pm_id));
        _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_pm_enable(unit, pm_id, pm_info, pm_enable_flags, 0));

    }

exit:
    SOC_FUNC_RETURN;
}

/*Port replace in PM level*/
int pm8x50_flexe_gen2_port_replace(int unit, int port, pm_info_t pm_info, int new_port)
{
    int i, tmp_port;
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_periodic_callback_unregister(unit, port));
    /* replace old port with new port */
    for(i = 0; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
        rv = PM8x50_FLEXE_GEN2_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
        _SOC_IF_ERR_EXIT(rv);
        if (tmp_port == port) {
            rv = PM8x50_FLEXE_GEN2_LANE2PORT_SET(unit, pm_info, i, new_port);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

exit:
    SOC_FUNC_RETURN;
}


/*Port enable*/
int pm8x50_flexe_gen2_port_enable_set(int unit, int port, pm_info_t pm_info,
                                   int flags, int enable)
{
    int actual_flags = flags;
    int cdmac_flags = 0, port_index;
    uint32 is_bypassed = 0, bitmap[1];
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys = 0;
    int rx_enable;
    uint64 mac_ctrl;
    int soft_reset_enable;

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
        _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port,
                                     pm_info, &params, 1, &phy_access,
                                     &nof_phys, NULL));
    }
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));

    if(enable){
        if((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) && (!is_bypassed)) {

            if(!PM_8x50_FLEXE_GEN2_INFO(pm_info)->portmod_mac_soft_reset)
            {
                _SOC_IF_ERR_EXIT(cdmac_enable_set(unit, port, cdmac_flags, 1));
            }
            else
            {
                /* Enable CDMAC RX,TX, skip SOFT_RESET */
                _SOC_IF_ERR_EXIT(cdmac_tx_enable_set (unit, port, 1));
                _SOC_IF_ERR_EXIT(cdmac_rx_enable_set (unit, port, 1));
                /* Get MAC CTRL */
                _SOC_IF_ERR_EXIT(cdmac_egress_queue_drain_get(unit, port, &mac_ctrl, &rx_enable));

                _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_soft_reset(unit, port, pm_info, portmodMacSoftResetModeOut,
                                                        &rx_enable, &mac_ctrl));
            }
        }

        if(PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) {
           _SOC_IF_ERR_EXIT
               (phymod_port_enable_set(&phy_access, enable));
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
            if(!PM_8x50_FLEXE_GEN2_INFO(pm_info)->portmod_mac_soft_reset)
            {
                _SOC_IF_ERR_EXIT(cdmac_enable_set(unit, port, cdmac_flags, 0));
            }
            else
            {
                /* Disable CDMAC RX,TX, skip SOFT_RESET */
               _SOC_IF_ERR_EXIT(cdmac_soft_reset_get(unit, port, &soft_reset_enable));
                if (!soft_reset_enable)
                {
                    /* not in Soft reset yet */
                 _SOC_IF_ERR_EXIT(cdmac_tx_enable_set (unit, port, 1));
                 _SOC_IF_ERR_EXIT(cdmac_rx_enable_set (unit, port, 0));
                 _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_soft_reset(unit, port, pm_info, portmodMacSoftResetModeIn,
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

int pm8x50_flexe_gen2_port_enable_get(int unit, int port, pm_info_t pm_info,
                                   int flags, int* enable)
{
    int nof_phys = 0, port_index;
    int phy_enable = 1, mac_enable = 1;
    int mac_rx_enable = 0, mac_tx_enable = 0;
    uint32 is_bypassed = 0, bitmap[1];
    int actual_flags = flags;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    phymod_phy_tx_lane_control_t tx_control = phymodTxSquelchOn;
    phymod_phy_rx_lane_control_t rx_control = phymodRxSquelchOn;
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
        _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port,
                                     pm_info, &params, 1, &phy_access,
                                     &nof_phys, NULL));
    }

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));

    if (PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) {
        _SOC_IF_ERR_EXIT(phymod_phy_tx_lane_control_get(&phy_access,
                                                        &tx_control));
        _SOC_IF_ERR_EXIT(phymod_phy_rx_lane_control_get(&phy_access,
                                                        &rx_control));

        /* TBD - set phy_enable to 1 for SIM */
        phy_enable = 0;

        if (PORTMOD_PORT_ENABLE_RX_GET(actual_flags)) {
            phy_enable |= (rx_control == phymodRxSquelchOn) ? 0 : 1;
        }
        if (PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) {
            phy_enable |= (tx_control == phymodTxSquelchOn) ? 0 : 1;
        }
    }

    if ((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) && (!is_bypassed)) {
        mac_enable = 0;

        if (PORTMOD_PORT_ENABLE_RX_GET(actual_flags)) {
            _SOC_IF_ERR_EXIT(cdmac_enable_get(unit, port,
                                              CDMAC_ENABLE_SET_FLAGS_RX_EN,
                                              &mac_rx_enable));
            mac_enable |= (mac_rx_enable)? 1: 0;
        }

        if (PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) {
            _SOC_IF_ERR_EXIT(cdmac_enable_get(unit, port,
                                              CDMAC_ENABLE_SET_FLAGS_TX_EN,
                                              &mac_tx_enable));
            mac_enable |= (mac_tx_enable)? 1: 0;
        }
    }

    *enable = (mac_enable && phy_enable);

exit:
    SOC_FUNC_RETURN;
}


STATIC
int _pm8x50_flexe_gen2_fec_lanebitmap_get(int unit,
                                       pm_info_t pm_info,
                                       uint8 *rs528_bitmap,
                                       uint8 *rs544_bitmap,
                                       uint8 *rs272_bitmap)
{
    int i, rv;
    portmod_fec_t tmp_fec;

    SOC_INIT_FUNC_DEFS;

    *rs528_bitmap = 0;
    *rs544_bitmap = 0;
    *rs272_bitmap = 0;
    /* Get RS528, RS544, RS272 usage bitmap from WB */
    for (i = 0 ; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++){
        rv = PM8x50_FLEXE_GEN2_LANE2FEC_GET(unit, pm_info, i, &tmp_fec);
        _SOC_IF_ERR_EXIT(rv);
        if (tmp_fec == PORTMOD_PORT_PHY_FEC_RS_FEC) {
            *rs528_bitmap |= 1 << i;
        } else if ((tmp_fec == PORTMOD_PORT_PHY_FEC_RS_544) ||
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
int pm8x50_flexe_gen2_pm_speed_config_validate(int unit,
                                            int pm_id,
                                            pm_info_t pm_info,
                                            const portmod_pbmp_t* ports,
                                            int flag,
                                            portmod_pm_vco_setting_t* vco_setting)
{
    int i, rv = 0;
    portmod_vco_type_t vco;
    int is_pcs_bypass;
    uint8_t rs528_bm = 0, rs544_bm = 0, rs272_bm = 0, affected_bm = 0;
    int nof_phys, port;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    SOC_INIT_FUNC_DEFS;

    /* Get RS FEC usage bitmaps from WB */
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_fec_lanebitmap_get(unit, pm_info, &rs528_bm, &rs544_bm, &rs272_bm));

    /* Remove the input ports from FEC usage bitmaps*/
    PORTMOD_PBMP_ITER(*ports, port) {
        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
        _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                    &params, 1, &phy_access, &nof_phys, NULL));
        rs528_bm &= ~phy_access.access.lane_mask;
        rs544_bm &= ~phy_access.access.lane_mask;
        rs272_bm &= ~phy_access.access.lane_mask;
    }

    /* Get the VCOs required by the input speed config list */
    for (i = 0; i < vco_setting->num_speeds; i++) {
        vco = portmodVCOInvalid;
        rv = _pm8x50_flexe_gen2_port_speed_config_to_vco_get(&(vco_setting->speed_config_list[i]),
                                                  vco_setting->speed_for_pcs_bypass_port[i],
                                                  &vco);
        if (rv == SOC_E_CONFIG) {
            _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                      (_SOC_MSG("Can not support speed: %d num_lane: %d fec: %d"),
                      vco_setting->speed_config_list[i].speed,
                      vco_setting->speed_config_list[i].num_lane,
                      vco_setting->speed_config_list[i].fec));
        }

        is_pcs_bypass = vco_setting->speed_for_pcs_bypass_port[i];
        if (!is_pcs_bypass) {
            if ((vco_setting->speed_config_list[i].fec == PORTMOD_PORT_PHY_FEC_RS_272) ||
                (vco_setting->speed_config_list[i].fec == PORTMOD_PORT_PHY_FEC_RS_272_2XN)) {
                _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_lanebitmap_set(vco_setting->port_starting_lane_list[i],
                                                        vco_setting->speed_config_list[i].num_lane,
                                                        &rs272_bm));
            } else if ((vco_setting->speed_config_list[i].fec == PORTMOD_PORT_PHY_FEC_RS_544) ||
                       (vco_setting->speed_config_list[i].fec == PORTMOD_PORT_PHY_FEC_RS_544_2XN)) {
                _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_lanebitmap_set(vco_setting->port_starting_lane_list[i],
                                                        vco_setting->speed_config_list[i].num_lane,
                                                        &rs544_bm));
            } else if (vco_setting->speed_config_list[i].fec == PORTMOD_PORT_PHY_FEC_RS_FEC) {
                _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_lanebitmap_set(vco_setting->port_starting_lane_list[i],
                                                        vco_setting->speed_config_list[i].num_lane,
                                                        &rs528_bm));
            }
        }

    }

    /* Validate FEC settings for RS528, RS544 and RS272 */
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_fec_validate(unit, rs528_bm, rs544_bm, rs272_bm, &affected_bm));

exit:
    SOC_FUNC_RETURN;
}

/*Port speed validation.*/
int pm8x50_flexe_gen2_port_speed_config_validate(int unit,
                                              int port,
                                              pm_info_t pm_info,
                                              const portmod_speed_config_t* speed_config,
                                              portmod_pbmp_t* affected_pbmp)
{
    int rv, i, tmp_port, port_num_lanes, port_index, port_is_pcs_bypassed;
    uint32_t port_lane_mask[1];
    uint8_t affected_lane_bitmap = 0;
    uint8_t rs528_bm = 0, rs544_bm = 0, rs272_bm = 0;
    portmod_vco_type_t required_vco = portmodVCOInvalid;
    uint32 flexible_pll_required = 0;

    SOC_INIT_FUNC_DEFS;

    /* clear the affected_pbmp first */
    PORTMOD_PBMP_CLEAR(*affected_pbmp);

    /* Get lane_mask and number of lanes for the port from DB */
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, port_lane_mask));
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &port_is_pcs_bypassed, port_index));
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_IS_FLEXIBLE_PLL_GET(unit, pm_info, &flexible_pll_required));

    /* Verify and get the required VCO */
    rv = _pm8x50_flexe_gen2_port_speed_config_to_vco_get(speed_config, port_is_pcs_bypassed, &required_vco);
    if (rv == SOC_E_CONFIG) {
        _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                  (_SOC_MSG("Speed config is not supported")));
    }

    if ((required_vco == portmodVCO20P625G) && flexible_pll_required) {
        _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                  (_SOC_MSG("VCO 20G is not supported for flexible PLL mode")));
    }

    /* Check speed_config->lane_num against lane number stored in DB */
    if (!port_is_pcs_bypassed)
    {
        PORTMOD_BIT_COUNT(port_lane_mask[0], port_num_lanes);
        if (port_num_lanes != speed_config->num_lane) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Invalid lane number request on exising logical port.")));
        }
        /* Check FEC settings in MPP0 and MPP1 */
        /* Get RS528, RS544, RS272 usage bitmap from WB */
        _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_fec_lanebitmap_get(unit, pm_info, &rs528_bm, &rs544_bm, &rs272_bm));

        rs528_bm &= ~port_lane_mask[0];
        rs544_bm &= ~port_lane_mask[0];
        rs272_bm &= ~port_lane_mask[0];
        if (speed_config->fec == PORTMOD_PORT_PHY_FEC_RS_FEC) {
            rs528_bm |= port_lane_mask[0];
        } else if ((speed_config->fec == PORTMOD_PORT_PHY_FEC_RS_544) ||
                   (speed_config->fec == PORTMOD_PORT_PHY_FEC_RS_544_2XN)) {
            rs544_bm |= port_lane_mask[0];
        } else if ((speed_config->fec == PORTMOD_PORT_PHY_FEC_RS_272) ||
                   (speed_config->fec == PORTMOD_PORT_PHY_FEC_RS_272_2XN)) {
            rs272_bm |= port_lane_mask[0];
        }

        /* Validate FEC settings for RS528, RS544 and RS272 */
        rv = _pm8x50_flexe_gen2_fec_validate(unit, rs528_bm, rs544_bm, rs272_bm, &affected_lane_bitmap);
        if (rv != SOC_E_NONE) {
            /* If FEC validate fails, return affected port bit map */
            affected_lane_bitmap &= ~port_lane_mask[0];
            for(i = 0 ; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++){
                if ((affected_lane_bitmap >> i) & 1) {
                    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_LANE2PORT_GET(unit, pm_info, i, &tmp_port));
                    PORTMOD_PBMP_PORT_ADD(*affected_pbmp, tmp_port);
                }
            }
            return rv;
        }
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm8x50_flexe_gen2_port_vco_reconfig(int unit, int port, pm_info_t pm_info,
                                         portmod_vco_type_t required_vco, phymod_phy_pll_state_t old_pll_state,
                                         phymod_phy_pll_state_t *new_pll_state)
{
    uint32 pll_index = 0;
    uint32 required_pll_div = phymod_TSCBH_PLL_DIVNONE;
    uint32 current_pll0_div = phymod_TSCBH_PLL_DIVNONE, current_pll1_div = phymod_TSCBH_PLL_DIVNONE;
    uint32 pll_pwrdn = 0, pll0_pwrdn = 0, pll1_pwrdn = 0;
    uint32 *lane_map_tx = NULL;
    int nof_phys, i, port_is_pcs_bypassed = 0, port_index;
    uint32 port_lane_mask = 0, port_physical_lane_mask = 0;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int temp_lane_mask, temp_pll0_lanes_bitmap, temp_pll1_lanes_bitmap;
    uint32_t p2l_lane_map[PM8X50_FLEXE_GEN2_LANES_PER_CORE] = {0};
    phymod_phy_pll_state_t phy_pll_state;
    int pll0_is_free, pll1_is_free;
    uint32 flexible_pll_required = 0;
    uint8 lane_mask_ins = 0;
    uint32 bitmap;
    int port_otn_mode = 0;
    SOC_INIT_FUNC_DEFS;

    sal_memset(new_pll_state, 0, sizeof(phymod_phy_pll_state_t));
    sal_memset(&phy_pll_state, 0, sizeof(phymod_phy_pll_state_t));

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    port_lane_mask = phy_access.access.lane_mask;
    port_is_pcs_bypassed = PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy_access.device_op_mode);
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_IS_FLEXIBLE_PLL_GET(unit, pm_info, &flexible_pll_required));

    /* check if in OTN mode */
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PORT_OTN_MODE_GET(unit, pm_info, &port_otn_mode, port_index));
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_vco_to_pll_get(PM_8x50_FLEXE_GEN2_INFO(pm_info)->ref_clk, required_vco, &required_pll_div, port_otn_mode));

    if (flexible_pll_required) {
        if (!port_is_pcs_bypassed) {
            switch (required_vco) {
                case portmodVCO26P562G:
                    pll_index = 0;
                    break;
                case portmodVCO25P781G:
                    pll_index = 1;
                    break;
                default:
                    _SOC_EXIT_WITH_ERR(SOC_E_CONFIG, (_SOC_MSG("Invalid VCO %d for pll flexible mode"), required_vco));
            }
            /* Reconfigure PLL */
            phy_access.access.pll_idx = pll_index;
            _SOC_IF_ERR_EXIT(phymod_phy_pll_powerdown_get(&phy_access, pll_index, &pll_pwrdn));
            if (pll_pwrdn) {
                _SOC_IF_ERR_EXIT(phymod_phy_pll_pwrdn(&phy_access, pll_index, 0));
            }
            _SOC_IF_ERR_EXIT(
                phymod_phy_pll_reconfig(&phy_access, pll_index, required_pll_div, PM_8x50_FLEXE_GEN2_INFO(pm_info)->ref_clk));
            /* Update pll lane bitmap */
            sal_memcpy(new_pll_state, &old_pll_state, sizeof(phymod_phy_pll_state_t));
            if (pll_index == 0) {
                new_pll_state->pll0_lanes_bitmap |= port_lane_mask;
            } else {
                new_pll_state->pll1_lanes_bitmap |= port_lane_mask;
            }
        } else {
            /* For PCS bypass */
            lane_map_tx = PM_8x50_FLEXE_GEN2_INFO(pm_info)->lane_map.lane_map_tx;
            /* Get pll physical lanes bitmap */
            for (i = 0; i < PM8X50_FLEXE_GEN2_LANES_PER_CORE; i++) {
                 phy_pll_state.pll0_lanes_bitmap |= ((old_pll_state.pll0_lanes_bitmap >> i) & 0x1) << lane_map_tx[i];
                 phy_pll_state.pll1_lanes_bitmap |= ((old_pll_state.pll1_lanes_bitmap >> i) & 0x1) << lane_map_tx[i];
            }

            /* Get the port physical lane mask */
            PM8x50_FLEXE_GEN2_PORT_PHYISCAL_LANE_MASK_GET(pm_info, port_lane_mask, port_physical_lane_mask);

            /* Get the PM logical lane array, p2l_lane map[physical_id] = logical_id */
            PM8x50_FLEXE_GEN2_LANE_MAP_TRANSLATE(lane_map_tx, p2l_lane_map);

            /* Per PMD instance */
            for (i = 0; i < MAX_NUM_PMD_INS; i++) {
                 /* Get the lane mask of the instance */
                 lane_mask_ins = 0x3 << (i * MAX_NUM_LANES_PER_PMD_INS);
                 temp_lane_mask = port_physical_lane_mask & lane_mask_ins;
                 temp_pll0_lanes_bitmap = phy_pll_state.pll0_lanes_bitmap & lane_mask_ins;
                 temp_pll1_lanes_bitmap = phy_pll_state.pll1_lanes_bitmap & lane_mask_ins;

                 /* PMD instance used by this port */
                 if (temp_lane_mask){
                     phy_access.access.lane_mask = 0x1 << p2l_lane_map[i * MAX_NUM_LANES_PER_PMD_INS];
                     /* Get PLL0 div */
                     phy_access.access.pll_idx = 0x0;
                     _SOC_IF_ERR_EXIT(phymod_phy_pll_powerdown_get(&phy_access, 0, &pll0_pwrdn));
                     if (!pll0_pwrdn) {
                         _SOC_IF_ERR_EXIT(phymod_phy_pll_multiplier_get(&phy_access, &current_pll0_div));
                     }
                     /* Get PLL1 div */
                     phy_access.access.pll_idx = 0x1;
                     _SOC_IF_ERR_EXIT(phymod_phy_pll_powerdown_get(&phy_access, phy_access.access.pll_idx , &pll1_pwrdn));
                     if (!pll1_pwrdn) {
                         _SOC_IF_ERR_EXIT(phymod_phy_pll_multiplier_get(&phy_access, &current_pll1_div));
                     }
                     if (current_pll0_div == required_pll_div) {
                        pll_index = 0;
                     } else if (current_pll1_div == required_pll_div) {
                        pll_index = 1;
                     } else {
                         /* Check which pll is free */
                         pll0_is_free = (temp_pll0_lanes_bitmap == 0x0) ? TRUE : FALSE;
                         pll1_is_free = (temp_pll1_lanes_bitmap == 0x0) ? TRUE : FALSE;
                         if (pll0_is_free) {
                             pll_index = 0;
                         } else if (pll1_is_free) {
                             pll_index = 1;
                         } else {
                             _SOC_EXIT_WITH_ERR(SOC_E_FAIL,
                                (_SOC_MSG(" VCO reconfigure failed, required: VCO %d, pll0_bitmap: 0x%x, pll1_bitmap: 0x%x \n"),
                                required_vco, old_pll_state.pll0_lanes_bitmap, old_pll_state.pll1_lanes_bitmap));
                         }
                         /* Reconfigure PLL */
                         phy_access.access.pll_idx = pll_index;
                         pll_pwrdn = (pll_index == 0) ? pll0_pwrdn : pll1_pwrdn;
                         if (pll_pwrdn) {
                            _SOC_IF_ERR_EXIT(phymod_phy_pll_pwrdn(&phy_access, pll_index, 0));
                         }
                         _SOC_IF_ERR_EXIT(
                             phymod_phy_pll_reconfig(&phy_access, pll_index, required_pll_div, PM_8x50_FLEXE_GEN2_INFO(pm_info)->ref_clk));
                     }
                     /* Update pll lane bitmap */
                     if (pll_index == 0) {
                         phy_pll_state.pll0_lanes_bitmap |= temp_lane_mask;
                     } else {
                         phy_pll_state.pll1_lanes_bitmap |= temp_lane_mask;
                     }
                 }
            }
            /* Convert to logical lanes bitmap */
            for (i = 0; i < PM8X50_FLEXE_GEN2_LANES_PER_CORE; i++) {
                 new_pll_state->pll0_lanes_bitmap |= (phy_pll_state.pll0_lanes_bitmap >> i & 0x1) << p2l_lane_map[i];
                 new_pll_state->pll1_lanes_bitmap |= (phy_pll_state.pll1_lanes_bitmap >> i & 0x1) << p2l_lane_map[i];
            }
        }
    } else {
        /* Fixed PLL mode. PLL0 - physical lane 0,  PLL1 - physical lane 1.
         * PLL index will be igonred, just call phymod API directly, phymod will loop each logical lane.
         */
        pll_index = 0;
        _SOC_IF_ERR_EXIT(phymod_phy_pll_powerdown_get(&phy_access, pll_index, &pll_pwrdn));
        if (pll_pwrdn) {
            _SOC_IF_ERR_EXIT(phymod_phy_pll_pwrdn(&phy_access, pll_index, 0));
        }
        _SOC_IF_ERR_EXIT(
            phymod_phy_pll_reconfig(&phy_access, pll_index, required_pll_div, PM_8x50_FLEXE_GEN2_INFO(pm_info)->ref_clk));
    }

exit:
    SOC_FUNC_RETURN;
}

/* set/get the speed config for the specified port.*/
int pm8x50_flexe_gen2_port_speed_config_set(int unit, int port, pm_info_t pm_info, const portmod_speed_config_t* speed_config)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    uint64 mac_ctrl;
    phymod_phy_signalling_method_t signalling_mode;
    int nof_phys, i, rx_enable = 0;
    portmod_pbmp_t affected_pbmp;
    portmod_pmd_lane_config_t portmod_lane_config;
    phymod_phy_speed_config_t phy_speed_config;
    phymod_phy_pll_state_t old_pll_state, new_pll_state, pll_state;
    uint32 lane_config, port_lane_mask;
    uint8 pll0_lanes_bitmap, pll1_lanes_bitmap;
    portmod_vco_type_t required_vco;
    phymod_an_mode_type_t an_mode = phymod_AN_MODE_NONE;
    int port_index;
    uint32 is_bypassed = 0;
    uint32 bitmap[1];
    int port_flexe_mode = 0, port_otn_mode = 0;
    uint32 flexible_pll_required = 0;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(mac_ctrl);
    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_t_init(&phy_speed_config));
    sal_memset(&old_pll_state, 0, sizeof(phymod_phy_pll_state_t));
    sal_memset(&new_pll_state, 0, sizeof(phymod_phy_pll_state_t));
    sal_memset(&pll_state, 0, sizeof(phymod_phy_pll_state_t));

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_IS_FLEXIBLE_PLL_GET(unit, pm_info, &flexible_pll_required));

    if (!PM_8x50_FLEXE_GEN2_INFO(pm_info)->portmod_mac_soft_reset) {
        /* Get original MAC CTRL */
        _SOC_IF_ERR_EXIT(cdmac_egress_queue_drain_get(unit, port, &mac_ctrl, &rx_enable));

        /* Disable MAC RX, drain MAC FIFO, assert MAC SOFT_RESET */
        _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_mac_drain_soft_reset(unit, port));
    } else if (!is_bypassed) {
       _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_soft_reset(unit, port, pm_info, portmodMacSoftResetModeIn, &rx_enable, &mac_ctrl));
    }

    /* Validate speed_config */
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_speed_config_validate(unit, port, pm_info, speed_config, &affected_pbmp));

    /* Update port_mode */
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, &port_lane_mask));
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_pm_port_mode_update(unit, port, pm_info, port_index, speed_config->num_lane));

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_speed_config_to_vco_get(speed_config, is_bypassed, &required_vco));

    _SOC_IF_ERR_EXIT(
        PM8x50_FLEXE_GEN2_PLL0_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll0_lanes_bitmap));
    _SOC_IF_ERR_EXIT(
        PM8x50_FLEXE_GEN2_PLL1_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll1_lanes_bitmap));
    old_pll_state.pll0_lanes_bitmap = pll0_lanes_bitmap & (~port_lane_mask);
    old_pll_state.pll1_lanes_bitmap = pll1_lanes_bitmap & (~port_lane_mask);

    /* Reconfig PLL */
    _SOC_IF_ERR_EXIT(
        _pm8x50_flexe_gen2_port_vco_reconfig(unit, port, pm_info, required_vco, old_pll_state, &new_pll_state));

    /* Update pll active lane bitmap per new_pll_state */
    _SOC_IF_ERR_EXIT(
        PM8x50_FLEXE_GEN2_PLL0_ACTIVE_LANE_BITMAP_SET(unit, pm_info, new_pll_state.pll0_lanes_bitmap));
    _SOC_IF_ERR_EXIT(
        PM8x50_FLEXE_GEN2_PLL1_ACTIVE_LANE_BITMAP_SET(unit, pm_info, new_pll_state.pll1_lanes_bitmap));

    /* Clear the adv lane bitmap for the port because it's configured as a force speed port */
    _SOC_IF_ERR_EXIT(
        PM8x50_FLEXE_GEN2_PLL0_ADV_LANE_BITMAP_GET(unit, pm_info, &pll0_lanes_bitmap));
    _SOC_IF_ERR_EXIT(
        PM8x50_FLEXE_GEN2_PLL1_ADV_LANE_BITMAP_GET(unit, pm_info, &pll1_lanes_bitmap));
    pll0_lanes_bitmap &= ~port_lane_mask;
    pll1_lanes_bitmap &= ~port_lane_mask;
    _SOC_IF_ERR_EXIT(
        PM8x50_FLEXE_GEN2_PLL0_ADV_LANE_BITMAP_SET(unit, pm_info, pll0_lanes_bitmap));
    _SOC_IF_ERR_EXIT(
        PM8x50_FLEXE_GEN2_PLL1_ADV_LANE_BITMAP_SET(unit, pm_info, pll1_lanes_bitmap));

     /* Power down unused PLLs for flexible pll mode */
     if (flexible_pll_required) {
        _SOC_IF_ERR_EXIT(
            _pm8x50_flexe_gen2_port_pll_power_down(unit, port, pm_info));
    }
    /* Decode the lane_config in speed_config */
    lane_config = speed_config->lane_config;
    /* Retrieve the default lane config */
    _pm8x50_flexe_gen2_signalling_method_get(is_bypassed, speed_config, &signalling_mode);
    if (lane_config == -1) {
        _SOC_IF_ERR_EXIT(
            phymod_phy_lane_config_default_get(&phy_access, signalling_mode, &portmod_lane_config.pmd_firmware_lane_config));
        portmod_lane_config.pam4_channel_loss = 0;
    } else {
        if (signalling_mode == phymodSignallingMethodPAM4) {
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
        } else if (speed_config->fec == PORTMOD_PORT_PHY_FEC_BASE_R) {
            phy_speed_config.fec_type = phymod_fec_CL74;
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

    /* check if in flexe mode */
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PORT_FLEXE_MODE_GET(unit, pm_info, &port_flexe_mode, port_index));
    if (port_flexe_mode) {
        PHYMOD_SPEED_CONFIG_IN_FLEXE_MODE_SET(&phy_speed_config);
    }
    /* check if in OTN mode */
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PORT_OTN_MODE_GET(unit, pm_info, &port_otn_mode, port_index));
    if (port_otn_mode) {
        PHYMOD_SPEED_CONFIG_IN_OTN_MODE_SET(&phy_speed_config);
    }

    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_set(&phy_access, &phy_speed_config, &new_pll_state, &pll_state));

    if (!PM_8x50_FLEXE_GEN2_INFO(pm_info)->portmod_mac_soft_reset) {
        /* Enable MAC RX, De-assert MAC SOFT_RESET */
        _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_rx_restore_mac_out_of_reset(unit, port, rx_enable));
        /* Restore original MAC CONTROL */
        _SOC_IF_ERR_EXIT(cdmac_mac_ctrl_set(unit, port, mac_ctrl));
    } else if (!is_bypassed) {
        _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_soft_reset(unit, port, pm_info, portmodMacSoftResetModeOut, &rx_enable, &mac_ctrl));
    }

    /* Clear an_mode for the port to force user set new abilities before enable AN later on */
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_AN_MODE_SET(unit, pm_info, an_mode, port_index));

    if (!is_bypassed) {
        /* Update FEC usage in WB */
        for (i = 0 ; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
            if ((port_lane_mask >> i) & 0x1) {
                _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_LANE2FEC_SET(unit, pm_info, i, speed_config->fec));
            }
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_speed_config_get(int unit, int port, pm_info_t pm_info, portmod_speed_config_t* speed_config)
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
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    PORTMOD_BIT_COUNT(phy_access.access.lane_mask, port_num_lanes);

    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_autoneg_status_get(unit, port, pm_info, &an_status));

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
    } else if (phy_speed_config.fec_type == phymod_fec_CL74) {
        speed_config->fec = PORTMOD_PORT_PHY_FEC_BASE_R;
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
int pm8x50_flexe_gen2_port_cl72_set(int unit, int port, pm_info_t pm_info, uint32 enable)
{
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    if (!SAL_BOOT_SIMULATION) {
        _SOC_IF_ERR_EXIT
            (phymod_phy_cl72_set(&phy_access, enable));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_cl72_get(int unit, int port, pm_info_t pm_info, uint32* enable)
{
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    if (!SAL_BOOT_SIMULATION) {
        _SOC_IF_ERR_EXIT
            (phymod_phy_cl72_get(&phy_access, enable));
    }

exit:
    SOC_FUNC_RETURN;
}

/*Get port cl72 status*/
int pm8x50_flexe_gen2_port_cl72_status_get(int unit, int port, pm_info_t pm_info,
                                phymod_cl72_status_t* status)
{
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    if (!SAL_BOOT_SIMULATION) {
        _SOC_IF_ERR_EXIT
            (phymod_phy_cl72_status_get(&phy_access, status));
    }

exit:
    SOC_FUNC_RETURN;
}

/*Port loopback set\get*/
int pm8x50_flexe_gen2_port_loopback_set(int unit, int port, pm_info_t pm_info,
                                     portmod_loopback_mode_t loopback_type, int enable)
{
    phymod_loopback_mode_t phymod_lb_type;
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int phy_acc;
    uint32 is_bypassed = 0, bitmap[1];
    int port_index;


    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_phy_access_get(unit, port, pm_info, &phy_acc));

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));

    /* loopback type validation*/
    switch(loopback_type){
        case portmodLoopbackMacOuter:
            _SOC_IF_ERR_EXIT(cdmac_loopback_set(unit, phy_acc, loopback_type, enable));
            break;
        case portmodLoopbackPhyRloopPCS:
        case portmodLoopbackPhyRloopPMD:
        case portmodLoopbackPhyGloopPMD:
        case portmodLoopbackPhyGloopPCS:
            if (enable && !is_bypassed) {
                _SOC_IF_ERR_EXIT(cdmac_loopback_set(unit, phy_acc, portmodLoopbackMacOuter, 0));
            }
            _SOC_IF_ERR_EXIT(portmod_commmon_portmod_to_phymod_loopback_type(unit,
                             loopback_type, &phymod_lb_type));
            _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
            _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                             &params, 1, &phy_access, &nof_phys, NULL));
            _SOC_IF_ERR_EXIT(phymod_phy_loopback_set(&phy_access, phymod_lb_type, enable));
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (
                      _SOC_MSG("unsupported loopback type %d"), loopback_type));
            break;
    }

    if (enable) {
        _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_lag_failover_status_toggle(unit, port, pm_info));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_loopback_get(int unit, int port, pm_info_t pm_info,
                                     portmod_loopback_mode_t loopback_type,
                                     int* enable)
{
    phymod_loopback_mode_t phymod_lb_type;
    portmod_access_get_params_t params;
    int nof_phys;
    phymod_phy_access_t phy_access;
    uint32 tmp_enable = 0;
    int rv = PHYMOD_E_NONE;

    SOC_INIT_FUNC_DEFS;

    switch(loopback_type){
        case portmodLoopbackMacOuter:
            _SOC_IF_ERR_EXIT(cdmac_loopback_get(unit, port, loopback_type, enable));
            break;
        case portmodLoopbackPhyRloopPCS:
        case portmodLoopbackPhyRloopPMD:
        case portmodLoopbackPhyGloopPMD:
        case portmodLoopbackPhyGloopPCS:
            _SOC_IF_ERR_EXIT(portmod_commmon_portmod_to_phymod_loopback_type(unit,
                             loopback_type, &phymod_lb_type));
            _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
            _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                             &params, 1, &phy_access, &nof_phys, NULL));
            rv = phymod_phy_loopback_get(&phy_access, phymod_lb_type, &tmp_enable);
            if (rv == PHYMOD_E_UNAVAIL) {
                rv = PHYMOD_E_NONE;
                tmp_enable = 0;
            }
            _SOC_IF_ERR_EXIT(rv);
            *enable = tmp_enable;
            break;
        default:
            (*enable) = 0; /* not supported --> no loopback */
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

/*Port RX MAC ENABLE set\get*/
int pm8x50_flexe_gen2_port_rx_mac_enable_set(int unit, int port,
                                          pm_info_t pm_info, int enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_rx_enable_set(unit, port, enable));
exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_rx_mac_enable_get(int unit, int port,
                                          pm_info_t pm_info, int* enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_rx_enable_get(unit, port, enable));
exit:
    SOC_FUNC_RETURN;
}

/*Port TX MAC ENABLE set\get*/
int pm8x50_flexe_gen2_port_tx_mac_enable_set(int unit, int port,
                                          pm_info_t pm_info,
                                          int enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_tx_enable_set(unit, port, enable));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_tx_mac_enable_get(int unit, int port,
                                          pm_info_t pm_info,
                                          int* enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_tx_enable_get(unit, port, enable));

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
int pm8x50_flexe_gen2_port_speed_ability_local_get(int unit, int port, pm_info_t pm_info,
                                                int max_num_abilities,
                                                portmod_port_speed_ability_t* abilities,
                                                int* num_abilities)
{
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys, port_num_lanes, i;
    int port_index, max_speed, fs_table_size, an_table_size;
    uint32 bitmap;
    uint32_t port_lane_mask;
    pm8x50_flexe_gen2_fs_ability_table_entry_t fs_table_entry;
    pm8x50_flexe_gen2_an_ability_table_entry_t an_table_entry;
    SOC_INIT_FUNC_DEFS;

    *num_abilities = 0;
    sal_memset(&fs_table_entry, 0, sizeof(pm8x50_flexe_gen2_fs_ability_table_entry_t));
    sal_memset(&an_table_entry, 0, sizeof(pm8x50_flexe_gen2_an_ability_table_entry_t));

    /* find the max speed of the port */
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get (unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_MAX_SPEED_GET(unit, pm_info, &max_speed, port_index));

    /* find number of lanes of the port */
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    port_lane_mask = phy_access.access.lane_mask;
    PORTMOD_BIT_COUNT(port_lane_mask, port_num_lanes);

    /* get PM version. */
    /*_SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_pm_version_get(unit, pm_info, &version));*/

    fs_table_size = PM8x50_FLEXE_GEN2_FS_ABILITY_TABLE_SIZE;
    an_table_size = PM8x50_FLEXE_GEN2_AN_ABILITY_TABLE_SIZE;
    for (i = 0; i < fs_table_size; i++) {
         _SOC_IF_ERR_EXIT
             (_pm8x50_flexe_gen2_fs_ability_table_read_entry(i, &fs_table_entry));
        if (fs_table_entry.num_lanes == port_num_lanes && fs_table_entry.speed <= max_speed) {
            if (*num_abilities < max_num_abilities) {
                abilities[*num_abilities].speed     = fs_table_entry.speed;
                abilities[*num_abilities].num_lanes = fs_table_entry.num_lanes;
                abilities[*num_abilities].fec_type  = fs_table_entry.fec_type;
                abilities[*num_abilities].medium    = PORTMOD_PORT_PHY_MEDIUM_ALL;
                abilities[*num_abilities].pause     = PORTMOD_PORT_PHY_PAUSE_ALL;
                abilities[*num_abilities].channel   = PORTMOD_PORT_PHY_CHANNEL_ALL;
                abilities[*num_abilities].an_mode   = PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_NONE;
                (*num_abilities) += 1;
            } else {
                break;
            }
        }
    }

    for (i = 0; i < an_table_size; i++) {
         _SOC_IF_ERR_EXIT
             (_pm8x50_flexe_gen2_an_ability_table_read_entry(i, &an_table_entry));
        if ((an_table_entry.num_lanes <= port_num_lanes) &&
           ( an_table_entry.speed <= max_speed)) {
            if (*num_abilities < max_num_abilities) {
                abilities[*num_abilities].speed      = an_table_entry.speed;
                abilities[*num_abilities].num_lanes  = an_table_entry.num_lanes;
                abilities[*num_abilities].fec_type   = an_table_entry.fec_type;
                if (an_table_entry.speed == 10000) {
                    /* 10G AN only support Backplane */
                    abilities[*num_abilities].medium = PORTMOD_PORT_PHY_MEDIUM_BACKPLANE;
                } else {
                    abilities[*num_abilities].medium = PORTMOD_PORT_PHY_MEDIUM_ALL;
                }
                abilities[*num_abilities].pause      = PORTMOD_PORT_PHY_PAUSE_ALL;
                abilities[*num_abilities].channel    = PORTMOD_PORT_PHY_CHANNEL_ALL;
                abilities[*num_abilities].an_mode    = an_table_entry.an_mode;
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
int _pm8x50_flexe_gen2_port_port_to_phy_ability(int num_abilities,
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
            case PORTMOD_PORT_PHY_FEC_BASE_R:
                an_advert_abilities->autoneg_abilities[i].fec = phymod_fec_CL74;
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
int _pm8x50_flexe_gen2_port_phy_to_port_ability(phymod_autoneg_advert_abilities_t* an_advert_abilities,
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
            case phymod_fec_CL74:
                abilities[i].fec_type = PORTMOD_PORT_PHY_FEC_BASE_R;
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

STATIC
int _pm8x50_flexe_gen2_port_autoneg_abilities_validate(int unit, int port, pm_info_t pm_info,
                                                       int num_abilities,
                                                       const portmod_port_speed_ability_t* abilities,
                                                       phymod_autoneg_advert_abilities_t* an_advert_abilities,
                                                       phymod_an_mode_type_t* an_mode,
                                                       pm8x50_flexe_gen2_vcos_bmp_t* vcos,
                                                       portmod_fec_t *rs_fec_req)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, i, port_index, mpp_index;
    uint32 bitmap;
    portmod_vco_type_t request_vco = portmodVCOInvalid, tmp_request_vco = portmodVCOInvalid;
    int is_msa = 0, is_bam = 0, current_media_type = 0;
    int is_dual_50g = 0, cl73_bam_msa_base_r_req = 0, dual_50g_req = 0;
    int rs544_req = 0, rs528_req = 0, rs272_req = 0;
    int req_100g_4lane_no_fec = 0, req_100g_4lane_with_fec = 0;
    uint32_t current_100g_an_fec_override = 0;
    phymod_an_mode_type_t cur_an_mode = 0;
    int fec_cl73, fec_cl73_25g, fec_cl73bam_msa, fec_rs528_bam_msa;
    uint8 rs528_bm = 0, rs544_bm = 0, rs272_bm = 0, affected_bm = 0;
    phymod_firmware_lane_config_t fw_lane_config;
    uint32 flexible_pll_required = 0;
    SOC_INIT_FUNC_DEFS;

    fec_cl73 = PORTMOD_PORT_PHY_FEC_INVALID;
    fec_cl73_25g = PORTMOD_PORT_PHY_FEC_INVALID;
    fec_cl73bam_msa = PORTMOD_PORT_PHY_FEC_INVALID;
    fec_rs528_bam_msa = PORTMOD_PORT_PHY_FEC_INVALID;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_IS_FLEXIBLE_PLL_GET(unit, pm_info, &flexible_pll_required));

    /* first get current PM core 100G 4lane FEC override bit */
    _SOC_IF_ERR_EXIT(phymod_phy_fec_override_get(&phy_access, &current_100g_an_fec_override));

    /* 1. Valid media type */
    _SOC_IF_ERR_EXIT(
        phymod_phy_firmware_lane_config_get(&phy_access, &fw_lane_config));

    if (fw_lane_config.MediaType == phymodFirmwareMediaTypeCopperCable) {
        current_media_type = PORTMOD_PORT_PHY_MEDIUM_COPPER;
    } else if (fw_lane_config.MediaType == phymodFirmwareMediaTypePcbTraceBackPlane) {
        current_media_type = PORTMOD_PORT_PHY_MEDIUM_BACKPLANE;
    }

    /* 2. Validate abilities */
    for (i = 0; i < num_abilities; i++) {
        is_dual_50g = 0;
        _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_an_ability_table_vco_get(abilities[i].speed, abilities[i].num_lanes,
                         abilities[i].fec_type, abilities[i].an_mode, &request_vco));

        /* 2.1. AN ability is supported by HW */
        if (request_vco == portmodVCOInvalid) {
            _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                               (_SOC_MSG("port %d: abilities[%d] is not supported."), port, i));
        }
        /* Flexible PLL: Can support (25.781G + 26.562G) abilities.
         * Fixed PLL   : All abilities can only share a common vco.
         */
        if (flexible_pll_required) {
            if ((request_vco != portmodVCO25P781G) && (request_vco != portmodVCO26P562G)) {
                _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                                   (_SOC_MSG("port %d: abilities[%d] vco[%d] is invalid. Only 25.781G and 26.562G are supported for flexible pll mode."),
                                   port, i, request_vco));
            }
        } else {
            if (tmp_request_vco == portmodVCOInvalid) {
                tmp_request_vco = request_vco;
            } else if (tmp_request_vco != request_vco) {
                _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                                   (_SOC_MSG("port %d: abilities[%d] vco[%d, %d] conflicts. Abilities can only share one common vco."),
                                   port, i, tmp_request_vco, request_vco));
            }
        }
        if ((abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_RS_544) ||
            (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_RS_544_2XN)) {
            rs544_req = 1;
        } else if (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_RS_FEC) {
            rs528_req = 1;
        } else if ((abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_RS_272) ||
                   (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_RS_272_2XN)) {
            rs272_req = 1;
        }

        /* 2.2. AN abilities do not conflict with each other */
        if (abilities[i].pause != abilities[0].pause) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                               (_SOC_MSG("port %d: Pause conflicts among abilities."), port));
        }
        if (abilities[i].medium != current_media_type) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                               (_SOC_MSG("port %d: Medium conflicts with current media type."), port));
        }
        if ((abilities[i].speed == 50000) &&
            (abilities[i].num_lanes == 2) &&
            (abilities[i].fec_type != PORTMOD_PORT_PHY_FEC_RS_544)) {
            is_dual_50g = 1;
            dual_50g_req = 1;
        }

        /* next if CL73 100G 4 lane with or without FEC */
        if ((abilities[i].speed == 100000) &&
            (abilities[i].num_lanes == 4)  &&
            (abilities[i].an_mode == PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73)) {
            if (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_RS_FEC) {
                req_100g_4lane_with_fec = 1;
            }
            if (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_NONE) {
                req_100g_4lane_no_fec = 1;
            }
        }

        if (abilities[i].an_mode == PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM) {
            is_bam = 1;
            /* CL74 request of 20G-1lane, 40G-2lane and fec_cl73 need
             * to be identical.
             */
            if ((abilities[i].speed == 20000) || (abilities[i].speed == 40000)) {
                if (((fec_cl73 == PORTMOD_PORT_PHY_FEC_NONE) &&
                     (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_BASE_R)) ||
                    ((fec_cl73 == PORTMOD_PORT_PHY_FEC_BASE_R) &&
                     (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_NONE))) {
                    _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                                       (_SOC_MSG("port %d: FEC conflicts among abilities."), port));
                } else {
                    if ((abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_BASE_R) ||
                        (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_NONE)) {
                        fec_cl73 = abilities[i].fec_type;
                    }
                    if (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_BASE_R) {
                        cl73_bam_msa_base_r_req = 1;
                    }
                }
            }

            /* CL74 request of 25G-1lane should be identical. */
            if (abilities[i].speed == 25000) {
                if (((fec_cl73bam_msa == PORTMOD_PORT_PHY_FEC_NONE)
                     && (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_BASE_R)) ||
                    ((fec_cl73bam_msa == PORTMOD_PORT_PHY_FEC_BASE_R)
                     && (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_NONE))) {
                    _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                                       (_SOC_MSG("port %d: FEC conflicts among abilities."), port));
                } else {
                    /* only BaseR and FEC_NONE are considered here */
                    if ((abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_BASE_R) ||
                        (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_NONE)) {
                        fec_cl73bam_msa = abilities[i].fec_type;
                    }
                    if (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_BASE_R) {
                        cl73_bam_msa_base_r_req = 1;
                    }
                }
            }
            /* RS528 request of 25G-1lane, 50G-2lane should be identical */
            if ((abilities[i].speed == 25000) || (is_dual_50g)) {
                if (((fec_rs528_bam_msa == PORTMOD_PORT_PHY_FEC_NONE)
                     && (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_RS_FEC)) ||
                    ((fec_rs528_bam_msa == PORTMOD_PORT_PHY_FEC_RS_FEC)
                     && (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_NONE))) {
                    _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                                       (_SOC_MSG("port %d: FEC conflicts among abilities."), port));
                } else {
                    /* only RSFEC and FEC_NONE are considered here */
                    if ((abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_RS_FEC) ||
                        (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_NONE)) {
                        fec_rs528_bam_msa = abilities[i].fec_type;
                    }
                }
            }
        } else if (abilities[i].an_mode == PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_MSA) {
            is_msa = 1;
            /* CL74 request of 25G-1lane should be identical */
            if (((fec_cl73bam_msa == PORTMOD_PORT_PHY_FEC_NONE)
                 && (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_BASE_R)) ||
                ((fec_cl73bam_msa == PORTMOD_PORT_PHY_FEC_BASE_R)
                 && (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_NONE))) {
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                                   (_SOC_MSG("port %d: FEC conflicts among abilities."), port));
            } else {
                /* only BaseR and FEC_NONE are considered here */
                if ((abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_BASE_R) ||
                    (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_NONE)) {
                    fec_cl73bam_msa = abilities[i].fec_type;
                }
                if (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_BASE_R) {
                    cl73_bam_msa_base_r_req = 1;
                }
            }
            /* RS528 request of 25G-1lane, 50G-2lane should be identical */
            if (((fec_rs528_bam_msa == PORTMOD_PORT_PHY_FEC_NONE)
                 && (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_RS_FEC)) ||
                ((fec_rs528_bam_msa == PORTMOD_PORT_PHY_FEC_RS_FEC)
                 && (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_NONE))) {
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                                   (_SOC_MSG("port %d: FEC conflicts among abilities."), port));
            } else {
                /* only RSFEC and FEC_NONE are considered here */
                if ((abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_RS_FEC) ||
                    (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_NONE)) {
                    fec_rs528_bam_msa = abilities[i].fec_type;
                }
            }
        } else {
            /* CL73 */
            if (abilities[i].speed == 40000 || abilities[i].speed == 10000) {
                if (((fec_cl73 == PORTMOD_PORT_PHY_FEC_NONE)
                     && (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_BASE_R)) ||
                    ((fec_cl73 == PORTMOD_PORT_PHY_FEC_BASE_R)
                     && (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_NONE))) {
                    _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                                       (_SOC_MSG("port %d: FEC conflicts among abilities."), port));
                } else {
                    /* only BaseR and FEC_NONE are considered here */
                    if ((abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_BASE_R) ||
                        (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_NONE)) {
                        fec_cl73 = abilities[i].fec_type;
                    }
                    /* 50G-2lane shares the same cl74 bit with other CL73,CL73BAM,MSA abilities */
                    /* cl73_bam_msa_base_r_req is used to remember if there is any CL73,BAM,MSA BaseR request */
                    if (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_BASE_R) {
                        cl73_bam_msa_base_r_req = 1;
                    }
                }
                if (abilities[i].speed == 10000) {
                    if (abilities[i].medium != PORTMOD_PORT_PHY_MEDIUM_BACKPLANE) {
                        _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                                            (_SOC_MSG("port %d: abilities[%d] is not supported."), port, i));
                    }
                }
            } else if (abilities[i].speed == 25000) {
                if ((abilities[i].channel == PORTMOD_PORT_PHY_CHANNEL_SHORT) &&
                    (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_RS_FEC)) {
                    _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                                       (_SOC_MSG("port %d: abilities[%d] is not supported."), port, i));
                }
                if ((fec_cl73_25g == PORTMOD_PORT_PHY_FEC_NONE)
                    && (abilities[i].fec_type != PORTMOD_PORT_PHY_FEC_NONE)) {
                    _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                                       (_SOC_MSG("port %d: FEC conflicts among abilities."), port));
                } else {
                    fec_cl73_25g = abilities[i].fec_type;
                }
            }
        }
        if (request_vco == portmodVCO20P625G) {
            PM8x50_FLEXE_GEN2_VCO_20P625G_SET(*vcos);
        } else if (request_vco == portmodVCO25P781G) {
            PM8x50_FLEXE_GEN2_VCO_25P781G_SET(*vcos);
        } else if (request_vco == portmodVCO26P562G) {
            PM8x50_FLEXE_GEN2_VCO_26P562G_SET(*vcos);
        }
    }

    /* both 100G 4 lane CL73 with and without FEC can not co-exist on the same core */
    if (req_100g_4lane_with_fec && req_100g_4lane_no_fec) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("port %d: CL91 and No FEC cannot be advertised together with 100G-4lane abilities."), port));
    } else if (req_100g_4lane_with_fec && current_100g_an_fec_override) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("port %d: CL91 and No FEC cannot be advertised together with 100G-4lane abilities."), port));
    } else if (req_100g_4lane_no_fec && !current_100g_an_fec_override) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("port %d: CL91 and No FEC cannot be advertised together with 100G-4lane abilities."), port));
    }

    /* 50G-2lane does not support Base-R.
       In AN, 50G-2lane shares the same cl74 bit with other CL73,CL73BAM,MSA abilities. */
    if (dual_50g_req && (cl73_bam_msa_base_r_req == 1)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("port %d: Base_R cannot be advertised together with 50G-2lane abilities."), port));
    }

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

    /* within same MPP, MSA and BAM can not be exist at same time */
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    mpp_index = (int)(port_index / 4);
    for (i = (0 + (mpp_index * 4)); i < (4 + (mpp_index*4)); i++) {
        if (i == port_index) {
            continue;
        }
        _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_AN_MODE_GET(unit, pm_info, &cur_an_mode, i));
        if ((*an_mode == phymod_AN_MODE_CL73_MSA) && (cur_an_mode == phymod_AN_MODE_CL73BAM)) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                (_SOC_MSG("port %d: CL73_MSA cannot be advertised when current MPP is in CL73_BAM."), port));
        }
        if ((*an_mode == phymod_AN_MODE_CL73BAM) && (cur_an_mode == phymod_AN_MODE_CL73_MSA)) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                (_SOC_MSG("port %d: CL73_BAM cannot be advertised when current MPP is in CL73_MSA."), port));
        }
    }

    /* 2.3. FEC arch can be supported within the each MPP */
    if (rs544_req || rs528_req || rs272_req) {
        _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_fec_lanebitmap_get(unit, pm_info, &rs528_bm, &rs544_bm, &rs272_bm));
        rs528_bm &= ~phy_access.access.lane_mask;
        rs544_bm &= ~phy_access.access.lane_mask;
        rs272_bm &= ~phy_access.access.lane_mask;
        if (rs544_req) {
            rs544_bm |= phy_access.access.lane_mask;
        }
        if (rs528_req) {
            rs528_bm |= phy_access.access.lane_mask;
        }
        if (rs272_req) {
            rs272_bm |= phy_access.access.lane_mask;
        }
        _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_fec_validate(unit, rs528_bm, rs544_bm, rs272_bm, &affected_bm));
    }

    /* update request fec_type */
    if (rs544_req) {
        *rs_fec_req = PORTMOD_PORT_PHY_FEC_RS_544;
    } else if (rs272_req) {
        *rs_fec_req = PORTMOD_PORT_PHY_FEC_RS_272;
    } else if (rs528_req) {
        *rs_fec_req = PORTMOD_PORT_PHY_FEC_RS_FEC;
    } else {
        *rs_fec_req = PORTMOD_PORT_PHY_FEC_NONE;
    }

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_port_to_phy_ability(num_abilities, abilities, an_advert_abilities));

exit:
    SOC_FUNC_RETURN;
}

/*Set/get port auto negotiation ability*/
int pm8x50_flexe_gen2_port_autoneg_ability_advert_set(int unit, int port, pm_info_t pm_info,
                                                   int num_abilities,
                                                   const portmod_port_speed_ability_t* abilities)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, i, port_index;
    uint32 bitmap;
    portmod_vco_type_t request_vco = portmodVCOInvalid;
    pm8x50_flexe_gen2_vcos_bmp_t vcos = 0;
    portmod_speed_config_t speed_config;
    phymod_autoneg_advert_ability_t autoneg_abilities[PM8x50_FLEXE_GEN2_MAX_AN_ABILITY];
    phymod_autoneg_advert_abilities_t an_advert_abilities;
    phymod_an_mode_type_t an_mode = 0;
    phymod_phy_pll_state_t old_pll_state, new_pll_state, pll_state;
    uint8 pll0_lanes_bitmap, pll1_lanes_bitmap;
    portmod_fec_t fec;
    uint32 is_bypassed = 0;
    uint32 port_lane_mask;
    uint32 flexible_pll_required = 0;
    portmod_fec_t rs_fec_req = PORTMOD_PORT_PHY_FEC_NONE;

    SOC_INIT_FUNC_DEFS;

    sal_memset(autoneg_abilities, 0,
               PM8x50_FLEXE_GEN2_MAX_AN_ABILITY * sizeof(phymod_autoneg_advert_ability_t));
    sal_memset(&an_advert_abilities, 0, sizeof(phymod_autoneg_advert_abilities_t));
    sal_memset(&old_pll_state, 0, sizeof(phymod_phy_pll_state_t));
    sal_memset(&new_pll_state, 0, sizeof(phymod_phy_pll_state_t));
    sal_memset(&pll_state, 0, sizeof(phymod_phy_pll_state_t));

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get (unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));
    if (is_bypassed) {
        _SOC_EXIT_WITH_ERR(SOC_E_CONFIG, (_SOC_MSG("port %d: AN is not supported by pcs bypass mode "), port));
    }
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_IS_FLEXIBLE_PLL_GET(unit, pm_info, &flexible_pll_required));

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    port_lane_mask = phy_access.access.lane_mask;

    /* 1. Validate abilities. */
    an_advert_abilities.autoneg_abilities = autoneg_abilities;
    _SOC_IF_ERR_EXIT(
        _pm8x50_flexe_gen2_port_autoneg_abilities_validate(unit, port, pm_info, num_abilities, abilities,
                                                           &an_advert_abilities, &an_mode, &vcos,
                                                           &rs_fec_req));
    _SOC_IF_ERR_EXIT(
        PM8x50_FLEXE_GEN2_PLL0_ADV_LANE_BITMAP_GET(unit, pm_info, &pll0_lanes_bitmap));
    _SOC_IF_ERR_EXIT(
        PM8x50_FLEXE_GEN2_PLL1_ADV_LANE_BITMAP_GET(unit, pm_info, &pll1_lanes_bitmap));
    old_pll_state.pll0_lanes_bitmap = pll0_lanes_bitmap & (~port_lane_mask);
    old_pll_state.pll1_lanes_bitmap = pll1_lanes_bitmap & (~port_lane_mask);

    /* 2. Reconfigure PLLs */
    if (flexible_pll_required) {
        if (PM8x50_FLEXE_GEN2_VCO_26P562G_GET(vcos) || PM8x50_FLEXE_GEN2_VCO_25P781G_GET(vcos)) {
            if (PM8x50_FLEXE_GEN2_VCO_26P562G_GET(vcos)) {
                request_vco = portmodVCO26P562G;
                _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_vco_reconfig(unit, port, pm_info, request_vco, old_pll_state, &new_pll_state));
                /* Update old_pll_state in case another VCO reconfiguration*/
                sal_memcpy(&old_pll_state, &new_pll_state, sizeof(phymod_phy_pll_state_t));
            }

            if (PM8x50_FLEXE_GEN2_VCO_25P781G_GET(vcos)) {
                request_vco = portmodVCO25P781G;
                _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_vco_reconfig(unit, port, pm_info, request_vco, old_pll_state, &new_pll_state));
            }
        } else {
            _SOC_EXIT_WITH_ERR(SOC_E_CONFIG, (_SOC_MSG("port %d: vco %d is invalid."), port, request_vco));
        }
    } else {
        if (PM8x50_FLEXE_GEN2_VCO_20P625G_GET(vcos)) {
            request_vco = portmodVCO20P625G;
        } else if (PM8x50_FLEXE_GEN2_VCO_25P781G_GET(vcos)) {
            request_vco = portmodVCO25P781G;
        } else if (PM8x50_FLEXE_GEN2_VCO_26P562G_GET(vcos)) {
            request_vco = portmodVCO26P562G;
        } else {
            _SOC_EXIT_WITH_ERR(SOC_E_CONFIG, (_SOC_MSG("port %d: vco %d is invalid."), port, request_vco));
        }
        _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_vco_reconfig(unit, port, pm_info,request_vco, old_pll_state, &new_pll_state));
    }

    /* 3. Set AN abilities in HW */
    _SOC_IF_ERR_EXIT(
        phymod_phy_autoneg_advert_ability_set(&phy_access, &an_advert_abilities, &new_pll_state, &pll_state));

    /* 4. Update WB DB and power down unused PLLs */
    /* 4.1 Update pll adv lane bitmap per new_pll_state */
    _SOC_IF_ERR_EXIT(
        PM8x50_FLEXE_GEN2_PLL0_ADV_LANE_BITMAP_SET(unit, pm_info, new_pll_state.pll0_lanes_bitmap));
    _SOC_IF_ERR_EXIT(
        PM8x50_FLEXE_GEN2_PLL1_ADV_LANE_BITMAP_SET(unit, pm_info, new_pll_state.pll1_lanes_bitmap));

    /* 4.2 Clear the active lane bitmap for the port because it's configured as a AN port */
    _SOC_IF_ERR_EXIT(
        PM8x50_FLEXE_GEN2_PLL0_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll0_lanes_bitmap));
    _SOC_IF_ERR_EXIT(
        PM8x50_FLEXE_GEN2_PLL1_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll1_lanes_bitmap));
    pll0_lanes_bitmap &= ~port_lane_mask;
    pll1_lanes_bitmap &= ~port_lane_mask;
    _SOC_IF_ERR_EXIT(
        PM8x50_FLEXE_GEN2_PLL0_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll0_lanes_bitmap));
    _SOC_IF_ERR_EXIT(
        PM8x50_FLEXE_GEN2_PLL1_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll1_lanes_bitmap));

    /* 4.3 Update an_mode in WB */
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_AN_MODE_SET(unit, pm_info, an_mode, port_index));

    /* 4.4 Update FEC type in WB for RS528, RS544 case */
    /* If AN request RS544, update DB and this will be treated as FEC arch restriction.
     * Else if PM is 16nm and AN request RS528, update DB and this will be treated as FEC arch restriction.
     * Else if AN request other FEC, keep the force speed FEC restriction. Need to update
     * FEC restrition when AN is enabled.
     */
    if ((rs_fec_req == PORTMOD_PORT_PHY_FEC_RS_544) ||
        (rs_fec_req == PORTMOD_PORT_PHY_FEC_RS_272)) {
        fec = rs_fec_req;
        for (i = 0 ; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
            if ((phy_access.access.lane_mask >> i) & 0x1) {
                _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_LANE2FEC_SET(unit, pm_info, i, fec));
            }
        }
    } else {
        /* Call speed_config_get to get the FEC type and update the WB */
        /* This is to make sure the DB stores the FS FEC instead of the previous adv FEC */
        _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_speed_config_get(unit, port, pm_info, &speed_config));
        for (i = 0 ; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
            if ((phy_access.access.lane_mask >> i) & 0x1) {
                _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_LANE2FEC_SET(unit, pm_info, i, speed_config.fec));
            }
        }
    }

    /* 4.5 Power down unused PLLs. */
    if (flexible_pll_required) {
        _SOC_IF_ERR_EXIT(
            _pm8x50_flexe_gen2_port_pll_power_down(unit, port, pm_info));
    }
exit:

    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_autoneg_ability_advert_get(int unit, int port, pm_info_t pm_info,
                                                   int max_num_abilities,
                                                   portmod_port_speed_ability_t* abilities,
                                                   int* actual_num_abilities)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, i;
    phymod_autoneg_advert_ability_t autoneg_abilities[PM8x50_FLEXE_GEN2_MAX_AN_ABILITY];
    phymod_autoneg_advert_abilities_t an_advert_abilities;

    SOC_INIT_FUNC_DEFS;

    for (i = 0; i < PM8x50_FLEXE_GEN2_MAX_AN_ABILITY; i++) {
        _SOC_IF_ERR_EXIT(phymod_autoneg_advert_ability_t_init(&autoneg_abilities[i]));
    }
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    an_advert_abilities.autoneg_abilities = autoneg_abilities;
    _SOC_IF_ERR_EXIT(phymod_phy_autoneg_advert_ability_get(&phy_access, &an_advert_abilities));

    if (an_advert_abilities.num_abilities > max_num_abilities) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("port %d: There are %d AN abilities. Larger array is needed."),
                           port, an_advert_abilities.num_abilities));
    } else {
        _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_phy_to_port_ability(&an_advert_abilities, actual_num_abilities, abilities));
    }

exit:
    SOC_FUNC_RETURN;
}

/*Port ability remote Adv get*/
int pm8x50_flexe_gen2_port_autoneg_ability_remote_get(int unit, int port, pm_info_t pm_info,
                                                   int max_num_abilities,
                                                   portmod_port_speed_ability_t* abilities,
                                                   int* actual_num_abilities)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, i;
    phymod_autoneg_advert_ability_t autoneg_abilities[PM8x50_FLEXE_GEN2_MAX_AN_ABILITY];
    phymod_autoneg_advert_abilities_t an_advert_abilities;
    int port_index, is_400g_an = 0, remote_pause = 0;
    uint32 bitmap;
    phymod_autoneg_status_t an_status;

    SOC_INIT_FUNC_DEFS;

    for (i = 0; i < PM8x50_FLEXE_GEN2_MAX_AN_ABILITY; i++) {
        _SOC_IF_ERR_EXIT(phymod_autoneg_advert_ability_t_init(&autoneg_abilities[i]));
    }

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    an_advert_abilities.autoneg_abilities = autoneg_abilities;

    _SOC_IF_ERR_EXIT(phymod_phy_autoneg_remote_advert_ability_get(&phy_access, &an_advert_abilities));

    /* next need to check 400G AN enabled or not */
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_ENABLE_GET(unit, pm_info, &is_400g_an, port_index));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_autoneg_status_get(unit, port, pm_info, &an_status));
    if (is_400g_an && an_status.enabled && an_status.locked) {
        _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_REMOTE_PAUSE_GET(unit, pm_info, &remote_pause, port_index));
        an_advert_abilities.num_abilities = 1;
        an_advert_abilities.autoneg_abilities[0].pause = remote_pause;
        an_advert_abilities.autoneg_abilities[0].speed = 400000;
        an_advert_abilities.autoneg_abilities[0].resolved_num_lanes = 8;
        an_advert_abilities.autoneg_abilities[0].fec = phymod_fec_RS544_2XN;
        an_advert_abilities.autoneg_abilities[0].channel = phymod_channel_long;
        an_advert_abilities.autoneg_abilities[0].an_mode = phymod_AN_MODE_MSA;
    }

    if (an_advert_abilities.num_abilities > max_num_abilities) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("port %d: There are %d AN abilities. Larger array is needed."),
                           port, an_advert_abilities.num_abilities));
    } else {
        _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_phy_to_port_ability(&an_advert_abilities, actual_num_abilities, abilities));
    }

exit:
    SOC_FUNC_RETURN;
}

/*Set\Get autoneg*/
int pm8x50_flexe_gen2_port_autoneg_set(int unit, int port, pm_info_t pm_info,
                                    uint32 phy_flags,
                                    const phymod_autoneg_control_t* an)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, port_num_lanes;
    int port_index, actual_num_abilities = 0, is_400g_an = 0;
    int rs544_req = 0, rs272_req = 0, rs528_req = 0;
    uint32_t bitmap;
    phymod_autoneg_control_t an_control;
    int i;
    portmod_port_speed_ability_t abilities[PM8x50_FLEXE_GEN2_MAX_AN_ABILITY];
    portmod_fec_t fec;
    int portmon_disable;
    uint32_t link_fail_wait_time;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    PORTMOD_BIT_COUNT(phy_access.access.lane_mask, port_num_lanes);

    an_control = *an;

    an_control.num_lane_adv = port_num_lanes;

    /* Get an_mode from WB */
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_AN_MODE_GET(unit, pm_info, &an_control.an_mode, port_index));
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
        /* Check if RS544/RS528/RS272 is required for AN abilities */
        _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_autoneg_ability_advert_get(unit, port, pm_info,
                                                                PM8x50_FLEXE_GEN2_MAX_AN_ABILITY,
                                                                abilities,
                                                                &actual_num_abilities));
        for (i = 0; i < actual_num_abilities; i++) {
            /* check if 400G an is enabled */
            if (abilities[i].speed == 400000) {
                is_400g_an = 1;
            }
            if ((abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_RS_544) ||
                (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_RS_544_2XN)) {
                rs544_req = 1;
            }
            if (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_RS_FEC) {
                rs528_req = 1;
            }
            if ((abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_RS_272) ||
                (abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_RS_272_2XN)) {
                rs272_req = 1;
            }
        }
        if (rs544_req) {
            PHYMOD_AN_F_FEC_RS272_CLR_SET(&an_control);
        } else if (rs272_req) {
            PHYMOD_AN_F_FEC_RS272_REQ_SET(&an_control);
        }
    }
    /*
     * Before enabling serdes AN bit, need to disable the port in PCS
     * and update mac port mode.
     */
    if (an_control.enable) {
        _SOC_IF_ERR_EXIT(phymod_phy_pcs_enable_set(&phy_access, 0));
        /* call port level port mode update */
        _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_pm_port_mode_update(unit, port, pm_info, port_index, 1));
    }

    _SOC_IF_ERR_EXIT(phymod_phy_autoneg_set(&phy_access, &an_control));

    /* Restore AN link fail inhibit timer LT PAM4 when 400G AN is disabled,
     * since the timer is changed during 400G AN enable.
     */
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_AN_LINK_FAIL_INHIBIT_TIMER_LT_PAM4_GET(unit, pm_info, &link_fail_wait_time));
    if (!an_control.enable &&
        (link_fail_wait_time != AN_LINK_FAIL_INHIBIT_TIMER_LT_PAM4_DEFAULT) &&
        (port_num_lanes == 8)) {
        _SOC_IF_ERR_EXIT(phymod_phy_an_timer_set(&phy_access, phymodAnTimerLinkFailInhibitLtPam4, link_fail_wait_time));
    }

    if (an_control.enable) {
        /* Update FEC usage in WB.
         * If AN require RS544, set FEC type to RS544 in WB.
         * Else if AN requrie RS528, set FEC type to RS528 in WB.
         * Otherwise, set to FEC_NONE, indicating the port
         * does not have FEC restriction within the MPP.
         */
        if (rs544_req) {
            fec = PORTMOD_PORT_PHY_FEC_RS_544;
        } else if (rs272_req) {
            fec = PORTMOD_PORT_PHY_FEC_RS_272;
        } else if (rs528_req) {
            fec = PORTMOD_PORT_PHY_FEC_RS_FEC;
        } else {
            fec = PORTMOD_PORT_PHY_FEC_NONE;
        }
        for (i = 0 ; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
            if ((phy_access.access.lane_mask >> i) & 0x1) {
                _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_LANE2FEC_SET(unit, pm_info, i, fec));
            }
        }

    }

    if (is_400g_an) {
        _SOC_IF_ERR_EXIT
            (portmod_portmon_disable_get(unit, &portmon_disable));
        if (portmon_disable) {
            _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                               (_SOC_MSG("port %d: 400G AN can`t be supported without portmod thread."), port));
        }
    }
    /* update the 400G An enabled */
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_ENABLE_SET(unit, pm_info, is_400g_an, port_index));
    /* if 400G an enabled, needs to enable call back */
    /* call back register should happen after 400G AN ENABLE is set */
    if (is_400g_an) {
         pm8x50_flexe_gen2_port_400g_an_state_t an_state = PM8X50_FLEXE_GEN2_PORT_AN_CHECK_PAGE_COMPLETE;
         uint32 an_wait_link_count = 0;
        /* update the 400G An state */
        _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_STATE_SET(unit, pm_info, an_state));
        _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_WAIT_LINK_COUNT_SET(unit, pm_info, an_wait_link_count));
        _SOC_IF_ERR_EXIT(portmod_port_periodic_callback_register(unit, port, pm8x50_flexe_gen2_periodic_callback, PORTMOD_THREAD_INTERVAL_1s));
    }


exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_autoneg_get(int unit, int port, pm_info_t pm_info,
                                    uint32 phy_flags,
                                    phymod_autoneg_control_t* an)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, port_index, is_400g_an = 0;
    uint32 bitmap;
    uint32_t an_done;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_autoneg_get(&phy_access, an, &an_done));

    /* next need to check 400G AN enabled or not */
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_ENABLE_GET(unit, pm_info, &is_400g_an, port_index));
    if (is_400g_an) {
         an->enable = 1;
         an->an_mode = phymod_AN_MODE_CL73_MSA;
    }


exit:
    SOC_FUNC_RETURN;
}

/*Get autoneg status*/
int pm8x50_flexe_gen2_port_autoneg_status_get(int unit, int port, pm_info_t pm_info,
                                           phymod_autoneg_status_t* an_status)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, port_index, is_400g_an = 0, an_400g_state;
    uint32 bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(phymod_phy_autoneg_status_get(&phy_access, an_status));
    /* TSCBH does not have interface type support */
    if (!(an_status->enabled && an_status->locked)) {
        /* upper layer should not rely on the
         * data rate if the AN is not locked
         */
        an_status->data_rate = 0;
        an_status->interface = phymodInterfaceBypass;
    }
    /* next need to check 400G AN enabled or not */
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_ENABLE_GET(unit, pm_info, &is_400g_an, port_index));
    if (is_400g_an) {
        an_status->data_rate = 400000;
        an_status->enabled = 1;
        an_status->locked = 0;
        _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_STATE_GET(unit, pm_info, &an_400g_state));
        if (an_400g_state == PM8X50_FLEXE_GEN2_PORT_AN_COMPLETE) {
            an_status->locked = 1;
        }
    }

exit:
    SOC_FUNC_RETURN;
}

/*get link status*/
int pm8x50_flexe_gen2_port_link_get(int unit, int port, pm_info_t pm_info,
                                 int flags, int* link)
{
    int latch_val = 0;
    uint32 rval, bitmap;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, port_index, is_400g_an = 0, an_400g_state;
    uint32 reg_val, is_400g_port = 0;
    int phy_acc;

    SOC_INIT_FUNC_DEFS;

    if (SAL_BOOT_SIMULATION){
        _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_mac_link_get(unit, port, pm_info, link));
    } else {
        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
        _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                        &params, 1, &phy_access, &nof_phys, NULL));
        _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_phy_access_get(unit, port, pm_info, &phy_acc));
        _SOC_IF_ERR_EXIT(READ_CDPORT_MODE_REGr(unit, phy_acc, &reg_val));
        is_400g_port = soc_reg_field_get(unit, CDPORT_MODE_REGr, reg_val, SINGLE_PORT_MODE_SPEED_400Gf);
        if (is_400g_port) {
            _SOC_IF_ERR_EXIT(
                pm8x50_flexe_gen2_port_link_latch_down_get(unit, port, pm_info, PORTMOD_PORT_LINK_LATCH_DOWN_F_CLEAR, &latch_val));
            if (latch_val) {
                *link = 0;
            } else {
                _SOC_IF_ERR_EXIT(
                    READ_CDPORT_XGXS0_LN0_STATUS_REGr(unit, port, &rval));
                *link = soc_reg_field_get(unit, CDPORT_XGXS0_LN0_STATUS_REGr, rval, LINK_STATUSf);
            }
            /* next need to check 400G AN enabled or not */
            _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, &bitmap));
            _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_ENABLE_GET(unit, pm_info, &is_400g_an, port_index));
            if (is_400g_an) {
                _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_STATE_GET(unit, pm_info, &an_400g_state));
                if (an_400g_state != PM8X50_FLEXE_GEN2_PORT_AN_COMPLETE) {
                     *link = 0;
                }
            }
        } else {
            _SOC_IF_ERR_EXIT(phymod_phy_link_status_get(&phy_access,
                                                       (uint32_t *) link));
        }
    }
exit:
    SOC_FUNC_RETURN;
}

/*get latch down link status (whether link was ever down since last clear)*/
int pm8x50_flexe_gen2_port_link_latch_down_get(int unit, int port,
                                            pm_info_t pm_info,
                                            uint32 flags, int* link)
{
    int first_index;
    uint32 rval, bitmap;
    soc_field_t status_field;
    SOC_INIT_FUNC_DEFS;

    *link = 0;

    _SOC_IF_ERR_EXIT(
        _pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &first_index, &bitmap));

     switch(first_index) {
        case 0:
            status_field = PORT0_LINKSTATUSf;
            break;

        case 1:
            status_field = PORT1_LINKSTATUSf;
            break;

        case 2:
            status_field = PORT2_LINKSTATUSf;
            break;

        case 3:
            status_field = PORT3_LINKSTATUSf;
            break;

        case 4:
            status_field = PORT4_LINKSTATUSf;
            break;

        case 5:
            status_field = PORT5_LINKSTATUSf;
            break;

        case 6:
            status_field = PORT6_LINKSTATUSf;
            break;

        case 7:
            status_field = PORT7_LINKSTATUSf;
            break;

        default:
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                     (_SOC_MSG("Port %d, failed to get port index"), port));
    }

    _SOC_IF_ERR_EXIT(READ_CDPORT_LINKSTATUS_DOWNr(unit, port, &rval));

    (*link) = soc_reg_field_get(unit, CDPORT_LINKSTATUS_DOWNr,
                                        rval, status_field);

    if (PORTMOD_PORT_LINK_LATCH_DOWN_F_CLEAR & flags) {
        /* CDPORT_LINKSTATUS_DOWN is sticky register.
         * Need to write 1b'1 to clear each field.
         */
        rval = 0;

        soc_reg_field_set(unit, CDPORT_LINKSTATUS_DOWNr,
                                        &rval, status_field, 1);

        _SOC_IF_ERR_EXIT(
                WRITE_CDPORT_LINKSTATUS_DOWNr(unit, port, rval));
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
int pm8x50_flexe_gen2_port_phy_link_up_event(int unit, int port, pm_info_t pm_info)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    phymod_autoneg_status_t an_status;
    int nof_phys, port_index = -1;
    int mac_stage_id, phy_acc;
    uint32 reg_val;
    uint32 timesync_config, bitmap, flags = 0;
    int num_advert, num_remote, is_400g_an;
    portmod_port_speed_ability_t advert_ability[PM8x50_FLEXE_GEN2_MAX_AN_ABILITY];
    portmod_port_speed_ability_t remote_ability[PM8x50_FLEXE_GEN2_MAX_AN_ABILITY];
    portmod_pause_control_t pause_ctrl;
    portmod_port_phy_pause_t pause_local, pause_remote;
    uint8 rx_pause, tx_pause;
    uint32_t timesync_adj_flags = 0;
    phymod_timesync_adjust_config_info_t timesync_adjust_config;


    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, &bitmap));

    /* 1. Update for AN ports */
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_autoneg_status_get(unit, port, pm_info, &an_status));
    if ((an_status.enabled && an_status.locked)) {
        /* read the 400G an enabled */
        _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_400G_AN_ENABLE_GET(unit, pm_info, &is_400g_an, port_index));
        /*!
         * 1.1 Update port mode.
         * For PM8x50 autoneg, once the port resolves and link up,
         * cdmac_port_mode needs to be updated based on the PCS final port mode.
         */
        if (!is_400g_an) {
            mac_stage_id = port_index / CDMAC_NUM_LANES;
            _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_phy_access_get(unit, port, pm_info, &phy_acc));

            _SOC_IF_ERR_EXIT(READ_CDPORT_MODE_REGr(unit, phy_acc, &reg_val));

            if (mac_stage_id) {
                /* MAC1 is single-port mode */
                soc_reg_field_set(unit, CDPORT_MODE_REGr, &reg_val, MAC1_PORT_MODEf, an_status.resolved_port_mode);
            } else {
                /* MAC0 is single-port mode */
                soc_reg_field_set(unit, CDPORT_MODE_REGr, &reg_val, MAC0_PORT_MODEf, an_status.resolved_port_mode);
            }
            _SOC_IF_ERR_EXIT(WRITE_CDPORT_MODE_REGr(unit, phy_acc, reg_val));
        }

        /*!
         * 1.2 Update pause settings.
         * Pause resolution rule refers to $SDK/doc/pause-resolution.txt.
         */
        _SOC_IF_ERR_EXIT
            (pm8x50_flexe_gen2_port_autoneg_ability_advert_get(unit, port, pm_info,
                                                    PM8x50_FLEXE_GEN2_MAX_AN_ABILITY,
                                                    advert_ability, &num_advert));
        _SOC_IF_ERR_EXIT
            (pm8x50_flexe_gen2_port_autoneg_ability_remote_get(unit, port, pm_info,
                                                    PM8x50_FLEXE_GEN2_MAX_AN_ABILITY,
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
            (pm8x50_flexe_gen2_port_pause_control_get(unit, port, pm_info, &pause_ctrl));
        if ((pause_ctrl.rx_enable != rx_pause) ||
            (pause_ctrl.tx_enable != tx_pause)) {
            pause_ctrl.rx_enable = rx_pause;
            pause_ctrl.tx_enable = tx_pause;
            _SOC_IF_ERR_EXIT
                (pm8x50_flexe_gen2_port_pause_control_set(unit, port, pm_info, &pause_ctrl));
        }
    }


    /* 2. Adjust timesync settings for 1588 port. */
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_TIMESYNC_CONFIG_GET(unit, pm_info, &timesync_config, port_index));

    if (PORTMOD_PORT_TIMESYNC_CONFIG_ENABLE_GET(timesync_config)) {
        /*
         * If 1588 is enabled on the port, need to adjust timesync during link up event.
         * rx_ts_update will be enabled at the end of adjust function.
         */
        if (PORTMOD_PORT_TIMESYNC_CONFIG_SFD_GET(timesync_config)){
            PHYMOD_TIMESYNC_F_SOP_CLR(timesync_adj_flags);
        } else {
            PHYMOD_TIMESYNC_F_SOP_SET(timesync_adj_flags);
        }

        _SOC_IF_ERR_EXIT
                (phymod_timesync_adjust_config_info_t_init(&timesync_adjust_config));

        if (PORTMOD_PORT_TIMESYNC_CONFIG_COMP_MODE_LATEST_LANE_GET(timesync_config)){
            timesync_adjust_config.am_norm_mode = phymodTimesyncCompensationModeLatestlane;
        } else {
            timesync_adjust_config.am_norm_mode = phymodTimesyncCompensationModeEarliestLane;
        }

        _SOC_IF_ERR_EXIT
            (phymod_timesync_adjust_set( &phy_access, timesync_adj_flags, &timesync_adjust_config));

        PHYMOD_TIMESYNC_ENABLE_F_RX_SET(flags);
        _SOC_IF_ERR_EXIT(phymod_timesync_enable_set(&phy_access, flags, 1));
        /* Wait for one AM spacing time */
        sal_usleep(500);
    }

exit:
    SOC_FUNC_RETURN;
}

/* link down event */
/* 1. 1588 port: Disable rx_ts_update.
 */
int pm8x50_flexe_gen2_port_phy_link_down_event(int unit, int port, pm_info_t pm_info)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, port_index = -1;
    uint32 timesync_config, bitmap, flags = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_TIMESYNC_CONFIG_GET(unit, pm_info, &timesync_config, port_index));

    if (PORTMOD_PORT_TIMESYNC_CONFIG_ENABLE_GET(timesync_config)) {
        /* If link down and 1588 is enabled on the port,
         * Need to disable rx_ts_update.
         */
        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
        _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                    &params, 1, &phy_access, &nof_phys, NULL));
        PHYMOD_TIMESYNC_ENABLE_F_RX_SET(flags);
        if (bitmap == 0xff) {
            /* Set RX_PCS_ENABLE flag  to  disable PCS to MAC only for 400G 8 lanes case.*/
            PHYMOD_TIMESYNC_ENABLE_F_RX_PCS_ENABLE_SET(flags);
        }
        _SOC_IF_ERR_EXIT(phymod_timesync_enable_set(&phy_access, flags, 0));
    }

exit:
    SOC_FUNC_RETURN;
}

/*PRBS configuration set/get*/
int pm8x50_flexe_gen2_port_prbs_config_set(int unit, int port, pm_info_t pm_info,
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
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_prbs_config_set(&phy_access, flags, config));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_prbs_config_get(int unit, int port, pm_info_t pm_info,
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
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_prbs_config_get(&phy_access, flags, config));

exit:
    SOC_FUNC_RETURN;
}

/*PRBS enable set/get*/
int pm8x50_flexe_gen2_port_prbs_enable_set(int unit, int port, pm_info_t pm_info,
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
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_prbs_enable_set(&phy_access, flags, enable));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_prbs_enable_get(int unit, int port, pm_info_t pm_info,
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
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_prbs_enable_get(&phy_access, flags,
                                                (uint32_t *) enable));

exit:
    SOC_FUNC_RETURN;
}

/*PRBS status get*/
int pm8x50_flexe_gen2_port_prbs_status_get(int unit, int port, pm_info_t pm_info,
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
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_prbs_status_get(&phy_access, flags, status));

exit:
    SOC_FUNC_RETURN;
}

/*Port tx taps set\get*/
int pm8x50_flexe_gen2_port_tx_set(int unit, int port, pm_info_t pm_info, const phymod_tx_t* tx)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_tx_set(&phy_access, tx));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_tx_get(int unit, int port, pm_info_t pm_info, phymod_tx_t* tx)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_tx_get(&phy_access, tx));

exit:
    SOC_FUNC_RETURN;
}

/*Number of lanes get*/
int pm8x50_flexe_gen2_port_nof_lanes_get(int unit, int port, pm_info_t pm_info,
                              int* nof_lanes)
{
    int port_index;
    uint32_t bitmap, bcnt = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get (unit, port, pm_info, &port_index, &bitmap));

    while (bitmap) {
        if (bitmap & 0x1) bcnt++;
        bitmap >>= 1;
    }
    *nof_lanes = bcnt;

exit:
    SOC_FUNC_RETURN;
}


/*Filter packets smaller than the specified threshold*/
int pm8x50_flexe_gen2_port_runt_threshold_set(int unit, int port,
                                           pm_info_t pm_info,
                                           int value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cdmac_runt_threshold_set(unit, port, value));
exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_runt_threshold_get(int unit, int port, pm_info_t pm_info,
                                           int* value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cdmac_runt_threshold_get(unit, port, value));
exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_tx_threshold_set(int unit, int port,
                                   pm_info_t pm_info,
                                   int value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cdmac_tx_threshold_set(unit, port, value));
exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_tx_threshold_get(int unit, int port, pm_info_t pm_info,
                                   int* value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cdmac_tx_threshold_get(unit, port, value));
exit:
    SOC_FUNC_RETURN;
}

/*Filter packets bigger than the specified value*/
int pm8x50_flexe_gen2_port_max_packet_size_set(int unit, int port, pm_info_t pm_info,
                                    int value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cdmac_rx_max_size_set(unit, port , value));
exit:
    SOC_FUNC_RETURN;
}
int pm8x50_flexe_gen2_port_max_packet_size_get(int unit, int port, pm_info_t pm_info,
                                    int* value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cdmac_rx_max_size_get(unit, port , value));
exit:
    SOC_FUNC_RETURN;
}

/*
 * TX pad packets to the specified size.
 * values smaller than 17 means pad is disabled.
 */
int pm8x50_flexe_gen2_port_pad_size_set(int unit, int port, pm_info_t pm_info, int value)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_pad_size_set(unit, port , value));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_pad_size_get(int unit, int port, pm_info_t pm_info, int* value)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_pad_size_get(unit, port , value));

exit:
    SOC_FUNC_RETURN;
}

/*set/get the MAC source address that will be sent in case of Pause/LLFC*/
int pm8x50_flexe_gen2_port_tx_mac_sa_set(int unit, int port, pm_info_t pm_info,
                                      sal_mac_addr_t mac_sa)
{
   SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_tx_mac_sa_set(unit, port, mac_sa));

exit:
    SOC_FUNC_RETURN;
}
int pm8x50_flexe_gen2_port_tx_mac_sa_get(int unit, int port, pm_info_t pm_info,
                                      sal_mac_addr_t mac_sa)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_tx_mac_sa_get(unit, port, mac_sa));

exit:
    SOC_FUNC_RETURN;
}

/* set/get SA recognized for MAC control packets
 * in addition to the standard 0x0180C2000001
 */
int pm8x50_flexe_gen2_port_rx_mac_sa_set(int unit, int port, pm_info_t pm_info,
                                      sal_mac_addr_t mac_sa)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_rx_mac_sa_set(unit, port, mac_sa));

exit:
    SOC_FUNC_RETURN;
}
int pm8x50_flexe_gen2_port_rx_mac_sa_get(int unit, int port, pm_info_t pm_info,
                                      sal_mac_addr_t mac_sa)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_rx_mac_sa_get(unit, port, mac_sa));

exit:
    SOC_FUNC_RETURN;
}

/*set/get Average inter-packet gap*/
int pm8x50_flexe_gen2_port_tx_average_ipg_set(int unit, int port, pm_info_t pm_info,
                                           int value)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_tx_average_ipg_set(unit, port, value));

exit:
    SOC_FUNC_RETURN;


}
int pm8x50_flexe_gen2_port_tx_average_ipg_get(int unit, int port, pm_info_t pm_info,
                                           int* value)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_tx_average_ipg_get(unit, port, value));

exit:
    SOC_FUNC_RETURN;

}

/*local fault set/get*/
int pm8x50_flexe_gen2_port_local_fault_control_set(int unit, int port, pm_info_t pm_info,
                                         const portmod_local_fault_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_local_fault_control_set(unit, port, control));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_local_fault_control_get(int unit, int port, pm_info_t pm_info,
                                              portmod_local_fault_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_local_fault_control_get(unit, port, control));

exit:
    SOC_FUNC_RETURN;

}

/*remote fault set/get*/
int pm8x50_flexe_gen2_port_remote_fault_control_set(int unit, int port, pm_info_t pm_info,
                                         const portmod_remote_fault_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_remote_fault_control_set(unit, port, control));

exit:
    SOC_FUNC_RETURN;

}
int pm8x50_flexe_gen2_port_remote_fault_control_get(int unit, int port, pm_info_t pm_info,
                                             portmod_remote_fault_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_remote_fault_control_get(unit, port, control));

exit:
    SOC_FUNC_RETURN;
}

/*local fault steatus get*/
int pm8x50_flexe_gen2_port_local_fault_status_get(int unit, int port, pm_info_t pm_info,
                                               int* value)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_local_fault_status_get(unit, port, value));

exit:
    SOC_FUNC_RETURN;
}

/*remote fault status get*/
int pm8x50_flexe_gen2_port_remote_fault_status_get(int unit, int port, pm_info_t pm_info,
                                                int* value)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_remote_fault_status_get(unit, port, value));

exit:
    SOC_FUNC_RETURN;
}

/*local fault steatus clear, there are no clear register for CDMAC, just clear by read*/
int pm8x50_flexe_gen2_port_local_fault_status_clear(int unit, int port, pm_info_t pm_info)
{
    int value;
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_local_fault_status_get(unit, port, &value));

exit:
    SOC_FUNC_RETURN;
}

/*remote fault status clear, there are no clear register for CDMAC, just clear by read*/
int pm8x50_flexe_gen2_port_remote_fault_status_clear(int unit, int port,
                                          pm_info_t pm_info)
{
    int value;
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_remote_fault_status_get(unit, port, &value));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_local_fault_force_set(int unit, int port, pm_info_t pm_info, int enable)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_link_fault_os_set(unit, port, 0, enable));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_local_fault_force_get(int unit, int port, pm_info_t pm_info, int * enable)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_link_fault_os_get(unit, port, 0, (uint32 *)enable));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_remote_fault_force_set(int unit, int port, pm_info_t pm_info, int enable)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_link_fault_os_set(unit, port, 1, enable));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_remote_fault_force_get(int unit, int port, pm_info_t pm_info, int * enable)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_link_fault_os_get(unit, port, 1, (uint32 *)enable));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_pause_control_set(int unit, int port, pm_info_t pm_info,
                                          const portmod_pause_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cdmac_pause_control_set(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_pause_control_get(int unit, int port, pm_info_t pm_info,
                                          portmod_pause_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cdmac_pause_control_get(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_pfc_control_set(int unit, int port, pm_info_t pm_info,
                                        const portmod_pfc_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cdmac_pfc_control_set(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_pfc_control_get(int unit, int port, pm_info_t pm_info,
                                        portmod_pfc_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cdmac_pfc_control_get(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

/*get port cores' phymod access*/
int pm8x50_flexe_gen2_port_core_access_get(int unit, int port, pm_info_t pm_info,
                                        int phyn, int max_cores,
                                        phymod_core_access_t* core_access_arr,
                                        int* nof_cores, int* is_most_ext)
{
    SOC_INIT_FUNC_DEFS;

    /* There are only internal phys(1) on PM8x50, setting phyn to 0 */
    if(phyn < 0)
    {
        phyn = 0;
    }

    _SOC_IF_ERR_EXIT(phymod_core_access_t_init(&core_access_arr[0]));

    if( phyn == 0 ){
        sal_memcpy(&core_access_arr[0],
                   &(PM_8x50_FLEXE_GEN2_INFO(pm_info)->int_core_access),
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
int pm8x50_flexe_gen2_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info,
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
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index,
                                            &iphy_lane_mask));

    rv = PM8x50_FLEXE_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index);
    _SOC_IF_ERR_EXIT(rv);

    for( i = 0 ; i < max_phys; i++) {
        _SOC_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access[i]));
    }

    *nof_phys = 1;
    /* internal core */
    sal_memcpy (&phy_access[0], &(PM_8x50_FLEXE_GEN2_INFO(pm_info)->int_core_access),
                sizeof(phymod_phy_access_t));
    phy_access[0].access.lane_mask = iphy_lane_mask;
    if (params->lane != -1) {
        lane_count = 0;
        phy_access[0].access.lane_mask = 0;
        for (i = 0; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; ++i)
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
int pm8x50_flexe_gen2_pm_phy_lane_access_get(int unit, int pm_id, pm_info_t pm_info,
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
    sal_memcpy (&phy_access[0], &(PM_8x50_FLEXE_GEN2_INFO(pm_info)->int_core_access),
                sizeof(phymod_phy_access_t));

    if (params->lane != -1) {
        /*
         * If lane index is not -1, need to check if there is any port attched on
         * the given lane. If there is active port on the lane, need to check if
         * the lane is working at bypass mode.
         */
        _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_LANE2PORT_GET(unit, pm_info, params->lane, &port));
        if (port > 0) {
            _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index,
                                                    &iphy_lane_mask));
            _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));
            if (is_bypassed) {
                PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_SET(phy_access[0].device_op_mode);
            }
        }
        phy_access[0].access.lane_mask = (0x1 << (params->lane));
    } else {
        /*
         * If lane index is -1, assume all the lanes are accessed
         */
        phy_access[0].access.lane_mask = (0x1 << PM8X50_FLEXE_GEN2_PORTS_PER_CORE) - 1;
    }
    /* only line is availabe for internal. */
    phy_access[0].port_loc = phymodPortLocLine;

    if (is_most_ext) {
        *is_most_ext = 1;
    }
exit:
    SOC_FUNC_RETURN;
}

/*Port duplex set\get*/
int pm8x50_flexe_gen2_port_duplex_set(int unit, int port, pm_info_t pm_info, int enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_duplex_set(unit, port, enable));
exit:
    SOC_FUNC_RETURN
}

int pm8x50_flexe_gen2_port_duplex_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_duplex_get(unit, port, enable));
exit:
    SOC_FUNC_RETURN
}

/*Port PHY Control register read*/
int pm8x50_flexe_gen2_port_phy_reg_read(int unit, int port, pm_info_t pm_info, int lane,
                                     int flags, int reg_addr, uint32* value)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    uint32 lane_mask;
    int start_lane;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    /* get start lane and lane mask */
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &start_lane, &lane_mask));

    PM8X50_FLEXE_GEN2_PHY_REG_SET(reg_addr);

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
int pm8x50_flexe_gen2_port_phy_reg_write(int unit, int port, pm_info_t pm_info,
                                      int lane, int flags, int reg_addr, uint32 value)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    uint32 lane_mask;
    int start_lane;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    /* get start lane and lane mask */
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &start_lane, &lane_mask));

    PM8X50_FLEXE_GEN2_PHY_REG_SET(reg_addr);

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
int pm8x50_flexe_gen2_port_reset_set(int unit, int port,
                                  pm_info_t pm_info, int mode,
                                  int opcode, int value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_phy_port_reset_set(unit, port, pm_info,
                                                mode, opcode, value));
exit:
    SOC_FUNC_RETURN
}
int pm8x50_flexe_gen2_port_reset_get(int unit, int port,
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
int pm8x50_flexe_gen2_port_drv_name_get(int unit, int port,
                                     pm_info_t pm_info,
                                     char* name, int len)
{
    strncpy(name, "PM8X50_FLEXE_GEN2 Driver", len);
    return (SOC_E_NONE);
}

/*set/get port fec enable according to local/remote FEC ability*/
int pm8x50_flexe_gen2_port_fec_enable_set(int unit, int port, pm_info_t pm_info,
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
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_fec_enable_set(&phy_access, enable));

exit:
    SOC_FUNC_RETURN;
}
int pm8x50_flexe_gen2_port_fec_enable_get(int unit, int port, pm_info_t pm_info,
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
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_fec_enable_get(&phy_access, enable));

exit:
    SOC_FUNC_RETURN;
}

/*set/get pass control frames.*/
int pm8x50_flexe_gen2_port_rx_control_set(int unit, int port, pm_info_t pm_info,
                                       const portmod_rx_control_t* rx_ctrl)
{
    SOC_INIT_FUNC_DEFS;

    if (rx_ctrl->flags & PORTMOD_MAC_PASS_CONTROL_FRAME) {
        _SOC_IF_ERR_EXIT(cdmac_rsv_mask_control_set(unit, port,
                                                    CDMAC_RSV_MASK_CONTROL_FRAME,
                                                    rx_ctrl->pass_control_frames));
        _SOC_IF_ERR_EXIT(cdmac_pass_control_frame_set(unit, port, rx_ctrl->pass_control_frames));
    }

    if (rx_ctrl->flags & PORTMOD_MAC_PASS_PFC_FRAME) {
        _SOC_IF_ERR_EXIT(cdmac_rsv_mask_control_set(unit, port,
                                                    CDMAC_RSV_MASK_PFC_FRAME,
                                                    rx_ctrl->pass_pfc_frames));
        _SOC_IF_ERR_EXIT(cdmac_pass_pfc_frame_set(unit, port, rx_ctrl->pass_pfc_frames));
    }

    if (rx_ctrl->flags & PORTMOD_MAC_PASS_PAUSE_FRAME) {
        _SOC_IF_ERR_EXIT(cdmac_rsv_mask_control_set(unit, port,
                                                    CDMAC_RSV_MASK_PAUSE_FRAME,
                                                    rx_ctrl->pass_pause_frames));
        _SOC_IF_ERR_EXIT(cdmac_pass_pause_frame_set(unit, port, rx_ctrl->pass_pause_frames));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_rx_control_get(int unit, int port, pm_info_t pm_info,
                                       portmod_rx_control_t* rx_ctrl)
{
    int rx_pass_control_frames, rx_pass_pfc_frames, rx_pass_pause_frames;
    uint32 rsv_pass_control_frames, rsv_pass_pfc_frames, rsv_pass_pause_frames;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_pass_control_frame_get(unit, port, &rx_pass_control_frames));
    _SOC_IF_ERR_EXIT(cdmac_pass_pfc_frame_get(unit, port, &rx_pass_pfc_frames));
    _SOC_IF_ERR_EXIT(cdmac_pass_pause_frame_get(unit, port, &rx_pass_pause_frames));
    _SOC_IF_ERR_EXIT(cdmac_rsv_mask_control_get(unit, port, CDMAC_RSV_MASK_CONTROL_FRAME,
                                                &rsv_pass_control_frames));
    _SOC_IF_ERR_EXIT(cdmac_rsv_mask_control_get(unit, port, CDMAC_RSV_MASK_PFC_FRAME,
                                                &rsv_pass_pfc_frames));
    _SOC_IF_ERR_EXIT(cdmac_rsv_mask_control_get(unit, port, CDMAC_RSV_MASK_PAUSE_FRAME,
                                                &rsv_pass_pause_frames));

    rx_ctrl->pass_control_frames = rx_pass_control_frames & rsv_pass_control_frames;
    rx_ctrl->pass_pfc_frames = rx_pass_pfc_frames & rsv_pass_pfc_frames;
    rx_ctrl->pass_pause_frames = rx_pass_pause_frames & rsv_pass_pause_frames;
exit:
    SOC_FUNC_RETURN;
}

/*set PFC config registers.*/
int pm8x50_flexe_gen2_port_pfc_config_set(int unit, int port, pm_info_t pm_info,
                                       const portmod_pfc_config_t* pfc_cfg)
{
    SOC_INIT_FUNC_DEFS;

    if (pfc_cfg->classes != 8) {
        return SOC_E_PARAM;
    }
    _SOC_IF_ERR_EXIT(cdmac_pfc_config_set(unit, port, pfc_cfg));
    /* set the pfc frame control in cdmac rsv */
    _SOC_IF_ERR_EXIT(cdmac_rsv_mask_control_set(unit, port,
                                                CDMAC_RSV_MASK_PFC_FRAME,
                                                pfc_cfg->rxpass));
exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_pfc_config_get(int unit, int port, pm_info_t pm_info,
                                       portmod_pfc_config_t* pfc_cfg)
{
    SOC_INIT_FUNC_DEFS;

    pfc_cfg->classes = 8;
    _SOC_IF_ERR_EXIT(cdmac_pfc_config_get(unit, port, pfc_cfg));
    _SOC_IF_ERR_EXIT(cdmac_rsv_mask_control_get(unit, port,
                                                CDMAC_RSV_MASK_PFC_FRAME,
                                                &pfc_cfg->rxpass));
exit:
    SOC_FUNC_RETURN;
}

/*set Vlan Inner/Outer tag.*/
int pm8x50_flexe_gen2_port_vlan_tag_set(int unit, int port, pm_info_t pm_info,
                                     const portmod_vlan_tag_t* vlan_tag)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_rx_vlan_tag_set (unit, port,
                                            vlan_tag->outer_vlan_tag,
                                            vlan_tag->inner_vlan_tag));
exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_vlan_tag_get(int unit, int port, pm_info_t pm_info,
                                     portmod_vlan_tag_t* vlan_tag)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_rx_vlan_tag_get (unit, port,
                                   (int*)&vlan_tag->outer_vlan_tag,
                                   (int*)&vlan_tag->inner_vlan_tag));
exit:
    SOC_FUNC_RETURN;
}

/*Toggle Lag Failover Status.*/
int pm8x50_flexe_gen2_port_lag_failover_status_toggle(int unit, int port,
                                                   pm_info_t pm_info)
{
    int phy_acc;
    uint32_t rval;
    int link = 0;
    soc_timeout_t to;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_phy_access_get(unit, port, pm_info, &phy_acc));

    /* Link status to IPIPE is updated by H/W and driven based on both the
     * rising edge of CDPORT_LAG_FAILOVER_CONFIG.LINK_STATUS_UP and
     * actual link up status from Serdes.
     * In some loopback scenarios it may take longer time to see Serdes link up status.
     */
    soc_timeout_init(&to, 10000, 0);

    do {
        _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_mac_link_get(unit, port, pm_info, &link));
        if (link) {
            break;
        }
    } while(!soc_timeout_check(&to));

    /* Toggle link_status_up bit to notify IPIPE on link up */
    _SOC_IF_ERR_EXIT(READ_CDPORT_LAG_FAILOVER_CONFIGr(unit, phy_acc, &rval));
    soc_reg_field_set(unit, CDPORT_LAG_FAILOVER_CONFIGr, &rval, LINK_STATUS_UPf, 1);
    _SOC_IF_ERR_EXIT(WRITE_CDPORT_LAG_FAILOVER_CONFIGr(unit, phy_acc, rval));
    soc_reg_field_set(unit, CDPORT_LAG_FAILOVER_CONFIGr, &rval, LINK_STATUS_UPf, 0);
    _SOC_IF_ERR_EXIT(WRITE_CDPORT_LAG_FAILOVER_CONFIGr(unit, phy_acc, rval));

exit:
    SOC_FUNC_RETURN;
}

/*Toggle Lag Failover loopback set / get.*/
int pm8x50_flexe_gen2_port_lag_failover_loopback_set(int unit, int port,
                                                  pm_info_t pm_info,
                                                  int value)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_lag_failover_loopback_set(unit, port, value));
exit:
    SOC_FUNC_RETURN;
}
int pm8x50_flexe_gen2_port_lag_failover_loopback_get(int unit, int port,
                                                  pm_info_t pm_info,
                                                  int* value)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_lag_failover_loopback_get(unit, port, value));
exit:
    SOC_FUNC_RETURN;
}

/*set modid field.*/
int pm8x50_flexe_gen2_port_mode_set(int unit, int port, pm_info_t pm_info,
                                    const portmod_port_mode_info_t* mode)
{

    return (SOC_E_NONE);
}

STATIC
int _cdport_flexe_gen2_mode_get(int unit, int port, pm_info_t pm_info,
                             int first_phy_index, portmod_port_mode_info_t *p_mode)
{
    uint32 reg_val = 0, port_mode = 0;
    int phy_acc, mac_id = 0, this_phy_index;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_phy_access_get(unit, port, pm_info, &phy_acc));

    _SOC_IF_ERR_EXIT(READ_CDPORT_MODE_REGr(unit, phy_acc, &reg_val));
    port_mode = soc_reg_field_get(unit, CDPORT_MODE_REGr, reg_val, SINGLE_PORT_MODE_SPEED_400Gf);
    if (port_mode){
        p_mode->cur_mode = portmodPortModeSingle;
        p_mode->lanes = 8;
        p_mode->port_index = 0;
    } else {
        mac_id = first_phy_index / CDMAC_NUM_LANES;
        this_phy_index = first_phy_index % CDMAC_NUM_LANES;
        if (mac_id) {
            /* CDMAC_1 Port Mode */
            port_mode = soc_reg_field_get(unit, CDPORT_MODE_REGr, reg_val, MAC1_PORT_MODEf);
        } else {
            /* CDMAC_0 Port Mode*/
            port_mode = soc_reg_field_get(unit, CDPORT_MODE_REGr, reg_val, MAC0_PORT_MODEf);
        }
        switch (port_mode) {
            case 0:
                p_mode->cur_mode = portmodPortModeQuad;
                p_mode->lanes = 1;
                p_mode->port_index = this_phy_index;
                break;
            case 1:
                p_mode->cur_mode = portmodPortModeTri012;
                if ((this_phy_index == 0)||(this_phy_index == 1)) {
                    p_mode->lanes = 1;
                } else {
                    p_mode->lanes = 2;
                }
                p_mode->port_index = this_phy_index;
                break;
            case 2:
                p_mode->cur_mode = portmodPortModeTri023;
                if ((this_phy_index == 2)||(this_phy_index == 3)) {
                    p_mode->lanes = 1;
                } else {
                    p_mode->lanes = 2;
                }
                p_mode->port_index = this_phy_index;
                break;
            case 3:
                p_mode->cur_mode = portmodPortModeDual;
                p_mode->lanes = 2;
                p_mode->port_index = this_phy_index;
                break;
            case 4:
                p_mode->cur_mode = portmodPortModeSingle;
                p_mode->lanes = 4;
                p_mode->port_index = this_phy_index;
                break;
            default:
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid port mode")));
                break;
        }
    }

exit:
    SOC_FUNC_RETURN;

}

int pm8x50_flexe_gen2_port_mode_get(int unit, int port, pm_info_t pm_info,
                                 portmod_port_mode_info_t* mode)
{
    int port_index;
    uint32_t bitmap;

    SOC_INIT_FUNC_DEFS;

     _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get (unit, port, pm_info, &port_index, &bitmap));
     _SOC_IF_ERR_EXIT(_cdport_flexe_gen2_mode_get(unit, port, pm_info, port_index, mode));

exit:
    SOC_FUNC_RETURN;
}

/*set port encap.*/
int pm8x50_flexe_gen2_port_encap_set(int unit, int port, pm_info_t pm_info,
                                  int flags, portmod_encap_t encap)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_encap_set(unit, port, flags, encap));
exit:
    SOC_FUNC_RETURN;
}
int pm8x50_flexe_gen2_port_encap_get(int unit, int port, pm_info_t pm_info,
                                  int* flags, portmod_encap_t* encap)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_encap_get(unit, port, flags, encap));
exit:
    SOC_FUNC_RETURN;
}

/*set/get hwfailover for trident.*/
int pm8x50_flexe_gen2_port_trunk_hwfailover_config_set(int unit, int port,
                                                    pm_info_t pm_info,
                                                    int hw_count)
{
    int old_failover_en=0, new_failover_en = 0;
    int old_link_status_sel=0, new_link_status_sel=0;
    int old_reset_flow_control=0, new_reset_flow_control = 0;
    int lag_failover_lpbk;
    SOC_INIT_FUNC_DEFS;

    if (hw_count) {
        new_failover_en        = 1;
        new_link_status_sel    = 1;
        new_reset_flow_control = 1;
    }

    _SOC_IF_ERR_EXIT(cdmac_lag_failover_loopback_get(unit, port,
                                                     &lag_failover_lpbk));

    if (lag_failover_lpbk) {
        return (SOC_E_NONE);
    }

    _SOC_IF_ERR_EXIT(cdmac_lag_failover_en_get(unit, port,
                                               &old_failover_en));
    _SOC_IF_ERR_EXIT(cdmac_sw_link_status_select_get(unit, port,
                                                 &old_link_status_sel));

    if (old_failover_en     != new_failover_en ||
        old_link_status_sel != new_link_status_sel) {

        _SOC_IF_ERR_EXIT(cdmac_sw_link_status_select_set(unit, port,
                                                    new_link_status_sel));
        _SOC_IF_ERR_EXIT(cdmac_lag_failover_en_set(unit, port,
                                                   new_failover_en));
    }

    _SOC_IF_ERR_EXIT(cdmac_reset_fc_timers_on_link_dn_get(unit,
                                       port, &old_reset_flow_control));
    if (old_reset_flow_control != new_reset_flow_control) {
        _SOC_IF_ERR_EXIT(cdmac_reset_fc_timers_on_link_dn_set(unit,
                                        port, new_reset_flow_control));
    }

exit:
    SOC_FUNC_RETURN;
}
int pm8x50_flexe_gen2_port_trunk_hwfailover_config_get(int unit, int port,
                                                    pm_info_t pm_info,
                                                    int* enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_lag_failover_en_get(unit, port, enable));
exit:
    SOC_FUNC_RETURN;
}

/*set/get hwfailover for trident.*/
int pm8x50_flexe_gen2_port_trunk_hwfailover_status_get(int unit, int port,
                                                    pm_info_t pm_info,
                                                    int* loopback)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_lag_failover_loopback_get(unit, port,
                                                     loopback));
exit:
    SOC_FUNC_RETURN;
}

/*set/get hwfailover for trident.*/
int pm8x50_flexe_gen2_port_diag_ctrl(int unit, int port, pm_info_t pm_info,
                                  uint32 inst, int op_type, int op_cmd,
                                  const void* arg)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    portmod_access_get_params_t params;
    int nof_phys = 0;

    SOC_INIT_FUNC_DEFS;
    /* Only internal phy supported. */
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                      &params, 1, phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(portmod_phy_port_diag_ctrl(unit, port, phy_access, nof_phys, inst, op_type, op_cmd, arg));

exit:
    SOC_FUNC_RETURN;

}

/*Get the reference clock value 156 or 125.*/
int pm8x50_flexe_gen2_port_ref_clk_get(int unit, int port,
                                    pm_info_t pm_info,
                                    int* ref_clk)
{
    *ref_clk = pm_info->pm_data.pm8x50_flexe_gen2_db->ref_clk;

    return (SOC_E_NONE);
}

/*Disable lag failover.*/
int pm8x50_flexe_gen2_port_lag_failover_disable(int unit, int port, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_lag_failover_disable(unit, port));

exit:
    SOC_FUNC_RETURN;
}

/*Disable lag failover.*/
int pm8x50_flexe_gen2_port_lag_remove_failover_lpbk_set(int unit, int port,
                                                     pm_info_t pm_info, int val)
{
   SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_lag_remove_failover_lpbk_set(unit, port, val));

exit:
    SOC_FUNC_RETURN;
}
int pm8x50_flexe_gen2_port_lag_remove_failover_lpbk_get(int unit, int port,
                                                     pm_info_t pm_info, int* val)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_lag_remove_failover_lpbk_get(unit, port, val));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_cntmaxsize_set(int unit, int port, pm_info_t pm_info, int val)
{
    return (cdmac_cntmaxsize_set(unit, port, val));
}

int pm8x50_flexe_gen2_port_cntmaxsize_get(int unit, int port, pm_info_t pm_info, int* val)
{
    return (cdmac_cntmaxsize_get(unit, port, val));
}

/*Get Info needed to restore after drain cells.*/
int pm8x50_flexe_gen2_port_drain_cell_get(int unit, int port, pm_info_t pm_info,
                                       portmod_drain_cells_t* drain_cells)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_drain_cell_get(unit, port, drain_cells));

exit:
    SOC_FUNC_RETURN;
}

/*Restore informaation after drain cells.*/
int pm8x50_flexe_gen2_port_drain_cell_stop(int unit, int port, pm_info_t pm_info,
                                        const portmod_drain_cells_t* drain_cells)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_drain_cell_stop(unit, port, drain_cells));

exit:
    SOC_FUNC_RETURN;
}

/*Restore informaation after drain cells.*/
int pm8x50_flexe_gen2_port_drain_cell_start(int unit, int port, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_drain_cell_start(unit, port));

exit:
    SOC_FUNC_RETURN;
}

/**/
int pm8x50_flexe_gen2_port_drain_cells_rx_enable(int unit, int port,
                                              pm_info_t pm_info,
                                              int rx_en)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_drain_cells_rx_enable(unit, port, rx_en));

exit:
    SOC_FUNC_RETURN;
}

/**/
int pm8x50_flexe_gen2_port_egress_queue_drain_rx_en(int unit, int port,
                                                 pm_info_t pm_info,
                                                 int rx_en)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_egress_queue_drain_rx_en(unit, port, rx_en));

exit:
    SOC_FUNC_RETURN;
}

/**/
int pm8x50_flexe_gen2_port_mac_ctrl_set(int unit, int port,
                                     pm_info_t pm_info,
                                     uint64 ctrl)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_mac_ctrl_set(unit, port, ctrl));

exit:
    SOC_FUNC_RETURN;
}

/**/
int pm8x50_flexe_gen2_port_txfifo_cell_cnt_get(int unit, int port,
                                            pm_info_t pm_info,
                                            uint32* cnt)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_txfifo_cell_cnt_get(unit, port, cnt));

exit:
    SOC_FUNC_RETURN;
}

/**/
int pm8x50_flexe_gen2_port_egress_queue_drain_get(int unit, int port,
                                               pm_info_t pm_info,
                                               uint64* ctrl, int* rxen)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_egress_queue_drain_get(unit, port, ctrl, rxen));

exit:
    SOC_FUNC_RETURN;
}

/**/
int pm8x50_flexe_gen2_port_mac_reset_set(int unit, int port, pm_info_t pm_info, int val)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_soft_reset_set(unit, port, val));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_mac_reset_get(int unit, int port, pm_info_t pm_info, int* val)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_soft_reset_get(unit, port, val));

    exit:
        SOC_FUNC_RETURN;
}

/*Check if MAC needs to be reset.*/
int pm8x50_flexe_gen2_port_mac_reset_check(int unit, int port,
                                        pm_info_t pm_info,
                                        int enable, int* reset)
{
    return (cdmac_reset_check(unit, port, enable, reset));
}

/**/
int pm8x50_flexe_gen2_port_core_num_get(int unit, int port,
                                     pm_info_t pm_info,
                                     int* core_num)
{
    *core_num = PM_8x50_FLEXE_GEN2_INFO(pm_info)->core_num;
    return (SOC_E_NONE);
}

/*get the speed for the specified port*/
int pm8x50_flexe_gen2_port_speed_get(int unit, int port, pm_info_t pm_info, int* speed)
{
    phymod_phy_speed_config_t speed_config;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    *speed = 0;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                   &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_t_init(&speed_config));
    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_get(&phy_access, &speed_config));

    *speed = speed_config.data_rate;
exit:
    SOC_FUNC_RETURN;
}

/*Port discard set*/
int pm8x50_flexe_gen2_port_discard_set(int unit, int port, pm_info_t pm_info, int discard)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_discard_set(unit, port, discard));

exit:
    SOC_FUNC_RETURN;
}

/*Port tx_en=0 and softreset mac*/
int pm8x50_flexe_gen2_port_tx_down(int unit, int port, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_tx_enable_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(cdmac_discard_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(cdmac_soft_reset_set(unit, port, 1));

exit:
    SOC_FUNC_RETURN;
}

/*port control phy timesync config set/get*/
int pm8x50_flexe_gen2_port_control_phy_timesync_set(int unit,
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
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
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

int pm8x50_flexe_gen2_port_control_phy_timesync_get(int unit,
                                                 int port, pm_info_t pm_info,
                                                 portmod_port_control_phy_timesync_t config,
                                                 uint64* value)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(portmod_common_control_phy_timesync_get(&phy_access, config, value));

exit:
    SOC_FUNC_RETURN;
}

/*"port timesync config set/get"*/
int pm8x50_flexe_gen2_port_timesync_config_set(int unit, int port, pm_info_t pm_info,
                                      const portmod_phy_timesync_config_t* config)
{
    int port_index, ts_enable_port_count;
    uint32 bitmap, one_step_req, is_one_step = 0, flags = 0;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, enable, is_enable;
    uint32_t timesync_config = 0;
    uint32 phy_flags = 0;
    phymod_autoneg_control_t an_control;
    phymod_phy_speed_config_t speed_config;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    enable = (config->flags & SOC_PORT_PHY_TIMESYNC_ENABLE) ? 1 : 0;

    _SOC_IF_ERR_EXIT
        (PM8x50_FLEXE_GEN2_TIMESYNC_CONFIG_GET(unit, pm_info, &timesync_config, port_index));
    is_enable = PORTMOD_PORT_TIMESYNC_CONFIG_ENABLE_GET(timesync_config);
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_TS_ENABLE_PORT_COUNT_GET(unit, pm_info, &ts_enable_port_count));

    if (!enable) {
        /* Disable 1588 */
        if (is_enable) {
            /* If 1588 is currently enabled on the port */
            /* 1. Disable One Stpe pipeline if it is enabled. */
            PHYMOD_TIMESYNC_ENABLE_F_ONE_STEP_PIPELINE_SET(flags);
            _SOC_IF_ERR_EXIT(phymod_timesync_enable_get(&phy_access, flags, &is_one_step));
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

            _SOC_IF_ERR_EXIT(phymod_timesync_enable_set(&phy_access, flags, 0));
        }
    } else {
        /* Enable 1588.
         * To be compatible with previous usage (no flag TWO_STEP_ENABLE), two-step will be enabled if ONE_STEP is 0, no matter TWO_STEP is 0 or 1.
         * If ONE_STEP and TWO_STEP both are 1, return error */
        if ((config->flags & SOC_PORT_PHY_TIMESYNC_ONE_STEP_ENABLE) && (config->flags & SOC_PORT_PHY_TIMESYNC_TWO_STEP_ENABLE)) {
            _SOC_IF_ERR_EXIT(SOC_E_PARAM);
        }

        if (is_enable) {
            /* Clear current 1588 configs if the port has 1588 enabled */
            PHYMOD_TIMESYNC_ENABLE_F_RX_SET(flags);
            PHYMOD_TIMESYNC_ENABLE_F_ONE_STEP_PIPELINE_SET(flags);
            _SOC_IF_ERR_EXIT(phymod_timesync_enable_set(&phy_access, flags, 0));
            flags = 0;
        } else {
            /* 1588 is not enabled on the port */
            if (ts_enable_port_count == 0) {
                /* This is the first 1588 port on the PM */
                PHYMOD_TIMESYNC_ENABLE_F_CORE_SET(flags);
            }
            ts_enable_port_count++;
        }

        one_step_req = (config->flags & SOC_PORT_PHY_TIMESYNC_ONE_STEP_ENABLE) ? 1 : 0;
        if (one_step_req) {
            PHYMOD_TIMESYNC_ENABLE_F_ONE_STEP_PIPELINE_SET(flags);
        }

        /*
         * SFD and LATEST Lanes are only supported in GEN2.
         */
        if (config->flags & SOC_PORT_PHY_TIMESYNC_SELECT_SFD) {
            PORTMOD_PORT_TIMESYNC_CONFIG_SFD_SET(timesync_config);
        } else {
            PORTMOD_PORT_TIMESYNC_CONFIG_SFD_CLR(timesync_config);
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

        /*
         * For forced speed 10G and 25G, TS_UPDATE_EN need to turn on first
         * before the link is up. Only for TH3.
         */
        _SOC_IF_ERR_EXIT
            (pm8x50_flexe_gen2_port_autoneg_get(unit, port, pm_info, phy_flags, &an_control));

        if (an_control.enable == 0) {
            /* first get the current port speed */
            _SOC_IF_ERR_EXIT(phymod_phy_speed_config_t_init(&speed_config));
            _SOC_IF_ERR_EXIT(phymod_phy_speed_config_get(&phy_access, &speed_config));

            if ((speed_config.data_rate == 10000) ||
                (speed_config.data_rate == 25000)) {
                PHYMOD_TIMESYNC_ENABLE_F_RX_PCS_ENABLE_SET(flags);
            }
        }
        /* Rx enable can only be done on link up as per programming requirement */
        _SOC_IF_ERR_EXIT(phymod_timesync_enable_set(&phy_access, flags, 1));

    }

    /* Update Timesync Enable Status in WB */
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_TS_ENABLE_PORT_COUNT_SET(unit, pm_info, ts_enable_port_count));
    if (enable) {
        PORTMOD_PORT_TIMESYNC_CONFIG_ENABLE_SET(timesync_config);
    } else {
        PORTMOD_PORT_TIMESYNC_CONFIG_ENABLE_CLR(timesync_config);
    }
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_TIMESYNC_CONFIG_SET(unit, pm_info, timesync_config, port_index));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_timesync_config_get(int unit, int port, pm_info_t pm_info,
                                            portmod_phy_timesync_config_t* config)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, timesync_config, timesync_enable = 0;
    int port_index;
    uint32 bitmap, is_one_step, flags = 0;
    SOC_INIT_FUNC_DEFS;

    sal_memset(config, 0,sizeof(portmod_phy_timesync_config_t));

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_TIMESYNC_CONFIG_GET(unit, pm_info, &timesync_config, port_index));
    PHYMOD_TIMESYNC_ENABLE_F_ONE_STEP_PIPELINE_SET(flags);
    _SOC_IF_ERR_EXIT(phymod_timesync_enable_get(&phy_access, flags, &is_one_step));
    timesync_enable = PORTMOD_PORT_TIMESYNC_CONFIG_ENABLE_GET(timesync_config);
    config->flags |= timesync_enable ? (is_one_step? SOC_PORT_PHY_TIMESYNC_ONE_STEP_ENABLE : SOC_PORT_PHY_TIMESYNC_TWO_STEP_ENABLE) : 0;
    config->flags |= PORTMOD_PORT_TIMESYNC_CONFIG_ENABLE_GET(timesync_config)?
                         SOC_PORT_PHY_TIMESYNC_ENABLE : 0;
    config->flags |= PORTMOD_PORT_TIMESYNC_CONFIG_SFD_GET(timesync_config)?
                         SOC_PORT_PHY_TIMESYNC_SELECT_SFD: 0;
    config->flags |= PORTMOD_PORT_TIMESYNC_CONFIG_COMP_MODE_ON_GET(timesync_config)?
                         SOC_PORT_PHY_TIMESYNC_COMP_MODE_ON: 0;
    config->flags |= PORTMOD_PORT_TIMESYNC_CONFIG_COMP_MODE_LATEST_LANE_GET(timesync_config) ?
                         SOC_PORT_PHY_TIMESYNC_COMP_MODE_LATEST_LANE: 0;

exit:
    SOC_FUNC_RETURN;
}

/*set/get interrupt enable value. */
int pm8x50_flexe_gen2_port_interrupt_enable_set(int unit, int port,
                                             pm_info_t pm_info,
                                             int intr_type, uint32 val)
{
    uint32 reg_val;
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_phy_access_get(unit, port, pm_info, &phy_acc));
    _SOC_IF_ERR_EXIT(READ_CDPORT_INTR_ENABLEr(unit, phy_acc, &reg_val));

    switch(intr_type) {
        case portmodIntrTypeLinkdown:
            soc_reg_field_set(unit, CDPORT_INTR_ENABLEr, &reg_val, LINK_DOWNf, val);
            break;
        case portmodIntrTypeMacErr:
            soc_reg_field_set(unit, CDPORT_INTR_ENABLEr, &reg_val, MAC_ERRf, val);
            break;
        case portmodIntrTypeTscCore0Err:
            soc_reg_field_set(unit, CDPORT_INTR_ENABLEr, &reg_val, TSC_CORE0_ERRf, val);
            break;
        case portmodIntrTypeTscCore1Err:
            soc_reg_field_set(unit, CDPORT_INTR_ENABLEr, &reg_val, TSC_CORE1_ERRf, val);
            break;
        case portmodIntrTypePmdErr:
            soc_reg_field_set(unit, CDPORT_INTR_ENABLEr, &reg_val, PMD_ERRf, val);
            break;
        case portmodIntrTypeFcReqFull:
            soc_reg_field_set(unit, CDPORT_INTR_ENABLEr, &reg_val, FLOWCONTROL_REQ_FULLf, val);
            break;
        case portmodIntrTypeTxPktUnderflow:
        case portmodIntrTypeTxPktOverflow:
        case portmodIntrTypeTxCdcSingleBitErr:
        case portmodIntrTypeTxCdcDoubleBitErr:
        case portmodIntrTypeLocalFaultStatus:
        case portmodIntrTypeRemoteFaultStatus:
        case portmodIntrTypeLinkInterruptionStatus:
        case portmodIntrTypeMibMemSingleBitErr:
        case portmodIntrTypeMibMemDoubleBitErr:
        case portmodIntrTypeMibMemMultipleBitErr:
            _SOC_IF_ERR_EXIT(cdmac_interrupt_enable_set(unit, port, intr_type, val));
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
            break;
    }
    _SOC_IF_ERR_EXIT(WRITE_CDPORT_INTR_ENABLEr(unit, phy_acc, reg_val));

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm8x50_flexe_gen2_phy_timesync_tx_info_get(int unit, int port, pm_info_t pm_info, portmod_fifo_status_t* tx_info)
{
    phymod_phy_access_t phy_access;
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_ts_fifo_status_t phy_ts_tx_info;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(phymod_ts_fifo_status_t_init(&phy_ts_tx_info));
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(phymod_timesync_tx_info_get(&phy_access, &phy_ts_tx_info));

    tx_info->timestamps_in_fifo = phy_ts_tx_info.ts_in_fifo_lo;
    tx_info->timestamps_in_fifo_hi = phy_ts_tx_info.ts_in_fifo_hi;
    tx_info->sequence_id = phy_ts_tx_info.ts_seq_id;
    tx_info->timestamp_sub_nanosec = phy_ts_tx_info.ts_sub_nanosec;

exit:
    SOC_FUNC_RETURN;
}

/*!
 * pm8x50_flexe_gen2_port_timesync_tx_info_get
 *
 * @brief get port timestamps in fifo
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [inout]  tx_info         - timestamp and seq id form fifo
 */

int pm8x50_flexe_gen2_port_timesync_tx_info_get(int unit, int port, pm_info_t pm_info, portmod_fifo_status_t* tx_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_phy_timesync_tx_info_get(unit, port, pm_info, tx_info));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_interrupt_enable_get(int unit, int port,
                                             pm_info_t pm_info,
                                             int intr_type, uint32* val)
{
    uint32 reg_val;
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_phy_access_get(unit, port, pm_info, &phy_acc));
    _SOC_IF_ERR_EXIT(READ_CDPORT_INTR_ENABLEr(unit, phy_acc, &reg_val));

    switch(intr_type) {
        case portmodIntrTypeLinkdown:
            *val = soc_reg_field_get(unit, CDPORT_INTR_ENABLEr, reg_val, LINK_DOWNf);
            break;
        case portmodIntrTypeMacErr:
            *val = soc_reg_field_get(unit, CDPORT_INTR_ENABLEr, reg_val, MAC_ERRf);
            break;
        case portmodIntrTypeTscCore0Err:
            *val = soc_reg_field_get(unit, CDPORT_INTR_ENABLEr, reg_val, TSC_CORE0_ERRf);
            break;
        case portmodIntrTypeTscCore1Err:
            *val = soc_reg_field_get(unit, CDPORT_INTR_ENABLEr, reg_val, TSC_CORE1_ERRf);
            break;
        case portmodIntrTypePmdErr:
            *val = soc_reg_field_get(unit, CDPORT_INTR_ENABLEr, reg_val, PMD_ERRf);
            break;
        case portmodIntrTypeFcReqFull:
            *val = soc_reg_field_get(unit, CDPORT_INTR_ENABLEr, reg_val, FLOWCONTROL_REQ_FULLf);
            break;
        case portmodIntrTypeTxPktUnderflow:
        case portmodIntrTypeTxPktOverflow:
        case portmodIntrTypeTxCdcSingleBitErr:
        case portmodIntrTypeTxCdcDoubleBitErr:
        case portmodIntrTypeLocalFaultStatus:
        case portmodIntrTypeRemoteFaultStatus:
        case portmodIntrTypeLinkInterruptionStatus:
        case portmodIntrTypeMibMemSingleBitErr:
        case portmodIntrTypeMibMemDoubleBitErr:
        case portmodIntrTypeMibMemMultipleBitErr:
            _SOC_IF_ERR_EXIT(cdmac_interrupt_enable_get(unit, port, intr_type, val));
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

/*get interrupt status value. */
int pm8x50_flexe_gen2_port_interrupt_get(int unit, int port, pm_info_t pm_info, int intr_type, uint32* val)
{
    uint64 reg_val;
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_phy_access_get(unit, port, pm_info, &phy_acc));
    _SOC_IF_ERR_EXIT(soc_reg_get(unit, CDPORT_INTR_STATUSr, phy_acc, 0, &reg_val));

    switch(intr_type) {
        case portmodIntrTypeLinkdown:
            *val = soc_reg_field_get(unit, CDPORT_INTR_STATUSr, COMPILER_64_LO(reg_val), LINK_DOWNf);
            break;
        case portmodIntrTypeMacErr:
            *val = soc_reg_field_get(unit, CDPORT_INTR_STATUSr, COMPILER_64_LO(reg_val), MAC_ERRf);
            break;
        case portmodIntrTypeTscCore0Err:
            *val = soc_reg_field_get(unit, CDPORT_INTR_STATUSr, COMPILER_64_LO(reg_val), TSC_CORE0_ERRf);
            break;
        case portmodIntrTypeTscCore1Err:
            *val = soc_reg_field_get(unit, CDPORT_INTR_STATUSr, COMPILER_64_LO(reg_val), TSC_CORE1_ERRf);
            break;
        case portmodIntrTypePmdErr:
            *val = soc_reg_field_get(unit, CDPORT_INTR_STATUSr, COMPILER_64_LO(reg_val), PMD_ERRf);
            break;
        case portmodIntrTypeFcReqFull:
            *val = soc_reg_field_get(unit, CDPORT_INTR_STATUSr, COMPILER_64_LO(reg_val), FLOWCONTROL_REQ_FULLf);
            break;
        case portmodIntrTypeTxPktUnderflow:
        case portmodIntrTypeTxPktOverflow:
        case portmodIntrTypeTxCdcSingleBitErr:
        case portmodIntrTypeTxCdcDoubleBitErr:
        case portmodIntrTypeLocalFaultStatus:
        case portmodIntrTypeRemoteFaultStatus:
        case portmodIntrTypeLinkInterruptionStatus:
        case portmodIntrTypeMibMemSingleBitErr:
        case portmodIntrTypeMibMemDoubleBitErr:
        case portmodIntrTypeMibMemMultipleBitErr:
            _SOC_IF_ERR_EXIT(cdmac_interrupt_status_get(unit, port, intr_type, val));
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

/*get interrupt value array. */
int pm8x50_flexe_gen2_port_interrupts_get(int unit, int port, pm_info_t pm_info,
                                       int arr_max_size, uint32* intr_arr,
                                       uint32* size)
{
    uint64 reg_val;
    uint32 cnt=0;
    int phy_acc;
    uint32 mac_intr_cnt = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_phy_access_get(unit, port, pm_info, &phy_acc));
    _SOC_IF_ERR_EXIT(soc_reg_get(unit, CDPORT_INTR_STATUSr, phy_acc, 0, &reg_val));

    if (soc_reg_field_get(unit, CDPORT_INTR_STATUSr, COMPILER_64_LO(reg_val), LINK_DOWNf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeLinkdown;
    }

    if (soc_reg_field_get(unit, CDPORT_INTR_STATUSr, COMPILER_64_LO(reg_val), MAC_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeMacErr;
    }

    if (soc_reg_field_get(unit, CDPORT_INTR_STATUSr, COMPILER_64_LO(reg_val), TSC_CORE0_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTscCore0Err;
    }

    if (soc_reg_field_get(unit, CDPORT_INTR_STATUSr, COMPILER_64_LO(reg_val), TSC_CORE1_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTscCore1Err;
    }

    if (soc_reg_field_get(unit, CDPORT_INTR_STATUSr, COMPILER_64_LO(reg_val), PMD_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypePmdErr;
    }

    if (soc_reg_field_get(unit, CDPORT_INTR_STATUSr, COMPILER_64_LO(reg_val), FLOWCONTROL_REQ_FULLf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeFcReqFull;
    }

    _SOC_IF_ERR_EXIT(cdmac_interrupts_status_get(unit, port, (arr_max_size - cnt), (intr_arr + cnt), &mac_intr_cnt));
    cnt += mac_intr_cnt;
    *size = cnt;

exit:
    SOC_FUNC_RETURN;
}

/* portmod port rsv mask set*/
int pm8x50_flexe_gen2_port_mac_rsv_mask_set(int unit, int port,
                                         pm_info_t pm_info,
                                         uint32 rsv_mask)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_rsv_mask_set(unit, port, rsv_mask));

exit:
    SOC_FUNC_RETURN;
}

/*Portmod state for any logical port dynamixc settings*/
int pm8x50_flexe_gen2_port_update_dynamic_state(int unit, int port,
                                             pm_info_t pm_info,
                                             uint32_t port_dynamic_state)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    uint32_t link_fail_wait_time;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    if (PORTMOD_PORT_IS_AN_LINK_FAIL_INHIBIT_TIMER_UPDATED(port_dynamic_state)) {
        /*Store the an link fail inhibit timer setting for 400G an enable /disable*/
        _SOC_IF_ERR_EXIT(phymod_phy_an_timer_get(&phy_access, phymodAnTimerLinkFailInhibitLtPam4, &link_fail_wait_time));
        _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_AN_LINK_FAIL_INHIBIT_TIMER_LT_PAM4_SET(unit, pm_info, link_fail_wait_time));
    }

exit:
    SOC_FUNC_RETURN;
}

/*Returns if the PortMacro associated with the port is initialized or not*/
int pm8x50_flexe_gen2_pm_is_initialized(int unit, int pm_id, pm_info_t pm_info, int* is_initialized)
{
    int is_core_initialized = 0, rv;
     rv = PM8x50_FLEXE_GEN2_IS_CORE_INITIALIZED_GET(unit, pm_info, &is_core_initialized);
     *is_initialized = is_core_initialized;

    return rv;
}

/* get the logical port bitmap of the current PM */
int pm8x50_flexe_gen2_pm_logical_pbmp_get(int unit, int pm_id, pm_info_t pm_info,
                                       portmod_pbmp_t* logical_pbmp)
{
    int i, port;
    SOC_INIT_FUNC_DEFS;

    PORTMOD_PBMP_CLEAR(*logical_pbmp);
    for (i = 0 ; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
        _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_LANE2PORT_GET(unit, pm_info, i, &port));
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

/*
 *Function:
 *  _pm8x50_flexe_gen2_txpi_sdm_scheme_set
 *
 *Purpose:
 *  Set Portmacro TXPI DSM scheme type
 *  Default type is portmodTxPiSdmType1StOderWithFloor
 *  This is a core level API
 *  SDM scheme can't be changed if the port is active
*/
int _pm8x50_flexe_gen2_txpi_sdm_scheme_set(int unit, pm_info_t pm_info, portmod_txpi_sdm_type_t type)
{
    int phy_acc;
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    /* Get the first physical port of the pm core */
    phy_acc = PM_8x50_FLEXE_GEN2_INFO(pm_info)->first_phy | SOC_REG_ADDR_PHY_ACC_MASK;

    _SOC_IF_ERR_EXIT(READ_CDPORT_TXPI_CSR_SEL_CTRLr(unit, phy_acc, &reg_val));
    soc_reg_field_set(unit, CDPORT_TXPI_CSR_SEL_CTRLr, &reg_val, TXPI_CSR_CTRL_SDM_TYPEf, type);
    _SOC_IF_ERR_EXIT(WRITE_CDPORT_TXPI_CSR_SEL_CTRLr(unit, phy_acc, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_core_add(int unit, int pm_id, pm_info_t pm_info, int flags, const portmod_port_add_info_t* add_info)
{
    int is_initialized = 0;
    int pm_enable_flags = 0;
    int force_mac_init = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_pm_is_initialized(unit, pm_id, pm_info, &is_initialized));

    if (PORTMOD_CORE_ADD_SKIP_MAC_GET(flags)) {
        PORTMOD_PM_ENABLE_SKIP_MAC_SET(pm_enable_flags);
    }
    force_mac_init = PORTMOD_CORE_ADD_FORCE_MAC_GET(flags);
    if (is_initialized && force_mac_init) {
        PORTMOD_PM_ENABLE_MAC_ONLY_SET(pm_enable_flags);
    }
    if (!is_initialized || force_mac_init) {
        /* Set txpi sdm scheme */
        _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_txpi_sdm_scheme_set(unit, pm_info, add_info->init_config.txpi_sdm_scheme));
        _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_pm_enable(unit, pm_id, pm_info, pm_enable_flags, 1));
    }

    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_pm_serdes_core_init(unit, pm_id, pm_info, add_info));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_lane_map_set(int unit, int port, pm_info_t pm_info, uint32 flags, const phymod_lane_map_t* lane_map)
{

    return (SOC_E_UNAVAIL);
}

int pm8x50_flexe_gen2_port_lane_map_get(int unit, int port, pm_info_t pm_info, uint32 flags, phymod_lane_map_t* lane_map)
{
    SOC_INIT_FUNC_DEFS;

    *lane_map = PM_8x50_FLEXE_GEN2_INFO(pm_info)->lane_map;

    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_synce_clk_ctrl_set(int unit, int port, pm_info_t pm_info,
                                           const portmod_port_synce_clk_ctrl_t* cfg)
{
    phymod_synce_clk_ctrl_t phy_synce_cfg;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    int config_valid = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
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

int pm8x50_flexe_gen2_port_synce_clk_ctrl_get(int unit, int port, pm_info_t pm_info,
                                           portmod_port_synce_clk_ctrl_t* cfg)
{
    phymod_synce_clk_ctrl_t phy_synce_cfg;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
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

STATIC
int _pm8x50_flexe_gen2_pm_tsc_interrupt_type_get(int unit, int intr_val, uint32 flags,
                                              pm8x50_flexe_gen2_tsc_err_intr_phymod_map_t *tsc_intr_info)
{
    pm8x50_flexe_gen2_tsc_intr_type_t iter = pm8x50_flexe_gen2TscIntrEccNone;
    pm8x50_flexe_gen2_tsc_intr_type_t iter_start = pm8x50_flexe_gen2TscIntrEccNone;
    pm8x50_flexe_gen2_tsc_intr_type_t iter_end = pm8x50_flexe_gen2TscIntrEccNone;

    /* Check is 1-bit or 2-bit ECC interrupt type */
    if (flags & PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR) {
        iter_start = pm8x50_flexe_gen2TscIntrEcc1bErr;
        iter_end = pm8x50_flexe_gen2TscIntrEcc1bErrCount;
    } else if (flags & PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR) {
        iter_start = pm8x50_flexe_gen2TscIntrEcc2bErr;
        iter_end = pm8x50_flexe_gen2TscIntrEcc2bErrCount;
    } else {
        return SOC_E_INTERNAL;
    }

    for(iter = iter_start; iter <= iter_end; iter++) {
        if (intr_val & pm8x50_flexe_gen2_tsc_ecc_intr_info[iter].flags) {
            /* return phymod tsc interrupt type */
            sal_memcpy(tsc_intr_info, &pm8x50_flexe_gen2_tsc_ecc_intr_info[iter],
                        sizeof(pm8x50_flexe_gen2_tsc_err_intr_phymod_map_t));
            break;
        }
    }

    return SOC_E_NONE;
}

int pm8x50_flexe_gen2_pm_interrupt_process(int unit, int pm_id, pm_info_t pm_info,
                                        portmod_ecc_intr_info_t *ecc_info)
{
    int i = 0, ii = 0, j = 0;
    int lport = 0;
    int prev_lport = 0;
    int first_lport = 0;
    uint32 val = 0, rval = 0, fval = 0;
    uint32 tsc_intr_flags = 0x0, is_handled = 0;
    uint64 rval64;
    pm8x50_flexe_gen2_tsc_err_intr_phymod_map_t tsc_intr_info;
    portmod_pm_intr_info_t *p_intr_info = &pm8x50_flexe_gen2_pm_intr_info[0];
    portmod_intr_reg_info_t *p_intr_grp_info = NULL;
    portmod_intr_type_t intr_type;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys = 0;
    int is_intr_found = 0;
    int mem_index = -1;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(rval64);
    sal_memset(&tsc_intr_info,0,sizeof(tsc_intr_info));

    /* Get the first physical port of the pm core */
    for(ii=0; ii < PM8X50_FLEXE_GEN2_LANES_PER_CORE; ii++) {
        /* get the logical port associated with the physical port */
        _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_LANE2PORT_GET(unit, pm_info, ii, &lport));

        /* skip if invalid or its the same logical port */
        if ((lport == -1) || (prev_lport == lport)) {
            /*LOG_CLI(("Port(%s) - Skipping interrupt processing for same logical port!\n",
                     SOC_PORT_NAME(unit, lport)));*/
            continue;
        }

        /* Remember the first port */
        if (first_lport == 0) {
            first_lport = lport;
        }
        prev_lport = lport;

        /*
         * Following is the sequence for processing the interrupts.
         * 1. Check the top level interrupt status register of the
         *    PM8x50 to determine if its a MAC interrupt or a
         *    TSC interrupti,
         * 2. If its a MAC interrupt, MAC interrupt status register
         *    is read, to determine the MAC interrupt type, This is
         *    is a per-port register and the function returns on the
         *    determining the first interrupt.
         * 3. If its a TSC interrupt, the TSC interrupt status
         *    register is read to determine the TSC inetrrupt type.
         */

        /*
         * Read the top level interrupt status only once, since its
         * only a single copy register.
         */
        if (first_lport == prev_lport) {
            for (i = 0; i < sizeof(pm8x50_flexe_gen2_pm_intr_info)/
                            sizeof(portmod_pm_intr_info_t); i++) {
                p_intr_info = &pm8x50_flexe_gen2_pm_intr_info[i];
                intr_type = p_intr_info->intr_type;
                _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_interrupt_get(unit, lport,
                                            pm_info, intr_type, &val));

                /* continue if interrupt is not set. */
                if (!val) {
                    continue;
                }

                /* Either MAC, TSC, PMD Interrupt is set, */
                /*
                 * Iterate through the individual interrupt status
                 * based on the list of status registers.
                 */
                if (p_intr_info->is_intr_status_group) {
                    p_intr_grp_info = &p_intr_info->intr_list_status_reg[0];
                    intr_type = p_intr_grp_info->intr_type;
                    is_intr_found = 1;
                }
                /*LOG_CLI(("%s\n", p_intr_info->mem_str));*/
                break;
            }

            if (!is_intr_found) {
                _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                  (_SOC_MSG("Port(%s) - Interrupt not found!\n"),
                            SOC_PORT_NAME(unit, lport)));
            }

            /*
             * Below logic determines the specific interrupt type
             * in MAC, TSC Core0, TSC Core1,
             */
            while (intr_type != portmodIntrTypeCount) {
                if (SOC_REG_IS_64(unit, p_intr_grp_info->st_reg)) {
                    uint64 fval64;

                    _SOC_IF_ERR_EXIT(soc_reg64_get(unit,
                                                   p_intr_grp_info->st_reg,
                                                   lport, 0, &rval64));
                    fval64 = soc_reg64_field_get(unit, p_intr_grp_info->st_reg,
                                                 rval64, p_intr_grp_info->st_fld);
                    COMPILER_64_TO_32_LO(fval, fval64);
                } else {
                    _SOC_IF_ERR_EXIT(soc_reg32_get(unit,
                                                   p_intr_grp_info->st_reg,
                                                   lport, 0, &rval));
                    fval = soc_reg_field_get(unit, p_intr_grp_info->st_reg,
                                             rval, p_intr_grp_info->st_fld);
                }

                /* If not set skip to the next interrupt type. */
                if (!fval) {
                    j++;
                    p_intr_grp_info = &p_intr_info->intr_list_status_reg[j];
                    intr_type = p_intr_info->intr_list_status_reg[j].intr_type;
                    continue;
                }

                LOG_VERBOSE(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Port - %s "
                           "Reg - %s, Field - %s fval - %d\n"),
                           SOC_PORT_NAME(unit, lport),
                           SOC_REG_NAME(unit,  p_intr_grp_info->st_reg),
                           SOC_FIELD_NAME(unit, p_intr_grp_info->st_fld), fval));

                /* If interrupt status is set, and status is clear-on-read */
                if (p_intr_grp_info->flags & PORTMOD_INTR_STATUS_COR) {

                    /* If its per-port clear status register */
                    if (!(p_intr_grp_info->flags &
                            PORTMOD_INTR_BLOCK_CLEAR_REG)) {
                        _SOC_IF_ERR_EXIT(soc_reg32_get(unit,
                                               p_intr_grp_info->clr_reg,
                                               lport, 0, &rval));
                LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit, "NOT BLOCK_CLEAR_REG\n")));
                    } else {
                        /* If its per-port clear status register */
                        uint32 raddr;
                        int phy_port, block;
                        uint8 at;

                        PORTMOD_PBMP_ITER(PM_8x50_FLEXE_GEN2_INFO(pm_info)->phys,
                                          phy_port) {
                            break;
                        }

                        raddr = soc_reg_addr_get(unit,
                                     p_intr_grp_info->clr_reg, phy_port, 0,
                                     SOC_REG_ADDR_OPTION_PRESERVE_PORT,
                                     &block, &at);
                        _SOC_IF_ERR_EXIT(_soc_reg32_get(unit, block, at,
                                                        raddr, &rval));
                LOG_CLI(("BLOCK_CLEAR_REG\n"));
                    }

                    fval = soc_reg_field_get(unit,
                                     p_intr_grp_info->clr_reg,
                                     rval, p_intr_grp_info->clr_fld);
                } else if (((intr_type == portmodIntrTypeTscCore0Err) ||
                           (intr_type == portmodIntrTypeTscCore1Err)) &&
                           (p_intr_grp_info->flags &
                            PORTMOD_INTR_STATUS_W1TC)) {

                    if (p_intr_grp_info->st_fld == TSC_ECC_1B_ERRf) {
                        tsc_intr_flags = PM8x50_FLEXE_GEN2_TSC_ECC_1B_INTR;
                    } else if (p_intr_grp_info->st_fld == TSC_ECC_2B_ERRf) {
                        tsc_intr_flags = PM8x50_FLEXE_GEN2_TSC_ECC_2B_INTR;
                    }

                    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_pm_tsc_interrupt_type_get(unit,
                                                fval, tsc_intr_flags,
                                                &tsc_intr_info));
                    /* Only internal phy supported. */
                    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit,
                                                                     &params));
                    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit,
                                                lport, pm_info, &params, 1,
                                                &phy_access, &nof_phys, NULL));

                    LOG_VERBOSE(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit, "TSC Core Error - %d\n"),
                                   tsc_intr_info.phymod_intr_map));
                    /* clear tsc interrupt */
                    _SOC_IF_ERR_EXIT(phymod_intr_handler(&phy_access,
                                             tsc_intr_info.phymod_intr_map,
                                             &is_handled,
                                             &mem_index));
                    if (!is_handled) {
                        /*LOG_CLI(("Cannot handle - %s\n", tsc_intr_info.str));*/
                    } else {
                        /*LOG_CLI(("Phymod interrupt handled - %d(%s)\n",
                                          is_handled, tsc_intr_info.str));*/
                    }
                    ecc_info->mem_str = tsc_intr_info.str;
                }

                ecc_info->port = lport;
                ecc_info->ecc_num_bits_err =
                       (p_intr_grp_info->flags & PORTMOD_INTR_ECC_1B_ERR) ?
                        PORTMOD_INTR_ECC_1B_ERR:
                       (p_intr_grp_info->flags & PORTMOD_INTR_ECC_2B_ERR) ?
                        PORTMOD_INTR_ECC_2B_ERR:
                       (p_intr_grp_info->flags & PORTMOD_INTR_ECC_MULTIB_ERR) ?
                        PORTMOD_INTR_ECC_MULTIB_ERR: PORTMOD_PM_ECC_ERR_NONE;
                /* Deskew memroy ECC workaround:
                 * For fake deskew memroy ECC error, phymod handler will return
                 * is_handled as 1. So Portmod does not need to report this ECC.
                 */
                if ((tsc_intr_info.phymod_intr_map == phymodIntrEccDeskew)
                     && is_handled) {
                    ecc_info->ecc_num_bits_err = PORTMOD_PM_ECC_ERR_NONE;
                }

                SOC_EXIT;
            } /* intr_type != portmodIntrTypeCount */
        } /* Interate through the interrupt types */
    } /* Iterate through logical ports */

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_pm_core_num_get(int unit, pm_info_t pm_info, int* core_num)
{
    *core_num = PM_8x50_FLEXE_GEN2_INFO(pm_info)->core_num;
    return (SOC_E_NONE);
}

int pm8x50_flexe_gen2_port_mac_enable_set(int unit,int port, pm_info_t pm_info,
                                       int enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_enable_set(unit, port, 0, enable));
exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_pmd_lock_counter_get(int unit, int port, pm_info_t pm_info, uint32* pmd_lock_counter_sum)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    /* get phy access */
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                    &params, 1, &phy_access, &nof_phys, NULL));
    /* get pmd_lock counter */
    _SOC_IF_ERR_EXIT(phymod_phy_rx_pmd_lock_counter_get(&phy_access, pmd_lock_counter_sum));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_egr_1588_timestamp_config_set(int unit, int port, pm_info_t pm_info,
                                  portmod_egr_1588_timestamp_config_t timestamp_config)
{
    /* PM8x50 only support 48bit mode. */
    if (timestamp_config.timestamp_mode == portmodTimestampMode48bit) {
        return SOC_E_NONE;
    } else {
        return SOC_E_UNAVAIL;
    }
}

int pm8x50_flexe_gen2_egr_1588_timestamp_config_get(int unit, int port, pm_info_t pm_info,
                                 portmod_egr_1588_timestamp_config_t* timestamp_config)
{
    /* PM8x50 only support 48bit mode. */
    timestamp_config->timestamp_mode = portmodTimestampMode48bit;

    return SOC_E_NONE;
}

/* This function collects the reasons for local_faults that might have
 * happened before it's invocation. Returns a bit-map with all those
 * reasons
 */
int pm8x50_flexe_gen2_port_local_fault_reasons_get(int unit, int port, pm_info_t pm_info,
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
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                &params, 1, &phy_access, &nof_phys, NULL));


    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &port_index, bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_FLEXE_GEN2_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));

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
        (pm8x50_flexe_gen2_port_speed_config_get(unit, port, pm_info, &speed_config));

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

int pm8x50_flexe_gen2_port_fec_error_inject_set(int unit, int port, pm_info_t pm_info,
                                             uint16 error_control_map,
                                             portmod_fec_error_mask_t bit_error_mask)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT
        (phymod_phy_fec_error_inject_set(&phy_access, error_control_map, bit_error_mask));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_fec_error_inject_get(int unit, int port, pm_info_t pm_info,
                                              uint16* error_control_map,
                                              portmod_fec_error_mask_t* bit_error_mask)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT
        (phymod_phy_fec_error_inject_get(&phy_access, error_control_map, bit_error_mask));

exit:
    SOC_FUNC_RETURN;
}

/*local and remote fault status get*/
int pm8x50_flexe_gen2_port_faults_status_get(int unit, int port, pm_info_t pm_info,
                                          portmod_port_fault_status_t* faults)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_faults_status_get(unit, port, faults));

exit:
    SOC_FUNC_RETURN;
}

/*
 *Function:
 *  pm8x50_flexe_gen2_port_txpi_override_set
 *
 *Purpose:
 *  Overwrite Portmacro TXPI SDM value
 *  Defaults to using value from CMIC
 *  This is a port level API
*/

int pm8x50_flexe_gen2_port_txpi_override_set (int unit, int port, pm_info_t pm_info, const portmod_txpi_override_t *override)
{
    int phy_acc;
    uint32 reg_val_en, bitmap;
    int i, first_index;
    int64 reg_value;

    soc_field_t field[PM8X50_FLEXE_GEN2_PORTS_PER_CORE] = {TXPI_CSR_OVRD_EN_0f, TXPI_CSR_OVRD_EN_1f, TXPI_CSR_OVRD_EN_2f, TXPI_CSR_OVRD_EN_3f,
                                               TXPI_CSR_OVRD_EN_4f, TXPI_CSR_OVRD_EN_5f, TXPI_CSR_OVRD_EN_6f, TXPI_CSR_OVRD_EN_7f};
    soc_reg_t over_reg_low[PM8X50_FLEXE_GEN2_PORTS_PER_CORE] = {CDPORT_TXPI_CSR_OVRD_VAL_0_LOWr, CDPORT_TXPI_CSR_OVRD_VAL_1_LOWr,
                                                    CDPORT_TXPI_CSR_OVRD_VAL_2_LOWr, CDPORT_TXPI_CSR_OVRD_VAL_3_LOWr,
                                                    CDPORT_TXPI_CSR_OVRD_VAL_4_LOWr, CDPORT_TXPI_CSR_OVRD_VAL_5_LOWr,
                                                    CDPORT_TXPI_CSR_OVRD_VAL_6_LOWr, CDPORT_TXPI_CSR_OVRD_VAL_7_LOWr};
    soc_reg_t over_reg_high[PM8X50_FLEXE_GEN2_PORTS_PER_CORE] = {CDPORT_TXPI_CSR_OVRD_VAL_0_HIGHr, CDPORT_TXPI_CSR_OVRD_VAL_1_HIGHr,
                                                     CDPORT_TXPI_CSR_OVRD_VAL_2_HIGHr, CDPORT_TXPI_CSR_OVRD_VAL_3_HIGHr,
                                                     CDPORT_TXPI_CSR_OVRD_VAL_4_HIGHr, CDPORT_TXPI_CSR_OVRD_VAL_5_HIGHr,
                                                     CDPORT_TXPI_CSR_OVRD_VAL_6_HIGHr, CDPORT_TXPI_CSR_OVRD_VAL_7_HIGHr};
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_phy_access_get(unit, port, pm_info, &phy_acc));
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &first_index, &bitmap));

    /* Enable TXPI DSM override */
    _SOC_IF_ERR_EXIT(READ_CDPORT_TXPI_CSR_OVRD_ENr(unit, phy_acc, &reg_val_en));
    for (i = 0; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
        if (bitmap & (0x1 << i)) {
            soc_reg_field_set(unit, CDPORT_TXPI_CSR_OVRD_ENr, &reg_val_en, field[i], override->enable ? TRUE : FALSE);
        }
    }
    _SOC_IF_ERR_EXIT(WRITE_CDPORT_TXPI_CSR_OVRD_ENr(unit, phy_acc, reg_val_en));

    /* Write TXPI DSM override value. Low 32 bits must be wroten before high register
     * reg_value = txpi_dsm_value_1ppm * override->value;
    */
    COMPILER_64_SET(reg_value, TXPI_DSM_VALUE_1PPM_HI, TXPI_DSM_VALUE_1PPM_LO);
    COMPILER_64_UMUL_32(reg_value, override->value);

    for (i = 0; i < PM8X50_FLEXE_GEN2_PORTS_PER_CORE; i++) {
        if (bitmap & (0x1 << i)) {
            soc_reg32_set(unit, over_reg_low[i], phy_acc, 0, override->enable ? COMPILER_64_LO(reg_value) : 0);
            soc_reg32_set(unit, over_reg_high[i], phy_acc, 0, override->enable ? (COMPILER_64_HI(reg_value) & 0xffff) : 0);
        }
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 *Function:
 *  pm8x50_flexe_gen2_port_txpi_override_get
 *
 *Purpose:
 *  Get Portmacro TXPI SDM override status and value
 *  Defaults to using value from CMIC
 *  This is a port level API
*/
int pm8x50_flexe_gen2_port_txpi_override_get (int unit, int port, pm_info_t pm_info, portmod_txpi_override_t *override)
{
    int phy_acc;
    uint32 reg_val_en, bitmap;
    int first_index;
    uint32 reg_value_lo, reg_value_hi;
    int64 tmp_reg_val64, txpi_dsm_value_1ppm;

    soc_field_t field[PM8X50_FLEXE_GEN2_PORTS_PER_CORE] = {TXPI_CSR_OVRD_EN_0f, TXPI_CSR_OVRD_EN_1f, TXPI_CSR_OVRD_EN_2f, TXPI_CSR_OVRD_EN_3f,
                                               TXPI_CSR_OVRD_EN_4f, TXPI_CSR_OVRD_EN_5f, TXPI_CSR_OVRD_EN_6f, TXPI_CSR_OVRD_EN_7f};
    soc_reg_t over_reg_low[PM8X50_FLEXE_GEN2_PORTS_PER_CORE] = {CDPORT_TXPI_CSR_OVRD_VAL_0_LOWr, CDPORT_TXPI_CSR_OVRD_VAL_1_LOWr,
                                                    CDPORT_TXPI_CSR_OVRD_VAL_2_LOWr, CDPORT_TXPI_CSR_OVRD_VAL_3_LOWr,
                                                    CDPORT_TXPI_CSR_OVRD_VAL_4_LOWr, CDPORT_TXPI_CSR_OVRD_VAL_5_LOWr,
                                                    CDPORT_TXPI_CSR_OVRD_VAL_6_LOWr, CDPORT_TXPI_CSR_OVRD_VAL_7_LOWr};
    soc_reg_t over_reg_high[PM8X50_FLEXE_GEN2_PORTS_PER_CORE] = {CDPORT_TXPI_CSR_OVRD_VAL_0_HIGHr, CDPORT_TXPI_CSR_OVRD_VAL_1_HIGHr,
                                                     CDPORT_TXPI_CSR_OVRD_VAL_2_HIGHr, CDPORT_TXPI_CSR_OVRD_VAL_3_HIGHr,
                                                     CDPORT_TXPI_CSR_OVRD_VAL_4_HIGHr, CDPORT_TXPI_CSR_OVRD_VAL_5_HIGHr,
                                                     CDPORT_TXPI_CSR_OVRD_VAL_6_HIGHr, CDPORT_TXPI_CSR_OVRD_VAL_7_HIGHr};
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_phy_access_get(unit, port, pm_info, &phy_acc));
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &first_index, &bitmap));

    /*Get enable status of TXPI DSM override */
    _SOC_IF_ERR_EXIT(READ_CDPORT_TXPI_CSR_OVRD_ENr(unit, phy_acc, &reg_val_en));
    override->enable = soc_reg_field_get(unit, CDPORT_TXPI_CSR_OVRD_ENr, reg_val_en, field[first_index]);

    /* Get TXPI DSM override value */
    if (override->enable == TRUE) {
        /*  The LOW register should be read first before reading from the HIGH register. */
        _SOC_IF_ERR_EXIT(soc_reg32_get(unit, over_reg_low[first_index],
                                       phy_acc, 0, &reg_value_lo));
        _SOC_IF_ERR_EXIT(soc_reg32_get(unit, over_reg_high[first_index],
                         phy_acc, 0, &reg_value_hi));
        /*
         * bit47 : 0 -- positive; 1 -- negative.
         * If it is negative, the high 16 bits must be filled with 1.
         */
        reg_value_hi = (reg_value_hi & 0x8000) ? (reg_value_hi | 0xffff0000) : (reg_value_hi & 0xffff);
        /*
         *Use COMPILER_64_XXX macros to put together 64 bit variables txpi_dsm_value_1ppm and tmp_reg_val64
         *and then generate the override->value
         */
        COMPILER_64_SET(txpi_dsm_value_1ppm, TXPI_DSM_VALUE_1PPM_HI, TXPI_DSM_VALUE_1PPM_LO);
        COMPILER_64_SET(tmp_reg_val64, reg_value_hi, reg_value_lo);
        COMPILER_64_UDIV_64(tmp_reg_val64, txpi_dsm_value_1ppm);

        COMPILER_64_TO_32_LO(override->value, tmp_reg_val64);
    } else {
        override->value = 0;
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 *Function:
 *  pm8x50_flexe_gen2_port_txpi_sdm_data_get
 *
 *Purpose:
 *  Get Portmacro TXPI SDM value of resolution
 *  This is a port level API
*/
int pm8x50_flexe_gen2_port_txpi_sdm_data_get(int unit, int port, pm_info_t pm_info, int *value)
{
    int phy_acc;
    uint32 bitmap;
    int first_index;
    uint32 reg_value_lo, reg_value_hi;
    int64 tmp_reg_val64, txpi_dsm_value_1ppm;

    soc_reg_t data_reg_low[PM8X50_FLEXE_GEN2_PORTS_PER_CORE] = {CDPORT_TXPI_DATA_OUT_STS_0_LOWr, CDPORT_TXPI_DATA_OUT_STS_1_LOWr,
                                                    CDPORT_TXPI_DATA_OUT_STS_2_LOWr, CDPORT_TXPI_DATA_OUT_STS_3_LOWr,
                                                    CDPORT_TXPI_DATA_OUT_STS_4_LOWr, CDPORT_TXPI_DATA_OUT_STS_5_LOWr,
                                                    CDPORT_TXPI_DATA_OUT_STS_6_LOWr, CDPORT_TXPI_DATA_OUT_STS_7_LOWr};
    soc_reg_t data_reg_high[PM8X50_FLEXE_GEN2_PORTS_PER_CORE] = {CDPORT_TXPI_DATA_OUT_STS_0_HIGHr, CDPORT_TXPI_DATA_OUT_STS_1_HIGHr,
                                                     CDPORT_TXPI_DATA_OUT_STS_2_HIGHr, CDPORT_TXPI_DATA_OUT_STS_3_HIGHr,
                                                     CDPORT_TXPI_DATA_OUT_STS_4_HIGHr, CDPORT_TXPI_DATA_OUT_STS_5_HIGHr,
                                                     CDPORT_TXPI_DATA_OUT_STS_6_HIGHr, CDPORT_TXPI_DATA_OUT_STS_7_HIGHr};
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_phy_access_get(unit, port, pm_info, &phy_acc));
    _SOC_IF_ERR_EXIT(_pm8x50_flexe_gen2_port_index_get(unit, port, pm_info, &first_index, &bitmap));

    /* get the first sub-port if the port is dual-lane or quad-lane mode.
     * The LOW register should be read first before reading from the HIGH register.
     */
    _SOC_IF_ERR_EXIT(soc_reg32_get(unit, data_reg_low[first_index],
                                   phy_acc, 0, &reg_value_lo));
    _SOC_IF_ERR_EXIT(soc_reg32_get(unit, data_reg_high[first_index],
                                   phy_acc, 0, &reg_value_hi));
    /*
     * bit47 : 0 -- positive; 1 -- negative.
     * If it is negative, the high 16 bits must be filled with 1.
     */
    reg_value_hi = (reg_value_hi & 0x8000) ? (reg_value_hi | 0xffff0000) : (reg_value_hi & 0xffff);

    /*
     *Use COMPILER_64_XXX macros to put together 64 bit variables txpi_dsm_value_1ppm and tmp_reg_val64
     *and then generate the override->value
     */
    COMPILER_64_SET(txpi_dsm_value_1ppm, TXPI_DSM_VALUE_1PPM_HI, TXPI_DSM_VALUE_1PPM_LO);
    COMPILER_64_SET(tmp_reg_val64, reg_value_hi, reg_value_lo);
    COMPILER_64_UDIV_64(tmp_reg_val64, txpi_dsm_value_1ppm);

    COMPILER_64_TO_32_LO(*value, tmp_reg_val64);
exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_txpi_rxcdr_clk_get(int unit,
                                       int port,
                                       pm_info_t pm_info,
                                       uint32* txpi_clk,
                                       uint32* rxcdr_clk)
{
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    uint32_t pll_multiplier, temp_clk_khz;
    phymod_osr_mode_t osr_mode;
    phymod_ref_clk_t ref_clk;
    uint32_t temp_cdr_clk;
    uint64_t temp_cdr_clk_64;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(phymod_phy_pll_multiplier_get(&phy_access, &pll_multiplier));

    /*next get the current port PLL divider */
    _SOC_IF_ERR_EXIT(phymod_phy_osr_mode_get(&phy_access, &osr_mode));
    /* get the rf clock */
    _SOC_IF_ERR_EXIT(phymod_phy_ref_clk_get(&phy_access, &ref_clk));
    /*first calculate rx cdr clock, which is VCO/40 */
    switch (ref_clk)  {
    case phymodRefClk156Mhz:
        temp_clk_khz = 156250;
        break;
    case phymodRefClk125Mhz:
        temp_clk_khz = 125000;
        break;
    default:
        temp_clk_khz = 156250;
        break;
    }

    /*first calculate rx cdr clock, which is VCO/(40 * osr)  expect 1G.
     * 1G should be VCO/40.
     */
    *rxcdr_clk = (temp_clk_khz * 100 * pll_multiplier) / 40;

    /*next convert it to Hz */
    *rxcdr_clk *= 10;

    switch (osr_mode) {
        case phymodOversampleMode1:
            temp_cdr_clk = *rxcdr_clk;
            break;
        case phymodOversampleMode2:
            temp_cdr_clk = *rxcdr_clk / 2;
            break;
        case phymodOversampleMode3:
            temp_cdr_clk = *rxcdr_clk / 3;
            break;
        case phymodOversampleMode3P3:
            COMPILER_64_SET(temp_cdr_clk_64, 0, *rxcdr_clk);
            COMPILER_64_UMUL_32(temp_cdr_clk_64, 10);
            COMPILER_64_UDIV_32(temp_cdr_clk_64, 3 * 10 + 3);
            temp_cdr_clk = COMPILER_64_LO(temp_cdr_clk_64);
            break;
        case phymodOversampleMode4:
            temp_cdr_clk = *rxcdr_clk / 4;
            break;
        case phymodOversampleMode5:
            temp_cdr_clk = *rxcdr_clk / 5;
            break;
        case phymodOversampleMode7P5:
            COMPILER_64_SET(temp_cdr_clk_64, 0, *rxcdr_clk);
            COMPILER_64_UMUL_32(temp_cdr_clk_64, 10);
            COMPILER_64_UDIV_32(temp_cdr_clk_64, 7 * 10 + 5);
            temp_cdr_clk = COMPILER_64_LO(temp_cdr_clk_64);
            break;
        case phymodOversampleMode8:
            temp_cdr_clk = *rxcdr_clk / 8;
            break;
        case phymodOversampleMode8P25:
            COMPILER_64_SET(temp_cdr_clk_64, 0, *rxcdr_clk);
            COMPILER_64_UMUL_32(temp_cdr_clk_64, 100);
            COMPILER_64_UDIV_32(temp_cdr_clk_64, 8 * 100 + 25);
            temp_cdr_clk = COMPILER_64_LO(temp_cdr_clk_64);
           break;
        case phymodOversampleMode10:
            temp_cdr_clk = *rxcdr_clk / 10;
            break;
        case phymodOversampleMode16P5:
        case phymodOversampleMode20P625:
            temp_cdr_clk = *rxcdr_clk;
            break;
        case phymodOversampleMode2P5:
            COMPILER_64_SET(temp_cdr_clk_64, 0, *rxcdr_clk);
            COMPILER_64_UMUL_32(temp_cdr_clk_64, 10);
            COMPILER_64_UDIV_32(temp_cdr_clk_64, 2 * 10 + 5);
            temp_cdr_clk = COMPILER_64_LO(temp_cdr_clk_64);
            break;
        default:
            temp_cdr_clk = *rxcdr_clk;
    }

    *txpi_clk = temp_cdr_clk;
    *rxcdr_clk = temp_cdr_clk;

exit:
    SOC_FUNC_RETURN;
}

/*
 * Function:
 *      pm8x50_flexe_gen2_preemption_control_set
 * Purpose:
 *      This function configures preemption feature parameters
 *      The application layer has to call this function with
 *      associated preemption-controls to configure parameters.
 * Parameters:
 *      unit                -(IN) Device unit number
 *      port                -(IN) logical port
 *      type                -(IN) preemption parameter to be configured
 *      value               -(IN) value
 * Returns:
 *      Status
 */
int pm8x50_flexe_gen2_preemption_control_set(int unit, int port, pm_info_t pm_info,
                                          portmod_preemption_control_t type,
                                          uint32 value)
{
    SOC_INIT_FUNC_DEFS;

    switch(type) {
        case  portmodPreemptionControlInvalid:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
              (_SOC_MSG("Invalid preemption control type(%d)"), type));
            break;
        case  portmodPreemptionControlPreemptionSupport:
            /* Enable/disable port preemption capability support */
            _SOC_IF_ERR_EXIT(cdmac_preemption_mode_set(unit, port, value));
            break;
        case  portmodPreemptionControlEnableTx:
            /*
             * Enable/disable port preemption on tx side
             * if verify_enable = 1
             * this function call blocks until, verify operation is complete
             * if verify_enable = 0
             * means static preemption, function call returns immediately
             * after enabling preemption in the hardware in tx direction
             */
            _SOC_IF_ERR_EXIT(cdmac_preemption_tx_enable_set(unit, port, value));
            break;
        case  portmodPreemptionControlVerifyEnable:
            /*
             * Enable/disable verify operation.
             * Before enabling the verify operation on a port,
             * the number of attempts for a veriy operation and
             * the timeout for each verify operation should be configured
             */
            _SOC_IF_ERR_EXIT(
                cdmac_preemption_verify_enable_set(unit, port, value));
            break;
        case  portmodPreemptionControlVerifyTime:
            /*
             * Configuring the verify timeout is valid only when verify
             * operation is enabled.
             * The timeout here means, if verify operation is not complete
             * success/failure/inprogress after the (timeout + 1) millisecs
             * the MAC sends another verify message to the link partner
             */
            _SOC_IF_ERR_EXIT(
                cdmac_preemption_verify_time_set(unit, port, value));
            break;
        case  portmodPreemptionControlVerifyAttempts:
            /*
             * Configuring the number of verify attempts is valid only when
             * verify operation is enabled.
             * If verify operation is not complete success/failure/inprogress
             * after the (timeout + 1) millisecs the MAC sends another verify
             * to the link partner. The MAC tries for (num_verify_attempts + 1)
             * to complete the verify operation.
             */
            _SOC_IF_ERR_EXIT(
                cdmac_preemption_verify_attempts_set(unit, port, value));
            break;
        case  portmodPreemptionControlNonFinalFragSizeRx:
            /* configure non-final rx fragment size */
            _SOC_IF_ERR_EXIT(
                cdmac_preemption_rx_frag_size_set(unit, port,
                                      PORTMOD_PREEMPTION_NON_FINAL_FRAG,
                                      value));
            break;
        case  portmodPreemptionControlFinalFragSizeRx:
            /* configure final rx fragment size */
            _SOC_IF_ERR_EXIT(
                cdmac_preemption_rx_frag_size_set(unit, port,
                                          PORTMOD_PREEMPTION_FINAL_FRAG,
                                          value));
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
              (_SOC_MSG("Invalid preemption control type(%d)"), type));
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * Function:
 *      pm8x50_flexe_gen2_preemption_control_get
 * Purpose:
 *      This function gets the value of preemption parameters
 *      configured. The application layer has to call this function with
 *      associated preemption-controls to configure parameters.
 * Parameters:
 *      unit                -(IN) Device unit number
 *      port                -(IN) logical port
 *      type                -(IN) preemption parameter to be configured
 *      value               -(OUT) value returned from hardware
 * Returns:
 *      Status
 */
int pm8x50_flexe_gen2_preemption_control_get(int unit, int port, pm_info_t pm_info,
                                          portmod_preemption_control_t type,
                                          uint32 *value)
{
    SOC_INIT_FUNC_DEFS;

    switch(type) {
        case  portmodPreemptionControlInvalid:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
              (_SOC_MSG("Invalid preemption control type(%d)"), type));
            break;
        case  portmodPreemptionControlPreemptionSupport:
            _SOC_IF_ERR_EXIT(cdmac_preemption_mode_get(unit, port, value));
            break;
        case  portmodPreemptionControlEnableTx:
            _SOC_IF_ERR_EXIT(cdmac_preemption_tx_enable_get(unit, port, value));
            break;
        case  portmodPreemptionControlVerifyEnable:
            _SOC_IF_ERR_EXIT(
                cdmac_preemption_verify_enable_get(unit, port, value));
            break;
        case  portmodPreemptionControlVerifyTime:
            _SOC_IF_ERR_EXIT(
                cdmac_preemption_verify_time_get(unit, port, value));
            break;
        case  portmodPreemptionControlVerifyAttempts:
            _SOC_IF_ERR_EXIT(
                cdmac_preemption_verify_attempts_get(unit, port, value));
            break;
        case  portmodPreemptionControlNonFinalFragSizeRx:
            /* get non-final rx fragment size */
            _SOC_IF_ERR_EXIT(
                cdmac_preemption_rx_frag_size_get(unit, port,
                                      PORTMOD_PREEMPTION_NON_FINAL_FRAG,
                                      value));
            break;
        case  portmodPreemptionControlFinalFragSizeRx:
            /* get final rx fragment size */
            _SOC_IF_ERR_EXIT(
                cdmac_preemption_rx_frag_size_get(unit, port,
                                          PORTMOD_PREEMPTION_FINAL_FRAG,
                                          value));
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
              (_SOC_MSG("Invalid preemption control type(%d)"), type));
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

/*!
 * pm8x50_flexe_gen2_preemption_tx_status_get
 *
 * @brief Get the preemption status on the transmit side Indicates if
 *        preemption on a port is
 *        Inactive: (Verifiy is enabled but its status is either failed
 *                  or not initiated) or (preemption is not enabled)
 *        Verify in progress: Preemption is enabled and Verify is
 *                            enabled and its operation is in progress
 *        Active: Preemption is enabled and ((Verify is enabled and
 *                succeeded) or (Verify is NOT enabled))
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [out] value           - indicates preemption status in tx direction.
 */
int pm8x50_flexe_gen2_preemption_tx_status_get(int unit, int port,
                                            pm_info_t pm_info,
                                            uint32* value)
{
    return (cdmac_preemption_tx_status_get(unit, port,
                                           CDMAC_PREEMPTION_TX_STATUS,
                                           value));
}

/*!
 * pm8x50_flexe_gen2_preemption_verify_status_get
 *
 * @brief Get the preemption verify status.
 *        This function gets verify operation status or verify count
 *        status based on the verify status type argument.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  type            - portmod_preemption_verify_status_type_t
 * @param [out] value           - indicates preemption verify op status
 */
int pm8x50_flexe_gen2_preemption_verify_status_get(int unit, int port,
                                         pm_info_t pm_info,
                                         portmod_preemption_verify_status_type_t type,
                                         uint32* value)
{
    SOC_INIT_FUNC_DEFS;

    switch (type) {
        case portmodPreemptionVerifyStatus:
            _SOC_IF_ERR_EXIT(cdmac_preemption_tx_status_get(unit, port,
                                       CDMAC_PREEMPTION_VERIFY_STATUS,
                                       value));
            break;
        case portmodPreemptionVerifyCountStatus:
            _SOC_IF_ERR_EXIT(cdmac_preemption_tx_status_get(unit, port,
                                  CDMAC_PREEMPTION_VERIFY_CNT_STATUS,
                                  value));
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
              (_SOC_MSG("Invalid verify status type(%d)"), type));
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

/*!
 * portmod_preemption_force_rx_eop_mode_set
 *
 * @brief Set preemption forced rx lost eop(end of packet) sequence mode
 * setting. The rx lost eop sequence can be either software or hardware
 * enforced.
 * If HW based rx lost eop is enabled, link down or rx disable even
 * forces rx_lost_eop sequence after some internal delay if preempt
 * packet EOP was not sent.
 * If Sw based rx lost eop is enabled,  the application rx eop sequence
 * When SW rx lost EOP is enabled(done by application layer), rising
 * edge MAC forces rx_lost_eop sequence after some internal delay if
 * preempt packet EOP was not sent.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  enable          - enable/disable
 * @param [in]  timeout_cnt     - timeout in millisecs
 */
int pm8x50_flexe_gen2_preemption_force_rx_eop_mode_set(int unit, int port,
                                      pm_info_t pm_info,
                                      portmod_preemption_force_rx_eop_mode_t type,
                                      uint32 value)
{
    SOC_INIT_FUNC_DEFS;

    switch (type) {
        case portmodPreemptionForceSwRxEop:
            _SOC_IF_ERR_EXIT(cdmac_preemption_force_rx_lost_eop_set(unit, port,
                                              CDMAC_PREEMPTION_FORCE_SW_RX_EOP,
                                              value));
            break;
        case portmodPreemptionForceAutoHwEop:
            _SOC_IF_ERR_EXIT(cdmac_preemption_force_rx_lost_eop_set(unit, port,
                                         CDMAC_PREEMPTION_FORCE_AUTO_HW_RX_EOP,
                                         value));
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
              (_SOC_MSG("Invalid Force RX lost EOP type(%d)"), type));
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

/*!
 * pm8x50_flexe_gen2_preemption_force_rx_eop_mode_get
 *
 * @brief Get preemption forced rx lost eop(end of packet) sequence mode
 * setting. The rx lost eop sequence can be either software or hardware
 * enforced.
 * This functin returns the rx lost eop setting based on the type
 * of lost eop is enforced.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [out] type            - portmod_preemption_force_rx_eop_mode_t
 * @param [out] value           - timeout in millisecs
 */
int pm8x50_flexe_gen2_preemption_force_rx_eop_mode_get(int unit, int port,
                                         pm_info_t pm_info,
                                         portmod_preemption_force_rx_eop_mode_t type,
                                         uint32 *value)
{
    SOC_INIT_FUNC_DEFS;

    switch (type) {
        case portmodPreemptionForceSwRxEop:
            _SOC_IF_ERR_EXIT(cdmac_preemption_force_rx_lost_eop_get(unit, port,
                                              CDMAC_PREEMPTION_FORCE_SW_RX_EOP,
                                              value));
            break;
        case portmodPreemptionForceAutoHwEop:
            _SOC_IF_ERR_EXIT(cdmac_preemption_force_rx_lost_eop_get(unit, port,
                                         CDMAC_PREEMPTION_FORCE_AUTO_HW_RX_EOP,
                                         value));
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
              (_SOC_MSG("Invalid Force RX lost EOP type(%d)"), type));
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

/*!
 * pm8x50_flexe_gen2_preemption_rx_timeout_set
 *
 * @brief Set specified preemption rx timeout and timeout count.
 * This function should only be called during device init.
 * There are 2 rx parameters rx timeout enable, rx timeout value in microsec
 * that can be configured using this function.
 * If set, enables TIMEOUT mechanism for Rx preempt traffic.
 * Timeout value in microsec, this configuration indicates link idle
 * time(in microsec) after which Rx timeout will force rx_lost_eop sequence
 * if preempt packet EOP was not sent.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  enable          - enables timeout for rx preempt traffic
 * @param [in]  timeout_cnt     - timeout in microsecs
 */
int pm8x50_flexe_gen2_preemption_rx_timeout_set(int unit, int port, pm_info_t pm_info,
                                              uint32 enable, uint32 timeout_cnt)
{
    return (cdmac_preemption_rx_timeout_info_set(unit, port, enable,
                                                 timeout_cnt));
}

/*!
 * pm8x50_flexe_gen2_preemption_rx_timeout_set
 *
 * @brief Get specified preemption rx timeout and timeout count.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  enable          - enables timeout for rx preempt traffic
 * @param [out] timeout_cnt     - timeout in millisec
 */
int pm8x50_flexe_gen2_preemption_rx_timeout_get(int unit, int port, pm_info_t pm_info,
                                             uint32 *enable, uint32 *timeout_cnt)
{
    return (cdmac_preemption_rx_timeout_info_get(unit, port, enable,
                                                 timeout_cnt));
}

/*! 
 * portmod_fec_degraded_ser_set
 *
 * @brief FEC Degraded SER configuration set/get 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  config          - FEC Degraded SER configs
 */
int pm8x50_flexe_gen2_fec_degraded_ser_set(int unit,
                                           int port,
                                           pm_info_t pm_info,
                                           portmod_fec_degraded_ser_config_t config)
{
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    phymod_phy_speed_config_t phy_speed_config;
    phymod_fec_degraded_ser_config_t degrade_config;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    /* get current speed */
    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_t_init(&phy_speed_config));
    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_get(&phy_access, &phy_speed_config));
    /* FEC Degraded SER feature is applicable for 200G & 400G only */
    if ((phy_speed_config.data_rate != 200000) &&
        (phy_speed_config.data_rate != 400000)) {
        _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
            (_SOC_MSG("port %d: FEC Degraded SER is not supported for current speed."), port));
    }

    degrade_config.enable = config.enable;
    degrade_config.activate_threshold = config.activate_threshold;
    degrade_config.deactivate_threshold = config.deactivate_threshold;
    degrade_config.interval = config.interval;
    _SOC_IF_ERR_EXIT(phymod_phy_fec_degraded_ser_set(&phy_access, degrade_config));

exit:
    SOC_FUNC_RETURN;
}

/*! 
 * portmod_fec_degraded_ser_get
 *
 * @brief FEC Degraded SER configuration set/get 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [out]  config          - FEC Degraded SER configs
 */
int pm8x50_flexe_gen2_fec_degraded_ser_get(int unit,
                                           int port,
                                           pm_info_t pm_info,
                                           portmod_fec_degraded_ser_config_t* config)
{
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    phymod_phy_speed_config_t phy_speed_config;
    phymod_fec_degraded_ser_config_t degrade_config;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    /* get current speed */
    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_t_init(&phy_speed_config));
    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_get(&phy_access, &phy_speed_config));
    /* FEC Degraded SER feature is applicable for 200G & 400G only */
    if ((phy_speed_config.data_rate != 200000) &&
        (phy_speed_config.data_rate != 400000)) {
        _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
            (_SOC_MSG("port %d: FEC Degraded SER is not supported for current speed."), port));
    }
    /* get current FEC Degraded SER settings */
    _SOC_IF_ERR_EXIT(phymod_phy_fec_degraded_ser_get(&phy_access, &degrade_config));

    config->enable = degrade_config.enable;
    config->activate_threshold = degrade_config.activate_threshold;
    config->deactivate_threshold = degrade_config.deactivate_threshold;
    config->interval = degrade_config.interval;

exit:
    SOC_FUNC_RETURN;
}

/*! 
 * portmod_fec_degraded_ser_status_get
 *
 * @brief FEC Degraded SER status get 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [out]  status          - FEC Degraded SER status
 */
int pm8x50_flexe_gen2_fec_degraded_ser_status_get(int unit,
                                                  int port,
                                                  pm_info_t pm_info,
                                                  portmod_fec_degraded_ser_status_t* status)
{
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    phymod_phy_speed_config_t phy_speed_config;
    phymod_fec_degraded_ser_status_t degrade_status;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_flexe_gen2_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    /* get current speed */
    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_t_init(&phy_speed_config));
    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_get(&phy_access, &phy_speed_config));
    /* FEC Degraded SER feature is applicable for 200G & 400G only */
    if ((phy_speed_config.data_rate != 200000) &&
        (phy_speed_config.data_rate != 400000)) {
        _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
            (_SOC_MSG("port %d: FEC Degraded SER is not supported for current speed."), port));
    }
    /* get FEC Degraded SER status */
    _SOC_IF_ERR_EXIT(phymod_phy_fec_degraded_ser_status_get(&phy_access, &degrade_status));

    status->fec_degraded_ser = degrade_status.fec_degraded_ser;
    status->rx_local_degraded = degrade_status.rx_local_degraded;
    status->rx_remote_degraded = degrade_status.rx_remote_degraded;
    status->rx_am_status_field = degrade_status.rx_am_status_field;

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_flexe_gen2_port_update(int unit, int port, pm_info_t pm_info,
                                  const portmod_port_update_control_t* update_control)
{

    return (SOC_E_NONE);
}

#endif
