/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE1600 FABRIC CELL H
 */
#include <soc/dcmn/vsc256_fabric_cell.h>
#include <soc/dcmn/dcmn_fabric_cell.h>
#include <soc/dfe/cmn/dfe_fabric_cell.h>

#ifndef _SOC_FE1600_FABRIC_CELL_H_

#define _SOC_FE1600_FABRIC_CELL_H_



#define FE1600_DATA_CELL_BYTE_SIZE                       (79)

#define FE1600_SR_DATA_CELL_PAYLOAD_START         (0)
#define FE1600_SR_DATA_CELL_PAYLOAD_LENGTH        (512)
#define FE1600_SR_DATA_CELL_PAYLOAD_LENGTH_U32    (16)
#define FE1600_SR_DATA_CELL_HEADER_START          (512)

#define FE1600_VSC128_DATA_CELL_BYTE_SIZE                (75)
#define FE1600_VSC128_DATA_CELL_CELL_TYPE_START          (256 + 24 + 318)
#define FE1600_VSC128_DATA_CELL_CELL_TYPE_LENGTH         (2  )
#define FE1600_VSC128_DATA_CELL_SOURCE_ID_START          (256 + 24 + 299)
#define FE1600_VSC128_DATA_CELL_SOURCE_ID_LENGTH         (11 )
#define FE1600_VSC128_DATA_CELL_SRC_LEVEL_START          (256 + 24 + 296)
#define FE1600_VSC128_DATA_CELL_SRC_LEVEL_LENGTH         (3  )
#define FE1600_VSC128_DATA_CELL_DEST_LEVEL_START         (256 + 24 + 293)
#define FE1600_VSC128_DATA_CELL_DEST_LEVEL_LENGTH        (3  )
#define FE1600_VSC128_DATA_CELL_FIP_SWITCH_START         (256 + 24 + 288)
#define FE1600_VSC128_DATA_CELL_FIP_SWITCH_LENGTH        (5  )
#define FE1600_VSC128_DATA_CELL_FE1_SWITCH_START         (256 + 24 + 283)
#define FE1600_VSC128_DATA_CELL_FE1_SWITCH_LENGTH        (5  )
#define FE1600_VSC128_DATA_CELL_FE2_SWITCH_START         (256 + 24 + 277)
#define FE1600_VSC128_DATA_CELL_FE2_SWITCH_LENGTH        (6  )
#define FE1600_VSC128_DATA_CELL_FE3_SWITCH_START         (256 + 24 + 272)
#define FE1600_VSC128_DATA_CELL_FE3_SWITCH_LENGTH        (5  )
#define FE1600_VSC128_SR_DATA_CELL_FIP_SWITCH_START      (5  )
#define FE1600_VSC128_SR_DATA_CELL_FIP_SWITCH_LENGTH     (1  )
#define FE1600_VSC128_SR_DATA_CELL_FIP_SWITCH_POSITION   (256 + 24 + 271)
#define FE1600_VSC128_SR_DATA_CELL_FE1_SWITCH_START      (5  )
#define FE1600_VSC128_SR_DATA_CELL_FE1_SWITCH_LENGTH     (1  )
#define FE1600_VSC128_SR_DATA_CELL_FE1_SWITCH_POSITION   (256 + 24 + 270)
#define FE1600_VSC128_SR_DATA_CELL_FE2_SWITCH_START      (6  )
#define FE1600_VSC128_SR_DATA_CELL_FE2_SWITCH_LENGTH     (1  )
#define FE1600_VSC128_SR_DATA_CELL_FE2_SWITCH_POSITION   (256 + 24 + 269)
#define FE1600_VSC128_SR_DATA_CELL_FE3_SWITCH_START      (5  )
#define FE1600_VSC128_SR_DATA_CELL_FE3_SWITCH_LENGTH     (1  )
#define FE1600_VSC128_SR_DATA_CELL_FE3_SWITCH_POSITION   (256 + 24 + 268)
#define FE1600_VSC128_SR_DATA_CELL_INBAND_CELL_LENGTH    (1  )
#define FE1600_VSC128_SR_DATA_CELL_INBAND_CELL_POSITION  (256 + 24 + 267)
#define FE1600_VSC128_SR_DATA_CELL_ACK_LENGTH            (1  )
#define FE1600_VSC128_SR_DATA_CELL_ACK_POSITION          (256 + 24 + 266)
#define FE1600_VSC128_SR_DATA_CELL_INDIRECT_LENGTH       (1  )

