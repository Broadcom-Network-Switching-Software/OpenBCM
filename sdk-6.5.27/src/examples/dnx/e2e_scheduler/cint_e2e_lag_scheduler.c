/*~~~~~~~~~~~~~~~~~~~~~~~~~~E2E: LAG Scheduler Configuration~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* $Id: cint_e2e_lag_scheduler.c,v 1.1 2020/04/02
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/**
 * GENERAL VARIABLES
 */
int MY_MODID = 0;
int NUM_COS = 8;
int MAX_NOF_MEMBERS_IN_GROUP = 64; /* Assuming default value of trunk_group_max_members SOC property */
int LAG_SCHED_KBITS_SEC      = 10000000; /* 10 Gbps */

/**
 * Function to get the base VOQ of the LAG member port
 */
int lag_member_port_base_voq_get(int unit, bcm_gport_t member_port, int *base_voq)
{

    /** Get each member port's base VOQ ID.
     * This is assuming DNX reference application. In order to be
     * ported to user code,users must provide a different implementation.
     */
    BCM_IF_ERROR_RETURN_MSG(appl_dnx_e2e_scheme_logical_port_to_base_voq_get(unit, MY_MODID, member_port, base_voq),
        "Getting member port's base VOQ ID failed");

    return BCM_E_NONE;
}

/**
 * Function to set the rates of LAG scheduling port
 */
int lag_sch_port_rates_set(int unit, int lag_sch_port, int rate)
{
    bcm_cosq_gport_info_t gport_info;
    int lag_sch_if;
    int cos;

   /** Get E2E Gport of the LAG scheduler port */
    gport_info.in_gport = lag_sch_port;
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeE2EPort, gport_info),
        "Getting E2E Gport of the LAG scheduler port failed");

    /** Rate - Interface */
    BCM_IF_ERROR_RETURN_MSG(bcm_fabric_port_get(unit, gport_info.out_gport, 0, &lag_sch_if),
        "Getting the LAG scheduler interface failed");
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_set(unit, lag_sch_if, 0, 0, rate, 0),
        "Setting LAG scheduler interface rate failed");

    /** Rate - E2E port */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_set(unit, gport_info.out_gport, 0, 0, rate, 0),
        "Setting E2E port rate of the LAG scheduler port failed");

    for (cos = 0; cos < NUM_COS; cos++)
    {
        gport_info.cosq = cos;

        /** Rate - E2E TC port */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeE2EPortTC, gport_info),
            "Getting the LAG scheduler E2E TC gport failed");
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_set(unit, gport_info.out_gport, gport_info.cosq, 0, rate, 0),
            "Setting E2E TC rate of the LAG scheduler port failed");

        /** Get the device's minimum number of port priorities for which TCG rate must be set */
        if (NUM_COS >= 4)
        {
            /** Rate - E2E TCG port for 4 and 8 priorities ports only */
            BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeE2EPortTCG, gport_info),
                "Getting the LAG scheduler E2E TCG gport failed");
            BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_set(unit, gport_info.out_gport, gport_info.cosq, 0, rate, 0),
                "Setting E2E TCG rate of the LAG scheduler port failed");
        }
    }
    return BCM_E_NONE;
}

/**
 * Function to create a LAG scheduling port and attach trunk members to it.
 * Parameters:
 * unit [INPUT] - Unit ID
 * core [INPUT] - Core ID on which the LAG scheduling port will be created
 * lag_sch_port [INPUT] - LAG scheduling port ID
 * hr_id [INPUT] - Base HR of the LAG scheduling port
 * trunk_id [INPUT] - Trunk ID. Trunk must be created and members added before calling this cint
 */
