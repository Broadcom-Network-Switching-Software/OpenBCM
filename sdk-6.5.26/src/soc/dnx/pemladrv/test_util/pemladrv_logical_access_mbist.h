
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved. 
 */
/* *INDENT-OFF* */
#include "../types/physical_types.h"
#include "../types/logical_types.h"

#ifndef _PEMLADRV_LOGICAL_ACCESS_MBIST_H_
#define _PEMLADRV_LOGICAL_ACCESS_MBIST_H_





unsigned int get_mbist_val_by_ndx(const unsigned int mbist_val_ndx);




shr_error_e dnx_pemladrv_create_pemladrv_mem_struct_db_direct(
        int unit,
        pemladrv_mem_t ** mem_access,
        LogicalDirectInfo * db_info);

void dnx_pemladrv_set_mbist_value_to_field_db_direct(
        pemladrv_mem_t * mem_access,
        LogicalDirectInfo * db_info,
        unsigned int field_val);

int dnx_pemladrv_compare_pemladrv_mem_structs_db_direct(
        pemladrv_mem_t * mem_access_a,
        pemladrv_mem_t * mem_access_b,
        LogicalDirectInfo * db_info);

shr_error_e  dnx_pemladrv_pem_mem_direct_db_virtual_bist(int unit);




shr_error_e dnx_pemladrv_create_pemladrv_mem_struct_db_tcam(
        int unit,
        pemladrv_mem_t ** key,
        pemladrv_mem_t ** mask,
        pemladrv_mem_t ** valid,
        pemladrv_mem_t ** result,
        LogicalTcamInfo * db_info) ;

void dnx_pemladrv_set_mbist_value_to_field_db_tcam(
        pemladrv_mem_t * key,
        pemladrv_mem_t * mask,
        pemladrv_mem_t * valid,
        pemladrv_mem_t * result,
        LogicalTcamInfo * db_info,
        unsigned int field_val);

int dnx_pemladrv_compare_pemladrv_mem_structs_db_tcam(
        pemladrv_mem_t * key_write,
        pemladrv_mem_t * key_read,
        pemladrv_mem_t * mask_write,
        pemladrv_mem_t * mask_read,
        pemladrv_mem_t * result_write,
        pemladrv_mem_t * result_read,
        pemladrv_mem_t * valid_write,
        pemladrv_mem_t * valid_read,
        LogicalTcamInfo * db_info);

void dnx_pemladrv_free_pemladrv_mem_structs_db_tcam(
        pemladrv_mem_t ** key,
        pemladrv_mem_t ** mask,
        pemladrv_mem_t ** valid,
        pemladrv_mem_t ** result);

shr_error_e dnx_pemladrv_pem_mem_tcam_db_virtual_bist(int unit);





shr_error_e dnx_pemladrv_create_pemladrv_mem_struct_db_em_lpm(
        int unit,
        pemladrv_mem_t ** key,
        pemladrv_mem_t ** result,
        LogicalTcamInfo * db_info);

void dnx_pemladrv_set_mbist_value_to_field_db_em_lpm(
        pemladrv_mem_t * key,
        pemladrv_mem_t * result,
        LogicalTcamInfo * db_info,
        unsigned int field_val);

int dnx_pemladrv_compare_pemladrv_mem_structs_db_em_lpm(
        pemladrv_mem_t * key_write,
        pemladrv_mem_t * key_read,
        pemladrv_mem_t * result_write,
        pemladrv_mem_t * result_read,
        LogicalTcamInfo * db_info);

shr_error_e dnx_pemladrv_pem_mem_em_db_virtual_bist(int unit);

shr_error_e dnx_pemladrv_pem_mem_lpm_db_virtual_bist(int unit);




shr_error_e dnx_pemladrv_pem_mem_reg_virtual_bist(int unit);


shr_error_e dnx_pemladrv_pem_mem_virtual_bist(int unit);

void print_debug_pem_read_write(
        const char *prefix,
        const unsigned int address,
        const unsigned int length_in_bits,
        const unsigned int *value);

#endif 


