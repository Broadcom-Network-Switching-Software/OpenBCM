
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _PEMLADRV_EM_LOGICAL_ACCESS_H_
#define _PEMLADRV_EM_LOGICAL_ACCESS_H_

shr_error_e dnx_pemladrv_build_em_mask(
    int unit,
    const FieldBitRange * key_field_bit_range_arr,
    const pemladrv_mem_t * key,
    pemladrv_mem_t ** mask);

void dnx_pemladrv_update_em_cache_and_next_free_index(
    const int core_id,
    unsigned int row_index,
    unsigned int *virtual_key_data,
    unsigned int *virtual_result_data,
    LogicalEmInfo * em_cache);

int dnx_pemladrv_find_next_em_free_entry(
    const int core_id,
    LogicalEmInfo * em_cache);

shr_error_e dnx_pemladrv_get_em_data_by_entry(
    const int unit,
    const int core_id,
    const int db_id,
    const int index,
    pemladrv_mem_t * key,
    pemladrv_mem_t * result);

shr_error_e pemladrv_em_insert_inner(
    const int unit,
    const int core_id,
    table_id_t table_id,
    pemladrv_mem_t * key,
    pemladrv_mem_t * result,
    int *index);

shr_error_e pemladrv_em_remove_all_inner(
    const int unit,
    const int core_id,
    table_id_t table_id);

#endif
