/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * VSC256 FABRIC CELL H
 */

#ifndef _SOC_VSC256_FABRIC_CELL_H_

#define _SOC_VSC256_FABRIC_CELL_H_

#include <soc/register.h>
#include <soc/dcmn/fabric.h>



#define VSC256_SR_DATA_CELL_HEADER_SIZE                 (11)

#define VSC256_SR_DATA_CELL_CELL_TYPE_START          (86)
#define VSC256_SR_DATA_CELL_CELL_TYPE_LENGTH         (2  )
#define VSC256_SR_DATA_CELL_SOURCE_ID_START          (51)
#define VSC256_SR_DATA_CELL_SOURCE_ID_LENGTH         (11 )
#define VSC256_SR_DATA_CELL_SRC_LEVEL_START          (48)
#define VSC256_SR_DATA_CELL_SRC_LEVEL_LENGTH         (3  )
#define VSC256_SR_DATA_CELL_DEST_LEVEL_START         (45)
#define VSC256_SR_DATA_CELL_DEST_LEVEL_LENGTH        (3  )
#define VSC256_SR_DATA_CELL_FIP_SWITCH_START         (40)
#define VSC256_SR_DATA_CELL_FIP_SWITCH_LENGTH        (5  )
#define VSC256_SR_DATA_CELL_FE1_SWITCH_START         (35)
#define VSC256_SR_DATA_CELL_FE1_SWITCH_LENGTH        (5  )
#define VSC256_SR_DATA_CELL_FE2_SWITCH_START         (29)
#define VSC256_SR_DATA_CELL_FE2_SWITCH_LENGTH        (6  )
#define VSC256_SR_DATA_CELL_FE3_SWITCH_START         (24)
#define VSC256_SR_DATA_CELL_FE3_SWITCH_LENGTH        (5  )
#define VSC256_SR_DATA_CELL_INBAND_CELL_LENGTH       (1  )
#define VSC256_SR_DATA_CELL_INBAND_CELL_POSITION     (19)
#define VSC256_SR_DATA_CELL_PIPE_ID_ENABLE_POSITION  (70)
#define VSC256_SR_DATA_CELL_PIPE_ID_ENABLE_LENGTH    (1)
#define VSC256_SR_DATA_CELL_PIPE_ID_POSITION         (67)
#define VSC256_SR_DATA_CELL_PIPE_ID_LENGTH           (3)


#define VSC256_SR_DATA_CELL_FIP_SWITCH_START_1    (5  )
#define VSC256_SR_DATA_CELL_FIP_SWITCH_LENGTH_1   (1  )
#define VSC256_SR_DATA_CELL_FIP_SWITCH_POSITION_1 (23)
#define VSC256_SR_DATA_CELL_FE1_SWITCH_START_1    (5  )
#define VSC256_SR_DATA_CELL_FE1_SWITCH_LENGTH_1   (1  )
#define VSC256_SR_DATA_CELL_FE1_SWITCH_POSITION_1 (22)
#define VSC256_SR_DATA_CELL_FE2_SWITCH_START_1    (6  )
#define VSC256_SR_DATA_CELL_FE2_SWITCH_LENGTH_1   (1  )
#define VSC256_SR_DATA_CELL_FE2_SWITCH_POSITION_1 (21)
#define VSC256_SR_DATA_CELL_FE3_SWITCH_START_1    (5  )
#define VSC256_SR_DATA_CELL_FE3_SWITCH_LENGTH_1   (1  )
#define VSC256_SR_DATA_CELL_FE3_SWITCH_POSITION_1 (20)

