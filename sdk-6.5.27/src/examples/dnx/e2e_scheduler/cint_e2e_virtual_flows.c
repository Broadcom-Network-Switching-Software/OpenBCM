/*~~~~~~~~~~~~~~~~~~~~~~~~~~E2E: Virtual flow Configuration~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:        cint_e2e_virtual_flows.c
 * Purpose:     Example of creating an E2E scheduler scheme with a virtual flow in it
 *
 * Example includes:
 *  o     Creating a Shared Shaper in quad mode
 *  o     Attaching all shared shaper subelements to E2E gport
 *  o     Creating a VOQ connector and attaching it to the shared shaper
 *  o     Creating a virtual flow
 *  o     Attaching it to first shared shaper subelement
 *  o     Configuring the shapers of all the created schedulers
 *  o     We want to limit the shared shaper to only 10G,
 *  o     to do this we will set the shaper of the first shared shaper subelement to 10 and disable the other
 *
 * NOTE! It is assumed E2E default application is disable (appl_enable_e2e_scheme=0)
 *
*/

/**
 * E2E Scheme
 *
 *                        +-----------------------------+
 *                        | HR0 of Local Port_provided  |
 *                        +---------------+-------------+
 *                                        |
 *                                        |10G
 *                                        |
 *                  +---------------------+----------------------+
 *              +---v--------------------------------------------v---+
 *              |Element| ...(NOF_ELEMENTS_IN_SHARED_SHAPER) |Last   |
 *              |0      |                                    |Element|
 *              |       |            SHARED SHAPER           |       |
 *              +-+-+---+------------------------------------+---+---+
 *                | |                                            |
 *                | |                                            |
 *                | |CIR RATE                                    |EIR RATE
 *     +----------+ |                                            |
 *     |            |                                            |
 * +   v     |   +  v  |                                      +  v  |
 * | Virtual |   |     |  ...(NOF_ELEMENTS_IN_SHARED_SHAPER)  |     |
 * |  flow   |   |COS0 |                                      |COS  |
 * | bundle  |   |     |                                      |LAST |
 * +---------+   +-----+                                      +-----+
 *
 */

/**
 * GENERAL VARIABLES
 */
int MY_MODID = 0;
int NUM_COS = 4;
int SE_SCHEDULER_MODE = BCM_COSQ_SP0;
int SHARED_SHAPER_MODE = BCM_COSQ_SP0;
int SCHED_MAX_BURST = (128 * 1024);
int IN_PORT_SPEED_MBPS = 10000;
/**
 * RATES
 */
int E2E_INTERFACE_KBITS_SEC_MAX     = 10000000; /* 10 Gbps */
int E2E_PORT_KBITS_SEC_MAX          = 10000000; /* 10 Gbps */
int SCHED_KBITS_SEC_MAX             = 10000000; /* 10 Gbps */
int VOQ_CONNECTOR_KBITS_SEC_MAX     = 10000000; /* 10 Gbps */
int CIR_RATE_KBITS                  = 10000000; /* 10 Gbps */
int EIR_RATE_KBITS                  = 0;        /* 0 Gbps */

/**
 * GLOBALS
 * Note! These variables is important to stay global in order to utilize
 * e2e_shared_shaper_setup_teardown
 */
bcm_gport_t e2e_gport;
bcm_gport_t gport_shared_shaper;
bcm_gport_t gport_voq_connector;
bcm_gport_t gport_ucast_voq;
bcm_gport_t virtual_flow_gport;

/**
 * Helper functions
 * {
 */

/**
 * Attach each element in the shared shaper to a SE
 */
int attach_shared_shaper_to_hr(int unit, int nof_elements_in_shared_shaper, bcm_gport_t gport_shared_shaper, bcm_gport_t e2e_gport)
{
    int se_index;
    bcm_cosq_gport_info_t gport_info;

    gport_info.in_gport = gport_shared_shaper;
    gport_info.cosq = 0;
    for (se_index = 0; se_index < nof_elements_in_shared_shaper; se_index++) {
        printf("\n Connect shared shaper 0x%x element %d to e2e gport 0x%x ", gport_shared_shaper, se_index, e2e_gport);

        /**
         * Get relevant element from the shared shaper
         */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSchedSharedShaper0 + se_index, &gport_info), "");

        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_sched_set(unit, gport_info.out_gport, 0, SHARED_SHAPER_MODE, 0), "");


        /**
         *
         */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_attach(unit, e2e_gport, gport_info.out_gport, -1), "");
    }

    return BCM_E_NONE;
}

