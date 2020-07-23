/** \file appl_ref_e2e_scheme.c
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
#include <bcm_int/dnx_dispatch.h>

#include <appl/reference/dnx/appl_ref_sys_ports.h>
#include <appl/reference/dnx/appl_ref_e2e_scheme.h>

#include <appl/reference/sand/appl_ref_sand.h>

#include "appl_ref_sys_device.h"

#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tune.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_iqs.h>

/** internal include -- used to obtain SOC property value */
#include <bcm_int/dnx/tdm/tdm.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/**
 * \brief - credit request profile id allocated for 10G SLOW ports
 */
#define APPL_DNX_E2E_CREDIT_PROFILE_10G_SLOW         (0)
/**
 * \brief - credit request profile id allocated for 40G SLOW ports
 */
#define APPL_DNX_E2E_CREDIT_PROFILE_40G_SLOW         (1)
/**
 * \brief - credit request profile id allocated for 100G SLOW ports
 */
#define APPL_DNX_E2E_CREDIT_PROFILE_100G_SLOW        (2)
/**
 * \brief - credit request profile id allocated for 200G SLOW ports
 */
#define APPL_DNX_E2E_CREDIT_PROFILE_200G_SLOW        (3)
/**
 * \brief - credit request profile id allocated for 400G SLOW ports
 */
#define APPL_DNX_E2E_CREDIT_PROFILE_400G_SLOW        (4)
/**
 * \brief - credit request profile id allocated for FMQs SLOW ports value per dnx_data_tune.iqs.fmq_max_rate_get
 */
#define APPL_DNX_E2E_CREDIT_PROFILE_MC_SLOW      (5)
/**
 * \brief - credit request profile id allocated for push queues
 */
#define APPL_DNX_E2E_CREDIT_PROFILE_PUSH_QUEUE        (31)

/**
 * \brief - rate class profile id allocated for 10G SLOW ports
 */
#define APPL_DNX_E2E_RATE_CLASS_PROFILE_10G_SLOW         (0)
/**
 * \brief - rate class profile id allocated for 40G SLOW ports
 */
#define APPL_DNX_E2E_RATE_CLASS_PROFILE_40G_SLOW         (1)
/**
 * \brief - rate class profile id allocated for 100G SLOW ports
 */
#define APPL_DNX_E2E_RATE_CLASS_PROFILE_100G_SLOW        (2)
/**
 * \brief - rate class profile id allocated for 200G SLOW ports
 */
#define APPL_DNX_E2E_RATE_CLASS_PROFILE_200G_SLOW        (3)
/**
 * \brief - rate class profile id allocated for 400G SLOW ports
 */
#define APPL_DNX_E2E_RATE_CLASS_PROFILE_400G_SLOW        (4)
/**
 * \brief - rate class profile id allocated for FMQs SLOW ports - value per dnx_data_tune.iqs.fmq_max_rate_get
 */
#define APPL_DNX_E2E_RATE_CLASS_PROFILE_MC_SLOW    (5)

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
appl_dnx_logical_port_to_base_voq_connector_id_get(
    int unit,
    bcm_port_t port,
    bcm_module_t remote_modid,
    int *base_voq_connector)
{
    int device_index, remote_core_id;
    appl_dnx_sys_device_t *sys_params;
    SHR_FUNC_INIT_VARS(unit);

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** convert remote modid to device index and core id */
    SHR_IF_ERR_EXIT(appl_dnx_modid_to_fap_and_core_convert(unit, remote_modid, &device_index, &remote_core_id));

    /** call internal function to get voq_connector id */
    SHR_IF_ERR_EXIT(appl_sand_device_to_base_voq_connector_id_convert(unit, port, BCM_CORE_ALL, device_index,
                                                                      sys_params->nof_devices, base_voq_connector));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_dnx_e2e_voq_profiles_get(
    int unit,
    int speed,
    int *delay_tolerance_level,
    int *rate_class)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get credit request profile according to port's speed
     */
    if (speed <= 20000)
    {
        *delay_tolerance_level = APPL_DNX_E2E_CREDIT_PROFILE_10G_SLOW;
        *rate_class = APPL_DNX_E2E_RATE_CLASS_PROFILE_10G_SLOW;
    }
    else if (speed <= 70000)
    {
        *delay_tolerance_level = APPL_DNX_E2E_CREDIT_PROFILE_40G_SLOW;
        *rate_class = APPL_DNX_E2E_RATE_CLASS_PROFILE_40G_SLOW;
    }
    else if (speed <= 150000)
    {
        *delay_tolerance_level = APPL_DNX_E2E_CREDIT_PROFILE_100G_SLOW;
        *rate_class = APPL_DNX_E2E_RATE_CLASS_PROFILE_100G_SLOW;
    }
    else if (speed <= 300000)
    {
        *delay_tolerance_level = APPL_DNX_E2E_CREDIT_PROFILE_200G_SLOW;
        *rate_class = APPL_DNX_E2E_RATE_CLASS_PROFILE_200G_SLOW;
    }
    else
    {
        *delay_tolerance_level = APPL_DNX_E2E_CREDIT_PROFILE_400G_SLOW;
        *rate_class = APPL_DNX_E2E_RATE_CLASS_PROFILE_400G_SLOW;
    }

    SHR_FUNC_EXIT;
}

/** get delay tolerance for TDM ports */
shr_error_e
appl_dnx_e2e_tdm_delay_tolerance_get(
    int unit,
    int *delay_tolerance_level)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(delay_tolerance_level, _SHR_E_PARAM, "delay_tolerance_level");

    *delay_tolerance_level = APPL_DNX_E2E_CREDIT_PROFILE_PUSH_QUEUE;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create VOQ connector
 */
shr_error_e
appl_dnx_e2e_scheme_voq_connector_create(
    int unit,
    int port,
    int ingress_device_idx)
{
    appl_dnx_sys_device_t *sys_params;
    uint32 dummy_flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_gport_t local_gport, voq_connector_gport;
    bcm_cosq_gport_info_t gport_info;
    int cos, nof_connections_per_hr, if_speed;
    bcm_port_resource_t port_resource;
    int slow_profile = 0;

    int voq_connector_id;
    bcm_cosq_voq_connector_gport_t voq_connector_config;

    SHR_FUNC_INIT_VARS(unit);

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** Set local port gport */
    BCM_GPORT_LOCAL_SET(local_gport, port);

    /** Get voq connector id */
    SHR_IF_ERR_EXIT(bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info));
    SHR_IF_ERR_EXIT(appl_sand_device_to_base_voq_connector_id_convert(unit, port, BCM_CORE_ALL,
                                                                      ingress_device_idx, sys_params->nof_devices,
                                                                      &voq_connector_id));
    BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(voq_connector_gport, voq_connector_id, mapping_info.core);

    voq_connector_config.flags = BCM_COSQ_GPORT_VOQ_CONNECTOR | BCM_COSQ_GPORT_WITH_ID;
    voq_connector_config.nof_remote_cores = APPL_DNX_NOF_REMOTE_CORES;
    voq_connector_config.numq = APPL_SAND_E2E_SCHEME_COS;

    /** there is no specific port on remote device - use tm port 0 */
    SHR_IF_ERR_EXIT(appl_dnx_fap_and_tm_port_to_modid_convert
                    (unit, ingress_device_idx, BCM_CORE_ALL, 0, &voq_connector_config.remote_modid));
    BCM_COSQ_GPORT_E2E_PORT_SET(voq_connector_config.port, port);

    /** allocate voq connectors */
    SHR_IF_ERR_EXIT(bcm_cosq_voq_connector_gport_add(unit, &voq_connector_config, &voq_connector_gport));

    /** Each HR should be attached to equal number of connectors  according to amount of HRs */
    nof_connections_per_hr = voq_connector_config.numq / mapping_info.num_sch_priorities;

    /** Each VOQ connector attach to suitable HR */
    for (cos = 0; cos < voq_connector_config.numq; cos++)
    {
        /** Connect the voq connector to strict priority 0 leg */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_sched_set(unit, voq_connector_gport, cos, BCM_COSQ_SP0, 0));

        BCM_COSQ_GPORT_E2E_PORT_TC_SET(gport_info.in_gport, local_gport);

        /** Connect voq to HR according to the amount of HRs (span the connectors over the HRs) */
        gport_info.cosq = cos / nof_connections_per_hr;

        /** get gport that represents port's HR */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &gport_info));

        /** attach HR SPi to connector */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_attach(unit, gport_info.out_gport, voq_connector_gport, cos));

        /** required delay tolerance profile */
        SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, port, &port_resource));
        if_speed = port_resource.speed;

        /** Set Slow Rate to be either 1 or 2 or disabled */
        /** For port <= 10G slow rate will be disabled */
        /** in scheduler_tune.c we have defined 2 sets of slow profile: 1 is for ports <= 100G, */
        /**                                                             2 is for ports > 100G */
        if (if_speed <= 10000)
        {
            /*
             * Slow Disabled
             */
            slow_profile = 0;
        }
        else if (if_speed <= 100000)
        {
            /*
             * Profile 1
             */
            slow_profile = 1;
        }
        else
        {
            /*
             * Profile 2
             */
            slow_profile = 2;
        }
        SHR_IF_ERR_EXIT(bcm_cosq_control_set(unit, voq_connector_gport, cos, bcmCosqControlFlowSlowRate, slow_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Destroy VOQ connector
 */
shr_error_e
appl_dnx_e2e_scheme_voq_connector_destroy(
    int unit,
    int port,
    int ingress_device_idx)
{

    appl_dnx_sys_device_t *sys_params;
    uint32 dummy_flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_gport_t local_gport, voq_connector_gport;
    bcm_cosq_gport_info_t gport_info;
    int cos;
    int voq_connector_id;
    int nof_connections_per_hr;

    SHR_FUNC_INIT_VARS(unit);

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** Set local port gport */
    BCM_GPORT_LOCAL_SET(local_gport, port);

    /** Get voq connector id */
    SHR_IF_ERR_EXIT(bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info));

    SHR_IF_ERR_EXIT(appl_sand_device_to_base_voq_connector_id_convert(unit, port, BCM_CORE_ALL,
                                                                      ingress_device_idx, sys_params->nof_devices,
                                                                      &voq_connector_id));
    BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(voq_connector_gport, voq_connector_id, mapping_info.core);

    /** Each HR should be attached to equal number of connectors  according to amount of HRs */
    nof_connections_per_hr = APPL_SAND_E2E_SCHEME_COS / mapping_info.num_sch_priorities;

    /** Each VOQ connector detach from suitable HR */
    for (cos = 0; cos < APPL_SAND_E2E_SCHEME_COS; cos++)
    {
        BCM_COSQ_GPORT_E2E_PORT_TC_SET(gport_info.in_gport, local_gport);
        /** detach voq connectors from HRs according to the amount of HRs */
        gport_info.cosq = cos / nof_connections_per_hr;

        /** get gport that represents port's HR */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &gport_info));

        /** detach HR SPi to connecter */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_detach(unit, gport_info.out_gport, voq_connector_gport, cos));
    }

    /** deallocate voq connectors */
    SHR_IF_ERR_EXIT(bcm_cosq_gport_delete(unit, voq_connector_gport));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Connect VOQ to Voq connector
 * on ingress side
 */
