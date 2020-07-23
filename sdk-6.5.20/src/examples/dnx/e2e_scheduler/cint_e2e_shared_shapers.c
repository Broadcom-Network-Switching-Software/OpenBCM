/*~~~~~~~~~~~~~~~~~~~~~~~~~~E2E: Shared Shaper Configuration~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_e2e_shared_shapers.c
 * Purpose:     Example of creating an E2E scheduler scheme with a shared shaper in it
 *
 * Example includes:
 *  o     Creating FQs
 *  o     Attaching FQs to E2E gport
 *  o     Creating Shared Shaper
 *  o     Attaching all shared shaper subelements to a FQ
 *  o     Creating a VOQ connector and attaching it to the shared shaper
 *  o     Configuring the shapers of all the creating schedulers
 *
 * NOTE! It is assumed E2E default application is disable (appl_enable_e2e_scheme=0)
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
 *                  +---------------------+--------------------+
 *               +--v-+                                      +-v--+
 *               | FQ |  ...(NOF_ELEMENTS_IN_SHARED_SHAPER)  | FQ |
 *               +-+--+                                      +--+-+
 *                 |                                            |
 *                 |                                            |
 *                 |                                            |
 *                 |10G                                         |10G
 *                 |                                            |
 *                 |                                            |
 *             +---v--------------------------------------------v---+
 *             |Element| ...(NOF_ELEMENTS_IN_SHARED_SHAPER) |Last   |
 *             |0      |                                    |Element|
 *             |       |            SHARED SHAPER           |       |
 *             +--+----+------------------------------------+----+--+
 *                |                                              |
 *                |                                              |
 *                |CIR RATE                                      |EIR RATE
 *                |                                              |
 *                |                                              |
 *             +  v  |                                        +  v  |
 *             |     |  ...(NOF_ELEMENTS_IN_SHARED_SHAPER)    |     |
 *             |COS0 |                                        |COS  |
 *             |     |                                        |LAST |
 *             +-----+                                        +-----+
 */


/**
 * GENERAL VARIABLES
 */
int MY_MODID = 0;
int MAX_NOF_ELEMENTS_IN_LEVEL = 8;
int NUM_COS = 8;
int FQ_SCHEDULER_MODE = BCM_COSQ_SP0;
int SHARED_SHAPER_MODE = BCM_COSQ_SP0;
int SCHED_MAX_BURST = 12000;
int IN_PORT_SPEED_MBPS = 10000;
/**
 * RATES
 */
int E2E_INTERFACE_KBITS_SEC_MAX     = 10000000; /* 10 Gbps */
int E2E_PORT_KBITS_SEC_MAX          = 10000000; /* 10 Gbps */
int SCHED_KBITS_SEC_MAX             = 10000000; /* 10 Gbps */
int VOQ_CONNECTOR_KBITS_SEC_MAX     = 10000000; /* 10 Gbps */

/**
 * GLOBALS
 * Note! These variables is important to stay global in order to utilize
 * e2e_shared_shaper_setup_teardown
 */
bcm_gport_t e2e_gport;
bcm_gport_t gport_ucast_scheduler_fq_arr[MAX_NOF_ELEMENTS_IN_LEVEL];
bcm_gport_t gport_shared_shaper;
bcm_gport_t gport_voq_connector;
bcm_gport_t gport_ucast_voq;

/**
 * Helper functions
 * {
 */
/**
 * Create as much FQ scheduler elements as nof_elements_in_shared_shaper
 */
int create_fqs(int unit, int core, int nof_elements_in_shared_shaper, bcm_gport_t * gport_ucast_scheduler_fq_arr)
{
    int rv = BCM_E_NONE;
    int se_index;
    bcm_cosq_scheduler_gport_params_t fq_params;

    fq_params.core = core;
    fq_params.type = bcmCosqSchedulerGportTypeFq;
    fq_params.cl_wfq_mode = bcmCosqSchedulerGportClWfqModeNone;
    fq_params.mode = bcmCosqSchedulerGportModeSimple;

    for (se_index = 0; se_index < nof_elements_in_shared_shaper; se_index++) {
        rv = bcm_cosq_scheduler_gport_add(unit, 0, fq_params, &gport_ucast_scheduler_fq_arr[se_index]);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_scheduler_gport_add failed $rv\n");
            return rv;
        }
    }

    return BCM_E_NONE;
}
/**
 * Attach all FQ elements to the first HR of the E2E port
 */
