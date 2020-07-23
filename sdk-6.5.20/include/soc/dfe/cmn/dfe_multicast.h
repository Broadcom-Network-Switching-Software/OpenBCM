/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DFE MULTICAST H
 */
 
#ifndef _SOC_DFE_MULTICAST_H_
#define _SOC_DFE_MULTICAST_H_

#include <bcm/types.h>
#include <soc/error.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <bcm/multicast.h>


typedef struct soc_dfe_multicast_read_range_info_s
{
    int group_min;
    int group_max;
    int table;
    int is_first_half;
} soc_dfe_multicast_read_range_info_t;

#endif 
