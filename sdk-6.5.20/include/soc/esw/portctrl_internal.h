/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        portctrl_internal.h
 * Purpose:     SDK SOC Port Control Glue Layer
 */

#ifndef   _SOC_ESW_PORTCTRL_INTERNAL_H_
#define   _SOC_ESW_PORTCTRL_INTERNAL_H_

#ifdef PORTMOD_SUPPORT

#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod.h>

/* utility functions called from device specific portmod code */
extern int
soc_esw_portctrl_pm_user_access_alloc(int unit, int num,
                   portmod_default_user_access_t **user_acc);
extern void
soc_esw_portctrl_dump_txrx_lane_map(int unit, int first_port, int logical_port,
                   int core_num, uint32 txlane_map_b, uint32 rxlane_map_b,
                   uint32 txlane_map, uint32 rxlane_map);
extern int soc_esw_portctrl_reset_tsc0_cb(int unit, int port, uint32 in_reset);
extern int soc_esw_portctrl_reset_tsc1_cb(int unit, int port, uint32 in_reset);
extern int soc_esw_portctrl_reset_tsc2_cb(int unit, int port, uint32 in_reset);
extern int soc_esw_portctrl_reset_tsc3_cb(int unit, int port, uint32 in_reset);
extern int soc_esw_portctrl_reset_qtsce_cb(int unit, int port, uint32 in_reset);
extern int soc_esw_portctrl_reset_gphy_cb(int unit, int port, uint32 in_reset);

extern int
soc_esw_portctrl_config_get(int unit, soc_port_t port, 
                            portmod_port_interface_config_t* interface_config,
                            portmod_port_init_config_t* init_config, 
                            phymod_operation_mode_t *phy_op_mode);


#endif /* PORTMOD_SUPPORT */

#endif /* _SOC_ESW_PORTCTRL_INTERNAL_H_ */

