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

#include <soc/dnxc/fabric.h>
#include <soc/dnxc/error.h>
#include <soc/dnxc/dnxc_intr.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/cmn/dnxf_warm_boot.h>
#include <soc/dnxf/cmn/dnxf_port.h>
#include <soc/dnxc/dnxc_cmic.h>
#include <soc/sand/sand_mem.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnxc/dnxc_mem.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_dev_init.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>

#include <sal/appl/sal.h>
#include <soc/dnxc/intr.h>
#include <soc/dnxc/dnxc_intr.h>
#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>

#ifdef BCM_CMICX_SUPPORT
#include <shared/cmicfw/cmicx_link.h>
#define CMICX_HW_LINKSCAN 1
#endif

#define SOC_DNXF_DEFAULT_MDIO_DIVISOR	(20)


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
        
        if (soc_dnxf_tbl_is_dynamic(unit, mem))
        {
            continue;
        }
        
        if (mem == RTP_SLSCTm)
        {
            if (soc_dnxf_load_balancing_mode_destination_unreachable !=
                dnxf_data_fabric.topology.load_balancing_mode_get(unit))
            {
                continue;
            }
        }

        
        if (sal_strstr(SOC_MEM_NAME(unit,mem), "BRDC_") != NULL)
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
soc_dnxf_soc_properties_validate(
    int unit)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_DNXF_IS_FE2(unit) && dnxf_data_fabric.general.local_routing_enable_uc_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "UC Local routing can be enabled only if device is in FE13 mode!");
    }

    if (SOC_DNXF_IS_FE2(unit) && dnxf_data_fabric.general.local_routing_enable_mc_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "MC Local routing can be enabled only if device is in FE13 mode!");
    }

    if (dnxf_data_fabric.cell.fifo_dma_enable_get(unit))
    {
        rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fifo_dma_fabric_cell_validate, (unit));
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}

STATIC int
soc_dnxf_info_soc_properties(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    
    soc_dnxf_soc_properties_control_set(unit);

    
     
    SHR_IF_ERR_EXIT(soc_dnxf_soc_properties_validate(unit));

    SHR_IF_ERR_EXIT(soc_dnxf_drv_soc_property_serdes_qrtt_read(unit));

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_drv_soc_property_serdes_qrtt_read(
    int unit)
{
    soc_pbmp_t          pbmp_disabled_links;
    int                 enabled_pbmp_control, disabled_pbmp_control;
    int                 quad, quad_inner_link, port, rc;
    uint32              quad_active;

    SHR_FUNC_INIT_VARS(unit);

    SOC_PBMP_CLEAR(pbmp_disabled_links);
    for (quad = 0; quad < dnxf_data_port.general.nof_links_get(unit)/dnxf_data_device.blocks.nof_links_in_fmac_get(unit); quad++) {

        
        quad_active = dnxf_data_port.static_add.quad_info_get(unit, quad)->quad_enable; 

         if (!quad_active) {
            for (quad_inner_link = 0; quad_inner_link < dnxf_data_device.blocks.nof_links_in_fmac_get(unit); quad_inner_link++) {
                SOC_PBMP_PORT_ADD(pbmp_disabled_links, quad*dnxf_data_device.blocks.nof_links_in_fmac_get(unit) + quad_inner_link);
            }
        }
    }

    for (port = 0; port < dnxf_data_port.general.nof_links_get(unit) ; port++) {
        

        enabled_pbmp_control =  SOC_DNXF_PBMP_PORT_ADD;
        disabled_pbmp_control = SOC_DNXF_PBMP_PORT_REMOVE;

        

        
        if (!(PBMP_MEMBER(dnxf_data_port.general.supported_phys_get(unit)->pbmp, port)) || PBMP_MEMBER(pbmp_disabled_links, port)) {
            enabled_pbmp_control =  SOC_DNXF_PBMP_PORT_REMOVE;
            disabled_pbmp_control = SOC_DNXF_PBMP_PORT_ADD;
        }

        
        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, enabled_pbmp_control, soc_dnxf_port_update_type_sfi));
        SHR_IF_ERR_EXIT(rc);

        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, enabled_pbmp_control, soc_dnxf_port_update_type_port));            
        SHR_IF_ERR_EXIT(rc);

        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, enabled_pbmp_control, soc_dnxf_port_update_type_all));            
        SHR_IF_ERR_EXIT(rc);

        
        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, disabled_pbmp_control, soc_dnxf_port_update_type_sfi_disabled));           
        SHR_IF_ERR_EXIT(rc);

        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, disabled_pbmp_control, soc_dnxf_port_update_type_port_disabled));            
        SHR_IF_ERR_EXIT(rc);

        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, disabled_pbmp_control, soc_dnxf_port_update_type_all_disabled));
        SHR_IF_ERR_EXIT(rc);
    }

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_control_init(
    int unit)
{
    int rv;
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
    rv = soc_dnxf_info_config(unit, dev_id);
    SHR_IF_ERR_CONT(rv);

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
dnxf_tbl_mem_cache_enable_specific_tbl(int unit, soc_mem_t mem, void* en)
{
    int rc        = SOC_E_NONE;
    int cache_enable = *(int*)en;

    SHR_FUNC_INIT_VARS(unit);

    SOC_MEM_ALIAS_TO_ORIG(unit, mem);
    if (soc_sand_mem_is_in_soc_property(unit, mem, !!cache_enable))
    {
        if(!SOC_MEM_IS_VALID(unit, mem) || !soc_mem_is_cachable(unit, mem))
        {
            LOG_ERROR(BSL_LS_SOC_MEM, (BSL_META("unit %d cache %s failed for %d (%s) rv %d\n"), unit, cache_enable ?"enable":"disable", mem, SOC_MEM_NAME(unit, mem), rc));
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
            LOG_ERROR(BSL_LS_APPL_SOCMEM, (BSL_META_U(unit, "soc_dpp_cache_enable_init: unit %d specific cache enable failed\n"), unit));
    }
    if (disable_specific)
    {
        cache_enable = 0;
        if (soc_mem_iterate(unit, dnxf_tbl_mem_cache_enable_specific_tbl, &cache_enable) < 0)
            LOG_ERROR(BSL_LS_APPL_SOCMEM, (BSL_META_U(unit, "soc_dpp_cache_enable_init: unit %d specific cache disable failed\n"), unit));
    }

    SHR_FUNC_EXIT;
}

int
soc_dnxf_init_reset(
    int unit)
{
    int rc = _SHR_E_NONE;
    soc_control_t *soc;
    DNXF_UNIT_LOCK_INIT(unit);

    SHR_FUNC_INIT_VARS(unit);

    DNXF_UNIT_LOCK_TAKE(unit);

    soc = SOC_CONTROL(unit);

    
    SOC_CMCS_NUM(unit) = CMIC_CMC_NUM_MAX;
    soc_endian_config(unit);

    
    rc = soc_dnxf_info_soc_properties(unit);
    SHR_IF_ERR_EXIT(rc);

    
    SHR_IF_ERR_EXIT(soc_dnxc_perform_system_reset_if_needed(unit));

    
    soc_dnxf_tbl_mark_cachable(unit);

    rc = soc_dnxf_cache_enable_init(unit);
    SHR_IF_ERR_EXIT(rc);

    
    rc = soc_dnxf_warm_boot_init(unit);
    SHR_IF_ERR_EXIT(rc);

    rc = MBCM_DNXF_DRIVER_CALL_NO_ARGS(unit, mbcm_dnxf_linkctrl_init);
    SHR_IF_ERR_EXIT(rc);

    
    if (!SOC_WARM_BOOT(unit) && !SOC_IS_RELOADING(unit))
    {
        SOC_DNXF_DRV_INIT_LOG(unit, "Device Reset");
        rc = MBCM_DNXF_DRIVER_CALL_NO_ARGS(unit, mbcm_dnxf_reset_device);
        SHR_IF_ERR_EXIT(rc);

    }
    else
    {
        
#ifdef BCM_WARM_BOOT_SUPPORT
        uint32 value;
        
        SHR_IF_ERR_EXIT(dnxf_state.intr.storm_nominal.get(unit, &value));
        soc->switch_event_nominal_storm = value;
#endif 
        rc = soc_dnxc_interrupts_init(unit);
        SHR_IF_ERR_CONT(rc);
    }

    
    rc = soc_counter_attach(unit);
    SHR_IF_ERR_EXIT(rc);

    rc = MBCM_DNXF_DRIVER_CALL_NO_ARGS(unit, mbcm_dnxf_port_soc_init);
    SHR_IF_ERR_EXIT(rc);

    soc->soc_flags |= SOC_F_INITED;

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    if (SHR_FAILURE(_func_rv))
    {
        soc_dnxf_deinit(unit);
    }
    SHR_FUNC_EXIT;
}


static int
soc_dnxf_shutdown_set(
    int unit,
    soc_pbmp_t active_links,
    int shutdown,
    int isolate_device)
{
    int rv;
    soc_port_t port;
    SHR_FUNC_INIT_VARS(unit);

    if (shutdown)
    {

        
        rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_topology_isolate_set,
                                   (unit, soc_dnxc_isolation_status_isolated));
        SHR_IF_ERR_EXIT(rv);

        
        SOC_PBMP_ITER(active_links, port)
        {
            rv = soc_dnxc_port_enable_set(unit, port, 0);
            SHR_IF_ERR_EXIT(rv);
        }

        sal_usleep(50000);      

    }
    else
    {   

        
        SOC_PBMP_ITER(active_links, port)
        {
            rv = soc_dnxc_port_enable_set(unit, port, 1);
            SHR_IF_ERR_EXIT(rv);
        }

        sal_usleep(500000);     

        if (!isolate_device)
        {
            rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_topology_isolate_set,
                                       (unit, soc_dnxc_isolation_status_active));
            SHR_IF_ERR_EXIT(rv);
        }

    }

exit:
    SHR_FUNC_EXIT;
}