#define FE1600_VSC128_SR_DATA_CELL_PAYLOAD_START         (0)
#define FE1600_VSC128_SR_DATA_CELL_PAYLOAD_LENGTH        (512)
#define FE1600_VSC128_SR_DATA_CELL_PAYLOAD_LENGTH_U32    (8)

#define FE1600_CELL_NOF_CELL_IDENTS                    (0x1ff)


#define FE1600_VSC256_CONTROL_CELL_FSM_DEST_PORT_START  0
#define FE1600_VSC256_CONTROL_CELL_FSM_DEST_PORT_LENGTH  8
#define FE1600_VSC256_CONTROL_CELL_FSM_SRC_QUEUE_NUM_START  16
#define FE1600_VSC256_CONTROL_CELL_FSM_SRC_QUEUE_NUM_LENGTH  20
#define FE1600_VSC256_CONTROL_CELL_FSM_FLOW_ID_START  36
#define FE1600_VSC256_CONTROL_CELL_FSM_FLOW_ID_LENGTH  20


#define FE1600_VSC256_CONTROL_CELL_CREDIT_DEST_Q_NUM_START  16
#define FE1600_VSC256_CONTROL_CELL_CREDIT_DEST_Q_NUM_LENGTH  20
#define FE1600_VSC256_CONTROL_CELL_CREDIT_SUB_FLOW_ID_START  36
#define FE1600_VSC256_CONTROL_CELL_CREDIT_SUB_FLOW_ID_LENGTH  2
#define FE1600_VSC256_CONTROL_CELL_CREDIT_FLOW_ID_START  38
#define FE1600_VSC256_CONTROL_CELL_CREDIT_FLOW_ID_LENGTH  18


#define FE1600_VSC256_CONTROL_CELL_REACHABILITY_BITMAP_START  0
#define FE1600_VSC256_CONTROL_CELL_REACHABILITY_BITMAP_LENGTH  32
#define FE1600_VSC256_CONTROL_CELL_REACHABILITY_BASE_ID_START  64
#define FE1600_VSC256_CONTROL_CELL_REACHABILITY_BASE_ID_LENGTH  6
#define FE1600_VSC256_CONTROL_CELL_REACHABILITY_SRC_LINK_NUM_START  79
#define FE1600_VSC256_CONTROL_CELL_REACHABILITY_SRC_LINK_NUM_LENGTH  8

#define FE1600_VSC256_CONTROL_CELL_SOURCE_ID_START  85
#define FE1600_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH  11
#define FE1600_VSC256_CONTROL_CELL_DEST_DEVICE_START  96
#define FE1600_VSC256_CONTROL_CELL_DEST_DEVICE_LENGTH  11
#define FE1600_VSC256_CONTROL_CELL_CONTROL_TYPE_START  107
#define FE1600_VSC256_CONTROL_CELL_CONTROL_TYPE_LENGTH  3
#define FE1600_VSC256_CONTROL_CELL_REACHABILITY_SOURCE_ID_START  93


#define FE1600_INBAND_PAYLOAD_CELL_OFFSET             (0)

#define FE1600_INBAND_PAYLOAD_FE600_CELL_FORMAT_START 256
#define FE1600_INBAND_PAYLOAD_FE600_CELL_FORMAT_LENGTH 2
#define FE1600_INBAND_PAYLOAD_FE600_CELL_FORMAT_VAL 0


#define FE600_INDIRECT_ACCESS_START 1
#define FE600_INDIRECT_ACCESS_LENGTH 1

#define FE600_WRITE_READ_N_START 0
#define FE600_WRITE_READ_N_LENGTH 1

#include <soc/dfe/cmn/dfe_fabric_source_routed_cell.h>

#define FE600_VSC128_INBAND_CELL_NOF_COMMANDS 4
#define FE1600_VSC128_INBAND_CELL_NOF_COMMANDS 1
#define FE1600_VSC256_INBAND_CELL_NOF_COMMANDS 2
#define FE1600_MAX_COMMANDS_ARRAY_SIZE 4

soc_error_t soc_fe1600_fabric_cell_get(int unit, soc_dcmn_fabric_cell_entry_t entry);
soc_error_t soc_fe1600_fabric_cell_type_get(int unit, soc_dcmn_fabric_cell_entry_t entry, soc_dcmn_fabric_cell_type_t *cell_type);
soc_error_t soc_fe1600_fabric_cell_parse_table_get(int unit, soc_dcmn_fabric_cell_type_t cell_type, uint32 max_nof_lines, soc_dcmn_fabric_cell_parse_table_t *parse_table, uint32 *nof_lines, int is_two_parts);
soc_error_t soc_fe1600_sr_cell_send(int unit, const vsc256_sr_cell_t* cell);
soc_error_t soc_fe1600_fabric_cell_is_cell_in_two_parts(int unit, soc_dcmn_fabric_cell_entry_t entry, int* is_two_parts);



