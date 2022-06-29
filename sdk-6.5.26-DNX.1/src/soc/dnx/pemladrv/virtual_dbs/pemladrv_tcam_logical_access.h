
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _PEMLADRV_TCAM_LOGICAL_ACCESS_H_
#define _PEMLADRV_TCAM_LOGICAL_ACCESS_H_

shr_error_e dnx_build_physical_db_key_data_array(
    int unit,
    int sub_unit_idx,
    const LogicalDbChunkMapperMatrix * db_chunk_mapper_matrix,
    const unsigned int nof_chunk_cols,
    const unsigned int chunk_matrix_row_index,
    const unsigned int virtual_row_index,
    const unsigned int total_virtual_key_width,
    unsigned int *virtual_db_data_arr);

shr_error_e dnx_produce_valid_field_bit_range(
    int unit,
    FieldBitRange ** valid_bit_array);

shr_error_e dnx_set_pem_mem_accesss_fldbuf_from_physical_tcam_data_array(
    int unit,
    const unsigned int *virtual_db_data_array,
    const unsigned int total_key_width,
    const FieldBitRange * field_bit_range_arr,
    pemladrv_mem_t * key,
    pemladrv_mem_t * mask,
    pemladrv_mem_t * valid);

shr_error_e dnx_convert_tcam_data_into_key_array(
    int unit,
    const int key_width_in_bits,
    unsigned int *tcam_data,
    unsigned int **key_arr);

shr_error_e dnx_convert_tcam_data_into_mask_array(
    int unit,
    const int total_key_width,
    const unsigned int *tcam_data,
    unsigned int **mask_arr);

shr_error_e dnx_get_prefix_length_from_physical_data(
    int unit,
    const int total_key_width,
    const unsigned int *tcam_data,
    unsigned int *prefix);

void dnx_build_virtual_db_mask(
    const unsigned int offset,
    const FieldBitRange * field_bit_range_arr,
    const pemladrv_mem_t * data,
    unsigned int *virtual_field_mask);

shr_error_e dnx_build_virtual_db_mask_and_data_from_key_fields(
    int unit,
    const int key_width,
    const unsigned int total_width,
    const FieldBitRange * field_bit_range_arr,
    const pemladrv_mem_t * key,
    const pemladrv_mem_t * mask,
    const pemladrv_mem_t * valid,
    unsigned int **virtual_key_mask,
    unsigned int **virtual_key_data);

unsigned int dnx_get_nof_cam_sram_chunk_cols(
    const unsigned int total_result_width);

shr_error_e dnx_build_virtual_db_mask_and_data_from_result_fields(
    int unit,
    const unsigned int total_width,
    const FieldBitRange * field_bit_range_arr,
    const pemladrv_mem_t * result,
    const int is_init_mask_to_ones_at_field_location,
    unsigned int **virtual_result_mask,
    unsigned int **virtual_result_data);

shr_error_e dnx_logical_cam_based_db_write(
    const LogicalTcamInfo * cam_info,
    const int unit,
    const int core_id,
    const uint32 row_index,
    const pemladrv_mem_t * key,
    const pemladrv_mem_t * mask,
    const pemladrv_mem_t * valid,
    const pemladrv_mem_t * data);

shr_error_e dnx_logical_cam_based_db_read(
    const LogicalTcamInfo * cam_info,
    const int unit,
    const int core_id,
    const uint32 row_index,
    pemladrv_mem_t * key,
    pemladrv_mem_t * mask,
    pemladrv_mem_t * valid,
    pemladrv_mem_t * result);

int dnx_pemladrv_does_key_exists_in_physical_db(
    const int unit,
    const LogicalTcamInfo * tcam_info,
    const unsigned char *valids_arr,
    unsigned int **key_entry_arr,
    const unsigned int *key);

shr_error_e dnx_pemladrv_build_tcam_valid(
    int unit,
    pemladrv_mem_t ** tcam);

unsigned int dnx_pemladrv_get_nof_chunks_in_fldbuf(
    const unsigned int field_index,
    const FieldBitRange * field_bit_range_arr);

unsigned int dnx_pemladrv_copmare_two_virtual_keys(
    const unsigned int key_width_in_bits,
    const unsigned int *first_key,
    const unsigned int *second_key);

shr_error_e dnx_pemladrv_find_physical_entry_index_by_key(
    const int unit,
    const int core_id,
    const LogicalTcamInfo * tcam_info,
    const unsigned char *valids_arr,
    unsigned int **key_field_arr,
    const pemladrv_mem_t * key,
    int *index);

void dnx_pemladrv_update_key_with_prefix_length(
    const int prefix_length,
    const int key_length,
    unsigned int *virtual_key_data);

shr_error_e dnx_pemladrv_zero_last_chunk_of_cam_based_dbs(
    const int unit);

shr_error_e dnx_pemladrv_zero_last_chunk_of_tcam_dbs(
    const int unit);

shr_error_e dnx_pemladrv_zero_last_chunk_of_lpm_dbs(
    const int unit);

shr_error_e dnx_pemladrv_zero_last_chunk_of_em_dbs(
    const int unit);

shr_error_e dnx_pemladrv_update_em_cache_from_physical(
    const int unit);

shr_error_e dnx_pemladrv_update_lpm_cache_from_physical(
    const int unit);

#endif