shr_error_e
appl_dnx_e2e_scheme_voq_connect(
    int unit,
    int system_port_id,
    const system_port_mapping_t * sys_port_map,
    int is_disconnect)
{
    bcm_cosq_gport_connection_t connection;
    int voq_base_id;
    int voq_connector_id;
    appl_dnx_sys_device_t *sys_params;

    SHR_FUNC_INIT_VARS(unit);

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** Fill connections attributes */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
    if (is_disconnect)
    {
        connection.flags |= BCM_COSQ_GPORT_CONNECTION_INVALID;
    }

    /** set voq id */
    SHR_IF_ERR_EXIT(appl_sand_sysport_id_to_base_voq_id_convert(unit, system_port_id, &voq_base_id));
    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(connection.voq, BCM_CORE_ALL, voq_base_id);

    /** set voq connector id */
    SHR_IF_ERR_EXIT(appl_sand_device_to_base_voq_connector_id_convert
                    (unit, sys_port_map->local_port, BCM_CORE_ALL, sys_params->unit_to_device_index[unit],
                     sys_params->nof_devices, &voq_connector_id));

    /** set voq connector gport */
    BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(connection.voq_connector, voq_connector_id, sys_port_map->core);

    /** set the 'voq to voq_connector' connection */
    SHR_IF_ERR_EXIT(bcm_cosq_gport_connection_set(unit, &connection));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Connect Voq connector to VOQ
 * on egress side
 */
shr_error_e
appl_dnx_e2e_scheme_voq_connector_connect(
    int unit,
    int system_port_id,
    const system_port_mapping_t * sys_port_map,
    int ingress_device_index,
    int is_disconnect)
{
    appl_dnx_sys_device_t *sys_params;
    bcm_cosq_gport_connection_t connection;
    int voq_base_id, voq_connector_id;

    SHR_FUNC_INIT_VARS(unit);

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** get voq id */
    SHR_IF_ERR_EXIT(appl_sand_sysport_id_to_base_voq_id_convert(unit, system_port_id, &voq_base_id));

    /** fill connection attributes */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    if (is_disconnect)
    {
        connection.flags |= BCM_COSQ_GPORT_CONNECTION_INVALID;
    }

    /** set voq connector id */
    SHR_IF_ERR_EXIT(appl_sand_device_to_base_voq_connector_id_convert
                    (unit, sys_port_map->local_port, BCM_CORE_ALL, ingress_device_index, sys_params->nof_devices,
                     &voq_connector_id));
    BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(connection.voq_connector, voq_connector_id, sys_port_map->core);

    /** set voq id */
    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(connection.voq, BCM_CORE_ALL, voq_base_id);

    /** set remote modid, core and TM port can be 0 because all that matters at this point is reaching the correct device */
    SHR_IF_ERR_EXIT(appl_dnx_fap_and_tm_port_to_modid_convert
                    (unit, ingress_device_index, 0, 0, &connection.remote_modid));

    /** set the 'voq_connector to voq' connection */
    SHR_IF_ERR_EXIT(bcm_cosq_gport_connection_set(unit, &connection));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_dnx_e2e_scheme_port_create(
    int unit,
    int system_port_id,
    const system_port_mapping_t * sys_port_map)
{
    appl_dnx_sys_device_t *sys_params;
    int ingress_device_index;

    uint32 port_flags;
    bcm_gport_t e2e_gport, local_gport;
    int sch_priority;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    int port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(sys_port_map, _SHR_E_PARAM, "sys_port_map");

    /** once system port DB is fully supported also for dynamic port interface to the function can
     * change to contain only system port id and not local port as well (can be safely deduced from system port) */
    port = sys_port_map->local_port;

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /*
     * [Egress] Configure HR mode (change to enhanced mode).
     *  Before creating the scheme, need to configure the existing HRs mode.
     */
    {
        SHR_IF_ERR_EXIT(bcm_port_get(unit, port, &port_flags, &interface_info, &mapping_info));

        /** Skip L1 ports */
        if (port_flags & (BCM_PORT_ADD_CROSS_CONNECT | BCM_PORT_ADD_FLEXE_PHY))
        {
            SHR_EXIT();
        }

        BCM_GPORT_LOCAL_SET(local_gport, port);
        port_flags = (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_HR_ENHANCED | BCM_COSQ_GPORT_REPLACE);
        for (sch_priority = 0; sch_priority < mapping_info.num_sch_priorities; sch_priority++)
        {
            BCM_COSQ_GPORT_E2E_PORT_TC_SET(e2e_gport, port);
            SHR_IF_ERR_EXIT(bcm_cosq_gport_add(unit, local_gport, sch_priority, port_flags, &e2e_gport));
        }
    }

    /** Create VOQ connectors for all VOQs pointing to the port on remote devices. */
    for (ingress_device_index = 0; ingress_device_index < sys_params->nof_devices; ingress_device_index++)
    {
        /** in stand alone configuration run only on device related to this unit */
        if (sys_params->stand_alone_configuration)
        {
            if (sys_params->unit_to_device_index[unit] != ingress_device_index)
            {
                continue;
            }
        }
        SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_voq_connector_create(unit, port, ingress_device_index));
    }

    /** [Egress] Connect VOQ connectors to VOQs of the remote devices. */
    for (ingress_device_index = 0; ingress_device_index < sys_params->nof_devices; ingress_device_index++)
    {
        /** in stand alone configuration run only on device related to this unit */
        if (sys_params->stand_alone_configuration)
        {
            if (sys_params->unit_to_device_index[unit] != ingress_device_index)
            {
                continue;
            }
        }
        /** Connect voq_connector to voq */
        SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_voq_connector_connect
                        (unit, system_port_id, sys_port_map, ingress_device_index, 0 /** is_disconnect */ ));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_dnx_e2e_scheme_port_destroy(
    int unit,
    int system_port_id,
    int port)
{
    appl_dnx_sys_device_t *sys_params;
    system_port_mapping_t sys_port_map;
    uint8 is_local_port = 0;
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /*
     * Destroy scheduling scheme for a given port:
     *   1. [Ingress] Disconect VOQ pointing to the port on this device from VOQ connector
     *   2. [Ingress] Destroy VOQs pointing to the port on this device.
     *   3. [Egress]  Disconnect Connectors for all VOQs pointing to the port on remote devices.
     *   4. [Egress] Destroy Connectors for all VOQs pointing to the port on remote devices.
     */

    rv = appl_dnx_system_port_db_get(unit, sys_params->system_port_db, system_port_id, &sys_port_map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_EXISTS);
    if (rv == _SHR_E_NONE)
    {
        /** if this unit is mapped to same device as system port, mark as local (do egress part) */
        if (sys_port_map.device_index == sys_params->unit_to_device_index[unit])
        {
            is_local_port = 1;
        }
    }
    else
    {
        /** once system port DB is fully supported also for dynamic port this part needs to be deleted as it will be dead code */
        /** at that point interface to the function can also be changed to contain only system port id and not local port as well */

        /** else assume local port to this unit and take info using driver BCM APIs */
        uint32 flags;
        bcm_port_interface_info_t interface_info;
        bcm_port_mapping_info_t mapping_info;
        bcm_port_resource_t port_resource;

        is_local_port = 1;
        /** get port info */
        SHR_IF_ERR_EXIT(bcm_port_get(unit, port, &flags, &interface_info, &mapping_info));
        /** get port speed */
        SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, port, &port_resource));
        sys_port_map.device_index = sys_params->unit_to_device_index[unit];
        sys_port_map.local_port = port;
        sys_port_map.core = mapping_info.core;
        sys_port_map.tm_port = mapping_info.tm_port;
        sys_port_map.rate = port_resource.speed;

        /** if port is in filtered ports it never got resources in the first place, we can just get out */
        if (flags & APPL_DNX_FILTERED_PORTS_FLAGS)
        {
            SHR_EXIT();
        }
    }

    /** [Ingress] DisConnect VOQs to VOQ connectors of the port of each remote device. */
    {
        /** Disconnect VOQ from VOQ Connector */
        SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_voq_connect(unit, system_port_id, &sys_port_map, 1 /** is_disconnect */ ));

        /** Destroy VOQs pointing to the port */
        SHR_IF_ERR_EXIT(appl_dnx_sys_ports_voq_destroy(unit, system_port_id));
    }

    /** [Egress] Disconnect VOQ connectors to VOQs of the remote devices and destroy connectors. */
    if (is_local_port)
    {
        for (int ingress_device_index = 0; ingress_device_index < sys_params->nof_devices; ingress_device_index++)
        {
            /** Disconnect voq_connector to voq */
            SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_voq_connector_connect
                            (unit, system_port_id, &sys_port_map, ingress_device_index, 1 /** is_disconnect */ ));

            /** Destroy voq connectors for all voqs pointing to the port on remote devices. */
            SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_voq_connector_destroy
                            (unit, sys_port_map.local_port, ingress_device_index));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create multicast scheduling scheme
 * Before creating VOQs, User must specify the range of the FMQs in the device.
 * In most cases, where fabric multicast is only defined by packet tc, range should set between 0-3.
 * Set range that is different than 0-3, need to change fabric scheduler mode.
 * Please see more details in the UM, Cint example:
 * cint_enhance_application.c and API:
 * bcm_fabric_control_set type: bcmFabricMulticastSchedulerMode.
 */
