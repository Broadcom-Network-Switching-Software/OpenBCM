/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * PHY Simulator (requires PHYMOD library)
 */

#ifndef __PHY_PHYMOD_SIM_H__
#define __PHY_PHYMOD_SIM_H__

#include <soc/defs.h>

#ifdef PHYMOD_SUPPORT

#include <phymod/phymod_sim.h>
#include <phymod/phymod.h>

extern int
soc_physim_add(int unit, uint32 phy_id,
               phymod_sim_drv_t *pms_drv);

#endif

extern int
soc_physim_wrmask(int unit, uint32 phy_id,
                  uint32 phy_reg_addr, uint16 phy_wr_data, uint16 wr_mask);

extern int
soc_physim_write(int unit, uint32 phy_id,
                 uint32 phy_reg_addr, uint16 phy_wr_data);

extern int
soc_physim_read(int unit, uint32 phy_id,
                uint32 phy_reg_addr, uint16 *phy_rd_data);

#ifdef PORTMOD_SUPPORT

extern int
soc_physim_check_sim(int unit, phymod_dispatch_type_t type,
                      phymod_access_t* access, uint32 addr_ident, int* is_sim);
extern int soc_physim_scache_allocate(int unit);
extern int soc_physim_scache_sync(int unit);
extern int soc_physim_scache_recovery(int unit);
extern void
soc_physim_enable_get(int unit, int* is_sim);
#endif /* PORTMOD_SUPPORT */

#endif /*__PHY_PHYMOD_SIM_H__*/

