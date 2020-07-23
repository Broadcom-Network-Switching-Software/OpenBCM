/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PM12X10_INTERNAL_H__H_
#define _PM12X10_INTERNAL_H__H_

#ifdef PORTMOD_PM12X10_SUPPORT

#include <phymod/phymod.h>
#include <phymod/phymod_diagnostics.h>
#include <soc/portmod/portmod_defs.h>

/* Utility functions provided for PM4x10 which layes under this PM12x10*/
int pm12x10_pm4x10_enable(int unit, int port, uint32 phy, uint32 enable);
int pm12x10_pm4x10_tsc_reset(int unit, int port, uint32 phy, uint32 in_reset);

#endif /* PORTMOD_PM12X10_SUPPORT */
 
#endif /*_PM12X10_INTERNAL_H__H_*/
