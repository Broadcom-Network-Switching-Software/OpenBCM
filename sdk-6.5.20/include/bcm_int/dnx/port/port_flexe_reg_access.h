/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_PORT_FLEXE_REG_ACCESS_INCLUDED__
#define __DNX_PORT_FLEXE_REG_ACCESS_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (Jr2) family only!"
#endif
#include <soc/register.h>

/*
 * Function:
 *      dnx_flexe_reg_read
 * Purpose:
 *      read flexe core register by address
 * Parameters:
 *      unit      - StrataSwitch unit #.
 *      reg_addr  - regisger address at flexe core 
 *      data      - (OUT) register read value.
 * Returns:
 *      _SHR_E_XXX.
 */
shr_error_e dnx_flexe_reg_read(
    int unit,
    uint32 reg_addr,
    uint32 *data);

/*
 * Function:
 *      dnx_flexe_reg_write
 * Purpose:
 *      write flexe core register by address
 * Parameters:
 *      unit      - StrataSwitch unit #.
 *      reg_addr  - regisger address at flexe core 
 *      data      - register write value.
 * Returns:
 *      _SHR_E_XXX.
 */
shr_error_e dnx_flexe_reg_write(
    int unit,
    uint32 reg_addr,
    uint32 data);

/*
 * Function:
 *      dnx_flexe_mem_read
 * Purpose:
 *      read flexe core memory by memory address
 * Parameters:
 *      unit      - StrataSwitch unit #.
 *      ram_addr  - memory ram address at flexe core 
 *      entry_idx - memory entry index.
 *      width     - bits number of memory entry.
 *      data      - (OUT) memory read value.
 * Returns:
 *      _SHR_E_XXX.
 */
shr_error_e dnx_flexe_mem_read(
    int unit,
    uint32 ram_addr,
    uint32 entry_idx,
    uint32 width,
    uint32 *data);

/*
 * Function:
 *      dnx_flexe_mem_write
 * Purpose:
 *      read flexe core memory by memory address
 * Parameters:
 *      unit      - StrataSwitch unit #.
 *      ram_addr  - memory ram address at flexe core 
 *      entry_idx - memory entry index.
 *      width     - bits number of memory entry.
 *      data      - (OUT) memory read value.
 * Returns:
 *      _SHR_E_XXX.
 */
shr_error_e dnx_flexe_mem_write(
    int unit,
    uint32 ram_addr,
    uint32 entry_idx,
    uint32 width,
    uint32 *data);

/*
 * Function:
 *      dnx_flexe_reg_get
 * Purpose:
 *      read flexe core register by reg id
 * Parameters:
 *      unit    - StrataSwitch unit #.
 *      reg     - id for flexe core register
 *      index   - array index for flexe core register.
 *      data    - (OUT) register read value.
 * Returns:
 *      _SHR_E_XXX.
 */
shr_error_e dnx_flexe_reg_get(
    int unit,
    soc_reg_t reg,
    uint32 index,
    uint32 *data);

/*
 * Function:
 *      dnx_flexe_reg_set
 * Purpose:
 *      write flexe core register by reg id
 * Parameters:
 *      unit    - StrataSwitch unit #.
 *      reg     - id for flexe core register
 *      index   - array index for flexe core register.
 *      data    - register write value.
 * Returns:
 *      _SHR_E_XXX.
 */
shr_error_e dnx_flexe_reg_set(
    int unit,
    soc_reg_t reg,
    uint32 index,
    uint32 *data);

/*
 * Function:
 *      dnx_flexe_mem_array_read
 * Purpose:
 *      read flexe core memory by mem id
 * Parameters:
 *      unit        - StrataSwitch unit #.
 *      flags       - only SOC_MEM_DONT_USE_CACHE is concern
 *      mem         - id for flexe core memory
 *      copyno      - copyno for flexe core memory
 *      array_index - array index for for flexe core memory
 *      entry_index - entry index for for flexe core memory
 *      entry_data  - memory read value.
 * Returns:
 *      _SHR_E_XXX.
 */
shr_error_e dnx_flexe_mem_array_read(
    int unit,
    uint32 flags,
    soc_mem_t mem,
    int copyno,
    uint32 array_index,
    uint32 entry_index,
    void *entry_data);

/*
 * Function:
 *      dnx_flexe_mem_array_write
 * Purpose:
 *      write flexe core memory by mem id
 * Parameters:
 *      unit        - StrataSwitch unit #.
 *      flags       - only SOC_MEM_DONT_USE_CACHE is concern
 *      mem         - id for flexe core memory
 *      copyno      - copyno for flexe core memory
 *      array_index - array index for for flexe core memory
 *      entry_index - entry index for for flexe core memory
 *      entry_data  - memory write value.
 * Returns:
 *      _SHR_E_XXX.
 */
shr_error_e dnx_flexe_mem_array_write(
    int unit,
    uint32 flags,
    soc_mem_t mem,
    int copyno,
    uint32 array_index,
    uint32 entry_index,
    void *entry_data);

/*
 * Function:
 *      dnx_flexe_mem_array_fill_range
 * Purpose:
 *      Fill memory entries with the same value
 * Parameters:
 *      unit        - StrataSwitch unit #.
 *      flags       - only SOC_MEM_DONT_USE_CACHE is concern
 *      mem         - id for flexe core memory
 *      min_ar_index  - minimum memory array index
 *      max_ar_index  - miaximum memory array index
 *      copyno  - block
 *      index_min - minimum entry index
 *      index_max - miaximum entry index
 *      entry_data  - memory write value.
 * Returns:
 *      _SHR_E_XXX.
 */
shr_error_e dnx_flexe_mem_array_fill_range(
    int unit,
    uint32 flags,
    soc_mem_t mem,
    uint32 min_ar_index,
    uint32 max_ar_index,
    int copyno,
    int index_min,
    int index_max,
    const void *entry_data);

#endif /* __DNX_PORT_FLEXE_REG_ACCESS_INCLUDED__ */
