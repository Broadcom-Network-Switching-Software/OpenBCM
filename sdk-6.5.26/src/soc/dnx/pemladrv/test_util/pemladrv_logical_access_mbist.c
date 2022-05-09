
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved. 
 */
/* *INDENT-OFF* */
#include <stdlib.h>
#include <time.h>
#include "../pemladrv_logical_access.h"
#include "../virtual_dbs/pemladrv_reg_logical_access.h"
#include "../virtual_dbs/pemladrv_direct_logical_access.h"
#include "../virtual_dbs/pemladrv_tcam_logical_access.h"
#include "../virtual_dbs/pemladrv_lpm_logical_access.h"
#include "../virtual_dbs/pemladrv_em_logical_access.h"
#include "../pemladrv_cfg_api_defines.h"

#ifdef BCM_DNX_SUPPORT
#define BSL_LOG_MODULE BSL_LS_SOCDNX_ADAPTERDNX
#endif

extern ApiInfo api_info[MAX_NOF_UNITS];






const unsigned int nof_mbist_values = 4;
const unsigned int gMbistVal[] = {0xFFFFFFFF, 0x0, 0xAAAAAAAA, 0x55555555};

unsigned int get_mbist_val_by_ndx(const unsigned int mbist_val_ndx) {
    return gMbistVal[mbist_val_ndx];
}




shr_error_e dnx_pemladrv_create_pemladrv_mem_struct_db_direct(
        int unit,
        pemladrv_mem_t ** mem_access,
        LogicalDirectInfo * db_info) {
    const int core_id = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pemladrv_allocate_pemladrv_mem_struct(unit, mem_access,
                                                     db_info->db_direct_info_per_core_arr[core_id].nof_fields,
                                                     db_info->db_direct_info_per_core_arr[core_id].field_bit_range_arr));
exit:
    SHR_FUNC_EXIT;
}

void dnx_pemladrv_set_mbist_value_to_field_db_direct(
        pemladrv_mem_t * mem_access,
        LogicalDirectInfo * db_info,
        unsigned int field_val) {
    const int core_id = 0;
    unsigned int nof_fields = db_info->db_direct_info_per_core_arr[core_id].nof_fields;

    dnx_pemladrv_set_pem_mem_access_with_value(mem_access, nof_fields,
                                               db_info->db_direct_info_per_core_arr[core_id].field_bit_range_arr,
                                               field_val);
}

int dnx_pemladrv_compare_pemladrv_mem_structs_db_direct(
        pemladrv_mem_t * mem_access_a,
        pemladrv_mem_t * mem_access_b,
        LogicalDirectInfo * db_info) {
    const int core_id = 0;
    return dnx_pemladrv_compare_pem_mem_access(mem_access_a, mem_access_b,
                                               db_info->db_direct_info_per_core_arr[core_id].nof_fields,
                                               db_info->db_direct_info_per_core_arr[core_id].field_bit_range_arr);
}

