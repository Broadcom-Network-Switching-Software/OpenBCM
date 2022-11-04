
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
#include "./pemladrv_direct_logical_access.h"
#include "pemladrv_tcam_logical_access.h"
#include "../pemladrv_meminfo_init.h"
#include "../pemladrv_meminfo_access.h"
#include "../pemladrv_cfg_api_defines.h"

#ifdef ADAPTER_SERVER_MODE
#include <soc/dnxc/dnxc_adapter_reg_access.h>
#endif

#ifdef BCM_DNX_SUPPORT
#define BSL_LOG_MODULE BSL_LS_SOCDNX_ADAPTERDNX
#endif

extern ApiInfo api_info[MAX_NOF_UNITS];

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/drv.h>
#ifdef BCM_ACCESS_SUPPORT
#include <soc/access/access.h>
#endif
#include <soc/cmic.h>

shr_error_e
dnx_pem_direct_access_wrapper(
    int unit,
    int sub_unit_idx,
    int is_read,
    int is_mem,
    int block_identifier,
    uint32 address,
    uint32 size_in_bits,
    uint32 *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    {
        int schan_core = get_core_from_block_id(unit, block_identifier);
        uint32 core_bitmap = dnx_data_device.general.valid_cores_bitmap_get(unit);

        if (!utilex_bitstream_test_bit(&core_bitmap, schan_core))
        {
            SHR_EXIT();
        }
    }
#endif

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        uint32 flags = 0;
        if (is_read == FALSE)
        {
            flags |= FLAG_ACCESS_IS_WRITE;
        }
        if (is_mem)
        {
            flags |= FLAG_ACCESS_IS_MEMORY;
        }
        ACCESS_SBUS_BLOCK_SET_SUBDEV(block_identifier, sub_unit_idx);
        SHR_IF_ERR_EXIT(access_direct_regmem
                        (unit, flags, block_identifier, address, BITS2WORDS(size_in_bits), entry_data));

    }
    else
#endif
    {
#ifdef ADAPTER_SERVER_MODE
        SHR_IF_ERR_EXIT(adapter_memreg_access
                        (unit, sub_unit_idx, block_identifier, address, -1, BITS2BYTES(size_in_bits), is_mem, is_read,
                         0, entry_data));
#else
        if (is_read)
        {
            SHR_IF_ERR_EXIT(soc_direct_memreg_get
                            (unit, block_identifier, address, BITS2WORDS(size_in_bits), is_mem, entry_data));
        }
        else
        {
            SHR_IF_ERR_EXIT(soc_direct_memreg_set
                            (unit, block_identifier, address, BITS2WORDS(size_in_bits), is_mem, entry_data));
        }
#endif
    }

exit:
    SHR_FUNC_EXIT;
}
#endif

unsigned int
dnx_get_nof_direct_chunk_cols(
    const int core_id,
    LogicalDirectInfo * direct_info)
{
    unsigned int chunks_num = 0;
    unsigned int field_num, curr_lsb_bit, curr_msb_bit, field_width;

    for (field_num = 0; field_num < direct_info->db_direct_info_per_core_arr[core_id].nof_fields; ++field_num)
    {
        curr_lsb_bit = direct_info->db_direct_info_per_core_arr[core_id].field_bit_range_arr[field_num].lsb;
        curr_msb_bit = direct_info->db_direct_info_per_core_arr[core_id].field_bit_range_arr[field_num].msb;
        field_width = curr_msb_bit - curr_lsb_bit + 1;
        chunks_num += dnx_get_nof_map_sram_chunk_cols(field_width);
    }
    return chunks_num;
}

unsigned int
dnx_pemladrv_get_nof_direct_chunk_rows(
    const int unit,
    const unsigned int nof_entries)
{
    if (nof_entries % PEM_CFG_API_MAP_CHUNK_N_ENTRIES == 0)
        return (nof_entries / PEM_CFG_API_MAP_CHUNK_N_ENTRIES);
    else
        return (nof_entries / PEM_CFG_API_MAP_CHUNK_N_ENTRIES) + 1;
}

