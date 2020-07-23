 /*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        simul.h
 * Purpose:     
 */

#ifndef   _SIMUL_H_
#define   _SIMUL_H_

#include <soc/register.h>
#include <soc/ll.h>

extern int phy_simul_enable_set(int uint, soc_port_t port, int enable);
extern int phy_simul_enable_get(int uint, soc_port_t port, int *enable);
extern int phy_simul_init(int unit, soc_port_t port);
extern int phy_simul_reset(int unit, soc_port_t port, void *user_arg);
extern int phy_simul_link_get(int unit, soc_port_t port, int *link);
extern int phy_simul_duplex_set(int unit, soc_port_t port, int duplex);
extern int phy_simul_duplex_get(int unit, soc_port_t port, int *duplex);
extern int phy_simul_speed_set(int unit, soc_port_t port, int speed);
extern int phy_simul_speed_get(int unit, soc_port_t port, int *speed);
extern int phy_simul_master_set(int unit, soc_port_t port, int speed);
extern int phy_simul_master_get(int unit, soc_port_t port, int *speed);
extern int phy_simul_an_set(int unit, soc_port_t port, int an);
extern int phy_simul_an_get(int unit, soc_port_t port, int *an,
                             int *an_done);
extern int phy_simul_adv_local_set(int unit, soc_port_t port,
				   soc_port_mode_t mode);
extern int phy_simul_adv_local_get(int unit, soc_port_t port,
				   soc_port_mode_t *mode);
extern int phy_simul_adv_remote_get(int unit, soc_port_t port,
				    soc_port_mode_t *mode);
extern int phy_simul_lb_set(int unit, soc_port_t port, int enable);
extern int phy_simul_lb_get(int unit, soc_port_t port, int *enable);
extern int phy_simul_interface_set(int unit, soc_port_t port,
				   soc_port_if_t pif);
extern int phy_simul_interface_get(int unit, soc_port_t port,
				   soc_port_if_t *pif);
extern int phy_simul_ability_get(int unit, soc_port_t port,
				 soc_port_mode_t *mode);
extern int phy_simul_mdix_set(int unit, soc_port_t port, 
                              soc_port_mdix_t mode);
extern int phy_simul_mdix_get(int unit, soc_port_t port, 
                              soc_port_mdix_t *mode);
extern int phy_simul_mdix_status_get(int unit, soc_port_t port, 
                                     soc_port_mdix_status_t *status);
extern int phy_simul_medium_get(int unit, soc_port_t port,
                                soc_port_medium_t *medium);


#endif /* _SIMUL_H_ */
