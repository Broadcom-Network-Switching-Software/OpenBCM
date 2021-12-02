/*~~~~~~~~~~~~~~~~~~~~~~~~~~E2E: Virtual flow Configuration~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
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
    int rv = BCM_E_NONE;
    int se_index;
    bcm_cosq_gport_info_t gport_info;

    gport_info.in_gport = gport_shared_shaper;
    gport_info.cosq = 0;
    for (se_index = 0; se_index < nof_elements_in_shared_shaper; se_index++) {
        printf("\n Connect shared shaper 0x%x element %d to e2e gport 0x%x ", gport_shared_shaper, se_index, e2e_gport);

        /**
         * Get relevant element from the shared shaper
         */
        rv = bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSchedSharedShaper0 + se_index, &gport_info);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_handle_get failed $rv\n");
            return rv;
        }

        rv = bcm_cosq_gport_sched_set(unit, gport_info.out_gport, 0, SHARED_SHAPER_MODE, 0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_sched_set failed $rv\n");
            return rv;
        }


        /**
         *
         */
        rv = bcm_cosq_gport_attach(unit, e2e_gport, gport_info.out_gport, -1);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_attach failed $rv\n");
            return(rv);
        }
    }

    return BCM_E_NONE;
}

/**
 * Attach all each voq connector from the bundle to an element from the shaper
 */
int attach_voq_connector_to_shared_shaper(int unit, int nof_elements_in_shared_shaper, bcm_gport_t gport_voq_connector, bcm_gport_t gport_shared_shaper)
{
    int rv = BCM_E_NONE;
    int cos;
    bcm_cosq_gport_info_t gport_info;

    gport_info.in_gport = gport_shared_shaper;
    gport_info.cosq = 0;
    for (cos = 0; cos < nof_elements_in_shared_shaper; cos++) {
        printf("\n Attach VOQ connector 0x%x cos %d to the Shared shaper 0x%x element %d \n", gport_voq_connector, cos, gport_shared_shaper, cos);
        /* COSx from the VOQ connector scheduling discipline setup */
        rv = bcm_cosq_gport_sched_set(unit, gport_voq_connector, cos, BCM_COSQ_SP0, 0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_sched_set failed $rv\n");
            return rv;
        }

        /**
         * Get relevant parent element from the shared shaper
         */
        rv = bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSchedSharedShaper0 + cos, &gport_info);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_handle_get failed $rv\n");
            return rv;
        }

        /* COS X from the connector attach to parent Shared shaper element X */
        printf(" Attach VOQ connector 0x%x cos %d to the Shared shaper 0x%x element %d \n", gport_voq_connector, cos, gport_shared_shaper, cos );
        rv = bcm_cosq_gport_attach(unit, gport_info.out_gport, gport_voq_connector, cos);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_attach failed $rv\n");
            return(rv);
        }
    }

    return BCM_E_NONE;
}

/**
 * Function to configure the shapers from the E2E scheme
 * All shapers are configured to 10G instead of the elements in the shared shapers.
 * The first element of the shaper is configured with the CIR rate and the others with the EIR rate
 */
