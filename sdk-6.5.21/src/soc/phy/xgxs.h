/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        xgxs.h
 * Purpose:     Defines common PHY driver routines for Broadcom XGXS core.
 */
#ifndef _PHY_XGXS_H
#define _PHY_XGXS_H

extern int
phy_xgxs_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_data);

extern int
phy_xgxs_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_data);

extern int
phy_xgxs_reg_modify(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data,
                    uint32 phy_data_mask);

#endif /* _PHY_XGXS_H */

