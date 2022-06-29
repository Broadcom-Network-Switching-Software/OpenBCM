
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
#include "pemladrv_tcam_logical_access.h"
#include "../pemladrv_meminfo_init.h"
#include "../pemladrv_meminfo_access.h"
#include "../pemladrv_cfg_api_defines.h"

#ifdef BCM_DNX_SUPPORT
#define BSL_LOG_MODULE BSL_LS_SOCDNX_ADAPTERDNX
#endif

extern ApiInfo api_info[MAX_NOF_UNITS];

shr_error_e
dnx_set_pem_mem_accesss_fldbuf_from_physical_tcam_data_array(
    int unit,
    const unsigned int *virtual_db_data_array,
    const unsigned int total_key_width,
    const FieldBitRange * field_bit_range_arr,
    pemladrv_mem_t * key,
    pemladrv_mem_t * mask,
    pemladrv_mem_t * valid)
{

    FieldBitRange *valid_field_bit_range_arr = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_produce_valid_field_bit_range(unit, &valid_field_bit_range_arr));

    dnx_set_pem_mem_accesss_fldbuf(0, virtual_db_data_array, field_bit_range_arr, key);
    dnx_set_pem_mem_accesss_fldbuf(total_key_width, virtual_db_data_array, field_bit_range_arr, mask);

    dnx_set_pem_mem_accesss_fldbuf(2 * total_key_width, virtual_db_data_array, valid_field_bit_range_arr, valid);

