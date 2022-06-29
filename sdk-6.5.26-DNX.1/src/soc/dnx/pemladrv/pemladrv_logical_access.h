
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _PEMLADRV_LOGICAL_ACCESS_H_
#define _PEMLADRV_LOGICAL_ACCESS_H_

#define MAX_MEM_DATA_LENGTH  500
#define MAX_DATA_VALUE_ARRAY 50
#define MEM_ADDR_LENGTH      16
#define MAX_SHTUT_LENGTH     3
#define NOF_BYTES_IN_WORD    8
#define NOF_BITS_IN_WORD     32
#define NOF_BITS_REPRESENTED_BY_CHAR    4

#include "pemladrv_physical_access.h"
#include "./types/logical_types.h"

#include "pemladrv_cfg_api_defines.h"

#include <stdio.h>
#include <string.h>

#define SRAM    0
#define TCAM    1

#define DOWN    0
#define UP      1

unsigned int dnx_pemladrv_get_nof_mapped_fields(
    unsigned int nof_fields,
    const FieldBitRange * bit_range_arr);

shr_error_e dnx_pemladrv_allocate_pemladrv_mem_struct(
    int unit,
    pemladrv_mem_t ** mem_access,
    unsigned int nof_fields,
    const FieldBitRange * bit_range_arr);

void dnx_pemladrv_free_pemladrv_mem_struct(
    pemladrv_mem_t ** mem_access);

void dnx_set_pem_mem_accesss_fldbuf(
    const unsigned int offset,
    const unsigned int *virtual_db_data_array,
    const FieldBitRange * field_bit_range_arr,
    pemladrv_mem_t * mem_access);

void print_debug_pem_read_write(
    const char *prefix,
    const unsigned int address,
    const unsigned int length_in_bits,
    const unsigned int *value);

shr_error_e pem_read(
    int unit,
    int sub_unit_idx,
    phy_mem_t * mem,
    int is_mem,
    void *entry_data);

shr_error_e pem_write(
    int unit,
    int sub_unit_idx,
    phy_mem_t * mem,
    int is_mem,
    void *entry_data,
    uint32 flags);

int get_core_from_block_id(
    unsigned int block_id);

int get_core_from_memory_address(
    unsigned long long address);

void get_address_and_block_id(
    unsigned long long full_address,
    uint32 *address,
    uint32 *block_id);

unsigned int dnx_get_nof_map_sram_chunk_cols(
    const unsigned int total_result_width);

unsigned int dnx_pemladrv_get_nof_unsigned_int_chunks(
    const int field_width);

void dnx_set_mask_with_ones(
    const unsigned int msb_bit,
    const unsigned int lsb_bit,
    unsigned int *virtual_field_mask);

unsigned int count_set_bits_in_unsinged_int_array(
    unsigned int nof_chunks,
    unsigned int *mapped_bits_arr);

shr_error_e dnx_pemladrv_pem_allocate_fldbuf_by_bit_range_array(
    int unit,
    const int nof_fields,
    const FieldBitRange * bit_range_array,
    pemladrv_mem_t * pem_mem_access);

void dnx_build_virtual_db_data(
    const unsigned int offset,
    const FieldBitRange * field_bit_range_arr,
    const pemladrv_mem_t * data,
    unsigned int *virtual_field_data);

void dnx_set_ones_in_chunk(
    const unsigned int lsb_bit,
    const unsigned int msb_bit,
    const unsigned int virtual_mask_arr_index,
    unsigned int *virtual_field_mask);

int dnx_do_chunk_require_writing(
    const DbChunkMapper * chunk_mapper,
    const unsigned int *virtual_field_mask);

shr_error_e dnx_find_or_allocate_and_read_physical_data(
    int unit,
    int sub_unit_idx,
    const unsigned int chunk_mem_block_id,
    const unsigned int chunk_phy_mem_addr,
    const unsigned int chunk_phy_mem_row_index,
    const unsigned int chunk_phy_mem_width,
    const unsigned int chunk_mapping_width,
    const unsigned int is_industrial_tcam,
    const unsigned int is_ingress,
    const int is_mem,
    PhysicalWriteInfo ** target_physical_memory_entry_data_list,
    PhysicalWriteInfo ** phy_wire);

shr_error_e dnx_modify_entry_data(
    int unit,
    const unsigned char flag,
    const unsigned char last_chunk,
    const unsigned int chunk_mapping_width,
    const unsigned int chunk_virtual_mem_width_offset,
    const unsigned int chunk_physical_mem_width_offset,
    const int total_key_width,
    const unsigned int *virtual_db_line_mask_arr,
    const unsigned int *virtual_db_line_input_data_arr,
    unsigned int *physical_memory_entry_data);