/**
 * Attach all each voq connector from the bundle to an element from the shaper
 */
int attach_voq_connector_to_shared_shaper(int unit, int nof_elements_in_shared_shaper, bcm_gport_t gport_voq_connector, bcm_gport_t gport_shared_shaper)
{
    int cos;
    bcm_cosq_gport_info_t gport_info;

    gport_info.in_gport = gport_shared_shaper;
    gport_info.cosq = 0;
    for (cos = 0; cos < nof_elements_in_shared_shaper; cos++) {
        printf("\n Attach VOQ connector 0x%x cos %d to the Shared shaper 0x%x element %d \n", gport_voq_connector, cos, gport_shared_shaper, cos);
        /* COSx from the VOQ connector scheduling discipline setup */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_sched_set(unit, gport_voq_connector, cos, BCM_COSQ_SP0, 0), "");

        /**
         * Get relevant parent element from the shared shaper
         */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSchedSharedShaper0 + cos, &gport_info), "");

        /* COS X from the connector attach to parent Shared shaper element X */
        printf(" Attach VOQ connector 0x%x cos %d to the Shared shaper 0x%x element %d \n", gport_voq_connector, cos, gport_shared_shaper, cos );
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_attach(unit, gport_info.out_gport, gport_voq_connector, cos), "");
    }

    return BCM_E_NONE;
}

/**
 * Function to configure the shapers from the E2E scheme
 * All shapers are configured to 10G instead of the elements in the shared shapers.
 * The first element of the shaper is configured with the CIR rate and the others with the EIR rate
 */
int configure_all_shapers(int unit, int nof_elements_in_shared_shaper, uint32 cir_rate, uint32 eir_rate, bcm_gport_t e2e_gport, bcm_gport_t gport_shared_shaper, bcm_gport_t gport_voq_connector) {
    int se_index, cos;
    bcm_gport_t e2e_parent_gport;
    bcm_cosq_gport_info_t gport_info;
    uint32 shaper_kbps_max;
    uint32 flags = 0;

    /*
     * Rate - Interface
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_fabric_port_get(unit, e2e_gport, 0, &e2e_parent_gport), "");
    printf("Set interface 0x%x to %d Kbps \n", e2e_parent_gport, E2E_INTERFACE_KBITS_SEC_MAX);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_set(unit, e2e_parent_gport, 0, 0, E2E_INTERFACE_KBITS_SEC_MAX, 0), "");
    /*
     * Rate - E2E port
     */
    printf("Set E2E gport rate 0x%x to %d Kbps \n ", e2e_gport, E2E_PORT_KBITS_SEC_MAX);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_set(unit, e2e_gport, 0, 0, E2E_PORT_KBITS_SEC_MAX, 0), "");

    /*
     * Rates - shared shaper
     */
    gport_info.in_gport = gport_shared_shaper;
    gport_info.cosq = 0;
    for (se_index = 0; se_index < nof_elements_in_shared_shaper; se_index++) {
        /**
         * Get relevant element from the shared shaper
         */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSchedSharedShaper0 + se_index, &gport_info), "");

        /**
         * Set first shared shaper element with CIR rate and all others with EIR rate
         */
        if (se_index == 0) {
            shaper_kbps_max = cir_rate;
        } else {
            shaper_kbps_max = eir_rate;
            flags = BCM_COSQ_BW_SHAPER_DISABLED;
        }
        printf("Set Shared shaper gport 0x%x (element %d) to %d Kbps and MAX burst of %u \n ", gport_info.out_gport, se_index, shaper_kbps_max, SCHED_MAX_BURST);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_set(unit, gport_info.out_gport, 0, 0, shaper_kbps_max, flags), "");

        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_set(unit, gport_info.out_gport, 0, bcmCosqControlBandwidthBurstMax, SCHED_MAX_BURST), "");
    }

    /*
     * Rate - COS 0/nof_elements_in_shared_shaper
     */
    for (cos = 0; cos < nof_elements_in_shared_shaper; cos++) {
        /* Set rate on the Connector flow */
        printf("Set VOQ Connector gport 0x%x (cos %d) to %d Kbps and MAX burst of %u \n ", gport_voq_connector, cos, VOQ_CONNECTOR_KBITS_SEC_MAX, SCHED_MAX_BURST);

        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_set(unit, gport_voq_connector, cos, 0, VOQ_CONNECTOR_KBITS_SEC_MAX, 0), "");

        /* Set maxburst on the Connector flow */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_set(unit, gport_voq_connector, cos, bcmCosqControlBandwidthBurstMax, SCHED_MAX_BURST), "");
    }

    return BCM_E_NONE;
}

