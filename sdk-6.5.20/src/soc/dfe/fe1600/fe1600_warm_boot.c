/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE1600 WARM BOOT
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/error.h>

#ifdef BCM_88750_A0

#include <soc/dcmn/error.h>

#include <soc/dfe/cmn/dfe_warm_boot.h>
#include <soc/drv.h>
#include <soc/dfe/fe1600/fe1600_warm_boot.h>
#include <soc/dfe/fe1600/fe1600_defs.h>

soc_error_t
soc_fe1600_warm_boot_buffer_id_supported_get(int unit, int buffer_id, int *is_supported)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    switch (buffer_id)
    {
       case SOC_DFE_WARM_BOOT_BUFFER_MODID:
       case SOC_DFE_WARM_BOOT_BUFFER_MC:
       case SOC_DFE_WARM_BOOT_BUFFER_FIFO:
       case SOC_DFE_WARM_BOOT_BUFFER_PORT:
       case SOC_DFE_WARM_BOOT_BUFFER_INTR:
       case SOC_DFE_WARM_BOOT_BUFFER_ISOLATE:
           *is_supported = 1;
           break;
       default:
           *is_supported = 0;
           break;
    }


    SOCDNX_FUNC_RETURN;
}

#endif 

#undef _ERR_MSG_MODULE_NAME
