/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC DFE FABRIC SNAKE TEST
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dfe/cmn/mbcm.h>
#include <soc/dfe/cmn/dfe_fabric_cell_snake_test.h>
#include <soc/dfe/fe1600/fe1600_fabric_cell_snake_test.h>
#include <bcm_int/control.h>

int
  soc_dfe_cell_snake_test_prepare(
    int unit, 
    uint32 flags)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;
    
    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }
    
    DFE_UNIT_LOCK_TAKE_SOCDNX(unit);
    
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cell_snake_test_prepare,(unit, flags));
    SOCDNX_IF_ERR_EXIT(rc);
    
exit:
    DFE_UNIT_LOCK_RELEASE_SOCDNX(unit);
    SOCDNX_FUNC_RETURN;
}
    
    

int
  soc_dfe_cell_snake_test_run(
    int unit, 
    uint32 flags, 
    soc_fabric_cell_snake_test_results_t* results)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;
    
    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }
    
    SOCDNX_NULL_CHECK(results);
    
    DFE_UNIT_LOCK_TAKE_SOCDNX(unit);
    
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cell_snake_test_run,(unit, flags, results));
    SOCDNX_IF_ERR_EXIT(rc);
        
exit:
    DFE_UNIT_LOCK_RELEASE_SOCDNX(unit);
    SOCDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME

