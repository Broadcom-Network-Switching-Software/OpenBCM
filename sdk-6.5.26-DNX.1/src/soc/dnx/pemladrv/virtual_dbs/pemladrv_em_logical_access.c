
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
#include "./pemladrv_tcam_logical_access.h"
#include "./pemladrv_em_logical_access.h"
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

shr_error_e
dnx_pemladrv_update_em_cache_from_physical(
    const int unit)
{
    unsigned int index, entry, total_key_width, total_result_width, chunk_matrix_row_index, valid;
    unsigned int *tcam_data = NULL, *key_arr = NULL, *result_arr = NULL;
    pemladrv_mem_t *key = NULL, *result = NULL;
    SHR_FUNC_INIT_VARS(unit);

    for (int core_id = 0; core_id < PEM_CFG_API_NOF_CORES_DYNAMIC; ++core_id)
    {
        int sub_unit_idx = PEM_CFG_CORE_ID_TO_SUB_UNIT_ID(core_id);
        for (index = 0; index < api_info[unit].db_em_container.nof_em_dbs; ++index)
        {
            if (NULL ==
                api_info[unit].db_em_container.db_em_info_arr[index].logical_em_info.
                db_tcam_per_core_arr[core_id].result_chunk_mapper_matrix_arr)

                continue;

            total_key_width =
                api_info[unit].db_em_container.db_em_info_arr[index].logical_em_info.
                db_tcam_per_core_arr[core_id].total_key_width;
            total_result_width =
                api_info[unit].db_em_container.db_em_info_arr[index].logical_em_info.
                db_tcam_per_core_arr[core_id].total_result_width;
            SHR_ALLOC_SET_ZERO_ERR_EXIT(tcam_data,
                                        UTILEX_DIV_ROUND_UP(2 * total_key_width + 1,
                                                            SAL_UINT32_NOF_BITS) * sizeof(uint32), "tcam_data",
                                        "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

            SHR_IF_ERR_EXIT(pemladrv_mem_alloc_em(unit, index, &key, &result));
            for (entry = 0; entry < PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES; ++entry)
            {
                chunk_matrix_row_index = entry / PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES;
                SHR_ALLOC_SET_ZERO_ERR_EXIT(result_arr,
                                            UTILEX_DIV_ROUND_UP(total_result_width,
                                                                SAL_UINT32_NOF_BITS) * sizeof(uint32), "result_arr",
                                            "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

                SHR_IF_ERR_EXIT(dnx_build_physical_db_key_data_array(unit, sub_unit_idx,
                                                                     api_info[unit].
                                                                     db_em_container.db_em_info_arr[index].
                                                                     logical_em_info.db_tcam_per_core_arr
                                                                     [core_id].key_chunk_mapper_matrix_arr,
                                                                     dnx_get_nof_tcam_chunk_cols(total_key_width),
                                                                     chunk_matrix_row_index, entry, total_key_width,
                                                                     tcam_data));
                valid =
                    (tcam_data[UTILEX_DIV_ROUND_UP(2 * total_key_width + 1, SAL_UINT32_NOF_BITS) - 1]) >>
                    ((2 * total_key_width + 1) % UINT_WIDTH_IN_BITS - 1);

                if (!valid)
                {
                    SHR_FREE(result_arr);
                    continue;
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_build_physical_db_result_data_array(unit, sub_unit_idx,
                                                                            api_info[unit].
                                                                            db_em_container.db_em_info_arr[index].
                                                                            logical_em_info.db_tcam_per_core_arr
                                                                            [core_id].result_chunk_mapper_matrix_arr,
                                                                            dnx_get_nof_cam_sram_chunk_cols
                                                                            (total_result_width),
                                                                            chunk_matrix_row_index, entry,
                                                                            api_info[unit].
                                                                            db_em_container.db_em_info_arr[index].
                                                                            logical_em_info.db_tcam_per_core_arr
                                                                            [core_id].result_field_bit_range_arr,
                                                                            result, result_arr));

                    SHR_IF_ERR_EXIT(dnx_convert_tcam_data_into_key_array(unit, total_key_width, tcam_data, &key_arr));
                    dnx_pemladrv_update_em_cache_and_next_free_index(core_id, entry, key_arr, result_arr,
                                                                     &api_info[unit].
                                                                     db_em_container.db_em_info_arr[index]);
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
    SHR_FREE(tcam_data);
    SHR_FREE(key_arr);
    SHR_FREE(result_arr);
    pemladrv_mem_free(key);
    pemladrv_mem_free(result);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_pemladrv_em_info_allocate(
    const int unit,
    pemladrv_db_info ** arr,
    int *size)
{
    int mapped_em_size = 0;
    int name_len, nof_key_fields, nof_fields;
    unsigned int em_index, field_index;
    const int core_id = 0;
    FieldBitRange *em_field_key_info_arr, *em_field_result_info_arr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(*arr, api_info[unit].db_em_container.nof_em_dbs * sizeof(pemladrv_db_info), "arr",
                                "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    for (em_index = 0; em_index < api_info[unit].db_em_container.nof_em_dbs; ++em_index)
    {
        LogicalEmInfo *em_info_arr = &api_info[unit].db_em_container.db_em_info_arr[em_index];

        if (api_info[unit].db_em_container.db_em_info_arr == NULL
            || api_info[unit].db_em_container.db_em_info_arr[em_index].logical_em_info.
            db_tcam_per_core_arr[core_id].result_chunk_mapper_matrix_arr == NULL)
        {
            continue;
        }

        (*arr)[mapped_em_size].db_id = em_index;
        name_len =
            sal_strnlen(api_info[unit].db_em_container.db_em_info_arr[em_index].
                        logical_em_info.db_tcam_per_core_arr[core_id].name, MAX_NAME_LENGTH);
        SHR_ALLOC_SET_ZERO_ERR_EXIT((*arr)[mapped_em_size].db_name, name_len + 1, "(*arr)[].db_name", "%s%s%s\r\n",
                                    EMPTY, EMPTY, EMPTY);
        sal_strncpy((*arr)[mapped_em_size].db_name,
                    api_info[unit].db_em_container.db_em_info_arr[em_index].
                    logical_em_info.db_tcam_per_core_arr[core_id].name, name_len);
        (*arr)[mapped_em_size].db_name[name_len] = '\0';
        nof_key_fields =
            api_info[unit].db_em_container.db_em_info_arr[em_index].logical_em_info.
            db_tcam_per_core_arr[core_id].nof_fields_in_key;

        nof_fields = nof_key_fields;

        (*arr)[mapped_em_size].nof_fields = nof_fields;

        SHR_ALLOC_SET_ZERO_ERR_EXIT((*arr)[mapped_em_size].ptr,
                                    (*arr)[mapped_em_size].nof_fields * sizeof(pemladrv_db_field_info), "(*arr)[].ptr",
                                    "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        for (field_index = 0; field_index < nof_fields; ++field_index)
        {
            em_field_key_info_arr =
                &em_info_arr->logical_em_info.db_tcam_per_core_arr[core_id].key_field_bit_range_arr[field_index];

            if (em_field_key_info_arr->is_field_mapped)
            {
                (*arr)[mapped_em_size].ptr[field_index].is_field_mapped = em_field_key_info_arr->is_field_mapped;
                (*arr)[mapped_em_size].ptr[field_index].field_id = field_index;
                (*arr)[mapped_em_size].ptr[field_index].field_size_in_bits =
                    em_field_key_info_arr->msb - em_field_key_info_arr->lsb + 1;

                (*arr)[mapped_em_size].total_nof_entries =
                    api_info[unit].db_em_container.db_em_info_arr[em_index].logical_em_info.
                    db_tcam_per_core_arr->total_nof_entries;
                (*arr)[mapped_em_size].total_key_width =
                    api_info[unit].db_em_container.db_em_info_arr[em_index].logical_em_info.
                    db_tcam_per_core_arr->total_key_width;
            }

            em_field_result_info_arr =
                &em_info_arr->logical_em_info.db_tcam_per_core_arr[core_id].result_field_bit_range_arr[field_index];

            if (em_field_result_info_arr->is_field_mapped)
            {
                (*arr)[mapped_em_size].ptr[field_index].is_field_mapped = em_field_result_info_arr->is_field_mapped;
                (*arr)[mapped_em_size].ptr[field_index].field_size_in_bits +=
                    em_field_result_info_arr->msb - em_field_result_info_arr->lsb + 1;
                (*arr)[mapped_em_size].total_result_width =
                    api_info[unit].db_em_container.db_em_info_arr[em_index].logical_em_info.
                    db_tcam_per_core_arr->total_result_width;
            }
        }

        mapped_em_size++;
    }

    *size = mapped_em_size;
exit:
    SHR_FUNC_EXIT;
}

void
dnx_pemladrv_em_info_de_allocate(
    const int unit,
    pemladrv_db_info * ptr)
{
    pemladrv_db_info temp;
    if (ptr)
    {
        for (unsigned int i = 0; i < api_info[unit].db_em_container.nof_em_dbs; ++i)
        {
            temp = ptr[i];
            if (temp.db_name == NULL)
                continue;
            SHR_FREE(temp.ptr);
            SHR_FREE(temp.db_name);
        }
        SHR_FREE(ptr);
    }
}

shr_error_e
pemladrv_em_insert_inner(
    const int unit,
    const int core_id,
    table_id_t table_id,
    pemladrv_mem_t * key,
    pemladrv_mem_t * result,
    int *index)
{

    unsigned int nof_fields_in_key =
        api_info[unit].db_em_container.db_em_info_arr[table_id].logical_em_info.
        db_tcam_per_core_arr[core_id].nof_fields_in_key;
    const unsigned int total_key_width_in_bits =
        api_info[unit].db_em_container.db_em_info_arr[table_id].logical_em_info.
        db_tcam_per_core_arr[core_id].total_key_width;
    const FieldBitRange *const key_fields_arr =
        api_info[unit].db_em_container.db_em_info_arr[table_id].logical_em_info.
        db_tcam_per_core_arr[core_id].key_field_bit_range_arr;
    int next_free_index = api_info[unit].db_em_container.db_em_info_arr[table_id].em_cache[core_id].next_free_index;

    unsigned int *virtual_key_data = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(virtual_key_data,
                                (size_t) pemladrv_ceil(total_key_width_in_bits, UINT_WIDTH_IN_BITS) * sizeof(uint32),
                                "virtual_key_data", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (key->nof_fields != nof_fields_in_key)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "One or More fields are missing while trying to write key. Exact-Match - #%d id\n", table_id);
    }

    if (next_free_index == -1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "The DB is already full, Can't write or add keys.");
    }

    dnx_build_virtual_db_data(0, key_fields_arr, key, virtual_key_data);

    *index =
        dnx_pemladrv_does_key_exists_in_physical_db(unit,
                                                    &api_info[unit].db_em_container.
                                                    db_em_info_arr[table_id].logical_em_info,
                                                    api_info[unit].db_em_container.
                                                    db_em_info_arr[table_id].em_cache[core_id].
                                                    physical_entry_occupation,
                                                    api_info[unit].db_em_container.
                                                    db_em_info_arr[table_id].em_cache[core_id].em_key_entry_arr,
                                                    virtual_key_data);
    if (*index != -1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Key already exists. Key index is %d\n", *index);
    }

    *index = next_free_index;
    SHR_IF_ERR_EXIT(pemladrv_em_entry_set_by_id(unit, core_id, table_id, next_free_index, key, result));

exit:
    SHR_FREE(virtual_key_data);
    SHR_FUNC_EXIT;
}

shr_error_e
pemladrv_em_insert(
    const int unit,
    const int core_id,
    table_id_t table_id,
    pemladrv_mem_t * key,
    pemladrv_mem_t * result,
    int *index)
{
    SHR_FUNC_INIT_VARS(unit);
    if (core_id == -1)
    {
        for (int core_i = 0; core_i < PEM_CFG_API_NOF_CORES_DYNAMIC; ++core_i)
        {
            SHR_IF_ERR_EXIT(pemladrv_em_insert_inner(unit, core_i, table_id, key, result, index));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(pemladrv_em_insert_inner(unit, core_id, table_id, key, result, index));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
pemladrv_em_remove(
    const int unit,
    const int core_id,
    table_id_t table_id,
    pemladrv_mem_t * key,
    int *index)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pemladrv_find_physical_entry_index_by_key
                    (unit, core_id, &api_info[unit].db_em_container.db_em_info_arr[table_id].logical_em_info,
                     api_info[unit].db_em_container.db_em_info_arr[table_id].
                     em_cache[core_id].physical_entry_occupation,
                     api_info[unit].db_em_container.db_em_info_arr[table_id].em_cache[core_id].em_key_entry_arr, key,
                     index));
    if (*index == -1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Key doesn't exist in DB");
    }
    SHR_IF_ERR_EXIT(pemladrv_em_remove_by_index(unit, core_id, table_id, *index));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
pemladrv_em_lookup(
    const int unit,
    const int core_id,
    table_id_t table_id,
    pemladrv_mem_t * key,
    pemladrv_mem_t * result,
    int *index)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pemladrv_find_physical_entry_index_by_key
                    (unit, core_id, &api_info[unit].db_em_container.db_em_info_arr[table_id].logical_em_info,
                     api_info[unit].db_em_container.db_em_info_arr[table_id].
                     em_cache[core_id].physical_entry_occupation,
                     api_info[unit].db_em_container.db_em_info_arr[table_id].em_cache[core_id].em_key_entry_arr, key,
                     index));
    if (*index == -1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Key doesn't exist in DB");
    }
    SHR_IF_ERR_EXIT(dnx_pemladrv_get_em_data_by_entry(unit, core_id, table_id, *index, key, result));

exit:
    SHR_FUNC_EXIT;
}

void
pemladrv_em_get_next_index(
    const int unit,
    const int core_id,
    table_id_t table_id,
    int *index)
{
    *index = api_info[unit].db_em_container.db_em_info_arr[table_id].em_cache[core_id].next_free_index;
}

shr_error_e
pemladrv_em_entry_get_by_id(
    const int unit,
    const int core_id,
    table_id_t table_id,
    int index,
    pemladrv_mem_t * key,
    pemladrv_mem_t * result)
{

    SHR_FUNC_INIT_VARS(unit);
    if (index == -1)
    {

        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid entry");
    }

    SHR_IF_ERR_EXIT(dnx_pemladrv_get_em_data_by_entry(unit, core_id, table_id, index, key, result));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
pemladrv_em_entry_set_by_id(
    const int unit,
    const int core_id,
    table_id_t table_id,
    int index,
    pemladrv_mem_t * key,
    pemladrv_mem_t * result)
{
    const FieldBitRange *key_fields_arr = NULL;
    const FieldBitRange *result_fields_arr = NULL;
    unsigned int total_key_width_in_bits;
    unsigned int total_result_width_in_bits;
    unsigned int *virtual_key_data = NULL;
    unsigned int *virtual_result_data = NULL;
    pemladrv_mem_t *mask = NULL, *valid = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (index < 0 || index > 31)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid entry %d", index);
    }

    if (api_info[unit].db_em_container.db_em_info_arr[table_id].em_cache[core_id].physical_entry_occupation[index] == 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Entry %d is already occupied.", index);
    }

    key_fields_arr =
        api_info[unit].db_em_container.db_em_info_arr[table_id].logical_em_info.
        db_tcam_per_core_arr[core_id].key_field_bit_range_arr;
    result_fields_arr =
        api_info[unit].db_em_container.db_em_info_arr[table_id].logical_em_info.
        db_tcam_per_core_arr[core_id].result_field_bit_range_arr;
    total_key_width_in_bits =
        api_info[unit].db_em_container.db_em_info_arr[table_id].logical_em_info.
        db_tcam_per_core_arr[core_id].total_key_width;
    total_result_width_in_bits =
        api_info[unit].db_em_container.db_em_info_arr[table_id].logical_em_info.
        db_tcam_per_core_arr[core_id].total_result_width;

    SHR_ALLOC_SET_ZERO_ERR_EXIT(virtual_key_data,
                                (size_t) pemladrv_ceil(total_key_width_in_bits, UINT_WIDTH_IN_BITS) * sizeof(uint32),
                                "virtual_key_data", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO_ERR_EXIT(virtual_result_data,
                                (size_t) pemladrv_ceil(total_result_width_in_bits, UINT_WIDTH_IN_BITS) * sizeof(uint32),
                                "virtual_result_data", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    dnx_build_virtual_db_data(0, key_fields_arr, key, virtual_key_data);
    dnx_build_virtual_db_data(0, result_fields_arr, result, virtual_result_data);

    SHR_IF_ERR_EXIT(dnx_pemladrv_build_em_mask(unit, key_fields_arr, key, &mask));
    SHR_IF_ERR_EXIT(dnx_pemladrv_build_tcam_valid(unit, &valid));

    SHR_IF_ERR_EXIT(dnx_logical_cam_based_db_write
                    (&api_info[unit].db_em_container.db_em_info_arr[table_id].logical_em_info, unit, core_id, index,
                     key, mask, valid, result));

exit:
    dnx_pemladrv_update_em_cache_and_next_free_index(core_id, index, virtual_key_data, virtual_result_data,
                                                     &api_info[unit].db_em_container.db_em_info_arr[table_id]);
    dnx_pemladrv_free_pemladrv_mem_struct(&mask);
    dnx_pemladrv_free_pemladrv_mem_struct(&valid);
    SHR_FUNC_EXIT;
}

shr_error_e
pemladrv_em_remove_all_inner(
    const int unit,
    const int core_id,
    table_id_t table_id)
{
    unsigned int nof_entries = PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES;
    unsigned int index;

    SHR_FUNC_INIT_VARS(unit);

    for (index = 0; index < nof_entries; ++index)
    {
        if (api_info[unit].db_em_container.db_em_info_arr[table_id].
            em_cache[core_id].physical_entry_occupation[index] == 1)
            SHR_IF_ERR_EXIT(pemladrv_em_remove_by_index(unit, core_id, table_id, index));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
pemladrv_em_remove_all(
    const int unit,
    const int core_id,
    table_id_t table_id)
{

    SHR_FUNC_INIT_VARS(unit);

    if (core_id == -1)
    {

        for (int core_i = 0; core_i < PEM_CFG_API_NOF_CORES_DYNAMIC; ++core_i)
        {
            SHR_IF_ERR_EXIT(pemladrv_em_remove_all_inner(unit, core_i, table_id));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(pemladrv_em_remove_all_inner(unit, core_id, table_id));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
pemladrv_em_remove_by_index(
    const int unit,
    const int core_id,
    table_id_t table_id,
    int index)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pemladrv_set_valid_bit_to_zero
                    (unit, core_id, index, &api_info[unit].db_em_container.db_em_info_arr[table_id].logical_em_info));

    api_info[unit].db_em_container.db_em_info_arr[table_id].em_cache[core_id].physical_entry_occupation[index] = 0;

    if (index < api_info[unit].db_em_container.db_em_info_arr[table_id].em_cache[core_id].next_free_index
        || api_info[unit].db_em_container.db_em_info_arr[table_id].em_cache[core_id].next_free_index == -1)
        api_info[unit].db_em_container.db_em_info_arr[table_id].em_cache[core_id].next_free_index = index;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_pemladrv_build_em_mask(
    int unit,
    const FieldBitRange * key_field_bit_range_arr,
    const pemladrv_mem_t * key,
    pemladrv_mem_t ** mask)
{
    int field_index;
    unsigned int num_of_chunks_in_fldbuf;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(*mask, sizeof(pemladrv_mem_t), "mask", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    (*mask)->nof_fields = key->nof_fields;
    (*mask)->flags = key->flags;
    SHR_ALLOC_SET_ZERO_ERR_EXIT((*mask)->fields, (*mask)->nof_fields * sizeof(pemladrv_field_t *), "(*mask)->fields",
                                "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    for (field_index = 0; field_index < (*mask)->nof_fields; ++field_index)
    {
        SHR_ALLOC_SET_ZERO_ERR_EXIT((*mask)->fields[field_index], sizeof(pemladrv_field_t), "(*mask)->fields[]",
                                    "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        (*mask)->fields[field_index]->field_id = key->fields[field_index]->field_id;
        (*mask)->fields[field_index]->flags = key->fields[field_index]->flags;

        num_of_chunks_in_fldbuf = dnx_pemladrv_get_nof_chunks_in_fldbuf(field_index, key_field_bit_range_arr);
        SHR_ALLOC_SET_ZERO_ERR_EXIT((*mask)->fields[field_index]->fldbuf,
                                    num_of_chunks_in_fldbuf * sizeof(unsigned int), "(*mask)->fields[]->fldbuf",
                                    "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    }
exit:
    SHR_FUNC_EXIT;
}

void
dnx_pemladrv_update_em_cache_and_next_free_index(
    const int core_id,
    unsigned int row_index,
    unsigned int *virtual_key_data,
    unsigned int *virtual_result_data,
    LogicalEmInfo * em_cache)
{
    em_cache->em_cache[core_id].em_key_entry_arr[row_index] = virtual_key_data;
    em_cache->em_cache[core_id].em_result_entry_arr[row_index] = virtual_result_data;
    em_cache->em_cache[core_id].physical_entry_occupation[row_index] = 1;
    em_cache->em_cache[core_id].next_free_index = dnx_pemladrv_find_next_em_free_entry(core_id, em_cache);
}

int
dnx_pemladrv_find_next_em_free_entry(
    const int core_id,
    LogicalEmInfo * em_cache)
{

    unsigned int next_free_index;
    for (next_free_index = 0;
         next_free_index < em_cache->logical_em_info.db_tcam_per_core_arr[core_id].total_nof_entries; ++next_free_index)
    {
        if (!em_cache->em_cache[core_id].physical_entry_occupation[next_free_index])
        {
            em_cache->em_cache[core_id].next_free_index = next_free_index;
            return next_free_index;
        }
    }
    return -1;
}

shr_error_e
dnx_pemladrv_get_em_data_by_entry(
    const int unit,
    const int core_id,
    const int db_id,
    const int index,
    pemladrv_mem_t * key,
    pemladrv_mem_t * result)
{

    SHR_FUNC_INIT_VARS(unit);

    dnx_set_pem_mem_accesss_fldbuf_from_physical_sram_data_array(api_info[unit].db_em_container.
                                                                 db_em_info_arr[db_id].em_cache[core_id].
                                                                 em_result_entry_arr[index],
                                                                 api_info[unit].db_em_container.
                                                                 db_em_info_arr[db_id].logical_em_info.
                                                                 db_tcam_per_core_arr
                                                                 [core_id].result_field_bit_range_arr, result);
    dnx_set_pem_mem_accesss_fldbuf_from_physical_sram_data_array(api_info[unit].db_em_container.
                                                                 db_em_info_arr[db_id].em_cache[core_id].
                                                                 em_key_entry_arr[index],
                                                                 api_info[unit].db_em_container.
                                                                 db_em_info_arr[db_id].logical_em_info.
                                                                 db_tcam_per_core_arr[core_id].key_field_bit_range_arr,
                                                                 key);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
pemladrv_mem_alloc_em(
    const int unit,
    const int db_id,
    pemladrv_mem_t ** key,
    pemladrv_mem_t ** result)
{
    int nof_fields_in_key, nof_fields_in_result;
    const int core_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.
        db_tcam_per_core_arr[core_id].result_chunk_mapper_matrix_arr == NULL
        || api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.
        db_tcam_per_core_arr[core_id].result_chunk_mapper_matrix_arr->db_chunk_mapper == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Warning: PEM Db was NOT mapped during the application run");
    }
    nof_fields_in_key =
        api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.
        db_tcam_per_core_arr[core_id].nof_fields_in_key;
    nof_fields_in_result =
        api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.
        db_tcam_per_core_arr[core_id].nof_fields_in_result;
    SHR_IF_ERR_EXIT(pemladrv_mem_alloc(unit, nof_fields_in_key, key));
    SHR_IF_ERR_EXIT(pemladrv_mem_alloc(unit, nof_fields_in_result, result));

    SHR_IF_ERR_EXIT(dnx_pemladrv_pem_allocate_fldbuf_by_bit_range_array(unit, nof_fields_in_key,
                                                                        api_info[unit].
                                                                        db_em_container.db_em_info_arr[db_id].
                                                                        logical_em_info.db_tcam_per_core_arr
                                                                        [core_id].key_field_bit_range_arr, *key));

    SHR_IF_ERR_EXIT(dnx_pemladrv_pem_allocate_fldbuf_by_bit_range_array(unit, nof_fields_in_result,
                                                                        api_info[unit].
                                                                        db_em_container.db_em_info_arr[db_id].
                                                                        logical_em_info.db_tcam_per_core_arr
                                                                        [core_id].result_field_bit_range_arr, *result));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_pemladrv_zero_last_chunk_of_em_dbs(
    const int unit)
{
    int i, entry_index, nof_dbs;

    SHR_FUNC_INIT_VARS(unit);

    nof_dbs = api_info[unit].db_em_container.nof_em_dbs;
    for (int core_id = 0; core_id < PEM_CFG_API_NOF_CORES_DYNAMIC; ++core_id)
    {
        for (i = 0; i < nof_dbs; ++i)
        {
            if (NULL ==
                api_info[unit].db_em_container.db_em_info_arr[i].logical_em_info.
                db_tcam_per_core_arr[core_id].result_chunk_mapper_matrix_arr)

                continue;
            for (entry_index = 0; entry_index < PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES; ++entry_index)
                SHR_IF_ERR_EXIT(dnx_pemladrv_set_valid_bit_to_zero(unit, core_id, entry_index,
                                                                   &api_info[unit].db_em_container.
                                                                   db_em_info_arr[i].logical_em_info));
        }
    }

exit:
    SHR_FUNC_EXIT;
}
