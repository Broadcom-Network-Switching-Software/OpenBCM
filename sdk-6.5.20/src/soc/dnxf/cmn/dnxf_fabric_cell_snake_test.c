/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC DNXF FABRIC SNAKE TEST
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC
#include <shared/bsl.h>
#include <soc/dnxc/error.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_fabric_cell_snake_test.h>
#include <bcm_int/control.h>

int
  soc_dnxf_cell_snake_test_prepare(
    int unit, 
    uint32 flags)
{
    int rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    
    if (!SOC_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }
    
    DNXF_UNIT_LOCK_TAKE(unit);
    
    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_cell_snake_test_prepare,(unit, flags));
    SHR_IF_ERR_EXIT(rc);
    
exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}
    
    

int
  soc_dnxf_cell_snake_test_run(
    int unit, 
    uint32 flags, 
    soc_dnxf_fabric_cell_snake_test_results_t* results)
{
    int rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    
    if (!SOC_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }
    
    SHR_NULL_CHECK(results, _SHR_E_PARAM, "results");
    
    DNXF_UNIT_LOCK_TAKE(unit);
    
    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_cell_snake_test_run,(unit, flags, results));
    SHR_IF_ERR_EXIT(rc);
        
exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE

