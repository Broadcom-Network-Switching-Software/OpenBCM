
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved. 
 */
/* *INDENT-OFF* */
#include <stdlib.h>
#include <time.h>
#include "../pemladrv_logical_access.h"
#include "pemladrv_reg_logical_access.h"
#include "pemladrv_tcam_logical_access.h"
#include "../pemladrv_meminfo_access.h"
#include "../pemladrv_cfg_api_defines.h"

#ifdef BCM_DNX_SUPPORT
#define BSL_LOG_MODULE BSL_LS_SOCDNX_ADAPTERDNX
#endif

extern ApiInfo api_info[MAX_NOF_UNITS];


void mask_reg_read(
        pemladrv_mem_t * result,
        const pemladrv_mem_t * result_read_bits_mask,
        unsigned int nof_fields,
        FieldBitRange * bit_range_arr)
{
    unsigned int field_ndx, mapped_field_ndx, nof_bits_in_field, nof_entries_in_fldbuf, entry_ndx_in_field;
    unsigned int nof_bits_in_uint32 = 8 * sizeof(uint32);

    for (field_ndx = 0, mapped_field_ndx = 0; field_ndx < nof_fields; ++field_ndx){
        if (!bit_range_arr[field_ndx].is_field_mapped)
            continue;
        
        nof_bits_in_field = bit_range_arr[field_ndx].msb - bit_range_arr[field_ndx].lsb + 1;
        nof_entries_in_fldbuf = (nof_bits_in_field % nof_bits_in_uint32 == 0 && nof_bits_in_field > 0) ? nof_bits_in_field / nof_bits_in_uint32 : nof_bits_in_field / nof_bits_in_uint32 + 1;

        for (entry_ndx_in_field = 0; entry_ndx_in_field < nof_entries_in_fldbuf; ++entry_ndx_in_field){
            result->fields[mapped_field_ndx]->fldbuf[entry_ndx_in_field] &= result_read_bits_mask->fields[mapped_field_ndx]->fldbuf[entry_ndx_in_field];
        }
        ++mapped_field_ndx;
    }
}


shr_error_e dnx_pemladrv_get_reg_or_reg_field_name(int unit, char* name, int reg1_reg_field0, char** target_name){
#ifdef BCM_DNX_SUPPORT
      char temp_name[256];
      char* name_to_set = NULL;
      char** temp_name_to_set = NULL;
#endif

SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    sal_strncpy(temp_name, name, sizeof(temp_name));
	temp_name[sal_strnlen(name, 256)] = '\0';

    if (reg1_reg_field0)
    {
        unsigned int num_of_tokens;
        temp_name_to_set = utilex_str_split(temp_name, ".", 2, &num_of_tokens);
        if (temp_name_to_set != NULL)
        {
            name_to_set = temp_name_to_set[0];
            if (NULL == name_to_set)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "invalid name_to_set");
            }
            *target_name = sal_strdup(name_to_set);
        }
    }
    else
    {
        unsigned int num_of_tokens;
        temp_name_to_set = utilex_str_split(temp_name, ".", 2, &num_of_tokens);
        if (temp_name_to_set == NULL)
        {
            SHR_EXIT();
        }
        name_to_set = temp_name_to_set[1];
        if (NULL == name_to_set)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid name_to_set");
        }
        *target_name = sal_strdup(name_to_set);
    }
#endif

exit:
#ifdef BCM_DNX_SUPPORT
    utilex_str_split_free(temp_name_to_set, 2);
#endif
    SHR_FUNC_EXIT;
}


