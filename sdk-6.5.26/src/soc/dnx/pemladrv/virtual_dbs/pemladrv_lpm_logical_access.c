
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved. 
 */
/* *INDENT-OFF* */
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "../pemladrv_logical_access.h"
#include "pemladrv_tcam_logical_access.h"
#include "pemladrv_lpm_logical_access.h"
#include "../pemladrv_meminfo_init.h"
#include "../pemladrv_meminfo_access.h"
#include "../pemladrv_cfg_api_defines.h"

#ifdef BCM_DNX_SUPPORT
#include <shared/utilex/utilex_integer_arithmetic.h>
#endif

#ifdef BCM_DNX_SUPPORT
#define BSL_LOG_MODULE BSL_LS_SOCDNX_ADAPTERDNX
#endif

extern ApiInfo api_info[MAX_NOF_UNITS];



shr_error_e dnx_pemladrv_update_lpm_cache_from_physical(const int unit) {
    unsigned int index, entry, total_key_width, total_result_width, chunk_matrix_row_index, valid, prefix_length;
    unsigned int *tcam_data = NULL, *key_arr = NULL, *result_arr = NULL;
    pemladrv_mem_t *key = NULL, *result = NULL; 

    SHR_FUNC_INIT_VARS(unit);

    for (int core_id=0; core_id<PEM_CFG_API_NOF_CORES_DYNAMIC; ++core_id) {
        int sub_unit_idx = PEM_CFG_CORE_ID_TO_SUB_UNIT_ID(core_id);
        for (index = 0; index < api_info[unit].db_lpm_container.nof_lpm_dbs; ++index) {
            if (NULL ==
                api_info[unit].db_lpm_container.db_lpm_info_arr[index].logical_lpm_info.db_tcam_per_core_arr[core_id].result_chunk_mapper_matrix_arr)     
                continue;
            
            total_key_width = api_info[unit].db_lpm_container.db_lpm_info_arr[index].logical_lpm_info.db_tcam_per_core_arr[core_id].total_key_width;
            total_result_width = api_info[unit].db_lpm_container.db_lpm_info_arr[index].logical_lpm_info.db_tcam_per_core_arr[core_id].total_result_width;
            SHR_ALLOC_SET_ZERO_ERR_EXIT(tcam_data, UTILEX_DIV_ROUND_UP(2 * total_key_width + 1, SAL_UINT32_NOF_BITS)* sizeof(uint32), "tcam_data", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

            SHR_IF_ERR_EXIT(pemladrv_mem_alloc_lpm(unit, index, &key, &result));
            for (entry = 0; entry < PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES; ++entry) {
                chunk_matrix_row_index = entry / PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES;
                SHR_ALLOC_SET_ZERO_ERR_EXIT(result_arr, UTILEX_DIV_ROUND_UP(total_result_width, SAL_UINT32_NOF_BITS)* sizeof(uint32), "result_arr", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

                SHR_IF_ERR_EXIT(dnx_build_physical_db_key_data_array(unit, sub_unit_idx,
                                                     api_info[unit].db_lpm_container.db_lpm_info_arr[index].logical_lpm_info.db_tcam_per_core_arr[core_id].key_chunk_mapper_matrix_arr,
                                                     dnx_get_nof_tcam_chunk_cols(total_key_width),
                                                     chunk_matrix_row_index,
                                                     entry,
                                                     total_key_width,
                                                     tcam_data));

                valid =
                  (tcam_data[UTILEX_DIV_ROUND_UP(2 * total_key_width + 1, SAL_UINT32_NOF_BITS) - 1]) >>
                  ((2 * total_key_width + 1) % UINT_WIDTH_IN_BITS - 1);

                
                if (!valid) {
                    SHR_FREE(result_arr);
                    continue;
                }
                else {
                    SHR_IF_ERR_EXIT(dnx_build_physical_db_result_data_array(unit, sub_unit_idx,
                                                            api_info[unit].db_lpm_container.db_lpm_info_arr[index].logical_lpm_info.db_tcam_per_core_arr[core_id].result_chunk_mapper_matrix_arr,
                                                            dnx_get_nof_cam_sram_chunk_cols(total_result_width),
                                                            chunk_matrix_row_index,
                                                            entry,
                                                            api_info[unit].db_lpm_container.db_lpm_info_arr[index].logical_lpm_info.db_tcam_per_core_arr[core_id].result_field_bit_range_arr,
                                                            result,
                                                            result_arr));
                    SHR_IF_ERR_EXIT(dnx_convert_tcam_data_into_key_array(unit, total_key_width, tcam_data, &key_arr));
                    SHR_IF_ERR_EXIT(dnx_get_prefix_length_from_physical_data(unit, total_key_width, tcam_data, &prefix_length));
                    dnx_pemladrv_update_lpm_cache(core_id, entry, prefix_length, key_arr, result_arr, &api_info[unit].db_lpm_container.db_lpm_info_arr[index]);
                    SHR_FREE(key_arr);
                    SHR_FREE(result_arr);
                }
            }
            SHR_FREE(tcam_data);
            pemladrv_mem_free(key);
            pemladrv_mem_free(result);
        }
    }

exit:
    SHR_FREE(key_arr);
    SHR_FREE(tcam_data);
    pemladrv_mem_free(key);
    pemladrv_mem_free(result);
    SHR_FREE(result_arr);
    SHR_FUNC_EXIT;
}


shr_error_e dnx_pemladrv_lpm_info_allocate(const int unit, pemladrv_db_info** arr, int* size) {
    int mapped_lpm_size = 0;
    int name_len;
    unsigned int lpm_index, field_index, nof_key_fields, nof_fields;
    const int core_id = 0;
    FieldBitRange *lpm_field_key_info_arr, *lpm_field_result_info_arr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(*arr, api_info[unit].db_lpm_container.nof_lpm_dbs* sizeof(pemladrv_db_info), "arr", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    for (lpm_index = 0; lpm_index < api_info[unit].db_lpm_container.nof_lpm_dbs; ++lpm_index)
    {
        LogicalLpmInfo* lpm_info_arr = &api_info[unit].db_lpm_container.db_lpm_info_arr[lpm_index];
        
        if (api_info[unit].db_lpm_container.db_lpm_info_arr == NULL || api_info[unit].db_lpm_container.db_lpm_info_arr[lpm_index].logical_lpm_info.db_tcam_per_core_arr[core_id].result_chunk_mapper_matrix_arr == NULL)
        {
            continue;
        }

        (*arr)[mapped_lpm_size].db_id = lpm_index;
        name_len = sal_strnlen(api_info[unit].db_lpm_container.db_lpm_info_arr[lpm_index].logical_lpm_info.db_tcam_per_core_arr[core_id].name, MAX_NAME_LENGTH);
        SHR_ALLOC_SET_ZERO_ERR_EXIT((*arr)[mapped_lpm_size].db_name, name_len + 1, "(*arr)[].db_name", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        sal_strncpy((*arr)[mapped_lpm_size].db_name, api_info[unit].db_lpm_container.db_lpm_info_arr[lpm_index].logical_lpm_info.db_tcam_per_core_arr[core_id].name, name_len);
        (*arr)[mapped_lpm_size].db_name[name_len] = '\0';
        nof_key_fields = api_info[unit].db_lpm_container.db_lpm_info_arr[lpm_index].logical_lpm_info.db_tcam_per_core_arr[core_id].nof_fields_in_key;

        nof_fields = nof_key_fields;

        (*arr)[mapped_lpm_size].nof_fields = nof_fields;

        SHR_ALLOC_SET_ZERO_ERR_EXIT((*arr)[mapped_lpm_size].ptr, (*arr)[mapped_lpm_size].nof_fields* sizeof(pemladrv_db_field_info), "(*arr)[].ptr", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        for (field_index = 0; field_index < nof_fields; ++field_index) {
            lpm_field_key_info_arr = &lpm_info_arr->logical_lpm_info.db_tcam_per_core_arr[core_id].key_field_bit_range_arr[field_index];

            if (lpm_field_key_info_arr->is_field_mapped) {
                (*arr)[mapped_lpm_size].ptr[field_index].is_field_mapped = lpm_field_key_info_arr->is_field_mapped;
                (*arr)[mapped_lpm_size].ptr[field_index].field_id = field_index;
                (*arr)[mapped_lpm_size].ptr[field_index].field_size_in_bits =
                        lpm_field_key_info_arr->msb - lpm_field_key_info_arr->lsb + 1;

                (*arr)[mapped_lpm_size].total_nof_entries = api_info[unit].db_lpm_container.db_lpm_info_arr[lpm_index].logical_lpm_info.db_tcam_per_core_arr->total_nof_entries;
                (*arr)[mapped_lpm_size].total_key_width = api_info[unit].db_lpm_container.db_lpm_info_arr[lpm_index].logical_lpm_info.db_tcam_per_core_arr->total_key_width;
            }

            lpm_field_result_info_arr = &lpm_info_arr->logical_lpm_info.db_tcam_per_core_arr[core_id].result_field_bit_range_arr[field_index];

            if (lpm_field_result_info_arr->is_field_mapped)
            {
                (*arr)[mapped_lpm_size].ptr[field_index].is_field_mapped = lpm_field_result_info_arr->is_field_mapped;
                (*arr)[mapped_lpm_size].ptr[field_index].field_size_in_bits += lpm_field_result_info_arr->msb - lpm_field_result_info_arr->lsb + 1;
                (*arr)[mapped_lpm_size].total_result_width = api_info[unit].db_lpm_container.db_lpm_info_arr[lpm_index].logical_lpm_info.db_tcam_per_core_arr->total_result_width;
            }
        }


        mapped_lpm_size++;
    }


    *size = mapped_lpm_size;
exit:
    SHR_FUNC_EXIT;
}

void dnx_pemladrv_lpm_info_de_allocate(const int unit, pemladrv_db_info* ptr){
    pemladrv_db_info temp;
    if(ptr)
    {
        for (unsigned int i = 0; i < api_info[unit].db_lpm_container.nof_lpm_dbs; ++i){
            temp = ptr[i];
            SHR_FREE(temp.ptr);
            SHR_FREE(temp.db_name);
        }
        SHR_FREE(ptr);
    }
}






shr_error_e pemladrv_lpm_insert_inner(
        const int unit,
        const int core_id,
        table_id_t table_id,
        pemladrv_mem_t * key,
        int length_in_bits,
        pemladrv_mem_t * result,
        int *index) {
    int shift_direction;
    unsigned int nof_fields_in_key = api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].logical_lpm_info.db_tcam_per_core_arr[core_id].nof_fields_in_key;
    const unsigned int total_key_width_in_bits = api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].logical_lpm_info.db_tcam_per_core_arr[core_id].total_key_width;
    const FieldBitRange *const key_fields_arr = api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].logical_lpm_info.db_tcam_per_core_arr[core_id].key_field_bit_range_arr;
    unsigned int uint_len_in_bits = length_in_bits;
    unsigned int *virtual_key_data = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(virtual_key_data, (size_t) pemladrv_ceil(total_key_width_in_bits, UINT_WIDTH_IN_BITS) *sizeof(uint32), "virtual_key_data", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    
    if (uint_len_in_bits >
        api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].logical_lpm_info.db_tcam_per_core_arr[core_id].total_key_width) {
        SHR_ERR_EXIT(_SHR_E_FULL, "ERROR. Wrong prefix in: Exact-Match - #%d id\n", table_id);
    }


    
    if (key->nof_fields != nof_fields_in_key) {
        SHR_ERR_EXIT(_SHR_E_FULL, "ERROR. One or More fields are missing while trying to write key. Exact-Match - #%d id", table_id);
    }

    
    dnx_build_virtual_db_data(0, key_fields_arr, key,
                              virtual_key_data);    
    dnx_pemladrv_update_key_with_prefix_length(length_in_bits, total_key_width_in_bits, virtual_key_data);

    SHR_IF_ERR_EXIT(dnx_pemladrv_does_key_exists_in_physical_lpm_db(
            unit,
            &api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].logical_lpm_info,
            uint_len_in_bits,
            &api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].lpm_cache[core_id],
            virtual_key_data,
            index));
    if (*index != -1) {
        
        SHR_IF_ERR_EXIT(dnx_pem_lpm_set_by_id(unit, core_id, table_id, *index, key, &length_in_bits,
                                     result));
        SHR_EXIT();
    }

    
    if (dnx_pemladrv_is_lpm_full(unit,
                                 api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].logical_lpm_info.db_tcam_per_core_arr[core_id].total_nof_entries,
                                 api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].lpm_cache[core_id].physical_entry_occupation)) {
        SHR_ERR_EXIT(_SHR_E_FULL, "The DB is already full, Can't write or add keys.");
    }

    
    *index = dnx_pemladrv_find_lpm_index_to_insert(unit,
                                                   length_in_bits, total_key_width_in_bits,
                                                   &api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].lpm_cache[core_id]);

    
    if (api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].lpm_cache[core_id].physical_entry_occupation[*index] == 0) {
        SHR_IF_ERR_EXIT(dnx_pem_lpm_set_by_id(unit, core_id, table_id, *index, key, &length_in_bits, result));
    }


    shift_direction = dnx_pemladrv_find_minimum_shifting_direction(unit, core_id, *index, table_id);

    SHR_IF_ERR_EXIT(dnx_pemladrv_shift_physical_to_clear_entry(unit, core_id, shift_direction, index, table_id));
    SHR_IF_ERR_EXIT(dnx_pem_lpm_set_by_id(unit, core_id, table_id, *index, key, &length_in_bits, result));