int attach_fqs_to_e2e_gport (int unit, int nof_elements_in_shared_shaper, bcm_gport_t * gport_ucast_scheduler_fq_arr, bcm_gport_t e2e_gport)
{
    int rv = BCM_E_NONE;
    int se_index;

    for (se_index = 0; se_index < nof_elements_in_shared_shaper; se_index++) {
        /* FQ scheduler scheduling discipline configuration */
        rv = bcm_cosq_gport_sched_set(unit, gport_ucast_scheduler_fq_arr[se_index], 0, FQ_SCHEDULER_MODE, 0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_sched_set failed $rv\n");
            return(rv);
        }

        printf("Attach FQ gport 0x%x to E2E gport 0x%x \n", gport_ucast_scheduler_fq_arr[se_index], e2e_gport );
        /* Attach FQ to first HR of the Port scheduler */
        rv = bcm_cosq_gport_attach(unit, e2e_gport, gport_ucast_scheduler_fq_arr[se_index], 0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_attach failed $rv\n");
            return(rv);
        }
    }

    return BCM_E_NONE;
}

/**
 * Attach each element in the shared shaper to a FQ
 */
int attach_shared_shaper_to_fqs(int unit, int nof_elements_in_shared_shaper, bcm_gport_t gport_shared_shaper, bcm_gport_t * gport_ucast_scheduler_fq_arr)
{
    int rv = BCM_E_NONE;
    int se_index;
    bcm_cosq_gport_info_t gport_info;

    gport_info.in_gport = gport_shared_shaper;
    gport_info.cosq = 0;
    for (se_index = 0; se_index < nof_elements_in_shared_shaper; se_index++) {
        printf("\n Connect shared shaper 0x%x element %d to FQ 0x%x ", gport_shared_shaper, se_index, gport_ucast_scheduler_fq_arr[se_index]);

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
 */
        rv = bcm_cosq_gport_attach(unit, gport_ucast_scheduler_fq_arr[se_index], gport_info.out_gport, 0);
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
int attach_voq_connectorto_to_shared_shaper(int unit, int nof_elements_in_shared_shaper, bcm_gport_t gport_voq_connector, bcm_gport_t gport_shared_shaper)
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

        rv = bcm_cosq_control_set(unit, gport_voq_connector, cos, bcmCosqControlFlowSlowRate, 1);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_control_set scheduler failed $rv\n");
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
int configure_shapers(int unit,int nof_elements_in_shared_shaper, uint32 cir_rate, uint32 eir_rate, bcm_gport_t e2e_gport, bcm_gport_t * gport_ucast_scheduler_fq_arr, bcm_gport_t gport_shared_shaper, bcm_gport_t gport_voq_connector) {
    int rv = BCM_E_NONE;
    int se_index, cos;
    bcm_gport_t e2e_parent_gport;
    bcm_cosq_gport_info_t gport_info;
    uint32 shaper_kbps_max;

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
     * Rate - FQ schedulers
     */
    for (se_index = 0; se_index < nof_elements_in_shared_shaper; se_index++) {
        printf("Set FQ gport 0x%x to %d Kbps and MAX burst of %u \n ", gport_ucast_scheduler_fq_arr[se_index], SCHED_KBITS_SEC_MAX, SCHED_MAX_BURST);
        rv = bcm_cosq_gport_bandwidth_set(unit, gport_ucast_scheduler_fq_arr[se_index], 0, 0, SCHED_KBITS_SEC_MAX, 0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_sched_set sched failed $rv\n");
            return(rv);
        }

        rv = bcm_cosq_control_set(unit, gport_ucast_scheduler_fq_arr[se_index], 0, bcmCosqControlBandwidthBurstMax, SCHED_MAX_BURST);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_control_set max burst sched failed $rv\n");
            return(rv);
        }
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
        }
        printf("Set Shared shaper gport 0x%x (element %d) to %d Kbps and MAX burst of %u \n ", gport_info.out_gport, se_index, shaper_kbps_max, SCHED_MAX_BURST);
        rv = bcm_cosq_gport_bandwidth_set(unit, gport_info.out_gport, 0, 0, shaper_kbps_max, 0);
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
 * }
 */

/**
 * Function to create a E2E scheme with shared shaper on a given port.
 * Parameters:
 * unit [INPUT] - unit
 * local_port [INPUT] - local port on which the E2E scheme will be applied
 * nof_elements_in_shared_shaper [INPUT] - Nof of elements in the shared shaper. Valid values are 2, 4 and 8
 * cir_rate_kbps [INPUT] - Cir rate for COS0 traffic and first elements  the shared shaper. If Input with COS0 is smaller than this number the free credits will be passed to the other elements in the shared shaper
 * eir_rate_kbps [INPUT] - Eir rate for other than COS0 traffic.
 */
int e2e_shared_shaper_setup(int unit, int local_port, int nof_elements_in_shared_shaper, uint32 cir_rate_kbps, uint32 eir_rate_kbps)
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
    printf("\n Create parent FQs and attach each to PORT's HR0 (Scheduler level 2)\n");
    /*
     * create several FQs (Level2) and attach them to the first Port HR of the E2E port.
     * The NOF of FQs is equal of the number of elements in the shared shaper
     */
    rv = create_fqs(unit, mapping_info.core, nof_elements_in_shared_shaper, gport_ucast_scheduler_fq_arr);
    if (rv != BCM_E_NONE) {
        printf("create_fqs failed $rv\n");
        return rv;
    }

    rv = attach_fqs_to_e2e_gport (unit, nof_elements_in_shared_shaper, gport_ucast_scheduler_fq_arr, e2e_gport);
    if (rv != BCM_E_NONE) {
        printf("attach_fqs_to_e2e_gport failed $rv\n");
        return rv;
    }

    printf("\n Create Shared Shaper (Scheduler level 1) \n");
    /*Create Shared Shaper*/
    shared_scheduler_params.core = mapping_info.core;
    shared_scheduler_params.type = bcmCosqSchedulerGportTypeClassWfq2Sp;
    shared_scheduler_params.cl_wfq_mode = bcmCosqSchedulerGportClWfqModeIndependent;
    switch (nof_elements_in_shared_shaper) {
        case 2:
            shared_scheduler_params.mode = bcmCosqSchedulerGportModeSharedDual;
            printf(" Add Dual Shaper \n");
            break;
        case 4:
            shared_scheduler_params.mode = bcmCosqSchedulerGportModeSharedQuad;
            printf(" Add Quad Shaper \n");
            break;
        case 8:
            shared_scheduler_params.mode = bcmCosqSchedulerGportModeSharedOcta;
            printf(" Add Octa Shaper \n");
            break;
        default:
            printf("Invalid NOF of elements for shared shaper provided : %d.! Valid options are 2, 4 and 8\n", nof_elements_in_shared_shaper);
            return  BCM_E_PARAM;
    }

    rv = bcm_cosq_scheduler_gport_add(unit, 0, shared_scheduler_params, &gport_shared_shaper);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_scheduler_gport_add failed $rv\n");
        return rv;
    }

    rv = attach_shared_shaper_to_fqs(unit, nof_elements_in_shared_shaper, gport_shared_shaper, gport_ucast_scheduler_fq_arr);
    if (rv != BCM_E_NONE) {
        printf("attach_shared_shaper_to_fqs failed $rv\n");
        return rv;
    }

    printf("\n Create VOQ connector");
    /*
      * create voq connector bundle, 8 elements in bundle
      */

    BCM_GPORT_LOCAL_SET(voq_connector_config.port, local_port);
    voq_connector_config.flags = BCM_COSQ_GPORT_VOQ_CONNECTOR;
    voq_connector_config.numq = NUM_COS;
    voq_connector_config.remote_modid = MY_MODID + mapping_info.core;
    voq_connector_config.nof_remote_cores = 2;
    rv = bcm_cosq_voq_connector_gport_add(unit, &voq_connector_config, &gport_voq_connector);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add connector failed $rv\n");
        return rv;
    }

    rv = attach_voq_connectorto_to_shared_shaper(unit, nof_elements_in_shared_shaper, gport_voq_connector, gport_shared_shaper);
    if (rv != BCM_E_NONE) {
        printf("attach_voq_connectorto_to_shared_shaper failed $rv\n");
        return(rv);
    }

    printf("Create VOQ \n");
    /*
    * create queue group bundle, 4 elements in bundle
    */

    queue_bundle_configure.port = sys_gport;
    queue_bundle_configure.flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP;
    queue_bundle_configure.numq = NUM_COS;
    queue_bundle_configure.local_core_id = BCM_CORE_ALL;

    /*
    * Get default credit request and compensation profiles based on port SPEED
    */
    rv = appl_dnx_e2e_voq_profiles_get(unit, IN_PORT_SPEED_MBPS, &delay_tolerance_level, &rate_class);
    if (rv != BCM_E_NONE) {
        printf("appl_dnx_e2e_voq_profiles_get failed $rv\n");
        return(rv);
    }
    for (cos = 0; cos < NUM_COS; cos++) {
        queue_bundle_configure.queue_atrributes[cos].delay_tolerance_level = delay_tolerance_level;
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
     rv = configure_shapers(unit, nof_elements_in_shared_shaper, cir_rate_kbps, eir_rate_kbps,  e2e_gport, gport_ucast_scheduler_fq_arr, gport_shared_shaper, gport_voq_connector);
     if (rv != BCM_E_NONE) {
         printf("configure_shapers failed $rv\n");
         return(rv);
     }

     printf(" \n e2e_shared_shaper_setup: PASS \n");
     return BCM_E_NONE;
}

