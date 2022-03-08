
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef DBAL_HL_ACCESS_H_INCLUDED
#define DBAL_HL_ACCESS_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALACCESSDNX
#include <soc/drv.h>
#include <shared/bsl.h>

#include "dbal_internal.h"
#include <soc/dnx/dbal/dbal.h>
#include <soc/mem.h>
#include <soc/sand/sand_mem.h>
#include <soc/mcm/memregs.h>
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_bitstream.h>
#include <bcm_int/dnx/init/init_mem.h>
#include <soc/cmic.h>
#if defined(BCM_ACCESS_SUPPORT)
#include <soc/access/access.h>
#endif

#define DBAL_HL_LOGGER_STATUS_UPDATE                                                                    \
 if(dbal_logger_is_enable(unit, entry_handle->table_id))                                                \
    {                                                                                                   \
        logger_enable = 1;                                                                              \
    }

#define DBAL_ACTION_PERFORMED_LOG                                                       \
        if(logger_enable)                                                               \
        {                                                                               \
            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Action performed!\n")));        \
        }                                                                               \


#define DBAL_FIELD_START_LOG(field_id, is_mem, hw_element)                                                  \
    if(logger_enable)                                                     \
    {                                                                                                       \
        LOG_INFO_EX(BSL_LOG_MODULE, "\nHandling field: %s mapped to %s %-30s %s\n", dbal_field_to_string(unit, field_id), \
            is_mem?"memory":"regstr", is_mem?SOC_MEM_NAME(unit, hw_element): SOC_REG_NAME(unit, hw_element), EMPTY);   \
    }

#define DBAL_MEM_READ_LOG( memory, mem_offset, block, mem_array_offset)                                         \
    if(logger_enable)                                                     \
    {                                                                                                       \
        LOG_INFO_EX(BSL_LOG_MODULE, "Reading from memory %-30s, entry offset %d, block %d, array offset %d ... ",  \
            SOC_MEM_NAME(unit, memory), mem_offset, block, mem_array_offset);                               \
    }

#define DBAL_MEM_WRITE_LOG( memory, mem_offset, block, mem_array_offset)                                        \
    if(logger_enable)                                                     \
    {                                                                                                       \
        LOG_INFO_EX(BSL_LOG_MODULE, "Writing into memory %-30s, entry offset %d, block %d, array offset %d ... ",  \
            SOC_MEM_NAME(unit, memory), mem_offset, block, mem_array_offset);                               \
    }

#define DBAL_MEM_WRITE_ARRAY_LOG( memory, mem_offset, block, num_elem)                                              \
    if(logger_enable)                                                         \
    {                                                                                                           \
        LOG_INFO_EX(BSL_LOG_MODULE, "Writing into memory %-30s, entry offset %d, block %d, array elmnts %d ... ",\
            SOC_MEM_NAME(unit, memory), mem_offset, block, num_elem);                                           \
    }

#define DBAL_REG_READ_LOG( reg, reg_offset, block, reg_array_offset)                                                \
    if(logger_enable)                                                         \
    {                                                                                                           \
            LOG_INFO_EX(BSL_LOG_MODULE, "Reading from regstr %-30s, entry offset %d, block %d, array offset %d ... ",\
            SOC_REG_NAME(unit, reg), reg_offset, block, reg_array_offset);                                      \
    }

#define DBAL_REG_WRITE_LOG( reg, reg_offset, block, reg_array_offset)                                               \
    if(logger_enable)                                                         \
    {                                                                                                           \
        LOG_INFO_EX(BSL_LOG_MODULE, "Writing into regstr %-30s, entry offset %d, block %d, array offset %d ... ",    \
             SOC_REG_NAME(unit, reg), reg_offset, block, reg_array_offset);                                     \
    }

#define DBAL_REG_WRITE_ARRAY_LOG( reg, reg_offset, block, num_elem)                                                 \
    if(logger_enable)                                                         \
    {                                                                                                           \
        LOG_INFO_EX(BSL_LOG_MODULE, "Writing into register %-30s, entry offset %d, block %d, array elmnts %d ... ",\
                SOC_REG_NAME(unit, reg), reg_offset, block, num_elem);                                          \
    }

#define DBAL_DATA_LOG( data, is_mem)                                                \
{                                                                                   \
    if(logger_enable)                             \
    {                                                                               \
        int ii;                                                                         \
        int size = is_mem?DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS:SOC_REG_ABOVE_64_MAX_SIZE_U32;\
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Data hex: ")));                        \
        for (ii = 0; ii < size; ii++)                                                   \
        {                                                                               \
            if(sal_memcmp(&(data[ii]), zero_buffer_to_compare, size-ii) == 0) \
            {                                                                           \
                LOG_VERBOSE(BSL_LOG_MODULE,(BSL_META_U(unit," {0 <repeats %d times>}"),8*(DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS-ii)));\
                ii = DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS;                             \
            }                                                                           \
            else                                                                        \
            {                                                                           \
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%08x"), data[ii]));      \
            }                                                                           \
        }                                                                               \
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));                          \
    }                                                                                   \
}

typedef enum
{
    DBAL_HL_NEW_ACCESS_SCOPE_LOCAL_FIELD,
    DBAL_HL_NEW_ACCESS_SCOPE_GLOBAL_FIELD
} dbal_hl_new_access_field_scope_e;

