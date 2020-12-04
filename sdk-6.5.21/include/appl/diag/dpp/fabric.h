/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DPP DEFS H
 */
#ifndef _APPL_DIAG_DPP_FABRIC_H_INCLUDED_
#define _APPL_DIAG_DPP_FABRIC_H_INCLUDED_

#include <appl/diag/shell.h>
#include <appl/diag/dcmn/diag.h>

#ifdef BCM_PETRA_SUPPORT

cmd_result_t diag_dpp_fabric_thresholds_threshold_to_str(int unit, bcm_fabric_link_threshold_type_t type, char **type_name);
cmd_result_t diag_dpp_fabric_thresholds_types_parse(int unit, char *th_name, diag_dnx_fabric_link_th_info_t *thresholds_info);
cmd_result_t diag_dpp_fabric_queues_print(int unit);

#endif /*BCM_PETRA_SUPPORT*/

#endif /*_APPL_DIAG_DPP_FABRIC_H_INCLUDED_*/



