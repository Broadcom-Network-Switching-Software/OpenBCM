/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phynull.h
 * Purpose:
 */

#ifndef   _PHY_PORTMOD_DISPATCH_H_
#define   _PHY_PORTMOD_DISPATCH_H_

#ifdef PORTMOD_SUPPORT
extern int phy_portmod_dispatch_init(int unit, soc_port_t port);
extern int phy_portmod_dispatch_reset(int unit, soc_port_t port, void *user_arg);
extern int phy_portmod_dispatch_set(int unit, soc_port_t port, int parm);
extern int phy_portmod_dispatch_one_get(int unit, soc_port_t port, int *parm);
extern int phy_portmod_dispatch_zero_get(int unit, soc_port_t port, int *parm);
extern int phy_portmod_dispatch_enable_get(int unit, soc_port_t port, int *enable);
extern int phy_portmod_dispatch_duplex_set(int unit, soc_port_t port, int parm);
extern int phy_portmod_dispatch_duplex_get(int unit, soc_port_t port, int *parm);
extern int phy_portmod_dispatch_speed_set(int unit, soc_port_t port, int parm);
extern int phy_portmod_dispatch_speed_get(int unit, soc_port_t port, int *parm);
extern int phy_portmod_dispatch_an_get(int unit, soc_port_t port, int *an, int *an_done);
extern int phy_portmod_dispatch_mode_set(int unit, soc_port_t port, soc_port_mode_t mode);
extern int phy_portmod_dispatch_mode_get(int unit, soc_port_t port, soc_port_mode_t *mode);
extern int phy_portmod_dispatch_mdix_set(int unit, soc_port_t port,
                             soc_port_mdix_t mode);
extern int phy_portmod_dispatch_mdix_get(int unit, soc_port_t port,
                             soc_port_mdix_t *mode);
extern int phy_portmod_dispatch_mdix_status_get(int unit, soc_port_t port,
                                    soc_port_mdix_status_t *status);
extern int phy_portmod_dispatch_medium_get(int unit, soc_port_t port,
                               soc_port_medium_t *medium);
extern int phy_portmod_dispatch_control_get(int unit, soc_port_t port,
                                soc_phy_control_t phy_ctrl, uint32 *param);

extern int phy_portmod_dispatch_notify(int unit, soc_port_t port,
                                      soc_phy_event_t event, uint32 value);
extern int
phy_portmod_dispatch_an_set(int unit, soc_port_t port, int an_enable);

#endif

#endif /* _PHY_PORTMOD_DISPATCH_H_ */
