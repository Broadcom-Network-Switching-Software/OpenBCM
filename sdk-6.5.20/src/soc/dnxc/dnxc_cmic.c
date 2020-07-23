/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
#include <shared/shrextend/shrextend_debug.h>

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

    SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_TIMEOUTr(unit, ticks));

exit:
    SHR_FUNC_EXIT;
}
