/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RAMON FABRIC CELL H
 */


#ifndef _SOC_RAMON_FABRIC_CELL_H_
#define _SOC_RAMON_FABRIC_CELL_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif 





#include <soc/error.h>

#include <soc/dnxc/dnxc_fabric_cell.h>
#include <soc/dnxc/vsc256_fabric_cell.h>

#include <soc/dnxf/cmn/dnxf_fabric_cell.h>





#define SOC_RAMON_FABRIC_CELL_MATCH_FILTER_REG_SOURCE_ID_START       (114)
#define SOC_RAMON_FABRIC_CELL_MATCH_FILTER_REG_SOURCE_ID_LENGTH      (11)
#define SOC_RAMON_FABRIC_CELL_MATCH_FILTER_REG_DEST_ID_START    (125)
#define SOC_RAMON_FABRIC_CELL_MATCH_FILTER_REG_DEST_ID_LENGTH   (19)
#define SOC_RAMON_FABRIC_CELL_MATCH_FILTER_REG_PRIORITY_TYPE_START   (80)
#define SOC_RAMON_FABRIC_CELL_MATCH_FILTER_REG_PRIORITY_TYPE_LENGTH  (2)
#define SOC_RAMON_FABRIC_CELL_MATCH_FILTER_REG_TRAFFIC_CAST_START    (144)
#define SOC_RAMON_FABRIC_CELL_MATCH_FILTER_REG_TRAFFIC_CAST_LENGTH   (2)


#define RAMON_VSC256_CONTROL_CELL_FSM_DEST_PORT_START  0
#define RAMON_VSC256_CONTROL_CELL_FSM_DEST_PORT_LENGTH  8
#define RAMON_VSC256_CONTROL_CELL_FSM_SRC_QUEUE_NUM_START  16
#define RAMON_VSC256_CONTROL_CELL_FSM_SRC_QUEUE_NUM_LENGTH  20
#define RAMON_VSC256_CONTROL_CELL_FSM_FLOW_ID_START  36
#define RAMON_VSC256_CONTROL_CELL_FSM_FLOW_ID_LENGTH  20


#define RAMON_VSC256_CONTROL_CELL_CREDIT_DEST_Q_NUM_START  16
#define RAMON_VSC256_CONTROL_CELL_CREDIT_DEST_Q_NUM_LENGTH  20
#define RAMON_VSC256_CONTROL_CELL_CREDIT_SUB_FLOW_ID_START  36
#define RAMON_VSC256_CONTROL_CELL_CREDIT_SUB_FLOW_ID_LENGTH  2
#define RAMON_VSC256_CONTROL_CELL_CREDIT_FLOW_ID_START  38
#define RAMON_VSC256_CONTROL_CELL_CREDIT_FLOW_ID_LENGTH  18


#define RAMON_VSC256_CONTROL_CELL_REACHABILITY_BITMAP_START  0
#define RAMON_VSC256_CONTROL_CELL_REACHABILITY_BITMAP_LENGTH  32
#define RAMON_VSC256_CONTROL_CELL_REACHABILITY_BASE_ID_START  64
#define RAMON_VSC256_CONTROL_CELL_REACHABILITY_BASE_ID_LENGTH  6
#define RAMON_VSC256_CONTROL_CELL_REACHABILITY_SRC_LINK_NUM_START  79
#define RAMON_VSC256_CONTROL_CELL_REACHABILITY_SRC_LINK_NUM_LENGTH  8

#define RAMON_VSC256_CONTROL_CELL_SOURCE_ID_START  85
#define RAMON_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH  11
#define RAMON_VSC256_CONTROL_CELL_DEST_DEVICE_START  96
#define RAMON_VSC256_CONTROL_CELL_DEST_DEVICE_LENGTH  11
#define RAMON_VSC256_CONTROL_CELL_CONTROL_TYPE_START  107
#define RAMON_VSC256_CONTROL_CELL_CONTROL_TYPE_LENGTH  3
#define RAMON_VSC256_CONTROL_CELL_REACHABILITY_SOURCE_ID_START  93

#define RAMON_DATA_CELL_BYTE_SIZE                       (79)

#define RAMON_SR_DATA_CELL_PAYLOAD_START         (0)
#define RAMON_SR_DATA_CELL_PAYLOAD_LENGTH        (512)





