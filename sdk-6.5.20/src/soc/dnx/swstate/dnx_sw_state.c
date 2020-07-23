


/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <soc/dnxc/swstate/sw_state_features.h>


#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>



#include <soc/dnxc/swstate/sw_state_defs.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/drv.h>    

#if defined(BCM_WARM_BOOT_SUPPORT) && !defined(__KERNEL__) && (defined(LINUX) || defined(UNIX))
#include <soc/ha.h>
#include <soc/scache.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dev_init.h>
#endif

#if defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_WARM_BOOT_API_TEST)
#include <soc/dnxc/dnxc_wb_test_utils.h>
#endif



#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL





#if defined(BCM_WARM_BOOT_SUPPORT) && !defined(__KERNEL__) && (defined(LINUX) || defined(UNIX))

static shr_error_e
soc_dnx_ha_init(
    int unit)
{
    int stable_location;
    uint32 stable_size;

    SHR_FUNC_INIT_VARS(unit);

    stable_size = dnx_data_dev_init.ha.stable_size_get(unit);

    if (stable_size == 0)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "stable_size - must be defined in the configuration file");
    }
    
    stable_location = dnx_data_dev_init.ha.stable_location_get(unit);
    if (stable_location == 4)
    {
        char *ha_name = dnx_data_dev_init.ha.stable_filename_get(unit)->val;
        
        SHR_IF_ERR_EXIT(ha_init(unit, ha_name, stable_size, SOC_WARM_BOOT(unit) ? 0 : 1));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
soc_dnx_ha_deinit(
    int unit)
{
    int rv;
    int stable_location;

    SHR_FUNC_INIT_VARS(unit);

    stable_location = dnx_data_dev_init.ha.stable_location_get(unit);
    if (stable_location == 4)
    {
        rv = ha_destroy(unit);
        SHR_IF_ERR_CONT(rv);
    }

    
    SHR_EXIT();
exit:

    SHR_FUNC_EXIT;
}

#endif


int
dnx_sw_state_init(
    int unit)
{
    uint32 warmboot = 0;
    uint32 sw_state_max_size = 0;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    
    warmboot = dnx_data_dev_init.ha.warmboot_support_get(unit);
#endif 
    dnxc_sw_state_is_warmboot_supported_set(unit, warmboot);
    sw_state_max_size = dnx_data_dev_init.ha.sw_state_max_size_get(unit);

#if defined(BCM_WARM_BOOT_SUPPORT) && !defined(__KERNEL__) && (defined(LINUX) || defined(UNIX))
    SHR_IF_ERR_EXIT(soc_dnx_ha_init(unit));
#endif
    SHR_IF_ERR_EXIT(dnxc_sw_state_init(unit, warmboot, sw_state_max_size));

    DNXC_SW_STATE_FUNC_RETURN;
}

int
dnx_sw_state_deinit(
    int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnxc_sw_state_deinit(unit));
#if defined(BCM_WARM_BOOT_SUPPORT) && !defined(__KERNEL__) && (defined(LINUX) || defined(UNIX))
    SHR_IF_ERR_EXIT(soc_dnx_ha_deinit(unit));
#endif

    DNXC_SW_STATE_FUNC_RETURN;
}
#undef _ERR_MSG_MODULE_NAME