/**
 * Function to create a virtual flow bundle, configure it to the lowest possible rate
 * and attach it to the first subelement of the shared shaper
 */
int create_and_attach_virtual_flow(int unit, int local_port, bcm_gport_t gport_parent_se)
{
    bcm_cosq_voq_connector_gport_t voq_connector_config;
    bcm_cosq_range_t range;
    int low_rate_region_start = 20;
    int low_rate_region_end = 20;
    int nof_remote_cores = *dnxc_data_2d_get (unit, "sch", "flow", "nof_remote_cores", "val", 0, 20);
    int credit_worth = *(dnxc_data_get(unit, "iqs", "credit", "worth", NULL));
    uint32 max_burst = 4*credit_worth;

    range.range_start = low_rate_region_start;
    range.range_end = low_rate_region_end;
    range.is_enabled = TRUE;

    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_range_set(unit, 0, 0, bcmCosqRangeLowRateVoqConnector, &range),  "");
    BCM_GPORT_LOCAL_SET(voq_connector_config.port, local_port);
    voq_connector_config.flags = BCM_COSQ_VOQ_CONNECTOR_GPORT_F_VIRTUAL | BCM_COSQ_VOQ_CONNECTOR_GPORT_F_LOW_RATE;
    voq_connector_config.numq = NUM_COS;
    voq_connector_config.remote_modid = MY_MODID;
    voq_connector_config.nof_remote_cores = nof_remote_cores;
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_voq_connector_gport_add(unit, voq_connector_config, &virtual_flow_gport), "");


    /* Set virtual flow to lowest possible rate */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_set(unit, virtual_flow_gport, 1, 0, 12500, 0), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_set(unit, virtual_flow_gport, 1, bcmCosqControlBandwidthBurstMax, max_burst), "");
    /* Virtual flow scheduling discipline configuration */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_sched_set(unit, virtual_flow_gport, 0, SE_SCHEDULER_MODE, 0), "");

    /* Attach virtual flow to first subelement of the shared shaper
     * Here we actually attach the second virtual flow in the bundle
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_attach(unit, gport_parent_se, virtual_flow_gport, 1), "");
    return BCM_E_NONE;
}
/**
 * }
 */
/**
 * Function to create a E2E scheme with quad shared shaper and a virtual flow on a given port.
 * Parameters:
 * unit [INPUT] - unit
 * local_port [INPUT] - local port on which the E2E scheme will be applied
 */
