/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE3200 DEFS H
 */

#ifndef _SOC_FE3200_DIAG_H_
#define _SOC_FE3200_DIAG_H_




#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_diag.h>



















soc_error_t soc_fe3200_diag_fabric_cell_snake_test_interrupts_name_get(int unit, const soc_dfe_diag_flag_str_t **intr_names);
soc_error_t soc_fe3200_counters_get_info(int unit,soc_dfe_counters_info_t* fe_counters_info);
soc_error_t soc_fe3200_diag_cell_pipe_counter_get(int unit, int pipe, uint64 *counter);
soc_error_t soc_fe3200_queues_get_info(int unit, soc_dfe_queues_info_t* fe_queues_info);
soc_error_t soc_fe3200_diag_mesh_topology_get(int unit, soc_dcmn_fabric_mesh_topology_diag_t *mesh_topology_diag);


#endif
