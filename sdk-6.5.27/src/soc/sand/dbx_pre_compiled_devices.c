/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#include <soc/sand/dbx_pre_compiled_devices.h>
#include <soc/dev_macros.h>
#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT) || defined(BCM_PETRA_SUPPORT)
#include <soc/sand/auto_generated/dnx2_dbx_pre_compiled_devices.h>
#endif

void*
dbx_pre_compiled_devices_top_get(
    int unit,
    char *filepath,
    char *topname,
    int flags)
{
#if defined(BCM_PETRA_SUPPORT)
    if (SOC_IS_ARAD(unit))
    {
        return dbx_pre_compiled_dnx2_devices_top_get(unit, filepath, topname, flags);
    }
#endif
#if defined(BCM_DNX_SUPPORT)
    if (SOC_IS_DNX2(unit))
    {
        return dbx_pre_compiled_dnx2_devices_top_get(unit, filepath, topname, flags);
    }
#endif
#if defined(BCM_DNXF_SUPPORT)
    if (SOC_IS_RAMON(unit))
    {
        return dbx_pre_compiled_dnx2_devices_top_get(unit, filepath, topname, flags);
    }
#endif
    return NULL;
}

