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
#ifdef BCM_CMICX_SUPPORT
#include <soc/cmicx.h>
#include <shared/cmicfw/iproc_m0ssq.h>
#else
#include <soc/cmicm.h>
#endif

#include <soc/mcm/driver.h>
#include <soc/error.h>
#include <soc/ipoll.h>
#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/dnxc/drv.h>
#include <soc/linkctrl.h>
#include <soc/led.h>

#include <sal/appl/sal.h>

#include <soc/sand/sand_mem.h>
#include <soc/sand/sand_aux_access.h>

#include <soc/dnxc/fabric.h>
#include <soc/dnxc/error.h>
#include <soc/dnxc/dnxc_intr.h>
#include <soc/dnxc/intr.h>
#include <soc/dnxc/dnxc_intr.h>
#include <soc/dnxc/dnxc_adapter_reg_access.h>
#include <soc/dnxc/dnxc_cmic.h>
#include <soc/dnxc/dnxc_mem.h>
#include <soc/dnxc/dnxc_time.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/cmn/dnxf_warm_boot.h>
#include <soc/dnxf/cmn/dnxf_port.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_dev_init.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>

#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>

#ifdef BCM_CMICX_SUPPORT
#include <shared/cmicfw/cmicx_link.h>
#define CMICX_HW_LINKSCAN 1
#endif

#define SOC_DNXF_DEFAULT_MDIO_DIVISOR    (20)

sal_mutex_t _soc_dnxf_lock[BCM_MAX_NUM_UNITS];

soc_interrupt_fn_t dnxf_intr_fn = soc_intr;

int
soc_dnxf_tbl_is_dynamic(
    int unit,
    soc_mem_t mem)
{
    return MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_tbl_is_dynamic, (unit, mem));
}

void
soc_dnxf_tbl_mark_cachable(
    int unit)
{
    soc_mem_t mem;
    const dnxc_data_table_info_t *table_info;
    const dnxf_data_dev_init_shadow_uncacheable_mem_t *uncache_mem;
    int mem_index;

    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        if (!SOC_MEM_IS_VALID(unit, mem))
        {
            continue;
        }

        if (soc_mem_is_readonly(unit, mem) || soc_mem_is_writeonly(unit, mem) || soc_mem_is_signal(unit, mem))
        {
            continue;
        }

        if (sal_strnstr(SOC_MEM_NAME(unit, mem), "BRDC_", 20) != NULL)
        {
            continue;
        }
        SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_CACHABLE;
    }

    table_info = dnxf_data_dev_init.shadow.uncacheable_mem_info_get(unit);

    for (mem_index = 0; mem_index < table_info->key_size[0]; mem_index++)
    {
        uncache_mem = dnxf_data_dev_init.shadow.uncacheable_mem_get(unit, mem_index);
        if (SOC_MEM_IS_VALID(unit, uncache_mem->mem))
        {
            SOC_MEM_INFO(unit, uncache_mem->mem).flags &= ~SOC_MEM_FLAG_CACHABLE;
            LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, "Skip caching %s \n"), SOC_MEM_NAME(unit, uncache_mem->mem)));
        }
    }
}

extern soc_controlled_counter_t soc_dnxf_controlled_counter[];

int
soc_dnxf_misc_init(
    int unit)
{
    return _SHR_E_NONE;
}

int
soc_dnxf_mmu_init(
    int unit)
{
    return _SHR_E_NONE;
}

soc_functions_t soc_dnxf_drv_funs = {
    soc_dnxf_misc_init,
    soc_dnxf_mmu_init,
    NULL,
    NULL,
    NULL,
};

STATIC void
soc_dnxf_soc_properties_control_set(
    int unit)
{
    soc_sand_access_conf_get(unit);
}

STATIC int
soc_dnxf_info_soc_properties(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    soc_dnxf_soc_properties_control_set(unit);

    SHR_FUNC_EXIT;
}

int
soc_dnxf_drv_soc_property_serdes_qrtt_read(
    int unit)
{
    soc_pbmp_t pbmp_disabled_fmac_lanes;
    soc_pbmp_t supported_lanes;
    int enabled_pbmp_control, disabled_pbmp_control;
    int quad, quad_inner_link, port, rc, fmac_lane;
    uint32 quad_active;
    int nof_links_in_fmac = dnxf_data_device.blocks.nof_links_in_fmac_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    SOC_PBMP_CLEAR(pbmp_disabled_fmac_lanes);
    for (quad = 0; quad < dnxf_data_port.general.nof_links_get(unit) / nof_links_in_fmac; quad++)
    {

        quad_active = dnxf_data_port.static_add.quad_info_get(unit, quad)->quad_enable;

        if (!quad_active)
        {
            for (quad_inner_link = 0; quad_inner_link < nof_links_in_fmac; quad_inner_link++)
            {
                fmac_lane = quad * nof_links_in_fmac + quad_inner_link;
                SOC_PBMP_PORT_ADD(pbmp_disabled_fmac_lanes, fmac_lane);
            }
        }
    }

    SHR_IF_ERR_EXIT(soc_dnxf_drv_supported_lanes_get(unit, &supported_lanes));

    for (port = 0; port < dnxf_data_port.general.nof_links_get(unit); port++)
    {

        enabled_pbmp_control = SOC_DNXF_PBMP_PORT_ADD;
        disabled_pbmp_control = SOC_DNXF_PBMP_PORT_REMOVE;

        if (dnxf_data_port.lane_map.feature_get(unit, dnxf_data_port_lane_map_is_flexible_link_mapping_supported))
        {
            SHR_IF_ERR_EXIT(soc_dnxf_lane_map_db_link_to_fmac_lane_get(unit, port, &fmac_lane));
        }
        else
        {

            fmac_lane = port;
        }

        if ((fmac_lane == -1) || !(PBMP_MEMBER(supported_lanes, fmac_lane))
            || PBMP_MEMBER(pbmp_disabled_fmac_lanes, fmac_lane))
        {
            enabled_pbmp_control = SOC_DNXF_PBMP_PORT_REMOVE;
            disabled_pbmp_control = SOC_DNXF_PBMP_PORT_ADD;
        }

        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update,
                                   (unit, port, enabled_pbmp_control, soc_dnxf_port_update_type_sfi));
        SHR_IF_ERR_EXIT(rc);

        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update,
                                   (unit, port, enabled_pbmp_control, soc_dnxf_port_update_type_port));
        SHR_IF_ERR_EXIT(rc);

        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update,
                                   (unit, port, enabled_pbmp_control, soc_dnxf_port_update_type_all));
        SHR_IF_ERR_EXIT(rc);

        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update,
                                   (unit, port, disabled_pbmp_control, soc_dnxf_port_update_type_sfi_disabled));
        SHR_IF_ERR_EXIT(rc);

        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update,
                                   (unit, port, disabled_pbmp_control, soc_dnxf_port_update_type_port_disabled));
        SHR_IF_ERR_EXIT(rc);

        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update,
                                   (unit, port, disabled_pbmp_control, soc_dnxf_port_update_type_all_disabled));
        SHR_IF_ERR_EXIT(rc);
    }

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_control_init(
    int unit)
{
    uint16 dev_id;
    uint8 rev_id;
    soc_dnxf_control_t *dnxf = NULL;
    SHR_FUNC_INIT_VARS(unit);

    dnxf = SOC_DNXF_CONTROL(unit);
    if (dnxf == NULL)
    {
        dnxf = (soc_dnxf_control_t *) sal_alloc(sizeof(soc_dnxf_control_t), "soc_dnxf_control");
        if (dnxf == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate soc_dnxf_control");
        }
        sal_memset(dnxf, 0, sizeof(soc_dnxf_control_t));

        SOC_CONTROL(unit)->drv = dnxf;
    }

    soc_cm_get_id(unit, &dev_id, &rev_id);
    SHR_IF_ERR_CONT(soc_dnxf_info_config(unit, dev_id));

exit:
    SHR_FUNC_EXIT;
}

int
dnxf_tbl_mem_cache_mem_set(
    int unit,
    soc_mem_t mem,
    void *en)
{
    int rc;
    int enable = *(int *) en;

    SHR_FUNC_INIT_VARS(unit);

    SOC_MEM_ALIAS_TO_ORIG(unit, mem);
    if (!SOC_MEM_IS_VALID(unit, mem) || !soc_mem_is_cachable(unit, mem))
    {
        return _SHR_E_NONE;
    }

    if ((SOC_MEM_INFO(unit, mem).blocks | SOC_MEM_INFO(unit, mem).blocks_hi) != 0)
    {

        rc = soc_mem_cache_set(unit, mem, COPYNO_ALL, enable);
        SHR_IF_ERR_EXIT(rc);
    }

exit:
    SHR_FUNC_EXIT;

}

int
dnxf_tbl_mem_cache_enable_parity_tbl(
    int unit,
    soc_mem_t mem,
    void *en)
{
    int rc = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    if (SOC_MEM_FIELD_VALID(unit, mem, PARITYf))
        rc = dnxf_tbl_mem_cache_mem_set(unit, mem, en);

    LOG_INFO(BSL_LS_SOC_MEM, (BSL_META_U(unit, "parity memory %s cache\n"), SOC_MEM_NAME(unit, mem)));
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;

}

int
dnxf_tbl_mem_cache_enable_specific_tbl(
    int unit,
    soc_mem_t mem,
    void *en)
{
    int rc = SOC_E_NONE;
    int cache_enable = *(int *) en;

    SHR_FUNC_INIT_VARS(unit);

    SOC_MEM_ALIAS_TO_ORIG(unit, mem);
    if (soc_sand_mem_is_in_soc_property(unit, mem, ! !cache_enable))
    {
        if (!SOC_MEM_IS_VALID(unit, mem) || !soc_mem_is_cachable(unit, mem))
        {
            LOG_ERROR(BSL_LS_SOC_MEM,
                      (BSL_META("unit %d cache %s failed for %d (%s) rv %d\n"), unit,
                       cache_enable ? "enable" : "disable", mem, SOC_MEM_NAME(unit, mem), rc));
            SHR_IF_ERR_EXIT(SOC_E_UNAVAIL);
        }
        rc = dnxf_tbl_mem_cache_mem_set(unit, mem, en);
        SHR_IF_ERR_EXIT(rc);
    }

exit:
    SHR_FUNC_EXIT;
}

int
dnxf_tbl_mem_cache_enable_ecc_tbl(
    int unit,
    soc_mem_t mem,
    void *en)
{
    int rc = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    if (SOC_MEM_FIELD_VALID(unit, mem, ECCf))
        rc = dnxf_tbl_mem_cache_mem_set(unit, mem, en);

    LOG_INFO(BSL_LS_SOC_MEM, (BSL_META_U(unit, "ecc memory %s cache\n"), SOC_MEM_NAME(unit, mem)));
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_cache_enable_init(
    int unit)
{
    uint32 cache_enable = 1;
    int enable_all, enable_parity, enable_ecc;
    int enable_specific, disable_specific;
    SHR_FUNC_INIT_VARS(unit);

    enable_all = dnxf_data_dev_init.shadow.cache_enable_all_get(unit);
    enable_parity = dnxf_data_dev_init.shadow.cache_enable_parity_get(unit);
    enable_ecc = dnxf_data_dev_init.shadow.cache_enable_ecc_get(unit);
    enable_specific = dnxf_data_dev_init.shadow.cache_enable_specific_get(unit);
    disable_specific = dnxf_data_dev_init.shadow.cache_disable_specific_get(unit);

    if (enable_all)
    {
        if (soc_mem_iterate(unit, dnxf_tbl_mem_cache_mem_set, &cache_enable) < 0)
            LOG_ERROR(BSL_LS_APPL_SOCMEM,
                      (BSL_META_U(unit, "soc_dpe_cache_enable_init: unit %d all_cache enable failed\n"), unit));
    }

    if (enable_parity)
    {
        if (soc_mem_iterate(unit, dnxf_tbl_mem_cache_enable_parity_tbl, &cache_enable) < 0)
            LOG_ERROR(BSL_LS_APPL_SOCMEM,
                      (BSL_META_U(unit, "soc_dnxf_cache_enable_init: unit %d parity cache enable failed\n"), unit));
    }

    if (enable_ecc)
    {
        if (soc_mem_iterate(unit, dnxf_tbl_mem_cache_enable_ecc_tbl, &cache_enable) < 0)
            LOG_ERROR(BSL_LS_APPL_SOCMEM,
                      (BSL_META_U(unit, "soc_dnxf_cache_enable_init: unit %d ecc cache enable failed\n"), unit));
    }
    if (enable_specific)
    {
        if (soc_mem_iterate(unit, dnxf_tbl_mem_cache_enable_specific_tbl, &cache_enable) < 0)
            LOG_ERROR(BSL_LS_APPL_SOCMEM,
                      (BSL_META_U(unit, "soc_dpp_cache_enable_init: unit %d specific cache enable failed\n"), unit));
    }
    if (disable_specific)
    {
        cache_enable = 0;
        if (soc_mem_iterate(unit, dnxf_tbl_mem_cache_enable_specific_tbl, &cache_enable) < 0)
            LOG_ERROR(BSL_LS_APPL_SOCMEM,
                      (BSL_META_U(unit, "soc_dpp_cache_enable_init: unit %d specific cache disable failed\n"), unit));
    }

    SHR_FUNC_EXIT;
}
int
soc_dnxf_drv_link_to_block_mapping(
    int unit,
    bcm_port_t link,
    int *block_id,
    int *inner_link,
    int block_type)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_link_to_block_mapping,
                                          (unit, link, block_id, inner_link, block_type)));
exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_drv_block_to_link_mapping(
    int unit,
    int block_id,
    int inner_link,
    int block_type,
    bcm_port_t * link)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_block_to_link_mapping,
                                          (unit, block_id, inner_link, block_type, link)));
exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_soc_info_get(
    int unit)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    DNXF_UNIT_LOCK_TAKE(unit);

    SHR_IF_ERR_EXIT(soc_dnxf_info_soc_properties(unit));

    if (!dnxf_data_port.lane_map.feature_get(unit, dnxf_data_port_lane_map_is_flexible_link_mapping_supported))
    {
#ifdef VEDOR_DNX

#endif

        SHR_IF_ERR_EXIT(soc_dnxf_drv_soc_property_serdes_qrtt_read(unit));
    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

int
soc_dnxf_init_device_reset(
    int unit)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    DNXF_UNIT_LOCK_TAKE(unit);

    SOC_CMCS_NUM(unit) = CMIC_CMC_NUM_MAX;
    soc_endian_config(unit);

    SHR_IF_ERR_EXIT(soc_dnxc_perform_system_reset_if_needed(unit));

    soc_dnxf_tbl_mark_cachable(unit);

    SHR_IF_ERR_EXIT(soc_dnxf_cache_enable_init(unit));

    if (!SOC_WARM_BOOT(unit))
    {
        if (!SOC_IS_RELOADING(unit))
        {

            SOC_DNXF_DRV_INIT_LOG(unit, "Device Reset");
        }
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL_NO_ARGS(unit, mbcm_dnxf_reset_device));
    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

int
soc_dnxf_init_device_reset_deinit(
    int unit)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNXF_UNIT_LOCK_TAKE(unit);

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx) && !SAL_BOOT_PLISIM)
    {

        soc_iproc_m0_exit(unit);
    }
#endif

    soc_dnxf_free_cache_memory(unit);

    SHR_IF_ERR_CONT(soc_dnxf_control_init(unit));

exit:

    DNXF_UNIT_LOCK_RELEASE(unit);

    if (SOC_UNIT_NUM_VALID(unit))
    {
        SOC_DETACH(unit, 0);
    }

    SHR_FUNC_EXIT;
}