#define VSC256_SR_DATA_CELL_FE1_SWITCH_START_2    (6)
#define VSC256_SR_DATA_CELL_FE1_SWITCH_LENGTH_2   (2)
#define VSC256_SR_DATA_CELL_FE1_SWITCH_POSITION_2 (65)
#define VSC256_SR_DATA_CELL_FE2_SWITCH_START_2    (7)
#define VSC256_SR_DATA_CELL_FE2_SWITCH_LENGTH_2   (1)
#define VSC256_SR_DATA_CELL_FE2_SWITCH_POSITION_2 (64)
#define VSC256_SR_DATA_CELL_FE3_SWITCH_START_2    (6)
#define VSC256_SR_DATA_CELL_FE3_SWITCH_LENGTH_2   (2)
#define VSC256_SR_DATA_CELL_FE3_SWITCH_POSITION_2 (62)


#define VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32  (32)


#define VSC256_TO_VSC128_DATA_CELL_CELL_TYPE_START          (86)
#define VSC256_TO_VSC128_DATA_CELL_CELL_TYPE_LENGTH         (2  )
#define VSC256_TO_VSC128_DATA_CELL_SOURCE_ID_START          (67)
#define VSC256_TO_VSC128_DATA_CELL_SOURCE_ID_LENGTH         (11 )
#define VSC256_TO_VSC128_DATA_CELL_SRC_LEVEL_START          (64)
#define VSC256_TO_VSC128_DATA_CELL_SRC_LEVEL_LENGTH         (3  )
#define VSC256_TO_VSC128_DATA_CELL_DEST_LEVEL_START         (61)
#define VSC256_TO_VSC128_DATA_CELL_DEST_LEVEL_LENGTH        (3  )
#define VSC256_TO_VSC128_DATA_CELL_FIP_SWITCH_START         (56)
#define VSC256_TO_VSC128_DATA_CELL_FIP_SWITCH_LENGTH        (5  )
#define VSC256_TO_VSC128_DATA_CELL_FE1_SWITCH_START         (51)
#define VSC256_TO_VSC128_DATA_CELL_FE1_SWITCH_LENGTH        (5  )
#define VSC256_TO_VSC128_DATA_CELL_FE2_SWITCH_START         (45)
#define VSC256_TO_VSC128_DATA_CELL_FE2_SWITCH_LENGTH        (6  )
#define VSC256_TO_VSC128_DATA_CELL_FE3_SWITCH_START         (40)
#define VSC256_TO_VSC128_DATA_CELL_FE3_SWITCH_LENGTH        (5  )
#define VSC256_TO_VSC128_SR_DATA_CELL_FIP_SWITCH_LENGTH     (1  )
#define VSC256_TO_VSC128_SR_DATA_CELL_FIP_SWITCH_POSITION   (39)
#define VSC256_TO_VSC128_SR_DATA_CELL_FE1_SWITCH_LENGTH     (1  )
#define VSC256_TO_VSC128_SR_DATA_CELL_FE1_SWITCH_POSITION   (38)
#define VSC256_TO_VSC128_SR_DATA_CELL_FE2_SWITCH_LENGTH     (1  )
#define VSC256_TO_VSC128_SR_DATA_CELL_FE2_SWITCH_POSITION   (37)
#define VSC256_TO_VSC128_SR_DATA_CELL_FE3_SWITCH_LENGTH     (1  )
#define VSC256_TO_VSC128_SR_DATA_CELL_FE3_SWITCH_POSITION   (36)
#define VSC256_TO_VSC128_SR_DATA_CELL_INBAND_CELL_LENGTH    (1  )
#define VSC256_TO_VSC128_SR_DATA_CELL_INBAND_CELL_POSITION  (35)


#define VSC256_INBAND_PAYLOAD_CELL_FORMAT_START 510
#define VSC256_INBAND_PAYLOAD_CELL_FORMAT_LENGTH 2

#define VSC256_INBAND_PAYLOAD_CELL_ID_START 499
#define VSC256_INBAND_PAYLOAD_CELL_ID_LENGTH 11

#define VSC256_INBAND_PAYLOAD_SEQUENCE_NUMBER_START 487
#define VSC256_INBAND_PAYLOAD_SEQUENCE_NUMBER_LENGTH 12

