/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DNXF MULTICAST H
 */
 
#ifndef _SOC_DNXF_MULTICAST_H_
#define _SOC_DNXF_MULTICAST_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif

#include <bcm/types.h>
#include <soc/error.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <bcm/multicast.h>

typedef struct soc_dnxf_multicast_read_range_info_s
{
    int group_min;
    int group_max;
    int table;
    int is_first_half;
} soc_dnxf_multicast_read_range_info_t;

#endif 
