/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DNXF STAT H
 */
 
#ifndef _SOC_DNXF_STAT_H_
#define _SOC_DNXF_STAT_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif

#include <bcm/types.h>
#include <soc/error.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <bcm/stat.h>

#define SOC_DNXF_MAX_NOF_COUNTERS_PER_STAT_TYPE (3)

#endif 
