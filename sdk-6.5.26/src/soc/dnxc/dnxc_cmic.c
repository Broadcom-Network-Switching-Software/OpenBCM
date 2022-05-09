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
#include <soc/dnxc/error.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#ifdef BCM_ACCESS_SUPPORT
#include <soc/access/access.h>
#include <soc/access/auto_generated/common_enum.h>
#endif

int
soc_dnxc_cmic_sbus_timeout_set(
    int unit,
    uint32 core_freq_khz,
    int schan_timeout)
{

    uint32 freq_mhz = core_freq_khz / 1000;
    uint32 ticks, max_uint = 0xFFFFFFFF, max_ticks = 0x3FFFFF;

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
#ifdef BCM_ACCESS_SUPPORT
    if (SOC_IS_NEW_ACCESS_INITIALIZED(unit))
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
