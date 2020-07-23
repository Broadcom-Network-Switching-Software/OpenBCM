/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC DFE warm_boot
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_INIT

#include <shared/bsl.h>
#include <soc/error.h>
#include <soc/wb_engine.h>
#include <soc/scache.h>
#include <soc/ha.h>

#include <sal/compiler.h>

#ifdef BCM_DFE_SUPPORT

#include <soc/dcmn/error.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_warm_boot.h>


#define DFE_VER(_ver) (_ver)


soc_error_t
soc_dfe_warm_boot_sync(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    SOCDNX_IF_ERR_EXIT(soc_wb_engine_sync(unit, SOC_DFE_WARM_BOOT_ENGINE));
#else
    SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Warm boot support requires compilation with warm boot flag.\n")));
#endif 

exit:  
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dfe_warm_boot_deinit(int unit)
{
    int rv;
#if defined(BCM_WARM_BOOT_SUPPORT) && !defined(__KERNEL__) && defined (LINUX)
    int stable_location;
    uint32 stable_flags;
#endif

    SOCDNX_INIT_FUNC_DEFS;
    
    rv = soc_wb_engine_deinit_tables(unit, SOC_DFE_WARM_BOOT_ENGINE);
    SOCDNX_IF_ERR_EXIT(rv);

#ifdef BCM_WARM_BOOT_SUPPORT
    rv = soc_scache_detach(unit);
    SOCDNX_IF_ERR_EXIT(rv);
#if !defined(__KERNEL__) && defined (LINUX)
    SOCDNX_IF_ERR_RETURN(soc_stable_get(unit,&stable_location,&stable_flags));
    if (stable_location == 4) {
        rv = ha_destroy(unit);
        SOCDNX_IF_ERR_CONT(rv);
    }
#endif
#endif

exit: 
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dfe_warm_boot_init(int unit)
{
    int rv,
        is_supported,
        buffer_id;
#if defined(BCM_WARM_BOOT_SUPPORT) && !defined(__KERNEL__) && defined (LINUX)
    int stable_location;
    uint32 stable_flags;
    uint32 stable_size;
#endif

    SOCDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
#if !defined(__KERNEL__) && defined (LINUX)
    stable_size = soc_property_get(unit, spn_STABLE_SIZE, 100000000);

    
    SOCDNX_IF_ERR_RETURN(soc_stable_get(unit,&stable_location,&stable_flags));
    if (stable_location == 4) {
        char *ha_name = soc_property_get_str(unit, "stable_filename");
        
        SOCDNX_IF_ERR_RETURN(ha_init(unit, ha_name, stable_size, SOC_WARM_BOOT(unit) ? 0 : 1));
    }
#endif

    
    
    if (SOC_WARM_BOOT(unit)) 
    {
        rv = soc_scache_recover(unit);
        if (SOC_FAILURE(rv)) 
        {
            
            SOCDNX_IF_ERR_RETURN(soc_stable_size_set(unit, 0));
            SOCDNX_IF_ERR_RETURN(soc_stable_set(unit, _SHR_SWITCH_STABLE_NONE, 0));
            
            SOCDNX_IF_ERR_RETURN
                (soc_event_generate(unit, SOC_SWITCH_EVENT_STABLE_ERROR, 
                                    SOC_STABLE_CORRUPT,
                                    SOC_STABLE_FLAGS(unit), 0));
            LOG_VERBOSE(BSL_LS_SOC_INIT,
                        (BSL_META_U(unit,
                                    "Unit %d: Corrupt stable cache.\n"),
                                    unit));
        }
    }
#endif  

    SOCDNX_IF_ERR_EXIT(soc_wb_engine_init_tables(unit, SOC_DFE_WARM_BOOT_ENGINE , SOC_DFE_WARM_BOOT_BUFFER_NOF, SOC_DFE_WARM_BOOT_VAR_NOF)); 

    
    for (buffer_id = 0; buffer_id < SOC_DFE_WARM_BOOT_BUFFER_NOF; buffer_id++)
    {
        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_warm_boot_buffer_id_supported_get, (unit, buffer_id, &is_supported));
        SOCDNX_IF_ERR_EXIT(rv);

        if (is_supported)
        {
            rv = soc_dfe_warm_boot_buffer_id_create(unit, buffer_id);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

    
    rv = SOC_DFE_WARM_BOOT_VAR_GET(unit, INTR_STORM_NOMINAL, &SOC_SWITCH_EVENT_NOMINAL_STORM(unit));
    SOCDNX_IF_ERR_EXIT(rv);


exit: 
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dfe_warm_boot_buffer_id_create(int unit, int buffer_id)
{
    int rc = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;

     
    rc = soc_dfe_warm_boot_engine_init_buffer_struct(unit, buffer_id);
    SOCDNX_IF_ERR_EXIT(rc);

    rc = soc_wb_engine_init_buffer(unit, SOC_DFE_WARM_BOOT_ENGINE, buffer_id, FALSE);
    if (rc == SOC_E_NOT_FOUND) {
        
        
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "Failed to recover buffer (%d) warm boot data"), buffer_id));

        
        SOCDNX_IF_ERR_EXIT(rc);
    } else {
        SOCDNX_IF_ERR_EXIT(rc);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dfe_warm_boot_engine_init_buffer_struct(int unit, int buffer_id)
{
    int rv,
        nof_links,
        nof_macs,
        nof_interrupts;
    uint32 mc_table_size;
    WB_ENGINE_INIT_TABLES_DEFS;
    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(&buffer_is_dynamic); 

    
    nof_links = SOC_DFE_DEFS_GET(unit, nof_links);
    nof_macs = SOC_DFE_DEFS_GET(unit, nof_instances_mac);
    nof_interrupts = SOC_DFE_DEFS_GET(unit, nof_interrupts);
    rv = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_multicast_table_size_get,(unit, &mc_table_size));
    SOCDNX_IF_ERR_EXIT(rv);

    switch (buffer_id)
    {
        case SOC_DFE_WARM_BOOT_BUFFER_MODID:
            
            SOC_WB_ENGINE_ADD_BUFF(SOC_DFE_WARM_BOOT_ENGINE, buffer_id, "Fabric modid buffer", NULL, NULL, DFE_VER(1), 1, SOC_WB_ENGINE_PRE_RELEASE);
            SOCDNX_IF_ERR_EXIT(rv);

            
            SOC_WB_ENGINE_ADD_ARR(SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_MODID_LOCAL_MAP,          "Local modids mapping",             buffer_id, sizeof(soc_dfe_modid_local_map_t),   NULL, SOC_DFE_MODID_LOCAL_NOF(unit),        DFE_VER(1));
            SOCDNX_IF_ERR_EXIT(rv);
            SOC_WB_ENGINE_ADD_ARR(SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_MODID_GROUP_MAP,          "Group modids mapping",             buffer_id, sizeof(soc_dfe_modid_group_map_t),   NULL, SOC_DFE_MODID_GROUP_NOF(unit),        DFE_VER(1));
            SOCDNX_IF_ERR_EXIT(rv);
            SOC_WB_ENGINE_ADD_ARR(SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_MODID_MODID_TO_GROUP_MAP, "Modid to group modids mapping",    buffer_id, sizeof(soc_module_t),                NULL, SOC_DFE_MODID_NOF,                    DFE_VER(1));
            SOCDNX_IF_ERR_EXIT(rv);
            break;

        case SOC_DFE_WARM_BOOT_BUFFER_MC:
            
            SOC_WB_ENGINE_ADD_BUFF(SOC_DFE_WARM_BOOT_ENGINE, buffer_id, "MC buffer", NULL, NULL, DFE_VER(1), 1, SOC_WB_ENGINE_PRE_RELEASE);
            SOCDNX_IF_ERR_EXIT(rv);

            
            SOC_WB_ENGINE_ADD_VAR(SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_MC_MODE,                   "MC Mode",                          buffer_id, sizeof(uint32),                      NULL,                                       DFE_VER(1));
            SOCDNX_IF_ERR_EXIT(rv);
            SOC_WB_ENGINE_ADD_ARR(SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_MC_ID_MAP,                "MC_MAP",                           buffer_id, sizeof(uint32),                      NULL, _shr_div32r(mc_table_size, 32),       DFE_VER(1));
            SOCDNX_IF_ERR_EXIT(rv);
            break;

        case SOC_DFE_WARM_BOOT_BUFFER_FIFO:
            
            SOC_WB_ENGINE_ADD_BUFF(SOC_DFE_WARM_BOOT_ENGINE, buffer_id, "FIFO handles", NULL, NULL, DFE_VER(1), 1, SOC_WB_ENGINE_PRE_RELEASE);
            SOCDNX_IF_ERR_EXIT(rv);

            
            SOC_WB_ENGINE_ADD_ARR(SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_FIFO_HANDLERS,             "Fifo handles",                     buffer_id, sizeof(soc_dfe_fifo_type_handle_t),  NULL, soc_dfe_fabric_nof_link_fifo_types,  DFE_VER(1));
            SOCDNX_IF_ERR_EXIT(rv);
            break;

        case SOC_DFE_WARM_BOOT_BUFFER_PORT:
            
            SOC_WB_ENGINE_ADD_BUFF(SOC_DFE_WARM_BOOT_ENGINE, buffer_id, "Port params", NULL, NULL, DFE_VER(1), 1, SOC_WB_ENGINE_PRE_RELEASE);
            SOCDNX_IF_ERR_EXIT(rv);

            
            SOC_WB_ENGINE_ADD_ARR(SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_PORT_COMMA_BURST_CONF,    "Port Comma burst",                 buffer_id, sizeof(uint32),                      NULL, nof_links,                            DFE_VER(1));
            SOCDNX_IF_ERR_EXIT(rv);
            SOC_WB_ENGINE_ADD_ARR(SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_PORT_CTRL_BURST_CONF,     "Port Control burst",               buffer_id, sizeof(uint32),                      NULL, nof_macs,                             DFE_VER(1));
            SOCDNX_IF_ERR_EXIT(rv);
            SOC_WB_ENGINE_ADD_ARR(SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_PORT_CL72_CONF,           "Port CL72",                        buffer_id, sizeof(uint32),                      NULL, nof_links,                            DFE_VER(1));
            SOCDNX_IF_ERR_EXIT(rv);
            break;

       case SOC_DFE_WARM_BOOT_BUFFER_INTR:
            
            SOC_WB_ENGINE_ADD_BUFF(SOC_DFE_WARM_BOOT_ENGINE, buffer_id, "Interrupts params", NULL, NULL, DFE_VER(1), 1, SOC_WB_ENGINE_PRE_RELEASE);
            SOCDNX_IF_ERR_EXIT(rv);

            
            SOC_WB_ENGINE_ADD_ARR(SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_INTR_FLAGS,               "intr flags",                       buffer_id, sizeof(uint32),                      NULL, nof_interrupts,                       DFE_VER(1));
            SOCDNX_IF_ERR_EXIT(rv);
            SOC_WB_ENGINE_ADD_ARR(SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_INTR_STORM_TIMED_COUNT,   "intr storm timed count",           buffer_id, sizeof(uint32),                      NULL, nof_interrupts,                       DFE_VER(1));
            SOCDNX_IF_ERR_EXIT(rv);
            SOC_WB_ENGINE_ADD_ARR(SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_INTR_STORM_TIMED_PERIOD,  "intr storm timed period",          buffer_id, sizeof(uint32),                      NULL, nof_interrupts,                       DFE_VER(1));
            SOCDNX_IF_ERR_EXIT(rv);
            SOC_WB_ENGINE_ADD_VAR(SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_INTR_STORM_NOMINAL,       "intr storm nominal",               buffer_id, sizeof(uint32),                      NULL,                                       DFE_VER(1));
            SOCDNX_IF_ERR_EXIT(rv);
            break;
        case SOC_DFE_WARM_BOOT_BUFFER_ISOLATE:
            
            SOC_WB_ENGINE_ADD_BUFF(SOC_DFE_WARM_BOOT_ENGINE, buffer_id, "Isolation params", NULL, NULL, DFE_VER(1), 1, SOC_WB_ENGINE_PRE_RELEASE);
            SOCDNX_IF_ERR_EXIT(rv);
            
            
            SOC_WB_ENGINE_ADD_VAR(SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_ISOLATE_UNISOLATED_LINKS, "Unisolated Links bitmap",         buffer_id , sizeof(soc_pbmp_t),                  NULL,                                         DFE_VER(1));
            SOCDNX_IF_ERR_EXIT(rv);    
            SOC_WB_ENGINE_ADD_VAR(SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_ISOLATE_ACTIVE_LINKS,     "Active Links bitmap",             buffer_id , sizeof(soc_pbmp_t),                  NULL,                                         DFE_VER(1));
            SOCDNX_IF_ERR_EXIT(rv);    
            SOC_WB_ENGINE_ADD_VAR(SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_ISOLATE_ISOLATE_DEVICE,   "Isolate device flag",             buffer_id , sizeof(soc_dcmn_isolation_status_t), NULL,                                         DFE_VER(1));
            SOCDNX_IF_ERR_EXIT(rv);    
            SOC_WB_ENGINE_ADD_VAR(SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_ISOLATE_TYPE,             "Valid isolation type",            buffer_id , sizeof(uint32),                      NULL,                                         DFE_VER(1));
            SOCDNX_IF_ERR_EXIT(rv);

            break;

        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INIT, (_BSL_SOCDNX_MSG("Unsupported buffer_id %d"), buffer_id));
            break;
    }

    SOC_WB_ENGINE_INIT_TABLES_SANITY(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

#endif 
