

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DBAL_HL_ACCESS_H_INCLUDED
#  define DBAL_HL_ACCESS_H_INCLUDED

#  ifndef BCM_DNX_SUPPORT
#    error "This file is for use by DNX (JR2) family only!"
#  endif


#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
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

#define DBAL_PACKED_START_LOG(is_mem, hw_element)                                                           \
    if(logger_enable)                                                     \
    {                                                                                                       \
        LOG_INFO_EX(BSL_LOG_MODULE, "\nPacked mode mapped to %s %-30s %s%s\n", is_mem?"memory":"regstr",             \
            is_mem?SOC_MEM_NAME(unit, hw_element): SOC_REG_NAME(unit, hw_element), EMPTY,EMPTY);            \
    }

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


#  define DBAL_HL_MUL_RES_INFO(access_type) (entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[access_type])

#  define DBAL_HL_MEM_MUL_RES_INFO    DBAL_HL_MUL_RES_INFO(DBAL_HL_ACCESS_MEMORY)

#  define DBAL_HL_REG_MUL_RES_INFO    DBAL_HL_MUL_RES_INFO(DBAL_HL_ACCESS_REGISTER)




shr_error_e dbal_hl_mem_block_calc(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 core_id,
    soc_mem_t memory,
    dbal_offset_encode_info_t * block_index_info,
    dbal_fields_e mapped_field_id,
    int *block,
    int *num_of_blocks);


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


shr_error_e dbal_hl_hook_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle);


shr_error_e dbal_hl_hook_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle);


shr_error_e dbal_hl_hook_res_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle);


shr_error_e dbal_hl_set_by_access_type(
    int unit,
    dbal_hard_logic_access_types_e access_type,
    dbal_entry_handle_t * entry_handle);


shr_error_e dbal_hl_get_by_access_type(
    int unit,
    dbal_hard_logic_access_types_e access_type,
    dbal_entry_handle_t * entry_handle,
    int result_type);

#endif 
