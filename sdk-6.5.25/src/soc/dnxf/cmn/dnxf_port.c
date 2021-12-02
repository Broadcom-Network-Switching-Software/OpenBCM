
/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * SOC DNXF PORT
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC

#ifdef BCM_DNXF_SUPPORT
#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/error.h>
#include <soc/drv.h>

#include <bcm_int/dnxf_dispatch.h>

#include <soc/dnxc/error.h>

#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_port.h>
#include <soc/dnxf/cmn/mbcm.h>

static int
soc_dnxf_is_loopback_type_local_get(
    int unit,
    portmod_loopback_mode_t loopback_type,
    uint32_t * is_local)
{
    SHR_FUNC_INIT_VARS(unit);

    *is_local = FALSE;

    switch (loopback_type)
    {
        case portmodLoopbackMacOuter:
        case portmodLoopbackMacCore:
        case portmodLoopbackMacPCS:
        case portmodLoopbackMacAsyncFifo:
        case portmodLoopbackPhyGloopPCS:
        case portmodLoopbackPhyGloopPMD:
            *is_local = TRUE;
            break;
        default:
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_port_loopback_set(
    int unit,
    soc_port_t port,
    portmod_loopback_mode_t loopback)
{
    int rc;
    portmod_loopback_mode_t lb_start;
    uint32 cl72_start;
    int phy_lane_config_start;
    uint32 is_lb_start_local = FALSE;
    uint32 is_lb_local = FALSE;
    bcm_port_resource_t resource;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit) || !SOC_IS_DNXF(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }

    DNXF_LINK_INPUT_CHECK_DNXC(unit, port);

    DNXF_UNIT_LOCK_TAKE(unit);

    SHR_IF_ERR_EXIT(soc_dnxf_port_loopback_get(unit, port, &lb_start));

    SHR_IF_ERR_EXIT(soc_dnxf_is_loopback_type_local_get(unit, lb_start, &is_lb_start_local));
    SHR_IF_ERR_EXIT(soc_dnxf_is_loopback_type_local_get(unit, loopback, &is_lb_local));

    if ((is_lb_start_local == FALSE) && (is_lb_local == TRUE))
    {
        SHR_IF_ERR_EXIT(bcm_dnxf_port_resource_get(unit, port, &resource));
        cl72_start = resource.link_training;
        rc = dnxf_state.port.cl72_conf.set(unit, port, cl72_start);
        SHR_IF_ERR_EXIT(rc);

        if (cl72_start == 1)
        {
            phy_lane_config_start = resource.phy_lane_config;
            rc = dnxf_state.port.phy_lane_config.set(unit, port, phy_lane_config_start);
            SHR_IF_ERR_EXIT(rc);

            resource.link_training = 0;
            resource.phy_lane_config = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
            SHR_IF_ERR_EXIT(bcm_dnxf_port_resource_default_get(unit, port, 0, &resource));

            SHR_IF_ERR_EXIT(bcm_dnxf_port_resource_set(unit, port, &resource));
        }
    }

    if (loopback == portmodLoopbackMacRloop)
    {
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL
                        (unit, mbcm_dnxf_fabric_links_link_integrity_down_set, (unit, port, TRUE)));
    }

    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_loopback_set, (unit, port, loopback));
    SHR_IF_ERR_EXIT(rc);

    if (loopback != portmodLoopbackMacRloop)
    {
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL
                        (unit, mbcm_dnxf_fabric_links_link_integrity_down_set, (unit, port, FALSE)));
    }

    if ((is_lb_start_local == TRUE) && (is_lb_local == FALSE))
    {
        rc = dnxf_state.port.cl72_conf.get(unit, port, &cl72_start);
        SHR_IF_ERR_EXIT(rc);

        if (cl72_start == 1)
        {
            rc = dnxf_state.port.phy_lane_config.get(unit, port, &phy_lane_config_start);
            SHR_IF_ERR_EXIT(rc);

            SHR_IF_ERR_EXIT(bcm_dnxf_port_resource_get(unit, port, &resource));
            resource.link_training = 1;
            resource.phy_lane_config = phy_lane_config_start;
            SHR_IF_ERR_EXIT(bcm_dnxf_port_resource_set(unit, port, &resource));
        }
    }

    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_burst_control_set, (unit, port, loopback));
    SHR_IF_ERR_EXIT(rc);

    if (loopback == portmodLoopbackMacRloop)
    {
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_link_retimer_calibrate, (unit, port, port)));
    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_port_loopback_get(
    int unit,
    soc_port_t port,
    portmod_loopback_mode_t * loopback)
{
    int rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit) || !SOC_IS_DNXF(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }

    DNXF_LINK_INPUT_CHECK_DNXC(unit, port);
    SHR_NULL_CHECK(loopback, _SHR_E_PARAM, "loopback");

    DNXF_UNIT_LOCK_TAKE(unit);

    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_loopback_get, (unit, port, loopback));
    SHR_IF_ERR_EXIT(rc);

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;

}

shr_error_e
soc_dnxf_port_disable_and_save_enable_state(
    int unit,
    soc_port_t port,
    uint32 flags,
    int *orig_enabled)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(soc_dnxc_port_enable_get(unit, port, flags, orig_enabled));

    if (*orig_enabled)
    {
        SHR_IF_ERR_EXIT(soc_dnxc_port_enable_set(unit, port, flags, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_port_restore_enable_state(
    int unit,
    soc_port_t port,
    uint32 flags,
    int orig_enabled)
{
    SHR_FUNC_INIT_VARS(unit);

    if (orig_enabled)
    {
        SHR_IF_ERR_EXIT(soc_dnxc_port_enable_set(unit, port, flags, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_port_enable_get(
    int unit,
    soc_port_t port,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxc_port_enable_get(unit, port, 0, enable));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_port_cable_swap_info_set(
    int unit)
{
    int pm_id, val;
    int device_mode;
    SHR_FUNC_INIT_VARS(unit);

    for (pm_id = 0; pm_id < dnxf_data_port.general.nof_pms_get(unit); ++pm_id)
    {

        val = dnxf_data_port.static_add.cable_swap_info_get(unit, pm_id)->enable;
        SHR_IF_ERR_EXIT(dnxf_state.cable_swap.is_enable.set(unit, pm_id, val));

        val = dnxf_data_port.static_add.cable_swap_info_get(unit, pm_id)->master;
        if (val == -1)
        {

            device_mode = dnxf_data_fabric.general.device_mode_get(unit);

            switch (device_mode)
            {
                case soc_dnxf_fabric_device_mode_multi_stage_fe13:
                    val = 0;
                    break;
                case soc_dnxf_fabric_device_mode_single_stage_fe2:
                    val = 0;
                    break;
                case soc_dnxf_fabric_device_mode_multi_stage_fe2:
                    val = 1;
                    break;
            }
        }
        SHR_IF_ERR_EXIT(dnxf_state.cable_swap.is_master.set(unit, pm_id, val));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_port_cable_swap_set(
    int unit,
    soc_port_t port)
{
    int pm_id;
    soc_dnxc_port_cable_swap_config_t cable_swap_config;
    SHR_FUNC_INIT_VARS(unit);

    pm_id = port / dnxf_data_device.blocks.nof_links_in_fsrd_get(unit);

    SHR_IF_ERR_EXIT(dnxf_state.cable_swap.is_enable.get(unit, pm_id, (int *) &cable_swap_config.enable));
    SHR_IF_ERR_EXIT(dnxf_state.cable_swap.is_master.get(unit, pm_id, (int *) &cable_swap_config.is_master));

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_cable_swap_set, (unit, port, &cable_swap_config)));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_port_pre_enable_isolate_set(
    int unit,
    soc_port_t port)
{
    int original_isolation_status, isolation_status_stored;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxf_state.port.isolation_status_stored.get(unit, port, &isolation_status_stored));

    if (isolation_status_stored == 0)
    {

        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL
                        (unit, mbcm_dnxf_fabric_links_isolate_get,
                         (unit, port, (soc_dnxc_isolation_status_t *) & original_isolation_status)));
        SHR_IF_ERR_EXIT(dnxf_state.port.orig_isolated_status.set(unit, port, original_isolation_status));

        SHR_IF_ERR_EXIT(dnxf_state.port.isolation_status_stored.set(unit, port, 1));
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_links_isolate_set, (unit, port, 1)));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_port_post_enable_isolate_set(
    int unit,
    soc_port_t port)
{
    int original_isolation_status, isolation_status_stored;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxf_state.port.isolation_status_stored.get(unit, port, &isolation_status_stored));

    if (isolation_status_stored == 1)
    {
        SHR_IF_ERR_EXIT(dnxf_state.port.orig_isolated_status.get(unit, port, &original_isolation_status));
        if (original_isolation_status == 0 && !SOC_HW_RESET(unit))
        {

            sal_usleep(4000);
        }
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL
                        (unit, mbcm_dnxf_fabric_links_isolate_set, (unit, port, original_isolation_status)));
        SHR_IF_ERR_EXIT(dnxf_state.port.isolation_status_stored.set(unit, port, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_port_dynamic_port_update(
    int unit,
    int port,
    int add_remove,
    soc_dnxf_port_update_type_t type)
{
    soc_info_t *si;
    int fmac_lane;
    bcm_pbmp_t supported_lanes;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxf_drv_supported_lanes_get(unit, &supported_lanes));

    SHR_IF_ERR_EXIT(soc_dnxf_lane_map_db_link_to_fmac_lane_get(unit, port, &fmac_lane));

    if (fmac_lane == -1 || !(PBMP_MEMBER(supported_lanes, fmac_lane)))
    {
        if (((add_remove == SOC_DNXF_PBMP_PORT_REMOVE) && (type == soc_dnxf_port_update_type_sfi_disabled)) ||
            ((add_remove == SOC_DNXF_PBMP_PORT_ADD) && (type == soc_dnxf_port_update_type_sfi)))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Fabric link %d is disabled for device %s", port, soc_dev_name(unit));
        }
    }

    si = &SOC_INFO(unit);

    if (add_remove == SOC_DNXF_PBMP_PORT_ADD)
    {
        switch (type)
        {
            case soc_dnxf_port_update_type_sfi:
                DNXF_ADD_DYNAMIC_PORT(sfi, port);
                break;
            case soc_dnxf_port_update_type_port:
                DNXF_ADD_DYNAMIC_PORT(port, port);
                break;
            case soc_dnxf_port_update_type_all:
                DNXF_ADD_DYNAMIC_PORT(all, port);
                break;
            case soc_dnxf_port_update_type_sfi_disabled:
                DNXF_ADD_DYNAMIC_DISABLED_PORT(sfi, port);
                break;
            case soc_dnxf_port_update_type_port_disabled:
                DNXF_ADD_DYNAMIC_DISABLED_PORT(port, port);
                break;
            case soc_dnxf_port_update_type_all_disabled:
                DNXF_ADD_DYNAMIC_DISABLED_PORT(all, port);
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown port update type");
                break;
        }
    }
    else if (add_remove == SOC_DNXF_PBMP_PORT_REMOVE)
    {
        switch (type)
        {
            case soc_dnxf_port_update_type_sfi:
                DNXF_REMOVE_DYNAMIC_PORT(sfi, port);
                break;
            case soc_dnxf_port_update_type_port:
                DNXF_REMOVE_DYNAMIC_PORT(port, port);
                break;
            case soc_dnxf_port_update_type_all:
                DNXF_REMOVE_DYNAMIC_PORT(all, port);
                break;
            case soc_dnxf_port_update_type_sfi_disabled:
                DNXF_REMOVE_DYNAMIC_DISABLED_PORT(sfi, port);
                break;
            case soc_dnxf_port_update_type_port_disabled:
                DNXF_REMOVE_DYNAMIC_DISABLED_PORT(port, port);
                break;
            case soc_dnxf_port_update_type_all_disabled:
                DNXF_REMOVE_DYNAMIC_DISABLED_PORT(all, port);
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown port update type");
                break;
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown control type %d provided", add_remove);
    }
exit:
    SHR_FUNC_EXIT;

}

shr_error_e
soc_dnxf_port_max_port_get(
    int unit,
    int *max_port)
{
    const SHR_BITDCL *active_cores;
    int nof_cores;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxf_state.info.active_core_bmp.get(unit, &active_cores));

    SHR_BITCOUNT_RANGE(active_cores, nof_cores, 0, dnxf_data_device.general.nof_cores_get(unit));

    *max_port = dnxf_data_device.blocks.nof_links_in_rtp_get(unit) * nof_cores;
exit:
    SHR_FUNC_EXIT;
}
#endif

#undef BSL_LOG_MODULE
