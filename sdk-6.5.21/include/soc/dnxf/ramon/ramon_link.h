/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RAMON STAT H
 */
 
#ifndef _SOC_RAMON_LINK_H_
#define _SOC_RAMON_LINK_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif 

#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/error.h>
#include <soc/dnxc/error.h>

int soc_ramon_linkctrl_init(int unit);

#endif