shr_error_e dnx_pemladrv_pem_mem_direct_db_virtual_bist(int unit)
{
    unsigned int db_ndx, entry_ndx, nof_entries, mbist_val_ndx;
    unsigned int nof_direct_dbs = api_info[unit].db_direct_container.nof_direct_dbs;
    LogicalDirectInfo* db_info = NULL;
    pemladrv_mem_t* data_to_write_to_design = NULL;
    pemladrv_mem_t* data_read_from_design = NULL;
    const int core_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    
    for (db_ndx = 0; db_ndx < nof_direct_dbs; ++db_ndx){
        db_info = &(api_info[unit].db_direct_container.db_direct_info_arr[db_ndx]);
        nof_entries = db_info->db_direct_info_per_core_arr[core_id].total_nof_entries;
        if (!dnx_pemladrv_is_db_mapped_to_pes(db_info->db_direct_info_per_core_arr[core_id].result_chunk_mapper_matrix_arr))
            continue;
        
        SHR_IF_ERR_EXIT(dnx_pemladrv_create_pemladrv_mem_struct_db_direct(unit, &data_to_write_to_design, db_info));
        SHR_IF_ERR_EXIT(dnx_pemladrv_create_pemladrv_mem_struct_db_direct(unit, &data_read_from_design,   db_info));
        dnx_pemladrv_set_mbist_value_to_field_db_direct(data_read_from_design, db_info, 0x0);
        for (entry_ndx = 0; entry_ndx < nof_entries; ++entry_ndx){
            
            for (mbist_val_ndx = 0; mbist_val_ndx < nof_mbist_values; ++mbist_val_ndx){
                dnx_pemladrv_set_mbist_value_to_field_db_direct(data_to_write_to_design, db_info, get_mbist_val_by_ndx(mbist_val_ndx));
                SHR_IF_ERR_EXIT(pemladrv_direct_write(unit, core_id, db_ndx, entry_ndx, data_to_write_to_design));
                SHR_IF_ERR_EXIT(pemladrv_direct_read(unit, core_id,  db_ndx, entry_ndx, data_read_from_design));
                if (dnx_pemladrv_compare_pemladrv_mem_structs_db_direct(data_to_write_to_design, data_read_from_design, db_info)) {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "");
                }
            }
        }
    }

exit:
    if(data_to_write_to_design)
        dnx_pemladrv_free_pemladrv_mem_struct(&data_to_write_to_design);
    if(data_read_from_design)
        dnx_pemladrv_free_pemladrv_mem_struct(&data_read_from_design);
    SHR_FUNC_EXIT;
}




shr_error_e dnx_pemladrv_create_pemladrv_mem_struct_db_tcam(
        int unit,
        pemladrv_mem_t ** key,
        pemladrv_mem_t ** mask,
        pemladrv_mem_t ** valid,
        pemladrv_mem_t ** result,
        LogicalTcamInfo * db_info) {
    const int core_id = 0;
    unsigned int nof_fields_in_key = db_info->db_tcam_per_core_arr[core_id].nof_fields_in_key;
    unsigned int nof_fields_in_result = db_info->db_tcam_per_core_arr[core_id].nof_fields_in_result;
    FieldBitRange valid_bit_range;

    SHR_FUNC_INIT_VARS(unit);

    valid_bit_range.lsb = 0;
    valid_bit_range.msb = 0;
    valid_bit_range.is_field_mapped = 1;

    
    SHR_IF_ERR_EXIT(dnx_pemladrv_allocate_pemladrv_mem_struct(unit, key, nof_fields_in_key,
                                                         db_info->db_tcam_per_core_arr[core_id].key_field_bit_range_arr));

    SHR_IF_ERR_EXIT(dnx_pemladrv_allocate_pemladrv_mem_struct(unit, mask, nof_fields_in_key,
                                                         db_info->db_tcam_per_core_arr[core_id].key_field_bit_range_arr));

    SHR_IF_ERR_EXIT(dnx_pemladrv_allocate_pemladrv_mem_struct(unit, result, nof_fields_in_result,
                                                         db_info->db_tcam_per_core_arr[core_id].result_field_bit_range_arr));

    SHR_IF_ERR_EXIT(dnx_pemladrv_allocate_pemladrv_mem_struct(unit, valid, 1, &valid_bit_range));

exit:
    SHR_FUNC_EXIT;
}

void dnx_pemladrv_set_mbist_value_to_field_db_tcam(
        pemladrv_mem_t * key,
        pemladrv_mem_t * mask,
        pemladrv_mem_t * valid,
        pemladrv_mem_t * result,
        LogicalTcamInfo * db_info,
        unsigned int field_val) {
    const int core_id = 0;
    unsigned int nof_fields_in_key = db_info->db_tcam_per_core_arr[core_id].nof_fields_in_key;
    unsigned int nof_fields_in_result = db_info->db_tcam_per_core_arr[core_id].nof_fields_in_result;
    FieldBitRange valid_bit_range;

    valid_bit_range.lsb = 0;
    valid_bit_range.msb = 0;
    valid_bit_range.is_field_mapped = 1;

    
    dnx_pemladrv_set_pem_mem_access_with_value(key, nof_fields_in_key,
                                               db_info->db_tcam_per_core_arr[core_id].key_field_bit_range_arr,
                                               field_val);
    dnx_pemladrv_set_pem_mem_access_with_value(mask, nof_fields_in_key,
                                               db_info->db_tcam_per_core_arr[core_id].key_field_bit_range_arr,
                                               field_val);
    dnx_pemladrv_set_pem_mem_access_with_value(result, nof_fields_in_result,
                                               db_info->db_tcam_per_core_arr[core_id].result_field_bit_range_arr,
                                               field_val);
    dnx_pemladrv_set_pem_mem_access_with_value(valid, 1, &valid_bit_range, 1);
}

