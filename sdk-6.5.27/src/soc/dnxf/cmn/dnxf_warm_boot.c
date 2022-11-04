/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * SOC DNXF warm_boot
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_INIT

#include <shared/bsl.h>
#include <soc/error.h>
#include <soc/scache.h>
#include <soc/sand/ha.h>
#include <soc/drv.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnxc/dnxc_ha.h>

#include <sal/compiler.h>

#ifdef BCM_DNXF_SUPPORT

#include <soc/dnxc/error.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_warm_boot.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_dev_init.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>

#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>

#define DNXF_VER(_ver) (_ver)

shr_error_e
soc_dnxf_ha_deinit(
    int unit)
{
#if defined(BCM_WARM_BOOT_SUPPORT) && !defined(__KERNEL__) && defined (LINUX)
    int rv;
    int stable_location;
    uint32 stable_flags;
#endif

    SHR_FUNC_INIT_VARS(unit);

#if defined(BCM_WARM_BOOT_SUPPORT) && !defined(__KERNEL__) && defined (LINUX)
    SHR_IF_ERR_EXIT(soc_stable_get(unit, &stable_location, &stable_flags));
    if (stable_location == 4)
    {
        rv = ha_destroy(unit);
        SHR_IF_ERR_CONT(rv);
    }
#endif

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_ha_init(
    int unit)
{
#if defined(BCM_WARM_BOOT_SUPPORT) && !defined(__KERNEL__) && defined (LINUX)
    int stable_location;
    uint32 stable_flags;
    uint32 stable_size;
#endif

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_WARM_BOOT_SUPPORT
#if !defined(__KERNEL__) && defined (LINUX)
    stable_size = dnxf_data_dev_init.ha.stable_size_get(unit);

    SHR_IF_ERR_EXIT(soc_stable_get(unit, &stable_location, &stable_flags));
    if (stable_location == 4)
    {
        char *ha_name = dnxf_data_dev_init.ha.stable_filename_get(unit)->val;

        SHR_IF_ERR_EXIT(ha_init(unit, ha_name, stable_size, SOC_WARM_BOOT(unit) ? 0 : 1));
    }
exit:
#endif
#endif
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_sw_state_alloc(
    int unit)
{
    int i;
    uint32 mc_table_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_multicast_table_size_get, (unit, &mc_table_size)));

    dnxf_state.init(unit);

    dnxf_state.info.active_core_bmp.alloc_bitmap(unit, dnxf_data_device.general.nof_cores_get(unit));
    dnxf_state.info.active_core_bmp.bit_range_set(unit, 0, dnxf_data_device.general.nof_cores_get(unit));

    dnxf_state.modid.local_map.alloc(unit, SOC_DNXF_MODID_LOCAL_NOF(unit));
    dnxf_state.modid.group_map.alloc(unit, SOC_DNXF_MODID_GROUP_NOF(unit));
    for (i = 0; i < SOC_DNXF_MODID_GROUP_NOF(unit); ++i)
    {
        dnxf_state.modid.group_map.raw.alloc_bitmap(unit, i, dnxf_data_fabric.topology.nof_system_modid_get(unit));
    }
    dnxf_state.modid.modid_to_group_map.alloc(unit, dnxf_data_fabric.topology.nof_system_modid_get(unit));
    dnxf_state.mc.id_map.alloc(unit, _shr_div32r(mc_table_size, 32));
    dnxf_state.lb.group_to_first_link.alloc(unit, dnxf_data_device.general.nof_cores_get(unit),
                                            dnxf_data_fabric.topology.nof_local_modid_get(unit));
    dnxf_state.port.cl72_conf.alloc(unit, dnxf_data_port.general.nof_links_get(unit));
    dnxf_state.port.phy_lane_config.alloc(unit, dnxf_data_port.general.nof_links_get(unit));
    dnxf_state.port.speed.alloc(unit, dnxf_data_port.general.nof_links_get(unit));
    dnxf_state.port.fec_type.alloc(unit, dnxf_data_port.general.nof_links_get(unit));
    if (dnxf_data_port.general.feature_get(unit, dnxf_data_port_general_is_connection_to_repeater_supported))
    {
        dnxf_state.port.is_connected_to_repeater.alloc_bitmap(unit, dnxf_data_port.general.nof_links_get(unit));
    }
    dnxf_state.port.isolation_status_stored.alloc(unit, dnxf_data_port.general.nof_links_get(unit));
    dnxf_state.port.orig_isolated_status.alloc(unit, dnxf_data_port.general.nof_links_get(unit));
    dnxf_state.intr.flags.alloc(unit, dnxf_data_device.interrupts.nof_interrupts_get(unit));
    dnxf_state.intr.storm_timed_count.alloc(unit, dnxf_data_device.interrupts.nof_interrupts_get(unit));
    dnxf_state.intr.storm_timed_period.alloc(unit, dnxf_data_device.interrupts.nof_interrupts_get(unit));
    dnxf_state.lane_map_db.serdes_map.lane2serdes.alloc(unit, dnxf_data_port.general.nof_links_get(unit));
    dnxf_state.lane_map_db.serdes_map.serdes2lane.alloc(unit, dnxf_data_port.general.nof_links_get(unit));
    dnxf_state.lane_map_db.link_to_fmac_lane_map.alloc(unit, dnxf_data_port.general.nof_links_get(unit));
    SHR_IF_ERR_EXIT(dnxf_state.lane_map_db.serdes_map.map_size.set(unit, dnxf_data_port.general.nof_links_get(unit)));
    SHR_IF_ERR_EXIT(dnxf_state.mc.nof_mc_groups_created.set(unit, 0));

    if (dnxf_data_port.general.feature_get(unit, dnxf_data_port_general_is_cable_swap_supported))
    {
        int nof_pm = dnxf_data_port.general.nof_pms_get(unit);
        dnxf_state.cable_swap.is_enable.alloc(unit, nof_pm);
        dnxf_state.cable_swap.is_master.alloc(unit, nof_pm);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_sw_state_init(
    int unit)
{
    uint32 warmboot = 0;
    uint32 sw_state_max_size = 0;

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_WARM_BOOT_SUPPORT

    warmboot = dnxf_data_dev_init.ha.warmboot_support_get(unit);
#endif

    dnxc_sw_state_is_warmboot_supported_set(unit, warmboot);
    sw_state_max_size = dnxf_data_dev_init.ha.sw_state_max_size_get(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_init(unit, warmboot, sw_state_max_size));

    if (!SOC_WARM_BOOT(unit))
    {

        SHR_IF_ERR_EXIT(soc_dnxf_sw_state_alloc(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_sw_state_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_deinit(unit));

exit:
    SHR_FUNC_EXIT;

}
#endif
