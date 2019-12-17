/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
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

#define PM_4x25_INFO(pm_info) ((pm_info)->pm_data.pm4x25_db)


int _pm4x25_txpi_sdm_scheme_set(int unit, int port, pm_info_t pm_info, portmod_txpi_sdm_type_t type);
int _pm4x25_txpi_lane_select_set(int unit, int port, pm_info_t pm_info, uint32_t *lane_select);
int _pm4x25_port_phy_txpi_mode_set(int unit, int port, pm_info_t pm_info, uint32 mode);

#endif /*_PM4X25TD_SHARED_H_*/