int e2e_virtual_flow_setup(int unit, int local_port)
{
    int cos, se_index;
    uint32 flags, dummy_flags, shaper_kbps_max;
    bcm_port_t port, sysport;
    bcm_gport_t sys_gport;
    bcm_cosq_gport_info_t gport_info;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_cosq_voq_connector_gport_t voq_connector_config;
    bcm_cosq_gport_connection_t connection;
    bcm_cosq_scheduler_gport_params_t shared_scheduler_params;
    bcm_cosq_ingress_queue_bundle_gport_config_t queue_bundle_configure;
    int delay_tolerance_level, rate_class;
    int nof_elements_in_shared_shaper = 4;
    int base_voq = 4;
    int profile_is_rate_class = *dnxc_data_get(unit, "iqs", "credit", "profile_is_rate_class", NULL);
    /**
     * Get sysport from local port(default sysport set application must be used for this to work)
     */
    BCM_IF_ERROR_RETURN_MSG(appl_dnx_logical_port_to_sysport_get(unit, MY_MODID, local_port, &sysport), "");

    BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport, sysport);
    BCM_COSQ_GPORT_E2E_PORT_SET(e2e_gport, local_port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, local_port, &dummy_flags, &interface_info, &mapping_info), "");

    printf("\n Start Setup on local port %d \n", local_port );

    printf("\n Create Quad Shared Shaper (Scheduler level 1) \n");
    /*Create Shared Shaper*/
    shared_scheduler_params.core = mapping_info.core;
    shared_scheduler_params.type = bcmCosqSchedulerGportTypeClassWfq2Sp;
    shared_scheduler_params.cl_wfq_mode = bcmCosqSchedulerGportClWfqModeIndependent;
    shared_scheduler_params.mode = bcmCosqSchedulerGportModeSharedQuad;
    shared_scheduler_params.shared_order = bcmCosqSchedulerGportSharedOrderNone;
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_scheduler_gport_add(unit, 0, shared_scheduler_params, &gport_shared_shaper), "");

    BCM_IF_ERROR_RETURN_MSG(attach_shared_shaper_to_hr(unit, nof_elements_in_shared_shaper, gport_shared_shaper, e2e_gport), "");

    printf("\n Create VOQ connector");
    /*
      * create voq connector bundle, 4 elements in bundle
      */

    BCM_GPORT_LOCAL_SET(voq_connector_config.port, local_port);
    voq_connector_config.numq = NUM_COS;
    voq_connector_config.remote_modid = MY_MODID + mapping_info.core;
    voq_connector_config.nof_remote_cores = 2;
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_voq_connector_gport_add(unit, &voq_connector_config, &gport_voq_connector), "");

    BCM_IF_ERROR_RETURN_MSG(attach_voq_connector_to_shared_shaper(unit, nof_elements_in_shared_shaper, gport_voq_connector, gport_shared_shaper), "");

    printf("Create VOQ \n");
    /*
    * create queue group bundle, 4 elements in bundle
    * Note: can be created without id
    */

    queue_bundle_configure.port = sys_gport;
    queue_bundle_configure.flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP|BCM_COSQ_GPORT_WITH_ID;
    queue_bundle_configure.numq = NUM_COS;
    queue_bundle_configure.local_core_id = BCM_CORE_ALL;
    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(gport_ucast_voq, BCM_CORE_ALL, base_voq);
    /*
    * Get default credit request and compensation profiles based on port SPEED
    */
    BCM_IF_ERROR_RETURN_MSG(appl_dnx_e2e_voq_profiles_get(unit, IN_PORT_SPEED_MBPS, &delay_tolerance_level, &rate_class), "");
    for (cos = 0; cos < NUM_COS; cos++) {
        if (profile_is_rate_class)
        {
            queue_bundle_configure.queue_atrributes[cos].delay_tolerance_level = -1;
        }
        else
        {
            queue_bundle_configure.queue_atrributes[cos].delay_tolerance_level = delay_tolerance_level;
        }
        queue_bundle_configure.queue_atrributes[cos].rate_class = rate_class;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_queue_bundle_gport_add(unit, &queue_bundle_configure, &gport_ucast_voq), "");

    printf("Connect VOQ to VOQ connector \n");
    /*
    * connect VoQ to flow
    */
    printf("Connect Ingress \n");
    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
    connection.remote_modid = MY_MODID + mapping_info.core;
    connection.voq = gport_ucast_voq;
    connection.voq_connector = gport_voq_connector;

    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_connection_set(unit, &connection), "");

    /*
    * connect flow to VoQ
    */
    printf("Connect Egress \n");
    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    connection.remote_modid = MY_MODID;
    connection.voq = gport_ucast_voq;
    connection.voq_connector = gport_voq_connector;

    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_connection_set(unit, &connection), "");

     printf("\n **********  Configure shapers ********** \n");
     BCM_IF_ERROR_RETURN_MSG(configure_all_shapers(unit, nof_elements_in_shared_shaper, CIR_RATE_KBITS, EIR_RATE_KBITS, e2e_gport, gport_shared_shaper, gport_voq_connector), "");

     printf("Create and attach virtual flow\n");
     /** Attach to the shared shaper first subelement */
     gport_info.in_gport = gport_shared_shaper;
     gport_info.cosq = 0;
     BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSchedSharedShaper0, &gport_info), "");

     BCM_IF_ERROR_RETURN_MSG(create_and_attach_virtual_flow(unit, local_port, gport_info.out_gport), "");

     printf(" \n e2e_virtual_flow_setup: PASS \n");
     return BCM_E_NONE;
}