int dnx_pemladrv_compare_pemladrv_mem_structs_db_tcam(
        pemladrv_mem_t * key_write,
        pemladrv_mem_t * key_read,
        pemladrv_mem_t * mask_write,
        pemladrv_mem_t * mask_read,
        pemladrv_mem_t * result_write,
        pemladrv_mem_t * result_read,
        pemladrv_mem_t * valid_write,
        pemladrv_mem_t * valid_read,
        LogicalTcamInfo * db_info) {
    const int core_id = 0;

    unsigned int nof_fields_in_key = db_info->db_tcam_per_core_arr[core_id].nof_fields_in_key;
    unsigned int nof_fields_in_result = db_info->db_tcam_per_core_arr[core_id].nof_fields_in_result;
    unsigned int ret_val = 0;

    FieldBitRange valid_bit_range;

    valid_bit_range.lsb = 0;
    valid_bit_range.msb = 0;
    valid_bit_range.is_field_mapped = 1;

    
    ret_val |= dnx_pemladrv_compare_pem_mem_access(key_write, key_read, nof_fields_in_key,
                                                   db_info->db_tcam_per_core_arr[core_id].key_field_bit_range_arr);
    ret_val |= dnx_pemladrv_compare_pem_mem_access(mask_write, mask_read, nof_fields_in_key,
                                                   db_info->db_tcam_per_core_arr[core_id].key_field_bit_range_arr);
    ret_val |= dnx_pemladrv_compare_pem_mem_access(result_write, result_read, nof_fields_in_result,
                                                   db_info->db_tcam_per_core_arr[core_id].result_field_bit_range_arr);
    ret_val |= dnx_pemladrv_compare_pem_mem_access(valid_write, valid_read, 1, &valid_bit_range);

    return ret_val;
}

void dnx_pemladrv_free_pemladrv_mem_structs_db_tcam(
        pemladrv_mem_t ** key,
        pemladrv_mem_t ** mask,
        pemladrv_mem_t ** valid,
        pemladrv_mem_t ** result)
{
    dnx_pemladrv_free_pemladrv_mem_struct(key);
    dnx_pemladrv_free_pemladrv_mem_struct(mask);
    dnx_pemladrv_free_pemladrv_mem_struct(valid);
    dnx_pemladrv_free_pemladrv_mem_struct(result);
}