unsigned int
dnx_get_nof_map_sram_chunk_cols(
    const unsigned int total_result_width)
{
    if (total_result_width % PEM_CFG_API_MAP_CHUNK_WIDTH == 0)
        return (total_result_width / PEM_CFG_API_MAP_CHUNK_WIDTH);
    else
        return (total_result_width / PEM_CFG_API_MAP_CHUNK_WIDTH) + 1;
}

shr_error_e
pemladrv_direct_write_inner(
    const int unit,
    const int core_id,
    table_id_t db_id,
    uint32 row_index,
    pemladrv_mem_t * data)
{
    PhysicalWriteInfo *target_result_physical_memory_entry_data_list = NULL;
    unsigned int chunk_matrix_row;
    unsigned int nof_implamentations;
    unsigned int nof_result_chunk_matrix_cols;

    unsigned int *virtual_result_mask = NULL;

    unsigned int *virtual_result_data = NULL;
    int sub_unit_idx = PEM_CFG_CORE_ID_TO_SUB_UNIT_ID(core_id);
    SHR_FUNC_INIT_VARS(unit);

    if (api_info[unit].db_direct_container.db_direct_info_arr == NULL
        || api_info[unit].db_direct_container.db_direct_info_arr[db_id].
        db_direct_info_per_core_arr[core_id].result_chunk_mapper_matrix_arr == NULL)
    {

        SHR_EXIT();
    }

    chunk_matrix_row = row_index / PEM_CFG_API_MAP_CHUNK_N_ENTRIES;
    nof_implamentations =
        api_info[unit].db_direct_container.db_direct_info_arr[db_id].
        db_direct_info_per_core_arr[core_id].nof_chunk_matrices;
    nof_result_chunk_matrix_cols =
        dnx_get_nof_direct_chunk_cols(core_id, &api_info[unit].db_direct_container.db_direct_info_arr[db_id]);

    SHR_IF_ERR_EXIT(dnx_build_virtual_db_mask_and_data_from_result_fields
                    (unit,
                     api_info[unit].db_direct_container.db_direct_info_arr[db_id].
                     db_direct_info_per_core_arr[core_id].total_result_width,
                     api_info[unit].db_direct_container.db_direct_info_arr[db_id].
                     db_direct_info_per_core_arr[core_id].field_bit_range_arr, data, 1, &virtual_result_mask,
                     &virtual_result_data));

    SHR_IF_ERR_EXIT(dnx_run_over_all_chunks_read_physical_data_and_update_physical_memory_entry_data_list(unit,
                                                                                                          sub_unit_idx,
                                                                                                          0,
                                                                                                          row_index,
                                                                                                          api_info
                                                                                                          [unit].db_direct_container.db_direct_info_arr
                                                                                                          [db_id].db_direct_info_per_core_arr
                                                                                                          [core_id].total_result_width,
                                                                                                          chunk_matrix_row,
                                                                                                          nof_implamentations,
                                                                                                          nof_result_chunk_matrix_cols,
                                                                                                          api_info
                                                                                                          [unit].db_direct_container.db_direct_info_arr
                                                                                                          [db_id].db_direct_info_per_core_arr
                                                                                                          [core_id].result_chunk_mapper_matrix_arr,
                                                                                                          virtual_result_mask,
                                                                                                          virtual_result_data,
                                                                                                          &target_result_physical_memory_entry_data_list));

    SHR_IF_ERR_EXIT(dnx_write_all_physical_data_from_list
                    (unit, sub_unit_idx, target_result_physical_memory_entry_data_list));

exit:
    dnx_free_list(target_result_physical_memory_entry_data_list);
    SHR_FREE(virtual_result_mask);
    SHR_FREE(virtual_result_data);
    SHR_FUNC_EXIT;
}

