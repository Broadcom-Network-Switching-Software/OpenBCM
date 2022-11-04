/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef SOC_DNXC_FABRIC_CELL_H
#define SOC_DNXC_FABRIC_CELL_H

#include <soc/sand/shrextend/shrextend_error.h>

#include <soc/register.h>
#include <soc/dnxc/dnxc_error.h>
#include <soc/dnxc/fabric.h>
#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#endif
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#endif

#define DNXC_DESTINATION_ROUTED_CELL_HEADER_SIZE_BITS          (120)
#define DNXC_DESTINATION_ROUTED_CELL_HEADER_SIZE_BYTES         (BITS2BYTES(DNXC_DESTINATION_ROUTED_CELL_HEADER_SIZE_BITS))
#define DNXC_DESTINATION_ROUTED_CELL_HEADER_SIZE_UINT32        (BYTES2WORDS(DNXC_DESTINATION_ROUTED_CELL_HEADER_SIZE_BITS))

#define DNXC_FABRIC_CELL_NOF_LINKS_IN_PATH_LINKS (4)
#define DNXC_FABRIC_CELL_SOURCE_ROUTED_CELL_SIZE (65)

#define SOC_DNXC_FABRIC_CELL_ENTRY_MAX_SIZE_UINT32      (80)
#define SOC_DNXC_FABRIC_CAPTURED_CELL_ENTRY_MAX_SIZE_UINT32 (64)

#define SOC_DNXC_FABRIC_CELL_SIZE_MAX 1
#ifdef BCM_DNXF_SUPPORT
#if SOC_DNXC_FABRIC_CELL_SIZE_MAX < DNXF_DATA_MAX_FABRIC_CELL_MAX_VSC_FORMAT_SIZE
#undef SOC_DNXC_FABRIC_CELL_SIZE_MAX
#define SOC_DNXC_FABRIC_CELL_SIZE_MAX DNXF_DATA_MAX_FABRIC_CELL_MAX_VSC_FORMAT_SIZE
#endif
#endif

#ifdef BCM_DNX_SUPPORT
#if SOC_DNXC_FABRIC_CELL_SIZE_MAX < DNX_DATA_MAX_FABRIC_CELL_MAX_VSC_FORMAT_SIZE
#undef SOC_DNXC_FABRIC_CELL_SIZE_MAX
#define SOC_DNXC_FABRIC_CELL_SIZE_MAX DNX_DATA_MAX_FABRIC_CELL_MAX_VSC_FORMAT_SIZE
#endif
#endif

#define DNXC_FTMH_BASE_HEADER_SIZE_BYTES                   (10)
#define DNXC_FTMH_BASE_HEADER_SIZE_WORDS                   (BYTES2WORDS(DNXC_FTMH_BASE_HEADER_SIZE_BYTES))
#define DNXC_FTMH_BASE_HEADER_LB_EXT_MIN_SIZE_BYTES        (3)
#define DNXC_FTMH_HEADER_OFFSET_BYTES                      (DNXC_FTMH_BASE_HEADER_SIZE_BYTES + DNXC_FTMH_BASE_HEADER_LB_EXT_MIN_SIZE_BYTES)

#define DNXC_FTMH_CRC_SIZE_BYTES                           (2)
#define DNXC_FTMH_CRC_SIZE_BITS                            (BYTES2BITS(DNXC_FTMH_CRC_SIZE_BYTES))

#define DNXC_VSC256_DESTINATION_ROUTED_CELL_PAYLOAD_SIZE_BYTES        (256)

#define DNXC_VSC256_SR_DATA_CELL_HEADER_SIZE              (11)

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

#define DNXC_VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32    (32)

typedef enum soc_dnxc_fabric_cell_type_e
{
    soc_dnxc_fabric_cell_type_data_cell = 0,
    soc_dnxc_fabric_cell_type_sr_cell = 1
} soc_dnxc_fabric_cell_type_t;

typedef enum soc_dnxc_fabric_cell_field_e
{
    soc_dnxc_fabric_cell_field_invalid = -1,
    soc_dnxc_fabric_cell_field_cell_type = 0,
    soc_dnxc_fabric_cell_field_src_device,
    soc_dnxc_fabric_cell_field_src_level,
    soc_dnxc_fabric_cell_field_destination,
    soc_dnxc_fabric_cell_field_dest_level,
    soc_dnxc_fabric_cell_field_fip,
    soc_dnxc_fabric_cell_field_fe1,
    soc_dnxc_fabric_cell_field_fe2,
    soc_dnxc_fabric_cell_field_fe3,
    soc_dnxc_fabric_cell_field_is_inband,
    soc_dnxc_fabric_cell_field_ack,
    soc_dnxc_fabric_cell_field_pipe_id,
    soc_dnxc_fabric_cell_field_payload,
    soc_dnxc_fabric_cell_field_data_in_msb,
    soc_dnxc_fabric_cell_field_cell_size,
    soc_dnxc_fabric_cell_field_last_cell_of_packet,
    soc_dnxc_fabric_cell_field_fragment_number,
    soc_dnxc_fabric_cell_field_pcp_type,
    soc_dnxc_fabric_cell_field_nof
} soc_dnxc_fabric_cell_field_t;

