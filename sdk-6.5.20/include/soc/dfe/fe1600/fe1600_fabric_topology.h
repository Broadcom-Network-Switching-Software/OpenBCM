/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE1600 FABRIC TOPOLOGY H
 */
 
#ifndef _SOC_FE1600_FABRIC_TOPOLOGY_H_
#define _SOC_FE1600_FABRIC_TOPOLOGY_H_

#include <bcm/fabric.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/error.h>

soc_error_t soc_fe1600_fabric_topology_isolate_set(int unit, soc_dcmn_isolation_status_t val);
soc_error_t soc_fe1600_fabric_topology_reachability_mask_set(int unit, soc_pbmp_t active_links, soc_dcmn_isolation_status_t val);
soc_error_t soc_fe1600_fabric_topology_isolate_get(int unit, soc_dcmn_isolation_status_t* val);
soc_error_t soc_fe1600_fabric_link_topology_set(int unit, soc_module_t destination_local_id, int links_count, soc_port_t *links_array);
soc_error_t soc_fe1600_fabric_link_topology_get(int unit, soc_module_t destination_local_id, int links_count_max, int *links_count, soc_port_t *links_array);
soc_error_t soc_fe1600_fabric_topology_repeater_destination_set(int unit, soc_port_t source, soc_port_t destination);
soc_error_t soc_fe1600_fabric_topology_repeater_destination_get(int unit, soc_port_t source, soc_port_t* destination);
soc_error_t soc_fe1600_fabric_topology_nof_links_to_min_nof_links_default(int unit, int nof_links, int *min_nof_links);
soc_error_t soc_fe1600_fabric_topology_min_nof_links_set(int unit, int min_nof_links);
soc_error_t soc_fe1600_fabric_topology_min_nof_links_get(int unit, int *min_nof_links);
soc_error_t soc_fe1600_fabric_topology_rmgr_set(int unit, int enable);
soc_error_t soc_fe1600_fabric_topology_mesh_topology_reset(int unit);
soc_error_t soc_fe1600_fabric_topology_all_reachable_integrity_update(int unit);
int soc_fe1600_log2_round_up(uint32 x);
#endif 