#define DBAL_HL_MUL_RES_INFO(access_type) (entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[access_type])

#define DBAL_HL_MEM_MUL_RES_INFO    DBAL_HL_MUL_RES_INFO(DBAL_HL_ACCESS_MEMORY)

#define DBAL_HL_REG_MUL_RES_INFO    DBAL_HL_MUL_RES_INFO(DBAL_HL_ACCESS_REGISTER)

#define DBAL_HL_NEW_ACCESS_DEVICE_TYPE_INFO(unit) (SOC_NEW_ACCESS_RUNTIME_INFO(unit)->device_type_info)

shr_error_e dbal_hl_access_block_calc(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 core_id,
    int memory,
    dbal_offset_encode_info_t * block_index_info,
    dbal_hl_l2p_field_info_t * curr_l2p_info,
    int is_mem,
    int *block,
    int *num_of_blocks);

shr_error_e dbal_hl_mem_write_per_access(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int mem,
    int block_indx,
    int array_indx,
    int entry_indx,
    uint32 *data);

shr_error_e dbal_hl_access_field_info_get_per_access(
    int unit,
    CONST dbal_logical_table_t * table,
    int regmem,
    int hw_fiel_id,
    dbal_hl_new_access_field_scope_e field_scope,
    int is_mem,
    int *field_offset,
    int *field_len,
    char *field_name);

shr_error_e dbal_hl_access_memory_max_index_get_per_access(
    int unit,
    int mem,
    dbal_logical_table_t * table,
    uint32 *index_max);

shr_error_e dbal_hl_access_is_field_exist_per_access(
    int unit,
    CONST dbal_logical_table_t * table,
    int regmem,
    int hw_field,
    int is_mem,
    int *is_field_exist);

int dbal_hl_access_block_any_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int is_mem);

shr_error_e dbal_hl_mem_read_per_access(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int mem,
    int block_indx,
    int array_indx,
    int entry_indx,
    uint32 *data);

const char *dbal_hl_access_regmem_name_get(
    int unit,
    CONST dbal_logical_table_t * table,
    int regmem,
    uint8 is_mem);

const char *dbal_hl_access_field_name_get(
    int unit,
    CONST dbal_logical_table_t * table,
    dbal_hl_l2p_field_info_t * curr_l2p_info);

shr_error_e dbal_hl_access_fill_partial_table_with_entry_per_access(
    int unit,
    const dbal_logical_table_t * table,
    int mem,
    unsigned array_index_start,
    int block_index,
    int index_start,
    int nof_entries,
    void *entry_data);

#if defined(BCM_ACCESS_SUPPORT)

shr_error_e dbal_hl_new_access_regmem_info_get(
    int unit,
    int global_regmem,
    const access_device_regmem_t ** rm_info);

shr_error_e dbal_hl_new_access_regmem_local_get(
    int unit,
    int global_regmem,
    access_local_regmem_id_t * local_regmem_id);
#endif

shr_error_e dbal_hl_access_regmem_validate(
    int unit,
    int table_id,
    int regmem,
    int is_reg);

shr_error_e dbal_hl_access_field_offset_get_per_access(
    int unit,
    CONST dbal_logical_table_t * table,
    dbal_hl_l2p_field_info_t * curr_l2p_info,
    int is_mem,
    int *field_offset);

shr_error_e dbal_hl_reg_default_mode_get_per_access(
    int unit,
    soc_reg_t reg,
    int is_new_access,
    int *is_non_standard);

shr_error_e dbal_hl_access_memreg_size_get_per_access(
    int unit,
    int global_regmem,
    int is_new_access,
    uint32 *regmem_size_bits);

shr_error_e dbal_hl_tcam_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);

shr_error_e dbal_hl_tcam_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle);

shr_error_e dbal_hl_tcam_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle);

shr_error_e dbal_hl_tcam_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle);

shr_error_e dbal_hl_tcam_is_entry_default(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *is_default);

shr_error_e dbal_hl_hook_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *continue_regular_process);

shr_error_e dbal_hl_hook_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *continue_regular_process);

shr_error_e dbal_hl_access_encode(
    int unit,
    dbal_value_field_encode_types_e encode_type,
    uint32 encode_param,
    uint32 *input_val,
    uint32 *output_val,
    uint32 field_size);

shr_error_e dbal_hl_access_decode(
    int unit,
    dbal_value_field_encode_types_e encode_type,
    uint32 encode_param,
    uint32 *input_val,
    uint32 *output_val,
    uint32 field_size);

shr_error_e dbal_hl_hook_res_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *continue_regular_process);

shr_error_e dbal_hl_set_by_access_type(
    int unit,
    dbal_hard_logic_access_types_e access_type,
    dbal_entry_handle_t * entry_handle);

shr_error_e dbal_hl_get_by_access_type(
    int unit,
    dbal_hard_logic_access_types_e access_type,
    dbal_entry_handle_t * entry_handle,
    int result_type);

shr_error_e dbal_hl_hook_oamp_rmep_db_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle);

uint32 dbal_hl_hook_oam_calc_access_address(
    int unit,
    uint32 logical_address);

shr_error_e dbal_hl_hook_oamp_mep_db_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle);

shr_error_e dbal_hl_hook_oamp_mep_db_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle);

#endif
