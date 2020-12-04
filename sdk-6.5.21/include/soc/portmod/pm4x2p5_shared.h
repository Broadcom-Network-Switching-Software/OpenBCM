/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PM4X2P5_SHARED_H_
#define _PM4X2P5_SHARED_H_

#define PM4X2P5_LANES_PER_CORE (4)
#define MAX_PORTS_PER_PM4X2P5 (4)

struct pm4x2p5_s{
    portmod_pbmp_t phys;
    int first_phy;
    phymod_ref_clk_t ref_clk;
    phymod_polarity_t polarity;
    phymod_lane_map_t lane_map;
    phymod_firmware_load_method_t fw_load_method;
    phymod_firmware_loader_f external_fw_loader;
    phymod_core_access_t int_core_access ; /* for internal SerDes only */
    int nof_phys[PM4X2P5_LANES_PER_CORE]; /* internal + External Phys for each lane*/
    uint8 default_fw_loader_is_used;
    uint8 default_bus_is_used;
    portmod_phy_external_reset_f  portmod_phy_external_reset;
    portmod_mac_soft_reset_f portmod_mac_soft_reset;
    portmod_xphy_lane_connection_t lane_conn[MAX_PHYN][PM4X2P5_LANES_PER_CORE];
};

#define PM_4x2P5_INFO(pm_info) ((pm_info)->pm_data.pm4x2p5_db)

#endif /*_PM4X2P5_SHARED_H_*/