int
soc_dnxf_init_link_mapping(
    int unit)
{
    DNXF_UNIT_LOCK_INIT(unit);

    SHR_FUNC_INIT_VARS(unit);
    DNXF_UNIT_LOCK_TAKE(unit);

    if (!SOC_WARM_BOOT(unit))
    {
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL_NO_ARGS(unit, mbcm_dnxf_init_link_mapping));
    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

int
soc_dnxf_init_blocks_config(
    int unit)
{
    int rc = _SHR_E_NONE;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    DNXF_UNIT_LOCK_TAKE(unit);

    if (!SOC_WARM_BOOT(unit))
    {
        rc = MBCM_DNXF_DRIVER_CALL_NO_ARGS(unit, mbcm_dnxf_init_blocks_config);
        SHR_IF_ERR_EXIT(rc);
    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

int
soc_dnxf_counter_attach(
    int unit)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    DNXF_UNIT_LOCK_TAKE(unit);

    SHR_IF_ERR_EXIT(soc_counter_attach(unit));

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

int
soc_dnxf_counter_dettach(
    int unit)
{
    int rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    DNXF_UNIT_LOCK_TAKE(unit);

    rc = soc_counter_detach(unit);
    if (SOC_FAILURE(rc))
    {
        LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "Failed to detach counter\n")));
    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

int
soc_dnxf_interrupts_init(
    int unit)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    DNXF_UNIT_LOCK_TAKE(unit);

    if (!SOC_WARM_BOOT(unit))
    {

        SHR_IF_ERR_EXIT(soc_dnxc_ser_init(unit));
    }

    if (SOC_WARM_BOOT(unit))
    {
#ifdef BCM_WARM_BOOT_SUPPORT
        uint32 value;
        soc_control_t *soc;
        soc = SOC_CONTROL(unit);

        SHR_IF_ERR_EXIT(dnxf_state.intr.storm_nominal.get(unit, &value));
        soc->switch_event_nominal_storm = value;
#endif

    }

    SHR_IF_ERR_CONT(soc_dnxc_interrupts_init(unit));

    SHR_IF_ERR_EXIT(soc_dnxc_interrupt_event_init(unit));

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

int
soc_dnxf_interrupts_deinit(
    int unit)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    DNXF_UNIT_LOCK_TAKE(unit);

    SHR_IF_ERR_CONT(soc_dnxc_interrupt_event_deinit(unit));

    SHR_IF_ERR_CONT(soc_dnxc_interrupts_deinit(unit));

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

int
soc_dnxf_soc_init_done(
    int unit)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    DNXF_UNIT_LOCK_TAKE(unit);

    SOC_FLAGS_SET(unit, SOC_FLAGS_GET(unit) | SOC_F_INITED);

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

int
soc_dnxf_soc_init_done_deinit(
    int unit)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "invalid unit");
    }

    DNXF_UNIT_LOCK_TAKE(unit);

    SOC_FLAGS_SET(unit, SOC_FLAGS_GET(unit) & ~SOC_F_INITED);

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

static int
soc_dnxf_shutdown_set(
    int unit,
    soc_pbmp_t active_links,
    int shutdown,
    int isolate_device)
{
    soc_port_t port;
    SHR_FUNC_INIT_VARS(unit);

    if (shutdown)
    {

        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_topology_isolate_set,
                                              (unit, soc_dnxc_isolation_status_isolated)));

        SOC_PBMP_ITER(active_links, port)
        {
            SHR_IF_ERR_EXIT(soc_dnxc_port_enable_set(unit, port, 0, 0));
        }

        sal_usleep(50000);

    }
    else
    {

        SOC_PBMP_ITER(active_links, port)
        {
            SHR_IF_ERR_EXIT(soc_dnxc_port_enable_set(unit, port, 0, 1));
        }

        sal_usleep(500000);

        if (!isolate_device)
        {
            SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_topology_isolate_set,
                                                  (unit, soc_dnxc_isolation_status_active)));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_init(
    int unit)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNXF_UNIT_LOCK_TAKE(unit);

    SHR_IF_ERR_EXIT(soc_dnxf_ha_init(unit));
    SHR_IF_ERR_EXIT(soc_dnxf_sw_state_deinit(unit));
    SHR_IF_ERR_EXIT(soc_dnxf_soc_info_get(unit));
#ifdef ADAPTER_SERVER_MODE
    SHR_IF_ERR_EXIT(dnxc_init_adapter_reg_access_init(unit));
#endif

    SHR_IF_ERR_EXIT(soc_dnxf_init_device_reset(unit));
    SHR_IF_ERR_EXIT(soc_dnxf_init_blocks_config(unit));
    SHR_IF_ERR_EXIT(soc_dnxf_counter_attach(unit));
    SHR_IF_ERR_EXIT(soc_dnxf_interrupts_init(unit));
    SHR_IF_ERR_EXIT(soc_dnxf_soc_init_done(unit));

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;

}

int
soc_dnxf_deinit(
    int unit)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    DNXF_UNIT_LOCK_TAKE(unit);

#ifdef ADAPTER_SERVER_MODE
    SHR_IF_ERR_EXIT(dnxc_init_adapter_reg_access_deinit(unit));
#endif

    SHR_IF_ERR_EXIT(soc_dnxf_init_device_reset_deinit(unit));
    SHR_IF_ERR_EXIT(soc_dnxf_counter_dettach(unit));
    SHR_IF_ERR_EXIT(soc_dnxf_interrupts_deinit(unit));
    SHR_IF_ERR_EXIT(soc_dnxf_soc_init_done_deinit(unit));

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

int
soc_dnxf_soft_reset_init(
    int unit)
{
    int rc = _SHR_E_NONE;

    DNXF_UNIT_LOCK_INIT(unit);

    soc_pbmp_t pbmp_enabled;
    soc_port_t port;
    int enable;
    soc_dnxc_isolation_status_t device_isolation_status;

    SHR_FUNC_INIT_VARS(unit);

    DNXF_UNIT_LOCK_TAKE(unit);

    if (!SOC_WARM_BOOT(unit) && !SOC_IS_RELOADING(unit))
    {

        SHR_IF_ERR_EXIT(soc_dnxc_interrupt_all_enable_set(unit, 0));
        SOC_PBMP_CLEAR(pbmp_enabled);
        SOC_PBMP_ITER(PBMP_SFI_ALL(unit), port)
        {
            rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_enable_get, (unit, port, &enable));
            SHR_IF_ERR_EXIT(rc);
            if (enable)
            {
                SOC_PBMP_PORT_ADD(pbmp_enabled, port);
            }
        }

        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_topology_isolate_get, (unit, &device_isolation_status));
        SHR_IF_ERR_EXIT(rc);

        rc = soc_dnxf_shutdown_set(unit, pbmp_enabled, 1, 0);
        SHR_IF_ERR_EXIT(rc);

        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_soft_init, (unit, SOC_DNXC_RESET_ACTION_INOUT_RESET));
        SHR_IF_ERR_EXIT(rc);

        sal_usleep(10000);

        rc = soc_dnxf_shutdown_set(unit, pbmp_enabled, 0,
                                   device_isolation_status == soc_dnxc_isolation_status_isolated ? 1 : 0);
        SHR_IF_ERR_EXIT(rc);

    }

    SOC_FLAGS_SET(unit, SOC_FLAGS_GET(unit) | SOC_F_INITED);

exit:
    if (!SOC_WARM_BOOT(unit) && !SOC_IS_RELOADING(unit))
    {

        SHR_IF_ERR_EXIT(soc_dnxc_interrupt_all_enable_set(unit, 1));
    }
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;

}

soc_driver_t *
soc_dnxf_chip_driver_find(
    int unit,
    uint16 pci_dev_id,
    uint8 pci_rev_id)
{
    uint16 driver_dev_id;
    uint16 driver_rev_id;
    if (soc_cm_get_id_driver(pci_dev_id, pci_rev_id, &driver_dev_id, &driver_rev_id) < 0)
    {
        return NULL;
    }

    if (SOC_IS_RAMON_TYPE(driver_dev_id))
    {
#if defined(BCM_88790_A0)
        return &soc_driver_bcm88790_a0;
#endif
    }
    else
    {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "soc_dnxf_chip_driver_find: driver in devid table " "not in soc_base_driver_table\n")));
    }

    return NULL;
}

int
soc_dnxf_drv_supported_lanes_get(
    int unit,
    soc_pbmp_t * supported_lanes)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_supported_lanes_get, (unit, supported_lanes)));

exit:

    SHR_FUNC_EXIT;
}

