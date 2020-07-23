/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PM4X25TD_SHARED_H_
#define _PM4X25TD_SHARED_H_

#define PM4X25_LANES_PER_CORE (4)
#define MAX_PORTS_PER_PM4X25 (4)
#define NUM_VSD_ACTIVE_GROUP_WORD  4

struct pm4x25_s{
    portmod_pbmp_t phys;
    int first_phy;
    phymod_ref_clk_t ref_clk;
    phymod_polarity_t polarity;
    phymod_lane_map_t lane_map;
    phymod_firmware_load_method_t fw_load_method;
    phymod_firmware_loader_f external_fw_loader;
    phymod_core_access_t int_core_access;
    int nof_phys[PM4X25_LANES_PER_CORE] ; /* internal + external phys for each lane */
    uint8 in_pm12x10;
    uint8 core_num;
    portmod_mac_soft_reset_f portmod_mac_soft_reset;
    portmod_xphy_lane_connection_t lane_conn[MAX_PHYN][PM4X25_LANES_PER_CORE];
    phymod_afe_pll_t afe_pll;
    int warmboot_skip_db_restore;
    int rescal;
    int is_mixed_mode;
};

typedef enum pm4x25_fabric_wb_vars{
    isActive = 0, /* specify the PM4x25 core is active (at least one port has been attached) */
    isBypassed, /*specify the PM is in Bypass mode - the MAC is not in use, some configurations do not apply*/
    isInitialized, /* specify initialization of all SerDes have been done */
    lane2portMap, /* map each lane in the PM to a logical port */
    threePortsMode, 
    phy_type, /* Phy type in PHYMOD dispatch phymod_dispatch_type_t */
    interfaceConfig, /* portmod_port_interface_config_t **removed** */
    portInitConfig, /* portmod_port_init_config_t **removed** */
    nofPhys,/* keep track of nun of phys (including internal Serdes. */
    maxSpeed, /* Max Port Speed Supported */
    interfaceType, /* interface of type soc_port_if_t */
    interfaceModes, /* see PHYMOD_INTF_MODES_ */
    isInterfaceRestore, /* interface should be restored from DB (otherwise retrieved from HW), replace PHYMOD_INTF_F_INTF_PARAM_SET_ONLY */
    isHg, /* specify wheather the port is higig port */
    anMode, /* spn_PHY_AN_MODE - Specify the AN mode */
    fsCl72, /* spn_FORCED_INIT_CL72 - if trainig should be enabled in Forced speed mode */
    cx4In10G, /* see PHYMOD_BAM_CL37_CAP_10G_CX4 */
    anCl72, /* spn_PHY_AN_C72 - Specify if the link training should be enabled */
    anFec, /* spn_FORCED_INIT_FEC specify if FEC should be enabled in AN mode */
    anMasterLane, /* spn_PHY_AUTONEG_MASTER_LANE - Specify the AN master lane */
    laneConnectionDb, /* represents the physical topology of the  serdes and ext phy lanes */
    interfaceWrittenIntoOutmostPhy, /* the interface type written into the outmost PHY */
    portDynamicState,
    tsAdjustOld, /* the time stamp adjust setting on MAC */
    pllDividerReq, /*pll_divider_req setting on the port */
    serdes1000xAt25gVco, /*1G working at 25g*/
    serdes10gAt25gVco, /*10G Base-R working at 25g*/
    cpriVsdActiveGrp,   /* CPRI TX VSD active group */
    tsAdjust, /* the time stamp adjust setting on MAC */
    linkRecovery, /* Used by the link recovery WAR */
    pll_vco_rate, /*VCO value of the PM tvco check*/
    pll0ActiveLaneBitmap, /* keeps allports using pll0 */
    pll1ActiveLaneBitmap,  /* keeps allports using pll1 */
    pll0vcoRate,       /* VCO rate pll0 */
    pll1vcoRate,        /* VCO rate pll1 */
    userTimeAdjust,      /* User time adjust. */
    ptpTimeAdjust,       /* PTP time adjust. */
    anCl72TxInitSkipOnRestart /* skip the TXFIR initialization in a restart event during the AN link training */
}pm4x25_wb_vars_t;

#define PM_4x25_INFO(pm_info) ((pm_info)->pm_data.pm4x25_db)

#define PM4x25_IS_ACTIVE_SET(unit, pm_info, is_active) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], &is_active)
#define PM4x25_IS_ACTIVE_GET(unit, pm_info, is_active) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], is_active)

#define PM4x25_IS_INITIALIZED_SET(unit, pm_info, is_initialized) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isInitialized], &is_initialized)
#define PM4x25_IS_INITIALIZED_GET(unit, pm_info, is_initialized) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isInitialized], is_initialized)

