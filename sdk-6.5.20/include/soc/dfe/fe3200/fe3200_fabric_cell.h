/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE3200 FABRIC CELL H
 */


#ifndef _SOC_FE3200_FABRIC_CELL_H_
#define _SOC_FE3200_FABRIC_CELL_H_





#include <soc/error.h>

#include <soc/dcmn/dcmn_fabric_cell.h>
#include <soc/dcmn/vsc256_fabric_cell.h>

#include <soc/dfe/cmn/dfe_fabric_cell.h>





#define SOC_FE3200_FABRIC_CELL_MATCH_FILTER_REG_SOURCE_ID_START       (114)
#define SOC_FE3200_FABRIC_CELL_MATCH_FILTER_REG_SOURCE_ID_LENGTH      (11)
#define SOC_FE3200_FABRIC_CELL_MATCH_FILTER_REG_DEST_ID_START    (125)
#define SOC_FE3200_FABRIC_CELL_MATCH_FILTER_REG_DEST_ID_LENGTH   (19)
#define SOC_FE3200_FABRIC_CELL_MATCH_FILTER_REG_PRIORITY_TYPE_START   (80)
#define SOC_FE3200_FABRIC_CELL_MATCH_FILTER_REG_PRIORITY_TYPE_LENGTH  (2)
#define SOC_FE3200_FABRIC_CELL_MATCH_FILTER_REG_TRAFFIC_CAST_START    (144)
#define SOC_FE3200_FABRIC_CELL_MATCH_FILTER_REG_TRAFFIC_CAST_LENGTH   (2)





soc_error_t soc_fe3200_cell_filter_clear(int unit);
soc_error_t soc_fe3200_control_cell_filter_set(int unit, uint32 flags, soc_dcmn_control_cell_types_t cell_type, uint32 array_size, soc_dcmn_control_cell_filter_type_t* filter_type_arr, uint32* filter_type_val);
soc_error_t soc_fe3200_control_cell_filter_clear(int unit);
soc_error_t soc_fe3200_control_cell_filter_receive(int unit,  soc_dcmn_captured_control_cell_t* data_out);
soc_error_t soc_fe3200_fabric_cell_get(int unit, soc_dcmn_fabric_cell_entry_t entry);
soc_error_t soc_fe3200_fabric_cell_parse_table_get(int unit, soc_dcmn_fabric_cell_type_t cell_type, uint32 max_nof_lines, soc_dcmn_fabric_cell_parse_table_t *parse_table, uint32 *nof_lines, int is_two_parts);
soc_error_t soc_fe3200_fabric_cell_type_get(int unit, soc_dcmn_fabric_cell_entry_t entry, soc_dcmn_fabric_cell_type_t *cell_type);
soc_error_t soc_fe3200_cell_filter_set(int unit, uint32 flags, uint32 array_size, soc_dcmn_filter_type_t* filter_type_arr, uint32* filter_type_val); 
soc_error_t soc_fe3200_cell_filter_count_get(int unit, int *count);
soc_error_t soc_fe3200_fabric_cell_is_cell_in_two_parts(int unit, soc_dcmn_fabric_cell_entry_t entry, int* is_two_parts);
soc_error_t soc_fe3200_fabric_cell_dcl_cpu_wide_memory_read(int unit, int blk_index, void* entry);



#define SOC_FE3200_FABRIC_CELL_CAPTURED_SRC_FORMAT_NOF_LINES    (12)
#define SOC_FE3200_FABRIC_CELL_CAPTURED_SRC_FORMAT {\
  \
    {soc_dcmn_fabric_cell_dest_id_src_cell_type,    NULL,         0,                1115,           1,      "CELL_TYPE"},\
    {soc_dcmn_fabric_cell_dest_id_src_src_device,   NULL,         0,                1090,           11,     "SOURCE_DEVICE"},\
    {soc_dcmn_fabric_cell_dest_id_src_src_level,    NULL,         0,                1087,           3,      "SOURCE_LEVEL"},\
    {soc_dcmn_fabric_cell_dest_id_src_dest_level,   NULL,         0,                1084,           3,      "DEST_LEVEL"},\
    {soc_dcmn_fabric_cell_dest_id_src_fip,          NULL,         0,                1078,           6,      "FIP"},\
    {soc_dcmn_fabric_cell_dest_id_src_fe1,          NULL,         0,                1070,           8,      "FE1"},\
    {soc_dcmn_fabric_cell_dest_id_src_fe2,          NULL,         0,                1062,           8,      "FE2"},\
    {soc_dcmn_fabric_cell_dest_id_src_fe3,          NULL,         0,                1054,           8,      "FE3"},\
    {soc_dcmn_fabric_cell_dest_id_src_is_inband,    NULL,         0,                1053,           1,      "IS_INBAND"},\
    {soc_dcmn_fabric_cell_dest_id_src_ack,          NULL,         0,                1052,           1,      "ACK"},\
    {soc_dcmn_fabric_cell_dest_id_src_pipe_id,      NULL,         0,                1049,           2,      "PIPE_ID"},\
    {soc_dcmn_fabric_cell_dest_id_src_payload,      NULL,         0,                0,              1024,   "PAYLOAD"}\
    }

#define SOC_FE3200_FABRIC_CELL_CPU_CAPTURED_FORMAT_NOF_LINES_ONE_CELL (5)
#define SOC_FE3200_FABRIC_CELL_CPU_CAPTURED_FORMAT_NOF_LINES_DOUBLE_CELL (6)

#define SOC_FE3200_FABRIC_CELL_CPU_CAPTURED_FORMAT {\
    {soc_dcmn_fabric_cell_dest_id_cap_cell_type,       NULL,          0,                1115,           2,      "CELL_TYPE"},\
    {soc_dcmn_fabric_cell_dest_id_cap_src_device,      NULL,          0,                1083,           11,     "SOURCE_DEVICE"},\
    {soc_dcmn_fabric_cell_dest_id_cap_cell_size,       NULL,          0,                1117,           8,      "CELL_SIZE"},\
    {soc_dcmn_fabric_cell_dest_id_cap_cell_pipe_index, NULL,          0,                1049,           2,      "PIPE_INDEX"},\
    {soc_dcmn_fabric_cell_dest_id_cap_cell_payload,    NULL,          0,                0,              1024,   "PAYLOAD"},\
    {soc_dcmn_fabric_cell_dest_id_cap_cell_payload_2,  NULL,          1024,             0,              1024,   "PAYLOAD_PART2"}\
    }

#endif 

