/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phynull.h
 * Purpose:     
 */

#ifndef   _PHY_NULL_H_
#define   _PHY_NULL_H_


extern int phy_null_init(int unit, soc_port_t port);
extern int phy_null_reset(int unit, soc_port_t port, void *user_arg);
extern int phy_null_set(int unit, soc_port_t port, int parm);
extern int phy_null_one_get(int unit, soc_port_t port, int *parm);
extern int phy_null_zero_get(int unit, soc_port_t port, int *parm);
extern int phy_null_enable_get(int unit, soc_port_t port, int *enable);
extern int phy_null_duplex_set(int unit, soc_port_t port, int parm);
extern int phy_null_duplex_get(int unit, soc_port_t port, int *parm);
extern int phy_null_speed_set(int unit, soc_port_t port, int parm);
extern int phy_null_speed_get(int unit, soc_port_t port, int *parm);
extern int phy_null_an_get(int unit, soc_port_t port, int *an, int *an_done);
extern int phy_null_mode_set(int unit, soc_port_t port, soc_port_mode_t mode);
extern int phy_null_mode_get(int unit, soc_port_t port, soc_port_mode_t *mode);
extern int phy_null_interface_set(int unit, soc_port_t port,
				  soc_port_if_t pif);
extern int phy_null_interface_get(int unit, soc_port_t port,
				  soc_port_if_t *pif);
extern int phy_null_mdix_set(int unit, soc_port_t port, 
                             soc_port_mdix_t mode);
extern int phy_null_mdix_get(int unit, soc_port_t port, 
                             soc_port_mdix_t *mode);
extern int phy_null_mdix_status_get(int unit, soc_port_t port, 
                                    soc_port_mdix_status_t *status);
extern int phy_null_medium_get(int unit, soc_port_t port,
                               soc_port_medium_t *medium);
extern int phy_null_control_get(int unit, soc_port_t port, 
                                soc_phy_control_t phy_ctrl, uint32 *param);
extern int phy_null_notify(int unit, soc_port_t port,
                           soc_phy_event_t event, uint32 value);

extern int phy_nocxn_mode_get(int unit, soc_port_t port,
			      soc_port_mode_t *mode);
extern int phy_nocxn_interface_set(int unit, soc_port_t port,
				   soc_port_if_t pif);
extern int phy_nocxn_interface_get(int unit, soc_port_t port,
				   soc_port_if_t *pif);

#endif /* _PHY_NULL_H_ */
