/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * SOC_JR2_REG_ACCESS
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_REG

 /*************
 * INCLUDES  *
 *************/
#include <sal/core/boot.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/mem.h>
#include <soc/defs.h>
#include <soc/cm.h>
#include <soc/portmode.h>
#include <soc/cmic.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <bcm_int/dnx/port/port_framer_reg_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_intr.h>

 /*************
 * DEFINES   *
 *************/

 /*************
 *  MACROS   *
 *************/
#define FRAMER_ACCESS_LOCK(unit, _lock) do                   \
        {                                                   \
            if (_lock != NULL)                              \
            {                                               \
                sal_mutex_take(_lock, sal_mutex_FOREVER);   \
            }                                               \
        } while (0)

#define FRAMER_ACCESS_UNLOCK(unit, _lock) do     \
        {                                       \
            if (_lock != NULL)                  \
            {                                   \
                sal_mutex_give(_lock);          \
            }                                   \
        } while (0)

/*
 * OTN Reg address format:
 * bit 0-12:    entry index of core reigster memory
 * bit 13:      should be 0. when ram access, MSB
 * bit 14-19:   module index
 * bit 20:      indicate this is ram control access, 
 *              and suppose this is MSB of entry index
 */
#define FRAMER_REG_ADDR_2_MODULE(_addr)    ((_addr & 0xFC000) >> 14)
#define FRAMER_REG_ADDR_2_ENTRY(_addr)     (_addr & 0x1FFF)

#define FRAMER_CORE_REG_MEM_WLEN        16
#define FRAMER_RAM_MAX_LEN              13

 /*
  * FRAMER reg/ram access control field
  * regr write:  32'h01040000,  assume all register width is 32 bits
  * ram write:   {n[3:0], 8'h5, 16'h0}
  * ram read:    {n[3:0], 8'h1, 16'h0}    
  */
#define FRAMER_CTRL_FIELD_GET(n, c)     ((n << 24) | (c << 16))
#define FRAMER_CTRL_FIELD_REG_WRITE     FRAMER_CTRL_FIELD_GET(1, 4)
#define FRAMER_CTRL_FIELD_RAM_READ(n)   FRAMER_CTRL_FIELD_GET(n, 1)
#define FRAMER_CTRL_FIELD_RAM_WRITE(n)  FRAMER_CTRL_FIELD_GET(n, 5)

 /*************
 * TYPE DEFS *
 *************/

 /*************
 * GLOBALS   *
 *************/

 /*************
 * FUNCTIONS *
 *************/

static int
_dnx_framer_regram_addr_parse(
    int unit,
    uint32 regram_addr,
    soc_mem_t * core_reg_mem,
    int *copyno,
    uint32 *core_reg_idx)
{
    int module_offset;
    const dnxc_data_table_info_t *table_info;
    const dnx_data_intr_general_framer_module_map_t *module_map;

    SHR_FUNC_INIT_VARS(unit);

    module_offset = FRAMER_REG_ADDR_2_MODULE(regram_addr);
    table_info = dnx_data_intr.general.framer_module_map_info_get(unit);
    if (module_offset >= table_info->key_size[0])
    {
        SHR_IF_ERR_EXIT(_SHR_E_NOT_FOUND);
    }
    module_map = dnx_data_intr.general.framer_module_map_get(unit, module_offset);

    *core_reg_mem = module_map->core_regs;
    *copyno = dnx_drv_soc_mem_block_min(unit, *core_reg_mem) + module_map->instance;
    *core_reg_idx = FRAMER_REG_ADDR_2_ENTRY(regram_addr);

exit:
    SHR_FUNC_EXIT;
}

/*
 * read flexe register without lock 
 * sbus_adpt_inst.sbus_addr[13:0] = register_address in the block.
 */
