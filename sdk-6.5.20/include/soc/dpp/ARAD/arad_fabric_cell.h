/*
* 
*
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* ARAD FABRIC CELL H
*/

#ifndef _SOC_ARAD_FABRIC_CELL_H_

#define _SOC_ARAD_FABRIC_CELL_H_


#define ARAD_VSC256_CONTROL_CELL_FSM_DEST_PORT_START  0
#define ARAD_VSC256_CONTROL_CELL_FSM_DEST_PORT_LENGTH  8
#define ARAD_VSC256_CONTROL_CELL_FSM_SRC_QUEUE_NUM_START  16
#define ARAD_VSC256_CONTROL_CELL_FSM_SRC_QUEUE_NUM_LENGTH  20
#define ARAD_VSC256_CONTROL_CELL_FSM_FLOW_ID_START  36
#define ARAD_VSC256_CONTROL_CELL_FSM_FLOW_ID_LENGTH  20


#define ARAD_VSC256_CONTROL_CELL_CREDIT_DEST_Q_NUM_START  16
#define ARAD_VSC256_CONTROL_CELL_CREDIT_DEST_Q_NUM_LENGTH  20
#define ARAD_VSC256_CONTROL_CELL_CREDIT_SUB_FLOW_ID_START  36
#define ARAD_VSC256_CONTROL_CELL_CREDIT_SUB_FLOW_ID_LENGTH  2
#define ARAD_VSC256_CONTROL_CELL_CREDIT_FLOW_ID_START  38
#define ARAD_VSC256_CONTROL_CELL_CREDIT_FLOW_ID_LENGTH  18


#define ARAD_VSC256_CONTROL_CELL_REACHABILITY_BITMAP_START  0
#define ARAD_VSC256_CONTROL_CELL_REACHABILITY_BITMAP_LENGTH  32
#define ARAD_VSC256_CONTROL_CELL_REACHABILITY_BASE_ID_START  64
#define ARAD_VSC256_CONTROL_CELL_REACHABILITY_BASE_ID_LENGTH  6
#define ARAD_VSC256_CONTROL_CELL_REACHABILITY_SRC_LINK_NUM_START  79
#define ARAD_VSC256_CONTROL_CELL_REACHABILITY_SRC_LINK_NUM_LENGTH  8

#define ARAD_VSC256_CONTROL_CELL_SOURCE_ID_START  85
#define ARAD_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH  11
#define ARAD_VSC256_CONTROL_CELL_DEST_DEVICE_START  96
#define ARAD_VSC256_CONTROL_CELL_DEST_DEVICE_LENGTH  11
#define ARAD_VSC256_CONTROL_CELL_CONTROL_TYPE_START  107
#define ARAD_VSC256_CONTROL_CELL_CONTROL_TYPE_LENGTH  3
#define ARAD_VSC256_CONTROL_CELL_REACHABILITY_SOURCE_ID_START  93

#define SR_CELL_TYPE 01

#include <soc/dcmn/dcmn_defs.h>
#include <soc/dcmn/dcmn_fabric_cell.h>
#include <soc/dpp/dpp_fabric_cell.h>

soc_error_t soc_arad_cell_filter_set(int unit, uint32 array_size, soc_dcmn_filter_type_t* filter_type_arr, uint32* filter_type_val);
soc_error_t soc_arad_cell_filter_clear(int unit);
soc_error_t soc_arad_cell_filter_receive(int unit,  dcmn_captured_cell_t* data_out);
soc_error_t soc_arad_control_cell_filter_set(int unit, soc_dcmn_control_cell_types_t cell_type, uint32 array_size, soc_dcmn_control_cell_filter_type_t* filter_type_arr, uint32* filter_type_val);
soc_error_t soc_arad_control_cell_filter_clear(int unit);
soc_error_t soc_arad_control_cell_filter_receive(int unit,  soc_dcmn_captured_control_cell_t* data_out);

#endif 