exit:
    SHR_FREE(virtual_key_data);
    SHR_FUNC_EXIT;
}


shr_error_e pemladrv_lpm_insert(
        const int unit,
        const int core_id,
        table_id_t table_id,
        pemladrv_mem_t * key,
        int length_in_bits,
        pemladrv_mem_t * result,
        int *index) {

    SHR_FUNC_INIT_VARS(unit);

    if (core_id==-1 ){
        for (int core_i=0; core_i<PEM_CFG_API_NOF_CORES_DYNAMIC; ++core_i){
            SHR_IF_ERR_EXIT(pemladrv_lpm_insert_inner(unit, core_i, table_id, key, length_in_bits, result, index));
        }
    }
    else{
        SHR_IF_ERR_EXIT(pemladrv_lpm_insert_inner(unit, core_id, table_id, key, length_in_bits, result, index));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e pemladrv_lpm_remove(
        const int unit,
        const int core_id,
        table_id_t table_id,
        pemladrv_mem_t * key,
        int length_in_bits,
        int *index)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pemladrv_find_physical_entry_index_by_key(unit, core_id, &api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].logical_lpm_info, api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].lpm_cache[core_id].physical_entry_occupation, api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].lpm_cache[core_id].lpm_key_entry_arr, key, index));
    if (*index == -1) {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Key doesn't exist in DB");
    }
    SHR_IF_ERR_EXIT(pemladrv_lpm_remove_by_index(unit, core_id, table_id, *index));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e pemladrv_lpm_lookup(
        const int unit,
        const int core_id,
        table_id_t table_id,
        pemladrv_mem_t * key,
        pemladrv_mem_t * result,
        int *index)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pemladrv_find_physical_lpm_entry_index_by_key(unit, core_id, &api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].logical_lpm_info, &api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].lpm_cache[core_id], key, index));
    if (*index == -1) {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Key doesn't exist in DB");
    }
    dnx_pemladrv_get_lpm_data_by_entry(unit, core_id, table_id, *index, key, result);

