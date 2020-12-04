/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DFE PORT
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_STK
#include <shared/bsl.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm/debug.h>
#include <bcm/stat.h>
#include <bcm/stack.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dfe/cmn/dfe_stack.h>
#include <soc/dfe/cmn/dfe_fabric.h>
#include <soc/dfe/cmn/mbcm.h>

/**********************************************************/
/*                  Implementation                        */
/**********************************************************/  

/*
 * Function:
 *      bcm_dfe_stk_init
 * Purpose:
 *      Initialize Stack module
 * Parameters:
 *      unit  - (IN)     Unit number.
 * Returns:
 *      BCM_E_XXX      Error occurred  
 */
 
int 
bcm_dfe_stk_init(
    int unit)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dfe_stk_init
 * Purpose:
 *      Internal function Deinitialize Stack module
 * Parameters:
 *      unit  - (IN)     Unit number.
 * Returns:
 *      BCM_E_XXX      Error occurred  
 */
 
int 
_bcm_dfe_stk_deinit(
    int unit)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dfe_stk_module_enable(int unit, bcm_module_t modid,
                      int nports, int enable)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    DFE_UNIT_LOCK_TAKE(unit);
    
    rv = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_topology_isolate_set,(unit, enable ? soc_dcmn_isolation_status_active : soc_dcmn_isolation_status_isolated));
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    DFE_UNIT_LOCK_RELEASE(unit); 
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dfe_stk_modid_set
 * Purpose:
 *      Set the module ID of the local device
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      fe_id - (IN)     FE id  
 * Returns:
 *      BCM_E_XXX      Error occurred  
 */
 
int 
bcm_dfe_stk_modid_set(
    int unit, 
    uint32 fe_id)
{
    int rv;
    soc_pbmp_t valid_link_bitmap;
    BCMDNX_INIT_FUNC_DEFS;
    
    /*validation*/
    if(!SOC_CHECK_MODULE_ID(fe_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_BADID, (_BSL_BCM_MSG("fe_id out of range %d"), fe_id));
    }
    
    DFE_UNIT_LOCK_TAKE(unit);
    
    rv = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_stk_modid_set,(unit, fe_id));
    BCMDNX_IF_ERR_EXIT(rv);
    rv = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_topology_isolate_set,(unit, soc_dcmn_isolation_status_isolated));
    BCMDNX_IF_ERR_EXIT(rv);

    SOC_PBMP_ASSIGN(valid_link_bitmap, SOC_INFO(unit).sfi.bitmap);

    rv = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_links_bmp_isolate_set,(unit, valid_link_bitmap, soc_dcmn_isolation_status_active)); 
    BCMDNX_IF_ERR_EXIT(rv);
   
exit:   
    DFE_UNIT_LOCK_RELEASE(unit); 
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dfe_stk_my_modid_set
 * Purpose:
 *      Set the module ID of the local device
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      fe_id - (IN)     FE id  
 * Returns:
 *      BCM_E_XXX      Error occurred  
 */
 
int 
bcm_dfe_stk_my_modid_set(
    int unit, 
    uint32 fe_id)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dfe_stk_modid_set(unit, fe_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:   
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dfe_stk_modid_get
 * Purpose:
 *      Get the module ID of the local device
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      fe_id - (OUT)    FE id  
 * Returns:
 *      BCM_E_XXX      Error occurred  
 */
int 
bcm_dfe_stk_modid_get(
    int unit, 
    uint32* fe_id)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    
    /*validation*/
    BCMDNX_NULL_CHECK(fe_id);
    
    DFE_UNIT_LOCK_TAKE(unit);
    
    rv = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_stk_modid_get,(unit, fe_id));
    BCMDNX_IF_ERR_EXIT(rv);
   
exit:   
    DFE_UNIT_LOCK_RELEASE(unit); 
    
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dfe_stk_my_modid_get
 * Purpose:
 *      Get the module ID of the local device
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      fe_id - (OUT)    FE id  
 * Returns:
 *      BCM_E_XXX      Error occurred  
 */
int 
bcm_dfe_stk_my_modid_get(
    int unit, 
    uint32* fe_id)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dfe_stk_modid_get(unit, fe_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:   
    BCMDNX_FUNC_RETURN;
}


int 
bcm_dfe_stk_module_control_get(
    int unit, 
    uint32 flags, 
    bcm_module_t module, 
    bcm_stk_module_control_t control, 
    int *arg)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(arg);

    DFE_UNIT_LOCK_TAKE(unit);

    switch (control)
    {
        case bcmStkModuleAllReachableIgnore:
            rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_stk_valid_module_id_verify, (unit, module, 0));
            BCMDNX_IF_ERR_EXIT(rv);
            rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_stk_module_all_reachable_ignore_id_get, (unit, module, arg));
            BCMDNX_IF_ERR_EXIT(rv);
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid control %d"),control));
    }
exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_dfe_stk_module_control_set(
    int unit, 
    uint32 flags, 
    bcm_module_t module, 
    bcm_stk_module_control_t control, 
    int arg)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    DFE_UNIT_LOCK_TAKE(unit);

    switch (control)
    {
    case bcmStkModuleAllReachableIgnore:
            rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_stk_valid_module_id_verify, (unit, module, 0));
            BCMDNX_IF_ERR_EXIT(rv);
            rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_stk_module_all_reachable_ignore_id_set, (unit, module, arg));
            BCMDNX_IF_ERR_EXIT(rv);
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid control %d"),control));
    }

exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_dfe_stk_module_max_get(
    int unit, 
    uint32 flags, 
    bcm_module_t *max_module)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(max_module);

    DFE_UNIT_LOCK_TAKE(unit);

    if (flags & BCM_STK_MODULE_MAX_ALL_REACHABLE)
    {
        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_stk_module_max_all_reachable_get, (unit, max_module));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else
    {
        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_stk_module_max_fap_get, (unit, max_module));
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_dfe_stk_module_max_set(
    int unit, 
    uint32 flags, 
    bcm_module_t max_module)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    DFE_UNIT_LOCK_TAKE(unit);

    if (flags & BCM_STK_MODULE_MAX_ALL_REACHABLE)
    {
        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_stk_module_max_all_reachable_verify, (unit, max_module));
        BCMDNX_IF_ERR_EXIT(rv);

        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_stk_module_max_all_reachable_set, (unit, max_module));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else
    {
        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_stk_module_max_fap_verify, (unit, max_module));
        BCMDNX_IF_ERR_EXIT(rv);

        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_stk_module_max_fap_set, (unit, max_module));
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
   DFE_UNIT_LOCK_RELEASE(unit);
   BCMDNX_FUNC_RETURN;   
}

#undef _ERR_MSG_MODULE_NAME

