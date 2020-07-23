/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MBIST
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_mbist.h>
#include <soc/dcmn/dcmn_mem.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/drv.h>
#include <soc/memory.h>
#include <soc/mem.h>


uint32
soc_dcmn_run_mbist_script(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN dcmn_mbist_script_t *script,  
    SOC_SAND_IN dcmn_mbist_device_t *mbist_device, 
    SOC_SAND_INOUT dcmn_mbist_dynamic_t *dynamic
    )
{
	SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(soc_sand_run_mbist_script(unit, script, mbist_device, dynamic));
exit:
    SOCDNX_FUNC_RETURN; 
}


uint32 soc_dcmn_mbist_init_legacy(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN dcmn_mbist_device_t *mbist_device, 
    dcmn_mbist_dynamic_t *dynamic
    )
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(soc_sand_mbist_init_legacy(unit, mbist_device, dynamic));
exit:
    SOCDNX_FUNC_RETURN;
}


uint32 soc_dcmn_mbist_deinit(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN dcmn_mbist_device_t *mbist_device,  
    SOC_SAND_IN dcmn_mbist_dynamic_t *dynamic
    )
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_sand_mbist_deinit(unit, mbist_device, dynamic));
exit:
    SOCDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME


