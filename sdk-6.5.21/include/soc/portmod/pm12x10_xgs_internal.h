/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PM12X10_XGS_INTERNAL_H__H_
#define _PM12X10_XGS_INTERNAL_H__H_

#ifdef PORTMOD_PM12X10_XGS_SUPPORT

int pm12x10_xgs_pm4x10_enable(int unit, int port, uint32 phy, uint32 enable);
int pm12x10_xgs_pm4x10_tsc_reset(int unit, int port, uint32 phy, uint32 in_reset);

#endif /* PORTMOD_PM12X10_XGS_SUPPORT */
 
#endif /*_PM12X10_XGS_INTERNAL_H__H_*/
