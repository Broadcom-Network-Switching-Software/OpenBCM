/** \file appl_ref_sys_ports.c
 * 
 *
 * System ports level application procedures for DNX.
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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
#include <soc/sand/shrextend/shrextend_debug.h>
#include <sal/types.h>
#include <sal/core/libc.h>
#include <soc/defs.h>
#include <soc/property.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm/stack.h>
#include <bcm/port.h>
#include <bcm_int/dnx/port/port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_iqs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include "appl_ref_sys_device.h"
#include <appl/reference/dnx/appl_ref_sys_ports.h>
#include <appl/reference/dnx/appl_ref_e2e_scheme.h>
#include <appl/reference/sand/appl_ref_sand.h>
#include <appl/diag/dnx/diag_dnx_cmdlist.h>
#include "appl_ref_l2_init.h"
#include "appl_ref_sys_db.h"
#include <bcm_int/dnx/tdm/tdm.h>
#include <soc/dnxc/drv_dnxc_utils.h>

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

    SHR_FUNC_INIT_VARS(unit);

    /** convert modid to device index */
    SHR_IF_ERR_EXIT(appl_dnx_modid_to_fap_and_core_convert(unit, modid, &device_index, &core));

    SHR_IF_ERR_EXIT(appl_sand_device_to_sysport_convert(unit, device_index,
                                                        APPL_DNX_NOF_SYSPORTS_PER_DEVICE, port, sysport));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Construct system_port_mapping_t given local logical port
 */
int
appl_dnx_logical_port_to_sysport_map_get(
    int unit,
    bcm_port_t port,
    system_port_mapping_t * sys_port_map)
{
    appl_dnx_sys_device_t *sys_params;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_resource_t port_resource;
    bcm_gport_t olp_port = -1;

    SHR_FUNC_INIT_VARS(unit);

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, port, &port_resource));
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));
    SHR_IF_ERR_EXIT(bcm_port_get(unit, port, &flags, &interface_info, &mapping_info));
    SHR_IF_ERR_EXIT(appl_dnx_olp_port_get(unit, 0, &olp_port));

    sys_port_map->device_index = sys_params->unit_to_device_index[unit];
    sys_port_map->local_port = port;
    sys_port_map->core = mapping_info.core;
    sys_port_map->pp_dsp = mapping_info.pp_dsp;
    sys_port_map->rate = port_resource.speed;
    if (flags & BCM_PORT_ADD_TDM_PORT)
    {
        sys_port_map->port_type = APPL_PORT_TYPE_TDM;
    }
    else if (port == olp_port)
    {
        sys_port_map->port_type = APPL_PORT_TYPE_OLP;
    }

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
    SHR_IF_ERR_EXIT(appl_dnx_fap_and_pp_dsp_to_modport_convert(unit, sys_port_map->device_index, sys_port_map->core,
                                                               sys_port_map->pp_dsp, &modid, &ftmh_pp_dsp));
    BCM_GPORT_MODPORT_SET(modport_gport, modid, ftmh_pp_dsp);
    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, system_port_id);
    SHR_IF_ERR_EXIT(bcm_stk_sysport_gport_set(unit, sysport_gport, modport_gport));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - destroy voq bundle
 * 1. disable system port to bundle base mapping. This will discard incoming traffic for destination based queues.
 * 2. discard incoming traffic by mapping the bundle's voqs into a rate class with tail drop of 0.
 * 3. flush voqs in bundle.
 * 4. destroy the voq bundle.
 */