int
soc_dnxf_drv_fmac_lane_aligned_get(
    int unit,
    int lane_id,
    int *aligned_lane_id)
{
    int nof_links_in_fmac, nof_links_in_fsrd, pm_offset, inner_lane;
    SHR_FUNC_INIT_VARS(unit);

    nof_links_in_fmac = dnxf_data_device.blocks.nof_links_in_fmac_get(unit);
    nof_links_in_fsrd = dnxf_data_device.blocks.nof_links_in_fsrd_get(unit);

    *aligned_lane_id = lane_id;

    if (dnxf_data_port.lane_map.feature_get(unit, dnxf_data_port_lane_map_is_flexible_link_mapping_supported) &&
        dnxf_data_device.general.feature_get(unit, dnxf_data_device_general_is_not_all_cores_active))
    {
        pm_offset = (lane_id / nof_links_in_fsrd) * nof_links_in_fmac;
        inner_lane = lane_id % nof_links_in_fmac;
        *aligned_lane_id = pm_offset + inner_lane;
    }

    SHR_FUNC_EXIT;
}

int
soc_dnxf_drv_block_valid_get(
    int unit,
    int blktype,
    int blockid,
    char *valid)
{

    int enabled = 1;
    soc_pbmp_t pbmp_supported;
    soc_pbmp_t pbmp_fsrd_tmp;

    SHR_FUNC_INIT_VARS(unit);

#ifdef PLISIM
    {

        if (!(SOC_FLAGS_GET(unit) & SOC_F_ATTACHED))
        {
            SHR_EXIT();
        }
    }
#endif

    SOC_PBMP_CLEAR(pbmp_fsrd_tmp);
    SHR_IF_ERR_EXIT(soc_dnxf_drv_supported_lanes_get(unit, &pbmp_supported));

    if (blktype == SOC_BLK_FSRD)
    {

        SOC_PBMP_PORTS_RANGE_ADD(pbmp_fsrd_tmp, blockid * dnxf_data_device.blocks.nof_links_in_fsrd_get(unit),
                                 dnxf_data_device.blocks.nof_links_in_fsrd_get(unit));
        SOC_PBMP_AND(pbmp_fsrd_tmp, pbmp_supported);
        if (SOC_PBMP_IS_NULL(pbmp_fsrd_tmp))
        {
            enabled = 0;
        }

        *valid = ((enabled == 1) ? 1 : 0);
    }
    else
    {
        *valid = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_info_config(
    int unit,
    int dev_id)
{
    soc_info_t *si;
    soc_control_t *soc;
    int mem, blk, blktype;
    int port, phy_port, bindex;

    SHR_FUNC_INIT_VARS(unit);
    soc = SOC_CONTROL(unit);

#ifdef BCM_DNXF1_SUPPORT
    if (SOC_IS_RAMON_TYPE(dev_id))
    {
        SOC_CHIP_STRING(unit) = "ramon";
    }
    else
#endif
    {
        SOC_CHIP_STRING(unit) = "???";
        LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "soc_dnxf_info_config: driver device %04x unexpected\n"), dev_id));
        SHR_ERR_EXIT(_SHR_E_UNIT, "failed to find device id");
    }

    si = &SOC_INFO(unit);
    si->driver_type = soc->chip_driver->type;
    si->driver_group = soc_chip_type_map[si->driver_type];

    si->fe.min = si->fe.max = -1;
    si->fe.num = 0;
    si->ge.min = si->ge.max = -1;
    si->ge.num = 0;
    si->xe.min = si->xe.max = -1;
    si->xe.num = 0;
    si->hg.min = si->hg.max = -1;
    si->hg.num = 0;
    si->hg_subport.min = si->hg_subport.max = -1;
    si->hg_subport.num = 0;
    si->hl.min = si->hl.max = -1;
    si->hl.num = 0;
    si->st.min = si->st.max = -1;
    si->st.num = 0;
    si->gx.min = si->gx.max = -1;
    si->gx.num = 0;
    si->xg.min = si->xg.max = -1;
    si->xg.num = 0;
    si->spi.min = si->spi.max = -1;
    si->spi.num = 0;
    si->spi_subport.min = si->spi_subport.max = -1;
    si->spi_subport.num = 0;
    si->sci.min = si->sci.max = -1;
    si->sci.num = 0;
    si->sfi.min = si->sfi.max = -1;
    si->sfi.num = 0;
    si->port.min = si->port.max = -1;
    si->port.num = 0;
    si->ether.min = si->ether.max = -1;
    si->ether.num = 0;
    si->all.min = si->all.max = -1;
    si->all.num = 0;

    si->port_num = 0;

    sal_memset(si->has_block, 0, sizeof(soc_block_t) * COUNTOF(si->has_block));

    for (blk = 0; blk < SOC_MAX_NUM_BLKS; blk++)
    {
        si->block_port[blk] = REG_PORT_ANY;
        si->block_valid[blk] = 0;
    }

    SOC_PBMP_CLEAR(si->cmic_bitmap);

    si->cmic_block = -1;

    SHR_IF_ERR_EXIT(soc_sand_info_config_blocks(unit, 0));

    si->block_num = blk;

    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        si->mem_block_any[mem] = -1;
        if (SOC_MEM_IS_VALID(unit, mem))
        {
            SOC_MEM_BLOCK_ITER(unit, mem, blk)
            {
                si->mem_block_any[mem] = blk;
                break;
            }
        }
    }

    for (phy_port = 0;; phy_port++)
    {

        blk = SOC_DRIVER(unit)->port_info[phy_port].blk;
        bindex = SOC_DRIVER(unit)->port_info[phy_port].bindex;
        if (blk < 0 && bindex < 0)
        {
            break;
        }

        port = phy_port;

        if (blk < 0)
        {
            blktype = 0;
        }
        else
        {
            blktype = SOC_BLOCK_INFO(unit, blk).type;
        }

        if (blktype == 0)
        {
            sal_snprintf(si->port_name[port], sizeof(si->port_name[port]), "sfi%d", port);
            si->port_offset[port] = port;
            continue;
        }

        switch (blktype)
        {
            case SOC_BLK_CMIC:
                si->cmic_port = port;
                sal_snprintf(SOC_PORT_NAME(unit, port), SOC_DRV_PORT_NAME_MAX, "CMIC");
                SOC_PBMP_PORT_ADD(si->cmic_bitmap, port);
                break;
            default:
                si->port_num_lanes[port] = 1;
                sal_snprintf(SOC_PORT_NAME(unit, port), SOC_DRV_PORT_NAME_MAX, "sfi%d", port);
                sal_snprintf(SOC_PORT_NAME_ALTER(unit, port), SOC_DRV_PORT_NAME_MAX, "fabric%d", port);
                SOC_PORT_NAME_ALTER_VALID(unit, port) = 1;
                DNXF_ADD_PORT(sfi, port);
                DNXF_ADD_PORT(port, port);
                DNXF_ADD_PORT(all, port);
                break;
        }

        si->port_type[phy_port] = blktype;

    }

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_detach(
    int unit)
{
    soc_control_t *soc;
    int mem;
    int cmc;
    int rc;
    int spl;
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "invalid unit");
    }

    soc = SOC_CONTROL(unit);
    if (soc == NULL)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_CONT(soc_sand_indirect_mem_access_deinit(unit));

    if (soc->miimMutex)
    {
        sal_mutex_destroy(soc->miimMutex);
        soc->miimMutex = NULL;
    }

    if (soc->counterMutex)
    {
        sal_mutex_destroy(soc->counterMutex);
        soc->counterMutex = NULL;
    }

    if (soc->schanMutex)
    {
        sal_mutex_destroy(soc->schanMutex);
        soc->schanMutex = NULL;
    }

    for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit) + 1; cmc++)
    {
        if (soc->schanIntr[cmc])
        {
            sal_sem_destroy(soc->schanIntr[cmc]);
            soc->schanIntr[cmc] = NULL;
        }
    }

    if ((soc_mem_dmaable(unit, RTP_MULTI_CAST_TABLE_UPDATEm, SOC_MEM_BLOCK_ANY(unit, RTP_MULTI_CAST_TABLE_UPDATEm))
         || soc_mem_slamable(unit, RTP_MULTI_CAST_TABLE_UPDATEm, SOC_MEM_BLOCK_ANY(unit, RTP_MULTI_CAST_TABLE_UPDATEm)))
        && soc_feature(unit, soc_feature_sbusdma))
    {

        SHR_IF_ERR_EXIT(sand_deinit_fill_table(unit));

        SHR_IF_ERR_EXIT(soc_sbusdma_desc_detach(unit));

        SHR_IF_ERR_EXIT(soc_dma_detach(unit));
    }
    (void) soc_sbusdma_lock_deinit(unit);

    SHR_IF_ERR_EXIT(soc_dnxc_intr_deinit(unit));

    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        if (SOC_MEM_IS_VALID(unit, mem))
        {

            if (soc->memState[mem].lock)
            {
                sal_mutex_destroy(soc->memState[mem].lock);
                soc->memState[mem].lock = NULL;
            }
        }
    }

    if (SOC_PERSIST(unit))
    {
        sal_free(SOC_PERSIST(unit));
        SOC_PERSIST(unit) = NULL;
    }

    if (soc->socControlMutex)
    {
        sal_mutex_destroy(soc->socControlMutex);
        soc->socControlMutex = NULL;
    }

    if (_soc_dnxf_lock[unit] != NULL)
    {
        sal_mutex_destroy(_soc_dnxf_lock[unit]);
        _soc_dnxf_lock[unit] = NULL;
    }
    if (soc->schan_wb_mutex != NULL)
    {
        sal_mutex_destroy(soc->schan_wb_mutex);
        soc->schan_wb_mutex = NULL;
    }

    rc = dnxf_data_deinit(unit);
    SHR_IF_ERR_EXIT(rc);

    if (SOC_CONTROL(unit)->drv != NULL)
    {
        sal_free((soc_dnxf_control_t *) SOC_CONTROL(unit)->drv);
        SOC_CONTROL(unit)->drv = NULL;
    }

    spl = sal_splhi();

    sal_free(soc);
    SOC_CONTROL(unit) = NULL;

    sal_spl(spl);

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_chip_type_set(
    int unit,
    uint16 dev_id)
{
    soc_info_t *si;
    SHR_FUNC_INIT_VARS(unit);
    si = &SOC_INFO(unit);

#ifdef BCM_DNXF1_SUPPORT
    if (SOC_IS_RAMON_TYPE(dev_id))
    {
        si->chip_type = SOC_INFO_CHIP_TYPE_RAMON;
    }
    else
#endif
    {
        si->chip_type = 0;

        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit, "soc_dnxf_chip_type_set: driver device %04x unexpected\n"), dev_id));
        SHR_ERR_EXIT(_SHR_E_UNIT, "failed to find device id");
    }

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_attach(
    int unit)
{
    soc_control_t *soc;
    soc_persist_t *sop;
    soc_info_t *si;
    uint16 dev_id;
    uint8 rev_id;
    int rc = _SHR_E_NONE, mem;
    int cmc;
#ifdef BCM_CMICX_SUPPORT
    static int default_fw_loaded[SOC_MAX_NUM_DEVICES] = { 0 };
#endif

    SHR_FUNC_INIT_VARS(unit);

    soc = SOC_CONTROL(unit);
    if (soc == NULL)
    {
        soc = sal_alloc(sizeof(soc_control_t), "soc_control");
        if (soc == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate soc_control");
        }
        sal_memset(soc, 0, sizeof(soc_control_t));
        SOC_CONTROL(unit) = soc;
    }
    else
    {
        if (SOC_FLAGS_GET(unit) & SOC_F_ATTACHED)
        {
            SHR_EXIT();
        }
    }

    soc->soc_link_pause = 0;

    SOC_PCI_CMCS_NUM(unit) = soc_property_uc_get(unit, 0, spn_PCI_CMCS_NUM, 1);

    soc_cm_get_id(unit, &dev_id, &rev_id);

    if ((CMDEV(unit).dev.info->rev_id & SOC_CM_DEVICE_ANY_LAST_DEVID_DIGIT) != 0)
    {
        uint32 rev_id_f = CMDEV(unit).dev.info->rev_id & SOC_CM_REVID_MASK;
        uint8 last_revid_digit = (rev_id_f - 1) & 0xf;
        sal_snprintf(SOC_CONTROL(unit)->device_name, sizeof(SOC_CONTROL(unit)->device_name) - 1, "BCM8%.4X_%c%c",
                     (unsigned) CMDEV(unit).dev.dev_id, 'a' + (rev_id_f >> 4),
                     (last_revid_digit < 10 ? ('0' + last_revid_digit) : ('a' - 10) + last_revid_digit));
    }
    SHR_IF_ERR_EXIT(soc_dnxf_chip_type_set(unit, dev_id));

    soc->chip_driver = soc_dnxf_chip_driver_find(unit, dev_id, rev_id);
    if (soc->chip_driver == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "unit has no driver (device 0x%04x rev 0x%02x)", dev_id, rev_id);
    }

    soc_feature_init(unit);

    soc_dnxc_warn_if_device_is_used_by_diffrent_sdk(unit);

    if (soc_cmic_intr_init(unit) < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "error initializing cmic interrupt");
    }

    soc_cmic_intr_all_disable(unit);

    SHR_IF_ERR_EXIT(soc_cmicx_handle_hotswap_and_dma_abort(unit));

    if ((soc->miimMutex = sal_mutex_create("MIIM")) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Failed to allocate MIIM lock");
    }

    if (_soc_dnxf_lock[unit] == NULL)
    {
        if ((_soc_dnxf_lock[unit] = sal_mutex_create("bcm_dnxf_config_lock")) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "Failed to allocate DNXF lock (_soc_dnxf_lock)");
        }
    }

    if ((soc->socControlMutex = sal_mutex_create("SOC_CONTROL")) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate socControlMutex");
    }

    soc->counterMutex = sal_mutex_create("Counter");
    if (soc->counterMutex == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate counterMutex");
    }

    soc->schanMutex = sal_mutex_create("SCHAN");
    if (soc->schanMutex == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate schanMutex");
    }

    for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit) + 1; cmc++)
    {
        if ((soc->schanIntr[cmc] = sal_sem_create("SCHAN interrupt", sal_sem_BINARY, 0)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate schanSem");
        }
    }

    SOC_PERSIST(unit) = sal_alloc(sizeof(soc_persist_t), "soc_persist");
    if (SOC_PERSIST(unit) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate soc_persist");
    }
    sal_memset(SOC_PERSIST(unit), 0, sizeof(soc_persist_t));
    sop = SOC_PERSIST(unit);
    sop->version = 1;

    soc_led_driver_init(unit);

    soc_linkscan_driver_init(unit);

    si = &SOC_INFO(unit);
    si->driver_type = soc->chip_driver->type;
    si->driver_group = soc_chip_type_map[si->driver_type];

    SOC_FLAGS_SET(unit, SOC_FLAGS_GET(unit) | SOC_F_ATTACHED);

    rc = dnxf_data_init(unit);
    SHR_IF_ERR_EXIT(rc);

    rc = mbcm_dnxf_init(unit);
    if (rc != _SHR_E_NONE)
    {
        LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "soc_dnxf_attach error in mbcm_dnxf_init\n")));
    }
    SHR_IF_ERR_EXIT(rc);

    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_controlled_counter_set, (unit));
    SHR_IF_ERR_EXIT(rc);

    rc = soc_dnxf_control_init(unit);
    SHR_IF_ERR_EXIT(rc);

