
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "../pemladrv_logical_access.h"
#include "../pemladrv_meminfo_init.h"
#include "../pemladrv_meminfo_access.h"
#include "../pemladrv_cfg_api_defines.h"

#ifndef _PEMLADRV_REG_LOGICAL_ACCESS_H_
#define _PEMLADRV_REG_LOGICAL_ACCESS_H_

void mask_reg_read(
    pemladrv_mem_t * result,
    const pemladrv_mem_t * result_read_bits_mask,
    unsigned int nof_fields,
    FieldBitRange * bit_range_arr);

shr_error_e dnx_pemladrv_get_reg_or_reg_field_name(
    int unit,
    char *name,
    int reg1_reg_field0,
    char **target_name);

shr_error_e dnx_pemladrv_get_reg_name(
    const int unit,
    const int reg_id,
    char **target_name);

shr_error_e dnx_pemladrv_get_reg_field_name(
    const int unit,
    const int reg_id,
    const int field_id,
    char **target_name);

unsigned int calculate_mapped_register_size(
    unsigned int unit,
    unsigned int reg_index,
    unsigned int field_index);

shr_error_e dnx_pemladrv_reg_field_run_over_all_chunks_read_physical_data_and_update_physical_memory_entry_data_list(
    int unit,
    int core_id,
    LogicalRegFieldInfo * reg_field_info_pnt,
    FieldBitRange * reg_field_bit_range,
    const unsigned int *virtual_mask,
    const unsigned int *virtual_data,
    PhysicalWriteInfo * target_physical_memory_entry_data_list,
    PhysicalWriteInfo ** pw_info);

shr_error_e dnx_pemladrv_reg_field_run_over_all_chunks_read_physical_data_to_virtual_data(
    int unit,
    int core_id,
    LogicalRegFieldInfo * reg_field_info_pnt,
    FieldBitRange * reg_field_bit_range,
    unsigned int *virtual_mask,
    unsigned int *virtual_data,
    PhysicalWriteInfo * target_physical_memory_entry_data_list,
    PhysicalWriteInfo ** pw_info);

shr_error_e pemladrv_reg_write_inner(
    int unit,
    int core_id,
    reg_id_t reg_id,
    pemladrv_mem_t * data);

void dnx_reg_read_entry_data(
    const unsigned int chunk_mapping_width,
    const unsigned int chunk_virtual_mem_width_offset,
    const unsigned int chunk_physical_mem_width_offset,
    const unsigned int *physical_memory_entry_data,
    unsigned int *virtual_db_line_mask_arr,
    unsigned int *virtual_db_line_input_data_arr);

shr_error_e dnx_pemladrv_reg_read_and_retreive_read_bits_mask(
    int unit,
    int core_id,
    reg_id_t reg_id,
    pemladrv_mem_t * result,
    pemladrv_mem_t * result_read_bits_mask);

#endif
