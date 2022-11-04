
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#include "../types/physical_types.h"
#include "../types/logical_types.h"

#ifndef _PEMLADRV_LOGICAL_ACCESS_APPLET_H_
#define _PEMLADRV_LOGICAL_ACCESS_APPLET_H_

void build_phy_mem_struct_for_applets_flush(
    const int unit,
    phy_mem_t * mem,
    const int core,
    const int is_ingress0_egress1,
    const int is_mem0_reg1);

shr_error_e wait_for_trigger_reg(
    const int unit,
    const int is_ingress0_egress1);

shr_error_e start_writing_applets(
    const int unit);

shr_error_e flush_applets_to_single_mem(
    const int unit,
    const int core,
    const int is_ingress0_egress1);

shr_error_e flush_applets(
    const int unit);

int is_mem_belong_to_ingress0_or_egress1(
    phy_mem_t * mem,
    int min_ndx,
    int max_ndx);

#endif
