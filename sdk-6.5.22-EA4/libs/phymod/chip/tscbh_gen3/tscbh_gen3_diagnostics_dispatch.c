/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *         
 *     
 * DO NOT EDIT THIS FILE!
 */

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>

#ifdef PHYMOD_TSCBH_GEN3_SUPPORT

#include <phymod/chip/tscbh_gen3_diagnostics.h>

#include <phymod/chip/blackhawk7_v1l8p1_diagnostics.h>

__phymod_diagnostics__dispatch__t__ phymod_diagnostics_tscbh_gen3_diagnostics_driver = {

    NULL, /* phymod_phy_rx_slicer_position_set */
    NULL, /* phymod_phy_rx_slicer_position_get */
    NULL, /* phymod_phy_rx_slicer_position_max_get */
    blackhawk7_v1l8p1_phy_prbs_config_set,
    blackhawk7_v1l8p1_phy_prbs_config_get,
    blackhawk7_v1l8p1_phy_prbs_enable_set,
    blackhawk7_v1l8p1_phy_prbs_enable_get,
    blackhawk7_v1l8p1_phy_prbs_status_get,
    blackhawk7_v1l8p1_phy_pattern_config_set,
    blackhawk7_v1l8p1_phy_pattern_config_get,
    blackhawk7_v1l8p1_phy_pattern_enable_set,
    blackhawk7_v1l8p1_phy_pattern_enable_get,
    NULL, /* phymod_core_diagnostics_get */
    NULL, /* phymod_phy_diagnostics_get */
    blackhawk7_v1l8p1_phy_pmd_info_dump,
    NULL, /* phymod_phy_pcs_info_dump */
    blackhawk7_v1l8p1_phy_eyescan_run,
    NULL, /* phymod_phy_link_mon_enable_set */
    NULL, /* phymod_phy_link_mon_enable_get */
    NULL, /* phymod_phy_link_mon_status_get */
    NULL, /* phymod_phy_fec_correctable_counter_get */
    NULL, /* phymod_phy_fec_uncorrectable_counter_get */
    NULL, /* phymod_phy_stat_get */
    tscbh_gen3_phy_fec_cl91_correctable_counter_get,
    tscbh_gen3_phy_fec_cl91_uncorrectable_counter_get,
    blackhawk7_v1l8p1_phy_PAM4_tx_pattern_enable_set,
    blackhawk7_v1l8p1_phy_PAM4_tx_pattern_enable_get,
    blackhawk7_v1l8p1_phy_ber_proj,
    blackhawk7_v1l8p1_phy_fast_ber_proj_get,
    tscbh_gen3_phy_rsfec_symbol_error_counter_get,
    blackhawk7_v1l8p1_phy_linkcat,
    blackhawk7_v1l8p1_phy_pmd_lane_diag_debug_level_set,
    blackhawk7_v1l8p1_phy_pmd_lane_diag_debug_level_get,
    NULL, /* phymod_phy_pmd_usr_misc_ctrl_set */
    NULL, /* phymod_phy_pmd_usr_misc_ctrl_get */
    tscbh_gen3_phy_fec_error_inject_set,
    tscbh_gen3_phy_fec_error_inject_get,
};

#endif /* PHYMOD_TSCBH_GEN3_SUPPORT */
