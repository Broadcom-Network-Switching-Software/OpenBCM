/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SOC_DNXC_FABRIC_CELL_H
#define SOC_DNXC_FABRIC_CELL_H

#include <soc/register.h>

#include <soc/dnxc/dnxc_error.h>
#include <soc/dnxc/vsc256_fabric_cell.h>

#define DNXC_FABRIC_CELL_NOF_LINKS_IN_PATH_LINKS (4)

#define SOC_DNXC_FABRIC_CELL_ENTRY_MAX_SIZE_UINT32      (80)
#define SOC_DNXC_FABRIC_CAPTURED_CELL_ENTRY_MAX_SIZE_UINT32 (64)

#define SOC_DNXC_SR_CELL_FLAG_FE600_FORMAT 0x1

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
    soc_dnxc_fabric_cell_field_nof
} soc_dnxc_fabric_cell_field_t;

typedef struct dnxc_soc_fabric_inband_route_s
{

    uint32 number_of_hops;
    int link_ids[DNXC_FABRIC_CELL_NOF_LINKS_IN_PATH_LINKS];
} dnxc_soc_fabric_inband_route_t;

typedef struct soc_dnxc_fabric_route_s
{
    uint32 pipe_id;
    int number_of_hops;
    int *hop_ids;
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

typedef union soc_dnxc_fabric_cell_u
{
    dnxc_captured_cell_t captured_cell;
    dnxc_vsc256_sr_cell_t sr_cell;
} soc_dnxc_fabric_cell_t;

typedef struct soc_dnxc_fabric_cell_info_s
{
    soc_dnxc_fabric_cell_type_t cell_type;
    soc_dnxc_fabric_cell_t cell;
} soc_dnxc_fabric_cell_info_t;

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

int dnxc_soc_inband_route_set(
    int unit,
    int route_id,
    dnxc_soc_fabric_inband_route_t * route);

int dnxc_soc_inband_route_get(
    int unit,
    int route_id,
    dnxc_soc_fabric_inband_route_t * route);

int dnxc_soc_inband_route_group_set(
    int unit,
    int group_id,
    int flags,
    int route_count,
    int *route_ids);

int dnxc_soc_inband_route_group_get(
    int unit,
    int group_id,
    int flags,
    int route_count_max,
    int *route_count,
    int *route_ids);

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

#endif