shr_error_e dnx_pemladrv_pem_mem_tcam_db_virtual_bist(int unit){
    unsigned int db_ndx, entry_ndx, nof_entries, mbist_val_ndx;
    unsigned int nof_dbs = api_info[unit].db_tcam_container.nof_tcam_dbs;
    LogicalTcamInfo* db_info = NULL;
    pemladrv_mem_t *key_write = NULL, *mask_write = NULL, *result_write = NULL, *valid_write = NULL;
    pemladrv_mem_t *key_read = NULL, *mask_read = NULL, *result_read = NULL, *valid_read = NULL;
    const int core_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    
    for (db_ndx = 0; db_ndx < nof_dbs; ++db_ndx){
        db_info = &(api_info[unit].db_tcam_container.db_tcam_info_arr[db_ndx]);
        nof_entries = db_info->db_tcam_per_core_arr[core_id].total_nof_entries;
        if (!dnx_pemladrv_is_db_mapped_to_pes(db_info->db_tcam_per_core_arr[core_id].result_chunk_mapper_matrix_arr))
            continue;
        
        dnx_pemladrv_create_pemladrv_mem_struct_db_tcam(unit, &key_write, &mask_write, &valid_write, &result_write, db_info);
        dnx_pemladrv_create_pemladrv_mem_struct_db_tcam(unit, &key_read, &mask_read, &valid_read, &result_read, db_info);
        dnx_pemladrv_set_mbist_value_to_field_db_tcam(key_read, mask_read, valid_read, result_read, db_info, 0x0);
        for (entry_ndx = 0; entry_ndx < nof_entries; ++entry_ndx){
            
            for (mbist_val_ndx = 0; mbist_val_ndx < nof_mbist_values; ++mbist_val_ndx){
                dnx_pemladrv_set_mbist_value_to_field_db_tcam(key_write, mask_write, valid_write, result_write, db_info, get_mbist_val_by_ndx(mbist_val_ndx));
                valid_read->fields[0]->fldbuf[0] = 0;
                SHR_IF_ERR_EXIT(pemladrv_tcam_write(unit, core_id, db_ndx, entry_ndx, key_write, mask_write, valid_write, result_write));

                SHR_IF_ERR_EXIT(pemladrv_tcam_read(unit, core_id, db_ndx, entry_ndx, key_read, mask_read, valid_read, result_read));

                if (dnx_pemladrv_compare_pemladrv_mem_structs_db_tcam(key_write, key_read, mask_write, mask_read, result_write, result_read, valid_write, valid_read, db_info)){
                    SHR_ERR_EXIT(_SHR_E_PARAM, "");
                }
            }
            
            valid_write->fields[0]->fldbuf[0] = 0;
            SHR_IF_ERR_EXIT(pemladrv_tcam_write(unit, core_id, db_ndx, entry_ndx, key_write, mask_write, valid_write, result_write));
        }
    }


exit:
    dnx_pemladrv_free_pemladrv_mem_structs_db_tcam(&key_write, &mask_write, &valid_write, &result_write);
    dnx_pemladrv_free_pemladrv_mem_structs_db_tcam(&key_read, &mask_read, &valid_read, &result_read);
    SHR_FUNC_EXIT;
}




shr_error_e dnx_pemladrv_create_pemladrv_mem_struct_db_em_lpm(
        int unit,
        pemladrv_mem_t ** key,
        pemladrv_mem_t ** result,
        LogicalTcamInfo * db_info) {
    const int core_id = 0;
    unsigned int nof_fields_in_key = db_info->db_tcam_per_core_arr[core_id].nof_fields_in_key;
    unsigned int nof_fields_in_result = db_info->db_tcam_per_core_arr[core_id].nof_fields_in_result;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_pemladrv_allocate_pemladrv_mem_struct(unit, key, nof_fields_in_key,
                                                         db_info->db_tcam_per_core_arr[core_id].key_field_bit_range_arr));
    SHR_IF_ERR_EXIT(dnx_pemladrv_allocate_pemladrv_mem_struct(unit, result, nof_fields_in_result,
                                                         db_info->db_tcam_per_core_arr[core_id].result_field_bit_range_arr));

exit:
    SHR_FUNC_EXIT;
}

void dnx_pemladrv_set_mbist_value_to_field_db_em_lpm(
        pemladrv_mem_t * key,
        pemladrv_mem_t * result,
        LogicalTcamInfo * db_info,
        unsigned int field_val) {
    const int core_id = 0;
    unsigned int nof_fields_in_key = db_info->db_tcam_per_core_arr[core_id].nof_fields_in_key;
    unsigned int nof_fields_in_result = db_info->db_tcam_per_core_arr[core_id].nof_fields_in_result;

    
    dnx_pemladrv_set_pem_mem_access_with_value(key, nof_fields_in_key,
                                               db_info->db_tcam_per_core_arr[core_id].key_field_bit_range_arr,
                                               field_val);
    dnx_pemladrv_set_pem_mem_access_with_value(result, nof_fields_in_result,
                                               db_info->db_tcam_per_core_arr[core_id].result_field_bit_range_arr,
                                               field_val);
}