#define PM4x25_IS_BYPASSED_SET(unit, pm_info, is_bypass) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isBypassed], &is_bypass)
#define PM4x25_IS_BYPASSED_GET(unit, pm_info, is_bypass) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isBypassed], is_bypass)

#define PM4x25_LANE2PORT_SET(unit, pm_info, lane, port) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[lane2portMap], &port, lane)
#define PM4x25_LANE2PORT_GET(unit, pm_info, lane, port) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[lane2portMap], port, lane)

#define PM4x25_THREE_PORTS_MODE_SET(unit, pm_info, three_ports_mode) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[threePortsMode], &three_ports_mode)
#define PM4x25_THREE_PORTS_MODE_GET(unit, pm_info, three_ports_mode) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[threePortsMode], three_ports_mode)

#define PM4x25_MAX_SPEED_SET(unit, pm_info, max_speed, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[maxSpeed], &max_speed, port_index)
#define PM4x25_MAX_SPEED_GET(unit, pm_info, max_speed, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[maxSpeed], max_speed, port_index)

#define PM4x25_INTERFACE_TYPE_SET(unit, pm_info, interface_type, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[interfaceType], &interface_type, port_index)
#define PM4x25_INTERFACE_TYPE_GET(unit, pm_info, interface_type, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[interfaceType], interface_type, port_index)

#define PM4x25_INTERFACE_MODES_SET(unit, pm_info, interface_modes, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[interfaceModes], &interface_modes, port_index)
#define PM4x25_INTERFACE_MODES_GET(unit, pm_info, interface_modes, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[interfaceModes], interface_modes, port_index)

#define PM4x25_OUTMOST_PHY_INTERFACE_TYPE_SET(unit, pm_info, interface_type, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                                   \
                          pm_info->wb_vars_ids[interfaceWrittenIntoOutmostPhy],          \
                          &interface_type, port_index)
#define PM4x25_OUTMOST_PHY_INTERFACE_TYPE_GET(unit, pm_info, interface_type, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                                   \
                          pm_info->wb_vars_ids[interfaceWrittenIntoOutmostPhy],          \
                          interface_type, port_index)

#define PM4x25_IS_INTERFACE_RESTORE_SET(unit, pm_info, is_interface_restore, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isInterfaceRestore], &is_interface_restore, port_index)
#define PM4x25_IS_INTERFACE_RESTORE_GET(unit, pm_info, is_interface_restore, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isInterfaceRestore], is_interface_restore, port_index)

#define PM4x25_IS_HG_SET(unit, pm_info, is_hg, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isHg], &is_hg, port_index)
#define PM4x25_IS_HG_GET(unit, pm_info, is_hg, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isHg], is_hg, port_index)

#define PM4x25_AN_MODE_SET(unit, pm_info, an_mode, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[anMode], &an_mode, port_index)
#define PM4x25_AN_MODE_GET(unit, pm_info, an_mode, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[anMode], an_mode, port_index)

#define PM4x25_FS_CL72_SET(unit, pm_info, fs_cl72, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[fsCl72], &fs_cl72, port_index)
#define PM4x25_FS_CL72_GET(unit, pm_info, fs_cl72, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[fsCl72], fs_cl72, port_index)

#define PM4x25_CX4_10G_SET(unit, pm_info, cx4_10G, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[cx4In10G], &cx4_10G, port_index)
#define PM4x25_CX4_10G_GET(unit, pm_info, cx4_10G, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[cx4In10G], cx4_10G, port_index)

#define PM4x25_AN_CL72_SET(unit, pm_info, an_cl72, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[anCl72], &an_cl72, port_index)
#define PM4x25_AN_CL72_GET(unit, pm_info, an_cl72, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[anCl72], an_cl72, port_index)

#define PM4x25_AN_FEC_SET(unit, pm_info, an_fec, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[anFec], &an_fec, port_index)
#define PM4x25_AN_FEC_GET(unit, pm_info, an_fec, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[anFec], an_fec, port_index)

#define PM4x25_AN_MASTER_LANE_SET(unit, pm_info, an_master_lane, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[anMasterLane], &an_master_lane, port_index)
#define PM4x25_AN_MASTER_LANE_GET(unit, pm_info, an_master_lane, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[anMasterLane], an_master_lane, port_index)

#define PM4x25_PORT_LANE_CONNECTION_DB_SET(unit, pm_info, lane_connection_db, xphy_idx, lane) \
    SOC_WB_ENGINE_SET_DBL_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[laneConnectionDb], \
        (uint8 *)lane_connection_db, lane, xphy_idx)
#define PM4x25_PORT_LANE_CONNECTION_DB_GET(unit, pm_info, lane_connection_db, xphy_idx, lane) \
    SOC_WB_ENGINE_GET_DBL_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[laneConnectionDb], \
        (uint8 *)lane_connection_db, lane, xphy_idx)

#define PM4x25_PORT_NUM_PHYS_SET(unit, pm_info, nof_phys, lane)   \
        SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[nofPhys], &nof_phys, lane)
#define PM4x25_PORT_NUM_PHYS_GET(unit, pm_info, nof_phys, lane)   \
        SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[nofPhys], nof_phys, lane)

#define PM4x25_PLL0_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll0_active_lane_bitmap)\
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                            \
                          pm_info->wb_vars_ids[pll0ActiveLaneBitmap],             \
                          &pll0_active_lane_bitmap)
#define PM4x25_PLL0_ACTIVE_LANE_BITMAP_GET(unit, pm_info, pll0_active_lane_bitmap)\
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                            \
                          pm_info->wb_vars_ids[pll0ActiveLaneBitmap],             \
                          pll0_active_lane_bitmap)

#define PM4x25_PLL1_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll1_active_lane_bitmap)\
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                            \
                          pm_info->wb_vars_ids[pll1ActiveLaneBitmap],             \
                          &pll1_active_lane_bitmap)
#define PM4x25_PLL1_ACTIVE_LANE_BITMAP_GET(unit, pm_info, pll1_active_lane_bitmap)\
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                            \
                          pm_info->wb_vars_ids[pll1ActiveLaneBitmap],             \
                          pll1_active_lane_bitmap)

#define PM4x25_PLL0_VCO_RATE_SET(unit, pm_info, pll0_vco_rate) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,         \
                          pm_info->wb_vars_ids[pll0vcoRate],   \
                          &pll0_vco_rate)
#define PM4x25_PLL0_VCO_RATE_GET(unit, pm_info, pll0_vco_rate) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,         \
                          pm_info->wb_vars_ids[pll0vcoRate],   \
                          pll0_vco_rate)

#define PM4x25_PLL1_VCO_RATE_SET(unit, pm_info, pll1_vco_rate) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,         \
                          pm_info->wb_vars_ids[pll1vcoRate],   \
                          &pll1_vco_rate)
#define PM4x25_PLL1_VCO_RATE_GET(unit, pm_info, pll1_vco_rate) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,         \
                          pm_info->wb_vars_ids[pll1vcoRate],   \
                          pll1_vco_rate)

#define PM4x25_PORT_DYNAMIC_STATE_SET(unit, pm_info, dynamic_state, \
                                      port_index)                   \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,              \
                          pm_info->wb_vars_ids[portDynamicState],   \
                          &dynamic_state, port_index)
#define PM4x25_PORT_DYNAMIC_STATE_GET(unit, pm_info, dynamic_state, \
                                      port_index)                   \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,              \
                          pm_info->wb_vars_ids[portDynamicState],   \
                          dynamic_state, port_index)
#define PM4x25_PORT_TIME_STAMP_ADJUST_OLD_SET(unit, pm_info,                    \
                                              port_ts_adjust,                 \
                                              adjust_index,                    \
                                              port_index)                     \
          SOC_WB_ENGINE_SET_DBL_ARR(unit, SOC_WB_ENGINE_PORTMOD,              \
                                    pm_info->wb_vars_ids[tsAdjustOld],           \
                                    (uint8 *)port_ts_adjust, adjust_index, port_index)

#define PM4x25_PORT_TIME_STAMP_ADJUST_OLD_GET(unit, pm_info,                    \
                                              port_ts_adjust,                 \
                                              adjust_index,                    \
                                              port_index)                     \
        SOC_WB_ENGINE_GET_DBL_ARR(unit, SOC_WB_ENGINE_PORTMOD,              \
                                  pm_info->wb_vars_ids[tsAdjustOld],           \
                                  (uint8 *)port_ts_adjust, adjust_index, port_index)

#define PM4x25_PORT_TIME_STAMP_ADJUST_SET(unit, pm_info,                    \
                                            port_ts_adjust,                 \
                                            adjust_index,                    \
                                            port_index)                     \
        SOC_WB_ENGINE_SET_DBL_ARR(unit, SOC_WB_ENGINE_PORTMOD,              \
                                  pm_info->wb_vars_ids[tsAdjust],           \
                                  (uint8 *)port_ts_adjust, adjust_index, port_index)