shr_error_e soc_ramon_sr_cell_send(int unit, const dnxc_vsc256_sr_cell_t* cell);
shr_error_e soc_ramon_cell_filter_init(int unit);
shr_error_e soc_ramon_cell_filter_clear(int unit);
shr_error_e soc_ramon_control_cell_filter_set(int unit, uint32 flags, soc_dnxc_control_cell_types_t cell_type, uint32 array_size, soc_dnxc_control_cell_filter_type_t* filter_type_arr, uint32* filter_type_val);
shr_error_e soc_ramon_control_cell_filter_clear(int unit);
shr_error_e soc_ramon_control_cell_filter_receive(int unit,  soc_dnxc_captured_control_cell_t* data_out);
shr_error_e soc_ramon_fabric_cell_get(int unit, soc_dnxc_fabric_cell_entry_t entry);
shr_error_e soc_ramon_fabric_cell_parse_table_get(int unit, soc_dnxc_fabric_cell_entry_t entry, soc_dnxc_fabric_cell_type_t *cell_type, soc_dnxc_fabric_cell_parse_table_t *parse_table);
shr_error_e soc_ramon_cell_filter_set(int unit, uint32 flags, uint32 array_size, soc_dnxc_filter_type_t* filter_type_arr, uint32* filter_type_val); 
shr_error_e soc_ramon_cell_filter_count_get(int unit, int *count);


#define SOC_RAMON_FABRIC_CAPTURED_CELL_DATA_IN_MSB_OFFSET         (1024)
#define SOC_RAMON_FABRIC_CAPTURED_CELL_SMALL_CELL_MAX_SIZE        (128)
#define SOC_RAMON_FABRIC_CAPTURED_CELL_BIG_CELL_MAX_SIZE          (256)
#define SOC_RAMON_FABRIC_CAPTURED_CELL_BIG_CELL_MAX_SIZE_BITS     (256)

#define SOC_RAMON_FABRIC_CAPTURED_CELL_SIZE_START                 (2157)
#define SOC_RAMON_FABRIC_CAPTURED_CELL_SIZE_LENGTH                (8)
#define SOC_RAMON_FABRIC_CAPTURED_CELL_IS_SMALL(cell_size) \
    ((cell_size) < SOC_RAMON_FABRIC_CAPTURED_CELL_SMALL_CELL_MAX_SIZE)


#define SOC_RAMON_FABRIC_CAPTURED_CELL_COMMON_FORMAT \
    {soc_dnxc_fabric_cell_field_data_in_msb,     NULL,          0,                2169,           1,      "DATA_IN_MSB"},\
    {soc_dnxc_fabric_cell_field_cell_size,       NULL,          0,                2157,           8,      "CELL_SIZE"},\
    {soc_dnxc_fabric_cell_field_cell_type,       NULL,          0,                2155,           2,      "CELL_TYPE"},\
     \
    {soc_dnxc_fabric_cell_field_payload,         NULL,          0,                0,              2048,   "PAYLOAD"}



#define SOC_RAMON_FABRIC_CELL_CAPTURED_SR_CELL_FORMAT_TABLE {\
  \
    SOC_RAMON_FABRIC_CAPTURED_CELL_COMMON_FORMAT, \
    {soc_dnxc_fabric_cell_field_pipe_id,      NULL,         0,                2141,           3,      "PIPE_ID"},\
    {soc_dnxc_fabric_cell_field_src_device,   NULL,         0,                2130,           11,     "SOURCE_DEVICE"},\
    {soc_dnxc_fabric_cell_field_src_level,    NULL,         0,                2127,           3,      "SOURCE_LEVEL"},\
    {soc_dnxc_fabric_cell_field_dest_level,   NULL,         0,                2124,           3,      "DEST_LEVEL"},\
    {soc_dnxc_fabric_cell_field_fip,          NULL,         0,                2118,           6,      "FIP"},\
    {soc_dnxc_fabric_cell_field_fe1,          NULL,         0,                2110,           8,      "FE1"},\
    {soc_dnxc_fabric_cell_field_fe2,          NULL,         0,                2102,           8,      "FE2"},\
    {soc_dnxc_fabric_cell_field_fe3,          NULL,         0,                2094,           8,      "FE3"},\
    {soc_dnxc_fabric_cell_field_is_inband,    NULL,         0,                2093,           1,      "IS_INBAND"},\
    {soc_dnxc_fabric_cell_field_ack,          NULL,         0,                2092,           1,      "ACK"},\
     \
    {soc_dnxc_fabric_cell_field_invalid,    NULL,         0,                -1,            -1,      "INVALID"}\
    }


#define SOC_RAMON_FABRIC_CELL_CAPTURED_DATA_CELL_FORMAT_TABLE { \
    \
    SOC_RAMON_FABRIC_CAPTURED_CELL_COMMON_FORMAT, \
    {soc_dnxc_fabric_cell_field_pipe_id,                NULL,          0,                2166,           2,      "PIPE_INDEX"},\
    {soc_dnxc_fabric_cell_field_src_device,             NULL,          0,                2123,           11,     "SOURCE_DEVICE"},\
     \
    {soc_dnxc_fabric_cell_field_invalid,                NULL,          0,                -1,            -1,      "INVALID"}\
    }

#endif 

