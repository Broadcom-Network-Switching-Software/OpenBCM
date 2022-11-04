/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * SOC DNXC IPROC
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_INIT

#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/dnxc/dnxc_cmic.h>
#include <soc/cmicx_miim.h>
#include <soc/dnxc/error.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#ifdef BCM_ACCESS_SUPPORT
#include <soc/access/access.h>
#include <soc/access/auto_generated/common_enum.h>
#include <soc/access/access_miim.h>
#endif

int
soc_dnxc_cmic_sbus_timeout_set(
    int unit,
    uint32 core_freq_khz,
    int schan_timeout)
{

    uint32 freq_mhz = core_freq_khz / 1000;
    uint32 ticks, max_uint = 0xFFFFFFFF, max_ticks = 0x3FFFFF;
    uint32 required_value, mantissa_value, nof_mantissa_bits, exponent_value, nof_exponent_bits, timeout;

    SHR_FUNC_INIT_VARS(unit);

    if ((max_uint / freq_mhz) > schan_timeout)
    {
        ticks = freq_mhz * schan_timeout;
        ticks = ((ticks / 100) * 75);
    }
    else
    {
        ticks = max_ticks;
    }

    if (SOC_IS_J2X(unit))
    {

        required_value = ((uint64) ticks) * 9 / 10;
        mantissa_value = required_value;
        nof_mantissa_bits = 10;
        nof_exponent_bits = 5;

        SHR_IF_ERR_EXIT(utilex_break_to_mnt_exp_round_down
                        (required_value, nof_mantissa_bits, nof_exponent_bits, 0, &mantissa_value, &exponent_value));

        timeout = (exponent_value << nof_mantissa_bits) | mantissa_value;

        SOC_CONTROL(unit)->bistTimeout = timeout;
    }

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        SHR_IF_ERR_EXIT(access_regmem(unit,
                                      FLAG_ACCESS_IS_WRITE,
                                      rCMIC_TOP_SBUS_TIMEOUT, ACCESS_ALL_BLOCK_INSTANCES, 0, 0, &ticks));
    }
    else
#endif
    {
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_TIMEOUTr(unit, ticks));
    }
exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxc_cmic_miim_operation(
    int unit,
    int is_write,
    int clause,
    uint32 phy_id,
    uint32 phy_reg_addr,
    uint16 *phy_data)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_ACCESS_SUPPORT
#endif
    {
        SHR_IF_ERR_EXIT(soc_cmicx_miim_operation(unit, is_write, clause, phy_id, phy_reg_addr, phy_data));
    }

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxc_cmic_miim_read(
    int unit,
    int clause,
    uint32 phy_id,
    uint32 phy_reg_addr,
    uint16 *phy_rd_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxc_cmic_miim_operation(unit, FALSE, clause, phy_id, phy_reg_addr, phy_rd_data));

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxc_cmic_miim_write(
    int unit,
    int clause,
    uint32 phy_id,
    uint32 phy_reg_addr,
    uint16 phy_wr_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxc_cmic_miim_operation(unit, TRUE, clause, phy_id, phy_reg_addr, &phy_wr_data));

exit:
    SHR_FUNC_EXIT;
}