int configure_all_shapers(int unit, int nof_elements_in_shared_shaper, uint32 cir_rate, uint32 eir_rate, bcm_gport_t e2e_gport, bcm_gport_t gport_shared_shaper, bcm_gport_t gport_voq_connector) {
    int rv = BCM_E_NONE;
    int se_index, cos;
    bcm_gport_t e2e_parent_gport;
    bcm_cosq_gport_info_t gport_info;
    uint32 shaper_kbps_max;
    uint32 flags = 0;

    /*
     * Rate - Interface
     */
    rv = bcm_fabric_port_get(unit, e2e_gport, 0, &e2e_parent_gport);
    if (rv != BCM_E_NONE) {
        printf("bcm_fabric_port_get failed $rv\n");
        return(rv);
    }
    printf("Set interface 0x%x to %d Kbps \n", e2e_parent_gport, E2E_INTERFACE_KBITS_SEC_MAX);
    rv = bcm_cosq_gport_bandwidth_set(unit, e2e_parent_gport, 0, 0, E2E_INTERFACE_KBITS_SEC_MAX, 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set if failed $rv\n");
        return(rv);
    }
    /*
     * Rate - E2E port
     */
    printf("Set E2E gport rate 0x%x to %d Kbps \n ", e2e_gport, E2E_PORT_KBITS_SEC_MAX);
    rv = bcm_cosq_gport_bandwidth_set(unit, e2e_gport, 0, 0, E2E_PORT_KBITS_SEC_MAX, 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set failed $rv\n");
        return(rv);
    }

    /*
     * Rates - shared shaper
     */
    gport_info.in_gport = gport_shared_shaper;
    gport_info.cosq = 0;
    for (se_index = 0; se_index < nof_elements_in_shared_shaper; se_index++) {
        /**
         * Get relevant element from the shared shaper
         */
        rv = bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSchedSharedShaper0 + se_index, &gport_info);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_handle_get failed $rv\n");
            return rv;
        }

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
        rv = bcm_cosq_gport_bandwidth_set(unit, gport_info.out_gport, 0, 0, shaper_kbps_max, flags);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_sched_set sched failed $rv\n");
            return(rv);
        }

        rv = bcm_cosq_control_set(unit, gport_info.out_gport, 0, bcmCosqControlBandwidthBurstMax, SCHED_MAX_BURST);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_control_set max burst sched failed $rv\n");
            return(rv);
        }
    }

    /*
     * Rate - COS 0/nof_elements_in_shared_shaper
     */
    for (cos = 0; cos < nof_elements_in_shared_shaper; cos++) {
        /* Set rate on the Connector flow */
        printf("Set VOQ Connector gport 0x%x (cos %d) to %d Kbps and MAX burst of %u \n ", gport_voq_connector, cos, VOQ_CONNECTOR_KBITS_SEC_MAX, SCHED_MAX_BURST);

        rv = bcm_cosq_gport_bandwidth_set(unit, gport_voq_connector, cos, 0, VOQ_CONNECTOR_KBITS_SEC_MAX, 0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_bandwidth_set connector failed $rv\n");
            return(rv);
        }

        /* Set maxburst on the Connector flow */
        rv = bcm_cosq_control_set(unit, gport_voq_connector, cos, bcmCosqControlBandwidthBurstMax, SCHED_MAX_BURST);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_control_set connector maxburst failed $rv\n");
            return(rv);
        }
    }

    return BCM_E_NONE;
}

/**
 * Function to create a virtual flow bundle, configure it to the lowest possible rate
 * and attach it to the first subelement of the shared shaper
 */