#define VSC256_INBAND_PAYLOAD_NUMBER_OF_COMMANDS_START 484
#define VSC256_INBAND_PAYLOAD_NUMBER_OF_COMMANDS_LENGTH 3
#define VSC256_INBAND_PAYLOAD_MAX_VALID_COMMANDS 5

#define VSC256_INBAND_PAYLOAD_OPCODE_START 482
#define VSC256_INBAND_PAYLOAD_OPCODE_LENGTH 2

#define VSC256_INBAND_PAYLOAD_BLOCK_START 476
#define VSC256_INBAND_PAYLOAD_BLOCK_LENGTH 6

#define VSC256_INBAND_PAYLOAD_LENGTH_START 474
#define VSC256_INBAND_PAYLOAD_LENGTH_LENGTH 2

#define VSC256_INBAND_PAYLOAD_ADDRESS_START 442
#define VSC256_INBAND_PAYLOAD_ADDRESS_LENGTH 32

#define VSC256_INBAND_PAYLOAD_DATA_START 314


#define VSC256_INBAND_PAYLOAD_DIFF 170


#define VSC256_MAX_COMMANDS_ARRAY_SIZE 5

typedef struct vsc256_sr_cell_header_s {
     uint32                             cell_type;
     uint32                             source_device;
     dcmn_fabric_device_type_t          source_level;
     dcmn_fabric_device_type_t          destination_level;
     int                                fip_link;
     int                                fe1_link;
     int                                fe2_link;
     int                                fe3_link;
     uint32                             is_inband;
     int                                pipe_id;
     uint32                             ack;     
} vsc256_sr_cell_header_t;

typedef enum soc_dcmn_inband_opcode_e 
{
    soc_dcmn_inband_reg_read = 0,
    soc_dcmn_inband_reg_write = 1,
    soc_dcmn_inband_mem_read = 2,
    soc_dcmn_inband_mem_write = 3
}soc_dcmn_inband_opcode_t;

#define MAX_DATA_SIZE 16

typedef struct soc_dcmn_inband_command_s 
{
    soc_dcmn_inband_opcode_t opcode;
    int                     schan_block;
    int                     length; 
    uint32                  offset;
    uint32                  data[MAX_DATA_SIZE];
} soc_dcmn_inband_command_t;

typedef struct vsc256_inband_payload_s {
     uint8                cell_format;
     uint32               cell_id;
     uint32               seq_num;
     uint32               nof_commands;
     soc_dcmn_inband_command_t commands[VSC256_MAX_COMMANDS_ARRAY_SIZE];
} vsc256_inband_payload_t;

#define    VSC256_COMMAND_DATA_SIZE_U32                  (4)

#define    VSC256_CELL_FORMAT_FE600  0
#define    VSC256_CELL_FORMAT_FE1600 1

typedef struct vsc256_paylod_u {
     uint32                  data[VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32];
     vsc256_inband_payload_t inband; 
} vsc256_sr_paylod_t;

typedef struct vsc256_sr_cell_s {
    vsc256_sr_cell_header_t    header;
    vsc256_sr_paylod_t         payload;
} vsc256_sr_cell_t;



soc_error_t soc_vsc256_build_payload(int unit, const vsc256_sr_cell_t* cell, int buf_size_bytes, uint32* buf, int offset);
soc_error_t soc_vsc256_parse_payload(int unit, const uint32 *buf, vsc256_sr_cell_t* cell, int offset);
soc_error_t soc_vsc256_build_header(int unit, const vsc256_sr_cell_t* cell, int buf_size_bytes, uint32* buf);
soc_error_t soc_vsc256_parse_header(int unit, soc_reg_above_64_val_t reg_val, vsc256_sr_cell_t* cell);



soc_error_t soc_vsc256_to_vsc128_build_header(int unit, const vsc256_sr_cell_t* cell, int buf_size_bytes, uint32* buf);

#endif 


