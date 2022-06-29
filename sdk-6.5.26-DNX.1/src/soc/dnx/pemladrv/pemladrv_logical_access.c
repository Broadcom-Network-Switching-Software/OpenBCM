
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "pemladrv_logical_access.h"
#include "pemladrv_meminfo_init.h"
#include "pemladrv_meminfo_access.h"
#include "pemladrv_cfg_api_defines.h"
#include "virtual_dbs/pemladrv_direct_logical_access.h"

#include <stdio.h>
#include <limits.h>

#ifdef BCM_DNX_SUPPORT
#include "soc/register.h"
#include "soc/mem.h"
#include "soc/drv.h"
#include <soc/dnx/pemladrv/pemladrv.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/utilex/utilex_str.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <shared/utilex/utilex_bitstream.h>

#ifdef BCM_SAND_SUPPORT
#include <soc/sand/sand_aux_access.h>
#endif
#endif

#ifdef BCM_DNX_SUPPORT
#define BSL_LOG_MODULE BSL_LS_SOCDNX_ADAPTERDNX
#endif

#ifndef DB_OBJECT_ACCESS

extern ApiInfo api_info[MAX_NOF_UNITS];

#ifdef _MSC_VER
#if (_MSC_VER >= 1400)
#pragma warning(push)
#pragma warning(disable: 4996)
#endif
#endif

void
get_bits(
    const unsigned int *in_buff,
    int start_bit_indx,
    int nof_bits,
    unsigned int *out_buff)
{
    int out_uint_indx = 0;
    int in_uint_indx = start_bit_indx / 32;
    int n_to_shift = start_bit_indx % 32;
    int nof_written_bits = 0;

    for (; nof_written_bits < nof_bits; out_uint_indx++, in_uint_indx++)
    {

        out_buff[out_uint_indx] = (in_buff[in_uint_indx] >> n_to_shift);
        if ((n_to_shift != 0) && (n_to_shift + nof_bits) > 32)
        {
            out_buff[out_uint_indx] |= (in_buff[in_uint_indx + 1] << (32 - n_to_shift));
        }
        nof_written_bits = nof_written_bits + 32;
    }
}

void
set_bits(
    const unsigned int *in_buff,
    int in_buff_start_bit_indx,
    int out_buff_start_bit_indx,
    int nof_bits,
    unsigned int *out_buff)
{
    int in_uint_indx = 0;
    int out_uint_indx = out_buff_start_bit_indx / 32;
    int n_to_shift = out_buff_start_bit_indx % 32;
    int nof_written_bits = 0;
    unsigned int local_buff[1024] = { 0 };
    unsigned int out_buff_mask = 0xffffffff;

    if (nof_bits == 0)
        return;

    get_bits(in_buff, in_buff_start_bit_indx, nof_bits, local_buff);

    if ((nof_bits + n_to_shift) < 32)
    {

        unsigned int mask = (1 << nof_bits) - 1;
        local_buff[0] = (local_buff[0] & mask) << n_to_shift;

        out_buff_mask = (out_buff_mask << (32 - nof_bits)) >> (32 - (nof_bits + n_to_shift));
        out_buff[out_uint_indx] = out_buff[out_uint_indx] & (~out_buff_mask);

        out_buff[out_uint_indx] = out_buff[out_uint_indx] | local_buff[0];

        return;
    }

    for (; nof_written_bits < nof_bits; out_uint_indx++, in_uint_indx++)
    {

        if (n_to_shift > 0)
        {
            out_buff[out_uint_indx] =
                (out_buff[out_uint_indx] & (out_buff_mask >> (32 - n_to_shift))) | (local_buff[in_uint_indx] <<
                                                                                    n_to_shift);
        }
        else
        {
            if (nof_bits - nof_written_bits < 32)
            {
                unsigned int mask = (1 << nof_bits) - 1;
                local_buff[in_uint_indx] = (local_buff[in_uint_indx] & mask) << n_to_shift;

                out_buff_mask = (out_buff_mask << (32 - nof_bits)) >> (32 - (nof_bits + n_to_shift));
                out_buff[out_uint_indx] = out_buff[out_uint_indx] & (~out_buff_mask);

                out_buff[out_uint_indx] = out_buff[out_uint_indx] | local_buff[in_uint_indx];
                return;
            }
            out_buff[out_uint_indx] = local_buff[in_uint_indx];
        }
        nof_written_bits = nof_written_bits + (32 - n_to_shift);

        if ((n_to_shift > 0) && (nof_written_bits < nof_bits))
        {
            const unsigned int nof_remaining_bits = (nof_bits - nof_written_bits);
            unsigned int local_buff_mask;

            out_buff_mask = (out_buff_mask << (nof_bits - (32 - n_to_shift)));
            local_buff_mask = (nof_remaining_bits > 31) ? 0xffffffff : (~out_buff_mask);
            out_buff[out_uint_indx + 1] =
                (out_buff[out_uint_indx + 1] & (out_buff_mask)) | ((local_buff[in_uint_indx] >> (32 - n_to_shift)) &
                                                                   local_buff_mask);
            nof_written_bits = nof_written_bits + n_to_shift;
            out_buff_mask = 0xffffffff;
        }

    }
}

unsigned int
count_set_bits_in_unsinged_int_array(
    unsigned int nof_chunks,
    unsigned int *mapped_bits_arr)
{
    unsigned int count = 0;
    for (unsigned int i = 0; i < nof_chunks; ++i)
    {
        for (unsigned int bit_index = 0; bit_index < NOF_BITS_IN_WORD; bit_index++)
        {
            count += mapped_bits_arr[i] & 1;
            mapped_bits_arr[i] >>= 1;
        }
    }
    return count;
}