soc_error_t soc_fe1600_cell_filter_set(int unit, uint32 flags, uint32 array_size, soc_dcmn_filter_type_t* filter_type_arr, uint32* filter_type_val);
soc_error_t soc_fe1600_cell_filter_clear(int unit);
soc_error_t soc_fe1600_cell_filter_count_get(int unit, int *count);

soc_error_t soc_fe1600_control_cell_filter_set(int unit, uint32 flags, soc_dcmn_control_cell_types_t cell_type, uint32 array_size, soc_dcmn_control_cell_filter_type_t* filter_type_arr, uint32* filter_type_val);
soc_error_t soc_fe1600_control_cell_filter_clear(int unit);
soc_error_t soc_fe1600_control_cell_filter_receive(int unit, soc_dcmn_captured_control_cell_t* data_out);




#define SOC_FE1600_FABRIC_CELL_CAPTURED_SRC_FORMAT_NOF_LINES    (15)
#define SOC_FE1600_FABRIC_CELL_CAPTURED_SRC_FORMAT {\
  \
    {soc_dcmn_fabric_cell_dest_id_src_cell_type,    NULL,         0,                512+63,         2,      "CELL_TYPE"},\
    {soc_dcmn_fabric_cell_dest_id_src_src_device,   NULL,         0,                512+35,         11,     "SOURCE_DEVICE"},\
    {soc_dcmn_fabric_cell_dest_id_src_src_level,    NULL,         0,                512+32,         3,      "SOURCE_LEVEL"},\
    {soc_dcmn_fabric_cell_dest_id_src_dest_level,   NULL,         0,                512+29,         3,      "DEST_LEVEL"},\
    {soc_dcmn_fabric_cell_dest_id_src_fip,          NULL,         0,                512+24,         5,      "FIP[0:4]"},\
    {soc_dcmn_fabric_cell_dest_id_src_fip,          NULL,         5,                512+7,          1,      "FIP[5]"},\
    {soc_dcmn_fabric_cell_dest_id_src_fe1,          NULL,         0,                512+19,         5,      "FE1[0:4]"},\
    {soc_dcmn_fabric_cell_dest_id_src_fe1,          NULL,         5,                512+6,          1,      "FE1[5]"},\
    {soc_dcmn_fabric_cell_dest_id_src_fe2,          NULL,         0,                512+13,         6,      "FE2[0:5]"},\
    {soc_dcmn_fabric_cell_dest_id_src_fe2,          NULL,         6,                512+5,          1,      "FE2[6]"},\
    {soc_dcmn_fabric_cell_dest_id_src_fe3,          NULL,         0,                512+8,          5,      "FE3[0:4]"},\
    {soc_dcmn_fabric_cell_dest_id_src_fe3,          NULL,         5,                512+4,          1,      "FE3[5]"},\
    {soc_dcmn_fabric_cell_dest_id_src_is_inband,    NULL,         0,                512+3,          1,      "IS_INBAND"},\
    {soc_dcmn_fabric_cell_dest_id_src_ack,          NULL,         0,                512+2,          1,      "ACK"},\
    {soc_dcmn_fabric_cell_dest_id_src_payload,      NULL,         0,                0,              512,    "PAYLOAD"}\
}



#define SOC_FE1600_FABRIC_CELL_CAPTURED_CELL_FORMAT_NOF_LINES    (4)
#define SOC_FE1600_FABRIC_CELL_CAPTURED_CELL_FORMAT {\
  \
    {soc_dcmn_fabric_cell_dest_id_cap_cell_type,    NULL,         0,                512+63,         2,      "CELL_TYPE"},\
    {soc_dcmn_fabric_cell_dest_id_cap_src_device,   NULL,         0,                544,            11,     "SOURCE_DEVICE"},\
    {soc_dcmn_fabric_cell_dest_id_cap_cell_dst,     NULL,         0,                555,            19,     "CELL_DEST"},\
    {soc_dcmn_fabric_cell_dest_id_cap_cell_ind,     NULL,         0,                576,            1,      "CELL_IND"},\
}

#endif 