shr_error_e dnx_pemladrv_get_reg_name(const int unit, const int reg_id, char** target_name){
    unsigned int i;
    const int core_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    for (i = 0; i < api_info[unit].reg_container.reg_info_arr[reg_id].nof_fields; ++i)
    {
        if (api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_info_arr[i].reg_field_mapping_per_core_arr[core_id].nof_mappings > 0)
        {
            break;
        }
    }
    SHR_IF_ERR_EXIT(dnx_pemladrv_get_reg_or_reg_field_name(unit, api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_info_arr[i].reg_field_mapping_per_core_arr[core_id].reg_field_mapping_arr[0].register_name, 1, target_name));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e dnx_pemladrv_get_reg_field_name(const int unit, const int reg_id, const int field_id, char** target_name){
    const int nof_fields = api_info[unit].reg_container.reg_info_arr[reg_id].nof_fields;
    const int core_id = 0;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    
    for (i = 0; i < nof_fields; ++i)
    {
        if ((api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_info_arr[i].reg_field_mapping_per_core_arr[core_id].nof_mappings > 0) &&
            (api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_info_arr[i].reg_field_mapping_per_core_arr[core_id].reg_field_mapping_arr[0].field_id ==
             field_id))
        {
            break;
        }
    }
    
    if (i == nof_fields)
    {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(dnx_pemladrv_get_reg_or_reg_field_name(unit, api_info[unit].reg_container.reg_info_arr[reg_id].
                                                   reg_field_info_arr[i].reg_field_mapping_per_core_arr[core_id].reg_field_mapping_arr[0].register_name, 0,
                                           target_name));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e dnx_pemladrv_full_reg_write(const int unit, const int core_id, const int reg_id, const int size, const unsigned int* data){
    const int nof_fields = api_info[unit].reg_container.reg_info_arr[reg_id].nof_fields;
    pemladrv_mem_t* reg_p = NULL;
    int mapped_nof_fields = 0;

    SHR_FUNC_INIT_VARS(unit);

    
    if (size > (int)api_info[unit].reg_container.reg_info_arr[reg_id].register_total_width)
        SHR_EXIT();

    for (int i = 0; i < nof_fields; ++i){
        if (api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_bit_range_arr[i].is_field_mapped)
            mapped_nof_fields += 1;
    }

    SHR_IF_ERR_EXIT(pemladrv_mem_alloc(unit, mapped_nof_fields, &reg_p));
    SHR_IF_ERR_EXIT(dnx_pemladrv_pem_allocate_fldbuf_by_bit_range_array(unit, nof_fields, api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_bit_range_arr, reg_p));
    dnx_set_pem_mem_accesss_fldbuf(0, data, api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_bit_range_arr, reg_p);

    SHR_IF_ERR_EXIT(pemladrv_reg_write(unit, core_id, reg_id, reg_p));

exit:
    pemladrv_mem_free(reg_p);
    SHR_FUNC_EXIT;
}


shr_error_e dnx_pemladrv_virtual_register_size_in_bits_get(const int unit, const int reg_id, const int field_id, int* size) {

    SHR_FUNC_INIT_VARS(unit);

    if (reg_id > (int)api_info[unit].reg_container.nof_registers || !api_info[unit].reg_container.reg_info_arr[reg_id].is_mapped)
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid reg_id %d", reg_id);

    *size = api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_bit_range_arr[field_id].msb - api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_bit_range_arr[field_id].lsb + 1;

exit:
    SHR_FUNC_EXIT;
}


unsigned int calculate_mapped_register_size(unsigned int unit, unsigned int reg_index, unsigned int  field_index){
    unsigned int mapping_index, nof_mappings, nof_chunks, lsb_bit, msb_bit;
    unsigned int mapped_bits_arr[MAX_DATA_VALUE_ARRAY] = { 0 };
    const int core_id = 0;

    nof_chunks = dnx_pemladrv_get_nof_unsigned_int_chunks(api_info[unit].reg_container.reg_info_arr[reg_index].register_total_width);

    nof_mappings = api_info[unit].reg_container.reg_info_arr[reg_index].reg_field_info_arr[field_index].reg_field_mapping_per_core_arr[core_id].nof_mappings;
    for (mapping_index = 0; mapping_index < nof_mappings; mapping_index++){
        lsb_bit = api_info[unit].reg_container.reg_info_arr[reg_index].reg_field_info_arr[field_index].reg_field_mapping_per_core_arr[core_id].reg_field_mapping_arr[mapping_index].virtual_field_lsb;
        msb_bit = api_info[unit].reg_container.reg_info_arr[reg_index].reg_field_info_arr[field_index].reg_field_mapping_per_core_arr[core_id].reg_field_mapping_arr[mapping_index].virtual_field_msb;
        dnx_set_mask_with_ones(msb_bit, lsb_bit, mapped_bits_arr);
    }
    return count_set_bits_in_unsinged_int_array(nof_chunks, mapped_bits_arr);
}


shr_error_e dnx_pemladrv_reg_info_allocate(const int unit, pemladrv_reg_info** arr, int* size) {
    int mapped_regs_size = 0;
    unsigned int reg_index, field_index;
    uint8 mapped_fields_index = 0;
    const int core_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(*arr, api_info[unit].reg_container.nof_registers* sizeof(pemladrv_reg_info), "arr", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    for (reg_index = 0; reg_index < api_info[unit].reg_container.nof_registers; ++reg_index)
    {
        LogicalRegInfo* reg_info_arr = &api_info[unit].reg_container.reg_info_arr[reg_index];
        if (!reg_info_arr->is_mapped)
        {
            continue;
        }
        (*arr)[mapped_regs_size].reg_id = reg_index;
        dnx_pemladrv_get_reg_name(unit, reg_index, &(*arr)[mapped_regs_size].reg_name);
        (*arr)[mapped_regs_size].is_binded = reg_info_arr->is_binded;
        (*arr)[mapped_regs_size].nof_fields = reg_info_arr->nof_fields;
        SHR_ALLOC_SET_ZERO_ERR_EXIT((*arr)[mapped_regs_size].ptr, (*arr)[mapped_regs_size].nof_fields* sizeof(pemladrv_reg_field_info), "(*arr)[].ptr", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        mapped_fields_index = 0;
        for (field_index = 0; field_index < api_info[unit].reg_container.reg_info_arr[reg_index].nof_fields; ++field_index)
        {
            LogicalRegFieldInfo *reg_field_info_arr = &reg_info_arr->reg_field_info_arr[field_index];

            if (reg_field_info_arr->reg_field_mapping_per_core_arr[core_id].nof_mappings > 0)
            {
                if ((*arr)[mapped_regs_size].is_binded)
                {
                    SHR_IF_ERR_EXIT(dnx_pemladrv_get_reg_field_name(unit, reg_index, field_index, &(*arr)[mapped_regs_size].ptr[mapped_fields_index].field_name));
                }
                else
                {
                    
                    SHR_IF_ERR_EXIT(dnx_pemladrv_get_reg_field_name(unit, reg_index, field_index, &(*arr)[mapped_regs_size].ptr[mapped_fields_index].field_name));
                }

                (*arr)[mapped_regs_size].ptr[mapped_fields_index].field_id = reg_field_info_arr->reg_field_mapping_per_core_arr[core_id].reg_field_mapping_arr->field_id;
                (*arr)[mapped_regs_size].ptr[mapped_fields_index].field_size_in_bits = calculate_mapped_register_size(unit, reg_index, field_index);
                (*arr)[mapped_regs_size].ptr[mapped_fields_index].nof_mappings = reg_field_info_arr->reg_field_mapping_per_core_arr[core_id].nof_mappings;
                SHR_ALLOC_SET_ZERO_ERR_EXIT((*arr)[mapped_regs_size].ptr[mapped_fields_index].addresses, reg_field_info_arr->reg_field_mapping_per_core_arr[core_id].nof_mappings* sizeof(pemladrv_reg_mapping_info), "(*arr)[].ptr[].addresses", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

                for (int mapping_index = 0; mapping_index < reg_field_info_arr->reg_field_mapping_per_core_arr[core_id].nof_mappings; ++mapping_index){
                    (*arr)[mapped_regs_size].ptr[mapped_fields_index].addresses[mapping_index].core_address = reg_field_info_arr->reg_field_mapping_per_core_arr[core_id].reg_field_mapping_arr->pem_mem_address;
                    (*arr)[mapped_regs_size].ptr[mapped_fields_index].addresses[mapping_index].block_id     = reg_field_info_arr->reg_field_mapping_per_core_arr[core_id].reg_field_mapping_arr->pem_mem_block_id;
                }
                mapped_fields_index++;
            }
        }
        (*arr)[mapped_regs_size].nof_fields = mapped_fields_index;
        mapped_regs_size++;
    }
    *size = mapped_regs_size;

exit:
    SHR_FUNC_EXIT;
}


void dnx_pemladrv_reg_info_de_allocate(const int unit, pemladrv_reg_info* ptr){
    pemladrv_reg_info temp;
    if(ptr)
    {
        for (unsigned int i = 0; i < api_info[unit].reg_container.nof_registers; ++i){
            temp = ptr[i];
            for(unsigned int field_idx = 0; field_idx < temp.nof_fields; field_idx++){
                SHR_FREE(temp.ptr[field_idx].field_name);
                SHR_FREE(temp.ptr[field_idx].addresses);
            }
            SHR_FREE(temp.ptr);
            SHR_FREE(temp.reg_name);
        }
        SHR_FREE(ptr);
    }
}



shr_error_e dnx_pemladrv_reg_field_run_over_all_chunks_read_physical_data_and_update_physical_memory_entry_data_list(
        int unit,
        int core_id,
        LogicalRegFieldInfo * reg_field_info_pnt,
        FieldBitRange * reg_field_bit_range,
        const unsigned int *virtual_mask,
        const unsigned int *virtual_data,
        PhysicalWriteInfo * target_physical_memory_entry_data_list,
        PhysicalWriteInfo** pw_info)
{
    PhysicalWriteInfo* target_physical_memory_entry_data_list_curr_element = NULL;
    unsigned int mapping_ndx;
    int sub_unit_idx = PEM_CFG_CORE_ID_TO_SUB_UNIT_ID(core_id);

    SHR_FUNC_INIT_VARS(unit);

    for (mapping_ndx = 0; mapping_ndx < reg_field_info_pnt->reg_field_mapping_per_core_arr[core_id].nof_mappings; ++mapping_ndx) {
        RegFieldMapper* reg_field_mapper = &reg_field_info_pnt->reg_field_mapping_per_core_arr[core_id].reg_field_mapping_arr[mapping_ndx];


        SHR_IF_ERR_EXIT(dnx_find_or_allocate_and_read_physical_data(unit,
                                                                      sub_unit_idx,
                                                                      reg_field_mapper->pem_mem_block_id,
                                                                      reg_field_mapper->pem_mem_address,
                                                                      reg_field_mapper->pem_mem_line,
                                                                      reg_field_mapper->pem_mem_total_width,
                                                                      reg_field_mapper->cfg_mapper_width,
                                                                      reg_field_mapper->is_industrial_tcam,
                                                                      reg_field_mapper->is_ingress,
                                                                      reg_field_mapper->is_mem,
                                                                      &target_physical_memory_entry_data_list,
                                                                      &target_physical_memory_entry_data_list_curr_element));

        SHR_IF_ERR_EXIT(dnx_modify_entry_data(unit, 0, 0, 
                              reg_field_mapper->cfg_mapper_width,
                              reg_field_bit_range->lsb + reg_field_mapper->virtual_field_lsb, 
                              reg_field_mapper->pem_mem_offset,
                              0,  
                              virtual_mask,
                              virtual_data,
                              target_physical_memory_entry_data_list_curr_element->entry_data));

    }

exit:
    *pw_info = target_physical_memory_entry_data_list;
    SHR_FUNC_EXIT;
}


shr_error_e dnx_pemladrv_reg_field_run_over_all_chunks_read_physical_data_to_virtual_data(
        int unit,
        int core_id,
        LogicalRegFieldInfo * reg_field_info_pnt,
        FieldBitRange * reg_field_bit_range,
        unsigned int *virtual_mask,
        unsigned int *virtual_data,
        PhysicalWriteInfo * target_physical_memory_entry_data_list,
        PhysicalWriteInfo** pw_info)
{

    PhysicalWriteInfo* target_physical_memory_entry_data_list_curr_element = NULL;
    unsigned int mapping_ndx;
    int sub_unit_idx = PEM_CFG_CORE_ID_TO_SUB_UNIT_ID(core_id);

    SHR_FUNC_INIT_VARS(unit);

    for (mapping_ndx = 0; mapping_ndx < reg_field_info_pnt->reg_field_mapping_per_core_arr[core_id].nof_mappings; ++mapping_ndx) {
        RegFieldMapper* reg_field_mapper = &reg_field_info_pnt->reg_field_mapping_per_core_arr[core_id].reg_field_mapping_arr[mapping_ndx];


        SHR_IF_ERR_EXIT(dnx_find_or_allocate_and_read_physical_data(unit,
                                                                      sub_unit_idx,
                                                                      reg_field_mapper->pem_mem_block_id,
                                                                      reg_field_mapper->pem_mem_address,
                                                                      reg_field_mapper->pem_mem_line,
                                                                      reg_field_mapper->pem_mem_total_width,
                                                                      reg_field_mapper->cfg_mapper_width,
                                                                      reg_field_mapper->is_industrial_tcam,
                                                                      reg_field_mapper->is_ingress,
                                                                      reg_field_mapper->is_mem,
                                                                      &target_physical_memory_entry_data_list,
                                                                      &target_physical_memory_entry_data_list_curr_element));

        dnx_reg_read_entry_data(reg_field_mapper->cfg_mapper_width,
                                reg_field_bit_range->lsb + reg_field_mapper->virtual_field_lsb,
                                reg_field_mapper->pem_mem_offset,
                                target_physical_memory_entry_data_list_curr_element->entry_data,
                                virtual_mask,
                                virtual_data);

    }

exit:
    *pw_info = target_physical_memory_entry_data_list;
    SHR_FUNC_EXIT;
}


void dnx_reg_read_entry_data(
        const unsigned int chunk_mapping_width,
        const unsigned int chunk_virtual_mem_width_offset,
        const unsigned int chunk_physical_mem_width_offset,
        const unsigned int *physical_memory_entry_data,
        unsigned int *virtual_db_line_mask_arr,
        unsigned int *virtual_db_line_input_data_arr)
{

    

    
    

    set_bits          (physical_memory_entry_data, chunk_physical_mem_width_offset, chunk_virtual_mem_width_offset, chunk_mapping_width, virtual_db_line_input_data_arr);
    dnx_set_mask_with_ones(chunk_virtual_mem_width_offset + chunk_mapping_width-1, chunk_virtual_mem_width_offset, virtual_db_line_mask_arr);
}




shr_error_e dnx_pemladrv_reg_read_and_retreive_read_bits_mask(
        int unit,
        int core_id,
        reg_id_t reg_id,
        pemladrv_mem_t * result,
        pemladrv_mem_t * result_read_bits_mask)
{
    unsigned int field_ndx;
    PhysicalWriteInfo* target_reg_physical_memory_entry_data_list    = NULL;

    const unsigned int register_total_width        = api_info[unit].reg_container.reg_info_arr[reg_id].register_total_width;
    const unsigned int nof_fields                  = api_info[unit].reg_container.reg_info_arr[reg_id].nof_fields;

    
    unsigned int *virtual_reg_mask    = NULL;
    unsigned int *virtual_reg_data    = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_build_virtual_db_mask_and_data_from_result_fields(unit, register_total_width, api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_bit_range_arr, result, 0, &virtual_reg_mask, &virtual_reg_data));


    for(field_ndx=0; field_ndx < nof_fields;++field_ndx) {
        LogicalRegFieldInfo* reg_field_info_ptr     = &(api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_info_arr[field_ndx]);
        FieldBitRange*       reg_field_bit_range    = &(api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_bit_range_arr[field_ndx]);

        SHR_IF_ERR_EXIT(dnx_pemladrv_reg_field_run_over_all_chunks_read_physical_data_to_virtual_data(unit,
                                                                                                core_id,
                                                                                                reg_field_info_ptr,
                                                                                                reg_field_bit_range,
                                                                                                virtual_reg_mask,
                                                                                                virtual_reg_data,
                                                                                                target_reg_physical_memory_entry_data_list,
                                                                                                &target_reg_physical_memory_entry_data_list));
    }

    dnx_set_pem_mem_accesss_fldbuf_from_physical_sram_data_array(virtual_reg_data, api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_bit_range_arr, result);
    if (result_read_bits_mask != 0) {
        dnx_set_pem_mem_accesss_fldbuf_from_physical_sram_data_array(virtual_reg_mask,
                                                                     api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_bit_range_arr,
                                                                     result_read_bits_mask);
    }

exit:
    dnx_free_list(target_reg_physical_memory_entry_data_list);
    SHR_FREE(virtual_reg_mask);
    SHR_FREE(virtual_reg_data);
    SHR_FUNC_EXIT;
}


shr_error_e pemladrv_reg_write_inner(
        int unit,
        int core_id,
        reg_id_t reg_id,
        pemladrv_mem_t * data)
{

    unsigned int field_ndx;
    int sub_unit_idx = PEM_CFG_CORE_ID_TO_SUB_UNIT_ID(core_id);

    PhysicalWriteInfo* target_reg_physical_memory_entry_data_list    = NULL;

    const unsigned int register_total_width        = api_info[unit].reg_container.reg_info_arr[reg_id].register_total_width;
    const unsigned int nof_fields                  = api_info[unit].reg_container.reg_info_arr[reg_id].nof_fields;

    
    unsigned int *virtual_reg_mask    = NULL;
    unsigned int *virtual_reg_data    = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_build_virtual_db_mask_and_data_from_result_fields(unit, register_total_width, api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_bit_range_arr, data, 1, &virtual_reg_mask, &virtual_reg_data));


    for(field_ndx=0; field_ndx < nof_fields;++field_ndx) {
        LogicalRegFieldInfo* reg_field_info_ptr     = &(api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_info_arr[field_ndx]);
        FieldBitRange*       reg_field_bit_range    = &(api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_bit_range_arr[field_ndx]);

        SHR_IF_ERR_EXIT(dnx_pemladrv_reg_field_run_over_all_chunks_read_physical_data_and_update_physical_memory_entry_data_list(unit,
                                                                                                                           core_id,
                                                                                                                           reg_field_info_ptr,
                                                                                                                           reg_field_bit_range,
                                                                                                                           virtual_reg_mask,
                                                                                                                           virtual_reg_data,
                                                                                                                           target_reg_physical_memory_entry_data_list,
                                                                                                                           &target_reg_physical_memory_entry_data_list));
    }

    SHR_IF_ERR_EXIT(dnx_write_all_physical_data_from_list(unit, sub_unit_idx, target_reg_physical_memory_entry_data_list));

exit:
    dnx_free_list(target_reg_physical_memory_entry_data_list);
    SHR_FREE(virtual_reg_mask);
    SHR_FREE(virtual_reg_data);
    SHR_FUNC_EXIT;
}


shr_error_e pemladrv_reg_write(
        const int unit,
        const int core_id, 
        reg_id_t reg_id,
        pemladrv_mem_t * data)
{
    SHR_FUNC_INIT_VARS(unit);
    if (core_id == -1) {
        for (int core_i = 0; core_i < PEM_CFG_API_NOF_CORES_DYNAMIC; ++core_i) {
            SHR_IF_ERR_EXIT(pemladrv_reg_write_inner(unit, core_i, reg_id, data));
        }
    }
    else{
        SHR_IF_ERR_EXIT(pemladrv_reg_write_inner(unit, core_id, reg_id, data));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e pemladrv_reg_read(
        const int unit,
        const int core_id,
        reg_id_t reg_id,
        pemladrv_mem_t * result)
{
    pemladrv_mem_t *result_read_bits_mask = NULL;
    pemladrv_mem_t *expected_result_read_bits_mask_when_all_mapped = NULL;
    int is_result_mask_equal_to_expected_when_all_mapped;
    int nof_fields                         = api_info[unit].reg_container.reg_info_arr[reg_id].nof_fields;
    FieldBitRange* reg_field_bit_range_arr = api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_bit_range_arr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pemladrv_allocate_pemladrv_mem_struct(unit, &result_read_bits_mask, nof_fields, api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_bit_range_arr));

    SHR_IF_ERR_EXIT(dnx_pemladrv_reg_read_and_retreive_read_bits_mask(unit, core_id, reg_id, result, result_read_bits_mask));

    
    SHR_IF_ERR_EXIT(dnx_pemladrv_allocate_pemladrv_mem_struct(unit, &expected_result_read_bits_mask_when_all_mapped, nof_fields, reg_field_bit_range_arr));

    dnx_pemladrv_set_pem_mem_access_with_value(expected_result_read_bits_mask_when_all_mapped, nof_fields, reg_field_bit_range_arr, 0xffffffff);

    is_result_mask_equal_to_expected_when_all_mapped = dnx_pemladrv_compare_pem_mem_access(result_read_bits_mask, expected_result_read_bits_mask_when_all_mapped, nof_fields, reg_field_bit_range_arr);
    if(is_result_mask_equal_to_expected_when_all_mapped != 0)
    {
        
    }

exit:
    dnx_pemladrv_free_pemladrv_mem_struct(&result_read_bits_mask);
    dnx_pemladrv_free_pemladrv_mem_struct(&expected_result_read_bits_mask_when_all_mapped);
    SHR_FUNC_EXIT;
}


