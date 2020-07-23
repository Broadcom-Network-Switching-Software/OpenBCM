/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* This file contains structures and functions declarations for 
* In-band cell configuration and Source Routed Cell.
 */
#ifndef _SOC_DPP_FABRIC_CELL_H
#define _SOC_DPP_FABRIC_CELL_H

#include <soc/dcmn/dcmn_defs.h>
#include <soc/dcmn/dcmn_fabric_cell.h>

int soc_dpp_cell_filter_set(int unit, uint32 array_size, soc_dcmn_filter_type_t* filter_type_arr, uint32* filter_type_val); 
int soc_dpp_cell_filter_clear(int unit);
int soc_dpp_cell_filter_receive(int unit, dcmn_captured_cell_t* data_out);
int soc_dpp_control_cell_filter_set(int unit, soc_dcmn_control_cell_types_t cell_type, uint32 array_size, soc_dcmn_control_cell_filter_type_t* control_cell_filter_type_arr, uint32* filter_type_val); 
int soc_dpp_control_cell_filter_clear(int unit);
int soc_dpp_control_cell_filter_receive(int unit, soc_dcmn_captured_control_cell_t* data_out);

#endif 