shr_error_e
pemladrv_direct_write(
    const int unit,
    const int core_id,
    table_id_t db_id,
    uint32 row_index,
    pemladrv_mem_t * data)
{
    SHR_FUNC_INIT_VARS(unit);
    if (core_id == -1)
    {
        for (int core_i = 0; core_i < PEM_CFG_API_NOF_CORES_DYNAMIC; ++core_i)
        {
            SHR_IF_ERR_EXIT(pemladrv_direct_write_inner(unit, core_i, db_id, row_index, data));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(pemladrv_direct_write_inner(unit, core_id, db_id, row_index, data));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
pemladrv_direct_read(
    const int unit,
    const int core_id,
    table_id_t db_id,
    uint32 row_index,
    pemladrv_mem_t * result)
{

    const unsigned int total_result_width_in_bits =
        api_info[unit].db_direct_container.db_direct_info_arr[db_id].
        db_direct_info_per_core_arr[core_id].total_result_width;
    unsigned int *virtual_db_data_array = NULL;
    unsigned int chunk_matrix_row_index = row_index / PEM_CFG_API_MAP_CHUNK_N_ENTRIES;
    int sub_unit_idx = PEM_CFG_CORE_ID_TO_SUB_UNIT_ID(core_id);
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(virtual_db_data_array,
                                (size_t) pemladrv_ceil(total_result_width_in_bits,
                                                       PEM_CFG_API_MAP_CHUNK_WIDTH) * sizeof(unsigned int),
                                "virtual_db_data_array", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_build_physical_db_result_data_array(unit, sub_unit_idx,
                                                            api_info[unit].db_direct_container.db_direct_info_arr
                                                            [db_id].db_direct_info_per_core_arr
                                                            [core_id].result_chunk_mapper_matrix_arr,
                                                            dnx_get_nof_direct_chunk_cols(core_id,
                                                                                          &api_info
                                                                                          [unit].
                                                                                          db_direct_container.db_direct_info_arr
                                                                                          [db_id]),
                                                            chunk_matrix_row_index, row_index,
                                                            api_info[unit].
                                                            db_direct_container.db_direct_info_arr
                                                            [db_id].db_direct_info_per_core_arr[core_id].
                                                            field_bit_range_arr, result, virtual_db_data_array));

    dnx_set_pem_mem_accesss_fldbuf_from_physical_sram_data_array(virtual_db_data_array,
                                                                 api_info[unit].db_direct_container.db_direct_info_arr
                                                                 [db_id].db_direct_info_per_core_arr
                                                                 [core_id].field_bit_range_arr, result);

exit:
    SHR_FREE(virtual_db_data_array);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_pemladrv_direct_info_allocate(
    const int unit,
    pemladrv_db_info ** arr,
    int *size)
{
    int mapped_diret_size = 0;
    int name_len;
    unsigned int direct_index, field_index;
    const int core_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(*arr, api_info[unit].db_direct_container.nof_direct_dbs * sizeof(pemladrv_db_info),
                                "arr", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    for (direct_index = 0; direct_index < api_info[unit].db_direct_container.nof_direct_dbs; ++direct_index)
    {
        LogicalDirectInfo *direct_info_arr = &api_info[unit].db_direct_container.db_direct_info_arr[direct_index];

        if (api_info[unit].db_direct_container.db_direct_info_arr == NULL
            || api_info[unit].db_direct_container.db_direct_info_arr[direct_index].
            db_direct_info_per_core_arr[core_id].result_chunk_mapper_matrix_arr == NULL)
        {
            continue;
        }

        (*arr)[mapped_diret_size].db_id = direct_index;
        name_len =
            sal_strnlen(api_info[unit].db_direct_container.
                        db_direct_info_arr[direct_index].db_direct_info_per_core_arr[core_id].name, MAX_NAME_LENGTH);
        SHR_ALLOC_SET_ZERO_ERR_EXIT((*arr)[mapped_diret_size].db_name, name_len + 1, "(*arr)[].db_name", "%s%s%s\r\n",
                                    EMPTY, EMPTY, EMPTY);
        sal_strncpy((*arr)[mapped_diret_size].db_name,
                    api_info[unit].db_direct_container.
                    db_direct_info_arr[direct_index].db_direct_info_per_core_arr[core_id].name, name_len);
        (*arr)[mapped_diret_size].db_name[name_len] = '\0';
        (*arr)[mapped_diret_size].nof_fields =
            api_info[unit].db_direct_container.db_direct_info_arr[direct_index].
            db_direct_info_per_core_arr[core_id].nof_fields;
        SHR_ALLOC_SET_ZERO_ERR_EXIT((*arr)[mapped_diret_size].ptr,
                                    (*arr)[mapped_diret_size].nof_fields * sizeof(pemladrv_db_field_info),
                                    "(*arr)[].ptr", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        for (field_index = 0;
             field_index <
             api_info[unit].db_direct_container.db_direct_info_arr[direct_index].
             db_direct_info_per_core_arr[core_id].nof_fields; ++field_index)
        {
            FieldBitRange *direct_field_info_arr =
                &direct_info_arr->db_direct_info_per_core_arr[core_id].field_bit_range_arr[field_index];
            if (direct_field_info_arr->is_field_mapped)
            {
                (*arr)[mapped_diret_size].ptr[field_index].is_field_mapped = direct_field_info_arr->is_field_mapped;
                (*arr)[mapped_diret_size].ptr[field_index].field_id = field_index;
                (*arr)[mapped_diret_size].ptr[field_index].field_size_in_bits =
                    direct_field_info_arr->msb - direct_field_info_arr->lsb + 1;

                (*arr)[mapped_diret_size].total_nof_entries =
                    api_info[unit].db_direct_container.db_direct_info_arr[direct_index].
                    db_direct_info_per_core_arr->total_nof_entries;
                (*arr)[mapped_diret_size].total_result_width =
                    api_info[unit].db_direct_container.db_direct_info_arr[direct_index].
                    db_direct_info_per_core_arr->total_result_width;
            }
        }

        mapped_diret_size++;
    }

    *size = mapped_diret_size;
exit:
    SHR_FUNC_EXIT;
}

void
dnx_pemladrv_direct_info_de_allocate(
    const int unit,
    pemladrv_db_info * ptr)
{
    if (ptr != NULL)
    {
        pemladrv_db_info temp;
        for (unsigned int i = 0; i < api_info[unit].db_direct_container.nof_direct_dbs; ++i)
        {
            temp = ptr[i];
            SHR_FREE(temp.db_name);
            SHR_FREE(temp.ptr);
        }
        SHR_FREE(ptr);
    }
}

shr_error_e
pemladrv_mem_alloc_direct(
    const int unit,
    const int db_id,
    pemladrv_mem_t ** result)
{
    int nof_fields;
    const int core_id = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (api_info[unit].db_direct_container.db_direct_info_arr == NULL ||
        api_info[unit].db_direct_container.db_direct_info_arr[db_id].
        db_direct_info_per_core_arr[core_id].result_chunk_mapper_matrix_arr == NULL)
    {

        SHR_ERR_EXIT(_SHR_E_INTERNAL, "PEM Db-id %d was NOT mapped during the application run.", db_id);
    }

    nof_fields =
        api_info[unit].db_direct_container.db_direct_info_arr[db_id].db_direct_info_per_core_arr[core_id].nof_fields;
    SHR_IF_ERR_EXIT(pemladrv_mem_alloc(unit, nof_fields, result));

    SHR_IF_ERR_EXIT(dnx_pemladrv_pem_allocate_fldbuf_by_bit_range_array(unit, nof_fields,
                                                                        api_info[unit].
                                                                        db_direct_container.db_direct_info_arr
                                                                        [db_id].db_direct_info_per_core_arr
                                                                        [core_id].field_bit_range_arr, *result));

exit:
    SHR_FUNC_EXIT;
}
