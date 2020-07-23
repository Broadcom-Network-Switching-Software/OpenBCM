/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC
#include <shared/bsl.h>
#include <soc/dnxc/dnxc_fabric_source_routed_cell.h>
#include <shared/shrextend/shrextend_debug.h>


shr_error_e
soc_dnxc_actual_entity_value(
    int unit,
    dnxc_fabric_device_type_t device_entity,
    soc_dnxc_device_type_actual_value_t * actual_entity)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (device_entity)
    {
        case dnxcFabricDeviceTypeFE1:
        {
            *actual_entity = soc_dnxc_device_type_actual_value_FE1;
            break;
        }
        case dnxcFabricDeviceTypeFE2:
        {
            *actual_entity = soc_dnxc_device_type_actual_value_FE2;
            break;
        }
        case dnxcFabricDeviceTypeFE3:
        {
            *actual_entity = soc_dnxc_device_type_actual_value_FE3;
            break;
        }
        case dnxcFabricDeviceTypeFAP:
        {
            *actual_entity = soc_dnxc_device_type_actual_value_FAP;
            break;
        }
        case dnxcFabricDeviceTypeFOP:
        {
            *actual_entity = soc_dnxc_device_type_actual_value_FOP;
            break;
        }
        case dnxcFabricDeviceTypeFIP:
        {
            *actual_entity = soc_dnxc_device_type_actual_value_FIP;
            break;
        }
        case dnxcFabricDeviceTypeUnknown:
        case dnxcFabricDeviceTypeFE13:
        {
            
            SHR_ERR_EXIT(_SHR_E_PARAM, "device_entity %d can't be FE13 or unknown", device_entity);
            break;
        }
        default:
        {
            
            SHR_ERR_EXIT(_SHR_E_PARAM, "device_entity %d is out-of-range", device_entity);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