#ifdef BCM_SBUSDMA_SUPPORT
    SOC_CONTROL(unit)->max_sbusdma_channels = SOC_DNXC_MAX_SBUSDMA_CHANNELS;
    SOC_CONTROL(unit)->tdma_ch = SOC_DNXC_TDMA_CHANNEL;
    SOC_CONTROL(unit)->tslam_ch = SOC_DNXC_TSLAM_CHANNEL;
    SOC_CONTROL(unit)->desc_ch = SOC_DNXC_DESC_CHANNEL;

    SOC_MEM_CLEAR_CHUNK_SIZE_SET(unit, dnxf_data_device.access.mem_clear_chunk_size_get(unit));
#endif

    rc = soc_dnxc_intr_init(unit);
    SHR_IF_ERR_EXIT(rc);

    rc = soc_sbusdma_lock_init(unit);
    if (rc != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(rc, "failed to Initialize SBUSDMA Locks");
    }

    if ((soc_mem_dmaable(unit, RTP_MULTI_CAST_TABLE_UPDATEm, SOC_MEM_BLOCK_ANY(unit, RTP_MULTI_CAST_TABLE_UPDATEm))
         || soc_mem_slamable(unit, RTP_MULTI_CAST_TABLE_UPDATEm, SOC_MEM_BLOCK_ANY(unit, RTP_MULTI_CAST_TABLE_UPDATEm)))
        && soc_feature(unit, soc_feature_sbusdma))
    {

        SHR_IF_ERR_EXIT(soc_dma_attach(unit, 1));

        rc = soc_sbusdma_init(unit, dnxf_data_device.access.sbus_dma_interval_get(unit),
                              dnxf_data_device.access.sbus_dma_intr_enable_get(unit));
        if (rc != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(rc, "failed to Initialize SBUSDMA");
        }
        SHR_IF_ERR_EXIT(sand_init_fill_table(unit));

    }

    SHR_IF_ERR_EXIT(soc_cmicx_uc_msg_init(unit));

    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        if (SOC_MEM_IS_VALID(unit, mem))
        {
            sop->memState[mem].index_max = SOC_MEM_INFO(unit, mem).index_max;

            if ((soc->memState[mem].lock = sal_mutex_create(SOC_MEM_NAME(unit, mem))) == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate memState lock");
            }

            sal_memset(soc->memState[mem].cache, 0, sizeof(soc->memState[mem].cache));
        }
        else
        {
            sop->memState[mem].index_max = -1;
        }
    }

    if ((soc->schan_wb_mutex = sal_mutex_create("SchanWB")) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Failed to allocate SchanWB");
    }

    rc = soc_schan_init(unit);
    if (rc != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(rc, "failed to Initialize SCHAN");
    }

    SHR_IF_ERR_EXIT(soc_sand_indirect_mem_access_init(unit));

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx) && !SAL_BOOT_PLISIM)
    {
        soc_cmicx_linkscan_pause(unit);
        soc_cmicx_led_enable(unit, 0);
        soc_iproc_m0ssq_reset(unit);

        if (default_fw_loaded[unit] == 0)
        {
            SOC_IF_ERROR_RETURN(soc_cmicx_led_linkscan_default_fw_load(unit));
            default_fw_loaded[unit] = 1;
        }
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_dump(
    int unit,
    char *pfx)
{
    soc_control_t *soc;
    soc_persist_t *sop;
    soc_stat_t *stat;
    uint16 dev_id;
    uint8 rev_id;
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "invalid unit");
    }

    soc = SOC_CONTROL(unit);
    sop = SOC_PERSIST(unit);

    stat = &soc->stat;

    LOG_CLI((BSL_META_U(unit, "%sUnit %d Driver Control Structure:\n"), pfx, unit));

    soc_cm_get_id(unit, &dev_id, &rev_id);

    LOG_CLI((BSL_META_U(unit,
                        "%sChip=%s Rev=0x%02x Driver=%s\n"),
             pfx, soc_dev_name(unit), rev_id, SOC_CHIP_NAME(soc->chip_driver->type)));
    LOG_CLI((BSL_META_U(unit, "%sFlags=0x%x:"), pfx, SOC_FLAGS_GET(unit)));
    if (SOC_FLAGS_GET(unit) & SOC_F_ATTACHED)
    {
        LOG_CLI((BSL_META_U(unit, " attached")));
    }
    if (SOC_FLAGS_GET(unit) & SOC_F_INITED)
    {
        LOG_CLI((BSL_META_U(unit, " initialized")));
    }
    if (SOC_FLAGS_GET(unit) & SOC_F_LSE)
    {
        LOG_CLI((BSL_META_U(unit, " link-scan")));
    }
    if (SOC_FLAGS_GET(unit) & SOC_F_SL_MODE)
    {
        LOG_CLI((BSL_META_U(unit, " sl-mode")));
    }
    if (SOC_FLAGS_GET(unit) & SOC_F_POLLED)
    {
        LOG_CLI((BSL_META_U(unit, " polled")));
    }
    if (SOC_FLAGS_GET(unit) & SOC_F_URPF_ENABLED)
    {
        LOG_CLI((BSL_META_U(unit, " urpf")));
    }
    if (SOC_FLAGS_GET(unit) & SOC_F_MEM_CLEAR_USE_DMA)
    {
        LOG_CLI((BSL_META_U(unit, " mem-clear-use-dma")));
    }
    if (SOC_FLAGS_GET(unit) & SOC_F_IPMCREPLSHR)
    {
        LOG_CLI((BSL_META_U(unit, " ipmc-repl-shared")));
    }
    if (soc->remote_cpu)
    {
        LOG_CLI((BSL_META_U(unit, " rcpu")));
    }
    LOG_CLI((BSL_META_U(unit, "; board type 0x%x"), soc->board_type));
    LOG_CLI((BSL_META_U(unit, "\n")));
    LOG_CLI((BSL_META_U(unit, "%s"), pfx));
    soc_cm_dump(unit);

    LOG_CLI((BSL_META_U(unit,
                        "%sDisabled: reg_flags=0x%x mem_flags=0x%x\n"),
             pfx, soc->disabled_reg_flags, soc->disabled_mem_flags));
    LOG_CLI((BSL_META_U(unit,
                        "%sSchanOps=%d MMUdbg=%d LinkPause=%d\n"),
             pfx, stat->schan_op, sop->debugMode, soc->soc_link_pause));
    LOG_CLI((BSL_META_U(unit,
                        "%sCounter: int=%dus per=%dus dmaBuf=%p\n"),
             pfx,
             soc->counter_interval,
             SAL_USECS_SUB(soc->counter_coll_cur, soc->counter_coll_prev), (void *) soc->counter_buf32));
    LOG_CLI((BSL_META_U(unit,
                        "%sTimeout: Schan=%d(%dus) MIIM=%d(%dus)\n"),
             pfx, stat->err_sc_tmo, soc->schanTimeout, stat->err_mii_tmo, soc->miimTimeout));
    LOG_CLI((BSL_META_U(unit,
                        "%sIntr: Total=%d Sc=%d ScErr=%d MMU/ARLErr=%d\n"
                        "%s      LinkStat=%d PCIfatal=%d PCIparity=%d\n"
                        "%s      ARLdrop=%d ARLmbuf=%d ARLxfer=%d ARLcnt0=%d\n"
                        "%s      TableDMA=%d TSLAM-DMA=%d\n"
                        "%s      MemCmd[BSE]=%d MemCmd[CSE]=%d MemCmd[HSE]=%d\n"
                        "%s      ChipFunc[0]=%d ChipFunc[1]=%d ChipFunc[2]=%d\n"
                        "%s      ChipFunc[3]=%d ChipFunc[4]=%d\n"
                        "%s      I2C=%d MII=%d StatsDMA=%d Desc=%d Chain=%d\n"),
             pfx, stat->intr, stat->intr_sc, stat->intr_sce, stat->intr_mmu,
             pfx, stat->intr_ls,
             stat->intr_pci_fe, stat->intr_pci_pe,
             pfx, stat->intr_arl_d, stat->intr_arl_m,
             stat->intr_arl_x, stat->intr_arl_0,
             pfx, stat->intr_tdma, stat->intr_tslam,
             pfx, stat->intr_mem_cmd[0],
             stat->intr_mem_cmd[1], stat->intr_mem_cmd[2],
             pfx, stat->intr_chip_func[0], stat->intr_chip_func[1],
             stat->intr_chip_func[2],
             pfx, stat->intr_chip_func[3], stat->intr_chip_func[4],
             pfx, stat->intr_i2c, stat->intr_mii, stat->intr_stats, stat->intr_desc, stat->intr_chain));
exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_nof_interrupts(
    int unit,
    int *nof_interrupts)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "invalid unit");
    }

    SHR_NULL_CHECK(nof_interrupts, _SHR_E_PARAM, "nof_interrupts");

    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_nof_interrupts, (unit, nof_interrupts));
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_nof_block_instances(
    int unit,
    soc_block_types_t block_types,
    int *nof_block_instances)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "invalid unit");
    }

    SHR_NULL_CHECK(nof_block_instances, _SHR_E_PARAM, "nof_block_instances");

    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_nof_block_instance, (unit, block_types, nof_block_instances));
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

void
soc_dnxf_free_cache_memory(
    int unit)
{
    soc_mem_t mem;
    soc_error_t rc = SOC_E_NONE;

    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        if (!SOC_MEM_IS_VALID(unit, mem))
        {
            continue;
        }

        rc = soc_mem_cache_set(unit, mem, COPYNO_ALL, FALSE);
        if (SOC_FAILURE(rc))
        {
            LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "Error to deallocate cache for mem %d\n"), mem));
        }
    }
}

int
soc_dnxf_compiler_64_div_32(
    uint64 x,
    uint32 y,
    uint32 *result)
{
    uint64 rem;
    uint64 b;
    uint64 res, d;
    uint32 high;

    COMPILER_64_SET(rem, COMPILER_64_HI(x), COMPILER_64_LO(x));
    COMPILER_64_SET(b, 0, y);
    COMPILER_64_SET(d, 0, 1);

    high = COMPILER_64_HI(rem);

    COMPILER_64_ZERO(res);
    if (high >= y)
    {

        LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META("soc_dnxf_compiler_64_div_32: result > 32bits\n")));
        return _SHR_E_PARAM;
    }

    while ((!COMPILER_64_BITTEST(b, 63)) && (COMPILER_64_LT(b, rem)))
    {
        COMPILER_64_ADD_64(b, b);
        COMPILER_64_ADD_64(d, d);
    }

    do
    {
        if (COMPILER_64_GE(rem, b))
        {
            COMPILER_64_SUB_64(rem, b);
            COMPILER_64_ADD_64(res, d);
        }
        COMPILER_64_SHR(b, 1);
        COMPILER_64_SHR(d, 1);
    }
    while (!COMPILER_64_IS_ZERO(d));

    *result = COMPILER_64_LO(res);

    return _SHR_E_NONE;
}