shr_error_e
dnx_get_version(
    const int unit,
    int print_to_cout,
    VersionInfo ** version_info)
{
    unsigned int version_filter = 0x0FFFFF00;
    unsigned int revision_filter = 0x000000FF;
    unsigned int year_filter = 0x00000FFF;
    unsigned int month_filter = 0x0000F000;
    unsigned int day_filter = 0x000F0000;

    unsigned int version, revision;
    unsigned int chuck_year, chuck_month, chuck_day;
    unsigned int device_year, device_month, device_day;
    unsigned int block_id0, block_id1;
    unsigned int pem_reg00_address_val, pem_reg01_address_val, pem_reg02_address_val, pem_reg03_address_val;
    unsigned int pem_reg10_address_val, pem_reg11_address_val, pem_reg12_address_val, pem_reg13_address_val;
    phy_mem_t spare_reg00, spare_reg01, spare_reg02, spare_reg03;
    phy_mem_t spare_reg10, spare_reg11, spare_reg12, spare_reg13;
    unsigned int entry_data00, entry_data01, entry_data02, entry_data03;
    unsigned int entry_data10, entry_data11, entry_data12, entry_data13;
    int sub_unit_idx = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(hexstr2addr
                    (unit, api_info[unit].version_info.spare_registers_addr_arr[0], &block_id0,
                     &pem_reg00_address_val));
    SHR_IF_ERR_EXIT(hexstr2addr
                    (unit, api_info[unit].version_info.spare_registers_addr_arr[1], &block_id0,
                     &pem_reg01_address_val));
    SHR_IF_ERR_EXIT(hexstr2addr
                    (unit, api_info[unit].version_info.spare_registers_addr_arr[2], &block_id0,
                     &pem_reg02_address_val));
    SHR_IF_ERR_EXIT(hexstr2addr
                    (unit, api_info[unit].version_info.spare_registers_addr_arr[3], &block_id0,
                     &pem_reg03_address_val));
    SHR_IF_ERR_EXIT(hexstr2addr
                    (unit, api_info[unit].version_info.spare_registers_addr_arr[4], &block_id1,
                     &pem_reg10_address_val));
    SHR_IF_ERR_EXIT(hexstr2addr
                    (unit, api_info[unit].version_info.spare_registers_addr_arr[5], &block_id1,
                     &pem_reg11_address_val));
    SHR_IF_ERR_EXIT(hexstr2addr
                    (unit, api_info[unit].version_info.spare_registers_addr_arr[6], &block_id1,
                     &pem_reg12_address_val));
    SHR_IF_ERR_EXIT(hexstr2addr
                    (unit, api_info[unit].version_info.spare_registers_addr_arr[7], &block_id1,
                     &pem_reg13_address_val));

    dnx_init_phy_mem_t_from_chunk_mapper(unit, block_id0, pem_reg00_address_val, 0, 32, 0, 0, 0, &spare_reg00);
    dnx_init_phy_mem_t_from_chunk_mapper(unit, block_id0, pem_reg01_address_val, 0, 32, 0, 0, 0, &spare_reg01);
    dnx_init_phy_mem_t_from_chunk_mapper(unit, block_id0, pem_reg02_address_val, 0, 32, 0, 0, 0, &spare_reg02);
    dnx_init_phy_mem_t_from_chunk_mapper(unit, block_id0, pem_reg03_address_val, 0, 32, 0, 0, 0, &spare_reg03);
    dnx_init_phy_mem_t_from_chunk_mapper(unit, block_id1, pem_reg10_address_val, 0, 32, 0, 0, 0, &spare_reg10);
    dnx_init_phy_mem_t_from_chunk_mapper(unit, block_id1, pem_reg11_address_val, 0, 32, 0, 0, 0, &spare_reg11);
    dnx_init_phy_mem_t_from_chunk_mapper(unit, block_id1, pem_reg12_address_val, 0, 32, 0, 0, 0, &spare_reg12);
    dnx_init_phy_mem_t_from_chunk_mapper(unit, block_id1, pem_reg13_address_val, 0, 32, 0, 0, 0, &spare_reg13);

    SHR_IF_ERR_EXIT(pem_read(unit, sub_unit_idx, &spare_reg00, 0, &entry_data00));
    SHR_IF_ERR_EXIT(pem_read(unit, sub_unit_idx, &spare_reg01, 0, &entry_data01));
    SHR_IF_ERR_EXIT(pem_read(unit, sub_unit_idx, &spare_reg02, 0, &entry_data02));
    SHR_IF_ERR_EXIT(pem_read(unit, sub_unit_idx, &spare_reg03, 0, &entry_data03));
    SHR_IF_ERR_EXIT(pem_read(unit, sub_unit_idx, &spare_reg10, 0, &entry_data10));
    SHR_IF_ERR_EXIT(pem_read(unit, sub_unit_idx, &spare_reg11, 0, &entry_data11));
    SHR_IF_ERR_EXIT(pem_read(unit, sub_unit_idx, &spare_reg12, 0, &entry_data12));
    SHR_IF_ERR_EXIT(pem_read(unit, sub_unit_idx, &spare_reg13, 0, &entry_data13));

    version = version_filter & entry_data10;
    version = version >> 8;
    revision = revision_filter & entry_data10;

    chuck_year = entry_data11 & year_filter;
    chuck_month = entry_data11 & month_filter;
    chuck_day = entry_data11 & day_filter;
    chuck_month = chuck_month >> 12;
    chuck_day = chuck_day >> 16;
    device_year = entry_data12 & year_filter;
    device_month = entry_data12 & month_filter;
    device_day = entry_data12 & day_filter;
    device_month = device_month >> 12;
    device_day = device_day >> 16;

    sal_snprintf(api_info[unit].version_info.version_info_str, 5000,
                 " Device is: %04d_%02x.\n Chuck lib create date: %d.%d.%d.\n Device lib create date: %d.%d.%d.\n Chuck lib signature: %08x%08x.\n Device lib signature: %08x%08x.\n",
                 version, revision, chuck_day, chuck_month, chuck_year, device_day, device_month, device_year,
                 entry_data01, entry_data00, entry_data03, entry_data02);
    sal_snprintf(api_info[unit].version_info.version_str, MAX_NAME_LENGTH, "%d", entry_data13);
    sal_snprintf(api_info[unit].version_info.device_str, MAX_NAME_LENGTH, "%04d_%02x", version, revision);
    sal_snprintf(api_info[unit].version_info.chuck_lib_date_str, MAX_NAME_LENGTH, "%d.%d.%d", chuck_day, chuck_month,
                 chuck_year);
    sal_snprintf(api_info[unit].version_info.device_lib_date_str, MAX_NAME_LENGTH, "%d.%d.%d", device_day, device_month,
                 device_year);
    sal_snprintf(api_info[unit].version_info.chuck_lib_signature_str, MAX_NAME_LENGTH, "%08x%08x", entry_data01,
                 entry_data00);
    sal_snprintf(api_info[unit].version_info.device_lib_signature_str, MAX_NAME_LENGTH, "%08x%08x", entry_data03,
                 entry_data02);

    if (print_to_cout)
    {
        sal_printf
            (" Device is: %04d_%02x.\n Chuck lib create date: %d.%d.%d.\n Device lib create date: %d.%d.%d.\n Chuck lib signature: %08x%08x.\n Device lib signature: %08x%08x.\n",
             version, revision, chuck_day, chuck_month, chuck_year, device_day, device_month, device_year, entry_data01,
             entry_data00, entry_data03, entry_data02);
    }

exit:
    *version_info = &api_info[unit].version_info;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_pemladrv_init(
    const int unit,
    int restore_after_reset,
    int use_file,
    const char *rel_file_path,
    const char *file_name)
{
    const char *fname;

    SHR_FUNC_INIT_VARS(unit);

    if (file_name != NULL)
    {
        fname = file_name;
    }
    else
    {
        fname = PEM_DEFAULT_DB_MEMORY_MAP_FILE;
    }

    SHR_IF_ERR_EXIT(parse_meminfo_definition_file(unit, restore_after_reset, use_file, rel_file_path, fname));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_pemladrv_deinit(
    const int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    free_api_info(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_pemladrv_dnx_init_all_db_arr_by_size(
    const int unit,
    const char *line)
{
    int nof_reg, nof_direct_dbs, nof_tcam_dbs, nof_em_dbs, nof_lpm_dbs, dummy;
    char key_word[MAX_NAME_LENGTH];

    SHR_FUNC_INIT_VARS(unit);

    sscanf(line, "%s  %d %d %d %d %d %d", key_word, &dummy, &nof_direct_dbs, &nof_tcam_dbs, &nof_em_dbs, &nof_lpm_dbs,
           &nof_reg);

    SHR_IF_ERR_EXIT(db_mapping_info_init(unit, nof_direct_dbs));
    SHR_IF_ERR_EXIT(reg_mapping_info_init(unit, nof_reg));
    SHR_IF_ERR_EXIT(tcam_mapping_info_init(unit, nof_tcam_dbs));
    SHR_IF_ERR_EXIT(em_mapping_info_init(unit, nof_em_dbs));
    SHR_IF_ERR_EXIT(lpm_mapping_info_init(unit, nof_lpm_dbs));

exit:
    SHR_FUNC_EXIT;
}

void
dnx_set_mask_with_ones(
    const unsigned int msb_bit,
    const unsigned int lsb_bit,
    unsigned int *virtual_field_mask)
{
    unsigned int curr_lsb = lsb_bit;
    unsigned int field_width, virtual_mask_arr_index;
    unsigned int lsb_offset;
    unsigned int msb_offset = msb_bit % UINT_WIDTH_IN_BITS;

    while (curr_lsb <= msb_bit)
    {
        virtual_mask_arr_index = curr_lsb / UINT_WIDTH_IN_BITS;
        lsb_offset = curr_lsb % UINT_WIDTH_IN_BITS;
        field_width = msb_bit - curr_lsb + 1;
        if (lsb_offset + field_width > UINT_WIDTH_IN_BITS)
        {
            dnx_set_ones_in_chunk(lsb_offset, UINT_WIDTH_IN_BITS - 1, virtual_mask_arr_index, virtual_field_mask);
            curr_lsb += (UINT_WIDTH_IN_BITS - lsb_offset);
            continue;
        }
        dnx_set_ones_in_chunk(lsb_offset, msb_offset, virtual_mask_arr_index, virtual_field_mask);
        curr_lsb += (msb_offset - lsb_offset + 1);
    }
}

void
dnx_set_ones_in_chunk(
    const unsigned int lsb_bit,
    const unsigned int msb_bit,
    const unsigned int virtual_mask_arr_index,
    unsigned int *virtual_mask_arr)
{
    unsigned int ones_mask = 0xffffffff;
    ones_mask = ones_mask >> (UINT_WIDTH_IN_BITS - (msb_bit - lsb_bit + 1));
    ones_mask = ones_mask << lsb_bit;
    virtual_mask_arr[virtual_mask_arr_index] = virtual_mask_arr[virtual_mask_arr_index] | ones_mask;
}

int
dnx_do_chunk_require_writing(
    const DbChunkMapper * chunk_mapper,
    const unsigned int *virtual_field_mask)
{
    unsigned int temp_mask[MAX_MEM_DATA_LENGTH / UINT_WIDTH_IN_BITS] = { 0 };
    unsigned int lsb = chunk_mapper->virtual_mem_width_offset;
    unsigned int msb = lsb + chunk_mapper->chunk_width;
    int i;
    int do_write = 0;
    int array_size = chunk_mapper->chunk_width / UINT_WIDTH_IN_BITS + 1;

    dnx_set_mask_with_ones(msb, lsb, temp_mask);
    for (i = lsb / UINT_WIDTH_IN_BITS; i < (int) (array_size + lsb / UINT_WIDTH_IN_BITS); ++i)
    {
        temp_mask[i] = temp_mask[i] & virtual_field_mask[i];
        if (temp_mask[i] != 0)
            do_write = 1;
    }
    return do_write;
}

shr_error_e
dnx_modify_entry_data(
    int unit,
    const unsigned char flag,
    const unsigned char last_chunk,
    const unsigned int chunk_mapping_width,
    const unsigned int chunk_virtual_mem_width_offset,
    const unsigned int chunk_physical_mem_width_offset,
    const int total_key_width,
    const unsigned int *virtual_db_line_mask_arr,
    const unsigned int *virtual_db_line_input_data_arr,
    unsigned int *physical_memory_entry_data)
{

    unsigned int *one_chunk_virt_mask = NULL;
    unsigned int *one_chunk_virt_data = NULL;
    int last_physical_data_arr_entry_to_update;

    int nof_entries_to_update = -1;
    int i;
    int physical_data_arr_entry_offset = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (flag == TCAM)
    {
        physical_data_arr_entry_offset = 0;

        SHR_ALLOC_SET_ZERO_ERR_EXIT(one_chunk_virt_data,
                                    (1 +
                                     (size_t) pemladrv_ceil(chunk_mapping_width, UINT_WIDTH_IN_BITS)) * sizeof(uint32),
                                    "one_chunk_virt_data", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        SHR_ALLOC_SET_ZERO_ERR_EXIT(one_chunk_virt_mask,
                                    (1 +
                                     (size_t) pemladrv_ceil(chunk_mapping_width, UINT_WIDTH_IN_BITS)) * sizeof(uint32),
                                    "one_chunk_virt_mask", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        nof_entries_to_update = 2;

        set_bits(virtual_db_line_mask_arr, chunk_virtual_mem_width_offset, 0, chunk_mapping_width, one_chunk_virt_mask);

        set_bits(virtual_db_line_input_data_arr, chunk_virtual_mem_width_offset, 0, chunk_mapping_width,
                 one_chunk_virt_data);

        set_bits(virtual_db_line_mask_arr, chunk_virtual_mem_width_offset + total_key_width,
                 PEM_CFG_API_CAM_TCAM_KEY_WIDTH, chunk_mapping_width, one_chunk_virt_mask);

        set_bits(virtual_db_line_input_data_arr, chunk_virtual_mem_width_offset + total_key_width,
                 PEM_CFG_API_CAM_TCAM_KEY_WIDTH, chunk_mapping_width, one_chunk_virt_data);

        set_bits(virtual_db_line_mask_arr, (2 * total_key_width), 2 * PEM_CFG_API_CAM_TCAM_KEY_WIDTH, 1,
                 one_chunk_virt_mask);

        set_bits(virtual_db_line_input_data_arr, (2 * total_key_width), 2 * PEM_CFG_API_CAM_TCAM_KEY_WIDTH, 1,
                 one_chunk_virt_data);

        if (last_chunk)
        {
            unsigned int ones[1];
            ones[0] = 0xFFFFFFFF;

            set_bits(ones, 0, PEM_CFG_API_CAM_TCAM_KEY_WIDTH + chunk_mapping_width,
                     PEM_CFG_API_CAM_TCAM_KEY_WIDTH - chunk_mapping_width, one_chunk_virt_mask);

            set_bits(ones, 0, PEM_CFG_API_CAM_TCAM_KEY_WIDTH + chunk_mapping_width,
                     PEM_CFG_API_CAM_TCAM_KEY_WIDTH - chunk_mapping_width, one_chunk_virt_data);
        }
    }
    else
    {
        assert(flag == SRAM);
        last_physical_data_arr_entry_to_update =
            (chunk_physical_mem_width_offset + chunk_mapping_width) / UINT_WIDTH_IN_BITS;
        physical_data_arr_entry_offset = chunk_physical_mem_width_offset / UINT_WIDTH_IN_BITS;
        nof_entries_to_update = last_physical_data_arr_entry_to_update - physical_data_arr_entry_offset + 1;

        SHR_ALLOC_SET_ZERO_ERR_EXIT(one_chunk_virt_mask,
                                    (1 +
                                     (size_t) pemladrv_ceil((chunk_physical_mem_width_offset + chunk_mapping_width),
                                                            UINT_WIDTH_IN_BITS)) * sizeof(uint32),
                                    "one_chunk_virt_mask", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        SHR_ALLOC_SET_ZERO_ERR_EXIT(one_chunk_virt_data,
                                    (1 +
                                     (size_t) pemladrv_ceil((chunk_physical_mem_width_offset + chunk_mapping_width),
                                                            UINT_WIDTH_IN_BITS)) * sizeof(uint32),
                                    "one_chunk_virt_data", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        set_bits(virtual_db_line_mask_arr, chunk_virtual_mem_width_offset, chunk_physical_mem_width_offset,
                 chunk_mapping_width, one_chunk_virt_mask);

        set_bits(virtual_db_line_input_data_arr, chunk_virtual_mem_width_offset, chunk_physical_mem_width_offset,
                 chunk_mapping_width, one_chunk_virt_data);
    }

    for (i = physical_data_arr_entry_offset; i < physical_data_arr_entry_offset + nof_entries_to_update; ++i)
    {
        physical_memory_entry_data[i] = (physical_memory_entry_data[i] & ~one_chunk_virt_mask[i]);
        one_chunk_virt_data[i] = (one_chunk_virt_data[i] & one_chunk_virt_mask[i]);
        physical_memory_entry_data[i] = (physical_memory_entry_data[i] | one_chunk_virt_data[i]);
    }

exit:
    SHR_FREE(one_chunk_virt_mask);
    SHR_FREE(one_chunk_virt_data);
    SHR_FUNC_EXIT;
}

int
dnx_pemladrv_calc_nof_arr_entries_from_width(
    const unsigned int width_in_bits)
{
    const int nof_bits_in_int = sizeof(int) * 8;
    unsigned int nof_entries =
        (width_in_bits % nof_bits_in_int == 0) ? width_in_bits / nof_bits_in_int : width_in_bits / nof_bits_in_int + 1;
    return nof_entries;
}

shr_error_e
dnx_find_or_allocate_and_read_physical_data(
    int unit,
    int sub_unit_idx,
    const unsigned int chunk_mem_block_id,
    const unsigned int chunk_phy_mem_addr,
    const unsigned int chunk_phy_mem_row_index,
    const unsigned int chunk_phy_mem_width,
    const unsigned int chunk_mapping_width,
    const unsigned int is_industrial_tcam,
    const unsigned int is_ingress,
    const int is_mem,
    PhysicalWriteInfo ** target_physical_memory_entry_data_list,
    PhysicalWriteInfo ** phy_wire)
{
    PhysicalWriteInfo *target_physical_memory_entry_data_list_curr_element = *target_physical_memory_entry_data_list;

    PhysicalWriteInfo *target_physical_memory_entry_data_new_element = NULL;
    int entry_data_arr_nof_entries;

    SHR_FUNC_INIT_VARS(unit);

    while (target_physical_memory_entry_data_list_curr_element)
    {
        if (((chunk_phy_mem_addr + chunk_phy_mem_row_index) ==
             target_physical_memory_entry_data_list_curr_element->mem.mem_address)
            && (chunk_mem_block_id == target_physical_memory_entry_data_list_curr_element->mem.block_identifier))
        {
            *phy_wire = target_physical_memory_entry_data_list_curr_element;
            SHR_EXIT();
        }
        if (target_physical_memory_entry_data_list_curr_element->next)
        {
            target_physical_memory_entry_data_list_curr_element =
                target_physical_memory_entry_data_list_curr_element->next;
            continue;
        }
        else
        {
            break;
        }
    }

    SHR_ALLOC_SET_ZERO_ERR_EXIT(target_physical_memory_entry_data_new_element, sizeof(PhysicalWriteInfo),
                                "target_physical_memory_entry_data_new_element", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    entry_data_arr_nof_entries = dnx_pemladrv_calc_nof_arr_entries_from_width(chunk_phy_mem_width);
    entry_data_arr_nof_entries += 1;
    SHR_ALLOC_SET_ZERO_ERR_EXIT(target_physical_memory_entry_data_new_element->entry_data,
                                entry_data_arr_nof_entries * sizeof(unsigned int),
                                "target_physical_memory_entry_data_new_element->entry_data", "%s%s%s\r\n", EMPTY, EMPTY,
                                EMPTY);
    target_physical_memory_entry_data_new_element->next = NULL;
    dnx_init_phy_mem_t_from_chunk_mapper(unit, chunk_mem_block_id,
                                         chunk_phy_mem_addr,
                                         chunk_phy_mem_row_index,
                                         chunk_phy_mem_width,
                                         is_industrial_tcam,
                                         is_ingress, is_mem, &target_physical_memory_entry_data_new_element->mem);

    SHR_IF_ERR_EXIT(pem_read
                    (unit, sub_unit_idx, &target_physical_memory_entry_data_new_element->mem, is_mem,
                     target_physical_memory_entry_data_new_element->entry_data));

    if (target_physical_memory_entry_data_list_curr_element)
        target_physical_memory_entry_data_list_curr_element->next = target_physical_memory_entry_data_new_element;
    else
        *target_physical_memory_entry_data_list = target_physical_memory_entry_data_new_element;
    *phy_wire = target_physical_memory_entry_data_new_element;

exit:
    if (SHR_FUNC_ERR())
        dnx_free_list(target_physical_memory_entry_data_new_element);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_write_all_physical_data_from_list(
    int unit,
    int sub_unit_idx,
    PhysicalWriteInfo * target_physical_memory_entry_data_curr)
{
    SHR_FUNC_INIT_VARS(unit);
    while (target_physical_memory_entry_data_curr)
    {
        SHR_IF_ERR_EXIT(pem_write
                        (unit, sub_unit_idx, &target_physical_memory_entry_data_curr->mem,
                         !(target_physical_memory_entry_data_curr->mem.is_reg),
                         target_physical_memory_entry_data_curr->entry_data, 0));
        target_physical_memory_entry_data_curr = target_physical_memory_entry_data_curr->next;
    }

exit:
    SHR_FUNC_EXIT;
}

void
dnx_free_list(
    PhysicalWriteInfo * target_physical_memory_entry_data_curr)
{
    PhysicalWriteInfo *next_elem = target_physical_memory_entry_data_curr;
    while (target_physical_memory_entry_data_curr)
    {
        SHR_FREE(target_physical_memory_entry_data_curr->entry_data);
        next_elem = target_physical_memory_entry_data_curr->next;
        SHR_FREE(target_physical_memory_entry_data_curr);

        target_physical_memory_entry_data_curr = next_elem;
    }
}

unsigned int
dnx_calculate_prefix_from_mask_array(
    const int total_key_width,
    const unsigned int *mask_arr)
{
    int j;
    unsigned int i;
    unsigned int nof_int_chunks_in_mask_arr, temp;
    unsigned int prefix_length = total_key_width;
    nof_int_chunks_in_mask_arr = dnx_pemladrv_get_nof_unsigned_int_chunks(total_key_width);

    for (i = 0; i < nof_int_chunks_in_mask_arr; ++i)
    {
        if (mask_arr[i] == -1)
        {
            prefix_length -= UINT_WIDTH_IN_BITS;
            continue;
        }
        break;
    }
    temp = mask_arr[i];
    for (j = 0; j < UINT_WIDTH_IN_BITS; ++j)
    {
        if (temp == 0)
            break;
        temp = temp >> 1;
        prefix_length -= 1;
    }
    return prefix_length;
}

void
dnx_set_pem_mem_accesss_fldbuf_from_physical_sram_data_array(
    const unsigned int *virtual_db_data_array,
    const FieldBitRange * field_bit_range_arr,
    pemladrv_mem_t * result)
{
    dnx_set_pem_mem_accesss_fldbuf(0, virtual_db_data_array, field_bit_range_arr, result);
}

void
dnx_set_pem_mem_accesss_fldbuf(
    const unsigned int offset,
    const unsigned int *virtual_db_data_array,
    const FieldBitRange * field_bit_range_arr,
    pemladrv_mem_t * mem_access)
{

    unsigned int field_index, field_width, field_id;

    for (field_index = 0; field_index < mem_access->nof_fields; ++field_index)
    {
        field_id = mem_access->fields[field_index]->field_id;
        field_width = field_bit_range_arr[field_id].msb - field_bit_range_arr[field_id].lsb + 1;

        set_bits(virtual_db_data_array, field_bit_range_arr[field_id].lsb + offset, 0, field_width,
                 mem_access->fields[field_index]->fldbuf);
    }
}

int
dnx_pemladrv_is_db_mapped_to_pes(
    const LogicalDbChunkMapperMatrix * const db_chunk_mapper)
{
    return (db_chunk_mapper != NULL);
}

unsigned int
dnx_pemladrv_get_nof_unsigned_int_chunks(
    const int field_width)
{
    if ((field_width % UINT_WIDTH_IN_BITS) == 0)
        return field_width / UINT_WIDTH_IN_BITS;
    else
        return (field_width / UINT_WIDTH_IN_BITS + 1);
}

shr_error_e
dnx_pemladrv_pem_allocate_fldbuf_by_bit_range_array(
    int unit,
    const int nof_fields,
    const FieldBitRange * bit_range_array,
    pemladrv_mem_t * pem_mem_access)
{
    int index, msb_bit, lsb_bit, field_width_in_bits;
    int mapped_field_index = 0;

    SHR_FUNC_INIT_VARS(unit);

    for (index = 0; index < nof_fields; ++index)
    {
        if (bit_range_array[index].is_field_mapped)
        {
            msb_bit = bit_range_array[index].msb;
            lsb_bit = bit_range_array[index].lsb;
            field_width_in_bits = msb_bit - lsb_bit + 1;
            SHR_IF_ERR_EXIT(pemladrv_mem_alloc_field
                            (unit, pem_mem_access->fields[mapped_field_index], field_width_in_bits));
            pem_mem_access->fields[mapped_field_index]->field_id = index;
            mapped_field_index++;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
pemladrv_mem_alloc(
    int unit,
    const unsigned int nof_fields,
    pemladrv_mem_t ** mem)
{
    unsigned int field_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(*mem, sizeof(pemladrv_mem_t), "mem", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    (*mem)->nof_fields = (uint16) nof_fields;
    SHR_ALLOC_SET_ZERO_ERR_EXIT((*mem)->fields, nof_fields * sizeof(pemladrv_field_t *), "(*mem)->fields", "%s%s%s\r\n",
                                EMPTY, EMPTY, EMPTY);
    for (field_index = 0; field_index < nof_fields; field_index++)
    {
        SHR_ALLOC_SET_ZERO_ERR_EXIT((*mem)->fields[field_index]->fldbuf, sizeof(pemladrv_field_t), "(*mem)->fields[]",
                                    "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
pemladrv_mem_alloc_field(
    int unit,
    pemladrv_field_t * pem_field_access,
    const unsigned int field_width_in_bits)
{

    unsigned int nof_entries;

    SHR_FUNC_INIT_VARS(unit);

    nof_entries =
        (field_width_in_bits % UINT_WIDTH_IN_BITS ==
         0) ? field_width_in_bits / UINT_WIDTH_IN_BITS : field_width_in_bits / UINT_WIDTH_IN_BITS + 1;
    SHR_ALLOC_SET_ZERO_ERR_EXIT(pem_field_access->fldbuf, nof_entries * sizeof(unsigned int),
                                "pem_field_access->fldbuf", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

void
pemladrv_mem_free(
    pemladrv_mem_t * pem_mem_access)
{
    if (pem_mem_access)
    {
        unsigned int field_ndx;
        unsigned int nof_fields = pem_mem_access->nof_fields;

        for (field_ndx = 0; field_ndx < nof_fields; ++field_ndx)
        {
            SHR_FREE(pem_mem_access->fields[field_ndx]->fldbuf);
        }
        SHR_FREE(pem_mem_access->fields);
        SHR_FREE(pem_mem_access);
    }
}

void
dnx_init_phy_mem_t_from_chunk_mapper(
    const int unit,
    const unsigned int chunk_mem_block_id,
    const unsigned int chunk_phy_mem_addr,
    const unsigned int chunk_phy_mem_row_index,
    const unsigned int chunk_phy_mem_width,
    const unsigned int is_industrial_tcam,
    const unsigned int is_ingress,
    const unsigned int is_mem,
    phy_mem_t * phy_mem)
{
    phy_mem->block_identifier = chunk_mem_block_id;
    phy_mem->mem_address = (is_mem == 0) ? chunk_phy_mem_addr : chunk_phy_mem_addr + chunk_phy_mem_row_index;
    phy_mem->mem_width_in_bits = chunk_phy_mem_width;
    phy_mem->is_industrial_tcam = is_industrial_tcam;
    phy_mem->reserve = is_ingress;
    phy_mem->is_reg = (unsigned int) (is_mem == 0);

}

unsigned int
dnx_pemladrv_get_nof_mapped_fields(
    unsigned int nof_fields,
    const FieldBitRange * bit_range_arr)
{
    unsigned int field_ndx = 0, nof_mapped_fields = 0;
    for (field_ndx = 0; field_ndx < nof_fields; ++field_ndx)
    {
        if (bit_range_arr[field_ndx].is_field_mapped)
            ++nof_mapped_fields;
    }

    return nof_mapped_fields;
}

shr_error_e
dnx_pemladrv_allocate_pemladrv_mem_struct(
    int unit,
    pemladrv_mem_t ** mem_access,
    unsigned int nof_fields,
    const FieldBitRange * bit_range_arr)
{
    unsigned int field_ndx, mapped_field_ndx, nof_bits_in_field, nof_entries_in_fldbuf;
    unsigned int nof_bits_in_uint32 = 8 * sizeof(uint32);
    unsigned int fld_buf_entry_ndx;
    unsigned int nof_mapped_fields = dnx_pemladrv_get_nof_mapped_fields(nof_fields, bit_range_arr);

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(*mem_access, sizeof(pemladrv_mem_t), "mem_access", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    (*mem_access)->nof_fields = (uint16) nof_mapped_fields;
    (*mem_access)->flags = 0;
    SHR_ALLOC_SET_ZERO_ERR_EXIT((*mem_access)->fields, nof_mapped_fields * sizeof(pemladrv_field_t *),
                                "(*mem_access)->fields", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    for (field_ndx = 0, mapped_field_ndx = 0; field_ndx < nof_fields; ++field_ndx)
    {
        if (!bit_range_arr[field_ndx].is_field_mapped)
        {
            continue;
        }
        SHR_ALLOC_SET_ZERO_ERR_EXIT((*mem_access)->fields[mapped_field_ndx], sizeof(pemladrv_field_t),
                                    "(*mem_access)->fields[]", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        nof_bits_in_field = bit_range_arr[field_ndx].msb - bit_range_arr[field_ndx].lsb + 1;
        nof_entries_in_fldbuf = (nof_bits_in_field % nof_bits_in_uint32 == 0
                                 && nof_bits_in_field >
                                 0) ? nof_bits_in_field / nof_bits_in_uint32 : nof_bits_in_field / nof_bits_in_uint32 +
            1;

        (*mem_access)->fields[mapped_field_ndx]->field_id = field_ndx;
        (*mem_access)->fields[mapped_field_ndx]->flags = 0;
        SHR_ALLOC_SET_ZERO_ERR_EXIT((*mem_access)->fields[mapped_field_ndx]->fldbuf,
                                    sizeof(uint32) * nof_entries_in_fldbuf, "(*mem_access)->fields[]->fldbuf",
                                    "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        for (fld_buf_entry_ndx = 0; fld_buf_entry_ndx < nof_entries_in_fldbuf; ++fld_buf_entry_ndx)
            (*mem_access)->fields[mapped_field_ndx]->fldbuf[fld_buf_entry_ndx] = 0;

        ++mapped_field_ndx;
    }

exit:
    SHR_FUNC_EXIT;
}

void
dnx_pemladrv_free_pemladrv_mem_struct(
    pemladrv_mem_t ** mem_access)
{
    if (*mem_access)
    {
        unsigned int field_ndx;
        unsigned int nof_fields = (*mem_access)->nof_fields;
        for (field_ndx = 0; field_ndx < nof_fields; ++field_ndx)
        {
            SHR_FREE((*mem_access)->fields[field_ndx]->fldbuf);
            SHR_FREE((*mem_access)->fields[field_ndx]);
        }
        SHR_FREE((*mem_access)->fields);
        SHR_FREE(*mem_access);
    }
}

void
dnx_pemladrv_set_pem_mem_access_with_value(
    pemladrv_mem_t * mem_access,
    unsigned int nof_fields,
    FieldBitRange * bit_range_arr,
    unsigned int field_val)
{
    unsigned int field_ndx, mapped_field_ndx, nof_bits_in_field, nof_bits_in_last_entry, nof_entries_in_fldbuf,
        entry_ndx_in_field;
    unsigned int nof_bits_in_uint32 = 8 * sizeof(uint32);
    unsigned int bit_mask;

    for (field_ndx = 0, mapped_field_ndx = 0; field_ndx < nof_fields; ++field_ndx)
    {
        if (!bit_range_arr[field_ndx].is_field_mapped)
            continue;

        nof_bits_in_field = bit_range_arr[field_ndx].msb - bit_range_arr[field_ndx].lsb + 1;
        nof_bits_in_last_entry = (nof_bits_in_field % nof_bits_in_uint32 == 0
                                  && nof_bits_in_field >
                                  0) ? nof_bits_in_uint32 : nof_bits_in_field % nof_bits_in_uint32;
        nof_entries_in_fldbuf = (nof_bits_in_field % nof_bits_in_uint32 == 0
                                 && nof_bits_in_field >
                                 0) ? nof_bits_in_field / nof_bits_in_uint32 : nof_bits_in_field / nof_bits_in_uint32 +
            1;

        for (entry_ndx_in_field = 0; entry_ndx_in_field < nof_entries_in_fldbuf; ++entry_ndx_in_field)
        {
            mem_access->fields[mapped_field_ndx]->fldbuf[entry_ndx_in_field] = field_val;
        }

        bit_mask = 0xffffffff;
        bit_mask >>= (nof_bits_in_uint32 - nof_bits_in_last_entry);
        mem_access->fields[mapped_field_ndx]->fldbuf[entry_ndx_in_field - 1] &= bit_mask;

        ++mapped_field_ndx;
    }
}

int
dnx_pemladrv_compare_pem_mem_access(
    pemladrv_mem_t * mem_access_a,
    pemladrv_mem_t * mem_access_b,
    unsigned int nof_fields,
    FieldBitRange * bit_range_arr)
{
    unsigned int field_ndx, entry_ndx_in_field, nof_bits_in_field, nof_entries_in_fldbuf;
    unsigned int nof_bits_in_uint32 = 8 * sizeof(uint32), error = 0;
    unsigned int mapped_field_ndx = 0;

    for (field_ndx = 0, mapped_field_ndx = 0; field_ndx < nof_fields; ++field_ndx)
    {
        if (!bit_range_arr[field_ndx].is_field_mapped)
            continue;

        nof_bits_in_field = bit_range_arr[field_ndx].msb - bit_range_arr[field_ndx].lsb + 1;
        nof_entries_in_fldbuf = (nof_bits_in_field % nof_bits_in_uint32 == 0
                                 && nof_bits_in_field >
                                 0) ? nof_bits_in_field / nof_bits_in_uint32 : nof_bits_in_field / nof_bits_in_uint32 +
            1;

        for (entry_ndx_in_field = 0; entry_ndx_in_field < nof_entries_in_fldbuf; ++entry_ndx_in_field)
        {
            error |=
                (mem_access_a->fields[mapped_field_ndx]->fldbuf[entry_ndx_in_field] !=
                 mem_access_b->fields[mapped_field_ndx]->fldbuf[entry_ndx_in_field]);
        }

        ++mapped_field_ndx;
    }

    return error;
}

int
get_core_from_block_id(
    unsigned int block_id)
{
#if defined(PROJECT_J2_A0) || defined(SOC_IS_JERICHO_2_A0) || defined(PROJECT_J2) || defined(SOC_IS_JERICHO_2)

    return block_id / 100;
#elif defined(PROJECT_J3) || defined(PROJECT_J2P)|| defined(SOC_IS_J2P)
    return block_id / 256;
#else
    return 0;
#endif
    return -1;
}

int
get_core_from_memory_address(
    unsigned long long address)
{
    int core = get_core_from_block_id((unsigned int) (address >> ADDRESS_SIZE_IN_BITS));
    return core;
}

void
get_address_and_block_id(
    unsigned long long full_address,
    uint32 *address,
    uint32 *block_id)
{
    *address = (uint32) full_address;
    full_address >>= ADDRESS_SIZE_IN_BITS;
    *block_id = (uint32) full_address;
}

int
compare_address_to_cache_entry(
    phy_mem_t * mem,
    const uint32 *cache_data)
{

    unsigned int address = cache_data[0];
    unsigned int block_id_mask = (1 << BLOCK_ID_SIZE_IN_BITS) - 1;
    unsigned int block_id = cache_data[1] & block_id_mask;

    return (address == mem->mem_address) && (block_id == mem->block_identifier);
}

int
find_entry_in_cache(
    const int unit,
    phy_mem_t * mem,
    int start_ndx,
    int core,
    int is_ingress0_egress1)
{
    int entry_ndx, count;
    unsigned int **cache = (unsigned int **) api_info[unit].applet_info.applet_mem_cache[core][is_ingress0_egress1];
    const unsigned int *cache_line;
    int nof_entries_in_mem = api_info[unit].applet_info.nof_pending_applets[core][is_ingress0_egress1];
    for (count = 0; count < nof_entries_in_mem; ++count)
    {
        entry_ndx = (start_ndx + count) % nof_entries_in_mem;
        cache_line = cache[entry_ndx];
        if (compare_address_to_cache_entry(mem, cache_line) != 0)
            return entry_ndx;
    }

    return -1;
}

shr_error_e
dnx_run_over_all_chunks_read_physical_data_and_update_physical_memory_entry_data_list(
    int unit,
    int sub_unit_idx,
    const unsigned char flag,
    const unsigned int virtual_row_index,
    const unsigned int total_width,
    const unsigned int chunk_matrix_row,
    const unsigned int nof_implamentations,
    const unsigned int nof_chunk_matrix_cols,
    const LogicalDbChunkMapperMatrix * db_chunk_mapper_matrix,
    const unsigned int *virtual_mask,
    const unsigned int *virtual_data,
    PhysicalWriteInfo ** phy_wire)
{

    int do_write = 0;
    unsigned int chunk_index, implamentation_index;
    unsigned char last_chunk = 0;

    PhysicalWriteInfo *target_physical_memory_entry_data_list = NULL;
    PhysicalWriteInfo *target_physical_memory_entry_data_list_curr_element = NULL;

    SHR_FUNC_INIT_VARS(unit);

    for (chunk_index = 0; chunk_index < nof_chunk_matrix_cols; ++chunk_index)
    {
        if (chunk_index == nof_chunk_matrix_cols - 1)
        {
            last_chunk = 1;
        }
        for (implamentation_index = 0; implamentation_index < nof_implamentations; ++implamentation_index)
        {

            do_write =
                dnx_do_chunk_require_writing(db_chunk_mapper_matrix[implamentation_index].db_chunk_mapper
                                             [chunk_matrix_row][chunk_index], virtual_mask);
            if (do_write)
            {
                const DbChunkMapper *chunk_mapper =
                    db_chunk_mapper_matrix[implamentation_index].db_chunk_mapper[chunk_matrix_row][chunk_index];
                const unsigned int physical_row_index =
                    dnx_calculate_physical_row_index_from_chunk_mapper(unit, chunk_mapper, virtual_row_index);
                SHR_IF_ERR_EXIT(dnx_find_or_allocate_and_read_physical_data
                                (unit, sub_unit_idx, chunk_mapper->mem_block_id, chunk_mapper->phy_mem_addr,
                                 physical_row_index, chunk_mapper->phy_mem_width, chunk_mapper->chunk_width, 0,
                                 chunk_mapper->is_ingress, 1, &target_physical_memory_entry_data_list,
                                 &target_physical_memory_entry_data_list_curr_element));

                SHR_IF_ERR_EXIT(dnx_modify_entry_data(unit, flag,
                                                      last_chunk,
                                                      chunk_mapper->chunk_width,
                                                      chunk_mapper->virtual_mem_width_offset,
                                                      chunk_mapper->phy_mem_width_offset,
                                                      total_width,
                                                      virtual_mask,
                                                      virtual_data,
                                                      target_physical_memory_entry_data_list_curr_element->entry_data));
            }
        }
    }
exit:
    *phy_wire = target_physical_memory_entry_data_list;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_build_physical_db_result_data_array(
    int unit,
    int sub_unit_idx,
    const LogicalDbChunkMapperMatrix * db_chunk_mapper_matrix,
    const unsigned int nof_chunk_cols,
    const unsigned int chunk_matrix_row_index,
    const unsigned int virtual_row_index,
    const FieldBitRange * field_bit_range_arr,
    pemladrv_mem_t * result,
    unsigned int *virtual_db_data_arr)
{
    unsigned int chunk_matrix_col_index, chunk_width, chunk_lsb_bit, chunk_msb_bit, field_idx, do_read;
    unsigned int virtual_data_array_offset = 0;
    DbChunkMapper *chunk_mapper;
    unsigned int physical_row_index;
    unsigned int *read_data = NULL;
    phy_mem_t *phy_mem = NULL;
    const unsigned int is_ingress = 0;
    const unsigned int is_mem = 1;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(read_data, 2 * sizeof(unsigned int), "read_data", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO_ERR_EXIT(phy_mem, sizeof(phy_mem_t), "phy_mem", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    for (chunk_matrix_col_index = 0; chunk_matrix_col_index < nof_chunk_cols; ++chunk_matrix_col_index)
    {
        chunk_width =
            db_chunk_mapper_matrix[0].db_chunk_mapper[chunk_matrix_row_index][chunk_matrix_col_index]->chunk_width;

        chunk_lsb_bit =
            db_chunk_mapper_matrix[0].
            db_chunk_mapper[chunk_matrix_row_index][chunk_matrix_col_index]->virtual_mem_width_offset;
        chunk_msb_bit = chunk_lsb_bit + chunk_width - 1;
        do_read = 0;
        for (field_idx = 0; field_idx < result->nof_fields; ++field_idx)
        {
            if (field_bit_range_arr[result->fields[field_idx]->field_id].lsb > chunk_msb_bit
                && field_bit_range_arr[result->fields[field_idx]->field_id].msb < chunk_lsb_bit)

                continue;
            else
            {
                do_read = 1;
                break;
            }
        }
        if (do_read)
        {

            chunk_mapper = db_chunk_mapper_matrix[0].db_chunk_mapper[chunk_matrix_row_index][chunk_matrix_col_index];
            physical_row_index =
                dnx_calculate_physical_row_index_from_chunk_mapper(unit, chunk_mapper, virtual_row_index);
            dnx_init_phy_mem_t_from_chunk_mapper(unit, chunk_mapper->mem_block_id, chunk_mapper->phy_mem_addr,
                                                 physical_row_index, chunk_mapper->phy_mem_width, 0, is_ingress, is_mem,
                                                 phy_mem);

            SHR_IF_ERR_EXIT(pem_read(unit, sub_unit_idx, phy_mem, 1, read_data));

            set_bits(read_data,
                     db_chunk_mapper_matrix[0].
                     db_chunk_mapper[chunk_matrix_row_index][chunk_matrix_col_index]->phy_mem_width_offset,
                     virtual_data_array_offset, chunk_width, virtual_db_data_arr);
        }
        virtual_data_array_offset += chunk_width;
    }

exit:
    SHR_FREE(read_data);
    SHR_FREE(phy_mem);
    SHR_FUNC_EXIT;
}

shr_error_e
pem_read(
    int unit,
    int sub_unit_idx,
    phy_mem_t * mem,
    int is_mem,
    void *entry_data)
{
    int core;
    int is_ingress0_egress1;
    int currently_writing_applets = api_info[unit].applet_info.currently_writing_applets_bit;
    int entry_in_cache = -1;

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    if (!currently_writing_applets)
    {
        if (mem->is_industrial_tcam)
        {

            SHR_IF_ERR_EXIT(pem_cs_tcam_read_entry(unit, sub_unit_idx, mem, entry_data));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_pem_direct_access_wrapper
                            (unit, sub_unit_idx, TRUE, is_mem, (int) mem->block_identifier, mem->mem_address,
                             mem->mem_width_in_bits, entry_data));
        }
        SHR_EXIT();
    }
#else
    if (currently_writing_applets == 0)
    {

        int res =
            pemladrv_physical_read(unit, mem->block_identifier, mem->is_reg, mem->mem_address, mem->mem_width_in_bits,
                                   mem->reserve, entry_data);

    }
#endif

    is_ingress0_egress1 = 0;
    core = get_core_from_block_id(mem->block_identifier);
    entry_in_cache = find_entry_in_cache(unit, mem, 0, core, is_ingress0_egress1);
    if (-1 == entry_in_cache)
    {
        is_ingress0_egress1 = 1;
        entry_in_cache = find_entry_in_cache(unit, mem, 0, core, is_ingress0_egress1);
    }

    if (-1 == entry_in_cache)
    {
#ifdef BCM_DNX_SUPPORT
        SHR_IF_ERR_EXIT(dnx_pem_direct_access_wrapper
                        (unit, sub_unit_idx, TRUE, is_mem, (int) mem->block_identifier, mem->mem_address,
                         mem->mem_width_in_bits, entry_data));
        SHR_EXIT();
#else
        SHR_IF_ERR_EXIT(pemladrv_physical_read
                        (unit, mem->block_identifier, mem->is_reg, mem->mem_address, mem->mem_width_in_bits,
                         mem->reserve, entry_data));
#endif
    }

    core = get_core_from_block_id(mem->block_identifier);
    api_info[unit].applet_info.entry_found_for_read_lately[core][is_ingress0_egress1] = entry_in_cache;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
pem_write(
    int unit,
    int sub_unit_idx,
    phy_mem_t * mem,
    int is_mem,
    void *entry_data,
    uint32 flags)
{
    int core, is_ingress0_egress1;
    int currently_writing_applets = api_info[unit].applet_info.currently_writing_applets_bit;
    int entry_in_cache = -1;
    int entry_found_in_cache_lately, nof_entries_in_cache;
    unsigned long long address;
    int applet_mem_size_in_bits = api_info[unit].applet_info.applet_mem_info[0][0].length_in_bits;

    SHR_FUNC_INIT_VARS(unit);

    if (!currently_writing_applets)
    {
#if defined(BCM_DNX_SUPPORT)
        if (flags & PEMLA_CACHE_WRITE_ONLY)
        {
            if (is_mem)
            {

                soc_direct_mem_set_cache_update(unit, (int) mem->block_identifier, mem->mem_address, entry_data);
            }
        }
        else
#endif
        {
#ifdef BCM_DNX_SUPPORT
            SHR_IF_ERR_EXIT(dnx_pem_direct_access_wrapper
                            (unit, sub_unit_idx, FALSE, is_mem, (int) mem->block_identifier, mem->mem_address,
                             mem->mem_width_in_bits, entry_data));
#else

            SHR_IF_ERR_EXIT(pemladrv_physical_write
                            (unit, mem->block_identifier, mem->is_reg, mem->mem_address, mem->mem_width_in_bits,
                             mem->reserve, entry_data));
#endif
        }
        SHR_EXIT();
    }

    core = get_core_from_block_id(mem->block_identifier);
    is_ingress0_egress1 = mem->reserve;
    nof_entries_in_cache = api_info[unit].applet_info.nof_pending_applets[core][is_ingress0_egress1];
    entry_found_in_cache_lately = api_info[unit].applet_info.entry_found_for_read_lately[core][is_ingress0_egress1];

    entry_found_in_cache_lately =
        (entry_found_in_cache_lately > nof_entries_in_cache - 1) ? 0 : entry_found_in_cache_lately;
    entry_in_cache = find_entry_in_cache(unit, mem, entry_found_in_cache_lately, 0, is_ingress0_egress1);

    if (-1 == entry_in_cache)
    {
        assert(nof_entries_in_cache < APPLET_MEM_NOF_ENTRIES);
#ifdef BCM_DNX_SUPPORT
        SHR_ALLOC_SET_ZERO_ERR_EXIT(api_info[unit].
                                    applet_info.applet_mem_cache[core][is_ingress0_egress1][nof_entries_in_cache],
                                    applet_mem_size_in_bits / (sizeof(uint32) + 1), "applet_mem_cache", "%s%s%s\r\n",
                                    EMPTY, EMPTY, EMPTY);
#else
        api_info[unit].applet_info.applet_mem_cache[core][is_ingress0_egress1][nof_entries_in_cache] =
            (uint32 *) calloc(applet_mem_size_in_bits / (sizeof(uint32) + 1), sizeof(uint32) + 1);
#endif
        entry_in_cache = api_info[unit].applet_info.nof_pending_applets[core][is_ingress0_egress1]++;
    }

    address = mem->block_identifier;
    address <<= ADDRESS_SIZE_IN_BITS;
    address |= (unsigned long long) (mem->mem_address);

exit:
    SHR_FUNC_EXIT;
}

#endif
