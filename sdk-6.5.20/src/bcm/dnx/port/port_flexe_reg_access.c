/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC_JR2_REG_ACCESS
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_REG

 /*************
 * INCLUDES  *
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/mem.h>
#include <soc/types.h>
#include <soc/defs.h>
#include <soc/cm.h>
#include <soc/portmode.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <bcm_int/dnx/port/port_flexe_reg_access.h>

 /*************
 * DEFINES   *
 */
#define FLEXE_TABLE_BASE_ADDR   0x20000

 /*************
 *  MACROS   *
 */

 /*************
 * TYPE DEFS *
 */

 /*************
 * GLOBALS   *
 */

 /*************
 * FUNCTIONS *
 */
/*
 * Function:
 *      _flexe_crc_caculate
 * Purpose:
 *      calculate crc for flexe core memory access
 * Parameters:
 *      value   - the value to caculate crc.
 *      b       - the pointer to the crc 
 * Returns:
 *      none.
 */
static void
_flexe_crc_caculate(
    uint32 value,
    uint8 *b)
{
    uint32 i = 0;
    uint8 d[24] = { 0 };
    uint8 newcrc[8];

    *b = 0;
    for (i = 0; i < 24; i++)
    {
        d[i] = (value >> i) & 1;
    }
    newcrc[0] = d[23] ^ d[21] ^ d[19] ^ d[18] ^ d[16] ^ d[14] ^ d[12] ^ d[8] ^ d[7] ^ d[6] ^ d[0];
    newcrc[1] =
        d[23] ^ d[22] ^ d[21] ^ d[20] ^ d[18] ^ d[17] ^ d[16] ^ d[15] ^ d[14] ^ d[13] ^ d[12] ^ d[9] ^ d[6] ^ d[1] ^
        d[0];
    newcrc[2] = d[22] ^ d[17] ^ d[15] ^ d[13] ^ d[12] ^ d[10] ^ d[8] ^ d[6] ^ d[2] ^ d[1] ^ d[0];
    newcrc[3] = d[23] ^ d[18] ^ d[16] ^ d[14] ^ d[13] ^ d[11] ^ d[9] ^ d[7] ^ d[3] ^ d[2] ^ d[1];
    newcrc[4] = d[19] ^ d[17] ^ d[15] ^ d[14] ^ d[12] ^ d[10] ^ d[8] ^ d[4] ^ d[3] ^ d[2];
    newcrc[5] = d[20] ^ d[18] ^ d[16] ^ d[15] ^ d[13] ^ d[11] ^ d[9] ^ d[5] ^ d[4] ^ d[3];
    newcrc[6] = d[21] ^ d[19] ^ d[17] ^ d[16] ^ d[14] ^ d[12] ^ d[10] ^ d[6] ^ d[5] ^ d[4];
    newcrc[7] = d[22] ^ d[20] ^ d[18] ^ d[17] ^ d[15] ^ d[13] ^ d[11] ^ d[7] ^ d[6] ^ d[5];
    for (i = 0; i < 8; i++)
    {
        *b = (newcrc[i] << i) | *b;
    }
}

/** see header file */
shr_error_e
dnx_flexe_reg_read(
    int unit,
    uint32 reg_addr,
    uint32 *data)
{
    uint32 val[16];

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(data, _SHR_E_PARAM, "data");

    /** set burst size = 0 */
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, FSCL_FLEXE_CORE_REGISTERS_BRIDGEr, REG_PORT_ANY,
                                           FLEXE_CORE_REGISTERS_BURST_SIZEf, 0));

    /*
     * read FSCL_FLEXE_CORE_REGISTERSm with entry of flexe core reg address 
     */
    SHR_IF_ERR_EXIT(soc_mem_array_read_flags(unit, FSCL_FLEXE_CORE_REGISTERSm, 0, -1,
                                             reg_addr, val, SOC_MEM_DONT_USE_CACHE));
    *data = val[0];
exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_flexe_reg_write(
    int unit,
    uint32 reg_addr,
    uint32 data)
{
    uint32 val[16];
    SHR_FUNC_INIT_VARS(unit);

    /** set burst size = 0 */
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, FSCL_FLEXE_CORE_REGISTERS_BRIDGEr, REG_PORT_ANY,
                                           FLEXE_CORE_REGISTERS_BURST_SIZEf, 0));

    /*
     * write FSCL_FLEXE_CORE_REGISTERSm with entry of flexe core reg address 
     */
    sal_memset(val, 0, sizeof(val));
    val[0] = data;
    SHR_IF_ERR_EXIT(soc_mem_array_write(unit, FSCL_FLEXE_CORE_REGISTERSm, 0, -1, reg_addr, val));

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_flexe_mem_read(
    int unit,
    uint32 ram_addr,
    uint32 entry_idx,
    uint32 width,
    uint32 *data)
{
    uint32 i, width_32;
    uint32 header = 0;
    uint8 crc_data;
    uint32 table_base_addr = FLEXE_TABLE_BASE_ADDR;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(data, _SHR_E_PARAM, "data");

    width_32 = BITS2WORDS(width);
    header = (width_32 << 16) | (1 << 8);
    _flexe_crc_caculate(header, &crc_data);
    header = (((uint32) crc_data) << 24) | (width_32 << 16) | (1 << 8);

    SHR_IF_ERR_EXIT(dnx_flexe_reg_write(unit, table_base_addr + 0, header));
    SHR_IF_ERR_EXIT(dnx_flexe_reg_write(unit, table_base_addr + 1, ram_addr));
    SHR_IF_ERR_EXIT(dnx_flexe_reg_write(unit, table_base_addr + 3, entry_idx));

    for (i = width_32; i >= 1; i--)
    {
        if (i == 1)
        {
            SHR_IF_ERR_EXIT(dnx_flexe_reg_read(unit, table_base_addr + 0xfd, data));
            break;
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_flexe_reg_read(unit, table_base_addr + 0xfc, data + i - 1));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_flexe_mem_write(
    int unit,
    uint32 ram_addr,
    uint32 entry_idx,
    uint32 width,
    uint32 *data)
{
    uint32 ii, header = 0, width_32;
    uint8 crc_data = 0;
    uint32 buf[16];

    SHR_FUNC_INIT_VARS(unit);
    width_32 = BITS2WORDS(width);
    header = (width_32 << 16) | (1 << 8) | 0x40;
    _flexe_crc_caculate(header, &crc_data);
    header = (((uint32) crc_data) << 24) | (width_32 << 16) | (1 << 8) | 0x40;

    sal_memset(buf, 0, sizeof(buf));
    buf[0] = header;
    buf[1] = ram_addr;
    buf[2] = entry_idx;

    for (ii = width_32; ii > 0; ii--)
    {
        buf[width_32 - ii + 3] = data[ii - 1];
    }

    /** set burst size */
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, FSCL_FLEXE_CORE_REGISTERS_BRIDGEr, REG_PORT_ANY,
                                           FLEXE_CORE_REGISTERS_BURST_SIZEf, width_32 + 2));
    SHR_IF_ERR_EXIT(soc_mem_array_write(unit, FSCL_FLEXE_CORE_REGISTERSm, 0, -1, FLEXE_TABLE_BASE_ADDR, buf));

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_flexe_reg_get(
    int unit,
    soc_reg_t reg,
    uint32 index,
    uint32 *data)
{
    uint32 ii, reg_addr, reg_length;
    int blk;
    uint8 at;
    SHR_FUNC_INIT_VARS(unit);

    if (!soc_is_reg_flexe_core(unit, reg))
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

    reg_addr = soc_reg_addr_get(unit, reg, 0, index, SOC_REG_ADDR_OPTION_NONE, &blk, &at);;
    reg_length = dnx_drv_soc_reg_words(unit, reg);

    for (ii = 0; ii < reg_length; ii++)
    {
        SHR_IF_ERR_EXIT(dnx_flexe_reg_read(unit, reg_addr + ii, data + ii));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_flexe_reg_set(
    int unit,
    soc_reg_t reg,
    uint32 index,
    uint32 *data)
{
    uint32 ii, reg_addr, reg_length;
    int blk;
    uint8 at;
    SHR_FUNC_INIT_VARS(unit);

    if (!soc_is_reg_flexe_core(unit, reg))
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

    reg_addr = soc_reg_addr_get(unit, reg, 0, index, SOC_REG_ADDR_OPTION_NONE, &blk, &at);;
    reg_length = dnx_drv_soc_reg_words(unit, reg);

    for (ii = 0; ii < reg_length; ii++)
    {
        SHR_IF_ERR_EXIT(dnx_flexe_reg_write(unit, reg_addr + ii, data[ii]));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_flexe_mem_array_read(
    int unit,
    uint32 flags,
    soc_mem_t mem,
    int copyno,
    uint32 array_index,
    uint32 entry_index,
    void *entry_data)
{
    int rc;
    uint32 mem_addr, width;
    uint8 acc_type;
    uint8 orig_read_mode;
    uint32 cache_consistency_check;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * First attemp read from cache 
     */
    if ((flags & SOC_MEM_DONT_USE_CACHE) != SOC_MEM_DONT_USE_CACHE)
    {
        orig_read_mode = dnx_drv_soc_mem_force_read_through(unit);
        dnx_drv_soc_mem_force_read_through_set(unit, 0);
        rc = _soc_mem_read_cache_attempt(unit, flags, mem, copyno, entry_index, array_index,
                                         entry_data, NULL, &cache_consistency_check);
        dnx_drv_soc_mem_force_read_through_set(unit, orig_read_mode);
        if (rc == TRUE)
        {
            SHR_EXIT();
        }
    }

    mem_addr = soc_mem_addr_get(unit, mem, array_index, copyno, 0, &acc_type);
    width = dnx_drv_soc_mem_words(unit, mem);
    width = WORDS2BITS(width);
    SHR_IF_ERR_EXIT(dnx_flexe_mem_read(unit, mem_addr, entry_index, width, entry_data));

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_flexe_mem_array_write(
    int unit,
    uint32 flags,
    soc_mem_t mem,
    int copyno,
    uint32 array_index,
    uint32 entry_index,
    void *entry_data)
{
    uint32 mem_addr, width;
    uint8 acc_type;
    SHR_FUNC_INIT_VARS(unit);

    mem_addr = soc_mem_addr_get(unit, mem, array_index, copyno, 0, &acc_type);
    width = dnx_drv_soc_mem_words(unit, mem);
    width = WORDS2BITS(width);
    SHR_IF_ERR_EXIT(dnx_flexe_mem_write(unit, mem_addr, entry_index, width, entry_data));

    /*
     * update cache 
     */
    if ((flags & SOC_MEM_DONT_USE_CACHE) != SOC_MEM_DONT_USE_CACHE)
    {
        _soc_mem_write_cache_update(unit, mem, copyno, 0, entry_index, array_index, entry_data, NULL, NULL, NULL);
    }
exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_flexe_mem_array_fill_range(
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
    uint32 mem_addr, width;
    uint8 acc_type;
    uint32 ii, jj;
    uint32 header = 0;
    const uint32 *data = (uint32 *) entry_data;
    uint8 crc_data = 0;
    uint32 buf[16];

    SHR_FUNC_INIT_VARS(unit);

    mem_addr = soc_mem_addr_get(unit, mem, min_ar_index, copyno, 0, &acc_type);
    width = dnx_drv_soc_mem_words(unit, mem);

    header = (width << 16) | (1 << 8) | 0x40;
    _flexe_crc_caculate(header, &crc_data);
    header = (((uint32) crc_data) << 24) | (width << 16) | (1 << 8) | 0x40;

    sal_memset(buf, 0, sizeof(buf));
    buf[0] = header;

    for (ii = width; ii > 0; ii--)
    {
        buf[width - ii + 3] = data[ii - 1];
    }

    /** set burst size */
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, FSCL_FLEXE_CORE_REGISTERS_BRIDGEr, REG_PORT_ANY,
                                           FLEXE_CORE_REGISTERS_BURST_SIZEf, width + 2));

    for (ii = min_ar_index; ii <= max_ar_index; ++ii)
    {
        mem_addr = soc_mem_addr_get(unit, mem, ii, copyno, 0, &acc_type);
        buf[1] = mem_addr;

        for (jj = index_min; jj <= index_max; ++jj)
        {
            buf[2] = jj;
            SHR_IF_ERR_EXIT(soc_mem_array_write(unit, FSCL_FLEXE_CORE_REGISTERSm, 0, -1, FLEXE_TABLE_BASE_ADDR, buf));
        }
    }

    /*
     * update cache
     */
    if ((flags & SOC_MEM_DONT_USE_CACHE) != SOC_MEM_DONT_USE_CACHE)
    {
        _soc_mem_write_cache_update_range(unit, mem, min_ar_index, max_ar_index, copyno, index_min, index_max,
                                          entry_data);
    }

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
