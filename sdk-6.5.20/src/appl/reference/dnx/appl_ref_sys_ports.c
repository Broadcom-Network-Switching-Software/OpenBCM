/** \file appl_ref_sys_ports.c
 * 
 *
 * System ports level application procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INITSEQDNX

 /*
  * Include files.
  * {
  */
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/types.h>
#include <sal/core/libc.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/property.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm/stack.h>
#include <bcm/port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include "appl_ref_sys_device.h"
#include <appl/reference/dnx/appl_ref_sys_ports.h>
#include <appl/reference/dnx/appl_ref_e2e_scheme.h>
#include <appl/reference/sand/appl_ref_sand.h>
#include "appl_ref_l2_init.h"
#include "appl_ref_sys_db.h"
#include <bcm_int/dnx/tdm/tdm.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

 /*
  * Global and Static
  */
/*
 * }
 */

/*
 * See .h files
 */
int
appl_dnx_logical_port_to_sysport_get(
    int unit,
    bcm_module_t modid,
    bcm_port_t port,
    int *sysport)
{
    int device_index;
    int core;
    appl_dnx_sys_device_t *sys_params;

    SHR_FUNC_INIT_VARS(unit);

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** convert modid to device index */
    SHR_IF_ERR_EXIT(appl_dnx_modid_to_fap_and_core_convert(unit, modid, &device_index, &core));

    SHR_IF_ERR_EXIT(appl_sand_device_to_sysport_convert(unit, device_index,
                                                        APPL_DNX_NOF_SYSPORTS_PER_DEVICE, port, sysport));

exit:
    SHR_FUNC_EXIT;
}

/** See .h files */
shr_error_e
appl_dnx_sys_ports_port_create_and_connect(
    int unit,
    int system_port_id,
    const system_port_mapping_t * sys_port_map)
{
    SHR_FUNC_INIT_VARS(unit);

    /** create system port */
    SHR_IF_ERR_EXIT(appl_dnx_sys_ports_port_create(unit, system_port_id, sys_port_map));
    /** create VOQs */
    SHR_IF_ERR_EXIT(appl_dnx_sys_port_voq_create(unit, system_port_id, sys_port_map));
    /** connect VOQ to VOQ connector */
    SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_voq_connect(unit, system_port_id, sys_port_map, 0 /** is_disconnect */ ));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_dnx_sys_ports_port_create(
    int unit,
    int system_port_id,
    const system_port_mapping_t * sys_port_map)
{
    bcm_gport_t sysport_gport;
    bcm_gport_t modport_gport;
    int modid;
    int ftmh_pp_dsp;

    SHR_FUNC_INIT_VARS(unit);

    /** Map system port to physical port (modport) */
    SHR_IF_ERR_EXIT(appl_dnx_fap_and_tm_port_to_modport_convert(unit, sys_port_map->device_index, sys_port_map->core,
                                                                sys_port_map->tm_port, &modid, &ftmh_pp_dsp));
    BCM_GPORT_MODPORT_SET(modport_gport, modid, ftmh_pp_dsp);
    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, system_port_id);
    SHR_IF_ERR_EXIT(bcm_stk_sysport_gport_set(unit, sysport_gport, modport_gport));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Destroy VOQ
 */
shr_error_e
appl_dnx_sys_ports_voq_destroy(
    int unit,
    int system_port_id)
{
    bcm_gport_t voq_gport;
    int voq_base_id;

    SHR_FUNC_INIT_VARS(unit);

    /** convert system port id to base voq id */
    SHR_IF_ERR_EXIT(appl_sand_sysport_id_to_base_voq_id_convert(unit, system_port_id, &voq_base_id));

    /** Destroy the queue bundle */
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport, voq_base_id);
    SHR_IF_ERR_EXIT(bcm_cosq_gport_delete(unit, voq_gport));

exit:
    SHR_FUNC_EXIT;

}