exit:
    SHR_FUNC_EXIT;
}


shr_error_e dnx_pem_lpm_get_by_id(
        const int unit,
        const int core_id,
        table_id_t table_id,
        int index,
        pemladrv_mem_t * key,
        int *length_in_bits,
        pemladrv_mem_t * result)
{
    SHR_FUNC_INIT_VARS(unit);

    if (index == -1) {
        
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid entry\n");
    }
    *length_in_bits = api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].lpm_cache[core_id].lpm_key_entry_prefix_length_arr[index];
    dnx_pemladrv_get_lpm_data_by_entry(unit, core_id, table_id, index, key, result);

exit:
    SHR_FUNC_EXIT;
}


shr_error_e dnx_pem_lpm_set_by_id(
        const int unit,
        const int core_id,
        table_id_t table_id,
        int index,
        pemladrv_mem_t * key,
        int *length_in_bits,
        pemladrv_mem_t * result)
{

    const FieldBitRange* key_fields_arr = NULL;
    const FieldBitRange* result_fields_arr = NULL;
    unsigned int         total_key_width_in_bits;
    unsigned int         total_result_width_in_bits;
    unsigned int*        virtual_key_data = NULL;
    unsigned int*        virtual_result_data = NULL;
    pemladrv_mem_t     *mask = NULL, *valid = NULL;
    unsigned int len_in_bits_uint = *length_in_bits;

    SHR_FUNC_INIT_VARS(unit);

    
    if(len_in_bits_uint > api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].logical_lpm_info.db_tcam_per_core_arr[core_id].total_key_width) {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ERROR. Wrong prefix in: Exact-Match - #%d id\n", table_id);
    }

    if (index < 0 || index > 31) {
        
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Invalid entry");
    }

    

    key_fields_arr              = api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].logical_lpm_info.db_tcam_per_core_arr[core_id].key_field_bit_range_arr;
    result_fields_arr           = api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].logical_lpm_info.db_tcam_per_core_arr[core_id].result_field_bit_range_arr;
    total_key_width_in_bits     = api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].logical_lpm_info.db_tcam_per_core_arr[core_id].total_key_width;
    total_result_width_in_bits  = api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].logical_lpm_info.db_tcam_per_core_arr[core_id].total_result_width;

    SHR_ALLOC_SET_ZERO_ERR_EXIT(virtual_key_data, (size_t)pemladrv_ceil(total_key_width_in_bits,  UINT_WIDTH_IN_BITS)* sizeof(uint32), "virtual_key_data", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO_ERR_EXIT(virtual_result_data, (size_t)pemladrv_ceil(total_result_width_in_bits,  UINT_WIDTH_IN_BITS)* sizeof(uint32), "virtual_result_data", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    dnx_build_virtual_db_data(0, key_fields_arr, key, virtual_key_data);
    dnx_build_virtual_db_data(0, result_fields_arr, result, virtual_result_data);
    dnx_pemladrv_update_key_with_prefix_length(*length_in_bits, total_key_width_in_bits, virtual_key_data);

    

    SHR_IF_ERR_EXIT(dnx_pemladrv_build_lpm_mask(unit, key_fields_arr, *length_in_bits, total_key_width_in_bits, key, &mask));
    SHR_IF_ERR_EXIT(dnx_pemladrv_build_tcam_valid(unit, &valid));
    dnx_set_pem_mem_accesss_fldbuf(0, virtual_key_data, key_fields_arr, key);

    SHR_IF_ERR_EXIT(dnx_logical_cam_based_db_write(&api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].logical_lpm_info, unit, core_id, index, key, mask, valid, result));
    dnx_pemladrv_update_lpm_cache(core_id, index, *length_in_bits, virtual_key_data, virtual_result_data, &api_info[unit].db_lpm_container.db_lpm_info_arr[table_id]);

exit:
    dnx_pemladrv_free_pemladrv_mem_struct(&mask);
    dnx_pemladrv_free_pemladrv_mem_struct(&valid);
    SHR_FREE(virtual_key_data);
    SHR_FREE(virtual_result_data);
    SHR_FUNC_EXIT;
}


