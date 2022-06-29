
#ifndef __DNX_PORT_FRAMER_REG_ACCESS_INCLUDED__
#define __DNX_PORT_FRAMER_REG_ACCESS_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif
#include <soc/register.h>

/*
 * Function:
 *      dnx_framer_reg_read_unlock
 * Purpose:
 *      read framer core register by address
 * Parameters:
 *      unit      - StrataSwitch unit #.
 *      reg_addr  - regisger address at flexe core 
 *      data      - (OUT) register read value.
 * Returns:
 *      _SHR_E_XXX.
 */
int dnx_framer_reg_read_unlock(
    int unit,
    uint32 reg_addr,
    uint32 *data);

/*
 * Function:
 *      dnx_framer_reg_write_unlock
 * Purpose:
 *      write framer core register by address
 * Parameters:
 *      unit      - StrataSwitch unit #.
 *      reg_addr  - regisger address at flexe core 
 *      data      - register write value.
 * Returns:
 *      _SHR_E_XXX.
 */
int dnx_framer_reg_write_unlock(
    int unit,
    uint32 reg_addr,
    uint32 data);

/*
 * Function:
 *      dnx_framer_reg_get
 * Purpose:
 *      read framer core register by reg id
 * Parameters:
 *      unit    - StrataSwitch unit #.
 *      reg     - id for framer core register
 *      index   - array index for framer core register.
 *      data    - (OUT) register read value.
 * Returns:
 *      _SHR_E_XXX.
 */
shr_error_e dnx_framer_reg_get(
    int unit,
    soc_reg_t reg,
    uint32 index,
    uint32 *data);

/*
 * Function:
 *      dnx_framer_reg_set
 * Purpose:
 *      write framer core register by reg id
 * Parameters:
 *      unit    - StrataSwitch unit #.
 *      reg     - id for framer core register
 *      index   - array index for framer core register.
 *      data    - register write value.
 * Returns:
 *      _SHR_E_XXX.
 */
shr_error_e dnx_framer_reg_set(
    int unit,
    soc_reg_t reg,
    uint32 index,
    uint32 *data);

/*
 * Function:
 *      dnx_framer_mem_array_read
 * Purpose:
 *      read framer core memory by mem id
 * Parameters:
 *      unit        - StrataSwitch unit #.
 *      flags       - only SOC_MEM_DONT_USE_CACHE is concern
 *      mem         - id for framer core memory
 *      copyno      - copyno for framer core memory
 *      array_index - array index for framer core memory
 *      entry_index - entry index for framer core memory
 *      entry_data  - memory read value.
 * Returns:
 *      _SHR_E_XXX.
 */
shr_error_e dnx_framer_mem_array_read(
    int unit,
    uint32 flags,
    soc_mem_t mem,
    int copyno,
    uint32 array_index,
    uint32 entry_index,
    void *entry_data);

/*
 * Function:
 *      dnx_framer_mem_array_write
 * Purpose:
 *      write framer core memory by mem id
 * Parameters:
 *      unit        - StrataSwitch unit #.
 *      flags       - only SOC_MEM_DONT_USE_CACHE is concern
 *      mem         - id for framer core memory
 *      copyno      - copyno for framer core memory
 *      array_index - array index for framer core memory
 *      entry_index - entry index for framer core memory
 *      entry_data  - memory write value.
 * Returns:
 *      _SHR_E_XXX.
 */
shr_error_e dnx_framer_mem_array_write(
    int unit,
    uint32 flags,
    soc_mem_t mem,
    int copyno,
    uint32 array_index,
    uint32 entry_index,
    void *entry_data);

/*
 * Function:
 *      dnx_framer_mem_array_fill_range
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
shr_error_e dnx_framer_mem_array_fill_range(
    int unit,
    uint32 flags,
    soc_mem_t mem,
    uint32 min_ar_index,
    uint32 max_ar_index,
    int copyno,
    int index_min,
    int index_max,
    const void *entry_data);

#ifndef NO_FRAMER_LIB_BUILD
/*
 * used for framer core uas access register 
 */
uint32 dnx_framer_reg_read(
    uint8 chip_id,
    uint32 base_addr,
    uint32 offset_addr,
    uint32 *out_data);

/*
 * used for framer core uas access register 
 */
uint32 dnx_framer_reg_write(
    uint8 chip_id,
    uint32 base_addr,
    uint32 offset_addr,
    uint32 in_data);

/*
 * used for framer core uas access memory 
 */
uint32 dnx_framer_ram_read(
    uint8 chip_id,
    uint32 base_addr,
    uint32 ram_addr,
    uint32 ram_deep,
    uint32 ram_width,
    uint32 *out_data);

/*
 * used for framer core uas access memory 
 */
uint32 dnx_framer_ram_write(
    uint8 chip_id,
    uint32 base_addr,
    uint32 ram_addr,
    uint32 ram_deep,
    uint32 ram_width,
    uint32 *in_data);
#endif

#endif /* __DNX_PORT_FLEXE_REG_ACCESS_INCLUDED__ */
