/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        stat.h
 * Purpose:     STAT internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DNXF_STAT_H_
#define   _BCM_INT_DNXF_STAT_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif

int dnxf_stat_init(int unit);
int dnxf_stat_deinit(int unit);

#endif /*_BCM_INT_DNXF_STAT_H_*/
