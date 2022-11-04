
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#include "../pemladrv_cfg_api_defines.h"

#ifndef _PEMLADRV_LPM_LOGICAL_ACCESS__H_
#define _PEMLADRV_LPM_LOGICAL_ACCESS__H_

shr_error_e dnx_pemladrv_does_key_exists_in_physical_lpm_db(
    const int unit,
    const LogicalTcamInfo * tcam_info,
    const unsigned int prefix_length_in_bits,
    const LpmDbCache * lpm_cache,
    const unsigned int *key,
    int *index);

shr_error_e dnx_pemladrv_build_lpm_mask(
    int unit,
    const FieldBitRange * key_field_bit_range_arr,
    int prefix_length_in_bits,
    const int key_length_in_bits,
    pemladrv_mem_t * key,
    pemladrv_mem_t ** mask);

void dnx_pemladrv_update_lpm_cache(
    const int core_id,
    unsigned int row_index,
    const int prefix_length,
    unsigned int *virtual_key_data,
    unsigned int *virtual_result_data,
    LogicalLpmInfo * lpm_cache);

int dnx_pemladrv_find_next_lpm_free_entry(
    const int unit,
    const int core_id,
    const int index,
    const int direction,
    unsigned char *physical_entry_occupation);

shr_error_e dnx_pemladrv_find_physical_lpm_entry_index_by_key(
    const int unit,
    const int core_id,
    const LogicalTcamInfo * tcam_info,
    const LpmDbCache * lpm_cache,
    const pemladrv_mem_t * key,
    int *index);

void dnx_pemladrv_get_lpm_data_by_entry(
    const int unit,
    const int core_id,
    const int db_id,
    const int index,
    pemladrv_mem_t * key,
    pemladrv_mem_t * result);

int dnx_pemladrv_find_lpm_index_to_insert(
    const int unit,
    const unsigned int prefix_length,
    const int key_length,
    LpmDbCache * lpm_cache);

int dnx_pemladrv_find_most_suitable_index_with_matching_prefix(
    const int unit,
    const int index,
    LpmDbCache * lpm_cache);

int dnx_pemladrv_find_last_index_with_same_prefix(
    const int unit,
    const int index,
    unsigned int *lpm_key_entry_prefix_length_arr);

int dnx_pemladrv_is_lpm_full(
    const int unit,
    const int nof_entries,
    unsigned char *physical_entry_occupation);

int dnx_pemladrv_find_minimum_shifting_direction(
    const int unit,
    const int core_id,
    const int index,
    const int table_id);

shr_error_e dnx_pemladrv_move_physical_lpm_entry(
    const int unit,
    const int core_id,
    const int old_index,
    const int new_index,
    const int table_id);

shr_error_e dnx_pemladrv_shift_physical_to_clear_entry(
    const int unit,
    const int core_id,
    const int shift_direction,
    int *index,
    const int table_id);

shr_error_e dnx_pem_lpm_get_by_id(
    const int unit,
    const int core_id,
    table_id_t table_id,
    int index,
    pemladrv_mem_t * key,
    int *length_in_bits,
    pemladrv_mem_t * result);

shr_error_e dnx_pem_lpm_set_by_id(
    const int unit,
    const int core_id,
    table_id_t table_id,
    int index,
    pemladrv_mem_t * key,
    int *length_in_bits,
    pemladrv_mem_t * result);

shr_error_e pemladrv_lpm_insert_inner(
    const int unit,
    const int core_id,
    table_id_t table_id,
    pemladrv_mem_t * key,
    int length_in_bits,
    pemladrv_mem_t * result,
    int *index);

shr_error_e pemladrv_lpm_remove_all_inner(
    const int unit,
    const int core_id,
    table_id_t table_id);

#endif