int
soc_dnxf_init_no_reset(
    int unit)
{
    int rc = _SHR_E_NONE;
    soc_control_t *soc;
    DNXF_UNIT_LOCK_INIT(unit);

    soc_pbmp_t pbmp_enabled;
    soc_port_t port;
    int enable;
    soc_dnxc_isolation_status_t device_isolation_status;

    SHR_FUNC_INIT_VARS(unit);

    DNXF_UNIT_LOCK_TAKE(unit);

    soc = SOC_CONTROL(unit);

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

    soc->soc_flags |= SOC_F_INITED;

exit:
    if (!SOC_WARM_BOOT(unit) && !SOC_IS_RELOADING(unit))
    {
        
        SHR_IF_ERR_EXIT(soc_dnxc_interrupt_all_enable_set(unit, 1));
    }
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;

}

int
soc_dnxf_init(
    int unit,
    int reset)
{
    int rc = _SHR_E_NONE;
    SHR_FUNC_INIT_VARS(unit);

    if (reset)
    {
        rc = soc_dnxf_init_reset(unit);
        SHR_IF_ERR_EXIT(rc);
    }
    else
    {
        rc = soc_dnxf_init_no_reset(unit);
        SHR_IF_ERR_EXIT(rc);
    }

exit:

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
        return &soc_driver_bcm88790_a0;
    }
    else
    {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "soc_dnxf_chip_driver_find: driver in devid table "
                              "not in soc_base_driver_table\n")));
    }

    return NULL;
}


int
soc_dnxf_info_config(
    int unit,
    int dev_id)
{
    soc_info_t *si;
    soc_control_t *soc;
    int mem, blk, blktype;
    char instance_string[3];
    int rv, port, phy_port, bindex;

    SHR_FUNC_INIT_VARS(unit);
    soc = SOC_CONTROL(unit);

    
    if (SOC_IS_RAMON_TYPE(dev_id))
    {
        SOC_CHIP_STRING(unit) = "ramon";
    } 
    else
    {
            SOC_CHIP_STRING(unit) = "???";
            LOG_ERROR(BSL_LS_SOC_INIT,
                      (BSL_META_U(unit, "soc_dnxf_info_config: driver device %04x unexpected\n"), dev_id));
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

    for (blk = 0; SOC_BLOCK_INFO(unit, blk).type >= 0; blk++)
    {
        blktype = SOC_BLOCK_INFO(unit, blk).type;

        if (blk >= SOC_MAX_NUM_BLKS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "soc_dnxf_info_config: too much blocks for device \n");
        }
        si->has_block[blk] = blktype;
        sal_snprintf(instance_string, sizeof(instance_string), "%d", SOC_BLOCK_INFO(unit, blk).number);

        rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_block_valid_get,
                                   (unit, blktype, SOC_BLOCK_INFO(unit, blk).number, &(si->block_valid[blk])));
        SHR_IF_ERR_EXIT(rv);

        switch (blktype)
        {
            case SOC_BLK_ECI:
                si->eci_block = blk;
                break;
            case SOC_BLK_MESH_TOPOLOGY:
                si->mesh_topology_block = blk;
                break;
            case SOC_BLK_CMIC:
                si->cmic_block = blk;
                break;
            case SOC_BLK_IPROC:
                si->iproc_block = blk;
                break;
            case SOC_BLK_FMAC:
                if (SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_FMAC_BLKS)
                {
                    si->fmac_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                    si->block_port[blk] = SOC_BLOCK_INFO(unit, blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "soc_dnxf_info_config: too much FMAC blocks");
                }
                break;
            case SOC_BLK_FSRD:
                if (SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_FSRD_BLKS)
                {
                    si->fsrd_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                    si->block_port[blk] = SOC_BLOCK_INFO(unit, blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "soc_dnxf_info_config: too much FSRD blocks");
                }
                break;
            case SOC_BLK_RTP:
                si->rtp_block = blk;
                break;
            case SOC_BLK_OCCG:
                si->occg_block = blk;
                break;
            case SOC_BLK_DCH:
                if (SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_DCH_BLKS)
                {
                    si->dch_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                    si->block_port[blk] = SOC_BLOCK_INFO(unit, blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "soc_dnxf_info_config: too much DCH blocks");
                }
                break;
            case SOC_BLK_DCML:
                if (SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_DCML_BLKS)
                {
                    si->dcml_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                    si->block_port[blk] = SOC_BLOCK_INFO(unit, blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "soc_dnxf_info_config: too much DCML blocks");
                }
                break;
            case SOC_BLK_MCT:
                si->mct_block = blk;
                break;
            case SOC_BLK_QRH:
                if (SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_QRH_BLKS)
                {
                    si->qrh_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                    si->block_port[blk] = SOC_BLOCK_INFO(unit, blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "soc_dnxf_info_config: too much QRH blocks");
                }
                break;
            case SOC_BLK_CCH:
                if (SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_CCH_BLKS)
                {
                    si->cch_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                    si->block_port[blk] = SOC_BLOCK_INFO(unit, blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "soc_dnxf_info_config: too much CCH blocks");
                }
                break;
            case SOC_BLK_LCM:
                if (SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_LCM_BLKS)
                {
                    si->lcm_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                    si->block_port[blk] = SOC_BLOCK_INFO(unit, blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "soc_dnxf_info_config: too much LCM blocks");
                }
                break;
            case SOC_BLK_BRDC_FSRD:
                si->brdc_fsrd_block = blk;
                si->broadcast_blocks[blk] = si->fsrd_block;
                si->broadcast_blocks_size[blk] = SOC_MAX_NUM_FSRD_BLKS;
                break;
            case SOC_BLK_BRDC_DCH:
                si->brdc_dch_block = blk;
                si->broadcast_blocks[blk] = si->dch_block;
                si->broadcast_blocks_size[blk] = SOC_MAX_NUM_DCH_BLKS;
                break;
            case SOC_BLK_BRDC_FMAC:
                si->brdc_fmac_block = blk;
                si->broadcast_blocks[blk] = si->fmac_block;
                si->broadcast_blocks_size[blk] = SOC_MAX_NUM_FMAC_BLKS;
                break;
            case SOC_BLK_BRDC_CCH:
                si->brdc_cch_block = blk;
                si->broadcast_blocks[blk] = si->cch_block;
                si->broadcast_blocks_size[blk] = SOC_MAX_NUM_CCH_BLKS;
                break;
            case SOC_BLK_BRDC_DCML:
                si->brdc_dcml_block = blk;
                si->broadcast_blocks[blk] = si->dcml_block;
                si->broadcast_blocks_size[blk] = SOC_MAX_NUM_DCML_BLKS;
                break;
            case SOC_BLK_BRDC_LCM:
                si->brdc_lcm_block = blk;
                si->broadcast_blocks[blk] = si->lcm_block;
                si->broadcast_blocks_size[blk] = SOC_MAX_NUM_LCM_BLKS;
                break;
            case SOC_BLK_BRDC_QRH:
                si->brdc_qrh_block = blk;
                si->broadcast_blocks[blk] = si->qrh_block;
                si->broadcast_blocks_size[blk] = SOC_MAX_NUM_QRH_BLKS;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "soc_dnxf_info_config: unknown block type");
                break;
        }

        sal_snprintf(si->block_name[blk], sizeof(si->block_name[blk]),
                     "%s%s", soc_block_name_lookup_ext(blktype, unit), instance_string);
    }
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
                sal_sprintf(SOC_PORT_NAME(unit, port), "CMIC");
                SOC_PBMP_PORT_ADD(si->cmic_bitmap, port);
                break;
            default:
                si->port_num_lanes[port] = 1;
                sal_sprintf(SOC_PORT_NAME(unit, port), "sfi%d", port);
                sal_sprintf(SOC_PORT_NAME_ALTER(unit, port), "fabric%d", port);
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

    
    if ((soc_mem_dmaable(unit, RTP_MULTI_CAST_TABLE_UPDATEm, SOC_MEM_BLOCK_ANY(unit, RTP_MULTI_CAST_TABLE_UPDATEm)) || soc_mem_slamable(unit, RTP_MULTI_CAST_TABLE_UPDATEm, SOC_MEM_BLOCK_ANY(unit, RTP_MULTI_CAST_TABLE_UPDATEm))) &&  
        soc_feature(unit, soc_feature_sbusdma))
    {

        SHR_IF_ERR_EXIT(sand_deinit_fill_table(unit));

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

    
     if (SOC_IS_RAMON_TYPE(dev_id))
     {
        si->chip_type = SOC_INFO_CHIP_TYPE_RAMON;
     } else {
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
    static int default_fw_loaded[SOC_MAX_NUM_DEVICES] = {0};
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
        if (soc->soc_flags & SOC_F_ATTACHED)
        {
            SHR_EXIT();
        }
    }

    soc->soc_link_pause = 0;

    SOC_PCI_CMCS_NUM(unit) = soc_property_uc_get(unit, 0, spn_PCI_CMCS_NUM, 1);

    soc_cm_get_id(unit, &dev_id, &rev_id);
    
    if ((CMDEV(unit).dev.info->rev_id & SOC_CM_DEVICE_ANY_LAST_DEVID_DIGIT) != 0) {
        uint32 rev_id_f = CMDEV(unit).dev.info->rev_id & SOC_CM_REVID_MASK;
        uint8 last_revid_digit = (rev_id_f - 1) & 0xf;
        sal_snprintf(SOC_CONTROL(unit)->device_name, sizeof(SOC_CONTROL(unit)->device_name) - 1, "BCM8%.4X_%c%c", (unsigned)CMDEV(unit).dev.dev_id, 'a' + (rev_id_f >> 4), (last_revid_digit < 10 ? ('0' + last_revid_digit) : ('a' - 10) + last_revid_digit));
    }
    SHR_IF_ERR_EXIT(soc_dnxf_chip_type_set(unit, dev_id));

    
    soc->chip_driver = soc_dnxf_chip_driver_find(unit, dev_id, rev_id);
    if (soc->chip_driver == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "unit has no driver (device 0x%04x rev 0x%02x)", dev_id, rev_id);
    }

    
    soc_feature_init(unit);

    
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

    si = &SOC_INFO(unit);
    si->driver_type = soc->chip_driver->type;
    si->driver_group = soc_chip_type_map[si->driver_type];

    
    soc->soc_flags |= SOC_F_ATTACHED;

    
    rc = dnxf_data_init(unit);
    SHR_IF_ERR_EXIT(rc);

    
    rc = mbcm_dnxf_init(unit);
    if (rc != _SHR_E_NONE)
    {
        LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "soc_dnxf_init error in mbcm_dnxf_init\n")));
    }
    SHR_IF_ERR_EXIT(rc);

    
    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_controlled_counter_set, (unit));
    SHR_IF_ERR_EXIT(rc);

    
    
    rc = soc_dnxf_control_init(unit);
    SHR_IF_ERR_EXIT(rc);

    
    soc->soc_flags |= SOC_F_ATTACHED;

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
    
    if ((soc_mem_dmaable(unit, RTP_MULTI_CAST_TABLE_UPDATEm, SOC_MEM_BLOCK_ANY(unit, RTP_MULTI_CAST_TABLE_UPDATEm)) || soc_mem_slamable(unit, RTP_MULTI_CAST_TABLE_UPDATEm, SOC_MEM_BLOCK_ANY(unit, RTP_MULTI_CAST_TABLE_UPDATEm))) &&  
        soc_feature(unit, soc_feature_sbusdma))
    {

        
        SHR_IF_ERR_EXIT(soc_dma_attach(unit, 1  ));

        

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
    if (SHR_FAILURE(_func_rv))
    {
        LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "soc_dnxf_attach: unit %d failed (%s)\n"), unit, soc_errmsg(rc)));
        soc_dnxf_detach(unit);
    }

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
    LOG_CLI((BSL_META_U(unit, "%sFlags=0x%x:"), pfx, soc->soc_flags));
    if (soc->soc_flags & SOC_F_ATTACHED)
    {
        LOG_CLI((BSL_META_U(unit, " attached")));
    }
    if (soc->soc_flags & SOC_F_INITED)
    {
        LOG_CLI((BSL_META_U(unit, " initialized")));
    }
    if (soc->soc_flags & SOC_F_LSE)
    {
        LOG_CLI((BSL_META_U(unit, " link-scan")));
    }
    if (soc->soc_flags & SOC_F_SL_MODE)
    {
        LOG_CLI((BSL_META_U(unit, " sl-mode")));
    }
    if (soc->soc_flags & SOC_F_POLLED)
    {
        LOG_CLI((BSL_META_U(unit, " polled")));
    }
    if (soc->soc_flags & SOC_F_URPF_ENABLED)
    {
        LOG_CLI((BSL_META_U(unit, " urpf")));
    }
    if (soc->soc_flags & SOC_F_MEM_CLEAR_USE_DMA)
    {
        LOG_CLI((BSL_META_U(unit, " mem-clear-use-dma")));
    }
    if (soc->soc_flags & SOC_F_IPMCREPLSHR)
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


void soc_dnxf_free_cache_memory(int unit)
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
soc_dnxf_deinit(
    int unit)
{
    int rc;
    soc_control_t *soc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "invalid unit");
    }

    DNXF_UNIT_LOCK_TAKE(unit);

    soc = SOC_CONTROL(unit);

    soc->soc_flags &= ~SOC_F_INITED;

    if (SOC_FAILURE(soc_linkctrl_deinit(unit)))
    {
        LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "Failed in soc_linkctrl_deinit\n")));
    }

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx) && !SAL_BOOT_PLISIM)
    {
        
        soc_iproc_m0_exit(unit);
    }
#endif 
    
    rc = soc_dnxf_warm_boot_deinit(unit);
    SHR_IF_ERR_CONT(rc);

    
    rc = soc_dnxc_interrupts_deinit(unit);
    SHR_IF_ERR_CONT(rc);

    
    rc = soc_counter_detach(unit);
    if (SOC_FAILURE(rc))
    {
        LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "Failed to detach counter\n")));
    }

    
    soc_dnxf_free_cache_memory(unit);

    
    
    rc = soc_dnxf_control_init(unit);
    SHR_IF_ERR_CONT(rc);

exit:

    DNXF_UNIT_LOCK_RELEASE(unit);

    
    if (SOC_UNIT_NUM_VALID(unit))
    {
        SOC_DETACH(unit, 0);
    }

    SHR_FUNC_EXIT;
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
    int rv = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    switch (mode)
    {
        case SOC_DNXC_RESET_MODE_BLOCKS_RESET:
            rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_blocks_reset, (unit, action, NULL));
            SHR_IF_ERR_EXIT(rv);
            break;
        case SOC_DNXC_RESET_MODE_BLOCKS_SOFT_RESET:
            rv = soc_dnxf_init(unit, 0);
            SHR_IF_ERR_EXIT(rv);
            break;
        case SOC_DNXC_RESET_MODE_INIT_RESET:
            rv = soc_dnxf_init(unit, 1);
            SHR_IF_ERR_EXIT(rv);
            break;
        case SOC_DNXC_RESET_MODE_REG_ACCESS:
            
            rv = soc_dnxf_info_soc_properties(unit);
            SHR_IF_ERR_EXIT(rv);
            rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_reg_access_only_reset, (unit));
            SHR_IF_ERR_EXIT(rv);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown/Unsupported Reset Mode");
    }

exit:
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

#undef BSL_LOG_MODULE
