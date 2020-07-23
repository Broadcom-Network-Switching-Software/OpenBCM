/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phyxehg.h
 * Purpose:     Defines common PHY driver routines for Broadcom 10G PHY.
 */
#ifndef _PHY_XEHG_H
#define _PHY_XEHG_H

extern int
phy_xehg_speed_set(int unit, soc_port_t port, int speed);

extern int
phy_xehg_speed_get(int unit, soc_port_t port, int *speed);

extern int
phy_xehg_interface_get(int unit, soc_port_t port, soc_port_if_t *pif);

extern int
phy_xehg_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode);

#endif /* _PHY_XEHG_H */