int dnx_pemladrv_compare_pemladrv_mem_structs_db_em_lpm(
        pemladrv_mem_t * key_write,
        pemladrv_mem_t * key_read,
        pemladrv_mem_t * result_write,
        pemladrv_mem_t * result_read,
        LogicalTcamInfo * db_info) {
    const int core_id = 0;
    unsigned int nof_fields_in_key = db_info->db_tcam_per_core_arr[core_id].nof_fields_in_key;
    unsigned int nof_fields_in_result = db_info->db_tcam_per_core_arr[core_id].nof_fields_in_result;
    unsigned int ret_val = 0;

    
    ret_val |= dnx_pemladrv_compare_pem_mem_access(key_write, key_read, nof_fields_in_key,
                                                   db_info->db_tcam_per_core_arr[core_id].key_field_bit_range_arr);
    ret_val |= dnx_pemladrv_compare_pem_mem_access(result_write, result_read, nof_fields_in_result,
                                                   db_info->db_tcam_per_core_arr[core_id].result_field_bit_range_arr);

    return ret_val;
}

shr_error_e dnx_pemladrv_pem_mem_em_db_virtual_bist(int unit){
    unsigned int db_ndx, entry_ndx, nof_entries, mbist_val_ndx;
    unsigned int nof_dbs = api_info[unit].db_em_container.nof_em_dbs;
    LogicalTcamInfo* db_info = NULL;
    pemladrv_mem_t *key_write = NULL, *result_write = NULL;
    pemladrv_mem_t *key_read = NULL, *result_read = NULL;
    const int core_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    
    for (db_ndx = 0; db_ndx < nof_dbs; ++db_ndx){
        db_info = &(api_info[unit].db_em_container.db_em_info_arr[db_ndx].logical_em_info);
        nof_entries = db_info->db_tcam_per_core_arr[core_id].total_nof_entries;
        if (!dnx_pemladrv_is_db_mapped_to_pes(db_info->db_tcam_per_core_arr[core_id].result_chunk_mapper_matrix_arr))
            continue;
        
        SHR_IF_ERR_EXIT(dnx_pemladrv_create_pemladrv_mem_struct_db_em_lpm(unit, &key_write, &result_write, db_info));
        SHR_IF_ERR_EXIT(dnx_pemladrv_create_pemladrv_mem_struct_db_em_lpm(unit, &key_read, &result_read, db_info));
        dnx_pemladrv_set_mbist_value_to_field_db_em_lpm(key_read, result_read, db_info, 0x0);
        for (entry_ndx = 0; entry_ndx < nof_entries; ++entry_ndx){
            
            for (mbist_val_ndx = 0; mbist_val_ndx < nof_mbist_values; ++mbist_val_ndx){
                dnx_pemladrv_set_mbist_value_to_field_db_em_lpm(key_write, result_write, db_info, get_mbist_val_by_ndx(mbist_val_ndx));
                SHR_IF_ERR_EXIT(pemladrv_em_entry_set_by_id(unit, core_id, db_ndx, entry_ndx, key_write, result_write));
                SHR_IF_ERR_EXIT(pemladrv_em_entry_get_by_id(unit, core_id, db_ndx, entry_ndx, key_read, result_read));
                if (dnx_pemladrv_compare_pemladrv_mem_structs_db_em_lpm(key_write, key_read, result_write, result_read, db_info)) {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "");
                }

                SHR_IF_ERR_EXIT(pemladrv_em_remove_by_index(unit, core_id, db_ndx, entry_ndx));
            }
        }
    }

exit:
    dnx_pemladrv_free_pemladrv_mem_struct(&key_write);
    dnx_pemladrv_free_pemladrv_mem_struct(&result_write);
    dnx_pemladrv_free_pemladrv_mem_struct(&key_read);
    dnx_pemladrv_free_pemladrv_mem_struct(&result_read);
    SHR_FUNC_EXIT;
}

