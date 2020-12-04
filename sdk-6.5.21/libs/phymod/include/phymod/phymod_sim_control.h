/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * PHY Simulator (requires PHYMOD library)
 */

#ifndef __PHY_PHYMOD_SIM_CONTROL_H__
#define __PHY_PHYMOD_SIM_CONTROL_H__

#include <phymod/phymod_sim.h>

int
phymod_control_physim_add(int unit, uint32_t phy_id,
    phymod_sim_drv_t *pms_drv);

int
phymod_control_physim_read(int unit, uint32_t phy_id,
    uint32_t phy_reg_addr, uint16_t *phy_rd_data);

int
phymod_control_physim_write(int unit, uint32_t phy_id,
    uint32_t phy_reg_addr, uint16_t phy_wr_data);

int
phymod_control_physim_wrmask(int unit, uint32_t phy_id,
    uint32_t phy_reg_addr, uint16_t phy_wr_data, uint16_t wr_mask);

int
phymod_control_physim_drv_get(int unit,
    phymod_dispatch_type_t dispatch_type, phymod_sim_drv_t **drv);

int
phymod_control_physim_bus_get(int unit, phymod_bus_t **phymod_sim_bus);

#endif /*__PHY_PHYMOD_SIM_CONTROL_H__*/

