/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC DFE FABRIC
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC

#ifdef BCM_DFE_SUPPORT
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <bcm/fabric.h>
#include <soc/defs.h>
#include <soc/error.h>
#include <bcm/error.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_fabric.h>
#include <soc/dfe/cmn/mbcm.h>

#include <soc/dfe/fe1600/fe1600_defs.h>
#include <soc/dfe/fe1600/fe1600_fabric_flow_control.h>
#include <soc/dfe/fe1600/fe1600_fabric_links.h>
#include <soc/dfe/fe1600/fe1600_fabric_multicast.h>
#include <soc/dfe/fe1600/fe1600_fabric_status.h>
#include <soc/dfe/fe1600/fe1600_fabric_topology.h>
#include <bcm_int/control.h>


soc_error_t
soc_dfe_fabric_link_status_all_get(int unit, int links_array_max_size, uint32* link_status, uint32* errored_token_count, int* links_array_count)
{
    int rc, nof_links = 0;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }
    
    nof_links = SOC_DFE_DEFS_GET(unit, nof_links);
    if(links_array_max_size < nof_links) {
         SOCDNX_EXIT_WITH_ERR(SOC_E_FULL, (_BSL_SOCDNX_MSG("Array is too small")));
    }

    SOCDNX_NULL_CHECK(link_status);
    SOCDNX_NULL_CHECK(errored_token_count);
    SOCDNX_NULL_CHECK(links_array_count);

    DFE_UNIT_LOCK_TAKE_SOCDNX(unit);

    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_link_status_all_get,(unit, links_array_max_size, link_status, errored_token_count, links_array_count));
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    DFE_UNIT_LOCK_RELEASE_SOCDNX(unit); 
    SOCDNX_FUNC_RETURN;
}

#endif 

#undef _ERR_MSG_MODULE_NAME