int create_and_attach_virtual_flow(int unit, int local_port, bcm_gport_t gport_parent_se)
{
    int rv = BCM_E_NONE;
    bcm_cosq_voq_connector_gport_t voq_connector_config;

    BCM_GPORT_LOCAL_SET(voq_connector_config.port, local_port);
    voq_connector_config.flags = BCM_COSQ_VOQ_CONNECTOR_GPORT_F_VIRTUAL;
    voq_connector_config.numq = NUM_COS;
    voq_connector_config.remote_modid = -1;
    voq_connector_config.nof_remote_cores = 1;
    rv = bcm_cosq_voq_connector_gport_add(unit, voq_connector_config, &virtual_flow_gport);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add connector failed $rv\n");
        return rv;
    }


    /* Set virtual flow to lowest possible rate */
    rv = bcm_cosq_gport_bandwidth_set(unit, virtual_flow_gport, 0, 0, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set connector failed $rv\n");
        return(rv);
    }

    /* Virtual flow scheduling discipline configuration */
    rv = bcm_cosq_gport_sched_set(unit, virtual_flow_gport, 0, SE_SCHEDULER_MODE, 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return(rv);
    }

    /* Attach virtual flow to first subelement of the shared shaper
     * Here we actually attach the second virtual flow in the bundle
     */
    rv = bcm_cosq_gport_attach(unit, gport_parent_se, virtual_flow_gport, 1);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return(rv);
    }
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
    int rv = BCM_E_NONE;
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
    rv = appl_dnx_logical_port_to_sysport_get(unit, MY_MODID, local_port, &sysport);
    if (BCM_FAILURE(rv)) {
        printf("appl_dnx_logical_port_to_sysport_get failed. Error:%d (%s) \n",
                 rv, bcm_errmsg(rv));
       return rv;
    }

    BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport, sysport);
    BCM_COSQ_GPORT_E2E_PORT_SET(e2e_gport, local_port);
    rv = bcm_port_get(unit, local_port, &dummy_flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_get scheduler failed $rv\n");
        return rv;
    }

    printf("\n Start Setup on local port %d \n", local_port );

    printf("\n Create Quad Shared Shaper (Scheduler level 1) \n");
    /*Create Shared Shaper*/
    shared_scheduler_params.core = mapping_info.core;
    shared_scheduler_params.type = bcmCosqSchedulerGportTypeClassWfq2Sp;
    shared_scheduler_params.cl_wfq_mode = bcmCosqSchedulerGportClWfqModeIndependent;
    shared_scheduler_params.mode = bcmCosqSchedulerGportModeSharedQuad;
    shared_scheduler_params.shared_order = bcmCosqSchedulerGportSharedOrderNone;
    rv = bcm_cosq_scheduler_gport_add(unit, 0, shared_scheduler_params, &gport_shared_shaper);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_scheduler_gport_add failed $rv\n");
        return rv;
    }

    rv = attach_shared_shaper_to_hr(unit, nof_elements_in_shared_shaper, gport_shared_shaper, e2e_gport);
    if (rv != BCM_E_NONE) {
        printf("attach_shared_shaper_to_hr failed $rv\n");
        return rv;
    }

    printf("\n Create VOQ connector");
    /*
      * create voq connector bundle, 4 elements in bundle
      */

    BCM_GPORT_LOCAL_SET(voq_connector_config.port, local_port);
    voq_connector_config.numq = NUM_COS;
    voq_connector_config.remote_modid = MY_MODID + mapping_info.core;
    voq_connector_config.nof_remote_cores = 2;
    rv = bcm_cosq_voq_connector_gport_add(unit, &voq_connector_config, &gport_voq_connector);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add connector failed $rv\n");
        return rv;
    }

    rv = attach_voq_connector_to_shared_shaper(unit, nof_elements_in_shared_shaper, gport_voq_connector, gport_shared_shaper);
    if (rv != BCM_E_NONE) {
        printf("attach_voq_connector_to_shared_shaper failed $rv\n");
        return(rv);
    }

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
    rv = appl_dnx_e2e_voq_profiles_get(unit, IN_PORT_SPEED_MBPS, &delay_tolerance_level, &rate_class);
    if (rv != BCM_E_NONE) {
        printf("appl_dnx_e2e_voq_profiles_get failed $rv\n");
        return(rv);
    }
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

    rv = bcm_cosq_ingress_queue_bundle_gport_add(unit, &queue_bundle_configure, &gport_ucast_voq);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_ingress_queue_bundle_gport_add failed $rv\n");
        return(rv);
    }

    printf("Connect VOQ to VOQ connector \n");
    /*
    * connect VoQ to flow
    */
    printf("Connect Ingress \n");
    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
    connection.remote_modid = MY_MODID + mapping_info.core;
    connection.voq = gport_ucast_voq;
    connection.voq_connector = gport_voq_connector;

    rv = bcm_cosq_gport_connection_set(unit, &connection);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_connection_set ingress failed $rv\n");
        return(rv);
    }

    /*
    * connect flow to VoQ
    */
    printf("Connect Egress \n");
    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    connection.remote_modid = MY_MODID;
    connection.voq = gport_ucast_voq;
    connection.voq_connector = gport_voq_connector;

    rv = bcm_cosq_gport_connection_set(unit, &connection);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_connection_set egress failed $rv\n");
        return(rv);
    }

     printf("\n **********  Configure shapers ********** \n");
     rv = configure_all_shapers(unit, nof_elements_in_shared_shaper, CIR_RATE_KBITS, EIR_RATE_KBITS, e2e_gport, gport_shared_shaper, gport_voq_connector);
     if (rv != BCM_E_NONE) {
         printf("configure_all_shapers failed $rv\n");
         return(rv);
     }

     printf("Create and attach virtual flow\n");
     /** Attach to the shared shaper first subelement */
     gport_info.in_gport = gport_shared_shaper;
     gport_info.cosq = 0;
     rv = bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSchedSharedShaper0, &gport_info);
     if (rv != BCM_E_NONE) {
         printf("bcm_cosq_gport_handle_get failed $rv\n");
         return rv;
     }

     rv = create_and_attach_virtual_flow(unit, local_port, gport_info.out_gport);
     if (rv != BCM_E_NONE) {
         printf("create_and_attach_virtual_flow failed $rv\n");
         return(rv);
     }

     printf(" \n e2e_virtual_flow_setup: PASS \n");
     return BCM_E_NONE;
}
