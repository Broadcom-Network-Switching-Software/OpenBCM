/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_INIT
#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <soc/dnxc/fabric.h>
#include <soc/dnxc/error.h>
#include <shared/shrextend/shrextend_debug.h>

#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_fabric.h>
#include <soc/dnxf/cmn/mbcm.h>
#endif

#ifdef BCM_DNX_SUPPORT
#include <bcm_int/dnx/fabric/fabric_mesh.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#endif

int
soc_dnxc_fabric_mesh_topology_diag_get(
    int unit,
    soc_dnxc_fabric_mesh_topology_diag_t * mesh_topology_diag)
{
    SHR_FUNC_INIT_VARS(unit);
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {

        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_diag_mesh_topology_get, (unit, mesh_topology_diag)));
    }
    else
#endif 
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
        {
            SHR_IF_ERR_EXIT(dnx_fabric_mesh_topology_get(unit, mesh_topology_diag));
        }
    }
    else
#endif 
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "feature unavail");
    }

exit:
    SHR_FUNC_EXIT;
}
