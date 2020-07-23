/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SOC_DCMN_FABRIC_CELL_H
#define SOC_DCMN_FABRIC_CELL_H

#include <soc/register.h>

#include <soc/dcmn/dcmn_error.h>
#include <soc/dcmn/vsc256_fabric_cell.h>

#define FABRIC_CELL_NOF_LINKS_IN_PATH_LINKS (4)

#define SOC_DCMN_FABRIC_CELL_ENTRY_MAX_SIZE_UINT32      (40)
#define SOC_DCMN_FABRIC_CELL_PARSE_TABLE_MAX_NOF_LINES  (30)
#define SOC_DCMN_FABRIC_CAPTURED_CELL_ENTRY_MAX_SIZE_UINT32 (64)


#define SOC_DCMN_SR_CELL_FLAG_FE600_FORMAT 0x1

typedef enum soc_dcmn_fabric_cell_type_e
{
    soc_dcmn_fabric_cell_type_dara_cell = 0,
    soc_dcmn_fabric_cell_type_sr_cell = 1
} soc_dcmn_fabric_cell_type_t;

typedef enum soc_dcmn_fabric_cell_dest_id_e
{
    soc_dcmn_fabric_cell_dest_id_src_cell_type = 0,
    soc_dcmn_fabric_cell_dest_id_src_src_device = 1,
    soc_dcmn_fabric_cell_dest_id_src_src_level = 2,
    soc_dcmn_fabric_cell_dest_id_src_dest_level = 3,
    soc_dcmn_fabric_cell_dest_id_src_fip = 4,
    soc_dcmn_fabric_cell_dest_id_src_fe1 = 5,
    soc_dcmn_fabric_cell_dest_id_src_fe2 = 6,
    soc_dcmn_fabric_cell_dest_id_src_fe3 = 7,
    soc_dcmn_fabric_cell_dest_id_src_is_inband = 8,
    soc_dcmn_fabric_cell_dest_id_src_ack = 9,
    soc_dcmn_fabric_cell_dest_id_src_pipe_id = 10,
    soc_dcmn_fabric_cell_dest_id_src_payload = 11,
    soc_dcmn_fabric_cell_dest_id_cap_cell_type = 12,
    soc_dcmn_fabric_cell_dest_id_cap_src_device = 13,
    soc_dcmn_fabric_cell_dest_id_cap_cell_dst = 14,
    soc_dcmn_fabric_cell_dest_id_cap_cell_ind = 15,
    soc_dcmn_fabric_cell_dest_id_cap_cell_size = 16,
    soc_dcmn_fabric_cell_dest_id_cap_cell_pipe_index = 18,
    soc_dcmn_fabric_cell_dest_id_cap_cell_payload = 19,
    soc_dcmn_fabric_cell_dest_id_cap_cell_payload_2 = 20
} soc_dcmn_fabric_cell_dest_id_t;

typedef struct soc_fabric_inband_route_s{
    
    uint32  number_of_hops;
    int	  link_ids[FABRIC_CELL_NOF_LINKS_IN_PATH_LINKS];
} soc_fabric_inband_route_t; 

typedef struct soc_dcmn_fabric_route_s{
    uint32 pipe_id;     
    int number_of_hops; 
    int *hop_ids;       
} soc_dcmn_fabric_route_t; 



typedef struct dcmn_captured_cells_s {
    int dest;
    int source_device;
    int is_last_cell;
    uint32 cell_type;
    uint32 data[SOC_DCMN_FABRIC_CAPTURED_CELL_ENTRY_MAX_SIZE_UINT32];            
    uint32 cell_size;                                                     
    uint32 prio_int;
    uint32 pipe_index;

} dcmn_captured_cell_t;

typedef union soc_dcmn_fabric_cell_u
{
    dcmn_captured_cell_t captured_cell;
    vsc256_sr_cell_t     sr_cell;
} soc_dcmn_fabric_cell_t;

typedef struct soc_dcmn_fabric_cell_info_s
{
    soc_dcmn_fabric_cell_type_t cell_type;
    soc_dcmn_fabric_cell_t      cell;
} soc_dcmn_fabric_cell_info_t;


typedef struct soc_dcmn_fabric_cell_parse_table_s
{
    uint32 dest_id;             
    uint32 *dest;               
    uint32 dest_start_bit;
    uint32 src_start_bit;
    uint32 length;
    char   *field_name;
} soc_dcmn_fabric_cell_parse_table_t;

typedef uint32 soc_dcmn_fabric_cell_entry_t[SOC_DCMN_FABRIC_CELL_ENTRY_MAX_SIZE_UINT32];

int
soc_inband_route_set(
                     int unit,
                     int route_id,
                     soc_fabric_inband_route_t* route
);

int
soc_inband_route_get(
                     int unit,
                     int route_id,
                     soc_fabric_inband_route_t* route
);

int
soc_inband_route_group_set(
                           int unit,
                           int group_id,
                           int flags,
                           int route_count,
                           int *route_ids
);

int
soc_inband_route_group_get(
                           int unit,
                           int group_id,
                           int flags,
                           int route_count_max,
                           int *route_count,
                           int *route_ids
);

int
soc_send_sr_cell(
                            int unit, 
                            uint32 flags,
                            soc_fabric_inband_route_t* route,
                            uint32 data_in_size, 
                            uint32 *data_in
);

int
soc_receive_sr_cell(
                               int unit, 
                               uint32 flags,
                               uint32  data_out_max_size,
                               uint32 *data_out_size,
                               uint32 *data_out
);

int 
soc_inband_mem_read(
                    int unit, 
                    uint32 flags, 
                    int route_id, 
                    int max_count,  
                    soc_mem_t *mem, 
                    int *copyno,
                    int *index,
                    void **entry_data, 
                    int *array_count
);

int
soc_inband_mem_write(
                         int unit, 
                         uint32 flags, 
                         int route_id, 
                         int array_count,
                         soc_mem_t *mem, 
                         int *copyno,
                         int *index,
                         void **entry_data 
);

int 
soc_inband_reg_read(
                    int unit, 
                    uint32 flags, 
                    int route_id,
                    int max_count, 
                    soc_reg_t *reg, 
                    int *port,
                    int *index,
                    uint64 *data, 
                    int *array_count
);

int 
soc_inband_reg_write(
                     int unit, 
                     uint32 flags, 
                     int route_id,
                     int array_count, 
                     soc_reg_t *reg, 
                     int *port,
                     int *index,
                     uint64 *data
);

int
soc_inband_reg_above_64_read(
                             int unit, uint32 flags,
                             int route_id,
                             int max_count,
                             soc_reg_t *reg,
                             int *port,
                             int *index,
                             soc_reg_above_64_val_t *data,
                             int *array_count
);

int
soc_inband_reg_above_64_write(
                                  int unit, uint32 flags,
                                  int route_id, int array_count,
                                  soc_reg_t *reg,
                                  int *port,
                                  int *index,
                                  soc_reg_above_64_val_t *data
);

soc_error_t 
soc_dcmn_parse_captured_cell(int unit, soc_reg_above_64_val_t reg_val, dcmn_captured_cell_t *captured_cell);



soc_error_t soc_dcmn_fabric_cell_parser(int unit, soc_dcmn_fabric_cell_entry_t entry, soc_dcmn_fabric_cell_entry_t entry_2 , soc_dcmn_fabric_cell_parse_table_t *parse_table, uint32 nof_lines, soc_dcmn_fabric_cell_info_t *cell_info, int is_two_parts);

#endif 