/** see header file */
shr_error_e
appl_dnx_sys_port_voq_create(
    int unit,
    int system_port_id,
    const system_port_mapping_t * sys_port_map)
{
    int delay_tolerance_level;
    int rate_class;
    bcm_gport_t sysport_gport;
    int voq_base_id;
    bcm_gport_t voq_gport;
    bcm_cosq_ingress_queue_bundle_gport_config_t queue_bundle_config;
    bcm_cosq_pkt_size_adjust_info_t adjust_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Set credit request profile according to port's speed  */
    SHR_IF_ERR_EXIT(appl_dnx_e2e_voq_profiles_get(unit, sys_port_map->rate, &delay_tolerance_level, &rate_class));

    
    /** get value of SOC property "tdm_mode" */
    if (dnx_data_tdm.params.mode_get(unit) == TDM_MODE_PACKET)
    {
        uint32 port_flags;
        bcm_port_interface_info_t interface_info;
        bcm_port_mapping_info_t mapping_info;
        /** IN TDM Packet mode, set VOQs for TDM ports to be push queues */
        SHR_IF_ERR_EXIT(bcm_port_get(unit, sys_port_map->local_port, &port_flags, &interface_info, &mapping_info));
        if (port_flags & BCM_PORT_ADD_TDM_PORT)
        {
            SHR_IF_ERR_EXIT(appl_dnx_e2e_tdm_delay_tolerance_get(unit, &delay_tolerance_level));
        }
    }

    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, system_port_id);

    /** convert system port id to base voq id */
    SHR_IF_ERR_EXIT(appl_sand_sysport_id_to_base_voq_id_convert(unit, system_port_id, &voq_base_id));

    /** Allocate the queue bundle and map system port to the base voq */
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport, voq_base_id);

    sal_memset(&queue_bundle_config, 0, sizeof(bcm_cosq_ingress_queue_bundle_gport_config_t));
    queue_bundle_config.flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | BCM_COSQ_GPORT_WITH_ID;
    queue_bundle_config.local_core_id = BCM_CORE_ALL; /** Irrelevant in case of WITH_ID allocation */
    queue_bundle_config.numq = APPL_SAND_E2E_SCHEME_COS;
    queue_bundle_config.port = sysport_gport;
    for (int cos = 0; cos < queue_bundle_config.numq; cos++)
    {
        queue_bundle_config.queue_atrributes[cos].delay_tolerance_level = delay_tolerance_level;
        queue_bundle_config.queue_atrributes[cos].rate_class = rate_class;
    }

    /** allocate voqs bundle */
    SHR_IF_ERR_EXIT(bcm_cosq_ingress_queue_bundle_gport_add(unit, &queue_bundle_config, &voq_gport));

    for (int cos = 0; cos < queue_bundle_config.numq; ++cos)
    {
        /** Configure Compensation - constant value of 24 bytes (Preamable, IFG, CRC) assuming regular NIF port */
        adjust_info.gport = voq_gport;
        adjust_info.source_info.source_type = bcmCosqPktSizeAdjustSourceGlobal;         /** compensation type*/
        adjust_info.flags = 0;
        adjust_info.cosq = cos;
        adjust_info.source_info.source_id = 0;         /** irrelevant */

        /** Set per queue compensation */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_pkt_size_adjust_set(unit, &adjust_info, 24));
    }

exit:
    SHR_FUNC_EXIT;
}

/** CB function for system port DB iterator */
static shr_error_e
appl_dnx_system_port_create_and_connect_cb(
    int unit,
    int system_port_id,
    const system_port_mapping_t * sys_port_map_info,
    void *opaque_data_pointer)
{
    SHR_FUNC_INIT_VARS(unit);

    /** create system port and VOQs, and connect VOQs to VOQ connectors */
    SHR_IF_ERR_EXIT(appl_dnx_sys_ports_port_create_and_connect(unit, system_port_id, sys_port_map_info));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_dnx_sys_ports_init(
    int unit)
{
    appl_dnx_sys_device_t *sys_params;

    SHR_FUNC_INIT_VARS(unit);

    /** create credit request profiles - to be used when creating ingress queues*/
    SHR_IF_ERR_EXIT(appl_dnx_e2e_credit_request_profiles_create(unit));

    /** create rate class profiles - to be used when creating ingress queues*/
    SHR_IF_ERR_EXIT(appl_dnx_e2e_rate_class_profiles_create(unit));

    /** Create FMQs (fabric multicast queues) e2e scheduling scheme before allocating any VoQs */
    SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_mcast_create(unit));

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** for stand alone configuration run only on local ports */
    if (sys_params->stand_alone_configuration)
    {
        bcm_pbmp_t pbmp;
        bcm_port_t port;
        bcm_gport_t olp_port = -1;

        /** get ports */
        SHR_IF_ERR_EXIT(appl_dnx_sys_device_ports_get(unit, &pbmp));

        /** get olp port */
        SHR_IF_ERR_EXIT(appl_dnx_olp_port_get(unit, &olp_port));

        /** iterate on ports and add them */
        BCM_PBMP_ITER(pbmp, port)
        {
            uint32 flags;
            bcm_port_interface_info_t interface_info;
            bcm_port_mapping_info_t mapping_info;
            bcm_port_resource_t port_resource;
            system_port_mapping_t sys_port_map = { 0 };
            int system_port_id;

            SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, port, &port_resource));
            SHR_IF_ERR_EXIT(bcm_port_get(unit, port, &flags, &interface_info, &mapping_info));
            sys_port_map.device_index = sys_params->unit_to_device_index[unit];
            sys_port_map.local_port = port;
            sys_port_map.core = mapping_info.core;
            sys_port_map.tm_port = mapping_info.tm_port;
            sys_port_map.rate = port_resource.speed;
            if (flags & BCM_PORT_ADD_TDM_PORT)
            {
                sys_port_map.port_type = APPL_PORT_TYPE_TDM;
            }
            else if (port == olp_port)
            {
                sys_port_map.port_type = APPL_PORT_TYPE_OLP;
            }

            /** Convert device and logical port to system port id */
            SHR_IF_ERR_EXIT(appl_sand_device_to_sysport_convert
                            (unit, sys_port_map.device_index, APPL_DNX_NOF_SYSPORTS_PER_DEVICE,
                             sys_port_map.local_port, &system_port_id));

            /** create system port and VOQs, and connect VOQs to VOQ connectors */
            SHR_IF_ERR_EXIT(appl_dnx_sys_ports_port_create_and_connect(unit, system_port_id, &sys_port_map));
        }

    }
    else
    {
        /** iterate over all system ports, create and connect them */
        SHR_IF_ERR_EXIT(appl_dnx_system_port_db_iterate(unit, sys_params->system_port_db,
                                                        appl_dnx_system_port_create_and_connect_cb, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}
