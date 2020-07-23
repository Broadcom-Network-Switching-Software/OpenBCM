/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef __DNX_PORT_CUSTOM_REG_ACCESS_INCLUDED__
#define __DNX_PORT_CUSTOM_REG_ACCESS_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (Jr2) family only!"
#endif

#include <soc/register.h>

/*
 * Function:
 *      dnx_custom_reg32_get
 * Purpose:
 *      Access customer registers for DNX device
 *      wraper function to access ILKN or Flexe core  register
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      reg   - core reg num. 
 *      port  - port num.
 *      index - index of reg, if array.
 *      data  - (OUT) 32-bit reg value.
 * Returns:
 *      _SHR_E_XXX.
 */
shr_error_e dnx_custom_reg32_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint32 *data);

/*
 * Function:
 *      dnx_custom_reg32_set
 * Purpose:
 *      set customer registers for DNX device
 *      wraper function to access ILKN or Flexe core  register
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      reg   - core reg num. 
 *      port  - port num.
 *      index - index of reg, if array.
 *      data  - 32-bit reg value.
 * Returns:
 *      _SHR_E_XXX.
 */
shr_error_e dnx_custom_reg32_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint32 data);

/*
 * Function:
 *      dnx_custom_reg64_get
 * Purpose:
 *      get customer registers(64bits) for DNX device
 *      wraper function to access ILKN or Flexe core  register
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      reg   - core reg num. 
 *      port  - port num.
 *      index - index of reg, if array.
 *      data  - (OUT) 64-bit reg value.
 * Returns:
 *      _SHR_E_XXX.
 */
shr_error_e dnx_custom_reg64_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint64 *data);

/*
 * Function:
 *      dnx_custom_reg64_set
 * Purpose:
 *      set customer registers(64bits) for DNX device
 *      wraper function to access ILKN or Flexe core  register
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      reg   - core reg num. 
 *      port  - port num.
 *      index - index of reg, if array.
 *      data  - 64-bit reg value.
 * Returns:
 *      _SHR_E_XXX.
 */
shr_error_e dnx_custom_reg64_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint64 data);

/*
 * Function:
 *      dnx_custom_reg_above_64_get
 * Purpose:
 *      get customer registers(above 64bits) for DNX device
 *      wraper function to access ILKN or Flexe core  register
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      reg   - core reg num. 
 *      port  - port num.
 *      index - index of reg, if array.
 *      data  - (OUT) above 64 bit reg value.
 * Returns:
 *      _SHR_E_XXX.
 */
shr_error_e dnx_custom_reg_above_64_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    soc_reg_above_64_val_t data);

/*
 * Function:
 *      dnx_custom_reg_above_64_set
 * Purpose:
 *      set customer registers(above 64bits) for DNX device
 *      wraper function to access ILKN or Flexe core  register
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      reg   - core reg num. 
 *      port  - port num.
 *      index - index of reg, if array.
 *      data  - above 64 bit reg value.
 * Returns:
 *      _SHR_E_XXX.
 */
shr_error_e dnx_custom_reg_above_64_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    soc_reg_above_64_val_t data);
/*
 * Function:
 *      dnx_custom_mem_array_read
 * Purpose:
 *      read customer memory for DNX device
 *      wraper function to access Flexe core memory
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      mem   - memory
 *      array_index  - memory array index
 *      copyno  - block
 *      index - entry index
 *      entry_data  - entry
 * Returns:
 *      _SHR_E_XXX.
 */
shr_error_e dnx_custom_mem_array_read(
    int unit,
    soc_mem_t mem,
    unsigned array_index,
    int copyno,
    int index,
    void *entry_data);
/*
 * Function:
 *      dnx_custom_mem_array_write
 * Purpose:
 *      write customer memory for DNX device
 *      wraper function to access Flexe core memory
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      mem   - memory
 *      array_index  - memory array index
 *      copyno  - block
 *      index - entry index
 *      entry_data  - entry
 * Returns:
 *      _SHR_E_XXX.
 */
shr_error_e dnx_custom_mem_array_write(
    int unit,
    soc_mem_t mem,
    unsigned array_index,
    int copyno,
    int index,
    void *entry_data);

/*
 * Function:
 *      dnx_custom_mem_array_fill_range
 * Purpose:
 *      Fill memory entries with the same value
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      mem   - memory
 *      min_ar_index  - minimum memory array index
 *      max_ar_index  - miaximum memory array index
 *      copyno  - block
 *      index_min - minimum entry index
 *      index_max - miaximum entry index
 *      entry_data  - entry
 * Returns:
 *      _SHR_E_XXX.
 */
shr_error_e dnx_custom_mem_array_fill_range(
    int unit,
    soc_mem_t mem,
    uint32 min_ar_index,
    uint32 max_ar_index,
    int copyno,
    int index_min,
    int index_max,
    const void *entry_data);

#endif /* __DNX_PORT_CUSTOM_REG_ACCESS_INCLUDED__ */