shr_error_e
appl_dnx_e2e_scheme_mcast_create(
    int unit)
{
    bcm_gport_t gport_mcast_queue_group;
    bcm_cosq_ingress_queue_bundle_gport_config_t queue_bundle_config;
    int numq;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        /** by default set simple FMQ mode */
        SHR_IF_ERR_EXIT(bcm_fabric_control_set(unit, bcmFabricMulticastSchedulerMode, 0));

        /** Set voq range for FMQs */
        SHR_IF_ERR_EXIT(bcm_fabric_control_set(unit, bcmFabricMulticastQueueMax, 3));

        /** Create 4 FMQs (0...3) */
        sal_memset(&queue_bundle_config, 0, sizeof(bcm_cosq_ingress_queue_bundle_gport_config_t));
        queue_bundle_config.flags = BCM_COSQ_GPORT_MCAST_QUEUE_GROUP | BCM_COSQ_GPORT_WITH_ID;
        queue_bundle_config.local_core_id = BCM_CORE_ALL; /** Irrelevant in case of WITH_ID allocation */
        queue_bundle_config.numq = 4;
        queue_bundle_config.port = 0; /** Irrelevant in case of FMQs */
        for (numq = 0; numq < queue_bundle_config.numq; numq++)
        {
            queue_bundle_config.queue_atrributes[numq].rate_class = APPL_DNX_E2E_RATE_CLASS_PROFILE_MC_SLOW;
            queue_bundle_config.queue_atrributes[numq].delay_tolerance_level = APPL_DNX_E2E_CREDIT_PROFILE_MC_SLOW;
        }
        BCM_GPORT_MCAST_QUEUE_GROUP_CORE_QUEUE_SET(gport_mcast_queue_group, BCM_CORE_ALL, 0);
        SHR_IF_ERR_EXIT(bcm_cosq_ingress_queue_bundle_gport_add(unit, &queue_bundle_config, &gport_mcast_queue_group));

        if (dnx_data_iqs.dqcq.feature_get(unit, dnx_data_iqs_dqcq_8_priorities) != 0)
        {
            /** fmq 3 should be mapped to priority 0 which is the  highest priority */
            SHR_IF_ERR_EXIT(bcm_cosq_control_set(unit, gport_mcast_queue_group, 3, bcmCosqControlPrioritySelect, 0));

            /** fmq 0-2 should be mapped to priorities 7-5 */
            for (numq = 0; numq < 3; numq++)
            {
                SHR_IF_ERR_EXIT(bcm_cosq_control_set(unit, gport_mcast_queue_group, numq, bcmCosqControlPrioritySelect,
                                                     dnx_data_iqs.dqcq.nof_priorities_get(unit) - 1 - numq));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_dnx_e2e_rate_class_profiles_create(
    int unit)
{
    int profiles[] = { APPL_DNX_E2E_RATE_CLASS_PROFILE_10G_SLOW, APPL_DNX_E2E_RATE_CLASS_PROFILE_40G_SLOW,
        APPL_DNX_E2E_RATE_CLASS_PROFILE_100G_SLOW, APPL_DNX_E2E_RATE_CLASS_PROFILE_200G_SLOW,
        APPL_DNX_E2E_RATE_CLASS_PROFILE_400G_SLOW, APPL_DNX_E2E_RATE_CLASS_PROFILE_MC_SLOW
    };
    uint32 root_fmq_max_rate = dnx_data_tune.iqs.fmq_max_rate_get(unit);
    int profile_rates[] = { 10, 40, 100, 200, 400, root_fmq_max_rate }; /* rates in gbps */
    int profile_index;
    int flags = 0;
    bcm_gport_t rate_class_gport;
    bcm_cosq_rate_class_create_info_t create_info;
    SHR_FUNC_INIT_VARS(unit);

    for (profile_index = 0; profile_index < COUNTOF(profiles); profile_index++)
    {
        create_info.attributes = 0;

        /** set multicast indication for FMQ profile */
        if (profiles[profile_index] == APPL_DNX_E2E_RATE_CLASS_PROFILE_MC_SLOW)
        {
            create_info.attributes |= BCM_COSQ_RATE_CLASS_CREATE_ATTR_MULTICAST;
        }

        BCM_GPORT_PROFILE_SET(rate_class_gport, profile_index);
        create_info.rate = profile_rates[profile_index];
        create_info.attributes |= BCM_COSQ_RATE_CLASS_CREATE_ATTR_SLOW_ENABLED;
        SHR_IF_ERR_EXIT(bcm_cosq_gport_rate_class_create(unit, rate_class_gport, flags, &create_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_dnx_e2e_credit_request_profiles_create(
    int unit)
{
    bcm_cosq_delay_tolerance_t credit_request_profile;
    bcm_cosq_delay_tolerance_preset_attr_t credit_request_attr;
    int profiles[] = { APPL_DNX_E2E_CREDIT_PROFILE_10G_SLOW, APPL_DNX_E2E_CREDIT_PROFILE_40G_SLOW,
        APPL_DNX_E2E_CREDIT_PROFILE_100G_SLOW, APPL_DNX_E2E_CREDIT_PROFILE_200G_SLOW,
        APPL_DNX_E2E_CREDIT_PROFILE_400G_SLOW, APPL_DNX_E2E_CREDIT_PROFILE_MC_SLOW,
        APPL_DNX_E2E_CREDIT_PROFILE_PUSH_QUEUE
    };
    uint32 root_fmq_max_rate = dnx_data_tune.iqs.fmq_max_rate_get(unit);
    int profile_rates[] = { 10, 40, 100, 200, 400, root_fmq_max_rate, 0 };      /* rates in gbps */
    int profile_index;
    bcm_stk_modid_config_t *modid_config = NULL;
    int modid_count;
    uint32 credit_size;
    int max_modids;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create credit request profiles
     */
    bcm_cosq_delay_tolerance_preset_attr_t_init(&credit_request_attr);

    /** assume symmetric system */
    SHR_IF_ERR_EXIT(bcm_stk_modid_count(unit, &max_modids));

    modid_config = sal_alloc(sizeof(bcm_stk_modid_config_t) * max_modids, "modid_config");

    SHR_IF_ERR_EXIT(bcm_stk_modid_config_get_all(unit, max_modids, modid_config, &modid_count));

    /** all modid(s) of a device have the same credit size */
    SHR_IF_ERR_EXIT(bcm_cosq_dest_credit_size_get(unit, modid_config[0].modid, &credit_size));
    credit_request_attr.credit_size = credit_size;
    credit_request_attr.flags = BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_SLOW_ENABLED;
    for (profile_index = 0; profile_index < COUNTOF(profiles); profile_index++)
    {
        /** get set of recommended values */
        credit_request_attr.rate = profile_rates[profile_index];

        /** set multicast indication for FMQ profile */
        if (profiles[profile_index] == APPL_DNX_E2E_CREDIT_PROFILE_MC_SLOW)
        {
            credit_request_attr.flags |= BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_MULTICAST;
        }

        /** set multicast indication for FMQ profile */
        if (profiles[profile_index] == APPL_DNX_E2E_CREDIT_PROFILE_PUSH_QUEUE)
        {
            credit_request_attr.flags = BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_PUSH_QUEUE;
            credit_request_attr.credit_size = 0;
        }

        SHR_IF_ERR_EXIT(bcm_cosq_delay_tolerance_preset_get(unit, &credit_request_attr, &credit_request_profile));

        /** set the threshold */
        SHR_IF_ERR_EXIT(bcm_cosq_delay_tolerance_level_set(unit, profiles[profile_index], &credit_request_profile));
    }

exit:
    SHR_FREE(modid_config);
    SHR_FUNC_EXIT;
}

/** CB function for system port DB iterator */
static shr_error_e
appl_dnx_e2e_scheme_port_create_cb(
    int unit,
    int system_port_id,
    const system_port_mapping_t * sys_port_map_info,
    void *device_index_as_void_ptr)
{
    int rv;
    int *device_index;
    SHR_FUNC_INIT_VARS(unit);

    /** cast opaque pointer */
    device_index = device_index_as_void_ptr;

    /** if not local port continue */
    if (sys_port_map_info->device_index == *device_index)
    {
        rv = appl_dnx_e2e_scheme_port_create(unit, system_port_id, sys_port_map_info);
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "Failed to create e2e scheme for port: %d.%s%s", sys_port_map_info->local_port,
                                 EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create unicast scheduling scheme for each port.
 * For more info \see appl_dnx_e2e_scheme_port_create().
 */
static shr_error_e
appl_dnx_e2e_scheme_ucast_create(
    int unit)
{
    appl_dnx_sys_device_t *sys_params;
    int device_index;

    SHR_FUNC_INIT_VARS(unit);

    /** get system info */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** get unit's device index */
    device_index = sys_params->unit_to_device_index[unit];

    if (sys_params->stand_alone_configuration)
    {
        bcm_pbmp_t pbmp;
        bcm_port_t port;

        /** get ports */
        SHR_IF_ERR_EXIT(appl_dnx_sys_device_ports_get(unit, &pbmp));

        /** iterate on ports and add them */
        BCM_PBMP_ITER(pbmp, port)
        {
            int system_port_id;
            system_port_mapping_t sys_port_map;
            bcm_port_interface_info_t interface_info;
            bcm_port_mapping_info_t mapping_info;
            bcm_port_resource_t port_resource;
            uint32 flags;

            /** Convert device and logical port to system port id */
            SHR_IF_ERR_EXIT(appl_sand_device_to_sysport_convert
                            (unit, device_index, APPL_DNX_NOF_SYSPORTS_PER_DEVICE, port, &system_port_id));

            SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, port, &port_resource));
            SHR_IF_ERR_EXIT(bcm_port_get(unit, port, &flags, &interface_info, &mapping_info));
            sys_port_map.device_index = sys_params->unit_to_device_index[unit];
            sys_port_map.local_port = port;
            sys_port_map.core = mapping_info.core;
            sys_port_map.tm_port = mapping_info.tm_port;
            sys_port_map.rate = port_resource.speed;

            SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_port_create(unit, system_port_id, &sys_port_map));
        }
    }
    else
    {
        /** create scheduling scheme for each port */
        SHR_IF_ERR_EXIT(appl_dnx_system_port_db_iterate(unit, sys_params->system_port_db,
                                                        appl_dnx_e2e_scheme_port_create_cb, &device_index));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - only top 2 priorities are set to high
 */
static shr_error_e
appl_dnx_e2e_scheme_queues_priorities_init(
    int unit)
{
    int max_low_prio;
    bcm_gport_t port = 0;
    bcm_cos_queue_t cosq = -1;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_iqs.dqcq.feature_get(unit, dnx_data_iqs_dqcq_8_priorities))
    {
        /*
         * DQCQs 0-1 will be high priority, DQCQs 2-7 will be low priority
         */
        max_low_prio = 2;
        SHR_IF_ERR_EXIT(bcm_cosq_control_set(unit, port, cosq, bcmCosqControlIngressMinLowPriority, max_low_prio));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_dnx_e2e_scheme_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Create unicast e2e scheduling scheme */
    SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_ucast_create(unit));

    /**setting queues half high half low*/
    SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_queues_priorities_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_dnx_e2e_profiles_create(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** create credit request profiles - to be used when creating ingress queues*/
    SHR_IF_ERR_EXIT(appl_dnx_e2e_credit_request_profiles_create(unit));

    /** create rate class profiles - to be used when creating ingress queues*/
    SHR_IF_ERR_EXIT(appl_dnx_e2e_rate_class_profiles_create(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
int
appl_dnx_e2e_scheme_logical_port_to_base_voq_get(
    int unit,
    bcm_module_t egress_modid,
    bcm_port_t logical_port,
    int *base_voq)
{
    int device_index;
    int sysport;
    appl_dnx_sys_device_t *sys_params;
    int egress_core;

    SHR_FUNC_INIT_VARS(unit);

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** convert modid to device index */
    SHR_IF_ERR_EXIT(appl_dnx_modid_to_fap_and_core_convert(unit, egress_modid, &device_index, &egress_core));
    SHR_IF_ERR_EXIT(appl_sand_device_to_sysport_convert(unit, device_index,
                                                        APPL_DNX_NOF_SYSPORTS_PER_DEVICE, logical_port, &sysport));
    SHR_IF_ERR_EXIT(appl_sand_sysport_id_to_base_voq_id_convert(unit, sysport, base_voq));

exit:
    SHR_FUNC_EXIT;
}