shr_error_e
appl_dnx_sys_ports_voq_destroy(
    int unit,
    int system_port_id)
{
    bcm_gport_t voq_gport;
    bcm_gport_t sysport_gport;
    bcm_gport_t physical_port;
    bcm_gport_t voq_rate_class, discard_rate_class;
    bcm_switch_profile_mapping_t profile_mapping;
    bcm_cosq_gport_size_t gport_size, discard_gport_size;
    uint32 flags;
    int num_cos_levels;
    int voq_base_id;
    int resource_index;
    int cosq;

    uint32 resources[] = { BCM_COSQ_GPORT_SIZE_BYTES, BCM_COSQ_GPORT_SIZE_OCB | BCM_COSQ_GPORT_SIZE_BUFFERS,
        BCM_COSQ_GPORT_SIZE_OCB | BCM_COSQ_GPORT_SIZE_PACKET_DESC
    };

    SHR_FUNC_INIT_VARS(unit);

    /** STEP 1 - disable system port to bundle base mapping */
    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, system_port_id);
    SHR_IF_ERR_EXIT(bcm_cosq_gport_enable_set(unit, sysport_gport, -1 /* cosq */ , 0 /* enable */ ));

    /** convert system port id to base voq id */
    SHR_IF_ERR_EXIT(appl_sand_sysport_id_to_base_voq_id_convert(unit, system_port_id, &voq_base_id));
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport, voq_base_id);

    /** get information about a bundle of queues */
    SHR_IF_ERR_EXIT(bcm_cosq_gport_get(unit, voq_gport, &physical_port, &num_cos_levels, &flags));

    /*
     * STEP 2 - discard incoming traffic by mapping the bundle's voqs into a rate class with tail drop of 0.
     * since the guarantee mechanism ignores tail drop, this solution should be used on voqs which doesn't have guaranteed resources.
     * discarding all traffic for voqs, which have guaranteed resources, can be achieved by defining a PMF rule.
     *
     * STEP 3 - flush voq
     */
    SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_discard_rate_class_profile_get(unit, &discard_rate_class));

    for (cosq = 0; cosq < num_cos_levels; cosq++)
    {
        /** get the voq rate class */
        profile_mapping.profile_type = bcmCosqIngressQueueToRateClass;
        SHR_IF_ERR_EXIT(bcm_cosq_profile_mapping_get(unit, voq_gport, cosq, 0, &profile_mapping));
        voq_rate_class = profile_mapping.mapped_profile;

        /** guaranteed size should not be changed during traffic. copy the voq's guaranteed size into discard rate class */
        for (resource_index = 0; resource_index < COUNTOF(resources); resource_index++)
        {
            flags = resources[resource_index];
            SHR_IF_ERR_EXIT(bcm_cosq_gport_color_size_get(unit, voq_rate_class, 0, 0, flags, &gport_size));

            SHR_IF_ERR_EXIT(bcm_cosq_gport_color_size_get(unit, discard_rate_class, 0, 0, flags, &discard_gport_size));
            discard_gport_size.size_min = gport_size.size_min;
            SHR_IF_ERR_EXIT(bcm_cosq_gport_color_size_set(unit, discard_rate_class, 0, 0, flags, &discard_gport_size));
        }

        /** map voq to discard rate class */
        profile_mapping.profile_type = bcmCosqIngressQueueToRateClass;
        profile_mapping.mapped_profile = discard_rate_class;
        SHR_IF_ERR_EXIT(bcm_cosq_profile_mapping_set(unit, voq_gport, cosq, 0, &profile_mapping));

        /*
         * flush voq (one second timeout)
         */
        SHR_IF_ERR_EXIT(bcm_cosq_control_set(unit, voq_gport, cosq, bcmCosqControlFlush, 1000000));
    }

    /** STEP 4 - destroy the voq bundle */
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
    int delay_tolerance_level = 0;
    int rate_class = 0;
    bcm_gport_t sysport_gport;
    int voq_base_id;
    bcm_gport_t voq_gport;
    bcm_cosq_ingress_queue_bundle_gport_config_t queue_bundle_config;
    bcm_cosq_pkt_size_adjust_info_t adjust_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Get rate class and credit request profile according to port's speed  */
    SHR_IF_ERR_EXIT(appl_dnx_e2e_voq_profiles_final_get(unit, sys_port_map, &delay_tolerance_level, &rate_class));

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
        delay_tolerance_level =
            dnx_data_iqs.credit.feature_get(unit,
                                            dnx_data_iqs_credit_profile_is_rate_class) ? -1 : delay_tolerance_level;
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

/** see header file */
shr_error_e
appl_dnx_sys_port_voq_modify(
    int unit,
    int system_port_id,
    const system_port_mapping_t * sys_port_map)
{
    int delay_tolerance_level = 0;
    int rate_class = 0;
    int voq_base_id;
    bcm_gport_t voq_gport;
    bcm_switch_profile_mapping_t profile_mapping;
    bcm_gport_t rate_class_gport;

    SHR_FUNC_INIT_VARS(unit);

    /** Get rate class and credit request profile according to port's speed  */
    SHR_IF_ERR_EXIT(appl_dnx_e2e_voq_profiles_final_get(unit, sys_port_map, &delay_tolerance_level, &rate_class));

    /** convert system port id to base voq id */
    SHR_IF_ERR_EXIT(appl_sand_sysport_id_to_base_voq_id_convert(unit, system_port_id, &voq_base_id));

    /** Allocate the queue bundle and map system port to the base voq */
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport, voq_base_id);

    /** re-map voq profiles */
    for (int cos = 0; cos < APPL_SAND_E2E_SCHEME_COS; cos++)
    {
        /** Map voq to new credit class */
        /** Map voq to new rate class */
        BCM_GPORT_PROFILE_SET(rate_class_gport, rate_class);
        profile_mapping.profile_type = bcmCosqIngressQueueToRateClass;
        profile_mapping.mapped_profile = rate_class_gport;
        SHR_IF_ERR_EXIT(bcm_cosq_profile_mapping_set(unit, voq_gport, cos, 0, &profile_mapping));
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

    /** init valid ports list for dynamic port usage in diag command*/
    SHR_IF_ERR_EXIT(cmd_dnx_sh_valid_ports_init(unit));

    if (dnx_data_nif.global.l1_only_mode_get(unit) != DNX_PORT_NIF_L1_ONLY_MODE_DISABLED)
    {
        /** nothing to do in L1-only mode */
        SHR_EXIT();
    }
    {
        /** create credit request profiles - to be used when creating ingress queues*/
        SHR_IF_ERR_EXIT(appl_dnx_e2e_credit_request_profiles_create(unit));

        /** create rate class profiles - to be used when creating ingress queues*/
        SHR_IF_ERR_EXIT(appl_dnx_e2e_rate_class_profiles_create(unit));
    }

    /** create discard rate class  */
    SHR_IF_ERR_EXIT(appl_dnx_e2e_discard_rate_class_create(unit));

    /** Create FMQs (fabric multicast queues) e2e scheduling scheme before allocating any VoQs */
    SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_mcast_create(unit));

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** for stand alone configuration run only on local ports */
    if (sys_params->stand_alone_configuration)
    {
        bcm_pbmp_t pbmp;
        bcm_port_t port;

        /** get ports */
        SHR_IF_ERR_EXIT(appl_dnx_sys_device_ports_get(unit, &pbmp));

        /** iterate on ports and add them */
        BCM_PBMP_ITER(pbmp, port)
        {
            system_port_mapping_t sys_port_map = { 0 };
            int system_port_id;

            SHR_IF_ERR_EXIT(appl_dnx_logical_port_to_sysport_map_get(unit, port, &sys_port_map));

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

/*
 * See .h files
 */
int
appl_dnx_sysport_map_to_ftmh_pp_dsp_and_modid_index_get(
    int unit,
    const system_port_mapping_t * sys_port_map,
    int *ftmh_pp_dsp,
    int *modid_index)
{
    bcm_module_t modid;
    appl_dnx_sys_device_t *sys_params;
    int modid_index_within_the_core, nof_modids_per_core;

    SHR_FUNC_INIT_VARS(unit);

    /** extract ftmh_pp_dsp and modid system port */
    SHR_IF_ERR_EXIT(appl_dnx_fap_and_pp_dsp_to_modport_convert(unit, sys_port_map->device_index, sys_port_map->core,
                                                               sys_port_map->pp_dsp, &modid, ftmh_pp_dsp));

    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));
    nof_modids_per_core = sys_params->devices_array[sys_port_map->device_index].nof_modids_per_core;
    modid_index_within_the_core = sys_params->modid_to_index[modid].base_modid_index;
    /** calculate a unique consecutive index within the device (not within a core) for the given modid */
    *modid_index = sys_port_map->core * nof_modids_per_core + modid_index_within_the_core;

exit:
    SHR_FUNC_EXIT;
}