int
soc_dnxf_drv_mbist(
    int unit,
    int skip_errors)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_mbist, (unit, skip_errors));
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_device_reset(
    int unit,
    int mode,
    int action)
{
    SHR_FUNC_INIT_VARS(unit);

    SOC_HW_RESET_START(unit);
    switch (mode)
    {
        case SOC_DNXC_RESET_MODE_BLOCKS_RESET:
            SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_blocks_reset, (unit, action, NULL)));
            break;
        case SOC_DNXC_RESET_MODE_BLOCKS_SOFT_RESET:
            SHR_IF_ERR_EXIT(soc_dnxf_soft_reset_init(unit));
            break;
        case SOC_DNXC_RESET_MODE_INIT_RESET:
            SHR_IF_ERR_EXIT(soc_dnxf_init(unit));
            break;
        case SOC_DNXC_RESET_MODE_REG_ACCESS:

            SHR_IF_ERR_EXIT(soc_dnxf_info_soc_properties(unit));
            SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_reg_access_only_reset, (unit)));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown/Unsupported Reset Mode");
    }

exit:
    SOC_HW_RESET_DONE(unit);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnxf_access_block_reset_set(
    int unit,
    soc_block_t block_type,
    int block_instance,
    int value)
{
    soc_info_t *si;
    SHR_FUNC_INIT_VARS(unit);
    si = &SOC_INFO(unit);
    if (value == 1)
    {
        si->block_valid[block_type] = 0;
    }
    LOG_WARN(BSL_LOG_MODULE,
             (BSL_META_U(unit, "block_type %d - instance %d - value %d\n"), block_type, block_instance, value));

    SHR_FUNC_EXIT;
}
shr_error_e
dnxf_access_blocks_reset_override(
    int unit)
{
    int index = 0;
    int table_size;
    const dnxf_data_device_blocks_override_t *override_info;
    const dnxc_data_table_info_t *table_info;
    soc_block_t block_type;
    SHR_FUNC_INIT_VARS(unit);

    table_info = dnxf_data_device.blocks.override_info_get(unit);
    table_size = table_info->key_size[0];

    for (index = 0; index < table_size; index++)
    {

        override_info = dnxf_data_device.blocks.override_get(unit, index);
        if (override_info->block_type != NULL)
        {

            block_type = soc_block_name_match(unit, override_info->block_type);
            if (block_type == SOC_BLK_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "block not found %s\n", override_info->block_type);
            }
            SHR_IF_ERR_EXIT(dnxf_access_block_reset_set
                            (unit, block_type, override_info->block_instance, override_info->value));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_drv_ms_to_cycles(
    int unit,
    int nof_ms,
    int *nof_cycles)
{
    int core_clock_khz;
    SHR_FUNC_INIT_VARS(unit);

    core_clock_khz = dnxf_data_device.general.core_clock_khz_get(unit);

    *nof_cycles = core_clock_khz / nof_ms;

    SHR_FUNC_EXIT;
}

int
soc_dnxf_set_rtp_config(
    int unit)
{
    uint32 reg_val32;
    uint64 reg_val64;
    soc_reg_above_64_val_t reg_above64_val;
    uint32 rtpwp;
    uint32 gpd_rmgr_global_in_clocks = 0;
    uint32 gpd_rmgr_global_in_units_clocks = 0;
    uint32 gpd_protect_cnt_th_in_clocks = 0;
    uint32 gpd_cnt_th_in_clocks = 0;
    dnxc_time_t dnxc_time;

    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO(reg_val64);

    dnxc_time.time_units = DNXC_TIME_UNIT_USEC;
    dnxc_time.time = dnxf_data_fabric.reachability.watchdog_rate_get(unit);
    SHR_IF_ERR_EXIT(dnxc_time_time_to_clock_cycles_get(unit, &dnxc_time, &rtpwp));

    SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_PROCESSOR_CONFIGURATIONr(unit, &reg_val64));
    soc_reg64_field32_set(unit, RTP_REACHABILITY_MESSAGE_PROCESSOR_CONFIGURATIONr, &reg_val64, RTPWPf, rtpwp);
    SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_PROCESSOR_CONFIGURATIONr(unit, reg_val64));

    if (SOC_DNXF_IS_FE13(unit))
    {
        SHR_IF_ERR_EXIT(READ_RTP_ALL_REACHABLE_CFGr(unit, &reg_val32));
        soc_reg_field_set(unit, RTP_ALL_REACHABLE_CFGr, &reg_val32, ALRC_ENABLE_SLOW_LINK_DOWNf, 1);
        SHR_IF_ERR_EXIT(WRITE_RTP_ALL_REACHABLE_CFGr(unit, reg_val32));
    }

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL
                    (unit, mbcm_dnxf_stk_module_max_fap_set,
                     (unit, dnxf_data_fabric.topology.nof_system_modid_get(unit) - 1)));

    SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above64_val));
    soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above64_val, RMGRf, 0);
    soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above64_val, RMGR_GLOBALf,
                                 0);
    SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above64_val));

    COMPILER_64_ZERO(reg_val64);

    dnxc_time.time_units = DNXC_TIME_UNIT_USEC;
    dnxc_time.time = dnxf_data_fabric.reachability.gpd_gen_rate_full_cycle_get(unit);
    SHR_IF_ERR_EXIT(dnxc_time_time_to_clock_cycles_get(unit, &dnxc_time, &gpd_rmgr_global_in_clocks));
    gpd_rmgr_global_in_units_clocks = gpd_rmgr_global_in_clocks / dnxf_data_fabric.reachability.rmgr_units_get(unit);

    dnxc_time.time_units = DNXC_TIME_UNIT_USEC;
    dnxc_time.time = dnxf_data_fabric.gpd.in_time_get(unit);
    SHR_IF_ERR_EXIT(dnxc_time_time_to_clock_cycles_get(unit, &dnxc_time, &gpd_protect_cnt_th_in_clocks));

    dnxc_time.time_units = DNXC_TIME_UNIT_MILISEC;
    dnxc_time.time = dnxf_data_fabric.gpd.out_time_get(unit);
    SHR_IF_ERR_EXIT(dnxc_time_time_to_clock_cycles_get(unit, &dnxc_time, &gpd_cnt_th_in_clocks));

    SHR_IF_ERR_EXIT(READ_RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr(unit, &reg_val64));
    soc_reg64_field32_set(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, &reg_val64, GRACEFUL_PD_ENf, 1);
    soc_reg64_field32_set(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, &reg_val64, GRACEFUL_PD_RMGRf,
                          gpd_rmgr_global_in_units_clocks);
    soc_reg64_field32_set(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, &reg_val64, GRACEFUL_PD_PROTECT_ENf, 1);
    soc_reg64_field32_set(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, &reg_val64, GRACEFUL_PD_PROTECT_CNT_THf,
                          gpd_protect_cnt_th_in_clocks);
    soc_reg64_field32_set(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, &reg_val64, GRACEFUL_PD_CNT_THf,
                          gpd_cnt_th_in_clocks);
    SHR_IF_ERR_EXIT(WRITE_RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr(unit, reg_val64));

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_link_device_mapping_bmp_get(
    int unit,
    int block_id,
    int block_type,
    soc_pbmp_t * links)
{
    int link_iter, link_iter_block_id, link_iter_block_inner_link, link_is_valid;

    SHR_FUNC_INIT_VARS(unit);

    SOC_PBMP_CLEAR(*links);

    SOC_PBMP_ITER(PBMP_SFI_ALL(unit), link_iter)
    {

        SHR_IF_ERR_EXIT(portmod_port_is_valid(unit, link_iter, &link_is_valid));
        if (link_is_valid)
        {
            SHR_IF_ERR_EXIT(soc_dnxf_drv_link_to_block_mapping
                            (unit, link_iter, &link_iter_block_id, &link_iter_block_inner_link, block_type));

            if (block_id == link_iter_block_id)
            {
                SOC_PBMP_PORT_ADD(*links, link_iter);
            }
        }

    }

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_block_instance_bmp_per_device_stage_get(
    int unit,
    int block_type,
    soc_dnxf_fabric_device_stage_t block_stage,
    soc_pbmp_t * block_bmp)
{
    int nof_dch_per_core =
        dnxf_data_device.blocks.nof_instances_dch_get(unit) / dnxf_data_device.general.nof_cores_get(unit);
    int dch_instance, core_idx, min_core_block, max_core_block;
    const SHR_BITDCL *active_cores;

    SHR_FUNC_INIT_VARS(unit);

    if (block_stage == soc_dnxf_fabric_device_stage_fe1)
    {
        block_stage = block_type == SOC_BLK_DCH ? soc_dnxf_fabric_device_stage_fe1 : soc_dnxf_fabric_device_stage_fe3;
    }
    else if (block_stage == soc_dnxf_fabric_device_stage_fe3)
    {
        block_stage = block_type == SOC_BLK_DCH ? soc_dnxf_fabric_device_stage_fe3 : soc_dnxf_fabric_device_stage_fe1;
    }
    else
    {
        block_stage = soc_dnxf_fabric_device_stage_fe2;
    }

    SOC_PBMP_CLEAR(*block_bmp);

    SHR_IF_ERR_EXIT(dnxf_state.info.active_core_bmp.get(unit, &active_cores));

    SOC_DNXF_CORE_ITER(active_cores, core_idx)
    {

        min_core_block = core_idx * nof_dch_per_core;
        max_core_block = nof_dch_per_core + (core_idx * nof_dch_per_core);
        SOC_PBMP_ITER(dnxf_data_device.blocks.dch_instances_to_device_stage_get(unit, block_stage)->dch_instance_bmp,
                      dch_instance)
        {
            if (dch_instance >= min_core_block && dch_instance < max_core_block)
            {
                SOC_PBMP_PORT_ADD(*block_bmp, dch_instance);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_fabric_link_device_stage_get(
    int unit,
    soc_port_t port,
    int is_rx,
    soc_dnxf_fabric_device_stage_t * link_device_stage)
{

    int dch_instance, dch_inner_link;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxf_drv_link_to_block_mapping(unit, port, &dch_instance, &dch_inner_link, SOC_BLK_DCH));

    switch (dnxf_data_fabric.general.device_mode_get(unit))
    {
        case soc_dnxf_fabric_device_mode_multi_stage_fe2:
        case soc_dnxf_fabric_device_mode_single_stage_fe2:
            *link_device_stage = soc_dnxf_fabric_device_stage_fe2;
            break;

        case soc_dnxf_fabric_device_mode_multi_stage_fe13:
            if (is_rx)
            {
                *link_device_stage =
                    (SOC_PBMP_MEMBER
                     (dnxf_data_device.blocks.dch_instances_to_device_stage_get(unit,
                                                                                soc_dnxf_fabric_device_stage_fe1)->dch_instance_bmp,
                      dch_instance)) ? soc_dnxf_fabric_device_stage_fe1 : soc_dnxf_fabric_device_stage_fe3;
            }
            else
            {
                *link_device_stage =
                    (SOC_PBMP_MEMBER
                     (dnxf_data_device.blocks.dch_instances_to_device_stage_get(unit,
                                                                                soc_dnxf_fabric_device_stage_fe3)->dch_instance_bmp,
                      dch_instance)) ? soc_dnxf_fabric_device_stage_fe1 : soc_dnxf_fabric_device_stage_fe3;
            }
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_FAIL, "device is not in a valid mode");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_drv_block_to_fmac_lane_mapping(
    int unit,
    int block_id,
    int inner_link,
    int block_type,
    int *fmac_lane)
{
    int fmac_lane_iter;
    int block_id_iter, inner_link_iter;
    uint32 nof_links = dnxf_data_port.general.nof_links_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    for (fmac_lane_iter = 0; fmac_lane_iter < nof_links; ++fmac_lane_iter)
    {
        SHR_IF_ERR_EXIT(soc_dnxf_drv_fmac_lane_to_block_mapping
                        (unit, fmac_lane_iter, &block_id_iter, &inner_link_iter, block_type));

        if ((block_id_iter == block_id) && (inner_link_iter == inner_link))
        {
            *fmac_lane = fmac_lane_iter;
            SHR_EXIT();
        }
    }

    *fmac_lane = -1;
    SHR_ERR_EXIT(_SHR_E_INTERNAL, "fmac lane matching the search criteria wasn't found");

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_drv_fmac_lane_to_block_mapping(
    int unit,
    int fmac_lane,
    int *block_id,
    int *inner_link,
    int block_type)
{
    uint32 nof_links_in_fsrd = dnxf_data_device.blocks.nof_links_in_fsrd_get(unit);
    uint32 nof_links_in_fmac = dnxf_data_device.blocks.nof_links_in_fmac_get(unit);
    uint32 nof_links_dch = dnxf_data_device.blocks.nof_links_in_dch_get(unit);
    uint32 nof_links_per_core = dnxf_data_device.blocks.nof_links_in_rtp_get(unit);

    soc_dnxc_lane_map_db_map_t lane2serdes;
    int rtp_link;

    SHR_FUNC_INIT_VARS(unit);

    if (dnxf_data_device.blocks.nof_instances_rtp_get(unit) > 1)
    {
        rtp_link = dnxf_data_device.blocks.fmac_lane_to_block_map_get(unit, fmac_lane)->rtp_link;
    }
    else
    {
        rtp_link = fmac_lane;
    }

    switch (block_type)
    {
        case SOC_BLK_RTP:
            *block_id = rtp_link / nof_links_per_core;
            *inner_link = rtp_link % nof_links_per_core;
            break;
        case SOC_BLK_FMAC:
            *block_id = fmac_lane / nof_links_in_fmac;
            *inner_link = fmac_lane % nof_links_in_fmac;
            break;
        case SOC_BLK_DCML:
        case SOC_BLK_DTM:
        case SOC_BLK_DTL:
        case SOC_BLK_LCM:
        case SOC_BLK_DFL:
        case SOC_BLK_DCH:
        case SOC_BLK_CCH:
        case SOC_BLK_CCML:
        case SOC_BLK_QRH:
            *block_id = rtp_link / nof_links_dch;
            *inner_link = rtp_link % nof_links_dch;
            break;
        case SOC_BLK_FSRD:
            SHR_IF_ERR_EXIT(dnxf_state.lane_map_db.serdes_map.lane2serdes.get(unit, fmac_lane, &lane2serdes));
            *block_id = lane2serdes.rx_id / nof_links_in_fsrd;
            *inner_link = lane2serdes.rx_id % nof_links_in_fsrd;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong block_type value %d", block_type);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_block_active_instance_bmp_get(
    int unit,
    int block_type,
    soc_pbmp_t * block_bmp)
{

    int logical_port = -1;
    int block_idx, block_inner_lane;

    SHR_FUNC_INIT_VARS(unit);

    SOC_PBMP_CLEAR(*block_bmp);
    SOC_PBMP_ITER(PBMP_SFI_ALL(unit), logical_port)
    {
        SHR_IF_ERR_EXIT(soc_dnxf_drv_link_to_block_mapping
                        (unit, logical_port, &block_idx, &block_inner_lane, block_type));
        SOC_PBMP_PORT_ADD(*block_bmp, block_idx);
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE
