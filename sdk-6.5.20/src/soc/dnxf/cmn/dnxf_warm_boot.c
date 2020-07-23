/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
#include <soc/ha.h>
#include <soc/drv.h>

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
soc_dnxf_warm_boot_deinit(int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    

    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_ha_deinit(int unit)
{
#if defined(BCM_WARM_BOOT_SUPPORT) && !defined(__KERNEL__) && defined (LINUX)
    int rv;
    int stable_location;
    uint32 stable_flags;
#endif

    SHR_FUNC_INIT_VARS(unit);
    
#if defined(BCM_WARM_BOOT_SUPPORT) && !defined(__KERNEL__) && defined (LINUX)
    SHR_IF_ERR_EXIT(soc_stable_get(unit,&stable_location,&stable_flags));
    if (stable_location == 4) {
        rv = ha_destroy(unit);
        SHR_IF_ERR_CONT(rv);
    }
#endif

    
    SHR_EXIT();
exit: 
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_ha_init(int unit)
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

    
    SHR_IF_ERR_EXIT(soc_stable_get(unit,&stable_location,&stable_flags));
    if (stable_location == 4) {
        char *ha_name =   dnxf_data_dev_init.ha.stable_filename_get(unit)->val;
        
        SHR_IF_ERR_EXIT(ha_init(unit, ha_name, stable_size, SOC_WARM_BOOT(unit) ? 0 : 1));
    }
exit: 
#endif
#endif
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_warm_boot_init(int unit)
{
    int rv;
    uint32 mc_table_size;

    SHR_FUNC_INIT_VARS(unit);

if (!SOC_WARM_BOOT(unit)) {
    rv = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_multicast_table_size_get,(unit, &mc_table_size));
    SHR_IF_ERR_EXIT(rv);
    dnxf_state.init(unit);
    dnxf_state.modid.local_map.alloc(unit, SOC_DNXF_MODID_LOCAL_NOF(unit));
    dnxf_state.modid.group_map.alloc(unit, SOC_DNXF_MODID_GROUP_NOF(unit));
    dnxf_state.mc.id_map.alloc(unit, _shr_div32r(mc_table_size,32));
    dnxf_state.lb.group_to_first_link.alloc(unit, dnxf_data_fabric.topology.nof_local_modid_get(unit));
    dnxf_state.port.cl72_conf.alloc(unit, dnxf_data_port.general.nof_links_get(unit));
    dnxf_state.port.phy_lane_config.alloc(unit, dnxf_data_port.general.nof_links_get(unit));
    dnxf_state.port.is_connected_to_repeater.alloc_bitmap(unit, dnxf_data_port.general.nof_links_get(unit));
    dnxf_state.port.isolation_status_stored.alloc(unit, dnxf_data_port.general.nof_links_get(unit));
    dnxf_state.port.orig_isolated_status.alloc(unit, dnxf_data_port.general.nof_links_get(unit));
    dnxf_state.intr.flags.alloc(unit, dnxf_data_device.interrupts.nof_interrupts_get(unit));
    dnxf_state.intr.storm_timed_count.alloc(unit, dnxf_data_device.interrupts.nof_interrupts_get(unit));
    dnxf_state.intr.storm_timed_period.alloc(unit, dnxf_data_device.interrupts.nof_interrupts_get(unit));
    dnxf_state.lane_map_db.lane2serdes.alloc(unit, dnxf_data_port.general.nof_links_get(unit));
    dnxf_state.lane_map_db.serdes2lane.alloc(unit, dnxf_data_port.general.nof_links_get(unit));
    SHR_IF_ERR_EXIT(dnxf_state.lane_map_db.map_size.set(unit, dnxf_data_port.general.nof_links_get(unit)));
    SHR_IF_ERR_EXIT(dnxf_state.mc.nof_mc_groups_created.set(unit, 0));
}
    
exit: 
    SHR_FUNC_EXIT;
}


#endif 