int e2e_lag_scheduling_port_setup(int unit, int core, int lag_sch_port, int hr_id, int trunk_id)
{
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_resource_t resource;
    bcm_cosq_gport_info_t gport_info;
    bcm_trunk_info_t trunk_info;
    bcm_trunk_member_t member_array[MAX_NOF_MEMBERS_IN_GROUP];
    bcm_cosq_voq_connector_gport_t voq_connector_config;
    bcm_gport_t voq_connector, temp_voq_connector;
    int cos, base_voq;
    int member_count, member_index;
    uint32 slow_profile, nof_cores;
    bcm_core_t core_id;
    bcm_cosq_gport_connection_t connection_get;
    bcm_cosq_gport_connection_t connection;
    bcm_gport_t lag_e2e_gport, lag_e2e_tc_gport;
    bcm_gport_t local_member_port, core_gport, voq_gport;

    /** Get the device's number of cores */
    nof_cores = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));

    /** Use dynamic port adding to create the LAG scheduling port */
    interface_info.interface = BCM_PORT_IF_SCH;
    bcm_port_mapping_info_t_init(&mapping_info);
    mapping_info.base_hr = hr_id;
    mapping_info.pp_port=-1;
    mapping_info.core = core;
    mapping_info.num_sch_priorities = NUM_COS;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_add(unit, lag_sch_port, 0, interface_info, mapping_info),
        "Dynamic port adding of LAG scheduling port failed");

    /** Set the LAG scheduler port resources */
    resource.speed = LAG_SCHED_KBITS_SEC;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_resource_set(unit, lag_sch_port, resource),
        "Setting LAG scheduler port resources failed");

    /** Set rates of the LAG scheduling port */
    BCM_IF_ERROR_RETURN_MSG(lag_sch_port_rates_set(unit, lag_sch_port, LAG_SCHED_KBITS_SEC),
        "Setting rates of the LAG scheduling port failed");

    /** Get trunk */
    BCM_IF_ERROR_RETURN_MSG(bcm_trunk_get(unit, trunk_id, &trunk_info, MAX_NOF_MEMBERS_IN_GROUP, member_array, &member_count),
        "Getting trunk members failed");

    for (member_index = 0; member_index < member_count; member_index++)
    {
        /** Trunk members are returned as System ports, get the modport */
        BCM_IF_ERROR_RETURN_MSG(bcm_stk_sysport_gport_get(unit, member_array[member_index].gport, &gport_info.in_gport),
            "Getting Modport for trunk member failed");
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeLocalPort, gport_info),
            "Getting Local Gport for trunk member failed");
        local_member_port = gport_info.out_gport;

        /** Get E2E Gport of the LAG scheduler port */
        gport_info.in_gport = lag_sch_port;
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeE2EPort, gport_info),
            "Getting E2E Gport of the LAG scheduler port failed");
        lag_e2e_gport = gport_info.out_gport;

        /** Attach trunk member port to the LAG scheduler E2E port */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_attach(unit, lag_e2e_gport, local_member_port, -1),
            "Attaching trunk member to the LAG scheduler E2E port failed");

        /** For each LAG member create a VOQ connector on LAG scheduler's core */
        BCM_COSQ_GPORT_CORE_SET(core_gport,core);
        voq_connector_config.port = core_gport;
        voq_connector_config.nof_remote_cores = nof_cores;
        voq_connector_config.remote_modid = MY_MODID;
        voq_connector_config.numq = NUM_COS;
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_voq_connector_gport_add(unit, voq_connector_config, &voq_connector),
            "Creating a VOQ connector failed");

        /** Attach the VOQ connector to the LAG Scheduler HRs */
        for (cos = 0; cos < NUM_COS; cos++)
        {
            /** Get LAG scheduler E2E TC port */
            gport_info.in_gport = lag_e2e_gport;
            gport_info.cosq = cos;
            BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeE2EPortTC, gport_info),
                "Getting the LAG scheduler E2E TC gport failed");
            lag_e2e_tc_gport = gport_info.out_gport;

            /** Get Gport that represents LAG scheduler port's HR */
            gport_info.in_gport = lag_e2e_tc_gport;
            gport_info.cosq = cos;
            BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, gport_info),
                "Getting Gport that represents LAG scheduler port's HR failed");

            /** Connect the VOQ connector to strict priority 0 leg */
            BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_sched_set(unit, voq_connector, cos, BCM_COSQ_SP0, 0),
                "Connecting the VOQ connector to strict priority 0 leg failed");

            /** Attach VOQ connector to LAG scheduler port HR */
            BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_attach(unit, gport_info.out_gport, voq_connector, cos),
                "Attaching VOQ connector to LAG scheduler port HR failed");

            /** Set Slow Rate to be either 1 or 2 or disabled */
            /** For port <= 10G slow rate will be disabled */
            /** in scheduler_tune.c we have defined 2 sets of slow profiles: 1 is for ports <= 100G, */
            /**                                                              2 is for ports > 100G */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_resource_get(unit, local_member_port, &resource),
                "Getting member port's resources failed");
            if (resource.speed <= 10000)
            {
                /** Slow Disabled */
                slow_profile = 0;
            }
            else if (resource.speed <= 100000)
            {
                /* * Profile 1 */
                slow_profile = 1;
            }
            else
            {
                /** Profile 2 */
                slow_profile = 2;
            }
            BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_set(unit, voq_connector, cos, bcmCosqControlFlowSlowRate, slow_profile),
                "Setting slow rate profile failed");
        }

        /** Connect the VOQ of member port to the VOQ connector */
        /** Get each member port's base VOQ ID */
        BCM_IF_ERROR_RETURN_MSG(lag_member_port_base_voq_get(unit, BCM_GPORT_SYSTEM_PORT_ID_GET(member_array[member_index].gport), &base_voq),
            "Getting member port's base VOQ ID failed");
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport, base_voq);

        /** Delete the existing connections for member port on both cores */
        for (core_id = 0; core_id < nof_cores; core_id++)
        {
            /** Get connector and disconnect Ingress connection */
            BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(connection_get.voq_connector, 0, &core_id);
            connection_get.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
            connection_get.remote_modid = MY_MODID;
            connection_get.voq = voq_gport;
            BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_connection_get(unit, &connection_get),
                "Getting member port's existing Ingress connection failed");
            connection_get.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
            BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_connection_set(unit, connection_get),
                "Deleting member port's existing Ingress connection failed");
        }

        /** Set ingress and egress connection of VoQ of member port to VOQ connector */
        connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS | BCM_COSQ_GPORT_CONNECTION_EGRESS;
        connection.remote_modid = MY_MODID;
        connection.voq = voq_gport;
        connection.voq_connector = voq_connector;
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_connection_set(unit, connection),
            "Setting member port's Ingress and Egress connection failed");
    }
    return BCM_E_NONE;
}

/**
 * Function to remove a LAG scheduling port
 * Parameters:
 * unit [INPUT] - Unit ID
 * lag_sch_port [INPUT] - LAG scheduling port ID
 */
int e2e_lag_scheduling_port_remove(int unit, int lag_sch_port)
{

    /** Set rates of the LAG scheduling port to 0 so it can be removed */
    BCM_IF_ERROR_RETURN_MSG(lag_sch_port_rates_set(unit, lag_sch_port, 0),
        "Setting rates of the LAG scheduling port to 0 failed");

    /** Remove the LAG scheduling port */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_remove(unit, lag_sch_port, 0),
        "Removing the LAG scheduling port failed");

    return BCM_E_NONE;
}
