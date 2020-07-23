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
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/mem.h>

#include <soc/dnxc/error.h>
#include <soc/types.h>
#include <soc/defs.h>
#include <soc/cm.h>
#include <soc/portmode.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

 /*************
 * DEFINES   *
 */

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

static int
soc_jr2_ilkn_core_instance_access_get(
    int unit,
    int ilkn_offset,
    soc_mem_t * mem,
    int *block_idx)
{
    int ilkn_core, port_in_core = 0;
    SHR_FUNC_INIT_VARS(unit);

    port_in_core = ilkn_offset % dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit);
    ilkn_core = ilkn_offset / dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit);

    *mem = (port_in_core == 0) ? ILE_PORT_0_CPU_ACCESSm : ILE_PORT_1_CPU_ACCESSm;
    *block_idx = dnx_drv_ile_block(unit, ilkn_core);

    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_jr2_ilkn_reg32_get/set
 * Purpose:
 *      Access ILKN-core registers through ILKN wrap blocks:
 *      ILKN_PMH, ILKN_PML0, ILKN_PML1.
 *      designated memorys in wrap blocks contain all core registers.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      reg   - ILKN-core reg num. 
 *      port  - ilkn port num.
 *      index - index of reg, if array.
 *      data  - (OUT) 32-bit reg value.
 * Returns:
 *      _SHR_E_XXX.
 */
int
soc_jr2_ilkn_reg32_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint32 *data)
{
    uint32 addr;
    int offset;
    int block, blk_id;
    uint8 acc_type;
    soc_mem_t mem;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(data, _SHR_E_PARAM, "data");

    if (dnx_drv_soc_reg_is_above_32(unit, (int) reg))
    {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "soc_jr2_ilkn_reg32_get: Register is too big\n")));
    }
    /*
     * get reg address to retrieve offset in ILKN-wrap table
     */
    addr = soc_reg_addr_get(unit, reg, port, index, SOC_REG_ADDR_OPTION_NONE, &block, &acc_type);
    /** taking all LSBs which are relevant for the access */
    addr &= 0xFFF;

    /*
     * get ilkn core instance (should be 0-1)
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &offset));
    /*
     * find which mem to access according to ilkn-core offset
     */
    SHR_IF_ERR_EXIT(soc_jr2_ilkn_core_instance_access_get(unit, offset, &mem, &blk_id));
    /*
     * read correct line from mem
     */
    SHR_IF_ERR_EXIT(soc_mem_read(unit, mem, blk_id, addr, data));

exit:
    SHR_FUNC_EXIT;
}

int
soc_jr2_ilkn_reg32_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint32 data)
{
    uint32 addr;
    int offset;
    int block, blk_id;
    uint8 acc_type;
    soc_mem_t mem;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_drv_soc_reg_is_above_32(unit, reg))
    {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "soc_jr2_ilkn_reg32_set: Register is too big\n")));
    }
    /*
     * get reg address to retrieve offset in ILKN-wrap table
     */
    addr = soc_reg_addr_get(unit, reg, port, index, SOC_REG_ADDR_OPTION_NONE, &block, &acc_type);
    /** taking all LSBs which are relevant for the access */
    addr &= 0xFFF;

    /*
     * get ilkn core instance (should be 0-1)
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &offset));
    /*
     * find which mem to access according to ilkn-core offset
     */
    SHR_IF_ERR_EXIT(soc_jr2_ilkn_core_instance_access_get(unit, offset, &mem, &blk_id));
    /*
     * read correct line from mem
     */
    SHR_IF_ERR_EXIT(soc_mem_write(unit, mem, blk_id, addr, &data));

exit:
    SHR_FUNC_EXIT;
}

int
soc_jr2_ilkn_reg64_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint64 *data)
{
    uint32 data_low;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(data, _SHR_E_PARAM, "data");

    SHR_IF_ERR_EXIT(soc_jr2_ilkn_reg32_get(unit, reg, port, index, &data_low));

    COMPILER_64_SET(*data, 0, data_low);

exit:
    SHR_FUNC_EXIT;
}

int
soc_jr2_ilkn_reg64_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint64 data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_jr2_ilkn_reg32_set(unit, reg, port, index, COMPILER_64_LO(data)));

exit:
    SHR_FUNC_EXIT;
}

int
soc_jr2_ilkn_reg_above_64_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    soc_reg_above_64_val_t data)
{
    uint32 data_low;
    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(data);

    SHR_IF_ERR_EXIT(soc_jr2_ilkn_reg32_get(unit, reg, port, index, &data_low));

    sal_memcpy(data, &data_low, WORDS2BYTES(1));

exit:
    SHR_FUNC_EXIT;
}

int
soc_jr2_ilkn_reg_above_64_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    soc_reg_above_64_val_t data)
{
    uint32 data_low;
    SHR_FUNC_INIT_VARS(unit);

    sal_memcpy(&data_low, data, WORDS2BYTES(1));

    SHR_IF_ERR_EXIT(soc_jr2_ilkn_reg32_set(unit, reg, port, index, data_low));

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
