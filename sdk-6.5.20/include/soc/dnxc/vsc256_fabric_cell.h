/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * VSC256 FABRIC CELL H
 */

#ifndef _SOC_DNXC_VSC256_FABRIC_CELL_H_

#define _SOC_DNXC_VSC256_FABRIC_CELL_H_

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNX (Jr2) and DNXF family only!"
#endif

#include <soc/register.h>
#include <soc/dnxc/fabric.h>
#include <shared/shrextend/shrextend_error.h>

#define DNXC_VSC256_SR_DATA_CELL_HEADER_SIZE                 (11)

#define DNXC_VSC256_SR_DATA_CELL_CELL_TYPE_START          (86)
#define DNXC_VSC256_SR_DATA_CELL_CELL_TYPE_LENGTH         (2  )
#define DNXC_VSC256_SR_DATA_CELL_SOURCE_ID_START          (51)
#define DNXC_VSC256_SR_DATA_CELL_SOURCE_ID_LENGTH         (11 )
#define DNXC_VSC256_SR_DATA_CELL_SRC_LEVEL_START          (48)
#define DNXC_VSC256_SR_DATA_CELL_SRC_LEVEL_LENGTH         (3  )
#define DNXC_VSC256_SR_DATA_CELL_DEST_LEVEL_START         (45)
#define DNXC_VSC256_SR_DATA_CELL_DEST_LEVEL_LENGTH        (3  )
#define DNXC_VSC256_SR_DATA_CELL_FIP_SWITCH_START         (40)
#define DNXC_VSC256_SR_DATA_CELL_FIP_SWITCH_LENGTH        (5  )
#define DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_START         (35)
#define DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_LENGTH        (5  )
#define DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_START         (29)
#define DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_LENGTH        (6  )
#define DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_START         (24)
#define DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_LENGTH        (5  )
#define DNXC_VSC256_SR_DATA_CELL_INBAND_CELL_LENGTH       (1  )
#define DNXC_VSC256_SR_DATA_CELL_INBAND_CELL_POSITION     (19)
#define DNXC_VSC256_SR_DATA_CELL_PIPE_ID_ENABLE_POSITION  (70)
#define DNXC_VSC256_SR_DATA_CELL_PIPE_ID_ENABLE_LENGTH    (1)
#define DNXC_VSC256_SR_DATA_CELL_PIPE_ID_POSITION         (67)
#define DNXC_VSC256_SR_DATA_CELL_PIPE_ID_LENGTH           (3)

#define DNXC_VSC256_SR_DATA_CELL_FIP_SWITCH_START_1    (5  )
#define DNXC_VSC256_SR_DATA_CELL_FIP_SWITCH_LENGTH_1   (1  )
#define DNXC_VSC256_SR_DATA_CELL_FIP_SWITCH_POSITION_1 (23)
#define DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_START_1    (5  )
#define DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_LENGTH_1   (1  )
#define DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_POSITION_1 (22)
#define DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_START_1    (6  )
#define DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_LENGTH_1   (1  )
#define DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_POSITION_1 (21)
#define DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_START_1    (5  )
#define DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_LENGTH_1   (1  )
#define DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_POSITION_1 (20)

#define DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_START_2    (6)
#define DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_LENGTH_2   (2)
#define DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_POSITION_2 (65)
#define DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_START_2    (7)
#define DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_LENGTH_2   (1)
#define DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_POSITION_2 (64)
#define DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_START_2    (6)
#define DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_LENGTH_2   (2)
#define DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_POSITION_2 (62)

#define DNXC_VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32  (32)

#define DNXC_VSC256_MAX_COMMANDS_ARRAY_SIZE 5

typedef struct dnxc_vsc256_sr_cell_header_s
{
    uint32 cell_type;
    int data_in_msb;
    uint32 cell_size;
    int source_device;
    dnxc_fabric_device_type_t source_level;
    dnxc_fabric_device_type_t destination_level;
    int fip_link;
    int fe1_link;
    int fe2_link;
    int fe3_link;
    uint32 is_inband;
    int pipe_id;
    uint32 ack;
} dnxc_vsc256_sr_cell_header_t;

typedef enum soc_dnxc_inband_opcode_e
{
    soc_dnxc_inband_reg_read = 0,
    soc_dnxc_inband_reg_write = 1,
    soc_dnxc_inband_mem_read = 2,
    soc_dnxc_inband_mem_write = 3
} soc_dnxc_inband_opcode_t;

#define MAX_DATA_SIZE 16

typedef struct soc_dnxc_inband_command_s
{
    soc_dnxc_inband_opcode_t opcode;
    int schan_block;
    int length;
    uint32 offset;
    uint32 data[MAX_DATA_SIZE];
} soc_dnxc_inband_command_t;

typedef struct dnxc_vsc256_inband_payload_s
{
    uint8 cell_format;
    uint32 cell_id;
    uint32 seq_num;
    uint32 nof_commands;
    soc_dnxc_inband_command_t commands[DNXC_VSC256_MAX_COMMANDS_ARRAY_SIZE];
} dnxc_vsc256_inband_payload_t;

#define    DNXC_VSC256_COMMAND_DATA_SIZE_U32                  (4)

#define    DNXC_VSC256_CELL_FORMAT_FE600  0
#define    DNXC_VSC256_CELL_FORMAT_FE1600 1

typedef struct dnxc_vsc256_paylod_u
{
    uint32 data[DNXC_VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32];
    dnxc_vsc256_inband_payload_t inband;
} dnxc_vsc256_sr_paylod_t;

typedef struct dnxc_vsc256_sr_cell_s
{
    dnxc_vsc256_sr_cell_header_t header;
    dnxc_vsc256_sr_paylod_t payload;
} dnxc_vsc256_sr_cell_t;

shr_error_e soc_dnxc_vsc256_build_header(
    int unit,
    const dnxc_vsc256_sr_cell_t * cell,
    int buf_size_bytes,
    uint32 *buf);

#endif
