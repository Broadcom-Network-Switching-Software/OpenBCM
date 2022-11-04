/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * RAMON DEFS H
 */

#ifndef _SOC_RAMON_DIAG_H_
#define _SOC_RAMON_DIAG_H_

#ifndef BCM_DNXF1_SUPPORT
#error "This file is for use by DNXF1 (Ramon) family only!"
#endif

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxc/fabric.h>

shr_error_e soc_ramon_diag_fabric_cell_snake_test_interrupts_name_get(
    int unit,
    const soc_dnxf_diag_flag_str_t ** intr_names);

shr_error_e soc_ramon_counters_get_info(
    int unit,
    int core_idx,
    soc_dnxf_counters_info_t * fe_counters_info);

shr_error_e soc_ramon_diag_cell_pipe_counter_get(
    int unit,
    int pipe,
    uint64 *counter);

shr_error_e soc_ramon_queues_get_info(
    int unit,
    soc_dnxf_queues_info_t * fe_queues_info);

shr_error_e soc_ramon_diag_mesh_topology_get(
    int unit,
    soc_dnxc_fabric_mesh_topology_info_t * mesh_topology_info);

shr_error_e soc_ramon_temperature_monitors_get(
    int unit,
    int temperature_monitors_max,
    soc_switch_temperature_monitor_t * temperature_array,
    int *temperature_monitors_count);

#endif
