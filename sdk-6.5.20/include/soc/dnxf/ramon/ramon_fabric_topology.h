/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RAMON FABRIC TOPOLOGY H
 */
 
#ifndef _SOC_RAMON_FABRIC_TOPOLOGY_H_
#define _SOC_RAMON_FABRIC_TOPOLOGY_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif

#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxc/dnxc_defs.h>
#include <soc/error.h>
#include <soc/types.h>

shr_error_e soc_ramon_fabric_topology_isolate_set(int unit, soc_dnxc_isolation_status_t val);
shr_error_e soc_ramon_fabric_topology_mesh_topology_reset(int unit);
shr_error_e soc_ramon_fabric_topology_modid_group_map_set(int unit, soc_module_t modid, int group);
shr_error_e soc_ramon_fabric_topology_mc_sw_control_set(int unit, int group, int enable);
shr_error_e soc_ramon_fabric_topology_mc_sw_control_get(int unit, int group, int *enable);
shr_error_e soc_ramon_fabric_topology_mc_set(int unit, soc_module_t destination_local_id, int links_count, soc_port_t *links_array);
shr_error_e soc_ramon_fabric_topology_mc_get(int unit, soc_module_t destination_local_id, int links_count_max, int *links_count, soc_port_t *links_array);
shr_error_e soc_ramon_fabric_topology_isolate_get(int unit, soc_dnxc_isolation_status_t* val);
shr_error_e soc_ramon_fabric_topology_rmgr_set(int unit, int enable);
shr_error_e soc_ramon_fabric_topology_lb_set(int unit, soc_module_t destination_local_id, int links_count, soc_port_t *links_array);
shr_error_e soc_ramon_fabric_topology_lb_get(int unit, soc_module_t destination_local_id, int links_count_max, int *links_count, soc_port_t *links_array);
shr_error_e soc_ramon_fabric_topology_repeater_destination_set(int unit, soc_port_t source, soc_port_t destination);
shr_error_e soc_ramon_fabric_topology_repeater_destination_get(int unit, soc_port_t source, soc_port_t* destination);
shr_error_e soc_ramon_fabric_topology_local_route_control_cells_enable_set(int unit, int enable);

#endif

