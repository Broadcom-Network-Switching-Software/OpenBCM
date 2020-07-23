/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RAMON DEFS H
 */

#ifndef _SOC_RAMON_DIAG_H_
#define _SOC_RAMON_DIAG_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif 




#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_diag.h>


















shr_error_e soc_ramon_diag_fabric_cell_snake_test_interrupts_name_get(int unit, const soc_dnxf_diag_flag_str_t **intr_names);
shr_error_e soc_ramon_counters_get_info(int unit,soc_dnxf_counters_info_t* fe_counters_info);
shr_error_e soc_ramon_diag_cell_pipe_counter_get(int unit, int pipe, uint64 *counter);
shr_error_e soc_ramon_queues_get_info(int unit, soc_dnxf_queues_info_t* fe_queues_info);
shr_error_e soc_ramon_diag_mesh_topology_get(int unit, soc_dnxc_fabric_mesh_topology_diag_t *mesh_topology_diag);
int soc_ramon_temperature_monitor_regs_get(int unit, const soc_reg_t **temp_reg_ptr, const soc_field_t **currr_temp_field_ptr, const soc_field_t **peak_temp_field_ptr);


#endif
