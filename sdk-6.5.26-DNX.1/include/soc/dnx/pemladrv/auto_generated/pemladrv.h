
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _PEMLADRV_H_
#define _PEMLADRV_H_

#ifdef BCM_DNX_SUPPORT
#include <soc/sand/shrextend/shrextend_debug.h>
#include "src/soc/dnx/pemladrv/types/physical_types.h"
#else
#include "./pemladrv/pemladrv_cfg_api_defines.h"
#include "./pemladrv/types/physical_types.h"
#endif

shr_error_e pemladrv_direct_write(
    const int unit,
    const int core_id,
    table_id_t db_id,
    unsigned int row_index,
    pemladrv_mem_t * data);
shr_error_e pemladrv_direct_read(
    const int unit,
    const int core_id,
    table_id_t db_id,
    unsigned int row_index,
    pemladrv_mem_t * result);

shr_error_e pemladrv_reg_write(
    int unit,
    int core_id,
    reg_id_t reg_id,
    pemladrv_mem_t * data);
shr_error_e pemladrv_reg_read(
    int unit,
    int core_id,
    reg_id_t reg_id,
    pemladrv_mem_t * result);

shr_error_e pemladrv_tcam_write(
    const int unit,
    const int core_id,
    tcam_id_t tcam_id,
    unsigned int row_index,
    pemladrv_mem_t * key,
    pemladrv_mem_t * mask,
    pemladrv_mem_t * valid,
    pemladrv_mem_t * data);
shr_error_e pemladrv_tcam_read(
    const int unit,
    const int core_id,
    tcam_id_t tcam_id,
    unsigned int row_index,
    pemladrv_mem_t * key,
    pemladrv_mem_t * mask,
    pemladrv_mem_t * valid,
    pemladrv_mem_t * result);

shr_error_e dnx_pemladrv_full_reg_write(
    const int unit,
    const int core_id,
    const int reg_id,
    const int size,
    const unsigned int *data);

shr_error_e dnx_pemladrv_virtual_register_size_in_bits_get(
    const int unit,
    const int reg_id,
    const int field_id,
    int *size);

shr_error_e dnx_pemladrv_reg_info_allocate(
    const int unit,
    pemladrv_reg_info ** ptr,
    int *size);

void dnx_pemladrv_reg_info_de_allocate(
    const int unit,
    pemladrv_reg_info * ptr);

shr_error_e dnx_pemladrv_direct_info_allocate(
    const int unit,
    pemladrv_db_info ** ptr,
    int *size);

void dnx_pemladrv_direct_info_de_allocate(
    const int unit,
    pemladrv_db_info * ptr);

shr_error_e dnx_pemladrv_tcam_info_allocate(
    const int unit,
    pemladrv_db_info ** ptr,
    int *size);

void dnx_pemladrv_tcam_info_de_allocate(
    const int unit,
    pemladrv_db_info * ptr);

shr_error_e dnx_pemladrv_em_info_allocate(
    const int unit,
    pemladrv_db_info ** arr,
    int *size);

void dnx_pemladrv_em_info_de_allocate(
    const int unit,
    pemladrv_db_info * ptr);

shr_error_e dnx_pemladrv_lpm_info_allocate(
    const int unit,
    pemladrv_db_info ** arr,
    int *size);

void dnx_pemladrv_lpm_info_de_allocate(
    const int unit,
    pemladrv_db_info * ptr);

VirtualWiresContainer *dnx_pemladrv_get_virtual_wires_info(
    const int unit);

shr_error_e pemladrv_em_insert(
    const int unit,
    const int core_id,
    table_id_t table_id,
    pemladrv_mem_t * key,
    pemladrv_mem_t * result,
    int *index);

shr_error_e pemladrv_em_remove(
    const int unit,
    const int core_id,
    table_id_t table_id,
    pemladrv_mem_t * key,
    int *index);

shr_error_e pemladrv_em_lookup(
    const int unit,
    const int core_id,
    table_id_t table_id,
    pemladrv_mem_t * key,
    pemladrv_mem_t * result,
    int *index);

void pemladrv_em_get_next_index(
    const int unit,
    const int core_id,
    table_id_t table_id,
    int *index);

shr_error_e pemladrv_em_entry_get_by_id(
    const int unit,
    const int core_id,
    table_id_t table_id,
    int index,
    pemladrv_mem_t * key,
    pemladrv_mem_t * result);

shr_error_e pemladrv_em_entry_set_by_id(
    const int unit,
    const int core_id,
    table_id_t table_id,
    int index,
    pemladrv_mem_t * key,
    pemladrv_mem_t * result);

shr_error_e pemladrv_em_remove_all(
    const int unit,
    const int core_id,
    table_id_t table_id);

shr_error_e pemladrv_em_remove_by_index(
    const int unit,
    const int core_id,
    table_id_t table_id,
    int index);

shr_error_e pemladrv_lpm_insert(
    const int unit,
    const int core_id,
    table_id_t table_id,
    pemladrv_mem_t * key,
    int length_in_bits,
    pemladrv_mem_t * result,
    int *index);

shr_error_e pemladrv_lpm_remove(
    const int unit,
    const int core_id,
    table_id_t table_id,
    pemladrv_mem_t * key,
    int length_in_bits,
    int *index);

shr_error_e pemladrv_lpm_lookup(
    const int unit,
    const int core_id,
    table_id_t table_id,
    pemladrv_mem_t * key,
    pemladrv_mem_t * result,
    int *index);

shr_error_e pemladrv_lpm_remove_all(
    const int unit,
    const int core_id,
    table_id_t table_id);

shr_error_e pemladrv_mem_alloc(
    int unit,
    const unsigned int nof_fields,
    pemladrv_mem_t ** mem);

shr_error_e pemladrv_mem_alloc_field(
    int unit,
    pemladrv_field_t * pem_field_access,
    const unsigned int field_width_in_bits);

shr_error_e pemladrv_mem_alloc_direct(
    const int unit,
    const int db_id,
    pemladrv_mem_t ** result);

shr_error_e pemladrv_mem_alloc_tcam(
    const int unit,
    const int db_id,
    pemladrv_mem_t ** key,
    pemladrv_mem_t ** mask,
    pemladrv_mem_t ** valid,
    pemladrv_mem_t ** result);

shr_error_e pemladrv_mem_alloc_lpm(
    const int unit,
    const int db_id,
    pemladrv_mem_t ** key,
    pemladrv_mem_t ** result);

shr_error_e pemladrv_mem_alloc_em(
    const int unit,
    const int db_id,
    pemladrv_mem_t ** key,
    pemladrv_mem_t ** result);

void pemladrv_mem_free(
    pemladrv_mem_t * pem_mem_access);

void free_api_info(
    const int unit);

shr_error_e dnx_pemladrv_init(
    const int unit,
    int restore_after_reset,
    int use_file,
    const char *rel_file_path,
    const char *vmem_definition_file_name);
shr_error_e dnx_pemladrv_deinit(
    const int unit);

shr_error_e dnx_get_version(
    const int unit,
    int print_to_cout,
    VersionInfo ** version);

#endif
