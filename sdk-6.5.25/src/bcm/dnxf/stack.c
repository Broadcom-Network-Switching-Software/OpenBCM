/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
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
#include <bcm_int/dnxf/auto_generated/dnxf_fabric_dispatch.h>
#include <bcm/debug.h>
#include <bcm/stat.h>
#include <bcm/stack.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/dnxc/dnxc_verify.h>
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

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API not supported! Per init step APIs not supported. Use advanced init  instead.");

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      dnxf_stk_init
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
    /**
     * Nothing to do.
     * Everything related to stack support is done as an application
     * because it is related to seting FE device modid
     * and putting links and device out of isolations
     * There is nothing generic needed to be configured
     * Still keep function for future use
     */
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      dnxf_stk_deinit
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

    /**
     * Nothing to do
     */
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnxf_stk_module_enable
 * Purpose:
 *      Function to join the device into the system.
 *      Practically the same as getting the device out of isolation
 * Parameters:
 *      unit   - (IN)     Unit number.
 *      modid  - (IN)     NOT USED! Set to -1
 *      nports - (IN)     NOT USED! Set to -1
 *      enable - (IN)     Get in/out of isolate
 * Returns:
 *      _SHR_E_XXX      Error occurred
 */
int
bcm_dnxf_stk_module_enable(
    int unit,
    bcm_module_t modid,
    int nports,
    int enable)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    DNXF_UNIT_LOCK_TAKE(unit);

    SHR_IF_ERR_EXIT(bcm_dnxf_fabric_control_set
                    (unit, bcmFabricIsolate,
                     enable ? soc_dnxc_isolation_status_active : soc_dnxc_isolation_status_isolated));

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
    int arg;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    DNXF_UNIT_LOCK_TAKE(unit);

    /*
     * validation
     */
    if (!SOC_DNXF_CHECK_MODULE_ID(fe_id))
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "fe_id out of range %d", fe_id);
    }

    /*
     * When reloading device is not expected to be isolated because it should be still active
     */
    if (!SOC_IS_RELOADING(unit))
    {
        SHR_IF_ERR_EXIT(bcm_dnxf_fabric_control_get(unit, bcmFabricIsolate, &arg));
        if (arg == 0)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "device should be isolated");
        }
    }

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_stk_modid_set, (unit, fe_id)));

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
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnxf_stk_modid_set(unit, fe_id));

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
    int *fe_id)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    DNXF_UNIT_LOCK_TAKE(unit);
    /*
     * validation
     */
    SHR_NULL_CHECK(fe_id, _SHR_E_PARAM, "fe_id");

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_stk_modid_get, (unit, fe_id)));

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
    int *fe_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnxf_stk_modid_get(unit, fe_id));

exit:
    SHR_FUNC_EXIT;
}

static int
bcm_dnxf_stk_module_control_verify(
    int unit,
    bcm_module_t module)
{
    SHR_FUNC_INIT_VARS(unit);

    if (module < 0 || module >= dnxf_data_fabric.topology.nof_system_modid_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid module id %d \n. Should be between 0 and %d ", module,
                     dnxf_data_fabric.topology.nof_system_modid_get(unit) - 1);
    }

exit:
    SHR_FUNC_EXIT;
}

static int
bcm_dnxf_stk_module_control_get_verify(
    int unit,
    bcm_module_t module,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");

    SHR_INVOKE_VERIFY_DNXC(bcm_dnxf_stk_module_control_verify(unit, module));
exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnxf_stk_module_control_get
 * Purpose:
 *      Get the module ID of the local device
 * Parameters:
 *      unit    - (IN)     Unit number.
 *      flags   - (IN)     flags.
 *      module  - (IN)     for which module to apply the control.
 *      control - (IN)     which control to apply.
 *      arg     - (OUT)    output value
 * Returns:
 *      _SHR_E_XXX      Error occurred
 */
int
bcm_dnxf_stk_module_control_get(
    int unit,
    uint32 flags,
    bcm_module_t module,
    bcm_stk_module_control_t control,
    int *arg)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    DNXF_UNIT_LOCK_TAKE(unit);

    SHR_INVOKE_VERIFY_DNXC(bcm_dnxf_stk_module_control_get_verify(unit, module, arg));

    switch (control)
    {
        case bcmStkModuleAllReachableIgnore:
            SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL
                            (unit, mbcm_dnxf_stk_module_all_reachable_ignore_id_get, (unit, module, arg)));
            break;
        default:
            SHR_ERR_EXIT(SOC_E_PARAM, "Invalid control %d", control);
    }
exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnxf_stk_module_control_set
 * Purpose:
 *      Get the module ID of the local device
 * Parameters:
 *      unit    - (IN)     Unit number.
 *      flags   - (IN)     flags.
 *      module  - (IN)     for which module to apply the control.
 *      control - (IN)     which control to apply.
 *      arg     - (IN)     output value
 * Returns:
 *      _SHR_E_XXX      Error occurred
 */
int
bcm_dnxf_stk_module_control_set(
    int unit,
    uint32 flags,
    bcm_module_t module,
    bcm_stk_module_control_t control,
    int arg)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    DNXF_UNIT_LOCK_TAKE(unit);

    SHR_INVOKE_VERIFY_DNXC(bcm_dnxf_stk_module_control_verify(unit, module));

    switch (control)
    {
        case bcmStkModuleAllReachableIgnore:
            SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL
                            (unit, mbcm_dnxf_stk_module_all_reachable_ignore_id_set, (unit, module, arg)));
            break;
        default:
            SHR_ERR_EXIT(SOC_E_PARAM, "Invalid control %d", control);
    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

static int
bcm_dnxf_stk_module_max_verify(
    int unit,
    bcm_module_t module)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(bcm_dnxf_stk_module_control_verify(unit, module));

    if ((module % dnxf_data_fabric.topology.all_reachable_fap_group_size_get(unit) != 0)
        && (module != (dnxf_data_fabric.topology.nof_system_modid_get(unit) - 1)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid module id , must be divisible by %d \n",
                     dnxf_data_fabric.topology.all_reachable_fap_group_size_get(unit));
    }

    if (module == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid module id , can't be 0 \n");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the maximum FAP ID configured in the system
 *
 * \param [in] unit - Unit ID
 * \param [in] flags - flags (use 0)
 * \param [in] flags - 0 to set Max FAP-ID in system
 *                   - BCM_STK_MODULE_MAX_ALL_REACHABLE to set Max all-reachable FAP-ID
 * \return
 *   _SHR_E_NONE
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnxf_stk_module_max_get(
    int unit,
    uint32 flags,
    bcm_module_t * max_module)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    DNXF_UNIT_LOCK_TAKE(unit);

    SHR_NULL_CHECK(max_module, _SHR_E_PARAM, "max_module");

    if (flags & BCM_STK_MODULE_MAX_ALL_REACHABLE)
    {
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_stk_module_max_all_reachable_get, (unit, max_module)));
    }
    else
    {
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_stk_module_max_fap_get, (unit, max_module)));
    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the maximum FAP ID configured in the system
 *
 * \param [in] unit - Unit ID
 * \param [in] flags - 0 to set Max FAP-ID in system
 *                   - BCM_STK_MODULE_MAX_ALL_REACHABLE to set Max all-reachable FAP-ID
 * \param [out] max_module - Max FAP ID
 * \return
 *   _SHR_E_NONE
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnxf_stk_module_max_set(
    int unit,
    uint32 flags,
    bcm_module_t max_module)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    DNXF_UNIT_LOCK_TAKE(unit);

    SHR_INVOKE_VERIFY_DNXC(bcm_dnxf_stk_module_max_verify(unit, max_module));

    if (flags & BCM_STK_MODULE_MAX_ALL_REACHABLE)
    {
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_stk_module_max_all_reachable_set, (unit, max_module)));
    }
    else
    {
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_stk_module_max_fap_set, (unit, max_module)));
    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
