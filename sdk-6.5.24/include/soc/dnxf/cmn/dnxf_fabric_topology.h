/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * DNXF FABRIC H
 */

#ifndef _SOC_DNXF_FABRIC_TOPOLOGY_H_
#define _SOC_DNXF_FABRIC_TOPOLOGY_H_

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF family only!"
#endif

#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxc/dnxc_defs.h>
#include <soc/error.h>
#include <soc/types.h>

shr_error_e soc_dnxf_fabric_topology_modid_group_map_set(
    int unit,
    soc_module_t modid,
    int group);

shr_error_e soc_dnxf_fabric_topology_mc_sw_control_set(
    int unit,
    int group,
    int enable);

shr_error_e soc_dnxf_fabric_topology_mc_sw_control_get(
    int unit,
    int group,
    int *enable);

#endif