shr_error_e dnx_pemladrv_pem_mem_lpm_db_virtual_bist(int unit) {
    unsigned int db_ndx, entry_ndx, nof_entries, mbist_val_ndx;
    unsigned int nof_dbs = api_info[unit].db_lpm_container.nof_lpm_dbs;
    LogicalTcamInfo *db_info = NULL;
    pemladrv_mem_t *key_write = NULL, *result_write = NULL;
    pemladrv_mem_t *key_read = NULL, *result_read = NULL;
    int length_in_bits_write, length_in_bits_read;
    const int core_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    
    for (db_ndx = 0; db_ndx < nof_dbs; ++db_ndx) {
        db_info = &(api_info[unit].db_lpm_container.db_lpm_info_arr[db_ndx].logical_lpm_info);
        nof_entries = db_info->db_tcam_per_core_arr[core_id].total_nof_entries;
        length_in_bits_write = db_info->db_tcam_per_core_arr[core_id].total_key_width;
        if (!dnx_pemladrv_is_db_mapped_to_pes(db_info->db_tcam_per_core_arr[core_id].result_chunk_mapper_matrix_arr))
            continue;
        
        SHR_IF_ERR_EXIT(dnx_pemladrv_create_pemladrv_mem_struct_db_em_lpm(unit, &key_write, &result_write, db_info));

        SHR_IF_ERR_EXIT(dnx_pemladrv_create_pemladrv_mem_struct_db_em_lpm(unit, &key_read, &result_read, db_info));

        dnx_pemladrv_set_mbist_value_to_field_db_em_lpm(key_read, result_read, db_info, 0x0);
        for (entry_ndx = 0; entry_ndx < nof_entries; ++entry_ndx) {
            
            for (mbist_val_ndx = 0; mbist_val_ndx < nof_mbist_values; ++mbist_val_ndx) {
                dnx_pemladrv_set_mbist_value_to_field_db_em_lpm(key_write, result_write, db_info,
                                                                get_mbist_val_by_ndx(mbist_val_ndx));
                SHR_IF_ERR_EXIT(dnx_pem_lpm_set_by_id(unit, core_id, db_ndx, entry_ndx, key_write, &length_in_bits_write, result_write));
                SHR_IF_ERR_EXIT(dnx_pem_lpm_get_by_id(unit, core_id, db_ndx, entry_ndx, key_read, &length_in_bits_read, result_read));
                if (dnx_pemladrv_compare_pemladrv_mem_structs_db_em_lpm(key_write, key_read, result_write, result_read, db_info)) {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "");
                }

                SHR_IF_ERR_EXIT(pemladrv_lpm_remove_by_index(unit, core_id, db_ndx, entry_ndx));
            }
        }
    }

exit:
    dnx_pemladrv_free_pemladrv_mem_struct(&key_write);
    dnx_pemladrv_free_pemladrv_mem_struct(&result_write);
    dnx_pemladrv_free_pemladrv_mem_struct(&key_read);
    dnx_pemladrv_free_pemladrv_mem_struct(&result_read);
    SHR_FUNC_EXIT;
}





