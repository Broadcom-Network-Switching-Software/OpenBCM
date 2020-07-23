/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PHY_PHYMOD_PORT_CONTROL_H_
#define _PHY_PHYMOD_PORT_CONTROL_H_

#include <soc/types.h>
#include <soc/phyctrl.h>
#include <phymod/phymod.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diag.h>

int soc_port_control_get_wrapper(int unit, 
                                 phymod_ref_clk_t ref_clock, 
                                 int is_lane_control, 
                                 phymod_phy_access_t *phys, 
                                 int nof_phys, 
                                 soc_phy_control_t control, 
                                 uint32 *value);

int soc_port_control_set_wrapper(int unit, 
                                 phymod_ref_clk_t ref_clock, 
                                 int is_lane_control, 
                                 phymod_phy_access_t *phys, 
                                 int nof_phys, 
                                 soc_phy_control_t control, 
                                 uint32 value);

int soc_prbs_poly_to_phymod(uint32 sdk_poly, phymod_prbs_poly_t *phymod_poly);
int phymod_prbs_poly_to_soc(phymod_prbs_poly_t phymod_poly, uint32 *sdk_poly);

int soc_phymod_phy_intf_to_port_intf (int unit,
                                phymod_interface_t phymod_interface,
                                soc_port_if_t *interface);

int soc_phymod_phy_intf_from_port_intf(int unit,
                                int speed,
                                soc_port_if_t interface,
                                phymod_interface_t *phymod_interface);
int soc_phymod_media_type_from_port_intf(int unit,
                                soc_port_if_t interface,
                                phymod_phy_inf_config_t *phy_interface_config);
int soc_phymod_intf_mode_from_phymod_intf(int unit,
                                phymod_interface_t phymod_interface,
                                phymod_phy_inf_config_t *phy_interface_config);

int soc_portctrl_phy_tx_get(int unit, phymod_phy_access_t *phys, phymod_tx_t *phy_tx);

int soc_portctrl_phy_tx_set(int unit, phymod_phy_access_t *phys, phymod_tx_t *phy_tx);

#endif /* _PHY_PHYMOD_PORT_CONTROL_H_ */