shr_error_e pemladrv_lpm_remove_all_inner(
        const int unit,
        const int core_id,
        table_id_t table_id)
{
    unsigned int nof_entries = PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES;
    unsigned int index;

    SHR_FUNC_INIT_VARS(unit);

    for (index = 0; index < nof_entries; ++index) {
        if (api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].lpm_cache[core_id].physical_entry_occupation[index] == 1) {
            SHR_IF_ERR_EXIT(pemladrv_lpm_remove_by_index(unit, core_id, table_id, index));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e pemladrv_lpm_remove_all(
        const int unit,
        const int core_id,
        table_id_t table_id){

    SHR_FUNC_INIT_VARS(unit);

    if (core_id == -1){
        for (int core_i=0; core_i<PEM_CFG_API_NOF_CORES_DYNAMIC; ++core_i){
            SHR_IF_ERR_EXIT(pemladrv_lpm_remove_all_inner(unit, core_i, table_id));
        }
    }
    else{
        SHR_IF_ERR_EXIT(pemladrv_lpm_remove_all_inner(unit, core_id, table_id));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e pemladrv_lpm_remove_by_index(
        const int unit,
        const int core_id,
        table_id_t table_id,
        int index) {
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pemladrv_set_valid_bit_to_zero(unit, core_id, index,
                                       &api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].logical_lpm_info));

    
    api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].lpm_cache[core_id].physical_entry_occupation[index] = 0;

exit:
    SHR_FUNC_EXIT;
}









shr_error_e dnx_pemladrv_does_key_exists_in_physical_lpm_db(
        const int unit,
        const LogicalTcamInfo * tcam_info,
        const unsigned int prefix_length_in_bits,
        const LpmDbCache * lpm_cache,
        const unsigned int *key,
        int *index)
{
    const int core_id = 0; 
    unsigned int entry_index, i;
    unsigned int key_width = tcam_info->db_tcam_per_core_arr[core_id].total_key_width;
    unsigned int key_chunks = dnx_pemladrv_calc_nof_arr_entries_from_width(key_width);
    unsigned int *temp_key_data = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(temp_key_data, (size_t)pemladrv_ceil(key_width, UINT_WIDTH_IN_BITS)* sizeof(uint32), "temp_key_data", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    for (entry_index = 0; entry_index < PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES; ++entry_index) {
        if ( 1 == lpm_cache->physical_entry_occupation[entry_index] ) {
            for (i = 0; i < key_chunks; ++i){
                temp_key_data[i] = key[i];
            }
            dnx_pemladrv_update_key_with_prefix_length(lpm_cache->lpm_key_entry_prefix_length_arr[entry_index], key_width, temp_key_data);
            if (dnx_pemladrv_copmare_two_virtual_keys(key_width, lpm_cache->lpm_key_entry_arr[entry_index], temp_key_data) && (lpm_cache->lpm_key_entry_prefix_length_arr[entry_index] == prefix_length_in_bits)){
                *index =  entry_index;
                SHR_EXIT();
            }
        }
    }
exit:
    SHR_FREE(temp_key_data);
    SHR_FUNC_EXIT;
}


shr_error_e dnx_pemladrv_build_lpm_mask(
        int unit,
        const FieldBitRange * key_field_bit_range_arr,
        int prefix_length_in_bits,
        const int key_length_in_bits,
        pemladrv_mem_t * key,
        pemladrv_mem_t** mask)
{
    int field_index, nof_bits_to_mask;
    unsigned int i;
    unsigned int *virtual_mask = NULL;
    unsigned int num_of_chunks_in_fldbuf;
    unsigned int num_of_uint_chunks_in_mask;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(*mask, sizeof(pemladrv_mem_t), "mask", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    
    num_of_uint_chunks_in_mask =  dnx_pemladrv_get_nof_unsigned_int_chunks(key_length_in_bits);
    SHR_ALLOC_SET_ZERO_ERR_EXIT(virtual_mask, num_of_uint_chunks_in_mask* sizeof(unsigned int), "virtual_mask", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    for(i = 0; i < num_of_uint_chunks_in_mask; ++i)
        virtual_mask[i] = 0;
    i = 0;
    nof_bits_to_mask = key_length_in_bits - prefix_length_in_bits;
    while (nof_bits_to_mask > 0) {
        if (nof_bits_to_mask >= 32) {
            virtual_mask[i] = (unsigned)-1;
            nof_bits_to_mask -= 32;
            ++i;
        }
        else {
            virtual_mask[i] =(( 1 << nof_bits_to_mask ) - 1);
            break;
        }
    }
    
    (*mask)->nof_fields = key->nof_fields;
    (*mask)->flags   = key->flags;
    SHR_ALLOC_SET_ZERO_ERR_EXIT((*mask)->fields, (*mask)->nof_fields* sizeof(pemladrv_field_t*), "(*mask)->fields", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    for (field_index = 0; field_index < (*mask)->nof_fields; ++field_index) {
        SHR_ALLOC_SET_ZERO_ERR_EXIT((*mask)->fields[field_index], sizeof(pemladrv_field_t), "(*mask)->fields[]", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        (*mask)->fields[field_index]->field_id  = key->fields[field_index]->field_id;  
        (*mask)->fields[field_index]->flags  = key->fields[field_index]->flags;

        num_of_chunks_in_fldbuf           = dnx_pemladrv_get_nof_chunks_in_fldbuf(field_index, key_field_bit_range_arr);
        SHR_ALLOC_SET_ZERO_ERR_EXIT((*mask)->fields[field_index]->fldbuf, num_of_chunks_in_fldbuf*sizeof(unsigned int), "(*mask)->fields[]->fldbuf", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    }

    dnx_set_pem_mem_accesss_fldbuf(0, virtual_mask, key_field_bit_range_arr, (*mask));

exit:
    SHR_FREE(virtual_mask);
    SHR_FUNC_EXIT;
}



void dnx_pemladrv_update_lpm_cache(
        const int core_id,
        unsigned int row_index,
        const int prefix_length,
        unsigned int *virtual_key_data,
        unsigned int *virtual_result_data,
        LogicalLpmInfo * lpm_cache) {
    lpm_cache->lpm_cache[core_id].lpm_key_entry_arr[row_index]               = virtual_key_data;
    lpm_cache->lpm_cache[core_id].lpm_result_entry_arr[row_index]            = virtual_result_data;
    lpm_cache->lpm_cache[core_id].physical_entry_occupation[row_index]       = 1;
    lpm_cache->lpm_cache[core_id].lpm_key_entry_prefix_length_arr[row_index] = prefix_length;
}


int dnx_pemladrv_find_next_lpm_free_entry(
        const int unit,
        const int core_id,
        const int index,
        const int direction,
        unsigned char *physical_entry_occupation) {

    int next_free_index;
    if(direction == UP) {
        for(next_free_index = index - 1; next_free_index >= 0; --next_free_index) {
            if(physical_entry_occupation[next_free_index] == 0)
                return next_free_index;
        }
        return -1;
    }

    else if(direction == DOWN) {
        for(next_free_index = index + 1; next_free_index < PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES; ++next_free_index) {
            if(physical_entry_occupation[next_free_index] == 0)
                return next_free_index;
        }
        return -1;
    }
    return -1; 
}



shr_error_e dnx_pemladrv_find_physical_lpm_entry_index_by_key(
        const int unit,
        const int core_id,
        const LogicalTcamInfo * tcam_info,
        const LpmDbCache * lpm_cache,
        const pemladrv_mem_t * key,
        int *index) {
    unsigned int entry_index, i;
    FieldBitRange* key_field_bit_rang_arr = tcam_info->db_tcam_per_core_arr[core_id].key_field_bit_range_arr;
    unsigned int   total_key_width        = tcam_info->db_tcam_per_core_arr[core_id].total_key_width;

    unsigned int*  virtual_key_data = NULL;

    unsigned int   key_chunks             = dnx_pemladrv_calc_nof_arr_entries_from_width(total_key_width);

    SHR_FUNC_INIT_VARS(unit);

    *index = -1;

    SHR_ALLOC_SET_ZERO_ERR_EXIT(virtual_key_data, (size_t)pemladrv_ceil(total_key_width, UINT_WIDTH_IN_BITS)* sizeof(uint32), "virtual_key_data", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    dnx_build_virtual_db_data(0, key_field_bit_rang_arr, key, virtual_key_data);

    for (entry_index = 0; entry_index < PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES; ++entry_index) {
        if ( 1 == lpm_cache->physical_entry_occupation[entry_index] ) {
            for (i = 0; i < key_chunks; ++i)
                virtual_key_data[i] = lpm_cache->lpm_key_entry_arr[entry_index][i];
            dnx_pemladrv_update_key_with_prefix_length(lpm_cache->lpm_key_entry_prefix_length_arr[entry_index], total_key_width, virtual_key_data);
            if (dnx_pemladrv_copmare_two_virtual_keys(total_key_width, lpm_cache->lpm_key_entry_arr[entry_index], virtual_key_data)){
                *index = entry_index;
                SHR_EXIT();
            }
        }
    }
exit:
    SHR_FREE(virtual_key_data);
    SHR_FUNC_EXIT;
}



void dnx_pemladrv_get_lpm_data_by_entry(
        const int unit,
        const int core_id,
        const int db_id,
        const int index,
        pemladrv_mem_t * key,
        pemladrv_mem_t * result) {
    dnx_set_pem_mem_accesss_fldbuf_from_physical_sram_data_array(
            api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].lpm_cache[core_id].lpm_result_entry_arr[index],
            api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.db_tcam_per_core_arr[core_id].result_field_bit_range_arr,
            result);
    dnx_set_pem_mem_accesss_fldbuf_from_physical_sram_data_array(
            api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].lpm_cache[core_id].lpm_key_entry_arr[index],
            api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.db_tcam_per_core_arr[core_id].key_field_bit_range_arr,    
            key);
}





int dnx_pemladrv_find_lpm_index_to_insert(
	const int unit,
    const unsigned int prefix_length,
    const int key_length,
    LpmDbCache * lpm_cache) {
  int index;
#ifdef BCM_DNX_SUPPORT
  int a, b, max_p, min_p, curr_p;
#else
  double a, b, max_p, min_p, curr_p;
#endif 
  curr_p = prefix_length;
  a = 0;
  b = PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES - 1;
  min_p = 0;
  max_p = key_length;

  for(index = 0; index < PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES - 1; ++index) {
    if (lpm_cache->physical_entry_occupation[index] == 1) {
      if(lpm_cache->lpm_key_entry_prefix_length_arr[index] == prefix_length) {
        return dnx_pemladrv_find_most_suitable_index_with_matching_prefix(unit, index, lpm_cache);
      }
      if(lpm_cache->lpm_key_entry_prefix_length_arr[index] > prefix_length) {
        a = index + 1;
        max_p = lpm_cache->lpm_key_entry_prefix_length_arr[index];
      }
      if(lpm_cache->lpm_key_entry_prefix_length_arr[index] < prefix_length) {
        b = index;
        min_p = lpm_cache->lpm_key_entry_prefix_length_arr[index];
        break;
      }
    }

  }
#ifdef BCM_DNX_SUPPORT
  return b - (b - a) * UTILEX_DIV_ROUND_UP((curr_p - min_p), (max_p - min_p));
#else
  return (int)(b - (size_t)ceil((b-a)*(curr_p - min_p)/(max_p - min_p))); 
#endif 
}


int dnx_pemladrv_find_most_suitable_index_with_matching_prefix(
        const int unit,
        const int index,
        LpmDbCache * lpm_cache) {

    int i;
    int distance_to_free_apace_above = 0;
    int distance_to_free_apace_below = 0;
    int last_index_of_same_prefix_length;
    int same_last_prefix = 1;

    unsigned int prefix_length = lpm_cache->lpm_key_entry_prefix_length_arr[index];

    last_index_of_same_prefix_length = dnx_pemladrv_find_last_index_with_same_prefix(unit, index, lpm_cache->lpm_key_entry_prefix_length_arr);
    

    
    for(i = index - 1; i >= 0 ; --i) {
        ++distance_to_free_apace_above;
        if (lpm_cache->physical_entry_occupation[i] == 0) {  
            if (same_last_prefix) 
                return i;
            break;
        }
        if ((lpm_cache->lpm_key_entry_prefix_length_arr[i] != prefix_length))
            same_last_prefix = 0;
    }
    
    if(i == -1)
        return last_index_of_same_prefix_length + 1;
    
    same_last_prefix = 1;
    for (i = last_index_of_same_prefix_length + 1; i < PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES; ++i) {
        ++distance_to_free_apace_below;
        if (lpm_cache->physical_entry_occupation[i] == 0) { 
            if (same_last_prefix) 
                return i;
            break;
        }
        if (lpm_cache->lpm_key_entry_prefix_length_arr[i] != prefix_length)
            same_last_prefix = 0;
    }
    
    if(i == PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES)
        return index - 1;

    
    if ( distance_to_free_apace_above <= distance_to_free_apace_below)
        return index;
    return last_index_of_same_prefix_length + 1;
}



int dnx_pemladrv_find_last_index_with_same_prefix(
        const int unit,
        const int index,
        unsigned int *lpm_key_entry_prefix_length_arr)
{
    int i;
    for(i = index + 1; i < PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES; ++i) {
        if (lpm_key_entry_prefix_length_arr[i] != lpm_key_entry_prefix_length_arr[index])
            return i - 1;
    }
    return PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES - 1;
}


int dnx_pemladrv_is_lpm_full(
        const int unit,
        const int nof_entries,
        unsigned char *physical_entry_occupation) {
    int is_full = 1;
    int nof_occupied_entries = 0;
    int i;

    for(i = 0; i < PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES; ++i) {
        if (physical_entry_occupation[i] == 1)
            ++nof_occupied_entries;
    }
    if (nof_occupied_entries == nof_entries)
        return is_full;
    else
        return !is_full;
}



int dnx_pemladrv_find_minimum_shifting_direction(
        const int unit,
        const int core_id,
        const int index,
        const int table_id)
{
    int i;
    int distance_to_free_apace_above = 0;
    int distance_to_free_apace_below = 0;
    int last_index_of_same_prefix_length =
            dnx_pemladrv_find_last_index_with_same_prefix(unit,
                                                          index,
                                                          api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].lpm_cache[core_id].lpm_key_entry_prefix_length_arr);
 

    
    for(i = index - 1; i >= 0 ; --i) {
        ++distance_to_free_apace_above;
        if (api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].lpm_cache[core_id].physical_entry_occupation[i] == 0)
            break;
    }
    
    if(i == -1)
        return DOWN;
    
    for(i = last_index_of_same_prefix_length + 1; i < PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES ; ++i) {
        ++distance_to_free_apace_below;
        if (api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].lpm_cache[core_id].physical_entry_occupation[i] == 0)
            break;
    }
    
    if(i == PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES)
        return UP;

    
    if ( distance_to_free_apace_above <= distance_to_free_apace_below)
        return UP;
    return DOWN;
}


shr_error_e dnx_pemladrv_move_physical_lpm_entry(
        const int unit,
        const int core_id,
        const int old_index,
        const int new_index,
        const int table_id) {
    int length_in_bits;
    pemladrv_mem_t *key = NULL;
    pemladrv_mem_t *result = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(key, sizeof(pemladrv_mem_t), "key", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO_ERR_EXIT(result, sizeof(pemladrv_mem_t), "result", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    key->nof_fields = 1;
    SHR_ALLOC_SET_ZERO_ERR_EXIT(key->fields, sizeof(pemladrv_field_t *), "key->fields", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO_ERR_EXIT(key->fields[0], sizeof(pemladrv_field_t), "key->fields[0]", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    key->fields[0]->field_id = 0;
    SHR_ALLOC_SET_ZERO_ERR_EXIT(key->fields[0]->fldbuf, 2*sizeof(unsigned int), "key->fields[0]->fldbuf", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    result->nof_fields = 1;
    SHR_ALLOC_SET_ZERO_ERR_EXIT(result->fields, sizeof(pemladrv_field_t *), "result->fields", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO_ERR_EXIT(result->fields[0], sizeof(pemladrv_field_t), "result->fields[0]", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    result->fields[0]->field_id = 0;
    SHR_ALLOC_SET_ZERO_ERR_EXIT(result->fields[0]->fldbuf, 2*sizeof(unsigned int), "result->fields[0]->fldbuf", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    length_in_bits = api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].lpm_cache[core_id].lpm_key_entry_prefix_length_arr[old_index];


    dnx_set_pem_mem_accesss_fldbuf_from_physical_sram_data_array(
            api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].lpm_cache[core_id].lpm_key_entry_arr[old_index],
            api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].logical_lpm_info.db_tcam_per_core_arr[core_id].key_field_bit_range_arr,
            key);


    dnx_set_pem_mem_accesss_fldbuf_from_physical_sram_data_array(
            api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].lpm_cache[core_id].lpm_result_entry_arr[old_index],
            api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].logical_lpm_info.db_tcam_per_core_arr[core_id].result_field_bit_range_arr,
            result);

    SHR_IF_ERR_EXIT(pemladrv_lpm_remove_by_index(unit, core_id, table_id, old_index));

    SHR_IF_ERR_EXIT(dnx_pem_lpm_set_by_id(unit, core_id, table_id, new_index, key, &length_in_bits, result));

exit:
    if(key)
        dnx_pemladrv_free_pemladrv_mem_struct(&key);
    if(result)
        dnx_pemladrv_free_pemladrv_mem_struct(&result);
    SHR_FUNC_EXIT;
}


shr_error_e dnx_pemladrv_shift_physical_to_clear_entry(
        const int unit,
        const int core_id,
        const int shift_direction,
        int *index,
        const int table_id) {
    int next_free_entry, index_to_write_to;

    SHR_FUNC_INIT_VARS(unit);

    if (shift_direction == UP) {
        *index = *index - 1;                   

        next_free_entry = dnx_pemladrv_find_next_lpm_free_entry(unit, core_id, *index, UP,
                                                                api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].lpm_cache[core_id].physical_entry_occupation);
        for (int core_index = 0; core_index < PEM_CFG_API_NOF_CORES_DYNAMIC; ++core_index) {
            for (index_to_write_to = next_free_entry; index_to_write_to != *index; ++index_to_write_to) {
                SHR_IF_ERR_EXIT(dnx_pemladrv_move_physical_lpm_entry(unit, core_index, index_to_write_to + 1, index_to_write_to,
                                                     table_id));
            }
        }
    } else if (shift_direction == DOWN) {


        next_free_entry = dnx_pemladrv_find_next_lpm_free_entry(
                unit,
                core_id,
                *index,
                DOWN,
                api_info[unit].db_lpm_container.db_lpm_info_arr[table_id].lpm_cache[core_id].physical_entry_occupation);
        for (int core_index = 0; core_index < PEM_CFG_API_NOF_CORES_DYNAMIC; ++core_index) {
            for (index_to_write_to = next_free_entry; index_to_write_to !=
                                                      *index; --index_to_write_to) {    
                SHR_IF_ERR_EXIT(dnx_pemladrv_move_physical_lpm_entry(unit, core_index, index_to_write_to - 1, index_to_write_to,
                                                     table_id));
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "shift_direction (%d) incorect", shift_direction);
    }

exit:
    SHR_FUNC_EXIT;
}




shr_error_e pemladrv_mem_alloc_lpm(
        const int unit,
        const int db_id,
        pemladrv_mem_t ** key,
        pemladrv_mem_t ** result) {
    int nof_fields_in_key, nof_fields_in_result;
    const int core_id = 0; 

    SHR_FUNC_INIT_VARS(unit);

    
    if (api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.db_tcam_per_core_arr[core_id].result_chunk_mapper_matrix_arr ==
        NULL ||
        api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.db_tcam_per_core_arr[core_id].result_chunk_mapper_matrix_arr->db_chunk_mapper ==
        NULL) {
        SHR_ERR_EXIT(_SHR_E_INIT, "Warning: PEM Db was NOT mapped during the application run");
    }
    nof_fields_in_key = api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.db_tcam_per_core_arr[core_id].nof_fields_in_key;
    nof_fields_in_result = api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.db_tcam_per_core_arr[core_id].nof_fields_in_result;
    SHR_IF_ERR_EXIT(pemladrv_mem_alloc(unit, nof_fields_in_key, key));
    SHR_IF_ERR_EXIT(pemladrv_mem_alloc(unit, nof_fields_in_result, result));

    
    SHR_IF_ERR_EXIT(dnx_pemladrv_pem_allocate_fldbuf_by_bit_range_array(unit, nof_fields_in_key,
                                                        api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.db_tcam_per_core_arr[core_id].key_field_bit_range_arr,
                                                        *key));
    
    SHR_IF_ERR_EXIT(dnx_pemladrv_pem_allocate_fldbuf_by_bit_range_array(unit, nof_fields_in_result,
                                                        api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.db_tcam_per_core_arr[core_id].result_field_bit_range_arr,
                                                        *result));

exit:
    SHR_FUNC_EXIT;
}



shr_error_e dnx_pemladrv_zero_last_chunk_of_lpm_dbs(const int unit) {
    int i, entry_index, nof_dbs;

    SHR_FUNC_INIT_VARS(unit);

    nof_dbs = api_info[unit].db_lpm_container.nof_lpm_dbs;
    for (int core_id=0; core_id<PEM_CFG_API_NOF_CORES_DYNAMIC; ++core_id) {
        for (i = 0; i < nof_dbs; ++i) {
            if (NULL ==
                api_info[unit].db_lpm_container.db_lpm_info_arr[i].logical_lpm_info.db_tcam_per_core_arr[core_id].result_chunk_mapper_matrix_arr)      
                continue;
            for (entry_index = 0; entry_index < PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES; ++entry_index)
                SHR_IF_ERR_EXIT(dnx_pemladrv_set_valid_bit_to_zero(unit, core_id, entry_index,
                                                   &api_info[unit].db_lpm_container.db_lpm_info_arr[i].logical_lpm_info));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