typedef enum soc_dnxc_ftmh_base_fields_e
{
    soc_dnxc_ftmh_base_header_field_invalid = -1,
    soc_dnxc_ftmh_base_header_field_visibility,
    soc_dnxc_ftmh_base_header_field_tm_profile,
    soc_dnxc_ftmh_base_header_field_pp_dsp,
    soc_dnxc_ftmh_base_header_field_paket_size,
    soc_dnxc_ftmh_base_header_field_nof
} soc_dnxc_ftmh_base_fields_t;

typedef struct soc_dnxc_fabric_route_s
{
    uint32 pipe_id;
    int number_of_hops;
    int *hop_ids;
    _shr_fabric_device_type_t dest_device_type;
} soc_dnxc_fabric_route_t;

typedef struct dnxc_captured_cells_s
{
    int dest;
    int source_device;
    int is_last_cell;
    int data_in_msb;
    uint32 cell_type;
    uint32 data[SOC_DNXC_FABRIC_CAPTURED_CELL_ENTRY_MAX_SIZE_UINT32];
    uint32 cell_size;
    uint32 prio_int;
    uint32 pipe_index;

} dnxc_captured_cell_t;

typedef struct soc_dnxc_fabric_cell_parse_table_s
{
    uint32 dest_id;
    uint32 *dest;
    uint32 dest_start_bit;
    uint32 src_start_bit;
    uint32 length;
    char *field_name;
} soc_dnxc_fabric_cell_parse_table_t;

typedef uint32 soc_dnxc_fabric_cell_entry_t[SOC_DNXC_FABRIC_CELL_ENTRY_MAX_SIZE_UINT32];

typedef struct dnxc_dest_routed_cell_header_s
{
    uint32 cell_type;
    uint32 dest_modid;
    uint32 source_modid;
    uint32 last_cell_of_paket;
    uint32 fragment_number;
    uint32 pcp_type;
} dnxc_dest_routed_cell_header_t;

typedef struct dnxc_dest_routed_cell_s
{
    dnxc_dest_routed_cell_header_t header;
    uint8 payload[SOC_DNXC_FABRIC_CELL_SIZE_MAX];
    uint32 payload_size;
} dnxc_dest_routed_cell_t;

typedef struct dnxc_sr_cell_header_s
{
    uint32 cell_type;
    int data_in_msb;
    uint32 cell_size;
    int source_device;
    _shr_fabric_device_type_t source_level;
    _shr_fabric_device_type_t destination_level;
    int fip_link;
    int fe1_link;
    int fe2_link;
    int fe3_link;
    uint32 is_inband;
    int pipe_id;
    uint32 ack;
} dnxc_sr_cell_header_t;

typedef struct dnxc_sr_cell_payload_u
{
    uint32 data[DNXC_VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32];
} dnxc_sr_cell_payload_t;

typedef struct dnxc_sr_cell_s
{
    dnxc_sr_cell_header_t header;
    dnxc_sr_cell_payload_t payload;
} dnxc_sr_cell_t;

typedef union soc_dnxc_fabric_cell_u
{
    dnxc_captured_cell_t captured_cell;
    dnxc_sr_cell_t sr_cell;
} soc_dnxc_fabric_cell_t;

typedef struct soc_dnxc_fabric_cell_info_s
{
    soc_dnxc_fabric_cell_type_t cell_type;
    soc_dnxc_fabric_cell_t cell;
} soc_dnxc_fabric_cell_info_t;

shr_error_e soc_dnxc_fabric_cell_table_field_row_get(
    int unit,
    soc_dnxc_fabric_cell_field_t field,
    soc_dnxc_fabric_cell_parse_table_t * table,
    int *row);

shr_error_e soc_dnxc_fabric_cell_parser(
    int unit,
    soc_dnxc_fabric_cell_entry_t entry,
    soc_dnxc_fabric_cell_parse_table_t * parse_table,
    soc_dnxc_fabric_cell_info_t * cell_info);

shr_error_e soc_dnxc_ftmh_base_build_header(
    int unit,
    int pp_destination_port,
    int paket_size,
    int buf_size,
    uint8 *buf);

shr_error_e soc_dnxc_dest_routed_cell_build_header(
    int unit,
    dnxc_dest_routed_cell_t * cell,
    int buf_size_bytes,
    uint32 *buf);

shr_error_e soc_dnxc_vsc256_sr_cell_build_header(
    int unit,
    const dnxc_sr_cell_t * cell,
    int buf_size_bytes,
    uint32 *buf);

#endif
