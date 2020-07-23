/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE1600 DEFS H
 */
#ifndef _APPL_DIAG_DFE_FABRIC_H_INCLUDED_
#define _APPL_DIAG_DFE_FABRIC_H_INCLUDED_

#include <appl/diag/shell.h>

#include <appl/diag/dcmn/diag.h>

#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_diag.h>
#include <soc/dfe/cmn/dfe_fabric_cell_snake_test.h>

cmd_result_t diag_dfe_fabric_counters_print(int unit);
cmd_result_t diag_dfe_fabric_queues_print(int unit);
cmd_result_t diag_dfe_fabric_thresholds_types_parse(int unit, char *th_name, diag_dnx_fabric_link_th_info_t *thresholds_info);
cmd_result_t diag_dfe_fabric_thresholds_supported_get(int unit, bcm_port_t port, bcm_fabric_link_threshold_type_t type, int *supported);
cmd_result_t diag_dfe_fabric_thresholds_threshold_to_str(int unit, bcm_fabric_link_threshold_type_t type, char **type_name);

void diag_dfe_fabric_cell_snake_test_result_print( int unit,  soc_fabric_cell_snake_test_results_t* results);
#endif /*BCM_DFE_SUPPORT*/

#endif /*!_APPL_DIAG_DFE_FABRIC_H_INCLUDED_*/