#define PM4x25_PORT_TIME_STAMP_ADJUST_GET(unit, pm_info,                    \
                                            port_ts_adjust,                 \
                                            adjust_index,                    \
                                            port_index)                     \
        SOC_WB_ENGINE_GET_DBL_ARR(unit, SOC_WB_ENGINE_PORTMOD,              \
                                  pm_info->wb_vars_ids[tsAdjust],           \
                                  (uint8 *)port_ts_adjust, adjust_index, port_index)

#define PM4x25_PORT_PLL_DIVIDER_REQ_SET(unit, pm_info,                \
                                        pll_divider_req,              \
                                        port_index)                   \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                \
                          pm_info->wb_vars_ids[pllDividerReq],        \
                          &pll_divider_req, port_index)
#define PM4x25_PORT_PLL_DIVIDER_REQ_GET(unit, pm_info,                \
                                        pll_divider_req,              \
                                        port_index)                   \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                \
                          pm_info->wb_vars_ids[pllDividerReq],        \
                          pll_divider_req, port_index)

#define PM4x25_PORT_1000X_AT_25G_SET(unit, pm_info,                   \
                                        serdes_1000x_at_25g_vco,      \
                                        port_index)                   \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                \
                          pm_info->wb_vars_ids[serdes1000xAt25gVco],  \
                          &serdes_1000x_at_25g_vco, port_index)
#define PM4x25_PORT_1000X_AT_25G_GET(unit, pm_info,                   \
                                        serdes_1000x_at_25g_vco,      \
                                        port_index)                   \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                \
                          pm_info->wb_vars_ids[serdes1000xAt25gVco],  \
                          serdes_1000x_at_25g_vco, port_index)

#define PM4x25_PORT_10G_AT_25G_SET(unit, pm_info,                     \
                                        serdes_10g_at_25g_vco,        \
                                        port_index)                   \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                \
                          pm_info->wb_vars_ids[serdes10gAt25gVco],    \
                          &serdes_10g_at_25g_vco, port_index)
#define PM4x25_PORT_10G_AT_25G_GET(unit, pm_info,                     \
                                        serdes_10g_at_25g_vco,        \
                                        port_index)                   \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                \
                          pm_info->wb_vars_ids[serdes10gAt25gVco],    \
                          serdes_10g_at_25g_vco, port_index)
#define PM4x25_CPRI_PORT_VSD_ACTIVE_GROUP_SET(unit, pm_info, group_list, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[cpriVsdActiveGrp], group_list, port_index)
#define PM4x25_CPRI_PORT_VSD_ACTIVE_GROUP_GET(unit, pm_info, group_list, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[cpriVsdActiveGrp], group_list, port_index)

#define PM4x25_PORT_LINK_RECOVERY_SET(unit, pm_info,                  \
                                        link_recovery,                \
                                        port_index)                   \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                \
                          pm_info->wb_vars_ids[linkRecovery],         \
                          &link_recovery, port_index)
#define PM4x25_PORT_LINK_RECOVERY_GET(unit, pm_info,                  \
                                        link_recovery,                \
                                        port_index)                   \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                \
                          pm_info->wb_vars_ids[linkRecovery],         \
                          link_recovery, port_index)

#define PM4x25_VCO_SET(unit, pm_info, vco) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[pll_vco_rate], &vco)
#define PM4x25_VCO_GET(unit, pm_info, vco) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[pll_vco_rate], vco)

#define PM4x25_USER_TIME_ADJUST_SET(unit, pm_info, user_time_adjust, lane_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[userTimeAdjust], &user_time_adjust, lane_index)
#define PM4x25_USER_TIME_ADJUST_GET(unit, pm_info, user_time_adjust, lane_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[userTimeAdjust], user_time_adjust, lane_index)

#define PM4x25_PTP_TIME_ADJUST_SET(unit, pm_info, ptp_time_adjust, lane_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[ptpTimeAdjust], &ptp_time_adjust, lane_index)
#define PM4x25_PTP_TIME_ADJUST_GET(unit, pm_info, ptp_time_adjust, lane_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[ptpTimeAdjust], ptp_time_adjust, lane_index)

int _pm4x25_txpi_sdm_scheme_set(int unit, int port, pm_info_t pm_info, portmod_txpi_sdm_type_t type);
int _pm4x25_txpi_lane_select_set(int unit, int port, pm_info_t pm_info, uint32_t *lane_select, int physical_lane);
int _pm4x25_port_phy_txpi_mode_set(int unit, int port, pm_info_t pm_info, uint32 mode);

#endif /*_PM4X25TD_SHARED_H_*/
