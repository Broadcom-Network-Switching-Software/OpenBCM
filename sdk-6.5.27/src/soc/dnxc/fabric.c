/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_INIT
#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <soc/dnxc/dnxc_port.h>
#include <soc/dnxc/fabric.h>
#include <soc/dnxc/error.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/drv.h>
#include <soc/sand/shrextend/shrextend_debug.h>

#ifdef BCM_ACCESS_SUPPORT
#include <soc/access/access.h>
#include <soc/access/auto_generated/common_enum.h>
#endif

#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_fabric.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#endif

#ifdef BCM_DNX_SUPPORT
#include <bcm_int/dnx/fabric/fabric_mesh.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#endif

int
soc_dnxc_fabric_mesh_topology_diag_get(
    int unit,
    soc_dnxc_fabric_mesh_topology_info_t * mesh_topology_info)
{
    SHR_FUNC_INIT_VARS(unit);
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_diag_mesh_topology_get, (unit, mesh_topology_info)));
    }
    else
#endif
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
        {
            mesh_topology_info->nof_diag = 1;
            SHR_IF_ERR_EXIT(dnx_fabric_mesh_topology_get(unit, mesh_topology_info->mesh_topology_diag));
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

shr_error_e
soc_dnxc_fabric_first_block_valid_get(
    int unit,
    int block_type,
    int *first_valid)
{
    int instance = 0;
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        while (TRUE)
        {
            int acccess_dis;
            SHR_IF_ERR_EXIT(access_block_instance_is_disabled(unit, block_type, instance, &acccess_dis));
            if (acccess_dis)
            {
                instance++;
            }
            else
            {
                break;
            }
        }
    }
#endif

    *first_valid = instance;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_fabric_fsrd_block_valid_set(
    int unit,
    int fsrd_block,
    int is_valid)
{
    int blk = 0;
    int *block_p;
    uint8 *block_valid_p;
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        SHR_IF_ERR_EXIT(access_valid_block_enable(unit, BLOCK_FSRD, fsrd_block, is_valid ? TRUE : FALSE));
    }
#endif

    SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, FSRD_BLOCK_INT, &block_p));
    block_p = &(block_p[fsrd_block]);
    blk = *block_p;
    SHR_IF_ERR_EXIT(soc_info_uint8_address_get(unit, BLOCK_VALID, &block_valid_p));
    block_valid_p = &(block_valid_p[blk]);
    *block_valid_p = is_valid ? 1 : 0;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_fabric_fmac_block_valid_set(
    int unit,
    int fmac_block,
    int is_valid)
{
    int blk = 0;
    int *block_p;
    uint8 *block_valid_p;
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        SHR_IF_ERR_EXIT(access_valid_block_enable(unit, BLOCK_FMAC, fmac_block, is_valid ? TRUE : FALSE));
    }
#endif

    SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, FMAC_BLOCK_INT, &block_p));
    block_p = &(block_p[fmac_block]);
    blk = *block_p;
    SHR_IF_ERR_EXIT(soc_info_uint8_address_get(unit, BLOCK_VALID, &block_valid_p));
    block_valid_p = &(block_valid_p[blk]);
    *block_valid_p = is_valid ? 1 : 0;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_fabric_fsrd_block_valid_get(
    int unit,
    int fsrd_block,
    int *is_valid)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        int is_disabled = 0;
        SHR_IF_ERR_EXIT(access_block_instance_is_disabled(unit, BLOCK_FSRD, fsrd_block, &is_disabled));
        *is_valid = !is_disabled;
    }
    else
#endif
    {
        int blk;
        int *fsrd_block_p;
        uint8 *block_valid_p;

        SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, FSRD_BLOCK_INT, &fsrd_block_p));
        fsrd_block_p = &(fsrd_block_p[fsrd_block]);
        blk = *fsrd_block_p;
        SHR_IF_ERR_EXIT(soc_info_uint8_address_get(unit, BLOCK_VALID, &block_valid_p));
        block_valid_p = &(block_valid_p[blk]);
        *is_valid = (*block_valid_p == 1) ? 1 : 0;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_fabric_fmac_block_valid_get(
    int unit,
    int fmac_block,
    int *is_valid)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        int is_disabled = 0;
        SHR_IF_ERR_EXIT(access_block_instance_is_disabled(unit, BLOCK_FMAC, fmac_block, &is_disabled));
        *is_valid = !is_disabled;
    }
    else
#endif
    {
        int blk;
        int *fmac_block_p;
        uint8 *block_valid_p;

        SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, FMAC_BLOCK_INT, &fmac_block_p));
        fmac_block_p = &(fmac_block_p[fmac_block]);
        blk = *fmac_block_p;
        SHR_IF_ERR_EXIT(soc_info_uint8_address_get(unit, BLOCK_VALID, &block_valid_p));
        block_valid_p = &(block_valid_p[blk]);
        *is_valid = (*block_valid_p == 1) ? 1 : 0;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_fabric_nof_links_in_fmac_get(
    int unit,
    int *nof_links_in_fmac)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        *nof_links_in_fmac = dnx_data_fabric.blocks.nof_links_in_fmac_get(unit);
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        *nof_links_in_fmac = dnxf_data_device.blocks.nof_links_in_fmac_get(unit);
#endif
    }

    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_fabric_supported_lanes_get(
    int unit,
    bcm_pbmp_t * supported_lanes)
{
    SHR_FUNC_INIT_VARS(unit);

    BCM_PBMP_CLEAR(*supported_lanes);

    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        SOC_PBMP_ASSIGN(*supported_lanes, dnx_data_fabric.links.general_get(unit)->supported_links);
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        SHR_IF_ERR_EXIT(soc_dnxf_drv_supported_lanes_get(unit, supported_lanes));
#endif
    }

#ifdef BCM_DNXF_SUPPORT
exit:
#endif
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_fabric_lane_to_logical_port_get(
    int unit,
    int lane,
    int *port)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_to_logical_get(unit, lane, port));
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        SHR_IF_ERR_EXIT(soc_dnxf_lane_map_db_fmac_lane_to_link_get(unit, lane, port));
#endif
    }

exit:
    SHR_FUNC_EXIT;
}
