/** \file init_mem.h
 * DNX init memories. 
 * - Set memories to default values 
 * - Get the default value defined per each memory 
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _BCMINT_DNX_INIT_INIT_MEM_H_INCLUDED
/*
 * { 
 */
#define _BCMINT_DNX_INIT_INIT_MEM_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * Include files.
 * {
 */
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/control.h>

#ifdef DNX_DATA_INTERNAL
#undef DNX_DATA_INTERNAL
#endif
#include <soc/mcm/allenum.h>

#ifdef BCM_ACCESS_SUPPORT
#include <soc/access/access.h>
#include <soc/access/auto_generated/common_enum.h>
#endif /* BCM_ACCESS_SUPPORT */

/*
 * }
 */
#define DNX_DYNAMIC_MEMORY_ACCESS_SET(unit, _blk_, _mem_, enable) \
    dnx_dynamic_memory_access_set(unit, _blk_##_ENABLE_DYNAMIC_MEMORY_ACCESSr, ENABLE_DYNAMIC_MEMORY_ACCESS_##_mem_##f, enable)

/** 
  * the macro is aim to disable write protection for array memory 
  * For array memory, there are different field in blk_ENABLE_DYNAMIC_MEMORY_ACCESSr register, each represent an array
  * such as OCB_ENABLE_DYNAMIC_MEMORY_ACCESSr ENABLE_DYNAMIC_MEMORY_ACCESS_FBM_BANK_MEM_0f ~ ENABLE_DYNAMIC_MEMORY_ACCESS_FBM_BANK_MEM_31f
  */
#define DNX_DYNAMIC_MEMORY_ARRAY_ACCESS_SET(unit, _blk_, mem, enable) \
    dnx_dynamic_memory_access_set_internal(unit, _blk_##_ENABLE_DYNAMIC_MEMORY_ACCESSr, mem, enable)

/* 
 * Typedefs 
 * { 
 */
 /**
  * \brief - each valid writable memory init value will be set according to mode
  */
typedef enum
{
    /**
     * Set init value to zero
     */
    dnx_init_mem_default_mode_zero = 0,
    /**
     * Do not set init value.  
     * Might be used if the table will be set in module init. 
     */
    dnx_init_mem_default_mode_none = 1,
    /**
     * Set default value to all ones (0xFFFF...)  
     */
    dnx_init_mem_default_mode_all_ones = 2,
    /**
     * Each entry value will be the index.
     */
    dnx_init_mem_default_mode_index = 3,
    /**
     * Each entry value will be the index. Should be used for memories that are array of indexes
     */
    dnx_init_mem_default_mode_index_seq_array = 4,
    /**
     * Custom, expected a callback to provide the init value.
     * This mode do not support a init value per entry (all the input parameters in the callback shuld be ignored).
     */
    dnx_init_mem_default_mode_custom = 5,

    /**
     * Custom per entry, expected a callback to provide the init value.
     * This mode does not use slam dma and therefore less optimized
     */
    dnx_init_mem_default_mode_custom_per_entry = 6,
} dnx_init_mem_default_mode_e;

/**
 * \brief - memory reset mode.
 *          Specify the method used to reset all writable memories that are not listed in dnx data (module='dev_init', sub_module='mem', table='default')
 */
typedef enum
{
    /**
     * Do not reset memories
     */
    dnx_init_mem_reset_mode_none = 0,
    /**
     * Reset using slam DMA
     */
    dnx_init_mem_reset_mode_dma = 1,
    /**
     * Reset using indirect commands mechanism
     */
    dnx_init_mem_reset_mode_indirect = 2
} dnx_init_mem_reset_mode_e;

/** 
 * \brief 
 * Callback to get custom init value. 
 * For simple custom mode all the input parameters should be ignored (one table for all the memory)
 */
typedef int (
    *dnx_init_mem_default_get_f) (
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data);

/*
 * } 
 */

shr_error_e dnx_init_mems(
    int unit);

shr_error_e access_init_mems(
    int unit);

/**
 * \brief - Perform clock power down
 *            
 * \param [in] unit - unit # 
 *  
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_clock_power_down(
    int unit);

/**
 * \brief - check if the memory has non standard default value and return it is_non_standard (non standard means that 
 *        not all zero).
 *  
 * \param [in] unit - unit # 
 * \param [in] mem - required memory
 * \param [in] is_non_standard - pointer to the result 0 means standard (all zero)
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_mem_default_mode_get(
    int unit,
    soc_mem_t mem,
    int *is_non_standard);

/**
 * \brief - Get default value of memory.
 *          Return the value which defined explicitly in dnx_data_dev_init.mem.default.
 *          If not found will return zero.
 * 
 * \param [in] unit - unit # 
 * \param [in] mem - required memory
 * \param [in] array_index - used for memory arrays (if not relevant set to 0) 
 * \param [in] copyno - block index 
 * \param [in] index - entry index 
 * \param [out] entry - pointer to uint32 buffer (size must be > SOC_MEM_WORDS(unit, mem))
 * \param [out] mode - the mode used to set the defaults
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_mem_default_get(
    int unit,
    soc_mem_t mem,
    int array_index,
    int copyno,
    int index,
    uint32 *entry,
    dnx_init_mem_default_mode_e * mode);

/**
 * \brief - Enable dynamic memory access.
 * 
 * \param [in] unit - unit # 
 * \param [in] reg - enable dynamic access register
 * \param [in] field - field
 * \param [in] enable - enable dynamic memory access or not 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_dynamic_memory_access_set(
    int unit,
    soc_reg_t reg,
    soc_field_t field,
    int enable);

/**
 * \brief - Enable dynamic memory access according to specific memory.
 * 
 * \param [in] unit - unit # 
 * \param [in] reg - enable dynamic access register
 * \param [in] mem - required memory
 * \param [in] enable - enable dynamic memory access or not 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_dynamic_memory_access_set_internal(
    int unit,
    soc_reg_t reg,
    soc_mem_t mem,
    int enable);
/**
 * \brief - test dynamic memory access, enable/disable memory write protection to test
 * 
 * \param [in] unit - unit # 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_test_dynamic_memory_access(
    int unit);

#endif /* _BCMINT_DNX_INIT_INIT_MEM_H_INCLUDED */