int
dnx_framer_reg_read_unlock(
    int unit,
    uint32 reg_addr,
    uint32 *data)
{
    uint32 val[FRAMER_CORE_REG_MEM_WLEN];
    soc_mem_t core_reg_mem;
    int copyno;
    uint32 core_reg_index;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(data, _SHR_E_PARAM, "data");

    SHR_IF_ERR_EXIT(_dnx_framer_regram_addr_parse(unit, reg_addr, &core_reg_mem, &copyno, &core_reg_index));
    /*
     * read CORE_REGISTERSm with entry of flexe core reg address 
     */
    SHR_IF_ERR_EXIT(soc_mem_array_read_flags(unit, core_reg_mem, 0, copyno,
                                             core_reg_index, val, SOC_MEM_DONT_USE_CACHE));
    *data = val[0];
exit:
    SHR_FUNC_EXIT;
}

/*
 * write flexe register without lock 
 * sbus_adpt_inst.sbus_addr[13:0]    = register_address in the block.
 * sbus_adpt_inst.sbus_wrdata[511:0] = {32'b data, 32'b control_fields};
 * 32'b control_fields = 32'h01040000 (assume all register width is 32 bits)
 */
int
dnx_framer_reg_write_unlock(
    int unit,
    uint32 reg_addr,
    uint32 data)
{
    uint32 val[FRAMER_CORE_REG_MEM_WLEN];
    soc_mem_t core_reg_mem;
    int copyno;
    uint32 core_reg_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(_dnx_framer_regram_addr_parse(unit, reg_addr, &core_reg_mem, &copyno, &core_reg_index));

    /*
     * write FSCL_FLEXE_CORE_REGISTERSm with entry of flexe core reg address 
     */
    sal_memset(val, 0, sizeof(val));
    val[0] = FRAMER_CTRL_FIELD_REG_WRITE;
    val[1] = data;
    SHR_IF_ERR_EXIT(soc_mem_array_write(unit, core_reg_mem, 0, copyno, core_reg_index, val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * read flexe ram without lock 
 * sbus_adpt_inst.sbus_addr[13:0]    = memory_address in the block.
 * sbus_adpt_inst.sbus_wrdata[511:0] = {32'b memory_offset, 32'b control_fields} ;
 * 32'b control_fields = { n[3:0], 8'h1, 16'h0 };
 * the max n is 13.
 */
int
dnx_framer_ram_read_unlock(
    int unit,
    uint32 ram_addr,
    uint32 ram_index,
    uint32 width_32,
    uint32 *data)
{
    uint32 val[FRAMER_CORE_REG_MEM_WLEN], ii;
    soc_mem_t core_reg_mem;
    int copyno;
    uint32 core_reg_index;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(data, _SHR_E_PARAM, "data");
    SHR_MAX_VERIFY(width_32, FRAMER_RAM_MAX_LEN, _SHR_E_PARAM, "width_32");

    SHR_IF_ERR_EXIT(_dnx_framer_regram_addr_parse(unit, ram_addr, &core_reg_mem, &copyno, &core_reg_index));

    /*
     * write _CORE_REGISTERSm for ram read operation
     * the write format is: {control_fields, ram_index}
     */
    sal_memset(val, 0, sizeof(val));
    val[0] = FRAMER_CTRL_FIELD_RAM_READ(width_32);
    val[1] = ram_index;
    SHR_IF_ERR_EXIT(soc_mem_array_write(unit, core_reg_mem, 0, copyno, core_reg_index, val));

    /** read _CORE_REGISTERSm to get the ram result for specific ram index */
    SHR_IF_ERR_EXIT(soc_mem_array_read_flags(unit, core_reg_mem, 0, copyno,
                                             core_reg_index, val, SOC_MEM_DONT_USE_CACHE));
    for (ii = 0; ii < width_32; ii++)
    {
        data[ii] = val[ii];
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * write flexe ram without lock 
 * sbus_adpt_inst.sbus_addr[13:0]    = memory_address in the block.
 * sbus_adpt_inst.sbus_wrdata[511:0] = {32'b memory_offset,  
 *                                      n*32'b data,
 *                                      32'b control_fields} ;
 * 32'b control_fields = { n[3:0], 8'h1, 16'h0 };
 * the max n is 13.
 */
int
dnx_framer_ram_write_unlock(
    int unit,
    uint32 ram_addr,
    uint32 ram_index,
    uint32 width_32,
    uint32 *data)
{
    uint32 val[FRAMER_CORE_REG_MEM_WLEN], ii;
    soc_mem_t core_reg_mem;
    int copyno;
    uint32 core_reg_index;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(data, _SHR_E_PARAM, "data");
    SHR_MAX_VERIFY(width_32, FRAMER_RAM_MAX_LEN, _SHR_E_PARAM, "width_32");

    SHR_IF_ERR_EXIT(_dnx_framer_regram_addr_parse(unit, ram_addr, &core_reg_mem, &copyno, &core_reg_index));

    /*
     * write _CORE_REGISTERSm for ram read operation
     * the write format is: {control_fields, n*32bit data, ram_index}
     */
    sal_memset(val, 0, sizeof(val));
    val[0] = FRAMER_CTRL_FIELD_RAM_WRITE(width_32);
    for (ii = 0; ii < width_32; ii++)
    {
        val[ii + 1] = data[ii];
    }
    val[width_32 + 1] = ram_index;
    SHR_IF_ERR_EXIT(soc_mem_array_write(unit, core_reg_mem, 0, copyno, core_reg_index, val));

exit:
    SHR_FUNC_EXIT;
}

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
shr_error_e
dnx_framer_reg_get(
    int unit,
    soc_reg_t reg,
    uint32 index,
    uint32 *data)
{
    sal_mutex_t framer_lock = NULL;
    soc_custom_reg_access_t *reg_access;
    uint32 ii, reg_addr, reg_length;
    int blk;
    uint8 at;

    SHR_FUNC_INIT_VARS(unit);

    reg_access = dnx_drv_soc_info_custom_reg_access_get(unit);
    framer_lock = reg_access->flexe_access_lock;
    reg_addr = soc_reg_addr_get(unit, reg, 0, index, SOC_REG_ADDR_OPTION_NONE, &blk, &at);;
    reg_length = dnx_drv_soc_reg_words(unit, reg);

    FRAMER_ACCESS_LOCK(unit, framer_lock);
    for (ii = 0; ii < reg_length; ii++)
    {
        SHR_IF_ERR_EXIT(dnx_framer_reg_read_unlock(unit, reg_addr + ii, data + ii));
    }
exit:
    FRAMER_ACCESS_UNLOCK(unit, framer_lock);
    SHR_FUNC_EXIT;
}

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
shr_error_e
dnx_framer_reg_set(
    int unit,
    soc_reg_t reg,
    uint32 index,
    uint32 *data)
{
    sal_mutex_t framer_lock = NULL;
    soc_custom_reg_access_t *reg_access;
    uint32 ii, reg_addr, reg_length;
    int blk;
    uint8 at;

    SHR_FUNC_INIT_VARS(unit);

    reg_access = dnx_drv_soc_info_custom_reg_access_get(unit);
    framer_lock = reg_access->flexe_access_lock;
    reg_addr = soc_reg_addr_get(unit, reg, 0, index, SOC_REG_ADDR_OPTION_NONE, &blk, &at);;
    reg_length = dnx_drv_soc_reg_words(unit, reg);

    FRAMER_ACCESS_LOCK(unit, framer_lock);
    for (ii = 0; ii < reg_length; ii++)
    {
        SHR_IF_ERR_EXIT(dnx_framer_reg_write_unlock(unit, reg_addr + ii, data[ii]));
    }

exit:
    FRAMER_ACCESS_UNLOCK(unit, framer_lock);
    SHR_FUNC_EXIT;
}

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
shr_error_e
dnx_framer_mem_array_read(
    int unit,
    uint32 flags,
    soc_mem_t mem,
    int copyno,
    uint32 array_index,
    uint32 entry_index,
    void *entry_data)
{
    int rc;
    uint32 mem_addr, width_32;
    uint8 acc_type;
    uint32 cache_consistency_check;
    sal_mutex_t framer_lock = NULL;
    soc_custom_reg_access_t *reg_access;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * First attemp read from cache 
     */
    if ((flags & SOC_MEM_DONT_USE_CACHE) != SOC_MEM_DONT_USE_CACHE)
    {
        rc = _soc_mem_read_cache_attempt(unit, flags | SOC_MEM_IGNORE_FORCE_READ_THROUGH, mem, copyno,
                                         entry_index, array_index, entry_data, NULL, &cache_consistency_check);
        if (rc == TRUE)
        {
            SHR_EXIT();
        }
    }

    reg_access = dnx_drv_soc_info_custom_reg_access_get(unit);
    framer_lock = reg_access->flexe_access_lock;

    mem_addr = soc_mem_addr_get(unit, mem, array_index, copyno, 0, &acc_type);
    width_32 = dnx_drv_soc_mem_words(unit, mem);

    FRAMER_ACCESS_LOCK(unit, framer_lock);
    SHR_IF_ERR_EXIT(dnx_framer_ram_read_unlock(unit, mem_addr, entry_index, width_32, entry_data));

exit:
    FRAMER_ACCESS_UNLOCK(unit, framer_lock);
    SHR_FUNC_EXIT;
}

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
shr_error_e
dnx_framer_mem_array_write(
    int unit,
    uint32 flags,
    soc_mem_t mem,
    int copyno,
    uint32 array_index,
    uint32 entry_index,
    void *entry_data)
{
    uint32 mem_addr, width_32;
    uint8 acc_type;
    sal_mutex_t framer_lock = NULL;
    soc_custom_reg_access_t *reg_access;
    SHR_FUNC_INIT_VARS(unit);

    reg_access = dnx_drv_soc_info_custom_reg_access_get(unit);
    framer_lock = reg_access->flexe_access_lock;
    mem_addr = soc_mem_addr_get(unit, mem, array_index, copyno, 0, &acc_type);
    width_32 = dnx_drv_soc_mem_words(unit, mem);

    FRAMER_ACCESS_LOCK(unit, framer_lock);
    SHR_IF_ERR_EXIT(dnx_framer_ram_write_unlock(unit, mem_addr, entry_index, width_32, entry_data));

    /*
     * update cache 
     */
    if ((flags & SOC_MEM_DONT_USE_CACHE) != SOC_MEM_DONT_USE_CACHE)
    {
        _soc_mem_write_cache_update(unit, mem, copyno, 0, entry_index, array_index, entry_data, NULL, NULL, NULL);
    }
exit:
    FRAMER_ACCESS_UNLOCK(unit, framer_lock);
    SHR_FUNC_EXIT;
}

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
shr_error_e
dnx_framer_mem_array_fill_range(
    int unit,
    uint32 flags,
    soc_mem_t mem,
    uint32 min_ar_index,
    uint32 max_ar_index,
    int copyno,
    int index_min,
    int index_max,
    const void *entry_data)
{
    uint32 mem_addr, width_32, ii, jj;
    uint8 acc_type;
    sal_mutex_t framer_lock = NULL;
    soc_custom_reg_access_t *reg_access;
    uint32 *data = (uint32 *) entry_data;
    SHR_FUNC_INIT_VARS(unit);

    reg_access = dnx_drv_soc_info_custom_reg_access_get(unit);
    framer_lock = reg_access->flexe_access_lock;
    width_32 = dnx_drv_soc_mem_words(unit, mem);

    FRAMER_ACCESS_LOCK(unit, framer_lock);

    for (ii = min_ar_index; ii <= max_ar_index; ++ii)
    {
        mem_addr = soc_mem_addr_get(unit, mem, ii, copyno, 0, &acc_type);
        for (jj = index_min; jj <= index_max; jj++)
        {
            SHR_IF_ERR_EXIT(dnx_framer_ram_write_unlock(unit, mem_addr, jj, width_32, data));
        }
    }

    /*
     * update cache
     */
    if ((flags & SOC_MEM_DONT_USE_CACHE) != SOC_MEM_DONT_USE_CACHE)
    {
        _soc_mem_write_cache_update_range(unit, mem, min_ar_index, max_ar_index, copyno,
                                          index_min, index_max, entry_data);
    }

exit:
    FRAMER_ACCESS_UNLOCK(unit, framer_lock);
    SHR_FUNC_EXIT;
}

uint32
dnx_framer_reg_read(
    uint8 chip_id,
    uint32 base_addr,
    uint32 offset_addr,
    uint32 *out_data)
{
    int unit = (int) chip_id;
    int rv;
    sal_mutex_t framer_lock = NULL;
    soc_custom_reg_access_t *reg_access;
    uint32 reg_addr;

    reg_access = dnx_drv_soc_info_custom_reg_access_get(unit);
    framer_lock = reg_access->flexe_access_lock;
    reg_addr = base_addr + offset_addr;

    FRAMER_ACCESS_LOCK(unit, framer_lock);
    rv = dnx_framer_reg_read_unlock(unit, reg_addr, out_data);
    FRAMER_ACCESS_UNLOCK(unit, framer_lock);

    return (uint32) rv;
}

uint32
dnx_framer_reg_write(
    uint8 chip_id,
    uint32 base_addr,
    uint32 offset_addr,
    uint32 in_data)
{
    int unit = (int) chip_id;
    int rv;
    sal_mutex_t framer_lock = NULL;
    soc_custom_reg_access_t *reg_access;
    uint32 reg_addr;

    reg_access = dnx_drv_soc_info_custom_reg_access_get(unit);
    framer_lock = reg_access->flexe_access_lock;
    reg_addr = base_addr + offset_addr;

    FRAMER_ACCESS_LOCK(unit, framer_lock);
    rv = dnx_framer_reg_write_unlock(unit, reg_addr, in_data);
    FRAMER_ACCESS_UNLOCK(unit, framer_lock);

    return (uint32) rv;
}

uint32
dnx_framer_ram_read(
    uint8 chip_id,
    uint32 base_addr,
    uint32 ram_addr,
    uint32 ram_deep,
    uint32 ram_width,
    uint32 *out_data)
{
    int unit = (int) chip_id;
    int rv;
    sal_mutex_t framer_lock = NULL;
    soc_custom_reg_access_t *reg_access;
    uint32 width_32;

    reg_access = dnx_drv_soc_info_custom_reg_access_get(unit);
    framer_lock = reg_access->flexe_access_lock;
    width_32 = (ram_width + 31) / 32;

    FRAMER_ACCESS_LOCK(unit, framer_lock);
    rv = dnx_framer_ram_read_unlock(unit, base_addr + ram_addr, ram_deep, width_32, out_data);
    FRAMER_ACCESS_UNLOCK(unit, framer_lock);
    return (uint32) rv;
}

uint32
dnx_framer_ram_write(
    uint8 chip_id,
    uint32 base_addr,
    uint32 ram_addr,
    uint32 ram_deep,
    uint32 ram_width,
    uint32 *in_data)
{
    int unit = (int) chip_id;
    int rv;
    sal_mutex_t framer_lock = NULL;
    soc_custom_reg_access_t *reg_access;
    uint32 width_32;

    reg_access = dnx_drv_soc_info_custom_reg_access_get(unit);
    framer_lock = reg_access->flexe_access_lock;
    width_32 = (ram_width + 31) / 32;

    FRAMER_ACCESS_LOCK(unit, framer_lock);
    rv = dnx_framer_ram_write_unlock(unit, base_addr + ram_addr, ram_deep, width_32, in_data);
    FRAMER_ACCESS_UNLOCK(unit, framer_lock);
    return (uint32) rv;
}
#undef BSL_LOG_MODULE
