/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DNXF PORT
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_STK
#include <shared/bsl.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dnxf_dispatch.h>
#include <bcm/debug.h>
#include <bcm/stat.h>
#include <bcm/stack.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/dnxf_stack.h>
#include <soc/dnxf/cmn/dnxf_fabric.h>
#include <soc/dnxf/cmn/mbcm.h>

/**********************************************************/
/*                  Implementation                        */
/**********************************************************/  

int
bcm_dnxf_stk_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API not supported!");

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnxf_stk_init
 * Purpose:
 *      Initialize Stack module
 * Parameters:
 *      unit  - (IN)     Unit number.
 * Returns:
 *      _SHR_E_XXX      Error occurred  
 */
 
int 
dnxf_stk_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      _bcm_dnxf_stk_init
 * Purpose:
 *      Internal function Deinitialize Stack module
 * Parameters:
 *      unit  - (IN)     Unit number.
 * Returns:
 *      _SHR_E_XXX      Error occurred  
 */
 
int 
dnxf_stk_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_stk_module_enable(int unit, bcm_module_t modid,
                      int nports, int enable)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNXF_UNIT_LOCK_TAKE(unit);
    
    SHR_IF_ERR_EXIT(bcm_dnxf_fabric_control_set(unit, bcmFabricIsolate, enable ? 0 : 1));

exit:
    DNXF_UNIT_LOCK_RELEASE(unit); 
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnxf_stk_modid_set
 * Purpose:
 *      Set the module ID of the local device
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      fe_id - (IN)     FE id  
 * Returns:
 *      _SHR_E_XXX      Error occurred  
 */
 
int 
bcm_dnxf_stk_modid_set(
    int unit, 
    int fe_id)
{
    int rv;
    int arg;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    
    /*validation*/
    if(!SOC_DNXF_CHECK_MODULE_ID(fe_id)) {
        SHR_ERR_EXIT(_SHR_E_BADID, "fe_id out of range %d", fe_id);
    }
    
    DNXF_UNIT_LOCK_TAKE(unit);
    
    if (!SOC_IS_RELOADING(unit))
    {
        SHR_IF_ERR_EXIT(bcm_dnxf_fabric_control_get(unit, bcmFabricIsolate, &arg));
        if (arg == 0)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "device should be isolated");
        }
    }

    rv = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_stk_modid_set,(unit, fe_id));
    SHR_IF_ERR_EXIT(rv);
   
exit:   
    DNXF_UNIT_LOCK_RELEASE(unit); 
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnxf_stk_my_modid_set
 * Purpose:
 *      Set the module ID of the local device
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      fe_id - (IN)     FE id  
 * Returns:
 *      _SHR_E_XXX      Error occurred  
 */
 
int 
bcm_dnxf_stk_my_modid_set(
    int unit, 
    int fe_id)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    rv = bcm_dnxf_stk_modid_set(unit, fe_id);
    SHR_IF_ERR_EXIT(rv);

exit:   
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnxf_stk_modid_get
 * Purpose:
 *      Get the module ID of the local device
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      fe_id - (OUT)    FE id  
 * Returns:
 *      _SHR_E_XXX      Error occurred  
 */
int 
bcm_dnxf_stk_modid_get(
    int unit, 
    int* fe_id)
{
    int rv;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    
    /*validation*/
    SHR_NULL_CHECK(fe_id, _SHR_E_PARAM, "fe_id");
    
    DNXF_UNIT_LOCK_TAKE(unit);
    
    rv = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_stk_modid_get,(unit, fe_id));
    SHR_IF_ERR_EXIT(rv);
   
exit:   
    DNXF_UNIT_LOCK_RELEASE(unit); 
    
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnxf_stk_my_modid_get
 * Purpose:
 *      Get the module ID of the local device
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      fe_id - (OUT)    FE id  
 * Returns:
 *      _SHR_E_XXX      Error occurred  
 */
int 
bcm_dnxf_stk_my_modid_get(
    int unit, 
    int* fe_id)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    rv = bcm_dnxf_stk_modid_get(unit, fe_id);
    SHR_IF_ERR_EXIT(rv);

exit:   
    SHR_FUNC_EXIT;
}


int 
bcm_dnxf_stk_module_control_get(
    int unit, 
    uint32 flags, 
    bcm_module_t module, 
    bcm_stk_module_control_t control, 
    int *arg)
{
    int rv;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");

    DNXF_UNIT_LOCK_TAKE(unit);

    switch (control)
    {
        case bcmStkModuleAllReachableIgnore:
            rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_stk_valid_module_id_verify, (unit, module, 0));
            SHR_IF_ERR_EXIT(rv);
            rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_stk_module_all_reachable_ignore_id_get, (unit, module, arg));
            SHR_IF_ERR_EXIT(rv);
            break;
        default:
            SHR_ERR_EXIT(SOC_E_PARAM, "Invalid control %d",control);
    }
exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

int 
bcm_dnxf_stk_module_control_set(
    int unit, 
    uint32 flags, 
    bcm_module_t module, 
    bcm_stk_module_control_t control, 
    int arg)
{
    int rv;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    DNXF_UNIT_LOCK_TAKE(unit);

    switch (control)
    {
    case bcmStkModuleAllReachableIgnore:
            rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_stk_valid_module_id_verify, (unit, module, 0));
            SHR_IF_ERR_EXIT(rv);
            rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_stk_module_all_reachable_ignore_id_set, (unit, module, arg));
            SHR_IF_ERR_EXIT(rv);
            break;
        default:
            SHR_ERR_EXIT(SOC_E_PARAM,"Invalid control %d",control);
    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

int 
bcm_dnxf_stk_module_max_get(
    int unit, 
    uint32 flags, 
    bcm_module_t *max_module)
{
    int rv;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(max_module, _SHR_E_PARAM, "max_module");

    DNXF_UNIT_LOCK_TAKE(unit);

    if (flags & BCM_STK_MODULE_MAX_ALL_REACHABLE)
    {
        rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_stk_module_max_all_reachable_get, (unit, max_module));
        SHR_IF_ERR_EXIT(rv);
    }
    else
    {
        rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_stk_module_max_fap_get, (unit, max_module));
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

int 
bcm_dnxf_stk_module_max_set(
    int unit, 
    uint32 flags, 
    bcm_module_t max_module)
{
    int rv;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNXF_UNIT_LOCK_TAKE(unit);

    if (flags & BCM_STK_MODULE_MAX_ALL_REACHABLE)
    {
        rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_stk_module_max_all_reachable_verify, (unit, max_module));
        SHR_IF_ERR_EXIT(rv);

        rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_stk_module_max_all_reachable_set, (unit, max_module));
        SHR_IF_ERR_EXIT(rv);
    }
    else
    {
        rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_stk_module_max_fap_verify, (unit, max_module));
        SHR_IF_ERR_EXIT(rv);

        rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_stk_module_max_fap_set, (unit, max_module));
        SHR_IF_ERR_EXIT(rv);
    }

exit:
   DNXF_UNIT_LOCK_RELEASE(unit);
   SHR_FUNC_EXIT;   
}

#undef BSL_LOG_MODULE

