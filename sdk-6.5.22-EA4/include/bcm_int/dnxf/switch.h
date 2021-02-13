/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * File:        link.h
 * Purpose:     LINK internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DNXF_SWITCH_H_
#define   _BCM_INT_DNXF_SWITCH_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif

#include <sal/types.h>
#include <bcm/types.h>
#include <bcm/switch.h>

extern int bcm_dnxf_switch_temperature_monitor_get(int,int,bcm_switch_temperature_monitor_t *,int *);

#endif /*_BCM_INT_DNXF_SWITCH_H_*/
