/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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

#ifdef PORTMOD_PM8X50_SUPPORT

#include <soc/portmod/cdmac.h>
#include <soc/portmod/pm8x50.h>
#include <soc/portmod/pm8x50_shared.h>
#include <soc/portmod/pm8x50_private.h>
#include <soc/portmod/pm8x50_internal.h>

#define PM_8x50_INFO(pm_info) ((pm_info)->pm_data.pm8x50_db)

#define PM8x50_FLEXE_TYPE  0x8
/* Warmboot variable defines - start */

#define PM8x50_WB_BUFFER_VERSION        (6)
#define PORTMOD_THREAD_INTERVAL_1s 1000000
#define TIMEOUT_COUNT_400G_AN     10

#define PM8x50_IS_CORE_INITIALIZED_SET(unit, pm_info, is_core_initialized) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[isCoreInitialized],         \
                          &is_core_initialized)
#define PM8x50_IS_CORE_INITIALIZED_GET(unit, pm_info, is_core_initialized) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[isCoreInitialized],         \
                          is_core_initialized)

#define PM8x50_IS_ACTIVE_SET(unit, pm_info, is_active)   \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[isActive],\
                         &is_active)
#define PM8x50_IS_ACTIVE_GET(unit, pm_info, is_active)    \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[isActive],\
                          is_active)

#define PM8x50_OVCO_PLL_ACTIVE_LANE_BITMAP_SET(unit, pm_info, ovco_pll_active_lane_bitmap)\
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                            \
                          pm_info->wb_vars_ids[ovcopllActiveLaneBitmap],             \
                          &ovco_pll_active_lane_bitmap)
#define PM8x50_OVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, ovco_pll_active_lane_bitmap)\
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                            \
                          pm_info->wb_vars_ids[ovcopllActiveLaneBitmap],             \
                          ovco_pll_active_lane_bitmap)

#define PM8x50_TVCO_PLL_ACTIVE_LANE_BITMAP_SET(unit, pm_info, tvco_pll_active_lane_bitmap)\
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                            \
                          pm_info->wb_vars_ids[tvcopllActiveLaneBitmap],             \
                          &tvco_pll_active_lane_bitmap)
#define PM8x50_TVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, tvco_pll_active_lane_bitmap)\
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                            \
                          pm_info->wb_vars_ids[tvcopllActiveLaneBitmap],             \
                          tvco_pll_active_lane_bitmap)

#define PM8x50_OVCO_PLL_ADV_LANE_BITMAP_SET(unit, pm_info, ovco_pll_adv_lane_bitmap)\
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[ovcopllAdvLaneBitmap],          \
                          &ovco_pll_adv_lane_bitmap)
#define PM8x50_OVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, ovco_pll_adv_lane_bitmap)\
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[ovcopllAdvLaneBitmap],          \
                          ovco_pll_adv_lane_bitmap)

#define PM8x50_TVCO_PLL_ADV_LANE_BITMAP_SET(unit, pm_info, tvco_pll_adv_lane_bitmap)\
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[tvcopllAdvLaneBitmap],          \
                          &tvco_pll_adv_lane_bitmap)
#define PM8x50_TVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, tvco_pll_adv_lane_bitmap)\
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[tvcopllAdvLaneBitmap],          \
                          tvco_pll_adv_lane_bitmap)

#define PM8x50_LANE2PORT_SET(unit, pm_info, lane, port)                  \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                   \
                          pm_info->wb_vars_ids[lane2portMap], &port, lane)
#define PM8x50_LANE2PORT_GET(unit, pm_info, lane, port)                 \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                  \
                          pm_info->wb_vars_ids[lane2portMap], port, lane)

#define PM8x50_MAX_SPEED_SET(unit, pm_info, max_speed, port_index)              \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                          \
                          pm_info->wb_vars_ids[maxSpeed], &max_speed, port_index)
#define PM8x50_MAX_SPEED_GET(unit, pm_info, max_speed, port_index)             \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                         \
                          pm_info->wb_vars_ids[maxSpeed], max_speed, port_index)

#define PM8x50_AN_MODE_SET(unit, pm_info, an_mode, port_index)              \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[anMode], &an_mode, port_index)
#define PM8x50_AN_MODE_GET(unit, pm_info, an_mode, port_index)             \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[anMode], an_mode, port_index)

#define PM8x50_LANE2FEC_SET(unit, pm_info, lane, fec)                   \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                  \
                          pm_info->wb_vars_ids[lane2fecMap], &fec, lane)
#define PM8x50_LANE2FEC_GET(unit, pm_info, lane, fec)                   \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                  \
                          pm_info->wb_vars_ids[lane2fecMap], fec, lane)

#define PM8x50_PORT_IS_PCS_BYPASSED_SET(unit, pm_info, is_bypassed, port_index)             \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                                      \
                          pm_info->wb_vars_ids[portIsPcsBypassed], &is_bypassed, port_index)
#define PM8x50_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, is_bypassed, port_index)             \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                                      \
                          pm_info->wb_vars_ids[portIsPcsBypassed], is_bypassed, port_index)

#define PM8x50_TS_ENABLE_PORT_COUNT_SET(unit, pm_info, ts_enable_port_count)   \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[tsEnablePortCount],\
                         &ts_enable_port_count)
#define PM8x50_TS_ENABLE_PORT_COUNT_GET(unit, pm_info, ts_enable_port_count)    \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[tsEnablePortCount],\
                          ts_enable_port_count)

#define PM8x50_TIMESYNC_ENABLE_SET(unit, pm_info, timesync_enable, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[timesyncEnable], &timesync_enable, port_index)
#define PM8x50_TIMESYNC_ENABLE_GET(unit, pm_info, timesync_enable, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[timesyncEnable], timesync_enable, port_index)

#define PM8x50_RLM_ENABLE_SET(unit, pm_info, rlmEnabled, port_index)                   \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[rlmEnable], &rlmEnabled, port_index)
#define PM8x50_RLM_ENABLE_GET(unit, pm_info, rlmEnabled, port_index)             \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[rlmEnable], rlmEnabled, port_index)

#define PM8x50_INITIATOR_SET(unit, pm_info, is_initiator, port_index)                   \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[initiator], &is_initiator, port_index)
#define PM8x50_INITIATOR_GET(unit, pm_info, is_initiator, port_index)             \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[initiator], is_initiator, port_index)

#define PM8x50_PCS_RECONFIG_SET(unit, pm_info, pcs_reconfig, port_index)                   \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[pcsReconfigured], &pcs_reconfig, port_index)
#define PM8x50_PCS_RECONFIG_GET(unit, pm_info, pcs_reconfig, port_index)             \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[pcsReconfigured], pcs_reconfig, port_index)

#define PM8x50_ACTIVE_LANE_MAP_SET(unit, pm_info, active_lane_map, port_index)                   \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[activeLaneBitMap], &active_lane_map, port_index)
#define PM8x50_ACTIVE_LANE_MAP_GET(unit, pm_info, active_lane_map, port_index)             \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[activeLaneBitMap], active_lane_map, port_index)

#define PM8x50_RLM_STATE_SET(unit, pm_info, rlm_state, port_index)                   \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[rlmState], &rlm_state, port_index)
#define PM8x50_RLM_STATE_GET(unit, pm_info, rlm_state, port_index)             \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[rlmState], rlm_state, port_index)

#define PM8x50_ORIGINAL_FEC_SET(unit, pm_info, orig_fec, port_index)                   \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[originalFec], &orig_fec, port_index)
#define PM8x50_ORIGINAL_FEC_GET(unit, pm_info, orig_fec, port_index)             \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[originalFec], orig_fec, port_index)

#define PM8x50_ORIG_TX_LANE_MAP_SET(unit, pm_info, tx_lane_map)\
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[originalTxLaneSwap],          \
                          &tx_lane_map)
#define PM8x50_ORIG_TX_LANE_MAP_GET(unit, pm_info, tx_lane_map)\
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[originalTxLaneSwap],          \
                          tx_lane_map)

#define PM8x50_ORIG_RX_LANE_MAP_SET(unit, pm_info, rx_lane_map)\
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[originalRxLaneSwap],          \
                          &rx_lane_map)
#define PM8x50_ORIG_RX_LANE_MAP_GET(unit, pm_info, rx_lane_map)\
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[originalRxLaneSwap],          \
                          rx_lane_map)

#define PM8x50_400G_AN_ENABLE_SET(unit, pm_info, is_400g_an, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[is400gAn], &is_400g_an, port_index)
#define PM8x50_400G_AN_ENABLE_GET(unit, pm_info, is_400g_an, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[is400gAn], is_400g_an, port_index)

#define PM8x50_400G_AN_STATE_SET(unit, pm_info, an_state_400g)   \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[anState400g],\
                         &an_state_400g)
#define PM8x50_400G_AN_STATE_GET(unit, pm_info, an_state_400g)    \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[anState400g],\
                          an_state_400g)

#define PM8x50_400G_AN_WAIT_LINK_COUNT_SET(unit, pm_info, an_wait_link_count)   \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[anWaitlinkCount],\
                         &an_wait_link_count)
#define PM8x50_400G_AN_WAIT_LINK_COUNT_GET(unit, pm_info, an_wait_link_count)    \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[anWaitlinkCount],\
                          an_wait_link_count)

#define PM8x50_BER_WAR_ENABLE_SET(unit, pm_info, is_ber_war_enable, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[isBerWarEnable], &is_ber_war_enable, port_index)
#define PM8x50_BER_WAR_ENABLE_GET(unit, pm_info, is_ber_war_enable, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[isBerWarEnable], is_ber_war_enable, port_index)

#define PM8x50_BER_WAR_STATE_SET(unit, pm_info, ber_war_state, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[berWarState], &ber_war_state, port_index)

#define PM8x50_BER_WAR_STATE_GET(unit, pm_info, ber_war_state, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[berWarState], ber_war_state, port_index)

#define PM8x50_BER_WAR_WAIT_COUNT_SET(unit, pm_info, ber_war_wait_count, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[berWarWaitCount], &ber_war_wait_count, port_index)

#define PM8x50_BER_WAR_WAIT_COUNT_GET(unit, pm_info, ber_war_wait_count, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[berWarWaitCount], ber_war_wait_count, port_index)

#define PM8x50_IS_CORE_FLEXE_CONFIGURED_SET(unit, pm_info, is_core_flexe_configured) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[coreisFlexeConfigured],         \
                          &is_core_flexe_configured)
#define PM8x50_IS_CORE_FLEXE_CONFIGURED_GET(unit, pm_info, is_core_flexe_configured) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[coreisFlexeConfigured],         \
                          is_core_flexe_configured)

#define PM8x50_400G_AN_REMOTE_PAUSE_SET(unit, pm_info, an_400g_remote_pause, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[an400gRemotePause], &an_400g_remote_pause, port_index)
#define PM8x50_400G_AN_REMOTE_PAUSE_GET(unit, pm_info, an_400g_remote_pause, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[an400gRemotePause], an_400g_remote_pause, port_index)

#define PM8x50_3_CW_BAD_ENABLE_SET(unit, pm_info, is_3_cw_bad_enable, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[isThreeCodewordBad], &is_3_cw_bad_enable, port_index)
#define PM8x50_3_CW_BAD_ENABLE_GET(unit, pm_info, is_3_cw_bad_enable, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[isThreeCodewordBad], is_3_cw_bad_enable, port_index)

#define PM8x50_3_CW_BAD_STATE_SET(unit, pm_info, three_cw_bad_state, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[threeCodewordBadState], &three_cw_bad_state, port_index)

#define PM8x50_3_CW_BAD_STATE_GET(unit, pm_info, three_cw_bad_state, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[threeCodewordBadState], three_cw_bad_state, port_index)

#define PM8x50_3_CW_BAD_TICK_CNT_SET(unit, pm_info, three_cw_bad_tick_cnt, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[threeCodewordBadTickCnt], &three_cw_bad_tick_cnt, port_index)

#define PM8x50_3_CW_BAD_TICK_CNT_GET(unit, pm_info, three_cw_bad_tick_cnt, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[threeCodewordBadTickCnt], three_cw_bad_tick_cnt, port_index)

#define PM8x50_IS_FLEXE_TVCO_26G_REQUIRED_SET(unit, pm_info, is_flexe_tvco_26g_required) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[isFlexeTvco26gRequired],         \
                          &is_flexe_tvco_26g_required)
#define PM8x50_IS_FLEXE_TVCO_26G_REQUIRED_GET(unit, pm_info, is_flexe_tvco_26g_required) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[isFlexeTvco26gRequired],         \
                          is_flexe_tvco_26g_required)

#define PM8x50_PORT_FLEXE_MODE_SET(unit, pm_info, port_flexe_mode, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[portFlexeMode], &port_flexe_mode, port_index)
#define PM8x50_PORT_FLEXE_MODE_GET(unit, pm_info, port_flexe_mode, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                          pm_info->wb_vars_ids[portFlexeMode], port_flexe_mode, port_index)

typedef enum pm8x50_wb_vars{
    isCoreInitialized,
    isActive,
    isBypassed, /* Removed */
    ovcopllActiveLaneBitmap,
    tvcopllActiveLaneBitmap,
    ovcopllAdvLaneBitmap,
    tvcopllAdvLaneBitmap,
    lane2portMap,
    maxSpeed,
    anMode,
    anFec,  /* Removed */
    fsCl72, /* Removed */
    lane2fecMap,
    tsEnablePortCount,
    timesyncEnable,
    rlmEnable,        /* for RLM */
    initiator,        /* for RLM */
    pcsReconfigured,  /* for RLM */
    activeLaneBitMap, /* for RLM */
    rlmState,           /* for RLM */
    originalFec,        /* for RLM */
    originalTxLaneSwap, /* for RLM */
    originalRxLaneSwap, /* for RLM */
    is400gAn,           /* 400g An enabled */
    anState400g,        /* 400G an state */
    anWaitlinkCount,    /* 400G wait for link up count */
    portIsPcsBypassed,
    isBerWarEnable,     /* 200G and 400G BER WAR enabled when FEC bypass indication ON */
    berWarState,        /* 200G and 400G BER WAR state */
    berWarWaitCount,    /* 200G and 400G BER WAR count */
    coreisFlexeConfigured,   /* this WB variable is unused */
    an400gRemotePause,  /* 400G AN Remote PAUSE value */
    isThreeCodewordBad,    /*FEC 3_BAD recover WAR enabled*/
    threeCodewordBadState,       /*FEC 3_BAD recover WAR state*/
    threeCodewordBadTickCnt,     /*FEC 3_BAD recover WAR tick cnt*/
    isFlexeTvco26gRequired,    /* used for Pm8x50FlexE if TVCO is required to set to 26G */
    portFlexeMode,               /* if port is in flexe mode */
}pm8x50_wb_vars_t;

/* Warmboot variable defines - end */

/* This macro set devad to reg_addr if it is PMD phy address. */
#define PM8X50_PHY_REG_SET(reg_addr) \
    do { \
        uint32 phy_reg_addr; \
        phy_reg_addr = reg_addr & 0xffff; \
        if ((0x90 <= phy_reg_addr && phy_reg_addr <= 0x9f) || \
            (0xd000 <= phy_reg_addr && phy_reg_addr <= 0xd4ff) || \
            (0xffd0 <= phy_reg_addr && phy_reg_addr <= 0xffdf)) { \
            reg_addr |= 1 << 16; \
        } \
    } while (0)

#define PM8X50_MAX_NUM_PLLS (2)

#define PM8X50_TVCO_PLL_INDEX_GET(pm_info) PM_8x50_INFO(pm_info)->int_core_access.access.tvco_pll_index

#define PM8x50_VCO_ALL (PM8x50_VCO_ILKN_20P625G | PM8x50_VCO_ETH_20P625G | PM8x50_VCO_ILKN_25G | PM8x50_VCO_ETH_25G | PM8x50_VCO_ETH_25P781G | PM8x50_VCO_ILKN_25P781G | PM8x50_VCO_ETH_26P562G | PM8x50_VCO_ILKN_26P562G | PM8x50_VCO_ILKN_27P343G | PM8x50_VCO_ILKN_28P125G)

#define PM8x50_VCO_BMP_EMPTY(bmp) \
        ( (bmp & PM8x50_VCO_ALL) == 0 )

#define PM8x50_VCO_BMP_20P625G_ETH_ONLY(bmp) \
        ( (bmp == PM8x50_VCO_ETH_20P625G) || (bmp == (PM8x50_VCO_ETH_20P625G | PM8x50_VCO_ILKN_20P625G)) )

#define PM8x50_VCO_BMP_25G_ETH_ONLY(bmp) \
        ((bmp == PM8x50_VCO_ETH_25G) || (bmp == (PM8x50_VCO_ETH_25G | PM8x50_VCO_ILKN_25G)))

/* VCO is dedicated for bypassed ports */
#define PORTMOD_VCO_PCS_BYPASS_DEDICATED(vco) \
    ((vco == portmodVCO28P125G) || (vco == portmodVCO27P343G) || (vco == portmodVCO25G))

/* Highest NRZ per lane speed is 28.125G, supported in ILKN mode */
#define NRZ_MAX_PER_LANE_SPEED 28125

#define PM8x50_FS_ABILITY_TABLE_SIZE 29
#define PM8x50_AN_ABILITY_TABLE_SIZE 35
#define PM8x50_GEN2_FS_ABILITY_TABLE_SIZE 32
#define PM8x50_GEN2_AN_ABILITY_TABLE_SIZE 37
#define PM8x50_MAX_AN_ABILITY 20

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

STATIC portmod_ucode_buf_t pm8x50_ucode_buf[SOC_MAX_NUM_DEVICES] = {{NULL, 0}};
STATIC portmod_ucode_buf_t pm8x50_ucode_buf_2nd[SOC_MAX_NUM_DEVICES] = {{NULL, 0}};

typedef enum pm8x50_port_400g_an_state_e {
    PM8X50_PORT_AN_CHECK_PAGE_COMPLETE,
    PM8X50_PORT_AN_CHECK_PCS_LINKUP,
    PM8X50_PORT_AN_CREDIT_RESET,
    PM8X50_PORT_AN_DISABLE,
    PM8X50_PORT_AN_RESTART,
    PM8X50_PORT_AN_COMPLETE,
    PM8X50_PORT_AN_STATE_COUNT
} pm8x50_port_400g_an_state_t;

/*
 * 200G and 400G BER monitoring workaround state
 */
typedef enum pm8x50_port_ber_war_state_e {
    PM8X50_PORT_BER_WAR_HI_SER_CHECK,
    PM8X50_PORT_BER_WAR_PATH_RESET,
    PM8X50_PORT_BER_WAR_WAIT_COUNT,
    PM8X50_PORT_BER_WAR_COMPLETE,
    PM8X50_PORT_BER_WAR_STATE_COUNT
} pm8x50_port_ber_war_state_t;

/*
 * FEC 3_bad recover workaround state
 */
typedef enum pm8x50_port_3_cw_bad_state_e {
    PM8X50_PORT_3_CW_BAD_RECOVER,
    PM8X50_PORT_3_CW_BAD_CHECK,
    PM8X50_PORT_3_CW_BAD_LINK_WAIT,
    PM8X50_PORT_3_CW_BAD_COMPLETED,
    PM8X50_PORT_3_CW_BAD_STATE_COUNT
} pm8x50_port_3_cw_bad_state_t;

/*
 * Entries of the force speed ability table; each entry specifies a
 * unique FS port speed ability and its associated VCO rate
 */
typedef struct pm8x50_fs_ability_table_entry_s {
    uint32 speed; /* port speed in Mbps */
    uint32 num_lanes; /* number of lanes */
    portmod_fec_t fec_type; /* FEC type */
    portmod_vco_type_t vco; /* associated VCO rate of the ability */
} pm8x50_fs_ability_table_entry_t;

/*
 * Entries of the autoneg ability table; each entry specifies a unique
 * AN speed ability and its associated VCO rate
 */
typedef struct pm8x50_an_ability_table_entry_s {
    uint32 speed; /* port speed in Mbps */
    uint32 num_lanes; /* number of lanes */
    portmod_fec_t fec_type; /* FEC type */
    portmod_port_phy_control_autoneg_mode_t an_mode; /* autoneg mode such as cl73, bam or msa */
    portmod_vco_type_t vco; /* associated VCO rate of the ability */
} pm8x50_an_ability_table_entry_t;

typedef enum cdmac_port_mode_e{
    CDMAC_4_LANES_SEPARATE  = 0,
    CDMAC_3_TRI_0_1_2_2     = 1,
    CDMAC_3_TRI_0_0_2_3     = 2,
    CDMAC_2_LANES_DUAL      = 3,
    CDMAC_4_LANES_TOGETHER  = 4
} cdmac_port_mode_t;

#define CDMAC_NUM_LANES 4
/*
 * Defines:
 *     portmod_port_rlm_fsm_t *
 * Purpose:
 *     Valid values Reduced Lane Mode state
 */
typedef enum portmod_port_rlm_fsm_e {
    PORTMOD_PORT_RLM_SEND_PACKET_DATA,  /*entry state for RLM */
    PORTMOD_PORT_RLM_STOP_MAC,          /* STALL TX */
    PORTMOD_PORT_RLM_SEND_LI,           /* SEND link interrupt protocol */
    PORTMOD_PORT_RLM_SEND_PRBS,         /* SEND prbs data  */
    PORTMOD_PORT_RLM_SEND_IDLE,         /* send idle */
    PORTMOD_PORT_RLM_DONE,              /* RLM transition done */
    PORTMOD_PORT_RLM_ERROR,             /* RLM error state */
    PORTMOD_PORT_RLM_FSM_COUNT
} portmod_port_rlm_fsm_t;

/*
 * Defines:
 *     portmod_port_rlm_state_t *
 * Purpose:
 *     pm8x50 RLM data structure
 */
typedef struct portmod_port_rlm_state_s {
       portmod_port_rlm_fsm_t rlm_state;
 } portmod_port_rlm_state_t;


/* a comprehensive list of pm8x50 forced speed abilities and their VCO rates */
const pm8x50_fs_ability_table_entry_t pm8x50_fs_ability_table[PM8x50_FS_ABILITY_TABLE_SIZE] =
{
    /* port_speed, num_lanes, fec_type, vco */
    {10000,  1, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO20P625G},
    {10000,  1, PORTMOD_PORT_PHY_FEC_BASE_R,     portmodVCO20P625G},
    {12000,  1, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO25G    },
    {20000,  1, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO20P625G},
    {20000,  1, PORTMOD_PORT_PHY_FEC_BASE_R,     portmodVCO20P625G},
    {40000,  4, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO20P625G},
    {40000,  4, PORTMOD_PORT_PHY_FEC_BASE_R,     portmodVCO20P625G},
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
    {200000, 4, PORTMOD_PORT_PHY_FEC_RS_544,     portmodVCO26P562G},
    {200000, 4, PORTMOD_PORT_PHY_FEC_RS_544_2XN, portmodVCO26P562G},
    {400000, 8, PORTMOD_PORT_PHY_FEC_RS_544_2XN, portmodVCO26P562G}
};

/* a comprehensive list of pm8x50 gen2 forced speed abilities and their VCO rates */
const pm8x50_fs_ability_table_entry_t pm8x50_gen2_fs_ability_table[PM8x50_GEN2_FS_ABILITY_TABLE_SIZE] =
{
    /* port_speed, num_lanes, fec_type, vco */
    {10000,  1, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO20P625G},
    {10000,  1, PORTMOD_PORT_PHY_FEC_BASE_R,     portmodVCO20P625G},
    {12000,  1, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO25G    },
    {20000,  1, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO20P625G},
    {20000,  1, PORTMOD_PORT_PHY_FEC_BASE_R,     portmodVCO20P625G},
    {40000,  4, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO20P625G},
    {40000,  4, PORTMOD_PORT_PHY_FEC_BASE_R,     portmodVCO20P625G},
    {40000,  2, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO20P625G},
    {25000,  1, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO25P781G},
    {25000,  1, PORTMOD_PORT_PHY_FEC_BASE_R,     portmodVCO25P781G},
    {25000,  1, PORTMOD_PORT_PHY_FEC_RS_FEC,     portmodVCO25P781G},
    {50000,  1, PORTMOD_PORT_PHY_FEC_NONE,       portmodVCO25P781G},
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

/* a comprehensive list of pm8x50 autoneg abilities and their VCO rates */
const pm8x50_an_ability_table_entry_t pm8x50_an_ability_table[PM8x50_AN_ABILITY_TABLE_SIZE] =
{
    /* port_speed, num_lanes, fec_type, autoneg_mode, vco */
    {10000,  1, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73    , portmodVCO20P625G},
    {10000,  1, PORTMOD_PORT_PHY_FEC_BASE_R,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73    , portmodVCO20P625G},
    {20000,  1, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO20P625G},
    {20000,  1, PORTMOD_PORT_PHY_FEC_BASE_R,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO20P625G},
    {40000,  2, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO20P625G},
    {40000,  4, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73    , portmodVCO20P625G},
    {40000,  4, PORTMOD_PORT_PHY_FEC_BASE_R,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73    , portmodVCO20P625G},
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
    {100000, 4, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73    , portmodVCO25P781G},
    {200000, 4, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO25P781G},
    {50000,  1, PORTMOD_PORT_PHY_FEC_RS_544,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73    , portmodVCO26P562G},
    {50000,  2, PORTMOD_PORT_PHY_FEC_RS_544,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO26P562G},
    {100000, 2, PORTMOD_PORT_PHY_FEC_RS_544,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73    , portmodVCO26P562G},
    {100000, 4, PORTMOD_PORT_PHY_FEC_RS_544,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO26P562G},
    {200000, 4, PORTMOD_PORT_PHY_FEC_RS_544_2XN, PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73    , portmodVCO26P562G},
    {200000, 4, PORTMOD_PORT_PHY_FEC_RS_544,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO26P562G},
    {400000, 8, PORTMOD_PORT_PHY_FEC_RS_544_2XN, PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_MSA, portmodVCO26P562G}
};

/* a comprehensive list of pm8x50 autoneg abilities and their VCO rates */
const pm8x50_an_ability_table_entry_t pm8x50_gen2_an_ability_table[PM8x50_GEN2_AN_ABILITY_TABLE_SIZE] =
{
    /* port_speed, num_lanes, fec_type, autoneg_mode, vco */
    {10000,  1, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73    , portmodVCO20P625G},
    {10000,  1, PORTMOD_PORT_PHY_FEC_BASE_R,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73    , portmodVCO20P625G},
    {20000,  1, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO20P625G},
    {20000,  1, PORTMOD_PORT_PHY_FEC_BASE_R,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO20P625G},
    {40000,  2, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, portmodVCO20P625G},
    {40000,  4, PORTMOD_PORT_PHY_FEC_NONE,       PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73    , portmodVCO20P625G},
    {40000,  4, PORTMOD_PORT_PHY_FEC_BASE_R,     PORTMOD_PORT_PHY_CONTROL_AUTONEG_MODE_CL73    , portmodVCO20P625G},
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

static phymod_dispatch_type_t pm8x50_serdes_list[] =
{
#ifdef PHYMOD_TSCBH_SUPPORT
    phymodDispatchTypeTscbh,
#endif
#ifdef PHYMOD_TSCBH_GEN2_SUPPORT
    phymodDispatchTypeTscbh_gen2,
#endif
#ifdef PHYMOD_TSCBH_FLEXE_SUPPORT
    phymodDispatchTypeTscbh_flexe,
#endif
    phymodDispatchTypeInvalid
};

/*
 * given the port speed, number of lanes, and FEC type, return the VCO specified in
 * pm8x50_fs_ability_table in the associated entry; VCO should be portmodVCOInvalid
 * if the ability is invalid
 */
STATIC
int _pm8x50_fs_ability_table_vco_get(uint32 speed, uint32 num_lanes, portmod_fec_t fec_type,
                                     portmod_vco_type_t* vco)
{
    int i;

    *vco = portmodVCOInvalid;
    for (i = 0; i < PM8x50_FS_ABILITY_TABLE_SIZE; i++) {
        if (pm8x50_fs_ability_table[i].speed == speed &&
            pm8x50_fs_ability_table[i].num_lanes == num_lanes &&
            pm8x50_fs_ability_table[i].fec_type == fec_type)
        {
            *vco = pm8x50_fs_ability_table[i].vco;
            break;
        }
    }

    return SOC_E_NONE;
}

/*
 * given the port speed, number of lanes, and FEC type, return the VCO specified in
 * pm8x50_gen2_fs_ability_table in the associated entry; VCO should be portmodVCOInvalid
 * if the ability is invalid
 */
STATIC
int _pm8x50_gen2_fs_ability_table_vco_get(uint32 speed, uint32 num_lanes, portmod_fec_t fec_type,
                                          portmod_vco_type_t* vco)
{
    int i;

    *vco = portmodVCOInvalid;
    for (i = 0; i < PM8x50_GEN2_FS_ABILITY_TABLE_SIZE; i++) {
        if (pm8x50_gen2_fs_ability_table[i].speed == speed &&
            pm8x50_gen2_fs_ability_table[i].num_lanes == num_lanes &&
            pm8x50_gen2_fs_ability_table[i].fec_type == fec_type)
        {
            *vco = pm8x50_gen2_fs_ability_table[i].vco;
            break;
        }
    }

    return SOC_E_NONE;
}

/*
 * given the port speed, number of lanes, FEC type, and autoneg mode, return the VCO specified in
 * pm8x50_an_ability_table; VCO should be portmodVCOInvalid if the ability is invalid.
 * this function is temporarily masked out because there is no caller. advert_ability_get() will
 * need to call this function to verify the VCO requirement
 */
STATIC
int _pm8x50_an_ability_table_vco_get(uint32 speed, uint32 num_lanes, portmod_fec_t fec_type,
                                     portmod_port_phy_control_autoneg_mode_t an_mode,
                                     portmod_vco_type_t* vco)
{
    int i;

    *vco = portmodVCOInvalid;
    for (i = 0; i < PM8x50_AN_ABILITY_TABLE_SIZE; i++) {
        if (pm8x50_an_ability_table[i].speed == speed &&
            pm8x50_an_ability_table[i].num_lanes == num_lanes &&
            pm8x50_an_ability_table[i].fec_type == fec_type &&
            pm8x50_an_ability_table[i].an_mode == an_mode)
        {
            *vco = pm8x50_an_ability_table[i].vco;
            break;
        }
    }

    return SOC_E_NONE;
}

/*
 * given the port speed, number of lanes, FEC type, and autoneg mode, return the VCO specified in
 * pm8x50_gen2_an_ability_table; VCO should be portmodVCOInvalid if the ability is invalid.
 * this function is temporarily masked out because there is no caller. advert_ability_get() will
 * need to call this function to verify the VCO requirement
 */
STATIC
int _pm8x50_gen2_an_ability_table_vco_get(uint32 speed, uint32 num_lanes, portmod_fec_t fec_type,
                                          portmod_port_phy_control_autoneg_mode_t an_mode,
                                          portmod_vco_type_t* vco)
{
    int i;

    *vco = portmodVCOInvalid;
    for (i = 0; i < PM8x50_GEN2_AN_ABILITY_TABLE_SIZE; i++) {
        if (pm8x50_gen2_an_ability_table[i].speed == speed &&
            pm8x50_gen2_an_ability_table[i].num_lanes == num_lanes &&
            pm8x50_gen2_an_ability_table[i].fec_type == fec_type &&
            pm8x50_gen2_an_ability_table[i].an_mode == an_mode)
        {
            *vco = pm8x50_gen2_an_ability_table[i].vco;
            break;
        }
    }

    return SOC_E_NONE;
}

/* read an entry of pm8x50_fs_ability_table[] */
STATIC
int _pm8x50_fs_ability_table_read_entry(int index, pm8x50_fs_ability_table_entry_t *table_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (index < 0 || index >= PM8x50_FS_ABILITY_TABLE_SIZE) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("index to read in pm8x50_fs_ability_table is out of boundary"));
    }

    *table_entry = pm8x50_fs_ability_table[index];

exit:
    SOC_FUNC_RETURN;
}

/* read an entry of pm8x50_gen2_fs_ability_table[] */
STATIC
int _pm8x50_gen2_fs_ability_table_read_entry(int index, pm8x50_fs_ability_table_entry_t *table_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (index < 0 || index >= PM8x50_GEN2_FS_ABILITY_TABLE_SIZE) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("index to read in pm8x50_gen2_fs_ability_table is out of boundary"));
    }

    *table_entry = pm8x50_gen2_fs_ability_table[index];

exit:
    SOC_FUNC_RETURN;
}

/* read an entry of pm8x50_an_ability_table[] */
STATIC
int _pm8x50_an_ability_table_read_entry(int index, pm8x50_an_ability_table_entry_t *table_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (index < 0 || index >= PM8x50_AN_ABILITY_TABLE_SIZE) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("index to read in pm8x50_an_ability_table is out of boundary"));
    }

    *table_entry = pm8x50_an_ability_table[index];

exit:
    SOC_FUNC_RETURN;
}

/* read an entry of pm8x50_gen2_an_ability_table[] */
STATIC
int _pm8x50_gen2_an_ability_table_read_entry(int index, pm8x50_an_ability_table_entry_t *table_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (index < 0 || index >= PM8x50_GEN2_AN_ABILITY_TABLE_SIZE) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("index to read in pm8x50_gen2_an_ability_table is out of boundary"));
    }

    *table_entry = pm8x50_gen2_an_ability_table[index];

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm8x50_port_index_get(int unit, int port, pm_info_t pm_info,
                           int *first_index, uint32 *bitmap)
{
   int i, rv = 0, tmp_port = 0;

   SOC_INIT_FUNC_DEFS;

   *first_index = -1;
   *bitmap = 0;

   for( i = 0 ; i < MAX_PORTS_PER_PM8X50; i++){
       rv = PM8x50_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
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
_pm8x50_phy_access_get(
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
    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));
    if (is_bypassed) {
        *phy_acc = (PM_8x50_INFO(pm_info)->first_phy + port_index) | SOC_REG_ADDR_PHY_ACC_MASK;
    } else {
        *phy_acc = port;
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm8x50_port_interrupt_all_enable_set(int unit, int port, int enable)
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
int _pm8x50_pm_port_mode_update(int unit,
                                int port,
                                pm_info_t pm_info,
                                int first_phy_index,
                                int num_lanes)
{
    uint32 reg_val, mac_port_mode, mac_new_port_mode = 0, is_400g_port = 0;
    int phy_acc, first_lane_local = 0, mac_stage_id = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_phy_access_get(unit, port, pm_info, &phy_acc));

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
int _pm8x50_400g_an_workaround(int unit, int port, pm_info_t pm_info, int* is_done)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    uint32_t serdes_reg_val, wait_link_count, base_reg_val, remote_pause;
    int nof_phys;
    pm8x50_port_400g_an_state_t an_state;
    phymod_autoneg_control_t an_control;
    char* state_string = "\n";
    int port_index;
    uint32 bitmap;

    SOC_INIT_FUNC_DEFS;

    *is_done = 0;

    /* first get the an state and wait link count */
    _SOC_IF_ERR_EXIT(PM8x50_400G_AN_STATE_GET(unit, pm_info, &an_state));
    _SOC_IF_ERR_EXIT(PM8x50_400G_AN_WAIT_LINK_COUNT_GET(unit, pm_info, &wait_link_count));

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    /* based on the AN state */
    switch (an_state) {
        case PM8X50_PORT_AN_CHECK_PAGE_COMPLETE:
        {
            state_string = "AN_CHECK_PAGE_COMPLETE\n";
            /* first read SW autoneg status register 0xc1e6 */
            /* clear the status first*/
            _SOC_IF_ERR_EXIT(phymod_phy_reg_read(&phy_access, AN_X4_SW_CTRL_STS_REG, &serdes_reg_val));
            sal_usleep(100);
            _SOC_IF_ERR_EXIT(phymod_phy_reg_read(&phy_access, AN_X4_SW_CTRL_STS_REG, &serdes_reg_val));

            if (serdes_reg_val & AN_X4_SW_CTRL_STS_AN_COMPLETE_MASK) {
                int phy_acc;
                uint32 reg_val;

                /* get port index */
                _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, &bitmap));
                _SOC_IF_ERR_EXIT(phymod_phy_reg_read(&phy_access, AN_X4_LP_BASE1r_REG, &base_reg_val));
                /* store remote pause value */
                remote_pause = (base_reg_val >> AN_LP_BASE1r_PAGE_PAUSE_OFFSET) & AN_LP_BASE1r_PAGE_PAUSE_MASK;
                _SOC_IF_ERR_EXIT(PM8x50_400G_AN_REMOTE_PAUSE_SET(unit, pm_info, remote_pause, port_index));

                /* need to move one to the next stage */
                /* disable AN_Cl73 and AN_CL73BAM */
                _SOC_IF_ERR_EXIT(phymod_phy_reg_read(&phy_access, AN_X4_CL73_CFG_REG, &serdes_reg_val));
                serdes_reg_val &= AN_X4_CL73_CFG_AN_DISABLE_MASK;
                _SOC_IF_ERR_EXIT(phymod_phy_reg_write(&phy_access, AN_X4_CL73_CFG_REG, serdes_reg_val));

                /* next disable rx/tx and set discard */
                _SOC_IF_ERR_EXIT(cdmac_rx_enable_set(unit, port, 0));
                _SOC_IF_ERR_EXIT(cdmac_discard_set(unit, port, 1));
                _SOC_IF_ERR_EXIT(cdmac_soft_reset_set(unit, port, 1));
                _SOC_IF_ERR_EXIT(cdmac_tx_enable_set(unit, port, 0));

                /* next update the cdport mode register */
                _SOC_IF_ERR_EXIT(_pm8x50_phy_access_get(unit, port, pm_info, &phy_acc));
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
                an_state = PM8X50_PORT_AN_CHECK_PCS_LINKUP;
            }
            break;
       }
       case PM8X50_PORT_AN_CHECK_PCS_LINKUP:
       {
            int link, phy_acc, remote_fault;
            uint32 rval;

            state_string = "AN_CHECK_PCS_LINKUP\n";
            /* first check link status */
            _SOC_IF_ERR_EXIT(phymod_phy_link_status_get(&phy_access, (uint32_t *)&link));
            _SOC_IF_ERR_EXIT(phymod_phy_link_status_get(&phy_access, (uint32_t *)&link));

            /* next check remote fualt */
            _SOC_IF_ERR_EXIT(cdmac_remote_fault_status_get(unit, port, &remote_fault));
            _SOC_IF_ERR_EXIT(cdmac_remote_fault_status_get(unit, port, &remote_fault));

            if (link && (!remote_fault)) {
                /*re-enable tx/rx and disable DISCARD */
                _SOC_IF_ERR_EXIT(cdmac_discard_set(unit, port, 0));

                /* next re-enable CDMAC TX/RX */
                _SOC_IF_ERR_EXIT(_pm8x50_phy_access_get(unit, port, pm_info, &phy_acc));
                _SOC_IF_ERR_EXIT(READ_CDMAC_CTRLr(unit, phy_acc, &rval));
                soc_reg_field_set(unit, CDMAC_CTRLr, &rval, TX_ENf, TRUE);
                soc_reg_field_set(unit, CDMAC_CTRLr, &rval, RX_ENf, TRUE);
                soc_reg_field_set(unit, CDMAC_CTRLr, &rval, SW_LINK_STATUSf, TRUE);
                _SOC_IF_ERR_EXIT(WRITE_CDMAC_CTRLr(unit, phy_acc, rval));
                wait_link_count = 0;
                an_state = PM8X50_PORT_AN_CREDIT_RESET;
                _SOC_IF_ERR_EXIT(cdmac_soft_reset_set(unit, port, 0));
            } else {
                wait_link_count++;
                /* next check if AN needs to restarted */
                if (wait_link_count > TIMEOUT_COUNT_400G_AN) {
                    an_state = PM8X50_PORT_AN_DISABLE;
                    wait_link_count = 0;
                }
            }

            break;
       }
       case PM8X50_PORT_AN_DISABLE:
       {
            state_string = "AN_DISABLE \n";
            /* first needs to disable */
            an_control.enable = 0;
            an_control.an_mode = phymod_AN_MODE_CL73_MSA;
            an_control.num_lane_adv = 8;
            _SOC_IF_ERR_EXIT(phymod_phy_autoneg_set(&phy_access, &an_control));
            an_state = PM8X50_PORT_AN_RESTART;
            break;
       }
       case PM8X50_PORT_AN_RESTART:
       {
            state_string = "AN_RESTART \n";
            _SOC_IF_ERR_EXIT(_pm8x50_pm_port_mode_update(unit, port, pm_info, 0, 1));

            /* needs to enable autoneg */
            an_control.enable = 1;
            an_control.an_mode = phymod_AN_MODE_CL73_MSA;
            an_control.num_lane_adv = 8;
            _SOC_IF_ERR_EXIT(phymod_phy_autoneg_set(&phy_access, &an_control));
            an_state = PM8X50_PORT_AN_CHECK_PAGE_COMPLETE;
            break;
       }

       case PM8X50_PORT_AN_CREDIT_RESET:
       {
            int link, remote_fault;
            state_string = "AN_CREDIT_RESET \n";

            /* first check if link is up, if not, need to re-start AN */
            _SOC_IF_ERR_EXIT(phymod_phy_link_status_get(&phy_access, (uint32_t *)&link));
            _SOC_IF_ERR_EXIT(phymod_phy_link_status_get(&phy_access, (uint32_t *)&link));

            /* next check remote fualt */
            _SOC_IF_ERR_EXIT(cdmac_remote_fault_status_get(unit, port, &remote_fault));
            _SOC_IF_ERR_EXIT(cdmac_remote_fault_status_get(unit, port, &remote_fault));

            if (link && (!remote_fault)) {
                /* first assert MAC soft reset */
                 _SOC_IF_ERR_EXIT(cdmac_soft_reset_set(unit, port, 1));
                /* Callback egress buffer reset function */
                if (PM_8x50_INFO(pm_info)->portmod_egress_buffer_reset) {
                     _SOC_IF_ERR_EXIT(PM_8x50_INFO(pm_info)->portmod_egress_buffer_reset(unit, port));
                } else {
                    _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                              (_SOC_MSG("The callback function is not defined \n")));
                }

                /* de-assert CDMAC soft reset */
                _SOC_IF_ERR_EXIT(cdmac_soft_reset_set(unit, port, 0));

                an_state = PM8X50_PORT_AN_COMPLETE;
            } else {
                /*if no link or remote fault, re-start AN */
                an_state = PM8X50_PORT_AN_DISABLE;
                wait_link_count = 0;
            }
            break;
       }
       case PM8X50_PORT_AN_COMPLETE:
       {
            int link, remote_fault;
            state_string = "AN_COMPLETE \n";

            /* first check link status */
            _SOC_IF_ERR_EXIT(phymod_phy_link_status_get(&phy_access, (uint32_t *)&link));
            _SOC_IF_ERR_EXIT(phymod_phy_link_status_get(&phy_access, (uint32_t *) &link));

            /* next check remote fault */
            _SOC_IF_ERR_EXIT(cdmac_remote_fault_status_get(unit, port, &remote_fault));
            _SOC_IF_ERR_EXIT(cdmac_remote_fault_status_get(unit, port, &remote_fault));

            /* only if link up and no remote fault, declare AN complete */
            if (link && (!remote_fault)) {
                state_string = "AN_COMPLETE \n";
                *is_done = 1;
            } else {
                an_state = PM8X50_PORT_AN_DISABLE;
                wait_link_count = 0;
                *is_done = 0;
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
    _SOC_IF_ERR_EXIT(PM8x50_400G_AN_STATE_SET(unit, pm_info, an_state));
    _SOC_IF_ERR_EXIT(PM8x50_400G_AN_WAIT_LINK_COUNT_SET(unit, pm_info, wait_link_count)); 

exit:
    SOC_FUNC_RETURN;
}

STATIC
void _pm8x50_signalling_method_get(int is_bypassed,
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

/* Convert the given VCO value to bitmap
 *
 * Inputs:
 *     unit:              unit number;
 *     vco:               vco value;
 *     is_pcs_bypass:     is port in PCS bypass mode;
 *     required_vco_bmp:  vco bitmap;
 *
 * Output:
 *     non_bypassed_ports:   Non-bypass ports bitmap
 */

STATIC
int _pm8x50_required_vco_bitmap_get(
    int unit,
    portmod_vco_type_t vco,
    int is_pcs_bypass,
    pm8x50_vcos_bmp_t *required_vco_bmp)
{
    SOC_INIT_FUNC_DEFS;

    *required_vco_bmp = 0;
    if (vco == portmodVCO20P625G && is_pcs_bypass) {
        PM8x50_VCO_ILKN_20P625G_SET(*required_vco_bmp);
    } else if (vco == portmodVCO20P625G && !is_pcs_bypass) {
        PM8x50_VCO_ETH_20P625G_SET(*required_vco_bmp);
    } else if (vco == portmodVCO25G && is_pcs_bypass) {
        PM8x50_VCO_ILKN_25G_SET(*required_vco_bmp);
    } else if (vco == portmodVCO25P781G && !is_pcs_bypass) {
        PM8x50_VCO_ETH_25P781G_SET(*required_vco_bmp);
    } else if (vco == portmodVCO25P781G && is_pcs_bypass) {
        PM8x50_VCO_ILKN_25P781G_SET(*required_vco_bmp);
    } else if (vco == portmodVCO26P562G && !is_pcs_bypass) {
        PM8x50_VCO_ETH_26P562G_SET(*required_vco_bmp);
    } else if (vco == portmodVCO26P562G && is_pcs_bypass) {
        PM8x50_VCO_ILKN_26P562G_SET(*required_vco_bmp);
    } else if (vco == portmodVCO27P343G && is_pcs_bypass) {
        PM8x50_VCO_ILKN_27P343G_SET(*required_vco_bmp);
    } else if (vco == portmodVCO28P125G && is_pcs_bypass) {
        PM8x50_VCO_ILKN_28P125G_SET(*required_vco_bmp);
    } else if (vco == portmodVCO25G && !is_pcs_bypass) {
        PM8x50_VCO_ETH_25G_SET(*required_vco_bmp);
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("Unsupport VCO value! \n")));
    }
exit:
    SOC_FUNC_RETURN;
}

/* Iterate over VCO values from VCO bitmap:
 *  - return the VCO value of highest rate.
 *  - clear returned VCO rate.
 *
 * Inputs:
 *     unit:           unit number;
 *     required_vcos:  VCO bitmap;
 *     vco:            VCO rate;
 *     is_pcs_bypass:  is in PCS bypass mode
 * Output:
 *     non_bypassed_ports:   Non-bypass ports bitmap
 */
STATIC
int _pm8x50_required_vco_value_iter(
        int unit,
        pm8x50_vcos_bmp_t *required_vcos,
        portmod_vco_type_t *vco,
        int *is_pcs_bypass)
{
    SOC_INIT_FUNC_DEFS;

    if (PM8x50_VCO_ILKN_28P125G_GET(*required_vcos)) {
        *vco = portmodVCO28P125G;
        *is_pcs_bypass = 1;
        PM8x50_VCO_ILKN_28P125G_CLR(*required_vcos);
    } else if (PM8x50_VCO_ILKN_27P343G_GET(*required_vcos)) {
        *vco = portmodVCO27P343G;
        *is_pcs_bypass = 1;
        PM8x50_VCO_ILKN_27P343G_CLR(*required_vcos);
    } else if (PM8x50_VCO_ETH_26P562G_GET(*required_vcos)) {
        *vco = portmodVCO26P562G;
        *is_pcs_bypass = 0;
        PM8x50_VCO_ETH_26P562G_CLR(*required_vcos);
    } else if (PM8x50_VCO_ILKN_26P562G_GET(*required_vcos)) {
        *vco = portmodVCO26P562G;
        *is_pcs_bypass = 1;
        PM8x50_VCO_ILKN_26P562G_CLR(*required_vcos);
    } else if (PM8x50_VCO_ETH_25P781G_GET(*required_vcos)) {
        *vco = portmodVCO25P781G;
        *is_pcs_bypass = 0;
        PM8x50_VCO_ETH_25P781G_CLR(*required_vcos);
    } else if (PM8x50_VCO_ILKN_25P781G_GET(*required_vcos)) {
        *vco = portmodVCO25P781G;
        *is_pcs_bypass = 1;
        PM8x50_VCO_ILKN_25P781G_CLR(*required_vcos);
    } else if (PM8x50_VCO_ILKN_25G_GET(*required_vcos)) {
        *vco = portmodVCO25G;
        *is_pcs_bypass = 1;
        PM8x50_VCO_ILKN_25G_CLR(*required_vcos);
    } else if (PM8x50_VCO_ETH_20P625G_GET(*required_vcos)) {
        *vco = portmodVCO20P625G;
        *is_pcs_bypass = 0;
        PM8x50_VCO_ETH_20P625G_CLR(*required_vcos);
    } else if (PM8x50_VCO_ILKN_20P625G_GET(*required_vcos)) {
        *vco = portmodVCO20P625G;
        *is_pcs_bypass = 1;
        PM8x50_VCO_ILKN_20P625G_CLR(*required_vcos);
    } else if (PM8x50_VCO_ETH_25G_GET(*required_vcos)) {
        /*
         * Ethernet 25G need to be served after Ethernet 20.625G.
         * Because TVCO can never run at 25G, while 20.625G could be
         * TVCO if requested by user.
         */
        *vco = portmodVCO25G;
        *is_pcs_bypass = 0;
        PM8x50_VCO_ETH_25G_CLR(*required_vcos);
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("Unsupport VCO value! \n")));
    }

exit:
    SOC_FUNC_RETURN;
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
int _pm8x50_pm_active_eth_lanes_get(int unit,
                           pm_info_t pm_info,
                           uint8_t * eth_active_lanes)
{
    int tmp_port, i, is_pcs_bypassed = 0;
    uint8_t ovco_pll_active_lane_bitmap, tvco_pll_active_lane_bitmap;
    uint8_t ovco_pll_adv_lane_bitmap, tvco_pll_adv_lane_bitmap;
    uint8_t all_active_lane_bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(
        PM8x50_OVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &ovco_pll_active_lane_bitmap));
    _SOC_IF_ERR_EXIT(
        PM8x50_TVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_active_lane_bitmap));
    _SOC_IF_ERR_EXIT(
        PM8x50_OVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &ovco_pll_adv_lane_bitmap));
    _SOC_IF_ERR_EXIT(
        PM8x50_TVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_adv_lane_bitmap));

    all_active_lane_bitmap = (ovco_pll_active_lane_bitmap | tvco_pll_active_lane_bitmap |
                              ovco_pll_adv_lane_bitmap | tvco_pll_adv_lane_bitmap);

    *eth_active_lanes = 0;
    for(i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
        /* Exclude the ports whose speeds are not configured */
        if (all_active_lane_bitmap & (1 << i)) {
            _SOC_IF_ERR_EXIT(PM8x50_LANE2PORT_GET(unit, pm_info, i, &tmp_port));
            if (tmp_port >= 0) {
                _SOC_IF_ERR_EXIT(PM8x50_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_pcs_bypassed, i));
                if (!is_pcs_bypassed) {
                    *eth_active_lanes |= (1 << i);
                }
            }
        }
    }
exit:
    SOC_FUNC_RETURN;
}


/*
 * Check if the VCO rate served as TVCO rate can drive Ethernet ports.
 *
 * This function will return 1 if the input VCO can drvie
 * Ethernet ports as TVCO rate. Otherwise, return 0.
 *
 * Inputs:
 *   unit    -  Unit number.
 *   flags   -  Flags to decide Ethernet driving TVCO rate. (PM8X50_TVCO_SUPPORT_20P625G)
 *   vco     -  VCO rate to check.
 */
STATIC
int _pm8x50_ethernet_tvco_check(int unit,
                                uint32 flags,
                                portmod_vco_type_t vco)
{
    if (PM8X50_TVCO_SUPPORT_20P625G_GET(flags)) {
        return ((vco == portmodVCO25P781G) ||
                (vco == portmodVCO26P562G) ||
                (vco == portmodVCO20P625G));
    } else {
        return ((vco == portmodVCO25P781G) || (vco == portmodVCO26P562G));
    }

}

/*Get whether the inerface type is supported by PM */
int pm8x50_pm_interface_type_is_supported(int unit,
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
pm8x50_default_fw_loader(const phymod_core_access_t* core, uint32_t length, const uint8_t* data)
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
                              &(pm8x50_ucode_buf[unit]),
                              &(pm8x50_ucode_buf_2nd[unit]),
                              CDPORT_TSC_UCMEM_DATAm,
                              CDPORT_TSC_MEM_CTRLr);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}


STATIC
int
pm8x50_default_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr,
                         uint32_t val)
{
    /* will force the core_addr to zero since it's  NOT being used */
    return portmod_common_phy_sbus_reg_write(CDPORT_TSC_UCMEM_DATAm, user_acc,
                                             0x0, reg_addr, val);
}

STATIC
int
pm8x50_default_mem_write(void* user_acc,
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
    case phymodMemTxLkup1588400G:
        tsc_mem_data = TX_LKUP_1588_MEM_400Gm;
        break;
    case phymodMemRxLkup1588Mpp0:
        tsc_mem_data = RX_LKUP_1588_MEM_MPP0m;
        break;
    case phymodMemRxLkup1588Mpp1:
        tsc_mem_data = RX_LKUP_1588_MEM_MPP1m;
        break;
    case phymodMemRxLkup1588400G:
        tsc_mem_data = RX_LKUP_1588_MEM_400Gm;
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
pm8x50_default_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr,
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
pm8x50_default_mem_read(void* user_acc,
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
    case phymodMemTxLkup1588400G:
        tsc_mem_data = TX_LKUP_1588_MEM_400Gm;
        break;
    case phymodMemRxLkup1588Mpp0:
        tsc_mem_data = RX_LKUP_1588_MEM_MPP0m;
        break;
    case phymodMemRxLkup1588Mpp1:
        tsc_mem_data = RX_LKUP_1588_MEM_MPP1m;
        break;
    case phymodMemRxLkup1588400G:
        tsc_mem_data = RX_LKUP_1588_MEM_400Gm;
        break;
    case phymodMemSpeedPriorityMapTable:
        tsc_mem_data = SPEED_PRIORITY_MAP_TBLm;
        break;
    default:
        return SOC_E_PARAM;
    }

    rv = soc_mem_read(user_data->unit, tsc_mem_data, user_data->blk_id, mem_index, val);

    return rv;
}


/*
 * Function:
 *      portmod_pm8x50_wb_upgrade_func
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

int portmod_pm8x50_wb_upgrade_func(int unit, void *arg, int recovered_version,
                                   int new_version)
{
    pm_info_t pm_info;
    int port_is_pcs_bypased, core_is_flexe_configured, port_flexe_mode;
    int i, ts_enable_port_count;
    uint32 fec_null, timesync_enable;
    int is_ber_war_enable, ber_war_state, ber_war_wait_count, remote_pause;
    int is_3_cw_bad_enable, three_cw_bad_state, three_cw_bad_tick_cnt;
    int is_flexe_tvco_26g_required;

    SOC_INIT_FUNC_DEFS;

    pm_info = arg;

    if ((recovered_version <= 1) &&
        (new_version >= 2)) {
        ts_enable_port_count = 0;
        fec_null = 0;
        timesync_enable = 0;
        _SOC_IF_ERR_EXIT(
            PM8x50_TS_ENABLE_PORT_COUNT_SET(unit, pm_info, ts_enable_port_count));

        for (i = 0; i < MAX_PORTS_PER_PM8X50; i++) {
            _SOC_IF_ERR_EXIT(
                PM8x50_LANE2FEC_SET(unit, pm_info, i, fec_null));
            _SOC_IF_ERR_EXIT(
                PM8x50_TIMESYNC_ENABLE_SET(unit, pm_info, timesync_enable, i));
        }
    }
    if ((recovered_version <= 2) &&
        (new_version >= 3)) {
        port_is_pcs_bypased = 0;

        for (i = 0; i < MAX_PORTS_PER_PM8X50; i++) {
            _SOC_IF_ERR_EXIT(
                PM8x50_PORT_IS_PCS_BYPASSED_SET(unit, pm_info, port_is_pcs_bypased, i));
        }
    }
    if ((recovered_version <= 3) &&
        (new_version >= 4)) {
        is_ber_war_enable = 0;
        ber_war_state = PM8X50_PORT_BER_WAR_STATE_COUNT;
        ber_war_wait_count = 0;
        remote_pause = 0;
        for (i = 0; i < MAX_PORTS_PER_PM8X50; i++) {
            _SOC_IF_ERR_EXIT(
                PM8x50_BER_WAR_ENABLE_SET(unit, pm_info, is_ber_war_enable, i));
            _SOC_IF_ERR_EXIT(
                PM8x50_BER_WAR_STATE_SET(unit, pm_info, ber_war_state, i));
            _SOC_IF_ERR_EXIT(
                PM8x50_BER_WAR_WAIT_COUNT_SET(unit, pm_info, ber_war_wait_count, i));
            _SOC_IF_ERR_EXIT(
                PM8x50_400G_AN_REMOTE_PAUSE_SET(unit, pm_info, remote_pause, i));
        }
    }

    if ((recovered_version <= 3) &&
        (new_version >= 4)) {
        core_is_flexe_configured = 0;
        _SOC_IF_ERR_EXIT(
            PM8x50_IS_CORE_FLEXE_CONFIGURED_SET(unit, pm_info, core_is_flexe_configured));
    }

    if ((recovered_version <= 4) &&
        (new_version >= 5)) {
        is_3_cw_bad_enable = 0;
        three_cw_bad_state = PM8X50_PORT_3_CW_BAD_STATE_COUNT;
        three_cw_bad_tick_cnt = 0;
        for (i = 0; i < MAX_PORTS_PER_PM8X50; i++) {
            _SOC_IF_ERR_EXIT(
                PM8x50_3_CW_BAD_ENABLE_SET(unit, pm_info, is_3_cw_bad_enable, i));
            _SOC_IF_ERR_EXIT(
                PM8x50_3_CW_BAD_STATE_SET(unit, pm_info, three_cw_bad_state, i));
            _SOC_IF_ERR_EXIT(
                PM8x50_3_CW_BAD_TICK_CNT_SET(unit, pm_info, three_cw_bad_tick_cnt, i));
        }
    }

    if ((recovered_version <= 5) &&
        (new_version >= 6)) {
        is_flexe_tvco_26g_required = 0;
        _SOC_IF_ERR_EXIT(
            PM8x50_IS_FLEXE_TVCO_26G_REQUIRED_SET(unit, pm_info, is_flexe_tvco_26g_required));
        port_flexe_mode = 0;
        for (i = 0; i < MAX_PORTS_PER_PM8X50; i++) {
            _SOC_IF_ERR_EXIT(
                PM8x50_PORT_FLEXE_MODE_SET(unit, pm_info, port_flexe_mode, i));
        }
    }
exit:
    SOC_FUNC_RETURN;
}

int pm8x50_pm_wb_debug_log (int unit, pm_info_t pm_info)
{
    int i, rv = 0;
    int an_state_400g;
    int tmp_port, max_speed;
    int ts_enable_port_count, is_core_flexe_configured = 0;
    int is_400g_an, is_ber_war_enable;
    int ber_war_wait_count, ber_war_state;
    int rlm_enabled, rlm_state;
    int an_400g_remote_pause;
    int is_3_cw_bad_enable;
    uint8_t ovco_pll_active_lane_bm, tvco_pll_active_lane_bm;
    uint8_t ovco_pll_adv_lane_bm, tvco_pll_adv_lane_bm;
    uint32 tx_lane_map, rx_lane_map;
    uint32 is_core_initialized, is_active, is_bypassed;
    uint32 is_initiator, pcs_reconfig;
    uint32 an_wait_link_count, timesync_enable;
    uint32 active_lane_map;
    uint32 three_cw_bad_tick_cnt;
    phymod_an_mode_type_t an_mode = 0;
    portmod_fec_t fec;
    phymod_fec_type_t orig_fec;
    pm8x50_port_3_cw_bad_state_t three_cw_bad_state;
    int is_flexe_tvco_26g_required = 0;
    int port_flexe_mode = 0;


    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50:{wb_buffer_index}\n")));
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "{%d}\n"), pm_info->wb_buffer_id));
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50:{wb_buffer_index}\n")));

    rv = PM8x50_IS_CORE_INITIALIZED_GET(unit, pm_info, &is_core_initialized);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x50:{is_initialized}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), is_core_initialized));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x50:{is_initialized}\n")));
    }

    rv = PM8x50_IS_ACTIVE_GET(unit, pm_info, &is_active);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x50:{is_active}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), is_active));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x50:{is_active}\n")));
    }

    rv = PM8x50_OVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &ovco_pll_active_lane_bm);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x50:{ovco_pll_active_lane_bitmap}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{0x%x}\n"), ovco_pll_active_lane_bm));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x50:{ovco_pll_active_lane_bitmap}\n")));
    }

    rv = PM8x50_TVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_active_lane_bm);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x50:{tvco_pll_active_lane_bitmap}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{0x%x}\n"), tvco_pll_active_lane_bm));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x50:{tvco_pll_active_lane_bitmap}\n")));
    }

    rv = PM8x50_OVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &ovco_pll_adv_lane_bm);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x50:{ovco_pll_adv_lane_bitmap}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{0x%x}\n"), ovco_pll_adv_lane_bm));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x50:{ovco_pll_adv_lane_bitmap}\n")));
    }

    rv = PM8x50_TVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_adv_lane_bm);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x50:{tvco_pll_adv_lane_bitmap}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{0x%x}\n"), tvco_pll_adv_lane_bm));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x50:{tvco_pll_adv_lane_bitmap}\n")));
    }

    rv = PM8x50_TS_ENABLE_PORT_COUNT_GET(unit, pm_info, &ts_enable_port_count);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x50:{ts_enable_port_count}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), ts_enable_port_count));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x50:{ts_enable_port_count}\n")));
    }

    rv = PM8x50_ORIG_TX_LANE_MAP_GET(unit, pm_info, &tx_lane_map);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x50:{tx_lane_map}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{0x%x}\n"), tx_lane_map));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x50:{tx_lane_map}\n")));
    }

    rv = PM8x50_ORIG_RX_LANE_MAP_GET(unit, pm_info, &rx_lane_map);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x50:{rx_lane_map}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{0x%x}\n"), rx_lane_map));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x50:{rx_lane_map}\n")));
    }

    rv = PM8x50_400G_AN_STATE_GET(unit, pm_info, &an_state_400g);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x50:{an_state_400g}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), an_state_400g));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x50:{an_state_400g}\n")));
    }

    rv = PM8x50_400G_AN_WAIT_LINK_COUNT_GET(unit, pm_info, &an_wait_link_count);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x50:{an_wait_link_count}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), an_wait_link_count));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x50:{an_wait_link_count}\n")));
    }

    rv = PM8x50_IS_CORE_FLEXE_CONFIGURED_GET(unit, pm_info, &is_core_flexe_configured);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x50:{is_core_flexe_configured}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), is_core_flexe_configured));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x50:{is_core_flexe_configured}\n")));
    }

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50:{lane,lane2port_map}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {

        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit, "\n")));
        rv = PM8x50_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, tmp_port));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50:{lane,lane2port_map}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50:{port,max_speed}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
        rv = PM8x50_MAX_SPEED_GET(unit, pm_info, &max_speed, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, max_speed));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50:{port,max_speed}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50:{port,an_mode}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
        rv = PM8x50_AN_MODE_GET(unit, pm_info, &an_mode, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, an_mode));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50:{port,an_mode}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50:{lane,lane2fec_map}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
        rv = PM8x50_LANE2FEC_GET(unit, pm_info, i, &fec);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, fec));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50:{lane,lane2fec_map}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50:{port,port_is_pcs_bypassed}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
        rv = PM8x50_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, is_bypassed));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50:{port,port_is_pcs_bypassed}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50:{port,timesync_enable}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
        rv = PM8x50_TIMESYNC_ENABLE_GET(unit, pm_info, &timesync_enable, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, timesync_enable));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50:{port,timesync_enable}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50:{port,rlm_enabled}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
        rv = PM8x50_RLM_ENABLE_GET(unit, pm_info, &rlm_enabled, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, rlm_enabled));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50:{port,rlm_enabled}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50:{port,is_initiator}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
        rv = PM8x50_INITIATOR_GET(unit, pm_info, &is_initiator, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, is_initiator));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50:{port,is_initiator}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50:{port,pcs_reconfigured}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
        rv = PM8x50_PCS_RECONFIG_GET(unit, pm_info, &pcs_reconfig, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, pcs_reconfig));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50:{port,pcs_reconfigured}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50:{port,active_lane_map}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
        rv = PM8x50_ACTIVE_LANE_MAP_GET(unit, pm_info, &active_lane_map, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,0x%x}\n"), i, active_lane_map));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50:{port,active_lane_map}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50:{port,rlm_state}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
        rv = PM8x50_RLM_STATE_GET(unit, pm_info, &rlm_state, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, rlm_state));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50:{port,rlm_state}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50:{port,fec_type}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
        rv = PM8x50_ORIGINAL_FEC_GET(unit, pm_info, &orig_fec, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, orig_fec));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50:{port,fec_type}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50:{port,400g_an_enable}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
        rv = PM8x50_400G_AN_ENABLE_GET(unit, pm_info, &is_400g_an, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, is_400g_an));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50:{port,400g_an_enable}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50:{port,is_ber_war_enable}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
        rv = PM8x50_BER_WAR_ENABLE_GET(unit, pm_info, &is_ber_war_enable, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, is_ber_war_enable));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50:{port,is_ber_war_enable}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50:{port,ber_war_state}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
        rv = PM8x50_BER_WAR_STATE_GET(unit, pm_info, &ber_war_state, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, ber_war_state));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50:{port,ber_war_state}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50:{port,ber_war_wait_count}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
        rv = PM8x50_BER_WAR_WAIT_COUNT_GET(unit, pm_info, &ber_war_wait_count, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, ber_war_wait_count));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50:{port,ber_war_wait_count}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50:{port,an_400g_remote_pause}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
        rv = PM8x50_400G_AN_REMOTE_PAUSE_GET(unit, pm_info, &an_400g_remote_pause, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, an_400g_remote_pause));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50:{port,an_400g_remote_pause}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50:{port,is_three_codeword_bad_enable}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
        rv = PM8x50_3_CW_BAD_ENABLE_GET(unit, pm_info, &is_3_cw_bad_enable, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, is_3_cw_bad_enable));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50:{port,is_three_codeword_bad_enable}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50:{port,three_codeword_bad_state}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
        rv = PM8x50_3_CW_BAD_STATE_GET(unit, pm_info, &three_cw_bad_state, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, three_cw_bad_state));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50:{port,three_codeword_bad_state}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50:{port,three_codeword_bad_tick_cnt}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
        rv = PM8x50_3_CW_BAD_TICK_CNT_GET(unit, pm_info, &three_cw_bad_tick_cnt, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, three_cw_bad_tick_cnt));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50:{port,three_codeword_bad_tick_cnt}\n")));

    rv = PM8x50_IS_FLEXE_TVCO_26G_REQUIRED_GET(unit, pm_info, &is_flexe_tvco_26g_required);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm8x50:{is_flexe_tvco_26g_required}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), is_flexe_tvco_26g_required));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm8x50:{is_flexe_tvco_26g_required}\n")));
    }

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm8x50:{port,port_flexe_mode}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
        rv = PM8x50_PORT_FLEXE_MODE_GET(unit, pm_info, &port_flexe_mode, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, port_flexe_mode));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50:{port,is_ber_war_enable}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm8x50\n")));

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
int pm8x50_wb_buffer_init(int unit, int wb_buffer_index, pm_info_t pm_info)
{
    /* Declare the common variables needed for warmboot */
    WB_ENGINE_INIT_TABLES_DEFS;
    int wb_var_id, rv;
    int buffer_id = wb_buffer_index; /*required by SOC_WB_ENGINE_ADD_ Macros*/
    SOC_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(buffer_is_dynamic);

    SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PORTMOD, wb_buffer_index, "pm8x50",
                           portmod_pm8x50_wb_upgrade_func, pm_info,
                           PM8x50_WB_BUFFER_VERSION, 1,
                           SOC_WB_ENGINE_PRE_RELEASE);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id,
                         "is_core_initialized", wb_buffer_index, sizeof(int),
                          NULL, MAX_PORTS_PER_PM8X50, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isCoreInitialized] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_active",
                          wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isActive] = wb_var_id;

    /* "isBypassed" has been not supported from version 3 */
    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_REMOVED_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_bypassed", wb_buffer_index, sizeof(uint32),
                              NULL, 1, VERSION(1), VERSION(3));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isBypassed] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "ovco_pll_active_lane_bitmap",
                          wb_buffer_index, sizeof(uint8), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[ovcopllActiveLaneBitmap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "tvco_pll_active_lane_bitmap",
                          wb_buffer_index, sizeof(uint8), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[tvcopllActiveLaneBitmap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "ovco_pll_adv_lane_bitmap",
                          wb_buffer_index, sizeof(uint8), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[ovcopllAdvLaneBitmap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "tvco_pll_adv_lane_bitmap",
                          wb_buffer_index, sizeof(uint8), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[tvcopllAdvLaneBitmap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "lane2portMap",
                          wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_PM8X50, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[lane2portMap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "max_speed",
                          wb_buffer_index, sizeof(uint32), NULL, MAX_PORTS_PER_PM8X50, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[maxSpeed] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "an_mode",
                          wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_PM8X50, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[anMode] = wb_var_id;

    /* Deleting below 2 WB structures since they are not needed. */
    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_REMOVED_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "an_fec", wb_buffer_index, sizeof(uint32),
                              NULL, MAX_PORTS_PER_PM8X50, VERSION(1), VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[anFec] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_REMOVED_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "fs_cl72", wb_buffer_index, sizeof(uint32),
                              NULL, MAX_PORTS_PER_PM8X50, VERSION(1), VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[fsCl72] = wb_var_id;
    
    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "lane2fecMap",
                          wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_PM8X50, VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[lane2fecMap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));

    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "ts_enable_port_count",
                          wb_buffer_index, sizeof(int), NULL, VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[tsEnablePortCount] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "timesync_enable",
                           wb_buffer_index, sizeof(uint32), NULL, MAX_PORTS_PER_PM8X50, VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[timesyncEnable] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "rlm_enabled",
                           wb_buffer_index, sizeof(uint32), NULL, MAX_PORTS_PER_PM8X50, VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[rlmEnable] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_initiator",
                           wb_buffer_index, sizeof(uint32), NULL, MAX_PORTS_PER_PM8X50, VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[initiator] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "pcs_reconfigured",
                           wb_buffer_index, sizeof(uint32), NULL, MAX_PORTS_PER_PM8X50, VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[pcsReconfigured] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "active_lame_map",
                           wb_buffer_index, sizeof(uint32), NULL, MAX_PORTS_PER_PM8X50, VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[activeLaneBitMap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "rlm_state",
                           wb_buffer_index, sizeof(uint32), NULL, MAX_PORTS_PER_PM8X50, VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[rlmState] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "fec_type",
                           wb_buffer_index, sizeof(uint32), NULL, MAX_PORTS_PER_PM8X50, VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[originalFec] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "tx_lane_swap",
                          wb_buffer_index, sizeof(int), NULL, VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[originalTxLaneSwap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "rx_lane_swap",
                          wb_buffer_index, sizeof(int), NULL, VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[originalRxLaneSwap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "400g_an_enable",
                           wb_buffer_index, sizeof(uint32), NULL, MAX_PORTS_PER_PM8X50, VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[is400gAn] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "an_state_400g",
                          wb_buffer_index, sizeof(int), NULL, VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[anState400g] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "an_wait_link_count",
                          wb_buffer_index, sizeof(int), NULL, VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[anWaitlinkCount] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "port_is_pcs_bypassed",
                          wb_buffer_index, sizeof(uint32), NULL, MAX_PORTS_PER_PM8X50, VERSION(3));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[portIsPcsBypassed] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_ber_war_enable",
                           wb_buffer_index, sizeof(uint32), NULL, MAX_PORTS_PER_PM8X50, VERSION(4));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isBerWarEnable] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "ber_war_state",
                          wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_PM8X50, VERSION(4));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[berWarState] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "ber_war_wait_count",
                          wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_PM8X50, VERSION(4));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[berWarWaitCount] = wb_var_id;

    /* coreisFlexeConfigured is unused. need to be revisted later */
    /*_SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));*/
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id,
                         "is_core_flexe_configured", wb_buffer_index, sizeof(int),
                          NULL, MAX_PORTS_PER_PM8X50, VERSION(4));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[coreisFlexeConfigured] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "an_400g_remote_pause",
                          wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_PM8X50, VERSION(4));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[an400gRemotePause] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_three_codeword_bad_enable",
                           wb_buffer_index, sizeof(uint32), NULL, MAX_PORTS_PER_PM8X50, VERSION(5));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isThreeCodewordBad] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "three_codeword_bad_state",
                          wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_PM8X50, VERSION(5));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[threeCodewordBadState] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "three_codeword_bad_tick_cnt",
                          wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_PM8X50, VERSION(5));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[threeCodewordBadTickCnt] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id,
                         "is_flexe_tvco_26g_required", wb_buffer_index, sizeof(int),
                          NULL, MAX_PORTS_PER_PM8X50, VERSION(6));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isFlexeTvco26gRequired] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "port_flexe_mode",
                          wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_PM8X50, VERSION(6));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[portFlexeMode] = wb_var_id;

    _SOC_IF_ERR_EXIT(soc_wb_engine_init_buffer(unit, SOC_WB_ENGINE_PORTMOD,
                                               wb_buffer_index, FALSE));

exit:
    SOC_FUNC_RETURN;
}

phymod_bus_t pm8x50_default_bus = {
    "PM8x50 Bus",
    pm8x50_default_bus_read,
    pm8x50_default_bus_write,
    NULL,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    pm8x50_default_mem_read,
    pm8x50_default_mem_write,
    PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

STATIC
int _pm8x50_pll_to_vco_get(phymod_ref_clk_t ref_clock, uint32_t pll, portmod_vco_type_t* vco)
{
    int rv = SOC_E_NONE;

    if (ref_clock == phymodRefClk156Mhz) {
        switch (pll) {
            case phymod_TSCBH_PLL_DIV132:
                *vco = portmodVCO20P625G;
                break;
            case phymod_TSCBH_PLL_DIV165:
                *vco = portmodVCO25P781G;
                break;
            case phymod_TSCBH_PLL_DIV170:
                *vco = portmodVCO26P562G;
                break;
            case phymod_TSCBH_PLL_DIV160:
                *vco = portmodVCO25G;
                break;
            case phymod_TSCBH_PLL_DIV175:
                *vco = portmodVCO27P343G;
                break;
            case phymod_TSCBH_PLL_DIV180:
                *vco = portmodVCO28P125G;
                break;
            default:
                rv = SOC_E_PARAM;
        }
    } else if (ref_clock == phymodRefClk312Mhz) {
        switch (pll) {
            case phymod_TSCBH_PLL_DIV66:
                *vco = portmodVCO20P625G;
                break;
            case phymod_TSCBH_PLL_DIV82P5:
                *vco = portmodVCO25P781G;
                break;
            case phymod_TSCBH_PLL_DIV85:
                *vco = portmodVCO26P562G;
                break;
            case phymod_TSCBH_PLL_DIV80:
                *vco = portmodVCO25G;
                break;
            case phymod_TSCBH_PLL_DIV87P5:
                *vco = portmodVCO27P343G;
                break;
            case phymod_TSCBH_PLL_DIV90:
                *vco = portmodVCO28P125G;
                break;
            default:
                rv = SOC_E_PARAM;
        }
    } else {
        rv = SOC_E_PARAM;
    }

    return rv;
}

/* Translate phymod_fec type to portmod_fec_type */
STATIC
int _pm8x50_port_phy_to_port_fec(phymod_fec_type_t phymod_fec, portmod_fec_t* portmod_fec)
{
    int rv = SOC_E_NONE;
    if (phymod_fec == phymod_fec_None) {
        *portmod_fec = PORTMOD_PORT_PHY_FEC_NONE;
    } else if (phymod_fec == phymod_fec_CL74) {
        *portmod_fec = PORTMOD_PORT_PHY_FEC_BASE_R;
    } else if (phymod_fec == phymod_fec_CL91) {
        *portmod_fec = PORTMOD_PORT_PHY_FEC_RS_FEC;
    } else if (phymod_fec == phymod_fec_RS544) {
        *portmod_fec = PORTMOD_PORT_PHY_FEC_RS_544;
    } else if (phymod_fec == phymod_fec_RS272) {
        *portmod_fec = PORTMOD_PORT_PHY_FEC_RS_272;
    } else if (phymod_fec == phymod_fec_RS544_2XN) {
        *portmod_fec = PORTMOD_PORT_PHY_FEC_RS_544_2XN;
    } else if (phymod_fec == phymod_fec_RS272_2XN) {
        *portmod_fec  = PORTMOD_PORT_PHY_FEC_RS_272_2XN;
    } else {
        rv = SOC_E_PARAM;
    }
    return rv;
}



/* this function is to  manage the RLM state machine during PORT
transition into RLM mode 
*/

int pm8x50_port_rlm_state_update(int unit, int port, pm_info_t pm_info, int *is_done)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access, phy_copy;
    uint32  rval, is_initiator, pcs_is_reconfigured, active_lane_map, active_lane_num;
    phymod_fec_type_t FecType;
    uint32 txLaneSwap, rxLaneSwap, bitmap, prbs_active_lane_num = 0, prbs_active_lane_map = 0x0, num_lane = 0;
    portmod_port_rlm_state_t local_port_rlm_state;
    int nof_phys,  port_index = -1, mac_link = 0;
    uint32 prbs_flag = 0x0, i;
    phymod_prbs_t prbs_config;
    phymod_prbs_status_t prbs_status;
    char* state_string = " \n";


    SOC_INIT_FUNC_DEFS;

    *is_done = 0;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    sal_memcpy(&phy_copy, &phy_access, sizeof(phy_copy));

    /* need to know if 4 lane or 8 lane port and also start lane of this port */
    PORTMOD_BIT_COUNT(phy_access.access.lane_mask, num_lane);
    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, &bitmap));

    /* first get all the RLM warmboot variable */
    _SOC_IF_ERR_EXIT
        (PM8x50_INITIATOR_GET(unit, pm_info, &is_initiator, port_index));

    _SOC_IF_ERR_EXIT
        (PM8x50_PCS_RECONFIG_GET(unit, pm_info, &pcs_is_reconfigured, port_index));

    _SOC_IF_ERR_EXIT
        (PM8x50_ACTIVE_LANE_MAP_GET(unit, pm_info, &active_lane_map, port_index));

    _SOC_IF_ERR_EXIT
        (PM8x50_RLM_STATE_GET(unit, pm_info, &local_port_rlm_state.rlm_state, port_index));

    _SOC_IF_ERR_EXIT
        (PM8x50_ORIGINAL_FEC_GET(unit, pm_info, &FecType, port_index));

    _SOC_IF_ERR_EXIT
        (PM8x50_ORIG_TX_LANE_MAP_GET(unit, pm_info, &txLaneSwap));

    _SOC_IF_ERR_EXIT
        (PM8x50_ORIG_RX_LANE_MAP_GET(unit, pm_info, &rxLaneSwap));

    /* get the active lane number */
    active_lane_num = 0;
    for (i = 0; i < num_lane; i++) {
        if (active_lane_map &  1 << (port_index + i)) {
            active_lane_num++;
        }
    }

    /* first thing is to check MAC link status */
    if (port_index == 4) {
        _SOC_IF_ERR_EXIT(
            READ_CDPORT_XGXS0_LN4_STATUS_REGr(unit, port, &rval));
        mac_link = soc_reg_field_get(unit, CDPORT_XGXS0_LN4_STATUS_REGr, rval, LINK_STATUSf);

    } else {
        _SOC_IF_ERR_EXIT(
            READ_CDPORT_XGXS0_LN0_STATUS_REGr(unit, port, &rval));
        mac_link = soc_reg_field_get(unit, CDPORT_XGXS0_LN0_STATUS_REGr, rval, LINK_STATUSf);
    }

    /* if mac link status ever go low, directly go to error state */
     if (!mac_link)  {
        local_port_rlm_state.rlm_state = PORTMOD_PORT_RLM_ERROR;
    } else    {
        /* RLM state machine update */
        switch (local_port_rlm_state.rlm_state) {
        case PORTMOD_PORT_RLM_SEND_PACKET_DATA:
            state_string ="SEND_PACKET_DATA";
            /* first disable tx stall */
             _SOC_IF_ERR_EXIT
                (cdmac_tx_stall_enable_set(unit, port, 0));

            /* first check initiator  set or not */
            if (is_initiator) {
                /* update the state */
                local_port_rlm_state.rlm_state = PORTMOD_PORT_RLM_STOP_MAC;
            } else { /* follower */
                /* first if link interruption status */
                uint32  li_status = 0;
                _SOC_IF_ERR_EXIT
                    (cdmac_link_interruption_live_status_get(unit, port, &li_status));
                if (li_status) {
                    /* update the state */
                    local_port_rlm_state.rlm_state = PORTMOD_PORT_RLM_STOP_MAC;
                }
            }
            break;
        case PORTMOD_PORT_RLM_STOP_MAC:
            state_string ="STOP_MAC";
            /* clear the pcs reconfigured */
            pcs_is_reconfigured = FALSE;
            /* enable tx stall */
             _SOC_IF_ERR_EXIT
                (cdmac_tx_stall_enable_set(unit, port, 1));
            /* update the state to SEND_LI*/
            local_port_rlm_state.rlm_state = PORTMOD_PORT_RLM_SEND_LI;
            break;
        case PORTMOD_PORT_RLM_SEND_LI:
        {
            uint32 li_enable = 0, prbs_enabled = 0, li_status;
            int pcs_link = 0;

            state_string ="SEND_LI";

            /* first check if LI is enabled or not */
            _SOC_IF_ERR_EXIT
                (cdmac_link_interrupt_ordered_set_enable_get(unit, port, &li_enable));
            if (!li_enable) {
                _SOC_IF_ERR_EXIT
                    (cdmac_link_interrupt_ordered_set_enable(unit, port, 1));
            }
            /*next check if initiator */
            if (is_initiator) {
                /* if initiator, then check pcs reconfigured status */
                if (pcs_is_reconfigured) {
                    /*next need to check prbs status */
                    _SOC_IF_ERR_EXIT
                        (phymod_phy_prbs_enable_get(&phy_access, 0, &prbs_enabled));
                    if (prbs_enabled) {
                        /* disable prbs for both tx/rx */
                        _SOC_IF_ERR_EXIT
                            (phymod_phy_prbs_enable_set(&phy_access, 0, 0));
                        /* disable the tx/disable and lane dp reset override */
                        _SOC_IF_ERR_EXIT
                            (phymod_phy_pmd_override_enable_set(&phy_access, phymodPMDLaneReset, 0,  0));
                        _SOC_IF_ERR_EXIT
                            (phymod_phy_pmd_override_enable_set(&phy_access, phymodPMDTxDisable, 0,  0));
                    }
                    /* next poll the pcs link status and LI status */
                    _SOC_IF_ERR_EXIT
                        (cdmac_link_interruption_live_status_get(unit, port, &li_status));
                   /* read twice the pcs link status */
                    _SOC_IF_ERR_EXIT
                        (pm8x50_port_link_get(unit, port, pm_info, 0,  &pcs_link));
                    _SOC_IF_ERR_EXIT
                        (pm8x50_port_link_get(unit, port, pm_info, 0,  &pcs_link));
                    if (li_status && pcs_link) {
                        local_port_rlm_state.rlm_state = PORTMOD_PORT_RLM_SEND_IDLE;
                    }
                } else { /* pcs is not reconfigured */
                    /* need to check LI status */
                    _SOC_IF_ERR_EXIT
                        (cdmac_link_interruption_live_status_get(unit, port, &li_status));
                    if (li_status) {
                        local_port_rlm_state.rlm_state = PORTMOD_PORT_RLM_SEND_PRBS;
                    }
                }
            } else { /*non initiator */
                if (pcs_is_reconfigured) {
                    /*next need to check prbs status */
                    _SOC_IF_ERR_EXIT
                        (phymod_phy_prbs_enable_get(&phy_access, 0, &prbs_enabled));
                    if (prbs_enabled) {
                        /* disable prbs for both tx/rx */
                        _SOC_IF_ERR_EXIT
                            (phymod_phy_prbs_enable_set(&phy_access, 0, 0));
                        /* disable the tx/disable and lane dp reset override */
                        _SOC_IF_ERR_EXIT
                            (phymod_phy_pmd_override_enable_set(&phy_access, phymodPMDLaneReset, 0,  0));
                        _SOC_IF_ERR_EXIT
                            (phymod_phy_pmd_override_enable_set(&phy_access, phymodPMDTxDisable, 0,  0));
                    }
                    /* next poll the pcs link status and LI status */
                    _SOC_IF_ERR_EXIT
                        (cdmac_link_interruption_live_status_get(unit, port, &li_status));
                   /* read twice the pcs link status */
                    _SOC_IF_ERR_EXIT
                        (pm8x50_port_link_get(unit, port, pm_info, 0,  &pcs_link));
                    _SOC_IF_ERR_EXIT
                        (pm8x50_port_link_get(unit, port, pm_info, 0,  &pcs_link));
                    if (li_status && pcs_link) {
                        local_port_rlm_state.rlm_state = PORTMOD_PORT_RLM_SEND_IDLE;
                    }
                } else { /* pcs is not reconfigured yet */
                    uint32 prbs_active_lane_num = 0, prbs_active_lane_map = 0x0, pmd_lock = 0;
                    prbs_flag = 0x0;

                    /* first set RX prbs only  and check if rx prbs is enabled or not*/
                    PHYMOD_PRBS_DIRECTION_RX_SET(prbs_flag);
                    _SOC_IF_ERR_EXIT
                        (phymod_phy_prbs_enable_get(&phy_access, prbs_flag, &prbs_enabled));
                    if (!prbs_enabled) {
                        prbs_config.poly = phymodPrbsPoly13;
                        prbs_config.invert = 0;
                        _SOC_IF_ERR_EXIT
                            (phymod_phy_prbs_config_set(&phy_access, prbs_flag, &prbs_config));
                        _SOC_IF_ERR_EXIT
                            (phymod_phy_prbs_enable_set(&phy_access, prbs_flag, 1));
                    }

                    /*before check the prbs status, make sure signal detection are valid on all lanes*/
                    _SOC_IF_ERR_EXIT
                        (phymod_phy_rx_pmd_locked_get(&phy_access, &pmd_lock));
                    sal_usleep(100);
                    _SOC_IF_ERR_EXIT
                        (phymod_phy_rx_pmd_locked_get(&phy_access, &pmd_lock));

                    /* next check per lane prbs status only if PMD is locked */
                    if ((pmd_lock) && (!is_initiator)) {
                        /* next check per lane prbs status */
                        for (i = 0; i < num_lane; i ++) {
                            phy_copy.access.lane_mask = 1 << (i + port_index);
                            /*first read clear the status */
                            _SOC_IF_ERR_EXIT
                                (phymod_phy_prbs_status_get(&phy_copy, 0, &prbs_status));

                            sal_usleep(100);

                            _SOC_IF_ERR_EXIT
                                (phymod_phy_prbs_status_get(&phy_copy, 0, &prbs_status));
                            /* next check the lock status */
                            if (prbs_status.prbs_lock) {
                                prbs_active_lane_num++;
                                prbs_active_lane_map |= 1 << (i + port_index);
                            }
                        }
                        /* next check active lane PRBS status */
                        if ( ((num_lane == 4) && (prbs_active_lane_num == 3)) ||
                             ((num_lane == 4) && (prbs_active_lane_num == 4)) ||
                             ((num_lane == 8) && (prbs_active_lane_num == 8)) ||
                             ((num_lane == 8) && (prbs_active_lane_num == 6)) ||
                             ((num_lane == 8) && (prbs_active_lane_num == 7)) ) {

                            if (active_lane_map == prbs_active_lane_map) {
                                local_port_rlm_state.rlm_state = PORTMOD_PORT_RLM_SEND_PRBS;
                            } else {
                                active_lane_map = prbs_active_lane_map;
                            }
                        }
                    }
                } /* end of pcs not configured for follower */
            }
            break;
        } /* end of this  LI case */
        case PORTMOD_PORT_RLM_SEND_PRBS:
        {
            uint32 first_active_lane_num = 0, prbs_enabled  = 0, need_reconfig_pcs = 0;
            int follower_prbs_resend = 0;
            prbs_flag = 0;

            state_string ="SEND_PRBS";

            /*first for follower check the prbs lane active map */
            if ((!is_initiator) && (!pcs_is_reconfigured)) {
                /* next check per lane prbs status */
                for (i = 0; i < num_lane; i ++) {
                    phy_copy.access.lane_mask = 1 << (i + port_index);
                    /*first read clear the status */
                    _SOC_IF_ERR_EXIT
                        (phymod_phy_prbs_status_get(&phy_copy, 0, &prbs_status));

                    sal_usleep(100);
                    /* next read the status */
                    _SOC_IF_ERR_EXIT
                        (phymod_phy_prbs_status_get(&phy_copy, 0, &prbs_status));
                    /*first read clear the status */
                    /* next check the lock status */
                    if (prbs_status.prbs_lock) {
                        prbs_active_lane_num++;
                        prbs_active_lane_map |= 1 << (i + port_index);
                    }
                }

                if ((prbs_active_lane_map != active_lane_map) && (prbs_active_lane_map != 0x0)) {
                    active_lane_map = prbs_active_lane_map;
                    follower_prbs_resend = 1;
                }
            }

            /* first set TX prbs only  and check if tx prbs is enabled or not*/
            /* only if the pcs is not reconfigured */
            if (!pcs_is_reconfigured) {
                for (i = 0; i < num_lane; i++) {
                    if (active_lane_map &  1 << (port_index + i)) {
                        first_active_lane_num = port_index + i;
                        break;
                    }
                }
                PHYMOD_PRBS_DIRECTION_TX_SET(prbs_flag);
                phy_copy.access.lane_mask = 1 << first_active_lane_num;
                _SOC_IF_ERR_EXIT
                    (phymod_phy_prbs_enable_get(&phy_copy, prbs_flag, &prbs_enabled));
                /* enable tx prbs if not enabled */
                prbs_config.invert = 0;

                if ((!prbs_enabled) || follower_prbs_resend)  {
                    for (i = 0; i < num_lane; i++) {
                        phy_copy.access.lane_mask = 1 << (port_index + i);
                        if (active_lane_map &  1 << (port_index + i)) {
                            /* if active lane, use prbspoly13 */
                            prbs_config.poly = phymodPrbsPoly13;
                        } else {
                            /* if inactive lane, use prbspoly31 */
                            prbs_config.poly = phymodPrbsPoly31;
                        }
                        /* enable prbs on a per lane basis */
                        _SOC_IF_ERR_EXIT
                            (phymod_phy_prbs_config_set(&phy_copy, prbs_flag, &prbs_config));
                        _SOC_IF_ERR_EXIT
                            (phymod_phy_prbs_enable_set(&phy_copy, prbs_flag, 1));
                        /* set tx_disable and pmd lane reset override enable */
                        _SOC_IF_ERR_EXIT(phymod_phy_pmd_override_enable_set(&phy_copy, phymodPMDTxDisable, 1, 0));
                        _SOC_IF_ERR_EXIT(phymod_phy_pmd_override_enable_set(&phy_copy, phymodPMDLaneReset, 1, 1));
                    }
                    /* next check if initiator  */
                    if (is_initiator) {
                        prbs_flag = 0;
                        prbs_config.invert = 0;
                        prbs_config.poly = phymodPrbsPoly13;
                        PHYMOD_PRBS_DIRECTION_RX_SET(prbs_flag);
                        _SOC_IF_ERR_EXIT
                            (phymod_phy_prbs_config_set(&phy_access, prbs_flag, &prbs_config));
                        _SOC_IF_ERR_EXIT
                            (phymod_phy_prbs_enable_set(&phy_access, prbs_flag, 1));
                    }
                }
            }

            /* next check prbs rx status if initiator */
            if ((is_initiator) && (!pcs_is_reconfigured)) {
                prbs_active_lane_num = 0;
                prbs_active_lane_map = 0x0;
                for (i = 0; i < num_lane; i++) {
                    phy_copy.access.lane_mask = 1 << (port_index + i);
                    /*first read clear the status */
                    _SOC_IF_ERR_EXIT
                        (phymod_phy_prbs_status_get(&phy_copy, 0, &prbs_status));
                    sal_usleep(500);
                    /* first read clear the status */
                    _SOC_IF_ERR_EXIT
                        (phymod_phy_prbs_status_get(&phy_copy, 0, &prbs_status));
                    /* next check the lock status */
                    if (prbs_status.prbs_lock) {
                        prbs_active_lane_num++;
                        prbs_active_lane_map |= 1 << (i + port_index);
                    }
                }
            }

            /* next check the prbs active lane map if initiator*/
            if ((is_initiator) && (prbs_active_lane_map == active_lane_map) && (!pcs_is_reconfigured)) {
                need_reconfig_pcs = 1;
            } else if ((!is_initiator) && (!pcs_is_reconfigured) && (!follower_prbs_resend)) {
                need_reconfig_pcs = 1;
            }

            /* check pcs needs to be re-configured if it's no re-congiured already */
            if (need_reconfig_pcs) {
                phymod_fec_type_t fec_type;
                /* need to reconfigure the pcs */
                /* first disable pcs of the port */
                phy_copy.access.lane_mask = 1 << port_index;
                _SOC_IF_ERR_EXIT
                    (phymod_phy_pcs_enable_set(&phy_access, 0));
                /*set the rx lock and sig detection override */
                _SOC_IF_ERR_EXIT(phymod_phy_pmd_override_enable_set(&phy_access, phymodPMDRxLock, 1, 1));
                _SOC_IF_ERR_EXIT(phymod_phy_pmd_override_enable_set(&phy_access, phymodPMDSignalDetect, 1, 1));

                /* need to adjust the pcs lane swap */
                _SOC_IF_ERR_EXIT
                    (phymod_phy_pcs_lane_swap_adjust(&phy_access, active_lane_map, txLaneSwap, rxLaneSwap));

                /*then disablethe rx lock and sig detection override */
                _SOC_IF_ERR_EXIT(phymod_phy_pmd_override_enable_set(&phy_access, phymodPMDRxLock, 0, 0));
                _SOC_IF_ERR_EXIT(phymod_phy_pmd_override_enable_set(&phy_access, phymodPMDSignalDetect, 0, 0));

                /* then load the speed id based on the active lane num */
                if ((active_lane_num == 4) || (active_lane_num == 8)) {
                    fec_type = FecType;
                } else {
                    /* for all the reduced lane speed, only 544_2xn is valid */
                    fec_type = phymod_fec_RS544_2XN;
                }
                _SOC_IF_ERR_EXIT
                    (phymod_phy_load_speed_id_entry(&phy_access, 50000 * active_lane_num, active_lane_num, fec_type));

                /* next re-enable pcs */
                phy_copy.access.lane_mask = 1 << port_index;
                _SOC_IF_ERR_EXIT
                    (phymod_phy_pcs_enable_set(&phy_access, 1));
                /* set the warm boot variable */
                pcs_is_reconfigured = TRUE;
            }

            if (is_initiator && need_reconfig_pcs) {
                local_port_rlm_state.rlm_state = PORTMOD_PORT_RLM_SEND_LI;
            } else if (!is_initiator) {
                /* for follower, need to check LI status */
                uint32 li_status = 0;
                _SOC_IF_ERR_EXIT
                    (cdmac_link_interruption_live_status_get(unit, port, &li_status));
                if (li_status) {
                    local_port_rlm_state.rlm_state = PORTMOD_PORT_RLM_SEND_LI;
                }
            }
            break;
        }
        case PORTMOD_PORT_RLM_SEND_IDLE:
        {
            uint32 li_enable, li_status, reg_val;
            int local_fault_status, remote_fault_status;

            state_string ="SEND_ILDE";

            /* first check if LI is enabled or not */
            _SOC_IF_ERR_EXIT
                (cdmac_link_interrupt_ordered_set_enable_get(unit, port, &li_enable));
            /* For initiator, disable LI if LI is enabled */
            if (li_enable && is_initiator) {
                _SOC_IF_ERR_EXIT
                    (cdmac_link_interrupt_ordered_set_enable(unit, port, 0));
            }
            /* next check idles are valid */
            _SOC_IF_ERR_EXIT(READ_CDMAC_RX_LSS_STATUSr(unit, port, &reg_val));
            li_status = soc_reg_field_get(unit, CDMAC_RX_LSS_STATUSr, reg_val, LINK_INTERRUPTION_LIVE_STATUSf);
            local_fault_status = soc_reg_field_get(unit, CDMAC_RX_LSS_STATUSr, reg_val, LOCAL_FAULT_STATUSf);
            remote_fault_status = soc_reg_field_get(unit, CDMAC_RX_LSS_STATUSr, reg_val, REMOTE_FAULT_STATUSf);
            /* idle is valid only if none of the above three status are 0 */
            if(!li_status && !local_fault_status && !remote_fault_status) {
                /* For follower, disable LT only if idle is valid. */
                if (!is_initiator && li_enable) {
                    _SOC_IF_ERR_EXIT
                        (cdmac_link_interrupt_ordered_set_enable(unit, port, 0));
                }
                local_port_rlm_state.rlm_state = PORTMOD_PORT_RLM_DONE;
                /* clear tx stall */
                _SOC_IF_ERR_EXIT
                   (cdmac_tx_stall_enable_set(unit, port, 0));
            }
            break;
        }
        case PORTMOD_PORT_RLM_ERROR:
            state_string ="ERROR";
            *is_done = 1;
            break;
        case PORTMOD_PORT_RLM_DONE:
            state_string ="DONE";
            *is_done = 1;
            /* do nothing */
            break;
        default:
            *is_done = 1;
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                               (_SOC_MSG("RLM state not supported \n")));
            break;
        }
    }

    /* finally restore all the Warm boot variables */
    _SOC_IF_ERR_EXIT
        (PM8x50_INITIATOR_SET(unit, pm_info, is_initiator, port_index));

    _SOC_IF_ERR_EXIT
        (PM8x50_PCS_RECONFIG_SET(unit, pm_info, pcs_is_reconfigured, port_index));

    _SOC_IF_ERR_EXIT
        (PM8x50_ACTIVE_LANE_MAP_SET(unit, pm_info, active_lane_map, port_index));

    _SOC_IF_ERR_EXIT
        (PM8x50_RLM_STATE_SET(unit, pm_info, local_port_rlm_state.rlm_state, port_index));

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "port %d RLM state is %-23s \n"),
                 port, state_string));

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm8x50_ber_monitor_workaround(int unit, int port, pm_info_t pm_info, int* is_off)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access, phy_copy;
    int nof_phys, port_index = 0;
    pm8x50_port_ber_war_state_t war_state;
    uint32_t wait_count = 0, fec_bypass_indication = 0, bitmap = 0;
    char* state_string = "\n";

    SOC_INIT_FUNC_DEFS;

    *is_off = 0;

    /* get port index */
    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    /* first get the an state and wait link count */
    _SOC_IF_ERR_EXIT(PM8x50_BER_WAR_STATE_GET(unit, pm_info, &war_state, port_index));
    _SOC_IF_ERR_EXIT(PM8x50_BER_WAR_WAIT_COUNT_GET(unit, pm_info, &wait_count, port_index));

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    sal_memcpy(&phy_copy, &phy_access, sizeof(phy_copy));

    /* based on the AN state */
    switch (war_state) {
        case PM8X50_PORT_BER_WAR_HI_SER_CHECK:
        {
            uint32_t hi_ser_lh = 0, hi_ser_live = 0;

            state_string = "BER_WAR_HI_SER_CHECK\n";
            _SOC_IF_ERR_EXIT(phymod_phy_fec_bypass_indication_get(&phy_access, &fec_bypass_indication));
            if (fec_bypass_indication == 0) {
                war_state = PM8X50_PORT_BER_WAR_COMPLETE;
                *is_off = 1;
            } else {
                /* read hi_ser_LH and hi_ser_Live */
                _SOC_IF_ERR_EXIT(phymod_phy_rs_fec_rxp_get(&phy_access, &hi_ser_lh, &hi_ser_live));
                if ((hi_ser_lh == 1) || (hi_ser_live == 1)) {
                    war_state = PM8X50_PORT_BER_WAR_PATH_RESET;
                } else {
                    war_state = PM8X50_PORT_BER_WAR_HI_SER_CHECK;
                }
            }
            break;
        }
        case PM8X50_PORT_BER_WAR_PATH_RESET:
        {
            uint32_t hi_ser_lh = 0, hi_ser_live = 0;
            phymod_autoneg_status_t an_status;
            portmod_speed_config_t speed_config;

            state_string = "BER_WAR_PATH_RESET\n";
            _SOC_IF_ERR_EXIT(phymod_phy_fec_bypass_indication_get(&phy_access, &fec_bypass_indication));
            if (fec_bypass_indication == 0) {
                war_state = PM8X50_PORT_BER_WAR_COMPLETE;
                *is_off = 1;
            } else {
                /* read AN status */
                _SOC_IF_ERR_EXIT(pm8x50_port_autoneg_status_get(unit, port, pm_info, &an_status));
                /* In case of AN, restart AN */
                if (an_status.enabled) {
                    /* read current speed configuration */
                    _SOC_IF_ERR_EXIT(pm8x50_port_speed_config_get(unit, port, pm_info, &speed_config));
                    if (speed_config.speed == 200000) {
                        uint32 phy_flags = 0;
                        phymod_autoneg_control_t an_control;
                        /* read current AN settings */
                        _SOC_IF_ERR_EXIT(pm8x50_port_autoneg_get(unit, port, pm_info, phy_flags, &an_control));
                        /* Get an_mode from WB */
                        _SOC_IF_ERR_EXIT(PM8x50_AN_MODE_GET(unit, pm_info, &an_control.an_mode, port_index));
                        /* disable AN */
                        an_control.enable = 0;
                        _SOC_IF_ERR_EXIT(phymod_phy_autoneg_set(&phy_access, &an_control));
                        /* enable AN */
                        an_control.enable = 1;
                        _SOC_IF_ERR_EXIT(phymod_phy_autoneg_set(&phy_access, &an_control));
                    } else if (speed_config.speed == 400000) {
                        pm8x50_port_400g_an_state_t an_state = PM8X50_PORT_AN_DISABLE;
                        uint32 an_wait_link_count = 0;
                        /* update the 400G AN state to restart AN */
                        _SOC_IF_ERR_EXIT(PM8x50_400G_AN_STATE_SET(unit, pm_info, an_state));
                        _SOC_IF_ERR_EXIT(PM8x50_400G_AN_WAIT_LINK_COUNT_SET(unit, pm_info, an_wait_link_count));
                    }
                    wait_count = 0;
                    war_state = PM8X50_PORT_BER_WAR_WAIT_COUNT;
                } else { /* FS */
                    /* in case of FS, disable PCS of the port */
                    phy_copy.access.lane_mask = 1 << port_index;
                    _SOC_IF_ERR_EXIT(phymod_phy_pcs_enable_set(&phy_access, 0));
                    /* re-enable pcs */
                    _SOC_IF_ERR_EXIT(phymod_phy_pcs_enable_set(&phy_access, 1));
                    /* read hi_ser_LH and hi_ser_Live again to clear the alarms */
                    _SOC_IF_ERR_EXIT(phymod_phy_rs_fec_rxp_get(&phy_access, &hi_ser_lh, &hi_ser_live));
                    wait_count = 0;
                    war_state = PM8X50_PORT_BER_WAR_WAIT_COUNT;
                }
            }
            break;
        }
        case PM8X50_PORT_BER_WAR_WAIT_COUNT:
        {
            state_string = "BER_WAR_WAIT_COUNT\n";
            wait_count++;
            if (wait_count >= 20) {
                war_state = PM8X50_PORT_BER_WAR_COMPLETE;
            }
            break;
        }
        case PM8X50_PORT_BER_WAR_COMPLETE:
        {
            state_string = "BER_WAR_COMPLETE\n";
            _SOC_IF_ERR_EXIT(phymod_phy_fec_bypass_indication_get(&phy_access, &fec_bypass_indication));
            if (fec_bypass_indication == 1) {
                war_state = PM8X50_PORT_BER_WAR_HI_SER_CHECK;
                *is_off = 0;
            } else {
                *is_off = 1;
            }
            break;
        }
        default:
            *is_off = 1;
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                               (_SOC_MSG("BER MON WAR state not supported \n")));
    }

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit, "port %d 200G 400G BER Workaround state is %-23s \n"),
                 port, state_string));

    /* store WB values */
    fec_bypass_indication = ((*is_off) == 1) ? 0 : 1;
    _SOC_IF_ERR_EXIT(PM8x50_BER_WAR_ENABLE_SET(unit, pm_info, fec_bypass_indication, port_index));
    _SOC_IF_ERR_EXIT(PM8x50_BER_WAR_STATE_SET(unit, pm_info, war_state, port_index));
    _SOC_IF_ERR_EXIT(PM8x50_BER_WAR_WAIT_COUNT_SET(unit, pm_info, wait_count, port_index));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_mac_link_get(int unit, int port, pm_info_t pm_info, int* link)
{
    uint32 reg_val, bitmap;
    int port_index, phy_acc;
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(_pm8x50_phy_access_get(unit, port, pm_info, &phy_acc));
    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, &bitmap));

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


STATIC
int _pm8x50_three_cw_bad_recover(int unit, int port, pm_info_t pm_info, int* is_completed)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, port_index = 0;
    pm8x50_port_3_cw_bad_state_t war_state;
    uint32_t tick_cnt = 0, fec_bypass_indication = 0, bitmap = 0;
    uint32_t signal_detect, three_bad;
    uint32_t prbs_tx_en = 0, prbs_rx_en=0, prbs_flag;
    int link, is_3_cw_bad_enable;
    char* state_string = "\n";

    SOC_INIT_FUNC_DEFS;

    *is_completed = 0;

    _SOC_IF_ERR_EXIT
        (_pm8x50_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT
        (PM8x50_3_CW_BAD_STATE_GET(unit, pm_info, &war_state, port_index));
    _SOC_IF_ERR_EXIT
        (PM8x50_3_CW_BAD_TICK_CNT_GET(unit, pm_info, &tick_cnt, port_index));

    _SOC_IF_ERR_EXIT
        (portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT
        (pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                         &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT
        (phymod_phy_fec_bypass_indication_get(&phy_access, &fec_bypass_indication));
    if (fec_bypass_indication) {
        war_state = PM8X50_PORT_3_CW_BAD_COMPLETED;
        tick_cnt = 0;
        is_3_cw_bad_enable = 0;
        _SOC_IF_ERR_EXIT
            (PM8x50_3_CW_BAD_ENABLE_SET(unit, pm_info, is_3_cw_bad_enable, port_index));
        _SOC_IF_ERR_EXIT
            (PM8x50_3_CW_BAD_STATE_SET(unit, pm_info, war_state, port_index));
        _SOC_IF_ERR_EXIT
            (PM8x50_3_CW_BAD_TICK_CNT_SET(unit, pm_info, tick_cnt, port_index));
        *is_completed = 1;
        return (SOC_E_NONE);
    }

    _SOC_IF_ERR_EXIT
        (pm8x50_port_mac_link_get(unit, port, pm_info, &link));

    switch (war_state) {
        case PM8X50_PORT_3_CW_BAD_CHECK:
        {
            state_string = "3_CW_BAD_CHECK\n";
            /*
             * Running PRBS on the link up and RS-FEC enabling
             * ports may cause the three back-to-back CW bad.
             * Complete the recovery if prbs is enabled.
             */
            prbs_flag = 0;
            PHYMOD_PRBS_DIRECTION_RX_CLR(prbs_flag);
            PHYMOD_PRBS_DIRECTION_TX_SET(prbs_flag);
            _SOC_IF_ERR_EXIT
                (phymod_phy_prbs_enable_get(&phy_access, prbs_flag, &prbs_tx_en));
            PHYMOD_PRBS_DIRECTION_TX_CLR(prbs_flag);
            PHYMOD_PRBS_DIRECTION_RX_SET(prbs_flag);
            _SOC_IF_ERR_EXIT
                (phymod_phy_prbs_enable_get(&phy_access, prbs_flag, &prbs_rx_en));
            if (prbs_tx_en | prbs_rx_en) {
                war_state = PM8X50_PORT_3_CW_BAD_COMPLETED;
                tick_cnt = 0;
                is_3_cw_bad_enable = 0;
                _SOC_IF_ERR_EXIT
                    (PM8x50_3_CW_BAD_ENABLE_SET(unit, pm_info, is_3_cw_bad_enable, port_index));
                _SOC_IF_ERR_EXIT
                    (PM8x50_3_CW_BAD_STATE_SET(unit, pm_info, war_state, port_index));
                _SOC_IF_ERR_EXIT
                    (PM8x50_3_CW_BAD_TICK_CNT_SET(unit, pm_info, tick_cnt, port_index));
                *is_completed = 1;
                return (SOC_E_NONE);
            }

            _SOC_IF_ERR_EXIT
                (phymod_phy_rx_signal_detect_get(&phy_access, &signal_detect));
            _SOC_IF_ERR_EXIT
                (phymod_phy_fec_three_cw_bad_state_get(&phy_access, &three_bad));
            if (signal_detect && three_bad && !link) {
                war_state = PM8X50_PORT_3_CW_BAD_RECOVER;
                tick_cnt = 0;
            } else {
                /*Normal down*/
                war_state = PM8X50_PORT_3_CW_BAD_COMPLETED;
                tick_cnt = 0;
            }
            break;
        }
        case PM8X50_PORT_3_CW_BAD_RECOVER:
        {
            state_string = "3_CW_BAD_RECOVER\n";
            if (link) {
                /* Complete the SW recovery, if HW recovers the three
                 * cw bad itself.
                 */
                war_state = PM8X50_PORT_3_CW_BAD_COMPLETED;
                tick_cnt = 0;
            } else {
                /*rx sque on and off*/
                _SOC_IF_ERR_EXIT
                    (phymod_phy_rx_lane_control_set(&phy_access, phymodRxSquelchOn));
                /* add some delay */
                sal_usleep(5000);
                _SOC_IF_ERR_EXIT
                    (phymod_phy_rx_lane_control_set(&phy_access, phymodRxSquelchOff));
                sal_usleep(10000);
                war_state = PM8X50_PORT_3_CW_BAD_LINK_WAIT;
                tick_cnt = 0;
            }
            break;
        }
        case PM8X50_PORT_3_CW_BAD_LINK_WAIT:
        {
            state_string = "3_CW_BAD_LINK_WAIT\n";
            if (link) {
                war_state = PM8X50_PORT_3_CW_BAD_COMPLETED;
                tick_cnt = 0;
            } else {
                if (tick_cnt < 10) {
                    war_state = PM8X50_PORT_3_CW_BAD_LINK_WAIT;
                    tick_cnt += 1;
                } else {
                    war_state = PM8X50_PORT_3_CW_BAD_CHECK;
                    tick_cnt = 0;
                }
            }
            break;
        }
        case PM8X50_PORT_3_CW_BAD_COMPLETED:
        {
            state_string = "3_CW_BAD_COMPLETED\n";
            war_state = PM8X50_PORT_3_CW_BAD_COMPLETED;
            tick_cnt = 0;
            is_3_cw_bad_enable = 0;
            _SOC_IF_ERR_EXIT
                (PM8x50_3_CW_BAD_ENABLE_SET(unit, pm_info, is_3_cw_bad_enable, port_index));
            *is_completed = 1;
            break;
        }
        default:
            war_state = PM8X50_PORT_3_CW_BAD_STATE_COUNT;
            tick_cnt = 0;
            is_3_cw_bad_enable = 0;
            _SOC_IF_ERR_EXIT
                (PM8x50_3_CW_BAD_ENABLE_SET(unit, pm_info, is_3_cw_bad_enable, port_index));
            _SOC_IF_ERR_EXIT
                (PM8x50_3_CW_BAD_STATE_SET(unit, pm_info, war_state, port_index));
            _SOC_IF_ERR_EXIT
                (PM8x50_3_CW_BAD_TICK_CNT_SET(unit, pm_info, tick_cnt, port_index));
            *is_completed = 1;
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                               (_SOC_MSG("FEC 3_CW_BAD state not supported \n")));
            break;
    }

    _SOC_IF_ERR_EXIT
        (PM8x50_3_CW_BAD_STATE_SET(unit, pm_info, war_state, port_index));
     _SOC_IF_ERR_EXIT
        (PM8x50_3_CW_BAD_TICK_CNT_SET(unit, pm_info, tick_cnt, port_index));

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit, "port %d FEC 3_CW_BAD state is %-23s\n"),
                 port, state_string));
exit:
    SOC_FUNC_RETURN;

}


static int pm8x50_periodic_callback(int unit, int port)
{
    pm_info_t pm_info;
    int port_index, rlm_enabled, rlm_state, is_400g_an, an_400g_state;
    uint32 bitmap;
    int rlm_done = 1, an_done = 1, ber_war_off = 1;
    int is_ber_war_enable = 0, valid = 0;
    int is_3_cw_bad_enable =0, is_3_cw_bad_completed = 0;
    portmod_speed_config_t speed_config;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_is_valid(unit, port, &valid));
    if (!valid) {
        return (SOC_E_NONE);
    }
    _SOC_IF_ERR_EXIT(portmod_pm_info_get(unit, port, &pm_info));

    /* LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit, "Callback was called port=%d\n"), port)); */

    /* get port index */
    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, &bitmap));

    /* first get all the RLM warmboot variable */
    _SOC_IF_ERR_EXIT
        (PM8x50_RLM_ENABLE_GET(unit, pm_info, &rlm_enabled, port_index));

    _SOC_IF_ERR_EXIT
        (PM8x50_RLM_STATE_GET(unit, pm_info, &rlm_state, port_index));

    /* read the 400G an enabled */
    _SOC_IF_ERR_EXIT(PM8x50_400G_AN_ENABLE_GET(unit, pm_info, &is_400g_an, port_index));

   /* check if RLM state machine needs to be updated */
   if (rlm_enabled == 1) {
        /*next check only state is not done or error */
        if ((rlm_state != PORTMOD_PORT_RLM_DONE) && (rlm_state != PORTMOD_PORT_RLM_ERROR)) {
            _SOC_IF_ERR_EXIT(pm8x50_port_rlm_state_update(unit, port, pm_info, &rlm_done));
        }
    }

    /* check if 400G an state machine needs to be updated */
    if (is_400g_an) {
        _SOC_IF_ERR_EXIT(PM8x50_400G_AN_STATE_GET(unit, pm_info, &an_400g_state));
        if (an_400g_state < PM8X50_PORT_AN_COMPLETE) {
            _SOC_IF_ERR_EXIT(_pm8x50_400g_an_workaround(unit, port, pm_info, &an_done));
        }
    }

    /* read current speed configuration */
    _SOC_IF_ERR_EXIT(pm8x50_port_speed_config_get(unit, port, pm_info, &speed_config));
    /* call BER monitor workaround if BER WAR is enabled(FEC_BYPASS_INDICATION is ON for 200G and 400G) */
    if ((speed_config.speed == 200000) || (speed_config.speed == 400000)) {
        /* read BER WAR warmboot variables */
        _SOC_IF_ERR_EXIT(PM8x50_BER_WAR_ENABLE_GET(unit, pm_info, &is_ber_war_enable, port_index));
        if (is_ber_war_enable) {
            _SOC_IF_ERR_EXIT(_pm8x50_ber_monitor_workaround(unit, port, pm_info, &ber_war_off));
        }
    }

    _SOC_IF_ERR_EXIT(PM8x50_3_CW_BAD_ENABLE_GET(unit, pm_info, &is_3_cw_bad_enable, port_index));
    if (is_3_cw_bad_enable) {
        _SOC_IF_ERR_EXIT(_pm8x50_three_cw_bad_recover(unit, port, pm_info, &is_3_cw_bad_completed));
    }

     /* if both signal unregister - go for it */
    if (rlm_done && an_done && ber_war_off && is_3_cw_bad_completed) {
        _SOC_IF_ERR_EXIT(portmod_port_periodic_callback_unregister(unit, port));
    }

exit:
    SOC_FUNC_RETURN;
}

/*Add new pm.*/
int pm8x50_pm_init(int unit,
                   const portmod_pm_create_info_internal_t* pm_add_info,
                   int wb_buffer_index,
                   pm_info_t pm_info)
{
    const portmod_pm8x50_create_info_t *info =
                &pm_add_info->pm_specific_info.pm8x50;
    pm8x50_t pm8x50_data = NULL;
    int i, rv;
    int pm_is_active, ts_enable_port_count, is_initiator, pcs_reconfigured, active_lane_map;
    int is_core_initialized, fec, invalid_port, rlm_state, fecType, txLaneMap, rxLaneMap, rlm_enabled;
    int an_state, is_400g_an, an_wait_link_count, is_ber_war_enable = 0, ber_war_state = 0, ber_war_wait_count = 0;
    int core_is_flexe_configured, remote_pause, is_flexe_tvco_26g_required;
    uint8 ovco_pll_adv_lane_bitmap, tvco_pll_adv_lane_bitmap;
    uint8 ovco_pll_active_lane_bitmap, tvco_pll_active_lane_bitmap;
    int probe= 0;
    uint32_t pll_div, is_pwrdn;
    phymod_phy_access_t phy_access;
    portmod_vco_type_t vco;
    int phy;
    int is_3_cw_bad_enable =0, three_cw_bad_state = 0, three_cw_bad_tick_cnt = 0;
    int portmon_disable, port_flexe_mode;
    SOC_INIT_FUNC_DEFS;

    pm_info->type = pm_add_info->type;
    pm_info->unit = unit;
    pm_info->wb_buffer_id = wb_buffer_index;

    /* PM8x50 specific info */
    pm8x50_data = sal_alloc(sizeof(struct pm8x50_s), "specific_db");
    SOC_NULL_CHECK(pm8x50_data);
    pm_info->pm_data.pm8x50_db = pm8x50_data;

    PORTMOD_PBMP_ASSIGN(pm8x50_data->phys, pm_add_info->phys);
    pm8x50_data->int_core_access.type = phymodDispatchTypeCount;
    PORTMOD_PBMP_ITER(pm_add_info->phys, phy) {
        PM_8x50_INFO(pm_info)->first_phy = phy;
        break;
    }
    pm8x50_data->warmboot_skip_db_restore = TRUE;
    pm8x50_data->rescal = info->rescal;
    pm8x50_data->portmod_mac_soft_reset = info->portmod_mac_soft_reset;
    pm8x50_data->portmod_egress_buffer_reset = info->portmod_egress_buffer_reset;

    /* init intertnal SerDes core access */
    phymod_core_access_t_init(&pm8x50_data->int_core_access);

    sal_memcpy(&pm8x50_data->polarity, &info->polarity,
               sizeof(phymod_polarity_t));
    sal_memcpy(&(pm8x50_data->int_core_access.access), &info->access.access,
                sizeof(phymod_access_t));

    sal_memcpy(&pm8x50_data->lane_map, &info->lane_map,
                sizeof(pm8x50_data->lane_map));
    pm8x50_data->ref_clk = info->ref_clk;
    pm8x50_data->fw_load_method = info->fw_load_method;
    pm8x50_data->external_fw_loader = info->external_fw_loader;
    pm8x50_data->tvco = info->tvco;
    pm8x50_data->ovco = info->ovco;
    pm8x50_data->core_num  = info->core_num;
    pm8x50_data->afe_pll = info->afe_pll;
    pm8x50_data->is_master_pm = info->is_master_pm;
    pm8x50_data->allow_20p625g_tvco = info->allow_20p625g_tvco;
    pm8x50_data->single_vco_used_by_ports = info->single_vco_used_by_ports;
    pm8x50_data->pm_offset = info->pm_offset;

    /* for DNX devices only */
    pm8x50_data->clock_buffer_disable_required = info->clock_buffer_disable_required;
    pm8x50_data->clock_buffer_bw_update_required = info->clock_buffer_bw_update_required;

    if (info->access.access.bus == NULL) {
        /* if null - use default */
        pm8x50_data->int_core_access.access.bus = &pm8x50_default_bus;
    } else {
        /* check null for mem_read/mem_write */
        if (pm8x50_data->int_core_access.access.bus->mem_read == NULL) {
            pm8x50_data->int_core_access.access.bus->mem_read =pm8x50_default_bus.mem_read;
        }
        if (pm8x50_data->int_core_access.access.bus->mem_write == NULL) {
            pm8x50_data->int_core_access.access.bus->mem_write =pm8x50_default_bus.mem_write;
        }
    }

    if (pm8x50_data->external_fw_loader == NULL) {
        /* if null - use default */
        pm8x50_data->external_fw_loader = pm8x50_default_fw_loader;
    }

    /*init wb buffer*/
    _SOC_IF_ERR_EXIT(pm8x50_wb_buffer_init(unit, wb_buffer_index,  pm_info));

    if (pm8x50_data->int_core_access.access.pmd_info_ptr == NULL) {
        pm8x50_data->int_core_access.access.pmd_info_ptr = sal_alloc(PMD_INFO_DATA_STRUCTURE_SIZE,
                             "PortMod PM PMD info");
        if (pm8x50_data->int_core_access.access.pmd_info_ptr == NULL) {
            return SOC_E_MEMORY;
        }
        sal_memset(pm8x50_data->int_core_access.access.pmd_info_ptr, 0, PMD_INFO_DATA_STRUCTURE_SIZE);
    }


    if (SOC_WARM_BOOT(unit)) {
        int rlm_enabled, rlm_state, i, port_local;
        /* For warmboot, probe Serdes driver type for active PMs. */
        is_core_initialized = 0;
        rlm_enabled = 0;
        rlm_state = PORTMOD_PORT_RLM_FSM_COUNT;
        rv = PM8x50_IS_CORE_INITIALIZED_GET(unit, pm_info, &is_core_initialized);
        _SOC_IF_ERR_EXIT(rv);
        _SOC_IF_ERR_EXIT(PM8x50_400G_AN_STATE_GET(unit, pm_info, &an_state));


        /* print scache information for debugging */
        pm8x50_pm_wb_debug_log(unit, pm_info);

        /* next check if RLM portmod thread needs to be re-registered */
        for (i = 0; i < MAX_PORTS_PER_PM8X50; i++) {
            /* first get the logical port id */
            _SOC_IF_ERR_EXIT(PM8x50_LANE2PORT_GET(unit, pm_info,i, &port_local));
            _SOC_IF_ERR_EXIT(PM8x50_RLM_ENABLE_GET(unit, pm_info, &rlm_enabled, i));
            _SOC_IF_ERR_EXIT(PM8x50_RLM_STATE_GET(unit, pm_info, &rlm_state, i));
            _SOC_IF_ERR_EXIT(PM8x50_400G_AN_ENABLE_GET(unit, pm_info, &is_400g_an, i));
            _SOC_IF_ERR_EXIT(PM8x50_BER_WAR_ENABLE_GET(unit, pm_info, &is_ber_war_enable, i));
            _SOC_IF_ERR_EXIT(PM8x50_BER_WAR_STATE_GET(unit, pm_info, &ber_war_state, i));
            _SOC_IF_ERR_EXIT(PM8x50_400G_AN_REMOTE_PAUSE_GET(unit, pm_info, &remote_pause, i));
            _SOC_IF_ERR_EXIT(PM8x50_3_CW_BAD_ENABLE_GET(unit, pm_info, &is_3_cw_bad_enable, i));
            /* need to re-register the call back */
            if ( (port_local >= 0) && (((rlm_enabled) && (rlm_state >= PORTMOD_PORT_RLM_SEND_PACKET_DATA) &&
               (rlm_state < PORTMOD_PORT_RLM_DONE)) ||
               ((is_400g_an) && (an_state < PM8X50_PORT_AN_COMPLETE)) ||
               (is_ber_war_enable) || (is_3_cw_bad_enable))) {
                    _SOC_IF_ERR_EXIT
                        (portmod_portmon_disable_get(unit, &portmon_disable));
                    if (!portmon_disable) {
                        _SOC_IF_ERR_EXIT(portmod_port_periodic_callback_register(unit, port_local, pm8x50_periodic_callback, PORTMOD_THREAD_INTERVAL_1s));
                    }
            }
        }

        if (is_core_initialized) {
            _SOC_IF_ERR_EXIT(portmod_common_serdes_probe(pm8x50_serdes_list, &pm8x50_data->int_core_access, &probe));
            if (!probe) {
                LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit, "ERROR: serdes probe failed type=%d\n"), pm8x50_data->int_core_access.type));
            }

            /* Get VCO rates from HW */
            sal_memcpy(&phy_access, &(pm8x50_data->int_core_access),
                       sizeof(phymod_phy_access_t));

            /* 1. Get TVCO rate :
             *    a. If TVCO is not free, get TVCO rate from HW.
             *    b. If TVCO is free, TVCO is powerdown.
             */
            _SOC_IF_ERR_EXIT(phymod_phy_pll_powerdown_get(&phy_access, phy_access.access.tvco_pll_index, &is_pwrdn));
            if (is_pwrdn)
            {
                pm8x50_data->tvco = portmodVCOInvalid;
            }
            else
            {
                phy_access.access.pll_idx = phy_access.access.tvco_pll_index;
                _SOC_IF_ERR_EXIT(phymod_phy_pll_multiplier_get(&phy_access, &pll_div));
                _SOC_IF_ERR_EXIT(_pm8x50_pll_to_vco_get(info->ref_clk, pll_div, &vco));
                pm8x50_data->tvco = vco;
            }

            /* 2. Get OVCO rate:
             *    a. If OVCO is not free, get OVCO rate from HW.
             *    b. If OVCO is free, OVCO is powerdown.
             */
            phy_access.access.pll_idx = (phy_access.access.tvco_pll_index == 1)? 0: 1;
            _SOC_IF_ERR_EXIT(phymod_phy_pll_powerdown_get(&phy_access, phy_access.access.pll_idx, &is_pwrdn));
            if (is_pwrdn)
            {
                pm8x50_data->ovco = portmodVCOInvalid;
            }
            else
            {
                _SOC_IF_ERR_EXIT(phymod_phy_pll_multiplier_get(&phy_access, &pll_div));
                _SOC_IF_ERR_EXIT(_pm8x50_pll_to_vco_get(info->ref_clk, pll_div, &vco));
                pm8x50_data->ovco = vco;
            }
        }
    } else {
        /* For coldboot, initialized warmboot variables for the PM. */
        is_core_initialized = 0;
        rv = PM8x50_IS_CORE_INITIALIZED_SET(unit, pm_info,
                                            is_core_initialized);
        _SOC_IF_ERR_EXIT(rv);

        pm_is_active = 0;
        rv = PM8x50_IS_ACTIVE_SET(unit, pm_info, pm_is_active);
        _SOC_IF_ERR_EXIT(rv);

        ovco_pll_adv_lane_bitmap = 0;
        rv = PM8x50_OVCO_PLL_ADV_LANE_BITMAP_SET(unit, pm_info, ovco_pll_adv_lane_bitmap);
        _SOC_IF_ERR_EXIT(rv);

        tvco_pll_adv_lane_bitmap = 0;
        rv = PM8x50_TVCO_PLL_ADV_LANE_BITMAP_SET(unit, pm_info, tvco_pll_adv_lane_bitmap);
        _SOC_IF_ERR_EXIT(rv);

        ovco_pll_active_lane_bitmap = 0;
        rv = PM8x50_OVCO_PLL_ACTIVE_LANE_BITMAP_SET(unit, pm_info, ovco_pll_active_lane_bitmap);
        _SOC_IF_ERR_EXIT(rv);

        tvco_pll_active_lane_bitmap = 0;
        rv = PM8x50_TVCO_PLL_ACTIVE_LANE_BITMAP_SET(unit, pm_info, tvco_pll_active_lane_bitmap);
        _SOC_IF_ERR_EXIT(rv);

        fec = 0;
        for (i = 0; i < MAX_PORTS_PER_PM8X50; i++) {
            _SOC_IF_ERR_EXIT(PM8x50_LANE2FEC_SET(unit, pm_info, i, fec));
        }

        invalid_port = -1;
        for (i = 0; i < MAX_PORTS_PER_PM8X50; i++) {
            _SOC_IF_ERR_EXIT(PM8x50_LANE2PORT_SET(unit, pm_info, i, invalid_port));
        }

        ts_enable_port_count = 0;
        rv = PM8x50_TS_ENABLE_PORT_COUNT_SET(unit, pm_info, ts_enable_port_count);
        _SOC_IF_ERR_EXIT(rv);

        rlm_enabled = 0;
        for (i = 0; i < MAX_PORTS_PER_PM8X50; i++) {
            _SOC_IF_ERR_EXIT(PM8x50_RLM_ENABLE_SET(unit, pm_info, rlm_enabled, i));
        }

        is_initiator = 0;
        for (i = 0; i < MAX_PORTS_PER_PM8X50; i++) {
            _SOC_IF_ERR_EXIT(PM8x50_INITIATOR_SET(unit, pm_info, is_initiator, i));
        }

        pcs_reconfigured = 0;
        for (i = 0; i < MAX_PORTS_PER_PM8X50; i++) {
            _SOC_IF_ERR_EXIT(PM8x50_PCS_RECONFIG_SET(unit, pm_info, pcs_reconfigured, i));
        }

        active_lane_map = 0;
        for (i = 0; i < MAX_PORTS_PER_PM8X50; i++) {
            _SOC_IF_ERR_EXIT(PM8x50_ACTIVE_LANE_MAP_SET(unit, pm_info, active_lane_map, i));
        }

        rlm_state = PORTMOD_PORT_RLM_FSM_COUNT;
        for (i = 0; i < MAX_PORTS_PER_PM8X50; i++) {
            _SOC_IF_ERR_EXIT(PM8x50_RLM_STATE_SET(unit, pm_info, rlm_state, i));
        }

        fecType = 0;
        for (i = 0; i < MAX_PORTS_PER_PM8X50; i++) {
            _SOC_IF_ERR_EXIT(PM8x50_ORIGINAL_FEC_SET(unit, pm_info, fecType, i));
        }

        txLaneMap = 0x76543210;
        rv = PM8x50_ORIG_TX_LANE_MAP_SET(unit, pm_info, txLaneMap);
        _SOC_IF_ERR_EXIT(rv);

        rxLaneMap = 0x76543210;
        rv = PM8x50_ORIG_RX_LANE_MAP_SET(unit, pm_info, rxLaneMap);
        _SOC_IF_ERR_EXIT(rv);

        is_400g_an = 0;
        for (i = 0; i < MAX_PORTS_PER_PM8X50; i++) {
            _SOC_IF_ERR_EXIT(PM8x50_400G_AN_ENABLE_SET(unit, pm_info, is_400g_an, i));
        }

        an_state = PM8X50_PORT_AN_STATE_COUNT;
        rv = PM8x50_400G_AN_STATE_SET(unit, pm_info, an_state);
        _SOC_IF_ERR_EXIT(rv);

        an_wait_link_count = 0;
        rv = PM8x50_400G_AN_WAIT_LINK_COUNT_SET(unit, pm_info, an_wait_link_count);
        _SOC_IF_ERR_EXIT(rv);

        is_ber_war_enable = 0;
        is_3_cw_bad_enable = 0;
        for (i = 0; i < MAX_PORTS_PER_PM8X50; i++) {
            _SOC_IF_ERR_EXIT(PM8x50_BER_WAR_ENABLE_SET(unit, pm_info, is_ber_war_enable, i));

            ber_war_state = PM8X50_PORT_BER_WAR_STATE_COUNT;
            rv = PM8x50_BER_WAR_STATE_SET(unit, pm_info, ber_war_state, i);
            _SOC_IF_ERR_EXIT(rv);

            ber_war_wait_count = 0;
            rv = PM8x50_BER_WAR_WAIT_COUNT_SET(unit, pm_info, ber_war_wait_count, i);
            _SOC_IF_ERR_EXIT(rv);

            _SOC_IF_ERR_EXIT(PM8x50_3_CW_BAD_ENABLE_SET(unit, pm_info, is_3_cw_bad_enable, i));

            three_cw_bad_state = PM8X50_PORT_3_CW_BAD_STATE_COUNT;
            rv = PM8x50_3_CW_BAD_STATE_SET(unit, pm_info, three_cw_bad_state, i);
            _SOC_IF_ERR_EXIT(rv);

            three_cw_bad_tick_cnt = 0;
            rv = PM8x50_3_CW_BAD_TICK_CNT_SET(unit, pm_info, three_cw_bad_tick_cnt, i);
            _SOC_IF_ERR_EXIT(rv);
        }

        core_is_flexe_configured = 0;
        rv = PM8x50_IS_CORE_FLEXE_CONFIGURED_SET(unit, pm_info, core_is_flexe_configured);
        _SOC_IF_ERR_EXIT(rv);

        remote_pause = 0;
        for (i = 0; i < MAX_PORTS_PER_PM8X50; i++) {
            _SOC_IF_ERR_EXIT(PM8x50_400G_AN_REMOTE_PAUSE_SET(unit, pm_info, remote_pause, i));
        }

        is_flexe_tvco_26g_required = 0;
        rv = PM8x50_IS_FLEXE_TVCO_26G_REQUIRED_SET(unit, pm_info, is_flexe_tvco_26g_required);
        _SOC_IF_ERR_EXIT(rv);

        port_flexe_mode = 0;
        for (i = 0; i < MAX_PORTS_PER_PM8X50; i++) {
            rv = PM8x50_PORT_FLEXE_MODE_SET(unit, pm_info, port_flexe_mode, i);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

exit:
    if (SOC_FUNC_ERROR) {
        pm8x50_pm_destroy(unit, pm_info);
    }
    SOC_FUNC_RETURN;
}

/*Release PM resources*/
int pm8x50_pm_destroy(int unit, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;

    if (pm_info->pm_data.pm8x50_db != NULL) {
        if (PM_8x50_INFO(pm_info)->int_core_access.access.pmd_info_ptr != NULL) {
            sal_free(PM_8x50_INFO(pm_info)->int_core_access.access.pmd_info_ptr);
            PM_8x50_INFO(pm_info)->int_core_access.access.pmd_info_ptr = NULL;
        }
        sal_free(pm_info->pm_data.pm8x50_db);
        pm_info->pm_data.pm8x50_db = NULL;
    }

    SOC_FUNC_RETURN;
}

/* This function will check the speed config is valid or not, and return required vco for valid input */
/* The supported speed config in this function is based on PM8x50 Portmod Spec */
STATIC
int _pm8x50_port_speed_config_to_vco_get(const portmod_speed_config_t* speed_config,
                                         int speed_for_pcs_bypass_port,
                                         uint32 flags,
                                         portmod_vco_type_t* vco)
{
    SOC_INIT_FUNC_DEFS;

    if (!speed_for_pcs_bypass_port) {
        if (PM8X50_PM_VERSION_F_REV0_16NM_GET(flags)) {
            _SOC_IF_ERR_EXIT(
                    _pm8x50_fs_ability_table_vco_get(speed_config->speed,
                                                     speed_config->num_lane,
                                                     speed_config->fec,
                                                     vco));
        } else {
            _SOC_IF_ERR_EXIT(
                    _pm8x50_gen2_fs_ability_table_vco_get(speed_config->speed,
                                                          speed_config->num_lane,
                                                          speed_config->fec,
                                                          vco));
        }
    } else {
        _SOC_IF_ERR_EXIT(
                pm8x50_shared_pcs_bypassed_vco_get(speed_config->speed, vco));
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

/* This function can be called before port is attached.
 * So we need to use physical port number to access the register.
 */
STATIC
int _pm8x50_pm_version_get(int unit, pm_info_t pm_info, pm_version_t *version)
{
    uint32 reg_val, tech_process, pm_type;
    int phy_acc;
    uint32 rev_number, rev_letter;

    SOC_INIT_FUNC_DEFS;

    version->type            = PM_TYPE_PM8X50;
    version->gen             = PM_GEN_INVALID;
    version->tech_process    = PM_TECH_INVALID;
    version->revision_number = PM_REVISION_NUM_INVALID;
    version->revision_letter = PM_REVISION_LETTER_INVALID;

    phy_acc = PM_8x50_INFO(pm_info)->first_phy | SOC_REG_ADDR_PHY_ACC_MASK;

    _SOC_IF_ERR_EXIT(READ_CDPORT_PM_VERSION_IDr(unit, phy_acc, &reg_val));

    tech_process  = soc_reg_field_get(unit, CDPORT_PM_VERSION_IDr, reg_val, TECH_PROCESSf);
    if (tech_process == 1) {
        version->tech_process  = PM_TECH_16NM;
    }

    /* need to check PM verion type */
    pm_type = soc_reg_field_get(unit, CDPORT_PM_VERSION_IDr, reg_val, PM_TYPEf);
    if (pm_type == PM8x50_FLEXE_TYPE) {
        version->type = PM_TYPE_PM8X50_FLEXE;
    }

    /* get revision number */
    rev_number = soc_reg_field_get(unit, CDPORT_PM_VERSION_IDr, reg_val, REV_NUMBERf);
    if (rev_number == 0) {
        version->revision_number = PM_REVISION_0;
    } else if (rev_number == 1) {
        version->revision_number = PM_REVISION_1;
    } else if (rev_number == 2) {
        version->revision_number = PM_REVISION_2;
    } else if (rev_number == 3) {
        version->revision_number = PM_REVISION_3;
    } else if (rev_number == 4) {
        version->revision_number = PM_REVISION_4;
    } else if (rev_number == 5) {
        version->revision_number = PM_REVISION_5;
    } else if (rev_number == 6) {
        version->revision_number = PM_REVISION_6;
    } else if (rev_number == 7) {
        version->revision_number = PM_REVISION_7;
    }

    /* get revision letter  */
    rev_letter = soc_reg_field_get(unit, CDPORT_PM_VERSION_IDr, reg_val, REV_LETTERf);
    if (rev_letter == 0) {
        version->revision_letter = PM_REVISION_A;
    } else if (rev_letter == 1) {
        version->revision_letter = PM_REVISION_B;
    } else if (rev_letter == 2) {
        version->revision_letter = PM_REVISION_C;
    } else if (rev_letter == 3) {
        version->revision_letter = PM_REVISION_D;
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm8x50_lanebitmap_set(int starting_lane, int num_lane, uint8 *bitmap)
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
int _pm8x50_fec_validate(int unit,
                         uint32 flags,
                         uint8 rs528_bitmap,
                         uint8 rs544_bitmap,
                         uint8 rs272_bitmap,
                         uint8 *affected_lane_bitmap)
{
    uint8 rs_fec_mpp0, rs_fec_mpp1, rs272_mpp0, rs272_mpp1;
    SOC_INIT_FUNC_DEFS;

    /* rs_fec_mpp0, rs_fec_mpp1 contains lane bitmap for
     * the RS FECs which can not co-exist with RS272.
     * For 16nm, rsfec contains bitmap for both RS544 and RS528.
     * For 7nm, rsfec contains bitmap for RS544 only.
     */
    if (PM8X50_PM_VERSION_F_REV0_16NM_GET(flags)) {
        rs_fec_mpp0 = (rs528_bitmap | rs544_bitmap) & 0xf;
        rs_fec_mpp1 = (rs528_bitmap | rs544_bitmap) & 0xf0;
    } else {
        rs_fec_mpp0 = rs544_bitmap & 0xf;
        rs_fec_mpp1 = rs544_bitmap & 0xf0;
    }
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

/*Get the suggested VCO values based on the speed config list*/
int pm8x50_pm_vcos_get(int unit,
                       portmod_dispatch_type_t pm_type,
                       uint32 flags,
                       portmod_pm_vco_setting_t* vco_select)
{
    pm8x50_vcos_bmp_t vcos = 0, vco_bmp = 0;
    portmod_vco_type_t current_tvco, current_ovco;
    uint8_t rs528_bm = 0, rs544_bm = 0, rs272_bm = 0, affected_bm = 0;
    portmod_vco_type_t vco;
    int i, rv = 0;
    int is_pcs_bypass;
    uint32 pm_version_flag = 0, vcos_flags = 0;

    SOC_INIT_FUNC_DEFS;

    if(vco_select == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("vco_select NULL paramaeter")));
    }

    if (PORTMOD_PM8X50_REV0_16NM_GET(flags)) {
        PM8X50_PM_VERSION_F_REV0_16NM_SET(pm_version_flag);
    }
    for (i = 0; i < vco_select->num_speeds; i++) {
        vco = portmodVCOInvalid;
        rv = _pm8x50_port_speed_config_to_vco_get(&(vco_select->speed_config_list[i]),
                                                  vco_select->speed_for_pcs_bypass_port[i],
                                                  pm_version_flag, &vco);
        if (rv == SOC_E_CONFIG) {
            _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                      (_SOC_MSG("Speed config is not supported")));
        }
        is_pcs_bypass = vco_select->speed_for_pcs_bypass_port[i];
        _SOC_IF_ERR_EXIT(_pm8x50_required_vco_bitmap_get(unit, vco, is_pcs_bypass, &vco_bmp));
        vcos |= vco_bmp;

        if (!is_pcs_bypass) {
            if ((vco_select->speed_config_list[i].fec == PORTMOD_PORT_PHY_FEC_RS_272) ||
                (vco_select->speed_config_list[i].fec == PORTMOD_PORT_PHY_FEC_RS_272_2XN)) {
                _SOC_IF_ERR_EXIT(_pm8x50_lanebitmap_set(vco_select->port_starting_lane_list[i],
                                                        vco_select->speed_config_list[i].num_lane,
                                                        &rs272_bm));
            } else if ((vco_select->speed_config_list[i].fec == PORTMOD_PORT_PHY_FEC_RS_544) ||
                       (vco_select->speed_config_list[i].fec == PORTMOD_PORT_PHY_FEC_RS_544_2XN)) {
                _SOC_IF_ERR_EXIT(_pm8x50_lanebitmap_set(vco_select->port_starting_lane_list[i],
                                                        vco_select->speed_config_list[i].num_lane,
                                                        &rs544_bm));
            } else if (vco_select->speed_config_list[i].fec == PORTMOD_PORT_PHY_FEC_RS_FEC) {
                _SOC_IF_ERR_EXIT(_pm8x50_lanebitmap_set(vco_select->port_starting_lane_list[i],
                                                        vco_select->speed_config_list[i].num_lane,
                                                        &rs528_bm));
            }
        }
    }

    /* Validate FEC settings for RS528, RS544 and RS272 */
    _SOC_IF_ERR_EXIT(_pm8x50_fec_validate(unit,
                                          pm_version_flag,
                                          rs528_bm,
                                          rs544_bm,
                                          rs272_bm,
                                          &affected_bm));

    current_tvco = portmodVCOInvalid;
    current_ovco = portmodVCOInvalid;

    if (PORTMOD_ALLOW_20P625G_TVCO_GET(flags)) {
        PM8X50_TVCO_SUPPORT_20P625G_SET(vcos_flags);
    }

    if (PORTMOD_PRIORITIZE_26P562G_TVCO_GET(flags)) {
        PM8X50_TVCO_PRIORITIZE_26P562G_SET(vcos_flags);
    }

    /* Try to allocate required VCOs */
    _SOC_IF_ERR_EXIT(
            pm8x50_vcos_request_allocate(unit, vcos_flags, vcos, &current_tvco, &current_ovco));

    /* Set Output */
    vco_select->tvco = current_tvco;
    vco_select->ovco = current_ovco;
    vco_select->is_tvco_new = (current_tvco == portmodVCOInvalid) ? 0 : 1;
    vco_select->is_ovco_new = (current_ovco == portmodVCOInvalid) ? 0 : 1;

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm8x50_port_tsc_reset_set(int unit, pm_info_t pm_info, int phy_acc, int tvco_pll_index, int in_reset)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDPORT_XGXS0_CTRL_REGr(unit, phy_acc, &reg_val));
    soc_reg_field_set(unit, CDPORT_XGXS0_CTRL_REGr, &reg_val, TSC_CLK_SELf, tvco_pll_index);

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
        if (!(PM_8x50_INFO(pm_info)->is_master_pm)) {
            /* Put TSC into reset for slave PMs */
            soc_reg_field_set(unit, CDPORT_XGXS0_CTRL_REGr, &reg_val, TSC_RSTBf, 0);
        }
        _SOC_IF_ERR_EXIT(WRITE_CDPORT_XGXS0_CTRL_REGr(unit, phy_acc, reg_val));

        /* clear the warm boot variable */
        _SOC_IF_ERR_EXIT(PM8x50_IS_CORE_INITIALIZED_SET(unit, pm_info, is_core_initialized));
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
int pm8x50_pm_enable(int unit,
                     int pm_id,
                     pm_info_t pm_info,
                     int flags,
                     int enable)
{
    uint32 reg_val;
    int phy_acc;
    int is_reset;
    int tvco_pll_index;
    uint32_t pm8x50_gen2_flag = 0;
    pm_version_t version;
    int skip_mac, mac_only;

    SOC_INIT_FUNC_DEFS;

    /* Get the first physical port of the pm core */
    phy_acc = PM_8x50_INFO(pm_info)->first_phy | SOC_REG_ADDR_PHY_ACC_MASK;

    _SOC_IF_ERR_EXIT(READ_CDPORT_MAC_CONTROLr(unit, phy_acc, &reg_val));
    is_reset = soc_reg_field_get(unit, CDPORT_MAC_CONTROLr, reg_val, CDMAC0_RESETf);

    tvco_pll_index = PM8X50_TVCO_PLL_INDEX_GET(pm_info);

    _SOC_IF_ERR_EXIT(_pm8x50_pm_version_get(unit, pm_info, &version));
    if ((version.tech_process != PM_TECH_16NM) || (version.type == PM_TYPE_PM8X50_FLEXE)) {
        pm8x50_gen2_flag = 1;
    }

    skip_mac = PORTMOD_PM_ENABLE_SKIP_MAC_GET(flags);
    mac_only = PORTMOD_PM_ENABLE_MAC_ONLY_GET(flags);
    if(enable && is_reset) {
        if (!mac_only) {
            /* Bring Serdes OOR */
            _SOC_IF_ERR_EXIT(_pm8x50_port_tsc_reset_set(unit, pm_info, phy_acc, tvco_pll_index, 0));
        }
        if (!skip_mac) {            /* Bring MAC OOR */
            _SOC_IF_ERR_EXIT(READ_CDPORT_MAC_CONTROLr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CDPORT_MAC_CONTROLr, &reg_val, CDMAC0_RESETf, 0);
            if (!pm8x50_gen2_flag) {
                soc_reg_field_set(unit, CDPORT_MAC_CONTROLr, &reg_val, CDMAC1_RESETf, 0);
            }
            _SOC_IF_ERR_EXIT(WRITE_CDPORT_MAC_CONTROLr(unit, phy_acc, reg_val));
        }
    } else if ((!enable) && (!is_reset)){ /* disable */
        if (!skip_mac) {
            /* put MAC in reset */
            _SOC_IF_ERR_EXIT(READ_CDPORT_MAC_CONTROLr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CDPORT_MAC_CONTROLr, &reg_val, CDMAC0_RESETf, 1);
            if (!pm8x50_gen2_flag) {
                soc_reg_field_set(unit, CDPORT_MAC_CONTROLr, &reg_val, CDMAC1_RESETf, 1);
            }
            _SOC_IF_ERR_EXIT(WRITE_CDPORT_MAC_CONTROLr(unit, phy_acc, reg_val));
        }
        if (!mac_only) {
            /* If current PM is not master PM, put Serdes into reset*/
            _SOC_IF_ERR_EXIT(_pm8x50_port_tsc_reset_set(unit, pm_info, phy_acc, tvco_pll_index, 1));
        }
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm8x50_pm_core_probe(int unit, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int probe =0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_common_serdes_probe(pm8x50_serdes_list, &PM_8x50_INFO(pm_info)->int_core_access, &probe));

exit:
    SOC_FUNC_RETURN;
}

/* This function will return required pll based on the ref_clk and vco */
/* For now, assume PM8x50 only support:
   VCO: 20.625G, 25.78125G,26.5625G;
   Ref_CLK: 156.25M, 312.5M
  For any other input, this function will return E_PARAM
*/
STATIC
int _pm8x50_vco_to_pll_get(phymod_ref_clk_t ref_clock, portmod_vco_type_t vco, uint32_t* pll)
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
                *pll = phymod_TSCBH_PLL_DIV165;
            } else if (ref_clock == phymodRefClk312Mhz) {
                *pll = phymod_TSCBH_PLL_DIV82P5;
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case portmodVCO26P562G:
            if (ref_clock == phymodRefClk156Mhz) {
                *pll = phymod_TSCBH_PLL_DIV170;
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
int _pm8x50_pm_serdes_core_init(int unit, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int rv, tvco_pll_index;
    phymod_core_init_config_t core_conf;
    phymod_core_status_t core_status;
    uint32_t core_is_initialized;
    uint32_t init_flags = 0;
    uint32_t pll0 = 0, pll1 = 0;
    uint32_t tx_lane_map = 0, rx_lane_map = 0, i;
    portmod_vco_type_t ovco = 0, tvco = 0;
    phymod_phy_access_t phy_access;

    SOC_INIT_FUNC_DEFS;

    rv = PM8x50_IS_CORE_INITIALIZED_GET(unit, pm_info, &core_is_initialized)
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(phymod_core_init_config_t_init(&core_conf));
    tvco_pll_index = PM8X50_TVCO_PLL_INDEX_GET(pm_info);

    
    core_conf.firmware_load_method = PM_8x50_INFO(pm_info)->fw_load_method;
    core_conf.firmware_loader = PM_8x50_INFO(pm_info)->external_fw_loader;
    core_conf.lane_map = PM_8x50_INFO(pm_info)->lane_map;
    core_conf.polarity_map = PM_8x50_INFO(pm_info)->polarity;

    /* next translate the lane map into encoded format for RLM usage */
    for (i = 0; i < core_conf.lane_map.num_of_lanes; i++) {
        tx_lane_map |= core_conf.lane_map.lane_map_tx[i] << (4 * i);
        rx_lane_map |= core_conf.lane_map.lane_map_rx[i] << (4 * i);
    }

    core_conf.afe_pll.afe_pll_change_default = PM_8x50_INFO(pm_info)->afe_pll.afe_pll_change_default;
    core_conf.afe_pll.ams_pll_iqp = PM_8x50_INFO(pm_info)->afe_pll.ams_pll_iqp;
    core_conf.afe_pll.ams_pll_en_hrz = PM_8x50_INFO(pm_info)->afe_pll.ams_pll_en_hrz;

    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&core_conf.interface));
    core_conf.interface.ref_clock = PM_8x50_INFO(pm_info)->ref_clk;

    if (PORTMOD_PORT_ADD_F_SKIP_SPEED_INIT_GET(add_info)) {
        /* If PORTMOD_PORT_ADD_F_SKIP_SPEED_INIT flag is set, do not bring up PLLs */
        if (!PORTMOD_CORE_INIT_FLAG_INITIALZIED_GET(core_is_initialized)) {
            /* Only if core is not initilized - put tvco and ovco as invalid */
            ovco = portmodVCOInvalid;
            tvco = portmodVCOInvalid;
            PM_8x50_INFO(pm_info)->ovco = ovco;
            PM_8x50_INFO(pm_info)->tvco = tvco;
        }
    } else if ((PM_8x50_INFO(pm_info)->ovco == portmodVCOInvalid) && (PM_8x50_INFO(pm_info)->tvco == portmodVCOInvalid)) {
        ovco = add_info->ovco;
        tvco = add_info->tvco;
        PM_8x50_INFO(pm_info)->ovco = ovco;
        PM_8x50_INFO(pm_info)->tvco = tvco;
    } else {
        ovco = PM_8x50_INFO(pm_info)->ovco;
        tvco = PM_8x50_INFO(pm_info)->tvco;
    }
    if ((tvco < portmodVCOInvalid) || (tvco >= portmodVCOCount)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("TVCO configuration is invalid.")));
    }

    if (tvco_pll_index == 1) {
        _SOC_IF_ERR_EXIT(_pm8x50_vco_to_pll_get(core_conf.interface.ref_clock, tvco, &pll1));
        _SOC_IF_ERR_EXIT(_pm8x50_vco_to_pll_get(core_conf.interface.ref_clock, ovco, &pll0));
    } else {
        _SOC_IF_ERR_EXIT(_pm8x50_vco_to_pll_get(core_conf.interface.ref_clock, tvco, &pll0));
        _SOC_IF_ERR_EXIT(_pm8x50_vco_to_pll_get(core_conf.interface.ref_clock, ovco, &pll1));
    }

    core_conf.pll0_div_init_value = pll0;
    core_conf.pll1_div_init_value = pll1;

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
#if 0
            
            if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) || init_flags == 0) {
                if (PM_8x50_INFO(pm_info)->rescal != -1) {
                    sal_memcpy(&serdes_access, &(PM_8x50_INFO(pm_info)->int_core_access), sizeof(phymod_phy_access_t));
                    _SOC_IF_ERR_EXIT(phymod_phy_rescal_set(&serdes_access, 1, PM_8x50_INFO(pm_info)->rescal));
                }
            }
#endif
             _SOC_IF_ERR_EXIT(phymod_core_init(&PM_8x50_INFO(pm_info)->int_core_access,
                                               &core_conf,
                                               &core_status));
            /* update PLL0 rx clock repeater buffer bandwidth, for DNX device only */
            if (PM_8x50_INFO(pm_info)->clock_buffer_bw_update_required) {
                sal_memcpy(&phy_access, &PM_8x50_INFO(pm_info)->int_core_access,
                       sizeof(phymod_phy_access_t));
                if (((tvco_pll_index == 0) && (tvco != portmodVCOInvalid)) ||
                    ((tvco_pll_index == 1) && (ovco != portmodVCOInvalid))) {
                    _SOC_IF_ERR_EXIT(phymod_phy_pll_rx_clock_buffer_bw_update(&phy_access, 0/* pll0 */,
                        pll0, PM_8x50_INFO(pm_info)->ref_clk));
                }
            }

            if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) || init_flags == 0) {
                PORTMOD_CORE_INIT_FLAG_FIRMWARE_LOADED_SET(core_is_initialized);
                rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                                            pm_info->wb_vars_ids[isCoreInitialized],
                                            &core_is_initialized);
                _SOC_IF_ERR_EXIT(rv);

                rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                                            pm_info->wb_vars_ids[originalTxLaneSwap],
                                            &tx_lane_map);
                _SOC_IF_ERR_EXIT(rv);

                rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                                            pm_info->wb_vars_ids[originalRxLaneSwap],
                                            &rx_lane_map);
                _SOC_IF_ERR_EXIT(rv);
            }
            if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info) || init_flags == 0) {
                /* Program PM offset at the end of core initialization. */
                _SOC_IF_ERR_EXIT
                    (phymod_timesync_offset_set(&PM_8x50_INFO(pm_info)->int_core_access,
                                                PM_8x50_INFO(pm_info)->pm_offset));

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
int pm8x50_pm_serdes_core_init(int unit,
                               int pm_id,
                               pm_info_t pm_info,
                               const portmod_port_add_info_t* add_info)
{
    int init_all = 0;
    int rv = SOC_E_NONE;

    SOC_INIT_FUNC_DEFS;

    /* Update per core based database */
    if (add_info->init_config.polarity_overwrite == 1) {
        sal_memcpy(&PM_8x50_INFO(pm_info)->polarity,
            &add_info->init_config.polarity[0], sizeof(phymod_polarity_t));
    }
    if (add_info->init_config.lane_map_overwrite == 1) {
        sal_memcpy(&PM_8x50_INFO(pm_info)->lane_map,
            &add_info->init_config.lane_map[0], sizeof(phymod_lane_map_t));
    }
    if (add_info->init_config.fw_load_method_overwrite == 1) {
        PM_8x50_INFO(pm_info)->fw_load_method =
            add_info->init_config.fw_load_method[0];
    }
    if (add_info->init_config.ref_clk_overwrite == 1) {
        PM_8x50_INFO(pm_info)->ref_clk = add_info->init_config.ref_clk;
    }

    init_all = (!PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) &&
                !PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) &&
                !PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) ? 1 : 0;

    /* probe serdes core */
    if (PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) || init_all) {
        rv = _pm8x50_pm_core_probe(unit, pm_info, add_info);
        _SOC_IF_ERR_EXIT(rv);
    }

    /* Return here if caller only request Core Probe */
    if (!(PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) &&
        (PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info))) {
        return (rv);
    }

    /* core config for internal serdes. */
    rv = _pm8x50_pm_serdes_core_init(unit, pm_info, add_info);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm8x50_pm_port_init(int unit,
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

    _SOC_IF_ERR_EXIT(_pm8x50_phy_access_get(unit, port, pm_info, &phy_acc));
    if (internal_port == -1)  {
        _SOC_EXIT_WITH_ERR(SOC_E_PORT, (_SOC_MSG("Invalid internal Port %d"),
                                internal_port));
    }

    if (enable) {
        _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, bitmap));
        _SOC_IF_ERR_EXIT(PM8x50_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));
        if (!is_bypassed)
        {
            /* RSV Mask */
            rsv_mask = 0;
            SHR_BITSET(&rsv_mask, 3); /* Receive terminate/code error */
            SHR_BITSET(&rsv_mask, 4); /* CRC error */
            SHR_BITSET(&rsv_mask, 6); /* Truncated/Frame out of Range */
            SHR_BITSET(&rsv_mask, 17); /* RUNT detected*/
            _SOC_IF_ERR_EXIT(pm8x50_port_mac_rsv_mask_set(unit, phy_acc, pm_info, rsv_mask));
            
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
            _SOC_IF_ERR_EXIT(pm8x50_port_cntmaxsize_set(unit, phy_acc, pm_info, 1518));
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
 *          pm8x50_port_soft_reset(unit, port, pinfo, portmodMacSoftResetModeIn, &saved_rx_enablel, &saved_mac_ctrl);
 *          ... here the MAC is in reset ...
 *          pm8x50_port_soft_reset(unit, port, pinfo, portmodMacSoftResetModeOut, &saved_rx_enablel, &saved_mac_ctrl);
 *     IN-OUT - get MAC IN reset and then out of reset. 
 *          When  reset_mode=INOUT, saved_rx_enable, saved_mac_ctrl can be set to NULL.
 */
int pm8x50_port_soft_reset(int unit,
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
    SOC_INIT_FUNC_DEFS;

    /* Callback soft reset function */
    if (PM_8x50_INFO(pm_info)->portmod_mac_soft_reset) {
        rv = PM_8x50_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypePre);
        _SOC_IF_ERR_EXIT(rv);
    }

    if (portmodMacSoftResetModeOut != reset_mode) {
        rv = cdmac_egress_queue_drain_get(unit, port, &mac_ctrl, &rx_enable);
        _SOC_IF_ERR_EXIT(rv);

        /* return MAC state if required */
        if ((NULL != saved_rx_enable) && (NULL != saved_mac_ctrl)) {
            *saved_rx_enable = rx_enable;
            COMPILER_64_COPY(*saved_mac_ctrl, mac_ctrl);
        }

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
    }

    if(portmodMacSoftResetModeIn != reset_mode) {
        if (PM_8x50_INFO(pm_info)->portmod_mac_soft_reset) {
            rv = PM_8x50_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypeDuring);
            _SOC_IF_ERR_EXIT(rv);
        }

        /* For out of reset only operation, use MAC state from input */
        if (portmodMacSoftResetModeOut == reset_mode) {
            SOC_NULL_CHECK(saved_rx_enable);
            SOC_NULL_CHECK(saved_mac_ctrl);
            rx_enable = *saved_rx_enable;
            COMPILER_64_COPY(mac_ctrl, *saved_mac_ctrl);
        }

        soc_reg64_field32_set(unit, CDMAC_CTRLr, &mac_ctrl, SOFT_RESETf, 0); /*make sure restored data wont put mac back in reset*/

        /* Enable TX, restore RX, de-assert SOFT_RESET */
        rv = cdmac_egress_queue_drain_rx_en(unit, port, rx_enable);
        _SOC_IF_ERR_EXIT(rv);

        /* Restore clmac_CTRL to original value */
        rv = cdmac_mac_ctrl_set(unit, port, mac_ctrl);
        _SOC_IF_ERR_EXIT(rv);

        /* Callback soft reset function */
        if (PM_8x50_INFO(pm_info)->portmod_mac_soft_reset) {
            rv = PM_8x50_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypePost);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

exit:
   SOC_FUNC_RETURN;
}

STATIC
int _pm8x50_port_mac_drain_soft_reset(int unit, int port)
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
int _pm8x50_port_rx_restore_mac_out_of_reset(int unit, int port, int rx_enable)
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
 * Function:
 * _pm8x50_port_clock_repeater_buffer_check
 *
 * Background Info:
 *      PLL0 is driving lanes 0-3 directly, and lanes 4-7 through clock repeater.
 *      PLL1 is driving lanes 4-7 directly, and lanes 0-3 through clock repeater.
 * Purpose:
 *      This function will check PLL0/1 TX and RX Clock repeater buffer
 *      could be powered down.
 *      Function calculate,
 *      - txMask: logical lane tx mask corresponding to physical lane 0-3 or 4-7
 *      - rxMask: logical lane rx mask corresponding to physical lane 0-3 or 4-7
 *      and then compare with the lane usage of current PLL,
 *      to decide if tx or rx clock repeater buffer could be powered down.
 *
 * Inputs:
 *     pm_info:              PM info struct
 *     pll_index:            PLL index, 0 or 1
 *     lanes_bitmap:         indicate the logical lane usage of current PLL
 * Output:
 *     tx_clk_rptr_dis_req:  1 means tx clock buffer could be powered down
 *     rx_clk_rptr_dis_req:  1 means rx clock buffer could be powered down
 */
STATIC 
int _pm8x50_port_clock_repeater_buffer_check(pm_info_t pm_info,
                                             int pll_index, uint8 lanes_bitmap,
                                             int *tx_clk_rptr_dis_req,
                                             int *rx_clk_rptr_dis_req)
{
    phymod_lane_map_t lane_map;
    uint8 rxMask = 0, txMask = 0;
    uint32_t logical_lane;

    PHYMOD_IF_ERR_RETURN
        (phymod_core_lane_map_get(&PM_8x50_INFO(pm_info)->int_core_access, &lane_map));

    /* PLL1, find corresponding logical lane for physical lane 0-3, and then calculate bitmap */
    if (pll_index == 1) {
        for (logical_lane = 0; logical_lane < 8; logical_lane++){
            if (lane_map.lane_map_tx[logical_lane] >= 4) {
                continue;
            } else {
                txMask |= 0x1 << logical_lane;
            }
        }
        for (logical_lane = 0; logical_lane < 8; logical_lane++){
            if (lane_map.lane_map_rx[logical_lane] >= 4) {
                continue;
            } else {
                rxMask |= 0x1 << logical_lane;
            }
        }
    }
    /* PLL0, find corresponding logical lane for physical lane 4-7, and then calculate bitmap */
    if (pll_index == 0) {
        for (logical_lane = 0; logical_lane < 8; logical_lane++){
            if (lane_map.lane_map_tx[logical_lane] <= 3) {
                continue;
            } else {
                txMask |= 0x1 << logical_lane;
            }
        }
        for (logical_lane = 0; logical_lane < 8; logical_lane++){
            if (lane_map.lane_map_rx[logical_lane] <= 3) {
                continue;
            } else {
                rxMask |= 0x1 << logical_lane;
            }
        }
    }

    *tx_clk_rptr_dis_req = 0;
    *rx_clk_rptr_dis_req = 0;
    if ((lanes_bitmap & txMask) == 0) {
        *tx_clk_rptr_dis_req = 1;
    }
    if ((lanes_bitmap & rxMask) == 0) {
        *rx_clk_rptr_dis_req = 1;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 * _pm8x50_pll_clock_repeater_buffer_pwrdn
 *
 * Background Info:
 *      PLL0 is driving lanes 0-3 directly, and lanes 4-7 through clock repeater.
 *      PLL1 is driving lanes 4-7 directly, and lanes 0-3 through clock repeater.
 * Purpose:
 *      This function will power down unused TX or RX clock repeater buffer
 * Inputs:
 *     unit:                 unit number
 *     pm_info:              PM info struct
 *     phy_access:           phy_access struct
 *     pll_index:            PLL index, 0 or 1
 *     pll_lanes_bitmap:     lane usage bitmap for PLL
 * Note:
 *     This function is used for DNX device only
 */
STATIC 
int _pm8x50_pll_clock_repeater_buffer_pwrdn(int unit, pm_info_t pm_info,
                                            const phymod_phy_access_t* phy_access,
                                            int pll_index,
                                            uint8 pll_lanes_bitmap)
{
    int tx_disable_required = 0, rx_disable_required = 0;

    SOC_INIT_FUNC_DEFS;

    /* disable PLL0 clock repeater buffer, if PLL0 is not used by lanes 4-7 */
    /* disable PLL1 clock repeater buffer, if PLL1 is not used by lanes 0-3 */
    _SOC_IF_ERR_EXIT(_pm8x50_port_clock_repeater_buffer_check(pm_info, pll_index,
        pll_lanes_bitmap, &tx_disable_required, &rx_disable_required));
    PHYMOD_IF_ERR_RETURN
        (phymod_phy_pll_clock_buffer_pwrdn(phy_access, pll_index, tx_disable_required, rx_disable_required));

exit:
    SOC_FUNC_RETURN;
}

/*
 * 50Gx2lane RS528/RS544 will be treated as a single lane port in FLEXE mode
 *
 *                                              tdm_mode
 * ------------------------------------------------------------
 * Lane 0&1: 50Gx2, RS528/RS544(FlexE)           QUAD
 * Lane 2&3: 50Gx2, RS528/RS544(FlexE)
 *
 * Lane 0&1: 50Gx2, RS528/RS544(FlexE)           TRI_012
 * Lane 2&3: 2lane mode(100Gx2)
 *
 * Lane 0&1: 2lane mode(100Gx2)                  TRI_023
 * Lane 2&3: 50Gx2, RS528/RS544(FlexE)
 *
 * Lane 0&1: 50Gx2, RS528/RS544(FlexE)           QUAD
 * Lane 2&3: EMPTY or 1lane mode
 *
 * Lane 0&1: EMPTY or 1lane mode                 QUAD
 * Lane 2&3: 50Gx2, RS528/RS544(FlexE)
 */
STATIC
int _pm8x50_tdm_mode_update(int unit, int port, pm_info_t pm_info, int is_detached)
{
    int lane_0_1_in_50gx2_flexe = 0, lane_2_3_in_50gx2_flexe = 0, temp_port;
    int lane_0_1_in_2lane_port = 0, lane_2_3_in_2lane_port = 0;
    uint32_t tdm_mode_en, tdm_mode;
    portmod_speed_config_t speed_config;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access, phy_copy;
    int nof_phys, mpp_index;
    int port_flexe_mode = 0;
    int port_index, port_num_lanes = 0;
    uint32 bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    sal_memcpy(&phy_copy, &phy_access, sizeof(phy_copy));
    PORTMOD_BIT_COUNT(phy_copy.access.lane_mask, port_num_lanes);

    /* set MPP index properly */
    if (phy_access.access.lane_mask & 0xf) {
        mpp_index = 0;
    } else {
        mpp_index = 1;
    }

    /* if lane 0 & 1 is in 2lane mode  */
    _SOC_IF_ERR_EXIT
        (PM8x50_LANE2PORT_GET(unit, pm_info, (mpp_index * 4), &temp_port));
    /* get speed_config */
    sal_memset(&speed_config, 0, sizeof(portmod_speed_config_t));
    if ((temp_port > 0) && (port_num_lanes <= 2)) {
        if ((temp_port == port) && is_detached) {
            lane_0_1_in_50gx2_flexe = 0;
        } else {
            _SOC_IF_ERR_EXIT(pm8x50_port_speed_config_get(unit,
                temp_port, pm_info, &speed_config));

            /* get port index */
            _SOC_IF_ERR_EXIT
                (_pm8x50_port_index_get(unit,
                    temp_port, pm_info, &port_index, &bitmap));
            _SOC_IF_ERR_EXIT
                (PM8x50_PORT_FLEXE_MODE_GET(unit,
                    pm_info, &port_flexe_mode, port_index));
            /* check if lane 0 & 1 is in 50Gx2 RS528/544 */
            if ((speed_config.speed == 50000) &&
                (speed_config.num_lane == 2) &&
                ((speed_config.fec == PORTMOD_PORT_PHY_FEC_RS_FEC) ||
                 (speed_config.fec == PORTMOD_PORT_PHY_FEC_RS_544)) &&
                (port_flexe_mode == 1)) {
                lane_0_1_in_50gx2_flexe = 1;
            } else if (speed_config.num_lane == 2) {
                /* lane 0 & 1 is two lane port*/
               lane_0_1_in_2lane_port = 1;
            }
        }
    }

    /* if lane 2 & 3 is in 2lane mode  */
    _SOC_IF_ERR_EXIT
        (PM8x50_LANE2PORT_GET(unit, pm_info, (mpp_index * 4 + 2), &temp_port));
    /* get speed_config */
    sal_memset(&speed_config, 0, sizeof(portmod_speed_config_t));
    if ((temp_port > 0) && (port_num_lanes <= 2)) {
        if ((temp_port == port) && is_detached) {
            lane_2_3_in_50gx2_flexe = 0;
        } else {
            _SOC_IF_ERR_EXIT(pm8x50_port_speed_config_get(unit,
                temp_port, pm_info, &speed_config));

            /* get port index */
            _SOC_IF_ERR_EXIT
                (_pm8x50_port_index_get(unit,
                    temp_port, pm_info, &port_index, &bitmap));
            _SOC_IF_ERR_EXIT
                (PM8x50_PORT_FLEXE_MODE_GET(unit,
                    pm_info, &port_flexe_mode, port_index));
            /* check if lane 2 & 3 is in 50Gx2 RS528/544 */
            if ((speed_config.speed == 50000) &&
                (speed_config.num_lane == 2) &&
                ((speed_config.fec == PORTMOD_PORT_PHY_FEC_RS_FEC) ||
                 (speed_config.fec == PORTMOD_PORT_PHY_FEC_RS_544)) &&
                (port_flexe_mode == 1)) {
                lane_2_3_in_50gx2_flexe = 1;
             } else if (speed_config.num_lane == 2) {
                lane_2_3_in_2lane_port = 1;
            }
        }
    }

    /* default value */
    tdm_mode_en = 0;
    tdm_mode = 1;

    /* 50Gx2lane RS528/RS544 will be treated as a single lane port */
    if ( (lane_0_1_in_50gx2_flexe) && (lane_2_3_in_50gx2_flexe)) {
        tdm_mode_en = 1;
        tdm_mode = portmodPortModeQuad;
    } else if (lane_0_1_in_50gx2_flexe) {
        if (lane_2_3_in_2lane_port == 1) {
            tdm_mode_en = 1;
            tdm_mode = 1; /* Tri012 */
        } else {
            tdm_mode_en = 1;
            tdm_mode = portmodPortModeQuad;
        }
    } else if (lane_2_3_in_50gx2_flexe) {
        if (lane_0_1_in_2lane_port == 1) {
            tdm_mode_en = 1;
            tdm_mode = 2; /* Tri023 */
        } else {
            tdm_mode_en = 1;
            tdm_mode = portmodPortModeQuad;
        }
    }

    /* set tdm mode */
    sal_memcpy(&phy_copy, &phy_access, sizeof(phy_copy));
    _SOC_IF_ERR_EXIT
        (phymod_phy_tdm_mode_update(&phy_copy, tdm_mode_en, tdm_mode));

exit:
    SOC_FUNC_RETURN;
}

/*Add new port*/
int pm8x50_port_attach(int unit, int port, pm_info_t pm_info,
                       const portmod_port_add_info_t* add_info)
{
    int port_index = -1, pm_is_active = 0, is_pcs_bypassed = 0;
    int port_flexe_mode = 0, is_flexe_tvco_26g_required = 0;
    int rv = 0, port_i, my_i, found_first_active_lane;
    int phy, ii, nof_phys;
    portmod_pbmp_t port_phys_in_pm;
    phymod_phy_init_config_t init_config;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    phymod_phy_signalling_method_t signalling_mode;
    phymod_an_mode_type_t init_an_mode = phymod_AN_MODE_NONE;
    const portmod_speed_config_t *speed_config = &add_info->speed_config;
    int usr_cfg_idx, rlm_enabled = 0, rlm_state = PORTMOD_PORT_RLM_FSM_COUNT;
    uint32 timesync_enable_default = 0;
    SOC_INIT_FUNC_DEFS;

    /* Get the first phy related to this port */
    PORTMOD_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
    PORTMOD_PBMP_AND(port_phys_in_pm, PM_8x50_INFO(pm_info)->phys);

    ii = 0;
    PORTMOD_PBMP_ITER(PM_8x50_INFO(pm_info)->phys, phy) {
        if(PORTMOD_PBMP_MEMBER(port_phys_in_pm, phy)) {
            rv = PM8x50_LANE2PORT_SET(unit, pm_info, ii, port);
            _SOC_IF_ERR_EXIT(rv);
            port_index = (port_index == -1 ? ii : port_index);
        }
        ii++;
    }

    is_pcs_bypassed = PORTMOD_PORT_ADD_F_PCS_BYPASSED_GET(add_info) ? 1 : 0;
    rv = PM8x50_PORT_IS_PCS_BYPASSED_SET(unit, pm_info, is_pcs_bypassed, port_index);
    _SOC_IF_ERR_EXIT(rv);

    port_flexe_mode = PORTMOD_PORT_ADD_F_FLEXE_CONFIG_GET(add_info) ? 1 : 0;
    rv = PM8x50_PORT_FLEXE_MODE_SET(unit, pm_info, port_flexe_mode, port_index);
    _SOC_IF_ERR_EXIT(rv);

    is_flexe_tvco_26g_required =
        PORTMOD_PORT_ADD_F_FLEXE_TVCO_26G_CONFIG_GET(add_info) ? 1 : 0;
    rv = PM8x50_IS_FLEXE_TVCO_26G_REQUIRED_SET(unit, pm_info, is_flexe_tvco_26g_required);
    _SOC_IF_ERR_EXIT(rv);

    /* initalize port */
    rv = _pm8x50_pm_port_init(unit, port, pm_info, port_index, add_info, 1);
    _SOC_IF_ERR_EXIT(rv);

    /* initialze phy */
    _SOC_IF_ERR_EXIT(phymod_phy_init_config_t_init(&init_config));
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    
    init_config.an_en = add_info->autoneg_en;
    init_config.cl72_en = add_info->link_training_en;
    init_config.op_mode = add_info->interface_config.port_op_mode;
    my_i = 0, usr_cfg_idx = 0;
    found_first_active_lane = 0;
    for(ii=0; ii<PM8X50_LANES_PER_CORE; ii++) {
        rv = PM8x50_LANE2PORT_GET(unit, pm_info, ii, &port_i);
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
            _pm8x50_signalling_method_get(is_pcs_bypassed, speed_config, &signalling_mode);
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

    _SOC_IF_ERR_EXIT(phymod_phy_init(&phy_access, &init_config));

    /* MAC SOFT_RESET will be released within pm8x50_port_speed_config_set */
    if (!PORTMOD_PORT_ADD_F_SKIP_SPEED_INIT_GET(add_info)) {
        rv = pm8x50_port_speed_config_set(unit, port, pm_info, speed_config);
    }
    else if (!is_pcs_bypassed)
    {
       _SOC_IF_ERR_EXIT(pm8x50_port_soft_reset(unit, port, pm_info, portmodMacSoftResetModeIn_Out, NULL, NULL));
    }

    _SOC_IF_ERR_EXIT(rv);

    /* Update WB DB */
    rv = PM8x50_IS_ACTIVE_GET(unit, pm_info, &pm_is_active);
    _SOC_IF_ERR_EXIT(rv);
    if (!pm_is_active) {
        pm_is_active = 1;
        rv = PM8x50_IS_ACTIVE_SET(unit, pm_info, pm_is_active);
        _SOC_IF_ERR_EXIT(rv);
    }

    rv = PM8x50_AN_MODE_SET(unit, pm_info, init_an_mode, port_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = PM8x50_MAX_SPEED_SET(unit, pm_info, add_info->interface_config.max_speed, port_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = PM8x50_TIMESYNC_ENABLE_SET(unit, pm_info, timesync_enable_default, port_index);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT
        (PM8x50_RLM_ENABLE_SET(unit, pm_info, rlm_enabled, port_index));

    _SOC_IF_ERR_EXIT
        (PM8x50_RLM_STATE_SET(unit, pm_info, rlm_state, port_index));

    if (add_info->init_config.parity_enable) {
        _pm8x50_port_interrupt_all_enable_set(unit, port, 1);

    }

exit:
    SOC_FUNC_RETURN;
}

/*get port cores' phymod access*/
int pm8x50_pm_core_info_get(int unit, pm_info_t pm_info, int phyn,
                            portmod_pm_core_info_t* core_info)
{
    SOC_INIT_FUNC_DEFS;

    if(phyn < 0) {
        phyn = PM8X50_MAX_NUM_PHYS;
    }

    core_info->ref_clk = PM_8x50_INFO(pm_info)->ref_clk;
    sal_memcpy(&core_info->lane_map, &(PM_8x50_INFO(pm_info)->lane_map),
               sizeof(phymod_lane_map_t));

    SOC_FUNC_RETURN;
}

/*Get PM phys.*/
int pm8x50_pm_phys_get(int unit, pm_info_t pm_info, portmod_pbmp_t* phys)
{
    SOC_INIT_FUNC_DEFS;
    PORTMOD_PBMP_ASSIGN(*phys, PM_8x50_INFO(pm_info)->phys);
    SOC_FUNC_RETURN;
}

int pm8x50_port_callback_unregister(int unit, int port, pm_info_t pm_info)
{
    int port_index;
    uint32 bitmap;
    int is_3_cw_bad_enable = 0, is_400g_an = 0;
    int is_ber_war_enable = 0, rlm_enabled = 0;
    phymod_an_mode_type_t an_mode;

    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, &bitmap));

    _SOC_IF_ERR_EXIT(PM8x50_3_CW_BAD_ENABLE_GET(unit, pm_info, &is_3_cw_bad_enable, port_index));
    if (is_3_cw_bad_enable) {
        is_3_cw_bad_enable = 0;
        _SOC_IF_ERR_EXIT(PM8x50_3_CW_BAD_ENABLE_SET(unit, pm_info, is_3_cw_bad_enable, port_index));
    }

    _SOC_IF_ERR_EXIT(PM8x50_400G_AN_ENABLE_GET(unit, pm_info, &is_400g_an, port_index));
    if (is_400g_an) {
        is_400g_an = 0;
        _SOC_IF_ERR_EXIT(PM8x50_400G_AN_ENABLE_SET(unit, pm_info, is_400g_an, port_index));
        an_mode = phymod_AN_MODE_NONE;
        _SOC_IF_ERR_EXIT(PM8x50_AN_MODE_SET(unit, pm_info, an_mode, port_index));
    }

    _SOC_IF_ERR_EXIT(PM8x50_BER_WAR_ENABLE_GET(unit, pm_info, &is_ber_war_enable, port_index));
    if (is_ber_war_enable) {
        is_ber_war_enable = 0;
        _SOC_IF_ERR_EXIT(PM8x50_BER_WAR_ENABLE_SET(unit, pm_info, is_ber_war_enable, port_index));
    }

    _SOC_IF_ERR_EXIT(PM8x50_RLM_ENABLE_GET(unit, pm_info, &rlm_enabled, port_index));
    if (rlm_enabled) {
        rlm_enabled = 0;
        _SOC_IF_ERR_EXIT(PM8x50_RLM_ENABLE_SET(unit, pm_info, rlm_enabled, port_index));
    }

    _SOC_IF_ERR_EXIT(portmod_port_periodic_callback_unregister(unit, port));

exit:
    SOC_FUNC_RETURN;
}

/*Port remove in PM level*/
int pm8x50_port_detach(int unit, int port, pm_info_t pm_info)
{
    int enable, invalid_port = -1, tmp_port, i = 0, flags = 0, pm_id;
    int is_last_one = TRUE, port_index = -1, fec_null = 0;
    const uint32 is_active = 0;
    int tvco_pll_index, ovco_pll_index;
    uint8_t ovco_pll_active_lane_bm, tvco_pll_active_lane_bm;
    uint8_t ovco_pll_adv_lane_bm, tvco_pll_adv_lane_bm, eth_active_lanes;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, is_400g_an = 0;
    portmod_phy_timesync_config_t config;
    phymod_an_mode_type_t an_mode;
    int is_flexe_tvco_26g_required = 0;
    uint32 is_bypassed, is_other_bypassed = 0;
    int other_port_index;
    uint32 other_bitmap[1];
    int is_last_non_bypassed;
    pm_version_t version;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_periodic_callback_unregister(unit, port));

    tvco_pll_index = PM8X50_TVCO_PLL_INDEX_GET(pm_info);
    ovco_pll_index = (tvco_pll_index == 1)? 0: 1;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(pm8x50_port_enable_get(unit, port, pm_info, flags, &enable));
    if (enable)
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("can't detach active port %d"), port));

    /* Disable 1588 */
    config.flags = 0;
    _SOC_IF_ERR_EXIT(pm8x50_port_timesync_config_set(unit, port, pm_info, &config));

    /* Disable PCS for the port */
    _SOC_IF_ERR_EXIT(phymod_phy_pcs_enable_set(&phy_access, 0));
    _SOC_IF_ERR_EXIT(PM8x50_OVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &ovco_pll_active_lane_bm));
    _SOC_IF_ERR_EXIT(PM8x50_TVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_active_lane_bm));
    _SOC_IF_ERR_EXIT(PM8x50_OVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &ovco_pll_adv_lane_bm));
    _SOC_IF_ERR_EXIT(PM8x50_TVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_adv_lane_bm));

    /* flexport workaround for 16nm  flexportHW issue */
    /* first get the port index */
    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
        _SOC_IF_ERR_EXIT(PM8x50_LANE2PORT_GET(unit, pm_info, i, &tmp_port));
        if (tmp_port == port) {
            port_index = (port_index == -1 ? i : port_index);
        }
    }
    /* call port level port mode update */
    _SOC_IF_ERR_EXIT(_pm8x50_pm_port_mode_update(unit, port, pm_info, port_index, 1));
    /* call speed toggle workaround */
    _SOC_IF_ERR_EXIT(phymod_phy_pcs_enable_set(&phy_access, FLEXPORT_SW_WAR_ENABLE));
    /*-- Flexport workaround ends here. --*/

    /* update tdm mode for PM8x50_FLEXE */
    _SOC_IF_ERR_EXIT(_pm8x50_pm_version_get(unit, pm_info, &version));
    if (version.type == PM_TYPE_PM8X50_FLEXE) {
        /* revision number will be checked inside phymod_phy_tdm_mode_update */
        _SOC_IF_ERR_EXIT
            (_pm8x50_tdm_mode_update(unit, port, pm_info, 1));
    }

    _SOC_IF_ERR_EXIT(PM8x50_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));

    is_last_non_bypassed = !is_bypassed;

    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
        _SOC_IF_ERR_EXIT(PM8x50_LANE2PORT_GET(unit, pm_info, i, &tmp_port));
        if (tmp_port == port) {
            port_index = (port_index == -1 ? i : port_index);
            _SOC_IF_ERR_EXIT(PM8x50_LANE2PORT_SET(unit, pm_info, i, invalid_port));
            _SOC_IF_ERR_EXIT(PM8x50_LANE2FEC_SET(unit, pm_info, i, fec_null));
            ovco_pll_active_lane_bm &= ~(1 << i);
            tvco_pll_active_lane_bm &= ~(1 << i);
            ovco_pll_adv_lane_bm &= ~(1 << i);
            tvco_pll_adv_lane_bm &= ~(1 << i);
        } else if (tmp_port != invalid_port) {
            is_last_one = FALSE;
            if (!is_bypassed) {
                /* for DNX only - check if this is last non bypassed (not ILKN) port in order to reset MAC before powering down its clock */
                _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, tmp_port, pm_info, &other_port_index, other_bitmap));
                _SOC_IF_ERR_EXIT(PM8x50_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_other_bypassed, other_port_index));
                if (!is_other_bypassed) {
                    is_last_non_bypassed = FALSE;
                }
            }
        }
    }

    if (port_index == -1)
        _SOC_EXIT_WITH_ERR(SOC_E_PORT, (_SOC_MSG("Port %d wasn't found"), port));

    if ((!ovco_pll_active_lane_bm) && (!ovco_pll_adv_lane_bm)) {
        /* Power down OVCO if it's not in use */
        _SOC_IF_ERR_EXIT(phymod_phy_pll_pwrdn(&phy_access, ovco_pll_index, 1));
        PM_8x50_INFO(pm_info)->ovco = portmodVCOInvalid;
    }

    _SOC_IF_ERR_EXIT(PM8x50_OVCO_PLL_ACTIVE_LANE_BITMAP_SET(unit, pm_info, ovco_pll_active_lane_bm));
    _SOC_IF_ERR_EXIT(PM8x50_TVCO_PLL_ACTIVE_LANE_BITMAP_SET(unit, pm_info, tvco_pll_active_lane_bm));
    _SOC_IF_ERR_EXIT(PM8x50_OVCO_PLL_ADV_LANE_BITMAP_SET(unit, pm_info, ovco_pll_adv_lane_bm));
    _SOC_IF_ERR_EXIT(PM8x50_TVCO_PLL_ADV_LANE_BITMAP_SET(unit, pm_info, tvco_pll_adv_lane_bm));

    /*
     * disable unused clock repeater buffer
     * for DNX only
     */
    if (PM_8x50_INFO(pm_info)->clock_buffer_disable_required) {
        _SOC_IF_ERR_EXIT(
            _pm8x50_pll_clock_repeater_buffer_pwrdn(unit, pm_info,
            &phy_access, ovco_pll_index, ovco_pll_active_lane_bm));
        _SOC_IF_ERR_EXIT(
            _pm8x50_pll_clock_repeater_buffer_pwrdn(unit, pm_info,
            &phy_access, tvco_pll_index, tvco_pll_active_lane_bm));
    }

    _SOC_IF_ERR_EXIT(_pm8x50_pm_active_eth_lanes_get(unit, pm_info, &eth_active_lanes));

    _SOC_IF_ERR_EXIT(PM8x50_IS_FLEXE_TVCO_26G_REQUIRED_GET(unit, pm_info, &is_flexe_tvco_26g_required));
    if (!is_flexe_tvco_26g_required){
        if (!eth_active_lanes && !tvco_pll_active_lane_bm) {
            /*
             * Power down TVCO if
             * 1. No active ethernet ports, and
             * 2. No ILKN port is using TVCO.
             * Note: Since no active eth port -> there are no adv ports -> no need to check tvco_pll_adv_lane_bm
             */
            _SOC_IF_ERR_EXIT(phymod_phy_pll_pwrdn(&phy_access, tvco_pll_index, 1));
            PM_8x50_INFO(pm_info)->tvco = portmodVCOInvalid;
        }
    }
    /*clear 400G AN enable Warmboot variable */
    _SOC_IF_ERR_EXIT(PM8x50_400G_AN_ENABLE_SET(unit, pm_info, is_400g_an, port_index));
    an_mode = phymod_AN_MODE_NONE;
    _SOC_IF_ERR_EXIT(PM8x50_AN_MODE_SET(unit, pm_info, an_mode, port_index));

    /*deinit PM in case of last one*/
    if (!is_flexe_tvco_26g_required){
        if (is_last_one) {
            _SOC_IF_ERR_EXIT(portmod_port_pm_id_get(unit, port, &pm_id));
            _SOC_IF_ERR_EXIT(pm8x50_pm_enable(unit, pm_id, pm_info, 0, 0));
            _SOC_IF_ERR_EXIT(PM8x50_IS_ACTIVE_SET(unit, pm_info, is_active));
            _SOC_IF_ERR_EXIT(PM8x50_IS_CORE_INITIALIZED_SET(unit, pm_info, is_active));

            PM_8x50_INFO(pm_info)->int_core_access.type = phymodDispatchTypeInvalid;
        }
    }
    if (!is_last_one && is_last_non_bypassed) {
        /* DNX only - reset MAC if last NON bypassed port is removed */  
        int pm_enable_flags = 0;
        PORTMOD_PM_ENABLE_MAC_ONLY_SET(pm_enable_flags);
        _SOC_IF_ERR_EXIT(portmod_port_pm_id_get(unit, port, &pm_id));
        _SOC_IF_ERR_EXIT(pm8x50_pm_enable(unit, pm_id, pm_info, pm_enable_flags, 0));

    }
exit:
    SOC_FUNC_RETURN;
}

/*Port replace in PM level*/
int pm8x50_port_replace(int unit, int port, pm_info_t pm_info, int new_port)
{
    int i, tmp_port;
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_periodic_callback_unregister(unit, port));
    /* replace old port with new port */
    for(i = 0; i < MAX_PORTS_PER_PM8X50; i++) {
        rv = PM8x50_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
        _SOC_IF_ERR_EXIT(rv);
        if (tmp_port == port) {
            rv = PM8x50_LANE2PORT_SET(unit, pm_info, i, new_port);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

exit:
    SOC_FUNC_RETURN;
}


/*Port enable*/
int pm8x50_port_enable_set(int unit, int port, pm_info_t pm_info,
                           int flags, int enable)
{
    int actual_flags = flags;
    int cdmac_flags = 0, port_index;
    uint32 is_bypassed = 0, bitmap[1];
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys = 0;
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
        _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port,
                                     pm_info, &params, 1, &phy_access,
                                     &nof_phys, NULL));
    }
    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));

    if(enable){
        if((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) && (!is_bypassed)) {

            if(!PM_8x50_INFO(pm_info)->portmod_mac_soft_reset)
            {
                _SOC_IF_ERR_EXIT(cdmac_enable_set(unit, port, cdmac_flags, 1));
            }
            else
            {
                /* Enable CDMAC RX,TX, skip SOFT_RESET */
                _SOC_IF_ERR_EXIT(cdmac_tx_enable_set (unit, port, 1));
                _SOC_IF_ERR_EXIT(cdmac_rx_enable_set (unit, port, 1));
                _SOC_IF_ERR_EXIT(pm8x50_port_soft_reset(unit, port, pm_info, portmodMacSoftResetModeIn_Out, NULL, NULL));
            }
        }

        if(PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) {
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
        } /* PORTMOD_PORT_ENABLE_PHY_GET */
    } else {/* disable */
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

        if((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags))  && (!is_bypassed)) {
            _SOC_IF_ERR_EXIT(cdmac_enable_set(unit, port, cdmac_flags, 0));
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_enable_get(int unit, int port, pm_info_t pm_info,
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
        _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port,
                                     pm_info, &params, 1, &phy_access,
                                     &nof_phys, NULL));
    }

    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));

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

/*
 * Function:
 * _pm8x50_vcos_request_allocate_internal.
 *
 * Purpose:
 *      Function gets current TVCO, OVCO and tries to accommodate input VCO requests.
 *
 * Inputs:
 *     unit:                 unit number
 *     flags:                Special flags for vco allocation. (PM8X50_TVCO_SUPPORT_20P625G)
 *     required_vcos_bmp:    required VCO bitmap
 *     current_tvco:         current TVCO value
 *     current_ovco:         current OVCO value
 * Output:
 *     current_tvco:         if validation pass, current_tvco will indicate new value TVCO value
 *     current_ovco:         if validation pass, current_ovco will indicate new value OVCO value
 */
STATIC
int _pm8x50_vcos_request_allocate_internal(
        int unit,
        uint32 flags,
        const pm8x50_vcos_bmp_t required_vcos_bmp,
        portmod_vco_type_t *current_tvco,
        portmod_vco_type_t *current_ovco)
{
    pm8x50_vcos_bmp_t required_vcos_bmp_temp;
    int is_tvco_in_use, is_ovco_in_use, is_eth_drv_tvco;
    portmod_vco_type_t required_vco;
    int required_pcs_bypass;

    SOC_INIT_FUNC_DEFS;

    is_tvco_in_use = (*current_tvco == portmodVCOInvalid) ? 0 : 1;
    is_ovco_in_use = (*current_ovco == portmodVCOInvalid) ? 0 : 1;

    required_vcos_bmp_temp = required_vcos_bmp;

    while (!PM8x50_VCO_BMP_EMPTY(required_vcos_bmp_temp)) {
        is_eth_drv_tvco = _pm8x50_ethernet_tvco_check(unit, flags, *current_tvco);
        /*
         *  1. Get next required VCO.
         *  2. Clear VCO from temp bitmap
         */
        _SOC_IF_ERR_EXIT(
                _pm8x50_required_vco_value_iter(unit, &required_vcos_bmp_temp, &required_vco, &required_pcs_bypass));

        /* Try to match with TVCO current configuration */
        if (is_tvco_in_use) {
            if (required_vco == *current_tvco) {
                /*
                 * If tvco match AND eth port is requried AND request is not TVCO supported => there is no match.
                 * For pcs bypass no restrictions.
                 */
                if (required_pcs_bypass || (!required_pcs_bypass && is_eth_drv_tvco)) {
                    continue;
                }
            }
        }

        /* Try to match with OVCO current configuration */
        if (is_ovco_in_use) {
            if (required_vco == *current_ovco) {
                /*
                 * If ovco match AND eth port is requried AND tvco is not eth => there is no match.
                 * For pcs bypass no restrictions.
                 */
                if (required_pcs_bypass || (!required_pcs_bypass && is_eth_drv_tvco)) {
                    continue;
                }
            }
        }

        /* For TVCO ethernet rates, try first to lock TVCO. For non-TVCO ethernet rate - try to first lock OVCO*/
        if (_pm8x50_ethernet_tvco_check(unit, flags, required_vco)) {
            if (!is_tvco_in_use) {
                *current_tvco = required_vco;
                is_tvco_in_use = 1;
                continue;
            } else if (!is_ovco_in_use) {
                /*
                 * Can use OVCO for ILKN ports with no condition,
                 * Can use OVCO for ETH only if TVCO is ETH
                 */
                if (required_pcs_bypass || (!required_pcs_bypass && is_eth_drv_tvco)) {
                    *current_ovco = required_vco;
                    is_ovco_in_use = 1;
                    continue;
                }
            }
        } else {
            /* non-TVCO ethernet rate */
            if (!is_ovco_in_use) {
                if ((required_pcs_bypass) || (!required_pcs_bypass && is_eth_drv_tvco)) {
                    *current_ovco = required_vco;
                    is_ovco_in_use = 1;
                    continue;
                }
            } else if (!is_tvco_in_use) {
                if (required_pcs_bypass) {
                    /*
                     * Support setting TVCO if bypass
                     */
                    *current_tvco = required_vco;
                    is_tvco_in_use = 1;
                    continue;
                }
            }
        }

        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                (_SOC_MSG("Can not accommodate VCO settings.\n request [VCO %d, PCS bypass %d], state [TVCO %d, OVCO %d]\n"), required_vco, required_pcs_bypass, *current_tvco, *current_ovco));

    }

 exit:
    SOC_FUNC_RETURN;
}


/*
 * Function:
 * pm8x50_vcos_request_allocate.
 *
 * Purpose:
 *      Function gets current TVCO, OVCO and tries to accommodate input VCO requests.
 *      Function splits required VCO bitmap into 2 bitmaps -
 *         - VCO rates existing in current state
 *         - new VCO rates.
 *      Function first tries serve new VCO rates and then existing VCO rates.
 *
 * Inputs:
 *     unit:                 unit number
 *     flags:                special flag for allocation. (PM8X50_TVCO_SUPPORT_20P625G)
 *     required_vcos_bmp:    required VCO bitmap
 *     current_tvco:         current TVCO value
 *     current_ovco:         current OVCO value
 * Output:
 *     current_tvco:         If validation pass, current_tvco will indicate new value TVCO value
 *     current_ovco:         If validation pass, current_ovco will indicate new value OVCO value
 */

int pm8x50_vcos_request_allocate(
        int unit,
        uint32 flags,
        const pm8x50_vcos_bmp_t required_vcos_bmp,
        portmod_vco_type_t *current_tvco,
        portmod_vco_type_t *current_ovco)
{
    pm8x50_vcos_bmp_t vco_bmp, required_new_vcos_bmp = 0,  required_existing_vcos_bmp = 0;
    pm8x50_vcos_bmp_t required_vcos_bmp_temp;
    portmod_vco_type_t required_vco;
    int required_pcs_bypass;

    SOC_INIT_FUNC_DEFS;

    /* Copy request bitmap */
    required_vcos_bmp_temp = required_vcos_bmp;

    /* handle special case for Flexe configuration, where 26.5625 TVCO is required */
     if (PM8X50_TVCO_FLEXE_SUPPORT_26P562G_GET(flags)) {
        if (*current_tvco == portmodVCOInvalid) {
            PM8x50_VCO_ETH_26P562G_SET(required_vcos_bmp_temp);
        } else if (*current_tvco != portmodVCO26P562G) {
            _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                      (_SOC_MSG("Flex E can not be configured on this core \n")));
        }
    } else {
        /* Handle special case of 20.625G Ethernet only request */
        if (!PM8X50_TVCO_SUPPORT_20P625G_GET(flags) &&
                PM8x50_VCO_BMP_20P625G_ETH_ONLY(required_vcos_bmp) &&
                (*current_tvco == portmodVCOInvalid) && ((*current_ovco == portmodVCOInvalid) || (*current_ovco == portmodVCO20P625G))) {
            if (PM8X50_TVCO_PRIORITIZE_26P562G_GET(flags)) {
                /* If flag is set to prioritize 26.562G VCO to be TVCO, add 26.562G VCO request to drive TVCO. */
                PM8x50_VCO_ETH_26P562G_SET(required_vcos_bmp_temp);
            } else {
                /* Otherwise, add 25.781G VCO request to drive TVCO. */
                PM8x50_VCO_ETH_25P781G_SET(required_vcos_bmp_temp);
            }
       }

        /* Handle special case of 25G Ethernet only request */
        if (PM8x50_VCO_BMP_25G_ETH_ONLY(required_vcos_bmp) &&
                (*current_tvco == portmodVCOInvalid) &&
                ((*current_ovco == portmodVCOInvalid) || (*current_ovco == portmodVCO25G))) {
            if (PM8X50_TVCO_PRIORITIZE_26P562G_GET(flags)) {
                /* If flag is set to prioritize 26.562G VCO to be TVCO, add 26.562G VCO request to drive TVCO. */
                PM8x50_VCO_ETH_26P562G_SET(required_vcos_bmp_temp);
            } else {
                /* Otherwise, add 25.781G VCO request to drive TVCO. */
                PM8x50_VCO_ETH_25P781G_SET(required_vcos_bmp_temp);
            }
        }
    }

    /*
     * Split required VCOs bitmap into new/existing requests bitmaps
     */

    while (!PM8x50_VCO_BMP_EMPTY(required_vcos_bmp_temp))
    {
        /*
         *  1. Get next required VCO.
         *  2. Clear VCO from temp bitmap
         */
        _SOC_IF_ERR_EXIT(
                _pm8x50_required_vco_value_iter(unit, &required_vcos_bmp_temp, &required_vco, &required_pcs_bypass));

        /* Get VCO bitmap */
        _SOC_IF_ERR_EXIT(_pm8x50_required_vco_bitmap_get(unit, required_vco, required_pcs_bypass, &vco_bmp));

        if (required_vco == *current_tvco || required_vco == *current_ovco)
        {
            /* Add to existing VCO bitmap */
            required_existing_vcos_bmp |= vco_bmp;
        }
        else
        {
            /* Add to new VCO bitmap */
            required_new_vcos_bmp |= vco_bmp;
        }
    }

    /* Serve new VCOs */
    _SOC_IF_ERR_EXIT(
            _pm8x50_vcos_request_allocate_internal(unit, flags, required_new_vcos_bmp, current_tvco, current_ovco));

    /* Serve existing VCOs */
    _SOC_IF_ERR_EXIT(
            _pm8x50_vcos_request_allocate_internal(unit, flags, required_existing_vcos_bmp, current_tvco, current_ovco));

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm8x50_fec_lanebitmap_get(int unit,
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
    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++){
        rv = PM8x50_LANE2FEC_GET(unit, pm_info, i, &tmp_fec);
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
 * Function:
 * _pm8x50_single_vco_used_by_ports_validate
 *
 * Purpose:
 *     Check that only one VCO is used by ports
 *
 * Inputs:
 *     unit:               unit number;
 *     required_vcos_bmp:  Required vcos bitmap;
 *     tvco_in_use_by_ports: TVCO is in use by ports before
 *     ovco_in_use_by_ports: OVCO is in use by ports before
 *     tvco:         new  TVCO value
 *     ovco:         new  OVCO value
  */
STATIC int
_pm8x50_single_vco_used_by_ports_validate(int unit,
                                          pm8x50_vcos_bmp_t required_vcos_bmp,
                                          int tvco_in_use_by_ports,
                                          int ovco_in_use_by_ports,
                                          portmod_vco_type_t tvco,
                                          portmod_vco_type_t ovco)
{

    pm8x50_vcos_bmp_t required_vcos_bmp_temp;
    portmod_vco_type_t required_vco;
    int required_pcs_bypass;
    int tvco_requested = FALSE, ovco_requested = FALSE;
    int is_tvco_in_use_after = FALSE, is_ovco_in_use_after = FALSE;

    SOC_INIT_FUNC_DEFS;

    required_vcos_bmp_temp = required_vcos_bmp;
    while (!PM8x50_VCO_BMP_EMPTY(required_vcos_bmp_temp))
    {
        /*
         *  1. Get next required VCO.
         *  2. Clear VCO from temp bitmap
         */
        _SOC_IF_ERR_EXIT(
            _pm8x50_required_vco_value_iter(unit, &required_vcos_bmp_temp, &required_vco, &required_pcs_bypass));
        if (required_vco == tvco) {
            tvco_requested = TRUE;
        }
        if (required_vco == ovco) {
            ovco_requested = TRUE;
        }
    }
    is_tvco_in_use_after = (tvco_in_use_by_ports || tvco_requested);
    is_ovco_in_use_after = (ovco_in_use_by_ports || ovco_requested);
    if (is_tvco_in_use_after && is_ovco_in_use_after)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("Can not accommodate VCO settings.\n Both %d and %d are in use by active ports, but the PM is limited to work with single VCO \n"),
                            tvco, ovco));
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * Function:
 *_pm8x50_vcos_setting_validate
 *
 * Purpose:
 *     VCOs setting validation.
 *
 * Inputs:
 *     unit:               unit number;
 *     pm_info:            pm_info data structure;
 *     ports:              Logical port bitmaps which reqiring the input vcos;
 *     required_vcos_bmp:  Required vcos bitmap;
 *     flag:               PLL switch flag;
 * Output:
 *     vco_setting:    If validation pass, vco_setting will indicating
 *                     what's the new vco rates and whether they are new or not;
 *                     vco_setting.num_speeds and vco_setting.speed_config_list
 *                     are no care in this function.
 */
int _pm8x50_vcos_setting_validate(int unit,
                                  pm_info_t pm_info,
                                  const portmod_pbmp_t* ports,
                                  pm8x50_vcos_bmp_t required_vcos_bmp,
                                  int flag,
                                  portmod_pm_vco_setting_t* vco_setting)
{
    uint8_t ovco_pll_active_lane_bitmap, tvco_pll_active_lane_bitmap, ovco_pll_adv_lane_bitmap, tvco_pll_adv_lane_bitmap;
    int is_tvco_in_use, is_ovco_in_use;
    int is_flexe_tvco_26g_required = 0;
    portmod_vco_type_t current_tvco, current_ovco;
    uint32_t port_lane_mask = 0, lane_bitmap[1];
    int port, port_index;
    uint8_t active_eth_lanes;
    uint32 flags = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(
            PM8x50_OVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &ovco_pll_active_lane_bitmap));
    _SOC_IF_ERR_EXIT(
            PM8x50_TVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_active_lane_bitmap));
    _SOC_IF_ERR_EXIT(
            PM8x50_OVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &ovco_pll_adv_lane_bitmap));
    _SOC_IF_ERR_EXIT(
            PM8x50_TVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_adv_lane_bitmap));
    _SOC_IF_ERR_EXIT(
            _pm8x50_pm_active_eth_lanes_get(unit, pm_info, &active_eth_lanes));
    _SOC_IF_ERR_EXIT(
            PM8x50_IS_FLEXE_TVCO_26G_REQUIRED_GET(unit, pm_info, &is_flexe_tvco_26g_required));

    /*
     * Get lane mask for the input ports
     */
    PORTMOD_PBMP_ITER(*ports, port) {
        _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, lane_bitmap));
        port_lane_mask |= lane_bitmap[0];
    }

    /* Remove input lanes from ovco and check if it is in use */
    ovco_pll_active_lane_bitmap &= ~port_lane_mask;
    ovco_pll_adv_lane_bitmap &= ~port_lane_mask;
    is_ovco_in_use = ((ovco_pll_active_lane_bitmap | ovco_pll_adv_lane_bitmap) == 0) ? 0 : 1;
    if (is_ovco_in_use) {
        /* ovco in use (not refering to the ports that are currently requested) */
        current_ovco = PM_8x50_INFO(pm_info)->ovco;
    }
    else {
        /*
         *  ovco not in use (not refering to the ports that are currently requested)
         *  currently set as invalid (ovco might be allcoate by VCO algorithm)
         */
        current_ovco = portmodVCOInvalid;
    }

    /* Remove input lanes from tvco and check if it is in use */
    tvco_pll_active_lane_bitmap &= ~port_lane_mask;
    tvco_pll_adv_lane_bitmap &= ~port_lane_mask;
    active_eth_lanes &= ~port_lane_mask;
    is_tvco_in_use = ((tvco_pll_active_lane_bitmap | tvco_pll_adv_lane_bitmap | active_eth_lanes) == 0) ? 0 : 1;
    if (is_tvco_in_use) {
        /* tvco in use (not refering to the ports that are currently requested) */
        current_tvco = PM_8x50_INFO(pm_info)->tvco;
    }
    else {
        /*
         *  tvco not in use (not refering to the ports that are currently requested)
         *  currently set as invalid (tvco might be allcoate by VCO algorithm)
         */
        current_tvco = portmodVCOInvalid;
    }

    if (PM_8x50_INFO(pm_info)->allow_20p625g_tvco) {
        PM8X50_TVCO_SUPPORT_20P625G_SET(flags);
    }

    /* next check if Flexe TVCO 26G is set */
    if (is_flexe_tvco_26g_required) {
        PM8X50_TVCO_FLEXE_SUPPORT_26P562G_SET(flags);
    }

    if (PORTMOD_PM_SPEED_VALIDATE_F_TVCO_PRIORITIZE_26P562G_GET(flag)) {
        PM8X50_TVCO_PRIORITIZE_26P562G_SET(flags);
    }

    /* Try to allocate required VCOs */
    _SOC_IF_ERR_EXIT(
            pm8x50_vcos_request_allocate(unit, flags, required_vcos_bmp, &current_tvco, &current_ovco));

    /** for Jericho 2 */
    if (PM_8x50_INFO(pm_info)->single_vco_used_by_ports) {
        /** make sure only one VCO is used by ports */
        _SOC_IF_ERR_EXIT(
            _pm8x50_single_vco_used_by_ports_validate(unit, required_vcos_bmp,
                                                      (tvco_pll_active_lane_bitmap | tvco_pll_adv_lane_bitmap) ? 1 : 0,
                                                      (ovco_pll_active_lane_bitmap | ovco_pll_adv_lane_bitmap) ? 1 : 0, 
                                                      current_tvco, current_ovco));
    }

    /* Set output */
    vco_setting->tvco = current_tvco;
    vco_setting->is_tvco_new = (PM_8x50_INFO(pm_info)->tvco == current_tvco) ? 0 : 1;
    vco_setting->ovco = current_ovco;
    vco_setting->is_ovco_new = (PM_8x50_INFO(pm_info)->ovco == current_ovco) ? 0 : 1;

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
 */
int pm8x50_pm_speed_config_validate(int unit,
                                    int pm_id,
                                    pm_info_t pm_info,
                                    const portmod_pbmp_t* ports,
                                    int flag,
                                    portmod_pm_vco_setting_t* vco_setting)
{
    int i, rv, pam4_eth_req = 0;
    portmod_vco_type_t vco;
    pm8x50_vcos_bmp_t required_vcos_bmp = 0, required_vco_bmp_temp = 0;
    int is_pcs_bypass;
    uint8_t rs528_bm = 0, rs544_bm = 0, rs272_bm = 0, affected_bm = 0;
    int nof_phys, port;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    pm_version_t version;
    uint32 pm_version_flag = 0;
    phymod_phy_signalling_method_t signalling_method;
    SOC_INIT_FUNC_DEFS;

    /* Get RS FEC usage bitmaps from WB */
    _SOC_IF_ERR_EXIT(_pm8x50_fec_lanebitmap_get(unit, pm_info, &rs528_bm, &rs544_bm, &rs272_bm));

    _SOC_IF_ERR_EXIT(_pm8x50_pm_version_get(unit, pm_info, &version));
    if ((version.tech_process == PM_TECH_16NM) && (version.type == PM_TYPE_PM8X50)) {
        PM8X50_PM_VERSION_F_REV0_16NM_SET(pm_version_flag);
    }
    /* Remove the input ports from FEC usage bitmaps*/
    PORTMOD_PBMP_ITER(*ports, port) {
        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
        _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                    &params, 1, &phy_access, &nof_phys, NULL));
        rs528_bm &= ~phy_access.access.lane_mask;
        rs544_bm &= ~phy_access.access.lane_mask;
        rs272_bm &= ~phy_access.access.lane_mask;
    }

    /* Get the VCOs required by the input speed config list */
    for (i = 0; i < vco_setting->num_speeds; i++) {
        vco = portmodVCOInvalid;
        rv = _pm8x50_port_speed_config_to_vco_get(&(vco_setting->speed_config_list[i]),
                                                  vco_setting->speed_for_pcs_bypass_port[i],
                                                  pm_version_flag, &vco);
        if (rv == SOC_E_CONFIG) {
            _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                      (_SOC_MSG("Can not support speed: %d num_lane: %d fec: %d"),
                      vco_setting->speed_config_list[i].speed,
                      vco_setting->speed_config_list[i].num_lane,
                      vco_setting->speed_config_list[i].fec));
        }

        is_pcs_bypass = vco_setting->speed_for_pcs_bypass_port[i];
        _SOC_IF_ERR_EXIT(_pm8x50_required_vco_bitmap_get(unit, vco, is_pcs_bypass, &required_vco_bmp_temp));
        required_vcos_bmp |= required_vco_bmp_temp;

        _pm8x50_signalling_method_get(vco_setting->speed_for_pcs_bypass_port[i],
                                      &(vco_setting->speed_config_list[i]),
                                      &signalling_method);
        if ((signalling_method == phymodSignallingMethodPAM4) &&
             !vco_setting->speed_for_pcs_bypass_port[i]) {
            pam4_eth_req = 1;
        }

        if (!is_pcs_bypass) {
            if ((vco_setting->speed_config_list[i].fec == PORTMOD_PORT_PHY_FEC_RS_272) ||
                (vco_setting->speed_config_list[i].fec == PORTMOD_PORT_PHY_FEC_RS_272_2XN)) {
                _SOC_IF_ERR_EXIT(_pm8x50_lanebitmap_set(vco_setting->port_starting_lane_list[i],
                                                        vco_setting->speed_config_list[i].num_lane,
                                                        &rs272_bm));
            } else if ((vco_setting->speed_config_list[i].fec == PORTMOD_PORT_PHY_FEC_RS_544) ||
                       (vco_setting->speed_config_list[i].fec == PORTMOD_PORT_PHY_FEC_RS_544_2XN)) {
                _SOC_IF_ERR_EXIT(_pm8x50_lanebitmap_set(vco_setting->port_starting_lane_list[i],
                                                        vco_setting->speed_config_list[i].num_lane,
                                                        &rs544_bm));
            } else if (vco_setting->speed_config_list[i].fec == PORTMOD_PORT_PHY_FEC_RS_FEC) {
                _SOC_IF_ERR_EXIT(_pm8x50_lanebitmap_set(vco_setting->port_starting_lane_list[i],
                                                        vco_setting->speed_config_list[i].num_lane,
                                                        &rs528_bm));
            }
        }

    }

    /* Validate FEC settings for RS528, RS544 and RS272 */
    _SOC_IF_ERR_EXIT(_pm8x50_fec_validate(unit, pm_version_flag, rs528_bm, rs544_bm, rs272_bm, &affected_bm));

    _SOC_IF_ERR_EXIT(_pm8x50_vcos_setting_validate(unit, pm_info, ports, required_vcos_bmp, flag, vco_setting));

    if (pam4_eth_req && (vco_setting->tvco == portmodVCO20P625G)) {
       _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
            (_SOC_MSG("Can not accommodate VCO settings.\n Current TVCO does not support Ethernet PAM4 speeds.\n")));
    }
exit:
    SOC_FUNC_RETURN;
}

/*Port speed validation.*/
int pm8x50_port_speed_config_validate(int unit,
                                      int port,
                                      pm_info_t pm_info,
                                      const portmod_speed_config_t* speed_config,
                                      portmod_pbmp_t* affected_pbmp)
{
    int rv, i, tmp_port, port_num_lanes, port_index, port_is_pcs_bypassed;
    portmod_vco_type_t required_vco;
    uint8_t ovco_pll_active_lane_bitmap, tvco_pll_active_lane_bitmap, ovco_pll_adv_lane_bitmap, tvco_pll_adv_lane_bitmap;
    uint32_t port_lane_mask[1];
    uint8_t affected_lane_bitmap = 0, pm_is_full_bypassed, active_eth_lanes;
    uint8_t rs528_bm = 0, rs544_bm = 0, rs272_bm = 0;
    pm_version_t version;
    uint32 flags = 0;
    phymod_phy_signalling_method_t signalling_method;

    SOC_INIT_FUNC_DEFS;

    /* clear the affected_pbmp first */
    PORTMOD_PBMP_CLEAR(*affected_pbmp);

    /* Get lane_mask and number of lanes for the port from DB */
    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, port_lane_mask));
    _SOC_IF_ERR_EXIT(PM8x50_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &port_is_pcs_bypassed, port_index));

    _SOC_IF_ERR_EXIT(_pm8x50_pm_version_get(unit, pm_info, &version));
    if ((version.tech_process == PM_TECH_16NM) && (version.type == PM_TYPE_PM8X50)) {
        PM8X50_PM_VERSION_F_REV0_16NM_SET(flags);
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
        _SOC_IF_ERR_EXIT(_pm8x50_fec_lanebitmap_get(unit, pm_info, &rs528_bm, &rs544_bm, &rs272_bm));

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
        rv = _pm8x50_fec_validate(unit, flags, rs528_bm, rs544_bm, rs272_bm, &affected_lane_bitmap);
        if (rv != SOC_E_NONE) {
            /* If FEC validate fails, return affected port bit map */
            affected_lane_bitmap &= ~port_lane_mask[0];
            for(i = 0 ; i < MAX_PORTS_PER_PM8X50; i++){
                if ((affected_lane_bitmap >> i) & 1) {
                    _SOC_IF_ERR_EXIT(PM8x50_LANE2PORT_GET(unit, pm_info, i, &tmp_port));
                    PORTMOD_PBMP_PORT_ADD(*affected_pbmp, tmp_port);
                }
            }
            return rv;
        }
    }
    _SOC_IF_ERR_EXIT(
        PM8x50_OVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &ovco_pll_active_lane_bitmap));
    _SOC_IF_ERR_EXIT(
        PM8x50_TVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_active_lane_bitmap));
    _SOC_IF_ERR_EXIT(
        PM8x50_OVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &ovco_pll_adv_lane_bitmap));
    _SOC_IF_ERR_EXIT(
        PM8x50_TVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_adv_lane_bitmap));
    _SOC_IF_ERR_EXIT(
        _pm8x50_pm_active_eth_lanes_get(unit, pm_info, &active_eth_lanes));

    /* Remove port lane mask from ovco_pll_active_lane_bitmap, ovco_pll_adv_lane_bitmap */
    ovco_pll_active_lane_bitmap &= ~port_lane_mask[0];
    ovco_pll_adv_lane_bitmap &= ~port_lane_mask[0];

    /* Remove port lane mask from tvco_pll_active_lane_bitmap, tvco_pll_adv_lane_bitmap */
    tvco_pll_active_lane_bitmap &= ~port_lane_mask[0];
    tvco_pll_adv_lane_bitmap &= ~port_lane_mask[0];

    pm_is_full_bypassed = ((active_eth_lanes & ~port_lane_mask[0]) == 0) ? 1 : 0;

    /* Verify and get the required VCO */
    rv = _pm8x50_port_speed_config_to_vco_get(speed_config, port_is_pcs_bypassed, flags, &required_vco);
    if (rv == SOC_E_CONFIG) {
        _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                  (_SOC_MSG("Speed config is not supported")));
    }

    _pm8x50_signalling_method_get(port_is_pcs_bypassed, speed_config, &signalling_method);
    if (!port_is_pcs_bypassed &&
        (signalling_method == phymodSignallingMethodPAM4) &&
        (PM_8x50_INFO(pm_info)->tvco == portmodVCO20P625G) &&
        (tvco_pll_adv_lane_bitmap || tvco_pll_active_lane_bitmap ||
         ovco_pll_adv_lane_bitmap || ovco_pll_active_lane_bitmap)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("Can not support Ethernet PAM4 speeds when TVCO is 20.625G.")));
    } else if (PORTMOD_VCO_PCS_BYPASS_DEDICATED(PM_8x50_INFO(pm_info)->tvco) && !port_is_pcs_bypassed) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("Can not accommodate VCO settings. TVCO is dedicated for PCS bypassed ports! \n")));
    } else if (required_vco == PM_8x50_INFO(pm_info)->tvco
        || required_vco == PM_8x50_INFO(pm_info)->ovco
        || (ovco_pll_active_lane_bitmap == 0 && ovco_pll_adv_lane_bitmap == 0)
        || (pm_is_full_bypassed && tvco_pll_active_lane_bitmap == 0 && tvco_pll_adv_lane_bitmap == 0)) {
        /*
         * 1. no vco change required
         * 2. OR, ovco is free to change
         * 3. OR, tvco is free to change if previous PM bypass mode is full bypass
         */
        return SOC_E_NONE;
    } else if (ovco_pll_active_lane_bitmap & ovco_pll_adv_lane_bitmap) {
        /* OVCO is used by resolved AN port */
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("Can not accommodate VCO settings due to resolved AN port(s).")));
    } else if (ovco_pll_active_lane_bitmap || ovco_pll_adv_lane_bitmap) {
        /* OVCO is used by FS port or is advertiesd by AN port */
        /* affected_lane_bitmap in this case should include ports that are actively using ovco or advertising the ovco */
        affected_lane_bitmap = ovco_pll_active_lane_bitmap | ovco_pll_adv_lane_bitmap;
        for( i = 0 ; i < MAX_PORTS_PER_PM8X50; i++){
            if ((affected_lane_bitmap >> i) & 1) {
                rv = PM8x50_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
                _SOC_IF_ERR_EXIT(rv);
                PORTMOD_PBMP_PORT_ADD(*affected_pbmp, tmp_port);
            }
        }
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("Can not accommodate VCO settings.")));
    } else {
        /* coverity[dead_error_line] */
        return SOC_E_PARAM;
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * This API reconfig the VCO rates of the PM.
 *
 * 'pm8x50_pm_speed_config_validate' need to be called before this function.
 * Only when pm8x50_pm_speed_config_validate reuturns E_NONE and indicates
 * VCO change requirement, user can call this API to change VCOs.
 *
 * Two VCO values are always
 * required by this API regardless of whether the caller intends to change
 * one or two VCO rates.
 * vco[0]: TVCO.
 * vco[1]: OVCO.
 */
int pm8x50_pm_vco_reconfig(int unit,
                           int pm_id,
                           pm_info_t pm_info,
                           const portmod_vco_type_t* vco)
{
    uint8_t active_eth_lanes, new_ovco_active_lanes;
    uint8_t ovco_pll_active_lane_bm, tvco_pll_active_lane_bm;
    uint8_t ovco_pll_adv_lane_bm, tvco_pll_adv_lane_bm, zero = 0;
    portmod_vco_type_t cur_tvco = PM_8x50_INFO(pm_info)->tvco;
    portmod_vco_type_t cur_ovco = PM_8x50_INFO(pm_info)->ovco;
    uint32_t pll_div;
    int tvco_pll_index, ovco_pll_index;
    phymod_phy_access_t phy_access;
    int fec_null = 0, i;
    uint8_t ovco_active_lanes_bitmap, ovco_adv_lanes_bitmap, ovco_lanes_bitmap;
    SOC_INIT_FUNC_DEFS;

    tvco_pll_index = PM8X50_TVCO_PLL_INDEX_GET(pm_info);
    ovco_pll_index = (tvco_pll_index == 1)? 0: 1;
    
    _SOC_IF_ERR_EXIT(PM8x50_OVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &ovco_pll_active_lane_bm));
    _SOC_IF_ERR_EXIT(PM8x50_TVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_active_lane_bm));
    _SOC_IF_ERR_EXIT(PM8x50_OVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &ovco_pll_adv_lane_bm));
    _SOC_IF_ERR_EXIT(PM8x50_TVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_adv_lane_bm));
    _SOC_IF_ERR_EXIT(_pm8x50_pm_active_eth_lanes_get(unit, pm_info, &active_eth_lanes));

    if (cur_tvco != vco[0]) {
        /* TVCO need to be changed */
        PM_8x50_INFO(pm_info)->tvco = vco[0];
        /* Get PLL_DIV */
        _SOC_IF_ERR_EXIT(_pm8x50_vco_to_pll_get(PM_8x50_INFO(pm_info)->ref_clk, vco[0], &pll_div));

        sal_memcpy (&phy_access, &(PM_8x50_INFO(pm_info)->int_core_access),
                    sizeof(phymod_phy_access_t));

        if (pll_div != phymod_TSCBH_PLL_DIVNONE) {
            if (active_eth_lanes == (tvco_pll_adv_lane_bm | tvco_pll_active_lane_bm |
                                     ovco_pll_adv_lane_bm | ovco_pll_active_lane_bm)) {
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
                phy_access.access.lane_mask = active_eth_lanes | tvco_pll_active_lane_bm;
            }
            _SOC_IF_ERR_EXIT(
                phymod_phy_pll_reconfig(&phy_access, tvco_pll_index, pll_div, PM_8x50_INFO(pm_info)->ref_clk));
        } else {
             /* will pwoer down this PLL */
             _SOC_IF_ERR_EXIT(phymod_phy_pll_pwrdn(&phy_access, tvco_pll_index, 1));
        }

        /* For Ethernet mode, TVCO change will reset the whole serdes core.
         * So we will clear:
         *     1. the lane bitmaps for both PLLs.
         *     2. FEC type for all lanes.
         */
        _SOC_IF_ERR_EXIT(PM8x50_OVCO_PLL_ACTIVE_LANE_BITMAP_SET(unit, pm_info, zero));
        _SOC_IF_ERR_EXIT(PM8x50_OVCO_PLL_ADV_LANE_BITMAP_SET(unit, pm_info, zero));
        _SOC_IF_ERR_EXIT(PM8x50_TVCO_PLL_ACTIVE_LANE_BITMAP_SET(unit, pm_info, zero));
        _SOC_IF_ERR_EXIT(PM8x50_TVCO_PLL_ADV_LANE_BITMAP_SET(unit, pm_info, zero));
        for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
            _SOC_IF_ERR_EXIT(PM8x50_LANE2FEC_SET(unit, pm_info, i, fec_null));
        }
        /*
         * Since TVCO has been changed, need to remove the Ethernet lanes in
         * ovco_pll_active_lane_bm.
 */
        new_ovco_active_lanes = ovco_pll_active_lane_bm & ~active_eth_lanes;
        _SOC_IF_ERR_EXIT(PM8x50_OVCO_PLL_ACTIVE_LANE_BITMAP_SET(unit, pm_info, new_ovco_active_lanes));
        _SOC_IF_ERR_EXIT(PM8x50_OVCO_PLL_ADV_LANE_BITMAP_SET(unit, pm_info, zero));

        /* update PLL0 rx clock repeater buffer bandwidth, for DNX device only */
        if (PM_8x50_INFO(pm_info)->clock_buffer_bw_update_required) {
             /* for PLL0 only */
            if ((tvco_pll_index == 0) && (PM_8x50_INFO(pm_info)->tvco != portmodVCOInvalid)) {
                _SOC_IF_ERR_EXIT(phymod_phy_pll_rx_clock_buffer_bw_update(&phy_access, 0/* pll0 */,
                    pll_div, PM_8x50_INFO(pm_info)->ref_clk));
            }
        }
    }

    if (cur_ovco != vco[1]) {
        /* OVCO need to be changed */
        PM_8x50_INFO(pm_info)->ovco = vco[1];
        /* next get the OVCO's PLL divider */
        _SOC_IF_ERR_EXIT(_pm8x50_vco_to_pll_get(PM_8x50_INFO(pm_info)->ref_clk, vco[1], &pll_div));

        sal_memcpy (&phy_access, &(PM_8x50_INFO(pm_info)->int_core_access),
                    sizeof(phymod_phy_access_t));
        /*once ilkn support is added, the phy_access lane mask needs to be adjusted */
        if (pll_div !=phymod_TSCBH_PLL_DIVNONE) {
            _SOC_IF_ERR_EXIT(
                phymod_phy_pll_reconfig(&phy_access, ovco_pll_index, pll_div, PM_8x50_INFO(pm_info)->ref_clk));
        } else {
             _SOC_IF_ERR_EXIT(phymod_phy_pll_pwrdn(&phy_access, ovco_pll_index, 1));
        }

        /* Clear FEC usage for all the OVCO lanes */
        _SOC_IF_ERR_EXIT(
            PM8x50_OVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &ovco_active_lanes_bitmap));
        _SOC_IF_ERR_EXIT(
            PM8x50_OVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &ovco_adv_lanes_bitmap));
        ovco_lanes_bitmap = ovco_active_lanes_bitmap | ovco_adv_lanes_bitmap;
        for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
            if ((ovco_lanes_bitmap >> i) & 0x1) {
              _SOC_IF_ERR_EXIT(PM8x50_LANE2FEC_SET(unit, pm_info, i, fec_null));
            }
        }

        _SOC_IF_ERR_EXIT(PM8x50_OVCO_PLL_ACTIVE_LANE_BITMAP_SET(unit, pm_info, zero));
        _SOC_IF_ERR_EXIT(PM8x50_OVCO_PLL_ADV_LANE_BITMAP_SET(unit, pm_info, zero));

        /* update PLL0 rx clock repeater buffer bandwidth, for DNX device only */
        if (PM_8x50_INFO(pm_info)->clock_buffer_bw_update_required) {
            if ((ovco_pll_index == 0) && (PM_8x50_INFO(pm_info)->ovco != portmodVCOInvalid)) {
                _SOC_IF_ERR_EXIT(phymod_phy_pll_rx_clock_buffer_bw_update(&phy_access, 0/* pll0 */,
                    pll_div, PM_8x50_INFO(pm_info)->ref_clk));
            }
        }
    }

exit:
    SOC_FUNC_RETURN;
}

/* set/get the speed config for the specified port.*/
int pm8x50_port_speed_config_set(int unit, int port, pm_info_t pm_info, const portmod_speed_config_t* speed_config)
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
    uint8 tvco_pll_lanes_bitmap, ovco_pll_lanes_bitmap;
    portmod_vco_type_t required_vco;
    phymod_an_mode_type_t an_mode = phymod_AN_MODE_NONE;
    int port_index;
    uint32 is_bypassed = 0, flags = 0;
    uint32 bitmap[1];
    pm_version_t version;
    int tvco_pll_index, ovco_pll_index, port_flexe_mode = 0;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(mac_ctrl);
    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_t_init(&phy_speed_config));

    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));

    if(!PM_8x50_INFO(pm_info)->portmod_mac_soft_reset)
    {
        /* Get original MAC CTRL */
        _SOC_IF_ERR_EXIT(cdmac_egress_queue_drain_get(unit, port, &mac_ctrl, &rx_enable));

        /* Disable MAC RX, drain MAC FIFO, assert MAC SOFT_RESET */
        _SOC_IF_ERR_EXIT(_pm8x50_port_mac_drain_soft_reset(unit, port));
    }
    else if (!is_bypassed)
    {
       _SOC_IF_ERR_EXIT(pm8x50_port_soft_reset(unit, port, pm_info, portmodMacSoftResetModeIn, &rx_enable, &mac_ctrl));
    }

    /* Validate speed_config */
    _SOC_IF_ERR_EXIT(pm8x50_port_speed_config_validate(unit, port, pm_info, speed_config, &affected_pbmp));

    /* Update port_mode */
    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, &port_lane_mask));
    _SOC_IF_ERR_EXIT(_pm8x50_pm_port_mode_update(unit, port, pm_info, port_index, speed_config->num_lane));

    _SOC_IF_ERR_EXIT(
        PM8x50_OVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &ovco_pll_lanes_bitmap));
    _SOC_IF_ERR_EXIT(
        PM8x50_TVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_lanes_bitmap));

    
    old_pll_state.pll0_lanes_bitmap = ovco_pll_lanes_bitmap;
    old_pll_state.pll1_lanes_bitmap = tvco_pll_lanes_bitmap;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    port_is_pcs_bypassed = PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy_access.device_op_mode);

    _SOC_IF_ERR_EXIT(_pm8x50_pm_version_get(unit, pm_info, &version));
    if ((version.tech_process == PM_TECH_16NM) && (version.type == PM_TYPE_PM8X50)) {
        PM8X50_PM_VERSION_F_REV0_16NM_SET(flags);
    }
    _SOC_IF_ERR_EXIT(_pm8x50_port_speed_config_to_vco_get(speed_config, port_is_pcs_bypassed, flags, &required_vco));

    port_lane_mask = phy_access.access.lane_mask;

    if (required_vco == PM_8x50_INFO(pm_info)->tvco) {
        tvco_pll_lanes_bitmap |= port_lane_mask;
        ovco_pll_lanes_bitmap &= ~port_lane_mask;
    } else if (required_vco == PM_8x50_INFO(pm_info)->ovco) {
        ovco_pll_lanes_bitmap |= port_lane_mask;
        tvco_pll_lanes_bitmap &= ~port_lane_mask;
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("VCO need to be reconfigured before changing speed.")));
    }

    /*
     * power up clock repeater buffer if it was powered down
     * for DNX device only
     */
    if (PM_8x50_INFO(pm_info)->clock_buffer_disable_required) {
       tvco_pll_index = PM8X50_TVCO_PLL_INDEX_GET(pm_info);
       ovco_pll_index = (tvco_pll_index == 1)? 0: 1;
       _SOC_IF_ERR_EXIT(
            _pm8x50_pll_clock_repeater_buffer_pwrdn(unit, pm_info,
            &phy_access, ovco_pll_index, ovco_pll_lanes_bitmap));
        _SOC_IF_ERR_EXIT(
            _pm8x50_pll_clock_repeater_buffer_pwrdn(unit, pm_info,
            &phy_access, tvco_pll_index, tvco_pll_lanes_bitmap));
    }

    /* Decode the lane_config in speed_config */
    lane_config = speed_config->lane_config;
    /* Retrieve the default lane config */
    _pm8x50_signalling_method_get(is_bypassed, speed_config, &signalling_mode);
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
    _SOC_IF_ERR_EXIT(PM8x50_PORT_FLEXE_MODE_GET(unit, pm_info, &port_flexe_mode, port_index));
    if (port_flexe_mode) {
        PHYMOD_SPEED_CONFIG_IN_FLEXE_MODE_SET(&phy_speed_config);
    }

    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_set(&phy_access, &phy_speed_config, &old_pll_state, &new_pll_state));

    if(!PM_8x50_INFO(pm_info)->portmod_mac_soft_reset)
    {
        /* Enable MAC RX, De-assert MAC SOFT_RESET */
        _SOC_IF_ERR_EXIT(_pm8x50_port_rx_restore_mac_out_of_reset(unit, port, rx_enable));
        /* Restore original MAC CONTROL */
        _SOC_IF_ERR_EXIT(cdmac_mac_ctrl_set(unit, port, mac_ctrl));
    }
    else if (!is_bypassed)
    {
        _SOC_IF_ERR_EXIT(pm8x50_port_soft_reset(unit, port, pm_info, portmodMacSoftResetModeOut, &rx_enable, &mac_ctrl));
    }

    /* Update PLL active lane bitmaps */
    _SOC_IF_ERR_EXIT(
        PM8x50_OVCO_PLL_ACTIVE_LANE_BITMAP_SET(unit, pm_info, ovco_pll_lanes_bitmap));
    _SOC_IF_ERR_EXIT(
        PM8x50_TVCO_PLL_ACTIVE_LANE_BITMAP_SET(unit, pm_info, tvco_pll_lanes_bitmap));

    /* Clear the adv lane bitmap for the port because it's configured as a force speed port */
    _SOC_IF_ERR_EXIT(
        PM8x50_OVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &ovco_pll_lanes_bitmap));
    _SOC_IF_ERR_EXIT(
        PM8x50_TVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &tvco_pll_lanes_bitmap));
    ovco_pll_lanes_bitmap &= ~port_lane_mask;
    tvco_pll_lanes_bitmap &= ~port_lane_mask;
    _SOC_IF_ERR_EXIT(
        PM8x50_OVCO_PLL_ADV_LANE_BITMAP_SET(unit, pm_info, ovco_pll_lanes_bitmap));
    _SOC_IF_ERR_EXIT(
        PM8x50_TVCO_PLL_ADV_LANE_BITMAP_SET(unit, pm_info, tvco_pll_lanes_bitmap));

    /* Clear an_mode for the port to force user set new abilities before enable AN later on */
    _SOC_IF_ERR_EXIT(PM8x50_AN_MODE_SET(unit, pm_info, an_mode, port_index));

    if (!is_bypassed) {
        /* Update FEC usage in WB */
        for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
            if ((port_lane_mask >> i) & 0x1) {
                _SOC_IF_ERR_EXIT(PM8x50_LANE2FEC_SET(unit, pm_info, i, speed_config->fec));
            }
        }
    }

    /* update FEC type for RLM usage */
    _SOC_IF_ERR_EXIT(PM8x50_ORIGINAL_FEC_SET(unit, pm_info, phy_speed_config.fec_type, port_index));

    /* update tdm mode for FLEXE mode */
    if (version.type == PM_TYPE_PM8X50_FLEXE) {
        /* revision number will be checked inside phymod_phy_tdm_mode_update */
        _SOC_IF_ERR_EXIT
            (_pm8x50_tdm_mode_update(unit, port, pm_info, 0));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_speed_config_get(int unit, int port, pm_info_t pm_info, portmod_speed_config_t* speed_config)
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
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    PORTMOD_BIT_COUNT(phy_access.access.lane_mask, port_num_lanes);

    _SOC_IF_ERR_EXIT(pm8x50_port_autoneg_status_get(unit, port, pm_info, &an_status));

    if (an_status.enabled && an_status.locked) {
        speed_config->num_lane = an_status.resolved_num_lane;
    } else {
        speed_config->num_lane = port_num_lanes;
    }

    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_t_init(&phy_speed_config));
    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_get(&phy_access, &phy_speed_config));

    /* for the RLM speed mode, need to update the num_lane */
    if ((phy_speed_config.data_rate == 150000) ||
        (phy_speed_config.data_rate == 350000) ||
        (phy_speed_config.data_rate == 300000)) {
        speed_config->num_lane = phy_speed_config.data_rate / 50000;
    }

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
int pm8x50_port_cl72_set(int unit, int port, pm_info_t pm_info, uint32 enable)
{
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    if (!SAL_BOOT_SIMULATION) {
        _SOC_IF_ERR_EXIT
            (phymod_phy_cl72_set(&phy_access, enable));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_cl72_get(int unit, int port, pm_info_t pm_info, uint32* enable)
{
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    if (!SAL_BOOT_SIMULATION) {
        _SOC_IF_ERR_EXIT
            (phymod_phy_cl72_get(&phy_access, enable));
    }

exit:
    SOC_FUNC_RETURN;
}

/*Get port cl72 status*/
int pm8x50_port_cl72_status_get(int unit, int port, pm_info_t pm_info,
                                phymod_cl72_status_t* status)
{
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    if (!SAL_BOOT_SIMULATION) {
        _SOC_IF_ERR_EXIT
            (phymod_phy_cl72_status_get(&phy_access, status));
    }

exit:
    SOC_FUNC_RETURN;
}

/*Port loopback set\get*/
int pm8x50_port_loopback_set(int unit, int port, pm_info_t pm_info,
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

    _SOC_IF_ERR_EXIT(_pm8x50_phy_access_get(unit, port, pm_info, &phy_acc));

    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));

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
            _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                             &params, 1, &phy_access, &nof_phys, NULL));
            _SOC_IF_ERR_EXIT(phymod_phy_loopback_set(&phy_access, phymod_lb_type, enable));
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (
                      _SOC_MSG("unsupported loopback type %d"), loopback_type));
            break;
    }

    if (enable) {
        _SOC_IF_ERR_EXIT(pm8x50_port_lag_failover_status_toggle(unit, port, pm_info));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_loopback_get(int unit, int port, pm_info_t pm_info,
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
            _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
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
int pm8x50_port_rx_mac_enable_set(int unit, int port,
                                  pm_info_t pm_info, int enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_rx_enable_set(unit, port, enable));
exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_rx_mac_enable_get(int unit, int port,
                                  pm_info_t pm_info, int* enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_rx_enable_get(unit, port, enable));
exit:
    SOC_FUNC_RETURN;
}

/*Port TX MAC ENABLE set\get*/
int pm8x50_port_tx_mac_enable_set(int unit, int port,
                                  pm_info_t pm_info,
                                  int enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_tx_enable_set(unit, port, enable));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_tx_mac_enable_get(int unit, int port,
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
int pm8x50_port_speed_ability_local_get(int unit, int port, pm_info_t pm_info,
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
    pm8x50_fs_ability_table_entry_t fs_table_entry;
    pm8x50_an_ability_table_entry_t an_table_entry;
    pm_version_t version;
    SOC_INIT_FUNC_DEFS;

    *num_abilities = 0;
    sal_memset(&fs_table_entry, 0, sizeof(pm8x50_fs_ability_table_entry_t));
    sal_memset(&an_table_entry, 0, sizeof(pm8x50_an_ability_table_entry_t));

    /* find the max speed of the port */
    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get (unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_MAX_SPEED_GET(unit, pm_info, &max_speed, port_index));

    /* find number of lanes of the port */
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    port_lane_mask = phy_access.access.lane_mask;
    PORTMOD_BIT_COUNT(port_lane_mask, port_num_lanes);

    /* get PM version. */
    _SOC_IF_ERR_EXIT(_pm8x50_pm_version_get(unit, pm_info, &version));

    if ((version.tech_process == PM_TECH_16NM) && (version.type == PM_TYPE_PM8X50)) {
        fs_table_size = PM8x50_FS_ABILITY_TABLE_SIZE;
        an_table_size = PM8x50_AN_ABILITY_TABLE_SIZE;
    } else {
        fs_table_size = PM8x50_GEN2_FS_ABILITY_TABLE_SIZE;
        an_table_size = PM8x50_GEN2_AN_ABILITY_TABLE_SIZE;
    }
    for (i = 0; i < fs_table_size; i++) {
        if ((version.tech_process == PM_TECH_16NM) && (version.type == PM_TYPE_PM8X50)) {
            _SOC_IF_ERR_EXIT
                (_pm8x50_fs_ability_table_read_entry(i, &fs_table_entry));
        } else {
            _SOC_IF_ERR_EXIT
                (_pm8x50_gen2_fs_ability_table_read_entry(i, &fs_table_entry));
        }
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
        if ((version.tech_process == PM_TECH_16NM) && (version.type == PM_TYPE_PM8X50)) {
            _SOC_IF_ERR_EXIT
                (_pm8x50_an_ability_table_read_entry(i, &an_table_entry));
        } else {
            _SOC_IF_ERR_EXIT
                (_pm8x50_gen2_an_ability_table_read_entry(i, &an_table_entry));
        }
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
int _pm8x50_port_port_to_phy_ability(int num_abilities,
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
int _pm8x50_port_phy_to_port_ability(phymod_autoneg_advert_abilities_t* an_advert_abilities,
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

/*Set/get port auto negotiation ability*/
int pm8x50_port_autoneg_ability_advert_set(int unit, int port, pm_info_t pm_info,
                                           int num_abilities,
                                           const portmod_port_speed_ability_t* abilities)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access, phy_copy;
    int nof_phys, i, port_index, mpp_index;
    uint32 bitmap, flags = 0;
    portmod_vco_type_t request_vco = 0;
    pm8x50_vcos_bmp_t vcos = 0;
    portmod_pm_vco_setting_t vco_setting;
    portmod_speed_config_t speed_config;
    portmod_pbmp_t ports;
    phymod_autoneg_advert_ability_t autoneg_abilities[PM8x50_MAX_AN_ABILITY];
    phymod_autoneg_advert_abilities_t an_advert_abilities;
    int is_msa = 0, is_bam = 0, current_media_type = 0;
    int is_dual_50g = 0, pam4_req = 0, cl73_bam_msa_base_r_req = 0, dual_50g_req = 0;
    int rs544_req = 0, rs528_req = 0, rs272_req = 0;
    int req_100g_4lane_no_fec = 0, req_100g_4lane_with_fec = 0;
    uint32_t current_100g_an_fec_override = 0;
    phymod_an_mode_type_t an_mode = 0, cur_an_mode = 0;
    int flag;
    int fec_cl73, fec_cl73_25g, fec_cl73bam_msa, fec_rs528_bam_msa;
    phymod_phy_pll_state_t old_pll_state, new_pll_state;
    uint8 pll_lanes_bitmap, rs528_bm = 0, rs544_bm = 0, rs272_bm = 0, affected_bm = 0;
    phymod_firmware_lane_config_t fw_lane_config;
    int tvco_pll_index, ovco_pll_index;
    portmod_fec_t fec;
    pm_version_t version;
    uint32_t pll_div;

    SOC_INIT_FUNC_DEFS;

    fec_cl73 = PORTMOD_PORT_PHY_FEC_INVALID;
    fec_cl73_25g = PORTMOD_PORT_PHY_FEC_INVALID;
    fec_cl73bam_msa = PORTMOD_PORT_PHY_FEC_INVALID;
    fec_rs528_bam_msa = PORTMOD_PORT_PHY_FEC_INVALID;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(_pm8x50_pm_version_get(unit, pm_info, &version));

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
        if ((version.tech_process == PM_TECH_16NM) && (version.type == PM_TYPE_PM8X50)) {
            _SOC_IF_ERR_EXIT(_pm8x50_an_ability_table_vco_get(abilities[i].speed, abilities[i].num_lanes,
                             abilities[i].fec_type, abilities[i].an_mode, &request_vco));
        } else {
            _SOC_IF_ERR_EXIT(_pm8x50_gen2_an_ability_table_vco_get(abilities[i].speed, abilities[i].num_lanes,
                             abilities[i].fec_type, abilities[i].an_mode, &request_vco));
        }
        /* 2.1. AN ability is supported by HW */
        if (request_vco == portmodVCOInvalid) {
            _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                               (_SOC_MSG("port %d: abilities[%d] is not supported."), port, i));
        }

        if (abilities[i].speed / abilities[i].num_lanes > NRZ_MAX_PER_LANE_SPEED) {
            pam4_req = 1;
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
            PM8x50_VCO_ETH_20P625G_SET(vcos);
        } else if (request_vco == portmodVCO25P781G) {
            PM8x50_VCO_ETH_25P781G_SET(vcos);
        } else if (request_vco == portmodVCO26P562G) {
            PM8x50_VCO_ETH_26P562G_SET(vcos);
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
        an_mode = phymod_AN_MODE_CL73BAM;
    } else if (is_msa) {
        an_mode = phymod_AN_MODE_CL73_MSA;
    } else {
        an_mode = phymod_AN_MODE_CL73;
    }

    /* within same MPP, MSA and BAM can not be exist at same time */
    sal_memcpy(&phy_copy, &phy_access, sizeof(phy_copy));
    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    mpp_index = (int)(port_index / 4);
    for (i = (0 + (mpp_index * 4)); i < (4 + (mpp_index*4)); i++) {
        if (i == port_index) {
            continue;
        }
        _SOC_IF_ERR_EXIT(PM8x50_AN_MODE_GET(unit, pm_info, &cur_an_mode, i));
        if ((an_mode == phymod_AN_MODE_CL73_MSA) && (cur_an_mode == phymod_AN_MODE_CL73BAM)) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                (_SOC_MSG("port %d: CL73_MSA cannot be advertised when current MPP is in CL73_BAM."), port));
        }
        if ((an_mode == phymod_AN_MODE_CL73BAM) && (cur_an_mode == phymod_AN_MODE_CL73_MSA)) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                (_SOC_MSG("port %d: CL73_BAM cannot be advertised when current MPP is in CL73_MSA."), port));
        }
    }

    /* 2.3. FEC arch can be supported within the each MPP */
    if (rs544_req || rs528_req || rs272_req) {
        _SOC_IF_ERR_EXIT(_pm8x50_fec_lanebitmap_get(unit, pm_info, &rs528_bm, &rs544_bm, &rs272_bm));
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
        if ((version.tech_process == PM_TECH_16NM) && (version.type == PM_TYPE_PM8X50)) {
            PM8X50_PM_VERSION_F_REV0_16NM_SET(flags);
        }
        _SOC_IF_ERR_EXIT(_pm8x50_fec_validate(unit, flags, rs528_bm, rs544_bm, rs272_bm, &affected_bm));
    }

    /* 2.4. AN ability can be supported by current VCOs */
    if (pam4_req && (PM_8x50_INFO(pm_info)->tvco == portmodVCO20P625G)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("Can not support Ethernet PAM4 speeds when TVCO is 20.625G.")));
    }

    _SOC_IF_ERR_EXIT(
        PM8x50_OVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll_lanes_bitmap));
    if (pll_lanes_bitmap & phy_access.access.lane_mask) {
        /* Current port is using OVCO, so that OVCO can not be changed. */
        flag = PORTMOD_PM_SPEED_VALIDATE_F_PLL_SWITCH_NOT_ALLOWED;
    } else {
        flag = PORTMOD_PM_SPEED_VALIDATE_F_ONE_PLL_SWITCH_ALLOWED;
    }

    /*
     * power up clock repeater buffer if it is powered down
     * for DNX device only
     */
    if (PM_8x50_INFO(pm_info)->clock_buffer_disable_required) {
        PHYMOD_IF_ERR_RETURN
            (phymod_phy_pll_clock_buffer_pwrdn(&phy_access, 0, 0, 0));
        PHYMOD_IF_ERR_RETURN
            (phymod_phy_pll_clock_buffer_pwrdn(&phy_access, 1, 0, 0));
    }

    PORTMOD_PBMP_PORT_SET(ports, port);
    _SOC_IF_ERR_EXIT(
       _pm8x50_vcos_setting_validate(unit, pm_info, &ports, vcos, flag, &vco_setting));

    an_advert_abilities.autoneg_abilities = autoneg_abilities;
    _SOC_IF_ERR_EXIT(_pm8x50_port_port_to_phy_ability(num_abilities, abilities, &an_advert_abilities));

    tvco_pll_index = PM8X50_TVCO_PLL_INDEX_GET(pm_info);
    ovco_pll_index = (tvco_pll_index == 1)? 0: 1;
    _SOC_IF_ERR_EXIT(
        PM8x50_OVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &pll_lanes_bitmap));
    if (ovco_pll_index == 1) {
        old_pll_state.pll1_lanes_bitmap = pll_lanes_bitmap;
    } else {
        old_pll_state.pll0_lanes_bitmap = pll_lanes_bitmap;
    }

    _SOC_IF_ERR_EXIT(
        PM8x50_TVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &pll_lanes_bitmap));
    if (tvco_pll_index == 1) {
        old_pll_state.pll1_lanes_bitmap = pll_lanes_bitmap;
    } else {
        old_pll_state.pll0_lanes_bitmap = pll_lanes_bitmap;
    }
    /* 3. Set AN abilities in HW */
    _SOC_IF_ERR_EXIT(
        phymod_phy_autoneg_advert_ability_set(&phy_access, &an_advert_abilities, &old_pll_state, &new_pll_state));

    /* 4. Update DB and WB */
    if (vco_setting.is_ovco_new) {
        PM_8x50_INFO(pm_info)->ovco = vco_setting.ovco;
    }

    /* 4.1. Update an_mode in WB */
    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get (unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_AN_MODE_SET(unit, pm_info, an_mode, port_index));

    /* 4.2. Update FEC type in WB for RS528, RS544 case */
    /* If AN request RS544, update DB and this will be treated as FEC arch restriction.
     * Else if PM is 16nm and AN request RS528, update DB and this will be treated as FEC arch restriction.
     * Else if AN request other FEC, keep the force speed FEC restriction. Need to update
     * FEC restrition when AN is enabled.
     */
    if (rs544_req) {
        fec = PORTMOD_PORT_PHY_FEC_RS_544;
        for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
            if ((phy_access.access.lane_mask >> i) & 0x1) {
                _SOC_IF_ERR_EXIT(PM8x50_LANE2FEC_SET(unit, pm_info, i, fec));
            }
        }
    } else if (rs272_req) {
        fec = PORTMOD_PORT_PHY_FEC_RS_272;
        for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
            if ((phy_access.access.lane_mask >> i) & 0x1) {
                _SOC_IF_ERR_EXIT(PM8x50_LANE2FEC_SET(unit, pm_info, i, fec));
            }
        }
    } else if (rs528_req && PM8X50_PM_VERSION_F_REV0_16NM_GET(flags)) {
        fec = PORTMOD_PORT_PHY_FEC_RS_FEC;
        for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
            if ((phy_access.access.lane_mask >> i) & 0x1) {
                _SOC_IF_ERR_EXIT(PM8x50_LANE2FEC_SET(unit, pm_info, i, fec));
            }
        }
    } else {
        /* Call speed_config_get to get the FEC type and update the WB */
        /* This is to make sure the DB stores the FS FEC instead of the previous adv FEC */
        _SOC_IF_ERR_EXIT(pm8x50_port_speed_config_get(unit, port, pm_info, &speed_config));
        for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
            if ((phy_access.access.lane_mask >> i) & 0x1) {
                _SOC_IF_ERR_EXIT(PM8x50_LANE2FEC_SET(unit, pm_info, i, speed_config.fec));
            }
        }
    }

    /* 4.3. Update PLL lane bit maps in WB, power down OVCO if it's free */
    _SOC_IF_ERR_EXIT(
        PM8x50_OVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll_lanes_bitmap));
    if (tvco_pll_index == 1) {
        /* Power down OVCO if it's not being used */
        if (!(pll_lanes_bitmap || new_pll_state.pll0_lanes_bitmap)) {
            _SOC_IF_ERR_EXIT(phymod_phy_pll_pwrdn(&phy_access, ovco_pll_index, 1));
            PM_8x50_INFO(pm_info)->ovco = portmodVCOInvalid;
        }
        pll_lanes_bitmap = (uint8) new_pll_state.pll0_lanes_bitmap;
        _SOC_IF_ERR_EXIT(
            PM8x50_OVCO_PLL_ADV_LANE_BITMAP_SET(unit, pm_info, pll_lanes_bitmap));
        pll_lanes_bitmap = (uint8) new_pll_state.pll1_lanes_bitmap;
        _SOC_IF_ERR_EXIT(
            PM8x50_TVCO_PLL_ADV_LANE_BITMAP_SET(unit, pm_info, pll_lanes_bitmap));
    } else {
        /* Power down OVCO if it's not being used */
        if (!(pll_lanes_bitmap || new_pll_state.pll1_lanes_bitmap)) {
            _SOC_IF_ERR_EXIT(phymod_phy_pll_pwrdn(&phy_access, ovco_pll_index, 1));
            PM_8x50_INFO(pm_info)->ovco = portmodVCOInvalid;
        }
        pll_lanes_bitmap = (uint8) new_pll_state.pll0_lanes_bitmap;
        _SOC_IF_ERR_EXIT(
            PM8x50_TVCO_PLL_ADV_LANE_BITMAP_SET(unit, pm_info, pll_lanes_bitmap));
        pll_lanes_bitmap = (uint8) new_pll_state.pll1_lanes_bitmap;
        _SOC_IF_ERR_EXIT(
            PM8x50_OVCO_PLL_ADV_LANE_BITMAP_SET(unit, pm_info, pll_lanes_bitmap));
    }

    /* update rx clock repeater buffer bandwidth, for DNX device only */
    if (PM_8x50_INFO(pm_info)->clock_buffer_bw_update_required) {
        /* tvco will never be powered down, only is_ovco_new need to be checked here */
        if ((ovco_pll_index == 0) &&
            (vco_setting.is_ovco_new) &&
            (PM_8x50_INFO(pm_info)->ovco != portmodVCOInvalid)) {
            /* Get PLL_DIV if PLLO is new */
            _SOC_IF_ERR_EXIT(
                _pm8x50_vco_to_pll_get(PM_8x50_INFO(pm_info)->ref_clk, PM_8x50_INFO(pm_info)->ovco, &pll_div));
            /* update rx clock repeater buffer bandwidth */
            _SOC_IF_ERR_EXIT(phymod_phy_pll_rx_clock_buffer_bw_update(&phy_access, 0/* pll0 */,
                pll_div, PM_8x50_INFO(pm_info)->ref_clk));
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_autoneg_ability_advert_get(int unit, int port, pm_info_t pm_info,
                                           int max_num_abilities,
                                           portmod_port_speed_ability_t* abilities,
                                           int* actual_num_abilities)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, i;
    phymod_autoneg_advert_ability_t autoneg_abilities[PM8x50_MAX_AN_ABILITY];
    phymod_autoneg_advert_abilities_t an_advert_abilities;

    SOC_INIT_FUNC_DEFS;

    for (i = 0; i < PM8x50_MAX_AN_ABILITY; i++) {
        _SOC_IF_ERR_EXIT(phymod_autoneg_advert_ability_t_init(&autoneg_abilities[i]));
    }
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    an_advert_abilities.autoneg_abilities = autoneg_abilities;
    _SOC_IF_ERR_EXIT(phymod_phy_autoneg_advert_ability_get(&phy_access, &an_advert_abilities));

    if (an_advert_abilities.num_abilities > max_num_abilities) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("port %d: There are %d AN abilities. Larger array is needed."),
                           port, an_advert_abilities.num_abilities));
    } else {
        _SOC_IF_ERR_EXIT(_pm8x50_port_phy_to_port_ability(&an_advert_abilities, actual_num_abilities, abilities));
    }

exit:
    SOC_FUNC_RETURN;
}

/*Port ability remote Adv get*/
int pm8x50_port_autoneg_ability_remote_get(int unit, int port, pm_info_t pm_info,
                                           int max_num_abilities,
                                           portmod_port_speed_ability_t* abilities,
                                           int* actual_num_abilities)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, i;
    phymod_autoneg_advert_ability_t autoneg_abilities[PM8x50_MAX_AN_ABILITY];
    phymod_autoneg_advert_abilities_t an_advert_abilities;
    int port_index, is_400g_an = 0, remote_pause = 0;
    uint32 bitmap;
    phymod_autoneg_status_t an_status;

    SOC_INIT_FUNC_DEFS;

    for (i = 0; i < PM8x50_MAX_AN_ABILITY; i++) {
        _SOC_IF_ERR_EXIT(phymod_autoneg_advert_ability_t_init(&autoneg_abilities[i]));
    }

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    an_advert_abilities.autoneg_abilities = autoneg_abilities;

    _SOC_IF_ERR_EXIT(phymod_phy_autoneg_remote_advert_ability_get(&phy_access, &an_advert_abilities));

    /* next need to check 400G AN enabled or not */
    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_400G_AN_ENABLE_GET(unit, pm_info, &is_400g_an, port_index));
    _SOC_IF_ERR_EXIT(pm8x50_port_autoneg_status_get(unit, port, pm_info, &an_status));
    if (is_400g_an && an_status.enabled && an_status.locked) {
        _SOC_IF_ERR_EXIT(PM8x50_400G_AN_REMOTE_PAUSE_GET(unit, pm_info, &remote_pause, port_index));
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
        _SOC_IF_ERR_EXIT(_pm8x50_port_phy_to_port_ability(&an_advert_abilities, actual_num_abilities, abilities));
    }

exit:
    SOC_FUNC_RETURN;
}

/*Set\Get autoneg*/
int pm8x50_port_autoneg_set(int unit, int port, pm_info_t pm_info,
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
    uint8_t pll_lanes_bitmap;
    int tvco_pll_index, ovco_pll_index, i;
    portmod_port_speed_ability_t abilities[PM8x50_MAX_AN_ABILITY];
    portmod_fec_t fec;
    pm_version_t version;
    int portmon_disable;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_pm_version_get(unit, pm_info, &version));

    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    PORTMOD_BIT_COUNT(phy_access.access.lane_mask, port_num_lanes);
    tvco_pll_index = PM8X50_TVCO_PLL_INDEX_GET(pm_info);
    ovco_pll_index = (tvco_pll_index == 1)? 0: 1;

    an_control = *an;

    an_control.num_lane_adv = port_num_lanes;

    /* Get an_mode from WB */
    _SOC_IF_ERR_EXIT(PM8x50_AN_MODE_GET(unit, pm_info, &an_control.an_mode, port_index));
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
        _SOC_IF_ERR_EXIT(pm8x50_port_autoneg_ability_advert_get(unit, port, pm_info,
                                                                PM8x50_MAX_AN_ABILITY,
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
        if (rs544_req || (rs528_req && version.tech_process == PM_TECH_16NM)) {
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
        _SOC_IF_ERR_EXIT(_pm8x50_pm_port_mode_update(unit, port, pm_info, port_index, 1));
    }

    _SOC_IF_ERR_EXIT(phymod_phy_autoneg_set(&phy_access, &an_control));

    if (an_control.enable) {
        /* When AN is enable on a port, always connect that port to TVCO */
        _SOC_IF_ERR_EXIT(
            PM8x50_OVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll_lanes_bitmap));
        pll_lanes_bitmap &= ~phy_access.access.lane_mask;
        _SOC_IF_ERR_EXIT(
            PM8x50_OVCO_PLL_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll_lanes_bitmap));

        /* If OVCO is not being used, power down OVCO */
        if (!pll_lanes_bitmap) {
            _SOC_IF_ERR_EXIT(
                PM8x50_OVCO_PLL_ADV_LANE_BITMAP_GET(unit, pm_info, &pll_lanes_bitmap));
            if (!pll_lanes_bitmap) {
                _SOC_IF_ERR_EXIT(phymod_phy_pll_pwrdn(&phy_access, ovco_pll_index, 1));
                PM_8x50_INFO(pm_info)->ovco = portmodVCOInvalid;
            }
        }

        _SOC_IF_ERR_EXIT(
            PM8x50_TVCO_PLL_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll_lanes_bitmap));
        pll_lanes_bitmap |= phy_access.access.lane_mask;
        _SOC_IF_ERR_EXIT(
            PM8x50_TVCO_PLL_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll_lanes_bitmap));

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
        for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
            if ((phy_access.access.lane_mask >> i) & 0x1) {
                _SOC_IF_ERR_EXIT(PM8x50_LANE2FEC_SET(unit, pm_info, i, fec));
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
    _SOC_IF_ERR_EXIT(PM8x50_400G_AN_ENABLE_SET(unit, pm_info, is_400g_an, port_index));
    /* if 400G an enabled, needs to enable call back */
    /* call back register should happen after 400G AN ENABLE is set */
    if (is_400g_an) {
         pm8x50_port_400g_an_state_t an_state = PM8X50_PORT_AN_CHECK_PAGE_COMPLETE;
         uint32 an_wait_link_count = 0;
        /* update the 400G An state */
        _SOC_IF_ERR_EXIT(PM8x50_400G_AN_STATE_SET(unit, pm_info, an_state));
        _SOC_IF_ERR_EXIT(PM8x50_400G_AN_WAIT_LINK_COUNT_SET(unit, pm_info, an_wait_link_count));
        _SOC_IF_ERR_EXIT(portmod_port_periodic_callback_register(unit, port, pm8x50_periodic_callback, PORTMOD_THREAD_INTERVAL_1s));
    }


exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_autoneg_get(int unit, int port, pm_info_t pm_info,
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
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_autoneg_get(&phy_access, an, &an_done));

    /* next need to check 400G AN enabled or not */
    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_400G_AN_ENABLE_GET(unit, pm_info, &is_400g_an, port_index));
    if (is_400g_an) {
         an->enable = 1;
         an->an_mode = phymod_AN_MODE_CL73_MSA;
    }


exit:
    SOC_FUNC_RETURN;
}

/*Get autoneg status*/
int pm8x50_port_autoneg_status_get(int unit, int port, pm_info_t pm_info,
                                   phymod_autoneg_status_t* an_status)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, port_index, is_400g_an = 0, an_400g_state;
    uint32 bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(phymod_phy_autoneg_status_get(&phy_access, an_status));
    
    if (!(an_status->enabled && an_status->locked)) {
        /* upper layer should not rely on the
         * data rate if the AN is not locked
         */
        an_status->data_rate = 0;
        an_status->interface = SOC_PORT_IF_NULL;
    }
    /* next need to check 400G AN enabled or not */
    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_400G_AN_ENABLE_GET(unit, pm_info, &is_400g_an, port_index));
    if (is_400g_an) {
        an_status->data_rate = 400000;
        an_status->enabled = 1;
        an_status->locked = 0;
        _SOC_IF_ERR_EXIT(PM8x50_400G_AN_STATE_GET(unit, pm_info, &an_400g_state));
        if (an_400g_state == PM8X50_PORT_AN_COMPLETE) {
            an_status->locked = 1;
        }
    }

exit:
    SOC_FUNC_RETURN;
}

/*get link status*/
int pm8x50_port_link_get(int unit, int port, pm_info_t pm_info,
                         int flags, int* link)
{
    int latch_val = 0;
    uint32 rval, bitmap;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, port_index, is_400g_an = 0, an_400g_state;
    uint32 fec_bypass_indication;
    int is_3_cw_bad_enable =0, three_cw_bad_tick_cnt = 0, three_cw_bad_state = 0;
    int link_status = 0, is_flexe_rev1 = 0;
    uint32_t prbs_tx_en = 0, prbs_rx_en=0, prbs_flag;
    phymod_core_info_t info;
    uint32 reg_val, is_400g_port = 0;
    uint32_t signal_detect, three_bad;
    uint32_t rlm_state, an_done;
    int rlm_enabled;
    phymod_autoneg_control_t an;
    int phy_acc;
    int portmon_disable = 0;
    int is_done = 0;

    SOC_INIT_FUNC_DEFS;

    if (SAL_BOOT_SIMULATION){
        _SOC_IF_ERR_EXIT(pm8x50_port_mac_link_get(unit, port, pm_info, link));
    } else {
        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
        _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                        &params, 1, &phy_access, &nof_phys, NULL));
        _SOC_IF_ERR_EXIT(_pm8x50_phy_access_get(unit, port, pm_info, &phy_acc));
        _SOC_IF_ERR_EXIT(READ_CDPORT_MODE_REGr(unit, phy_acc, &reg_val));
        is_400g_port = soc_reg_field_get(unit, CDPORT_MODE_REGr, reg_val, SINGLE_PORT_MODE_SPEED_400Gf);
        if (is_400g_port) {
            _SOC_IF_ERR_EXIT(
                pm8x50_port_link_latch_down_get(unit, port, pm_info, PORTMOD_PORT_LINK_LATCH_DOWN_F_CLEAR, &latch_val));
            if (latch_val) {
                *link = 0;
            } else {
                _SOC_IF_ERR_EXIT(
                    READ_CDPORT_XGXS0_LN0_STATUS_REGr(unit, port, &rval));
                *link = soc_reg_field_get(unit, CDPORT_XGXS0_LN0_STATUS_REGr, rval, LINK_STATUSf);
            }
            /* next need to check 400G AN enabled or not */
            _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, &bitmap));
            _SOC_IF_ERR_EXIT(PM8x50_400G_AN_ENABLE_GET(unit, pm_info, &is_400g_an, port_index));
            if (is_400g_an) {
                _SOC_IF_ERR_EXIT(PM8x50_400G_AN_STATE_GET(unit, pm_info, &an_400g_state));
                if (an_400g_state != PM8X50_PORT_AN_COMPLETE) {
                     *link = 0;
                } else {
                    /* need to check link status, if down, need to restart 400G AN */
                    if (*link == 0) {
                         pm8x50_port_400g_an_state_t an_state = PM8X50_PORT_AN_DISABLE;
                         uint32 an_wait_link_count = 0;
                        /* update the 400G An state */
                        _SOC_IF_ERR_EXIT(PM8x50_400G_AN_STATE_SET(unit, pm_info, an_state));
                        _SOC_IF_ERR_EXIT(PM8x50_400G_AN_WAIT_LINK_COUNT_SET(unit, pm_info, an_wait_link_count));
                        _SOC_IF_ERR_EXIT(portmod_port_periodic_callback_register(unit, port, pm8x50_periodic_callback, PORTMOD_THREAD_INTERVAL_1s));
                   }
                }
            }
        } else {
            _SOC_IF_ERR_EXIT(phymod_phy_link_status_get(&phy_access,
                                                       (uint32_t *) link));
        }

        _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, &bitmap));
        link_status = *link;

        /*FlexE rev num 1 does not need the SW recovery*/
        _SOC_IF_ERR_EXIT
            (phymod_core_info_get(&PM_8x50_INFO(pm_info)->int_core_access, &info));
        is_flexe_rev1 = ((info.core_version == phymodCoreVersionTscbhFlexE) &&
                        (((info.serdes_id >> 14) & 0x3) == 0) &&
                        (((info.serdes_id >> 11) & 0x7) == 1)) ? 1 : 0;
        _SOC_IF_ERR_EXIT
            (portmod_portmon_disable_get(unit, &portmon_disable));
        if (!link_status) {
            if (!is_flexe_rev1) {
                _SOC_IF_ERR_EXIT
                    (PM8x50_3_CW_BAD_ENABLE_GET(unit, pm_info, &is_3_cw_bad_enable, port_index));
                if (!is_3_cw_bad_enable) {
                    _SOC_IF_ERR_EXIT
                        (phymod_phy_rx_signal_detect_get(&phy_access, &signal_detect));
                    _SOC_IF_ERR_EXIT
                        (phymod_phy_fec_three_cw_bad_state_get(&phy_access, &three_bad));
                    if (signal_detect && three_bad) {
                        _SOC_IF_ERR_EXIT
                            (PM8x50_RLM_ENABLE_GET(unit, pm_info, &rlm_enabled, port_index));
                        _SOC_IF_ERR_EXIT
                            (PM8x50_RLM_STATE_GET(unit, pm_info, &rlm_state, port_index));
                        /* Don`t enable the three cw bad recovery,
                         * if rlm is enabled and rlm configuration
                         * is not done.
                         */
                        if (rlm_enabled && rlm_state != PORTMOD_PORT_RLM_DONE) {
                            return SOC_E_NONE;
                        }
                        _SOC_IF_ERR_EXIT
                            (phymod_phy_fec_bypass_indication_get(&phy_access, &fec_bypass_indication));
                        if (fec_bypass_indication) {
                            return SOC_E_NONE;
                        }
                        prbs_flag = 0;
                        PHYMOD_PRBS_DIRECTION_RX_CLR(prbs_flag);
                        PHYMOD_PRBS_DIRECTION_TX_SET(prbs_flag);
                        _SOC_IF_ERR_EXIT
                            (phymod_phy_prbs_enable_get(&phy_access, prbs_flag, &prbs_tx_en));
                        PHYMOD_PRBS_DIRECTION_TX_CLR(prbs_flag);
                        PHYMOD_PRBS_DIRECTION_RX_SET(prbs_flag);
                        _SOC_IF_ERR_EXIT
                            (phymod_phy_prbs_enable_get(&phy_access, prbs_flag, &prbs_rx_en));
                        /* Don`t register the three cw bad recovery,
                         * if prbs is enabled.
                         */
                        if (prbs_tx_en || prbs_rx_en) {
                            return SOC_E_NONE;
                        }
                        _SOC_IF_ERR_EXIT
                            (phymod_phy_autoneg_get(&phy_access, &an, &an_done));
                        _SOC_IF_ERR_EXIT
                            (PM8x50_400G_AN_ENABLE_GET(unit, pm_info, &is_400g_an, port_index));
                        /* Don`t register the three cw bad recovery,
                         * if an is enabled.
                         */
                        if (an.enable || is_400g_an) {
                            return SOC_E_NONE;
                        }
                        three_cw_bad_tick_cnt = 0;
                        three_cw_bad_state = PM8X50_PORT_3_CW_BAD_RECOVER;
                        is_3_cw_bad_enable = 1;
                        _SOC_IF_ERR_EXIT
                            (PM8x50_3_CW_BAD_STATE_SET(unit, pm_info, three_cw_bad_state, port_index));
                        _SOC_IF_ERR_EXIT
                            (PM8x50_3_CW_BAD_TICK_CNT_SET(unit, pm_info, three_cw_bad_tick_cnt, port_index));
                        _SOC_IF_ERR_EXIT
                            (PM8x50_3_CW_BAD_ENABLE_SET(unit, pm_info, is_3_cw_bad_enable, port_index));
                        if (!portmon_disable) {
                            _SOC_IF_ERR_EXIT
                                (portmod_port_periodic_callback_register(unit, port, pm8x50_periodic_callback, PORTMOD_THREAD_INTERVAL_1s));
                        }
                    }
                }
            }
        }

        /*Recover the three cw bad in link get in case of port monitor is disabled. */
        _SOC_IF_ERR_EXIT
            (PM8x50_3_CW_BAD_ENABLE_GET(unit, pm_info, &is_3_cw_bad_enable, port_index));
        if (is_3_cw_bad_enable && !is_flexe_rev1 && portmon_disable) {
            _SOC_IF_ERR_EXIT
                (_pm8x50_three_cw_bad_recover(unit, port, pm_info, &is_done));
            if (!is_done) {
                *link = 0;
            }
        }
    }
exit:
    SOC_FUNC_RETURN;
}

/*get latch down link status (whether link was ever down since last clear)*/
int pm8x50_port_link_latch_down_get(int unit, int port,
                                    pm_info_t pm_info,
                                    uint32 flags, int* link)
{
    int first_index;
    uint32 rval, bitmap;
    soc_field_t status_field;
    SOC_INIT_FUNC_DEFS;

    *link = 0;

    _SOC_IF_ERR_EXIT(
        _pm8x50_port_index_get(unit, port, pm_info, &first_index, &bitmap));

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
int pm8x50_port_phy_link_up_event(int unit, int port, pm_info_t pm_info)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    phymod_autoneg_status_t an_status;
    int nof_phys, port_index = -1;
    int mac_stage_id, phy_acc;
    uint32 reg_val;
    uint32 timesync_enable, bitmap, flags = 0;
    int is_ber_war_enable = 0;
    uint32_t ber_war_wait_count = 0, fec_bypass_indication = 0;
    pm8x50_port_ber_war_state_t state;
    portmod_speed_config_t speed_config;
    int num_advert, num_remote, is_400g_an;
    portmod_port_speed_ability_t advert_ability[PM8x50_MAX_AN_ABILITY];
    portmod_port_speed_ability_t remote_ability[PM8x50_MAX_AN_ABILITY];
    portmod_pause_control_t pause_ctrl;
    portmod_port_phy_pause_t pause_local, pause_remote;
    uint8 rx_pause, tx_pause;
    int portmon_disable;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, &bitmap));

    /* 1. Update for AN ports */
    _SOC_IF_ERR_EXIT(pm8x50_port_autoneg_status_get(unit, port, pm_info, &an_status));
    if ((an_status.enabled && an_status.locked)) {
        /* read the 400G an enabled */
        _SOC_IF_ERR_EXIT(PM8x50_400G_AN_ENABLE_GET(unit, pm_info, &is_400g_an, port_index));
        /*!
         * 1.1 Update port mode.
         * For PM8x50 autoneg, once the port resolves and link up,
         * cdmac_port_mode needs to be updated based on the PCS final port mode.
         */
        if (!is_400g_an) {
            mac_stage_id = port_index / CDMAC_NUM_LANES;
            _SOC_IF_ERR_EXIT(_pm8x50_phy_access_get(unit, port, pm_info, &phy_acc));

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
            (pm8x50_port_autoneg_ability_advert_get(unit, port, pm_info,
                                                    PM8x50_MAX_AN_ABILITY,
                                                    advert_ability, &num_advert));
        _SOC_IF_ERR_EXIT
            (pm8x50_port_autoneg_ability_remote_get(unit, port, pm_info,
                                                    PM8x50_MAX_AN_ABILITY,
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
            (pm8x50_port_pause_control_get(unit, port, pm_info, &pause_ctrl));
        if ((pause_ctrl.rx_enable != rx_pause) ||
            (pause_ctrl.tx_enable != tx_pause)) {
            pause_ctrl.rx_enable = rx_pause;
            pause_ctrl.tx_enable = tx_pause;
            _SOC_IF_ERR_EXIT
                (pm8x50_port_pause_control_set(unit, port, pm_info, &pause_ctrl));
        }
    }


    /* 2. Adjust timesync settings for 1588 port. */
    _SOC_IF_ERR_EXIT(PM8x50_TIMESYNC_ENABLE_GET(unit, pm_info, &timesync_enable, port_index));

    if (timesync_enable) {
        /* If 1588 is enabled on the port, need to adjust timesync during link up event.
         * rx_ts_update will be enabled at the end of adjust function.
         * Current driver code only support EarliestLane mode.
         */
        _SOC_IF_ERR_EXIT(phymod_timesync_adjust_set(&phy_access, 0, phymodTimesyncCompensationModeEarliestLane));
        PHYMOD_TIMESYNC_ENABLE_F_RX_SET(flags);
        _SOC_IF_ERR_EXIT(phymod_timesync_enable_set(&phy_access, flags, 1));
        /* Wait for one AM spacing time */
        sal_usleep(500);
    }

    /* 3. register callback function for BER WAR, when FEC bypass indication is enabled for 200G and 400G */
    _SOC_IF_ERR_EXIT(pm8x50_port_speed_config_get(unit, port, pm_info, &speed_config));
    if ((speed_config.speed == 200000) || (speed_config.speed == 400000)) {
        _SOC_IF_ERR_EXIT(phymod_phy_fec_bypass_indication_get(&phy_access, &fec_bypass_indication));
        if (fec_bypass_indication == 1) {
            _SOC_IF_ERR_EXIT
                (portmod_portmon_disable_get(unit, &portmon_disable));
            if (portmon_disable) {
                return SOC_E_NONE;
            }
            /* read current BER_WAR_ENABLE */
            _SOC_IF_ERR_EXIT(PM8x50_BER_WAR_ENABLE_GET(unit, pm_info, &is_ber_war_enable, port_index));
            if (!is_ber_war_enable) {
                /* if BER_WAR is not enabled yet, register callback function  */
                ber_war_wait_count = 0;
                state = PM8X50_PORT_BER_WAR_HI_SER_CHECK;
                _SOC_IF_ERR_EXIT(PM8x50_BER_WAR_STATE_SET(unit, pm_info, state, port_index));
                _SOC_IF_ERR_EXIT(PM8x50_BER_WAR_WAIT_COUNT_SET(unit, pm_info, ber_war_wait_count, port_index));
                _SOC_IF_ERR_EXIT(PM8x50_BER_WAR_ENABLE_SET(unit, pm_info, fec_bypass_indication, port_index));
                _SOC_IF_ERR_EXIT(portmod_port_periodic_callback_register(unit, port, pm8x50_periodic_callback, PORTMOD_THREAD_INTERVAL_1s));
            } else {
                /* reset BER WAR to initial state when link is UP */
                state = PM8X50_PORT_BER_WAR_HI_SER_CHECK;
                _SOC_IF_ERR_EXIT(PM8x50_BER_WAR_STATE_SET(unit, pm_info, state, port_index));
                ber_war_wait_count = 0;
                _SOC_IF_ERR_EXIT(PM8x50_BER_WAR_WAIT_COUNT_SET(unit, pm_info, ber_war_wait_count, port_index));
            }
        }
    }
exit:
    SOC_FUNC_RETURN;
}

/* link down event */
/* 1. 1588 port: Disable rx_ts_update.
 */
int pm8x50_port_phy_link_down_event(int unit, int port, pm_info_t pm_info)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, port_index = -1;
    uint32 timesync_enable, bitmap, flags = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_TIMESYNC_ENABLE_GET(unit, pm_info, &timesync_enable, port_index));

    if (timesync_enable) {
        /* If link down and 1588 is enabled on the port,
         * Need to disable rx_ts_update.
         */
        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
        _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                    &params, 1, &phy_access, &nof_phys, NULL));
        PHYMOD_TIMESYNC_ENABLE_F_RX_SET(flags);
        _SOC_IF_ERR_EXIT(phymod_timesync_enable_set(&phy_access, flags, 0));
    }

exit:
    SOC_FUNC_RETURN;
}

/*PRBS configuration set/get*/
int pm8x50_port_prbs_config_set(int unit, int port, pm_info_t pm_info,
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
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_prbs_config_set(&phy_access, flags, config));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_prbs_config_get(int unit, int port, pm_info_t pm_info,
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
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_prbs_config_get(&phy_access, flags, config));

exit:
    SOC_FUNC_RETURN;
}

/*PRBS enable set/get*/
int pm8x50_port_prbs_enable_set(int unit, int port, pm_info_t pm_info,
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
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_prbs_enable_set(&phy_access, flags, enable));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_prbs_enable_get(int unit, int port, pm_info_t pm_info,
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
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_prbs_enable_get(&phy_access, flags,
                                                (uint32_t *) enable));

exit:
    SOC_FUNC_RETURN;
}

/*PRBS status get*/
int pm8x50_port_prbs_status_get(int unit, int port, pm_info_t pm_info,
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
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_prbs_status_get(&phy_access, flags, status));

exit:
    SOC_FUNC_RETURN;
}

/*Port tx taps set\get*/
int pm8x50_port_tx_set(int unit, int port, pm_info_t pm_info, const phymod_tx_t* tx)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_tx_set(&phy_access, tx));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_tx_get(int unit, int port, pm_info_t pm_info, phymod_tx_t* tx)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_tx_get(&phy_access, tx));

exit:
    SOC_FUNC_RETURN;
}

/*Number of lanes get*/
int pm8x50_port_nof_lanes_get(int unit, int port, pm_info_t pm_info,
                              int* nof_lanes)
{
    int port_index;
    uint32_t bitmap, bcnt = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get (unit, port, pm_info, &port_index, &bitmap));

    while (bitmap) {
        if (bitmap & 0x1) bcnt++;
        bitmap >>= 1;
    }
    *nof_lanes = bcnt;

exit:
    SOC_FUNC_RETURN;
}

/*Set port PHYs' firmware mode*/
int pm8x50_port_firmware_mode_set(int unit, int port, pm_info_t pm_info,
                                  phymod_firmware_mode_t fw_mode)
{

    return (SOC_E_NONE);
}
int pm8x50_port_firmware_mode_get(int unit, int port, pm_info_t pm_info,
                                  phymod_firmware_mode_t* fw_mode)
{

    return (SOC_E_NONE);
}

/*Filter packets smaller than the specified threshold*/
int pm8x50_port_runt_threshold_set(int unit, int port,
                                   pm_info_t pm_info,
                                   int value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cdmac_runt_threshold_set(unit, port, value));
exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_runt_threshold_get(int unit, int port, pm_info_t pm_info,
                                   int* value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cdmac_runt_threshold_get(unit, port, value));
exit:
    SOC_FUNC_RETURN;
}

/*Filter packets bigger than the specified value*/
int pm8x50_port_max_packet_size_set(int unit, int port, pm_info_t pm_info,
                                    int value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cdmac_rx_max_size_set(unit, port , value));
exit:
    SOC_FUNC_RETURN;
}
int pm8x50_port_max_packet_size_get(int unit, int port, pm_info_t pm_info,
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
int pm8x50_port_pad_size_set(int unit, int port, pm_info_t pm_info, int value)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_pad_size_set(unit, port , value));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_pad_size_get(int unit, int port, pm_info_t pm_info, int* value)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_pad_size_get(unit, port , value));

exit:
    SOC_FUNC_RETURN;
}

/*set/get the MAC source address that will be sent in case of Pause/LLFC*/
int pm8x50_port_tx_mac_sa_set(int unit, int port, pm_info_t pm_info,
                              sal_mac_addr_t mac_sa)
{
   SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_tx_mac_sa_set(unit, port, mac_sa));

exit:
    SOC_FUNC_RETURN;
}
int pm8x50_port_tx_mac_sa_get(int unit, int port, pm_info_t pm_info,
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
int pm8x50_port_rx_mac_sa_set(int unit, int port, pm_info_t pm_info,
                              sal_mac_addr_t mac_sa)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_rx_mac_sa_set(unit, port, mac_sa));

exit:
    SOC_FUNC_RETURN;
}
int pm8x50_port_rx_mac_sa_get(int unit, int port, pm_info_t pm_info,
                              sal_mac_addr_t mac_sa)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_rx_mac_sa_get(unit, port, mac_sa));

exit:
    SOC_FUNC_RETURN;
}

/*set/get Average inter-packet gap*/
int pm8x50_port_tx_average_ipg_set(int unit, int port, pm_info_t pm_info,
                                   int value)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_tx_average_ipg_set(unit, port, value));

exit:
    SOC_FUNC_RETURN;


}
int pm8x50_port_tx_average_ipg_get(int unit, int port, pm_info_t pm_info,
                                   int* value)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_tx_average_ipg_get(unit, port, value));

exit:
    SOC_FUNC_RETURN;

}

/*local fault set/get*/
int pm8x50_port_local_fault_control_set(int unit, int port, pm_info_t pm_info,
                                 const portmod_local_fault_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_local_fault_control_set(unit, port, control));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_local_fault_control_get(int unit, int port, pm_info_t pm_info,
                                      portmod_local_fault_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_local_fault_control_get(unit, port, control));

exit:
    SOC_FUNC_RETURN;

}

/*remote fault set/get*/
int pm8x50_port_remote_fault_control_set(int unit, int port, pm_info_t pm_info,
                                 const portmod_remote_fault_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_remote_fault_control_set(unit, port, control));

exit:
    SOC_FUNC_RETURN;

}
int pm8x50_port_remote_fault_control_get(int unit, int port, pm_info_t pm_info,
                                     portmod_remote_fault_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_remote_fault_control_get(unit, port, control));

exit:
    SOC_FUNC_RETURN;
}

/*local fault steatus get*/
int pm8x50_port_local_fault_status_get(int unit, int port, pm_info_t pm_info,
                                       int* value)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_local_fault_status_get(unit, port, value));

exit:
    SOC_FUNC_RETURN;
}

/*remote fault status get*/
int pm8x50_port_remote_fault_status_get(int unit, int port, pm_info_t pm_info,
                                        int* value)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_remote_fault_status_get(unit, port, value));

exit:
    SOC_FUNC_RETURN;
}

/*local fault steatus clear, there are no clear register for CDMAC, just clear by read*/
int pm8x50_port_local_fault_status_clear(int unit, int port, pm_info_t pm_info)
{
    int value;
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_local_fault_status_get(unit, port, &value));

exit:
    SOC_FUNC_RETURN;
}

/*remote fault status clear, there are no clear register for CDMAC, just clear by read*/
int pm8x50_port_remote_fault_status_clear(int unit, int port,
                                          pm_info_t pm_info)
{
    int value;
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_remote_fault_status_get(unit, port, &value));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_pause_control_set(int unit, int port, pm_info_t pm_info,
                                  const portmod_pause_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cdmac_pause_control_set(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_pause_control_get(int unit, int port, pm_info_t pm_info,
                                  portmod_pause_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cdmac_pause_control_get(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_pfc_control_set(int unit, int port, pm_info_t pm_info,
                                const portmod_pfc_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cdmac_pfc_control_set(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_pfc_control_get(int unit, int port, pm_info_t pm_info,
                                portmod_pfc_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cdmac_pfc_control_get(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

/*Routine for MAC\PHY sync.*/
int pm8x50_port_update(int unit, int port, pm_info_t pm_info,
                       const portmod_port_update_control_t* update_control)
{

    return (SOC_E_NONE);
}

/*get port cores' phymod access*/
int pm8x50_port_core_access_get(int unit, int port, pm_info_t pm_info,
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
                   &(PM_8x50_INFO(pm_info)->int_core_access),
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
int pm8x50_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info,
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
    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index,
                                            &iphy_lane_mask));

    rv = PM8x50_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index);
    _SOC_IF_ERR_EXIT(rv);

    for( i = 0 ; i < max_phys; i++) {
        _SOC_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access[i]));
    }

    *nof_phys = 1;
    /* internal core */
    sal_memcpy (&phy_access[0], &(PM_8x50_INFO(pm_info)->int_core_access),
                sizeof(phymod_phy_access_t));
    phy_access[0].access.lane_mask = iphy_lane_mask;
    if (params->lane != -1) {
        lane_count = 0;
        phy_access[0].access.lane_mask = 0;
        for (i = 0; i < MAX_PORTS_PER_PM8X50; ++i)
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
int pm8x50_pm_phy_lane_access_get(
    int unit, int pm_id, pm_info_t pm_info,
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
    sal_memcpy (&phy_access[0], &(PM_8x50_INFO(pm_info)->int_core_access),
                sizeof(phymod_phy_access_t));

    if (params->lane != -1) {
        /*
         * If lane index is not -1, need to check if there is any port attched on
         * the given lane. If there is active port on the lane, need to check if
         * the lane is working at bypass mode.
         */
        _SOC_IF_ERR_EXIT(PM8x50_LANE2PORT_GET(unit, pm_info, params->lane, &port));
        if (port > 0) {
            _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index,
                                                    &iphy_lane_mask));
            _SOC_IF_ERR_EXIT(PM8x50_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));
            if (is_bypassed) {
                PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_SET(phy_access[0].device_op_mode);
            }
        }
        phy_access[0].access.lane_mask = (0x1 << (params->lane));
    } else {
        /*
         * If lane index is -1, assume all the lanes are accessed
         */
        phy_access[0].access.lane_mask = (0x1 << MAX_PORTS_PER_PM8X50) - 1;
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
int pm8x50_port_duplex_set(int unit, int port, pm_info_t pm_info, int enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_duplex_set(unit, port, enable));
exit:
    SOC_FUNC_RETURN
}

int pm8x50_port_duplex_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_duplex_get(unit, port, enable));
exit:
    SOC_FUNC_RETURN
}

/*Port PHY Control register read*/
int pm8x50_port_phy_reg_read(int unit, int port, pm_info_t pm_info, int lane,
                             int flags, int reg_addr, uint32* value)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    PM8X50_PHY_REG_SET(reg_addr);

    if (lane >= 0) {
        reg_addr |= PHYMOD_REG_ACC_AER_IBLK_FORCE_LANE;
        reg_addr &= ~(0x7 << PHYMOD_REG_ACCESS_FLAGS_SHIFT);
        reg_addr |= ((lane & 0x7) << PHYMOD_REG_ACCESS_FLAGS_SHIFT);
    }
    _SOC_IF_ERR_EXIT(phymod_phy_reg_read(&phy_access, reg_addr, value));


exit:
    SOC_FUNC_RETURN;
}

/*Port PHY Control register write*/
int pm8x50_port_phy_reg_write(int unit, int port, pm_info_t pm_info,
                              int lane, int flags, int reg_addr, uint32 value)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    PM8X50_PHY_REG_SET(reg_addr);

    if (lane >= 0) {
        reg_addr |= PHYMOD_REG_ACC_AER_IBLK_FORCE_LANE;
        reg_addr &= ~(0x7 << PHYMOD_REG_ACCESS_FLAGS_SHIFT);
        reg_addr |= ((lane & 0x7) << PHYMOD_REG_ACCESS_FLAGS_SHIFT);
    }
    _SOC_IF_ERR_EXIT(phymod_phy_reg_write(&phy_access, reg_addr, value));

exit:
    SOC_FUNC_RETURN;
}

/*Port Reset set\get*/
int pm8x50_port_reset_set(int unit, int port,
                          pm_info_t pm_info, int mode,
                          int opcode, int value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_phy_port_reset_set(unit, port, pm_info,
                                                mode, opcode, value));
exit:
    SOC_FUNC_RETURN
}
int pm8x50_port_reset_get(int unit, int port,
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
int pm8x50_port_drv_name_get(int unit, int port,
                             pm_info_t pm_info,
                             char* name, int len)
{
    strncpy(name, "PM8X50 Driver", len);
    return (SOC_E_NONE);
}

/*set/get port fec enable according to local/remote FEC ability*/
int pm8x50_port_fec_enable_set(int unit, int port, pm_info_t pm_info,
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
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_fec_enable_set(&phy_access, enable));

exit:
    SOC_FUNC_RETURN;
}
int pm8x50_port_fec_enable_get(int unit, int port, pm_info_t pm_info,
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
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_fec_enable_get(&phy_access, enable));

exit:
    SOC_FUNC_RETURN;
}

/*get port auto negotiation local ability*/
int pm8x50_port_ability_advert_set(int unit, int port, pm_info_t pm_info,
                                   uint32 phy_flags,
                                   portmod_port_ability_t* ability)
{

    return (SOC_E_NONE);
}
int pm8x50_port_ability_advert_get(int unit, int port, pm_info_t pm_info,
                                   uint32 phy_flags,
                                   portmod_port_ability_t* ability)
{

    return (SOC_E_NONE);
}

/*Port ability remote Adv get*/
int pm8x50_port_ability_remote_get(int unit, int port, pm_info_t pm_info,
                                   uint32 phy_flags,
                                   portmod_port_ability_t* ability)
{

    return (SOC_E_NONE);
}

/*Port Mac Control Spacing Stretch*/
int pm8x50_port_frame_spacing_stretch_set(int unit, int port,
                                          pm_info_t pm_info, int spacing)
{

    return (SOC_E_NONE);
}
int pm8x50_port_frame_spacing_stretch_get(int unit, int port,
                                          pm_info_t pm_info,
                                          const int* spacing)
{

    return (SOC_E_NONE);
}

/*get port timestamps in fifo*/
int pm8x50_port_diag_fifo_status_get(int unit, int port, pm_info_t pm_info,
                                     const portmod_fifo_status_t* diag_info)
{

    return (SOC_E_NONE);
}

/*set/get pass control frames.*/
int pm8x50_port_rx_control_set(int unit, int port, pm_info_t pm_info,
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

int pm8x50_port_rx_control_get(int unit, int port, pm_info_t pm_info,
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
int pm8x50_port_pfc_config_set(int unit, int port, pm_info_t pm_info,
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

int pm8x50_port_pfc_config_get(int unit, int port, pm_info_t pm_info,
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

/*set EEE Config.*/
int pm8x50_port_eee_set(int unit, int port, pm_info_t pm_info,
                        const portmod_eee_t* eee)
{

    return (SOC_E_UNAVAIL);
}
int pm8x50_port_eee_get(int unit, int port, pm_info_t pm_info,
                        portmod_eee_t* eee)
{

    return (SOC_E_UNAVAIL);
}

/*set EEE Config.*/
int pm8x50_port_eee_clock_set(int unit, int port, pm_info_t pm_info,
                              const portmod_eee_clock_t* eee_clk)
{

    return (SOC_E_NONE);
}
int pm8x50_port_eee_clock_get(int unit, int port, pm_info_t pm_info,
                              portmod_eee_clock_t* eee_clk)
{

    return (SOC_E_NONE);
}

/*set Vlan Inner/Outer tag.*/
int pm8x50_port_vlan_tag_set(int unit, int port, pm_info_t pm_info,
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
int pm8x50_port_vlan_tag_get(int unit, int port, pm_info_t pm_info,
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

/*set modid field.*/
int pm8x50_port_modid_set(int unit, int port, pm_info_t pm_info, int value)
{

    return (SOC_E_NONE);
}

/*set modid field.*/
int pm8x50_port_led_chain_config(int unit, int port, pm_info_t pm_info,
                                 int value)
{

    return (SOC_E_NONE);
}

/*set modid field.*/
int pm8x50_port_clear_rx_lss_status_set(int unit, int port, pm_info_t pm_info,
                                        int lcl_fault, int rmt_fault)
{

    return (SOC_E_NONE);
}
int pm8x50_port_clear_rx_lss_status_get(int unit, int port, pm_info_t pm_info,
                                        int* lcl_fault, int* rmt_fault)
{

    return (SOC_E_NONE);
}

/*Attaches an external phy lane to a specific port macro*/
int pm8x50_xphy_lane_attach_to_pm(int unit,
                         pm_info_t pm_info, int iphy, int phyn,
                         const portmod_xphy_lane_connection_t* lane_connection)
{

    return (SOC_E_NONE);
}

/*Attaches an external phy lane to a specific port macro*/
int pm8x50_xphy_lane_detach_from_pm(int unit,
                               pm_info_t pm_info, int iphy, int phyn,
                               portmod_xphy_lane_connection_t* lane_connection)
{

    return (SOC_E_NONE);
}

/*Toggle Lag Failover Status.*/
int pm8x50_port_lag_failover_status_toggle(int unit, int port,
                                           pm_info_t pm_info)
{
    int phy_acc;
    uint32_t rval;
    int link = 0;
    soc_timeout_t to;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_phy_access_get(unit, port, pm_info, &phy_acc));

    /* Link status to IPIPE is updated by H/W and driven based on both the
     * rising edge of CDPORT_LAG_FAILOVER_CONFIG.LINK_STATUS_UP and
     * actual link up status from Serdes.
     * In some loopback scenarios it may take longer time to see Serdes link up status.
     */
    soc_timeout_init(&to, 10000, 0);

    do {
        _SOC_IF_ERR_EXIT(pm8x50_port_mac_link_get(unit, port, pm_info, &link));
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
int pm8x50_port_lag_failover_loopback_set(int unit, int port,
                                          pm_info_t pm_info,
                                          int value)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_lag_failover_loopback_set(unit, port, value));
exit:
    SOC_FUNC_RETURN;
}
int pm8x50_port_lag_failover_loopback_get(int unit, int port,
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
int pm8x50_port_mode_set(int unit, int port, pm_info_t pm_info,
                         const portmod_port_mode_info_t* mode)
{

    return (SOC_E_NONE);
}

STATIC
int _cdport_mode_get(int unit, int port, pm_info_t pm_info,
                     int first_phy_index, portmod_port_mode_info_t *p_mode)
{
    uint32 reg_val = 0, port_mode = 0;
    int phy_acc, mac_id = 0, this_phy_index;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_phy_access_get(unit, port, pm_info, &phy_acc));

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

int pm8x50_port_mode_get(int unit, int port, pm_info_t pm_info,
                         portmod_port_mode_info_t* mode)
{
    int port_index;
    uint32_t bitmap;

    SOC_INIT_FUNC_DEFS;

     _SOC_IF_ERR_EXIT(_pm8x50_port_index_get (unit, port, pm_info, &port_index, &bitmap));
     _SOC_IF_ERR_EXIT(_cdport_mode_get(unit, port, pm_info, port_index, mode));

exit:
    SOC_FUNC_RETURN;
}

/*set port encap.*/
int pm8x50_port_encap_set(int unit, int port, pm_info_t pm_info,
                          int flags, portmod_encap_t encap)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_encap_set(unit, port, flags, encap));
exit:
    SOC_FUNC_RETURN;
}
int pm8x50_port_encap_get(int unit, int port, pm_info_t pm_info,
                          int* flags, portmod_encap_t* encap)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_encap_get(unit, port, flags, encap));
exit:
    SOC_FUNC_RETURN;
}

/*set port register higig field.*/
int pm8x50_port_higig_mode_set(int unit, int port,
                               pm_info_t pm_info,
                               int mode)
{

    return (SOC_E_NONE);
}
int pm8x50_port_higig_mode_get(int unit, int port,
                               pm_info_t pm_info,
                               int* mode)
{

    return (SOC_E_NONE);
}

/*set port register higig field.*/
int pm8x50_port_higig2_mode_set(int unit, int port,
                                pm_info_t pm_info,
                                int mode)
{

    return (SOC_E_NONE);
}
int pm8x50_port_higig2_mode_get(int unit, int port,
                                pm_info_t pm_info,
                                int* mode)
{

    return (SOC_E_NONE);
}

/*set port register port type field.*/
int pm8x50_port_config_port_type_set(int unit, int port,
                                     pm_info_t pm_info,
                                     int type)
{

    return (SOC_E_NONE);
}
int pm8x50_port_config_port_type_get(int unit, int port,
                                     pm_info_t pm_info,
                                     int* type)
{

    return (SOC_E_NONE);
}

/*set/get hwfailover for trident.*/
int pm8x50_port_trunk_hwfailover_config_set(int unit, int port,
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
int pm8x50_port_trunk_hwfailover_config_get(int unit, int port,
                                            pm_info_t pm_info,
                                            int* enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_lag_failover_en_get(unit, port, enable));
exit:
    SOC_FUNC_RETURN;
}

/*set/get hwfailover for trident.*/
int pm8x50_port_trunk_hwfailover_status_get(int unit, int port,
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
int pm8x50_port_diag_ctrl(int unit, int port, pm_info_t pm_info,
                          uint32 inst, int op_type, int op_cmd,
                          const void* arg)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    portmod_access_get_params_t params;
    int nof_phys = 0;

    SOC_INIT_FUNC_DEFS;
    /* Only internal phy supported. */
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                      &params, 1, phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(portmod_phy_port_diag_ctrl(unit, port, phy_access, nof_phys, inst, op_type, op_cmd, arg));

exit:
    SOC_FUNC_RETURN;

}

/*Get/Set InterFrameGap Setting. */
int pm8x50_port_ifg_set(int unit, int port, pm_info_t pm_info, int speed,
                        soc_port_duplex_t duplex, int ifg, int* real_ifg)
{

    return (SOC_E_NONE);
}
int pm8x50_port_ifg_get(int unit, int port, pm_info_t pm_info, int speed,
                        soc_port_duplex_t duplex, int* ifg)
{

    return (SOC_E_NONE);
}

/*Get the reference clock value 156 or 125.*/
int pm8x50_port_ref_clk_get(int unit, int port,
                            pm_info_t pm_info,
                            int* ref_clk)
{
    *ref_clk = pm_info->pm_data.pm8x50_db->ref_clk;

    return (SOC_E_NONE);
}

/*Disable lag failover.*/
int pm8x50_port_lag_failover_disable(int unit, int port, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_lag_failover_disable(unit, port));

exit:
    SOC_FUNC_RETURN;
}

/*Disable lag failover.*/
int pm8x50_port_lag_remove_failover_lpbk_set(int unit, int port,
                                             pm_info_t pm_info, int val)
{
   SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_lag_remove_failover_lpbk_set(unit, port, val));

exit:
    SOC_FUNC_RETURN;
}
int pm8x50_port_lag_remove_failover_lpbk_get(int unit, int port,
                                             pm_info_t pm_info, int* val)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_lag_remove_failover_lpbk_get(unit, port, val));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_cntmaxsize_set(int unit, int port, pm_info_t pm_info, int val)
{
    return (cdmac_cntmaxsize_set(unit, port, val));
}

int pm8x50_port_cntmaxsize_get(int unit, int port, pm_info_t pm_info, int* val)
{
    return (cdmac_cntmaxsize_get(unit, port, val));
}

/*Get Info needed to restore after drain cells.*/
int pm8x50_port_drain_cell_get(int unit, int port, pm_info_t pm_info,
                               portmod_drain_cells_t* drain_cells)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_drain_cell_get(unit, port, drain_cells));

exit:
    SOC_FUNC_RETURN;
}

/*Restore informaation after drain cells.*/
int pm8x50_port_drain_cell_stop(int unit, int port, pm_info_t pm_info,
                                const portmod_drain_cells_t* drain_cells)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_drain_cell_stop(unit, port, drain_cells));

exit:
    SOC_FUNC_RETURN;
}

/*Restore informaation after drain cells.*/
int pm8x50_port_drain_cell_start(int unit, int port, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_drain_cell_start(unit, port));

exit:
    SOC_FUNC_RETURN;
}

/**/
int pm8x50_port_drain_cells_rx_enable(int unit, int port,
                                      pm_info_t pm_info,
                                      int rx_en)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_drain_cells_rx_enable(unit, port, rx_en));

exit:
    SOC_FUNC_RETURN;
}

/**/
int pm8x50_port_egress_queue_drain_rx_en(int unit, int port,
                                         pm_info_t pm_info,
                                         int rx_en)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_egress_queue_drain_rx_en(unit, port, rx_en));

exit:
    SOC_FUNC_RETURN;
}

/**/
int pm8x50_port_mac_ctrl_set(int unit, int port,
                             pm_info_t pm_info,
                             uint64 ctrl)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_mac_ctrl_set(unit, port, ctrl));

exit:
    SOC_FUNC_RETURN;
}

/**/
int pm8x50_port_txfifo_cell_cnt_get(int unit, int port,
                                    pm_info_t pm_info,
                                    uint32* cnt)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_txfifo_cell_cnt_get(unit, port, cnt));

exit:
    SOC_FUNC_RETURN;
}

/**/
int pm8x50_port_egress_queue_drain_get(int unit, int port,
                                       pm_info_t pm_info,
                                       uint64* ctrl, int* rxen)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_egress_queue_drain_get(unit, port, ctrl, rxen));

exit:
    SOC_FUNC_RETURN;
}

/**/
int pm8x50_port_mac_reset_set(int unit, int port, pm_info_t pm_info, int val)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_soft_reset_set(unit, port, val));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_mac_reset_get(int unit, int port, pm_info_t pm_info, int* val)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_soft_reset_get(unit, port, val));

    exit:
        SOC_FUNC_RETURN;
}

/**/
int pm8x50_port_soft_reset_toggle(int unit, int port, pm_info_t pm_info, int idx)
{

    return (SOC_E_NONE);
}

/*Check if MAC needs to be reset.*/
int pm8x50_port_mac_reset_check(int unit, int port,
                                pm_info_t pm_info,
                                int enable, int* reset)
{
    return (cdmac_reset_check(unit, port, enable, reset));
}

/**/
int pm8x50_port_core_num_get(int unit, int port,
                             pm_info_t pm_info,
                             int* core_num)
{
    *core_num = PM_8x50_INFO(pm_info)->core_num;
    return (SOC_E_NONE);
}

/**/
int pm8x50_port_e2ecc_hdr_set(int unit, int port, pm_info_t pm_info,
                              const portmod_port_higig_e2ecc_hdr_t* e2ecc_hdr)
{

    return (SOC_E_NONE);
}
int pm8x50_port_e2ecc_hdr_get(int unit, int port, pm_info_t pm_info,
                              portmod_port_higig_e2ecc_hdr_t* e2ecc_hdr)
{

    return (SOC_E_NONE);
}

/**/
int pm8x50_port_e2e_enable_set(int unit, int port,
                               pm_info_t pm_info,
                               int enable)
{

    return (SOC_E_NONE);
}
int pm8x50_port_e2e_enable_get(int unit, int port,
                               pm_info_t pm_info,
                               int* enable)
{

    return (SOC_E_NONE);
}

/*get the speed for the specified port*/
int pm8x50_port_speed_get(int unit, int port, pm_info_t pm_info, int* speed)
{
    phymod_phy_speed_config_t speed_config;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    *speed = 0;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                   &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_t_init(&speed_config));
    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_get(&phy_access, &speed_config));

    *speed = speed_config.data_rate;
exit:
    SOC_FUNC_RETURN;
}

/*Port discard set*/
int pm8x50_port_discard_set(int unit, int port, pm_info_t pm_info, int discard)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_discard_set(unit, port, discard));

exit:
    SOC_FUNC_RETURN;
}

/*Port soft reset set set*/
int pm8x50_port_soft_reset_set(int unit, int port, pm_info_t pm_info,
                               int idx, int val, int flags)
{

    return (SOC_E_NONE);
}

/*Port tx_en=0 and softreset mac*/
int pm8x50_port_tx_down(int unit, int port, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_tx_enable_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(cdmac_discard_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(cdmac_soft_reset_set(unit, port, 1));

exit:
    SOC_FUNC_RETURN;
}

/*reconfig pgw.*/
int pm8x50_port_pgw_reconfig(int unit, int port, pm_info_t pm_info,
                             const portmod_port_mode_info_t* pmode,
                             int phy_port, int flags)
{

    return (SOC_E_NONE);
}

/*Routine to notify internal phy of external phy link state.*/
int pm8x50_port_notify(int unit, int port, pm_info_t pm_info, int link)
{

    return (SOC_E_NONE);
}

/*port control phy timesync config set/get*/
int pm8x50_port_control_phy_timesync_set(int unit,
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
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
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

int pm8x50_port_control_phy_timesync_get(int unit,
                                         int port, pm_info_t pm_info,
                                         portmod_port_control_phy_timesync_t config,
                                         uint64* value)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(portmod_common_control_phy_timesync_get(&phy_access, config, value));

exit:
    SOC_FUNC_RETURN;
}

/*"port timesync config set/get"*/
int pm8x50_port_timesync_config_set(int unit, int port, pm_info_t pm_info,
                              const portmod_phy_timesync_config_t* config)
{
    int port_index, ts_enable_port_count;
    uint32 bitmap, one_step_req, is_one_step = 0, flags = 0;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, enable, is_enable;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    enable = (config->flags & SOC_PORT_PHY_TIMESYNC_ENABLE) ? 1 : 0;

    _SOC_IF_ERR_EXIT(PM8x50_TIMESYNC_ENABLE_GET(unit, pm_info, &is_enable, port_index));
    _SOC_IF_ERR_EXIT(PM8x50_TS_ENABLE_PORT_COUNT_GET(unit, pm_info, &ts_enable_port_count));

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
        /* Enable 1588 */
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

        /* Rx enable can only be done on link up as per programming requirement */
        _SOC_IF_ERR_EXIT(phymod_timesync_enable_set(&phy_access, flags, 1));

    }

    /* Update Timesync Enable Status in WB */
    _SOC_IF_ERR_EXIT(PM8x50_TS_ENABLE_PORT_COUNT_SET(unit, pm_info, ts_enable_port_count));
    _SOC_IF_ERR_EXIT(PM8x50_TIMESYNC_ENABLE_SET(unit, pm_info, enable, port_index));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_timesync_config_get(int unit, int port, pm_info_t pm_info,
                                    portmod_phy_timesync_config_t* config)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, timesync_enable;
    int port_index;
    uint32 bitmap, is_one_step, flags = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(PM8x50_TIMESYNC_ENABLE_GET(unit, pm_info, &timesync_enable, port_index));
    PHYMOD_TIMESYNC_ENABLE_F_ONE_STEP_PIPELINE_SET(flags);
    _SOC_IF_ERR_EXIT(phymod_timesync_enable_get(&phy_access, flags, &is_one_step));
    config->flags |= is_one_step? SOC_PORT_PHY_TIMESYNC_ONE_STEP_ENABLE : 0;
    config->flags |= timesync_enable ? SOC_PORT_PHY_TIMESYNC_ENABLE : 0;

exit:
    SOC_FUNC_RETURN;
}

/*"port timesync enable set/get"*/
int pm8x50_port_timesync_enable_set(int unit, int port,
                                    pm_info_t pm_info,
                                    uint32 enable)
{

    return (SOC_E_NONE);
}
int pm8x50_port_timesync_enable_get(int unit, int port,
                                    pm_info_t pm_info,
                                     uint32* enable)
{

    return (SOC_E_NONE);
}

/*"port timesync nco addend  set/get"*/
int pm8x50_port_timesync_nco_addend_set(int unit, int port,
                                        pm_info_t pm_info,
                                        uint32 freq_step)
{

    return (SOC_E_NONE);
}
int pm8x50_port_timesync_nco_addend_get(int unit, int port,
                                        pm_info_t pm_info,
                                        uint32* freq_step)
{

    return (SOC_E_NONE);
}

/*"port timesync framesync info  set/get"*/
int pm8x50_port_timesync_framesync_mode_set(int unit,
                                int port, pm_info_t pm_info,
                                const portmod_timesync_framesync_t* framesync)
{

    return (SOC_E_NONE);
}
int pm8x50_port_timesync_framesync_mode_get(int unit,
                                     int port, pm_info_t pm_info,
                                     portmod_timesync_framesync_t* framesync)
{

    return (SOC_E_NONE);
}

/*"port timesync local time  set/get"*/
int pm8x50_port_timesync_local_time_set(int unit, int port,
                                        pm_info_t pm_info,
                                        uint64 local_time)
{

    return (SOC_E_NONE);
}
int pm8x50_port_timesync_local_time_get(int unit, int port,
                                        pm_info_t pm_info,
                                        uint64* local_time)
{

    return (SOC_E_NONE);
}

/*"port timesync framesync info  set/get"*/
int pm8x50_port_timesync_load_ctrl_set(int unit, int port, pm_info_t pm_info,
                                       uint32 load_once, uint32 load_always)
{

    return (SOC_E_NONE);
}
int pm8x50_port_timesync_load_ctrl_get(int unit, int port, pm_info_t pm_info,
                                       uint32* load_once, uint32* load_always)
{

    return (SOC_E_NONE);
}

/*"port timesync tx timestamp offset set/get"*/
int pm8x50_port_timesync_tx_timestamp_offset_set(int unit, int port,
                                                 pm_info_t pm_info,
                                                 uint32 ts_offset)
{

    return (SOC_E_NONE);
}
int pm8x50_port_timesync_tx_timestamp_offset_get(int unit, int port,
                                                 pm_info_t pm_info,
                                                 uint32* ts_offset)
{

    return (SOC_E_NONE);
}

/*"port timesync rx timestamp offset set/get"*/
int pm8x50_port_timesync_rx_timestamp_offset_set(int unit, int port,
                                                 pm_info_t pm_info,
                                                 uint32 ts_offset)
{

    return (SOC_E_NONE);
}
int pm8x50_port_timesync_rx_timestamp_offset_get(int unit, int port,
                                                 pm_info_t pm_info,
                                                 uint32* ts_offset)
{

    return (SOC_E_NONE);
}

/*set/get interrupt enable value. */
int pm8x50_port_interrupt_enable_set(int unit, int port,
                                     pm_info_t pm_info,
                                     int intr_type, uint32 val)
{
    uint32 reg_val;
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_phy_access_get(unit, port, pm_info, &phy_acc));
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
int _pm8x50_phy_timesync_tx_info_get(int unit, int port, pm_info_t pm_info, portmod_fifo_status_t* tx_info)
{
    phymod_phy_access_t phy_access;
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_ts_fifo_status_t phy_ts_tx_info;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(phymod_ts_fifo_status_t_init(&phy_ts_tx_info));
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
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
 * pm8x50_port_timesync_tx_info_get
 *
 * @brief get port timestamps in fifo
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [inout]  tx_info         - timestamp and seq id form fifo
 */

int pm8x50_port_timesync_tx_info_get(int unit, int port, pm_info_t pm_info, portmod_fifo_status_t* tx_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_phy_timesync_tx_info_get(unit, port, pm_info, tx_info));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_interrupt_enable_get(int unit, int port,
                                     pm_info_t pm_info,
                                     int intr_type, uint32* val)
{
    uint32 reg_val;
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_phy_access_get(unit, port, pm_info, &phy_acc));
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
int pm8x50_port_interrupt_get(int unit, int port, pm_info_t pm_info, int intr_type, uint32* val)
{
    uint32 reg_val;
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_phy_access_get(unit, port, pm_info, &phy_acc));
    _SOC_IF_ERR_EXIT(READ_CDPORT_INTR_STATUSr(unit, phy_acc, &reg_val));

    switch(intr_type) {
        case portmodIntrTypeLinkdown:
            *val = soc_reg_field_get(unit, CDPORT_INTR_STATUSr, reg_val, LINK_DOWNf);
            break;
        case portmodIntrTypeMacErr:
            *val = soc_reg_field_get(unit, CDPORT_INTR_STATUSr, reg_val, MAC_ERRf);
            break;
        case portmodIntrTypeTscCore0Err:
            *val = soc_reg_field_get(unit, CDPORT_INTR_STATUSr, reg_val, TSC_CORE0_ERRf);
            break;
        case portmodIntrTypeTscCore1Err:
            *val = soc_reg_field_get(unit, CDPORT_INTR_STATUSr, reg_val, TSC_CORE1_ERRf);
            break;
        case portmodIntrTypePmdErr:
            *val = soc_reg_field_get(unit, CDPORT_INTR_STATUSr, reg_val, PMD_ERRf);
            break;
        case portmodIntrTypeFcReqFull:
            *val = soc_reg_field_get(unit, CDPORT_INTR_STATUSr, reg_val, FLOWCONTROL_REQ_FULLf);
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
int pm8x50_port_interrupts_get(int unit, int port, pm_info_t pm_info,
                               int arr_max_size, uint32* intr_arr,
                               uint32* size)
{
    uint32 reg_val, cnt=0;
    int phy_acc;
    uint32 mac_intr_cnt = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_phy_access_get(unit, port, pm_info, &phy_acc));
    _SOC_IF_ERR_EXIT(READ_CDPORT_INTR_STATUSr(unit, phy_acc, &reg_val));

    if (soc_reg_field_get(unit, CDPORT_INTR_STATUSr, reg_val, LINK_DOWNf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeLinkdown;
    }

    if (soc_reg_field_get(unit, CDPORT_INTR_STATUSr, reg_val, MAC_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeMacErr;
    }

    if (soc_reg_field_get(unit, CDPORT_INTR_STATUSr, reg_val, TSC_CORE0_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTscCore0Err;
    }

    if (soc_reg_field_get(unit, CDPORT_INTR_STATUSr, reg_val, TSC_CORE1_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTscCore1Err;
    }

    if (soc_reg_field_get(unit, CDPORT_INTR_STATUSr, reg_val, PMD_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypePmdErr;
    }

    if (soc_reg_field_get(unit, CDPORT_INTR_STATUSr, reg_val, FLOWCONTROL_REQ_FULLf)) {
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

/* portmod check if external phy is legacy*/
int pm8x50_port_check_legacy_phy(int unit, int port,
                                 pm_info_t pm_info,
                                 int* legacy_phy)
{

    return (SOC_E_NONE);
}

/* portmod phy failover mode*/
int pm8x50_port_failover_mode_set(int unit, int port,
                                  pm_info_t pm_info,
                                  phymod_failover_mode_t failover)
{

    return (SOC_E_NONE);
}
int pm8x50_port_failover_mode_get(int unit, int port,
                                  pm_info_t pm_info,
                                  phymod_failover_mode_t* failover)
{

    return (SOC_E_NONE);
}

/* portmod port rsv mask set*/
int pm8x50_port_mac_rsv_mask_set(int unit, int port,
                                 pm_info_t pm_info,
                                 uint32 rsv_mask)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_rsv_mask_set(unit, port, rsv_mask));

exit:
    SOC_FUNC_RETURN;
}

/* portmod restore information after warmboot*/
int pm8x50_port_warmboot_db_restore(int unit, int port, pm_info_t pm_info,
                        const portmod_port_interface_config_t* intf_config,
                        const portmod_port_init_config_t* init_config,
                        phymod_operation_mode_t phy_op_mode)
{

    return (SOC_E_NONE);
}

/* portmod port flow control config*/
int pm8x50_port_flow_control_set(int unit, int port,
                                 pm_info_t pm_info,
                                 int merge_mode_en,
                                 int parallel_fc_en)
{

    return (SOC_E_NONE);
}

/*Portmod state for any logical port dynamixc settings*/
int pm8x50_port_update_dynamic_state(int unit, int port,
                                     pm_info_t pm_info,
                                     uint32_t port_dynamic_state)
{

    return (SOC_E_NONE);
}

/*get phy operation mode. */
int pm8x50_port_phy_op_mode_get(int unit, int port, pm_info_t pm_info,
                                phymod_operation_mode_t* val)
{

    return (SOC_E_NONE);
}

/*Returns if the PortMacro associated with the port is initialized or not*/
int pm8x50_pm_is_initialized(int unit, int pm_id, pm_info_t pm_info, int* is_initialized)
{
    int is_core_initialized = 0, rv;
     rv = PM8x50_IS_CORE_INITIALIZED_GET(unit, pm_info, &is_core_initialized);
     *is_initialized = is_core_initialized;

    return rv;
}

/* get the logical port bitmap of the current PM */
int pm8x50_pm_logical_pbmp_get(int unit, int pm_id, pm_info_t pm_info,
                               portmod_pbmp_t* logical_pbmp)
{
    int i, port;
    SOC_INIT_FUNC_DEFS;

    PORTMOD_PBMP_CLEAR(*logical_pbmp);
    for (i = 0 ; i < MAX_PORTS_PER_PM8X50; i++) {
        _SOC_IF_ERR_EXIT(PM8x50_LANE2PORT_GET(unit, pm_info, i, &port));
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

int pm8x50_core_add(int unit, int pm_id, pm_info_t pm_info, int flags, const portmod_port_add_info_t* add_info)
{
    int is_initialized = 0;
    int pm_enable_flags = 0;
    int force_mac_init = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pm8x50_pm_is_initialized(unit, pm_id, pm_info, &is_initialized));

    if (PORTMOD_CORE_ADD_SKIP_MAC_GET(flags)) {
        PORTMOD_PM_ENABLE_SKIP_MAC_SET(pm_enable_flags);
    }
    force_mac_init = PORTMOD_CORE_ADD_FORCE_MAC_GET(flags);
    if (is_initialized && force_mac_init) {
        PORTMOD_PM_ENABLE_MAC_ONLY_SET(pm_enable_flags);
    }
    if (!is_initialized || force_mac_init) {
        _SOC_IF_ERR_EXIT(pm8x50_pm_enable(unit, pm_id, pm_info, pm_enable_flags, 1));
    }

    _SOC_IF_ERR_EXIT(pm8x50_pm_serdes_core_init(unit, pm_id, pm_info, add_info));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_lane_map_set(int unit, int port, pm_info_t pm_info, uint32 flags, const phymod_lane_map_t* lane_map)
{

    return (SOC_E_UNAVAIL);
}

int pm8x50_port_lane_map_get(int unit, int port, pm_info_t pm_info, uint32 flags, phymod_lane_map_t* lane_map)
{
    SOC_INIT_FUNC_DEFS;

    *lane_map = PM_8x50_INFO(pm_info)->lane_map;

    SOC_FUNC_RETURN;
}

int pm8x50_port_synce_clk_ctrl_set(int unit, int port, pm_info_t pm_info,
                                   const portmod_port_synce_clk_ctrl_t* cfg)
{
    phymod_synce_clk_ctrl_t phy_synce_cfg;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    int config_valid = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
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

int pm8x50_port_synce_clk_ctrl_get(int unit, int port, pm_info_t pm_info,
                                   portmod_port_synce_clk_ctrl_t* cfg)
{
    phymod_synce_clk_ctrl_t phy_synce_cfg;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
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
int _pm8x50_pm_tsc_interrupt_type_get(int unit, int intr_val, uint32 flags,
                              pm8x50_tsc_err_intr_phymod_map_t *tsc_intr_info)
{
    pm8x50_tsc_intr_type_t iter = pm8x50TscIntrEccNone;
    pm8x50_tsc_intr_type_t iter_start = pm8x50TscIntrEccNone;
    pm8x50_tsc_intr_type_t iter_end = pm8x50TscIntrEccNone;

    /* Check is 1-bit or 2-bit ECC interrupt type */
    if (flags & PM8x50_TSC_ECC_1B_INTR) {
        iter_start = pm8x50TscIntrEcc1bErr;
        iter_end = pm8x50TscIntrEcc1bErrCount;
    } else if (flags & PM8x50_TSC_ECC_2B_INTR) {
        iter_start = pm8x50TscIntrEcc2bErr;
        iter_end = pm8x50TscIntrEcc2bErrCount;
    } else {
        return SOC_E_INTERNAL;
    }

    for(iter = iter_start; iter <= iter_end; iter++) {
        if (intr_val & pm8x50_tsc_ecc_intr_info[iter].flags) {
            /* return phymod tsc interrupt type */
            sal_memcpy(tsc_intr_info, &pm8x50_tsc_ecc_intr_info[iter],
                        sizeof(pm8x50_tsc_err_intr_phymod_map_t));
            break;
        }
    }

    return SOC_E_NONE;
}

int pm8x50_pm_interrupt_process(int unit, int pm_id, pm_info_t pm_info,
                                portmod_ecc_intr_info_t *ecc_info)
{
    int i = 0, ii = 0, j = 0;
    int lport = 0;
    int prev_lport = 0;
    int first_lport = 0;
    uint32 val = 0, rval = 0, fval = 0;
    uint32 tsc_intr_flags = 0x0, is_handled = 0;
    uint64 rval64;
    pm8x50_tsc_err_intr_phymod_map_t tsc_intr_info;
    portmod_pm_intr_info_t *p_intr_info = &pm8x50_pm_intr_info[0];
    portmod_intr_reg_info_t *p_intr_grp_info = NULL;
    portmod_intr_type_t intr_type;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys = 0;
    int is_intr_found = 0;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(rval64);
    sal_memset(&tsc_intr_info,0,sizeof(tsc_intr_info));

    /* Get the first physical port of the pm core */
    for(ii=0; ii < PM8X50_LANES_PER_CORE; ii++) {
        /* get the logical port associated with the physical port */
        _SOC_IF_ERR_EXIT(PM8x50_LANE2PORT_GET(unit, pm_info, ii, &lport));

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
            for (i = 0; i < sizeof(pm8x50_pm_intr_info)/
                            sizeof(portmod_pm_intr_info_t); i++) {
                p_intr_info = &pm8x50_pm_intr_info[i];
                intr_type = p_intr_info->intr_type;
                _SOC_IF_ERR_EXIT(pm8x50_port_interrupt_get(unit, lport,
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

                        PORTMOD_PBMP_ITER(PM_8x50_INFO(pm_info)->phys,
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
                        tsc_intr_flags = PM8x50_TSC_ECC_1B_INTR;
                    } else if (p_intr_grp_info->st_fld == TSC_ECC_2B_ERRf) {
                        tsc_intr_flags = PM8x50_TSC_ECC_2B_INTR;
                    }

                    _SOC_IF_ERR_EXIT(_pm8x50_pm_tsc_interrupt_type_get(unit,
                                                fval, tsc_intr_flags,
                                                &tsc_intr_info));
                    /* Only internal phy supported. */
                    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit,
                                                                     &params));
                    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit,
                                                lport, pm_info, &params, 1,
                                                &phy_access, &nof_phys, NULL));

                    LOG_VERBOSE(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit, "TSC Core Error - %d\n"), 
                                   tsc_intr_info.phymod_intr_map));
                    /* clear tsc interrupt */
                    _SOC_IF_ERR_EXIT(phymod_intr_handler(&phy_access,
                                             tsc_intr_info.phymod_intr_map,
                                             &is_handled));
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

int pm8x50_pm_core_num_get(int unit, pm_info_t pm_info, int* core_num)
{
    *core_num = PM_8x50_INFO(pm_info)->core_num;
    return (SOC_E_NONE);
}

int _portmod_rlm_config_validate(int unit, int port, int speed, int port_index,  uint32 active_lane_map)
{
    int i, num_lane = 8, active_lane_num = 0;
    SOC_INIT_FUNC_DEFS;

    for (i = 0; i < num_lane; i++) {
        if (active_lane_map &  1 << (port_index + i)) {
            active_lane_num++;
        }
    }

    /* first check 200G case */
    switch (speed) {
    case 150000:
        if ((active_lane_map != 0xf0) && (active_lane_map != 0xf)) {
            _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                      (_SOC_MSG("Active lane map is not valid")));
        }
        break;
    case 200000:
        if (active_lane_num != 3) {
            _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                      (_SOC_MSG("Active lane map is not valid")));
        } else if ((port_index == 0) && (active_lane_map  & 0xf0)) {
            _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                      (_SOC_MSG("Active lane map is not valid")));
        } else if ((port_index == 4) && (active_lane_map  & 0x0f)) {
            _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                      (_SOC_MSG("Active lane map is not valid")));
        }
        break;
    case 300000:
    case 350000:
        if (active_lane_map != 0xff) {
            _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                      (_SOC_MSG("Active lane map is not valid")));
        }
        break;
    case 400000:
        if ((active_lane_num != 6) && (active_lane_num != 7)) {
            _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                      (_SOC_MSG("Active lane map is not valid")));
        }
        break;
    default:
        break;
    }
exit:
    SOC_FUNC_RETURN;

}

int pm8x50_port_rlm_config_set(int unit, int port, pm_info_t pm_info,
                               int enable,
                               const portmod_port_rlm_config_t* rlm_config)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    uint32_t  is_initiator, pcs_is_reconfigured, active_lane_map_local;
    phymod_fec_type_t FecType;
    uint32_t txLaneSwap, rxLaneSwap, bitmap, port_lane_mask;
    portmod_port_rlm_state_t local_port_rlm_state;
    int nof_phys,  port_index = -1, rlm_enabled;
    phymod_phy_speed_config_t phy_speed_config;
    int portmon_disable;


    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, &port_lane_mask));

    _SOC_IF_ERR_EXIT
        (portmod_portmon_disable_get(unit, &portmon_disable));
    if (portmon_disable) {
        _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                           (_SOC_MSG("RLM mode is not supported without portmod thread")));
    }

    /* first get the current port speed */
    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_t_init(&phy_speed_config));
    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_get(&phy_access, &phy_speed_config));

    /* only 150G/300G/350G/200G/400G can enable RLM mode */
    if ((phy_speed_config.data_rate == 150000) ||
        ((phy_speed_config.data_rate == 200000) && (phy_speed_config.fec_type != phymod_fec_None)) ||
        (phy_speed_config.data_rate == 300000) ||
        (phy_speed_config.data_rate == 350000) ||
        (phy_speed_config.data_rate == 400000)) {
        /* first get all the RLM warmboot variable */
        _SOC_IF_ERR_EXIT
            (PM8x50_RLM_ENABLE_GET(unit, pm_info, &rlm_enabled, port_index));

        _SOC_IF_ERR_EXIT
            (PM8x50_INITIATOR_GET(unit, pm_info, &is_initiator, port_index));

        _SOC_IF_ERR_EXIT
            (PM8x50_PCS_RECONFIG_GET(unit, pm_info, &pcs_is_reconfigured, port_index));

        _SOC_IF_ERR_EXIT
            (PM8x50_ACTIVE_LANE_MAP_GET(unit, pm_info, &active_lane_map_local, port_index));

        _SOC_IF_ERR_EXIT
            (PM8x50_RLM_STATE_GET(unit, pm_info, &local_port_rlm_state.rlm_state, port_index));

        _SOC_IF_ERR_EXIT
            (PM8x50_ORIGINAL_FEC_GET(unit, pm_info, &FecType, port_index));

        _SOC_IF_ERR_EXIT
            (PM8x50_ORIG_TX_LANE_MAP_GET(unit, pm_info, &txLaneSwap));

        _SOC_IF_ERR_EXIT
            (PM8x50_ORIG_RX_LANE_MAP_GET(unit, pm_info, &rxLaneSwap));
    } else {
        /* RLM is not supported */
        _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,
                  (_SOC_MSG("RLM mode is not supported on this speed")));
    }

    /* set the rlm_enabled */
    rlm_enabled = enable;

    /* if to enable RLM */
    if (enable) {
        is_initiator = rlm_config->is_initiator;
        /* set the RLM variable and state accordingly */
        pcs_is_reconfigured = FALSE;
        /* next check if initiator is set */
        /*if so need to get the active_lane_map */
        if (is_initiator) {
            _SOC_IF_ERR_EXIT(_portmod_rlm_config_validate(unit, port,
                                                          phy_speed_config.data_rate,
                                                          port_index,
                                                          rlm_config->active_lane_bit_map));
            active_lane_map_local = rlm_config->active_lane_bit_map;
        } else {
            active_lane_map_local = 0x0;
        }
        local_port_rlm_state.rlm_state = PORTMOD_PORT_RLM_SEND_PACKET_DATA;
    } else {
        uint32_t is_400g_port = 0, tmp_active_lane_map = 0x0f;
        portmod_speed_config_t  local_speed_config;

        /* is RLM disable, then clear both initiator and active lane map */
        is_initiator = FALSE;
        active_lane_map_local = 0x0;
        /* need to know if 4 lane or 8 lane port and also start lane of this port */
        _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, &bitmap));
        /*next based on the port index, need to get mpp index */
        if (port_index == 4) {
            tmp_active_lane_map = 0xf0;
        } else if (bitmap == 0xff) {
            is_400g_port = 1;
            tmp_active_lane_map = 0xff;
        }
        /* next need to restore the tx/rx original lane swap */
        _SOC_IF_ERR_EXIT(phymod_phy_pcs_lane_swap_adjust(&phy_access,
                                                         tmp_active_lane_map,
                                                         txLaneSwap,
                                                         rxLaneSwap));

        /*then disable PRBS tx/rx  */
        _SOC_IF_ERR_EXIT(phymod_phy_prbs_enable_set(&phy_access, 0, 0));
        /* next disable all the PMD override */
        _SOC_IF_ERR_EXIT(phymod_phy_pmd_override_enable_set(&phy_access, phymodPMDRxLock, 0, 0));
        _SOC_IF_ERR_EXIT(phymod_phy_pmd_override_enable_set(&phy_access, phymodPMDSignalDetect, 0, 0));
        _SOC_IF_ERR_EXIT(phymod_phy_pmd_override_enable_set(&phy_access, phymodPMDTxDisable, 0, 0));
        _SOC_IF_ERR_EXIT(phymod_phy_pmd_override_enable_set(&phy_access, phymodPMDLaneReset, 0, 0));

        /* next disable LI  */
        _SOC_IF_ERR_EXIT(cdmac_link_interrupt_ordered_set_enable(unit, port, 0));

        /* next read the current PMD lane config info */
        _SOC_IF_ERR_EXIT(pm8x50_port_speed_config_get(unit, port, pm_info, &local_speed_config));
        if (is_400g_port) {
            local_speed_config.num_lane = 8;
            local_speed_config.speed = 400000;
        } else {
            local_speed_config.num_lane = 4;
            local_speed_config.speed = 200000;
        }

        /* based on the saved PHYMOD  FEctype, get the portmod enum */
        _SOC_IF_ERR_EXIT(_pm8x50_port_phy_to_port_fec(FecType, &local_speed_config.fec));

        /* then do a forced speed config to restore original speed/fec mode */
        _SOC_IF_ERR_EXIT(pm8x50_port_speed_config_set(unit, port, pm_info, &local_speed_config));

        pcs_is_reconfigured = FALSE;
        active_lane_map_local = 0x0;
        local_port_rlm_state.rlm_state = PORTMOD_PORT_RLM_FSM_COUNT;
    }

    /* finally restore all the Warm boot variables */
    _SOC_IF_ERR_EXIT
        (PM8x50_RLM_ENABLE_SET(unit, pm_info, rlm_enabled, port_index));

    _SOC_IF_ERR_EXIT
        (PM8x50_INITIATOR_SET(unit, pm_info, is_initiator, port_index));

    _SOC_IF_ERR_EXIT
        (PM8x50_PCS_RECONFIG_SET(unit, pm_info, pcs_is_reconfigured, port_index));

    _SOC_IF_ERR_EXIT
        (PM8x50_ACTIVE_LANE_MAP_SET(unit, pm_info, active_lane_map_local, port_index));

    _SOC_IF_ERR_EXIT
        (PM8x50_RLM_STATE_SET(unit, pm_info, local_port_rlm_state.rlm_state, port_index));

    /* call back register should happen after RLM ENABLE is set */
    if (enable) {
        _SOC_IF_ERR_EXIT(portmod_port_periodic_callback_register(unit, port, pm8x50_periodic_callback, PORTMOD_THREAD_INTERVAL_1s));
    }


exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_rlm_config_get(int unit, int port, pm_info_t pm_info,
                               int* enable,
                               portmod_port_rlm_config_t* rlm_config)
{
    uint32_t  is_initiator, active_lane_map_local, port_lane_mask;
    int rlm_enabled, port_index;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, &port_lane_mask));

    /* first get all the RLM warmboot variable */
    _SOC_IF_ERR_EXIT
        (PM8x50_RLM_ENABLE_GET(unit, pm_info, &rlm_enabled, port_index));

    _SOC_IF_ERR_EXIT
        (PM8x50_INITIATOR_GET(unit, pm_info, &is_initiator, port_index));

    _SOC_IF_ERR_EXIT
        (PM8x50_ACTIVE_LANE_MAP_GET(unit, pm_info, &active_lane_map_local, port_index));

    *enable = rlm_enabled;
    rlm_config->is_initiator = is_initiator;
    rlm_config->active_lane_bit_map = active_lane_map_local;

exit:
    SOC_FUNC_RETURN;
}


int pm8x50_port_rlm_status_get(int unit, int port, pm_info_t pm_info,
                               portmod_port_rlm_status_t *rlm_status)
{
    uint32_t rlm_state, port_lane_mask;
    int rlm_enabled, port_index;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, &port_lane_mask));

    _SOC_IF_ERR_EXIT
        (PM8x50_RLM_ENABLE_GET(unit, pm_info, &rlm_enabled, port_index));
    _SOC_IF_ERR_EXIT
        (PM8x50_RLM_STATE_GET(unit, pm_info, &rlm_state, port_index));

    /* first initialize the status */
    rlm_status->rlm_disabled = !rlm_enabled;
    rlm_status->rlm_busy = FALSE;
    rlm_status->rlm_failed  = FALSE;
    rlm_status->rlm_done = FALSE;


    /* first check rlm_state */
    if (rlm_state == PORTMOD_PORT_RLM_ERROR) {
        rlm_status->rlm_failed  = TRUE;
    } else if (rlm_state == PORTMOD_PORT_RLM_DONE) {
        rlm_status->rlm_done = TRUE;
    } else if ((rlm_state < PORTMOD_PORT_RLM_FSM_COUNT) && (rlm_enabled)) {
        rlm_status->rlm_busy = TRUE;
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_mac_enable_set(int unit,int port, pm_info_t pm_info,
                               int enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_enable_set(unit, port, 0, enable));
exit:
    SOC_FUNC_RETURN;
} 

/*
 This function is intended for reconfiguring PCS lane mapping dynamically.
 It should be called for Q2A A0 only.
*/
int pm8x50_port_pcs_lane_map_reconfig(int unit,
                                      int port,
                                      pm_info_t pm_info,
                                      uint32 tx_lane_swap,
                                      uint32 rx_lane_swap)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    phymod_lane_map_t lane_map;
    uint32 idx;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));

    lane_map.num_of_lanes = 8;
    for (idx=0; idx < 8; idx++) {
        lane_map.lane_map_tx[idx] = (tx_lane_swap >> (idx * 4 /*4 bit per lane*/)) & 0xf;
        lane_map.lane_map_rx[idx] = (rx_lane_swap >> (idx * 4 /*4 bit per lane*/)) & 0xf;
    }

    _SOC_IF_ERR_EXIT(phymod_phy_pcs_lane_map_set(&phy_access, &lane_map));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_pmd_lock_counter_get(int unit, int port, pm_info_t pm_info, uint32* pmd_lock_counter_sum)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    /* get phy access */
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                    &params, 1, &phy_access, &nof_phys, NULL));
    /* get pmd_lock counter */
    _SOC_IF_ERR_EXIT(phymod_phy_rx_pmd_lock_counter_get(&phy_access, pmd_lock_counter_sum));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_egr_1588_timestamp_config_set(int unit, int port, pm_info_t pm_info,
                          portmod_egr_1588_timestamp_config_t timestamp_config)
{
    /* PM8x50 only support 48bit mode. */
    if (timestamp_config.timestamp_mode == portmodTimestampMode48bit) {
        return SOC_E_NONE;
    } else {
        return SOC_E_UNAVAIL;
    }
}

int pm8x50_egr_1588_timestamp_config_get(int unit, int port, pm_info_t pm_info,
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
int pm8x50_port_local_fault_reasons_get(int unit, int port, pm_info_t pm_info,
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
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                &params, 1, &phy_access, &nof_phys, NULL));


    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index, bitmap));
    _SOC_IF_ERR_EXIT(PM8x50_PORT_IS_PCS_BYPASSED_GET(unit, pm_info, &is_bypassed, port_index));

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
        (pm8x50_port_speed_config_get(unit, port, pm_info, &speed_config));

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

int pm8x50_port_fec_error_inject_set(int unit, int port, pm_info_t pm_info,
                                     uint16 error_control_map,
                                     portmod_fec_error_mask_t bit_error_mask)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT
        (phymod_phy_fec_error_inject_set(&phy_access, error_control_map, bit_error_mask));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_fec_error_inject_get(int unit, int port, pm_info_t pm_info,
                                     uint16* error_control_map,
                                     portmod_fec_error_mask_t* bit_error_mask)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT
        (phymod_phy_fec_error_inject_get(&phy_access, error_control_map, bit_error_mask));

exit:
    SOC_FUNC_RETURN;
}

#endif