void set_bits(
    const unsigned int *in_buff,
    int in_buff_start_bit_indx,
    int out_buff_start_bit_indx,
    int nof_bits,
    unsigned int *out_buff);

void dnx_set_pem_mem_accesss_fldbuf_from_physical_sram_data_array(
    const unsigned int *virtual_db_data_array,
    const FieldBitRange * field_bit_range_arr,
    pemladrv_mem_t * result);

void dnx_free_list(
    PhysicalWriteInfo * target_physical_memory_entry_data_curr);

shr_error_e dnx_write_all_physical_data_from_list(
    int unit,
    int sub_unit_idx,
    PhysicalWriteInfo * target_physical_memory_entry_data_curr);

shr_error_e dnx_pemladrv_set_valid_bit_to_zero(
    const int unit,
    const int core_id,
    const int virtual_row_index,
    LogicalTcamInfo * tcam_info);

unsigned int dnx_get_nof_tcam_chunk_cols(
    const unsigned int total_key_width);

int dnx_calculate_physical_row_index_from_chunk_mapper(
    const int unit,
    const DbChunkMapper * chunk_mapper,
    const int virtual_row_index);

void dnx_init_phy_mem_t_from_chunk_mapper(
    const int unit,
    const unsigned int chunk_mem_block_id,
    const unsigned int chunk_phy_mem_addr,
    const unsigned int chunk_phy_mem_row_index,
    const unsigned int chunk_phy_mem_width,
    const unsigned int is_indestrial_tcam,
    const unsigned int is_ingress,
    const unsigned int is_mem,
    phy_mem_t * phy_mem);

shr_error_e pemladrv_lpm_remove_by_index(
    const int unit,
    const int core_id,
    table_id_t table_id,
    int index);

shr_error_e dnx_pemladrv_read_physical_and_update_cache(
    const int unit);

void dnx_pemladrv_set_pem_mem_access_with_value(
    pemladrv_mem_t * mem_access,
    unsigned int nof_fields,
    FieldBitRange * bit_range_arr,
    unsigned int field_val);

unsigned int dnx_calculate_prefix_from_mask_array(
    const int total_key_width,
    const unsigned int *mask_arr);

int dnx_pemladrv_is_db_mapped_to_pes(
    const LogicalDbChunkMapperMatrix * const db_chunk_mapper);

int dnx_pemladrv_compare_pem_mem_access(
    pemladrv_mem_t * mem_access_a,
    pemladrv_mem_t * mem_access_b,
    unsigned int nof_fields,
    FieldBitRange * bit_range_arr);

void get_bits(
    const unsigned int *in_buff,
    int start_bit_indx,
    int nof_bits,
    unsigned int *out_buff);

int dnx_pemladrv_calc_nof_arr_entries_from_width(
    const unsigned int width_in_bits);

int compare_address_to_cache_entry(
    phy_mem_t * mem,
    const uint32 *cache_data);

int find_entry_in_cache(
    const int unit,
    phy_mem_t * mem,
    int start_ndx,
    int core,
    int is_ingress0_egress1);

shr_error_e dnx_run_over_all_chunks_read_physical_data_and_update_physical_memory_entry_data_list(
    int unit,
    int sub_unit_idx,
    const unsigned char flag,
    const unsigned int virtual_row_index,
    const unsigned int total_width,
    const unsigned int chunk_matrix_row,
    const unsigned int nof_implamentations,
    const unsigned int nof_chunk_matrix_cols,
    const LogicalDbChunkMapperMatrix * db_chunk_mapper_matrix,
    const unsigned int *virtual_mask,
    const unsigned int *virtual_data,
    PhysicalWriteInfo ** phy_wire);

shr_error_e dnx_build_physical_db_result_data_array(
    int unit,
    int sub_unit_idx,
    const LogicalDbChunkMapperMatrix * db_chunk_mapper_matrix,
    const unsigned int nof_chunk_cols,
    const unsigned int chunk_matrix_row_index,
    const unsigned int virtual_row_index,
    const FieldBitRange * field_bit_range_arr,
    pemladrv_mem_t * result,
    unsigned int *virtual_db_data_arr);

#ifdef BCM_DNX_SUPPORT
shr_error_e pem_cs_tcam_read_entry(
    int unit,
    int sub_unit_idx,
    phy_mem_t * mem,
    void *entry_data);
#endif

#endif