/**
 * Cint to teardown and delete the scheme created in e2e_shared_shaper_setup.
 * The function is useful for creating a scheme with other shared shaper on same in port.
 * NOTE! This procedure works with the global variables declared in the beggining of the file.
 * The function is meant to work only if used right after the e2e_shared_shaper_setup set and no cint_reset() in between
 */
int e2e_shared_shaper_setup_teardown(int unit, int nof_elements_in_shared_shaper)
{

    int rv, se_index;
    bcm_cosq_gport_connection_t connection;
    bcm_cosq_gport_info_t gport_info;

    for (se_index = 0; se_index < nof_elements_in_shared_shaper; se_index++) {
        printf(" \n Detach E2E gport 0x%x and FQ 0x%x", e2e_gport, gport_ucast_scheduler_fq_arr[se_index]);

        rv = bcm_cosq_gport_detach(unit, e2e_gport,  gport_ucast_scheduler_fq_arr[se_index] , -1);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_detach failed $rv\n");
            return(rv);
        }
    }

    gport_info.in_gport = gport_shared_shaper;
    gport_info.cosq = 0;

    for (se_index = 0; se_index < nof_elements_in_shared_shaper; se_index++) {
        printf(" \n  Detach shared shaper 0x%x index %d and FQ 0x%x", gport_shared_shaper, se_index, gport_ucast_scheduler_fq_arr[se_index]);
        /**
         * Get relevant parent element from the shared shaper
         */
        rv = bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSchedSharedShaper0 + se_index, &gport_info);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_handle_get failed $rv\n");
            return rv;
        }

        rv = bcm_cosq_gport_detach(unit, gport_ucast_scheduler_fq_arr[se_index], gport_info.out_gport , -1);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_detach failed $rv\n");
            return(rv);
        }
    }

    for (se_index = 0; se_index < nof_elements_in_shared_shaper; se_index++) {
        rv = bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSchedSharedShaper0 + se_index, &gport_info);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_handle_get failed $rv\n");
            return rv;
        }

        printf(" \n  Detach shared shaper and VOQ connector 0x%x", gport_voq_connector);
        rv = bcm_cosq_gport_detach(unit, gport_info.out_gport , gport_voq_connector, se_index);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_detach failed $rv\n");
            return(rv);
        }
    }

    printf(" \n Disconnected Ingress connection ");
    /* dis-associate queue from connectors */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
    connection.remote_modid = MY_MODID;
    connection.voq = gport_ucast_voq;
    connection.voq_connector = gport_voq_connector;
    rv = bcm_cosq_gport_connection_set(unit, &connection);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_connection_set  failed $rv\n");
        return(rv);
    }

    printf("\n Disconnected Egress connection");
    /* dis-associate connectors from queues */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
    connection.remote_modid = MY_MODID;
    connection.voq = gport_ucast_voq;
    connection.voq_connector = gport_voq_connector;

    rv = bcm_cosq_gport_connection_set(unit, &connection);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_connection_set  failed $rv\n");
        return(rv);
    }

    for (se_index = 0; se_index < nof_elements_in_shared_shaper; se_index++) {
        printf("\n  Delete FQs");
        rv = bcm_cosq_gport_delete(unit, gport_ucast_scheduler_fq_arr[se_index]);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_delete  failed $rv\n");
            return(rv);
        }
    }

    printf("\n  Delete Shared shaper");
    rv = bcm_cosq_gport_delete(unit, gport_shared_shaper);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_delete  failed $rv\n");
        return(rv);
    }

    printf("\n  Delete VOQ connector");
    rv = bcm_cosq_gport_delete(unit, gport_voq_connector);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_delete  failed $rv\n");
        return(rv);
    }

    printf("\n  Delete VOQ");
    rv = bcm_cosq_gport_delete(unit, gport_ucast_voq);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_delete  failed $rv\n");
        return(rv);
    }
    printf("\n e2e_shared_shaper_setup_teardown: PASS \n");

    return BCM_E_NONE;
}