shr_error_e dnx_pemladrv_pem_mem_reg_virtual_bist(int unit){
    unsigned int reg_ndx, mbist_val_ndx, nof_fields;
    unsigned int nof_regs = api_info[unit].reg_container.nof_registers;
    LogicalRegInfo* reg_info = NULL;
    FieldBitRange* bit_range_arr = NULL;
    pemladrv_mem_t *data_write = NULL, *data_read = NULL, *mask = NULL;

    SHR_FUNC_INIT_VARS(unit);

    
    for (reg_ndx = 0; reg_ndx < nof_regs; ++reg_ndx){
        for (int core_id=0; core_id<PEM_CFG_API_NOF_CORES_DYNAMIC; ++core_id) {
            
            reg_info = &(api_info[unit].reg_container.reg_info_arr[reg_ndx]);
            
            if (!reg_info->is_mapped)
                continue;
            nof_fields = reg_info->nof_fields;
            
            bit_range_arr = reg_info->reg_field_bit_range_arr;
            
            SHR_IF_ERR_EXIT(dnx_pemladrv_allocate_pemladrv_mem_struct(unit, &data_write, nof_fields, bit_range_arr));

            SHR_IF_ERR_EXIT(dnx_pemladrv_allocate_pemladrv_mem_struct(unit, &data_read, nof_fields, bit_range_arr));

            SHR_IF_ERR_EXIT(dnx_pemladrv_allocate_pemladrv_mem_struct(unit, &mask, nof_fields, bit_range_arr));

            dnx_pemladrv_set_pem_mem_access_with_value(data_read, nof_fields, bit_range_arr, 0x0);
            
            for (mbist_val_ndx = 0; mbist_val_ndx < nof_mbist_values; ++mbist_val_ndx) {
                dnx_pemladrv_set_pem_mem_access_with_value(data_write, nof_fields, bit_range_arr,
                                                           get_mbist_val_by_ndx(mbist_val_ndx));
                dnx_pemladrv_set_pem_mem_access_with_value(mask, nof_fields, bit_range_arr, 0x0);
                SHR_IF_ERR_EXIT(pemladrv_reg_write(unit, core_id, reg_ndx, data_write));
                SHR_IF_ERR_EXIT(dnx_pemladrv_reg_read_and_retreive_read_bits_mask(unit, core_id, reg_ndx, data_read, mask));
                mask_reg_read(data_write, mask, nof_fields, bit_range_arr);
                if (dnx_pemladrv_compare_pem_mem_access(data_write, data_read, nof_fields, bit_range_arr)) {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "");
                }
            }
        }
    }

exit:
    if(data_write)
        dnx_pemladrv_free_pemladrv_mem_struct(&data_write);
    if(data_read)
        dnx_pemladrv_free_pemladrv_mem_struct(&data_read);
    if(mask)
        dnx_pemladrv_free_pemladrv_mem_struct(&mask);
    SHR_FUNC_EXIT;
}


shr_error_e dnx_pemladrv_pem_mem_virtual_bist(int unit){
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pemladrv_pem_mem_direct_db_virtual_bist(unit));
    SHR_IF_ERR_EXIT(dnx_pemladrv_pem_mem_tcam_db_virtual_bist(unit));
    SHR_IF_ERR_EXIT(dnx_pemladrv_pem_mem_em_db_virtual_bist(unit));
    SHR_IF_ERR_EXIT(dnx_pemladrv_pem_mem_lpm_db_virtual_bist(unit));
    SHR_IF_ERR_EXIT(dnx_pemladrv_pem_mem_reg_virtual_bist(unit));

exit:
    SHR_FUNC_EXIT;
}

void print_debug_pem_read_write(
        const char *prefix,
        const unsigned int address,
        const unsigned int length_in_bits,
        const unsigned int *value) {

    unsigned int nof_hex_chars;
    unsigned int mask, char_index, curr_char_loc, curr_value_ndx, curr_char_value;
    unsigned int last_char_mask = (length_in_bits % 4 == 0) ? 0xf : ((1 << (length_in_bits % 4)) - 1); 
    char* hex_value = NULL;
    char curr_char;


    nof_hex_chars = (length_in_bits + 3) / 4;
    hex_value = (char*)sal_alloc(sizeof(char)*nof_hex_chars + 1, "hex_value");
    mask = 0xf;
    curr_char_loc = nof_hex_chars-1;
    curr_value_ndx = 0;


    
    for (char_index = 0; char_index < nof_hex_chars; ++char_index, --curr_char_loc){
        
        curr_value_ndx = char_index / 8;
        
        if (char_index % 8 == 0)
            mask = 0xf;
        
        curr_char_value = (value[curr_value_ndx] & mask) >> (char_index%8 * 4);
        
        if (char_index == nof_hex_chars-1)
            curr_char_value &= last_char_mask;
        
        curr_char = (curr_char_value < 10) ? (char)curr_char_value + '0' :
                    (char)curr_char_value + 'a' - 10;
        hex_value[curr_char_loc] = curr_char;
        mask = mask << 4;
    }
    hex_value[nof_hex_chars] = '\0';
    
    
    SHR_FREE(hex_value);
}


