/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE3200 FABRIC TOPOLOGY H
 */
 
#ifndef _SOC_FE3200_FABRIC_TOPOLOGY_H_
#define _SOC_FE3200_FABRIC_TOPOLOGY_H_

#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dcmn/dcmn_defs.h>
#include <soc/error.h>
#include <soc/types.h>

soc_error_t soc_fe3200_fabric_topology_reachability_mask_set(int unit, soc_pbmp_t active_links, soc_dcmn_isolation_status_t val);
soc_error_t soc_fe3200_fabric_topology_nof_links_to_min_nof_links_default(int unit, int nof_links, int *min_nof_links);
soc_error_t soc_fe3200_fabric_topology_min_nof_links_set(int unit, int min_nof_links);
soc_error_t soc_fe3200_fabric_topology_min_nof_links_get(int unit, int *min_nof_links);
soc_error_t soc_fe3200_fabric_topology_isolate_set(int unit, soc_dcmn_isolation_status_t val);
soc_error_t soc_fe3200_fabric_topology_mesh_topology_reset(int unit);


#endif