exit:
    SHR_FREE(valid_field_bit_range_arr);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_convert_tcam_data_into_key_array(
    int unit,
    const int key_width_in_bits,
    unsigned int *tcam_data,
    unsigned int **key_arr)
{
    unsigned int nof_bits_to_write;
    int nof_bits_written, index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(*key_arr,
                                (size_t) pemladrv_ceil(key_width_in_bits, UINT_WIDTH_IN_BITS) * sizeof(uint32),
                                "key_arr", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    for (index = 0, nof_bits_written = 0; nof_bits_written < key_width_in_bits;
         nof_bits_written += nof_bits_to_write, ++index)
    {
        if ((key_width_in_bits - nof_bits_written) >= PEM_CFG_API_CAM_TCAM_KEY_WIDTH)
            nof_bits_to_write = PEM_CFG_API_CAM_TCAM_KEY_WIDTH;
        else
            nof_bits_to_write = (key_width_in_bits - nof_bits_written) % PEM_CFG_API_CAM_TCAM_KEY_WIDTH;

        set_bits(tcam_data, nof_bits_written, index * PEM_CFG_API_CAM_TCAM_CHUNK_WIDTH, nof_bits_to_write, *key_arr);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_convert_tcam_data_into_mask_array(
    int unit,
    const int total_key_width,
    const unsigned int *tcam_data,
    unsigned int **mask_arr)
{
    unsigned int nof_bits_to_write;
    int nof_bits_written, index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(*mask_arr, (size_t) pemladrv_ceil(total_key_width, UINT_WIDTH_IN_BITS) * sizeof(uint32),
                                "mask_arr", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    for (index = 0, nof_bits_written = 0; nof_bits_written < total_key_width;
         nof_bits_written += nof_bits_to_write, ++index)
    {
        if ((total_key_width - nof_bits_written) >= PEM_CFG_API_CAM_TCAM_KEY_WIDTH)
            nof_bits_to_write = PEM_CFG_API_CAM_TCAM_KEY_WIDTH;
        else
            nof_bits_to_write = (total_key_width - nof_bits_written) % PEM_CFG_API_CAM_TCAM_KEY_WIDTH;

        set_bits(tcam_data, nof_bits_written + total_key_width, index * PEM_CFG_API_CAM_TCAM_CHUNK_WIDTH,
                 nof_bits_to_write, *mask_arr);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_get_prefix_length_from_physical_data(
    int unit,
    const int total_key_width,
    const unsigned int *tcam_data,
    unsigned int *prefix)
{
    unsigned int *mask_arr = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_convert_tcam_data_into_mask_array(unit, total_key_width, tcam_data, &mask_arr));
    *prefix = dnx_calculate_prefix_from_mask_array(total_key_width, mask_arr);

exit:
    SHR_FREE(mask_arr);
    SHR_FUNC_EXIT;
}

void
dnx_build_virtual_db_mask(
    const unsigned int offset,
    const FieldBitRange * field_bit_range_arr,
    const pemladrv_mem_t * data,
    unsigned int *virtual_field_mask)
{
    unsigned int nof_fields = data->nof_fields;
    unsigned int msb_bit, lsb_bit;
    unsigned int field_index;

    for (field_index = 0; field_index < nof_fields; ++field_index)
    {
        msb_bit = field_bit_range_arr[data->fields[field_index]->field_id].msb;
        lsb_bit = field_bit_range_arr[data->fields[field_index]->field_id].lsb;

        dnx_set_mask_with_ones(msb_bit + offset, lsb_bit + offset, virtual_field_mask);
    }
}

shr_error_e
dnx_build_virtual_db_mask_and_data_from_key_fields(
    int unit,
    const int key_width,
    const unsigned int total_width,
    const FieldBitRange * field_bit_range_arr,
    const pemladrv_mem_t * key,
    const pemladrv_mem_t * mask,
    const pemladrv_mem_t * valid,
    unsigned int **virtual_key_mask,
    unsigned int **virtual_key_data)
{
    FieldBitRange *valid_field_bit_range_arr = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_produce_valid_field_bit_range(unit, &valid_field_bit_range_arr));

    SHR_ALLOC_SET_ZERO_ERR_EXIT(*virtual_key_mask,
                                (size_t) pemladrv_ceil(total_width, UINT_WIDTH_IN_BITS) * sizeof(uint32),
                                "virtual_key_mask", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    dnx_build_virtual_db_mask(0, field_bit_range_arr, key, *virtual_key_mask);
    dnx_build_virtual_db_mask(key_width, field_bit_range_arr, mask, *virtual_key_mask);
    dnx_build_virtual_db_mask(2 * key_width, valid_field_bit_range_arr, valid, *virtual_key_mask);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(*virtual_key_data,
                                (size_t) pemladrv_ceil(total_width, UINT_WIDTH_IN_BITS) * sizeof(uint32),
                                "*virtual_key_data", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    dnx_build_virtual_db_data(0, field_bit_range_arr, key, *virtual_key_data);
    dnx_build_virtual_db_data(key_width, field_bit_range_arr, mask, *virtual_key_data);
    dnx_build_virtual_db_data(2 * key_width, valid_field_bit_range_arr, valid, *virtual_key_data);

exit:
    SHR_FREE(valid_field_bit_range_arr);
    SHR_FUNC_EXIT;
}

unsigned int
dnx_get_nof_cam_sram_chunk_cols(
    const unsigned int total_result_width)
{
    if (total_result_width % PEM_CFG_API_CAM_RESULT_CHUNK_WIDTH == 0)
        return (total_result_width / PEM_CFG_API_CAM_RESULT_CHUNK_WIDTH);
    else
        return (total_result_width / PEM_CFG_API_CAM_RESULT_CHUNK_WIDTH) + 1;
}

shr_error_e
dnx_build_virtual_db_mask_and_data_from_result_fields(
    int unit,
    const unsigned int total_width,
    const FieldBitRange * field_bit_range_arr,
    const pemladrv_mem_t * result,
    const int is_init_mask_to_ones_at_field_location,
    unsigned int **virtual_result_mask,
    unsigned int **virtual_result_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(*virtual_result_mask,
                                (size_t) pemladrv_ceil(total_width, UINT_WIDTH_IN_BITS) * sizeof(uint32),
                                "virtual_result_mask", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (is_init_mask_to_ones_at_field_location)
    {
        dnx_build_virtual_db_mask(0, field_bit_range_arr, result, *virtual_result_mask);
    }

    SHR_ALLOC_SET_ZERO_ERR_EXIT(*virtual_result_data,
                                (size_t) pemladrv_ceil(total_width, UINT_WIDTH_IN_BITS) * sizeof(uint32),
                                "virtual_result_data", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    dnx_build_virtual_db_data(0, field_bit_range_arr, result, *virtual_result_data);
exit:
    SHR_FUNC_EXIT;
}

void
dnx_build_virtual_db_data(
    const unsigned int offset,
    const FieldBitRange * field_bit_range_arr,
    const pemladrv_mem_t * data,
    unsigned int *virtual_field_data)
{
    unsigned int nof_fields = data->nof_fields;
    unsigned int msb_bit, lsb_bit;
    unsigned int field_index;
    unsigned int field_width;

    for (field_index = 0; field_index < nof_fields; ++field_index)
    {
        msb_bit = field_bit_range_arr[data->fields[field_index]->field_id].msb;
        lsb_bit = field_bit_range_arr[data->fields[field_index]->field_id].lsb;
        field_width = msb_bit - lsb_bit + 1;

        set_bits(data->fields[field_index]->fldbuf, 0, lsb_bit + offset, field_width, virtual_field_data);
    }
}

shr_error_e
dnx_logical_cam_based_db_read(
    const LogicalTcamInfo * cam_info,
    const int unit,
    const int core_id,
    const uint32 row_index,
    pemladrv_mem_t * key,
    pemladrv_mem_t * mask,
    pemladrv_mem_t * valid,
    pemladrv_mem_t * result)
{

    unsigned int total_key_width_in_bits = 2 * cam_info->db_tcam_per_core_arr[core_id].total_key_width + 1;

    unsigned int total_result_width_in_bits = cam_info->db_tcam_per_core_arr[core_id].total_result_width;
    unsigned int total_key_width = cam_info->db_tcam_per_core_arr[core_id].total_key_width;
    unsigned int chunk_matrix_row_index = row_index / PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES;

    unsigned int *virtual_db_key_data_array = NULL;
    unsigned int *virtual_db_result_data_array = NULL;
    int sub_unit_idx = PEM_CFG_CORE_ID_TO_SUB_UNIT_ID(core_id);

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(virtual_db_key_data_array,
                                (size_t) pemladrv_ceil(total_key_width_in_bits, UINT_WIDTH_IN_BITS) * sizeof(uint32),
                                "virtual_db_key_data_array", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO_ERR_EXIT(virtual_db_result_data_array,
                                (size_t) pemladrv_ceil(total_result_width_in_bits, UINT_WIDTH_IN_BITS) * sizeof(uint32),
                                "virtual_db_result_data_array", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_build_physical_db_key_data_array
                    (unit, sub_unit_idx, cam_info->db_tcam_per_core_arr[core_id].key_chunk_mapper_matrix_arr,
                     dnx_get_nof_tcam_chunk_cols(total_key_width), chunk_matrix_row_index, row_index, total_key_width,
                     virtual_db_key_data_array));
    SHR_IF_ERR_EXIT(dnx_build_physical_db_result_data_array
                    (unit, sub_unit_idx, cam_info->db_tcam_per_core_arr[core_id].result_chunk_mapper_matrix_arr,
                     dnx_get_nof_cam_sram_chunk_cols(total_result_width_in_bits), chunk_matrix_row_index, row_index,
                     cam_info->db_tcam_per_core_arr[core_id].result_field_bit_range_arr, result,
                     virtual_db_result_data_array));

    SHR_IF_ERR_EXIT(dnx_set_pem_mem_accesss_fldbuf_from_physical_tcam_data_array(unit, virtual_db_key_data_array,
                                                                                 cam_info->db_tcam_per_core_arr
                                                                                 [core_id].total_key_width,
                                                                                 cam_info->db_tcam_per_core_arr
                                                                                 [core_id].key_field_bit_range_arr, key,
                                                                                 mask, valid));
    dnx_set_pem_mem_accesss_fldbuf_from_physical_sram_data_array(virtual_db_result_data_array,
                                                                 cam_info->db_tcam_per_core_arr
                                                                 [core_id].result_field_bit_range_arr, result);

exit:
    SHR_FREE(virtual_db_key_data_array);
    SHR_FREE(virtual_db_result_data_array);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_logical_cam_based_db_write(
    const LogicalTcamInfo * cam_info,
    const int unit,
    const int core_id,
    const uint32 row_index,
    const pemladrv_mem_t * key,
    const pemladrv_mem_t * mask,
    const pemladrv_mem_t * valid,
    const pemladrv_mem_t * data)
{
    PhysicalWriteInfo *target_key_physical_memory_entry_data_list = NULL;
    PhysicalWriteInfo *target_result_physical_memory_entry_data_list = NULL;

    unsigned int tcam_key_width;

    unsigned int chunk_matrix_row;
    unsigned int nof_implamentations;

    unsigned int nof_key_chunk_matrix_cols;
    unsigned int nof_result_chunk_matrix_cols;

    unsigned int *virtual_key_mask = NULL;
    unsigned int *virtual_result_mask = NULL;

    unsigned int *virtual_key_data = NULL;
    unsigned int *virtual_result_data = NULL;

    int sub_unit_idx = PEM_CFG_CORE_ID_TO_SUB_UNIT_ID(core_id);

    SHR_FUNC_INIT_VARS(unit);

    if (cam_info->db_tcam_per_core_arr[core_id].result_chunk_mapper_matrix_arr == NULL
        || cam_info->db_tcam_per_core_arr[core_id].result_chunk_mapper_matrix_arr->db_chunk_mapper == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "PEM Db was NOT mapped during the application run. Unable to write");
    }

    target_key_physical_memory_entry_data_list = NULL;
    target_result_physical_memory_entry_data_list = NULL;

    tcam_key_width = cam_info->db_tcam_per_core_arr[core_id].total_key_width;

    chunk_matrix_row = row_index / PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES;
    nof_implamentations = cam_info->db_tcam_per_core_arr[core_id].nof_chunk_matrices;

    nof_key_chunk_matrix_cols = dnx_get_nof_tcam_chunk_cols(cam_info->db_tcam_per_core_arr[core_id].total_key_width);
    nof_result_chunk_matrix_cols =
        dnx_get_nof_cam_sram_chunk_cols(cam_info->db_tcam_per_core_arr[core_id].total_result_width);

    SHR_IF_ERR_EXIT(dnx_build_virtual_db_mask_and_data_from_key_fields
                    (unit, tcam_key_width, 2 * tcam_key_width + 1,
                     cam_info->db_tcam_per_core_arr[core_id].key_field_bit_range_arr, key, mask, valid,
                     &virtual_key_mask, &virtual_key_data));
    SHR_IF_ERR_EXIT(dnx_build_virtual_db_mask_and_data_from_result_fields
                    (unit, cam_info->db_tcam_per_core_arr[core_id].total_result_width,
                     cam_info->db_tcam_per_core_arr[core_id].result_field_bit_range_arr, data, 1, &virtual_result_mask,
                     &virtual_result_data));

    SHR_IF_ERR_EXIT(dnx_run_over_all_chunks_read_physical_data_and_update_physical_memory_entry_data_list(unit,
                                                                                                          sub_unit_idx,
                                                                                                          1,
                                                                                                          row_index %
                                                                                                          PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES,
                                                                                                          cam_info->db_tcam_per_core_arr
                                                                                                          [core_id].total_key_width,
                                                                                                          chunk_matrix_row,
                                                                                                          nof_implamentations,
                                                                                                          nof_key_chunk_matrix_cols,
                                                                                                          cam_info->db_tcam_per_core_arr
                                                                                                          [core_id].key_chunk_mapper_matrix_arr,
                                                                                                          virtual_key_mask,
                                                                                                          virtual_key_data,
                                                                                                          &target_key_physical_memory_entry_data_list));

    SHR_IF_ERR_EXIT(dnx_run_over_all_chunks_read_physical_data_and_update_physical_memory_entry_data_list(unit,
                                                                                                          sub_unit_idx,
                                                                                                          0,
                                                                                                          row_index
                                                                                                          %
                                                                                                          PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES,
                                                                                                          cam_info->db_tcam_per_core_arr
                                                                                                          [core_id].total_result_width,
                                                                                                          chunk_matrix_row,
                                                                                                          nof_implamentations,
                                                                                                          nof_result_chunk_matrix_cols,
                                                                                                          cam_info->db_tcam_per_core_arr
                                                                                                          [core_id].result_chunk_mapper_matrix_arr,
                                                                                                          virtual_result_mask,
                                                                                                          virtual_result_data,
                                                                                                          &target_result_physical_memory_entry_data_list));

    SHR_IF_ERR_EXIT(dnx_write_all_physical_data_from_list
                    (unit, sub_unit_idx, target_key_physical_memory_entry_data_list));

    SHR_IF_ERR_EXIT(dnx_write_all_physical_data_from_list
                    (unit, sub_unit_idx, target_result_physical_memory_entry_data_list));

exit:
    dnx_free_list(target_key_physical_memory_entry_data_list);
    dnx_free_list(target_result_physical_memory_entry_data_list);
    SHR_FREE(virtual_result_mask);
    SHR_FREE(virtual_result_data);
    SHR_FREE(virtual_key_mask);
    SHR_FREE(virtual_key_data);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_produce_valid_field_bit_range(
    int unit,
    FieldBitRange ** valid_bit_array)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(*valid_bit_array, sizeof(FieldBitRange), "valid_bit_array", "%s%s%s\r\n", EMPTY, EMPTY,
                                EMPTY);

    (*valid_bit_array)->lsb = 0;
    (*valid_bit_array)->msb = 0;
    (*valid_bit_array)->is_field_mapped = 1;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_build_physical_db_key_data_array(
    int unit,
    int sub_unit_idx,
    const LogicalDbChunkMapperMatrix * db_chunk_mapper_matrix,
    const unsigned int nof_chunk_cols,
    const unsigned int chunk_matrix_row_index,
    const unsigned int virtual_row_index,
    const unsigned int total_virtual_key_width,
    unsigned int *virtual_db_data_arr)
{
    unsigned int chunk_matrix_col_index;
    unsigned int chunk_width, phy_mem_entry_offset;
    unsigned int virtual_data_array_offset = 0;
    DbChunkMapper *chunk_mapper;
    unsigned int *read_data = NULL;
    phy_mem_t *phy_mem = NULL;
    unsigned int physical_row_index;
    const unsigned int is_egress = 0;
    const unsigned int is_mem = 1;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(read_data, 2 * sizeof(unsigned int), "read_data", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(phy_mem, sizeof(phy_mem_t), "phy_mem", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    for (chunk_matrix_col_index = 0; chunk_matrix_col_index < nof_chunk_cols; ++chunk_matrix_col_index)
    {
        chunk_width =
            db_chunk_mapper_matrix[0].db_chunk_mapper[chunk_matrix_row_index][chunk_matrix_col_index]->chunk_width;
        phy_mem_entry_offset =
            db_chunk_mapper_matrix[0].
            db_chunk_mapper[chunk_matrix_row_index][chunk_matrix_col_index]->phy_mem_entry_offset;

        chunk_mapper = db_chunk_mapper_matrix[0].db_chunk_mapper[chunk_matrix_row_index][chunk_matrix_col_index];
        physical_row_index = dnx_calculate_physical_row_index_from_chunk_mapper(unit, chunk_mapper, virtual_row_index);
        dnx_init_phy_mem_t_from_chunk_mapper(unit, chunk_mapper->mem_block_id, chunk_mapper->phy_mem_addr,
                                             physical_row_index, chunk_mapper->phy_mem_width, 0, is_egress, is_mem,
                                             phy_mem);

        SHR_IF_ERR_EXIT(pem_read(unit, sub_unit_idx, phy_mem, 1, read_data));

        set_bits(read_data, phy_mem_entry_offset, virtual_data_array_offset, chunk_width, virtual_db_data_arr);

        set_bits(read_data, phy_mem_entry_offset + PEM_CFG_API_CAM_TCAM_CHUNK_WIDTH,
                 virtual_data_array_offset + total_virtual_key_width, chunk_width, virtual_db_data_arr);

        if (chunk_matrix_col_index == nof_chunk_cols - 1)
            set_bits(read_data, 2 * PEM_CFG_API_CAM_TCAM_CHUNK_WIDTH, 2 * total_virtual_key_width, 1,
                     virtual_db_data_arr);

        virtual_data_array_offset += chunk_width;
    }

exit:
    SHR_FREE(read_data);
    SHR_FREE(phy_mem);
    SHR_FUNC_EXIT;
}

int
dnx_pemladrv_does_key_exists_in_physical_db(
    const int unit,
    const LogicalTcamInfo * tcam_info,
    const unsigned char *valids_arr,
    unsigned int **key_entry_arr,
    const unsigned int *key)
{
    const int core_id = 0;
    unsigned int entry_index;

    unsigned int total_key_width = tcam_info->db_tcam_per_core_arr[core_id].total_key_width;

    for (entry_index = 0; entry_index < PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES; ++entry_index)
    {
        if ((1 == valids_arr[entry_index])
            && (dnx_pemladrv_copmare_two_virtual_keys(total_key_width, key, key_entry_arr[entry_index])))
            return entry_index;
    }
    return -1;
}

shr_error_e
pemladrv_tcam_write(
    int unit,
    int core_id,
    tcam_id_t tcam_id,
    uint32 row_index,
    pemladrv_mem_t * key,
    pemladrv_mem_t * mask,
    pemladrv_mem_t * valid,
    pemladrv_mem_t * data)
{
    SHR_FUNC_INIT_VARS(unit);

    if (core_id == -1)
    {
        for (int core_i = 0; core_i < PEM_CFG_API_NOF_CORES_DYNAMIC; ++core_i)
        {
            SHR_IF_ERR_EXIT(dnx_logical_cam_based_db_write
                            (&api_info[unit].db_tcam_container.db_tcam_info_arr[tcam_id], unit, core_i, row_index, key,
                             mask, valid, data));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_logical_cam_based_db_write
                        (&api_info[unit].db_tcam_container.db_tcam_info_arr[tcam_id], unit, core_id, row_index, key,
                         mask, valid, data));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
pemladrv_tcam_read(
    const int unit,
    const int core_id,
    tcam_id_t tcam_id,
    uint32 row_index,
    pemladrv_mem_t * key,
    pemladrv_mem_t * mask,
    pemladrv_mem_t * valid,
    pemladrv_mem_t * result)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_logical_cam_based_db_read
                    (&api_info[unit].db_tcam_container.db_tcam_info_arr[tcam_id], unit, core_id, row_index, key, mask,
                     valid, result));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_pemladrv_tcam_info_allocate(
    const int unit,
    pemladrv_db_info ** arr,
    int *size)
{
    int mapped_tcam_size = 0;
    int name_len, nof_key_fields, nof_fields;
    unsigned int tcam_index, field_index;
    const int core_id = 0;
    FieldBitRange *tcam_field_key_info_arr, *tcam_field_result_info_arr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(*arr, api_info[unit].db_tcam_container.nof_tcam_dbs * sizeof(pemladrv_db_info), "arr",
                                "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    for (tcam_index = 0; tcam_index < api_info[unit].db_tcam_container.nof_tcam_dbs; ++tcam_index)
    {
        LogicalTcamInfo *tcam_info_arr = &api_info[unit].db_tcam_container.db_tcam_info_arr[tcam_index];

        if (api_info[unit].db_tcam_container.db_tcam_info_arr == NULL
            || api_info[unit].db_tcam_container.db_tcam_info_arr[tcam_index].
            db_tcam_per_core_arr[core_id].result_chunk_mapper_matrix_arr == NULL)
        {
            continue;
        }

        (*arr)[mapped_tcam_size].db_id = tcam_index;
        name_len =
            sal_strnlen(api_info[unit].db_tcam_container.db_tcam_info_arr[tcam_index].
                        db_tcam_per_core_arr[core_id].name, MAX_NAME_LENGTH);
        SHR_ALLOC_SET_ZERO_ERR_EXIT((*arr)[mapped_tcam_size].db_name, name_len + 1, "(*arr)[].db_name", "%s%s%s\r\n",
                                    EMPTY, EMPTY, EMPTY);

        sal_strncpy((*arr)[mapped_tcam_size].db_name,
                    api_info[unit].db_tcam_container.db_tcam_info_arr[tcam_index].db_tcam_per_core_arr[core_id].name,
                    name_len);
        (*arr)[mapped_tcam_size].db_name[name_len] = '\0';

        nof_key_fields =
            api_info[unit].db_tcam_container.db_tcam_info_arr[tcam_index].
            db_tcam_per_core_arr[core_id].nof_fields_in_key;

        nof_fields = nof_key_fields;

        (*arr)[mapped_tcam_size].nof_fields = nof_fields;

        SHR_ALLOC_SET_ZERO_ERR_EXIT((*arr)[mapped_tcam_size].ptr,
                                    (*arr)[mapped_tcam_size].nof_fields * sizeof(pemladrv_db_field_info),
                                    "(*arr)[].ptr", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        for (field_index = 0; field_index < nof_fields; ++field_index)
        {
            tcam_field_key_info_arr =
                &tcam_info_arr->db_tcam_per_core_arr[core_id].key_field_bit_range_arr[field_index];
            tcam_field_result_info_arr =
                &tcam_info_arr->db_tcam_per_core_arr[core_id].result_field_bit_range_arr[field_index];

            if (tcam_field_key_info_arr->is_field_mapped)
            {
                (*arr)[mapped_tcam_size].ptr[field_index].is_field_mapped = tcam_field_key_info_arr->is_field_mapped;
                (*arr)[mapped_tcam_size].ptr[field_index].field_id = field_index;
                (*arr)[mapped_tcam_size].ptr[field_index].field_size_in_bits =
                    tcam_field_key_info_arr->msb - tcam_field_key_info_arr->lsb + 1;

                (*arr)[mapped_tcam_size].total_nof_entries =
                    api_info[unit].db_tcam_container.db_tcam_info_arr[tcam_index].
                    db_tcam_per_core_arr->total_nof_entries;
                (*arr)[mapped_tcam_size].total_key_width =
                    api_info[unit].db_tcam_container.db_tcam_info_arr[tcam_index].db_tcam_per_core_arr->total_key_width;

            }

            if (tcam_field_result_info_arr->is_field_mapped)
            {
                (*arr)[mapped_tcam_size].ptr[field_index].is_field_mapped = tcam_field_result_info_arr->is_field_mapped;

                (*arr)[mapped_tcam_size].ptr[field_index].field_size_in_bits +=
                    tcam_field_result_info_arr->msb - tcam_field_result_info_arr->lsb + 1;

                (*arr)[mapped_tcam_size].total_result_width =
                    api_info[unit].db_tcam_container.db_tcam_info_arr[tcam_index].
                    db_tcam_per_core_arr->total_result_width;
            }
        }

        mapped_tcam_size++;
    }

    *size = mapped_tcam_size;

exit:
    SHR_FUNC_EXIT;
}

void
dnx_pemladrv_tcam_info_de_allocate(
    const int unit,
    pemladrv_db_info * ptr)
{
    pemladrv_db_info temp;
    if (ptr)
    {
        for (unsigned int i = 0; i < api_info[unit].db_tcam_container.nof_tcam_dbs; ++i)
        {
            temp = ptr[i];
            SHR_FREE(temp.db_name);
            SHR_FREE(temp.ptr);
        }
        SHR_FREE(ptr);
    }
}

shr_error_e
dnx_pemladrv_build_tcam_valid(
    int unit,
    pemladrv_mem_t ** tcam)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(*tcam, sizeof(pemladrv_mem_t), "tcam", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO_ERR_EXIT((*tcam)->fields, sizeof(pemladrv_field_t *), "(*tcam)->fields", "%s%s%s\r\n", EMPTY,
                                EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO_ERR_EXIT((*tcam)->fields[0], sizeof(pemladrv_field_t), "(*tcam)->fields[0]", "%s%s%s\r\n", EMPTY,
                                EMPTY, EMPTY);

    (*tcam)->flags = 0;
    (*tcam)->nof_fields = 1;
    (*tcam)->fields[0]->flags = 0;
    (*tcam)->fields[0]->field_id = 0;
    SHR_ALLOC_SET_ZERO_ERR_EXIT((*tcam)->fields[0]->fldbuf, sizeof(unsigned int), "(*tcam)->fields[0]->fldbuf",
                                "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    (*tcam)->fields[0]->fldbuf[0] = 1;

exit:
    SHR_FUNC_EXIT;
}

unsigned int
dnx_pemladrv_get_nof_chunks_in_fldbuf(
    const unsigned int field_index,
    const FieldBitRange * field_bit_range_arr)
{
    int field_width = field_bit_range_arr[field_index].msb - field_bit_range_arr[field_index].lsb + 1;
    return dnx_pemladrv_get_nof_unsigned_int_chunks(field_width);
}

unsigned int
dnx_pemladrv_copmare_two_virtual_keys(
    const unsigned int key_width_in_bits,
    const unsigned int *first_key,
    const unsigned int *second_key)
{
    int nof_chunks = dnx_pemladrv_get_nof_unsigned_int_chunks(key_width_in_bits);
    unsigned int last_chunk_of_first_key, last_chunk_of_second_key;
    int chunk_index;

    for (chunk_index = 0; chunk_index < nof_chunks - 1; ++chunk_index)
    {
        if (first_key[chunk_index] != second_key[chunk_index])
            return 0;
    }

    last_chunk_of_first_key = first_key[chunk_index] << (32 - (key_width_in_bits % 32));
    last_chunk_of_second_key = second_key[chunk_index] << (32 - (key_width_in_bits % 32));
    if (last_chunk_of_first_key != last_chunk_of_second_key)
        return 0;
    return 1;
}

shr_error_e
dnx_pemladrv_find_physical_entry_index_by_key(
    const int unit,
    const int core_id,
    const LogicalTcamInfo * tcam_info,
    const unsigned char *valids_arr,
    unsigned int **key_field_arr,
    const pemladrv_mem_t * key,
    int *index)
{
    FieldBitRange *key_field_bit_rang_arr = tcam_info->db_tcam_per_core_arr[core_id].key_field_bit_range_arr;
    unsigned int total_key_width = tcam_info->db_tcam_per_core_arr[core_id].total_key_width;
    unsigned int *virtual_key_data = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(virtual_key_data,
                                (size_t) pemladrv_ceil(total_key_width, UINT_WIDTH_IN_BITS) * sizeof(uint32),
                                "virtual_key_data", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    dnx_build_virtual_db_data(0, key_field_bit_rang_arr, key, virtual_key_data);
    *index = dnx_pemladrv_does_key_exists_in_physical_db(unit, tcam_info, valids_arr, key_field_arr, virtual_key_data);

exit:
    SHR_FREE(virtual_key_data);
    SHR_FUNC_EXIT;
}

void
dnx_pemladrv_update_key_with_prefix_length(
    const int prefix_length,
    const int key_length,
    unsigned int *virtual_key_data)
{
    unsigned int zeros_arr[10] = { 0 };
    set_bits(zeros_arr, 0, 0, key_length - prefix_length, virtual_key_data);
}

shr_error_e
pemladrv_mem_alloc_tcam(
    const int unit,
    const int db_id,
    pemladrv_mem_t ** key,
    pemladrv_mem_t ** mask,
    pemladrv_mem_t ** valid,
    pemladrv_mem_t ** result)
{
    int nof_fields_in_key, nof_fields_in_result;
    const int core_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].
        db_tcam_per_core_arr[core_id].result_chunk_mapper_matrix_arr == NULL
        || api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].
        db_tcam_per_core_arr[core_id].result_chunk_mapper_matrix_arr->db_chunk_mapper == NULL)
    {
        ;
        SHR_ERR_EXIT(_SHR_E_PARAM, "PEM Db was NOT mapped during the application run");
    }

    nof_fields_in_key =
        api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].db_tcam_per_core_arr[core_id].nof_fields_in_key;
    nof_fields_in_result =
        api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].db_tcam_per_core_arr[core_id].nof_fields_in_result;
    SHR_IF_ERR_EXIT(pemladrv_mem_alloc(unit, nof_fields_in_key, key));
    SHR_IF_ERR_EXIT(pemladrv_mem_alloc(unit, nof_fields_in_key, mask));
    SHR_IF_ERR_EXIT(pemladrv_mem_alloc(unit, 1, valid));
    SHR_IF_ERR_EXIT(pemladrv_mem_alloc(unit, nof_fields_in_result, result));

    SHR_IF_ERR_EXIT(dnx_pemladrv_pem_allocate_fldbuf_by_bit_range_array(unit, nof_fields_in_key,
                                                                        api_info[unit].
                                                                        db_tcam_container.db_tcam_info_arr
                                                                        [db_id].db_tcam_per_core_arr
                                                                        [core_id].key_field_bit_range_arr, *key));
    SHR_IF_ERR_EXIT(dnx_pemladrv_pem_allocate_fldbuf_by_bit_range_array
                    (unit, nof_fields_in_key,
                     api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].
                     db_tcam_per_core_arr[core_id].key_field_bit_range_arr, *mask));

    SHR_IF_ERR_EXIT(pemladrv_mem_alloc_field(unit, (*valid)->fields[0], 1));
    (*valid)->fields[0]->field_id = 0;

    SHR_IF_ERR_EXIT(dnx_pemladrv_pem_allocate_fldbuf_by_bit_range_array(unit, nof_fields_in_result,
                                                                        api_info[unit].
                                                                        db_tcam_container.db_tcam_info_arr
                                                                        [db_id].db_tcam_per_core_arr
                                                                        [core_id].key_field_bit_range_arr, *result));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_pemladrv_set_valid_bit_to_zero(
    const int unit,
    const int core_id,
    const int virtual_row_index,
    LogicalTcamInfo * tcam_info)
{
    int physical_row_index;
    unsigned int nof_tcam_chunk_cols;
    unsigned int implementation_index, nof_implementations;
    unsigned int entry_data[2] = { 0xFFFFFFFF, 0 };
    DbChunkMapper *chunk_mapper;
    phy_mem_t phy_mem;
    const unsigned int is_ingress = 0;
    const unsigned int is_mem = 1;

    int sub_unit_idx = PEM_CFG_CORE_ID_TO_SUB_UNIT_ID(core_id);

    SHR_FUNC_INIT_VARS(unit);

    nof_tcam_chunk_cols = dnx_get_nof_tcam_chunk_cols(tcam_info->db_tcam_per_core_arr[core_id].total_key_width);
    nof_implementations = tcam_info->db_tcam_per_core_arr[core_id].nof_chunk_matrices;

    for (implementation_index = 0; implementation_index < nof_implementations; ++implementation_index)
    {
        chunk_mapper =
            tcam_info->db_tcam_per_core_arr[core_id].
            key_chunk_mapper_matrix_arr[implementation_index].db_chunk_mapper[0][nof_tcam_chunk_cols - 1];
        physical_row_index = dnx_calculate_physical_row_index_from_chunk_mapper(unit, chunk_mapper, virtual_row_index);
        dnx_init_phy_mem_t_from_chunk_mapper(unit,
                                             chunk_mapper->mem_block_id,
                                             chunk_mapper->phy_mem_addr,
                                             physical_row_index,
                                             chunk_mapper->phy_mem_width, 0, is_ingress, is_mem, &phy_mem);

        SHR_IF_ERR_EXIT(pem_write(unit, sub_unit_idx, &phy_mem, 1, entry_data, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

unsigned int
dnx_get_nof_tcam_chunk_cols(
    const unsigned int total_key_width)
{
    if (total_key_width % PEM_CFG_API_CAM_TCAM_CHUNK_WIDTH == 0)
        return (total_key_width / PEM_CFG_API_CAM_TCAM_CHUNK_WIDTH);
    else
        return (total_key_width / PEM_CFG_API_CAM_TCAM_CHUNK_WIDTH) + 1;
}

int
dnx_calculate_physical_row_index_from_chunk_mapper(
    const int unit,
    const DbChunkMapper * chunk_mapper,
    const int virtual_row_index)
{
    const unsigned int internal_chunk_offset = (virtual_row_index % PEM_CFG_API_MAP_CHUNK_N_ENTRIES);
    return chunk_mapper->phy_mem_entry_offset + internal_chunk_offset;
}

shr_error_e
dnx_pemladrv_zero_last_chunk_of_cam_based_dbs(
    const int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pemladrv_zero_last_chunk_of_tcam_dbs(unit));

    SHR_IF_ERR_EXIT(dnx_pemladrv_zero_last_chunk_of_lpm_dbs(unit));

    SHR_IF_ERR_EXIT(dnx_pemladrv_zero_last_chunk_of_em_dbs(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_pemladrv_zero_last_chunk_of_tcam_dbs(
    const int unit)
{
    int i, entry_index, nof_dbs;

    SHR_FUNC_INIT_VARS(unit);

    nof_dbs = api_info[unit].db_tcam_container.nof_tcam_dbs;

    for (int core_id = 0; core_id < PEM_CFG_API_NOF_CORES_DYNAMIC; ++core_id)
    {
        for (i = 0; i < nof_dbs; ++i)
        {
            if (NULL ==
                api_info[unit].db_tcam_container.db_tcam_info_arr[i].
                db_tcam_per_core_arr[core_id].result_chunk_mapper_matrix_arr)

                continue;
            for (entry_index = 0; entry_index < PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES; ++entry_index)
                SHR_IF_ERR_EXIT(dnx_pemladrv_set_valid_bit_to_zero(unit, core_id, entry_index,
                                                                   &api_info[unit].
                                                                   db_tcam_container.db_tcam_info_arr[i]));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_pemladrv_read_physical_and_update_cache(
    const int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pemladrv_update_lpm_cache_from_physical(unit));
    SHR_IF_ERR_EXIT(dnx_pemladrv_update_em_cache_from_physical(unit));

exit:
    SHR_FUNC_EXIT;
}
