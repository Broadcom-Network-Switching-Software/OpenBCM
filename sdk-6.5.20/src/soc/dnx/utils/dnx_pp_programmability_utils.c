/*
 * $Id: dnx_pp_programmability_utils.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOC_COMMON

#include <sal/types.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/drv.h>


shr_error_e
dnx_pp_prgm_device_image_name_get(
    int unit,
    char **device_image_name)
{
    SHR_FUNC_INIT_VARS(unit);

    *device_image_name = dnx_data_pp.application.device_image_get(unit)->name;

    SHR_FUNC_EXIT;
}


shr_error_e
dnx_pp_prgm_device_image_name_legacy_get(
    int unit,
    char **device_image_name)
{
    SHR_FUNC_INIT_VARS(unit);

    *device_image_name = soc_property_get_str(unit, spn_PROGRAMMABILITY_IMAGE_NAME);
    if (!(*device_image_name))
    {
        
        *device_image_name = "standard_1";
    }

    SHR_FUNC_EXIT;
}


shr_error_e
dnx_pp_prgm_default_image_check(
    int unit,
    uint8 *is_default)
{
    SHR_FUNC_INIT_VARS(unit);

    *is_default = sal_strcmp(dnx_data_pp.application.device_image_get(unit)->name,
                             dnx_data_pp.application.device_image_get(unit)->standard_image_name) ? FALSE : TRUE;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_pp_prgm_current_image_check(
    int unit,
    char *image_to_check,
    uint8 *is_image)
{
    SHR_FUNC_INIT_VARS(unit);

    *is_image = sal_strcmp(dnx_data_pp.application.device_image_get(unit)->name, image_to_check) ? FALSE : TRUE;

    SHR_FUNC_EXIT;
}
